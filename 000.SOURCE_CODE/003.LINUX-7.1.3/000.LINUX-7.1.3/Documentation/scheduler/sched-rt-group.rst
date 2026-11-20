======================================
Real-Time group scheduling(实时组调度)
======================================

.. CONTENTS

   0. WARNING
   1. Overview
     1.1 The problem
     1.2 The solution
   2. The interface
     2.1 System-wide settings
     2.2 Default behaviour
     2.3 Basis for grouping tasks
   3. Future plans


0. WARNING(0. 警告)
===================

 Fiddling with these settings can result in an unstable system, the knobs are
 root only and assumes root knows what he is doing.
 (摆弄这些设置可能导致系统不稳定。这些开关仅限 root 使用，并假定 root 清楚自己在做什么。)

Most notable:
(最值得注意的：)

 * very small values in sched_rt_period_us can result in an unstable
   system when the period is smaller than either the available hrtimer
   resolution, or the time it takes to handle the budget refresh itself.
   (sched_rt_period_us 取值过小可能导致系统不稳定：当周期小于可用的 hrtimer 分辨率，或小于处理预算刷新本身所需时间时。)

 * very small values in sched_rt_runtime_us can result in an unstable
   system when the runtime is so small the system has difficulty making
   forward progress (NOTE: the migration thread and kstopmachine both
   are real-time processes).
   (sched_rt_runtime_us 取值过小可能导致系统不稳定：当 runtime 小到系统难以取得进展时(注意：migration 线程与 kstopmachine 都是实时进程)。)

1. Overview(1. 概述)
====================


1.1 The problem(1.1 问题)
-------------------------

Real-time scheduling is all about determinism, a group has to be able to rely on
the amount of bandwidth (eg. CPU time) being constant. In order to schedule
multiple groups of real-time tasks, each group must be assigned a fixed portion
of the CPU time available.  Without a minimum guarantee a real-time group can
obviously fall short. A fuzzy upper limit is of no use since it cannot be
relied upon. Which leaves us with just the single fixed portion.
(实时调度讲究的是确定性，一个组必须能够依赖带宽(例如 CPU 时间)是恒定的。为了调度多个实时任务组，必须给每个组分配可用 CPU 时间中的一个固定份额。没有最小保证，实时组显然可能不够用。模糊的上限没有用处，因为它不可依赖。于是只剩唯一的固定份额方案。)

1.2 The solution(1.2 解决方案)
------------------------------

CPU time is divided by means of specifying how much time can be spent running
in a given period. We allocate this "run time" for each real-time group which
the other real-time groups will not be permitted to use.
(CPU 时间的划分方式是指定在一个给定周期内可以运行多长时间。我们为每个实时组分配这段"运行时间"，其他实时组不允许使用它。)

Any time not allocated to a real-time group will be used to run normal priority
tasks (SCHED_OTHER). Any allocated run time not used will also be picked up by
SCHED_OTHER.
(未分配给实时组的时间将用于运行普通优先级任务 (SCHED_OTHER)。已分配但未用完的运行时间同样会被 SCHED_OTHER 拿去使用。)

Let's consider an example: a frame fixed real-time renderer must deliver 25
frames a second, which yields a period of 0.04s per frame. Now say it will also
have to play some music and respond to input, leaving it with around 80% CPU
time dedicated for the graphics. We can then give this group a run time of 0.8
* 0.04s = 0.032s.
(来看一个例子：一个定帧实时渲染器必须每秒交付 25 帧，即每帧周期为 0.04s。假设它还需要播放音乐并响应输入，大约只留下 80% 的 CPU 时间专用于图形。那么我们可以给这个组 0.8 * 0.04s = 0.032s 的运行时间。)

This way the graphics group will have a 0.04s period with a 0.032s run time
limit. Now if the audio thread needs to refill the DMA buffer every 0.005s, but
needs only about 3% CPU time to do so, it can do with a 0.03 * 0.005s =
0.00015s. So this group can be scheduled with a period of 0.005s and a run time
of 0.00015s.
(这样，图形组将拥有 0.04s 的周期和 0.032s 的运行时间上限。再如，音频线程需要每 0.005s 填充一次 DMA 缓冲，但只需约 3% 的 CPU 时间，那么 0.03 * 0.005s = 0.00015s 就够了。于是这个组可以用 0.005s 的周期和 0.00015s 的运行时间来调度。)

The remaining CPU time will be used for user input and other tasks. Because
real-time tasks have explicitly allocated the CPU time they need to perform
their tasks, buffer underruns in the graphics or audio can be eliminated.
(剩余的 CPU 时间将用于用户输入和其他任务。由于实时任务显式分配了完成任务所需的 CPU 时间，图形或音频的缓冲区下溢 (buffer underrun) 可以被消除。)

NOTE: the above example is not fully implemented yet. We still
lack an EDF scheduler to make non-uniform periods usable.
(注意：上面的例子尚未完全实现。我们仍缺少一个 EDF 调度器来让非均匀周期变得可用。)


2. The Interface(2. 接口)
=========================


2.1 System wide settings(2.1 系统级设置)
----------------------------------------

The system wide settings are configured under the /proc virtual file system:
(系统级设置在 /proc 虚拟文件系统下配置：)

/proc/sys/kernel/sched_rt_period_us:
  The scheduling period that is equivalent to 100% CPU bandwidth.
  (相当于 100% CPU 带宽的调度周期。)

/proc/sys/kernel/sched_rt_runtime_us:
  A global limit on how much time real-time scheduling may use. This is always
  less or equal to the period_us, as it denotes the time allocated from the
  period_us for the real-time tasks. Without CONFIG_RT_GROUP_SCHED enabled,
  this only serves for admission control of deadline tasks. With
  CONFIG_RT_GROUP_SCHED=y it also signifies the total bandwidth available to
  all real-time groups.
  (实时调度可使用时间的全局上限。它总是小于等于 period_us，因为它表示从 period_us 中分配给实时任务的时间。在未启用 CONFIG_RT_GROUP_SCHED 时，它仅用于 deadline 任务的准入控制。启用 CONFIG_RT_GROUP_SCHED=y 时，它还表示所有实时组可用的总带宽。)

  * Time is specified in us because the interface is s32. This gives an
    operating range from 1us to about 35 minutes.
    (时间以 us 为单位，因为接口是 s32 类型。这给出了从 1us 到约 35 分钟的可操作范围。)
  * sched_rt_period_us takes values from 1 to INT_MAX.
    (sched_rt_period_us 取值范围为 1 到 INT_MAX。)
  * sched_rt_runtime_us takes values from -1 to sched_rt_period_us.
    (sched_rt_runtime_us 取值范围为 -1 到 sched_rt_period_us。)
  * A run time of -1 specifies runtime == period, ie. no limit.
    (运行时间为 -1 表示 runtime == period，即无限制。)
  * sched_rt_runtime_us/sched_rt_period_us > 0.05 inorder to preserve
    bandwidth for fair dl_server. For accurate value check average of
    runtime/period in /sys/kernel/debug/sched/fair_server/cpuX/
    (sched_rt_runtime_us/sched_rt_period_us > 0.05，以便为公平 dl_server 保留带宽。要获取准确数值，可查看 /sys/kernel/debug/sched/fair_server/cpuX/ 中 runtime/period 的平均值。)


2.2 Default behaviour(2.2 默认行为)
-----------------------------------

The default values for sched_rt_period_us (1000000 or 1s) and
sched_rt_runtime_us (950000 or 0.95s).  This gives 0.05s to be used by
SCHED_OTHER (non-RT tasks). These defaults were chosen so that a run-away
real-time tasks will not lock up the machine but leave a little time to recover
it.  By setting runtime to -1 you'd get the old behaviour back.
(sched_rt_period_us 的默认值为 1000000(即 1s)，sched_rt_runtime_us 的默认值为 950000(即 0.95s)。这留出 0.05s 供 SCHED_OTHER(非实时任务)使用。选择这些默认值是为了让失控的实时任务不会锁死机器，而是留出一点时间用于恢复。把 runtime 设为 -1 即可回到旧行为。)

By default all bandwidth is assigned to the root group and new groups get the
period from /proc/sys/kernel/sched_rt_period_us and a run time of 0. If you
want to assign bandwidth to another group, reduce the root group's bandwidth
and assign some or all of the difference to another group.
(默认情况下，所有带宽都分配给根组，新组从 /proc/sys/kernel/sched_rt_period_us 获取周期，运行时间为 0。如果想把带宽分配给其他组，需要先减少根组的带宽，再把差值的一部分或全部分配给另一个组。)

Real-time group scheduling means you have to assign a portion of total CPU
bandwidth to the group before it will accept real-time tasks. Therefore you will
not be able to run real-time tasks as any user other than root until you have
done that, even if the user has the rights to run processes with real-time
priority!
(实时组调度意味着必须先给组分配一部分总 CPU 带宽，它才会接受实时任务。因此，在这样做之前，你将无法以 root 以外的任何用户身份运行实时任务，即使用户有权以实时优先级运行进程！)


2.3 Basis for grouping tasks(2.3 任务分组的基础)
------------------------------------------------

Enabling CONFIG_RT_GROUP_SCHED lets you explicitly allocate real
CPU bandwidth to task groups.
(启用 CONFIG_RT_GROUP_SCHED 后，可以显式地向任务组分配真实的 CPU 带宽。)

This uses the cgroup virtual file system and "<cgroup>/cpu.rt_runtime_us"
to control the CPU time reserved for each control group.
(它使用 cgroup 虚拟文件系统和 "<cgroup>/cpu.rt_runtime_us" 来控制为每个控制组预留的 CPU 时间。)

For more information on working with control groups, you should read
Documentation/admin-guide/cgroup-v1/cgroups.rst as well.
(有关使用控制组的更多信息，还应阅读 Documentation/admin-guide/cgroup-v1/cgroups.rst。)

Group settings are checked against the following limits in order to keep the
configuration schedulable:
(为保持配置可调度，组设置会对照以下限制进行检查：)

   \Sum_{i} runtime_{i} / global_period <= global_runtime / global_period

For now, this can be simplified to just the following (but see Future plans):
(目前，这可以简化为下式(但参见"未来计划"一节)：)

   \Sum_{i} runtime_{i} <= global_runtime


3. Future plans(3. 未来计划)
============================

There is work in progress to make the scheduling period for each group
("<cgroup>/cpu.rt_period_us") configurable as well.
(目前正在进行的工作还包括让每个组的调度周期("<cgroup>/cpu.rt_period_us")也可配置。)

The constraint on the period is that a subgroup must have a smaller or
equal period to its parent. But realistically its not very useful _yet_
as its prone to starvation without deadline scheduling.
(对周期的约束是：子组的周期必须小于等于其父组。但现实中，在没有 deadline 调度的情况下它_还_没有太大用处，因为容易发生饥饿。)

Consider two sibling groups A and B; both have 50% bandwidth, but A's
period is twice the length of B's.
(考虑两个兄弟组 A 和 B：两者都有 50% 的带宽，但 A 的周期是 B 的两倍。)

* group A: period=100000us, runtime=50000us
  (组 A：period=100000us，runtime=50000us)

	- this runs for 0.05s once every 0.1s
	  (每 0.1s 运行 0.05s 一次)

* group B: period= 50000us, runtime=25000us
  (组 B：period=50000us，runtime=25000us)

	- this runs for 0.025s twice every 0.1s (or once every 0.05 sec).
	  (每 0.1s 运行 0.025s 两次(或每 0.05 秒运行一次)。)

This means that currently a while (1) loop in A will run for the full period of
B and can starve B's tasks (assuming they are of lower priority) for a whole
period.
(这意味着目前 A 中的一个 while (1) 循环会跑满 B 的整个周期，并可能使 B 的任务(假定其优先级较低)整整一个周期得不到调度。)

The next project will be SCHED_EDF (Earliest Deadline First scheduling) to bring
full deadline scheduling to the linux kernel. Deadline scheduling the above
groups and treating end of the period as a deadline will ensure that they both
get their allocated time.
(下一个项目将是 SCHED_EDF(最早截止期优先调度)，为 Linux 内核带来完整的 deadline 调度。对上述组进行 deadline 调度并把周期结束视为截止期，将确保两者都能获得分配给它们的时间。)

Implementing SCHED_EDF might take a while to complete. Priority Inheritance is
the biggest challenge as the current linux PI infrastructure is geared towards
the limited static priority levels 0-99. With deadline scheduling you need to
do deadline inheritance (since priority is inversely proportional to the
deadline delta (deadline - now)).
(实现 SCHED_EDF 可能需要一段时间。优先级继承 (Priority Inheritance) 是最大的挑战，因为当前 Linux 的 PI 基础设施是围绕有限的静态优先级 0-99 设计的。在 deadline 调度中需要做 deadline 继承(因为优先级与截止期差 (deadline - now) 成反比)。)

This means the whole PI machinery will have to be reworked - and that is one of
the most complex pieces of code we have.
(这意味着整套 PI 机制都必须重做——而这可是我们所拥有的最复杂的代码之一。)
