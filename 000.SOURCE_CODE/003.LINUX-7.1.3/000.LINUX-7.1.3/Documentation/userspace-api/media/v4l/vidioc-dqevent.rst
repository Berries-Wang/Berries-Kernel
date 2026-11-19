.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_DQEVENT:

******************************
ioctl VIDIOC_DQEVENT(出队事件)
******************************

Name(名称)
==========

VIDIOC_DQEVENT - Dequeue event
(VIDIOC_DQEVENT - 出队事件)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_DQEVENT

``int ioctl(int fd, VIDIOC_DQEVENT, struct v4l2_event *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 open() 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_event`.
    (指向 struct v4l2_event 的指针。)

Description(描述)
=================

Dequeue an event from a video device. No input is required for this
ioctl. All the fields of the struct :c:type:`v4l2_event`
structure are filled by the driver. The file handle will also receive
exceptions which the application may get by e.g. using the select system
call.
(从视频设备出队一个事件。此 ioctl 不需要任何输入。struct v4l2_event
结构体的所有字段都由驱动填充。该文件句柄也会收到异常 (exceptions)，
应用程序可以用例如 select 系统调用来获取它们。)

.. c:type:: v4l2_event

.. tabularcolumns:: |p{3.0cm}|p{3.4cm}|p{10.9cm}|


.. flat-table:: struct v4l2_event
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Type of the event, see :ref:`event-type`.
	(事件类型，见 event-type。)
    * - union {
      - ``u``
    * - struct :c:type:`v4l2_event_vsync`
      - ``vsync``
      - Event data for event ``V4L2_EVENT_VSYNC``.
	(V4L2_EVENT_VSYNC 事件的事件数据。)
    * - struct :c:type:`v4l2_event_ctrl`
      - ``ctrl``
      - Event data for event ``V4L2_EVENT_CTRL``.
	(V4L2_EVENT_CTRL 事件的事件数据。)
    * - struct :c:type:`v4l2_event_frame_sync`
      - ``frame_sync``
      - Event data for event ``V4L2_EVENT_FRAME_SYNC``.
	(V4L2_EVENT_FRAME_SYNC 事件的事件数据。)
    * - struct :c:type:`v4l2_event_motion_det`
      - ``motion_det``
      - Event data for event V4L2_EVENT_MOTION_DET.
	(V4L2_EVENT_MOTION_DET 事件的事件数据。)
    * - struct :c:type:`v4l2_event_src_change`
      - ``src_change``
      - Event data for event V4L2_EVENT_SOURCE_CHANGE.
	(V4L2_EVENT_SOURCE_CHANGE 事件的事件数据。)
    * - __u8
      - ``data``\ [64]
      - Event data. Defined by the event type. The union should be used to
	define easily accessible type for events.
	(事件数据。由事件类型定义。应使用该联合体为事件定义易于访问的类型。)
    * - }
      -
    * - __u32
      - ``pending``
      - Number of pending events excluding this one.
	(除本事件之外的待处理事件数。)
    * - __u32
      - ``sequence``
      - Event sequence number. The sequence number is incremented for
	every subscribed event that takes place. If sequence numbers are
	not contiguous it means that events have been lost.
	(事件序列号。对发生的每一个已订阅事件，序列号都会递增。如果序列号
	不连续，说明有事件丢失了。)
    * - struct timespec
      - ``timestamp``
      - Event timestamp. The timestamp has been taken from the
	``CLOCK_MONOTONIC`` clock. To access the same clock outside V4L2,
	use :c:func:`clock_gettime`.
	(事件时间戳。该时间戳取自 ``CLOCK_MONOTONIC`` 时钟。要在 V4L2 之外
	访问同一时钟，请使用 clock_gettime。)
    * - u32
      - ``id``
      - The ID associated with the event source. If the event does not
	have an associated ID (this depends on the event type), then this
	is 0.
	(与事件源相关联的 ID。如果该事件没有关联的 ID (取决于事件类型)，
	则为 0。)
    * - __u32
      - ``reserved``\ [8]
      - Reserved for future extensions. Drivers must set the array to
	zero.
	(保留供未来扩展使用。驱动必须将该数组置零。)


.. tabularcolumns:: |p{6.2cm}|p{2.6cm}|p{8.5cm}|

.. cssclass:: longtable

.. _event-type:

.. flat-table:: Event Types
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_EVENT_ALL``
      - 0
      - All events. V4L2_EVENT_ALL is valid only for
	VIDIOC_UNSUBSCRIBE_EVENT for unsubscribing all events at once.
	(所有事件。V4L2_EVENT_ALL 仅对 VIDIOC_UNSUBSCRIBE_EVENT 有效，用于
	一次性取消订阅所有事件。)
    * - ``V4L2_EVENT_VSYNC``
      - 1
      - This event is triggered on the vertical sync. This event has a
	struct :c:type:`v4l2_event_vsync` associated
	with it.
	(该事件在垂直同步 (vertical sync) 时触发。该事件关联一个 struct
	v4l2_event_vsync。)
    * - ``V4L2_EVENT_EOS``
      - 2
      - This event is triggered when the end of a stream is reached. This
	is typically used with MPEG decoders to report to the application
	when the last of the MPEG stream has been decoded.
	(当到达流的末尾时触发该事件。这通常与 MPEG 解码器一起使用，用于在
	MPEG 流的最后一部分被解码时向应用程序报告。)
    * - ``V4L2_EVENT_CTRL``
      - 3
      - This event requires that the ``id`` matches the control ID from
	which you want to receive events. This event is triggered if the
	control's value changes, if a button control is pressed or if the
	control's flags change. This event has a struct
	:c:type:`v4l2_event_ctrl` associated with it.
	This struct contains much of the same information as struct
	:ref:`v4l2_queryctrl <v4l2-queryctrl>` and struct
	:c:type:`v4l2_control`.
	(该事件要求 ``id`` 匹配你希望从中接收事件的控件 ID。当控件的值变化、
	按钮控件被按下或控件的标志变化时触发该事件。该事件关联一个 struct
	v4l2_event_ctrl。该结构体包含与 struct v4l2_queryctrl 和 struct
	v4l2_control 大体相同的信息。)

	If the event is generated due to a call to
	:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` or
	:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`, then the
	event will *not* be sent to the file handle that called the ioctl
	function. This prevents nasty feedback loops. If you *do* want to
	get the event, then set the ``V4L2_EVENT_SUB_FL_ALLOW_FEEDBACK``
	flag.
	(如果事件是因调用 VIDIOC_S_CTRL 或 VIDIOC_S_EXT_CTRLS 而产生的，
	则该事件不会被发送给调用 ioctl 函数的文件句柄。这可以避免糟糕的
	反馈循环。如果你确实想收到该事件，请设置
	``V4L2_EVENT_SUB_FL_ALLOW_FEEDBACK`` 标志。)

	This event type will ensure that no information is lost when more
	events are raised than there is room internally. In that case the
	struct :c:type:`v4l2_event_ctrl` of the
	second-oldest event is kept, but the ``changes`` field of the
	second-oldest event is ORed with the ``changes`` field of the
	oldest event.
	(当产生的事件多于内部可容纳的数量时，此事件类型可确保不丢失信息。
	在这种情况下，会保留次旧事件的 struct v4l2_event_ctrl，但次旧事件
	的 ``changes`` 字段会与最旧事件的 ``changes`` 字段按位或。)
    * - ``V4L2_EVENT_FRAME_SYNC``
      - 4
      - Triggered immediately when the reception of a frame has begun.
	This event has a struct
	:c:type:`v4l2_event_frame_sync`
	associated with it.
	(在一帧的接收开始时立即触发。该事件关联一个 struct
	v4l2_event_frame_sync。)

	If the hardware needs to be stopped in the case of a buffer
	underrun it might not be able to generate this event. In such
	cases the ``frame_sequence`` field in struct
	:c:type:`v4l2_event_frame_sync` will not
	be incremented. This causes two consecutive frame sequence numbers
	to have n times frame interval in between them.
	(如果在缓冲区欠载 (buffer underrun) 的情况下需要停止硬件，它可能
	无法产生该事件。此时 struct v4l2_event_frame_sync 中的
	``frame_sequence`` 字段不会递增。这导致两个连续的帧序号之间相隔
	n 倍的帧间隔。)
    * - ``V4L2_EVENT_SOURCE_CHANGE``
      - 5
      - This event is triggered when a source parameter change is detected
	during runtime by the video device. It can be a runtime resolution
	change triggered by a video decoder or the format change happening
	on an input connector. This event requires that the ``id`` matches
	the input index (when used with a video device node) or the pad
	index (when used with a subdevice node) from which you want to
	receive events.
	(当视频设备在运行时检测到源参数变化时触发该事件。它可以是视频解码器
	触发的运行时分辨率变化，也可以是输入连接器上发生的格式变化。该事件
	要求 ``id`` 匹配你希望从中接收事件的输入索引 (用于视频设备节点时)
	或 pad 索引 (用于子设备节点时)。)

	This event has a struct
	:c:type:`v4l2_event_src_change`
	associated with it. The ``changes`` bitfield denotes what has
	changed for the subscribed pad. If multiple events occurred before
	application could dequeue them, then the changes will have the
	ORed value of all the events generated.
	(该事件关联一个 struct v4l2_event_src_change。``changes`` 位域表示
	已订阅的 pad 上发生了什么变化。如果应用程序尚未出队时已发生多个
	事件，则 changes 将是所有已产生事件的按位或值。)
    * - ``V4L2_EVENT_MOTION_DET``
      - 6
      - Triggered whenever the motion detection state for one or more of
	the regions changes. This event has a struct
	:c:type:`v4l2_event_motion_det`
	associated with it.
	(当一个或多个区域的运动检测状态发生变化时触发。该事件关联一个
	struct v4l2_event_motion_det。)
    * - ``V4L2_EVENT_PRIVATE_START``
      - 0x08000000
      - Base event number for driver-private events.
	(驱动私有事件的基础事件号。)


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_event_vsync

.. flat-table:: struct v4l2_event_vsync
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``field``
      - The upcoming field. See enum :c:type:`v4l2_field`.
	(即将到来的场 (field)。见 enum v4l2_field。)


.. tabularcolumns:: |p{3.5cm}|p{3.0cm}|p{10.8cm}|

.. c:type:: v4l2_event_ctrl

.. flat-table:: struct v4l2_event_ctrl
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``changes``
      - A bitmask that tells what has changed. See
	:ref:`ctrl-changes-flags`.
	(一个位掩码，指示发生了什么变化。见 ctrl-changes-flags。)
    * - __u32
      - ``type``
      - The type of the control. See enum
	:c:type:`v4l2_ctrl_type`.
	(控件的类型。见 enum v4l2_ctrl_type。)
    * - union {
      - (anonymous)(匿名)
    * - __s32
      - ``value``
      - The 32-bit value of the control for 32-bit control types. This is
	0 for string controls since the value of a string cannot be passed
	using :ref:`VIDIOC_DQEVENT`.
	(32 位控件类型的 32 位控件值。对于字符串控件该值为 0，因为字符串
	的值无法通过 VIDIOC_DQEVENT 传递。)
    * - __s64
      - ``value64``
      - The 64-bit value of the control for 64-bit control types.
	(64 位控件类型的 64 位控件值。)
    * - }
      -
    * - __u32
      - ``flags``
      - The control flags. See :ref:`control-flags`.
	(控件标志。见 control-flags。)
    * - __s32
      - ``minimum``
      - The minimum value of the control. See struct
	:ref:`v4l2_queryctrl <v4l2-queryctrl>`.
	(控件的最小值。见 struct v4l2_queryctrl。)
    * - __s32
      - ``maximum``
      - The maximum value of the control. See struct
	:ref:`v4l2_queryctrl <v4l2-queryctrl>`.
	(控件的最大值。见 struct v4l2_queryctrl。)
    * - __s32
      - ``step``
      - The step value of the control. See struct
	:ref:`v4l2_queryctrl <v4l2-queryctrl>`.
	(控件的步长值。见 struct v4l2_queryctrl。)
    * - __s32
      - ``default_value``
      - The default value of the control. See struct
	:ref:`v4l2_queryctrl <v4l2-queryctrl>`.
	(控件的默认值。见 struct v4l2_queryctrl。)


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_event_frame_sync

.. flat-table:: struct v4l2_event_frame_sync
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``frame_sequence``
      - The sequence number of the frame being received.
	(正在接收的帧的序列号。)


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_event_src_change

.. flat-table:: struct v4l2_event_src_change
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``changes``
      - A bitmask that tells what has changed. See
	:ref:`src-changes-flags`.
	(一个位掩码，指示发生了什么变化。见 src-changes-flags。)


.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_event_motion_det

.. flat-table:: struct v4l2_event_motion_det
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``flags``
      - Currently only one flag is available: if
	``V4L2_EVENT_MD_FL_HAVE_FRAME_SEQ`` is set, then the
	``frame_sequence`` field is valid, otherwise that field should be
	ignored.
	(目前只有一个标志可用：如果设置了 ``V4L2_EVENT_MD_FL_HAVE_FRAME_SEQ``，
	则 ``frame_sequence`` 字段有效，否则该字段应被忽略。)
    * - __u32
      - ``frame_sequence``
      - The sequence number of the frame being received. Only valid if the
	``V4L2_EVENT_MD_FL_HAVE_FRAME_SEQ`` flag was set.
	(正在接收的帧的序列号。仅在设置了 ``V4L2_EVENT_MD_FL_HAVE_FRAME_SEQ``
	标志时有效。)
    * - __u32
      - ``region_mask``
      - The bitmask of the regions that reported motion. There is at least
	one region. If this field is 0, then no motion was detected at
	all. If there is no ``V4L2_CID_DETECT_MD_REGION_GRID`` control
	(see :ref:`detect-controls`) to assign a different region to
	each cell in the motion detection grid, then that all cells are
	automatically assigned to the default region 0.
	(报告了运动的区域的位掩码。至少有一个区域。如果该字段为 0，则完全
	没有检测到运动。如果没有 ``V4L2_CID_DETECT_MD_REGION_GRID`` 控件
	(见 detect-controls) 来为运动检测网格中的每个单元格分配不同的
	区域，则所有单元格都会自动分配到默认区域 0。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _ctrl-changes-flags:

.. flat-table:: Control Changes
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_EVENT_CTRL_CH_VALUE``
      - 0x0001
      - This control event was triggered because the value of the control
	changed. Special cases: Volatile controls do no generate this
	event; If a control has the ``V4L2_CTRL_FLAG_EXECUTE_ON_WRITE``
	flag set, then this event is sent as well, regardless its value.
	(触发此控件事件是因为控件的值发生了变化。特殊情况：易失性 (volatile)
	控件不产生该事件；如果控件设置了 ``V4L2_CTRL_FLAG_EXECUTE_ON_WRITE``
	标志，则无论其值如何，也会发送该事件。)
    * - ``V4L2_EVENT_CTRL_CH_FLAGS``
      - 0x0002
      - This control event was triggered because the control flags
	changed.
	(触发此控件事件是因为控件标志发生了变化。)
    * - ``V4L2_EVENT_CTRL_CH_RANGE``
      - 0x0004
      - This control event was triggered because the minimum, maximum,
	step or the default value of the control changed.
	(触发此控件事件是因为控件的最小值、最大值、步长或默认值发生了
	变化。)
    * - ``V4L2_EVENT_CTRL_CH_DIMENSIONS``
      - 0x0008
      - This control event was triggered because the dimensions of the
	control changed. Note that the number of dimensions remains the
	same.
	(触发此控件事件是因为控件的维度发生了变化。注意维度的数量保持
	不变。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _src-changes-flags:

.. flat-table:: Source Changes
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_EVENT_SRC_CH_RESOLUTION``
      - 0x0001
      - This event gets triggered when a resolution change is detected at
	an input. This can come from an input connector or from a video
	decoder. Applications will have to query the new resolution (if
	any, the signal may also have been lost).
	(在输入上检测到分辨率变化时触发该事件。它可能来自输入连接器，也
	可能来自视频解码器。应用程序必须查询新的分辨率 (如果有的话，信号
	也可能已丢失)。)

	For stateful decoders follow the guidelines in :ref:`decoder`.
	Video Capture devices have to query the new timings using
	:ref:`VIDIOC_QUERY_DV_TIMINGS` or
	:ref:`VIDIOC_QUERYSTD <VIDIOC_QUERYSTD>`.
	(对于有状态解码器，请遵循 decoder 中的指南。视频捕获设备必须使用
	VIDIOC_QUERY_DV_TIMINGS 或 VIDIOC_QUERYSTD 查询新的时序。)

	*Important*: even if the new video timings appear identical to the old
	ones, receiving this event indicates that there was an issue with the
	video signal and you must stop and restart streaming
	(:ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>`
	followed by :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>`). The reason is
	that many Video Capture devices are not able to recover from a temporary
	loss of signal and so restarting streaming I/O is required in order for
	the hardware to synchronize to the video signal.
	(*Important* (重要)：即使新的视频时序看起来与旧的完全相同，收到此
	事件也表明视频信号存在问题，你必须停止并重新启动流式传输
	(VIDIOC_STREAMOFF 之后接 VIDIOC_STREAMON)。原因在于许多视频捕获
	设备无法从暂时的信号丢失中恢复，因此必须重启流式 I/O 才能让硬件
	与视频信号同步。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码见
Generic Error Codes (通用错误码) 一章。)
