.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-M420:

**************************
V4L2_PIX_FMT_M420 ('M420')
**************************

Format with ½ horizontal and vertical chroma resolution, also known as
YUV 4:2:0. Hybrid plane line-interleaved layout.
(水平和垂直色度分辨率减半 (½) 的格式，也称为 YUV 4:2:0。混合平面行交
织布局。)


Description(描述)
=================

M420 is a YUV format with ½ horizontal and vertical chroma subsampling
(YUV 4:2:0). Pixels are organized as interleaved luma and chroma planes.
Two lines of luma data are followed by one line of chroma data.
(M420 是水平和垂直色度都按 ½ 下采样的 YUV 格式 (YUV 4:2:0)。像素被组
织成交织的亮度平面与色度平面。两行亮度数据之后跟一行色度数据。)

The luma plane has one byte per pixel. The chroma plane contains
interleaved CbCr pixels subsampled by ½ in the horizontal and vertical
directions. Each CbCr pair belongs to four pixels. For example,
Cb\ :sub:`0`/Cr\ :sub:`0` belongs to Y'\ :sub:`00`, Y'\ :sub:`01`,
Y'\ :sub:`10`, Y'\ :sub:`11`.
(亮度平面每像素一个字节。色度平面包含在水平和垂直方向上按 ½ 下采样
的交织 CbCr 像素。每个 CbCr 对属于四个像素。例如，Cb\ :sub:`0`/Cr\ :sub:`0`
属于 Y'\ :sub:`00` 、Y'\ :sub:`01` 、Y'\ :sub:`10` 、Y'\ :sub:`11` 。)

All line lengths are identical: if the Y lines include pad bytes so do
the CbCr lines.
(所有行的长度都相同：如果 Y 行包含填充字节，那么 CbCr 行也包含。)

**Byte Order.**
Each cell is one byte.
( **字节序。** 每个单元是一个字节。)


.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start + 4:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start + 8:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
    * - start + 16:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 20:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 24:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`


**Color Sample Location:**
Chroma samples are :ref:`interstitially sited<yuv-chroma-centered>`
horizontally and vertically.
( **颜色样本位置：** 色度样本在水平和垂直方向上均为
:ref:`interstitially sited<yuv-chroma-centered>` (居于样本之间)。)
