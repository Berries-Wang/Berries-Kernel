===============================
Split page table lock(分页表锁)
===============================

Originally, mm->page_table_lock spinlock protected all page tables of the
mm_struct. But this approach leads to poor page fault scalability of
multi-threaded applications due to high contention on the lock. To improve
scalability, split page table lock was introduced.
(最初，mm->page_table_lock 自旋锁保护 mm_struct 的所有页表。但由于
锁上的高竞争，这种做法导致多线程应用的缺页中断可扩展性很差。为了
改善可扩展性，引入了分页表锁(split page table lock)。)

With split page table lock we have separate per-table lock to serialize
access to the table. At the moment we use split lock for PTE and PMD
tables. Access to higher level tables protected by mm->page_table_lock.
(使用分页表锁后，每张表都有独立的锁来串行化对它的访问。目前我们对
PTE 和 PMD 表使用分立锁。对更高级别表的访问由 mm->page_table_lock
保护。)

There are helpers to lock/unlock a table and other accessor functions:
(有一些用于加锁/解锁表以及其他访问功能的辅助函数：)

 - pte_offset_map_lock()
	maps PTE and takes PTE table lock, returns pointer to PTE with
	pointer to its PTE table lock, or returns NULL if no PTE table;
        (映射 PTE 并获取 PTE 表锁，返回指向 PTE 的指针及指向其 PTE 表
        锁的指针；如果没有 PTE 表，则返回 NULL；)
 - pte_offset_map_ro_nolock()
	maps PTE, returns pointer to PTE with pointer to its PTE table
	lock (not taken), or returns NULL if no PTE table;
        (映射 PTE，返回指向 PTE 的指针及指向其 PTE 表锁的指针(未加
        锁)；如果没有 PTE 表，则返回 NULL；)
 - pte_offset_map_rw_nolock()
	maps PTE, returns pointer to PTE with pointer to its PTE table
	lock (not taken) and the value of its pmd entry, or returns NULL
	if no PTE table;
        (映射 PTE，返回指向 PTE 的指针、指向其 PTE 表锁的指针(未加
        锁)以及其 pmd 项的值；如果没有 PTE 表，则返回 NULL；)
 - pte_offset_map()
	maps PTE, returns pointer to PTE, or returns NULL if no PTE table;
        (映射 PTE，返回指向 PTE 的指针；如果没有 PTE 表，则返回 NULL；)
 - pte_unmap()
	unmaps PTE table;
        (取消映射 PTE 表；)
 - pte_unmap_unlock()
	unlocks and unmaps PTE table;
        (解锁并取消映射 PTE 表；)
 - pte_alloc_map_lock()
	allocates PTE table if needed and takes its lock, returns pointer to
	PTE with pointer to its lock, or returns NULL if allocation failed;
        (如有需要则分配 PTE 表并获取其锁，返回指向 PTE 的指针及指向
        其锁的指针；如果分配失败，则返回 NULL；)
 - pmd_lock()
	takes PMD table lock, returns pointer to taken lock;
        (获取 PMD 表锁，返回指向已获取锁的指针；)
 - pmd_lockptr()
	returns pointer to PMD table lock;
        (返回指向 PMD 表锁的指针；)

Split page table lock for PTE tables is enabled compile-time if
CONFIG_SPLIT_PTLOCK_CPUS (usually 4) is less or equal to NR_CPUS.
If split lock is disabled, all tables are guarded by mm->page_table_lock.
(当 CONFIG_SPLIT_PTLOCK_CPUS(通常为 4)小于等于 NR_CPUS 时，PTE 表
的分页表锁在编译期启用。如果分立锁被禁用，所有表都由
mm->page_table_lock 保护。)

Split page table lock for PMD tables is enabled, if it's enabled for PTE
tables and the architecture supports it (see below).
(PMD 表的分页表锁在 PTE 表的分页表锁已启用且架构支持它(见下文)时
启用。)

Hugetlb and split page table lock(Hugetlb 与分页表锁)
=====================================================

Hugetlb can support several page sizes. We use split lock only for PMD
level, but not for PUD.
(Hugetlb 可以支持多种页大小。我们只对 PMD 级别使用分立锁，而不用于
PUD。)

Hugetlb-specific helpers:
(Hugetlb 专用的辅助函数：)

 - huge_pte_lock()
	takes pmd split lock for PMD_SIZE page, mm->page_table_lock
	otherwise;
        (对 PMD_SIZE 页获取 pmd 分立锁，否则获取 mm->page_table_lock；)
 - huge_pte_lockptr()
	returns pointer to table lock;
        (返回指向表锁的指针；)

Support of split page table lock by an architecture(架构对分页表锁的支持)
=========================================================================

There's no need in special enabling of PTE split page table lock: everything
required is done by pagetable_pte_ctor() and pagetable_dtor(), which
must be called on PTE table allocation / freeing.
(无需对 PTE 分页表锁做特别的启用：所需的一切都由
pagetable_pte_ctor() 和 pagetable_dtor() 完成，它们必须在 PTE 表
分配/释放时被调用。)

Make sure the architecture doesn't use slab allocator for page table
allocation: slab uses page->slab_cache for its pages.
This field shares storage with page->ptl.
(请确保架构不使用 slab 分配器来分配页表：slab 对其页使用
page->slab_cache。该字段与 page->ptl 共享存储。)

PMD split lock only makes sense if you have more than two page table
levels.
(只有当页表级别超过两级时，PMD 分立锁才有意义。)

PMD split lock enabling requires pagetable_pmd_ctor() call on PMD table
allocation and pagetable_dtor() on freeing.
(启用 PMD 分立锁要求在 PMD 表分配时调用 pagetable_pmd_ctor()，在
释放时调用 pagetable_dtor()。)

Allocation usually happens in pmd_alloc_one(), freeing in pmd_free() and
pmd_free_tlb(), but make sure you cover all PMD table allocation / freeing
paths: i.e X86_PAE preallocate few PMDs on pgd_alloc().
(分配通常发生在 pmd_alloc_one() 中，释放在 pmd_free() 和
pmd_free_tlb() 中，但请确保覆盖所有 PMD 表的分配/释放路径：例如
X86_PAE 会在 pgd_alloc() 中预分配几个 PMD。)

With everything in place you can set CONFIG_ARCH_ENABLE_SPLIT_PMD_PTLOCK.
(一切就绪后，你就可以设置 CONFIG_ARCH_ENABLE_SPLIT_PMD_PTLOCK。)

NOTE: pagetable_pte_ctor() and pagetable_pmd_ctor() can fail -- it must
be handled properly.
(注意：pagetable_pte_ctor() 和 pagetable_pmd_ctor() 可能失败——必须
妥善处理。)

page->ptl(page->ptl 字段)
=========================

page->ptl is used to access split page table lock, where 'page' is struct
page of page containing the table. It shares storage with page->private
(and few other fields in union).
(page->ptl 用于访问分页表锁，其中 'page' 是包含该表的页的
struct page。它与 page->private 共享存储(以及联合体中的其他几个
字段)。)

To avoid increasing size of struct page and have best performance, we use a
trick:
(为了避免增大 struct page 的尺寸并获得最佳性能，我们使用了一个
技巧：)

 - if spinlock_t fits into long, we use page->ptr as spinlock, so we
   can avoid indirect access and save a cache line.
   (如果 spinlock_t 能放进 long，我们就把 page->ptr 当作自旋锁使用，
   从而避免间接访问并节省一条缓存行。)
 - if size of spinlock_t is bigger then size of long, we use page->ptl as
   pointer to spinlock_t and allocate it dynamically. This allows to use
   split lock with enabled DEBUG_SPINLOCK or DEBUG_LOCK_ALLOC, but costs
   one more cache line for indirect access;
   (如果 spinlock_t 的大小超过 long 的大小，我们就把 page->ptl 用作
   指向 spinlock_t 的指针并动态分配它。这使得在启用 DEBUG_SPINLOCK 或
   DEBUG_LOCK_ALLOC 的情况下也能使用分立锁，但间接访问要多付出一条
   缓存行；)

The spinlock_t allocated in pagetable_pte_ctor() for PTE table and in
pagetable_pmd_ctor() for PMD table.
(spinlock_t 在 pagetable_pte_ctor() 中为 PTE 表分配，在
pagetable_pmd_ctor() 中为 PMD 表分配。)

Please, never access page->ptl directly -- use appropriate helper.
(请勿直接访问 page->ptl——请使用合适的辅助函数。)
