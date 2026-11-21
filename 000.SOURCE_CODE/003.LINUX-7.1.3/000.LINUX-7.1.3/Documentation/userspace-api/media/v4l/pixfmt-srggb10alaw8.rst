.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-SBGGR10ALAW8:
.. _v4l2-pix-fmt-sgbrg10alaw8:
.. _v4l2-pix-fmt-sgrbg10alaw8:
.. _v4l2-pix-fmt-srggb10alaw8:

***********************************************************************************************************************************************
V4L2_PIX_FMT_SBGGR10ALAW8 ('aBA8'), V4L2_PIX_FMT_SGBRG10ALAW8 ('aGA8'), V4L2_PIX_FMT_SGRBG10ALAW8 ('agA8'), V4L2_PIX_FMT_SRGGB10ALAW8 ('aRA8'),
***********************************************************************************************************************************************

V4L2_PIX_FMT_SGBRG10ALAW8
V4L2_PIX_FMT_SGRBG10ALAW8
V4L2_PIX_FMT_SRGGB10ALAW8
10-bit Bayer formats compressed to 8 bits
(10 位 Bayer 格式，使用 A-LAW 算法压缩到 8 位。)


Description(描述)
=================

These four pixel formats are raw sRGB / Bayer formats with 10 bits per
color compressed to 8 bits each, using the A-LAW algorithm. Each color
component consumes 8 bits of memory. In other respects this format is
similar to :ref:`V4L2-PIX-FMT-SRGGB8`.
(这四种像素格式是每颜色分量 10 位、使用 A-LAW 算法压缩到 8 位的原始 sRGB/Bayer 格式。每个颜色分量占用 8 位内存。在其他方面，此格式与 :ref:`V4L2-PIX-FMT-SRGGB8` 类似。)
