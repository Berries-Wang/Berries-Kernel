.. SPDX-License-Identifier: GPL-2.0+

======================
ID Allocation(ID 分配)
======================

:Author: Matthew Wilcox
(作者：Matthew Wilcox)

Overview(概述)
==============

A common problem to solve is allocating identifiers (IDs); generally
small numbers which identify a thing.  Examples include file descriptors,
process IDs, packet identifiers in networking protocols, SCSI tags
and device instance numbers.  The IDR and the IDA provide a reasonable
solution to the problem to avoid everybody inventing their own.  The IDR
provides the ability to map an ID to a pointer, while the IDA provides
only ID allocation, and as a result is much more memory-efficient.
(一个常见的待解决问题是分配标识符 (ID)，即标识某个事物的小数字。例子
包括文件描述符、进程 ID、网络协议中的包标识符、SCSI tag 和设备实例号。
IDR 与 IDA 为该问题提供了一个合理的解决方案，以免每个人都自己发明一
套。IDR 提供把 ID 映射到指针的能力，而 IDA 只提供 ID 分配，因而内存效
率高得多。)

The IDR interface is deprecated; please use the :doc:`XArray <xarray>`
instead.
(IDR 接口已被弃用；请改用 :doc:`XArray <xarray>` 。)

IDR usage(IDR 用法)
===================

Start by initialising an IDR, either with DEFINE_IDR()
for statically allocated IDRs or idr_init() for dynamically
allocated IDRs.
(首先初始化一个 IDR：静态分配的 IDR 用 DEFINE_IDR()，动态分配的 IDR
用 idr_init()。)

You can call idr_alloc() to allocate an unused ID.  Look up
the pointer you associated with the ID by calling idr_find()
and free the ID by calling idr_remove().
(你可以调用 idr_alloc() 来分配一个未使用的 ID。通过调用 idr_find()
查找与该 ID 关联的指针，并通过调用 idr_remove() 释放该 ID。)

If you need to change the pointer associated with an ID, you can call
idr_replace().  One common reason to do this is to reserve an
ID by passing a ``NULL`` pointer to the allocation function; initialise the
object with the reserved ID and finally insert the initialised object
into the IDR.
(如果你需要更改与某个 ID 关联的指针，可以调用 idr_replace()。这样做
的一个常见原因是：通过向分配函数传递 ``NULL`` 指针来保留一个 ID；用
保留的 ID 初始化对象，最后把初始化好的对象插入 IDR。)

Some users need to allocate IDs larger than ``INT_MAX``.  So far all of
these users have been content with a ``UINT_MAX`` limit, and they use
idr_alloc_u32().  If you need IDs that will not fit in a u32,
we will work with you to address your needs.
(有些使用者需要分配大于 ``INT_MAX`` 的 ID。到目前为止，所有这些使用
者都满足于 ``UINT_MAX`` 的上限，并且使用 idr_alloc_u32()。如果你需要
无法放进 u32 的 ID，我们会与你一起解决你的需求。)

If you need to allocate IDs sequentially, you can use
idr_alloc_cyclic().  The IDR becomes less efficient when dealing
with larger IDs, so using this function comes at a slight cost.
(如果你需要按顺序分配 ID，可以使用 idr_alloc_cyclic()。IDR 在处理较
大的 ID 时效率会降低，因此使用该函数会付出轻微的代价。)

To perform an action on all pointers used by the IDR, you can
either use the callback-based idr_for_each() or the
iterator-style idr_for_each_entry().  You may need to use
idr_for_each_entry_continue() to continue an iteration.  You can
also use idr_get_next() if the iterator doesn't fit your needs.
(要对 IDR 使用的所有指针执行某个操作，可以使用基于回调的
idr_for_each() 或迭代器风格的 idr_for_each_entry()。你可能需要使用
idr_for_each_entry_continue() 来继续一次迭代。如果迭代器不能满足你的
需求，还可以使用 idr_get_next()。)

When you have finished using an IDR, you can call idr_destroy()
to release the memory used by the IDR.  This will not free the objects
pointed to from the IDR; if you want to do that, use one of the iterators
to do it.
(当你用完一个 IDR 之后，可以调用 idr_destroy() 来释放该 IDR 使用的内
存。这不会释放从 IDR 指向的对象；如果你想那样做，请使用某个迭代器来
完成。)

You can use idr_is_empty() to find out whether there are any
IDs currently allocated.
(你可以使用 idr_is_empty() 来查明当前是否还有已分配的 ID。)

If you need to take a lock while allocating a new ID from the IDR,
you may need to pass a restrictive set of GFP flags, which can lead
to the IDR being unable to allocate memory.  To work around this,
you can call idr_preload() before taking the lock, and then
idr_preload_end() after the allocation.
(如果你需要在从 IDR 分配新 ID 时持有锁，可能需要传递一组受限的 GFP
标志，这可能导致 IDR 无法分配内存。要规避这一点，你可以在取锁之前调
用 idr_preload()，并在分配之后调用 idr_preload_end()。)

.. kernel-doc:: include/linux/idr.h
   :doc: idr sync

IDA usage(IDA 用法)
===================

.. kernel-doc:: lib/idr.c
   :doc: IDA description

Functions and structures(函数与结构)
====================================

.. kernel-doc:: include/linux/idr.h
   :functions:
.. kernel-doc:: lib/idr.c
   :functions:
