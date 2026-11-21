.. SPDX-License-Identifier: GPL-2.0
.. c:namespace:: V4L

.. _v4l2-meta-fmt-rk-isp1-stat-3a:

*******************************************************************************************************************************************
V4L2_META_FMT_RK_ISP1_PARAMS ('rk1p'), V4L2_META_FMT_RK_ISP1_STAT_3A ('rk1s'), V4L2_META_FMT_RK_ISP1_EXT_PARAMS ('rk1e')(RKISP1 元数据格式)
*******************************************************************************************************************************************

==================================
Configuration parameters(配置参数)
==================================

The configuration of the RkISP1 ISP is performed by userspace by providing
parameters for the ISP to the driver using the :c:type:`v4l2_meta_format`
interface.
(RkISP1 ISP 的配置由用户空间完成：用户空间使用 :c:type:`v4l2_meta_format`
接口向驱动程序提供 ISP 参数。)

There are two methods that allow to configure the ISP, the `fixed parameters`
configuration format and the `extensible parameters` configuration
format.
(有两种方法可以配置 ISP:`fixed parameters`(固定参数)配置格式和
`extensible parameters`(可扩展参数)配置格式。)

.. _v4l2-meta-fmt-rk-isp1-params:

Fixed parameters configuration format(固定参数配置格式)
=======================================================

When using the fixed configuration format, parameters are passed to the
:ref:`rkisp1_params <rkisp1_params>` metadata output video node, using
the `V4L2_META_FMT_RK_ISP1_PARAMS` meta format.
(使用固定配置格式时，参数通过 `V4L2_META_FMT_RK_ISP1_PARAMS` 元格式
传递给 :ref:`rkisp1_params <rkisp1_params>` 元数据输出视频节点。)

The buffer contains a single instance of the C structure
:c:type:`rkisp1_params_cfg` defined in ``rkisp1-config.h``. So the structure can
be obtained from the buffer by:
(缓冲区包含 ``rkisp1-config.h`` 中定义的 C 结构 :c:type:`rkisp1_params_cfg`
的单个实例。因此可以通过以下方式从缓冲区取得该结构：)

.. code-block:: c

	struct rkisp1_params_cfg *params = (struct rkisp1_params_cfg*) buffer;

This method supports a subset of the ISP features only, new applications should
use the extensible parameters method.
(此方法只支持 ISP 功能的一个子集，新应用程序应使用可扩展参数方法。)

.. _v4l2-meta-fmt-rk-isp1-ext-params:

Extensible parameters configuration format(可扩展参数配置格式)
==============================================================

When using the extensible configuration format, parameters are passed to the
:ref:`rkisp1_params <rkisp1_params>` metadata output video node, using
the `V4L2_META_FMT_RK_ISP1_EXT_PARAMS` meta format.
(使用可扩展配置格式时，参数通过 `V4L2_META_FMT_RK_ISP1_EXT_PARAMS`
元格式传递给 :ref:`rkisp1_params <rkisp1_params>` 元数据输出视频
节点。)

The buffer contains a single instance of the C structure
:c:type:`rkisp1_ext_params_cfg` defined in ``rkisp1-config.h``. The
:c:type:`rkisp1_ext_params_cfg` structure is designed to allow userspace to
populate the data buffer with only the configuration data for the ISP blocks it
intends to configure. The extensible parameters format design allows developers
to define new block types to support new configuration parameters, and defines a
versioning scheme so that it can be extended and versioned without breaking
compatibility with existing applications.
(缓冲区包含 ``rkisp1-config.h`` 中定义的 C 结构
:c:type:`rkisp1_ext_params_cfg` 的单个实例。:c:type:`rkisp1_ext_params_cfg`
结构的设计使用户空间可以只用它打算配置的 ISP 块的配置数据来填充数据
缓冲区。可扩展参数格式的设计允许开发者定义新的块类型以支持新的配置
参数，并定义了一个版本化方案，使其可以在不破坏与现有应用程序兼容性
的前提下扩展和版本化。)

For these reasons, this configuration method is preferred over the `fixed
parameters` format alternative.
(出于这些原因，此配置方法比 `fixed parameters` 格式更受青睐。)

.. rkisp1_stat_buffer

============================================
3A and histogram statistics(3A 与直方图统计)
============================================

The ISP1 device collects different statistics over an input Bayer frame.
Those statistics are obtained from the :ref:`rkisp1_stats <rkisp1_stats>`
metadata capture video node,
using the :c:type:`v4l2_meta_format` interface. The buffer contains a single
instance of the C structure :c:type:`rkisp1_stat_buffer` defined in
``rkisp1-config.h``. So the structure can be obtained from the buffer by:
(ISP1 设备在输入的 Bayer 帧上收集不同的统计信息。这些统计信息通过
:c:type:`v4l2_meta_format` 接口从 :ref:`rkisp1_stats <rkisp1_stats>`
元数据捕获视频节点获取。缓冲区包含 ``rkisp1-config.h`` 中定义的 C
结构 :c:type:`rkisp1_stat_buffer` 的单个实例。因此可以通过以下方式
从缓冲区取得该结构：)

.. code-block:: c

	struct rkisp1_stat_buffer *stats = (struct rkisp1_stat_buffer*) buffer;

The statistics collected are Exposure, AWB (Auto-white balance), Histogram and
AF (Auto-focus). See :c:type:`rkisp1_stat_buffer` for details of the statistics.
(收集的统计信息包括曝光(Exposure)、AWB(自动白平衡)、直方图
(Histogram)和 AF(自动对焦)。统计信息的细节参见
:c:type:`rkisp1_stat_buffer`。)

The 3A statistics and configuration parameters described here are usually
consumed and produced by dedicated user space libraries that comprise the
important tuning tools using software control loop.
(这里描述的 3A 统计信息和配置参数通常由专门的用户空间库消费和生成，
这些库通过软件控制环构成了重要的调优工具。)

rkisp1 uAPI data types(rkisp1 uAPI 数据类型)
============================================

.. kernel-doc:: include/uapi/linux/rkisp1-config.h
