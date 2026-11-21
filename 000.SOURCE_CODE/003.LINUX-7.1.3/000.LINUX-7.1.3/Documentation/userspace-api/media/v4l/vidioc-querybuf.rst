.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_QUERYBUF:

*********************
ioctl VIDIOC_QUERYBUF
*********************

Name(名称)
==========

VIDIOC_QUERYBUF - Query the status of a buffer
(VIDIOC_QUERYBUF - 查询缓冲区的状态。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_QUERYBUF

``int ioctl(int fd, VIDIOC_QUERYBUF, struct v4l2_buffer *argp)``

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

This ioctl is part of the :ref:`streaming <mmap>` I/O method. It can
be used to query the status of a buffer at any time after buffers have
been allocated with the :ref:`VIDIOC_REQBUFS` ioctl.
(此 ioctl 属于 :ref:`streaming <mmap>` I/O 方法的一部分。在用
:ref:`VIDIOC_REQBUFS` ioctl 分配缓冲区之后的任何时刻，都可以用它查
询缓冲区的状态。)

Applications set the ``type`` field of a struct
:c:type:`v4l2_buffer` to the same buffer type as was
previously used with struct :c:type:`v4l2_format` ``type``
and struct :c:type:`v4l2_requestbuffers` ``type``,
and the ``index`` field. Valid index numbers range from zero to the
number of buffers allocated with
:ref:`VIDIOC_REQBUFS` (struct
:c:type:`v4l2_requestbuffers` ``count``) minus
one. The ``reserved`` and ``reserved2`` fields must be set to 0. When
using the :ref:`multi-planar API <planar-apis>`, the ``m.planes``
field must contain a userspace pointer to an array of struct
:c:type:`v4l2_plane` and the ``length`` field has to be set
to the number of elements in that array. After calling
:ref:`VIDIOC_QUERYBUF` with a pointer to this structure drivers return an
error code or fill the rest of the structure.
(应用程序把 struct :c:type:`v4l2_buffer` 的 ``type`` 字段设置为与之前
在 struct :c:type:`v4l2_format` 的 ``type`` 和 struct
:c:type:`v4l2_requestbuffers` 的 ``type`` 中使用的相同缓冲区类型，并
设置 ``index`` 字段。有效索引号的范围是从 0 到用 :ref:`VIDIOC_REQBUFS`
分配的缓冲区数量 (struct :c:type:`v4l2_requestbuffers` 的 ``count``)
减一。 ``reserved`` 与 ``reserved2`` 字段必须设置为 0。使用
:ref:`multi-planar API <planar-apis>` 时， ``m.planes`` 字段必须包含
一个指向 struct :c:type:`v4l2_plane` 数组的用户空间指针，并且
``length`` 字段必须设置为该数组中元素的数量。以指向此结构的指针调用
:ref:`VIDIOC_QUERYBUF` 之后，驱动程序返回一个错误码或填充该结构的其
余部分。)

In the ``flags`` field the ``V4L2_BUF_FLAG_MAPPED``,
``V4L2_BUF_FLAG_PREPARED``, ``V4L2_BUF_FLAG_QUEUED`` and
``V4L2_BUF_FLAG_DONE`` flags will be valid. The ``memory`` field will be
set to the current I/O method. For the single-planar API, the
``m.offset`` contains the offset of the buffer from the start of the
device memory, the ``length`` field its size. For the multi-planar API,
fields ``m.mem_offset`` and ``length`` in the ``m.planes`` array
elements will be used instead and the ``length`` field of struct
:c:type:`v4l2_buffer` is set to the number of filled-in
array elements. The driver may or may not set the remaining fields and
flags, they are meaningless in this context.
( ``flags`` 字段中的 ``V4L2_BUF_FLAG_MAPPED`` 、
``V4L2_BUF_FLAG_PREPARED`` 、 ``V4L2_BUF_FLAG_QUEUED`` 与
``V4L2_BUF_FLAG_DONE`` 标志将是有效的。 ``memory`` 字段会被设置为当
前的 I/O 方法。对于单平面 API， ``m.offset`` 包含缓冲区相对于设备内
存起始处的偏移量， ``length`` 字段为其大小。对于多平面 API，将改用
``m.planes`` 数组元素中的 ``m.mem_offset`` 与 ``length`` 字段，并且
struct :c:type:`v4l2_buffer` 的 ``length`` 字段被设置为已填入的数组
元素数量。驱动程序可能设置也可能不设置其余的字段和标志，它们在此上
下文中没有意义。)

The struct :c:type:`v4l2_buffer` structure is specified in
:ref:`buffer`.
(struct :c:type:`v4l2_buffer` 结构在 :ref:`buffer` 中说明。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The buffer ``type`` is not supported, or the ``index`` is out of
    bounds.
    (缓冲区 ``type`` 不受支持，或 ``index`` 越界。)
