.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-pix-fmt-pisp-comp1-rggb:
.. _v4l2-pix-fmt-pisp-comp1-grbg:
.. _v4l2-pix-fmt-pisp-comp1-gbrg:
.. _v4l2-pix-fmt-pisp-comp1-bggr:
.. _v4l2-pix-fmt-pisp-comp1-mono:
.. _v4l2-pix-fmt-pisp-comp2-rggb:
.. _v4l2-pix-fmt-pisp-comp2-grbg:
.. _v4l2-pix-fmt-pisp-comp2-gbrg:
.. _v4l2-pix-fmt-pisp-comp2-bggr:
.. _v4l2-pix-fmt-pisp-comp2-mono:

**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
V4L2_PIX_FMT_PISP_COMP1_RGGB ('PC1R'), V4L2_PIX_FMT_PISP_COMP1_GRBG ('PC1G'), V4L2_PIX_FMT_PISP_COMP1_GBRG ('PC1g'), V4L2_PIX_FMT_PISP_COMP1_BGGR ('PC1B), V4L2_PIX_FMT_PISP_COMP1_MONO ('PC1M'), V4L2_PIX_FMT_PISP_COMP2_RGGB ('PC2R'), V4L2_PIX_FMT_PISP_COMP2_GRBG ('PC2G'), V4L2_PIX_FMT_PISP_COMP2_GBRG ('PC2g'), V4L2_PIX_FMT_PISP_COMP2_BGGR ('PC2B), V4L2_PIX_FMT_PISP_COMP2_MONO ('PC2M')
**************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************

========================================================================================
Raspberry Pi PiSP compressed 8-bit Bayer formats(Raspberry Pi PiSP 压缩 8 位 Bayer 格式)
========================================================================================

Description(描述)
=================

The Raspberry Pi ISP (PiSP) uses a family of three fixed-rate compressed Bayer
formats. A black-level offset may be subtracted to improve compression
efficiency; the nominal black level and amount of offset must be signalled out
of band. Each scanline is padded to a multiple of 8 pixels wide, and each block
of 8 horizontally-contiguous pixels is coded using 8 bytes.
(Raspberry Pi ISP (PiSP) 使用一族固定速率的压缩 Bayer 格式。可以减去一
个黑电平偏移量以提高压缩效率；标称黑电平与偏移量的大小必须在带外
(out of band) 传送。每条扫描线都填充到 8 像素宽的倍数，每个由 8 个水
平连续像素组成的块用 8 字节编码。)

Mode 1 uses a quantization and delta-based coding scheme which preserves up to
12 significant bits. Mode 2 is a simple sqrt-like companding scheme with 6 PWL
chords, preserving up to 12 significant bits. Mode 3 combines both companding
(with 4 chords) and the delta scheme, preserving up to 14 significant bits.
(模式 1 使用一种保留最多 12 个有效位的量化与基于增量的编码方案。模式
2 是一个带有 6 段 PWL 弦的简单类 sqrt 压扩 (companding) 方案，保留最多
12 个有效位。模式 3 结合了压扩 (4 段弦) 与增量方案，保留最多 14 个有
效位。)

The remainder of this description applies to Modes 1 and 3.
(本描述的其余部分适用于模式 1 和 3。)

Each block of 8 pixels is separated into even and odd phases of 4 pixels,
coded independently by 32-bit words at successive locations in memory.
The two LS bits of each 32-bit word give its "quantization mode".
(每个 8 像素块被分成各含 4 像素的偶相位与奇相位，由内存中相继位置的
32 位字独立编码。每个 32 位字的低 2 位给出其"量化模式"。)

In quantization mode 0, the lowest 321 quantization levels are multiples of
FSD/4096 and the remaining levels are successive multiples of FSD/2048.
Quantization modes 1 and 2 use linear quantization with step sizes of
FSD/1024 and FSD/512 respectively. Each of the four pixels is quantized
independently, with rounding to the nearest level.
In quantization mode 2 where the middle two samples have quantized values
(q1,q2) both in the range [384..511], they are coded using 9 bits for q1
followed by 7 bits for (q2 & 127). Otherwise, for quantization modes
0, 1 and 2: a 9-bit field encodes MIN(q1,q2) which must be in the range
[0..511] and a 7-bit field encodes (q2-q1+64) which must be in [0..127].
(在量化模式 0 中，最低的 321 个量化级是 FSD/4096 的倍数，其余各级是
FSD/2048 的连续倍数。量化模式 1 和 2 使用线性量化，步长分别为
FSD/1024 与 FSD/512。四个像素各自独立量化，并四舍五入到最近的级。
在量化模式 2 中，当中间两个样本的量化值 (q1,q2) 都在 [384..511] 范围
内时，它们用 9 位编码 q1，后跟 7 位编码 (q2 & 127)。否则，对于量化模
式 0、1 和 2：一个 9 位字段编码必须在 [0..511] 范围内的 MIN(q1,q2)，
一个 7 位字段编码必须在 [0..127] 范围内的 (q2-q1+64)。)

Each of the outer samples (q0,q3) is encoded using a 7-bit field based
on its inner neighbour q1 or q2. In quantization mode 2 where the inner
sample has a quantized value in the range [448..511], the field value is
(q0-384). Otherwise for quantization modes 0, 1 and 2: The outer sample
is encoded as (q0-MAX(0,q1-64)). q3 is likewise coded based on q2.
Each of these values must be in the range [0..127]. All these fields
of 2, 9, 7, 7, 7 bits respectively are packed in little-endian order
to give a 32-bit word with LE byte order.
(每个外侧样本 (q0,q3) 用一个基于其内侧邻居 q1 或 q2 的 7 位字段编
码。在量化模式 2 中，当内侧样本的量化值在 [448..511] 范围内时，字段
值为 (q0-384)。否则，对于量化模式 0、1 和 2：外侧样本编码为
(q0-MAX(0,q1-64))。q3 同样基于 q2 编码。这些值都必须在 [0..127] 范围
内。这些分别为 2、9、7、7、7 位的字段按小端序打包，得到一个 LE 字节
序的 32 位字。)

Quantization mode 3 has a "7.5-bit" escape, used when none of the above
encodings will fit. Each pixel value is quantized to the nearest of 176
levels, where the lowest 95 levels are multiples of FSD/256 and the
remaining levels are multiples of FSD/128 (level 175 represents values
very close to FSD and may require saturating arithmetic to decode).
(量化模式 3 有一个"7.5 位"逃逸码，用于上述编码都不适用的情况。每个
像素值被量化到 176 个级中最近的一个，其中最低的 95 级是 FSD/256 的倍
数，其余各级是 FSD/128 的倍数 (第 175 级表示非常接近 FSD 的值，解码
时可能需要饱和运算)。)

Each pair of quantized pixels (q0,q1) or (q2,q3) is jointly coded
by a 15-bit field: 2816*(q0>>4) + 16*q1 + (q0&15).
Three fields of 2, 15, 15 bits are packed in LE order {15,15,2}.
(每对量化像素 (q0,q1) 或 (q2,q3) 由一个 15 位字段联合编码：
2816*(q0>>4) + 16*q1 + (q0&15)。三个分别为 2、15、15 位的字段按 LE
顺序 {15,15,2} 打包。)

An implementation of a software decoder of compressed formats is available
in `Raspberry Pi camera applications code base
<https://github.com/raspberrypi/rpicam-apps/blob/main/image/dng.cpp>`_.
(压缩格式的一个软件解码器实现可在 `Raspberry Pi camera applications
code base <https://github.com/raspberrypi/rpicam-apps/blob/main/image/dng.cpp>`_
中找到。)
