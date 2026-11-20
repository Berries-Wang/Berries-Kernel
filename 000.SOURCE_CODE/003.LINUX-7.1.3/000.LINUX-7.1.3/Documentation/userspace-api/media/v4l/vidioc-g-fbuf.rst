.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_FBUF:

***********************************************************
ioctl VIDIOC_G_FBUF, VIDIOC_S_FBUF(获取/设置帧缓冲覆盖参数)
***********************************************************

Name(名称)
==========

VIDIOC_G_FBUF - VIDIOC_S_FBUF - Get or set frame buffer overlay parameters
(VIDIOC_G_FBUF - VIDIOC_S_FBUF - 获取或设置帧缓冲覆盖参数)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_FBUF

``int ioctl(int fd, VIDIOC_G_FBUF, struct v4l2_framebuffer *argp)``

.. c:macro:: VIDIOC_S_FBUF

``int ioctl(int fd, VIDIOC_S_FBUF, const struct v4l2_framebuffer *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_framebuffer`.
    (指向 struct :c:type:`v4l2_framebuffer` 的指针。)

Description(描述)
=================

Applications can use the :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` and :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctl
to get and set the framebuffer parameters for a
:ref:`Video Overlay <overlay>` or :ref:`Video Output Overlay <osd>`
(OSD). The type of overlay is implied by the device type (capture or
output device) and can be determined with the
:ref:`VIDIOC_QUERYCAP` ioctl. One ``/dev/videoN``
device must not support both kinds of overlay.
(应用程序可以使用 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` 和 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` ioctl 来获取和设置 :ref:`Video Overlay <overlay>`（视频覆盖）或 :ref:`Video Output Overlay <osd>`（OSD，视频输出覆盖）的帧缓冲参数。覆盖的类型由设备类型（采集设备或输出设备）隐含决定，可以用 :ref:`VIDIOC_QUERYCAP` ioctl 确定。一个 ``/dev/videoN`` 设备不得同时支持两种覆盖。)

The V4L2 API distinguishes destructive and non-destructive overlays. A
destructive overlay copies captured video images into the video memory
of a graphics card. A non-destructive overlay blends video images into a
VGA signal or graphics into a video signal. *Video Output Overlays* are
always non-destructive.
(V4L2 API 区分破坏性覆盖与非破坏性覆盖。破坏性覆盖把采集到的视频图像复制进显卡的视频内存；非破坏性覆盖则把视频图像与 VGA 信号混合，或把图形与视频信号混合。*Video Output Overlays*（视频输出覆盖）始终是非破坏性的。)

Destructive overlay support has been removed: with modern GPUs and CPUs
this is no longer needed, and it was always a very dangerous feature.
(破坏性覆盖的支持已被移除：在现代 GPU 和 CPU 下它已无必要，而且它一直是一个非常危险的功能。)

To get the current parameters applications call the :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>`
ioctl with a pointer to a struct :c:type:`v4l2_framebuffer`
structure. The driver fills all fields of the structure or returns an
EINVAL error code when overlays are not supported.
(要获取当前参数，应用程序调用 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` ioctl，并传入指向 struct :c:type:`v4l2_framebuffer` 结构的指针。驱动会填充该结构的所有字段；如果不支持覆盖，则返回 EINVAL 错误码。)

To set the parameters for a *Video Output Overlay*, applications must
initialize the ``flags`` field of a struct
:c:type:`v4l2_framebuffer`. Since the framebuffer is
implemented on the TV card all other parameters are determined by the
driver. When an application calls :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` with a pointer to
this structure, the driver prepares for the overlay and returns the
framebuffer parameters as :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` does, or it returns an error
code.
(要设置 *Video Output Overlay*（视频输出覆盖）的参数，应用程序必须初始化 struct :c:type:`v4l2_framebuffer` 的 ``flags`` 字段。由于帧缓冲是在电视卡上实现的，其余所有参数都由驱动决定。当应用程序带指向该结构的指针调用 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` 时，驱动为覆盖做准备，并像 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` 那样返回帧缓冲参数，或返回一个错误码。)

To set the parameters for a *Video Capture Overlay*
applications must initialize the ``flags`` field, the ``fmt``
substructure, and call :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>`. Again the driver prepares for
the overlay and returns the framebuffer parameters as :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>`
does, or it returns an error code.
(要设置 *Video Capture Overlay*（视频采集覆盖）的参数，应用程序必须初始化 ``flags`` 字段和 ``fmt`` 子结构，并调用 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>`。同样，驱动会为覆盖做准备，并像 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` 那样返回帧缓冲参数，或返回一个错误码。)

.. tabularcolumns:: |p{3.5cm}|p{3.5cm}|p{3.5cm}|p{6.6cm}|

.. c:type:: v4l2_framebuffer

.. cssclass:: longtable

.. flat-table:: struct v4l2_framebuffer
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 1 2

    * - __u32
      - ``capability``
      -
      - Overlay capability flags set by the driver, see
	:ref:`framebuffer-cap`.
	(由驱动设置的覆盖能力标志，参见 :ref:`framebuffer-cap`。)
    * - __u32
      - ``flags``
      -
      - Overlay control flags set by application and driver, see
	:ref:`framebuffer-flags`
	(由应用程序和驱动设置的覆盖控制标志，参见 :ref:`framebuffer-flags`。)
    * - void *
      - ``base``
      -
      - Physical base address of the framebuffer, that is the address of
	the pixel in the top left corner of the framebuffer.
	For :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` this field is no longer supported
	and the kernel will always set this to NULL.
	For *Video Output Overlays*
	the driver will return a valid base address, so applications can
	find the corresponding Linux framebuffer device (see
	:ref:`osd`). For *Video Capture Overlays* this field will always be
	NULL.
	(帧缓冲的物理基地址，即帧缓冲左上角像素的地址。对 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` 而言，该字段已不再受支持，内核总是把它设为 NULL。对 *Video Output Overlays*，驱动会返回有效的基地址，应用程序因此能找到对应的 Linux 帧缓冲设备（参见 :ref:`osd`）。对 *Video Capture Overlays*，该字段将始终为 NULL。)
    * - struct
      - ``fmt``
      -
      - Layout of the frame buffer.
	(帧缓冲的布局。)
    * -
      - __u32
      - ``width``
      - Width of the frame buffer in pixels.
	(帧缓冲的宽度，以像素为单位。)
    * -
      - __u32
      - ``height``
      - Height of the frame buffer in pixels.
	(帧缓冲的高度，以像素为单位。)
    * -
      - __u32
      - ``pixelformat``
      - The pixel format of the framebuffer.
	(帧缓冲的像素格式。)
    * -
      -
      -
      - For *non-destructive Video Overlays* this field only defines a
	format for the struct :c:type:`v4l2_window`
	``chromakey`` field.
	(对 *非破坏性 Video Overlays*，该字段只为 struct :c:type:`v4l2_window` 的 ``chromakey`` 字段定义一个格式。)
    * -
      -
      -
      - For *Video Output Overlays* the driver must return a valid
	format.
	(对 *Video Output Overlays*，驱动必须返回有效的格式。)
    * -
      -
      -
      - Usually this is an RGB format (for example
	:ref:`V4L2_PIX_FMT_RGB565 <V4L2-PIX-FMT-RGB565>`) but YUV
	formats (only packed YUV formats when chroma keying is used, not
	including ``V4L2_PIX_FMT_YUYV`` and ``V4L2_PIX_FMT_UYVY``) and the
	``V4L2_PIX_FMT_PAL8`` format are also permitted. The behavior of
	the driver when an application requests a compressed format is
	undefined. See :ref:`pixfmt` for information on pixel formats.
	(通常这是一个 RGB 格式，例如 :ref:`V4L2_PIX_FMT_RGB565 <V4L2-PIX-FMT-RGB565>`；但也允许 YUV 格式（使用色键时仅限打包 YUV 格式，且不包括 ``V4L2_PIX_FMT_YUYV`` 与 ``V4L2_PIX_FMT_UYVY``），以及 ``V4L2_PIX_FMT_PAL8`` 格式。应用程序请求压缩格式时，驱动的行为未定义。像素格式的信息参见 :ref:`pixfmt`。)
    * -
      - enum :c:type:`v4l2_field`
      - ``field``
      - Drivers and applications shall ignore this field. If applicable,
	the field order is selected with the
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl, using the ``field``
	field of struct :c:type:`v4l2_window`.
	(驱动和应用程序应忽略该字段。若适用，场序通过 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 选择，使用的是 struct :c:type:`v4l2_window` 的 ``field`` 字段。)
    * -
      - __u32
      - ``bytesperline``
      - Distance in bytes between the leftmost pixels in two adjacent
	lines.
	(相邻两行最左侧像素之间以字节计的距离。)
    * - :cspan:`3`

	This field is irrelevant to *non-destructive Video Overlays*.
	(该字段对 *非破坏性 Video Overlays* 无意义。)

	For *Video Output Overlays* the driver must return a valid value.
	(对 *Video Output Overlays*，驱动必须返回有效的值。)

	Video hardware may access padding bytes, therefore they must
	reside in accessible memory. Consider for example the case where
	padding bytes after the last line of an image cross a system page
	boundary. Capture devices may write padding bytes, the value is
	undefined. Output devices ignore the contents of padding bytes.
	(视频硬件可能会访问填充字节，因此它们必须位于可访问的内存中。例如考虑这种情况：图像最后一行之后的填充字节跨越了系统页边界。采集设备可能会写填充字节，其值未定义；输出设备则忽略填充字节的内容。)

	When the image format is planar the ``bytesperline`` value applies
	to the first plane and is divided by the same factor as the
	``width`` field for the other planes. For example the Cb and Cr
	planes of a YUV 4:2:0 image have half as many padding bytes
	following each line as the Y plane. To avoid ambiguities drivers
	must return a ``bytesperline`` value rounded up to a multiple of
	the scale factor.
	(当图像格式为平面（planar）格式时，``bytesperline`` 值适用于第一个平面；对其余平面，该值按 ``width`` 字段相同的因子相除。例如 YUV 4:2:0 图像的 Cb 与 Cr 平面，每行之后的填充字节数是 Y 平面的一半。为避免歧义，驱动返回的 ``bytesperline`` 值必须向上取整到缩放因子的倍数。)
    * -
      - __u32
      - ``sizeimage``
      - This field is irrelevant to *non-destructive Video Overlays*.
	For *Video Output Overlays* the driver must return a valid
	format.
	(该字段对 *非破坏性 Video Overlays* 无意义。对 *Video Output Overlays*，驱动必须返回有效的格式。)

	Together with ``base`` it defines the framebuffer memory
	accessible by the driver.
	(它与 ``base`` 一起定义驱动可访问的帧缓冲内存。)
    * -
      - enum :c:type:`v4l2_colorspace`
      - ``colorspace``
      - This information supplements the ``pixelformat`` and must be set
	by the driver, see :ref:`colorspaces`.
	(该信息是对 ``pixelformat`` 的补充，必须由驱动设置，参见 :ref:`colorspaces`。)
    * -
      - __u32
      - ``priv``
      - Reserved. Drivers and applications must set this field to zero.
	(保留。驱动和应用程序必须把该字段设为 0。)

.. tabularcolumns:: |p{7.4cm}|p{1.6cm}|p{8.3cm}|

.. _framebuffer-cap:

.. flat-table:: Frame Buffer Capability Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_FBUF_CAP_EXTERNOVERLAY``
      - 0x0001
      - The device is capable of non-destructive overlays. When the driver
	clears this flag, only destructive overlays are supported. There
	are no drivers yet which support both destructive and
	non-destructive overlays. Video Output Overlays are in practice
	always non-destructive.
	(设备支持非破坏性覆盖。当驱动清除该标志时，只支持破坏性覆盖。目前尚无同时支持破坏性与非破坏性覆盖的驱动。Video Output Overlays 实际上总是非破坏性的。)
    * - ``V4L2_FBUF_CAP_CHROMAKEY``
      - 0x0002
      - The device supports clipping by chroma-keying the images. That is,
	image pixels replace pixels in the VGA or video signal only where
	the latter assume a certain color. Chroma-keying makes no sense
	for destructive overlays.
	(设备支持通过色键（chroma-keying）对图像进行剪裁。也就是说，仅当 VGA 或视频信号中的像素呈某种特定颜色时，图像像素才会替换它们。色键对破坏性覆盖毫无意义。)
    * - ``V4L2_FBUF_CAP_LIST_CLIPPING``
      - 0x0004
      - The device supports clipping using a list of clip rectangles.
        Note that this is no longer supported.
	(设备支持使用剪裁矩形列表进行剪裁。注意，这已不再受支持。)
    * - ``V4L2_FBUF_CAP_BITMAP_CLIPPING``
      - 0x0008
      - The device supports clipping using a bit mask.
        Note that this is no longer supported.
	(设备支持使用位掩码进行剪裁。注意，这已不再受支持。)
    * - ``V4L2_FBUF_CAP_LOCAL_ALPHA``
      - 0x0010
      - The device supports clipping/blending using the alpha channel of
	the framebuffer or VGA signal. Alpha blending makes no sense for
	destructive overlays.
	(设备支持使用帧缓冲或 VGA 信号的 alpha 通道进行剪裁/混合。alpha 混合对破坏性覆盖毫无意义。)
    * - ``V4L2_FBUF_CAP_GLOBAL_ALPHA``
      - 0x0020
      - The device supports alpha blending using a global alpha value.
	Alpha blending makes no sense for destructive overlays.
	(设备支持使用全局 alpha 值进行 alpha 混合。alpha 混合对破坏性覆盖毫无意义。)
    * - ``V4L2_FBUF_CAP_LOCAL_INV_ALPHA``
      - 0x0040
      - The device supports clipping/blending using the inverted alpha
	channel of the framebuffer or VGA signal. Alpha blending makes no
	sense for destructive overlays.
	(设备支持使用帧缓冲或 VGA 信号的反转 alpha 通道进行剪裁/混合。alpha 混合对破坏性覆盖毫无意义。)
    * - ``V4L2_FBUF_CAP_SRC_CHROMAKEY``
      - 0x0080
      - The device supports Source Chroma-keying. Video pixels with the
	chroma-key colors are replaced by framebuffer pixels, which is
	exactly opposite of ``V4L2_FBUF_CAP_CHROMAKEY``
	(设备支持源色键（Source Chroma-keying）。具有色键颜色的视频像素被帧缓冲像素替换，这与 ``V4L2_FBUF_CAP_CHROMAKEY`` 恰好相反。)

.. tabularcolumns:: |p{7.4cm}|p{1.6cm}|p{8.3cm}|

.. _framebuffer-flags:

.. cssclass:: longtable

.. flat-table:: Frame Buffer Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_FBUF_FLAG_PRIMARY``
      - 0x0001
      - The framebuffer is the primary graphics surface. In other words,
	the overlay is destructive. This flag is typically set by any
	driver that doesn't have the ``V4L2_FBUF_CAP_EXTERNOVERLAY``
	capability and it is cleared otherwise.
	(该帧缓冲是主图形表面。换句话说，覆盖是破坏性的。该标志通常由任何不具备 ``V4L2_FBUF_CAP_EXTERNOVERLAY`` 能力的驱动设置，否则被清除。)
    * - ``V4L2_FBUF_FLAG_OVERLAY``
      - 0x0002
      - If this flag is set for a video capture device, then the driver
	will set the initial overlay size to cover the full framebuffer
	size, otherwise the existing overlay size (as set by
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`) will be used. Only one
	video capture driver (bttv) supports this flag. The use of this
	flag for capture devices is deprecated. There is no way to detect
	which drivers support this flag, so the only reliable method of
	setting the overlay size is through
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`. If this flag is set for a
	video output device, then the video output overlay window is
	relative to the top-left corner of the framebuffer and restricted
	to the size of the framebuffer. If it is cleared, then the video
	output overlay window is relative to the video output display.
	(如果对视频采集设备设置该标志，驱动会把初始覆盖大小设为覆盖整个帧缓冲，否则使用现有的覆盖大小（由 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 设置）。只有一个视频采集驱动（bttv）支持该标志；对采集设备使用该标志已被废弃。无法检测哪些驱动支持该标志，因此设置覆盖大小的唯一可靠方法是通过 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`。如果对视频输出设备设置该标志，则视频输出覆盖窗口相对帧缓冲左上角，并限制在帧缓冲大小之内。如果清除该标志，则视频输出覆盖窗口相对视频输出显示。)
    * - ``V4L2_FBUF_FLAG_CHROMAKEY``
      - 0x0004
      - Use chroma-keying. The chroma-key color is determined by the
	``chromakey`` field of struct :c:type:`v4l2_window`
	and negotiated with the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
	ioctl, see :ref:`overlay` and :ref:`osd`.
	(使用色键。色键颜色由 struct :c:type:`v4l2_window` 的 ``chromakey`` 字段决定，并通过 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 协商，参见 :ref:`overlay` 和 :ref:`osd`。)
    * - :cspan:`2` There are no flags to enable clipping using a list of
	clip rectangles or a bitmap. These methods are negotiated with the
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl, see :ref:`overlay`
	and :ref:`osd`.
	(没有用于启用"剪裁矩形列表剪裁"或"位掩码剪裁"的标志。这些方法通过 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 协商，参见 :ref:`overlay` 和 :ref:`osd`。)
    * - ``V4L2_FBUF_FLAG_LOCAL_ALPHA``
      - 0x0008
      - Use the alpha channel of the framebuffer to clip or blend
	framebuffer pixels with video images. The blend function is:
	output = framebuffer pixel * alpha + video pixel * (1 - alpha).
	The actual alpha depth depends on the framebuffer pixel format.
	(使用帧缓冲的 alpha 通道对帧缓冲像素与视频图像进行剪裁或混合。混合函数为：output = framebuffer pixel * alpha + video pixel * (1 - alpha)。实际的 alpha 深度取决于帧缓冲像素格式。)
    * - ``V4L2_FBUF_FLAG_GLOBAL_ALPHA``
      - 0x0010
      - Use a global alpha value to blend the framebuffer with video
	images. The blend function is: output = (framebuffer pixel * alpha
	+ video pixel * (255 - alpha)) / 255. The alpha value is
	determined by the ``global_alpha`` field of struct
	:c:type:`v4l2_window` and negotiated with the
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl, see :ref:`overlay`
	and :ref:`osd`.
	(使用全局 alpha 值把帧缓冲与视频图像混合。混合函数为：output = (framebuffer pixel * alpha + video pixel * (255 - alpha)) / 255。alpha 值由 struct :c:type:`v4l2_window` 的 ``global_alpha`` 字段决定，并通过 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 协商，参见 :ref:`overlay` 和 :ref:`osd`。)
    * - ``V4L2_FBUF_FLAG_LOCAL_INV_ALPHA``
      - 0x0020
      - Like ``V4L2_FBUF_FLAG_LOCAL_ALPHA``, use the alpha channel of the
	framebuffer to clip or blend framebuffer pixels with video images,
	but with an inverted alpha value. The blend function is: output =
	framebuffer pixel * (1 - alpha) + video pixel * alpha. The actual
	alpha depth depends on the framebuffer pixel format.
	(与 ``V4L2_FBUF_FLAG_LOCAL_ALPHA`` 类似，使用帧缓冲的 alpha 通道对帧缓冲像素与视频图像进行剪裁或混合，但 alpha 值是反转的。混合函数为：output = framebuffer pixel * (1 - alpha) + video pixel * alpha。实际的 alpha 深度取决于帧缓冲像素格式。)
    * - ``V4L2_FBUF_FLAG_SRC_CHROMAKEY``
      - 0x0040
      - Use source chroma-keying. The source chroma-key color is
	determined by the ``chromakey`` field of struct
	:c:type:`v4l2_window` and negotiated with the
	:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl, see :ref:`overlay`
	and :ref:`osd`. Both chroma-keying are mutual exclusive to each
	other, so same ``chromakey`` field of struct
	:c:type:`v4l2_window` is being used.
	(使用源色键。源色键颜色由 struct :c:type:`v4l2_window` 的 ``chromakey`` 字段决定，并通过 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 协商，参见 :ref:`overlay` 和 :ref:`osd`。两种色键互斥，因此使用的是 struct :c:type:`v4l2_window` 的同一个 ``chromakey`` 字段。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EPERM
    :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` can only be called by a privileged user to
    negotiate the parameters for a destructive overlay.
    (只有特权用户才能调用 :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` 来协商破坏性覆盖的参数。)

EINVAL
    The :ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` parameters are unsuitable.
    (:ref:`VIDIOC_S_FBUF <VIDIOC_G_FBUF>` 的参数不合适。)
