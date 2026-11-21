.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _image-process-controls:

*************************************************
Image Process Control Reference(图像处理控件参考)
*************************************************

The Image Process control class is intended for low-level control of
image processing functions. Unlike ``V4L2_CID_IMAGE_SOURCE_CLASS``, the
controls in this class affect processing the image, and do not control
capturing of it.
(图像处理 (Image Process) 控件类用于对图像处理功能进行低层控制。与
``V4L2_CID_IMAGE_SOURCE_CLASS`` 不同，此类中的控件影响的是对图像的处理，
而不控制图像的采集。)


.. _image-process-control-id:

Image Process Control IDs(图像处理控件 ID)
==========================================

``V4L2_CID_IMAGE_PROC_CLASS (class)``
    The IMAGE_PROC class descriptor.
    (IMAGE_PROC 类的类描述符。)

.. _v4l2-cid-link-freq:

``V4L2_CID_LINK_FREQ (integer menu)``
    The frequency of the data bus (e.g. parallel or CSI-2).
    (数据总线 (例如并口或 CSI-2) 的频率。)

.. _v4l2-cid-pixel-rate:

``V4L2_CID_PIXEL_RATE (64-bit integer)``
    Pixel sampling rate in the device's pixel array. This control is
    read-only and its unit is pixels / second.
    (设备像素阵列中的像素采样率。此控件为只读，单位是像素/秒。)

    Some devices use horizontal and vertical blanking to configure the frame
    rate. The frame rate can be calculated from the pixel rate, analogue crop
    rectangle as well as horizontal and vertical blanking. The pixel rate
    control may be present in a different sub-device than the blanking controls
    and the analogue crop rectangle configuration.
    (有些设备使用水平和垂直消隐来配置帧率。帧率可以由像素率、模拟裁剪矩形
    以及水平和垂直消隐计算得出。像素率控件可能与消隐控件及模拟裁剪矩形配置
    位于不同的子设备上。)

    The configuration of the frame rate is performed by selecting the desired
    horizontal and vertical blanking. The unit of this control is Hz.
    (帧率的配置通过选择期望的水平和垂直消隐来完成。此控件的单位是 Hz。)

``V4L2_CID_TEST_PATTERN (menu)``
    Some capture/display/sensor devices have the capability to generate
    test pattern images. These hardware specific test patterns can be
    used to test if a device is working properly.
    (有些采集/显示/传感器设备具备生成测试图案图像的能力。这些硬件特定的
    测试图案可用于测试设备是否工作正常。)

``V4L2_CID_DEINTERLACING_MODE (menu)``
    The video deinterlacing mode (such as Bob, Weave, ...). The menu items are
    driver specific and are documented in :ref:`uapi-v4l-drivers`.
    (视频去隔行模式 (如 Bob、Weave 等)。菜单项由驱动决定，在
    :ref:`uapi-v4l-drivers` 中有说明。)

``V4L2_CID_DIGITAL_GAIN (integer)``
    Digital gain is the value by which all colour components
    are multiplied by. Typically the digital gain applied is the
    control value divided by e.g. 0x100, meaning that to get no
    digital gain the control value needs to be 0x100. The no-gain
    configuration is also typically the default.
    (数字增益是所有颜色分量相乘所用的值。通常所施加的数字增益是控件值除以
    例如 0x100，也就是说要得到无数字增益，控件值需要为 0x100。无增益配置
    通常也是默认值。)
