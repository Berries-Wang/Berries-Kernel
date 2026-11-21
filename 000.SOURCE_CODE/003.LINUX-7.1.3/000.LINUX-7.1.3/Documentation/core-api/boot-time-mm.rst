===========================================
Boot time memory management(启动时内存管理)
===========================================

Early system initialization cannot use "normal" memory management
simply because it is not set up yet. But there is still need to
allocate memory for various data structures, for instance for the
physical page allocator.
(早期系统初始化无法使用"常规"内存管理，因为它尚未建立起来。但仍然需要为
各种数据结构分配内存，例如物理页分配器。)

A specialized allocator called ``memblock`` performs the
boot time memory management. The architecture specific initialization
must set it up in :c:func:`setup_arch` and tear it down in
:c:func:`mem_init` functions.
(一个名为 ``memblock`` 的专用分配器执行启动时内存管理。体系结构相关的初始
化必须在 :c:func:`setup_arch` 中建立它，并在 :c:func:`mem_init` 函数中将其
拆除。)

Once the early memory management is available it offers a variety of
functions and macros for memory allocations. The allocation request
may be directed to the first (and probably the only) node or to a
particular node in a NUMA system. There are API variants that panic
when an allocation fails and those that don't.
(一旦早期内存管理可用，它会提供各种用于内存分配的函数和宏。分配请求可以
定向到第一个（可能也是唯一的）节点，或 NUMA 系统中的特定节点。有的 API
变体在分配失败时会触发 panic，有的则不会。)

Memblock also offers a variety of APIs that control its own behaviour.
(Memblock 还提供各种控制其自身行为的 API。)

Memblock Overview(Memblock 概述)
================================

.. kernel-doc:: mm/memblock.c
   :doc: memblock overview


Functions and structures(函数与结构)
====================================

Here is the description of memblock data structures, functions and
macros. Some of them are actually internal, but since they are
documented it would be silly to omit them. Besides, reading the
descriptions for the internal functions can help to understand what
really happens under the hood.
(以下是 memblock 数据结构、函数和宏的描述。其中一些实际上是内部的，但既然
已经为它们编写了文档，将其省略就太可惜了。此外，阅读内部函数的描述有助于
理解幕后实际发生的事情。)

.. kernel-doc:: include/linux/memblock.h
.. kernel-doc:: mm/memblock.c
   :functions:
