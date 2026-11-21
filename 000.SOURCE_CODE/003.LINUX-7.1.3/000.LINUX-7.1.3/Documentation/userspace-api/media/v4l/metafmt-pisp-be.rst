.. SPDX-License-Identifier: GPL-2.0
.. c:namespace:: V4L

.. _v4l2-meta-fmt-rpi-be-cfg:

*********************************************
V4L2_META_FMT_RPI_BE_CFG(PiSP 后端配置元格式)
*********************************************

Raspberry Pi PiSP Back End configuration format(树莓派 PiSP 后端配置格式)
=========================================================================

The Raspberry Pi PiSP Back End memory-to-memory image signal processor is
configured by userspace by providing a buffer of configuration parameters
to the `pispbe-config` output video device node using the
:c:type:`v4l2_meta_format` interface.
(树莓派 PiSP 后端 (Back End) 内存到内存图像信号处理器由用户态配置：使用
:c:type:`v4l2_meta_format` 接口，向 `pispbe-config` 输出视频设备节点提供
一个包含配置参数的缓冲区。)

The PiSP Back End processes images in tiles, and its configuration requires
specifying two different sets of parameters by populating the members of
:c:type:`pisp_be_tiles_config` defined in the ``pisp_be_config.h`` header file.
(PiSP 后端按瓦片 (tile) 处理图像，其配置需要通过填充 ``pisp_be_config.h``
头文件中定义的 :c:type:`pisp_be_tiles_config` 的成员，来指定两组不同的
参数。)

The `Raspberry Pi PiSP technical specification
<https://datasheets.raspberrypi.com/camera/raspberry-pi-image-signal-processor-specification.pdf>`_
provide detailed description of the ISP back end configuration and programming
model.
(`Raspberry Pi PiSP technical specification
<https://datasheets.raspberrypi.com/camera/raspberry-pi-image-signal-processor-specification.pdf>`_
对 ISP 后端的配置与编程模型有详细描述。)

Global configuration data(全局配置数据)
---------------------------------------

The global configuration data describe how the pixels in a particular image are
to be processed and is therefore shared across all the tiles of the image. So
for example, LSC (Lens Shading Correction) or Denoise parameters would be common
across all tiles from the same frame.
(全局配置数据描述特定图像中的像素将如何被处理，因此在图像的所有瓦片之间
共享。例如，LSC (镜头阴影校正) 或降噪参数对来自同一帧的所有瓦片都是通用
的。)

Global configuration data are passed to the ISP by populating the member of
:c:type:`pisp_be_config`.
(全局配置数据通过填充 :c:type:`pisp_be_config` 的成员传递给 ISP。)

Tile parameters(瓦片参数)
-------------------------

As the ISP processes images in tiles, each set of tiles parameters describe how
a single tile in an image is going to be processed. A single set of tile
parameters consist of 160 bytes of data and to process a batch of tiles several
sets of tiles parameters are required.
(由于 ISP 按瓦片处理图像，每组瓦片参数描述图像中单个瓦片将如何被处理。一组
瓦片参数由 160 字节的数据组成，要处理一批瓦片需要多组瓦片参数。)

Tiles parameters are passed to the ISP by populating the member of
``pisp_tile`` and the ``num_tiles`` fields of :c:type:`pisp_be_tiles_config`.
(瓦片参数通过填充 :c:type:`pisp_be_tiles_config` 的 ``pisp_tile`` 成员和
``num_tiles`` 字段传递给 ISP。)

Raspberry Pi PiSP Back End uAPI data types(树莓派 PiSP 后端 uAPI 数据类型)
==========================================================================

This section describes the data types exposed to userspace by the Raspberry Pi
PiSP Back End. The section is informative only, for a detailed description of
each field refer to the `Raspberry Pi PiSP technical specification
<https://datasheets.raspberrypi.com/camera/raspberry-pi-image-signal-processor-specification.pdf>`_.
(本节描述树莓派 PiSP 后端向用户态暴露的数据类型。本节仅供参考，各字段的
详细描述请参阅 `Raspberry Pi PiSP technical specification
<https://datasheets.raspberrypi.com/camera/raspberry-pi-image-signal-processor-specification.pdf>`_ 。)

.. kernel-doc:: include/uapi/linux/media/raspberrypi/pisp_be_config.h
