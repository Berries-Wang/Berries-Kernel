.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _codec-controls:

*****************************************
Codec Control Reference(编解码器控制参考)
*****************************************

Below all controls within the Codec control class are described. First
the generic controls, then controls specific for certain hardware.
(下面描述 Codec 控制类中的所有控制。先是通用控制，然后是针对特定硬件的控制。)

.. note::

   These controls are applicable to all codecs and not just MPEG. The
   defines are prefixed with V4L2_CID_MPEG/V4L2_MPEG as the controls
   were originally made for MPEG codecs and later extended to cover all
   encoding formats.
   (这些控制适用于所有编解码器，而不仅仅是 MPEG。这些定义之所以以
   V4L2_CID_MPEG/V4L2_MPEG 为前缀，是因为这些控制最初是为 MPEG 编解码器
   制定的，后来才扩展到涵盖所有编码格式。)


Generic Codec Controls(通用编解码器控制)
========================================


.. _mpeg-control-id:

Codec Control IDs(编解码器控制 ID)
----------------------------------

``V4L2_CID_CODEC_CLASS (class)`` (类描述符)
    The Codec class descriptor. Calling
    :ref:`VIDIOC_QUERYCTRL` for this control will
    return a description of this control class. This description can be
    used as the caption of a Tab page in a GUI, for example.
    (Codec 类的描述符。对该控制调用 :ref:`VIDIOC_QUERYCTRL` 将返回此控制类
    的描述。例如，该描述可以用作 GUI 中选项卡页面的标题。)

.. _v4l2-mpeg-stream-type:

``V4L2_CID_MPEG_STREAM_TYPE`` (流类型)
    (enum)

enum v4l2_mpeg_stream_type -
    The MPEG-1, -2 or -4 output stream type. One cannot assume anything
    here. Each hardware MPEG encoder tends to support different subsets
    of the available MPEG stream types. This control is specific to
    multiplexed MPEG streams. The currently defined stream types are:
    (MPEG-1、-2 或 -4 输出流类型。这里不能做任何假设。每个硬件 MPEG 编码器
    往往只支持可用 MPEG 流类型的不同子集。此控制专用于复用的 MPEG 流。当前
    定义的流类型有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_STREAM_TYPE_MPEG2_PS``
      - MPEG-2 program stream (MPEG-2 节目流)
    * - ``V4L2_MPEG_STREAM_TYPE_MPEG2_TS``
      - MPEG-2 transport stream (MPEG-2 传输流)
    * - ``V4L2_MPEG_STREAM_TYPE_MPEG1_SS``
      - MPEG-1 system stream (MPEG-1 系统流)
    * - ``V4L2_MPEG_STREAM_TYPE_MPEG2_DVD``
      - MPEG-2 DVD-compatible stream (MPEG-2 DVD 兼容流)
    * - ``V4L2_MPEG_STREAM_TYPE_MPEG1_VCD``
      - MPEG-1 VCD-compatible stream (MPEG-1 VCD 兼容流)
    * - ``V4L2_MPEG_STREAM_TYPE_MPEG2_SVCD``
      - MPEG-2 SVCD-compatible stream (MPEG-2 SVCD 兼容流)



``V4L2_CID_MPEG_STREAM_PID_PMT (integer)`` (PMT 包 ID)
    Program Map Table Packet ID for the MPEG transport stream (default
    16)
    (MPEG 传输流的节目映射表 (Program Map Table) 包 ID（默认 16）。)

``V4L2_CID_MPEG_STREAM_PID_AUDIO (integer)`` (音频包 ID)
    Audio Packet ID for the MPEG transport stream (default 256)
    (MPEG 传输流的音频包 ID（默认 256）。)

``V4L2_CID_MPEG_STREAM_PID_VIDEO (integer)`` (视频包 ID)
    Video Packet ID for the MPEG transport stream (default 260)
    (MPEG 传输流的视频包 ID（默认 260）。)

``V4L2_CID_MPEG_STREAM_PID_PCR (integer)`` (PCR 包 ID)
    Packet ID for the MPEG transport stream carrying PCR fields (default
    259)
    (MPEG 传输流中承载 PCR 字段的包 ID（默认 259）。)

``V4L2_CID_MPEG_STREAM_PES_ID_AUDIO (integer)`` (PES 音频 ID)
    Audio ID for MPEG PES
    (MPEG PES 的音频 ID。)

``V4L2_CID_MPEG_STREAM_PES_ID_VIDEO (integer)`` (PES 视频 ID)
    Video ID for MPEG PES
    (MPEG PES 的视频 ID。)

.. _v4l2-mpeg-stream-vbi-fmt:

``V4L2_CID_MPEG_STREAM_VBI_FMT`` (VBI 格式)
    (enum)

enum v4l2_mpeg_stream_vbi_fmt -
    Some cards can embed VBI data (e. g. Closed Caption, Teletext) into
    the MPEG stream. This control selects whether VBI data should be
    embedded, and if so, what embedding method should be used. The list
    of possible VBI formats depends on the driver. The currently defined
    VBI format types are:
    (某些卡可以将 VBI 数据（如隐藏字幕、图文电视）嵌入 MPEG 流。此控制选择
    是否应嵌入 VBI 数据，如果嵌入，应使用哪种嵌入方法。可能的 VBI 格式列表
    取决于驱动程序。当前定义的 VBI 格式类型有：)



.. tabularcolumns:: |p{6.6 cm}|p{10.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_STREAM_VBI_FMT_NONE``
      - No VBI in the MPEG stream (MPEG 流中无 VBI)
    * - ``V4L2_MPEG_STREAM_VBI_FMT_IVTV``
      - VBI in private packets, IVTV format (documented in the kernel
	sources in the file
	``Documentation/userspace-api/media/drivers/cx2341x-uapi.rst``)
        (私有包中的 VBI，IVTV 格式（其文档在内核源码的
        ``Documentation/userspace-api/media/drivers/cx2341x-uapi.rst``
        文件中）。)



.. _v4l2-mpeg-audio-sampling-freq:

``V4L2_CID_MPEG_AUDIO_SAMPLING_FREQ`` (音频采样频率)
    (enum)

enum v4l2_mpeg_audio_sampling_freq -
    MPEG Audio sampling frequency. Possible values are:
    (MPEG 音频采样频率。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_SAMPLING_FREQ_44100``
      - 44.1 kHz
    * - ``V4L2_MPEG_AUDIO_SAMPLING_FREQ_48000``
      - 48 kHz
    * - ``V4L2_MPEG_AUDIO_SAMPLING_FREQ_32000``
      - 32 kHz



.. _v4l2-mpeg-audio-encoding:

``V4L2_CID_MPEG_AUDIO_ENCODING`` (音频编码)
    (enum)

enum v4l2_mpeg_audio_encoding -
    MPEG Audio encoding. This control is specific to multiplexed MPEG
    streams. Possible values are:
    (MPEG 音频编码。此控制专用于复用的 MPEG 流。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_ENCODING_LAYER_1``
      - MPEG-1/2 Layer I encoding (MPEG-1/2 Layer I 编码)
    * - ``V4L2_MPEG_AUDIO_ENCODING_LAYER_2``
      - MPEG-1/2 Layer II encoding (MPEG-1/2 Layer II 编码)
    * - ``V4L2_MPEG_AUDIO_ENCODING_LAYER_3``
      - MPEG-1/2 Layer III encoding (MPEG-1/2 Layer III 编码)
    * - ``V4L2_MPEG_AUDIO_ENCODING_AAC``
      - MPEG-2/4 AAC (Advanced Audio Coding) (MPEG-2/4 AAC（高级音频编码）)
    * - ``V4L2_MPEG_AUDIO_ENCODING_AC3``
      - AC-3 aka ATSC A/52 encoding (AC-3，又称 ATSC A/52 编码)



.. _v4l2-mpeg-audio-l1-bitrate:

``V4L2_CID_MPEG_AUDIO_L1_BITRATE`` (Layer I 比特率)
    (enum)

enum v4l2_mpeg_audio_l1_bitrate -
    MPEG-1/2 Layer I bitrate. Possible values are:
    (MPEG-1/2 Layer I 比特率。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_32K``
      - 32 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_64K``
      - 64 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_96K``
      - 96 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_128K``
      - 128 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_160K``
      - 160 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_192K``
      - 192 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_224K``
      - 224 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_256K``
      - 256 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_288K``
      - 288 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_320K``
      - 320 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_352K``
      - 352 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_384K``
      - 384 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_416K``
      - 416 kbit/s
    * - ``V4L2_MPEG_AUDIO_L1_BITRATE_448K``
      - 448 kbit/s



.. _v4l2-mpeg-audio-l2-bitrate:

``V4L2_CID_MPEG_AUDIO_L2_BITRATE`` (Layer II 比特率)
    (enum)

enum v4l2_mpeg_audio_l2_bitrate -
    MPEG-1/2 Layer II bitrate. Possible values are:
    (MPEG-1/2 Layer II 比特率。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_32K``
      - 32 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_48K``
      - 48 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_56K``
      - 56 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_64K``
      - 64 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_80K``
      - 80 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_96K``
      - 96 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_112K``
      - 112 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_128K``
      - 128 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_160K``
      - 160 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_192K``
      - 192 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_224K``
      - 224 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_256K``
      - 256 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_320K``
      - 320 kbit/s
    * - ``V4L2_MPEG_AUDIO_L2_BITRATE_384K``
      - 384 kbit/s



.. _v4l2-mpeg-audio-l3-bitrate:

``V4L2_CID_MPEG_AUDIO_L3_BITRATE`` (Layer III 比特率)
    (enum)

enum v4l2_mpeg_audio_l3_bitrate -
    MPEG-1/2 Layer III bitrate. Possible values are:
    (MPEG-1/2 Layer III 比特率。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_32K``
      - 32 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_40K``
      - 40 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_48K``
      - 48 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_56K``
      - 56 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_64K``
      - 64 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_80K``
      - 80 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_96K``
      - 96 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_112K``
      - 112 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_128K``
      - 128 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_160K``
      - 160 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_192K``
      - 192 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_224K``
      - 224 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_256K``
      - 256 kbit/s
    * - ``V4L2_MPEG_AUDIO_L3_BITRATE_320K``
      - 320 kbit/s



``V4L2_CID_MPEG_AUDIO_AAC_BITRATE (integer)`` (AAC 比特率)
    AAC bitrate in bits per second.
    (AAC 比特率，单位为比特每秒。)

.. _v4l2-mpeg-audio-ac3-bitrate:

``V4L2_CID_MPEG_AUDIO_AC3_BITRATE`` (AC-3 比特率)
    (enum)

enum v4l2_mpeg_audio_ac3_bitrate -
    AC-3 bitrate. Possible values are:
    (AC-3 比特率。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_32K``
      - 32 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_40K``
      - 40 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_48K``
      - 48 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_56K``
      - 56 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_64K``
      - 64 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_80K``
      - 80 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_96K``
      - 96 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_112K``
      - 112 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_128K``
      - 128 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_160K``
      - 160 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_192K``
      - 192 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_224K``
      - 224 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_256K``
      - 256 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_320K``
      - 320 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_384K``
      - 384 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_448K``
      - 448 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_512K``
      - 512 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_576K``
      - 576 kbit/s
    * - ``V4L2_MPEG_AUDIO_AC3_BITRATE_640K``
      - 640 kbit/s



.. _v4l2-mpeg-audio-mode:

``V4L2_CID_MPEG_AUDIO_MODE`` (音频模式)
    (enum)

enum v4l2_mpeg_audio_mode -
    MPEG Audio mode. Possible values are:
    (MPEG 音频模式。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_MODE_STEREO``
      - Stereo (立体声)
    * - ``V4L2_MPEG_AUDIO_MODE_JOINT_STEREO``
      - Joint Stereo (联合立体声)
    * - ``V4L2_MPEG_AUDIO_MODE_DUAL``
      - Bilingual (双语)
    * - ``V4L2_MPEG_AUDIO_MODE_MONO``
      - Mono (单声道)



.. _v4l2-mpeg-audio-mode-extension:

``V4L2_CID_MPEG_AUDIO_MODE_EXTENSION`` (音频模式扩展)
    (enum)

enum v4l2_mpeg_audio_mode_extension -
    Joint Stereo audio mode extension. In Layer I and II they indicate
    which subbands are in intensity stereo. All other subbands are coded
    in stereo. Layer III is not (yet) supported. Possible values are:
    (联合立体声音频模式的扩展。在 Layer I 和 Layer II 中，它们指示哪些子带
    采用强度立体声 (intensity stereo)。所有其他子带按立体声编码。Layer III
    尚（未）支持。可能的取值有：)

.. tabularcolumns:: |p{9.1cm}|p{8.4cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_4``
      - Subbands 4-31 in intensity stereo (子带 4-31 为强度立体声)
    * - ``V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_8``
      - Subbands 8-31 in intensity stereo (子带 8-31 为强度立体声)
    * - ``V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_12``
      - Subbands 12-31 in intensity stereo (子带 12-31 为强度立体声)
    * - ``V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_16``
      - Subbands 16-31 in intensity stereo (子带 16-31 为强度立体声)



.. _v4l2-mpeg-audio-emphasis:

``V4L2_CID_MPEG_AUDIO_EMPHASIS`` (音频预加重)
    (enum)

enum v4l2_mpeg_audio_emphasis -
    Audio Emphasis. Possible values are:
    (音频预加重 (emphasis)。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_EMPHASIS_NONE``
      - None (无)
    * - ``V4L2_MPEG_AUDIO_EMPHASIS_50_DIV_15_uS``
      - 50/15 microsecond emphasis (50/15 微秒预加重)
    * - ``V4L2_MPEG_AUDIO_EMPHASIS_CCITT_J17``
      - CCITT J.17



.. _v4l2-mpeg-audio-crc:

``V4L2_CID_MPEG_AUDIO_CRC`` (音频 CRC)
    (enum)

enum v4l2_mpeg_audio_crc -
    CRC method. Possible values are:
    (CRC 方法。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_CRC_NONE``
      - None (无)
    * - ``V4L2_MPEG_AUDIO_CRC_CRC16``
      - 16 bit parity check (16 位奇偶校验)



``V4L2_CID_MPEG_AUDIO_MUTE (boolean)`` (音频静音)
    Mutes the audio when capturing. This is not done by muting audio
    hardware, which can still produce a slight hiss, but in the encoder
    itself, guaranteeing a fixed and reproducible audio bitstream. 0 =
    unmuted, 1 = muted.
    (采集时将音频静音。这不是通过静音音频硬件来实现的（那样仍会产生轻微的
    嘶声），而是在编码器内部完成，从而保证音频比特流固定且可重现。0 =
    不静音，1 = 静音。)

.. _v4l2-mpeg-audio-dec-playback:

``V4L2_CID_MPEG_AUDIO_DEC_PLAYBACK`` (单声道音频播放方式)
    (enum)

enum v4l2_mpeg_audio_dec_playback -
    Determines how monolingual audio should be played back. Possible
    values are:
    (决定单声道音频 (monolingual audio) 应如何播放。可能的取值有：)



.. tabularcolumns:: |p{9.8cm}|p{7.7cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_AUDIO_DEC_PLAYBACK_AUTO``
      - Automatically determines the best playback mode.
        (自动确定最佳播放模式。)
    * - ``V4L2_MPEG_AUDIO_DEC_PLAYBACK_STEREO``
      - Stereo playback.
        (立体声播放。)
    * - ``V4L2_MPEG_AUDIO_DEC_PLAYBACK_LEFT``
      - Left channel playback.
        (左声道播放。)
    * - ``V4L2_MPEG_AUDIO_DEC_PLAYBACK_RIGHT``
      - Right channel playback.
        (右声道播放。)
    * - ``V4L2_MPEG_AUDIO_DEC_PLAYBACK_MONO``
      - Mono playback.
        (单声道播放。)
    * - ``V4L2_MPEG_AUDIO_DEC_PLAYBACK_SWAPPED_STEREO``
      - Stereo playback with swapped left and right channels.
        (左右声道互换的立体声播放。)



.. _v4l2-mpeg-audio-dec-multilingual-playback:

``V4L2_CID_MPEG_AUDIO_DEC_MULTILINGUAL_PLAYBACK`` (多语言音频播放方式)
    (enum)

enum v4l2_mpeg_audio_dec_playback -
    Determines how multilingual audio should be played back.
    (决定多语言音频应如何播放。)

.. _v4l2-mpeg-video-encoding:

``V4L2_CID_MPEG_VIDEO_ENCODING`` (视频编码)
    (enum)

enum v4l2_mpeg_video_encoding -
    MPEG Video encoding method. This control is specific to multiplexed
    MPEG streams. Possible values are:
    (MPEG 视频编码方法。此控制专用于复用的 MPEG 流。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_ENCODING_MPEG_1``
      - MPEG-1 Video encoding (MPEG-1 视频编码)
    * - ``V4L2_MPEG_VIDEO_ENCODING_MPEG_2``
      - MPEG-2 Video encoding (MPEG-2 视频编码)
    * - ``V4L2_MPEG_VIDEO_ENCODING_MPEG_4_AVC``
      - MPEG-4 AVC (H.264) Video encoding (MPEG-4 AVC (H.264) 视频编码)



.. _v4l2-mpeg-video-aspect:

``V4L2_CID_MPEG_VIDEO_ASPECT`` (视频宽高比)
    (enum)

enum v4l2_mpeg_video_aspect -
    Video aspect. Possible values are:
    (视频宽高比。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_ASPECT_1x1``
    * - ``V4L2_MPEG_VIDEO_ASPECT_4x3``
    * - ``V4L2_MPEG_VIDEO_ASPECT_16x9``
    * - ``V4L2_MPEG_VIDEO_ASPECT_221x100``



``V4L2_CID_MPEG_VIDEO_B_FRAMES (integer)`` (B 帧数量)
    Number of B-Frames (default 2)
    (B 帧的数量（默认 2）。)

``V4L2_CID_MPEG_VIDEO_GOP_SIZE (integer)`` (GOP 大小)
    GOP size (default 12)
    (GOP 大小（默认 12）。)

``V4L2_CID_MPEG_VIDEO_GOP_CLOSURE (boolean)`` (GOP 闭合)
    GOP closure (default 1)
    (GOP 闭合（默认 1）。)

``V4L2_CID_MPEG_VIDEO_PULLDOWN (boolean)`` (3:2 pulldown，3:2 下拉)
    Enable 3:2 pulldown (default 0)
    (启用 3:2 pulldown（默认 0）。)

.. _v4l2-mpeg-video-bitrate-mode:

``V4L2_CID_MPEG_VIDEO_BITRATE_MODE`` (比特率模式)
    (enum)

enum v4l2_mpeg_video_bitrate_mode -
    Video bitrate mode. Possible values are:
    (视频比特率模式。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_BITRATE_MODE_VBR``
      - Variable bitrate (可变比特率)
    * - ``V4L2_MPEG_VIDEO_BITRATE_MODE_CBR``
      - Constant bitrate (恒定比特率)
    * - ``V4L2_MPEG_VIDEO_BITRATE_MODE_CQ``
      - Constant quality (恒定质量)



``V4L2_CID_MPEG_VIDEO_BITRATE (integer)`` (平均视频比特率)
    Average video bitrate in bits per second.
    (平均视频比特率，单位为比特每秒。)

``V4L2_CID_MPEG_VIDEO_BITRATE_PEAK (integer)`` (峰值视频比特率)
    Peak video bitrate in bits per second. Must be larger or equal to
    the average video bitrate. It is ignored if the video bitrate mode
    is set to constant bitrate.
    (峰值视频比特率，单位为比特每秒。必须大于或等于平均视频比特率。如果
    视频比特率模式设置为恒定比特率，则忽略该值。)

``V4L2_CID_MPEG_VIDEO_CONSTANT_QUALITY (integer)`` (恒定质量级别)
    Constant quality level control. This control is applicable when
    ``V4L2_CID_MPEG_VIDEO_BITRATE_MODE`` value is
    ``V4L2_MPEG_VIDEO_BITRATE_MODE_CQ``. Valid range is 1 to 100
    where 1 indicates lowest quality and 100 indicates highest quality.
    Encoder will decide the appropriate quantization parameter and
    bitrate to produce requested frame quality.
    (恒定质量级别控制。当 ``V4L2_CID_MPEG_VIDEO_BITRATE_MODE`` 的值为
    ``V4L2_MPEG_VIDEO_BITRATE_MODE_CQ`` 时，此控制适用。有效范围为 1 到
    100，其中 1 表示最低质量，100 表示最高质量。编码器将决定合适的量化参数
    和比特率，以产生所请求的帧质量。)


``V4L2_CID_MPEG_VIDEO_FRAME_SKIP_MODE (enum)`` (跳帧模式)

enum v4l2_mpeg_video_frame_skip_mode -
    Indicates in what conditions the encoder should skip frames. If
    encoding a frame would cause the encoded stream to be larger then a
    chosen data limit then the frame will be skipped. Possible values
    are:
    (指示编码器在什么条件下应跳过帧。如果编码一帧会导致编码后的流超过选定的
    数据上限，则该帧将被跳过。可能的取值有：)


.. tabularcolumns:: |p{8.2cm}|p{9.3cm}|

.. raw:: latex

    \small

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_FRAME_SKIP_MODE_DISABLED``
      - Frame skip mode is disabled.
        (跳帧模式被禁用。)
    * - ``V4L2_MPEG_VIDEO_FRAME_SKIP_MODE_LEVEL_LIMIT``
      - Frame skip mode enabled and buffer limit is set by the chosen
        level and is defined by the standard.
        (启用跳帧模式，缓冲区上限由所选级别设定，并由标准定义。)
    * - ``V4L2_MPEG_VIDEO_FRAME_SKIP_MODE_BUF_LIMIT``
      - Frame skip mode enabled and buffer limit is set by the
        :ref:`VBV (MPEG1/2/4) <v4l2-mpeg-video-vbv-size>` or
        :ref:`CPB (H264) buffer size <v4l2-mpeg-video-h264-cpb-size>` control.
        (启用跳帧模式，缓冲区上限由 :ref:`VBV (MPEG1/2/4)
        <v4l2-mpeg-video-vbv-size>` 或 :ref:`CPB (H264) buffer size
        <v4l2-mpeg-video-h264-cpb-size>` 控制设定。)

.. raw:: latex

    \normalsize

``V4L2_CID_MPEG_VIDEO_TEMPORAL_DECIMATION (integer)`` (时域抽帧)
    For every captured frame, skip this many subsequent frames (default
    0).
    (对每个采集到的帧，跳过其后这么多的帧（默认 0）。)

``V4L2_CID_MPEG_VIDEO_MUTE (boolean)`` (视频静色)
    "Mutes" the video to a fixed color when capturing. This is useful
    for testing, to produce a fixed video bitstream. 0 = unmuted, 1 =
    muted.
    (采集时将视频"静音"为固定颜色。这对测试很有用，可以产生固定的视频
    比特流。0 = 不静色，1 = 静色。)

``V4L2_CID_MPEG_VIDEO_MUTE_YUV (integer)`` (静色 YUV 值)
    Sets the "mute" color of the video. The supplied 32-bit integer is
    interpreted as follows (bit 0 = least significant bit):
    (设置视频的"静音"颜色。所提供的 32 位整数按如下方式解释
    （位 0 = 最低有效位）：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - Bit 0:7
      - V chrominance information (V 色度信息)
    * - Bit 8:15
      - U chrominance information (U 色度信息)
    * - Bit 16:23
      - Y luminance information (Y 亮度信息)
    * - Bit 24:31
      - Must be zero. (必须为 0。)



.. _v4l2-mpeg-video-dec-pts:

``V4L2_CID_MPEG_VIDEO_DEC_PTS (integer64)`` (显示时间戳)
    This read-only control returns the 33-bit video Presentation Time
    Stamp as defined in ITU T-REC-H.222.0 and ISO/IEC 13818-1 of the
    currently displayed frame. This is the same PTS as is used in
    :ref:`VIDIOC_DECODER_CMD`.
    (此只读控制返回当前显示帧的 33 位视频显示时间戳 (Presentation Time
    Stamp)，定义见 ITU T-REC-H.222.0 和 ISO/IEC 13818-1。它与
    :ref:`VIDIOC_DECODER_CMD` 中使用的 PTS 相同。)

.. _v4l2-mpeg-video-dec-frame:

``V4L2_CID_MPEG_VIDEO_DEC_FRAME (integer64)`` (解码帧计数)
    This read-only control returns the frame counter of the frame that
    is currently displayed (decoded). This value is reset to 0 whenever
    the decoder is started.
    (此只读控制返回当前显示（解码）帧的帧计数器。每当解码器启动时，此值都会
    重置为 0。)

``V4L2_CID_MPEG_VIDEO_DEC_CONCEAL_COLOR (integer64)`` (差错隐藏颜色)
    This control sets the conceal color in YUV color space. It describes
    the client preference of the error conceal color in case of an error
    where the reference frame is missing. The decoder should fill the
    reference buffer with the preferred color and use it for future
    decoding. The control is using 16 bits per channel.
    Applicable to decoders.
    (此控制在 YUV 色彩空间中设置差错隐藏颜色。它描述客户端在参考帧缺失的
    差错情况下对差错隐藏颜色的偏好。解码器应使用该偏好颜色填充参考缓冲区，
    并将其用于后续解码。此控制每个通道使用 16 位。适用于解码器。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * -
      - 8bit  format (8bit 格式)
      - 10bit format (10bit 格式)
      - 12bit format (12bit 格式)
    * - Y luminance (Y 亮度)
      - Bit 0:7
      - Bit 0:9
      - Bit 0:11
    * - Cb chrominance (Cb 色度)
      - Bit 16:23
      - Bit 16:25
      - Bit 16:27
    * - Cr chrominance (Cr 色度)
      - Bit 32:39
      - Bit 32:41
      - Bit 32:43
    * - Must be zero (必须为 0)
      - Bit 48:63
      - Bit 48:63
      - Bit 48:63

``V4L2_CID_MPEG_VIDEO_DECODER_SLICE_INTERFACE (boolean)`` (切片接口)
    If enabled the decoder expects to receive a single slice per buffer,
    otherwise the decoder expects a single frame in per buffer.
    Applicable to the decoder, all codecs.
    (如果启用，解码器期望每个缓冲区接收单个切片；否则解码器期望每个缓冲区
    包含单个帧。适用于解码器、所有编解码器。)

``V4L2_CID_MPEG_VIDEO_DEC_DISPLAY_DELAY_ENABLE (boolean)`` (显示延迟启用)
    If the display delay is enabled then the decoder is forced to return
    a CAPTURE buffer (decoded frame) after processing a certain number
    of OUTPUT buffers. The delay can be set through
    ``V4L2_CID_MPEG_VIDEO_DEC_DISPLAY_DELAY``. This
    feature can be used for example for generating thumbnails of videos.
    Applicable to the decoder.
    (如果启用了显示延迟，则解码器在处理一定数量的 OUTPUT 缓冲区后被强制返回
    一个 CAPTURE 缓冲区（解码帧）。延迟可以通过
    ``V4L2_CID_MPEG_VIDEO_DEC_DISPLAY_DELAY`` 设置。此功能可用于例如生成
    视频缩略图。适用于解码器。)

``V4L2_CID_MPEG_VIDEO_DEC_DISPLAY_DELAY (integer)`` (显示延迟)
    Display delay value for decoder. The decoder is forced to
    return a decoded frame after the set 'display delay' number of
    frames. If this number is low it may result in frames returned out
    of display order, in addition the hardware may still be using the
    returned buffer as a reference picture for subsequent frames.
    (解码器的显示延迟值。解码器在设定数量的"display delay"帧之后被强制返回
    一个解码帧。如果此数值较小，可能导致返回的帧不按显示顺序；此外，硬件
    可能仍在把返回的缓冲区用作后续帧的参考图像。)

``V4L2_CID_MPEG_VIDEO_AU_DELIMITER (boolean)`` (AU 分隔符)
    If enabled then, AUD (Access Unit Delimiter) NALUs will be generated.
    That could be useful to find the start of a frame without having to
    fully parse each NALU. Applicable to the H264 and HEVC encoders.
    (如果启用，则会生成 AUD（Access Unit Delimiter，访问单元分隔符）NALU。
    这有助于找到帧的起始位置，而不必完整解析每个 NALU。适用于 H264 和 HEVC
    编码器。)

``V4L2_CID_MPEG_VIDEO_H264_VUI_SAR_ENABLE (boolean)`` (SAR 写入启用)
    Enable writing sample aspect ratio in the Video Usability
    Information. Applicable to the H264 encoder.
    (启用在视频可用性信息 (Video Usability Information) 中写入样本宽高比。
    适用于 H264 编码器。)

.. _v4l2-mpeg-video-h264-vui-sar-idc:

``V4L2_CID_MPEG_VIDEO_H264_VUI_SAR_IDC`` (SAR 指示值)
    (enum)

enum v4l2_mpeg_video_h264_vui_sar_idc -
    VUI sample aspect ratio indicator for H.264 encoding. The value is
    defined in the table E-1 in the standard. Applicable to the H264
    encoder.
    (H.264 编码的 VUI 样本宽高比指示值。该值在标准的表 E-1 中定义。适用于
    H264 编码器。)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_UNSPECIFIED``
      - Unspecified (未指定)
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_1x1``
      - 1x1
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_12x11``
      - 12x11
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_10x11``
      - 10x11
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_16x11``
      - 16x11
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_40x33``
      - 40x33
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_24x11``
      - 24x11
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_20x11``
      - 20x11
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_32x11``
      - 32x11
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_80x33``
      - 80x33
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_18x11``
      - 18x11
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_15x11``
      - 15x11
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_64x33``
      - 64x33
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_160x99``
      - 160x99
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_4x3``
      - 4x3
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_3x2``
      - 3x2
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_2x1``
      - 2x1
    * - ``V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_EXTENDED``
      - Extended SAR (扩展 SAR)



``V4L2_CID_MPEG_VIDEO_H264_VUI_EXT_SAR_WIDTH (integer)`` (扩展 SAR 宽度)
    Extended sample aspect ratio width for H.264 VUI encoding.
    Applicable to the H264 encoder.
    (H.264 VUI 编码的扩展样本宽高比宽度。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_VUI_EXT_SAR_HEIGHT (integer)`` (扩展 SAR 高度)
    Extended sample aspect ratio height for H.264 VUI encoding.
    Applicable to the H264 encoder.
    (H.264 VUI 编码的扩展样本宽高比高度。适用于 H264 编码器。)

.. _v4l2-mpeg-video-h264-level:

``V4L2_CID_MPEG_VIDEO_H264_LEVEL`` (H264 级别)
    (enum)

enum v4l2_mpeg_video_h264_level -
    The level information for the H264 video elementary stream.
    Applicable to the H264 encoder. Possible values are:
    (H264 视频基本流的级别信息。适用于 H264 编码器。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_1_0``
      - Level 1.0
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_1B``
      - Level 1B
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_1_1``
      - Level 1.1
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_1_2``
      - Level 1.2
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_1_3``
      - Level 1.3
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_2_0``
      - Level 2.0
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_2_1``
      - Level 2.1
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_2_2``
      - Level 2.2
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_3_0``
      - Level 3.0
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_3_1``
      - Level 3.1
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_3_2``
      - Level 3.2
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_4_0``
      - Level 4.0
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_4_1``
      - Level 4.1
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_4_2``
      - Level 4.2
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_5_0``
      - Level 5.0
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_5_1``
      - Level 5.1
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_5_2``
      - Level 5.2
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_6_0``
      - Level 6.0
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_6_1``
      - Level 6.1
    * - ``V4L2_MPEG_VIDEO_H264_LEVEL_6_2``
      - Level 6.2



.. _v4l2-mpeg-video-mpeg2-level:

``V4L2_CID_MPEG_VIDEO_MPEG2_LEVEL`` (MPEG2 级别)
    (enum)

enum v4l2_mpeg_video_mpeg2_level -
    The level information for the MPEG2 elementary stream. Applicable to
    MPEG2 codecs. Possible values are:
    (MPEG2 基本流的级别信息。适用于 MPEG2 编解码器。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_MPEG2_LEVEL_LOW``
      - Low Level (LL) (低级别 (LL))
    * - ``V4L2_MPEG_VIDEO_MPEG2_LEVEL_MAIN``
      - Main Level (ML) (主级别 (ML))
    * - ``V4L2_MPEG_VIDEO_MPEG2_LEVEL_HIGH_1440``
      - High-1440 Level (H-14) (High-1440 级别 (H-14))
    * - ``V4L2_MPEG_VIDEO_MPEG2_LEVEL_HIGH``
      - High Level (HL) (高级别 (HL))



.. _v4l2-mpeg-video-mpeg4-level:

``V4L2_CID_MPEG_VIDEO_MPEG4_LEVEL`` (MPEG4 级别)
    (enum)

enum v4l2_mpeg_video_mpeg4_level -
    The level information for the MPEG4 elementary stream. Applicable to
    the MPEG4 encoder. Possible values are:
    (MPEG4 基本流的级别信息。适用于 MPEG4 编码器。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_MPEG4_LEVEL_0``
      - Level 0
    * - ``V4L2_MPEG_VIDEO_MPEG4_LEVEL_0B``
      - Level 0b
    * - ``V4L2_MPEG_VIDEO_MPEG4_LEVEL_1``
      - Level 1
    * - ``V4L2_MPEG_VIDEO_MPEG4_LEVEL_2``
      - Level 2
    * - ``V4L2_MPEG_VIDEO_MPEG4_LEVEL_3``
      - Level 3
    * - ``V4L2_MPEG_VIDEO_MPEG4_LEVEL_3B``
      - Level 3b
    * - ``V4L2_MPEG_VIDEO_MPEG4_LEVEL_4``
      - Level 4
    * - ``V4L2_MPEG_VIDEO_MPEG4_LEVEL_5``
      - Level 5



.. _v4l2-mpeg-video-h264-profile:

``V4L2_CID_MPEG_VIDEO_H264_PROFILE`` (H264 档次)
    (enum)

enum v4l2_mpeg_video_h264_profile -
    The profile information for H264. Applicable to the H264 encoder.
    Possible values are:
    (H264 的档次信息。适用于 H264 编码器。可能的取值有：)

.. raw:: latex

    \small

.. tabularcolumns:: |p{10.2cm}|p{7.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_BASELINE``
      - Baseline profile (基线档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_CONSTRAINED_BASELINE``
      - Constrained Baseline profile (受限基线档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_MAIN``
      - Main profile (主档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_EXTENDED``
      - Extended profile (扩展档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_HIGH``
      - High profile (高级档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_10``
      - High 10 profile (High 10 档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_422``
      - High 422 profile (High 422 档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_444_PREDICTIVE``
      - High 444 Predictive profile (High 444 预测档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_10_INTRA``
      - High 10 Intra profile (High 10 帧内档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_422_INTRA``
      - High 422 Intra profile (High 422 帧内档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_HIGH_444_INTRA``
      - High 444 Intra profile (High 444 帧内档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_CAVLC_444_INTRA``
      - CAVLC 444 Intra profile (CAVLC 444 帧内档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_BASELINE``
      - Scalable Baseline profile (可伸缩基线档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_HIGH``
      - Scalable High profile (可伸缩高级档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_SCALABLE_HIGH_INTRA``
      - Scalable High Intra profile (可伸缩高级帧内档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_STEREO_HIGH``
      - Stereo High profile (立体声高级档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_MULTIVIEW_HIGH``
      - Multiview High profile (多视图高级档次)
    * - ``V4L2_MPEG_VIDEO_H264_PROFILE_CONSTRAINED_HIGH``
      - Constrained High profile (受限高级档次)

.. raw:: latex

    \normalsize

.. _v4l2-mpeg-video-mpeg2-profile:

``V4L2_CID_MPEG_VIDEO_MPEG2_PROFILE`` (MPEG2 档次)
    (enum)

enum v4l2_mpeg_video_mpeg2_profile -
    The profile information for MPEG2. Applicable to MPEG2 codecs.
    Possible values are:
    (MPEG2 的档次信息。适用于 MPEG2 编解码器。可能的取值有：)

.. raw:: latex

    \small

.. tabularcolumns:: |p{10.2cm}|p{7.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_MPEG2_PROFILE_SIMPLE``
      - Simple profile (SP) (简单档次 (SP))
    * - ``V4L2_MPEG_VIDEO_MPEG2_PROFILE_MAIN``
      - Main profile (MP) (主档次 (MP))
    * - ``V4L2_MPEG_VIDEO_MPEG2_PROFILE_SNR_SCALABLE``
      - SNR Scalable profile (SNR) (SNR 可伸缩档次 (SNR))
    * - ``V4L2_MPEG_VIDEO_MPEG2_PROFILE_SPATIALLY_SCALABLE``
      - Spatially Scalable profile (Spt) (空间可伸缩档次 (Spt))
    * - ``V4L2_MPEG_VIDEO_MPEG2_PROFILE_HIGH``
      - High profile (HP) (高级档次 (HP))
    * - ``V4L2_MPEG_VIDEO_MPEG2_PROFILE_MULTIVIEW``
      - Multi-view profile (MVP) (多视图档次 (MVP))


.. raw:: latex

    \normalsize

.. _v4l2-mpeg-video-mpeg4-profile:

``V4L2_CID_MPEG_VIDEO_MPEG4_PROFILE`` (MPEG4 档次)
    (enum)

enum v4l2_mpeg_video_mpeg4_profile -
    The profile information for MPEG4. Applicable to the MPEG4 encoder.
    Possible values are:
    (MPEG4 的档次信息。适用于 MPEG4 编码器。可能的取值有：)

.. raw:: latex

    \small

.. tabularcolumns:: |p{11.8cm}|p{5.7cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_MPEG4_PROFILE_SIMPLE``
      - Simple profile (简单档次)
    * - ``V4L2_MPEG_VIDEO_MPEG4_PROFILE_ADVANCED_SIMPLE``
      - Advanced Simple profile (高级简单档次)
    * - ``V4L2_MPEG_VIDEO_MPEG4_PROFILE_CORE``
      - Core profile (核心档次)
    * - ``V4L2_MPEG_VIDEO_MPEG4_PROFILE_SIMPLE_SCALABLE``
      - Simple Scalable profile (简单可伸缩档次)
    * - ``V4L2_MPEG_VIDEO_MPEG4_PROFILE_ADVANCED_CODING_EFFICIENCY``
      - Advanced Coding Efficiency profile (高级编码效率档次)

.. raw:: latex

    \normalsize

``V4L2_CID_MPEG_VIDEO_MAX_REF_PIC (integer)`` (最大参考图像数)
    The maximum number of reference pictures used for encoding.
    Applicable to the encoder.
    (编码所使用的最大参考图像数量。适用于编码器。)

.. _v4l2-mpeg-video-multi-slice-mode:

``V4L2_CID_MPEG_VIDEO_MULTI_SLICE_MODE`` (多切片模式)
    (enum)

enum v4l2_mpeg_video_multi_slice_mode -
    Determines how the encoder should handle division of frame into
    slices. Applicable to the encoder. Possible values are:
    (决定编码器应如何处理帧到切片的划分。适用于编码器。可能的取值有：)



.. tabularcolumns:: |p{9.6cm}|p{7.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_SINGLE``
      - Single slice per frame. (每帧单个切片。)
    * - ``V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_MB``
      - Multiple slices with set maximum number of macroblocks per slice.
        (多个切片，每个切片设置最大宏块数。)
    * - ``V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_BYTES``
      - Multiple slice with set maximum size in bytes per slice.
        (多个切片，每个切片设置最大字节大小。)



``V4L2_CID_MPEG_VIDEO_MULTI_SLICE_MAX_MB (integer)`` (每切片最大宏块数)
    The maximum number of macroblocks in a slice. Used when
    ``V4L2_CID_MPEG_VIDEO_MULTI_SLICE_MODE`` is set to
    ``V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_MB``. Applicable to the
    encoder.
    (切片中宏块的最大数量。当 ``V4L2_CID_MPEG_VIDEO_MULTI_SLICE_MODE`` 设置
    为 ``V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_MB`` 时使用。适用于编码器。)

``V4L2_CID_MPEG_VIDEO_MULTI_SLICE_MAX_BYTES (integer)`` (每切片最大字节数)
    The maximum size of a slice in bytes. Used when
    ``V4L2_CID_MPEG_VIDEO_MULTI_SLICE_MODE`` is set to
    ``V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_BYTES``. Applicable to the
    encoder.
    (切片的最大字节大小。当 ``V4L2_CID_MPEG_VIDEO_MULTI_SLICE_MODE`` 设置为
    ``V4L2_MPEG_VIDEO_MULTI_SLICE_MODE_MAX_BYTES`` 时使用。适用于编码器。)

.. _v4l2-mpeg-video-h264-loop-filter-mode:

``V4L2_CID_MPEG_VIDEO_H264_LOOP_FILTER_MODE`` (H264 环路滤波模式)
    (enum)

enum v4l2_mpeg_video_h264_loop_filter_mode -
    Loop filter mode for H264 encoder. Possible values are:
    (H264 编码器的环路滤波模式。可能的取值有：)

.. raw:: latex

    \small

.. tabularcolumns:: |p{13.5cm}|p{4.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_LOOP_FILTER_MODE_ENABLED``
      - Loop filter is enabled. (环路滤波已启用。)
    * - ``V4L2_MPEG_VIDEO_H264_LOOP_FILTER_MODE_DISABLED``
      - Loop filter is disabled. (环路滤波被禁用。)
    * - ``V4L2_MPEG_VIDEO_H264_LOOP_FILTER_MODE_DISABLED_AT_SLICE_BOUNDARY``
      - Loop filter is disabled at the slice boundary.
        (在切片边界处禁用环路滤波。)

.. raw:: latex

    \normalsize


``V4L2_CID_MPEG_VIDEO_H264_LOOP_FILTER_ALPHA (integer)`` (环路滤波 alpha 系数)
    Loop filter alpha coefficient, defined in the H264 standard.
    This value corresponds to the slice_alpha_c0_offset_div2 slice header
    field, and should be in the range of -6 to +6, inclusive. The actual alpha
    offset FilterOffsetA is twice this value.
    Applicable to the H264 encoder.
    (环路滤波 alpha 系数，在 H264 标准中定义。此值对应切片头的
    slice_alpha_c0_offset_div2 字段，应在 -6 到 +6（含）范围内。实际的 alpha
    偏移 FilterOffsetA 是此值的两倍。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_LOOP_FILTER_BETA (integer)`` (环路滤波 beta 系数)
    Loop filter beta coefficient, defined in the H264 standard.
    This corresponds to the slice_beta_offset_div2 slice header field, and
    should be in the range of -6 to +6, inclusive. The actual beta offset
    FilterOffsetB is twice this value.
    Applicable to the H264 encoder.
    (环路滤波 beta 系数，在 H264 标准中定义。它对应切片头的
    slice_beta_offset_div2 字段，应在 -6 到 +6（含）范围内。实际的 beta 偏移
    FilterOffsetB 是此值的两倍。适用于 H264 编码器。)

.. _v4l2-mpeg-video-h264-entropy-mode:

``V4L2_CID_MPEG_VIDEO_H264_ENTROPY_MODE`` (熵编码模式)
    (enum)

enum v4l2_mpeg_video_h264_entropy_mode -
    Entropy coding mode for H264 - CABAC/CAVALC. Applicable to the H264
    encoder. Possible values are:
    (H264 的熵编码模式 - CABAC/CAVLC。适用于 H264 编码器。可能的取值有：)


.. tabularcolumns:: |p{9.0cm}|p{8.5cm}|


.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_ENTROPY_MODE_CAVLC``
      - Use CAVLC entropy coding. (使用 CAVLC 熵编码。)
    * - ``V4L2_MPEG_VIDEO_H264_ENTROPY_MODE_CABAC``
      - Use CABAC entropy coding. (使用 CABAC 熵编码。)



``V4L2_CID_MPEG_VIDEO_H264_8X8_TRANSFORM (boolean)`` (8X8 变换)
    Enable 8X8 transform for H264. Applicable to the H264 encoder.
    (为 H264 启用 8X8 变换。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_CONSTRAINED_INTRA_PREDICTION (boolean)`` (受限帧内预测)
    Enable constrained intra prediction for H264. Applicable to the H264
    encoder.
    (为 H264 启用受限帧内预测。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_CHROMA_QP_INDEX_OFFSET (integer)`` (色度 QP 索引偏移)
    Specify the offset that should be added to the luma quantization
    parameter to determine the chroma quantization parameter. Applicable
    to the H264 encoder.
    (指定应加到亮度量化参数上以确定色度量化参数的偏移量。适用于 H264
    编码器。)

``V4L2_CID_MPEG_VIDEO_CYCLIC_INTRA_REFRESH_MB (integer)`` (循环帧内刷新宏块数)
    Cyclic intra macroblock refresh. This is the number of continuous
    macroblocks refreshed every frame. Each frame a successive set of
    macroblocks is refreshed until the cycle completes and starts from
    the top of the frame. Setting this control to zero means that
    macroblocks will not be refreshed.  Note that this control will not
    take effect when ``V4L2_CID_MPEG_VIDEO_INTRA_REFRESH_PERIOD`` control
    is set to non zero value.
    Applicable to H264, H263 and MPEG4 encoder.
    (循环帧内宏块刷新。这是每帧刷新的连续宏块数量。每帧刷新一组接续的宏块，
    直到该循环完成并从帧顶部重新开始。将此控制设置为 0 表示不刷新宏块。注意，
    当 ``V4L2_CID_MPEG_VIDEO_INTRA_REFRESH_PERIOD`` 控制设置为非零值时，此
    控制不会生效。适用于 H264、H263 和 MPEG4 编码器。)

``V4L2_CID_MPEG_VIDEO_INTRA_REFRESH_PERIOD_TYPE (enum)`` (帧内刷新周期类型)

enum v4l2_mpeg_video_intra_refresh_period_type -
    Sets the type of intra refresh. The period to refresh
    the whole frame is specified by V4L2_CID_MPEG_VIDEO_INTRA_REFRESH_PERIOD.
    Note that if this control is not present, then it is undefined what
    refresh type is used and it is up to the driver to decide.
    Applicable to H264 and HEVC encoders. Possible values are:
    (设置帧内刷新的类型。刷新整帧的周期由
    V4L2_CID_MPEG_VIDEO_INTRA_REFRESH_PERIOD 指定。注意，如果此控制不存在，
    则使用何种刷新类型是未定义的，由驱动程序自行决定。适用于 H264 和 HEVC
    编码器。可能的取值有：)

.. tabularcolumns:: |p{9.6cm}|p{7.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_INTRA_REFRESH_PERIOD_TYPE_RANDOM``
      - The whole frame is completely refreshed randomly
        after the specified period.
        (在指定周期后，整帧被随机地完全刷新。)
    * - ``V4L2_MPEG_VIDEO_INTRA_REFRESH_PERIOD_TYPE_CYCLIC``
      - The whole frame MBs are completely refreshed in cyclic order
        after the specified period.
        (在指定周期后，整帧的宏块按循环顺序被完全刷新。)

``V4L2_CID_MPEG_VIDEO_INTRA_REFRESH_PERIOD (integer)`` (帧内刷新周期)
    Intra macroblock refresh period. This sets the period to refresh
    the whole frame. In other words, this defines the number of frames
    for which the whole frame will be intra-refreshed.  An example:
    setting period to 1 means that the whole frame will be refreshed,
    setting period to 2 means that the half of macroblocks will be
    intra-refreshed on frameX and the other half of macroblocks
    will be refreshed in frameX + 1 and so on. Setting the period to
    zero means no period is specified.
    Note that if the client sets this control to non zero value the
    ``V4L2_CID_MPEG_VIDEO_CYCLIC_INTRA_REFRESH_MB`` control shall be
    ignored. Applicable to H264 and HEVC encoders.
    (帧内宏块刷新周期。它设置刷新整帧的周期。换句话说，它定义了整帧将在多少
    帧内被帧内刷新。举例来说：周期设为 1 表示整帧将被刷新；周期设为 2 表示
    一半宏块在第 X 帧被帧内刷新，另一半宏块在第 X + 1 帧被刷新，依此类推。
    周期设为 0 表示未指定周期。注意，如果客户端将此控制设置为非零值，
    ``V4L2_CID_MPEG_VIDEO_CYCLIC_INTRA_REFRESH_MB`` 控制将被忽略。适用于
    H264 和 HEVC 编码器。)

``V4L2_CID_MPEG_VIDEO_FRAME_RC_ENABLE (boolean)`` (帧级码率控制)
    Frame level rate control enable. If this control is disabled then
    the quantization parameter for each frame type is constant and set
    with appropriate controls (e.g.
    ``V4L2_CID_MPEG_VIDEO_H263_I_FRAME_QP``). If frame rate control is
    enabled then quantization parameter is adjusted to meet the chosen
    bitrate. Minimum and maximum value for the quantization parameter
    can be set with appropriate controls (e.g.
    ``V4L2_CID_MPEG_VIDEO_H263_MIN_QP``). Applicable to encoders.
    (帧级码率控制启用。如果禁用此控制，则每种帧类型的量化参数是恒定的，由
    相应的控制设置（例如 ``V4L2_CID_MPEG_VIDEO_H263_I_FRAME_QP``）。如果
    启用帧级码率控制，则量化参数会被调整以满足选定的比特率。量化参数的最小
    值和最大值可以用相应的控制设置（例如
    ``V4L2_CID_MPEG_VIDEO_H263_MIN_QP``）。适用于编码器。)

``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE (boolean)`` (宏块级码率控制)
    Macroblock level rate control enable. Applicable to the MPEG4 and
    H264 encoders.
    (宏块级码率控制启用。适用于 MPEG4 和 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_MPEG4_QPEL (boolean)`` (四分之一像素运动估计)
    Quarter pixel motion estimation for MPEG4. Applicable to the MPEG4
    encoder.
    (MPEG4 的四分之一像素运动估计。适用于 MPEG4 编码器。)

``V4L2_CID_MPEG_VIDEO_H263_I_FRAME_QP (integer)`` (H263 I 帧 QP)
    Quantization parameter for an I frame for H263. Valid range: from 1
    to 31.
    (H263 I 帧的量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_H263_MIN_QP (integer)`` (H263 最小 QP)
    Minimum quantization parameter for H263. Valid range: from 1 to 31.
    (H263 的最小量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_H263_MAX_QP (integer)`` (H263 最大 QP)
    Maximum quantization parameter for H263. Valid range: from 1 to 31.
    (H263 的最大量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_H263_P_FRAME_QP (integer)`` (H263 P 帧 QP)
    Quantization parameter for an P frame for H263. Valid range: from 1
    to 31.
    (H263 P 帧的量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_H263_B_FRAME_QP (integer)`` (H263 B 帧 QP)
    Quantization parameter for an B frame for H263. Valid range: from 1
    to 31.
    (H263 B 帧的量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_H264_I_FRAME_QP (integer)`` (H264 I 帧 QP)
    Quantization parameter for an I frame for H264. Valid range: from 0
    to 51.
    (H264 I 帧的量化参数。有效范围：0 到 51。)

``V4L2_CID_MPEG_VIDEO_H264_MIN_QP (integer)`` (H264 最小 QP)
    Minimum quantization parameter for H264. Valid range: from 0 to 51.
    (H264 的最小量化参数。有效范围：0 到 51。)

``V4L2_CID_MPEG_VIDEO_H264_MAX_QP (integer)`` (H264 最大 QP)
    Maximum quantization parameter for H264. Valid range: from 0 to 51.
    (H264 的最大量化参数。有效范围：0 到 51。)

``V4L2_CID_MPEG_VIDEO_H264_P_FRAME_QP (integer)`` (H264 P 帧 QP)
    Quantization parameter for an P frame for H264. Valid range: from 0
    to 51.
    (H264 P 帧的量化参数。有效范围：0 到 51。)

``V4L2_CID_MPEG_VIDEO_H264_B_FRAME_QP (integer)`` (H264 B 帧 QP)
    Quantization parameter for an B frame for H264. Valid range: from 0
    to 51.
    (H264 B 帧的量化参数。有效范围：0 到 51。)

``V4L2_CID_MPEG_VIDEO_H264_I_FRAME_MIN_QP (integer)`` (H264 I 帧最小 QP)
    Minimum quantization parameter for the H264 I frame to limit I frame
    quality to a range. Valid range: from 0 to 51. If
    V4L2_CID_MPEG_VIDEO_H264_MIN_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (H264 I 帧的最小量化参数，用于将 I 帧质量限制在一定范围内。有效范围：
    0 到 51。如果同时设置了 V4L2_CID_MPEG_VIDEO_H264_MIN_QP，则应选择满足
    两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_H264_I_FRAME_MAX_QP (integer)`` (H264 I 帧最大 QP)
    Maximum quantization parameter for the H264 I frame to limit I frame
    quality to a range. Valid range: from 0 to 51. If
    V4L2_CID_MPEG_VIDEO_H264_MAX_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (H264 I 帧的最大量化参数，用于将 I 帧质量限制在一定范围内。有效范围：
    0 到 51。如果同时设置了 V4L2_CID_MPEG_VIDEO_H264_MAX_QP，则应选择满足
    两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_H264_P_FRAME_MIN_QP (integer)`` (H264 P 帧最小 QP)
    Minimum quantization parameter for the H264 P frame to limit P frame
    quality to a range. Valid range: from 0 to 51. If
    V4L2_CID_MPEG_VIDEO_H264_MIN_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (H264 P 帧的最小量化参数，用于将 P 帧质量限制在一定范围内。有效范围：
    0 到 51。如果同时设置了 V4L2_CID_MPEG_VIDEO_H264_MIN_QP，则应选择满足
    两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_H264_P_FRAME_MAX_QP (integer)`` (H264 P 帧最大 QP)
    Maximum quantization parameter for the H264 P frame to limit P frame
    quality to a range. Valid range: from 0 to 51. If
    V4L2_CID_MPEG_VIDEO_H264_MAX_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (H264 P 帧的最大量化参数，用于将 P 帧质量限制在一定范围内。有效范围：
    0 到 51。如果同时设置了 V4L2_CID_MPEG_VIDEO_H264_MAX_QP，则应选择满足
    两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_H264_B_FRAME_MIN_QP (integer)`` (H264 B 帧最小 QP)
    Minimum quantization parameter for the H264 B frame to limit B frame
    quality to a range. Valid range: from 0 to 51. If
    V4L2_CID_MPEG_VIDEO_H264_MIN_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (H264 B 帧的最小量化参数，用于将 B 帧质量限制在一定范围内。有效范围：
    0 到 51。如果同时设置了 V4L2_CID_MPEG_VIDEO_H264_MIN_QP，则应选择满足
    两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_H264_B_FRAME_MAX_QP (integer)`` (H264 B 帧最大 QP)
    Maximum quantization parameter for the H264 B frame to limit B frame
    quality to a range. Valid range: from 0 to 51. If
    V4L2_CID_MPEG_VIDEO_H264_MAX_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (H264 B 帧的最大量化参数，用于将 B 帧质量限制在一定范围内。有效范围：
    0 到 51。如果同时设置了 V4L2_CID_MPEG_VIDEO_H264_MAX_QP，则应选择满足
    两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_MPEG4_I_FRAME_QP (integer)`` (MPEG4 I 帧 QP)
    Quantization parameter for an I frame for MPEG4. Valid range: from 1
    to 31.
    (MPEG4 I 帧的量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_MPEG4_MIN_QP (integer)`` (MPEG4 最小 QP)
    Minimum quantization parameter for MPEG4. Valid range: from 1 to 31.
    (MPEG4 的最小量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_MPEG4_MAX_QP (integer)`` (MPEG4 最大 QP)
    Maximum quantization parameter for MPEG4. Valid range: from 1 to 31.
    (MPEG4 的最大量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_MPEG4_P_FRAME_QP (integer)`` (MPEG4 P 帧 QP)
    Quantization parameter for an P frame for MPEG4. Valid range: from 1
    to 31.
    (MPEG4 P 帧的量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_MPEG4_B_FRAME_QP (integer)`` (MPEG4 B 帧 QP)
    Quantization parameter for an B frame for MPEG4. Valid range: from 1
    to 31.
    (MPEG4 B 帧的量化参数。有效范围：1 到 31。)

.. _v4l2-mpeg-video-vbv-size:

``V4L2_CID_MPEG_VIDEO_VBV_SIZE (integer)`` (VBV 大小)
    The Video Buffer Verifier size in kilobytes, it is used as a
    limitation of frame skip. The VBV is defined in the standard as a
    mean to verify that the produced stream will be successfully
    decoded. The standard describes it as "Part of a hypothetical
    decoder that is conceptually connected to the output of the encoder.
    Its purpose is to provide a constraint on the variability of the
    data rate that an encoder or editing process may produce.".
    Applicable to the MPEG1, MPEG2, MPEG4 encoders.
    (视频缓冲校验器 (Video Buffer Verifier) 大小，单位为千字节，用作跳帧的
    限制。标准将 VBV 定义为验证所产生的流能否被成功解码的一种手段。标准对
    它的描述是："概念上连接到编码器输出的假想解码器的一部分。其目的是对编码
    器或编辑过程可能产生的数据速率变化提供约束。"。适用于 MPEG1、MPEG2、
    MPEG4 编码器。)

.. _v4l2-mpeg-video-vbv-delay:

``V4L2_CID_MPEG_VIDEO_VBV_DELAY (integer)`` (VBV 初始延迟)
    Sets the initial delay in milliseconds for VBV buffer control.
    (以毫秒为单位设置 VBV 缓冲控制的初始延迟。)

.. _v4l2-mpeg-video-hor-search-range:

``V4L2_CID_MPEG_VIDEO_MV_H_SEARCH_RANGE (integer)`` (水平搜索范围)
    Horizontal search range defines maximum horizontal search area in
    pixels to search and match for the present Macroblock (MB) in the
    reference picture. This V4L2 control macro is used to set horizontal
    search range for motion estimation module in video encoder.
    (水平搜索范围定义了在参考图像中搜索并匹配当前宏块 (MB) 的最大水平搜索
    区域（以像素为单位）。此 V4L2 控制宏用于设置视频编码器运动估计模块的水平
    搜索范围。)

.. _v4l2-mpeg-video-vert-search-range:

``V4L2_CID_MPEG_VIDEO_MV_V_SEARCH_RANGE (integer)`` (垂直搜索范围)
    Vertical search range defines maximum vertical search area in pixels
    to search and match for the present Macroblock (MB) in the reference
    picture. This V4L2 control macro is used to set vertical search
    range for motion estimation module in video encoder.
    (垂直搜索范围定义了在参考图像中搜索并匹配当前宏块 (MB) 的最大垂直搜索
    区域（以像素为单位）。此 V4L2 控制宏用于设置视频编码器运动估计模块的
    垂直搜索范围。)

.. _v4l2-mpeg-video-force-key-frame:

``V4L2_CID_MPEG_VIDEO_FORCE_KEY_FRAME (button)`` (强制关键帧)
    Force a key frame for the next queued buffer. Applicable to
    encoders. This is a general, codec-agnostic keyframe control.
    (为下一个入队缓冲区强制产生关键帧。适用于编码器。这是一个通用的、与
    编解码器无关的关键帧控制。)

.. _v4l2-mpeg-video-h264-cpb-size:

``V4L2_CID_MPEG_VIDEO_H264_CPB_SIZE (integer)`` (CPB 大小)
    The Coded Picture Buffer size in kilobytes, it is used as a
    limitation of frame skip. The CPB is defined in the H264 standard as
    a mean to verify that the produced stream will be successfully
    decoded. Applicable to the H264 encoder.
    (编码图像缓冲区 (Coded Picture Buffer) 大小，单位为千字节，用作跳帧的
    限制。H264 标准将 CPB 定义为验证所产生的流能否被成功解码的一种手段。
    适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_I_PERIOD (integer)`` (开放 GOP I 帧周期)
    Period between I-frames in the open GOP for H264. In case of an open
    GOP this is the period between two I-frames. The period between IDR
    (Instantaneous Decoding Refresh) frames is taken from the GOP_SIZE
    control. An IDR frame, which stands for Instantaneous Decoding
    Refresh is an I-frame after which no prior frames are referenced.
    This means that a stream can be restarted from an IDR frame without
    the need to store or decode any previous frames. Applicable to the
    H264 encoder.
    (H264 开放 GOP 中 I 帧之间的周期。对于开放 GOP，这是两个 I 帧之间的
    周期。IDR（Instantaneous Decoding Refresh，即时解码刷新）帧之间的周期
    取自 GOP_SIZE 控制。IDR 帧即即时解码刷新帧，是一种在其后不再引用任何
    先前帧的 I 帧。这意味着流可以从 IDR 帧重新开始，而无需存储或解码任何
    先前的帧。适用于 H264 编码器。)

.. _v4l2-mpeg-video-header-mode:

``V4L2_CID_MPEG_VIDEO_HEADER_MODE`` (头模式)
    (enum)

enum v4l2_mpeg_video_header_mode -
    Determines whether the header is returned as the first buffer or is
    it returned together with the first frame. Applicable to encoders.
    Possible values are:
    (决定头是作为第一个缓冲区返回，还是与第一帧一起返回。适用于编码器。
    可能的取值有：)

.. raw:: latex

    \small

.. tabularcolumns:: |p{10.3cm}|p{7.2cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_HEADER_MODE_SEPARATE``
      - The stream header is returned separately in the first buffer.
        (流头在第一个缓冲区中单独返回。)
    * - ``V4L2_MPEG_VIDEO_HEADER_MODE_JOINED_WITH_1ST_FRAME``
      - The stream header is returned together with the first encoded
	frame.
        (流头与第一个编码帧一起返回。)

.. raw:: latex

    \normalsize


``V4L2_CID_MPEG_VIDEO_REPEAT_SEQ_HEADER (boolean)`` (重复序列头)
    Repeat the video sequence headers. Repeating these headers makes
    random access to the video stream easier. Applicable to the MPEG1, 2
    and 4 encoder.
    (重复视频序列头。重复这些头使得对视频流的随机访问更容易。适用于 MPEG1、
    MPEG2 和 MPEG4 编码器。)

``V4L2_CID_MPEG_VIDEO_DECODER_MPEG4_DEBLOCK_FILTER (boolean)`` (MPEG4 去块滤波)
    Enabled the deblocking post processing filter for MPEG4 decoder.
    Applicable to the MPEG4 decoder.
    (为 MPEG4 解码器启用去块后处理滤波。适用于 MPEG4 解码器。)

``V4L2_CID_MPEG_VIDEO_MPEG4_VOP_TIME_RES (integer)`` (VOP 时间分辨率)
    vop_time_increment_resolution value for MPEG4. Applicable to the
    MPEG4 encoder.
    (MPEG4 的 vop_time_increment_resolution 值。适用于 MPEG4 编码器。)

``V4L2_CID_MPEG_VIDEO_MPEG4_VOP_TIME_INC (integer)`` (VOP 时间增量)
    vop_time_increment value for MPEG4. Applicable to the MPEG4
    encoder.
    (MPEG4 的 vop_time_increment 值。适用于 MPEG4 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_SEI_FRAME_PACKING (boolean)`` (帧打包 SEI)
    Enable generation of frame packing supplemental enhancement
    information in the encoded bitstream. The frame packing SEI message
    contains the arrangement of L and R planes for 3D viewing.
    Applicable to the H264 encoder.
    (在编码比特流中启用帧打包补充增强信息 (SEI) 的生成。帧打包 SEI 消息包含
    用于 3D 观看的 L 和 R 平面的排列方式。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_SEI_FP_CURRENT_FRAME_0 (boolean)`` (当前帧为 frame0)
    Sets current frame as frame0 in frame packing SEI. Applicable to the
    H264 encoder.
    (在帧打包 SEI 中将当前帧设置为 frame0。适用于 H264 编码器。)

.. _v4l2-mpeg-video-h264-sei-fp-arrangement-type:

``V4L2_CID_MPEG_VIDEO_H264_SEI_FP_ARRANGEMENT_TYPE`` (帧打包排列类型)
    (enum)

enum v4l2_mpeg_video_h264_sei_fp_arrangement_type -
    Frame packing arrangement type for H264 SEI. Applicable to the H264
    encoder. Possible values are:
    (H264 SEI 的帧打包排列类型。适用于 H264 编码器。可能的取值有：)

.. raw:: latex

    \small

.. tabularcolumns:: |p{12cm}|p{5.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_SEI_FP_ARRANGEMENT_TYPE_CHEKERBOARD``
      - Pixels are alternatively from L and R.
        (像素交替取自 L 和 R。)
    * - ``V4L2_MPEG_VIDEO_H264_SEI_FP_ARRANGEMENT_TYPE_COLUMN``
      - L and R are interlaced by column.
        (L 和 R 按列交错。)
    * - ``V4L2_MPEG_VIDEO_H264_SEI_FP_ARRANGEMENT_TYPE_ROW``
      - L and R are interlaced by row.
        (L 和 R 按行交错。)
    * - ``V4L2_MPEG_VIDEO_H264_SEI_FP_ARRANGEMENT_TYPE_SIDE_BY_SIDE``
      - L is on the left, R on the right.
        (L 在左，R 在右。)
    * - ``V4L2_MPEG_VIDEO_H264_SEI_FP_ARRANGEMENT_TYPE_TOP_BOTTOM``
      - L is on top, R on bottom.
        (L 在上，R 在下。)
    * - ``V4L2_MPEG_VIDEO_H264_SEI_FP_ARRANGEMENT_TYPE_TEMPORAL``
      - One view per frame.
        (每帧一个视图。)

.. raw:: latex

    \normalsize



``V4L2_CID_MPEG_VIDEO_H264_FMO (boolean)`` (灵活宏块排序)
    Enables flexible macroblock ordering in the encoded bitstream. It is
    a technique used for restructuring the ordering of macroblocks in
    pictures. Applicable to the H264 encoder.
    (在编码比特流中启用灵活宏块排序 (FMO)。这是一种用于重构图像中宏块顺序的
    技术。适用于 H264 编码器。)

.. _v4l2-mpeg-video-h264-fmo-map-type:

``V4L2_CID_MPEG_VIDEO_H264_FMO_MAP_TYPE`` (FMO 映射类型)
   (enum)

enum v4l2_mpeg_video_h264_fmo_map_type -
    When using FMO, the map type divides the image in different scan
    patterns of macroblocks. Applicable to the H264 encoder. Possible
    values are:
    (使用 FMO 时，映射类型将图像划分为宏块的不同扫描模式。适用于 H264
    编码器。可能的取值有：)

.. raw:: latex

    \small

.. tabularcolumns:: |p{12.5cm}|p{5.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_FMO_MAP_TYPE_INTERLEAVED_SLICES``
      - Slices are interleaved one after other with macroblocks in run
	length order.
        (切片按宏块的行程长度顺序依次交错。)
    * - ``V4L2_MPEG_VIDEO_H264_FMO_MAP_TYPE_SCATTERED_SLICES``
      - Scatters the macroblocks based on a mathematical function known to
	both encoder and decoder.
        (基于编码器和解码器都已知的一个数学函数分散宏块。)
    * - ``V4L2_MPEG_VIDEO_H264_FMO_MAP_TYPE_FOREGROUND_WITH_LEFT_OVER``
      - Macroblocks arranged in rectangular areas or regions of interest.
        (宏块排列在矩形区域或感兴趣区域中。)
    * - ``V4L2_MPEG_VIDEO_H264_FMO_MAP_TYPE_BOX_OUT``
      - Slice groups grow in a cyclic way from centre to outwards.
        (切片组从中心向外以环形方式扩展。)
    * - ``V4L2_MPEG_VIDEO_H264_FMO_MAP_TYPE_RASTER_SCAN``
      - Slice groups grow in raster scan pattern from left to right.
        (切片组按从左到右的光栅扫描模式扩展。)
    * - ``V4L2_MPEG_VIDEO_H264_FMO_MAP_TYPE_WIPE_SCAN``
      - Slice groups grow in wipe scan pattern from top to bottom.
        (切片组按从上到下的擦除扫描模式扩展。)
    * - ``V4L2_MPEG_VIDEO_H264_FMO_MAP_TYPE_EXPLICIT``
      - User defined map type.
        (用户定义的映射类型。)

.. raw:: latex

    \normalsize



``V4L2_CID_MPEG_VIDEO_H264_FMO_SLICE_GROUP (integer)`` (FMO 切片组数)
    Number of slice groups in FMO. Applicable to the H264 encoder.
    (FMO 中切片组的数量。适用于 H264 编码器。)

.. _v4l2-mpeg-video-h264-fmo-change-direction:

``V4L2_CID_MPEG_VIDEO_H264_FMO_CHANGE_DIRECTION`` (FMO 变化方向)
    (enum)

enum v4l2_mpeg_video_h264_fmo_change_dir -
    Specifies a direction of the slice group change for raster and wipe
    maps. Applicable to the H264 encoder. Possible values are:
    (为光栅和擦除映射指定切片组变化的方向。适用于 H264 编码器。可能的取值
    有：)

.. tabularcolumns:: |p{9.6cm}|p{7.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_FMO_CHANGE_DIR_RIGHT``
      - Raster scan or wipe right. (光栅扫描或向右擦除。)
    * - ``V4L2_MPEG_VIDEO_H264_FMO_CHANGE_DIR_LEFT``
      - Reverse raster scan or wipe left. (反向光栅扫描或向左擦除。)



``V4L2_CID_MPEG_VIDEO_H264_FMO_CHANGE_RATE (integer)`` (FMO 变化速率)
    Specifies the size of the first slice group for raster and wipe map.
    Applicable to the H264 encoder.
    (为光栅和擦除映射指定第一个切片组的大小。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_FMO_RUN_LENGTH (integer)`` (FMO 行程长度)
    Specifies the number of consecutive macroblocks for the interleaved
    map. Applicable to the H264 encoder.
    (为交错映射指定连续宏块的数量。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_ASO (boolean)`` (任意切片顺序)
    Enables arbitrary slice ordering in encoded bitstream. Applicable to
    the H264 encoder.
    (在编码比特流中启用任意切片顺序 (ASO)。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_ASO_SLICE_ORDER (integer)`` (ASO 切片顺序)
    Specifies the slice order in ASO. Applicable to the H264 encoder.
    The supplied 32-bit integer is interpreted as follows (bit 0 = least
    significant bit):
    (指定 ASO 中的切片顺序。适用于 H264 编码器。所提供的 32 位整数按如下
    方式解释（位 0 = 最低有效位）：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - Bit 0:15
      - Slice ID (切片 ID)
    * - Bit 16:32
      - Slice position or order (切片位置或顺序)



``V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING (boolean)`` (层次编码)
    Enables H264 hierarchical coding. Applicable to the H264 encoder.
    (启用 H264 层次编码。适用于 H264 编码器。)

.. _v4l2-mpeg-video-h264-hierarchical-coding-type:

``V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_TYPE`` (层次编码类型)
    (enum)

enum v4l2_mpeg_video_h264_hierarchical_coding_type -
    Specifies the hierarchical coding type. Applicable to the H264
    encoder. Possible values are:
    (指定层次编码类型。适用于 H264 编码器。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_H264_HIERARCHICAL_CODING_B``
      - Hierarchical B coding. (层次 B 编码。)
    * - ``V4L2_MPEG_VIDEO_H264_HIERARCHICAL_CODING_P``
      - Hierarchical P coding. (层次 P 编码。)



``V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER (integer)`` (层次编码层数)
    Specifies the number of hierarchical coding layers. Applicable to
    the H264 encoder.
    (指定层次编码的层数。适用于 H264 编码器。)

``V4L2_CID_MPEG_VIDEO_H264_HIERARCHICAL_CODING_LAYER_QP (integer)`` (各层 QP)
    Specifies a user defined QP for each layer. Applicable to the H264
    encoder. The supplied 32-bit integer is interpreted as follows (bit
    0 = least significant bit):
    (为每一层指定用户定义的 QP。适用于 H264 编码器。所提供的 32 位整数按
    如下方式解释（位 0 = 最低有效位）：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - Bit 0:15
      - QP value (QP 值)
    * - Bit 16:32
      - Layer number (层号)

``V4L2_CID_MPEG_VIDEO_H264_HIER_CODING_L0_BR (integer)`` (层 0 比特率)
    Indicates bit rate (bps) for hierarchical coding layer 0 for H264 encoder.
    (指示 H264 编码器层次编码层 0 的比特率 (bps)。)

``V4L2_CID_MPEG_VIDEO_H264_HIER_CODING_L1_BR (integer)`` (层 1 比特率)
    Indicates bit rate (bps) for hierarchical coding layer 1 for H264 encoder.
    (指示 H264 编码器层次编码层 1 的比特率 (bps)。)

``V4L2_CID_MPEG_VIDEO_H264_HIER_CODING_L2_BR (integer)`` (层 2 比特率)
    Indicates bit rate (bps) for hierarchical coding layer 2 for H264 encoder.
    (指示 H264 编码器层次编码层 2 的比特率 (bps)。)

``V4L2_CID_MPEG_VIDEO_H264_HIER_CODING_L3_BR (integer)`` (层 3 比特率)
    Indicates bit rate (bps) for hierarchical coding layer 3 for H264 encoder.
    (指示 H264 编码器层次编码层 3 的比特率 (bps)。)

``V4L2_CID_MPEG_VIDEO_H264_HIER_CODING_L4_BR (integer)`` (层 4 比特率)
    Indicates bit rate (bps) for hierarchical coding layer 4 for H264 encoder.
    (指示 H264 编码器层次编码层 4 的比特率 (bps)。)

``V4L2_CID_MPEG_VIDEO_H264_HIER_CODING_L5_BR (integer)`` (层 5 比特率)
    Indicates bit rate (bps) for hierarchical coding layer 5 for H264 encoder.
    (指示 H264 编码器层次编码层 5 的比特率 (bps)。)

``V4L2_CID_MPEG_VIDEO_H264_HIER_CODING_L6_BR (integer)`` (层 6 比特率)
    Indicates bit rate (bps) for hierarchical coding layer 6 for H264 encoder.
    (指示 H264 编码器层次编码层 6 的比特率 (bps)。)

``V4L2_CID_FWHT_I_FRAME_QP (integer)`` (FWHT I 帧 QP)
    Quantization parameter for an I frame for FWHT. Valid range: from 1
    to 31.
    (FWHT I 帧的量化参数。有效范围：1 到 31。)

``V4L2_CID_FWHT_P_FRAME_QP (integer)`` (FWHT P 帧 QP)
    Quantization parameter for a P frame for FWHT. Valid range: from 1
    to 31.
    (FWHT P 帧的量化参数。有效范围：1 到 31。)

``V4L2_CID_MPEG_VIDEO_AVERAGE_QP (integer)`` (平均 QP)
    This read-only control returns the average QP value of the currently
    encoded frame. The value applies to the last dequeued capture buffer
    (VIDIOC_DQBUF). Its valid range depends on the encoding format and parameters.
    For H264, its valid range is from 0 to 51.
    For HEVC, its valid range is from 0 to 51 for 8 bit and
    from 0 to 63 for 10 bit.
    For H263 and MPEG4, its valid range is from 1 to 31.
    For VP8, its valid range is from 0 to 127.
    For VP9, its valid range is from 0 to 255.
    If the codec's MIN_QP and MAX_QP are set, then the QP will meet both requirements.
    Codecs need to always use the specified range, rather then a HW custom range.
    Applicable to encoders
    (此只读控制返回当前编码帧的平均 QP 值。该值适用于最后一个出队的捕获
    缓冲区 (VIDIOC_DQBUF)。其有效范围取决于编码格式和参数。对 H264，有效
    范围为 0 到 51。对 HEVC，8 位时有效范围为 0 到 51，10 位时为 0 到 63。
    对 H263 和 MPEG4，有效范围为 1 到 31。对 VP8，有效范围为 0 到 127。
    对 VP9，有效范围为 0 到 255。如果设置了编解码器的 MIN_QP 和 MAX_QP，
    则 QP 将同时满足两个要求。编解码器必须始终使用指定的范围，而不是硬件
    自定义范围。适用于编码器。)

.. raw:: latex

    \normalsize


MFC 5.1 MPEG Controls(MFC 5.1 MPEG 控制)
========================================

The following MPEG class controls deal with MPEG decoding and encoding
settings that are specific to the Multi Format Codec 5.1 device present
in the S5P family of SoCs by Samsung.
(以下 MPEG 类控制处理三星 S5P 系列 SoC 中 Multi Format Codec 5.1 设备专有的
MPEG 解码和编码设置。)


.. _mfc51-control-id:

MFC 5.1 Control IDs(MFC 5.1 控制 ID)
------------------------------------

``V4L2_CID_MPEG_MFC51_VIDEO_DECODER_H264_DISPLAY_DELAY_ENABLE (boolean)`` (显示延迟启用)
    If the display delay is enabled then the decoder is forced to return
    a CAPTURE buffer (decoded frame) after processing a certain number
    of OUTPUT buffers. The delay can be set through
    ``V4L2_CID_MPEG_MFC51_VIDEO_DECODER_H264_DISPLAY_DELAY``. This
    feature can be used for example for generating thumbnails of videos.
    Applicable to the H264 decoder.
    (如果启用了显示延迟，则解码器在处理一定数量的 OUTPUT 缓冲区后被强制返回
    一个 CAPTURE 缓冲区（解码帧）。延迟可以通过
    ``V4L2_CID_MPEG_MFC51_VIDEO_DECODER_H264_DISPLAY_DELAY`` 设置。此功能
    可用于例如生成视频缩略图。适用于 H264 解码器。)

    .. note::

       This control is deprecated. Use the standard
       ``V4L2_CID_MPEG_VIDEO_DEC_DISPLAY_DELAY_ENABLE`` control instead.
       (此控制已弃用。请改用标准的
       ``V4L2_CID_MPEG_VIDEO_DEC_DISPLAY_DELAY_ENABLE`` 控制。)

``V4L2_CID_MPEG_MFC51_VIDEO_DECODER_H264_DISPLAY_DELAY (integer)`` (显示延迟)
    Display delay value for H264 decoder. The decoder is forced to
    return a decoded frame after the set 'display delay' number of
    frames. If this number is low it may result in frames returned out
    of display order, in addition the hardware may still be using the
    returned buffer as a reference picture for subsequent frames.
    (H264 解码器的显示延迟值。解码器在设定数量的"display delay"帧之后被
    强制返回一个解码帧。如果此数值较小，可能导致返回的帧不按显示顺序；此外，
    硬件可能仍在把返回的缓冲区用作后续帧的参考图像。)

    .. note::

       This control is deprecated. Use the standard
       ``V4L2_CID_MPEG_VIDEO_DEC_DISPLAY_DELAY`` control instead.
       (此控制已弃用。请改用标准的
       ``V4L2_CID_MPEG_VIDEO_DEC_DISPLAY_DELAY`` 控制。)

``V4L2_CID_MPEG_MFC51_VIDEO_H264_NUM_REF_PIC_FOR_P (integer)`` (P 图像参考数)
    The number of reference pictures used for encoding a P picture.
    Applicable to the H264 encoder.
    (编码 P 图像所使用的参考图片数量。适用于 H264 编码器。)

``V4L2_CID_MPEG_MFC51_VIDEO_PADDING (boolean)`` (填充启用)
    Padding enable in the encoder - use a color instead of repeating
    border pixels. Applicable to encoders.
    (编码器中的填充启用 - 使用一种颜色而不是重复边缘像素。适用于编码器。)

``V4L2_CID_MPEG_MFC51_VIDEO_PADDING_YUV (integer)`` (填充 YUV 颜色)
    Padding color in the encoder. Applicable to encoders. The supplied
    32-bit integer is interpreted as follows (bit 0 = least significant
    bit):
    (编码器中的填充颜色。适用于编码器。所提供的 32 位整数按如下方式解释
    （位 0 = 最低有效位）：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - Bit 0:7
      - V chrominance information (V 色度信息)
    * - Bit 8:15
      - U chrominance information (U 色度信息)
    * - Bit 16:23
      - Y luminance information (Y 亮度信息)
    * - Bit 24:31
      - Must be zero. (必须为 0。)



``V4L2_CID_MPEG_MFC51_VIDEO_RC_REACTION_COEFF (integer)`` (码率控制反应系数)
    Reaction coefficient for MFC rate control. Applicable to encoders.
    (MFC 码率控制的反应系数。适用于编码器。)

    .. note::

       #. Valid only when the frame level RC is enabled.
          (仅在启用帧级码率控制 (RC) 时有效。)

       #. For tight CBR, this field must be small (ex. 2 ~ 10). For
	  VBR, this field must be large (ex. 100 ~ 1000).
          (对于严格的 CBR，此字段必须较小（例如 2 ~ 10）。对于 VBR，此字段
          必须较大（例如 100 ~ 1000）。)

       #. It is not recommended to use the greater number than
	  FRAME_RATE * (10^9 / BIT_RATE).
          (不建议使用大于 FRAME_RATE * (10^9 / BIT_RATE) 的数值。)

``V4L2_CID_MPEG_MFC51_VIDEO_H264_ADAPTIVE_RC_DARK (boolean)`` (暗区自适应码率控制)
    Adaptive rate control for dark region. Valid only when H.264 and
    macroblock level RC is enabled
    (``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE``). Applicable to the H264
    encoder.
    (针对暗区的自适应码率控制。仅在 H.264 且宏块级码率控制 (RC) 已启用
    （``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE``）时有效。适用于 H264 编码器。)

``V4L2_CID_MPEG_MFC51_VIDEO_H264_ADAPTIVE_RC_SMOOTH (boolean)`` (平滑区自适应码率控制)
    Adaptive rate control for smooth region. Valid only when H.264 and
    macroblock level RC is enabled
    (``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE``). Applicable to the H264
    encoder.
    (针对平滑区的自适应码率控制。仅在 H.264 且宏块级码率控制 (RC) 已启用
    （``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE``）时有效。适用于 H264 编码器。)

``V4L2_CID_MPEG_MFC51_VIDEO_H264_ADAPTIVE_RC_STATIC (boolean)`` (静止区自适应码率控制)
    Adaptive rate control for static region. Valid only when H.264 and
    macroblock level RC is enabled
    (``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE``). Applicable to the H264
    encoder.
    (针对静止区的自适应码率控制。仅在 H.264 且宏块级码率控制 (RC) 已启用
    （``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE``）时有效。适用于 H264 编码器。)

``V4L2_CID_MPEG_MFC51_VIDEO_H264_ADAPTIVE_RC_ACTIVITY (boolean)`` (活动区自适应码率控制)
    Adaptive rate control for activity region. Valid only when H.264 and
    macroblock level RC is enabled
    (``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE``). Applicable to the H264
    encoder.
    (针对活动区的自适应码率控制。仅在 H.264 且宏块级码率控制 (RC) 已启用
    （``V4L2_CID_MPEG_VIDEO_MB_RC_ENABLE``）时有效。适用于 H264 编码器。)

.. _v4l2-mpeg-mfc51-video-frame-skip-mode:

``V4L2_CID_MPEG_MFC51_VIDEO_FRAME_SKIP_MODE`` (跳帧模式)
    (enum)

    .. note::

       This control is deprecated. Use the standard
       ``V4L2_CID_MPEG_VIDEO_FRAME_SKIP_MODE`` control instead.
       (此控制已弃用。请改用标准的
       ``V4L2_CID_MPEG_VIDEO_FRAME_SKIP_MODE`` 控制。)

enum v4l2_mpeg_mfc51_video_frame_skip_mode -
    Indicates in what conditions the encoder should skip frames. If
    encoding a frame would cause the encoded stream to be larger then a
    chosen data limit then the frame will be skipped. Possible values
    are:
    (指示编码器在什么条件下应跳过帧。如果编码一帧会导致编码后的流超过选定的
    数据上限，则该帧将被跳过。可能的取值有：)


.. tabularcolumns:: |p{9.4cm}|p{8.1cm}|

.. raw:: latex

    \small

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_MFC51_VIDEO_FRAME_SKIP_MODE_DISABLED``
      - Frame skip mode is disabled.
        (跳帧模式被禁用。)
    * - ``V4L2_MPEG_MFC51_VIDEO_FRAME_SKIP_MODE_LEVEL_LIMIT``
      - Frame skip mode enabled and buffer limit is set by the chosen
	level and is defined by the standard.
        (启用跳帧模式，缓冲区上限由所选级别设定，并由标准定义。)
    * - ``V4L2_MPEG_MFC51_VIDEO_FRAME_SKIP_MODE_BUF_LIMIT``
      - Frame skip mode enabled and buffer limit is set by the VBV
	(MPEG1/2/4) or CPB (H264) buffer size control.
        (启用跳帧模式，缓冲区上限由 VBV (MPEG1/2/4) 或 CPB (H264) 缓冲区
        大小控制设定。)

.. raw:: latex

    \normalsize

``V4L2_CID_MPEG_MFC51_VIDEO_RC_FIXED_TARGET_BIT (integer)`` (固定目标比特)
    Enable rate-control with fixed target bit. If this setting is
    enabled, then the rate control logic of the encoder will calculate
    the average bitrate for a GOP and keep it below or equal the set
    bitrate target. Otherwise the rate control logic calculates the
    overall average bitrate for the stream and keeps it below or equal
    to the set bitrate. In the first case the average bitrate for the
    whole stream will be smaller then the set bitrate. This is caused
    because the average is calculated for smaller number of frames, on
    the other hand enabling this setting will ensure that the stream
    will meet tight bandwidth constraints. Applicable to encoders.
    (启用固定目标比特的码率控制。如果启用此设置，编码器的码率控制逻辑将为
    一个 GOP 计算平均比特率，并使其低于或等于设定的比特率目标。否则，码率
    控制逻辑为整个流计算总体平均比特率，并使其低于或等于设定的比特率。在第
    一种情况下，整个流的平均比特率会小于设定的比特率，这是因为平均值是对
    更少的帧计算的；另一方面，启用此设置将确保流满足严格的带宽约束。适用于
    编码器。)

.. _v4l2-mpeg-mfc51-video-force-frame-type:

``V4L2_CID_MPEG_MFC51_VIDEO_FORCE_FRAME_TYPE`` (强制帧类型)
    (enum)

enum v4l2_mpeg_mfc51_video_force_frame_type -
    Force a frame type for the next queued buffer. Applicable to
    encoders. Possible values are:
    (为下一个入队缓冲区强制指定帧类型。适用于编码器。可能的取值有：)

.. tabularcolumns:: |p{9.9cm}|p{7.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_MFC51_FORCE_FRAME_TYPE_DISABLED``
      - Forcing a specific frame type disabled.
        (禁用强制特定帧类型。)
    * - ``V4L2_MPEG_MFC51_FORCE_FRAME_TYPE_I_FRAME``
      - Force an I-frame. (强制 I 帧。)
    * - ``V4L2_MPEG_MFC51_FORCE_FRAME_TYPE_NOT_CODED``
      - Force a non-coded frame. (强制非编码帧。)


CX2341x MPEG Controls(CX2341x MPEG 控制)
========================================

The following MPEG class controls deal with MPEG encoding settings that
are specific to the Conexant CX23415 and CX23416 MPEG encoding chips.
(以下 MPEG 类控制处理 Conexant CX23415 和 CX23416 MPEG 编码芯片专有的
MPEG 编码设置。)


.. _cx2341x-control-id:

CX2341x Control IDs(CX2341x 控制 ID)
------------------------------------

.. _v4l2-mpeg-cx2341x-video-spatial-filter-mode:

``V4L2_CID_MPEG_CX2341X_VIDEO_SPATIAL_FILTER_MODE`` (空间滤波模式)
    (enum)

enum v4l2_mpeg_cx2341x_video_spatial_filter_mode -
    Sets the Spatial Filter mode (default ``MANUAL``). Possible values
    are:
    (设置空间滤波模式（默认 ``MANUAL``）。可能的取值有：)


.. tabularcolumns:: |p{11.5cm}|p{6.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_CX2341X_VIDEO_SPATIAL_FILTER_MODE_MANUAL``
      - Choose the filter manually (手动选择滤波器)
    * - ``V4L2_MPEG_CX2341X_VIDEO_SPATIAL_FILTER_MODE_AUTO``
      - Choose the filter automatically (自动选择滤波器)



``V4L2_CID_MPEG_CX2341X_VIDEO_SPATIAL_FILTER (integer (0-15))`` (空间滤波设置)
    The setting for the Spatial Filter. 0 = off, 15 = maximum. (Default
    is 0.)
    (空间滤波的设置。0 = 关闭，15 = 最大。（默认为 0。）)

.. _luma-spatial-filter-type:

``V4L2_CID_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE`` (亮度空间滤波类型)
    (enum)

enum v4l2_mpeg_cx2341x_video_luma_spatial_filter_type -
    Select the algorithm to use for the Luma Spatial Filter (default
    ``1D_HOR``). Possible values:
    (选择亮度空间滤波器使用的算法（默认 ``1D_HOR``）。可能的取值：)

.. tabularcolumns:: |p{13.1cm}|p{4.4cm}|

.. raw:: latex

    \footnotesize

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_OFF``
      - No filter (无滤波)
    * - ``V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_1D_HOR``
      - One-dimensional horizontal (一维水平)
    * - ``V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_1D_VERT``
      - One-dimensional vertical (一维垂直)
    * - ``V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_2D_HV_SEPARABLE``
      - Two-dimensional separable (二维可分离)
    * - ``V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_2D_SYM_NON_SEPARABLE``
      - Two-dimensional symmetrical non-separable (二维对称不可分离)

.. raw:: latex

    \normalsize

.. _chroma-spatial-filter-type:

``V4L2_CID_MPEG_CX2341X_VIDEO_CHROMA_SPATIAL_FILTER_TYPE`` (色度空间滤波类型)
    (enum)

enum v4l2_mpeg_cx2341x_video_chroma_spatial_filter_type -
    Select the algorithm for the Chroma Spatial Filter (default
    ``1D_HOR``). Possible values are:
    (选择色度空间滤波器使用的算法（默认 ``1D_HOR``）。可能的取值有：)

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{11.0cm}|p{6.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_CX2341X_VIDEO_CHROMA_SPATIAL_FILTER_TYPE_OFF``
      - No filter (无滤波)
    * - ``V4L2_MPEG_CX2341X_VIDEO_CHROMA_SPATIAL_FILTER_TYPE_1D_HOR``
      - One-dimensional horizontal (一维水平)

.. raw:: latex

    \normalsize

.. _v4l2-mpeg-cx2341x-video-temporal-filter-mode:

``V4L2_CID_MPEG_CX2341X_VIDEO_TEMPORAL_FILTER_MODE`` (时域滤波模式)
    (enum)

enum v4l2_mpeg_cx2341x_video_temporal_filter_mode -
    Sets the Temporal Filter mode (default ``MANUAL``). Possible values
    are:
    (设置时域滤波模式（默认 ``MANUAL``）。可能的取值有：)

.. raw:: latex

    \footnotesize

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_CX2341X_VIDEO_TEMPORAL_FILTER_MODE_MANUAL``
      - Choose the filter manually (手动选择滤波器)
    * - ``V4L2_MPEG_CX2341X_VIDEO_TEMPORAL_FILTER_MODE_AUTO``
      - Choose the filter automatically (自动选择滤波器)

.. raw:: latex

    \normalsize

``V4L2_CID_MPEG_CX2341X_VIDEO_TEMPORAL_FILTER (integer (0-31))`` (时域滤波设置)
    The setting for the Temporal Filter. 0 = off, 31 = maximum. (Default
    is 8 for full-scale capturing and 0 for scaled capturing.)
    (时域滤波的设置。0 = 关闭，31 = 最大。（全尺寸采集默认为 8，缩放采集
    默认为 0。）)

.. _v4l2-mpeg-cx2341x-video-median-filter-type:

``V4L2_CID_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE`` (中值滤波类型)
    (enum)

enum v4l2_mpeg_cx2341x_video_median_filter_type -
    Median Filter Type (default ``OFF``). Possible values are:
    (中值滤波类型（默认 ``OFF``）。可能的取值有：)


.. raw:: latex

    \small

.. tabularcolumns:: |p{11.0cm}|p{6.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_OFF``
      - No filter (无滤波)
    * - ``V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_HOR``
      - Horizontal filter (水平滤波)
    * - ``V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_VERT``
      - Vertical filter (垂直滤波)
    * - ``V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_HOR_VERT``
      - Horizontal and vertical filter (水平和垂直滤波)
    * - ``V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_DIAG``
      - Diagonal filter (对角滤波)

.. raw:: latex

    \normalsize

``V4L2_CID_MPEG_CX2341X_VIDEO_LUMA_MEDIAN_FILTER_BOTTOM (integer (0-255))`` (亮度中值滤波下限)
    Threshold above which the luminance median filter is enabled
    (default 0)
    (启用亮度中值滤波的阈值下限（默认 0）。)

``V4L2_CID_MPEG_CX2341X_VIDEO_LUMA_MEDIAN_FILTER_TOP (integer (0-255))`` (亮度中值滤波上限)
    Threshold below which the luminance median filter is enabled
    (default 255)
    (启用亮度中值滤波的阈值上限（默认 255）。)

``V4L2_CID_MPEG_CX2341X_VIDEO_CHROMA_MEDIAN_FILTER_BOTTOM (integer (0-255))`` (色度中值滤波下限)
    Threshold above which the chroma median filter is enabled (default
    0)
    (启用色度中值滤波的阈值下限（默认 0）。)

``V4L2_CID_MPEG_CX2341X_VIDEO_CHROMA_MEDIAN_FILTER_TOP (integer (0-255))`` (色度中值滤波上限)
    Threshold below which the chroma median filter is enabled (default
    255)
    (启用色度中值滤波的阈值上限（默认 255）。)

``V4L2_CID_MPEG_CX2341X_STREAM_INSERT_NAV_PACKETS (boolean)`` (插入 NAV 包)
    The CX2341X MPEG encoder can insert one empty MPEG-2 PES packet into
    the stream between every four video frames. The packet size is 2048
    bytes, including the packet_start_code_prefix and stream_id
    fields. The stream_id is 0xBF (private stream 2). The payload
    consists of 0x00 bytes, to be filled in by the application. 0 = do
    not insert, 1 = insert packets.
    (CX2341X MPEG 编码器可以每四个视频帧之间向流中插入一个空的 MPEG-2 PES
    包。包大小为 2048 字节，包括 packet_start_code_prefix 和 stream_id
    字段。stream_id 为 0xBF（私有流 2）。有效载荷由 0x00 字节组成，由应用
    程序填写。0 = 不插入，1 = 插入包。)


VPX Control Reference(VPX 控制参考)
===================================

The VPX controls include controls for encoding parameters of VPx video
codec.
(VPX 控制包括 VPx 视频编解码器编码参数的控制。)


.. _vpx-control-id:

VPX Control IDs(VPX 控制 ID)
----------------------------

.. _v4l2-vpx-num-partitions:

``V4L2_CID_MPEG_VIDEO_VPX_NUM_PARTITIONS`` (分区数量)
    (enum)

enum v4l2_vp8_num_partitions -
    The number of token partitions to use in VP8 encoder. Possible
    values are:
    (VP8 编码器中要使用的 token 分区数量。可能的取值有：)



.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_CID_MPEG_VIDEO_VPX_1_PARTITION``
      - 1 coefficient partition (1 个系数分区)
    * - ``V4L2_CID_MPEG_VIDEO_VPX_2_PARTITIONS``
      - 2 coefficient partitions (2 个系数分区)
    * - ``V4L2_CID_MPEG_VIDEO_VPX_4_PARTITIONS``
      - 4 coefficient partitions (4 个系数分区)
    * - ``V4L2_CID_MPEG_VIDEO_VPX_8_PARTITIONS``
      - 8 coefficient partitions (8 个系数分区)



``V4L2_CID_MPEG_VIDEO_VPX_IMD_DISABLE_4X4 (boolean)`` (禁用帧内 4x4)
    Setting this prevents intra 4x4 mode in the intra mode decision.
    (设置此项可阻止帧内模式决策中使用帧内 4x4 模式。)

.. _v4l2-vpx-num-ref-frames:

``V4L2_CID_MPEG_VIDEO_VPX_NUM_REF_FRAMES`` (参考帧数量)
    (enum)

enum v4l2_vp8_num_ref_frames -
    The number of reference pictures for encoding P frames. Possible
    values are:
    (编码 P 帧的参考图片数量。可能的取值有：)

.. tabularcolumns:: |p{7.5cm}|p{7.5cm}|

.. raw:: latex

    \small

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_CID_MPEG_VIDEO_VPX_1_REF_FRAME``
      - Last encoded frame will be searched
        (将搜索最后一个编码帧)
    * - ``V4L2_CID_MPEG_VIDEO_VPX_2_REF_FRAME``
      - Two frames will be searched among the last encoded frame, the
	golden frame and the alternate reference (altref) frame. The
	encoder implementation will decide which two are chosen.
        (将在最后一个编码帧、黄金帧和备用参考 (altref) 帧之间搜索两个帧。
        由编码器实现决定选择哪两个。)
    * - ``V4L2_CID_MPEG_VIDEO_VPX_3_REF_FRAME``
      - The last encoded frame, the golden frame and the altref frame will
	be searched.
        (将搜索最后一个编码帧、黄金帧和 altref 帧。)

.. raw:: latex

    \normalsize



``V4L2_CID_MPEG_VIDEO_VPX_FILTER_LEVEL (integer)`` (滤波级别)
    Indicates the loop filter level. The adjustment of the loop filter
    level is done via a delta value against a baseline loop filter
    value.
    (指示环路滤波级别。环路滤波级别的调整通过相对于基准环路滤波值的增量值
    完成。)

``V4L2_CID_MPEG_VIDEO_VPX_FILTER_SHARPNESS (integer)`` (滤波锐度)
    This parameter affects the loop filter. Anything above zero weakens
    the deblocking effect on the loop filter.
    (此参数影响环路滤波。任何大于 0 的值都会削弱环路滤波的去块效果。)

``V4L2_CID_MPEG_VIDEO_VPX_GOLDEN_FRAME_REF_PERIOD (integer)`` (黄金帧刷新周期)
    Sets the refresh period for the golden frame. The period is defined
    in number of frames. For a value of 'n', every nth frame starting
    from the first key frame will be taken as a golden frame. For eg.
    for encoding sequence of 0, 1, 2, 3, 4, 5, 6, 7 where the golden
    frame refresh period is set as 4, the frames 0, 4, 8 etc will be
    taken as the golden frames as frame 0 is always a key frame.
    (设置黄金帧的刷新周期。周期以帧数定义。若值为 'n'，则从第一个关键帧起，
    每第 n 帧将被当作黄金帧。例如，对编码序列 0, 1, 2, 3, 4, 5, 6, 7，如果
    黄金帧刷新周期设为 4，则第 0、4、8 帧等将被当作黄金帧，因为第 0 帧总是
    关键帧。)

.. _v4l2-vpx-golden-frame-sel:

``V4L2_CID_MPEG_VIDEO_VPX_GOLDEN_FRAME_SEL`` (黄金帧选择)
    (enum)

enum v4l2_vp8_golden_frame_sel -
    Selects the golden frame for encoding. Possible values are:
    (选择用于编码的黄金帧。可能的取值有：)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{8.6cm}|p{8.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_CID_MPEG_VIDEO_VPX_GOLDEN_FRAME_USE_PREV``
      - Use the (n-2)th frame as a golden frame, current frame index being
	'n'.
        (使用第 (n-2) 帧作为黄金帧，当前帧索引为 'n'。)
    * - ``V4L2_CID_MPEG_VIDEO_VPX_GOLDEN_FRAME_USE_REF_PERIOD``
      - Use the previous specific frame indicated by
	``V4L2_CID_MPEG_VIDEO_VPX_GOLDEN_FRAME_REF_PERIOD`` as a
	golden frame.
        (使用 ``V4L2_CID_MPEG_VIDEO_VPX_GOLDEN_FRAME_REF_PERIOD`` 所指明的
        前一个特定帧作为黄金帧。)

.. raw:: latex

    \normalsize


``V4L2_CID_MPEG_VIDEO_VPX_MIN_QP (integer)`` (VP8 最小 QP)
    Minimum quantization parameter for VP8.
    (VP8 的最小量化参数。)

``V4L2_CID_MPEG_VIDEO_VPX_MAX_QP (integer)`` (VP8 最大 QP)
    Maximum quantization parameter for VP8.
    (VP8 的最大量化参数。)

``V4L2_CID_MPEG_VIDEO_VPX_I_FRAME_QP (integer)`` (VP8 I 帧 QP)
    Quantization parameter for an I frame for VP8.
    (VP8 I 帧的量化参数。)

``V4L2_CID_MPEG_VIDEO_VPX_P_FRAME_QP (integer)`` (VP8 P 帧 QP)
    Quantization parameter for a P frame for VP8.
    (VP8 P 帧的量化参数。)

.. _v4l2-mpeg-video-vp8-profile:

``V4L2_CID_MPEG_VIDEO_VP8_PROFILE`` (VP8 档次)
    (enum)

enum v4l2_mpeg_video_vp8_profile -
    This control allows selecting the profile for VP8 encoder.
    This is also used to enumerate supported profiles by VP8 encoder or decoder.
    Possible values are:
    (此控制允许为 VP8 编码器选择档次。它也用于枚举 VP8 编码器或解码器支持的
    档次。可能的取值有：)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_VP8_PROFILE_0``
      - Profile 0 (档次 0)
    * - ``V4L2_MPEG_VIDEO_VP8_PROFILE_1``
      - Profile 1 (档次 1)
    * - ``V4L2_MPEG_VIDEO_VP8_PROFILE_2``
      - Profile 2 (档次 2)
    * - ``V4L2_MPEG_VIDEO_VP8_PROFILE_3``
      - Profile 3 (档次 3)

.. _v4l2-mpeg-video-vp9-profile:

``V4L2_CID_MPEG_VIDEO_VP9_PROFILE`` (VP9 档次)
    (enum)

enum v4l2_mpeg_video_vp9_profile -
    This control allows selecting the profile for VP9 encoder.
    This is also used to enumerate supported profiles by VP9 encoder or decoder.
    Possible values are:
    (此控制允许为 VP9 编码器选择档次。它也用于枚举 VP9 编码器或解码器支持的
    档次。可能的取值有：)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_VP9_PROFILE_0``
      - Profile 0 (档次 0)
    * - ``V4L2_MPEG_VIDEO_VP9_PROFILE_1``
      - Profile 1 (档次 1)
    * - ``V4L2_MPEG_VIDEO_VP9_PROFILE_2``
      - Profile 2 (档次 2)
    * - ``V4L2_MPEG_VIDEO_VP9_PROFILE_3``
      - Profile 3 (档次 3)

.. _v4l2-mpeg-video-vp9-level:

``V4L2_CID_MPEG_VIDEO_VP9_LEVEL (enum)`` (VP9 级别)

enum v4l2_mpeg_video_vp9_level -
    This control allows selecting the level for VP9 encoder.
    This is also used to enumerate supported levels by VP9 encoder or decoder.
    More information can be found at
    `webmproject <https://www.webmproject.org/vp9/levels/>`__. Possible values are:
    (此控制允许为 VP9 编码器选择级别。它也用于枚举 VP9 编码器或解码器支持的
    级别。更多信息见
    `webmproject <https://www.webmproject.org/vp9/levels/>`__。可能的取值有：)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_1_0``
      - Level 1
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_1_1``
      - Level 1.1
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_2_0``
      - Level 2
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_2_1``
      - Level 2.1
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_3_0``
      - Level 3
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_3_1``
      - Level 3.1
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_4_0``
      - Level 4
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_4_1``
      - Level 4.1
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_5_0``
      - Level 5
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_5_1``
      - Level 5.1
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_5_2``
      - Level 5.2
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_6_0``
      - Level 6
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_6_1``
      - Level 6.1
    * - ``V4L2_MPEG_VIDEO_VP9_LEVEL_6_2``
      - Level 6.2


High Efficiency Video Coding (HEVC/H.265) Control Reference(高效视频编码 (HEVC/H.265) 控制参考)
===============================================================================================

The HEVC/H.265 controls include controls for encoding parameters of HEVC/H.265
video codec.
(HEVC/H.265 控制包括 HEVC/H.265 视频编解码器编码参数的控制。)


.. _hevc-control-id:

HEVC/H.265 Control IDs(HEVC/H.265 控制 ID)
------------------------------------------

``V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP (integer)`` (HEVC 最小 QP)
    Minimum quantization parameter for HEVC.
    Valid range: from 0 to 51 for 8 bit and from 0 to 63 for 10 bit.
    (HEVC 的最小量化参数。有效范围：8 位时为 0 到 51，10 位时为 0 到 63。)

``V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP (integer)`` (HEVC 最大 QP)
    Maximum quantization parameter for HEVC.
    Valid range: from 0 to 51 for 8 bit and from 0 to 63 for 10 bit.
    (HEVC 的最大量化参数。有效范围：8 位时为 0 到 51，10 位时为 0 到 63。)

``V4L2_CID_MPEG_VIDEO_HEVC_I_FRAME_QP (integer)`` (HEVC I 帧 QP)
    Quantization parameter for an I frame for HEVC.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (HEVC I 帧的量化参数。有效范围：[V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_P_FRAME_QP (integer)`` (HEVC P 帧 QP)
    Quantization parameter for a P frame for HEVC.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (HEVC P 帧的量化参数。有效范围：[V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_B_FRAME_QP (integer)`` (HEVC B 帧 QP)
    Quantization parameter for a B frame for HEVC.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (HEVC B 帧的量化参数。有效范围：[V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_I_FRAME_MIN_QP (integer)`` (HEVC I 帧最小 QP)
    Minimum quantization parameter for the HEVC I frame to limit I frame
    quality to a range. Valid range: from 0 to 51 for 8 bit and from 0 to 63 for 10 bit.
    If V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (HEVC I 帧的最小量化参数，用于将 I 帧质量限制在一定范围内。有效范围：
    8 位时为 0 到 51，10 位时为 0 到 63。如果同时设置了
    V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP，则应选择满足两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_HEVC_I_FRAME_MAX_QP (integer)`` (HEVC I 帧最大 QP)
    Maximum quantization parameter for the HEVC I frame to limit I frame
    quality to a range. Valid range: from 0 to 51 for 8 bit and from 0 to 63 for 10 bit.
    If V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (HEVC I 帧的最大量化参数，用于将 I 帧质量限制在一定范围内。有效范围：
    8 位时为 0 到 51，10 位时为 0 到 63。如果同时设置了
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP，则应选择满足两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_HEVC_P_FRAME_MIN_QP (integer)`` (HEVC P 帧最小 QP)
    Minimum quantization parameter for the HEVC P frame to limit P frame
    quality to a range. Valid range: from 0 to 51 for 8 bit and from 0 to 63 for 10 bit.
    If V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (HEVC P 帧的最小量化参数，用于将 P 帧质量限制在一定范围内。有效范围：
    8 位时为 0 到 51，10 位时为 0 到 63。如果同时设置了
    V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP，则应选择满足两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_HEVC_P_FRAME_MAX_QP (integer)`` (HEVC P 帧最大 QP)
    Maximum quantization parameter for the HEVC P frame to limit P frame
    quality to a range. Valid range: from 0 to 51 for 8 bit and from 0 to 63 for 10 bit.
    If V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (HEVC P 帧的最大量化参数，用于将 P 帧质量限制在一定范围内。有效范围：
    8 位时为 0 到 51，10 位时为 0 到 63。如果同时设置了
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP，则应选择满足两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_HEVC_B_FRAME_MIN_QP (integer)`` (HEVC B 帧最小 QP)
    Minimum quantization parameter for the HEVC B frame to limit B frame
    quality to a range. Valid range: from 0 to 51 for 8 bit and from 0 to 63 for 10 bit.
    If V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (HEVC B 帧的最小量化参数，用于将 B 帧质量限制在一定范围内。有效范围：
    8 位时为 0 到 51，10 位时为 0 到 63。如果同时设置了
    V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP，则应选择满足两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_HEVC_B_FRAME_MAX_QP (integer)`` (HEVC B 帧最大 QP)
    Maximum quantization parameter for the HEVC B frame to limit B frame
    quality to a range. Valid range: from 0 to 51 for 8 bit and from 0 to 63 for 10 bit.
    If V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP is also set, the quantization parameter
    should be chosen to meet both requirements.
    (HEVC B 帧的最大量化参数，用于将 B 帧质量限制在一定范围内。有效范围：
    8 位时为 0 到 51，10 位时为 0 到 63。如果同时设置了
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP，则应选择满足两个要求的量化参数。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_QP (boolean)`` (层次 QP)
    HIERARCHICAL_QP allows the host to specify the quantization parameter
    values for each temporal layer through HIERARCHICAL_QP_LAYER. This is
    valid only if HIERARCHICAL_CODING_LAYER is greater than 1. Setting the
    control value to 1 enables setting of the QP values for the layers.
    (HIERARCHICAL_QP 允许主机通过 HIERARCHICAL_QP_LAYER 为每个时间层指定
    量化参数值。仅当 HIERARCHICAL_CODING_LAYER 大于 1 时有效。将控制值设为
    1 即可启用对各层 QP 值的设置。)

.. _v4l2-hevc-hier-coding-type:

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_TYPE`` (层次编码类型)
    (enum)

enum v4l2_mpeg_video_hevc_hier_coding_type -
    Selects the hierarchical coding type for encoding. Possible values are:
    (选择用于编码的层次编码类型。可能的取值有：)

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{8.2cm}|p{9.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_B``
      - Use the B frame for hierarchical coding.
        (使用 B 帧进行层次编码。)
    * - ``V4L2_MPEG_VIDEO_HEVC_HIERARCHICAL_CODING_P``
      - Use the P frame for hierarchical coding.
        (使用 P 帧进行层次编码。)

.. raw:: latex

    \normalsize


``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_LAYER (integer)`` (层次编码层)
    Selects the hierarchical coding layer. In normal encoding
    (non-hierarchial coding), it should be zero. Possible values are [0, 6].
    0 indicates HIERARCHICAL CODING LAYER 0, 1 indicates HIERARCHICAL CODING
    LAYER 1 and so on.
    (选择层次编码层。在普通编码（非层次编码）中，它应为 0。可能的取值是
    [0, 6]。0 表示 HIERARCHICAL CODING LAYER 0，1 表示 HIERARCHICAL CODING
    LAYER 1，依此类推。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L0_QP (integer)`` (层 0 QP)
    Indicates quantization parameter for hierarchical coding layer 0.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (指示层次编码层 0 的量化参数。有效范围：
    [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP, V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L1_QP (integer)`` (层 1 QP)
    Indicates quantization parameter for hierarchical coding layer 1.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (指示层次编码层 1 的量化参数。有效范围：
    [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP, V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L2_QP (integer)`` (层 2 QP)
    Indicates quantization parameter for hierarchical coding layer 2.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (指示层次编码层 2 的量化参数。有效范围：
    [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP, V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L3_QP (integer)`` (层 3 QP)
    Indicates quantization parameter for hierarchical coding layer 3.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (指示层次编码层 3 的量化参数。有效范围：
    [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP, V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L4_QP (integer)`` (层 4 QP)
    Indicates quantization parameter for hierarchical coding layer 4.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (指示层次编码层 4 的量化参数。有效范围：
    [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP, V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L5_QP (integer)`` (层 5 QP)
    Indicates quantization parameter for hierarchical coding layer 5.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (指示层次编码层 5 的量化参数。有效范围：
    [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP, V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L6_QP (integer)`` (层 6 QP)
    Indicates quantization parameter for hierarchical coding layer 6.
    Valid range: [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP,
    V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP].
    (指示层次编码层 6 的量化参数。有效范围：
    [V4L2_CID_MPEG_VIDEO_HEVC_MIN_QP, V4L2_CID_MPEG_VIDEO_HEVC_MAX_QP]。)

.. _v4l2-hevc-profile:

``V4L2_CID_MPEG_VIDEO_HEVC_PROFILE`` (HEVC 档次)
    (enum)

enum v4l2_mpeg_video_hevc_profile -
    Select the desired profile for HEVC encoder.
    (为 HEVC 编码器选择所需的档次。)

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{9.0cm}|p{8.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_HEVC_PROFILE_MAIN``
      - Main profile. (主档次。)
    * - ``V4L2_MPEG_VIDEO_HEVC_PROFILE_MAIN_STILL_PICTURE``
      - Main still picture profile. (主静止图像档次。)
    * - ``V4L2_MPEG_VIDEO_HEVC_PROFILE_MAIN_10``
      - Main 10 profile. (Main 10 档次。)

.. raw:: latex

    \normalsize


.. _v4l2-hevc-level:

``V4L2_CID_MPEG_VIDEO_HEVC_LEVEL`` (HEVC 级别)
    (enum)

enum v4l2_mpeg_video_hevc_level -
    Selects the desired level for HEVC encoder.
    (为 HEVC 编码器选择所需的级别。)

==================================	=========
``V4L2_MPEG_VIDEO_HEVC_LEVEL_1``	Level 1.0 (级别 1.0)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_2``	Level 2.0 (级别 2.0)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_2_1``	Level 2.1 (级别 2.1)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_3``	Level 3.0 (级别 3.0)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_3_1``	Level 3.1 (级别 3.1)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_4``	Level 4.0 (级别 4.0)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_4_1``	Level 4.1 (级别 4.1)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_5``	Level 5.0 (级别 5.0)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_5_1``	Level 5.1 (级别 5.1)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_5_2``	Level 5.2 (级别 5.2)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_6``	Level 6.0 (级别 6.0)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_6_1``	Level 6.1 (级别 6.1)
``V4L2_MPEG_VIDEO_HEVC_LEVEL_6_2``	Level 6.2 (级别 6.2)
==================================	=========

``V4L2_CID_MPEG_VIDEO_HEVC_FRAME_RATE_RESOLUTION (integer)`` (帧率分辨率)
    Indicates the number of evenly spaced subintervals, called ticks, within
    one second. This is a 16 bit unsigned integer and has a maximum value up to
    0xffff and a minimum value of 1.
    (指示一秒内均匀分布的子区间（称为 tick）的数量。这是一个 16 位无符号
    整数，最大值可达 0xffff，最小值为 1。)

.. _v4l2-hevc-tier:

``V4L2_CID_MPEG_VIDEO_HEVC_TIER`` (HEVC 层级)
    (enum)

enum v4l2_mpeg_video_hevc_tier -
    TIER_FLAG specifies tiers information of the HEVC encoded picture. Tier
    were made to deal with applications that differ in terms of maximum bit
    rate. Setting the flag to 0 selects HEVC tier as Main tier and setting
    this flag to 1 indicates High tier. High tier is for applications requiring
    high bit rates.
    (TIER_FLAG 指定 HEVC 编码图像的层级 (tier) 信息。设立层级是为了应对最大
    比特率不同的应用。将标志设为 0 选择 HEVC 层级为主层级 (Main tier)，将
    此标志设为 1 表示高级层 (High tier)。高级层适用于要求高比特率的应用。)

==================================	==========
``V4L2_MPEG_VIDEO_HEVC_TIER_MAIN``	Main tier. (主层级。)
``V4L2_MPEG_VIDEO_HEVC_TIER_HIGH``	High tier. (高层级。)
==================================	==========


``V4L2_CID_MPEG_VIDEO_HEVC_MAX_PARTITION_DEPTH (integer)`` (最大划分深度)
    Selects HEVC maximum coding unit depth.
    (选择 HEVC 最大编码单元深度。)

.. _v4l2-hevc-loop-filter-mode:

``V4L2_CID_MPEG_VIDEO_HEVC_LOOP_FILTER_MODE`` (环路滤波模式)
    (enum)

enum v4l2_mpeg_video_hevc_loop_filter_mode -
    Loop filter mode for HEVC encoder. Possible values are:
    (HEVC 编码器的环路滤波模式。可能的取值有：)

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{12.1cm}|p{5.4cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_HEVC_LOOP_FILTER_MODE_DISABLED``
      - Loop filter is disabled. (环路滤波被禁用。)
    * - ``V4L2_MPEG_VIDEO_HEVC_LOOP_FILTER_MODE_ENABLED``
      - Loop filter is enabled. (环路滤波已启用。)
    * - ``V4L2_MPEG_VIDEO_HEVC_LOOP_FILTER_MODE_DISABLED_AT_SLICE_BOUNDARY``
      - Loop filter is disabled at the slice boundary.
        (在切片边界处禁用环路滤波。)

.. raw:: latex

    \normalsize


``V4L2_CID_MPEG_VIDEO_HEVC_LF_BETA_OFFSET_DIV2 (integer)`` (环路滤波 beta 偏移)
    Selects HEVC loop filter beta offset. The valid range is [-6, +6].
    (选择 HEVC 环路滤波 beta 偏移。有效范围是 [-6, +6]。)

``V4L2_CID_MPEG_VIDEO_HEVC_LF_TC_OFFSET_DIV2 (integer)`` (环路滤波 tc 偏移)
    Selects HEVC loop filter tc offset. The valid range is [-6, +6].
    (选择 HEVC 环路滤波 tc 偏移。有效范围是 [-6, +6]。)

.. _v4l2-hevc-refresh-type:

``V4L2_CID_MPEG_VIDEO_HEVC_REFRESH_TYPE`` (刷新类型)
    (enum)

enum v4l2_mpeg_video_hevc_hier_refresh_type -
    Selects refresh type for HEVC encoder.
    Host has to specify the period into
    V4L2_CID_MPEG_VIDEO_HEVC_REFRESH_PERIOD.
    (为 HEVC 编码器选择刷新类型。主机必须在
    V4L2_CID_MPEG_VIDEO_HEVC_REFRESH_PERIOD 中指定周期。)

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{6.2cm}|p{11.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_HEVC_REFRESH_NONE``
      - Use the B frame for hierarchical coding.
        (使用 B 帧进行层次编码。)
    * - ``V4L2_MPEG_VIDEO_HEVC_REFRESH_CRA``
      - Use CRA (Clean Random Access Unit) picture encoding.
        (使用 CRA（Clean Random Access Unit，干净随机访问单元）图像编码。)
    * - ``V4L2_MPEG_VIDEO_HEVC_REFRESH_IDR``
      - Use IDR (Instantaneous Decoding Refresh) picture encoding.
        (使用 IDR（Instantaneous Decoding Refresh，即时解码刷新）图像编码。)

.. raw:: latex

    \normalsize


``V4L2_CID_MPEG_VIDEO_HEVC_REFRESH_PERIOD (integer)`` (刷新周期)
    Selects the refresh period for HEVC encoder.
    This specifies the number of I pictures between two CRA/IDR pictures.
    This is valid only if REFRESH_TYPE is not 0.
    (选择 HEVC 编码器的刷新周期。它指定两个 CRA/IDR 图像之间 I 图像的数量。
    仅当 REFRESH_TYPE 不为 0 时有效。)

``V4L2_CID_MPEG_VIDEO_HEVC_LOSSLESS_CU (boolean)`` (无损编码单元)
    Indicates HEVC lossless encoding. Setting it to 0 disables lossless
    encoding. Setting it to 1 enables lossless encoding.
    (指示 HEVC 无损编码。设为 0 禁用无损编码，设为 1 启用无损编码。)

``V4L2_CID_MPEG_VIDEO_HEVC_CONST_INTRA_PRED (boolean)`` (受限帧内预测)
    Indicates constant intra prediction for HEVC encoder. Specifies the
    constrained intra prediction in which intra largest coding unit (LCU)
    prediction is performed by using residual data and decoded samples of
    neighboring intra LCU only. Setting the value to 1 enables constant intra
    prediction and setting the value to 0 disables constant intra prediction.
    (指示 HEVC 编码器的受限帧内预测。它指定一种受限帧内预测，其中帧内最大
    编码单元 (LCU) 的预测仅使用残差数据和相邻帧内 LCU 的解码样本来执行。设
    为 1 启用受限帧内预测，设为 0 禁用受限帧内预测。)

``V4L2_CID_MPEG_VIDEO_HEVC_WAVEFRONT (boolean)`` (波前并行处理)
    Indicates wavefront parallel processing for HEVC encoder. Setting it to 0
    disables the feature and setting it to 1 enables the wavefront parallel
    processing.
    (指示 HEVC 编码器的波前并行处理 (WPP)。设为 0 禁用该功能，设为 1 启用
    波前并行处理。)

``V4L2_CID_MPEG_VIDEO_HEVC_GENERAL_PB (boolean)`` (P/B 帧组合)
    Setting the value to 1 enables combination of P and B frame for HEVC
    encoder.
    (设为 1 启用 HEVC 编码器的 P 帧与 B 帧组合。)

``V4L2_CID_MPEG_VIDEO_HEVC_TEMPORAL_ID (boolean)`` (时间标识符)
    Indicates temporal identifier for HEVC encoder which is enabled by
    setting the value to 1.
    (指示 HEVC 编码器的时间标识符，设为 1 时启用。)

``V4L2_CID_MPEG_VIDEO_HEVC_STRONG_SMOOTHING (boolean)`` (强平滑)
    Indicates bi-linear interpolation is conditionally used in the intra
    prediction filtering process in the CVS when set to 1. Indicates bi-linear
    interpolation is not used in the CVS when set to 0.
    (设为 1 时，指示在 CVS 的帧内预测滤波过程中有条件地使用双线性插值。设为
    0 时，指示在 CVS 中不使用双线性插值。)

``V4L2_CID_MPEG_VIDEO_HEVC_MAX_NUM_MERGE_MV_MINUS1 (integer)`` (最大合并候选 MV 数)
    Indicates maximum number of merge candidate motion vectors.
    Values are from 0 to 4.
    (指示合并候选运动矢量的最大数量。取值范围为 0 到 4。)

``V4L2_CID_MPEG_VIDEO_HEVC_TMV_PREDICTION (boolean)`` (时域运动矢量预测)
    Indicates temporal motion vector prediction for HEVC encoder. Setting it to
    1 enables the prediction. Setting it to 0 disables the prediction.
    (指示 HEVC 编码器的时域运动矢量预测。设为 1 启用预测，设为 0 禁用
    预测。)

``V4L2_CID_MPEG_VIDEO_HEVC_WITHOUT_STARTCODE (boolean)`` (无起始码)
    Specifies if HEVC generates a stream with a size of the length field
    instead of start code pattern. The size of the length field is configurable
    through the V4L2_CID_MPEG_VIDEO_HEVC_SIZE_OF_LENGTH_FIELD control. Setting
    the value to 0 disables encoding without startcode pattern. Setting the
    value to 1 will enables encoding without startcode pattern.
    (指定 HEVC 是否生成用长度字段代替起始码模式的流。长度字段的大小可以通过
    V4L2_CID_MPEG_VIDEO_HEVC_SIZE_OF_LENGTH_FIELD 控制配置。设为 0 禁用无
    起始码模式编码，设为 1 启用无起始码模式编码。)

.. _v4l2-hevc-size-of-length-field:

``V4L2_CID_MPEG_VIDEO_HEVC_SIZE_OF_LENGTH_FIELD`` (长度字段大小)
(enum)

enum v4l2_mpeg_video_hevc_size_of_length_field -
    Indicates the size of length field.
    This is valid when encoding WITHOUT_STARTCODE_ENABLE is enabled.
    (指示长度字段的大小。仅在启用了 WITHOUT_STARTCODE_ENABLE 的编码中有效。)

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{5.5cm}|p{12.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_MPEG_VIDEO_HEVC_SIZE_0``
      - Generate start code pattern (Normal).
        (生成起始码模式（常规）。)
    * - ``V4L2_MPEG_VIDEO_HEVC_SIZE_1``
      - Generate size of length field instead of start code pattern and length is 1.
        (生成长度字段大小而非起始码模式，长度为 1。)
    * - ``V4L2_MPEG_VIDEO_HEVC_SIZE_2``
      - Generate size of length field instead of start code pattern and length is 2.
        (生成长度字段大小而非起始码模式，长度为 2。)
    * - ``V4L2_MPEG_VIDEO_HEVC_SIZE_4``
      - Generate size of length field instead of start code pattern and length is 4.
        (生成长度字段大小而非起始码模式，长度为 4。)

.. raw:: latex

    \normalsize

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L0_BR (integer)`` (层 0 比特率)
    Indicates bit rate for hierarchical coding layer 0 for HEVC encoder.
    (指示 HEVC 编码器层次编码层 0 的比特率。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L1_BR (integer)`` (层 1 比特率)
    Indicates bit rate for hierarchical coding layer 1 for HEVC encoder.
    (指示 HEVC 编码器层次编码层 1 的比特率。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L2_BR (integer)`` (层 2 比特率)
    Indicates bit rate for hierarchical coding layer 2 for HEVC encoder.
    (指示 HEVC 编码器层次编码层 2 的比特率。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L3_BR (integer)`` (层 3 比特率)
    Indicates bit rate for hierarchical coding layer 3 for HEVC encoder.
    (指示 HEVC 编码器层次编码层 3 的比特率。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L4_BR (integer)`` (层 4 比特率)
    Indicates bit rate for hierarchical coding layer 4 for HEVC encoder.
    (指示 HEVC 编码器层次编码层 4 的比特率。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L5_BR (integer)`` (层 5 比特率)
    Indicates bit rate for hierarchical coding layer 5 for HEVC encoder.
    (指示 HEVC 编码器层次编码层 5 的比特率。)

``V4L2_CID_MPEG_VIDEO_HEVC_HIER_CODING_L6_BR (integer)`` (层 6 比特率)
    Indicates bit rate for hierarchical coding layer 6 for HEVC encoder.
    (指示 HEVC 编码器层次编码层 6 的比特率。)

``V4L2_CID_MPEG_VIDEO_REF_NUMBER_FOR_PFRAMES (integer)`` (P 帧参考数)
    Selects number of P reference pictures required for HEVC encoder.
    P-Frame can use 1 or 2 frames for reference.
    (选择 HEVC 编码器所需的 P 参考图片数量。P 帧可以使用 1 或 2 帧作
    参考。)

``V4L2_CID_MPEG_VIDEO_PREPEND_SPSPPS_TO_IDR (integer)`` (IDR 前置 SPS/PPS)
    Indicates whether to generate SPS and PPS at every IDR. Setting it to 0
    disables generating SPS and PPS at every IDR. Setting it to one enables
    generating SPS and PPS at every IDR.
    (指示是否在每个 IDR 处生成 SPS 和 PPS。设为 0 禁用在每个 IDR 处生成
    SPS 和 PPS，设为 1 启用在每个 IDR 处生成 SPS 和 PPS。)
