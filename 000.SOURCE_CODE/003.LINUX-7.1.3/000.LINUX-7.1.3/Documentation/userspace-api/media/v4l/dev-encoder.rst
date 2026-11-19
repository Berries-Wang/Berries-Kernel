.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _encoder:

*********************************************************************************
Memory-to-Memory Stateful Video Encoder Interface(内存到内存有状态视频编码器接口)
*********************************************************************************

A stateful video encoder takes raw video frames in display order and encodes
them into a bytestream. It generates complete chunks of the bytestream, including
all metadata, headers, etc. The resulting bytestream does not require any
further post-processing by the client.
(有状态视频编码器接收显示顺序的 raw 视频帧，并把它们编码为字节流。它生成字节流的
完整块，包括所有元数据、头等。得到的字节流不需要客户端做进一步的后处理。)

Performing software stream processing, header generation etc. in the driver
in order to support this interface is strongly discouraged. In case such
operations are needed, use of the Stateless Video Encoder Interface (in
development) is strongly advised.
(在驱动中执行软件流处理、头生成等操作以支持此接口是强烈不鼓励的。如果需要这类操作，
强烈建议使用无状态视频编码器接口（开发中）。)

Conventions and Notations Used in This Document(本文档使用的约定与记法)
=======================================================================

1. The general V4L2 API rules apply if not specified in this document
   otherwise.
   (除非本文档另有说明，否则适用 V4L2 API 的一般规则。)

2. The meaning of words "must", "may", "should", etc. is as per `RFC
   2119 <https://tools.ietf.org/html/rfc2119>`_.
   (词语 "must"、"may"、"should" 等的含义遵循 RFC 2119。)

3. All steps not marked "optional" are required.
(所有未标记 "optional" 的步骤都是必需的。)

4. :c:func:`VIDIOC_G_EXT_CTRLS` and :c:func:`VIDIOC_S_EXT_CTRLS` may be used
   interchangeably with :c:func:`VIDIOC_G_CTRL` and :c:func:`VIDIOC_S_CTRL`,
   unless specified otherwise.
   (VIDIOC_G_EXT_CTRLS 与 VIDIOC_S_EXT_CTRLS 可以与 VIDIOC_G_CTRL 和
   VIDIOC_S_CTRL 互换使用，除非另有说明。)

5. Single-planar API (see :ref:`planar-apis`) and applicable structures may be
   used interchangeably with multi-planar API, unless specified otherwise,
   depending on encoder capabilities and following the general V4L2 guidelines.
   (除非另有说明，单平面 API（见 :ref:`planar-apis`）及适用的结构可以与多平面 API
   互换使用，具体取决于编码器能力并遵循一般 V4L2 准则。)

6. i = [a..b]: sequence of integers from a to b, inclusive, i.e. i =
   [0..2]: i = 0, 1, 2.
   (i = [a..b]：从 a 到 b（含）的整数序列，即 i = [0..2] 表示 i = 0, 1, 2。)

7. Given an ``OUTPUT`` buffer A, then A' represents a buffer on the ``CAPTURE``
   queue containing data that resulted from processing buffer A.
   (给定一个 ``OUTPUT`` 缓冲 A，则 A' 表示 ``CAPTURE`` 队列上包含处理缓冲 A 所得
   结果的缓冲。)

Glossary(术语表)
================

Refer to :ref:`decoder-glossary`.
(参见 :ref:`decoder-glossary`。)

State Machine(状态机)
=====================

.. kernel-render:: DOT
   :alt: DOT digraph of encoder state machine
   :caption: Encoder State Machine

   digraph encoder_state_machine {
       node [shape = doublecircle, label="Encoding"] Encoding;

       node [shape = circle, label="Initialization"] Initialization;
       node [shape = circle, label="Stopped"] Stopped;
       node [shape = circle, label="Drain"] Drain;
       node [shape = circle, label="Reset"] Reset;

       node [shape = point]; qi
       qi -> Initialization [ label = "open()" ];

       Initialization -> Encoding [ label = "Both queues streaming" ];

       Encoding -> Drain [ label = "V4L2_ENC_CMD_STOP" ];
       Encoding -> Reset [ label = "VIDIOC_STREAMOFF(CAPTURE)" ];
       Encoding -> Stopped [ label = "VIDIOC_STREAMOFF(OUTPUT)" ];
       Encoding -> Encoding;

       Drain -> Stopped [ label = "All CAPTURE\nbuffers dequeued\nor\nVIDIOC_STREAMOFF(OUTPUT)" ];
       Drain -> Reset [ label = "VIDIOC_STREAMOFF(CAPTURE)" ];

       Reset -> Encoding [ label = "VIDIOC_STREAMON(CAPTURE)" ];
       Reset -> Initialization [ label = "VIDIOC_REQBUFS(OUTPUT, 0)" ];

       Stopped -> Encoding [ label = "V4L2_ENC_CMD_START\nor\nVIDIOC_STREAMON(OUTPUT)" ];
       Stopped -> Reset [ label = "VIDIOC_STREAMOFF(CAPTURE)" ];
   }

Querying Capabilities(查询能力)
===============================

1. To enumerate the set of coded formats supported by the encoder, the
   client may call :c:func:`VIDIOC_ENUM_FMT` on ``CAPTURE``.
   (要枚举编码器支持的编码格式集合，客户端可以在 ``CAPTURE`` 上调用
   VIDIOC_ENUM_FMT。)

   * The full set of supported formats will be returned, regardless of the
     format set on ``OUTPUT``.
     (将返回受支持格式的完整集合，与 ``OUTPUT`` 上设置的格式无关。)

2. To enumerate the set of supported raw formats, the client may call
   :c:func:`VIDIOC_ENUM_FMT` on ``OUTPUT``.
   (要枚举受支持的 raw 格式集合，客户端可以在 ``OUTPUT`` 上调用
   VIDIOC_ENUM_FMT。)

   * Only the formats supported for the format currently active on ``CAPTURE``
     will be returned.
     (只会返回与 ``CAPTURE`` 上当前活动格式所支持的格式。)

   * In order to enumerate raw formats supported by a given coded format,
     the client must first set that coded format on ``CAPTURE`` and then
     enumerate the formats on ``OUTPUT``.
     (为了枚举某个给定编码格式所支持的 raw 格式，客户端必须先在 ``CAPTURE`` 上
     设置该编码格式，然后再在 ``OUTPUT`` 上枚举格式。)

3. The client may use :c:func:`VIDIOC_ENUM_FRAMESIZES` to detect supported
   resolutions for a given format, passing the desired pixel format in
   :c:type:`v4l2_frmsizeenum` ``pixel_format``.
   (客户端可以使用 VIDIOC_ENUM_FRAMESIZES 来检测给定格式支持的分辨率，方法是把
   期望的像素格式传入 :c:type:`v4l2_frmsizeenum` 的 ``pixel_format``。)

   * Values returned by :c:func:`VIDIOC_ENUM_FRAMESIZES` for a coded pixel
     format will include all possible coded resolutions supported by the
     encoder for the given coded pixel format.
     (对于编码像素格式，VIDIOC_ENUM_FRAMESIZES 返回的值将包括编码器为该编码像素
     格式所支持的全部可能的编码分辨率。)

   * Values returned by :c:func:`VIDIOC_ENUM_FRAMESIZES` for a raw pixel format
     will include all possible frame buffer resolutions supported by the
     encoder for the given raw pixel format and coded format currently set on
     ``CAPTURE``.
     (对于 raw 像素格式，VIDIOC_ENUM_FRAMESIZES 返回的值将包括编码器为该 raw 像素
     格式以及 ``CAPTURE`` 上当前设置的编码格式所支持的全部可能的帧缓冲分辨率。)

4. The client may use :c:func:`VIDIOC_ENUM_FRAMEINTERVALS` to detect supported
   frame intervals for a given format and resolution, passing the desired pixel
   format in :c:type:`v4l2_frmivalenum` ``pixel_format`` and the resolution
   in :c:type:`v4l2_frmivalenum` ``width`` and :c:type:`v4l2_frmivalenum`
   ``height``.
   (客户端可以使用 VIDIOC_ENUM_FRAMEINTERVALS 来检测给定格式与分辨率支持的帧
   间隔，方法是把期望的像素格式传入 :c:type:`v4l2_frmivalenum` 的
   ``pixel_format``，并把分辨率传入 :c:type:`v4l2_frmivalenum` 的 ``width`` 与
   ``height``。)

   * Values returned by :c:func:`VIDIOC_ENUM_FRAMEINTERVALS` for a coded pixel
     format and coded resolution will include all possible frame intervals
     supported by the encoder for the given coded pixel format and resolution.
     (对于编码像素格式与编码分辨率，VIDIOC_ENUM_FRAMEINTERVALS 返回的值将包括
     编码器为给定的编码像素格式与分辨率所支持的全部可能的帧间隔。)

   * Values returned by :c:func:`VIDIOC_ENUM_FRAMEINTERVALS` for a raw pixel
     format and resolution will include all possible frame intervals supported
     by the encoder for the given raw pixel format and resolution and for the
     coded format, coded resolution and coded frame interval currently set on
     ``CAPTURE``.
     (对于 raw 像素格式与分辨率，VIDIOC_ENUM_FRAMEINTERVALS 返回的值将包括编码器
     为给定的 raw 像素格式与分辨率、以及 ``CAPTURE`` 上当前设置的编码格式、编码
     分辨率与编码帧间隔所支持的全部可能的帧间隔。)

   * Support for :c:func:`VIDIOC_ENUM_FRAMEINTERVALS` is optional. If it is
     not implemented, then there are no special restrictions other than the
     limits of the codec itself.
     (对 VIDIOC_ENUM_FRAMEINTERVALS 的支持是可选的。如果未实现，则除了编解码器
     本身的限制之外没有其他特殊限制。)

5. Supported profiles and levels for the coded format currently set on
   ``CAPTURE``, if applicable, may be queried using their respective controls
   via :c:func:`VIDIOC_QUERYCTRL`.
   (对于 ``CAPTURE`` 上当前设置的编码格式，其支持的 profile 与 level（如适用）
   可以通过 VIDIOC_QUERYCTRL 用各自的控件查询。)

6. Any additional encoder capabilities may be discovered by querying
   their respective controls.
   (任何其他编码器能力都可以通过查询其各自的控件来发现。)

Initialization(初始化)
======================

1. Set the coded format on the ``CAPTURE`` queue via :c:func:`VIDIOC_S_FMT`.
   (通过 VIDIOC_S_FMT 在 ``CAPTURE`` 队列上设置编码格式。)

   * **Required fields:**
     (必填字段：)

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
         (适用于 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举。)

     ``pixelformat``
         the coded format to be produced.
         (要生成的编码格式。)

     ``sizeimage``
         desired size of ``CAPTURE`` buffers; the encoder may adjust it to
         match hardware requirements.
         (``CAPTURE`` 缓冲的期望大小；编码器可以调整它以满足硬件要求。)

     ``width``, ``height``
         ignored (read-only).
         (忽略（只读）。)

     other fields
         follow standard semantics.
         (其他字段遵循标准语义。)

   * **Returned fields:**
     (返回字段：)

     ``sizeimage``
         adjusted size of ``CAPTURE`` buffers.
         (调整后的 ``CAPTURE`` 缓冲大小。)

     ``width``, ``height``
         the coded size selected by the encoder based on current state, e.g.
         ``OUTPUT`` format, selection rectangles, etc. (read-only).
         (编码器根据当前状态（如 ``OUTPUT`` 格式、选择矩形等）选择的编码大小
         （只读）。)

   .. important::

      Changing the ``CAPTURE`` format may change the currently set ``OUTPUT``
      format. How the new ``OUTPUT`` format is determined is up to the encoder
      and the client must ensure it matches its needs afterwards.
      (更改 ``CAPTURE`` 格式可能会更改当前设置的 ``OUTPUT`` 格式。新的 ``OUTPUT``
      格式如何确定由编码器决定，客户端必须确保其事后符合自己的需求。)

2. **Optional.** Enumerate supported ``OUTPUT`` formats (raw formats for
   source) for the selected coded format via :c:func:`VIDIOC_ENUM_FMT`.
   (可选。通过 VIDIOC_ENUM_FMT 为所选编码格式枚举受支持的 ``OUTPUT`` 格式
   （源 raw 格式）。)

   * **Required fields:**

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.

     other fields
         follow standard semantics.

   * **Returned fields:**

     ``pixelformat``
         raw format supported for the coded format currently selected on
         the ``CAPTURE`` queue.
         (当前 ``CAPTURE`` 队列上所选编码格式支持的 raw 格式。)

     other fields
         follow standard semantics.

3. Set the raw source format on the ``OUTPUT`` queue via
   :c:func:`VIDIOC_S_FMT`.
   (通过 VIDIOC_S_FMT 在 ``OUTPUT`` 队列上设置 raw 源格式。)

   * **Required fields:**

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
         (适用于 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举。)

     ``pixelformat``
         raw format of the source.
         (源的 raw 格式。)

     ``width``, ``height``
         source resolution.
         (源分辨率。)

     other fields
         follow standard semantics.

   * **Returned fields:**

     ``width``, ``height``
         may be adjusted to match encoder minimums, maximums and alignment
         requirements, as required by the currently selected formats, as
         reported by :c:func:`VIDIOC_ENUM_FRAMESIZES`.
         (可以调整以匹配编码器的最小值、最大值与对齐要求，具体取决于当前所选格式，
         如 VIDIOC_ENUM_FRAMESIZES 所报告的那样。)

     other fields
         follow standard semantics.

   * Setting the ``OUTPUT`` format will reset the selection rectangles to their
     default values, based on the new resolution, as described in the next
     step.
     (设置 ``OUTPUT`` 格式会把选择矩形重置为基于新分辨率的默认值，如下一步所述。)

4. Set the raw frame interval on the ``OUTPUT`` queue via
   :c:func:`VIDIOC_S_PARM`. This also sets the coded frame interval on the
   ``CAPTURE`` queue to the same value.
   (通过 VIDIOC_S_PARM 设置 ``OUTPUT`` 队列上的 raw 帧间隔。这也会把 ``CAPTURE``
   队列上的编码帧间隔设置为相同的值。)

   * **Required fields:**

     ``type``
	 a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
	 (适用于 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举。)

     ``parm.output``
	 set all fields except ``parm.output.timeperframe`` to 0.
	 (把除 ``parm.output.timeperframe`` 之外的所有字段设为 0。)

     ``parm.output.timeperframe``
	 the desired frame interval; the encoder may adjust it to
	 match hardware requirements.
	 (期望的帧间隔；编码器可以调整它以满足硬件要求。)

   * **Returned fields:**

     ``parm.output.timeperframe``
	 the adjusted frame interval.
	 (调整后的帧间隔。)

   .. important::

      Changing the ``OUTPUT`` frame interval *also* sets the framerate that
      the encoder uses to encode the video. So setting the frame interval
      to 1/24 (or 24 frames per second) will produce a coded video stream
      that can be played back at that speed. The frame interval for the
      ``OUTPUT`` queue is just a hint, the application may provide raw
      frames at a different rate. It can be used by the driver to help
      schedule multiple encoders running in parallel.
      (更改 ``OUTPUT`` 帧间隔 *同时* 也会设置编码器用来编码视频的帧率。因此，把帧
      间隔设置为 1/24（即每秒 24 帧）将产生一个可以按该速度回放的编码视频流。
      ``OUTPUT`` 队列的帧间隔只是一个提示，应用可以以不同的速率提供 raw 帧。驱动
      可以用它来帮助调度多个并行运行的编码器。)

      In the next step the ``CAPTURE`` frame interval can optionally be
      changed to a different value. This is useful for off-line encoding
      were the coded frame interval can be different from the rate at
      which raw frames are supplied.
      (在下一步中，可以选择把 ``CAPTURE`` 帧间隔更改为不同的值。这对于离线编码
      很有用——编码帧间隔可以与提供 raw 帧的速率不同。)

   .. important::

      ``timeperframe`` deals with *frames*, not fields. So for interlaced
      formats this is the time per two fields, since a frame consists of
      a top and a bottom field.
      (``timeperframe`` 针对的是 *帧*，而不是场（field）。因此对于隔行扫描格式，
      这是两场的时间，因为一帧由顶场与底场组成。)

   .. note::

      It is due to historical reasons that changing the ``OUTPUT`` frame
      interval also changes the coded frame interval on the ``CAPTURE``
      queue. Ideally these would be independent settings, but that would
      break the existing API.
      (出于历史原因，更改 ``OUTPUT`` 帧间隔也会更改 ``CAPTURE`` 队列上的编码帧
      间隔。理想情况下这两者应是独立的设置，但那样会破坏现有 API。)

5. **Optional** Set the coded frame interval on the ``CAPTURE`` queue via
   :c:func:`VIDIOC_S_PARM`. This is only necessary if the coded frame
   interval is different from the raw frame interval, which is typically
   the case for off-line encoding. Support for this feature is signalled
   by the :ref:`V4L2_FMT_FLAG_ENC_CAP_FRAME_INTERVAL <fmtdesc-flags>` format flag.
   (可选。通过 VIDIOC_S_PARM 设置 ``CAPTURE`` 队列上的编码帧间隔。仅当编码帧间隔
   与 raw 帧间隔不同时才需要这样做——离线编码通常就是这种情况。是否支持此特性由
   :ref:`V4L2_FMT_FLAG_ENC_CAP_FRAME_INTERVAL <fmtdesc-flags>` 格式标志标示。)

   * **Required fields:**

     ``type``
	 a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
	 (适用于 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举。)

     ``parm.capture``
	 set all fields except ``parm.capture.timeperframe`` to 0.
	 (把除 ``parm.capture.timeperframe`` 之外的所有字段设为 0。)

     ``parm.capture.timeperframe``
	 the desired coded frame interval; the encoder may adjust it to
	 match hardware requirements.
	 (期望的编码帧间隔；编码器可以调整它以满足硬件要求。)

   * **Returned fields:**

     ``parm.capture.timeperframe``
	 the adjusted frame interval.
	 (调整后的帧间隔。)

   .. important::

      Changing the ``CAPTURE`` frame interval sets the framerate for the
      coded video. It does *not* set the rate at which buffers arrive on the
      ``CAPTURE`` queue, that depends on how fast the encoder is and how
      fast raw frames are queued on the ``OUTPUT`` queue.
      (更改 ``CAPTURE`` 帧间隔会设置编码视频的帧率。它 *不会* 设置缓冲到达
      ``CAPTURE`` 队列的速率——那取决于编码器的速度以及 raw 帧在 ``OUTPUT`` 队列上
      入队的速度。)

   .. important::

      ``timeperframe`` deals with *frames*, not fields. So for interlaced
      formats this is the time per two fields, since a frame consists of
      a top and a bottom field.
      (``timeperframe`` 针对的是 *帧*，而不是场（field）。因此对于隔行扫描格式，
      这是两场的时间，因为一帧由顶场与底场组成。)

   .. note::

      Not all drivers support this functionality, in that case just set
      the desired coded frame interval for the ``OUTPUT`` queue.
      (并非所有驱动都支持此功能；这种情况下，只需为 ``OUTPUT`` 队列设置期望的
      编码帧间隔。)

      However, drivers that can schedule multiple encoders based on the
      ``OUTPUT`` frame interval must support this optional feature.
      (不过，能够基于 ``OUTPUT`` 帧间隔调度多个编码器的驱动必须支持这一可选
      特性。)

6. **Optional.** Set the visible resolution for the stream metadata via
   :c:func:`VIDIOC_S_SELECTION` on the ``OUTPUT`` queue if it is desired
   to be different than the full OUTPUT resolution.
   (可选。如果希望流元数据的可见分辨率与完整的 OUTPUT 分辨率不同，可通过
   VIDIOC_S_SELECTION 在 ``OUTPUT`` 队列上设置它。)

   * **Required fields:**

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
         (适用于 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举。)

     ``target``
         set to ``V4L2_SEL_TGT_CROP``.
         (设置为 ``V4L2_SEL_TGT_CROP``。)

     ``r.left``, ``r.top``, ``r.width``, ``r.height``
         visible rectangle; this must fit within the `V4L2_SEL_TGT_CROP_BOUNDS`
         rectangle and may be subject to adjustment to match codec and
         hardware constraints.
         (可见矩形；必须位于 V4L2_SEL_TGT_CROP_BOUNDS 矩形之内，并且可能被调整
         以匹配编解码器与硬件约束。)

   * **Returned fields:**

     ``r.left``, ``r.top``, ``r.width``, ``r.height``
         visible rectangle adjusted by the encoder.
         (编码器调整后的可见矩形。)

   * The following selection targets are supported on ``OUTPUT``:
     (``OUTPUT`` 上支持以下选择目标：)

     ``V4L2_SEL_TGT_CROP_BOUNDS``
         equal to the full source frame, matching the active ``OUTPUT``
         format.
         (等于完整的源帧，与当前活动的 ``OUTPUT`` 格式匹配。)

     ``V4L2_SEL_TGT_CROP_DEFAULT``
         equal to ``V4L2_SEL_TGT_CROP_BOUNDS``.
         (等于 ``V4L2_SEL_TGT_CROP_BOUNDS``。)

     ``V4L2_SEL_TGT_CROP``
         rectangle within the source buffer to be encoded into the
         ``CAPTURE`` stream; defaults to ``V4L2_SEL_TGT_CROP_DEFAULT``.
         (源缓冲内将被编码进 ``CAPTURE`` 流的矩形；默认为
         ``V4L2_SEL_TGT_CROP_DEFAULT``。)

         .. note::

            A common use case for this selection target is encoding a source
            video with a resolution that is not a multiple of a macroblock,
            e.g.  the common 1920x1080 resolution may require the source
            buffers to be aligned to 1920x1088 for codecs with 16x16 macroblock
            size. To avoid encoding the padding, the client needs to explicitly
            configure this selection target to 1920x1080.
            (该选择目标的一个常见用例是编码分辨率不是宏块整数倍的源视频。例如，
            常见的 1920x1080 分辨率，对于宏块大小为 16x16 的编解码器，可能要求
            源缓冲按 1920x1088 对齐。为避免编码填充内容，客户端需要显式地把该
            选择目标配置为 1920x1080。)

   .. warning::

      The encoder may adjust the crop/compose rectangles to the nearest
      supported ones to meet codec and hardware requirements. The client needs
      to check the adjusted rectangle returned by :c:func:`VIDIOC_S_SELECTION`.
      (编码器可以把裁剪/合成矩形调整到最近的受支持值，以满足编解码器与硬件要求。
      客户端需要检查 VIDIOC_S_SELECTION 返回的调整后的矩形。)

7. Allocate buffers for both ``OUTPUT`` and ``CAPTURE`` via
   :c:func:`VIDIOC_REQBUFS`. This may be performed in any order.
   (通过 VIDIOC_REQBUFS 为 ``OUTPUT`` 与 ``CAPTURE`` 分配缓冲。可以按任意顺序
   执行。)

   * **Required fields:**

     ``count``
         requested number of buffers to allocate; greater than zero.

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT`` or
         ``CAPTURE``.
         (适用于 ``OUTPUT`` 或 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举。)

     other fields
         follow standard semantics.

   * **Returned fields:**

     ``count``
          actual number of buffers allocated.
          (实际分配的缓冲数量。)

   .. warning::

      The actual number of allocated buffers may differ from the ``count``
      given. The client must check the updated value of ``count`` after the
      call returns.
      (实际分配的缓冲数量可能与给定的 ``count`` 不同。客户端必须在调用返回后检查
      更新后的 ``count`` 值。)

   .. note::

      To allocate more than the minimum number of OUTPUT buffers (for pipeline
      depth), the client may query the ``V4L2_CID_MIN_BUFFERS_FOR_OUTPUT``
      control to get the minimum number of buffers required, and pass the
      obtained value plus the number of additional buffers needed in the
      ``count`` field to :c:func:`VIDIOC_REQBUFS`.
      (要分配多于最少数量的 OUTPUT 缓冲（为了流水线深度），客户端可以查询
      ``V4L2_CID_MIN_BUFFERS_FOR_OUTPUT`` 控件来获得所需的最小缓冲数量，并把所得
      值加上所需的额外缓冲数量作为 ``count`` 字段传给 VIDIOC_REQBUFS。)

   Alternatively, :c:func:`VIDIOC_CREATE_BUFS` can be used to have more
   control over buffer allocation.
   (或者，也可以使用 VIDIOC_CREATE_BUFS 来对缓冲分配进行更多控制。)

   * **Required fields:**

     ``count``
         requested number of buffers to allocate; greater than zero.
         (请求分配的缓冲数量；大于零。)

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.

     other fields
         follow standard semantics.

   * **Returned fields:**

     ``count``
         adjusted to the number of allocated buffers.
         (调整为已分配缓冲的数量。)

8. Begin streaming on both ``OUTPUT`` and ``CAPTURE`` queues via
   :c:func:`VIDIOC_STREAMON`. This may be performed in any order. The actual
   encoding process starts when both queues start streaming.
   (通过 VIDIOC_STREAMON 在 ``OUTPUT`` 与 ``CAPTURE`` 两个队列上开始流式传输。
   可以按任意顺序执行。当两个队列都开始流式传输时，实际的编码过程才会开始。)

.. note::

   If the client stops the ``CAPTURE`` queue during the encode process and then
   restarts it again, the encoder will begin generating a stream independent
   from the stream generated before the stop. The exact constraints depend
   on the coded format, but may include the following implications:
   (如果客户端在编码过程中停止 ``CAPTURE`` 队列然后重新启动，编码器将开始生成一个
   独立于停止前所生成流的流。具体约束取决于编码格式，但可能包括以下含义：)

   * encoded frames produced after the restart must not reference any
     frames produced before the stop, e.g. no long term references for
     H.264/HEVC,
     (重启后产生的编码帧不得引用停止前产生的任何帧，例如 H.264/HEVC 不使用长期
     参考帧，)

   * any headers that must be included in a standalone stream must be
     produced again, e.g. SPS and PPS for H.264/HEVC.
     (必须包含在独立流中的任何头都必须重新生成，例如 H.264/HEVC 的 SPS 与 PPS。)

Encoding(编码)
==============

This state is reached after the `Initialization(初始化)` sequence finishes
successfully.  In this state, the client queues and dequeues buffers to both
queues via :c:func:`VIDIOC_QBUF` and :c:func:`VIDIOC_DQBUF`, following the
standard semantics.
(在 `Initialization(初始化)` 序列成功完成之后进入此状态。在此状态下，客户端
按照标准语义通过 VIDIOC_QBUF 与 VIDIOC_DQBUF 向两个队列入队和出队缓冲。)

The content of encoded ``CAPTURE`` buffers depends on the active coded pixel
format and may be affected by codec-specific extended controls, as stated
in the documentation of each format.
(编码后的 ``CAPTURE`` 缓冲的内容取决于当前活动的编码像素格式，并可能受到
编解码器特定扩展控件的影响，如每种格式的文档所述。)

Both queues operate independently, following standard behavior of V4L2 buffer
queues and memory-to-memory devices. In addition, the order of encoded frames
dequeued from the ``CAPTURE`` queue may differ from the order of queuing raw
frames to the ``OUTPUT`` queue, due to properties of the selected coded format,
e.g. frame reordering.
(两个队列独立运作，遵循 V4L2 缓冲队列与内存到内存设备的标准行为。此外，由于
所选编码格式的特性（例如帧重排序），从 ``CAPTURE`` 队列出队的编码帧的顺序可能
不同于向 ``OUTPUT`` 队列入队 raw 帧的顺序。)

The client must not assume any direct relationship between ``CAPTURE`` and
``OUTPUT`` buffers and any specific timing of buffers becoming
available to dequeue. Specifically:
(客户端不得假设 ``CAPTURE`` 与 ``OUTPUT`` 缓冲之间存在任何直接关系，也不得假设
缓冲变为可出队状态的任何特定时序。具体来说：)

* a buffer queued to ``OUTPUT`` may result in more than one buffer produced on
  ``CAPTURE`` (for example, if returning an encoded frame allowed the encoder
  to return a frame that preceded it in display, but succeeded it in the decode
  order; however, there may be other reasons for this as well),
  (一个入队到 ``OUTPUT`` 的缓冲可能在 ``CAPTURE`` 上产生多于一个缓冲（例如，如果
  返回一个编码帧使编码器得以返回一个在显示顺序上先于它、但在解码顺序上后于它的
  帧；不过也可能有其他原因），)

* a buffer queued to ``OUTPUT`` may result in a buffer being produced on
  ``CAPTURE`` later into encode process, and/or after processing further
  ``OUTPUT`` buffers, or be returned out of order, e.g. if display
  reordering is used,
  (一个入队到 ``OUTPUT`` 的缓冲可能要到编码过程的稍后时刻、和/或在处理了更多
  ``OUTPUT`` 缓冲之后才在 ``CAPTURE`` 上产生缓冲，或者乱序返回，例如在使用显示
  重排序时，)

* buffers may become available on the ``CAPTURE`` queue without additional
  buffers queued to ``OUTPUT`` (e.g. during drain or ``EOS``), because of the
  ``OUTPUT`` buffers queued in the past whose encoding results are only
  available at later time, due to specifics of the encoding process,
  (``CAPTURE`` 队列上可能在没有额外 ``OUTPUT`` 缓冲入队的情况下出现可用的缓冲
  （例如在排空或 ``EOS`` 期间），这是由于过去入队的 ``OUTPUT`` 缓冲的编码结果因
  编码过程的特性而只在稍后时刻才可用，)

* buffers queued to ``OUTPUT`` may not become available to dequeue instantly
  after being encoded into a corresponding ``CAPTURE`` buffer, e.g. if the
  encoder needs to use the frame as a reference for encoding further frames.
  (入队到 ``OUTPUT`` 的缓冲在被编码进对应的 ``CAPTURE`` 缓冲之后，可能不会立即
  可供出队，例如编码器需要把该帧用作编码后续帧的参考。)

.. note::

   To allow matching encoded ``CAPTURE`` buffers with ``OUTPUT`` buffers they
   originated from, the client can set the ``timestamp`` field of the
   :c:type:`v4l2_buffer` struct when queuing an ``OUTPUT`` buffer. The
   ``CAPTURE`` buffer(s), which resulted from encoding that ``OUTPUT`` buffer
   will have their ``timestamp`` field set to the same value when dequeued.
   (为了把编码后的 ``CAPTURE`` 缓冲与其来源的 ``OUTPUT`` 缓冲匹配起来，客户端可以在
   ``OUTPUT`` 缓冲入队时设置 :c:type:`v4l2_buffer` 结构的 ``timestamp`` 字段。由
   该 ``OUTPUT`` 缓冲编码产生的 ``CAPTURE`` 缓冲在出队时其 ``timestamp`` 字段将被
   设置为相同的值。)

   In addition to the straightforward case of one ``OUTPUT`` buffer producing
   one ``CAPTURE`` buffer, the following cases are defined:
   (除了一个 ``OUTPUT`` 缓冲产生一个 ``CAPTURE`` 缓冲这种直接情形之外，还定义了
   以下情形：)

   * one ``OUTPUT`` buffer generates multiple ``CAPTURE`` buffers: the same
     ``OUTPUT`` timestamp will be copied to multiple ``CAPTURE`` buffers,
     (一个 ``OUTPUT`` 缓冲生成多个 ``CAPTURE`` 缓冲：同一个 ``OUTPUT`` 时间戳会被
     复制到多个 ``CAPTURE`` 缓冲上，)

   * the encoding order differs from the presentation order (i.e. the
     ``CAPTURE`` buffers are out-of-order compared to the ``OUTPUT`` buffers):
     ``CAPTURE`` timestamps will not retain the order of ``OUTPUT`` timestamps.
     (编码顺序与呈现顺序不同（即 ``CAPTURE`` 缓冲相对于 ``OUTPUT`` 缓冲是乱序的）：
     ``CAPTURE`` 时间戳将不保持 ``OUTPUT`` 时间戳的顺序。)

.. note::

   To let the client distinguish between frame types (keyframes, intermediate
   frames; the exact list of types depends on the coded format), the
   ``CAPTURE`` buffers will have corresponding flag bits set in their
   :c:type:`v4l2_buffer` struct when dequeued. See the documentation of
   :c:type:`v4l2_buffer` and each coded pixel format for exact list of flags
   and their meanings.
   (为了让客户端能够区分帧类型（关键帧、中间帧；确切类型列表取决于编码格式），
   ``CAPTURE`` 缓冲在出队时会在其 :c:type:`v4l2_buffer` 结构中设置相应的标志位。
   确切标志列表及其含义见 :c:type:`v4l2_buffer` 与每种编码像素格式的文档。)

Should an encoding error occur, it will be reported to the client with the level
of details depending on the encoder capabilities. Specifically:
(如果发生编码错误，将以取决于编码器能力的详细程度报告给客户端。具体来说：)

* the ``CAPTURE`` buffer (if any) that contains the results of the failed encode
  operation will be returned with the ``V4L2_BUF_FLAG_ERROR`` flag set,
  (包含失败编码操作结果的 ``CAPTURE`` 缓冲（如果有的话）将带着
  ``V4L2_BUF_FLAG_ERROR`` 标志返回，)

* if the encoder is able to precisely report the ``OUTPUT`` buffer(s) that triggered
  the error, such buffer(s) will be returned with the ``V4L2_BUF_FLAG_ERROR`` flag
  set.
  (如果编码器能够精确报告触发错误的 ``OUTPUT`` 缓冲，这样的缓冲将带着
  ``V4L2_BUF_FLAG_ERROR`` 标志返回。)

.. note::

   If a ``CAPTURE`` buffer is too small then it is just returned with the
   ``V4L2_BUF_FLAG_ERROR`` flag set. More work is needed to detect that this
   error occurred because the buffer was too small, and to provide support to
   free existing buffers that were too small.
   (如果 ``CAPTURE`` 缓冲太小，它只会带着 ``V4L2_BUF_FLAG_ERROR`` 标志返回。要检测
   出这个错误是因为缓冲太小而发生、并提供对释放过小的既有缓冲的支持，还需要做
   更多工作。)

In case of a fatal failure that does not allow the encoding to continue, any
further operations on corresponding encoder file handle will return the -EIO
error code. The client may close the file handle and open a new one, or
alternatively reinitialize the instance by stopping streaming on both queues,
releasing all buffers and performing the Initialization sequence again.
(如果发生不允许编码继续的致命故障，对相应编码器文件句柄的任何后续操作都将返回
-EIO 错误码。客户端可以关闭该文件句柄并打开一个新句柄，或者通过停止两个队列上的
流式传输、释放所有缓冲并重新执行初始化序列来重新初始化该实例。)

Encoding Parameter Changes(编码参数变更)
========================================

The client is allowed to use :c:func:`VIDIOC_S_CTRL` to change encoder
parameters at any time. The availability of parameters is encoder-specific
and the client must query the encoder to find the set of available controls.
(允许客户端随时使用 VIDIOC_S_CTRL 更改编码器参数。参数的可用性因编码器而异，
客户端必须查询编码器以找出可用控件集合。)

The ability to change each parameter during encoding is encoder-specific, as
per the standard semantics of the V4L2 control interface. The client may
attempt to set a control during encoding and if the operation fails with the
-EBUSY error code, the ``CAPTURE`` queue needs to be stopped for the
configuration change to be allowed. To do this, it may follow the `Drain(排空)`
(能否在编码过程中更改每个参数取决于编码器，这遵循 V4L2 控制接口的标准语义。
客户端可以尝试在编码过程中设置控件，如果操作以 -EBUSY 错误码失败，则需要停止
``CAPTURE`` 队列才允许进行配置更改。为此，可以遵循 `Drain(排空)` 序列，以避免
丢失已入队/已编码的帧。)
sequence to avoid losing the already queued/encoded frames.

The timing of parameter updates is encoder-specific, as per the standard
semantics of the V4L2 control interface. If the client needs to apply the
parameters exactly at specific frame, using the Request API
(:ref:`media-request-api`) should be considered, if supported by the encoder.
(参数更新的时机取决于编码器，这遵循 V4L2 控制接口的标准语义。如果客户端需要在
特定帧上精确应用参数，应考虑使用 Request API（:ref:`media-request-api`）（如果
编码器支持）。)

Drain(排空)
===========

To ensure that all the queued ``OUTPUT`` buffers have been processed and the
related ``CAPTURE`` buffers are given to the client, the client must follow the
drain sequence described below. After the drain sequence ends, the client has
received all encoded frames for all ``OUTPUT`` buffers queued before the
sequence was started.
(为确保所有已入队的 ``OUTPUT`` 缓冲都已被处理、且相关的 ``CAPTURE`` 缓冲已交付
给客户端，客户端必须遵循下述排空序列。排空序列结束后，客户端已收到序列开始之前
入队的所有 ``OUTPUT`` 缓冲的全部编码帧。)

1. Begin the drain sequence by issuing :c:func:`VIDIOC_ENCODER_CMD`.
   (通过发出 VIDIOC_ENCODER_CMD 开始排空序列。)

   * **Required fields:**

     ``cmd``
         set to ``V4L2_ENC_CMD_STOP``.
         (设置为 ``V4L2_ENC_CMD_STOP``。)

     ``flags``
         set to 0.
         (设置为 0。)

     ``pts``
         set to 0.

   .. warning::

      The sequence can be only initiated if both ``OUTPUT`` and ``CAPTURE``
      queues are streaming. For compatibility reasons, the call to
      :c:func:`VIDIOC_ENCODER_CMD` will not fail even if any of the queues is
      not streaming, but at the same time it will not initiate the `Drain(排空)`
      sequence and so the steps described below would not be applicable.
      (只有当 ``OUTPUT`` 与 ``CAPTURE`` 队列都在流式传输时才能启动该序列。出于
      兼容性原因，即使任一队列未在流式传输，调用 VIDIOC_ENCODER_CMD 也不会失败，
      但同时它也不会启动 `Drain(排空)` 序列，因此下述步骤将不适用。)

2. Any ``OUTPUT`` buffers queued by the client before the
   :c:func:`VIDIOC_ENCODER_CMD` was issued will be processed and encoded as
   normal. The client must continue to handle both queues independently,
   similarly to normal encode operation. This includes:
   (在发出 VIDIOC_ENCODER_CMD 之前由客户端入队的所有 ``OUTPUT`` 缓冲将照常被处理
   和编码。客户端必须继续独立处理两个队列，与正常编码操作类似。这包括：)

   * queuing and dequeuing ``CAPTURE`` buffers, until a buffer marked with the
     ``V4L2_BUF_FLAG_LAST`` flag is dequeued,
     (入队与出队 ``CAPTURE`` 缓冲，直到一个标记了 ``V4L2_BUF_FLAG_LAST`` 标志的
     缓冲被出队，)

     .. warning::

        The last buffer may be empty (with :c:type:`v4l2_buffer`
        ``bytesused`` = 0) and in that case it must be ignored by the client,
        as it does not contain an encoded frame.
        (最后一个缓冲可能为空（:c:type:`v4l2_buffer` 的 ``bytesused`` = 0），这种
        情况下客户端必须忽略它，因为它不包含编码帧。)

     .. note::

        Any attempt to dequeue more ``CAPTURE`` buffers beyond the buffer
        marked with ``V4L2_BUF_FLAG_LAST`` will result in a -EPIPE error from
        :c:func:`VIDIOC_DQBUF`.
        (任何在标记了 ``V4L2_BUF_FLAG_LAST`` 的缓冲之后出队更多 ``CAPTURE`` 缓冲
        的尝试，都会导致 VIDIOC_DQBUF 返回 -EPIPE 错误。)

   * dequeuing processed ``OUTPUT`` buffers, until all the buffers queued
     before the ``V4L2_ENC_CMD_STOP`` command are dequeued,
     (出队已处理的 ``OUTPUT`` 缓冲，直到 ``V4L2_ENC_CMD_STOP`` 命令之前入队的所有
     缓冲都被出队，)

   * dequeuing the ``V4L2_EVENT_EOS`` event, if the client subscribes to it.
     (如果客户端订阅了 ``V4L2_EVENT_EOS`` 事件，则出队该事件。)

   .. note::

      For backwards compatibility, the encoder will signal a ``V4L2_EVENT_EOS``
      event when the last frame has been encoded and all frames are ready to be
      dequeued. It is deprecated behavior and the client must not rely on it.
      The ``V4L2_BUF_FLAG_LAST`` buffer flag should be used instead.
      (出于向后兼容性，当最后一帧编码完成、所有帧都可供出队时，编码器会发出
      ``V4L2_EVENT_EOS`` 事件。这是已弃用的行为，客户端不得依赖它。应当改用
      ``V4L2_BUF_FLAG_LAST`` 缓冲标志。)

3. Once all ``OUTPUT`` buffers queued before the ``V4L2_ENC_CMD_STOP`` call are
   dequeued and the last ``CAPTURE`` buffer is dequeued, the encoder is stopped
   and it will accept, but not process any newly queued ``OUTPUT`` buffers
   until the client issues any of the following operations:
   (一旦 ``V4L2_ENC_CMD_STOP`` 调用之前入队的所有 ``OUTPUT`` 缓冲都已出队、且最后
   一个 ``CAPTURE`` 缓冲已出队，编码器就停止了；它会接受、但不处理任何新入队的
   ``OUTPUT`` 缓冲，直到客户端发出以下任一操作：)

   * ``V4L2_ENC_CMD_START`` - the encoder will not be reset and will resume
     operation normally, with all the state from before the drain,
     (``V4L2_ENC_CMD_START``——编码器不会被复位，将带着排空之前的全部状态恢复
     正常运行，)

   * a pair of :c:func:`VIDIOC_STREAMOFF` and :c:func:`VIDIOC_STREAMON` on the
     ``CAPTURE`` queue - the encoder will be reset (see the `Reset(复位)` sequence)
     and then resume encoding,
     (在 ``CAPTURE`` 队列上成对执行 VIDIOC_STREAMOFF 与 VIDIOC_STREAMON——编码器
     将被复位（见 `Reset(复位)` 序列）然后恢复编码，)

   * a pair of :c:func:`VIDIOC_STREAMOFF` and :c:func:`VIDIOC_STREAMON` on the
     ``OUTPUT`` queue - the encoder will resume operation normally, however any
     source frames queued to the ``OUTPUT`` queue between ``V4L2_ENC_CMD_STOP``
     and :c:func:`VIDIOC_STREAMOFF` will be discarded.
     (在 ``OUTPUT`` 队列上成对执行 VIDIOC_STREAMOFF 与 VIDIOC_STREAMON——编码器
     将恢复正常运行，但 ``V4L2_ENC_CMD_STOP`` 与 VIDIOC_STREAMOFF 之间入队到
     ``OUTPUT`` 队列的任何源帧都会被丢弃。)

.. note::

   Once the drain sequence is initiated, the client needs to drive it to
   completion, as described by the steps above, unless it aborts the process by
   issuing :c:func:`VIDIOC_STREAMOFF` on any of the ``OUTPUT`` or ``CAPTURE``
   queues.  The client is not allowed to issue ``V4L2_ENC_CMD_START`` or
   ``V4L2_ENC_CMD_STOP`` again while the drain sequence is in progress and they
   will fail with -EBUSY error code if attempted.
   (一旦排空序列被启动，客户端就需要按上述步骤把它驱动到完成，除非它通过在任一
   ``OUTPUT`` 或 ``CAPTURE`` 队列上发出 VIDIOC_STREAMOFF 而中止该过程。在排空
   序列进行期间，不允许客户端再次发出 ``V4L2_ENC_CMD_START`` 或
   ``V4L2_ENC_CMD_STOP``；如果尝试，将以 -EBUSY 错误码失败。)

   For reference, handling of various corner cases is described below:
   (作为参考，各种边角情形的处理如下：)

   * In case of no buffer in the ``OUTPUT`` queue at the time the
     ``V4L2_ENC_CMD_STOP`` command was issued, the drain sequence completes
     immediately and the encoder returns an empty ``CAPTURE`` buffer with the
     ``V4L2_BUF_FLAG_LAST`` flag set.
     (如果在发出 ``V4L2_ENC_CMD_STOP`` 命令时 ``OUTPUT`` 队列中没有缓冲，排空
     序列立即完成，编码器返回一个设置了 ``V4L2_BUF_FLAG_LAST`` 标志的空
     ``CAPTURE`` 缓冲。)

   * In case of no buffer in the ``CAPTURE`` queue at the time the drain
     sequence completes, the next time the client queues a ``CAPTURE`` buffer
     it is returned at once as an empty buffer with the ``V4L2_BUF_FLAG_LAST``
     flag set.
     (如果排空序列完成时 ``CAPTURE`` 队列中没有缓冲，客户端下次入队 ``CAPTURE``
     缓冲时，该缓冲会立即作为一个设置了 ``V4L2_BUF_FLAG_LAST`` 标志的空缓冲
     返回。)

   * If :c:func:`VIDIOC_STREAMOFF` is called on the ``CAPTURE`` queue in the
     middle of the drain sequence, the drain sequence is canceled and all
     ``CAPTURE`` buffers are implicitly returned to the client.
     (如果在排空序列进行中对 ``CAPTURE`` 队列调用了 VIDIOC_STREAMOFF，排空序列
     被取消，所有 ``CAPTURE`` 缓冲被隐式返回给客户端。)

   * If :c:func:`VIDIOC_STREAMOFF` is called on the ``OUTPUT`` queue in the
     middle of the drain sequence, the drain sequence completes immediately and
     next ``CAPTURE`` buffer will be returned empty with the
     ``V4L2_BUF_FLAG_LAST`` flag set.
     (如果在排空序列进行中对 ``OUTPUT`` 队列调用了 VIDIOC_STREAMOFF，排空序列
     立即完成，下一个 ``CAPTURE`` 缓冲将作为设置了 ``V4L2_BUF_FLAG_LAST`` 标志的
     空缓冲返回。)

   Although not mandatory, the availability of encoder commands may be queried
   using :c:func:`VIDIOC_TRY_ENCODER_CMD`.
   (尽管不是强制要求，编码器命令的可用性可以通过 VIDIOC_TRY_ENCODER_CMD 查询。)

Reset(复位)
===========

The client may want to request the encoder to reinitialize the encoding, so
that the following stream data becomes independent from the stream data
generated before. Depending on the coded format, that may imply that:
(客户端可能希望请求编码器重新初始化编码，使接下来的流数据独立于之前生成的流数据。
根据编码格式，这可能意味着：)

* encoded frames produced after the restart must not reference any frames
  produced before the stop, e.g. no long term references for H.264/HEVC,
  (重启后产生的编码帧不得引用停止前产生的任何帧，例如 H.264/HEVC 不使用长期参考
  帧，)

* any headers that must be included in a standalone stream must be produced
  again, e.g. SPS and PPS for H.264/HEVC.
  (必须包含在独立流中的任何头都必须重新生成，例如 H.264/HEVC 的 SPS 与 PPS。)

This can be achieved by performing the reset sequence.
(这可以通过执行复位序列来实现。)

1. Perform the `Drain(排空)` sequence to ensure all the in-flight encoding finishes
   and respective buffers are dequeued.
   (执行 `Drain(排空)` 序列，以确保所有进行中的编码完成、相应缓冲被出队。)

2. Stop streaming on the ``CAPTURE`` queue via :c:func:`VIDIOC_STREAMOFF`. This
   will return all currently queued ``CAPTURE`` buffers to the client, without
   valid frame data.
   (通过 VIDIOC_STREAMOFF 停止 ``CAPTURE`` 队列上的流式传输。这会把当前已入队的
   所有 ``CAPTURE`` 缓冲返回给客户端，但不带有效帧数据。)

3. Start streaming on the ``CAPTURE`` queue via :c:func:`VIDIOC_STREAMON` and
   continue with regular encoding sequence. The encoded frames produced into
   ``CAPTURE`` buffers from now on will contain a standalone stream that can be
   decoded without the need for frames encoded before the reset sequence,
   starting at the first ``OUTPUT`` buffer queued after issuing the
   `V4L2_ENC_CMD_STOP` of the `Drain(排空)` sequence.
   (通过 VIDIOC_STREAMON 在 ``CAPTURE`` 队列上开始流式传输，并继续常规编码序列。
   从现在起编码进 ``CAPTURE`` 缓冲的编码帧将包含一个独立流，无需复位序列之前编码
   的帧即可解码，从发出 `Drain(排空)` 序列的 ``V4L2_ENC_CMD_STOP`` 之后入队的第一
   个 ``OUTPUT`` 缓冲开始。)

This sequence may be also used to change encoding parameters for encoders
without the ability to change the parameters on the fly.
(对于不支持在运行中更改参数的编码器，该序列也可用于更改编码参数。)

Commit Points(提交点)
=====================

Setting formats and allocating buffers triggers changes in the behavior of the
encoder.
(设置格式与分配缓冲会触发编码器行为的变更。)

1. Setting the format on the ``CAPTURE`` queue may change the set of formats
   supported/advertised on the ``OUTPUT`` queue. In particular, it also means
   that the ``OUTPUT`` format may be reset and the client must not rely on the
   previously set format being preserved.
   (在 ``CAPTURE`` 队列上设置格式可能会更改 ``OUTPUT`` 队列上支持/宣告的格式集合。
   特别地，这也意味着 ``OUTPUT`` 格式可能被重置，客户端不得依赖先前设置的格式会
   被保留。)

2. Enumerating formats on the ``OUTPUT`` queue always returns only formats
   supported for the current ``CAPTURE`` format.
   (在 ``OUTPUT`` 队列上枚举格式总是只返回当前 ``CAPTURE`` 格式所支持的格式。)

3. Setting the format on the ``OUTPUT`` queue does not change the list of
   formats available on the ``CAPTURE`` queue. An attempt to set the ``OUTPUT``
   format that is not supported for the currently selected ``CAPTURE`` format
   will result in the encoder adjusting the requested ``OUTPUT`` format to a
   supported one.
   (在 ``OUTPUT`` 队列上设置格式不会更改 ``CAPTURE`` 队列上可用的格式列表。尝试
   设置当前所选 ``CAPTURE`` 格式不支持的 ``OUTPUT`` 格式，将导致编码器把请求的
   ``OUTPUT`` 格式调整为受支持的格式。)

4. Enumerating formats on the ``CAPTURE`` queue always returns the full set of
   supported coded formats, irrespective of the current ``OUTPUT`` format.
   (在 ``CAPTURE`` 队列上枚举格式总是返回受支持编码格式的完整集合，与当前
   ``OUTPUT`` 格式无关。)

5. While buffers are allocated on any of the ``OUTPUT`` or ``CAPTURE`` queues,
   the client must not change the format on the ``CAPTURE`` queue. Drivers will
   return the -EBUSY error code for any such format change attempt.
   (当 ``OUTPUT`` 或 ``CAPTURE`` 任一队列上已分配缓冲时，客户端不得更改
   ``CAPTURE`` 队列上的格式。驱动将对任何此类格式更改尝试返回 -EBUSY 错误码。)

To summarize, setting formats and allocation must always start with the
``CAPTURE`` queue and the ``CAPTURE`` queue is the master that governs the
set of supported formats for the ``OUTPUT`` queue.
(总而言之，设置格式与分配必须始终从 ``CAPTURE`` 队列开始，``CAPTURE`` 队列是主导
``OUTPUT`` 队列受支持格式集合的主方。)
