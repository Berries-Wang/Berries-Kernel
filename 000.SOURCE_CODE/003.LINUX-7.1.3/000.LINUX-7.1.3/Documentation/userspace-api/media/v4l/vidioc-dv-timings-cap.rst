.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_DV_TIMINGS_CAP:

******************************************************************************************
ioctl VIDIOC_DV_TIMINGS_CAP, VIDIOC_SUBDEV_DV_TIMINGS_CAP(查询数字视频接收器/发送器的能力)
******************************************************************************************

Name(名称)
==========

VIDIOC_DV_TIMINGS_CAP - VIDIOC_SUBDEV_DV_TIMINGS_CAP - The capabilities of the Digital Video receiver/transmitter
(VIDIOC_DV_TIMINGS_CAP - VIDIOC_SUBDEV_DV_TIMINGS_CAP - 数字视频接收器/发送器的
能力)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_DV_TIMINGS_CAP

``int ioctl(int fd, VIDIOC_DV_TIMINGS_CAP, struct v4l2_dv_timings_cap *argp)``

.. c:macro:: VIDIOC_SUBDEV_DV_TIMINGS_CAP

``int ioctl(int fd, VIDIOC_SUBDEV_DV_TIMINGS_CAP, struct v4l2_dv_timings_cap *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_dv_timings_cap`.
    (指向 struct :c:type:`v4l2_dv_timings_cap` 的指针。)

Description(描述)
=================

To query the capabilities of the DV receiver/transmitter applications
initialize the ``pad`` field to 0, zero the reserved array of struct
:c:type:`v4l2_dv_timings_cap` and call the
``VIDIOC_DV_TIMINGS_CAP`` ioctl on a video node and the driver will fill
in the structure.
(要查询 DV 接收器/发送器的能力，应用程序应将 ``pad`` 字段初始化为 0，将
struct :c:type:`v4l2_dv_timings_cap` 的 ``reserved`` 数组清零，并在视频节点上
调用 ``VIDIOC_DV_TIMINGS_CAP`` ioctl，驱动程序会填充该结构体。)

.. note::

   Drivers may return different values after
   switching the video input or output.
   (驱动程序在切换视频输入或输出之后，可能返回不同的值。)

When implemented by the driver DV capabilities of subdevices can be
queried by calling the ``VIDIOC_SUBDEV_DV_TIMINGS_CAP`` ioctl directly
on a subdevice node. The capabilities are specific to inputs (for DV
receivers) or outputs (for DV transmitters), applications must specify
the desired pad number in the struct
:c:type:`v4l2_dv_timings_cap` ``pad`` field and
zero the ``reserved`` array. Attempts to query capabilities on a pad
that doesn't support them will return an ``EINVAL`` error code.
(当驱动程序实现该功能时，可以通过在子设备节点上直接调用
``VIDIOC_SUBDEV_DV_TIMINGS_CAP`` ioctl 来查询子设备的 DV 能力。这些能力特定于
输入（对 DV 接收器而言）或输出（对 DV 发送器而言），应用程序必须在 struct
:c:type:`v4l2_dv_timings_cap` 的 ``pad`` 字段中指定所需的 pad 编号，并将
``reserved`` 数组清零。在不支持能力查询的 pad 上查询能力将返回 ``EINVAL``
错误码。)

.. tabularcolumns:: |p{1.2cm}|p{3.2cm}|p{12.9cm}|

.. c:type:: v4l2_bt_timings_cap

.. flat-table:: struct v4l2_bt_timings_cap
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``min_width``
      - Minimum width of the active video in pixels.
	(活动视频的最小宽度，单位为像素。)
    * - __u32
      - ``max_width``
      - Maximum width of the active video in pixels.
	(活动视频的最大宽度，单位为像素。)
    * - __u32
      - ``min_height``
      - Minimum height of the active video in lines.
	(活动视频的最小高度，单位为行。)
    * - __u32
      - ``max_height``
      - Maximum height of the active video in lines.
	(活动视频的最大高度，单位为行。)
    * - __u64
      - ``min_pixelclock``
      - Minimum pixelclock frequency in Hz.
	(最小像素时钟频率，单位为 Hz。)
    * - __u64
      - ``max_pixelclock``
      - Maximum pixelclock frequency in Hz.
	(最大像素时钟频率，单位为 Hz。)
    * - __u32
      - ``standards``
      - The video standard(s) supported by the hardware. See
	:ref:`dv-bt-standards` for a list of standards.
	(硬件支持的视频制式。制式列表参见 :ref:`dv-bt-standards`。)
    * - __u32
      - ``capabilities``
      - Several flags giving more information about the capabilities. See
	:ref:`dv-bt-cap-capabilities` for a description of the flags.
	(给出更多能力信息的若干标志。标志的描述参见
	:ref:`dv-bt-cap-capabilities`。)
    * - __u32
      - ``reserved``\ [16]
      - Reserved for future extensions.
	Drivers must set the array to zero.
	(为未来的扩展保留。驱动程序必须将数组置零。)


.. tabularcolumns:: |p{4.4cm}|p{3.6cm}|p{9.3cm}|

.. c:type:: v4l2_dv_timings_cap

.. flat-table:: struct v4l2_dv_timings_cap
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Type of DV timings as listed in :ref:`dv-timing-types`.
	(DV 时序类型，如 :ref:`dv-timing-types` 所列。)
    * - __u32
      - ``pad``
      - Pad number as reported by the media controller API. This field is
	only used when operating on a subdevice node. When operating on a
	video node applications must set this field to zero.
	(media controller API 报告的 pad 编号。此字段仅在子设备节点上操作时
	使用。在视频节点上操作时，应用程序必须将此字段置零。)
    * - __u32
      - ``reserved``\ [2]
      - Reserved for future extensions.

	Drivers and applications must set the array to zero.
	(为未来的扩展保留。驱动程序和应用程序必须将数组置零。)
    * - union {
      - (anonymous)
    * - struct :c:type:`v4l2_bt_timings_cap`
      - ``bt``
      - BT.656/1120 timings capabilities of the hardware.
	(硬件的 BT.656/1120 时序能力。)
    * - __u32
      - ``raw_data``\ [32]
    * - }
      -

.. tabularcolumns:: |p{7.2cm}|p{10.3cm}|

.. _dv-bt-cap-capabilities:

.. flat-table:: DV BT Timing capabilities
    :header-rows:  0
    :stub-columns: 0

    * - Flag(标志)
      - Description(描述)
    * -
      -
    * - ``V4L2_DV_BT_CAP_INTERLACED``
      - Interlaced formats are supported.
	(支持隔行扫描格式。)
    * - ``V4L2_DV_BT_CAP_PROGRESSIVE``
      - Progressive formats are supported.
	(支持逐行扫描格式。)
    * - ``V4L2_DV_BT_CAP_REDUCED_BLANKING``
      - CVT/GTF specific: the timings can make use of reduced blanking
	(CVT) or the 'Secondary GTF' curve (GTF).
	(CVT/GTF 特有：时序可以使用缩减消隐（CVT）或 "Secondary GTF"
	曲线（GTF）。)
    * - ``V4L2_DV_BT_CAP_CUSTOM``
      - Can support non-standard timings, i.e. timings not belonging to
	the standards set in the ``standards`` field.
	(可以支持非标准时序，即不属于 ``standards`` 字段所设定制式的
	时序。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
