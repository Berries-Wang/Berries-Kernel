.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_G_FRAME_INTERVAL:

********************************************************************
ioctl VIDIOC_SUBDEV_G_FRAME_INTERVAL, VIDIOC_SUBDEV_S_FRAME_INTERVAL
********************************************************************

Name(名称)
==========

VIDIOC_SUBDEV_G_FRAME_INTERVAL - VIDIOC_SUBDEV_S_FRAME_INTERVAL - Get or set the frame interval on a subdev pad
(VIDIOC_SUBDEV_G_FRAME_INTERVAL - VIDIOC_SUBDEV_S_FRAME_INTERVAL - 获取或设置子设备 pad 上的帧间隔。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_G_FRAME_INTERVAL

``int ioctl(int fd, VIDIOC_SUBDEV_G_FRAME_INTERVAL, struct v4l2_subdev_frame_interval *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_FRAME_INTERVAL

``int ioctl(int fd, VIDIOC_SUBDEV_S_FRAME_INTERVAL, struct v4l2_subdev_frame_interval *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_frame_interval`.
    (指向 struct :c:type:`v4l2_subdev_frame_interval` 的指针。)

Description(描述)
=================

These ioctls are used to get and set the frame interval at specific
subdev pads in the image pipeline. The frame interval only makes sense
for sub-devices that can control the frame period on their own. This
includes, for instance, image sensors and TV tuners. Sub-devices that
don't support frame intervals must not implement these ioctls.
(这些 ioctl 用于获取和设置图像管线中特定 subdev pad 上的帧间隔。帧间隔只对能够自行控制帧周期的子设备有意义。例如，这包括图像传感器和电视调谐器。不支持帧间隔的子设备不应实现这些 ioctl。)

To retrieve the current frame interval applications set the ``pad``
field of a struct
:c:type:`v4l2_subdev_frame_interval` to
the desired pad number as reported by the media controller API. When
they call the ``VIDIOC_SUBDEV_G_FRAME_INTERVAL`` ioctl with a pointer to
this structure the driver fills the members of the ``interval`` field.
(要获取当前帧间隔，应用程序将 struct :c:type:`v4l2_subdev_frame_interval` 的 ``pad`` 字段设置为媒体控制器 API 所报告的所需 pad 编号。当它们以指向该结构的指针调用 ``VIDIOC_SUBDEV_G_FRAME_INTERVAL`` ioctl 时，驱动程序会填充 ``interval`` 字段的成员。)

To change the current frame interval applications set both the ``pad``
field and all members of the ``interval`` field. When they call the
``VIDIOC_SUBDEV_S_FRAME_INTERVAL`` ioctl with a pointer to this
structure the driver verifies the requested interval, adjusts it based
on the hardware capabilities and configures the device. Upon return the
struct
:c:type:`v4l2_subdev_frame_interval`
contains the current frame interval as would be returned by a
``VIDIOC_SUBDEV_G_FRAME_INTERVAL`` call.
(要更改当前帧间隔，应用程序设置 ``pad`` 字段和 ``interval`` 字段的所有成员。当它们以指向该结构的指针调用 ``VIDIOC_SUBDEV_S_FRAME_INTERVAL`` ioctl 时，驱动程序会验证请求的间隔，根据硬件能力进行调整并配置设备。返回时，struct :c:type:`v4l2_subdev_frame_interval` 包含当前帧间隔，如同 ``VIDIOC_SUBDEV_G_FRAME_INTERVAL`` 调用返回的那样。)

If the subdev device node has been registered in read-only mode, calls to
``VIDIOC_SUBDEV_S_FRAME_INTERVAL`` are only valid if the ``which`` field is set
to ``V4L2_SUBDEV_FORMAT_TRY``, otherwise an error is returned and the errno
variable is set to ``-EPERM``.
(如果 subdev 设备节点以只读模式注册，则对 ``VIDIOC_SUBDEV_S_FRAME_INTERVAL`` 的调用仅在 ``which`` 字段设置为 ``V4L2_SUBDEV_FORMAT_TRY`` 时有效，否则返回错误并将 errno 变量设置为 ``-EPERM`` 。)

Drivers must not return an error solely because the requested interval
doesn't match the device capabilities. They must instead modify the
interval to match what the hardware can provide. The modified interval
should be as close as possible to the original request.
(驱动程序不得仅仅因为请求的间隔与设备能力不匹配而返回错误。它们必须改为修改该间隔以匹配硬件所能提供的。修改后的间隔应尽可能接近原始请求。)

Changing the frame interval shall never change the format. Changing the
format, on the other hand, may change the frame interval.
(更改帧间隔绝不应更改格式。另一方面，更改格式可能会改变帧间隔。)

Sub-devices that support the frame interval ioctls should implement them
on a single pad only. Their behaviour when supported on multiple pads of
the same sub-device is not defined.
(支持帧间隔 ioctl 的子设备应只在单个 pad 上实现它们。当在同一子设备的多个 pad 上支持它们时，其行为未定义。)

.. c:type:: v4l2_subdev_frame_interval

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_subdev_frame_interval
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``pad``
      - Pad number as reported by the media controller API.
        (媒体控制器 API 所报告的 pad 编号。)
    * - struct :c:type:`v4l2_fract`
      - ``interval``
      - Period, in seconds, between consecutive video frames.
        (连续视频帧之间的周期，以秒为单位。)
    * - __u32
      - ``stream``
      - Stream identifier.
        (流标识符。)
    * - __u32
      - ``which``
      - Active or try frame interval, from enum
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
        (活动或 try 帧间隔，来自 enum :ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>` 。)
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
    The frame interval can't be changed because the pad is currently
    busy. This can be caused, for instance, by an active video stream on
    the pad. The ioctl must not be retried without performing another
    action to fix the problem first. Only returned by
    ``VIDIOC_SUBDEV_S_FRAME_INTERVAL``
    (无法更改帧间隔，因为 pad 当前正忙。例如，这可能是由 pad 上活动的视频流引起的。必须先执行其他操作解决该问题，然后才能重试此 ioctl。仅由 ``VIDIOC_SUBDEV_S_FRAME_INTERVAL`` 返回。)

EINVAL
    The struct :c:type:`v4l2_subdev_frame_interval` ``pad`` references a
    non-existing pad, the ``which`` field has an unsupported value, or the pad
    doesn't support frame intervals.
    (struct :c:type:`v4l2_subdev_frame_interval` 的 ``pad`` 引用了不存在的 pad、``which`` 字段的值不受支持，或该 pad 不支持帧间隔。)

EPERM
    The ``VIDIOC_SUBDEV_S_FRAME_INTERVAL`` ioctl has been called on a read-only
    subdevice and the ``which`` field is set to ``V4L2_SUBDEV_FORMAT_ACTIVE``.
    (在只读子设备上调用了 ``VIDIOC_SUBDEV_S_FRAME_INTERVAL`` ioctl，且 ``which`` 字段被设置为 ``V4L2_SUBDEV_FORMAT_ACTIVE`` 。)
