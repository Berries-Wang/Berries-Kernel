.. SPDX-License-Identifier: GPL-2.0

=======================
Multi-Gen LRU(多代 LRU)
=======================
The multi-gen LRU is an alternative LRU implementation that optimizes
page reclaim and improves performance under memory pressure. Page
reclaim decides the kernel's caching policy and ability to overcommit
memory. It directly impacts the kswapd CPU usage and RAM efficiency.
(多代 LRU (multi-gen LRU) 是另一种 LRU 实现，用于优化页回收，并在内存压力下
提升性能。页回收决定了内核的缓存策略和内存超售 (overcommit) 能力。它直接影响
kswapd 的 CPU 使用率和内存效率。)

Design overview(设计概览)
=========================
Objectives(目标)
----------------
The design objectives are:
(设计目标如下：)

* Good representation of access recency
  (良好地表示访问的新近程度。)
* Try to profit from spatial locality
  (尽量从空间局部性中获益。)
* Fast paths to make obvious choices
  (用快速路径完成显而易见的抉择。)
* Simple self-correcting heuristics
  (简单且能自我纠正的启发式规则。)

The representation of access recency is at the core of all LRU
implementations. In the multi-gen LRU, each generation represents a
group of pages with similar access recency. Generations establish a
(time-based) common frame of reference and therefore help make better
choices, e.g., between different memcgs on a computer or different
computers in a data center (for job scheduling).
(对访问新近程度的表示是所有 LRU 实现的核心。在多代 LRU 中，每一代
(generation) 代表一组访问新近程度相近的页。各代建立起一个(基于时间的)公共
参照系，因此有助于做出更好的选择，例如在一台计算机上不同 memcg 之间、或数据
中心内不同计算机之间(用于作业调度)的选择。)

Exploiting spatial locality improves efficiency when gathering the
accessed bit. A rmap walk targets a single page and does not try to
profit from discovering a young PTE. A page table walk can sweep all
the young PTEs in an address space, but the address space can be too
sparse to make a profit. The key is to optimize both methods and use
them in combination.
(利用空间局部性可以提高收集 accessed 位的效率。rmap 遍历只针对单个页，不会
试图借助发现 young PTE 的机会获利。页表遍历可以扫过地址空间中所有 young
PTE，但地址空间可能过于稀疏而得不偿失。关键在于把两种方法都优化好，并结合
使用。)

Fast paths reduce code complexity and runtime overhead. Unmapped pages
do not require TLB flushes; clean pages do not require writeback.
These facts are only helpful when other conditions, e.g., access
recency, are similar. With generations as a common frame of reference,
additional factors stand out. But obvious choices might not be good
choices; thus self-correction is necessary.
(快速路径降低了代码复杂度和运行时开销。未映射页不需要 TLB 刷新；干净页不需要
回写。这些事实只有在其他条件(例如访问新近程度)相似时才有帮助。以各代作为公共
参照系之后，其他因素便凸显出来。但显而易见的选择未必是好的选择，因此自我纠正
是必要的。)

The benefits of simple self-correcting heuristics are self-evident.
Again, with generations as a common frame of reference, this becomes
attainable. Specifically, pages in the same generation can be
categorized based on additional factors, and a feedback loop can
statistically compare the refault percentages across those categories
and infer which of them are better choices.
(简单且能自我纠正的启发式规则，其好处不言自明。同样，以各代作为公共参照系后，
这也变得可以实现。具体而言，同一代中的页可以按其他因素进一步分类，反馈回路则
统计比较各分类之间的 refault(重新缺页)百分比，从而推断哪些分类是更好的
选择。)

Assumptions(前提假设)
---------------------
The protection of hot pages and the selection of cold pages are based
on page access channels and patterns. There are two access channels:
(对热页的保护与对冷页的甄选基于页访问通道和访问模式。访问通道有两种：)

* Accesses through page tables
  (通过页表的访问。)
* Accesses through file descriptors
  (通过文件描述符的访问。)

The protection of the former channel is by design stronger because:
(前一种通道受到的保护在设计上更强，原因如下：)

1. The uncertainty in determining the access patterns of the former
   channel is higher due to the approximation of the accessed bit.
   (由于 accessed 位只是一种近似，判断前一种通道访问模式的不确定性更高。)
2. The cost of evicting the former channel is higher due to the TLB
   flushes required and the likelihood of encountering the dirty bit.
   (驱逐前一种通道的页代价更高，因为需要 TLB 刷新，而且遇到 dirty 位的可能
   性更大。)
3. The penalty of underprotecting the former channel is higher because
   applications usually do not prepare themselves for major page
   faults like they do for blocked I/O. E.g., GUI applications
   commonly use dedicated I/O threads to avoid blocking rendering
   threads.
   (对前一种通道保护不足的代价更高，因为应用程序通常不像为阻塞式 I/O 做准备
   那样为 major page fault 做准备。例如，GUI 应用程序通常使用专门的 I/O
   线程，以避免阻塞渲染线程。)

There are also two access patterns:
(此外还有两种访问模式：)

* Accesses exhibiting temporal locality
  (表现出时间局部性的访问。)
* Accesses not exhibiting temporal locality
  (不表现出时间局部性的访问。)

For the reasons listed above, the former channel is assumed to follow
the former pattern unless ``VM_SEQ_READ`` or ``VM_RAND_READ`` is
present, and the latter channel is assumed to follow the latter
pattern unless outlying refaults have been observed.
(基于上述原因，除非存在 ``VM_SEQ_READ`` 或 ``VM_RAND_READ``，否则假定前一种
通道遵循前一种模式；除非观察到离群的 refault，否则假定后一种通道遵循后一种
模式。)

Workflow overview(工作流概览)
=============================
Evictable pages are divided into multiple generations for each
``lruvec``. The youngest generation number is stored in
``lrugen->max_seq`` for both anon and file types as they are aged on
an equal footing. The oldest generation numbers are stored in
``lrugen->min_seq[]`` separately for anon and file types as clean file
pages can be evicted regardless of swap constraints. These three
variables are monotonically increasing.
(对于每个 ``lruvec``，可驱逐页被划分为多个代。最年轻代的代号存储在
``lrugen->max_seq`` 中，由 anon 和 file 两种类型共用，因为它们以同等地位老化。
最老代的代号则按 anon 与 file 类型分别存储在 ``lrugen->min_seq[]`` 中，因为
干净的文件页可以不受 swap 约束地被驱逐。这三个变量都单调递增。)

Generation numbers are truncated into ``order_base_2(MAX_NR_GENS+1)``
bits in order to fit into the gen counter in ``folio->flags``. Each
truncated generation number is an index to ``lrugen->folios[]``. The
sliding window technique is used to track at least ``MIN_NR_GENS`` and
at most ``MAX_NR_GENS`` generations. The gen counter stores a value
within ``[1, MAX_NR_GENS]`` while a page is on one of
``lrugen->folios[]``; otherwise it stores zero.
(代号被截断为 ``order_base_2(MAX_NR_GENS+1)`` 位，以便放入 ``folio->flags``
中的 gen 计数器。每个截断后的代号是 ``lrugen->folios[]`` 的下标。这里采用
滑动窗口技术，跟踪的代数最少 ``MIN_NR_GENS`` 个、最多 ``MAX_NR_GENS`` 个。
当页位于 ``lrugen->folios[]`` 中的某一代时，gen 计数器存储 ``[1,
MAX_NR_GENS]`` 范围内的值；否则存储零。)

Each generation is divided into multiple tiers. A page accessed ``N``
times through file descriptors is in tier ``order_base_2(N)``. Unlike
generations, tiers do not have dedicated ``lrugen->folios[]``. In
contrast to moving across generations, which requires the LRU lock,
moving across tiers only involves atomic operations on
``folio->flags`` and therefore has a negligible cost. A feedback loop
modeled after the PID controller monitors refaults over all the tiers
from anon and file types and decides which tiers from which types to
evict or protect. The desired effect is to balance refault percentages
between anon and file types proportional to the swappiness level.
(每一代又划分为多个层 (tier)。通过文件描述符被访问 ``N`` 次的页位于
``order_base_2(N)`` 层。与代不同，层没有专属的 ``lrugen->folios[]``。跨代
移动需要 LRU 锁；与之相对，跨层移动只涉及对 ``folio->flags`` 的原子操作，
因此开销可以忽略不计。一个仿照 PID 控制器建模的反馈回路监测 anon 与 file
两种类型在所有层上的 refault，并决定驱逐或保护哪些类型的哪些层。预期效果是
按 swappiness 水平成比例地平衡 anon 与 file 类型之间的 refault 百分比。)

There are two conceptually independent procedures: the aging and the
eviction. They form a closed-loop system, i.e., the page reclaim.
(有两个概念上相互独立的过程：老化 (aging) 与驱逐 (eviction)。它们构成一个
闭环系统，即页回收。)

Aging(老化)
-----------
The aging produces young generations. Given an ``lruvec``, it
increments ``max_seq`` when ``max_seq-min_seq+1`` approaches
``MIN_NR_GENS``. The aging promotes hot pages to the youngest
generation when it finds them accessed through page tables; the
demotion of cold pages happens consequently when it increments
``max_seq``. The aging uses page table walks and rmap walks to find
young PTEs. For the former, it iterates ``lruvec_memcg()->mm_list``
and calls ``walk_page_range()`` with each ``mm_struct`` on this list
to scan PTEs, and after each iteration, it increments ``max_seq``. For
the latter, when the eviction walks the rmap and finds a young PTE,
the aging scans the adjacent PTEs. For both, on finding a young PTE,
the aging clears the accessed bit and updates the gen counter of the
page mapped by this PTE to ``(max_seq%MAX_NR_GENS)+1``.
(老化过程产生年轻的代。给定一个 ``lruvec``，当 ``max_seq-min_seq+1`` 接近
``MIN_NR_GENS`` 时，它会递增 ``max_seq``。老化过程发现某些热页经由页表被访问
时，会把它们提升到最年轻的代；相应地，冷页的降级则随它递增 ``max_seq`` 而发生。
老化过程使用页表遍历和 rmap 遍历来寻找 young PTE。对于前者，它迭代
``lruvec_memcg()->mm_list``，并对该列表上的每个 ``mm_struct`` 调用
``walk_page_range()`` 来扫描 PTE，每轮迭代之后递增一次 ``max_seq``。对于
后者，当驱逐过程遍历 rmap 发现 young PTE 时，老化过程会扫描相邻的 PTE。对
两者而言，发现 young PTE 时，老化过程会清除 accessed 位，并把该 PTE 所映射
页的 gen 计数器更新为 ``(max_seq%MAX_NR_GENS)+1``。)

Eviction(驱逐)
--------------
The eviction consumes old generations. Given an ``lruvec``, it
increments ``min_seq`` when ``lrugen->folios[]`` indexed by
``min_seq%MAX_NR_GENS`` becomes empty. To select a type and a tier to
evict from, it first compares ``min_seq[]`` to select the older type.
If both types are equally old, it selects the one whose first tier has
a lower refault percentage. The first tier contains single-use
unmapped clean pages, which are the best bet. The eviction sorts a
page according to its gen counter if the aging has found this page
accessed through page tables and updated its gen counter. It also
moves a page to the next generation, i.e., ``min_seq+1``, if this page
was accessed multiple times through file descriptors and the feedback
loop has detected outlying refaults from the tier this page is in. To
this end, the feedback loop uses the first tier as the baseline, for
the reason stated earlier.
(驱逐过程消费旧的代。给定一个 ``lruvec``，当由 ``min_seq%MAX_NR_GENS`` 索引
的 ``lrugen->folios[]`` 变空时，它会递增 ``min_seq``。要选择从哪种类型、哪个
层驱逐，它首先比较 ``min_seq[]`` 以选出较旧的类型。若两种类型同样旧，则选择
第一层 refault 百分比较低的那一种。第一层包含一次性使用、未映射的干净页，它们
是最佳选择。如果老化过程发现某页经由页表被访问并更新了它的 gen 计数器，驱逐
过程会按其 gen 计数器对页排序。此外，若某页通过文件描述符被多次访问，且反馈
回路检测到该页所在层出现离群 refault，则该页会被移动到下一代，即
``min_seq+1``。为此，反馈回路以第一层作为基线，理由如前所述。)

Working set protection(工作集保护)
----------------------------------
Each generation is timestamped at birth. If ``lru_gen_min_ttl`` is
set, an ``lruvec`` is protected from the eviction when its oldest
generation was born within ``lru_gen_min_ttl`` milliseconds. In other
words, it prevents the working set of ``lru_gen_min_ttl`` milliseconds
from getting evicted. The OOM killer is triggered if this working set
cannot be kept in memory.
(每一代在诞生时都会被打上时间戳。如果设置了 ``lru_gen_min_ttl``，那么当某个
``lruvec`` 最老的代诞生于 ``lru_gen_min_ttl`` 毫秒以内时，该 ``lruvec`` 就会
受到保护而不被驱逐。换句话说，它可以防止 ``lru_gen_min_ttl`` 毫秒的工作集被
驱逐。如果这些工作集无法保留在内存中，就会触发 OOM killer。)

This time-based approach has the following advantages:
(这种基于时间的方法有以下优点：)

1. It is easier to configure because it is agnostic to applications
   and memory sizes.
   (它更容易配置，因为它与应用程序和内存大小无关。)
2. It is more reliable because it is directly wired to the OOM killer.
   (它更可靠，因为它直接接入 OOM killer。)

``mm_struct`` list(``mm_struct`` 链表)
--------------------------------------
An ``mm_struct`` list is maintained for each memcg, and an
``mm_struct`` follows its owner task to the new memcg when this task
is migrated.
(每个 memcg 都维护一个 ``mm_struct`` 链表；当任务被迁移时，``mm_struct`` 会
跟随其属主任务进入新的 memcg。)

A page table walker iterates ``lruvec_memcg()->mm_list`` and calls
``walk_page_range()`` with each ``mm_struct`` on this list to scan
PTEs. When multiple page table walkers iterate the same list, each of
them gets a unique ``mm_struct``, and therefore they can run in
parallel.
(页表遍历器迭代 ``lruvec_memcg()->mm_list``，并对该列表上的每个
``mm_struct`` 调用 ``walk_page_range()`` 来扫描 PTE。当多个页表遍历器迭代
同一列表时，每个遍历器都会拿到一个独一无二的 ``mm_struct``，因此它们可以并行
运行。)

Page table walkers ignore any misplaced pages, e.g., if an
``mm_struct`` was migrated, pages left in the previous memcg will be
ignored when the current memcg is under reclaim. Similarly, page table
walkers will ignore pages from nodes other than the one under reclaim.
(页表遍历器会忽略所有放错位置的页。例如，若某个 ``mm_struct`` 已被迁移，当
当前 memcg 正处于回收之下时，遗留在先前 memcg 中的页将被忽略。类似地，页表
遍历器也会忽略来自非回收节点的页。)

This infrastructure also tracks the usage of ``mm_struct`` between
context switches so that page table walkers can skip processes that
have been sleeping since the last iteration.
(这一基础设施还会跟踪上下文切换之间 ``mm_struct`` 的使用情况，使页表遍历器
能够跳过自上一轮以来一直睡眠的进程。)

Rmap/PT walk feedback(rmap/页表遍历反馈)
----------------------------------------
Searching the rmap for PTEs mapping each page on an LRU list (to test
and clear the accessed bit) can be expensive because pages from
different VMAs (PA space) are not cache friendly to the rmap (VA
space). For workloads mostly using mapped pages, searching the rmap
can incur the highest CPU cost in the reclaim path.
(在 rmap 中搜索映射 LRU 链表上每一页的 PTE(以测试并清除 accessed 位)可能
代价高昂，因为来自不同 VMA (PA 空间) 的页对 rmap (VA 空间) 不缓存友好。对于
主要使用映射页的工作负载，搜索 rmap 可能是回收路径中 CPU 开销最大的部分。)

``lru_gen_look_around()`` exploits spatial locality to reduce the
trips into the rmap. It scans the adjacent PTEs of a young PTE and
promotes hot pages. If the scan was done cacheline efficiently, it
adds the PMD entry pointing to the PTE table to the Bloom filter. This
forms a feedback loop between the eviction and the aging.
(``lru_gen_look_around()`` 利用空间局部性来减少进入 rmap 的次数。它会扫描
young PTE 相邻的 PTE 并提升热页。如果这次扫描在 cacheline 层面足够高效，它
会把指向该 PTE 表的 PMD 项加入布隆过滤器。这在驱逐与老化之间形成了一个反馈
回路。)

Bloom filters(布隆过滤器)
-------------------------
Bloom filters are a space and memory efficient data structure for set
membership test, i.e., test if an element is not in the set or may be
in the set.
(布隆过滤器是一种空间和内存高效的数据结构，用于集合成员测试，即测试某个元素
是不在集合中，还是可能在集合中。)

In the eviction path, specifically, in ``lru_gen_look_around()``, if a
PMD has a sufficient number of hot pages, its address is placed in the
filter. In the aging path, set membership means that the PTE range
will be scanned for young pages.
(在驱逐路径中，具体来说是在 ``lru_gen_look_around()`` 中，如果一个 PMD 拥有
足够多的热页，其地址就会被放入过滤器。在老化路径中，集合成员关系意味着该
PTE 范围将被扫描以寻找 young 页。)

Note that Bloom filters are probabilistic on set membership. If a test
is false positive, the cost is an additional scan of a range of PTEs,
which may yield hot pages anyway. Parameters of the filter itself can
control the false positive rate in the limit.
(注意，布隆过滤器对集合成员的判断是概率性的。如果一次测试出现假阳性，代价是
额外扫描一段 PTE 范围，而这次扫描反正也可能找到热页。过滤器自身的参数可以在
极限意义上控制假阳性率。)

PID controller(PID 控制器)
--------------------------
A feedback loop modeled after the Proportional-Integral-Derivative
(PID) controller monitors refaults over anon and file types and
decides which type to evict when both types are available from the
same generation.
(一个仿照比例-积分-微分 (PID) 控制器建模的反馈回路监测 anon 与 file 类型的
refault，并在同一代中两种类型都可用时决定驱逐哪一种类型。)

The PID controller uses generations rather than the wall clock as the
time domain because a CPU can scan pages at different rates under
varying memory pressure. It calculates a moving average for each new
generation to avoid being permanently locked in a suboptimal state.
(PID 控制器使用代而非墙上时钟作为时间域，因为在不同的内存压力下，CPU 扫描页
的速率会不同。它为每个新生代计算一个移动平均，以避免被永久锁定在次优状态。)

Memcg LRU(memcg 的 LRU)
-----------------------
An memcg LRU is a per-node LRU of memcgs. It is also an LRU of LRUs,
since each node and memcg combination has an LRU of folios (see
``mem_cgroup_lruvec()``). Its goal is to improve the scalability of
global reclaim, which is critical to system-wide memory overcommit in
data centers. Note that memcg LRU only applies to global reclaim.
(memcg LRU 是每个节点上 memcg 的 LRU。它也是 LRU 的 LRU，因为每种节点与
memcg 的组合都有一个 folio 的 LRU(见 ``mem_cgroup_lruvec()``)。它的目标是
提升全局回收的可扩展性，这对数据中心内系统级的内存超售至关重要。注意，
memcg LRU 只适用于全局回收。)

The basic structure of an memcg LRU can be understood by an analogy to
the active/inactive LRU (of folios):
(memcg LRU 的基本结构可以通过与 active/inactive LRU (folio 的 LRU) 类比来
理解：)

1. It has the young and the old (generations), i.e., the counterparts
   to the active and the inactive;
   (它有年轻与年老(各代)，即 active 与 inactive 的对应物；)
2. The increment of ``max_seq`` triggers promotion, i.e., the
   counterpart to activation;
   (``max_seq`` 的递增触发提升 (promotion)，即激活 (activation) 的对应物；)
3. Other events trigger similar operations, e.g., offlining an memcg
   triggers demotion, i.e., the counterpart to deactivation.
   (其他事件会触发类似的操作，例如 memcg 下线触发降级 (demotion)，即去激活
   (deactivation) 的对应物。)

In terms of global reclaim, it has two distinct features:
(就全局回收而言，它有两个鲜明的特性：)

1. Sharding, which allows each thread to start at a random memcg (in
   the old generation) and improves parallelism;
   (分片 (sharding)，允许每个线程从(旧代中的)随机 memcg 开始，提高并行度；)
2. Eventual fairness, which allows direct reclaim to bail out at will
   and reduces latency without affecting fairness over some time.
   (最终公平 (eventual fairness)，允许直接回收随时中途退出，从而在不影响一段
   时间跨度上公平性的前提下降低延迟。)

In terms of traversing memcgs during global reclaim, it improves the
best-case complexity from O(n) to O(1) and does not affect the
worst-case complexity O(n). Therefore, on average, it has a sublinear
complexity.
(就全局回收期间遍历 memcg 而言，它把最好情况的复杂度从 O(n) 提升到 O(1)，
同时不影响最坏情况复杂度 O(n)。因此，平均而言它具有亚线性复杂度。)

Summary(小结)
-------------
The multi-gen LRU (of folios) can be disassembled into the following
parts:
(多代 LRU (folio 的 LRU) 可以拆解为以下几个部分：)

* Generations
  (各代 (generations)。)
* Rmap walks
  (rmap 遍历。)
* Page table walks via ``mm_struct`` list
  (借助 ``mm_struct`` 链表的页表遍历。)
* Bloom filters for rmap/PT walk feedback
  (用于 rmap/页表遍历反馈的布隆过滤器。)
* PID controller for refault feedback
  (用于 refault 反馈的 PID 控制器。)

The aging and the eviction form a producer-consumer model;
specifically, the latter drives the former by the sliding window over
generations. Within the aging, rmap walks drive page table walks by
inserting hot densely populated page tables to the Bloom filters.
Within the eviction, the PID controller uses refaults as the feedback
to select types to evict and tiers to protect.
(老化与驱逐构成一个生产者-消费者模型；具体来说，驱逐通过跨越各代的滑动窗口
驱动老化。在老化内部，rmap 遍历通过把热而密集填充的页表插入布隆过滤器来驱动
页表遍历。在驱逐内部，PID 控制器使用 refault 作为反馈，选择要驱逐的类型和要
保护的层。)
