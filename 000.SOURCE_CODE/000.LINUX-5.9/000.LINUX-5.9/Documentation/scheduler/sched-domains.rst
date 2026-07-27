=================
Scheduler Domains
=================

Each CPU has a "base" scheduling domain (struct sched_domain). The domain
hierarchy is built from these base domains via the ->parent pointer. ->parent
MUST be NULL terminated, and domain structures should be per-CPU as they are
locklessly updated.
(每个 CPU 都有一个“基础”调度域（struct sched_domain）。
整个域的层级结构是通过 ->parent 指针从这些基础域构建起来的。
->parent 指针必须以 NULL 结尾，而且域结构体必须是每 CPU 的，因为它们是在无锁状态下进行更新的。)

Each scheduling domain spans a number of CPUs (stored in the ->span field).
A domain's span MUST be a superset of it child's span (this restriction could
be relaxed if the need arises), and a base domain for CPU i MUST span at least
i. The top domain for each CPU will generally span all CPUs in the system
although strictly it doesn't have to, but this could lead to a case where some
CPUs will never be given tasks to run unless the CPUs allowed mask is
explicitly set. A sched domain's span means "balance process load among these
CPUs".
每个调度域都覆盖（span）了一定数量的 CPU（这些 CPU 信息存储在 ->span 字段中）。
一个域的 span 必须是其子域 span 的超集（如果未来有需要，这个限制或许可以放宽），并且针对 CPU i 的基础域必须至少包含 CPU i 本身。
每个 CPU 的顶层域通常会覆盖系统中的所有 CPU，虽然严格来说并不强制要求这样做，但如果顶层域不覆盖所有 CPU，
可能会导致某些 CPU 永远分不到任务运行（如果顶层域没有覆盖某个 CPU，调度器就不会把任务“推”给那个 CPU，导致那个 CPU 闲置）—— 除非显式地设置了 CPU 允许掩码（allowed mask）。
一个调度域的 span 意味着“在这些 CPU 之间进行进程负载均衡”。


Each scheduling domain must have one or more CPU groups (struct sched_group)
which are organised as a circular one way linked list from the ->groups
pointer. The union of cpumasks of these groups MUST be the same as the
domain's span. The group pointed to by the ->groups pointer MUST contain the CPU
to which the domain belongs. Groups may be shared among CPUs as they contain
read only data after they have been set up. The intersection of cpumasks from
any two of these groups may be non empty. If this is the case the SD_OVERLAP
flag is set on the corresponding scheduling domain and its groups may not be
shared between CPUs.
每个调度域必须包含一个或多个 CPU 组（struct sched_group），这些组通过 ->groups 指针组织成一个单向循环链表。
这些组的 CPU 掩码（cpumask）的并集必须与该域的 span（覆盖范围）完全一致。
由 ->groups 指针所指向的那个组，必须包含该调度域所属的那个 CPU。
由于组在设置完成后仅包含只读数据，因此它们可以在多个 CPU 之间共享。
任意两个组的 CPU 掩码的交集可能非空（即组之间可能存在重叠）。如果出现这种情况，相应的调度域会设置 SD_OVERLAP 标志，并且其组不能在 CPU 之间共享。


Balancing within a sched domain occurs between groups. That is, each group
is treated as one entity. The load of a group is defined as the sum of the
load of each of its member CPUs, and only when the load of a group becomes
out of balance are tasks moved between groups. (将调度组(sched_group)视为一个整体)
调度域内的负载均衡是在组（groups）之间进行的。
也就是说，每个组都被视为一个单一的实体。
一个组的负载被定义为其所有成员 CPU 负载的总和。
只有当一个组的负载出现失衡时，任务才会在不同的组之间进行迁移。

In kernel/sched/core.c, trigger_load_balance() is run periodically on each CPU
through scheduler_tick(). It raises a softirq after the next regularly scheduled
rebalancing event for the current runqueue has arrived. The actual load
balancing workhorse, run_rebalance_domains()->rebalance_domains(), is then run
in softirq context (SCHED_SOFTIRQ).
在 kernel/sched/core.c 中，trigger_load_balance() 函数通过 scheduler_tick() 在每个 CPU 上周期性地运行。
当当前运行队列（runqueue）的下一个预定重新平衡事件到期时，它会触发一个软中断（softirq）。
随后，真正的负载均衡主力函数——run_rebalance_domains() -> rebalance_domains()——将在软中断上下文（SCHED_SOFTIRQ）中执行。


The latter function takes two arguments: the current CPU and whether it was idle
at the time the scheduler_tick() happened and iterates over all sched domains
our CPU is on, starting from its base domain and going up the ->parent chain.
While doing that, it checks to see if the current domain has exhausted its
rebalance interval. If so, it runs load_balance() on that domain. It then checks
the parent sched_domain (if it exists), and the parent of the parent and so
forth.
后一个函数接受两个参数：当前的 CPU，以及在 scheduler_tick() 发生时该 CPU 是否处于空闲状态。
它会遍历当前 CPU 所属的所有调度域（sched domains），从其基础域开始，沿着 ->parent 链逐级向上。
在此过程中，它会检查当前域是否已经耗尽了其重新平衡间隔（即是否到了该平衡的时间点）。如果是，它就在该域上运行 load_balance()。
随后，它会检查父级 sched_domain（如果存在），接着是父级的父级，以此类推。
--- 自底向上地遍历调度域层级


Initially, load_balance() finds the busiest group in the current sched domain.
If it succeeds, it looks for the busiest runqueue of all the CPUs' runqueues in
that group. If it manages to find such a runqueue, it locks both our initial
CPU's runqueue and the newly found busiest one and starts moving tasks from it
to our runqueue. The exact number of tasks amounts to an imbalance previously
computed while iterating over this sched domain's groups.
最初，load_balance() 会在当前的调度域中寻找最繁忙的组。
如果寻找成功，它会进一步在该组内所有 CPU 的运行队列中，寻找最繁忙的那个运行队列。
如果成功找到了这样一个运行队列，它会同时锁定我们当前 CPU 的运行队列和新找到的那个最繁忙的运行队列，并开始将任务从那个队列移动到我们的运行队列中。
需要移动的任务的确切数量，等于之前遍历该调度域的各个组时所计算出的不平衡量。
--- 锁定目标，锁定资源，执行搬运任务

Implementing sched domains
==========================

The "base" domain will "span" the first level of the hierarchy. In the case
of SMT, you'll span all siblings of the physical CPU, with each group being
a single virtual CPU.
“基础”域将覆盖层级结构的第一层。
在超线程技术（SMT）的情况下，它将覆盖物理 CPU 的所有兄弟逻辑处理器（siblings），其中每个组代表一个虚拟 CPU。
--
当一个物理核心开启超线程后，它在操作系统看来变成了两个或多个逻辑核心（比如 CPU 0 和 CPU 1 其实是同一个物理核）

组的粒度
- 在这个最底层，每个组只包含一个虚拟 CPU（逻辑处理器）。这意味着调度器是在单个逻辑核心的粒度上进行负载均衡的

In SMP, the parent of the base domain will span all physical CPUs in the
node. Each group being a single physical CPU. Then with NUMA, the parent
of the SMP domain will span the entire machine, with each group having the
cpumask of a node. Or, you could do multi-level NUMA or Opteron, for example,
might have just one domain covering its one NUMA level.
在 SMP（对称多处理） 架构中，基础域的父级域将覆盖该节点内的所有物理 CPU。此时，每个组代表一个物理 CPU。
而在 NUMA（非统一内存访问） 架构中，SMP 域的父级域将覆盖整个机器，此时每个组包含一个节点的 CPU 掩码（cpumask）。
或者，你也可以采用多级 NUMA 架构，例如 Opteron 处理器可能只有一个域来覆盖其单一层级的 NUMA 结构。
--> 阅读 001.UNIX-DOCS/026.SMP/017.SMP负载均衡.md ， 里面有示意图

The implementor should read comments in include/linux/sched.h:
struct sched_domain fields, SD_FLAG_*, SD_*_INIT to get an idea of
the specifics and what to tune.
实现者应当阅读 include/linux/sched.h 文件中的注释：
重点关注 struct sched_domain 的字段、SD_FLAG_* 以及 SD_*_INIT 宏，以便了解具体的细节以及需要调整哪些参数。


Architectures may retain the regular override the default SD_*_INIT flags
while using the generic domain builder in kernel/sched/core.c if they wish to
retain the traditional SMT->SMP->NUMA topology (or some subset of that). This
can be done by #define'ing ARCH_HASH_SCHED_TUNE.
如果硬件架构希望保留传统的 SMT -> SMP -> NUMA 拓扑结构（或其中的某个子集），它们可以在使用 kernel/sched/core.c 中的通用域构建器的同时，选择保留或覆盖默认的 SD_*_INIT 标志。
这可以通过定义 ARCH_HASH_SCHED_TUNE 宏来实现。


Alternatively, the architecture may completely override the generic domain
builder by #define'ing ARCH_HASH_SCHED_DOMAIN, and exporting your
arch_init_sched_domains function. This function will attach domains to all
CPUs using cpu_attach_domain.
或者，硬件架构也可以通过定义 ARCH_HASH_SCHED_DOMAIN 宏，并导出（export）其自定义的 arch_init_sched_domains 函数，来完全覆盖通用的域构建器。
该函数将使用 cpu_attach_domain 将调度域附加到所有 CPU 上。


The sched-domains debugging infrastructure can be enabled by enabling
CONFIG_SCHED_DEBUG. This enables an error checking parse of the sched domains
which should catch most possible errors (described above). It also prints out
the domain structure in a visual format.
可以通过启用 CONFIG_SCHED_DEBUG 配置选项来开启调度域（sched-domains）的调试基础设施。
这将启用对调度域的错误检查解析，应该能捕获大多数可能的错误（如上文所述）。此外，它还会以可视化格式打印出域的结构。

