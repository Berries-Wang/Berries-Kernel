.. SPDX-License-Identifier: GPL-2.0+

=======================
Folio Queue(Folio 队列)
=======================

:Author: David Howells <dhowells@redhat.com>

(作者: David Howells <dhowells@redhat.com>)

.. Contents:

 * Overview(概述)
 * Initialisation(初始化)
 * Adding and removing folios(添加与移除 folio)
 * Querying information about a folio(查询关于 folio 的信息)
 * Querying information about a folio_queue(查询关于 folio_queue 的信息)
 * Folio queue iteration(folio 队列迭代)
 * Folio marks(folio 标记)
 * Lockless simultaneous production/consumption issues(无锁并发生产/消费问题)


Overview(概述)
==============

The folio_queue struct forms a single segment in a segmented list of folios
that can be used to form an I/O buffer.  As such, the list can be iterated over
using the ITER_FOLIOQ iov_iter type.
(folio_queue 结构体构成一个分段 folio 列表中的单个段(segment)，可用于组成 I/O 缓冲区。因此，可以使用 ITER_FOLIOQ 类型的 iov_iter 来遍历该列表。)

The publicly accessible members of the structure are::

	struct folio_queue {
		struct folio_queue *next;
		struct folio_queue *prev;
		...
	};

(该结构体公开可访问的成员如下：)

A pair of pointers are provided, ``next`` and ``prev``, that point to the
segments on either side of the segment being accessed.  Whilst this is a
doubly-linked list, it is intentionally not a circular list; the outward
sibling pointers in terminal segments should be NULL.
(结构体提供了一对指针 ``next`` 和 ``prev`` ，分别指向所访问段两侧的相邻段。虽然这是一个双向链表，但它刻意不是循环链表；末端段朝外的兄弟指针应为 NULL。)

Each segment in the list also stores:

 * an ordered sequence of folio pointers,
   (一个有序的 folio 指针序列，)
 * the size of each folio and
   (每个 folio 的大小，以及)
 * three 1-bit marks per folio,
   (每个 folio 三个 1 位标记，)

but these should not be accessed directly as the underlying data structure may
change, but rather the access functions outlined below should be used.
(但不应直接访问这些内容，因为底层数据结构可能会变化，而应使用下文概述的访问函数。)

The facility can be made accessible by::

	#include <linux/folio_queue.h>

(可通过以下方式使该功能可用：)

and to use the iterator::

	#include <linux/uio.h>

(而要使用迭代器，则包含以下头文件：)


Initialisation(初始化)
======================

A segment should be initialised by calling::

	void folioq_init(struct folio_queue *folioq);

(应通过调用以下函数来初始化一个段：)

with a pointer to the segment to be initialised.  Note that this will not
necessarily initialise all the folio pointers, so care must be taken to check
the number of folios added.
(调用时传入指向待初始化段的指针。注意，这未必会初始化所有 folio 指针，因此必须小心检查已添加的 folio 数量。)


Adding and removing folios(添加与移除 folio)
============================================

Folios can be set in the next unused slot in a segment struct by calling one
of::

	unsigned int folioq_append(struct folio_queue *folioq,
				   struct folio *folio);

	unsigned int folioq_append_mark(struct folio_queue *folioq,
					struct folio *folio);

(可以通过调用以下函数之一，把 folio 设置到段结构体中下一个未使用的槽位：)

Both functions update the stored folio count, store the folio and note its
size.  The second function also sets the first mark for the folio added.  Both
functions return the number of the slot used.  [!] Note that no attempt is made
to check that the capacity wasn't overrun and the list will not be extended
automatically.
(这两个函数都会更新已存储的 folio 计数，保存该 folio 并记录其大小。第二个函数还会为添加的 folio 设置第一个标记。两个函数都返回所用槽位的编号。[!] 注意，函数不会检查容量是否被超出，列表也不会自动扩展。)

A folio can be excised by calling::

	void folioq_clear(struct folio_queue *folioq, unsigned int slot);

(可以通过调用以下函数来剔除一个 folio：)

This clears the slot in the array and also clears all the marks for that folio,
but doesn't change the folio count - so future accesses of that slot must check
if the slot is occupied.
(这会清空数组中的该槽位，并清除该 folio 的所有标记，但不会改变 folio 计数——因此将来访问该槽位时必须检查该槽位是否已被占用。)


Querying information about a folio(查询关于 folio 的信息)
=========================================================

Information about the folio in a particular slot may be queried by the
following function::

	struct folio *folioq_folio(const struct folio_queue *folioq,
				   unsigned int slot);

(可以通过以下函数查询特定槽位中 folio 的信息：)

If a folio has not yet been set in that slot, this may yield an undefined
pointer.  The size of the folio in a slot may be queried with either of::

	unsigned int folioq_folio_order(const struct folio_queue *folioq,
					unsigned int slot);

	size_t folioq_folio_size(const struct folio_queue *folioq,
				 unsigned int slot);

(如果该槽位尚未设置 folio，这可能得到一个未定义的指针。可以用以下任一函数查询槽位中 folio 的大小：)

The first function returns the size as an order and the second as a number of
bytes.
(第一个函数以阶(order)的形式返回大小，第二个函数则以字节数的形式返回。)


Querying information about a folio_queue(查询关于 folio_queue 的信息)
=====================================================================

Information may be retrieved about a particular segment with the following
functions::

	unsigned int folioq_nr_slots(const struct folio_queue *folioq);

	unsigned int folioq_count(struct folio_queue *folioq);

	bool folioq_full(struct folio_queue *folioq);

(可以通过以下函数获取关于特定段的信息：)

The first function returns the maximum capacity of a segment.  It must not be
assumed that this won't vary between segments.  The second returns the number
of folios added to a segments and the third is a shorthand to indicate if the
segment has been filled to capacity.
(第一个函数返回一个段的最大容量。不能假定该值在各段之间不会变化。第二个函数返回已添加到段中的 folio 数量，第三个函数是"段是否已填满"的简写形式。)

Not that the count and fullness are not affected by clearing folios from the
segment.  These are more about indicating how many slots in the array have been
initialised, and it assumed that slots won't get reused, but rather the segment
will get discarded as the queue is consumed.
(注意，计数与满溢状态不会因从段中清除 folio 而受影响。它们更多是用于指示数组中已有多少个槽位被初始化，并且假定槽位不会被复用，而是在队列被消费时整个段被丢弃。)


Folio marks(folio 标记)
=======================

Folios within a queue can also have marks assigned to them.  These marks can be
used to note information such as if a folio needs folio_put() calling upon it.
There are three marks available to be set for each folio.
(队列中的 folio 还可以被赋予标记。这些标记可用于记录诸如"是否需要对某个 folio 调用 folio_put()"之类的信息。每个 folio 有三个可用的标记。)

The marks can be set by::

	void folioq_mark(struct folio_queue *folioq, unsigned int slot);
	void folioq_mark2(struct folio_queue *folioq, unsigned int slot);

(可以通过以下方式设置标记：)

Cleared by::

	void folioq_unmark(struct folio_queue *folioq, unsigned int slot);
	void folioq_unmark2(struct folio_queue *folioq, unsigned int slot);

(可以通过以下方式清除标记：)

And the marks can be queried by::

	bool folioq_is_marked(const struct folio_queue *folioq, unsigned int slot);
	bool folioq_is_marked2(const struct folio_queue *folioq, unsigned int slot);

(可以通过以下函数查询标记：)

The marks can be used for any purpose and are not interpreted by this API.
(这些标记可用于任何目的，本 API 不会对它们做任何解释。)


Folio queue iteration(folio 队列迭代)
=====================================

A list of segments may be iterated over using the I/O iterator facility using
an ``iov_iter`` iterator of ``ITER_FOLIOQ`` type.  The iterator may be
initialised with::

	void iov_iter_folio_queue(struct iov_iter *i, unsigned int direction,
				  const struct folio_queue *folioq,
				  unsigned int first_slot, unsigned int offset,
				  size_t count);

(可以使用 I/O 迭代器设施、以 ``ITER_FOLIOQ`` 类型的 ``iov_iter`` 迭代器来遍历段列表。可以用以下函数初始化迭代器：)

This may be told to start at a particular segment, slot and offset within a
queue.  The iov iterator functions will follow the next pointers when advancing
and prev pointers when reverting when needed.
(可以指定迭代器从队列中特定的段、槽位和偏移处开始。iov 迭代函数在前进时会跟随 next 指针，而在需要回退时跟随 prev 指针。)


Lockless simultaneous production/consumption issues(无锁并发生产/消费问题)
==========================================================================

If properly managed, the list can be extended by the producer at the head end
and shortened by the consumer at the tail end simultaneously without the need
to take locks.  The ITER_FOLIOQ iterator inserts appropriate barriers to aid
with this.
(如果管理得当，生产者可以在头部扩展列表，同时消费者在尾部缩短列表，而无需获取锁。ITER_FOLIOQ 迭代器会插入适当的内存屏障来辅助实现这一点。)

Care must be taken when simultaneously producing and consuming a list.  If the
last segment is reached and the folios it refers to are entirely consumed by
the IOV iterators, an iov_iter struct will be left pointing to the last segment
with a slot number equal to the capacity of that segment.  The iterator will
try to continue on from this if there's another segment available when it is
used again, but care must be taken lest the segment got removed and freed by
the consumer before the iterator was advanced.
(同时生产与消费列表时必须小心。当到达最后一个段、且它所引用的 folio 已被 IOV 迭代器完全消费时，iov_iter 结构体会停留在指向最后一个段、且槽位号等于该段容量的状态。如果再次使用迭代器时还有另一个段可用，迭代器会尝试从这里继续；但必须小心，以免该段在迭代器前进之前就被消费者移除并释放。)

It is recommended that the queue always contain at least one segment, even if
that segment has never been filled or is entirely spent.  This prevents the
head and tail pointers from collapsing.
(建议队列中始终至少保留一个段，即使该段从未被填满或已完全用尽。这可以防止头指针和尾指针坍缩到一起。)


API Function Reference(API 函数参考)
====================================

.. kernel-doc:: include/linux/folio_queue.h
