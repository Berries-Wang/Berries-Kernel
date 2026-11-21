.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-sdr-fmt-cu8:

**************************************************
V4L2_SDR_FMT_CU8 ('CU08')(复数无符号 8 位 IQ 样本)
**************************************************

Complex unsigned 8-bit IQ sample
(复数无符号 8 位 IQ 样本。)


Description(描述)
=================

This format contains sequence of complex number samples. Each complex
number consist two parts, called In-phase and Quadrature (IQ). Both I
and Q are represented as a 8 bit unsigned number. I value comes first
and Q value after that.
(该格式包含一系列复数样本。每个复数由两部分组成，称为同相（In-phase）与
正交（Quadrature，IQ）。I 和 Q 都表示为 8 位无符号数。I 值在前，Q 值在后。)

**Byte Order.**
Each cell is one byte.
(**字节序。** 每个单元格为一字节。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - I'\ :sub:`0`
    * - start + 1:
      - Q'\ :sub:`0`
