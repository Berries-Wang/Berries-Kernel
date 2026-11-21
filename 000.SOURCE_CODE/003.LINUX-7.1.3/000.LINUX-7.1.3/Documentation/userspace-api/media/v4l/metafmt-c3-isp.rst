.. SPDX-License-Identifier: (GPL-2.0-only OR MIT)
.. c:namespace:: V4L

.. _v4l2-meta-fmt-c3isp-stats:
.. _v4l2-meta-fmt-c3isp-params:

******************************************************************************************
V4L2_META_FMT_C3ISP_STATS ('C3ST'), V4L2_META_FMT_C3ISP_PARAMS ('C3PM')(C3 ISP 元数据格式)
******************************************************************************************

.. c3_isp_stats_info

3A Statistics(3A 统计)
======================

The C3 ISP can collect different statistics over an input Bayer frame.
Those statistics are obtained from the "c3-isp-stats" metadata capture video nodes,
using the :c:type:`v4l2_meta_format` interface.
They are formatted as described by the :c:type:`c3_isp_stats_info` structure.
(C3 ISP 可以在输入的 Bayer 帧上收集不同的统计信息。这些统计信息通过
:c:type:`v4l2_meta_format` 接口从 "c3-isp-stats" 元数据捕获视频节点
获取。它们的格式由 :c:type:`c3_isp_stats_info` 结构描述。)

The statistics collected are  Auto-white balance,
Auto-exposure and Auto-focus information.
(收集的统计信息包括自动白平衡、自动曝光和自动对焦信息。)

.. c3_isp_params_cfg

Configuration Parameters(配置参数)
==================================

The configuration parameters are passed to the c3-isp-params metadata output video node,
using the :c:type:`v4l2_meta_format` interface. Rather than a single struct containing
sub-structs for each configurable area of the ISP, parameters for the C3-ISP
are defined as distinct structs or "blocks" which may be added to the data
member of :c:type:`c3_isp_params_cfg`. Userspace is responsible for
populating the data member with the blocks that need to be configured by the driver, but
need not populate it with **all** the blocks, or indeed with any at all if there
are no configuration changes to make. Populated blocks **must** be consecutive
in the buffer. To assist both userspace and the driver in identifying the
blocks each block-specific struct embeds
:c:type:`c3_isp_params_block_header` as its first member and userspace
must populate the type member with a value from
:c:type:`c3_isp_params_block_type`. Once the blocks have been populated
into the data buffer, the combined size of all populated blocks shall be set in
the data_size member of :c:type:`c3_isp_params_cfg`. For example:
(配置参数通过 :c:type:`v4l2_meta_format` 接口传递给 c3-isp-params
元数据输出视频节点。C3-ISP 的参数被定义为各自独立的结构体或"块"
(block)，可以添加到 :c:type:`c3_isp_params_cfg` 的 data 成员中，而
不是用一个为 ISP 每个可配置区域包含子结构的单一结构体。用户空间负责
用需要由驱动程序配置的块来填充 data 成员，但不必填充 **所有** 的
块——如果没有要做的配置更改，甚至可以一个都不填。已填充的块在缓冲区
中 **必须** 连续。为了帮助用户空间和驱动程序识别各个块，每个块特定的
结构都把 :c:type:`c3_isp_params_block_header` 作为其第一个成员，用户
空间必须用来自 :c:type:`c3_isp_params_block_type` 的值填充其 type
成员。把这些块填充到数据缓冲区之后，所有已填充块的总大小应设置在
:c:type:`c3_isp_params_cfg` 的 data_size 成员中。例如：)

.. code-block:: c

	struct c3_isp_params_cfg *params =
		(struct c3_isp_params_cfg *)buffer;

	params->version = C3_ISP_PARAM_BUFFER_V0;
	params->data_size = 0;

	void *data = (void *)params->data;

	struct c3_isp_params_awb_gains *gains =
		(struct c3_isp_params_awb_gains *)data;

	gains->header.type = C3_ISP_PARAMS_BLOCK_AWB_GAINS;
	gains->header.flags = C3_ISP_PARAMS_BLOCK_FL_ENABLE;
	gains->header.size = sizeof(struct c3_isp_params_awb_gains);

	gains->gr_gain = 256;
	gains->r_gain = 256;
	gains->b_gain = 256;
	gains->gb_gain = 256;

	data += sizeof(struct c3_isp__params_awb_gains);
	params->data_size += sizeof(struct c3_isp_params_awb_gains);

	struct c3_isp_params_awb_config *awb_cfg =
		(struct c3_isp_params_awb_config *)data;

	awb_cfg->header.type = C3_ISP_PARAMS_BLOCK_AWB_CONFIG;
	awb_cfg->header.flags = C3_ISP_PARAMS_BLOCK_FL_ENABLE;
	awb_cfg->header.size = sizeof(struct c3_isp_params_awb_config);

	awb_cfg->tap_point = C3_ISP_AWB_STATS_TAP_BEFORE_WB;
	awb_cfg->satur = 1;
	awb_cfg->horiz_zones_num = 32;
	awb_cfg->vert_zones_num = 24;

	params->data_size += sizeof(struct c3_isp_params_awb_config);

Amlogic C3 ISP uAPI data types(Amlogic C3 ISP uAPI 数据类型)
============================================================

.. kernel-doc:: include/uapi/linux/media/amlogic/c3-isp-config.h
