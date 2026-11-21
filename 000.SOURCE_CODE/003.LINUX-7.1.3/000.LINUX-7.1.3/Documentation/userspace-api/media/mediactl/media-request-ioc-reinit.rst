.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_request_ioc_reinit:

**********************************************
ioctl MEDIA_REQUEST_IOC_REINIT(重新初始化请求)
**********************************************

Name(名称)
==========

MEDIA_REQUEST_IOC_REINIT - Re-initialize a request(MEDIA_REQUEST_IOC_REINIT - 重新初始化一个请求)

Synopsis(概要)
==============

.. c:macro:: MEDIA_REQUEST_IOC_REINIT

``int ioctl(int request_fd, MEDIA_REQUEST_IOC_REINIT)``

Arguments(参数)
===============

``request_fd``
    File descriptor returned by :ref:`MEDIA_IOC_REQUEST_ALLOC`.
    (由 :ref:`MEDIA_IOC_REQUEST_ALLOC` 返回的文件描述符。)

Description(描述)
=================

If the media device supports :ref:`requests <media-request-api>`, then
this request ioctl can be used to re-initialize a previously allocated
request.
(如果媒体设备支持 :ref:`requests <media-request-api>`，则此请求 ioctl 可
用于重新初始化之前分配的请求。)

Re-initializing a request will clear any existing data from the request.
This avoids having to :c:func:`close()` a completed
request and allocate a new request. Instead the completed request can just
be re-initialized and it is ready to be used again.
(重新初始化请求会清除请求中的所有既有数据。这避免了必须 :c:func:`close()`
已完成的请求再分配新请求。相反，已完成的请求只需重新初始化即可再次
使用。)

A request can only be re-initialized if it either has not been queued
yet, or if it was queued and completed. Otherwise it will set ``errno``
to ``EBUSY``. No other error codes can be returned.
(只有当请求尚未入队，或者已入队且已完成时，才能重新初始化。否则它会把
``errno`` 设置为 ``EBUSY``。不会返回其他错误码。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。)

EBUSY
    The request is queued but not yet completed.
    (请求已入队但尚未完成。)
