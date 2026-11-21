.. SPDX-License-Identifier: GPL-2.0+


==================
Maple Tree(枫树)
==================

:Author: Liam R. Howlett

(作者: Liam R. Howlett)

Overview(概述)
================

The Maple Tree is a B-Tree data type which is optimized for storing
non-overlapping ranges, including ranges of size 1.  The tree was designed to
be simple to use and does not require a user written search method.  It
supports iterating over a range of entries and going to the previous or next
entry in a cache-efficient manner.  The tree can also be put into an RCU-safe
mode of operation which allows reading and writing concurrently.  Writers must
synchronize on a lock, which can be the default spinlock, or the user can set
the lock to an external lock of a different type.
(枫树(Maple Tree)是一种针对存储互不重叠的范围而优化的 B-Tree 数据类型，包括大小为 1 的范围。该树的设计注重易用性，不需要用户自行编写查找方法。它支持以缓存友好的方式遍历一个范围内的条目，以及前进到上一个或下一个条目。该树还可以进入 RCU 安全的操作模式，允许并发读写。写操作必须在锁上同步，该锁可以是默认的自旋锁，用户也可以将其设置为不同类型的外部锁。)

The Maple Tree maintains a small memory footprint and was designed to use
modern processor cache efficiently.  The majority of the users will be able to
use the normal API.  An :ref:`maple-tree-advanced-api` exists for more complex
scenarios.  The most important usage of the Maple Tree is the tracking of the
virtual memory areas.
(枫树尽量保持较小的内存占用，并在设计上高效利用现代处理器的缓存。大多数用户可以使用普通 API。对于更复杂的场景，还存在 :ref:`maple-tree-advanced-api` 。枫树最重要的用途是虚拟内存区域(VMA)的跟踪。)

The Maple Tree can store values between ``0`` and ``ULONG_MAX``.  The Maple
Tree reserves values with the bottom two bits set to '10' which are below 4096
(ie 2, 6, 10 .. 4094) for internal use.  If the entries may use reserved
entries then the users can convert the entries using xa_mk_value() and convert
them back by calling xa_to_value().  If the user needs to use a reserved
value, then the user can convert the value when using the
:ref:`maple-tree-advanced-api`, but are blocked by the normal API.
(枫树可以存储 ``0`` 到 ``ULONG_MAX`` 之间的值。枫树把低两位为 '10' 且小于 4096 的值(即 2、6、10 .. 4094)保留为内部使用。如果条目可能使用这些保留值，用户可以用 xa_mk_value() 对条目进行转换，并调用 xa_to_value() 转换回来。如果用户需要使用保留值，那么在使用 :ref:`maple-tree-advanced-api` 时可以对该值进行转换，但普通 API 会拒绝这样做。)

The Maple Tree can also be configured to support searching for a gap of a given
size (or larger).
(枫树还可以配置为支持搜索给定大小(或更大)的空隙(gap)。)

Pre-allocating of nodes is also supported using the
:ref:`maple-tree-advanced-api`.  This is useful for users who must guarantee a
successful store operation within a given
code segment when allocating cannot be done.  Allocations of nodes are
relatively small at around 256 bytes.
(也支持使用 :ref:`maple-tree-advanced-api` 预分配节点。这对于必须在给定代码段内保证存储操作成功、而在该代码段内无法进行分配的用户非常有用。节点的分配开销相对较小，约 256 字节。)

.. _maple-tree-normal-api:

Normal API(普通 API)
====================

Start by initialising a maple tree, either with DEFINE_MTREE() for statically
allocated maple trees or mt_init() for dynamically allocated ones.  A
freshly-initialised maple tree contains a ``NULL`` pointer for the range ``0``
- ``ULONG_MAX``.  There are currently two types of maple trees supported: the
allocation tree and the regular tree.  The regular tree has a higher branching
factor for internal nodes.  The allocation tree has a lower branching factor
but allows the user to search for a gap of a given size or larger from either
``0`` upwards or ``ULONG_MAX`` down.  An allocation tree can be used by
passing in the ``MT_FLAGS_ALLOC_RANGE`` flag when initialising the tree.
(首先初始化一棵枫树：静态分配的枫树用 DEFINE_MTREE()，动态分配的用 mt_init()。刚初始化的枫树在范围 ``0`` - ``ULONG_MAX`` 上包含一个 ``NULL`` 指针。目前支持两种类型的枫树：分配树(allocation tree)和普通树(regular tree)。普通树的内部节点具有更高的分支因子。分配树的分支因子较低，但允许用户从 ``0`` 向上或从 ``ULONG_MAX`` 向下搜索给定大小或更大的空隙。在初始化树时传入 ``MT_FLAGS_ALLOC_RANGE`` 标志即可使用分配树。)

You can then set entries using mtree_store() or mtree_store_range().
mtree_store() will overwrite any entry with the new entry and return 0 on
success or an error code otherwise.  mtree_store_range() works in the same way
but takes a range.  mtree_load() is used to retrieve the entry stored at a
given index.  You can use mtree_erase() to erase an entire range by only
knowing one value within that range, or mtree_store() call with an entry of
NULL may be used to partially erase a range or many ranges at once.
(之后可以使用 mtree_store() 或 mtree_store_range() 设置条目。mtree_store() 会用新条目覆盖任何已有条目，成功时返回 0，否则返回错误码。mtree_store_range() 的工作方式相同，但接受一个范围。mtree_load() 用于取出存储在给定索引处的条目。可以使用 mtree_erase() 仅凭范围内的一个值就擦除整个范围；或者调用 mtree_store() 时传入 NULL 条目，以一次部分擦除一个或多个范围。)

If you want to only store a new entry to a range (or index) if that range is
currently ``NULL``, you can use mtree_insert_range() or mtree_insert() which
return -EEXIST if the range is not empty.
(如果只想在某个范围(或索引)当前为 ``NULL`` 时才向其中存储新条目，可以使用 mtree_insert_range() 或 mtree_insert()，当范围非空时它们会返回 -EEXIST。)

You can search for an entry from an index upwards by using mt_find().
(可以使用 mt_find() 从某个索引开始向上搜索条目。)

You can walk each entry within a range by calling mt_for_each().  You must
provide a temporary variable to store a cursor.  If you want to walk each
element of the tree then ``0`` and ``ULONG_MAX`` may be used as the range.  If
the caller is going to hold the lock for the duration of the walk then it is
worth looking at the mas_for_each() API in the :ref:`maple-tree-advanced-api`
section.
(可以通过调用 mt_for_each() 遍历某个范围内的每个条目。必须提供一个临时变量来保存游标(cursor)。如果要遍历整棵树的每个元素，可以使用 ``0`` 和 ``ULONG_MAX`` 作为范围。如果调用方将在遍历期间一直持有锁，那么值得看看 :ref:`maple-tree-advanced-api` 一节中的 mas_for_each() API。)

Sometimes it is necessary to ensure the next call to store to a maple tree does
not allocate memory, please see :ref:`maple-tree-advanced-api` for this use case.
(有时需要确保对枫树的下一次存储调用不会分配内存，这种用例请参见 :ref:`maple-tree-advanced-api` 。)

You can use mtree_dup() to duplicate an entire maple tree. It is a more
efficient way than inserting all elements one by one into a new tree.
(可以使用 mtree_dup() 复制一整棵枫树。这比把所有元素逐个插入新树更高效。)

Finally, you can remove all entries from a maple tree by calling
mtree_destroy().  If the maple tree entries are pointers, you may wish to free
the entries first.
(最后，可以通过调用 mtree_destroy() 移除枫树中的所有条目。如果枫树的条目是指针，你可能希望先释放这些条目。)

Allocating Nodes(节点分配)
--------------------------

The allocations are handled by the internal tree code.  See
:ref:`maple-tree-advanced-alloc` for other options.
(分配由树的内部代码处理。其他选项参见 :ref:`maple-tree-advanced-alloc` 。)

Locking(锁定)
-------------

You do not have to worry about locking.  See :ref:`maple-tree-advanced-locks`
for other options.
(你无需担心锁定问题。其他选项参见 :ref:`maple-tree-advanced-locks` 。)

The Maple Tree uses RCU and an internal spinlock to synchronise access:
(枫树使用 RCU 和一个内部自旋锁来同步访问：)

Takes RCU read lock: (获取 RCU 读锁：)
 * mtree_load()
 * mt_find()
 * mt_for_each()
 * mt_next()
 * mt_prev()

Takes ma_lock internally: (在内部获取 ma_lock：)
 * mtree_store()
 * mtree_store_range()
 * mtree_insert()
 * mtree_insert_range()
 * mtree_erase()
 * mtree_dup()
 * mtree_destroy()
 * mt_set_in_rcu()
 * mt_clear_in_rcu()

If you want to take advantage of the internal lock to protect the data
structures that you are storing in the Maple Tree, you can call mtree_lock()
before calling mtree_load(), then take a reference count on the object you
have found before calling mtree_unlock().  This will prevent stores from
removing the object from the tree between looking up the object and
incrementing the refcount.  You can also use RCU to avoid dereferencing
freed memory, but an explanation of that is beyond the scope of this
document.
(如果你想利用内部锁来保护存入 Maple Tree 的数据结构，可以在调用 mtree_load() 之前先调用
mtree_lock()，并在调用 mtree_unlock() 之前对找到的对象获取一个引用计数。这样可以防止存储操
作在查找对象与递增引用计数之间把对象从树中移除。你也可以使用 RCU 来避免解引用已释放的内存，
但对此的说明超出了本文档的范围。)

.. _maple-tree-advanced-api:

Advanced API(高级 API)
======================

The advanced API offers more flexibility and better performance at the
cost of an interface which can be harder to use and has fewer safeguards.
You must take care of your own locking while using the advanced API.
You can use the ma_lock, RCU or an external lock for protection.
You can mix advanced and normal operations on the same array, as long
as the locking is compatible.  The :ref:`maple-tree-normal-api` is implemented
in terms of the advanced API.
(高级 API 以更难使用、安全保护更少的接口为代价，提供了更大的灵活性和更好的性能。使用高级 API 时，你必须自己负责锁定。可以使用 ma_lock、RCU 或外部锁进行保护。只要锁是兼容的，你可以在同一个数组上混合使用高级操作和普通操作。 :ref:`maple-tree-normal-api` 就是基于高级 API 实现的。)

The advanced API is based around the ma_state, this is where the 'mas'
prefix originates.  The ma_state struct keeps track of tree operations to make
life easier for both internal and external tree users.
(高级 API 围绕 ma_state 构建，'mas' 前缀即由此而来。ma_state 结构体负责跟踪树操作，让内部和外部树用户都更轻松。)

Initialising the maple tree is the same as in the :ref:`maple-tree-normal-api`.
Please see above.
(枫树的初始化与 :ref:`maple-tree-normal-api` 中相同。请参见上文。)

The maple state keeps track of the range start and end in mas->index and
mas->last, respectively.
( maple 状态分别在 mas->index 和 mas->last 中跟踪范围的起点和终点。)

mas_walk() will walk the tree to the location of mas->index and set the
mas->index and mas->last according to the range for the entry.
(mas_walk() 会沿着树走到 mas->index 所在的位置，并根据该条目所属的范围设置 mas->index 和 mas->last。)

You can set entries using mas_store().  mas_store() will overwrite any entry
with the new entry and return the first existing entry that is overwritten.
The range is passed in as members of the maple state: index and last.
(可以使用 mas_store() 设置条目。mas_store() 会用新条目覆盖任何已有条目，并返回被覆盖的第一个已有条目。范围以 maple 状态的成员 index 和 last 传入。)

You can use mas_erase() to erase an entire range by setting index and
last of the maple state to the desired range to erase.  This will erase
the first range that is found in that range, set the maple state index
and last as the range that was erased and return the entry that existed
at that location.
(可以使用 mas_erase() 擦除整个范围：把 maple 状态的 index 和 last 设置为想要擦除的范围即可。这会擦除在该范围内找到的第一个范围，把 maple 状态的 index 和 last 设置为被擦除的范围，并返回该位置上原有的条目。)

You can walk each entry within a range by using mas_for_each().  If you want
to walk each element of the tree then ``0`` and ``ULONG_MAX`` may be used as
the range.  If the lock needs to be periodically dropped, see the locking
section mas_pause().
(可以使用 mas_for_each() 遍历某个范围内的每个条目。如果要遍历整棵树的每个元素，可以使用 ``0`` 和 ``ULONG_MAX`` 作为范围。如果需要周期性地释放锁，参见锁定一节的 mas_pause()。)

Using a maple state allows mas_next() and mas_prev() to function as if the
tree was a linked list.  With such a high branching factor the amortized
performance penalty is outweighed by cache optimization.  mas_next() will
return the next entry which occurs after the entry at index.  mas_prev()
will return the previous entry which occurs before the entry at index.
(使用 maple 状态可以让 mas_next() 和 mas_prev() 像操作链表一样工作。在如此高的分支因子下，摊销性能损失被缓存优化所抵消。mas_next() 返回 index 处条目之后出现的下一个条目。mas_prev() 返回 index 处条目之前出现的上一个条目。)

mas_find() will find the first entry which exists at or above index on
the first call, and the next entry from every subsequent calls.
(mas_find() 在第一次调用时查找 index 处或之上存在的第一个条目，之后的每次调用都返回下一个条目。)

mas_find_rev() will find the first entry which exists at or below the last on
the first call, and the previous entry from every subsequent calls.
(mas_find_rev() 在第一次调用时查找 last 处或之下存在的第一个条目，之后的每次调用都返回上一个条目。)

If the user needs to yield the lock during an operation, then the maple state
must be paused using mas_pause().
(如果用户需要在操作期间让出锁，必须使用 mas_pause() 暂停 maple 状态。)

There are a few extra interfaces provided when using an allocation tree.
If you wish to search for a gap within a range, then mas_empty_area()
or mas_empty_area_rev() can be used.  mas_empty_area() searches for a gap
starting at the lowest index given up to the maximum of the range.
mas_empty_area_rev() searches for a gap starting at the highest index given
and continues downward to the lower bound of the range.
(使用分配树时还提供了几个额外的接口。如果要搜索一个范围内的空隙，可以使用 mas_empty_area() 或 mas_empty_area_rev()。mas_empty_area() 从给定的最低索引开始向上搜索，直到范围的最大值。mas_empty_area_rev() 从给定的最高索引开始向下搜索，直到范围的下界。)

.. _maple-tree-advanced-alloc:

Advanced Allocating Nodes(高级节点分配)
---------------------------------------

Allocations are usually handled internally to the tree, however if allocations
need to occur before a write occurs then calling mas_expected_entries() will
allocate the worst-case number of needed nodes to insert the provided number of
ranges.  This also causes the tree to enter mass insertion mode.  Once
insertions are complete calling mas_destroy() on the maple state will free the
unused allocations.
(分配通常由树在内部处理，但如果需要在写操作发生之前进行分配，可以调用 mas_expected_entries()，它会按最坏情况预分配插入指定数量范围所需的节点数。这也会使树进入批量插入(mass insertion)模式。插入完成后，对 maple 状态调用 mas_destroy() 将释放未使用的分配。)

.. _maple-tree-advanced-locks:

Advanced Locking(高级锁定)
--------------------------

The maple tree uses a spinlock by default, but external locks can be used for
tree updates as well.  To use an external lock, the tree must be initialized
with the ``MT_FLAGS_LOCK_EXTERN flag``, this is usually done with the
MTREE_INIT_EXT() #define, which takes an external lock as an argument.
(枫树默认使用自旋锁，但树更新也可以使用外部锁。要使用外部锁，必须以 ``MT_FLAGS_LOCK_EXTERN flag`` 初始化树，这通常通过 MTREE_INIT_EXT() #define 完成，它把外部锁作为参数。)

Functions and structures(函数与结构体)
======================================

.. kernel-doc:: include/linux/maple_tree.h
.. kernel-doc:: lib/maple_tree.c
