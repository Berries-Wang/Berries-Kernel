=============================================================
Cache and TLB Flushing Under Linux(Linux 下的缓存与 TLB 刷新)
=============================================================

:Author: David S. Miller <davem@redhat.com>
(作者: David S. Miller <davem@redhat.com>)

This document describes the cache/tlb flushing interfaces called
by the Linux VM subsystem.  It enumerates over each interface,
describes its intended purpose, and what side effect is expected
after the interface is invoked.
(本文档描述 Linux VM 子系统调用的缓存/TLB 刷新接口。它逐个枚举这些接口，说明其预期用途，以及调用该接口后预期产生的副作用。)

The side effects described below are stated for a uniprocessor
implementation, and what is to happen on that single processor.  The
SMP cases are a simple extension, in that you just extend the
definition such that the side effect for a particular interface occurs
on all processors in the system.  Don't let this scare you into
thinking SMP cache/tlb flushing must be so inefficient, this is in
fact an area where many optimizations are possible.  For example,
if it can be proven that a user address space has never executed
on a cpu (see mm_cpumask()), one need not perform a flush
for this address space on that cpu.
(下面描述的副作用是针对单处理器实现、以及在该单处理器上要发生什么而言的。SMP 情形只是简单的扩展：只需扩展定义，使某个接口的副作用发生在系统中所有处理器上即可。不要因此以为 SMP 缓存/TLB 刷新注定效率低下——事实上这是一个可以做出大量优化的领域。例如，如果能证明某个用户地址空间从未在某个 CPU 上执行过(参见 mm_cpumask())，就无需在该 CPU 上针对这个地址空间执行刷新。)

First, the TLB flushing interfaces, since they are the simplest.  The
"TLB" is abstracted under Linux as something the cpu uses to cache
virtual-->physical address translations obtained from the software
page tables.  Meaning that if the software page tables change, it is
possible for stale translations to exist in this "TLB" cache.
Therefore when software page table changes occur, the kernel will
invoke one of the following flush methods _after_ the page table
changes occur:
(首先是 TLB 刷新接口，因为它们最简单。在 Linux 中，"TLB" 被抽象为 CPU 用来缓存从软件页表获得的"虚拟地址→物理地址"转换的部件。这意味着，如果软件页表发生变化，这个 "TLB" 缓存中就可能存在过期 (stale) 的转换。因此，当软件页表发生变化时，内核会在页表变化发生 *之后* 调用以下刷新方法之一：)

1) ``void flush_tlb_all(void)``

	The most severe flush of all.  After this interface runs,
	any previous page table modification whatsoever will be
	visible to the cpu.

	This is usually invoked when the kernel page tables are
	changed, since such translations are "global" in nature.
	(最彻底的刷新。此接口运行后，CPU 对此前任何页表修改都可见。

	它通常在内核页表被修改时调用，因为这类转换本质上是"全局"的。)

2) ``void flush_tlb_mm(struct mm_struct *mm)``

	This interface flushes an entire user address space from
	the TLB.  After running, this interface must make sure that
	any previous page table modifications for the address space
	'mm' will be visible to the cpu.  That is, after running,
	there will be no entries in the TLB for 'mm'.

	This interface is used to handle whole address space
	page table operations such as what happens during
	fork, and exec.
	(此接口把整个用户地址空间从 TLB 中刷新掉。运行之后，此接口必须确保地址空间 'mm' 此前的任何页表修改对 CPU 可见。也就是说，运行之后，TLB 中将不再有 'mm' 的条目。

	此接口用于处理整个地址空间层面的页表操作，例如 fork 与 exec 期间发生的操作。)

3) ``void flush_tlb_range(struct vm_area_struct *vma,
   unsigned long start, unsigned long end)``

	Here we are flushing a specific range of (user) virtual
	address translations from the TLB.  After running, this
	interface must make sure that any previous page table
	modifications for the address space 'vma->vm_mm' in the range
	'start' to 'end-1' will be visible to the cpu.  That is, after
	running, there will be no entries in the TLB for 'mm' for
	virtual addresses in the range 'start' to 'end-1'.

	The "vma" is the backing store being used for the region.
	Primarily, this is used for munmap() type operations.

	The interface is provided in hopes that the port can find
	a suitably efficient method for removing multiple page
	sized translations from the TLB, instead of having the kernel
	call flush_tlb_page (see below) for each entry which may be
	modified.
	(这里我们从 TLB 中刷新一段特定范围的(用户)虚拟地址转换。运行之后，此接口必须确保地址空间 'vma->vm_mm' 在 'start' 到 'end-1' 范围内此前的任何页表修改对 CPU 可见。也就是说，运行之后，TLB 中将不再有 'mm' 在 'start' 到 'end-1' 范围内虚拟地址的条目。

	"vma" 是该区域正在使用的后备存储。此接口主要用于 munmap() 一类的操作。

	提供此接口是希望移植代码能找到一种足够高效的方法，从 TLB 中一次移除多个页大小的转换，而不必让内核对每个可能被修改的条目都调用一次 flush_tlb_page(见下文)。)

4) ``void flush_tlb_page(struct vm_area_struct *vma, unsigned long addr)``

	This time we need to remove the PAGE_SIZE sized translation
	from the TLB.  The 'vma' is the backing structure used by
	Linux to keep track of mmap'd regions for a process, the
	address space is available via vma->vm_mm.  Also, one may
	test (vma->vm_flags & VM_EXEC) to see if this region is
	executable (and thus could be in the 'instruction TLB' in
	split-tlb type setups).

	After running, this interface must make sure that any previous
	page table modification for address space 'vma->vm_mm' for
	user virtual address 'addr' will be visible to the cpu.  That
	is, after running, there will be no entries in the TLB for
	'vma->vm_mm' for virtual address 'addr'.

	This is used primarily during fault processing.
	(这次我们需要从 TLB 中移除一个 PAGE_SIZE 大小的转换。'vma' 是 Linux 用来跟踪进程 mmap 区域的后备结构，地址空间可经由 vma->vm_mm 获得。此外，可以测试 (vma->vm_flags & VM_EXEC) 来判断该区域是否可执行(在 split-tlb 类型的设计中，它可能位于"指令 TLB"中)。

	运行之后，此接口必须确保地址空间 'vma->vm_mm' 中用户虚拟地址 'addr' 此前的任何页表修改对 CPU 可见。也就是说，运行之后，TLB 中将不再有 'vma->vm_mm' 在虚拟地址 'addr' 处的条目。

	这主要用于缺页异常处理。)

5) ``void update_mmu_cache_range(struct vm_fault *vmf,
   struct vm_area_struct *vma, unsigned long address, pte_t *ptep,
   unsigned int nr)``

	At the end of every page fault, this routine is invoked to tell
	the architecture specific code that translations now exists
	in the software page tables for address space "vma->vm_mm"
	at virtual address "address" for "nr" consecutive pages.

	This routine is also invoked in various other places which pass
	a NULL "vmf".

	A port may use this information in any way it so chooses.
	For example, it could use this event to pre-load TLB
	translations for software managed TLB configurations.
	The sparc64 port currently does this.
	(在每次缺页异常结束时会调用此例程，告知体系结构相关代码：软件页表中现已存在地址空间 "vma->vm_mm" 在虚拟地址 "address" 处连续 "nr" 个页的转换。

	在各种其他地方也会调用此例程，并传入 NULL 的 "vmf"。

	移植代码可以用它自选的任何方式使用这一信息。例如，对于软件管理 TLB 的配置，可以利用这一事件预加载 TLB 转换。sparc64 移植目前就是这么做的。)

Next, we have the cache flushing interfaces.  In general, when Linux
is changing an existing virtual-->physical mapping to a new value,
the sequence will be in one of the following forms::

	1) flush_cache_mm(mm);
	   change_all_page_tables_of(mm);
	   flush_tlb_mm(mm);

	2) flush_cache_range(vma, start, end);
	   change_range_of_page_tables(mm, start, end);
	   flush_tlb_range(vma, start, end);

	3) flush_cache_page(vma, addr, pfn);
	   set_pte(pte_pointer, new_pte_val);
	   flush_tlb_page(vma, addr);

(接下来是缓存刷新接口。一般而言，当 Linux 把一个已有的"虚拟地址→物理地址"映射改为新值时，操作序列会呈现为以下几种形式之一：)

The cache level flush will always be first, because this allows
us to properly handle systems whose caches are strict and require
a virtual-->physical translation to exist for a virtual address
when that virtual address is flushed from the cache.  The HyperSparc
cpu is one such cpu with this attribute.
(缓存层的刷新总是排在最前面，因为这让我们能够正确处理那些"严格"缓存系统——这类系统要求在把某个虚拟地址从缓存中刷新掉时，该虚拟地址的"虚拟地址→物理地址"转换必须存在。HyperSparc CPU 就是具有这一属性的 CPU。)

The cache flushing routines below need only deal with cache flushing
to the extent that it is necessary for a particular cpu.  Mostly,
these routines must be implemented for cpus which have virtually
indexed caches which must be flushed when virtual-->physical
translations are changed or removed.  So, for example, the physically
indexed physically tagged caches of IA32 processors have no need to
implement these interfaces since the caches are fully synchronized
and have no dependency on translation information.
(下文的缓存刷新例程只需在特定 CPU 需要的范围内处理缓存刷新。大多数情况下，这些例程必须为拥有虚拟索引缓存的 CPU 实现——当"虚拟地址→物理地址"转换被修改或移除时，这类缓存必须被刷新。例如，IA32 处理器的物理索引物理标记 (PIPT) 缓存就无需实现这些接口，因为其缓存是完全同步的，不依赖转换信息。)

Here are the routines, one by one:

(下面逐一介绍这些例程：)

1) ``void flush_cache_mm(struct mm_struct *mm)``

	This interface flushes an entire user address space from
	the caches.  That is, after running, there will be no cache
	lines associated with 'mm'.

	This interface is used to handle whole address space
	page table operations such as what happens during exit and exec.
	(此接口把整个用户地址空间从缓存中刷新掉。也就是说，运行之后，缓存中不会再有与 'mm' 关联的缓存行。

	此接口用于处理整个地址空间层面的页表操作，例如 exit 与 exec 期间发生的操作。)

2) ``void flush_cache_dup_mm(struct mm_struct *mm)``

	This interface flushes an entire user address space from
	the caches.  That is, after running, there will be no cache
	lines associated with 'mm'.

	This interface is used to handle whole address space
	page table operations such as what happens during fork.

	This option is separate from flush_cache_mm to allow some
	optimizations for VIPT caches.
	(此接口把整个用户地址空间从缓存中刷新掉。也就是说，运行之后，缓存中不会再有与 'mm' 关联的缓存行。

	此接口用于处理整个地址空间层面的页表操作，例如 fork 期间发生的操作。

	把它与 flush_cache_mm 分开提供，是为了给 VIPT 缓存留出一些优化空间。)

3) ``void flush_cache_range(struct vm_area_struct *vma,
   unsigned long start, unsigned long end)``

	Here we are flushing a specific range of (user) virtual
	addresses from the cache.  After running, there will be no
	entries in the cache for 'vma->vm_mm' for virtual addresses in
	the range 'start' to 'end-1'.

	The "vma" is the backing store being used for the region.
	Primarily, this is used for munmap() type operations.

	The interface is provided in hopes that the port can find
	a suitably efficient method for removing multiple page
	sized regions from the cache, instead of having the kernel
	call flush_cache_page (see below) for each entry which may be
	modified.
	(这里我们从缓存中刷新一段特定范围的(用户)虚拟地址。运行之后，缓存中不会再有 'vma->vm_mm' 在 'start' 到 'end-1' 范围内虚拟地址的条目。

	"vma" 是该区域正在使用的后备存储。此接口主要用于 munmap() 一类的操作。

	提供此接口是希望移植代码能找到一种足够高效的方法，从缓存中一次移除多个页大小的区域，而不必让内核对每个可能被修改的条目都调用一次 flush_cache_page(见下文)。)

4) ``void flush_cache_page(struct vm_area_struct *vma, unsigned long addr, unsigned long pfn)``

	This time we need to remove a PAGE_SIZE sized range
	from the cache.  The 'vma' is the backing structure used by
	Linux to keep track of mmap'd regions for a process, the
	address space is available via vma->vm_mm.  Also, one may
	test (vma->vm_flags & VM_EXEC) to see if this region is
	executable (and thus could be in the 'instruction cache' in
	"Harvard" type cache layouts).

	The 'pfn' indicates the physical page frame (shift this value
	left by PAGE_SHIFT to get the physical address) that 'addr'
	translates to.  It is this mapping which should be removed from
	the cache.

	After running, there will be no entries in the cache for
	'vma->vm_mm' for virtual address 'addr' which translates
	to 'pfn'.

	This is used primarily during fault processing.
	(这次我们需要从缓存中移除一个 PAGE_SIZE 大小的范围。'vma' 是 Linux 用来跟踪进程 mmap 区域的后备结构，地址空间可经由 vma->vm_mm 获得。此外，可以测试 (vma->vm_flags & VM_EXEC) 来判断该区域是否可执行(在"哈佛" (Harvard) 类型的缓存布局中，它可能位于"指令缓存"中)。

	'pfn' 指示 'addr' 所转换到的物理页帧(把该值左移 PAGE_SHIFT 位即得物理地址)。应当从缓存中移除的正是这一映射。

	运行之后，缓存中不会再有 'vma->vm_mm' 在虚拟地址 'addr'(转换到 'pfn')处的条目。

	这主要用于缺页异常处理。)

5) ``void flush_cache_kmaps(void)``

	This routine need only be implemented if the platform utilizes
	highmem.  It will be called right before all of the kmaps
	are invalidated.

	After running, there will be no entries in the cache for
	the kernel virtual address range PKMAP_ADDR(0) to
	PKMAP_ADDR(LAST_PKMAP).

	This routing should be implemented in asm/highmem.h
	(只有平台使用 highmem 时才需要实现此例程。它会在所有 kmap 被失效之前被立即调用。

	运行之后，缓存中不会再有内核虚拟地址范围 PKMAP_ADDR(0) 到 PKMAP_ADDR(LAST_PKMAP) 的条目。

	此例程应在 asm/highmem.h 中实现。)

6) ``void flush_cache_vmap(unsigned long start, unsigned long end)``
   ``void flush_cache_vunmap(unsigned long start, unsigned long end)``

	Here in these two interfaces we are flushing a specific range
	of (kernel) virtual addresses from the cache.  After running,
	there will be no entries in the cache for the kernel address
	space for virtual addresses in the range 'start' to 'end-1'.

	The first of these two routines is invoked after vmap_range()
	has installed the page table entries.  The second is invoked
	before vunmap_range() deletes the page table entries.
	(在这两个接口中，我们从缓存中刷新一段特定范围的(内核)虚拟地址。运行之后，内核地址空间在 'start' 到 'end-1' 范围内虚拟地址的缓存条目将不复存在。

	这两个例程中的第一个在 vmap_range() 装好页表项之后被调用；第二个在 vunmap_range() 删除页表项之前被调用。)

There exists another whole class of cpu cache issues which currently
require a whole different set of interfaces to handle properly.
The biggest problem is that of virtual aliasing in the data cache
of a processor.
(还有另外一大类 CPU 缓存问题，目前需要一套完全不同的接口才能妥善处理。最大的问题是处理器的数据缓存中的虚拟别名 (virtual aliasing)。)

Is your port susceptible to virtual aliasing in its D-cache?
Well, if your D-cache is virtually indexed, is larger in size than
PAGE_SIZE, and does not prevent multiple cache lines for the same
physical address from existing at once, you have this problem.
(你的移植是否易受 D-cache 虚拟别名影响？如果你的 D-cache 是虚拟索引的、容量大于 PAGE_SIZE、且不阻止同一物理地址的多个缓存行同时存在，那你就有这个问题。)

If your D-cache has this problem, first define asm/shmparam.h SHMLBA
properly, it should essentially be the size of your virtually
addressed D-cache (or if the size is variable, the largest possible
size).  This setting will force the SYSv IPC layer to only allow user
processes to mmap shared memory at address which are a multiple of
this value.
(如果你的 D-cache 有这个问题，首先要正确定义 asm/shmparam.h 中的 SHMLBA，它基本上应当等于你虚拟寻址 D-cache 的大小(如果大小可变，则取可能的最大值)。这一设置会迫使 SYSv IPC 层只允许用户进程在以此值为倍数的地址上 mmap 共享内存。)

.. note::

  This does not fix shared mmaps, check out the sparc64 port for
  one way to solve this (in particular SPARC_FLAG_MMAPSHARED).
  (注意：这并不能解决共享 mmap 的问题；sparc64 移植给出了一种解决办法(尤其是 SPARC_FLAG_MMAPSHARED)，可供参考。)

Next, you have to solve the D-cache aliasing issue for all
other cases.  Please keep in mind that fact that, for a given page
mapped into some user address space, there is always at least one more
mapping, that of the kernel in its linear mapping starting at
PAGE_OFFSET.  So immediately, once the first user maps a given
physical page into its address space, by implication the D-cache
aliasing problem has the potential to exist since the kernel already
maps this page at its virtual address.
(接下来，你必须为所有其他情形解决 D-cache 别名问题。请记住：对于一个被映射进某个用户地址空间的给定页面，总是至少还存在另一个映射，即内核从 PAGE_OFFSET 开始的线性映射中的映射。所以，一旦第一个用户把某个物理页映射进自己的地址空间，就意味着 D-cache 别名问题随即有了存在的可能，因为内核早已在自己的虚拟地址上映射了这一页。)

  ``void copy_user_page(void *to, void *from, unsigned long addr, struct page *page)``
  ``void clear_user_page(void *to, unsigned long addr, struct page *page)``

	These two routines store data in user anonymous or COW
	pages.  It allows a port to efficiently avoid D-cache alias
	issues between userspace and the kernel.

	For example, a port may temporarily map 'from' and 'to' to
	kernel virtual addresses during the copy.  The virtual address
	for these two pages is chosen in such a way that the kernel
	load/store instructions happen to virtual addresses which are
	of the same "color" as the user mapping of the page.  Sparc64
	for example, uses this technique.

	The 'addr' parameter tells the virtual address where the
	user will ultimately have this page mapped, and the 'page'
	parameter gives a pointer to the struct page of the target.

	If D-cache aliasing is not an issue, these two routines may
	simply call memcpy/memset directly and do nothing more.
	(这两个例程向用户匿名页或 COW 页写入数据。它使移植代码能够高效地避免用户空间与内核之间的 D-cache 别名问题。

	例如，移植代码可以在复制期间把 'from' 与 'to' 临时映射到内核虚拟地址。这两个页面的虚拟地址要选得让内核的加载/存储指令恰好落在与该页用户映射相同"颜色" (color) 的虚拟地址上。例如 sparc64 就使用这一技术。

	'addr' 参数告知用户最终将把该页映射到的虚拟地址，'page' 参数给出目标 struct page 的指针。

	如果 D-cache 别名不构成问题，这两个例程可以直接调用 memcpy/memset，无需再做别的。)

  ``void flush_dcache_folio(struct folio *folio)``

        This routines must be called when:

	  a) the kernel did write to a page that is in the page cache page
	     and / or in high memory
	  b) the kernel is about to read from a page cache page and user space
	     shared/writable mappings of this page potentially exist.  Note
	     that {get,pin}_user_pages{_fast} already call flush_dcache_folio
	     on any page found in the user address space and thus driver
	     code rarely needs to take this into account.

	.. note::

	      This routine need only be called for page cache pages
	      which can potentially ever be mapped into the address
	      space of a user process.  So for example, VFS layer code
	      handling vfs symlinks in the page cache need not call
	      this interface at all.

	The phrase "kernel writes to a page cache page" means, specifically,
	that the kernel executes store instructions that dirty data in that
	page at the kernel virtual mapping of that page.  It is important to
	flush here to handle D-cache aliasing, to make sure these kernel stores
	are visible to user space mappings of that page.

	The corollary case is just as important, if there are users which have
	shared+writable mappings of this file, we must make sure that kernel
	reads of these pages will see the most recent stores done by the user.

	If D-cache aliasing is not an issue, this routine may simply be defined
	as a nop on that architecture.

        There is a bit set aside in folio->flags (PG_arch_1) as "architecture
	private".  The kernel guarantees that, for pagecache pages, it will
	clear this bit when such a page first enters the pagecache.

	This allows these interfaces to be implemented much more
	efficiently.  It allows one to "defer" (perhaps indefinitely) the
	actual flush if there are currently no user processes mapping this
	page.  See sparc64's flush_dcache_folio and update_mmu_cache_range
	implementations for an example of how to go about doing this.

	The idea is, first at flush_dcache_folio() time, if
	folio_flush_mapping() returns a mapping, and mapping_mapped() on that
	mapping returns %false, just mark the architecture private page
	flag bit.  Later, in update_mmu_cache_range(), a check is made
	of this flag bit, and if set the flush is done and the flag bit
	is cleared.

	.. important::

			It is often important, if you defer the flush,
			that the actual flush occurs on the same CPU
			as did the cpu stores into the page to make it
			dirty.  Again, see sparc64 for examples of how
			to deal with this.
	(必须在以下情况下调用此例程：

	  a) 内核写入了某个页缓存页和/或高端内存中的页；
	  b) 内核即将从某个页缓存页读取，且该页可能存在用户空间的共享/可写映射。注意，{get,pin}_user_pages{_fast} 已经会对在用户地址空间中找到的任何页调用 flush_dcache_folio，因此驱动代码很少需要考虑这一点。

	.. note::

	      (只需对可能被映射进用户进程地址空间的页缓存页调用此例程。例如，在页缓存中处理 vfs 符号链接的 VFS 层代码根本无需调用此接口。)

	"内核写入页缓存页"这句话的确切含义是：内核在该页的内核虚拟映射处执行存储指令，弄脏该页中的数据。这里必须进行刷新以处理 D-cache 别名，确保这些内核存储对该页的用户空间映射可见。

	推论情形同样重要：如果有用户对该文件持有共享+可写映射，我们必须确保内核对这些页的读取能看到用户最近完成的存储。

	如果 D-cache 别名不构成问题，该例程在该体系结构上可以简单地定义为一个 nop。

        folio->flags 中保留了一个"体系结构私有"位 (PG_arch_1)。内核保证，对页缓存页而言，当这样的页首次进入页缓存时会清除该位。

	这使得这些接口的实现可以高效得多。如果当前没有用户进程映射该页，就可以把实际刷新"推迟"(甚至无限期推迟)。具体做法可参见 sparc64 的 flush_dcache_folio 与 update_mmu_cache_range 实现。

	思路是：首先在 flush_dcache_folio() 时，如果 folio_flush_mapping() 返回一个映射、且对该映射调用 mapping_mapped() 返回 %false，就只设置那个体系结构私有页标志位。之后在 update_mmu_cache_range() 中检查该标志位，若已置位则执行刷新并清除该位。)

	.. important::

			(注意：如果推迟了刷新，实际刷新务必发生在与把该页弄脏的 CPU 存储相同的 CPU 上，这一点往往很重要。具体应对方法同样可参见 sparc64。)

  ``void copy_to_user_page(struct vm_area_struct *vma, struct page *page,
  unsigned long user_vaddr, void *dst, void *src, int len)``
  ``void copy_from_user_page(struct vm_area_struct *vma, struct page *page,
  unsigned long user_vaddr, void *dst, void *src, int len)``

	When the kernel needs to copy arbitrary data in and out
	of arbitrary user pages (f.e. for ptrace()) it will use
	these two routines.

	Any necessary cache flushing or other coherency operations
	that need to occur should happen here.  If the processor's
	instruction cache does not snoop cpu stores, it is very
	likely that you will need to flush the instruction cache
	for copy_to_user_page().
	(当内核需要在任意用户页中进出复制任意数据时(例如用于 ptrace())，会使用这两个例程。

	任何需要发生的缓存刷新或其他一致性操作都应在此进行。如果处理器的指令缓存不侦听 (snoop) CPU 存储，那么你很可能需要为 copy_to_user_page() 刷新指令缓存。)

  ``void flush_anon_page(struct vm_area_struct *vma, struct page *page,
  unsigned long vmaddr)``

  	When the kernel needs to access the contents of an anonymous
	page, it calls this function (currently only
	get_user_pages()).  Note: flush_dcache_folio() deliberately
	doesn't work for an anonymous page.  The default
	implementation is a nop (and should remain so for all coherent
	architectures).  For incoherent architectures, it should flush
	the cache of the page at vmaddr.
	(当内核需要访问匿名页的内容时会调用此函数(目前只有 get_user_pages())。注意：flush_dcache_folio() 刻意不适用于匿名页。默认实现是一个 nop(对所有一致性 (coherent) 体系结构也应保持如此)。对于非一致性体系结构，它应刷新该页在 vmaddr 处的缓存。)

  ``void flush_icache_range(unsigned long start, unsigned long end)``

  	When the kernel stores into addresses that it will execute
	out of (eg when loading modules), this function is called.

	If the icache does not snoop stores then this routine will need
	to flush it.
	(当内核向自己将要执行的地址写入时(例如加载模块时)，会调用此函数。

	如果指令缓存不侦听存储，则此例程需要刷新它。)

  ``void flush_icache_page(struct vm_area_struct *vma, struct page *page)``

	All the functionality of flush_icache_page can be implemented in
	flush_dcache_folio and update_mmu_cache_range. In the future, the hope
	is to remove this interface completely.
	(flush_icache_page 的全部功能都可以在 flush_dcache_folio 与 update_mmu_cache_range 中实现。未来希望能彻底移除这一接口。)

The final category of APIs is for I/O to deliberately aliased address
ranges inside the kernel.  Such aliases are set up by use of the
vmap/vmalloc API.  Since kernel I/O goes via physical pages, the I/O
subsystem assumes that the user mapping and kernel offset mapping are
the only aliases.  This isn't true for vmap aliases, so anything in
the kernel trying to do I/O to vmap areas must manually manage
coherency.  It must do this by flushing the vmap range before doing
I/O and invalidating it after the I/O returns.
(最后一类 API 用于对内核内部刻意建立的别名地址范围进行 I/O。这类别名通过 vmap/vmalloc API 建立。由于内核 I/O 经由物理页进行，I/O 子系统假定用户映射与内核偏移映射是仅有的别名。对 vmap 别名而言这并不成立，因此内核中任何试图对 vmap 区域做 I/O 的部分都必须手动管理一致性：在做 I/O 之前刷新 vmap 范围，并在 I/O 返回之后将其失效。)

  ``void flush_kernel_vmap_range(void *vaddr, int size)``

       flushes the kernel cache for a given virtual address range in
       the vmap area.  This is to make sure that any data the kernel
       modified in the vmap range is made visible to the physical
       page.  The design is to make this area safe to perform I/O on.
       Note that this API does *not* also flush the offset map alias
       of the area.
       (刷新 vmap 区域中给定虚拟地址范围的内核缓存。这是为了确保内核在该 vmap 范围内修改的任何数据都对物理页可见。其设计目的是让这一区域可以安全地执行 I/O。注意，此 API *不会* 同时刷新该区域的偏移映射别名。)

  ``void invalidate_kernel_vmap_range(void *vaddr, int size) invalidates``

       the cache for a given virtual address range in the vmap area
       which prevents the processor from making the cache stale by
       speculatively reading data while the I/O was occurring to the
       physical pages.  This is only necessary for data reads into the
       vmap area.
       (使 vmap 区域中给定虚拟地址范围的缓存失效，以防处理器在针对物理页进行 I/O 期间通过投机读取使缓存变得过期。只有向 vmap 区域读入数据时才需要这样做。)
