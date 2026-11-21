.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _func-write:

************
V4L2 write()
************

Name(名称)
==========

v4l2-write - Write to a V4L2 device
(v4l2-write - 写入一个 V4L2 设备。)

Synopsis(概要)
==============

.. code-block:: c

    #include <unistd.h>

.. c:function:: ssize_t write( int fd, void *buf, size_t count )

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``buf``
     Buffer with data to be written
     (包含待写入数据的缓冲区。)

``count``
    Number of bytes at the buffer
    (缓冲区中的字节数。)

Description(描述)
=================

:c:func:`write()` writes up to ``count`` bytes to the device
referenced by the file descriptor ``fd`` from the buffer starting at
``buf``. When the hardware outputs are not active yet, this function
enables them. When ``count`` is zero, :c:func:`write()` returns 0
without any other effect.
( :c:func:`write()` 把最多 ``count`` 字节的数据，从 ``buf`` 起始的缓冲
区写入文件描述符 ``fd`` 所引用的设备。当硬件输出尚未激活时，该函数会
启用它们。当 ``count`` 为零时， :c:func:`write()` 返回 0 且没有其他影
响。)

When the application does not provide more data in time, the previous
video frame, raw VBI image, sliced VPS or WSS data is displayed again.
Sliced Teletext or Closed Caption data is not repeated, the driver
inserts a blank line instead.
(当应用程序没有及时提供更多数据时，将再次显示上一个视频帧、raw VBI
图像、sliced VPS 或 WSS 数据。sliced Teletext 或 Closed Caption 数据
不会重复，驱动程序会插入一个空白行代替。)

Return Value(返回值)
====================

On success, the number of bytes written are returned. Zero indicates
nothing was written. On error, -1 is returned, and the ``errno``
variable is set appropriately. In this case the next write will start at
the beginning of a new frame. Possible error codes are:
(成功时返回写入的字节数。零表示没有写入任何内容。出错时返回 -1，并适
当设置 ``errno`` 变量。在这种情况下，下一次写入将从一个新帧的开头开
始。可能的错误码：)

EAGAIN
    Non-blocking I/O has been selected using the
    :ref:`O_NONBLOCK <func-open>` flag and no buffer space was
    available to write the data immediately.
    (已通过 :ref:`O_NONBLOCK <func-open>` 标志选择了非阻塞 I/O，但没有
    可用来立即写入数据的缓冲区空间。)

EBADF
    ``fd`` is not a valid file descriptor or is not open for writing.
    ( ``fd`` 不是有效的文件描述符，或未以写方式打开。)

EBUSY
    The driver does not support multiple write streams and the device is
    already in use.
    (驱动程序不支持多个写流，且设备已在使用中。)

EFAULT
    ``buf`` references an inaccessible memory area.
    ( ``buf`` 引用了不可访问的内存区域。)

EINTR
    The call was interrupted by a signal before any data was written.
    (在写入任何数据之前，调用被信号中断。)

EIO
    I/O error. This indicates some hardware problem.
    (I/O 错误。这表示存在某种硬件问题。)

EINVAL
    The :c:func:`write()` function is not supported by this driver,
    not on this device, or generally not on this type of device.
    (此驱动程序不支持 :c:func:`write()` 函数——在该设备上不支持，或一
    般而言在此类设备上都不支持。)
