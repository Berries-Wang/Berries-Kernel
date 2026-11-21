.. SPDX-License-Identifier: GPL-2.0
.. c:namespace:: V4L

Guidelines for Video4Linux pixel format 4CCs(Video4Linux 像素格式 4CC 指南)
===========================================================================

Guidelines for Video4Linux 4CC codes defined using v4l2_fourcc() are
specified in this document. First of the characters defines the nature of
the pixel format, compression and colour space. The interpretation of the
other three characters depends on the first one.
(本文档给出使用 v4l2_fourcc() 定义的 Video4Linux 4CC 编码的指南。第一个字
符定义像素格式的性质、压缩方式与色彩空间。其余三个字符的释义取决于第一个
字符。)

Existing 4CCs may not obey these guidelines.
(现有的 4CC 可能并不遵循这些指南。)

Raw bayer(原始 Bayer)
---------------------

The following first characters are used by raw bayer formats:
(以下首字符用于原始 bayer 格式：)

- B: raw bayer, uncompressed
  (B：原始 bayer，未压缩)
- b: raw bayer, DPCM compressed
  (b：原始 bayer，DPCM 压缩)
- a: A-law compressed
  (a：A-law 压缩)
- u: u-law compressed
  (u：u-law 压缩)

2nd character: pixel order
(第二个字符：像素顺序)

- B: BGGR
- G: GBRG
- g: GRBG
- R: RGGB

3rd character: uncompressed bits-per-pixel 0--9, A--
(第三个字符：未压缩的每像素位数 0--9、A--)

4th character: compressed bits-per-pixel 0--9, A--
(第四个字符：压缩的每像素位数 0--9、A--)
