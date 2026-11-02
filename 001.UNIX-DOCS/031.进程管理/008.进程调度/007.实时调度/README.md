# 实时调度 — Linux 7.1.3

> 学习:<br/>
> - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub#8.5　实时调度](../../../../007.BOOKs/Run%20Linux%20Kernel%20%282nd%20Edition%29%20Volume%201:%20Infrastructure.epub)<br/>
> - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub#9.3.5　案例分析——为何不能调度](../../../../007.BOOKs/Run%20Linux%20Kernel%20%282nd%20Edition%29%20Volume%201:%20Infrastructure.epub)<br/>
> - [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub#4.12 关于Crash工具的调试技巧汇总](../../../../007.BOOKs/Run%20Linux%20Kernel%20%282nd%20Edition%29%20Volume%202:%20Debugging%20and%20Case%20Analysis.epub)

## 摘要

| 项 | 说明 |
|---|---|
| 主题 | Linux 7.1.3 实时调度: SCHED_FIFO / SCHED_RR / SCHED_DEADLINE 的内核实现 |
| 核心源码 | [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/rt.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/rt.c)、[cpupri.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/cpupri.c)、[deadline.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/deadline.c)、[core.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/core.c) |
| 调度类层次 | stop > dl > rt > fair > ext > idle（7.1.3 中 sched_ext 的 ext 类位于 fair 与 idle 之间） |
| RT 优先级 | 用户 1~99 ↔ 内核 `p->prio` 98~0，`MAX_RT_PRIO=100`（[include/linux/sched/prio.h:16](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched/prio.h)） |
| RT 负载均衡 | 不走 CFS 均衡器，用 push/pull + cpupri 优先级位图 O(1) 找最低优先级 CPU |
| RT throttling | 默认每 1000ms 周期最多跑 950ms RT（`sysctl_sched_rt_runtime=950000`，rt.c:24），超限节流 50ms |
| SCHED_DEADLINE | EDF + CBS，runtime/deadline/period 三元组，准入控制 `\Sum(runtime/period) < M` |
| 大规模实例 | AMD EPYC 9996（256 核/512 线程, Zen 6c, 8 CCD/16 CCX, NPS4）上的 RT 部署与隔离 |
| 架构说明 | rt.c/deadline.c 架构无关通用走读；中断/定时器差异以 arm64（Arm AGI CPU，SP113012，136×Neoverse V3，双 chiplet）为参照，x86 规模化以 EPYC 9996 为例 |

---

## 1. 实时调度总览

### 1.1 软实时与硬实时

实时指的是一个特定任务的执行时间必须是确定的并且可预测的（书 §8.5）：

- **软实时**：只要求统计意义上实时，偶发超时可以接受，如视频播放器；
- **硬实时**：执行时间必须绝对保证，否则产生灾难性后果，如飞行控制器。

主线 Linux（含 7.1.3）不是硬实时操作系统；硬实时需要 PREEMPT_RT 补丁集（中断线程化、可抢占自旋锁/RCU 等）。主线已经合入了表 8.15（书中）所列的大部分实时性改进：内核抢占（2.5）、PI Mutexes、高精度定时器（2.6.24）、可抢占 RCU（2.6.25）、中断线程化（2.6.30/2.6.39）、Deadline 调度器（3.14）。

### 1.2 实时延时分解

书 §8.5.1 把"中断发生 → RT 进程真正上 CPU"的延时分解为四段：

```
 T0          T1            T2            T3        T4
  |----中断延时----|---中断处理延时---|--调度延时--|--上下文切换--|
  外设中断          CPU 响应中断       唤醒进程A     调度器选中A     A 开始执行

  中断延时     : 关中断临界区(spin_lock_irqsave) + 中断控制器优先级调度
  中断处理延时 : 上半部/下半部, 下半部可被其他中断打断
  调度延时     : 自旋锁临界区禁抢占; 就绪队列中可能有更高优先级进程
  上下文切换   : switch_to() 本身的开销
```

对 RT 调度器本身而言，它能优化的只有"调度延时"这一段（第 4 节的 push/pull、第 7 节的 CPU 隔离都是在压缩这一段）；其余三段来自中断子系统与内核抢占模型（arm64 上即 GIC 中断控制器 + arch timer，见 7.6 节）。

### 1.3 四种策略对比

| 策略 | 调度类 | 优先级/参数 | 抢占语义 | 时间片 |
|---|---|---|---|---|
| SCHED_FIFO | rt_sched_class | 用户 RT 优先级 1~99 | 永远抢占同 CPU 上 prio 更低者；同级新唤醒任务不抢占正在运行的（prio 相同不重调度，见 wakeup_preempt_rt，rt.c:1619） | 无 |
| SCHED_RR | rt_sched_class | 同 FIFO | 同 FIFO | 100ms（`RR_TIMESLICE`，[include/linux/sched/rt.h:82](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched/rt.h)），用尽后排到同级队列尾部（task_tick_rt，rt.c:2534） |
| SCHED_DEADLINE | dl_sched_class | runtime/deadline/period 三元组 | EDF：绝对截止期最早者先跑；dl 类整体压过 rt 类 | CBS 按周期补充 runtime |
| SCHED_NORMAL/OTHER | fair_sched_class | nice -20~19 | CFS 公平份额，永远让位于 dl/rt | 由虚拟时间决定 |

补充两点：

- FIFO/RR 任务 `timer_slack_ns` 会被清零（`__setscheduler_params()`，syscalls.c:258-263），定时器精度更高；
- FIFO/RR 受 RT throttling 约束（第 5 节），DEADLINE 受准入控制 + CBS 节流约束（第 6 节），fair 不受任何带宽上限约束。

### 1.4 RT 优先级映射：用户 1~99 ↔ 内核 0~98

```c
// include/linux/sched/prio.h:16
#define MAX_RT_PRIO		100        // RT 优先级共 100 档: 内核 prio 0~99
#define MAX_PRIO		(MAX_RT_PRIO + NICE_WIDTH)   // +40 档 nice = 140
```

```c
// kernel/sched/syscalls.c:19
static inline int __normal_prio(int policy, int rt_prio, int nice)
{
	int prio;

	if (dl_policy(policy))
		prio = MAX_DL_PRIO - 1;            // DL 任务统一 prio = -1
	else if (rt_policy(policy))
		prio = MAX_RT_PRIO - 1 - rt_prio;  // 用户 1~99 → 内核 98~0 (数值越小优先级越高)
	else
		prio = NICE_TO_PRIO(nice);         // nice -20~19 → 100~139

	return prio;
}
```

数值方向是反的：**内核 `p->prio` 越小优先级越高**。`chrt -f 99` 的任务 `p->prio=0`，是全系统优先级最高的用户任务：

```
 用户视角 (rt_priority)   99  98   ...   50   ...  2   1      (nice -20 ... +19)
                          |   |         |         |   |        |
 内核 p->prio             0   1   ...  49   ...  97  98  100 ······ 139
                          \_____________ ___________/    \___ ___/
                                        V                  V
                             RT 范围 [0, MAX_RT_PRIO)     CFS 范围 [100, MAX_PRIO)
                             stop/migration 线程也是 prio 0 (SCHED_FIFO 伪装)
```

校验入口在 `__sched_setscheduler()`（syscalls.c:493）：`attr->sched_priority > MAX_RT_PRIO-1` 返回 `-EINVAL`（即用户值 0~99 合法，且 rt 策略必须非 0、非 rt 策略必须为 0）；无 `CAP_SYS_NICE` 时还受 `RLIMIT_RTPRIO` 约束（syscalls.c:442-451），DEADLINE 一律需要特权（syscalls.c:453-457）。

---

## 2. sched_class 调度类层次

### 2.1 类链顺序（以 7.1.3 为准）

调度类实例被链接进 `__sched_class_highest[]` ~ `__sched_class_lowest[]` 区间（定义见 include/asm-generic/vmlinux.lds.h），**地址越靠前优先级越高**：

```c
// kernel/sched/sched.h:2771
#define sched_class_above(_a, _b)	((_a) < (_b))
```

core.c 在启动时用 BUG_ON 自检了这个顺序：

```c
// kernel/sched/core.c:8908-8914
	BUG_ON(!sched_class_above(&stop_sched_class, &dl_sched_class));
	BUG_ON(!sched_class_above(&dl_sched_class, &rt_sched_class));
	BUG_ON(!sched_class_above(&rt_sched_class, &fair_sched_class));
	BUG_ON(!sched_class_above(&fair_sched_class, &idle_sched_class));
#ifdef CONFIG_SCHED_CLASS_EXT
	BUG_ON(!sched_class_above(&fair_sched_class, &ext_sched_class));  // fair > ext
	BUG_ON(!sched_class_above(&ext_sched_class, &idle_sched_class));  // ext > idle
#endif
```

```
 __sched_class_highest ─────────────────────────────────────────────► __sched_class_lowest
 ┌────────┬────────┬────────┬────────┬────────┬────────┐
 │  stop  │   dl   │   rt   │  fair  │  ext   │  idle  │   挑选方向: 从左到右
 └────────┴────────┴────────┴────────┴────────┴────────┘
  stop_task.c:99 deadline.c:3428 rt.c:2595 fair.c:14207 ext.c:4546 idle.c:569
  SCHED_FIFO伪装  SCHED_DEADLINE FIFO/RR    CFS       sched_ext  swapper
  (migration/X,                                              (0号进程)
   cpu_stop)
```

注意两点（与旧内核资料不同）：

1. **sched_ext 的 ext 类在 fair 之下、idle 之上**。scx 只接管 fair 类任务（`__setscheduler_class()` 中 `task_should_scx(policy)`，core.c:7567-7569），永远不会抢 RT/DL；`scx_switched_all()` 时 fair 被跳过、scx 禁用时 ext 被跳过（`next_active_class()`，sched.h:2747-2758）。
2. rt 类由 rt.c:2595 的 `DEFINE_SCHED_CLASS(rt)` 定义，是本文主角。

### 2.2 pick_next_task 的类遍历

`__schedule()` 选下一个进程时按类链从高到低遍历，每个类实现 `pick_task`（7.1.3 已把 `pick_next_task` 拆为 `pick_task` + `set_next_task` 两段）：

```c
// kernel/sched/sched.h:2762-2763
#define for_each_class(class) \
	for_class_range(class, __sched_class_highest, __sched_class_lowest)
```

7.1.3 还引入了 `rq->next_class` 快速路径：`rq_modified_begin()/rq_modified_above()`（sched.h:2774-2781）记录"本 rq 上出现过的最高活跃类"，挑选时从 `next_class` 开始，多数情况下一步命中 fair，无需逐类扫描。对 RT 任务的意义：只要一个 CPU 的 rt_rq 非空，fair/idle 永远没有机会——这正是"RT 可以饿死 CFS"的结构性原因（对策即第 5 节的 throttling）。

策略到类的映射在 `__setscheduler_class()`：

```c
// kernel/sched/core.c:7557
const struct sched_class *__setscheduler_class(int policy, int prio)
{
	if (dl_prio(prio))
		return &dl_sched_class;
	if (rt_prio(prio))
		return &rt_sched_class;
#ifdef CONFIG_SCHED_CLASS_EXT
	if (task_should_scx(policy))
		return &ext_sched_class;
#endif
	return &fair_sched_class;
}
```

---

## 3. rt_rq 数据结构：位图 + 队列数组

### 3.1 两个核心结构

```c
// kernel/sched/sched.h:311
struct rt_prio_array {
	DECLARE_BITMAP(bitmap, MAX_RT_PRIO+1); /* include 1 bit for delimiter */
	struct list_head queue[MAX_RT_PRIO];
};
```

```c
// kernel/sched/sched.h:838
struct rt_rq {
	struct rt_prio_array	active;         // 位图 + 100 条链表
	unsigned int		rt_nr_running;
	unsigned int		rr_nr_running;   // RR 任务数(决定 task_tick 是否重排队)
	struct {
		int		curr;   /* highest queued rt task prio */
		int		next;   /* next highest */
	} highest_prio;                  // 缓存本 rq 最高/次高优先级, pull 时免锁速查
	bool			overloaded;     // RT 过载, 参与 push/pull
	struct plist_head	pushable_tasks; // 可推送任务(按优先级排序的 plist)

	int			rt_queued;      // 顶层 rt_rq 是否挂在 rq 上

#ifdef CONFIG_RT_GROUP_SCHED
	int			rt_throttled;   // ★ 节流标志
	u64			rt_time;        /* consumed RT time */
	u64			rt_runtime;     /* allotted RT time */
	raw_spinlock_t		rt_runtime_lock;
	unsigned int		rt_nr_boosted;  // PI 提升的任务数(提升期间禁止节流)
	struct rq		*rq;
#endif
#ifdef CONFIG_CGROUP_SCHED
	struct task_group	*tg;
#endif
};
```

每个 CPU 的 `struct rq` 内嵌一个顶层 `rt_rq`（sched.h:1184 `struct rt_rq rt;`）：

```
                         struct rq (每CPU)
                              │
                              ▼
 ┌──────────────────────── rt_rq ────────────────────────┐
 │ active.bitmap (101 bit):                              │
 │   bit:   0   1   2   ...        97  98  99  100       │
 │         [1] [0] [0]  ...         [0] [0] [0] [1]      │←100号是哨兵位,
 │          │                                哨兵        保证 sched_find_first_bit
 │          │  __set_bit(prio)                           一定能停在有效位
 │          ▼
 │ active.queue[0..99]:  ┌────┐
 │   queue[0]  (prio 0)  │rt_se│→rt_se→rt_se   FIFO 99 同级链表;
 │   queue[1]  (prio 1)  ├────┤               RR 用尽时间片重新排到队尾
 │   ...                 │    │
 │   queue[98](prio 98)  │    │
 │   queue[99](prio 99,恒空)  │
 └───────────────────────────────────────────────────────┘
 取任务: sched_find_first_bit(bitmap) → queue[idx] 首元素  == O(1)
```

初始化（每位清零、置哨兵位）：

```c
// kernel/sched/rt.c:68
void init_rt_rq(struct rt_rq *rt_rq)
{
	struct rt_prio_array *array;
	int i;

	array = &rt_rq->active;
	for (i = 0; i < MAX_RT_PRIO; i++) {
		INIT_LIST_HEAD(array->queue + i);
		__clear_bit(i, array->bitmap);
	}
	/* delimiter for bitsearch: */
	__set_bit(MAX_RT_PRIO, array->bitmap);   // 哨兵位

	rt_rq->highest_prio.curr = MAX_RT_PRIO-1;  // 99 = "无 RT 任务"
	...
}
```

### 3.2 enqueue_task_rt 走读

```c
// kernel/sched/rt.c:1436
enqueue_task_rt(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_rt_entity *rt_se = &p->rt;

	if (flags & ENQUEUE_WAKEUP)
		rt_se->timeout = 0;

	check_schedstat_required();
	update_stats_wait_start_rt(rt_rq_of_se(rt_se), rt_se);

	enqueue_rt_entity(rt_se, flags);          // ① 挂入优先级链表

	if (task_is_blocked(p))
		return;

	if (!task_current(rq, p) && p->nr_cpus_allowed > 1)
		enqueue_pushable_task(rq, p);      // ② 非当前任务且可迁移 → 进入 pushable 列表
}
```

① 的落点是 `__enqueue_rt_entity()`：

```c
// kernel/sched/rt.c:1331
static void __enqueue_rt_entity(struct sched_rt_entity *rt_se, unsigned int flags)
{
	struct rt_rq *rt_rq = rt_rq_of_se(rt_se);
	struct rt_prio_array *array = &rt_rq->active;
	struct rt_rq *group_rq = group_rt_rq(rt_se);
	struct list_head *queue = array->queue + rt_se_prio(rt_se);  // prio → 链表

	// 组调度: 若所属子组 rt_rq 已被节流, 该组实体不挂载 (组级节流的传导)
	if (group_rq && (rt_rq_throttled(group_rq) || !group_rq->rt_nr_running)) {
		if (rt_se->on_list)
			__delist_rt_entity(rt_se, array);
		return;
	}

	if (move_entity(flags)) {
		WARN_ON_ONCE(rt_se->on_list);
		if (flags & ENQUEUE_HEAD)
			list_add(&rt_se->run_list, queue);      // 唤醒抢占/PI 提升: 插队头
		else
			list_add_tail(&rt_se->run_list, queue); // 普通入队: 排队尾

		__set_bit(rt_se_prio(rt_se), array->bitmap); // 置位图
		rt_se->on_list = 1;
	}
	rt_se->on_rq = 1;

	inc_rt_tasks(rt_se, rt_rq);   // 更新 rt_nr_running / highest_prio /
	                              // overloaded / cpupri(inc_rt_prio_smp, rt.c:1080)
}
```

`dequeue_task_rt()`（rt.c:1455）是对称操作：先 `update_curr_rt()` 结算运行时间（节流检查点之一），再摘链表、清位图、从 pushable 列表移除。

### 3.3 pick_task_rt：O(1) 选取

```c
// kernel/sched/rt.c:1676
static struct sched_rt_entity *pick_next_rt_entity(struct rt_rq *rt_rq)
{
	struct rt_prio_array *array = &rt_rq->active;
	struct sched_rt_entity *next = NULL;
	struct list_head *queue;
	int idx;

	idx = sched_find_first_bit(array->bitmap);   // 位扫描找最高优先级
	BUG_ON(idx >= MAX_RT_PRIO);

	queue = array->queue + idx;
	...
	next = list_entry(queue->next, struct sched_rt_entity, run_list);
	return next;
}
```

```c
// kernel/sched/rt.c:1694
static struct task_struct *_pick_next_task_rt(struct rq *rq)
{
	struct sched_rt_entity *rt_se;
	struct rt_rq *rt_rq  = &rq->rt;

	do {                       // 组调度: 沿子组 rt_rq 下钻
		rt_se = pick_next_rt_entity(rt_rq);
		if (unlikely(!rt_se))
			return NULL;
		rt_rq = group_rt_rq(rt_se);
	} while (rt_rq);

	return rt_task_of(rt_se);
}
```

唤醒抢占回调在 7.1.3 中名为 **`wakeup_preempt_rt`**（rt.c:1619；旧内核叫 `check_preempt_curr_rt`，读旧资料时注意对应关系）：新任务 `prio` 更小则 `resched_curr()`；`prio` 相同且当前任务可迁移时调用 `check_preempt_equal_prio()`（rt.c:1576）——主动制造一次重调度，给 push 逻辑腾出机会把当前任务挤走（把"同级互卡"转化为迁移）。

### 3.4 RT 组调度（CONFIG_RT_GROUP_SCHED）

开启后每个 `task_group` 在**每个 CPU**上都有一个自己的 rt_rq，任务按 cgroup 归属挂到叶子组的 rt_rq，组本身作为 `sched_rt_entity` 挂到父组，形成一棵可节流的层级树：

```
                root_task_group (rt_bandwidth: 950ms/1000ms)
                 │ rt_rq[0] … rt_rq[255]                (EPYC 9996: 256 个)
        ┌────────┴─────────┐
   group A (rt_runtime=400ms)      group B (rt_runtime=500ms)
   rt_rq[cpu] + rt_se[cpu]          rt_rq[cpu] + rt_se[cpu]
        │                               │
   task FIFO50  task RR30           task FIFO80
   (挂组A的rt_rq)                   (挂组B的rt_rq)

 组A超预算 → 组A的rt_rq.rt_throttled=1 → rt_se 从父队列摘除 →
 组A的所有任务(无论内部优先级)整体不可调度; 兄弟组B不受影响
```

- 组创建: `alloc_rt_sched_group()`（rt.c:253），初始 runtime=0，需显式分配；
- 准入: `tg_rt_schedulable()`（rt.c:2665）保证 *子组带宽之和 ≤ 父组带宽*、无人超过全局设置；
- 接口: `cpu.rt_runtime_us` / `cpu.rt_period_us`，**仅 cgroup v1**（core.c:10325 用 `cgroup_add_legacy_cftypes()` 注册，文件表定义 core.c:10283-10293）。

**cgroup v2 没有 RT 带宽接口**——v2 的 cpu 控制器只有 weight/latency/uclamp；如需 RT 组带宽必须回到 v1 的 cpu controller（或用 `rt_group_sched=0` 启动参数禁用组调度，见 core.c:10303 `setup_rt_group_sched`）。`CONFIG_RT_GROUP_SCHED_DEFAULT_DISABLED`（core.c:10297）可在编译期默认关闭。

---

## 4. RT 负载均衡：push / pull 与 cpupri

### 4.1 为什么 RT 不用 CFS 均衡器

CFS 均衡器按 PELT 负载/权重搬任务，追求"总量公平"，是周期性、启发式的；RT 的需求完全不同——**优先级驱动的紧急放置**：优先级 99 的任务醒来时必须立刻找到"当前运行任务优先级最低"的 CPU，而不是"负载最轻"的 CPU。因此 rt.c 自己维护了一套 O(1) 的 push/pull 机制，靠两个数据结构：

- `rt_rq.pushable_tasks`：本 CPU 上"可迁移且未在运行"的 RT 任务（优先级有序 plist，`enqueue_pushable_task` 维护）；
- root_domain 的 **cpupri** 表：全局视角"每个 CPU 当前最高 RT 优先级"。

### 4.2 push_rt_task 流程

push 的触发点（`rt_queue_push_tasks()` 把 `push_rt_tasks` 挂到 balance callback，rt.c:384-389）：

- `set_next_task_rt()`（rt.c:1650，切换到新 RT 任务时）；
- `task_woken_rt()`（rt.c:2367，RT 任务被唤醒但没抢占成功时）；
- `switched_to_rt()`（rt.c:2436，任务刚切到 RT 策略时）；
- `rq_offline_rt()`（rt.c:2392，CPU 下线前把任务推走）。

```
 push_rt_tasks(rq)  rt.c:2071:  while (push_rt_task(rq, false));
   │
   ▼
 push_rt_task(rq, pull=false)  rt.c:1953
   │ ① rq->rt.overloaded? 否则退出 (overload: 本rq RT任务数 > 可用CPU数)
   │ ② pick_next_pushable_task(rq)  rt.c:1859
   │      取 pushable_tasks 中最高优先级且 !on_cpu 的任务
   ▼
 find_lock_lowest_rq(task, rq)  rt.c:1890   最多重试 RT_MAX_TRIES(3) 次
   │ ③ find_lowest_rq(task)  rt.c:1768
   │      ├─ cpupri_find / cpupri_find_fitness → lowest_mask
   │      │     (fitness 版本用于非对称容量, 逐CPU过滤不满足算力的CPU)
   │      ├─ 优先返回 task 上次运行的CPU (cache hot)
   │      └─ 否则沿 sched_domain 找拓扑最近的 CPU (SD_WAKE_AFFINE)
   │ ④ 目标 rq->rt.highest_prio.curr > task->prio ?  (目标确实更低优先级)
   │ ⑤ double_lock_balance() 双锁; 期间任务可能被别人迁移 → 重验
   ▼
 move_queued_task_locked(rq, lowest_rq, next_task)
 resched_curr(lowest_rq)        → 目标 CPU 抢占它的低优先级任务
```

migration_disabled 任务不能直接迁移时，push_rt_task（pull=true 场景）会把**当前正在运行的任务**通过 migration stopper 强推走（`get_push_task()` + `push_cpu_stop`，rt.c:2018-2040）——这是 7.1.3 中 proxy-exec/migrate-disable 相关的强化逻辑。

### 4.3 cpupri：全局 CPU 优先级表

```c
// kernel/sched/cpupri.h
#define CPUPRI_NR_PRIORITIES	(MAX_RT_PRIO+1)   // 101 个桶
#define CPUPRI_INVALID		-1   // CPU 不在线(rq_offline_rt 设置)
#define CPUPRI_NORMAL		 0   // CPU 上只有普通任务
/* values 1-99 are for RT1-RT99 priorities */
#define CPUPRI_HIGHER		100   // DL 任务在跑(压过一切 RT)

struct cpupri_vec {
	atomic_t		count;
	cpumask_var_t		mask;        // 该优先级桶包含哪些 CPU
};

struct cpupri {
	struct cpupri_vec	pri_to_cpu[CPUPRI_NR_PRIORITIES];  // 优先级 → CPU 集合
	int			*cpu_to_pri;                       // CPU → 优先级(反向表)
};
```

内核 `prio`（小者优先）先经 `convert_prio()`（cpupri.c:44）翻转成"cpupri 值大者优先"的桶号：

```
 p->prio      cpupri 桶       含义
   -1     →   100 (HIGHER)    DL 任务
   0..98  →   99..1           RT: 桶号 = 99 - prio (恰为用户视角优先级)
   99     →    0 (NORMAL)     无 RT (只有 CFS/idle)
  不在线  →   -1 (INVALID)

 pri_to_cpu 向量表 (每个桶: count + CPU 位图):
 桶号:    100     99      98    ...    3     2     1      0
        ┌─────┬──────┬──────┬─────┬─────┬─────┬─────┬────────┐
        │ DL  │FIFO99│FIFO98│ ... │ RR3 │ RR2 │FIFO1│NORMAL  │←桶内是"正在运行
        │CPU们│ CPU们 │ CPU们│     │CPU们│     │CPU们│ CPU们  │  该优先级任务"的CPU
        └─────┴──────┴──────┴─────┴─────┴─────┴─────┴────────┘
 cpupri_find(cp, p, lowest_mask)  cpupri.c:122:
   task_pri = convert_prio(p->prio)
   for (idx = 0; idx < task_pri; idx++)      从桶 0 往上扫
       if (桶 idx 有 CPU 且与 p->cpus_mask 有交集)
           lowest_mask = 交集 ∩ cpu_active_mask; return 1
   → 得到"运行着比 p 更低优先级任务"的全部候选 CPU
```

维护点是 `cpupri_set()`（cpupri.c:211）：`rq_online_rt()`（rt.c:2381）与 `inc_rt_prio_smp()`（rt.c:1080）在 CPU 最高优先级变化时更新，`rq_offline_rt()`（rt.c:2392）置 `CPUPRI_INVALID`。复杂度：无亲和性约束的任务两次位搜索即 O(1)；有亲和约束时最坏 O(min(101, nr_domcpus))（cpupri.c:20-23 注释）。

### 4.4 pull_rt_task 与触发时机

pull 是 push 的镜像：低优先级 CPU 主动从过载 CPU"偷"任务。入口 `pull_rt_task(this_rq)`（rt.c:2254）：

1. `rt_overloaded(this_rq)` 为 0 直接返回——过载信息由 `rt_set_overload()` 把本 CPU 置入 root_domain 的 `rto_mask`；
2. 若 `sched_feat(RT_PUSH_IPI)`（features.h:114，默认开），改为 `tell_cpu_to_push()`：向 rto_mask 中的 CPU 逐个投递 IPI，让对方主动 push——避免 pull 方为每个 src_rq 抢锁（arm64 上即用一次 GIC SGI/IPI 的成本换多次锁竞争）；
3. 否则遍历 `rto_mask`：先用 `src_rq->rt.highest_prio.next >= this_rq->rt.highest_prio.curr` **免锁过滤**（对方备胎不比我当前任务高就跳过），再双锁 `pick_highest_pushable_task(src_rq, this_cpu)` 拉回。

pull 的触发时机（`need_pull_rt_task`，rt.c:333：`rq->rt.highest_prio.curr > prev->prio`）：

- `pick_task_rt()` 路径（rt.c:1601）：调度切换发现本 CPU 优先级将下降（高优先级任务运行结束睡去）；
- `prio_changed_rt()`（rt.c:2465）：运行中任务**降**优先级，可能引来更高优先级的任务；
- `switched_to_rt()`（rt.c:2436）等。

---

## 5. RT throttling：带宽节流（重点）

### 5.1 全局参数与记账路径

```c
// kernel/sched/rt.c:18
int sysctl_sched_rt_period = 1000000;   /* 1s, us */
// kernel/sched/rt.c:24
int sysctl_sched_rt_runtime = 950000;   /* 0.95s, us */
```

| sysctl（/proc/sys/kernel/） | 默认 | 含义 |
|---|---|---|
| `sched_rt_period_us` | 1000000 | 节流周期（rt.c:34） |
| `sched_rt_runtime_us` | 950000 | 周期内 RT 配额；**-1 = 关闭节流**（extra1=SYSCTL_NEG_ONE，rt.c:46） |
| `sched_rr_timeslice_ms` | 100 | RR 时间片（rt.c:49；`RR_TIMESLICE`=100*HZ/1000，rt.h:82） |

记账发生在 RT 任务每次让出/被抢占时的 `update_curr_rt()`（调度 tick 也会经 task_tick_rt 调到它）：

```c
// kernel/sched/rt.c:974
static void update_curr_rt(struct rq *rq)
{
	struct task_struct *donor = rq->donor;
	s64 delta_exec;

	if (donor->sched_class != &rt_sched_class)
		return;

	delta_exec = update_curr_common(rq);
	if (unlikely(delta_exec <= 0))
		return;

#ifdef CONFIG_RT_GROUP_SCHED
	struct sched_rt_entity *rt_se = &donor->rt;

	if (!rt_bandwidth_enabled())
		return;

	for_each_sched_rt_entity(rt_se) {          // 沿组层级向上, 每层 rt_rq 都记账
		struct rt_rq *rt_rq = rt_rq_of_se(rt_se);
		int exceeded;

		if (sched_rt_runtime(rt_rq) != RUNTIME_INF) {
			raw_spin_lock(&rt_rq->rt_runtime_lock);
			rt_rq->rt_time += delta_exec;                  // ★ 累计 RT 运行时间
			exceeded = sched_rt_runtime_exceeded(rt_rq);   // ★ 超限检查
			if (exceeded)
				resched_curr(rq);                      // 踢出重调度
			raw_spin_unlock(&rt_rq->rt_runtime_lock);
			if (exceeded)
				do_start_rt_bandwidth(sched_rt_bandwidth(rt_rq)); // 启动周期定时器
		}
	}
#endif /* CONFIG_RT_GROUP_SCHED */
}
```

### 5.2 sched_rt_runtime_exceeded 走读

```c
// kernel/sched/rt.c:863
static int sched_rt_runtime_exceeded(struct rt_rq *rt_rq)
{
	u64 runtime = sched_rt_runtime(rt_rq);

	if (rt_rq->rt_throttled)                      // 已节流: 保持
		return rt_rq_throttled(rt_rq);

	if (runtime >= sched_rt_period(rt_rq))        // runtime >= period: 未启用
		return 0;

	balance_runtime(rt_rq);                       // 尝试向邻居 CPU 借配额
	runtime = sched_rt_runtime(rt_rq);
	if (runtime == RUNTIME_INF)
		return 0;

	if (rt_rq->rt_time > runtime) {               // ★ 超限
		struct rt_bandwidth *rt_b = sched_rt_bandwidth(rt_rq);

		if (likely(rt_b->rt_runtime)) {
			rt_rq->rt_throttled = 1;                       // 置节流标志
			printk_deferred_once("sched: RT throttling activated\n"); // rt.c:887
		} else {
			rt_rq->rt_time = 0;    // 无配额组(被提升)只记账不节流
		}

		if (rt_rq_throttled(rt_rq)) {              // rt_nr_boosted==0 才真正摘除
			sched_rt_rq_dequeue(rt_rq);            // ★ 把整个 rt_rq 摘出 rq
			return 1;
		}
	}

	return 0;
}
```

注意 `rt_rq_throttled()` 的定义（rt.c:564）：`rt_rq->rt_throttled && !rt_rq->rt_nr_boosted`——被 PI（优先级继承）提升的任务所在队列**不会被节流**，否则会造成优先级反转。

被节流的直接后果：`sched_rt_rq_dequeue()` → `dequeue_top_rt_rq()`（rt.c:1011）把 `rt_rq->rt_nr_running` 从 `rq->nr_running` 中减掉、`rt_queued=0`。此后 `sched_rt_runnable(rq)` 为假，**pick 阶段整个 rt 类被跳过**，该 CPU 上的所有 FIFO/RR 任务在周期结束前一个都选不上，只能跑 CFS/idle。

### 5.3 一个周期内的时间线

```
 |←——————————— period = 1000ms ————————————→|
 ┌───────────────────────────────┬───────────┬─────────────────────────────┐
 │ RT 任务可运行 (rt_time 累计)   │  节流 50ms │ 新周期: rt_time 清减, 解除节流 │
 │ 0 ──────────────→ 950ms       │           │ rt_period_timer 到期          │
 └───────────────────────────────┴───────────┴─────────────────────────────┘
      FIFO99 死循环狂奔            RT 不可调度     hrtimer 硬回调
                                   (rt_rq 摘出,    sched_rt_period_timer (rt.c:101)
                                    CFS/交互任务    → do_sched_rt_period_timer (rt.c:778)
                                    终于能跑了)        rt_time -= min(rt_time, overrun*runtime)
                                                      rt_throttled=0
                                                      sched_rt_rq_enqueue() 重新挂回
 950ms 时刻: printk "sched: RT throttling activated" (printk_deferred_once, 只打一次)
```

解节流在周期定时器回调 `do_sched_rt_period_timer()`（rt.c:778）中：遍历 `sched_rt_period_mask()` 的所有 CPU，`rt_rq->rt_time -= min(rt_time, overrun*runtime)`，若已节流且 `rt_time < runtime` 则清 `rt_throttled` 并重新入队。该回调还有一个与隔离直接有关的 FIXME（rt.c:787-794）：**root task group 的 period timer 用的是 `cpu_online_mask`，isolated CPU 并未真正退出 root task group**——官方注释直言"若真要隔离，用户应直接关掉 throttle 以消除它带来的扰动"。

### 5.4 配额借用：do_balance_runtime

`balance_runtime()` → `do_balance_runtime()`（rt.c:608）：超限前先向 root_domain 内其他 rt_rq 借富余配额（每家拿 `spare/weight`，借满一个 period 为止）。受 `SCHED_FEAT(RT_RUNTIME_SHARE, false)`（features.h:117）控制——**7.1.3 默认关闭**，因为跨 CPU 借配额会把节流抖动传染到空闲 CPU（`do_sched_rt_period_timer` 在该 feature 关闭时直接重置 `rt_rq->rt_runtime = rt_b->rt_runtime`，rt.c:809-810）。

### 5.5 案例：RT 进程为何"不能调度"（RT throttle 经典场景）

书籍配套的 §9.3.5"案例分析——为何不能调度"讲的是**另一条独立路径**：不支持内核抢占的内核中，`need_resched` 置位后内核态中断返回（arm64 `el1_irq`）不检查重调度，导致新内核线程永远上不了 CPU——结论是"调度发生在中断返回路径 + `switch_to()` 沿栈帧回溯"。RT throttle 与它同属"任务就绪却选不上"的家族，但根因在调度类自身的带宽机制。下面按"现象→排查→定位→修复"的套路，结合 7.1.3 源码推演一个 RT throttle 的完整案例：

**① 现象**。某数据采集服务器上，工程师用 `chrt -f 80 ./busyloop`（绑单核死循环）做压力测试。几秒后系统交互开始卡顿：ssh 响应数秒、`ls` 偶发挂起、采集进程的日志出现几十毫秒级的缺口；但系统并未死机，`top` 里该 FIFO 进程 CPU 占用只有 ~95%。

**② 初步排查**。卡顿是周期性的、每秒最后一小段最严重；`dmesg` 中出现一条：

```text
sched: RT throttling activated
```

（rt.c:887，`printk_deferred_once` 全内核生命周期只打一次——没有第二条不代表没再发生。）

**③ 定位**。读 `/proc/sys/kernel/sched_rt_runtime_us = 950000`、`sched_rt_period_us = 1000000`：busyloop 一家就用满了单核 950ms 配额。950ms 处 `sched_rt_runtime_exceeded()` 置 `rt_throttled=1` 并 `sched_rt_rq_dequeue()`，此后 50ms 内该核 rt 类整体不可选——同核上其余 RT 任务（含绑核的采集线程）**就绪但不可调度**；如果某内核线程在等待一个由被节流 RT 任务持有的资源，它会以 UN（不可中断睡眠，D 状态族）出现在 `ps` 里——这正是 vol2 §4.12 的排查入口（见 8.4 节 crash 部分）。注意节流粒度是"每 CPU 的 rt_rq"（root task group 在每 CPU 各有一份 rt_rq/rt_time），所以多核部署时每个被 RT 压满的核都会各自节流。

**④ 修复**（按代价从低到高）：

| 方案 | 操作 | 代价/风险 |
|---|---|---|
| 关闭节流 | `echo -1 > /proc/sys/kernel/sched_rt_runtime_us` | RT bug 任务可永久饿死 CFS（含 ksoftirqd/kworker，可能引发 watchdog/RCU stall）；只应配合"RT 任务总配额有把握 < 100%"时使用 |
| 调大配额 | `echo 990000 > sched_rt_runtime_us` | 保留最后 1% 兜底，节流仍可能发生只是更晚 |
| 收敛 RT 用量 | busyloop 降优先级/改 CFS/限流；业务 RT 任务用 SCHED_DEADLINE 表达真实带宽需求 | 需改代码，最正确 |
| 物理+调度隔离 | 压力任务与业务分到不同 CPU/组（`taskset`、cgroup v1 `cpu.rt_runtime_us`、cpuset 隔离），隔离核单独关节流，见 7.5 节 | 部署复杂 |

另有两点高频误会：

- **`!CONFIG_RT_GROUP_SCHED` 内核不会节流 RT**：7.1.3 中 `update_curr_rt` 的记账块在 `#ifdef CONFIG_RT_GROUP_SCHED` 内，无组调度时 `rt_rq_throttled()` 恒为 false（rt.c:937 附近的桩实现），sysctl 文件虽在但只影响 DEADLINE 准入（sched-deadline.rst §4.1：'With !CONFIG_RT_GROUP_SCHED the knob only serves for the -dl admission control'）；
- **节流不影响优先级语义**：解除节流后 FIFO 任务依旧按位图 O(1) 立即恢复运行，`rt_time` 由 period timer 清减，不需要任务自己做任何事。

### 5.6 RT 组带宽（cgroup v1 only）

见 3.4 节结构图。要点：`cpu.rt_runtime_us`/`cpu.rt_period_us` 是 **legacy（v1）cftype**（core.c:10325 `cgroup_add_legacy_cftypes(&cpu_cgrp_subsys, rt_group_files)`）；分配时经 `tg_set_rt_bandwidth()` → `__rt_schedulable()`（rt.c:2722）→ `walk_tg_tree(tg_rt_schedulable)`（rt.c:2665）做全树可调度性检查（子树之和不超过父配额、任何人不超过全局）。**cgroup v2 至今没有 RT 带宽控制文件**，这是 v1/v2 功能差异中与 RT 最直接相关的一条。

---

## 6. SCHED_DEADLINE：EDF + CBS

### 6.1 三元组与数据结构

```c
// include/linux/sched.h:644
struct sched_dl_entity {
	struct rb_node			rb_node;     // 挂入 dl_rq 红黑树(按绝对截止期排序)

	u64				dl_runtime;	/* 每实例最大运行时 */
	u64				dl_deadline;	/* 相对截止期 */
	u64				dl_period;	/* 周期 */
	u64				dl_bw;		/* dl_runtime / dl_period */
	u64				dl_density;	/* dl_runtime / dl_deadline */

	s64				runtime;	/* 本实例剩余运行时(可为负=超支) */
	u64				deadline;	/* 绝对截止期 */
	...
};
```

```
 调度器视角(一个周期内):

   t0 ──────── runtime ────────►│              │◄──── period ────►
   ├────────────────────────────┼──────────────┤
   │   CBS 服务器保证的执行预算   │  耗尽后节流    │  dl_task_timer 到期
   │   (dl_runtime)              │  (throttled)  │  replenish: runtime += dl_runtime
   │                             │               │  deadline += dl_period
   └──────────── dl_deadline ────┤
                                 ▲ 绝对截止期 = t0 + dl_deadline, EDF 以它排序
```

- **EDF**：dl_rq 是按 `deadline` 排序的红黑树（`struct dl_rq`，sched.h:872），选最左节点——截止期最早者先跑，天然支持不同周期任务的合理交错；
- **CBS**（Constant Bandwidth Server）：即使任务高估/低估执行时间，服务器按自己的参数补充/节流，任务间带宽隔离——A 超支不会偷 B 的预算（sched-deadline.rst §2.1）。

### 6.2 准入控制（admission control）

按 sched-deadline.rst §4：对 M 个 CPU 的 root_domain，所有 DL 任务满足

```text
\Sum (runtime_i / period_i)  <  M * (sched_rt_runtime_us / sched_rt_period_us)
```

代码路径：`sched_setattr()` → `__checkparam_dl()`（deadline.c:3673，参数合法性：runtime>0、deadline>0、period>=deadline 等）→ `sched_dl_overflow()`（deadline.c:3560）：

```c
// kernel/sched/deadline.c:207
static inline bool
__dl_overflow(struct dl_bw *dl_b, unsigned long cap, u64 old_bw, u64 new_bw)
{
	return dl_b->bw != -1 &&
	       cap_scale(dl_b->bw, cap) < dl_b->total_bw - old_bw + new_bw;
}
```

`cap` 来自 `dl_bw_capacity()`（deadline.c:153；对称容量系统 = `dl_bw_cpus() << SCHED_CAPACITY_SHIFT`，非对称系统用 `__dl_bw_capacity()` 对 rd->span 内 CPU 求和算力，deadline.c:138）。超了就 `-EBUSY`，**任务根本进不了 DL 类**——这与 RT throttling"先进来、超了再罚"的哲学相反：DL 是事前准入，RT 是事后节流。全局 `-1` 写入 `sched_rt_runtime_us` 同样可关闭 DL 准入（sched-deadline.rst §4.1）。

### 6.3 运行时节流与补充

`update_curr_dl_se()`（deadline.c:1416）扣减 `runtime`；耗尽后置 `dl_throttled`、摘出队列，由 `dl_task_timer`（deadline.c:1210，每实体一个 hrtimer，`init_dl_task_timer` deadline.c:1305）在补充点调用 `replenish_dl_entity()`（deadline.c:799）恢复 `runtime`/推进 `deadline`。`update_dl_entity()`（deadline.c:1013）在入队/唤醒时决定是沿旧服务器周期还是开新周期（constrained deadline 走 Revised CBS）。DL 同样有 push/pull：`find_later_rq()`（deadline.c:2716）+ **cpudl**——按各 CPU 最早截止期组织的最大堆（`struct cpudl`，cpudeadline.h:13，`cpudl_find()` cpudeadline.c:117），逻辑与 RT 的 cpupri 同构，只是"优先级"换成"截止期"。

### 6.4 FIFO vs DEADLINE 选型

| 维度 | SCHED_FIFO/RR | SCHED_DEADLINE |
|---|---|---|
| 表达能力 | 单一优先级，无时间概念 | 周期/预算/截止期三元组 |
| 多任务共存 | 同核多 RT 任务靠优先级硬排，易互相饿死 | EDF 数学上可调度率更高（implicit deadline 下接近 M） |
| 过载保护 | RT throttling 事后节流（950ms/1s） | 准入控制事前拒绝 + CBS 隔离 |
| 唤醒延迟 | 最高（同级立即抢） | 取决于相对截止期，可能被更紧急的 DL 压住 |
| 权限 | RLIMIT_RTPRIO 可放权（syscalls.c:442） | 必须 CAP_SYS_NICE（syscalls.c:453） |
| 适合 | 单一关键中断线程、内核线程、延迟极敏感的小任务 | 周期性控制环、音视频管线等有明确 QoS 预算的任务 |

经验法则：**能写出"每 T ms 需要 C ms"的任务用 DEADLINE；纯粹"我最重要"的用 FIFO；需要同级轮转的用 RR。**

---

## 7. EPYC 9996 上的实时部署（重点）

### 7.1 平台事实

| 项 | 值 |
|---|---|
| 型号 | AMD EPYC 9996x（9006 系列，"Venice"） |
| 核/线程 | 256C / 512T（Zen 6c，SMT2） |
| 频率 | 基频 2.55GHz，加速 4.1GHz，TDP 600W |
| 封装 | 8 CCD + 2 IOD；每 CCD = 2 CCX × 16 核；全片 16 CCX |
| 缓存/内存 | 每 CCX 共享 64MB L3（全片 1GB）；16 通道 DDR5 |
| NUMA | NPS1/2/4 可配；NPS4 = 4 节点 × 2 CCD = 64C/节点 |
| 内核识别 | Linux 7.1.3 已识别 Zen6 拓扑；cpuset v2 支持 isolated partition（`PRS_ISOLATED`，kernel/cgroup/cpuset.c:208） |

### 7.2 拓扑与抖动来源

256 核下 RT 任务的确定性敌人不再是"算力不足"，而是**共享资源与后台噪声**：

```
 EPYC 9996 (NPS4 视角, 每节点 64C/128T; SMT 线程略去)
 ┌────────────────────────── Package ──────────────────────────┐
 │  NUMA0 (2 CCD)        NUMA1 (2 CCD)   NUMA2    NUMA3        │
 │ ┌───────────────┐   ┌───────────────┐                       │
 │ │ CCD0          │   │ CCD2          │                       │
 │ │ ┌───────────┐ │   │ ┌───────────┐ │                       │
 │ │ │CCX0: 16C  │ │   │ │CCX4: 16C  │ │   每个 CCX 独享 64MB L3 │
 │ │ │共享64MB L3│ │   │ │共享64MB L3│ │                       │
 │ │ └───────────┘ │   │ └───────────┘ │                       │
 │ │ ┌───────────┐ │   │ ┌───────────┐ │                       │
 │ │ │CCX1: 16C  │ │   │ │CCX5: 16C  │ │                       │
 │ │ └───────────┘ │   │ └───────────┘ │                       │
 │ └───────────────┘   └───────────────┘                       │
 │        └── 16ch DDR5 经 IOD; 跨 NUMA 访存要过 IOD 互联        │
 └─────────────────────────────────────────────────────────────┘
 抖动来源(按量级从大到小):
  1. 跨 NUMA 访存(远端 DDR5)        → RT 任务与其工作集必须同 NUMA
  2. 跨 CCX 的 L3 未命中(远端 L3 探测) → 绑定到任务热数据所在的 CCX
  3. SMT 兄弟线程的干扰(共享执行端口/L1/L2) → 关键核只跑单线程或隔离 SMT 兄弟
  4. 内核噪声: tick、RCU、workqueue、内核线程、内存回收
  5. 中断: 设备 IRQ 落到 RT 核
```

### 7.3 延迟域划分：housekeeping + isolated + RT 绑定

以 NPS4 为例，把 NUMA0 的 4 个 CCX 拆成"管家域 + 隔离域"：

```
 NUMA0 (64C, CCD0-1, 4 个 CCX)              其余 NUMA1~3 = 普通业务域
 ┌──────────────────────────────────┐
 │ CCX0 (CPU 0-15)   housekeeping    │  ← 所有内核噪声的汇聚地:
 │ CCX1 (CPU 16-31)  housekeeping    │     unbound workqueue/timer/RCU callback,
 │                                   │     网卡/磁盘 IRQ (irqaffinity), OS 自身
 │ CCX2 (CPU 32-47)  ISOLATED (RT)   │  ← cpuset v2 isolated partition:
 │   - CPU 32-39: RT 采集线程        │     cgroup.subtree_control=+cpuset
 │     (FIFO 80, taskset 绑核)       │     cpuset.cpus.partition = isolated
 │   - CPU 40-47: RT 控制环          │     (PRS_ISOLATED, cpuset.c:208)
 │     (DEADLINE 2/10/10ms)          │     调度域被拆除: 无 load balance
 │ CCX3 (CPU 48-63)  ISOLATED (RT)   │
 │   - 延迟敏感的低带宽轮询           │
 └──────────────────────────────────┘
 规则:
  - 同 CCX 的 RT 线程互相共享 64MB L3, 邻居噪声可控; housekeeping 与
    isolated 不同 CCX, 共享的只有 IOD/内存控制器
  - RT 任务的内存用 numactl --membind=0 固定在 NUMA0
```

对应启动参数（arm64 服务器对照同理，只是 IRQ 控制器从 x2apic 换成 GICv3）：

```text
isolcpus=managed_irq,32-63 nohz_full=32-63 rcu_nocbs=32-63 \
irqaffinity=0-31,64-255
```

### 7.4 cpuset v2 isolated partition vs nohz_full vs isolcpus

三者层次不同、常配合使用：

| 机制 | 层面 | 7.1.3 落点 | 效果 |
|---|---|---|---|
| `isolcpus=` | 启动参数，调度域 | [kernel/sched/isolation.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/isolation.c)（housekeeping 管理） | 把 CPU 移出调度域，CFS 负载均衡不再碰它；`managed_irq` 顺带挪走托管中断 |
| cpuset v2 `cpuset.cpus.partition = isolated/root` | cgroup，调度域（可运行时切换） | kernel/cgroup/cpuset.c:208 `#define PRS_ISOLATED 2`；cgroup-v2.rst:2600 | 运行时把分区 CPU 移出调度域；`isolated` 比 `root` 拆得更干净（isolated 分区无调度域结构） |
| `nohz_full=` | 启动参数，tick | isolation.c + time/tick-sched | 仅 1 个任务在跑时停掉周期 tick（默认每 1/HZ 一次的心跳消失），tick 退到 housekeeping 核 |
| `rcu_nocbs=` | RCU 回调 | isolation.c（HK 类型） | RCU callback 移出本核 |
| `irqaffinity=` | 中断亲和 | 互补启动参数 | 未绑死的设备 IRQ 默认落在 housekeeping 核 |

housekeeping 的统一抽象在 isolation.c：`enum hk_flags`（isolation.c:14-19，含 `HK_FLAG_DOMAIN`/`HK_FLAG_MANAGED_IRQ`/`HK_FLAG_KERNEL_NOISE`）+ `housekeeping_cpumask()`（isolation.c:64）——isolcpus/nohz_full 都汇入同一套 housekeeping 掩码。

### 7.5 隔离核上的 RT throttling

结合 5.3 节的 FIXME（rt.c:787-794）：root task group 的 period timer 用 `cpu_online_mask` 做 span，隔离 CPU 并没有真正退出 root 组。工程惯例因此是：**给隔离核关节流，给业务核保留节流**——但 sysctl 是全局变量，无法按核设置；可行做法：

- `sched_rt_runtime_us=-1` 全局关闭，同时用 cgroup v1 `cpu.rt_runtime_us` 给业务分组限带宽（v1/v2 可在同一系统混挂，RT 组文件挂 v1 层级）；
- 或保持 950ms/1s：反正隔离核上的 RT 任务配额就是"每秒 950ms"，业务核照常受保护。若 RT 任务本就是周期性的（每 10ms 跑 2ms），950ms 上限根本碰不到，throttling 无感——这也是把压力型 RT 任务（死循环）与业务型 RT 任务分域的又一理由。

### 7.6 中断与定时器：arm64 对照

RT 核不可能完全没有中断，EPYC 9996（x2apic/AMD IOMMU）与 arm64 服务器（GICv3 + arch timer）的最小中断集合同构：

- **本地定时器中断**（arm64：arch timer 私有中断 PPI，计数器读数见 [drivers/clocksource/arm_arch_timer.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/drivers/clocksource/arm_arch_timer.c) 的 `arch_counter_get_cntvct`；EPYC：local APIC timer）：`nohz_full` 后仅保留单射 hrtimer——这是 DL 任务 refill timer 与 RT period timer 的载体；
- **IPI**（arm64：GICv3 SGI，如 `tell_cpu_to_push()` 的 RT_PUSH_IPI；EPYC：x2apic self-IPI）：push/pull 负载均衡本身就会向 RT 核发 IPI——`RT_PUSH_IPI` 特性默认开启（features.h:114）；
- **功能中断**（timer broadcast、CMCI、IOMMU 事件等）：`isolcpus=managed_irq` + `irqaffinity=` 尽量挪走；arm64 上还可用 GICv3 的中断优先级/PMR 机制配合内核的 `gic_write_pmr()`（[arch/arm64/kernel/smp.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/arch/arm64/kernel/smp.c) 等）控制中断屏蔽层级。

书 §8.5.2 的结论在此适用：要压缩中断/调度延时，工具链是 ftrace 的 `preemptirqsoff`/`wakeup_rt` tracer（测量关中断/关抢占时长与唤醒→运行延时）与 cyclictest/rteval（统计抖动）——在 9996 这样 256 核的平台上，先用它们区分"调度器问题"还是"中断/锁问题"，再决定继续调哪一层。

### 7.7 异构大小核（Cortex-A725/A520）上的 RT

RT 调度类本身**不感知容量**（优先级/时间片与算力无关）——FIFO 任务落在小核上只是"跑得慢"，调度器无感。但 7.1.3 在 **push 路径**上为异构系统加了两道容量闸门（同构的 AGI/EPYC 9996 上 `sched_asym_cpucap_active()` 恒 false，全部跳过）：

```c
// kernel/sched/rt.c:455 (仅异构系统生效; 无 CONFIG_UCLAMP_TASK 时恒 true)
static inline bool rt_task_fits_capacity(struct task_struct *p, int cpu)
{
	if (!sched_asym_cpucap_active())          // 只有 big.LITTLE 这类系统才做检查
		return true;
	min_cap = uclamp_eff_value(p, UCLAMP_MIN);
	max_cap = uclamp_eff_value(p, UCLAMP_MAX);
	cpu_cap = arch_scale_cpu_capacity(cpu);
	return cpu_cap >= min(min_cap, max_cap);  // 核容量 >= 任务要求的下限
}

// kernel/sched/rt.c:1789 (find_lowest_rq: 把容量检查作为 fitness 传入)
	ret = cpupri_find_fitness(&task_rq(task)->rd->cpupri, task, lowest_mask,
				  rt_task_fits_capacity);   // cpupri.c:145
```

关键默认值：RT 任务的 uclamp 下限默认拉满——`sysctl_sched_uclamp_util_min_rt_default = SCHED_CAPACITY_SCALE`（1024，[core.c:1562](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/core.c)）：

```
 Cortex-A725(大核, cap=1024) + A520(小核, cap≈252) 上的 push 寻位:

   FIFO 任务 (uclamp_min 默认 1024)
        |
        v
   find_lowest_rq -> cpupri_find_fitness
        |                |
        |   A725: cap 1024 >= 1024  -> fitness=true   -> 进 lowest_mask
        |   A520: cap 252  <  1024  -> fitness=false  -> 被排除!
        v
   push 目标只在大核簇里挑; 小核对默认 RT 任务"装不下"
   (唤醒路径同样复查: rt.c:1550/1557)
```

- 想让轻量 RT（如低频看门狗）允许落小核：调低 `/proc/sys/kernel/sched_uclamp_util_min_rt_default`（或对该任务设 `util_min` uclamp）——1024 → 252 以下时 A520 重新进入 lowest_mask；
- **SCHED_DEADLINE 不享此待遇**：准入控制 `\Sum(runtime/period) < M` 按 **CPU 时间**计，不看算力——同一任务在 A520 上要占更长 wall time 才能算完，deadline 更容易破，需在三元组里自行放大 runtime 预留（sched-deadline.rst 对小核的提示）；
- 对照：AGI（136×V3 同构 1024）与 EPYC 9996（256×Zen 6c 同构）上无此分化，push/pull 退化为纯优先级问题。

---

## 8. 调度策略设置与调试

### 8.1 设置接口

```c
// kernel/sched/syscalls.c:758
int sched_setscheduler(struct task_struct *p, int policy,
		       const struct sched_param *param);
// sched_setattr() 则走 sched_attr, 可携带 DEADLINE 三元组
```

```text
# FIFO 优先级 80
chrt -f 80 ./rt-collect
# RR 优先级 50, 绑核 32
chrt -r 50 taskset -c 32 ./rt-poll
# DEADLINE: runtime=2ms deadline=10ms period=10ms
chrt -d --sched-runtime 2000000 --sched-deadline 10000000 \
        --sched-period 10000000 taskset -c 40 ./rt-loop
# 查看
chrt -p $(pidof rt-collect)      # pid XXX, prio 80, policy SCHED_FIFO
```

权限模型（`user_check_sched_setscheduler()`，syscalls.c:435）：非特权用户受 `RLIMIT_RTPRIO`（只能在该限内升 RT 优先级）与 `RLIMIT_NICE` 约束；DL 一律 `CAP_SYS_NICE`；`SCHED_FLAG_RESET_ON_FORK` 防止特权属性泄漏给子进程。

### 8.2 观测点速查

| 位置 | 内容 |
|---|---|
| `/proc/sys/kernel/sched_rt_period_us` / `sched_rt_runtime_us` / `sched_rr_timeslice_ms` | 全局 RT 带宽与 RR 时间片（rt.c:32-57 的 sysctl 表） |
| `/sys/kernel/debug/sched/features` | `RT_RUNTIME_SHARE`（默认 false）、`RT_PUSH_IPI`（默认 true）等（features.h:114/117） |
| `dmesg` | `sched: RT throttling activated`（rt.c:887，只打一次） |
| `/proc/sched_debug`、`/sys/kernel/debug/sched/debug` | 每 CPU `rt_rq` 的 `rt_nr_running`/`rt_throttled`/`rt_time`/`rt_runtime`（print_rt_rq） |
| tracepoints: `sched/sched_wakeup`、`sched/sched_switch`（[include/trace/events/sched.h:178](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/trace/events/sched.h)） | 唤醒→上 CPU 延时（wakeup tracer） |

### 8.3 ftrace 定位 RT throttle

```text
# 1) 谁在节流: kprobe 跟踪 sched_rt_runtime_exceeded
cd /sys/kernel/tracing
echo 'p:throttle kernel/sched/rt.c:sched_rt_runtime_exceeded' > kprobe_events
echo 1 > events/kprobes/throttle/enable
echo 1 > tracing_on
# 触发后: cat trace | grep throttle   → 每条即一次"超限检查命中"

# 2) 函数图确认节流后的 50ms 空窗: 看该 CPU 上 sched_switch 全是普通任务
echo 1 > events/sched/sched_switch/enable
grep '"rt-collect"' trace | tail    # 时间戳间隔出现 ~50ms 的空洞

# 3) 唤醒延迟统计 (书 §8.5.2 的 ftrace 思路; preemptirqsoff 找关中断/关抢占过长)
echo wakeup_rt > current_tracer    # RT 任务专属的唤醒延迟 tracer
cat tracing_max_latency
```

结合 5.5 节案例：kprobe 命中 + dmesg 打印 + sched_switch 空窗三者对齐，即可把"周期性卡顿"钉死在 throttle 上，而不是驱动或锁。

### 8.4 crash 离线排查思路（RT 进程 D 状态栈）

活体系统无解时（比如节流的 RT 任务持有锁导致别的任务长期 UN/D），用 kdump + crash（vol2 第4章）：

```text
# 1) 找不可中断任务 (vol2 §4.12: ps | grep UN)
crash> ps | grep UN
   5522  2165  2  ffff94940f64b0c0  UN  0.1  153192  1896 test

# 2) 批量导出它们的栈, 统计卡在哪 (§4.12 foreach 技巧)
crash> foreach UN bt > un.log
crash> grep -c __schedule un.log

# 3) 单看嫌疑任务: 在等谁的锁
crash> bt 5522
   #0 ... __schedule
   #1 ... schedule
   #2 ... rt_mutex_slowlock        ← 在等 RT mutex
   ...

# 4) 检查持有者是不是被节流的 RT 任务: 读 rt_rq 与任务状态
crash> task -R policy,prio,rt_priority 5523        # 持锁者优先级/策略
crash> struct rt_rq rt_throttled,rt_time,rt_runtime <rq地址>   # 所在CPU的rt_rq
crash> runq -t                                     # 每 CPU 最后运行时间戳
crash> ps -l                                       # 每任务时间戳, 估算被阻塞时长
# 若持锁者 policy=SCHED_FIFO 且其 CPU 的 rt_rq.rt_throttled=1 →
# 锁被节流冻结, 等锁者成 D; 修复方向回到 5.5 节表格
```

`runq -t` 与 `ps -l` 配合能估算节流空窗时长——这与 vol2 §4.12"通过时间戳计算进程被阻塞了多长时间"是同一手法。

---

## Q&A

**Q1：RT 任务把 CFS 饿死了怎么办？**
先看是否已触发节流（`dmesg` 的 "sched: RT throttling activated"）：默认 950ms/1s 配额下 CFS 至少还有每秒 50ms 活命，通常表现为周期性卡顿。根治要回到 RT 任务本身——它按设计本就不该长期占满 CPU；周期性任务迁到 SCHED_DEADLINE（准入控制会拒绝超卖），常驻线程限流或降级；或者把 RT 与业务分域隔离（第 7 节）。

**Q2：为什么需要 RT throttling？既然是实时系统，100% 给 RT 不行吗？**
RT 任务可能有 bug（死循环）或配置错误，而内核的 migration 线程自身也是（伪装的）RT 任务；ksoftirqd、RCU、watchdog 若被饿死，轻则停摆重则触发 soft lockup panic。throttling 是"RT 优先级模型"的最后防线：宁可让 RT 任务在每周期尾部饿 50ms，也不让整个机器失去服务能力。代价是它给"正确的" RT 任务也引入了最坏 50ms 的调度黑洞——所以第 7 节的隔离方案里常对隔离核直接 `-1` 关闭它。

**Q3：DL 与 FIFO 选谁？**
见 6.4 表。一句话：需要"每 period 保证 runtime"这种带宽语义、且任务确实是周期性的，用 DEADLINE，它的 CBS 隔离比 FIFO 的裸优先级安全得多；任务是事件驱动（中断下半部、紧急唤醒）且极短，用 FIFO。两者可共存，dl 类整体压过 rt 类（core.c:8909）。

**Q4：RT 任务为什么不做负载均衡，push/pull 不会漏吗？**
因为 RT 的正确性判据是"优先级最高的可运行任务必须立刻在某个 CPU 上运行"，与负载均分无关。push（在事件点主动推）+ pull（低优先级 CPU 顺手拉）+ rto_mask 过载位图 + cpupri 全局视图共同保证：漏推的情况会被 pull 兜底，`highest_prio.next` 的免锁预检让 pull 的代价接近零。CFS 均衡器的 PELT 时间平均语义反而会引入不可接受的迁移延迟。

**Q5：cgroup v2 里怎么限制一组 RT 任务的 CPU 带宽？**
现在不能。RT 组带宽接口（`cpu.rt_runtime_us`/`cpu.rt_period_us`）是 v1 legacy 文件（core.c:10325）。v2 下只有三条路：全局 sysctl、把该组任务改造成 SCHED_DEADLINE 用准入控制表达带宽、或混挂 v1 层级专管 RT 组。

---

## 参考资料

- 书籍:
  - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub#8.5　实时调度](../../../../007.BOOKs/Run%20Linux%20Kernel%20%282nd%20Edition%29%20Volume%201:%20Infrastructure.epub)（软/硬实时、实时延时分解、实时性改进时间线、ftrace/latencytop/cyclictest）
  - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub#9.3.5　案例分析——为何不能调度](../../../../007.BOOKs/Run%20Linux%20Kernel%20%282nd%20Edition%29%20Volume%201:%20Infrastructure.epub)（内核抢占与中断返回路径的"不能调度"案例；与本文 5.5 节 RT throttle 案例互补）
  - [Run Linux Kernel (2nd Edition) Volume 2: Debugging and Case Analysis.epub#4.12 关于Crash工具的调试技巧汇总](../../../../007.BOOKs/Run%20Linux%20Kernel%20%282nd%20Edition%29%20Volume%202:%20Debugging%20and%20Case%20Analysis.epub)（ps/foreach/task/runq 调试技巧）
- 内核文档:
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/scheduler/sched-rt-group.rst](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/scheduler/sched-rt-group.rst)
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/scheduler/sched-deadline.rst](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/scheduler/sched-deadline.rst)
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/admin-guide/cgroup-v2.rst](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/Documentation/admin-guide/cgroup-v2.rst)（cpuset.cpus.partition / isolated）
- 源码（Linux 7.1.3）:
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/rt.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/rt.c)
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/cpupri.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/cpupri.c) / [cpupri.h](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/cpupri.h)
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/deadline.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/deadline.c) / [cpudeadline.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/cpudeadline.c) / [cpudeadline.h](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/cpudeadline.h)
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/core.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/core.c) / [syscalls.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/syscalls.c) / [sched.h](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/sched.h) / [isolation.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/isolation.c) / [features.h](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/features.h) / [idle.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/idle.c) / [stop_task.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/stop_task.c) / [ext.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/sched/ext.c)
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/cgroup/cpuset.c](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/kernel/cgroup/cpuset.c)
  - [000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched/prio.h](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched/prio.h) / [include/linux/sched.h](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched.h) / [include/linux/sched/rt.h](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/linux/sched/rt.h) / [include/trace/events/sched.h](../../../../000.SOURCE_CODE/003.LINUX-7.1.3/000.LINUX-7.1.3/include/trace/events/sched.h)
- AMD: <https://www.amd.com/en/products/processors/server/epyc/9006-series/amd-epyc-9996.html>
