.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_CROP:

**********************************************************
ioctl VIDIOC_G_CROP, VIDIOC_S_CROP(获取或设置当前裁剪矩形)
**********************************************************

Name(名称)
==========

VIDIOC_G_CROP - VIDIOC_S_CROP - Get or set the current cropping rectangle(VIDIOC_G_CROP - VIDIOC_S_CROP - 获取或设置当前裁剪矩形)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_CROP

``int ioctl(int fd, VIDIOC_G_CROP, struct v4l2_crop *argp)``

.. c:macro:: VIDIOC_S_CROP

``int ioctl(int fd, VIDIOC_S_CROP, const struct v4l2_crop *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_crop`.
    (指向 struct :c:type:`v4l2_crop` 的指针。)

Description(描述)
=================

To query the cropping rectangle size and position applications set the
``type`` field of a struct :c:type:`v4l2_crop` structure to the
respective buffer (stream) type and call the :ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` ioctl
with a pointer to this structure. The driver fills the rest of the
structure or returns the ``EINVAL`` error code if cropping is not supported.
(要查询裁剪矩形的尺寸和位置，应用程序把 struct :c:type:`v4l2_crop` 结构体的
``type`` 字段设置为相应的缓冲区(流)类型，并以指向该结构体的指针调用
:ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` ioctl。驱动程序会填充结构体的其余部分；
如果不支持裁剪，则返回 ``EINVAL`` 错误码。)

To change the cropping rectangle applications initialize the ``type``
and struct :c:type:`v4l2_rect` substructure named ``c`` of a
v4l2_crop structure and call the :ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` ioctl with a pointer
to this structure.
(要更改裁剪矩形，应用程序初始化 v4l2_crop 结构体的 ``type`` 字段和名为
``c`` 的 struct :c:type:`v4l2_rect` 子结构，并以指向该结构体的指针调用
:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` ioctl。)

The driver first adjusts the requested dimensions against hardware
limits, i. e. the bounds given by the capture/output window, and it
rounds to the closest possible values of horizontal and vertical offset,
width and height. In particular the driver must round the vertical
offset of the cropping rectangle to frame lines modulo two, such that
the field order cannot be confused.
(驱动程序首先根据硬件限制调整请求的尺寸，即捕获/输出窗口所给定的边界，
并把水平与垂直偏移、宽度和高度舍入到最接近的可能值。特别地，驱动程序
必须把裁剪矩形的垂直偏移按帧线舍入到 2 的倍数，以免弄错场序。)

Second the driver adjusts the image size (the opposite rectangle of the
scaling process, source or target depending on the data direction) to
the closest size possible while maintaining the current horizontal and
vertical scaling factor.
(其次，驱动程序在保持当前水平和垂直缩放因子不变的前提下，把图像尺寸
(缩放过程的另一个矩形，依数据方向而定是源还是目标)调整到最接近的可能
尺寸。)

Finally the driver programs the hardware with the actual cropping and
image parameters. :ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` is a write-only ioctl, it does not
return the actual parameters. To query them applications must call
:ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` and :ref:`VIDIOC_G_FMT`. When the
parameters are unsuitable the application may modify the cropping or
image parameters and repeat the cycle until satisfactory parameters have
been negotiated.
(最后，驱动程序用实际的裁剪和图像参数对硬件进行编程。:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>`
是只写 ioctl，不返回实际参数。要查询这些参数，应用程序必须调用
:ref:`VIDIOC_G_CROP <VIDIOC_G_CROP>` 和 :ref:`VIDIOC_G_FMT`。当参数不合适时，应用程序可以
修改裁剪或图像参数并重复这一循环，直到协商出令人满意的参数为止。)

When cropping is not supported then no parameters are changed and
:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` returns the ``EINVAL`` error code.
(当不支持裁剪时，任何参数都不会被更改，:ref:`VIDIOC_S_CROP <VIDIOC_G_CROP>` 返回
``EINVAL`` 错误码。)

.. c:type:: v4l2_crop

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_crop
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Type of the data stream, set by the application. Only these types
	are valid here: ``V4L2_BUF_TYPE_VIDEO_CAPTURE``, ``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``,
	``V4L2_BUF_TYPE_VIDEO_OUTPUT``, ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE`` and
	``V4L2_BUF_TYPE_VIDEO_OVERLAY``. See :c:type:`v4l2_buf_type` and the note below.
        (数据流的类型，由应用程序设置。此处只有以下类型有效：
        ``V4L2_BUF_TYPE_VIDEO_CAPTURE``、``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``、
        ``V4L2_BUF_TYPE_VIDEO_OUTPUT``、``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE`` 和
        ``V4L2_BUF_TYPE_VIDEO_OVERLAY``。参见 :c:type:`v4l2_buf_type` 及下文说明。)
    * - struct :c:type:`v4l2_rect`
      - ``c``
      - Cropping rectangle. The same co-ordinate system as for struct
	:c:type:`v4l2_cropcap` ``bounds`` is used.
        (裁剪矩形。使用与 struct :c:type:`v4l2_cropcap` ``bounds`` 相同的坐标系。)

.. note::
   Unfortunately in the case of multiplanar buffer types
   (``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` and ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``)
   this API was messed up with regards to how the :c:type:`v4l2_crop` ``type`` field
   should be filled in. Some drivers only accepted the ``_MPLANE`` buffer type while
   other drivers only accepted a non-multiplanar buffer type (i.e. without the
   ``_MPLANE`` at the end).

   Starting with kernel 4.13 both variations are allowed.

   (遗憾的是，对于多平面缓冲区类型
   (``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` 和 ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``)，
   这个 API 在 :c:type:`v4l2_crop` ``type`` 字段应如何填写方面一团糟。有些驱动程序
   只接受 ``_MPLANE`` 缓冲区类型，而另一些驱动程序只接受非多平面缓冲区类型
   (即结尾不带 ``_MPLANE``)。

   从内核 4.13 开始，两种写法都被允许。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

ENODATA
    Cropping is not supported for this input or output.
    (此输入或输出不支持裁剪。)
