.. SPDX-License-Identifier: GPL-2.0

===================================
Physical Memory Model(物理内存模型)
===================================

Physical memory in a system may be addressed in different ways. The
simplest case is when the physical memory starts at address 0 and
spans a contiguous range up to the maximal address. It could be,
however, that this range contains small holes that are not accessible
for the CPU. Then there could be several contiguous ranges at
completely distinct addresses. And, don't forget about NUMA, where
different memory banks are attached to different CPUs.
(系统中的物理内存可以用不同的方式寻址。最简单的情况是物理内存从地址 0 开始，并连续延伸到最大地址。然而，该范围中可能包含 CPU 无法访问的小空洞。也可能在完全不同的地址上存在多个连续的范围。而且，不要忘记 NUMA，其中不同的内存条连接到不同的 CPU。)

Linux abstracts this diversity using one of the two memory models:
FLATMEM and SPARSEMEM. Each architecture defines what
memory models it supports, what the default memory model is and
whether it is possible to manually override that default.
(Linux 使用两种内存模型之一来抽象这种多样性：FLATMEM 和 SPARSEMEM。每种架构定义它支持哪些内存模型、默认的内存模型是什么，以及是否可以手动覆盖该默认值。)

All the memory models track the status of physical page frames using
struct page arranged in one or more arrays.
(所有内存模型都使用排列在一个或多个数组中的 struct page 来跟踪物理页帧的状态。)

Regardless of the selected memory model, there exists one-to-one
mapping between the physical page frame number (PFN) and the
corresponding `struct page`.
(无论选择哪种内存模型，物理页帧号 (PFN) 与对应的 `struct page` 之间都存在一一映射。)

Each memory model defines :c:func:`pfn_to_page` and :c:func:`page_to_pfn`
helpers that allow the conversion from PFN to `struct page` and vice
versa.
(每种内存模型都定义了 :c:func:`pfn_to_page` 和 :c:func:`page_to_pfn` 辅助函数，允许在 PFN 与 `struct page` 之间相互转换。)

FLATMEM(平坦内存模型)
=====================

The simplest memory model is FLATMEM. This model is suitable for
non-NUMA systems with contiguous, or mostly contiguous, physical
memory.
(最简单的内存模型是 FLATMEM。该模型适用于物理内存连续或基本连续的非 NUMA 系统。)

In the FLATMEM memory model, there is a global `mem_map` array that
maps the entire physical memory. For most architectures, the holes
have entries in the `mem_map` array. The `struct page` objects
corresponding to the holes are never fully initialized.
(在 FLATMEM 内存模型中，有一个全局的 `mem_map` 数组来映射整个物理内存。对于大多数架构，空洞在 `mem_map` 数组中也有条目。与空洞对应的 `struct page` 对象永远不会被完全初始化。)

To allocate the `mem_map` array, architecture specific setup code should
call :c:func:`free_area_init` function. Yet, the mappings array is not
usable until the call to :c:func:`memblock_free_all` that hands all the
memory to the page allocator.
(要分配 `mem_map` 数组，架构特定的 setup 代码应调用 :c:func:`free_area_init` 函数。然而，在调用把所有内存交给页面分配器的 :c:func:`memblock_free_all` 之前，映射数组是不可用的。)

An architecture may free parts of the `mem_map` array that do not cover the
actual physical pages. In such case, the architecture specific
:c:func:`pfn_valid` implementation should take the holes in the
`mem_map` into account.
(架构可以释放 `mem_map` 数组中不覆盖实际物理页面的部分。在这种情况下，架构特定的 :c:func:`pfn_valid` 实现应将 `mem_map` 中的空洞考虑在内。)

With FLATMEM, the conversion between a PFN and the `struct page` is
straightforward: `PFN - ARCH_PFN_OFFSET` is an index to the
`mem_map` array.
(使用 FLATMEM 时，PFN 与 `struct page` 之间的转换非常直接： `PFN - ARCH_PFN_OFFSET` 是 `mem_map` 数组的索引。)

The `ARCH_PFN_OFFSET` defines the first page frame number for
systems with physical memory starting at address different from 0.
( `ARCH_PFN_OFFSET` 定义了物理内存起始地址不为 0 的系统的第一个页帧号。)

SPARSEMEM(稀疏内存模型)
=======================

SPARSEMEM is the most versatile memory model available in Linux and it
is the only memory model that supports several advanced features such
as hot-plug and hot-remove of the physical memory, alternative memory
maps for non-volatile memory devices and deferred initialization of
the memory map for larger systems.
(SPARSEMEM 是 Linux 中可用的最通用的内存模型，也是唯一支持多种高级特性的内存模型，例如物理内存的热插拔与热移除、非易失性内存设备的备用内存映射，以及为大型系统延迟初始化内存映射。)

The SPARSEMEM model presents the physical memory as a collection of
sections. A section is represented with struct mem_section
that contains `section_mem_map` that is, logically, a pointer to an
array of struct pages. However, it is stored with some other magic
that aids the sections management. The section size and maximal number
of section is specified using `SECTION_SIZE_BITS` and
`MAX_PHYSMEM_BITS` constants defined by each architecture that
supports SPARSEMEM. While `MAX_PHYSMEM_BITS` is an actual width of a
physical address that an architecture supports, the
`SECTION_SIZE_BITS` is an arbitrary value.
(SPARSEMEM 模型将物理内存呈现为节 (section) 的集合。一个节用 struct mem_section 表示，它包含 `section_mem_map`，从逻辑上讲，它是一个指向 struct page 数组的指针。不过，它与其他一些有助于节管理的魔数一起存储。节的大小和最大节数使用 `SECTION_SIZE_BITS` 和 `MAX_PHYSMEM_BITS` 常量指定，这些常量由每个支持 SPARSEMEM 的架构定义。 `MAX_PHYSMEM_BITS` 是架构支持的物理地址的实际宽度，而 `SECTION_SIZE_BITS` 是一个任意值。)

The maximal number of sections is denoted `NR_MEM_SECTIONS` and
defined as

.. math::

   NR\_MEM\_SECTIONS = 2 ^ {(MAX\_PHYSMEM\_BITS - SECTION\_SIZE\_BITS)}

(最大节数记为 `NR_MEM_SECTIONS`，其定义为：)

The `mem_section` objects are arranged in a two-dimensional array
called `mem_sections`. The size and placement of this array depend
on `CONFIG_SPARSEMEM_EXTREME` and the maximal possible number of
sections:
( `mem_section` 对象排列在一个名为 `mem_sections` 的二维数组中。该数组的大小和布局取决于 `CONFIG_SPARSEMEM_EXTREME` 以及最大可能的节数：)

* When `CONFIG_SPARSEMEM_EXTREME` is disabled, the `mem_sections`
  array is static and has `NR_MEM_SECTIONS` rows. Each row holds a
  single `mem_section` object.
  (当 `CONFIG_SPARSEMEM_EXTREME` 被禁用时， `mem_sections` 数组是静态的，有 `NR_MEM_SECTIONS` 行。每行包含一个 `mem_section` 对象。)
* When `CONFIG_SPARSEMEM_EXTREME` is enabled, the `mem_sections`
  array is dynamically allocated. Each row contains PAGE_SIZE worth of
  `mem_section` objects and the number of rows is calculated to fit
  all the memory sections.
  (当 `CONFIG_SPARSEMEM_EXTREME` 被启用时， `mem_sections` 数组是动态分配的。每行包含 PAGE_SIZE 大小的 `mem_section` 对象，行数经计算以容纳所有内存节。)

With SPARSEMEM there are two possible ways to convert a PFN to the
corresponding `struct page` - a "classic sparse" and "sparse
vmemmap". The selection is made at build time and it is determined by
the value of `CONFIG_SPARSEMEM_VMEMMAP`.
(使用 SPARSEMEM 时，有两种可能的方式将 PFN 转换为对应的 `struct page`："classic sparse"(经典稀疏)和 "sparse vmemmap"(稀疏 vmemmap)。选择在构建时做出，由 `CONFIG_SPARSEMEM_VMEMMAP` 的值决定。)

The classic sparse encodes the section number of a page in page->flags
and uses high bits of a PFN to access the section that maps that page
frame. Inside a section, the PFN is the index to the array of pages.
(classic sparse 将页面的节号编码在 page->flags 中，并使用 PFN 的高位访问映射该页帧的节。在节内部，PFN 是页数组的索引。)

The sparse vmemmap uses a virtually mapped memory map to optimize
pfn_to_page and page_to_pfn operations. There is a global `struct
page *vmemmap` pointer that points to a virtually contiguous array of
`struct page` objects. A PFN is an index to that array and the
offset of the `struct page` from `vmemmap` is the PFN of that
page.
(sparse vmemmap 使用虚拟映射的内存映射来优化 pfn_to_page 和 page_to_pfn 操作。有一个全局的 `struct page *vmemmap` 指针，指向一个虚拟连续的 `struct page` 对象数组。PFN 是该数组的索引， `struct page` 相对于 `vmemmap` 的偏移就是该页面的 PFN。)

To use vmemmap, an architecture has to reserve a range of virtual
addresses that will map the physical pages containing the memory
map and make sure that `vmemmap` points to that range. In addition,
the architecture should implement :c:func:`vmemmap_populate` method
that will allocate the physical memory and create page tables for the
virtual memory map. If an architecture does not have any special
requirements for the vmemmap mappings, it can use default
:c:func:`vmemmap_populate_basepages` provided by the generic memory
management.
(要使用 vmemmap，架构必须保留一段虚拟地址范围，用于映射包含内存映射的物理页，并确保 `vmemmap` 指向该范围。此外，架构应实现 :c:func:`vmemmap_populate` 方法，该方法将分配物理内存并为虚拟内存映射创建页表。如果架构对 vmemmap 映射没有任何特殊要求，可以使用通用内存管理提供的默认 :c:func:`vmemmap_populate_basepages`。)

The virtually mapped memory map allows storing `struct page` objects
for persistent memory devices in pre-allocated storage on those
devices. This storage is represented with struct vmem_altmap
that is eventually passed to vmemmap_populate() through a long chain
of function calls. The vmemmap_populate() implementation may use the
`vmem_altmap` along with :c:func:`vmemmap_alloc_block_buf` helper to
allocate memory map on the persistent memory device.
(虚拟映射的内存映射允许将持久性内存设备的 `struct page` 对象存储在这些设备上预分配的存储中。该存储用 struct vmem_altmap 表示，最终通过一长串函数调用传递给 vmemmap_populate()。 vmemmap_populate() 的实现可以将 `vmem_altmap` 与 :c:func:`vmemmap_alloc_block_buf` 辅助函数一起使用，在持久性内存设备上分配内存映射。)

ZONE_DEVICE(ZONE_DEVICE 设备内存区)
===================================
The `ZONE_DEVICE` facility builds upon `SPARSEMEM_VMEMMAP` to offer
`struct page` `mem_map` services for device driver identified physical
address ranges. The "device" aspect of `ZONE_DEVICE` relates to the fact
that the page objects for these address ranges are never marked online,
and that a reference must be taken against the device, not just the page
to keep the memory pinned for active use. `ZONE_DEVICE`, via
:c:func:`devm_memremap_pages`, performs just enough memory hotplug to
turn on :c:func:`pfn_to_page`, :c:func:`page_to_pfn`, and
:c:func:`get_user_pages` service for the given range of pfns. Since the
page reference count never drops below 1 the page is never tracked as
free memory and the page's `struct list_head lru` space is repurposed
for back referencing to the host device / driver that mapped the memory.
( `ZONE_DEVICE` 功能构建在 `SPARSEMEM_VMEMMAP` 之上，为设备驱动所标识的物理地址范围提供 `struct page` `mem_map` 服务。 `ZONE_DEVICE` 的 "device" (设备) 方面与这样一个事实有关：这些地址范围的页对象永远不会被标记为在线，并且必须对设备(而不仅仅是页面)持有引用，以便在活跃使用期间将内存固定住。 `ZONE_DEVICE` 通过 :c:func:`devm_memremap_pages` 执行刚好够用的内存热插拔操作，为给定的 pfn 范围启用 :c:func:`pfn_to_page` 、 :c:func:`page_to_pfn` 和 :c:func:`get_user_pages` 服务。由于页面引用计数永远不会降到 1 以下，该页面永远不会被当作空闲内存来跟踪，并且页面的 `struct list_head lru` 空间被重新用于反向引用映射该内存的主机设备/驱动。)

While `SPARSEMEM` presents memory as a collection of sections,
optionally collected into memory blocks, `ZONE_DEVICE` users have a need
for smaller granularity of populating the `mem_map`. Given that
`ZONE_DEVICE` memory is never marked online it is subsequently never
subject to its memory ranges being exposed through the sysfs memory
hotplug api on memory block boundaries. The implementation relies on
this lack of user-api constraint to allow sub-section sized memory
ranges to be specified to :c:func:`arch_add_memory`, the top-half of
memory hotplug. Sub-section support allows for 2MB as the cross-arch
common alignment granularity for :c:func:`devm_memremap_pages`.
(虽然 `SPARSEMEM` 将内存呈现为节的集合(可选地再组成内存块)，但 `ZONE_DEVICE` 用户需要更细的粒度来填充 `mem_map`。鉴于 `ZONE_DEVICE` 内存永远不会被标记为在线，因此它随后永远不会受制于"其内存范围通过 sysfs 内存热插拔 API 按内存块边界暴露"的情况。实现正是依赖这种缺乏用户 API 约束的特性，允许把子节 (sub-section) 大小的内存范围指定给内存热插拔的上半部 :c:func:`arch_add_memory`。子节支持允许以 2MB 作为 :c:func:`devm_memremap_pages` 的跨架构通用对齐粒度。)

The users of `ZONE_DEVICE` are:
( `ZONE_DEVICE` 的用户有：)

* pmem: Map platform persistent memory to be used as a direct-I/O target
  via DAX mappings.
  (pmem: 将平台持久性内存映射为可通过 DAX 映射使用的直接 I/O 目标。)

* hmm: Extend `ZONE_DEVICE` with `->page_fault()` and `->folio_free()`
  event callbacks to allow a device-driver to coordinate memory management
  events related to device-memory, typically GPU memory. See
  Documentation/mm/hmm.rst.
  (hmm: 用 `->page_fault()` 和 `->folio_free()` 事件回调扩展 `ZONE_DEVICE` ，允许设备驱动协调与设备内存(通常是 GPU 内存)相关的内存管理事件。参见 Documentation/mm/hmm.rst。)

* p2pdma: Create `struct page` objects to allow peer devices in a
  PCI/-E topology to coordinate direct-DMA operations between themselves,
  i.e. bypass host memory.
  (p2pdma: 创建 `struct page` 对象，允许 PCI/-E 拓扑中的对等设备之间协调直接 DMA 操作，即绕过主机内存。)
