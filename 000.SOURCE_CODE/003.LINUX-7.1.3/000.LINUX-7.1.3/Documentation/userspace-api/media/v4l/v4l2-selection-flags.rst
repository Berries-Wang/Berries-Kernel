.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-selection-flags:

*************************
Selection flags(选择标志)
*************************

.. _v4l2-selection-flags-table:

.. raw:: latex

   \small

.. tabularcolumns:: |p{5.6cm}|p{2.0cm}|p{6.5cm}|p{1.2cm}|p{1.2cm}|

.. cssclass:: longtable

.. flat-table:: Selection flag definitions
    :header-rows:  1
    :stub-columns: 0

    * - Flag name(标志名)
      - id
      - Definition(定义)
      - Valid for V4L2(V4L2 适用)
      - Valid for V4L2 subdev(V4L2 subdev 适用)
    * - ``V4L2_SEL_FLAG_GE``
      - (1 << 0)
      - Suggest the driver it should choose greater or equal rectangle (in
	size) than was requested. Albeit the driver may choose a lesser
	size, it will only do so due to hardware limitations. Without this
	flag (and ``V4L2_SEL_FLAG_LE``) the behaviour is to choose the
	closest possible rectangle.(建议驱动应选择大于或等于 (按大小) 所请求
	的矩形。虽然驱动可以选择更小的尺寸，但只有在硬件限制的情况下才会这样
	做。没有此标志 (和 ``V4L2_SEL_FLAG_LE``) 时，行为是选择最接近的可能
	矩形。)
      - Yes
      - Yes
    * - ``V4L2_SEL_FLAG_LE``
      - (1 << 1)
      - Suggest the driver it should choose lesser or equal rectangle (in
	size) than was requested. Albeit the driver may choose a greater
	size, it will only do so due to hardware limitations.(建议驱动应选择
	小于或等于 (按大小) 所请求的矩形。虽然驱动可以选择更大的尺寸，但只有
	在硬件限制的情况下才会这样做。)
      - Yes
      - Yes
    * - ``V4L2_SEL_FLAG_KEEP_CONFIG``
      - (1 << 2)
      - The configuration must not be propagated to any further processing
	steps. If this flag is not given, the configuration is propagated
	inside the subdevice to all further processing steps.(不得将配置传播
	到任何后续处理步骤。如果没有给出此标志，配置会在子设备内部传播到所有
	后续处理步骤。)
      - No
      - Yes

.. raw:: latex

   \normalsize
