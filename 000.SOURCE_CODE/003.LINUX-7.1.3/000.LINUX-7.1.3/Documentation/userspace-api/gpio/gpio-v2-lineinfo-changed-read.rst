.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_V2_LINEINFO_CHANGED_READ:

*****************************
GPIO_V2_LINEINFO_CHANGED_READ
*****************************

Name(名称)
==========

GPIO_V2_LINEINFO_CHANGED_READ - Read line info changed events for watched
lines from the chip.
(GPIO_V2_LINEINFO_CHANGED_READ - 从芯片读取被监视线路的线路信息变化事
件。)

Synopsis(概要)
==============

``int read(int chip_fd, void *buf, size_t count)``

Arguments(参数)
===============

``chip_fd``
    The file descriptor of the GPIO character device returned by `open()`.
    (由 `open()` 返回的 GPIO 字符设备文件描述符。)

``buf``
    The buffer to contain the :c:type:`events<gpio_v2_line_info_changed>`.
    (用于承载 :c:type:`events<gpio_v2_line_info_changed>` 的缓冲区。)

``count``
    The number of bytes available in ``buf``, which must be at least the size
    of a :c:type:`gpio_v2_line_info_changed` event.
    ( ``buf`` 中可用的字节数，必须至少为一个
    :c:type:`gpio_v2_line_info_changed` 事件的大小。)

Description(描述)
=================

Read line info changed events for watched lines from the chip.
(从芯片读取被监视线路的线路信息变化事件。)

.. note::
    Monitoring line info changes is not generally required, and would typically
    only be performed by a system monitoring component.
    (通常并不需要监视线路信息变化，这一般只由系统监控组件执行。)

    These events relate to changes in a line's request state or configuration,
    not its value. Use gpio-v2-line-event-read.rst to receive events when a
    line changes value.
    (这些事件与线路的请求状态或配置的变化有关，而与其值无关。当线路的
    值发生变化时，请使用 gpio-v2-line-event-read.rst 接收事件。)

A line must be watched using gpio-v2-get-lineinfo-watch-ioctl.rst to generate
info changed events.  Subsequently, a request, release, or reconfiguration
of the line will generate an info changed event.
(必须使用 gpio-v2-get-lineinfo-watch-ioctl.rst 监视一条线路才能产生信
息变化事件。随后，对该线路的请求、释放或重新配置都会产生一个信息变
化事件。)

The kernel timestamps events when they occur and stores them in a buffer
from where they can be read by userspace at its convenience using `read()`.
(内核在事件发生时为其加时间戳，并把它们存储在一个缓冲区中，用户空间
可以在方便的时候用 `read()` 读取。)

The size of the kernel event buffer is fixed at 32 events per ``chip_fd``.
(内核事件缓冲区的大小固定为每个 ``chip_fd`` 32 个事件。)

The buffer may overflow if bursts of events occur quicker than they are read
by userspace. If an overflow occurs then the most recent event is discarded.
Overflow cannot be detected from userspace.
(如果事件爆发式出现的速度快于用户空间读取的速度，缓冲区可能溢出。如
果发生溢出，则最新的事件会被丢弃。用户空间无法检测到溢出。)

Events read from the buffer are always in the same order that they were
detected by the kernel, including when multiple lines are being monitored by
the one ``chip_fd``.
(从缓冲区读出的事件始终与内核检测到它们的顺序相同，包括由同一个
``chip_fd`` 监视多条线路的情况。)

To minimize the number of calls required to copy events from the kernel to
userspace, `read()` supports copying multiple events. The number of events
copied is the lower of the number available in the kernel buffer and the
number that will fit in the userspace buffer (``buf``).
(为了尽量减少把事件从内核复制到用户空间所需的调用次数， `read()` 支
持一次复制多个事件。复制的事件数是内核缓冲区中可用数量与用户空间缓
冲区 ( ``buf`` ) 可容纳数量两者中较小的那个。)

A `read()` will block if no event is available and the ``chip_fd`` has not
been set **O_NONBLOCK**.
(如果没有事件可用且 ``chip_fd`` 未被设置为 **O_NONBLOCK** ， `read()`
会阻塞。)

The presence of an event can be tested for by checking that the ``chip_fd`` is
readable using `poll()` or an equivalent.
(可以通过用 `poll()` 或等价方法检查 ``chip_fd`` 是否可读，来测试是否
有事件存在。)

Return Value(返回值)
====================

On success the number of bytes read, which will be a multiple of the size
of a :c:type:`gpio_v2_line_info_changed` event.
(成功时返回读取的字节数，它将是一个
:c:type:`gpio_v2_line_info_changed` 事件大小的倍数。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1 并适当设置 ``errno`` 变量。常见错误码在 error-codes.rst
中描述。)
