.. SPDX-License-Identifier: GPL-2.0-only

.. include:: <isonum.txt>

.. _media-ccs-uapi:

======================================================
MIPI CCS camera sensor driver(MIPI CCS 相机传感器驱动)
======================================================

The MIPI CCS camera sensor driver is a generic driver for `MIPI CCS
<https://www.mipi.org/specifications/camera-command-set>`_ compliant
camera sensors. It exposes three sub-devices representing the pixel array,
the binner and the scaler.
(MIPI CCS 相机传感器驱动是 `MIPI CCS <https://www.mipi.org/specifications/camera-command-set>`_ 兼容相机传感器的通用驱动。它暴露三个子设备，分别表示像素阵列、合并器（binner）和缩放器（scaler）。)

As the capabilities of individual devices vary, the driver exposes
interfaces based on the capabilities that exist in hardware.
(由于各个设备的能力不同，驱动程序会根据硬件中存在的能力来暴露接口。)

Also see :ref:`the CCS driver kernel documentation <media-ccs-driver>`.
(另参见 :ref:`the CCS driver kernel documentation <media-ccs-driver>` （CCS 驱动程序内核文档）。)

Pixel Array sub-device(像素阵列子设备)
--------------------------------------

The pixel array sub-device represents the camera sensor's pixel matrix, as well
as analogue crop functionality present in many compliant devices. The analogue
crop is configured using the ``V4L2_SEL_TGT_CROP`` on the source pad (0) of the
entity. The size of the pixel matrix can be obtained by getting the
``V4L2_SEL_TGT_NATIVE_SIZE`` target.
(像素阵列子设备表示相机传感器的像素矩阵，以及许多兼容设备中存在的模拟裁剪功能。模拟裁剪使用实体源 pad（0）上的 ``V4L2_SEL_TGT_CROP`` 进行配置。可以通过获取 ``V4L2_SEL_TGT_NATIVE_SIZE`` 目标来得到像素矩阵的大小。)

Binner(合并器)
--------------

The binner sub-device represents the binning functionality on the sensor. For
that purpose, selection target ``V4L2_SEL_TGT_COMPOSE`` is supported on the
sink pad (0).
(合并器子设备表示传感器上的合并（binning）功能。为此，在接收 pad（0）上支持选择目标 ``V4L2_SEL_TGT_COMPOSE`` 。)

Additionally, if a device has no scaler or digital crop functionality, the
source pad (1) exposes another digital crop selection rectangle that can only
crop at the end of the lines and frames.
(此外，如果设备没有缩放器或数字裁剪功能，源 pad（1）会暴露另一个数字裁剪选择矩形，它只能在行和帧的末尾进行裁剪。)

Scaler(缩放器)
--------------

The scaler sub-device represents the digital crop and scaling functionality of
the sensor. The V4L2 selection target ``V4L2_SEL_TGT_CROP`` is used to
configure the digital crop on the sink pad (0) when digital crop is supported.
Scaling is configured using selection target ``V4L2_SEL_TGT_COMPOSE`` on the
sink pad (0) as well.
(缩放器子设备表示传感器的数字裁剪和缩放功能。当支持数字裁剪时，V4L2 选择目标 ``V4L2_SEL_TGT_CROP`` 用于在接收 pad（0）上配置数字裁剪。缩放同样使用接收 pad（0）上的选择目标 ``V4L2_SEL_TGT_COMPOSE`` 配置。)

Additionally, if the scaler sub-device exists, its source pad (1) exposes
another digital crop selection rectangle that can only crop at the end of the
lines and frames.
(此外，如果缩放器子设备存在，其源 pad（1）会暴露另一个数字裁剪选择矩形，它只能在行和帧的末尾进行裁剪。)

Digital and analogue crop(数字与模拟裁剪)
-----------------------------------------

Digital crop functionality is referred to as cropping that effectively works by
dropping some data on the floor. Analogue crop, on the other hand, means that
the cropped information is never retrieved. In case of camera sensors, the
analogue data is never read from the pixel matrix that are outside the
configured selection rectangle that designates crop. The difference has an
effect in device timing and likely also in power consumption.
(数字裁剪功能是指通过把某些数据丢弃来生效的裁剪。而模拟裁剪则意味着被裁剪的信息永远不会被读取。对于相机传感器，位于指定裁剪的选择矩形之外的模拟数据永远不会从像素矩阵中读取。这种差异会影响设备时序，很可能也影响功耗。)

Private controls(私有控件)
--------------------------

The MIPI CCS driver implements a number of private controls under
``V4L2_CID_USER_BASE_CCS`` to control the MIPI CCS compliant camera sensors.
(MIPI CCS 驱动程序在 ``V4L2_CID_USER_BASE_CCS`` 下实现了一些私有控件，用于控制 MIPI CCS 兼容的相机传感器。)

Analogue gain model(模拟增益模型)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The CCS defines an analogue gain model where the gain can be calculated using
the following formula:
(CCS 定义了一个模拟增益模型，其中增益可以使用以下公式计算：)

	gain = m0 * x + c0 / (m1 * x + c1)

Either m0 or c0 will be zero. The constants that are device specific, can be
obtained from the following controls:
(m0 或 c0 之一将为零。这些设备特定的常量可以从以下控件获得：)

	V4L2_CID_CCS_ANALOGUE_GAIN_M0
	V4L2_CID_CCS_ANALOGUE_GAIN_M1
	V4L2_CID_CCS_ANALOGUE_GAIN_C0
	V4L2_CID_CCS_ANALOGUE_GAIN_C1

The analogue gain (``x`` in the formula) is controlled through
``V4L2_CID_ANALOGUE_GAIN`` in this case.
(此情况下，模拟增益（公式中的 ``x`` ）通过 ``V4L2_CID_ANALOGUE_GAIN`` 控制。)

Alternate analogue gain model(替代模拟增益模型)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The CCS defines another analogue gain model called alternate analogue gain. In
this case, the formula to calculate actual gain consists of linear and
exponential parts:
(CCS 定义了另一种称为替代模拟增益（alternate analogue gain）的模拟增益模型。在这种情况下，计算实际增益的公式由线性和指数部分组成：)

	gain = linear * 2 ^ exponent

The ``linear`` and ``exponent`` factors can be set using the
``V4L2_CID_CCS_ANALOGUE_LINEAR_GAIN`` and
``V4L2_CID_CCS_ANALOGUE_EXPONENTIAL_GAIN`` controls, respectively
(``linear`` 和 ``exponent`` 因子可以分别使用 ``V4L2_CID_CCS_ANALOGUE_LINEAR_GAIN`` 和 ``V4L2_CID_CCS_ANALOGUE_EXPONENTIAL_GAIN`` 控件设置。)

Shading correction(阴影校正)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The CCS standard supports lens shading correction. The feature can be controlled
using ``V4L2_CID_CCS_SHADING_CORRECTION``. Additionally, the luminance
correction level may be changed using
``V4L2_CID_CCS_LUMINANCE_CORRECTION_LEVEL``, where value 0 indicates no
correction and 128 indicates correcting the luminance in corners to 10 % less
than in the centre.
(CCS 标准支持镜头阴影校正。可以使用 ``V4L2_CID_CCS_SHADING_CORRECTION`` 控制该特性。此外，可以使用 ``V4L2_CID_CCS_LUMINANCE_CORRECTION_LEVEL`` 更改亮度校正级别，值 0 表示不校正，128 表示把角落的亮度校正到比中心低 10%。)

Shading correction needs to be enabled for luminance correction level to have an
effect.
(需要启用阴影校正，亮度校正级别才能生效。)

**Copyright** |copy| 2020 Intel Corporation
