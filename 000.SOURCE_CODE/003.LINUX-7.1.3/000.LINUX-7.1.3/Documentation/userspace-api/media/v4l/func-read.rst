.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _func-read:

***********
V4L2 read()
***********

Name(名称)
==========

v4l2-read - Read from a V4L2 device
(v4l2-read - 从 V4L2 设备读取。)

Synopsis(概要)
==============

.. code-block:: c

    #include <unistd.h>

.. c:function:: ssize_t read( int fd, void *buf, size_t count )

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``buf``
   Buffer to be filled
   (待填充的缓冲区)

``count``
  Max number of bytes to read
  (要读取的最大字节数)

Description(描述)
=================

:c:func:`read()` attempts to read up to ``count`` bytes from file
descriptor ``fd`` into the buffer starting at ``buf``. The layout of the
data in the buffer is discussed in the respective device interface
section, see ##. If ``count`` is zero, :c:func:`read()` returns zero
and has no other results. If ``count`` is greater than ``SSIZE_MAX``,
the result is unspecified. Regardless of the ``count`` value each
:c:func:`read()` call will provide at most one frame (two fields)
worth of data.
(:c:func:`read()` 尝试从文件描述符 ``fd`` 读取最多 ``count`` 字节到从 ``buf`` 开始的缓冲区中。缓冲区中数据的布局在相应的设备接口一节中讨论，参见 ##。如果 ``count`` 为零，:c:func:`read()` 返回零且没有其他结果。如果 ``count`` 大于 ``SSIZE_MAX`` ，则结果未定义。无论 ``count`` 的值如何，每次 :c:func:`read()` 调用最多提供一帧（两场）的数据量。)

By default :c:func:`read()` blocks until data becomes available. When
the ``O_NONBLOCK`` flag was given to the :c:func:`open()`
function it returns immediately with an ``EAGAIN`` error code when no data
is available. The :c:func:`select()` or
:c:func:`poll()` functions can always be used to suspend
execution until data becomes available. All drivers supporting the
:c:func:`read()` function must also support :c:func:`select()` and
:c:func:`poll()`.
(默认情况下，:c:func:`read()` 会阻塞直到有数据可用。如果在调用 :c:func:`open()` 函数时给出了 ``O_NONBLOCK`` 标志，则当没有数据可用时它会立即返回 ``EAGAIN`` 错误码。:c:func:`select()` 或 :c:func:`poll()` 函数总是可以用来挂起执行，直到有数据可用。所有支持 :c:func:`read()` 函数的驱动程序也必须支持 :c:func:`select()` 和 :c:func:`poll()` 。)

Drivers can implement read functionality in different ways, using a
single or multiple buffers and discarding the oldest or newest frames
once the internal buffers are filled.
(驱动程序可以用不同的方式实现读取功能，使用单个或多个缓冲区，并在内部缓冲区填满后丢弃最旧或最新的帧。)

:c:func:`read()` never returns a "snapshot" of a buffer being filled.
Using a single buffer the driver will stop capturing when the
application starts reading the buffer until the read is finished. Thus
only the period of the vertical blanking interval is available for
reading, or the capture rate must fall below the nominal frame rate of
the video standard.
(:c:func:`read()` 绝不会返回正在填充中的缓冲区的"快照"。使用单个缓冲区时，当应用程序开始读取缓冲区后，驱动程序会停止捕获，直到读取完成。因此只有垂直消隐间隔的时段可用于读取，或者捕获速率必须降到视频标准标称帧率以下。)

The behavior of :c:func:`read()` when called during the active picture
period or the vertical blanking separating the top and bottom field
depends on the discarding policy. A driver discarding the oldest frames
keeps capturing into an internal buffer, continuously overwriting the
previously, not read frame, and returns the frame being received at the
time of the :c:func:`read()` call as soon as it is complete.
(在活动图像期间或在分隔顶场与底场的垂直消隐期间调用 :c:func:`read()` 的行为取决于丢弃策略。丢弃最旧帧的驱动程序会持续捕获到内部缓冲区中，不断覆盖之前未读取的帧，并在 :c:func:`read()` 调用时正在接收的帧完成后立即将其返回。)

A driver discarding the newest frames stops capturing until the next
:c:func:`read()` call. The frame being received at :c:func:`read()`
time is discarded, returning the following frame instead. Again this
implies a reduction of the capture rate to one half or less of the
nominal frame rate. An example of this model is the video read mode of
the bttv driver, initiating a DMA to user memory when :c:func:`read()`
is called and returning when the DMA finished.
(丢弃最新帧的驱动程序会停止捕获，直到下一次 :c:func:`read()` 调用。在 :c:func:`read()` 调用时正在接收的帧被丢弃，改为返回下一帧。这同样意味着捕获速率降低到标称帧率的一半或更低。这种模型的一个例子是 bttv 驱动的视频读取模式：在调用 :c:func:`read()` 时向用户内存发起 DMA，并在 DMA 完成时返回。)

In the multiple buffer model drivers maintain a ring of internal
buffers, automatically advancing to the next free buffer. This allows
continuous capturing when the application can empty the buffers fast
enough. Again, the behavior when the driver runs out of free buffers
depends on the discarding policy.
(在多缓冲区模型中，驱动程序维护一个内部缓冲区环，自动前进到下一个空闲缓冲区。这使得应用程序在能够足够快地清空缓冲区时可以实现连续捕获。同样，当驱动程序的空闲缓冲区用尽时的行为取决于丢弃策略。)

Applications can get and set the number of buffers used internally by
the driver with the :ref:`VIDIOC_G_PARM <VIDIOC_G_PARM>` and
:ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctls. They are optional,
however. The discarding policy is not reported and cannot be changed.
For minimum requirements see :ref:`devices`.
(应用程序可以使用 :ref:`VIDIOC_G_PARM <VIDIOC_G_PARM>` 和 :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctl 获取和设置驱动程序内部使用的缓冲区数量。不过它们是可选的。丢弃策略不会被报告，也无法更改。最低要求参见 :ref:`devices` 。)

Return Value(返回值)
====================

On success, the number of bytes read is returned. It is not an error if
this number is smaller than the number of bytes requested, or the amount
of data required for one frame. This may happen for example because
:c:func:`read()` was interrupted by a signal. On error, -1 is
returned, and the ``errno`` variable is set appropriately. In this case
the next read will start at the beginning of a new frame. Possible error
codes are:
(成功时返回读取的字节数。如果该数字小于请求的字节数或一帧所需的数据量，并不算错误。例如这可能是因为 :c:func:`read()` 被信号中断。出错时返回 -1，并适当设置 ``errno`` 变量。在这种情况下，下一次读取将从新帧的开头开始。可能的错误码有：)

EAGAIN
    Non-blocking I/O has been selected using O_NONBLOCK and no data was
    immediately available for reading.
    (已通过 O_NONBLOCK 选择了非阻塞 I/O，但没有可立即读取的数据。)

EBADF
    ``fd`` is not a valid file descriptor or is not open for reading, or
    the process already has the maximum number of files open.
    (``fd`` 不是有效的文件描述符，或未以读方式打开，或该进程已打开的文件数已达上限。)

EBUSY
    The driver does not support multiple read streams and the device is
    already in use.
    (驱动程序不支持多个读取流，且设备已被占用。)

EFAULT
    ``buf`` references an inaccessible memory area.
    (``buf`` 引用了不可访问的内存区域。)

EINTR
    The call was interrupted by a signal before any data was read.
    (在读取任何数据之前，调用被信号中断。)

EIO
    I/O error. This indicates some hardware problem or a failure to
    communicate with a remote device (USB camera etc.).
    (I/O 错误。这表示某种硬件问题，或与远程设备（USB 摄像头等）通信失败。)

EINVAL
    The :c:func:`read()` function is not supported by this driver, not
    on this device, or generally not on this type of device.
    (此驱动程序不支持 :c:func:`read()` 函数——不在此设备上支持，或通常不在此类设备上支持。)
