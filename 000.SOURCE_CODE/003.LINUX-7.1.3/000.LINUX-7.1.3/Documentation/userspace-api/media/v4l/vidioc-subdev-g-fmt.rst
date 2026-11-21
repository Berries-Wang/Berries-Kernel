.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_G_FMT:

*******************************************************************************
ioctl VIDIOC_SUBDEV_G_FMT, VIDIOC_SUBDEV_S_FMT(获取或设置子设备 pad 的数据格式)
*******************************************************************************

Name(名称)
==========

VIDIOC_SUBDEV_G_FMT - VIDIOC_SUBDEV_S_FMT - Get or set the data format on a subdev pad
(VIDIOC_SUBDEV_G_FMT - VIDIOC_SUBDEV_S_FMT - 获取或设置子设备 pad 上的数据格式)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_G_FMT

``int ioctl(int fd, VIDIOC_SUBDEV_G_FMT, struct v4l2_subdev_format *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_FMT

``int ioctl(int fd, VIDIOC_SUBDEV_S_FMT, struct v4l2_subdev_format *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_format`.
    (指向 struct :c:type:`v4l2_subdev_format` 的指针。)

Description(描述)
=================

These ioctls are used to negotiate the frame format at specific subdev
pads in the image pipeline.
(这些 ioctl 用于协商图像管线中特定子设备 pad 上的帧格式。)

To retrieve the current format applications set the ``pad`` field of a
struct :c:type:`v4l2_subdev_format` to the desired
pad number as reported by the media API and the ``which`` field to
``V4L2_SUBDEV_FORMAT_ACTIVE``. When they call the
``VIDIOC_SUBDEV_G_FMT`` ioctl with a pointer to this structure the
driver fills the members of the ``format`` field.
(要获取当前格式，应用程序将 struct :c:type:`v4l2_subdev_format` 的 ``pad``
字段设置为 media API 报告的所需 pad 编号，并将 ``which`` 字段设置为
``V4L2_SUBDEV_FORMAT_ACTIVE``。当他们用指向该结构体的指针调用
``VIDIOC_SUBDEV_G_FMT`` ioctl 时，驱动会填充 ``format`` 字段的各个成员。)

To change the current format applications set both the ``pad`` and
``which`` fields and all members of the ``format`` field. When they call
the ``VIDIOC_SUBDEV_S_FMT`` ioctl with a pointer to this structure the
driver verifies the requested format, adjusts it based on the hardware
capabilities and configures the device. Upon return the struct
:c:type:`v4l2_subdev_format` contains the current
format as would be returned by a ``VIDIOC_SUBDEV_G_FMT`` call.
(要更改当前格式，应用程序设置 ``pad`` 与 ``which`` 字段以及 ``format``
字段的所有成员。当他们用指向该结构体的指针调用 ``VIDIOC_SUBDEV_S_FMT``
ioctl 时，驱动会校验所请求的格式，根据硬件能力进行调整并配置设备。返回时，
struct :c:type:`v4l2_subdev_format` 包含当前格式，与 ``VIDIOC_SUBDEV_G_FMT``
调用将返回的格式相同。)

Applications can query the device capabilities by setting the ``which``
to ``V4L2_SUBDEV_FORMAT_TRY``. When set, 'try' formats are not applied
to the device by the driver, but are changed exactly as active formats
and stored in the sub-device file handle. Two applications querying the
same sub-device would thus not interact with each other.
(应用程序可以通过将 ``which`` 设置为 ``V4L2_SUBDEV_FORMAT_TRY`` 来查询设备
能力。设置后，驱动不会把 "try" 格式应用到设备，但会以与活动格式完全相同的
方式修改它们，并将其存储在子设备文件句柄中。这样，查询同一子设备的两个
应用程序之间就不会互相影响。)

For instance, to try a format at the output pad of a sub-device,
applications would first set the try format at the sub-device input with
the ``VIDIOC_SUBDEV_S_FMT`` ioctl. They would then either retrieve the
default format at the output pad with the ``VIDIOC_SUBDEV_G_FMT`` ioctl,
or set the desired output pad format with the ``VIDIOC_SUBDEV_S_FMT``
ioctl and check the returned value.
(例如，要在子设备的输出 pad 上尝试某种格式，应用程序会先用
``VIDIOC_SUBDEV_S_FMT`` ioctl 在子设备输入端设置 try 格式。然后，它们既可以
用 ``VIDIOC_SUBDEV_G_FMT`` ioctl 检索输出 pad 上的默认格式，也可以用
``VIDIOC_SUBDEV_S_FMT`` ioctl 设置所需的输出 pad 格式并检查返回值。)

Try formats do not depend on active formats, but can depend on the
current links configuration or sub-device controls value. For instance,
a low-pass noise filter might crop pixels at the frame boundaries,
modifying its output frame size.
(try 格式不依赖于活动格式，但可能依赖于当前的链路配置或子设备控制的值。
例如，低通噪声滤波器可能会在帧边界处裁剪像素，从而改变其输出帧大小。)

If the subdev device node has been registered in read-only mode, calls to
``VIDIOC_SUBDEV_S_FMT`` are only valid if the ``which`` field is set to
``V4L2_SUBDEV_FORMAT_TRY``, otherwise an error is returned and the errno
variable is set to ``-EPERM``.
(如果子设备节点以只读模式注册，则仅当 ``which`` 字段被设置为
``V4L2_SUBDEV_FORMAT_TRY`` 时，调用 ``VIDIOC_SUBDEV_S_FMT`` 才有效；否则将
返回错误，并把 errno 变量设置为 ``-EPERM``。)

Drivers must not return an error solely because the requested format
doesn't match the device capabilities. They must instead modify the
format to match what the hardware can provide. The modified format
should be as close as possible to the original request.
(驱动不得仅因为所请求的格式与设备能力不匹配而返回错误。它们必须改为修改
格式，使之与硬件所能提供的格式相匹配。修改后的格式应尽可能接近原始
请求。)

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_subdev_format

.. flat-table:: struct v4l2_subdev_format
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``pad``
      - Pad number as reported by the media controller API.
	(media controller API 报告的 pad 编号。)
    * - __u32
      - ``which``
      - Format to modified, from enum
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
	(要修改的格式，取自枚举
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`。)
    * - struct :c:type:`v4l2_mbus_framefmt`
      - ``format``
      - Definition of an image format, see :c:type:`v4l2_mbus_framefmt` for
	details.
	(图像格式的定义，详情参见 :c:type:`v4l2_mbus_framefmt`。)
    * - __u32
      - ``stream``
      - Stream identifier.
	(流标识符。)
    * - __u32
      - ``reserved``\ [7]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.
	(为未来扩展保留。应用程序和驱动必须将数组置零。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _v4l2-subdev-format-whence:

.. flat-table:: enum v4l2_subdev_format_whence
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - V4L2_SUBDEV_FORMAT_TRY
      - 0
      - Try formats, used for querying device capabilities.
	(try 格式，用于查询设备能力。)
    * - V4L2_SUBDEV_FORMAT_ACTIVE
      - 1
      - Active formats, applied to the hardware.
	(活动格式，应用于硬件。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EBUSY
    The format can't be changed because the pad is currently busy. This
    can be caused, for instance, by an active video stream on the pad.
    The ioctl must not be retried without performing another action to
    fix the problem first. Only returned by ``VIDIOC_SUBDEV_S_FMT``
    (无法更改格式，因为 pad 当前正忙。例如，这可能是由 pad 上活动的视频流
    造成的。必须先执行其他操作解决该问题，然后才能重试此 ioctl。仅由
    ``VIDIOC_SUBDEV_S_FMT`` 返回)

EINVAL
    The struct :c:type:`v4l2_subdev_format` ``pad`` references a non-existing
    pad, or the ``which`` field has an unsupported value.
    (struct :c:type:`v4l2_subdev_format` 的 ``pad`` 引用了不存在的 pad，
    或 ``which`` 字段的值不受支持。)

EPERM
    The ``VIDIOC_SUBDEV_S_FMT`` ioctl has been called on a read-only subdevice
    and the ``which`` field is set to ``V4L2_SUBDEV_FORMAT_ACTIVE``.
    (在只读子设备上调用了 ``VIDIOC_SUBDEV_S_FMT`` ioctl，且 ``which`` 字段
    被设置为 ``V4L2_SUBDEV_FORMAT_ACTIVE``。)

============

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
