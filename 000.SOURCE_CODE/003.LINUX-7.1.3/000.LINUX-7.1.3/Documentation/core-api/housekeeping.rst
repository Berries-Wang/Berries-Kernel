======================
Housekeeping(内务处理)
======================


CPU Isolation moves away kernel work that may otherwise run on any CPU.
The purpose of its related features is to reduce the OS jitter that some
extreme workloads can't stand, such as in some DPDK usecases.
(CPU 隔离把原本可能在任何 CPU 上运行的内核工作移走。其相关特性的目的是减少某些极端工作负载无法忍受的 OS 抖动，例如某些 DPDK 使用场景。)

The kernel work moved away by CPU isolation is commonly described as
"housekeeping" because it includes ground work that performs cleanups,
statistics maintainance and actions relying on them, memory release,
various deferrals etc...
(CPU 隔离移走的内核工作通常被描述为"内务处理"（housekeeping），因为它包括执行清理、维护统计信息及依赖它们的操作、内存释放、各种延迟工作等基础工作。)

Sometimes housekeeping is just some unbound work (unbound workqueues,
unbound timers, ...) that gets easily assigned to non-isolated CPUs.
But sometimes housekeeping is tied to a specific CPU and requires
elaborate tricks to be offloaded to non-isolated CPUs (RCU_NOCB, remote
scheduler tick, etc...).
(有时内务处理只是一些非绑定工作（非绑定 workqueue、非绑定定时器等），可以轻松分配给非隔离的 CPU。但有时内务处理与特定 CPU 绑定，需要精巧的技巧才能卸载到非隔离的 CPU（RCU_NOCB、远程 scheduler tick 等）。)

Thus, a housekeeping CPU can be considered as the reverse of an isolated
CPU. It is simply a CPU that can execute housekeeping work. There must
always be at least one online housekeeping CPU at any time. The CPUs that
are not	isolated are automatically assigned as housekeeping.
(因此，内务处理 CPU 可以看作隔离 CPU 的反面。它只是一个能够执行内务处理工作的 CPU。任何时刻都必须至少有一个在线的内务处理 CPU。未隔离的 CPU 会被自动指定为内务处理 CPU。)

Housekeeping is currently divided in four features described
by the ``enum hk_type type``:
(内务处理目前分为四个特性，由 ``enum hk_type type`` 描述：)

1.	HK_TYPE_DOMAIN matches the work moved away by scheduler domain
	isolation performed through ``isolcpus=domain`` boot parameter or
	isolated cpuset partitions in cgroup v2. This includes scheduler
	load balancing, unbound workqueues and timers.
        (HK_TYPE_DOMAIN 对应通过 ``isolcpus=domain`` 启动参数或 cgroup v2 中隔离的 cpuset 分区执行的调度器域隔离所移走的工作。这包括调度器负载均衡、非绑定 workqueue 和定时器。)

2.	HK_TYPE_KERNEL_NOISE matches the work moved away by tick isolation
	performed through ``nohz_full=`` or ``isolcpus=nohz`` boot
	parameters. This includes remote scheduler tick, vmstat and lockup
	watchdog.
        (HK_TYPE_KERNEL_NOISE 对应通过 ``nohz_full=`` 或 ``isolcpus=nohz`` 启动参数执行的 tick 隔离所移走的工作。这包括远程 scheduler tick、vmstat 和 lockup watchdog。)

3.	HK_TYPE_MANAGED_IRQ matches the IRQ handlers moved away by managed
	IRQ isolation performed through ``isolcpus=managed_irq``.
        (HK_TYPE_MANAGED_IRQ 对应通过 ``isolcpus=managed_irq`` 执行的受管 IRQ 隔离所移走的 IRQ 处理程序。)

4.	HK_TYPE_DOMAIN_BOOT matches the work moved away by scheduler domain
	isolation performed through ``isolcpus=domain`` only. It is similar
	to HK_TYPE_DOMAIN except it ignores the isolation performed by
	cpusets.
        (HK_TYPE_DOMAIN_BOOT 仅对应通过 ``isolcpus=domain`` 执行的调度器域隔离所移走的工作。它与 HK_TYPE_DOMAIN 类似，但它忽略由 cpuset 执行的隔离。)


Housekeeping cpumasks(内务处理 cpumask 掩码)
============================================

Housekeeping cpumasks include the CPUs that can execute the work moved
away by the matching isolation feature. These cpumasks are returned by
the following function::

	const struct cpumask *housekeeping_cpumask(enum hk_type type)

(内务处理 cpumask 包含能够执行由匹配的隔离特性所移走工作的 CPU。这些 cpumask 由以下函数返回：)

By default, if neither ``nohz_full=``, nor ``isolcpus``, nor cpuset's
isolated partitions are used, which covers most usecases, this function
returns the cpu_possible_mask.
(默认情况下，如果既未使用 ``nohz_full=`` 、也未使用 ``isolcpus`` 、也未使用 cpuset 的隔离分区（这覆盖了大多数使用场景），此函数返回 cpu_possible_mask。)

Otherwise the function returns the cpumask complement of the isolation
feature. For example:
(否则，该函数返回该隔离特性的补集 cpumask。例如：)

With isolcpus=domain,7 the following will return a mask with all possible
CPUs except 7::

	housekeeping_cpumask(HK_TYPE_DOMAIN)

(使用 isolcpus=domain,7 时，以下调用将返回一个包含除 7 之外所有可能 CPU 的掩码：)

Similarly with nohz_full=5,6 the following will return a mask with all
possible CPUs except 5,6::

	housekeeping_cpumask(HK_TYPE_KERNEL_NOISE)

(类似地，使用 nohz_full=5,6 时，以下调用将返回一个包含除 5、6 之外所有可能 CPU 的掩码：)


Synchronization against cpusets(与 cpuset 同步)
===============================================

Cpuset can modify the HK_TYPE_DOMAIN housekeeping cpumask while creating,
modifying or deleting an isolated partition.
(在创建、修改或删除隔离分区时，cpuset 可以修改 HK_TYPE_DOMAIN 内务处理 cpumask。)

The users of HK_TYPE_DOMAIN cpumask must then make sure to synchronize
properly against cpuset in order to make sure that:
(HK_TYPE_DOMAIN cpumask 的使用者随后必须确保与 cpuset 正确同步，以保证：)

1.	The cpumask snapshot stays coherent.
        (cpumask 快照保持一致。)

2.	No housekeeping work is queued on a newly made isolated CPU.
        (不会有内务处理工作被排入新变为隔离的 CPU。)

3.	Pending housekeeping work that was queued to a non isolated
	CPU which just turned isolated through cpuset must be flushed
	before the related created/modified isolated partition is made
	available to userspace.
        (在被排入一个刚通过 cpuset 变为隔离的非隔离 CPU 上的未完成内务处理工作，必须在相关的新建/修改的隔离分区对用户空间可用之前被刷新。)

This synchronization is maintained by an RCU based scheme. The cpuset update
side waits for an RCU grace period after updating the HK_TYPE_DOMAIN
cpumask and before flushing pending works. On the read side, care must be
taken to gather the housekeeping target election and the work enqueue within
the same RCU read side critical section.
(这种同步由一个基于 RCU 的方案维护。cpuset 更新一侧在更新 HK_TYPE_DOMAIN cpumask 之后、刷新未完成工作之前等待一个 RCU 宽限期。在读一侧，必须注意把内务处理目标的选择和工作入队放在同一个 RCU 读侧临界区内完成。)

A typical layout example would look like this on the update side
(``housekeeping_update()``)::

	rcu_assign_pointer(housekeeping_cpumasks[type], trial);
	synchronize_rcu();
	flush_workqueue(example_workqueue);

(更新一侧的典型布局示例如下（ ``housekeeping_update()`` ）：)

And then on the read side::

	rcu_read_lock();
	cpu = housekeeping_any_cpu(HK_TYPE_DOMAIN);
	queue_work_on(cpu, example_workqueue, work);
	rcu_read_unlock();

(然后在读一侧：)
