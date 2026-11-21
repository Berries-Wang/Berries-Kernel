=========================
Scheduler Domains(调度域)
=========================

Each CPU has a "base" scheduling domain (struct sched_domain). The domain
hierarchy is built from these base domains via the ->parent pointer. ->parent
MUST be NULL terminated, and domain structures should be per-CPU as they are
locklessly updated.
(每个 CPU 都有一个"基础"调度域 (struct sched_domain)。域层次结构通过
->parent 指针从这些基础域构建而成。->parent 必须以 NULL 结尾，并且域
结构应当是 per-CPU 的，因为它们会被无锁地更新。)

Each scheduling domain spans a number of CPUs (stored in the ->span field).
A domain's span MUST be a superset of it child's span (this restriction could
be relaxed if the need arises), and a base domain for CPU i MUST span at least
i. The top domain for each CPU will generally span all CPUs in the system
although strictly it doesn't have to, but this could lead to a case where some
CPUs will never be given tasks to run unless the CPUs allowed mask is
explicitly set. A sched domain's span means "balance process load among these
CPUs".
(每个调度域跨越若干个 CPU (存储在 ->span 字段中)。一个域的 span 必须是
其子域 span 的超集 (如有需要可以放宽这一限制)，并且 CPU i 的基础域必
须至少跨越 i。每个 CPU 的顶层域通常会跨越系统中的所有 CPU，尽管严格来
说并非必须，但这可能导致某些 CPU 永远不会被分配任务运行，除非显式设
置 CPUs allowed 掩码。调度域的 span 意味着"在这些 CPU 之间平衡进程负
载"。)

Each scheduling domain must have one or more CPU groups (struct sched_group)
which are organised as a circular one way linked list from the ->groups
pointer. The union of cpumasks of these groups MUST be the same as the
domain's span. The group pointed to by the ->groups pointer MUST contain the CPU
to which the domain belongs. Groups may be shared among CPUs as they contain
read only data after they have been set up. The intersection of cpumasks from
any two of these groups may be non empty. If this is the case the SD_OVERLAP
flag is set on the corresponding scheduling domain and its groups may not be
shared between CPUs.
(每个调度域必须有一个或多个 CPU 组 (struct sched_group)，它们通过
->groups 指针组织成一个单向循环链表。这些组的 cpumask 的并集必须与域
的 span 相同。->groups 指针所指向的组必须包含该域所属的 CPU。组可以在
多个 CPU 之间共享，因为它们在建立之后包含只读数据。这些组中任意两个
组的 cpumask 的交集可以非空。如果是这种情况，则会在相应的调度域上设
置 SD_OVERLAP 标志，并且其组不得在 CPU 之间共享。)

Balancing within a sched domain occurs between groups. That is, each group
is treated as one entity. The load of a group is defined as the sum of the
load of each of its member CPUs, and only when the load of a group becomes
out of balance are tasks moved between groups.
(调度域内的均衡在组之间进行。也就是说，每个组被视为一个实体。组的负
载定义为其各成员 CPU 的负载之和，只有当某个组的负载失去均衡时，任务
才会在组之间移动。)

In kernel/sched/core.c, sched_balance_trigger() is run periodically on each CPU
through sched_tick(). It raises a softirq after the next regularly scheduled
rebalancing event for the current runqueue has arrived. The actual load
balancing workhorse, sched_balance_softirq()->sched_balance_domains(), is then run
in softirq context (SCHED_SOFTIRQ).
(在 kernel/sched/core.c 中，sched_balance_trigger() 通过 sched_tick()
在每个 CPU 上周期性地运行。在当前运行队列的下一次定期重新均衡事件到
达之后，它会触发一个 softirq。实际执行负载均衡工作的
sched_balance_softirq()->sched_balance_domains() 随后在 softirq 上下
文 (SCHED_SOFTIRQ) 中运行。)

The latter function takes two arguments: the runqueue of current CPU and whether
the CPU was idle at the time the sched_tick() happened and iterates over all
sched domains our CPU is on, starting from its base domain and going up the ->parent
chain. While doing that, it checks to see if the current domain has exhausted its
rebalance interval. If so, it runs sched_balance_rq() on that domain. It then checks
the parent sched_domain (if it exists), and the parent of the parent and so
forth.
(后一个函数接受两个参数：当前 CPU 的运行队列，以及 sched_tick() 发生
时该 CPU 是否空闲；它遍历我们 CPU 所在的所有调度域，从其基础域开始沿
->parent 链向上。在此过程中，它检查当前域是否已耗尽其再均衡间隔。如
果是，则在该域上运行 sched_balance_rq()。然后它检查父 sched_domain
(如果存在)，以及父域的父域，依此类推。)

Initially, sched_balance_rq() finds the busiest group in the current sched domain.
If it succeeds, it looks for the busiest runqueue of all the CPUs' runqueues in
that group. If it manages to find such a runqueue, it locks both our initial
CPU's runqueue and the newly found busiest one and starts moving tasks from it
to our runqueue. The exact number of tasks amounts to an imbalance previously
computed while iterating over this sched domain's groups.
(最初，sched_balance_rq() 在当前调度域中找到最繁忙的组。如果成功，它
会在该组中所有 CPU 的运行队列里寻找最繁忙的运行队列。如果找到了这样
的运行队列，它会同时锁定我们初始 CPU 的运行队列和新找到的最繁忙队
列，并开始把任务从后者移到我们的运行队列。任务的确切数量等于之前遍
历该调度域各组时计算出的不均衡量。)

Implementing sched domains(实现调度域)
======================================

The "base" domain will "span" the first level of the hierarchy. In the case
of SMT, you'll span all siblings of the physical CPU, with each group being
a single virtual CPU.
("基础"域将"跨越"层次结构的第一层。就 SMT 而言，你将跨越物理 CPU 的
所有同级线程，每个组是一个单独的虚拟 CPU。)

In SMP, the parent of the base domain will span all physical CPUs in the
node. Each group being a single physical CPU. Then with NUMA, the parent
of the SMP domain will span the entire machine, with each group having the
cpumask of a node. Or, you could do multi-level NUMA or Opteron, for example,
might have just one domain covering its one NUMA level.
(在 SMP 中，基础域的父域将跨越节点中的所有物理 CPU，每个组是一个单独
的物理 CPU。而在 NUMA 中，SMP 域的父域将跨越整台机器，每个组具有一个
节点的 cpumask。或者，你可以做多级 NUMA；例如 Opteron 可能只有一个覆
盖其单一 NUMA 层级的域。)

The implementor should read comments in include/linux/sched/sd_flags.h:
SD_* to get an idea of the specifics and what to tune for the SD flags
of a sched_domain.
(实现者应当阅读 include/linux/sched/sd_flags.h 中关于 SD_* 的注释，以
了解 sched_domain 的 SD 标志的细节以及要针对什么进行调优。)

Architectures may override the generic domain builder and the default SD flags
for a given topology level by creating a sched_domain_topology_level array and
calling set_sched_topology() with this array as the parameter.
(架构可以通过创建一个 sched_domain_topology_level 数组并以该数组为参
数调用 set_sched_topology()，来覆盖给定拓扑层级的通用域构建器与默认
SD 标志。)

The sched-domains debugging infrastructure can be enabled by 'sched_verbose'
to your cmdline. If you forgot to tweak your cmdline, you can also flip the
/sys/kernel/debug/sched/verbose knob. This enables an error checking parse of
the sched domains which should catch most possible errors (described above). It
also prints out the domain structure in a visual format.
(通过在 cmdline 中添加 'sched_verbose' 可以启用调度域调试基础设施。
如果你忘记调整 cmdline，也可以翻转 /sys/kernel/debug/sched/verbose 开
关。这会启用对调度域的错误检查解析，应该能捕获大多数可能的错误 (如上
所述)。它还会以可视化的形式打印出域结构。)
