=====================================
Core API Documentation(核心 API 文档)
=====================================

This is the beginning of a manual for core kernel APIs.  The conversion
(and writing!) of documents for this manual is much appreciated!
(这是核心内核 API 手册的开端。非常欢迎大家为这本手册转换（以及编写！）
文档！)

Core utilities(核心工具)
========================

This section has general and "core core" documentation.  The first is a
massive grab-bag of kerneldoc info left over from the docbook days; it
should really be broken up someday when somebody finds the energy to do
it.
(本节包含通用文档和 "core core" 文档。前者是从 docbook 时代遗留下来的
海量 kerneldoc 信息大杂烩；等哪天有人有精力了，真的应该把它拆分开来。)

.. toctree::
   :maxdepth: 1

   kernel-api
   workqueue
   watch_queue
   printk-basics
   printk-formats
   printk-index
   symbol-namespaces
   asm-annotations
   real-time/index
   housekeeping.rst

Data structures and low-level utilities(数据结构与低层工具)
===========================================================

Library functionality that is used throughout the kernel.
(整个内核都在使用的库功能。)

.. toctree::
   :maxdepth: 1

   kobject
   kref
   cleanup
   assoc_array
   folio_queue
   xarray
   maple_tree
   idr
   circular-buffers
   rbtree
   generic-radix-tree
   packing
   this_cpu_ops
   timekeeping
   errseq
   wrappers/atomic_t
   wrappers/atomic_bitops
   floating-point
   union_find
   min_heap
   parser
   list

Low level entry and exit(低层进入与退出)
========================================

.. toctree::
   :maxdepth: 1

   entry

Concurrency primitives(并发原语)
================================

How Linux keeps everything from happening at the same time.  See
Documentation/locking/index.rst for more related documentation.
(Linux 如何防止所有事情同时发生。更多相关文档参见
Documentation/locking/index.rst。)

.. toctree::
   :maxdepth: 1

   refcount-vs-atomic
   irq/index
   local_ops
   padata
   ../RCU/index
   wrappers/memory-barriers.rst

Low-level hardware management(低层硬件管理)
===========================================

Cache management, managing CPU hotplug, etc.
(缓存管理、CPU 热插拔管理等。)

.. toctree::
   :maxdepth: 1

   cachetlb
   cpu_hotplug
   memory-hotplug
   genericirq
   protection-keys

Memory management(内存管理)
===========================

How to allocate and use memory in the kernel.  Note that there is a lot
more memory-management documentation in Documentation/mm/index.rst.
(如何在内核中分配和使用内存。注意，Documentation/mm/index.rst 中还有
更多的内存管理文档。)

.. toctree::
   :maxdepth: 1

   memory-allocation
   unaligned-memory-access
   dma-api
   dma-api-howto
   dma-attributes
   dma-isa-lpc
   swiotlb
   mm-api
   cgroup
   genalloc
   pin_user_pages
   boot-time-mm
   gfp_mask-from-fs-io
   kho/index

Interfaces for kernel debugging(内核调试接口)
=============================================

.. toctree::
   :maxdepth: 1

   debug-objects
   tracepoint
   debugging-via-ohci1394

Everything else(其他一切)
=========================

Documents that don't fit elsewhere or which have yet to be categorized.
(不适合放在其他地方、或尚未归类的文档。)

.. toctree::
   :maxdepth: 1

   librs
   liveupdate
   netlink
