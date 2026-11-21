.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _image-source-controls:

**********************************************
Image Source Control Reference(图像源控制参考)
**********************************************

The Image Source control class is intended for low-level control of
image source devices such as image sensors. The devices feature an
analogue to digital converter and a bus transmitter to transmit the
image data out of the device.
(Image Source 控制类用于对图像传感器等图像源设备进行低层控制。这些
设备具有模数转换器和用于把图像数据传出设备的总线发送器。)


.. _image-source-control-id:

Image Source Control IDs(图像源控制 ID)
=======================================

``V4L2_CID_IMAGE_SOURCE_CLASS (class)``
    The IMAGE_SOURCE class descriptor.
    (IMAGE_SOURCE 类描述符。)

``V4L2_CID_VBLANK (integer)``
    Vertical blanking. The idle period after every frame during which no
    image data is produced. The unit of vertical blanking is a line.
    Every line has length of the image width plus horizontal blanking at
    the pixel rate defined by ``V4L2_CID_PIXEL_RATE`` control in the
    same sub-device.
    (垂直消隐。每帧之后不产生图像数据的空闲时段。垂直消隐的单位是
    行。每一行的长度为图像宽度加上水平消隐，按同一子设备中
    ``V4L2_CID_PIXEL_RATE`` 控制定义的像素速率计时。)

``V4L2_CID_HBLANK (integer)``
    Horizontal blanking. The idle period after every line of image data
    during which no image data is produced. The unit of horizontal
    blanking is pixels.
    (水平消隐。每行图像数据之后不产生图像数据的空闲时段。水平消隐
    的单位是像素。)

``V4L2_CID_ANALOGUE_GAIN (integer)``
    Analogue gain is gain affecting all colour components in the pixel
    matrix. The gain operation is performed in the analogue domain
    before A/D conversion.
    (模拟增益是影响像素矩阵中所有颜色分量的增益。增益操作在 A/D
    转换之前在模拟域中执行。)

``V4L2_CID_TEST_PATTERN_RED (integer)``
    Test pattern red colour component.
    (测试图案的红色分量。)

``V4L2_CID_TEST_PATTERN_GREENR (integer)``
    Test pattern green (next to red) colour component.
    (测试图案的绿色(紧邻红色)分量。)

``V4L2_CID_TEST_PATTERN_BLUE (integer)``
    Test pattern blue colour component.
    (测试图案的蓝色分量。)

``V4L2_CID_TEST_PATTERN_GREENB (integer)``
    Test pattern green (next to blue) colour component.
    (测试图案的绿色(紧邻蓝色)分量。)

``V4L2_CID_UNIT_CELL_SIZE (struct)``
    This control returns the unit cell size in nanometers. The struct
    :c:type:`v4l2_area` provides the width and the height in separate
    fields to take into consideration asymmetric pixels.
    This control does not take into consideration any possible hardware
    binning.
    The unit cell consists of the whole area of the pixel, sensitive and
    non-sensitive.
    This control is required for automatic calibration of sensors/cameras.
    (该控制返回以纳米为单位的单元胞(unit cell)大小。struct
    :c:type:`v4l2_area` 把宽度和高度放在单独的字段中提供，以顾及非
    对称像素。该控制不考虑任何可能的硬件合并(binning)。单元胞包含
    像素的整个区域，包括感光部分和非感光部分。传感器/摄像头的自动
    校准需要该控制。)

.. c:type:: v4l2_area

.. flat-table:: struct v4l2_area
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``width``
      - Width of the area.
        (区域的宽度。)
    * - __u32
      - ``height``
      - Height of the area.
        (区域的高度。)

``V4L2_CID_NOTIFY_GAINS (integer array)``
    The sensor is notified what gains will be applied to the different
    colour channels by subsequent processing (such as by an ISP). The
    sensor is merely informed of these values in case it performs
    processing that requires them, but it does not apply them itself to
    the output pixels.
    (通知传感器后续处理(例如由 ISP 执行)将对不同颜色通道施加哪些
    增益。传感器只是被告知这些值，以防它执行需要这些值的处理，但它
    不会自行把它们应用到输出像素上。)

    Currently it is defined only for Bayer sensors, and is an array
    control taking 4 gain values, being the gains for each of the
    Bayer channels. The gains are always in the order B, Gb, Gr and R,
    irrespective of the exact Bayer order of the sensor itself.
    (目前它只为 Bayer 传感器定义，是一个数组控制，接受 4 个增益值，
    分别对应每个 Bayer 通道的增益。无论传感器本身的具体 Bayer 顺序
    如何，增益的顺序始终是 B、Gb、Gr 和 R。)

    The use of an array allows this control to be extended to sensors
    with, for example, non-Bayer CFAs (colour filter arrays).
    (使用数组使该控制可以扩展到例如非 Bayer CFA(颜色滤波阵列)的
    传感器。)

    The units for the gain values are linear, with the default value
    representing a gain of exactly 1.0. For example, if this default value
    is reported as being (say) 128, then a value of 192 would represent
    a gain of exactly 1.5.
    (增益值的单位是线性的，默认值表示恰好为 1.0 的增益。例如，如果
    该默认值报告为(比方说)128，那么值 192 就表示恰好为 1.5 的增益。)
