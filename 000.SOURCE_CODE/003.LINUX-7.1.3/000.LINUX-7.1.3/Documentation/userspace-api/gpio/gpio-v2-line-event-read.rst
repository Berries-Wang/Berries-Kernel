.. SPDX-License-Identifier: GPL-2.0

.. _GPIO_V2_LINE_EVENT_READ:

*****************************************
GPIO_V2_LINE_EVENT_READ(读取边沿检测事件)
*****************************************

Name(名称)
==========

GPIO_V2_LINE_EVENT_READ - Read edge detection events for lines from a request.
(GPIO_V2_LINE_EVENT_READ - 从请求(request)读取线路的边沿检测事件。)

Synopsis(概要)
==============

``int read(int req_fd, void *buf, size_t count)``

Arguments(参数)
===============

``req_fd``
    The file descriptor of the GPIO character device, as returned in the
    :c:type:`request.fd<gpio_v2_line_request>` by gpio-v2-get-line-ioctl.rst.
    (GPIO 字符设备的文件描述符，由 gpio-v2-get-line-ioctl.rst 在
    :c:type:`request.fd<gpio_v2_line_request>` 中返回。)

``buf``
    The buffer to contain the :c:type:`events<gpio_v2_line_event>`.
    (用于存放 :c:type:`events<gpio_v2_line_event>` 的缓冲区。)

``count``
    The number of bytes available in ``buf``, which must be at
    least the size of a :c:type:`gpio_v2_line_event`.
    (``buf`` 中可用的字节数，必须至少为 :c:type:`gpio_v2_line_event`
    的大小。)

Description(描述)
=================

Read edge detection events for lines from a request.
(从一个请求读取线路的边沿检测事件。)

Edge detection must be enabled for the input line using either
``GPIO_V2_LINE_FLAG_EDGE_RISING`` or ``GPIO_V2_LINE_FLAG_EDGE_FALLING``, or
both. Edge events are then generated whenever edge interrupts are detected on
the input line.
(必须使用 ``GPIO_V2_LINE_FLAG_EDGE_RISING`` 或
``GPIO_V2_LINE_FLAG_EDGE_FALLING``，或两者，为输入线路启用边沿检测。
之后，只要在输入线路上检测到边沿中断，就会生成边沿事件。)

Edges are defined in terms of changes to the logical line value, so an inactive
to active transition is a rising edge.  If ``GPIO_V2_LINE_FLAG_ACTIVE_LOW`` is
set then logical polarity is the opposite of physical polarity, and
``GPIO_V2_LINE_FLAG_EDGE_RISING`` then corresponds to a falling physical edge.
(边沿是按逻辑线路值的变化定义的，因此从非活动到活动的跳变是上升沿。
如果设置了 ``GPIO_V2_LINE_FLAG_ACTIVE_LOW``，则逻辑极性与物理极性
相反，``GPIO_V2_LINE_FLAG_EDGE_RISING`` 对应的将是物理下降沿。)

The kernel captures and timestamps edge events as close as possible to their
occurrence and stores them in a buffer from where they can be read by
userspace at its convenience using `read()`.
(内核在尽可能接近事件发生的时刻捕获边沿事件并加上时间戳，把它们存储
在一个缓冲区中，用户空间可以在方便的时候用 read() 从中读取。)

Events read from the buffer are always in the same order that they were
detected by the kernel, including when multiple lines are being monitored by
the one request.
(从缓冲区读取的事件始终与内核检测到它们的顺序相同，包括由同一个请求
监控多条线路的情况。)

The size of the kernel event buffer is fixed at the time of line request
creation, and can be influenced by the
:c:type:`request.event_buffer_size<gpio_v2_line_request>`.
The default size is 16 times the number of lines requested.
(内核事件缓冲区的大小在线路请求创建时固定下来，并可能受
:c:type:`request.event_buffer_size<gpio_v2_line_request>` 的影响。
默认大小是请求线路数的 16 倍。)

The buffer may overflow if bursts of events occur quicker than they are read
by userspace. If an overflow occurs then the oldest buffered event is
discarded. Overflow can be detected from userspace by monitoring the event
sequence numbers.
(如果事件突发产生的速度快于用户空间读取的速度，缓冲区可能溢出。
如果发生溢出，缓冲中最旧的事件会被丢弃。用户空间可以通过监控事件
序列号来检测溢出。)

To minimize the number of calls required to copy events from the kernel to
userspace, `read()` supports copying multiple events. The number of events
copied is the lower of the number available in the kernel buffer and the
number that will fit in the userspace buffer (``buf``).
(为了把事件从内核复制到用户空间所需的调用次数降到最少，read() 支持
复制多个事件。复制的事件数是内核缓冲区中的可用数量与能放入用户空间
缓冲区(``buf``)的数量两者中较小的那个。)

Changing the edge detection flags using gpio-v2-line-set-config-ioctl.rst
does not remove or modify the events already contained in the kernel event
buffer.
(使用 gpio-v2-line-set-config-ioctl.rst 更改边沿检测标志并不会移除或
修改内核事件缓冲区中已有的事件。)

The `read()` will block if no event is available and the ``req_fd`` has not
been set **O_NONBLOCK**.
(如果没有事件可用且 ``req_fd`` 尚未设置 **O_NONBLOCK** ，read()
将阻塞。)

The presence of an event can be tested for by checking that the ``req_fd`` is
readable using `poll()` or an equivalent.
(可以用 poll() 或等效方法检查 ``req_fd`` 是否可读，以测试是否有
事件。)

Return Value(返回值)
====================

On success the number of bytes read, which will be a multiple of the size of a
:c:type:`gpio_v2_line_event` event.
(成功时返回读取的字节数，它是 :c:type:`gpio_v2_line_event` 事件
大小的倍数。)

On error -1 and the ``errno`` variable is set appropriately.
Common error codes are described in error-codes.rst.
(出错时返回 -1 并适当设置 ``errno`` 变量。常见错误码在
error-codes.rst 中描述。)
