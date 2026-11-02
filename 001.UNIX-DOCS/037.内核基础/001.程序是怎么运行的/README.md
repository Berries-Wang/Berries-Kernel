# 程序是怎么运行的

从"敲下命令"到"main 执行"再到"exit 收尸"，PC 与地址空间的完整接力。内核侧主基准 [003.LINUX-7.1.3](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3)，用户态 glibc 2.40（[002.GLIBC](../../../000.SOURCE_CODE/002.GLIBC/000.GLIBC-2.40)）。

## 目录

| # | 文档 | 内容 |
| - | - | - |
| 000 | [Program Counter 与0x8000](./000.Program%20Counter%20与0x8000.md) | PC 的初始值=复位向量（勘误"PC 上电即 0x8000"）；0x8000 的三重身份（armlink 缺省/32 位内核 TEXT_OFFSET/教学惯例）；从上电到 main 的 PC 接力图 |
| 001 | [execve 与 ELF 加载：内核侧](./001.execve与ELF加载：内核侧.md) | `execve → search_binary_handler → load_elf_binary`：按 `PT_LOAD` 装配地址空间、铺 argc/argv/envp/auxv、`start_thread` 设 `pc/pstate`，`eret` 出发 |
| 002 | [从 _start 到 main：用户态的接力](./002.从_start到main：用户态的接力.md) | 入口栈布局、glibc `_start` 逐行（`:got:main` 的 GOT 取址）、`__libc_start_main` → `main` → `exit` |

## 全景：一条命令的生命周期

```
 用户态                          内核态                          磁盘
 ──────────────────────────────────────────────────────────────────────
 shell: fork() ──────────────────► copy_process(子进程)
        execve("./a.out") ──────► do_execveat_common               ─► 读 ELF 头
                                   └► load_elf_binary
                                       ├ vm_mmap(PT_LOAD: .text/.data/.bss) ─► 页缓存/读入
                                       ├ 装载解释器 ld.so(PT_INTERP)
                                       ├ 铺启动栈(argc/argv/envp/auxv)
                                       └ start_thread: pc=e_entry, pstate=EL0t
        ◄──────────────────────── eret(回到 EL0, 已是新程序)
 [ld.so] ─► 重定位 GOT(有 PT_INTERP 时; 入口先落它) ─► 跳 auxv AT_ENTRY
 _start(crt1.o) ─► __libc_start_main(TLS/.init_array) ─► main() ─► exit(ret) ─► do_exit
```

三篇各管一段：000 管"为什么必须有入口地址"，001 管"内核怎么把程序摆到 PC 面前"，002 管"PC 到手之后用户态先跑谁"。

## 相关资料

| 资料 | 链接 | 说明 |
| - | - | - |
| PSTATE（Processor State）寄存器 | [009.中断与异常/003.PSTATE.md](../../009.中断与异常/003.PSTATE.md) | PSTATE：Arm 下特有的 Processor State（处理器状态），`eret` 后进入的 EL0t 就写在这里 |
| 链接器（入口是谁选的） | [028.编译原理/000.链接器](../../028.编译原理/000.链接器/README.md) | `_start`/`e_entry` 从符号解析来 |
| ADRP 指令 | [008.COMPUTER_SCIENCE/001.汇编指令](../../../008.COMPUTER_SCIENCE/001.汇编指令/README.md) | `_start` 里取 `main` 地址用的那条指令 |
| 内核自己的"入口" | [029.内核启动流程](../../029.内核启动/002.内核启动流程/README.md) | 对照组：内核没有 crt/加载器，bootloader 直接跳 `_head` |
