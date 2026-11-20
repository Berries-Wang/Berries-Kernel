.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_DV_TIMINGS:

*****************************************************************************
ioctl VIDIOC_G_DV_TIMINGS, VIDIOC_S_DV_TIMINGS(获取或设置输入/输出的 DV 时序)
*****************************************************************************

Name(名称)
==========

VIDIOC_G_DV_TIMINGS - VIDIOC_S_DV_TIMINGS - VIDIOC_SUBDEV_G_DV_TIMINGS - VIDIOC_SUBDEV_S_DV_TIMINGS - Get or set DV timings for input or output
(VIDIOC_G_DV_TIMINGS - VIDIOC_S_DV_TIMINGS - VIDIOC_SUBDEV_G_DV_TIMINGS - VIDIOC_SUBDEV_S_DV_TIMINGS - 获取或设置输入或输出的 DV 时序)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_DV_TIMINGS

``int ioctl(int fd, VIDIOC_G_DV_TIMINGS, struct v4l2_dv_timings *argp)``

.. c:macro:: VIDIOC_S_DV_TIMINGS

``int ioctl(int fd, VIDIOC_S_DV_TIMINGS, struct v4l2_dv_timings *argp)``

.. c:macro:: VIDIOC_SUBDEV_G_DV_TIMINGS

``int ioctl(int fd, VIDIOC_SUBDEV_G_DV_TIMINGS, struct v4l2_dv_timings *argp)``

.. c:macro:: VIDIOC_SUBDEV_S_DV_TIMINGS

``int ioctl(int fd, VIDIOC_SUBDEV_S_DV_TIMINGS, struct v4l2_dv_timings *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_dv_timings`.
    (指向 struct :c:type:`v4l2_dv_timings` 的指针。)

Description(描述)
=================

To set DV timings for the input or output, applications use the
:ref:`VIDIOC_S_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>` ioctl and to get the current timings,
applications use the :ref:`VIDIOC_G_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>` ioctl. The detailed timing
information is filled in using the structure struct
:c:type:`v4l2_dv_timings`. These ioctls take a
pointer to the struct :c:type:`v4l2_dv_timings`
structure as argument. If the ioctl is not supported or the timing
values are not correct, the driver returns ``EINVAL`` error code.
(要设置输入或输出的 DV 时序，应用程序使用 :ref:`VIDIOC_S_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>` ioctl；要获取当前时序，应用程序使用 :ref:`VIDIOC_G_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>` ioctl。详细的时序信息通过 struct :c:type:`v4l2_dv_timings` 结构填写。这些 ioctl 接受指向 struct :c:type:`v4l2_dv_timings` 结构的指针作为参数。如果不支持该 ioctl 或时序值不正确，驱动返回 ``EINVAL`` 错误码。)

Calling ``VIDIOC_SUBDEV_S_DV_TIMINGS`` on a subdev device node that has been
registered in read-only mode is not allowed. An error is returned and the errno
variable is set to ``-EPERM``.
(不允许在以只读模式注册的 subdev 设备节点上调用 ``VIDIOC_SUBDEV_S_DV_TIMINGS``。此时会返回错误，并把 errno 变量设为 ``-EPERM``。)

The ``linux/v4l2-dv-timings.h`` header can be used to get the timings of
the formats in the :ref:`cea861` and :ref:`vesadmt` standards. If
the current input or output does not support DV timings (e.g. if
:ref:`VIDIOC_ENUMINPUT` does not set the
``V4L2_IN_CAP_DV_TIMINGS`` flag), then ``ENODATA`` error code is returned.
(``linux/v4l2-dv-timings.h`` 头文件可用于获取 :ref:`cea861` 与 :ref:`vesadmt` 标准中各格式的时序。如果当前输入或输出不支持 DV 时序（例如 :ref:`VIDIOC_ENUMINPUT` 未设置 ``V4L2_IN_CAP_DV_TIMINGS`` 标志），则返回 ``ENODATA`` 错误码。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    This ioctl is not supported, or the :ref:`VIDIOC_S_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>`
    parameter was unsuitable.
    (不支持该 ioctl，或 :ref:`VIDIOC_S_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>` 的参数不合适。)

ENODATA
    Digital video timings are not supported for this input or output.
    (该输入或输出不支持数字视频时序。)

EBUSY
    The device is busy and therefore can not change the timings.
    (设备忙，因此无法更改时序。)

EPERM
    ``VIDIOC_SUBDEV_S_DV_TIMINGS`` has been called on a read-only subdevice.
    (在只读 subdevice 上调用了 ``VIDIOC_SUBDEV_S_DV_TIMINGS``。)

.. c:type:: v4l2_bt_timings

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_bt_timings
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``width``
      - Width of the active video in pixels.
	(活动视频的宽度，以像素为单位。)
    * - __u32
      - ``height``
      - Height of the active video frame in lines. So for interlaced
	formats the height of the active video in each field is
	``height``/2.
	(活动视频帧的高度，以行为单位。因此对隔行格式，每个场中活动视频的高度为 ``height``/2。)
    * - __u32
      - ``interlaced``
      - Progressive (``V4L2_DV_PROGRESSIVE``) or interlaced (``V4L2_DV_INTERLACED``).
	(逐行（``V4L2_DV_PROGRESSIVE``）或隔行（``V4L2_DV_INTERLACED``）。)
    * - __u32
      - ``polarities``
      - This is a bit mask that defines polarities of sync signals. bit 0
	(``V4L2_DV_VSYNC_POS_POL``) is for vertical sync polarity and bit
	1 (``V4L2_DV_HSYNC_POS_POL``) is for horizontal sync polarity. If
	the bit is set (1) it is positive polarity and if is cleared (0),
	it is negative polarity.
	(这是一个定义同步信号极性的位掩码。位 0（``V4L2_DV_VSYNC_POS_POL``）是垂直同步极性，位 1（``V4L2_DV_HSYNC_POS_POL``）是水平同步极性。位被置位（1）为正极性，被清除（0）为负极性。)
    * - __u64
      - ``pixelclock``
      - Pixel clock in Hz. Ex. 74.25MHz->74250000
	(像素时钟，单位 Hz。例如 74.25MHz->74250000。)
    * - __u32
      - ``hfrontporch``
      - Horizontal front porch in pixels
	(水平前肩（front porch），以像素为单位。)
    * - __u32
      - ``hsync``
      - Horizontal sync length in pixels
	(水平同步长度，以像素为单位。)
    * - __u32
      - ``hbackporch``
      - Horizontal back porch in pixels
	(水平后肩（back porch），以像素为单位。)
    * - __u32
      - ``vfrontporch``
      - Vertical front porch in lines. For interlaced formats this refers
	to the odd field (aka field 1).
	(垂直前肩，以行为单位。对隔行格式指奇数场（即 field 1）。)
    * - __u32
      - ``vsync``
      - Vertical sync length in lines. For interlaced formats this refers
	to the odd field (aka field 1).
	(垂直同步长度，以行为单位。对隔行格式指奇数场（即 field 1）。)
    * - __u32
      - ``vbackporch``
      - Vertical back porch in lines. For interlaced formats this refers
	to the odd field (aka field 1).
	(垂直后肩，以行为单位。对隔行格式指奇数场（即 field 1）。)
    * - __u32
      - ``il_vfrontporch``
      - Vertical front porch in lines for the even field (aka field 2) of
	interlaced field formats. Must be 0 for progressive formats.
	(隔行场格式中偶数场（即 field 2）的垂直前肩，以行为单位。逐行格式必须为 0。)
    * - __u32
      - ``il_vsync``
      - Vertical sync length in lines for the even field (aka field 2) of
	interlaced field formats. Must be 0 for progressive formats.
	(隔行场格式中偶数场（即 field 2）的垂直同步长度，以行为单位。逐行格式必须为 0。)
    * - __u32
      - ``il_vbackporch``
      - Vertical back porch in lines for the even field (aka field 2) of
	interlaced field formats. Must be 0 for progressive formats.
	(隔行场格式中偶数场（即 field 2）的垂直后肩，以行为单位。逐行格式必须为 0。)
    * - __u32
      - ``standards``
      - The video standard(s) this format belongs to. This will be filled
	in by the driver. Applications must set this to 0. See
	:ref:`dv-bt-standards` for a list of standards.
	(该格式所属的视频标准。由驱动填写，应用程序必须设为 0。标准列表参见 :ref:`dv-bt-standards`。)
    * - __u32
      - ``flags``
      - Several flags giving more information about the format. See
	:ref:`dv-bt-flags` for a description of the flags.
	(提供该格式更多信息的若干标志。标志的说明参见 :ref:`dv-bt-flags`。)
    * - struct :c:type:`v4l2_fract`
      - ``picture_aspect``
      - The picture aspect if the pixels are not square. Only valid if the
        ``V4L2_DV_FL_HAS_PICTURE_ASPECT`` flag is set.
	(像素非正方形时的画面宽高比。仅在设置了 ``V4L2_DV_FL_HAS_PICTURE_ASPECT`` 标志时有效。)
    * - __u8
      - ``cea861_vic``
      - The Video Identification Code according to the CEA-861 standard.
        Only valid if the ``V4L2_DV_FL_HAS_CEA861_VIC`` flag is set.
	(符合 CEA-861 标准的视频识别码（VIC）。仅在设置了 ``V4L2_DV_FL_HAS_CEA861_VIC`` 标志时有效。)
    * - __u8
      - ``hdmi_vic``
      - The Video Identification Code according to the HDMI standard.
        Only valid if the ``V4L2_DV_FL_HAS_HDMI_VIC`` flag is set.
	(符合 HDMI 标准的视频识别码（VIC）。仅在设置了 ``V4L2_DV_FL_HAS_HDMI_VIC`` 标志时有效。)
    * - __u8
      - ``reserved[46]``
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
	(为将来扩展保留。驱动和应用程序必须把该数组置 0。)

.. tabularcolumns:: |p{3.5cm}|p{3.5cm}|p{7.0cm}|p{3.1cm}|

.. c:type:: v4l2_dv_timings

.. flat-table:: struct v4l2_dv_timings
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Type of DV timings as listed in :ref:`dv-timing-types`.
	(DV 时序类型，如 :ref:`dv-timing-types` 所列。)
    * - union {
      - (anonymous)
      - (匿名)
    * - struct :c:type:`v4l2_bt_timings`
      - ``bt``
      - Timings defined by BT.656/1120 specifications
	(BT.656/1120 规范定义的时序)
    * - __u32
      - ``reserved``\ [32]
      -
    * - }
      -

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. _dv-timing-types:

.. flat-table:: DV Timing types
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - Timing type(时序类型)
      - value(值)
      - Description(描述)
    * -
      -
      -
    * - ``V4L2_DV_BT_656_1120``
      - 0
      - BT.656/1120 timings
	(BT.656/1120 时序)

.. tabularcolumns:: |p{6.5cm}|p{11.0cm}|

.. cssclass:: longtable

.. _dv-bt-standards:

.. flat-table:: DV BT Timing standards
    :header-rows:  0
    :stub-columns: 0

    * - Timing standard(时序标准)
      - Description(描述)
    * - ``V4L2_DV_BT_STD_CEA861``
      - The timings follow the CEA-861 Digital TV Profile standard
	(时序遵循 CEA-861 Digital TV Profile 标准)
    * - ``V4L2_DV_BT_STD_DMT``
      - The timings follow the VESA Discrete Monitor Timings standard
	(时序遵循 VESA Discrete Monitor Timings 标准)
    * - ``V4L2_DV_BT_STD_CVT``
      - The timings follow the VESA Coordinated Video Timings standard
	(时序遵循 VESA Coordinated Video Timings 标准)
    * - ``V4L2_DV_BT_STD_GTF``
      - The timings follow the VESA Generalized Timings Formula standard
	(时序遵循 VESA Generalized Timings Formula 标准)
    * - ``V4L2_DV_BT_STD_SDI``
      - The timings follow the SDI Timings standard.
	There are no horizontal syncs/porches at all in this format.
	Total blanking timings must be set in hsync or vsync fields only.
	(时序遵循 SDI Timings 标准。该格式中完全没有水平同步/肩部。总消隐时序必须只填在 hsync 或 vsync 字段中。)

.. tabularcolumns:: |p{7.7cm}|p{9.8cm}|

.. cssclass:: longtable

.. _dv-bt-flags:

.. flat-table:: DV BT Timing flags
    :header-rows:  0
    :stub-columns: 0

    * - Flag(标志)
      - Description(描述)
    * - ``V4L2_DV_FL_REDUCED_BLANKING``
      - CVT/GTF specific: the timings use reduced blanking (CVT) or the
	'Secondary GTF' curve (GTF). In both cases the horizontal and/or
	vertical blanking intervals are reduced, allowing a higher
	resolution over the same bandwidth. This is a read-only flag,
	applications must not set this.
	(CVT/GTF 专属：时序使用消减消隐（CVT）或"次级 GTF"曲线（GTF）。两种情况下水平和/或垂直消隐区间都被缩减，从而在相同带宽下支持更高的分辨率。这是只读标志，应用程序不得设置。)
    * - ``V4L2_DV_FL_CAN_REDUCE_FPS``
      - CEA-861 specific: set for CEA-861 formats with a framerate that is
	a multiple of six. These formats can be optionally played at 1 /
	1.001 speed to be compatible with 60 Hz based standards such as
	NTSC and PAL-M that use a framerate of 29.97 frames per second. If
	the transmitter can't generate such frequencies, then the flag
	will also be cleared. This is a read-only flag, applications must
	not set this.
	(CEA-861 专属：对帧率为 6 的倍数的 CEA-861 格式设置。这些格式可以选择以 1/1.001 速度播放，以兼容基于 60 Hz、帧率为每秒 29.97 帧的 NTSC 与 PAL-M 等标准。如果发送端无法产生这样的频率，该标志也会被清除。这是只读标志，应用程序不得设置。)
    * - ``V4L2_DV_FL_REDUCED_FPS``
      - CEA-861 specific: only valid for video transmitters or video
        receivers that have the ``V4L2_DV_FL_CAN_DETECT_REDUCED_FPS``
	set. This flag is cleared otherwise. It is also only valid for
	formats with the ``V4L2_DV_FL_CAN_REDUCE_FPS`` flag set, for other
	formats the flag will be cleared by the driver.
	(CEA-861 专属：仅对设置了 ``V4L2_DV_FL_CAN_DETECT_REDUCED_FPS`` 的视频发送端或接收端有效，否则该标志被清除。它也只对设置了 ``V4L2_DV_FL_CAN_REDUCE_FPS`` 标志的格式有效；对其他格式，驱动会清除该标志。)

	If the application sets this flag for a transmitter, then the
	pixelclock used to set up the transmitter is divided by 1.001 to
	make it compatible with NTSC framerates. If the transmitter can't
	generate such frequencies, then the flag will be cleared.
	(如果应用程序为发送端设置该标志，则用于配置发送端的像素时钟会除以 1.001，以与 NTSC 帧率兼容。如果发送端无法产生这样的频率，该标志会被清除。)

	If a video receiver detects that the format uses a reduced framerate,
	then it will set this flag to signal this to the application.
	(如果视频接收端检测到该格式使用了缩减的帧率，它会设置该标志以向应用程序传递这一信息。)
    * - ``V4L2_DV_FL_HALF_LINE``
      - Specific to interlaced formats: if set, then the vertical
	frontporch of field 1 (aka the odd field) is really one half-line
	longer and the vertical backporch of field 2 (aka the even field)
	is really one half-line shorter, so each field has exactly the
	same number of half-lines. Whether half-lines can be detected or
	used depends on the hardware.
	(隔行格式专属：若设置，则 field 1（即奇数场）的垂直前肩实际上要长半个行，而 field 2（即偶数场）的垂直后肩实际上要短半个行，使每个场的半行数完全相同。能否检测或使用半行取决于硬件。)
    * - ``V4L2_DV_FL_IS_CE_VIDEO``
      - If set, then this is a Consumer Electronics (CE) video format.
	Such formats differ from other formats (commonly called IT
	formats) in that if R'G'B' encoding is used then by default the
	R'G'B' values use limited range (i.e. 16-235) as opposed to full
	range (i.e. 0-255). All formats defined in CEA-861 except for the
	640x480p59.94 format are CE formats.
	(若设置，则这是消费电子（CE）视频格式。这类格式与其他格式（通常称为 IT 格式）的区别在于：若使用 R'G'B' 编码，默认情况下 R'G'B' 值使用有限范围（即 16-235）而非全范围（即 0-255）。CEA-861 中定义的所有格式除 640x480p59.94 外都是 CE 格式。)
    * - ``V4L2_DV_FL_FIRST_FIELD_EXTRA_LINE``
      - Some formats like SMPTE-125M have an interlaced signal with a odd
	total height. For these formats, if this flag is set, the first
	field has the extra line. Else, it is the second field.
	(一些格式（如 SMPTE-125M）的隔行信号总高度为奇数。对这些格式，若设置了该标志，则第一场多出这一行；否则是第二场。)
    * - ``V4L2_DV_FL_HAS_PICTURE_ASPECT``
      - If set, then the picture_aspect field is valid. Otherwise assume that
        the pixels are square, so the picture aspect ratio is the same as the
	width to height ratio.
	(若设置，则 picture_aspect 字段有效。否则假定像素为正方形，即画面宽高比等于宽度与高度之比。)
    * - ``V4L2_DV_FL_HAS_CEA861_VIC``
      - If set, then the cea861_vic field is valid and contains the Video
        Identification Code as per the CEA-861 standard.
	(若设置，则 cea861_vic 字段有效，包含符合 CEA-861 标准的视频识别码。)
    * - ``V4L2_DV_FL_HAS_HDMI_VIC``
      - If set, then the hdmi_vic field is valid and contains the Video
        Identification Code as per the HDMI standard (HDMI Vendor Specific
	InfoFrame).
	(若设置，则 hdmi_vic 字段有效，包含符合 HDMI 标准（HDMI Vendor Specific InfoFrame）的视频识别码。)
    * - ``V4L2_DV_FL_CAN_DETECT_REDUCED_FPS``
      - CEA-861 specific: only valid for video receivers, the flag is
        cleared by transmitters.
        If set, then the hardware can detect the difference between
	regular framerates and framerates reduced by 1000/1001. E.g.:
	60 vs 59.94 Hz, 30 vs 29.97 Hz or 24 vs 23.976 Hz.
	(CEA-861 专属：仅对视频接收端有效，发送端会清除该标志。若设置，则硬件能够检测正常帧率与按 1000/1001 缩减的帧率之间的差异。例如：60 与 59.94 Hz、30 与 29.97 Hz 或 24 与 23.976 Hz。)
