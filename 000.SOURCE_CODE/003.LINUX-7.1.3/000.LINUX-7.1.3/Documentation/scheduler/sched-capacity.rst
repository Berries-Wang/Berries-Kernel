=======================================
Capacity Aware Scheduling(容量感知调度)
=======================================

1. CPU Capacity(1. CPU 容量)
============================

1.1 Introduction(1.1 引言)
--------------------------

Conventional, homogeneous SMP platforms are composed of purely identical
CPUs. Heterogeneous platforms on the other hand are composed of CPUs with
different performance characteristics - on such platforms, not all CPUs can be
considered equal.
(传统的同构 SMP 平台由完全相同的 CPU 组成。异构平台则由具有不同性能特性的 CPU 组成——在这类平台上，并非所有 CPU 都可视为等同。)

CPU capacity is a measure of the performance a CPU can reach, normalized against
the most performant CPU in the system. Heterogeneous systems are also called
asymmetric CPU capacity systems, as they contain CPUs of different capacities.
(CPU 容量是对 CPU 可达到的性能的度量，以系统中性能最强的 CPU 为基准归一化。异构系统也被称为非对称 CPU 容量(asymmetric CPU capacity)系统，因为它们包含容量不同的 CPU。)

Disparity in maximum attainable performance (IOW in maximum CPU capacity) stems
from two factors:
(最大可达性能的差异(即最大 CPU 容量的差异)源于两个因素：)

- not all CPUs may have the same microarchitecture (µarch).
  (并非所有 CPU 都具有相同的微架构(µarch)。)
- with Dynamic Voltage and Frequency Scaling (DVFS), not all CPUs may be
  physically able to attain the higher Operating Performance Points (OPP).
  (在动态电压频率调节(DVFS)下，并非所有 CPU 都能在物理上达到更高的运行性能点(OPP)。)

Arm big.LITTLE systems are an example of both. The big CPUs are more
performance-oriented than the LITTLE ones (more pipeline stages, bigger caches,
smarter predictors, etc), and can usually reach higher OPPs than the LITTLE ones
can.
(Arm big.LITTLE 系统兼有这两种因素。big CPU 比 LITTLE CPU 更面向性能(更多流水线级、更大的缓存、更聪明的预测器等)，而且通常能达到比 LITTLE CPU 更高的 OPP。)

CPU performance is usually expressed in Millions of Instructions Per Second
(MIPS), which can also be expressed as a given amount of instructions attainable
per Hz, leading to::

  capacity(cpu) = work_per_hz(cpu) * max_freq(cpu)

(CPU 性能通常以每秒百万条指令(MIPS)表示，也可以表示为每 Hz 可执行的指令数，由此得到：)

1.2 Scheduler terms(1.2 调度器术语)
-----------------------------------

Two different capacity values are used within the scheduler. A CPU's
``original capacity`` is its maximum attainable capacity, i.e. its maximum
attainable performance level. This original capacity is returned by
the function arch_scale_cpu_capacity(). A CPU's ``capacity`` is its ``original
capacity`` to which some loss of available performance (e.g. time spent
handling IRQs) is subtracted.
(调度器内部使用两个不同的容量值。CPU 的 ``original capacity`` (原始容量)是它可达到的最大容量，即其最大可达性能水平。该原始容量由函数 arch_scale_cpu_capacity() 返回。CPU 的 ``capacity`` (容量)则是其 ``original capacity`` 减去某些可用性能损耗(例如处理 IRQ 所花的时间)之后的值。)

Note that a CPU's ``capacity`` is solely intended to be used by the CFS class,
while ``original capacity`` is class-agnostic. The rest of this document will use
the term ``capacity`` interchangeably with ``original capacity`` for the sake of
brevity.
(注意，CPU 的 ``capacity`` 仅打算供 CFS 调度类使用，而 ``original capacity`` 则与调度类无关。为简洁起见，本文档其余部分将把 ``capacity`` 与 ``original capacity`` 两个术语互换使用。)

1.3 Platform examples(1.3 平台示例)
-----------------------------------

1.3.1 Identical OPPs(1.3.1 相同的 OPP)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Consider an hypothetical dual-core asymmetric CPU capacity system where
(考虑一个假想的非对称 CPU 容量双核系统，其中)

- work_per_hz(CPU0) = W
- work_per_hz(CPU1) = W/2
- all CPUs are running at the same fixed frequency
  (所有 CPU 都运行在相同的固定频率上)

By the above definition of capacity:
(根据上面对容量的定义：)

- capacity(CPU0) = C
- capacity(CPU1) = C/2

To draw the parallel with Arm big.LITTLE, CPU0 would be a big while CPU1 would
be a LITTLE.
(与 Arm big.LITTLE 类比，CPU0 相当于 big，而 CPU1 相当于 LITTLE。)

With a workload that periodically does a fixed amount of work, you will get an
execution trace like so::

 CPU0 work ^
           |     ____                ____                ____
           |    |    |              |    |              |    |
           +----+----+----+----+----+----+----+----+----+----+-> time

 CPU1 work ^
           |     _________           _________           ____
           |    |         |         |         |         |
           +----+----+----+----+----+----+----+----+----+----+-> time

CPU0 has the highest capacity in the system (C), and completes a fixed amount of
work W in T units of time. On the other hand, CPU1 has half the capacity of
CPU0, and thus only completes W/2 in T.
(CPU0 拥有系统中最高的容量(C)，并在 T 个时间单位内完成固定工作量 W。而 CPU1 的容量是 CPU0 的一半，因此在 T 内只完成 W/2。)

1.3.2 Different max OPPs(1.3.2 不同的最大 OPP)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Usually, CPUs of different capacity values also have different maximum
OPPs. Consider the same CPUs as above (i.e. same work_per_hz()) with:
(通常，容量值不同的 CPU 也具有不同的最大 OPP。考虑与上面相同的 CPU(即相同的 work_per_hz())，另有：)

- max_freq(CPU0) = F
- max_freq(CPU1) = 2/3 * F

This yields:
(这得出：)

- capacity(CPU0) = C
- capacity(CPU1) = C/3

Executing the same workload as described in 1.3.1, which each CPU running at its
maximum frequency results in::

 CPU0 work ^
           |     ____                ____                ____
           |    |    |              |    |              |    |
           +----+----+----+----+----+----+----+----+----+----+-> time

                            workload on CPU1
 CPU1 work ^
           |     ______________      ______________      ____
           |    |              |    |              |    |
           +----+----+----+----+----+----+----+----+----+----+-> time

(执行与 1.3.1 中描述相同的负载，且每个 CPU 以其最大频率运行，结果如下：)

1.4 Representation caveat(1.4 表示方法的注意事项)
-------------------------------------------------

It should be noted that having a *single* value to represent differences in CPU
performance is somewhat of a contentious point. The relative performance
difference between two different µarchs could be X% on integer operations, Y% on
floating point operations, Z% on branches, and so on. Still, results using this
simple approach have been satisfactory for now.
(应当指出，用单一数值来表示 CPU 性能差异是一个有争议的做法。两个不同 µarch 之间的相对性能差异在整数运算上可能是 X%，在浮点运算上是 Y%，在分支上是 Z%，等等。尽管如此，迄今为止使用这种简单方法得到的结果还是令人满意的。)

2. Task utilization(2. 任务利用率)
==================================

2.1 Introduction(2.1 引言)
--------------------------

Capacity aware scheduling requires an expression of a task's requirements with
regards to CPU capacity. Each scheduler class can express this differently, and
while task utilization is specific to CFS, it is convenient to describe it here
in order to introduce more generic concepts.
(容量感知调度需要以 CPU 容量的角度来表达任务的需求。每个调度类可以用不同的方式来表达这一点；虽然任务利用率(task utilization)是 CFS 特有的概念，但在这里描述它有助于引入更一般的概念。)

Task utilization is a percentage meant to represent the throughput requirements
of a task. A simple approximation of it is the task's duty cycle, i.e.::

  task_util(p) = duty_cycle(p)

(任务利用率是一个百分比，用来表示任务的吞吐需求。它的一个简单近似就是任务的占空比(duty cycle)，即：)

On an SMP system with fixed frequencies, 100% utilization suggests the task is a
busy loop. Conversely, 10% utilization hints it is a small periodic task that
spends more time sleeping than executing. Variable CPU frequencies and
asymmetric CPU capacities complexify this somewhat; the following sections will
expand on these.
(在频率固定的 SMP 系统上，100% 的利用率表明该任务是一个忙循环(busy loop)；相反，10% 的利用率则暗示它是一个大部分时间在睡眠、小部分时间在执行的小型周期性任务。可变的 CPU 频率与非对称的 CPU 容量让问题变得更复杂一些；以下各节将展开讨论。)

2.2 Frequency invariance(2.2 频率不变性)
----------------------------------------

One issue that needs to be taken into account is that a workload's duty cycle is
directly impacted by the current OPP the CPU is running at. Consider running a
periodic workload at a given frequency F::

  CPU work ^
           |     ____                ____                ____
           |    |    |              |    |              |    |
           +----+----+----+----+----+----+----+----+----+----+-> time

(需要考虑的一个问题是：负载的占空比直接受 CPU 当前运行所处 OPP 的影响。考虑以给定频率 F 运行一个周期性负载：)

This yields duty_cycle(p) == 25%.
(这得出 duty_cycle(p) == 25%。)

Now, consider running the *same* workload at frequency F/2::

  CPU work ^
           |     _________           _________           ____
           |    |         |         |         |         |
           +----+----+----+----+----+----+----+----+----+----+-> time

(现在，考虑以频率 F/2 运行同一个负载：)

This yields duty_cycle(p) == 50%, despite the task having the exact same
behaviour (i.e. executing the same amount of work) in both executions.
(尽管任务在两次执行中的行为完全相同(即执行相同的工作量)，这次却得出 duty_cycle(p) == 50%。)

The task utilization signal can be made frequency invariant using the following
formula::

  task_util_freq_inv(p) = duty_cycle(p) * (curr_frequency(cpu) / max_frequency(cpu))

(任务利用率信号可以用以下公式做到频率不变：)

Applying this formula to the two examples above yields a frequency invariant
task utilization of 25%.
(把该公式应用到上面两个例子，会得到频率不变的任务利用率 25%。)

2.3 CPU invariance(2.3 CPU 不变性)
----------------------------------

CPU capacity has a similar effect on task utilization in that running an
identical workload on CPUs of different capacity values will yield different
duty cycles.
(CPU 容量对任务利用率也有类似的影响：在容量值不同的 CPU 上运行同一负载会得出不同的占空比。)

Consider the system described in 1.3.2., i.e.::

- capacity(CPU0) = C
- capacity(CPU1) = C/3

(考虑 1.3.2 节所描述的系统，即：)

Executing a given periodic workload on each CPU at their maximum frequency would
result in::

 CPU0 work ^
           |     ____                ____                ____
           |    |    |              |    |              |    |
           +----+----+----+----+----+----+----+----+----+----+-> time

 CPU1 work ^
           |     ______________      ______________      ____
           |    |              |    |              |    |
           +----+----+----+----+----+----+----+----+----+----+-> time

(在每个 CPU 上以其最大频率执行给定的周期性负载，结果如下：)

IOW,

- duty_cycle(p) == 25% if p runs on CPU0 at its maximum frequency
  (换言之，若 p 以其最大频率运行在 CPU0 上，则 duty_cycle(p) == 25%)
- duty_cycle(p) == 75% if p runs on CPU1 at its maximum frequency
  (若 p 以其最大频率运行在 CPU1 上，则 duty_cycle(p) == 75%)

The task utilization signal can be made CPU invariant using the following
formula::

  task_util_cpu_inv(p) = duty_cycle(p) * (capacity(cpu) / max_capacity)

(任务利用率信号可以用以下公式做到 CPU 不变：)

with ``max_capacity`` being the highest CPU capacity value in the
system. Applying this formula to the above example above yields a CPU
invariant task utilization of 25%.
(其中 ``max_capacity`` 是系统中最高的 CPU 容量值。把该公式应用到上面的例子，会得到 CPU 不变的任务利用率 25%。)

2.4 Invariant task utilization(2.4 不变的任务利用率)
----------------------------------------------------

Both frequency and CPU invariance need to be applied to task utilization in
order to obtain a truly invariant signal. The pseudo-formula for a task
utilization that is both CPU and frequency invariant is thus, for a given
task p::

                                     curr_frequency(cpu)   capacity(cpu)
  task_util_inv(p) = duty_cycle(p) * ------------------- * -------------
                                     max_frequency(cpu)    max_capacity

(要获得真正不变的信号，频率不变性与 CPU 不变性都需要应用到任务利用率上。因此，对于给定任务 p，同时具有 CPU 与频率不变性的任务利用率的伪公式为：)

In other words, invariant task utilization describes the behaviour of a task as
if it were running on the highest-capacity CPU in the system, running at its
maximum frequency.
(换言之，不变任务利用率描述的是这样一个任务的行为：仿佛它运行在系统中容量最高的 CPU 上，并以其最大频率运行。)

Any mention of task utilization in the following sections will imply its
invariant form.
(下文各节中任何提到任务利用率之处，都指其不变形式。)

2.5 Utilization estimation(2.5 利用率估算)
------------------------------------------

Without a crystal ball, task behaviour (and thus task utilization) cannot
accurately be predicted the moment a task first becomes runnable. The CFS class
maintains a handful of CPU and task signals based on the Per-Entity Load
Tracking (PELT) mechanism, one of those yielding an *average* utilization (as
opposed to instantaneous).
(在没有水晶球的情况下，任务第一次变为可运行的瞬间无法准确预测其行为(因而也无法准确预测其利用率)。CFS 调度类基于每实体负载跟踪(PELT)机制维护着少数几个 CPU 与任务信号，其中一个给出的是 *平均* 利用率(而非瞬时利用率)。)

This means that while the capacity aware scheduling criteria will be written
considering a "true" task utilization (using a crystal ball), the implementation
will only ever be able to use an estimator thereof.
(这意味着，虽然容量感知调度准则在书写时会考虑 "真正的" 任务利用率(用上水晶球)，但实现永远只能使用它的某个估计量。)

3. Capacity aware scheduling requirements(3. 容量感知调度的要求)
================================================================

3.1 CPU capacity(3.1 CPU 容量)
------------------------------

Linux cannot currently figure out CPU capacity on its own, this information thus
needs to be handed to it. Architectures must define arch_scale_cpu_capacity()
for that purpose.
(Linux 目前无法自行推算 CPU 容量，因此这一信息需要交给它。为此，架构必须定义 arch_scale_cpu_capacity()。)

The arm, arm64, and RISC-V architectures directly map this to the arch_topology driver
CPU scaling data, which is derived from the capacity-dmips-mhz CPU binding; see
Documentation/devicetree/bindings/cpu/cpu-capacity.txt.
(arm、arm64 与 RISC-V 架构直接把它映射到 arch_topology 驱动的 CPU 伸缩(scaling)数据，该数据源自 capacity-dmips-mhz CPU 绑定；见 Documentation/devicetree/bindings/cpu/cpu-capacity.txt。)

3.2 Frequency invariance(3.2 频率不变性)
----------------------------------------

As stated in 2.2, capacity-aware scheduling requires a frequency-invariant task
utilization. Architectures must define arch_scale_freq_capacity(cpu) for that
purpose.
(如 2.2 节所述，容量感知调度需要频率不变的任务利用率。为此，架构必须定义 arch_scale_freq_capacity(cpu)。)

Implementing this function requires figuring out at which frequency each CPU
have been running at. One way to implement this is to leverage hardware counters
whose increment rate scale with a CPU's current frequency (APERF/MPERF on x86,
AMU on arm64). Another is to directly hook into cpufreq frequency transitions,
when the kernel is aware of the switched-to frequency (also employed by
arm/arm64).
(实现该函数需要弄清每个 CPU 一直以什么频率运行。一种实现方式是利用其递增速率随 CPU 当前频率伸缩的硬件计数器(x86 上的 APERF/MPERF，arm64 上的 AMU)。另一种方式是直接挂接到 cpufreq 频率切换，此时内核知道切换到的频率(arm/arm64 也采用这种方式)。)

4. Scheduler topology(4. 调度器拓扑)
====================================

During the construction of the sched domains, the scheduler will figure out
whether the system exhibits asymmetric CPU capacities. Should that be the
case:
(在构建调度域(sched domain)的过程中，调度器会判断系统是否表现出非对称 CPU 容量。如果是：)

- The sched_asym_cpucapacity static key will be enabled.
  (sched_asym_cpucapacity 静态键(static key)将被启用。)
- The SD_ASYM_CPUCAPACITY_FULL flag will be set at the lowest sched_domain
  level that spans all unique CPU capacity values.
  (SD_ASYM_CPUCAPACITY_FULL 标志将设置在覆盖所有互不相同的 CPU 容量值的最低 sched_domain 层级上。)
- The SD_ASYM_CPUCAPACITY flag will be set for any sched_domain that spans
  CPUs with any range of asymmetry.
  (SD_ASYM_CPUCAPACITY 标志将设置在任何一个覆盖具有某种不对称程度 CPU 的 sched_domain 上。)

The sched_asym_cpucapacity static key is intended to guard sections of code that
cater to asymmetric CPU capacity systems. Do note however that said key is
*system-wide*. Imagine the following setup using cpusets::

  capacity    C/2          C
            ________    ________
           /        \  /        \
  CPUs     0  1  2  3  4  5  6  7
           \__/  \______________/
  cpusets   cs0         cs1

(sched_asym_cpucapacity 静态键意在守护那些为非对称 CPU 容量系统服务的代码段。但请注意，该键是 *系统范围* 的。想象下面这个使用 cpuset 的配置：)

Which could be created via:
(可以通过以下方式创建：)

.. code-block:: sh

  mkdir /sys/fs/cgroup/cpuset/cs0
  echo 0-1 > /sys/fs/cgroup/cpuset/cs0/cpuset.cpus
  echo 0 > /sys/fs/cgroup/cpuset/cs0/cpuset.mems

  mkdir /sys/fs/cgroup/cpuset/cs1
  echo 2-7 > /sys/fs/cgroup/cpuset/cs1/cpuset.cpus
  echo 0 > /sys/fs/cgroup/cpuset/cs1/cpuset.mems

  echo 0 > /sys/fs/cgroup/cpuset/cpuset.sched_load_balance

Since there *is* CPU capacity asymmetry in the system, the
sched_asym_cpucapacity static key will be enabled. However, the sched_domain
hierarchy of CPUs 0-1 spans a single capacity value: SD_ASYM_CPUCAPACITY isn't
set in that hierarchy, it describes an SMP island and should be treated as such.
(由于系统中确实存在 CPU 容量不对称，sched_asym_cpucapacity 静态键将被启用。然而，CPU 0-1 的 sched_domain 层级只覆盖单一容量值：SD_ASYM_CPUCAPACITY 在该层级中不会被设置——它描述的是一个 SMP 岛，应当如此对待。)

Therefore, the 'canonical' pattern for protecting codepaths that cater to
asymmetric CPU capacities is to:
(因此，保护那些服务于非对称 CPU 容量的代码路径的 "canonical"(标准)模式是：)

- Check the sched_asym_cpucapacity static key
  (检查 sched_asym_cpucapacity 静态键)
- If it is enabled, then also check for the presence of SD_ASYM_CPUCAPACITY in
  the sched_domain hierarchy (if relevant, i.e. the codepath targets a specific
  CPU or group thereof)
  (如果它已启用，那么还要检查 sched_domain 层级中是否存在 SD_ASYM_CPUCAPACITY(若与场景相关，即代码路径针对某个特定 CPU 或其组))

5. Capacity aware scheduling implementation(5. 容量感知调度的实现)
==================================================================

5.1 CFS(5.1 CFS)
----------------

5.1.1 Capacity fitness(5.1.1 容量适配性)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The main capacity scheduling criterion of CFS is::

  task_util(p) < capacity(task_cpu(p))

(CFS 的主要容量调度准则是：)

This is commonly called the capacity fitness criterion, i.e. CFS must ensure a
task "fits" on its CPU. If it is violated, the task will need to achieve more
work than what its CPU can provide: it will be CPU-bound.
(这通常被称为容量适配性(capacity fitness)准则，即 CFS 必须确保任务在其 CPU 上 "放得下"。如果违反该准则，任务需要完成的工作量将超过其 CPU 所能提供的：它会受 CPU 限制(CPU-bound)。)

Furthermore, uclamp lets userspace specify a minimum and a maximum utilization
value for a task, either via sched_setattr() or via the cgroup interface (see
Documentation/admin-guide/cgroup-v2.rst). As its name imply, this can be used to
clamp task_util() in the previous criterion.
(此外，uclamp 允许用户空间通过 sched_setattr() 或 cgroup 接口(见 Documentation/admin-guide/cgroup-v2.rst)为任务指定最小与最大利用率值。顾名思义，它可以用来钳制前一准则中的 task_util()。)

5.1.2 Wakeup CPU selection(5.1.2 唤醒 CPU 选择)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

CFS task wakeup CPU selection follows the capacity fitness criterion described
above. On top of that, uclamp is used to clamp the task utilization values,
which lets userspace have more leverage over the CPU selection of CFS
tasks. IOW, CFS wakeup CPU selection searches for a CPU that satisfies::

  clamp(task_util(p), task_uclamp_min(p), task_uclamp_max(p)) < capacity(cpu)

(CFS 任务唤醒 CPU 选择遵循上文描述的容量适配性准则。在此基础上，uclamp 被用来钳制任务利用率值，这让用户空间对 CFS 任务的 CPU 选择有更多掌控。换言之，CFS 唤醒 CPU 选择寻找满足以下条件的 CPU：)

By using uclamp, userspace can e.g. allow a busy loop (100% utilization) to run
on any CPU by giving it a low uclamp.max value. Conversely, it can force a small
periodic task (e.g. 10% utilization) to run on the highest-performance CPUs by
giving it a high uclamp.min value.
(通过使用 uclamp，用户空间可以让一个忙循环(100% 利用率)运行在任意 CPU 上，只需给它一个较低的 uclamp.max 值；反过来，也可以通过给它较高的 uclamp.min 值，迫使一个小型周期性任务(如 10% 利用率)运行在性能最强的 CPU 上。)

.. note::

  Wakeup CPU selection in CFS can be eclipsed by Energy Aware Scheduling
  (EAS), which is described in Documentation/scheduler/sched-energy.rst.
  (CFS 中的唤醒 CPU 选择可能被能量感知调度(Energy Aware Scheduling，EAS)所取代，EAS 在 Documentation/scheduler/sched-energy.rst 中描述。)

5.1.3 Load balancing(5.1.3 负载均衡)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A pathological case in the wakeup CPU selection occurs when a task rarely
sleeps, if at all - it thus rarely wakes up, if at all. Consider::

  w == wakeup event

  capacity(CPU0) = C
  capacity(CPU1) = C / 3

                           workload on CPU0
  CPU work ^
           |     _________           _________           ____
           |    |         |         |         |         |
           +----+----+----+----+----+----+----+----+----+----+-> time
                w                   w                   w

                           workload on CPU1
  CPU work ^
           |     ____________________________________________
           |    |
           +----+----+----+----+----+----+----+----+----+----+->
                w

(唤醒 CPU 选择中的一种病态情形出现在任务几乎不睡眠时——因而它也几乎不唤醒。考虑：)

This workload should run on CPU0, but if the task either:
(这种负载应当运行在 CPU0 上，但如果该任务：)

- was improperly scheduled from the start (inaccurate initial
  utilization estimation)
  (从一开始就被不当调度(初始利用率估算不准确))
- was properly scheduled from the start, but suddenly needs more
  processing power
  (一开始被正确调度，但突然需要更多算力)

then it might become CPU-bound, IOW ``task_util(p) > capacity(task_cpu(p))``;
the CPU capacity scheduling criterion is violated, and there may not be any more
wakeup event to fix this up via wakeup CPU selection.
(那么它可能变成受 CPU 限制，即 ``task_util(p) > capacity(task_cpu(p))``；CPU 容量调度准则被违反，而且可能不再有任何唤醒事件可以通过唤醒 CPU 选择来纠正这一状况。)

Tasks that are in this situation are dubbed "misfit" tasks, and the mechanism
put in place to handle this shares the same name. Misfit task migration
leverages the CFS load balancer, more specifically the active load balance part
(which caters to migrating currently running tasks). When load balance happens,
a misfit active load balance will be triggered if a misfit task can be migrated
to a CPU with more capacity than its current one.
(处于这种境况的任务被称为 "misfit"(不匹配)任务，为处理这一情况而设的机制也使用同样的名字。misfit 任务迁移借助 CFS 负载均衡器，更具体地说是活动负载均衡(active load balance)部分(它负责迁移当前正在运行的任务)。当负载均衡发生时，如果可以把 misfit 任务迁移到容量比其当前 CPU 更大的 CPU 上，就会触发一次 misfit 活动负载均衡。)

5.2 RT(5.2 RT)
--------------

5.2.1 Wakeup CPU selection(5.2.1 唤醒 CPU 选择)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

RT task wakeup CPU selection searches for a CPU that satisfies::

  task_uclamp_min(p) <= capacity(task_cpu(cpu))

(RT 任务唤醒 CPU 选择寻找满足以下条件的 CPU：)

while still following the usual priority constraints. If none of the candidate
CPUs can satisfy this capacity criterion, then strict priority based scheduling
is followed and CPU capacities are ignored.
(同时仍遵循通常的优先级约束。如果没有候选 CPU 能满足这一容量准则，则按严格的基于优先级的调度进行，并忽略 CPU 容量。)

5.3 DL(5.3 DL)
--------------

5.3.1 Wakeup CPU selection(5.3.1 唤醒 CPU 选择)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DL task wakeup CPU selection searches for a CPU that satisfies::

  task_bandwidth(p) < capacity(task_cpu(p))

(DL 任务唤醒 CPU 选择寻找满足以下条件的 CPU：)

while still respecting the usual bandwidth and deadline constraints. If
none of the candidate CPUs can satisfy this capacity criterion, then the
task will remain on its current CPU.
(同时仍遵守通常的带宽与截止期(deadline)约束。如果没有候选 CPU 能满足这一容量准则，则任务将留在其当前 CPU 上。)
