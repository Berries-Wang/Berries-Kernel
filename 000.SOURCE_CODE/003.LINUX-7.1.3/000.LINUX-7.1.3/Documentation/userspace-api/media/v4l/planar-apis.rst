.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _planar-apis:

*************************************************
Single- and multi-planar APIs(单平面与多平面 API)
*************************************************

Some devices require data for each input or output video frame to be
placed in discontiguous memory buffers. In such cases, one video frame
has to be addressed using more than one memory address, i.e. one pointer
per "plane". A plane is a sub-buffer of the current frame. For examples
of such formats see :ref:`pixfmt`.
(有些设备要求每个输入或输出视频帧的数据放在不连续的内存缓冲区中。这种
情况下，一个视频帧必须用多于一个内存地址来寻址，即每个"平面"
(plane) 一个指针。平面是当前帧的一个子缓冲区。此类格式的例子参见
:ref:`pixfmt` 。)

Initially, V4L2 API did not support multi-planar buffers and a set of
extensions has been introduced to handle them. Those extensions
constitute what is being referred to as the "multi-planar API".
(最初，V4L2 API 不支持多平面缓冲区，后来引入了一组扩展来处理它们。这些
扩展构成了通常所说的"多平面 API"。)

Some of the V4L2 API calls and structures are interpreted differently,
depending on whether single- or multi-planar API is being used. An
application can choose whether to use one or the other by passing a
corresponding buffer type to its ioctl calls. Multi-planar versions of
buffer types are suffixed with an ``_MPLANE`` string. For a list of
available multi-planar buffer types see enum
:c:type:`v4l2_buf_type`.
(一些 V4L2 API 调用和结构会根据使用的是单平面还是多平面 API 而有不同解释。
应用可以通过向其 ioctl 调用传入相应的缓冲区类型来选择使用其中一种。多平面
版本的缓冲区类型以 ``_MPLANE`` 字符串作为后缀。可用的多平面缓冲区类型列表
见 enum :c:type:`v4l2_buf_type` 。)


Multi-planar formats(多平面格式)
================================

Multi-planar API introduces new multi-planar formats. Those formats use
a separate set of FourCC codes. It is important to distinguish between
the multi-planar API and a multi-planar format. Multi-planar API calls
can handle all single-planar formats as well (as long as they are passed
in multi-planar API structures), while the single-planar API cannot
handle multi-planar formats.
(多平面 API 引入了新的多平面格式。这些格式使用一组独立的 FourCC 码。
区分多平面 API 与多平面格式非常重要。多平面 API 调用也可以处理所有单平面
格式 (只要以多平面 API 结构传入)，而单平面 API 无法处理多平面格式。)


Calls that distinguish between single and multi-planar APIs(区分单平面与多平面 API 的调用)
==========================================================================================

:ref:`VIDIOC_QUERYCAP <VIDIOC_QUERYCAP>`
    Two additional multi-planar capabilities are added. They can be set
    together with non-multi-planar ones for devices that handle both
    single- and multi-planar formats.
    (新增了两个额外的多平面能力标志。对于同时处理单平面和多平面格式的
    设备，它们可以与非多平面的标志一起设置。)

:ref:`VIDIOC_G_FMT <VIDIOC_G_FMT>`, :ref:`VIDIOC_S_FMT <VIDIOC_G_FMT>`, :ref:`VIDIOC_TRY_FMT <VIDIOC_G_FMT>`
    New structures for describing multi-planar formats are added: struct
    :c:type:`v4l2_pix_format_mplane` and
    struct :c:type:`v4l2_plane_pix_format`.
    Drivers may define new multi-planar formats, which have distinct
    FourCC codes from the existing single-planar ones.
    (新增了用于描述多平面格式的新结构：struct
    :c:type:`v4l2_pix_format_mplane` 和 struct
    :c:type:`v4l2_plane_pix_format` 。驱动可以定义新的多平面格式，它们
    使用与现有单平面格式不同的 FourCC 码。)

:ref:`VIDIOC_QBUF <VIDIOC_QBUF>`, :ref:`VIDIOC_DQBUF <VIDIOC_QBUF>`, :ref:`VIDIOC_QUERYBUF <VIDIOC_QUERYBUF>`
    A new struct :c:type:`v4l2_plane` structure for
    describing planes is added. Arrays of this structure are passed in
    the new ``m.planes`` field of struct
    :c:type:`v4l2_buffer`.
    (新增了用于描述平面的 struct :c:type:`v4l2_plane` 结构。该结构的数组
    通过 struct :c:type:`v4l2_buffer` 中新的 ``m.planes`` 字段传入。)

:ref:`VIDIOC_REQBUFS <VIDIOC_REQBUFS>`
    Will allocate multi-planar buffers as requested.
    (将按请求分配多平面缓冲区。)
