.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _v4l2-meta-fmt-params:
.. _v4l2-meta-fmt-stat-3a:

******************************************************************
V4L2_META_FMT_IPU3_PARAMS ('ip3p'), V4L2_META_FMT_IPU3_3A ('ip3s')
******************************************************************

.. ipu3_uapi_stats_3a

3A statistics(3A 统计)
======================

The IPU3 ImgU 3A statistics accelerators collect different statistics over
an input Bayer frame. Those statistics are obtained from the "ipu3-imgu [01] 3a
stat" metadata capture video nodes, using the :c:type:`v4l2_meta_format`
interface. They are formatted as described by the :c:type:`ipu3_uapi_stats_3a`
structure.
(IPU3 ImgU 的 3A 统计加速器在一帧输入的 Bayer 图像上收集不同的统计信
息。这些统计信息从 "ipu3-imgu [01] 3a stat" 元数据捕获视频节点获取，
使用的是 :c:type:`v4l2_meta_format` 接口。它们按
:c:type:`ipu3_uapi_stats_3a` 结构所描述的格式编排。)

The statistics collected are AWB (Auto-white balance) RGBS (Red, Green, Blue and
Saturation measure) cells, AWB filter response, AF (Auto-focus) filter response,
and AE (Auto-exposure) histogram.
(收集的统计信息包括 AWB (自动白平衡) 的 RGBS (红、绿、蓝和饱和度度量)
单元、AWB 滤波器响应、AF (自动对焦) 滤波器响应，以及 AE (自动曝光) 直
方图。)

The struct :c:type:`ipu3_uapi_4a_config` saves all configurable parameters.
(struct :c:type:`ipu3_uapi_4a_config` 保存所有可配置的参数。)

.. code-block:: c

	struct ipu3_uapi_stats_3a {
		struct ipu3_uapi_awb_raw_buffer awb_raw_buffer;
		struct ipu3_uapi_ae_raw_buffer_aligned ae_raw_buffer[IPU3_UAPI_MAX_STRIPES];
		struct ipu3_uapi_af_raw_buffer af_raw_buffer;
		struct ipu3_uapi_awb_fr_raw_buffer awb_fr_raw_buffer;
		struct ipu3_uapi_4a_config stats_4a_config;
		__u32 ae_join_buffers;
		__u8 padding[28];
		struct ipu3_uapi_stats_3a_bubble_info_per_stripe stats_3a_bubble_per_stripe;
		struct ipu3_uapi_ff_status stats_3a_status;
	};

.. ipu3_uapi_params

Pipeline parameters(管线参数)
=============================

The pipeline parameters are passed to the "ipu3-imgu [01] parameters" metadata
output video nodes, using the :c:type:`v4l2_meta_format` interface. They are
formatted as described by the :c:type:`ipu3_uapi_params` structure.
(管线参数传递给 "ipu3-imgu [01] parameters" 元数据输出视频节点，使用
的是 :c:type:`v4l2_meta_format` 接口。它们按 :c:type:`ipu3_uapi_params`
结构所描述的格式编排。)

Both 3A statistics and pipeline parameters described here are closely tied to
the underlying camera sub-system (CSS) APIs. They are usually consumed and
produced by dedicated user space libraries that comprise the important tuning
tools, thus freeing the developers from being bothered with the low level
hardware and algorithm details.
(这里描述的 3A 统计与管线参数都与底层的相机子系统 (CSS) API 紧密相关。
它们通常由包含重要调优工具的专门用户空间库来消费和生产，从而使开发者
免于为底层硬件和算法细节所困扰。)

.. code-block:: c

	struct ipu3_uapi_params {
		/* Flags which of the settings below are to be applied */
		struct ipu3_uapi_flags use;

		/* Accelerator cluster parameters */
		struct ipu3_uapi_acc_param acc_param;

		/* ISP vector address space parameters */
		struct ipu3_uapi_isp_lin_vmem_params lin_vmem_params;
		struct ipu3_uapi_isp_tnr3_vmem_params tnr3_vmem_params;
		struct ipu3_uapi_isp_xnr3_vmem_params xnr3_vmem_params;

		/* ISP data memory (DMEM) parameters */
		struct ipu3_uapi_isp_tnr3_params tnr3_dmem_params;
		struct ipu3_uapi_isp_xnr3_params xnr3_dmem_params;

		/* Optical black level compensation */
		struct ipu3_uapi_obgrid_param obgrid_param;
	};

Intel IPU3 ImgU uAPI data types(Intel IPU3 ImgU uAPI 数据类型)
==============================================================

.. kernel-doc:: drivers/staging/media/ipu3/include/uapi/intel-ipu3.h
