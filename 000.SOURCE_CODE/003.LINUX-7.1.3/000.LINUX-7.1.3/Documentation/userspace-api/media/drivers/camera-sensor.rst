.. SPDX-License-Identifier: GPL-2.0

.. _media_using_camera_sensor_drivers:

Using camera sensor drivers(使用相机传感器驱动)
===============================================

This section describes common practices for how the V4L2 sub-device interface is
used to control the camera sensor drivers.
(本节描述使用 V4L2 子设备接口控制相机传感器驱动的常见做法。)

You may also find :ref:`media_writing_camera_sensor_drivers` useful.
(:ref:`media_writing_camera_sensor_drivers` 或许也对你有用。)

Sensor internal pipeline configuration(传感器内部流水线配置)
------------------------------------------------------------

Camera sensors have an internal processing pipeline including cropping and
binning functionality. The sensor drivers belong to two distinct classes, freely
configurable and register list-based drivers, depending on how the driver
configures this functionality.
(相机传感器有一个内部处理流水线，包括裁剪(cropping)和合并(binning)
功能。根据驱动程序如何配置这些功能，传感器驱动属于两个不同的类别：
自由配置型和寄存器列表式驱动。)

Freely configurable camera sensor drivers(自由配置的相机传感器驱动)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Freely configurable camera sensor drivers expose the device's internal
processing pipeline as one or more sub-devices with different cropping and
scaling configurations. The output size of the device is the result of a series
of cropping and scaling operations from the device's pixel array's size.
(自由配置的相机传感器驱动把设备的内部处理流水线暴露为一个或多个具有
不同裁剪与缩放配置的子设备。设备的输出尺寸是从设备像素阵列尺寸出发、
经过一系列裁剪与缩放操作后的结果。)

An example of such a driver is the CCS driver.
(这类驱动的一个例子是 CCS 驱动。)

Register list-based drivers(寄存器列表式驱动)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Register list-based drivers generally, instead of able to configure the device
they control based on user requests, are limited to a number of preset
configurations that combine a number of different parameters that on hardware
level are independent. How a driver picks such configuration is based on the
format set on a source pad at the end of the device's internal pipeline.
(寄存器列表式驱动通常无法根据用户请求来配置它们所控制的设备，而是只能
在若干预设配置中选择；这些预设配置把许多在硬件层面相互独立的参数捆绑
在一起。驱动如何挑选这样的配置，取决于设备内部流水线末端源 pad 上
设置的格式。)

Most sensor drivers are implemented this way.
(大多数传感器驱动都以这种方式实现。)

Frame interval configuration(帧间隔配置)
----------------------------------------

There are two different methods for obtaining possibilities for different frame
intervals as well as configuring the frame interval. Which one to implement
depends on the type of the device.
(获取不同帧间隔的可能性以及配置帧间隔有两种不同的方法。实现哪一种
取决于设备的类型。)

Raw camera sensors(RAW 相机传感器)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Instead of a high level parameter such as frame interval, the frame interval is
a result of the configuration of a number of camera sensor implementation
specific parameters. Luckily, these parameters tend to be the same for more or
less all modern raw camera sensors.
(这里并不直接暴露帧间隔这样的高层参数；帧间隔是由若干相机传感器实现
特定参数的配置推导出来的。幸运的是，这些参数对几乎所有现代 RAW 相机
传感器而言都大同小异。)

The frame interval is calculated using the following equation::

	frame interval = (analogue crop width + horizontal blanking) *
			 (analogue crop height + vertical blanking) / pixel rate

(帧间隔使用以下公式计算：)

The formula is bus independent and is applicable for raw timing parameters on
large variety of devices beyond camera sensors. Devices that have no analogue
crop, use the full source image size, i.e. pixel array size.
(该公式与总线无关，适用于相机传感器之外的大量设备的 RAW 时序参数。
没有模拟裁剪(analogue crop)的设备使用完整的源图像尺寸，即像素阵列
尺寸。)

Horizontal and vertical blanking are specified by ``V4L2_CID_HBLANK`` and
``V4L2_CID_VBLANK``, respectively. The unit of the ``V4L2_CID_HBLANK`` control
is pixels and the unit of the ``V4L2_CID_VBLANK`` is lines. The pixel rate in
the sensor's **pixel array** is specified by ``V4L2_CID_PIXEL_RATE`` in the same
sub-device. The unit of that control is pixels per second.
(水平与垂直消隐分别由 ``V4L2_CID_HBLANK`` 和 ``V4L2_CID_VBLANK`` 指定。
``V4L2_CID_HBLANK`` 控件的单位是像素，``V4L2_CID_VBLANK`` 的单位是行。
传感器 **pixel array** (像素阵列)中的像素速率由同一子设备的
``V4L2_CID_PIXEL_RATE`` 指定。该控件的单位是每秒像素数。)

Register list-based drivers need to implement read-only sub-device nodes for the
purpose. Devices that are not register list based need these to configure the
device's internal processing pipeline.
(为此，寄存器列表式驱动需要实现只读的子设备节点。非寄存器列表式的
设备则需要这些节点来配置设备的内部处理流水线。)

The first entity in the linear pipeline is the pixel array. The pixel array may
be followed by other entities that are there to allow configuring binning,
skipping, scaling or digital crop, see :ref:`VIDIOC_SUBDEV_G_SELECTION
<VIDIOC_SUBDEV_G_SELECTION>`.
(线性流水线中的第一个实体是像素阵列。像素阵列之后可以有其他实体，用于
配置合并(binning)、跳采(skipping)、缩放(scaling)或数字裁剪，参见
:ref:`VIDIOC_SUBDEV_G_SELECTION <VIDIOC_SUBDEV_G_SELECTION>`。)

USB cameras etc. devices(USB 摄像头等设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

USB video class hardware, as well as many cameras offering a similar higher
level interface natively, generally use the concept of frame interval (or frame
rate) on device level in firmware or hardware. This means lower level controls
implemented by raw cameras may not be used on uAPI (or even kAPI) to control the
frame interval on these devices.
(USB 视频类(UVC)硬件，以及许多本身原生提供类似更高层接口的相机，
通常在固件或硬件层面于设备级使用帧间隔(或帧率)的概念。这意味着 RAW
相机实现的更底层控件不能在 uAPI(甚至 kAPI)上用于控制这些设备的
帧间隔。)

Rotation, orientation and flipping(旋转、朝向与翻转)
----------------------------------------------------

Some systems have the camera sensor mounted upside down compared to its natural
mounting rotation. In such cases, drivers shall expose the information to
userspace with the :ref:`V4L2_CID_CAMERA_SENSOR_ROTATION
<v4l2-camera-sensor-rotation>` control.
(某些系统的相机传感器相对于其自然安装旋转方向是倒置安装的。在这种
情况下，驱动程序应通过 :ref:`V4L2_CID_CAMERA_SENSOR_ROTATION
<v4l2-camera-sensor-rotation>` 控件把该信息暴露给用户空间。)

Sensor drivers shall also report the sensor's mounting orientation with the
:ref:`V4L2_CID_CAMERA_SENSOR_ORIENTATION <v4l2-camera-sensor-orientation>`.
(传感器驱动还应通过 :ref:`V4L2_CID_CAMERA_SENSOR_ORIENTATION
<v4l2-camera-sensor-orientation>` 报告传感器的安装朝向。)

Sensor drivers that have any vertical or horizontal flips embedded in the
register programming sequences shall initialize the :ref:`V4L2_CID_HFLIP
<v4l2-cid-hflip>` and :ref:`V4L2_CID_VFLIP <v4l2-cid-vflip>` controls with the
values programmed by the register sequences. The default values of these
controls shall be 0 (disabled). Especially these controls shall not be inverted,
independently of the sensor's mounting rotation.
(寄存器编程序列中内嵌任何垂直或水平翻转的传感器驱动，应使用寄存器
序列所编程的值来初始化 :ref:`V4L2_CID_HFLIP <v4l2-cid-hflip>` 和
:ref:`V4L2_CID_VFLIP <v4l2-cid-vflip>` 控件。这些控件的默认值应为
0(禁用)。尤其要注意，无论传感器的安装旋转方向如何，这些控件都不得
反转。)
