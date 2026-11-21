.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _osd:

************************************************
Video Output Overlay Interface(视频输出覆盖接口)
************************************************

**Also known as On-Screen Display (OSD)**
(也称为屏幕显示 (On-Screen Display，OSD)。)

Some video output devices can overlay a framebuffer image onto the
outgoing video signal. Applications can set up such an overlay using
this interface, which borrows structures and ioctls of the
:ref:`Video Overlay <overlay>` interface.
(一些视频输出设备可以把帧缓冲图像叠加到输出的视频信号上。应用程序可以使用
此接口建立这种叠加，该接口借用了 :ref:`Video Overlay <overlay>`（视频覆盖）
接口的结构体和 ioctl。)

The OSD function is accessible through the same character special file
as the :ref:`Video Output <capture>` function.
(OSD 功能可以通过与 :ref:`Video Output <capture>`（视频输出）功能相同的
字符特殊文件访问。)

.. note::

   The default function of such a ``/dev/video`` device is video
   capturing or output. The OSD function is only available after calling
   the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl.
   (这种 ``/dev/video`` 设备的默认功能是视频采集或输出。OSD 功能只有在
   调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 之后才可用。)


Querying Capabilities(查询能力)
===============================

Devices supporting the *Video Output Overlay* interface set the
``V4L2_CAP_VIDEO_OUTPUT_OVERLAY`` flag in the ``capabilities`` field of
struct :c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl.
(支持 *Video Output Overlay* 接口的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl
返回的 struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置
``V4L2_CAP_VIDEO_OUTPUT_OVERLAY`` 标志。)


Framebuffer(帧缓冲)
===================

Contrary to the *Video Overlay* interface the framebuffer is normally
implemented on the TV card and not the graphics card. On Linux it is
accessible as a framebuffer device (``/dev/fbN``). Given a V4L2 device,
applications can find the corresponding framebuffer device by calling
the :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` ioctl. It returns, amongst
other information, the physical address of the framebuffer in the
``base`` field of struct :c:type:`v4l2_framebuffer`.
The framebuffer device ioctl ``FBIOGET_FSCREENINFO`` returns the same
address in the ``smem_start`` field of struct
:c:type:`fb_fix_screeninfo`. The ``FBIOGET_FSCREENINFO``
ioctl and struct :c:type:`fb_fix_screeninfo` are defined in
the ``linux/fb.h`` header file.
(与 *Video Overlay* 接口相反，帧缓冲通常实现在电视卡上而不是显卡上。在
Linux 上，它可以作为帧缓冲设备（``/dev/fbN``）访问。给定一个 V4L2 设备，
应用程序可以通过调用 :ref:`VIDIOC_G_FBUF <VIDIOC_G_FBUF>` ioctl 找到对应
的帧缓冲设备。它除其他信息外，还会在 struct :c:type:`v4l2_framebuffer` 的
``base`` 字段中返回帧缓冲的物理地址。帧缓冲设备 ioctl
``FBIOGET_FSCREENINFO`` 在 struct :c:type:`fb_fix_screeninfo` 的
``smem_start`` 字段中返回相同的地址。``FBIOGET_FSCREENINFO`` ioctl 和
struct :c:type:`fb_fix_screeninfo` 定义在 ``linux/fb.h`` 头文件中。)

The width and height of the framebuffer depends on the current video
standard. A V4L2 driver may reject attempts to change the video standard
(or any other ioctl which would imply a framebuffer size change) with an
``EBUSY`` error code until all applications closed the framebuffer device.
(帧缓冲的宽度和高度取决于当前的视频制式。在所有应用程序都关闭帧缓冲设备
之前，V4L2 驱动可以用 ``EBUSY`` 错误码拒绝更改视频制式（或任何其他意味着
帧缓冲尺寸变化的 ioctl）的尝试。)

Example: Finding a framebuffer device for OSD(示例：为 OSD 查找帧缓冲设备)
--------------------------------------------------------------------------

.. code-block:: c

    #include <linux/fb.h>

    struct v4l2_framebuffer fbuf;
    unsigned int i;
    int fb_fd;

    if (-1 == ioctl(fd, VIDIOC_G_FBUF, &fbuf)) {
	perror("VIDIOC_G_FBUF");
	exit(EXIT_FAILURE);
    }

    for (i = 0; i < 30; i++) {
	char dev_name[16];
	struct fb_fix_screeninfo si;

	snprintf(dev_name, sizeof(dev_name), "/dev/fb%u", i);

	fb_fd = open(dev_name, O_RDWR);
	if (-1 == fb_fd) {
	    switch (errno) {
	    case ENOENT: /* no such file */
	    case ENXIO:  /* no driver */
		continue;

	    default:
		perror("open");
		exit(EXIT_FAILURE);
	    }
	}

	if (0 == ioctl(fb_fd, FBIOGET_FSCREENINFO, &si)) {
	    if (si.smem_start == (unsigned long)fbuf.base)
		break;
	} else {
	    /* Apparently not a framebuffer device. */
	}

	close(fb_fd);
	fb_fd = -1;
    }

    /* fb_fd is the file descriptor of the framebuffer device
       for the video output overlay, or -1 if no device was found. */


Overlay Window and Scaling(覆盖窗口与缩放)
==========================================

The overlay is controlled by source and target rectangles. The source
rectangle selects a subsection of the framebuffer image to be overlaid,
the target rectangle an area in the outgoing video signal where the
image will appear. Drivers may or may not support scaling, and arbitrary
sizes and positions of these rectangles. Further drivers may support any
(or none) of the clipping/blending methods defined for the
:ref:`Video Overlay <overlay>` interface.
(叠加由源矩形和目标矩形控制。源矩形选择帧缓冲图像中要被叠加的一个子区域，
目标矩形选择输出视频信号中图像将出现的一个区域。驱动可能支持也可能不支持
缩放，以及这些矩形的任意尺寸和位置。此外，驱动可能支持（或不支持）为
:ref:`Video Overlay <overlay>` 接口定义的任何裁剪/混合方法。)

A struct :c:type:`v4l2_window` defines the size of the
source rectangle, its position in the framebuffer and the
clipping/blending method to be used for the overlay. To get the current
parameters applications set the ``type`` field of a struct
:c:type:`v4l2_format` to
``V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY`` and call the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl. The driver fills the
struct :c:type:`v4l2_window` substructure named ``win``. It is not
possible to retrieve a previously programmed clipping list or bitmap.
(struct :c:type:`v4l2_window` 定义源矩形的大小、它在帧缓冲中的位置以及
叠加要使用的裁剪/混合方法。要获取当前参数，应用程序把 struct
:c:type:`v4l2_format` 的 ``type`` 字段设置为
``V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY`` 并调用 :ref:`VIDIOC_G_FMT
<VIDIOC_G_FMT>` ioctl。驱动填充名为 ``win`` 的 struct
:c:type:`v4l2_window` 子结构。无法检索之前编程的裁剪列表或位图。)

To program the source rectangle applications set the ``type`` field of a
struct :c:type:`v4l2_format` to
``V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY``, initialize the ``win``
substructure and call the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl.
The driver adjusts the parameters against hardware limits and returns
the actual parameters as :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` does. Like :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`,
the :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl can be used to learn
about driver capabilities without actually changing driver state. Unlike
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` this also works after the overlay has been enabled.
(要编程源矩形，应用程序把 struct :c:type:`v4l2_format` 的 ``type`` 字段
设置为 ``V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY``，初始化 ``win`` 子结构，并
调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl。驱动会对照硬件限制调整
参数，并像 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 那样返回实际参数。与
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 一样，:ref:`VIDIOC_TRY_FMT
<VIDIOC_G_FMT>` ioctl 可用于了解驱动能力而不实际改变驱动状态。与
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 不同的是，这在叠加已启用之后也能
工作。)

A struct :c:type:`v4l2_crop` defines the size and position
of the target rectangle. The scaling factor of the overlay is implied by
the width and height given in struct :c:type:`v4l2_window`
and struct :c:type:`v4l2_crop`. The cropping API applies to
*Video Output* and *Video Output Overlay* devices in the same way as to
*Video Capture* and *Video Overlay* devices, merely reversing the
direction of the data flow. For more information see :ref:`crop`.
(struct :c:type:`v4l2_crop` 定义目标矩形的大小和位置。叠加的缩放因子由
struct :c:type:`v4l2_window` 和 struct :c:type:`v4l2_crop` 中给出的宽度
和高度隐含。裁剪 API 适用于 *Video Output* 和 *Video Output Overlay*
设备的方式与适用于 *Video Capture* 和 *Video Overlay* 设备的方式相同，
只是颠倒了数据流的方向。更多信息参见 :ref:`crop`。)


Enabling Overlay(启用叠加)
==========================

There is no V4L2 ioctl to enable or disable the overlay, however the
framebuffer interface of the driver may support the ``FBIOBLANK`` ioctl.
(没有用于启用或禁用叠加的 V4L2 ioctl，不过驱动的帧缓冲接口可能支持
``FBIOBLANK`` ioctl。)
