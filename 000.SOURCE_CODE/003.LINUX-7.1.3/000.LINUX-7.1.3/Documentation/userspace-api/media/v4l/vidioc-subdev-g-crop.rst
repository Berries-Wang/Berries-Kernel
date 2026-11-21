.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_G_CROP:

************************************************
ioctl VIDIOC_SUBDEV_G_CROP, VIDIOC_SUBDEV_S_CROP
************************************************

Name(名称)
==========

VIDIOC_SUBDEV_G_CROP - VIDIOC_SUBDEV_S_CROP - Get or set the crop rectangle on a subdev pad
(VIDIOC_SUBDEV_G_CROP - VIDIOC_SUBDEV_S_CROP - 获取或设置子设备 pad 上的裁剪矩形。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_G_CROP

``int ioctl(int fd, VIDIOC_SUBDEV_G_CROP, struct v4l2_subdev_crop *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_CROP

``int ioctl(int fd, VIDIOC_SUBDEV_S_CROP, const struct v4l2_subdev_crop *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_crop`.
    (指向 struct :c:type:`v4l2_subdev_crop` 的指针。)

Description(描述)
=================

.. note::

    This is an :ref:`obsolete` interface and may be removed in the future. It is
    superseded by :ref:`the selection API <VIDIOC_SUBDEV_G_SELECTION>`. No new
    extensions to the :c:type:`v4l2_subdev_crop` structure will be accepted.
    (这是一个 :ref:`obsolete` （已过时）接口，将来可能会被移除。它已被 :ref:`the selection API <VIDIOC_SUBDEV_G_SELECTION>` 取代。不会再接受对 :c:type:`v4l2_subdev_crop` 结构的新扩展。)

To retrieve the current crop rectangle applications set the ``pad``
field of a struct :c:type:`v4l2_subdev_crop` to the
desired pad number as reported by the media API and the ``which`` field
to ``V4L2_SUBDEV_FORMAT_ACTIVE``. They then call the
``VIDIOC_SUBDEV_G_CROP`` ioctl with a pointer to this structure. The
driver fills the members of the ``rect`` field or returns ``EINVAL`` error
code if the input arguments are invalid, or if cropping is not supported
on the given pad.
(要获取当前裁剪矩形，应用程序将 struct :c:type:`v4l2_subdev_crop` 的 ``pad`` 字段设置为媒体 API 所报告的所需 pad 编号，并把 ``which`` 字段设置为 ``V4L2_SUBDEV_FORMAT_ACTIVE`` 。然后以指向该结构的指针调用 ``VIDIOC_SUBDEV_G_CROP`` ioctl。驱动程序填充 ``rect`` 字段的成员；如果输入参数无效或给定 pad 不支持裁剪，则返回 ``EINVAL`` 错误码。)

To change the current crop rectangle applications set both the ``pad``
and ``which`` fields and all members of the ``rect`` field. They then
call the ``VIDIOC_SUBDEV_S_CROP`` ioctl with a pointer to this
structure. The driver verifies the requested crop rectangle, adjusts it
based on the hardware capabilities and configures the device. Upon
return the struct :c:type:`v4l2_subdev_crop`
contains the current format as would be returned by a
``VIDIOC_SUBDEV_G_CROP`` call.
(要更改当前裁剪矩形，应用程序设置 ``pad`` 和 ``which`` 字段以及 ``rect`` 字段的所有成员。然后以指向该结构的指针调用 ``VIDIOC_SUBDEV_S_CROP`` ioctl。驱动程序验证请求的裁剪矩形，根据硬件能力进行调整并配置设备。返回时，struct :c:type:`v4l2_subdev_crop` 包含当前格式，如同 ``VIDIOC_SUBDEV_G_CROP`` 调用返回的那样。)

Applications can query the device capabilities by setting the ``which``
to ``V4L2_SUBDEV_FORMAT_TRY``. When set, 'try' crop rectangles are not
applied to the device by the driver, but are mangled exactly as active
crop rectangles and stored in the sub-device file handle. Two
applications querying the same sub-device would thus not interact with
each other.
(应用程序可以通过把 ``which`` 设置为 ``V4L2_SUBDEV_FORMAT_TRY`` 来查询设备能力。设置之后，'try' 裁剪矩形不会被驱动程序应用到设备上，但会与活动裁剪矩形完全一样地被修整，并存储在子设备文件句柄中。这样，两个查询同一子设备的应用程序之间就不会互相影响。)

If the subdev device node has been registered in read-only mode, calls to
``VIDIOC_SUBDEV_S_CROP`` are only valid if the ``which`` field is set to
``V4L2_SUBDEV_FORMAT_TRY``, otherwise an error is returned and the errno
variable is set to ``-EPERM``.
(如果 subdev 设备节点以只读模式注册，则对 ``VIDIOC_SUBDEV_S_CROP`` 的调用仅在 ``which`` 字段设置为 ``V4L2_SUBDEV_FORMAT_TRY`` 时有效，否则返回错误并将 errno 变量设置为 ``-EPERM`` 。)

Drivers must not return an error solely because the requested crop
rectangle doesn't match the device capabilities. They must instead
modify the rectangle to match what the hardware can provide. The
modified format should be as close as possible to the original request.
(驱动程序不得仅仅因为请求的裁剪矩形与设备能力不匹配而返回错误。它们必须改为修改该矩形以匹配硬件所能提供的。修改后的格式应尽可能接近原始请求。)

.. c:type:: v4l2_subdev_crop

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_subdev_crop
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``pad``
      - Pad number as reported by the media framework.
        (媒体框架所报告的 pad 编号。)
    * - __u32
      - ``which``
      - Crop rectangle to get or set, from enum
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
        (要获取或设置的裁剪矩形，来自 enum :ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>` 。)
    * - struct :c:type:`v4l2_rect`
      - ``rect``
      - Crop rectangle boundaries, in pixels.
        (裁剪矩形的边界，以像素为单位。)
    * - __u32
      - ``stream``
      - Stream identifier.
        (流标识符。)
    * - __u32
      - ``reserved``\ [7]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.
        (保留用于未来的扩展。应用程序和驱动程序必须把该数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EBUSY
    The crop rectangle can't be changed because the pad is currently
    busy. This can be caused, for instance, by an active video stream on
    the pad. The ioctl must not be retried without performing another
    action to fix the problem first. Only returned by
    ``VIDIOC_SUBDEV_S_CROP``
    (无法更改裁剪矩形，因为 pad 当前正忙。例如，这可能是由 pad 上活动的视频流引起的。必须先执行其他操作解决该问题，然后才能重试此 ioctl。仅由 ``VIDIOC_SUBDEV_S_CROP`` 返回。)

EINVAL
    The struct :c:type:`v4l2_subdev_crop` ``pad`` references a non-existing pad,
    the ``which`` field has an unsupported value, or cropping is not supported
    on the given subdev pad.
    (struct :c:type:`v4l2_subdev_crop` 的 ``pad`` 引用了不存在的 pad、``which`` 字段的值不受支持，或给定的 subdev pad 不支持裁剪。)

EPERM
    The ``VIDIOC_SUBDEV_S_CROP`` ioctl has been called on a read-only subdevice
    and the ``which`` field is set to ``V4L2_SUBDEV_FORMAT_ACTIVE``.
    (在只读子设备上调用了 ``VIDIOC_SUBDEV_S_CROP`` ioctl，且 ``which`` 字段被设置为 ``V4L2_SUBDEV_FORMAT_ACTIVE`` 。)
