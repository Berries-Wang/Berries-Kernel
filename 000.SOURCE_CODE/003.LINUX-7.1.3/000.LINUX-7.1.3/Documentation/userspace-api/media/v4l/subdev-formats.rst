.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-mbus-format:

Media Bus Formats(媒体总线格式)
===============================

.. c:type:: v4l2_mbus_framefmt

.. tabularcolumns:: |p{2.0cm}|p{4.0cm}|p{11.3cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_mbus_framefmt
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``width``
      - Image width in pixels.(图像宽度，单位为像素。)
    * - __u32
      - ``height``
      - Image height in pixels. If ``field`` is one of ``V4L2_FIELD_TOP``,
	``V4L2_FIELD_BOTTOM`` or ``V4L2_FIELD_ALTERNATE`` then height
	refers to the number of lines in the field, otherwise it refers to
	the number of lines in the frame (which is twice the field height
	for interlaced formats).
	(图像高度，单位为像素。如果 ``field`` 为 ``V4L2_FIELD_TOP``、``V4L2_FIELD_BOTTOM`` 或 ``V4L2_FIELD_ALTERNATE`` 之一，则高度指场内的行数；否则指帧内的行数（对隔行格式是场高的两倍）。)
    * - __u32
      - ``code``
      - Format code, from enum
	:ref:`v4l2_mbus_pixelcode <v4l2-mbus-pixelcode>`.
	(格式代码，取自枚举 v4l2_mbus_pixelcode。)
    * - __u32
      - ``field``
      - Field order, from enum :c:type:`v4l2_field`. See
	:ref:`field-order` for details. Zero for metadata mbus codes.
	(场序，取自枚举 :c:type:`v4l2_field`。详情见 :ref:`field-order`。元数据 mbus 代码取 0。)
    * - __u32
      - ``colorspace``
      - Image colorspace, from enum :c:type:`v4l2_colorspace`.
        Must be set by the driver for subdevices. If the application sets the
	flag ``V4L2_MBUS_FRAMEFMT_SET_CSC`` then the application can set this
	field on the source pad to request a specific colorspace for the media
	bus data. If the driver cannot handle the requested conversion, it will
	return another supported colorspace. The driver indicates that colorspace
	conversion is supported by setting the flag
	V4L2_SUBDEV_MBUS_CODE_CSC_COLORSPACE in the corresponding struct
	:c:type:`v4l2_subdev_mbus_code_enum` during enumeration.
	See :ref:`v4l2-subdev-mbus-code-flags`. Zero for metadata mbus codes.
	(图像色彩空间，取自枚举 :c:type:`v4l2_colorspace`。必须由驱动程序为子设备设置。若应用设置了标志 ``V4L2_MBUS_FRAMEFMT_SET_CSC``，则应用可以在源 pad 上设置该字段，以请求媒体总线数据使用特定的色彩空间。若驱动程序无法处理所请求的转换，将返回另一种受支持的色彩空间。驱动程序通过在枚举时于相应的 struct :c:type:`v4l2_subdev_mbus_code_enum` 中设置标志 V4L2_SUBDEV_MBUS_CODE_CSC_COLORSPACE 来表明支持色彩空间转换。参见 :ref:`v4l2-subdev-mbus-code-flags`。元数据 mbus 代码取 0。)
    * - union {
      - (anonymous)(匿名)
    * - __u16
      - ``ycbcr_enc``
      - Y'CbCr encoding, from enum :c:type:`v4l2_ycbcr_encoding`.
        This information supplements the ``colorspace`` and must be set by
	the driver for subdevices, see :ref:`colorspaces`. If the application
	sets the flag ``V4L2_MBUS_FRAMEFMT_SET_CSC`` then the application can set
	this field on a source pad to request a specific Y'CbCr encoding
	for the media bus data. If the driver cannot handle the requested
	conversion, it will return another supported encoding.
	This field is ignored for HSV media bus formats. The driver indicates
	that ycbcr_enc conversion is supported by setting the flag
	V4L2_SUBDEV_MBUS_CODE_CSC_YCBCR_ENC in the corresponding struct
	:c:type:`v4l2_subdev_mbus_code_enum` during enumeration.
	See :ref:`v4l2-subdev-mbus-code-flags`. Zero for metadata mbus codes.
	(Y'CbCr 编码，取自枚举 :c:type:`v4l2_ycbcr_encoding`。该信息是对 ``colorspace`` 的补充，必须由驱动程序为子设备设置，参见 :ref:`colorspaces`。若应用设置了标志 ``V4L2_MBUS_FRAMEFMT_SET_CSC``，则应用可以在源 pad 上设置该字段，以请求媒体总线数据使用特定的 Y'CbCr 编码。若驱动程序无法处理所请求的转换，将返回另一种受支持的编码。对 HSV 媒体总线格式，该字段被忽略。驱动程序通过在枚举时于相应的 struct :c:type:`v4l2_subdev_mbus_code_enum` 中设置标志 V4L2_SUBDEV_MBUS_CODE_CSC_YCBCR_ENC 来表明支持 ycbcr_enc 转换。参见 :ref:`v4l2-subdev-mbus-code-flags`。元数据 mbus 代码取 0。)
    * - __u16
      - ``hsv_enc``
      - HSV encoding, from enum :c:type:`v4l2_hsv_encoding`.
        This information supplements the ``colorspace`` and must be set by
	the driver for subdevices, see :ref:`colorspaces`. If the application
	sets the flag ``V4L2_MBUS_FRAMEFMT_SET_CSC`` then the application can set
	this field on a source pad to request a specific HSV encoding
	for the media bus data. If the driver cannot handle the requested
	conversion, it will return another supported encoding.
	This field is ignored for Y'CbCr media bus formats. The driver indicates
	that hsv_enc conversion is supported by setting the flag
	V4L2_SUBDEV_MBUS_CODE_CSC_HSV_ENC in the corresponding struct
	:c:type:`v4l2_subdev_mbus_code_enum` during enumeration.
	See :ref:`v4l2-subdev-mbus-code-flags`. Zero for metadata mbus codes.
	(HSV 编码，取自枚举 :c:type:`v4l2_hsv_encoding`。该信息是对 ``colorspace`` 的补充，必须由驱动程序为子设备设置，参见 :ref:`colorspaces`。若应用设置了标志 ``V4L2_MBUS_FRAMEFMT_SET_CSC``，则应用可以在源 pad 上设置该字段，以请求媒体总线数据使用特定的 HSV 编码。若驱动程序无法处理所请求的转换，将返回另一种受支持的编码。对 Y'CbCr 媒体总线格式，该字段被忽略。驱动程序通过在枚举时于相应的 struct :c:type:`v4l2_subdev_mbus_code_enum` 中设置标志 V4L2_SUBDEV_MBUS_CODE_CSC_HSV_ENC 来表明支持 hsv_enc 转换。参见 :ref:`v4l2-subdev-mbus-code-flags`。元数据 mbus 代码取 0。)
    * - }
      -
    * - __u16
      - ``quantization``
      - Quantization range, from enum :c:type:`v4l2_quantization`.
        This information supplements the ``colorspace`` and must be set by
	the driver for subdevices, see :ref:`colorspaces`. If the application
	sets the flag ``V4L2_MBUS_FRAMEFMT_SET_CSC`` then the application can set
	this field on a source pad to request a specific quantization
	for the media bus data. If the driver cannot handle the requested
	conversion, it will return another supported quantization.
	The driver indicates that quantization conversion is supported by
	setting the flag V4L2_SUBDEV_MBUS_CODE_CSC_QUANTIZATION in the
	corresponding struct :c:type:`v4l2_subdev_mbus_code_enum`
	during enumeration. See :ref:`v4l2-subdev-mbus-code-flags`. Zero for
	metadata mbus codes.
	(量化范围，取自枚举 :c:type:`v4l2_quantization`。该信息是对 ``colorspace`` 的补充，必须由驱动程序为子设备设置，参见 :ref:`colorspaces`。若应用设置了标志 ``V4L2_MBUS_FRAMEFMT_SET_CSC``，则应用可以在源 pad 上设置该字段，以请求媒体总线数据使用特定的量化方式。若驱动程序无法处理所请求的转换，将返回另一种受支持的量化方式。驱动程序通过在枚举时于相应的 struct :c:type:`v4l2_subdev_mbus_code_enum` 中设置标志 V4L2_SUBDEV_MBUS_CODE_CSC_QUANTIZATION 来表明支持量化转换。参见 :ref:`v4l2-subdev-mbus-code-flags`。元数据 mbus 代码取 0。)
    * - __u16
      - ``xfer_func``
      - Transfer function, from enum :c:type:`v4l2_xfer_func`.
        This information supplements the ``colorspace`` and must be set by
	the driver for subdevices, see :ref:`colorspaces`. If the application
	sets the flag ``V4L2_MBUS_FRAMEFMT_SET_CSC`` then the application can set
	this field on a source pad to request a specific transfer
	function for the media bus data. If the driver cannot handle the requested
	conversion, it will return another supported transfer function.
	The driver indicates that the transfer function conversion is supported by
	setting the flag V4L2_SUBDEV_MBUS_CODE_CSC_XFER_FUNC in the
	corresponding struct :c:type:`v4l2_subdev_mbus_code_enum`
	during enumeration. See :ref:`v4l2-subdev-mbus-code-flags`. Zero for
	metadata mbus codes.
	(传输函数，取自枚举 :c:type:`v4l2_xfer_func`。该信息是对 ``colorspace`` 的补充，必须由驱动程序为子设备设置，参见 :ref:`colorspaces`。若应用设置了标志 ``V4L2_MBUS_FRAMEFMT_SET_CSC``，则应用可以在源 pad 上设置该字段，以请求媒体总线数据使用特定的传输函数。若驱动程序无法处理所请求的转换，将返回另一种受支持的传输函数。驱动程序通过在枚举时于相应的 struct :c:type:`v4l2_subdev_mbus_code_enum` 中设置标志 V4L2_SUBDEV_MBUS_CODE_CSC_XFER_FUNC 来表明支持传输函数转换。参见 :ref:`v4l2-subdev-mbus-code-flags`。元数据 mbus 代码取 0。)
    * - __u16
      - ``flags``
      - flags See:  :ref:v4l2-mbus-framefmt-flags
	(标志，参见下文的 v4l2_mbus_framefmt 标志表。)
    * - __u16
      - ``reserved``\ [10]
      - Reserved for future extensions. Applications and drivers must set
	the array to zero.
	(保留给未来的扩展使用。应用和驱动程序必须将该数组置零。)

.. _v4l2-mbus-framefmt-flags:

.. tabularcolumns:: |p{6.5cm}|p{1.6cm}|p{9.2cm}|

.. flat-table:: v4l2_mbus_framefmt Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * .. _`mbus-framefmt-set-csc`:

      - ``V4L2_MBUS_FRAMEFMT_SET_CSC``
      - 0x0001
      - Set by the application. It is only used for source pads and is
	ignored for sink pads. If set, then request the subdevice to do
	colorspace conversion from the received colorspace to the requested
	colorspace values. If the colorimetry field (``colorspace``, ``xfer_func``,
	``ycbcr_enc``, ``hsv_enc`` or ``quantization``) is set to ``*_DEFAULT``,
	then that colorimetry setting will remain unchanged from what was received.
	So in order to change the quantization, only the ``quantization`` field shall
	be set to non default value (``V4L2_QUANTIZATION_FULL_RANGE`` or
	``V4L2_QUANTIZATION_LIM_RANGE``) and all other colorimetry fields shall
	be set to ``*_DEFAULT``.

	To check which conversions are supported by the hardware for the current
	media bus frame format, see :ref:`v4l2-subdev-mbus-code-flags`.
	(由应用设置。仅用于源 pad，在汇 (sink) pad 上被忽略。设置该标志后，请求子设备把接收到的色彩空间转换为所请求的色彩空间值。如果色彩相关字段（``colorspace``、``xfer_func``、``ycbcr_enc``、``hsv_enc`` 或 ``quantization``）被设为 ``*_DEFAULT``，则该色彩设置将保持接收时的值不变。因此，要改变量化方式，只需将 ``quantization`` 字段设为非默认值（``V4L2_QUANTIZATION_FULL_RANGE`` 或 ``V4L2_QUANTIZATION_LIM_RANGE``），并将所有其他色彩字段设为 ``*_DEFAULT``。要了解硬件对当前媒体总线帧格式支持哪些转换，参见 :ref:`v4l2-subdev-mbus-code-flags`。)


.. _v4l2-mbus-pixelcode:

Media Bus Pixel Codes(媒体总线像素代码)
---------------------------------------

The media bus pixel codes describe image formats as flowing over
physical buses (both between separate physical components and inside
SoC devices). This should not be confused with the V4L2 pixel formats
that describe, using four character codes, image formats as stored in
memory.
(媒体总线像素代码描述的是图像格式在物理总线上的流动形态（既包括独立物理器件之间，也包括 SoC 器件内部）。这不应与 V4L2 像素格式相混淆，后者用四字符代码描述图像格式在内存中的存储形态。)

While there is a relationship between image formats on buses and image
formats in memory (a raw Bayer image won't be magically converted to
JPEG just by storing it to memory), there is no one-to-one
correspondence between them.
(总线上的图像格式与内存中的图像格式之间确实存在关联（把原始 Bayer 图像存入内存并不会奇迹般地变成 JPEG），但二者之间并不存在一一对应关系。)

While the media bus pixel codes are named based on how pixels are
transmitted on parallel buses, serial buses do not define separate
codes. By convention, they use the codes that transfer a sample on a
single clock cycle, and whose bit orders from LSB to MSB correspond to
the order in which colour components are transmitted on the serial bus.
For instance, the MIPI CSI-2 24-bit RGB (RGB888) format uses the
MEDIA_BUS_FMT_RGB888_1X24 media bus code because CSI-2 transmits the
blue colour component first, followed by green and red, and
MEDIA_BUS_FMT_RGB888_1X24 defines the first bit of blue at bit 0.
While used for 24-bit RGB data on parallel buses, the
MEDIA_BUS_FMT_RGB888_3X8 or MEDIA_BUS_FMT_BGR888_1X24 codes must not be
used for CSI-2.
(媒体总线像素代码按像素在并行总线上的传输方式来命名，而串行总线不定义单独的代码。按惯例，串行总线使用在单个时钟周期内传输一个采样、且从 LSB 到 MSB 的位序与色彩分量在串行总线上的传输顺序相对应的那些代码。例如，MIPI CSI-2 的 24 位 RGB (RGB888) 格式使用 MEDIA_BUS_FMT_RGB888_1X24 媒体总线代码，因为 CSI-2 先传输蓝色分量，随后是绿色和红色，而 MEDIA_BUS_FMT_RGB888_1X24 将蓝色的第一个位定义在第 0 位。MEDIA_BUS_FMT_RGB888_3X8 或 MEDIA_BUS_FMT_BGR888_1X24 代码虽然在并行总线上用于 24 位 RGB 数据，但不得用于 CSI-2。)

Packed RGB Formats(打包 RGB 格式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Those formats transfer pixel data as red, green and blue components. The
format code is made of the following information.
(这些格式以红、绿、蓝分量的形式传输像素数据。格式代码由以下信息构成。)

-  The red, green and blue components order code, as encoded in a pixel
   sample. Possible values are RGB and BGR.
   (像素采样中编码的红、绿、蓝分量顺序代码。可能的取值为 RGB 和 BGR。)

-  The number of bits per component, for each component. The values can
   be different for all components. Common values are 555 and 565.
   (每个分量的位数。各分量的位数可以不同。常见取值为 555 和 565。)

-  The number of bus samples per pixel. Pixels that are wider than the
   bus width must be transferred in multiple samples. Common values are
   1 and 2.
   (每像素的总线采样数。比总线宽度宽的像素必须分多个采样传输。常见取值为 1 和 2。)

-  The bus width.
   (总线宽度。)

-  For formats where the total number of bits per pixel is smaller than
   the number of bus samples per pixel times the bus width, a padding
   value stating if the bytes are padded in their most high order bits
   (PADHI) or low order bits (PADLO). A "C" prefix is used for
   component-wise padding in the most high order bits (CPADHI) or low
   order bits (CPADLO) of each separate component.
   (对于每像素总位数小于每像素总线采样数乘以总线宽度的格式，用一个填充值指明字节是在高位填充 (PADHI) 还是在低位填充 (PADLO)。前缀 "C" 表示按分量填充，即在每个独立分量的高位填充 (CPADHI) 或低位填充 (CPADLO)。)

-  For formats where the number of bus samples per pixel is larger than
   1, an endianness value stating if the pixel is transferred MSB first
   (BE) or LSB first (LE).
   (对于每像素总线采样数大于 1 的格式，用一个字节序值指明像素是 MSB 在前 (BE) 还是 LSB 在前 (LE) 传输。)

For instance, a format where pixels are encoded as 5-bits red, 5-bits
green and 5-bit blue values padded on the high bit, transferred as 2
8-bit samples per pixel with the most significant bits (padding, red and
half of the green value) transferred first will be named
``MEDIA_BUS_FMT_RGB555_2X8_PADHI_BE``.
(例如，一种格式：像素编码为 5 位红、5 位绿、5 位蓝并在高位填充，以每像素 2 个 8 位采样传输，且最高有效位（填充位、红色以及绿色值的一半）先传输，它将被命名为 ``MEDIA_BUS_FMT_RGB555_2X8_PADHI_BE``。)

The following tables list existing packed RGB formats.
(下列各表列出现有的打包 RGB 格式。)

.. HACK: ideally, we would be using adjustbox here. However, Sphinx
.. is a very bad behaviored guy: if the table has more than 30 cols,
.. it switches to long table, and there's no way to override it.


.. tabularcolumns:: |p{5.0cm}|p{0.7cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-rgb:

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. flat-table:: RGB formats
    :header-rows:  2
    :stub-columns: 0
    :widths: 36 7 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`31` Data organization(数据组织)
    * -
      -
      - Bit(位)
      - 31
      - 30
      - 29
      - 28
      - 27
      - 26
      - 25
      - 24
      - 23
      - 22
      - 21
      - 20
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-RGB444-1X12:

      - MEDIA_BUS_FMT_RGB444_1X12
      - 0x1016
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB444-2X8-PADHI-BE:

      - MEDIA_BUS_FMT_RGB444_2X8_PADHI_BE
      - 0x1001
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - 0
      - 0
      - 0
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB444-2X8-PADHI-LE:

      - MEDIA_BUS_FMT_RGB444_2X8_PADHI_LE
      - 0x1002
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - 0
      - 0
      - 0
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB555-2X8-PADHI-BE:

      - MEDIA_BUS_FMT_RGB555_2X8_PADHI_BE
      - 0x1003
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB555-2X8-PADHI-LE:

      - MEDIA_BUS_FMT_RGB555_2X8_PADHI_LE
      - 0x1004
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`4`
      - g\ :sub:`3`
    * .. _MEDIA-BUS-FMT-RGB565-1X16:

      - MEDIA_BUS_FMT_RGB565_1X16
      - 0x1017
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-BGR565-2X8-BE:

      - MEDIA_BUS_FMT_BGR565_2X8_BE
      - 0x1005
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-BGR565-2X8-LE:

      - MEDIA_BUS_FMT_BGR565_2X8_LE
      - 0x1006
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
    * .. _MEDIA-BUS-FMT-RGB565-2X8-BE:

      - MEDIA_BUS_FMT_RGB565_2X8_BE
      - 0x1007
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB565-2X8-LE:

      - MEDIA_BUS_FMT_RGB565_2X8_LE
      - 0x1008
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
    * .. _MEDIA-BUS-FMT-RGB666-1X18:

      - MEDIA_BUS_FMT_RGB666_1X18
      - 0x1009
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB666-2X9-BE:

      - MEDIA_BUS_FMT_RGB666_2X9_BE
      - 0x1025
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-BGR666-1X18:

      - MEDIA_BUS_FMT_BGR666_1X18
      - 0x1023
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RBG888-1X24:

      - MEDIA_BUS_FMT_RBG888_1X24
      - 0x100e
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB666-1X24_CPADHI:

      - MEDIA_BUS_FMT_RGB666_1X24_CPADHI
      - 0x1015
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - 0
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - 0
      - 0
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - 0
      - 0
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-BGR666-1X24_CPADHI:

      - MEDIA_BUS_FMT_BGR666_1X24_CPADHI
      - 0x1024
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - 0
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - 0
      - 0
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - 0
      - 0
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB565-1X24_CPADHI:

      - MEDIA_BUS_FMT_RGB565_1X24_CPADHI
      - 0x1022
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - 0
      - 0
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - 0
      - 0
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - 0
      - 0
      - 0
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-BGR888-1X24:

      - MEDIA_BUS_FMT_BGR888_1X24
      - 0x1013
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-BGR888-3X8:

      - MEDIA_BUS_FMT_BGR888_3X8
      - 0x101b
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-GBR888-1X24:

      - MEDIA_BUS_FMT_GBR888_1X24
      - 0x1014
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB888-1X24:

      - MEDIA_BUS_FMT_RGB888_1X24
      - 0x100a
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB888-2X12-BE:

      - MEDIA_BUS_FMT_RGB888_2X12_BE
      - 0x100b
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB888-2X12-LE:

      - MEDIA_BUS_FMT_RGB888_2X12_LE
      - 0x100c
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
    * .. _MEDIA-BUS-FMT-RGB888-3X8:

      - MEDIA_BUS_FMT_RGB888_3X8
      - 0x101c
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB666-1X30-CPADLO:

      - MEDIA_BUS_FMT_RGB666_1X30-CPADLO
      - 0x101e
      -
      -
      -
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
    * .. _MEDIA-BUS-FMT-RGB888-1X30-CPADLO:

      - MEDIA_BUS_FMT_RGB888_1X30-CPADLO
      - 0x101f
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - 0
      - 0
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - 0
      - 0
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - 0
      - 0
    * .. _MEDIA-BUS-FMT-ARGB888-1X32:

      - MEDIA_BUS_FMT_ARGB888_1X32
      - 0x100d
      -
      - a\ :sub:`7`
      - a\ :sub:`6`
      - a\ :sub:`5`
      - a\ :sub:`4`
      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB888-1X32-PADHI:

      - MEDIA_BUS_FMT_RGB888_1X32_PADHI
      - 0x100f
      -
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB101010-1X30:

      - MEDIA_BUS_FMT_RGB101010_1X30
      - 0x1018
      -
      -
      -
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

.. raw:: latex

    \endgroup


The following table list existing packed 36bit wide RGB formats.
(下表列出现有的 36 位宽打包 RGB 格式。)

.. tabularcolumns:: |p{4.0cm}|p{0.7cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-rgb-36:

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. flat-table:: 36bit RGB formats
    :header-rows:  2
    :stub-columns: 0
    :widths: 36 7 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`35` Data organization(数据组织)
    * -
      -
      - Bit(位)
      - 35
      - 34
      - 33
      - 32
      - 31
      - 30
      - 29
      - 28
      - 27
      - 26
      - 25
      - 24
      - 23
      - 22
      - 21
      - 20
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-RGB666-1X36-CPADLO:

      - MEDIA_BUS_FMT_RGB666_1X36_CPADLO
      - 0x1020
      -
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
    * .. _MEDIA-BUS-FMT-RGB888-1X36-CPADLO:

      - MEDIA_BUS_FMT_RGB888_1X36_CPADLO
      - 0x1021
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
    * .. _MEDIA-BUS-FMT-RGB121212-1X36:

      - MEDIA_BUS_FMT_RGB121212_1X36
      - 0x1019
      -
      - r\ :sub:`11`
      - r\ :sub:`10`
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`11`
      - b\ :sub:`10`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

.. raw:: latex

    \endgroup


The following table list existing packed 48bit wide RGB formats.
(下表列出现有的 48 位宽打包 RGB 格式。)

.. tabularcolumns:: |p{4.0cm}|p{0.7cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-rgb-48:

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. flat-table:: 48bit RGB formats
    :header-rows:  3
    :stub-columns: 0
    :widths: 36 7 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`31` Data organization(数据组织)
    * -
      -
      - Bit(位)
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 47
      - 46
      - 45
      - 44
      - 43
      - 42
      - 41
      - 40
      - 39
      - 38
      - 37
      - 36
      - 35
      - 34
      - 33
      - 32
    * -
      -
      -
      - 31
      - 30
      - 29
      - 28
      - 27
      - 26
      - 25
      - 24
      - 23
      - 22
      - 21
      - 20
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-RGB161616-1X48:

      - MEDIA_BUS_FMT_RGB161616_1X48
      - 0x101a
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`15`
      - r\ :sub:`14`
      - r\ :sub:`13`
      - r\ :sub:`12`
      - r\ :sub:`11`
      - r\ :sub:`10`
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * -
      -
      -
      - g\ :sub:`15`
      - g\ :sub:`14`
      - g\ :sub:`13`
      - g\ :sub:`12`
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`15`
      - b\ :sub:`14`
      - b\ :sub:`13`
      - b\ :sub:`12`
      - b\ :sub:`11`
      - b\ :sub:`10`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

.. raw:: latex

    \endgroup

The following table list existing packed 60bit wide RGB formats.
(下表列出现有的 60 位宽打包 RGB 格式。)

.. tabularcolumns:: |p{4.0cm}|p{0.7cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-rgb-60:

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. flat-table:: 60bit RGB formats
    :header-rows:  3
    :stub-columns: 0
    :widths: 36 7 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`31` Data organization(数据组织)
    * -
      -
      - Bit(位)
      -
      -
      -
      -
      - 59
      - 58
      - 57
      - 56
      - 55
      - 54
      - 53
      - 52
      - 51
      - 50
      - 49
      - 48
      - 47
      - 46
      - 45
      - 44
      - 43
      - 42
      - 41
      - 40
      - 39
      - 38
      - 37
      - 36
      - 35
      - 34
      - 33
      - 32
    * -
      -
      -
      - 31
      - 30
      - 29
      - 28
      - 27
      - 26
      - 25
      - 24
      - 23
      - 22
      - 21
      - 20
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-RGB202020-1X60:

      - MEDIA_BUS_FMT_RGB202020_1X60
      - 0x1026
      -
      -
      -
      -
      -
      - r\ :sub:`19`
      - r\ :sub:`18`
      - r\ :sub:`17`
      - r\ :sub:`16`
      - r\ :sub:`15`
      - r\ :sub:`14`
      - r\ :sub:`13`
      - r\ :sub:`12`
      - r\ :sub:`11`
      - r\ :sub:`10`
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
      - g\ :sub:`19`
      - g\ :sub:`18`
      - g\ :sub:`17`
      - g\ :sub:`16`
      - g\ :sub:`15`
      - g\ :sub:`14`
      - g\ :sub:`13`
      - g\ :sub:`12`
    * -
      -
      -
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
      - b\ :sub:`19`
      - b\ :sub:`18`
      - b\ :sub:`17`
      - b\ :sub:`16`
      - b\ :sub:`15`
      - b\ :sub:`14`
      - b\ :sub:`13`
      - b\ :sub:`12`
      - b\ :sub:`11`
      - b\ :sub:`10`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`

.. raw:: latex

    \endgroup

On LVDS buses, usually each sample is transferred serialized in seven
time slots per pixel clock, on three (18-bit) or four (24-bit) or five (30-bit)
differential data pairs at the same time. The remaining bits are used
for control signals as defined by SPWG/PSWG/VESA or JEIDA standards. The
24-bit RGB format serialized in seven time slots on four lanes using
JEIDA defined bit mapping will be named
``MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA``, for example.
(在 LVDS 总线上，通常每个采样在每个像素时钟内以串行方式在三个（18 位）、四个（24 位）或五个（30 位）差分数据对上同时传输，占用七个时间槽。其余的位用作 SPWG/PSWG/VESA 或 JEIDA 标准定义的控制信号。例如，在四个通道上以七个时间槽串行化、采用 JEIDA 所定义位映射的 24 位 RGB 格式将被命名为 ``MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA``。)

.. raw:: latex

    \small

.. _v4l2-mbus-pixelcode-rgb-lvds:

.. flat-table:: LVDS RGB formats
    :header-rows:  2
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      -
      -
      - :cspan:`4` Data organization(数据组织)
    * -
      -
      - Timeslot(时间槽)
      - Lane(通道)
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-RGB666-1X7X3-SPWG:

      - MEDIA_BUS_FMT_RGB666_1X7X3_SPWG
      - 0x1010
      - 0
      -
      -
      -
      - d
      - b\ :sub:`1`
      - g\ :sub:`0`
    * -
      -
      - 1
      -
      -
      -
      - d
      - b\ :sub:`0`
      - r\ :sub:`5`
    * -
      -
      - 2
      -
      -
      -
      - d
      - g\ :sub:`5`
      - r\ :sub:`4`
    * -
      -
      - 3
      -
      -
      -
      - b\ :sub:`5`
      - g\ :sub:`4`
      - r\ :sub:`3`
    * -
      -
      - 4
      -
      -
      -
      - b\ :sub:`4`
      - g\ :sub:`3`
      - r\ :sub:`2`
    * -
      -
      - 5
      -
      -
      -
      - b\ :sub:`3`
      - g\ :sub:`2`
      - r\ :sub:`1`
    * -
      -
      - 6
      -
      -
      -
      - b\ :sub:`2`
      - g\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB888-1X7X4-SPWG:

      - MEDIA_BUS_FMT_RGB888_1X7X4_SPWG
      - 0x1011
      - 0
      -
      -
      - d
      - d
      - b\ :sub:`1`
      - g\ :sub:`0`
    * -
      -
      - 1
      -
      -
      - b\ :sub:`7`
      - d
      - b\ :sub:`0`
      - r\ :sub:`5`
    * -
      -
      - 2
      -
      -
      - b\ :sub:`6`
      - d
      - g\ :sub:`5`
      - r\ :sub:`4`
    * -
      -
      - 3
      -
      -
      - g\ :sub:`7`
      - b\ :sub:`5`
      - g\ :sub:`4`
      - r\ :sub:`3`
    * -
      -
      - 4
      -
      -
      - g\ :sub:`6`
      - b\ :sub:`4`
      - g\ :sub:`3`
      - r\ :sub:`2`
    * -
      -
      - 5
      -
      -
      - r\ :sub:`7`
      - b\ :sub:`3`
      - g\ :sub:`2`
      - r\ :sub:`1`
    * -
      -
      - 6
      -
      -
      - r\ :sub:`6`
      - b\ :sub:`2`
      - g\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB888-1X7X4-JEIDA:

      - MEDIA_BUS_FMT_RGB888_1X7X4_JEIDA
      - 0x1012
      - 0
      -
      -
      - d
      - d
      - b\ :sub:`3`
      - g\ :sub:`2`
    * -
      -
      - 1
      -
      -
      - b\ :sub:`1`
      - d
      - b\ :sub:`2`
      - r\ :sub:`7`
    * -
      -
      - 2
      -
      -
      - b\ :sub:`0`
      - d
      - g\ :sub:`7`
      - r\ :sub:`6`
    * -
      -
      - 3
      -
      -
      - g\ :sub:`1`
      - b\ :sub:`7`
      - g\ :sub:`6`
      - r\ :sub:`5`
    * -
      -
      - 4
      -
      -
      - g\ :sub:`0`
      - b\ :sub:`6`
      - g\ :sub:`5`
      - r\ :sub:`4`
    * -
      -
      - 5
      -
      -
      - r\ :sub:`1`
      - b\ :sub:`5`
      - g\ :sub:`4`
      - r\ :sub:`3`
    * -
      -
      - 6
      -
      -
      - r\ :sub:`0`
      - b\ :sub:`4`
      - g\ :sub:`3`
      - r\ :sub:`2`
    * .. _MEDIA-BUS-FMT-RGB101010-1X7X5-SPWG:

      - MEDIA_BUS_FMT_RGB101010_1X7X5_SPWG
      - 0x1026
      - 0
      -
      - d
      - d
      - d
      - b\ :sub:`1`
      - g\ :sub:`0`
    * -
      -
      - 1
      -
      - b\ :sub:`9`
      - b\ :sub:`7`
      - d
      - b\ :sub:`0`
      - r\ :sub:`5`
    * -
      -
      - 2
      -
      - b\ :sub:`8`
      - b\ :sub:`6`
      - d
      - g\ :sub:`5`
      - r\ :sub:`4`
    * -
      -
      - 3
      -
      - g\ :sub:`9`
      - g\ :sub:`7`
      - b\ :sub:`5`
      - g\ :sub:`4`
      - r\ :sub:`3`
    * -
      -
      - 4
      -
      - g\ :sub:`8`
      - g\ :sub:`6`
      - b\ :sub:`4`
      - g\ :sub:`3`
      - r\ :sub:`2`
    * -
      -
      - 5
      -
      - r\ :sub:`9`
      - r\ :sub:`7`
      - b\ :sub:`3`
      - g\ :sub:`2`
      - r\ :sub:`1`
    * -
      -
      - 6
      -
      - r\ :sub:`8`
      - r\ :sub:`6`
      - b\ :sub:`2`
      - g\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-RGB101010-1X7X5-JEIDA:

      - MEDIA_BUS_FMT_RGB101010_1X7X5_JEIDA
      - 0x1027
      - 0
      -
      - d
      - d
      - d
      - b\ :sub:`5`
      - g\ :sub:`4`
    * -
      -
      - 1
      -
      - b\ :sub:`1`
      - b\ :sub:`3`
      - d
      - b\ :sub:`4`
      - r\ :sub:`9`
    * -
      -
      - 2
      -
      - b\ :sub:`0`
      - b\ :sub:`2`
      - d
      - g\ :sub:`9`
      - r\ :sub:`8`
    * -
      -
      - 3
      -
      - g\ :sub:`1`
      - g\ :sub:`3`
      - b\ :sub:`9`
      - g\ :sub:`8`
      - r\ :sub:`7`
    * -
      -
      - 4
      -
      - g\ :sub:`0`
      - g\ :sub:`2`
      - b\ :sub:`8`
      - g\ :sub:`7`
      - r\ :sub:`6`
    * -
      -
      - 5
      -
      - r\ :sub:`1`
      - r\ :sub:`3`
      - b\ :sub:`7`
      - g\ :sub:`6`
      - r\ :sub:`5`
    * -
      -
      - 6
      -
      - r\ :sub:`0`
      - r\ :sub:`2`
      - b\ :sub:`6`
      - g\ :sub:`5`
      - r\ :sub:`4`

.. raw:: latex

    \normalsize


Bayer Formats(Bayer 格式)
^^^^^^^^^^^^^^^^^^^^^^^^^

Those formats transfer pixel data as red, green and blue components. The
format code is made of the following information.
(这些格式以红、绿、蓝分量的形式传输像素数据。格式代码由以下信息构成。)

-  The red, green and blue components order code, as encoded in a pixel
   sample. The possible values are shown in :ref:`bayer-patterns`.
   (像素采样中编码的红、绿、蓝分量顺序代码。可能的取值见 :ref:`bayer-patterns`。)

-  The number of bits per pixel component. All components are
   transferred on the same number of bits. Common values are 8, 10 and
   12.
   (每像素分量的位数。所有分量都以相同的位数传输。常见取值为 8、10 和 12。)

-  The compression (optional). If the pixel components are ALAW- or
   DPCM-compressed, a mention of the compression scheme and the number
   of bits per compressed pixel component.
   (压缩（可选）。如果像素分量经过 ALAW 或 DPCM 压缩，则还需注明压缩方案以及每个压缩后像素分量的位数。)

-  The number of bus samples per pixel. Pixels that are wider than the
   bus width must be transferred in multiple samples. Common values are
   1 and 2.
   (每像素的总线采样数。比总线宽度宽的像素必须分多个采样传输。常见取值为 1 和 2。)

-  The bus width.
   (总线宽度。)

-  For formats where the total number of bits per pixel is smaller than
   the number of bus samples per pixel times the bus width, a padding
   value stating if the bytes are padded in their most high order bits
   (PADHI) or low order bits (PADLO).
   (对于每像素总位数小于每像素总线采样数乘以总线宽度的格式，用一个填充值指明字节是在高位填充 (PADHI) 还是在低位填充 (PADLO)。)

-  For formats where the number of bus samples per pixel is larger than
   1, an endianness value stating if the pixel is transferred MSB first
   (BE) or LSB first (LE).
   (对于每像素总线采样数大于 1 的格式，用一个字节序值指明像素是 MSB 在前 (BE) 还是 LSB 在前 (LE) 传输。)

For instance, a format with uncompressed 10-bit Bayer components
arranged in a red, green, green, blue pattern transferred as 2 8-bit
samples per pixel with the least significant bits transferred first will
be named ``MEDIA_BUS_FMT_SRGGB10_2X8_PADHI_LE``.
(例如，一种格式：未压缩的 10 位 Bayer 分量按红、绿、绿、蓝排列，以每像素 2 个 8 位采样传输且最低有效位先传输，它将被命名为 ``MEDIA_BUS_FMT_SRGGB10_2X8_PADHI_LE``。)


.. _bayer-patterns:

.. kernel-figure:: bayer.svg
    :alt:    bayer.svg
    :align:  center

    Bayer Patterns(Bayer 图案)

The following table lists existing packed Bayer formats. The data
organization is given as an example for the first pixel only.
(下表列出现有的打包 Bayer 格式。数据组织仅以第一个像素为例给出。)


.. HACK: ideally, we would be using adjustbox here. However, Sphinx
.. is a very bad behaviored guy: if the table has more than 30 cols,
.. it switches to long table, and there's no way to override it.


.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{6.0cm}|p{0.7cm}|p{0.3cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-bayer:

.. cssclass: longtable

.. flat-table:: Bayer Formats
    :header-rows:  2
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`19` Data organization(数据组织)
    * -
      -
      - Bit(位)
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-SBGGR8-1X8:

      - MEDIA_BUS_FMT_SBGGR8_1X8
      - 0x3001
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGBRG8-1X8:

      - MEDIA_BUS_FMT_SGBRG8_1X8
      - 0x3013
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGRBG8-1X8:

      - MEDIA_BUS_FMT_SGRBG8_1X8
      - 0x3002
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SRGGB8-1X8:

      - MEDIA_BUS_FMT_SRGGB8_1X8
      - 0x3014
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SBGGR10-ALAW8-1X8:

      - MEDIA_BUS_FMT_SBGGR10_ALAW8_1X8
      - 0x3015
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGBRG10-ALAW8-1X8:

      - MEDIA_BUS_FMT_SGBRG10_ALAW8_1X8
      - 0x3016
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGRBG10-ALAW8-1X8:

      - MEDIA_BUS_FMT_SGRBG10_ALAW8_1X8
      - 0x3017
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SRGGB10-ALAW8-1X8:

      - MEDIA_BUS_FMT_SRGGB10_ALAW8_1X8
      - 0x3018
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SBGGR10-DPCM8-1X8:

      - MEDIA_BUS_FMT_SBGGR10_DPCM8_1X8
      - 0x300b
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGBRG10-DPCM8-1X8:

      - MEDIA_BUS_FMT_SGBRG10_DPCM8_1X8
      - 0x300c
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGRBG10-DPCM8-1X8:

      - MEDIA_BUS_FMT_SGRBG10_DPCM8_1X8
      - 0x3009
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SRGGB10-DPCM8-1X8:

      - MEDIA_BUS_FMT_SRGGB10_DPCM8_1X8
      - 0x300d
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SBGGR10-2X8-PADHI-BE:

      - MEDIA_BUS_FMT_SBGGR10_2X8_PADHI_BE
      - 0x3003
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
      - b\ :sub:`9`
      - b\ :sub:`8`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SBGGR10-2X8-PADHI-LE:

      - MEDIA_BUS_FMT_SBGGR10_2X8_PADHI_LE
      - 0x3004
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
      - b\ :sub:`9`
      - b\ :sub:`8`
    * .. _MEDIA-BUS-FMT-SBGGR10-2X8-PADLO-BE:

      - MEDIA_BUS_FMT_SBGGR10_2X8_PADLO_BE
      - 0x3005
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`1`
      - b\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
    * .. _MEDIA-BUS-FMT-SBGGR10-2X8-PADLO-LE:

      - MEDIA_BUS_FMT_SBGGR10_2X8_PADLO_LE
      - 0x3006
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`1`
      - b\ :sub:`0`
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
    * .. _MEDIA-BUS-FMT-SBGGR10-1X10:

      - MEDIA_BUS_FMT_SBGGR10_1X10
      - 0x3007
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGBRG10-1X10:

      - MEDIA_BUS_FMT_SGBRG10_1X10
      - 0x300e
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGRBG10-1X10:

      - MEDIA_BUS_FMT_SGRBG10_1X10
      - 0x300a
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SRGGB10-1X10:

      - MEDIA_BUS_FMT_SRGGB10_1X10
      - 0x300f
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SBGGR12-1X12:

      - MEDIA_BUS_FMT_SBGGR12_1X12
      - 0x3008
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`11`
      - b\ :sub:`10`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGBRG12-1X12:

      - MEDIA_BUS_FMT_SGBRG12_1X12
      - 0x3010
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGRBG12-1X12:

      - MEDIA_BUS_FMT_SGRBG12_1X12
      - 0x3011
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SRGGB12-1X12:

      - MEDIA_BUS_FMT_SRGGB12_1X12
      - 0x3012
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`11`
      - r\ :sub:`10`
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SBGGR14-1X14:

      - MEDIA_BUS_FMT_SBGGR14_1X14
      - 0x3019
      -
      -
      -
      -
      -
      -
      -
      - b\ :sub:`13`
      - b\ :sub:`12`
      - b\ :sub:`11`
      - b\ :sub:`10`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGBRG14-1X14:

      - MEDIA_BUS_FMT_SGBRG14_1X14
      - 0x301a
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`13`
      - g\ :sub:`12`
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGRBG14-1X14:

      - MEDIA_BUS_FMT_SGRBG14_1X14
      - 0x301b
      -
      -
      -
      -
      -
      -
      -
      - g\ :sub:`13`
      - g\ :sub:`12`
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SRGGB14-1X14:

      - MEDIA_BUS_FMT_SRGGB14_1X14
      - 0x301c
      -
      -
      -
      -
      -
      -
      -
      - r\ :sub:`13`
      - r\ :sub:`12`
      - r\ :sub:`11`
      - r\ :sub:`10`
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SBGGR16-1X16:

      - MEDIA_BUS_FMT_SBGGR16_1X16
      - 0x301d
      -
      -
      -
      -
      -
      - b\ :sub:`15`
      - b\ :sub:`14`
      - b\ :sub:`13`
      - b\ :sub:`12`
      - b\ :sub:`11`
      - b\ :sub:`10`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGBRG16-1X16:

      - MEDIA_BUS_FMT_SGBRG16_1X16
      - 0x301e
      -
      -
      -
      -
      -
      - g\ :sub:`15`
      - g\ :sub:`14`
      - g\ :sub:`13`
      - g\ :sub:`12`
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGRBG16-1X16:

      - MEDIA_BUS_FMT_SGRBG16_1X16
      - 0x301f
      -
      -
      -
      -
      -
      - g\ :sub:`15`
      - g\ :sub:`14`
      - g\ :sub:`13`
      - g\ :sub:`12`
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SRGGB16-1X16:

      - MEDIA_BUS_FMT_SRGGB16_1X16
      - 0x3020
      -
      -
      -
      -
      -
      - r\ :sub:`15`
      - r\ :sub:`14`
      - r\ :sub:`13`
      - r\ :sub:`12`
      - r\ :sub:`11`
      - r\ :sub:`10`
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SBGGR20-1X20:

      - MEDIA_BUS_FMT_SBGGR20_1X20
      - 0x3021
      -
      - b\ :sub:`19`
      - b\ :sub:`18`
      - b\ :sub:`17`
      - b\ :sub:`16`
      - b\ :sub:`15`
      - b\ :sub:`14`
      - b\ :sub:`13`
      - b\ :sub:`12`
      - b\ :sub:`11`
      - b\ :sub:`10`
      - b\ :sub:`9`
      - b\ :sub:`8`
      - b\ :sub:`7`
      - b\ :sub:`6`
      - b\ :sub:`5`
      - b\ :sub:`4`
      - b\ :sub:`3`
      - b\ :sub:`2`
      - b\ :sub:`1`
      - b\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGBRG20-1X20:

      - MEDIA_BUS_FMT_SGBRG20_1X20
      - 0x3022
      -
      - g\ :sub:`19`
      - g\ :sub:`18`
      - g\ :sub:`17`
      - g\ :sub:`16`
      - g\ :sub:`15`
      - g\ :sub:`14`
      - g\ :sub:`13`
      - g\ :sub:`12`
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SGRBG20-1X20:

      - MEDIA_BUS_FMT_SGRBG20_1X20
      - 0x3023
      -
      - g\ :sub:`19`
      - g\ :sub:`18`
      - g\ :sub:`17`
      - g\ :sub:`16`
      - g\ :sub:`15`
      - g\ :sub:`14`
      - g\ :sub:`13`
      - g\ :sub:`12`
      - g\ :sub:`11`
      - g\ :sub:`10`
      - g\ :sub:`9`
      - g\ :sub:`8`
      - g\ :sub:`7`
      - g\ :sub:`6`
      - g\ :sub:`5`
      - g\ :sub:`4`
      - g\ :sub:`3`
      - g\ :sub:`2`
      - g\ :sub:`1`
      - g\ :sub:`0`
    * .. _MEDIA-BUS-FMT-SRGGB20-1X20:

      - MEDIA_BUS_FMT_SRGGB20_1X20
      - 0x3024
      -
      - r\ :sub:`19`
      - r\ :sub:`18`
      - r\ :sub:`17`
      - r\ :sub:`16`
      - r\ :sub:`15`
      - r\ :sub:`14`
      - r\ :sub:`13`
      - r\ :sub:`12`
      - r\ :sub:`11`
      - r\ :sub:`10`
      - r\ :sub:`9`
      - r\ :sub:`8`
      - r\ :sub:`7`
      - r\ :sub:`6`
      - r\ :sub:`5`
      - r\ :sub:`4`
      - r\ :sub:`3`
      - r\ :sub:`2`
      - r\ :sub:`1`
      - r\ :sub:`0`

.. raw:: latex

    \endgroup


Packed YUV Formats(打包 YUV 格式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Those data formats transfer pixel data as (possibly downsampled) Y, U
and V components. Some formats include dummy bits in some of their
samples and are collectively referred to as "YDYC" (Y-Dummy-Y-Chroma)
formats. One cannot rely on the values of these dummy bits as those are
undefined.
(这些数据格式以（可能经下采样的）Y、U、V 分量的形式传输像素数据。一些格式在其部分采样中包含哑元位 (dummy bits)，这类格式统称为 "YDYC" (Y-Dummy-Y-Chroma) 格式。这些哑元位的值没有定义，不能依赖其取值。)

The format code is made of the following information.
(格式代码由以下信息构成。)

-  The Y, U and V components order code, as transferred on the bus.
   Possible values are YUYV, UYVY, YVYU and VYUY for formats with no
   dummy bit, and YDYUYDYV, YDYVYDYU, YUYDYVYD and YVYDYUYD for YDYC
   formats.
   (总线上传输的 Y、U、V 分量顺序代码。无哑元位的格式取值为 YUYV、UYVY、YVYU 和 VYUY；YDYC 格式的取值为 YDYUYDYV、YDYVYDYU、YUYDYVYD 和 YVYDYUYD。)

-  The number of bits per pixel component. All components are
   transferred on the same number of bits. Common values are 8, 10 and
   12.
   (每像素分量的位数。所有分量都以相同的位数传输。常见取值为 8、10 和 12。)

-  The number of bus samples per pixel. Pixels that are wider than the
   bus width must be transferred in multiple samples. Common values are
   0.5 (encoded as 0_5; in this case two pixels are transferred per bus
   sample), 1, 1.5 (encoded as 1_5) and 2.
   (每像素的总线采样数。比总线宽度宽的像素必须分多个采样传输。常见取值为 0.5（编码为 0_5；此时每个总线采样传输两个像素）、1、1.5（编码为 1_5）和 2。)

-  The bus width. When the bus width is larger than the number of bits
   per pixel component, several components are packed in a single bus
   sample. The components are ordered as specified by the order code,
   with components on the left of the code transferred in the high order
   bits. Common values are 8 and 16.
   (总线宽度。当总线宽度大于每像素分量的位数时，多个分量被打包进单个总线采样。分量按顺序代码的规定排列，代码中靠左的分量在高位传输。常见取值为 8 和 16。)

For instance, a format where pixels are encoded as 8-bit YUV values
downsampled to 4:2:2 and transferred as 2 8-bit bus samples per pixel in
the U, Y, V, Y order will be named ``MEDIA_BUS_FMT_UYVY8_2X8``.
(例如，一种格式：像素编码为 8 位 YUV 值、下采样到 4:2:2、以每像素 2 个 8 位总线采样按 U、Y、V、Y 顺序传输，它将被命名为 ``MEDIA_BUS_FMT_UYVY8_2X8``。)

:ref:`v4l2-mbus-pixelcode-yuv8` lists existing packed YUV formats and
describes the organization of each pixel data in each sample. When a
format pattern is split across multiple samples each of the samples in
the pattern is described.
(:ref:`v4l2-mbus-pixelcode-yuv8` 列出现有的打包 YUV 格式，并描述每个采样中各像素数据的组织方式。当一个格式模式被拆分到多个采样时，会对该模式中的每个采样分别加以描述。)

The role of each bit transferred over the bus is identified by one of
the following codes.
(总线上传输的每个位的用途由以下代码之一标识。)

-  y\ :sub:`x` for luma component bit number x
   (亮度分量位号 x)

-  u\ :sub:`x` for blue chroma component bit number x
   (蓝色色度分量位号 x)

-  v\ :sub:`x` for red chroma component bit number x
   (红色色度分量位号 x)

-  a\ :sub:`x` for alpha component bit number x
   (Alpha 分量位号 x)

- for non-available bits (for positions higher than the bus width)
  (不可用位（位置高于总线宽度时）)

-  d for dummy bits
   (哑元位)

.. HACK: ideally, we would be using adjustbox here. However, this
.. will never work for this table, as, even with tiny font, it is
.. to big for a single page. So, we need to manually adjust the
.. size.

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{5.0cm}|p{0.7cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-yuv8:

.. flat-table:: YUV Formats
    :header-rows:  2
    :stub-columns: 0
    :widths: 36 7 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`31` Data organization(数据组织)
    * -
      -
      - Bit(位)
      - 31
      - 30
      - 29
      - 28
      - 27
      - 26
      - 25
      - 24
      - 23
      - 22
      - 21
      - 10
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-Y8-1X8:

      - MEDIA_BUS_FMT_Y8_1X8
      - 0x2001
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UV8-1X8:

      - MEDIA_BUS_FMT_UV8_1X8
      - 0x2015
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYVY8-1_5X8:

      - MEDIA_BUS_FMT_UYVY8_1_5X8
      - 0x2002
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-VYUY8-1_5X8:

      - MEDIA_BUS_FMT_VYUY8_1_5X8
      - 0x2003
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUYV8-1_5X8:

      - MEDIA_BUS_FMT_YUYV8_1_5X8
      - 0x2004
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YVYU8-1_5X8:

      - MEDIA_BUS_FMT_YVYU8_1_5X8
      - 0x2005
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYVY8-2X8:

      - MEDIA_BUS_FMT_UYVY8_2X8
      - 0x2006
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-VYUY8-2X8:

      - MEDIA_BUS_FMT_VYUY8_2X8
      - 0x2007
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUYV8-2X8:

      - MEDIA_BUS_FMT_YUYV8_2X8
      - 0x2008
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YVYU8-2X8:

      - MEDIA_BUS_FMT_YVYU8_2X8
      - 0x2009
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * .. _MEDIA-BUS-FMT-Y10-1X10:

      - MEDIA_BUS_FMT_Y10_1X10
      - 0x200a
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-Y10-2X8-PADHI_LE:

      - MEDIA_BUS_FMT_Y10_2X8_PADHI_LE
      - 0x202c
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 0
      - 0
      - 0
      - 0
      - 0
      - 0
      - y\ :sub:`9`
      - y\ :sub:`8`
    * .. _MEDIA-BUS-FMT-UYVY10-2X10:

      - MEDIA_BUS_FMT_UYVY10_2X10
      - 0x2018
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-VYUY10-2X10:

      - MEDIA_BUS_FMT_VYUY10_2X10
      - 0x2019
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUYV10-2X10:

      - MEDIA_BUS_FMT_YUYV10_2X10
      - 0x200b
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YVYU10-2X10:

      - MEDIA_BUS_FMT_YVYU10_2X10
      - 0x200c
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * .. _MEDIA-BUS-FMT-Y12-1X12:

      - MEDIA_BUS_FMT_Y12_1X12
      - 0x2013
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYVY12-2X12:

      - MEDIA_BUS_FMT_UYVY12_2X12
      - 0x201c
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-VYUY12-2X12:

      - MEDIA_BUS_FMT_VYUY12_2X12
      - 0x201d
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUYV12-2X12:

      - MEDIA_BUS_FMT_YUYV12_2X12
      - 0x201e
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YVYU12-2X12:

      - MEDIA_BUS_FMT_YVYU12_2X12
      - 0x201f
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * .. _MEDIA-BUS-FMT-Y14-1X14:

      - MEDIA_BUS_FMT_Y14_1X14
      - 0x202d
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`13`
      - y\ :sub:`12`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-Y16-1X16:

      - MEDIA_BUS_FMT_Y16_1X16
      - 0x202e
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`15`
      - y\ :sub:`14`
      - y\ :sub:`13`
      - y\ :sub:`12`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYVY8-1X16:

      - MEDIA_BUS_FMT_UYVY8_1X16
      - 0x200f
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-VYUY8-1X16:

      - MEDIA_BUS_FMT_VYUY8_1X16
      - 0x2010
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUYV8-1X16:

      - MEDIA_BUS_FMT_YUYV8_1X16
      - 0x2011
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YVYU8-1X16:

      - MEDIA_BUS_FMT_YVYU8_1X16
      - 0x2012
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YDYUYDYV8-1X16:

      - MEDIA_BUS_FMT_YDYUYDYV8_1X16
      - 0x2014
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - d
      - d
      - d
      - d
      - d
      - d
      - d
      - d
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - d
      - d
      - d
      - d
      - d
      - d
      - d
      - d
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYVY10-1X20:

      - MEDIA_BUS_FMT_UYVY10_1X20
      - 0x201a
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-VYUY10-1X20:

      - MEDIA_BUS_FMT_VYUY10_1X20
      - 0x201b
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUYV10-1X20:

      - MEDIA_BUS_FMT_YUYV10_1X20
      - 0x200d
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YVYU10-1X20:

      - MEDIA_BUS_FMT_YVYU10_1X20
      - 0x200e
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * .. _MEDIA-BUS-FMT-VUY8-1X24:

      - MEDIA_BUS_FMT_VUY8_1X24
      - 0x201a
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUV8-1X24:

      - MEDIA_BUS_FMT_YUV8_1X24
      - 0x2025
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYYVYY8-0-5X24:

      - MEDIA_BUS_FMT_UYYVYY8_0_5X24
      - 0x2026
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYVY12-1X24:

      - MEDIA_BUS_FMT_UYVY12_1X24
      - 0x2020
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-VYUY12-1X24:

      - MEDIA_BUS_FMT_VYUY12_1X24
      - 0x2021
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUYV12-1X24:

      - MEDIA_BUS_FMT_YUYV12_1X24
      - 0x2022
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YVYU12-1X24:

      - MEDIA_BUS_FMT_YVYU12_1X24
      - 0x2023
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUV10-1X30:

      - MEDIA_BUS_FMT_YUV10_1X30
      - 0x2016
      -
      -
      -
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYYVYY10-0-5X30:

      - MEDIA_BUS_FMT_UYYVYY10_0_5X30
      - 0x2027
      -
      -
      -
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-AYUV8-1X32:

      - MEDIA_BUS_FMT_AYUV8_1X32
      - 0x2017
      -
      - a\ :sub:`7`
      - a\ :sub:`6`
      - a\ :sub:`5`
      - a\ :sub:`4`
      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`


.. raw:: latex

	\endgroup


The following table list existing packed 36bit wide YUV formats.
(下表列出现有的 36 位宽打包 YUV 格式。)

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{4.1cm}|p{0.7cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-yuv8-36bit:

.. flat-table:: 36bit YUV Formats
    :header-rows:  2
    :stub-columns: 0
    :widths: 36 7 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`35` Data organization(数据组织)
    * -
      -
      - Bit(位)
      - 35
      - 34
      - 33
      - 32
      - 31
      - 30
      - 29
      - 28
      - 27
      - 26
      - 25
      - 24
      - 23
      - 22
      - 21
      - 10
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-UYYVYY12-0-5X36:

      - MEDIA_BUS_FMT_UYYVYY12_0_5X36
      - 0x2028
      -
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * .. _MEDIA-BUS-FMT-YUV12-1X36:

      - MEDIA_BUS_FMT_YUV12_1X36
      - 0x2029
      -
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`


.. raw:: latex

	\endgroup


The following table list existing packed 48bit wide YUV formats.
(下表列出现有的 48 位宽打包 YUV 格式。)

.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{5.6cm}|p{0.7cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-yuv8-48bit:

.. flat-table:: 48bit YUV Formats
    :header-rows:  3
    :stub-columns: 0
    :widths: 36 7 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`31` Data organization(数据组织)
    * -
      -
      - Bit(位)
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - 47
      - 46
      - 45
      - 44
      - 43
      - 42
      - 41
      - 40
      - 39
      - 38
      - 37
      - 36
      - 35
      - 34
      - 33
      - 32
    * -
      -
      -
      - 31
      - 30
      - 29
      - 28
      - 27
      - 26
      - 25
      - 24
      - 23
      - 22
      - 21
      - 10
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-YUV16-1X48:

      - MEDIA_BUS_FMT_YUV16_1X48
      - 0x202a
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - y\ :sub:`15`
      - y\ :sub:`14`
      - y\ :sub:`13`
      - y\ :sub:`12`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`8`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      - u\ :sub:`15`
      - u\ :sub:`14`
      - u\ :sub:`13`
      - u\ :sub:`12`
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
      - v\ :sub:`15`
      - v\ :sub:`14`
      - v\ :sub:`13`
      - v\ :sub:`12`
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * .. _MEDIA-BUS-FMT-UYYVYY16-0-5X48:

      - MEDIA_BUS_FMT_UYYVYY16_0_5X48
      - 0x202b
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - u\ :sub:`15`
      - u\ :sub:`14`
      - u\ :sub:`13`
      - u\ :sub:`12`
      - u\ :sub:`11`
      - u\ :sub:`10`
      - u\ :sub:`9`
      - u\ :sub:`8`
      - u\ :sub:`7`
      - u\ :sub:`6`
      - u\ :sub:`5`
      - u\ :sub:`4`
      - u\ :sub:`3`
      - u\ :sub:`2`
      - u\ :sub:`1`
      - u\ :sub:`0`
    * -
      -
      -
      - y\ :sub:`15`
      - y\ :sub:`14`
      - y\ :sub:`13`
      - y\ :sub:`12`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - y\ :sub:`15`
      - y\ :sub:`14`
      - y\ :sub:`13`
      - y\ :sub:`12`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`8`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
    * -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - v\ :sub:`15`
      - v\ :sub:`14`
      - v\ :sub:`13`
      - v\ :sub:`12`
      - v\ :sub:`11`
      - v\ :sub:`10`
      - v\ :sub:`9`
      - v\ :sub:`8`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`
    * -
      -
      -
      - y\ :sub:`15`
      - y\ :sub:`14`
      - y\ :sub:`13`
      - y\ :sub:`12`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`9`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`
      - y\ :sub:`15`
      - y\ :sub:`14`
      - y\ :sub:`13`
      - y\ :sub:`12`
      - y\ :sub:`11`
      - y\ :sub:`10`
      - y\ :sub:`8`
      - y\ :sub:`8`
      - y\ :sub:`7`
      - y\ :sub:`6`
      - y\ :sub:`5`
      - y\ :sub:`4`
      - y\ :sub:`3`
      - y\ :sub:`2`
      - y\ :sub:`1`
      - y\ :sub:`0`


.. raw:: latex

	\endgroup

HSV/HSL Formats(HSV/HSL 格式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Those formats transfer pixel data as RGB values in a
cylindrical-coordinate system using Hue-Saturation-Value or
Hue-Saturation-Lightness components. The format code is made of the
following information.
(这些格式在采用色相-饱和度-明度(Hue-Saturation-Value)或色相-饱和度-亮度(Hue-Saturation-Lightness)分量的圆柱坐标系中，以 RGB 值的形式传输像素数据。格式代码由以下信息构成。)

-  The hue, saturation, value or lightness and optional alpha components
   order code, as encoded in a pixel sample. The only currently
   supported value is AHSV.
   (色相、饱和度、明度或亮度以及可选的 alpha 分量的顺序代码，按其在像素样本中的编码方式表示。目前唯一支持的取值是 AHSV。)

-  The number of bits per component, for each component. The values can
   be different for all components. The only currently supported value
   is 8888.
   (每个分量的位数，各分量的位数可以不同。目前唯一支持的取值是 8888。)

-  The number of bus samples per pixel. Pixels that are wider than the
   bus width must be transferred in multiple samples. The only currently
   supported value is 1.
   (每像素的总线采样数。宽于总线宽度的像素必须分多次采样传输。目前唯一支持的取值是 1。)

-  The bus width.
   (总线宽度。)

-  For formats where the total number of bits per pixel is smaller than
   the number of bus samples per pixel times the bus width, a padding
   value stating if the bytes are padded in their most high order bits
   (PADHI) or low order bits (PADLO).
   (对于每像素总位数小于每像素总线采样数乘以总线宽度的格式，此填充值表明字节是在最高位侧填充(PADHI)还是在最低位侧填充(PADLO)。)

-  For formats where the number of bus samples per pixel is larger than
   1, an endianness value stating if the pixel is transferred MSB first
   (BE) or LSB first (LE).
   (对于每像素总线采样数大于 1 的格式，此字节序值表明像素是 MSB 在前(BE)还是 LSB 在前(LE)传输。)

The following table lists existing HSV/HSL formats.
(下表列出现有的 HSV/HSL 格式。)


.. raw:: latex

    \begingroup
    \tiny
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{3.9cm}|p{0.73cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|p{0.22cm}|

.. _v4l2-mbus-pixelcode-hsv:

.. flat-table:: HSV/HSL formats
    :header-rows:  2
    :stub-columns: 0
    :widths: 28 7 3 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2 2

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`31` Data organization(数据组织)
    * -
      -
      - Bit(位)
      - 31
      - 30
      - 29
      - 28
      - 27
      - 26
      - 25
      - 24
      - 23
      - 22
      - 21
      - 20
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-AHSV8888-1X32:

      - MEDIA_BUS_FMT_AHSV8888_1X32
      - 0x6001
      -
      - a\ :sub:`7`
      - a\ :sub:`6`
      - a\ :sub:`5`
      - a\ :sub:`4`
      - a\ :sub:`3`
      - a\ :sub:`2`
      - a\ :sub:`1`
      - a\ :sub:`0`
      - h\ :sub:`7`
      - h\ :sub:`6`
      - h\ :sub:`5`
      - h\ :sub:`4`
      - h\ :sub:`3`
      - h\ :sub:`2`
      - h\ :sub:`1`
      - h\ :sub:`0`
      - s\ :sub:`7`
      - s\ :sub:`6`
      - s\ :sub:`5`
      - s\ :sub:`4`
      - s\ :sub:`3`
      - s\ :sub:`2`
      - s\ :sub:`1`
      - s\ :sub:`0`
      - v\ :sub:`7`
      - v\ :sub:`6`
      - v\ :sub:`5`
      - v\ :sub:`4`
      - v\ :sub:`3`
      - v\ :sub:`2`
      - v\ :sub:`1`
      - v\ :sub:`0`

.. raw:: latex

    \endgroup


JPEG Compressed Formats(JPEG 压缩格式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Those data formats consist of an ordered sequence of 8-bit bytes
obtained from JPEG compression process. Additionally to the ``_JPEG``
postfix the format code is made of the following information.
(这些数据格式由 JPEG 压缩过程产生的 8 位字节按顺序排列而成。除了 ``_JPEG`` 后缀之外，格式代码还由以下信息构成。)

-  The number of bus samples per entropy encoded byte.
   (每个熵编码字节的总线采样数。)

-  The bus width.
   (总线宽度。)

For instance, for a JPEG baseline process and an 8-bit bus width the
format will be named ``MEDIA_BUS_FMT_JPEG_1X8``.
(例如，对于 JPEG 基线(baseline)处理和 8 位总线宽度，该格式将被命名为 MEDIA_BUS_FMT_JPEG_1X8。)

The following table lists existing JPEG compressed formats.
(下表列出现有的 JPEG 压缩格式。)


.. _v4l2-mbus-pixelcode-jpeg:

.. tabularcolumns:: |p{6.0cm}|p{1.4cm}|p{9.9cm}|

.. flat-table:: JPEG Formats
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - Remarks(备注)
    * .. _MEDIA-BUS-FMT-JPEG-1X8:

      - MEDIA_BUS_FMT_JPEG_1X8
      - 0x4001
      - Besides of its usage for the parallel bus this format is
	recommended for transmission of JPEG data over MIPI CSI bus using
	the User Defined 8-bit Data types.(该格式除用于并行总线外，还推荐用于通过 MIPI CSI 总线、使用用户自定义 8 位数据类型(User Defined 8-bit Data types)传输 JPEG 数据。)



.. _v4l2-mbus-vendor-spec-fmts:

Vendor and Device Specific Formats(厂商和设备特定格式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section lists complex data formats that are either vendor or device
specific.
(本节列出厂商特定或设备特定的复杂数据格式。)

The following table lists the existing vendor and device specific
formats.
(下表列出现有的厂商和设备特定格式。)


.. _v4l2-mbus-pixelcode-vendor-specific:

.. tabularcolumns:: |p{8.0cm}|p{1.4cm}|p{7.9cm}|

.. flat-table:: Vendor and device specific formats
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - Comments(说明)
    * .. _MEDIA-BUS-FMT-S5C-UYVY-JPEG-1X8:

      - MEDIA_BUS_FMT_S5C_UYVY_JPEG_1X8
      - 0x5001
      - Interleaved raw UYVY and JPEG image format with embedded meta-data
	used by Samsung S3C73MX camera sensors.(交错的原始 UYVY 与 JPEG 图像格式，内嵌元数据，由 Samsung S3C73MX 相机传感器使用。)

.. _v4l2-mbus-metadata-fmts:

Metadata Formats(元数据格式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This section lists all metadata formats.
(本节列出所有元数据格式。)

The following table lists the existing metadata formats.
(下表列出现有的元数据格式。)

.. tabularcolumns:: |p{8.0cm}|p{1.4cm}|p{7.9cm}|

.. flat-table:: Metadata formats
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - Comments(说明)
    * .. _MEDIA-BUS-FMT-METADATA-FIXED:

      - MEDIA_BUS_FMT_METADATA_FIXED
      - 0x7001
      - This format should be used when the same driver handles
	both sides of the link and the bus format is a fixed
	metadata format that is not configurable from userspace.
	Width and height will be set to 0 for this format.(当同一个驱动程序处理链路两端、且总线格式是用户空间不可配置的固定元数据格式时，应使用此格式。此时宽度和高度将被设置为 0。)

Generic Serial Metadata Formats(通用串行元数据格式)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Generic serial metadata formats are used on serial buses where the actual data
content is more or less device specific but the data is transmitted and received
by multiple devices that do not process the data in any way, simply writing
it to system memory for processing in software at the end of the pipeline.
(通用串行元数据格式用于串行总线，其上的实际数据内容或多或少与设备相关，但数据由多个设备收发，这些设备不以任何方式处理数据，只是把数据写入系统内存，以便在管线末端交由软件处理。)

"b" in an array cell signifies a byte of data, followed by the number of the bit
and finally the bit number in subscript. "x" indicates a padding bit.
(数组单元格中的 "b" 表示一个数据字节，其后是该字节的编号，下标则是位编号。"x" 表示填充位。)

.. _media-bus-format-generic-meta:

.. cssclass: longtable

.. flat-table:: Generic Serial Metadata Formats
    :header-rows:  2
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      -
      - :cspan:`23` Data organization within bus :term:`Data Unit` (总线内按数据单元的数据组织)
    * -
      -
      - Bit(位)
      - 23
      - 22
      - 21
      - 20
      - 19
      - 18
      - 17
      - 16
      - 15
      - 14
      - 13
      - 12
      - 11
      - 10
      - 9
      - 8
      - 7
      - 6
      - 5
      - 4
      - 3
      - 2
      - 1
      - 0
    * .. _MEDIA-BUS-FMT-META-8:

      - MEDIA_BUS_FMT_META_8
      - 0x8001
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b0\ :sub:`7`
      - b0\ :sub:`6`
      - b0\ :sub:`5`
      - b0\ :sub:`4`
      - b0\ :sub:`3`
      - b0\ :sub:`2`
      - b0\ :sub:`1`
      - b0\ :sub:`0`
    * .. _MEDIA-BUS-FMT-META-10:

      - MEDIA_BUS_FMT_META_10
      - 0x8002
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b0\ :sub:`7`
      - b0\ :sub:`6`
      - b0\ :sub:`5`
      - b0\ :sub:`4`
      - b0\ :sub:`3`
      - b0\ :sub:`2`
      - b0\ :sub:`1`
      - b0\ :sub:`0`
      - x
      - x
    * .. _MEDIA-BUS-FMT-META-12:

      - MEDIA_BUS_FMT_META_12
      - 0x8003
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b0\ :sub:`7`
      - b0\ :sub:`6`
      - b0\ :sub:`5`
      - b0\ :sub:`4`
      - b0\ :sub:`3`
      - b0\ :sub:`2`
      - b0\ :sub:`1`
      - b0\ :sub:`0`
      - x
      - x
      - x
      - x
    * .. _MEDIA-BUS-FMT-META-14:

      - MEDIA_BUS_FMT_META_14
      - 0x8004
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b0\ :sub:`7`
      - b0\ :sub:`6`
      - b0\ :sub:`5`
      - b0\ :sub:`4`
      - b0\ :sub:`3`
      - b0\ :sub:`2`
      - b0\ :sub:`1`
      - b0\ :sub:`0`
      - x
      - x
      - x
      - x
      - x
      - x
    * .. _MEDIA-BUS-FMT-META-16:

      - MEDIA_BUS_FMT_META_16
      - 0x8005
      -
      -
      -
      -
      -
      -
      -
      -
      -
      - b0\ :sub:`7`
      - b0\ :sub:`6`
      - b0\ :sub:`5`
      - b0\ :sub:`4`
      - b0\ :sub:`3`
      - b0\ :sub:`2`
      - b0\ :sub:`1`
      - b0\ :sub:`0`
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
    * .. _MEDIA-BUS-FMT-META-20:

      - MEDIA_BUS_FMT_META_20
      - 0x8006
      -
      -
      -
      -
      -
      - b0\ :sub:`7`
      - b0\ :sub:`6`
      - b0\ :sub:`5`
      - b0\ :sub:`4`
      - b0\ :sub:`3`
      - b0\ :sub:`2`
      - b0\ :sub:`1`
      - b0\ :sub:`0`
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
    * .. _MEDIA-BUS-FMT-META-24:

      - MEDIA_BUS_FMT_META_24
      - 0x8007
      -
      - b0\ :sub:`7`
      - b0\ :sub:`6`
      - b0\ :sub:`5`
      - b0\ :sub:`4`
      - b0\ :sub:`3`
      - b0\ :sub:`2`
      - b0\ :sub:`1`
      - b0\ :sub:`0`
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
      - x
