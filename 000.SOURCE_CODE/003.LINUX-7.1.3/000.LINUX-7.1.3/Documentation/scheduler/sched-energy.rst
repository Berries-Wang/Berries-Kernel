=====================================
Energy Aware Scheduling(能效感知调度)
=====================================

1. Introduction(1. 引言)
------------------------

Energy Aware Scheduling (or EAS) gives the scheduler the ability to predict
the impact of its decisions on the energy consumed by CPUs. EAS relies on an
Energy Model (EM) of the CPUs to select an energy efficient CPU for each task,
with a minimal impact on throughput. This document aims at providing an
introduction on how EAS works, what are the main design decisions behind it, and
details what is needed to get it to run.
(能效感知调度 (Energy Aware Scheduling，简称 EAS) 让调度器能够预测其决策对 CPU 能耗的影响。EAS 依赖 CPU 的能量模型 (Energy Model，EM) 来为每个任务选择能效最高、对吞吐量影响最小的 CPU。本文档旨在介绍 EAS 的工作原理、其背后的主要设计决策，并详细说明让它运行起来需要哪些条件。)

Before going any further, please note that at the time of writing::

   /!\ EAS does not support platforms with symmetric CPU topologies /!\

(EAS 不支持对称 CPU 拓扑的平台。)

EAS operates only on heterogeneous CPU topologies (such as Arm big.LITTLE)
because this is where the potential for saving energy through scheduling is
the highest.
(EAS 只在非对称 CPU 拓扑(例如 Arm big.LITTLE)上工作，因为在这类平台上，通过调度节省能耗的潜力最大。)

The actual EM used by EAS is _not_ maintained by the scheduler, but by a
dedicated framework. For details about this framework and what it provides,
please refer to its documentation (see Documentation/power/energy-model.rst).
(EAS 实际使用的 EM 并不由调度器维护，而是由一个专门的框架维护。关于该框架及其提供能力的细节，请参阅其文档(参见 Documentation/power/energy-model.rst)。)


2. Background and Terminology(2. 背景与术语)
--------------------------------------------

To make it clear from the start:
 - energy = [joule] (resource like a battery on powered devices)
   (能量 = [焦耳]，即供电设备上电池一类的资源)
 - power = energy/time = [joule/second] = [watt]
   (功率 = 能量/时间 = [焦耳/秒] = [瓦特])

The goal of EAS is to minimize energy, while still getting the job done. That
is, we want to maximize::

	performance [inst/s]
	--------------------
	    power [W]

(EAS 的目标是在把工作做完的前提下尽量减少能量消耗。也就是说，我们希望最大化：)

which is equivalent to minimizing::

	energy [J]
	-----------
	instruction

(这等价于最小化：)

while still getting 'good' performance. It is essentially an alternative
optimization objective to the current performance-only objective for the
scheduler. This alternative considers two objectives: energy-efficiency and
performance.
(同时仍获得"良好"的性能。它本质上是调度器当前"只看性能"这一优化目标之外的另一种优化目标。这种替代方案同时考虑两个目标：能效与性能。)

The idea behind introducing an EM is to allow the scheduler to evaluate the
implications of its decisions rather than blindly applying energy-saving
techniques that may have positive effects only on some platforms. At the same
time, the EM must be as simple as possible to minimize the scheduler latency
impact.
(引入 EM 背后的想法，是让调度器能够评估其决策带来的后果，而不是盲目套用那些可能只在某些平台上有正面效果的节能技术。与此同时，EM 必须尽可能简单，以把对调度器时延的影响降到最低。)

In short, EAS changes the way CFS tasks are assigned to CPUs. When it is time
for the scheduler to decide where a task should run (during wake-up), the EM
is used to break the tie between several good CPU candidates and pick the one
that is predicted to yield the best energy consumption without harming the
system's throughput. The predictions made by EAS rely on specific elements of
knowledge about the platform's topology, which include the 'capacity' of CPUs,
and their respective energy costs.
(简而言之，EAS 改变了 CFS 任务分配到 CPU 的方式。当调度器需要决定任务应在哪里运行(唤醒期间)时，EM 被用来在几个都不错的 CPU 候选者之间做出取舍，挑选出预计能在不损害系统吞吐量的情况下带来最低能耗的那个。EAS 所做的预测依赖关于平台拓扑的若干特定要素，包括各 CPU 的"容量" (capacity) 及其各自的能耗开销。)


3. Topology information(3. 拓扑信息)
------------------------------------

EAS (as well as the rest of the scheduler) uses the notion of 'capacity' to
differentiate CPUs with different computing throughput. The 'capacity' of a CPU
represents the amount of work it can absorb when running at its highest
frequency compared to the most capable CPU of the system. Capacity values are
normalized in a 1024 range, and are comparable with the utilization signals of
tasks and CPUs computed by the Per-Entity Load Tracking (PELT) mechanism. Thanks
to capacity and utilization values, EAS is able to estimate how big/busy a
task/CPU is, and to take this into consideration when evaluating performance vs
energy trade-offs. The capacity of CPUs is provided via arch-specific code
through the arch_scale_cpu_capacity() callback.
(EAS(以及调度器的其余部分)使用"容量" (capacity) 的概念来区分计算吞吐量不同的 CPU。一个 CPU 的"容量"表示它与系统中能力最强的 CPU 相比、在最高频率运行时可承受的工作量。容量值被归一化到 1024 的范围内，可与由单实体负载跟踪 (Per-Entity Load Tracking，PELT) 机制计算出的任务和 CPU 利用率信号直接比较。借助容量值和利用率值，EAS 能够估计任务/CPU 的大小与繁忙程度，并在权衡性能与能耗时将其纳入考量。CPU 的容量由体系结构相关代码通过 arch_scale_cpu_capacity() 回调提供。)

The rest of platform knowledge used by EAS is directly read from the Energy
Model (EM) framework. The EM of a platform is composed of a power cost table
per 'performance domain' in the system (see Documentation/power/energy-model.rst
for further details about performance domains).
(EAS 使用的其余平台知识直接读取自能量模型 (EM) 框架。平台的 EM 由系统中每个"性能域" (performance domain) 一张的功耗成本表构成(关于性能域的更多细节参见 Documentation/power/energy-model.rst)。)

The scheduler manages references to the EM objects in the topology code when the
scheduling domains are built, or re-built. For each root domain (rd), the
scheduler maintains a singly linked list of all performance domains intersecting
the current rd->span. Each node in the list contains a pointer to a struct
em_perf_domain as provided by the EM framework.
(在调度域被构建或重建时，调度器在拓扑代码中管理对 EM 对象的引用。对于每个根域 (rd)，调度器维护一个单链表，其中包含与当前 rd->span 相交的所有性能域。链表的每个节点都含有一个指向 EM 框架所提供的 struct em_perf_domain 的指针。)

The lists are attached to the root domains in order to cope with exclusive
cpuset configurations. Since the boundaries of exclusive cpusets do not
necessarily match those of performance domains, the lists of different root
domains can contain duplicate elements.
(这些链表挂在根域上，以应对独占 cpuset 配置。由于独占 cpuset 的边界不一定与性能域的边界重合，不同根域的链表可能包含重复的元素。)

Example 1.
(例 1。)
    Let us consider a platform with 12 CPUs, split in 3 performance domains
    (pd0, pd4 and pd8), organized as follows::

	          CPUs:   0 1 2 3 4 5 6 7 8 9 10 11
	          PDs:   |--pd0--|--pd4--|---pd8---|
	          RDs:   |----rd1----|-----rd2-----|

    (让我们考虑一个有 12 个 CPU、划分为 3 个性能域 (pd0、pd4 与 pd8) 的平台，组织如下：)

    Now, consider that userspace decided to split the system with two
    exclusive cpusets, hence creating two independent root domains, each
    containing 6 CPUs. The two root domains are denoted rd1 and rd2 in the
    above figure. Since pd4 intersects with both rd1 and rd2, it will be
    present in the linked list '->pd' attached to each of them:

       * rd1->pd: pd0 -> pd4
       * rd2->pd: pd4 -> pd8

    (现在，假设用户空间决定用两个独占 cpuset 把系统一分为二，从而创建两个各自包含 6 个 CPU 的独立根域。这两个根域在上图中记作 rd1 与 rd2。由于 pd4 与 rd1、rd2 都相交，它会出现在各自附着的 '->pd' 链表中：)

    Please note that the scheduler will create two duplicate list nodes for
    pd4 (one for each list). However, both just hold a pointer to the same
    shared data structure of the EM framework.

    (请注意，调度器会为 pd4 创建两个重复的链表节点(每个链表一个)。不过，两者都只是持有指向 EM 框架同一份共享数据结构的指针。)

Since the access to these lists can happen concurrently with hotplug and other
things, they are protected by RCU, like the rest of topology structures
manipulated by the scheduler.
(由于对这些链表的访问可能与热插拔等操作并发发生，它们和调度器操纵的其他拓扑结构一样由 RCU 保护。)

EAS also maintains a static key (sched_energy_present) which is enabled when at
least one root domain meets all conditions for EAS to start. Those conditions
are summarized in Section 6.
(EAS 还维护一个静态键 (static key) sched_energy_present，当至少一个根域满足 EAS 启动的全部条件时它会被使能。这些条件汇总在第 6 节。)


4. Energy-Aware task placement(4. 能效感知的任务放置)
------------------------------------------------------

EAS overrides the CFS task wake-up balancing code. It uses the EM of the
platform and the PELT signals to choose an energy-efficient target CPU during
wake-up balance. When EAS is enabled, select_task_rq_fair() calls
find_energy_efficient_cpu() to do the placement decision. This function looks
for the CPU with the highest spare capacity (CPU capacity - CPU utilization) in
each performance domain since it is the one which will allow us to keep the
frequency the lowest. Then, the function checks if placing the task there could
save energy compared to leaving it on prev_cpu, i.e. the CPU where the task ran
in its previous activation.
(EAS 接管了 CFS 任务的唤醒均衡代码。在唤醒均衡期间，它使用平台的 EM 和 PELT 信号来挑选能效高的目标 CPU。EAS 启用后，select_task_rq_fair() 会调用 find_energy_efficient_cpu() 来做放置决策。该函数在每个性能域中寻找空闲容量 (CPU 容量 - CPU 利用率) 最大的 CPU，因为这样才有可能让频率保持在最低。然后，该函数会检查把任务放在那里是否比让它留在 prev_cpu(即任务上一次运行时所在的 CPU)更省电。)

find_energy_efficient_cpu() uses compute_energy() to estimate what will be the
energy consumed by the system if the waking task was migrated. compute_energy()
looks at the current utilization landscape of the CPUs and adjusts it to
'simulate' the task migration. The EM framework provides the em_pd_energy() API
which computes the expected energy consumption of each performance domain for
the given utilization landscape.
(find_energy_efficient_cpu() 使用 compute_energy() 来估算如果唤醒的任务被迁移，系统将消耗多少能量。compute_energy() 查看各 CPU 当前的利用率分布，并对其进行调整以"模拟"任务迁移。EM 框架提供 em_pd_energy() API，用于计算在给定利用率分布下每个性能域的预期能耗。)

An example of energy-optimized task placement decision is detailed below.
(下面详细给出一个以能效为优化目标的任务放置决策示例。)

Example 2.
(例 2。)
    Let us consider a (fake) platform with 2 independent performance domains
    composed of two CPUs each. CPU0 and CPU1 are little CPUs; CPU2 and CPU3
    are big.

    (让我们考虑一个(虚构的)平台，它有 2 个相互独立的性能域，每个性能域由两个 CPU 组成。CPU0 和 CPU1 是小核；CPU2 和 CPU3 是大核。)

    The scheduler must decide where to place a task P whose util_avg = 200
    and prev_cpu = 0.

    (调度器必须决定把 util_avg = 200 且 prev_cpu = 0 的任务 P 放到哪里。)

    The current utilization landscape of the CPUs is depicted on the graph
    below. CPUs 0-3 have a util_avg of 400, 100, 600 and 500 respectively
    Each performance domain has three Operating Performance Points (OPPs).
    The CPU capacity and power cost associated with each OPP is listed in
    the Energy Model table. The util_avg of P is shown on the figures
    below as 'PP'::

     CPU util.
      1024                 - - - - - - -              Energy Model
                                               +-----------+-------------+
                                               |  Little   |     Big     |
       768                 =============       +-----+-----+------+------+
                                               | Cap | Pwr | Cap  | Pwr  |
                                               +-----+-----+------+------+
       512  ===========    - ##- - - - -       | 170 | 50  | 512  | 400  |
                             ##     ##         | 341 | 150 | 768  | 800  |
       341  -PP - - - -      ##     ##         | 512 | 300 | 1024 | 1700 |
             PP              ##     ##         +-----+-----+------+------+
       170  -## - - - -      ##     ##
             ##     ##       ##     ##
           ------------    -------------
            CPU0   CPU1     CPU2   CPU3

      Current OPP: =====       Other OPP: - - -     util_avg (100 each): ##

    (各 CPU 当前的利用率分布如下图所示。CPU0-3 的 util_avg 分别为 400、100、600 和 500。每个性能域有三个工作性能点 (Operating Performance Point，OPP)。每个 OPP 对应的 CPU 容量与功耗成本列在能量模型表中。P 的 util_avg 在下图中以 'PP' 标出：)

    find_energy_efficient_cpu() will first look for the CPUs with the
    maximum spare capacity in the two performance domains. In this example,
    CPU1 and CPU3. Then it will estimate the energy of the system if P was
    placed on either of them, and check if that would save some energy
    compared to leaving P on CPU0. EAS assumes that OPPs follow utilization
    (which is coherent with the behaviour of the schedutil CPUFreq
    governor, see Section 6. for more details on this topic).

    (find_energy_efficient_cpu() 会首先在两个性能域中寻找空闲容量最大的 CPU。在本例中是 CPU1 与 CPU3。然后它会估算把 P 放到其中任一 CPU 上时系统的能耗，并与把 P 留在 CPU0 上的情形比较，看是否更省电。EAS 假设 OPP 跟随利用率变化(这与 schedutil CPUFreq 调频器的行为一致，关于此话题的更多细节见第 6 节)。)

    **Case 1. P is migrated to CPU1**::

      1024                 - - - - - - -

                                            Energy calculation:
       768                 =============     * CPU0: 200 / 341 * 150 = 88
                                             * CPU1: 300 / 341 * 150 = 131
                                             * CPU2: 600 / 768 * 800 = 625
       512  - - - - - -    - ##- - - - -     * CPU3: 500 / 768 * 800 = 520
                             ##     ##          => total_energy = 1364
       341  ===========      ##     ##
                    PP       ##     ##
       170  -## - - PP-      ##     ##
             ##     ##       ##     ##
           ------------    -------------
            CPU0   CPU1     CPU2   CPU3

    (情况 1. P 迁移到 CPU1。各 CPU 能耗计算：CPU0: 200 / 341 * 150 = 88；CPU1: 300 / 341 * 150 = 131；CPU2: 600 / 768 * 800 = 625；CPU3: 500 / 768 * 800 = 520；总能耗 total_energy = 1364。)

    **Case 2. P is migrated to CPU3**::

      1024                 - - - - - - -

                                            Energy calculation:
       768                 =============     * CPU0: 200 / 341 * 150 = 88
                                             * CPU1: 100 / 341 * 150 = 43
                                    PP       * CPU2: 600 / 768 * 800 = 625
       512  - - - - - -    - ##- - -PP -     * CPU3: 700 / 768 * 800 = 729
                             ##     ##          => total_energy = 1485
       341  ===========      ##     ##
                             ##     ##
       170  -## - - - -      ##     ##
             ##     ##       ##     ##
           ------------    -------------
            CPU0   CPU1     CPU2   CPU3

    (情况 2. P 迁移到 CPU3。各 CPU 能耗计算：CPU0: 200 / 341 * 150 = 88；CPU1: 100 / 341 * 150 = 43；CPU2: 600 / 768 * 800 = 625；CPU3: 700 / 768 * 800 = 729；总能耗 total_energy = 1485。)

    **Case 3. P stays on prev_cpu / CPU 0**::

      1024                 - - - - - - -

                                            Energy calculation:
       768                 =============     * CPU0: 400 / 512 * 300 = 234
                                             * CPU1: 100 / 512 * 300 = 58
                                             * CPU2: 600 / 768 * 800 = 625
       512  ===========    - ##- - - - -     * CPU3: 500 / 768 * 800 = 520
                             ##     ##          => total_energy = 1437
       341  -PP - - - -      ##     ##
             PP              ##     ##
       170  -## - - - -      ##     ##
             ##     ##       ##     ##
           ------------    -------------
            CPU0   CPU1     CPU2   CPU3

    (情况 3. P 留在 prev_cpu / CPU0。各 CPU 能耗计算：CPU0: 400 / 512 * 300 = 234；CPU1: 100 / 512 * 300 = 58；CPU2: 600 / 768 * 800 = 625；CPU3: 500 / 768 * 800 = 520；总能耗 total_energy = 1437。)

    From these calculations, the Case 1 has the lowest total energy. So CPU 1
    is the best candidate from an energy-efficiency standpoint.

    (从这些计算来看，情况 1 的总能耗最低。因此从能效角度看，CPU1 是最佳候选。)

Big CPUs are generally more power hungry than the little ones and are thus used
mainly when a task doesn't fit the littles. However, little CPUs aren't always
necessarily more energy-efficient than big CPUs. For some systems, the high OPPs
of the little CPUs can be less energy-efficient than the lowest OPPs of the
bigs, for example. So, if the little CPUs happen to have enough utilization at
a specific point in time, a small task waking up at that moment could be better
off executing on the big side in order to save energy, even though it would fit
on the little side.
(大核通常比小核更耗电，因此主要用在任务在小核上放不下的场合。然而，小核并不必然总比大核更省电。例如，对某些系统而言，小核的高档 OPP 可能还不如大核的最低档 OPP 省电。所以，如果小核在某个时刻恰好已有足够的利用率，那么在该时刻唤醒的小任务即使能放进小核，执行在大核上反而可能更省电。)

And even in the case where all OPPs of the big CPUs are less energy-efficient
than those of the little, using the big CPUs for a small task might still, under
specific conditions, save energy. Indeed, placing a task on a little CPU can
result in raising the OPP of the entire performance domain, and that will
increase the cost of the tasks already running there. If the waking task is
placed on a big CPU, its own execution cost might be higher than if it was
running on a little, but it won't impact the other tasks of the little CPUs
which will keep running at a lower OPP. So, when considering the total energy
consumed by CPUs, the extra cost of running that one task on a big core can be
smaller than the cost of raising the OPP on the little CPUs for all the other
tasks.
(即使在大核的所有 OPP 都不如小核省电的情况下，在特定条件下把小任务放到大核上仍可能省电。事实上，把任务放到一个小核上可能导致整个性能域的 OPP 升档，而这会增加已在该域上运行的任务的成本。如果唤醒的任务被放到大核上，它自身的执行成本可能比在小核上更高，但不会影响小核上继续以较低 OPP 运行的其他任务。因此，从 CPU 消耗的总能量来看，让这一个任务跑在大核上的额外成本，可能小于让小核上所有其他任务的 OPP 都升档的成本。)

The examples above would be nearly impossible to get right in a generic way, and
for all platforms, without knowing the cost of running at different OPPs on all
CPUs of the system. Thanks to its EM-based design, EAS should cope with them
correctly without too many troubles. However, in order to ensure a minimal
impact on throughput for high-utilization scenarios, EAS also implements another
mechanism called 'over-utilization'.
(如果不知道系统所有 CPU 在不同 OPP 上运行的成本，上述例子几乎不可能以通用方式、对所有平台都做出正确处理。得益于基于 EM 的设计，EAS 应能毫无大碍地正确应对这些情形。不过，为了确保高利用率场景下对吞吐量的影响降到最低，EAS 还实现了另一种机制，即"过载使用" (over-utilization)。)


5. Over-utilization(5. 过载使用)
--------------------------------

From a general standpoint, the use-cases where EAS can help the most are those
involving a light/medium CPU utilization. Whenever long CPU-bound tasks are
being run, they will require all of the available CPU capacity, and there isn't
much that can be done by the scheduler to save energy without severely harming
throughput. In order to avoid hurting performance with EAS, CPUs are flagged as
'over-utilized' as soon as they are used at more than 80% of their compute
capacity. As long as no CPUs are over-utilized in a root domain, load balancing
is disabled and EAS overrides the wake-up balancing code. EAS is likely to load
the most energy efficient CPUs of the system more than the others if that can be
done without harming throughput. So, the load-balancer is disabled to prevent
it from breaking the energy-efficient task placement found by EAS. It is safe to
do so when the system isn't overutilized since being below the 80% tipping point
implies that:

    a. there is some idle time on all CPUs, so the utilization signals used by
       EAS are likely to accurately represent the 'size' of the various tasks
       in the system;
    b. all tasks should already be provided with enough CPU capacity,
       regardless of their nice values;
    c. since there is spare capacity all tasks must be blocking/sleeping
       regularly and balancing at wake-up is sufficient.

(从总体上看，EAS 能帮上最大忙的用例是那些 CPU 利用率较低或中等的场景。只要长 CPU 密集型任务在运行，它们就会占满全部可用 CPU 容量，调度器在不严重损害吞吐量的前提下能做的节能动作不多。为了避免 EAS 损害性能，一旦 CPU 的使用率超过其计算容量的 80%，就会被标记为"过载" (over-utilized)。只要某个根域内没有 CPU 过载，负载均衡就被禁用，由 EAS 接管唤醒均衡代码。在不损害吞吐量的前提下，EAS 很可能把系统中能效最高的 CPU 负载得更重一些。因此，负载均衡器被禁用，以防止它破坏 EAS 找到的能效任务放置。在系统未过载时这样做是安全的，因为低于 80% 这个临界点意味着：)

    a. there is some idle time on all CPUs, so the utilization signals used by
       EAS are likely to accurately represent the 'size' of the various tasks
       in the system;
       (所有 CPU 上都有一些空闲时间，因此 EAS 使用的利用率信号能够较准确地代表系统中各个任务的"大小"；)
    b. all tasks should already be provided with enough CPU capacity,
       regardless of their nice values;
       (无论 nice 值如何，所有任务都应已获得足够的 CPU 容量；)
    c. since there is spare capacity all tasks must be blocking/sleeping
       regularly and balancing at wake-up is sufficient.
       (既然还有空闲容量，所有任务必然在规律地阻塞/休眠，唤醒时做均衡就足够了。)

As soon as one CPU goes above the 80% tipping point, at least one of the three
assumptions above becomes incorrect. In this scenario, the 'overutilized' flag
is raised for the entire root domain, EAS is disabled, and the load-balancer is
re-enabled. By doing so, the scheduler falls back onto load-based algorithms for
wake-up and load balance under CPU-bound conditions. This provides a better
respect of the nice values of tasks.
(一旦某个 CPU 越过 80% 临界点，上述三个假设中至少有一个不再成立。在这种情形下，整个根域都会置上 'overutilized' 标志，EAS 被禁用，负载均衡器被重新启用。这样一来，在 CPU 密集条件下，调度器在唤醒与负载均衡上回退到基于负载的算法。这能更好地尊重任务的 nice 值。)

Since the notion of overutilization largely relies on detecting whether or not
there is some idle time in the system, the CPU capacity 'stolen' by higher
(than CFS) scheduling classes (as well as IRQ) must be taken into account. As
such, the detection of overutilization accounts for the capacity used not only
by CFS tasks, but also by the other scheduling classes and IRQ.
(由于过载的概念在很大程度上依赖检测系统中是否还有空闲时间，更高调度类(高于 CFS 的类)以及中断"偷走"的 CPU 容量必须被计入。因此，过载检测不仅考虑 CFS 任务使用的容量，也考虑其他调度类和中断使用的容量。)


6. Dependencies and requirements for EAS(6. EAS 的依赖与要求)
-------------------------------------------------------------

Energy Aware Scheduling depends on the CPUs of the system having specific
hardware properties and on other features of the kernel being enabled. This
section lists these dependencies and provides hints as to how they can be met.
(能效感知调度要求系统的 CPU 具备特定硬件属性，并要求内核启用其他一些特性。本节列出这些依赖项，并就如何满足它们给出提示。)


6.1 - Asymmetric CPU topology(6.1 - 非对称 CPU 拓扑)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


As mentioned in the introduction, EAS is only supported on platforms with
asymmetric CPU topologies for now. This requirement is checked at run-time by
looking for the presence of the SD_ASYM_CPUCAPACITY_FULL flag when the scheduling
domains are built.
(如引言所述，EAS 目前只支持具有非对称 CPU 拓扑的平台。该要求在运行时通过在构建调度域时检查是否存在 SD_ASYM_CPUCAPACITY_FULL 标志来验证。)

See Documentation/scheduler/sched-capacity.rst for requirements to be met for this
flag to be set in the sched_domain hierarchy.
(要在 sched_domain 层级中置位该标志所需满足的要求，参见 Documentation/scheduler/sched-capacity.rst。)

Please note that EAS is not fundamentally incompatible with SMP, but no
significant savings on SMP platforms have been observed yet. This restriction
could be amended in the future if proven otherwise.
(请注意，EAS 与 SMP 并非根本不兼容，但目前在 SMP 平台上尚未观察到显著的节能效果。如果日后有证据表明并非如此，这一限制可能会被修订。)


6.2 - Energy Model presence(6.2 - 能量模型的就绪)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

EAS uses the EM of a platform to estimate the impact of scheduling decisions on
energy. So, your platform must provide power cost tables to the EM framework in
order to make EAS start. To do so, please refer to documentation of the
independent EM framework in Documentation/power/energy-model.rst.
(EAS 使用平台的 EM 来估计调度决策对能耗的影响。因此，平台必须向 EM 框架提供功耗成本表，EAS 才会启动。具体做法请参阅 Documentation/power/energy-model.rst 中关于独立 EM 框架的文档。)

Please also note that the scheduling domains need to be re-built after the
EM has been registered in order to start EAS.
(另请注意，在 EM 注册之后需要重建调度域，EAS 才会启动。)

EAS uses the EM to make a forecasting decision on energy usage and thus it is
more focused on the difference when checking possible options for task
placement. For EAS it doesn't matter whether the EM power values are expressed
in milli-Watts or in an 'abstract scale'.
(EAS 使用 EM 对能耗做前瞻性决策，因而在检查任务放置的可选方案时它更关注差值。对 EAS 来说，EM 的功耗值以毫瓦还是以"抽象刻度"表示并无区别。)


6.3 - Energy Model complexity(6.3 - 能量模型的复杂度)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

EAS does not impose any complexity limit on the number of PDs/OPPs/CPUs but
restricts the number of CPUs to EM_MAX_NUM_CPUS to prevent overflows during
the energy estimation.
(EAS 不对 PD/OPP/CPU 的数量施加任何复杂度限制，但把 CPU 数量限制为 EM_MAX_NUM_CPUS，以防止能耗估算期间发生溢出。)


6.4 - Schedutil governor(6.4 - Schedutil 调频器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

EAS tries to predict at which OPP will the CPUs be running in the close future
in order to estimate their energy consumption. To do so, it is assumed that OPPs
of CPUs follow their utilization.
(EAS 试图预测 CPU 在不远的将来会运行在哪个 OPP 上，以估算其能耗。为此，它假设 CPU 的 OPP 跟随其利用率变化。)

Although it is very difficult to provide hard guarantees regarding the accuracy
of this assumption in practice (because the hardware might not do what it is
told to do, for example), schedutil as opposed to other CPUFreq governors at
least _requests_ frequencies calculated using the utilization signals.
Consequently, the only sane governor to use together with EAS is schedutil,
because it is the only one providing some degree of consistency between
frequency requests and energy predictions.
(虽然在实践中很难就这一假设的准确性提供硬性保证(例如硬件可能不听指挥)，但与其他 CPUFreq 调频器不同，schedutil 至少会按利用率信号算出的频率去"请求"调频。因此，与 EAS 搭配使用的唯一合理调频器就是 schedutil，因为只有它在频率请求与能耗预测之间提供了一定程度的一致性。)

Using EAS with any other governor than schedutil is not supported.
(不支持将 EAS 与 schedutil 之外的任何调频器搭配使用。)


6.5 Scale-invariant utilization signals(6.5 尺度无关的利用率信号)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In order to make accurate predictions across CPUs and for all performance
states, EAS needs frequency-invariant and CPU-invariant PELT signals. These can
be obtained using the architecture-defined arch_scale{cpu,freq}_capacity()
callbacks.
(为了跨 CPU、跨所有性能状态做出准确预测，EAS 需要频率不变与 CPU 不变的 PELT 信号。这些信号可通过体系结构定义的 arch_scale{cpu,freq}_capacity() 回调获得。)

Using EAS on a platform that doesn't implement these two callbacks is not
supported.
(不支持在未实现这两个回调的平台上使用 EAS。)


6.6 Multithreading (SMT)(6.6 多线程 (SMT))
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

EAS in its current form is SMT unaware and is not able to leverage
multithreaded hardware to save energy. EAS considers threads as independent
CPUs, which can actually be counter-productive for both performance and energy.
(目前的 EAS 不感知 SMT，无法借助多线程硬件来节能。EAS 把线程当作独立的 CPU 对待，这对性能和能耗实际上都可能适得其反。)

EAS on SMT is not supported.
(不支持在 SMT 上使用 EAS。)
