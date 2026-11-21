.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_FMT:

******************************************************************************
ioctl VIDIOC_G_FMT, VIDIOC_S_FMT, VIDIOC_TRY_FMT(获取或设置数据格式，尝试格式)
******************************************************************************

Name(名称)
==========

VIDIOC_G_FMT - VIDIOC_S_FMT - VIDIOC_TRY_FMT - Get or set the data format, try a format
(VIDIOC_G_FMT - VIDIOC_S_FMT - VIDIOC_TRY_FMT - 获取或设置数据格式，尝试格式)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_FMT

``int ioctl(int fd, VIDIOC_G_FMT, struct v4l2_format *argp)``

.. c:macro:: VIDIOC_S_FMT

``int ioctl(int fd, VIDIOC_S_FMT, struct v4l2_format *argp)``

.. c:macro:: VIDIOC_TRY_FMT

``int ioctl(int fd, VIDIOC_TRY_FMT, struct v4l2_format *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_format`.
    (指向 struct :c:type:`v4l2_format` 的指针。)

Description(描述)
=================

These ioctls are used to negotiate the format of data (typically image
format) exchanged between driver and application.
(这些 ioctl 用于协商驱动与应用程序之间交换的数据格式（通常是图像
格式）。)

To query the current parameters applications set the ``type`` field of a
struct :c:type:`v4l2_format` to the respective buffer (stream)
type. For example video capture devices use
``V4L2_BUF_TYPE_VIDEO_CAPTURE`` or
``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``. When the application calls the
:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl with a pointer to this structure the driver fills
the respective member of the ``fmt`` union. In case of video capture
devices that is either the struct
:c:type:`v4l2_pix_format` ``pix`` or the struct
:c:type:`v4l2_pix_format_mplane` ``pix_mp``
member. When the requested buffer type is not supported drivers return
an ``EINVAL`` error code.
(要查询当前参数，应用程序将 struct :c:type:`v4l2_format` 的 ``type`` 字段
设置为相应的缓冲区（流）类型。例如，视频采集设备使用
``V4L2_BUF_TYPE_VIDEO_CAPTURE`` 或 ``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``。
当应用程序用指向该结构体的指针调用 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` ioctl
时，驱动会填充 ``fmt`` 联合体的相应成员。对视频采集设备而言，该成员是
struct :c:type:`v4l2_pix_format` ``pix`` 或 struct
:c:type:`v4l2_pix_format_mplane` ``pix_mp``。当所请求的缓冲区类型不受支持
时，驱动返回 ``EINVAL`` 错误码。)

To change the current format parameters applications initialize the
``type`` field and all fields of the respective ``fmt`` union member.
For details see the documentation of the various devices types in
:ref:`devices`. Good practice is to query the current parameters
first, and to modify only those parameters not suitable for the
application. When the application calls the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl with
a pointer to a struct :c:type:`v4l2_format` structure the driver
checks and adjusts the parameters against hardware abilities. Drivers
should not return an error code unless the ``type`` field is invalid,
this is a mechanism to fathom device capabilities and to approach
parameters acceptable for both the application and driver. On success
the driver may program the hardware, allocate resources and generally
prepare for data exchange. Finally the :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl returns
the current format parameters as :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` does. Very simple,
inflexible devices may even ignore all input and always return the
default parameters. However all V4L2 devices exchanging data with the
application must implement the :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` and :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
ioctl. When the requested buffer type is not supported drivers return an
EINVAL error code on a :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` attempt. When I/O is already in
progress or the resource is not available for other reasons drivers
return the ``EBUSY`` error code.
(要更改当前格式参数，应用程序初始化 ``type`` 字段以及相应 ``fmt`` 联合体
成员的所有字段。详情参见 :ref:`devices` 中各种设备类型的文档。良好的做法
是先查询当前参数，只修改那些不适合应用程序的参数。当应用程序用指向
struct :c:type:`v4l2_format` 结构的指针调用 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`
ioctl 时，驱动会根据硬件能力检查并调整参数。除非 ``type`` 字段无效，否则
驱动不应返回错误码——这是一种探测设备能力、并逼近对应用程序和驱动都可接受
的参数的机制。成功时，驱动可以编程硬件、分配资源，并为数据交换做好一般性
准备。最后，:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl 返回当前格式参数，
与 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 的行为相同。非常简单、不灵活的设备
甚至可能忽略所有输入，总是返回默认参数。然而，所有与应用程序交换数据的
V4L2 设备都必须实现 :ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>` 和
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` ioctl。当所请求的缓冲区类型不受支持时，
驱动会在 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 尝试时返回 EINVAL 错误码。当
I/O 已经在进行中，或资源因其他原因不可用时，驱动返回 ``EBUSY`` 错误码。)

The :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl is equivalent to :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` with one
exception: it does not change driver state. It can also be called at any
time, never returning ``EBUSY``. This function is provided to negotiate
parameters, to learn about hardware limitations, without disabling I/O
or possibly time consuming hardware preparations. Although strongly
recommended drivers are not required to implement this ioctl.
(:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` ioctl 等价于
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`，只有一个例外：它不改变驱动状态。它也
可以在任何时候调用，从不返回 ``EBUSY``。提供此函数是为了协商参数、了解
硬件限制，而无需禁用 I/O 或可能耗时的硬件准备工作。尽管强烈建议实现，
但驱动并不被要求实现此 ioctl。)

The format as returned by :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` must be identical to what
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` returns for the same input or output.
(:ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>` 返回的格式必须与
:ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 对相同输入或输出返回的格式完全
一致。)

.. c:type:: v4l2_format

.. tabularcolumns::  |p{7.4cm}|p{4.4cm}|p{5.5cm}|

.. flat-table:: struct v4l2_format
    :header-rows:  0
    :stub-columns: 0

    * - __u32
      - ``type``
      - Type of the data stream, see :c:type:`v4l2_buf_type`.
	(数据流的类型，参见 :c:type:`v4l2_buf_type`。)
    * - union {
      - ``fmt``
    * - struct :c:type:`v4l2_pix_format`
      - ``pix``
      - Definition of an image format, see :ref:`pixfmt`, used by video
	capture and output devices.
	(图像格式的定义，参见 :ref:`pixfmt`，由视频采集和输出设备使用。)
    * - struct :c:type:`v4l2_pix_format_mplane`
      - ``pix_mp``
      - Definition of an image format, see :ref:`pixfmt`, used by video
	capture and output devices that support the
	:ref:`multi-planar version of the API <planar-apis>`.
	(图像格式的定义，参见 :ref:`pixfmt`，由支持 :ref:`multi-planar
	version of the API <planar-apis>`（API 的多平面版本）的视频采集和
	输出设备使用。)
    * - struct :c:type:`v4l2_window`
      - ``win``
      - Definition of an overlaid image, see :ref:`overlay`, used by
	video overlay devices.
	(覆盖图像的定义，参见 :ref:`overlay`，由视频覆盖设备使用。)
    * - struct :c:type:`v4l2_vbi_format`
      - ``vbi``
      - Raw VBI capture or output parameters. This is discussed in more
	detail in :ref:`raw-vbi`. Used by raw VBI capture and output
	devices.
	(原始 VBI 采集或输出参数。在 :ref:`raw-vbi` 中有更详细的讨论。由
	原始 VBI 采集和输出设备使用。)
    * - struct :c:type:`v4l2_sliced_vbi_format`
      - ``sliced``
      - Sliced VBI capture or output parameters. See :ref:`sliced` for
	details. Used by sliced VBI capture and output devices.
	(切片 VBI 采集或输出参数。详情参见 :ref:`sliced`。由切片 VBI 采集
	和输出设备使用。)
    * - struct :c:type:`v4l2_sdr_format`
      - ``sdr``
      - Definition of a data format, see :ref:`pixfmt`, used by SDR
	capture and output devices.
	(数据格式的定义，参见 :ref:`pixfmt`，由 SDR 采集和输出设备使用。)
    * - struct :c:type:`v4l2_meta_format`
      - ``meta``
      - Definition of a metadata format, see :ref:`meta-formats`, used by
	metadata capture devices.
	(元数据格式的定义，参见 :ref:`meta-formats`，由元数据采集设备
	使用。)
    * - __u8
      - ``raw_data``\ [200]
      - Place holder for future extensions.
	(为未来扩展保留的占位符。)
    * - }
      -

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`v4l2_format` ``type`` field is
    invalid or the requested buffer type not supported.
    (struct :c:type:`v4l2_format` 的 ``type`` 字段无效，或所请求的缓冲区
    类型不受支持。)

EBUSY
    The device is busy and cannot change the format. This could be
    because or the device is streaming or buffers are allocated or
    queued to the driver. Relevant for :ref:`VIDIOC_S_FMT
    <VIDIOC_G_FMT>` only.
    (设备正忙，无法更改格式。这可能是因为设备正在流式传输，或缓冲区已
    分配或已入队到驱动。仅与 :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>` 相关。)
