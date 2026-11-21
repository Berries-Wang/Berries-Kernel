.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-SRGGB12P:
.. _v4l2-pix-fmt-sbggr12p:
.. _v4l2-pix-fmt-sgbrg12p:
.. _v4l2-pix-fmt-sgrbg12p:

****************************************************************************************************************************************************
V4L2_PIX_FMT_SRGGB12P ('pRCC'), V4L2_PIX_FMT_SGRBG12P ('pgCC'), V4L2_PIX_FMT_SGBRG12P ('pGCC'), V4L2_PIX_FMT_SBGGR12P ('pBCC')(12 位打包 Bayer 格式)
****************************************************************************************************************************************************


12-bit packed Bayer formats(12 位打包 Bayer 格式)
-------------------------------------------------


Description(描述)
=================

These four pixel formats are packed raw sRGB / Bayer formats with 12
bits per colour. Every two consecutive samples are packed into three
bytes. Each of the first two bytes contain the 8 high order bits of
the pixels, and the third byte contains the four least significant
bits of each pixel, in the same order.
(这四种像素格式是每个颜色 12 位的打包原始 sRGB/Bayer 格式。每两个
连续样本被打包进三个字节。前两个字节各包含像素的 8 个高位，第三个
字节按相同顺序包含每个像素的 4 个最低有效位。)

Each n-pixel row contains n/2 green samples and n/2 blue or red
samples, with alternating green-red and green-blue rows. They are
conventionally described as GRGR... BGBG..., RGRG... GBGB..., etc.
Below is an example of a small V4L2_PIX_FMT_SBGGR12P image:
(每个 n 像素的行包含 n/2 个绿色样本和 n/2 个蓝色或红色样本，绿-红行
与绿-蓝行交替出现。按惯例它们被描述为 GRGR... BGBG...、RGRG...
GBGB... 等。下面是一个小的 V4L2_PIX_FMT_SBGGR12P 图像的示例：)

**Byte Order.**
(字节顺序。)
Each cell is one byte.
(每个单元格为一个字节。)

.. tabularcolumns:: |p{2.2cm}|p{1.2cm}|p{1.2cm}|p{3.1cm}|p{1.2cm}|p{1.2cm}|p{6.4cm}|


.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       2 1 1 1 1 1 1


    -  -  start + 0:
       -  B\ :sub:`00high`
       -  G\ :sub:`01high`
       -  G\ :sub:`01low`\ (bits 7--4)

          B\ :sub:`00low`\ (bits 3--0)
       -  B\ :sub:`02high`
       -  G\ :sub:`03high`
       -  G\ :sub:`03low`\ (bits 7--4)

          B\ :sub:`02low`\ (bits 3--0)

    -  -  start + 6:
       -  G\ :sub:`10high`
       -  R\ :sub:`11high`
       -  R\ :sub:`11low`\ (bits 7--4)

          G\ :sub:`10low`\ (bits 3--0)
       -  G\ :sub:`12high`
       -  R\ :sub:`13high`
       -  R\ :sub:`13low`\ (bits 7--4)

          G\ :sub:`12low`\ (bits 3--0)
    -  -  start + 12:
       -  B\ :sub:`20high`
       -  G\ :sub:`21high`
       -  G\ :sub:`21low`\ (bits 7--4)

          B\ :sub:`20low`\ (bits 3--0)
       -  B\ :sub:`22high`
       -  G\ :sub:`23high`
       -  G\ :sub:`23low`\ (bits 7--4)

          B\ :sub:`22low`\ (bits 3--0)
    -  -  start + 18:
       -  G\ :sub:`30high`
       -  R\ :sub:`31high`
       -  R\ :sub:`31low`\ (bits 7--4)

          G\ :sub:`30low`\ (bits 3--0)
       -  G\ :sub:`32high`
       -  R\ :sub:`33high`
       -  R\ :sub:`33low`\ (bits 7--4)

          G\ :sub:`32low`\ (bits 3--0)
