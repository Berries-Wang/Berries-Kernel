.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_ioc_request_alloc:

***************************************
ioctl MEDIA_IOC_REQUEST_ALLOC(分配请求)
***************************************

Name(名称)
==========

MEDIA_IOC_REQUEST_ALLOC - Allocate a request(MEDIA_IOC_REQUEST_ALLOC - 分配一个请求)

Synopsis(概要)
==============

.. c:macro:: MEDIA_IOC_REQUEST_ALLOC

``int ioctl(int fd, MEDIA_IOC_REQUEST_ALLOC, int *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to an integer.
    (指向一个整数的指针。)

Description(描述)
=================

If the media device supports :ref:`requests <media-request-api>`, then
this ioctl can be used to allocate a request. If it is not supported, then
``errno`` is set to ``ENOTTY``. A request is accessed through a file descriptor
that is returned in ``*argp``.
(如果媒体设备支持 :ref:`requests <media-request-api>`，那么可以使用此 ioctl 来
分配一个请求。如果不支持，则 ``errno`` 被设置为 ``ENOTTY``。请求通过 ``*argp``
中返回的文件描述符来访问。)

If the request was successfully allocated, then the request file descriptor
can be passed to the :ref:`VIDIOC_QBUF <VIDIOC_QBUF>`,
:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`,
:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` and
:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` ioctls.
(如果请求分配成功，请求文件描述符就可以传给 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>`、
:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`、
:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 和
:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 这些 ioctl。)

In addition, the request can be queued by calling
:ref:`MEDIA_REQUEST_IOC_QUEUE` and re-initialized by calling
:ref:`MEDIA_REQUEST_IOC_REINIT`.
(此外，可以调用 :ref:`MEDIA_REQUEST_IOC_QUEUE` 将请求入队，调用
:ref:`MEDIA_REQUEST_IOC_REINIT` 将其重新初始化。)

Finally, the file descriptor can be :ref:`polled <request-func-poll>` to wait
for the request to complete.
(最后，可以对文件描述符进行 :ref:`poll <request-func-poll>`，以等待请求完成。)

The request will remain allocated until all the file descriptors associated
with it are closed by :c:func:`close()` and the driver no
longer uses the request internally. See also
:ref:`here <media-request-life-time>` for more information.
(请求会保持已分配状态，直到与其关联的所有文件描述符都被 :c:func:`close()`
关闭，且驱动不再在内部使用该请求。更多信息参见
:ref:`here <media-request-life-time>`。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

ENOTTY
    The driver has no support for requests.
    (驱动不支持请求。)
