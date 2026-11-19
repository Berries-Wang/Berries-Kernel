.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _buffer:

***************
Buffers(缓冲区)
***************

A buffer contains data exchanged by application and driver using one of
the Streaming I/O methods. In the multi-planar API, the data is held in
planes, while the buffer structure acts as a container for the planes.
Only pointers to buffers (planes) are exchanged, the data itself is not
copied. These pointers, together with meta-information like timestamps
or field parity, are stored in a struct :c:type:`v4l2_buffer`,
argument to the :ref:`VIDIOC_QUERYBUF`,
:ref:`VIDIOC_QBUF <VIDIOC_QBUF>` and
:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl. In the multi-planar API,
some plane-specific members of struct :c:type:`v4l2_buffer`,
such as pointers and sizes for each plane, are stored in
struct :c:type:`v4l2_plane` instead. In that case,
struct :c:type:`v4l2_buffer` contains an array of plane structures.
(缓冲区包含应用与驱动通过某种流式 I/O 方法交换的数据。在多平面 (multi-planar) API 中，数据保存在各个平面 (plane) 中，而缓冲区结构体充当这些平面的容器。交换的只是指向缓冲区 (平面) 的指针，数据本身不会被复制。这些指针连同时间戳、场奇偶性等元信息一起，存储在 v4l2_buffer 结构体中，它是 VIDIOC_QUERYBUF、VIDIOC_QBUF 和 VIDIOC_DQBUF ioctl 的参数。在多平面 API 中，v4l2_buffer 结构体中一些特定于平面的成员 (如每个平面的指针和大小) 改为存储在 v4l2_plane 结构体中，此时 v4l2_buffer 结构体包含一个平面结构体数组。)

Dequeued video buffers come with timestamps. The driver decides at which
part of the frame and with which clock the timestamp is taken. Please
see flags in the masks ``V4L2_BUF_FLAG_TIMESTAMP_MASK`` and
``V4L2_BUF_FLAG_TSTAMP_SRC_MASK`` in :ref:`buffer-flags`. These flags
are always valid and constant across all buffers during the whole video
stream. Changes in these flags may take place as a side effect of
:ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>` or
:ref:`VIDIOC_S_OUTPUT <VIDIOC_G_OUTPUT>` however. The
``V4L2_BUF_FLAG_TIMESTAMP_COPY`` timestamp type which is used by e.g. on
mem-to-mem devices is an exception to the rule: the timestamp source
flags are copied from the OUTPUT video buffer to the CAPTURE video
buffer.
(出队的视频缓冲区带有时间戳。驱动决定在帧的哪个部分、用哪种时钟来取时间戳。请参阅 :ref:`buffer-flags` 一节中 ``V4L2_BUF_FLAG_TIMESTAMP_MASK`` 和 ``V4L2_BUF_FLAG_TSTAMP_SRC_MASK`` 掩码里的标志。在整个视频流期间，这些标志在所有缓冲区上都始终有效且恒定。不过，这些标志的变化可能作为 :ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>` 或 :ref:`VIDIOC_S_OUTPUT <VIDIOC_G_OUTPUT>` 的副作用而发生。例如 mem-to-mem 设备使用的 ``V4L2_BUF_FLAG_TIMESTAMP_COPY`` 时间戳类型是该规则的例外：时间戳来源标志会从 OUTPUT 视频缓冲区复制到 CAPTURE 视频缓冲区。)

Interactions between formats, controls and buffers(格式、控制与缓冲区之间的相互作用)
====================================================================================

V4L2 exposes parameters that influence the buffer size, or the way data is
laid out in the buffer. Those parameters are exposed through both formats and
controls. One example of such a control is the ``V4L2_CID_ROTATE`` control
that modifies the direction in which pixels are stored in the buffer, as well
as the buffer size when the selected format includes padding at the end of
lines.
(V4L2 会暴露一些影响缓冲区大小或数据在缓冲区中排布方式的参数。这些参数同时通过格式 (formats) 和控制 (controls) 暴露。这类控制的一个例子是 ``V4L2_CID_ROTATE`` 控制，它会修改像素在缓冲区中的存储方向；当所选格式在行尾包含填充 (padding) 时，还会影响缓冲区大小。)

The set of information needed to interpret the content of a buffer (e.g. the
pixel format, the line stride, the tiling orientation or the rotation) is
collectively referred to in the rest of this section as the buffer layout.
(解读缓冲区内容所需的一组信息 (如像素格式、行步长 (stride)、平铺方向或旋转) 在本节其余部分统称为缓冲区布局 (buffer layout)。)

Controls that can modify the buffer layout shall set the
``V4L2_CTRL_FLAG_MODIFY_LAYOUT`` flag.
(能够修改缓冲区布局的控制应设置 ``V4L2_CTRL_FLAG_MODIFY_LAYOUT`` 标志。)

Modifying formats or controls that influence the buffer size or layout require
the stream to be stopped. Any attempt at such a modification while the stream
is active shall cause the ioctl setting the format or the control to return
the ``EBUSY`` error code. In that case drivers shall also set the
``V4L2_CTRL_FLAG_GRABBED`` flag when calling
:c:func:`VIDIOC_QUERYCTRL` or :c:func:`VIDIOC_QUERY_EXT_CTRL` for such a
control while the stream is active.
(修改影响缓冲区大小或布局的格式或控制要求流处于停止状态。在流处于活动状态时尝试此类修改，设置格式或控制的 ioctl 应返回 ``EBUSY`` 错误码。此时，在流活动期间对这类控制调用 :c:func:`VIDIOC_QUERYCTRL` 或 :c:func:`VIDIOC_QUERY_EXT_CTRL` 时，驱动还应设置 ``V4L2_CTRL_FLAG_GRABBED`` 标志。)

.. note::

   The :c:func:`VIDIOC_S_SELECTION` ioctl can, depending on the hardware (for
   instance if the device doesn't include a scaler), modify the format in
   addition to the selection rectangle. Similarly, the
   :c:func:`VIDIOC_S_INPUT`, :c:func:`VIDIOC_S_OUTPUT`, :c:func:`VIDIOC_S_STD`
   and :c:func:`VIDIOC_S_DV_TIMINGS` ioctls can also modify the format and
   selection rectangles. When those ioctls result in a buffer size or layout
   change, drivers shall handle that condition as they would handle it in the
   :c:func:`VIDIOC_S_FMT` ioctl in all cases described in this section.
   (:c:func:`VIDIOC_S_SELECTION` ioctl 依据硬件情况 (例如设备不包含缩放器时) 除选择矩形外还可能修改格式。类似地， :c:func:`VIDIOC_S_INPUT` 、 :c:func:`VIDIOC_S_OUTPUT` 、 :c:func:`VIDIOC_S_STD` 和 :c:func:`VIDIOC_S_DV_TIMINGS` 这些 ioctl 也可能修改格式和选择矩形。当这些 ioctl 导致缓冲区大小或布局变化时，在本节描述的所有情形中，驱动都应像处理 :c:func:`VIDIOC_S_FMT` ioctl 那样处理该情况。)

Controls that only influence the buffer layout can be modified at any time
when the stream is stopped. As they don't influence the buffer size, no
special handling is needed to synchronize those controls with buffer
allocation and the ``V4L2_CTRL_FLAG_GRABBED`` flag is cleared once the
stream is stopped.
(只影响缓冲区布局的控制可以在流停止后的任何时刻修改。由于它们不影响缓冲区大小，无需特殊处理来使这些控制与缓冲区分配同步，而且一旦流停止， ``V4L2_CTRL_FLAG_GRABBED`` 标志即被清除。)

Formats and controls that influence the buffer size interact with buffer
allocation. The simplest way to handle this is for drivers to always require
buffers to be reallocated in order to change those formats or controls. In
that case, to perform such changes, userspace applications shall first stop
the video stream with the :c:func:`VIDIOC_STREAMOFF` ioctl if it is running
and free all buffers with the :c:func:`VIDIOC_REQBUFS` ioctl if they are
allocated. After freeing all buffers the ``V4L2_CTRL_FLAG_GRABBED`` flag
for controls is cleared. The format or controls can then be modified, and
buffers shall then be reallocated and the stream restarted. A typical ioctl
sequence is
(影响缓冲区大小的格式和控制与缓冲区分配相互影响。处理这个问题最简单的方式，是驱动总是要求重新分配缓冲区才能更改这些格式或控制。在这种情况下，要执行此类更改，用户空间应用应先在视频流运行时用 :c:func:`VIDIOC_STREAMOFF` ioctl 停止视频流，并在缓冲区已分配时用 :c:func:`VIDIOC_REQBUFS` ioctl 释放所有缓冲区。释放全部缓冲区后，控制的 ``V4L2_CTRL_FLAG_GRABBED`` 标志即被清除。然后可以修改格式或控制，接着重新分配缓冲区并重启流。典型的 ioctl 序列是)

 #. VIDIOC_STREAMOFF
 #. VIDIOC_REQBUFS(0)
 #. VIDIOC_S_EXT_CTRLS
 #. VIDIOC_S_FMT
 #. VIDIOC_REQBUFS(n)
 #. VIDIOC_QBUF
 #. VIDIOC_STREAMON

The second :c:func:`VIDIOC_REQBUFS` call will take the new format and control
value into account to compute the buffer size to allocate. Applications can
also retrieve the size by calling the :c:func:`VIDIOC_G_FMT` ioctl if needed.
(第二次 :c:func:`VIDIOC_REQBUFS` 调用会把新的格式和控制值考虑在内来计算要分配的缓冲区大小。如有需要，应用也可以通过调用 :c:func:`VIDIOC_G_FMT` ioctl 获取该大小。)

.. note::

   The API doesn't mandate the above order for control (3.) and format (4.)
   changes. Format and controls can be set in a different order, or even
   interleaved, depending on the device and use case. For instance some
   controls might behave differently for different pixel formats, in which
   case the format might need to be set first.
   (API 并不强制要求上面控制 (第 3 步) 与格式 (第 4 步) 更改的先后顺序。格式和控制可以按不同顺序设置，甚至交错设置，具体取决于设备和使用场景。例如某些控制对不同像素格式的行为可能不同，这时可能需要先设置格式。)

When reallocation is required, any attempt to modify format or controls that
influences the buffer size while buffers are allocated shall cause the format
or control set ioctl to return the ``EBUSY`` error. Any attempt to queue a
buffer too small for the current format or controls shall cause the
:c:func:`VIDIOC_QBUF` ioctl to return a ``EINVAL`` error.
(在需要重新分配缓冲区时，若缓冲区已分配，任何对影响缓冲区大小的格式或控制的修改尝试都应使相应的设置 ioctl 返回 ``EBUSY`` 错误。任何把对当前格式或控制而言过小的缓冲区入队的尝试，都应使 :c:func:`VIDIOC_QBUF` ioctl 返回 ``EINVAL`` 错误。)

Buffer reallocation is an expensive operation. To avoid that cost, drivers can
(and are encouraged to) allow format or controls that influence the buffer
size to be changed with buffers allocated. In that case, a typical ioctl
sequence to modify format and controls is
(缓冲区重新分配是代价高昂的操作。为避免这一开销，驱动可以 (也鼓励) 允许在缓冲区已分配的情况下更改影响缓冲区大小的格式或控制。此时，修改格式和控制的典型 ioctl 序列是)

 #. VIDIOC_STREAMOFF
 #. VIDIOC_S_EXT_CTRLS
 #. VIDIOC_S_FMT
 #. VIDIOC_QBUF
 #. VIDIOC_STREAMON

For this sequence to operate correctly, queued buffers need to be large enough
for the new format or controls. Drivers shall return a ``ENOSPC`` error in
response to format change (:c:func:`VIDIOC_S_FMT`) or control changes
(:c:func:`VIDIOC_S_CTRL` or :c:func:`VIDIOC_S_EXT_CTRLS`) if buffers too small
for the new format are currently queued. As a simplification, drivers are
allowed to return a ``EBUSY`` error from these ioctls if any buffer is
currently queued, without checking the queued buffers sizes.
(要使该序列正确运作，已入队的缓冲区必须足以容纳新格式或控制。如果当前已入队的缓冲区对新格式来说太小，驱动应在格式更改 (:c:func:`VIDIOC_S_FMT`) 或控制更改 (:c:func:`VIDIOC_S_CTRL` 或 :c:func:`VIDIOC_S_EXT_CTRLS`) 时返回 ``ENOSPC`` 错误。作为一种简化，只要有任何缓冲区已入队，驱动也允许这些 ioctl 直接返回 ``EBUSY`` 错误，而不检查已入队缓冲区的大小。)

Additionally, drivers shall return a ``EINVAL`` error from the
:c:func:`VIDIOC_QBUF` ioctl if the buffer being queued is too small for the
current format or controls. Together, these requirements ensure that queued
buffers will always be large enough for the configured format and controls.
(此外，如果要入队的缓冲区对当前格式或控制来说太小，驱动应让 :c:func:`VIDIOC_QBUF` ioctl 返回 ``EINVAL`` 错误。这些要求合在一起，确保已入队的缓冲区对于所配置的格式和控制总是足够大。)

Userspace applications can query the buffer size required for a given format
and controls by first setting the desired control values and then trying the
desired format. The :c:func:`VIDIOC_TRY_FMT` ioctl will return the required
buffer size.
(用户空间应用可以先设置所需的控制值、再尝试所需格式，来查询给定格式和控制所需的缓冲区大小。:c:func:`VIDIOC_TRY_FMT` ioctl 会返回所需的缓冲区大小。)

 #. VIDIOC_S_EXT_CTRLS(x)
 #. VIDIOC_TRY_FMT()
 #. VIDIOC_S_EXT_CTRLS(y)
 #. VIDIOC_TRY_FMT()

The :c:func:`VIDIOC_CREATE_BUFS` ioctl can then be used to allocate buffers
based on the queried sizes (for instance by allocating a set of buffers large
enough for all the desired formats and controls, or by allocating separate set
of appropriately sized buffers for each use case).
(随后可以基于查询到的大小用 :c:func:`VIDIOC_CREATE_BUFS` ioctl 分配缓冲区 (例如分配一组足以容纳所有所需格式和控制的缓冲区，或者为每个用例分别分配一组大小合适的缓冲区)。)

.. c:type:: v4l2_buffer

struct v4l2_buffer(v4l2_buffer 结构体)
=======================================

.. tabularcolumns:: |p{2.9cm}|p{2.4cm}|p{12.0cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_buffer
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 2 10

    * - __u32
      - ``index``
      - Number of the buffer, set by the application except when calling
	:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>`, then it is set by the
	driver. This field can range from zero to the number of buffers
	allocated with the :ref:`VIDIOC_REQBUFS` ioctl
	(struct :c:type:`v4l2_requestbuffers`
	``count``), plus any buffers allocated with
	:ref:`VIDIOC_CREATE_BUFS` minus one.
        (缓冲区编号，由应用设置；但调用 VIDIOC_DQBUF 时除外，此时由驱动设置。
        该字段的取值范围从零到用 VIDIOC_REQBUFS ioctl 分配的缓冲区数量
        (v4l2_requestbuffers 结构体的 count 字段)，加上用 VIDIOC_CREATE_BUFS
        分配的缓冲区数量，再减一。)
    * - __u32
      - ``type``
      - Type of the buffer, same as struct
	:c:type:`v4l2_format` ``type`` or struct
	:c:type:`v4l2_requestbuffers` ``type``, set
	by the application. See :c:type:`v4l2_buf_type`
        (缓冲区类型，与应用在 v4l2_format 结构体的 type 或
        v4l2_requestbuffers 结构体的 type 中设置的值相同。参见
        v4l2_buf_type。)
    * - __u32
      - ``bytesused``
      - The number of bytes occupied by the data in the buffer. It depends
	on the negotiated data format and may change with each buffer for
	compressed variable size data like JPEG images. Drivers must set
	this field when ``type`` refers to a capture stream, applications
	when it refers to an output stream. For multiplanar formats this field
        is ignored and the
	``planes`` pointer is used instead.
        (缓冲区中数据所占的字节数。它取决于协商好的数据格式，对于 JPEG
        图像这类大小可变的压缩数据，每个缓冲区可能不同。当 type 指向捕获流
        时驱动必须设置该字段，指向输出流时由应用设置。对于多平面格式，该
        字段被忽略，改用 planes 指针。)
    * - __u32
      - ``flags``
      - Flags set by the application or driver, see :ref:`buffer-flags`.
        (由应用或驱动设置的标志，参见 buffer-flags 一节。)
    * - __u32
      - ``field``
      - Indicates the field order of the image in the buffer, see
	:c:type:`v4l2_field`. This field is not used when the buffer
	contains VBI data. Drivers must set it when ``type`` refers to a
	capture stream, applications when it refers to an output stream.
        (指示缓冲区中图像的场序 (field order)，参见 v4l2_field。当缓冲区
        包含 VBI 数据时不使用该字段。当 type 指向捕获流时驱动必须设置它，
        指向输出流时由应用设置。)
    * - struct timeval
      - ``timestamp``
      - For capture streams this is time when the first data byte was
	captured, as returned by the :c:func:`clock_gettime()` function
	for the relevant clock id; see ``V4L2_BUF_FLAG_TIMESTAMP_*`` in
	:ref:`buffer-flags`. For output streams the driver stores the
	time at which the last data byte was actually sent out in the
	``timestamp`` field. This permits applications to monitor the
	drift between the video and system clock. For output streams that
	use ``V4L2_BUF_FLAG_TIMESTAMP_COPY`` the application has to fill
	in the timestamp which will be copied by the driver to the capture
	stream.
        (对于捕获流，这是第一个数据字节被捕获的时间，由针对相关时钟 id
        调用 clock_gettime() 函数返回；参见 buffer-flags 一节中的
        V4L2_BUF_FLAG_TIMESTAMP_* 标志。对于输出流，驱动在 timestamp
        字段中存储最后一个数据字节实际被发送出去的时间。这使应用能够
        监视视频时钟与系统时钟之间的漂移。对于使用
        V4L2_BUF_FLAG_TIMESTAMP_COPY 的输出流，应用必须填写时间戳，
        驱动会把它复制到捕获流。)
    * - struct :c:type:`v4l2_timecode`
      - ``timecode``
      - When the ``V4L2_BUF_FLAG_TIMECODE`` flag is set in ``flags``, this
	structure contains a frame timecode. In
	:c:type:`V4L2_FIELD_ALTERNATE <v4l2_field>` mode the top and
	bottom field contain the same timecode. Timecodes are intended to
	help video editing and are typically recorded on video tapes, but
	also embedded in compressed formats like MPEG. This field is
	independent of the ``timestamp`` and ``sequence`` fields.
        (当 flags 中设置了 V4L2_BUF_FLAG_TIMECODE 标志时，该结构体包含
        一个帧时间码。在 V4L2_FIELD_ALTERNATE 模式下，顶场和底场包含
        相同的时间码。时间码旨在辅助视频剪辑，通常记录在录像带上，但
        也嵌入在 MPEG 等压缩格式中。该字段独立于 timestamp 和 sequence
        字段。)
    * - __u32
      - ``sequence``
      - Set by the driver, counting the frames (not fields!) in sequence.
	This field is set for both input and output devices.
        (由驱动设置，按顺序对帧 (而不是场！) 计数。输入和输出设备都会
        设置该字段。)
    * - :cspan:`2`

	In :c:type:`V4L2_FIELD_ALTERNATE <v4l2_field>` mode the top and
	bottom field have the same sequence number. The count starts at
	zero and includes dropped or repeated frames. A dropped frame was
	received by an input device but could not be stored due to lack of
	free buffer space. A repeated frame was displayed again by an
	output device because the application did not pass new data in
	time.
        (在 V4L2_FIELD_ALTERNATE 模式下，顶场和底场具有相同的序号。计数
        从零开始，包括被丢弃或重复的帧。丢弃帧 (dropped frame) 是被输入
        设备接收到、但因缺少空闲缓冲空间而无法存储的帧。重复帧 (repeated
        frame) 是因应用未及时传入新数据而被输出设备再次显示的帧。)

	.. note::

	   This may count the frames received e.g. over USB, without
	   taking into account the frames dropped by the remote hardware due
	   to limited compression throughput or bus bandwidth. These devices
	   identify by not enumerating any video standards, see
	   :ref:`standard`.
	   (这里计数的可能是通过 USB 等途径接收到的帧，而未把远端硬件因压缩
	   吞吐或总线带宽有限而丢弃的帧计入。这类设备不枚举任何视频标准，
	   据此可以识别它们，参见 standard 一节。)

    * - __u32
      - ``memory``
      - This field must be set by applications and/or drivers in
	accordance with the selected I/O method. See :c:type:`v4l2_memory`
        (该字段必须由应用和/或驱动按照所选的 I/O 方法设置。参见
        v4l2_memory。)
    * - union {
      - ``m``
    * - __u32
      - ``offset``
      - For the single-planar API and when ``memory`` is
	``V4L2_MEMORY_MMAP`` this is the offset of the buffer from the
	start of the device memory. The value is returned by the driver
	and apart of serving as parameter to the
	:c:func:`mmap()` function not useful for applications.
	See :ref:`mmap` for details
        (对于单平面 API，当 memory 为 V4L2_MEMORY_MMAP 时，这是缓冲区相对
        于设备内存起始处的偏移。该值由驱动返回，除作为 mmap() 函数的参数
        外对应用没有别的用处。细节参见 mmap 一节。)
    * - unsigned long
      - ``userptr``
      - For the single-planar API and when ``memory`` is
	``V4L2_MEMORY_USERPTR`` this is a pointer to the buffer (casted to
	unsigned long type) in virtual memory, set by the application. See
	:ref:`userp` for details.
        (对于单平面 API，当 memory 为 V4L2_MEMORY_USERPTR 时，这是应用
        设置的、指向虚拟内存中缓冲区的指针 (强制转换为 unsigned long
        类型)。细节参见 userp 一节。)
    * - struct v4l2_plane
      - ``*planes``
      - When using the multi-planar API, contains a userspace pointer to
	an array of struct :c:type:`v4l2_plane`. The size of
	the array should be put in the ``length`` field of this
	struct :c:type:`v4l2_buffer` structure.
        (使用多平面 API 时，包含指向 v4l2_plane 结构体数组的用户空间
        指针。数组的大小应放入本 v4l2_buffer 结构体的 length 字段。)
    * - int
      - ``fd``
      - For the single-plane API and when ``memory`` is
	``V4L2_MEMORY_DMABUF`` this is the file descriptor associated with
	a DMABUF buffer.
        (对于单平面 API，当 memory 为 V4L2_MEMORY_DMABUF 时，这是与
        DMABUF 缓冲区关联的文件描述符。)
    * - }
      -
    * - __u32
      - ``length``
      - Size of the buffer (not the payload) in bytes for the
	single-planar API. This is set by the driver based on the calls to
	:ref:`VIDIOC_REQBUFS` and/or
	:ref:`VIDIOC_CREATE_BUFS`. For the
	multi-planar API the application sets this to the number of
	elements in the ``planes`` array. The driver will fill in the
	actual number of valid elements in that array.
        (对于单平面 API，这是缓冲区的大小 (而不是有效载荷)，以字节为
        单位。由驱动根据 VIDIOC_REQBUFS 和/或 VIDIOC_CREATE_BUFS 的调用
        来设置。对于多平面 API，应用把它设置为 planes 数组中的元素个数，
        驱动会填入该数组中实际有效元素的个数。)
    * - __u32
      - ``reserved2``
      - A place holder for future extensions. Drivers and applications
	must set this to 0.
        (为未来扩展保留的占位字段。驱动和应用必须把它设置为 0。)
    * - __u32
      - ``request_fd``
      - The file descriptor of the request to queue the buffer to. If the flag
        ``V4L2_BUF_FLAG_REQUEST_FD`` is set, then the buffer will be
	queued to this request. If the flag is not set, then this field will
	be ignored.
        (要把缓冲区入队到的请求的文件描述符。如果设置了
        V4L2_BUF_FLAG_REQUEST_FD 标志，缓冲区将入队到该请求；如果未设置
        该标志，则忽略此字段。)

	The ``V4L2_BUF_FLAG_REQUEST_FD`` flag and this field are only used by
	:ref:`ioctl VIDIOC_QBUF <VIDIOC_QBUF>` and ignored by other ioctls that
	take a :c:type:`v4l2_buffer` as argument.
        (V4L2_BUF_FLAG_REQUEST_FD 标志和此字段仅被 VIDIOC_QBUF 使用，其他
        以 v4l2_buffer 结构体为参数的 ioctl 都会忽略它们。)

	Applications should not set ``V4L2_BUF_FLAG_REQUEST_FD`` for any ioctls
	other than :ref:`VIDIOC_QBUF <VIDIOC_QBUF>`.
        (应用不应在 VIDIOC_QBUF 之外的任何 ioctl 上设置
        V4L2_BUF_FLAG_REQUEST_FD。)

	If the device does not support requests, then ``EBADR`` will be returned.
	If requests are supported but an invalid request file descriptor is
	given, then ``EINVAL`` will be returned.
        (如果设备不支持 requests，将返回 EBADR；如果支持 requests 但给出
        了无效的请求文件描述符，将返回 EINVAL。)


.. c:type:: v4l2_plane

struct v4l2_plane(v4l2_plane 结构体)
=====================================

.. tabularcolumns:: |p{3.5cm}|p{3.5cm}|p{10.3cm}|

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``bytesused``
      - The number of bytes occupied by data in the plane (its payload).
	Drivers must set this field when ``type`` refers to a capture
	stream, applications when it refers to an output stream.
        (平面中数据所占的字节数 (其有效载荷)。当 type 指向捕获流时驱动
        必须设置该字段，指向输出流时由应用设置。)

	.. note::

	   Note that the actual image data starts at ``data_offset``
	   which may not be 0.
	   (注意，实际图像数据从 data_offset 处开始，而 data_offset 可能
	   并不为 0。)
    * - __u32
      - ``length``
      - Size in bytes of the plane (not its payload). This is set by the
	driver based on the calls to
	:ref:`VIDIOC_REQBUFS` and/or
	:ref:`VIDIOC_CREATE_BUFS`.
        (平面的大小 (而不是其有效载荷)，以字节为单位。由驱动根据
        VIDIOC_REQBUFS 和/或 VIDIOC_CREATE_BUFS 的调用来设置。)
    * - union {
      - ``m``
    * - __u32
      - ``mem_offset``
      - When the memory type in the containing struct
	:c:type:`v4l2_buffer` is ``V4L2_MEMORY_MMAP``, this
	is the value that should be passed to :c:func:`mmap()`,
	similar to the ``offset`` field in struct
	:c:type:`v4l2_buffer`.
        (当所在 v4l2_buffer 结构体中的内存类型为 V4L2_MEMORY_MMAP 时，
        这就是应传给 mmap() 的值，类似于 v4l2_buffer 结构体中的 offset
        字段。)
    * - unsigned long
      - ``userptr``
      - When the memory type in the containing struct
	:c:type:`v4l2_buffer` is ``V4L2_MEMORY_USERPTR``,
	this is a userspace pointer to the memory allocated for this plane
	by an application.
        (当所在 v4l2_buffer 结构体中的内存类型为 V4L2_MEMORY_USERPTR 时，
        这是应用为该平面分配的内存的用户空间指针。)
    * - int
      - ``fd``
      - When the memory type in the containing struct
	:c:type:`v4l2_buffer` is ``V4L2_MEMORY_DMABUF``,
	this is a file descriptor associated with a DMABUF buffer, similar
	to the ``fd`` field in struct :c:type:`v4l2_buffer`.
        (当所在 v4l2_buffer 结构体中的内存类型为 V4L2_MEMORY_DMABUF 时，
        这是与 DMABUF 缓冲区关联的文件描述符，类似于 v4l2_buffer 结构体
        中的 fd 字段。)
    * - }
      -
    * - __u32
      - ``data_offset``
      - Offset in bytes to video data in the plane. Drivers must set this
	field when ``type`` refers to a capture stream, applications when
	it refers to an output stream.
        (平面内视频数据的字节偏移。当 type 指向捕获流时驱动必须设置该
        字段，指向输出流时由应用设置。)

	.. note::

	   That data_offset is included  in ``bytesused``. So the
	   size of the image in the plane is ``bytesused``-``data_offset``
	   at offset ``data_offset`` from the start of the plane.
	   (data_offset 包含在 bytesused 之内。因此，平面中图像的大小为
	   bytesused 减去 data_offset，位于距平面起始处 data_offset 的
	   位置。)
    * - __u32
      - ``reserved[11]``
      - Reserved for future use. Should be zeroed by drivers and
	applications.
        (保留供将来使用。驱动和应用应将其清零。)


.. c:type:: v4l2_buf_type

enum v4l2_buf_type(v4l2_buf_type 枚举)
=======================================

.. cssclass:: longtable

.. tabularcolumns:: |p{7.8cm}|p{0.6cm}|p{8.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       4 1 9

    * - ``V4L2_BUF_TYPE_VIDEO_CAPTURE``
      - 1
      - Buffer of a single-planar video capture stream, see
	:ref:`capture`.
        (单平面视频捕获流的缓冲区，参见 capture 一节。)
    * - ``V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE``
      - 9
      - Buffer of a multi-planar video capture stream, see
	:ref:`capture`.
        (多平面视频捕获流的缓冲区，参见 capture 一节。)
    * - ``V4L2_BUF_TYPE_VIDEO_OUTPUT``
      - 2
      - Buffer of a single-planar video output stream, see
	:ref:`output`.
        (单平面视频输出流的缓冲区，参见 output 一节。)
    * - ``V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE``
      - 10
      - Buffer of a multi-planar video output stream, see :ref:`output`.
        (多平面视频输出流的缓冲区，参见 output 一节。)
    * - ``V4L2_BUF_TYPE_VIDEO_OVERLAY``
      - 3
      - Buffer for video overlay, see :ref:`overlay`.
        (用于视频叠加 (overlay) 的缓冲区，参见 overlay 一节。)
    * - ``V4L2_BUF_TYPE_VBI_CAPTURE``
      - 4
      - Buffer of a raw VBI capture stream, see :ref:`raw-vbi`.
        (原始 VBI 捕获流的缓冲区，参见 raw-vbi 一节。)
    * - ``V4L2_BUF_TYPE_VBI_OUTPUT``
      - 5
      - Buffer of a raw VBI output stream, see :ref:`raw-vbi`.
        (原始 VBI 输出流的缓冲区，参见 raw-vbi 一节。)
    * - ``V4L2_BUF_TYPE_SLICED_VBI_CAPTURE``
      - 6
      - Buffer of a sliced VBI capture stream, see :ref:`sliced`.
        (切片 (sliced) VBI 捕获流的缓冲区，参见 sliced 一节。)
    * - ``V4L2_BUF_TYPE_SLICED_VBI_OUTPUT``
      - 7
      - Buffer of a sliced VBI output stream, see :ref:`sliced`.
        (切片 VBI 输出流的缓冲区，参见 sliced 一节。)
    * - ``V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY``
      - 8
      - Buffer for video output overlay (OSD), see :ref:`osd`.
        (用于视频输出叠加 (OSD) 的缓冲区，参见 osd 一节。)
    * - ``V4L2_BUF_TYPE_SDR_CAPTURE``
      - 11
      - Buffer for Software Defined Radio (SDR) capture stream, see
	:ref:`sdr`.
        (软件定义无线电 (SDR) 捕获流的缓冲区，参见 sdr 一节。)
    * - ``V4L2_BUF_TYPE_SDR_OUTPUT``
      - 12
      - Buffer for Software Defined Radio (SDR) output stream, see
	:ref:`sdr`.
        (软件定义无线电 (SDR) 输出流的缓冲区，参见 sdr 一节。)
    * - ``V4L2_BUF_TYPE_META_CAPTURE``
      - 13
      - Buffer for metadata capture, see :ref:`metadata`.
        (元数据捕获缓冲区，参见 metadata 一节。)
    * - ``V4L2_BUF_TYPE_META_OUTPUT``
      - 14
      - Buffer for metadata output, see :ref:`metadata`.
        (元数据输出缓冲区，参见 metadata 一节。)


.. _buffer-flags:

Buffer Flags(缓冲区标志)
=========================

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{6.5cm}|p{1.8cm}|p{9.0cm}|

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       65 18 70

    * .. _`V4L2-BUF-FLAG-MAPPED`:

      - ``V4L2_BUF_FLAG_MAPPED``
      - 0x00000001
      - The buffer resides in device memory and has been mapped into the
	application's address space, see :ref:`mmap` for details.
	Drivers set or clear this flag when the
	:ref:`VIDIOC_QUERYBUF`,
	:ref:`VIDIOC_QBUF` or
	:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl is called. Set by the
	driver.
        (缓冲区位于设备内存中，并已被映射到应用的地址空间，细节参见
        mmap 一节。当 VIDIOC_QUERYBUF、VIDIOC_QBUF 或 VIDIOC_DQBUF ioctl
        被调用时，驱动设置或清除该标志。由驱动设置。)
    * .. _`V4L2-BUF-FLAG-QUEUED`:

      - ``V4L2_BUF_FLAG_QUEUED``
      - 0x00000002
      - Internally drivers maintain two buffer queues, an incoming and
	outgoing queue. When this flag is set, the buffer is currently on
	the incoming queue. It automatically moves to the outgoing queue
	after the buffer has been filled (capture devices) or displayed
	(output devices). Drivers set or clear this flag when the
	``VIDIOC_QUERYBUF`` ioctl is called. After (successful) calling
	the ``VIDIOC_QBUF``\ ioctl it is always set and after
	``VIDIOC_DQBUF`` always cleared.
        (驱动内部维护两个缓冲区队列：传入队列和传出队列。设置此标志时，
        缓冲区当前位于传入队列中。缓冲区被填充 (捕获设备) 或显示 (输出
        设备) 之后，它会自动移到传出队列。当 VIDIOC_QUERYBUF ioctl 被调用
        时，驱动设置或清除该标志。(成功) 调用 VIDIOC_QBUF ioctl 后它总是
        被置位，而 VIDIOC_DQBUF 之后总是被清除。)
    * .. _`V4L2-BUF-FLAG-DONE`:

      - ``V4L2_BUF_FLAG_DONE``
      - 0x00000004
      - When this flag is set, the buffer is currently on the outgoing
	queue, ready to be dequeued from the driver. Drivers set or clear
	this flag when the ``VIDIOC_QUERYBUF`` ioctl is called. After
	calling the ``VIDIOC_QBUF`` or ``VIDIOC_DQBUF`` it is always
	cleared. Of course a buffer cannot be on both queues at the same
	time, the ``V4L2_BUF_FLAG_QUEUED`` and ``V4L2_BUF_FLAG_DONE`` flag
	are mutually exclusive. They can be both cleared however, then the
	buffer is in "dequeued" state, in the application domain so to
	say.
        (设置此标志时，缓冲区当前位于传出队列中，随时可以从驱动出队。
        当 VIDIOC_QUERYBUF ioctl 被调用时，驱动设置或清除该标志。调用
        VIDIOC_QBUF 或 VIDIOC_DQBUF 之后它总是被清除。当然，一个缓冲区
        不能同时位于两个队列中，V4L2_BUF_FLAG_QUEUED 与
        V4L2_BUF_FLAG_DONE 这两个标志互斥。不过两者也可以同时被清除，
        此时缓冲区处于 "已出队" 状态，可以说位于应用一方。)
    * .. _`V4L2-BUF-FLAG-ERROR`:

      - ``V4L2_BUF_FLAG_ERROR``
      - 0x00000040
      - When this flag is set, the buffer has been dequeued successfully,
	although the data might have been corrupted. This is recoverable,
	streaming may continue as normal and the buffer may be reused
	normally. Drivers set this flag when the ``VIDIOC_DQBUF`` ioctl is
	called.
        (设置此标志时，缓冲区已成功出队，尽管数据可能已经损坏。这是可
        恢复的，流可以照常继续，缓冲区也可以正常复用。当 VIDIOC_DQBUF
        ioctl 被调用时，驱动设置此标志。)
    * .. _`V4L2-BUF-FLAG-IN-REQUEST`:

      - ``V4L2_BUF_FLAG_IN_REQUEST``
      - 0x00000080
      - This buffer is part of a request that hasn't been queued yet.
        (该缓冲区属于一个尚未入队的请求。)
    * .. _`V4L2-BUF-FLAG-KEYFRAME`:

      - ``V4L2_BUF_FLAG_KEYFRAME``
      - 0x00000008
      - Drivers set or clear this flag when calling the ``VIDIOC_DQBUF``
	ioctl. It may be set by video capture devices when the buffer
	contains a compressed image which is a key frame (or field), i. e.
	can be decompressed on its own. Also known as an I-frame.
	Applications can set this bit when ``type`` refers to an output
	stream.
        (调用 VIDIOC_DQBUF ioctl 时，驱动设置或清除此标志。当缓冲区包含
        一幅本身即可解压的压缩图像 (关键帧或关键场) 时，视频捕获设备可能
        设置它，也称为 I 帧。当 type 指向输出流时，应用可以设置此位。)
    * .. _`V4L2-BUF-FLAG-PFRAME`:

      - ``V4L2_BUF_FLAG_PFRAME``
      - 0x00000010
      - Similar to ``V4L2_BUF_FLAG_KEYFRAME`` this flags predicted frames
	or fields which contain only differences to a previous key frame.
	Applications can set this bit when ``type`` refers to an output
	stream.
        (与 V4L2_BUF_FLAG_KEYFRAME 类似，此标志标记预测帧或场，它们只包含
        相对前一个关键帧的差异。当 type 指向输出流时，应用可以设置此位。)
    * .. _`V4L2-BUF-FLAG-BFRAME`:

      - ``V4L2_BUF_FLAG_BFRAME``
      - 0x00000020
      - Similar to ``V4L2_BUF_FLAG_KEYFRAME`` this flags a bi-directional
	predicted frame or field which contains only the differences
	between the current frame and both the preceding and following key
	frames to specify its content. Applications can set this bit when
	``type`` refers to an output stream.
        (与 V4L2_BUF_FLAG_KEYFRAME 类似，此标志标记双向预测帧或场，其内容
        只包含当前帧与其前向、后向关键帧之间的差异。当 type 指向输出流
        时，应用可以设置此位。)
    * .. _`V4L2-BUF-FLAG-TIMECODE`:

      - ``V4L2_BUF_FLAG_TIMECODE``
      - 0x00000100
      - The ``timecode`` field is valid. Drivers set or clear this flag
	when the ``VIDIOC_DQBUF`` ioctl is called. Applications can set
	this bit and the corresponding ``timecode`` structure when
	``type`` refers to an output stream.
        (timecode 字段有效。当 VIDIOC_DQBUF ioctl 被调用时，驱动设置或
        清除此标志。当 type 指向输出流时，应用可以设置此位以及相应的
        timecode 结构体。)
    * .. _`V4L2-BUF-FLAG-PREPARED`:

      - ``V4L2_BUF_FLAG_PREPARED``
      - 0x00000400
      - The buffer has been prepared for I/O and can be queued by the
	application. Drivers set or clear this flag when the
	:ref:`VIDIOC_QUERYBUF <VIDIOC_QUERYBUF>`,
	:ref:`VIDIOC_PREPARE_BUF <VIDIOC_QBUF>`,
	:ref:`VIDIOC_QBUF <VIDIOC_QBUF>` or
	:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl is called.
        (缓冲区已为 I/O 准备就绪，可以由应用入队。当 VIDIOC_QUERYBUF、
        VIDIOC_PREPARE_BUF、VIDIOC_QBUF 或 VIDIOC_DQBUF ioctl 被调用时，
        驱动设置或清除此标志。)
    * .. _`V4L2-BUF-FLAG-NO-CACHE-INVALIDATE`:

      - ``V4L2_BUF_FLAG_NO_CACHE_INVALIDATE``
      - 0x00000800
      - Caches do not have to be invalidated for this buffer. Typically
	applications shall use this flag if the data captured in the
	buffer is not going to be touched by the CPU, instead the buffer
	will, probably, be passed on to a DMA-capable hardware unit for
	further processing or output. This flag is ignored unless the
	queue is used for :ref:`memory mapping <mmap>` streaming I/O and
	reports :ref:`V4L2_BUF_CAP_SUPPORTS_MMAP_CACHE_HINTS
	<V4L2-BUF-CAP-SUPPORTS-MMAP-CACHE-HINTS>` capability.
        (对此缓冲区无需使缓存失效。通常，如果缓冲区中捕获的数据不会被
        CPU 触碰、而是大概会被传递给具备 DMA 能力的硬件单元做进一步处理
        或输出，应用应使用此标志。除非该队列用于内存映射流式 I/O 且报告
        V4L2_BUF_CAP_SUPPORTS_MMAP_CACHE_HINTS 能力，否则此标志被忽略。)
    * .. _`V4L2-BUF-FLAG-NO-CACHE-CLEAN`:

      - ``V4L2_BUF_FLAG_NO_CACHE_CLEAN``
      - 0x00001000
      - Caches do not have to be cleaned for this buffer. Typically
	applications shall use this flag for output buffers if the data in
	this buffer has not been created by the CPU but by some
	DMA-capable unit, in which case caches have not been used. This flag
	is ignored unless the queue is used for :ref:`memory mapping <mmap>`
	streaming I/O and reports :ref:`V4L2_BUF_CAP_SUPPORTS_MMAP_CACHE_HINTS
	<V4L2-BUF-CAP-SUPPORTS-MMAP-CACHE-HINTS>` capability.
        (对此缓冲区无需清理缓存。通常，如果输出缓冲区中的数据不是由 CPU
        生成而是由某个具备 DMA 能力的单元生成的 (此时缓存并未被使用)，
        应用应使用此标志。除非该队列用于内存映射流式 I/O 且报告
        V4L2_BUF_CAP_SUPPORTS_MMAP_CACHE_HINTS 能力，否则此标志被忽略。)
    * .. _`V4L2-BUF-FLAG-M2M-HOLD-CAPTURE-BUF`:

      - ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF``
      - 0x00000200
      - Only valid if struct :c:type:`v4l2_requestbuffers` flag ``V4L2_BUF_CAP_SUPPORTS_M2M_HOLD_CAPTURE_BUF`` is
	set. It is typically used with stateless decoders where multiple
	output buffers each decode to a slice of the decoded frame.
	Applications can set this flag when queueing the output buffer
	to prevent the driver from dequeueing the capture buffer after
	the output buffer has been decoded (i.e. the capture buffer is
	'held'). If the timestamp of this output buffer differs from that
	of the previous output buffer, then that indicates the start of a
	new frame and the previously held capture buffer is dequeued.
        (仅在设置了 v4l2_requestbuffers 结构体的
        V4L2_BUF_CAP_SUPPORTS_M2M_HOLD_CAPTURE_BUF 标志时有效。它通常与
        无状态 (stateless) 解码器一起使用：多个输出缓冲区各自解码到解码
        帧的一个条带 (slice)。应用可以在入队输出缓冲区时设置此标志，阻止
        驱动在输出缓冲区解码完成后立刻出队捕获缓冲区 (即 "持有" 捕获缓冲
        区)。如果该输出缓冲区的时间戳与前一个输出缓冲区不同，则表示新帧
        开始，先前被持有的捕获缓冲区将被出队。)
    * .. _`V4L2-BUF-FLAG-LAST`:

      - ``V4L2_BUF_FLAG_LAST``
      - 0x00100000
      - Last buffer produced by the hardware. mem2mem codec drivers set
	this flag on the capture queue for the last buffer when the
	:ref:`VIDIOC_QUERYBUF` or
	:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl is called. Due to
	hardware limitations, the last buffer may be empty. In this case
	the driver will set the ``bytesused`` field to 0, regardless of
	the format. Any subsequent call to the
	:ref:`VIDIOC_DQBUF <VIDIOC_QBUF>` ioctl will not block anymore,
	but return an ``EPIPE`` error code.
        (硬件产生的最后一个缓冲区。当 VIDIOC_QUERYBUF 或 VIDIOC_DQBUF
        ioctl 被调用时，mem2mem 编解码驱动会在捕获队列的最后一个缓冲区上
        设置此标志。由于硬件限制，最后一个缓冲区可能是空的。此时驱动会把
        bytesused 字段设为 0，无论格式如何。此后对 VIDIOC_DQBUF ioctl 的
        任何调用都不再阻塞，而是返回 EPIPE 错误码。)
    * .. _`V4L2-BUF-FLAG-REQUEST-FD`:

      - ``V4L2_BUF_FLAG_REQUEST_FD``
      - 0x00800000
      - The ``request_fd`` field contains a valid file descriptor.
        (request_fd 字段包含有效的文件描述符。)
    * .. _`V4L2-BUF-FLAG-TIMESTAMP-MASK`:

      - ``V4L2_BUF_FLAG_TIMESTAMP_MASK``
      - 0x0000e000
      - Mask for timestamp types below. To test the timestamp type, mask
	out bits not belonging to timestamp type by performing a logical
	and operation with buffer flags and timestamp mask.
        (下方各时间戳类型的掩码。要测试时间戳类型，将缓冲区标志与时间戳
        掩码做按位与运算，屏蔽掉不属于时间戳类型的位。)
    * .. _`V4L2-BUF-FLAG-TIMESTAMP-UNKNOWN`:

      - ``V4L2_BUF_FLAG_TIMESTAMP_UNKNOWN``
      - 0x00000000
      - Unknown timestamp type. This type is used by drivers before Linux
	3.9 and may be either monotonic (see below) or realtime (wall
	clock). Monotonic clock has been favoured in embedded systems
	whereas most of the drivers use the realtime clock. Either kinds
	of timestamps are available in user space via
	:c:func:`clock_gettime` using clock IDs ``CLOCK_MONOTONIC``
	and ``CLOCK_REALTIME``, respectively.
        (未知时间戳类型。Linux 3.9 之前的驱动使用此类型，可能是单调时钟
        (见下) 或实时时钟 (墙上时钟)。嵌入式系统偏爱单调时钟，而大多数
        驱动使用实时时钟。两种时间戳在用户空间都可以通过 clock_gettime
        分别以时钟 ID CLOCK_MONOTONIC 与 CLOCK_REALTIME 获得。)
    * .. _`V4L2-BUF-FLAG-TIMESTAMP-MONOTONIC`:

      - ``V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC``
      - 0x00002000
      - The buffer timestamp has been taken from the ``CLOCK_MONOTONIC``
	clock. To access the same clock outside V4L2, use
	:c:func:`clock_gettime`.
        (缓冲区时间戳取自 CLOCK_MONOTONIC 时钟。要在 V4L2 之外访问同一
        时钟，请使用 clock_gettime。)
    * .. _`V4L2-BUF-FLAG-TIMESTAMP-COPY`:

      - ``V4L2_BUF_FLAG_TIMESTAMP_COPY``
      - 0x00004000
      - The CAPTURE buffer timestamp has been taken from the corresponding
	OUTPUT buffer. This flag applies only to mem2mem devices.
        (CAPTURE 缓冲区的时间戳取自对应的 OUTPUT 缓冲区。此标志只适用于
        mem2mem 设备。)
    * .. _`V4L2-BUF-FLAG-TSTAMP-SRC-MASK`:

      - ``V4L2_BUF_FLAG_TSTAMP_SRC_MASK``
      - 0x00070000
      - Mask for timestamp sources below. The timestamp source defines the
	point of time the timestamp is taken in relation to the frame.
	Logical 'and' operation between the ``flags`` field and
	``V4L2_BUF_FLAG_TSTAMP_SRC_MASK`` produces the value of the
	timestamp source. Applications must set the timestamp source when
	``type`` refers to an output stream and
	``V4L2_BUF_FLAG_TIMESTAMP_COPY`` is set.
        (下方各时间戳来源的掩码。时间戳来源定义时间戳相对于帧的取样时间
        点。flags 字段与 V4L2_BUF_FLAG_TSTAMP_SRC_MASK 的按位与运算即得
        时间戳来源的值。当 type 指向输出流且设置了
        V4L2_BUF_FLAG_TIMESTAMP_COPY 时，应用必须设置时间戳来源。)
    * .. _`V4L2-BUF-FLAG-TSTAMP-SRC-EOF`:

      - ``V4L2_BUF_FLAG_TSTAMP_SRC_EOF``
      - 0x00000000
      - End Of Frame. The buffer timestamp has been taken when the last
	pixel of the frame has been received or the last pixel of the
	frame has been transmitted. In practice, software generated
	timestamps will typically be read from the clock a small amount of
	time after the last pixel has been received or transmitten,
	depending on the system and other activity in it.
        (帧结束 (End Of Frame)。缓冲区时间戳在接收到帧的最后一个像素或
        发送出帧的最后一个像素时获取。实践中，软件生成的时间戳通常会在
        最后一个像素被接收或发送之后的一小段时间才从时钟读取，具体取决
        于系统及其中的其他活动。)
    * .. _`V4L2-BUF-FLAG-TSTAMP-SRC-SOE`:

      - ``V4L2_BUF_FLAG_TSTAMP_SRC_SOE``
      - 0x00010000
      - Start Of Exposure. The buffer timestamp has been taken when the
	exposure of the frame has begun. This is only valid for the
	``V4L2_BUF_TYPE_VIDEO_CAPTURE`` buffer type.
        (曝光开始 (Start Of Exposure)。缓冲区时间戳在帧的曝光开始时获取。
        仅对 V4L2_BUF_TYPE_VIDEO_CAPTURE 缓冲区类型有效。)

.. c:enum:: v4l2_memory

.. raw:: latex

    \normalsize

enum v4l2_memory(v4l2_memory 枚举)
==================================

.. tabularcolumns:: |p{5.0cm}|p{0.8cm}|p{11.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_MEMORY_MMAP``
      - 1
      - The buffer is used for :ref:`memory mapping <mmap>` I/O.
        (缓冲区用于内存映射 I/O。)
    * - ``V4L2_MEMORY_USERPTR``
      - 2
      - The buffer is used for :ref:`user pointer <userp>` I/O.
        (缓冲区用于用户指针 I/O。)
    * - ``V4L2_MEMORY_OVERLAY``
      - 3
      - [to do]
    * - ``V4L2_MEMORY_DMABUF``
      - 4
      - The buffer is used for :ref:`DMA shared buffer <dmabuf>` I/O.
        (缓冲区用于 DMA 共享缓冲区 I/O。)

.. raw:: latex

    \normalsize

Timecodes(时间码)
=================

The :c:type:`v4l2_buffer_timecode` structure is designed to hold a
:ref:`smpte12m` or similar timecode.
(struct :c:type:`timeval` timestamps are stored in the struct
:c:type:`v4l2_buffer` ``timestamp`` field.)
(v4l2_buffer_timecode 结构体设计用于承载 smpte12m 或类似的时间码。
struct timeval 类型的时间戳则存储在 v4l2_buffer 结构体的 timestamp
字段中。)

.. c:type:: v4l2_timecode

struct v4l2_timecode(v4l2_timecode 结构体)
-------------------------------------------

.. tabularcolumns:: |p{1.4cm}|p{2.8cm}|p{13.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Frame rate the timecodes are based on, see :ref:`timecode-type`.
        (时间码所基于的帧率，参见 timecode-type 一节。)
    * - __u32
      - ``flags``
      - Timecode flags, see :ref:`timecode-flags`.
        (时间码标志，参见 timecode-flags 一节。)
    * - __u8
      - ``frames``
      - Frame count, 0 ... 23/24/29/49/59, depending on the type of
	timecode.
        (帧计数，0 ... 23/24/29/49/59，取决于时间码类型。)
    * - __u8
      - ``seconds``
      - Seconds count, 0 ... 59. This is a binary, not BCD number.
        (秒计数，0 ... 59。这是二进制数，不是 BCD 数。)
    * - __u8
      - ``minutes``
      - Minutes count, 0 ... 59. This is a binary, not BCD number.
        (分计数，0 ... 59。这是二进制数，不是 BCD 数。)
    * - __u8
      - ``hours``
      - Hours count, 0 ... 29. This is a binary, not BCD number.
        (时计数，0 ... 29。这是二进制数，不是 BCD 数。)
    * - __u8
      - ``userbits``\ [4]
      - The "user group" bits from the timecode.
        (时间码中的 "用户组" 位。)


.. _timecode-type:

Timecode Types(时间码类型)
--------------------------

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_TC_TYPE_24FPS``
      - 1
      - 24 frames per second, i. e. film.
        (每秒 24 帧，即胶片电影。)
    * - ``V4L2_TC_TYPE_25FPS``
      - 2
      - 25 frames per second, i. e. PAL or SECAM video.
        (每秒 25 帧，即 PAL 或 SECAM 视频。)
    * - ``V4L2_TC_TYPE_30FPS``
      - 3
      - 30 frames per second, i. e. NTSC video.
        (每秒 30 帧，即 NTSC 视频。)
    * - ``V4L2_TC_TYPE_50FPS``
      - 4
      -
    * - ``V4L2_TC_TYPE_60FPS``
      - 5
      -


.. _timecode-flags:

Timecode Flags(时间码标志)
--------------------------

.. tabularcolumns:: |p{6.6cm}|p{1.4cm}|p{9.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_TC_FLAG_DROPFRAME``
      - 0x0001
      - Indicates "drop frame" semantics for counting frames in 29.97 fps
	material. When set, frame numbers 0 and 1 at the start of each
	minute, except minutes 0, 10, 20, 30, 40, 50 are omitted from the
	count.
        (指示在 29.97 fps 素材中计数帧时采用 "丢帧" (drop frame) 语义。
        置位时，除第 0、10、20、30、40、50 分钟外，每分钟开头的帧号 0 和
        1 不计入计数。)
    * - ``V4L2_TC_FLAG_COLORFRAME``
      - 0x0002
      - The "color frame" flag.
        ("彩色帧" (color frame) 标志。)
    * - ``V4L2_TC_USERBITS_field``
      - 0x000C
      - Field mask for the "binary group flags".
        ("二进制组标志" (binary group flags) 的字段掩码。)
    * - ``V4L2_TC_USERBITS_USERDEFINED``
      - 0x0000
      - Unspecified format.
        (未指定的格式。)
    * - ``V4L2_TC_USERBITS_8BITCHARS``
      - 0x0008
      - 8-bit ISO characters.
        (8 位 ISO 字符。)
