.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-selection-targets:

***************************
Selection targets(选择目标)
***************************

The precise meaning of the selection targets may be dependent on which
of the two interfaces they are used.
(选择目标的精确含义可能取决于它们被用于这两个接口中的哪一个。)


.. _v4l2-selection-targets-table:

.. raw:: latex

   \small

.. tabularcolumns:: |p{6.2cm}|p{1.4cm}|p{7.3cm}|p{1.2cm}|p{0.8cm}|

.. cssclass:: longtable

.. flat-table:: Selection target definitions
    :header-rows:  1
    :stub-columns: 0

    * - Target name(目标名称)
      - id
      - Definition(定义)
      - Valid for V4L2(对 V4L2 有效)
      - Valid for V4L2 subdev(对 V4L2 subdev 有效)
    * - ``V4L2_SEL_TGT_CROP``
      - 0x0000
      - Crop rectangle. Defines the cropped area.
        (裁剪矩形。定义裁剪区域。)
      - Yes
      - Yes
    * - ``V4L2_SEL_TGT_CROP_DEFAULT``
      - 0x0001
      - Suggested cropping rectangle that covers the "whole picture".
        This includes only active pixels and excludes other non-active
        pixels such as black pixels.
        (建议的裁剪矩形，覆盖"完整图像"。它只包含有效像素，不包括黑色
        像素等其他非有效像素。)
      - Yes
      - Yes
    * - ``V4L2_SEL_TGT_CROP_BOUNDS``
      - 0x0002
      - Bounds of the crop rectangle. All valid crop rectangles fit inside
	the crop bounds rectangle.
        (裁剪矩形的边界。所有有效的裁剪矩形都位于裁剪边界矩形之内。)
      - Yes
      - Yes
    * - ``V4L2_SEL_TGT_NATIVE_SIZE``
      - 0x0003
      - The native size of the device, e.g. a sensor's pixel array.
	``left`` and ``top`` fields are zero for this target.
        (设备的原始尺寸，例如传感器的像素阵列。对于此目标， ``left`` 与
        ``top`` 字段为零。)
      - Yes
      - Yes
    * - ``V4L2_SEL_TGT_COMPOSE``
      - 0x0100
      - Compose rectangle. Used to configure scaling and composition.
        (合成矩形。用于配置缩放与合成。)
      - Yes
      - Yes
    * - ``V4L2_SEL_TGT_COMPOSE_DEFAULT``
      - 0x0101
      - Suggested composition rectangle that covers the "whole picture".
        (建议的合成矩形，覆盖"完整图像"。)
      - Yes
      - No
    * - ``V4L2_SEL_TGT_COMPOSE_BOUNDS``
      - 0x0102
      - Bounds of the compose rectangle. All valid compose rectangles fit
	inside the compose bounds rectangle.
        (合成矩形的边界。所有有效的合成矩形都位于合成边界矩形之内。)
      - Yes
      - Yes
    * - ``V4L2_SEL_TGT_COMPOSE_PADDED``
      - 0x0103
      - The active area and all padding pixels that are inserted or
	modified by hardware.
        (有效区域以及由硬件插入或修改的所有填充像素。)
      - Yes
      - No

.. raw:: latex

   \normalsize
