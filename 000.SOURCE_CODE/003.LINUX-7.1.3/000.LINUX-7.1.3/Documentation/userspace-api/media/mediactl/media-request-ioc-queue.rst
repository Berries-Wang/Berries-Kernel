.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_request_ioc_queue:

*****************************
ioctl MEDIA_REQUEST_IOC_QUEUE
*****************************

Name(名称)
==========

MEDIA_REQUEST_IOC_QUEUE - Queue a request
(MEDIA_REQUEST_IOC_QUEUE - 将一个请求排入队列。)

Synopsis(概要)
==============

.. c:macro:: MEDIA_REQUEST_IOC_QUEUE

``int ioctl(int request_fd, MEDIA_REQUEST_IOC_QUEUE)``

Arguments(参数)
===============

``request_fd``
    File descriptor returned by :ref:`MEDIA_IOC_REQUEST_ALLOC`.
    (由 :ref:`MEDIA_IOC_REQUEST_ALLOC` 返回的文件描述符。)

Description(描述)
=================

If the media device supports :ref:`requests <media-request-api>`, then
this request ioctl can be used to queue a previously allocated request.
(如果媒体设备支持 :ref:`requests <media-request-api>`，则此请求 ioctl
可用于把之前分配的请求排入队列。)

If the request was successfully queued, then the file descriptor can be
:ref:`polled <request-func-poll>` to wait for the request to complete.
(如果请求已成功入队，则可以对文件描述符进行
:ref:`polled <request-func-poll>`，以等待请求完成。)

If the request was already queued before, then ``EBUSY`` is returned.
Other errors can be returned if the contents of the request contained
invalid or inconsistent data, see the next section for a list of
common error codes. On error both the request and driver state are unchanged.
(如果该请求之前已被入队，则返回 ``EBUSY``。如果请求的内容包含无效或
不一致的数据，也可能返回其他错误；常见错误码的列表见下一节。出错时，
请求和驱动程序的状态都不会改变。)

Once a request is queued, then the driver is required to gracefully handle
errors that occur when the request is applied to the hardware. The
exception is the ``EIO`` error which signals a fatal error that requires
the application to stop streaming to reset the hardware state.
(一旦请求入队，驱动程序就必须妥善处理把请求应用到硬件时发生的错误。
例外是 ``EIO`` 错误，它表示致命错误，需要应用程序停止流传输以重置硬
件状态。)

It is not allowed to mix queuing requests with queuing buffers directly
(without a request). ``EBUSY`` will be returned if the first buffer was
queued directly and you next try to queue a request, or vice versa.
(不允许把请求入队与直接排队缓冲区 (不使用请求) 混用。如果第一个缓冲
区是直接排队的，而你接着尝试将请求入队，或反之，将返回 ``EBUSY``。)

A request must contain at least one buffer, otherwise this ioctl will
return an ``ENOENT`` error.
(一个请求必须至少包含一个缓冲区，否则此 ioctl 将返回 ``ENOENT`` 错
误。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EBUSY
    The request was already queued or the application queued the first
    buffer directly, but later attempted to use a request. It is not permitted
    to mix the two APIs.
    (该请求已经入队，或应用程序先直接排队了第一个缓冲区、之后却试图
    使用请求。不允许混用这两种 API。)
ENOENT
    The request did not contain any buffers. All requests are required
    to have at least one buffer. This can also be returned if some required
    configuration is missing in the request.
    (请求未包含任何缓冲区。所有请求都要求至少有一个缓冲区。如果请求
    中缺少某些必需的配置，也可能返回此错误码。)
ENOMEM
    Out of memory when allocating internal data structures for this
    request.
    (为该请求分配内部数据结构时内存不足。)
EINVAL
    The request has invalid data.
    (请求包含无效数据。)
EIO
    The hardware is in a bad state. To recover, the application needs to
    stop streaming to reset the hardware state and then try to restart
    streaming.
    (硬件处于不良状态。为恢复，应用程序需要停止流传输以重置硬件状
    态，然后尝试重新开始流传输。)
