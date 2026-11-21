.. SPDX-License-Identifier: GPL-2.0

:Author: Chris Li <chrisl@kernel.org>, Kairui Song <kasong@tencent.com>
(作者：Chris Li <chrisl@kernel.org>、Kairui Song <kasong@tencent.com>)

==================
Swap Table(交换表)
==================

Swap table implements swap cache as a per-cluster swap cache value array.
(Swap table 把交换缓存 (swap cache) 实现为每簇 (cluster) 一份的交换缓
存值数组。)

Swap Entry(交换项)
------------------

A swap entry contains the information required to serve the anonymous page
fault.
(一个交换项 (swap entry) 包含服务匿名页缺页所需的信息。)

Swap entry is encoded as two parts: swap type and swap offset.
(交换项编码为两个部分：swap type 与 swap offset。)

The swap type indicates which swap device to use.
The swap offset is the offset of the swap file to read the page data from.
(swap type 指示要使用哪个交换设备。swap offset 是交换文件中读取页面
数据的偏移量。)

Swap Cache(交换缓存)
--------------------

Swap cache is a map to look up folios using swap entry as the key. The result
value can have three possible types depending on which stage of this swap entry
was in.
(交换缓存是一个以交换项为键来查找 folio 的映射。根据该交换项所处的
阶段，结果值可能有三种类型。)

1. NULL: This swap entry is not used.
   (1. NULL：此交换项未被使用。)

2. folio: A folio has been allocated and bound to this swap entry. This is
   the transient state of swap out or swap in. The folio data can be in
   the folio or swap file, or both.
   (2. folio：一个 folio 已被分配并绑定到该交换项。这是 swap out 或
   swap in 期间的瞬态。folio 数据可能在 folio 中、交换文件中，或两者
   都有。)

3. shadow: The shadow contains the working set information of the swapped
   out folio. This is the normal state for a swapped out page.
   (3. shadow：shadow 包含被换出 folio 的工作集信息。这是被换出页面的
   正常状态。)

Swap Table Internals(Swap Table 内部实现)
-----------------------------------------

The previous swap cache is implemented by XArray. The XArray is a tree
structure. Each lookup will go through multiple nodes. Can we do better?
(以前的交换缓存由 XArray 实现。XArray 是一种树结构。每次查找都要经
过多个节点。我们能做得更好吗？)

Notice that most of the time when we look up the swap cache, we are either
in a swap in or swap out path. We should already have the swap cluster,
which contains the swap entry.
(注意，大多数时候当我们查找交换缓存时，要么正处于 swap in 路径，要
么正处于 swap out 路径。此时我们应当已经持有包含该交换项的交换簇
(swap cluster)。)

If we have a per-cluster array to store swap cache value in the cluster.
Swap cache lookup within the cluster can be a very simple array lookup.
(如果我们用一个每簇数组来在簇内存储交换缓存值，那么簇内的交换缓存
查找就可以是非常简单的数组查找。)

We give such a per-cluster swap cache value array a name: the swap table.
(我们给这种每簇交换缓存值数组起了一个名字：swap table。)

A swap table is an array of pointers. Each pointer is the same size as a
PTE. The size of a swap table for one swap cluster typically matches a PTE
page table, which is one page on modern 64-bit systems.
(swap table 是一个指针数组。每个指针与一个 PTE 大小相同。一个交换簇
的 swap table 的大小通常与一个 PTE 页表相当，即现代 64 位系统上的一
个页面。)

With swap table, swap cache lookup can achieve great locality, simpler,
and faster.
(有了 swap table，交换缓存查找可以获得极佳的局部性，更简单，也更
快。)

Locking(加锁规则)
-----------------

Swap table modification requires taking the cluster lock. If a folio
is being added to or removed from the swap table, the folio must be
locked prior to the cluster lock. After adding or removing is done, the
folio shall be unlocked.
(修改 swap table 需要获取簇锁。如果一个 folio 正被加入或移出 swap
table，则必须在获取簇锁之前先锁住该 folio。添加或移除完成后，folio
应被解锁。)

Swap table lookup is protected by RCU and atomic read. If the lookup
returns a folio, the user must lock the folio before use.
(swap table 查找由 RCU 与原子读保护。如果查找返回一个 folio，使用者
必须在使用之前锁住该 folio。)
