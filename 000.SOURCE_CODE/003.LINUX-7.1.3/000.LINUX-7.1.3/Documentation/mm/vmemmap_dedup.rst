
.. SPDX-License-Identifier: GPL-2.0

================================================================================
A vmemmap diet for HugeTLB and Device DAX(为 HugeTLB 和 Device DAX 精简 vmemmap)
================================================================================

HugeTLB(巨页)
=============

This section is to explain how HugeTLB Vmemmap Optimization (HVO) works.
(本节解释 HugeTLB Vmemmap 优化(HVO)的工作原理。)

The ``struct page`` structures are used to describe a physical page frame. By
default, there is a one-to-one mapping from a page frame to its corresponding
``struct page``.
( ``struct page`` 结构用于描述一个物理页帧。默认情况下，从页帧到其对应的 ``struct page`` 之间存在一一映射。)

HugeTLB pages consist of multiple base page size pages and is supported by many
architectures. See Documentation/admin-guide/mm/hugetlbpage.rst for more
details. On the x86-64 architecture, HugeTLB pages of size 2MB and 1GB are
currently supported. Since the base page size on x86 is 4KB, a 2MB HugeTLB page
consists of 512 base pages and a 1GB HugeTLB page consists of 262144 base pages.
For each base page, there is a corresponding ``struct page``.
(HugeTLB 页由多个基础页大小的页组成，并且得到许多架构的支持。详见 Documentation/admin-guide/mm/hugetlbpage.rst。在 x86-64 架构上，当前支持 2MB 和 1GB 大小的 HugeTLB 页。由于 x86 的基础页大小为 4KB，一个 2MB 的 HugeTLB 页由 512 个基础页组成，一个 1GB 的 HugeTLB 页由 262144 个基础页组成。每个基础页都有一个对应的 ``struct page`` 。)

Within the HugeTLB subsystem, only the first 4 ``struct page`` are used to
contain unique information about a HugeTLB page. ``__NR_USED_SUBPAGE`` provides
this upper limit. The only 'useful' information in the remaining ``struct page``
is the compound_info field, and this field is the same for all tail pages.
(在 HugeTLB 子系统中，只有前 4 个 ``struct page`` 用于保存 HugeTLB 页的独特信息。 ``__NR_USED_SUBPAGE`` 给出了这个上限。其余 ``struct page`` 中唯一"有用"的信息是 compound_info 字段，且该字段对所有尾页都是相同的。)

By removing redundant ``struct page`` for HugeTLB pages, memory can be returned
to the buddy allocator for other uses.
(通过移除 HugeTLB 页冗余的 ``struct page`` ，内存可以归还给伙伴分配器作其他用途。)

Different architectures support different HugeTLB pages. For example, the
following table is the HugeTLB page size supported by x86 and arm64
architectures. Because arm64 supports 4k, 16k, and 64k base pages and
supports contiguous entries, so it supports many kinds of sizes of HugeTLB
page.
(不同的架构支持不同的 HugeTLB 页。例如，下表是 x86 和 arm64 架构支持的 HugeTLB 页大小。由于 arm64 支持 4k、16k 和 64k 基础页，并且支持连续(contiguous)表项，因此它支持多种大小的 HugeTLB 页。)

+--------------+-----------+-----------------------------------------------+
| Architecture | Page Size |                HugeTLB Page Size              |
| (架构)       | (页大小)  |                (HugeTLB 页大小)               |
+--------------+-----------+-----------+-----------+-----------+-----------+
|    x86-64    |    4KB    |    2MB    |    1GB    |           |           |
+--------------+-----------+-----------+-----------+-----------+-----------+
|              |    4KB    |   64KB    |    2MB    |    32MB   |    1GB    |
|              +-----------+-----------+-----------+-----------+-----------+
|    arm64     |   16KB    |    2MB    |   32MB    |     1GB   |           |
|              +-----------+-----------+-----------+-----------+-----------+
|              |   64KB    |    2MB    |  512MB    |    16GB   |           |
+--------------+-----------+-----------+-----------+-----------+-----------+

When the system boot up, every HugeTLB page has more than one ``struct page``
structs which size is (unit: pages)::

   struct_size = HugeTLB_Size / PAGE_SIZE * sizeof(struct page) / PAGE_SIZE

(系统启动时，每个 HugeTLB 页都拥有多于一个 ``struct page`` 结构，其大小为(单位：页)：)

Where HugeTLB_Size is the size of the HugeTLB page. We know that the size
of the HugeTLB page is always n times PAGE_SIZE. So we can get the following
relationship::

   HugeTLB_Size = n * PAGE_SIZE

(其中 HugeTLB_Size 是 HugeTLB 页的大小。我们知道 HugeTLB 页的大小总是 PAGE_SIZE 的 n 倍，因此可以得到如下关系：)

Then::

   struct_size = n * PAGE_SIZE / PAGE_SIZE * sizeof(struct page) / PAGE_SIZE
               = n * sizeof(struct page) / PAGE_SIZE

(进而可得：)

We can use huge mapping at the pud/pmd level for the HugeTLB page.
(我们可以在 pud/pmd 级别对 HugeTLB 页使用大页映射。)

For the HugeTLB page of the pmd level mapping, then::

   struct_size = n * sizeof(struct page) / PAGE_SIZE
               = PAGE_SIZE / sizeof(pte_t) * sizeof(struct page) / PAGE_SIZE
               = sizeof(struct page) / sizeof(pte_t)
               = 64 / 8
               = 8 (pages)

(对于采用 pmd 级别映射的 HugeTLB 页：)

Where n is how many pte entries which one page can contains. So the value of
n is (PAGE_SIZE / sizeof(pte_t)).
(其中 n 是一页可以包含的 pte 表项数量。因此 n 的值为(PAGE_SIZE / sizeof(pte_t))。)

This optimization only supports 64-bit system, so the value of sizeof(pte_t)
is 8. And this optimization also applicable only when the size of ``struct page``
is a power of two. In most cases, the size of ``struct page`` is 64 bytes (e.g.
x86-64 and arm64). So if we use pmd level mapping for a HugeTLB page, the
size of ``struct page`` structs of it is 8 page frames which size depends on the
size of the base page.
(该优化只支持 64 位系统，因此 sizeof(pte_t) 的值为 8。此外，该优化也仅在 ``struct page`` 的大小为 2 的幂时才适用。在大多数情况下， ``struct page`` 的大小为 64 字节(例如 x86-64 和 arm64)。因此，如果对一个 HugeTLB 页使用 pmd 级别映射，其 ``struct page`` 结构的大小为 8 个页帧，具体大小取决于基础页的大小。)

For the HugeTLB page of the pud level mapping, then::

   struct_size = PAGE_SIZE / sizeof(pmd_t) * struct_size(pmd)
               = PAGE_SIZE / 8 * 8 (pages)
               = PAGE_SIZE (pages)

(对于采用 pud 级别映射的 HugeTLB 页：)

Where the struct_size(pmd) is the size of the ``struct page`` structs of a
HugeTLB page of the pmd level mapping.
(其中 struct_size(pmd) 是采用 pmd 级别映射的 HugeTLB 页的 ``struct page`` 结构的大小。)

E.g.: A 2MB HugeTLB page on x86_64 consists in 8 page frames while 1GB
HugeTLB page consists in 4096.
(例如：x86_64 上一个 2MB 的 HugeTLB 页由 8 个页帧组成，而一个 1GB 的 HugeTLB 页则由 4096 个组成。)

Next, we take the pmd level mapping of the HugeTLB page as an example to
show the internal implementation of this optimization. There are 8 pages
``struct page`` structs associated with a HugeTLB page which is pmd mapped.
(接下来，我们以 HugeTLB 页的 pmd 级别映射为例，展示该优化的内部实现。一个采用 pmd 映射的 HugeTLB 页关联着 8 页 ``struct page`` 结构。)

Here is how things look before optimization::

    HugeTLB                  struct pages(8 pages)         page frame(8 pages)
 +-----------+ ---virt_to_page---> +-----------+   mapping to   +-----------+
 |           |                     |     0     | -------------> |     0     |
 |           |                     +-----------+                +-----------+
 |           |                     |     1     | -------------> |     1     |
 |           |                     +-----------+                +-----------+
 |           |                     |     2     | -------------> |     2     |
 |           |                     +-----------+                +-----------+
 |           |                     |     3     | -------------> |     3     |
 |           |                     +-----------+                +-----------+
 |           |                     |     4     | -------------> |     4     |
 |    PMD    |                     +-----------+                +-----------+
 |   level   |                     |     5     | -------------> |     5     |
 |  mapping  |                     +-----------+                +-----------+
 |           |                     |     6     | -------------> |     6     |
 |           |                     +-----------+                +-----------+
 |           |                     |     7     | -------------> |     7     |
 |           |                     +-----------+                +-----------+
 |           |
 |           |
 |           |
 +-----------+

(优化前的情况如下所示：)

The first page of ``struct page`` (page 0) associated with the HugeTLB page
contains the 4 ``struct page`` necessary to describe the HugeTLB. The remaining
pages of ``struct page`` (page 1 to page 7) are tail pages.
(与 HugeTLB 页关联的 ``struct page`` 的第一页(page 0)包含了描述该 HugeTLB 所必需的 4 个 ``struct page``。其余的 ``struct page`` 页(page 1 至 page 7)是尾页。)

The optimization is only applied when the size of the struct page is a power
of 2. In this case, all tail pages of the same order are identical. See
compound_head(). This allows us to remap the tail pages of the vmemmap to a
shared, read-only page. The head page is also remapped to a new page. This
allows the original vmemmap pages to be freed.
(该优化只在 struct page 的大小为 2 的幂时应用。在这种情况下，同一阶(order)的所有尾页都是相同的。参见 compound_head()。这使我们可以把 vmemmap 的尾页重映射到一个共享的只读页上。头页也被重映射到一个新页上。这样，原来的 vmemmap 页就可以被释放。)

Here is how things look after remapping::

    HugeTLB                  struct pages(8 pages)                 page frame (new)
 +-----------+ ---virt_to_page---> +-----------+   mapping to   +----------------+
 |           |                     |     0     | -------------> |       0        |
 |           |                     +-----------+                +----------------+
 |           |                     |     1     | ------┐
 |           |                     +-----------+       |
 |           |                     |     2     | ------┼        +----------------------------+
 |           |                     +-----------+       |        | A single, per-zone page    |
 |           |                     |     3     | ------┼------> | frame shared among all     |
 |           |                     +-----------+       |        | hugepages of the same size |
 |           |                     |     4     | ------┼        +----------------------------+
 |           |                     +-----------+       |
 |           |                     |     5     | ------┼
 |    PMD    |                     +-----------+       |
 |   level   |                     |     6     | ------┼
 |  mapping  |                     +-----------+       |
 |           |                     |     7     | ------┘
 |           |                     +-----------+
 |           |
 |           |
 |           |
 +-----------+

When a HugeTLB is freed to the buddy system, we should allocate 7 pages for
vmemmap pages and restore the previous mapping relationship.
(当一个 HugeTLB 被释放回伙伴系统时，我们需要为 vmemmap 页分配 7 页，并恢复之前的映射关系。)

For the HugeTLB page of the pud level mapping. It is similar to the former.
We also can use this approach to free (PAGE_SIZE - 1) vmemmap pages.
(对于采用 pud 级别映射的 HugeTLB 页，情况与前面类似。我们同样可以用这种方法释放(PAGE_SIZE - 1)个 vmemmap 页。)

Apart from the HugeTLB page of the pmd/pud level mapping, some architectures
(e.g. aarch64) provides a contiguous bit in the translation table entries
that hints to the MMU to indicate that it is one of a contiguous set of
entries that can be cached in a single TLB entry.
(除了采用 pmd/pud 级别映射的 HugeTLB 页之外，一些架构(例如 aarch64)在转换表表项中提供了一个连续(contiguous)位，向 MMU 提示它是可以缓存到单个 TLB 表项中的连续表项集合之一。)

The contiguous bit is used to increase the mapping size at the pmd and pte
(last) level. So this type of HugeTLB page can be optimized only when its
size of the ``struct page`` structs is greater than **1** page.
(连续位用于增大 pmd 和 pte(最末)级别的映射大小。因此，只有当这类 HugeTLB 页的 ``struct page`` 结构的大小大于 **1** 页时，才能对其进行优化。)

Device DAX(设备 DAX)
====================

The device-dax interface uses the same tail deduplication technique explained
in the previous chapter, except when used with the vmemmap in
the device (altmap).
(device-dax 接口使用与上一章所述相同的尾页去重技术，但当其与设备(altmap)中的 vmemmap 一起使用时除外。)

The following page sizes are supported in DAX: PAGE_SIZE (4K on x86_64),
PMD_SIZE (2M on x86_64) and PUD_SIZE (1G on x86_64).
For powerpc equivalent details see Documentation/arch/powerpc/vmemmap_dedup.rst
(DAX 支持以下页大小：PAGE_SIZE(x86_64 上为 4K)、PMD_SIZE(x86_64 上为 2M)和 PUD_SIZE(x86_64 上为 1G)。powerpc 上的对应细节参见 Documentation/arch/powerpc/vmemmap_dedup.rst。)

The differences with HugeTLB are relatively minor.
(与 HugeTLB 的差异相对较小。)

It only use 3 ``struct page`` for storing all information as opposed
to 4 on HugeTLB pages.
(它只使用 3 个 ``struct page`` 来保存全部信息，而 HugeTLB 页需要 4 个。)

There's no remapping of vmemmap given that device-dax memory is not part of
System RAM ranges initialized at boot. Thus the tail page deduplication
happens at a later stage when we populate the sections. HugeTLB reuses the
the head vmemmap page representing, whereas device-dax reuses the tail
vmemmap page. This results in only half of the savings compared to HugeTLB.
(由于 device-dax 内存不属于启动时初始化的 System RAM 范围，因此不存在 vmemmap 重映射。于是尾页去重发生在稍后的阶段，即我们填充 section 的时候。HugeTLB 复用代表头页的 vmemmap 页，而 device-dax 复用尾 vmemmap 页。这使得相比 HugeTLB 只有一半的节省。)

Deduplicated tail pages are not mapped read-only.
(去重后的尾页不映射为只读。)

Here's how things look like on device-dax after the sections are populated::

 +-----------+ ---virt_to_page---> +-----------+   mapping to   +-----------+
 |           |                     |     0     | -------------> |     0     |
 |           |                     +-----------+                +-----------+
 |           |                     |     1     | -------------> |     1     |
 |           |                     +-----------+                +-----------+
 |           |                     |     2     | ----------------^ ^ ^ ^ ^ ^
 |           |                     +-----------+                   | | | | |
 |           |                     |     3     | ------------------+ | | | |
 |           |                     +-----------+                     | | | |
 |           |                     |     4     | --------------------+ | | |
 |    PMD    |                     +-----------+                       | | |
 |   level   |                     |     5     | ----------------------+ | |
 |  mapping  |                     +-----------+                         | |
 |           |                     |     6     | ------------------------+ |
 |           |                     +-----------+                           |
 |           |                     |     7     | --------------------------+
 |           |                     +-----------+
 |           |
 |           |
 |           |
 +-----------+

(以下是 device-dax 在 section 填充完成后的样子：)
