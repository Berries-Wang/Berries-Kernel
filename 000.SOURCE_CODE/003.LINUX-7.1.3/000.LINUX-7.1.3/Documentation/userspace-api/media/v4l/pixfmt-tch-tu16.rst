.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-TCH-FMT-TU16:

********************************
V4L2_TCH_FMT_TU16 ('TU16')
********************************

*man V4L2_TCH_FMT_TU16(2)*

16-bit unsigned little endian raw touch data
(16 位无符号小端原始触摸数据。)


Description(描述)
=================

This format represents unsigned 16-bit data from a touch controller.
(此格式表示来自触摸控制器的无符号 16 位数据。)

This may be used for output for raw and reference data. Values may range from
0 to 65535.
(这可用于输出原始数据与参考数据。取值范围可以从 0 到 65535。)

**Byte Order.**
Each cell is one byte.
( **字节序。** 每个单元是一个字节。)


.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       2 1 1 1 1 1 1 1 1

    * - start + 0:
      - R'\ :sub:`00low`
      - R'\ :sub:`00high`
      - R'\ :sub:`01low`
      - R'\ :sub:`01high`
      - R'\ :sub:`02low`
      - R'\ :sub:`02high`
      - R'\ :sub:`03low`
      - R'\ :sub:`03high`
    * - start + 8:
      - R'\ :sub:`10low`
      - R'\ :sub:`10high`
      - R'\ :sub:`11low`
      - R'\ :sub:`11high`
      - R'\ :sub:`12low`
      - R'\ :sub:`12high`
      - R'\ :sub:`13low`
      - R'\ :sub:`13high`
    * - start + 16:
      - R'\ :sub:`20low`
      - R'\ :sub:`20high`
      - R'\ :sub:`21low`
      - R'\ :sub:`21high`
      - R'\ :sub:`22low`
      - R'\ :sub:`22high`
      - R'\ :sub:`23low`
      - R'\ :sub:`23high`
    * - start + 24:
      - R'\ :sub:`30low`
      - R'\ :sub:`30high`
      - R'\ :sub:`31low`
      - R'\ :sub:`31high`
      - R'\ :sub:`32low`
      - R'\ :sub:`32high`
      - R'\ :sub:`33low`
      - R'\ :sub:`33high`
