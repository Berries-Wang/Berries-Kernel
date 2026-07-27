# EBPF
#### 001.EBPF执行链路全景(TC)

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