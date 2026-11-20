.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_PARM:

************************************************************************
ioctl VIDIOC_G_PARM, VIDIOC_S_PARM(ioctl VIDIOC_G_PARM 与 VIDIOC_S_PARM)
************************************************************************

Name(名称)
==========

VIDIOC_G_PARM - VIDIOC_S_PARM - Get or set streaming parameters
(VIDIOC_G_PARM - VIDIOC_S_PARM - 获取或设置流参数)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_PARM

``int ioctl(int fd, VIDIOC_G_PARM, v4l2_streamparm *argp)``

.. c:macro:: VIDIOC_S_PARM

``int ioctl(int fd, VIDIOC_S_PARM, v4l2_streamparm *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_streamparm`.
    (指向 struct :c:type:`v4l2_streamparm` 的指针。)

Description(描述)
=================

Applications can request a different frame interval. The capture or
output device will be reconfigured to support the requested frame
interval if possible. Optionally drivers may choose to skip or
repeat frames to achieve the requested frame interval.
(应用程序可以请求不同的帧间隔。如果可能，捕获或输出设备会被重新配置以支持所
请求的帧间隔。驱动程序也可以选择跳过或重复帧来达到所请求的帧间隔。)

For stateful encoders (see :ref:`encoder`) this represents the
frame interval that is typically embedded in the encoded video stream.
(对有状态编码器(见 :ref:`encoder`)而言，这表示通常嵌入编码视频流中的帧间隔。)

Changing the frame interval shall never change the format. Changing the
format, on the other hand, may change the frame interval.
(改变帧间隔绝不应改变格式。反过来，改变格式则可能改变帧间隔。)

Further these ioctls can be used to determine the number of buffers used
internally by a driver in read/write mode. For implications see the
section discussing the :c:func:`read()` function.
(此外，这些 ioctl 还可用于确定驱动程序在读写模式下内部使用的缓冲区数量。相关
影响见讨论 :c:func:`read()` 函数的章节。)

To get and set the streaming parameters applications call the
:ref:`VIDIOC_G_PARM <VIDIOC_G_PARM>` and
:ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctl, respectively. They take a
pointer to a struct :c:type:`v4l2_streamparm` which contains a
union holding separate parameters for input and output devices.
(应用程序分别调用 :ref:`VIDIOC_G_PARM <VIDIOC_G_PARM>` 与
:ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctl 来获取和设置流参数。它们接受一个
指向 struct :c:type:`v4l2_streamparm` 的指针，该结构体包含一个联合体，为输入
与输出设备分别存放各自的参数。)

.. tabularcolumns:: |p{3.7cm}|p{3.5cm}|p{10.1cm}|

.. c:type:: v4l2_streamparm

.. flat-table:: struct v4l2_streamparm
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - The buffer (stream) type, same as struct
	:c:type:`v4l2_format` ``type``, set by the
	application. See :c:type:`v4l2_buf_type`.
	(缓冲区(流)类型，与 struct :c:type:`v4l2_format` 的 ``type`` 相同，
	由应用程序设置。见 :c:type:`v4l2_buf_type`。)
    * - union {
      - ``parm``
    * - struct :c:type:`v4l2_captureparm`
      - ``capture``
      - Parameters for capture devices, used when ``type`` is
	``V4L2_BUF_TYPE_VIDEO_CAPTURE`` or
	``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``.
	(捕获设备的参数，在 ``type`` 为 ``V4L2_BUF_TYPE_VIDEO_CAPTURE`` 或
	``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` 时使用。)
    * - struct :c:type:`v4l2_outputparm`
      - ``output``
      - Parameters for output devices, used when ``type`` is
	``V4L2_BUF_TYPE_VIDEO_OUTPUT`` or ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``.
	(输出设备的参数，在 ``type`` 为 ``V4L2_BUF_TYPE_VIDEO_OUTPUT`` 或
	``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE`` 时使用。)
    * - __u8
      - ``raw_data``\ [200]
      - A place holder for future extensions.
	(为未来扩展预留的占位符。)
    * - }


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_captureparm

.. flat-table:: struct v4l2_captureparm
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``capability``
      - See :ref:`parm-caps`.
	(见 :ref:`parm-caps` 一节。)
    * - __u32
      - ``capturemode``
      - Set by drivers and applications, see :ref:`parm-flags`.
	(由驱动程序和应用程序设置，见 :ref:`parm-flags` 一节。)
    * - struct :c:type:`v4l2_fract`
      - ``timeperframe``
      - This is the desired period between successive frames captured by
	the driver, in seconds.
	(这是驱动程序捕获相邻两帧之间所期望的间隔时间，以秒为单位。)
    * - :cspan:`2`

	This will configure the speed at which the video source (e.g. a sensor)
	generates video frames. If the speed is fixed, then the driver may
	choose to skip or repeat frames in order to achieve the requested
	frame rate.
	(这将配置视频源(例如传感器)生成视频帧的速度。如果该速度是固定的，驱动
	程序可以选择跳过或重复帧，以达到所请求的帧率。)

	For stateful encoders (see :ref:`encoder`) this represents the
	frame interval that is typically embedded in the encoded video stream.
	(对有状态编码器(见 :ref:`encoder`)而言，这表示通常嵌入编码视频流中的
	帧间隔。)

	Applications store here the desired frame period, drivers return
	the actual frame period.
	(应用程序在此存放所期望的帧周期，驱动程序返回实际的帧周期。)

	Changing the video standard (also implicitly by switching
	the video input) may reset this parameter to the nominal frame
	period. To reset manually applications can just set this field to
	zero.
	(改变视频制式(包括切换视频输入时隐式发生的改变)可能把该参数重置为标称
	帧周期。要手动重置，应用程序只需把该字段设为零。)

	Drivers support this function only when they set the
	``V4L2_CAP_TIMEPERFRAME`` flag in the ``capability`` field.
	(仅当驱动程序在 ``capability`` 字段中设置了 ``V4L2_CAP_TIMEPERFRAME``
	标志时，才支持此功能。)
    * - __u32
      - ``extendedmode``
      - Custom (driver specific) streaming parameters. When unused,
	applications and drivers must set this field to zero. Applications
	using this field should check the driver name and version, see
	:ref:`querycap`.
	(自定义的(驱动程序特有的)流参数。不使用时，应用程序和驱动程序必须把该
	字段设为零。使用该字段的应用程序应核对驱动程序的名称和版本，见
	:ref:`querycap`。)
    * - __u32
      - ``readbuffers``
      - Applications set this field to the desired number of buffers used
	internally by the driver in :c:func:`read()` mode.
	Drivers return the actual number of buffers. When an application
	requests zero buffers, drivers should just return the current
	setting rather than the minimum or an error code. For details see
	:ref:`rw`.
	(应用程序把该字段设置为 :c:func:`read()` 模式下驱动程序内部使用的缓冲区
	的期望数量。驱动程序返回实际的缓冲区数量。当应用程序请求零个缓冲区时，
	驱动程序应直接返回当前设置，而不是最小值或错误码。详情见 :ref:`rw`。)
    * - __u32
      - ``reserved``\ [4]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
	(为未来扩展保留。驱动程序和应用程序必须把该数组置零。)


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_outputparm

.. flat-table:: struct v4l2_outputparm
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``capability``
      - See :ref:`parm-caps`.
	(见 :ref:`parm-caps` 一节。)
    * - __u32
      - ``outputmode``
      - Set by drivers and applications, see :ref:`parm-flags`.
	(由驱动程序和应用程序设置，见 :ref:`parm-flags` 一节。)
    * - struct :c:type:`v4l2_fract`
      - ``timeperframe``
      - This is the desired period between successive frames output by the
	driver, in seconds.
	(这是驱动程序输出相邻两帧之间所期望的间隔时间，以秒为单位。)
    * - :cspan:`2`

	The field is intended to repeat frames on the driver side in
	:c:func:`write()` mode (in streaming mode timestamps
	can be used to throttle the output), saving I/O bandwidth.
	(该字段用于让驱动程序在 :c:func:`write()` 模式下重复帧(在流模式下可以
	用时间戳来节流输出)，从而节省 I/O 带宽。)

	For stateful encoders (see :ref:`encoder`) this represents the
	frame interval that is typically embedded in the encoded video stream
	and it provides a hint to the encoder of the speed at which raw
	frames are queued up to the encoder.
	(对有状态编码器(见 :ref:`encoder`)而言，这表示通常嵌入编码视频流中的帧
	间隔，并向编码器提示原始帧送入编码器的速度。)

	Applications store here the desired frame period, drivers return
	the actual frame period.
	(应用程序在此存放所期望的帧周期，驱动程序返回实际的帧周期。)

	Changing the video standard (also implicitly by switching
	the video output) may reset this parameter to the nominal frame
	period. To reset manually applications can just set this field to
	zero.
	(改变视频制式(包括切换视频输出时隐式发生的改变)可能把该参数重置为标称
	帧周期。要手动重置，应用程序只需把该字段设为零。)

	Drivers support this function only when they set the
	``V4L2_CAP_TIMEPERFRAME`` flag in the ``capability`` field.
	(仅当驱动程序在 ``capability`` 字段中设置了 ``V4L2_CAP_TIMEPERFRAME``
	标志时，才支持此功能。)
    * - __u32
      - ``extendedmode``
      - Custom (driver specific) streaming parameters. When unused,
	applications and drivers must set this field to zero. Applications
	using this field should check the driver name and version, see
	:ref:`querycap`.
	(自定义的(驱动程序特有的)流参数。不使用时，应用程序和驱动程序必须把该
	字段设为零。使用该字段的应用程序应核对驱动程序的名称和版本，见
	:ref:`querycap`。)
    * - __u32
      - ``writebuffers``
      - Applications set this field to the desired number of buffers used
	internally by the driver in :c:func:`write()` mode. Drivers
	return the actual number of buffers. When an application requests
	zero buffers, drivers should just return the current setting
	rather than the minimum or an error code. For details see
	:ref:`rw`.
	(应用程序把该字段设置为 :c:func:`write()` 模式下驱动程序内部使用的缓冲
	区的期望数量。驱动程序返回实际的缓冲区数量。当应用程序请求零个缓冲区时，
	驱动程序应直接返回当前设置，而不是最小值或错误码。详情见 :ref:`rw`。)
    * - __u32
      - ``reserved``\ [4]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
	(为未来扩展保留。驱动程序和应用程序必须把该数组置零。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _parm-caps:

.. flat-table:: Streaming Parameters Capabilities
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_CAP_TIMEPERFRAME``
      - 0x1000
      - The frame period can be modified by setting the ``timeperframe``
	field.
	(可以通过设置 ``timeperframe`` 字段来修改帧周期。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _parm-flags:

.. flat-table:: Capture Parameters Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_MODE_HIGHQUALITY``
      - 0x0001
      - High quality imaging mode. High quality mode is intended for still
	imaging applications. The idea is to get the best possible image
	quality that the hardware can deliver. It is not defined how the
	driver writer may achieve that; it will depend on the hardware and
	the ingenuity of the driver writer. High quality mode is a
	different mode from the regular motion video capture modes. In
	high quality mode:
	(高质量成像模式。高质量模式面向静态成像应用。其理念是获得硬件所能提供的
	最佳图像质量。驱动程序作者如何实现这一点并未定义；这将取决于硬件以及驱动
	程序作者的巧思。高质量模式不同于常规的运动视频捕获模式。在高质量模式下：)

	-  The driver may be able to capture higher resolutions than for
	   motion capture.
	   (驱动程序可能能够以高于动态捕获的分辨率进行捕获。)

	-  The driver may support fewer pixel formats than motion capture
	   (eg; true color).
	   (驱动程序支持的像素格式可能比动态捕获更少(例如真彩色)。)

	-  The driver may capture and arithmetically combine multiple
	   successive fields or frames to remove color edge artifacts and
	   reduce the noise in the video data.
	   (驱动程序可以捕获多个连续的场或帧并进行算术合并，以消除彩色边缘伪影
	   并降低视频数据中的噪声。)

	-  The driver may capture images in slices like a scanner in order
	   to handle larger format images than would otherwise be
	   possible.
	   (驱动程序可以像扫描仪那样分片捕获图像，以处理比其他情况下更大的图像
	   格式。)

	-  An image capture operation may be significantly slower than
	   motion capture.
	   (单次图像捕获操作可能比动态捕获慢得多。)

	-  Moving objects in the image might have excessive motion blur.
	   (图像中的运动物体可能出现过度运动模糊。)

	-  Capture might only work through the :c:func:`read()` call.
	   (捕获可能只能通过 :c:func:`read()` 调用来完成。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
