# 进程间通信(Inter-Process Communication, IPC)

> 学习: [进程间通信IPC](../../001.操作系统课程/000.中山大学-操作系统2025/7-0410-ipc.pdf)
> 口径：Linux 7.1.3（[000.SOURCE_CODE/003.LINUX-7.1.3](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3)）源码行号均 `grep -n` 实测；平台口径按仓库约定（arm64 主实例 AGI / 对照 EPYC 9996），横切内容统一见 [005.平台注记与调度衔接](./005.平台注记与调度衔接.md)。
> 各文件末尾的 C 示例均已在本机编译运行验证，源码存于 [003.TEST-SPACE/ipc-demo](../../../003.TEST-SPACE/ipc-demo/)。

## 目录

| 文件 | 对应 IPC 方式 | 内核机制 | 源码 | 示例 |
|-|-|-|-|-|
| [000.SysV-IPC公共框架](./000.SysV-IPC公共框架.md) | （三兄弟底座）| kern_ipc_perm / ipc_ids / IPCMNI / ipcperms | `ipc/util.c`、`include/linux/ipc.h` | 同 key 建三对象 |
| [001.共享内存](./001.共享内存.md) | 共享内存 | SysV shm / 匿名 mmap / memfd | `ipc/shm.c`、`mm/shmem.c` | shmget+shmat 父子传话 |
| [002.消息队列](./002.消息队列.md) | 消息传递 / 带类型消息 | SysV msg / POSIX mqueue（优先级树）| `ipc/msg.c`、`ipc/mqueue.c` | msgsnd/msgrcv 的 msgtyp |
| [003.Unix管道](./003.Unix管道.md) | Unix 管道 | pipe 环形页缓冲 / FIFO / unix socket | `fs/pipe.c`、`net/unix/af_unix.c` | `ls \| wc -l` 原型 |
| [004.同步型IPC](./004.同步型IPC.md) | （不搬数据的另一半）| futex / SysV sem / 信号 / eventfd | `kernel/futex/`、`kernel/signal.c`、`fs/eventfd.c` | eventfd 门铃 |
| [005.平台注记与调度衔接](./005.平台注记与调度衔接.md) | （横切）| arm64 底座 / 睡眠-唤醒 | — | — |

## 一句话总纲

- **数据面**：只有共享内存是零拷贝（内核只当"开房东"）；消息类通道都要"用户→内核→用户"两次拷贝——内核要排队就得先持有数据（[001](./001.共享内存.md) vs [002](./002.消息队列.md)/[003](./003.Unix管道.md)）；
- **控制面**：排队等的是"事件"不是数据——管道空、队列满、futex 没人就绪，全部落在调度器的等待队列上。所以 **IPC 的性能问题，一半是拷贝问题，一半是唤醒路径问题**（[005](./005.平台注记与调度衔接.md)）。
