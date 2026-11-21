.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_STREAMON:

***********************************************************
ioctl VIDIOC_STREAMON, VIDIOC_STREAMOFF(启动或停止流式 I/O)
***********************************************************

Name(名称)
==========

VIDIOC_STREAMON - VIDIOC_STREAMOFF - Start or stop streaming I/O(VIDIOC_STREAMON - VIDIOC_STREAMOFF - 启动或停止流式 I/O)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_STREAMON

``int ioctl(int fd, VIDIOC_STREAMON, const int *argp)``

.. c:macro:: VIDIOC_STREAMOFF

``int ioctl(int fd, VIDIOC_STREAMOFF, const int *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to an integer.
    (指向一个整数的指针。)

Description(描述)
=================

The ``VIDIOC_STREAMON`` and ``VIDIOC_STREAMOFF`` ioctl start and stop
the capture or output process during streaming
(:ref:`memory mapping <mmap>`, :ref:`user pointer <userp>` or
:ref:`DMABUF <dmabuf>`) I/O.
(``VIDIOC_STREAMON`` 和 ``VIDIOC_STREAMOFF`` ioctl 在流式(:ref:`memory
mapping <mmap>`、:ref:`user pointer <userp>` 或 :ref:`DMABUF <dmabuf>`)
I/O 期间启动和停止捕获或输出过程。)

Capture hardware is disabled and no input buffers are filled (if there
are any empty buffers in the incoming queue) until ``VIDIOC_STREAMON``
has been called. Output hardware is disabled and no video signal is
produced until ``VIDIOC_STREAMON`` has been called.
(在调用 ``VIDIOC_STREAMON`` 之前，捕获硬件是禁用的，并且不会填充任何
输入缓冲区(如果传入队列中有空缓冲区)。在调用 ``VIDIOC_STREAMON``
之前，输出硬件是禁用的，不会产生视频信号。)

Memory-to-memory devices will not start until ``VIDIOC_STREAMON`` has
been called for both the capture and output stream types.
(对于内存到内存设备，直到对捕获和输出两种流类型都调用了
``VIDIOC_STREAMON`` 之后才会开始工作。)

If ``VIDIOC_STREAMON`` fails then any already queued buffers will remain
queued.
(如果 ``VIDIOC_STREAMON`` 失败，任何已入队的缓冲区将保持入队状态。)

The ``VIDIOC_STREAMOFF`` ioctl, apart of aborting or finishing any DMA
in progress, unlocks any user pointer buffers locked in physical memory,
and it removes all buffers from the incoming and outgoing queues. That
means all images captured but not dequeued yet will be lost, likewise
all images enqueued for output but not transmitted yet. I/O returns to
the same state as after calling
:ref:`VIDIOC_REQBUFS` and can be restarted
accordingly.
(``VIDIOC_STREAMOFF`` ioctl 除了中止或完成正在进行的任何 DMA 之外，
还会解锁所有锁定在物理内存中的用户指针缓冲区，并把所有缓冲区从传入
和传出队列中移除。这意味着所有已捕获但尚未出队的图像都会丢失，同样，
所有已为输出入队但尚未发送的图像也会丢失。I/O 返回到与调用
:ref:`VIDIOC_REQBUFS` 之后相同的状态，并可以相应地重新开始。)

If buffers have been queued with :ref:`VIDIOC_QBUF` and
``VIDIOC_STREAMOFF`` is called without ever having called
``VIDIOC_STREAMON``, then those queued buffers will also be removed from
the incoming queue and all are returned to the same state as after
calling :ref:`VIDIOC_REQBUFS` and can be restarted
accordingly.
(如果缓冲区已用 :ref:`VIDIOC_QBUF` 入队，而在从未调用
``VIDIOC_STREAMON`` 的情况下调用了 ``VIDIOC_STREAMOFF``，那么这些已
入队的缓冲区也会从传入队列中移除，全部返回到与调用
:ref:`VIDIOC_REQBUFS` 之后相同的状态，并可以相应地重新开始。)

Both ioctls take a pointer to an integer, the desired buffer or stream
type. This is the same as struct
:c:type:`v4l2_requestbuffers` ``type``.
(两个 ioctl 都接受一个指向整数的指针，即所需的缓冲区或流类型。它与
struct :c:type:`v4l2_requestbuffers` 的 ``type`` 相同。)

If ``VIDIOC_STREAMON`` is called when streaming is already in progress,
or if ``VIDIOC_STREAMOFF`` is called when streaming is already stopped,
then 0 is returned. Nothing happens in the case of ``VIDIOC_STREAMON``,
but ``VIDIOC_STREAMOFF`` will return queued buffers to their starting
state as mentioned above.
(如果在流已经在进行中时调用 ``VIDIOC_STREAMON``，或在流已经停止时
调用 ``VIDIOC_STREAMOFF``，则返回 0。对于 ``VIDIOC_STREAMON`` 不会有
任何事情发生，但 ``VIDIOC_STREAMOFF`` 会如上所述把已入队的缓冲区恢复
到其起始状态。)

.. note::

   Applications can be preempted for unknown periods right before
   or after the ``VIDIOC_STREAMON`` or ``VIDIOC_STREAMOFF`` calls, there is
   no notion of starting or stopping "now". Buffer timestamps can be used
   to synchronize with other events.
   (应用程序可能会在 ``VIDIOC_STREAMON`` 或 ``VIDIOC_STREAMOFF`` 调用
   之前或之后的未知时段内被抢占，不存在"立即"启动或停止的概念。
   可以使用缓冲区时间戳来与其他事件同步。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The buffer ``type`` is not supported, or no buffers have been
    allocated (memory mapping) or enqueued (output) yet.
    (缓冲区 ``type`` 不受支持，或者尚未分配(内存映射)或入队(输出)
    任何缓冲区。)

EPIPE
    The driver implements
    :ref:`pad-level format configuration <pad-level-formats>` and the
    pipeline configuration is invalid.
    (驱动程序实现了 :ref:`pad-level format configuration
    <pad-level-formats>`，而流水线配置无效。)

ENOLINK
    The driver implements Media Controller interface and the pipeline
    link configuration is invalid.
    (驱动程序实现了 Media Controller 接口，而流水线链路配置无效。)
