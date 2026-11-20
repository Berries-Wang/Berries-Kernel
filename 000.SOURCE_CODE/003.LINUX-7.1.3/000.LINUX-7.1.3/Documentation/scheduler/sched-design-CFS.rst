.. _sched_design_CFS:

=========================
CFS Scheduler(CFS 调度器)
=========================


1.  OVERVIEW(1. 概述)
=====================

CFS stands for "Completely Fair Scheduler," and is the "desktop" process
scheduler implemented by Ingo Molnar and merged in Linux 2.6.23. When
originally merged, it was the replacement for the previous vanilla
scheduler's SCHED_OTHER interactivity code. Nowadays, CFS is making room
for EEVDF, for which documentation can be found in
Documentation/scheduler/sched-eevdf.rst.
(CFS 即"完全公平调度器" (Completely Fair Scheduler)，是由 Ingo Molnar 实现并
在 Linux 2.6.23 中合入的"桌面"进程调度器。刚合入时，它是之前原版调度器
SCHED_OTHER 交互性代码的替代品。如今，CFS 正在为 EEVDF 让位，相关文档见
Documentation/scheduler/sched-eevdf.rst。)

80% of CFS's design can be summed up in a single sentence: CFS basically models
an "ideal, precise multi-tasking CPU" on real hardware.
(CFS 的设计有 80% 可以用一句话概括：CFS 基本上是在真实硬件上对一个"理想、精确
的多任务 CPU"建模。)

"Ideal multi-tasking CPU" is a (non-existent  :-)) CPU that has 100% physical
power and which can run each task at precise equal speed, in parallel, each at
1/nr_running speed.  For example: if there are 2 tasks running, then it runs
each at 50% physical power --- i.e., actually in parallel.
("理想多任务 CPU"是一种(并不存在 :-)的)CPU，它拥有 100% 的物理算力，能够以
精确相等的速度并行运行每个任务，每个任务各占 1/nr_running 的速度。例如：如果
有 2 个任务在运行，那么它以各 50% 的物理算力运行它们——也就是说，真正并行。)

On real hardware, we can run only a single task at once, so we have to
introduce the concept of "virtual runtime."  The virtual runtime of a task
specifies when its next timeslice would start execution on the ideal
multi-tasking CPU described above.  In practice, the virtual runtime of a task
is its actual runtime normalized to the total number of running tasks.
(在真实硬件上，我们一次只能运行单个任务，因此必须引入"虚拟运行时间
(virtual runtime)"的概念。任务的虚拟运行时间指明它的下一个时间片会在上述理想
多任务 CPU 上何时开始执行。实践中，任务的虚拟运行时间就是其实际运行时间按运行
任务总数归一化后的结果。)



2.  FEW IMPLEMENTATION DETAILS(2. 少量实现细节)
===============================================

In CFS the virtual runtime is expressed and tracked via the per-task
p->se.vruntime (nanosec-unit) value.  This way, it's possible to accurately
timestamp and measure the "expected CPU time" a task should have gotten.
(在 CFS 中，虚拟运行时间通过每任务的 p->se.vruntime(纳秒单位)值来表示和跟踪。
这样，就可以准确地打时间戳并度量任务本应获得的"期望 CPU 时间"。)

   Small detail: on "ideal" hardware, at any time all tasks would have the same
   p->se.vruntime value --- i.e., tasks would execute simultaneously and no task
   would ever get "out of balance" from the "ideal" share of CPU time.
   (小细节：在"理想"硬件上，任何时刻所有任务的 p->se.vruntime 值都相同——
   也就是说，任务会同时执行，任何任务都不会偏离其"理想"的 CPU 时间份额。)

CFS's task picking logic is based on this p->se.vruntime value and it is thus
very simple: it always tries to run the task with the smallest p->se.vruntime
value (i.e., the task which executed least so far).  CFS always tries to split
up CPU time between runnable tasks as close to "ideal multitasking hardware" as
possible.
(CFS 的任务挑选逻辑基于 p->se.vruntime 值，因此非常简单：它总是尝试运行
p->se.vruntime 值最小的任务(即到目前为止执行得最少的任务)。CFS 总是尽力把
CPU 时间在可运行任务之间划分得尽可能接近"理想多任务硬件"。)

Most of the rest of CFS's design just falls out of this really simple concept,
with a few add-on embellishments like nice levels, multiprocessing and various
algorithm variants to recognize sleepers.
(CFS 设计的其余部分大多由这个非常简单的概念自然引出，只附加了少量点缀，例如
nice 级别、多处理，以及用于识别休眠者的各种算法变体。)



3.  THE RBTREE(3. 红黑树)
=========================

CFS's design is quite radical: it does not use the old data structures for the
runqueues, but it uses a time-ordered rbtree to build a "timeline" of future
task execution, and thus has no "array switch" artifacts (by which both the
previous vanilla scheduler and RSDL/SD are affected).
(CFS 的设计相当激进：它不使用旧式的运行队列数据结构，而是使用按时间排序的
红黑树来构建未来任务执行的"时间线"，因此不存在"数组切换"造成的缺陷(之前的
原版调度器和 RSDL/SD 都受其影响)。)

CFS also maintains the rq->cfs.min_vruntime value, which is a monotonic
increasing value tracking the smallest vruntime among all tasks in the
runqueue.  The total amount of work done by the system is tracked using
min_vruntime; that value is used to place newly activated entities on the left
side of the tree as much as possible.
(CFS 还维护 rq->cfs.min_vruntime 值，它是一个单调递增的值，跟踪运行队列中所有
任务里最小的 vruntime。系统完成的总工作量用 min_vruntime 来跟踪；该值用于把
新激活的实体尽可能放在树的左侧。)

The total number of running tasks in the runqueue is accounted through the
rq->cfs.load value, which is the sum of the weights of the tasks queued on the
runqueue.
(运行队列中运行任务的总数通过 rq->cfs.load 值来统计，它是排在运行队列上的任务
权重之和。)

CFS maintains a time-ordered rbtree, where all runnable tasks are sorted by the
p->se.vruntime key. CFS picks the "leftmost" task from this tree and sticks to it.
As the system progresses forwards, the executed tasks are put into the tree
more and more to the right --- slowly but surely giving a chance for every task
to become the "leftmost task" and thus get on the CPU within a deterministic
amount of time.
(CFS 维护一棵按时间排序的红黑树，所有可运行任务按 p->se.vruntime 键排序。CFS
从这棵树中挑出"最左边"的任务并紧抓着它不放。随着系统向前推进，已执行的任务
在树中被放得越来越靠右——缓慢但坚定地让每个任务都有机会成为"最左边的任务"，
从而在确定的时间内获得 CPU。)

Summing up, CFS works like this: it runs a task a bit, and when the task
schedules (or a scheduler tick happens) the task's CPU usage is "accounted
for": the (small) time it just spent using the physical CPU is added to
p->se.vruntime.  Once p->se.vruntime gets high enough so that another task
becomes the "leftmost task" of the time-ordered rbtree it maintains (plus a
small amount of "granularity" distance relative to the leftmost task so that we
do not over-schedule tasks and trash the cache), then the new leftmost task is
picked and the current task is preempted.
(概括起来，CFS 的工作方式是：运行一个任务一小段时间；当该任务调度出去(或发生
一次调度器 tick)时，任务的 CPU 使用被"记账"：它刚刚使用物理 CPU 花掉的(少量)
时间被累加到 p->se.vruntime。一旦 p->se.vruntime 高到让另一个任务成为其维护的
按时间排序红黑树的"最左边任务"(再加上相对于最左任务的一小段"粒度"距离，以免
过度调度任务、把缓存搅乱)，就挑选新的最左任务并抢占当前任务。)



4.  SOME FEATURES OF CFS(4. CFS 的一些特性)
===========================================

CFS uses nanosecond granularity accounting and does not rely on any jiffies or
other HZ detail.  Thus the CFS scheduler has no notion of "timeslices" in the
way the previous scheduler had, and has no heuristics whatsoever.  There is
only one central tunable: (只有一个中心化的可调参数：)

   /sys/kernel/debug/sched/base_slice_ns

which can be used to tune the scheduler from "desktop" (i.e., low latencies) to
"server" (i.e., good batching) workloads.  It defaults to a setting suitable
for desktop workloads.  SCHED_BATCH is handled by the CFS scheduler module too.
(它可用于把调度器从"桌面"(即低延迟)负载调整到"服务器"(即良好的批处理)负载。
默认值适合桌面负载。SCHED_BATCH 也由 CFS 调度器模块处理。)

In case CONFIG_HZ results in base_slice_ns < TICK_NSEC, the value of
base_slice_ns will have little to no impact on the workloads.
(如果 CONFIG_HZ 导致 base_slice_ns < TICK_NSEC，那么 base_slice_ns 的取值对
这些负载几乎没有影响。)

Due to its design, the CFS scheduler is not prone to any of the "attacks" that
exist today against the heuristics of the stock scheduler: fiftyp.c, thud.c,
chew.c, ring-test.c, massive_intr.c all work fine and do not impact
interactivity and produce the expected behavior.
(由于其设计，CFS 调度器不易受当今针对原版调度器启发式规则的各类"攻击"的影响：
fiftyp.c、thud.c、chew.c、ring-test.c、massive_intr.c 全都正常工作，不影响
交互性，并产生预期的行为。)

The CFS scheduler has a much stronger handling of nice levels and SCHED_BATCH
than the previous vanilla scheduler: both types of workloads are isolated much
more aggressively.
(CFS 调度器对 nice 级别和 SCHED_BATCH 的处理比之前的原版调度器强得多：这两类
负载被更加激进地隔离。)

SMP load-balancing has been reworked/sanitized: the runqueue-walking
assumptions are gone from the load-balancing code now, and iterators of the
scheduling modules are used.  The balancing code got quite a bit simpler as a
result.
(SMP 负载均衡已被重构/清理：负载均衡代码中不再有遍历运行队列的假设，转而使用
调度模块的迭代器。均衡代码因此变得相当简洁。)



5. Scheduling policies(5. 调度策略)
===================================

CFS implements three scheduling policies:
(CFS 实现了三种调度策略：)

  - SCHED_NORMAL (traditionally called SCHED_OTHER): The scheduling
    policy that is used for regular tasks.
    (SCHED_NORMAL(传统上称为 SCHED_OTHER)：用于普通任务的调度策略。)

  - SCHED_BATCH: Does not preempt nearly as often as regular tasks
    would, thereby allowing tasks to run longer and make better use of
    caches but at the cost of interactivity. This is well suited for
    batch jobs.
    (SCHED_BATCH：几乎不像普通任务那样频繁地被抢占，从而让任务运行得更久、
    更好地利用缓存，但代价是交互性。非常适合批处理作业。)

  - SCHED_IDLE: This is even weaker than nice 19, but its not a true
    idle timer scheduler in order to avoid to get into priority
    inversion problems which would deadlock the machine.
    (SCHED_IDLE：比 nice 19 还要弱，但它不是真正的 idle 定时器调度器，以免
    陷入会死锁机器的优先级反转问题。)

SCHED_FIFO/_RR are implemented in sched/rt.c and are as specified by
POSIX.
(SCHED_FIFO/_RR 在 sched/rt.c 中实现，符合 POSIX 的规定。)

The command chrt from util-linux-ng 2.13.1.1 can set all of these except
SCHED_IDLE.
(util-linux-ng 2.13.1.1 的 chrt 命令可以设置除 SCHED_IDLE 之外的上述所有
策略。)



6.  SCHEDULING CLASSES(6. 调度类)
=================================

The new CFS scheduler has been designed in such a way to introduce "Scheduling
Classes," an extensible hierarchy of scheduler modules.  These modules
encapsulate scheduling policy details and are handled by the scheduler core
without the core code assuming too much about them.
(新的 CFS 调度器在设计上引入了"调度类 (Scheduling Classes)"——一个可扩展的
调度器模块层级。这些模块封装调度策略细节，由调度器核心处理，而核心代码不对
它们做过多假设。)

sched/fair.c implements the CFS scheduler described above.
(sched/fair.c 实现了上文描述的 CFS 调度器。)

sched/rt.c implements SCHED_FIFO and SCHED_RR semantics, in a simpler way than
the previous vanilla scheduler did.  It uses 100 runqueues (for all 100 RT
priority levels, instead of 140 in the previous scheduler) and it needs no
expired array.
(sched/rt.c 以比之前的原版调度器更简单的方式实现 SCHED_FIFO 和 SCHED_RR 语义。
它使用 100 个运行队列(对应全部 100 个 RT 优先级，而不是之前调度器的 140 个)，
并且不需要 expired 数组。)

Scheduling classes are implemented through the sched_class structure, which
contains hooks to functions that must be called whenever an interesting event
occurs.
(调度类通过 sched_class 结构实现，其中包含在发生值得关注的事件时必须调用的
函数挂钩。)

This is the (partial) list of the hooks:
(以下是这些挂钩的(不完全)列表：)

 - enqueue_task(...)

   Called when a task enters a runnable state.
   It puts the scheduling entity (task) into the red-black tree and
   increments the nr_running variable.
   (在任务进入可运行状态时调用。它把调度实体(任务)放入红黑树，并递增
   nr_running 变量。)

 - dequeue_task(...)

   When a task is no longer runnable, this function is called to keep the
   corresponding scheduling entity out of the red-black tree.  It decrements
   the nr_running variable.
   (当任务不再可运行时调用此函数，使相应的调度实体离开红黑树。它会递减
   nr_running 变量。)

 - yield_task(...)

   This function yields the CPU by moving the currently running task's position back
   in the runqueue, so that other runnable tasks get scheduled first.
   (该函数让出 CPU：把当前运行任务的位置向运行队列后方移动，使其他可运行任务
   先被调度。)

 - wakeup_preempt(...)

   This function checks if a task that entered the runnable state should
   preempt the currently running task.
   (该函数检查进入可运行状态的任务是否应当抢占当前正在运行的任务。)

 - pick_next_task(...)

   This function chooses the most appropriate task eligible to run next.
   (该函数选择下一个最适合运行的任务。)

 - set_next_task(...)

   This function is called when a task changes its scheduling class, changes
   its task group or is scheduled.
   (当任务更改其调度类、更改其任务组或被调度时，会调用该函数。)

 - task_tick(...)

   This function is mostly called from time tick functions; it might lead to
   process switch.  This drives the running preemption.
   (该函数大多由时间 tick 函数调用；它可能引发进程切换。运行中的抢占由它
   驱动。)




7.  GROUP SCHEDULER EXTENSIONS TO CFS(7. CFS 的组调度扩展)
==========================================================

Normally, the scheduler operates on individual tasks and strives to provide
fair CPU time to each task.  Sometimes, it may be desirable to group tasks and
provide fair CPU time to each such task group.  For example, it may be
desirable to first provide fair CPU time to each user on the system and then to
each task belonging to a user.
(通常，调度器针对单个任务进行操作，并努力为每个任务提供公平的 CPU 时间。
有时，把任务分组并为每个这样的任务组提供公平的 CPU 时间可能更合意。例如，
可以先在系统中的每个用户之间公平分配 CPU 时间，然后在属于某个用户的各任务
之间分配。)

CONFIG_CGROUP_SCHED strives to achieve exactly that.  It lets tasks to be
grouped and divides CPU time fairly among such groups.
(CONFIG_CGROUP_SCHED 致力于精确实现这一点。它允许把任务分组，并在这些组之间
公平地划分 CPU 时间。)

CONFIG_RT_GROUP_SCHED permits to group real-time (i.e., SCHED_FIFO and
SCHED_RR) tasks.
(CONFIG_RT_GROUP_SCHED 允许对实时任务(即 SCHED_FIFO 和 SCHED_RR)分组。)

CONFIG_FAIR_GROUP_SCHED permits to group CFS (i.e., SCHED_NORMAL and
SCHED_BATCH) tasks.
(CONFIG_FAIR_GROUP_SCHED 允许对 CFS 任务(即 SCHED_NORMAL 和 SCHED_BATCH)
分组。)

   These options need CONFIG_CGROUPS to be defined, and let the administrator
   create arbitrary groups of tasks, using the "cgroup" pseudo filesystem.  See
   Documentation/admin-guide/cgroup-v1/cgroups.rst for more information about this filesystem.
   (这些选项需要定义 CONFIG_CGROUPS，并允许管理员使用 "cgroup" 伪文件系统创建
   任意的任务组。关于该文件系统的更多信息见
   Documentation/admin-guide/cgroup-v1/cgroups.rst。)

When CONFIG_FAIR_GROUP_SCHED is defined, a "cpu.shares" file is created for each
group created using the pseudo filesystem.  See example steps below to create
task groups and modify their CPU share using the "cgroups" pseudo filesystem::

	# mount -t tmpfs cgroup_root /sys/fs/cgroup
	# mkdir /sys/fs/cgroup/cpu
	# mount -t cgroup -ocpu none /sys/fs/cgroup/cpu
	# cd /sys/fs/cgroup/cpu

	# mkdir multimedia	# create "multimedia" group of tasks
	# mkdir browser		# create "browser" group of tasks

	# #Configure the multimedia group to receive twice the CPU bandwidth
	# #that of browser group

	# echo 2048 > multimedia/cpu.shares
	# echo 1024 > browser/cpu.shares

	# firefox &	# Launch firefox and move it to "browser" group
	# echo <firefox_pid> > browser/tasks

	# #Launch gmplayer (or your favourite movie player)
	# echo <movie_player_pid> > multimedia/tasks

(定义 CONFIG_FAIR_GROUP_SCHED 时，会为使用伪文件系统创建的每个组创建一个
"cpu.shares" 文件。下面是使用 "cgroups" 伪文件系统创建任务组并修改其 CPU
份额的示例步骤：)
