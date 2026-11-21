.. SPDX-License-Identifier: GPL-2.0

====================================
Frequently Asked Questions(常见问题)
====================================

Does DAMON support virtual memory only?(DAMON 只支持虚拟内存吗？)
=================================================================

No.  The core of the DAMON is address space independent.  The address space
specific monitoring operations including monitoring target regions
constructions and actual access checks can be implemented and configured on the
DAMON core by the users.  In this way, DAMON users can monitor any address
space with any access check technique.
(不支持。DAMON 的核心与地址空间无关。包括监控目标区域构建和实际访问检查在内的地址空间特定监控操作，可由用户在 DAMON 核心上实现并配置。这样，DAMON 用户便可以用任意访问检查技术监控任意地址空间。)

Nonetheless, DAMON provides vma/rmap tracking and PTE Accessed bit check based
implementations of the address space dependent functions for the virtual memory
and the physical memory by default, for a reference and convenient use.
(尽管如此，DAMON 默认针对虚拟内存和物理内存提供了基于 vma/rmap 跟踪与 PTE Accessed 位检查的地址空间相关功能实现，以供参考并方便使用。)


Can I simply monitor page granularity?(可以简单地按页粒度进行监控吗？)
======================================================================

Yes.  You can do so by setting the ``min_nr_regions`` attribute higher than the
working set size divided by the page size.  Because the monitoring target
regions size is forced to be ``>=page size``, the region split will make no
effect.
(可以。将 ``min_nr_regions`` 属性设置为大于工作集大小除以页大小的值即可。由于监控目标区域的大小被强制为 ``>=page size``，区域拆分将不会产生任何效果。)
