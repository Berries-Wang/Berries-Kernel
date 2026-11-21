.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_QBUF:

*************************
ioctl DMX_QBUF, DMX_DQBUF
*************************

Name(名称)
==========

DMX_QBUF - DMX_DQBUF - Exchange a buffer with the driver
(DMX_QBUF - DMX_DQBUF - 与驱动程序交换一个缓冲区。)

.. warning:: this API is still experimental
             (警告：此 API 仍处于实验阶段。)

Synopsis(概要)
==============

.. c:macro:: DMX_QBUF

``int ioctl(int fd, DMX_QBUF, struct dmx_buffer *argp)``

.. c:macro:: DMX_DQBUF

``int ioctl(int fd, DMX_DQBUF, struct dmx_buffer *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`dmx_buffer`.
    (指向 struct :c:type:`dmx_buffer` 的指针。)

Description(描述)
=================

Applications call the ``DMX_QBUF`` ioctl to enqueue an empty
(capturing) or filled (output) buffer in the driver's incoming queue.
The semantics depend on the selected I/O method.
(应用程序调用 ``DMX_QBUF`` ioctl 把一个空的 (捕获) 或填满的 (输出) 缓冲
区放入驱动的传入队列。其语义取决于所选的 I/O 方法。)

To enqueue a buffer applications set the ``index`` field. Valid index
numbers range from zero to the number of buffers allocated with
:ref:`DMX_REQBUFS` (struct :c:type:`dmx_requestbuffers` ``count``) minus
one. The contents of the struct :c:type:`dmx_buffer` returned
by a :ref:`DMX_QUERYBUF` ioctl will do as well.
(要入队一个缓冲区，应用程序设置 ``index`` 字段。有效索引号的范围是从
0 到用 :ref:`DMX_REQBUFS` 分配的缓冲区数量 (struct
:c:type:`dmx_requestbuffers` 的 ``count``) 减一。由 :ref:`DMX_QUERYBUF`
ioctl 返回的 struct :c:type:`dmx_buffer` 的内容也同样适用。)

When ``DMX_QBUF`` is called with a pointer to this structure, it locks the
memory pages of the buffer in physical memory, so they cannot be swapped
out to disk. Buffers remain locked until dequeued, until the
device is closed.
(当 ``DMX_QBUF`` 以指向该结构的指针被调用时，它会把缓冲区的内存页锁定
在物理内存中，使其不能被换出到磁盘。缓冲区保持锁定，直到被出队或设备
被关闭。)

Applications call the ``DMX_DQBUF`` ioctl to dequeue a filled
(capturing) buffer from the driver's outgoing queue.
They just set the ``index`` field with the buffer ID to be queued.
When ``DMX_DQBUF`` is called with a pointer to struct :c:type:`dmx_buffer`,
the driver fills the remaining fields or returns an error code.
(应用程序调用 ``DMX_DQBUF`` ioctl 从驱动的传出队列中出队一个填满的
(捕获) 缓冲区。它们只需把要排队的缓冲区 ID 设置到 ``index`` 字段。当
``DMX_DQBUF`` 以指向 struct :c:type:`dmx_buffer` 的指针被调用时，驱动
程序会填充其余字段或返回一个错误码。)

By default ``DMX_DQBUF`` blocks when no buffer is in the outgoing
queue. When the ``O_NONBLOCK`` flag was given to the
:c:func:`open()` function, ``DMX_DQBUF`` returns
immediately with an ``EAGAIN`` error code when no buffer is available.
(默认情况下，当传出队列中没有缓冲区时，``DMX_DQBUF`` 会阻塞。如果向
:c:func:`open()` 函数传入了 ``O_NONBLOCK`` 标志，那么当没有可用缓冲区
时，``DMX_DQBUF`` 会立即以 ``EAGAIN`` 错误码返回。)

The struct :c:type:`dmx_buffer` structure is specified in
:ref:`buffer`.
(struct :c:type:`dmx_buffer` 结构在 :ref:`buffer` 中说明。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EAGAIN
    Non-blocking I/O has been selected using ``O_NONBLOCK`` and no
    buffer was in the outgoing queue.
    (已通过 ``O_NONBLOCK`` 选择非阻塞 I/O，但传出队列中没有缓冲区。)

EINVAL
    The ``index`` is out of bounds, or no buffers have been allocated yet.
    ( ``index`` 越界，或者尚未分配任何缓冲区。)

EIO
    ``DMX_DQBUF`` failed due to an internal error. Can also indicate
    temporary problems like signal loss or CRC errors.
    ( ``DMX_DQBUF`` 因内部错误而失败。也可能表示诸如信号丢失或 CRC
    错误之类的临时性问题。)
