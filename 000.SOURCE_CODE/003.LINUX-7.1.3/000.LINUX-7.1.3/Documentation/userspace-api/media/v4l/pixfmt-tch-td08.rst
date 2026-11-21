.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-TCH-FMT-DELTA-TD08:

****************************************************
V4L2_TCH_FMT_DELTA_TD08 ('TD08')(8 位有符号触摸差值)
****************************************************

*man V4L2_TCH_FMT_DELTA_TD08(2)*

8-bit signed Touch Delta
(8 位有符号触摸差值。)

Description(描述)
=================

This format represents delta data from a touch controller.
(此格式表示来自触摸控制器的差值 (delta) 数据。)

Delta values may range from -128 to 127. Typically the values will vary through
a small range depending on whether the sensor is touched or not. The full value
may be seen if one of the touchscreen nodes has a fault or the line is not
connected.
(差值可以在 -128 到 127 之间。通常，数值会根据传感器是否被触摸而在一个小
范围内变化。如果某个触摸屏节点有故障或线路未连接，则可能看到完整量程的
数值。)

**Byte Order.**
Each cell is one byte.
(字节序。每个单元格为一个字节。)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       2 1 1 1 1

    * - start + 0:
      - D'\ :sub:`00`
      - D'\ :sub:`01`
      - D'\ :sub:`02`
      - D'\ :sub:`03`
    * - start + 4:
      - D'\ :sub:`10`
      - D'\ :sub:`11`
      - D'\ :sub:`12`
      - D'\ :sub:`13`
    * - start + 8:
      - D'\ :sub:`20`
      - D'\ :sub:`21`
      - D'\ :sub:`22`
      - D'\ :sub:`23`
    * - start + 12:
      - D'\ :sub:`30`
      - D'\ :sub:`31`
      - D'\ :sub:`32`
      - D'\ :sub:`33`
