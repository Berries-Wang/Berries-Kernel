===================================
CFS Bandwidth Control(CFS 带宽控制)
===================================

.. note::
   This document only discusses CPU bandwidth control for SCHED_NORMAL.
   The SCHED_RT case is covered in Documentation/scheduler/sched-rt-group.rst
   (本文档只讨论针对 SCHED_NORMAL 的 CPU 带宽控制。SCHED_RT 的情况见 Documentation/scheduler/sched-rt-group.rst。)

CFS bandwidth control is a CONFIG_FAIR_GROUP_SCHED extension which allows the
specification of the maximum CPU bandwidth available to a group or hierarchy.
(CFS 带宽控制是 CONFIG_FAIR_GROUP_SCHED 的一个扩展，允许为一个组或层级指定可用的最大 CPU 带宽。)

The bandwidth allowed for a group is specified using a quota and period. Within
each given "period" (microseconds), a task group is allocated up to "quota"
microseconds of CPU time. That quota is assigned to per-cpu run queues in
slices as threads in the cgroup become runnable. Once all quota has been
assigned any additional requests for quota will result in those threads being
throttled. Throttled threads will not be able to run again until the next
period when the quota is replenished.
(一个组允许的带宽通过配额 (quota) 与周期 (period) 来指定。在每个给定的 "period" (微秒) 内，一个任务组最多被分配 "quota" 微秒的 CPU 时间。当 cgroup 中的线程变为可运行时，该配额以分片 (slice) 的形式分配到各 CPU 的运行队列。一旦全部配额被分配完，任何额外的配额请求都会导致这些线程被限流 (throttle)。被限流的线程要到下一个周期配额得到补充后才能再次运行。)

A group's unassigned quota is globally tracked, being refreshed back to
cfs_quota units at each period boundary. As threads consume this bandwidth it
is transferred to cpu-local "silos" on a demand basis. The amount transferred
within each of these updates is tunable and described as the "slice".
(组中未分配的配额由全局跟踪，在每个周期边界处刷新回 cfs_quota 单位。当线程消耗这部分带宽时，会按需转移到 CPU 本地的 "silos" (囤积池) 中。每次此类更新转移的数量可调，称为 "slice" (分片)。)

Burst feature(突发特性)
-----------------------
This feature borrows time now against our future underrun, at the cost of
increased interference against the other system users. All nicely bounded.
(该特性以对系统其他用户的干扰增加为代价，现在就借用时间来抵补未来的低估用量。整体是有界受控的。)

Traditional (UP-EDF) bandwidth control is something like:
(传统 (UP-EDF) 带宽控制大致如下：)

  (U = \Sum u_i) <= 1

This guaranteeds both that every deadline is met and that the system is
stable. After all, if U were > 1, then for every second of walltime,
we'd have to run more than a second of program time, and obviously miss
our deadline, but the next deadline will be further out still, there is
never time to catch up, unbounded fail.
(这保证每个截止期都被满足且系统稳定。毕竟，如果 U > 1，那么每过一秒的真实时间，就必须运行超过一秒的程序时间，显然会错过截止期，而下一个截止期又更远，永远没有时间追赶，失败无界。)

The burst feature observes that a workload doesn't always executes the full
quota; this enables one to describe u_i as a statistical distribution.
(突发特性观察到负载并不总是用满全部配额；这使得可以把 u_i 描述为一个统计分布。)

For example, have u_i = {x,e}_i, where x is the p(95) and x+e p(100)
(the traditional WCET). This effectively allows u to be smaller,
increasing the efficiency (we can pack more tasks in the system), but at
the cost of missing deadlines when all the odds line up. However, it
does maintain stability, since every overrun must be paired with an
underrun as long as our x is above the average.
(例如，令 u_i = {x,e}_i，其中 x 是 p(95)，x+e 是 p(100) (传统的 WCET)。这实际上允许 u 更小，提高效率 (可以在系统中容纳更多任务)，但代价是当各种小概率事件同时发生时会错过截止期。不过只要 x 高于平均值，每个超量必然伴随一个欠量，因此仍能维持稳定。)

That is, suppose we have 2 tasks, both specify a p(95) value, then we
have a p(95)*p(95) = 90.25% chance both tasks are within their quota and
everything is good. At the same time we have a p(5)p(5) = 0.25% chance
both tasks will exceed their quota at the same time (guaranteed deadline
fail). Somewhere in between there's a threshold where one exceeds and
the other doesn't underrun enough to compensate; this depends on the
specific CDFs.
(也就是说，假设有 2 个任务，都指定 p(95) 值，那么有 p(95)*p(95) = 90.25% 的概率两个任务都在各自配额内，一切正常。同时也有 p(5)p(5) = 0.25% 的概率两个任务同时超出配额 (必然错过截止期)。两者之间存在某个阈值：一个任务超量而另一个的欠量不足以补偿；这取决于具体的 CDF。)

At the same time, we can say that the worst case deadline miss, will be
\Sum e_i; that is, there is a bounded tardiness (under the assumption
that x+e is indeed WCET).
(同时，可以说最坏情况的截止期错过量为 \Sum e_i；也就是说，延误是有上界的 (前提是 x+e 确实是 WCET)。)

The interference when using burst is valued by the possibilities for
missing the deadline and the average WCET. Test results showed that when
there many cgroups or CPU is under utilized, the interference is
limited. More details are shown in:
(使用突发时的干扰由错过截止期的可能性与平均 WCET 来衡量。测试结果表明，当 cgroup 数量很多或 CPU 利用率不高时，干扰是有限的。更多细节见：)

https://lore.kernel.org/lkml/5371BD36-55AE-4F71-B9D7-B86DC32E3D2B@linux.alibaba.com/

Management(管理)
----------------
Quota, period and burst are managed within the cpu subsystem via cgroupfs.
(配额、周期与突发量通过 cgroupfs 在 cpu 子系统中管理。)

.. note::
   The cgroupfs files described in this section are only applicable
   to cgroup v1. For cgroup v2, see
   :ref:`Documentation/admin-guide/cgroup-v2.rst <cgroup-v2-cpu>`.
   (本节描述的 cgroupfs 文件只适用于 cgroup v1。cgroup v2 请参见
   :ref:`Documentation/admin-guide/cgroup-v2.rst <cgroup-v2-cpu>`。)

- cpu.cfs_quota_us: run-time replenished within a period (in microseconds)(一个周期内补充的运行时间 (微秒))
- cpu.cfs_period_us: the length of a period (in microseconds)(一个周期的长度 (微秒))
- cpu.stat: exports throttling statistics [explained further below](导出限流统计 [下文进一步说明])
- cpu.cfs_burst_us: the maximum accumulated run-time (in microseconds)(最大累积运行时间 (微秒))

The default values are::

	cpu.cfs_period_us=100ms
	cpu.cfs_quota_us=-1
	cpu.cfs_burst_us=0

(默认值：)

A value of -1 for cpu.cfs_quota_us indicates that the group does not have any
bandwidth restriction in place, such a group is described as an unconstrained
bandwidth group. This represents the traditional work-conserving behavior for
CFS.
(cpu.cfs_quota_us 为 -1 表示该组没有任何带宽限制，这样的组被称为无约束带宽组。它代表 CFS 传统的工作保持 (work-conserving) 行为。)

Writing any (valid) positive value(s) no smaller than cpu.cfs_burst_us will
enact the specified bandwidth limit. The minimum quota allowed for the quota or
period is 1ms. There is also an upper bound on the period length of 1s.
Additional restrictions exist when bandwidth limits are used in a hierarchical
fashion, these are explained in more detail below.
(写入任何不小于 cpu.cfs_burst_us 的 (合法) 正值都会实施指定的带宽限制。配额与周期允许的最小值是 1ms，周期长度上限为 1s。当带宽限制以层级方式使用时还有额外限制，下文将更详细地说明。)

Writing any negative value to cpu.cfs_quota_us will remove the bandwidth limit
and return the group to an unconstrained state once more.
(向 cpu.cfs_quota_us 写入任何负值都会移除带宽限制，使该组重新回到无约束状态。)

A value of 0 for cpu.cfs_burst_us indicates that the group can not accumulate
any unused bandwidth. It makes the traditional bandwidth control behavior for
CFS unchanged. Writing any (valid) positive value(s) no larger than
cpu.cfs_quota_us into cpu.cfs_burst_us will enact the cap on unused bandwidth
accumulation.
(cpu.cfs_burst_us 为 0 表示该组不能累积任何未用完的带宽，使 CFS 的传统带宽控制行为保持不变。向 cpu.cfs_burst_us 写入任何不大于 cpu.cfs_quota_us 的 (合法) 正值，都会对未用带宽的累积设置上限。)

Any updates to a group's bandwidth specification will result in it becoming
unthrottled if it is in a constrained state.
(对组带宽设置的任何更新，都会使其在处于受限状态时解除限流。)

System wide settings(系统级设置)
--------------------------------
For efficiency run-time is transferred between the global pool and CPU local
"silos" in a batch fashion. This greatly reduces global accounting pressure
on large systems. The amount transferred each time such an update is required
is described as the "slice".
(为了效率，运行时间以批处理方式在全局池与 CPU 本地 "silos" 之间转移。这大大降低了大型系统上的全局记账压力。每次此类更新所需转移的数量称为 "slice"。)

This is tunable via procfs::

	/proc/sys/kernel/sched_cfs_bandwidth_slice_us (default=5ms)

(可通过 procfs 调整：)

Larger slice values will reduce transfer overheads, while smaller values allow
for more fine-grained consumption.
(较大的分片值会降低转移开销，较小的值则允许更细粒度的消耗。)

Statistics(统计)
----------------
A group's bandwidth statistics are exported via 5 fields in cpu.stat.
(组的带宽统计通过 cpu.stat 中的 5 个字段导出。)

cpu.stat:
(cpu.stat：)

- nr_periods: Number of enforcement intervals that have elapsed.(已经过去的执行间隔数。)
- nr_throttled: Number of times the group has been throttled/limited.(该组被限流/限制的次数。)
- throttled_time: The total time duration (in nanoseconds) for which entities
  of the group have been throttled.
  (组内实体被限流的总时长 (纳秒)。)
- nr_bursts: Number of periods burst occurs.(发生突发的周期数。)
- burst_time: Cumulative wall-time (in nanoseconds) that any CPUs has used
  above quota in respective periods.
  (各周期内任何 CPU 超出配额使用的累计真实时间 (纳秒)。)

This interface is read-only.
(该接口为只读。)

Hierarchical considerations(层级考量)
--------------------------------------
The interface enforces that an individual entity's bandwidth is always
attainable, that is: max(c_i) <= C. However, over-subscription in the
aggregate case is explicitly allowed to enable work-conserving semantics
within a hierarchy:
(该接口强制单个实体的带宽总是可获得的，即 max(c_i) <= C。不过，聚合情况下的超额订阅 (over-subscription) 被明确允许，以便在层级内实现工作保持语义：)

  e.g. \Sum (c_i) may exceed C

[ Where C is the parent's bandwidth, and c_i its children ]
([ 其中 C 是父级的带宽，c_i 是其子级 ])


There are two ways in which a group may become throttled:
(一个组可能因以下两种方式被限流：)

	a. it fully consumes its own quota within a period
	   (a. 在一个周期内用尽自身的全部配额)
	b. a parent's quota is fully consumed within its period
	   (b. 父级的配额在其周期内被用尽)

In case b) above, even though the child may have runtime remaining it will not
be allowed to until the parent's runtime is refreshed.
(在上面的情况 b) 中，即使子组还有剩余运行时间，在父级运行时间刷新之前也不允许使用。)

CFS Bandwidth Quota Caveats(CFS 带宽配额注意事项)
--------------------------------------------------
Once a slice is assigned to a cpu it does not expire.  However all but 1ms of
the slice may be returned to the global pool if all threads on that cpu become
unrunnable. This is configured at compile time by the min_cfs_rq_runtime
variable. This is a performance tweak that helps prevent added contention on
the global lock.
(分片一旦分配给某个 CPU 就不会过期。不过，如果该 CPU 上的所有线程都变为不可运行，分片中除 1ms 以外的部分可以退还给全局池。这在编译期由 min_cfs_rq_runtime 变量配置。这是一项性能调整，有助于防止全局锁上的额外争用。)

The fact that cpu-local slices do not expire results in some interesting corner
cases that should be understood.
(CPU 本地分片不过期这一事实导致了一些值得理解的有趣边界情况。)

For cgroup cpu constrained applications that are cpu limited this is a
relatively moot point because they will naturally consume the entirety of their
quota as well as the entirety of each cpu-local slice in each period. As a
result it is expected that nr_periods roughly equal nr_throttled, and that
cpuacct.usage will increase roughly equal to cfs_quota_us in each period.
(对于受 cgroup CPU 约束且受 CPU 限制的应用来说，这一点基本无关紧要，因为它们自然会在每个周期用尽全部配额以及每个 CPU 本地分片的全部。因此预期 nr_periods 大致等于 nr_throttled，且 cpuacct.usage 每个周期的增长大致等于 cfs_quota_us。)

For highly-threaded, non-cpu bound applications this non-expiration nuance
allows applications to briefly burst past their quota limits by the amount of
unused slice on each cpu that the task group is running on (typically at most
1ms per cpu or as defined by min_cfs_rq_runtime).  This slight burst only
applies if quota had been assigned to a cpu and then not fully used or returned
in previous periods. This burst amount will not be transferred between cores.
As a result, this mechanism still strictly limits the task group to quota
average usage, albeit over a longer time window than a single period.  This
also limits the burst ability to no more than 1ms per cpu.  This provides
better more predictable user experience for highly threaded applications with
small quota limits on high core count machines. It also eliminates the
propensity to throttle these applications while simultaneously using less than
quota amounts of cpu. Another way to say this, is that by allowing the unused
portion of a slice to remain valid across periods we have decreased the
possibility of wastefully expiring quota on cpu-local silos that don't need a
full slice's amount of cpu time.
(对于高度多线程、非 CPU 受限的应用，这种不过期的细节允许应用短暂突发地超过其配额限制，超出量为任务组所运行的每个 CPU 上未用完的分片量 (通常每 CPU 至多 1ms 或由 min_cfs_rq_runtime 定义)。这种轻微突发只在如下情况下才发生：配额此前被分配给某个 CPU 但未用完也未归还。这一突发量不会在核间转移。因此，该机制仍然严格把任务组限制在配额平均用量之内，只不过时间窗口比单个周期更长。这也把突发能力限制在每 CPU 至多 1ms。这为在高核数机器上配额限制较小的高度多线程应用提供了更好、更可预测的用户体验。它还消除了在这些应用 CPU 用量低于配额时仍对其限流的倾向。换句话说，允许分片中未用的部分跨周期保持有效，降低了在不需要整个分片 CPU 时间的 CPU 本地囤积池上浪费性过期配额的可能性。)

The interaction between cpu-bound and non-cpu-bound-interactive applications
should also be considered, especially when single core usage hits 100%. If you
gave each of these applications half of a cpu-core and they both got scheduled
on the same CPU it is theoretically possible that the non-cpu bound application
will use up to 1ms additional quota in some periods, thereby preventing the
cpu-bound application from fully using its quota by that same amount. In these
instances it will be up to the CFS algorithm (see sched-design-CFS.rst) to
decide which application is chosen to run, as they will both be runnable and
have remaining quota. This runtime discrepancy will be made up in the following
periods when the interactive application idles.
(还应考虑 CPU 受限应用与非 CPU 受限交互式应用之间的相互影响，尤其是单核使用率达到 100% 时。如果给这两个应用各半个 CPU 核，而它们都被调度到同一个 CPU 上，理论上非 CPU 受限应用在某些周期可能多用至多 1ms 的额外配额，从而使 CPU 受限应用同样少用这么多配额。在这种情况下，将由 CFS 算法 (见 sched-design-CFS.rst) 决定选择哪个应用运行，因为两者都可运行且都还有剩余配额。当交互式应用空闲时，这种运行时间差异会在随后的周期中得到补偿。)

Examples(示例)
--------------
1. Limit a group to 1 CPU worth of runtime::

	If period is 250ms and quota is also 250ms, the group will get
	1 CPU worth of runtime every 250ms.

	# echo 250000 > cpu.cfs_quota_us /* quota = 250ms */
	# echo 250000 > cpu.cfs_period_us /* period = 250ms */

   (把一个组限制为 1 个 CPU 的运行量。若周期为 250ms、配额也为 250ms，则该组每 250ms 获得 1 个 CPU 的运行量。)

2. Limit a group to 2 CPUs worth of runtime on a multi-CPU machine
   (2. 在多 CPU 机器上把一个组限制为 2 个 CPU 的运行量)

   With 500ms period and 1000ms quota, the group can get 2 CPUs worth of
   runtime every 500ms::

	# echo 1000000 > cpu.cfs_quota_us /* quota = 1000ms */
	# echo 500000 > cpu.cfs_period_us /* period = 500ms */

	The larger period here allows for increased burst capacity.

   (周期 500ms、配额 1000ms，该组每 500ms 可获得 2 个 CPU 的运行量。这里较大的周期允许更大的突发容量。)

3. Limit a group to 20% of 1 CPU.
   (3. 把一个组限制为 1 个 CPU 的 20%。)

   With 50ms period, 10ms quota will be equivalent to 20% of 1 CPU::

	# echo 10000 > cpu.cfs_quota_us /* quota = 10ms */
	# echo 50000 > cpu.cfs_period_us /* period = 50ms */

   (周期 50ms，配额 10ms 等价于 1 个 CPU 的 20%。)
   By using a small period here we are ensuring a consistent latency
   response at the expense of burst capacity.
   (这里使用较小的周期，是以牺牲突发容量为代价来确保一致的延迟响应。)

4. Limit a group to 40% of 1 CPU, and allow accumulate up to 20% of 1 CPU
   additionally, in case accumulation has been done.
   (4. 把一个组限制为 1 个 CPU 的 40%，并允许在已做累积的情况下额外累积至多 1 个 CPU 的 20%。)

   With 50ms period, 20ms quota will be equivalent to 40% of 1 CPU.
   And 10ms burst will be equivalent to 20% of 1 CPU::

	# echo 20000 > cpu.cfs_quota_us /* quota = 20ms */
	# echo 50000 > cpu.cfs_period_us /* period = 50ms */
	# echo 10000 > cpu.cfs_burst_us /* burst = 10ms */

   (周期 50ms，配额 20ms 等价于 1 个 CPU 的 40%；突发 10ms 等价于 1 个 CPU 的 20%。)
   Larger buffer setting (no larger than quota) allows greater burst capacity.
   (更大的缓冲设置 (不大于配额) 允许更大的突发容量。)
