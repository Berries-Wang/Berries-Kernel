.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-meta-fmt-vivid:

**********************************************
V4L2_META_FMT_VIVID ('VIVD')(VIVID 元数据格式)
**********************************************

VIVID Metadata Format
(VIVID 元数据格式。)


Description(描述)
=================

This describes metadata format used by the vivid driver.
(本节描述 vivid 驱动所使用的元数据格式。)

It sets Brightness, Saturation, Contrast and Hue, each of which maps to
corresponding controls of the vivid driver with respect to the range and default values.
(它设置亮度（Brightness）、饱和度（Saturation）、对比度（Contrast）和色调
（Hue），各项在取值范围与默认值上分别对应 vivid 驱动的相应控件。)

It contains the following fields:
(它包含以下字段：)

.. flat-table:: VIVID Metadata
    :widths: 1 4
    :header-rows:  1
    :stub-columns: 0

    * - Field(字段)
      - Description(描述)
    * - u16 brightness;
      - Image brightness, the value is in the range 0 to 255, with the default value as 128.(图像亮度，取值范围为 0 到 255，默认值为 128。)
    * - u16 contrast;
      - Image contrast, the value is in the range 0 to 255, with the default value as 128.(图像对比度，取值范围为 0 到 255，默认值为 128。)
    * - u16 saturation;
      - Image color saturation, the value is in the range 0 to 255, with the default value as 128.(图像色彩饱和度，取值范围为 0 到 255，默认值为 128。)
    * - s16 hue;
      - Image color balance, the value is in the range -128 to 128, with the default value as 0.(图像色彩平衡，取值范围为 -128 到 128，默认值为 0。)
