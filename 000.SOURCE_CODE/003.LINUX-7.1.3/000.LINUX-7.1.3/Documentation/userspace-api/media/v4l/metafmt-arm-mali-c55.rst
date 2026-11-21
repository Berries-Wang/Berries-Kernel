.. SPDX-License-Identifier: GPL-2.0

.. _v4l2-meta-fmt-mali-c55-params:
.. _v4l2-meta-fmt-mali-c55-stats:

*****************************************************************************
V4L2_META_FMT_MALI_C55_STATS ('C55S'), V4L2_META_FMT_MALI_C55_PARAMS ('C55P')
*****************************************************************************

3A Statistics(3A 统计)
======================

The ISP device collects different statistics over an input bayer frame. Those
statistics can be obtained by userspace from the
:ref:`mali-c55 3a stats <mali-c55-3a-stats>` metadata capture video node, using
the :c:type:`v4l2_meta_format` interface. The buffer contains a single instance
of the C structure :c:type:`mali_c55_stats_buffer` defined in
``mali-c55-config.h``, so the structure can be obtained from the buffer by:
(ISP 设备在一帧输入的 bayer 图像上收集不同的统计信息。用户空间可以从
:ref:`mali-c55 3a stats <mali-c55-3a-stats>` 元数据捕获视频节点获取这
些统计信息，使用的是 :c:type:`v4l2_meta_format` 接口。缓冲区包含定义
在 ``mali-c55-config.h`` 中的 C 结构 :c:type:`mali_c55_stats_buffer` 的
单个实例，因此可以通过以下方式从缓冲区取得该结构：)

.. code-block:: C

	struct mali_c55_stats_buffer *stats =
		(struct mali_c55_stats_buffer *)buf;

For details of the statistics see :c:type:`mali_c55_stats_buffer`.
(统计信息的详情参见 :c:type:`mali_c55_stats_buffer`。)

Configuration Parameters(配置参数)
==================================

The configuration parameters are passed to the :ref:`mali-c55 3a params
<mali-c55-3a-params>` metadata output video node, using the
:c:type:`v4l2_meta_format` interface. Rather than a single struct containing
sub-structs for each configurable area of the ISP, parameters for the Mali-C55
use the v4l2-isp parameters system, through which groups of parameters are
defined as distinct structs or "blocks" which may be added to the data member of
:c:type:`v4l2_isp_params_buffer`. Userspace is responsible for populating the
data member with the blocks that need to be configured by the driver.  Each
block-specific struct embeds :c:type:`v4l2_isp_params_block_header` as its first
member and userspace must populate the type member with a value from
:c:type:`mali_c55_param_block_type`.
(配置参数通过 :c:type:`v4l2_meta_format` 接口传递给 :ref:`mali-c55 3a
params <mali-c55-3a-params>` 元数据输出视频节点。Mali-C55 的参数没有
采用把 ISP 每个可配置区域都作为子结构的单一 struct，而是使用 v4l2-isp
参数系统：通过该系统，各组参数被定义为独立的 struct 或 "block"，它们
可以加入到 :c:type:`v4l2_isp_params_buffer` 的 data 成员中。用户空间
负责用需要驱动程序配置的 block 来填充 data 成员。每个 block 专属的
struct 都把 :c:type:`v4l2_isp_params_block_header` 作为其第一个成员，
并且用户空间必须用来自 :c:type:`mali_c55_param_block_type` 的值填充其
type 成员。)

.. code-block:: c

	struct v4l2_isp_params_buffer *params =
		(struct v4l2_isp_params_buffer *)buffer;

	params->version = V4L2_ISP_PARAMS_VERSION_V1;
	params->data_size = 0;

	void *data = (void *)params->data;

	struct mali_c55_params_awb_gains *gains =
		(struct mali_c55_params_awb_gains *)data;

	gains->header.type = MALI_C55_PARAM_BLOCK_AWB_GAINS;
	gains->header.flags |= V4L2_ISP_PARAMS_FL_BLOCK_ENABLE;
	gains->header.size = sizeof(struct mali_c55_params_awb_gains);

	gains->gain00 = 256;
	gains->gain00 = 256;
	gains->gain00 = 256;
	gains->gain00 = 256;

	data += sizeof(struct mali_c55_params_awb_gains);
	params->data_size += sizeof(struct mali_c55_params_awb_gains);

	struct mali_c55_params_sensor_off_preshading *blc =
		(struct mali_c55_params_sensor_off_preshading *)data;

	blc->header.type = MALI_C55_PARAM_BLOCK_SENSOR_OFFS;
	blc->header.flags |= V4L2_ISP_PARAMS_FL_BLOCK_ENABLE;
	blc->header.size = sizeof(struct mali_c55_params_sensor_off_preshading);

	blc->chan00 = 51200;
	blc->chan01 = 51200;
	blc->chan10 = 51200;
	blc->chan11 = 51200;

	params->data_size += sizeof(struct mali_c55_params_sensor_off_preshading);

Arm Mali-C55 uAPI data types(Arm Mali-C55 uAPI 数据类型)
========================================================

.. kernel-doc:: include/uapi/linux/media/arm/mali-c55-config.h
