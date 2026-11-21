.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-Y12I:

******************************************
V4L2_PIX_FMT_Y12I ('Y12I')(交错 12 位灰度)
******************************************

Interleaved grey-scale image, e.g. from a stereo-pair
(交错灰度图像，例如来自立体像对。)


Description(描述)
=================

This is a grey-scale image with a depth of 12 bits per pixel, but with
pixels from 2 sources interleaved and bit-packed. Each pixel is stored
in a 24-bit word in the little-endian order. On a little-endian machine
these pixels can be deinterlaced using

.. code-block:: c

    __u8 *buf;
    left0 = 0xfff & *(__u16 *)buf;
    right0 = *(__u16 *)(buf + 1) >> 4;

(这是一种每像素 12 位深度的灰度图像，但来自两个源的像素交错并进行位打包。
每个像素以 24 位字按小端序存储。在小端机器上，这些像素可以用以下方式去交
错：)

**Bit-packed representation.**
pixels cross the byte boundary and have a ratio of 3 bytes for each
interleaved pixel.
(**位打包表示。** 像素跨字节边界，每个交错像素占 3 字节。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - Y'\ :sub:`0left[7:0]`
      - Y'\ :sub:`0right[3:0]`\ Y'\ :sub:`0left[11:8]`
      - Y'\ :sub:`0right[11:4]`
