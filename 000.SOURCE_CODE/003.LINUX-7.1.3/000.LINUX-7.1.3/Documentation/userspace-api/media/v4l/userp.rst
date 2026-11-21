.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _userp:

***************************************************
Streaming I/O (User Pointers)(流式 I/O（用户指针）)
***************************************************

Input and output devices support this I/O method when the
``V4L2_CAP_STREAMING`` flag in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl is set. If the
particular user pointer method (not only memory mapping) is supported
must be determined by calling the :ref:`VIDIOC_REQBUFS` ioctl
with the memory type set to ``V4L2_MEMORY_USERPTR``.
(当 :ref:`VIDIOC_QUERYCAP` ioctl 返回的 struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置了 ``V4L2_CAP_STREAMING`` 标志时，输入和输出设备支持此 I/O 方法。是否支持特定的用户指针方法（不仅仅是内存映射），必须通过调用 :ref:`VIDIOC_REQBUFS` ioctl 并把内存类型设置为 ``V4L2_MEMORY_USERPTR`` 来确定。)

This I/O method combines advantages of the read/write and memory mapping
methods. Buffers (planes) are allocated by the application itself, and
can reside for example in virtual or shared memory. Only pointers to
data are exchanged, these pointers and meta-information are passed in
struct :c:type:`v4l2_buffer` (or in struct
:c:type:`v4l2_plane` in the multi-planar API case). The
driver must be switched into user pointer I/O mode by calling the
:ref:`VIDIOC_REQBUFS` with the desired buffer type.
No buffers (planes) are allocated beforehand, consequently they are not
indexed and cannot be queried like mapped buffers with the
:ref:`VIDIOC_QUERYBUF <VIDIOC_QUERYBUF>` ioctl.
(这种 I/O 方法结合了读/写和内存映射方法的优点。缓冲区（平面）由应用程序自己分配，可以驻留在例如虚拟内存或共享内存中。只交换指向数据的指针，这些指针和元信息通过 struct :c:type:`v4l2_buffer` 传递（在多平面 API 的情况下则在 struct :c:type:`v4l2_plane` 中传递）。必须通过以所需的缓冲区类型调用 :ref:`VIDIOC_REQBUFS` 把驱动程序切换到用户指针 I/O 模式。缓冲区（平面）不会预先分配，因此它们不被索引，也无法像映射缓冲区那样用 :ref:`VIDIOC_QUERYBUF <VIDIOC_QUERYBUF>` ioctl 查询。)

Example: Initiating streaming I/O with user pointers(示例：使用用户指针发起流式 I/O)
====================================================================================

.. code-block:: c

    struct v4l2_requestbuffers reqbuf;

    memset (&reqbuf, 0, sizeof (reqbuf));
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_USERPTR;

    if (ioctl (fd, VIDIOC_REQBUFS, &reqbuf) == -1) {
	if (errno == EINVAL)
	    printf ("Video capturing or user pointer streaming is not supported\\n");
	else
	    perror ("VIDIOC_REQBUFS");

	exit (EXIT_FAILURE);
    }

Buffer (plane) addresses and sizes are passed on the fly with the
:ref:`VIDIOC_QBUF <VIDIOC_QBUF>` ioctl. Although buffers are commonly
cycled, applications can pass different addresses and sizes at each
:ref:`VIDIOC_QBUF <VIDIOC_QBUF>` call. If required by the hardware the
driver swaps memory pages within physical memory to create a continuous
area of memory. This happens transparently to the application in the
virtual memory subsystem of the kernel. When buffer pages have been
swapped out to disk they are brought back and finally locked in physical
memory for DMA. [#f1]_
(缓冲区（平面）的地址和大小通过 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` ioctl 动态传递。虽然缓冲区通常是循环使用的，但应用程序可以在每次 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` 调用时传递不同的地址和大小。如果硬件需要，驱动程序会在物理内存中交换内存页以创建一块连续的内存区域。这在内核的虚拟内存子系统中对应用程序是透明的。当缓冲区页被换出到磁盘时，它们会被换回并最终锁定在物理内存中用于 DMA。)

Filled or displayed buffers are dequeued with the
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl. The driver can unlock the
memory pages at any time between the completion of the DMA and this
ioctl. The memory is also unlocked when
:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` is called,
:ref:`VIDIOC_REQBUFS`, or when the device is closed.
Applications must take care not to free buffers without dequeuing.
Firstly, the buffers remain locked for longer, wasting physical memory.
Secondly the driver will not be notified when the memory is returned to
the application's free list and subsequently reused for other purposes,
possibly completing the requested DMA and overwriting valuable data.
(已填充或已显示的缓冲区通过 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl 出队。驱动程序可以在 DMA 完成与此 ioctl 之间的任何时刻解锁内存页。当调用 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 、:ref:`VIDIOC_REQBUFS` 或设备关闭时，内存也会被解锁。应用程序必须注意不要在未出队的情况下释放缓冲区。首先，缓冲区会被锁定更长时间，浪费物理内存。其次，当内存被归还给应用程序的自由列表并随后被重新用于其他目的时，驱动程序不会收到通知，可能会完成所请求的 DMA 并覆盖有价值的数据。)

For capturing applications it is customary to enqueue a number of empty
buffers, to start capturing and enter the read loop. Here the
application waits until a filled buffer can be dequeued, and re-enqueues
the buffer when the data is no longer needed. Output applications fill
and enqueue buffers, when enough buffers are stacked up output is
started. In the write loop, when the application runs out of free
buffers it must wait until an empty buffer can be dequeued and reused.
Two methods exist to suspend execution of the application until one or
more buffers can be dequeued. By default :ref:`VIDIOC_DQBUF
<VIDIOC_QBUF>` blocks when no buffer is in the outgoing queue. When the
``O_NONBLOCK`` flag was given to the :c:func:`open()` function,
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` returns immediately with an ``EAGAIN``
error code when no buffer is available. The :ref:`select()
<func-select>` or :c:func:`poll()` function are always
available.
(对于捕获应用程序，习惯做法是把多个空缓冲区入队，然后开始捕获并进入读取循环。这里应用程序等待直到可以出队一个已填充的缓冲区，并在不再需要数据时重新把该缓冲区入队。输出应用程序填充并入队缓冲区，当足够多的缓冲区堆积后输出开始。在写入循环中，当应用程序用尽空闲缓冲区时，它必须等待直到可以出队一个空缓冲区并重新使用。有两种方法可以挂起应用程序的执行，直到一个或多个缓冲区可以出队。默认情况下，当输出队列中没有缓冲区时 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` 会阻塞。当给 :c:func:`open()` 函数传入 ``O_NONBLOCK`` 标志时，在没有缓冲区可用时 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` 会立即返回 ``EAGAIN`` 错误码。:ref:`select() <func-select>` 或 :c:func:`poll()` 函数总是可用。)

To start and stop capturing or output applications call the
:ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` and
:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctl.
(要开始和停止捕获或输出，应用程序调用 :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` 和 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctl。)

.. note::

   :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` removes all buffers from
   both queues and unlocks all buffers as a side effect. Since there is no
   notion of doing anything "now" on a multitasking system, if an
   application needs to synchronize with another event it should examine
   the struct :c:type:`v4l2_buffer` ``timestamp`` of captured or
   outputted buffers.
   (:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 作为副作用会从两个队列中移除所有缓冲区并解锁所有缓冲区。由于在多任务系统上不存在"立即"做任何事情的概念，如果应用程序需要与另一个事件同步，它应检查所捕获或输出的缓冲区的 struct :c:type:`v4l2_buffer` ``timestamp`` 。)

Drivers implementing user pointer I/O must support the
:ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>`, :ref:`VIDIOC_QBUF <VIDIOC_QBUF>`,
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>`, :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>`
and :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctls, the
:c:func:`select()` and :c:func:`poll()` function. [#f2]_
(实现用户指针 I/O 的驱动程序必须支持 :ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>` 、 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` 、 :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` 、 :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` 和 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` ioctl，以及 :c:func:`select()` 和 :c:func:`poll()` 函数。)

.. [#f1]
   We expect that frequently used buffers are typically not swapped out.
   Anyway, the process of swapping, locking or generating scatter-gather
   lists may be time consuming. The delay can be masked by the depth of
   the incoming buffer queue, and perhaps by maintaining caches assuming
   a buffer will be soon enqueued again. On the other hand, to optimize
   memory usage drivers can limit the number of buffers locked in
   advance and recycle the most recently used buffers first. Of course,
   the pages of empty buffers in the incoming queue need not be saved to
   disk. Output buffers must be saved on the incoming and outgoing queue
   because an application may share them with other processes.
   (我们预期经常使用的缓冲区通常不会被换出。无论如何，换页、锁定或生成分散-聚集列表的过程可能很耗时。这个延迟可以被传入缓冲区队列的深度所掩盖，或许还可以通过维护缓存来掩盖，即假定缓冲区很快会再次入队。另一方面，为了优化内存使用，驱动程序可以限制预先锁定的缓冲区数量，并优先回收最近使用的缓冲区。当然，传入队列中空缓冲区的页不需要保存到磁盘。输出缓冲区必须保存在传入和传出队列上，因为应用程序可能与其他进程共享它们。)

.. [#f2]
   At the driver level :c:func:`select()` and :c:func:`poll()` are
   the same, and :c:func:`select()` is too important to be optional.
   The rest should be evident.
   (在驱动程序层面，:c:func:`select()` 和 :c:func:`poll()` 是相同的，而 :c:func:`select()` 太重要了，不能是可选的。其余的应当显而易见。)
