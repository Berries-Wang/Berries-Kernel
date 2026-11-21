.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-SRGGB10:
.. _v4l2-pix-fmt-sbggr10:
.. _v4l2-pix-fmt-sgbrg10:
.. _v4l2-pix-fmt-sgrbg10:

***************************************************************************************************************************
V4L2_PIX_FMT_SRGGB10 ('RG10'), V4L2_PIX_FMT_SGRBG10 ('BA10'), V4L2_PIX_FMT_SGBRG10 ('GB10'), V4L2_PIX_FMT_SBGGR10 ('BG10'),
***************************************************************************************************************************


V4L2_PIX_FMT_SGRBG10
V4L2_PIX_FMT_SGBRG10
V4L2_PIX_FMT_SBGGR10
10-bit Bayer formats expanded to 16 bits
(V4L2_PIX_FMT_SGRBG10、V4L2_PIX_FMT_SGBRG10、V4L2_PIX_FMT_SBGGR10——
扩展为 16 位的 10 位 Bayer 格式。)


Description(描述)
=================

These four pixel formats are raw sRGB / Bayer formats with 10 bits per
sample. Each sample is stored in a 16-bit word, with 6 unused
high bits filled with zeros. Each n-pixel row contains n/2 green samples and
n/2 blue or red samples, with alternating red and blue rows. Bytes are
stored in memory in little endian order. They are conventionally described
as GRGR... BGBG..., RGRG... GBGB..., etc. Below is an example of one of
these formats:
(这四种像素格式是每样本 10 位的 raw sRGB / Bayer 格式。每个样本存储在
一个 16 位字中，6 个未用的高位以零填充。每个 n 像素行包含 n/2 个绿色
样本和 n/2 个蓝色或红色样本，红蓝行交替。字节在内存中以小端序存储。
它们通常被描述为 GRGR... BGBG...、RGRG... GBGB... 等。下面是其中一种
格式的示例：)

**Byte Order.**
Each cell is one byte, the 6 most significant bits in the high bytes
are 0.
( **字节序。** 每个单元是一个字节，高字节中的最高 6 个有效位为 0。)




.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - B\ :sub:`00low`
      - B\ :sub:`00high`
      - G\ :sub:`01low`
      - G\ :sub:`01high`
      - B\ :sub:`02low`
      - B\ :sub:`02high`
      - G\ :sub:`03low`
      - G\ :sub:`03high`
    * - start + 8:
      - G\ :sub:`10low`
      - G\ :sub:`10high`
      - R\ :sub:`11low`
      - R\ :sub:`11high`
      - G\ :sub:`12low`
      - G\ :sub:`12high`
      - R\ :sub:`13low`
      - R\ :sub:`13high`
    * - start + 16:
      - B\ :sub:`20low`
      - B\ :sub:`20high`
      - G\ :sub:`21low`
      - G\ :sub:`21high`
      - B\ :sub:`22low`
      - B\ :sub:`22high`
      - G\ :sub:`23low`
      - G\ :sub:`23high`
    * - start + 24:
      - G\ :sub:`30low`
      - G\ :sub:`30high`
      - R\ :sub:`31low`
      - R\ :sub:`31high`
      - G\ :sub:`32low`
      - G\ :sub:`32high`
      - R\ :sub:`33low`
      - R\ :sub:`33high`
