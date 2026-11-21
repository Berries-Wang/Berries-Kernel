.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

=============================
GPU SVM Section(GPU SVM 章节)
=============================

Agreed upon design principles(商定的设计原则)
=============================================

* migrate_to_ram path
  (migrate_to_ram 路径)

	* Rely only on core MM concepts (migration PTEs, page references, and
	  page locking).
	  (仅依赖核心 MM 概念（迁移 PTE、页引用和页锁定）。)
	* No driver specific locks other than locks for hardware interaction in
	  this path. These are not required and generally a bad idea to
	  invent driver defined locks to seal core MM races.
	  (在此路径中，除与硬件交互所需的锁之外，不使用任何驱动程序特定的锁。发明驱动程序自定义的锁来封闭核心 MM 竞态既无必要，通常也是坏主意。)
	* An example of a driver-specific lock causing issues occurred before
	  fixing do_swap_page to lock the faulting page. A driver-exclusive lock
	  in migrate_to_ram produced a stable livelock if enough threads read
	  the faulting page.
	  (在修复 do_swap_page 以锁定缺页页面之前，曾发生过驱动程序特定锁导致问题的例子。如果有足够多的线程读取缺页页面，migrate_to_ram 中的驱动程序独占锁会产生稳定的活锁。)
	* Partial migration is supported (i.e., a subset of pages attempting to
	  migrate can actually migrate, with only the faulting page guaranteed
	  to migrate).
	  (支持部分迁移（即，尝试迁移的页面子集中只有一部分实际迁移，仅保证缺页页面完成迁移）。)
	* Driver handles mixed migrations via retry loops rather than locking.
	  (驱动程序通过重试循环而不是加锁来处理混合迁移。)
* Eviction
  (逐出（Eviction）)

	* Eviction is defined as migrating data from the GPU back to the
	  CPU without a virtual address to free up GPU memory.
	  (逐出定义为在没有虚拟地址的情况下把数据从 GPU 迁回 CPU，以释放 GPU 内存。)
	* Only looking at physical memory data structures and locks as opposed to
	  looking at virtual memory data structures and locks.
	  (只关注物理内存的数据结构和锁，而不关注虚拟内存的数据结构和锁。)
	* No looking at mm/vma structs or relying on those being locked.
	  (不查看 mm/vma 结构，也不依赖它们被锁定。)
	* The rationale for the above two points is that CPU virtual addresses
	  can change at any moment, while the physical pages remain stable.
	  (上述两点的理由是：CPU 虚拟地址随时可能改变，而物理页保持稳定。)
	* GPU page table invalidation, which requires a GPU virtual address, is
	  handled via the notifier that has access to the GPU virtual address.
	  (需要 GPU 虚拟地址的 GPU 页表失效操作，由能够访问 GPU 虚拟地址的 notifier 处理。)
* GPU fault side
  (GPU 缺页一侧)

	* mmap_read only used around core MM functions which require this lock
	  and should strive to take mmap_read lock only in GPU SVM layer.
	  (mmap_read 只在需要此锁的核心 MM 函数周围使用，并应尽量只在 GPU SVM 层获取 mmap_read 锁。)
	* Big retry loop to handle all races with the mmu notifier under the gpu
	  pagetable locks/mmu notifier range lock/whatever we end up calling
          those.
	  (用一个大重试循环，在 GPU 页表锁/mmu notifier 范围锁/我们最终给它起的任何名字之下，处理与 mmu notifier 的所有竞态。)
	* Races (especially against concurrent eviction or migrate_to_ram)
	  should not be handled on the fault side by trying to hold locks;
	  rather, they should be handled using retry loops. One possible
	  exception is holding a BO's dma-resv lock during the initial migration
	  to VRAM, as this is a well-defined lock that can be taken underneath
	  the mmap_read lock.
	  (竞态（尤其是与并发逐出或 migrate_to_ram 的竞态）不应在缺页一侧通过尝试持有锁来处理，而应使用重试循环处理。一个可能的例外是在初始迁移到 VRAM 期间持有 BO 的 dma-resv 锁，因为这是一个定义良好、可以在 mmap_read 锁之下获取的锁。)
	* One possible issue with the above approach is if a driver has a strict
	  migration policy requiring GPU access to occur in GPU memory.
	  Concurrent CPU access could cause a livelock due to endless retries.
	  While no current user (Xe) of GPU SVM has such a policy, it is likely
	  to be added in the future. Ideally, this should be resolved on the
	  core-MM side rather than through a driver-side lock.
	  (上述方法的一个可能问题是：如果驱动程序有严格的迁移策略，要求 GPU 访问必须发生在 GPU 内存中，那么并发的 CPU 访问可能因无休止的重试而导致活锁。虽然 GPU SVM 目前没有这样的用户（Xe）具有这种策略，但它将来很可能会被加入。理想情况下，这应在核心 MM 一侧解决，而不是通过驱动程序侧的锁解决。)
* Physical memory to virtual backpointer
  (物理内存到虚拟地址的反向指针)

	* This does not work, as no pointers from physical memory to virtual
	  memory should exist. mremap() is an example of the core MM updating
	  the virtual address without notifying the driver of address
	  change rather the driver only receiving the invalidation notifier.
	  (这行不通，因为不应存在从物理内存到虚拟内存的指针。mremap() 就是核心 MM 在不通知驱动程序地址变化的情况下更新虚拟地址的一个例子；驱动程序只会收到失效 notifier。)
	* The physical memory backpointer (page->zone_device_data) should remain
	  stable from allocation to page free. Safely updating this against a
	  concurrent user would be very difficult unless the page is free.
	  (物理内存反向指针（page->zone_device_data）应从分配到页释放保持稳定。除非页已空闲，否则在存在并发使用者的情况下安全地更新它将非常困难。)
* GPU pagetable locking
  (GPU 页表锁定)

	* Notifier lock only protects range tree, pages valid state for a range
	  (rather than seqno due to wider notifiers), pagetable entries, and
	  mmu notifier seqno tracking, it is not a global lock to protect
          against races.
	  (notifier 锁只保护范围树、某个范围（而不是 seqno，因为 notifier 覆盖范围更宽）的页有效状态、页表项以及 mmu notifier seqno 跟踪，它不是用来防范竞态的全局锁。)
	* All races handled with big retry as mentioned above.
	  (如上所述，所有竞态都用大重试来处理。)

Overview of baseline design(基线设计概览)
=========================================

.. kernel-doc:: drivers/gpu/drm/drm_gpusvm.c
   :doc: Overview

.. kernel-doc:: drivers/gpu/drm/drm_gpusvm.c
   :doc: Locking

.. kernel-doc:: drivers/gpu/drm/drm_gpusvm.c
   :doc: Partial Unmapping of Ranges

.. kernel-doc:: drivers/gpu/drm/drm_gpusvm.c
   :doc: Examples

Overview of drm_pagemap design(drm_pagemap 设计概览)
====================================================

.. kernel-doc:: drivers/gpu/drm/drm_pagemap.c
   :doc: Overview

.. kernel-doc:: drivers/gpu/drm/drm_pagemap.c
   :doc: Migration

Possible future design features(未来可能的设计特性)
===================================================

* Concurrent GPU faults
  (并发 GPU 缺页)

	* CPU faults are concurrent so makes sense to have concurrent GPU
	  faults.
	  (CPU 缺页是并发的，因此让 GPU 缺页也是并发的才合理。)
	* Should be possible with fined grained locking in the driver GPU
	  fault handler.
	  (通过在驱动程序 GPU 缺页处理程序中使用细粒度锁定，应当是可行的。)
	* No expected GPU SVM changes required.
	  (预期不需要更改 GPU SVM。)
* Ranges with mixed system and device pages
  (混合系统页和设备页的范围)

	* Can be added if required to drm_gpusvm_get_pages fairly easily.
	  (如需要，可以相当容易地在 drm_gpusvm_get_pages 中添加。)
* Multi-GPU support
  (多 GPU 支持)

	* Work in progress and patches expected after initially landing on GPU
	  SVM.
	  (正在进行中，预计在 GPU SVM 初步落地后提交补丁。)
	* Ideally can be done with little to no changes to GPU SVM.
	  (理想情况下，可以在几乎不更改 GPU SVM 的情况下完成。)
* Drop ranges in favor of radix tree
  (放弃 ranges，改用基数树)

	* May be desirable for faster notifiers.
	  (为了更快的 notifier，这可能是可取的。)
* Compound device pages
  (复合设备页)

	* Nvidia, AMD, and Intel all have agreed expensive core MM functions in
	  migrate device layer are a performance bottleneck, having compound
	  device pages should help increase performance by reducing the number
	  of these expensive calls.
	  (Nvidia、AMD 和 Intel 都一致认为，迁移设备层中昂贵的核心 MM 函数是性能瓶颈；使用复合设备页应能通过减少这些昂贵调用的次数来帮助提高性能。)
* Higher order dma mapping for migration
  (用于迁移的高阶 DMA 映射)

	* 4k dma mapping adversely affects migration performance on Intel
	  hardware, higher order (2M) dma mapping should help here.
	  (4k DMA 映射对 Intel 硬件上的迁移性能有不利影响，更高阶（2M）的 DMA 映射应能有所帮助。)
* Build common userptr implementation on top of GPU SVM
  (在 GPU SVM 之上构建通用的 userptr 实现)
* Driver side madvise implementation and migration policies
  (驱动程序侧的 madvise 实现和迁移策略)
* Pull in pending dma-mapping API changes from Leon / Nvidia when these land
  (当 Leon / Nvidia 的 dma-mapping API 待定更改落地时将其拉入)
