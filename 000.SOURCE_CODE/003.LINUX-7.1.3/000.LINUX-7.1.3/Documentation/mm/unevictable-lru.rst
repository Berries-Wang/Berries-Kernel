=====================================================
Unevictable LRU Infrastructure(不可逐出 LRU 基础设施)
=====================================================

.. contents:: :local:


Introduction(引言)
==================

This document describes the Linux memory manager's "Unevictable LRU"
infrastructure and the use of this to manage several types of "unevictable"
folios.
(本文档描述 Linux 内存管理器的 "Unevictable LRU" 基础设施，以及用它来管理
若干类型"不可逐出" (unevictable) folio 的方式。)

The document attempts to provide the overall rationale behind this mechanism
and the rationale for some of the design decisions that drove the
implementation.  The latter design rationale is discussed in the context of an
implementation description.  Admittedly, one can obtain the implementation
details - the "what does it do?" - by reading the code.  One hopes that the
descriptions below add value by provide the answer to "why does it do that?".
(本文档试图给出这一机制背后的总体依据，以及驱动其实现的若干设计决策的依
据。后一种设计依据是在实现描述的语境中讨论的。诚然，通过阅读代码就能获得
实现细节——即"它做了什么？"——但人们希望下文的描述能提供"它为什么这么
做？"的答案，从而增加价值。)



The Unevictable LRU(不可逐出 LRU)
=================================

The Unevictable LRU facility adds an additional LRU list to track unevictable
folios and to hide these folios from vmscan.  This mechanism is based on a patch
by Larry Woodman of Red Hat to address several scalability problems with folio
reclaim in Linux.  The problems have been observed at customer sites on large
memory x86_64 systems.
(不可逐出 LRU 机制增加了一条额外的 LRU 链表来跟踪不可逐出的 folio，并把这
些 folio 对 vmscan 隐藏起来。该机制基于 Red Hat 的 Larry Woodman 提交的一个
补丁，用于解决 Linux 中 folio 回收的若干可扩展性问题。这些问题曾在客户现
场的大内存 x86_64 系统上被观察到。)

To illustrate this with an example, a non-NUMA x86_64 platform with 128GB of
main memory will have over 32 million 4k pages in a single node.  When a large
fraction of these pages are not evictable for any reason [see below], vmscan
will spend a lot of time scanning the LRU lists looking for the small fraction
of pages that are evictable.  This can result in a situation where all CPUs are
spending 100% of their time in vmscan for hours or days on end, with the system
completely unresponsive.
(举个例子：一台 128GB 主内存的非 NUMA x86_64 平台，单个节点内将有三千两百
多万个 4k 页面。当其中很大一部分页面因任何原因[见下文]不可逐出时，vmscan
将花费大量时间扫描 LRU 链表，寻找那一小部分可逐出的页面。这可能导致所有
CPU 连续数小时或数天 100% 的时间都耗在 vmscan 里，系统完全失去响应。)

The unevictable list addresses the following classes of unevictable pages:
(不可逐出链表处理以下几类不可逐出页面：)

 * Those owned by ramfs.
   (那些归 ramfs 所有的页面。)

 * Those owned by tmpfs with the noswap mount option.
   (那些归带 noswap 挂载选项的 tmpfs 所有的页面。)

 * Those mapped into SHM_LOCK'd shared memory regions.
   (那些被映射进 SHM_LOCK 过的共享内存区域的页面。)

 * Those mapped into VM_LOCKED [mlock()ed] VMAs.
   (那些被映射进 VM_LOCKED[mlock() 过] VMA 的页面。)

The infrastructure may also be able to handle other conditions that make pages
unevictable, either by definition or by circumstance, in the future.
(未来，该基础设施或许还能处理其他使页面不可逐出的情况——无论是定义上的
还是情势所迫的。)


The Unevictable LRU Folio List(不可逐出 LRU folio 链表)
-------------------------------------------------------

The Unevictable LRU folio list is a lie.  It was never an LRU-ordered
list, but a companion to the LRU-ordered anonymous and file, active and
inactive folio lists; and now it is not even a folio list.  But following
familiar convention, here in this document and in the source, we often
imagine it as a fifth LRU folio list.
("不可逐出 LRU folio 链表"是个谎言。它从来都不是按 LRU 排序的链表，而是
按 LRU 排序的匿名/文件、活跃/非活跃 folio 链表的伴生物；如今它甚至不再是
一个 folio 链表。但遵照熟悉的惯例，在本文档与源码中，我们常把它想象成第
五条 LRU folio 链表。)

The Unevictable LRU infrastructure consists of an additional, per-node, LRU list
called the "unevictable" list and an associated folio flag, PG_unevictable, to
indicate that the folio is being managed on the unevictable list.
(不可逐出 LRU 基础设施由一条额外的每节点 LRU 链表组成，称为 "unevictable"
链表，以及一个关联的 folio 标志 PG_unevictable，用于表明该 folio 正在不可
逐出链表上被管理。)

The PG_unevictable flag is analogous to, and mutually exclusive with, the
PG_active flag in that it indicates on which LRU list a folio resides when
PG_lru is set.
(PG_unevictable 标志与 PG_active 标志类似且互斥：它表示当 PG_lru 被设置
时，folio 驻留在哪条 LRU 链表上。)

The Unevictable LRU infrastructure maintains unevictable folios as if they were
on an additional LRU list for a few reasons:
(不可逐出 LRU 基础设施把不可逐出 folio 当作在一条额外 LRU 链表上来维护，
原因有几点：)

 (1) We get to "treat unevictable folios just like we treat other folios in the
     system - which means we get to use the same code to manipulate them, the
     same code to isolate them (for migrate, etc.), the same code to keep track
     of the statistics, etc..." [Rik van Riel]
     (我们能"像对待系统中其他 folio 一样对待不可逐出 folio——这意味着我们
     可以用同样的代码去操作它们、用同样的代码去隔离它们(用于迁移等)、用
     同样的代码去统计，等等……" [Rik van Riel])

 (2) We want to be able to migrate unevictable folios between nodes for memory
     defragmentation, workload management and memory hotplug.  The Linux kernel
     can only migrate folios that it can successfully isolate from the LRU
     lists (or "Movable" folios: outside of consideration here).  If we were to
     maintain folios elsewhere than on an LRU-like list, where they can be
     detected by folio_isolate_lru(), we would prevent their migration.
     (我们希望能够为了内存碎片整理、负载管理与内存热插拔而在节点之间迁移
     不可逐出 folio。Linux 内核只能迁移它能从 LRU 链表成功隔离出来的 folio
     (或 "Movable" folio：此处不予考虑)。如果我们把 folio 维护在 LRU 类链
     表之外——folio_isolate_lru() 无法检测到它们的地方——就会阻止它们的
     迁移。)

The unevictable list does not differentiate between file-backed and
anonymous, swap-backed folios.  This differentiation is only important
while the folios are, in fact, evictable.
(不可逐出链表不区分文件后备与匿名、交换后备的 folio。这种区分只在 folio
确实可逐出时才重要。)

The unevictable list benefits from the "arrayification" of the per-node LRU
lists and statistics originally proposed and posted by Christoph Lameter.
(不可逐出链表受益于每节点 LRU 链表与统计信息的"数组化" (arrayification)，
该做法最初由 Christoph Lameter 提出并发布。)


Memory Control Group Interaction(与内存控制组的交互)
----------------------------------------------------

The unevictable LRU facility interacts with the memory control group [aka
memory controller; see Documentation/admin-guide/cgroup-v1/memory.rst] by
extending the lru_list enum.
(不可逐出 LRU 机制通过扩展 lru_list 枚举与内存控制组[又称 memory
controller；见 Documentation/admin-guide/cgroup-v1/memory.rst]交互。)

The memory controller data structure automatically gets a per-node unevictable
list as a result of the "arrayification" of the per-node LRU lists (one per
lru_list enum element).  The memory controller tracks the movement of pages to
and from the unevictable list.
(由于每节点 LRU 链表的"数组化"(每个 lru_list 枚举元素一条)，内存控制器
的数据结构自动获得每节点不可逐出链表。内存控制器跟踪页面进出不可逐出链表
的移动。)

When a memory control group comes under memory pressure, the controller will
not attempt to reclaim pages on the unevictable list.  This has a couple of
effects:
(当内存控制组面临内存压力时，控制器不会尝试回收不可逐出链表上的页面。这
有几个影响：)

 (1) Because the pages are "hidden" from reclaim on the unevictable list, the
     reclaim process can be more efficient, dealing only with pages that have a
     chance of being reclaimed.
     (由于这些页面对回收而言在不可逐出链表上是"隐藏"的，回收过程可以更高
     效，只处理那些有机会被回收的页面。)

 (2) On the other hand, if too many of the pages charged to the control group
     are unevictable, the evictable portion of the working set of the tasks in
     the control group may not fit into the available memory.  This can cause
     the control group to thrash or to OOM-kill tasks.
     (另一方面，如果控制组中被计入的页面有太多不可逐出，控制组中各任务工
     作集中可逐出的部分可能装不进可用内存。这可能导致控制组颠簸 (thrash)
     或 OOM 杀死任务。)


.. _mark_addr_space_unevict:

Marking Address Spaces Unevictable(将地址空间标记为不可逐出)
------------------------------------------------------------

For facilities such as ramfs none of the pages attached to the address space
may be evicted.  To prevent eviction of any such pages, the AS_UNEVICTABLE
address space flag is provided, and this can be manipulated by a filesystem
using a number of wrapper functions:
(对于 ramfs 这类设施，挂在地址空间上的任何页面都不可被逐出。为了防止这类
页面被逐出，内核提供了 AS_UNEVICTABLE 地址空间标志，文件系统可以用若干包
装函数来操作它：)

 * ``void mapping_set_unevictable(struct address_space *mapping);``

	Mark the address space as being completely unevictable.
	(把地址空间标记为完全不可逐出。)

 * ``void mapping_clear_unevictable(struct address_space *mapping);``

	Mark the address space as being evictable.
	(把地址空间标记为可逐出。)

 * ``int mapping_unevictable(struct address_space *mapping);``

	Query the address space, and return true if it is completely
	unevictable.
	(查询地址空间，如果它完全不可逐出则返回 true。)

These are currently used in three places in the kernel:
(目前内核中有三处使用它们：)

 (1) By ramfs to mark the address spaces of its inodes when they are created,
     and this mark remains for the life of the inode.
     (ramfs 在其 inode 创建时标记它们的地址空间，该标记保持到 inode 的生命
     周期结束。)

 (2) By SYSV SHM to mark SHM_LOCK'd address spaces until SHM_UNLOCK is called.
     Note that SHM_LOCK is not required to page in the locked pages if they're
     swapped out; the application must touch the pages manually if it wants to
     ensure they're in memory.
     (SYSV SHM 用它标记 SHM_LOCK 过的地址空间，直到调用 SHM_UNLOCK。注意，
     如果被锁页面已被换出，SHM_LOCK 并不要求把它们换入；应用若想确保这些
     页面在内存中，必须手动触碰它们。)

 (3) By the i915 driver to mark pinned address space until it's unpinned. The
     amount of unevictable memory marked by i915 driver is roughly the bounded
     object size in debugfs/dri/0/i915_gem_objects.
     (i915 驱动用它标记已固定的地址空间，直到解除固定。i915 驱动标记的不
     可逐出内存量大致是 debugfs/dri/0/i915_gem_objects 中有界对象的大小。)


Detecting Unevictable Pages(检测不可逐出页面)
---------------------------------------------

The function folio_evictable() in mm/internal.h determines whether a folio is
evictable or not using the query function outlined above [see section
:ref:`Marking address spaces unevictable <mark_addr_space_unevict>`]
to check the AS_UNEVICTABLE flag.
(mm/internal.h 中的 folio_evictable() 函数用上文概述的查询函数[参见
:ref:`Marking address spaces unevictable(将地址空间标记为不可逐出)
<mark_addr_space_unevict>` 一节]检查 AS_UNEVICTABLE 标志，来判断一个 folio
是否可逐出。)

For address spaces that are so marked after being populated (as SHM regions
might be), the lock action (e.g. SHM_LOCK) can be lazy, and need not populate
the page tables for the region as does, for example, mlock(), nor need it make
any special effort to push any pages in the SHM_LOCK'd area to the unevictable
list.  Instead, vmscan will do this if and when it encounters the folios during
a reclamation scan.
(对于在填充内容之后才被这样标记的地址空间(SHM 区域可能如此)，加锁动作
(例如 SHM_LOCK)可以是懒惰的：不必像 mlock() 那样为该区域填充页表，也不必
特意把 SHM_LOCK 区域内的页面推到不可逐出链表上。相反，vmscan 会在回收扫描
中遇到这些 folio 时(如果遇到的话)代劳。)

On an unlock action (such as SHM_UNLOCK), the unlocker (e.g. shmctl()) must scan
the pages in the region and "rescue" them from the unevictable list if no other
condition is keeping them unevictable.  If an unevictable region is destroyed,
the pages are also "rescued" from the unevictable list in the process of
freeing them.
(在解锁动作(如 SHM_UNLOCK)时，解锁者(例如 shmctl())必须扫描区域内的页
面，在没有其他条件使其保持不可逐出时，把它们从不可逐出链表上"拯救"出来。
如果一个不可逐出区域被销毁，这些页面也会在释放过程中从不可逐出链表上被
"拯救"出来。)

folio_evictable() also checks for mlocked folios by calling
folio_test_mlocked(), which is set when a folio is faulted into a
VM_LOCKED VMA, or found in a VMA being VM_LOCKED.
(folio_evictable() 还通过调用 folio_test_mlocked() 来检查 mlocked folio；
当一个 folio 缺页进入 VM_LOCKED VMA、或在一个正在被 VM_LOCKED 的 VMA 中被
发现时，该标志被设置。)


Vmscan's Handling of Unevictable Folios(Vmscan 对不可逐出 folio 的处理)
-----------------------------------------------------------------------

If unevictable folios are culled in the fault path, or moved to the unevictable
list at mlock() or mmap() time, vmscan will not encounter the folios until they
have become evictable again (via munlock() for example) and have been "rescued"
from the unevictable list.  However, there may be situations where we decide,
for the sake of expediency, to leave an unevictable folio on one of the regular
active/inactive LRU lists for vmscan to deal with.  vmscan checks for such
folios in all of the shrink_{active|inactive|folio}_list() functions and will
"cull" such folios that it encounters: that is, it diverts those folios to the
unevictable list for the memory cgroup and node being scanned.
(如果不可逐出 folio 在缺页路径中就被剔除，或在 mlock()/mmap() 时就被移到不
可逐出链表，那么 vmscan 在它们重新变得可逐出(例如通过 munlock())并从不可
逐出链表被"拯救"出来之前，不会遇到它们。不过，也可能出于省事的考虑，我们
决定把某个不可逐出 folio 留在常规的活跃/非活跃 LRU 链表上，交给 vmscan 处
理。vmscan 在所有 shrink_{active|inactive|folio}_list() 函数中都会检查这类
folio，并会"剔除" (cull) 它遇到的这类 folio：也就是把这些 folio 转移到正
在扫描的内存 cgroup 与节点的不可逐出链表上。)

There may be situations where a folio is mapped into a VM_LOCKED VMA,
but the folio does not have the mlocked flag set.  Such folios will make
it all the way to shrink_active_list() or shrink_folio_list() where they
will be detected when vmscan walks the reverse map in folio_referenced()
or try_to_unmap().  The folio is culled to the unevictable list when it
is released by the shrinker.
(也可能存在这样的情况：folio 被映射进 VM_LOCKED VMA，但没有设置 mlocked 标
志。这类 folio 会一路走到 shrink_active_list() 或 shrink_folio_list()，在
vmscan 于 folio_referenced() 或 try_to_unmap() 中遍历反向映射时被检测出来。
当 shrinker 释放该 folio 时，它会被剔除到不可逐出链表。)

To "cull" an unevictable folio, vmscan simply puts the folio back on
the LRU list using folio_putback_lru() - the inverse operation to
folio_isolate_lru() - after dropping the folio lock.  Because the
condition which makes the folio unevictable may change once the folio
is unlocked, __pagevec_lru_add_fn() will recheck the unevictable state
of a folio before placing it on the unevictable list.
(要"剔除"一个不可逐出 folio，vmscan 只需在放下 folio 锁之后，用
folio_putback_lru()——folio_isolate_lru() 的逆操作——把 folio 放回 LRU 链
表。由于使 folio 不可逐出的条件在 folio 解锁后可能变化，
__pagevec_lru_add_fn() 会把 folio 放上不可逐出链表之前重新检查其不可逐出
状态。)


MLOCKED Pages(MLOCKED 页面)
===========================

The unevictable folio list is also useful for mlock(), in addition to ramfs and
SYSV SHM.  Note that mlock() is only available in CONFIG_MMU=y situations; in
NOMMU situations, all mappings are effectively mlocked.
(除了 ramfs 与 SYSV SHM，不可逐出 folio 链表对 mlock() 也很有用。注意，
mlock() 只在 CONFIG_MMU=y 的情况下可用；在 NOMMU 情况下，所有映射实际上都
是 mlocked 的。)


History(历史)
-------------

The "Unevictable mlocked Pages" infrastructure is based on work originally
posted by Nick Piggin in an RFC patch entitled "mm: mlocked pages off LRU".
Nick posted his patch as an alternative to a patch posted by Christoph Lameter
to achieve the same objective: hiding mlocked pages from vmscan.
("不可逐出 mlocked 页面"基础设施基于 Nick Piggin 最初以题为 "mm: mlocked
pages off LRU" 的 RFC 补丁发布的工作。Nick 发布他的补丁，作为
Christoph Lameter 为实现同一目标——把 mlocked 页面对 vmscan 隐藏——所发
补丁的替代方案。)

In Nick's patch, he used one of the struct page LRU list link fields as a count
of VM_LOCKED VMAs that map the page (Rik van Riel had the same idea three years
earlier).  But this use of the link field for a count prevented the management
of the pages on an LRU list, and thus mlocked pages were not migratable as
folio_isolate_lru() could not detect them, and the LRU list link field was not
available to the migration subsystem.
(在 Nick 的补丁中，他把 struct page 的某个 LRU 链表链接字段用作映射该页面
的 VM_LOCKED VMA 计数(Rik van Riel 三年前有过同样的想法)。但把链接字段用
作计数使得页面无法在 LRU 链表上管理，因此 mlocked 页面不可迁移：
folio_isolate_lru() 检测不到它们，而且 LRU 链表链接字段也无法供迁移子系统
使用。)

Nick resolved this by putting mlocked pages back on the LRU list before
attempting to isolate them, thus abandoning the count of VM_LOCKED VMAs.  When
Nick's patch was integrated with the Unevictable LRU work, the count was
replaced by walking the reverse map when munlocking, to determine whether any
other VM_LOCKED VMAs still mapped the page.
(Nick 通过在尝试隔离之前先把 mlocked 页面放回 LRU 链表解决了这个问题，从
而放弃了 VM_LOCKED VMA 计数。当 Nick 的补丁与不可逐出 LRU 工作整合时，该计
数被取代：munlock 时改为遍历反向映射，以确定是否还有其他 VM_LOCKED VMA 映
射着该页面。)

However, walking the reverse map for each page when munlocking was ugly and
inefficient, and could lead to catastrophic contention on a file's rmap lock,
when many processes which had it mlocked were trying to exit.  In 5.18, the
idea of keeping mlock_count in Unevictable LRU list link field was revived and
put to work, without preventing the migration of mlocked pages.  This is why
the "Unevictable LRU list" cannot be a linked list of pages now; but there was
no use for that linked list anyway - though its size is maintained for meminfo.
(然而，munlock 时对每个页面都遍历反向映射既难看又低效，而且当许多 mlock 过
它的进程同时退出时，可能在文件的 rmap 锁上造成灾难性竞争。在 5.18 中，把
mlock_count 保存在不可逐出 LRU 链表链接字段中的想法被重启并投入使用，且不
妨碍 mlocked 页面的迁移。这就是为什么 "不可逐出 LRU 链表" 现在不能是一个
页面链表；反正那条链表也没有什么用处——不过它的大小仍为 meminfo 而维护。)


Basic Management(基本管理)
--------------------------

mlocked pages - pages mapped into a VM_LOCKED VMA - are a class of unevictable
pages.  When such a page has been "noticed" by the memory management subsystem,
the folio is marked with the PG_mlocked flag.  This can be manipulated using
folio_set_mlocked() and folio_clear_mlocked() functions.
(mlocked 页面——被映射进 VM_LOCKED VMA 的页面——是不可逐出页面中的一类。
当内存管理子系统"注意到"这样一个页面时，folio 会被打上 PG_mlocked 标志。
可以用 folio_set_mlocked() 与 folio_clear_mlocked() 函数操作它。)

A PG_mlocked page will be placed on the unevictable list when it is added to
the LRU.  Such pages can be "noticed" by memory management in several places:
(PG_mlocked 页面在被加入 LRU 时会被放到不可逐出链表上。内存管理可能在以下
若干地方"注意到"这类页面：)

 (1) in the mlock()/mlock2()/mlockall() system call handlers;
     (在 mlock()/mlock2()/mlockall() 系统调用处理函数中；)

 (2) in the mmap() system call handler when mmapping a region with the
     MAP_LOCKED flag;
     (在 mmap() 系统调用处理函数中，当以 MAP_LOCKED 标志映射一个区域时；)

 (3) mmapping a region in a task that has called mlockall() with the MCL_FUTURE
     flag;
     (在已用 MCL_FUTURE 标志调用过 mlockall() 的任务中映射一个区域时；)

 (4) in the fault path and when a VM_LOCKED stack segment is expanded; or
     (在缺页路径中，以及 VM_LOCKED 栈段被扩展时；或)

 (5) as mentioned above, in vmscan:shrink_folio_list() when attempting to
     reclaim a page in a VM_LOCKED VMA by folio_referenced() or try_to_unmap().
     (如上所述，在 vmscan:shrink_folio_list() 中，当试图通过
     folio_referenced() 或 try_to_unmap() 回收一个位于 VM_LOCKED VMA 中的页
     面时。)

mlocked pages become unlocked and rescued from the unevictable list when:
(mlocked 页面在以下情况被解锁并从不可逐出链表拯救出来：)

 (1) mapped in a range unlocked via the munlock()/munlockall() system calls;
     (它映射的范围通过 munlock()/munlockall() 系统调用解锁时；)

 (2) munmap()'d out of the last VM_LOCKED VMA that maps the page, including
     unmapping at task exit;
     (通过 munmap() 从最后一个映射该页面的 VM_LOCKED VMA 中解除映射时，包
     括任务退出时的解除映射；)

 (3) when the page is truncated from the last VM_LOCKED VMA of an mmapped file;
     or
     (当页面从已 mmap 文件的最后一个 VM_LOCKED VMA 中被截断掉时；或)

 (4) before a page is COW'd in a VM_LOCKED VMA.
     (在页面于 VM_LOCKED VMA 中被 COW 之前。)


mlock()/mlock2()/mlockall() System Call Handling(mlock()/mlock2()/mlockall() 系统调用的处理)
--------------------------------------------------------------------------------------------

mlock(), mlock2() and mlockall() system call handlers proceed to mlock_fixup()
for each VMA in the range specified by the call.  In the case of mlockall(),
this is the entire active address space of the task.  Note that mlock_fixup()
is used for both mlocking and munlocking a range of memory.  A call to mlock()
an already VM_LOCKED VMA, or to munlock() a VMA that is not VM_LOCKED, is
treated as a no-op and mlock_fixup() simply returns.
(mlock()、mlock2() 与 mlockall() 系统调用处理函数会对调用指定范围内的每个
VMA 执行 mlock_fixup()。对 mlockall() 而言，该范围就是任务的整个活动地址空
间。注意 mlock_fixup() 既用于 mlock 也用于 munlock 一段内存。对已经
VM_LOCKED 的 VMA 调用 mlock()，或对不是 VM_LOCKED 的 VMA 调用 munlock()，都
被视为无操作，mlock_fixup() 直接返回。)

If the VMA passes some filtering as described in "Filtering Special VMAs"
below, mlock_fixup() will attempt to merge the VMA with its neighbors or split
off a subset of the VMA if the range does not cover the entire VMA.  Any pages
already present in the VMA are then marked as mlocked by mlock_folio() via
mlock_pte_range() via walk_page_range() via mlock_vma_pages_range().
(如果该 VMA 通过了下文 "Filtering Special VMAs" 所述的某种过滤，
mlock_fixup() 会尝试把该 VMA 与相邻 VMA 合并；若范围未覆盖整个 VMA，则分裂
出该 VMA 的一个子集。VMA 中已存在的页面随后由 mlock_folio() 标记为
mlocked，调用路径是 mlock_vma_pages_range() → walk_page_range() →
mlock_pte_range() → mlock_folio()。)

Before returning from the system call, do_mlock() or mlockall() will call
__mm_populate() to fault in the remaining pages via get_user_pages() and to
mark those pages as mlocked as they are faulted.
(在从系统调用返回之前，do_mlock() 或 mlockall() 会调用 __mm_populate()，通
过 get_user_pages() 把其余页面缺页调入，并在它们被调入时标记为 mlocked。)

Note that the VMA being mlocked might be mapped with PROT_NONE.  In this case,
get_user_pages() will be unable to fault in the pages.  That's okay.  If pages
do end up getting faulted into this VM_LOCKED VMA, they will be handled in the
fault path - which is also how mlock2()'s MLOCK_ONFAULT areas are handled.
(注意，被 mlock 的 VMA 可能是以 PROT_NONE 映射的。这种情况下
get_user_pages() 将无法把页面调入。这没关系。如果页面最终确实被调入这个
VM_LOCKED VMA，它们会在缺页路径中被处理——mlock2() 的 MLOCK_ONFAULT 区域也
是这样处理的。)

For each PTE (or PMD) being faulted into a VMA, the page add rmap function
calls mlock_vma_folio(), which calls mlock_folio() when the VMA is VM_LOCKED
(unless it is a PTE mapping of a part of a transparent huge page).  Or when
it is a newly allocated anonymous page, folio_add_lru_vma() calls
mlock_new_folio() instead: similar to mlock_folio(), but can make better
judgments, since this page is held exclusively and known not to be on LRU yet.
(对每个被缺页调入 VMA 的 PTE(或 PMD)，页面添加 rmap 函数会调用
mlock_vma_folio()；当 VMA 是 VM_LOCKED 时，它会调用 mlock_folio()(除非这是
透明大页一部分的 PTE 映射)。而如果是新分配的匿名页，folio_add_lru_vma()
会改为调用 mlock_new_folio()：它与 mlock_folio() 类似，但能做更好的判断，
因为这个页面被独占持有，且已知尚未在 LRU 上。)

mlock_folio() sets PG_mlocked immediately, then places the page on the CPU's
mlock folio batch, to batch up the rest of the work to be done under lru_lock by
__mlock_folio().  __mlock_folio() sets PG_unevictable, initializes mlock_count
and moves the page to unevictable state ("the unevictable LRU", but with
mlock_count in place of LRU threading).  Or if the page was already PG_lru
and PG_unevictable and PG_mlocked, it simply increments the mlock_count.
(mlock_folio() 立即设置 PG_mlocked，然后把页面放到 CPU 的 mlock folio 批处
理中，把余下的工作攒起来在 lru_lock 之下由 __mlock_folio() 完成。
__mlock_folio() 设置 PG_unevictable，初始化 mlock_count，并把页面移动到不
可逐出状态("不可逐出 LRU"，但用 mlock_count 代替 LRU 穿线)。或者，如果页
面已经同时具有 PG_lru、PG_unevictable 与 PG_mlocked，它只是递增
mlock_count。)

But in practice that may not work ideally: the page may not yet be on an LRU, or
it may have been temporarily isolated from LRU.  In such cases the mlock_count
field cannot be touched, but will be set to 0 later when __munlock_folio()
returns the page to "LRU".  Races prohibit mlock_count from being set to 1 then:
rather than risk stranding a page indefinitely as unevictable, always err with
mlock_count on the low side, so that when munlocked the page will be rescued to
an evictable LRU, then perhaps be mlocked again later if vmscan finds it in a
VM_LOCKED VMA.
(但实践中这可能并不理想：页面可能尚未在 LRU 上，或已被临时从 LRU 隔离。这
些情况下 mlock_count 字段不能碰，稍后 __munlock_folio() 把页面归还 "LRU"
时会把它设为 0。由于竞争的存在，此时不能把 mlock_count 设为 1：与其冒险让
页面作为不可逐出页面被无限期搁浅，不如总是让 mlock_count 偏低，这样 munlock
时页面会被拯救到可逐出 LRU，之后若 vmscan 发现它位于某个 VM_LOCKED VMA 中，
也许会再次 mlock 它。)


Filtering Special VMAs(过滤特殊 VMA)
------------------------------------

mlock_fixup() filters several classes of "special" VMAs:
(mlock_fixup() 会过滤若干类"特殊" VMA：)

1) VMAs with VM_IO or VM_PFNMAP set are skipped entirely.  The pages behind
   these mappings are inherently pinned, so we don't need to mark them as
   mlocked.  In any case, most of the pages have no struct page in which to so
   mark the page.  Because of this, get_user_pages() will fail for these VMAs,
   so there is no sense in attempting to visit them.
   (设置了 VM_IO 或 VM_PFNMAP 的 VMA 被完全跳过。这些映射背后的页面本来就
   是固定的，所以无需把它们标记为 mlocked。况且，其中大多数页面没有
   struct page，无处做此标记。因此 get_user_pages() 对这些 VMA 会失败，尝
   试访问它们毫无意义。)

2) VMAs mapping hugetlbfs page are already effectively pinned into memory.  We
   neither need nor want to mlock() these pages.  But __mm_populate() includes
   hugetlbfs ranges, allocating the huge pages and populating the PTEs.
   (映射 hugetlbfs 页面的 VMA 实际上已被固定进内存。我们既不需要也不希望
   mlock() 这些页面。但 __mm_populate() 会包含 hugetlbfs 范围，分配大页并
   填充 PTE。)

3) VMAs with VM_DONTEXPAND are generally userspace mappings of kernel pages,
   such as the VDSO page, relay channel pages, etc.  These pages are inherently
   unevictable and are not managed on the LRU lists.  __mm_populate() includes
   these ranges, populating the PTEs if not already populated.
   (带 VM_DONTEXPAND 的 VMA 通常是内核页面的用户空间映射，例如 VDSO 页面、
   relay 通道页面等。这些页面天然不可逐出，也不在 LRU 链表上管理。
   __mm_populate() 会包含这些范围，在尚未填充的情况下填充 PTE。)

4) VMAs with VM_MIXEDMAP set are not marked VM_LOCKED, but __mm_populate()
   includes these ranges, populating the PTEs if not already populated.
   (设置了 VM_MIXEDMAP 的 VMA 不会被标记为 VM_LOCKED，但 __mm_populate() 会
   包含这些范围，在尚未填充的情况下填充 PTE。)

Note that for all of these special VMAs, mlock_fixup() does not set the
VM_LOCKED flag.  Therefore, we won't have to deal with them later during
munlock(), munmap() or task exit.  Neither does mlock_fixup() account these
VMAs against the task's "locked_vm".
(注意，对所有这些特殊 VMA，mlock_fixup() 都不会设置 VM_LOCKED 标志。因此，
之后在 munlock()、munmap() 或任务退出时，我们无需再处理它们。
mlock_fixup() 也不会把这些 VMA 计入任务的 "locked_vm"。)


munlock()/munlockall() System Call Handling(munlock()/munlockall() 系统调用的处理)
----------------------------------------------------------------------------------

The munlock() and munlockall() system calls are handled by the same
mlock_fixup() function as mlock(), mlock2() and mlockall() system calls are.
If called to munlock an already munlocked VMA, mlock_fixup() simply returns.
Because of the VMA filtering discussed above, VM_LOCKED will not be set in
any "special" VMAs.  So, those VMAs will be ignored for munlock.
(munlock() 与 munlockall() 系统调用由与 mlock()、mlock2()、mlockall() 系统调
用相同的 mlock_fixup() 函数处理。如果调用它去 munlock 一个已经 munlock 的
VMA，mlock_fixup() 直接返回。由于上文讨论的 VMA 过滤，任何"特殊" VMA 都不
会设置 VM_LOCKED。因此，munlock 时这些 VMA 会被忽略。)

If the VMA is VM_LOCKED, mlock_fixup() again attempts to merge or split off the
specified range.  All pages in the VMA are then munlocked by munlock_folio() via
mlock_pte_range() via walk_page_range() via mlock_vma_pages_range() - the same
function used when mlocking a VMA range, with new flags for the VMA indicating
that it is munlock() being performed.
(如果 VMA 是 VM_LOCKED，mlock_fixup() 再次尝试合并或分裂出指定范围。VMA 中
的所有页面随后由 munlock_folio() 解除 mlock，调用路径是
mlock_vma_pages_range() → walk_page_range() → mlock_pte_range() →
munlock_folio()——与 mlock 一个 VMA 范围时用的是同一个函数，只是 VMA 的新
标志表明这次执行的是 munlock()。)

munlock_folio() uses the mlock pagevec to batch up work to be done
under lru_lock by  __munlock_folio().  __munlock_folio() decrements the
folio's mlock_count, and when that reaches 0 it clears the mlocked flag
and clears the unevictable flag, moving the folio from unevictable state
to the inactive LRU.
(munlock_folio() 用 mlock pagevec 把工作攒起来，在 lru_lock 之下由
__munlock_folio() 完成。__munlock_folio() 递减 folio 的 mlock_count；当减到
0 时，清除 mlocked 标志与 unevictable 标志，把 folio 从不可逐出状态移到非
活跃 LRU。)

But in practice that may not work ideally: the folio may not yet have reached
"the unevictable LRU", or it may have been temporarily isolated from it.  In
those cases its mlock_count field is unusable and must be assumed to be 0: so
that the folio will be rescued to an evictable LRU, then perhaps be mlocked
again later if vmscan finds it in a VM_LOCKED VMA.
(但实践中这可能并不理想：folio 可能尚未到达"不可逐出 LRU"，或已被临时从
那里隔离。这些情况下它的 mlock_count 字段不可用，必须假定为 0：这样 folio
会被拯救到可逐出 LRU，之后若 vmscan 发现它位于某个 VM_LOCKED VMA 中，也许
会再次 mlock 它。)


Migrating MLOCKED Pages(迁移 MLOCKED 页面)
------------------------------------------

A page that is being migrated has been isolated from the LRU lists and is held
locked across unmapping of the page, updating the page's address space entry
and copying the contents and state, until the page table entry has been
replaced with an entry that refers to the new page.  Linux supports migration
of mlocked pages and other unevictable pages.  PG_mlocked is cleared from the
the old page when it is unmapped from the last VM_LOCKED VMA, and set when the
new page is mapped in place of migration entry in a VM_LOCKED VMA.  If the page
was unevictable because mlocked, PG_unevictable follows PG_mlocked; but if the
page was unevictable for other reasons, PG_unevictable is copied explicitly.
(被迁移的页面已从 LRU 链表隔离，并在解除页面映射、更新页面的地址空间条目、
拷贝内容与状态的整个过程中保持持锁，直到页表项被替换为指向新页面的条目。
Linux 支持迁移 mlocked 页面及其他不可逐出页面。当旧页面从最后一个
VM_LOCKED VMA 解除映射时，PG_mlocked 从旧页面清除；当新页面在 VM_LOCKED
VMA 中替代迁移条目被映射进来时，PG_mlocked 被设置。如果页面因 mlocked 而不
可逐出，PG_unevictable 跟随 PG_mlocked；如果页面因其他原因不可逐出，
PG_unevictable 会被显式拷贝。)

Note that page migration can race with mlocking or munlocking of the same page.
There is mostly no problem since page migration requires unmapping all PTEs of
the old page (including munlock where VM_LOCKED), then mapping in the new page
(including mlock where VM_LOCKED).  The page table locks provide sufficient
synchronization.
(注意，页面迁移可能与对同一页面的 mlock 或 munlock 竞争。这大体上没有问
题，因为页面迁移要求解除旧页面所有 PTE 的映射(VM_LOCKED 的地方包括
munlock)，然后映射进新页面(VM_LOCKED 的地方包括 mlock)。页表锁提供了足够
的同步。)

However, since mlock_vma_pages_range() starts by setting VM_LOCKED on a VMA,
before mlocking any pages already present, if one of those pages were migrated
before mlock_pte_range() reached it, it would get counted twice in mlock_count.
To prevent that, mlock_vma_pages_range() temporarily marks the VMA as VM_IO,
so that mlock_vma_folio() will skip it.
(不过，由于 mlock_vma_pages_range() 先在 VMA 上设置 VM_LOCKED，然后才
mlock 已存在的页面，如果其中某个页面在 mlock_pte_range() 到达它之前被迁
移，它在 mlock_count 中会被计两次。为防止这一点，
mlock_vma_pages_range() 会临时把 VMA 标记为 VM_IO，使 mlock_vma_folio() 跳
过它。)

To complete page migration, we place the old and new pages back onto the LRU
afterwards.  The "unneeded" page - old page on success, new page on failure -
is freed when the reference count held by the migration process is released.
(为了完成页面迁移，我们随后把旧页面与新页面放回 LRU。"不再需要"的那个页
面——成功时是旧页面，失败时是新页面——在迁移进程持有的引用计数被释放时
被释放。)


Compacting MLOCKED Pages(压缩 MLOCKED 页面)
-------------------------------------------

The memory map can be scanned for compactable regions and the default behavior
is to let unevictable pages be moved.  /proc/sys/vm/compact_unevictable_allowed
controls this behavior (see Documentation/admin-guide/sysctl/vm.rst).  The work
of compaction is mostly handled by the page migration code and the same work
flow as described in Migrating MLOCKED Pages will apply.
(可以扫描内存映射寻找可压缩区域，默认行为是允许移动不可逐出页面。
/proc/sys/vm/compact_unevictable_allowed 控制这一行为(见
Documentation/admin-guide/sysctl/vm.rst)。压缩的工作大部分由页面迁移代码
完成，并且适用 "迁移 MLOCKED 页面" 一节所述的同一工作流程。)


MLOCKING Transparent Huge Pages(MLOCKING 透明大页)
--------------------------------------------------

A transparent huge page is represented by a single entry on an LRU list.
Therefore, we can only make unevictable an entire compound page, not
individual subpages.
(一个透明大页在 LRU 链表上由单个条目表示。因此，我们只能把整个复合页
(compound page)变为不可逐出，而不是单个子页。)

If a user tries to mlock() part of a huge page, and no user mlock()s the
whole of the huge page, we want the rest of the page to be reclaimable.
(如果用户试图 mlock() 大页的一部分，而没有任何用户 mlock() 整个大页，我们
希望页面的其余部分仍可回收。)

We cannot just split the page on partial mlock() as split_huge_page() can
fail and a new intermittent failure mode for the syscall is undesirable.
(我们不能在部分 mlock() 时直接分裂页面，因为 split_huge_page() 可能失败，
而给系统调用增加一种新的间歇性失败模式是不可取的。)

We handle this by keeping PTE-mlocked huge pages on evictable LRU lists:
the PMD on the border of a VM_LOCKED VMA will be split into a PTE table.
(我们这样处理：把 PTE 级 mlock 的大页保留在可逐出 LRU 链表上：VM_LOCKED
VMA 边界上的 PMD 会被分裂成 PTE 表。)

This way the huge page is accessible for vmscan.  Under memory pressure the
page will be split, subpages which belong to VM_LOCKED VMAs will be moved
to the unevictable LRU and the rest can be reclaimed.
(这样 vmscan 就能访问大页。在内存压力下，页面会被分裂，属于 VM_LOCKED VMA
的子页会被移到不可逐出 LRU，其余的可以被回收。)

/proc/meminfo's Unevictable and Mlocked amounts do not include those parts
of a transparent huge page which are mapped only by PTEs in VM_LOCKED VMAs.
(/proc/meminfo 的 Unevictable 与 Mlocked 数值不包括透明大页中仅由
VM_LOCKED VMA 内的 PTE 映射的那些部分。)


mmap(MAP_LOCKED) System Call Handling(mmap(MAP_LOCKED) 系统调用的处理)
----------------------------------------------------------------------

In addition to the mlock(), mlock2() and mlockall() system calls, an application
can request that a region of memory be mlocked by supplying the MAP_LOCKED flag
to the mmap() call.  There is one important and subtle difference here, though.
mmap() + mlock() will fail if the range cannot be faulted in (e.g. because
mm_populate fails) and returns with ENOMEM while mmap(MAP_LOCKED) will not fail.
The mmapped area will still have properties of the locked area - pages will not
get swapped out - but major page faults to fault memory in might still happen.
(除了 mlock()、mlock2() 与 mlockall() 系统调用之外，应用还可以通过向 mmap()
调用提供 MAP_LOCKED 标志来请求 mlock 一段内存。不过这里有一个重要而细微的
区别：mmap() + mlock() 在范围无法缺页调入时(例如 mm_populate 失败)会失败并
返回 ENOMEM，而 mmap(MAP_LOCKED) 不会失败。被 mmap 的区域仍将具有锁定区域
的性质——页面不会被换出——但把内存调入的主缺页 (major page fault) 仍可能
发生。)

Furthermore, any mmap() call or brk() call that expands the heap by a task
that has previously called mlockall() with the MCL_FUTURE flag will result
in the newly mapped memory being mlocked.  Before the unevictable/mlock
changes, the kernel simply called make_pages_present() to allocate pages
and populate the page table.
(此外，已用 MCL_FUTURE 标志调用过 mlockall() 的任务，任何扩展堆的 mmap()
调用或 brk() 调用都会导致新映射的内存被 mlock。在不可逐出/mlock 改动之前，
内核只是简单调用 make_pages_present() 来分配页面并填充页表。)

To mlock a range of memory under the unevictable/mlock infrastructure,
the mmap() handler and task address space expansion functions call
populate_vma_page_range() specifying the vma and the address range to mlock.
(在不可逐出/mlock 基础设施之下 mlock 一段内存时，mmap() 处理函数与任务地
址空间扩展函数会调用 populate_vma_page_range()，指定要 mlock 的 vma 与地
址范围。)


munmap()/exit()/exec() System Call Handling(munmap()/exit()/exec() 系统调用的处理)
----------------------------------------------------------------------------------

When unmapping an mlocked region of memory, whether by an explicit call to
munmap() or via an internal unmap from exit() or exec() processing, we must
munlock the pages if we're removing the last VM_LOCKED VMA that maps the pages.
Before the unevictable/mlock changes, mlocking did not mark the pages in any
way, so unmapping them required no processing.
(解除一段 mlocked 内存的映射时——无论是显式调用 munmap()，还是 exit() 或
exec() 处理中的内部解除映射——如果我们移除的是最后一个映射这些页面的
VM_LOCKED VMA，就必须 munlock 这些页面。在不可逐出/mlock 改动之前，mlock
不以任何方式标记页面，因此解除映射无需任何处理。)

For each PTE (or PMD) being unmapped from a VMA, folio_remove_rmap_*() calls
munlock_vma_folio(), which calls munlock_folio() when the VMA is VM_LOCKED
(unless it was a PTE mapping of a part of a transparent huge page).
(对每个从 VMA 解除映射的 PTE(或 PMD)，folio_remove_rmap_*() 会调用
munlock_vma_folio()；当 VMA 是 VM_LOCKED 时，它会调用 munlock_folio()(除非
这是透明大页一部分的 PTE 映射)。)

munlock_folio() uses the mlock pagevec to batch up work to be done
under lru_lock by  __munlock_folio().  __munlock_folio() decrements the
folio's mlock_count, and when that reaches 0 it clears the mlocked flag
and clears the unevictable flag, moving the folio from unevictable state
to the inactive LRU.
(munlock_folio() 用 mlock pagevec 把工作攒起来，在 lru_lock 之下由
__munlock_folio() 完成。__munlock_folio() 递减 folio 的 mlock_count；当减到
0 时，清除 mlocked 标志与 unevictable 标志，把 folio 从不可逐出状态移到非
活跃 LRU。)

But in practice that may not work ideally: the folio may not yet have reached
"the unevictable LRU", or it may have been temporarily isolated from it.  In
those cases its mlock_count field is unusable and must be assumed to be 0: so
that the folio will be rescued to an evictable LRU, then perhaps be mlocked
again later if vmscan finds it in a VM_LOCKED VMA.
(但实践中这可能并不理想：folio 可能尚未到达"不可逐出 LRU"，或已被临时从
那里隔离。这些情况下它的 mlock_count 字段不可用，必须假定为 0：这样 folio
会被拯救到可逐出 LRU，之后若 vmscan 发现它位于某个 VM_LOCKED VMA 中，也许
会再次 mlock 它。)


Truncating MLOCKED Pages(截断 MLOCKED 页面)
-------------------------------------------

File truncation or hole punching forcibly unmaps the deleted pages from
userspace; truncation even unmaps and deletes any private anonymous pages
which had been Copied-On-Write from the file pages now being truncated.
(文件截断或打洞会强制从用户空间解除被删页面的映射；截断甚至会解除映射并删
除那些从正被截断的文件页 COW 而来的私有匿名页。)

Mlocked pages can be munlocked and deleted in this way: like with munmap(),
for each PTE (or PMD) being unmapped from a VMA, folio_remove_rmap_*() calls
munlock_vma_folio(), which calls munlock_folio() when the VMA is VM_LOCKED
(unless it was a PTE mapping of a part of a transparent huge page).
(mlocked 页面也可以这样被 munlock 并删除：与 munmap() 一样，对每个从 VMA
解除映射的 PTE(或 PMD)，folio_remove_rmap_*() 会调用 munlock_vma_folio()；
当 VMA 是 VM_LOCKED 时，它会调用 munlock_folio()(除非这是透明大页一部分的
PTE 映射)。)

However, if there is a racing munlock(), since mlock_vma_pages_range() starts
munlocking by clearing VM_LOCKED from a VMA, before munlocking all the pages
present, if one of those pages were unmapped by truncation or hole punch before
mlock_pte_range() reached it, it would not be recognized as mlocked by this VMA,
and would not be counted out of mlock_count.  In this rare case, a page may
still appear as PG_mlocked after it has been fully unmapped: and it is left to
release_pages() (or __page_cache_release()) to clear it and update statistics
before freeing (this event is counted in /proc/vmstat unevictable_pgs_cleared,
which is usually 0).
(不过，如果存在竞争的 munlock()：由于 mlock_vma_pages_range() 是先从 VMA 清
除 VM_LOCKED 开始 munlock，然后才 munlock 所有已存在的页面，如果其中某个页
面在 mlock_pte_range() 到达它之前就被截断或打洞解除映射，它就不会被识别为
被这个 VMA mlock，也就不会从 mlock_count 中减掉。在这种罕见情况下，页面在
被完全解除映射之后可能仍显示为 PG_mlocked：这就留给 release_pages()(或
__page_cache_release())在释放之前清除它并更新统计(该事件计入 /proc/vmstat
的 unevictable_pgs_cleared，通常为 0)。)


Page Reclaim in shrink_*_list()(shrink_*_list() 中的页面回收)
-------------------------------------------------------------

vmscan's shrink_active_list() culls any obviously unevictable pages -
i.e. !page_evictable(page) pages - diverting those to the unevictable list.
However, shrink_active_list() only sees unevictable pages that made it onto the
active/inactive LRU lists.  Note that these pages do not have PG_unevictable
set - otherwise they would be on the unevictable list and shrink_active_list()
would never see them.
(vmscan 的 shrink_active_list() 会剔除任何明显不可逐出的页面——即
!page_evictable(page) 的页面——把它们转移到不可逐出链表。然而，
shrink_active_list() 只能看到那些进入了活跃/非活跃 LRU 链表的不可逐出页
面。注意这些页面没有设置 PG_unevictable——否则它们会在不可逐出链表上，
shrink_active_list() 永远看不到它们。)

Some examples of these unevictable pages on the LRU lists are:
(LRU 链表上这类不可逐出页面的一些例子：)

 (1) ramfs pages that have been placed on the LRU lists when first allocated.
     (ramfs 页面在首次分配时就被放上了 LRU 链表。)

 (2) SHM_LOCK'd shared memory pages.  shmctl(SHM_LOCK) does not attempt to
     allocate or fault in the pages in the shared memory region.  This happens
     when an application accesses the page the first time after SHM_LOCK'ing
     the segment.
     (SHM_LOCK 过的共享内存页面。shmctl(SHM_LOCK) 并不尝试分配或把共享内存
     区域中的页面调入，这发生在应用程序在 SHM_LOCK 该段之后第一次访问页面
     时。)

 (3) pages still mapped into VM_LOCKED VMAs, which should be marked mlocked,
     but events left mlock_count too low, so they were munlocked too early.
     (仍被映射进 VM_LOCKED VMA 的页面，它们本应被标记为 mlocked，但一些事件
     使 mlock_count 偏低，导致它们过早被 munlock。)

vmscan's shrink_inactive_list() and shrink_folio_list() also divert obviously
unevictable pages found on the inactive lists to the appropriate memory cgroup
and node unevictable list.
(vmscan 的 shrink_inactive_list() 与 shrink_folio_list() 也会把在非活跃链
表上发现的明显不可逐出页面转移到相应内存 cgroup 与节点的不可逐出链表。)

rmap's folio_referenced_one(), called via vmscan's shrink_active_list() or
shrink_folio_list(), and rmap's try_to_unmap_one() called via shrink_folio_list(),
check for (3) pages still mapped into VM_LOCKED VMAs, and call mlock_vma_folio()
to correct them.  Such pages are culled to the unevictable list when released
by the shrinker.
(rmap 的 folio_referenced_one()——经由 vmscan 的 shrink_active_list() 或
shrink_folio_list() 调用——以及 rmap 的 try_to_unmap_one()——经由
shrink_folio_list() 调用——会检查第 (3) 类仍被映射进 VM_LOCKED VMA 的页
面，并调用 mlock_vma_folio() 加以纠正。这类页面在 shrinker 释放时被剔除到
不可逐出链表。)
