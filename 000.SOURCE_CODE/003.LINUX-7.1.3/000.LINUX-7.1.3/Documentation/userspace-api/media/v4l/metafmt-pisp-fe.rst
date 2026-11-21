.. SPDX-License-Identifier: GPL-2.0
.. c:namespace:: V4L

.. _v4l2-meta-fmt-rpi-fe-cfg:

*******************************************
V4L2_META_FMT_RPI_FE_CFG(PiSP 前端配置格式)
*******************************************

Raspberry Pi PiSP Front End configuration format(Raspberry Pi PiSP 前端配置格式)
================================================================================

The Raspberry Pi PiSP Front End image signal processor is configured by
userspace by providing a buffer of configuration parameters to the
`rp1-cfe-fe-config` output video device node using the
:c:type:`v4l2_meta_format` interface.
(用户空间通过 :c:type:`v4l2_meta_format` 接口向 `rp1-cfe-fe-config` 输出视
频设备节点提供一整个缓冲区的配置参数，从而对 Raspberry Pi PiSP 前端图像信
号处理器进行配置。)

The `Raspberry Pi PiSP technical specification
<https://datasheets.raspberrypi.com/camera/raspberry-pi-image-signal-processor-specification.pdf>`_
provide detailed description of the Front End configuration and programming
model.
(《Raspberry Pi PiSP 技术规范》对前端的配置与编程模型有详细描述。)

.. _v4l2-meta-fmt-rpi-fe-stats:

*********************************************
V4L2_META_FMT_RPI_FE_STATS(PiSP 前端统计格式)
*********************************************

Raspberry Pi PiSP Front End statistics format(Raspberry Pi PiSP 前端统计格式)
=============================================================================

The Raspberry Pi PiSP Front End image signal processor provides statistics data
by writing to a buffer provided via the `rp1-cfe-fe-stats` capture video device
node using the
:c:type:`v4l2_meta_format` interface.
(Raspberry Pi PiSP 前端图像信号处理器通过 :c:type:`v4l2_meta_format` 接口，
向 `rp1-cfe-fe-stats` 采集视频设备节点所提供的缓冲区写入统计数据。)

The `Raspberry Pi PiSP technical specification
<https://datasheets.raspberrypi.com/camera/raspberry-pi-image-signal-processor-specification.pdf>`_
provide detailed description of the Front End configuration and programming
model.
(《Raspberry Pi PiSP 技术规范》对前端的配置与编程模型有详细描述。)
