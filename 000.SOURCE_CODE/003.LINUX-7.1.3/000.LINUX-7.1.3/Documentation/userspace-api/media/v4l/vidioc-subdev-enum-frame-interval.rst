.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL:

***************************************
ioctl VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL
***************************************

Name(名称)
==========

VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL - Enumerate frame intervals
(VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL - 枚举帧间隔。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL

``int ioctl(int fd, VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL, struct v4l2_subdev_frame_interval_enum * argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_subdev_frame_interval_enum`.
    (指向 struct :c:type:`v4l2_subdev_frame_interval_enum` 的指针。)

Description(描述)
=================

This ioctl lets applications enumerate available frame intervals on a
given sub-device pad. Frame intervals only makes sense for sub-devices
that can control the frame period on their own. This includes, for
instance, image sensors and TV tuners.
(此 ioctl 让应用程序枚举给定子设备 pad 上可用的帧间隔。帧间隔只对能够自行控制帧周期的子设备有意义。例如，这包括图像传感器和电视调谐器。)

For the common use case of image sensors, the frame intervals available
on the sub-device output pad depend on the frame format and size on the
same pad. Applications must thus specify the desired format and size
when enumerating frame intervals.
(对于图像传感器这一常见使用场景，子设备输出 pad 上可用的帧间隔取决于同一 pad 上的帧格式和尺寸。因此，应用程序在枚举帧间隔时必须指定期望的格式和尺寸。)

To enumerate frame intervals applications initialize the ``index``,
``pad``, ``which``, ``code``, ``width`` and ``height`` fields of struct
:c:type:`v4l2_subdev_frame_interval_enum`
and call the :ref:`VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL` ioctl with a pointer
to this structure. Drivers fill the rest of the structure or return an
EINVAL error code if one of the input fields is invalid. All frame
intervals are enumerable by beginning at index zero and incrementing by
one until ``EINVAL`` is returned.
(要枚举帧间隔，应用程序初始化 struct :c:type:`v4l2_subdev_frame_interval_enum` 的 ``index`` 、 ``pad`` 、 ``which`` 、 ``code`` 、 ``width`` 和 ``height`` 字段，并以指向该结构的指针调用 :ref:`VIDIOC_SUBDEV_ENUM_FRAME_INTERVAL` ioctl。驱动程序填充结构的其余部分；如果某个输入字段无效，则返回 EINVAL 错误码。所有帧间隔都可以通过从索引 0 开始、每次加 1 直到返回 ``EINVAL`` 来枚举。)

Available frame intervals may depend on the current 'try' formats at
other pads of the sub-device, as well as on the current active links.
See :ref:`VIDIOC_SUBDEV_G_FMT` for more
information about the try formats.
(可用的帧间隔可能取决于子设备其他 pad 上当前的 'try' 格式，以及当前的活动链路。有关 try 格式的更多信息，参见 :ref:`VIDIOC_SUBDEV_G_FMT` 。)

Sub-devices that support the frame interval enumeration ioctl should
implemented it on a single pad only. Its behaviour when supported on
multiple pads of the same sub-device is not defined.
(支持帧间隔枚举 ioctl 的子设备应只在单个 pad 上实现它。当在同一子设备的多个 pad 上支持它时，其行为未定义。)

.. c:type:: v4l2_subdev_frame_interval_enum

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_subdev_frame_interval_enum
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Number of the format in the enumeration, set by the application.
        (枚举中格式的编号，由应用程序设置。)
    * - __u32
      - ``pad``
      - Pad number as reported by the media controller API.
        (媒体控制器 API 所报告的 pad 编号。)
    * - __u32
      - ``code``
      - The media bus format code, as defined in
	:ref:`v4l2-mbus-format`.
        (媒体总线格式代码，定义见 :ref:`v4l2-mbus-format` 。)
    * - __u32
      - ``width``
      - Frame width, in pixels.
        (帧宽度，以像素为单位。)
    * - __u32
      - ``height``
      - Frame height, in pixels.
        (帧高度，以像素为单位。)
    * - struct :c:type:`v4l2_fract`
      - ``interval``
      - Period, in seconds, between consecutive video frames.
        (连续视频帧之间的周期，以秒为单位。)
    * - __u32
      - ``which``
      - Frame intervals to be enumerated, from enum
	:ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>`.
        (要枚举的帧间隔，来自 enum :ref:`v4l2_subdev_format_whence <v4l2-subdev-format-whence>` 。)
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

EINVAL
    The struct :c:type:`v4l2_subdev_frame_interval_enum` ``pad`` references a
    non-existing pad, the ``which`` field has an unsupported value, one of the
    ``code``, ``width`` or ``height`` fields are invalid for the given pad, or
    the ``index`` field is out of bounds.
    (struct :c:type:`v4l2_subdev_frame_interval_enum` 的 ``pad`` 引用了不存在的 pad、``which`` 字段的值不受支持、``code`` 、 ``width`` 或 ``height`` 字段之一对给定 pad 无效，或 ``index`` 字段越界。)
