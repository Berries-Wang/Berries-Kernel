.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _common:

##################################
Common API Elements(通用 API 元素)
##################################
Programming a V4L2 device consists of these steps:
(对 V4L2 设备编程由以下步骤组成：)

-  Opening the device
   (打开设备。)

-  Changing device properties, selecting a video and audio input, video
   standard, picture brightness a. o.
   (修改设备属性，选择视频与音频输入、视频制式、图像亮度等。)

-  Negotiating a data format
   (协商数据格式。)

-  Negotiating an input/output method
   (协商输入/输出方法。)

-  The actual input/output loop
   (实际的输入/输出循环。)

-  Closing the device
   (关闭设备。)

In practice most steps are optional and can be executed out of order. It
depends on the V4L2 device type, you can read about the details in
:ref:`devices`. In this chapter we will discuss the basic concepts
applicable to all devices.
(实践中大多数步骤都是可选的，可以乱序执行。这取决于 V4L2 设备类型，具体
细节可参阅 :ref:`devices` 。在本章中，我们将讨论适用于所有设备的基本
概念。)


.. toctree::
    :maxdepth: 1

    open
    querycap
    app-pri
    video
    audio
    tuner
    standard
    dv-timings
    control
    extended-controls
    ext-ctrls-camera
    ext-ctrls-flash
    ext-ctrls-image-source
    ext-ctrls-image-process
    ext-ctrls-codec
    ext-ctrls-codec-stateless
    ext-ctrls-jpeg
    ext-ctrls-dv
    ext-ctrls-rf-tuner
    ext-ctrls-fm-tx
    ext-ctrls-fm-rx
    ext-ctrls-detect
    ext-ctrls-colorimetry
    fourcc
    format
    planar-apis
    selection-api
    crop
    streaming-par
