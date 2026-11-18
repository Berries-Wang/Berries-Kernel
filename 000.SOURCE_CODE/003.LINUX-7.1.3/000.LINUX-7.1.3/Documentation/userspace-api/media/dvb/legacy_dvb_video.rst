.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later OR GPL-2.0

.. c:namespace:: dtv.legacy.video

.. _dvb_video:

==============================
DVB Video Device(DVB 视频设备)
==============================

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

The DVB video device controls the MPEG2 video decoder of the DVB
hardware. It can be accessed through ``/dev/dvb/adapter0/video0``. Data
types and ioctl definitions can be accessed by including
``linux/dvb/video.h`` in your application.
(DVB 视频设备控制 DVB 硬件中的 MPEG2 视频解码器。可以通过 ``/dev/dvb/adapter0/video0`` 访问该设备。在应用程序中包含 ``linux/dvb/video.h`` 头文件，即可获得相关数据类型和 ioctl 定义。)

Note that the DVB video device only controls decoding of the MPEG video
stream, not its presentation on the TV or computer screen. On PCs this
is typically handled by an associated video4linux device, e.g.
``/dev/video``, which allows scaling and defining output windows.
(注意，DVB 视频设备只控制 MPEG 视频流的解码，而不负责其在电视或计算机屏幕上的呈现。在 PC 上，这通常由关联的 video4linux 设备（例如 ``/dev/video`` ）处理，它支持缩放和定义输出窗口。)

Most DVB cards don’t have their own MPEG decoder, which results in the
omission of the audio and video device as well as the video4linux
device.
(大多数 DVB 卡没有自己的 MPEG 解码器，因此这些卡也就没有音频、视频设备以及 video4linux 设备。)

These ioctls were also used by V4L2 to control MPEG decoders implemented
in V4L2. The use of these ioctls for that purpose has been made obsolete
and proper V4L2 ioctls or controls have been created to replace that
functionality. Use :ref:`V4L2 ioctls<video>` for new drivers!
(这些 ioctl 也曾被 V4L2 用于控制 V4L2 中实现的 MPEG 解码器。这种用途如今已被废弃，V4L2 已经创建了相应的 ioctl 或控件来取代该功能。新驱动请使用 :ref:`V4L2 ioctls<video>`！)


Video Data Types(视频数据类型)
==============================



video_format_t(视频格式类型)
----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef enum {
	VIDEO_FORMAT_4_3,
	VIDEO_FORMAT_16_9,
	VIDEO_FORMAT_221_1
    } video_format_t;

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``VIDEO_FORMAT_4_3``

       -  Select 4:3 format.(选择 4:3 格式。)

    -  ..

       -  ``VIDEO_FORMAT_16_9``

       -  Select 16:9 format.(选择 16:9 格式。)

    -  ..

       -  ``VIDEO_FORMAT_221_1``

       -  Select 2.21:1 format.(选择 2.21:1 格式。)

Description(描述)
~~~~~~~~~~~~~~~~~

The ``video_format_t`` data type
is used in the `VIDEO_SET_FORMAT(设置屏幕格式)`_ function to tell the driver which
aspect ratio the output hardware (e.g. TV) has. It is also used in the
data structures `video_status(视频状态)`_ returned by `VIDEO_GET_STATUS(获取视频状态)`_
and `video_event(视频事件)`_ returned by `VIDEO_GET_EVENT(获取视频事件)`_ which report
about the display format of the current video stream.
(``video_format_t`` 数据类型用于 `VIDEO_SET_FORMAT(设置屏幕格式)`_ 函数，告知驱动程序输出硬件（例如电视）具有哪种宽高比。它也用于 `VIDEO_GET_STATUS(获取视频状态)`_ 返回的 `video_status(视频状态)`_ 数据结构，以及 `VIDEO_GET_EVENT(获取视频事件)`_ 返回的 `video_event(视频事件)`_ 数据结构，二者报告当前视频流的显示格式。)


-----


video_displayformat_t(显示格式类型)
-----------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef enum {
	VIDEO_PAN_SCAN,
	VIDEO_LETTER_BOX,
	VIDEO_CENTER_CUT_OUT
    } video_displayformat_t;

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``VIDEO_PAN_SCAN``

       -  Use pan and scan format.(使用平移扫描 (pan & scan) 格式。)

    -  ..

       -  ``VIDEO_LETTER_BOX``

       -  Use letterbox format.(使用信箱 (letterbox) 格式。)

    -  ..

       -  ``VIDEO_CENTER_CUT_OUT``

       -  Use center cut out format.(使用居中裁剪 (center cut-out) 格式。)

Description(描述)
~~~~~~~~~~~~~~~~~

In case the display format of the video stream and of the display
hardware differ the application has to specify how to handle the
cropping of the picture. This can be done using the
`VIDEO_SET_DISPLAY_FORMAT(设置显示格式)`_ call which accepts this enum as argument.
(当视频流的显示格式与显示硬件的格式不一致时，应用程序必须指定如何处理画面裁剪。这可以通过 `VIDEO_SET_DISPLAY_FORMAT(设置显示格式)`_ 调用来完成，该调用接受此枚举作为参数。)


-----


video_size_t(视频尺寸类型)
--------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef struct {
	int w;
	int h;
	video_format_t aspect_ratio;
    } video_size_t;

Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int w``

       -  Video width in pixels.(视频宽度，单位为像素。)

    -  ..

       -  ``int h``

       -  Video height in pixels.(视频高度，单位为像素。)

    -  ..

       -  `video_format_t(视频格式类型)`_ ``aspect_ratio``

       -  Aspect ratio.(宽高比。)

Description(描述)
~~~~~~~~~~~~~~~~~

Used in the struct `video_event(视频事件)`_. It stores the resolution and
aspect ratio of the video.
(用于 `video_event(视频事件)`_ 结构体中。它保存视频的分辨率和宽高比。)


-----


video_stream_source_t(视频流来源类型)
-------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef enum {
	VIDEO_SOURCE_DEMUX,
	VIDEO_SOURCE_MEMORY
    } video_stream_source_t;

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``VIDEO_SOURCE_DEMUX``

       -  :cspan:`1` Select the demux as the main source.(选择解复用器作为主来源。)

    -  ..

       -  ``VIDEO_SOURCE_MEMORY``

       -  If this source is selected, the stream
          comes from the user through the write
          system call.
          (选择此来源时，数据流来自用户，通过 write 系统调用提供。)

Description(描述)
~~~~~~~~~~~~~~~~~

The video stream source is set through the `VIDEO_SELECT_SOURCE(选择视频源)`_ call
and can take the following values, depending on whether we are replaying
from an internal (demuxer) or external (user write) source.
VIDEO_SOURCE_DEMUX selects the demultiplexer (fed either by the
frontend or the DVR device) as the source of the video stream. If
VIDEO_SOURCE_MEMORY is selected the stream comes from the application
through the `write()(写入视频数据)`_ system call.
(视频流来源通过 `VIDEO_SELECT_SOURCE(选择视频源)`_ 调用设置，根据是从内部（解复用器）还是外部（用户写入）来源回放，可取以下值。VIDEO_SOURCE_DEMUX 选择解复用器（由前端或 DVR 设备馈送）作为视频流的来源。若选择 VIDEO_SOURCE_MEMORY，则数据流来自应用程序，通过 `write()(写入视频数据)`_ 系统调用提供。)


-----


video_play_state_t(播放状态类型)
--------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    typedef enum {
	VIDEO_STOPPED,
	VIDEO_PLAYING,
	VIDEO_FREEZED
    } video_play_state_t;

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``VIDEO_STOPPED``

       -  Video is stopped.(视频已停止。)

    -  ..

       -  ``VIDEO_PLAYING``

       -  Video is currently playing.(视频正在播放。)

    -  ..

       -  ``VIDEO_FREEZED``

       -  Video is frozen.(视频已冻结。)

Description(描述)
~~~~~~~~~~~~~~~~~

This values can be returned by the `VIDEO_GET_STATUS(获取视频状态)`_ call
representing the state of video playback.
(这些值可由 `VIDEO_GET_STATUS(获取视频状态)`_ 调用返回，表示视频播放的状态。)


-----


struct video_command(视频命令结构体)
------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    struct video_command {
	__u32 cmd;
	__u32 flags;
	union {
	    struct {
		__u64 pts;
	    } stop;

	    struct {
		__s32 speed;
		__u32 format;
	    } play;

	    struct {
		__u32 data[16];
	    } raw;
	};
    };


Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``__u32 cmd``

       -  `Decoder command`_

    -  ..

       -  ``__u32 flags``

       -  Flags for the `Decoder command`_.(解码器命令的标志。)

    -  ..

       -  ``struct stop``

       -  ``__u64 pts``

       -  MPEG PTS(MPEG 显示时间戳。)

    -  ..

       -  :rspan:`5` ``stuct play``

       -  :rspan:`4` ``__s32 speed``

       -   0 or 1000 specifies normal speed,(0 或 1000 表示正常速度，)

    -  ..

       -   1:  specifies forward single stepping,(1 表示向前单步播放，)

    -  ..

       -   -1: specifies backward single stepping,(-1 表示向后单步播放，)

    -  ..

       -   >1: playback at speed / 1000 of the normal speed(大于 1 时，以正常速度的 speed/1000 倍播放，)

    -  ..

       -   <-1: reverse playback at ( -speed / 1000 ) of the normal speed.(小于 -1 时，以正常速度的 ( -speed / 1000 ) 反向播放。)

    -  ..

       -  ``__u32 format``

       -  `Play input formats`_

    -  ..

       -  ``__u32 data[16]``

       -  Reserved(保留。)

Description(描述)
~~~~~~~~~~~~~~~~~

The structure must be zeroed before use by the application. This ensures
it can be extended safely in the future.
(应用程序在使用前必须将该结构体清零。这可确保它在未来能够被安全地扩展。)


-----


Predefined decoder commands and flags(预定义解码器命令与标志)
-------------------------------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    #define VIDEO_CMD_PLAY                      (0)
    #define VIDEO_CMD_STOP                      (1)
    #define VIDEO_CMD_FREEZE                    (2)
    #define VIDEO_CMD_CONTINUE                  (3)

    #define VIDEO_CMD_FREEZE_TO_BLACK      (1 << 0)

    #define VIDEO_CMD_STOP_TO_BLACK        (1 << 0)
    #define VIDEO_CMD_STOP_IMMEDIATELY     (1 << 1)

    #define VIDEO_PLAY_FMT_NONE                 (0)
    #define VIDEO_PLAY_FMT_GOP                  (1)

    #define VIDEO_VSYNC_FIELD_UNKNOWN           (0)
    #define VIDEO_VSYNC_FIELD_ODD               (1)
    #define VIDEO_VSYNC_FIELD_EVEN              (2)
    #define VIDEO_VSYNC_FIELD_PROGRESSIVE       (3)

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  :rspan:`3` _`Decoder command`

       -  ``VIDEO_CMD_PLAY``

       -  Start playback.(开始播放。)

    -  ..

       -  ``VIDEO_CMD_STOP``

       -  Stop playback.(停止播放。)

    -  ..

       -  ``VIDEO_CMD_FREEZE``

       -  Freeze playback.(冻结播放。)

    -  ..

       -  ``VIDEO_CMD_CONTINUE``

       -  Continue playback after freeze.(冻结后继续播放。)

    -  ..

       -  Flags for ``VIDEO_CMD_FREEZE``

       -  ``VIDEO_CMD_FREEZE_TO_BLACK``

       -  Show black picture on freeze.(冻结时显示黑屏。)

    -  ..

       -  :rspan:`1` Flags for ``VIDEO_CMD_STOP``

       -  ``VIDEO_CMD_STOP_TO_BLACK``

       -  Show black picture on stop.(停止时显示黑屏。)

    -  ..

       -  ``VIDEO_CMD_STOP_IMMEDIATELY``

       -  Stop immediately, without emptying buffers.(立即停止，不清空缓冲区。)

    -  ..

       -  :rspan:`1` _`Play input formats`

       -  ``VIDEO_PLAY_FMT_NONE``

       -  The decoder has no special format requirements(解码器没有特殊的格式要求)

    -  ..

       -  ``VIDEO_PLAY_FMT_GOP``

       -  The decoder requires full GOPs(解码器要求完整的 GOP)

    -  ..

       -  :rspan:`3` Field order(场序)

       -  ``VIDEO_VSYNC_FIELD_UNKNOWN``

       -  FIELD_UNKNOWN can be used if the hardware does not know
          whether the Vsync is for an odd, even or progressive
          (i.e. non-interlaced) field.
          (若硬件不知道 Vsync 对应的是奇数场、偶数场还是逐行（即非隔行）场，可使用 FIELD_UNKNOWN。)

    -  ..

       -  ``VIDEO_VSYNC_FIELD_ODD``

       -  Vsync is for an odd field.(Vsync 对应奇数场。)

    -  ..

       -  ``VIDEO_VSYNC_FIELD_EVEN``

       -  Vsync is for an even field.(Vsync 对应偶数场。)

    -  ..

       -  ``VIDEO_VSYNC_FIELD_PROGRESSIVE``

       -  progressive (i.e. non-interlaced)(逐行（即非隔行）)


-----


video_event(视频事件)
---------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    struct video_event {
	__s32 type;
    #define VIDEO_EVENT_SIZE_CHANGED        1
    #define VIDEO_EVENT_FRAME_RATE_CHANGED  2
    #define VIDEO_EVENT_DECODER_STOPPED     3
    #define VIDEO_EVENT_VSYNC               4
	long timestamp;
	union {
	    video_size_t size;
	    unsigned int frame_rate;
	    unsigned char vsync_field;
	} u;
    };

Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  :rspan:`4` ``__s32 type``

       -  :cspan:`1` Event type.(事件类型。)

    -  ..

       -  ``VIDEO_EVENT_SIZE_CHANGED``

       -  Size changed.(尺寸改变。)

    -  ..

       -  ``VIDEO_EVENT_FRAME_RATE_CHANGED``

       -  Framerate changed.(帧率改变。)

    -  ..

       -  ``VIDEO_EVENT_DECODER_STOPPED``

       -  Decoder stopped.(解码器已停止。)

    -  ..

       -  ``VIDEO_EVENT_VSYNC``

       -  Vsync occurred.(发生了 Vsync。)

    -  ..

       -  ``long timestamp``

       -  :cspan:`1` MPEG PTS at occurrence.(事件发生时的 MPEG PTS。)

    -  ..

       -  :rspan:`2` ``union u``

       -  `video_size_t(视频尺寸类型)`_ size

       -  Resolution and aspect ratio of the video.(视频的分辨率和宽高比。)

    -  ..

       -  ``unsigned int frame_rate``

       -  in frames per 1000sec(单位为每 1000 秒的帧数)

    -  ..

       -  ``unsigned char vsync_field``

       -  | unknown / odd / even / progressive
          | See: `Predefined decoder commands and flags(预定义解码器命令与标志)`_
          | (未知／奇数／偶数／逐行。)

Description(描述)
~~~~~~~~~~~~~~~~~

This is the structure of a video event as it is returned by the
`VIDEO_GET_EVENT(获取视频事件)`_ call. See there for more details.
(这是 `VIDEO_GET_EVENT(获取视频事件)`_ 调用返回的视频事件结构体。详情参见该节。)


-----


video_status(视频状态)
----------------------

Synopsis(概要)
~~~~~~~~~~~~~~

The `VIDEO_GET_STATUS(获取视频状态)`_ call returns the following structure informing
about various states of the playback operation.
(`VIDEO_GET_STATUS(获取视频状态)`_ 调用返回以下结构体，告知播放操作的各种状态。)

.. code-block:: c

    struct video_status {
	int                    video_blank;
	video_play_state_t     play_state;
	video_stream_source_t  stream_source;
	video_format_t         video_format;
	video_displayformat_t  display_format;
    };

Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  :rspan:`2` ``int video_blank``

       -  :cspan:`1` Show blank video on freeze?(冻结时是否显示黑屏？)

    -  ..

       -  TRUE  ( != 0 )

       -  Blank screen when freeze.(冻结时黑屏。)

    -  ..

       -  FALSE ( == 0 )

       -  Show last decoded frame.(显示最后解码的一帧。)

    -  ..

       -  `video_play_state_t(播放状态类型)`_ ``play_state``

       -  Current state of playback.(当前播放状态。)

    -  ..

       -  `video_stream_source_t(视频流来源类型)`_ ``stream_source``

       -  Current source (demux/memory).(当前来源（解复用器/内存）。)

    -  ..

       -  `video_format_t(视频格式类型)`_ ``video_format``

       -  Current aspect ratio of stream.(流的当前宽高比。)

    -  ..

       -  `video_displayformat_t(显示格式类型)`_ ``display_format``

       -  Applied cropping mode.(所采用的裁剪模式。)

Description(描述)
~~~~~~~~~~~~~~~~~

If ``video_blank`` is set ``TRUE`` video will be blanked out if the
channel is changed or if playback is stopped. Otherwise, the last picture
will be displayed. ``play_state`` indicates if the video is currently
frozen, stopped, or being played back. The ``stream_source`` corresponds
to the selected source for the video stream. It can come either from the
demultiplexer or from memory. The ``video_format`` indicates the aspect
ratio (one of 4:3 or 16:9) of the currently played video stream.
Finally, ``display_format`` corresponds to the applied cropping mode in
case the source video format is not the same as the format of the output
device.
(若 ``video_blank`` 被设为 ``TRUE``，则切换频道或停止播放时视频将被抹黑；否则显示最后一幅画面。``play_state`` 指示视频当前是冻结、停止还是正在播放。``stream_source`` 对应视频流所选的来源，可来自解复用器或内存。``video_format`` 指示当前播放视频流的宽高比（4:3 或 16:9 之一）。最后，当源视频格式与输出设备的格式不一致时，``display_format`` 对应所采用的裁剪模式。)


-----


video_still_picture(视频静止图像)
---------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    struct video_still_picture {
    char *iFrame;
    int32_t size;
    };

Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``char *iFrame``

       -  Pointer to a single iframe in memory.(指向内存中单个 I 帧的指针。)

    -  ..

       -  ``int32_t size``

       -  Size of the iframe.(I 帧的大小。)


Description(描述)
~~~~~~~~~~~~~~~~~

An I-frame displayed via the `VIDEO_STILLPICTURE(显示静止图像)`_ call is passed on
within this structure.
(通过 `VIDEO_STILLPICTURE(显示静止图像)`_ 调用显示的 I 帧经该结构体传递。)


-----


video capabilities(视频能力)
----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    #define VIDEO_CAP_MPEG1   1
    #define VIDEO_CAP_MPEG2   2
    #define VIDEO_CAP_SYS     4
    #define VIDEO_CAP_PROG    8

Constants(常量)
~~~~~~~~~~~~~~~
Bit definitions for capabilities:(能力位的定义：)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``VIDEO_CAP_MPEG1``

       -  :cspan:`1` The hardware can decode MPEG1.(硬件可以解码 MPEG1。)

    -  ..

       -  ``VIDEO_CAP_MPEG2``

       -  The hardware can decode MPEG2.(硬件可以解码 MPEG2。)

    -  ..

       -  ``VIDEO_CAP_SYS``

       -  The video device accepts system stream.(视频设备接受系统流。)

          You still have to open the video and the audio device
          but only send the stream to the video device.
          (此时仍需打开视频和音频设备，但只需把数据流发送给视频设备即可。)

    -  ..

       -  ``VIDEO_CAP_PROG``

       -  The video device accepts program stream.(视频设备接受节目流。)

          You still have to open the video and the audio device
          but only send the stream to the video device.
          (此时仍需打开视频和音频设备，但只需把数据流发送给视频设备即可。)

Description(描述)
~~~~~~~~~~~~~~~~~

A call to `VIDEO_GET_CAPABILITIES(获取解码能力)`_ returns an unsigned integer with the
following bits set according to the hardware's capabilities.
(调用 `VIDEO_GET_CAPABILITIES(获取解码能力)`_ 会返回一个无符号整数，其中的位根据硬件能力按下列定义置位。)


-----


Video Function Calls(视频函数调用)
==================================


VIDEO_STOP(停止视频播放)
------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_STOP

.. code-block:: c

	int ioctl(fd, VIDEO_STOP, int mode)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  :cspan:`1` Equals ``VIDEO_STOP`` for this command.(此命令中等于 ``VIDEO_STOP``。)

    -  ..

       -  :rspan:`2` ``int mode``

       -  :cspan:`1` Indicates how the screen shall be handled.(指示屏幕应如何处理。)

    -  ..

       -  TRUE  ( != 0 )

       -  Blank screen when stop.(停止时黑屏。)

    -  ..

       -  FALSE ( == 0 )

       -  Show last decoded frame.(显示最后解码的一帧。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl is for Digital TV devices only. To control a V4L2 decoder use
the V4L2 :ref:`VIDIOC_DECODER_CMD` instead.
(该 ioctl 仅适用于数字电视设备。要控制 V4L2 解码器，请改用 V4L2 的 :ref:`VIDIOC_DECODER_CMD`。)

This ioctl call asks the Video Device to stop playing the current
stream. Depending on the input parameter, the screen can be blanked out
or displaying the last decoded frame.
(该 ioctl 调用请求视频设备停止播放当前数据流。根据输入参数的不同，屏幕可以被抹黑，也可以显示最后解码的一帧。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_PLAY(开始视频播放)
------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_PLAY

.. code-block:: c

	int ioctl(fd, VIDEO_PLAY)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_PLAY`` for this command.(此命令中等于 ``VIDEO_PLAY``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl is for Digital TV devices only. To control a V4L2 decoder use
the V4L2 :ref:`VIDIOC_DECODER_CMD` instead.
(该 ioctl 仅适用于数字电视设备。要控制 V4L2 解码器，请改用 V4L2 的 :ref:`VIDIOC_DECODER_CMD`。)

This ioctl call asks the Video Device to start playing a video stream
from the selected source.
(该 ioctl 调用请求视频设备从所选来源开始播放视频流。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_FREEZE(冻结视频播放)
--------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_FREEZE

.. code-block:: c

	int ioctl(fd, VIDEO_FREEZE)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_FREEZE`` for this command.(此命令中等于 ``VIDEO_FREEZE``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl is for Digital TV devices only. To control a V4L2 decoder use
the V4L2 :ref:`VIDIOC_DECODER_CMD` instead.
(该 ioctl 仅适用于数字电视设备。要控制 V4L2 解码器，请改用 V4L2 的 :ref:`VIDIOC_DECODER_CMD`。)

This ioctl call suspends the live video stream being played, if
VIDEO_SOURCE_DEMUX is selected. Decoding and playing are frozen.
It is then possible to restart the decoding and playing process of the
video stream using the `VIDEO_CONTINUE(继续视频播放)`_ command.
If VIDEO_SOURCE_MEMORY is selected in the ioctl call
`VIDEO_SELECT_SOURCE(选择视频源)`_, the Digital TV subsystem will not decode any more
data until the ioctl call `VIDEO_CONTINUE(继续视频播放)`_ or `VIDEO_PLAY(开始视频播放)`_ is performed.
(若选择了 VIDEO_SOURCE_DEMUX，该 ioctl 调用会挂起正在播放的实况视频流，解码和播放被冻结。之后可以使用 `VIDEO_CONTINUE(继续视频播放)`_ 命令重新启动视频流的解码和播放过程。如果在 ioctl 调用 `VIDEO_SELECT_SOURCE(选择视频源)`_ 中选择了 VIDEO_SOURCE_MEMORY，那么在执行 `VIDEO_CONTINUE(继续视频播放)`_ 或 `VIDEO_PLAY(开始视频播放)`_ 调用之前，数字电视子系统将不再解码任何数据。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_CONTINUE(继续视频播放)
----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_CONTINUE

.. code-block:: c

	int ioctl(fd, VIDEO_CONTINUE)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_CONTINUE`` for this command.(此命令中等于 ``VIDEO_CONTINUE``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl is for Digital TV devices only. To control a V4L2 decoder use
the V4L2 :ref:`VIDIOC_DECODER_CMD` instead.
(该 ioctl 仅适用于数字电视设备。要控制 V4L2 解码器，请改用 V4L2 的 :ref:`VIDIOC_DECODER_CMD`。)

This ioctl call restarts decoding and playing processes of the video
stream which was played before a call to `VIDEO_FREEZE(冻结视频播放)`_ was made.
(该 ioctl 调用重新启动视频流的解码和播放过程，恢复执行 `VIDEO_FREEZE(冻结视频播放)`_ 调用之前的播放。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_SELECT_SOURCE(选择视频源)
-------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_SELECT_SOURCE

.. code-block:: c

	int ioctl(fd, VIDEO_SELECT_SOURCE, video_stream_source_t source)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_SELECT_SOURCE`` for this command.(此命令中等于 ``VIDEO_SELECT_SOURCE``。)

    -  ..

       -  `video_stream_source_t(视频流来源类型)`_ ``source``

       -  Indicates which source shall be used for the Video stream.(指示视频流应使用哪个来源。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl is for Digital TV devices only. This ioctl was also supported
by the V4L2 ivtv driver, but that has been replaced by the ivtv-specific
``IVTV_IOC_PASSTHROUGH_MODE`` ioctl.
(该 ioctl 仅适用于数字电视设备。V4L2 的 ivtv 驱动也曾支持此 ioctl，但它已被 ivtv 专用的 ``IVTV_IOC_PASSTHROUGH_MODE`` ioctl 取代。)

This ioctl call informs the video device which source shall be used for
the input data. The possible sources are demux or memory. If memory is
selected, the data is fed to the video device through the write command
using the struct `video_stream_source_t(视频流来源类型)`_. If demux is selected, the data
is directly transferred from the onboard demux-device to the decoder.
(该 ioctl 调用告知视频设备输入数据应使用哪个来源。可能的来源是解复用器 (demux) 或内存 (memory)。若选择内存，数据通过 write 命令、以 `video_stream_source_t(视频流来源类型)`_ 结构体的形式馈送给视频设备。若选择解复用器，数据直接从板载解复用设备传输到解码器。)

The data fed to the decoder is also controlled by the PID-filter.
Output selection: :c:type:`dmx_output` ``DMX_OUT_DECODER``.
(馈送给解码器的数据还受 PID 过滤器控制。输出选择：:c:type:`dmx_output` ``DMX_OUT_DECODER``。)


Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_SET_BLANK(设置黑屏)
-------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_SET_BLANK

.. code-block:: c

	int ioctl(fd, VIDEO_SET_BLANK, int mode)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  :cspan:`1` Equals ``VIDEO_SET_BLANK`` for this command.(此命令中等于 ``VIDEO_SET_BLANK``。)

    -  ..

       -  :rspan:`2` ``int mode``

       -  :cspan:`1` Indicates if the screen shall be blanked.(指示是否应将屏幕抹黑。)

    -  ..

       -  TRUE  ( != 0 )

       -  Blank screen when stop.(停止时黑屏。)

    -  ..

       -  FALSE ( == 0 )

       -  Show last decoded frame.(显示最后解码的一帧。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Video Device to blank out the picture.
(该 ioctl 调用请求视频设备将画面抹黑。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_GET_STATUS(获取视频状态)
------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_GET_STATUS

.. code-block:: c

	int ioctl(fd, int request = VIDEO_GET_STATUS,
	struct video_status *status)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_GET_STATUS`` for this command.(此命令中等于 ``VIDEO_GET_STATUS``。)

    -  ..

       -  ``struct`` `video_status(视频状态)`_ ``*status``

       -  Returns the current status of the Video Device.(返回视频设备的当前状态。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Video Device to return the current status of
the device.
(该 ioctl 调用请求视频设备返回设备的当前状态。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_GET_EVENT(获取视频事件)
-----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_GET_EVENT

.. code-block:: c

	int ioctl(fd, int request = VIDEO_GET_EVENT,
	struct video_event *ev)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_GET_EVENT`` for this command.(此命令中等于 ``VIDEO_GET_EVENT``。)

    -  ..

       -  ``struct`` `video_event(视频事件)`_ ``*ev``

       -  Points to the location where the event, if any, is to be stored.(指向存放事件（如果有）的位置。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl is for DVB devices only. To get events from a V4L2 decoder
use the V4L2 :ref:`VIDIOC_DQEVENT` ioctl instead.
(该 ioctl 仅适用于 DVB 设备。要从 V4L2 解码器获取事件，请改用 V4L2 的 :ref:`VIDIOC_DQEVENT` ioctl。)

This ioctl call returns an event of type `video_event(视频事件)`_ if available. A
certain number of the latest events will be cued and returned in order of
occurrence. Older events may be discarded if not fetched in time. If
an event is not available, the behavior depends on whether the device is
in blocking or non-blocking mode. In the latter case, the call fails
immediately with errno set to ``EWOULDBLOCK``. In the former case, the
call blocks until an event becomes available. The standard Linux poll()
and/or select() system calls can be used with the device file descriptor
to watch for new events. For select(), the file descriptor should be
included in the exceptfds argument, and for poll(), POLLPRI should be
specified as the wake-up condition. Read-only permissions are sufficient
for this ioctl call.
(若有 `video_event(视频事件)`_ 类型的事件可用，该 ioctl 调用即返回之。一定数量的最新事件会排队并按发生顺序返回；若未及时取走，较早的事件可能被丢弃。若无事件可用，其行为取决于设备处于阻塞还是非阻塞模式：后者立即失败并将 errno 设为 ``EWOULDBLOCK``；前者则阻塞，直到有事件可用为止。可以使用标准的 Linux poll() 和/或 select() 系统调用配合设备文件描述符来监视新事件。对 select()，文件描述符应加入 exceptfds 参数；对 poll()，应将 POLLPRI 指定为唤醒条件。该 ioctl 调用只需只读权限。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EWOULDBLOCK``

       -  :cspan:`1` There is no event pending, and the device is in
          non-blocking mode.
          (当前没有待处理的事件，且设备处于非阻塞模式。)

    -  ..

       -  ``EOVERFLOW``

       -  Overflow in event queue - one or more events were lost.(事件队列溢出——一个或多个事件丢失。)


-----


VIDEO_SET_DISPLAY_FORMAT(设置显示格式)
--------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_SET_DISPLAY_FORMAT

.. code-block:: c

	int ioctl(fd, int request = VIDEO_SET_DISPLAY_FORMAT,
	video_display_format_t format)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_SET_DISPLAY_FORMAT`` for this command.(此命令中等于 ``VIDEO_SET_DISPLAY_FORMAT``。)

    -  ..

       -  `video_displayformat_t(显示格式类型)`_ ``format``

       -  Selects the video format to be used.(选择要使用的视频格式。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Video Device to select the video format to be
applied by the MPEG chip on the video.
(该 ioctl 调用请求视频设备选择由 MPEG 芯片对视频采用的显示格式。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_STILLPICTURE(显示静止图像)
--------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_STILLPICTURE

.. code-block:: c

	int ioctl(fd, int request = VIDEO_STILLPICTURE,
	struct video_still_picture *sp)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_STILLPICTURE`` for this command.(此命令中等于 ``VIDEO_STILLPICTURE``。)

    -  ..

       -  ``struct`` `video_still_picture(视频静止图像)`_ ``*sp``

       -  Pointer to the location where the struct with the I-frame
          and size is stored.
          (指向存放含 I 帧及其大小的结构体的位置。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Video Device to display a still picture
(I-frame). The input data shall be the section of an elementary video
stream containing an I-frame. Typically this section is extracted from a
TS or PES recording. Resolution and codec (see `video capabilities(视频能力)`_) must
be supported by the device. If the pointer is NULL, then the current
displayed still picture is blanked.
(该 ioctl 调用请求视频设备显示一幅静止图像（I 帧）。输入数据应是包含 I 帧的原始视频流 (elementary video stream) 片段，通常从 TS 或 PES 录制中提取。设备必须支持相应的分辨率和编解码器（参见 `video capabilities(视频能力)`_ ）。若指针为 NULL，则当前显示的静止图像被抹掉。)

e.g. The AV7110 supports MPEG1 and MPEG2 with the common PAL-SD
resolutions.
(例如，AV7110 支持 MPEG1 和 MPEG2 以及常见的 PAL-SD 分辨率。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_FAST_FORWARD(快速前进)
----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_FAST_FORWARD

.. code-block:: c

	int ioctl(fd, int request = VIDEO_FAST_FORWARD, int nFrames)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_FAST_FORWARD`` for this command.(此命令中等于 ``VIDEO_FAST_FORWARD``。)

    -  ..

       -  ``int nFrames``

       -  The number of frames to skip.(要跳过的帧数。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Video Device to skip decoding of N number of
I-frames. This call can only be used if ``VIDEO_SOURCE_MEMORY`` is
selected.
(该 ioctl 调用请求视频设备跳过 N 个 I 帧的解码。仅在选择 ``VIDEO_SOURCE_MEMORY`` 时才能使用此调用。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EPERM``

       -  Mode ``VIDEO_SOURCE_MEMORY`` not selected.(未选择 ``VIDEO_SOURCE_MEMORY`` 模式。)


-----


VIDEO_SLOWMOTION(慢动作播放)
----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_SLOWMOTION

.. code-block:: c

	int ioctl(fd, int request = VIDEO_SLOWMOTION, int nFrames)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_SLOWMOTION`` for this command.(此命令中等于 ``VIDEO_SLOWMOTION``。)

    -  ..

       -  ``int nFrames``

       -  The number of times to repeat each frame.(每帧重复的次数。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the video device to repeat decoding frames N number
of times. This call can only be used if ``VIDEO_SOURCE_MEMORY`` is
selected.
(该 ioctl 调用请求视频设备将解码出的帧重复播放 N 次。仅在选择 ``VIDEO_SOURCE_MEMORY`` 时才能使用此调用。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EPERM``

       -  Mode ``VIDEO_SOURCE_MEMORY`` not selected.(未选择 ``VIDEO_SOURCE_MEMORY`` 模式。)


-----


VIDEO_GET_CAPABILITIES(获取解码能力)
------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_GET_CAPABILITIES

.. code-block:: c

	int ioctl(fd, int request = VIDEO_GET_CAPABILITIES, unsigned int *cap)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_GET_CAPABILITIES`` for this command.(此命令中等于 ``VIDEO_GET_CAPABILITIES``。)

    -  ..

       -  ``unsigned int *cap``

       -  Pointer to a location where to store the capability information.(指向存储能力信息的位置。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the video device about its decoding capabilities.
On success it returns an integer which has bits set according to the
defines in `video capabilities(视频能力)`_.
(该 ioctl 调用查询视频设备的解码能力。成功时返回一个整数，其中的位按 `video capabilities(视频能力)`_ 一节中的定义置位。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_CLEAR_BUFFER(清空视频缓冲区)
----------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_CLEAR_BUFFER

.. code-block:: c

	int ioctl(fd, int request = VIDEO_CLEAR_BUFFER)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_CLEAR_BUFFER`` for this command.(此命令中等于 ``VIDEO_CLEAR_BUFFER``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call clears all video buffers in the driver and in the
decoder hardware.
(该 ioctl 调用清空驱动程序和解码器硬件中的所有视频缓冲区。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_SET_STREAMTYPE(设置流类型)
--------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_SET_STREAMTYPE

.. code-block:: c

	int ioctl(fd, int request = VIDEO_SET_STREAMTYPE, int type)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_SET_STREAMTYPE`` for this command.(此命令中等于 ``VIDEO_SET_STREAMTYPE``。)

    -  ..

       -  ``int type``

       -  Stream type.(流类型。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl tells the driver which kind of stream to expect being written
to it.
Intelligent decoder might also not support or ignore (like the AV7110)
this call and determine the stream type themselves.
(该 ioctl 告知驱动程序将要写入的数据流是什么类型。智能解码器也可能不支持或忽略（如 AV7110）此调用，而自行判断流类型。)

Currently used stream types:(当前使用的流类型：)

.. flat-table::
    :header-rows:  1
    :stub-columns: 0

    -  ..

       -  Codec(编解码器)

       -  Stream type(流类型)

    -  ..

       -  MPEG2

       -  0

    -  ..

       -  MPEG4 h.264

       -  1

    -  ..

       -  VC1

       -  3

    -  ..

       -  MPEG4 Part2

       -  4

    -  ..

       -  VC1 SM

       -  5

    -  ..

       -  MPEG1

       -  6

    -  ..

       -  HEVC h.265

       -  | 7
          | DREAMBOX: 22

    -  ..

       -  AVS

       -  16

    -  ..

       -  AVS2

       -  40

Not every decoder supports all stream types.(并非所有解码器都支持全部流类型。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_SET_FORMAT(设置屏幕格式)
------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_SET_FORMAT

.. code-block:: c

	int ioctl(fd, int request = VIDEO_SET_FORMAT, video_format_t format)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_SET_FORMAT`` for this command.(此命令中等于 ``VIDEO_SET_FORMAT``。)

    -  ..

       -  `video_format_t(视频格式类型)`_ ``format``

       -  Video format of TV as defined in section `video_format_t(视频格式类型)`_.(电视的视频格式，取值见 `video_format_t(视频格式类型)`_ 一节。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl sets the screen format (aspect ratio) of the connected output
device (TV) so that the output of the decoder can be adjusted
accordingly.
(该 ioctl 设置所连接输出设备（电视）的屏幕格式（宽高比），使解码器的输出能够据此调整。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_GET_SIZE(获取视频尺寸)
----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_GET_SIZE

.. code-block:: c

	int ioctl(int fd, int request = VIDEO_GET_SIZE, video_size_t *size)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call,
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_GET_SIZE`` for this command.(此命令中等于 ``VIDEO_GET_SIZE``。)

    -  ..

       -  `video_size_t(视频尺寸类型)`_ ``*size``

       -  Returns the size and aspect ratio.(返回尺寸和宽高比。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl returns the size and aspect ratio.
(该 ioctl 返回尺寸和宽高比。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_GET_PTS(获取 PTS 时间戳)
------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_GET_PTS

.. code-block:: c

	int ioctl(int fd, int request = VIDEO_GET_PTS, __u64 *pts)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_GET_PTS`` for this command.(此命令中等于 ``VIDEO_GET_PTS``。)

    -  ..

       -  ``__u64 *pts``

       -  Returns the 33-bit timestamp as defined in ITU T-REC-H.222.0 /
          ISO/IEC 13818-1.
          (返回 ITU T-REC-H.222.0 / ISO/IEC 13818-1 中定义的 33 位时间戳。)

          The PTS should belong to the currently played frame if possible,
          but may also be a value close to it like the PTS of the last
          decoded frame or the last PTS extracted by the PES parser.
          (PTS 应尽可能属于当前正在播放的帧，但也可能是接近它的值，例如最后解码一帧的 PTS 或 PES 解析器提取的最后一个 PTS。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

For V4L2 decoders this ioctl has been replaced by the
``V4L2_CID_MPEG_VIDEO_DEC_PTS`` control.
(对 V4L2 解码器而言，此 ioctl 已被 ``V4L2_CID_MPEG_VIDEO_DEC_PTS`` 控件取代。)

This ioctl call asks the Video Device to return the current PTS
timestamp.
(该 ioctl 调用请求视频设备返回当前 PTS 时间戳。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_GET_FRAME_COUNT(获取帧计数)
---------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_GET_FRAME_COUNT

.. code-block:: c

	int ioctl(int fd, VIDEO_GET_FRAME_COUNT, __u64 *pts)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_GET_FRAME_COUNT`` for this command.(此命令中等于 ``VIDEO_GET_FRAME_COUNT``。)

    -  ..

       -  ``__u64 *pts``

       -  Returns the number of frames displayed since the decoder was
          started.
          (返回自解码器启动以来已显示的帧数。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

For V4L2 decoders this ioctl has been replaced by the
``V4L2_CID_MPEG_VIDEO_DEC_FRAME`` control.
(对 V4L2 解码器而言，此 ioctl 已被 ``V4L2_CID_MPEG_VIDEO_DEC_FRAME`` 控件取代。)

This ioctl call asks the Video Device to return the number of displayed
frames since the decoder was started.
(该 ioctl 调用请求视频设备返回自解码器启动以来已显示的帧数。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_COMMAND(发送解码器命令)
-----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_COMMAND

.. code-block:: c

	int ioctl(int fd, int request = VIDEO_COMMAND,
	struct video_command *cmd)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_COMMAND`` for this command.(此命令中等于 ``VIDEO_COMMAND``。)

    -  ..

       -  `struct video_command(视频命令结构体)`_ ``*cmd``

       -  Commands the decoder.(向解码器下达命令。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

For V4L2 decoders this ioctl has been replaced by the
:ref:`VIDIOC_DECODER_CMD` ioctl.
(对 V4L2 解码器而言，此 ioctl 已被 :ref:`VIDIOC_DECODER_CMD` ioctl 取代。)

This ioctl commands the decoder. The `struct video_command(视频命令结构体)`_ is a
subset of the ``v4l2_decoder_cmd`` struct, so refer to the
:ref:`VIDIOC_DECODER_CMD` documentation for
more information.
(该 ioctl 向解码器下达命令。`struct video_command(视频命令结构体)`_ 是 ``v4l2_decoder_cmd`` 结构体的子集，更多信息请参阅 :ref:`VIDIOC_DECODER_CMD` 文档。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


VIDEO_TRY_COMMAND(试探解码器命令)
---------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: VIDEO_TRY_COMMAND

.. code-block:: c

	int ioctl(int fd, int request = VIDEO_TRY_COMMAND,
	struct video_command *cmd)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``VIDEO_TRY_COMMAND`` for this command.(此命令中等于 ``VIDEO_TRY_COMMAND``。)

    -  ..

       -  `struct video_command(视频命令结构体)`_ ``*cmd``

       -  Try a decoder command.(试用的解码器命令。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

For V4L2 decoders this ioctl has been replaced by the
:ref:`VIDIOC_TRY_DECODER_CMD <VIDIOC_DECODER_CMD>` ioctl.
(对 V4L2 解码器而言，此 ioctl 已被 :ref:`VIDIOC_TRY_DECODER_CMD <VIDIOC_DECODER_CMD>` ioctl 取代。)

This ioctl tries a decoder command. The `struct video_command(视频命令结构体)`_ is a
subset of the ``v4l2_decoder_cmd`` struct, so refer to the
:ref:`VIDIOC_TRY_DECODER_CMD <VIDIOC_DECODER_CMD>` documentation
for more information.
(该 ioctl 试探一条解码器命令。`struct video_command(视频命令结构体)`_ 是 ``v4l2_decoder_cmd`` 结构体的子集，更多信息请参阅 :ref:`VIDIOC_TRY_DECODER_CMD <VIDIOC_DECODER_CMD>` 文档。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


open()(打开视频设备)
--------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    #include <fcntl.h>

.. c:function:: 	int open(const char *deviceName, int flags)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``const char *deviceName``

       -  Name of specific video device.(要打开的具体视频设备名。)

    -  ..

       -  :rspan:`3` ``int flags``

       -  :cspan:`1` A bit-wise OR of the following flags:(以下标志的按位或：)

    -  ..

       -  ``O_RDONLY``

       -  read-only access(只读访问)

    -  ..

       -  ``O_RDWR``

       -  read/write access(读/写访问)

    -  ..

       -  ``O_NONBLOCK``
       -  | Open in non-blocking mode
          | (blocking mode is the default)
          | (以非阻塞模式打开（默认为阻塞模式）)

Description(描述)
~~~~~~~~~~~~~~~~~

This system call opens a named video device (e.g.
/dev/dvb/adapter?/video?) for subsequent use.
(该系统调用打开一个命名的视频设备（例如 /dev/dvb/adapter?/video?）以供后续使用。)

When an open() call has succeeded, the device will be ready for use. The
significance of blocking or non-blocking mode is described in the
documentation for functions where there is a difference. It does not
affect the semantics of the open() call itself. A device opened in
blocking mode can later be put into non-blocking mode (and vice versa)
using the F_SETFL command of the fcntl system call. This is a standard
system call, documented in the Linux manual page for fcntl. Only one
user can open the Video Device in O_RDWR mode. All other attempts to
open the device in this mode will fail, and an error-code will be
returned. If the Video Device is opened in O_RDONLY mode, the only
ioctl call that can be used is `VIDEO_GET_STATUS(获取视频状态)`_. All other call will
return an error code.
(open() 调用成功后，设备即可使用。阻塞或非阻塞模式的意义在存在差异的各函数的文档中说明；它不影响 open() 调用本身的语义。以阻塞模式打开的设备之后可以用 fcntl 系统调用的 F_SETFL 命令切换为非阻塞模式（反之亦然）。这是一个标准系统调用，在 Linux 的 fcntl 手册页中有说明。只允许一个用户以 O_RDWR 模式打开视频设备；其他在该模式下的打开尝试都会失败并返回错误码。若视频设备以 O_RDONLY 模式打开，则唯一可用的 ioctl 调用是 `VIDEO_GET_STATUS(获取视频状态)`_ ，其他调用都会返回错误码。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``ENODEV``

       -  :cspan:`1` Device driver not loaded/available.(设备驱动未加载/不可用。)

    -  ..

       -  ``EINTERNAL``

       -  Internal error.(内部错误。)

    -  ..

       -  ``EBUSY``

       -  Device or resource busy.(设备或资源忙。)

    -  ..

       -  ``EINVAL``

       -  Invalid argument.(无效参数。)


-----


close()(关闭视频设备)
---------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:function:: 	int close(int fd)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

Description(描述)
~~~~~~~~~~~~~~~~~

This system call closes a previously opened video device.
(该系统调用关闭之前打开的视频设备。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EBADF``

       -  fd is not a valid open file descriptor.(fd 不是有效的已打开文件描述符。)


-----


write()(写入视频数据)
---------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:function:: size_t write(int fd, const void *buf, size_t count)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开视频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``void *buf``

       -  Pointer to the buffer containing the PES data.(指向包含 PES 数据的缓冲区的指针。)

    -  ..

       -  ``size_t count``

       -  Size of buf.(buf 的大小。)

Description(描述)
~~~~~~~~~~~~~~~~~

This system call can only be used if VIDEO_SOURCE_MEMORY is selected
in the ioctl call `VIDEO_SELECT_SOURCE(选择视频源)`_. The data provided shall be in
PES format, unless the capability allows other formats. TS is the
most common format for storing DVB-data, it is usually supported too.
If O_NONBLOCK is not specified the function will block until buffer space
is available. The amount of data to be transferred is implied by count.
(只有在 ioctl 调用 `VIDEO_SELECT_SOURCE(选择视频源)`_ 中选择了 VIDEO_SOURCE_MEMORY，才能使用该系统调用。提供的数据应为 PES 格式，除非能力允许其他格式。TS 是存储 DVB 数据最常见的格式，通常也受支持。若未指定 O_NONBLOCK，该函数将阻塞直到有缓冲区空间可用。要传输的数据量由 count 隐含给出。)

.. note:: See: :ref:`DVB Data Formats <legacy_dvb_decoder_formats>`
          (参见 :ref:`DVB Data Formats <legacy_dvb_decoder_formats>`。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EPERM``

       -  :cspan:`1` Mode ``VIDEO_SOURCE_MEMORY`` not selected.(未选择 ``VIDEO_SOURCE_MEMORY`` 模式。)

    -  ..

       -  ``ENOMEM``

       -  Attempted to write more data than the internal buffer can hold.(试图写入的数据量超过了内部缓冲区的容量。)

    -  ..

       -  ``EBADF``

       -  fd is not a valid open file descriptor.(fd 不是有效的已打开文件描述符。)
