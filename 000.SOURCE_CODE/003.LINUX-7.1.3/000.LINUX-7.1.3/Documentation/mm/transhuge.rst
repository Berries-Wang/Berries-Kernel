==========================================
Transparent Hugepage Support(透明大页支持)
==========================================

This document describes design principles for Transparent Hugepage (THP)
support and its interaction with other parts of the memory management
system.
(本文档描述透明大页 (THP) 支持的设计原则，以及它与内存管理系统其他部分的交互。)

Design principles(设计原则)
===========================

- "graceful fallback": mm components which don't have transparent hugepage
  knowledge fall back to breaking huge pmd mapping into table of ptes and,
  if necessary, split a transparent hugepage. Therefore these components
  can continue working on the regular pages or regular pte mappings.
  ("优雅回退"：不了解透明大页的 mm 组件会回退为把大 pmd 映射打散成 pte 表，并在必要时拆分透明大页。因此这些组件可以继续在常规页或常规 pte 映射上工作。)

- if a hugepage allocation fails because of memory fragmentation,
  regular pages should be gracefully allocated instead and mixed in
  the same vma without any failure or significant delay and without
  userland noticing
  (如果因内存碎片化导致大页分配失败，应改为优雅地分配常规页，并混入同一个 vma 中，不产生任何失败或显著延迟，用户态也察觉不到)

- if some task quits and more hugepages become available (either
  immediately in the buddy or through the VM), guest physical memory
  backed by regular pages should be relocated on hugepages
  automatically (with khugepaged)
  (如果某些任务退出后更多大页变得可用(无论是在 buddy 中立即可用，还是通过 VM 释放)，由常规页支撑的虚拟机物理内存应当(借助 khugepaged)自动迁移到大页上)

- it doesn't require memory reservation and in turn it uses hugepages
  whenever possible (the only possible reservation here is kernelcore=
  to avoid unmovable pages to fragment all the memory but such a tweak
  is not specific to transparent hugepage support and it's a generic
  feature that applies to all dynamic high order allocations in the
  kernel)
  (它不需要内存预留，反而会尽可能使用大页(这里唯一可能的预留是 kernelcore=，用于避免不可移动页把内存碎片化，但这一调整并非透明大页支持特有，而是适用于内核中所有动态高阶分配的通用特性))

get_user_pages and pin_user_pages(get_user_pages 与 pin_user_pages)
===================================================================

get_user_pages and pin_user_pages if run on a hugepage, will return the
head or tail pages as usual (exactly as they would do on
hugetlbfs). Most GUP users will only care about the actual physical
address of the page and its temporary pinning to release after the I/O
is complete, so they won't ever notice the fact the page is huge. But
if any driver is going to mangle over the page structure of the tail
page (like for checking page->mapping or other bits that are relevant
for the head page and not the tail page), it should be updated to jump
to check head page instead. Taking a reference on any head/tail page would
prevent the page from being split by anyone.
(get_user_pages 与 pin_user_pages 在大页上运行时，会照常返回 head 或 tail 页(与它们在 hugetlbfs 上的行为完全相同)。大多数 GUP 用户只关心页的实际物理地址以及 I/O 完成后释放的临时固定 (pinning)，因此永远不会注意到页是大页这一事实。但如果任何驱动要改动 tail 页的 page 结构(比如检查 page->mapping 或其他只与 head 页相关而与 tail 页无关的位)，则应将其更新为改为检查 head 页。对任何 head/tail 页持有引用都会阻止该页被任何人拆分。)

.. note::
   these aren't new constraints to the GUP API, and they match the
   same constraints that apply to hugetlbfs too, so any driver capable
   of handling GUP on hugetlbfs will also work fine on transparent
   hugepage backed mappings.
   (这些对 GUP API 来说并不是新约束，它们与同样适用于 hugetlbfs 的约束一致，因此任何能够在 hugetlbfs 上处理 GUP 的驱动，在透明大页支撑的映射上也能正常工作。)

Graceful fallback(优雅回退)
===========================

Code walking pagetables but unaware about huge pmds can simply call
split_huge_pmd(vma, pmd, addr) where the pmd is the one returned by
pmd_offset. It's trivial to make the code transparent hugepage aware
by just grepping for "pmd_offset" and adding split_huge_pmd where
missing after pmd_offset returns the pmd. Thanks to the graceful
fallback design, with a one liner change, you can avoid to write
hundreds if not thousands of lines of complex code to make your code
hugepage aware.
(遍历页表但不了解大 pmd 的代码，可以简单地调用 split_huge_pmd(vma, pmd, addr)，其中 pmd 就是 pmd_offset 返回的那个。只需 grep "pmd_offset"，在 pmd_offset 返回 pmd 后补上缺失的 split_huge_pmd，就能轻而易举地让代码具备透明大页感知能力。多亏优雅回退的设计，只需一行代码的改动，就能免于编写成百上千行复杂代码来让代码支持大页。)

If you're not walking pagetables but you run into a physical hugepage
that you can't handle natively in your code, you can split it by
calling split_huge_page(page). This is what the Linux VM does before
it tries to swapout the hugepage for example. split_huge_page() can fail
if the page is pinned and you must handle this correctly.
(如果你没有在遍历页表，却在代码中遇到一个无法原生处理的物理大页，可以通过调用 split_huge_page(page) 将其拆分。例如 Linux VM 在尝试换出大页之前就是这么做的。如果页已被固定 (pinned)，split_huge_page() 可能失败，你必须正确处理这种情况。)

Example to make mremap.c transparent hugepage aware with a one liner
change::

	diff --git a/mm/mremap.c b/mm/mremap.c
	--- a/mm/mremap.c
	+++ b/mm/mremap.c
	@@ -41,6 +41,7 @@ static pmd_t *get_old_pmd(struct mm_stru
		return NULL;

		pmd = pmd_offset(pud, addr);
	+	split_huge_pmd(vma, pmd, addr);
		if (pmd_none_or_clear_bad(pmd))
			return NULL;

(用一行改动让 mremap.c 具备透明大页感知能力的示例：)

Locking in hugepage aware code(大页感知代码中的加锁)
====================================================

We want as much code as possible hugepage aware, as calling
split_huge_page() or split_huge_pmd() has a cost.
(我们希望尽可能多的代码具备大页感知能力，因为调用 split_huge_page() 或 split_huge_pmd() 是有代价的。)

To make pagetable walks huge pmd aware, all you need to do is to call
pmd_trans_huge() on the pmd returned by pmd_offset. You must hold the
mmap_lock in read (or write) mode to be sure a huge pmd cannot be
created from under you by khugepaged (khugepaged collapse_huge_page
takes the mmap_lock in write mode in addition to the anon_vma lock). If
pmd_trans_huge returns false, you just fallback in the old code
paths. If instead pmd_trans_huge returns true, you have to take the
page table lock (pmd_lock()) and re-run pmd_trans_huge. Taking the
page table lock will prevent the huge pmd being converted into a
regular pmd from under you (split_huge_pmd can run in parallel to the
pagetable walk). If the second pmd_trans_huge returns false, you
should just drop the page table lock and fallback to the old code as
before. Otherwise, you can proceed to process the huge pmd and the
hugepage natively. Once finished, you can drop the page table lock.
(要让页表遍历具备大 pmd 感知能力，只需对 pmd_offset 返回的 pmd 调用 pmd_trans_huge()。你必须以读(或写)模式持有 mmap_lock，以确保 khugepaged 不会在你背后创建大 pmd(khugepaged 的 collapse_huge_page 除了 anon_vma 锁之外还会以写模式获取 mmap_lock)。如果 pmd_trans_huge 返回 false，你只需回退到旧的代码路径。反之若 pmd_trans_huge 返回 true，你必须获取页表锁 (pmd_lock()) 并重新运行 pmd_trans_huge。获取页表锁可以防止大 pmd 在你背后被转换成常规 pmd(split_huge_pmd 可能与页表遍历并行运行)。如果第二次 pmd_trans_huge 返回 false，你应像之前那样释放页表锁并回退到旧代码。否则，你就可以继续以原生方式处理大 pmd 和大页。处理完成后，即可释放页表锁。)

Refcounts and transparent huge pages(引用计数与透明大页)
========================================================

Refcounting on THP is mostly consistent with refcounting on other compound
pages:
(THP 上的引用计数与其他复合页上的引用计数基本一致：)

  - get_page()/put_page() and GUP operate on the folio->_refcount.
    (get_page()/put_page() 与 GUP 作用于 folio->_refcount。)

  - ->_refcount in tail pages is always zero: get_page_unless_zero() never
    succeeds on tail pages.
    (tail 页中的 ->_refcount 始终为零：get_page_unless_zero() 在 tail 页上永远不会成功。)

  - map/unmap of a PMD entry for the whole THP increment/decrement
    folio->_entire_mapcount and folio->_large_mapcount.
    (对整个 THP 的 PMD 表项进行 map/unmap 会递增/递减 folio->_entire_mapcount 与 folio->_large_mapcount。)

    We also maintain the two slots for tracking MM owners (MM ID and
    corresponding mapcount), and the current status ("maybe mapped shared" vs.
    "mapped exclusively").

    With CONFIG_PAGE_MAPCOUNT, we also increment/decrement
    folio->_nr_pages_mapped by ENTIRELY_MAPPED when _entire_mapcount goes
    from -1 to 0 or 0 to -1.
    (我们还维护两个用于跟踪 MM 所有者的槽位(MM ID 及对应的 mapcount)，以及当前状态("可能以共享方式映射"还是"以独占方式映射")。

    在启用 CONFIG_PAGE_MAPCOUNT 时，当 _entire_mapcount 从 -1 变为 0 或从 0 变为 -1，我们还会把 folio->_nr_pages_mapped 增/减 ENTIRELY_MAPPED。)

  - map/unmap of individual pages with PTE entry increment/decrement
    folio->_large_mapcount.
    (用 PTE 表项对单个页进行 map/unmap 会递增/递减 folio->_large_mapcount。)

    We also maintain the two slots for tracking MM owners (MM ID and
    corresponding mapcount), and the current status ("maybe mapped shared" vs.
    "mapped exclusively").

    With CONFIG_PAGE_MAPCOUNT, we also increment/decrement
    page->_mapcount and increment/decrement folio->_nr_pages_mapped when
    page->_mapcount goes from -1 to 0 or 0 to -1 as this counts the number
    of pages mapped by PTE.
    (我们还维护两个用于跟踪 MM 所有者的槽位(MM ID 及对应的 mapcount)，以及当前状态("可能以共享方式映射"还是"以独占方式映射")。

    在启用 CONFIG_PAGE_MAPCOUNT 时，当 page->_mapcount 从 -1 变为 0 或从 0 变为 -1(它统计按 PTE 映射的页数)，我们还会增/减 page->_mapcount 与 folio->_nr_pages_mapped。)

split_huge_page internally has to distribute the refcounts in the head
page to the tail pages before clearing all PG_head/tail bits from the page
structures. It can be done easily for refcounts taken by page table
entries, but we don't have enough information on how to distribute any
additional pins (i.e. from get_user_pages). split_huge_page() fails any
requests to split pinned huge pages: it expects page count to be equal to
the sum of mapcount of all sub-pages plus one (split_huge_page caller must
have a reference to the head page).
(split_huge_page 在清除页结构中的所有 PG_head/tail 位之前，必须先把 head 页中的引用计数分摊到 tail 页上。对由页表项获取的引用计数来说这很容易做到，但对于额外的固定 (pin)(即来自 get_user_pages 的)如何分摊，我们没有足够的信息。split_huge_page() 对拆分已固定大页的任何请求都会返回失败：它要求 page count 等于所有子页 mapcount 之和再加一(split_huge_page 的调用者必须持有对 head 页的引用)。)

split_huge_page uses migration entries to stabilize page->_refcount and
page->_mapcount of anonymous pages. File pages just get unmapped.
(split_huge_page 使用迁移表项来稳定匿名页的 page->_refcount 与 page->_mapcount。文件页则直接被解除映射。)

We are safe against physical memory scanners too: the only legitimate way
a scanner can get a reference to a page is get_page_unless_zero().
(我们对物理内存扫描器也是安全的：扫描器获得页引用的唯一合法途径是 get_page_unless_zero()。)

All tail pages have zero ->_refcount until atomic_add(). This prevents the
scanner from getting a reference to the tail page up to that point. After the
atomic_add() we don't care about the ->_refcount value. We already know how
many references should be uncharged from the head page.
(在 atomic_add() 之前，所有 tail 页的 ->_refcount 都为零。这防止扫描器在此之前获得对 tail 页的引用。在 atomic_add() 之后，我们不再关心 ->_refcount 的值。我们已经知道应从 head 页上扣减多少引用。)

For head page get_page_unless_zero() will succeed and we don't mind. It's
clear where references should go after split: it will stay on the head page.
(对 head 页而言 get_page_unless_zero() 会成功，我们并不在意。拆分后引用应归属何处是明确的：它将留在 head 页上。)

Note that split_huge_pmd() doesn't have any limitations on refcounting:
pmd can be split at any point and never fails.
(注意，split_huge_pmd() 在引用计数方面没有任何限制：pmd 可以在任何时点被拆分，且从不失败。)

Partial unmap and deferred_split_folio() (anon THP only)(部分解除映射与 deferred_split_folio()(仅限匿名 THP))
=============================================================================================================

Unmapping part of THP (with munmap() or other way) is not going to free
memory immediately. Instead, we detect that a subpage of THP is not in use
in folio_remove_rmap_*() and queue the THP for splitting if memory pressure
comes. Splitting will free up unused subpages.
(解除 THP 的部分映射(通过 munmap() 或其他方式)不会立即释放内存。相反，我们在 folio_remove_rmap_*() 中检测到 THP 的某个子页不再被使用，并在内存压力出现时把该 THP 排入待拆分队列。拆分会释放未使用的子页。)

Splitting the page right away is not an option due to locking context in
the place where we can detect partial unmap. It also might be
counterproductive since in many cases partial unmap happens during exit(2) if
a THP crosses a VMA boundary.
(由于检测到部分解除映射的位置所处的加锁上下文，立即拆分页并不可行。而且这还可能适得其反，因为当 THP 跨越 VMA 边界时，部分解除映射常常发生在 exit(2) 期间。)

The function deferred_split_folio() is used to queue a folio for splitting.
The splitting itself will happen when we get memory pressure via shrinker
interface.
(函数 deferred_split_folio() 用于把 folio 排入待拆分队列。拆分本身会在通过 shrinker 接口感受到内存压力时进行。)

With CONFIG_PAGE_MAPCOUNT, we reliably detect partial mappings based on
folio->_nr_pages_mapped.
(启用 CONFIG_PAGE_MAPCOUNT 时，我们基于 folio->_nr_pages_mapped 可靠地检测部分映射。)

With CONFIG_NO_PAGE_MAPCOUNT, we detect partial mappings based on the
average per-page mapcount in a THP: if the average is < 1, an anon THP is
certainly partially mapped. As long as only a single process maps a THP,
this detection is reliable. With long-running child processes, there can
be scenarios where partial mappings can currently not be detected, and
might need asynchronous detection during memory reclaim in the future.
(启用 CONFIG_NO_PAGE_MAPCOUNT 时，我们基于 THP 中每页的平均 mapcount 来检测部分映射：如果平均值 < 1，匿名 THP 必然被部分映射。只要只有一个进程映射某个 THP，这种检测就是可靠的。在存在长期运行的子进程时，可能出现目前无法检测到部分映射的场景，将来可能需要在内存回收期间进行异步检测。)
