.. SPDX-License-Identifier: GPL-2.0

=========================
Physical Memory(物理内存)
=========================

Linux is available for a wide range of architectures so there is a need for an
architecture-independent abstraction to represent the physical memory. This
chapter describes the structures used to manage physical memory in a running
system.
(Linux 可用于非常广泛的架构，因此需要一个与架构无关的抽象来表示物理内存。本章描述在运行系统中管理物理内存所用的结构。)

The first principal concept prevalent in the memory management is
`Non-Uniform Memory Access (NUMA)
<https://en.wikipedia.org/wiki/Non-uniform_memory_access>`_.
With multi-core and multi-socket machines, memory may be arranged into banks
that incur a different cost to access depending on the “distance” from the
processor. For example, there might be a bank of memory assigned to each CPU or
a bank of memory very suitable for DMA near peripheral devices.
(内存管理中第一个普遍存在的主要概念是 `Non-Uniform Memory Access (NUMA)
<https://en.wikipedia.org/wiki/Non-uniform_memory_access>`_ (非一致内存访问)。
在多核与多插槽机器上，内存可能被组织成多个存储体 (bank)，根据其与处理器的"距离"不同，访问代价也不同。例如，可能每个 CPU 都分配有一个存储体，或者外围设备附近有一个非常适合 DMA 的存储体。)

Each bank is called a node and the concept is represented under Linux by a
``struct pglist_data`` even if the architecture is UMA. This structure is
always referenced by its typedef ``pg_data_t``. A ``pg_data_t`` structure
for a particular node can be referenced by ``NODE_DATA(nid)`` macro where
``nid`` is the ID of that node.
(每个存储体称为一个节点 (node)，即使在 UMA 架构下，Linux 也用 ``struct pglist_data`` 来表示这个概念。该结构总是通过其 typedef ``pg_data_t`` 来引用。特定节点的 ``pg_data_t`` 结构可以通过 ``NODE_DATA(nid)`` 宏引用，其中 ``nid`` 是该节点的 ID。)

For NUMA architectures, the node structures are allocated by the architecture
specific code early during boot. Usually, these structures are allocated
locally on the memory bank they represent. For UMA architectures, only one
static ``pg_data_t`` structure called ``contig_page_data`` is used. Nodes will
be discussed further in Section :ref:`Nodes <nodes>`
(对 NUMA 架构，节点结构由架构相关代码在启动早期分配。通常，这些结构就分配在它们所代表的存储体本地。对 UMA 架构，只使用一个名为 ``contig_page_data`` 的静态 ``pg_data_t`` 结构。节点将在 :ref:`Nodes <nodes>` 一节进一步讨论。)

The entire physical address space is partitioned into one or more blocks
called zones which represent ranges within memory. These ranges are usually
determined by architectural constraints for accessing the physical memory.
The memory range within a node that corresponds to a particular zone is
described by a ``struct zone``. Each zone has
one of the types described below.
(整个物理地址空间被划分为一个或多个称为管理区 (zone) 的块，它们代表内存中的各个范围。这些范围通常由访问物理内存的架构约束决定。节点内与管理区对应的内存范围由 ``struct zone`` 描述。每个管理区都属于下文所述的类型之一。)

* ``ZONE_DMA`` and ``ZONE_DMA32`` historically represented memory suitable for
  DMA by peripheral devices that cannot access all of the addressable
  memory. For many years there are better more and robust interfaces to get
  memory with DMA specific requirements (Documentation/core-api/dma-api.rst),
  but ``ZONE_DMA`` and ``ZONE_DMA32`` still represent memory ranges that have
  restrictions on how they can be accessed.
  Depending on the architecture, either of these zone types or even they both
  can be disabled at build time using ``CONFIG_ZONE_DMA`` and
  ``CONFIG_ZONE_DMA32`` configuration options. Some 64-bit platforms may need
  both zones as they support peripherals with different DMA addressing
  limitations.
  (``ZONE_DMA`` 和 ``ZONE_DMA32`` 历史上代表适合那些无法访问全部可寻址内存的外围设备做 DMA 的内存。多年来已经有了更好、更健壮的接口来获取有特定 DMA 要求的内存 (Documentation/core-api/dma-api.rst)，但 ``ZONE_DMA`` 和 ``ZONE_DMA32`` 仍然代表在访问方式上有限制的内存范围。取决于架构，这两类管理区可以单独或同时在构建时通过 ``CONFIG_ZONE_DMA`` 和 ``CONFIG_ZONE_DMA32`` 配置选项禁用。一些 64 位平台可能两个管理区都需要，因为它们支持 DMA 寻址能力不同的外围设备。)

* ``ZONE_NORMAL`` is for normal memory that can be accessed by the kernel all
  the time. DMA operations can be performed on pages in this zone if the DMA
  devices support transfers to all addressable memory. ``ZONE_NORMAL`` is
  always enabled.
  (``ZONE_NORMAL`` 用于内核随时都能访问的普通内存。如果 DMA 设备支持向全部可寻址内存传输，则可以对该管理区中的页执行 DMA 操作。``ZONE_NORMAL`` 总是启用。)

* ``ZONE_HIGHMEM`` is the part of the physical memory that is not covered by a
  permanent mapping in the kernel page tables. The memory in this zone is only
  accessible to the kernel using temporary mappings. This zone is available
  only on some 32-bit architectures and is enabled with ``CONFIG_HIGHMEM``.
  (``ZONE_HIGHMEM`` 是物理内存中未被内核页表永久映射覆盖的部分。该管理区中的内存只能通过临时映射供内核访问。此管理区仅在某些 32 位架构上可用，通过 ``CONFIG_HIGHMEM`` 启用。)

* ``ZONE_MOVABLE`` is for normal accessible memory, just like ``ZONE_NORMAL``.
  The difference is that the contents of most pages in ``ZONE_MOVABLE`` is
  movable. That means that while virtual addresses of these pages do not
  change, their content may move between different physical pages. Often
  ``ZONE_MOVABLE`` is populated during memory hotplug, but it may be
  also populated on boot using one of ``kernelcore``, ``movablecore`` and
  ``movable_node`` kernel command line parameters. See
  Documentation/mm/page_migration.rst and
  Documentation/admin-guide/mm/memory-hotplug.rst for additional details.
  (``ZONE_MOVABLE`` 与 ``ZONE_NORMAL`` 一样是普通可访问内存。区别在于 ``ZONE_MOVABLE`` 中大多数页的内容是可移动的。也就是说，这些页的虚拟地址不变，但其内容可能在不同物理页之间移动。``ZONE_MOVABLE`` 通常在内存热插拔期间填充，但也可以在启动时通过 ``kernelcore``、``movablecore`` 和 ``movable_node`` 内核命令行参数之一填充。更多细节见 Documentation/mm/page_migration.rst 和 Documentation/admin-guide/mm/memory-hotplug.rst。)

* ``ZONE_DEVICE`` represents memory residing on devices such as PMEM and GPU.
  It has different characteristics than RAM zone types and it exists to provide
  :ref:`struct page <Pages>` and memory map services for device driver
  identified physical address ranges. ``ZONE_DEVICE`` is enabled with
  configuration option ``CONFIG_ZONE_DEVICE``.
  (``ZONE_DEVICE`` 表示位于 PMEM 和 GPU 等设备上的内存。它与 RAM 类管理区特性不同，其存在是为了为设备驱动识别出的物理地址范围提供 :ref:`struct page <Pages>` 与内存映射服务。``ZONE_DEVICE`` 通过配置选项 ``CONFIG_ZONE_DEVICE`` 启用。)

It is important to note that many kernel operations can only take place using
``ZONE_NORMAL`` so it is the most performance critical zone. Zones are
discussed further in Section :ref:`Zones <zones>`.
(需要注意的是，许多内核操作只能使用 ``ZONE_NORMAL`` 进行，因此它是性能最关键的管理区。管理区将在 :ref:`Zones <zones>` 一节进一步讨论。)

The relation between node and zone extents is determined by the physical memory
map reported by the firmware, architectural constraints for memory addressing
and certain parameters in the kernel command line.
(节点与管理区范围之间的关系由固件报告的物理内存映射、内存寻址的架构约束以及内核命令行中的某些参数决定。)

For example, with 32-bit kernel on an x86 UMA machine with 2 Gbytes of RAM the
entire memory will be on node 0 and there will be three zones: ``ZONE_DMA``,
``ZONE_NORMAL`` and ``ZONE_HIGHMEM``::

  0                                                            2G
  +-------------------------------------------------------------+
  |                            node 0                           |
  +-------------------------------------------------------------+

  0         16M                    896M                        2G
  +----------+-----------------------+--------------------------+
  | ZONE_DMA |      ZONE_NORMAL      |       ZONE_HIGHMEM       |
  +----------+-----------------------+--------------------------+

(例如，在具有 2 GB 内存的 x86 UMA 机器上使用 32 位内核时，全部内存都位于节点 0 上，并且会有三个管理区：``ZONE_DMA``、``ZONE_NORMAL`` 和 ``ZONE_HIGHMEM``：)


With a kernel built with ``ZONE_DMA`` disabled and ``ZONE_DMA32`` enabled and
booted with ``movablecore=80%`` parameter on an arm64 machine with 16 Gbytes of
RAM equally split between two nodes, there will be ``ZONE_DMA32``,
``ZONE_NORMAL`` and ``ZONE_MOVABLE`` on node 0, and ``ZONE_NORMAL`` and
``ZONE_MOVABLE`` on node 1::


  1G                                9G                         17G
  +--------------------------------+ +--------------------------+
  |              node 0            | |          node 1          |
  +--------------------------------+ +--------------------------+

  1G       4G        4200M          9G          9320M          17G
  +---------+----------+-----------+ +------------+-------------+
  |  DMA32  |  NORMAL  |  MOVABLE  | |   NORMAL   |   MOVABLE   |
  +---------+----------+-----------+ +------------+-------------+

(如果内核构建时禁用了 ``ZONE_DMA``、启用了 ``ZONE_DMA32``，并以 ``movablecore=80%`` 参数启动一台内存为 16 GB、平均分到两个节点的 arm64 机器，则节点 0 上会有 ``ZONE_DMA32``、``ZONE_NORMAL`` 和 ``ZONE_MOVABLE``，节点 1 上会有 ``ZONE_NORMAL`` 和 ``ZONE_MOVABLE``：)


Memory banks may belong to interleaving nodes. In the example below an x86
machine has 16 Gbytes of RAM in 4 memory banks, even banks belong to node 0
and odd banks belong to node 1::


  0              4G              8G             12G            16G
  +-------------+ +-------------+ +-------------+ +-------------+
  |    node 0   | |    node 1   | |    node 0   | |    node 1   |
  +-------------+ +-------------+ +-------------+ +-------------+

  0   16M      4G
  +-----+-------+ +-------------+ +-------------+ +-------------+
  | DMA | DMA32 | |    NORMAL   | |    NORMAL   | |    NORMAL   |
  +-----+-------+ +-------------+ +-------------+ +-------------+

(存储体可能属于交错的节点。在下面的例子中，一台 x86 机器有 4 个存储体共 16 GB 内存，偶数存储体属于节点 0，奇数存储体属于节点 1：)

In this case node 0 will span from 0 to 12 Gbytes and node 1 will span from
4 to 16 Gbytes.
(这种情况下，节点 0 覆盖 0 到 12 GB，节点 1 覆盖 4 到 16 GB。)

.. _nodes:

Nodes(节点)
===========

As we have mentioned, each node in memory is described by a ``pg_data_t`` which
is a typedef for a ``struct pglist_data``. When allocating a page, by default
Linux uses a node-local allocation policy to allocate memory from the node
closest to the running CPU. As processes tend to run on the same CPU, it is
likely the memory from the current node will be used. The allocation policy can
be controlled by users as described in
Documentation/admin-guide/mm/numa_memory_policy.rst.
(如前所述，内存中的每个节点都由 ``pg_data_t`` 描述，它是 ``struct pglist_data`` 的 typedef。分配页时，Linux 默认采用节点本地分配策略，从距运行 CPU 最近的节点分配内存。由于进程倾向于在同一 CPU 上运行，使用当前节点内存的可能性较大。用户可以按 Documentation/admin-guide/mm/numa_memory_policy.rst 中的说明控制分配策略。)

Most NUMA architectures maintain an array of pointers to the node
structures. The actual structures are allocated early during boot when
architecture specific code parses the physical memory map reported by the
firmware. The bulk of the node initialization happens slightly later in the
boot process by free_area_init() function, described later in Section
:ref:`Initialization <initialization>`.
(大多数 NUMA 架构维护一个指向节点结构的指针数组。实际的结构在启动早期由架构相关代码解析固件报告的物理内存映射时分配。节点初始化的主要工作稍晚一些在启动过程中由 free_area_init() 函数完成，见后文 :ref:`Initialization <initialization>` 一节。)


Along with the node structures, kernel maintains an array of ``nodemask_t``
bitmasks called ``node_states``. Each bitmask in this array represents a set of
nodes with particular properties as defined by ``enum node_states``:
(与节点结构一起，内核还维护一个名为 ``node_states`` 的 ``nodemask_t`` 位掩码数组。该数组中的每个位掩码代表具有特定属性的一组节点，属性由 ``enum node_states`` 定义：)

``N_POSSIBLE``
  The node could become online at some point.
  (该节点将来某个时刻可能上线。)
``N_ONLINE``
  The node is online.
  (该节点已上线。)
``N_NORMAL_MEMORY``
  The node has regular memory.
  (该节点拥有常规内存。)
``N_HIGH_MEMORY``
  The node has regular or high memory. When ``CONFIG_HIGHMEM`` is disabled
  aliased to ``N_NORMAL_MEMORY``.
  (该节点拥有常规内存或高端内存。当 ``CONFIG_HIGHMEM`` 被禁用时，它是 ``N_NORMAL_MEMORY`` 的别名。)
``N_MEMORY``
  The node has memory(regular, high, movable)
  (该节点拥有内存(常规、高端、可移动)。)
``N_CPU``
  The node has one or more CPUs
  (该节点拥有一个或多个 CPU。)
``N_GENERIC_INITIATOR``
  The node has one or more Generic Initiators
  (该节点拥有一个或多个 Generic Initiator。)

For each node that has a property described above, the bit corresponding to the
node ID in the ``node_states[<property>]`` bitmask is set.
(对于具有上述属性的每个节点，``node_states[<property>]`` 位掩码中与该节点 ID 对应的比特会被置位。)

For example, for node 2 with normal memory and CPUs, bit 2 will be set in ::

  node_states[N_POSSIBLE]
  node_states[N_ONLINE]
  node_states[N_NORMAL_MEMORY]
  node_states[N_HIGH_MEMORY]
  node_states[N_MEMORY]
  node_states[N_CPU]

(例如，对于拥有常规内存和 CPU 的节点 2，比特 2 将在以下位掩码中被置位：)

For various operations possible with nodemasks please refer to
``include/linux/nodemask.h``.
(节点掩码可进行的各种操作见 ``include/linux/nodemask.h``。)

Among other things, nodemasks are used to provide macros for node traversal,
namely ``for_each_node()`` and ``for_each_online_node()``.
(节点掩码还用于提供节点遍历宏，即 ``for_each_node()`` 和 ``for_each_online_node()``。)

For instance, to call a function foo() for each online node::

	for_each_online_node(nid) {
		pg_data_t *pgdat = NODE_DATA(nid);

		foo(pgdat);
	}

(例如，要对每个在线节点调用函数 foo()：)

Node structure(节点结构)
------------------------

The nodes structure ``struct pglist_data`` is declared in
``include/linux/mmzone.h``. Here we briefly describe fields of this
structure:
(节点结构 ``struct pglist_data`` 声明在 ``include/linux/mmzone.h`` 中。这里我们简要描述该结构的字段：)

General(通用)
~~~~~~~~~~~~~

``node_zones``
  The zones for this node.  Not all of the zones may be populated, but it is
  the full list. It is referenced by this node's node_zonelists as well as
  other node's node_zonelists.
  (本节点的各管理区。并非所有管理区都一定被填充，但这是完整列表。本节点的 node_zonelists 以及其他节点的 node_zonelists 都会引用它。)

``node_zonelists``
  The list of all zones in all nodes. This list defines the order of zones
  that allocations are preferred from. The ``node_zonelists`` is set up by
  ``build_zonelists()`` in ``mm/page_alloc.c`` during the initialization of
  core memory management structures.
  (所有节点中全部管理区的列表。该列表定义分配时优先使用的管理区顺序。``node_zonelists`` 由 ``mm/page_alloc.c`` 中的 ``build_zonelists()`` 在初始化核心内存管理结构时建立。)

``nr_zones``
  Number of populated zones in this node.
  (本节点中已填充的管理区数量。)

``node_mem_map``
  For UMA systems that use FLATMEM memory model the 0's node
  ``node_mem_map`` is array of struct pages representing each physical frame.
  (对于使用 FLATMEM 内存模型的 UMA 系统，节点 0 的 ``node_mem_map`` 是表示每个物理帧的 struct page 数组。)

``node_page_ext``
  For UMA systems that use FLATMEM memory model the 0's node
  ``node_page_ext`` is array of extensions of struct pages. Available only
  in the kernels built with ``CONFIG_PAGE_EXTENSION`` enabled.
  (对于使用 FLATMEM 内存模型的 UMA 系统，节点 0 的 ``node_page_ext`` 是 struct page 扩展信息的数组。仅在启用 ``CONFIG_PAGE_EXTENSION`` 构建的内核中可用。)

``node_start_pfn``
  The page frame number of the starting page frame in this node.
  (本节点起始页帧的页帧号。)

``node_present_pages``
  Total number of physical pages present in this node.
  (本节点中存在的物理页总数。)

``node_spanned_pages``
  Total size of physical page range, including holes.
  (物理页范围的总大小，包括空洞。)

``node_size_lock``
  A lock that protects the fields defining the node extents. Only defined when
  at least one of ``CONFIG_MEMORY_HOTPLUG`` or
  ``CONFIG_DEFERRED_STRUCT_PAGE_INIT`` configuration options are enabled.
  ``pgdat_resize_lock()`` and ``pgdat_resize_unlock()`` are provided to
  manipulate ``node_size_lock`` without checking for ``CONFIG_MEMORY_HOTPLUG``
  or ``CONFIG_DEFERRED_STRUCT_PAGE_INIT``.
  (保护定义节点范围字段的锁。仅在 ``CONFIG_MEMORY_HOTPLUG`` 或 ``CONFIG_DEFERRED_STRUCT_PAGE_INIT`` 配置选项至少启用一个时才有定义。提供了 ``pgdat_resize_lock()`` 和 ``pgdat_resize_unlock()``，无需检查 ``CONFIG_MEMORY_HOTPLUG`` 或 ``CONFIG_DEFERRED_STRUCT_PAGE_INIT`` 即可操作 ``node_size_lock``。)

``node_id``
  The Node ID (NID) of the node, starts at 0.
  (节点的节点 ID (NID)，从 0 开始。)

``totalreserve_pages``
  This is a per-node reserve of pages that are not available to userspace
  allocations.
  (每节点保留的、不可用于用户态分配的页数。)

``first_deferred_pfn``
  If memory initialization on large machines is deferred then this is the first
  PFN that needs to be initialized. Defined only when
  ``CONFIG_DEFERRED_STRUCT_PAGE_INIT`` is enabled
  (如果大型机器上的内存初始化被延迟，这就是需要初始化的第一个 PFN。仅在启用 ``CONFIG_DEFERRED_STRUCT_PAGE_INIT`` 时定义。)

``deferred_split_queue``
  Per-node queue of huge pages that their split was deferred. Defined only when ``CONFIG_TRANSPARENT_HUGEPAGE`` is enabled.
  (每节点的"拆分被延迟"大页队列。仅在启用 ``CONFIG_TRANSPARENT_HUGEPAGE`` 时定义。)

``__lruvec``
  Per-node lruvec holding LRU lists and related parameters. Used only when
  memory cgroups are disabled. It should not be accessed directly, use
  ``mem_cgroup_lruvec()`` to look up lruvecs instead.
  (保存 LRU 链表及相关参数的每节点 lruvec。仅在内存 cgroup 被禁用时使用。不应直接访问，应改用 ``mem_cgroup_lruvec()`` 查找 lruvec。)

Reclaim control(回收控制)
~~~~~~~~~~~~~~~~~~~~~~~~~

See also Documentation/mm/page_reclaim.rst.
(另见 Documentation/mm/page_reclaim.rst。)

``kswapd``
  Per-node instance of kswapd kernel thread.
  (每节点一个实例的 kswapd 内核线程。)

``kswapd_wait``, ``pfmemalloc_wait``, ``reclaim_wait``
  Workqueues used to synchronize memory reclaim tasks
  (用于同步内存回收任务的工作队列。)

``nr_writeback_throttled``
  Number of tasks that are throttled waiting on dirty pages to clean.
  (因等待脏页清理而被限流的任务数。)

``nr_reclaim_start``
  Number of pages written while reclaim is throttled waiting for writeback.
  (回收被限流等待回写期间写入的页数。)

``kswapd_order``
  Controls the order kswapd tries to reclaim
  (控制 kswapd 尝试回收的阶 (order)。)

``kswapd_highest_zoneidx``
  The highest zone index to be reclaimed by kswapd
  (kswapd 要回收的最高管理区索引。)

``kswapd_failures``
  Number of runs kswapd was unable to reclaim any pages
  (kswapd 未能回收任何页的运行次数。)

``min_unmapped_pages``
  Minimal number of unmapped file backed pages that cannot be reclaimed.
  Determined by ``vm.min_unmapped_ratio`` sysctl. Only defined when
  ``CONFIG_NUMA`` is enabled.
  (不可回收的未映射文件页的最小数量。由 ``vm.min_unmapped_ratio`` sysctl 决定。仅在启用 ``CONFIG_NUMA`` 时定义。)

``min_slab_pages``
  Minimal number of SLAB pages that cannot be reclaimed. Determined by
  ``vm.min_slab_ratio sysctl``. Only defined when ``CONFIG_NUMA`` is enabled
  (不可回收的 SLAB 页的最小数量。由 ``vm.min_slab_ratio sysctl`` 决定。仅在启用 ``CONFIG_NUMA`` 时定义。)

``flags``
  Flags controlling reclaim behavior.
  (控制回收行为的标志。)

Compaction control(规整控制)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``kcompactd_max_order``
  Page order that kcompactd should try to achieve.
  (kcompactd 应尝试达到的页阶 (order)。)

``kcompactd_highest_zoneidx``
  The highest zone index to be compacted by kcompactd.
  (kcompactd 要规整的最高管理区索引。)

``kcompactd_wait``
  Workqueue used to synchronize memory compaction tasks.
  (用于同步内存规整任务的工作队列。)

``kcompactd``
  Per-node instance of kcompactd kernel thread.
  (每节点一个实例的 kcompactd 内核线程。)

``proactive_compact_trigger``
  Determines if proactive compaction is enabled. Controlled by
  ``vm.compaction_proactiveness`` sysctl.
  (决定是否启用主动规整。由 ``vm.compaction_proactiveness`` sysctl 控制。)

Statistics(统计)
~~~~~~~~~~~~~~~~

``per_cpu_nodestats``
  Per-CPU VM statistics for the node
  (本节点的每 CPU VM 统计。)

``vm_stat``
  VM statistics for the node.
  (本节点的 VM 统计。)

.. _zones:

Zones(管理区)
=============
As we have mentioned, each zone in memory is described by a ``struct zone``
which is an element of the ``node_zones`` array of the node it belongs to.
``struct zone`` is the core data structure of the page allocator. A zone
represents a range of physical memory and may have holes.
(如前所述，内存中的每个管理区都由 ``struct zone`` 描述，它是所属节点 ``node_zones`` 数组的一个元素。``struct zone`` 是页分配器的核心数据结构。管理区代表一段物理内存范围，且可能存在空洞。)

The page allocator uses the GFP flags, see :ref:`mm-api-gfp-flags`, specified by
a memory allocation to determine the highest zone in a node from which the
memory allocation can allocate memory. The page allocator first allocates memory
from that zone, if the page allocator can't allocate the requested amount of
memory from the zone, it will allocate memory from the next lower zone in the
node, the process continues up to and including the lowest zone. For example, if
a node contains ``ZONE_DMA32``, ``ZONE_NORMAL`` and ``ZONE_MOVABLE`` and the
highest zone of a memory allocation is ``ZONE_MOVABLE``, the order of the zones
from which the page allocator allocates memory is ``ZONE_MOVABLE`` >
``ZONE_NORMAL`` > ``ZONE_DMA32``.
(页分配器使用内存分配指定的 GFP 标志 (见 :ref:`mm-api-gfp-flags`) 来确定节点中允许该次内存分配的最高管理区。页分配器首先从该管理区分配内存；如果无法从该管理区分配到所需数量的内存，就会从节点中下一个更低的管理区分配，这一过程一直持续到 (并包括) 最低的管理区。例如，如果一个节点包含 ``ZONE_DMA32``、``ZONE_NORMAL`` 和 ``ZONE_MOVABLE``，而某次内存分配的最高管理区是 ``ZONE_MOVABLE``，则页分配器分配内存的管理区顺序为 ``ZONE_MOVABLE`` > ``ZONE_NORMAL`` > ``ZONE_DMA32``。)

At runtime, free pages in a zone are in the Per-CPU Pagesets (PCP) or free areas
of the zone. The Per-CPU Pagesets are a vital mechanism in the kernel's memory
management system. By handling most frequent allocations and frees locally on
each CPU, the Per-CPU Pagesets improve performance and scalability, especially
on systems with many cores. The page allocator in the kernel employs a two-step
strategy for memory allocation, starting with the Per-CPU Pagesets before
falling back to the buddy allocator. Pages are transferred between the Per-CPU
Pagesets and the global free areas (managed by the buddy allocator) in batches.
This minimizes the overhead of frequent interactions with the global buddy
allocator.
(运行时，管理区中的空闲页位于每 CPU 页集 (Per-CPU Pagesets, PCP) 或管理区的空闲区 (free areas) 中。每 CPU 页集是内核内存管理系统中的重要机制。通过在每 个 CPU 本地处理最频繁的分配与释放，每 CPU 页集提升了性能与可扩展性，尤其是在多核系统上。内核的页分配器采用两步分配策略：先使用每 CPU 页集，再回退到伙伴分配器 (buddy allocator)。页以批次方式在每 CPU 页集与全局空闲区 (由伙伴分配器管理) 之间转移。这最大限度地减少了与全局伙伴分配器频繁交互的开销。)

Architecture specific code calls free_area_init() to initializes zones.
(架构相关代码调用 free_area_init() 来初始化各管理区。)

Zone structure(管理区结构)
--------------------------
The zones structure ``struct zone`` is defined in ``include/linux/mmzone.h``.
Here we briefly describe fields of this structure:
(管理区结构 ``struct zone`` 定义在 ``include/linux/mmzone.h`` 中。这里我们简要描述该结构的字段：)

General(通用)
~~~~~~~~~~~~~

``_watermark``
  The watermarks for this zone. When the amount of free pages in a zone is below
  the min watermark, boosting is ignored, an allocation may trigger direct
  reclaim and direct compaction, it is also used to throttle direct reclaim.
  When the amount of free pages in a zone is below the low watermark, kswapd is
  woken up. When the amount of free pages in a zone is above the high watermark,
  kswapd stops reclaiming (a zone is balanced) when the
  ``NUMA_BALANCING_MEMORY_TIERING`` bit of ``sysctl_numa_balancing_mode`` is not
  set. The promo watermark is used for memory tiering and NUMA balancing. When
  the amount of free pages in a zone is above the promo watermark, kswapd stops
  reclaiming when the ``NUMA_BALANCING_MEMORY_TIERING`` bit of
  ``sysctl_numa_balancing_mode`` is set. The watermarks are set by
  ``__setup_per_zone_wmarks()``. The min watermark is calculated according to
  ``vm.min_free_kbytes`` sysctl. The other three watermarks are set according
  to the distance between two watermarks. The distance itself is calculated
  taking ``vm.watermark_scale_factor`` sysctl into account.
  (本管理区的水位线 (watermark)。当管理区空闲页数量低于 min 水位线时，水位提升 (boosting) 被忽略，一次分配可能触发直接回收 (direct reclaim) 与直接规整 (direct compaction)，min 水位线也用于对直接回收限流。当空闲页数量低于 low 水位线时，唤醒 kswapd。当 ``sysctl_numa_balancing_mode`` 的 ``NUMA_BALANCING_MEMORY_TIERING`` 位未置位且空闲页数量高于 high 水位线时，kswapd 停止回收 (此时管理区是均衡的)。promo 水位线用于内存分层 (memory tiering) 与 NUMA 平衡：当 ``sysctl_numa_balancing_mode`` 的 ``NUMA_BALANCING_MEMORY_TIERING`` 位置位且空闲页数量高于 promo 水位线时，kswapd 停止回收。水位线由 ``__setup_per_zone_wmarks()`` 设置。min 水位线按 ``vm.min_free_kbytes`` sysctl 计算。其余三个水位线按两个水位线之间的距离设置，距离本身在计算时考虑 ``vm.watermark_scale_factor`` sysctl。)

``watermark_boost``
  The number of pages which are used to boost watermarks to increase reclaim
  pressure to reduce the likelihood of future fallbacks and wake kswapd now
  as the node may be balanced overall and kswapd will not wake naturally.
  (用于提升水位线以加大回收压力的页数，目的是减少未来回退 (fallback) 的可能性，并立即唤醒 kswapd——因为节点整体可能是均衡的，kswapd 不会自然醒来。)

``nr_reserved_highatomic``
  The number of pages which are reserved for high-order atomic allocations.
  (为高阶原子分配保留的页数。)

``nr_free_highatomic``
  The number of free pages in reserved highatomic pageblocks
  (保留的高阶原子 pageblock 中的空闲页数。)

``lowmem_reserve``
  The array of the amounts of the memory reserved in this zone for memory
  allocations. For example, if the highest zone a memory allocation can
  allocate memory from is ``ZONE_MOVABLE``, the amount of memory reserved in
  this zone for this allocation is ``lowmem_reserve[ZONE_MOVABLE]`` when
  attempting to allocate memory from this zone. This is a mechanism the page
  allocator uses to prevent allocations which could use ``highmem`` from using
  too much ``lowmem``. For some specialised workloads on ``highmem`` machines,
  it is dangerous for the kernel to allow process memory to be allocated from
  the ``lowmem`` zone. This is because that memory could then be pinned via the
  ``mlock()`` system call, or by unavailability of swapspace.
  ``vm.lowmem_reserve_ratio`` sysctl determines how aggressive the kernel is in
  defending these lower zones. This array is recalculated by
  ``setup_per_zone_lowmem_reserve()`` at runtime if ``vm.lowmem_reserve_ratio``
  sysctl changes.
  (本管理区为各内存分配保留的内存量数组。例如，若某次内存分配可用的最高管理区是 ``ZONE_MOVABLE``，那么尝试从本管理区为该分配分配内存时，本管理区为其保留的内存量是 ``lowmem_reserve[ZONE_MOVABLE]``。这是页分配器用来防止本可使用 ``highmem`` 的分配占用过多 ``lowmem`` 的机制。对 ``highmem`` 机器上的某些特殊工作负载，允许从 ``lowmem`` 管理区分配进程内存是危险的，因为这些内存随后可能通过 ``mlock()`` 系统调用或因交换空间不可用而被固定 (pin)。``vm.lowmem_reserve_ratio`` sysctl 决定内核守护这些低层管理区的激进程度。当 ``vm.lowmem_reserve_ratio`` sysctl 变化时，该数组会在运行时由 ``setup_per_zone_lowmem_reserve()`` 重新计算。)

``node``
  The index of the node this zone belongs to. Available only when
  ``CONFIG_NUMA`` is enabled because there is only one zone in a UMA system.
  (本管理区所属节点的索引。仅在启用 ``CONFIG_NUMA`` 时可用，因为 UMA 系统中只有一个管理区。)

``zone_pgdat``
  Pointer to the ``struct pglist_data`` of the node this zone belongs to.
  (指向本管理区所属节点 ``struct pglist_data`` 的指针。)

``per_cpu_pageset``
  Pointer to the Per-CPU Pagesets (PCP) allocated and initialized by
  ``setup_zone_pageset()``. By handling most frequent allocations and frees
  locally on each CPU, PCP improves performance and scalability on systems with
  many cores.
  (指向由 ``setup_zone_pageset()`` 分配并初始化的每 CPU 页集 (PCP) 的指针。通过在每 个 CPU 本地处理最频繁的分配与释放，PCP 提升了多核系统的性能与可扩展性。)

``pageset_high_min``
  Copied to the ``high_min`` of the Per-CPU Pagesets for faster access.
  (拷贝到每 CPU 页集的 ``high_min``，以便更快访问。)

``pageset_high_max``
  Copied to the ``high_max`` of the Per-CPU Pagesets for faster access.
  (拷贝到每 CPU 页集的 ``high_max``，以便更快访问。)

``pageset_batch``
  Copied to the ``batch`` of the Per-CPU Pagesets for faster access. The
  ``batch``, ``high_min`` and ``high_max`` of the Per-CPU Pagesets are used to
  calculate the number of elements the Per-CPU Pagesets obtain from the buddy
  allocator under a single hold of the lock for efficiency. They are also used
  to decide if the Per-CPU Pagesets return pages to the buddy allocator in page
  free process.
  (拷贝到每 CPU 页集的 ``batch``，以便更快访问。为提高效率，每 CPU 页集的 ``batch``、``high_min`` 和 ``high_max`` 用于计算每 CPU 页集在单次持锁期间从伙伴分配器获取的元素数量；它们还用于决定页释放过程中每 CPU 页集是否把页归还给伙伴分配器。)

``pageblock_flags``
  The pointer to the flags for the pageblocks in the zone (see
  ``include/linux/pageblock-flags.h`` for flags list). The memory is allocated
  in ``setup_usemap()``. Each pageblock occupies ``NR_PAGEBLOCK_BITS`` bits.
  Defined only when ``CONFIG_FLATMEM`` is enabled. The flags is stored in
  ``mem_section`` when ``CONFIG_SPARSEMEM`` is enabled.
  (指向本管理区 pageblock 标志的指针 (标志列表见 ``include/linux/pageblock-flags.h``)。内存在 ``setup_usemap()`` 中分配。每个 pageblock 占用 ``NR_PAGEBLOCK_BITS`` 个比特。仅在启用 ``CONFIG_FLATMEM`` 时定义。启用 ``CONFIG_SPARSEMEM`` 时，标志存储在 ``mem_section`` 中。)

``zone_start_pfn``
  The start pfn of the zone. It is initialized by
  ``calculate_node_totalpages()``.
  (管理区的起始 pfn。由 ``calculate_node_totalpages()`` 初始化。)

``managed_pages``
  The present pages managed by the buddy system, which is calculated as:
  ``managed_pages`` = ``present_pages`` - ``reserved_pages``, ``reserved_pages``
  includes pages allocated by the memblock allocator. It should be used by page
  allocator and vm scanner to calculate all kinds of watermarks and thresholds.
  It is accessed using ``atomic_long_xxx()`` functions. It is initialized in
  ``free_area_init_core()`` and then is reinitialized when memblock allocator
  frees pages into buddy system.
  (伙伴系统管理的现存页数，计算方式为 ``managed_pages`` = ``present_pages`` - ``reserved_pages``，其中 ``reserved_pages`` 包括 memblock 分配器分配的页。页分配器与 VM 扫描器应使用它来计算各种水位线与阈值。它通过 ``atomic_long_xxx()`` 函数访问。它在 ``free_area_init_core()`` 中初始化，随后在 memblock 分配器把页释放进伙伴系统时重新初始化。)

``spanned_pages``
  The total pages spanned by the zone, including holes, which is calculated as:
  ``spanned_pages`` = ``zone_end_pfn`` - ``zone_start_pfn``. It is initialized
  by ``calculate_node_totalpages()``.
  (管理区跨越的总页数，包括空洞，计算方式为 ``spanned_pages`` = ``zone_end_pfn`` - ``zone_start_pfn``。由 ``calculate_node_totalpages()`` 初始化。)

``present_pages``
  The physical pages existing within the zone, which is calculated as:
  ``present_pages`` = ``spanned_pages`` - ``absent_pages`` (pages in holes). It
  may be used by memory hotplug or memory power management logic to figure out
  unmanaged pages by checking (``present_pages`` - ``managed_pages``). Write
  access to ``present_pages`` at runtime should be protected by
  ``mem_hotplug_begin/done()``. Any reader who can't tolerant drift of
  ``present_pages`` should use ``get_online_mems()`` to get a stable value. It
  is initialized by ``calculate_node_totalpages()``.
  (管理区内实际存在的物理页数，计算方式为 ``present_pages`` = ``spanned_pages`` - ``absent_pages`` (空洞中的页)。内存热插拔或内存电源管理逻辑可以通过检查 (``present_pages`` - ``managed_pages``) 来算出未管理的页。运行时对 ``present_pages`` 的写访问应由 ``mem_hotplug_begin/done()`` 保护。无法容忍 ``present_pages`` 漂移的读取者应使用 ``get_online_mems()`` 获取稳定值。由 ``calculate_node_totalpages()`` 初始化。)

``present_early_pages``
  The present pages existing within the zone located on memory available since
  early boot, excluding hotplugged memory. Defined only when
  ``CONFIG_MEMORY_HOTPLUG`` is enabled and initialized by
  ``calculate_node_totalpages()``.
  (管理区内位于自早期启动以来可用内存上的现存页数，不包括热插拔的内存。仅在启用 ``CONFIG_MEMORY_HOTPLUG`` 时定义，由 ``calculate_node_totalpages()`` 初始化。)

``cma_pages``
  The pages reserved for CMA use. These pages behave like ``ZONE_MOVABLE`` when
  they are not used for CMA. Defined only when ``CONFIG_CMA`` is enabled.
  (为 CMA 用途保留的页。这些页在不用于 CMA 时表现得像 ``ZONE_MOVABLE``。仅在启用 ``CONFIG_CMA`` 时定义。)

``name``
  The name of the zone. It is a pointer to the corresponding element of
  the ``zone_names`` array.
  (管理区的名字。它是指向 ``zone_names`` 数组对应元素的指针。)

``nr_isolate_pageblock``
  Number of isolated pageblocks. It is used to solve incorrect freepage counting
  problem due to racy retrieving migratetype of pageblock. Protected by
  ``zone->lock``. Defined only when ``CONFIG_MEMORY_ISOLATION`` is enabled.
  (隔离的 pageblock 数量。用于解决因竞态获取 pageblock 迁移类型而导致空闲页计数不正确的问题。由 ``zone->lock`` 保护。仅在启用 ``CONFIG_MEMORY_ISOLATION`` 时定义。)

``span_seqlock``
  The seqlock to protect ``zone_start_pfn`` and ``spanned_pages``. It is a
  seqlock because it has to be read outside of ``zone->lock``, and it is done in
  the main allocator path. However, the seqlock is written quite infrequently.
  Defined only when ``CONFIG_MEMORY_HOTPLUG`` is enabled.
  (保护 ``zone_start_pfn`` 与 ``spanned_pages`` 的 seqlock。之所以用 seqlock，是因为它必须在 ``zone->lock`` 之外读取，而且读取发生在主分配路径上；不过该 seqlock 的写入相当不频繁。仅在启用 ``CONFIG_MEMORY_HOTPLUG`` 时定义。)

``initialized``
  The flag indicating if the zone is initialized. Set by
  ``init_currently_empty_zone()`` during boot.
  (指示管理区是否已初始化的标志。启动期间由 ``init_currently_empty_zone()`` 设置。)

``free_area``
  The array of free areas, where each element corresponds to a specific order
  which is a power of two. The buddy allocator uses this structure to manage
  free memory efficiently. When allocating, it tries to find the smallest
  sufficient block, if the smallest sufficient block is larger than the
  requested size, it will be recursively split into the next smaller blocks
  until the required size is reached. When a page is freed, it may be merged
  with its buddy to form a larger block. It is initialized by
  ``zone_init_free_lists()``.
  (空闲区数组，每个元素对应一个特定的 2 的幂次阶 (order)。伙伴分配器用该结构高效管理空闲内存。分配时，它尝试找到最小的足够块；如果最小的足够块大于请求的大小，会被递归地拆分成更小的下一级块，直到达到所需大小。释放页时，它可能与伙伴合并成更大的块。由 ``zone_init_free_lists()`` 初始化。)

``unaccepted_pages``
  The list of pages to be accepted. All pages on the list are ``MAX_PAGE_ORDER``.
  Defined only when ``CONFIG_UNACCEPTED_MEMORY`` is enabled.
  (待接受 (accept) 的页的链表。链表上的所有页都是 ``MAX_PAGE_ORDER`` 阶。仅在启用 ``CONFIG_UNACCEPTED_MEMORY`` 时定义。)

``flags``
  The zone flags. The least three bits are used and defined by
  ``enum zone_flags``. ``ZONE_BOOSTED_WATERMARK`` (bit 0): zone recently boosted
  watermarks. Cleared when kswapd is woken. ``ZONE_RECLAIM_ACTIVE`` (bit 1):
  kswapd may be scanning the zone. ``ZONE_BELOW_HIGH`` (bit 2): zone is below
  high watermark.
  (管理区标志。最低三个比特被使用，由 ``enum zone_flags`` 定义。``ZONE_BOOSTED_WATERMARK`` (比特 0)：管理区最近提升过水位线，kswapd 被唤醒时清除。``ZONE_RECLAIM_ACTIVE`` (比特 1)：kswapd 可能正在扫描该管理区。``ZONE_BELOW_HIGH`` (比特 2)：管理区低于 high 水位线。)

``lock``
  The main lock that protects the internal data structures of the page allocator
  specific to the zone, especially protects ``free_area``.
  (保护该管理区页分配器内部数据结构的主锁，尤其保护 ``free_area``。)

``percpu_drift_mark``
  When free pages are below this point, additional steps are taken when reading
  the number of free pages to avoid per-cpu counter drift allowing watermarks
  to be breached. It is updated in ``refresh_zone_stat_thresholds()``.
  (当空闲页低于此点时，读取空闲页数量会采取额外步骤，以避免每 CPU 计数器漂移导致水位线被突破。在 ``refresh_zone_stat_thresholds()`` 中更新。)

Compaction control(规整控制)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``compact_cached_free_pfn``
  The PFN where compaction free scanner should start in the next scan.
  (下次扫描时规整空闲扫描器 (free scanner) 应起始的 PFN。)

``compact_cached_migrate_pfn``
  The PFNs where compaction migration scanner should start in the next scan.
  This array has two elements: the first one is used in ``MIGRATE_ASYNC`` mode,
  and the other one is used in ``MIGRATE_SYNC`` mode.
  (下次扫描时规整迁移扫描器 (migration scanner) 应起始的 PFN。该数组有两个元素：第一个用于 ``MIGRATE_ASYNC`` 模式，另一个用于 ``MIGRATE_SYNC`` 模式。)

``compact_init_migrate_pfn``
  The initial migration PFN which is initialized to 0 at boot time, and to the
  first pageblock with migratable pages in the zone after a full compaction
  finishes. It is used to check if a scan is a whole zone scan or not.
  (初始迁移 PFN，启动时初始化为 0，在完整规整结束后初始化为管理区中第一个含可迁移页的 pageblock。用于判断一次扫描是否为整区扫描。)

``compact_init_free_pfn``
  The initial free PFN which is initialized to 0 at boot time and to the last
  pageblock with free ``MIGRATE_MOVABLE`` pages in the zone. It is used to check
  if it is the start of a scan.
  (初始空闲 PFN，启动时初始化为 0，随后初始化为管理区中最后一个含空闲 ``MIGRATE_MOVABLE`` 页的 pageblock。用于判断是否处于一次扫描的起点。)

``compact_considered``
  The number of compactions attempted since last failure. It is reset in
  ``defer_compaction()`` when a compaction fails to result in a page allocation
  success. It is increased by 1 in ``compaction_deferred()`` when a compaction
  should be skipped. ``compaction_deferred()`` is called before
  ``compact_zone()`` is called, ``compaction_defer_reset()`` is called when
  ``compact_zone()`` returns ``COMPACT_SUCCESS``, ``defer_compaction()`` is
  called when ``compact_zone()`` returns ``COMPACT_PARTIAL_SKIPPED`` or
  ``COMPACT_COMPLETE``.
  (自上次失败以来尝试规整的次数。当一次规整未能带来页分配成功时，在 ``defer_compaction()`` 中重置。当应当跳过一次规整时，在 ``compaction_deferred()`` 中加 1。``compaction_deferred()`` 在 ``compact_zone()`` 调用之前被调用；``compact_zone()`` 返回 ``COMPACT_SUCCESS`` 时调用 ``compaction_defer_reset()``；``compact_zone()`` 返回 ``COMPACT_PARTIAL_SKIPPED`` 或 ``COMPACT_COMPLETE`` 时调用 ``defer_compaction()``。)

``compact_defer_shift``
  The number of compactions skipped before trying again is
  ``1<<compact_defer_shift``. It is increased by 1 in ``defer_compaction()``.
  It is reset in ``compaction_defer_reset()`` when a direct compaction results
  in a page allocation success. Its maximum value is ``COMPACT_MAX_DEFER_SHIFT``.
  (重试之前跳过的规整次数为 ``1<<compact_defer_shift``。在 ``defer_compaction()`` 中加 1。当一次直接规整带来页分配成功时，在 ``compaction_defer_reset()`` 中重置。其最大值为 ``COMPACT_MAX_DEFER_SHIFT``。)

``compact_order_failed``
  The minimum compaction failed order. It is set in ``compaction_defer_reset()``
  when a compaction succeeds and in ``defer_compaction()`` when a compaction
  fails to result in a page allocation success.
  (规整失败的最小阶。规整成功时在 ``compaction_defer_reset()`` 中设置；规整未能带来页分配成功时在 ``defer_compaction()`` 中设置。)

``compact_blockskip_flush``
  Set to true when compaction migration scanner and free scanner meet, which
  means the ``PB_compact_skip`` bits should be cleared.
  (当规整迁移扫描器与空闲扫描器相遇时置为 true，表示应清除 ``PB_compact_skip`` 比特。)

``contiguous``
  Set to true when the zone is contiguous (in other words, no hole).
  (当管理区连续 (即没有空洞) 时置为 true。)

Statistics(统计)
~~~~~~~~~~~~~~~~

``vm_stat``
  VM statistics for the zone. The items tracked are defined by
  ``enum zone_stat_item``.
  (本管理区的 VM 统计。跟踪的条目由 ``enum zone_stat_item`` 定义。)

``vm_numa_event``
  VM NUMA event statistics for the zone. The items tracked are defined by
  ``enum numa_stat_item``.
  (本管理区的 VM NUMA 事件统计。跟踪的条目由 ``enum numa_stat_item`` 定义。)

``per_cpu_zonestats``
  Per-CPU VM statistics for the zone. It records VM statistics and VM NUMA event
  statistics on a per-CPU basis. It reduces updates to the global ``vm_stat``
  and ``vm_numa_event`` fields of the zone to improve performance.
  (本管理区的每 CPU VM 统计。它以每 CPU 为基础记录 VM 统计与 VM NUMA 事件统计，减少对管理区全局 ``vm_stat`` 与 ``vm_numa_event`` 字段的更新以提升性能。)

.. _pages:

Pages(页)
=========

.. admonition:: Stub

   This section is incomplete. Please list and describe the appropriate fields.
   (本节尚未完成。请列出并描述相应的字段。)

.. _folios:

Folios(folio)
=============

.. admonition:: Stub

   This section is incomplete. Please list and describe the appropriate fields.
   (本节尚未完成。请列出并描述相应的字段。)

.. _initialization:

Initialization(初始化)
======================

.. admonition:: Stub

   This section is incomplete. Please list and describe the appropriate fields.
   (本节尚未完成。请列出并描述相应的字段。)
