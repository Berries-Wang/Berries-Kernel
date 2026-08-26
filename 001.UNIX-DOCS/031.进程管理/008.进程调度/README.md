# 进程管理/调度
> 阅读: <br/> - [处理器调度：单核调度策略](../../../001.UNIX-DOCS/000.内存管理/998.REFS/000.中山大学-操作系统/6-0331-schedule-1.pdf) <br/> - [处理器调度：多核调度策略](../../../../001.UNIX-DOCS/000.内存管理/998.REFS/000.中山大学-操作系统/7-0407-schedule-2.pdf) <br/> - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure](../../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub) <br/> - 内核中有多个调度器类， CFS算法只是其中一种 <br/> - CFS算法只处理优先级(task_struct->prio)为范围[100, 139]的进程

---
·
## 进程调度
|类型|核心概念|说明|备注|
|-|-|-|-|
|- 多核调度|- 任务在CPU核心之间频繁切换，对缓存不友好<sup>:因为可能之前的缓存无法使用<sup>|- 每个CPU核心(逻辑核心)都有自己的L1缓存 <br/> - 两个同属一个物理核心的逻辑核心，共享L2缓存 <br/> - 一个物理CPU上的所有物理核心共享L3缓存 |- ![wechat_2026-03-19_073523_109.png](./../999.IMGS/wechat_2026-03-19_073523_109.png)|
|-|-|-|-|
|- Linux的多核调度： 标准的两级调度|- 每个CPU有各自的本地调度器和调度队列(rq)|-|- ![wechat_2026-03-19_074551_996.png](./../999.IMGS/wechat_2026-03-19_074551_996.png)|
|- 两级调度问题: 负载均衡|-|-|- ![wechat_2026-03-19_074727_708.png](./../999.IMGS/wechat_2026-03-19_074727_708.png)|
|-两级调度问题: 负载均衡: 负载追踪(负载计算) -> PELT(Per Entity Load Tracking)|-|-|- [处理器调度：多核调度策略](../../../../001.UNIX-DOCS/000.内存管理/998.REFS/000.中山大学-操作系统/7-0407-schedule-2.pdf) > P18|
|-|-|-|-|
|-程序主动选择用哪个CPU来运行|-|-|cpu_set_t mask;<br/>  CPU_ZERO(&mask);<br/> CPU_SET(0,&mask);<br/> CPU_SET(0,&mask);<br/> sched_setaffinity(0,sizeof(mask),&mask);|
|-|-|-|-|

---

## 进程优先级和权重
> 学习: <br/> [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure#P7.4.2　进程优先级和权重](../../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub)

- 关于进程优先级字段: [000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/include/linux/sched.h](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/include/linux/sched.h)
  |优先级字段|说明|备注|
  |-|-|-|
  |- prio|-|- 均参考 struct task_struct 代码注释|
  |- static_prio|-|-|
  |- normal_prio|-|-|
  |- rt_priority|-|-|

- 关于权重:[000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/core.c#`sched_prio_to_weight`](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/core.c)

---

## 调度策略
在 [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure#P7.4.2　进程优先级和权重](../../../007.BOOKs/Run%20Linux%20Kernel%20(2nd%20Edition)%20Volume%201:%20Infrastructure.epub) 中的优先级<sup>字段: task_struct->prio</sup>： 0 ~ 139 ， 其中 0 ~ 99 是实时优先级，100 ~ 139 是普通优先级。在task_struct结构体中，对应的是不同的字段: 为了在代码逻辑上实现硬隔离
- 0-99 (实时)：由实时调度类（Real-Time Scheduler）处理，遵循严格的优先级抢占。
- 100-139 (普通)：由完全公平调度器（Completely Fair Scheduler, CFS）处理，采用时间片轮转和优先级调整机制。


### 调度类 <sup>[000.LINUX-7.1.3/include/uapi/linux/sched.h](../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/uapi/linux/sched.h)</sup>
进程调度依赖于调度策略（schedule policy），Linux内核把相同的调度策略抽象成调度类（schedule class）。不同类型的进程采用不同的调度策略，目前Linux内核中默认实现了5种调度类，分别是stop、deadline、realtime、CFS和idle，它们分别使用sched_class来定义，并且通过next指针串联在一起
```txt
    # 按照优先级从高到低的顺序串联在一起的
    +----------+      +------------+      +------------+      +---------+      +-----------+        +----------+
    |   stop   | ---> |  deadline  | ---> |  realtime  | ---> |   CFS   | ---> |EXT(>=6.12)|  --->  |   idle   |
    +----------+      +------------+      +------------+      +---------+      +-----------+        +----------+
```

|调度类|调度策略|使用范围|说　　明|
|-|-|-|-|
|- stop|- 无|- 最高优先级的进程，比deadline进程的优先级高|（1）可以抢占任何进程。<br/>（2）在每个CPU上实现一个名为“migration/N”的内核线程，N表示CPU的编号。该内核线程的优先级最高，可以抢占任何进程的运行，一般用来运行特殊的功能。<br/>（3）用于负载均衡机制中的进程迁移、softlockup检测、CPU热插拔、RCU等|
|-|-|-|-|
|- deadline|- SCHED_DEADLINE|- 最高优先级的实时进程。优先级为−1|- 用于调度有严格时间要求的实时进程，如视频编/译码等。|
|-|-|-|-|
|- realtime|- SCHED_FIFO、SCHED_RR|- 普通实时进程。优先级为0～99|- 用于普通的实时进程，如IRQ线程化|
|-|-|-|-|
|- CFS|- SCHED_NORMAL、 SCHED_BATCH、 SCHED_IDLE|- 普通进程。 优先级为100～139|- 由CFS来调度|
|-|-|-|-|
|- sched_ext|- SCHED_EXT|- 优先级在CFS与idle之间，通过BPF程序动态加载|- 由BPF调度器调度，可热插拔替换CFS调度策略。<br/>- 通过 ``struct sched_ext_ops`` 实现自定义调度算法。<br/>- 支持 ``SCX_OPS_SWITCH_PARTIAL`` 仅接管SCHED_EXT任务。<br/>- 出错时自动回退到CFS调度器。|
|-|-|-|-|
|- idle|- 无|- 	最低优先级的进程|- **当就绪队列中没有其他进程时进入idle调度类**。<br/> - idle调度类会让CPU进入低功耗模式|

---

### 进程调度
[__schedule](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/core.c)是调度器的核心函数，其作用是让调度器选择和切换到一个合适进程并运行。调度的时机可以分为如下3种:
- 在阻塞操作中，如使用互斥量（mutex）、信号量（semaphore）、等待队列（waitqueue）等。
- 在中断返回前和系统调用返回用户空间时，检查TIF_NEED_RESCHED标志位以判断是否需要调度。
- 将要被唤醒的进程不会马上调用schedule()，而是会被添加到CFS就绪队列中，并且设置了TIF_NEED_RESCHED标志位。

##### 那么被唤醒的进程什么时候被调度呢？这要根据内核是否具有可抢占功能（CONFIG_PREEMPT=y）分两种情况
|内核是否支持抢占|规则|
|-|-|
|- 支持抢占|- 如果唤醒动作发生在系统调用或者异常处理上下文中，在下一次调用preempt_enable()时会检查是否需要抢占调度。 <br/> - 如果唤醒动作发生在硬中断处理上下文中，硬件中断处理返回前会检查是否要抢占当前进程。|
|-|-|
|- 不支持抢占|- 当前进程调用cond_resched时会检查是否要调度。<br/> - 主动调用schedule。|

##### 调度节拍(8.1.7　调度节拍)
每当时钟中断发生时，Linux调度器的[scheduler_tick](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/core.c)函数会被调用，执行和调度相关的一些操作，如检查是否有进程需要调度和切换。
|注意事项|说明|备注|
|-|-|-|
|- 在scheduler_tick执行流程中，并不会执行schedule，而是给当前进程设置TIF_NEED_RESCHED标志位|-|-|
|-|-|-|

---

### 进程切换
在操作系统中把当前正在运行的进程挂起并且恢复以前挂起的某个进程的执行，这个过程称为进程切换或者上下文切换。Linux内核实现进程切换的核心函数是[context_switch](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/core.c)。

进程上下文是进程执行活动时的静态描述，它包含与执行该进程有关的各种寄存器、内核栈、task_struct等数据结构。我们把已执行过的进程指令和相关数据称为上文，把待执行的指令和数据称为下文。进程切换主要涉及3个操作。
- 保存当前进程（prev进程）的上下文。
- 恢复某个先前被调度出去的进程（next进程）的上下文。
- 运行next进程。

---

## 附录：CFS 队列的根在哪里

> 分析对象: `kernel/sched/core.c`、`kernel/sched/fair.c`、`kernel/sched/sched.h`（Linux 5.9）

CFS 队列的"根"分三层，需要区分开看。

### 1. 组调度（层次）的根：`root_task_group`

[core.c:7351](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/core.c) 定义全局静态变量，是整个 cgroup 组调度树的最顶端：

```c
struct task_group root_task_group;
```

- 在 `sched_init()`（core.c:7386-7428）中挂入全局链表 `task_groups`，并初始化 `cfs_bandwidth`、`list` 等。
- 所有通过 cgroup 创建的任务组都挂在它下面；`walk_tg_tree()` 等遍历也从它出发。

### 2. 每 CPU 顶层的 cfs_rq：`rq->cfs`（这才是"队列"的本体）

CFS 没有全局唯一的队列，每个 CPU 各有一个根队列，内嵌在 `struct rq` 里（[sched.h:991](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/sched.h)）：

```c
struct cfs_rq  cfs;   /* 通用就绪队列中的CFS就绪队列 */
```

关键初始化在 [core.c:7467](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/core.c)：

```c
init_tg_cfs_entry(&root_task_group, &rq->cfs, NULL, i, NULL);
```

`init_tg_cfs_entry()`（[fair.c:11604](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/fair.c)）：

```c
tg->cfs_rq[cpu] = cfs_rq;   /* root_task_group.cfs_rq[i]  = &rq->cfs */
tg->se[cpu]     = se;       /* root_task_group.se[i]      = NULL   */
if (!se) return;            /* se==NULL 直接返回，不建嵌套实体 */
```

即 `root_task_group` 每 CPU 的队列指针直接指向该 CPU 的 `rq->cfs`，而它的 `se[i]` 为 NULL —— 根组的任务不套任何一层"组实体"，直接排队进 `rq->cfs` 的顶层红黑树（对应 core.c:7459 的注释）。

### 3. 真正的红黑树根：`cfs_rq->tasks_timeline`

每个 `cfs_rq` 内部管可运行任务的红黑树根（[sched.h:586](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/sched.h)，按 vruntime 排序），由 `init_cfs_rq()`（[fair.c:11425](../../../000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/kernel/sched/fair.c)）初始化：

```c
cfs_rq->tasks_timeline = RB_ROOT_CACHED;
```

`enqueue_entity()` / `pick_next_entity()` 进出的就是这颗树。

### 关系图

```txt
             root_task_group (core.c:7351, 组调度根)
            /                    \     ... 每个CPU
   cfs_rq[0]────→ &rq->cfs       cfs_rq[N-1]────→ &rq->cfs   (se[] = NULL)
                     │
             ┌───────┴─────────┐
        struct rq.cfs   struct rq.cfs        ← 每CPU顶层cfs_rq
             │                     │
      tasks_timeline(rbtree)   tasks_timeline(rbtree)  ← 真正排队的红黑树
```

补充：`rq->leaf_cfs_rq_list`（sched.h:1035）把所有叶子 cfs_rq（含各 CPU 的 `rq->cfs`）串成链表，供负载均衡使用；根组的 `rq->cfs` 也在此链表的表头（fair.c:377-409）。

> **总结**：层次结构的根是 `root_task_group`；每 CPU 顶层的根队列是 `rq->cfs`（`root_task_group.cfs_rq[i]` 指向它）；真正存可运行任务的红黑树根是 `cfs_rq->tasks_timeline`。

---

## 附录：进程挂起与唤醒

> 分析对象: `kernel/sched/core.c`、`kernel/sched/fair.c`、`kernel/sched/wait.c`（Linux 5.9）

核心要点：**挂起/唤醒时 `task_struct` 本体没有"移动"，变的是 `se->run_node` 挂在哪个树上 + `p->on_rq` 标志位**。只有 `TASK_RUNNING` 的任务在 CFS 运行队列里；挂起任务被出队，挂在各自等待对象的等待队列上。

### 挂起：从 CFS 队列移除，移到等待队列

```txt
set_current_state(TASK_INTERRUPTIBLE)        ① 改状态
prepare_to_wait(wq_head, wq_entry, state)    ② 挂到等待对象的等待队列(wait.c:251)
   └─ __add_wait_queue() 把 task 链进 wait_queue_head.head
schedule() → __schedule()                    ③ core.c:4730
   └─ prev_state != 0 → deactivate_task(rq, prev, DEQUEUE_SLEEP)   core.c:4763
        ├─ p->on_rq = 0                                              core.c:1623
        └─ dequeue_task → dequeue_task_fair → dequeue_entity        fair.c
             └─ __dequeue_entity()  rb_erase_cached(se->run_node, &cfs_rq->tasks_timeline)   fair.c:664
```

**移到哪里去了**：任务睡眠前先把自己挂到"它正在等的那个对象"的等待队列上（`prepare_to_wait()`，wait.c:251）。然后 `__schedule()` 发现它非 RUNNING，才把它从 CFS 红黑树里**擦除**（`__dequeue_entity()`，fair.c:664）。

- **等待队列**（`wait_queue_head` 上的 `wq_entry->entry` 链表，`private` 指向 task_struct）——这是"挂"的位置；
- task_struct 本身**留在全局任务链表** `init_task.tasks` 里没被移动，只是 `p->on_rq = 0`（core.c:1623）标记为"不在运行队列"。

### 唤醒：从等待队列取出，重新加入 CFS 队列

```txt
wake_up() → __wake_up() → __wake_up_common()              ① 遍历等待队列
   └─ 调用每个 entry 的 func = default_wake_function()     ②
        └─ try_to_wake_up(curr->private, ...)  ← 从等待队列 entry 里取出 task_struct   (core.c, private=task)
try_to_wake_up()                                          ③ core.c:2930
   ├─ p->state = TASK_RUNNING
   └─ ttwu_queue → ttwu_do_activate()                      core.c:2777 / :2526
        └─ activate_task(rq, p, ENQUEUE_WAKEUP|ENQUEUE_NOCLOCK)   core.c:1608
             ├─ p->on_rq = TASK_ON_RQ_QUEUED
             └─ enqueue_task → enqueue_task_fair → enqueue_entity → __enqueue_entity()  fair.c
                  └─ rb_add_cached(se->run_node, &cfs_rq->tasks_timeline, __entity_less)  fair.c:631
                  按 vruntime 找到位置插回红黑树
```

**从哪里加入的**：`default_wake_function()`（core.c）里 `try_to_wake_up(curr->private, ...)` —— **`curr->private` 就是挂起时存在等待队列 entry 里的 task_struct 指针**。唤醒顺着等待队列 entry 把 task_struct 拿回来，再把它那个**同一个 `se->run_node`**（挂起时只是被擦除、没销毁）按 vruntime 重新 `rb_add_cached` 插回 `cfs_rq->tasks_timeline`（fair.c:631）。

### 挂起与唤醒闭环图

```txt
                        ┌───────────────────────────────────┐
                        │   等待对象(互斥量/完成量/futex等)   │
                        │   wait_queue_head                 │
                        │   └─ head 链表 ◄── 挂起进程的 entry │
                        └───────────────▲───────────────────┘
                                        │ prepare_to_wait() 挂入(wait.c)
          __schedule→deactivate_task    │
CFS 红黑树 tasks_timeline ◄─────────────┤
  └─ rb_erase_cached (fair.c:664)       │
  └─ rb_add_cached  (fair.c:631)        │ try_to_wake_up(private) 取出(core.c)
                        └───────────────┴───────────────────┐
                        │     wake_up→default_wake_function  │
                        └────────────────────────────────────┘
```

### 进程等待 I/O 时的挂起

阻塞式 I/O（同步 read/write）会让进程睡眠进入 `TASK_UNINTERRUPTIBLE`（D 状态，不可中断睡眠，连信号都叫不醒）或 `TASK_INTERRUPTIBLE`。出队路径与通用挂起一致，唯一多出的标记是 `io_schedule()`：

```c
/* kernel/sched/core.c:6562-6594 */
int io_schedule_prepare(void)
{
	int old_iowait = current->in_iowait;
	current->in_iowait = 1;        /* 标记"我在等IO" */
	blk_schedule_flush_plug(current);
	return old_iowait;
}
void __sched io_schedule(void)
{
	token = io_schedule_prepare();
	schedule();
	io_schedule_finish(token);
}
```

`__schedule()` 看到 `prev->in_iowait` 为真会 `rq->nr_iowait++`（core.c:4765）——这是 `top` 里 iowait 那列数据的来源。

**放到哪里**：挂在"它正在等的那个 I/O 对象"的等待队列上，没有统一的 IO 队列。最经典的是页缓存 I/O（`mm/filemap.c`）：

- `wait_on_page_bit_common()`：用 `struct wait_page_queue`（含 `wait_queue_entry` + page + bit_nr）把自己 `__add_wait_queue_entry_tail` 挂到**那一页**的等待队列，循环里 `io_schedule()`（mm/filemap.c:1253）让出 CPU；
- 等待队列头来自 `page_waitqueue(page)` → `page_wait_table[hash_ptr(page, ...)]`（mm/filemap.c:976）——**全局一张按页地址哈希的等待队列表**；
- 设备驱动场景：驱动内嵌 `wait_queue_head_t`，用 `wait_event_interruptible()`/`wait_event()` 挂上去；块层用 `blk_io_schedule()`（fs/block_dev.c:247）。

**唤醒**：I/O 完成中断 → `wake_up()` → `__wake_up_common()` → 从等待队列 entry 取出 task → `try_to_wake_up()` → 重新插回 CFS 红黑树。

> 注意：`O_NONBLOCK`/`O_ASYNC` 返回 `-EAGAIN` 不挂起；io_uring/AIO 异步 I/O 不阻塞进程本身；mmap 缺页触发的 I/O 会挂起。

### sleep 挂起（定时器路径）

`sleep` 挂起时状态是 `TASK_INTERRUPTIBLE`（可被信号提前唤醒），但**不是挂在等待队列上，而是挂在内核定时器上**。

用户态 `sleep()` → `nanosleep()` → `do_nanosleep()`（`kernel/time/hrtimer.c`）：

```c
static int __sched do_nanosleep(struct hrtimer_sleeper *t, enum hrtimer_mode mode)
{
	do {
		set_current_state(TASK_INTERRUPTIBLE);   /* 可被信号打断 */
		hrtimer_sleeper_start_expires(t, mode);   /* 启动高精度定时器 */
		if (likely(t->task))
			freezable_schedule();               /* 让出CPU → 从CFS红黑树出队 */
		hrtimer_cancel(&t->timer);
	} while (t->task && !signal_pending(current)); /* 定时器没到期且没信号 → 继续睡 */
	__set_current_state(TASK_RUNNING);
	...
}
```

承载睡眠的是 `struct hrtimer_sleeper`（带 `task` 指针，include/linux/hrtimer.h），定时器被挂到**当前 CPU 的 hrtimer 红黑树**（按到期时间排序）上。

内核代码里的睡眠走 `schedule_timeout()`（`kernel/time/timer.c`），挂到 **timer wheel**（jiffies 定时器轮）上：

```c
timer.task = current;
timer_setup_on_stack(&timer.timer, process_timeout, 0);
__mod_timer(&timer.timer, expire, MOD_TIMER_NOTPENDING);  /* 挂进 timer wheel */
schedule();
```

**唤醒**：定时器到期回调 → `wake_up_process()` → `try_to_wake_up()` → 重新插回 CFS 红黑树：

```c
/* hrtimer 到期（kernel/time/hrtimer.c） */
hrtimer_wakeup()  →  t->task = NULL;  wake_up_process(task);
/* jiffies 定时器到期（kernel/time/timer.c） */
process_timeout() →  wake_up_process(timeout->task);
```

`do_nanosleep()` 的循环也解释了 `sleep()` 能被信号提前返回：中途收到信号则 `signal_pending()` 为真退出循环，把剩余时间返回用户态（`sleep` 提前醒来时返回的就是剩余秒数）。

### 三种挂起位置对比

| 挂起原因 | 状态 | 挂在哪里 | 唤醒者 |
|-|-|-|-|
| 等 I/O | `TASK_UNINTERRUPTIBLE`（D）或 `INTERRUPTIBLE` | 所等对象的**等待队列**（驱动 `wait_queue_head`、页哈希等待队列表 `page_wait_table`） | I/O 完成中断 → `wake_up()` |
| `sleep()`/`nanosleep()` | `TASK_INTERRUPTIBLE` | **hrtimer 红黑树**（`hrtimer_sleeper`） | 定时器到期 → `hrtimer_wakeup()` |
| `schedule_timeout()`/`msleep()` | `INTERRUPTIBLE`/`UNINTERRUPTIBLE`（调用者设） | **timer wheel**（jiffies 定时器轮） | 定时器到期 → `process_timeout()` |

### 代码位置速查

| 环节 | 函数 | 位置 |
|-|-|-|
| 挂起：置状态 + 挂入等待队列 | `set_current_state()` / `prepare_to_wait()` | kernel/sched/wait.c:251 |
| 挂起：检查状态并出队 | `__schedule()` → `deactivate_task()` | core.c:4730 / core.c:1620 |
| 挂起：CFS 红黑树删除 | `__dequeue_entity()` | fair.c:664 |
| 唤醒：从等待队列取出 task | `default_wake_function()` → `try_to_wake_up()` | core.c / core.c:2930 |
| 唤醒：入队 | `ttwu_do_activate()` → `activate_task()` | core.c:2526 / core.c:1608 |
| 唤醒：CFS 红黑树插入 | `__enqueue_entity()` | fair.c:631 |
| I/O：标记 in_iowait + schedule | `io_schedule()` | core.c:6594 |
| I/O：计入 iowait | `__schedule()` 里 `rq->nr_iowait++` | core.c:4765 |
| I/O：页等待队列头 | `page_waitqueue()` / `page_wait_table` | mm/filemap.c:976 |
| sleep：nanosleep 路径 | `do_nanosleep()` | kernel/time/hrtimer.c |
| sleep：hrtimer 到期回调 | `hrtimer_wakeup()` | kernel/time/hrtimer.c |
| sleep：通用定时器路径 | `schedule_timeout()` / `process_timeout()` | kernel/time/timer.c |

> **总结**：挂起 = 从 CFS 红黑树擦除 `se->run_node` + 把 task_struct 挂进等待对象的等待队列（`p->on_rq=0`）；唤醒 = 从等待队列 entry 的 `private` 拿回 task_struct，把同一个 `se->run_node` 按 vruntime 重新插回 `tasks_timeline`（`p->on_rq=TASK_ON_RQ_QUEUED`）。task_struct 本体从头到尾都在全局任务链表里，没有移动过。

