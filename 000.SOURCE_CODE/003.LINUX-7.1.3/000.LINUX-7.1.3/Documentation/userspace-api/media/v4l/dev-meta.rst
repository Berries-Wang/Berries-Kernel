.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _metadata:

******************************
Metadata Interface(元数据接口)
******************************

Metadata refers to any non-image data that supplements video frames with
additional information. This may include statistics computed over the image,
frame capture parameters supplied by the image source or device specific
parameters for specifying how the device processes images. This interface is
intended for transfer of metadata between the userspace and the hardware and
control of that operation.
(元数据指为视频帧补充附加信息的任何非图像数据。这可能包括对图像计算
出的统计信息、由图像源提供的帧捕获参数，或用于指定设备如何处理图像
的设备特定参数。该接口用于在用户空间与硬件之间传输元数据，并控制该
操作。)

The metadata interface is implemented on video device nodes. The device can be
dedicated to metadata or can support both video and metadata as specified in its
reported capabilities.
(元数据接口实现在视频设备节点上。设备可以专用于元数据，也可以像其
报告的能力所指定的那样，同时支持视频和元数据。)

Querying Capabilities(查询能力)
===============================

Device nodes supporting the metadata capture interface set the
``V4L2_CAP_META_CAPTURE`` flag in the ``device_caps`` field of the
:c:type:`v4l2_capability` structure returned by the :c:func:`VIDIOC_QUERYCAP`
ioctl. That flag means the device can capture metadata to memory. Similarly,
device nodes supporting metadata output interface set the
``V4L2_CAP_META_OUTPUT`` flag in the ``device_caps`` field of
:c:type:`v4l2_capability` structure. That flag means the device can read
metadata from memory.
(支持元数据捕获接口的设备节点会在 :c:func:`VIDIOC_QUERYCAP` ioctl
返回的 :c:type:`v4l2_capability` 结构的 ``device_caps`` 字段中设置
``V4L2_CAP_META_CAPTURE`` 标志。该标志表示设备可以把元数据捕获到
内存。类似地，支持元数据输出接口的设备节点会在 :c:type:`v4l2_capability`
结构的 ``device_caps`` 字段中设置 ``V4L2_CAP_META_OUTPUT`` 标志。
该标志表示设备可以从内存读取元数据。)

At least one of the read/write or streaming I/O methods must be supported.
(至少必须支持读/写或流式 I/O 方法中的一种。)


Data Format Negotiation(数据格式协商)
=====================================

The metadata device uses the :ref:`format` ioctls to select the capture format.
The metadata buffer content format is bound to that selected format. In addition
to the basic :ref:`format` ioctls, the :c:func:`VIDIOC_ENUM_FMT` ioctl must be
supported as well.
(元数据设备使用 :ref:`format` ioctl 来选择捕获格式。元数据缓冲区
内容的格式绑定到该所选格式。除了基本的 :ref:`format` ioctl 之外，
还必须支持 :c:func:`VIDIOC_ENUM_FMT` ioctl。)

To use the :ref:`format` ioctls applications set the ``type`` field of the
:c:type:`v4l2_format` structure to ``V4L2_BUF_TYPE_META_CAPTURE`` or to
``V4L2_BUF_TYPE_META_OUTPUT`` and use the :c:type:`v4l2_meta_format` ``meta``
member of the ``fmt`` union as needed per the desired operation. Both drivers
and applications must set the remainder of the :c:type:`v4l2_format` structure
to 0.
(要使用 :ref:`format` ioctl，应用程序把 :c:type:`v4l2_format` 结构的
``type`` 字段设为 ``V4L2_BUF_TYPE_META_CAPTURE`` 或
``V4L2_BUF_TYPE_META_OUTPUT``，并根据所需操作按需使用 ``fmt`` 联合体
中的 :c:type:`v4l2_meta_format` ``meta`` 成员。驱动程序和应用程序都
必须把 :c:type:`v4l2_format` 结构的其余部分置为 0。)

Devices that capture metadata by line have the struct v4l2_fmtdesc
``V4L2_FMT_FLAG_META_LINE_BASED`` flag set for :c:func:`VIDIOC_ENUM_FMT`. Such
devices can typically also :ref:`capture image data <capture>`. This primarily
involves devices that receive the data from a different devices such as a camera
sensor.
(按行捕获元数据的设备会在 :c:func:`VIDIOC_ENUM_FMT` 中设置 struct
v4l2_fmtdesc 的 ``V4L2_FMT_FLAG_META_LINE_BASED`` 标志。这类设备
通常也能 :ref:`捕获图像数据 <capture>`。这主要涉及从其他设备(例如
相机传感器)接收数据的设备。)

.. c:type:: v4l2_meta_format

.. tabularcolumns:: |p{1.4cm}|p{2.4cm}|p{13.5cm}|

.. flat-table:: struct v4l2_meta_format
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``dataformat``
      - The data format, set by the application. This is a little endian
        :ref:`four character code <v4l2-fourcc>`. V4L2 defines metadata formats
        in :ref:`meta-formats`.
        (数据格式，由应用程序设置。这是一个小端序的 :ref:`four
        character code <v4l2-fourcc>`。V4L2 在 :ref:`meta-formats` 中
        定义元数据格式。)
    * - __u32
      - ``buffersize``
      - Maximum buffer size in bytes required for data. The value is set by the
        driver.
        (数据所需的最大缓冲区大小(字节)。该值由驱动程序设置。)
    * - __u32
      - ``width``
      - Width of a line of metadata in Data Units. Valid when
	:c:type`v4l2_fmtdesc` flag ``V4L2_FMT_FLAG_META_LINE_BASED`` is set,
	otherwise zero. See :c:func:`VIDIOC_ENUM_FMT`.
        (以数据单元(Data Units)计的一行元数据的宽度。仅在设置了
        :c:type`v4l2_fmtdesc` 标志 ``V4L2_FMT_FLAG_META_LINE_BASED``
        时有效，否则为零。参见 :c:func:`VIDIOC_ENUM_FMT`。)
    * - __u32
      - ``height``
      - Number of rows of metadata. Valid when :c:type`v4l2_fmtdesc` flag
	``V4L2_FMT_FLAG_META_LINE_BASED`` is set, otherwise zero. See
	:c:func:`VIDIOC_ENUM_FMT`.
        (元数据的行数。仅在设置了 :c:type`v4l2_fmtdesc` 标志
        ``V4L2_FMT_FLAG_META_LINE_BASED`` 时有效，否则为零。参见
        :c:func:`VIDIOC_ENUM_FMT`。)
    * - __u32
      - ``bytesperline``
      - Offset in bytes between the beginning of two consecutive lines. Valid
	when :c:type`v4l2_fmtdesc` flag ``V4L2_FMT_FLAG_META_LINE_BASED`` is
	set, otherwise zero. See :c:func:`VIDIOC_ENUM_FMT`.
        (相邻两行起始之间的字节偏移。仅在设置了 :c:type`v4l2_fmtdesc`
        标志 ``V4L2_FMT_FLAG_META_LINE_BASED`` 时有效，否则为零。参见
        :c:func:`VIDIOC_ENUM_FMT`。)
