.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _touch:

***********************
Touch Devices(触摸设备)
***********************

Touch devices are accessed through character device special files named
``/dev/v4l-touch0`` to ``/dev/v4l-touch255`` with major number 81 and
dynamically allocated minor numbers 0 to 255.
(触摸设备通过名为 ``/dev/v4l-touch0`` 到 ``/dev/v4l-touch255`` 的字符设备
特殊文件访问，主设备号为 81，次设备号为动态分配的 0 到 255。)

Overview(概述)
==============

Sensors may be Optical, or Projected Capacitive touch (PCT).
(传感器可以是光学的，也可以是投射式电容触摸 (PCT)。)

Processing is required to analyse the raw data and produce input events. In
some systems, this may be performed on the ASIC and the raw data is purely a
side-channel for diagnostics or tuning. In other systems, the ASIC is a simple
analogue front end device which delivers touch data at high rate, and any touch
processing must be done on the host.
(需要通过处理来分析原始数据并产生输入事件。在有些系统中，这可以在 ASIC
上完成，原始数据纯粹是用于诊断或调校的辅助通道。在另一些系统中，ASIC 只是
一个以高速率提供触摸数据的简单模拟前端器件，任何触摸处理都必须在主机上
完成。)

For capacitive touch sensing, the touchscreen is composed of an array of
horizontal and vertical conductors (alternatively called rows/columns, X/Y
lines, or tx/rx). Mutual Capacitance measured is at the nodes where the
conductors cross. Alternatively, Self Capacitance measures the signal from each
column and row independently.
(对于电容触摸感应，触摸屏由水平和垂直导体阵列构成 (也称为行/列、X/Y 线或
tx/rx)。测量的互电容位于导体交叉的节点处。另一种方式是自电容，它独立测量
每列和每行的信号。)

A touch input may be determined by comparing the raw capacitance measurement to
a no-touch reference (or "baseline") measurement:
(可以通过把原始电容测量值与无触摸参考值 (即"基线") 测量值进行比较来确定
触摸输入：)

Delta = Raw - Reference
(差值 = 原始值 - 参考值)

The reference measurement takes account of variations in the capacitance across
the touch sensor matrix, for example manufacturing irregularities,
environmental or edge effects.
(参考测量值考虑了触摸传感器矩阵中电容的变化，例如制造上的不规则性、环境
或边缘效应。)

Querying Capabilities(查询能力)
===============================

Devices supporting the touch interface set the ``V4L2_CAP_VIDEO_CAPTURE`` flag
and the ``V4L2_CAP_TOUCH`` flag in the ``capabilities`` field of
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl.
(支持触摸接口的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的
:c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置
``V4L2_CAP_VIDEO_CAPTURE`` 标志和 ``V4L2_CAP_TOUCH`` 标志。)

At least one of the read/write or streaming I/O methods must be
supported.
(读/写或流式 I/O 方法中至少一种必须受支持。)

The formats supported by touch devices are documented in
:ref:`Touch Formats <tch-formats>`.
(触摸设备支持的格式在 :ref:`Touch Formats <tch-formats>` 中说明。)

Data Format Negotiation(数据格式协商)
=====================================

A touch device may support any I/O method.
(触摸设备可以支持任何 I/O 方法。)
