.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-SBGGR10DPCM8:
.. _v4l2-pix-fmt-sgbrg10dpcm8:
.. _v4l2-pix-fmt-sgrbg10dpcm8:
.. _v4l2-pix-fmt-srggb10dpcm8:


***********************************************************************************************************************************************
V4L2_PIX_FMT_SBGGR10DPCM8 ('bBA8'), V4L2_PIX_FMT_SGBRG10DPCM8 ('bGA8'), V4L2_PIX_FMT_SGRBG10DPCM8 ('BD10'), V4L2_PIX_FMT_SRGGB10DPCM8 ('bRA8'),
***********************************************************************************************************************************************

*man V4L2_PIX_FMT_SBGGR10DPCM8(2)*

V4L2_PIX_FMT_SGBRG10DPCM8
V4L2_PIX_FMT_SGRBG10DPCM8
V4L2_PIX_FMT_SRGGB10DPCM8
10-bit Bayer formats compressed to 8 bits
(压缩为 8 位的 10 位 Bayer 格式。)


Description(描述)
=================

These four pixel formats are raw sRGB / Bayer formats with 10 bits per
colour compressed to 8 bits each, using DPCM compression. DPCM,
differential pulse-code modulation, is lossy. Each colour component
consumes 8 bits of memory. In other respects this format is similar to
:ref:`V4L2-PIX-FMT-SRGGB10`.
(这四种像素格式是每颜色分量 10 位、使用 DPCM 压缩到 8 位的原始 sRGB/Bayer 格式。DPCM 即差分脉冲编码调制 (differential pulse-code modulation)，是有损压缩。每个颜色分量占用 8 位内存。在其他方面，此格式与 :ref:`V4L2-PIX-FMT-SRGGB10` 类似。)
