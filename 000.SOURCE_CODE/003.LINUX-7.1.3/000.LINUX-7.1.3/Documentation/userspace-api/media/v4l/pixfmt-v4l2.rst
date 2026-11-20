.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

**********************************************
Single-planar format structure(单平面格式结构)
**********************************************

.. tabularcolumns:: |p{4.0cm}|p{2.6cm}|p{10.7cm}|

.. c:type:: v4l2_pix_format

.. cssclass:: longtable

.. flat-table:: struct v4l2_pix_format
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``width``
      - Image width in pixels.
        (图像宽度，单位为像素。)
    * - __u32
      - ``height``
      - Image height in pixels. If ``field`` is one of ``V4L2_FIELD_TOP``,
	``V4L2_FIELD_BOTTOM`` or ``V4L2_FIELD_ALTERNATE`` then height
	refers to the number of lines in the field, otherwise it refers to
	the number of lines in the frame (which is twice the field height
	for interlaced formats).
        (图像高度，单位为像素。如果 ``field`` 为 ``V4L2_FIELD_TOP`` 、``V4L2_FIELD_BOTTOM`` 或 ``V4L2_FIELD_ALTERNATE`` 之一，则 height 指的是场 (field) 中的行数；否则指的是帧中的行数 (对于隔行扫描格式，是场高度的两倍)。)
    * - :cspan:`2` Applications set these fields to request an image
	size, drivers return the closest possible values. In case of
	planar formats the ``width`` and ``height`` applies to the largest
	plane. To avoid ambiguities drivers must return values rounded up
	to a multiple of the scale factor of any smaller planes. For
	example when the image format is YUV 4:2:0, ``width`` and
	``height`` must be multiples of two.

	For compressed formats that contain the resolution information encoded
	inside the stream, when fed to a stateful mem2mem decoder, the fields
	may be zero to rely on the decoder to detect the right values. For more
	details see :ref:`decoder` and format descriptions.

	For compressed formats on the CAPTURE side of a stateful mem2mem
	encoder, the fields must be zero, since the coded size is expected to
	be calculated internally by the encoder itself, based on the OUTPUT
	side. For more details see :ref:`encoder` and format descriptions.

        (应用程序设置这些字段以请求图像尺寸，驱动返回最接近的可能值。对于平面 (planar) 格式，``width`` 与 ``height`` 适用于最大的平面。为避免歧义，驱动返回的值必须向上取整为任何较小平面的缩放因子的倍数。例如当图像格式为 YUV 4:2:0 时，``width`` 与 ``height`` 必须是 2 的倍数。

        对于流内编码了分辨率信息的压缩格式，当送入有状态 mem2mem 解码器时，这些字段可以为零，交由解码器检测正确的值。更多细节见 :ref:`decoder` 与各格式描述。

        对于有状态 mem2mem 编码器的 CAPTURE 侧的压缩格式，这些字段必须为零，因为编码尺寸预期由编码器自身基于 OUTPUT 侧在内部计算。更多细节见 :ref:`encoder` 与各格式描述。)
    * - __u32
      - ``pixelformat``
      - The pixel format or type of compression, set by the application.
	This is a little endian
	:ref:`four character code <v4l2-fourcc>`. V4L2 defines standard
	RGB formats in :ref:`pixfmt-rgb`, YUV formats in
	:ref:`yuv-formats`, and reserved codes in
	:ref:`reserved-formats`
        (像素格式或压缩类型，由应用程序设置。这是一个小端序的
	:ref:`four character code <v4l2-fourcc>` 。V4L2 在 :ref:`pixfmt-rgb` 中定义标准 RGB 格式，在
	:ref:`yuv-formats` 中定义 YUV 格式，在
	:ref:`reserved-formats` 中定义保留码。)
    * - __u32
      - ``field``
      - Field order, from enum :c:type:`v4l2_field`.
        Video images are typically interlaced. Applications can request to
	capture or output only the top or bottom field, or both fields
	interlaced or sequentially stored in one buffer or alternating in
	separate buffers. Drivers return the actual field order selected.
	For more details on fields see :ref:`field-order`.
        (场序，来自 enum :c:type:`v4l2_field`。视频图像通常是隔行的。应用程序可以请求只采集或输出顶场或底场，或者两场交错或顺序存放于同一缓冲，或在多个缓冲中交替存放。驱动返回实际选择的场序。场的更多细节见 :ref:`field-order`。)
    * - __u32
      - ``bytesperline``
      - Distance in bytes between the leftmost pixels in two adjacent
	lines.
        (相邻两行最左侧像素之间的字节距离。)
    * - :cspan:`2`

	Both applications and drivers can set this field to request
	padding bytes at the end of each line. Drivers however may ignore
	the value requested by the application, returning ``width`` times
	bytes per pixel or a larger value required by the hardware. That
	implies applications can just set this field to zero to get a
	reasonable default.

	Video hardware may access padding bytes, therefore they must
	reside in accessible memory. Consider cases where padding bytes
	after the last line of an image cross a system page boundary.
	Input devices may write padding bytes, the value is undefined.
	Output devices ignore the contents of padding bytes.

	When the image format is planar the ``bytesperline`` value applies
	to the first plane and is divided by the same factor as the
	``width`` field for the other planes. For example the Cb and Cr
	planes of a YUV 4:2:0 image have half as many padding bytes
	following each line as the Y plane. To avoid ambiguities drivers
	must return a ``bytesperline`` value rounded up to a multiple of
	the scale factor.

	For compressed formats the ``bytesperline`` value makes no sense.
	Applications and drivers must set this to 0 in that case.

        (应用程序与驱动都可以设置该字段，以请求每行末尾的填充 (padding) 字节。不过驱动可以忽略应用程序请求的值，返回 ``width`` 乘以每像素字节数，或返回硬件要求的更大值。这意味着应用程序可以直接把该字段设为 0 来获得合理的默认值。

        视频硬件可能访问填充字节，因此它们必须位于可访问的内存中。请考虑图像最后一行之后的填充字节跨越系统页边界的情况。输入设备可能写入填充字节，其值未定义。输出设备忽略填充字节的内容。

        当图像格式为平面格式时，``bytesperline`` 值适用于第一个平面，并且对其他平面按与 ``width`` 字段相同的因子相除。例如 YUV 4:2:0 图像的 Cb 与 Cr 平面每行之后的填充字节数是 Y 平面的一半。为避免歧义，驱动必须返回向上取整为缩放因子倍数的 ``bytesperline`` 值。

        对于压缩格式，``bytesperline`` 值没有意义。这种情况下应用程序与驱动必须把它设为 0。)
    * - __u32
      - ``sizeimage``
      - Size in bytes of the buffer to hold a complete image, set by the
	driver. Usually this is ``bytesperline`` times ``height``. When
	the image consists of variable length compressed data this is the
	number of bytes required by the codec to support the worst-case
	compression scenario.

	The driver will set the value for uncompressed images.

	Clients are allowed to set the sizeimage field for variable length
	compressed data flagged with ``V4L2_FMT_FLAG_COMPRESSED`` at
	:ref:`VIDIOC_ENUM_FMT`, but the driver may ignore it and set the
	value itself, or it may modify the provided value based on
	alignment requirements or minimum/maximum size requirements.
	If the client wants to leave this to the driver, then it should
	set sizeimage to 0.
        (保存完整图像的缓冲大小，单位为字节，由驱动设置。通常为 ``bytesperline`` 乘以 ``height``。当图像为变长压缩数据时，这是编解码器支持最坏情况压缩场景所需的字节数。

        对于未压缩图像，驱动会设置该值。

        允许客户端为在 :ref:`VIDIOC_ENUM_FMT` 中带有 ``V4L2_FMT_FLAG_COMPRESSED`` 标志的变长压缩数据设置 sizeimage 字段，但驱动可以忽略它并自行设置值，或者根据对齐要求或最小/最大尺寸要求修改所提供的值。如果客户端想把此事留给驱动处理，应把 sizeimage 设为 0。)
    * - __u32
      - ``colorspace``
      - Image colorspace, from enum :c:type:`v4l2_colorspace`.
        This information supplements the ``pixelformat`` and must be set
	by the driver for capture streams and by the application for
	output streams, see :ref:`colorspaces`. If the application sets the
	flag ``V4L2_PIX_FMT_FLAG_SET_CSC`` then the application can set
	this field for a capture stream to request a specific colorspace
	for the captured image data. If the driver cannot handle requested
	conversion, it will return another supported colorspace.
	The driver indicates that colorspace conversion is supported by setting
	the flag V4L2_FMT_FLAG_CSC_COLORSPACE in the corresponding struct
	:c:type:`v4l2_fmtdesc` during enumeration. See :ref:`fmtdesc-flags`.
        (图像色彩空间，来自 enum :c:type:`v4l2_colorspace`。该信息补充 ``pixelformat``，对于采集流必须由驱动设置，对于输出流必须由应用程序设置，见 :ref:`colorspaces`。如果应用程序设置了 ``V4L2_PIX_FMT_FLAG_SET_CSC`` 标志，则应用程序可以为采集流设置该字段，为采集到的图像数据请求特定的色彩空间。如果驱动无法处理所请求的转换，它将返回另一个受支持的色彩空间。驱动通过在枚举期间在相应的 struct
	:c:type:`v4l2_fmtdesc` 中设置 V4L2_FMT_FLAG_CSC_COLORSPACE 标志来表明支持色彩空间转换。见 :ref:`fmtdesc-flags`。)
    * - __u32
      - ``priv``
      - This field indicates whether the remaining fields of the
	struct :c:type:`v4l2_pix_format`, also called the
	extended fields, are valid. When set to
	``V4L2_PIX_FMT_PRIV_MAGIC``, it indicates that the extended fields
	have been correctly initialized. When set to any other value it
	indicates that the extended fields contain undefined values.

	Applications that wish to use the pixel format extended fields
	must first ensure that the feature is supported by querying the
	device for the :ref:`V4L2_CAP_EXT_PIX_FORMAT <querycap>`
	capability. If the capability isn't set the pixel format extended
	fields are not supported and using the extended fields will lead
	to undefined results.

	To use the extended fields, applications must set the ``priv``
	field to ``V4L2_PIX_FMT_PRIV_MAGIC``, initialize all the extended
	fields and zero the unused bytes of the
	struct :c:type:`v4l2_format` ``raw_data`` field.

	When the ``priv`` field isn't set to ``V4L2_PIX_FMT_PRIV_MAGIC``
	drivers must act as if all the extended fields were set to zero.
	On return drivers must set the ``priv`` field to
	``V4L2_PIX_FMT_PRIV_MAGIC`` and all the extended fields to
	applicable values.
        (该字段指示 struct :c:type:`v4l2_pix_format` 的其余字段 (也称为扩展字段) 是否有效。当设置为 ``V4L2_PIX_FMT_PRIV_MAGIC`` 时，表示扩展字段已正确初始化；当设置为任何其他值时，表示扩展字段包含未定义的值。

        希望使用像素格式扩展字段的应用程序，必须先通过查询设备的 :ref:`V4L2_CAP_EXT_PIX_FORMAT <querycap>`
        能力来确保该特性受支持。如果该能力未设置，则不支持像素格式扩展字段，使用它们将导致未定义的结果。

        要使用扩展字段，应用程序必须把 ``priv`` 字段设为 ``V4L2_PIX_FMT_PRIV_MAGIC`` ，初始化所有扩展字段，并把 struct :c:type:`v4l2_format` 的 ``raw_data`` 字段的未用字节清零。

        当 ``priv`` 字段未设置为 ``V4L2_PIX_FMT_PRIV_MAGIC`` 时，驱动必须表现得好像所有扩展字段都被设为零。返回时，驱动必须把 ``priv`` 字段设为 ``V4L2_PIX_FMT_PRIV_MAGIC`` 并把所有扩展字段设为适用的值。)
    * - __u32
      - ``flags``
      - Flags set by the application or driver, see :ref:`format-flags`.
        (由应用程序或驱动设置的标志，见 :ref:`format-flags`。)
    * - union {
      - (anonymous)
    * - __u32
      - ``ycbcr_enc``
      - Y'CbCr encoding, from enum :c:type:`v4l2_ycbcr_encoding`.
        This information supplements the ``colorspace`` and must be set by
	the driver for capture streams and by the application for output
	streams, see :ref:`colorspaces`. If the application sets the
	flag ``V4L2_PIX_FMT_FLAG_SET_CSC`` then the application can set
	this field for a capture stream to request a specific Y'CbCr encoding
	for the captured image data. If the driver cannot handle requested
	conversion, it will return another supported encoding.
	This field is ignored for HSV pixelformats. The driver indicates that
	ycbcr_enc conversion is supported by setting the flag
	V4L2_FMT_FLAG_CSC_YCBCR_ENC in the corresponding struct
	:c:type:`v4l2_fmtdesc` during enumeration. See :ref:`fmtdesc-flags`.
        (Y'CbCr 编码，来自 enum :c:type:`v4l2_ycbcr_encoding`。该信息补充 ``colorspace``，对于采集流必须由驱动设置，对于输出流必须由应用程序设置，见 :ref:`colorspaces`。如果应用程序设置了 ``V4L2_PIX_FMT_FLAG_SET_CSC`` 标志，则应用程序可以为采集流设置该字段，为采集到的图像数据请求特定的 Y'CbCr 编码。如果驱动无法处理所请求的转换，它将返回另一个受支持的编码。HSV 像素格式忽略该字段。驱动通过在枚举期间在相应的 struct
	:c:type:`v4l2_fmtdesc` 中设置 V4L2_FMT_FLAG_CSC_YCBCR_ENC 标志来表明支持 ycbcr_enc 转换。见 :ref:`fmtdesc-flags`。)
    * - __u32
      - ``hsv_enc``
      - HSV encoding, from enum :c:type:`v4l2_hsv_encoding`.
        This information supplements the ``colorspace`` and must be set by
	the driver for capture streams and by the application for output
	streams, see :ref:`colorspaces`. If the application sets the flag
	``V4L2_PIX_FMT_FLAG_SET_CSC`` then the application can set this
	field for a capture stream to request a specific HSV encoding for the
	captured image data. If the driver cannot handle requested
	conversion, it will return another supported encoding.
	This field is ignored for non-HSV pixelformats. The driver indicates
	that hsv_enc conversion is supported by setting the flag
	V4L2_FMT_FLAG_CSC_HSV_ENC in the corresponding struct
	:c:type:`v4l2_fmtdesc` during enumeration. See :ref:`fmtdesc-flags`.
        (HSV 编码，来自 enum :c:type:`v4l2_hsv_encoding`。该信息补充 ``colorspace``，对于采集流必须由驱动设置，对于输出流必须由应用程序设置，见 :ref:`colorspaces`。如果应用程序设置了 ``V4L2_PIX_FMT_FLAG_SET_CSC`` 标志，则应用程序可以为采集流设置该字段，为采集到的图像数据请求特定的 HSV 编码。如果驱动无法处理所请求的转换，它将返回另一个受支持的编码。非 HSV 像素格式忽略该字段。驱动通过在枚举期间在相应的 struct
	:c:type:`v4l2_fmtdesc` 中设置 V4L2_FMT_FLAG_CSC_HSV_ENC 标志来表明支持 hsv_enc 转换。见 :ref:`fmtdesc-flags`。)
    * - }
      -
    * - __u32
      - ``quantization``
      - Quantization range, from enum :c:type:`v4l2_quantization`.
        This information supplements the ``colorspace`` and must be set by
	the driver for capture streams and by the application for output
	streams, see :ref:`colorspaces`. If the application sets the flag
	``V4L2_PIX_FMT_FLAG_SET_CSC`` then the application can set
	this field for a capture stream to request a specific quantization
	range for the captured image data. If the driver cannot handle requested
	conversion, it will return another supported quantization.
	The driver indicates that quantization conversion is supported by setting
	the flag V4L2_FMT_FLAG_CSC_QUANTIZATION in the corresponding struct
	:c:type:`v4l2_fmtdesc` during enumeration. See :ref:`fmtdesc-flags`.
        (量化范围，来自 enum :c:type:`v4l2_quantization`。该信息补充 ``colorspace``，对于采集流必须由驱动设置，对于输出流必须由应用程序设置，见 :ref:`colorspaces`。如果应用程序设置了 ``V4L2_PIX_FMT_FLAG_SET_CSC`` 标志，则应用程序可以为采集流设置该字段，为采集到的图像数据请求特定的量化范围。如果驱动无法处理所请求的转换，它将返回另一个受支持的量化范围。驱动通过在枚举期间在相应的 struct
	:c:type:`v4l2_fmtdesc` 中设置 V4L2_FMT_FLAG_CSC_QUANTIZATION 标志来表明支持量化转换。见 :ref:`fmtdesc-flags`。)
    * - __u32
      - ``xfer_func``
      - Transfer function, from enum :c:type:`v4l2_xfer_func`.
        This information supplements the ``colorspace`` and must be set by
	the driver for capture streams and by the application for output
	streams, see :ref:`colorspaces`. If the application sets the flag
	``V4L2_PIX_FMT_FLAG_SET_CSC`` then the application can set
	this field for a capture stream to request a specific transfer function
	for the captured image data. If the driver cannot handle requested
	conversion, it will return another supported transfer function.
	The driver indicates that xfer_func conversion is supported by setting
	the flag V4L2_FMT_FLAG_CSC_XFER_FUNC in the corresponding struct
	:c:type:`v4l2_fmtdesc` during enumeration. See :ref:`fmtdesc-flags`.
        (传输函数，来自 enum :c:type:`v4l2_xfer_func`。该信息补充 ``colorspace``，对于采集流必须由驱动设置，对于输出流必须由应用程序设置，见 :ref:`colorspaces`。如果应用程序设置了 ``V4L2_PIX_FMT_FLAG_SET_CSC`` 标志，则应用程序可以为采集流设置该字段，为采集到的图像数据请求特定的传输函数。如果驱动无法处理所请求的转换，它将返回另一个受支持的传输函数。驱动通过在枚举期间在相应的 struct
	:c:type:`v4l2_fmtdesc` 中设置 V4L2_FMT_FLAG_CSC_XFER_FUNC 标志来表明支持 xfer_func 转换。见 :ref:`fmtdesc-flags`。)

.. tabularcolumns:: |p{6.8cm}|p{2.3cm}|p{8.2cm}|

.. _format-flags:

.. flat-table:: Format Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_PIX_FMT_FLAG_PREMUL_ALPHA``
      - 0x00000001
      - The color values are premultiplied by the alpha channel value. For
        example, if a light blue pixel with 50% transparency was described
	by RGBA values (128, 192, 255, 128), the same pixel described with
	premultiplied colors would be described by RGBA values (64, 96,
	128, 128)
        (颜色值已按 alpha 通道值预乘。例如，如果一个透明度为 50% 的浅蓝色像素由 RGBA 值 (128, 192, 255, 128) 描述，则同一像素用预乘颜色描述时，其 RGBA 值为 (64, 96,
	128, 128)。)
    * .. _`v4l2-pix-fmt-flag-set-csc`:

      - ``V4L2_PIX_FMT_FLAG_SET_CSC``
      - 0x00000002
      - Set by the application. It is only used for capture and is
        ignored for output streams. If set, then request the device to do
	colorspace conversion from the received colorspace to the requested
	colorspace values. If the colorimetry field (``colorspace``, ``xfer_func``,
	``ycbcr_enc``, ``hsv_enc`` or ``quantization``) is set to ``*_DEFAULT``,
	then that colorimetry setting will remain unchanged from what was received.
	So in order to change the quantization, only the ``quantization`` field shall
	be set to non default value (``V4L2_QUANTIZATION_FULL_RANGE`` or
	``V4L2_QUANTIZATION_LIM_RANGE``) and all other colorimetry fields shall
	be set to ``*_DEFAULT``.

	To check which conversions are supported by the hardware for the current
	pixel format, see :ref:`fmtdesc-flags`.

        (由应用程序设置。仅用于采集，对输出流忽略。设置后请求设备进行色彩空间转换，把接收到的色彩空间转换为所请求的色彩空间值。如果色度字段 (``colorspace``、``xfer_func``、
	``ycbcr_enc``、``hsv_enc`` 或 ``quantization``) 被设为 ``*_DEFAULT`` ，则该色度设置将保持与接收到的相同。因此，要改变量化方式，只应把 ``quantization`` 字段设为非默认值 (``V4L2_QUANTIZATION_FULL_RANGE`` 或
	``V4L2_QUANTIZATION_LIM_RANGE``)，而所有其他色度字段都应设为 ``*_DEFAULT`` 。

        要查看硬件对当前像素格式支持哪些转换，见 :ref:`fmtdesc-flags`。)
