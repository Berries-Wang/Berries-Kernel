.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_CROPCAP:

********************
ioctl VIDIOC_CROPCAP
********************

Name(名称)
==========

VIDIOC_CROPCAP - Information about the video cropping and scaling abilities
(VIDIOC_CROPCAP - 关于视频裁剪和缩放能力的信息。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_CROPCAP

``int ioctl(int fd, VIDIOC_CROPCAP, struct v4l2_cropcap *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_cropcap`.
    (指向 struct :c:type:`v4l2_cropcap` 的指针。)

Description(描述)
=================

Applications use this function to query the cropping limits, the pixel
aspect of images and to calculate scale factors. They set the ``type``
field of a v4l2_cropcap structure to the respective buffer (stream)
type and call the :ref:`VIDIOC_CROPCAP` ioctl with a pointer to this
structure. Drivers fill the rest of the structure. The results are
constant except when switching the video standard. Remember this switch
can occur implicit when switching the video input or output.
(应用程序使用此函数查询裁剪限制、图像的像素宽高比并计算缩放因子。它们把 v4l2_cropcap 结构的 ``type`` 字段设置为相应的缓冲区（流）类型，并以指向该结构的指针调用 :ref:`VIDIOC_CROPCAP` ioctl。驱动程序填充结构的其余部分。除切换视频标准时之外，结果都是恒定的。请记住，切换视频输入或输出时可能会隐式发生这种切换。)

This ioctl must be implemented for video capture or output devices that
support cropping and/or scaling and/or have non-square pixels, and for
overlay devices.
(支持裁剪和/或缩放和/或具有非方形像素的视频捕获或输出设备，以及 overlay 设备，必须实现此 ioctl。)

.. c:type:: v4l2_cropcap

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_cropcap
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Type of the data stream, set by the application. Only these types
	are valid here: ``V4L2_BUF_TYPE_VIDEO_CAPTURE``, ``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``,
	``V4L2_BUF_TYPE_VIDEO_OUTPUT``, ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE`` and
	``V4L2_BUF_TYPE_VIDEO_OVERLAY``. See :c:type:`v4l2_buf_type` and the note below.
        (数据流的类型，由应用程序设置。此处只有以下类型有效： ``V4L2_BUF_TYPE_VIDEO_CAPTURE`` 、 ``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` 、 ``V4L2_BUF_TYPE_VIDEO_OUTPUT`` 、 ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE`` 和 ``V4L2_BUF_TYPE_VIDEO_OVERLAY`` 。参见 :c:type:`v4l2_buf_type` 以及下面的说明。)
    * - struct :ref:`v4l2_rect <v4l2-rect-crop>`
      - ``bounds``
      - Defines the window within capturing or output is possible, this
	may exclude for example the horizontal and vertical blanking
	areas. The cropping rectangle cannot exceed these limits. Width
	and height are defined in pixels, the driver writer is free to
	choose origin and units of the coordinate system in the analog
	domain.
        (定义可以进行捕获或输出的窗口，例如可能不包括水平和垂直消隐区域。裁剪矩形不能超过这些限制。宽度和高度以像素定义，驱动程序编写者可自由选择模拟域中坐标系的原点和单位。)
    * - struct :ref:`v4l2_rect <v4l2-rect-crop>`
      - ``defrect``
      - Default cropping rectangle, it shall cover the "whole picture".
	Assuming pixel aspect 1/1 this could be for example a 640 × 480
	rectangle for NTSC, a 768 × 576 rectangle for PAL and SECAM
	centered over the active picture area. The same coordinate system
	as for ``bounds`` is used.
        (默认裁剪矩形，它应覆盖"完整图像"。假设像素宽高比为 1/1，对于 NTSC 它可以是例如以有效图像区域为中心的 640 × 480 矩形，对于 PAL 和 SECAM 则是 768 × 576 矩形。使用与 ``bounds`` 相同的坐标系。)
    * - struct :c:type:`v4l2_fract`
      - ``pixelaspect``
      - This is the pixel aspect (y / x) when no scaling is applied, the
	ratio of the actual sampling frequency and the frequency required
	to get square pixels.

	When cropping coordinates refer to square pixels, the driver sets
	``pixelaspect`` to 1/1. Other common values are 54/59 for PAL and
	SECAM, 11/10 for NTSC sampled according to [:ref:`itu601`].
        (这是未应用缩放时的像素宽高比（y / x），即实际采样频率与获得方形像素所需频率之比。)

        (当裁剪坐标对应方形像素时，驱动程序将 ``pixelaspect`` 设置为 1/1。其他常见值：按 [:ref:`itu601`] 采样的 PAL 和 SECAM 为 54/59，NTSC 为 11/10。)

.. note::
   Unfortunately in the case of multiplanar buffer types
   (``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` and ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``)
   this API was messed up with regards to how the :c:type:`v4l2_cropcap` ``type`` field
   should be filled in. Some drivers only accepted the ``_MPLANE`` buffer type while
   other drivers only accepted a non-multiplanar buffer type (i.e. without the
   ``_MPLANE`` at the end).
   (遗憾的是，对于多平面缓冲区类型（ ``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` 和 ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE`` ），这个 API 在 :c:type:`v4l2_cropcap` 的 ``type`` 字段应如何填写方面被搞乱了。一些驱动程序只接受 ``_MPLANE`` 缓冲区类型，而另一些驱动程序只接受非多平面缓冲区类型（即结尾不带 ``_MPLANE`` ）。)

   Starting with kernel 4.13 both variations are allowed.
   (从内核 4.13 开始，两种变体都被允许。)


.. _v4l2-rect-crop:

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_rect
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s32
      - ``left``
      - Horizontal offset of the top, left corner of the rectangle, in
	pixels.
        (矩形左上角的水平偏移，以像素为单位。)
    * - __s32
      - ``top``
      - Vertical offset of the top, left corner of the rectangle, in
	pixels.
        (矩形左上角的垂直偏移，以像素为单位。)
    * - __u32
      - ``width``
      - Width of the rectangle, in pixels.
        (矩形的宽度，以像素为单位。)
    * - __u32
      - ``height``
      - Height of the rectangle, in pixels.
        (矩形的高度，以像素为单位。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_cropcap` ``type`` is
    invalid.
    (struct :c:type:`v4l2_cropcap` 的 ``type`` 无效。)

ENODATA
    Cropping is not supported for this input or output.
    (此输入或输出不支持裁剪。)
