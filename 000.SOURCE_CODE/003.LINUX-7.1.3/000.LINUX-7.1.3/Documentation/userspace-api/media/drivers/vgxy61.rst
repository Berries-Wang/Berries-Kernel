.. SPDX-License-Identifier: GPL-2.0

============================================================
ST VGXY61 camera sensor driver(ST VGXY61 相机传感器驱动)
============================================================

The ST VGXY61 driver implements the following controls:
(ST VGXY61 驱动实现了以下控件：)

``V4L2_CID_HDR_SENSOR_MODE``
-------------------------------
    Change the sensor HDR mode. A HDR picture is obtained by merging two
    captures of the same scene using two different exposure periods.
    (更改传感器的 HDR 模式。HDR 图像通过使用两个不同曝光周期对同一场景进行两次拍摄并合并而获得。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 4

    * - HDR linearize(HDR 线性化)
      - The merger outputs a long exposure capture as long as it is not
        saturated.(只要未饱和，合并器就会输出长曝光图像。)
    * - HDR subtraction(HDR 相减)
      - This involves subtracting the short exposure frame from the long
        exposure frame.(即从长曝光帧中减去短曝光帧。)
    * - No HDR(无 HDR)
      - This mode is used for standard dynamic range (SDR) exposures.
        (该模式用于标准动态范围 (SDR) 曝光。)
