.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_LINEEVENT_DATA_READ:

******************************************
GPIO_LINEEVENT_DATA_READ(读取边沿检测事件)
******************************************

.. warning::
    This ioctl is part of chardev_v1.rst and is obsoleted by
    gpio-v2-line-event-read.rst.
    (此 ioctl 属于 chardev_v1.rst，已被 gpio-v2-line-event-read.rst
    废弃取代。)

Name(名称)
==========

GPIO_LINEEVENT_DATA_READ - Read edge detection events from a line event.
(GPIO_LINEEVENT_DATA_READ - 从行事件(line event)读取边沿检测事件。)

Synopsis(概要)
==============

``int read(int event_fd, void *buf, size_t count)``

Arguments(参数)
===============

``event_fd``
    The file descriptor of the GPIO character device, as returned in the
    :c:type:`request.fd<gpioevent_request>` by gpio-get-lineevent-ioctl.rst.
    (GPIO 字符设备的文件描述符，由 gpio-get-lineevent-ioctl.rst 在
    :c:type:`request.fd<gpioevent_request>` 中返回。)

``buf``
    The buffer to contain the :c:type:`events<gpioevent_data>`.
    (用于存放 :c:type:`events<gpioevent_data>` 的缓冲区。)

``count``
    The number of bytes available in ``buf``, which must be at
    least the size of a :c:type:`gpioevent_data`.
    (``buf`` 中可用的字节数，必须至少为 :c:type:`gpioevent_data` 的
    大小。)

Description(描述)
=================

Read edge detection events for a line from a line event.
(从一个行事件读取一条线路的边沿检测事件。)

Edge detection must be enabled for the input line using either
``GPIOEVENT_REQUEST_RISING_EDGE`` or ``GPIOEVENT_REQUEST_FALLING_EDGE``, or
both. Edge events are then generated whenever edge interrupts are detected on
the input line.
(必须使用 ``GPIOEVENT_REQUEST_RISING_EDGE`` 或
``GPIOEVENT_REQUEST_FALLING_EDGE``，或两者，为输入线路启用边沿检测。
之后，只要在输入线路上检测到边沿中断，就会生成边沿事件。)

Edges are defined in terms of changes to the logical line value, so an inactive
to active transition is a rising edge.  If ``GPIOHANDLE_REQUEST_ACTIVE_LOW`` is
set then logical polarity is the opposite of physical polarity, and
``GPIOEVENT_REQUEST_RISING_EDGE`` then corresponds to a falling physical edge.
(边沿是按逻辑线路值的变化定义的，因此从非活动到活动的跳变是上升沿。
如果设置了 ``GPIOHANDLE_REQUEST_ACTIVE_LOW``，则逻辑极性与物理极性
相反，``GPIOEVENT_REQUEST_RISING_EDGE`` 对应的将是物理下降沿。)

The kernel captures and timestamps edge events as close as possible to their
occurrence and stores them in a buffer from where they can be read by
userspace at its convenience using `read()`.
(内核在尽可能接近事件发生的时刻捕获边沿事件并加上时间戳，把它们存储
在一个缓冲区中，用户空间可以在方便的时候用 read() 从中读取。)

The source of the clock for :c:type:`event.timestamp<gpioevent_data>` is
``CLOCK_MONOTONIC``, except for kernels earlier than Linux 5.7 when it was
``CLOCK_REALTIME``.  There is no indication in the :c:type:`gpioevent_data`
as to which clock source is used, it must be determined from either the kernel
version or sanity checks on the timestamp itself.
(:c:type:`event.timestamp<gpioevent_data>` 的时钟源是
``CLOCK_MONOTONIC``，Linux 5.7 之前的内核除外，那时是
``CLOCK_REALTIME``。:c:type:`gpioevent_data` 中没有指明使用了哪个
时钟源，必须从内核版本或对时间戳本身的合理性检查来确定。)

Events read from the buffer are always in the same order that they were
detected by the kernel.
(从缓冲区读取的事件始终与内核检测到它们的顺序相同。)

The size of the kernel event buffer is fixed at 16 events.
(内核事件缓冲区的大小固定为 16 个事件。)

The buffer may overflow if bursts of events occur quicker than they are read
by userspace. If an overflow occurs then the most recent event is discarded.
Overflow cannot be detected from userspace.
(如果事件突发产生的速度快于用户空间读取的速度，缓冲区可能溢出。
如果发生溢出，则最新的事件会被丢弃。用户空间无法检测到溢出。)

To minimize the number of calls required to copy events from the kernel to
userspace, `read()` supports copying multiple events. The number of events
copied is the lower of the number available in the kernel buffer and the
number that will fit in the userspace buffer (``buf``).
(为了把事件从内核复制到用户空间所需的调用次数降到最少，read() 支持
复制多个事件。复制的事件数是内核缓冲区中的可用数量与能放入用户空间
缓冲区(``buf``)的数量两者中较小的那个。)

The `read()` will block if no event is available and the ``event_fd`` has not
been set **O_NONBLOCK**.
(如果没有事件可用且 ``event_fd`` 尚未设置 **O_NONBLOCK** ，read()
将阻塞。)

The presence of an event can be tested for by checking that the ``event_fd`` is
readable using `poll()` or an equivalent.
(可以用 poll() 或等效方法检查 ``event_fd`` 是否可读，以测试是否有
事件。)

Return Value(返回值)
====================

On success the number of bytes read, which will be a multiple of the size of
a :c:type:`gpio_lineevent_data` event.
(成功时返回读取的字节数，它是 :c:type:`gpio_lineevent_data` 事件
大小的倍数。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1 并适当设置 ``errno`` 变量。常见错误码在
error-codes.rst 中描述。)
