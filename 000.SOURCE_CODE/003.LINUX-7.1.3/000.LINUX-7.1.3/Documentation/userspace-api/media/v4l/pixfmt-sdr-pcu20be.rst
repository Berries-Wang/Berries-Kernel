.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-SDR-FMT-PCU20BE:

****************************************************
V4L2_SDR_FMT_PCU20BE ('PC20')(20 位大端 IQ 样本格式)
****************************************************

Planar complex unsigned 20-bit big endian IQ sample
(平面复数无符号 20 位大端 IQ 样本。)

Description(描述)
=================

This format contains a sequence of complex number samples. Each complex
number consist of two parts called In-phase and Quadrature (IQ). Both I
and Q are represented as a 20 bit unsigned big endian number stored in
32 bit space. The remaining unused bits within the 32 bit space will be
padded with 0. I value starts first and Q value starts at an offset
equalling half of the buffer size (i.e.) offset = buffersize/2. Out of
the 20 bits, bit 19:2 (18 bit) is data and bit 1:0 (2 bit) can be any
value.
(此格式包含一个复数样本序列。每个复数由两部分组成，称为同相 (In-phase) 和
正交 (Quadrature) (IQ)。I 和 Q 都表示为存储在 32 位空间中的 20 位无符号
大端数。32 位空间中剩余的未用位用 0 填充。I 值先开始，Q 值从等于缓冲区大小
一半的偏移处开始 (即 offset = buffersize/2)。在 20 位中，位 19:2 (18 位) 是
数据，位 1:0 (2 位) 可以是任意值。)

**Byte Order.**
Each cell is one byte.
(字节序。每个单元格为一个字节。)

.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    * -  Offset:
      -  Byte B0
      -  Byte B1
      -  Byte B2
      -  Byte B3
    * -  start + 0:
      -  I'\ :sub:`0[19:12]`
      -  I'\ :sub:`0[11:4]`
      -  I'\ :sub:`0[3:0]; B2[3:0]=pad`
      -  pad
    * -  start + 4:
      -  I'\ :sub:`1[19:12]`
      -  I'\ :sub:`1[11:4]`
      -  I'\ :sub:`1[3:0]; B2[3:0]=pad`
      -  pad
    * -  ...
    * - start + offset:
      -  Q'\ :sub:`0[19:12]`
      -  Q'\ :sub:`0[11:4]`
      -  Q'\ :sub:`0[3:0]; B2[3:0]=pad`
      -  pad
    * - start + offset + 4:
      -  Q'\ :sub:`1[19:12]`
      -  Q'\ :sub:`1[11:4]`
      -  Q'\ :sub:`1[3:0]; B2[3:0]=pad`
      -  pad
