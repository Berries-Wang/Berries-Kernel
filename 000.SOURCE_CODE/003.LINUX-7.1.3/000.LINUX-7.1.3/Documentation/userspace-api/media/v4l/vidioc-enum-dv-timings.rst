.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUM_DV_TIMINGS:

***********************************************************************************
ioctl VIDIOC_ENUM_DV_TIMINGS, VIDIOC_SUBDEV_ENUM_DV_TIMINGS(枚举支持的数字视频时序)
***********************************************************************************

Name(名称)
==========

VIDIOC_ENUM_DV_TIMINGS - VIDIOC_SUBDEV_ENUM_DV_TIMINGS - Enumerate supported Digital Video timings(VIDIOC_ENUM_DV_TIMINGS - VIDIOC_SUBDEV_ENUM_DV_TIMINGS - 枚举支持的数字视频时序)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUM_DV_TIMINGS

``int ioctl(int fd, VIDIOC_ENUM_DV_TIMINGS, struct v4l2_enum_dv_timings *argp)``

.. c:macro:: VIDIOC_SUBDEV_ENUM_DV_TIMINGS

``int ioctl(int fd, VIDIOC_SUBDEV_ENUM_DV_TIMINGS, struct v4l2_enum_dv_timings *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_enum_dv_timings`.
    (指向 struct :c:type:`v4l2_enum_dv_timings` 的指针。)

Description(描述)
=================

While some DV receivers or transmitters support a wide range of timings,
others support only a limited number of timings. With this ioctl
applications can enumerate a list of known supported timings. Call
:ref:`VIDIOC_DV_TIMINGS_CAP` to check if it
also supports other standards or even custom timings that are not in
this list.
(一些 DV 接收器或发送器支持很宽范围的时序，另一些则只支持有限数量的
时序。通过这个 ioctl，应用程序可以枚举一个已知支持时序的列表。调用
:ref:`VIDIOC_DV_TIMINGS_CAP` 以检查设备是否还支持其他标准，甚至不在此
列表中的自定义时序。)

To query the available timings, applications initialize the ``index``
field, set the ``pad`` field to 0, zero the reserved array of struct
:c:type:`v4l2_enum_dv_timings` and call the
``VIDIOC_ENUM_DV_TIMINGS`` ioctl on a video node with a pointer to this
structure. Drivers fill the rest of the structure or return an ``EINVAL``
error code when the index is out of bounds. To enumerate all supported
DV timings, applications shall begin at index zero, incrementing by one
until the driver returns ``EINVAL``.
(要查询可用的时序，应用程序初始化 ``index`` 字段，把 ``pad`` 字段设为
0，把 struct :c:type:`v4l2_enum_dv_timings` 的 reserved 数组清零，然后
以指向该结构体的指针在视频节点上调用 ``VIDIOC_ENUM_DV_TIMINGS``
ioctl。驱动程序填充结构体的其余部分；当索引越界时返回 ``EINVAL``
错误码。要枚举所有支持的 DV 时序，应用程序应从索引 0 开始，每次加 1，
直到驱动程序返回 ``EINVAL``。)

.. note::

   Drivers may enumerate a different set of DV timings after
   switching the video input or output.
   (切换视频输入或输出之后，驱动程序可能会枚举出另一组不同的 DV 时序。)

When implemented by the driver DV timings of subdevices can be queried
by calling the ``VIDIOC_SUBDEV_ENUM_DV_TIMINGS`` ioctl directly on a
subdevice node. The DV timings are specific to inputs (for DV receivers)
or outputs (for DV transmitters), applications must specify the desired
pad number in the struct
:c:type:`v4l2_enum_dv_timings` ``pad`` field.
Attempts to enumerate timings on a pad that doesn't support them will
return an ``EINVAL`` error code.
(如果驱动程序实现了该功能，可以通过直接在子设备节点上调用
``VIDIOC_SUBDEV_ENUM_DV_TIMINGS`` ioctl 来查询子设备的 DV 时序。
DV 时序特定于输入(DV 接收器)或输出(DV 发送器)，应用程序必须在
struct :c:type:`v4l2_enum_dv_timings` 的 ``pad`` 字段中指定期望的
pad 编号。在不支持时序的 pad 上枚举时序会返回 ``EINVAL`` 错误码。)

.. c:type:: v4l2_enum_dv_timings

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_enum_dv_timings
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Number of the DV timings, set by the application.
        (DV 时序的编号，由应用程序设置。)
    * - __u32
      - ``pad``
      - Pad number as reported by the media controller API. This field is
	only used when operating on a subdevice node. When operating on a
	video node applications must set this field to zero.
        (media controller API 报告的 pad 编号。此字段仅在操作子设备节点
        时使用。操作视频节点时，应用程序必须把此字段设为 0。)
    * - __u32
      - ``reserved``\ [2]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
        (保留用于未来的扩展。驱动程序和应用程序必须将此数组置零。)
    * - struct :c:type:`v4l2_dv_timings`
      - ``timings``
      - The timings.
        (时序。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_enum_dv_timings`
    ``index`` is out of bounds or the ``pad`` number is invalid.
    (struct :c:type:`v4l2_enum_dv_timings` 的 ``index`` 越界，或
    ``pad`` 编号无效。)

ENODATA
    Digital video presets are not supported for this input or output.
    (此输入或输出不支持数字视频预置。)
