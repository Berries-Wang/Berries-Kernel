> 📌 迁移自 `Berries-CYBERCLOAK/000.CODE_SPACE/003.JA4T_PLUGIN/docs/EBPF_LOAD_EXEC.md`(2026-10-24);导读见 [000.导读.md](../000.导读.md)。
> ⚠️ 文中内核源码行号(如 cls_bpf.c:81-136、filter.h:905)基于 **Linux 5.9 树**;仓库 7.1.3 树的对照行号见 [../../011.TC-SCHED_CLS与SCHED_ACT.md](../../011.TC-SCHED_CLS与SCHED_ACT.md) 与 [../../000.从bpf系统调用来入门EBPF.md](../../000.从bpf系统调用来入门EBPF.md)。

# eBPF 加载与执行流程

## 概述

JA4T 守护进程使用两个 eBPF TC 分类器程序采集 TCP 指纹：

| 程序 | 源文件 | 方向 | 用途 |
|------|--------|------|------|
| `ja4t_parse_syn` | `src/ebpf/tcp_options.bpf.c` | Ingress | 解析入站 TCP SYN 包，提取 JA4T 指纹 |
| `ja4s_parse_server_hello` | `src/ebpf/tls_server_hello.bpf.c` | Egress | 捕获出站 TLS Server Hello，提取 JA4S 指纹 |

整个流程分为三个阶段：**编译时 → 加载时 → 运行时**。

---

## 一、编译时：C 源码 → BPF 字节码

### 构建脚本

文件：`build.rs`

```
tcp_options.bpf.c          tls_server_hello.bpf.c
        │                          │
        │  clang -target bpf -O2 -g -Wall -Werror
        │                          │
        ▼                          ▼
 tcp_options.bpf.o         tls_server_hello.bpf.o
 (BPF 字节码)               (BPF 字节码)
```

`build.rs` 在 Cargo 构建时直接调用系统 `clang` 编译器，以 `bpf` 为目标架构编译：

```rust
fn compile_ebpf(src: &PathBuf, obj: &PathBuf) {
    let status = Command::new("clang")
        .args([
            "-target", "bpf",   // 目标：BPF 字节码
            "-O2",              // 优化级别
            "-g",               // 包含调试信息（BTF）
            "-Wall", "-Werror", // 严格检查
            "-I", "src/ebpf",   // 头文件路径
            "-c", src,          // 仅编译
            "-o", obj,          // 输出文件
        ])
        // ...
}
```

关键点：

- **不依赖系统头文件**：`.bpf.c` 文件自包含所有类型定义（`__u8`、`__sk_buff` 等），无需 `<linux/bpf.h>`
- **输出路径注入**：编译后的 `.o` 文件路径通过 Cargo 环境变量注入 Rust 二进制

```rust
println!("cargo:rustc-env=EBPF_OBJECT_PATH={}", syn_obj.display());
println!("cargo:rustc-env=EBPF_SERVER_HELLO_PATH={}", sh_obj.display());
```

- **增量构建**：通过 `cargo:rerun-if-changed` 指令，仅当源文件变化时重新编译 eBPF

---

## 二、加载时：BPF 字节码 → 内核执行

### 2.1 加载入口

文件：`src/main.rs`

```rust
// 加载 JA4T (ingress)
let (reader, _bpf) = load_and_attach(cli.bpf_object.as_ref(), &cli.interface)?;

// 加载 JA4S (egress)
let (sh_reader, _sh_bpf) =
    load_and_attach_server_hello(cli.bpf_server_hello_object.as_ref(), &cli.interface)?;
```

### 2.2 加载器细节

文件：`src/ja4t/loader.rs` / `src/ja4s/loader.rs`

使用 **Aya 库**（`aya = "0.12"`）完成加载和挂载：

```
Rust 进程 (ja4t-daemon)
  │
  ├─ ① BpfLoader::load_file("tcp_options.bpf.o")
  │     读取 .o ELF 文件 → 解析 section → 找到 "classifier" 段
  │
  ├─ ② bpf.program_mut("ja4t_parse_syn")?.try_into()
  │     通过函数名匹配程序，转换为 SchedClassifier 类型
  │
  ├─ ③ prog.load()
  │     → 将 BPF 字节码送入内核 BPF 验证器 (verifier)
  │     → 验证器检查：循环边界、内存访问、栈大小、辅助函数调用等
  │     → 通过后 JIT 编译为 native 机器码，注入内核
  │
  └─ ④ prog.attach(interface, TcAttachType::Ingress)
        → 内核创建 TC 过滤器 (tc filter)
        → 挂载到指定网卡的 ingress 路径
        → 从此每个入站 TCP 包都会经过此 eBPF 程序
```

核心代码：

```rust
pub fn load_and_attach(
    bpf_obj_path: Option<&PathBuf>,
    interface: &str,
) -> Result<(BpfReader, aya::Bpf)> {
    let mut loader = BpfLoader::new();
    let mut bpf = loader.load_file(obj_path)?;

    let prog: &mut SchedClassifier = bpf
        .program_mut("ja4t_parse_syn")?
        .try_into()?;

    prog.load()?;                              // 加载并通过验证器
    prog.attach(interface, TcAttachType::Ingress)?;  // 挂载到 ingress

    let map = bpf.take_map("tcp_options_map")?;   // 取出 Map 句柄
    let debug_map = bpf.take_map("debug_counters")?;

    Ok((BpfReader::new(map, debug_map), bpf))
}
```

### 2.3 关键约束

- **`aya::Bpf` 必须保持存活**：`_bpf` 对象一旦被 drop，内核自动卸载 TC 过滤器。因此 `main.rs` 中将其绑定到 `_bpf` 变量，使其存活到进程退出。
- **Map 所有权转移**：通过 `take_map()` 将 Map 所有权从 `Bpf` 对象转移到 `BpfReader`，之后 `Bpf` 对象仍然可以保持程序存活。

---

## 三、运行时：数据采集与读取

### 3.1 eBPF 内核侧处理

```
每个入站 TCP 包
      │
      ▼
ja4t_parse_syn(struct __sk_buff *skb)
      │
      ├─ ① P0F 风格 IP 头定位
      │    在偏移 0..38 扫描 IPv4 版本号 (兼容多种链路层格式)
      │
      ├─ ② 提取五元组 → 构造 key {src_ip, dst_ip, src_port, dst_port, protocol}
      │
      ├─ ③ 判断包类型：
      │    ├─ 纯 SYN (SYN=1, ACK=0):
      │    │   ├─ 提取 Window Size / IP TTL / TCP Options 原始字节
      │    │   ├─ info.capture_ns = bpf_ktime_get_ns()  // 记录捕获时间
      │    │   └─ bpf_map_update_elem(&tcp_options_map, &key, &info, BPF_ANY)
      │    │
      │    └─ SYN-ACK (SYN=1, ACK=1):
      │        ├─ 交换 src/dst 构建反向 key
      │        ├─ bpf_map_lookup_elem(&tcp_options_map, &rev_key)
      │        └─ existing->synack_ns = bpf_ktime_get_ns()  // 记录 RTT
      │
      └─ ④ 返回 TC_ACT_UNSPEC
           纯观测模式，不修改/丢弃任何包
```

### 3.2 用户态轮询读取

文件：`src/ja4t/poller.rs`

```
tokio 后台任务 (poll_bpf_map)
      │
      ├─ 每 N 秒 (默认 2s) 执行一次
      │
      ├─ reader.dump_all()
      │   → aya::maps::HashMap::iter()
      │   → 遍历 BPF LRU Hash Map 所有条目
      │   → 将内核数据复制到用户态 HashMap
      │
      ├─ 解析 TCP Options (用户态解析，避免 eBPF 限制)
      │   → MSS / Window Scale / SACK / Options Order
      │
      ├─ 计算 RTT = (synack_ns - capture_ns) / 1000 (微秒)
      │
      ├─ 更新内存缓存 cache.write().await.update(entries)
      │
      └─ 输出 JSON 日志 (供下游系统使用)
```

### 3.3 Unix Socket 查询服务

文件：`src/server.rs`

```
nginx 查询请求 (JSON over Unix Socket)
      │
      ├─ ① 快速路径：查内存缓存 (ConnectionCache)
      │   cache.lookup(src_ip, dst_ip, src_port, dst_port)
      │
      ├─ ② 兜底路径：缓存未命中时直接查 BPF Map
      │   bpf_reader.lookup_direct(&key)
      │   → aya::maps::HashMap::get(key, 0)
      │   → 直接调用内核 bpf_map_lookup_elem
      │   → 消除轮询间隔导致的竞态窗口
      │
      ├─ ③ JA4S 查询：反向五元组查 ServerHello 缓存/Map
      │   (server→client 方向，与 SYN 的 client→server 相反)
      │
      └─ ④ 返回 JSON 响应 (含 JA4T + JA4S 指纹)
```

### 3.4 JA4S 指纹计算

JA4S 指纹在用户态计算，而非 eBPF 内核中，因为 SHA256 哈希的指令数远超 eBPF 验证器的限制：

```
eBPF 内核侧                         用户态 Rust 侧
──────────                           ────────────
采集原始数据：                        轮询读取 Map 条目：
  TLS Version                          → 构造 Ja4sData
  Cipher Suite                         → ja4s::compute_ja4s()
  Extension Count                      → SHA256 哈希
  ALPN 协议名                          → 生成 JA4S 指纹字符串
  Extension Raw Types                  → 缓存到 ServerHelloCache
```

---

## 四、数据流全景图

```
┌─────────────────────────────────────────────────────────────────────┐
│                        内核空间 (Kernel)                            │
│                                                                     │
│  ┌─────────────┐    ┌──────────────────────┐    ┌────────────────┐  │
│  │ 网卡 Ingress │───▶│ ja4t_parse_syn       │───▶│ tcp_options_map│  │
│  │ (入站 TCP)   │    │ TC classifier         │    │ LRU Hash       │  │
│  └─────────────┘    └──────────────────────┘    │ 65536 条       │  │
│                                                  └────────────────┘  │
│  ┌─────────────┐    ┌──────────────────────┐    ┌────────────────┐  │
│  │ 网卡 Egress  │───▶│ ja4s_parse_server_   │───▶│tls_server_     │  │
│  │ (出站 TLS)   │    │ hello TC classifier   │    │ hello_map      │  │
│  └─────────────┘    └──────────────────────┘    │ LRU Hash       │  │
│                                                  └────────────────┘  │
└─────────────────────────────────────────────────────────────────────┘
                              │
                    ┌─────────┴──────────┐
                    │ Aya 库 (Map 读取)   │
                    │ dump_all() / get()  │
                    └─────────┬──────────┘
                              │
┌─────────────────────────────────────────────────────────────────────┐
│                       用户空间 (User Space)                         │
│                                                                     │
│  ┌─────────────────┐     ┌──────────────────┐     ┌──────────────┐  │
│  │ poll_bpf_map()  │────▶│ ConnectionCache  │────▶│              │  │
│  │ 轮询任务 (每2s)  │     │ 内存缓存          │     │  Unix Socket  │  │
│  └─────────────────┘     └──────────────────┘     │  服务器       │  │
│                                                     │   (server.rs) │  │
│  ┌─────────────────┐     ┌──────────────────┐     │              │  │
│  │ poll_server_    │────▶│ ServerHelloCache │────▶│              │  │
│  │ hello_map()     │     │ 内存缓存          │     └──────┬───────┘  │
│  └─────────────────┘     └──────────────────┘            │          │
│                                                            │          │
│                                                    ┌───────▼───────┐  │
│                                                    │  nginx 查询    │  │
│                                                    │ (Unix Socket)  │  │
│                                                    └───────────────┘  │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 五、关键技术决策

### 5.1 为什么用 TC classifier 而非 XDP

见 [Q2](#q2-为什么选择-tc-classifier-而非-xdp)

### 5.2 为什么用户态解析 TCP Options

见 [Q3](#q3-为什么-ebpf-只采集原始字节由用户态解析-tcp-options)

### 5.3 为什么用户态计算 JA4S 指纹

见 [Q4](#q4-为什么-ja4s-指纹在用户态计算而非-ebpf-内核中)

### 5.4 两级查询策略

见 [Q5](#q5-为什么需要两级查询内存缓存--bpf-map-直查)

### 5.5 生命周期管理

见 [Q6](#q6-为什么-_bpf-变量必须保持存活)

---

## 六、相关文件

| 文件 | 作用 |
|------|------|
| `src/ebpf/tcp_options.bpf.c` | JA4T eBPF C 源码（ingress） |
| `src/ebpf/tls_server_hello.bpf.c` | JA4S eBPF C 源码（egress） |
| `src/ebpf/tcp_options.h` | 共享结构体定义 |
| `build.rs` | Cargo 构建脚本，编译 eBPF C → BPF 字节码 |
| `src/ja4t/loader.rs` | JA4T eBPF 加载器（加载 + 挂载 ingress） |
| `src/ja4s/loader.rs` | JA4S eBPF 加载器（加载 + 挂载 egress） |
| `src/ja4t/reader.rs` | BPF Map 读取器（dump_all + lookup_direct） |
| `src/ja4s/reader.rs` | JA4S Map 读取器 |
| `src/ja4t/poller.rs` | 轮询任务（定期导出 Map → 更新缓存） |
| `src/ja4s/poller.rs` | JA4S 轮询任务 |
| `src/server.rs` | Unix Socket 查询服务 |
| `src/main.rs` | 主入口，编排所有组件 |

---

## 七、Q&A

### Q1: 为什么 eBPF 程序使用 `SEC("classifier")`（或 `__attribute__((section("classifier")))`）？

这是 **Aya 库与 eBPF 框架之间的约定协议**。Aya 通过 ELF section 名称来推断 BPF 程序类型，并自动选择对应的 Rust 接口类型。

**section 名称 → BPF 程序类型 → Rust 类型** 的映射关系：

| Section 名称 | BPF 程序类型 | Rust 类型（Aya） |
|-------------|-------------|-----------------|
| `"classifier"` | `BPF_PROG_TYPE_SCHED_CLS` | `SchedClassifier` |
| `"xdp"` | `BPF_PROG_TYPE_XDP` | `Xdp` |
| `"kprobe"` | `BPF_PROG_TYPE_KPROBE` | `KProbe` |
| `"tracepoint"` | `BPF_PROG_TYPE_TRACEPOINT` | `TracePoint` |
| `"socket"` | `BPF_PROG_TYPE_SOCKET_FILTER` | `SocketFilter` |

**加载流程中的匹配逻辑**：

```rust
// loader.rs
let prog: &mut SchedClassifier = bpf
    .program_mut("ja4t_parse_syn")?  // 按函数名找到程序
    .try_into()?;                     // ← 检查 section 名，匹配到 "classifier" → SchedClassifier
```

`try_into()` 内部读取 ELF section 名称：
- 匹配到 `"classifier"` → 返回 `SchedClassifier`，后续调用 `prog.attach(iface, TcAttachType::Ingress)` 挂载 TC 过滤器
- 如果 section 名是 `"xdp"` → 返回 `Xdp`，后续调用 `prog.attach(iface)` 挂载 XDP 程序
- 如果是未知名称 → `try_into()` 返回错误

**为什么叫 "classifier" 而非 "tc"**？这是历史原因。Linux TC（Traffic Control）子系统中有多个分类器组件（`cls_u32`、`cls_flow`、`cls_fw`、`cls_bpf` 等），BPF 程序作为 TC 分类器的一种，沿用了 `"classifier"` 这个名称。较新的 libbpf 版本也支持 `"tc"` 作为 section 名，但 Aya 0.12 使用的是传统 `"classifier"` 名称。

---

### Q2: 为什么选择 TC classifier 而非 XDP？

| 维度 | TC classifier ✅ | XDP ❌ |
|------|-----------------|--------|
| 可访问数据结构 | `__sk_buff`（完整网络栈上下文） | `xdp_md`（有限信息，无法直接访问 TCP 头） |
| TCP 选项读取 | 支持，通过 `bpf_skb_load_bytes()` | 不支持，无法读取 TCP 选项 |
| 性能 | 足够（非驱动层，但纯观测模式无瓶颈） | 更高（驱动层，但本项目不需要） |
| 挂载方式 | `tc filter` 挂载到 ingress/egress | `xdp` 程序挂载到驱动层 |
| 本项目需求 | ✅ 需要读取 TCP Option 和 TLS 负载 | ❌ 无法满足需求 |

---

### Q3: 为什么 eBPF 只采集原始字节，由用户态解析 TCP Options？

- **eBPF 验证器限制**：TCP Options 解析涉及复杂的分支和可变长度逻辑，BPF 验证器难以通过安全证明
- **eBPF 栈大小限制**：仅 512 字节，复杂解析容易溢出
- **方案**：eBPF 只采集 `raw_opts[40]`（40 字节原始 TCP Option 字节），用户态 Rust 使用 `parse_tcp_options()` 解析

---

### Q4: 为什么 JA4S 指纹在用户态计算，而非 eBPF 内核中？

- **SHA256 指令数**：远超 eBPF 验证器的指令数上限（4096 条 BPF 指令）
- **方案**：eBPF 采集原始字段（TLS 版本、Cipher Suite、扩展列表、ALPN），用户态通过 `ja4s::compute_ja4s()` 计算 SHA256 哈希，生成 JA4S 指纹字符串

---

### Q5: 为什么需要两级查询（内存缓存 + BPF Map 直查）？

| 路径 | 延迟 | 命中率 | 说明 |
|------|------|--------|------|
| 内存缓存（快速路径） | 纳秒级 | 高 | 轮询任务每 2 秒更新一次，读锁保护 |
| BPF Map 直查（兜底路径） | 微秒级 | 低 | 缓存未命中时直接调用 `bpf_map_lookup_elem`，消除轮询间隔导致的竞态窗口 |

---

### Q6: 为什么 `_bpf` 变量必须保持存活？

```rust
let (reader, _bpf) = load_and_attach(...);
```

`_bpf` 持有 `aya::Bpf` 对象，它管理着内核中 eBPF 程序的生命周期。当 `_bpf` 被 drop 时，drop 实现会通知内核卸载对应的 TC 过滤器。因此：

- `_bpf` 必须存活到进程退出
- 进程退出时，内核自动清理 eBPF 程序
- 不需要手动卸载逻辑

---

### Q7: 内核中对应的代码是什么？内核在哪里加载 eBPF 程序并执行？

#### 7.1 类型定义：什么是 TC classifier

**内核文件：`include/linux/bpf_types.h`**

```c
BPF_PROG_TYPE(BPF_PROG_TYPE_SCHED_CLS, tc_cls_act,
              struct __sk_buff, struct sk_buff)
```

这行宏展开后定义了 BPF 程序类型的核心映射关系：

| 角色 | 类型 | 说明 |
|------|------|------|
| BPF 程序类型枚举 | `BPF_PROG_TYPE_SCHED_CLS` | 内核中唯一标识 TC 分类器 |
| BPF 侧上下文（你在 C 中看到的） | `struct __sk_buff *` | BPF 验证器看到的上下文类型 |
| 内核侧上下文（实际传递的） | `struct sk_buff *` | 内核网络栈的 socket buffer |

验证器在加载时建立 `struct __sk_buff` 字段 ↔ `struct sk_buff` 字段的映射关系，确保你在 eBPF 中访问 `skb->len`、`skb->data` 等字段时，实际上读取的是内核 `sk_buff` 的正确偏移。

其他程序类型的对比：

```c
BPF_PROG_TYPE(BPF_PROG_TYPE_XDP, xdp,
              struct xdp_md, struct xdp_buff)        // XDP 用 xdp_md
BPF_PROG_TYPE(BPF_PROG_TYPE_KPROBE, kprobe,
              bpf_user_pt_regs_t, struct pt_regs)    // kprobe 用 pt_regs
BPF_PROG_TYPE(BPF_PROG_TYPE_SOCKET_FILTER, sk_filter,
              struct __sk_buff, struct sk_buff)      // socket filter
```

---

#### 7.2 TC 分类器获取 BPF 程序

**内核文件：`net/sched/cls_bpf.c` — `cls_bpf_prog_from_efd()`**

```c
static int cls_bpf_prog_from_efd(struct nlattr **tb, struct cls_bpf_prog *prog,
                                  u32 gen_flags, const struct tcf_proto *tp)
{
    struct bpf_prog *fp;
    u32 bpf_fd;

    bpf_fd = nla_get_u32(tb[TCA_BPF_FD]);
    // 从 fd 获取 BPF 程序，并验证类型必须是 SCHED_CLS
    fp = bpf_prog_get_type_dev(bpf_fd, BPF_PROG_TYPE_SCHED_CLS, skip_sw);
    // 存入 cls_bpf_prog 结构体，挂载到 TC 过滤器链
    prog->filter = fp;
}
```

当用户态执行 `prog.attach()` 时，Aya 通过 netlink 发送 `TCA_BPF_FD`（BPF 程序的文件描述符）。内核 `cls_bpf_change()` 调用 `cls_bpf_prog_from_efd()`，通过 `bpf_prog_get_type_dev()` 验证程序类型，然后将其挂载到 TC 过滤器的链表中。

---

#### 7.3 执行入口：每个包到达时调用

**内核文件：`net/sched/cls_bpf.c:81-136` — `cls_bpf_classify()`**

这是**你的 eBPF 程序被调用的地方**——内核网络栈在处理每个包时，到达 TC 挂载点，遍历过滤器链表，找到 `cls_bpf` 过滤器后执行：

```c
TC_INDIRECT_SCOPE int cls_bpf_classify(struct sk_buff *skb,
                                        const struct tcf_proto *tp,
                                        struct tcf_result *res)
{
    struct cls_bpf_head *head = rcu_dereference_bh(tp->root);
    bool at_ingress = skb_at_tc_ingress(skb);
    struct cls_bpf_prog *prog;
    int ret = -1;

    list_for_each_entry_rcu(prog, &head->plist, link) {
        int filter_res;

        if (at_ingress) {
            // Ingress 方向：补充 mac 头（内核已剥离）
            __skb_push(skb, skb->mac_len);
            filter_res = bpf_prog_run_data_pointers(prog->filter, skb);
            //                     ↑ 执行 BPF 程序！
            __skb_pull(skb, skb->mac_len);
        } else {
            // Egress 方向：直接执行
            filter_res = bpf_prog_run_data_pointers(prog->filter, skb);
        }

        // 处理返回值
        ret = cls_bpf_exec_opcode(filter_res);  // 映射到 TC 动作码
        if (ret == TC_ACT_UNSPEC)
            continue;  // UNSPEC 表示继续下一个过滤器
        break;
    }
    return ret;
}
```

**关键点**：
- `at_ingress` 由 `skb_at_tc_ingress(skb)` 判断，对应 Aya 的 `TcAttachType::Ingress` / `Egress`
- `bpf_prog_run_data_pointers()` 是执行 BPF 程序的函数
- 返回值通过 `cls_bpf_exec_opcode()` 映射到 TC 动作码

---

#### 7.4 实际的 BPF 程序执行

**内核文件：`include/linux/filter.h:905-923` — `bpf_prog_run_data_pointers()`**

```c
static inline int bpf_prog_run_data_pointers(
    const struct bpf_prog *prog,
    struct sk_buff *skb)
{
    struct bpf_skb_data_end *cb = (struct bpf_skb_data_end *)skb->cb;

    bpf_compute_data_pointers(skb);  // 设置 data/data_end 指针
    res = bpf_prog_run(prog, skb);   // 执行 BPF 程序

    return res;
}
```

**`include/linux/filter.h:699-725` — `__bpf_prog_run()`**

```c
static __always_inline u32 __bpf_prog_run(const struct bpf_prog *prog,
                                           const void *ctx,
                                           bpf_dispatcher_fn dfunc)
{
    u32 ret;
    // 直接调用 JIT 编译后的函数指针！
    ret = dfunc(ctx, prog->insnsi, prog->bpf_func);
    //    ↑ 如果 JIT 已启用，这里直接调用 native 机器码
    //      如果未启用，走解释器路径
    return ret;
}
```

**关键**：`dfunc` 是 BPF 调度器。如果 BPF 程序通过了验证器并完成了 **JIT 编译**，`prog->bpf_func` 就是一段 **native 机器码**，直接执行，没有额外的解释开销。

---

#### 7.5 返回值映射

**内核文件：`net/sched/cls_bpf.c:66-79` — `cls_bpf_exec_opcode()`**

```c
static int cls_bpf_exec_opcode(int code)
{
    switch (code) {
    case TC_ACT_OK:       // 0
    case TC_ACT_SHOT:     // 2
    case TC_ACT_STOLEN:   // 4
    case TC_ACT_TRAP:     // 8
    case TC_ACT_REDIRECT: // 7
    case TC_ACT_UNSPEC:   // -1 ← 本项目使用
        return code;
    default:
        return TC_ACT_UNSPEC;  // 未知值默认 UNSPEC
    }
}
```

**内核文件：`include/uapi/linux/pkt_cls.h:63-71`** — TC 动作码定义

```c
#define TC_ACT_UNSPEC    (-1)  // 不做处理，继续（本项目使用，纯观测模式）
#define TC_ACT_OK         0   // 接受，通过
#define TC_ACT_RECLASSIFY 1   // 重新分类
#define TC_ACT_SHOT       2   // 丢弃包
#define TC_ACT_PIPE       3   // 继续下一个动作
#define TC_ACT_STOLEN     4   // 没收（已接管 skb 所有权）
#define TC_ACT_QUEUED     5   // 已入队
#define TC_ACT_REPEAT     6   // 重复
#define TC_ACT_REDIRECT   7   // 重定向到其他接口
#define TC_ACT_TRAP       8   // 捕获到 CPU
```

---

#### 7.6 验证器上下文检查

**内核文件：`kernel/bpf/verifier.c:4883`**

```c
case BPF_PROG_TYPE_SCHED_CLS:
    // 验证器允许 SCHED_CLS 直接访问包数据（data/data_end）
    // 并且将 struct __sk_buff 的字段映射到 struct sk_buff
    env->seen_direct_write = true;
    return true;
```

验证器在加载时做三件事：
1. **检查函数签名** — 必须是 `int func(struct __sk_buff *)`
2. **字段映射** — 将 `__sk_buff` 的字段偏移映射到内核 `sk_buff` 的实际位置
3. **辅助函数检查** — 确保 `bpf_skb_load_bytes()` 等调用合法

---

#### 7.7 完整执行链路全景

```
用户态 (Rust / Aya)                             内核态
─────────────────                              ──────

prog.load()
  │  ──bpf() syscall──▶  kernel/bpf/syscall.c
  │                         ├─ 加载 BPF 字节码
  │                         ├─ 验证器 (verifier.c)
  │                         │   ├─ 检查签名：int func(__sk_buff *)
  │                         │   ├─ 检查指令合法性
  │                         │   └─ JIT 编译 → native 机器码
  │                         └─ 返回 fd
  │
prog.attach(iface, Ingress)
  │  ──netlink──▶  net/sched/cls_bpf.c
  │                  ├─ cls_bpf_change()
  │                  │   └─ cls_bpf_prog_from_efd()
  │                  │       └─ bpf_prog_get_type_dev(fd, SCHED_CLS)
  │                  │           ↑ 验证程序类型必须匹配
  │                  └─ 挂载到 TC 过滤器链表
  │
  │               每个包到达网卡时：
  │                     │
  ▼                     ▼
               net/sched/cls_bpf.c:83
               cls_bpf_classify(skb, tp, res)
                      │
                      ├─ 遍历过滤器链表
                      ├─ 找到 cls_bpf 程序
                      ├─ bpf_prog_run_data_pointers(prog->filter, skb)
                      │     │
                      │     ├─ include/linux/filter.h:905
                      │     │  bpf_compute_data_pointers(skb)
                      │     │  bpf_prog_run(prog, skb)
                      │     │       └─ __bpf_prog_run()
                      │     │             └─ dfunc(ctx, insnsi, bpf_func)
                      │     │                    ↑ 直接执行 JIT native 代码
                      │     │
                      │     └─ 返回值 ← 你的 ja4t_parse_syn 的 return
                      │
                      ├─ cls_bpf_exec_opcode(filter_res)
                      │     └─ TC_ACT_UNSPEC(-1) → 继续处理
                      │
                      └─ 返回 TC 动作码 → 内核网络栈继续处理
```

**你的 C 代码与内核代码的对应关系：**

| 你的代码 | 内核对应 |
|----------|---------|
| `struct __sk_buff *skb` | `struct sk_buff *skb`（`bpf_types.h` 建立字段映射） |
| `int` 返回值 | `u32` 被 `cls_bpf_exec_opcode()` 解释为 TC 动作码 |
| `SEC("classifier")` | 告诉 Aya 用 `SchedClassifier` 加载，内核注册为 `BPF_PROG_TYPE_SCHED_CLS` |
| `bpf_skb_load_bytes(skb, offset, buf, len)` | 内核 helper → 从 `skb->data` 读取指定偏移的字节 |

#### 相关内核源文件索引

| 文件 | 作用 |
|------|------|
| `include/linux/bpf_types.h` | 定义 `BPF_PROG_TYPE_SCHED_CLS` 及上下文类型映射 |
| `net/sched/cls_bpf.c` | TC BPF 分类器：加载、挂载、分类执行（`cls_bpf_classify`） |
| `include/linux/filter.h` | `bpf_prog_run()` / `bpf_prog_run_data_pointers()` — 实际执行 BPF 程序 |
| `include/uapi/linux/pkt_cls.h` | TC 动作码定义（`TC_ACT_UNSPEC`、`TC_ACT_OK` 等） |
| `kernel/bpf/verifier.c` | BPF 验证器：检查程序签名、指令合法性、上下文类型 |
| `kernel/bpf/syscall.c` | `bpf()` 系统调用入口 |