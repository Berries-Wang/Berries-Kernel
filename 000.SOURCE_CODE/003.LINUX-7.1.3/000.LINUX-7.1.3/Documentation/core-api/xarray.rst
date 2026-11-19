.. SPDX-License-Identifier: GPL-2.0+

===================
XArray(XArray 数组)
===================

:Author: Matthew Wilcox

(作者: Matthew Wilcox)

Overview(概述)
==============

The XArray is an abstract data type which behaves like a very large array
of pointers.  It meets many of the same needs as a hash or a conventional
resizable array.  Unlike a hash, it allows you to sensibly go to the
next or previous entry in a cache-efficient manner.  In contrast to a
resizable array, there is no need to copy data or change MMU mappings in
order to grow the array.  It is more memory-efficient, parallelisable
and cache friendly than a doubly-linked list.  It takes advantage of
RCU to perform lookups without locking.
( XArray 是一种抽象数据类型，其行为就像一个非常大的指针数组。它满足哈希表或
传统可变长数组的许多相同需求。与哈希不同，它允许你以缓存高效的方式合理地前往
下一个或上一个条目。与传统可变长数组相比，增长数组无需复制数据或更改 MMU
映射。它比双向链表更节省内存、更可并行化、对缓存也更友好。它利用 RCU 在无锁
的情况下执行查找。)

The XArray implementation is efficient when the indices used are densely
clustered; hashing the object and using the hash as the index will not
perform well.  The XArray is optimised for small indices, but still has
good performance with large indices.  If your index can be larger than
``ULONG_MAX`` then the XArray is not the data type for you.  The most
important user of the XArray is the page cache.
(当所使用的索引密集聚集时，XArray 的实现是高效的；把对象散列并用散列值作为
索引不会有好的性能。XArray 针对较小的索引做了优化，但对较大的索引仍有良好的
性能。如果你的索引可能超过 ``ULONG_MAX`` ，那么 XArray 就不是适合你的数据
类型。XArray 最重要的使用者是页缓存。)

Normal pointers may be stored in the XArray directly.  They must be 4-byte
aligned, which is true for any pointer returned from kmalloc() and
alloc_page().  It isn't true for arbitrary user-space pointers,
nor for function pointers.  You can store pointers to statically allocated
objects, as long as those objects have an alignment of at least 4.
(普通指针可以直接存入 XArray。它们必须按 4 字节对齐，从 kmalloc() 和
alloc_page() 返回的任何指针都满足这一点。任意的用户空间指针以及函数指针则
不满足。你可以存储指向静态分配对象的指针，只要这些对象的对齐至少为 4。)

You can also store integers between 0 and ``LONG_MAX`` in the XArray.
You must first convert it into an entry using xa_mk_value().
When you retrieve an entry from the XArray, you can check whether it is
a value entry by calling xa_is_value(), and convert it back to
an integer by calling xa_to_value().
(你也可以在 XArray 中存储 0 到 ``LONG_MAX`` 之间的整数。必须先用 xa_mk_value()
把它转换为一个条目。当你从 XArray 取回一个条目时，可以调用 xa_is_value() 检查
它是否为值条目，并调用 xa_to_value() 把它转换回整数。)

Some users want to tag the pointers they store in the XArray.  You can
call xa_tag_pointer() to create an entry with a tag, xa_untag_pointer()
to turn a tagged entry back into an untagged pointer and xa_pointer_tag()
to retrieve the tag of an entry.  Tagged pointers use the same bits that
are used to distinguish value entries from normal pointers, so you must
decide whether you want to store value entries or tagged pointers in any
particular XArray.
(有些使用者希望给自己存入 XArray 的指针打标记。可以调用 xa_tag_pointer() 创建
一个带标记的条目，调用 xa_untag_pointer() 把带标记的条目转回未打标记的指针，
调用 xa_pointer_tag() 获取条目的标记。带标记的指针使用的位与区分值条目和普通
指针所用的位相同，因此在任何一个具体的 XArray 中，你必须决定要存储值条目还是
带标记的指针。)

The XArray does not support storing IS_ERR() pointers as some
conflict with value entries or internal entries.
( XArray 不支持存储 IS_ERR() 指针，因为其中一些会与值条目或内部条目冲突。)

An unusual feature of the XArray is the ability to create entries which
occupy a range of indices.  Once stored to, looking up any index in
the range will return the same entry as looking up any other index in
the range.  Storing to any index will store to all of them.  Multi-index
entries can be explicitly split into smaller entries. Unsetting (using
xa_erase() or xa_store() with ``NULL``) any entry will cause the XArray
to forget about the range.
( XArray 的一个不寻常特性是能够创建占据一段索引范围的条目。一旦存储完成，
查找该范围内的任何索引都会返回与查找其他索引相同的条目。向任何一个索引存储
都会存储到整个范围。多索引条目可以被显式地拆分为更小的条目。取消 (使用
xa_erase() 或以 ``NULL`` 调用 xa_store() ) 任何条目都会使 XArray 忘记这个
范围。)

Normal API(普通 API)
====================

Start by initialising an XArray, either with DEFINE_XARRAY()
for statically allocated XArrays or xa_init() for dynamically
allocated ones.  A freshly-initialised XArray contains a ``NULL``
pointer at every index.
(首先要初始化一个 XArray：静态分配的 XArray 用 DEFINE_XARRAY()，动态分配的
用 xa_init()。新初始化的 XArray 在每个索引处都包含一个 ``NULL`` 指针。)

You can then set entries using xa_store() and get entries using
xa_load().  xa_store() will overwrite any entry with the new entry and
return the previous entry stored at that index.  You can unset entries
using xa_erase() or by setting the entry to ``NULL`` using xa_store().
There is no difference between an entry that has never been stored to
and one that has been erased with xa_erase(); an entry that has most
recently had ``NULL`` stored to it is also equivalent except if the
XArray was initialized with ``XA_FLAGS_ALLOC``.
(然后可以用 xa_store() 设置条目、用 xa_load() 获取条目。xa_store() 会用新
条目覆盖任何条目，并返回之前存储在该索引处的条目。可以用 xa_erase() 或通过
xa_store() 把条目设为 ``NULL`` 来取消条目。从未被存储过的条目与被 xa_erase()
擦除过的条目之间没有区别；最近被存入 ``NULL`` 的条目也是等价的，除非该
XArray 是用 ``XA_FLAGS_ALLOC`` 初始化的。)

You can conditionally replace an entry at an index by using
xa_cmpxchg().  Like cmpxchg(), it will only succeed if
the entry at that index has the 'old' value.  It also returns the entry
which was at that index; if it returns the same entry which was passed as
'old', then xa_cmpxchg() succeeded.
(可以使用 xa_cmpxchg() 有条件地替换某个索引处的条目。与 cmpxchg() 类似，只有
当该索引处的条目为 'old' 值时才会成功。它还返回当时位于该索引的条目；如果返回
的条目与传入的 'old' 相同，则 xa_cmpxchg() 成功。)

If you want to only store a new entry to an index if the current entry
at that index is ``NULL``, you can use xa_insert() which
returns ``-EBUSY`` if the entry is not empty.
(如果只想在当前条目为 ``NULL`` 时向某个索引存储新条目，可以使用 xa_insert()，
当条目非空时它返回 ``-EBUSY`` 。)

You can copy entries out of the XArray into a plain array by calling
xa_extract().  Or you can iterate over the present entries in the XArray
by calling xa_for_each(), xa_for_each_start() or xa_for_each_range().
You may prefer to use xa_find() or xa_find_after() to move to the next
present entry in the XArray.
(可以调用 xa_extract() 把条目从 XArray 复制到一个普通数组中。也可以调用
xa_for_each()、xa_for_each_start() 或 xa_for_each_range() 来迭代 XArray 中
现存的条目。你可能更愿意使用 xa_find() 或 xa_find_after() 移动到 XArray 中
下一个现存的条目。)

Calling xa_store_range() stores the same entry in a range
of indices.  If you do this, some of the other operations will behave
in a slightly odd way.  For example, marking the entry at one index
may result in the entry being marked at some, but not all of the other
indices.  Storing into one index may result in the entry retrieved by
some, but not all of the other indices changing.
(调用 xa_store_range() 会把同一个条目存储到一段索引范围。如果这样做，其他一些
操作的行为会略有奇怪。例如，标记某个索引处的条目可能导致该条目只在部分而非
全部其他索引处被标记。向某个索引存储可能导致部分而非全部其他索引取回的条目
发生改变。)

Sometimes you need to ensure that a subsequent call to xa_store()
will not need to allocate memory.  The xa_reserve() function
will store a reserved entry at the indicated index.  Users of the
normal API will see this entry as containing ``NULL``.  If you do
not need to use the reserved entry, you can call xa_release()
to remove the unused entry.  If another user has stored to the entry
in the meantime, xa_release() will do nothing; if instead you
want the entry to become ``NULL``, you should use xa_erase().
Using xa_insert() on a reserved entry will fail.
(有时你需要确保后续对 xa_store() 的调用不需要分配内存。xa_reserve() 函数会在
指定索引处存储一个保留条目。普通 API 的使用者会看到该条目包含 ``NULL`` 。如果
你不再需要使用这个保留条目，可以调用 xa_release() 移除这个未使用的条目。如果
在此期间其他使用者已经向该条目存储过，xa_release() 将不做任何事；如果你想让
该条目变为 ``NULL`` ，则应使用 xa_erase()。对保留条目使用 xa_insert() 会
失败。)

If all entries in the array are ``NULL``, the xa_empty() function
will return ``true``.
(如果数组中的所有条目都是 ``NULL`` ，xa_empty() 函数将返回 ``true`` 。)

Finally, you can remove all entries from an XArray by calling
xa_destroy().  If the XArray entries are pointers, you may wish
to free the entries first.  You can do this by iterating over all present
entries in the XArray using the xa_for_each() iterator.
(最后，可以调用 xa_destroy() 从 XArray 中移除所有条目。如果 XArray 条目是
指针，你可能希望先释放它们。可以用 xa_for_each() 迭代器遍历 XArray 中所有
现存的条目来完成这一点。)

Search Marks(搜索标记)
----------------------

Each entry in the array has three bits associated with it called marks.
Each mark may be set or cleared independently of the others.  You can
iterate over marked entries by using the xa_for_each_marked() iterator.
(数组中的每个条目都有三个与之关联的位，称为标记 (mark)。每个标记可以独立于
其他标记进行设置或清除。可以使用 xa_for_each_marked() 迭代器迭代已标记的
条目。)

You can enquire whether a mark is set on an entry by using
xa_get_mark().  If the entry is not ``NULL``, you can set a mark on it
by using xa_set_mark() and remove the mark from an entry by calling
xa_clear_mark().  You can ask whether any entry in the XArray has a
particular mark set by calling xa_marked().  Erasing an entry from the
XArray causes all marks associated with that entry to be cleared.
(可以使用 xa_get_mark() 查询某个条目是否设置了标记。如果条目不是 ``NULL`` ，
可以用 xa_set_mark() 在其上设置标记，用 xa_clear_mark() 移除条目上的标记。
可以调用 xa_marked() 询问 XArray 中是否有任何条目设置了特定标记。从 XArray
擦除一个条目会导致与该条目关联的所有标记被清除。)

Setting or clearing a mark on any index of a multi-index entry will
affect all indices covered by that entry.  Querying the mark on any
index will return the same result.
(对多索引条目的任何索引设置或清除标记，会影响该条目覆盖的所有索引。查询任何
索引的标记都会返回相同的结果。)

There is no way to iterate over entries which are not marked; the data
structure does not allow this to be implemented efficiently.  There are
not currently iterators to search for logical combinations of bits (eg
iterate over all entries which have both ``XA_MARK_1`` and ``XA_MARK_2``
set, or iterate over all entries which have ``XA_MARK_0`` or ``XA_MARK_2``
set).  It would be possible to add these if a user arises.
(没有办法迭代未标记的条目；数据结构不允许高效地实现这一点。目前也没有用于
搜索位的逻辑组合的迭代器 (例如迭代同时设置了 ``XA_MARK_1`` 和 ``XA_MARK_2``
的所有条目，或迭代设置了 ``XA_MARK_0`` 或 ``XA_MARK_2`` 的所有条目)。如果有
使用者出现，可以添加这些功能。)

Allocating XArrays(分配 XArray)
-------------------------------

If you use DEFINE_XARRAY_ALLOC() to define the XArray, or
initialise it by passing ``XA_FLAGS_ALLOC`` to xa_init_flags(),
the XArray changes to track whether entries are in use or not.
(如果使用 DEFINE_XARRAY_ALLOC() 定义 XArray，或通过向 xa_init_flags() 传递
``XA_FLAGS_ALLOC`` 来初始化它，XArray 会转为跟踪条目是否正在使用。)

You can call xa_alloc() to store the entry at an unused index
in the XArray.  If you need to modify the array from interrupt context,
you can use xa_alloc_bh() or xa_alloc_irq() to disable
interrupts while allocating the ID.
(可以调用 xa_alloc() 把条目存储到 XArray 中一个未使用的索引。如果需要在中断
上下文中修改数组，可以使用 xa_alloc_bh() 或 xa_alloc_irq() 在分配 ID 时禁用
中断。)

Using xa_store(), xa_cmpxchg() or xa_insert() will
also mark the entry as being allocated.  Unlike a normal XArray, storing
``NULL`` will mark the entry as being in use, like xa_reserve().
To free an entry, use xa_erase() (or xa_release() if
you only want to free the entry if it's ``NULL``).
(使用 xa_store()、xa_cmpxchg() 或 xa_insert() 也会把条目标记为已分配。与普通
XArray 不同，存储 ``NULL`` 会像 xa_reserve() 一样把条目标记为正在使用。要释放
条目，使用 xa_erase() (如果只想在该条目为 ``NULL`` 时才释放，则使用
xa_release() )。)

By default, the lowest free entry is allocated starting from 0.  If you
want to allocate entries starting at 1, it is more efficient to use
DEFINE_XARRAY_ALLOC1() or ``XA_FLAGS_ALLOC1``.  If you want to
allocate IDs up to a maximum, then wrap back around to the lowest free
ID, you can use xa_alloc_cyclic().
(默认情况下，从 0 开始分配最低的空闲条目。如果想从 1 开始分配条目，使用
DEFINE_XARRAY_ALLOC1() 或 ``XA_FLAGS_ALLOC1`` 会更高效。如果想分配 ID 直到
某个最大值，然后回绕到最低的空闲 ID，可以使用 xa_alloc_cyclic()。)

You cannot use ``XA_MARK_0`` with an allocating XArray as this mark
is used to track whether an entry is free or not.  The other marks are
available for your use.
(不能在分配型 XArray 中使用 ``XA_MARK_0``，因为该标记用于跟踪条目是否空闲。
其他标记可供你使用。)

Memory allocation(内存分配)
---------------------------

The xa_store(), xa_cmpxchg(), xa_alloc(),
xa_reserve() and xa_insert() functions take a gfp_t
parameter in case the XArray needs to allocate memory to store this entry.
If the entry is being deleted, no memory allocation needs to be performed,
and the GFP flags specified will be ignored.
( xa_store()、xa_cmpxchg()、xa_alloc()、xa_reserve() 与 xa_insert() 函数都
接受一个 gfp_t 参数，以备 XArray 需要分配内存来存储该条目。如果要删除条目，
则不需要执行内存分配，指定的 GFP 标志将被忽略。)

It is possible for no memory to be allocatable, particularly if you pass
a restrictive set of GFP flags.  In that case, the functions return a
special value which can be turned into an errno using xa_err().
If you don't need to know exactly which error occurred, using
xa_is_err() is slightly more efficient.
(可能出现无法分配内存的情况，尤其是在传入一组限制严格的 GFP 标志时。在这种
情况下，这些函数会返回一个特殊值，可以用 xa_err() 把它转换为 errno。如果你
不需要确切知道发生了哪个错误，使用 xa_is_err() 会稍微更高效一些。)

Locking(锁)
-----------

When using the Normal API, you do not have to worry about locking.
The XArray uses RCU and an internal spinlock to synchronise access:
(使用普通 API 时，你不必操心加锁问题。XArray 使用 RCU 和一个内部自旋锁来
同步访问：)

No lock needed:
(无需加锁：)

 * xa_empty()
 * xa_marked()

Takes RCU read lock:
(持有 RCU 读锁：)

 * xa_load()
 * xa_for_each()
 * xa_for_each_start()
 * xa_for_each_range()
 * xa_find()
 * xa_find_after()
 * xa_extract()
 * xa_get_mark()

Takes xa_lock internally:
(内部获取 xa_lock：)

 * xa_store()
 * xa_store_bh()
 * xa_store_irq()
 * xa_insert()
 * xa_insert_bh()
 * xa_insert_irq()
 * xa_erase()
 * xa_erase_bh()
 * xa_erase_irq()
 * xa_cmpxchg()
 * xa_cmpxchg_bh()
 * xa_cmpxchg_irq()
 * xa_store_range()
 * xa_alloc()
 * xa_alloc_bh()
 * xa_alloc_irq()
 * xa_reserve()
 * xa_reserve_bh()
 * xa_reserve_irq()
 * xa_destroy()
 * xa_set_mark()
 * xa_clear_mark()

Assumes xa_lock held on entry:
(进入时假定已持有 xa_lock：)

 * __xa_store()
 * __xa_insert()
 * __xa_erase()
 * __xa_cmpxchg()
 * __xa_alloc()
 * __xa_set_mark()
 * __xa_clear_mark()

If you want to take advantage of the lock to protect the data structures
that you are storing in the XArray, you can call xa_lock()
before calling xa_load(), then take a reference count on the
object you have found before calling xa_unlock().  This will
prevent stores from removing the object from the array between looking
up the object and incrementing the refcount.  You can also use RCU to
avoid dereferencing freed memory, but an explanation of that is beyond
the scope of this document.
(如果想利用这个锁来保护你存储在 XArray 中的数据结构，可以在调用 xa_load()
之前调用 xa_lock()，然后在调用 xa_unlock() 之前对找到的对象获取一个引用
计数。这样可以防止在查找对象与递增引用计数之间，存储操作把对象从数组中移除。
也可以使用 RCU 来避免解引用已释放的内存，但这超出了本文档的范围。)

The XArray does not disable interrupts or softirqs while modifying
the array.  It is safe to read the XArray from interrupt or softirq
context as the RCU lock provides enough protection.
( XArray 在修改数组时不会禁用中断或软中断。从中断或软中断上下文读取 XArray
是安全的，因为 RCU 锁提供了足够的保护。)

If, for example, you want to store entries in the XArray in process
context and then erase them in softirq context, you can do that this way::

    void foo_init(struct foo *foo)
    {
        xa_init_flags(&foo->array, XA_FLAGS_LOCK_BH);
    }

    int foo_store(struct foo *foo, unsigned long index, void *entry)
    {
        int err;

        xa_lock_bh(&foo->array);
        err = xa_err(__xa_store(&foo->array, index, entry, GFP_KERNEL));
        if (!err)
            foo->count++;
        xa_unlock_bh(&foo->array);
        return err;
    }

    /* foo_erase() is only called from softirq context */
    void foo_erase(struct foo *foo, unsigned long index)
    {
        xa_lock(&foo->array);
        __xa_erase(&foo->array, index);
        foo->count--;
        xa_unlock(&foo->array);
    }

(例如，如果你想在进程上下文向 XArray 存储条目，然后在软中断上下文擦除它们，
可以这样实现：)

If you are going to modify the XArray from interrupt or softirq context,
you need to initialise the array using xa_init_flags(), passing
``XA_FLAGS_LOCK_IRQ`` or ``XA_FLAGS_LOCK_BH``.
(如果要从中断或软中断上下文修改 XArray，需要用 xa_init_flags() 初始化数组，
传递 ``XA_FLAGS_LOCK_IRQ`` 或 ``XA_FLAGS_LOCK_BH``。)

The above example also shows a common pattern of wanting to extend the
coverage of the xa_lock on the store side to protect some statistics
associated with the array.
(上面的例子还展示了一种常见模式：希望把存储侧 xa_lock 的覆盖范围扩展到保护
与该数组相关的一些统计信息。)

Sharing the XArray with interrupt context is also possible, either
using xa_lock_irqsave() in both the interrupt handler and process
context, or xa_lock_irq() in process context and xa_lock()
in the interrupt handler.  Some of the more common patterns have helper
functions such as xa_store_bh(), xa_store_irq(),
xa_erase_bh(), xa_erase_irq(), xa_cmpxchg_bh()
and xa_cmpxchg_irq().
(与中断上下文共享 XArray 也是可能的：在中断处理程序和进程上下文中都使用
xa_lock_irqsave()，或者在进程上下文用 xa_lock_irq() 而在中断处理程序中用
xa_lock()。一些更常见的模式提供了辅助函数，例如 xa_store_bh()、
xa_store_irq()、xa_erase_bh()、xa_erase_irq()、xa_cmpxchg_bh() 与
xa_cmpxchg_irq()。)

Sometimes you need to protect access to the XArray with a mutex because
that lock sits above another mutex in the locking hierarchy.  That does
not entitle you to use functions like __xa_erase() without taking
the xa_lock; the xa_lock is used for lockdep validation and will be used
for other purposes in the future.
(有时需要用互斥锁保护对 XArray 的访问，因为该锁在锁层级中位于另一个互斥锁
之上。但这并不赋予你在不持有 xa_lock 的情况下使用 __xa_erase() 之类函数的
权利；xa_lock 用于 lockdep 验证，将来还会用于其他目的。)

The __xa_set_mark() and __xa_clear_mark() functions are also
available for situations where you look up an entry and want to atomically
set or clear a mark.  It may be more efficient to use the advanced API
in this case, as it will save you from walking the tree twice.
( __xa_set_mark() 与 __xa_clear_mark() 函数也可用于查找条目并希望原子地设置
或清除标记的情形。在这种情况下使用高级 API 可能更高效，因为它可以让你免于
两次遍历树。)

Advanced API(高级 API)
======================

The advanced API offers more flexibility and better performance at the
cost of an interface which can be harder to use and has fewer safeguards.
No locking is done for you by the advanced API, and you are required
to use the xa_lock while modifying the array.  You can choose whether
to use the xa_lock or the RCU lock while doing read-only operations on
the array.  You can mix advanced and normal operations on the same array;
indeed the normal API is implemented in terms of the advanced API.  The
advanced API is only available to modules with a GPL-compatible license.
(高级 API 以更难使用、保护更少的接口为代价，提供更大的灵活性和更好的性能。
高级 API 不为你做任何加锁，并且要求你在修改数组时使用 xa_lock。对数组进行
只读操作时，你可以选择使用 xa_lock 还是 RCU 锁。可以在同一个数组上混合使用
高级操作与普通操作；实际上普通 API 就是以高级 API 来实现的。高级 API 仅对
拥有 GPL 兼容许可证的模块可用。)

The advanced API is based around the xa_state.  This is an opaque data
structure which you declare on the stack using the XA_STATE() macro.
This macro initialises the xa_state ready to start walking around the
XArray.  It is used as a cursor to maintain the position in the XArray
and let you compose various operations together without having to restart
from the top every time.  The contents of the xa_state are protected by
the rcu_read_lock() or the xas_lock().  If you need to drop whichever of
those locks is protecting your state and tree, you must call xas_pause()
so that future calls do not rely on the parts of the state which were
left unprotected.
(高级 API 围绕 xa_state 构建。这是一种不透明的数据结构，用 XA_STATE() 宏在
栈上声明。该宏把 xa_state 初始化好，随时可以开始遍历 XArray。它被用作游标来
维护在 XArray 中的位置，让你可以组合各种操作而无需每次都从头开始。xa_state
的内容由 rcu_read_lock() 或 xas_lock() 保护。如果你需要释放保护你的状态和树
的那些锁中的某一个，必须调用 xas_pause()，使后续调用不依赖于状态中未被保护
的部分。)

The xa_state is also used to store errors.  You can call
xas_error() to retrieve the error.  All operations check whether
the xa_state is in an error state before proceeding, so there's no need
for you to check for an error after each call; you can make multiple
calls in succession and only check at a convenient point.  The only
errors currently generated by the XArray code itself are ``ENOMEM`` and
``EINVAL``, but it supports arbitrary errors in case you want to call
xas_set_err() yourself.
( xa_state 也用于存储错误。可以调用 xas_error() 获取错误。所有操作在继续
之前都会检查 xa_state 是否处于错误状态，因此你无需在每次调用之后检查错误；
可以连续多次调用，只在一个方便的时机检查。目前 XArray 代码自身产生的错误
只有 ``ENOMEM`` 和 ``EINVAL`` ，但它支持任意错误，以便你想自己调用
xas_set_err()。)

If the xa_state is holding an ``ENOMEM`` error, calling xas_nomem()
will attempt to allocate more memory using the specified gfp flags and
cache it in the xa_state for the next attempt.  The idea is that you take
the xa_lock, attempt the operation and drop the lock.  The operation
attempts to allocate memory while holding the lock, but it is more
likely to fail.  Once you have dropped the lock, xas_nomem()
can try harder to allocate more memory.  It will return ``true`` if it
is worth retrying the operation (i.e. that there was a memory error *and*
more memory was allocated).  If it has previously allocated memory, and
that memory wasn't used, and there is no error (or some error that isn't
``ENOMEM``), then it will free the memory previously allocated.
(如果 xa_state 持有 ``ENOMEM`` 错误，调用 xas_nomem() 会尝试用指定的 gfp
标志分配更多内存，并把它缓存在 xa_state 中以备下次尝试。其思路是：获取
xa_lock，尝试操作，然后释放锁。操作在持锁期间尝试分配内存，但更可能失败。
释放锁之后，xas_nomem() 可以更努力地尝试分配更多内存。如果值得重试该操作
(即发生了内存错误 *并且* 分配到了更多内存)，它会返回 ``true`` 。如果它此前
分配的内存没有被使用，并且没有错误 (或者错误不是 ``ENOMEM`` )，则它会释放
之前分配的内存。)

Internal Entries(内部条目)
--------------------------

The XArray reserves some entries for its own purposes.  These are never
exposed through the normal API, but when using the advanced API, it's
possible to see them.  Usually the best way to handle them is to pass them
to xas_retry(), and retry the operation if it returns ``true``.
( XArray 为自己的用途保留了一些条目。这些条目永远不会通过普通 API 暴露出来，
但在使用高级 API 时可能会看到它们。通常处理它们的最佳方式是把它们传给
xas_retry()，如果它返回 ``true`` 就重试该操作。)

.. flat-table::
   :widths: 1 1 6

   * - Name(名称)
     - Test(测试)
     - Usage(用法)

   * - Node
     - xa_is_node()
     - An XArray node.  May be visible when using a multi-index xa_state.
       (一个 XArray 节点。使用多索引 xa_state 时可能可见。)

   * - Sibling
     - xa_is_sibling()
     - A non-canonical entry for a multi-index entry.  The value indicates
       which slot in this node has the canonical entry.
       (多索引条目的非规范条目。该值指出本节点中哪个槽位拥有规范条目。)

   * - Retry
     - xa_is_retry()
     - This entry is currently being modified by a thread which has the
       xa_lock.  The node containing this entry may be freed at the end
       of this RCU period.  You should restart the lookup from the head
       of the array.
       (该条目当前正被持有 xa_lock 的线程修改。包含该条目的节点可能在本
       RCU 周期结束时被释放。你应该从数组的头部重新开始查找。)

   * - Zero
     - xa_is_zero()
     - Zero entries appear as ``NULL`` through the Normal API, but occupy
       an entry in the XArray which can be used to reserve the index for
       future use.  This is used by allocating XArrays for allocated entries
       which are ``NULL``.
       (零条目通过普通 API 呈现为 ``NULL`` ，但占用 XArray 中的一个条目，可以
       用它为将来的使用保留该索引。分配型 XArray 用它表示已分配但为
       ``NULL`` 的条目。)

Other internal entries may be added in the future.  As far as possible, they
will be handled by xas_retry().
(将来可能会添加其他内部条目。它们会尽可能由 xas_retry() 来处理。)

Additional functionality(附加功能)
----------------------------------

The xas_create_range() function allocates all the necessary memory
to store every entry in a range.  It will set ENOMEM in the xa_state if
it cannot allocate memory.
( xas_create_range() 函数分配在一段范围内存储每个条目所需的全部内存。如果
无法分配内存，它会在 xa_state 中设置 ENOMEM。)

You can use xas_init_marks() to reset the marks on an entry
to their default state.  This is usually all marks clear, unless the
XArray is marked with ``XA_FLAGS_TRACK_FREE``, in which case mark 0 is set
and all other marks are clear.  Replacing one entry with another using
xas_store() will not reset the marks on that entry; if you want
the marks reset, you should do that explicitly.
(可以用 xas_init_marks() 把条目上的标记重置为默认状态。通常是清除所有标记，
除非 XArray 被标记了 ``XA_FLAGS_TRACK_FREE``，此时标记 0 被设置，其他所有
标记被清除。用 xas_store() 把一个条目替换为另一个条目不会重置该条目上的
标记；如果你想重置标记，应当显式地进行。)

The xas_load() will walk the xa_state as close to the entry
as it can.  If you know the xa_state has already been walked to the
entry and need to check that the entry hasn't changed, you can use
xas_reload() to save a function call.
( xas_load() 会把 xa_state 尽可能深入地走到接近该条目的位置。如果你知道
xa_state 已经走到该条目，并且需要检查该条目没有改变，可以使用 xas_reload()
来省去一次函数调用。)

If you need to move to a different index in the XArray, call
xas_set().  This resets the cursor to the top of the tree, which
will generally make the next operation walk the cursor to the desired
spot in the tree.  If you want to move to the next or previous index,
call xas_next() or xas_prev().  Setting the index does
not walk the cursor around the array so does not require a lock to be
held, while moving to the next or previous index does.
(如果需要移动到 XArray 中的另一个索引，调用 xas_set()。它把游标重置到树的
顶部，这通常会让下一个操作把游标走到树中期望的位置。如果想移动到下一个或
上一个索引，调用 xas_next() 或 xas_prev()。设置索引不会让游标在数组中走动，
因此不需要持有锁；而移动到下一个或上一个索引则需要。)

You can search for the next present entry using xas_find().  This
is the equivalent of both xa_find() and xa_find_after();
if the cursor has been walked to an entry, then it will find the next
entry after the one currently referenced.  If not, it will return the
entry at the index of the xa_state.  Using xas_next_entry() to
move to the next present entry instead of xas_find() will save
a function call in the majority of cases at the expense of emitting more
inline code.
(可以用 xas_find() 搜索下一个现存的条目。它同时等价于 xa_find() 与
xa_find_after()；如果游标已经走到某个条目，它会找到当前引用条目之后的
下一个条目；如果没有，它会返回 xa_state 索引处的条目。在大多数情况下，用
xas_next_entry() 代替 xas_find() 移动到下一个现存条目可以省去一次函数
调用，代价是生成更多的内联代码。)

The xas_find_marked() function is similar.  If the xa_state has
not been walked, it will return the entry at the index of the xa_state,
if it is marked.  Otherwise, it will return the first marked entry after
the entry referenced by the xa_state.  The xas_next_marked()
function is the equivalent of xas_next_entry().
( xas_find_marked() 函数与之类似。如果 xa_state 还没有走过，它返回
xa_state 索引处的条目 (如果该条目已标记)。否则，它返回 xa_state 所引用
条目之后的第一个已标记条目。 xas_next_marked() 函数等价于
xas_next_entry()。)

When iterating over a range of the XArray using xas_for_each()
or xas_for_each_marked(), it may be necessary to temporarily stop
the iteration.  The xas_pause() function exists for this purpose.
After you have done the necessary work and wish to resume, the xa_state
is in an appropriate state to continue the iteration after the entry
you last processed.  If you have interrupts disabled while iterating,
then it is good manners to pause the iteration and reenable interrupts
every ``XA_CHECK_SCHED`` entries.
(使用 xas_for_each() 或 xas_for_each_marked() 迭代 XArray 的一段范围时，
可能需要临时停止迭代。 xas_pause() 函数就是为此而存在的。在你完成必要的
工作并希望恢复之后，xa_state 处于合适的状态，可以继续迭代你最后处理的条目
之后的条目。如果迭代期间你禁用了中断，那么礼貌的做法是每处理
``XA_CHECK_SCHED`` 个条目就暂停迭代并重新启用中断。)

The xas_get_mark(), xas_set_mark() and xas_clear_mark() functions require
the xa_state cursor to have been moved to the appropriate location in the
XArray; they will do nothing if you have called xas_pause() or xas_set()
immediately before.
( xas_get_mark()、xas_set_mark() 与 xas_clear_mark() 函数要求 xa_state 游标
已被移动到 XArray 中相应的位置；如果你在此之前刚调用了 xas_pause() 或
xas_set()，它们将什么也不做。)

You can call xas_set_update() to have a callback function
called each time the XArray updates a node.  This is used by the page
cache workingset code to maintain its list of nodes which contain only
shadow entries.
(可以调用 xas_set_update() 让 XArray 在每次更新节点时调用一个回调函数。页
缓存 workingset 代码用它来维护其仅包含影子条目的节点列表。)

Multi-Index Entries(多索引条目)
-------------------------------

The XArray has the ability to tie multiple indices together so that
operations on one index affect all indices.  For example, storing into
any index will change the value of the entry retrieved from any index.
Setting or clearing a mark on any index will set or clear the mark
on every index that is tied together.  The current implementation
only allows tying ranges which are aligned powers of two together;
eg indices 64-127 may be tied together, but 2-6 may not be.  This may
save substantial quantities of memory; for example tying 512 entries
together will save over 4kB.
( XArray 能够把多个索引绑在一起，使得对一个索引的操作影响所有索引。例如，
向任何一个索引存储都会改变从任何索引取回的条目的值。对任何索引设置或清除
标记，会同时设置或清除所有被绑定索引上的标记。当前实现只允许绑定按 2 的幂
对齐的范围；例如索引 64-127 可以绑定在一起，而 2-6 不行。这可以节省大量
内存；例如把 512 个条目绑在一起可以节省 4kB 以上的内存。)

You can create a multi-index entry by using XA_STATE_ORDER()
or xas_set_order() followed by a call to xas_store().
Calling xas_load() with a multi-index xa_state will walk the
xa_state to the right location in the tree, but the return value is not
meaningful, potentially being an internal entry or ``NULL`` even when there
is an entry stored within the range.  Calling xas_find_conflict()
will return the first entry within the range or ``NULL`` if there are no
entries in the range.  The xas_for_each_conflict() iterator will
iterate over every entry which overlaps the specified range.
(可以使用 XA_STATE_ORDER() 或 xas_set_order() 创建多索引条目，随后调用
xas_store()。用多索引 xa_state 调用 xas_load() 会把 xa_state 走到树中正确
的位置，但返回值没有意义——即使该范围内存储了条目，返回值也可能是内部条目
或 ``NULL`` 。调用 xas_find_conflict() 会返回该范围内的第一个条目，如果
范围内没有条目则返回 ``NULL`` 。 xas_for_each_conflict() 迭代器会迭代与
指定范围重叠的每一个条目。)

If xas_load() encounters a multi-index entry, the xa_index
in the xa_state will not be changed.  When iterating over an XArray
or calling xas_find(), if the initial index is in the middle
of a multi-index entry, it will not be altered.  Subsequent calls
or iterations will move the index to the first index in the range.
Each entry will only be returned once, no matter how many indices it
occupies.
(如果 xas_load() 遇到多索引条目，xa_state 中的 xa_index 不会改变。在迭代
XArray 或调用 xas_find() 时，如果初始索引位于某个多索引条目的中间，它也
不会被改变。后续的调用或迭代会把索引移动到该范围内的第一个索引。每个条目
只会被返回一次，无论它占据多少个索引。)

Using xas_next() or xas_prev() with a multi-index xa_state is not
supported.  Using either of these functions on a multi-index entry will
reveal sibling entries; these should be skipped over by the caller.
(不支持对多索引 xa_state 使用 xas_next() 或 xas_prev()。对多索引条目使用
这两个函数中的任何一个都会暴露出兄弟 (sibling) 条目；调用者应当跳过它们。)

Storing ``NULL`` into any index of a multi-index entry will set the
entry at every index to ``NULL`` and dissolve the tie.  A multi-index
entry can be split into entries occupying smaller ranges by calling
xas_split_alloc() without the xa_lock held, followed by taking the lock
and calling xas_split() or calling xas_try_split() with xa_lock. The
difference between xas_split_alloc()+xas_split() and xas_try_alloc() is
that xas_split_alloc() + xas_split() split the entry from the original
order to the new order in one shot uniformly, whereas xas_try_split()
iteratively splits the entry containing the index non-uniformly.
For example, to split an order-9 entry, which takes 2^(9-6)=8 slots,
assuming ``XA_CHUNK_SHIFT`` is 6, xas_split_alloc() + xas_split() need
8 xa_node. xas_try_split() splits the order-9 entry into
2 order-8 entries, then split one order-8 entry, based on the given index,
to 2 order-7 entries, ..., and split one order-1 entry to 2 order-0 entries.
When splitting the order-6 entry and a new xa_node is needed, xas_try_split()
will try to allocate one if possible. As a result, xas_try_split() would only
need 1 xa_node instead of 8.
(向多索引条目的任何索引存入 ``NULL`` 会把所有索引处的条目都设为 ``NULL`` 并
解除绑定。可以在不持有 xa_lock 的情况下调用 xas_split_alloc()，随后获取锁
并调用 xas_split()，或者在持有 xa_lock 的情况下调用 xas_try_split()，从而
把一个多索引条目拆分为占据更小范围的条目。 xas_split_alloc()+xas_split()
与 xas_try_alloc() 的区别在于： xas_split_alloc() + xas_split() 从原始阶
(order) 到新阶一次性均匀地拆分条目，而 xas_try_split() 则非均匀地迭代拆分
包含给定索引的条目。例如，要拆分一个占用 2^(9-6)=8 个槽位的 order-9 条目，
假设 ``XA_CHUNK_SHIFT`` 为 6， xas_split_alloc() + xas_split() 需要 8 个
xa_node。而 xas_try_split() 先把 order-9 条目拆成 2 个 order-8 条目，然后
根据给定的索引，把其中一个 order-8 条目拆成 2 个 order-7 条目，……，并把
一个 order-1 条目拆成 2 个 order-0 条目。当拆分 order-6 条目且需要新的
xa_node 时，xas_try_split() 会尽可能尝试分配一个。因此， xas_try_split()
只需要 1 个 xa_node 而不是 8 个。)

Functions and structures(函数与结构)
====================================

.. kernel-doc:: include/linux/xarray.h
.. kernel-doc:: lib/xarray.c
