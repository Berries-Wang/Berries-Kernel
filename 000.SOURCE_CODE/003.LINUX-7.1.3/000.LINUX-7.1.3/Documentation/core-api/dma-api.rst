=========================================================================
Dynamic DMA mapping using the generic device(使用通用设备的动态 DMA 映射)
=========================================================================

:Author: James E.J. Bottomley <James.Bottomley@HansenPartnership.com>
(作者: James E.J. Bottomley <James.Bottomley@HansenPartnership.com>)

This document describes the DMA API.  For a more gentle introduction
of the API (and actual examples), see Documentation/core-api/dma-api-howto.rst.
(本文档描述 DMA API。关于该 API 更平易的介绍（以及实际示例），参见 Documentation/core-api/dma-api-howto.rst。)

This API is split into two pieces.  Part I describes the basic API.
Part II describes extensions for supporting non-coherent memory
machines.  Unless you know that your driver absolutely has to support
non-coherent platforms (this is usually only legacy platforms) you
should only use the API described in part I.
(该 API 分为两大部分。Part I 描述基本 API。Part II 描述对支持非一致性内存机器的扩展。除非你确知自己的驱动程序必须支持非一致性平台（通常只有遗留平台），否则你应当只使用 Part I 中描述的 API。)

Part I - DMA API(第 I 部分 - DMA API)
-------------------------------------

To get the DMA API, you must #include <linux/dma-mapping.h>.  This
provides dma_addr_t and the interfaces described below.
(要使用 DMA API，你必须 #include <linux/dma-mapping.h>。它提供 dma_addr_t 以及下文描述的接口。)

A dma_addr_t can hold any valid DMA address for the platform.  It can be
given to a device to use as a DMA source or target.  A CPU cannot reference
a dma_addr_t directly because there may be translation between its physical
address space and the DMA address space.
(dma_addr_t 可以保存平台上任何有效的 DMA 地址。它可以交给设备用作 DMA 源或目标。CPU 不能直接引用 dma_addr_t，因为在 CPU 的物理地址空间与 DMA 地址空间之间可能存在转换。)

Part Ia - Using large DMA-coherent buffers(第 Ia 部分 - 使用大块 DMA 一致性缓冲区)
----------------------------------------------------------------------------------

::

	void *
	dma_alloc_coherent(struct device *dev, size_t size,
			   dma_addr_t *dma_handle, gfp_t flag)

Coherent memory is memory for which a write by either the device or
the processor can immediately be read by the processor or device
without having to worry about caching effects.  (You may however need
to make sure to flush the processor's write buffers before telling
devices to read that memory.)
(一致性内存（coherent memory）是指设备或处理器任何一方的写入都能立即被另一方读取、无需担心缓存效应的内存。（不过你可能仍需确保在让设备读取该内存之前，先刷新处理器的写缓冲区。）)

This routine allocates a region of <size> bytes of coherent memory.
(该例程分配一段 <size> 字节的一致性内存区域。)

It returns a pointer to the allocated region (in the processor's virtual
address space) or NULL if the allocation failed.
(它返回指向所分配区域的指针（位于处理器的虚拟地址空间中）；如果分配失败则返回 NULL。)

It also returns a <dma_handle> which may be cast to an unsigned integer the
same width as the bus and given to the device as the DMA address base of
the region.
(它还返回一个 <dma_handle>，可以把它转换为与总线等宽的无符号整数，并作为该区域的 DMA 地址基址交给设备。)

Note: coherent memory can be expensive on some platforms, and the
minimum allocation length may be as big as a page, so you should
consolidate your requests for coherent memory as much as possible.
The simplest way to do that is to use the dma_pool calls (see below).
(注意：一致性内存在某些平台上可能很昂贵，且最小分配长度可能达到一整页，因此你应当尽可能合并对一致性内存的请求。最简单的做法是使用 dma_pool 调用（见下文）。)

The flag parameter allows the caller to specify the ``GFP_`` flags (see
kmalloc()) for the allocation (the implementation may ignore flags that affect
the location of the returned memory, like GFP_DMA).
(flag 参数允许调用者为分配指定 ``GFP_`` 标志（参见 kmalloc()）（实现可能忽略影响返回内存位置的标志，例如 GFP_DMA）。)

::

	void
	dma_free_coherent(struct device *dev, size_t size, void *cpu_addr,
			  dma_addr_t dma_handle)

Free a previously allocated region of coherent memory.  dev, size and dma_handle
must all be the same as those passed into dma_alloc_coherent().  cpu_addr must
be the virtual address returned by dma_alloc_coherent().
(释放之前分配的一致性内存区域。dev、size 和 dma_handle 必须与传入 dma_alloc_coherent() 的完全相同。cpu_addr 必须是 dma_alloc_coherent() 返回的虚拟地址。)

Note that unlike the sibling allocation call, this routine may only be called
with IRQs enabled.
(注意，与同族的分配调用不同，该例程只能在 IRQ 启用时调用。)


Part Ib - Using small DMA-coherent buffers(第 Ib 部分 - 使用小块 DMA 一致性缓冲区)
----------------------------------------------------------------------------------

To get this part of the DMA API, you must #include <linux/dmapool.h>
(要使用 DMA API 的这一部分，你必须 #include <linux/dmapool.h>)

Many drivers need lots of small DMA-coherent memory regions for DMA
descriptors or I/O buffers.  Rather than allocating in units of a page
or more using dma_alloc_coherent(), you can use DMA pools.  These work
much like a struct kmem_cache, except that they use the DMA-coherent allocator,
not __get_free_pages().  Also, they understand common hardware constraints
for alignment, like queue heads needing to be aligned on N-byte boundaries.
(许多驱动需要大量小块 DMA 一致性内存区域用作 DMA 描述符或 I/O 缓冲区。与其用 dma_alloc_coherent() 以一页或更大为单位分配，不如使用 DMA 池（DMA pool）。它们的工作方式很像 struct kmem_cache，只是使用 DMA 一致性分配器而不是 __get_free_pages()。此外，它们理解常见的硬件对齐约束，比如队列头需要按 N 字节边界对齐。)

.. kernel-doc:: mm/dmapool.c
   :export:

.. kernel-doc:: include/linux/dmapool.h


Part Ic - DMA addressing limitations(第 Ic 部分 - DMA 寻址限制)
---------------------------------------------------------------

DMA mask is a bit mask of the addressable region for the device. In other words,
if applying the DMA mask (a bitwise AND operation) to the DMA address of a
memory region does not clear any bits in the address, then the device can
perform DMA to that memory region.
(DMA 掩码是设备可寻址区域的位掩码。换句话说，如果把 DMA 掩码（按位与运算）应用到一个内存区域的 DMA 地址上之后地址中没有任何位被清零，那么设备就可以对该内存区域执行 DMA。)

All the below functions which set a DMA mask may fail if the requested mask
cannot be used with the device, or if the device is not capable of doing DMA.
(下面所有设置 DMA 掩码的函数，在请求的掩码无法与该设备一起使用、或该设备不具备 DMA 能力时都可能失败。)

::

	int
	dma_set_mask_and_coherent(struct device *dev, u64 mask)

Updates both streaming and coherent DMA masks.
(同时更新流式和一致性 DMA 掩码。)

Returns: 0 if successful and a negative error if not.
(返回：成功时为 0，失败时为负错误码。)

::

	int
	dma_set_mask(struct device *dev, u64 mask)

Updates only the streaming DMA mask.
(只更新流式 DMA 掩码。)

Returns: 0 if successful and a negative error if not.
(返回：成功时为 0，失败时为负错误码。)

::

	int
	dma_set_coherent_mask(struct device *dev, u64 mask)

Updates only the coherent DMA mask.
(只更新一致性 DMA 掩码。)

Returns: 0 if successful and a negative error if not.
(返回：成功时为 0，失败时为负错误码。)

::

	u64
	dma_get_required_mask(struct device *dev)

This API returns the mask that the platform requires to
operate efficiently.  Usually this means the returned mask
is the minimum required to cover all of memory.  Examining the
required mask gives drivers with variable descriptor sizes the
opportunity to use smaller descriptors as necessary.
(该 API 返回平台高效运作所需的掩码。通常这意味着返回的掩码是覆盖全部内存所需的最小掩码。检查所需掩码，让拥有可变描述符大小的驱动有机会按需使用更小的描述符。)

Requesting the required mask does not alter the current mask.  If you
wish to take advantage of it, you should issue a dma_set_mask()
call to set the mask to the value returned.
(请求所需掩码不会改变当前掩码。如果你想利用它，应当调用 dma_set_mask() 把掩码设置为返回的值。)

::

	size_t
	dma_max_mapping_size(struct device *dev);

Returns the maximum size of a mapping for the device. The size parameter
of the mapping functions like dma_map_single(), dma_map_page() and
others should not be larger than the returned value.
(返回设备单个映射的最大大小。dma_map_single()、dma_map_page() 等映射函数的 size 参数不应大于该返回值。)

::

	size_t
	dma_opt_mapping_size(struct device *dev);

Returns the maximum optimal size of a mapping for the device.
(返回设备单个映射的最大最优大小。)

Mapping larger buffers may take much longer in certain scenarios. In
addition, for high-rate short-lived streaming mappings, the upfront time
spent on the mapping may account for an appreciable part of the total
request lifetime. As such, if splitting larger requests incurs no
significant performance penalty, then device drivers are advised to
limit total DMA streaming mappings length to the returned value.
(在某些场景下，映射更大的缓冲区可能耗时久得多。此外，对于高频率、短生命周期的流式映射，映射所花费的前置时间可能占请求总生命周期的相当一部分。因此，如果拆分较大的请求不会带来明显的性能损失，建议设备驱动把 DMA 流式映射的总长度限制在该返回值以内。)

::

	bool
	dma_need_sync(struct device *dev, dma_addr_t dma_addr);

Returns %true if dma_sync_single_for_{device,cpu} calls are required to
transfer memory ownership.  Returns %false if those calls can be skipped.
(如果转移内存所有权需要调用 dma_sync_single_for_{device,cpu}，则返回 %true。如果可以跳过这些调用，则返回 %false。)

::

	unsigned long
	dma_get_merge_boundary(struct device *dev);

Returns the DMA merge boundary. If the device cannot merge any DMA address
segments, the function returns 0.
(返回 DMA 合并边界。如果设备不能合并任何 DMA 地址段，该函数返回 0。)

Part Id - Streaming DMA mappings(第 Id 部分 - 流式 DMA 映射)
------------------------------------------------------------

Streaming DMA allows to map an existing buffer for DMA transfers and then
unmap it when finished.  Map functions are not guaranteed to succeed, so the
return value must be checked.
(流式 DMA 允许把已有缓冲区映射用于 DMA 传输，并在完成之后解除映射。映射函数不保证成功，因此必须检查返回值。)

.. note::

	In particular, mapping may fail for memory not addressable by the
	device, e.g. if it is not within the DMA mask of the device and/or a
	connecting bus bridge.  Streaming DMA functions try to overcome such
	addressing constraints, either by using an IOMMU (a device which maps
	I/O DMA addresses to physical memory addresses), or by copying the
	data to/from a bounce buffer if the kernel is configured with a
	:doc:`SWIOTLB <swiotlb>`.  However, these methods are not always
	available, and even if they are, they may still fail for a number of
	reasons.
	(特别地，对于设备无法寻址的内存，映射可能失败，例如内存不在设备和/或所连接总线桥的 DMA 掩码范围内。流式 DMA 函数会尝试克服这类寻址限制：要么使用 IOMMU（一种把 I/O DMA 地址映射到物理内存地址的设备），要么在内核配置了 :doc:`SWIOTLB <swiotlb>` 时通过弹跳缓冲区（bounce buffer）复制数据。然而，这些方法并非总是可用，即使可用，也可能因多种原因而失败。)

	In short, a device driver may need to be wary of where buffers are
	located in physical memory, especially if the DMA mask is less than 32
	bits.
	(简而言之，设备驱动可能需要留意缓冲区在物理内存中的位置，尤其是当 DMA 掩码小于 32 位时。)

::

	dma_addr_t
	dma_map_single(struct device *dev, void *cpu_addr, size_t size,
		       enum dma_data_direction direction)

Maps a piece of processor virtual memory so it can be accessed by the
device and returns the DMA address of the memory.
(映射一段处理器虚拟内存使其可被设备访问，并返回该内存的 DMA 地址。)

The DMA API uses a strongly typed enumerator for its direction:
(DMA API 使用强类型枚举表示方向：)

======================= =============================================
DMA_NONE		no direction (used for debugging) (无方向（用于调试）)
DMA_TO_DEVICE		data is going from the memory to the device (数据从内存流向设备)
DMA_FROM_DEVICE		data is coming from the device to the memory (数据从设备流向内存)
DMA_BIDIRECTIONAL	direction isn't known (方向未知)
======================= =============================================

.. note::

	Contiguous kernel virtual space may not be contiguous as
	physical memory.  Since this API does not provide any scatter/gather
	capability, it will fail if the user tries to map a non-physically
	contiguous piece of memory.  For this reason, memory to be mapped by
	this API should be obtained from sources which guarantee it to be
	physically contiguous (like kmalloc).
	(连续的内核虚拟空间在物理内存中未必连续。由于该 API 不提供任何分散/聚集（scatter/gather）能力，如果用户试图映射物理上不连续的内存，它将失败。因此，要用该 API 映射的内存应当从能保证物理连续的来源获得（例如 kmalloc）。)

.. warning::

	Memory coherency operates at a granularity called the cache
	line width.  In order for memory mapped by this API to operate
	correctly, the mapped region must begin exactly on a cache line
	boundary and end exactly on one (to prevent two separately mapped
	regions from sharing a single cache line).  Since the cache line size
	may not be known at compile time, the API will not enforce this
	requirement.  Therefore, it is recommended that driver writers who
	don't take special care to determine the cache line size at run time
	only map virtual regions that begin and end on page boundaries (which
	are guaranteed also to be cache line boundaries).
	(内存一致性以称为缓存行（cache line）宽度的粒度运作。要让该 API 映射的内存正确工作，被映射区域必须恰好从缓存行边界开始、并恰好在一个缓存行边界结束（以防止两个分别映射的区域共享同一条缓存行）。由于编译时可能不知道缓存行大小，该 API 不会强制这一要求。因此，建议不在运行时专门确定缓存行大小的驱动编写者，只映射起止都位于页边界上的虚拟区域（页边界保证也是缓存行边界）。)

	DMA_TO_DEVICE synchronisation must be done after the last modification
	of the memory region by the software and before it is handed off to
	the device.  Once this primitive is used, memory covered by this
	primitive should be treated as read-only by the device.  If the device
	may write to it at any point, it should be DMA_BIDIRECTIONAL (see
	below).
	(DMA_TO_DEVICE 同步必须在软件最后一次修改内存区域之后、把它移交给设备之前完成。一旦使用了该原语，该原语覆盖的内存就应被设备当作只读。如果设备随时可能写它，则应使用 DMA_BIDIRECTIONAL（见下文）。)

	DMA_FROM_DEVICE synchronisation must be done before the driver
	accesses data that may be changed by the device.  This memory should
	be treated as read-only by the driver.  If the driver needs to write
	to it at any point, it should be DMA_BIDIRECTIONAL (see below).
	(DMA_FROM_DEVICE 同步必须在驱动访问可能已被设备更改的数据之前完成。该内存应被驱动当作只读。如果驱动随时需要写它，则应使用 DMA_BIDIRECTIONAL（见下文）。)

	DMA_BIDIRECTIONAL requires special handling: it means that the driver
	isn't sure if the memory was modified before being handed off to the
	device and also isn't sure if the device will also modify it.  Thus,
	you must always sync bidirectional memory twice: once before the
	memory is handed off to the device (to make sure all memory changes
	are flushed from the processor) and once before the data may be
	accessed after being used by the device (to make sure any processor
	cache lines are updated with data that the device may have changed).
	(DMA_BIDIRECTIONAL 需要特殊处理：它表示驱动既不确定内存移交给设备之前是否被修改过，也不确定设备是否还会修改它。因此，你必须始终对双向内存同步两次：一次在内存移交给设备之前（确保所有内存修改都已从处理器刷出），一次在数据被设备使用之后、可能被访问之前（确保处理器的所有缓存行都用设备可能已更改的数据更新）。)

::

	void
	dma_unmap_single(struct device *dev, dma_addr_t dma_addr, size_t size,
			 enum dma_data_direction direction)

Unmaps the region previously mapped.  All the parameters passed in
must be identical to those passed to (and returned by) dma_map_single().
(解除之前映射的区域。所有传入的参数必须与传给 dma_map_single()（并由其返回）的完全相同。)

::

	dma_addr_t
	dma_map_page(struct device *dev, struct page *page,
		     unsigned long offset, size_t size,
		     enum dma_data_direction direction)

	void
	dma_unmap_page(struct device *dev, dma_addr_t dma_address, size_t size,
		       enum dma_data_direction direction)

API for mapping and unmapping for pages.  All the notes and warnings
for the other mapping APIs apply here.  Also, although the <offset>
and <size> parameters are provided to do partial page mapping, it is
recommended that you never use these unless you really know what the
cache width is.
(用于映射和解除映射页面的 API。其他映射 API 的所有注意事项和警告在此同样适用。此外，虽然提供了 <offset> 和 <size> 参数用于部分页映射，但除非你确切知道缓存行宽度，否则建议永远不要使用它们。)

::

	dma_addr_t
	dma_map_resource(struct device *dev, phys_addr_t phys_addr, size_t size,
			 enum dma_data_direction dir, unsigned long attrs)

	void
	dma_unmap_resource(struct device *dev, dma_addr_t addr, size_t size,
			   enum dma_data_direction dir, unsigned long attrs)

API for mapping and unmapping for MMIO resources. All the notes and
warnings for the other mapping APIs apply here. The API should only be
used to map device MMIO resources, mapping of RAM is not permitted.
(用于映射和解除映射 MMIO 资源的 API。其他映射 API 的所有注意事项和警告在此同样适用。该 API 只应用于映射设备的 MMIO 资源，不允许映射 RAM。)

::

	int
	dma_mapping_error(struct device *dev, dma_addr_t dma_addr)

In some circumstances dma_map_single(), dma_map_page() and dma_map_resource()
will fail to create a mapping. A driver can check for these errors by testing
the returned DMA address with dma_mapping_error(). A non-zero return value
means the mapping could not be created and the driver should take appropriate
action (e.g. reduce current DMA mapping usage or delay and try again later).
(在某些情况下，dma_map_single()、dma_map_page() 和 dma_map_resource() 会无法创建映射。驱动可以通过用 dma_mapping_error() 测试返回的 DMA 地址来检查这些错误。非零返回值表示无法创建映射，驱动应当采取适当的措施（例如减少当前的 DMA 映射使用量，或者延迟并稍后重试）。)

::

	int
	dma_map_sg(struct device *dev, struct scatterlist *sg,
		   int nents, enum dma_data_direction direction)

Maps a scatter/gather list for DMA. Returns the number of DMA address segments
mapped, which may be smaller than <nents> passed in if several consecutive
sglist entries are merged (e.g. with an IOMMU, or if some adjacent segments
just happen to be physically contiguous).
(为 DMA 映射一个分散/聚集（scatter/gather）列表。返回被映射的 DMA 地址段数量；如果若干连续的 sglist 条目被合并（例如使用 IOMMU，或某些相邻段恰好物理连续），该数量可能小于传入的 <nents>。)

Please note that the sg cannot be mapped again if it has been mapped once.
The mapping process is allowed to destroy information in the sg.
(请注意，sg 一旦被映射过就不能再次映射。映射过程允许破坏 sg 中的信息。)

As with the other mapping interfaces, dma_map_sg() can fail. When it
does, 0 is returned and a driver must take appropriate action. It is
critical that the driver do something, in the case of a block driver
aborting the request or even oopsing is better than doing nothing and
corrupting the filesystem.
(与其他映射接口一样，dma_map_sg() 也可能失败。失败时返回 0，驱动必须采取适当的措施。关键是驱动必须有所动作：对于块设备驱动来说，中止请求甚至直接崩溃（oops）都比什么都不做、破坏文件系统要好。)

With scatterlists, you use the resulting mapping like this::

	int i, count = dma_map_sg(dev, sglist, nents, direction);
	struct scatterlist *sg;

	for_each_sg(sglist, sg, count, i) {
		hw_address[i] = sg_dma_address(sg);
		hw_len[i] = sg_dma_len(sg);
	}

(对于分散列表（scatterlist），你像下面这样使用得到的映射：)

where nents is the number of entries in the sglist.
(其中 nents 是 sglist 中的条目数。)

The implementation is free to merge several consecutive sglist entries
into one.  The returned number is the actual number of sg entries it
mapped them to. On failure, 0 is returned.
(实现可以自由地把若干连续的 sglist 条目合并为一个。返回的数字是它实际映射到的 sg 条目数量。失败时返回 0。)

Then you should loop count times (note: this can be less than nents times)
and use sg_dma_address() and sg_dma_len() macros where you previously
accessed sg->address and sg->length as shown above.
(然后你应当循环 count 次（注意：这可能小于 nents 次），并在先前访问 sg->address 和 sg->length 的地方改用 sg_dma_address() 和 sg_dma_len() 宏，如上所示。)

::

	void
	dma_unmap_sg(struct device *dev, struct scatterlist *sg,
		     int nents, enum dma_data_direction direction)

Unmap the previously mapped scatter/gather list.  All the parameters
must be the same as those and passed in to the scatter/gather mapping
API.
(解除之前映射的分散/聚集列表。所有参数必须与传给分散/聚集映射 API 的完全相同。)

Note: <nents> must be the number you passed in, *not* the number of
DMA address entries returned.
(注意：<nents> 必须是你传入的数字，*而不是*返回的 DMA 地址条目数。)

::

	void
	dma_sync_single_for_cpu(struct device *dev, dma_addr_t dma_handle,
				size_t size,
				enum dma_data_direction direction)

	void
	dma_sync_single_for_device(struct device *dev, dma_addr_t dma_handle,
				   size_t size,
				   enum dma_data_direction direction)

	void
	dma_sync_sg_for_cpu(struct device *dev, struct scatterlist *sg,
			    int nents,
			    enum dma_data_direction direction)

	void
	dma_sync_sg_for_device(struct device *dev, struct scatterlist *sg,
			       int nents,
			       enum dma_data_direction direction)

Synchronise a single contiguous or scatter/gather mapping for the CPU
and device. With the sync_sg API, all the parameters must be the same
as those passed into the sg mapping API. With the sync_single API,
you can use dma_handle and size parameters that aren't identical to
those passed into the single mapping API to do a partial sync.
(为 CPU 和设备同步单个连续映射或分散/聚集映射。使用 sync_sg API 时，所有参数必须与传给 sg 映射 API 的完全相同。使用 sync_single API 时，你可以使用与传给 single 映射 API 不完全相同的 dma_handle 和 size 参数来做部分同步。)


.. note::

   You must do this:

   - Before reading values that have been written by DMA from the device
     (use the DMA_FROM_DEVICE direction)
   - After writing values that will be written to the device using DMA
     (use the DMA_TO_DEVICE) direction
   - before *and* after handing memory to the device if the memory is
     DMA_BIDIRECTIONAL
   (你必须这样做：

   - 在读取由设备通过 DMA 写入的值之前 (使用 DMA_FROM_DEVICE 方向)
   - 在写入将通过 DMA 送到设备的值之后 (使用 DMA_TO_DEVICE 方向)
   - 如果内存是 DMA_BIDIRECTIONAL，在把内存移交给设备之前*和*之后都要做)

See also dma_map_single().
(另见 dma_map_single()。)

::

	dma_addr_t
	dma_map_single_attrs(struct device *dev, void *cpu_addr, size_t size,
			     enum dma_data_direction dir,
			     unsigned long attrs)

	void
	dma_unmap_single_attrs(struct device *dev, dma_addr_t dma_addr,
			       size_t size, enum dma_data_direction dir,
			       unsigned long attrs)

	int
	dma_map_sg_attrs(struct device *dev, struct scatterlist *sgl,
			 int nents, enum dma_data_direction dir,
			 unsigned long attrs)

	void
	dma_unmap_sg_attrs(struct device *dev, struct scatterlist *sgl,
			   int nents, enum dma_data_direction dir,
			   unsigned long attrs)

The four functions above are just like the counterpart functions
without the _attrs suffixes, except that they pass an optional
dma_attrs.
(上面四个函数与不带 _attrs 后缀的对应函数一样，只是它们传递一个可选的 dma_attrs。)

The interpretation of DMA attributes is architecture-specific, and
each attribute should be documented in
Documentation/core-api/dma-attributes.rst.
(DMA 属性的解释是体系结构相关的，每个属性都应当在 Documentation/core-api/dma-attributes.rst 中有文档。)

If dma_attrs are 0, the semantics of each of these functions
is identical to those of the corresponding function
without the _attrs suffix. As a result dma_map_single_attrs()
can generally replace dma_map_single(), etc.
(如果 dma_attrs 为 0，这些函数中每一个的语义都与不带 _attrs 后缀的对应函数相同。因此 dma_map_single_attrs() 通常可以替代 dma_map_single() 等。)

As an example of the use of the ``*_attrs`` functions, here's how
you could pass an attribute DMA_ATTR_FOO when mapping memory
for DMA::

	#include <linux/dma-mapping.h>
	/* DMA_ATTR_FOO should be defined in linux/dma-mapping.h and
	* documented in Documentation/core-api/dma-attributes.rst */
	...

		unsigned long attr;
		attr |= DMA_ATTR_FOO;
		....
		n = dma_map_sg_attrs(dev, sg, nents, DMA_TO_DEVICE, attr);
		....

(作为 ``*_attrs`` 函数用法的示例，下面展示了在为 DMA 映射内存时如何传递属性 DMA_ATTR_FOO：)

Architectures that care about DMA_ATTR_FOO would check for its
presence in their implementations of the mapping and unmapping
routines, e.g.:::

	void whizco_dma_map_sg_attrs(struct device *dev, dma_addr_t dma_addr,
				     size_t size, enum dma_data_direction dir,
				     unsigned long attrs)
	{
		....
		if (attrs & DMA_ATTR_FOO)
			/* twizzle the frobnozzle */
		....
	}

(关心 DMA_ATTR_FOO 的体系结构会在其映射和解除映射例程的实现中检查它是否存在，例如：)

Part Ie - IOVA-based DMA mappings(第 Ie 部分 - 基于 IOVA 的 DMA 映射)
---------------------------------------------------------------------

These APIs allow a very efficient mapping when using an IOMMU.  They are an
optional path that requires extra code and are only recommended for drivers
where DMA mapping performance, or the space usage for storing the DMA addresses
matter.  All the considerations from the previous section apply here as well.
(这些 API 在使用 IOMMU 时可以实现非常高效的映射。它们是一条需要额外代码的可选路径，只推荐给关心 DMA 映射性能或存储 DMA 地址所需空间占用的驱动使用。上一节的所有注意事项在此同样适用。)

::

    bool dma_iova_try_alloc(struct device *dev, struct dma_iova_state *state,
		phys_addr_t phys, size_t size);

Is used to try to allocate IOVA space for mapping operation.  If it returns
false this API can't be used for the given device and the normal streaming
DMA mapping API should be used.  The ``struct dma_iova_state`` is allocated
by the driver and must be kept around until unmap time.
(用于尝试为映射操作分配 IOVA 空间。如果它返回 false，则该 API 不能用于给定设备，应当改用普通的流式 DMA 映射 API。``struct dma_iova_state`` 由驱动分配，并必须保留到解除映射之时。)

::

    static inline bool dma_use_iova(struct dma_iova_state *state)

Can be used by the driver to check if the IOVA-based API is used after a
call to dma_iova_try_alloc.  This can be useful in the unmap path.
(驱动可以在调用 dma_iova_try_alloc 之后用它来检查是否使用了基于 IOVA 的 API。这在解除映射的路径上可能有用。)

::

    int dma_iova_link(struct device *dev, struct dma_iova_state *state,
		phys_addr_t phys, size_t offset, size_t size,
		enum dma_data_direction dir, unsigned long attrs);

Is used to link ranges to the IOVA previously allocated.  The start of all
but the first call to dma_iova_link for a given state must be aligned
to the DMA merge boundary returned by ``dma_get_merge_boundary())``, and
the size of all but the last range must be aligned to the DMA merge boundary
as well.
(用于把地址范围链接到之前分配的 IOVA。对于给定的 state，除第一次之外的所有 dma_iova_link 调用的起始地址都必须按 ``dma_get_merge_boundary())`` 返回的 DMA 合并边界对齐，除最后一个范围之外的所有范围大小也必须按 DMA 合并边界对齐。)

::

    int dma_iova_sync(struct device *dev, struct dma_iova_state *state,
		size_t offset, size_t size);

Must be called to sync the IOMMU page tables for IOVA-range mapped by one or
more calls to ``dma_iova_link()``.
(必须调用它来为通过一次或多次 ``dma_iova_link()`` 调用映射的 IOVA 范围同步 IOMMU 页表。)

For drivers that use a one-shot mapping, all ranges can be unmapped and the
IOVA freed by calling:
(对于使用一次性映射的驱动，可以通过调用以下函数解除所有范围的映射并释放 IOVA：)

::

   void dma_iova_destroy(struct device *dev, struct dma_iova_state *state,
		size_t mapped_len, enum dma_data_direction dir,
                unsigned long attrs);

Alternatively drivers can dynamically manage the IOVA space by unmapping
and mapping individual regions.  In that case
(或者，驱动可以通过逐个解除映射和映射各个区域来动态管理 IOVA 空间。在这种情况下)

::

    void dma_iova_unlink(struct device *dev, struct dma_iova_state *state,
	size_t offset, size_t size, enum dma_data_direction dir,
	unsigned long attrs);

is used to unmap a range previously mapped, and
(用于解除之前映射的一个范围的映射；而)

::

   void dma_iova_free(struct device *dev, struct dma_iova_state *state);

is used to free the IOVA space.  All regions must have been unmapped using
``dma_iova_unlink()`` before calling ``dma_iova_free()``.
(用于释放 IOVA 空间。在调用 ``dma_iova_free()`` 之前，所有区域必须已经用 ``dma_iova_unlink()`` 解除过映射。)

Part II - Non-coherent DMA allocations(第 II 部分 - 非一致性 DMA 分配)
----------------------------------------------------------------------

These APIs allow to allocate pages that are guaranteed to be DMA addressable
by the passed in device, but which need explicit management of memory ownership
for the kernel vs the device.
(这些 API 允许分配保证可被传入设备 DMA 寻址的页面，但需要显式管理内核与设备之间的内存所有权。)

If you don't understand how cache line coherency works between a processor and
an I/O device, you should not be using this part of the API.
(如果你不理解处理器与 I/O 设备之间缓存行一致性的工作方式，就不应当使用 API 的这一部分。)

::

	struct page *
	dma_alloc_pages(struct device *dev, size_t size, dma_addr_t *dma_handle,
			enum dma_data_direction dir, gfp_t gfp)

This routine allocates a region of <size> bytes of non-coherent memory.  It
returns a pointer to first struct page for the region, or NULL if the
allocation failed. The resulting struct page can be used for everything a
struct page is suitable for.
(该例程分配一段 <size> 字节的非一致性内存。它返回该区域第一个 struct page 的指针，分配失败则返回 NULL。得到的 struct page 可以用于任何 struct page 适用的场合。)

It also returns a <dma_handle> which may be cast to an unsigned integer the
same width as the bus and given to the device as the DMA address base of
the region.
(它还返回一个 <dma_handle>，可以把它转换为与总线等宽的无符号整数，并作为该区域的 DMA 地址基址交给设备。)

The dir parameter specified if data is read and/or written by the device,
see dma_map_single() for details.
(dir 参数指明数据是否由设备读取和/或写入，详见 dma_map_single()。)

The gfp parameter allows the caller to specify the ``GFP_`` flags (see
kmalloc()) for the allocation, but rejects flags used to specify a memory
zone such as GFP_DMA or GFP_HIGHMEM.
(gfp 参数允许调用者为分配指定 ``GFP_`` 标志（参见 kmalloc()），但拒绝用于指定内存区域的标志，例如 GFP_DMA 或 GFP_HIGHMEM。)

Before giving the memory to the device, dma_sync_single_for_device() needs
to be called, and before reading memory written by the device,
dma_sync_single_for_cpu(), just like for streaming DMA mappings that are
reused.
(在把内存交给设备之前需要调用 dma_sync_single_for_device()，在读取设备写入的内存之前调用 dma_sync_single_for_cpu()，就像对待被复用的流式 DMA 映射那样。)

::

	void
	dma_free_pages(struct device *dev, size_t size, struct page *page,
			dma_addr_t dma_handle, enum dma_data_direction dir)

Free a region of memory previously allocated using dma_alloc_pages().
dev, size, dma_handle and dir must all be the same as those passed into
dma_alloc_pages().  page must be the pointer returned by dma_alloc_pages().
(释放之前用 dma_alloc_pages() 分配的内存区域。dev、size、dma_handle 和 dir 必须与传入 dma_alloc_pages() 的完全相同。page 必须是 dma_alloc_pages() 返回的指针。)

::

	int
	dma_mmap_pages(struct device *dev, struct vm_area_struct *vma,
		       size_t size, struct page *page)

Map an allocation returned from dma_alloc_pages() into a user address space.
dev and size must be the same as those passed into dma_alloc_pages().
page must be the pointer returned by dma_alloc_pages().
(把 dma_alloc_pages() 返回的分配映射到用户地址空间。dev 和 size 必须与传入 dma_alloc_pages() 的完全相同。page 必须是 dma_alloc_pages() 返回的指针。)

::

	void *
	dma_alloc_noncoherent(struct device *dev, size_t size,
			dma_addr_t *dma_handle, enum dma_data_direction dir,
			gfp_t gfp)

This routine is a convenient wrapper around dma_alloc_pages that returns the
kernel virtual address for the allocated memory instead of the page structure.
(该例程是围绕 dma_alloc_pages 的便捷封装，返回所分配内存的内核虚拟地址而不是页结构。)

::

	void
	dma_free_noncoherent(struct device *dev, size_t size, void *cpu_addr,
			dma_addr_t dma_handle, enum dma_data_direction dir)

Free a region of memory previously allocated using dma_alloc_noncoherent().
dev, size, dma_handle and dir must all be the same as those passed into
dma_alloc_noncoherent().  cpu_addr must be the virtual address returned by
dma_alloc_noncoherent().
(释放之前用 dma_alloc_noncoherent() 分配的内存区域。dev、size、dma_handle 和 dir 必须与传入 dma_alloc_noncoherent() 的完全相同。cpu_addr 必须是 dma_alloc_noncoherent() 返回的虚拟地址。)

::

	struct sg_table *
	dma_alloc_noncontiguous(struct device *dev, size_t size,
				enum dma_data_direction dir, gfp_t gfp,
				unsigned long attrs);

This routine allocates  <size> bytes of non-coherent and possibly non-contiguous
memory.  It returns a pointer to struct sg_table that describes the allocated
and DMA mapped memory, or NULL if the allocation failed. The resulting memory
can be used for struct page mapped into a scatterlist are suitable for.
(该例程分配 <size> 字节的非一致性且可能不连续的内存。它返回描述已分配并已 DMA 映射内存的 struct sg_table 指针，分配失败则返回 NULL。得到的内存可以用于映射进分散列表（scatterlist）的 struct page 所适用的任何场合。)

The return sg_table is guaranteed to have 1 single DMA mapped segment as
indicated by sgt->nents, but it might have multiple CPU side segments as
indicated by sgt->orig_nents.
(返回的 sg_table 保证只有一个 DMA 映射段（由 sgt->nents 指示），但可能有多个 CPU 侧的段（由 sgt->orig_nents 指示）。)

The dir parameter specified if data is read and/or written by the device,
see dma_map_single() for details.
(dir 参数指明数据是否由设备读取和/或写入，详见 dma_map_single()。)

The gfp parameter allows the caller to specify the ``GFP_`` flags (see
kmalloc()) for the allocation, but rejects flags used to specify a memory
zone such as GFP_DMA or GFP_HIGHMEM.
(gfp 参数允许调用者为分配指定 ``GFP_`` 标志（参见 kmalloc()），但拒绝用于指定内存区域的标志，例如 GFP_DMA 或 GFP_HIGHMEM。)

The attrs argument must be either 0 or DMA_ATTR_ALLOC_SINGLE_PAGES.
(attrs 参数必须是 0 或 DMA_ATTR_ALLOC_SINGLE_PAGES。)

Before giving the memory to the device, dma_sync_sgtable_for_device() needs
to be called, and before reading memory written by the device,
dma_sync_sgtable_for_cpu(), just like for streaming DMA mappings that are
reused.
(在把内存交给设备之前需要调用 dma_sync_sgtable_for_device()，在读取设备写入的内存之前调用 dma_sync_sgtable_for_cpu()，就像对待被复用的流式 DMA 映射那样。)

::

	void
	dma_free_noncontiguous(struct device *dev, size_t size,
			       struct sg_table *sgt,
			       enum dma_data_direction dir)

Free memory previously allocated using dma_alloc_noncontiguous().  dev, size,
and dir must all be the same as those passed into dma_alloc_noncontiguous().
sgt must be the pointer returned by dma_alloc_noncontiguous().
(释放之前用 dma_alloc_noncontiguous() 分配的内存。dev、size 和 dir 必须与传入 dma_alloc_noncontiguous() 的完全相同。sgt 必须是 dma_alloc_noncontiguous() 返回的指针。)

::

	void *
	dma_vmap_noncontiguous(struct device *dev, size_t size,
		struct sg_table *sgt)

Return a contiguous kernel mapping for an allocation returned from
dma_alloc_noncontiguous().  dev and size must be the same as those passed into
dma_alloc_noncontiguous().  sgt must be the pointer returned by
dma_alloc_noncontiguous().
(为 dma_alloc_noncontiguous() 返回的分配返回一个连续的内核映射。dev 和 size 必须与传入 dma_alloc_noncontiguous() 的完全相同。sgt 必须是 dma_alloc_noncontiguous() 返回的指针。)

Once a non-contiguous allocation is mapped using this function, the
flush_kernel_vmap_range() and invalidate_kernel_vmap_range() APIs must be used
to manage the coherency between the kernel mapping, the device and user space
mappings (if any).
(一旦用该函数映射了非连续分配，就必须使用 flush_kernel_vmap_range() 和 invalidate_kernel_vmap_range() API 来管理内核映射、设备与用户空间映射（如果有的话）之间的一致性。)

::

	void
	dma_vunmap_noncontiguous(struct device *dev, void *vaddr)

Unmap a kernel mapping returned by dma_vmap_noncontiguous().  dev must be the
same the one passed into dma_alloc_noncontiguous().  vaddr must be the pointer
returned by dma_vmap_noncontiguous().
(解除 dma_vmap_noncontiguous() 返回的内核映射。dev 必须与传入 dma_alloc_noncontiguous() 的相同。vaddr 必须是 dma_vmap_noncontiguous() 返回的指针。)


::

	int
	dma_mmap_noncontiguous(struct device *dev, struct vm_area_struct *vma,
			       size_t size, struct sg_table *sgt)

Map an allocation returned from dma_alloc_noncontiguous() into a user address
space.  dev and size must be the same as those passed into
dma_alloc_noncontiguous().  sgt must be the pointer returned by
dma_alloc_noncontiguous().
(把 dma_alloc_noncontiguous() 返回的分配映射到用户地址空间。dev 和 size 必须与传入 dma_alloc_noncontiguous() 的完全相同。sgt 必须是 dma_alloc_noncontiguous() 返回的指针。)

::

	int
	dma_get_cache_alignment(void)

Returns the processor cache alignment.  This is the absolute minimum
alignment *and* width that you must observe when either mapping
memory or doing partial flushes.
(返回处理器缓存对齐值。这是映射内存或做部分刷新时必须遵守的最小对齐*和*宽度。)

.. note::

	This API may return a number *larger* than the actual cache
	line, but it will guarantee that one or more cache lines fit exactly
	into the width returned by this call.  It will also always be a power
	of two for easy alignment.
	(该 API 可能返回比实际缓存行*更大*的数字，但它保证一条或多条缓存行能恰好放入该调用返回的宽度之中。为了便于对齐，它也总是 2 的幂。)


Part III - Debug drivers use of the DMA API(第 III 部分 - 调试驱动程序对 DMA API 的使用)
----------------------------------------------------------------------------------------

The DMA API as described above has some constraints. DMA addresses must be
released with the corresponding function with the same size for example. With
the advent of hardware IOMMUs it becomes more and more important that drivers
do not violate those constraints. In the worst case such a violation can
result in data corruption up to destroyed filesystems.
(如上所述的 DMA API 有一些约束。例如，DMA 地址必须用对应的函数、以相同的大小释放。随着硬件 IOMMU 的出现，驱动不违反这些约束变得越来越重要。在最坏的情况下，违反约束可能导致数据损坏，甚至毁掉文件系统。)

To debug drivers and find bugs in the usage of the DMA API checking code can
be compiled into the kernel which will tell the developer about those
violations. If your architecture supports it you can select the "Enable
debugging of DMA API usage" option in your kernel configuration. Enabling this
option has a performance impact. Do not enable it in production kernels.
(为了调试驱动并发现 DMA API 使用中的错误，可以把检查代码编译进内核，它会向开发者报告这些违规行为。如果你的体系结构支持，可以在内核配置中选择 "Enable debugging of DMA API usage" 选项。启用该选项会影响性能。不要在生产内核中启用它。)

If you boot the resulting kernel will contain code which does some bookkeeping
about what DMA memory was allocated for which device. If this code detects an
error it prints a warning message with some details into your kernel log. An
example warning message may look like this::

	WARNING: at /data2/repos/linux-2.6-iommu/lib/dma-debug.c:448
		check_unmap+0x203/0x490()
	Hardware name:
	forcedeth 0000:00:08.0: DMA-API: device driver frees DMA memory with wrong
		function [device address=0x00000000640444be] [size=66 bytes] [mapped as
	single] [unmapped as page]
	Modules linked in: nfsd exportfs bridge stp llc r8169
	Pid: 0, comm: swapper Tainted: G        W  2.6.28-dmatest-09289-g8bb99c0 #1
	Call Trace:
	<IRQ>  [<ffffffff80240b22>] warn_slowpath+0xf2/0x130
	[<ffffffff80647b70>] _spin_unlock+0x10/0x30
	[<ffffffff80537e75>] usb_hcd_link_urb_to_ep+0x75/0xc0
	[<ffffffff80647c22>] _spin_unlock_irqrestore+0x12/0x40
	[<ffffffff8055347f>] ohci_urb_enqueue+0x19f/0x7c0
	[<ffffffff80252f96>] queue_work+0x56/0x60
	[<ffffffff80237e10>] enqueue_task_fair+0x20/0x50
	[<ffffffff80539279>] usb_hcd_submit_urb+0x379/0xbc0
	[<ffffffff803b78c3>] cpumask_next_and+0x23/0x40
	[<ffffffff80235177>] find_busiest_group+0x207/0x8a0
	[<ffffffff8064784f>] _spin_lock_irqsave+0x1f/0x50
	[<ffffffff803c7ea3>] check_unmap+0x203/0x490
	[<ffffffff803c8259>] debug_dma_unmap_phys+0x49/0x50
	[<ffffffff80485f26>] nv_tx_done_optimized+0xc6/0x2c0
	[<ffffffff80486c13>] nv_nic_irq_optimized+0x73/0x2b0
	[<ffffffff8026df84>] handle_IRQ_event+0x34/0x70
	[<ffffffff8026ffe9>] handle_edge_irq+0xc9/0x150
	[<ffffffff8020e3ab>] do_IRQ+0xcb/0x1c0
	[<ffffffff8020c093>] ret_from_intr+0x0/0xa
	<EOI> <4>---[ end trace f6435a98e2a38c0e ]---

The driver developer can find the driver and the device including a stacktrace
of the DMA API call which caused this warning.
(驱动开发者可以从中找到驱动和设备，以及引发该警告的 DMA API 调用的栈回溯。)

Per default only the first error will result in a warning message. All other
errors will only silently counted. This limitation exist to prevent the code
from flooding your kernel log. To support debugging a device driver this can
be disabled via debugfs. See the debugfs interface documentation below for
details.
(默认情况下只有第一个错误会产生警告消息，所有其他错误只会被静默计数。这一限制的存在是为了防止代码淹没你的内核日志。为了支持调试某个设备驱动，可以通过 debugfs 禁用该限制。详情参见下文的 debugfs 接口文档。)

The debugfs directory for the DMA API debugging code is called dma-api/. In
this directory the following files can currently be found:
(DMA API 调试代码的 debugfs 目录名为 dma-api/。目前在该目录中可以找到以下文件：)

=============================== ===============================================
dma-api/all_errors		This file contains a numeric value. If this
				value is not equal to zero the debugging code
				will print a warning for every error it finds
				into the kernel log. Be careful with this
				option, as it can easily flood your logs. (该文件包含一个数值。如果该值不为零，调试代码会把它发现的每一个错误都以警告形式打印进内核日志。小心使用该选项，因为它很容易淹没你的日志。)

dma-api/disabled		This read-only file contains the character 'Y'
				if the debugging code is disabled. This can
				happen when it runs out of memory or if it was
				disabled at boot time (该只读文件在调试代码被禁用时包含字符 'Y'。这可能发生在它内存耗尽时，或它在启动时被禁用时。)

dma-api/dump			This read-only file contains current DMA
				mappings. (该只读文件包含当前的 DMA 映射。)

dma-api/error_count		This file is read-only and shows the total
				numbers of errors found. (该文件只读，显示已发现错误的总数。)

dma-api/num_errors		The number in this file shows how many
				warnings will be printed to the kernel log
				before it stops. This number is initialized to
				one at system boot and be set by writing into
				this file (该文件中的数字显示在停止之前会向内核日志打印多少条警告。该数字在系统启动时被初始化为 1，可以通过写入该文件来设置。)

dma-api/min_free_entries	This read-only file can be read to get the
				minimum number of free dma_debug_entries the
				allocator has ever seen. If this value goes
				down to zero the code will attempt to increase
				nr_total_entries to compensate. (可以读取该只读文件来获取分配器所见过的最小空闲 dma_debug_entries 数量。如果该值降到零，代码会尝试增加 nr_total_entries 来补偿。)

dma-api/num_free_entries	The current number of free dma_debug_entries
				in the allocator. (分配器中当前空闲的 dma_debug_entries 数量。)

dma-api/nr_total_entries	The total number of dma_debug_entries in the
				allocator, both free and used. (分配器中 dma_debug_entries 的总数，包括空闲的和已使用的。)

dma-api/driver_filter		You can write a name of a driver into this file
				to limit the debug output to requests from that
				particular driver. Write an empty string to
				that file to disable the filter and see
				all errors again. (你可以把一个驱动的名称写入该文件，把调试输出限制为来自该特定驱动的请求。向该文件写入空字符串可禁用过滤器，重新看到所有错误。)
=============================== ===============================================

If you have this code compiled into your kernel it will be enabled by default.
If you want to boot without the bookkeeping anyway you can provide
'dma_debug=off' as a boot parameter. This will disable DMA API debugging.
Notice that you can not enable it again at runtime. You have to reboot to do
so.
(如果这段代码被编译进了你的内核，它默认是启用的。如果你无论如何都想在没有簿记的情况下启动，可以提供 'dma_debug=off' 作为启动参数。这将禁用 DMA API 调试。注意，运行时无法再次启用它，必须重启才能做到。)

If you want to see debug messages only for a special device driver you can
specify the dma_debug_driver=<drivername> parameter. This will enable the
driver filter at boot time. The debug code will only print errors for that
driver afterwards. This filter can be disabled or changed later using debugfs.
(如果你只想查看特定设备驱动的调试消息，可以指定 dma_debug_driver=<drivername> 参数。这会在启动时启用驱动过滤器。此后调试代码只会为该驱动打印错误。之后可以使用 debugfs 禁用或更改该过滤器。)

When the code disables itself at runtime this is most likely because it ran
out of dma_debug_entries and was unable to allocate more on-demand. 65536
entries are preallocated at boot - if this is too low for you boot with
'dma_debug_entries=<your_desired_number>' to overwrite the default. Note
that the code allocates entries in batches, so the exact number of
preallocated entries may be greater than the actual number requested. The
code will print to the kernel log each time it has dynamically allocated
as many entries as were initially preallocated. This is to indicate that a
larger preallocation size may be appropriate, or if it happens continually
that a driver may be leaking mappings.
(当代码在运行时自我禁用时，最可能的原因是 dma_debug_entries 已用尽且无法按需分配更多。启动时会预分配 65536 个条目——如果这对来说太低，可以用 'dma_debug_entries=<你的期望数量>' 启动来覆盖默认值。注意，代码按批次分配条目，因此预分配条目的确切数量可能大于实际请求的数量。每当它动态分配的条目数达到最初预分配的数量时，代码都会向内核日志打印一条消息。这表示更大的预分配大小可能是合适的；如果这种情况持续发生，则可能说明某个驱动正在泄漏映射。)

::

	void
	debug_dma_mapping_error(struct device *dev, dma_addr_t dma_addr);

dma-debug interface debug_dma_mapping_error() to debug drivers that fail
to check DMA mapping errors on addresses returned by dma_map_single() and
dma_map_page() interfaces. This interface clears a flag set by
debug_dma_map_phys() to indicate that dma_mapping_error() has been called by
the driver. When driver does unmap, debug_dma_unmap() checks the flag and if
this flag is still set, prints warning message that includes call trace that
leads up to the unmap. This interface can be called from dma_mapping_error()
routines to enable DMA mapping error check debugging.
(dma-debug 接口 debug_dma_mapping_error() 用于调试那些没有对 dma_map_single() 和 dma_map_page() 接口返回的地址检查 DMA 映射错误的驱动。该接口清除由 debug_dma_map_phys() 设置的标志，以表明驱动已经调用过 dma_mapping_error()。当驱动执行解除映射时，debug_dma_unmap() 会检查该标志，如果该标志仍然置位，就打印一条包含直到解除映射为止的调用回溯的警告消息。可以从 dma_mapping_error() 例程中调用该接口，以启用 DMA 映射错误检查调试。)

Functions and structures(函数与结构体)
======================================

.. kernel-doc:: include/linux/scatterlist.h
.. kernel-doc:: lib/scatterlist.c
