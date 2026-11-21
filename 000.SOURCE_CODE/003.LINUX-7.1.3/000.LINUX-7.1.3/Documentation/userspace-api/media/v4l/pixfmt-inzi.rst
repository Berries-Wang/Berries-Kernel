.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-INZI:

**************************
V4L2_PIX_FMT_INZI ('INZI')
**************************

Infrared 10-bit linked with Depth 16-bit images
(10 位红外图像与 16 位深度图像相链接。)


Description(描述)
=================

Proprietary multi-planar format used by Intel SR300 Depth cameras, comprise of
Infrared image followed by Depth data. The pixel definition is 32-bpp,
with the Depth and Infrared Data split into separate continuous planes of
identical dimensions.
(Intel SR300 深度相机使用的专有多平面格式，由红外图像后跟深度数据组成。
像素定义为 32 bpp，深度数据与红外数据被拆分到两个尺寸相同的独立连续平
面中。)



The first plane - Infrared data - is stored according to
:ref:`V4L2_PIX_FMT_Y10 <V4L2-PIX-FMT-Y10>` greyscale format.
Each pixel is 16-bit cell, with actual data stored in the 10 LSBs
with values in range 0 to 1023.
The six remaining MSBs are padded with zeros.
(第一个平面——红外数据——按照 :ref:`V4L2_PIX_FMT_Y10 <V4L2-PIX-FMT-Y10>`
灰度格式存储。每个像素是一个 16 位单元，实际数据存储在低 10 位 (LSB)
中，取值范围为 0 到 1023。其余 6 个高位 (MSB) 以零填充。)


The second plane provides 16-bit per-pixel Depth data arranged in
:ref:`V4L2-PIX-FMT-Z16 <V4L2-PIX-FMT-Z16>` format.
(第二个平面提供每像素 16 位的深度数据，按
:ref:`V4L2-PIX-FMT-Z16 <V4L2-PIX-FMT-Z16>` 格式排布。)


**Frame Structure.**
Each cell is a 16-bit word with more significant data stored at higher
memory address (byte order is little-endian).
( **帧结构。** 每个单元是一个 16 位字，较高有效位的数据存储在较高的内
存地址中 (字节序为小端)。)


.. raw:: latex

    \small

.. tabularcolumns:: |p{2.5cm}|p{2.5cm}|p{2.5cm}|p{2.5cm}|p{2.5cm}|p{2.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 1
    :widths:    1 1 1 1 1 1

    * - Ir\ :sub:`0,0`
      - Ir\ :sub:`0,1`
      - Ir\ :sub:`0,2`
      - ...
      - ...
      - ...
    * - :cspan:`5` ...
    * - :cspan:`5` Infrared Data
    * - :cspan:`5` ...
    * - ...
      - ...
      - ...
      - Ir\ :sub:`n-1,n-3`
      - Ir\ :sub:`n-1,n-2`
      - Ir\ :sub:`n-1,n-1`
    * - Depth\ :sub:`0,0`
      - Depth\ :sub:`0,1`
      - Depth\ :sub:`0,2`
      - ...
      - ...
      - ...
    * - :cspan:`5` ...
    * - :cspan:`5` Depth Data
    * - :cspan:`5` ...
    * - ...
      - ...
      - ...
      - Depth\ :sub:`n-1,n-3`
      - Depth\ :sub:`n-1,n-2`
      - Depth\ :sub:`n-1,n-1`

.. raw:: latex

    \normalsize
