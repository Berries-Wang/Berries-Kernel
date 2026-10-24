# EBPF

## 目录(2026-10 整理:类型 / 使用场景 / 生效 / 卸载系列)

> 类型笔记的源码行号均基于仓库 **Linux 7.1.3 树**,并逐条核实;生命周期与各类型"如何生效/如何卸载"见各篇 ASCII 图。

- 入门与总纲:
  - [000.从bpf系统调用来入门EBPF](000.从bpf系统调用来入门EBPF.md) — bpf(2) 命令表 + BPF_PROG_LOAD 下沉路径
  - [001.eBPF程序类型总览](001.eBPF程序类型总览.md) — 全部 prog type 分类、选型速记
  - [002.eBPF生命周期-加载生效卸载](002.eBPF生命周期-加载生效卸载.md) — 状态机、引用计数、pin、link
- 网络类:[010.XDP](010.XDP.md) · [011.TC](011.TC-SCHED_CLS与SCHED_ACT.md) · [012.SOCKET_FILTER](012.SOCKET_FILTER.md) · [013.SK_MSG与SK_SKB(sockmap)](013.SK_MSG与SK_SKB-sockmap.md) · [014.SK_LOOKUP](014.SK_LOOKUP.md) · [015.FLOW_DISSECTOR](015.FLOW_DISSECTOR.md) · [016.LWT](016.LWT.md)
- 跟踪类:[020.KPROBE与UPROBE](020.KPROBE与UPROBE.md) · [021.TRACEPOINT](021.TRACEPOINT.md) · [022.RAW_TRACEPOINT](022.RAW_TRACEPOINT.md) · [023.PERF_EVENT](023.PERF_EVENT.md) · [024.TRACING(fentry/fexit)](024.TRACING-fentry-fexit-fmod_ret.md)
- cgroup/安全/其他:[030.CGROUP系列](030.CGROUP系列.md) · [031.LSM](031.LSM.md) · [032.STRUCT_OPS](032.STRUCT_OPS.md) · [033.SYSCALL](033.SYSCALL.md) · [034.其他类型](034.其他类型-LIRC-SK_REUSEPORT-EXT-NETFILTER.md)
- 实战示例:[100.JA4T-TC指纹示例](100.JA4T-TC指纹示例/000.导读.md) — TC 分类器完整项目拷贝(eBPF 程序 + Rust daemon + 构建脚本 + 文档,`cargo build` 可用),迁移自 Berries-CYBERCLOAK
- 索引:[000.EBPF-学习资料索引](000.EBPF-学习资料索引.md)

#### 001.EBPF执行链路全景(TC)

> 注:下图为 5.9 树的分析;结论在 7.1.3 仍适用,行号有偏移(如 `cls_bpf_classify` 7.1.3 在 cls_bpf.c:81,详见 [011.TC 笔记](011.TC-SCHED_CLS与SCHED_ACT.md))。

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