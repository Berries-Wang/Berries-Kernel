.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _capture:

*************************************
Video Capture Interface(视频捕获接口)
*************************************

Video capture devices sample an analog video signal and store the
digitized images in memory. Today nearly all devices can capture at full
25 or 30 frames/second. With this interface applications can control the
capture process and move images from the driver into user space.
(视频捕获设备对模拟视频信号采样，并把数字化后的图像存储到内存中。
如今几乎所有设备都能以完整的每秒 25 或 30 帧进行捕获。通过该接口，
应用程序可以控制捕获过程，并把图像从驱动程序移入用户空间。)

Conventionally V4L2 video capture devices are accessed through character
device special files named ``/dev/video`` and ``/dev/video0`` to
``/dev/video63`` with major number 81 and minor numbers 0 to 63.
``/dev/video`` is typically a symbolic link to the preferred video
device.
(按照惯例，V4L2 视频捕获设备通过名为 ``/dev/video`` 和
``/dev/video0`` 到 ``/dev/video63`` 的字符设备特殊文件访问，主设备号
为 81，次设备号为 0 到 63。``/dev/video`` 通常是指向首选视频设备的
符号链接。)

.. note:: The same device file names are used for video output devices.
          (同样的设备文件名也用于视频输出设备。)

Querying Capabilities(查询能力)
===============================

Devices supporting the video capture interface set the
``V4L2_CAP_VIDEO_CAPTURE`` or ``V4L2_CAP_VIDEO_CAPTURE_MPLANE`` flag in
the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl. As secondary device
functions they may also support the :ref:`video overlay <overlay>`
(``V4L2_CAP_VIDEO_OVERLAY``) and the :ref:`raw VBI capture <raw-vbi>`
(``V4L2_CAP_VBI_CAPTURE``) interface. At least one of the read/write or
streaming I/O methods must be supported. Tuners and audio inputs are
optional.
(支持视频捕获接口的设备会在 :ref:`VIDIOC_QUERYCAP` ioctl 返回的
struct :c:type:`v4l2_capability` 的 ``capabilities`` 字段中设置
``V4L2_CAP_VIDEO_CAPTURE`` 或 ``V4L2_CAP_VIDEO_CAPTURE_MPLANE`` 标志。
作为次要设备功能，它们还可以支持 :ref:`video overlay <overlay>`
(``V4L2_CAP_VIDEO_OVERLAY``)和 :ref:`raw VBI capture <raw-vbi>`
(``V4L2_CAP_VBI_CAPTURE``)接口。至少必须支持读/写或流式 I/O 方法中的
一种。调谐器和音频输入是可选的。)

Supplemental Functions(辅助功能)
================================

Video capture devices shall support :ref:`audio input <audio>`,
:ref:`tuner`, :ref:`controls <control>`,
:ref:`cropping and scaling <crop>` and
:ref:`streaming parameter <streaming-par>` ioctls as needed. The
:ref:`video input <video>` ioctls must be supported by all video
capture devices.
(视频捕获设备应按需支持 :ref:`audio input <audio>`、:ref:`tuner`、
:ref:`controls <control>`、:ref:`cropping and scaling <crop>` 和
:ref:`streaming parameter <streaming-par>` ioctl。所有视频捕获设备都
必须支持 :ref:`video input <video>` ioctl。)

Image Format Negotiation(图像格式协商)
======================================

The result of a capture operation is determined by cropping and image
format parameters. The former select an area of the video picture to
capture, the latter how images are stored in memory, i. e. in RGB or YUV
format, the number of bits per pixel or width and height. Together they
also define how images are scaled in the process.
(捕获操作的结果由裁剪参数和图像格式参数共同决定。前者选择要捕获的
视频画面区域，后者决定图像在内存中的存储方式，即 RGB 或 YUV 格式、
每像素位数以及宽度和高度。它们还共同定义了图像在此过程中的缩放
方式。)

As usual these parameters are *not* reset at :c:func:`open()`
time to permit Unix tool chains, programming a device and then reading
from it as if it was a plain file. Well written V4L2 applications ensure
they really get what they want, including cropping and scaling.
(照例，这些参数在 :c:func:`open()` 时不会被重置，以允许 Unix 工具链
先对设备编程，然后像读取普通文件一样从它读取。编写良好的 V4L2 应用
程序会确保自己真正得到了想要的东西，包括裁剪和缩放。)

Cropping initialization at minimum requires to reset the parameters to
defaults. An example is given in :ref:`crop`.
(裁剪初始化至少需要把参数重置为默认值。示例见 :ref:`crop`。)

To query the current image format applications set the ``type`` field of
a struct :c:type:`v4l2_format` to
``V4L2_BUF_TYPE_VIDEO_CAPTURE`` or
``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE`` and call the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl with a pointer to this
structure. Drivers fill the struct
:c:type:`v4l2_pix_format` ``pix`` or the struct
:c:type:`v4l2_pix_format_mplane` ``pix_mp``
member of the ``fmt`` union.
(要查询当前图像格式，应用程序把 struct :c:type:`v4l2_format` 的
``type`` 字段设为 ``V4L2_BUF_TYPE_VIDEO_CAPTURE`` 或
``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``，然后以指向该结构体的指针调用
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl。驱动程序填充 ``fmt`` 联合体
中的 struct :c:type:`v4l2_pix_format` ``pix`` 或 struct
:c:type:`v4l2_pix_format_mplane` ``pix_mp`` 成员。)

To request different parameters applications set the ``type`` field of a
struct :c:type:`v4l2_format` as above and initialize all
fields of the struct :c:type:`v4l2_pix_format`
``vbi`` member of the ``fmt`` union, or better just modify the results
of :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`, and call the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
ioctl with a pointer to this structure. Drivers may adjust the
parameters and finally return the actual parameters as :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`
does.
(要请求不同的参数，应用程序按上述方式设置 struct
:c:type:`v4l2_format` 的 ``type`` 字段，并初始化 ``fmt`` 联合体中
struct :c:type:`v4l2_pix_format` ``vbi`` 成员的所有字段——或者更好的
做法是直接修改 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 的结果——然后以指向
该结构体的指针调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl。驱动程序
可以调整参数，并最终像 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 那样返回
实际参数。)

Like :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` the :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl
can be used to learn about hardware limitations without disabling I/O or
possibly time consuming hardware preparations.
(与 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 一样，:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>`
ioctl 也可用于了解硬件限制，而无需禁用 I/O 或进行可能耗时的硬件
准备。)

The contents of struct :c:type:`v4l2_pix_format` and
struct :c:type:`v4l2_pix_format_mplane` are
discussed in :ref:`pixfmt`. See also the specification of the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`, :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` and :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctls for
details. Video capture devices must implement both the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`
and :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl, even if :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ignores all
requests and always returns default parameters as :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` does.
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` is optional.
(struct :c:type:`v4l2_pix_format` 和 struct
:c:type:`v4l2_pix_format_mplane` 的内容在 :ref:`pixfmt` 中讨论。
详情另见 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`、:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
和 :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl 的规范。视频捕获设备必须
同时实现 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
ioctl，即使 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 忽略所有请求并总是像
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 那样返回默认参数。
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` 是可选的。)

Reading Images(读取图像)
========================

A video capture device may support the :ref:`read() function <func-read>`
and/or streaming (:ref:`memory mapping <func-mmap>` or
:ref:`user pointer <userp>`) I/O. See :ref:`io` for details.
(视频捕获设备可以支持 :ref:`read() function <func-read>` 和/或流式
(:ref:`memory mapping <func-mmap>` 或 :ref:`user pointer <userp>`)
I/O。详情见 :ref:`io`。)
