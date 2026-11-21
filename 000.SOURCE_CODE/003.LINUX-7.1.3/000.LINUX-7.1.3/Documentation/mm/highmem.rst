==================================
High Memory Handling(高端内存处理)
==================================

By: Peter Zijlstra <a.p.zijlstra@chello.nl>
(作者: Peter Zijlstra <a.p.zijlstra@chello.nl>)

.. contents:: :local:

What Is High Memory?(什么是高端内存？)
======================================

High memory (highmem) is used when the size of physical memory approaches or
exceeds the maximum size of virtual memory.  At that point it becomes
impossible for the kernel to keep all of the available physical memory mapped
at all times.  This means the kernel needs to start using temporary mappings of
the pieces of physical memory that it wants to access.
(当物理内存的大小接近或超过虚拟内存的最大值时，就会用到高端内存(highmem)。在那种情况下，内核不可能始终把所有可用的物理内存都保持在映射状态。这意味着内核需要开始对它想访问的物理内存片段使用临时映射。)

The part of (physical) memory not covered by a permanent mapping is what we
refer to as 'highmem'.  There are various architecture dependent constraints on
where exactly that border lies.
(未被永久映射覆盖的那部分(物理)内存就是我们所说的 'highmem'。这条边界的确切位置受各种架构相关约束的限制。)

In the i386 arch, for example, we choose to map the kernel into every process's
VM space so that we don't have to pay the full TLB invalidation costs for
kernel entry/exit.  This means the available virtual memory space (4GiB on
i386) has to be divided between user and kernel space.
(例如在 i386 架构中，我们选择把内核映射进每个进程的 VM 地址空间，这样在进入/退出内核时就不必付出完整的 TLB 失效代价。这意味着可用的虚拟内存空间(i386 上为 4GiB)必须在用户空间与内核空间之间划分。)

The traditional split for architectures using this approach is 3:1, 3GiB for
userspace and the top 1GiB for kernel space::

		+--------+ 0xffffffff
		| Kernel |
		+--------+ 0xc0000000
		|        |
		| User   |
		|        |
		+--------+ 0x00000000

(对于使用这一方案的架构，传统划分是 3:1：3GiB 给用户空间，顶部 1GiB 给内核空间。如上图所示。)

This means that the kernel can at most map 1GiB of physical memory at any one
time, but because we need virtual address space for other things - including
temporary maps to access the rest of the physical memory - the actual direct
map will typically be less (usually around ~896MiB).
(这意味着内核在任一时刻最多只能映射 1GiB 物理内存，但由于我们需要把虚拟地址空间留作他用——包括访问其余物理内存的临时映射——实际的直接映射通常会小一些(一般约为 896MiB 左右)。)

Other architectures that have mm context tagged TLBs can have separate kernel
and user maps.  Some hardware (like some ARMs), however, have limited virtual
space when they use mm context tags.
(其他拥有 mm 上下文标记 TLB 的架构可以拥有相互独立的内核与用户映射。然而，一些硬件(例如某些 ARM)在使用 mm 上下文标记时虚拟空间有限。)


Temporary Virtual Mappings(临时虚拟映射)
========================================

The kernel contains several ways of creating temporary mappings. The following
list shows them in order of preference of use.
(内核包含多种创建临时映射的方式。下面的列表按使用优先顺序列出它们。)

* kmap_local_page(), kmap_local_folio() - These functions are used to create
  short term mappings. They can be invoked from any context (including
  interrupts) but the mappings can only be used in the context which acquired
  them. The only differences between them consist in the first taking a pointer
  to a struct page and the second taking a pointer to struct folio and the byte
  offset within the folio which identifies the page.
  (kmap_local_page() 与 kmap_local_folio() ——这些函数用于创建短期映射。它们可以在任何上下文(包括中断)中调用，但映射只能在获取它的那个上下文中使用。两者的唯一区别在于：前者接受指向 struct page 的指针，后者接受指向 struct folio 的指针以及 folio 内标识该页的字节偏移。)

  These functions should always be used, whereas kmap_atomic() and kmap() have
  been deprecated.
  (应当始终使用这些函数，而 kmap_atomic() 与 kmap() 已被弃用。)

  These mappings are thread-local and CPU-local, meaning that the mapping
  can only be accessed from within this thread and the thread is bound to the
  CPU while the mapping is active. Although preemption is never disabled by
  this function, the CPU can not be unplugged from the system via
  CPU-hotplug until the mapping is disposed.
  (这些映射是线程局部且 CPU 局部的，也就是说，映射只能在该线程内部访问，并且在映射存续期间该线程被绑定在该 CPU 上。虽然此函数从不禁用抢占，但在映射被释放之前，不能通过 CPU 热插拔把该 CPU 从系统中拔出。)

  It's valid to take pagefaults in a local kmap region, unless the context
  in which the local mapping is acquired does not allow it for other reasons.
  (在局部 kmap 区域内发生缺页是允许的，除非获取局部映射的上下文出于其他原因不允许。)

  As said, pagefaults and preemption are never disabled. There is no need to
  disable preemption because, when context switches to a different task, the
  maps of the outgoing task are saved and those of the incoming one are
  restored.
  (如前所述，缺页与抢占都不会被禁用。之所以无需禁用抢占，是因为当上下文切换到另一个任务时，退出任务的映射会被保存，而进入任务的映射会被恢复。)

  kmap_local_page(), as well as kmap_local_folio() always returns valid virtual
  kernel addresses and it is assumed that kunmap_local() will never fail.
  (kmap_local_page() 与 kmap_local_folio() 总是返回有效的内核虚拟地址，并且假定 kunmap_local() 永远不会失败。)

  On CONFIG_HIGHMEM=n kernels and for low memory pages they return the
  virtual address of the direct mapping. Only real highmem pages are
  temporarily mapped. Therefore, users may call a plain page_address()
  for pages which are known to not come from ZONE_HIGHMEM. However, it is
  always safe to use kmap_local_{page,folio}() / kunmap_local().
  (在 CONFIG_HIGHMEM=n 的内核上，对于低端内存页，它们返回直接映射的虚拟地址。只有真正的高端内存页才会被临时映射。因此，对于已知不来自 ZONE_HIGHMEM 的页，用户可以直接调用普通的 page_address()。不过，使用 kmap_local_{page,folio}() / kunmap_local() 总是安全的。)

  While they are significantly faster than kmap(), for the highmem case they
  come with restrictions about the pointers validity. Contrary to kmap()
  mappings, the local mappings are only valid in the context of the caller
  and cannot be handed to other contexts. This implies that users must
  be absolutely sure to keep the use of the return address local to the
  thread which mapped it.
  (虽然它们比 kmap() 快得多，但对于高端内存的情形，它们对指针的有效性有限制。与 kmap() 映射不同，局部映射只在调用者的上下文中有效，不能交给其他上下文。这意味着用户必须绝对确保把返回地址的使用局限在建立映射的那个线程内。)

  Most code can be designed to use thread local mappings. User should
  therefore try to design their code to avoid the use of kmap() by mapping
  pages in the same thread the address will be used and prefer
  kmap_local_page() or kmap_local_folio().
  (大多数代码都可以设计成使用线程局部映射。因此用户应当尽量设计代码以避免使用 kmap()：在将要使用该地址的同一个线程中映射页，并优先选用 kmap_local_page() 或 kmap_local_folio()。)

  Nesting kmap_local_page() and kmap_atomic() mappings is allowed to a certain
  extent (up to KMAP_TYPE_NR) but their invocations have to be strictly ordered
  because the map implementation is stack based. See kmap_local_page() kdocs
  (included in the "Functions" section) for details on how to manage nested
  mappings.
  (在一定程度上允许嵌套 kmap_local_page() 与 kmap_atomic() 映射(上限为 KMAP_TYPE_NR)，但由于映射实现是基于栈的，它们的调用必须严格按顺序进行。关于如何管理嵌套映射的细节，参见 kmap_local_page() 的内核文档(包含在"函数"一节中)。)

* kmap_atomic(). This function has been deprecated; use kmap_local_page().
  (kmap_atomic()。此函数已被弃用；请使用 kmap_local_page()。)

  NOTE: Conversions to kmap_local_page() must take care to follow the mapping
  restrictions imposed on kmap_local_page(). Furthermore, the code between
  calls to kmap_atomic() and kunmap_atomic() may implicitly depend on the side
  effects of atomic mappings, i.e. disabling page faults or preemption, or both.
  In that case, explicit calls to pagefault_disable() or preempt_disable() or
  both must be made in conjunction with the use of kmap_local_page().
  (注意：转换为 kmap_local_page() 时必须注意遵循 kmap_local_page() 施加的映射限制。此外，kmap_atomic() 与 kunmap_atomic() 调用之间的代码可能隐式依赖原子映射的副作用，即禁用缺页或抢占，或两者都禁用。在这种情况下，必须在使用 kmap_local_page() 的同时显式调用 pagefault_disable() 或 preempt_disable()，或两者都调用。)

  [Legacy documentation]
  ([遗留文档])

  This permits a very short duration mapping of a single page.  Since the
  mapping is restricted to the CPU that issued it, it performs well, but
  the issuing task is therefore required to stay on that CPU until it has
  finished, lest some other task displace its mappings.
  (这允许对单个页进行持续时间非常短的映射。由于映射被限制在发起它的那个 CPU 上，它的性能很好，但因此要求发起任务在完成之前留在该 CPU 上，以免其他任务挤掉它的映射。)

  kmap_atomic() may also be used by interrupt contexts, since it does not
  sleep and the callers too may not sleep until after kunmap_atomic() is
  called.
  (中断上下文也可以使用 kmap_atomic()，因为它不会休眠，而且调用者在 kunmap_atomic() 被调用之前也不得休眠。)

  Each call of kmap_atomic() in the kernel creates a non-preemptible section
  and disable pagefaults. This could be a source of unwanted latency. Therefore
  users should prefer kmap_local_page() instead of kmap_atomic().
  (内核中每次调用 kmap_atomic() 都会创建一个不可抢占的区段并禁用缺页。这可能是造成非预期延迟的一个来源。因此，用户应优先使用 kmap_local_page() 而不是 kmap_atomic()。)

  It is assumed that k[un]map_atomic() won't fail.
  (假定 k[un]map_atomic() 不会失败。)

* kmap(). This function has been deprecated; use kmap_local_page().
  (kmap()。此函数已被弃用；请使用 kmap_local_page()。)

  NOTE: Conversions to kmap_local_page() must take care to follow the mapping
  restrictions imposed on kmap_local_page(). In particular, it is necessary to
  make sure that the kernel virtual memory pointer is only valid in the thread
  that obtained it.
  (注意：转换为 kmap_local_page() 时必须注意遵循 kmap_local_page() 施加的映射限制。特别要确保内核虚拟内存指针只在获取它的那个线程中有效。)

  [Legacy documentation]
  ([遗留文档])

  This should be used to make short duration mapping of a single page with no
  restrictions on preemption or migration. It comes with an overhead as mapping
  space is restricted and protected by a global lock for synchronization. When
  mapping is no longer needed, the address that the page was mapped to must be
  released with kunmap().
  (它应用于对单个页进行持续时间较短的映射，且对抢占或迁移没有限制。由于映射空间受限并通过一把全局锁进行同步保护，它带有额外开销。当不再需要映射时，必须用 kunmap() 释放该页被映射到的地址。)

  Mapping changes must be propagated across all the CPUs. kmap() also
  requires global TLB invalidation when the kmap's pool wraps and it might
  block when the mapping space is fully utilized until a slot becomes
  available. Therefore, kmap() is only callable from preemptible context.
  (映射变更必须传播到所有 CPU。当 kmap 的池子回绕时，kmap() 还需要全局 TLB 失效；当映射空间被完全占用时，它可能阻塞，直到有槽位可用。因此，kmap() 只能在可抢占上下文中调用。)

  All the above work is necessary if a mapping must last for a relatively
  long time but the bulk of high-memory mappings in the kernel are
  short-lived and only used in one place. This means that the cost of
  kmap() is mostly wasted in such cases. kmap() was not intended for long
  term mappings but it has morphed in that direction and its use is
  strongly discouraged in newer code and the set of the preceding functions
  should be preferred.
  (如果映射必须持续相对较长的时间，上述所有工作都是必要的；但内核中的大部分高端内存映射都是短期的，且只在一个地方使用。这意味着在这些情况下 kmap() 的开销大多被浪费了。kmap() 本不是为长期映射设计的，但它已经演变成那个方向，在新代码中强烈不建议使用它，而应优先选用前面列出的那组函数。)

  On 64-bit systems, calls to kmap_local_page(), kmap_atomic() and kmap() have
  no real work to do because a 64-bit address space is more than sufficient to
  address all the physical memory whose pages are permanently mapped.
  (在 64 位系统上，对 kmap_local_page()、kmap_atomic() 和 kmap() 的调用没有实际工作要做，因为 64 位地址空间足以寻址所有其页被永久映射的物理内存。)

* vmap().  This can be used to make a long duration mapping of multiple
  physical pages into a contiguous virtual space.  It needs global
  synchronization to unmap.
  (vmap()。它可用于把多个物理页长期映射到一个连续的虚拟空间中。解除映射需要全局同步。)


Cost of Temporary Mappings(临时映射的代价)
==========================================

The cost of creating temporary mappings can be quite high.  The arch has to
manipulate the kernel's page tables, the data TLB and/or the MMU's registers.
(创建临时映射的代价可能相当高。架构必须操作内核的页表、数据 TLB 和/或 MMU 的寄存器。)

If CONFIG_HIGHMEM is not set, then the kernel will try and create a mapping
simply with a bit of arithmetic that will convert the page struct address into
a pointer to the page contents rather than juggling mappings about.  In such a
case, the unmap operation may be a null operation.
(如果未设置 CONFIG_HIGHMEM，内核将尝试仅用一点算术运算来创建映射——把页结构地址换算成指向页内容的指针——而不是来回折腾映射。在这种情况下，解除映射的操作可能是空操作。)

If CONFIG_MMU is not set, then there can be no temporary mappings and no
highmem.  In such a case, the arithmetic approach will also be used.
(如果未设置 CONFIG_MMU，则不可能有临时映射，也不存在高端内存。在这种情况下，同样会采用算术换算的方式。)


i386 PAE(原名)
==============

The i386 arch, under some circumstances, will permit you to stick up to 64GiB
of RAM into your 32-bit machine.  This has a number of consequences:
(i386 架构在某些情况下允许你把最多 64GiB 的内存塞进 32 位机器。这会带来若干后果：)

* Linux needs a page-frame structure for each page in the system and the
  pageframes need to live in the permanent mapping, which means:
  (Linux 需要为系统中的每个页准备一个页帧结构，而这些页帧必须位于永久映射中，这意味着：)

* you can have 896M/sizeof(struct page) page-frames at most; with struct
  page being 32-bytes that would end up being something in the order of 112G
  worth of pages; the kernel, however, needs to store more than just
  page-frames in that memory...
  (你最多只能拥有 896M/sizeof(struct page) 个页帧；如果 struct page 为 32 字节，那相当于大约 112G 规模的页；然而，内核需要在那块内存中保存的可不只是页帧……)

* PAE makes your page tables larger - which slows the system down as more
  data has to be accessed to traverse in TLB fills and the like.  One
  advantage is that PAE has more PTE bits and can provide advanced features
  like NX and PAT.
  (PAE 会使你的页表变大——由于 TLB 填充等操作需要访问更多数据，系统会变慢。一个优点是 PAE 拥有更多的 PTE 位，可以提供 NX 与 PAT 等高级特性。)

The general recommendation is that you don't use more than 8GiB on a 32-bit
machine - although more might work for you and your workload, you're pretty
much on your own - don't expect kernel developers to really care much if things
come apart.
(一般的建议是：在 32 位机器上不要使用超过 8GiB 内存——尽管更大的内存对你的工作负载也许可行，但你基本只能靠自己——如果出了问题，不要指望内核开发者会真的在意。)


Functions(函数)
===============

.. kernel-doc:: include/linux/highmem.h
.. kernel-doc:: mm/highmem.c
.. kernel-doc:: include/linux/highmem-internal.h
