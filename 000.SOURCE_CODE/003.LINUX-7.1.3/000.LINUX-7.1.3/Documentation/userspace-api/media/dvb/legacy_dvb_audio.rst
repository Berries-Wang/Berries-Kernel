.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later OR GPL-2.0

.. c:namespace:: dtv.legacy.audio

.. _dvb_audio:

==============================
DVB Audio Device(DVB 音频设备)
==============================

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

The DVB audio device controls the MPEG2 audio decoder of the DVB
hardware. It can be accessed through ``/dev/dvb/adapter?/audio?``. Data
types and ioctl definitions can be accessed by including
``linux/dvb/audio.h`` in your application.
(DVB 音频设备控制 DVB 硬件中的 MPEG2 音频解码器。可以通过 ``/dev/dvb/adapter?/audio?`` 访问该设备。在应用程序中包含 ``linux/dvb/audio.h`` 头文件，即可获得相关数据类型和 ioctl 定义。)

Please note that most DVB cards don’t have their own MPEG decoder, which
results in the omission of the audio and video device.
(请注意，大多数 DVB 卡没有自己的 MPEG 解码器，因此也就没有音频和视频设备。)

These ioctls were also used by V4L2 to control MPEG decoders implemented
in V4L2. The use of these ioctls for that purpose has been made obsolete
and proper V4L2 ioctls or controls have been created to replace that
functionality. Use :ref:`V4L2 ioctls<audio>` for new drivers!
(这些 ioctl 也曾被 V4L2 用于控制 V4L2 中实现的 MPEG 解码器。这种用途如今已被废弃，V4L2 已经创建了相应的 ioctl 或控件来取代该功能。新驱动请使用 :ref:`V4L2 ioctls<audio>`！)


Audio Data Types(音频数据类型)
==============================

This section describes the structures, data types and defines used when
talking to the audio device.
(本节描述与音频设备通信时所用的结构体、数据类型和定义。)


-----


audio_stream_source_t(音频流来源类型)
-------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:enum:: audio_stream_source_t

.. code-block:: c

    typedef enum {
    AUDIO_SOURCE_DEMUX,
    AUDIO_SOURCE_MEMORY
    } audio_stream_source_t;

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``AUDIO_SOURCE_DEMUX``

       -  :cspan:`1` Selects the demultiplexer (fed either by the frontend
          or the DVR device) as the source of the video stream.
          (选择解复用器（由前端或 DVR 设备馈送）作为流的来源。)

    -  ..

       -  ``AUDIO_SOURCE_MEMORY``

       -  Selects the stream from the application that comes through
          the `write()(写入音频数据)`_ system call.
          (选择来自应用程序、通过 write 系统调用提供的数据流。)

Description(描述)
~~~~~~~~~~~~~~~~~

The audio stream source is set through the `AUDIO_SELECT_SOURCE(选择音频源)`_ call
and can take the following values, depending on whether we are replaying
from an internal (demux) or external (user write) source.
(音频流来源通过 `AUDIO_SELECT_SOURCE(选择音频源)`_ 调用设置，根据是从内部（解复用器）还是外部（用户写入）来源回放，可取以下值。)

The data fed to the decoder is also controlled by the PID-filter.
Output selection: :c:type:`dmx_output` ``DMX_OUT_DECODER``.
(馈送给解码器的数据还受 PID 过滤器控制。输出选择：:c:type:`dmx_output` ``DMX_OUT_DECODER``。)


-----


audio_play_state_t(播放状态类型)
--------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:enum:: audio_play_state_t

.. code-block:: c

    typedef enum {
	AUDIO_STOPPED,
	AUDIO_PLAYING,
	AUDIO_PAUSED
    } audio_play_state_t;

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``AUDIO_STOPPED``

       -  Audio is stopped.(音频已停止。)

    -  ..

       -  ``AUDIO_PLAYING``

       -  Audio is currently playing.(音频正在播放。)

    -  ..

       -  ``AUDIO_PAUSE``

       -  Audio is frozen.(音频已冻结。)

Description(描述)
~~~~~~~~~~~~~~~~~

This values can be returned by the `AUDIO_GET_STATUS(获取音频状态)`_ call
representing the state of audio playback.
(这些值可由 `AUDIO_GET_STATUS(获取音频状态)`_ 调用返回，表示音频播放的状态。)


-----


audio_channel_select_t(声道选择类型)
------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:enum:: audio_channel_select_t

.. code-block:: c

    typedef enum {
	AUDIO_STEREO,
	AUDIO_MONO_LEFT,
	AUDIO_MONO_RIGHT,
	AUDIO_MONO,
	AUDIO_STEREO_SWAPPED
    } audio_channel_select_t;

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``AUDIO_STEREO``

       -  Stereo.(立体声。)

    -  ..

       -  ``AUDIO_MONO_LEFT``

       -  Mono, select left stereo channel as source.(单声道，选择左立体声声道作为来源。)

    -  ..

       -  ``AUDIO_MONO_RIGHT``

       -  Mono, select right stereo channel as source.(单声道，选择右立体声声道作为来源。)

    -  ..

       -  ``AUDIO_MONO``

       -  Mono source only.(仅单声道来源。)

    -  ..

       -  ``AUDIO_STEREO_SWAPPED``

       -  Stereo, swap L & R.(立体声，交换左右声道。)

Description(描述)
~~~~~~~~~~~~~~~~~

The audio channel selected via `AUDIO_CHANNEL_SELECT(声道选择)`_ is determined by
this value.
(通过 `AUDIO_CHANNEL_SELECT(声道选择)`_ 选择的音频声道由此值决定。)


-----


audio_mixer_t(混音器设置)
-------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:struct:: audio_mixer

.. code-block:: c

    typedef struct audio_mixer {
	unsigned int volume_left;
	unsigned int volume_right;
    } audio_mixer_t;

Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``unsigned int volume_left``

       -  Volume left channel.
          Valid range: 0 ... 255
          (左声道音量。有效范围：0 ... 255)

    -  ..

       -  ``unsigned int volume_right``

       -  Volume right channel.
          Valid range: 0 ... 255
          (右声道音量。有效范围：0 ... 255)

Description(描述)
~~~~~~~~~~~~~~~~~

This structure is used by the `AUDIO_SET_MIXER(设置混音器)`_ call to set the
audio volume.
(`AUDIO_SET_MIXER(设置混音器)`_ 调用使用该结构体来设置音频音量。)


-----


audio_status(音频状态)
----------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:struct:: audio_status

.. code-block:: c

    typedef struct audio_status {
	int AV_sync_state;
	int mute_state;
	audio_play_state_t play_state;
	audio_stream_source_t stream_source;
	audio_channel_select_t channel_select;
	int bypass_mode;
	audio_mixer_t mixer_state;
    } audio_status_t;

Variables(变量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  :rspan:`2` ``int AV_sync_state``

       -  :cspan:`1` Shows if A/V synchronization is ON or OFF.(显示 A/V 同步是开启还是关闭。)

    -  ..

       -  TRUE  ( != 0 )

       -  AV-sync ON.(A/V 同步开启。)

    -  ..

       -  FALSE ( == 0 )

       -  AV-sync OFF.(A/V 同步关闭。)

    -  ..

       -  :rspan:`2` ``int mute_state``

       -  :cspan:`1` Indicates if audio is muted or not.(指示音频是否静音。)

    -  ..

       -  TRUE  ( != 0 )

       -  mute audio(静音)

    -  ..

       -  FALSE ( == 0 )

       -  unmute audio(取消静音)

    -  ..

       -  `audio_play_state_t(播放状态类型)`_ ``play_state``

       -  Current playback state.(当前播放状态。)

    -  ..

       -  `audio_stream_source_t(音频流来源类型)`_ ``stream_source``

       -  Current source of the data.(数据的当前来源。)

    -  ..

       -  :rspan:`2` ``int bypass_mode``

       -  :cspan:`1` Is the decoding of the current Audio stream in
          the DVB subsystem enabled or disabled.
          (DVB 子系统对当前音频流的解码是启用还是禁用。)

    -  ..

       -  TRUE  ( != 0 )

       -  Bypass disabled.(直通禁用。)

    -  ..

       -  FALSE ( == 0 )

       -  Bypass enabled.(直通启用。)

    -  ..

       -  `audio_mixer_t(混音器设置)`_ ``mixer_state``

       -  Current volume settings.(当前音量设置。)

Description(描述)
~~~~~~~~~~~~~~~~~

The `AUDIO_GET_STATUS(获取音频状态)`_ call returns this structure as information
about various states of the playback operation.
(`AUDIO_GET_STATUS(获取音频状态)`_ 调用返回该结构体，作为播放操作各种状态的信息。)


-----


audio encodings(音频编码)
-------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

     #define AUDIO_CAP_DTS    1
     #define AUDIO_CAP_LPCM   2
     #define AUDIO_CAP_MP1    4
     #define AUDIO_CAP_MP2    8
     #define AUDIO_CAP_MP3   16
     #define AUDIO_CAP_AAC   32
     #define AUDIO_CAP_OGG   64
     #define AUDIO_CAP_SDDS 128
     #define AUDIO_CAP_AC3  256

Constants(常量)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``AUDIO_CAP_DTS``

       -  :cspan:`1` The hardware accepts DTS audio tracks.(硬件接受 DTS 音轨。)

    -  ..

       -  ``AUDIO_CAP_LPCM``

       -   The hardware accepts uncompressed audio with
           Linear Pulse-Code Modulation (LPCM)
           (硬件接受线性脉冲编码调制 (LPCM) 的未压缩音频。)

    -  ..

       -  ``AUDIO_CAP_MP1``

       -  The hardware accepts MPEG-1 Audio Layer 1.(硬件接受 MPEG-1 音频层 1。)

    -  ..

       -  ``AUDIO_CAP_MP2``

       -  The hardware accepts MPEG-1 Audio Layer 2.
          Also known as MUSICAM.
          (硬件接受 MPEG-1 音频层 2。也称为 MUSICAM。)

    -  ..

       -  ``AUDIO_CAP_MP3``

       -  The hardware accepts MPEG-1 Audio Layer III.
          Commonly known as .mp3.
          (硬件接受 MPEG-1 音频层 III。通常称为 .mp3。)

    -  ..

       -  ``AUDIO_CAP_AAC``

       -  The hardware accepts AAC (Advanced Audio Coding).(硬件接受 AAC（高级音频编码）。)

    -  ..

       -  ``AUDIO_CAP_OGG``

       -  The hardware accepts Vorbis audio tracks.(硬件接受 Vorbis 音轨。)

    -  ..

       -  ``AUDIO_CAP_SDDS``

       -  The hardware accepts Sony Dynamic Digital Sound (SDDS).(硬件接受索尼动态数字声 (SDDS)。)

    -  ..

       -  ``AUDIO_CAP_AC3``

       -  The hardware accepts Dolby Digital ATSC A/52 audio.
          Also known as AC-3.
          (硬件接受杜比数字 ATSC A/52 音频。也称为 AC-3。)

Description(描述)
~~~~~~~~~~~~~~~~~

A call to `AUDIO_GET_CAPABILITIES(获取解码能力)`_ returns an unsigned integer with the
following bits set according to the hardware's capabilities.
(调用 `AUDIO_GET_CAPABILITIES(获取解码能力)`_ 会返回一个无符号整数，其中的位根据硬件能力按下列定义置位。)


-----


Audio Function Calls(音频函数调用)
==================================


AUDIO_STOP(停止音频播放)
------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_STOP

.. code-block:: c

	 int ioctl(int fd, int request = AUDIO_STOP)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  File descriptor returned by a previous call to `open()(打开音频设备)`_.(文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  :cspan:`1` Equals ``AUDIO_STOP`` for this command.(此命令中等于 ``AUDIO_STOP``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Audio Device to stop playing the current
stream.
(该 ioctl 调用请求音频设备停止播放当前数据流。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_PLAY(开始音频播放)
------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_PLAY

.. code-block:: c

	 int  ioctl(int fd, int request = AUDIO_PLAY)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  File descriptor returned by a previous call to `open()(打开音频设备)`_.(文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  :cspan:`1` Equals ``AUDIO_PLAY`` for this command.(此命令中等于 ``AUDIO_PLAY``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Audio Device to start playing an audio stream
from the selected source.
(该 ioctl 调用请求音频设备从所选来源开始播放音频流。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_PAUSE(暂停音频播放)
-------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_PAUSE

.. code-block:: c

	 int  ioctl(int fd, int request = AUDIO_PAUSE)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_PAUSE`` for this command.(此命令中等于 ``AUDIO_PAUSE``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call suspends the audio stream being played. Decoding and
playing are paused. It is then possible to restart again decoding and
playing process of the audio stream using `AUDIO_CONTINUE(继续音频播放)`_ command.
(该 ioctl 调用挂起正在播放的音频流，解码和播放被暂停。之后可以使用 `AUDIO_CONTINUE(继续音频播放)`_ 命令再次重启音频流的解码和播放过程。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_CONTINUE(继续音频播放)
----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_CONTINUE

.. code-block:: c

	 int  ioctl(int fd, int request = AUDIO_CONTINUE)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_CONTINUE`` for this command.(此命令中等于 ``AUDIO_CONTINUE``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl restarts the decoding and playing process previously paused
with `AUDIO_PAUSE(暂停音频播放)`_ command.
(该 ioctl 重启之前由 `AUDIO_PAUSE(暂停音频播放)`_ 命令暂停的解码和播放过程。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_SELECT_SOURCE(选择音频源)
-------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_SELECT_SOURCE

.. code-block:: c

	 int ioctl(int fd, int request = AUDIO_SELECT_SOURCE,
	 audio_stream_source_t source)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_SELECT_SOURCE`` for this command.(此命令中等于 ``AUDIO_SELECT_SOURCE``。)

    -  ..

       -  `audio_stream_source_t(音频流来源类型)`_ ``source``

       -  Indicates the source that shall be used for the Audio stream.(指示音频流应使用的来源。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call informs the audio device which source shall be used for
the input data. The possible sources are demux or memory. If
``AUDIO_SOURCE_MEMORY`` is selected, the data is fed to the Audio Device
through the write command. If ``AUDIO_SOURCE_DEMUX`` is selected, the data
is directly transferred from the onboard demux-device to the decoder.
Note: This only supports DVB-devices with one demux and one decoder so far.
(该 ioctl 调用告知音频设备输入数据应使用哪个来源。可能的来源是解复用器 (demux) 或内存 (memory)。若选择 ``AUDIO_SOURCE_MEMORY``，数据通过 write 命令馈送给音频设备。若选择 ``AUDIO_SOURCE_DEMUX``，数据直接从板载解复用设备传输到解码器。注意：到目前为止仅支持带有一个解复用器和一个解码器的 DVB 设备。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_SET_MUTE(设置静音)
------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_SET_MUTE

.. code-block:: c

	 int  ioctl(int fd, int request = AUDIO_SET_MUTE, int state)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  :cspan:`1` Equals ``AUDIO_SET_MUTE`` for this command.(此命令中等于 ``AUDIO_SET_MUTE``。)

    -  ..

       -  :rspan:`2` ``int state``

       -  :cspan:`1` Indicates if audio device shall mute or not.(指示音频设备是否应静音。)

    -  ..

       -  TRUE  ( != 0 )

       -  mute audio(静音)

    -  ..

       -  FALSE ( == 0 )

       -  unmute audio(取消静音)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl is for DVB devices only. To control a V4L2 decoder use the
V4L2 :ref:`VIDIOC_DECODER_CMD` with the
``V4L2_DEC_CMD_START_MUTE_AUDIO`` flag instead.
(该 ioctl 仅适用于 DVB 设备。要控制 V4L2 解码器，请改用带 ``V4L2_DEC_CMD_START_MUTE_AUDIO`` 标志的 V4L2 :ref:`VIDIOC_DECODER_CMD`。)

This ioctl call asks the audio device to mute the stream that is
currently being played.
(该 ioctl 调用请求音频设备将当前正在播放的数据流静音。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_SET_AV_SYNC(设置 A/V 同步)
--------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_SET_AV_SYNC

.. code-block:: c

	 int  ioctl(int fd, int request = AUDIO_SET_AV_SYNC, int state)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  :cspan:`1` Equals ``AUDIO_AV_SYNC`` for this command.(此命令中等于 ``AUDIO_AV_SYNC``。)

    -  ..

       -  :rspan:`2` ``int state``

       -  :cspan:`1` Tells the DVB subsystem if A/V synchronization
          shall be ON or OFF.
          (告知 DVB 子系统 A/V 同步应开启还是关闭。)

    -  ..

       -  TRUE  ( != 0 )

       -  AV-sync ON.(A/V 同步开启。)

    -  ..

       -  FALSE ( == 0 )

       -  AV-sync OFF.(A/V 同步关闭。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Audio Device to turn ON or OFF A/V
synchronization.
(该 ioctl 调用请求音频设备开启或关闭 A/V 同步。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_SET_BYPASS_MODE(设置直通模式)
-----------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_SET_BYPASS_MODE

.. code-block:: c

	 int ioctl(int fd, int request = AUDIO_SET_BYPASS_MODE, int mode)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  :cspan:`1` Equals ``AUDIO_SET_BYPASS_MODE`` for this command.(此命令中等于 ``AUDIO_SET_BYPASS_MODE``。)

    -  ..

       -  :rspan:`2` ``int mode``

       -  :cspan:`1` Enables or disables the decoding of the current
          Audio stream in the DVB subsystem.
          (启用或禁用 DVB 子系统对当前音频流的解码。)
    -  ..

       -  TRUE  ( != 0 )

       -  Disable bypass(禁用直通)

    -  ..

       -  FALSE ( == 0 )

       -  Enable bypass(启用直通)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Audio Device to bypass the Audio decoder and
forward the stream without decoding. This mode shall be used if streams
that can’t be handled by the DVB system shall be decoded. Dolby
DigitalTM streams are automatically forwarded by the DVB subsystem if
the hardware can handle it.
(该 ioctl 调用请求音频设备绕过音频解码器，直接转发数据流而不解码。当需要解码 DVB 系统无法处理的数据流时，应使用此模式。若硬件支持，杜比数字 (Dolby Digital) 数据流会被 DVB 子系统自动转发。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_CHANNEL_SELECT(声道选择)
------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_CHANNEL_SELECT

.. code-block:: c

	 int ioctl(int fd, int request = AUDIO_CHANNEL_SELECT,
	 audio_channel_select_t)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_CHANNEL_SELECT`` for this command.(此命令中等于 ``AUDIO_CHANNEL_SELECT``。)

    -  ..

       -  `audio_channel_select_t(声道选择类型)`_ ``ch``

       -  Select the output format of the audio (mono left/right, stereo).(选择音频的输出格式（左/右单声道、立体声）。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl is for DVB devices only. To control a V4L2 decoder use the
V4L2 ``V4L2_CID_MPEG_AUDIO_DEC_PLAYBACK`` control instead.
(该 ioctl 仅适用于 DVB 设备。要控制 V4L2 解码器，请改用 V4L2 的 ``V4L2_CID_MPEG_AUDIO_DEC_PLAYBACK`` 控件。)

This ioctl call asks the Audio Device to select the requested channel if
possible.
(该 ioctl 调用请求音频设备在可能的情况下选择所请求的声道。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_GET_STATUS(获取音频状态)
------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_GET_STATUS

.. code-block:: c

	 int ioctl(int fd, int request = AUDIO_GET_STATUS,
	 struct audio_status *status)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals AUDIO_GET_STATUS for this command.(此命令中等于 AUDIO_GET_STATUS。)

    -  ..

       -  ``struct`` `audio_status(音频状态)`_ ``*status``

       -  Returns the current state of Audio Device.(返回音频设备的当前状态。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Audio Device to return the current state of the
Audio Device.
(该 ioctl 调用请求音频设备返回音频设备的当前状态。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_GET_CAPABILITIES(获取解码能力)
------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_GET_CAPABILITIES

.. code-block:: c

	 int ioctl(int fd, int request = AUDIO_GET_CAPABILITIES,
	 unsigned int *cap)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_GET_CAPABILITIES`` for this command.(此命令中等于 ``AUDIO_GET_CAPABILITIES``。)

    -  ..

       -  ``unsigned int *cap``

       -  Returns a bit array of supported sound formats.
          Bits are defined in `audio encodings(音频编码)`_.
          (返回受支持声音格式的位数组。各比特的定义见 `audio encodings(音频编码)`_ 一节。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Audio Device to tell us about the decoding
capabilities of the audio hardware.
(该 ioctl 调用请求音频设备告知音频硬件的解码能力。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_CLEAR_BUFFER(清空音频缓冲区)
----------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_CLEAR_BUFFER

.. code-block:: c

	 int  ioctl(int fd, int request = AUDIO_CLEAR_BUFFER)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_CLEAR_BUFFER`` for this command.(此命令中等于 ``AUDIO_CLEAR_BUFFER``。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl call asks the Audio Device to clear all software and hardware
buffers of the audio decoder device.
(该 ioctl 调用请求音频设备清空音频解码器设备的所有软件和硬件缓冲区。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_SET_ID(设置音频子流 ID)
-----------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_SET_ID

.. code-block:: c

	 int  ioctl(int fd, int request = AUDIO_SET_ID, int id)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_SET_ID`` for this command.(此命令中等于 ``AUDIO_SET_ID``。)

    -  ..

       -  ``int id``

       -  Audio sub-stream id.(音频子流 ID。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl selects which sub-stream is to be decoded if a program or
system stream is sent to the video device.
(如果向视频设备发送的是节目流或系统流，该 ioctl 选择要解码的子流。)

If no audio stream type is set the id has to be in range [0xC0,0xDF]
for MPEG sound, in [0x80,0x87] for AC3 and in [0xA0,0xA7] for LPCM.
See ITU-T H.222.0 | ISO/IEC 13818-1 for further description.
(若未设置音频流类型，则 id 必须在 [0xC0,0xDF] 范围内表示 MPEG 声音，在 [0x80,0x87] 范围内表示 AC3，在 [0xA0,0xA7] 范围内表示 LPCM。更多说明见 ITU-T H.222.0 | ISO/IEC 13818-1。)

If the stream type is set with `AUDIO_SET_STREAMTYPE(设置流类型)`_, specifies the
id just the sub-stream id of the audio stream and only the first 5 bits
(& 0x1F) are recognized.
(如果已用 `AUDIO_SET_STREAMTYPE(设置流类型)`_ 设置了流类型，则 id 只指定音频流的子流 id，且仅识别前 5 位（& 0x1F）。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_SET_MIXER(设置混音器)
---------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_SET_MIXER

.. code-block:: c

	 int ioctl(int fd, int request = AUDIO_SET_MIXER, audio_mixer_t *mix)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_SET_MIXER`` for this command.(此命令中等于 ``AUDIO_SET_MIXER``。)

    -  ..

       -  ``audio_mixer_t *mix``

       -  Mixer settings.(混音器设置。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl lets you adjust the mixer settings of the audio decoder.
(该 ioctl 用于调整音频解码器的混音器设置。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


AUDIO_SET_STREAMTYPE(设置流类型)
--------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_SET_STREAMTYPE

.. code-block:: c

	 int  ioctl(fd, int request = AUDIO_SET_STREAMTYPE, int type)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_SET_STREAMTYPE`` for this command.(此命令中等于 ``AUDIO_SET_STREAMTYPE``。)

    -  ..

       -  ``int type``

       -  Stream type.(流类型。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl tells the driver which kind of audio stream to expect. This
is useful if the stream offers several audio sub-streams like LPCM and
AC3.
(该 ioctl 告知驱动程序预期的音频流类型。当数据流提供多个音频子流（如 LPCM 和 AC3）时，该调用很有用。)

Stream types defined in ITU-T H.222.0 | ISO/IEC 13818-1 are used.
(使用 ITU-T H.222.0 | ISO/IEC 13818-1 中定义的流类型。)


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

       -  ``EINVAL``

       -  Type is not a valid or supported stream type.(类型不是有效或受支持的流类型。)


-----


AUDIO_BILINGUAL_CHANNEL_SELECT(双语声道选择)
--------------------------------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. c:macro:: AUDIO_BILINGUAL_CHANNEL_SELECT

.. code-block:: c

	 int ioctl(int fd, int request = AUDIO_BILINGUAL_CHANNEL_SELECT,
	 audio_channel_select_t)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``int request``

       -  Equals ``AUDIO_BILINGUAL_CHANNEL_SELECT`` for this command.(此命令中等于 ``AUDIO_BILINGUAL_CHANNEL_SELECT``。)

    -  ..

       -  ``audio_channel_select_t ch``

       -  Select the output format of the audio (mono left/right, stereo).(选择音频的输出格式（左/右单声道、立体声）。)

Description(描述)
~~~~~~~~~~~~~~~~~

.. attention:: Do **not** use in new drivers!
             See: :ref:`legacy_dvb_decoder_notes`
             (请勿在新驱动中使用！参见 :ref:`legacy_dvb_decoder_notes`。)

This ioctl has been replaced by the V4L2
``V4L2_CID_MPEG_AUDIO_DEC_MULTILINGUAL_PLAYBACK`` control
for MPEG decoders controlled through V4L2.
(对于通过 V4L2 控制的 MPEG 解码器，此 ioctl 已被 V4L2 的 ``V4L2_CID_MPEG_AUDIO_DEC_MULTILINGUAL_PLAYBACK`` 控件取代。)

This ioctl call asks the Audio Device to select the requested channel
for bilingual streams if possible.
(该 ioctl 调用请求音频设备在可能的情况下为双语数据流选择所请求的声道。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码参见 :ref:`Generic Error Codes <gen-errors>` 一章。)


-----


open()(打开音频设备)
--------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

    #include <fcntl.h>

.. c:function:: int  open(const char *deviceName, int flags)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``const char *deviceName``

       -  Name of specific audio device.(要打开的具体音频设备名。)

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

This system call opens a named audio device (e.g.
``/dev/dvb/adapter0/audio0``) for subsequent use. When an open() call has
succeeded, the device will be ready for use. The significance of
blocking or non-blocking mode is described in the documentation for
functions where there is a difference. It does not affect the semantics
of the open() call itself. A device opened in blocking mode can later be
put into non-blocking mode (and vice versa) using the F_SETFL command
of the fcntl system call. This is a standard system call, documented in
the Linux manual page for fcntl. Only one user can open the Audio Device
in O_RDWR mode. All other attempts to open the device in this mode will
fail, and an error code will be returned. If the Audio Device is opened
in O_RDONLY mode, the only ioctl call that can be used is
`AUDIO_GET_STATUS(获取音频状态)`_. All other call will return with an error code.
(该系统调用打开一个命名的音频设备（例如 ``/dev/dvb/adapter0/audio0`` ）以供后续使用。open() 调用成功后，设备即可使用。阻塞或非阻塞模式的意义在存在差异的各函数的文档中说明；它不影响 open() 调用本身的语义。以阻塞模式打开的设备之后可以用 fcntl 系统调用的 F_SETFL 命令切换为非阻塞模式（反之亦然）。这是一个标准系统调用，在 Linux 的 fcntl 手册页中有说明。只允许一个用户以 O_RDWR 模式打开音频设备；其他在该模式下的打开尝试都会失败并返回错误码。若音频设备以 O_RDONLY 模式打开，则唯一可用的 ioctl 调用是 `AUDIO_GET_STATUS(获取音频状态)`_ ，其他调用都会返回错误码。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``ENODEV``

       -  Device driver not loaded/available.(设备驱动未加载/不可用。)

    -  ..

       -  ``EBUSY``

       -  Device or resource busy.(设备或资源忙。)

    -  ..

       -  ``EINVAL``

       -  Invalid argument.(无效参数。)


-----


close()(关闭音频设备)
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
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

Description(描述)
~~~~~~~~~~~~~~~~~

This system call closes a previously opened audio device.
(该系统调用关闭之前打开的音频设备。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EBADF``

       -  Fd is not a valid open file descriptor.(fd 不是有效的已打开文件描述符。)

-----


write()(写入音频数据)
---------------------

Synopsis(概要)
~~~~~~~~~~~~~~

.. code-block:: c

	 size_t write(int fd, const void *buf, size_t count)

Arguments(参数)
~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``int fd``

       -  :cspan:`1` File descriptor returned by a previous call
          to `open()(打开音频设备)`_.
          (文件描述符，来自之前 open() 调用返回的值。)

    -  ..

       -  ``void *buf``

       -  Pointer to the buffer containing the PES data.(指向包含 PES 数据的缓冲区的指针。)

    -  ..

       -  ``size_t count``

       -  Size of buf.(buf 的大小。)

Description(描述)
~~~~~~~~~~~~~~~~~

This system call can only be used if ``AUDIO_SOURCE_MEMORY`` is selected
in the ioctl call `AUDIO_SELECT_SOURCE(选择音频源)`_. The data provided shall be in
PES format. If ``O_NONBLOCK`` is not specified the function will block
until buffer space is available. The amount of data to be transferred is
implied by count.
(只有在 ioctl 调用 `AUDIO_SELECT_SOURCE(选择音频源)`_ 中选择了 ``AUDIO_SOURCE_MEMORY``，才能使用该系统调用。提供的数据应为 PES 格式。若未指定 ``O_NONBLOCK``，该函数将阻塞直到有缓冲区空间可用。要传输的数据量由 count 隐含给出。)

Return Value(返回值)
~~~~~~~~~~~~~~~~~~~~

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  ..

       -  ``EPERM``

       -  :cspan:`1` Mode ``AUDIO_SOURCE_MEMORY`` not selected.(未选择 ``AUDIO_SOURCE_MEMORY`` 模式。)

    -  ..

       -  ``ENOMEM``

       -  Attempted to write more data than the internal buffer can hold.(试图写入的数据量超过了内部缓冲区的容量。)

    -  ..

       -  ``EBADF``

       -  Fd is not a valid open file descriptor.(fd 不是有效的已打开文件描述符。)
