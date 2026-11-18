.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. planar-yuv:

*********************************
Planar YUV formats(平面 YUV 格式)
*********************************

Planar formats split luma and chroma data in separate memory regions. They
exist in two variants:
(平面格式将亮度 (luma) 和色度 (chroma) 数据存放在相互独立的内存区域中。它有两种变体：)

- Semi-planar formats use two planes. The first plane is the luma plane and
  stores the Y components. The second plane is the chroma plane and stores the
  Cb and Cr components interleaved.
  (半平面格式使用两个平面。第一个平面是亮度平面，存储 Y 分量；第二个平面是色度平面，以交错方式存储 Cb 和 Cr 分量。)

- Fully planar formats use three planes to store the Y, Cb and Cr components
  separately.
  (全平面格式使用三个平面分别存储 Y、Cb 和 Cr 分量。)

Within a plane, components are stored in pixel order, which may be linear or
tiled. Padding may be supported at the end of the lines, and the line stride of
the chroma planes may be constrained by the line stride of the luma plane.
(在一个平面内，分量按像素顺序存储，可以是线性或平铺 (tiled) 的。行尾可以支持填充 (padding)，色度平面的行步长 (line stride) 可能受亮度平面行步长的约束。)

Some planar formats allow planes to be placed in independent memory locations.
They are identified by an 'M' suffix in their name (such as in
``V4L2_PIX_FMT_NV12M``). Those formats are intended to be used only in drivers
and applications that support the multi-planar API, described in
:ref:`planar-apis`. Unless explicitly documented as supporting non-contiguous
planes, formats require the planes to follow each other immediately in memory.
(某些平面格式允许把各平面放在相互独立的内存位置上。这类格式的名称带有 'M' 后缀（例如 ``V4L2_PIX_FMT_NV12M`` ）。这些格式仅供支持多平面 API 的驱动程序和应用程序使用，多平面 API 在 :ref:`planar-apis` 中描述。除非明确记载支持非连续平面，否则格式要求各平面在内存中紧邻存放。)


Semi-Planar YUV Formats(半平面 YUV 格式)
========================================

These formats are commonly referred to as NV formats (NV12, NV16, ...). They
use two planes, and store the luma components in the first plane and the chroma
components in the second plane. The Cb and Cr components are interleaved in the
chroma plane, with Cb and Cr always stored in pairs. The chroma order is
exposed as different formats.
(这些格式通常称为 NV 格式（NV12、NV16 等）。它们使用两个平面，把亮度分量存储在第一个平面，把色度分量存储在第二个平面。Cb 和 Cr 分量在色度平面中交错存储，且始终成对存放。色度顺序的不同组合以不同的格式公开。)

For memory contiguous formats, the number of padding pixels at the end of the
chroma lines is identical to the padding of the luma lines. Without horizontal
subsampling, the chroma line stride (in bytes) is thus equal to twice the luma
line stride. With horizontal subsampling by 2, the chroma line stride is equal
to the luma line stride. Vertical subsampling doesn't affect the line stride.
(对于内存连续的格式，色度行末尾的填充像素数与亮度行的填充相同。因此，在没有水平子采样时，色度行步长（以字节计）等于亮度行步长的两倍；水平子采样为 2 时，色度行步长等于亮度行步长。垂直子采样不影响行步长。)

For non-contiguous formats, no constraints are enforced by the format on the
relationship between the luma and chroma line padding and stride.
(对于非连续格式，格式本身对亮度行与色度行的填充和步长之间的关系不做任何约束。)

All components are stored with the same number of bits per component.
(所有分量都以相同的每分量位数存储。)

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{5.2cm}|p{1.0cm}|p{1.5cm}|p{1.9cm}|p{1.2cm}|p{1.8cm}|p{2.7cm}|

.. flat-table:: Overview of Semi-Planar YUV Formats
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - Bits per component(每分量位数)
      - Subsampling(子采样)
      - Chroma order [1]_(色度顺序)
      - Contiguous [2]_(是否连续)
      - Tiling [3]_(平铺方式)
    * - V4L2_PIX_FMT_NV12
      - 'NV12'
      - 8
      - 4:2:0
      - Cb, Cr
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV21
      - 'NV21'
      - 8
      - 4:2:0
      - Cr, Cb
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV12M
      - 'NM12'
      - 8
      - 4:2:0
      - Cb, Cr
      - No(否)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV21M
      - 'NM21'
      - 8
      - 4:2:0
      - Cr, Cb
      - No(否)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV12MT
      - 'TM12'
      - 8
      - 4:2:0
      - Cb, Cr
      - No(否)
      - 64x32 tiles

        Horizontal Z order(水平 Z 顺序)
    * - V4L2_PIX_FMT_NV12MT_16X16
      - 'VM12'
      - 8
      - 4:2:2
      - Cb, Cr
      - No(否)
      - 16x16 tiles(16x16 平铺)
    * - V4L2_PIX_FMT_P010
      - 'P010'
      - 10
      - 4:2:0
      - Cb, Cr
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_P010_4L4
      - 'T010'
      - 10
      - 4:2:0
      - Cb, Cr
      - Yes(是)
      - 4x4 tiles(4x4 平铺)
    * - V4L2_PIX_FMT_P012
      - 'P012'
      - 12
      - 4:2:0
      - Cb, Cr
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_P012M
      - 'PM12'
      - 12
      - 4:2:0
      - Cb, Cr
      - No(否)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV15
      - 'NV15'
      - 10
      - 4:2:0
      - Cb, Cr
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV15_4L4
      - 'VT15'
      - 15
      - 4:2:0
      - Cb, Cr
      - Yes(是)
      - 4x4 tiles(4x4 平铺)
    * - V4L2_PIX_FMT_MT2110T
      - 'MT2T'
      - 15
      - 4:2:0
      - Cb, Cr
      - No(否)
      - 16x32 / 16x16 tiles tiled low bits(16x32 / 16x16 平铺，低位平铺)
    * - V4L2_PIX_FMT_MT2110R
      - 'MT2R'
      - 15
      - 4:2:0
      - Cb, Cr
      - No(否)
      - 16x32 / 16x16 tiles raster low bits(16x32 / 16x16 平铺，低位光栅)
    * - V4L2_PIX_FMT_NV16
      - 'NV16'
      - 8
      - 4:2:2
      - Cb, Cr
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV61
      - 'NV61'
      - 8
      - 4:2:2
      - Cr, Cb
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV16M
      - 'NM16'
      - 8
      - 4:2:2
      - Cb, Cr
      - No(否)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV61M
      - 'NM61'
      - 8
      - 4:2:2
      - Cr, Cb
      - No(否)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV20
      - 'NV20'
      - 10
      - 4:2:2
      - Cb, Cr
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV24
      - 'NV24'
      - 8
      - 4:4:4
      - Cb, Cr
      - Yes(是)
      - Linear(线性)
    * - V4L2_PIX_FMT_NV42
      - 'NV42'
      - 8
      - 4:4:4
      - Cr, Cb
      - Yes(是)
      - Linear(线性)

.. raw:: latex

    \normalsize

.. [1] Order of chroma samples in the second plane
       (第二平面中色度样本的顺序。)
.. [2] Indicates if planes have to be contiguous in memory or can be
       disjoint
       (指示各平面在内存中必须连续还是可以分离。)
.. [3] Macroblock size in pixels(以像素为单位的宏块大小。)


**Color Sample Location:**
Chroma samples are :ref:`interstitially sited<yuv-chroma-centered>`
horizontally.
(色度样本在水平方向上按 :ref:`interstitially sited<yuv-chroma-centered>` 所述以间隙方式放置。)


.. _V4L2-PIX-FMT-NV12:
.. _V4L2-PIX-FMT-NV21:
.. _V4L2-PIX-FMT-NV12M:
.. _V4L2-PIX-FMT-NV21M:
.. _V4L2-PIX-FMT-P010:

NV12, NV21, NV12M and NV21M(半平面 4:2:0 格式)
----------------------------------------------

Semi-planar YUV 4:2:0 formats. The chroma plane is subsampled by 2 in each
direction. Chroma lines contain half the number of pixels and the same number
of bytes as luma lines, and the chroma plane contains half the number of lines
of the luma plane.
(半平面 YUV 4:2:0 格式。色度平面在每个方向上都做 2 倍子采样。色度行包含亮度行一半的像素数、相同的字节数，色度平面包含亮度平面一半的行数。)

.. flat-table:: Sample 4x4 NV12 Image
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
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 16:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
    * - start + 20:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`

.. flat-table:: Sample 4x4 NV12M Image
    :header-rows:  0
    :stub-columns: 0

    * - start0 + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start0 + 4:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start0 + 8:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start0 + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * -
    * - start1 + 0:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
    * - start1 + 4:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`


.. _V4L2-PIX-FMT-NV15:

NV15(10 位半平面格式)
---------------------

Semi-planar 10-bit YUV 4:2:0 format similar to NV12, using 10-bit components
with no padding between each component. A group of 4 components are stored over
5 bytes in little endian order.
(与 NV12 类似的半平面 10 位 YUV 4:2:0 格式，使用 10 位分量，分量之间没有填充。每 4 个分量以小端序存储在 5 个字节中。)

.. flat-table:: Sample 4x4 NV15 Image (1 byte per cell)
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - Y'\ :sub:`00[7:0]`
      - Y'\ :sub:`01[5:0]`\ Y'\ :sub:`00[9:8]`
      - Y'\ :sub:`02[3:0]`\ Y'\ :sub:`01[9:6]`
      - Y'\ :sub:`03[1:0]`\ Y'\ :sub:`02[9:4]`
      - Y'\ :sub:`03[9:2]`
    * - start + 5:
      - Y'\ :sub:`10[7:0]`
      - Y'\ :sub:`11[5:0]`\ Y'\ :sub:`10[9:8]`
      - Y'\ :sub:`12[3:0]`\ Y'\ :sub:`11[9:6]`
      - Y'\ :sub:`13[1:0]`\ Y'\ :sub:`12[9:4]`
      - Y'\ :sub:`13[9:2]`
    * - start + 10:
      - Y'\ :sub:`20[7:0]`
      - Y'\ :sub:`21[5:0]`\ Y'\ :sub:`20[9:8]`
      - Y'\ :sub:`22[3:0]`\ Y'\ :sub:`21[9:6]`
      - Y'\ :sub:`23[1:0]`\ Y'\ :sub:`22[9:4]`
      - Y'\ :sub:`23[9:2]`
    * - start + 15:
      - Y'\ :sub:`30[7:0]`
      - Y'\ :sub:`31[5:0]`\ Y'\ :sub:`30[9:8]`
      - Y'\ :sub:`32[3:0]`\ Y'\ :sub:`31[9:6]`
      - Y'\ :sub:`33[1:0]`\ Y'\ :sub:`32[9:4]`
      - Y'\ :sub:`33[9:2]`
    * - start + 20:
      - Cb\ :sub:`00[7:0]`
      - Cr\ :sub:`00[5:0]`\ Cb\ :sub:`00[9:8]`
      - Cb\ :sub:`01[3:0]`\ Cr\ :sub:`00[9:6]`
      - Cr\ :sub:`01[1:0]`\ Cb\ :sub:`01[9:4]`
      - Cr\ :sub:`01[9:2]`
    * - start + 25:
      - Cb\ :sub:`10[7:0]`
      - Cr\ :sub:`10[5:0]`\ Cb\ :sub:`10[9:8]`
      - Cb\ :sub:`11[3:0]`\ Cr\ :sub:`10[9:6]`
      - Cr\ :sub:`11[1:0]`\ Cb\ :sub:`11[9:4]`
      - Cr\ :sub:`11[9:2]`


.. _V4L2-PIX-FMT-NV12MT:
.. _V4L2-PIX-FMT-NV12MT-16X16:
.. _V4L2-PIX-FMT-NV12-4L4:
.. _V4L2-PIX-FMT-NV12-16L16:
.. _V4L2-PIX-FMT-NV12-32L32:
.. _V4L2-PIX-FMT-NV12M-8L128:
.. _V4L2-PIX-FMT-NV12-8L128:
.. _V4L2-PIX-FMT-MM21:

Tiled NV12(平铺 NV12)
---------------------

Semi-planar YUV 4:2:0 formats, using macroblock tiling. The chroma plane is
subsampled by 2 in each direction. Chroma lines contain half the number of
pixels and the same number of bytes as luma lines, and the chroma plane
contains half the number of lines of the luma plane. Each tile follows the
previous one linearly in memory (from left to right, top to bottom).
(使用宏块平铺 (macroblock tiling) 的半平面 YUV 4:2:0 格式。色度平面在每个方向上都做 2 倍子采样。色度行包含亮度行一半的像素数、相同的字节数，色度平面包含亮度平面一半的行数。每个平铺块 (tile) 在内存中紧随前一个块线性排列（从左到右、从上到下）。)

``V4L2_PIX_FMT_NV12MT_16X16`` is similar to ``V4L2_PIX_FMT_NV12M`` but stores
pixels in 2D 16x16 tiles, and stores tiles linearly in memory.
The line stride and image height must be aligned to a multiple of 16.
The layouts of the luma and chroma planes are identical.
(``V4L2_PIX_FMT_NV12MT_16X16`` 与 ``V4L2_PIX_FMT_NV12M`` 类似，但以 2D 16x16 平铺块存储像素，并将平铺块线性存入内存。行步长和图像高度必须按 16 的倍数对齐。亮度平面和色度平面的布局相同。)

``V4L2_PIX_FMT_NV12MT`` is similar to ``V4L2_PIX_FMT_NV12M`` but stores
pixels in 2D 64x32 tiles, and stores 2x2 groups of tiles in
Z-order in memory, alternating Z and mirrored Z shapes horizontally.
The line stride must be a multiple of 128 pixels to ensure an
integer number of Z shapes. The image height must be a multiple of 32 pixels.
If the vertical resolution is an odd number of tiles, the last row of
tiles is stored in linear order. The layouts of the luma and chroma
planes are identical.
(``V4L2_PIX_FMT_NV12MT`` 与 ``V4L2_PIX_FMT_NV12M`` 类似，但以 2D 64x32 平铺块存储像素，并将 2x2 的平铺块组以 Z 顺序存入内存，Z 形与镜像 Z 形在水平方向上交替出现。行步长必须是 128 像素的倍数，以保证整数个 Z 形。图像高度必须是 32 像素的倍数。若垂直分辨率为奇数个平铺块，则最后一行平铺块按线性顺序存储。亮度平面和色度平面的布局相同。)

.. _nv12mt:

.. kernel-figure:: nv12mt.svg
    :alt:    nv12mt.svg
    :align:  center

    V4L2_PIX_FMT_NV12MT macroblock Z shape memory layout

.. _nv12mt_ex:

.. kernel-figure:: nv12mt_example.svg
    :alt:    nv12mt_example.svg
    :align:  center

    Example V4L2_PIX_FMT_NV12MT memory layout of tiles

``V4L2_PIX_FMT_NV12_4L4`` stores pixels in 4x4 tiles, and stores
tiles linearly in memory. The line stride and image height must be
aligned to a multiple of 4. The layouts of the luma and chroma planes are
identical.
(``V4L2_PIX_FMT_NV12_4L4`` 以 4x4 平铺块存储像素，并将平铺块线性存入内存。行步长和图像高度必须按 4 的倍数对齐。亮度平面和色度平面的布局相同。)

``V4L2_PIX_FMT_NV12_16L16`` stores pixels in 16x16 tiles, and stores
tiles linearly in memory. The line stride and image height must be
aligned to a multiple of 16. The layouts of the luma and chroma planes are
identical.
(``V4L2_PIX_FMT_NV12_16L16`` 以 16x16 平铺块存储像素，并将平铺块线性存入内存。行步长和图像高度必须按 16 的倍数对齐。亮度平面和色度平面的布局相同。)

``V4L2_PIX_FMT_NV12_32L32`` stores pixels in 32x32 tiles, and stores
tiles linearly in memory. The line stride and image height must be
aligned to a multiple of 32. The layouts of the luma and chroma planes are
identical.
(``V4L2_PIX_FMT_NV12_32L32`` 以 32x32 平铺块存储像素，并将平铺块线性存入内存。行步长和图像高度必须按 32 的倍数对齐。亮度平面和色度平面的布局相同。)

``V4L2_PIX_FMT_NV12M_8L128`` is similar to ``V4L2_PIX_FMT_NV12M`` but stores
pixels in 2D 8x128 tiles, and stores tiles linearly in memory.
The image height must be aligned to a multiple of 128.
The layouts of the luma and chroma planes are identical.
(``V4L2_PIX_FMT_NV12M_8L128`` 与 ``V4L2_PIX_FMT_NV12M`` 类似，但以 2D 8x128 平铺块存储像素，并将平铺块线性存入内存。图像高度必须按 128 的倍数对齐。亮度平面和色度平面的布局相同。)

``V4L2_PIX_FMT_NV12_8L128`` is similar to ``V4L2_PIX_FMT_NV12M_8L128`` but stores
two planes in one memory.
(``V4L2_PIX_FMT_NV12_8L128`` 与 ``V4L2_PIX_FMT_NV12M_8L128`` 类似，但把两个平面存放在同一块内存中。)

``V4L2_PIX_FMT_MM21`` store luma pixel in 16x32 tiles, and chroma pixels
in 16x16 tiles. The line stride must be aligned to a multiple of 16 and the
image height must be aligned to a multiple of 32. The number of luma and chroma
tiles are identical, even though the tile size differ. The image is formed of
two non-contiguous planes.
(``V4L2_PIX_FMT_MM21`` 以 16x32 平铺块存储亮度像素，以 16x16 平铺块存储色度像素。行步长必须按 16 的倍数对齐，图像高度必须按 32 的倍数对齐。尽管平铺块大小不同，亮度与色度平铺块的数量相同。该图像由两个非连续的平面构成。)


.. _V4L2-PIX-FMT-NV15-4L4:
.. _V4L2-PIX-FMT-NV12M-10BE-8L128:
.. _V4L2-PIX-FMT-NV12-10BE-8L128:
.. _V4L2-PIX-FMT-MT2110T:
.. _V4L2-PIX-FMT-MT2110R:

Tiled NV15(平铺 NV15)
---------------------

``V4L2_PIX_FMT_NV15_4L4`` Semi-planar 10-bit YUV 4:2:0 formats, using 4x4 tiling.
All components are packed without any padding between each other.
As a side-effect, each group of 4 components are stored over 5 bytes
(YYYY or UVUV = 4 * 10 bits = 40 bits = 5 bytes).
(``V4L2_PIX_FMT_NV15_4L4`` 是使用 4x4 平铺的半平面 10 位 YUV 4:2:0 格式。所有分量紧密打包，彼此之间没有任何填充。其副作用是每 4 个分量存储在 5 个字节中（YYYY 或 UVUV = 4 * 10 位 = 40 位 = 5 字节）。)

``V4L2_PIX_FMT_NV12M_10BE_8L128`` is similar to ``V4L2_PIX_FMT_NV12M`` but stores
10 bits pixels in 2D 8x128 tiles, and stores tiles linearly in memory.
the data is arranged in big endian order.
The image height must be aligned to a multiple of 128.
The layouts of the luma and chroma planes are identical.
Note the tile size is 8bytes multiplied by 128 bytes,
it means that the low bits and high bits of one pixel may be in different tiles.
The 10 bit pixels are packed, so 5 bytes contain 4 10-bit pixels layout like
this (for luma):
byte 0: Y0(bits 9-2)
byte 1: Y0(bits 1-0) Y1(bits 9-4)
byte 2: Y1(bits 3-0) Y2(bits 9-6)
byte 3: Y2(bits 5-0) Y3(bits 9-8)
byte 4: Y3(bits 7-0)
(``V4L2_PIX_FMT_NV12M_10BE_8L128`` 与 ``V4L2_PIX_FMT_NV12M`` 类似，但以 2D 8x128 平铺块存储 10 位像素，并将平铺块线性存入内存。数据按大端序排列。图像高度必须按 128 的倍数对齐。亮度平面和色度平面的布局相同。注意平铺块大小为 8 字节乘以 128 字节，这意味着一个像素的低位和高位可能位于不同的平铺块中。10 位像素是打包存储的，因此 5 个字节包含 4 个 10 位像素，其布局如下（以亮度为例）：字节 0：Y0（位 9-2）；字节 1：Y0（位 1-0）Y1（位 9-4）；字节 2：Y1（位 3-0）Y2（位 9-6）；字节 3：Y2（位 5-0）Y3（位 9-8）；字节 4：Y3（位 7-0）。)

``V4L2_PIX_FMT_NV12_10BE_8L128`` is similar to ``V4L2_PIX_FMT_NV12M_10BE_8L128`` but stores
two planes in one memory.
(``V4L2_PIX_FMT_NV12_10BE_8L128`` 与 ``V4L2_PIX_FMT_NV12M_10BE_8L128`` 类似，但把两个平面存放在同一块内存中。)

``V4L2_PIX_FMT_MT2110T`` is one of Mediatek packed 10bit YUV 4:2:0 formats.
It is fully packed 10bit 4:2:0 format like NV15 (15 bits per pixel), except
that the lower two bits data is stored in separate partitions. The format is
composed of 16x32 luma tiles, and 16x16 chroma tiles. Each tiles is 640 bytes
long, divided into 8 partitions of 80 bytes.  The first 16 bytes of the
partition represent the 2 least significant bits of pixel data. The remaining
64 bytes represent the 8 most significant bits of pixel data.
(``V4L2_PIX_FMT_MT2110T`` 是 Mediatek 打包 10 位 YUV 4:2:0 格式之一。它是与 NV15 类似的完全打包的 10 位 4:2:0 格式（每像素 15 位），但最低两位的数据存储在独立的分区中。该格式由 16x32 亮度平铺块和 16x16 色度平铺块构成。每个平铺块长 640 字节，分为 8 个 80 字节的分区。分区的前 16 字节表示像素数据的最低 2 位，其余 64 字节表示像素数据的最高 8 位。)

.. kernel-figure:: mt2110t.svg
    :alt:    mt2110t.svg
    :align:  center

    Layout of MT2110T Chroma Tile

Filtering out the upper part of each partitions results in a valid
``V4L2_PIX_FMT_MM21`` frame. A partition is a sub-tile of size 16 x 4. The
lower two bits is said to be tiled since each bytes contains the lower two
bits of the column of for pixel matching the same index. The chroma tiles
only have 4 partitions.
(过滤掉每个分区的上部，就得到一帧有效的 ``V4L2_PIX_FMT_MM21`` 。分区是大小为 16 x 4 的子平铺块。最低两位之所以称为"平铺"，是因为每个字节包含同一列上 4 个与之索引对应的像素的最低两位。色度平铺块只有 4 个分区。)

.. flat-table:: MT2110T LSB bits layout
    :header-rows:  1
    :stub-columns: 1

    * -
      - start + 0:
      - start + 1:
      - . . .
      - start\ +\ 15:
    * - Bits 1:0
      - Y'\ :sub:`0:0`
      - Y'\ :sub:`0:1`
      - . . .
      - Y'\ :sub:`0:15`
    * - Bit 3:2
      - Y'\ :sub:`1:0`
      - Y'\ :sub:`1:1`
      - . . .
      - Y'\ :sub:`1:15`
    * - Bits 5:4
      - Y'\ :sub:`2:0`
      - Y'\ :sub:`2:1`
      - . . .
      - Y'\ :sub:`2:15`
    * - Bits 7:6
      - Y'\ :sub:`3:0`
      - Y'\ :sub:`3:1`
      - . . .
      - Y'\ :sub:`3:15`

``V4L2_PIX_FMT_MT2110R`` is identical to ``V4L2_PIX_FMT_MT2110T`` except that
the least significant two bits layout is in raster order. This means the first byte
contains 4 pixels of the first row, with 4 bytes per line.
(``V4L2_PIX_FMT_MT2110R`` 与 ``V4L2_PIX_FMT_MT2110T`` 相同，只是最低两位的布局按光栅顺序排列。这意味着第一个字节包含第一行的 4 个像素，每行 4 字节。)

.. flat-table:: MT2110R LSB bits layout
    :header-rows:  2
    :stub-columns: 1

    * -
      - :cspan:`3` Byte 0
      - ...
      - :cspan:`3` Byte 3
    * -
      - 7:6
      - 5:4
      - 3:2
      - 1:0
      - ...
      - 7:6
      - 5:4
      - 3:2
      - 1:0
    * - start + 0:
      - Y'\ :sub:`0:3`
      - Y'\ :sub:`0:2`
      - Y'\ :sub:`0:1`
      - Y'\ :sub:`0:0`
      - ...
      - Y'\ :sub:`0:15`
      - Y'\ :sub:`0:14`
      - Y'\ :sub:`0:13`
      - Y'\ :sub:`0:12`
    * - start + 4:
      - Y'\ :sub:`1:3`
      - Y'\ :sub:`1:2`
      - Y'\ :sub:`1:1`
      - Y'\ :sub:`1:0`
      - ...
      - Y'\ :sub:`1:15`
      - Y'\ :sub:`1:14`
      - Y'\ :sub:`1:13`
      - Y'\ :sub:`1:12`
    * - start + 8:
      - Y'\ :sub:`2:3`
      - Y'\ :sub:`2:2`
      - Y'\ :sub:`2:1`
      - Y'\ :sub:`2:0`
      - ...
      - Y'\ :sub:`2:15`
      - Y'\ :sub:`2:14`
      - Y'\ :sub:`2:13`
      - Y'\ :sub:`2:12`
    * - start\ +\ 12:
      - Y'\ :sub:`3:3`
      - Y'\ :sub:`3:2`
      - Y'\ :sub:`3:1`
      - Y'\ :sub:`3:0`
      - ...
      - Y'\ :sub:`3:15`
      - Y'\ :sub:`3:14`
      - Y'\ :sub:`3:13`
      - Y'\ :sub:`3:12`


.. _V4L2-PIX-FMT-NV16:
.. _V4L2-PIX-FMT-NV61:
.. _V4L2-PIX-FMT-NV16M:
.. _V4L2-PIX-FMT-NV61M:

NV16, NV61, NV16M and NV61M(半平面 4:2:2 格式)
----------------------------------------------

Semi-planar YUV 4:2:2 formats. The chroma plane is subsampled by 2 in the
horizontal direction. Chroma lines contain half the number of pixels and the
same number of bytes as luma lines, and the chroma plane contains the same
number of lines as the luma plane.
(半平面 YUV 4:2:2 格式。色度平面在水平方向上做 2 倍子采样。色度行包含亮度行一半的像素数、相同的字节数，色度平面包含与亮度平面相同的行数。)

.. flat-table:: Sample 4x4 NV16 Image
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
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 16:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
    * - start + 20:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`
    * - start + 24:
      - Cb\ :sub:`20`
      - Cr\ :sub:`20`
      - Cb\ :sub:`21`
      - Cr\ :sub:`21`
    * - start + 28:
      - Cb\ :sub:`30`
      - Cr\ :sub:`30`
      - Cb\ :sub:`31`
      - Cr\ :sub:`31`

.. flat-table:: Sample 4x4 NV16M Image
    :header-rows:  0
    :stub-columns: 0

    * - start0 + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start0 + 4:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start0 + 8:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start0 + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * -
    * - start1 + 0:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`02`
      - Cr\ :sub:`02`
    * - start1 + 4:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`12`
      - Cr\ :sub:`12`
    * - start1 + 8:
      - Cb\ :sub:`20`
      - Cr\ :sub:`20`
      - Cb\ :sub:`22`
      - Cr\ :sub:`22`
    * - start1 + 12:
      - Cb\ :sub:`30`
      - Cr\ :sub:`30`
      - Cb\ :sub:`32`
      - Cr\ :sub:`32`


.. _V4L2-PIX-FMT-NV20:

NV20(10 位半平面格式)
---------------------

Semi-planar 10-bit YUV 4:2:2 format similar to NV16, using 10-bit components
with no padding between each component. A group of 4 components are stored over
5 bytes in little endian order.
(与 NV16 类似的半平面 10 位 YUV 4:2:2 格式，使用 10 位分量，分量之间没有填充。每 4 个分量以小端序存储在 5 个字节中。)

.. flat-table:: Sample 4x4 NV20 Image (1 byte per cell)
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - Y'\ :sub:`00[7:0]`
      - Y'\ :sub:`01[5:0]`\ Y'\ :sub:`00[9:8]`
      - Y'\ :sub:`02[3:0]`\ Y'\ :sub:`01[9:6]`
      - Y'\ :sub:`03[1:0]`\ Y'\ :sub:`02[9:4]`
      - Y'\ :sub:`03[9:2]`
    * - start + 5:
      - Y'\ :sub:`10[7:0]`
      - Y'\ :sub:`11[5:0]`\ Y'\ :sub:`10[9:8]`
      - Y'\ :sub:`12[3:0]`\ Y'\ :sub:`11[9:6]`
      - Y'\ :sub:`13[1:0]`\ Y'\ :sub:`12[9:4]`
      - Y'\ :sub:`13[9:2]`
    * - start + 10:
      - Y'\ :sub:`20[7:0]`
      - Y'\ :sub:`21[5:0]`\ Y'\ :sub:`20[9:8]`
      - Y'\ :sub:`22[3:0]`\ Y'\ :sub:`21[9:6]`
      - Y'\ :sub:`23[1:0]`\ Y'\ :sub:`22[9:4]`
      - Y'\ :sub:`23[9:2]`
    * - start + 15:
      - Y'\ :sub:`30[7:0]`
      - Y'\ :sub:`31[5:0]`\ Y'\ :sub:`30[9:8]`
      - Y'\ :sub:`32[3:0]`\ Y'\ :sub:`31[9:6]`
      - Y'\ :sub:`33[1:0]`\ Y'\ :sub:`32[9:4]`
      - Y'\ :sub:`33[9:2]`
    * - start + 20:
      - Cb\ :sub:`00[7:0]`
      - Cr\ :sub:`00[5:0]`\ Cb\ :sub:`00[9:8]`
      - Cb\ :sub:`01[3:0]`\ Cr\ :sub:`00[9:6]`
      - Cr\ :sub:`01[1:0]`\ Cb\ :sub:`01[9:4]`
      - Cr\ :sub:`01[9:2]`
    * - start + 25:
      - Cb\ :sub:`10[7:0]`
      - Cr\ :sub:`10[5:0]`\ Cb\ :sub:`10[9:8]`
      - Cb\ :sub:`11[3:0]`\ Cr\ :sub:`10[9:6]`
      - Cr\ :sub:`11[1:0]`\ Cb\ :sub:`11[9:4]`
      - Cr\ :sub:`11[9:2]`
    * - start + 30:
      - Cb\ :sub:`20[7:0]`
      - Cr\ :sub:`20[5:0]`\ Cb\ :sub:`20[9:8]`
      - Cb\ :sub:`21[3:0]`\ Cr\ :sub:`20[9:6]`
      - Cr\ :sub:`21[1:0]`\ Cb\ :sub:`21[9:4]`
      - Cr\ :sub:`21[9:2]`
    * - start + 35:
      - Cb\ :sub:`30[7:0]`
      - Cr\ :sub:`30[5:0]`\ Cb\ :sub:`30[9:8]`
      - Cb\ :sub:`31[3:0]`\ Cr\ :sub:`30[9:6]`
      - Cr\ :sub:`31[1:0]`\ Cb\ :sub:`31[9:4]`
      - Cr\ :sub:`31[9:2]`


.. _V4L2-PIX-FMT-NV24:
.. _V4L2-PIX-FMT-NV42:

NV24 and NV42(半平面 4:4:4 格式)
--------------------------------

Semi-planar YUV 4:4:4 formats. The chroma plane is not subsampled.
Chroma lines contain the same number of pixels and twice the
number of bytes as luma lines, and the chroma plane contains the same
number of lines as the luma plane.
(半平面 YUV 4:4:4 格式。色度平面不做子采样。色度行包含与亮度行相同的像素数、两倍的字节数，色度平面包含与亮度平面相同的行数。)

.. flat-table:: Sample 4x4 NV24 Image
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
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 16:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
      - Cb\ :sub:`02`
      - Cr\ :sub:`02`
      - Cb\ :sub:`03`
      - Cr\ :sub:`03`
    * - start + 24:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`
      - Cb\ :sub:`12`
      - Cr\ :sub:`12`
      - Cb\ :sub:`13`
      - Cr\ :sub:`13`
    * - start + 32:
      - Cb\ :sub:`20`
      - Cr\ :sub:`20`
      - Cb\ :sub:`21`
      - Cr\ :sub:`21`
      - Cb\ :sub:`22`
      - Cr\ :sub:`22`
      - Cb\ :sub:`23`
      - Cr\ :sub:`23`
    * - start + 40:
      - Cb\ :sub:`30`
      - Cr\ :sub:`30`
      - Cb\ :sub:`31`
      - Cr\ :sub:`31`
      - Cb\ :sub:`32`
      - Cr\ :sub:`32`
      - Cb\ :sub:`33`
      - Cr\ :sub:`33`

.. _V4L2_PIX_FMT_P010:
.. _V4L2-PIX-FMT-P010-4L4:

P010 and tiled P010(P010 与平铺 P010)
-------------------------------------

P010 is like NV12 with 10 bits per component, expanded to 16 bits.
Data in the 10 high bits, zeros in the 6 low bits, arranged in little endian order.
(P010 类似于 NV12，但每分量 10 位，扩展为 16 位。数据位于高 10 位，低 6 位为零，按小端序排列。)

.. flat-table:: Sample 4x4 P010 Image
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start + 8:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start + 16:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 24:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 32:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
    * - start + 40:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`

.. _V4L2-PIX-FMT-P012:
.. _V4L2-PIX-FMT-P012M:

P012 and P012M(P012 与 P012M)
-----------------------------

P012 is like NV12 with 12 bits per component, expanded to 16 bits.
Data in the 12 high bits, zeros in the 4 low bits, arranged in little endian order.
(P012 类似于 NV12，但每分量 12 位，扩展为 16 位。数据位于高 12 位，低 4 位为零，按小端序排列。)

.. flat-table:: Sample 4x4 P012 Image
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start + 8:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start + 16:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 24:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 32:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
    * - start + 40:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`

.. flat-table:: Sample 4x4 P012M Image
    :header-rows:  0
    :stub-columns: 0

    * - start0 + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start0 + 8:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start0 + 16:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start0 + 24:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * -
    * - start1 + 0:
      - Cb\ :sub:`00`
      - Cr\ :sub:`00`
      - Cb\ :sub:`01`
      - Cr\ :sub:`01`
    * - start1 + 8:
      - Cb\ :sub:`10`
      - Cr\ :sub:`10`
      - Cb\ :sub:`11`
      - Cr\ :sub:`11`


Fully Planar YUV Formats(全平面 YUV 格式)
=========================================

These formats store the Y, Cb and Cr components in three separate planes. The
luma plane comes first, and the order of the two chroma planes varies between
formats. The two chroma planes always use the same subsampling.
(这些格式把 Y、Cb 和 Cr 分量存储在三个相互独立的平面中。亮度平面在最前，两个色度平面的先后顺序因格式而异。两个色度平面始终使用相同的子采样。)

For memory contiguous formats, the number of padding pixels at the end of the
chroma lines is identical to the padding of the luma lines. The chroma line
stride (in bytes) is thus equal to the luma line stride divided by the
horizontal subsampling factor. Vertical subsampling doesn't affect the line
stride.
(对于内存连续的格式，色度行末尾的填充像素数与亮度行的填充相同。因此色度行步长（以字节计）等于亮度行步长除以水平子采样因子。垂直子采样不影响行步长。)

For non-contiguous formats, no constraints are enforced by the format on the
relationship between the luma and chroma line padding and stride.
(对于非连续格式，格式本身对亮度行与色度行的填充和步长之间的关系不做任何约束。)

All components are stored with the same number of bits per component.
(所有分量都以相同的每分量位数存储。)

``V4L2_PIX_FMT_P010_4L4`` stores pixels in 4x4 tiles, and stores tiles linearly
in memory. The line stride must be aligned to multiple of 8 and image height to
a multiple of 4. The layouts of the luma and chroma planes are identical.
(``V4L2_PIX_FMT_P010_4L4`` 以 4x4 平铺块存储像素，并将平铺块线性存入内存。行步长必须按 8 的倍数对齐，图像高度按 4 的倍数对齐。亮度平面和色度平面的布局相同。)

.. raw:: latex

    \small

.. tabularcolumns:: |p{5.0cm}|p{1.1cm}|p{1.5cm}|p{2.2cm}|p{1.2cm}|p{3.7cm}|

.. flat-table:: Overview of Fully Planar YUV Formats
    :header-rows:  1
    :stub-columns: 0

    * - Identifier(标识符)
      - Code(代码)
      - Bits per component(每分量位数)
      - Subsampling(子采样)
      - Planes order [4]_(平面顺序)
      - Contiguous [5]_(是否连续)

    * - V4L2_PIX_FMT_YUV410
      - 'YUV9'
      - 8
      - 4:1:0
      - Y, Cb, Cr
      - Yes(是)
    * - V4L2_PIX_FMT_YVU410
      - 'YVU9'
      - 8
      - 4:1:0
      - Y, Cr, Cb
      - Yes(是)
    * - V4L2_PIX_FMT_YUV411P
      - '411P'
      - 8
      - 4:1:1
      - Y, Cb, Cr
      - Yes(是)
    * - V4L2_PIX_FMT_YUV420M
      - 'YM12'
      - 8
      - 4:2:0
      - Y, Cb, Cr
      - No(否)
    * - V4L2_PIX_FMT_YVU420M
      - 'YM21'
      - 8
      - 4:2:0
      - Y, Cr, Cb
      - No(否)
    * - V4L2_PIX_FMT_YUV420
      - 'YU12'
      - 8
      - 4:2:0
      - Y, Cb, Cr
      - Yes(是)
    * - V4L2_PIX_FMT_YVU420
      - 'YV12'
      - 8
      - 4:2:0
      - Y, Cr, Cb
      - Yes(是)
    * - V4L2_PIX_FMT_YUV422P
      - '422P'
      - 8
      - 4:2:2
      - Y, Cb, Cr
      - Yes(是)
    * - V4L2_PIX_FMT_YUV422M
      - 'YM16'
      - 8
      - 4:2:2
      - Y, Cb, Cr
      - No(否)
    * - V4L2_PIX_FMT_YVU422M
      - 'YM61'
      - 8
      - 4:2:2
      - Y, Cr, Cb
      - No(否)
    * - V4L2_PIX_FMT_YUV444M
      - 'YM24'
      - 8
      - 4:4:4
      - Y, Cb, Cr
      - No(否)
    * - V4L2_PIX_FMT_YVU444M
      - 'YM42'
      - 8
      - 4:4:4
      - Y, Cr, Cb
      - No(否)

.. raw:: latex

    \normalsize

.. [4] Order of luma and chroma planes
       (亮度平面与色度平面的顺序。)
.. [5] Indicates if planes have to be contiguous in memory or can be
       disjoint
       (指示各平面在内存中必须连续还是可以分离。)


**Color Sample Location:**
Chroma samples are :ref:`interstitially sited<yuv-chroma-centered>`
horizontally.
(色度样本在水平方向上按 :ref:`interstitially sited<yuv-chroma-centered>` 所述以间隙方式放置。)

.. _V4L2-PIX-FMT-YUV410:
.. _V4L2-PIX-FMT-YVU410:

YUV410 and YVU410(平面 4:1:0 格式)
----------------------------------

Planar YUV 4:1:0 formats. The chroma planes are subsampled by 4 in each
direction. Chroma lines contain a quarter of the number of pixels and bytes of
the luma lines, and the chroma planes contain a quarter of the number of lines
of the luma plane.
(平面 YUV 4:1:0 格式。色度平面在每个方向上都做 4 倍子采样。色度行包含亮度行四分之一的像素数和字节数，色度平面包含亮度平面四分之一的行数。)

.. flat-table:: Sample 4x4 YUV410 Image
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
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 16:
      - Cr\ :sub:`00`
    * - start + 17:
      - Cb\ :sub:`00`


.. _V4L2-PIX-FMT-YUV411P:

YUV411P(平面 4:1:1 格式)
------------------------

Planar YUV 4:1:1 formats. The chroma planes are subsampled by 4 in the
horizontal direction. Chroma lines contain a quarter of the number of pixels
and bytes of the luma lines, and the chroma planes contain the same number of
lines as the luma plane.
(平面 YUV 4:1:1 格式。色度平面在水平方向上做 4 倍子采样。色度行包含亮度行四分之一的像素数和字节数，色度平面包含与亮度平面相同的行数。)

.. flat-table:: Sample 4x4 YUV411P Image
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
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 16:
      - Cb\ :sub:`00`
    * - start + 17:
      - Cb\ :sub:`10`
    * - start + 18:
      - Cb\ :sub:`20`
    * - start + 19:
      - Cb\ :sub:`30`
    * - start + 20:
      - Cr\ :sub:`00`
    * - start + 21:
      - Cr\ :sub:`10`
    * - start + 22:
      - Cr\ :sub:`20`
    * - start + 23:
      - Cr\ :sub:`30`


.. _V4L2-PIX-FMT-YUV420:
.. _V4L2-PIX-FMT-YVU420:
.. _V4L2-PIX-FMT-YUV420M:
.. _V4L2-PIX-FMT-YVU420M:

YUV420, YVU420, YUV420M and YVU420M(平面 4:2:0 格式)
----------------------------------------------------

Planar YUV 4:2:0 formats. The chroma planes are subsampled by 2 in each
direction. Chroma lines contain half of the number of pixels and bytes of the
luma lines, and the chroma planes contain half of the number of lines of the
luma plane.
(平面 YUV 4:2:0 格式。色度平面在每个方向上都做 2 倍子采样。色度行包含亮度行一半的像素数和字节数，色度平面包含亮度平面一半的行数。)

.. flat-table:: Sample 4x4 YUV420 Image
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
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 16:
      - Cr\ :sub:`00`
      - Cr\ :sub:`01`
    * - start + 18:
      - Cr\ :sub:`10`
      - Cr\ :sub:`11`
    * - start + 20:
      - Cb\ :sub:`00`
      - Cb\ :sub:`01`
    * - start + 22:
      - Cb\ :sub:`10`
      - Cb\ :sub:`11`

.. flat-table:: Sample 4x4 YUV420M Image
    :header-rows:  0
    :stub-columns: 0

    * - start0 + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start0 + 4:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start0 + 8:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start0 + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * -
    * - start1 + 0:
      - Cb\ :sub:`00`
      - Cb\ :sub:`01`
    * - start1 + 2:
      - Cb\ :sub:`10`
      - Cb\ :sub:`11`
    * -
    * - start2 + 0:
      - Cr\ :sub:`00`
      - Cr\ :sub:`01`
    * - start2 + 2:
      - Cr\ :sub:`10`
      - Cr\ :sub:`11`


.. _V4L2-PIX-FMT-YUV422P:
.. _V4L2-PIX-FMT-YUV422M:
.. _V4L2-PIX-FMT-YVU422M:

YUV422P, YUV422M and YVU422M(平面 4:2:2 格式)
---------------------------------------------

Planar YUV 4:2:2 formats. The chroma planes are subsampled by 2 in the
horizontal direction. Chroma lines contain half of the number of pixels and
bytes of the luma lines, and the chroma planes contain the same number of lines
as the luma plane.
(平面 YUV 4:2:2 格式。色度平面在水平方向上做 2 倍子采样。色度行包含亮度行一半的像素数和字节数，色度平面包含与亮度平面相同的行数。)

.. flat-table:: Sample 4x4 YUV422P Image
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
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * - start + 16:
      - Cb\ :sub:`00`
      - Cb\ :sub:`01`
    * - start + 18:
      - Cb\ :sub:`10`
      - Cb\ :sub:`11`
    * - start + 20:
      - Cb\ :sub:`20`
      - Cb\ :sub:`21`
    * - start + 22:
      - Cb\ :sub:`30`
      - Cb\ :sub:`31`
    * - start + 24:
      - Cr\ :sub:`00`
      - Cr\ :sub:`01`
    * - start + 26:
      - Cr\ :sub:`10`
      - Cr\ :sub:`11`
    * - start + 28:
      - Cr\ :sub:`20`
      - Cr\ :sub:`21`
    * - start + 30:
      - Cr\ :sub:`30`
      - Cr\ :sub:`31`

.. flat-table:: Sample 4x4 YUV422M Image
    :header-rows:  0
    :stub-columns: 0

    * - start0 + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start0 + 4:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start0 + 8:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start0 + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * -
    * - start1 + 0:
      - Cb\ :sub:`00`
      - Cb\ :sub:`01`
    * - start1 + 2:
      - Cb\ :sub:`10`
      - Cb\ :sub:`11`
    * - start1 + 4:
      - Cb\ :sub:`20`
      - Cb\ :sub:`21`
    * - start1 + 6:
      - Cb\ :sub:`30`
      - Cb\ :sub:`31`
    * -
    * - start2 + 0:
      - Cr\ :sub:`00`
      - Cr\ :sub:`01`
    * - start2 + 2:
      - Cr\ :sub:`10`
      - Cr\ :sub:`11`
    * - start2 + 4:
      - Cr\ :sub:`20`
      - Cr\ :sub:`21`
    * - start2 + 6:
      - Cr\ :sub:`30`
      - Cr\ :sub:`31`


.. _V4L2-PIX-FMT-YUV444M:
.. _V4L2-PIX-FMT-YVU444M:

YUV444M and YVU444M(平面 4:4:4 格式)
------------------------------------

Planar YUV 4:4:4 formats. The chroma planes are no subsampled. Chroma lines
contain the same number of pixels and bytes of the luma lines, and the chroma
planes contain the same number of lines as the luma plane.
(平面 YUV 4:4:4 格式。色度平面不做子采样。色度行包含与亮度行相同的像素数和字节数，色度平面包含与亮度平面相同的行数。)

.. flat-table:: Sample 4x4 YUV444M Image
    :header-rows:  0
    :stub-columns: 0

    * - start0 + 0:
      - Y'\ :sub:`00`
      - Y'\ :sub:`01`
      - Y'\ :sub:`02`
      - Y'\ :sub:`03`
    * - start0 + 4:
      - Y'\ :sub:`10`
      - Y'\ :sub:`11`
      - Y'\ :sub:`12`
      - Y'\ :sub:`13`
    * - start0 + 8:
      - Y'\ :sub:`20`
      - Y'\ :sub:`21`
      - Y'\ :sub:`22`
      - Y'\ :sub:`23`
    * - start0 + 12:
      - Y'\ :sub:`30`
      - Y'\ :sub:`31`
      - Y'\ :sub:`32`
      - Y'\ :sub:`33`
    * -
    * - start1 + 0:
      - Cb\ :sub:`00`
      - Cb\ :sub:`01`
      - Cb\ :sub:`02`
      - Cb\ :sub:`03`
    * - start1 + 4:
      - Cb\ :sub:`10`
      - Cb\ :sub:`11`
      - Cb\ :sub:`12`
      - Cb\ :sub:`13`
    * - start1 + 8:
      - Cb\ :sub:`20`
      - Cb\ :sub:`21`
      - Cb\ :sub:`22`
      - Cb\ :sub:`23`
    * - start1 + 12:
      - Cb\ :sub:`20`
      - Cb\ :sub:`21`
      - Cb\ :sub:`32`
      - Cb\ :sub:`33`
    * -
    * - start2 + 0:
      - Cr\ :sub:`00`
      - Cr\ :sub:`01`
      - Cr\ :sub:`02`
      - Cr\ :sub:`03`
    * - start2 + 4:
      - Cr\ :sub:`10`
      - Cr\ :sub:`11`
      - Cr\ :sub:`12`
      - Cr\ :sub:`13`
    * - start2 + 8:
      - Cr\ :sub:`20`
      - Cr\ :sub:`21`
      - Cr\ :sub:`22`
      - Cr\ :sub:`23`
    * - start2 + 12:
      - Cr\ :sub:`30`
      - Cr\ :sub:`31`
      - Cr\ :sub:`32`
      - Cr\ :sub:`33`
