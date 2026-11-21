.. -*- coding: utf-8; mode: rst -*-
.. c:namespace:: V4L

.. _V4L2-PIX-FMT-CNF4:

******************************************
V4L2_PIX_FMT_CNF4 ('CNF4')(深度置信度格式)
******************************************

Depth sensor confidence information as a 4 bits per pixel packed array
(深度传感器置信度信息，以每像素 4 位的打包数组表示)

Description(描述)
=================

Proprietary format used by Intel RealSense Depth cameras containing depth
confidence information in range 0-15 with 0 indicating that the sensor was
unable to resolve any signal and 15 indicating maximum level of confidence for
the specific sensor (actual error margins might change from sensor to sensor).
(Intel RealSense 深度相机所使用的专有格式，包含取值范围 0-15 的深度置信度信
息：0 表示传感器完全无法解析出任何信号，15 表示对该特定传感器而言置信度达
到最高（实际误差界限可能因传感器而异）。)

Every two consecutive pixels are packed into a single byte.
Bits 0-3 of byte n refer to confidence value of depth pixel 2*n,
bits 4-7 to confidence value of depth pixel 2*n+1.
(每两个连续像素打包进一个字节。字节 n 的第 0-3 位对应深度像素 2*n 的置信
度值，第 4-7 位对应深度像素 2*n+1 的置信度值。)

**Bit-packed representation.**
(**位打包表示。**)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 64 64

    * - Y'\ :sub:`01[3:0]`\ (bits 7--4) Y'\ :sub:`00[3:0]`\ (bits 3--0)
      - Y'\ :sub:`03[3:0]`\ (bits 7--4) Y'\ :sub:`02[3:0]`\ (bits 3--0)
