.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _streaming-par:

****************************
Streaming Parameters(流参数)
****************************

Streaming parameters are intended to optimize the video capture process
as well as I/O. Presently applications can request a high quality
capture mode with the :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctl.
(流参数（streaming parameters）旨在优化视频采集过程以及 I/O。目前，应用程
序可以通过 :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctl 请求高质量采集模式。)

The current video standard determines a nominal number of frames per
second. If less than this number of frames is to be captured or output,
applications can request frame skipping or duplicating on the driver
side. This is especially useful when using the
:c:func:`read()` or :c:func:`write()`, which are
not augmented by timestamps or sequence counters, and to avoid
unnecessary data copying.
(当前视频标准决定了标称的每秒帧数。如果要采集或输出的帧数少于此数，应用
程序可以请求在驱动一侧进行跳帧或复制帧。这在使用 :c:func:`read()` 或
:c:func:`write()` 时尤其有用——它们没有时间戳或序列计数器作为辅助——同时也
可避免不必要的数据复制。)

Finally these ioctls can be used to determine the number of buffers used
internally by a driver in read/write mode. For implications see the
section discussing the :c:func:`read()` function.
(最后，这些 ioctl 可用于确定驱动在读写模式下内部使用的缓冲区数量。相关影
响见讨论 :c:func:`read()` 函数的章节。)

To get and set the streaming parameters applications call the
:ref:`VIDIOC_G_PARM <VIDIOC_G_PARM>` and
:ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctl, respectively. They take
a pointer to a struct :c:type:`v4l2_streamparm`, which
contains a union holding separate parameters for input and output
devices.
(要获取和设置流参数，应用程序分别调用 :ref:`VIDIOC_G_PARM <VIDIOC_G_PARM>`
与 :ref:`VIDIOC_S_PARM <VIDIOC_G_PARM>` ioctl。它们接受指向 struct
:c:type:`v4l2_streamparm` 的指针，该结构包含一个联合体，为输入与输出设备
分别保存参数。)

These ioctls are optional, drivers need not implement them. If so, they
return the ``EINVAL`` error code.
(这些 ioctl 是可选的，驱动无需实现它们。如果不实现，它们会返回 ``EINVAL``
错误码。)
