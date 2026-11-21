.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_STD:

**************************************************************************
ioctl VIDIOC_G_STD, VIDIOC_S_STD, VIDIOC_SUBDEV_G_STD, VIDIOC_SUBDEV_S_STD
**************************************************************************

Name(名称)
==========

VIDIOC_G_STD - VIDIOC_S_STD - VIDIOC_SUBDEV_G_STD - VIDIOC_SUBDEV_S_STD - Query or select the video standard of the current input
(VIDIOC_G_STD - VIDIOC_S_STD - VIDIOC_SUBDEV_G_STD - VIDIOC_SUBDEV_S_STD
- 查询或选择当前输入的视频标准。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_STD

``int ioctl(int fd, VIDIOC_G_STD, v4l2_std_id *argp)``

.. c:macro:: VIDIOC_S_STD

``int ioctl(int fd, VIDIOC_S_STD, const v4l2_std_id *argp)``

.. c:macro:: VIDIOC_SUBDEV_G_STD

``int ioctl(int fd, VIDIOC_SUBDEV_G_STD, v4l2_std_id *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_STD

``int ioctl(int fd, VIDIOC_SUBDEV_S_STD, const v4l2_std_id *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to :c:type:`v4l2_std_id`.
    (指向 :c:type:`v4l2_std_id` 的指针。)

Description(描述)
=================

To query and select the current video standard applications use the
:ref:`VIDIOC_G_STD <VIDIOC_G_STD>` and :ref:`VIDIOC_S_STD <VIDIOC_G_STD>` ioctls which take a pointer to a
:ref:`v4l2_std_id <v4l2-std-id>` type as argument. :ref:`VIDIOC_G_STD <VIDIOC_G_STD>`
can return a single flag or a set of flags as in struct
:c:type:`v4l2_standard` field ``id``. The flags must be
unambiguous such that they appear in only one enumerated
struct :c:type:`v4l2_standard` structure.
(应用程序使用 :ref:`VIDIOC_G_STD <VIDIOC_G_STD>` 与
:ref:`VIDIOC_S_STD <VIDIOC_G_STD>` ioctl 来查询和选择当前视频标准，它
们接受指向 :ref:`v4l2_std_id <v4l2-std-id>` 类型的指针作为参数。
:ref:`VIDIOC_G_STD <VIDIOC_G_STD>` 可以返回单个标志或一组标志，如同
struct :c:type:`v4l2_standard` 的 ``id`` 字段那样。这些标志必须无歧
义，即它们只出现在一个被枚举的 struct :c:type:`v4l2_standard` 结构
中。)

:ref:`VIDIOC_S_STD <VIDIOC_G_STD>` accepts one or more flags, being a write-only ioctl it
does not return the actual new standard as :ref:`VIDIOC_G_STD <VIDIOC_G_STD>` does. When
no flags are given or the current input does not support the requested
standard the driver returns an ``EINVAL`` error code. When the standard set
is ambiguous drivers may return ``EINVAL`` or choose any of the requested
standards. If the current input or output does not support standard
video timings (e.g. if :ref:`VIDIOC_ENUMINPUT`
does not set the ``V4L2_IN_CAP_STD`` flag), then ``ENODATA`` error code is
returned.
( :ref:`VIDIOC_S_STD <VIDIOC_G_STD>` 接受一个或多个标志；由于它是只写
ioctl，它不像 :ref:`VIDIOC_G_STD <VIDIOC_G_STD>` 那样返回实际的新标
准。当没有给出标志，或当前输入不支持所请求的标准时，驱动程序返回
``EINVAL`` 错误码。当标准集合有歧义时，驱动程序可以返回 ``EINVAL`` 或
选择所请求标准中的任意一个。如果当前输入或输出不支持标准视频时序 (例
如 :ref:`VIDIOC_ENUMINPUT` 未设置 ``V4L2_IN_CAP_STD`` 标志)，则返回
``ENODATA`` 错误码。)

Calling ``VIDIOC_SUBDEV_S_STD`` on a subdev device node that has been registered
in read-only mode is not allowed. An error is returned and the errno variable is
set to ``-EPERM``.
(不允许对以只读模式注册的 subdev 设备节点调用 ``VIDIOC_SUBDEV_S_STD``。
此时会返回一个错误，且 errno 变量被设置为 ``-EPERM``。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The :ref:`VIDIOC_S_STD <VIDIOC_G_STD>` parameter was unsuitable.
    ( :ref:`VIDIOC_S_STD <VIDIOC_G_STD>` 参数不合适。)

ENODATA
    Standard video timings are not supported for this input or output.
    (此输入或输出不支持标准视频时序。)

EPERM
    ``VIDIOC_SUBDEV_S_STD`` has been called on a read-only subdevice.
    ( ``VIDIOC_SUBDEV_S_STD`` 被调用在一个只读的 subdevice 上。)
