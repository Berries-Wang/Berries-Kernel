========================================================
Heterogeneous Memory Management (HMM)(异构内存管理(HMM))
========================================================

Provide infrastructure and helpers to integrate non-conventional memory (device
memory like GPU on board memory) into regular kernel path, with the cornerstone
of this being specialized struct page for such memory (see sections 5 to 7 of
this document).
(提供基础设施与辅助函数，把非传统内存(设备内存，如 GPU 板载内存)集成到常规内核路径中；其基石是为这类内存专门提供的 struct page(见本文档第 5 至 7 节)。)

HMM also provides optional helpers for SVM (Share Virtual Memory), i.e.,
allowing a device to transparently access program addresses coherently with
the CPU meaning that any valid pointer on the CPU is also a valid pointer
for the device. This is becoming mandatory to simplify the use of advanced
heterogeneous computing where GPU, DSP, or FPGA are used to perform various
computations on behalf of a process.
(HMM 还为 SVM(共享虚拟内存，Share Virtual Memory)提供可选的辅助函数，即让设备能够与 CPU 一致地透明访问程序地址——这意味着 CPU 上任何有效的指针对设备同样有效。要简化高级异构计算的使用，这一点正变得不可或缺；在这些场景中，GPU、DSP 或 FPGA 被用来代表进程执行各种计算。)

This document is divided as follows: in the first section I expose the problems
related to using device specific memory allocators. In the second section, I
expose the hardware limitations that are inherent to many platforms. The third
section gives an overview of the HMM design. The fourth section explains how
CPU page-table mirroring works and the purpose of HMM in this context. The
fifth section deals with how device memory is represented inside the kernel.
Finally, the last section presents a new migration helper that allows
leveraging the device DMA engine.
(本文档的组织如下：第一节讨论使用设备专属内存分配器相关的问题；第二节讨论许多平台固有的硬件限制；第三节概述 HMM 的设计；第四节解释 CPU 页表的镜像工作方式以及 HMM 在此背景下的作用；第五节讨论设备内存如何在内核中表示；最后一节介绍一个新的迁移辅助函数，它允许利用设备的 DMA 引擎。)

.. contents:: :local:

Problems of using a device specific memory allocator(使用设备专属内存分配器的问题)
==================================================================================

Devices with a large amount of on board memory (several gigabytes) like GPUs
have historically managed their memory through dedicated driver specific APIs.
This creates a disconnect between memory allocated and managed by a device
driver and regular application memory (private anonymous, shared memory, or
regular file backed memory). From here on I will refer to this aspect as split
address space. I use shared address space to refer to the opposite situation:
i.e., one in which any application memory region can be used by a device
transparently.
(拥有大量板载内存(若干 GB)的设备(如 GPU)历来通过专属的驱动 API 管理其内存。这造成了由设备驱动分配与管理的内存和常规应用内存(私有匿名、共享内存或常规文件后备内存)之间的割裂。下文中我将把这一方面称为分割地址空间(split address space)；而共享地址空间(shared address space)则指相反的情形：即任何应用内存区域都可以被设备透明使用。)

Split address space happens because devices can only access memory allocated
through a device specific API. This implies that all memory objects in a program
are not equal from the device point of view which complicates large programs
that rely on a wide set of libraries.
(分割地址空间的成因是：设备只能访问通过设备专属 API 分配的内存。这意味着从设备的角度看，程序中的所有内存对象并非平等，这让依赖大量库的大型程序变得复杂。)

Concretely, this means that code that wants to leverage devices like GPUs needs
to copy objects between generically allocated memory (malloc, mmap private, mmap
share) and memory allocated through the device driver API (this still ends up
with an mmap but of the device file).
(具体而言，这意味着想要利用 GPU 之类设备的代码需要在通用分配的内存(malloc、mmap 私有、mmap 共享)与通过设备驱动 API 分配的内存之间拷贝对象(后者最终也是一个 mmap，只不过是对设备文件的 mmap)。)

For flat data sets (array, grid, image, ...) this isn't too hard to achieve but
for complex data sets (list, tree, ...) it's hard to get right. Duplicating a
complex data set needs to re-map all the pointer relations between each of its
elements. This is error prone and programs get harder to debug because of the
duplicate data set and addresses.
(对于扁平数据集(数组、网格、图像等)，这并不太难实现；但对于复杂数据集(链表、树等)，则很难做对。复制复杂数据集需要重新映射其各元素之间的所有指针关系。这容易出错，而且由于数据集与地址的重复，程序也更难调试。)

Split address space also means that libraries cannot transparently use data
they are getting from the core program or another library and thus each library
might have to duplicate its input data set using the device specific memory
allocator. Large projects suffer from this and waste resources because of the
various memory copies.
(分割地址空间还意味着库无法透明地使用从核心程序或另一个库获得的数据，因此每个库可能都得使用设备专属内存分配器来复制其输入数据集。大型项目深受其害，并因各种内存拷贝而浪费资源。)

Duplicating each library API to accept as input or output memory allocated by
each device specific allocator is not a viable option. It would lead to a
combinatorial explosion in the library entry points.
(把每个库的 API 都复制一份以接受每个设备专属分配器所分配的内存作为输入或输出，并不可行——那会导致库入口点的组合爆炸。)

Finally, with the advance of high level language constructs (in C++ but in
other languages too) it is now possible for the compiler to leverage GPUs and
other devices without programmer knowledge. Some compiler identified patterns
are only doable with a shared address space. It is also more reasonable to use
a shared address space for all other patterns.
(最后，随着高级语言构造的进步(C++ 及其他语言也是如此)，编译器如今可以在程序员不知情的情况下利用 GPU 与其他设备。某些由编译器识别出的模式只有共享地址空间才能实现；对所有其他模式而言，使用共享地址空间也更为合理。)


I/O bus, device memory characteristics(I/O 总线与设备内存特性)
==============================================================

I/O buses cripple shared address spaces due to a few limitations. Most I/O
buses only allow basic memory access from device to main memory; even cache
coherency is often optional. Access to device memory from a CPU is even more
limited. More often than not, it is not cache coherent.
(I/O 总线由于若干限制而削弱了共享地址空间。大多数 I/O 总线只允许设备对主存进行基本的内存访问；连缓存一致性也往往是可选的。CPU 对设备内存的访问则更加受限；多数情况下不具备缓存一致性。)

If we only consider the PCIE bus, then a device can access main memory (often
through an IOMMU) and be cache coherent with the CPUs. However, it only allows
a limited set of atomic operations from the device on main memory. This is worse
in the other direction: the CPU can only access a limited range of the device
memory and cannot perform atomic operations on it. Thus device memory cannot
be considered the same as regular memory from the kernel point of view.
(如果只考虑 PCIE 总线，那么设备可以访问主存(通常经由 IOMMU)并与 CPU 保持缓存一致。但它只允许设备对主存执行有限的几种原子操作。反方向更糟：CPU 只能访问设备内存的有限范围，且不能对其执行原子操作。因此从内核的角度看，设备内存不能被视为与常规内存相同。)

Another crippling factor is the limited bandwidth (~32GBytes/s with PCIE 4.0
and 16 lanes). This is 33 times less than the fastest GPU memory (1 TBytes/s).
The final limitation is latency. Access to main memory from the device has an
order of magnitude higher latency than when the device accesses its own memory.
(另一个削弱因素是带宽有限(PCIE 4.0 与 16 通道约为 32GBytes/s)。这比最快的 GPU 内存(1 TBytes/s)少 33 倍。最后一个限制是延迟。设备访问主存的延迟比其访问自己内存的延迟高一个数量级。)

Some platforms are developing new I/O buses or additions/modifications to PCIE
to address some of these limitations (OpenCAPI, CCIX). They mainly allow
two-way cache coherency between CPU and device and allow all atomic operations the
architecture supports. Sadly, not all platforms are following this trend and
some major architectures are left without hardware solutions to these problems.
(一些平台正在开发新的 I/O 总线，或对 PCIE 进行增补/修改，以解决其中部分限制(OpenCAPI、CCIX)。它们主要允许 CPU 与设备之间的双向缓存一致性，并允许架构支持的所有原子操作。可惜，并非所有平台都跟进这一趋势，一些主要架构仍然没有解决这些问题的硬件方案。)

So for shared address space to make sense, not only must we allow devices to
access any memory but we must also permit any memory to be migrated to device
memory while the device is using it (blocking CPU access while it happens).
(因此，要让共享地址空间有意义，我们不仅必须允许设备访问任何内存，还必须允许任何内存可以在设备使用它期间迁移到设备内存(期间阻塞 CPU 访问)。)


Shared address space and migration(共享地址空间与迁移)
======================================================

HMM intends to provide two main features. The first one is to share the address
space by duplicating the CPU page table in the device page table so the same
address points to the same physical memory for any valid main memory address in
the process address space.
(HMM 打算提供两个主要特性。第一个是通过把 CPU 页表复制到设备页表中来共享地址空间，使得进程地址空间中任何有效的主存地址在两边都指向相同的物理内存。)

To achieve this, HMM offers a set of helpers to populate the device page table
while keeping track of CPU page table updates. Device page table updates are
not as easy as CPU page table updates. To update the device page table, you must
allocate a buffer (or use a pool of pre-allocated buffers) and write GPU
specific commands in it to perform the update (unmap, cache invalidations, and
flush, ...). This cannot be done through common code for all devices. Hence
why HMM provides helpers to factor out everything that can be while leaving the
hardware specific details to the device driver.
(为实现这一点，HMM 提供了一组辅助函数来填充设备页表，同时跟踪 CPU 页表的更新。设备页表的更新不像 CPU 页表更新那样容易。要更新设备页表，你必须分配一个缓冲区(或使用预分配缓冲池)，并在其中写入 GPU 专属的命令来执行更新(解除映射、缓存失效与刷新等)。这无法用对所有设备都相同的通用代码完成。因此 HMM 提供辅助函数，把能够抽出来的部分都抽取出来，而把硬件特定的细节留给设备驱动。)

The second mechanism HMM provides is a new kind of ZONE_DEVICE memory that
allows allocating a struct page for each page of device memory. Those pages
are special because the CPU cannot map them. However, they allow migrating
main memory to device memory using existing migration mechanisms and everything
looks like a page that is swapped out to disk from the CPU point of view. Using a
struct page gives the easiest and cleanest integration with existing mm
mechanisms. Here again, HMM only provides helpers, first to hotplug new ZONE_DEVICE
memory for the device memory and second to perform migration. Policy decisions
of what and when to migrate is left to the device driver.
(HMM 提供的第二个机制是一种新的 ZONE_DEVICE 内存，它允许为设备内存的每一页分配一个 struct page。这些页很特殊，因为 CPU 无法映射它们。不过，它们允许使用现有的迁移机制把主存迁移到设备内存，而且从 CPU 的角度看，一切都像是一个换出到磁盘的页。使用 struct page 可以与现有 mm 机制实现最简单、最干净的集成。这里 HMM 同样只提供辅助函数：第一，为设备内存热插拔新的 ZONE_DEVICE 内存；第二，执行迁移。至于迁移什么、何时迁移，这些策略决定留给设备驱动。)

Note that any CPU access to a device page triggers a page fault and a migration
back to main memory. For example, when a page backing a given CPU address A is
migrated from a main memory page to a device page, then any CPU access to
address A triggers a page fault and initiates a migration back to main memory.
(注意，CPU 对设备页的任何访问都会触发缺页以及迁回主存的迁移。例如，当支撑某个 CPU 地址 A 的页从主存页迁移为设备页后，CPU 对地址 A 的任何访问都会触发缺页，并启动迁回主存的迁移。)

With these two features, HMM not only allows a device to mirror process address
space and keeps both CPU and device page tables synchronized, but also
leverages device memory by migrating the part of the data set that is actively being
used by the device.
(借助这两个特性，HMM 不仅允许设备镜像进程地址空间并保持 CPU 与设备页表同步，还通过迁移数据集中正被设备积极使用的部分来利用设备内存。)


Address space mirroring implementation and API(地址空间镜像的实现与 API)
========================================================================

Address space mirroring's main objective is to allow duplication of a range of
CPU page table into a device page table; HMM helps keep both synchronized. A
device driver that wants to mirror a process address space must start with the
registration of a mmu_interval_notifier::

 int mmu_interval_notifier_insert(struct mmu_interval_notifier *interval_sub,
				  struct mm_struct *mm, unsigned long start,
				  unsigned long length,
				  const struct mmu_interval_notifier_ops *ops);

(地址空间镜像的主要目标是允许把一段 CPU 页表复制到设备页表中；HMM 帮助保持两者同步。想要镜像进程地址空间的设备驱动必须先注册一个 mmu_interval_notifier：)

During the ops->invalidate() callback the device driver must perform the
update action to the range (mark range read only, or fully unmap, etc.). The
device must complete the update before the driver callback returns.
(在 ops->invalidate() 回调中，设备驱动必须对该范围执行更新动作(把范围标记为只读，或完全解除映射等)。设备必须在驱动回调返回之前完成更新。)

When the device driver wants to populate a range of virtual addresses, it can
use::

  int hmm_range_fault(struct hmm_range *range);

(当设备驱动想要填充一段虚拟地址范围时，可以使用：)

It will trigger a page fault on missing or read-only entries if write access is
requested (see below). Page faults use the generic mm page fault code path just
like a CPU page fault. The usage pattern is::

 int driver_populate_range(...)
 {
      struct hmm_range range;
      ...

      range.notifier = &interval_sub;
      range.start = ...;
      range.end = ...;
      range.hmm_pfns = ...;

      if (!mmget_not_zero(interval_sub->notifier.mm))
          return -EFAULT;

 again:
      range.notifier_seq = mmu_interval_read_begin(&interval_sub);
      mmap_read_lock(mm);
      ret = hmm_range_fault(&range);
      if (ret) {
          mmap_read_unlock(mm);
          if (ret == -EBUSY)
                 goto again;
          return ret;
      }
      mmap_read_unlock(mm);

      take_lock(driver->update);
      if (mmu_interval_read_retry(&ni, range.notifier_seq) {
          release_lock(driver->update);
          goto again;
      }

      /* Use pfns array content to update device page table,
       * under the update lock */

      release_lock(driver->update);
      return 0;
 }

(若请求写访问，它会对缺失或只读的表项触发缺页(见下文)。缺页使用通用的 mm 缺页代码路径，与 CPU 缺页一样。使用模式如下：)

The driver->update lock is the same lock that the driver takes inside its
invalidate() callback. That lock must be held before calling
mmu_interval_read_retry() to avoid any race with a concurrent CPU page table
update.
(driver->update 锁与驱动在其 invalidate() 回调内部获取的是同一把锁。调用 mmu_interval_read_retry() 之前必须持有该锁，以避免与并发的 CPU 页表更新发生竞态。)

Leverage default_flags and pfn_flags_mask(利用 default_flags 与 pfn_flags_mask)
===============================================================================

The hmm_range struct has 2 fields, default_flags and pfn_flags_mask, that specify
fault or snapshot policy for the whole range instead of having to set them
for each entry in the pfns array.
(hmm_range 结构有 2 个字段——default_flags 与 pfn_flags_mask——用于为整个范围指定缺页或快照策略，而不必为 pfns 数组中的每个表项逐一设置。)

For instance if the device driver wants pages for a range with at least read
permission, it sets::

    range->default_flags = HMM_PFN_REQ_FAULT;
    range->pfn_flags_mask = 0;

(例如，如果设备驱动想要一个范围内至少具有读权限的页面，它设置：)

and calls hmm_range_fault() as described above. This will fill fault all pages
in the range with at least read permission.
(然后按上文所述调用 hmm_range_fault()。这会以缺页方式填充范围内所有至少具有读权限的页面。)

Now let's say the driver wants to do the same except for one page in the range for
which it wants to have write permission. Now driver set::

    range->default_flags = HMM_PFN_REQ_FAULT;
    range->pfn_flags_mask = HMM_PFN_REQ_WRITE;
    range->pfns[index_of_write] = HMM_PFN_REQ_WRITE;

(现在假设驱动想做同样的事，只是范围内有一个页面它希望具有写权限。此时驱动设置：)

With this, HMM will fault in all pages with at least read (i.e., valid) and for the
address == range->start + (index_of_write << PAGE_SHIFT) it will fault with
write permission i.e., if the CPU pte does not have write permission set then HMM
will call handle_mm_fault().
(这样，HMM 会以至少读(即有效)权限缺页填入所有页面，而对于地址 == range->start + (index_of_write << PAGE_SHIFT) 的那个页面，它会以写权限缺页填入——也就是说，如果 CPU pte 没有置写权限，HMM 会调用 handle_mm_fault()。)

After hmm_range_fault completes the flag bits are set to the current state of
the page tables, ie HMM_PFN_VALID | HMM_PFN_WRITE will be set if the page is
writable.
(hmm_range_fault 完成后，标志位会被设置为页表的当前状态——即如果页面可写，则会设置 HMM_PFN_VALID | HMM_PFN_WRITE。)


Represent and manage device memory from core kernel point of view(从核心内核的角度表示与管理设备内存)
=====================================================================================================

Several different designs were tried to support device memory. The first one
used a device specific data structure to keep information about migrated memory
and HMM hooked itself in various places of mm code to handle any access to
addresses that were backed by device memory. It turns out that this ended up
replicating most of the fields of struct page and also needed many kernel code
paths to be updated to understand this new kind of memory.
(为支持设备内存曾尝试过几种不同的设计。第一种使用设备专属的数据结构来保存已迁移内存的信息，并让 HMM 在 mm 代码的各个位置挂钩，以处理对由设备内存支撑的地址的任何访问。事实证明，这最终复制了 struct page 的大多数字段，而且还需要更新许多内核代码路径以理解这种新内存。)

Most kernel code paths never try to access the memory behind a page
but only care about struct page contents. Because of this, HMM switched to
directly using struct page for device memory which left most kernel code paths
unaware of the difference. We only need to make sure that no one ever tries to
map those pages from the CPU side.
(大多数内核代码路径从不试图访问页背后的内存，只关心 struct page 的内容。正因如此，HMM 改为直接用 struct page 表示设备内存，从而让大多数内核代码路径意识不到差别。我们只需确保没有人试图从 CPU 侧映射这些页面。)

Migration to and from device memory(设备内存的迁入与迁出)
=========================================================

Because the CPU cannot access device memory directly, the device driver must
use hardware DMA or device specific load/store instructions to migrate data.
The migrate_vma_setup(), migrate_vma_pages(), and migrate_vma_finalize()
functions are designed to make drivers easier to write and to centralize common
code across drivers.
(由于 CPU 无法直接访问设备内存，设备驱动必须使用硬件 DMA 或设备专属的加载/存储指令来迁移数据。migrate_vma_setup()、migrate_vma_pages() 与 migrate_vma_finalize() 函数旨在让驱动更容易编写，并把各驱动间的公共代码集中起来。)

Before migrating pages to device private memory, special device private
``struct page`` needs to be created. These will be used as special "swap"
page table entries so that a CPU process will fault if it tries to access
a page that has been migrated to device private memory.
(在把页面迁移到设备私有内存之前，需要创建特殊的设备私有 ``struct page`` 。它们将用作特殊的 "swap" 页表项，这样当 CPU 进程试图访问已迁移到设备私有内存的页面时会触发缺页。)

These can be allocated and freed with::

    struct resource *res;
    struct dev_pagemap pagemap;

    res = request_free_mem_region(&iomem_resource, /* number of bytes */,
                                  "name of driver resource");
    pagemap.type = MEMORY_DEVICE_PRIVATE;
    pagemap.range.start = res->start;
    pagemap.range.end = res->end;
    pagemap.nr_range = 1;
    pagemap.ops = &device_devmem_ops;
    memremap_pages(&pagemap, numa_node_id());

    memunmap_pages(&pagemap);
    release_mem_region(pagemap.range.start, range_len(&pagemap.range));

(这些内存可以用以下方式分配与释放：)

There are also devm_request_free_mem_region(), devm_memremap_pages(),
devm_memunmap_pages(), and devm_release_mem_region() when the resources can
be tied to a ``struct device``.
(当资源可以绑定到 ``struct device`` 时，还有 devm_request_free_mem_region()、devm_memremap_pages()、devm_memunmap_pages() 与 devm_release_mem_region() 可用。)

The overall migration steps are similar to migrating NUMA pages within system
memory (see Documentation/mm/page_migration.rst) but the steps are split
between device driver specific code and shared common code:
(总体迁移步骤类似于在系统内存内迁移 NUMA 页(见 Documentation/mm/page_migration.rst)，但这些步骤被拆分在设备驱动专属代码与共享的公共代码之间：)

1. ``mmap_read_lock()``

   The device driver has to pass a ``struct vm_area_struct`` to
   migrate_vma_setup() so the mmap_read_lock() or mmap_write_lock() needs to
   be held for the duration of the migration.
   (设备驱动必须向 migrate_vma_setup() 传入一个 ``struct vm_area_struct`` ，因此在迁移期间需要持有 mmap_read_lock() 或 mmap_write_lock()。)

2. ``migrate_vma_setup(struct migrate_vma *args)``

   The device driver initializes the ``struct migrate_vma`` fields and passes
   the pointer to migrate_vma_setup(). The ``args->flags`` field is used to
   filter which source pages should be migrated. For example, setting
   ``MIGRATE_VMA_SELECT_SYSTEM`` will only migrate system memory and
   ``MIGRATE_VMA_SELECT_DEVICE_PRIVATE`` will only migrate pages residing in
   device private memory. If the latter flag is set, the ``args->pgmap_owner``
   field is used to identify device private pages owned by the driver. This
   avoids trying to migrate device private pages residing in other devices.
   Currently only anonymous private VMA ranges can be migrated to or from
   system memory and device private memory.
   (设备驱动初始化 ``struct migrate_vma`` 的各字段，并把指针传给 migrate_vma_setup()。``args->flags`` 字段用于筛选哪些源页应当被迁移。例如，设置 ``MIGRATE_VMA_SELECT_SYSTEM`` 将只迁移系统内存，而 ``MIGRATE_VMA_SELECT_DEVICE_PRIVATE`` 将只迁移驻留在设备私有内存中的页面。若设置了后者，则用 ``args->pgmap_owner`` 字段来标识属于该驱动的设备私有页。这避免试图迁移驻留在其他设备中的设备私有页。目前只有匿名私有 VMA 范围可以在系统内存与设备私有内存之间迁移。)

   One of the first steps migrate_vma_setup() does is to invalidate other
   device's MMUs with the ``mmu_notifier_invalidate_range_start(()`` and
   ``mmu_notifier_invalidate_range_end()`` calls around the page table
   walks to fill in the ``args->src`` array with PFNs to be migrated.
   The ``invalidate_range_start()`` callback is passed a
   ``struct mmu_notifier_range`` with the ``event`` field set to
   ``MMU_NOTIFY_MIGRATE`` and the ``owner`` field set to
   the ``args->pgmap_owner`` field passed to migrate_vma_setup(). This
   allows the device driver to skip the invalidation callback and only
   invalidate device private MMU mappings that are actually migrating.
   This is explained more in the next section.
   (migrate_vma_setup() 最先做的几步之一，是使用 ``mmu_notifier_invalidate_range_start(()`` 与 ``mmu_notifier_invalidate_range_end()`` 调用(围绕页表遍历)使其他设备的 MMU 失效，以把待迁移的 PFN 填入 ``args->src`` 数组。``invalidate_range_start()`` 回调会收到一个 ``struct mmu_notifier_range`` ，其 ``event`` 字段被设为 ``MMU_NOTIFY_MIGRATE`` ， ``owner`` 字段被设为传给 migrate_vma_setup() 的 ``args->pgmap_owner`` 字段。这允许设备驱动跳过失效回调，只使真正在迁移的设备私有 MMU 映射失效。下一节会更详细地解释这一点。)

   While walking the page tables, a ``pte_none()`` or ``is_zero_pfn()``
   entry results in a valid "zero" PFN stored in the ``args->src`` array.
   This lets the driver allocate device private memory and clear it instead
   of copying a page of zeros. Valid PTE entries to system memory or
   device private struct pages will be locked with ``lock_page()``, isolated
   from the LRU (if system memory since device private pages are not on
   the LRU), unmapped from the process, and a special migration PTE is
   inserted in place of the original PTE.
   migrate_vma_setup() also clears the ``args->dst`` array.
   (在遍历页表时， ``pte_none()`` 或 ``is_zero_pfn()`` 表项会在 ``args->src`` 数组中产生一个有效的 "zero" PFN。这使驱动可以分配设备私有内存并将其清零，而不必拷贝一页零。指向系统内存或设备私有 struct page 的有效 PTE 表项会被 ``lock_page()`` 锁定、从 LRU 隔离(若是系统内存；设备私有页不在 LRU 上)、从进程解除映射，并在原 PTE 的位置插入一个特殊的迁移 PTE。migrate_vma_setup() 还会清空 ``args->dst`` 数组。)

3. The device driver allocates destination pages and copies source pages to
   destination pages.
   (设备驱动分配目标页，并把源页拷贝到目标页。)

   The driver checks each ``src`` entry to see if the ``MIGRATE_PFN_MIGRATE``
   bit is set and skips entries that are not migrating. The device driver
   can also choose to skip migrating a page by not filling in the ``dst``
   array for that page.
   (驱动检查每个 ``src`` 表项，看 ``MIGRATE_PFN_MIGRATE`` 位是否已设置，并跳过未在迁移的表项。设备驱动也可以选择不迁移某个页面，只需不为该页填写 ``dst`` 数组。)

   The driver then allocates either a device private struct page or a
   system memory page, locks the page with ``lock_page()``, and fills in the
   ``dst`` array entry with::

     dst[i] = migrate_pfn(page_to_pfn(dpage));

   (随后，驱动分配一个设备私有 struct page 或系统内存页，用 ``lock_page()`` 锁定该页，并把 ``dst`` 数组表项填为：)

   Now that the driver knows that this page is being migrated, it can
   invalidate device private MMU mappings and copy device private memory
   to system memory or another device private page. The core Linux kernel
   handles CPU page table invalidations so the device driver only has to
   invalidate its own MMU mappings.
   (既然驱动已知道该页正在迁移，它就可以使用设备私有 MMU 映射失效，并把设备私有内存拷贝到系统内存或另一个设备私有页。CPU 页表失效由核心 Linux 内核处理，因此设备驱动只需使其自身的 MMU 映射失效。)

   The driver can use ``migrate_pfn_to_page(src[i])`` to get the
   ``struct page`` of the source and either copy the source page to the
   destination or clear the destination device private memory if the pointer
   is ``NULL`` meaning the source page was not populated in system memory.
   (驱动可以用 ``migrate_pfn_to_page(src[i])`` 获取源页的 ``struct page`` ，然后把源页拷贝到目标页；如果指针为 ``NULL`` (表示源页未在系统内存中填充)，则把目标设备私有内存清零。)

4. ``migrate_vma_pages()``

   This step is where the migration is actually "committed".
   (这一步是迁移真正 "提交" 的地方。)

   If the source page was a ``pte_none()`` or ``is_zero_pfn()`` page, this
   is where the newly allocated page is inserted into the CPU's page table.
   This can fail if a CPU thread faults on the same page. However, the page
   table is locked and only one of the new pages will be inserted.
   The device driver will see that the ``MIGRATE_PFN_MIGRATE`` bit is cleared
   if it loses the race.
   (如果源页是 ``pte_none()`` 或 ``is_zero_pfn()`` 页，那么这一步会把新分配的页插入 CPU 页表。如果有 CPU 线程在同一个页上缺页，这一步可能失败。不过页表是加锁的，两个新页中只会有一个被插入。如果设备驱动在竞争中落败，它会看到 ``MIGRATE_PFN_MIGRATE`` 位被清除。)

   If the source page was locked, isolated, etc. the source ``struct page``
   information is now copied to destination ``struct page`` finalizing the
   migration on the CPU side.
   (如果源页已被锁定、隔离等，此时源 ``struct page`` 的信息会被拷贝到目标 ``struct page`` ，从而在 CPU 侧完成迁移。)

5. Device driver updates device MMU page tables for pages still migrating,
   rolling back pages not migrating.
   (设备驱动为仍在迁移的页面更新设备 MMU 页表，并为不迁移的页面回滚。)

   If the ``src`` entry still has ``MIGRATE_PFN_MIGRATE`` bit set, the device
   driver can update the device MMU and set the write enable bit if the
   ``MIGRATE_PFN_WRITE`` bit is set.
   (如果 ``src`` 表项仍设置着 ``MIGRATE_PFN_MIGRATE`` 位，设备驱动就可以更新设备 MMU，并且若 ``MIGRATE_PFN_WRITE`` 位已设置，则置写使能位。)

6. ``migrate_vma_finalize()``

   This step replaces the special migration page table entry with the new
   page's page table entry and releases the reference to the source and
   destination ``struct page``.
   (这一步用新页的页表项替换特殊的迁移页表项，并释放对源与目标 ``struct page`` 的引用。)

7. ``mmap_read_unlock()``

   The lock can now be released.
   (现在可以释放锁了。)

Exclusive access memory(独占访问内存)
=====================================

Some devices have features such as atomic PTE bits that can be used to implement
atomic access to system memory. To support atomic operations to a shared virtual
memory page such a device needs access to that page which is exclusive of any
userspace access from the CPU. The ``make_device_exclusive()`` function
can be used to make a memory range inaccessible from userspace.
(某些设备具有诸如原子 PTE 位之类的特性，可用于实现对系统内存的原子访问。为支持对共享虚拟内存页的原子操作，这类设备需要对该页的访问是独占的，排斥来自 CPU 的任何用户空间访问。 ``make_device_exclusive()`` 函数可用于使一段内存范围无法从用户空间访问。)

This replaces all mappings for pages in the given range with special swap
entries. Any attempt to access the swap entry results in a fault which is
resolved by replacing the entry with the original mapping. A driver gets
notified that the mapping has been changed by MMU notifiers, after which point
it will no longer have exclusive access to the page. Exclusive access is
guaranteed to last until the driver drops the page lock and page reference, at
which point any CPU faults on the page may proceed as described.
(这会把给定范围内各页的所有映射替换为特殊的 swap 表项。任何对 swap 表项的访问尝试都会导致缺页，该缺页通过把表项替换回原映射来解决。驱动会通过 MMU 通知器收到映射已更改的通知，此后它将不再对该页拥有独占访问。独占访问保证持续到驱动释放页锁与页引用为止——此后该页上的任何 CPU 缺页都可按所述方式进行。)

Memory cgroup (memcg) and rss accounting(内存 cgroup(memcg)与 rss 统计)
=======================================================================

For now, device memory is accounted as any regular page in rss counters (either
anonymous if device page is used for anonymous, file if device page is used for
file backed page, or shmem if device page is used for shared memory). This is a
deliberate choice to keep existing applications, that might start using device
memory without knowing about it, running unimpacted.
(目前，设备内存按普通页的方式计入 rss 计数器(若设备页用于匿名页则计入匿名，用于文件后备页则计入 file，用于共享内存则计入 shmem)。这是一个刻意的选择：让可能在不知情的情况下开始使用设备内存的现有应用不受影响地继续运行。)

A drawback is that the OOM killer might kill an application using a lot of
device memory and not a lot of regular system memory and thus not freeing much
system memory. We want to gather more real world experience on how applications
and system react under memory pressure in the presence of device memory before
deciding to account device memory differently.
(一个缺点是：OOM killer 可能杀掉一个使用大量设备内存、却没使用多少常规系统内存的应用，因而释放不了多少系统内存。在决定以不同方式统计设备内存之前，我们希望收集更多真实世界的经验，了解在设备内存存在的条件下应用与系统在内存压力下的反应。)


Same decision was made for memory cgroup. Device memory pages are accounted
against same memory cgroup a regular page would be accounted to. This does
simplify migration to and from device memory. This also means that migration
back from device memory to regular memory cannot fail because it would
go above memory cgroup limit. We might revisit this choice later on once we
get more experience in how device memory is used and its impact on memory
resource control.
(内存 cgroup 也做了同样的决定。设备内存页计入与普通页相同的内存 cgroup。这确实简化了设备内存的迁入与迁出。这也意味着从设备内存迁回常规内存不会因为超出内存 cgroup 限制而失败。等我们对设备内存的使用方式及其对内存资源控制的影响有了更多经验后，可能会重新审视这一选择。)


Note that device memory can never be pinned by a device driver nor through GUP
and thus such memory is always free upon process exit. Or when last reference
is dropped in case of shared memory or file backed memory.
(注意，设备内存永远不能被设备驱动或 GUP 固定(pinned)，因此这类内存总是在进程退出时释放；对于共享内存或文件后备内存，则在最后一个引用被丢弃时释放。)
