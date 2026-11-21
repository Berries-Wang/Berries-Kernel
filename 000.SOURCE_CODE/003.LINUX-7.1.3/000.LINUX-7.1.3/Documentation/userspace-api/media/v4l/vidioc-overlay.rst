.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_OVERLAY:

****************************************
ioctl VIDIOC_OVERLAY(开始或停止视频叠加)
****************************************

Name(名称)
==========

VIDIOC_OVERLAY - Start or stop video overlay(VIDIOC_OVERLAY - 开始或停止视频叠加)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_OVERLAY

``int ioctl(int fd, VIDIOC_OVERLAY, const int *argp)``

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

This ioctl is part of the :ref:`video overlay <overlay>` I/O method.
Applications call :ref:`VIDIOC_OVERLAY` to start or stop the overlay. It
takes a pointer to an integer which must be set to zero by the
application to stop overlay, to one to start.
(此 ioctl 属于 :ref:`video overlay <overlay>` I/O 方法。应用调用
:ref:`VIDIOC_OVERLAY` 来开始或停止叠加。它接受一个指向整数的指针：应用
必须将其设为 0 以停止叠加，设为 1 以开始。)

Drivers do not support :ref:`VIDIOC_STREAMON` or
:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` with
``V4L2_BUF_TYPE_VIDEO_OVERLAY``.
(对于 ``V4L2_BUF_TYPE_VIDEO_OVERLAY``，驱动不支持 :ref:`VIDIOC_STREAMON`
或 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The overlay parameters have not been set up. See :ref:`overlay`
    for the necessary steps.
    (尚未设置叠加参数。必要的步骤参见 :ref:`overlay` 。)
