.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_DECODER_CMD:

****************************************************************
ioctl VIDIOC_DECODER_CMD, VIDIOC_TRY_DECODER_CMD(执行解码器命令)
****************************************************************

Name(名称)
==========

VIDIOC_DECODER_CMD - VIDIOC_TRY_DECODER_CMD - Execute an decoder command
(VIDIOC_DECODER_CMD - VIDIOC_TRY_DECODER_CMD - 执行解码器命令)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_DECODER_CMD

``int ioctl(int fd, VIDIOC_DECODER_CMD, struct v4l2_decoder_cmd *argp)``

.. c:macro:: VIDIOC_TRY_DECODER_CMD

``int ioctl(int fd, VIDIOC_TRY_DECODER_CMD, struct v4l2_decoder_cmd *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    pointer to struct :c:type:`v4l2_decoder_cmd`.
    (指向 struct :c:type:`v4l2_decoder_cmd` 的指针。)

Description(描述)
=================

These ioctls control an audio/video (usually MPEG-) decoder.
``VIDIOC_DECODER_CMD`` sends a command to the decoder,
``VIDIOC_TRY_DECODER_CMD`` can be used to try a command without actually
executing it. To send a command applications must initialize all fields
of a struct :c:type:`v4l2_decoder_cmd` and call
``VIDIOC_DECODER_CMD`` or ``VIDIOC_TRY_DECODER_CMD`` with a pointer to
this structure.
(这些 ioctl 控制音频/视频(通常是 MPEG)解码器。 ``VIDIOC_DECODER_CMD`` 向解码器发送一条命令； ``VIDIOC_TRY_DECODER_CMD`` 可用于试运行一条命令而不实际执行它。要发送命令，应用程序必须初始化 struct :c:type:`v4l2_decoder_cmd` 的所有字段，并以指向该结构的指针调用 ``VIDIOC_DECODER_CMD`` 或 ``VIDIOC_TRY_DECODER_CMD`` 。)

The ``cmd`` field must contain the command code. Some commands use the
``flags`` field for additional information.
( ``cmd`` 字段必须包含命令代码。某些命令会使用 ``flags`` 字段来携带附加信息。)

A :c:func:`write()` or :ref:`VIDIOC_STREAMON`
call sends an implicit START command to the decoder if it has not been
started yet. Applies to both queues of mem2mem decoders.
(如果解码器尚未启动， :c:func:`write()` 或 :ref:`VIDIOC_STREAMON` 调用会向解码器发送一条隐式的 START 命令。这适用于 mem2mem 解码器的两个队列。)

A :c:func:`close()` or :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>`
call of a streaming file descriptor sends an implicit immediate STOP
command to the decoder, and all buffered data is discarded. Applies to both
queues of mem2mem decoders.
(对流文件描述符调用 :c:func:`close()` 或 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 会向解码器发送一条隐式的立即 STOP 命令，所有缓冲的数据都会被丢弃。这适用于 mem2mem 解码器的两个队列。)

In principle, these ioctls are optional, not all drivers may support them. They were
introduced in Linux 3.3. They are, however, mandatory for stateful mem2mem decoders
(as further documented in :ref:`decoder`).
(原则上，这些 ioctl 是可选的，并非所有驱动都支持。它们在 Linux 3.3 中引入。不过，对于有状态的 mem2mem 解码器(详见 :ref:`decoder` )，它们是强制要求的。)

.. tabularcolumns:: |p{2.0cm}|p{1.1cm}|p{2.2cm}|p{11.8cm}|

.. c:type:: v4l2_decoder_cmd

.. cssclass:: longtable

.. flat-table:: struct v4l2_decoder_cmd
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 1 1 3

    * - __u32
      - ``cmd``
      -
      - The decoder command, see :ref:`decoder-cmds`.
        (解码器命令，参见 :ref:`decoder-cmds` 。)
    * - __u32
      - ``flags``
      -
      - Flags to go with the command. If no flags are defined for this
	command, drivers and applications must set this field to zero.
        (随命令一起使用的标志。如果此命令没有定义任何标志，驱动和应用程序必须把此字段置零。)
    * - union {
      - (anonymous)
    * - struct
      - ``start``
      -
      - Structure containing additional data for the
	``V4L2_DEC_CMD_START`` command.
        (包含 ``V4L2_DEC_CMD_START`` 命令附加数据的结构。)
    * -
      - __s32
      - ``speed``
      - Playback speed and direction. The playback speed is defined as
	``speed``/1000 of the normal speed. So 1000 is normal playback.
	Negative numbers denote reverse playback, so -1000 does reverse
	playback at normal speed. Speeds -1, 0 and 1 have special
	meanings: speed 0 is shorthand for 1000 (normal playback). A speed
	of 1 steps just one frame forward, a speed of -1 steps just one
	frame back.
        (播放速度与方向。播放速度定义为正常速度的 ``speed``/1000。因此 1000 为正常播放。负数表示倒放，例如 -1000 表示以正常速度倒放。速度 -1、0 和 1 有特殊含义：速度 0 是 1000(正常播放)的简写。速度 1 表示单步前进一帧，速度 -1 表示单步后退一帧。)
    * -
      - __u32
      - ``format``
      - Format restrictions. This field is set by the driver, not the
	application. Possible values are ``V4L2_DEC_START_FMT_NONE`` if
	there are no format restrictions or ``V4L2_DEC_START_FMT_GOP`` if
	the decoder operates on full GOPs (*Group Of Pictures*). This is
	usually the case for reverse playback: the decoder needs full
	GOPs, which it can then play in reverse order. So to implement
	reverse playback the application must feed the decoder the last
	GOP in the video file, then the GOP before that, etc. etc.
        (格式限制。此字段由驱动设置，而不是应用程序。可能的取值： ``V4L2_DEC_START_FMT_NONE`` 表示没有格式限制； ``V4L2_DEC_START_FMT_GOP`` 表示解码器以完整 GOP(*Group Of Pictures*，图像组)为单位工作。倒放通常属于后一种情况：解码器需要完整的 GOP，然后才能按相反顺序播放。因此要实现倒放，应用程序必须先把视频文件中最后一个 GOP 送给解码器，再送前一个 GOP，依此类推。)
    * - struct
      - ``stop``
      -
      - Structure containing additional data for the ``V4L2_DEC_CMD_STOP``
	command.
        (包含 ``V4L2_DEC_CMD_STOP`` 命令附加数据的结构。)
    * -
      - __u64
      - ``pts``
      - Stop playback at this ``pts`` or immediately if the playback is
	already past that timestamp. Leave to 0 if you want to stop after
	the last frame was decoded.
        (在此 ``pts`` 处停止播放；如果播放已越过该时间戳，则立即停止。若想在最后一帧解码完毕后停止，请置为 0。)
    * - struct
      - ``raw``
    * -
      - __u32
      - ``data``\ [16]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
        (为未来的扩展保留。驱动和应用程序必须把该数组置零。)
    * - }
      -


.. tabularcolumns:: |p{5.6cm}|p{0.6cm}|p{11.1cm}|

.. cssclass:: longtable

.. _decoder-cmds:

.. flat-table:: Decoder Commands
    :header-rows:  0
    :stub-columns: 0
    :widths: 56 6 113

    * - ``V4L2_DEC_CMD_START``
      - 0
      - Start the decoder. When the decoder is already running or paused,
	this command will just change the playback speed. That means that
	calling ``V4L2_DEC_CMD_START`` when the decoder was paused will
	*not* resume the decoder. You have to explicitly call
	``V4L2_DEC_CMD_RESUME`` for that. This command has one flag:
	``V4L2_DEC_CMD_START_MUTE_AUDIO``. If set, then audio will be
	muted when playing back at a non-standard speed.
        (启动解码器。当解码器已经在运行或已暂停时，该命令只会改变播放速度。这意味着在解码器被暂停时调用 ``V4L2_DEC_CMD_START`` *不会* 恢复解码器，必须显式调用 ``V4L2_DEC_CMD_RESUME`` 才行。该命令有一个标志： ``V4L2_DEC_CMD_START_MUTE_AUDIO``。如果设置，则以非标准速度播放时音频将被静音。)

	For a device implementing the :ref:`decoder`, once the drain sequence
	is initiated with the ``V4L2_DEC_CMD_STOP`` command, it must be driven
	to completion before this command can be invoked.  Any attempt to
	invoke the command while the drain sequence is in progress will trigger
	an ``EBUSY`` error code.  The command may be also used to restart the
	decoder in case of an implicit stop initiated by the decoder itself,
	without the ``V4L2_DEC_CMD_STOP`` being called explicitly. See
	:ref:`decoder` for more details.
        (对于实现 :ref:`decoder` 的设备，一旦用 ``V4L2_DEC_CMD_STOP`` 命令启动了排空(drain)序列，就必须让它完成之后才能调用本命令。在排空序列进行期间调用本命令的任何尝试都会触发 ``EBUSY`` 错误码。当解码器自身发起了隐式停止、而没有显式调用 ``V4L2_DEC_CMD_STOP`` 时，本命令也可用于重启解码器。详见 :ref:`decoder` 。)
    * - ``V4L2_DEC_CMD_STOP``
      - 1
      - Stop the decoder. When the decoder is already stopped, this
	command does nothing. This command has two flags: if
	``V4L2_DEC_CMD_STOP_TO_BLACK`` is set, then the decoder will set
	the picture to black after it stopped decoding. Otherwise the last
	image will repeat. If
	``V4L2_DEC_CMD_STOP_IMMEDIATELY`` is set, then the decoder stops
	immediately (ignoring the ``pts`` value), otherwise it will keep
	decoding until timestamp >= pts or until the last of the pending
	data from its internal buffers was decoded.
        (停止解码器。当解码器已经停止时，该命令不做任何事。该命令有两个标志：如果设置了 ``V4L2_DEC_CMD_STOP_TO_BLACK``，解码器将在停止解码后把画面置为黑色；否则最后一幅图像会重复显示。如果设置了 ``V4L2_DEC_CMD_STOP_IMMEDIATELY``，解码器会立即停止(忽略 ``pts`` 值)；否则它会继续解码，直到时间戳 >= pts，或者其内部缓冲区中最后一份待处理数据解码完毕。)

	For a device implementing the :ref:`decoder`, the command will initiate
	the drain sequence as documented in :ref:`decoder`.  No flags or other
	arguments are accepted in this case. Any attempt to invoke the command
	again before the sequence completes will trigger an ``EBUSY`` error
	code.
        (对于实现 :ref:`decoder` 的设备，该命令将按 :ref:`decoder` 中的文档说明启动排空序列。在这种情况下不接受任何标志或其他参数。在序列完成之前再次调用该命令的任何尝试都会触发 ``EBUSY`` 错误码。)
    * - ``V4L2_DEC_CMD_PAUSE``
      - 2
      - Pause the decoder. When the decoder has not been started yet, the
	driver will return an ``EPERM`` error code. When the decoder is
	already paused, this command does nothing. This command has one
	flag: if ``V4L2_DEC_CMD_PAUSE_TO_BLACK`` is set, then set the
	decoder output to black when paused.
        (暂停解码器。当解码器尚未启动时，驱动会返回 ``EPERM`` 错误码。当解码器已经暂停时，该命令不做任何事。该命令有一个标志：如果设置了 ``V4L2_DEC_CMD_PAUSE_TO_BLACK``，则在暂停时把解码器输出置为黑色。)
    * - ``V4L2_DEC_CMD_RESUME``
      - 3
      - Resume decoding after a PAUSE command. When the decoder has not
	been started yet, the driver will return an ``EPERM`` error code. When
	the decoder is already running, this command does nothing. No
	flags are defined for this command.
        (在 PAUSE 命令之后恢复解码。当解码器尚未启动时，驱动会返回 ``EPERM`` 错误码。当解码器已经在运行时，该命令不做任何事。该命令没有定义任何标志。)
    * - ``V4L2_DEC_CMD_FLUSH``
      - 4
      - Flush any held capture buffers. Only valid for stateless decoders.
	This command is typically used when the application reached the
	end of the stream and the last output buffer had the
	``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` flag set. This would prevent
	dequeueing the capture buffer containing the last decoded frame.
	So this command can be used to explicitly flush that final decoded
	frame. This command does nothing if there are no held capture buffers.
        (冲刷所有被持有的捕获缓冲区。仅对无状态(stateless)解码器有效。当应用程序到达流的末尾且最后一个输出缓冲区带有 ``V4L2_BUF_FLAG_M2M_HOLD_CAPTURE_BUF`` 标志时，通常使用该命令。这会阻止含有最后已解码帧的捕获缓冲区出队。因此该命令可用于显式地冲刷最后那个已解码的帧。如果没有被持有的捕获缓冲区，该命令不做任何事。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码见 :ref:`Generic Error Codes <gen-errors>` 一章。)

EBUSY
    A drain sequence of a device implementing the :ref:`decoder` is still in
    progress. It is not allowed to issue another decoder command until it
    completes.
    (实现 :ref:`decoder` 的设备的排空序列仍在进行中。在该序列完成之前，不允许发出其他解码器命令。)

EINVAL
    The ``cmd`` field is invalid.
    ( ``cmd`` 字段无效。)

EPERM
    The application sent a PAUSE or RESUME command when the decoder was
    not running.
    (解码器未运行时，应用程序发送了 PAUSE 或 RESUME 命令。)
