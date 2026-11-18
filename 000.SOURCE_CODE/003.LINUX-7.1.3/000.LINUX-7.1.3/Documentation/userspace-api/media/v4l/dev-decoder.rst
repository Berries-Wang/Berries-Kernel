.. SPDX-License-Identifier: GPL-2.0
.. c:namespace:: V4L

.. _decoder:

*********************************************************************************
Memory-to-Memory Stateful Video Decoder Interface(内存到内存有状态视频解码器接口)
*********************************************************************************

A stateful video decoder takes complete chunks of the bytestream (e.g. Annex-B
H.264/HEVC stream, raw VP8/9 stream) and decodes them into raw video frames in
display order. The decoder is expected not to require any additional information
from the client to process these buffers.
(有状态视频解码器接收完整的字节流块（例如 Annex-B 格式的 H.264/HEVC 流、原始 VP8/9 流），并按显示顺序把它们解码为原始视频帧。解码器不应需要客户端提供任何额外信息来处理这些缓冲区。)

Performing software parsing, processing etc. of the stream in the driver in
order to support this interface is strongly discouraged. In case such
operations are needed, use of the Stateless Video Decoder Interface (in
development) is strongly advised.
(强烈不建议在驱动中对码流进行软件解析、处理等操作来支持本接口。如果确实需要这类操作，强烈建议改用仍在开发中的无状态视频解码器接口（Stateless Video Decoder Interface）。)

Conventions and Notations Used in This Document(本文档中使用的约定与记法)
==========================================================================

1. The general V4L2 API rules apply if not specified in this document
   otherwise.
   (除非本文档另有说明，通用 V4L2 API 规则均适用。)

2. The meaning of words "must", "may", "should", etc. is as per `RFC
   2119 <https://tools.ietf.org/html/rfc2119>`_.
   ("must"、"may"、"should" 等词的含义遵循 RFC 2119。)

3. All steps not marked "optional" are required.
   (所有未标注 "optional" 的步骤都是必需的。)

4. :c:func:`VIDIOC_G_EXT_CTRLS` and :c:func:`VIDIOC_S_EXT_CTRLS` may be used
   interchangeably with :c:func:`VIDIOC_G_CTRL` and :c:func:`VIDIOC_S_CTRL`,
   unless specified otherwise.
   (除非另有说明，:c:func:`VIDIOC_G_EXT_CTRLS` 与 :c:func:`VIDIOC_S_EXT_CTRLS` 可以和 :c:func:`VIDIOC_G_CTRL` 与 :c:func:`VIDIOC_S_CTRL` 互换使用。)

5. Single-planar API (see :ref:`planar-apis`) and applicable structures may be
   used interchangeably with multi-planar API, unless specified otherwise,
   depending on decoder capabilities and following the general V4L2 guidelines.
   (除非另有说明，依据解码器的能力并遵循通用 V4L2 指南，单平面 API（参见 :ref:`planar-apis`）及相应结构可以与多平面 API 互换使用。)

6. i = [a..b]: sequence of integers from a to b, inclusive, i.e. i =
   [0..2]: i = 0, 1, 2.
   (i = [a..b]：从 a 到 b（含两端）的整数序列，即 i = [0..2] 表示 i = 0, 1, 2。)

7. Given an ``OUTPUT`` buffer A, then A' represents a buffer on the ``CAPTURE``
   queue containing data that resulted from processing buffer A.
   (给定 ``OUTPUT`` 缓冲区 A，则 A' 表示 ``CAPTURE`` 队列上包含处理缓冲区 A 所得数据的缓冲区。)

.. _decoder-glossary:

Glossary(术语表)
================

CAPTURE
   the destination buffer queue; for decoders, the queue of buffers containing
   decoded frames; for encoders, the queue of buffers containing an encoded
   bytestream; ``V4L2_BUF_TYPE_VIDEO_CAPTURE`` or
   ``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``; data is captured from the hardware
   into ``CAPTURE`` buffers.
   (CAPTURE：目的缓冲区队列；对解码器而言，是包含已解码帧的缓冲区队列；对编码器而言，是包含已编码字节流的缓冲区队列；对应 ``V4L2_BUF_TYPE_VIDEO_CAPTURE`` 或 ``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``；数据由硬件捕获进 ``CAPTURE`` 缓冲区。)

client
   the application communicating with the decoder or encoder implementing
   this interface.
   (client：与实现本接口的解码器或编码器通信的应用程序。)

coded format
   encoded/compressed video bytestream format (e.g. H.264, VP8, etc.); see
   also: raw format.
   (coded format：编码/压缩后的视频字节流格式（例如 H.264、VP8 等）；另见 raw format（原始格式）。)

coded height
   height for given coded resolution.
   (coded height：给定编码分辨率下的高度。)

coded resolution
   stream resolution in pixels aligned to codec and hardware requirements;
   typically visible resolution rounded up to full macroblocks;
   see also: visible resolution.
   (coded resolution：按编解码器与硬件要求对齐的码流分辨率（像素）；通常是向上取整到完整宏块的可见分辨率；另见 visible resolution（可见分辨率）。)

coded width
   width for given coded resolution.
   (coded width：给定编码分辨率下的宽度。)

coding tree unit
   processing unit of the HEVC codec (corresponds to macroblock units in
   H.264, VP8, VP9),
   can use block structures of up to 64×64 pixels.
   Good at sub-partitioning the picture into variable sized structures.
   (coding tree unit：HEVC 编解码器的处理单元（对应 H.264、VP8、VP9 中的宏块单元），可以使用最大 64×64 像素的块结构，擅长把图像细分为大小可变的结构。)

decode order
   the order in which frames are decoded; may differ from display order if the
   coded format includes a feature of frame reordering; for decoders,
   ``OUTPUT`` buffers must be queued by the client in decode order; for
   encoders ``CAPTURE`` buffers must be returned by the encoder in decode order.
   (decode order：帧被解码的顺序；如果编码格式支持帧重排序特性，它可能与显示顺序不同；对解码器而言，客户端必须按解码顺序把 ``OUTPUT`` 缓冲区入队；对编码器而言，编码器必须按解码顺序返回 ``CAPTURE`` 缓冲区。)

destination
   data resulting from the decode process; see ``CAPTURE``.
   (destination：解码过程产生的数据；参见 ``CAPTURE``。)

display order
   the order in which frames must be displayed; for encoders, ``OUTPUT``
   buffers must be queued by the client in display order; for decoders,
   ``CAPTURE`` buffers must be returned by the decoder in display order.
   (display order：帧必须被显示的顺序；对编码器而言，客户端必须按显示顺序把 ``OUTPUT`` 缓冲区入队；对解码器而言，解码器必须按显示顺序返回 ``CAPTURE`` 缓冲区。)

DPB
   Decoded Picture Buffer; an H.264/HEVC term for a buffer that stores a decoded
   raw frame available for reference in further decoding steps.
   (DPB：Decoded Picture Buffer（解码图像缓冲区），H.264/HEVC 术语，指存储已解码原始帧的缓冲区，供后续解码步骤参考使用。)

EOS
   end of stream.
   (EOS：流结束（end of stream）。)

IDR
   Instantaneous Decoder Refresh; a type of a keyframe in an H.264/HEVC-encoded
   stream, which clears the list of earlier reference frames (DPBs).
   (IDR：Instantaneous Decoder Refresh（即时解码刷新），H.264/HEVC 编码流中的一种关键帧，会清除较早的参考帧列表（DPB）。)

keyframe
   an encoded frame that does not reference frames decoded earlier, i.e.
   can be decoded fully on its own.
   (keyframe：不引用先前已解码帧的编码帧，即可以完全凭自身解码。)

macroblock
   a processing unit in image and video compression formats based on linear
   block transforms (e.g. H.264, VP8, VP9); codec-specific, but for most of
   popular codecs the size is 16x16 samples (pixels). The HEVC codec uses a
   slightly more flexible processing unit called coding tree unit (CTU).
   (macroblock：基于线性块变换的图像与视频压缩格式（例如 H.264、VP8、VP9）中的处理单元；因编解码器而异，但对大多数常见编解码器而言大小为 16x16 个采样（像素）。HEVC 编解码器使用一种稍微更灵活的处理单元，称为编码树单元（coding tree unit，CTU）。)

OUTPUT
   the source buffer queue; for decoders, the queue of buffers containing
   an encoded bytestream; for encoders, the queue of buffers containing raw
   frames; ``V4L2_BUF_TYPE_VIDEO_OUTPUT`` or
   ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``; the hardware is fed with data
   from ``OUTPUT`` buffers.
   (OUTPUT：源缓冲区队列；对解码器而言，是包含已编码字节流的缓冲区队列；对编码器而言，是包含原始帧的缓冲区队列；对应 ``V4L2_BUF_TYPE_VIDEO_OUTPUT`` 或 ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``；硬件从 ``OUTPUT`` 缓冲区获取数据。)

PPS
   Picture Parameter Set; a type of metadata entity in an H.264/HEVC bytestream.
   (PPS：Picture Parameter Set（图像参数集），H.264/HEVC 字节流中的一种元数据实体。)

raw format
   uncompressed format containing raw pixel data (e.g. YUV, RGB formats).
   (raw format：包含原始像素数据的未压缩格式（例如 YUV、RGB 格式）。)

resume point
   a point in the bytestream from which decoding may start/continue, without
   any previous state/data present, e.g.: a keyframe (VP8/VP9) or
   SPS/PPS/IDR sequence (H.264/HEVC); a resume point is required to start decode
   of a new stream, or to resume decoding after a seek.
   (resume point：字节流中可以（在没有任何先前状态/数据的情况下）开始或继续解码的位置，例如关键帧（VP8/VP9）或 SPS/PPS/IDR 序列（H.264/HEVC）；开始解码新码流或在 seek 之后恢复解码都需要一个恢复点。)

source
   data fed to the decoder or encoder; see ``OUTPUT``.
   (source：馈送给解码器或编码器的数据；参见 ``OUTPUT``。)

source height
   height in pixels for given source resolution; relevant to encoders only.
   (source height：给定源分辨率下的高度（像素）；仅与编码器相关。)

source resolution
   resolution in pixels of source frames being source to the encoder and
   subject to further cropping to the bounds of visible resolution; relevant to
   encoders only.
   (source resolution：作为编码器输入的源帧的像素分辨率，之后还会被进一步裁剪到可见分辨率的边界内；仅与编码器相关。)

source width
   width in pixels for given source resolution; relevant to encoders only.
   (source width：给定源分辨率下的宽度（像素）；仅与编码器相关。)

SPS
   Sequence Parameter Set; a type of metadata entity in an H.264/HEVC bytestream.
   (SPS：Sequence Parameter Set（序列参数集），H.264/HEVC 字节流中的一种元数据实体。)

stream metadata
   additional (non-visual) information contained inside encoded bytestream;
   for example: coded resolution, visible resolution, codec profile.
   (stream metadata：编码字节流内包含的额外（非视觉）信息，例如：编码分辨率、可见分辨率、编解码器 profile。)

visible height
   height for given visible resolution; display height.
   (visible height：给定可见分辨率下的高度；即显示高度。)

visible resolution
   stream resolution of the visible picture, in pixels, to be used for
   display purposes; must be smaller or equal to coded resolution;
   display resolution.
   (visible resolution：用于显示目的的可见图像的码流分辨率（像素）；必须小于或等于编码分辨率；即显示分辨率。)

visible width
   width for given visible resolution; display width.
   (visible width：给定可见分辨率下的宽度；即显示宽度。)

State Machine(状态机)
=====================

.. kernel-render:: DOT
   :alt: DOT digraph of decoder state machine
   :caption: Decoder State Machine

   digraph decoder_state_machine {
       node [shape = doublecircle, label="Decoding"] Decoding;

       node [shape = circle, label="Initialization"] Initialization;
       node [shape = circle, label="Capture\nsetup"] CaptureSetup;
       node [shape = circle, label="Dynamic\nResolution\nChange"] ResChange;
       node [shape = circle, label="Stopped"] Stopped;
       node [shape = circle, label="Drain"] Drain;
       node [shape = circle, label="Seek"] Seek;
       node [shape = circle, label="End of Stream"] EoS;

       node [shape = point]; qi
       qi -> Initialization [ label = "open()" ];

       Initialization -> CaptureSetup [ label = "CAPTURE\nformat\nestablished" ];

       CaptureSetup -> Stopped [ label = "CAPTURE\nbuffers\nready" ];

       Decoding -> ResChange [ label = "Stream\nresolution\nchange" ];
       Decoding -> Drain [ label = "V4L2_DEC_CMD_STOP" ];
       Decoding -> EoS [ label = "EoS mark\nin the stream" ];
       Decoding -> Seek [ label = "VIDIOC_STREAMOFF(OUTPUT)" ];
       Decoding -> Stopped [ label = "VIDIOC_STREAMOFF(CAPTURE)" ];
       Decoding -> Decoding;

       ResChange -> CaptureSetup [ label = "CAPTURE\nformat\nestablished" ];
       ResChange -> Seek [ label = "VIDIOC_STREAMOFF(OUTPUT)" ];

       EoS -> Drain [ label = "Implicit\ndrain" ];

       Drain -> Stopped [ label = "All CAPTURE\nbuffers dequeued\nor\nVIDIOC_STREAMOFF(CAPTURE)" ];
       Drain -> Seek [ label = "VIDIOC_STREAMOFF(OUTPUT)" ];

       Seek -> Decoding [ label = "VIDIOC_STREAMON(OUTPUT)" ];
       Seek -> Initialization [ label = "VIDIOC_REQBUFS(OUTPUT, 0)" ];

       Stopped -> Decoding [ label = "V4L2_DEC_CMD_START\nor\nVIDIOC_STREAMON(CAPTURE)" ];
       Stopped -> Seek [ label = "VIDIOC_STREAMOFF(OUTPUT)" ];
   }

Querying Capabilities(查询能力)
===============================

1. To enumerate the set of coded formats supported by the decoder, the
   client may call :c:func:`VIDIOC_ENUM_FMT` on ``OUTPUT``.
   (1. 要枚举解码器支持的编码格式集合，客户端可以在 ``OUTPUT`` 上调用 :c:func:`VIDIOC_ENUM_FMT`。)

   * The full set of supported formats will be returned, regardless of the
     format set on ``CAPTURE``.
     (无论 ``CAPTURE`` 上设置了什么格式，都会返回受支持格式的全集。)
   * Check the flags field of :c:type:`v4l2_fmtdesc` for more information
     about the decoder's capabilities with respect to each coded format.
     In particular whether or not the decoder has a full-fledged bytestream
     parser and if the decoder supports dynamic resolution changes.
     (查看 :c:type:`v4l2_fmtdesc` 的 flags 字段可了解解码器对每种编码格式的能力，特别是解码器是否具备完整的字节流解析器，以及是否支持动态分辨率变更。)

2. To enumerate the set of supported raw formats, the client may call
   :c:func:`VIDIOC_ENUM_FMT` on ``CAPTURE``.
   (2. 要枚举受支持的原始格式集合，客户端可以在 ``CAPTURE`` 上调用 :c:func:`VIDIOC_ENUM_FMT`。)

   * Only the formats supported for the format currently active on ``OUTPUT``
     will be returned.
     (只会返回与 ``OUTPUT`` 当前生效格式相匹配的受支持格式。)

   * In order to enumerate raw formats supported by a given coded format,
     the client must first set that coded format on ``OUTPUT`` and then
     enumerate formats on ``CAPTURE``.
     (要枚举某个给定编码格式所支持的原始格式，客户端必须先在 ``OUTPUT`` 上设置该编码格式，然后在 ``CAPTURE`` 上枚举格式。)

3. The client may use :c:func:`VIDIOC_ENUM_FRAMESIZES` to detect supported
   resolutions for a given format, passing desired pixel format in
   :c:type:`v4l2_frmsizeenum` ``pixel_format``.
   (3. 客户端可以用 :c:func:`VIDIOC_ENUM_FRAMESIZES` 来探测给定格式支持的分辨率，在 :c:type:`v4l2_frmsizeenum` 的 ``pixel_format`` 中传入想要的像素格式。)

   * Values returned by :c:func:`VIDIOC_ENUM_FRAMESIZES` for a coded pixel
     format will include all possible coded resolutions supported by the
     decoder for given coded pixel format.
     (对编码像素格式，:c:func:`VIDIOC_ENUM_FRAMESIZES` 返回的值将包含解码器对该给定编码像素格式支持的所有可能编码分辨率。)

   * Values returned by :c:func:`VIDIOC_ENUM_FRAMESIZES` for a raw pixel format
     will include all possible frame buffer resolutions supported by the
     decoder for given raw pixel format and the coded format currently set on
     ``OUTPUT``.
     (对原始像素格式，:c:func:`VIDIOC_ENUM_FRAMESIZES` 返回的值将包含解码器针对该给定原始像素格式以及 ``OUTPUT`` 当前设置的编码格式所支持的所有可能帧缓冲分辨率。)

4. Supported profiles and levels for the coded format currently set on
   ``OUTPUT``, if applicable, may be queried using their respective controls
   via :c:func:`VIDIOC_QUERYCTRL`.
   (4. 对于 ``OUTPUT`` 当前设置的编码格式，如适用，可以通过相应的控制项并借助 :c:func:`VIDIOC_QUERYCTRL` 查询其受支持的 profile 和 level。)

Initialization(初始化)
======================

1. Set the coded format on ``OUTPUT`` via :c:func:`VIDIOC_S_FMT`.
   (1. 通过 :c:func:`VIDIOC_S_FMT` 在 ``OUTPUT`` 上设置编码格式。)

   * **Required fields:**
     (必需字段：)

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
         (适合 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

     ``pixelformat``
         a coded pixel format.
         (一种编码像素格式。)

     ``width``, ``height``
         coded resolution of the stream; required only if it cannot be parsed
         from the stream for the given coded format; otherwise the decoder will
         use this resolution as a placeholder resolution that will likely change
         as soon as it can parse the actual coded resolution from the stream.
         (码流的编码分辨率；仅当给定编码格式无法从码流中解析出该值时才必填；否则解码器会把它当作占位分辨率使用，一旦能从码流解析出实际编码分辨率，它很可能随之改变。)

     ``sizeimage``
         desired size of ``OUTPUT`` buffers; the decoder may adjust it to
         match hardware requirements.
         (期望的 ``OUTPUT`` 缓冲区大小；解码器可能会调整它以匹配硬件要求。)

     other fields
         follow standard semantics.
         (其他字段遵循标准语义。)

   * **Returned fields:**
     (返回字段：)

     ``sizeimage``
         adjusted size of ``OUTPUT`` buffers.
         (调整后的 ``OUTPUT`` 缓冲区大小。)

   * The ``CAPTURE`` format will be updated with an appropriate frame buffer
     resolution instantly based on the width and height returned by
     :c:func:`VIDIOC_S_FMT`.
     However, for coded formats that include stream resolution information,
     after the decoder is done parsing the information from the stream, it will
     update the ``CAPTURE`` format with new values and signal a source change
     event, regardless of whether they match the values set by the client or
     not.
     (``CAPTURE`` 格式会立刻根据 :c:func:`VIDIOC_S_FMT` 返回的宽高更新为相应的帧缓冲分辨率。然而，对于码流中包含分辨率信息的编码格式，解码器在完成码流信息解析之后，会用新值更新 ``CAPTURE`` 格式并发送源变更事件，无论它们是否与客户端设置的值一致。)

   .. important::

      Changing the ``OUTPUT`` format may change the currently set ``CAPTURE``
      format. How the new ``CAPTURE`` format is determined is up to the decoder
      and the client must ensure it matches its needs afterwards.
      (更改 ``OUTPUT`` 格式可能会改变当前设置的 ``CAPTURE`` 格式。新的 ``CAPTURE`` 格式如何确定由解码器决定，客户端必须事后自行确认它满足自己的需求。)

2.  Allocate source (bytestream) buffers via :c:func:`VIDIOC_REQBUFS` on
    ``OUTPUT``.
    (2. 通过 :c:func:`VIDIOC_REQBUFS` 在 ``OUTPUT`` 上分配源（字节流）缓冲区。)

    * **Required fields:**
      (必需字段：)

      ``count``
          requested number of buffers to allocate; greater than zero.
          (请求分配的缓冲区数量；大于零。)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
          (适合 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``memory``
          follows standard semantics.
          (遵循标准语义。)

    * **Returned fields:**
      (返回字段：)

      ``count``
          the actual number of buffers allocated.
          (实际分配的缓冲区数量。)

    .. warning::

       The actual number of allocated buffers may differ from the ``count``
       given. The client must check the updated value of ``count`` after the
       call returns.
       (实际分配的缓冲区数量可能与给定的 ``count`` 不同。调用返回后，客户端必须检查更新后的 ``count`` 值。)

    Alternatively, :c:func:`VIDIOC_CREATE_BUFS` on the ``OUTPUT`` queue can be
    used to have more control over buffer allocation.
    (另外，也可以在 ``OUTPUT`` 队列上使用 :c:func:`VIDIOC_CREATE_BUFS`，以便对缓冲区分配做更多控制。)

    * **Required fields:**
      (必需字段：)

      ``count``
          requested number of buffers to allocate; greater than zero.
          (请求分配的缓冲区数量；大于零。)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
          (适合 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``memory``
          follows standard semantics.
          (遵循标准语义。)

      ``format``
          follows standard semantics.
          (遵循标准语义。)

    * **Returned fields:**
      (返回字段：)

      ``count``
          adjusted to the number of allocated buffers.
          (调整为已分配缓冲区的数量。)

    .. warning::

       The actual number of allocated buffers may differ from the ``count``
       given. The client must check the updated value of ``count`` after the
       call returns.
       (实际分配的缓冲区数量可能与给定的 ``count`` 不同。调用返回后，客户端必须检查更新后的 ``count`` 值。)

3.  Start streaming on the ``OUTPUT`` queue via :c:func:`VIDIOC_STREAMON`.
    (3. 通过 :c:func:`VIDIOC_STREAMON` 在 ``OUTPUT`` 队列上开始流式传输。)

4.  **This step only applies to coded formats that contain resolution information
    in the stream.** Continue queuing/dequeuing bytestream buffers to/from the
    ``OUTPUT`` queue via :c:func:`VIDIOC_QBUF` and :c:func:`VIDIOC_DQBUF`. The
    buffers will be processed and returned to the client in order, until
    required metadata to configure the ``CAPTURE`` queue are found. This is
    indicated by the decoder sending a ``V4L2_EVENT_SOURCE_CHANGE`` event with
    ``changes`` set to ``V4L2_EVENT_SRC_CH_RESOLUTION``.
    (4. **本步骤仅适用于码流中包含分辨率信息的编码格式。** 继续通过 :c:func:`VIDIOC_QBUF` 和 :c:func:`VIDIOC_DQBUF` 向 ``OUTPUT`` 队列入队/出队字节流缓冲区。这些缓冲区会被按序处理并返回给客户端，直到找到配置 ``CAPTURE`` 队列所需的元数据。解码器会通过发送 ``changes`` 设为 ``V4L2_EVENT_SRC_CH_RESOLUTION`` 的 ``V4L2_EVENT_SOURCE_CHANGE`` 事件来指示这一点。)

    * It is not an error if the first buffer does not contain enough data for
      this to occur. Processing of the buffers will continue as long as more
      data is needed.
      (如果第一个缓冲区包含的数据不足以触发这一情况，并不算错误。只要还需要更多数据，缓冲区的处理就会继续。)

    * If data in a buffer that triggers the event is required to decode the
      first frame, it will not be returned to the client, until the
      initialization sequence completes and the frame is decoded.
      (如果触发事件的缓冲区中的数据是解码第一帧所必需的，那么在初始化序列完成且该帧被解码之前，它不会返回给客户端。)

    * If the client has not set the coded resolution of the stream on its own,
      calling :c:func:`VIDIOC_G_FMT`, :c:func:`VIDIOC_S_FMT`,
      :c:func:`VIDIOC_TRY_FMT` or :c:func:`VIDIOC_REQBUFS` on the ``CAPTURE``
      queue will not return the real values for the stream until a
      ``V4L2_EVENT_SOURCE_CHANGE`` event with ``changes`` set to
      ``V4L2_EVENT_SRC_CH_RESOLUTION`` is signaled.
      (如果客户端没有自行设置码流的编码分辨率，那么在发出 ``changes`` 设为 ``V4L2_EVENT_SRC_CH_RESOLUTION`` 的 ``V4L2_EVENT_SOURCE_CHANGE`` 事件之前，在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_G_FMT`、:c:func:`VIDIOC_S_FMT`、:c:func:`VIDIOC_TRY_FMT` 或 :c:func:`VIDIOC_REQBUFS` 都不会返回码流的真实值。)

    .. important::

       Any client query issued after the decoder queues the event will return
       values applying to the just parsed stream, including queue formats,
       selection rectangles and controls.
       (在解码器把事件入队之后，客户端发出的任何查询都会返回适用于刚解析出的码流的值，包括队列格式、选择矩形和控制项。)

    .. note::

       A client capable of acquiring stream parameters from the bytestream on
       its own may attempt to set the width and height of the ``OUTPUT`` format
       to non-zero values matching the coded size of the stream, skip this step
       and continue with the `Capture Setup(捕获设置)` sequence. However, it must not
       rely on any driver queries regarding stream parameters, such as
       selection rectangles and controls, since the decoder has not parsed them
       from the stream yet. If the values configured by the client do not match
       those parsed by the decoder, a `Dynamic Resolution Change(动态分辨率变更)` will be
       triggered to reconfigure them.
       (能够自行从字节流获取码流参数的客户端，可以尝试把 ``OUTPUT`` 格式的宽高设置为与码流编码尺寸匹配的非零值，跳过本步骤并继续 `Capture Setup(捕获设置)` 序列。但是，它不得依赖任何关于码流参数的驱动查询，例如选择矩形和控制项，因为解码器尚未从码流中解析出它们。如果客户端配置的值与解码器解析出的值不匹配，会触发 `Dynamic Resolution Change(动态分辨率变更)` 来重新配置。)

    .. note::

       No decoded frames are produced during this phase.
       (在此阶段不会产生任何已解码帧。)

5.  Continue with the `Capture Setup(捕获设置)` sequence.
    (5. 继续 `Capture Setup(捕获设置)` 序列。)

Capture Setup(捕获设置)
=======================

1.  Call :c:func:`VIDIOC_G_FMT` on the ``CAPTURE`` queue to get format for the
    destination buffers parsed/decoded from the bytestream.
    (1. 在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_G_FMT`，获取从字节流解析/解码出的目的缓冲区的格式。)

    * **Required fields:**
      (必需字段：)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
          (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

    * **Returned fields:**
      (返回字段：)

      ``width``, ``height``
          frame buffer resolution for the decoded frames.
          (已解码帧的帧缓冲分辨率。)

      ``pixelformat``
          pixel format for decoded frames.
          (已解码帧的像素格式。)

      ``num_planes`` (for _MPLANE ``type`` only)
          number of planes for pixelformat.
          (该像素格式的平面数。)

      ``sizeimage``, ``bytesperline``
          as per standard semantics; matching frame buffer format.
          (遵循标准语义；与帧缓冲格式匹配。)

    .. note::

       The value of ``pixelformat`` may be any pixel format supported by the
       decoder for the current stream. The decoder should choose a
       preferred/optimal format for the default configuration. For example, a
       YUV format may be preferred over an RGB format if an additional
       conversion step would be required for the latter.
       (``pixelformat`` 的值可以是解码器为当前码流支持的任何像素格式。解码器应为默认配置选择一个偏好/最优的格式。例如，如果 RGB 格式需要额外的转换步骤，则可能优先选择 YUV 格式。)

2.  **Optional.** Acquire the visible resolution via
    :c:func:`VIDIOC_G_SELECTION`.
    (2. **可选。** 通过 :c:func:`VIDIOC_G_SELECTION` 获取可见分辨率。)

    * **Required fields:**
      (必需字段：)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
          (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``target``
          set to ``V4L2_SEL_TGT_COMPOSE``.
          (设置为 ``V4L2_SEL_TGT_COMPOSE``。)

    * **Returned fields:**
      (返回字段：)

      ``r.left``, ``r.top``, ``r.width``, ``r.height``
          the visible rectangle; it must fit within the frame buffer resolution
          returned by :c:func:`VIDIOC_G_FMT` on ``CAPTURE``.
          (可见矩形；它必须落在 ``CAPTURE`` 上 :c:func:`VIDIOC_G_FMT` 返回的帧缓冲分辨率范围内。)

    * The following selection targets are supported on ``CAPTURE``:
      (``CAPTURE`` 上支持以下选择目标：)

      ``V4L2_SEL_TGT_CROP_BOUNDS``
          corresponds to the coded resolution of the stream.
          (对应码流的编码分辨率。)

      ``V4L2_SEL_TGT_CROP_DEFAULT``
          the rectangle covering the part of the ``CAPTURE`` buffer that
          contains meaningful picture data (visible area); width and height
          will be equal to the visible resolution of the stream.
          (覆盖 ``CAPTURE`` 缓冲区中含有有效图像数据部分（可见区域）的矩形；其宽高等于码流的可见分辨率。)

      ``V4L2_SEL_TGT_CROP``
          the rectangle within the coded resolution to be output to
          ``CAPTURE``; defaults to ``V4L2_SEL_TGT_CROP_DEFAULT``; read-only on
          hardware without additional compose/scaling capabilities.
          (编码分辨率内将被输出到 ``CAPTURE`` 的矩形；默认为 ``V4L2_SEL_TGT_CROP_DEFAULT``；在无额外合成/缩放能力的硬件上为只读。)

      ``V4L2_SEL_TGT_COMPOSE_BOUNDS``
          the maximum rectangle within a ``CAPTURE`` buffer, which the cropped
          frame can be composed into; equal to ``V4L2_SEL_TGT_CROP`` if the
          hardware does not support compose/scaling.
          (``CAPTURE`` 缓冲区内裁剪帧可以合成进去的最大矩形；若硬件不支持合成/缩放，则等于 ``V4L2_SEL_TGT_CROP``。)

      ``V4L2_SEL_TGT_COMPOSE_DEFAULT``
          equal to ``V4L2_SEL_TGT_CROP``.
          (等于 ``V4L2_SEL_TGT_CROP``。)

      ``V4L2_SEL_TGT_COMPOSE``
          the rectangle inside a ``CAPTURE`` buffer into which the cropped
          frame is written; defaults to ``V4L2_SEL_TGT_COMPOSE_DEFAULT``;
          read-only on hardware without additional compose/scaling capabilities.
          (``CAPTURE`` 缓冲区内写入裁剪帧的矩形；默认为 ``V4L2_SEL_TGT_COMPOSE_DEFAULT``；在无额外合成/缩放能力的硬件上为只读。)

      ``V4L2_SEL_TGT_COMPOSE_PADDED``
          the rectangle inside a ``CAPTURE`` buffer which is overwritten by the
          hardware; equal to ``V4L2_SEL_TGT_COMPOSE`` if the hardware does not
          write padding pixels.
          (``CAPTURE`` 缓冲区内会被硬件覆写的矩形；若硬件不写填充像素，则等于 ``V4L2_SEL_TGT_COMPOSE``。)

    .. warning::

       The values are guaranteed to be meaningful only after the decoder
       successfully parses the stream metadata. The client must not rely on the
       query before that happens.
       (只有在解码器成功解析码流元数据之后，这些值才保证有意义。在此之前客户端不得依赖该查询。)

3.  **Optional.** Enumerate ``CAPTURE`` formats via :c:func:`VIDIOC_ENUM_FMT` on
    the ``CAPTURE`` queue. Once the stream information is parsed and known, the
    client may use this ioctl to discover which raw formats are supported for
    given stream and select one of them via :c:func:`VIDIOC_S_FMT`.
    (3. **可选。** 在 ``CAPTURE`` 队列上通过 :c:func:`VIDIOC_ENUM_FMT` 枚举 ``CAPTURE`` 格式。一旦码流信息被解析并知晓，客户端就可以用这个 ioctl 发现给定码流支持哪些原始格式，并通过 :c:func:`VIDIOC_S_FMT` 选择其中之一。)

    .. important::

       The decoder will return only formats supported for the currently
       established coded format, as per the ``OUTPUT`` format and/or stream
       metadata parsed in this initialization sequence, even if more formats
       may be supported by the decoder in general. In other words, the set
       returned will be a subset of the initial query mentioned in the
       `Querying Capabilities(查询能力)` section.
       (解码器只会返回当前已确立的编码格式所支持的格式——依据 ``OUTPUT`` 格式和/或本初始化序列中解析出的码流元数据——即使解码器总体上可能支持更多格式。换言之，返回的集合将是 `Querying Capabilities(查询能力)` 一节所述初始查询结果的一个子集。)

       For example, a decoder may support YUV and RGB formats for resolutions
       1920x1088 and lower, but only YUV for higher resolutions (due to
       hardware limitations). After parsing a resolution of 1920x1088 or lower,
       :c:func:`VIDIOC_ENUM_FMT` may return a set of YUV and RGB pixel formats,
       but after parsing resolution higher than 1920x1088, the decoder will not
       return RGB, unsupported for this resolution.
       (例如，解码器可能在 1920x1088 及更低分辨率下支持 YUV 和 RGB 格式，而在更高分辨率下只支持 YUV（受硬件限制）。解析出 1920x1088 或更低的分辨率后，:c:func:`VIDIOC_ENUM_FMT` 可能返回 YUV 和 RGB 像素格式的集合；但解析出高于 1920x1088 的分辨率后，解码器不会返回 RGB——该分辨率不支持它。)

       However, subsequent resolution change event triggered after
       discovering a resolution change within the same stream may switch
       the stream into a lower resolution and :c:func:`VIDIOC_ENUM_FMT`
       would return RGB formats again in that case.
       (不过，在同一码流中发现分辨率变化后触发的后续分辨率变更事件，可能把码流切换到更低的分辨率，这种情况下 :c:func:`VIDIOC_ENUM_FMT` 会再次返回 RGB 格式。)

4.  **Optional.** Set the ``CAPTURE`` format via :c:func:`VIDIOC_S_FMT` on the
    ``CAPTURE`` queue. The client may choose a different format than
    selected/suggested by the decoder in :c:func:`VIDIOC_G_FMT`.
    (4. **可选。** 在 ``CAPTURE`` 队列上通过 :c:func:`VIDIOC_S_FMT` 设置 ``CAPTURE`` 格式。客户端可以选择与解码器在 :c:func:`VIDIOC_G_FMT` 中选定/建议的格式不同的格式。)

    * **Required fields:**
      (必需字段：)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
          (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``pixelformat``
          a raw pixel format.
          (一种原始像素格式。)

      ``width``, ``height``
         frame buffer resolution of the decoded stream; typically unchanged from
	 what was returned with :c:func:`VIDIOC_G_FMT`, but it may be different
	 if the hardware supports composition and/or scaling.
         (已解码码流的帧缓冲分辨率；通常与 :c:func:`VIDIOC_G_FMT` 返回值相同，但如果硬件支持合成和/或缩放，则可能不同。)

   * Setting the ``CAPTURE`` format will reset the compose selection rectangles
     to their default values, based on the new resolution, as described in the
     previous step.
     (设置 ``CAPTURE`` 格式会基于新分辨率，把合成选择矩形重置为默认值，如前一步骤所述。)

5. **Optional.** Set the compose rectangle via :c:func:`VIDIOC_S_SELECTION` on
   the ``CAPTURE`` queue if it is desired and if the decoder has compose and/or
   scaling capabilities.
   (5. **可选。** 如有需要，且解码器具备合成和/或缩放能力，可在 ``CAPTURE`` 队列上通过 :c:func:`VIDIOC_S_SELECTION` 设置合成矩形。)

   * **Required fields:**
     (必需字段：)

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
         (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

     ``target``
         set to ``V4L2_SEL_TGT_COMPOSE``.
         (设置为 ``V4L2_SEL_TGT_COMPOSE``。)

     ``r.left``, ``r.top``, ``r.width``, ``r.height``
         the rectangle inside a ``CAPTURE`` buffer into which the cropped
         frame is written; defaults to ``V4L2_SEL_TGT_COMPOSE_DEFAULT``;
         read-only on hardware without additional compose/scaling capabilities.
         (``CAPTURE`` 缓冲区内写入裁剪帧的矩形；默认为 ``V4L2_SEL_TGT_COMPOSE_DEFAULT``；在无额外合成/缩放能力的硬件上为只读。)

   * **Returned fields:**
     (返回字段：)

     ``r.left``, ``r.top``, ``r.width``, ``r.height``
         the visible rectangle; it must fit within the frame buffer resolution
         returned by :c:func:`VIDIOC_G_FMT` on ``CAPTURE``.
         (可见矩形；它必须落在 ``CAPTURE`` 上 :c:func:`VIDIOC_G_FMT` 返回的帧缓冲分辨率范围内。)

   .. warning::

      The decoder may adjust the compose rectangle to the nearest
      supported one to meet codec and hardware requirements. The client needs
      to check the adjusted rectangle returned by :c:func:`VIDIOC_S_SELECTION`.
      (解码器可能把合成矩形调整到最近的受支持值以满足编解码器和硬件要求。客户端需要检查 :c:func:`VIDIOC_S_SELECTION` 返回的调整后的矩形。)

6.  If all the following conditions are met, the client may resume the decoding
    instantly:
    (6. 如果以下所有条件都满足，客户端可以立即恢复解码：)

    * ``sizeimage`` of the new format (determined in previous steps) is less
      than or equal to the size of currently allocated buffers,
      (新格式（由前述步骤确定）的 ``sizeimage`` 小于或等于当前已分配缓冲区的大小，)

    * the number of buffers currently allocated is greater than or equal to the
      minimum number of buffers acquired in previous steps. To fulfill this
      requirement, the client may use :c:func:`VIDIOC_CREATE_BUFS` to add new
      buffers.
      (当前已分配的缓冲区数量大于或等于前述步骤获得的最小缓冲区数量。为满足这一要求，客户端可以用 :c:func:`VIDIOC_CREATE_BUFS` 添加新缓冲区。)

    In that case, the remaining steps do not apply and the client may resume
    the decoding by one of the following actions:
    (在这种情况下，其余步骤不再适用，客户端可以通过以下操作之一恢复解码：)

    * if the ``CAPTURE`` queue is streaming, call :c:func:`VIDIOC_DECODER_CMD`
      with the ``V4L2_DEC_CMD_START`` command,
      (如果 ``CAPTURE`` 队列正在流式传输，用 ``V4L2_DEC_CMD_START`` 命令调用 :c:func:`VIDIOC_DECODER_CMD`，)

    * if the ``CAPTURE`` queue is not streaming, call :c:func:`VIDIOC_STREAMON`
      on the ``CAPTURE`` queue.
      (如果 ``CAPTURE`` 队列未在流式传输，在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_STREAMON`。)

    However, if the client intends to change the buffer set, to lower
    memory usage or for any other reasons, it may be achieved by following
    the steps below.
    (不过，如果客户端打算更换缓冲区集合——为了降低内存占用或其他原因——可以按照以下步骤进行。)

7.  **If the** ``CAPTURE`` **queue is streaming,** keep queuing and dequeuing
    buffers on the ``CAPTURE`` queue until a buffer marked with the
    ``V4L2_BUF_FLAG_LAST`` flag is dequeued.
    (7. **如果 ``CAPTURE`` 队列正在流式传输，**继续在 ``CAPTURE`` 队列上入队和出队缓冲区，直到出队一个带有 ``V4L2_BUF_FLAG_LAST`` 标志的缓冲区。)

8.  **If the** ``CAPTURE`` **queue is streaming,** call :c:func:`VIDIOC_STREAMOFF`
    on the ``CAPTURE`` queue to stop streaming.
    (8. **如果 ``CAPTURE`` 队列正在流式传输，**在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_STREAMOFF` 停止流式传输。)

    .. warning::

       The ``OUTPUT`` queue must remain streaming. Calling
       :c:func:`VIDIOC_STREAMOFF` on it would abort the sequence and trigger a
       seek.
       (``OUTPUT`` 队列必须保持流式传输。对它调用 :c:func:`VIDIOC_STREAMOFF` 会中止本序列并触发一次跳转（seek）。)

9.  **If the** ``CAPTURE`` **queue has buffers allocated,** free the ``CAPTURE``
    buffers using :c:func:`VIDIOC_REQBUFS`.
    (9. **如果 ``CAPTURE`` 队列已分配缓冲区，**用 :c:func:`VIDIOC_REQBUFS` 释放 ``CAPTURE`` 缓冲区。)

    * **Required fields:**
      (必需字段：)

      ``count``
          set to 0.
          (设置为 0。)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
          (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``memory``
          follows standard semantics.
          (遵循标准语义。)

10. Allocate ``CAPTURE`` buffers via :c:func:`VIDIOC_REQBUFS` on the
    ``CAPTURE`` queue.
    (10. 在 ``CAPTURE`` 队列上通过 :c:func:`VIDIOC_REQBUFS` 分配 ``CAPTURE`` 缓冲区。)

    * **Required fields:**
      (必需字段：)

      ``count``
          requested number of buffers to allocate; greater than zero.
          (请求分配的缓冲区数量；大于零。)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
          (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``memory``
          follows standard semantics.
          (遵循标准语义。)

    * **Returned fields:**
      (返回字段：)

      ``count``
          actual number of buffers allocated.
          (实际分配的缓冲区数量。)

    .. warning::

       The actual number of allocated buffers may differ from the ``count``
       given. The client must check the updated value of ``count`` after the
       call returns.
       (实际分配的缓冲区数量可能与给定的 ``count`` 不同。调用返回后，客户端必须检查更新后的 ``count`` 值。)

    .. note::

       To allocate more than the minimum number of buffers (for pipeline
       depth), the client may query the ``V4L2_CID_MIN_BUFFERS_FOR_CAPTURE``
       control to get the minimum number of buffers required, and pass the
       obtained value plus the number of additional buffers needed in the
       ``count`` field to :c:func:`VIDIOC_REQBUFS`.
       (要分配比最少数量更多的缓冲区（为了流水线深度），客户端可以查询 ``V4L2_CID_MIN_BUFFERS_FOR_CAPTURE`` 控制项以获得所需的最少缓冲区数量，并把所得值加上额外需要的缓冲区数量，通过 ``count`` 字段传给 :c:func:`VIDIOC_REQBUFS`。)

    Alternatively, :c:func:`VIDIOC_CREATE_BUFS` on the ``CAPTURE`` queue can be
    used to have more control over buffer allocation. For example, by
    allocating buffers larger than the current ``CAPTURE`` format, future
    resolution changes can be accommodated.
    (另外，也可以在 ``CAPTURE`` 队列上使用 :c:func:`VIDIOC_CREATE_BUFS`，以便对缓冲区分配做更多控制。例如，分配比当前 ``CAPTURE`` 格式更大的缓冲区，就可以容纳未来的分辨率变更。)

    * **Required fields:**
      (必需字段：)

      ``count``
          requested number of buffers to allocate; greater than zero.
          (请求分配的缓冲区数量；大于零。)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
          (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``memory``
          follows standard semantics.
          (遵循标准语义。)

      ``format``
          a format representing the maximum framebuffer resolution to be
          accommodated by newly allocated buffers.
          (表示新分配缓冲区要容纳的最大帧缓冲分辨率的格式。)

    * **Returned fields:**
      (返回字段：)

      ``count``
          adjusted to the number of allocated buffers.
          (调整为已分配缓冲区的数量。)

    .. warning::

        The actual number of allocated buffers may differ from the ``count``
        given. The client must check the updated value of ``count`` after the
        call returns.
        (实际分配的缓冲区数量可能与给定的 ``count`` 不同。调用返回后，客户端必须检查更新后的 ``count`` 值。)

    .. note::

       To allocate buffers for a format different than parsed from the stream
       metadata, the client must proceed as follows, before the metadata
       parsing is initiated:
       (要为不同于从码流元数据解析出的格式分配缓冲区，客户端必须在元数据解析开始之前按以下步骤操作：)

       * set width and height of the ``OUTPUT`` format to desired coded resolution to
         let the decoder configure the ``CAPTURE`` format appropriately,
         (把 ``OUTPUT`` 格式的宽高设置为期望的编码分辨率，让解码器相应地配置 ``CAPTURE`` 格式，)

       * query the ``CAPTURE`` format using :c:func:`VIDIOC_G_FMT` and save it
         until this step.
         (用 :c:func:`VIDIOC_G_FMT` 查询 ``CAPTURE`` 格式并保存，留待本步骤使用。)

       The format obtained in the query may be then used with
       :c:func:`VIDIOC_CREATE_BUFS` in this step to allocate the buffers.
       (查询得到的格式随后可在本步骤中配合 :c:func:`VIDIOC_CREATE_BUFS` 来分配缓冲区。)

11. Call :c:func:`VIDIOC_STREAMON` on the ``CAPTURE`` queue to start decoding
    frames.
    (11. 在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_STREAMON` 开始解码帧。)

Decoding(解码)
==============

This state is reached after the `Capture Setup(捕获设置)` sequence finishes successfully.
In this state, the client queues and dequeues buffers to both queues via
:c:func:`VIDIOC_QBUF` and :c:func:`VIDIOC_DQBUF`, following the standard
semantics.
(`Capture Setup(捕获设置)` 序列成功完成后就进入本状态。在此状态下，客户端遵循标准语义，通过 :c:func:`VIDIOC_QBUF` 和 :c:func:`VIDIOC_DQBUF` 对两个队列进行缓冲区的入队和出队操作。)

The content of the source ``OUTPUT`` buffers depends on the active coded pixel
format and may be affected by codec-specific extended controls, as stated in
the documentation of each format.
(源 ``OUTPUT`` 缓冲区的内容取决于当前生效的编码像素格式，并且可能受特定编解码器扩展控制项的影响，详见各格式的文档。)

Both queues operate independently, following the standard behavior of V4L2
buffer queues and memory-to-memory devices. In addition, the order of decoded
frames dequeued from the ``CAPTURE`` queue may differ from the order of queuing
coded frames to the ``OUTPUT`` queue, due to properties of the selected coded
format, e.g. frame reordering.
(两个队列相互独立地工作，遵循 V4L2 缓冲区队列和内存到内存设备的标准行为。此外，由于所选编码格式的特性（例如帧重排序），从 ``CAPTURE`` 队列出队的已解码帧的顺序，可能与编码帧入队到 ``OUTPUT`` 队列的顺序不同。)

The client must not assume any direct relationship between ``CAPTURE``
and ``OUTPUT`` buffers and any specific timing of buffers becoming
available to dequeue. Specifically:
(客户端不得假设 ``CAPTURE`` 缓冲区与 ``OUTPUT`` 缓冲区之间存在任何直接对应关系，也不得假设缓冲区可供出队的具体时机。具体而言：)

* a buffer queued to ``OUTPUT`` may result in no buffers being produced
  on ``CAPTURE`` (e.g. if it does not contain encoded data, or if only
  metadata syntax structures are present in it),
  (一个入队到 ``OUTPUT`` 的缓冲区可能不会在 ``CAPTURE`` 上产生任何缓冲区（例如其中不含编码数据，或只包含元数据语法结构），)

* a buffer queued to ``OUTPUT`` may result in more than one buffer produced
  on ``CAPTURE`` (if the encoded data contained more than one frame, or if
  returning a decoded frame allowed the decoder to return a frame that
  preceded it in decode, but succeeded it in the display order),
  (一个入队到 ``OUTPUT`` 的缓冲区可能在 ``CAPTURE`` 上产生不止一个缓冲区（如果编码数据包含多帧，或者返回某解码帧使得解码器得以返回一帧在解码顺序上先于它、但在显示顺序上后于它的帧），)

* a buffer queued to ``OUTPUT`` may result in a buffer being produced on
  ``CAPTURE`` later into decode process, and/or after processing further
  ``OUTPUT`` buffers, or be returned out of order, e.g. if display
  reordering is used,
  (一个入队到 ``OUTPUT`` 的缓冲区可能在解码过程较晚时才在 ``CAPTURE`` 上产生缓冲区，和/或在处理了更多 ``OUTPUT`` 缓冲区之后才产生，或者乱序返回（例如使用了显示重排序时），)

* buffers may become available on the ``CAPTURE`` queue without additional
  buffers queued to ``OUTPUT`` (e.g. during drain or ``EOS``), because of the
  ``OUTPUT`` buffers queued in the past whose decoding results are only
  available at later time, due to specifics of the decoding process.
  (即使没有新的缓冲区入队到 ``OUTPUT``，``CAPTURE`` 队列上也可能有缓冲区变得可用（例如在排空或 ``EOS`` 期间），这是因为过去入队的 ``OUTPUT`` 缓冲区，由于其解码过程的特殊性，解码结果要到较晚时候才可用。)

.. note::

   To allow matching decoded ``CAPTURE`` buffers with ``OUTPUT`` buffers they
   originated from, the client can set the ``timestamp`` field of the
   :c:type:`v4l2_buffer` struct when queuing an ``OUTPUT`` buffer. The
   ``CAPTURE`` buffer(s), which resulted from decoding that ``OUTPUT`` buffer
   will have their ``timestamp`` field set to the same value when dequeued.
   (为了能把解码出的 ``CAPTURE`` 缓冲区与其来源的 ``OUTPUT`` 缓冲区对应起来，客户端可以在 ``OUTPUT`` 缓冲区入队时设置 :c:type:`v4l2_buffer` 结构的 ``timestamp`` 字段。由该 ``OUTPUT`` 缓冲区解码产生的 ``CAPTURE`` 缓冲区在出队时，其 ``timestamp`` 字段会被设置为相同的值。)

   In addition to the straightforward case of one ``OUTPUT`` buffer producing
   one ``CAPTURE`` buffer, the following cases are defined:
   (除了一个 ``OUTPUT`` 缓冲区产生一个 ``CAPTURE`` 缓冲区这种简单情形外，还定义了以下几种情况：)

   * one ``OUTPUT`` buffer generates multiple ``CAPTURE`` buffers: the same
     ``OUTPUT`` timestamp will be copied to multiple ``CAPTURE`` buffers.
     (一个 ``OUTPUT`` 缓冲区产生多个 ``CAPTURE`` 缓冲区：相同的 ``OUTPUT`` 时间戳会被复制到多个 ``CAPTURE`` 缓冲区。)

   * multiple ``OUTPUT`` buffers generate one ``CAPTURE`` buffer: timestamp of
     the ``OUTPUT`` buffer queued first will be copied.
     (多个 ``OUTPUT`` 缓冲区产生一个 ``CAPTURE`` 缓冲区：会复制最先入队的 ``OUTPUT`` 缓冲区的时间戳。)

   * the decoding order differs from the display order (i.e. the ``CAPTURE``
     buffers are out-of-order compared to the ``OUTPUT`` buffers): ``CAPTURE``
     timestamps will not retain the order of ``OUTPUT`` timestamps.
     (解码顺序不同于显示顺序（即 ``CAPTURE`` 缓冲区相对于 ``OUTPUT`` 缓冲区是乱序的）：``CAPTURE`` 时间戳不会保持 ``OUTPUT`` 时间戳的顺序。)

.. note::

   The backing memory of ``CAPTURE`` buffers that are used as reference frames
   by the stream may be read by the hardware even after they are dequeued.
   Consequently, the client should avoid writing into this memory while the
   ``CAPTURE`` queue is streaming. Failure to observe this may result in
   corruption of decoded frames.
   (码流用作参考帧的 ``CAPTURE`` 缓冲区，其底层内存在出队之后仍可能被硬件读取。因此，在 ``CAPTURE`` 队列流式传输期间，客户端应避免写入这些内存。违反这一点可能导致已解码帧损坏。)

   Similarly, when using a memory type other than ``V4L2_MEMORY_MMAP``, the
   client should make sure that each ``CAPTURE`` buffer is always queued with
   the same backing memory for as long as the ``CAPTURE`` queue is streaming.
   The reason for this is that V4L2 buffer indices can be used by drivers to
   identify frames. Thus, if the backing memory of a reference frame is
   submitted under a different buffer ID, the driver may misidentify it and
   decode a new frame into it while it is still in use, resulting in corruption
   of the following frames.
   (类似地，当使用的内存类型不是 ``V4L2_MEMORY_MMAP`` 时，只要 ``CAPTURE`` 队列还在流式传输，客户端就应确保每个 ``CAPTURE`` 缓冲区总是带着相同的底层内存入队。原因在于驱动可以使用 V4L2 缓冲区索引来识别帧。于是，如果参考帧的底层内存以不同的缓冲区 ID 提交，驱动可能误判它，并在它仍在使用期间把新帧解码进去，导致后续帧损坏。)

During the decoding, the decoder may initiate one of the special sequences, as
listed below. The sequences will result in the decoder returning all the
``CAPTURE`` buffers that originated from all the ``OUTPUT`` buffers processed
before the sequence started. Last of the buffers will have the
``V4L2_BUF_FLAG_LAST`` flag set. To determine the sequence to follow, the client
must check if there is any pending event and:
(在解码过程中，解码器可能发起下面列出的某个特殊序列。这些序列会使解码器返回由该序列开始之前处理的所有 ``OUTPUT`` 缓冲区产生的全部 ``CAPTURE`` 缓冲区，最后一个缓冲区会设置 ``V4L2_BUF_FLAG_LAST`` 标志。要确定接下来该遵循哪个序列，客户端必须检查是否有待处理事件，并且：)

* if a ``V4L2_EVENT_SOURCE_CHANGE`` event with ``changes`` set to
  ``V4L2_EVENT_SRC_CH_RESOLUTION`` is pending, the `Dynamic Resolution
  Change(动态分辨率变更)` sequence needs to be followed,
  (如果有 ``changes`` 设为 ``V4L2_EVENT_SRC_CH_RESOLUTION`` 的 ``V4L2_EVENT_SOURCE_CHANGE`` 事件待处理，则需要遵循 `Dynamic Resolution Change(动态分辨率变更)` 序列，)

* if a ``V4L2_EVENT_EOS`` event is pending, the `End of Stream(流结束)` sequence needs
  to be followed.
  (如果有 ``V4L2_EVENT_EOS`` 事件待处理，则需要遵循 `End of Stream(流结束)` 序列。)

Some of the sequences can be intermixed with each other and need to be handled
as they happen. The exact operation is documented for each sequence.
(有些序列可能彼此交织，需要在其发生时加以处理。每种序列的具体操作都在各自的文档中说明。)

Should a decoding error occur, it will be reported to the client with the level
of details depending on the decoder capabilities. Specifically:
(如果发生解码错误，将向客户端报告，详细程度取决于解码器的能力。具体而言：)

* the CAPTURE buffer that contains the results of the failed decode operation
  will be returned with the V4L2_BUF_FLAG_ERROR flag set,
  (包含失败解码操作结果的 CAPTURE 缓冲区会带着 V4L2_BUF_FLAG_ERROR 标志返回，)

* if the decoder is able to precisely report the OUTPUT buffer that triggered
  the error, such buffer will be returned with the V4L2_BUF_FLAG_ERROR flag
  set.
  (如果解码器能够精确指出触发错误的 OUTPUT 缓冲区，该缓冲区会带着 V4L2_BUF_FLAG_ERROR 标志返回。)

In case of a fatal failure that does not allow the decoding to continue, any
further operations on corresponding decoder file handle will return the -EIO
error code. The client may close the file handle and open a new one, or
alternatively reinitialize the instance by stopping streaming on both queues,
releasing all buffers and performing the Initialization sequence again.
(如果发生无法继续解码的致命故障，在该解码器文件句柄上的任何后续操作都会返回 -EIO 错误码。客户端可以关闭该文件句柄并打开新的句柄，或者选择重新初始化实例：停止两个队列的流式传输、释放所有缓冲区，然后重新执行 Initialization(初始化) 序列。)

Seek(跳转)
==========

Seek is controlled by the ``OUTPUT`` queue, as it is the source of coded data.
The seek does not require any specific operation on the ``CAPTURE`` queue, but
it may be affected as per normal decoder operation.
(跳转（seek）由 ``OUTPUT`` 队列控制，因为它是编码数据的来源。跳转不需要对 ``CAPTURE`` 队列做任何特定操作，但 ``CAPTURE`` 队列可能按解码器的正常运作受到影响。)

1. Stop the ``OUTPUT`` queue to begin the seek sequence via
   :c:func:`VIDIOC_STREAMOFF`.
   (1. 通过 :c:func:`VIDIOC_STREAMOFF` 停止 ``OUTPUT`` 队列，开始跳转序列。)

   * **Required fields:**
     (必需字段：)

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
         (适合 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

   * The decoder will drop all the pending ``OUTPUT`` buffers and they must be
     treated as returned to the client (following standard semantics).
     (解码器会丢弃所有待处理的 ``OUTPUT`` 缓冲区，必须把它们视为已返回给客户端（遵循标准语义）。)

2. Restart the ``OUTPUT`` queue via :c:func:`VIDIOC_STREAMON`.
   (2. 通过 :c:func:`VIDIOC_STREAMON` 重启 ``OUTPUT`` 队列。)

   * **Required fields:**
     (必需字段：)

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
         (适合 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

   * The decoder will start accepting new source bytestream buffers after the
     call returns.
     (调用返回后，解码器将开始接受新的源字节流缓冲区。)

3. Start queuing buffers containing coded data after the seek to the ``OUTPUT``
   queue until a suitable resume point is found.
   (3. 开始把包含跳转之后编码数据的缓冲区入队到 ``OUTPUT`` 队列，直到找到合适的恢复点。)

   .. note::

      There is no requirement to begin queuing coded data starting exactly
      from a resume point (e.g. SPS or a keyframe). Any queued ``OUTPUT``
      buffers will be processed and returned to the client until a suitable
      resume point is found.  While looking for a resume point, the decoder
      should not produce any decoded frames into ``CAPTURE`` buffers.
      (并不要求恰好从某个恢复点（例如 SPS 或关键帧）开始入队编码数据。在找到合适的恢复点之前，任何入队的 ``OUTPUT`` 缓冲区都会被处理并返回给客户端。在寻找恢复点期间，解码器不应向 ``CAPTURE`` 缓冲区产生任何已解码帧。)

      Some hardware is known to mishandle seeks to a non-resume point. Such an
      operation may result in an unspecified number of corrupted decoded frames
      being made available on the ``CAPTURE`` queue. Drivers must ensure that
      no fatal decoding errors or crashes occur, and implement any necessary
      handling and workarounds for hardware issues related to seek operations.
      (已知某些硬件无法正确处理跳转到非恢复点的情形。这种操作可能导致不定数量的损坏解码帧出现在 ``CAPTURE`` 队列上。驱动必须确保不发生致命解码错误或崩溃，并为与跳转操作相关的硬件问题实现一切必要的处理与变通方案。)

   .. warning::

      In case of the H.264/HEVC codec, the client must take care not to seek
      over a change of SPS/PPS. Even though the target frame could be a
      keyframe, the stale SPS/PPS inside decoder state would lead to undefined
      results when decoding. Although the decoder must handle that case without
      a crash or a fatal decode error, the client must not expect a sensible
      decode output.
      (对于 H.264/HEVC 编解码器，客户端必须注意不要跨 SPS/PPS 变更处跳转。即使目标帧是关键帧，解码器状态中过时的 SPS/PPS 也会导致解码结果未定义。尽管解码器必须在不崩溃、不发生致命解码错误的前提下处理这种情况，客户端也不应指望得到有意义的解码输出。)

      If the hardware can detect such corrupted decoded frames, then
      corresponding buffers will be returned to the client with the
      V4L2_BUF_FLAG_ERROR set. See the `Decoding(解码)` section for further
      description of decode error reporting.
      (如果硬件能检测到这类损坏的解码帧，相应的缓冲区会带着 V4L2_BUF_FLAG_ERROR 标志返回给客户端。解码错误报告的进一步描述参见 `Decoding(解码)` 一节。)

4. After a resume point is found, the decoder will start returning ``CAPTURE``
   buffers containing decoded frames.
   (4. 找到恢复点之后，解码器将开始返回包含已解码帧的 ``CAPTURE`` 缓冲区。)

.. important::

   A seek may result in the `Dynamic Resolution Change(动态分辨率变更)` sequence being
   initiated, due to the seek target having decoding parameters different from
   the part of the stream decoded before the seek. The sequence must be handled
   as per normal decoder operation.
   (由于跳转目标的解码参数可能不同于跳转之前所解码的那部分码流，跳转可能引发 `Dynamic Resolution Change(动态分辨率变更)` 序列。该序列必须按解码器的正常运作加以处理。)

.. warning::

   It is not specified when the ``CAPTURE`` queue starts producing buffers
   containing decoded data from the ``OUTPUT`` buffers queued after the seek,
   as it operates independently from the ``OUTPUT`` queue.
   (``CAPTURE`` 队列何时开始产生包含跳转后入队的 ``OUTPUT`` 缓冲区所解码数据的缓冲区，是没有规定的，因为它独立于 ``OUTPUT`` 队列运作。)

   The decoder may return a number of remaining ``CAPTURE`` buffers containing
   decoded frames originating from the ``OUTPUT`` buffers queued before the
   seek sequence is performed.
   (解码器可能返回若干剩余的 ``CAPTURE`` 缓冲区，其中包含源自执行跳转序列之前所入队 ``OUTPUT`` 缓冲区的已解码帧。)

   The ``VIDIOC_STREAMOFF`` operation discards any remaining queued
   ``OUTPUT`` buffers, which means that not all of the ``OUTPUT`` buffers
   queued before the seek sequence may have matching ``CAPTURE`` buffers
   produced.  For example, given the sequence of operations on the
   ``OUTPUT`` queue:
   (``VIDIOC_STREAMOFF`` 操作会丢弃所有仍在队列中的 ``OUTPUT`` 缓冲区，这意味着跳转序列之前入队的 ``OUTPUT`` 缓冲区未必都会产生匹配的 ``CAPTURE`` 缓冲区。例如，给定对 ``OUTPUT`` 队列的如下操作序列：)

     QBUF(A), QBUF(B), STREAMOFF(), STREAMON(), QBUF(G), QBUF(H),

   any of the following results on the ``CAPTURE`` queue is allowed:
   (``CAPTURE`` 队列上允许出现以下任意一种结果：)

     {A', B', G', H'}, {A', G', H'}, {G', H'}.

   To determine the CAPTURE buffer containing the first decoded frame after the
   seek, the client may observe the timestamps to match the CAPTURE and OUTPUT
   buffers or use V4L2_DEC_CMD_STOP and V4L2_DEC_CMD_START to drain the
   decoder.
   (要确定包含跳转后第一帧已解码帧的 CAPTURE 缓冲区，客户端可以通过观察时间戳来匹配 CAPTURE 与 OUTPUT 缓冲区，或者用 V4L2_DEC_CMD_STOP 和 V4L2_DEC_CMD_START 排空解码器。)

.. note::

   To achieve instantaneous seek, the client may restart streaming on the
   ``CAPTURE`` queue too to discard decoded, but not yet dequeued buffers.
   (为了实现瞬时跳转，客户端也可以在 ``CAPTURE`` 队列上重启流式传输，以丢弃已解码但尚未出队的缓冲区。)

Dynamic Resolution Change(动态分辨率变更)
=========================================

Streams that include resolution metadata in the bytestream may require switching
to a different resolution during the decoding.
(字节流中包含分辨率元数据的码流，可能需要在解码过程中切换到不同的分辨率。)

.. note::

   Not all decoders can detect resolution changes. Those that do set the
   ``V4L2_FMT_FLAG_DYN_RESOLUTION`` flag for the coded format when
   :c:func:`VIDIOC_ENUM_FMT` is called.
   (并非所有解码器都能检测分辨率变化。能检测的那些解码器，会在调用 :c:func:`VIDIOC_ENUM_FMT` 时为该编码格式设置 ``V4L2_FMT_FLAG_DYN_RESOLUTION`` 标志。)

The sequence starts when the decoder detects a coded frame with one or more of
the following parameters different from those previously established (and
reflected by corresponding queries):
(当解码器检测到某个编码帧的以下一个或多个参数与先前确立的值（并已反映在相应查询中）不同时，序列开始：)

* coded resolution (``OUTPUT`` width and height),
  (编码分辨率（``OUTPUT`` 的宽和高），)

* visible resolution (selection rectangles),
  (可见分辨率（选择矩形），)

* the minimum number of buffers needed for decoding,
  (解码所需的最少缓冲区数量，)

* bit-depth of the bitstream has been changed,
  (码流的位深发生了变化，)

* colorspace of the bitstream has been changed, but it doesn't require
  buffer reallocation.
  (码流的色彩空间发生了变化，但不需要重新分配缓冲区。)

Whenever that happens, the decoder must proceed as follows:
(每当发生这种情况，解码器必须按如下流程处理：)

1.  After encountering a resolution change in the stream, the decoder sends a
    ``V4L2_EVENT_SOURCE_CHANGE`` event with ``changes`` set to
    ``V4L2_EVENT_SRC_CH_RESOLUTION``.
    (1. 在码流中遇到分辨率变化后，解码器发送 ``changes`` 设为 ``V4L2_EVENT_SRC_CH_RESOLUTION`` 的 ``V4L2_EVENT_SOURCE_CHANGE`` 事件。)

    .. important::

       Any client query issued after the decoder queues the event will return
       values applying to the stream after the resolution change, including
       queue formats, selection rectangles and controls.
       (在解码器把事件入队之后，客户端发出的任何查询都会返回适用于分辨率变更之后码流的值，包括队列格式、选择矩形和控制项。)

2.  The decoder will then process and decode all remaining buffers from before
    the resolution change point.
    (2. 然后，解码器会处理并解码分辨率变更点之前的所有剩余缓冲区。)

    * The last buffer from before the change must be marked with the
      ``V4L2_BUF_FLAG_LAST`` flag, similarly to the `Drain(排空)` sequence above.
      (变更之前的最后一个缓冲区必须带有 ``V4L2_BUF_FLAG_LAST`` 标志，类似于上文的 `Drain(排空)` 序列。)

    .. warning::

       The last buffer may be empty (with :c:type:`v4l2_buffer` ``bytesused``
       = 0) and in that case it must be ignored by the client, as it does not
       contain a decoded frame.
       (最后一个缓冲区可能为空（:c:type:`v4l2_buffer` 的 ``bytesused`` 为 0），这种情况下客户端必须忽略它，因为它不包含已解码帧。)

    .. note::

       Any attempt to dequeue more ``CAPTURE`` buffers beyond the buffer marked
       with ``V4L2_BUF_FLAG_LAST`` will result in a -EPIPE error from
       :c:func:`VIDIOC_DQBUF`.
       (任何在带有 ``V4L2_BUF_FLAG_LAST`` 标志的缓冲区之后继续出队更多 ``CAPTURE`` 缓冲区的尝试，都会从 :c:func:`VIDIOC_DQBUF` 得到 -EPIPE 错误。)

The client must continue the sequence as described below to continue the
decoding process.
(客户端必须按下述说明继续该序列，以继续解码过程。)

1.  Dequeue the source change event.
    (1. 出队源变更事件。)

    .. important::

       A source change triggers an implicit decoder drain, similar to the
       explicit `Drain(排空)` sequence. The decoder is stopped after it completes.
       The decoding process must be resumed with either a pair of calls to
       :c:func:`VIDIOC_STREAMOFF` and :c:func:`VIDIOC_STREAMON` on the
       ``CAPTURE`` queue, or a call to :c:func:`VIDIOC_DECODER_CMD` with the
       ``V4L2_DEC_CMD_START`` command.
       (源变更会触发一次隐式的解码器排空，类似于显式的 `Drain(排空)` 序列。完成后解码器处于停止状态。必须通过以下方式之一恢复解码过程：在 ``CAPTURE`` 队列上成对调用 :c:func:`VIDIOC_STREAMOFF` 和 :c:func:`VIDIOC_STREAMON`，或者用 ``V4L2_DEC_CMD_START`` 命令调用 :c:func:`VIDIOC_DECODER_CMD`。)

2.  Continue with the `Capture Setup(捕获设置)` sequence.
    (2. 继续 `Capture Setup(捕获设置)` 序列。)

.. note::

   During the resolution change sequence, the ``OUTPUT`` queue must remain
   streaming. Calling :c:func:`VIDIOC_STREAMOFF` on the ``OUTPUT`` queue would
   abort the sequence and initiate a seek.
   (在分辨率变更序列期间，``OUTPUT`` 队列必须保持流式传输。对 ``OUTPUT`` 队列调用 :c:func:`VIDIOC_STREAMOFF` 会中止该序列并发起一次跳转。)

   In principle, the ``OUTPUT`` queue operates separately from the ``CAPTURE``
   queue and this remains true for the duration of the entire resolution change
   sequence as well.
   (原则上，``OUTPUT`` 队列独立于 ``CAPTURE`` 队列运作，在整个分辨率变更序列期间也是如此。)

   The client should, for best performance and simplicity, keep queuing/dequeuing
   buffers to/from the ``OUTPUT`` queue even while processing this sequence.
   (为了获得最佳性能并保持简单，客户端即使在处理该序列期间，也应继续保持对 ``OUTPUT`` 队列的入队/出队操作。)

Drain(排空)
===========

To ensure that all queued ``OUTPUT`` buffers have been processed and related
``CAPTURE`` buffers are given to the client, the client must follow the drain
sequence described below. After the drain sequence ends, the client has
received all decoded frames for all ``OUTPUT`` buffers queued before the
sequence was started.
(为确保所有已入队的 ``OUTPUT`` 缓冲区都得到处理、相关的 ``CAPTURE`` 缓冲区都已交给客户端，客户端必须遵循下述排空序列。排空序列结束后，客户端已收到在该序列开始之前入队的所有 ``OUTPUT`` 缓冲区的全部已解码帧。)

1. Begin drain by issuing :c:func:`VIDIOC_DECODER_CMD`.
   (1. 通过发出 :c:func:`VIDIOC_DECODER_CMD` 开始排空。)

   * **Required fields:**
     (必需字段：)

     ``cmd``
         set to ``V4L2_DEC_CMD_STOP``.
         (设置为 ``V4L2_DEC_CMD_STOP``。)

     ``flags``
         set to 0.
         (设置为 0。)

     ``pts``
         set to 0.
         (设置为 0。)

   .. warning::

      The sequence can be only initiated if both ``OUTPUT`` and ``CAPTURE``
      queues are streaming. For compatibility reasons, the call to
      :c:func:`VIDIOC_DECODER_CMD` will not fail even if any of the queues is
      not streaming, but at the same time it will not initiate the `Drain(排空)`
      sequence and so the steps described below would not be applicable.
      (只有当 ``OUTPUT`` 和 ``CAPTURE`` 两个队列都在流式传输时才能发起该序列。出于兼容性原因，即使有队列未在流式传输，:c:func:`VIDIOC_DECODER_CMD` 调用也不会失败，但与此同时它不会发起 `Drain(排空)` 序列，因此下述步骤也就不适用。)

2. Any ``OUTPUT`` buffers queued by the client before the
   :c:func:`VIDIOC_DECODER_CMD` was issued will be processed and decoded as
   normal. The client must continue to handle both queues independently,
   similarly to normal decode operation. This includes:
   (2. 在发出 :c:func:`VIDIOC_DECODER_CMD` 之前由客户端入队的所有 ``OUTPUT`` 缓冲区都会照常被处理和解码。客户端必须继续像正常解码操作那样独立地处理两个队列。这包括：)

   * handling any operations triggered as a result of processing those buffers,
     such as the `Dynamic Resolution Change(动态分辨率变更)` sequence, before continuing with
     the drain sequence,
     (在继续排空序列之前，先处理因处理这些缓冲区而触发的任何操作，例如 `Dynamic Resolution Change(动态分辨率变更)` 序列，)

   * queuing and dequeuing ``CAPTURE`` buffers, until a buffer marked with the
     ``V4L2_BUF_FLAG_LAST`` flag is dequeued,
     (对 ``CAPTURE`` 缓冲区的入队和出队，直到出队一个带有 ``V4L2_BUF_FLAG_LAST`` 标志的缓冲区，)

     .. warning::

        The last buffer may be empty (with :c:type:`v4l2_buffer`
        ``bytesused`` = 0) and in that case it must be ignored by the client,
        as it does not contain a decoded frame.
        (最后一个缓冲区可能为空（:c:type:`v4l2_buffer` 的 ``bytesused`` 为 0），这种情况下客户端必须忽略它，因为它不包含已解码帧。)

     .. note::

        Any attempt to dequeue more ``CAPTURE`` buffers beyond the buffer
        marked with ``V4L2_BUF_FLAG_LAST`` will result in a -EPIPE error from
        :c:func:`VIDIOC_DQBUF`.
        (任何在带有 ``V4L2_BUF_FLAG_LAST`` 标志的缓冲区之后继续出队更多 ``CAPTURE`` 缓冲区的尝试，都会从 :c:func:`VIDIOC_DQBUF` 得到 -EPIPE 错误。)

   * dequeuing processed ``OUTPUT`` buffers, until all the buffers queued
     before the ``V4L2_DEC_CMD_STOP`` command are dequeued,
     (出队已处理的 ``OUTPUT`` 缓冲区，直到 ``V4L2_DEC_CMD_STOP`` 命令之前入队的所有缓冲区都被出队，)

   * dequeuing the ``V4L2_EVENT_EOS`` event, if the client subscribed to it.
     (如果客户端订阅了该事件，则出队 ``V4L2_EVENT_EOS`` 事件。)

   .. note::

      For backwards compatibility, the decoder will signal a ``V4L2_EVENT_EOS``
      event when the last frame has been decoded and all frames are ready to be
      dequeued. It is a deprecated behavior and the client must not rely on it.
      The ``V4L2_BUF_FLAG_LAST`` buffer flag should be used instead.
      (出于向后兼容的考虑，当最后一帧解码完成、所有帧都可出队时，解码器会发出 ``V4L2_EVENT_EOS`` 事件。这是一种已弃用的行为，客户端不得依赖它。应当改用 ``V4L2_BUF_FLAG_LAST`` 缓冲区标志。)

3. Once all the ``OUTPUT`` buffers queued before the ``V4L2_DEC_CMD_STOP`` call
   are dequeued and the last ``CAPTURE`` buffer is dequeued, the decoder is
   stopped and it will accept, but not process, any newly queued ``OUTPUT``
   buffers until the client issues any of the following operations:
   (3. 一旦 ``V4L2_DEC_CMD_STOP`` 调用之前入队的所有 ``OUTPUT`` 缓冲区都被出队，且最后一个 ``CAPTURE`` 缓冲区也被出队，解码器就进入停止状态；它会接受但不处理任何新入队的 ``OUTPUT`` 缓冲区，直到客户端发出以下操作之一：)

   * ``V4L2_DEC_CMD_START`` - the decoder will not be reset and will resume
     operation normally, with all the state from before the drain,
     (``V4L2_DEC_CMD_START``——解码器不会被重置，将带着排空之前的全部状态正常恢复运作，)

   * a pair of :c:func:`VIDIOC_STREAMOFF` and :c:func:`VIDIOC_STREAMON` on the
     ``CAPTURE`` queue - the decoder will resume the operation normally,
     however any ``CAPTURE`` buffers still in the queue will be returned to the
     client,
     (在 ``CAPTURE`` 队列上成对调用 :c:func:`VIDIOC_STREAMOFF` 和 :c:func:`VIDIOC_STREAMON`——解码器将正常恢复运作，但队列中仍在的任何 ``CAPTURE`` 缓冲区都会返回给客户端，)

   * a pair of :c:func:`VIDIOC_STREAMOFF` and :c:func:`VIDIOC_STREAMON` on the
     ``OUTPUT`` queue - any pending source buffers will be returned to the
     client and the `Seek(跳转)` sequence will be triggered.
     (在 ``OUTPUT`` 队列上成对调用 :c:func:`VIDIOC_STREAMOFF` 和 :c:func:`VIDIOC_STREAMON`——所有待处理的源缓冲区都会返回给客户端，并触发 `Seek(跳转)` 序列。)

.. note::

   Once the drain sequence is initiated, the client needs to drive it to
   completion, as described by the steps above, unless it aborts the process by
   issuing :c:func:`VIDIOC_STREAMOFF` on any of the ``OUTPUT`` or ``CAPTURE``
   queues.  The client is not allowed to issue ``V4L2_DEC_CMD_START`` or
   ``V4L2_DEC_CMD_STOP`` again while the drain sequence is in progress and they
   will fail with -EBUSY error code if attempted.
   (排空序列一旦发起，客户端就必须按上述步骤把它执行完毕，除非它通过对 ``OUTPUT`` 或 ``CAPTURE`` 任一队列发出 :c:func:`VIDIOC_STREAMOFF` 来中止该过程。在排空序列进行期间，不允许客户端再次发出 ``V4L2_DEC_CMD_START`` 或 ``V4L2_DEC_CMD_STOP``；如果尝试，会以 -EBUSY 错误码失败。)

   Although not mandatory, the availability of decoder commands may be queried
   using :c:func:`VIDIOC_TRY_DECODER_CMD`.
   (虽然并非强制，但可以用 :c:func:`VIDIOC_TRY_DECODER_CMD` 查询解码器命令的可用性。)

End of Stream(流结束)
=====================

If the decoder encounters an end of stream marking in the stream, the decoder
will initiate the `Drain(排空)` sequence, which the client must handle as described
above, skipping the initial :c:func:`VIDIOC_DECODER_CMD`.
(如果解码器在码流中遇到流结束标记，解码器会发起 `Drain(排空)` 序列；客户端必须按上文所述处理该序列，只是跳过最初的 :c:func:`VIDIOC_DECODER_CMD`。)

Commit Points(提交点)
=====================

Setting formats and allocating buffers trigger changes in the behavior of the
decoder.
(设置格式和分配缓冲区会触发解码器行为的改变。)

1. Setting the format on the ``OUTPUT`` queue may change the set of formats
   supported/advertised on the ``CAPTURE`` queue. In particular, it also means
   that the ``CAPTURE`` format may be reset and the client must not rely on the
   previously set format being preserved.
   (1. 在 ``OUTPUT`` 队列上设置格式可能改变 ``CAPTURE`` 队列支持/通告的格式集合。特别是，这也意味着 ``CAPTURE`` 格式可能被重置，客户端不得依赖先前设置的格式得以保留。)

2. Enumerating formats on the ``CAPTURE`` queue always returns only formats
   supported for the current ``OUTPUT`` format.
   (2. 在 ``CAPTURE`` 队列上枚举格式，始终只返回当前 ``OUTPUT`` 格式所支持的格式。)

3. Setting the format on the ``CAPTURE`` queue does not change the list of
   formats available on the ``OUTPUT`` queue. An attempt to set a ``CAPTURE``
   format that is not supported for the currently selected ``OUTPUT`` format
   will result in the decoder adjusting the requested ``CAPTURE`` format to a
   supported one.
   (3. 在 ``CAPTURE`` 队列上设置格式不会改变 ``OUTPUT`` 队列上可用的格式列表。如果尝试设置的 ``CAPTURE`` 格式不被当前所选 ``OUTPUT`` 格式支持，解码器会把请求的 ``CAPTURE`` 格式调整为受支持的格式。)

4. Enumerating formats on the ``OUTPUT`` queue always returns the full set of
   supported coded formats, irrespectively of the current ``CAPTURE`` format.
   (4. 在 ``OUTPUT`` 队列上枚举格式，始终返回受支持编码格式的全集，与当前 ``CAPTURE`` 格式无关。)

5. While buffers are allocated on any of the ``OUTPUT`` or ``CAPTURE`` queues,
   the client must not change the format on the ``OUTPUT`` queue. Drivers will
   return the -EBUSY error code for any such format change attempt.
   (5. 只要 ``OUTPUT`` 或 ``CAPTURE`` 任一队列上已分配缓冲区，客户端就不得更改 ``OUTPUT`` 队列的格式。对此类格式更改尝试，驱动都会返回 -EBUSY 错误码。)

To summarize, setting formats and allocation must always start with the
``OUTPUT`` queue and the ``OUTPUT`` queue is the master that governs the
set of supported formats for the ``CAPTURE`` queue.
(总而言之，设置格式和分配缓冲区必须始终从 ``OUTPUT`` 队列开始，``OUTPUT`` 队列是主宰，决定着 ``CAPTURE`` 队列受支持格式的集合。)
