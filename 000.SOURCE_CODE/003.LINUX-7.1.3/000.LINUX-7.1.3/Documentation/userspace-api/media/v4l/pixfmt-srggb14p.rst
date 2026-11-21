.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-SRGGB14P:
.. _v4l2-pix-fmt-sbggr14p:
.. _v4l2-pix-fmt-sgbrg14p:
.. _v4l2-pix-fmt-sgrbg14p:

*****************************************************************************************************************************************************
V4L2_PIX_FMT_SRGGB14P ('pREE'), V4L2_PIX_FMT_SGRBG14P ('pgEE'), V4L2_PIX_FMT_SGBRG14P ('pGEE'), V4L2_PIX_FMT_SBGGR14P ('pBEE'),(14 位打包 Bayer 格式)
*****************************************************************************************************************************************************

*man V4L2_PIX_FMT_SRGGB14P(2)*

V4L2_PIX_FMT_SGRBG14P
V4L2_PIX_FMT_SGBRG14P
V4L2_PIX_FMT_SBGGR14P
14-bit packed Bayer formats
(14 位打包 Bayer 格式)


Description(描述)
=================

These four pixel formats are packed raw sRGB / Bayer formats with 14
bits per colour. Every four consecutive samples are packed into seven
bytes. Each of the first four bytes contain the eight high order bits
of the pixels, and the three following bytes contains the six least
significant bits of each pixel, in the same order.
(这四种像素格式是每种颜色 14 位的打包 raw sRGB/Bayer 格式。每四个连续的
样本被打包进七个字节。前四个字节中的每一个包含像素的高 8 位，随后的三个
字节包含每个像素的 6 个最低有效位，顺序相同。)

Each n-pixel row contains n/2 green samples and n/2 blue or red samples,
with alternating green-red and green-blue rows. They are conventionally
described as GRGR... BGBG..., RGRG... GBGB..., etc. Below is an example
of one of these formats:
(每个 n 像素的行包含 n/2 个绿色样本和 n/2 个蓝色或红色样本，绿-红行与
绿-蓝行交替出现。它们通常被描述为 GRGR... BGBG...、RGRG... GBGB... 等。
下面是其中一种格式的示例：)

**Byte Order.**
Each cell is one byte.
(字节序。每个单元格是一个字节。)

.. raw:: latex

    \begingroup
    \footnotesize
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{1.6cm}|p{1.0cm}|p{1.0cm}|p{1.0cm}|p{1.0cm}|p{3.5cm}|p{3.5cm}|p{3.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       2 1 1 1 1 3 3 3


    -  .. row 1

       -  start + 0

       -  B\ :sub:`00high`

       -  G\ :sub:`01high`

       -  B\ :sub:`02high`

       -  G\ :sub:`03high`

       -  G\ :sub:`01low bits 1--0`\ (bits 7--6)

	  B\ :sub:`00low bits 5--0`\ (bits 5--0)

       -  B\ :sub:`02low bits 3--0`\ (bits 7--4)

	  G\ :sub:`01low bits 5--2`\ (bits 3--0)

       -  G\ :sub:`03low bits 5--0`\ (bits 7--2)

	  B\ :sub:`02low bits 5--4`\ (bits 1--0)

    -  .. row 2

       -  start + 7

       -  G\ :sub:`10high`

       -  R\ :sub:`11high`

       -  G\ :sub:`12high`

       -  R\ :sub:`13high`

       -  R\ :sub:`11low bits 1--0`\ (bits 7--6)

	  G\ :sub:`10low bits 5--0`\ (bits 5--0)

       -  G\ :sub:`12low bits 3--0`\ (bits 7--4)

	  R\ :sub:`11low bits 5--2`\ (bits 3--0)

       -  R\ :sub:`13low bits 5--0`\ (bits 7--2)

	  G\ :sub:`12low bits 5--4`\ (bits 1--0)

    -  .. row 3

       -  start + 14

       -  B\ :sub:`20high`

       -  G\ :sub:`21high`

       -  B\ :sub:`22high`

       -  G\ :sub:`23high`

       -  G\ :sub:`21low bits 1--0`\ (bits 7--6)

	  B\ :sub:`20low bits 5--0`\ (bits 5--0)

       -  B\ :sub:`22low bits 3--0`\ (bits 7--4)

	  G\ :sub:`21low bits 5--2`\ (bits 3--0)

       -  G\ :sub:`23low bits 5--0`\ (bits 7--2)

	  B\ :sub:`22low bits 5--4`\ (bits 1--0)

    -  .. row 4

       -  start + 21

       -  G\ :sub:`30high`

       -  R\ :sub:`31high`

       -  G\ :sub:`32high`

       -  R\ :sub:`33high`

       -  R\ :sub:`31low bits 1--0`\ (bits 7--6)
	  G\ :sub:`30low bits 5--0`\ (bits 5--0)

       -  G\ :sub:`32low bits 3--0`\ (bits 7--4)
	  R\ :sub:`31low bits 5--2`\ (bits 3--0)

       -  R\ :sub:`33low bits 5--0`\ (bits 7--2)
	  G\ :sub:`32low bits 5--4`\ (bits 1--0)

.. raw:: latex

    \endgroup

