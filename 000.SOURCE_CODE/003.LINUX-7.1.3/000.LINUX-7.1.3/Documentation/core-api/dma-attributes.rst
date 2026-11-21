========================
DMA attributes(DMA 属性)
========================

This document describes the semantics of the DMA attributes that are
defined in linux/dma-mapping.h.
(本文描述 linux/dma-mapping.h 中定义的各 DMA 属性的语义。)

DMA_ATTR_WEAK_ORDERING(弱排序)
------------------------------

DMA_ATTR_WEAK_ORDERING specifies that reads and writes to the mapping
may be weakly ordered, that is that reads and writes may pass each other.
(DMA_ATTR_WEAK_ORDERING 表示对映射的读与写可以是弱序的，即读与写可以互相超越。)

Since it is optional for platforms to implement DMA_ATTR_WEAK_ORDERING,
those that do not will simply ignore the attribute and exhibit default
behavior.
(由于平台实现 DMA_ATTR_WEAK_ORDERING 是可选的，未实现的平台会直接忽略该属性并表现为默认行为。)

DMA_ATTR_WRITE_COMBINE(写合并)
------------------------------

DMA_ATTR_WRITE_COMBINE specifies that writes to the mapping may be
buffered to improve performance.
(DMA_ATTR_WRITE_COMBINE 表示对映射的写入可以被缓冲，以提升性能。)

Since it is optional for platforms to implement DMA_ATTR_WRITE_COMBINE,
those that do not will simply ignore the attribute and exhibit default
behavior.
(由于平台实现 DMA_ATTR_WRITE_COMBINE 是可选的，未实现的平台会直接忽略该属性并表现为默认行为。)

DMA_ATTR_NO_KERNEL_MAPPING(无内核映射)
--------------------------------------

DMA_ATTR_NO_KERNEL_MAPPING lets the platform to avoid creating a kernel
virtual mapping for the allocated buffer. On some architectures creating
such mapping is non-trivial task and consumes very limited resources
(like kernel virtual address space or dma consistent address space).
Buffers allocated with this attribute can be only passed to user space
by calling dma_mmap_attrs(). By using this API, you are guaranteeing
that you won't dereference the pointer returned by dma_alloc_attr(). You
can treat it as a cookie that must be passed to dma_mmap_attrs() and
dma_free_attrs(). Make sure that both of these also get this attribute
set on each call.
(DMA_ATTR_NO_KERNEL_MAPPING 让平台避免为分配的缓冲区创建内核虚拟映射。在某些架构上，创建这样的映射并非易事，而且会消耗非常有限的资源(如内核虚拟地址空间或 DMA 一致性地址空间)。用该属性分配的缓冲区只能通过调用 dma_mmap_attrs() 传给用户空间。使用此 API 即表示你保证不会解引用 dma_alloc_attr() 返回的指针。你可以把它当作一个必须传给 dma_mmap_attrs() 与 dma_free_attrs() 的凭据 (cookie)。注意确保每次调用时这两个 API 也都设置了该属性。)

Since it is optional for platforms to implement
DMA_ATTR_NO_KERNEL_MAPPING, those that do not will simply ignore the
attribute and exhibit default behavior.
(由于平台实现 DMA_ATTR_NO_KERNEL_MAPPING 是可选的，未实现的平台会直接忽略该属性并表现为默认行为。)

DMA_ATTR_SKIP_CPU_SYNC(跳过 CPU 同步)
-------------------------------------

By default dma_map_{single,page,sg} functions family transfer a given
buffer from CPU domain to device domain. Some advanced use cases might
require sharing a buffer between more than one device. This requires
having a mapping created separately for each device and is usually
performed by calling dma_map_{single,page,sg} function more than once
for the given buffer with device pointer to each device taking part in
the buffer sharing. The first call transfers a buffer from 'CPU' domain
to 'device' domain, what synchronizes CPU caches for the given region
(usually it means that the cache has been flushed or invalidated
depending on the dma direction). However, next calls to
dma_map_{single,page,sg}() for other devices will perform exactly the
same synchronization operation on the CPU cache. CPU cache synchronization
might be a time consuming operation, especially if the buffers are
large, so it is highly recommended to avoid it if possible.
DMA_ATTR_SKIP_CPU_SYNC allows platform code to skip synchronization of
the CPU cache for the given buffer assuming that it has been already
transferred to 'device' domain. This attribute can be also used for
dma_unmap_{single,page,sg} functions family to force buffer to stay in
device domain after releasing a mapping for it. Use this attribute with
care!
(默认情况下，dma_map_{single,page,sg} 函数族会把给定缓冲区从 CPU 域传输到设备域。一些高级用例可能需要在多个设备之间共享一个缓冲区。这要求为每个设备单独建立映射，通常做法是对给定缓冲区多次调用 dma_map_{single,page,sg}，每次传入参与共享的各设备的设备指针。第一次调用把缓冲区从 'CPU' 域传输到 'device' 域，这会同步该区域的 CPU 缓存(通常意味着根据 DMA 方向刷新或失效缓存)。然而，随后对其他设备调用 dma_map_{single,page,sg}() 时，会对 CPU 缓存执行完全相同的同步操作。CPU 缓存同步可能是耗时的操作，缓冲区越大越是如此，因此强烈建议在可能时避免它。DMA_ATTR_SKIP_CPU_SYNC 允许平台代码跳过对给定缓冲区的 CPU 缓存同步，前提是该缓冲区已传输到 'device' 域。该属性也可用于 dma_unmap_{single,page,sg} 函数族，以强制缓冲区在释放其映射之后仍停留在设备域。请谨慎使用该属性！)

DMA_ATTR_FORCE_CONTIGUOUS(强制连续)
-----------------------------------

By default DMA-mapping subsystem is allowed to assemble the buffer
allocated by dma_alloc_attrs() function from individual pages if it can
be mapped as contiguous chunk into device dma address space. By
specifying this attribute the allocated buffer is forced to be contiguous
also in physical memory.
(默认情况下，如果缓冲区能以连续块的形式映射进设备的 DMA 地址空间，DMA-mapping 子系统允许把 dma_alloc_attrs() 函数分配的缓冲区由多个独立页拼装而成。指定该属性后，将强制所分配的缓冲区在物理内存中也连续。)

DMA_ATTR_ALLOC_SINGLE_PAGES(仅分配单页)
---------------------------------------

This is a hint to the DMA-mapping subsystem that it's probably not worth
the time to try to allocate memory to in a way that gives better TLB
efficiency (AKA it's not worth trying to build the mapping out of larger
pages).  You might want to specify this if:
(这是给 DMA-mapping 子系统的一个提示：为获得更好的 TLB 效率而分配内存(即尝试用更大的页构建映射)可能不值得花这个工夫。以下情况你可能希望指定该属性：)

- You know that the accesses to this memory won't thrash the TLB.
  You might know that the accesses are likely to be sequential or
  that they aren't sequential but it's unlikely you'll ping-pong
  between many addresses that are likely to be in different physical
  pages.
  (你知道对这块内存的访问不会颠簸 (thrash) TLB。你可能知道这些访问很可能是顺序的；或者虽非顺序，但不太可能在多个可能位于不同物理页的地址之间来回切换。)
- You know that the penalty of TLB misses while accessing the
  memory will be small enough to be inconsequential.  If you are
  doing a heavy operation like decryption or decompression this
  might be the case.
  (你知道访问这块内存时 TLB 未命中的代价足够小、无关紧要。如果你在做解密或解压这类重负载操作，情况可能正是如此。)
- You know that the DMA mapping is fairly transitory.  If you expect
  the mapping to have a short lifetime then it may be worth it to
  optimize allocation (avoid coming up with large pages) instead of
  getting the slight performance win of larger pages.
  (你知道这个 DMA 映射相当短暂。如果你预计映射的生命周期很短，那么优化分配(避免凑出大页)可能比换取大页带来的微小性能收益更值得。)

Setting this hint doesn't guarantee that you won't get huge pages, but it
means that we won't try quite as hard to get them.
(设置该提示并不保证你不会得到大页，只是意味着我们不会那么卖力地去凑大页。)

.. note:: At the moment DMA_ATTR_ALLOC_SINGLE_PAGES is only implemented on ARM,
	  though ARM64 patches will likely be posted soon.
	  (注意：目前 DMA_ATTR_ALLOC_SINGLE_PAGES 只在 ARM 上实现，不过 ARM64 补丁可能很快就会发布。)

DMA_ATTR_NO_WARN(不告警)
------------------------

This tells the DMA-mapping subsystem to suppress allocation failure reports
(similarly to __GFP_NOWARN).
(这告诉 DMA-mapping 子系统抑制分配失败报告(类似 __GFP_NOWARN)。)

On some architectures allocation failures are reported with error messages
to the system logs.  Although this can help to identify and debug problems,
drivers which handle failures (eg, retry later) have no problems with them,
and can actually flood the system logs with error messages that aren't any
problem at all, depending on the implementation of the retry mechanism.
(在某些架构上，分配失败会以错误消息的形式报告到系统日志。虽然这有助于发现与调试问题，但那些会自行处理失败(例如稍后重试)的驱动对此并无困扰；取决于重试机制的实现，它们反而可能用完全无关紧要的错误消息刷爆系统日志。)

So, this provides a way for drivers to avoid those error messages on calls
where allocation failures are not a problem, and shouldn't bother the logs.
(因此，该属性为驱动提供了一种途径：在分配失败无关紧要、不该打扰日志的调用上避免这些错误消息。)

.. note:: At the moment DMA_ATTR_NO_WARN is only implemented on PowerPC.
	  (注意：目前 DMA_ATTR_NO_WARN 只在 PowerPC 上实现。)

DMA_ATTR_PRIVILEGED(特权访问)
-----------------------------

Some advanced peripherals such as remote processors and GPUs perform
accesses to DMA buffers in both privileged "supervisor" and unprivileged
"user" modes.  This attribute is used to indicate to the DMA-mapping
subsystem that the buffer is fully accessible at the elevated privilege
level (and ideally inaccessible or at least read-only at the
lesser-privileged levels).
(一些高级外设(如远程处理器与 GPU)会以特权的 "supervisor" 与非特权的 "user" 两种模式访问 DMA 缓冲区。该属性用于向 DMA-mapping 子系统表明：该缓冲区在提升的特权级别下完全可访问(理想情况下，在较低特权级别下不可访问，或至少只读)。)

DMA_ATTR_MMIO(MMIO 地址)
------------------------

This attribute indicates the physical address is not normal system
memory. It may not be used with kmap*()/phys_to_virt()/phys_to_page()
functions, it may not be cacheable, and access using CPU load/store
instructions may not be allowed.
(该属性表明物理地址不是普通系统内存。它不可与 kmap*()/phys_to_virt()/phys_to_page() 函数一起使用，它可能不可缓存，并且可能不允许使用 CPU load/store 指令访问。)

Usually this will be used to describe MMIO addresses, or other non-cacheable
register addresses. When DMA mapping this sort of address we call
the operation Peer to Peer as a one device is DMA'ing to another device.
For PCI devices the p2pdma APIs must be used to determine if
DMA_ATTR_MMIO is appropriate.
(通常它用于描述 MMIO 地址或其他不可缓存的寄存器地址。对这类地址做 DMA 映射时，我们把该操作称为点对点 (Peer to Peer)，因为是一个设备向另一个设备发起 DMA。对 PCI 设备，必须使用 p2pdma API 来判断 DMA_ATTR_MMIO 是否适用。)

For architectures that require cache flushing for DMA coherence
DMA_ATTR_MMIO will not perform any cache flushing. The address
provided must never be mapped cacheable into the CPU.
(对需要缓存刷新才能保持 DMA 一致性的架构，DMA_ATTR_MMIO 不会执行任何缓存刷新。所提供的地址绝不能以可缓存方式映射进 CPU。)

DMA_ATTR_DEBUGGING_IGNORE_CACHELINES(忽略缓存行)
------------------------------------------------

This attribute indicates that CPU cache lines may overlap for buffers mapped
with DMA_FROM_DEVICE or DMA_BIDIRECTIONAL.
(该属性表明，以 DMA_FROM_DEVICE 或 DMA_BIDIRECTIONAL 映射的缓冲区，其 CPU 缓存行可能相互重叠。)

Such overlap may occur when callers map multiple small buffers that reside
within the same cache line. In this case, callers must guarantee that the CPU
will not dirty these cache lines after the mappings are established. When this
condition is met, multiple buffers can safely share a cache line without risking
data corruption.
(当调用者映射多个位于同一缓存行内的小缓冲区时，就会出现这种重叠。此时调用者必须保证：在映射建立之后，CPU 不会弄脏 (dirty) 这些缓存行。满足该条件时，多个缓冲区可以安全地共享一个缓存行，而不会有数据损坏的风险。)

All mappings that share a cache line must set this attribute to suppress DMA
debug warnings about overlapping mappings.
(共享同一缓存行的所有映射都必须设置该属性，以抑制 DMA 调试关于映射重叠的告警。)

DMA_ATTR_REQUIRE_COHERENT(要求一致性)
-------------------------------------

DMA mapping requests with the DMA_ATTR_REQUIRE_COHERENT fail on any
system where SWIOTLB or cache management is required. This should only
be used to support uAPI designs that require continuous HW DMA
coherence with userspace processes, for example RDMA and DRM. At a
minimum the memory being mapped must be userspace memory from
pin_user_pages() or similar.
(带 DMA_ATTR_REQUIRE_COHERENT 的 DMA 映射请求，在任何需要 SWIOTLB 或缓存管理的系统上都会失败。它只应用于那些要求与用户空间进程保持持续硬件 DMA 一致性的 uAPI 设计，例如 RDMA 与 DRM。至少要求被映射的内存是来自 pin_user_pages() 或类似接口的用户空间内存。)

Drivers should consider using dma_mmap_pages() instead of this
interface when building their uAPIs, when possible.
(在构建 uAPI 时，驱动应尽可能考虑改用 dma_mmap_pages() 而非该接口。)

It must never be used in an in-kernel driver that only works with
kernel memory.
(它绝不能用于只使用内核内存的内核内驱动。)
