.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-SRGGB10P:
.. _v4l2-pix-fmt-sbggr10p:
.. _v4l2-pix-fmt-sgbrg10p:
.. _v4l2-pix-fmt-sgrbg10p:

*******************************************************************************************************************************
V4L2_PIX_FMT_SRGGB10P ('pRAA'), V4L2_PIX_FMT_SGRBG10P ('pgAA'), V4L2_PIX_FMT_SGBRG10P ('pGAA'), V4L2_PIX_FMT_SBGGR10P ('pBAA'),
*******************************************************************************************************************************


V4L2_PIX_FMT_SGRBG10P
V4L2_PIX_FMT_SGBRG10P
V4L2_PIX_FMT_SBGGR10P
10-bit packed Bayer formats
(V4L2_PIX_FMT_SGRBG10P、V4L2_PIX_FMT_SGBRG10P、V4L2_PIX_FMT_SBGGR10P
——10 位打包 Bayer 格式。)


Description(描述)
=================

These four pixel formats are packed raw sRGB / Bayer formats with 10
bits per sample. Every four consecutive samples are packed into 5
bytes. Each of the first 4 bytes contain the 8 high order bits
of the pixels, and the 5th byte contains the 2 least significants
bits of each pixel, in the same order.
(这四种像素格式是每样本 10 位的打包 (packed) raw sRGB / Bayer 格式。
每四个连续样本打包进 5 个字节。前 4 个字节各包含像素的高 8 个有效
位，第 5 个字节按相同顺序包含每个像素的 2 个最低有效位。)

Each n-pixel row contains n/2 green samples and n/2 blue or red samples,
with alternating green-red and green-blue rows. They are conventionally
described as GRGR... BGBG..., RGRG... GBGB..., etc. Below is an example
of a small V4L2_PIX_FMT_SBGGR10P image:
(每个 n 像素行包含 n/2 个绿色样本和 n/2 个蓝色或红色样本，绿-红与
绿-蓝行交替。它们通常被描述为 GRGR... BGBG...、RGRG... GBGB... 等。
下面是一个小型 V4L2_PIX_FMT_SBGGR10P 图像的示例：)

**Byte Order.**
Each cell is one byte.
( **字节序。** 每个单元是一个字节。)

.. tabularcolumns:: |p{2.4cm}|p{1.4cm}|p{1.2cm}|p{1.2cm}|p{1.2cm}|p{9.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 12 8 8 8 8 68

    * - start + 0:
      - B\ :sub:`00high`
      - G\ :sub:`01high`
      - B\ :sub:`02high`
      - G\ :sub:`03high`
      - G\ :sub:`03low`\ (bits 7--6) B\ :sub:`02low`\ (bits 5--4)

	G\ :sub:`01low`\ (bits 3--2) B\ :sub:`00low`\ (bits 1--0)
    * - start + 5:
      - G\ :sub:`10high`
      - R\ :sub:`11high`
      - G\ :sub:`12high`
      - R\ :sub:`13high`
      - R\ :sub:`13low`\ (bits 7--6) G\ :sub:`12low`\ (bits 5--4)

	R\ :sub:`11low`\ (bits 3--2) G\ :sub:`10low`\ (bits 1--0)
    * - start + 10:
      - B\ :sub:`20high`
      - G\ :sub:`21high`
      - B\ :sub:`22high`
      - G\ :sub:`23high`
      - G\ :sub:`23low`\ (bits 7--6) B\ :sub:`22low`\ (bits 5--4)

	G\ :sub:`21low`\ (bits 3--2) B\ :sub:`20low`\ (bits 1--0)
    * - start + 15:
      - G\ :sub:`30high`
      - R\ :sub:`31high`
      - G\ :sub:`32high`
      - R\ :sub:`33high`
      - R\ :sub:`33low`\ (bits 7--6) G\ :sub:`32low`\ (bits 5--4)

	R\ :sub:`31low`\ (bits 3--2) G\ :sub:`30low`\ (bits 1--0)
