> 📌 迁移自 `Berries-CYBERCLOAK/000.CODE_SPACE/003.JA4T_PLUGIN/docs/BPF_SYSCALL_ANALYSIS.md`(2026-10-24);导读见 [000.导读.md](../000.导读.md)。

# BPF 系统调用与挂载机制分析

## 概述

JA4T_PLUGIN 是一个 Rust 守护进程（`ja4t-daemon`），通过 **eBPF** 技术在内核态采集 TCP SYN 指纹（JA4T）和 TLS Server Hello 指纹（JA4S）。该项目**不直接调用 `bpf()` 系统调用**，而是通过 **`aya` 库 (v0.12)** 间接完成所有 BPF 相关操作。

---

## 整体架构

```
┌─────────────────────────────────────────────────────────────┐
│                    用户态 (ja4t-daemon)                      │
│                                                             │
│  build.rs ──clang──▶ .bpf.o (ELF BPF 字节码)                │
│                                                             │
│  loader.rs ──aya──▶ bpf() syscall (BPF_PROG_LOAD)           │
│             ──aya──▶ sendmsg() (RTM_NEWTFILTER, netlink)    │
│                                                             │
│  reader.rs ──aya──▶ bpf() syscall (BPF_MAP_LOOKUP_ELEM)     │
│             ──aya──▶ bpf() syscall (BPF_MAP_GET_NEXT_KEY)    │
│                                                             │
│  poller.rs ──▶ reader.dump_all() (定期轮询 BPF Map)          │
│                                                             │
│  server.rs ──▶ Unix Domain Socket (与 nginx 通信)            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    内核态 (eBPF)                             │
│                                                             │
│  TC Ingress: ja4t_parse_syn  (解析入站 TCP SYN)             │
│  TC Egress:  ja4s_parse_server_hello (解析出站 TLS SH)      │
│                                                             │
│  BPF Maps:                                                  │
│    - tcp_options_map (LRU_HASH, 65536)                      │
│    - tls_server_hello_map (LRU_HASH, 65536)                 │
│    - debug_counters (ARRAY)                                 │
└─────────────────────────────────────────────────────────────┘
```

---

## 逐层分析：系统调用链路

### 1. 构建阶段：`build.rs` — 编译 BPF 字节码

**文件**: `build.rs`

```rust
fn compile_ebpf(src: &PathBuf, obj: &PathBuf) {
    Command::new("clang")
        .args(["-target", "bpf", "-O2", "-g", "-c", src, "-o", obj])
        .status()...
}
```

| 调用 | 系统调用 | 说明 |
|------|---------|------|
| `Command::new("clang")` | `fork()`, `execve()` | 启动 clang 编译进程 |
| `clang -target bpf` | 文件读写 | 编译 `.bpf.c` → `.bpf.o` (ELF 格式 BPF 字节码) |

**不涉及 bpf() 系统调用**，只是常规的编译操作。

---

### 2. 加载 BPF 程序：`loader.rs` — 两个关键步骤

#### 2a. `BpfLoader::load_file(path)` — 解析 ELF 对象文件

```rust
let mut bpf = loader.load_file(path)?;
```

| 操作 | 系统调用 | 说明 |
|------|---------|------|
| 打开 `.bpf.o` 文件 | `openat()`, `read()` | 读取 ELF 格式的 BPF 目标文件 |
| 解析 ELF 段 | 无（用户态内存操作） | 提取程序段 (`.text`) 和 Map 段 (`.maps`) |

**不涉及 bpf() 系统调用**，只是读取和解析 ELF 文件。

#### 2b. `prog.load()` — **⚡ 关键：调用 bpf(BPF_PROG_LOAD)**

```rust
let prog: &mut SchedClassifier = bpf.program_mut("ja4t_parse_syn")?.try_into()?;
prog.load()?;  // 这里触发 bpf() 系统调用
```

**`aya` 库内部实现**（简化伪代码）：

```
bpf(BPF_PROG_LOAD, {
    .prog_type = BPF_PROG_TYPE_SCHED_CLS,  // TC 分类器
    .insns     = 指向 BPF 指令数组的指针,     // 从 ELF 解析出的字节码
    .insn_cnt  = 指令数量,
    .license   = "GPL",
    .log_level = 7,                          // 获取验证器日志
    .log_size  = 65536,
    .log_buf   = 指向验证器日志缓冲区,
    .kern_version = 当前内核版本号,
    .prog_flags = 0,
    .prog_name  = "ja4t_parse_syn",          // 程序名
    .prog_ifindex = 0,
    .expected_attach_type = 0,
})
```

| 系统调用 | 命令 | 说明 |
|---------|------|------|
| **`bpf(BPF_PROG_LOAD, ...)`** | BPF_PROG_LOAD | 将 BPF 字节码加载到内核，通过验证器安全性检查，JIT 编译为机器码 |
| Linux x86_64 编号 | 321 |  |
| Linux arm64 编号 | 280 |  |

**内核侧执行**:
1. BPF 验证器（verifier）检查字节码安全性（无循环、无越界、无指针泄漏）
2. BPF JIT 编译器将字节码编译为本地机器码
3. 返回一个文件描述符（fd），指向已加载的 BPF 程序

#### 2c. `prog.attach(interface, TcAttachType::Ingress)` — **⚡ 关键：Netlink 套接字**

```rust
prog.attach(interface, TcAttachType::Ingress)?;  // 入站方向
prog.attach(interface, TcAttachType::Egress)?;   // 出站方向
```

**`aya` 库内部实现**（简化伪代码）：

```
// 1. 获取网卡索引
ioctl(socket_fd, SIOCGIFINDEX, &ifr)  // 或通过 netlink 获取

// 2. 创建 NETLINK_ROUTE 套接字
socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE)

// 3. 发送 RTM_NEWTFILTER 消息
sendmsg(netlink_fd, {
    .nlmsghdr = {
        .nlmsg_type = RTM_NEWTFILTER,     // 添加新的 TC 过滤器
        .nlmsg_flags = NLM_F_REQUEST | NLM_F_EXCL | NLM_F_CREATE,
    },
    .tcf_header = {
        .tcm_family  = AF_UNSPEC,
        .tcm_ifindex = 网卡索引,
        .tcm_handle  = 0,
        .tcm_parent  = TC_H_INGRESS,      // 或 TC_H_ROOT (for egress)
        .tcm_info    = (优先级的 16 位值),
    },
    .tca[0] = {
        .nla_type = TCA_KIND,
        .nla_data = "bpf",                // 过滤器类型为 bpf
    },
    .tca[1] = {
        .nla_type = TCA_OPTIONS,
        .nla_data = {
            .nla_type = TCA_BPF_FD,
            .nla_data = bpf_prog_fd,      // 之前 BPF_PROG_LOAD 返回的 fd
        },
    },
})
```

| 系统调用 | 说明 |
|---------|------|
| **`socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)`** | 创建 netlink 套接字，用于内核网络配置 |
| **`sendmsg(netlink_fd, RTM_NEWTFILTER, ...)`** | 发送 RTM_NEWTFILTER 消息，创建 TC 过滤器并绑定 BPF 程序 |
| `recvmsg()` | 接收内核的响应消息 |

**注意**：这里使用的是 **`sendmsg()` 系统调用**（通过 netlink 协议），而不是 `bpf()` 系统调用。但最终效果是让内核网络栈在指定网卡的 ingress/egress 路径上执行 BPF 程序。

---

### 3. BPF Map 操作：`reader.rs` — 读取内核数据

#### 3a. `map.iter()` — 遍历 BPF LRU HashMap

```rust
let map: aya::maps::HashMap<...> = aya::maps::HashMap::try_from(&self.map)?;
for item in map.iter() {
    let (key, value) = item?;
    result.insert(key, value);
}
```

**`aya` 库内部实现**：

```
// 获取第一个 key
bpf(BPF_MAP_GET_NEXT_KEY, {
    .map_fd = map 文件描述符,
    .key    = NULL,                          // NULL 表示获取第一个
    .next_key = 指向 key 缓冲区的指针,
})

// 循环获取下一个 key，直到 ENOENT
bpf(BPF_MAP_GET_NEXT_KEY, {
    .map_fd = map 文件描述符,
    .key    = 当前 key,
    .next_key = 指向下一个 key 缓冲区的指针,
})

// 对每个 key 获取 value
bpf(BPF_MAP_LOOKUP_ELEM, {
    .map_fd = map 文件描述符,
    .key    = 指向 key 的指针,
    .value  = 指向 value 缓冲区的指针,
})
```

| 系统调用 | 命令 | 说明 |
|---------|------|------|
| **`bpf(BPF_MAP_GET_NEXT_KEY, ...)`** | BPF_MAP_GET_NEXT_KEY | 获取 BPF Map 中的下一个 key（遍历用） |
| **`bpf(BPF_MAP_LOOKUP_ELEM, ...)`** | BPF_MAP_LOOKUP_ELEM | 按 key 查询 BPF Map 中的 value |

#### 3b. `map.get(key, 0)` — 直接查询单个 Key

```rust
let value = map.get(&key, 0)?;
```

**`aya` 库内部实现**：

```
bpf(BPF_MAP_LOOKUP_ELEM, {
    .map_fd = map 文件描述符,
    .key    = 指向 key 的指针,
    .value  = 指向 value 缓冲区的指针,
    .flags  = 0,
})
```

| 系统调用 | 命令 | 说明 |
|---------|------|------|
| **`bpf(BPF_MAP_LOOKUP_ELEM, ...)`** | BPF_MAP_LOOKUP_ELEM | 直接查询单个 key（兜底路径） |

---

### 4. 轮询任务：`poller.rs` — 定期导出数据

```rust
pub async fn poll_bpf_map(reader: Arc<BpfReader>, cache: SharedCache, interval_secs: u64) {
    loop {
        interval.tick().await;
        let entries = reader.dump_all()?;     // 遍历 BPF Map
        let counters = reader.dump_debug_counters()?;  // 读取调试计数器
        cache.write().await.update(entries);  // 更新内存缓存
    }
}
```

| 调用 | 内部系统调用 | 说明 |
|------|-------------|------|
| `reader.dump_all()` | `bpf(BPF_MAP_GET_NEXT_KEY)` + `bpf(BPF_MAP_LOOKUP_ELEM)` | 遍历整个 LRU HashMap |
| `reader.dump_debug_counters()` | `bpf(BPF_MAP_LOOKUP_ELEM)` | 读取 Array Map 中的每个计数器 |

---

### 5. IPC 通信：`server.rs` — Unix Domain Socket

```rust
let listener = UnixListener::bind(&socket_path)?;
// ...
match listener.accept().await {
    Ok((stream, _)) => {
        tokio::spawn(async move {
            handle_client(stream, ...).await
        });
    }
}
```

| 系统调用 | 说明 |
|---------|------|
| `socket(AF_UNIX, SOCK_STREAM, 0)` | 创建 Unix Stream 套接字 |
| `bind(socket_path)` | 绑定到文件系统路径 |
| `listen()` | 开始监听 |
| `accept()` | 接受 nginx 连接 |
| `read()`, `write()` | 读写 JSON 格式的请求/响应 |

---

## 完整系统调用清单

| 阶段 | 操作 | 系统调用 | 说明 |
|------|------|---------|------|
| 构建 | 编译 BPF C 代码 | `fork()`, `execve()`, `read()`, `write()` | clang 编译 |
| 加载 | 读取 BPF 目标文件 | `openat()`, `read()`, `mmap()` | 读取 ELF 文件 |
| 加载 | **加载 BPF 程序到内核** | **`bpf(BPF_PROG_LOAD, ...)`** | 验证器 + JIT 编译 |
| 加载 | 获取网卡索引 | `ioctl(SIOCGIFINDEX)` | 获取网卡 ifindex |
| 加载 | 创建 netlink 套接字 | **`socket(AF_NETLINK, ...)`** | 用于网络配置 |
| 加载 | **挂载 TC 过滤器** | **`sendmsg(RTM_NEWTFILTER, ...)`** | 绑定 BPF 程序到网卡 |
| 运行 | 遍历 BPF Map | **`bpf(BPF_MAP_GET_NEXT_KEY, ...)`** | 轮询读取 |
| 运行 | 查询 BPF Map | **`bpf(BPF_MAP_LOOKUP_ELEM, ...)`** | 读取指纹数据 |
| IPC | 创建 Unix Socket | `socket(AF_UNIX, ...)`, `bind()`, `listen()` | 与 nginx 通信 |
| IPC | 处理请求 | `accept()`, `read()`, `write()` | 响应 nginx 查询 |

---

## 涉及的 BPF 辅助函数（eBPF 程序内部）

eBPF C 程序（`tcp_options.bpf.c`、`tls_server_hello.bpf.c`）使用了以下 BPF 辅助函数：

| BPF 辅助函数 | ID | 说明 | 内核函数 |
|-------------|----|------|---------|
| `bpf_map_lookup_elem` | 1 | 查找 BPF Map 条目 | `bpf_map_lookup_elem` |
| `bpf_map_update_elem` | 2 | 更新/插入 BPF Map 条目 | `bpf_map_update_elem` |
| `bpf_ktime_get_ns` | 5 | 获取内核纳秒时间戳 | `bpf_ktime_get_ns` |
| `bpf_skb_load_bytes` | 26 | 从 skb 读取指定偏移的字节 | `bpf_skb_load_bytes` |

这些辅助函数由 eBPF 程序通过函数指针方式调用（`static void *(*bpf_map_lookup_elem)(void *map, const void *key) = (void *)1;`），在 BPF 验证器加载时被解析为内核内部函数。

---

## 总结

| 问题 | 答案 |
|------|------|
| 谁调用 `bpf()` 系统调用？ | **`aya` 库 (v0.12)** 内部调用，项目代码不直接调用 |
| 调用 `bpf(BPF_PROG_LOAD)` 的地方？ | `loader.rs` 中 `prog.load()` → `aya::programs::SchedClassifier::load()` |
| 调用 `bpf(BPF_MAP_*)` 的地方？ | `reader.rs` 中 `dump_all()`, `lookup_direct()`, `dump_debug_counters()` |
| 用什么挂载到网卡？ | **Netlink 套接字** + `RTM_NEWTFILTER` 消息，由 `prog.attach()` → `aya` 内部实现 |
| 为什么不直接用 `bpf()` 挂载？ | TC 过滤器的创建使用传统的 netlink 路由协议，不是 bpf 系统调用的职责 |
| 使用了 `libbpf` 吗？ | **没有**，使用纯 Rust 实现的 `aya` 库替代 |
| 使用了 `bpftool` 吗？ | **没有**，仅文档中提及作为手动调试工具 |