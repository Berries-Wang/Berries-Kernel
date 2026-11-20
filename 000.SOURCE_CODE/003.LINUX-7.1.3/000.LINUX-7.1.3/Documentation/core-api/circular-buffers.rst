==============================
Circular Buffers(循环缓冲区)
==============================

:Author: David Howells <dhowells@redhat.com>(作者：David Howells)
:Author: Paul E. McKenney <paulmck@linux.ibm.com>(作者：Paul E. McKenney)


Linux provides a number of features that can be used to implement circular
buffering.  There are two sets of such features:
(Linux 提供了许多可用于实现循环缓冲 (circular buffering) 的特性。这样的特性有两类：)

 (1) Convenience functions for determining information about power-of-2 sized
     buffers.
     ((1) 用于确定 2 的幂大小缓冲区相关信息的便利函数。)

 (2) Memory barriers for when the producer and the consumer of objects in the
     buffer don't want to share a lock.
     ((2) 当缓冲区中对象的生产者与消费者不想共享一把锁时使用的内存屏障。)

To use these facilities, as discussed below, there needs to be just one
producer and just one consumer.  It is possible to handle multiple producers by
serialising them, and to handle multiple consumers by serialising them.
(要使用这些设施，如下文所述，只需要一个生产者和一个消费者。可以通过串行化来处理多个生产者，也可以通过串行化来处理多个消费者。)


.. Contents:

 (*) What is a circular buffer?

 (*) Measuring power-of-2 buffers.

 (*) Using memory barriers with circular buffers.
     - The producer.
     - The consumer.



What is a circular buffer?(什么是循环缓冲区？)
==============================================

First of all, what is a circular buffer?  A circular buffer is a buffer of
fixed, finite size into which there are two indices:
(首先，什么是循环缓冲区？循环缓冲区是一个固定有限大小的缓冲区，其中有两个索引：)

 (1) A 'head' index - the point at which the producer inserts items into the
     buffer.
     ((1) 'head' (头) 索引——生产者向缓冲区插入条目的位置。)

 (2) A 'tail' index - the point at which the consumer finds the next item in
     the buffer.
     ((2) 'tail' (尾) 索引——消费者在缓冲区中找到下一个条目的位置。)

Typically when the tail pointer is equal to the head pointer, the buffer is
empty; and the buffer is full when the head pointer is one less than the tail
pointer.
(通常，当尾指针等于头指针时，缓冲区为空；当头指针比尾指针小 1 时，缓冲区为满。)

The head index is incremented when items are added, and the tail index when
items are removed.  The tail index should never jump the head index, and both
indices should be wrapped to 0 when they reach the end of the buffer, thus
allowing an infinite amount of data to flow through the buffer.
(添加条目时头索引递增，移除条目时尾索引递增。尾索引绝不应跳过头索引，并且两个索引到达缓冲区末尾时都应回绕到 0，从而允许无限量的数据流经缓冲区。)

Typically, items will all be of the same unit size, but this isn't strictly
required to use the techniques below.  The indices can be increased by more
than 1 if multiple items or variable-sized items are to be included in the
buffer, provided that neither index overtakes the other.  The implementer must
be careful, however, as a region more than one unit in size may wrap the end of
the buffer and be broken into two segments.
(通常，条目都具有相同的单元大小，但使用下述技术并不严格要求这一点。如果要往缓冲区中加入多个条目或可变大小的条目，索引一次可以增加多于 1，只要两个索引互不超越即可。不过实现者必须小心，因为大于一个单元的区域可能回绕跨越缓冲区末尾而被分割成两段。)

Measuring power-of-2 buffers(度量 2 的幂大小的缓冲区)
=====================================================

Calculation of the occupancy or the remaining capacity of an arbitrarily sized
circular buffer would normally be a slow operation, requiring the use of a
modulus (divide) instruction.  However, if the buffer is of a power-of-2 size,
then a much quicker bitwise-AND instruction can be used instead.
(计算任意大小循环缓冲区的占用率或剩余容量通常是一个慢操作，需要使用取模 (除法) 指令。不过，如果缓冲区大小是 2 的幂，就可以改用快得多的按位与指令。)

Linux provides a set of macros for handling power-of-2 circular buffers.  These
can be made use of by::

	#include <linux/circ_buf.h>

(Linux 提供了一组用于处理 2 的幂循环缓冲区的宏。可以这样使用它们：)

The macros are:
(这些宏是：)

 (#) Measure the remaining capacity of a buffer::

	CIRC_SPACE(head_index, tail_index, buffer_size);

     (测量缓冲区的剩余容量：)

     This returns the amount of space left in the buffer[1] into which items
     can be inserted.
     (它返回缓冲区[1]中剩余的空间量，可以往其中插入条目。)


 (#) Measure the maximum consecutive immediate space in a buffer::

	CIRC_SPACE_TO_END(head_index, tail_index, buffer_size);

     (测量缓冲区中最大的连续立即可用空间：)

     This returns the amount of consecutive space left in the buffer[1] into
     which items can be immediately inserted without having to wrap back to the
     beginning of the buffer.
     (它返回缓冲区[1]中剩余的连续空间量，可以立即往其中插入条目而无需回绕到缓冲区开头。)


 (#) Measure the occupancy of a buffer::

	CIRC_CNT(head_index, tail_index, buffer_size);

     (测量缓冲区的占用率：)

     This returns the number of items currently occupying a buffer[2].
     (它返回当前占用缓冲区[2]的条目数。)


 (#) Measure the non-wrapping occupancy of a buffer::

	CIRC_CNT_TO_END(head_index, tail_index, buffer_size);

     (测量缓冲区中不回绕的占用率：)

     This returns the number of consecutive items[2] that can be extracted from
     the buffer without having to wrap back to the beginning of the buffer.
     (它返回可以从缓冲区中提取的连续条目数[2]，无需回绕到缓冲区开头。)


Each of these macros will nominally return a value between 0 and buffer_size-1,
however:
(这些宏名义上都返回 0 到 buffer_size-1 之间的值，但是：)

 (1) CIRC_SPACE*() are intended to be used in the producer.  To the producer
     they will return a lower bound as the producer controls the head index,
     but the consumer may still be depleting the buffer on another CPU and
     moving the tail index.
     ((1) CIRC_SPACE*() 旨在由生产者使用。对生产者而言，它们返回的是下界，因为生产者控制头索引，而消费者可能还在另一个 CPU 上消耗缓冲区并移动尾索引。)

     To the consumer it will show an upper bound as the producer may be busy
     depleting the space.
     (对消费者而言，它显示的是上界，因为生产者可能正忙着消耗空间。)

 (2) CIRC_CNT*() are intended to be used in the consumer.  To the consumer they
     will return a lower bound as the consumer controls the tail index, but the
     producer may still be filling the buffer on another CPU and moving the
     head index.
     ((2) CIRC_CNT*() 旨在由消费者使用。对消费者而言，它们返回的是下界，因为消费者控制尾索引，而生产者可能还在另一个 CPU 上填充缓冲区并移动头索引。)

     To the producer it will show an upper bound as the consumer may be busy
     emptying the buffer.
     (对生产者而言，它显示的是上界，因为消费者可能正忙着清空缓冲区。)

 (3) To a third party, the order in which the writes to the indices by the
     producer and consumer become visible cannot be guaranteed as they are
     independent and may be made on different CPUs - so the result in such a
     situation will merely be a guess, and may even be negative.
     ((3) 对第三方而言，无法保证生产者与消费者对索引的写入的可见顺序，因为它们相互独立，且可能发生在不同的 CPU 上——这种情况下的结果仅是猜测，甚至可能为负。)

Using memory barriers with circular buffers(在循环缓冲区中使用内存屏障)
========================================================================

By using memory barriers in conjunction with circular buffers, you can avoid
the need to:
(通过把内存屏障与循环缓冲区结合使用，可以避免：)

 (1) use a single lock to govern access to both ends of the buffer, thus
     allowing the buffer to be filled and emptied at the same time; and
     ((1) 用单一锁来管理对缓冲区两端的访问，从而允许同时填充和清空缓冲区；以及)

 (2) use atomic counter operations.
     ((2) 使用原子计数器操作。)

There are two sides to this: the producer that fills the buffer, and the
consumer that empties it.  Only one thing should be filling a buffer at any one
time, and only one thing should be emptying a buffer at any one time, but the
two sides can operate simultaneously.
(这有两方：填充缓冲区的生产者，以及清空缓冲区的消费者。任一时刻只应有一方在填充缓冲区，也只应有一方在清空缓冲区，但双方可以同时操作。)


The producer(生产者)
--------------------

The producer will look something like this::

	spin_lock(&producer_lock);

	unsigned long head = buffer->head;
	/* The spin_unlock() and next spin_lock() provide needed ordering. */
	unsigned long tail = READ_ONCE(buffer->tail);

	if (CIRC_SPACE(head, tail, buffer->size) >= 1) {
		/* insert one item into the buffer */
		struct item *item = buffer[head];

		produce_item(item);

		smp_store_release(buffer->head,
				  (head + 1) & (buffer->size - 1));

		/* wake_up() will make sure that the head is committed before
		 * waking anyone up */
		wake_up(consumer);
	}

	spin_unlock(&producer_lock);

(生产者大致如下：)

This will instruct the CPU that the contents of the new item must be written
before the head index makes it available to the consumer and then instructs the
CPU that the revised head index must be written before the consumer is woken.
(这将指示 CPU：必须先写入新条目的内容，然后头索引才能使其对消费者可用；随后再指示 CPU：必须先写入修改后的头索引，然后才能唤醒消费者。)

Note that wake_up() does not guarantee any sort of barrier unless something
is actually awakened.  We therefore cannot rely on it for ordering.  However,
there is always one element of the array left empty.  Therefore, the
producer must produce two elements before it could possibly corrupt the
element currently being read by the consumer.  Therefore, the unlock-lock
pair between consecutive invocations of the consumer provides the necessary
ordering between the read of the index indicating that the consumer has
vacated a given element and the write by the producer to that same element.
(注意 wake_up() 在没有真正唤醒任何东西时不保证任何形式的屏障。因此我们不能依赖它来保证顺序。不过，数组中总有一个元素保持为空。因此，生产者必须先生产两个元素，才可能破坏消费者当前正在读取的元素。所以，消费者连续两次调用之间的解锁-加锁对，在"读取表明消费者已腾出某元素的索引"与"生产者对同一元素的写入"之间提供了必要的顺序。)


The Consumer(消费者)
--------------------

The consumer will look something like this::

	spin_lock(&consumer_lock);

	/* Read index before reading contents at that index. */
	unsigned long head = smp_load_acquire(buffer->head);
	unsigned long tail = buffer->tail;

	if (CIRC_CNT(head, tail, buffer->size) >= 1) {

		/* extract one item from the buffer */
		struct item *item = buffer[tail];

		consume_item(item);

		/* Finish reading descriptor before incrementing tail. */
		smp_store_release(buffer->tail,
				  (tail + 1) & (buffer->size - 1));
	}

	spin_unlock(&consumer_lock);

(消费者大致如下：)

This will instruct the CPU to make sure the index is up to date before reading
the new item, and then it shall make sure the CPU has finished reading the item
before it writes the new tail pointer, which will erase the item.
(这将指示 CPU 在读取新条目之前确保索引是最新的；随后它应确保 CPU 在写入将擦除该条目的新尾指针之前，已经读完该条目。)

Note the use of READ_ONCE() and smp_load_acquire() to read the
opposition index.  This prevents the compiler from discarding and
reloading its cached value.  This isn't strictly needed if you can
be sure that the opposition index will _only_ be used the once.
The smp_load_acquire() additionally forces the CPU to order against
subsequent memory references.  Similarly, smp_store_release() is used
in both algorithms to write the thread's index.  This documents the
fact that we are writing to something that can be read concurrently,
prevents the compiler from tearing the store, and enforces ordering
against previous accesses.
(注意使用 READ_ONCE() 与 smp_load_acquire() 来读取对方索引。这可防止编译器丢弃并重新加载其缓存值。如果能够确定对方索引 _only_ 会被使用一次，这并非严格必需。smp_load_acquire() 还强制 CPU 对后续内存引用保持顺序。类似地，两种算法都用 smp_store_release() 来写入本线程的索引。这记录了"我们正在写入可能被并发读取的内容"这一事实，防止编译器撕裂存储 (tearing the store)，并对之前的访问强制顺序。)


Further reading(延伸阅读)
=========================

See also Documentation/memory-barriers.txt for a description of Linux's memory
barrier facilities.
(另见 Documentation/memory-barriers.txt，其中有对 Linux 内存屏障设施的描述。)
