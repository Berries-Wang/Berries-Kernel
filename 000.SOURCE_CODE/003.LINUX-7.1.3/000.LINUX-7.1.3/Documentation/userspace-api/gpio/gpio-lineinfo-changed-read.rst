.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_LINEINFO_CHANGED_READ:

************************************************
GPIO_LINEINFO_CHANGED_READ(读取线路信息变更事件)
************************************************

.. warning::
    This ioctl is part of chardev_v1.rst and is obsoleted by
    gpio-v2-lineinfo-changed-read.rst.
    (此 ioctl 属于 chardev_v1.rst，已被 gpio-v2-lineinfo-changed-read.rst
    废弃取代。)

Name(名称)
==========

GPIO_LINEINFO_CHANGED_READ - Read line info change events for watched lines
from the chip.
(GPIO_LINEINFO_CHANGED_READ - 从芯片读取被监视线路的线路信息变更
事件。)

Synopsis(概要)
==============

``int read(int chip_fd, void *buf, size_t count)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (GPIO 字符设备的文件描述符，由 open() 返回。)

``buf``
    The buffer to contain the :c:type:`events<gpioline_info_changed>`.
    (用于存放 :c:type:`events<gpioline_info_changed>` 的缓冲区。)

``count``
    The number of bytes available in ``buf``, which must be at least the size
    of a :c:type:`gpioline_info_changed` event.
    (``buf`` 中可用的字节数，必须至少为
    :c:type:`gpioline_info_changed` 事件的大小。)

Description(描述)
=================

Read line info change events for watched lines from the chip.
(从芯片读取被监视线路的线路信息变更事件。)

.. note::
    Monitoring line info changes is not generally required, and would typically
    only be performed by a system monitoring component.

    These events relate to changes in a line's request state or configuration,
    not its value. Use gpio-lineevent-data-read.rst to receive events when a
    line changes value.
    (通常不需要监控线路信息变更，一般只有系统监控组件才会这样做。

    这些事件与线路的请求状态或配置的变化有关，而不是其值。要在线路
    值发生变化时接收事件，请使用 gpio-lineevent-data-read.rst。)

A line must be watched using gpio-get-lineinfo-watch-ioctl.rst to generate
info changed events.  Subsequently, a request, release, or reconfiguration
of the line will generate an info changed event.
(必须使用 gpio-get-lineinfo-watch-ioctl.rst 监视一条线路才能生成
info changed 事件。随后，对线路的请求、释放或重新配置都会生成一个
info changed 事件。)

The kernel timestamps events when they occur and stores them in a buffer
from where they can be read by userspace at its convenience using `read()`.
(内核在事件发生时为其加上时间戳，并把它们存储在一个缓冲区中，用户
空间可以在方便的时候用 read() 从中读取。)

The size of the kernel event buffer is fixed at 32 events per ``chip_fd``.
(内核事件缓冲区的大小固定为每个 ``chip_fd`` 32 个事件。)

The buffer may overflow if bursts of events occur quicker than they are read
by userspace. If an overflow occurs then the most recent event is discarded.
Overflow cannot be detected from userspace.
(如果事件突发产生的速度快于用户空间读取的速度，缓冲区可能溢出。
如果发生溢出，则最新的事件会被丢弃。用户空间无法检测到溢出。)

Events read from the buffer are always in the same order that they were
detected by the kernel, including when multiple lines are being monitored by
the one ``chip_fd``.
(从缓冲区读取的事件始终与内核检测到它们的顺序相同，包括由同一个
``chip_fd`` 监控多条线路的情况。)

To minimize the number of calls required to copy events from the kernel to
userspace, `read()` supports copying multiple events. The number of events
copied is the lower of the number available in the kernel buffer and the
number that will fit in the userspace buffer (``buf``).
(为了把事件从内核复制到用户空间所需的调用次数降到最少，read() 支持
复制多个事件。复制的事件数是内核缓冲区中的可用数量与能放入用户空间
缓冲区(``buf``)的数量两者中较小的那个。)

A `read()` will block if no event is available and the ``chip_fd`` has not
been set **O_NONBLOCK**.
(如果没有事件可用且 ``chip_fd`` 尚未设置 **O_NONBLOCK** ，read()
将阻塞。)

The presence of an event can be tested for by checking that the ``chip_fd`` is
readable using `poll()` or an equivalent.
(可以用 poll() 或等效方法检查 ``chip_fd`` 是否可读，以测试是否有
事件。)

First added in 5.7.
(首次添加于 5.7。)

Return Value(返回值)
====================

On success the number of bytes read, which will be a multiple of the size of
a :c:type:`gpioline_info_changed` event.
(成功时返回读取的字节数，它是 :c:type:`gpioline_info_changed` 事件
大小的倍数。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1 并适当设置 ``errno`` 变量。常见错误码在
error-codes.rst 中描述。)
