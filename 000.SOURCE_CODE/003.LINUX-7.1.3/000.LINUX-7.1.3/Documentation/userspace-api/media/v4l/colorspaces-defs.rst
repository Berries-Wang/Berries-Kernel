.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

****************************************************
Defining Colorspaces in V4L2(在 V4L2 中定义色彩空间)
****************************************************

In V4L2 colorspaces are defined by four values. The first is the
colorspace identifier (enum :c:type:`v4l2_colorspace`)
which defines the chromaticities, the default transfer function, the
default Y'CbCr encoding and the default quantization method. The second
is the transfer function identifier (enum
:c:type:`v4l2_xfer_func`) to specify non-standard
transfer functions. The third is the Y'CbCr encoding identifier (enum
:c:type:`v4l2_ycbcr_encoding`) to specify
non-standard Y'CbCr encodings and the fourth is the quantization
identifier (enum :c:type:`v4l2_quantization`) to
specify non-standard quantization methods. Most of the time only the
colorspace field of struct :c:type:`v4l2_pix_format`
or struct :c:type:`v4l2_pix_format_mplane`
needs to be filled in.
(在 V4L2 中，色彩空间由四个值定义。第一个是色彩空间标识符 (enum :c:type:`v4l2_colorspace`)，它定义了色度坐标、默认传递函数、默认 Y'CbCr 编码和默认量化方法。第二个是传递函数标识符 (enum :c:type:`v4l2_xfer_func`)，用于指定非标准的传递函数。第三个是 Y'CbCr 编码标识符 (enum :c:type:`v4l2_ycbcr_encoding`)，用于指定非标准的 Y'CbCr 编码；第四个是量化标识符 (enum :c:type:`v4l2_quantization`)，用于指定非标准的量化方法。大多数情况下，只需填写 struct :c:type:`v4l2_pix_format` 或 struct :c:type:`v4l2_pix_format_mplane` 的 colorspace 字段即可。)

.. _hsv-colorspace:

On :ref:`HSV formats <hsv-formats>` the *Hue* is defined as the angle on
the cylindrical color representation. Usually this angle is measured in
degrees, i.e. 0-360. When we map this angle value into 8 bits, there are
two basic ways to do it: Divide the angular value by 2 (0-179), or use the
whole range, 0-255, dividing the angular value by 1.41. The enum
:c:type:`v4l2_hsv_encoding` specifies which encoding is used.
(在 :ref:`HSV formats <hsv-formats>` 中，色调 (Hue) 被定义为圆柱形颜色表示上的角度。通常这个角度以度为单位测量，即 0-360。当我们将这个角度值映射到 8 位时，有两种基本做法：将角度值除以 2 (0-179)，或者使用整个范围 0-255，将角度值除以 1.41。枚举 :c:type:`v4l2_hsv_encoding` 指定使用哪种编码。)

.. note:: The default R'G'B' quantization is full range for all
   colorspaces. HSV formats are always full range.
   (所有色彩空间的默认 R'G'B' 量化都是 full range (全范围)。HSV 格式始终是 full range。)

.. tabularcolumns:: |p{6.7cm}|p{10.8cm}|

.. c:type:: v4l2_colorspace

.. flat-table:: V4L2 Colorspaces
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Details(详细说明)
    * - ``V4L2_COLORSPACE_DEFAULT``
      - The default colorspace. This can be used by applications to let
	the driver fill in the colorspace.
	(默认色彩空间。应用程序可以用它来让驱动填充色彩空间。)
    * - ``V4L2_COLORSPACE_SMPTE170M``
      - See :ref:`col-smpte-170m`.(参见 :ref:`col-smpte-170m` 。)
    * - ``V4L2_COLORSPACE_REC709``
      - See :ref:`col-rec709`.(参见 :ref:`col-rec709` 。)
    * - ``V4L2_COLORSPACE_SRGB``
      - See :ref:`col-srgb`.(参见 :ref:`col-srgb` 。)
    * - ``V4L2_COLORSPACE_OPRGB``
      - See :ref:`col-oprgb`.(参见 :ref:`col-oprgb` 。)
    * - ``V4L2_COLORSPACE_BT2020``
      - See :ref:`col-bt2020`.(参见 :ref:`col-bt2020` 。)
    * - ``V4L2_COLORSPACE_DCI_P3``
      - See :ref:`col-dcip3`.(参见 :ref:`col-dcip3` 。)
    * - ``V4L2_COLORSPACE_SMPTE240M``
      - See :ref:`col-smpte-240m`.(参见 :ref:`col-smpte-240m` 。)
    * - ``V4L2_COLORSPACE_470_SYSTEM_M``
      - See :ref:`col-sysm`.(参见 :ref:`col-sysm` 。)
    * - ``V4L2_COLORSPACE_470_SYSTEM_BG``
      - See :ref:`col-sysbg`.(参见 :ref:`col-sysbg` 。)
    * - ``V4L2_COLORSPACE_JPEG``
      - See :ref:`col-jpeg`.(参见 :ref:`col-jpeg` 。)
    * - ``V4L2_COLORSPACE_RAW``
      - The raw colorspace. This is used for raw image capture where the
	image is minimally processed and is using the internal colorspace
	of the device. The software that processes an image using this
	'colorspace' will have to know the internals of the capture
	device.
	(原始色彩空间。用于原始图像捕获，此时图像仅做最少处理，并使用设备的内部色彩空间。使用这种 'colorspace' 处理图像的软件必须了解捕获设备的内部细节。)



.. c:type:: v4l2_xfer_func

.. tabularcolumns:: |p{5.5cm}|p{12.0cm}|

.. flat-table:: V4L2 Transfer Function
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Details(详细说明)
    * - ``V4L2_XFER_FUNC_DEFAULT``
      - Use the default transfer function as defined by the colorspace.(使用由色彩空间定义的默认传递函数。)
    * - ``V4L2_XFER_FUNC_709``
      - Use the Rec. 709 transfer function.(使用 Rec. 709 传递函数。)
    * - ``V4L2_XFER_FUNC_SRGB``
      - Use the sRGB transfer function.(使用 sRGB 传递函数。)
    * - ``V4L2_XFER_FUNC_OPRGB``
      - Use the opRGB transfer function.(使用 opRGB 传递函数。)
    * - ``V4L2_XFER_FUNC_SMPTE240M``
      - Use the SMPTE 240M transfer function.(使用 SMPTE 240M 传递函数。)
    * - ``V4L2_XFER_FUNC_NONE``
      - Do not use a transfer function (i.e. use linear RGB values).(不使用传递函数(即使用线性 RGB 值)。)
    * - ``V4L2_XFER_FUNC_DCI_P3``
      - Use the DCI-P3 transfer function.(使用 DCI-P3 传递函数。)
    * - ``V4L2_XFER_FUNC_SMPTE2084``
      - Use the SMPTE 2084 transfer function. See :ref:`xf-smpte-2084`.(使用 SMPTE 2084 传递函数。参见 :ref:`xf-smpte-2084` 。)



.. c:type:: v4l2_ycbcr_encoding

.. tabularcolumns:: |p{7.2cm}|p{10.3cm}|

.. flat-table:: V4L2 Y'CbCr Encodings
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Details(详细说明)
    * - ``V4L2_YCBCR_ENC_DEFAULT``
      - Use the default Y'CbCr encoding as defined by the colorspace.(使用由色彩空间定义的默认 Y'CbCr 编码。)
    * - ``V4L2_YCBCR_ENC_601``
      - Use the BT.601 Y'CbCr encoding.(使用 BT.601 Y'CbCr 编码。)
    * - ``V4L2_YCBCR_ENC_709``
      - Use the Rec. 709 Y'CbCr encoding.(使用 Rec. 709 Y'CbCr 编码。)
    * - ``V4L2_YCBCR_ENC_XV601``
      - Use the extended gamut xvYCC BT.601 encoding.(使用扩展色域 xvYCC BT.601 编码。)
    * - ``V4L2_YCBCR_ENC_XV709``
      - Use the extended gamut xvYCC Rec. 709 encoding.(使用扩展色域 xvYCC Rec. 709 编码。)
    * - ``V4L2_YCBCR_ENC_BT2020``
      - Use the default non-constant luminance BT.2020 Y'CbCr encoding.(使用默认的非常亮度 BT.2020 Y'CbCr 编码。)
    * - ``V4L2_YCBCR_ENC_BT2020_CONST_LUM``
      - Use the constant luminance BT.2020 Yc'CbcCrc encoding.(使用恒亮度 BT.2020 Yc'CbcCrc 编码。)
    * - ``V4L2_YCBCR_ENC_SMPTE_240M``
      - Use the SMPTE 240M Y'CbCr encoding.(使用 SMPTE 240M Y'CbCr 编码。)



.. c:type:: v4l2_hsv_encoding

.. tabularcolumns:: |p{6.5cm}|p{11.0cm}|

.. flat-table:: V4L2 HSV Encodings
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Details(详细说明)
    * - ``V4L2_HSV_ENC_180``
      - For the Hue, each LSB is two degrees.(对于色调 (Hue)，每个 LSB 为两度。)
    * - ``V4L2_HSV_ENC_256``
      - For the Hue, the 360 degrees are mapped into 8 bits, i.e. each
	LSB is roughly 1.41 degrees.
	(对于色调 (Hue)，360 度被映射到 8 位中，即每个 LSB 大约为 1.41 度。)



.. c:type:: v4l2_quantization

.. tabularcolumns:: |p{6.5cm}|p{11.0cm}|

.. flat-table:: V4L2 Quantization Methods
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Details(详细说明)
    * - ``V4L2_QUANTIZATION_DEFAULT``
      - Use the default quantization encoding as defined by the
	colorspace. This is always full range for R'G'B' and HSV.
	It is usually limited range for Y'CbCr.
	(使用由色彩空间定义的默认量化编码。对 R'G'B' 和 HSV 而言这始终是 full range (全范围)；对 Y'CbCr 而言通常是 limited range (限幅范围)。)
    * - ``V4L2_QUANTIZATION_FULL_RANGE``
      - Use the full range quantization encoding. I.e. the range [0…1] is
	mapped to [0…255] (with possible clipping to [1…254] to avoid the
	0x00 and 0xff values). Cb and Cr are mapped from [-0.5…0.5] to
	[0…255] (with possible clipping to [1…254] to avoid the 0x00 and
	0xff values).
	(使用 full range (全范围) 量化编码。即范围 [0…1] 被映射到 [0…255](可能会裁剪到 [1…254] 以避免出现 0x00 和 0xff 值)。Cb 和 Cr 从 [-0.5…0.5] 映射到 [0…255](可能会裁剪到 [1…254] 以避免出现 0x00 和 0xff 值)。)
    * - ``V4L2_QUANTIZATION_LIM_RANGE``
      - Use the limited range quantization encoding. I.e. the range [0…1]
	is mapped to [16…235]. Cb and Cr are mapped from [-0.5…0.5] to
	[16…240]. Limited Range cannot be used with HSV.
	(使用 limited range (限幅范围) 量化编码。即范围 [0…1] 被映射到 [16…235]。Cb 和 Cr 从 [-0.5…0.5] 映射到 [16…240]。Limited Range 不能与 HSV 一起使用。)
