.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_PREPARE_BUF:

***********************************************
ioctl VIDIOC_PREPARE_BUF(准备用于 I/O 的缓冲区)
***********************************************

Name(名称)
==========

VIDIOC_PREPARE_BUF - Prepare a buffer for I/O(VIDIOC_PREPARE_BUF - 为 I/O 准备缓冲区)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_PREPARE_BUF

``int ioctl(int fd, VIDIOC_PREPARE_BUF, struct v4l2_buffer *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_buffer`.
    (指向 struct :c:type:`v4l2_buffer` 的指针。)

Description(描述)
=================

Applications can optionally call the :ref:`VIDIOC_PREPARE_BUF` ioctl to
pass ownership of the buffer to the driver before actually enqueuing it,
using the :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` ioctl, and to prepare it for future I/O. Such
preparations may include cache invalidation or cleaning. Performing them
in advance saves time during the actual I/O.
(应用可以可选地调用 :ref:`VIDIOC_PREPARE_BUF` ioctl，在使用
:ref:`VIDIOC_QBUF <VIDIOC_QBUF>` ioctl 实际将缓冲区入队之前，先把缓冲区的
所有权移交给驱动，并为将来的 I/O 做准备。此类准备可能包括缓存失效 (invalidate)
或清理操作。提前执行它们可以节省实际 I/O 期间的时间。)

The struct :c:type:`v4l2_buffer` structure is specified in
:ref:`buffer`.
(struct :c:type:`v4l2_buffer` 结构在 :ref:`buffer` 中说明。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EBUSY
    File I/O is in progress.
    (文件 I/O 正在进行中。)

EINVAL
    The buffer ``type`` is not supported, or the ``index`` is out of
    bounds, or no buffers have been allocated yet, or the ``userptr`` or
    ``length`` are invalid.
    (缓冲区 ``type`` 不受支持，或 ``index`` 越界，或尚未分配任何缓冲区，
    或 ``userptr`` 或 ``length`` 无效。)
