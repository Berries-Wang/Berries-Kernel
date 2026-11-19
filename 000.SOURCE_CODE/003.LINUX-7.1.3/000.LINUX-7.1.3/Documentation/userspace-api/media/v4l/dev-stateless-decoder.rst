.. SPDX-License-Identifier: GPL-2.0
.. c:namespace:: V4L

.. _stateless_decoder:

**********************************************************************************
Memory-to-memory Stateless Video Decoder Interface(内存到内存无状态视频解码器接口)
**********************************************************************************

A stateless decoder is a decoder that works without retaining any kind of state
between processed frames. This means that each frame is decoded independently
of any previous and future frames, and that the client is responsible for
maintaining the decoding state and providing it to the decoder with each
decoding request. This is in contrast to the stateful video decoder interface,
where the hardware and driver maintain the decoding state and all the client
has to do is to provide the raw encoded stream and dequeue decoded frames in
display order.
(无状态解码器是一种在处理帧之间不保留任何状态而工作的解码器。这意味着每一帧的解码都独立于之前与之后的帧，由客户端负责维护解码状态，并在每次解码请求中把状态提供给解码器。这与有状态视频解码器接口形成对比：在有状态接口中，硬件与驱动维护解码状态，客户端只需提供原始编码码流，并按显示顺序出队解码后的帧。)

This section describes how user-space ("the client") is expected to communicate
with stateless decoders in order to successfully decode an encoded stream.
Compared to stateful codecs, the decoder/client sequence is simpler, but the
cost of this simplicity is extra complexity in the client which is responsible
for maintaining a consistent decoding state.
(本节描述用户空间("客户端")应当如何与无状态解码器通信，才能成功解码一段编码码流。与有状态编解码器相比，解码器/客户端的时序更简单，但这种简单性的代价是客户端中额外的复杂性——客户端负责维护一致的解码状态。)

Stateless decoders make use of the :ref:`media-request-api`. A stateless
decoder must expose the ``V4L2_BUF_CAP_SUPPORTS_REQUESTS`` capability on its
``OUTPUT`` queue when :c:func:`VIDIOC_REQBUFS` or :c:func:`VIDIOC_CREATE_BUFS`
are invoked.
(无状态解码器使用 :ref:`media-request-api` 。无状态解码器在 :c:func:`VIDIOC_REQBUFS` 或 :c:func:`VIDIOC_CREATE_BUFS` 被调用时，必须在它的 ``OUTPUT`` 队列上暴露 ``V4L2_BUF_CAP_SUPPORTS_REQUESTS`` 能力。)

Depending on the encoded formats supported by the decoder, a single decoded
frame may be the result of several decode requests (for instance, H.264 streams
with multiple slices per frame). Decoders that support such formats must also
expose the ``V4L2_BUF_CAP_SUPPORTS_M2M_HOLD_CAPTURE_BUF`` capability on their
``OUTPUT`` queue.
(根据解码器支持的编码格式，一个解码后的帧可能是多次解码请求的结果(例如每帧含多个 slice 的 H.264 码流)。支持此类格式的解码器还必须在 ``OUTPUT`` 队列上暴露 ``V4L2_BUF_CAP_SUPPORTS_M2M_HOLD_CAPTURE_BUF`` 能力。)

Querying capabilities(查询能力)

1. To enumerate the set of coded formats supported by the decoder, the client
   calls :c:func:`VIDIOC_ENUM_FMT` on the ``OUTPUT`` queue.
   (1. 要枚举解码器支持的编码格式集合，客户端在 ``OUTPUT`` 队列上调用 :c:func:`VIDIOC_ENUM_FMT` 。)

   * The driver must always return the full set of supported ``OUTPUT`` formats,
     irrespective of the format currently set on the ``CAPTURE`` queue.
     (驱动必须总是返回 ``OUTPUT`` 所支持格式的完整集合，而不管 ``CAPTURE`` 队列当前设置的格式是什么。)

   * Simultaneously, the driver must restrain the set of values returned by
     codec-specific capability controls (such as H.264 profiles) to the set
     actually supported by the hardware.
     (同时，驱动必须把编解码器特定能力控制(如 H.264 profile)返回的取值集合限定为硬件实际支持的集合。)

2. To enumerate the set of supported raw formats, the client calls
   :c:func:`VIDIOC_ENUM_FMT` on the ``CAPTURE`` queue.
   (2. 要枚举支持的原始格式集合，客户端在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_ENUM_FMT` 。)

   * The driver must return only the formats supported for the format currently
     active on the ``OUTPUT`` queue.
     (驱动必须只返回对 ``OUTPUT`` 队列上当前活动格式所支持的格式。)

   * Depending on the currently set ``OUTPUT`` format, the set of supported raw
     formats may depend on the value of some codec-dependent controls.
     The client is responsible for making sure that these controls are set
     before querying the ``CAPTURE`` queue. Failure to do so will result in the
     default values for these controls being used, and a returned set of formats
     that may not be usable for the media the client is trying to decode.
     (根据当前设置的 ``OUTPUT`` 格式，支持的原始格式集合可能取决于某些编解码器相关控制的值。客户端负责确保在查询 ``CAPTURE`` 队列之前设置好这些控制。不这样做会导致使用这些控制的默认值，返回的格式集合可能不适用于客户端想要解码的媒体。)

3. The client may use :c:func:`VIDIOC_ENUM_FRAMESIZES` to detect supported
   resolutions for a given format, passing desired pixel format in
   :c:type:`v4l2_frmsizeenum`'s ``pixel_format``.
   (3. 客户端可以用 :c:func:`VIDIOC_ENUM_FRAMESIZES` 来检测给定格式支持的分辨率，把期望的像素格式传入 :c:type:`v4l2_frmsizeenum` 的 ``pixel_format`` 。)

4. Supported profiles and levels for the current ``OUTPUT`` format, if
   applicable, may be queried using their respective controls via
   :c:func:`VIDIOC_QUERYCTRL`.
   (4. 当前 ``OUTPUT`` 格式支持的 profile 与 level(如适用)，可以通过相应的控制、用 :c:func:`VIDIOC_QUERYCTRL` 来查询。)

Initialization(初始化)
======================

1. Set the coded format on the ``OUTPUT`` queue via :c:func:`VIDIOC_S_FMT`.
   (1. 通过 :c:func:`VIDIOC_S_FMT` 在 ``OUTPUT`` 队列上设置编码格式。)

   * **Required fields:**
     (必填字段：)

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``OUTPUT``.
         (适合 ``OUTPUT`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

     ``pixelformat``
         a coded pixel format.
         (一个编码像素格式。)

     ``width``, ``height``
         coded width and height parsed from the stream.
         (从码流中解析出的编码宽度与高度。)

     other fields
         follow standard semantics.
         (遵循标准语义。)

   .. note::

      Changing the ``OUTPUT`` format may change the currently set ``CAPTURE``
      format. The driver will derive a new ``CAPTURE`` format from the
      ``OUTPUT`` format being set, including resolution, colorimetry
      parameters, etc. If the client needs a specific ``CAPTURE`` format,
      it must adjust it afterwards.
      (更改 ``OUTPUT`` 格式可能会更改当前设置的 ``CAPTURE`` 格式。驱动会根据正在设置的 ``OUTPUT`` 格式派生新的 ``CAPTURE`` 格式，包括分辨率、色度(colorimetry)参数等。如果客户端需要特定的 ``CAPTURE`` 格式，它必须在此后进行调整。)

2. Call :c:func:`VIDIOC_S_EXT_CTRLS` to set all the controls (parsed headers,
   etc.) required by the ``OUTPUT`` format to enumerate the ``CAPTURE`` formats.
   (2. 调用 :c:func:`VIDIOC_S_EXT_CTRLS` 设置 ``OUTPUT`` 格式所需的全部控制(解析出的头信息等)，以枚举 ``CAPTURE`` 格式。)

3. Call :c:func:`VIDIOC_G_FMT` for ``CAPTURE`` queue to get the format for the
   destination buffers parsed/decoded from the bytestream.
   (3. 对 ``CAPTURE`` 队列调用 :c:func:`VIDIOC_G_FMT`，获取从字节流解析/解码得到的目标缓冲区的格式。)

   * **Required fields:**
     (必填字段：)

     ``type``
         a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
         (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

   * **Returned fields:**
     (返回字段：)

     ``width``, ``height``
         frame buffer resolution for the decoded frames.
         (解码帧的帧缓冲分辨率。)

     ``pixelformat``
         pixel format for decoded frames.
         (解码帧的像素格式。)

     ``num_planes`` (for _MPLANE ``type`` only)
         number of planes for pixelformat.
         (像素格式的平面数。)

     ``sizeimage``, ``bytesperline``
         as per standard semantics; matching frame buffer format.
         (遵循标准语义；与帧缓冲格式匹配。)

   .. note::

      The value of ``pixelformat`` may be any pixel format supported for the
      ``OUTPUT`` format, based on the hardware capabilities. It is suggested
      that the driver chooses the preferred/optimal format for the current
      configuration. For example, a YUV format may be preferred over an RGB
      format, if an additional conversion step would be required for RGB.
      ( ``pixelformat`` 的值可以是 ``OUTPUT`` 格式所支持的任意像素格式(取决于硬件能力)。建议驱动为当前配置选择首选/最优的格式。例如，如果选用 RGB 需要额外的转换步骤，则可能优先选择 YUV 格式而非 RGB 格式。)

4. *[optional]* Enumerate ``CAPTURE`` formats via :c:func:`VIDIOC_ENUM_FMT` on
   the ``CAPTURE`` queue. The client may use this ioctl to discover which
   alternative raw formats are supported for the current ``OUTPUT`` format and
   select one of them via :c:func:`VIDIOC_S_FMT`.
   (4. *[可选]* 通过在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_ENUM_FMT` 枚举 ``CAPTURE`` 格式。客户端可以用这个 ioctl 发现当前 ``OUTPUT`` 格式支持哪些可选的原始格式，并通过 :c:func:`VIDIOC_S_FMT` 选择其中之一。)

   .. note::

      The driver will return only formats supported for the currently selected
      ``OUTPUT`` format and currently set controls, even if more formats may be
      supported by the decoder in general.

      For example, a decoder may support YUV and RGB formats for
      resolutions 1920x1088 and lower, but only YUV for higher resolutions (due
      to hardware limitations). After setting a resolution of 1920x1088 or lower
      as the ``OUTPUT`` format, :c:func:`VIDIOC_ENUM_FMT` may return a set of
      YUV and RGB pixel formats, but after setting a resolution higher than
      1920x1088, the driver will not return RGB pixel formats, since they are
      unsupported for this resolution.
      (驱动只会返回对当前选择的 ``OUTPUT`` 格式与当前设置的控制所支持的格式，即使解码器总体上可能支持更多格式。)

      (例如，解码器可能对 1920x1088 及以下分辨率支持 YUV 与 RGB 格式，但对更高分辨率只支持 YUV(由于硬件限制)。在把 1920x1088 或更低的分辨率设为 ``OUTPUT`` 格式之后， :c:func:`VIDIOC_ENUM_FMT` 可能返回一组 YUV 与 RGB 像素格式；而在设置高于 1920x1088 的分辨率之后，驱动将不会返回 RGB 像素格式，因为该分辨率不支持它们。)

5. *[optional]* Choose a different ``CAPTURE`` format than suggested via
   :c:func:`VIDIOC_S_FMT` on ``CAPTURE`` queue. It is possible for the client to
   choose a different format than selected/suggested by the driver in
   :c:func:`VIDIOC_G_FMT`.
   (5. *[可选]* 通过在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_S_FMT`，选择一个不同于驱动建议的 ``CAPTURE`` 格式。客户端可以选用与驱动在 :c:func:`VIDIOC_G_FMT` 中选择/建议的格式不同的格式。)

    * **Required fields:**
      (必填字段：)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
          (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``pixelformat``
          a raw pixel format.
          (一个原始像素格式。)

      ``width``, ``height``
         frame buffer resolution of the decoded stream; typically unchanged from
         what was returned with :c:func:`VIDIOC_G_FMT`, but it may be different
         if the hardware supports composition and/or scaling.
         (解码码流的帧缓冲分辨率；通常与 :c:func:`VIDIOC_G_FMT` 返回的相同，但如果硬件支持合成和/或缩放，则可能不同。)

   After performing this step, the client must perform step 3 again in order
   to obtain up-to-date information about the buffers size and layout.
   (执行完这一步后，客户端必须再次执行第 3 步，以获得关于缓冲区大小与布局的最新信息。)

6. Allocate source (bytestream) buffers via :c:func:`VIDIOC_REQBUFS` on
   ``OUTPUT`` queue.
   (6. 通过 :c:func:`VIDIOC_REQBUFS` 在 ``OUTPUT`` 队列上分配源(字节流)缓冲区。)

    * **Required fields:**

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
          actual number of buffers allocated.
          (实际分配的缓冲区数量。)

    * If required, the driver will adjust ``count`` to be equal or bigger to the
      minimum of required number of ``OUTPUT`` buffers for the given format and
      requested count. The client must check this value after the ioctl returns
      to get the actual number of buffers allocated.
      (如有需要，驱动会把 ``count`` 调整为不小于——给定格式所需的最少 ``OUTPUT`` 缓冲区数量与请求的数量这两者中的较大值——的数。客户端必须在 ioctl 返回后检查该值，以获得实际分配的缓冲区数量。)

7. Allocate destination (raw format) buffers via :c:func:`VIDIOC_REQBUFS` on the
   ``CAPTURE`` queue.
   (7. 通过 :c:func:`VIDIOC_REQBUFS` 在 ``CAPTURE`` 队列上分配目标(原始格式)缓冲区。)

    * **Required fields:**

      ``count``
          requested number of buffers to allocate; greater than zero. The client
          is responsible for deducing the minimum number of buffers required
          for the stream to be properly decoded (taking e.g. reference frames
          into account) and pass an equal or bigger number.
          (请求分配的缓冲区数量；大于零。客户端负责推断该码流被正确解码所需的最少缓冲区数量(例如考虑参考帧)，并传入不小于该数量的值。)

      ``type``
          a ``V4L2_BUF_TYPE_*`` enum appropriate for ``CAPTURE``.
          (适合 ``CAPTURE`` 的 ``V4L2_BUF_TYPE_*`` 枚举值。)

      ``memory``
          follows standard semantics. ``V4L2_MEMORY_USERPTR`` is not supported
          for ``CAPTURE`` buffers.
          (遵循标准语义。 ``CAPTURE`` 缓冲区不支持 ``V4L2_MEMORY_USERPTR`` 。)

    * **Returned fields:**

      ``count``
          adjusted to allocated number of buffers, in case the codec requires
          more buffers than requested.
          (在编解码器需要比请求更多的缓冲区时，调整为实际分配的缓冲区数量。)

    * The driver must adjust count to the minimum of required number of
      ``CAPTURE`` buffers for the current format, stream configuration and
      requested count. The client must check this value after the ioctl
      returns to get the number of buffers allocated.
      (驱动必须把 count 调整为当前格式、码流配置与请求数量下所需的最少 ``CAPTURE`` 缓冲区数量。客户端必须在 ioctl 返回后检查该值，以获得分配的缓冲区数量。)

8. Allocate requests (likely one per ``OUTPUT`` buffer) via
    :c:func:`MEDIA_IOC_REQUEST_ALLOC` on the media device.
    (8. 通过 :c:func:`MEDIA_IOC_REQUEST_ALLOC` 在媒体设备上分配 request(通常每个 ``OUTPUT`` 缓冲区一个)。)

9. Start streaming on both ``OUTPUT`` and ``CAPTURE`` queues via
    :c:func:`VIDIOC_STREAMON`.
    (9. 通过 :c:func:`VIDIOC_STREAMON` 在 ``OUTPUT`` 与 ``CAPTURE`` 两个队列上开始流式传输。)

Decoding(解码)

For each frame, the client is responsible for submitting at least one request to
which the following is attached:
(对于每一帧，客户端负责提交至少一个 request，并把以下内容附加到其上：)

* The amount of encoded data expected by the codec for its current
  configuration, as a buffer submitted to the ``OUTPUT`` queue. Typically, this
  corresponds to one frame worth of encoded data, but some formats may allow (or
  require) different amounts per unit.
  (编解码器在其当前配置下所期望的编码数据量，以提交到 ``OUTPUT`` 队列的缓冲区形式给出。通常这对应于一帧的编码数据量，但某些格式可能允许(或要求)每个单元不同的数量。)
* All the metadata needed to decode the submitted encoded data, in the form of
  controls relevant to the format being decoded.
  (解码所提交编码数据所需的全部元数据，以与所解码格式相关的控制的形式给出。)

The amount of data and contents of the source ``OUTPUT`` buffer, as well as the
controls that must be set on the request, depend on the active coded pixel
format and might be affected by codec-specific extended controls, as stated in
documentation of each format.
(源 ``OUTPUT`` 缓冲区的数据量与内容，以及必须在 request 上设置的控制，取决于活动的编码像素格式，并可能受编解码器特定扩展控制的影响，正如每种格式的文档所述。)

If there is a possibility that the decoded frame will require one or more
decode requests after the current one in order to be produced, then the client
must set the ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` flag on the ``OUTPUT``
buffer. This will result in the (potentially partially) decoded ``CAPTURE``
buffer not being made available for dequeueing, and reused for the next decode
request if the timestamp of the next ``OUTPUT`` buffer has not changed.
(如果解码后的帧有可能需要在当前请求之后再来一次或多次解码请求才能产生，那么客户端必须在 ``OUTPUT`` 缓冲区上设置 ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` 标志。这将导致(可能部分)解码的 ``CAPTURE`` 缓冲区不被开放给出队，并在下一个 ``OUTPUT`` 缓冲区的时间戳未改变时被复用于下一次解码请求。)

A typical frame would thus be decoded using the following sequence:
(因此，一帧典型的解码会使用以下时序：)

1. Queue an ``OUTPUT`` buffer containing one unit of encoded bytestream data for
   the decoding request, using :c:func:`VIDIOC_QBUF`.
   (1. 使用 :c:func:`VIDIOC_QBUF` 把一个含有一单位编码字节流数据的 ``OUTPUT`` 缓冲区入队，供解码请求使用。)

    * **Required fields:**

      ``index``
          index of the buffer being queued.
          (正在入队的缓冲区的索引。)

      ``type``
          type of the buffer.
          (缓冲区的类型。)

      ``bytesused``
          number of bytes taken by the encoded data frame in the buffer.
          (缓冲区中编码数据帧占用的字节数。)

      ``flags``
          the ``V4L2_BUF_FLAG_REQUEST_FD`` flag must be set. Additionally, if
          we are not sure that the current decode request is the last one needed
          to produce a fully decoded frame, then
          ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` must also be set.
          (必须设置 ``V4L2_BUF_FLAG_REQUEST_FD`` 标志。此外，如果我们不确定当前解码请求是否是产生完整解码帧所需的最后一个请求，则还必须设置 ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` 。)

      ``request_fd``
          must be set to the file descriptor of the decoding request.
          (必须设置为解码请求的文件描述符。)

      ``timestamp``
          must be set to a unique value per frame. This value will be propagated
          into the decoded frame's buffer and can also be used to use this frame
          as the reference of another. If using multiple decode requests per
          frame, then the timestamps of all the ``OUTPUT`` buffers for a given
          frame must be identical. If the timestamp changes, then the currently
          held ``CAPTURE`` buffer will be made available for dequeuing and the
          current request will work on a new ``CAPTURE`` buffer.
          (必须为每帧设置一个唯一值。该值会传播到解码帧的缓冲区中，也可以用来把这一帧作为另一帧的参考。如果每帧使用多次解码请求，那么给定帧的所有 ``OUTPUT`` 缓冲区的时间戳必须相同。如果时间戳发生变化，那么当前持有的 ``CAPTURE`` 缓冲区将被开放给出队，而当前请求将作用于新的 ``CAPTURE`` 缓冲区。)

2. Set the codec-specific controls for the decoding request, using
   :c:func:`VIDIOC_S_EXT_CTRLS`.
   (2. 使用 :c:func:`VIDIOC_S_EXT_CTRLS` 为解码请求设置编解码器特定的控制。)

    * **Required fields:**

      ``which``
          must be ``V4L2_CTRL_WHICH_REQUEST_VAL``.
          (必须是 ``V4L2_CTRL_WHICH_REQUEST_VAL`` 。)

      ``request_fd``
          must be set to the file descriptor of the decoding request.
          (必须设置为解码请求的文件描述符。)

      other fields
          other fields are set as usual when setting controls. The ``controls``
          array must contain all the codec-specific controls required to decode
          a frame.
          (其他字段按设置控制时的通常方式设置。 ``controls`` 数组必须包含解码一帧所需的全部编解码器特定控制。)

   .. note::

      It is possible to specify the controls in different invocations of
      :c:func:`VIDIOC_S_EXT_CTRLS`, or to overwrite a previously set control, as
      long as ``request_fd`` and ``which`` are properly set. The controls state
      at the moment of request submission is the one that will be considered.
      (可以在多次 :c:func:`VIDIOC_S_EXT_CTRLS` 调用中指定这些控制，也可以覆盖先前设置的控制，只要正确设置了 ``request_fd`` 与 ``which`` 即可。以提交 request 那一刻的控制状态为准。)

   .. note::

      The order in which steps 1 and 2 take place is interchangeable.
      (步骤 1 与步骤 2 的先后顺序可以互换。)

3. Submit the request by invoking :c:func:`MEDIA_REQUEST_IOC_QUEUE` on the
   request FD.
   (3. 通过在 request FD 上调用 :c:func:`MEDIA_REQUEST_IOC_QUEUE` 提交该 request。)

    If the request is submitted without an ``OUTPUT`` buffer, or if some of the
    required controls are missing from the request, then
    :c:func:`MEDIA_REQUEST_IOC_QUEUE` will return ``-ENOENT``. If more than one
    ``OUTPUT`` buffer is queued, then it will return ``-EINVAL``.
    :c:func:`MEDIA_REQUEST_IOC_QUEUE` returning non-zero means that no
    ``CAPTURE`` buffer will be produced for this request.
    (如果提交的 request 没有附加 ``OUTPUT`` 缓冲区，或者 request 缺少某些必需的控制，则 :c:func:`MEDIA_REQUEST_IOC_QUEUE` 会返回 ``-ENOENT`` 。如果入队了多于一个 ``OUTPUT`` 缓冲区，则返回 ``-EINVAL`` 。 :c:func:`MEDIA_REQUEST_IOC_QUEUE` 返回非零值意味着不会为该 request 产生 ``CAPTURE`` 缓冲区。)

``CAPTURE`` buffers must not be part of the request, and are queued
independently. They are returned in decode order (i.e. the same order as coded
frames were submitted to the ``OUTPUT`` queue).
( ``CAPTURE`` 缓冲区不得属于 request，而是独立入队。它们按解码顺序返回(即与编码帧提交到 ``OUTPUT`` 队列相同的顺序)。)

Runtime decoding errors are signaled by the dequeued ``CAPTURE`` buffers
carrying the ``V4L2_BUF_FLAG_ERROR`` flag. If a decoded reference frame has an
error, then all following decoded frames that refer to it also have the
``V4L2_BUF_FLAG_ERROR`` flag set, although the decoder will still try to
produce (likely corrupted) frames.
(运行时解码错误通过出队的 ``CAPTURE`` 缓冲区携带 ``V4L2_BUF_FLAG_ERROR`` 标志来指示。如果解码出的参考帧有错误，那么其后所有引用它的解码帧也都设置 ``V4L2_BUF_FLAG_ERROR`` 标志，尽管解码器仍会尝试产生(很可能已损坏的)帧。)

Buffer management while decoding(解码期间的缓冲区管理)
Contrary to stateful decoders, a stateless decoder does not perform any kind of
buffer management: it only guarantees that dequeued ``CAPTURE`` buffers can be
used by the client for as long as they are not queued again. "Used" here
encompasses using the buffer for compositing or display.
(与有状态解码器相反，无状态解码器不执行任何形式的缓冲区管理：它只保证出队的 ``CAPTURE`` 缓冲区在再次入队之前可被客户端使用。这里的 "使用" 包括把缓冲区用于合成或显示。)

A dequeued capture buffer can also be used as the reference frame of another
buffer.
(出队的捕获缓冲区还可以用作另一个缓冲区的参考帧。)

A frame is specified as reference by converting its timestamp into nanoseconds,
and storing it into the relevant member of a codec-dependent control structure.
The :c:func:`v4l2_timeval_to_ns` function must be used to perform that
conversion. The timestamp of a frame can be used to reference it as soon as all
its units of encoded data are successfully submitted to the ``OUTPUT`` queue.
(通过把某一帧的时间戳转换为纳秒，并存入某个编解码器相关控制结构的相关成员，即可把该帧指定为参考。必须使用 :c:func:`v4l2_timeval_to_ns` 函数执行该转换。只要某帧的所有编码数据单元都已成功提交到 ``OUTPUT`` 队列，其时间戳就可以用来引用它。)

A decoded buffer containing a reference frame must not be reused as a decoding
target until all the frames referencing it have been decoded. The safest way to
achieve this is to refrain from queueing a reference buffer until all the
decoded frames referencing it have been dequeued. However, if the driver can
guarantee that buffers queued to the ``CAPTURE`` queue are processed in queued
order, then user-space can take advantage of this guarantee and queue a
reference buffer when the following conditions are met:
(含有参考帧的已解码缓冲区，在所有引用它的帧都被解码之前，不得复用为解码目标。实现这一点的最稳妥办法是：在所有引用它的解码帧都出队之前，暂缓把该参考缓冲区重新入队。不过，如果驱动能够保证 ``CAPTURE`` 队列上入队的缓冲区按入队顺序处理，那么用户空间可以利用这一保证，在满足以下条件时就重新入队参考缓冲区：)

1. All the requests for frames affected by the reference frame have been
   queued, and
   (1. 受该参考帧影响的所有帧的请求都已入队，并且)

2. A sufficient number of ``CAPTURE`` buffers to cover all the decoded
   referencing frames have been queued.
   (2. 已入队数量足够的 ``CAPTURE`` 缓冲区，以覆盖所有解码中的引用帧。)

When queuing a decoding request, the driver will increase the reference count of
all the resources associated with reference frames. This means that the client
can e.g. close the DMABUF file descriptors of reference frame buffers if it
won't need them afterwards.
(入队一个解码请求时，驱动会递增与参考帧相关联的所有资源的引用计数。这意味着客户端可以例如关闭参考帧缓冲区的 DMABUF 文件描述符——如果之后不再需要它们的话。)

Seeking(跳转)
In order to seek, the client just needs to submit requests using input buffers
corresponding to the new stream position. It must however be aware that
resolution may have changed and follow the dynamic resolution change sequence in
that case. Also depending on the codec used, picture parameters (e.g. SPS/PPS
for H.264) may have changed and the client is responsible for making sure that a
valid state is sent to the decoder.
(要跳转，客户端只需使用对应于新码流位置的输入缓冲区提交请求即可。但它必须意识到分辨率可能已经改变，并在这种情况下遵循动态分辨率变更时序。此外，取决于所使用的编解码器，图像参数(如 H.264 的 SPS/PPS)可能已经改变，客户端负责确保向解码器发送有效的状态。)

The client is then free to ignore any returned ``CAPTURE`` buffer that comes
from the pre-seek position.
(此后，客户端可以随意忽略任何来自跳转前位置的 ``CAPTURE`` 缓冲区。)

Pausing(暂停)
=============

In order to pause, the client can just cease queuing buffers onto the ``OUTPUT``
queue. Without source bytestream data, there is no data to process and the codec
will remain idle.
(要暂停，客户端只需停止向 ``OUTPUT`` 队列入队缓冲区即可。没有源字节流数据，就没有数据可处理，编解码器将保持空闲。)

Dynamic resolution change(动态分辨率变更)
=========================================

If the client detects a resolution change in the stream, it will need to perform
the initialization sequence again with the new resolution:
(如果客户端检测到码流中分辨率发生变化，它需要以新的分辨率重新执行初始化时序：)

1. If the last submitted request resulted in a ``CAPTURE`` buffer being
   held by the use of the ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` flag, then the
   last frame is not available on the ``CAPTURE`` queue. In this case, a
   ``V4L2_DEC_CMD_FLUSH`` command shall be sent. This will make the driver
   dequeue the held ``CAPTURE`` buffer.
   (1. 如果最后提交的请求因使用 ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` 标志而持有一个 ``CAPTURE`` 缓冲区，那么最后一帧在 ``CAPTURE`` 队列上不可用。在这种情况下，应发送 ``V4L2_DEC_CMD_FLUSH`` 命令。这会使驱动把持有的 ``CAPTURE`` 缓冲区出队。)

2. Wait until all submitted requests have completed and dequeue the
   corresponding output buffers.
   (2. 等待所有已提交的请求完成，并把对应的输出缓冲区出队。)

3. Call :c:func:`VIDIOC_STREAMOFF` on both the ``OUTPUT`` and ``CAPTURE``
   queues.
   (3. 在 ``OUTPUT`` 与 ``CAPTURE`` 两个队列上调用 :c:func:`VIDIOC_STREAMOFF` 。)

4. Free all ``CAPTURE`` buffers by calling :c:func:`VIDIOC_REQBUFS` on the
   ``CAPTURE`` queue with a buffer count of zero.
   (4. 通过在 ``CAPTURE`` 队列上调用 :c:func:`VIDIOC_REQBUFS` 并把缓冲区数量设为 0，释放所有 ``CAPTURE`` 缓冲区。)

5. Perform the initialization sequence again (minus the allocation of
   ``OUTPUT`` buffers), with the new resolution set on the ``OUTPUT`` queue.
   Note that due to resolution constraints, a different format may need to be
   picked on the ``CAPTURE`` queue.
   (5. 重新执行初始化时序(减去 ``OUTPUT`` 缓冲区的分配)，并在 ``OUTPUT`` 队列上设置新的分辨率。注意，由于分辨率的约束， ``CAPTURE`` 队列上可能需要选择不同的格式。)

Drain(排空)
===========

If the last submitted request resulted in a ``CAPTURE`` buffer being
held by the use of the ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` flag, then the
last frame is not available on the ``CAPTURE`` queue. In this case, a
``V4L2_DEC_CMD_FLUSH`` command shall be sent. This will make the driver
dequeue the held ``CAPTURE`` buffer.
(如果最后提交的请求因使用 ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` 标志而持有一个 ``CAPTURE`` 缓冲区，那么最后一帧在 ``CAPTURE`` 队列上不可用。在这种情况下，应发送 ``V4L2_DEC_CMD_FLUSH`` 命令。这会使驱动把持有的 ``CAPTURE`` 缓冲区出队。)

After that, in order to drain the stream on a stateless decoder, the client
just needs to wait until all the submitted requests are completed.
(此后，要在无状态解码器上排空码流，客户端只需等待所有已提交的请求完成即可。)
