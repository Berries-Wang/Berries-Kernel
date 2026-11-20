.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUM_FMT:

********************************************
ioctl VIDIOC_ENUM_FMT(ioctl VIDIOC_ENUM_FMT)
********************************************

Name(名称)
==========

VIDIOC_ENUM_FMT - Enumerate image formats
(VIDIOC_ENUM_FMT - 枚举图像格式)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUM_FMT

``int ioctl(int fd, VIDIOC_ENUM_FMT, struct v4l2_fmtdesc *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_fmtdesc`.
    (指向 struct :c:type:`v4l2_fmtdesc` 的指针。)

Description(描述)
=================

To enumerate image formats applications initialize the ``type``, ``mbus_code``
and ``index`` fields of struct :c:type:`v4l2_fmtdesc` and call
the :ref:`VIDIOC_ENUM_FMT` ioctl with a pointer to this structure. Drivers
fill the rest of the structure or return an ``EINVAL`` error code. All
formats are enumerable by beginning at index zero and incrementing by
one until ``EINVAL`` is returned. If applicable, drivers shall return
formats in preference order, where preferred formats are returned before
(that is, with lower ``index`` value) less-preferred formats.
(为了枚举图像格式，应用程序初始化 struct :c:type:`v4l2_fmtdesc` 的
``type``、``mbus_code`` 与 ``index`` 字段，然后以指向该结构的指针调用
:ref:`VIDIOC_ENUM_FMT` ioctl。驱动程序填充结构的其余部分，或返回 ``EINVAL``
错误码。所有格式都可以通过从索引 0 开始、每次加 1 递增直到返回 ``EINVAL`` 的
方式来枚举。如果适用，驱动程序应按优先顺序返回格式，较优的格式先于(即
``index`` 值较小)较次的格式返回。)

Depending on the ``V4L2_CAP_IO_MC`` :ref:`capability <device-capabilities>`,
the ``mbus_code`` field is handled differently:
(根据 ``V4L2_CAP_IO_MC`` :ref:`capability <device-capabilities>` 能力的有无，
``mbus_code`` 字段的处理方式有所不同：)

1) ``V4L2_CAP_IO_MC`` is not set (also known as a 'video-node-centric' driver)
   (``V4L2_CAP_IO_MC`` 未设置的情况(也称为 'video-node-centric'(以视频节点
   为中心)的驱动程序)。)

   Applications shall initialize the ``mbus_code`` field to zero and drivers
   shall ignore the value of the field.
   (应用程序应把 ``mbus_code`` 字段初始化为 0，驱动程序则应忽略该字段的值。)

   Drivers shall enumerate all image formats.
   (驱动程序应枚举所有图像格式。)

   .. note::

      After switching the input or output the list of enumerated image
      formats may be different.
      (切换输入或输出之后，枚举出的图像格式列表可能有所不同。)

2) ``V4L2_CAP_IO_MC`` is set (also known as an 'MC-centric' driver)
   (``V4L2_CAP_IO_MC`` 已设置的情况(也称为 'MC-centric'(以 MC 为中心)的驱动
   程序)。)

   If the ``mbus_code`` field is zero, then all image formats
   shall be enumerated.
   (如果 ``mbus_code`` 字段为 0，则应枚举所有图像格式。)

   If the ``mbus_code`` field is initialized to a valid (non-zero)
   :ref:`media bus format code <v4l2-mbus-pixelcode>`, then drivers
   shall restrict enumeration to only the image formats that can produce
   (for video output devices) or be produced from (for video capture
   devices) that media bus code. If the ``mbus_code`` is unsupported by
   the driver, then ``EINVAL`` shall be returned.
   (如果 ``mbus_code`` 字段被初始化为一个有效的(非 0)
   :ref:`media bus format code <v4l2-mbus-pixelcode>`，则驱动程序应把枚举
   限制为仅能产生(对视频输出设备而言)或由其产生(对视频捕获设备而言)该
   media bus 码的那些图像格式。如果驱动程序不支持该 ``mbus_code``，则应返回
   ``EINVAL``。)

   Regardless of the value of the ``mbus_code`` field, the enumerated image
   formats shall not depend on the active configuration of the video device
   or device pipeline.
   (无论 ``mbus_code`` 字段的取值如何，枚举出的图像格式都不应依赖于视频设备或
   设备管线的当前配置。)

.. c:type:: v4l2_fmtdesc

.. cssclass:: longtable

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_fmtdesc
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Number of the format in the enumeration, set by the application.
        This is in no way related to the ``pixelformat`` field.
        When the index is ORed with ``V4L2_FMTDESC_FLAG_ENUM_ALL`` the
        driver clears the flag and enumerates all the possible formats,
        ignoring any limitations from the current configuration. Drivers
        which do not support this flag always return an ``EINVAL``
        error code without clearing this flag.
        Formats enumerated when using ``V4L2_FMTDESC_FLAG_ENUM_ALL`` flag
        shouldn't be used when calling :c:func:`VIDIOC_ENUM_FRAMESIZES`
        or :c:func:`VIDIOC_ENUM_FRAMEINTERVALS`.
        ``V4L2_FMTDESC_FLAG_ENUM_ALL`` should only be used by drivers that
        can return different format list depending on this flag.
        (枚举中格式的编号，由应用程序设置。它与 ``pixelformat`` 字段毫无关系。
        当 index 与 ``V4L2_FMTDESC_FLAG_ENUM_ALL`` 按位或时，驱动程序会清除
        该标志并枚举所有可能的格式，忽略当前配置带来的任何限制。不支持该标志
        的驱动程序总是返回 ``EINVAL`` 错误码，且不清除该标志。使用
        ``V4L2_FMTDESC_FLAG_ENUM_ALL`` 标志枚举出的格式，不应用于调用
        :c:func:`VIDIOC_ENUM_FRAMESIZES` 或
        :c:func:`VIDIOC_ENUM_FRAMEINTERVALS`。
        ``V4L2_FMTDESC_FLAG_ENUM_ALL`` 只应由能够依据该标志返回不同格式列表的
        驱动程序使用。)
    * - __u32
      - ``type``
      - Type of the data stream, set by the application. Only these types
	are valid here: ``V4L2_BUF_TYPE_VIDEO_CAPTURE``,
	``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``,
	``V4L2_BUF_TYPE_VIDEO_OUTPUT``,
	``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``,
	``V4L2_BUF_TYPE_VIDEO_OVERLAY``,
	``V4L2_BUF_TYPE_SDR_CAPTURE``,
	``V4L2_BUF_TYPE_SDR_OUTPUT``,
	``V4L2_BUF_TYPE_META_CAPTURE`` and
	``V4L2_BUF_TYPE_META_OUTPUT``.
	See :c:type:`v4l2_buf_type`.
	(数据流的类型，由应用程序设置。此处只有这些类型是有效的：
	``V4L2_BUF_TYPE_VIDEO_CAPTURE``、
	``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``、
	``V4L2_BUF_TYPE_VIDEO_OUTPUT``、
	``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``、
	``V4L2_BUF_TYPE_VIDEO_OVERLAY``、
	``V4L2_BUF_TYPE_SDR_CAPTURE``、
	``V4L2_BUF_TYPE_SDR_OUTPUT``、
	``V4L2_BUF_TYPE_META_CAPTURE`` 与
	``V4L2_BUF_TYPE_META_OUTPUT``。见 :c:type:`v4l2_buf_type`。)
    * - __u32
      - ``flags``
      - See :ref:`fmtdesc-flags`
        (见 :ref:`fmtdesc-flags`)
    * - __u8
      - ``description``\ [32]
      - Description of the format, a NUL-terminated ASCII string. This
	information is intended for the user, for example: "YUV 4:2:2".
	(格式的描述，一个以 NUL 结尾的 ASCII 字符串。该信息面向用户，例如：
	"YUV 4:2:2"。)
    * - __u32
      - ``pixelformat``
      - The image format identifier. This is a four character code as
	computed by the v4l2_fourcc() macro:
	(图像格式标识符。这是一个四字符码，由 v4l2_fourcc() 宏计算得到：)
    * - :cspan:`2`

	.. _v4l2-fourcc:

	``#define v4l2_fourcc(a,b,c,d)``

	``(((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))``

	Several image formats are already defined by this specification in
	:ref:`pixfmt`.
	(本规范已在 :ref:`pixfmt` 中定义了多种图像格式。)

	.. attention::

	   These codes are not the same as those used
	   in the Windows world.
	   (这些码与 Windows 世界中使用的码并不相同。)
    * - __u32
      - ``mbus_code``
      - Media bus code restricting the enumerated formats, set by the
        application. Only applicable to drivers that advertise the
        ``V4L2_CAP_IO_MC`` :ref:`capability <device-capabilities>`, shall be 0
        otherwise.
        (限制所枚举格式的 media bus 码，由应用程序设置。仅对宣称支持
        ``V4L2_CAP_IO_MC`` :ref:`capability <device-capabilities>` 能力的驱动
        程序适用，否则应为 0。)
    * - __u32
      - ``reserved``\ [3]
      - Reserved for future extensions. Drivers must set the array to
	zero.
	(为未来扩展保留。驱动程序必须把该数组置零。)


.. tabularcolumns:: |p{8.4cm}|p{1.8cm}|p{7.1cm}|

.. cssclass:: longtable

.. _fmtdesc-flags:

.. flat-table:: Image Format Description Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_FMT_FLAG_COMPRESSED``
      - 0x0001
      - This is a compressed format.
        (这是一个压缩格式。)
    * - ``V4L2_FMT_FLAG_EMULATED``
      - 0x0002
      - This format is not native to the device but emulated through
	software (usually libv4l2), where possible try to use a native
	format instead for better performance.
	(该格式并非设备原生格式，而是通过软件(通常是 libv4l2)模拟出来的；在
	可能的情况下应尽量改用原生格式以获得更好的性能。)
    * - ``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``
      - 0x0004
      - The hardware decoder for this compressed bytestream format (aka coded
	format) is capable of parsing a continuous bytestream. Applications do
	not need to parse the bytestream themselves to find the boundaries
	between frames/fields.
	(该压缩字节流格式(即编码格式)的硬件解码器能够解析连续的字节流。应用
	程序无需自行解析字节流来寻找帧/场之间的边界。)

	This flag can only be used in combination with the
	``V4L2_FMT_FLAG_COMPRESSED`` flag, since this applies to compressed
	formats only. This flag is valid for stateful decoders only.
	(该标志只能与 ``V4L2_FMT_FLAG_COMPRESSED`` 标志组合使用，因为它仅适用
	于压缩格式。该标志仅对有状态解码器有效。)
    * - ``V4L2_FMT_FLAG_DYN_RESOLUTION``
      - 0x0008
      - Dynamic resolution switching is supported by the device for this
	compressed bytestream format (aka coded format). It will notify the user
	via the event ``V4L2_EVENT_SOURCE_CHANGE`` when changes in the video
	parameters are detected.
	(设备对该压缩字节流格式(即编码格式)支持动态分辨率切换。当检测到视频
	参数发生变化时，它会通过事件 ``V4L2_EVENT_SOURCE_CHANGE`` 通知用户。)

	This flag can only be used in combination with the
	``V4L2_FMT_FLAG_COMPRESSED`` flag, since this applies to
	compressed formats only. This flag is valid for stateful codecs only.
	(该标志只能与 ``V4L2_FMT_FLAG_COMPRESSED`` 标志组合使用，因为它仅适用
	于压缩格式。该标志仅对有状态编解码器有效。)
    * - ``V4L2_FMT_FLAG_ENC_CAP_FRAME_INTERVAL``
      - 0x0010
      - The hardware encoder supports setting the ``CAPTURE`` coded frame
	interval separately from the ``OUTPUT`` raw frame interval.
	Setting the ``OUTPUT`` raw frame interval with :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>`
	also sets the ``CAPTURE`` coded frame interval to the same value.
	If this flag is set, then the ``CAPTURE`` coded frame interval can be
	set to a different value afterwards. This is typically used for
	offline encoding where the ``OUTPUT`` raw frame interval is used as
	a hint for reserving hardware encoder resources and the ``CAPTURE`` coded
	frame interval is the actual frame rate embedded in the encoded video
	stream.
	(硬件编码器支持把 ``CAPTURE`` 编码帧间隔与 ``OUTPUT`` 原始帧间隔分开
	设置。用 :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` 设置 ``OUTPUT`` 原始帧
	间隔时，也会把 ``CAPTURE`` 编码帧间隔设置为相同的值。如果设置了该标志，
	则之后可以把 ``CAPTURE`` 编码帧间隔设置为不同的值。这通常用于离线编码：
	``OUTPUT`` 原始帧间隔用作预留硬件编码器资源的提示，而 ``CAPTURE`` 编码
	帧间隔则是编码视频流中嵌入的实际帧率。)

	This flag can only be used in combination with the
	``V4L2_FMT_FLAG_COMPRESSED`` flag, since this applies to
        compressed formats only. This flag is valid for stateful encoders only.
        (该标志只能与 ``V4L2_FMT_FLAG_COMPRESSED`` 标志组合使用，因为它仅适用
        于压缩格式。该标志仅对有状态编码器有效。)
    * - ``V4L2_FMT_FLAG_CSC_COLORSPACE``
      - 0x0020
      - The driver allows the application to try to change the default
	colorspace. This flag is relevant only for capture devices.
	The application can ask to configure the colorspace of the capture device
	when calling the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl with
	:ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>` set.
	(驱动程序允许应用程序尝试更改默认的色彩空间。该标志仅与捕获设备相关。
	在设置了 :ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>`
	的情况下调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 时，应用程序可以
	要求配置捕获设备的色彩空间。)
    * - ``V4L2_FMT_FLAG_CSC_XFER_FUNC``
      - 0x0040
      - The driver allows the application to try to change the default
	transfer function. This flag is relevant only for capture devices.
	The application can ask to configure the transfer function of the capture
	device when calling the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl with
	:ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>` set.
	(驱动程序允许应用程序尝试更改默认的传递函数。该标志仅与捕获设备相关。
	在设置了 :ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>`
	的情况下调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 时，应用程序可以
	要求配置捕获设备的传递函数。)
    * - ``V4L2_FMT_FLAG_CSC_YCBCR_ENC``
      - 0x0080
      - The driver allows the application to try to change the default
	Y'CbCr encoding. This flag is relevant only for capture devices.
	The application can ask to configure the Y'CbCr encoding of the capture device
	when calling the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl with
	:ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>` set.
	(驱动程序允许应用程序尝试更改默认的 Y'CbCr 编码。该标志仅与捕获设备
	相关。在设置了
	:ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>`
	的情况下调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 时，应用程序可以
	要求配置捕获设备的 Y'CbCr 编码。)
    * - ``V4L2_FMT_FLAG_CSC_HSV_ENC``
      - 0x0080
      - The driver allows the application to try to change the default
	HSV encoding. This flag is relevant only for capture devices.
	The application can ask to configure the HSV encoding of the capture device
	when calling the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl with
	:ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>` set.
	(驱动程序允许应用程序尝试更改默认的 HSV 编码。该标志仅与捕获设备相关。
	在设置了
	:ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>`
	的情况下调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 时，应用程序可以
	要求配置捕获设备的 HSV 编码。)
    * - ``V4L2_FMT_FLAG_CSC_QUANTIZATION``
      - 0x0100
      - The driver allows the application to try to change the default
	quantization. This flag is relevant only for capture devices.
	The application can ask to configure the quantization of the capture
	device when calling the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl with
	:ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>` set.
	(驱动程序允许应用程序尝试更改默认的量化方式。该标志仅与捕获设备相关。
	在设置了
	:ref:`V4L2_PIX_FMT_FLAG_SET_CSC <v4l2-pix-fmt-flag-set-csc>`
	的情况下调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 时，应用程序可以
	要求配置捕获设备的量化方式。)
    * - ``V4L2_FMT_FLAG_META_LINE_BASED``
      - 0x0200
      - The metadata format is line-based. In this case the ``width``,
	``height`` and ``bytesperline`` fields of :c:type:`v4l2_meta_format` are
	valid. The buffer consists of ``height`` lines, each having ``width``
	Data Units of data and the offset (in bytes) between the beginning of
	each two consecutive lines is ``bytesperline``.
	(该元数据格式是按行组织的。此时 :c:type:`v4l2_meta_format` 的
	``width``、``height`` 与 ``bytesperline`` 字段有效。缓冲区由 ``height``
	行组成，每行包含 ``width`` 个数据单元 (Data Unit)，相邻两行起点之间的
	偏移(以字节计)为 ``bytesperline``。)
    * - ``V4L2_FMTDESC_FLAG_ENUM_ALL``
      - 0x80000000
      - When the applications ORs ``index`` with ``V4L2_FMTDESC_FLAG_ENUM_ALL`` flag
        the driver enumerates all the possible pixel formats without taking care
        of any already set configuration. Drivers which do not support this flag,
        always return ``EINVAL`` without clearing this flag.
        (当应用程序把 ``index`` 与 ``V4L2_FMTDESC_FLAG_ENUM_ALL`` 标志按位或
        时，驱动程序会枚举所有可能的像素格式，而不考虑任何已设置的配置。不支持
        该标志的驱动程序总是返回 ``EINVAL``，且不清除该标志。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_fmtdesc` ``type`` is not
    supported or the ``index`` is out of bounds.
    (struct :c:type:`v4l2_fmtdesc` 的 ``type`` 不受支持，或 ``index`` 越界。)

    If ``V4L2_CAP_IO_MC`` is set and the specified ``mbus_code``
    is unsupported, then also return this error code.
    (如果设置了 ``V4L2_CAP_IO_MC`` 且指定的 ``mbus_code`` 不受支持，也会返回
    此错误码。)
