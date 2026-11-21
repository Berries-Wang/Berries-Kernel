.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _format:

**********************
Data Formats(数据格式)
**********************

Data Format Negotiation(数据格式协商)
=====================================

Different devices exchange different kinds of data with applications,
for example video images, raw or sliced VBI data, RDS datagrams. Even
within one kind many different formats are possible, in particular there is an
abundance of image formats. Although drivers must provide a default and
the selection persists across closing and reopening a device,
applications should always negotiate a data format before engaging in
data exchange. Negotiation means the application asks for a particular
format and the driver selects and reports the best the hardware can do
to satisfy the request. Of course applications can also just query the
current selection.
(不同的设备与应用程序交换不同种类的数据，例如视频图像、原始(raw)
或分段(sliced)VBI 数据、RDS 数据报。即使在同一类数据中也可能有许多
不同的格式，尤其是图像格式繁多。尽管驱动程序必须提供默认格式，且所选
格式在关闭并重新打开设备后依然保持，应用程序在进行数据交换之前仍应
始终协商数据格式。协商是指应用程序请求一种特定格式，驱动程序选择并
报告硬件在满足该请求方面能做到的最好结果。当然，应用程序也可以只
查询当前的选择。)

A single mechanism exists to negotiate all data formats using the
aggregate struct :c:type:`v4l2_format` and the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctls. Additionally the
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl can be used to examine
what the hardware *could* do, without actually selecting a new data
format. The data formats supported by the V4L2 API are covered in the
respective device section in :ref:`devices`. For a closer look at
image formats see :ref:`pixfmt`.
(存在一种单一的机制来协商所有数据格式，它使用聚合结构 struct
:c:type:`v4l2_format` 以及 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl。此外，:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>`
ioctl 可用于探查硬件 *能够* 做什么，而无需实际选择新的数据格式。
V4L2 API 支持的数据格式在 :ref:`devices` 的相应设备章节中介绍。
要深入了解图像格式，参见 :ref:`pixfmt`。)

The :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl is a major turning-point in the
initialization sequence. Prior to this point multiple panel applications
can access the same device concurrently to select the current input,
change controls or modify other properties. The first :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
assigns a logical stream (video data, VBI data etc.) exclusively to one
file descriptor.
(:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 是初始化序列中的一个重要
转折点。在此之前，多个面板应用程序可以并发访问同一设备，以选择当前
输入、更改控制项或修改其他属性。第一个 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
会把一个逻辑流(视频数据、VBI 数据等)独占地分配给一个文件描述符。)

Exclusive means no other application, more precisely no other file
descriptor, can grab this stream or change device properties
inconsistent with the negotiated parameters. A video standard change for
example, when the new standard uses a different number of scan lines,
can invalidate the selected image format. Therefore only the file
descriptor owning the stream can make invalidating changes. Accordingly
multiple file descriptors which grabbed different logical streams
prevent each other from interfering with their settings. When for
example video overlay is about to start or already in progress,
simultaneous video capturing may be restricted to the same cropping and
image size.
(独占意味着没有其他应用程序——更准确地说，没有其他文件描述符——
能够抢占这个流，或以与协商参数不一致的方式更改设备属性。例如，当
视频标准发生变化而新标准使用的扫描行数不同时，所选的图像格式可能
失效。因此，只有拥有该流的文件描述符才能做出会使格式失效的更改。
相应地，抢占了不同逻辑流的多个文件描述符会相互阻止对方干扰自己的
设置。例如，当视频 overlay 即将开始或已经在进行时，并发的视频捕获
可能被限制为相同的裁剪和图像尺寸。)

When applications omit the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl its locking side
effects are implied by the next step, the selection of an I/O method
with the :ref:`VIDIOC_REQBUFS` ioctl or implicit
with the first :c:func:`read()` or
:c:func:`write()` call.
(当应用程序省略 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 时，其锁定
副作用会由下一步隐含完成：用 :ref:`VIDIOC_REQBUFS` ioctl 选择 I/O
方法，或通过第一次 :c:func:`read()` 或 :c:func:`write()` 调用隐式
完成。)

Generally only one logical stream can be assigned to a file descriptor,
the exception being drivers permitting simultaneous video capturing and
overlay using the same file descriptor for compatibility with V4L and
earlier versions of V4L2. Switching the logical stream or returning into
"panel mode" is possible by closing and reopening the device. Drivers
*may* support a switch using :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`.
(一般而言，一个文件描述符只能被分配一个逻辑流；例外是一些驱动程序
为了兼容 V4L 和早期版本的 V4L2，允许使用同一文件描述符同时进行视频
捕获和 overlay。可以通过关闭并重新打开设备来切换逻辑流或返回"面板
模式"。驱动程序 *可以* 支持使用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
进行切换。)

All drivers exchanging data with applications must support the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl. Implementation of the
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` is highly recommended but optional.
(所有与应用程序交换数据的驱动程序都必须支持 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`
和 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl。强烈建议实现
:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>`，但它是可选的。)

Image Format Enumeration(图像格式枚举)
======================================

Apart of the generic format negotiation functions a special ioctl to
enumerate all image formats supported by video capture, overlay or
output devices is available. [#f1]_
(除了通用的格式协商功能之外，还有一个特殊的 ioctl 用于枚举视频
捕获、overlay 或输出设备支持的所有图像格式。)

The :ref:`VIDIOC_ENUM_FMT` ioctl must be supported
by all drivers exchanging image data with applications.
(所有与应用程序交换图像数据的驱动程序都必须支持 :ref:`VIDIOC_ENUM_FMT`
ioctl。)

.. important::

    Drivers are not supposed to convert image formats in kernel space.
    They must enumerate only formats directly supported by the hardware.
    If necessary driver writers should publish an example conversion
    routine or library for integration into applications.
    (驱动程序不应在内核空间转换图像格式。它们必须只枚举硬件直接
    支持的格式。如有必要，驱动作者应发布示例转换例程或库，供集成到
    应用程序中使用。)

.. [#f1]
   Enumerating formats an application has no a-priori knowledge of
   (otherwise it could explicitly ask for them and need not enumerate)
   seems useless, but there are applications serving as proxy between
   drivers and the actual video applications for which this is useful.
   (枚举应用程序没有任何先验知识的格式(否则它可以直接请求它们而
   无需枚举)看似无用，但对于那些充当驱动程序与实际视频应用之间代理
   的应用程序来说，这很有用。)
