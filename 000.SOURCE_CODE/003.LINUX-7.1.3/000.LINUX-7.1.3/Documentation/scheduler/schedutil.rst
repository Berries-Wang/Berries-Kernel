===========================
Schedutil(Schedutil 调频器)
===========================

.. note::

   All this assumes a linear relation between frequency and work capacity,
   we know this is flawed, but it is the best workable approximation.
   (所有这些都假设频率与工作能力之间呈线性关系。我们知道这是有缺陷的，但这是目前最好的可行近似。)


PELT (Per Entity Load Tracking)(PELT 按实体负载跟踪)
====================================================

With PELT we track some metrics across the various scheduler entities, from
individual tasks to task-group slices to CPU runqueues. As the basis for this
we use an Exponentially Weighted Moving Average (EWMA), each period (1024us)
is decayed such that y^32 = 0.5. That is, the most recent 32ms contribute
half, while the rest of history contribute the other half.
(通过 PELT，我们跨各类调度实体跟踪一些指标，从单个任务到任务组切片再到 CPU 运行队列。作为其基础，我们使用指数加权移动平均 (EWMA)，每个周期 (1024us) 都按 y^32 = 0.5 进行衰减。也就是说，最近的 32ms 贡献一半，其余历史贡献另一半。)

Specifically:
(具体来说：)

  ewma_sum(u) := u_0 + u_1*y + u_2*y^2 + ...

  ewma(u) = ewma_sum(u) / ewma_sum(1)

Since this is essentially a progression of an infinite geometric series, the
results are composable, that is ewma(A) + ewma(B) = ewma(A+B). This property
is key, since it gives the ability to recompose the averages when tasks move
around.
(由于这本质上是一个无穷几何级数的演进，其结果是可组合 (composable) 的，即 ewma(A) + ewma(B) = ewma(A+B)。这个性质是关键，因为它使任务在移动时能够重新组合平均值。)

Note that blocked tasks still contribute to the aggregates (task-group slices
and CPU runqueues), which reflects their expected contribution when they
resume running.
(注意，被阻塞的任务仍然计入聚合值(任务组切片和 CPU 运行队列)，这反映了它们恢复运行时的预期贡献。)

Using this we track 2 key metrics: 'running' and 'runnable'. 'Running'
reflects the time an entity spends on the CPU, while 'runnable' reflects the
time an entity spends on the runqueue. When there is only a single task these
two metrics are the same, but once there is contention for the CPU 'running'
will decrease to reflect the fraction of time each task spends on the CPU
while 'runnable' will increase to reflect the amount of contention.
(使用它，我们跟踪两个关键指标：'running' 和 'runnable'。'Running' 反映实体在 CPU 上花费的时间，而 'runnable' 反映实体在运行队列上花费的时间。当只有一个任务时，这两个指标相同；但一旦出现对 CPU 的争用，'running' 会下降以反映每个任务在 CPU 上花费的时间比例，而 'runnable' 会上升以反映争用的程度。)

For more detail see: kernel/sched/pelt.c
(更多细节参见 kernel/sched/pelt.c。)


Frequency / CPU Invariance(频率 / CPU 不变性)
=============================================

Because consuming the CPU for 50% at 1GHz is not the same as consuming the CPU
for 50% at 2GHz, nor is running 50% on a LITTLE CPU the same as running 50% on
a big CPU, we allow architectures to scale the time delta with two ratios, one
Dynamic Voltage and Frequency Scaling (DVFS) ratio and one microarch ratio.
(因为在 1GHz 下消耗 50% 的 CPU 与在 2GHz 下消耗 50% 的 CPU 并不相同，在 LITTLE CPU 上运行 50% 与在 big CPU 上运行 50% 也不相同，所以我们允许架构用两个比率来缩放时间增量：一个是动态电压与频率调节 (DVFS) 比率，另一个是微架构比率。)

For simple DVFS architectures (where software is in full control) we trivially
compute the ratio as::

	    f_cur
  r_dvfs := -----
            f_max

(对于软件完全掌控的简单 DVFS 架构，我们直接将该比率计算为：)

For more dynamic systems where the hardware is in control of DVFS we use
hardware counters (Intel APERF/MPERF, ARMv8.4-AMU) to provide us this ratio.
For Intel specifically, we use::

	   APERF
  f_cur := ----- * P0
	   MPERF

	     4C-turbo;	if available and turbo enabled
  f_max := { 1C-turbo;	if turbo enabled
	     P0;	otherwise

                    f_cur
  r_dvfs := min( 1, ----- )
                    f_max

(对于硬件掌控 DVFS 的更动态的系统，我们使用硬件计数器 (Intel APERF/MPERF、ARMv8.4-AMU) 来提供这个比率。对于 Intel，我们特别使用：)

We pick 4C turbo over 1C turbo to make it slightly more sustainable.
(我们选择 4C turbo 而非 1C turbo，以使其在能效上稍微更具可持续性。)

r_cpu is determined as the ratio of highest performance level of the current
CPU vs the highest performance level of any other CPU in the system.
(r_cpu 由当前 CPU 的最高性能等级与系统中任何其他 CPU 的最高性能等级之比确定。)

  r_tot = r_dvfs * r_cpu

The result is that the above 'running' and 'runnable' metrics become invariant
of DVFS and CPU type. IOW. we can transfer and compare them between CPUs.
(其结果是，上述 'running' 和 'runnable' 指标变得与 DVFS 和 CPU 类型无关。换句话说，我们可以在 CPU 之间传递和比较它们。)

For more detail see:
(更多细节参见：)

 - kernel/sched/pelt.h:update_rq_clock_pelt()
 - arch/x86/kernel/smpboot.c:"APERF/MPERF frequency ratio computation."
 - Documentation/scheduler/sched-capacity.rst:"1. CPU Capacity + 2. Task utilization"


UTIL_EST(利用率估计)
====================

Because periodic tasks have their averages decayed while they sleep, even
though when running their expected utilization will be the same, they suffer a
(DVFS) ramp-up after they are running again.
(因为周期性任务在睡眠期间平均值会衰减，即使运行时的预期利用率不变，它们在重新运行后仍会经历一次 (DVFS) 爬升 (ramp-up)。)

To alleviate this (a default enabled option) UTIL_EST drives an Infinite
Impulse Response (IIR) EWMA with the 'running' value on dequeue -- when it is
highest. UTIL_EST filters to instantly increase and only decay on decrease.
(为了缓解这一点(一个默认启用的选项)，UTIL_EST 在出队 (dequeue) 时——即其值最高时——用 'running' 值驱动一个无限脉冲响应 (IIR) EWMA。UTIL_EST 过滤器会瞬间增大，只在减小时衰减。)

A further runqueue wide sum (of runnable tasks) is maintained of:

  util_est := \Sum_t max( t_running, t_util_est_ewma )

(另外还维护一个运行队列级别的(针对可运行任务的)总和：util_est := \Sum_t max( t_running, t_util_est_ewma )。)

For more detail see: kernel/sched/fair.c:util_est_dequeue()


UCLAMP(利用率钳位)
==================

It is possible to set effective u_min and u_max clamps on each CFS or RT task;
the runqueue keeps an max aggregate of these clamps for all running tasks.
(可以在每个 CFS 或 RT 任务上设置有效的 u_min 和 u_max 钳位 (clamp)；运行队列会为所有正在运行的任务保存这些钳位的最大聚合值。)

For more detail see: include/uapi/linux/sched/types.h
(更多细节参见 include/uapi/linux/sched/types.h。)


Schedutil / DVFS(Schedutil 与 DVFS)
===================================

Every time the scheduler load tracking is updated (task wakeup, task
migration, time progression) we call out to schedutil to update the hardware
DVFS state.
(每当调度器负载跟踪被更新(任务唤醒、任务迁移、时间推进)时，我们都会调用 schedutil 来更新硬件 DVFS 状态。)

The basis is the CPU runqueue's 'running' metric, which per the above it is
the frequency invariant utilization estimate of the CPU. From this we compute
a desired frequency like::

             max( running, util_est );	if UTIL_EST
  u_cfs := { running;			otherwise

               clamp( u_cfs + u_rt , u_min, u_max );	if UCLAMP_TASK
  u_clamp := { u_cfs + u_rt;				otherwise

  u := u_clamp + u_irq + u_dl;		[approx. see source for more detail]

  f_des := min( f_max, 1.25 u * f_max )

(其基础是 CPU 运行队列的 'running' 指标，根据上文，它是 CPU 的频率不变性利用率估计。由此我们按如下方式计算期望频率：)

XXX IO-wait: when the update is due to a task wakeup from IO-completion we
boost 'u' above.
(XXX IO-wait: 当更新是由任务因 IO 完成而唤醒引起时，我们会提升上面的 'u'。)

This frequency is then used to select a P-state/OPP or directly munged into a
CPPC style request to the hardware.
(然后，这个频率被用于选择 P-state/OPP，或直接加工成 CPPC 风格的请求发送给硬件。)

XXX: deadline tasks (Sporadic Task Model) allows us to calculate a hard f_min
required to satisfy the workload.
(XXX: deadline 任务(偶发任务模型 Sporadic Task Model)使我们能够计算出满足工作负载所需的硬性 f_min。)

Because these callbacks are directly from the scheduler, the DVFS hardware
interaction should be 'fast' and non-blocking. Schedutil supports
rate-limiting DVFS requests for when hardware interaction is slow and
expensive, this reduces effectiveness.
(因为这些回调直接来自调度器，所以与 DVFS 硬件的交互应当是'快速'且非阻塞的。当硬件交互缓慢且代价高昂时，Schedutil 支持对 DVFS 请求进行限速 (rate-limiting)，这会降低其效果。)

For more information see: kernel/sched/cpufreq_schedutil.c
(更多信息参见 kernel/sched/cpufreq_schedutil.c。)


NOTES(注意事项)
===============

 - On low-load scenarios, where DVFS is most relevant, the 'running' numbers
   will closely reflect utilization.
   (在低负载场景——即 DVFS 最相关的场景——中，'running' 数值会非常接近地反映利用率。)

 - In saturated scenarios task movement will cause some transient dips,
   suppose we have a CPU saturated with 4 tasks, then when we migrate a task
   to an idle CPU, the old CPU will have a 'running' value of 0.75 while the
   new CPU will gain 0.25. This is inevitable and time progression will
   correct this. XXX do we still guarantee f_max due to no idle-time?
   (在饱和场景中，任务移动会导致一些瞬时的下降：假设有一个被 4 个任务饱和的 CPU，当我们把一个任务迁移到空闲 CPU 时，原 CPU 的 'running' 值会变为 0.75，而新 CPU 会获得 0.25。这是不可避免的，时间推进会修正这一点。XXX 由于没有空闲时间，我们是否仍保证 f_max？)

 - Much of the above is about avoiding DVFS dips, and independent DVFS domains
   having to re-learn / ramp-up when load shifts.
   (上面的大部分内容都是关于如何避免 DVFS 的下降，以及独立的 DVFS 域在负载转移时不得不重新学习/爬升的问题。)

