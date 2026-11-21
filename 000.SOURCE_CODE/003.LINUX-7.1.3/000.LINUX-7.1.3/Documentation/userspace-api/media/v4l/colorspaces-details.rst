.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

**************************************************
Detailed Colorspace Descriptions(色彩空间详细说明)
**************************************************


.. _col-smpte-170m:

Colorspace SMPTE 170M (V4L2_COLORSPACE_SMPTE170M)(色彩空间 SMPTE 170M)
======================================================================

The :ref:`smpte170m` standard defines the colorspace used by NTSC and
PAL and by SDTV in general. The default transfer function is
``V4L2_XFER_FUNC_709``. The default Y'CbCr encoding is
``V4L2_YCBCR_ENC_601``. The default Y'CbCr quantization is limited
range. The chromaticities of the primary colors and the white reference
are:
(:ref:`smpte170m` 标准定义了 NTSC 与 PAL 乃至一般 SDTV 使用的色彩空间。默认传递函数是 ``V4L2_XFER_FUNC_709`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_601`` 。默认 Y'CbCr 量化是 limited range (限幅范围)。基色与白基准的色度坐标如下：)

.. flat-table:: SMPTE 170M Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.630
      - 0.340
    * - Green(绿)
      - 0.310
      - 0.595
    * - Blue(蓝)
      - 0.155
      - 0.070
    * - White Reference (D65)(白基准 (D65))
      - 0.3127
      - 0.3290


The red, green and blue chromaticities are also often referred to as the
SMPTE C set, so this colorspace is sometimes called SMPTE C as well.
(红、绿、蓝的色度坐标也常被称为 SMPTE C 组，因此该色彩空间有时也被称为 SMPTE C。)

The transfer function defined for SMPTE 170M is the same as the one
defined in Rec. 709.
(为 SMPTE 170M 定义的传递函数与 Rec. 709 中定义的相同。)

.. math::

    L' = -1.099(-L)^{0.45} + 0.099 \text{, for } L \le-0.018

    L' = 4.5L \text{, for } -0.018 < L < 0.018

    L' = 1.099L^{0.45} - 0.099 \text{, for } L \ge 0.018

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = -\left( \frac{L' - 0.099}{-1.099} \right) ^{\frac{1}{0.45}} \text{, for } L' \le -0.081

    L = \frac{L'}{4.5} \text{, for } -0.081 < L' < 0.081

    L = \left(\frac{L' + 0.099}{1.099}\right)^{\frac{1}{0.45} } \text{, for } L' \ge 0.081

The luminance (Y') and color difference (Cb and Cr) are obtained with
the following ``V4L2_YCBCR_ENC_601`` encoding:
(亮度 (Y') 与色差 (Cb 和 Cr) 用如下 ``V4L2_YCBCR_ENC_601`` 编码获得：)

.. math::

    Y' = 0.2990R' + 0.5870G' + 0.1140B'

    Cb = -0.1687R' - 0.3313G' + 0.5B'

    Cr = 0.5R' - 0.4187G' - 0.0813B'

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the range
[-0.5…0.5]. This conversion to Y'CbCr is identical to the one defined in
the :ref:`itu601` standard and this colorspace is sometimes called
BT.601 as well, even though BT.601 does not mention any color primaries.
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围。这一 Y'CbCr 转换与 :ref:`itu601` 标准中定义的完全相同，因此该色彩空间有时也被称为 BT.601，尽管 BT.601 并未提及任何基色。)

The default quantization is limited range, but full range is possible
although rarely seen.
(默认量化是 limited range，但也可以是 full range (全范围)，只是很少见。)


.. _col-rec709:

Colorspace Rec. 709 (V4L2_COLORSPACE_REC709)(色彩空间 Rec. 709)
===============================================================

The :ref:`itu709` standard defines the colorspace used by HDTV in
general. The default transfer function is ``V4L2_XFER_FUNC_709``. The
default Y'CbCr encoding is ``V4L2_YCBCR_ENC_709``. The default Y'CbCr
quantization is limited range. The chromaticities of the primary colors
and the white reference are:
(:ref:`itu709` 标准定义了一般 HDTV 使用的色彩空间。默认传递函数是 ``V4L2_XFER_FUNC_709`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_709`` 。默认 Y'CbCr 量化是 limited range。基色与白基准的色度坐标如下：)

.. flat-table:: Rec. 709 Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.640
      - 0.330
    * - Green(绿)
      - 0.300
      - 0.600
    * - Blue(蓝)
      - 0.150
      - 0.060
    * - White Reference (D65)(白基准 (D65))
      - 0.3127
      - 0.3290


The full name of this standard is Rec. ITU-R BT.709-5.
(该标准的全称是 Rec. ITU-R BT.709-5。)

Transfer function. Normally L is in the range [0…1], but for the
extended gamut xvYCC encoding values outside that range are allowed.
(传递函数。通常 L 在 [0…1] 范围内，但对于扩展色域 (extended gamut) 的 xvYCC 编码，允许超出该范围的值。)

.. math::

    L' = -1.099(-L)^{0.45} + 0.099 \text{, for } L \le -0.018

    L' = 4.5L \text{, for } -0.018 < L < 0.018

    L' = 1.099L^{0.45} - 0.099 \text{, for } L \ge 0.018

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = -\left( \frac{L' - 0.099}{-1.099} \right)^\frac{1}{0.45} \text{, for } L' \le -0.081

    L = \frac{L'}{4.5}\text{, for } -0.081 < L' < 0.081

    L = \left(\frac{L' + 0.099}{1.099}\right)^{\frac{1}{0.45} } \text{, for } L' \ge 0.081

The luminance (Y') and color difference (Cb and Cr) are obtained with
the following ``V4L2_YCBCR_ENC_709`` encoding:
(亮度 (Y') 与色差 (Cb 和 Cr) 用如下 ``V4L2_YCBCR_ENC_709`` 编码获得：)

.. math::

    Y' = 0.2126R' + 0.7152G' + 0.0722B'

    Cb = -0.1146R' - 0.3854G' + 0.5B'

    Cr = 0.5R' - 0.4542G' - 0.0458B'

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the range
[-0.5…0.5].
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围。)

The default quantization is limited range, but full range is possible
although rarely seen.
(默认量化是 limited range，但也可以是 full range，只是很少见。)

The ``V4L2_YCBCR_ENC_709`` encoding described above is the default for
this colorspace, but it can be overridden with ``V4L2_YCBCR_ENC_601``,
in which case the BT.601 Y'CbCr encoding is used.
(上文描述的 ``V4L2_YCBCR_ENC_709`` 编码是该色彩空间的默认编码，但可以用 ``V4L2_YCBCR_ENC_601`` 覆盖，此时使用 BT.601 Y'CbCr 编码。)

Two additional extended gamut Y'CbCr encodings are also possible with
this colorspace:
(该色彩空间还可以使用另外两种扩展色域 Y'CbCr 编码：)

The xvYCC 709 encoding (``V4L2_YCBCR_ENC_XV709``, :ref:`xvycc`) is
similar to the Rec. 709 encoding, but it allows for R', G' and B' values
that are outside the range [0…1]. The resulting Y', Cb and Cr values are
scaled and offset according to the limited range formula:
(xvYCC 709 编码 ( ``V4L2_YCBCR_ENC_XV709`` ，:ref:`xvycc` ) 与 Rec. 709 编码类似，但允许 R'、G'、B' 的值超出 [0…1] 范围。得到的 Y'、Cb、Cr 值按 limited range 公式进行缩放和偏移：)

.. math::

    Y' = \frac{219}{256} * (0.2126R' + 0.7152G' + 0.0722B') + \frac{16}{256}

    Cb = \frac{224}{256} * (-0.1146R' - 0.3854G' + 0.5B')

    Cr = \frac{224}{256} * (0.5R' - 0.4542G' - 0.0458B')

The xvYCC 601 encoding (``V4L2_YCBCR_ENC_XV601``, :ref:`xvycc`) is
similar to the BT.601 encoding, but it allows for R', G' and B' values
that are outside the range [0…1]. The resulting Y', Cb and Cr values are
scaled and offset according to the limited range formula:
(xvYCC 601 编码 ( ``V4L2_YCBCR_ENC_XV601`` ，:ref:`xvycc` ) 与 BT.601 编码类似，但允许 R'、G'、B' 的值超出 [0…1] 范围。得到的 Y'、Cb、Cr 值按 limited range 公式进行缩放和偏移：)

.. math::

    Y' = \frac{219}{256} * (0.2990R' + 0.5870G' + 0.1140B') + \frac{16}{256}

    Cb = \frac{224}{256} * (-0.1687R' - 0.3313G' + 0.5B')

    Cr = \frac{224}{256} * (0.5R' - 0.4187G' - 0.0813B')

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the range
[-0.5…0.5] and quantized without further scaling or offsets.
The non-standard xvYCC 709 or xvYCC 601 encodings can be
used by selecting ``V4L2_YCBCR_ENC_XV709`` or ``V4L2_YCBCR_ENC_XV601``.
As seen by the xvYCC formulas these encodings always use limited range quantization,
there is no full range variant. The whole point of these extended gamut encodings
is that values outside the limited range are still valid, although they
map to R', G' and B' values outside the [0…1] range and are therefore outside
the Rec. 709 colorspace gamut.
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围，并在不做进一步缩放或偏移的情况下量化。选择 ``V4L2_YCBCR_ENC_XV709`` 或 ``V4L2_YCBCR_ENC_XV601`` 即可使用非标准的 xvYCC 709 或 xvYCC 601 编码。从 xvYCC 公式可以看出，这些编码总是使用 limited range 量化，没有 full range 变体。这些扩展色域编码的全部意义在于：超出 limited range 的值仍然有效，尽管它们映射到 [0…1] 范围之外的 R'、G'、B' 值，因而处于 Rec. 709 色彩空间色域之外。)


.. _col-srgb:

Colorspace sRGB (V4L2_COLORSPACE_SRGB)(色彩空间 sRGB)
=====================================================

The :ref:`srgb` standard defines the colorspace used by most webcams
and computer graphics. The default transfer function is
``V4L2_XFER_FUNC_SRGB``. The default Y'CbCr encoding is
``V4L2_YCBCR_ENC_601``. The default Y'CbCr quantization is limited range.
(:ref:`srgb` 标准定义了大多数网络摄像头与计算机图形使用的色彩空间。默认传递函数是 ``V4L2_XFER_FUNC_SRGB`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_601`` 。默认 Y'CbCr 量化是 limited range。)

Note that the :ref:`sycc` standard specifies full range quantization,
however all current capture hardware supported by the kernel convert
R'G'B' to limited range Y'CbCr. So choosing full range as the default
would break how applications interpret the quantization range.
(注意，:ref:`sycc` 标准规定的是 full range 量化，然而内核支持的所有现有采集硬件都把 R'G'B' 转换为 limited range Y'CbCr。因此若把 full range 作为默认值，会破坏应用对量化范围的解读。)

The chromaticities of the primary colors and the white reference are:
(基色与白基准的色度坐标如下：)

.. flat-table:: sRGB Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.640
      - 0.330
    * - Green(绿)
      - 0.300
      - 0.600
    * - Blue(蓝)
      - 0.150
      - 0.060
    * - White Reference (D65)(白基准 (D65))
      - 0.3127
      - 0.3290


These chromaticities are identical to the Rec. 709 colorspace.
(这些色度坐标与 Rec. 709 色彩空间相同。)

Transfer function. Note that negative values for L are only used by the
Y'CbCr conversion.
(传递函数。注意，L 的负值只用于 Y'CbCr 转换。)

.. math::

    L' = -1.055(-L)^{\frac{1}{2.4} } + 0.055\text{, for }L < -0.0031308

    L' = 12.92L\text{, for }-0.0031308 \le L \le 0.0031308

    L' = 1.055L ^{\frac{1}{2.4} } - 0.055\text{, for }0.0031308 < L \le 1

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = -((-L' + 0.055) / 1.055) ^{2.4}\text{, for }L' < -0.04045

    L = L' / 12.92\text{, for }-0.04045 \le L' \le 0.04045

    L = ((L' + 0.055) / 1.055) ^{2.4}\text{, for }L' > 0.04045

The luminance (Y') and color difference (Cb and Cr) are obtained with
the following ``V4L2_YCBCR_ENC_601`` encoding as defined by :ref:`sycc`:
(亮度 (Y') 与色差 (Cb 和 Cr) 用 :ref:`sycc` 定义的如下 ``V4L2_YCBCR_ENC_601`` 编码获得：)

.. math::

    Y' = 0.2990R' + 0.5870G' + 0.1140B'

    Cb = -0.1687R' - 0.3313G' + 0.5B'

    Cr = 0.5R' - 0.4187G' - 0.0813B'

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the range
[-0.5…0.5]. This transform is identical to one defined in SMPTE
170M/BT.601. The Y'CbCr quantization is limited range.
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围。该变换与 SMPTE 170M/BT.601 中定义的相同。Y'CbCr 量化是 limited range。)


.. _col-oprgb:

Colorspace opRGB (V4L2_COLORSPACE_OPRGB)(色彩空间 opRGB)
========================================================

The :ref:`oprgb` standard defines the colorspace used by computer
graphics that use the opRGB colorspace. The default transfer function is
``V4L2_XFER_FUNC_OPRGB``. The default Y'CbCr encoding is
``V4L2_YCBCR_ENC_601``. The default Y'CbCr quantization is limited
range.
(:ref:`oprgb` 标准定义了使用 opRGB 色彩空间的计算机图形所用的色彩空间。默认传递函数是 ``V4L2_XFER_FUNC_OPRGB`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_601`` 。默认 Y'CbCr 量化是 limited range。)

Note that the :ref:`oprgb` standard specifies full range quantization,
however all current capture hardware supported by the kernel convert
R'G'B' to limited range Y'CbCr. So choosing full range as the default
would break how applications interpret the quantization range.
(注意，:ref:`oprgb` 标准规定的是 full range 量化，然而内核支持的所有现有采集硬件都把 R'G'B' 转换为 limited range Y'CbCr。因此若把 full range 作为默认值，会破坏应用对量化范围的解读。)

The chromaticities of the primary colors and the white reference are:
(基色与白基准的色度坐标如下：)

.. flat-table:: opRGB Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.6400
      - 0.3300
    * - Green(绿)
      - 0.2100
      - 0.7100
    * - Blue(蓝)
      - 0.1500
      - 0.0600
    * - White Reference (D65)(白基准 (D65))
      - 0.3127
      - 0.3290



Transfer function:
(传递函数：)

.. math::

    L' = L ^{\frac{1}{2.19921875}}

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = L'^{(2.19921875)}

The luminance (Y') and color difference (Cb and Cr) are obtained with
the following ``V4L2_YCBCR_ENC_601`` encoding:
(亮度 (Y') 与色差 (Cb 和 Cr) 用如下 ``V4L2_YCBCR_ENC_601`` 编码获得：)

.. math::

    Y' = 0.2990R' + 0.5870G' + 0.1140B'

    Cb = -0.1687R' - 0.3313G' + 0.5B'

    Cr = 0.5R' - 0.4187G' - 0.0813B'

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the range
[-0.5…0.5]. This transform is identical to one defined in SMPTE
170M/BT.601. The Y'CbCr quantization is limited range.
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围。该变换与 SMPTE 170M/BT.601 中定义的相同。Y'CbCr 量化是 limited range。)


.. _col-bt2020:

Colorspace BT.2020 (V4L2_COLORSPACE_BT2020)(色彩空间 BT.2020)
=============================================================

The :ref:`itu2020` standard defines the colorspace used by Ultra-high
definition television (UHDTV). The default transfer function is
``V4L2_XFER_FUNC_709``. The default Y'CbCr encoding is
``V4L2_YCBCR_ENC_BT2020``. The default Y'CbCr quantization is limited range.
The chromaticities of the primary colors and the white reference are:
(:ref:`itu2020` 标准定义了超高清电视 (UHDTV) 使用的色彩空间。默认传递函数是 ``V4L2_XFER_FUNC_709`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_BT2020`` 。默认 Y'CbCr 量化是 limited range。基色与白基准的色度坐标如下：)

.. flat-table:: BT.2020 Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.708
      - 0.292
    * - Green(绿)
      - 0.170
      - 0.797
    * - Blue(蓝)
      - 0.131
      - 0.046
    * - White Reference (D65)(白基准 (D65))
      - 0.3127
      - 0.3290



Transfer function (same as Rec. 709):
(传递函数 (与 Rec. 709 相同)：)

.. math::

    L' = 4.5L\text{, for }0 \le L < 0.018

    L' = 1.099L ^{0.45} - 0.099\text{, for } 0.018 \le L \le 1

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = L' / 4.5\text{, for } L' < 0.081

    L = \left( \frac{L' + 0.099}{1.099}\right) ^{\frac{1}{0.45} }\text{, for } L' \ge 0.081

Please note that while Rec. 709 is defined as the default transfer function
by the :ref:`itu2020` standard, in practice this colorspace is often used
with the :ref:`xf-smpte-2084`. In particular Ultra HD Blu-ray discs use
this combination.
(请注意，虽然 :ref:`itu2020` 标准把 Rec. 709 定义为默认传递函数，但实践中该色彩空间常与 :ref:`xf-smpte-2084` 搭配使用。特别是 Ultra HD Blu-ray 光盘就采用这种组合。)

The luminance (Y') and color difference (Cb and Cr) are obtained with
the following ``V4L2_YCBCR_ENC_BT2020`` encoding:
(亮度 (Y') 与色差 (Cb 和 Cr) 用如下 ``V4L2_YCBCR_ENC_BT2020`` 编码获得：)

.. math::

    Y' = 0.2627R' + 0.6780G' + 0.0593B'

    Cb = -0.1396R' - 0.3604G' + 0.5B'

    Cr = 0.5R' - 0.4598G' - 0.0402B'

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the range
[-0.5…0.5]. The Y'CbCr quantization is limited range.
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围。Y'CbCr 量化是 limited range。)

There is also an alternate constant luminance R'G'B' to Yc'CbcCrc
(``V4L2_YCBCR_ENC_BT2020_CONST_LUM``) encoding:
(还有一种备选的恒定亮度 (constant luminance) R'G'B' 到 Yc'CbcCrc 的编码 ( ``V4L2_YCBCR_ENC_BT2020_CONST_LUM`` )：)

Luma:
(亮度：)

.. math::
    :nowrap:

    \begin{align*}
    Yc' = (0.2627R + 0.6780G + 0.0593B)'& \\
    B' - Yc' \le 0:& \\
        &Cbc = (B' - Yc') / 1.9404 \\
    B' - Yc' > 0: & \\
        &Cbc = (B' - Yc') / 1.5816 \\
    R' - Yc' \le 0:& \\
        &Crc = (R' - Y') / 1.7184 \\
    R' - Yc' > 0:& \\
        &Crc = (R' - Y') / 0.9936
    \end{align*}

Yc' is clamped to the range [0…1] and Cbc and Crc are clamped to the
range [-0.5…0.5]. The Yc'CbcCrc quantization is limited range.
(Yc' 被钳位到 [0…1] 范围，Cbc 和 Crc 被钳位到 [-0.5…0.5] 范围。Yc'CbcCrc 量化是 limited range。)


.. _col-dcip3:

Colorspace DCI-P3 (V4L2_COLORSPACE_DCI_P3)(色彩空间 DCI-P3)
===========================================================

The :ref:`smpte431` standard defines the colorspace used by cinema
projectors that use the DCI-P3 colorspace. The default transfer function
is ``V4L2_XFER_FUNC_DCI_P3``. The default Y'CbCr encoding is
``V4L2_YCBCR_ENC_709``. The default Y'CbCr quantization is limited range.
(:ref:`smpte431` 标准定义了使用 DCI-P3 色彩空间的电影放映机所用的色彩空间。默认传递函数是 ``V4L2_XFER_FUNC_DCI_P3`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_709`` 。默认 Y'CbCr 量化是 limited range。)

.. note::

   Note that this colorspace standard does not specify a
   Y'CbCr encoding since it is not meant to be encoded to Y'CbCr. So this
   default Y'CbCr encoding was picked because it is the HDTV encoding.
   (注意，该色彩空间标准没有规定 Y'CbCr 编码，因为它本不打算被编码为 Y'CbCr。之所以选择这个默认 Y'CbCr 编码，只是因为它就是 HDTV 的编码。)

The chromaticities of the primary colors and the white reference are:
(基色与白基准的色度坐标如下：)


.. flat-table:: DCI-P3 Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.6800
      - 0.3200
    * - Green(绿)
      - 0.2650
      - 0.6900
    * - Blue(蓝)
      - 0.1500
      - 0.0600
    * - White Reference(白基准)
      - 0.3140
      - 0.3510



Transfer function:
(传递函数：)

.. math::

    L' = L^{\frac{1}{2.6}}

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = L'^{(2.6)}

Y'CbCr encoding is not specified. V4L2 defaults to Rec. 709.
(未规定 Y'CbCr 编码。V4L2 默认使用 Rec. 709。)


.. _col-smpte-240m:

Colorspace SMPTE 240M (V4L2_COLORSPACE_SMPTE240M)(色彩空间 SMPTE 240M)
======================================================================

The :ref:`smpte240m` standard was an interim standard used during the
early days of HDTV (1988-1998). It has been superseded by Rec. 709. The
default transfer function is ``V4L2_XFER_FUNC_SMPTE240M``. The default
Y'CbCr encoding is ``V4L2_YCBCR_ENC_SMPTE240M``. The default Y'CbCr
quantization is limited range. The chromaticities of the primary colors
and the white reference are:
(:ref:`smpte240m` 标准是 HDTV 早期 (1988-1998) 使用的一个过渡性标准。它已被 Rec. 709 取代。默认传递函数是 ``V4L2_XFER_FUNC_SMPTE240M`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_SMPTE240M`` 。默认 Y'CbCr 量化是 limited range。基色与白基准的色度坐标如下：)


.. flat-table:: SMPTE 240M Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.630
      - 0.340
    * - Green(绿)
      - 0.310
      - 0.595
    * - Blue(蓝)
      - 0.155
      - 0.070
    * - White Reference (D65)(白基准 (D65))
      - 0.3127
      - 0.3290


These chromaticities are identical to the SMPTE 170M colorspace.
(这些色度坐标与 SMPTE 170M 色彩空间相同。)

Transfer function:
(传递函数：)

.. math::

    L' = 4L\text{, for } 0 \le L < 0.0228

    L' = 1.1115L ^{0.45} - 0.1115\text{, for } 0.0228 \le L \le 1

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = \frac{L'}{4}\text{, for } 0 \le L' < 0.0913

    L = \left( \frac{L' + 0.1115}{1.1115}\right) ^{\frac{1}{0.45} }\text{, for } L' \ge 0.0913

The luminance (Y') and color difference (Cb and Cr) are obtained with
the following ``V4L2_YCBCR_ENC_SMPTE240M`` encoding:
(亮度 (Y') 与色差 (Cb 和 Cr) 用如下 ``V4L2_YCBCR_ENC_SMPTE240M`` 编码获得：)

.. math::

    Y' = 0.2122R' + 0.7013G' + 0.0865B'

    Cb = -0.1161R' - 0.3839G' + 0.5B'

    Cr = 0.5R' - 0.4451G' - 0.0549B'

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the
range [-0.5…0.5]. The Y'CbCr quantization is limited range.
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围。Y'CbCr 量化是 limited range。)


.. _col-sysm:

Colorspace NTSC 1953 (V4L2_COLORSPACE_470_SYSTEM_M)(色彩空间 NTSC 1953)
========================================================================

This standard defines the colorspace used by NTSC in 1953. In practice
this colorspace is obsolete and SMPTE 170M should be used instead. The
default transfer function is ``V4L2_XFER_FUNC_709``. The default Y'CbCr
encoding is ``V4L2_YCBCR_ENC_601``. The default Y'CbCr quantization is
limited range. The chromaticities of the primary colors and the white
reference are:
(该标准定义了 NTSC 在 1953 年使用的色彩空间。实践中该色彩空间已过时，应改用 SMPTE 170M。默认传递函数是 ``V4L2_XFER_FUNC_709`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_601`` 。默认 Y'CbCr 量化是 limited range。基色与白基准的色度坐标如下：)


.. flat-table:: NTSC 1953 Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.67
      - 0.33
    * - Green(绿)
      - 0.21
      - 0.71
    * - Blue(蓝)
      - 0.14
      - 0.08
    * - White Reference (C)(白基准 (C))
      - 0.310
      - 0.316


.. note::

   This colorspace uses Illuminant C instead of D65 as the white
   reference. To correctly convert an image in this colorspace to another
   that uses D65 you need to apply a chromatic adaptation algorithm such as
   the Bradford method.
   (该色彩空间使用标准光源 C (Illuminant C) 而非 D65 作为白基准。要把该色彩空间的图像正确转换为使用 D65 的其他色彩空间，需要应用色适应 (chromatic adaptation) 算法，例如 Bradford 方法。)

The transfer function was never properly defined for NTSC 1953. The Rec.
709 transfer function is recommended in the literature:
(NTSC 1953 的传递函数从未被正式定义。文献中推荐使用 Rec. 709 传递函数：)

.. math::

    L' = 4.5L\text{, for } 0 \le L < 0.018

    L' = 1.099L ^{0.45} - 0.099\text{, for } 0.018 \le L \le 1

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = \frac{L'}{4.5} \text{, for } L' < 0.081

    L = \left( \frac{L' + 0.099}{1.099}\right) ^{\frac{1}{0.45} }\text{, for } L' \ge 0.081

The luminance (Y') and color difference (Cb and Cr) are obtained with
the following ``V4L2_YCBCR_ENC_601`` encoding:
(亮度 (Y') 与色差 (Cb 和 Cr) 用如下 ``V4L2_YCBCR_ENC_601`` 编码获得：)

.. math::

    Y' = 0.2990R' + 0.5870G' + 0.1140B'

    Cb = -0.1687R' - 0.3313G' + 0.5B'

    Cr = 0.5R' - 0.4187G' - 0.0813B'

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the range
[-0.5…0.5]. The Y'CbCr quantization is limited range. This transform is
identical to one defined in SMPTE 170M/BT.601.
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围。Y'CbCr 量化是 limited range。该变换与 SMPTE 170M/BT.601 中定义的相同。)


.. _col-sysbg:

Colorspace EBU Tech. 3213 (V4L2_COLORSPACE_470_SYSTEM_BG)(色彩空间 EBU Tech. 3213)
===================================================================================

The :ref:`tech3213` standard defines the colorspace used by PAL/SECAM
in 1975. Note that this colorspace is not supported by the HDMI interface.
Instead :ref:`tech3321` recommends that Rec. 709 is used instead for HDMI.
The default transfer function is
``V4L2_XFER_FUNC_709``. The default Y'CbCr encoding is
``V4L2_YCBCR_ENC_601``. The default Y'CbCr quantization is limited
range. The chromaticities of the primary colors and the white reference
are:
(:ref:`tech3213` 标准定义了 PAL/SECAM 在 1975 年使用的色彩空间。注意，HDMI 接口不支持该色彩空间。:ref:`tech3321` 反而建议在 HDMI 上改用 Rec. 709。默认传递函数是 ``V4L2_XFER_FUNC_709`` 。默认 Y'CbCr 编码是 ``V4L2_YCBCR_ENC_601`` 。默认 Y'CbCr 量化是 limited range。基色与白基准的色度坐标如下：)


.. flat-table:: EBU Tech. 3213 Chromaticities
    :header-rows:  1
    :stub-columns: 0
    :widths:       1 1 2

    * - Color(颜色)
      - x
      - y
    * - Red(红)
      - 0.64
      - 0.33
    * - Green(绿)
      - 0.29
      - 0.60
    * - Blue(蓝)
      - 0.15
      - 0.06
    * - White Reference (D65)(白基准 (D65))
      - 0.3127
      - 0.3290



The transfer function was never properly defined for this colorspace.
The Rec. 709 transfer function is recommended in the literature:
(该色彩空间的传递函数从未被正式定义。文献中推荐使用 Rec. 709 传递函数：)

.. math::

    L' = 4.5L\text{, for } 0 \le L < 0.018

    L' = 1.099L ^{0.45} - 0.099\text{, for } 0.018 \le L \le 1

Inverse Transfer function:
(逆传递函数：)

.. math::

    L = \frac{L'}{4.5} \text{, for } L' < 0.081

    L = \left(\frac{L' + 0.099}{1.099} \right) ^{\frac{1}{0.45} }\text{, for } L' \ge 0.081

The luminance (Y') and color difference (Cb and Cr) are obtained with
the following ``V4L2_YCBCR_ENC_601`` encoding:
(亮度 (Y') 与色差 (Cb 和 Cr) 用如下 ``V4L2_YCBCR_ENC_601`` 编码获得：)

.. math::

    Y' = 0.2990R' + 0.5870G' + 0.1140B'

    Cb = -0.1687R' - 0.3313G' + 0.5B'

    Cr = 0.5R' - 0.4187G' - 0.0813B'

Y' is clamped to the range [0…1] and Cb and Cr are clamped to the range
[-0.5…0.5]. The Y'CbCr quantization is limited range. This transform is
identical to one defined in SMPTE 170M/BT.601.
(Y' 被钳位到 [0…1] 范围，Cb 和 Cr 被钳位到 [-0.5…0.5] 范围。Y'CbCr 量化是 limited range。该变换与 SMPTE 170M/BT.601 中定义的相同。)


.. _col-jpeg:

Colorspace JPEG (V4L2_COLORSPACE_JPEG)(色彩空间 JPEG)
=====================================================

This colorspace defines the colorspace used by most (Motion-)JPEG
formats. The chromaticities of the primary colors and the white
reference are identical to sRGB. The transfer function use is
``V4L2_XFER_FUNC_SRGB``. The Y'CbCr encoding is ``V4L2_YCBCR_ENC_601``
with full range quantization where Y' is scaled to [0…255] and Cb/Cr are
scaled to [-128…128] and then clipped to [-128…127].
(该色彩空间定义了大多数 (Motion-)JPEG 格式使用的色彩空间。基色与白基准的色度坐标与 sRGB 相同。使用的传递函数是 ``V4L2_XFER_FUNC_SRGB`` 。Y'CbCr 编码是 ``V4L2_YCBCR_ENC_601`` 加 full range 量化，其中 Y' 缩放到 [0…255]，Cb/Cr 缩放到 [-128…128]，然后裁剪到 [-128…127]。)

.. note::

   The JPEG standard does not actually store colorspace
   information. So if something other than sRGB is used, then the driver
   will have to set that information explicitly. Effectively
   ``V4L2_COLORSPACE_JPEG`` can be considered to be an abbreviation for
   ``V4L2_COLORSPACE_SRGB``, ``V4L2_XFER_FUNC_SRGB``, ``V4L2_YCBCR_ENC_601``
   and ``V4L2_QUANTIZATION_FULL_RANGE``.
   (JPEG 标准实际上并不存储色彩空间信息。因此如果使用的不是 sRGB，驱动就必须显式设置该信息。实际上， ``V4L2_COLORSPACE_JPEG`` 可以视为 ``V4L2_COLORSPACE_SRGB`` 、 ``V4L2_XFER_FUNC_SRGB`` 、 ``V4L2_YCBCR_ENC_601`` 和 ``V4L2_QUANTIZATION_FULL_RANGE`` 的缩写。)

*********************************************************
Detailed Transfer Function Descriptions(传递函数详细说明)
*********************************************************

.. _xf-smpte-2084:

Transfer Function SMPTE 2084 (V4L2_XFER_FUNC_SMPTE2084)(传递函数 SMPTE 2084)
============================================================================

The :ref:`smpte2084` standard defines the transfer function used by
High Dynamic Range content.
(:ref:`smpte2084` 标准定义了高动态范围 (High Dynamic Range) 内容使用的传递函数。)

Constants:(常数：)
    m1 = (2610 / 4096) / 4

    m2 = (2523 / 4096) * 128

    c1 = 3424 / 4096

    c2 = (2413 / 4096) * 32

    c3 = (2392 / 4096) * 32

Transfer function:(传递函数：)
    L' = ((c1 + c2 * L\ :sup:`m1`) / (1 + c3 * L\ :sup:`m1`))\ :sup:`m2`

Inverse Transfer function:(逆传递函数：)
    L = (max(L':sup:`1/m2` - c1, 0) / (c2 - c3 *
    L'\ :sup:`1/m2`))\ :sup:`1/m1`

Take care when converting between this transfer function and non-HDR transfer
functions: the linear RGB values [0…1] of HDR content map to a luminance range
of 0 to 10000 cd/m\ :sup:`2` whereas the linear RGB values of non-HDR (aka
Standard Dynamic Range or SDR) map to a luminance range of 0 to 100 cd/m\ :sup:`2`.
(在这个传递函数与非 HDR 传递函数之间转换时要当心：HDR 内容的线性 RGB 值 [0…1] 映射到 0 至 10000 cd/m\ :sup:`2` 的亮度范围，而非 HDR (即 Standard Dynamic Range，标准动态范围，SDR) 的线性 RGB 值则映射到 0 至 100 cd/m\ :sup:`2` 的亮度范围。)

To go from SDR to HDR you will have to divide L by 100 first. To go in the other
direction you will have to multiply L by 100. Of course, this clamps all
luminance values over 100 cd/m\ :sup:`2` to 100 cd/m\ :sup:`2`.
(从 SDR 转到 HDR 需要先把 L 除以 100；往另一个方向则需要把 L 乘以 100。当然，这会把所有超过 100 cd/m\ :sup:`2` 的亮度值钳位到 100 cd/m\ :sup:`2` 。)

There are better methods, see e.g. :ref:`colimg` for more in-depth information
about this.
(还有更好的方法，例如参见 :ref:`colimg` 获取这方面的更深入的信息。)
