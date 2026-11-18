.. SPDX-License-Identifier: GPL-2.0

============
Design(设计)
============


.. _damon_design_execution_model_and_data_structures:

Execution Model and Data Structures(执行模型与数据结构)
=======================================================

The monitoring-related information including the monitoring request
specification and DAMON-based operation schemes are stored in a data structure
called DAMON ``context``.  DAMON executes each context with a kernel thread
called ``kdamond``.  Multiple kdamonds could run in parallel, for different
types of monitoring.
(包括监控请求规格和基于 DAMON 的操作方案（operation scheme）在内的监控相关信息，存储在一个名为 DAMON ``context`` 的数据结构中。DAMON 用一个名为 ``kdamond`` 的内核线程执行每个 context。多个 kdamond 可以并行运行，用于不同类型的监控。)

To know how user-space can do the configurations and start/stop DAMON, refer to
:ref:`DAMON sysfs interface <sysfs_interface>` documentation.
(要了解用户空间如何进行配置以及启动/停止 DAMON，请参阅 :ref:`DAMON sysfs interface <sysfs_interface>` 文档。)


Overall Architecture(整体架构)
==============================

DAMON subsystem is configured with three layers including
(DAMON 子系统由以下三个层级构成：)

- :ref:`Operations Set <damon_operations_set>`: Implements fundamental
  operations for DAMON that depends on the given monitoring target
  address-space and available set of software/hardware primitives,
  (:ref:`Operations Set <damon_operations_set>`（操作集合层）：实现依赖于给定监控目标地址空间以及可用软硬件原语集合的 DAMON 基础操作；)
- :ref:`Core <damon_core_logic>`: Implements core logics including monitoring
  overhead/accuracy control and access-aware system operations on top of the
  operations set layer, and
  (:ref:`Core <damon_core_logic>`（核心逻辑层）：在操作集合层之上实现核心逻辑，包括监控开销/精度控制以及访问感知的系统操作；以及)
- :ref:`Modules <damon_modules>`: Implements kernel modules for various
  purposes that provides interfaces for the user space, on top of the core
  layer.
  (:ref:`Modules <damon_modules>`（模块层）：在核心层之上为各种目的实现内核模块，为用户空间提供接口。)


.. _damon_operations_set:

Operations Set Layer(操作集合层)
================================

.. _damon_design_configurable_operations_set:

For data access monitoring and additional low level work, DAMON needs a set of
implementations for specific operations that are dependent on and optimized for
the given target address space.  For example, below two operations for access
monitoring are address-space dependent.
(对于数据访问监控和其他额外的底层工作，DAMON 需要一组针对特定操作的实现，这些操作依赖于给定的目标地址空间并针对其做了优化。例如，下面两个用于访问监控的操作都与地址空间相关。)

1. Identification of the monitoring target address range for the address space.
   (为该地址空间确定监控目标地址范围。)
2. Access check of specific address range in the target space.
   (检查目标空间中特定地址范围的访问情况。)

DAMON consolidates these implementations in a layer called DAMON Operations
Set, and defines the interface between it and the upper layer.  The upper layer
is dedicated for DAMON's core logics including the mechanism for control of the
monitoring accuracy and the overhead.
(DAMON 把这些实现在一个名为 DAMON Operations Set（操作集合）的层中统一起来，并定义它与上层之间的接口。上层专门负责 DAMON 的核心逻辑，包括控制监控精度和开销的机制。)

Hence, DAMON can easily be extended for any address space and/or available
hardware features by configuring the core logic to use the appropriate
operations set.  If there is no available operations set for a given purpose, a
new operations set can be implemented following the interface between the
layers.
(因此，通过配置核心逻辑使用合适的操作集合，DAMON 可以很容易地扩展到任何地址空间和/或可用的硬件特性。如果针对某个给定目的没有可用的操作集合，可以按照层间接口实现新的操作集合。)

For example, physical memory, virtual memory, swap space, those for specific
processes, NUMA nodes, files, and backing memory devices would be supportable.
Also, if some architectures or devices support special optimized access check
features, those will be easily configurable.
(例如，物理内存、虚拟内存、交换空间、特定进程的地址空间、NUMA 节点、文件以及后备内存设备等都可以得到支持。此外，如果某些体系结构或设备支持特殊的优化访问检查特性，也可以方便地进行配置。)

DAMON currently provides below three operation sets.  Below three subsections
describe how those work.
(DAMON 目前提供以下三种操作集合。下面三个小节描述它们的工作方式。)

 - vaddr: Monitor virtual address spaces of specific processes
   (vaddr：监控特定进程的虚拟地址空间)
 - fvaddr: Monitor fixed virtual address ranges
   (fvaddr：监控固定的虚拟地址范围)
 - paddr: Monitor the physical address space of the system
   (paddr：监控系统的物理地址空间)

To know how user-space can do the configuration via :ref:`DAMON sysfs interface
<sysfs_interface>`, refer to :ref:`operations <sysfs_context>` file part of the
documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface
<sysfs_interface>` 进行配置，请参阅文档中 :ref:`operations <sysfs_context>` 文件部分。)


 .. _damon_design_vaddr_target_regions_construction:

VMA-based Target Address Range Construction(基于 VMA 的目标地址范围构建)
------------------------------------------------------------------------

A mechanism of ``vaddr`` DAMON operations set that automatically initializes
and updates the monitoring target address regions so that entire memory
mappings of the target processes can be covered.
(``vaddr`` DAMON 操作集合中的一种机制，它自动初始化并更新监控目标地址区域，从而覆盖目标进程的全部内存映射。)

This mechanism is only for the ``vaddr`` operations set.  In cases of
``fvaddr`` and ``paddr`` operation sets, users are asked to manually set the
monitoring target address ranges.
(该机制仅适用于 ``vaddr`` 操作集合。对于 ``fvaddr`` 和 ``paddr`` 操作集合，需要用户手动设置监控目标地址范围。)

Only small parts in the super-huge virtual address space of the processes are
mapped to the physical memory and accessed.  Thus, tracking the unmapped
address regions is just wasteful.  However, because DAMON can deal with some
level of noise using the adaptive regions adjustment mechanism, tracking every
mapping is not strictly required but could even incur a high overhead in some
cases.  That said, too huge unmapped areas inside the monitoring target should
be removed to not take the time for the adaptive mechanism.
(进程超大虚拟地址空间中只有一小部分被映射到物理内存并被访问。因此，跟踪未映射的地址区域纯属浪费。不过，由于 DAMON 可以借助自适应区域调整机制应对一定程度的噪声，跟踪每一个映射并非严格必要，在某些情况下甚至可能带来很高的开销。话虽如此，监控目标内部过大的未映射区域仍应被去除，以免占用自适应机制的处理时间。)

For the reason, this implementation converts the complex mappings to three
distinct regions that cover every mapped area of the address space.  The two
gaps between the three regions are the two biggest unmapped areas in the given
address space.  The two biggest unmapped areas would be the gap between the
heap and the uppermost mmap()-ed region, and the gap between the lowermost
mmap()-ed region and the stack in most of the cases.  Because these gaps are
exceptionally huge in usual address spaces, excluding these will be sufficient
to make a reasonable trade-off.  Below shows this in detail::

    <heap>
    <BIG UNMAPPED REGION 1>
    <uppermost mmap()-ed region>
    (small mmap()-ed regions and munmap()-ed regions)
    <lowermost mmap()-ed region>
    <BIG UNMAPPED REGION 2>
    <stack>

(出于这个原因，该实现把复杂的映射转换为覆盖该地址空间中所有已映射区域的三个不同区域。这三个区域之间的两个间隙，就是给定地址空间中两个最大的未映射区域。在大多数情况下，这两个最大的未映射区域是堆与最顶部的 mmap() 映射区域之间的间隙，以及最底部的 mmap() 映射区域与栈之间的间隙。由于这些间隙在通常的地址空间中异常巨大，排除它们就足以做出合理的权衡。下面详细展示了这一点：)


PTE Accessed-bit Based Access Check(基于 PTE Accessed 位的访问检查)
-------------------------------------------------------------------

Both of the implementations for physical and virtual address spaces use PTE
Accessed-bit for basic access checks.  Only one difference is the way of
finding the relevant PTE Accessed bit(s) from the address.  While the
implementation for the virtual address walks the page table for the target task
of the address, the implementation for the physical address walks every page
table having a mapping to the address.  In this way, the implementations find
and clear the bit(s) for next sampling target address and checks whether the
bit(s) set again after one sampling period.  This could disturb other kernel
subsystems using the Accessed bits, namely Idle page tracking and the reclaim
logic.  DAMON does nothing to avoid disturbing Idle page tracking, so handling
the interference is the responsibility of sysadmins.  However, it solves the
conflict with the reclaim logic using ``PG_idle`` and ``PG_young`` page flags,
as Idle page tracking does.
(物理地址空间和虚拟地址空间两种实现都使用 PTE Accessed 位做基本的访问检查。唯一的差别在于从地址找到相关 PTE Accessed 位的方式：虚拟地址的实现遍历该地址所属目标任务的页表，而物理地址的实现遍历每一个映射到该地址的页表。通过这种方式，这些实现为下一个采样目标地址找到并清除相应的位，然后检查该位是否在一个采样周期之后再次被置位。这可能会干扰使用 Accessed 位的其他内核子系统，即 Idle page tracking（空闲页跟踪）和回收（reclaim）逻辑。DAMON 没有做任何避免干扰 Idle page tracking 的事情，因此处理这种干扰是系统管理员的责任。不过，它像 Idle page tracking 一样，使用 ``PG_idle`` 和 ``PG_young`` 页标志来解决与回收逻辑的冲突。)

.. _damon_design_addr_unit:

Address Unit(地址单位)
----------------------

DAMON core layer uses ``unsinged long`` type for monitoring target address
ranges.  In some cases, the address space for a given operations set could be
too large to be handled with the type.  ARM (32-bit) with large physical
address extension is an example.  For such cases, a per-operations set
parameter called ``address unit`` is provided.  It represents the scale factor
that need to be multiplied to the core layer's address for calculating real
address on the given address space.  Support of ``address unit`` parameter is
up to each operations set implementation.  ``paddr`` is the only operations set
implementation that supports the parameter.
(DAMON 核心层使用 ``unsinged long`` 类型表示监控目标地址范围。在某些情况下，给定操作集合的地址空间可能大到无法用该类型处理。启用大物理地址扩展（LPAE）的 ARM（32 位）就是一个例子。针对这类情况，DAMON 提供了一个按操作集合生效的参数 ``address unit``。它表示需要与核心层地址相乘的缩放因子，用于计算给定地址空间上的真实地址。是否支持 ``address unit`` 参数取决于各操作集合的实现。``paddr`` 是唯一支持该参数的操作集合实现。)

If the value is smaller than ``PAGE_SIZE``, only a power of two should be used.
(如果该值小于 ``PAGE_SIZE``，则只能使用 2 的幂。)

.. _damon_core_logic:

Core Logics(核心逻辑)
=====================

.. _damon_design_monitoring:

Monitoring(监控)
----------------

Below four sections describe each of the DAMON core mechanisms and the five
monitoring attributes, ``sampling interval``, ``aggregation interval``,
``update interval``, ``minimum number of regions``, and ``maximum number of
regions``.
(下面四节描述 DAMON 的各个核心机制以及五个监控属性：``sampling interval`` (采样间隔)、``aggregation interval`` (聚合间隔)、``update interval`` (更新间隔)、``minimum number of regions`` (最小区域数) 和 ``maximum number of regions`` (最大区域数)。)

Note that ``minimum number of regions`` must be 3 or higher. This is because the
virtual address space monitoring is designed to handle at least three regions to
accommodate two large unmapped areas commonly found in normal virtual address
spaces. While this restriction might not be strictly necessary for other
operation sets like ``paddr``, it is currently enforced across all DAMON
operations for consistency.
(注意，``minimum number of regions`` 必须为 3 或更大。这是因为虚拟地址空间监控在设计上至少要处理三个区域，以容纳普通虚拟地址空间中常见的两个大的未映射区域。虽然对于 ``paddr`` 等其他操作集合来说这条限制可能并非严格必要，但为了保持一致性，目前它在所有 DAMON 操作中都被强制执行。)

To know how user-space can set the attributes via :ref:`DAMON sysfs interface
<sysfs_interface>`, refer to :ref:`monitoring_attrs <sysfs_monitoring_attrs>`
part of the documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface
<sysfs_interface>` 设置这些属性，请参阅文档中 :ref:`monitoring_attrs <sysfs_monitoring_attrs>`
部分。)


Access Frequency Monitoring(访问频率监控)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The output of DAMON says what pages are how frequently accessed for a given
duration.  The resolution of the access frequency is controlled by setting
``sampling interval`` and ``aggregation interval``.  In detail, DAMON checks
access to each page per ``sampling interval`` and aggregates the results.  In
other words, counts the number of the accesses to each page.  After each
``aggregation interval`` passes, DAMON calls callback functions that previously
registered by users so that users can read the aggregated results and then
clears the results.  This can be described in below simple pseudo-code::

    while monitoring_on:
        for page in monitoring_target:
            if accessed(page):
                nr_accesses[page] += 1
        if time() % aggregation_interval == 0:
            for callback in user_registered_callbacks:
                callback(monitoring_target, nr_accesses)
            for page in monitoring_target:
                nr_accesses[page] = 0
        sleep(sampling interval)

(DAMON 的输出说明在给定时长内哪些页面被访问得有多频繁。访问频率的分辨率通过设置 ``sampling interval`` 和 ``aggregation interval`` 控制。具体来说，DAMON 每个 ``sampling interval`` 检查一次对各页面的访问并聚合结果，换句话说，就是统计对每个页面的访问次数。每过一个 ``aggregation interval``，DAMON 会调用用户之前注册的回调函数，让用户可以读取聚合结果，然后清除这些结果。这可以用下面的简单伪代码来描述：)

The monitoring overhead of this mechanism will arbitrarily increase as the
size of the target workload grows.
(随着目标工作负载规模的增大，这种机制的监控开销会无限制地增加。)


.. _damon_design_region_based_sampling:

Region Based Sampling(基于区域的采样)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To avoid the unbounded increase of the overhead, DAMON groups adjacent pages
that assumed to have the same access frequencies into a region.  As long as the
assumption (pages in a region have the same access frequencies) is kept, only
one page in the region is required to be checked.  Thus, for each ``sampling
interval``, DAMON randomly picks one page in each region, waits for one
``sampling interval``, checks whether the page is accessed meanwhile, and
increases the access frequency counter of the region if so.  The counter is
called ``nr_accesses`` of the region.  Therefore, the monitoring overhead is
controllable by setting the number of regions.  DAMON allows users to set the
minimum and the maximum number of regions for the trade-off.
(为了避免开销无限制地增长，DAMON 把假定具有相同访问频率的相邻页面分组为一个区域。只要这一假设（同一区域内的页面具有相同的访问频率）成立，就只需检查区域中的一个页面。因此，对于每个 ``sampling
interval``，DAMON 在每个区域中随机挑选一个页面，等待一个 ``sampling interval``，检查该页面在此期间是否被访问过，如果被访问过就增加该区域的访问频率计数器。该计数器称为区域的 ``nr_accesses``。因此，监控开销可以通过设置区域数量来控制。DAMON 允许用户设置区域数量的最小值和最大值来进行权衡。)

This scheme, however, cannot preserve the quality of the output if the
assumption is not guaranteed.
(然而，如果这一假设得不到保证，该方案就无法保证输出质量。)


.. _damon_design_adaptive_regions_adjustment:

Adaptive Regions Adjustment(自适应区域调整)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Even somehow the initial monitoring target regions are well constructed to
fulfill the assumption (pages in same region have similar access frequencies),
the data access pattern can be dynamically changed.  This will result in low
monitoring quality.  To keep the assumption as much as possible, DAMON
adaptively merges and splits each region based on their access frequency.
(即使初始监控目标区域构建得足够好、能够满足假设（同一区域内的页面具有相似的访问频率），数据访问模式也可能动态变化。这会导致监控质量下降。为了尽可能保持这一假设，DAMON 根据各区域的访问频率自适应地合并和拆分区域。)

For each ``aggregation interval``, it compares the access frequencies
(``nr_accesses``) of adjacent regions.  If the difference is small, and if the
sum of the two regions' sizes is smaller than the size of total regions divided
by the ``minimum number of regions``, DAMON merges the two regions.  If the
resulting number of total regions is still higher than ``maximum number of
regions``, it repeats the merging with increasing access frequenceis difference
threshold until the upper-limit of the number of regions is met, or the
threshold becomes higher than possible maximum value (``aggregation interval``
divided by ``sampling interval``).   Then, after it reports and clears the
aggregated access frequency of each region, it splits each region into two or
three regions if the total number of regions will not exceed the user-specified
maximum number of regions after the split.
(对于每个 ``aggregation interval``，DAMON 会比较相邻区域的访问频率 (``nr_accesses``)。如果差异较小，并且两个区域的大小之和小于区域总大小除以 ``minimum number of regions`` 的结果，DAMON 就合并这两个区域。如果合并后的区域总数仍然高于 ``maximum number of regions``，它会不断提高访问频率差异阈值并重复合并，直到区域数量达到上限，或者阈值超过可能的最大值 (``aggregation interval`` 除以 ``sampling interval``)。随后，在报告并清除每个区域的聚合访问频率之后，如果拆分后的区域总数不会超过用户指定的最大区域数，它就把每个区域拆分为两个或三个区域。)

In this way, DAMON provides its best-effort quality and minimal overhead while
keeping the bounds users set for their trade-off.
(通过这种方式，DAMON 在保持用户为权衡而设定的边界的同时，提供尽力而为的监控质量和最小的开销。)


.. _damon_design_age_tracking:

Age Tracking(年龄跟踪)
~~~~~~~~~~~~~~~~~~~~~~

By analyzing the monitoring results, users can also find how long the current
access pattern of a region has maintained.  That could be used for good
understanding of the access pattern.  For example, page placement algorithm
utilizing both the frequency and the recency could be implemented using that.
To make such access pattern maintained period analysis easier, DAMON maintains
yet another counter called ``age`` in each region.  For each ``aggregation
interval``, DAMON checks if the region's size and access frequency
(``nr_accesses``) has significantly changed.  If so, the counter is reset to
zero.  Otherwise, the counter is increased.
(通过分析监控结果，用户还可以了解某个区域当前的访问模式已经维持了多长时间。这有助于更好地理解访问模式。例如，可以利用它实现同时考虑频率和新近度（recency）的页面放置算法。为了让这类"访问模式维持时长"的分析更容易，DAMON 在每个区域中还维护另一个计数器 ``age``。对于每个 ``aggregation
interval``，DAMON 检查区域的大小和访问频率 (``nr_accesses``) 是否发生了显著变化。如果是，该计数器重置为零；否则计数器递增。)


Dynamic Target Space Updates Handling(动态目标空间更新处理)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The monitoring target address range could dynamically changed.  For example,
virtual memory could be dynamically mapped and unmapped.  Physical memory could
be hot-plugged.
(监控目标地址范围可能动态变化。例如，虚拟内存可能被动态映射和取消映射，物理内存可能被热插拔。)

As the changes could be quite frequent in some cases, DAMON allows the
monitoring operations to check dynamic changes including memory mapping changes
and applies it to monitoring operations-related data structures such as the
abstracted monitoring target memory area only for each of a user-specified time
interval (``update interval``).
(由于这些变化在某些情况下可能相当频繁，DAMON 允许监控操作检查包括内存映射变化在内的动态变化，并且只在每个用户指定的时间间隔 (``update interval``) 才将其应用到监控操作相关的数据结构（例如抽象出的监控目标内存区域）上。)

User-space can get the monitoring results via DAMON sysfs interface and/or
tracepoints.  For more details, please refer to the documentations for
:ref:`DAMOS tried regions <sysfs_schemes_tried_regions>` and :ref:`tracepoint`,
respectively.
(用户空间可以通过 DAMON sysfs 接口和/或跟踪点获取监控结果。更多细节请分别参阅 :ref:`DAMOS tried regions <sysfs_schemes_tried_regions>` 和 :ref:`tracepoint` 的文档。)


.. _damon_design_monitoring_params_tuning_guide:

Monitoring Parameters Tuning Guide(监控参数调优指南)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In short, set ``aggregation interval`` to capture meaningful amount of accesses
for the purpose.  The amount of accesses can be measured using ``nr_accesses``
and ``age`` of regions in the aggregated monitoring results snapshot.  The
default value of the interval, ``100ms``, turns out to be too short in many
cases.  Set ``sampling interval`` proportional to ``aggregation interval``.  By
default, ``1/20`` is recommended as the ratio.
(简而言之，把 ``aggregation interval`` 设置为能够为你的目的捕获有意义访问量的时长。访问量可以用聚合监控结果快照中区域的 ``nr_accesses`` 和 ``age`` 来衡量。该间隔的默认值 ``100ms`` 在很多情况下被证明太短。``sampling interval`` 应与 ``aggregation interval`` 成比例设置。默认推荐的比例为 ``1/20``。)

``Aggregation interval`` should be set as the time interval that the workload
can make an amount of accesses for the monitoring purpose, within the interval.
If the interval is too short, only small number of accesses are captured.  As a
result, the monitoring results look everything is samely accessed only rarely.
For many purposes, that would be useless.  If it is too long, however, the time
to converge regions with the :ref:`regions adjustment mechanism
<damon_design_adaptive_regions_adjustment>` can be too long, depending on the
time scale of the given purpose.  This could happen if the workload is actually
making only rare accesses but the user thinks the amount of accesses for the
monitoring purpose too high.  For such cases, the target amount of access to
capture per ``aggregation interval`` should carefully reconsidered.  Also, note
that the captured amount of accesses is represented with not only
``nr_accesses``, but also ``age``.  For example, even if every region on the
monitoring results show zero ``nr_accesses``, regions could still be
distinguished using ``age`` values as the recency information.
(``Aggregation interval`` 应设置为工作负载能够在该间隔内为监控目的产生一定访问量的时长。如果间隔太短，只能捕获到很少的访问。结果是监控结果看起来就是所有内容都同样很少被访问。对很多目的来说这是无用的。但如果间隔太长，区域通过 :ref:`regions adjustment mechanism
<damon_design_adaptive_regions_adjustment>` 收敛所需的时间可能会太长，具体取决于给定目的的时间尺度。如果工作负载实际上只进行少量访问，而用户却认为用于监控目的的访问量过高，就可能发生这种情况。对于这类情况，应当仔细重新考虑每个 ``aggregation interval`` 要捕获的目标访问量。另外请注意，捕获的访问量不仅用 ``nr_accesses`` 表示，还用 ``age`` 表示。例如，即使监控结果中每个区域的 ``nr_accesses`` 都为零，仍然可以利用 ``age`` 值作为新近度信息来区分各个区域。)

Hence the optimum value of ``aggregation interval`` depends on the access
intensiveness of the workload.  The user should tune the interval based on the
amount of access that captured on each aggregated snapshot of the monitoring
results.
(因此，``aggregation interval`` 的最优值取决于工作负载的访问密集程度。用户应当根据监控结果每个聚合快照中捕获到的访问量来调优该间隔。)

Note that the default value of the interval is 100 milliseconds, which is too
short in many cases, especially on large systems.
(注意，该间隔的默认值为 100 毫秒，这在很多情况下都太短，尤其是在大型系统上。)

``Sampling interval`` defines the resolution of each aggregation.  If it is set
too large, monitoring results will look like every region was samely rarely
accessed, or samely frequently accessed.  That is, regions become
undistinguishable based on access pattern, and therefore the results will be
useless in many use cases.  If ``sampling interval`` is too small, it will not
degrade the resolution, but will increase the monitoring overhead.  If it is
appropriate enough to provide a resolution of the monitoring results that
sufficient for the given purpose, it shouldn't be unnecessarily further
lowered.  It is recommended to be set proportional to ``aggregation interval``.
By default, the ratio is set as ``1/20``, and it is still recommended.
(``Sampling interval`` 定义每次聚合的分辨率。如果设置得太大，监控结果会看起来所有区域都同样很少被访问，或同样频繁地被访问。也就是说，区域之间无法根据访问模式加以区分，因此结果在许多使用场景中都没有用处。如果 ``sampling interval`` 太小，虽然不会降低分辨率，但会增加监控开销。如果它已经足以提供满足给定目的的监控结果分辨率，就不应该再无必要地进一步调低。建议将其设置为与 ``aggregation interval`` 成比例。默认比例设为 ``1/20``，这一取值仍然值得推荐。)

Based on the manual tuning guide, DAMON provides more intuitive knob-based
intervals auto tuning mechanism.  Please refer to :ref:`the design document of
the feature <damon_design_monitoring_intervals_autotuning>` for detail.
(在手动调优指南的基础上，DAMON 提供了更直观的基于旋钮的间隔自动调优机制。详情请参阅 :ref:`the design document of
the feature <damon_design_monitoring_intervals_autotuning>`。)

Refer to below documents for an example tuning based on the above guide.
(基于上述指南的调优示例请参阅以下文档。)

.. toctree::
   :maxdepth: 1

   monitoring_intervals_tuning_example


.. _damon_design_monitoring_intervals_autotuning:

Monitoring Intervals Auto-tuning(监控间隔自动调优)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DAMON provides automatic tuning of the ``sampling interval`` and ``aggregation
interval`` based on the :ref:`the tuning guide idea
<damon_design_monitoring_params_tuning_guide>`.  The tuning mechanism allows
users to set the aimed amount of access events to observe via DAMON within
given time interval.  The target can be specified by the user as a ratio of
DAMON-observed access events to the theoretical maximum amount of the events
(``access_bp``) that measured within a given number of aggregations
(``aggrs``).
(DAMON 基于 :ref:`the tuning guide idea
<damon_design_monitoring_params_tuning_guide>` 提供对 ``sampling interval`` 和 ``aggregation
interval`` 的自动调优。该调优机制允许用户设定在给定时间间隔内希望通过 DAMON 观察到的目标访问事件数量。目标可以由用户指定为 DAMON 观察到的访问事件数与理论最大事件数之比 (``access_bp``)，并在给定的聚合次数 (``aggrs``) 内进行测量。)

The DAMON-observed access events are calculated in byte granularity based on
DAMON :ref:`region assumption <damon_design_region_based_sampling>`.  For
example, if a region of size ``X`` bytes of ``Y`` ``nr_accesses`` is found, it
means ``X * Y`` access events are observed by DAMON.  Theoretical maximum
access events for the region is calculated in same way, but replacing ``Y``
with theoretical maximum ``nr_accesses``, which can be calculated as
``aggregation interval / sampling interval``.
(DAMON 观察到的访问事件按字节粒度计算，依据是 DAMON 的 :ref:`region assumption <damon_design_region_based_sampling>`。例如，如果发现一个大小为 ``X`` 字节、``nr_accesses`` 为 ``Y`` 的区域，就意味着 DAMON 观察到了 ``X * Y`` 次访问事件。该区域的理论最大访问事件数用同样的方式计算，只是把 ``Y`` 替换为理论最大 ``nr_accesses``，后者可以按 ``aggregation interval / sampling interval`` 计算。)

The mechanism calculates the ratio of access events for ``aggrs`` aggregations,
and increases or decrease the ``sampleing interval`` and ``aggregation
interval`` in same ratio, if the observed access ratio is lower or higher than
the target, respectively.  The ratio of the intervals change is decided in
proportion to the distance between current samples ratio and the target ratio.
(该机制计算 ``aggrs`` 次聚合的访问事件比例：如果观察到的访问比例低于目标，就按相同比例增大 ``sampleing interval`` 和 ``aggregation
interval``；如果高于目标，则按相同比例减小。间隔变化的比例按当前采样比例与目标比例之间的距离成比例决定。)

The user can further set the minimum and maximum ``sampling interval`` that can
be set by the tuning mechanism using two parameters (``min_sample_us`` and
``max_sample_us``).  Because the tuning mechanism changes ``sampling interval``
and ``aggregation interval`` in same ratio always, the minimum and maximum
``aggregation interval`` after each of the tuning changes can automatically set
together.
(用户还可以通过两个参数 (``min_sample_us`` 和 ``max_sample_us``) 进一步设置调优机制可以设置的 ``sampling interval`` 最小值和最大值。由于调优机制始终以相同比例同时更改 ``sampling interval`` 和 ``aggregation interval``，每次调优更改后的 ``aggregation interval`` 最小值和最大值也会自动一并设置。)

The tuning is turned off by default, and need to be set explicitly by the user.
As a rule of thumbs and the Parreto principle, 4% access samples ratio target
is recommended.  Note that Parreto principle (80/20 rule) has applied twice.
That is, assumes 4% (20% of 20%) DAMON-observed access events ratio (source)
to capture 64% (80% multipled by 80%) real access events (outcomes).
(该调优功能默认关闭，需要用户显式设置。根据经验法则和帕累托原则（Parreto principle），推荐使用 4% 的访问采样比例目标。注意，帕累托原则（80/20 法则）在这里应用了两次：即假设 4%（20% 的 20%）的 DAMON 观察到的访问事件比例（投入）能够捕获 64%（80% 乘以 80%）的真实访问事件（产出）。)

To know how user-space can use this feature via :ref:`DAMON sysfs interface
<sysfs_interface>`, refer to :ref:`intervals_goal
<damon_usage_sysfs_monitoring_intervals_goal>` part of the documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface
<sysfs_interface>` 使用该特性，请参阅文档中 :ref:`intervals_goal
<damon_usage_sysfs_monitoring_intervals_goal>` 部分。)


.. _damon_design_damos:

Operation Schemes(操作方案)
---------------------------

One common purpose of data access monitoring is access-aware system efficiency
optimizations.  For example,

    paging out memory regions that are not accessed for more than two minutes
    (换出超过两分钟未被访问的内存区域)

or
(或者)

    using THP for memory regions that are larger than 2 MiB and showing a high
    access frequency for more than one minute.
    (对大于 2 MiB 且持续一分钟以上表现出高访问频率的内存区域使用 THP)

One straightforward approach for such schemes would be profile-guided
optimizations.  That is, getting data access monitoring results of the
workloads or the system using DAMON, finding memory regions of special
characteristics by profiling the monitoring results, and making system
operation changes for the regions.  The changes could be made by modifying or
providing advice to the software (the application and/or the kernel), or
reconfiguring the hardware.  Both offline and online approaches could be
available.
(实现这类方案的一个直接方法是剖析（profile）引导的优化。也就是说，使用 DAMON 获取工作负载或系统的数据访问监控结果，通过剖析监控结果找出具有特殊特征的内存区域，然后针对这些区域做出系统操作更改。更改可以通过修改软件（应用程序和/或内核）或向其提供建议来实现，也可以通过重新配置硬件来实现。离线和在线两种方式都可以采用。)

Among those, providing advice to the kernel at runtime would be flexible and
effective, and therefore widely be used.   However, implementing such schemes
could impose unnecessary redundancy and inefficiency.  The profiling could be
redundant if the type of interest is common.  Exchanging the information
including monitoring results and operation advice between kernel and user
spaces could be inefficient.
(在上述方法中，在运行时向内核提供建议既灵活又有效，因此会被广泛使用。然而，实现这类方案可能带来不必要的冗余和低效。如果关注的类型很常见，剖析就可能是多余的。在内核空间和用户空间之间交换包括监控结果和操作建议在内的信息也可能低效。)

To allow users to reduce such redundancy and inefficiencies by offloading the
works, DAMON provides a feature called Data Access Monitoring-based Operation
Schemes (DAMOS).  It lets users specify their desired schemes at a high
level.  For such specifications, DAMON starts monitoring, finds regions having
the access pattern of interest, and applies the user-desired operation actions
to the regions, for every user-specified time interval called
``apply_interval``.
(为了让用户能够通过卸载这些工作来减少此类冗余和低效，DAMON 提供了一个名为 Data Access Monitoring-based Operation Schemes（基于数据访问监控的操作方案，DAMOS）的特性。它让用户可以在高层次上描述自己想要的方案。对于这样的描述，DAMON 会开始监控，找出具有所关注访问模式的区域，并按每个用户指定的时间间隔 (``apply_interval``) 对这些区域应用用户期望的操作动作。)

To know how user-space can set ``apply_interval`` via :ref:`DAMON sysfs
interface <sysfs_interface>`, refer to :ref:`apply_interval_us <sysfs_scheme>`
part of the documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs
interface <sysfs_interface>` 设置 ``apply_interval``，请参阅文档中 :ref:`apply_interval_us <sysfs_scheme>`
部分。)


.. _damon_design_damos_action:

Operation Action(操作动作)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The management action that the users desire to apply to the regions of their
interest.  For example, paging out, prioritizing for next reclamation victim
selection, advising ``khugepaged`` to collapse or split, or doing nothing but
collecting statistics of the regions.
(用户希望对其关注的区域应用的管理动作。例如，换出（paging out）、为下一次回收受害者选择提升优先级、建议 ``khugepaged`` 进行合并或拆分，或者什么都不做而只是收集区域的统计信息。)

The list of supported actions is defined in DAMOS, but the implementation of
each action is in the DAMON operations set layer because the implementation
normally depends on the monitoring target address space.  For example, the code
for paging specific virtual address ranges out would be different from that for
physical address ranges.  And the monitoring operations implementation sets are
not mandated to support all actions of the list.  Hence, the availability of
specific DAMOS action depends on what operations set is selected to be used
together.
(支持的动作列表在 DAMOS 中定义，但每个动作的实现位于 DAMON 操作集合层，因为实现通常依赖于监控目标地址空间。例如，换出特定虚拟地址范围的代码就与换出物理地址范围的代码不同。而且监控操作实现集合也不被强制要求支持列表中的所有动作。因此，某个特定 DAMOS 动作是否可用，取决于选择了哪个操作集合与它一起使用。)

The list of the supported actions, their meaning, and DAMON operations sets
that supports each action are as below.
(支持的动作列表、它们的含义，以及支持每个动作的 DAMON 操作集合如下。)

 - ``willneed``: Call ``madvise()`` for the region with ``MADV_WILLNEED``.
   Supported by ``vaddr`` and ``fvaddr`` operations set.
   (对该区域调用 ``madvise()`` 并传入 ``MADV_WILLNEED``。由 ``vaddr`` 和 ``fvaddr`` 操作集合支持。)
 - ``cold``: Call ``madvise()`` for the region with ``MADV_COLD``.
   Supported by ``vaddr`` and ``fvaddr`` operations set.
   (对该区域调用 ``madvise()`` 并传入 ``MADV_COLD``。由 ``vaddr`` 和 ``fvaddr`` 操作集合支持。)
 - ``pageout``: Reclaim the region.
   Supported by ``vaddr``, ``fvaddr`` and ``paddr`` operations set.
   (回收该区域。由 ``vaddr``、``fvaddr`` 和 ``paddr`` 操作集合支持。)
 - ``hugepage``: Call ``madvise()`` for the region with ``MADV_HUGEPAGE``.
   Supported by ``vaddr`` and ``fvaddr`` operations set. When
   TRANSPARENT_HUGEPAGE is disabled, the application of the action will just
   fail.
   (对该区域调用 ``madvise()`` 并传入 ``MADV_HUGEPAGE``。由 ``vaddr`` 和 ``fvaddr`` 操作集合支持。当 TRANSPARENT_HUGEPAGE 被禁用时，应用该动作只会失败。)
 - ``nohugepage``: Call ``madvise()`` for the region with ``MADV_NOHUGEPAGE``.
   Supported by ``vaddr`` and ``fvaddr`` operations set. When
   TRANSPARENT_HUGEPAGE is disabled, the application of the action will just
   fail.
   (对该区域调用 ``madvise()`` 并传入 ``MADV_NOHUGEPAGE``。由 ``vaddr`` 和 ``fvaddr`` 操作集合支持。当 TRANSPARENT_HUGEPAGE 被禁用时，应用该动作只会失败。)
 - ``lru_prio``: Prioritize the region on its LRU lists.
   Supported by ``paddr`` operations set.
   (提升该区域在其 LRU 链表上的优先级。由 ``paddr`` 操作集合支持。)
 - ``lru_deprio``: Deprioritize the region on its LRU lists.
   Supported by ``paddr`` operations set.
   (降低该区域在其 LRU 链表上的优先级。由 ``paddr`` 操作集合支持。)
 - ``migrate_hot``: Migrate the regions prioritizing warmer regions.
   Supported by ``vaddr``, ``fvaddr`` and ``paddr`` operations set.
   (迁移区域，优先迁移更热的区域。由 ``vaddr``、``fvaddr`` 和 ``paddr`` 操作集合支持。)
 - ``migrate_cold``: Migrate the regions prioritizing colder regions.
   Supported by ``vaddr``, ``fvaddr`` and ``paddr`` operations set.
   (迁移区域，优先迁移更冷的区域。由 ``vaddr``、``fvaddr`` 和 ``paddr`` 操作集合支持。)
 - ``stat``: Do nothing but count the statistics.
   Supported by all operations sets.
   (什么都不做，只统计计数。由所有操作集合支持。)

Applying the actions except ``stat`` to a region is considered as changing the
region's characteristics.  Hence, DAMOS resets the age of regions when any such
actions are applied to those.
(对区域应用除 ``stat`` 之外的任何动作都被视为改变该区域的特征。因此，当任何此类动作被应用到区域上时，DAMOS 会重置这些区域的年龄。)

To know how user-space can set the action via :ref:`DAMON sysfs interface
<sysfs_interface>`, refer to :ref:`action <sysfs_scheme>` part of the
documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface
<sysfs_interface>` 设置动作，请参阅文档中 :ref:`action <sysfs_scheme>` 部分。)


.. _damon_design_damos_access_pattern:

Target Access Pattern(目标访问模式)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The access pattern of the schemes' interest.  The patterns are constructed with
the properties that DAMON's monitoring results provide, specifically the size,
the access frequency, and the age.  Users can describe their access pattern of
interest by setting minimum and maximum values of the three properties.  If a
region's three properties are in the ranges, DAMOS classifies it as one of the
regions that the scheme is having an interest in.
(方案所关注的访问模式。这些模式由 DAMON 监控结果提供的属性构成，具体是大小、访问频率和年龄（age）。用户可以通过设置这三个属性的最小值和最大值来描述自己关注的访问模式。如果某个区域的这三个属性都落在范围之内，DAMOS 就把它归类为该方案关注的区域之一。)

To know how user-space can set the access pattern via :ref:`DAMON sysfs
interface <sysfs_interface>`, refer to :ref:`access_pattern
<sysfs_access_pattern>` part of the documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs
interface <sysfs_interface>` 设置访问模式，请参阅文档中 :ref:`access_pattern
<sysfs_access_pattern>` 部分。)


.. _damon_design_damos_quotas:

Quotas(配额)
~~~~~~~~~~~~

DAMOS upper-bound overhead control feature.  DAMOS could incur high overhead if
the target access pattern is not properly tuned.  For example, if a huge memory
region having the access pattern of interest is found, applying the scheme's
action to all pages of the huge region could consume unacceptably large system
resources.  Preventing such issues by tuning the access pattern could be
challenging, especially if the access patterns of the workloads are highly
dynamic.
(DAMOS 的开销上限控制特性。如果目标访问模式没有调优好，DAMOS 可能产生很高的开销。例如，如果发现一个拥有所关注访问模式的巨大内存区域，对该巨大区域的所有页面应用方案动作，可能消耗大得无法接受的系统资源。通过调优访问模式来预防此类问题可能很有挑战性，尤其当工作负载的访问模式高度动态时更是如此。)

To mitigate that situation, DAMOS provides an upper-bound overhead control
feature called quotas.  It lets users specify an upper limit of time that DAMOS
can use for applying the action, and/or a maximum bytes of memory regions that
the action can be applied within a user-specified time duration.
(为了缓解这种情况，DAMOS 提供了一个名为配额（quotas）的开销上限控制特性。它允许用户指定 DAMOS 可用于应用动作的时间上限，和/或在用户指定的时长内动作可应用到的内存区域的最大字节数。)

To know how user-space can set the basic quotas via :ref:`DAMON sysfs interface
<sysfs_interface>`, refer to :ref:`quotas <sysfs_quotas>` part of the
documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface
<sysfs_interface>` 设置基本配额，请参阅文档中 :ref:`quotas <sysfs_quotas>` 部分。)


.. _damon_design_damos_quotas_prioritization:

Prioritization(优先级排序)
^^^^^^^^^^^^^^^^^^^^^^^^^^

A mechanism for making a good decision under the quotas.  When the action
cannot be applied to all regions of interest due to the quotas, DAMOS
prioritizes regions and applies the action to only regions having high enough
priorities so that it will not exceed the quotas.
(在配额约束下做出良好决策的机制。当由于配额限制而无法对所有关注区域应用动作时，DAMOS 会对区域进行优先级排序，只对优先级足够高的区域应用动作，从而不超出配额。)

The prioritization mechanism should be different for each action.  For example,
rarely accessed (colder) memory regions would be prioritized for page-out
scheme action.  In contrast, the colder regions would be deprioritized for huge
page collapse scheme action.  Hence, the prioritization mechanisms for each
action are implemented in each DAMON operations set, together with the actions.
(优先级排序机制应当因动作而异。例如，对于换出（page-out）方案动作，很少访问的（更冷的）内存区域会被优先处理；相反，对于大页合并方案动作，更冷的区域会被降低优先级。因此，每个动作的优先级排序机制与动作一起实现在各自的 DAMON 操作集合中。)

Though the implementation is up to the DAMON operations set, it would be common
to calculate the priority using the access pattern properties of the regions.
Some users would want the mechanisms to be personalized for their specific
case.  For example, some users would want the mechanism to weigh the recency
(``age``) more than the access frequency (``nr_accesses``).  DAMOS allows users
to specify the weight of each access pattern property and passes the
information to the underlying mechanism.  Nevertheless, how and even whether
the weight will be respected are up to the underlying prioritization mechanism
implementation.
(尽管具体实现取决于 DAMON 操作集合，但利用区域的访问模式属性来计算优先级是常见做法。有些用户希望机制能针对他们的特定场景进行个性化。例如，有些用户希望机制更看重新近度 (``age``) 而不是访问频率 (``nr_accesses``)。DAMOS 允许用户指定每个访问模式属性的权重，并把该信息传递给底层机制。不过，权重如何被尊重、甚至是否被尊重，都取决于底层优先级排序机制的实现。)

To know how user-space can set the prioritization weights via :ref:`DAMON sysfs
interface <sysfs_interface>`, refer to :ref:`weights <sysfs_quotas>` part of
the documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs
interface <sysfs_interface>` 设置优先级权重，请参阅文档中 :ref:`weights <sysfs_quotas>` 部分。)


.. _damon_design_damos_quotas_auto_tuning:

Aim-oriented Feedback-driven Auto-tuning(面向目标的反馈驱动自动调优)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Automatic feedback-driven quota tuning.  Instead of setting the absolute quota
value, users can specify the metric of their interest, and what target value
they want the metric value to be.  DAMOS then automatically tunes the
aggressiveness (the quota) of the corresponding scheme.  For example, if DAMOS
is under achieving the goal, DAMOS automatically increases the quota.  If DAMOS
is over achieving the goal, it decreases the quota.
(自动的、反馈驱动的配额调优。用户无需设置绝对配额值，而是可以指定自己关心的指标以及希望该指标达到的目标值。然后 DAMOS 会自动调整相应方案的激进程度（即配额）。例如，如果 DAMOS 未达成目标，DAMOS 会自动提高配额；如果超额达成目标，则会降低配额。)

There are two such tuning algorithms that users can select as they need.
(有两种这样的调优算法，用户可以按需选择。)

- ``consist``: A proportional feedback loop based algorithm.  Tries to find an
  optimum quota that should be consistently kept, to keep achieving the goal.
  Useful for kernel-only operation on dynamic and long-running environments.
  This is the default selection.  If unsure, use this.
  (``consist``：基于比例反馈回路的算法。试图找到一个应当持续保持的最优配额，以持续达成目标。适用于仅在内核侧运行、动态且长时间运行的环境。这是默认选择。如果不确定，就用它。)
- ``temporal``: More straightforward algorithm.  Tries to achieve the goal as
  fast as possible, using maximum allowed quota, but only for a temporal short
  time.  When the quota is under-achieved, this algorithm keeps tuning quota to
  a maximum allowed one.  Once the quota is [over]-achieved, this sets the
  quota zero.  Useful for deterministic control required environments.
  (``temporal``：更直接的算法。使用允许的最大配额尽快达成目标，但只在短暂的临时时间段内如此。当配额未达成目标时，该算法持续把配额调到允许的最大值；一旦[超]额达成目标，就把配额设为零。适用于需要确定性控制的环境。)

The goal can be specified with five parameters, namely ``target_metric``,
``target_value``, ``current_value``, ``nid`` and ``path``.  The auto-tuning
mechanism tries to make ``current_value`` of ``target_metric`` be same to
``target_value``.
(目标可以用五个参数来指定：``target_metric``、``target_value``、``current_value``、``nid`` 和 ``path``。自动调优机制试图让 ``target_metric`` 的 ``current_value`` 与 ``target_value`` 相同。)

- ``user_input``: User-provided value.  Users could use any metric that they
  has interest in for the value.  Use space main workload's latency or
  throughput, system metrics like free memory ratio or memory pressure stall
  time (PSI) could be examples.  Note that users should explicitly set
  ``current_value`` on their own in this case.  In other words, users should
  repeatedly provide the feedback.
  (``user_input``：用户提供的值。用户可以为该值使用任何自己关心的指标。例如用户空间主工作负载的延迟或吞吐量，以及空闲内存比例、内存压力停顿时间（PSI）等系统指标。注意，在这种情况下用户应当自行显式设置 ``current_value``。换句话说，用户需要反复提供反馈。)
- ``some_mem_psi_us``: System-wide ``some`` memory pressure stall information
  in microseconds that measured from last quota reset to next quota reset.
  DAMOS does the measurement on its own, so only ``target_value`` need to be
  set by users at the initial time.  In other words, DAMOS does self-feedback.
  (``some_mem_psi_us``：系统范围的 ``some`` 内存压力停顿信息，单位为微秒，测量范围为从上次配额重置到下一次配额重置。DAMOS 自行完成测量，因此最初只需由用户设置 ``target_value``。换句话说，DAMOS 进行自我反馈。)
- ``node_mem_used_bp``: Specific NUMA node's used memory ratio in bp (1/10,000).
  (``node_mem_used_bp``：特定 NUMA 节点的已用内存比例，单位为 bp（1/10,000）。)
- ``node_mem_free_bp``: Specific NUMA node's free memory ratio in bp (1/10,000).
  (``node_mem_free_bp``：特定 NUMA 节点的空闲内存比例，单位为 bp（1/10,000）。)
- ``node_memcg_used_bp``: Specific cgroup's node used memory ratio for a
  specific NUMA node, in bp (1/10,000).
  (``node_memcg_used_bp``：特定 cgroup 在特定 NUMA 节点上的已用内存比例，单位为 bp（1/10,000）。)
- ``node_memcg_free_bp``: Specific cgroup's node unused memory ratio for a
  specific NUMA node, in bp (1/10,000).
  (``node_memcg_free_bp``：特定 cgroup 在特定 NUMA 节点上的未用内存比例，单位为 bp（1/10,000）。)
- ``active_mem_bp``: Active to active + inactive (LRU) memory size ratio in bp
  (1/10,000).
  (``active_mem_bp``：active 占 active + inactive（LRU）内存大小之比，单位为 bp（1/10,000）。)
- ``inactive_mem_bp``: Inactive to active + inactive (LRU) memory size ratio in
  bp (1/10,000).
  (``inactive_mem_bp``：inactive 占 active + inactive（LRU）内存大小之比，单位为 bp（1/10,000）。)

``nid`` is optionally required for only ``node_mem_used_bp``,
``node_mem_free_bp``, ``node_memcg_used_bp`` and ``node_memcg_free_bp`` to
point the specific NUMA node.
(只有 ``node_mem_used_bp``、``node_mem_free_bp``、``node_memcg_used_bp`` 和 ``node_memcg_free_bp`` 才可选地需要 ``nid`` 来指向特定的 NUMA 节点。)

``path`` is optionally required for only ``node_memcg_used_bp`` and
``node_memcg_free_bp`` to point the path to the cgroup.  The value should be
the path of the memory cgroup from the cgroups mount point.
(只有 ``node_memcg_used_bp`` 和 ``node_memcg_free_bp`` 才可选地需要 ``path`` 来指向 cgroup 路径。该值应当是从 cgroups 挂载点算起的 memory cgroup 路径。)

To know how user-space can set the tuning goal metric, the target value, and/or
the current value via :ref:`DAMON sysfs interface <sysfs_interface>`, refer to
:ref:`quota goals <sysfs_schemes_quota_goals>` part of the documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface <sysfs_interface>` 设置调优目标指标、目标值和/或当前值，请参阅文档中 :ref:`quota goals <sysfs_schemes_quota_goals>` 部分。)


.. _damon_design_damos_watermarks:

Watermarks(水位线)
~~~~~~~~~~~~~~~~~~

Conditional DAMOS (de)activation automation.  Users might want DAMOS to run
only under certain situations.  For example, when a sufficient amount of free
memory is guaranteed, running a scheme for proactive reclamation would only
consume unnecessary system resources.  To avoid such consumption, the user would
need to manually monitor some metrics such as free memory ratio, and turn
DAMON/DAMOS on or off.
(条件化的 DAMOS 激活/停用自动化。用户可能希望 DAMOS 只在某些情况下运行。例如，在空闲内存充足已有保证时，运行主动回收方案只会消耗不必要的系统资源。为了避免这种消耗，用户需要手动监控空闲内存比例等一些指标，并开启或关闭 DAMON/DAMOS。)

DAMOS allows users to offload such works using three watermarks.  It allows the
users to configure the metric of their interest, and three watermark values,
namely high, middle, and low.  If the value of the metric becomes above the
high watermark or below the low watermark, the scheme is deactivated.  If the
metric becomes below the mid watermark but above the low watermark, the scheme
is activated.  If all schemes are deactivated by the watermarks, the monitoring
is also deactivated.  In this case, the DAMON worker thread only periodically
checks the watermarks and therefore incurs nearly zero overhead.
(DAMOS 允许用户使用三个水位线（watermark）来卸载这类工作。它允许用户配置自己关心的指标以及三个水位线值：高（high）、中（middle）和低（low）。如果指标值高于高水位线或低于低水位线，方案被停用。如果指标低于中水位线但高于低水位线，方案被激活。如果所有方案都被水位线停用，监控也会被停用。在这种情况下，DAMON 工作线程只周期性地检查水位线，因此开销几乎为零。)

To know how user-space can set the watermarks via :ref:`DAMON sysfs interface
<sysfs_interface>`, refer to :ref:`watermarks <sysfs_watermarks>` part of the
documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface
<sysfs_interface>` 设置水位线，请参阅文档中 :ref:`watermarks <sysfs_watermarks>` 部分。)


.. _damon_design_damos_filters:

Filters(过滤器)
~~~~~~~~~~~~~~~

Non-access pattern-based target memory regions filtering.  If users run
self-written programs or have good profiling tools, they could know something
more than the kernel, such as future access patterns or some special
requirements for specific types of memory. For example, some users may know
only anonymous pages can impact their program's performance.  They can also
have a list of latency-critical processes.
(非基于访问模式的目标内存区域过滤。如果用户运行自己编写的程序或拥有良好的剖析工具，他们可能比内核知道得更多，例如未来的访问模式，或对特定类型内存的特殊要求。例如，有些用户可能知道只有匿名页才会影响其程序的性能。他们也可能持有一个对延迟敏感的进程列表。)

To let users optimize DAMOS schemes with such special knowledge, DAMOS provides
a feature called DAMOS filters.  The feature allows users to set an arbitrary
number of filters for each scheme.  Each filter specifies
(为了让用户能够利用这类特殊知识优化 DAMOS 方案，DAMOS 提供了一个名为 DAMOS 过滤器（filters）的特性。该特性允许用户为每个方案设置任意数量的过滤器。每个过滤器指定：)

- a type of memory (``type``),
  (一种内存类型 (``type``)；)
- whether it is for the memory of the type or all except the type
  (``matching``), and
  (是针对该类型的内存还是除该类型之外的所有内存 (``matching``)；以及)
- whether it is to allow (include) or reject (exclude) applying
  the scheme's action to the memory (``allow``).
  (是允许（包含）还是拒绝（排除）对该内存应用方案动作 (``allow``)。)

For efficient handling of filters, some types of filters are handled by the
core layer, while others are handled by operations set.  In the latter case,
hence, support of the filter types depends on the DAMON operations set.  In
case of the core layer-handled filters, the memory regions that excluded by the
filter are not counted as the scheme has tried to the region.  In contrast, if
a memory regions is filtered by an operations set layer-handled filter, it is
counted as the scheme has tried.  This difference affects the statistics.
(为了高效地处理过滤器，一些类型的过滤器由核心层处理，另一些由操作集合处理。因此，在后一种情况下，过滤器类型的支持情况取决于 DAMON 操作集合。对于由核心层处理的过滤器，被过滤器排除的内存区域不计入方案已尝试（tried）的区域；相反，如果内存区域被操作集合层处理的过滤器过滤掉，它仍计入方案已尝试的区域。这一差异会影响统计数据。)

When multiple filters are installed, the group of filters that handled by the
core layer are evaluated first.  After that, the group of filters that handled
by the operations layer are evaluated.  Filters in each of the groups are
evaluated in the installed order.  If a part of memory is matched to one of the
filter, next filters are ignored.  If the part passes through the filters
evaluation stage because it is not matched to any of the filters, applying the
scheme's action to it depends on the last filter's allowance type.  If the last
filter was for allowing, the part of memory will be rejected, and vice versa.
(当安装了多个过滤器时，先评估由核心层处理的那组过滤器，然后评估由操作层处理的那组过滤器。每组内的过滤器按安装顺序评估。如果某部分内存匹配了其中一个过滤器，则忽略后续过滤器。如果该部分内存因为与任何过滤器都不匹配而通过了过滤器评估阶段，是否对其应用方案动作取决于最后一个过滤器的允许类型：如果最后一个过滤器是允许型，该部分内存将被拒绝；反之亦然。)

For example, let's assume 1) a filter for allowing anonymous pages and 2)
another filter for rejecting young pages are installed in the order.  If a page
of a region that eligible to apply the scheme's action is an anonymous page,
the scheme's action will be applied to the page regardless of whether it is
young or not, since it matches with the first allow-filter.  If the page is
not anonymous but young, the scheme's action will not be applied, since the
second reject-filter blocks it.  If the page is neither anonymous nor young,
the page will pass through the filters evaluation stage since there is no
matching filter, and the action will be applied to the page.
(例如，假设按顺序安装了 1) 一个允许匿名页的过滤器和 2) 一个拒绝年轻页（young page）的过滤器。如果一个符合方案动作应用条件的区域中的页面是匿名页，无论它是否年轻，方案动作都会应用到该页，因为它匹配了第一个允许型过滤器。如果页面不是匿名页而是年轻页，方案动作就不会应用，因为第二个拒绝型过滤器阻止了它。如果页面既不是匿名页也不是年轻页，该页面会因为没有匹配的过滤器而通过过滤器评估阶段，动作将被应用到该页。)

Below ``type`` of filters are currently supported.
(目前支持以下 ``type`` 的过滤器。)

- Core layer handled
  (核心层处理)
    - addr
        - Applied to pages that belonging to a given address range.
          (应用于属于给定地址范围的页面。)
    - target
        - Applied to pages that belonging to a given DAMON monitoring target.
          (应用于属于给定 DAMON 监控目标的页面。)
- Operations layer handled, supported by only ``paddr`` operations set.
  (操作层处理，仅由 ``paddr`` 操作集合支持。)
    - anon
        - Applied to pages that containing data that not stored in files.
          (应用于包含未存储在文件中的数据的页面。)
    - active
        - Applied to active pages.
          (应用于活动页（active pages）。)
    - memcg
        - Applied to pages that belonging to a given cgroup.
          (应用于属于给定 cgroup 的页面。)
    - young
        - Applied to pages that are accessed after the last access check from the
          scheme.
          (应用于自方案上次访问检查之后被访问过的页面。)
    - hugepage_size
        - Applied to pages that managed in a given size range.
          (应用于按给定大小范围管理的页面。)
    - unmapped
        - Applied to pages that unmapped.
          (应用于未映射的页面。)

To know how user-space can set the filters via :ref:`DAMON sysfs interface
<sysfs_interface>`, refer to :ref:`filters <sysfs_filters>` part of the
documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface
<sysfs_interface>` 设置过滤器，请参阅文档中 :ref:`filters <sysfs_filters>` 部分。)

.. _damon_design_damos_stat:

Statistics(统计信息)
~~~~~~~~~~~~~~~~~~~~

The statistics of DAMOS behaviors that designed to help monitoring, tuning and
debugging of DAMOS.
(DAMOS 行为的统计信息，旨在帮助监控、调优和调试 DAMOS。)

DAMOS accounts below statistics for each scheme, from the beginning of the
scheme's execution.
(DAMOS 从每个方案开始执行起统计以下各项数据。)

- ``nr_tried``: Total number of regions that the scheme is tried to be applied.
  (``nr_tried``：方案已尝试应用的区域总数。)
- ``sz_tried``: Total size of regions that the scheme is tried to be applied.
  (``sz_tried``：方案已尝试应用的区域总大小。)
- ``sz_ops_filter_passed``: Total bytes that passed operations set
  layer-handled DAMOS filters.
  (``sz_ops_filter_passed``：通过操作集合层处理的 DAMOS 过滤器的总字节数。)
- ``nr_applied``: Total number of regions that the scheme is applied.
  (``nr_applied``：方案已应用动作的区域总数。)
- ``sz_applied``: Total size of regions that the scheme is applied.
  (``sz_applied``：方案已应用动作的区域总大小。)
- ``qt_exceeds``: Total number of times the quota of the scheme has exceeded.
  (``qt_exceeds``：方案配额被超出的总次数。)
- ``nr_snapshots``: Total number of DAMON snapshots that the scheme is tried to
  be applied.
  (``nr_snapshots``：方案已尝试应用所在的 DAMON 快照总数。)
- ``max_nr_snapshots``: Upper limit of ``nr_snapshots``.
  (``max_nr_snapshots``：``nr_snapshots`` 的上限。)

"A scheme is tried to be applied to a region" means DAMOS core logic determined
the region is eligible to apply the scheme's :ref:`action
<damon_design_damos_action>`.  The :ref:`access pattern
<damon_design_damos_access_pattern>`, :ref:`quotas
<damon_design_damos_quotas>`, :ref:`watermarks
<damon_design_damos_watermarks>`, and :ref:`filters
<damon_design_damos_filters>` that handled on core logic could affect this.
The core logic will only ask the underlying :ref:`operation set
<damon_operations_set>` to do apply the action to the region, so whether the
action is really applied or not is unclear.  That's why it is called "tried".
("方案被尝试应用到某个区域"是指 DAMOS 核心逻辑已判定该区域符合应用方案 :ref:`action
<damon_design_damos_action>` 的条件。由核心逻辑处理的 :ref:`access pattern
<damon_design_damos_access_pattern>`、:ref:`quotas
<damon_design_damos_quotas>`、:ref:`watermarks
<damon_design_damos_watermarks>` 和 :ref:`filters
<damon_design_damos_filters>` 都可能影响这一点。核心逻辑只会要求底层 :ref:`operation set
<damon_operations_set>` 对该区域应用动作，因此动作是否真正被应用并不确定。这就是它被称为"尝试"（tried）的原因。)

"A scheme is applied to a region" means the :ref:`operation set
<damon_operations_set>` has applied the action to at least a part of the
region.  The :ref:`filters <damon_design_damos_filters>` that handled by the
operation set, and the types of the :ref:`action <damon_design_damos_action>`
and the pages of the region can affect this.  For example, if a filter is set
to exclude anonymous pages and the region has only anonymous pages, or if the
action is ``pageout`` while all pages of the region are unreclaimable, applying
the action to the region will fail.
("方案已应用到某个区域"是指 :ref:`operation set
<damon_operations_set>` 已把动作应用到了该区域的至少一部分。由操作集合处理的 :ref:`filters <damon_design_damos_filters>`、:ref:`action <damon_design_damos_action>`
的类型以及区域中页面的类型都可能影响这一点。例如，如果过滤器被设置为排除匿名页而区域中只有匿名页，或者动作是 ``pageout`` 而区域的所有页面都不可回收，那么对该区域应用动作就会失败。)

Unlike normal stats, ``max_nr_snapshots`` is set by users.  If it is set as
non-zero and ``nr_snapshots`` be same to or greater than ``nr_snapshots``, the
scheme is deactivated.
(与普通统计数据不同，``max_nr_snapshots`` 由用户设置。如果它被设为非零且 ``nr_snapshots`` 达到或超过该值，方案就会被停用。)

To know how user-space can read the stats via :ref:`DAMON sysfs interface
<sysfs_interface>`, refer to :ref:s`stats <sysfs_stats>` part of the
documentation.
(要了解用户空间如何通过 :ref:`DAMON sysfs interface
<sysfs_interface>` 读取统计数据，请参阅文档中 :ref:s`stats <sysfs_stats>` 部分。)

Regions Walking(区域遍历)
~~~~~~~~~~~~~~~~~~~~~~~~~

DAMOS feature allowing users access each region that a DAMOS action has just
applied.  Using this feature, DAMON :ref:`API <damon_design_api>` allows users
access full properties of the regions including the access monitoring results
and amount of the region's internal memory that passed the DAMOS filters.
:ref:`DAMON sysfs interface <sysfs_interface>` also allows users read the data
via special :ref:`files <sysfs_schemes_tried_regions>`.
(DAMOS 特性，允许用户访问 DAMOS 动作刚刚应用到的每个区域。借助该特性，DAMON :ref:`API <damon_design_api>` 允许用户访问区域的完整属性，包括访问监控结果以及区域内部通过 DAMOS 过滤器的内存量。:ref:`DAMON sysfs interface <sysfs_interface>` 也允许用户通过特殊的 :ref:`files <sysfs_schemes_tried_regions>` 读取这些数据。)

.. _damon_design_api:

Application Programming Interface(应用程序编程接口)
---------------------------------------------------

The programming interface for kernel space data access-aware applications.
DAMON is a framework, so it does nothing by itself.  Instead, it only helps
other kernel components such as subsystems and modules building their data
access-aware applications using DAMON's core features.  For this, DAMON exposes
its all features to other kernel components via its application programming
interface, namely ``include/linux/damon.h``.  Please refer to the API
:doc:`document </mm/damon/api>` for details of the interface.
(面向内核空间数据访问感知应用的编程接口。DAMON 是一个框架，它本身什么也不做。相反，它只是帮助子系统、模块等其他内核组件利用 DAMON 的核心特性构建自己的数据访问感知应用。为此，DAMON 通过其应用程序编程接口（即 ``include/linux/damon.h``）把所有特性暴露给其他内核组件。接口详情请参阅 API :doc:`document </mm/damon/api>`。)


.. _damon_modules:

Modules(模块)
=============

Because the core of DAMON is a framework for kernel components, it doesn't
provide any direct interface for the user space.  Such interfaces should be
implemented by each DAMON API user kernel components, instead.  DAMON subsystem
itself implements such DAMON API user modules, which are supposed to be used
for general purpose DAMON control and special purpose data access-aware system
operations, and provides stable application binary interfaces (ABI) for the
user space.  The user space can build their efficient data access-aware
applications using the interfaces.
(由于 DAMON 的核心是面向内核组件的框架，它不为用户空间提供任何直接接口。这类接口应当由每个使用 DAMON API 的内核组件来实现。DAMON 子系统自身实现了一些这样的 DAMON API 用户模块，它们用于通用 DAMON 控制和专用数据访问感知系统操作，并为用户空间提供稳定的应用二进制接口（ABI）。用户空间可以利用这些接口构建高效的数据访问感知应用。)


General Purpose User Interface Modules(通用用户接口模块)
--------------------------------------------------------

DAMON modules that provide user space ABIs for general purpose DAMON usage in
runtime.
(为运行时通用 DAMON 使用提供用户空间 ABI 的 DAMON 模块。)

Like many other ABIs, the modules create files on pseudo file systems like
'sysfs', allow users to specify their requests to and get the answers from
DAMON by writing to and reading from the files.  As a response to such I/O,
DAMON user interface modules control DAMON and retrieve the results as user
requested via the DAMON API, and return the results to the user-space.
(与许多其他 ABI 一样，这些模块在 'sysfs' 等伪文件系统上创建文件，允许用户通过写入和读取这些文件向 DAMON 提出请求并获取回答。作为对这类 I/O 的响应，DAMON 用户接口模块通过 DAMON API 按用户的请求控制 DAMON 并取回结果，然后把结果返回给用户空间。)

The ABIs are designed to be used for user space applications development,
rather than human beings' fingers.  Human users are recommended to use such
user space tools.  One such Python-written user space tool is available at
Github (https://github.com/damonitor/damo), Pypi
(https://pypistats.org/packages/damo), and multiple distros
(https://repology.org/project/damo/versions).
(这些 ABI 的设计用途是用户空间应用开发，而不是供人手直接操作。建议人类用户使用这样的用户空间工具。一个用 Python 编写的此类用户空间工具可以在 Github (https://github.com/damonitor/damo)、Pypi
(https://pypistats.org/packages/damo) 以及多个发行版
(https://repology.org/project/damo/versions) 上获得。)

Currently, one module for this type, namely 'DAMON sysfs interface' is
available.  Please refer to the ABI :ref:`doc <sysfs_interface>` for details of
the interfaces.
(目前，这一类型的模块有一个，即 'DAMON sysfs interface'。接口详情请参阅 ABI :ref:`doc <sysfs_interface>`。)


.. _damon_modules_special_purpose:

Special-Purpose Access-aware Kernel Modules(专用访问感知内核模块)
-----------------------------------------------------------------

DAMON modules that provide user space ABI for specific purpose DAMON usage.
(为特定目的的 DAMON 使用提供用户空间 ABI 的 DAMON 模块。)

DAMON user interface modules are for full control of all DAMON features in
runtime.  For each special-purpose system-wide data access-aware system
operations such as proactive reclamation or LRU lists balancing, the interfaces
could be simplified by removing unnecessary knobs for the specific purpose, and
extended for boot-time and even compile time control.  Default values of DAMON
control parameters for the usage would also need to be optimized for the
purpose.
(DAMON 用户接口模块用于在运行时完全控制所有 DAMON 特性。对于每个专用的系统级数据访问感知系统操作（如主动回收或 LRU 链表平衡），可以移除对该特定目的不必要的旋钮来简化接口，并扩展出启动时甚至编译时的控制能力。用于该用途的 DAMON 控制参数默认值也需要针对该目的进行优化。)

To support such cases, yet more DAMON API user kernel modules that provide more
simple and optimized user space interfaces are available.  Currently, two
modules for proactive reclamation and LRU lists manipulation are provided.  For
more detail, please read the usage documents for those
(:doc:`/admin-guide/mm/damon/stat`, :doc:`/admin-guide/mm/damon/reclaim` and
:doc:`/admin-guide/mm/damon/lru_sort`).
(为了支持这类场景，还有更多提供更简单、更优化用户空间接口的 DAMON API 用户内核模块可用。目前提供了两个模块，分别用于主动回收和 LRU 链表操作。更多细节请阅读它们的使用文档 (:doc:`/admin-guide/mm/damon/stat`、:doc:`/admin-guide/mm/damon/reclaim` 和 :doc:`/admin-guide/mm/damon/lru_sort`)。)

.. _damon_design_special_purpose_modules_exclusivity:

Note that these modules currently run in an exclusive manner.  If one of those
is already running, others will return ``-EBUSY`` upon start requests.
(注意，这些模块目前以互斥方式运行。如果其中一个已经在运行，其他模块在收到启动请求时会返回 ``-EBUSY``。)

Sample DAMON Modules(示例 DAMON 模块)
-------------------------------------

DAMON modules that provides example DAMON kernel API usages.
(提供 DAMON 内核 API 使用示例的 DAMON 模块。)

kernel programmers can build their own special or general purpose DAMON modules
using DAMON kernel API.  To help them easily understand how DAMON kernel API
can be used, a few sample modules are provided under ``samples/damon/`` of the
linux source tree.  Please note that these modules are not developed for being
used on real products, but only for showing how DAMON kernel API can be used in
simple ways.
(内核程序员可以使用 DAMON 内核 API 构建自己的专用或通用 DAMON 模块。为了帮助他们轻松理解 DAMON 内核 API 的使用方法，linux 源码树的 ``samples/damon/`` 目录下提供了一些示例模块。请注意，这些模块并非为真实产品使用而开发，只是用于展示如何以简单的方式使用 DAMON 内核 API。)
