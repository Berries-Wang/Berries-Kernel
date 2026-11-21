Started Nov 1999 by Kanoj Sarcar <kanoj@sgi.com>
(由 Kanoj Sarcar <kanoj@sgi.com> 于 1999 年 11 月开始编写)

============================
What is NUMA?(什么是 NUMA？)
============================

This question can be answered from a couple of perspectives:  the
hardware view and the Linux software view.
(这个问题可以从两个视角来回答：硬件视角和 Linux 软件视角。)

From the hardware perspective, a NUMA system is a computer platform that
comprises multiple components or assemblies each of which may contain 0
or more CPUs, local memory, and/or IO buses.  For brevity and to
disambiguate the hardware view of these physical components/assemblies
from the software abstraction thereof, we'll call the components/assemblies
'cells' in this document.
(从硬件视角来看，NUMA 系统是一种计算机平台，它由多个组件或组件集合组成，
其中每个组件可以包含 0 个或多个 CPU、本地内存和/或 IO 总线。为了简洁起见，
也为了把这些物理组件/组件集合的硬件视角与其软件抽象区分开来，我们在本文
中把这些组件/组件集合称为 "cell"（单元）。)

Each of the 'cells' may be viewed as an SMP [symmetric multi-processor] subset
of the system--although some components necessary for a stand-alone SMP system
may not be populated on any given cell.   The cells of the NUMA system are
connected together with some sort of system interconnect--e.g., a crossbar or
point-to-point link are common types of NUMA system interconnects.  Both of
these types of interconnects can be aggregated to create NUMA platforms with
cells at multiple distances from other cells.
(每个 "cell" 都可以被看作系统的一个 SMP（对称多处理器）子集——尽管独立
SMP 系统所需的某些组件可能没有装配在某个给定的 cell 上。NUMA 系统的各个
cell 通过某种系统互连连接在一起——例如，交叉开关 (crossbar) 或点对点链路
是常见的 NUMA 系统互连类型。这两种类型的互连都可以聚合起来，创建出各个
cell 与其他 cell 之间具有多种距离的 NUMA 平台。)

For Linux, the NUMA platforms of interest are primarily what is known as Cache
Coherent NUMA or ccNUMA systems.   With ccNUMA systems, all memory is visible
to and accessible from any CPU attached to any cell and cache coherency
is handled in hardware by the processor caches and/or the system interconnect.
(对 Linux 而言，受关注的 NUMA 平台主要是所谓的缓存一致 NUMA（Cache
Coherent NUMA，ccNUMA）系统。在 ccNUMA 系统中，所有内存对连接到任何 cell
的任何 CPU 都是可见且可访问的，而缓存一致性由处理器缓存和/或系统互连在
硬件中处理。)

Memory access time and effective memory bandwidth varies depending on how far
away the cell containing the CPU or IO bus making the memory access is from the
cell containing the target memory.  For example, access to memory by CPUs
attached to the same cell will experience faster access times and higher
bandwidths than accesses to memory on other, remote cells.  NUMA platforms
can have cells at multiple remote distances from any given cell.
(内存访问时间和有效内存带宽随发起内存访问的 CPU 或 IO 总线所在 cell 与
目标内存所在 cell 之间的距离远近而变化。例如，连接到同一 cell 的 CPU 对
内存的访问，比访问其他远程 cell 上的内存会有更快的访问时间和更高的带宽。
NUMA 平台上，相对于任意给定的 cell，可能存在处于多个不同远程距离上的
cell。)

Platform vendors don't build NUMA systems just to make software developers'
lives interesting.  Rather, this architecture is a means to provide scalable
memory bandwidth.  However, to achieve scalable memory bandwidth, system and
application software must arrange for a large majority of the memory references
[cache misses] to be to "local" memory--memory on the same cell, if any--or
to the closest cell with memory.
(平台厂商构建 NUMA 系统并不是为了让软件开发者的生活变得有趣，而是这种
架构是提供可扩展内存带宽的一种手段。然而，要实现可扩展的内存带宽，系统
软件和应用软件必须确保绝大部分内存引用（缓存缺失）指向 "本地" 内存——
即同一 cell 上的内存（如果有的话）——或指向最近的带内存的 cell。)

This leads to the Linux software view of a NUMA system:
(由此引出 Linux 软件视角下的 NUMA 系统：)

Linux divides the system's hardware resources into multiple software
abstractions called "nodes".  Linux maps the nodes onto the physical cells
of the hardware platform, abstracting away some of the details for some
architectures.  As with physical cells, software nodes may contain 0 or more
CPUs, memory and/or IO buses.  And, again, memory accesses to memory on
"closer" nodes--nodes that map to closer cells--will generally experience
faster access times and higher effective bandwidth than accesses to more
remote cells.
(Linux 把系统的硬件资源划分为多个称为 "node"（节点）的软件抽象。Linux 把
节点映射到硬件平台的物理 cell 上，并为某些架构抽象掉一些细节。与物理 cell
一样，软件节点可以包含 0 个或多个 CPU、内存和/或 IO 总线。同样，对 "更近"
节点——映射到更近 cell 的节点——上内存的访问，通常会比访问更远程的 cell
获得更快的访问时间和更高的有效带宽。)

For some architectures, such as x86, Linux will "hide" any node representing a
physical cell that has no memory attached, and reassign any CPUs attached to
that cell to a node representing a cell that does have memory.  Thus, on
these architectures, one cannot assume that all CPUs that Linux associates with
a given node will see the same local memory access times and bandwidth.
(对于某些架构（例如 x86），Linux 会 "隐藏" 任何代表没有连接内存的物理
cell 的节点，并把连接到该 cell 的所有 CPU 重新分配给代表有内存的 cell 的
节点。因此，在这些架构上，不能假设 Linux 关联到给定节点的所有 CPU 都会
看到相同的本地内存访问时间和带宽。)

In addition, for some architectures, again x86 is an example, Linux supports
the emulation of additional nodes.  For NUMA emulation, linux will carve up
the existing nodes--or the system memory for non-NUMA platforms--into multiple
nodes.  Each emulated node will manage a fraction of the underlying cells'
physical memory.  NUMA emulation is useful for testing NUMA kernel and
application features on non-NUMA platforms, and as a sort of memory resource
management mechanism when used together with cpusets.
[see Documentation/admin-guide/cgroup-v1/cpusets.rst]
(此外，对于某些架构（x86 仍是一例），Linux 支持模拟额外的节点。在 NUMA
模拟中，Linux 会把现有节点（对于非 NUMA 平台则是系统内存）划分成多个
节点。每个模拟节点管理底层 cell 物理内存的一部分。NUMA 模拟可用于在非
NUMA 平台上测试 NUMA 内核与应用特性，并且在与 cpusets 配合使用时作为一种
内存资源管理机制。
[参见 Documentation/admin-guide/cgroup-v1/cpusets.rst])

For each node with memory, Linux constructs an independent memory management
subsystem, complete with its own free page lists, in-use page lists, usage
statistics and locks to mediate access.  In addition, Linux constructs for
each memory zone [one or more of DMA, DMA32, NORMAL, HIGH_MEMORY, MOVABLE],
an ordered "zonelist".  A zonelist specifies the zones/nodes to visit when a
selected zone/node cannot satisfy the allocation request.  This situation,
when a zone has no available memory to satisfy a request, is called
"overflow" or "fallback".
(对于每个带内存的节点，Linux 构建一个独立的内存管理子系统，它拥有自己的
空闲页链表、在用页链表、使用统计信息以及用于调解访问的锁。此外，Linux 为
每个内存 zone（DMA、DMA32、NORMAL、HIGH_MEMORY、MOVABLE 中的一个或多个）
构建一个有序的 "zonelist"（区域链表）。当所选 zone/node 无法满足分配请求
时，zonelist 指定了接下来要访问的 zone/node。当某个 zone 没有可用内存来
满足请求时，这种情况被称为 "overflow"（溢出）或 "fallback"（回退）。)

Because some nodes contain multiple zones containing different types of
memory, Linux must decide whether to order the zonelists such that allocations
fall back to the same zone type on a different node, or to a different zone
type on the same node.  This is an important consideration because some zones,
such as DMA or DMA32, represent relatively scarce resources.  Linux chooses
a default Node ordered zonelist. This means it tries to fallback to other zones
from the same node before using remote nodes which are ordered by NUMA distance.
(由于某些节点包含多个含有不同内存类型的 zone，Linux 必须决定 zonelist 的
排序方式：是让分配回退到不同节点上的同类型 zone，还是回退到同一节点上的
不同类型 zone。这是一个重要的考量，因为某些 zone（例如 DMA 或 DMA32）代表
相对稀缺的资源。Linux 选择默认的按节点排序的 zonelist。这意味着它会先尝试
回退到同一节点的其他 zone，然后才使用按 NUMA 距离排序的远程节点。)

By default, Linux will attempt to satisfy memory allocation requests from the
node to which the CPU that executes the request is assigned.  Specifically,
Linux will attempt to allocate from the first node in the appropriate zonelist
for the node where the request originates.  This is called "local allocation."
If the "local" node cannot satisfy the request, the kernel will examine other
nodes' zones in the selected zonelist looking for the first zone in the list
that can satisfy the request.
(默认情况下，Linux 会尝试从执行该请求的 CPU 所分配到的节点满足内存分配
请求。具体来说，Linux 会尝试从请求发起节点对应的适当 zonelist 中
的第一个节点分配。这称为 "本地分配 (local allocation)"。如果 "本地" 节点
无法满足请求，内核将在所选 zonelist 中检查其他节点的 zone，寻找链表中
第一个能满足请求的 zone。)

Local allocation will tend to keep subsequent access to the allocated memory
"local" to the underlying physical resources and off the system interconnect--
as long as the task on whose behalf the kernel allocated some memory does not
later migrate away from that memory.  The Linux scheduler is aware of the
NUMA topology of the platform--embodied in the "scheduling domains" data
structures [see Documentation/scheduler/sched-domains.rst]--and the scheduler
attempts to minimize task migration to distant scheduling domains.  However,
the scheduler does not take a task's NUMA footprint into account directly.
Thus, under sufficient imbalance, tasks can migrate between nodes, remote
from their initial node and kernel data structures.
(本地分配倾向于让对所分配内存的后续访问保持在底层物理资源的 "本地"，不
经过系统互连——只要内核为其分配内存的那个任务之后不迁移离开那块内存。
Linux 调度器知道平台的 NUMA 拓扑——体现在 "调度域 (scheduling domains)"
数据结构中 [参见 Documentation/scheduler/sched-domains.rst]——并且调度器
会尽量减少任务向远处调度域的迁移。然而，调度器并不直接考虑任务的 NUMA
足迹。因此，在足够的不平衡情况下，任务可能在与初始节点及内核数据结构相距
甚远的节点之间迁移。)

System administrators and application designers can restrict a task's migration
to improve NUMA locality using various CPU affinity command line interfaces,
such as taskset(1) and numactl(1), and program interfaces such as
sched_setaffinity(2).  Further, one can modify the kernel's default local
allocation behavior using Linux NUMA memory policy. [see
Documentation/admin-guide/mm/numa_memory_policy.rst].
(系统管理员和应用设计者可以使用各种 CPU 亲和性命令行接口（例如
taskset(1) 和 numactl(1)）以及程序接口（例如 sched_setaffinity(2)）来
限制任务的迁移，以改善 NUMA 局部性。此外，可以使用 Linux NUMA 内存策略
修改内核默认的本地分配行为。[参见
Documentation/admin-guide/mm/numa_memory_policy.rst])

System administrators can restrict the CPUs and nodes' memories that a non-
privileged user can specify in the scheduling or NUMA commands and functions
using control groups and CPUsets.  [see Documentation/admin-guide/cgroup-v1/cpusets.rst]
(系统管理员可以使用控制组和 CPUsets 限制非特权用户在调度或 NUMA 命令与
函数中可以指定的 CPU 和节点内存。[参见
Documentation/admin-guide/cgroup-v1/cpusets.rst])

On architectures that do not hide memoryless nodes, Linux will include only
zones [nodes] with memory in the zonelists.  This means that for a memoryless
node the "local memory node"--the node of the first zone in CPU's node's
zonelist--will not be the node itself.  Rather, it will be the node that the
kernel selected as the nearest node with memory when it built the zonelists.
So, default, local allocations will succeed with the kernel supplying the
closest available memory.  This is a consequence of the same mechanism that
allows such allocations to fallback to other nearby nodes when a node that
does contain memory overflows.
(在不隐藏无内存节点的架构上，Linux 只会把带内存的 zone（节点）包含在
zonelist 中。这意味着对于无内存节点，其 "本地内存节点"——CPU 所在节点的
zonelist 中第一个 zone 所属的节点——不会是该节点本身，而是内核在构建
zonelist 时选定的最近的有内存节点。因此，默认情况下，本地分配会由内核
提供最近的可用内存而成功。这也是允许这类分配在真正含有内存的节点溢出时
回退到附近其他节点的同一机制的结果。)

Some kernel allocations do not want or cannot tolerate this allocation fallback
behavior.  Rather they want to be sure they get memory from the specified node
or get notified that the node has no free memory.  This is usually the case when
a subsystem allocates per CPU memory resources, for example.
(一些内核分配不想要或无法容忍这种分配回退行为。它们希望确保从指定节点获得
内存，或者得到该节点没有空闲内存的通知。例如，当子系统分配 per CPU 内存
资源时通常就是这种情况。)

A typical model for making such an allocation is to obtain the node id of the
node to which the "current CPU" is attached using one of the kernel's
numa_node_id() or CPU_to_node() functions and then request memory from only
the node id returned.  When such an allocation fails, the requesting subsystem
may revert to its own fallback path.  The slab kernel memory allocator is an
example of this.  Or, the subsystem may choose to disable or not to enable
itself on allocation failure.  The kernel profiling subsystem is an example of
this.
(进行这类分配的典型模型是：使用内核的 numa_node_id() 或 CPU_to_node()
函数之一获取 "当前 CPU" 所连接节点的节点 id，然后只从返回的节点 id 请求
内存。当这种分配失败时，发起请求的子系统可以退回到自己的回退路径。slab
内核内存分配器就是一个例子。或者，子系统可以选择在分配失败时禁用或不
启用自身。内核性能剖析 (profiling) 子系统就是一个例子。)

If the architecture supports--does not hide--memoryless nodes, then CPUs
attached to memoryless nodes would always incur the fallback path overhead
or some subsystems would fail to initialize if they attempted to allocate
memory exclusively from a node without memory.  To support such
architectures transparently, kernel subsystems can use the numa_mem_id()
or cpu_to_mem() function to locate the "local memory node" for the calling or
specified CPU.  Again, this is the same node from which default, local page
allocations will be attempted.
(如果架构支持——不隐藏——无内存节点，那么连接到无内存节点的 CPU 将总是
承受回退路径的开销，或者某些子系统在试图仅从无内存节点分配内存时将无法
初始化。为了透明地支持这类架构，内核子系统可以使用 numa_mem_id() 或
cpu_to_mem() 函数来定位调用者或指定 CPU 的 "本地内存节点"。同样，这也是
默认的本地页分配将尝试使用的同一个节点。)
