.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-SDR-FMT-RU12LE:

******************************************************
V4L2_SDR_FMT_RU12LE ('RU12')(实数无符号 12 位小端样本)
******************************************************


Real unsigned 12-bit little endian sample
(实数无符号 12 位小端样本。)


Description(描述)
=================

This format contains sequence of real number samples. Each sample is
represented as a 12 bit unsigned little endian number. Sample is stored
in 16 bit space with unused high bits padded with 0.
(该格式包含一系列实数样本。每个样本表示为 12 位无符号小端数。样本存储在
16 位空间中，未用的高位以 0 填充。)

**Byte Order.**
Each cell is one byte.
(**字节序。** 每个单元格为一字节。)




.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - start + 0:
      - I'\ :sub:`0[7:0]`
      - I'\ :sub:`0[11:8]`
