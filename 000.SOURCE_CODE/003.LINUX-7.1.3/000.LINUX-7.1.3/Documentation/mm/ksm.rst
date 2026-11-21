=====================================
Kernel Samepage Merging(内核同页合并)
=====================================

KSM is a memory-saving de-duplication feature, enabled by CONFIG_KSM=y,
added to the Linux kernel in 2.6.32.  See ``mm/ksm.c`` for its implementation,
and http://lwn.net/Articles/306704/ and https://lwn.net/Articles/330589/
(KSM 是一项节省内存的去重特性，由 CONFIG_KSM=y 启用，于 2.6.32 加入
Linux 内核。其实现参见 ``mm/ksm.c``，另见 http://lwn.net/Articles/306704/
与 https://lwn.net/Articles/330589/。)

The userspace interface of KSM is described in Documentation/admin-guide/mm/ksm.rst
(KSM 的用户空间接口在 Documentation/admin-guide/mm/ksm.rst 中描述。)

Design(设计)
============

Overview(概述)
--------------

.. kernel-doc:: mm/ksm.c
   :DOC: Overview

Reverse mapping(反向映射)
-------------------------
KSM maintains reverse mapping information for KSM pages in the stable
tree.
(KSM 在稳定树 (stable tree) 中维护 KSM 页面的反向映射信息。)

If a KSM page is shared between less than ``max_page_sharing`` VMAs,
the node of the stable tree that represents such KSM page points to a
list of struct ksm_rmap_item and the ``page->mapping`` of the
KSM page points to the stable tree node.
(如果一个 KSM 页面被少于 ``max_page_sharing`` 个 VMA 共享，则代表该
KSM 页面的稳定树节点指向一个 struct ksm_rmap_item 列表，而该 KSM 页面
的 ``page->mapping`` 指向稳定树节点。)

When the sharing passes this threshold, KSM adds a second dimension to
the stable tree. The tree node becomes a "chain" that links one or
more "dups". Each "dup" keeps reverse mapping information for a KSM
page with ``page->mapping`` pointing to that "dup".
(当共享数超过该阈值时，KSM 为稳定树增加第二个维度。树节点变成一条
"chain"，链接一个或多个 "dup"。每个 "dup" 保存一个 KSM 页面的反向映射
信息，其 ``page->mapping`` 指向该 "dup"。)

Every "chain" and all "dups" linked into a "chain" enforce the
invariant that they represent the same write protected memory content,
even if each "dup" will be pointed by a different KSM page copy of
that content.
(每条 "chain" 以及链接进某条 "chain" 的所有 "dup" 都强制一个不变式：
它们代表相同的写保护内存内容，即使每个 "dup" 将由该内容的一个不同的
KSM 页面副本所指向。)

This way the stable tree lookup computational complexity is unaffected
if compared to an unlimited list of reverse mappings. It is still
enforced that there cannot be KSM page content duplicates in the
stable tree itself.
(这样，与无限制的反向映射列表相比，稳定树查找的计算复杂度不受影响。
同时仍然强制保证稳定树本身中不能存在 KSM 页面内容的重复。)

The deduplication limit enforced by ``max_page_sharing`` is required
to avoid the virtual memory rmap lists to grow too large. The rmap
walk has O(N) complexity where N is the number of rmap_items
(i.e. virtual mappings) that are sharing the page, which is in turn
capped by ``max_page_sharing``. So this effectively spreads the linear
O(N) computational complexity from rmap walk context over different
KSM pages. The ksmd walk over the stable_node "chains" is also O(N),
but N is the number of stable_node "dups", not the number of
rmap_items, so it has not a significant impact on ksmd performance. In
practice the best stable_node "dup" candidate will be kept and found
at the head of the "dups" list.
( ``max_page_sharing`` 强制施加的去重上限是为了避免虚拟内存的 rmap
列表增长过大。rmap 遍历具有 O(N) 复杂度，其中 N 是共享该页面的
rmap_item 数量 (即虚拟映射数量)，而它又被 ``max_page_sharing`` 封顶。
因此，这实际上把 rmap 遍历上下文中的线性 O(N) 计算复杂度分散到了不同
的 KSM 页面上。ksmd 对 stable_node "chain" 的遍历也是 O(N)，但这里的
N 是 stable_node "dup" 的数量而不是 rmap_item 的数量，因此对 ksmd 的
性能没有显著影响。实践中，最优的 stable_node "dup" 候选会被保留，并
可在 "dups" 列表的头部找到。)

High values of ``max_page_sharing`` result in faster memory merging
(because there will be fewer stable_node dups queued into the
stable_node chain->hlist to check for pruning) and higher
deduplication factor at the expense of slower worst case for rmap
walks for any KSM page which can happen during swapping, compaction,
NUMA balancing and page migration.
(较高的 ``max_page_sharing`` 值会带来更快的内存合并 (因为排入
stable_node chain->hlist 以检查修剪的 stable_node dup 更少) 和更高的
去重因子，代价是任何 KSM 页面的 rmap 遍历最坏情况变慢——这可能发生在
交换、紧缩 (compaction)、NUMA 平衡和页面迁移期间。)

The ``stable_node_dups/stable_node_chains`` ratio is also affected by the
``max_page_sharing`` tunable, and an high ratio may indicate fragmentation
in the stable_node dups, which could be solved by introducing
fragmentation algorithms in ksmd which would refile rmap_items from
one stable_node dup to another stable_node dup, in order to free up
stable_node "dups" with few rmap_items in them, but that may increase
the ksmd CPU usage and possibly slowdown the readonly computations on
the KSM pages of the applications.
( ``stable_node_dups/stable_node_chains`` 比率也受 ``max_page_sharing``
可调参数的影响，比率过高可能表明 stable_node dup 中存在碎片化。这可以
通过在 ksmd 中引入碎片整理算法来解决，这些算法会把 rmap_item 从一个
stable_node dup 移到另一个 stable_node dup，以释放其中 rmap_item 很少
的 stable_node "dup"，但这可能会增加 ksmd 的 CPU 使用率，并可能拖慢对
应用程序 KSM 页面的只读计算。)

The whole list of stable_node "dups" linked in the stable_node
"chains" is scanned periodically in order to prune stale stable_nodes.
The frequency of such scans is defined by
``stable_node_chains_prune_millisecs`` sysfs tunable.
(链接在 stable_node "chain" 中的 stable_node "dup" 的整个列表会被周
期性扫描，以修剪过期的 stable_node。此类扫描的频率由
``stable_node_chains_prune_millisecs`` sysfs 可调参数定义。)

Reference(参考)
---------------
.. kernel-doc:: mm/ksm.c
   :functions: mm_slot ksm_scan stable_node rmap_item

--
Izik Eidus,
Hugh Dickins, 17 Nov 2009
(Izik Eidus、Hugh Dickins，2009 年 11 月 17 日。)
