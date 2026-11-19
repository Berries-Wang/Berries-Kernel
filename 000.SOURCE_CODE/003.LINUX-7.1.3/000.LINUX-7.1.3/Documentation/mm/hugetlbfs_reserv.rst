===============================
Hugetlbfs Reservation(巨页预留)
===============================

Overview(概述)
==============

Huge pages as described at Documentation/admin-guide/mm/hugetlbpage.rst are
typically preallocated for application use.  These huge pages are instantiated
in a task's address space at page fault time if the VMA indicates huge pages
are to be used.  If no huge page exists at page fault time, the task is sent
a SIGBUS and often dies an unhappy death.  Shortly after huge page support
was added, it was determined that it would be better to detect a shortage
of huge pages at mmap() time.  The idea is that if there were not enough
huge pages to cover the mapping, the mmap() would fail.  This was first
done with a simple check in the code at mmap() time to determine if there
were enough free huge pages to cover the mapping.  Like most things in the
kernel, the code has evolved over time.  However, the basic idea was to
'reserve' huge pages at mmap() time to ensure that huge pages would be
available for page faults in that mapping.  The description below attempts to
describe how huge page reserve processing is done in the v4.10 kernel.
(如 Documentation/admin-guide/mm/hugetlbpage.rst 所述，巨页通常会被预先分配
以供应用使用。如果 VMA 表明要使用巨页，这些巨页会在缺页异常发生时被实例化
到任务的地址空间中。如果缺页时不存在巨页，任务会收到 SIGBUS 信号，通常随
后便死亡。在巨页支持加入后不久，人们便认定最好在 mmap() 时就检测巨页短缺。
其设想是：如果没有足够的巨页来覆盖该映射，mmap() 就会失败。最初的做法只是
在 mmap() 时的代码中做一个简单检查，判断是否有足够的空闲巨页来覆盖映射。
与内核中的大多数事物一样，这段代码随时间不断演进。不过其基本思想是：在
mmap() 时"预留"巨页，以确保该映射中的缺页异常能有巨页可用。下文的描述试
图说明 v4.10 内核中巨页预留处理是如何完成的。)


Audience(读者对象)
==================
This description is primarily targeted at kernel developers who are modifying
hugetlbfs code.
(本文描述主要面向正在修改 hugetlbfs 代码的内核开发者。)


The Data Structures(数据结构)
=============================

resv_huge_pages
	This is a global (per-hstate) count of reserved huge pages.  Reserved
	huge pages are only available to the task which reserved them.
	Therefore, the number of huge pages generally available is computed
	as (``free_huge_pages - resv_huge_pages``).
	(resv_huge_pages 是保留巨页的全局(每 hstate)计数。保留巨页只对执
	行保留的 task 可用。因此，通常可用的巨页数量按 ``free_huge_pages -
	resv_huge_pages`` 计算。)
Reserve Map
	A reserve map is described by the structure::

		struct resv_map {
			struct kref refs;
			spinlock_t lock;
			struct list_head regions;
			long adds_in_progress;
			struct list_head region_cache;
			long region_cache_count;
		};

	(预留映射 (reserve map) 由如下结构描述：)

	There is one reserve map for each huge page mapping in the system.
	The regions list within the resv_map describes the regions within
	the mapping.  A region is described as::

		struct file_region {
			struct list_head link;
			long from;
			long to;
		};

	(系统中每个巨页映射都有一个预留映射。resv_map 中的 regions 链表
	描述该映射内的各个区域。区域由如下结构描述：)

	The 'from' and 'to' fields of the file region structure are huge page
	indices into the mapping.  Depending on the type of mapping, a
	region in the reserv_map may indicate reservations exist for the
	range, or reservations do not exist.
	(file_region 结构的 'from' 和 'to' 字段是映射中的巨页索引。根据映
	射类型的不同，reserv_map 中的一个区域可能表示该范围存在预留，也
	可能表示不存在预留。)
Flags for MAP_PRIVATE Reservations
	These are stored in the bottom bits of the reservation map pointer.
	(这些标志存储在预留映射指针的低位比特中。)

	``#define HPAGE_RESV_OWNER    (1UL << 0)``
		Indicates this task is the owner of the reservations
		associated with the mapping.
		(表示该 task 是与该映射关联的预留的拥有者。)
	``#define HPAGE_RESV_UNMAPPED (1UL << 1)``
		Indicates task originally mapping this range (and creating
		reserves) has unmapped a page from this task (the child)
		due to a failed COW.
		(表示最初映射该范围(并创建预留)的 task 因 COW 失败而从这个
		task(子进程)解除映射了一个页面。)
Page Flags
	The PagePrivate page flag is used to indicate that a huge page
	reservation must be restored when the huge page is freed.  More
	details will be discussed in the "Freeing huge pages" section.
	(PagePrivate 页面标志用于表示释放该巨页时必须恢复其巨页预留。更
	多细节将在 "Freeing huge pages" 一节中讨论。)


Reservation Map Location (Private or Shared)(预留映射的位置(私有或共享))
========================================================================

A huge page mapping or segment is either private or shared.  If private,
it is typically only available to a single address space (task).  If shared,
it can be mapped into multiple address spaces (tasks).  The location and
semantics of the reservation map is significantly different for the two types
of mappings.  Location differences are:
(巨页映射或内存段要么是私有的，要么是共享的。如果是私有的，它通常只对单个
地址空间(task)可用；如果是共享的，则可以映射进多个地址空间(task)。这两种
映射类型的预留映射在位置和语义上都有显著差异。位置上的差异如下：)

- For private mappings, the reservation map hangs off the VMA structure.
  Specifically, vma->vm_private_data.  This reserve map is created at the
  time the mapping (mmap(MAP_PRIVATE)) is created.
  (对于私有映射，预留映射挂在 VMA 结构上。具体来说，就是
  vma->vm_private_data。该预留映射在创建映射(mmap(MAP_PRIVATE))时创建。)
- For shared mappings, the reservation map hangs off the inode.  Specifically,
  inode->i_mapping->private_data.  Since shared mappings are always backed
  by files in the hugetlbfs filesystem, the hugetlbfs code ensures each inode
  contains a reservation map.  As a result, the reservation map is allocated
  when the inode is created.
  (对于共享映射，预留映射挂在 inode 上。具体来说，就是
  inode->i_mapping->private_data。由于共享映射总是由 hugetlbfs 文件系统中
  的文件作后备，hugetlbfs 代码会确保每个 inode 都包含一个预留映射。因此，
  预留映射在 inode 创建时分配。)


Creating Reservations(创建预留)
===============================
Reservations are created when a huge page backed shared memory segment is
created (shmget(SHM_HUGETLB)) or a mapping is created via mmap(MAP_HUGETLB).
These operations result in a call to the routine hugetlb_reserve_pages()::

	int hugetlb_reserve_pages(struct inode *inode,
				  long from, long to,
				  struct vm_area_struct *vma,
				  vm_flags_t vm_flags)

(当创建由巨页作后备的共享内存段(shmget(SHM_HUGETLB))，或通过
mmap(MAP_HUGETLB) 创建映射时，就会创建预留。这些操作会导致对例程
hugetlb_reserve_pages() 的调用：)

The first thing hugetlb_reserve_pages() does is check if the NORESERVE
flag was specified in either the shmget() or mmap() call.  If NORESERVE
was specified, then this routine returns immediately as no reservations
are desired.
(hugetlb_reserve_pages() 做的第一件事是检查 shmget() 或 mmap() 调用中
是否指定了 NORESERVE 标志。如果指定了 NORESERVE，由于不需要预留，该
例程会立即返回。)

The arguments 'from' and 'to' are huge page indices into the mapping or
underlying file.  For shmget(), 'from' is always 0 and 'to' corresponds to
the length of the segment/mapping.  For mmap(), the offset argument could
be used to specify the offset into the underlying file.  In such a case,
the 'from' and 'to' arguments have been adjusted by this offset.
(参数 'from' 和 'to' 是映射或底层文件中的巨页索引。对于 shmget()，
'from' 总是 0，'to' 对应段/映射的长度。对于 mmap()，offset 参数可用于
指定底层文件内的偏移。在这种情况下，'from' 和 'to' 参数已经按该偏移
做了调整。)

One of the big differences between PRIVATE and SHARED mappings is the way
in which reservations are represented in the reservation map.
(PRIVATE 与 SHARED 映射之间的一大差异是预留映射中预留的表示方式。)

- For shared mappings, an entry in the reservation map indicates a reservation
  exists or did exist for the corresponding page.  As reservations are
  consumed, the reservation map is not modified.
  (对于共享映射，预留映射中的一个条目表示对应页面存在或曾经存在预留。
  随着预留被消耗，预留映射不会被修改。)
- For private mappings, the lack of an entry in the reservation map indicates
  a reservation exists for the corresponding page.  As reservations are
  consumed, entries are added to the reservation map.  Therefore, the
  reservation map can also be used to determine which reservations have
  been consumed.
  (对于私有映射，预留映射中缺少条目表示对应页面存在预留。随着预留被
  消耗，条目会被加入预留映射。因此，预留映射还可用于确定哪些预留已被
  消耗。)

For private mappings, hugetlb_reserve_pages() creates the reservation map and
hangs it off the VMA structure.  In addition, the HPAGE_RESV_OWNER flag is set
to indicate this VMA owns the reservations.
(对于私有映射，hugetlb_reserve_pages() 创建预留映射并把它挂在 VMA 结构
上。此外，还会设置 HPAGE_RESV_OWNER 标志，以表明该 VMA 拥有这些预留。)

The reservation map is consulted to determine how many huge page reservations
are needed for the current mapping/segment.  For private mappings, this is
always the value (to - from).  However, for shared mappings it is possible that
some reservations may already exist within the range (to - from).  See the
section :ref:`Reservation Map Modifications <resv_map_modifications>`
for details on how this is accomplished.
(查询预留映射以确定当前映射/段需要多少巨页预留。对于私有映射，该值总是
(to - from)。而对于共享映射，(to - from) 范围内可能已经存在一些预留。具体
如何完成，参见 :ref:`Reservation Map Modifications(预留映射的修改)
<resv_map_modifications>` 一节。)

The mapping may be associated with a subpool.  If so, the subpool is consulted
to ensure there is sufficient space for the mapping.  It is possible that the
subpool has set aside reservations that can be used for the mapping.  See the
section :ref:`Subpool Reservations <sub_pool_resv>` for more details.
(该映射可能关联一个子池。如果是这样，会查询该子池以确保映射有足够空间。
子池可能已经预留了可供该映射使用的预留。更多细节参见
:ref:`Subpool Reservations(子池预留) <sub_pool_resv>` 一节。)

After consulting the reservation map and subpool, the number of needed new
reservations is known.  The routine hugetlb_acct_memory() is called to check
for and take the requested number of reservations.  hugetlb_acct_memory()
calls into routines that potentially allocate and adjust surplus page counts.
However, within those routines the code is simply checking to ensure there
are enough free huge pages to accommodate the reservation.  If there are,
the global reservation count resv_huge_pages is adjusted something like the
following::

	if (resv_needed <= (free_huge_pages - resv_huge_pages)
		resv_huge_pages += resv_needed;

(查询预留映射和子池之后，所需新预留的数量就确定了。接着调用例程
hugetlb_acct_memory() 来检查并取得所请求数量的预留。
hugetlb_acct_memory() 会调用一些可能分配并调整盈余 (surplus) 页面计数的
例程。不过在这些例程内部，代码只是检查是否有足够的空闲巨页来满足预留。
如果有，全局预留计数 resv_huge_pages 会大致按如下方式调整：)

Note that the global lock hugetlb_lock is held when checking and adjusting
these counters.
(注意，检查和调整这些计数器时会持有全局锁 hugetlb_lock。)

If there were enough free huge pages and the global count resv_huge_pages
was adjusted, then the reservation map associated with the mapping is
modified to reflect the reservations.  In the case of a shared mapping, a
file_region will exist that includes the range 'from' - 'to'.  For private
mappings, no modifications are made to the reservation map as lack of an
entry indicates a reservation exists.
(如果有足够的空闲巨页并且全局计数 resv_huge_pages 已被调整，那么与该
映射关联的预留映射会被修改以反映这些预留。对于共享映射，会存在一个包
含 'from' - 'to' 范围的 file_region。对于私有映射，预留映射不做任何修改，
因为缺少条目即表示存在预留。)

If hugetlb_reserve_pages() was successful, the global reservation count and
reservation map associated with the mapping will be modified as required to
ensure reservations exist for the range 'from' - 'to'.
(如果 hugetlb_reserve_pages() 成功，全局预留计数和与映射关联的预留映射
都会按要求被修改，以确保 'from' - 'to' 范围存在预留。)

.. _consume_resv:

Consuming Reservations/Allocating a Huge Page(消费预留/分配巨页)
================================================================

Reservations are consumed when huge pages associated with the reservations
are allocated and instantiated in the corresponding mapping.  The allocation
is performed within the routine alloc_hugetlb_folio()::

	struct folio *alloc_hugetlb_folio(struct vm_area_struct *vma,
				     unsigned long addr, int avoid_reserve)

(当与预留关联的巨页被分配并实例化到相应映射中时，预留即被消费。分配是在
例程 alloc_hugetlb_folio() 内完成的：)

alloc_hugetlb_folio is passed a VMA pointer and a virtual address, so it can
consult the reservation map to determine if a reservation exists.  In addition,
alloc_hugetlb_folio takes the argument avoid_reserve which indicates reserves
should not be used even if it appears they have been set aside for the
specified address.  The avoid_reserve argument is most often used in the case
of Copy on Write and Page Migration where additional copies of an existing
page are being allocated.
(alloc_hugetlb_folio 被传入一个 VMA 指针和一个虚拟地址，因此它可以查询
预留映射以确定预留是否存在。此外，alloc_hugetlb_folio 带有 avoid_reserve
参数，它表明即使预留看起来已为指定地址留出，也不应使用。avoid_reserve
参数最常用于写时复制 (Copy on Write) 和页面迁移 (Page Migration) 场合，
此时正在为已有页面分配额外副本。)

The helper routine vma_needs_reservation() is called to determine if a
reservation exists for the address within the mapping(vma).  See the section
:ref:`Reservation Map Helper Routines <resv_map_helpers>` for detailed
information on what this routine does.
The value returned from vma_needs_reservation() is generally
0 or 1.  0 if a reservation exists for the address, 1 if no reservation exists.
If a reservation does not exist, and there is a subpool associated with the
mapping the subpool is consulted to determine if it contains reservations.
If the subpool contains reservations, one can be used for this allocation.
However, in every case the avoid_reserve argument overrides the use of
a reservation for the allocation.  After determining whether a reservation
exists and can be used for the allocation, the routine dequeue_huge_page_vma()
is called.  This routine takes two arguments related to reservations:
(调用辅助例程 vma_needs_reservation() 来确定映射(vma)中该地址是否存在预
留。该例程做什么的详细信息参见 :ref:`Reservation Map Helper Routines(预
留映射辅助例程) <resv_map_helpers>` 一节。vma_needs_reservation() 返回
的值一般为 0 或 1：0 表示该地址存在预留，1 表示不存在。如果不存在预留，
且映射关联了子池，则会查询子池以确定它是否包含预留。如果子池包含预留，
其中之一便可用于本次分配。但在任何情况下，avoid_reserve 参数都会优先于
预留的使用。在确定预留是否存在以及能否用于分配之后，会调用例程
dequeue_huge_page_vma()。该例程接受两个与预留相关的参数：)

- avoid_reserve, this is the same value/argument passed to
  alloc_hugetlb_folio().
  (avoid_reserve，与传给 alloc_hugetlb_folio() 的值/参数相同。)
- chg, even though this argument is of type long only the values 0 or 1 are
  passed to dequeue_huge_page_vma.  If the value is 0, it indicates a
  reservation exists (see the section "Memory Policy and Reservations" for
  possible issues).  If the value is 1, it indicates a reservation does not
  exist and the page must be taken from the global free pool if possible.
  (chg，虽然该参数类型为 long，但传给 dequeue_huge_page_vma 的值只有 0
  或 1。值为 0 表示存在预留(可能的问题参见 "Memory Policy and
  Reservations" 一节)；值为 1 表示不存在预留，页面必须(如果可能)取自全
  局空闲池。)

The free lists associated with the memory policy of the VMA are searched for
a free page.  If a page is found, the value free_huge_pages is decremented
when the page is removed from the free list.  If there was a reservation
associated with the page, the following adjustments are made::

	SetPagePrivate(page);	/* Indicates allocating this page consumed
				 * a reservation, and if an error is
				 * encountered such that the page must be
				 * freed, the reservation will be restored. */
	resv_huge_pages--;	/* Decrement the global reservation count */

(在与该 VMA 内存策略关联的空闲链表中搜索空闲页面。如果找到页面，页面从
空闲链表移除时 free_huge_pages 递减。如果页面关联了预留，则做如下调整：)

Note, if no huge page can be found that satisfies the VMA's memory policy
an attempt will be made to allocate one using the buddy allocator.  This
brings up the issue of surplus huge pages and overcommit which is beyond
the scope reservations.  Even if a surplus page is allocated, the same
reservation based adjustments as above will be made: SetPagePrivate(page) and
resv_huge_pages--.
(注意，如果找不到满足该 VMA 内存策略的巨页，会尝试用伙伴分配器 (buddy
allocator) 分配一个。这就引出了盈余巨页 (surplus huge pages) 与超额提交
(overcommit) 的问题，它们超出了预留的讨论范围。即使分配的是盈余页，也会
做与上述相同的基于预留的调整：SetPagePrivate(page) 和 resv_huge_pages--。)

After obtaining a new hugetlb folio, (folio)->_hugetlb_subpool is set to the
value of the subpool associated with the page if it exists.  This will be used
for subpool accounting when the folio is freed.
(获得新 hugetlb folio 后，如果存在与页面关联的子池，(folio)->
_hugetlb_subpool 会被设置为该子池的值。它将在 folio 释放时用于子池记账。)

The routine vma_commit_reservation() is then called to adjust the reserve
map based on the consumption of the reservation.  In general, this involves
ensuring the page is represented within a file_region structure of the region
map.  For shared mappings where the reservation was present, an entry
in the reserve map already existed so no change is made.  However, if there
was no reservation in a shared mapping or this was a private mapping a new
entry must be created.
(随后调用例程 vma_commit_reservation()，根据预留的消费情况调整预留映射。
一般来说，这涉及确保页面在区域映射的某个 file_region 结构中有相应表示。
对于预留原本就存在的共享映射，预留映射中已有条目，因此不做更改；而对于
没有预留的共享映射或私有映射，则必须创建新条目。)

It is possible that the reserve map could have been changed between the call
to vma_needs_reservation() at the beginning of alloc_hugetlb_folio() and the
call to vma_commit_reservation() after the folio was allocated.  This would
be possible if hugetlb_reserve_pages was called for the same page in a shared
mapping.  In such cases, the reservation count and subpool free page count
will be off by one.  This rare condition can be identified by comparing the
return value from vma_needs_reservation and vma_commit_reservation.  If such
a race is detected, the subpool and global reserve counts are adjusted to
compensate.  See the section
:ref:`Reservation Map Helper Routines <resv_map_helpers>` for more
information on these routines.
(在 alloc_hugetlb_folio() 开头调用 vma_needs_reservation() 与 folio 分配完
成后调用 vma_commit_reservation() 之间，预留映射可能已被改变。如果共享
映射中同一页面调用了 hugetlb_reserve_pages，就会出现这种情况。此时，预留
计数和子池空闲页计数会偏差 1。通过比较 vma_needs_reservation 与
vma_commit_reservation 的返回值可以识别这种罕见情况。如果检测到这样的竞
争，会调整子池和全局预留计数加以补偿。关于这些例程的更多信息参见
:ref:`Reservation Map Helper Routines(预留映射辅助例程) <resv_map_helpers>`
一节。)


Instantiate Huge Pages(实例化巨页)
==================================

After huge page allocation, the page is typically added to the page tables
of the allocating task.  Before this, pages in a shared mapping are added
to the page cache and pages in private mappings are added to an anonymous
reverse mapping.  In both cases, the PagePrivate flag is cleared.  Therefore,
when a huge page that has been instantiated is freed no adjustment is made
to the global reservation count (resv_huge_pages).
(巨页分配完成后，页面通常会被加入分配它的任务的页表。在此之前，共享映射
中的页面会被加入页面缓存 (page cache)，私有映射中的页面会被加入匿名反向
映射。两种情况下 PagePrivate 标志都会被清除。因此，已实例化的巨页被释放
时，全局预留计数 (resv_huge_pages) 不会做任何调整。)


Freeing Huge Pages(释放巨页)
============================

Huge pages are freed by free_huge_folio().  It is only passed a pointer
to the folio as it is called from the generic MM code.  When a huge page
is freed, reservation accounting may need to be performed.  This would
be the case if the page was associated with a subpool that contained
reserves, or the page is being freed on an error path where a global
reserve count must be restored.
(巨页由 free_huge_folio() 释放。由于它是从通用 MM 代码中调用的，所以只
传给它一个指向 folio 的指针。释放巨页时，可能需要做预留记账。如果页面
关联了包含预留的子池，或者页面正在错误路径上被释放而必须恢复全局预留
计数，就属于这种情况。)

The page->private field points to any subpool associated with the page.
If the PagePrivate flag is set, it indicates the global reserve count should
be adjusted (see the section
:ref:`Consuming Reservations/Allocating a Huge Page <consume_resv>`
for information on how these are set).
(page->private 字段指向与页面关联的子池(如果有)。如果设置了 PagePrivate
标志，则表明应调整全局预留计数(这些标志如何设置的说明参见
:ref:`Consuming Reservations/Allocating a Huge Page(消费预留/分配巨页)
<consume_resv>` 一节)。)

The routine first calls hugepage_subpool_put_pages() for the page.  If this
routine returns a value of 0 (which does not equal the value passed 1) it
indicates reserves are associated with the subpool, and this newly free page
must be used to keep the number of subpool reserves above the minimum size.
Therefore, the global resv_huge_pages counter is incremented in this case.
(该例程首先对页面调用 hugepage_subpool_put_pages()。如果该例程返回 0
(与传入的值 1 不相等)，则表明子池关联了预留，必须用这个新释放的页面使
子池预留数量保持在最小规模之上。因此在这种情况下，全局 resv_huge_pages
计数器递增。)

If the PagePrivate flag was set in the page, the global resv_huge_pages counter
will always be incremented.
(如果页面中设置了 PagePrivate 标志，全局 resv_huge_pages 计数器总是会递
增。)

.. _sub_pool_resv:

Subpool Reservations(子池预留)
==============================

There is a struct hstate associated with each huge page size.  The hstate
tracks all huge pages of the specified size.  A subpool represents a subset
of pages within a hstate that is associated with a mounted hugetlbfs
filesystem.
(每个巨页大小都有一个关联的 struct hstate。hstate 跟踪指定大小的所有巨
页。子池表示 hstate 内与某个已挂载 hugetlbfs 文件系统关联的页面子集。)

When a hugetlbfs filesystem is mounted a min_size option can be specified
which indicates the minimum number of huge pages required by the filesystem.
If this option is specified, the number of huge pages corresponding to
min_size are reserved for use by the filesystem.  This number is tracked in
the min_hpages field of a struct hugepage_subpool.  At mount time,
hugetlb_acct_memory(min_hpages) is called to reserve the specified number of
huge pages.  If they can not be reserved, the mount fails.
(挂载 hugetlbfs 文件系统时可以指定 min_size 选项，它表示该文件系统所需
的最少巨页数。如果指定了该选项，与 min_size 对应数量的巨页会被预留供该
文件系统使用。这个数量记录在 struct hugepage_subpool 的 min_hpages 字段
中。挂载时会调用 hugetlb_acct_memory(min_hpages) 来预留指定数量的巨页。
如果无法预留，挂载失败。)

The routines hugepage_subpool_get/put_pages() are called when pages are
obtained from or released back to a subpool.  They perform all subpool
accounting, and track any reservations associated with the subpool.
hugepage_subpool_get/put_pages are passed the number of huge pages by which
to adjust the subpool 'used page' count (down for get, up for put).  Normally,
they return the same value that was passed or an error if not enough pages
exist in the subpool.
(从子池获取页面或把页面释放回子池时会调用 hugepage_subpool_get/put_pages()
例程。它们执行所有子池记账，并跟踪与子池关联的任何预留。
hugepage_subpool_get/put_pages 被传入用于调整子池"已用页面"计数的巨页数
量(get 时向下调，put 时向上调)。正常情况下，它们返回传入的相同值；如果
子池中没有足够的页面则返回错误。)

However, if reserves are associated with the subpool a return value less
than the passed value may be returned.  This return value indicates the
number of additional global pool adjustments which must be made.  For example,
suppose a subpool contains 3 reserved huge pages and someone asks for 5.
The 3 reserved pages associated with the subpool can be used to satisfy part
of the request.  But, 2 pages must be obtained from the global pools.  To
relay this information to the caller, the value 2 is returned.  The caller
is then responsible for attempting to obtain the additional two pages from
the global pools.
(不过，如果子池关联了预留，返回值可能小于传入值。该返回值表示还必须做
多少额外的全局池调整。举例来说，假设子池包含 3 个保留巨页，而有人请求
5 个。与子池关联的这 3 个保留页可用于满足部分请求，但另外 2 页必须从全
局池获取。为了把这个信息传递给调用者，返回值 2。调用者随后负责尝试从全
局池获取额外的两页。)


COW and Reservations(COW 与预留)
================================

Since shared mappings all point to and use the same underlying pages, the
biggest reservation concern for COW is private mappings.  In this case,
two tasks can be pointing at the same previously allocated page.  One task
attempts to write to the page, so a new page must be allocated so that each
task points to its own page.
(由于共享映射都指向并使用相同的底层页面，COW 最大的预留隐患在于私有映
射。这种情况下，两个 task 可能指向同一个先前分配的页面。其中一个 task
尝试写该页面，因此必须分配一个新页面，使每个 task 指向自己的页面。)

When the page was originally allocated, the reservation for that page was
consumed.  When an attempt to allocate a new page is made as a result of
COW, it is possible that no free huge pages are free and the allocation
will fail.
(页面最初分配时，其预留已被消费。当因 COW 而尝试分配新页面时，可能出现
没有空闲巨页的情况，分配将会失败。)

When the private mapping was originally created, the owner of the mapping
was noted by setting the HPAGE_RESV_OWNER bit in the pointer to the reservation
map of the owner.  Since the owner created the mapping, the owner owns all
the reservations associated with the mapping.  Therefore, when a write fault
occurs and there is no page available, different action is taken for the owner
and non-owner of the reservation.
(私有映射最初创建时，通过在拥有者的预留映射指针中设置 HPAGE_RESV_OWNER
位来记录映射的拥有者。既然拥有者创建了这个映射，拥有者就拥有与该映射
关联的所有预留。因此，当发生写缺页异常而没有页面可用时，对预留的拥有
者和非拥有者会采取不同的处理。)

In the case where the faulting task is not the owner, the fault will fail and
the task will typically receive a SIGBUS.
(如果发生缺页的 task 不是拥有者，缺页处理将失败，该 task 通常会收到
SIGBUS。)

If the owner is the faulting task, we want it to succeed since it owned the
original reservation.  To accomplish this, the page is unmapped from the
non-owning task.  In this way, the only reference is from the owning task.
In addition, the HPAGE_RESV_UNMAPPED bit is set in the reservation map pointer
of the non-owning task.  The non-owning task may receive a SIGBUS if it later
faults on a non-present page.  But, the original owner of the
mapping/reservation will behave as expected.
(如果发生缺页的 task 是拥有者，我们希望它成功，因为它拥有最初的预留。
为此，该页面会从非拥有者的 task 中解除映射。这样，唯一的引用就来自拥有
者的 task。此外，非拥有者 task 的预留映射指针中会设置 HPAGE_RESV_UNMAPPED
位。非拥有者 task 之后若对不存在的页面发生缺页，可能会收到 SIGBUS。但
映射/预留的最初拥有者将按预期工作。)


.. _resv_map_modifications:

Reservation Map Modifications(预留映射的修改)
=============================================

The following low level routines are used to make modifications to a
reservation map.  Typically, these routines are not called directly.  Rather,
a reservation map helper routine is called which calls one of these low level
routines.  These low level routines are fairly well documented in the source
code (mm/hugetlb.c).  These routines are::

	long region_chg(struct resv_map *resv, long f, long t);
	long region_add(struct resv_map *resv, long f, long t);
	void region_abort(struct resv_map *resv, long f, long t);
	long region_count(struct resv_map *resv, long f, long t);

(以下低层例程用于对预留映射做修改。通常不会直接调用这些例程，而是调用
某个预留映射辅助例程，由它调用这些低层例程之一。这些低层例程在源代码
(mm/hugetlb.c) 中有相当完善的文档。这些例程是：)

Operations on the reservation map typically involve two operations:
(对预留映射的操作通常涉及两步操作：)

1) region_chg() is called to examine the reserve map and determine how
   many pages in the specified range [f, t) are NOT currently represented.
   (调用 region_chg() 检查预留映射，确定指定范围 [f, t) 内当前有多少页面
   尚未在映射中表示。)

   The calling code performs global checks and allocations to determine if
   there are enough huge pages for the operation to succeed.
   (调用代码执行全局检查和分配，以确定是否有足够的巨页使操作成功。)

2)
  a) If the operation can succeed, region_add() is called to actually modify
     the reservation map for the same range [f, t) previously passed to
     region_chg().
     (如果操作可以成功，则调用 region_add() 实际修改预留映射中先前传给
     region_chg() 的同一范围 [f, t)。)
  b) If the operation can not succeed, region_abort is called for the same
     range [f, t) to abort the operation.
     (如果操作无法成功，则对同一范围 [f, t) 调用 region_abort 以中止操
     作。)

Note that this is a two step process where region_add() and region_abort()
are guaranteed to succeed after a prior call to region_chg() for the same
range.  region_chg() is responsible for pre-allocating any data structures
necessary to ensure the subsequent operations (specifically region_add()))
will succeed.
(注意这是一个两步过程：对同一范围先调用 region_chg() 之后，region_add()
和 region_abort() 保证会成功。region_chg() 负责预分配必要的数据结构，以
确保后续操作(特别是 region_add())能够成功。)

As mentioned above, region_chg() determines the number of pages in the range
which are NOT currently represented in the map.  This number is returned to
the caller.  region_add() returns the number of pages in the range added to
the map.  In most cases, the return value of region_add() is the same as the
return value of region_chg().  However, in the case of shared mappings it is
possible for changes to the reservation map to be made between the calls to
region_chg() and region_add().  In this case, the return value of region_add()
will not match the return value of region_chg().  It is likely that in such
cases global counts and subpool accounting will be incorrect and in need of
adjustment.  It is the responsibility of the caller to check for this condition
and make the appropriate adjustments.
(如上所述，region_chg() 确定范围内当前尚未在映射中表示的页面数，该数值
返回给调用者。region_add() 返回该范围内加入映射的页面数。大多数情况下，
region_add() 的返回值与 region_chg() 相同。但对于共享映射，在
region_chg() 和 region_add() 调用之间预留映射可能被修改。此时
region_add() 的返回值将与 region_chg() 不匹配。这种情况很可能导致全局
计数和子池记账不正确而需要调整。调用者有责任检查这种情况并做适当调整。)

The routine region_del() is called to remove regions from a reservation map.
It is typically called in the following situations:
(例程 region_del() 用于从预留映射中移除区域。它通常在以下情况下被调用：)

- When a file in the hugetlbfs filesystem is being removed, the inode will
  be released and the reservation map freed.  Before freeing the reservation
  map, all the individual file_region structures must be freed.  In this case
  region_del is passed the range [0, LONG_MAX).
  (当 hugetlbfs 文件系统中的文件被删除时，inode 会被释放，预留映射会被
  释放。在释放预留映射之前，必须释放所有单独的 file_region 结构。这种情
  况下传给 region_del 的范围是 [0, LONG_MAX)。)
- When a hugetlbfs file is being truncated.  In this case, all allocated pages
  after the new file size must be freed.  In addition, any file_region entries
  in the reservation map past the new end of file must be deleted.  In this
  case, region_del is passed the range [new_end_of_file, LONG_MAX).
  (当 hugetlbfs 文件被截断时，新文件大小之后所有已分配的页面必须释放。
  此外，预留映射中超出新文件末尾的所有 file_region 条目必须删除。这种情
  况下传给 region_del 的范围是 [new_end_of_file, LONG_MAX)。)
- When a hole is being punched in a hugetlbfs file.  In this case, huge pages
  are removed from the middle of the file one at a time.  As the pages are
  removed, region_del() is called to remove the corresponding entry from the
  reservation map.  In this case, region_del is passed the range
  [page_idx, page_idx + 1).
  (当在 hugetlbfs 文件中打洞 (punch hole) 时，巨页从文件中间逐个移除。页
  面移除时会调用 region_del() 从预留映射中移除相应条目。这种情况下传给
  region_del 的范围是 [page_idx, page_idx + 1)。)

In every case, region_del() will return the number of pages removed from the
reservation map.  In VERY rare cases, region_del() can fail.  This can only
happen in the hole punch case where it has to split an existing file_region
entry and can not allocate a new structure.  In this error case, region_del()
will return -ENOMEM.  The problem here is that the reservation map will
indicate that there is a reservation for the page.  However, the subpool and
global reservation counts will not reflect the reservation.  To handle this
situation, the routine hugetlb_fix_reserve_counts() is called to adjust the
counters so that they correspond with the reservation map entry that could
not be deleted.
(在所有情况下，region_del() 都会返回从预留映射中移除的页面数。在极罕见
的情况下，region_del() 可能失败。这只可能发生在打洞场合：它必须拆分现有
的 file_region 条目却无法分配新结构。在这种错误情况下，region_del() 会返
回 -ENOMEM。这里的问题是：预留映射会表明该页面存在预留，但子池和全局预
留计数不会反映这个预留。为处理这种情况，会调用例程
hugetlb_fix_reserve_counts() 来调整计数器，使其与无法删除的预留映射条目
相对应。)

region_count() is called when unmapping a private huge page mapping.  In
private mappings, the lack of a entry in the reservation map indicates that
a reservation exists.  Therefore, by counting the number of entries in the
reservation map we know how many reservations were consumed and how many are
outstanding (outstanding = (end - start) - region_count(resv, start, end)).
Since the mapping is going away, the subpool and global reservation counts
are decremented by the number of outstanding reservations.
(解除私有巨页映射时调用 region_count()。在私有映射中，预留映射中缺少条
目表示存在预留。因此，通过统计预留映射中的条目数，我们知道消耗了多少预
留、还有多少未消费(outstanding = (end - start) - region_count(resv, start,
end))。由于映射即将消失，子池和全局预留计数会按未消费预留的数量递减。)

.. _resv_map_helpers:

Reservation Map Helper Routines(预留映射辅助例程)
=================================================

Several helper routines exist to query and modify the reservation maps.
These routines are only interested with reservations for a specific huge
page, so they just pass in an address instead of a range.  In addition,
they pass in the associated VMA.  From the VMA, the type of mapping (private
or shared) and the location of the reservation map (inode or VMA) can be
determined.  These routines simply call the underlying routines described
in the section "Reservation Map Modifications".  However, they do take into
account the 'opposite' meaning of reservation map entries for private and
shared mappings and hide this detail from the caller::

	long vma_needs_reservation(struct hstate *h,
				   struct vm_area_struct *vma,
				   unsigned long addr)

(存在若干用于查询和修改预留映射的辅助例程。这些例程只关心某个特定巨页
的预留，因此只传入一个地址而不是范围。此外，它们还传入关联的 VMA。从
VMA 可以确定映射类型(私有或共享)以及预留映射的位置(inode 或 VMA)。这些
例程只是调用 "Reservation Map Modifications" 一节所述的底层例程。不过，
它们会考虑私有映射与共享映射在预留映射条目含义上"相反"的语义，并把这一
细节对调用者隐藏起来：)

This routine calls region_chg() for the specified page.  If no reservation
exists, 1 is returned.  If a reservation exists, 0 is returned::

	long vma_commit_reservation(struct hstate *h,
				    struct vm_area_struct *vma,
				    unsigned long addr)

(该例程对指定页面调用 region_chg()。如果不存在预留，返回 1；如果存在预
留，返回 0：)

This calls region_add() for the specified page.  As in the case of region_chg
and region_add, this routine is to be called after a previous call to
vma_needs_reservation.  It will add a reservation entry for the page.  It
returns 1 if the reservation was added and 0 if not.  The return value should
be compared with the return value of the previous call to
vma_needs_reservation.  An unexpected difference indicates the reservation
map was modified between calls::

	void vma_end_reservation(struct hstate *h,
				 struct vm_area_struct *vma,
				 unsigned long addr)

(它对指定页面调用 region_add()。与 region_chg 和 region_add 的情况一样，
该例程要在先前调用 vma_needs_reservation 之后调用。它会为该页面添加一
个预留条目。如果预留已添加则返回 1，否则返回 0。返回值应与先前
vma_needs_reservation 调用的返回值比较。出现意外差异表明两次调用之间预
留映射被修改过：)

This calls region_abort() for the specified page.  As in the case of region_chg
and region_abort, this routine is to be called after a previous call to
vma_needs_reservation.  It will abort/end the in progress reservation add
operation::

	long vma_add_reservation(struct hstate *h,
				 struct vm_area_struct *vma,
				 unsigned long addr)

(它对指定页面调用 region_abort()。与 region_chg 和 region_abort 的情况一
样，该例程要在先前调用 vma_needs_reservation 之后调用。它将中止/结束进
行中的预留添加操作：)

This is a special wrapper routine to help facilitate reservation cleanup
on error paths.  It is only called from the routine restore_reserve_on_error().
This routine is used in conjunction with vma_needs_reservation in an attempt
to add a reservation to the reservation map.  It takes into account the
different reservation map semantics for private and shared mappings.  Hence,
region_add is called for shared mappings (as an entry present in the map
indicates a reservation), and region_del is called for private mappings (as
the absence of an entry in the map indicates a reservation).  See the section
"Reservation cleanup in error paths" for more information on what needs to
be done on error paths.
(这是一个特殊的包装例程，用于帮助在错误路径上进行预留清理。它只从例程
restore_reserve_on_error() 中调用。该例程与 vma_needs_reservation 配合使
用，尝试向预留映射添加预留。它考虑了私有映射与共享映射不同的预留映射语
义。因此，共享映射调用 region_add(因为映射中存在条目表示有预留)，私有映
射调用 region_del(因为映射中缺少条目表示有预留)。错误路径上需要做什么
的更多信息，参见 "Reservation cleanup in error paths" 一节。)


Reservation Cleanup in Error Paths(错误路径中的预留清理)
========================================================

As mentioned in the section
:ref:`Reservation Map Helper Routines <resv_map_helpers>`, reservation
map modifications are performed in two steps.  First vma_needs_reservation
is called before a page is allocated.  If the allocation is successful,
then vma_commit_reservation is called.  If not, vma_end_reservation is called.
Global and subpool reservation counts are adjusted based on success or failure
of the operation and all is well.
(如 :ref:`Reservation Map Helper Routines(预留映射辅助例程)
<resv_map_helpers>` 一节所述，预留映射的修改分两步进行。首先在分配页面
之前调用 vma_needs_reservation。如果分配成功，则调用 vma_commit_reservation；
否则调用 vma_end_reservation。全局和子池预留计数根据操作的成败进行调整，
一切正常。)

Additionally, after a huge page is instantiated the PagePrivate flag is
cleared so that accounting when the page is ultimately freed is correct.
(此外，巨页实例化之后 PagePrivate 标志会被清除，这样页面最终释放时的记
账才是正确的。)

However, there are several instances where errors are encountered after a huge
page is allocated but before it is instantiated.  In this case, the page
allocation has consumed the reservation and made the appropriate subpool,
reservation map and global count adjustments.  If the page is freed at this
time (before instantiation and clearing of PagePrivate), then free_huge_folio
will increment the global reservation count.  However, the reservation map
indicates the reservation was consumed.  This resulting inconsistent state
will cause the 'leak' of a reserved huge page.  The global reserve count will
be  higher than it should and prevent allocation of a pre-allocated page.
(然而，有若干情形会在巨页分配之后、实例化之前遇到错误。此时页面分配已
经消费了预留，并对子池、预留映射和全局计数做了相应调整。如果此时(在实
例化和清除 PagePrivate 之前)释放页面，free_huge_folio 会递增全局预留计数。
但预留映射却表明预留已被消费。这种不一致状态会导致保留巨页"泄漏"。全局
预留计数会高于应有值，从而阻碍预分配页面的分配。)

The routine restore_reserve_on_error() attempts to handle this situation.  It
is fairly well documented.  The intention of this routine is to restore
the reservation map to the way it was before the page allocation.   In this
way, the state of the reservation map will correspond to the global reservation
count after the page is freed.
(例程 restore_reserve_on_error() 尝试处理这种情况。它有相当完善的文档。
该例程的意图是把预留映射恢复到页面分配之前的样子。这样，页面释放后预留
映射的状态将与全局预留计数相对应。)

The routine restore_reserve_on_error itself may encounter errors while
attempting to restore the reservation map entry.  In this case, it will
simply clear the PagePrivate flag of the page.  In this way, the global
reserve count will not be incremented when the page is freed.  However, the
reservation map will continue to look as though the reservation was consumed.
A page can still be allocated for the address, but it will not use a reserved
page as originally intended.
(例程 restore_reserve_on_error 自身在尝试恢复预留映射条目时也可能遇到错
误。这种情况下，它只是清除页面的 PagePrivate 标志。这样，页面释放时全局
预留计数不会递增。但预留映射看起来仍像预留已被消费。该地址仍然可以分配
页面，但不会像最初意图那样使用保留页面。)

There is some code (most notably userfaultfd) which can not call
restore_reserve_on_error.  In this case, it simply modifies the PagePrivate
so that a reservation will not be leaked when the huge page is freed.
(有些代码(最著名的是 userfaultfd)无法调用 restore_reserve_on_error。这种
情况下，它只是修改 PagePrivate，使得释放巨页时不会泄漏预留。)


Reservations and Memory Policy(预留与内存策略)
==============================================
Per-node huge page lists existed in struct hstate when git was first used
to manage Linux code.  The concept of reservations was added some time later.
When reservations were added, no attempt was made to take memory policy
into account.  While cpusets are not exactly the same as memory policy, this
comment in hugetlb_acct_memory sums up the interaction between reservations
and cpusets/memory policy::

	/*
	 * When cpuset is configured, it breaks the strict hugetlb page
	 * reservation as the accounting is done on a global variable. Such
	 * reservation is completely rubbish in the presence of cpuset because
	 * the reservation is not checked against page availability for the
	 * current cpuset. Application can still potentially OOM'ed by kernel
	 * with lack of free htlb page in cpuset that the task is in.
	 * Attempt to enforce strict accounting with cpuset is almost
	 * impossible (or too ugly) because cpuset is too fluid that
	 * task or memory node can be dynamically moved between cpusets.
	 *
	 * The change of semantics for shared hugetlb mapping with cpuset is
	 * undesirable. However, in order to preserve some of the semantics,
	 * we fall back to check against current free page availability as
	 * a best attempt and hopefully to minimize the impact of changing
	 * semantics that cpuset has.
	 */

(当 git 最初用于管理 Linux 代码时，struct hstate 中就已经存在每节点的巨
页链表。预留的概念是很久之后才加入的。加入预留时，并没有尝试考虑内存策
略。虽然 cpuset 与内存策略并不完全相同，但 hugetlb_acct_memory 中的这段
注释概括了预留与 cpuset/内存策略之间的相互作用：)

Huge page reservations were added to prevent unexpected page allocation
failures (OOM) at page fault time.  However, if an application makes use
of cpusets or memory policy there is no guarantee that huge pages will be
available on the required nodes.  This is true even if there are a sufficient
number of global reservations.

Hugetlbfs regression testing
============================

The most complete set of hugetlb tests are in the libhugetlbfs repository.
If you modify any hugetlb related code, use the libhugetlbfs test suite
to check for regressions.  In addition, if you add any new hugetlb
functionality, please add appropriate tests to libhugetlbfs.

--
Mike Kravetz, 7 April 2017
