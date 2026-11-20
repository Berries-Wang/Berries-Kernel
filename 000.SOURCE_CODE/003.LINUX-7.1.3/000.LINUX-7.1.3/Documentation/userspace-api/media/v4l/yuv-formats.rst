.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _yuv-formats:

*********************
YUV Formats(YUV 格式)
*********************

YUV is the format native to TV broadcast and composite video signals. It
separates the brightness information (Y) from the color information (U
and V or Cb and Cr). The color information consists of red and blue
*color difference* signals, this way the green component can be
reconstructed by subtracting from the brightness component. See
:ref:`colorspaces` for conversion examples. YUV was chosen because
early television would only transmit brightness information. To add
color in a way compatible with existing receivers a new signal carrier
was added to transmit the color difference signals.
(YUV 是电视广播与复合视频信号的原生格式。它把亮度信息 (Y) 与颜色信息 (U 和 V
或 Cb 和 Cr) 分离开来。颜色信息由红色与蓝色的*色差* (color difference) 信号
构成，这样对亮度分量做减法即可重建绿色分量。转换示例见 colorspaces 一节。
之所以选用 YUV，是因为早期的电视只能传输亮度信息；为了以与既有接收机兼容的
方式加入颜色，人们增加了一个新的信号载波来传输色差信号。)

===================
Subsampling(子采样)
===================

YUV formats commonly encode images with a lower resolution for the chroma
components than for the luma component. This compression technique, taking
advantage of the human eye being more sensitive to luminance than color
differences, is called chroma subsampling.
(YUV 格式为色度 (chroma) 分量编码的图像分辨率通常低于亮度 (luma) 分量。这种
利用人眼对亮度比色度差异更敏感的特性来压缩数据的技术，称为色度子采样
(chroma subsampling)。)

While many combinations of subsampling factors in the horizontal and vertical
direction are possible, common factors are 1 (no subsampling), 2 and 4, with
horizontal subsampling always larger than or equal to vertical subsampling.
Common combinations are named as follows.
(水平与垂直方向上的子采样因子可以有多种组合，常用的因子是 1(不子采样)、2 和
4，且水平子采样总是大于或等于垂直子采样。常见的组合命名如下。)

- `4:4:4`: No subsampling
  (`4:4:4`：不做子采样。)
- `4:2:2`: Horizontal subsampling by 2, no vertical subsampling
  (`4:2:2`：水平方向按 2 子采样，垂直方向不子采样。)
- `4:2:0`: Horizontal subsampling by 2, vertical subsampling by 2
  (`4:2:0`：水平方向按 2 子采样，垂直方向按 2 子采样。)
- `4:1:1`: Horizontal subsampling by 4, no vertical subsampling
  (`4:1:1`：水平方向按 4 子采样，垂直方向不子采样。)
- `4:1:0`: Horizontal subsampling by 4, vertical subsampling by 4
  (`4:1:0`：水平方向按 4 子采样，垂直方向按 4 子采样。)

Subsampling the chroma component effectively creates chroma values that can be
located in different spatial locations:
(对色度分量做子采样，实际上会生成可能位于不同空间位置上的色度值：)

- .. _yuv-chroma-centered:

  The subsampled chroma value may be calculated by simply averaging the chroma
  value of two consecutive pixels. It effectively models the chroma of a pixel
  sited between the two original pixels. This is referred to as centered or
  interstitially sited chroma.
  (子采样后的色度值可以简单地通过对相邻两个像素的色度值取平均得到。它实际建模
  的是位于这两个原始像素之间位置的像素的色度。这称为居中色度 (centered
  chroma) 或间隙色度 (interstitially sited chroma)。)

- .. _yuv-chroma-cosited:

  The other option is to subsample chroma values in a way that place them in
  the same spatial sites as the pixels. This may be performed by skipping every
  other chroma sample (creating aliasing artifacts), or with filters using an
  odd number of taps. This is referred to as co-sited chroma.
  (另一种做法是让子采样后的色度值与像素处于相同的空间位置。这可以通过隔一个
  丢弃一个色度采样(会产生混叠伪影)来实现，也可以用奇数抽头的滤波器来实现。这
  称为共址色度 (co-sited chroma)。)

The following examples show different combination of chroma siting in a 4x4
image.
(下面的例子展示了 4x4 图像中色度位置安排的各种组合。)

.. flat-table:: 4:2:2 subsampling, interstitially sited
    :header-rows: 1
    :stub-columns: 1

    * -
      - 0
      -
      - 1
      -
      - 2
      -
      - 3
    * - 0
      - Y
      - C
      - Y
      -
      - Y
      - C
      - Y
    * - 1
      - Y
      - C
      - Y
      -
      - Y
      - C
      - Y
    * - 2
      - Y
      - C
      - Y
      -
      - Y
      - C
      - Y
    * - 3
      - Y
      - C
      - Y
      -
      - Y
      - C
      - Y

.. flat-table:: 4:2:2 subsampling, co-sited
    :header-rows: 1
    :stub-columns: 1

    * -
      - 0
      -
      - 1
      -
      - 2
      -
      - 3
    * - 0
      - Y/C
      -
      - Y
      -
      - Y/C
      -
      - Y
    * - 1
      - Y/C
      -
      - Y
      -
      - Y/C
      -
      - Y
    * - 2
      - Y/C
      -
      - Y
      -
      - Y/C
      -
      - Y
    * - 3
      - Y/C
      -
      - Y
      -
      - Y/C
      -
      - Y

.. flat-table:: 4:2:0 subsampling, horizontally interstitially sited, vertically co-sited
    :header-rows: 1
    :stub-columns: 1

    * -
      - 0
      -
      - 1
      -
      - 2
      -
      - 3
    * - 0
      - Y
      - C
      - Y
      -
      - Y
      - C
      - Y
    * - 1
      - Y
      -
      - Y
      -
      - Y
      -
      - Y
    * - 2
      - Y
      - C
      - Y
      -
      - Y
      - C
      - Y
    * - 3
      - Y
      -
      - Y
      -
      - Y
      -
      - Y

.. flat-table:: 4:1:0 subsampling, horizontally and vertically interstitially sited
    :header-rows: 1
    :stub-columns: 1

    * -
      - 0
      -
      - 1
      -
      - 2
      -
      - 3
    * - 0
      - Y
      -
      - Y
      -
      - Y
      -
      - Y
    * -
      -
      -
      -
      -
      -
      -
      -
    * - 1
      - Y
      -
      - Y
      -
      - Y
      -
      - Y
    * -
      -
      -
      -
      - C
      -
      -
      -
    * - 2
      - Y
      -
      - Y
      -
      - Y
      -
      - Y
    * -
      -
      -
      -
      -
      -
      -
      -
    * - 3
      - Y
      -
      - Y
      -
      - Y
      -
      - Y


.. toctree::
    :maxdepth: 1

    pixfmt-packed-yuv
    pixfmt-yuv-planar
    pixfmt-yuv-luma
    pixfmt-y8i
    pixfmt-y12i
    pixfmt-y16i
    pixfmt-uv8
    pixfmt-m420
