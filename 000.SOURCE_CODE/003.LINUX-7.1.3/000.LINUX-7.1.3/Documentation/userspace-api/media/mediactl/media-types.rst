.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _media-controller-types:

Types and flags used to represent the media graph elements(表示媒体图元素所用的类型与标志)
==========================================================================================

..  tabularcolumns:: |p{8.2cm}|p{9.3cm}|

.. _media-entity-functions:
.. _MEDIA-ENT-F-UNKNOWN:
.. _MEDIA-ENT-F-V4L2-SUBDEV-UNKNOWN:
.. _MEDIA-ENT-F-IO-V4L:
.. _MEDIA-ENT-F-IO-VBI:
.. _MEDIA-ENT-F-IO-SWRADIO:
.. _MEDIA-ENT-F-IO-DTV:
.. _MEDIA-ENT-F-DTV-DEMOD:
.. _MEDIA-ENT-F-TS-DEMUX:
.. _MEDIA-ENT-F-DTV-CA:
.. _MEDIA-ENT-F-DTV-NET-DECAP:
.. _MEDIA-ENT-F-CONN-RF:
.. _MEDIA-ENT-F-CONN-SVIDEO:
.. _MEDIA-ENT-F-CONN-COMPOSITE:
.. _MEDIA-ENT-F-CAM-SENSOR:
.. _MEDIA-ENT-F-FLASH:
.. _MEDIA-ENT-F-LENS:
.. _MEDIA-ENT-F-ATV-DECODER:
.. _MEDIA-ENT-F-TUNER:
.. _MEDIA-ENT-F-IF-VID-DECODER:
.. _MEDIA-ENT-F-IF-AUD-DECODER:
.. _MEDIA-ENT-F-AUDIO-CAPTURE:
.. _MEDIA-ENT-F-AUDIO-PLAYBACK:
.. _MEDIA-ENT-F-AUDIO-MIXER:
.. _MEDIA-ENT-F-PROC-VIDEO-COMPOSER:
.. _MEDIA-ENT-F-PROC-VIDEO-PIXEL-FORMATTER:
.. _MEDIA-ENT-F-PROC-VIDEO-PIXEL-ENC-CONV:
.. _MEDIA-ENT-F-PROC-VIDEO-LUT:
.. _MEDIA-ENT-F-PROC-VIDEO-SCALER:
.. _MEDIA-ENT-F-PROC-VIDEO-STATISTICS:
.. _MEDIA-ENT-F-PROC-VIDEO-ENCODER:
.. _MEDIA-ENT-F-PROC-VIDEO-DECODER:
.. _MEDIA-ENT-F-PROC-VIDEO-ISP:
.. _MEDIA-ENT-F-VID-MUX:
.. _MEDIA-ENT-F-VID-IF-BRIDGE:
.. _MEDIA-ENT-F-DV-DECODER:
.. _MEDIA-ENT-F-DV-ENCODER:

.. cssclass:: longtable

.. flat-table:: Media entity functions
    :header-rows:  0
    :stub-columns: 0

    *  -  ``MEDIA_ENT_F_UNKNOWN`` and
	  ``MEDIA_ENT_F_V4L2_SUBDEV_UNKNOWN``
       -  Unknown entity. That generally indicates that a driver didn't
	  initialize properly the entity, which is a Kernel bug
	  (未知实体。这通常表明驱动未正确初始化该实体，这是内核的一个缺陷。)

    *  -  ``MEDIA_ENT_F_IO_V4L``
       -  Data streaming input and/or output entity.(数据流输入和/或输出实体。)

    *  -  ``MEDIA_ENT_F_IO_VBI``
       -  V4L VBI streaming input or output entity(V4L VBI 数据流输入或输出实体)

    *  -  ``MEDIA_ENT_F_IO_SWRADIO``
       -  V4L Software Digital Radio (SDR) streaming input or output entity(V4L 软件数字收音机(SDR)数据流输入或输出实体)

    *  -  ``MEDIA_ENT_F_IO_DTV``
       -  DVB Digital TV streaming input or output entity(DVB 数字电视数据流输入或输出实体)

    *  -  ``MEDIA_ENT_F_DTV_DEMOD``
       -  Digital TV demodulator entity.(数字电视解调器实体。)

    *  -  ``MEDIA_ENT_F_TS_DEMUX``
       -  MPEG Transport stream demux entity. Could be implemented on
	  hardware or in Kernelspace by the Linux DVB subsystem.
	  (MPEG 传输流解复用实体。可以在硬件中实现，也可以由 Linux DVB 子系统在内核空间实现。)

    *  -  ``MEDIA_ENT_F_DTV_CA``
       -  Digital TV Conditional Access module (CAM) entity(数字电视条件接收模块(CAM)实体)

    *  -  ``MEDIA_ENT_F_DTV_NET_DECAP``
       -  Digital TV network ULE/MLE desencapsulation entity. Could be
	  implemented on hardware or in Kernelspace
	  (数字电视网络 ULE/MLE 解封装实体。可以在硬件或内核空间中实现)

    *  -  ``MEDIA_ENT_F_CONN_RF``
       -  Connector for a Radio Frequency (RF) signal.(射频(RF)信号连接器。)

    *  -  ``MEDIA_ENT_F_CONN_SVIDEO``
       -  Connector for a S-Video signal.(S-Video 信号连接器。)

    *  -  ``MEDIA_ENT_F_CONN_COMPOSITE``
       -  Connector for a RGB composite signal.(RGB 复合信号连接器。)

    *  -  ``MEDIA_ENT_F_CAM_SENSOR``
       -  Camera video sensor entity.(摄像头视频传感器实体。)

    *  -  ``MEDIA_ENT_F_FLASH``
       -  Flash controller entity.(闪光灯控制器实体。)

    *  -  ``MEDIA_ENT_F_LENS``
       -  Lens controller entity.(镜头控制器实体。)

    *  -  ``MEDIA_ENT_F_ATV_DECODER``
       -  Analog video decoder, the basic function of the video decoder is
	  to accept analogue video from a wide variety of sources such as
	  broadcast, DVD players, cameras and video cassette recorders, in
	  either NTSC, PAL, SECAM or HD format, separating the stream into
	  its component parts, luminance and chrominance, and output it in
	  some digital video standard, with appropriate timing signals.
	  (模拟视频解码器。视频解码器的基本功能是接受来自广播、DVD 播放机、摄像机和录像机等多种来源的模拟视频信号(NTSC、PAL、SECAM 或 HD 格式)，把码流分离为其组成部分——亮度与色度——并以某种数字视频标准、连同适当的定时信号输出。)

    *  -  ``MEDIA_ENT_F_TUNER``
       -  Digital TV, analog TV, radio and/or software radio tuner, with
	  consists on a PLL tuning stage that converts radio frequency (RF)
	  signal into an Intermediate Frequency (IF). Modern tuners have
	  internally IF-PLL decoders for audio and video, but older models
	  have those stages implemented on separate entities.
	  (数字电视、模拟电视、收音机和/或软件收音机调谐器，其核心是一个 PLL 调谐级，把射频(RF)信号转换为中频(IF)。现代调谐器内部带有用于音频和视频的 IF-PLL 解码器，而较老的型号则把这些级实现为单独的实体。)

    *  -  ``MEDIA_ENT_F_IF_VID_DECODER``
       -  IF-PLL video decoder. It receives the IF from a PLL and decodes
	  the analog TV video signal. This is commonly found on some very
	  old analog tuners, like Philips MK3 designs. They all contain a
	  tda9887 (or some software compatible similar chip, like tda9885).
	  Those devices use a different I2C address than the tuner PLL.
	  (IF-PLL 视频解码器。它接收来自 PLL 的 IF 并解码模拟电视视频信号。常见于一些非常古老的模拟调谐器，如 Philips MK3 设计。它们都包含一颗 tda9887(或某种软件兼容的类似芯片，如 tda9885)。这些器件使用的 I2C 地址与调谐器 PLL 不同。)

    *  -  ``MEDIA_ENT_F_IF_AUD_DECODER``
       -  IF-PLL sound decoder. It receives the IF from a PLL and decodes
	  the analog TV audio signal. This is commonly found on some very
	  old analog hardware, like Micronas msp3400, Philips tda9840,
	  tda985x, etc. Those devices use a different I2C address than the
	  tuner PLL and should be controlled together with the IF-PLL video
	  decoder.
	  (IF-PLL 音频解码器。它接收来自 PLL 的 IF 并解码模拟电视音频信号。常见于一些非常古老的模拟硬件，如 Micronas msp3400、Philips tda9840、tda985x 等。这些器件使用的 I2C 地址与调谐器 PLL 不同，应当与 IF-PLL 视频解码器一起控制。)

    *  -  ``MEDIA_ENT_F_AUDIO_CAPTURE``
       -  Audio Capture Function Entity.(音频捕获功能实体。)

    *  -  ``MEDIA_ENT_F_AUDIO_PLAYBACK``
       -  Audio Playback Function Entity.(音频播放功能实体。)

    *  -  ``MEDIA_ENT_F_AUDIO_MIXER``
       -  Audio Mixer Function Entity.(音频混音器功能实体。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_COMPOSER``
       -  Video composer (blender). An entity capable of video
	  composing must have at least two sink pads and one source
	  pad, and composes input video frames onto output video
	  frames. Composition can be performed using alpha blending,
	  color keying, raster operations (ROP), stitching or any other
	  means.
	  (视频合成器(blender)。具备视频合成能力的实体必须至少有两个 sink pad 和一个 source pad，并把输入视频帧合成到输出视频帧上。合成可以通过 alpha 混合、色键(color keying)、光栅操作(ROP)、拼接(stitching)或其他任何手段完成。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_PIXEL_FORMATTER``
       -  Video pixel formatter. An entity capable of pixel formatting
	  must have at least one sink pad and one source pad. Read
	  pixel formatters read pixels from memory and perform a subset
	  of unpacking, cropping, color keying, alpha multiplication
	  and pixel encoding conversion. Write pixel formatters perform
	  a subset of dithering, pixel encoding conversion and packing
	  and write pixels to memory.
	  (视频像素格式化器。具备像素格式化能力的实体必须至少有一个 sink pad 和一个 source pad。读像素格式化器从内存读取像素，并执行解包(unpacking)、裁剪(cropping)、色键、alpha 乘法与像素编码转换的一个子集；写像素格式化器执行抖动(dithering)、像素编码转换与打包(packing)的一个子集，并把像素写入内存。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_PIXEL_ENC_CONV``
       -  Video pixel encoding converter. An entity capable of pixel
	  encoding conversion must have at least one sink pad and one
	  source pad, and convert the encoding of pixels received on
	  its sink pad(s) to a different encoding output on its source
	  pad(s). Pixel encoding conversion includes but isn't limited
	  to RGB to/from HSV, RGB to/from YUV and CFA (Bayer) to RGB
	  conversions.
	  (视频像素编码转换器。具备像素编码转换能力的实体必须至少有一个 sink pad 和一个 source pad，并把在其 sink pad 上收到的像素的编码转换为不同的编码后从其 source pad 输出。像素编码转换包括但不限于 RGB 与 HSV、RGB 与 YUV、CFA(Bayer)与 RGB 之间的转换。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_LUT``
       -  Video look-up table. An entity capable of video lookup table
	  processing must have one sink pad and one source pad. It uses
	  the values of the pixels received on its sink pad to look up
	  entries in internal tables and output them on its source pad.
	  The lookup processing can be performed on all components
	  separately or combine them for multi-dimensional table
	  lookups.
	  (视频查找表。具备视频查找表处理能力的实体必须有一个 sink pad 和一个 source pad。它使用在其 sink pad 上收到的像素值来查找内部表中的条目，并在其 source pad 上输出。查找处理可以对所有分量分别进行，也可以把它们组合起来进行多维表查找。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_SCALER``
       -  Video scaler. An entity capable of video scaling must have
	  at least one sink pad and one source pad, and scale the
	  video frame(s) received on its sink pad(s) to a different
	  resolution output on its source pad(s). The range of
	  supported scaling ratios is entity-specific and can differ
	  between the horizontal and vertical directions (in particular
	  scaling can be supported in one direction only). Binning and
	  sub-sampling (occasionally also referred to as skipping) are
	  considered as scaling.
	  (视频缩放器。具备视频缩放能力的实体必须至少有一个 sink pad 和一个 source pad，并把在其 sink pad 上收到的视频帧缩放到不同的分辨率后从其 source pad 输出。支持的缩放比率范围因实体而异，水平与垂直方向可以不同(特别是可能只支持单向缩放)。合并读出(binning)与子采样(sub-sampling，偶尔也称为 skipping)被视为缩放。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_STATISTICS``
       -  Video statistics computation (histogram, 3A, etc.). An entity
	  capable of statistics computation must have one sink pad and
	  one source pad. It computes statistics over the frames
	  received on its sink pad and outputs the statistics data on
	  its source pad.
	  (视频统计计算(直方图、3A 等)。具备统计计算能力的实体必须有一个 sink pad 和一个 source pad。它对在其 sink pad 上收到的帧计算统计量，并在其 source pad 上输出统计数据。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_ENCODER``
       -  Video (MPEG, HEVC, VPx, etc.) encoder. An entity capable of
          compressing video frames. Must have one sink pad and at least
	  one source pad.
	  (视频(MPEG、HEVC、VPx 等)编码器。具备压缩视频帧能力的实体。必须有一个 sink pad 和至少一个 source pad。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_DECODER``
       -  Video (MPEG, HEVC, VPx, etc.) decoder. An entity capable of
          decompressing a compressed video stream into uncompressed video
	  frames. Must have one sink pad and at least one source pad.
	  (视频(MPEG、HEVC、VPx 等)解码器。具备把压缩视频流解压缩为未压缩视频帧能力的实体。必须有一个 sink pad 和至少一个 source pad。)

    *  -  ``MEDIA_ENT_F_PROC_VIDEO_ISP``
       -  An Image Signal Processor (ISP) device. ISPs generally are one of a
	  kind devices that have their specific control interfaces using a
	  combination of custom V4L2 controls and IOCTLs, and parameters
	  supplied in a metadata buffer.
	  (图像信号处理器(ISP)设备。ISP 通常是一类独特的设备，它们使用自定义 V4L2 控件与 IOCTL 的组合、以及元数据缓冲区中提供的参数，作为其专属的控制接口。)

    *  -  ``MEDIA_ENT_F_VID_MUX``
       - Video multiplexer. An entity capable of multiplexing must have at
         least two sink pads and one source pad, and must pass the video
         frame(s) received from the active sink pad to the source pad.
         (视频多路复用器。具备多路复用能力的实体必须至少有两个 sink pad 和一个 source pad，并且必须把从活动的 sink pad 收到的视频帧传递到 source pad。)

    *  -  ``MEDIA_ENT_F_VID_IF_BRIDGE``
       - Video interface bridge. A video interface bridge entity must have at
         least one sink pad and at least one source pad. It receives video
         frames on its sink pad from an input video bus of one type (HDMI, eDP,
         MIPI CSI-2, etc.), and outputs them on its source pad to an output
         video bus of another type (eDP, MIPI CSI-2, parallel, etc.).
         (视频接口桥。视频接口桥实体必须至少有一个 sink pad 和至少一个 source pad。它在其 sink pad 上从某种类型的输入视频总线(HDMI、eDP、MIPI CSI-2 等)接收视频帧，并在其 source pad 上把它们输出到另一种类型的输出视频总线(eDP、MIPI CSI-2、并行总线等)。)

    *  -  ``MEDIA_ENT_F_DV_DECODER``
       -  Digital video decoder. The basic function of the video decoder is
	  to accept digital video from a wide variety of sources
	  and output it in some digital video standard, with appropriate
	  timing signals.
	  (数字视频解码器。视频解码器的基本功能是接受来自多种来源的数字视频，并以某种数字视频标准、连同适当的定时信号输出。)

    *  -  ``MEDIA_ENT_F_DV_ENCODER``
       -  Digital video encoder. The basic function of the video encoder is
	  to accept digital video from some digital video standard with
	  appropriate timing signals (usually a parallel video bus with sync
	  signals) and output this to a digital video output connector such
	  as HDMI or DisplayPort.
	  (数字视频编码器。视频编码器的基本功能是接受带有适当定时信号的某种数字视频标准(通常是有同步信号的并行视频总线)，并将其输出到诸如 HDMI 或 DisplayPort 的数字视频输出连接器。)

..  tabularcolumns:: |p{5.5cm}|p{12.0cm}|

.. _media-entity-flag:
.. _MEDIA-ENT-FL-DEFAULT:
.. _MEDIA-ENT-FL-CONNECTOR:

.. flat-table:: Media entity flags
    :header-rows:  0
    :stub-columns: 0

    *  -  ``MEDIA_ENT_FL_DEFAULT``
       -  Default entity for its type. Used to discover the default audio,
	  VBI and video devices, the default camera sensor, etc.
	  (其类型的默认实体。用于发现默认的音频、VBI 和视频设备、默认的摄像头传感器等。)

    *  -  ``MEDIA_ENT_FL_CONNECTOR``
       -  The entity represents a connector.(该实体表示一个连接器。)


..  tabularcolumns:: |p{6.5cm}|p{6.0cm}|p{4.8cm}|

.. _media-intf-type:
.. _MEDIA-INTF-T-DVB-FE:
.. _MEDIA-INTF-T-DVB-DEMUX:
.. _MEDIA-INTF-T-DVB-DVR:
.. _MEDIA-INTF-T-DVB-CA:
.. _MEDIA-INTF-T-DVB-NET:
.. _MEDIA-INTF-T-V4L-VIDEO:
.. _MEDIA-INTF-T-V4L-VBI:
.. _MEDIA-INTF-T-V4L-RADIO:
.. _MEDIA-INTF-T-V4L-SUBDEV:
.. _MEDIA-INTF-T-V4L-SWRADIO:
.. _MEDIA-INTF-T-V4L-TOUCH:
.. _MEDIA-INTF-T-ALSA-PCM-CAPTURE:
.. _MEDIA-INTF-T-ALSA-PCM-PLAYBACK:
.. _MEDIA-INTF-T-ALSA-CONTROL:
.. _MEDIA-INTF-T-ALSA-COMPRESS:
.. _MEDIA-INTF-T-ALSA-RAWMIDI:
.. _MEDIA-INTF-T-ALSA-HWDEP:
.. _MEDIA-INTF-T-ALSA-SEQUENCER:
.. _MEDIA-INTF-T-ALSA-TIMER:

.. flat-table:: Media interface types
    :header-rows:  0
    :stub-columns: 0

    *  -  ``MEDIA_INTF_T_DVB_FE``
       -  Device node interface for the Digital TV frontend(数字电视前端的设备节点接口)
       -  typically, /dev/dvb/adapter?/frontend?(通常为 /dev/dvb/adapter?/frontend?)

    *  -  ``MEDIA_INTF_T_DVB_DEMUX``
       -  Device node interface for the Digital TV demux(数字电视解复用器的设备节点接口)
       -  typically, /dev/dvb/adapter?/demux?(通常为 /dev/dvb/adapter?/demux?)

    *  -  ``MEDIA_INTF_T_DVB_DVR``
       -  Device node interface for the Digital TV DVR(数字电视 DVR 的设备节点接口)
       -  typically, /dev/dvb/adapter?/dvr?(通常为 /dev/dvb/adapter?/dvr?)

    *  -  ``MEDIA_INTF_T_DVB_CA``
       -  Device node interface for the Digital TV Conditional Access(数字电视条件接收的设备节点接口)
       -  typically, /dev/dvb/adapter?/ca?(通常为 /dev/dvb/adapter?/ca?)

    *  -  ``MEDIA_INTF_T_DVB_NET``
       -  Device node interface for the Digital TV network control(数字电视网络控制的设备节点接口)
       -  typically, /dev/dvb/adapter?/net?(通常为 /dev/dvb/adapter?/net?)

    *  -  ``MEDIA_INTF_T_V4L_VIDEO``
       -  Device node interface for video (V4L)(视频(V4L)的设备节点接口)
       -  typically, /dev/video?(通常为 /dev/video?)

    *  -  ``MEDIA_INTF_T_V4L_VBI``
       -  Device node interface for VBI (V4L)(VBI(V4L)的设备节点接口)
       -  typically, /dev/vbi?(通常为 /dev/vbi?)

    *  -  ``MEDIA_INTF_T_V4L_RADIO``
       -  Device node interface for radio (V4L)(收音机(V4L)的设备节点接口)
       -  typically, /dev/radio?(通常为 /dev/radio?)

    *  -  ``MEDIA_INTF_T_V4L_SUBDEV``
       -  Device node interface for a V4L subdevice(V4L 子设备的设备节点接口)
       -  typically, /dev/v4l-subdev?(通常为 /dev/v4l-subdev?)

    *  -  ``MEDIA_INTF_T_V4L_SWRADIO``
       -  Device node interface for Software Defined Radio (V4L)(软件定义收音机(V4L)的设备节点接口)
       -  typically, /dev/swradio?(通常为 /dev/swradio?)

    *  -  ``MEDIA_INTF_T_V4L_TOUCH``
       -  Device node interface for Touch device (V4L)(触摸设备(V4L)的设备节点接口)
       -  typically, /dev/v4l-touch?(通常为 /dev/v4l-touch?)

    *  -  ``MEDIA_INTF_T_ALSA_PCM_CAPTURE``
       -  Device node interface for ALSA PCM Capture(ALSA PCM 捕获的设备节点接口)
       -  typically, /dev/snd/pcmC?D?c(通常为 /dev/snd/pcmC?D?c)

    *  -  ``MEDIA_INTF_T_ALSA_PCM_PLAYBACK``
       -  Device node interface for ALSA PCM Playback(ALSA PCM 播放的设备节点接口)
       -  typically, /dev/snd/pcmC?D?p(通常为 /dev/snd/pcmC?D?p)

    *  -  ``MEDIA_INTF_T_ALSA_CONTROL``
       -  Device node interface for ALSA Control(ALSA 控件的设备节点接口)
       -  typically, /dev/snd/controlC?(通常为 /dev/snd/controlC?)

    *  -  ``MEDIA_INTF_T_ALSA_COMPRESS``
       -  Device node interface for ALSA Compress(ALSA 压缩的设备节点接口)
       -  typically, /dev/snd/compr?(通常为 /dev/snd/compr?)

    *  -  ``MEDIA_INTF_T_ALSA_RAWMIDI``
       -  Device node interface for ALSA Raw MIDI(ALSA Raw MIDI 的设备节点接口)
       -  typically, /dev/snd/midi?(通常为 /dev/snd/midi?)

    *  -  ``MEDIA_INTF_T_ALSA_HWDEP``
       -  Device node interface for ALSA Hardware Dependent(ALSA 硬件相关设备的节点接口)
       -  typically, /dev/snd/hwC?D?(通常为 /dev/snd/hwC?D?)

    *  -  ``MEDIA_INTF_T_ALSA_SEQUENCER``
       -  Device node interface for ALSA Sequencer(ALSA 音序器的设备节点接口)
       -  typically, /dev/snd/seq(通常为 /dev/snd/seq)

    *  -  ``MEDIA_INTF_T_ALSA_TIMER``
       -  Device node interface for ALSA Timer(ALSA 定时器的设备节点接口)
       -  typically, /dev/snd/timer(通常为 /dev/snd/timer)


.. tabularcolumns:: |p{5.5cm}|p{12.0cm}|

.. _media-pad-flag:
.. _MEDIA-PAD-FL-SINK:
.. _MEDIA-PAD-FL-SOURCE:
.. _MEDIA-PAD-FL-MUST-CONNECT:

.. flat-table:: Media pad flags
    :header-rows:  0
    :stub-columns: 0

    *  -  ``MEDIA_PAD_FL_SINK``
       -  Input pad, relative to the entity. Input pads sink data and are
	  targets of links.
	  (输入 pad，相对于实体而言。输入 pad 吸收(sink)数据，是连接(link)的目标。)

    *  -  ``MEDIA_PAD_FL_SOURCE``
       -  Output pad, relative to the entity. Output pads source data and
	  are origins of links.
	  (输出 pad，相对于实体而言。输出 pad 产生(source)数据，是连接的起点。)

    *  -  ``MEDIA_PAD_FL_MUST_CONNECT``
       -  If this flag is set, then for this pad to be able to stream, it must
	  be connected by at least one enabled link. There could be temporary
	  reasons (e.g. device configuration dependent) for the pad to need
	  enabled links even when this flag isn't set; the absence of the flag
	  doesn't imply there is none.
	  (如果设置了此标志，那么要使该 pad 能够流式传输，它必须由至少一条已启用的连接相连。即使未设置此标志，也可能存在临时性的原因(例如取决于设备配置)使得该 pad 需要已启用的连接；没有该标志并不意味着没有这种要求。)


One and only one of ``MEDIA_PAD_FL_SINK`` and ``MEDIA_PAD_FL_SOURCE``
must be set for every pad.
(每个 pad 必须且只能设置 ``MEDIA_PAD_FL_SINK`` 与 ``MEDIA_PAD_FL_SOURCE`` 中的一个。)

.. tabularcolumns:: |p{5.5cm}|p{12.0cm}|

.. _media-link-flag:
.. _MEDIA-LNK-FL-ENABLED:
.. _MEDIA-LNK-FL-IMMUTABLE:
.. _MEDIA-LNK-FL-DYNAMIC:
.. _MEDIA-LNK-FL-LINK-TYPE:

.. flat-table:: Media link flags
    :header-rows:  0
    :stub-columns: 0

    *  -  ``MEDIA_LNK_FL_ENABLED``
       -  The link is enabled and can be used to transfer media data. When
	  two or more links target a sink pad, only one of them can be
	  enabled at a time.
	  (该连接已启用，可用于传输媒体数据。当两条或多条连接指向同一个 sink pad 时，同一时间只能启用其中一条。)

    *  -  ``MEDIA_LNK_FL_IMMUTABLE``
       -  The link enabled state can't be modified at runtime. An immutable
	  link is always enabled.
	  (该连接的启用状态不能在运行时修改。不可变连接始终处于启用状态。)

    *  -  ``MEDIA_LNK_FL_DYNAMIC``
       -  The link enabled state can be modified during streaming. This flag
	  is set by drivers and is read-only for applications.
	  (该连接的启用状态可以在流式传输期间修改。该标志由驱动设置，对应用而言是只读的。)

    *  -  ``MEDIA_LNK_FL_LINK_TYPE``
       -  This is a bitmask that defines the type of the link. The following
	  link types are currently supported:
	  (这是一个定义连接类型的位掩码。目前支持以下连接类型：)

	  .. _MEDIA-LNK-FL-DATA-LINK:

	  ``MEDIA_LNK_FL_DATA_LINK`` for links that represent a data connection
	  between two pads.
	  ( ``MEDIA_LNK_FL_DATA_LINK`` 用于表示两个 pad 之间数据连接的连接类型。)

	  .. _MEDIA-LNK-FL-INTERFACE-LINK:

	  ``MEDIA_LNK_FL_INTERFACE_LINK`` for links that associate an entity to its
	  interface.
	  ( ``MEDIA_LNK_FL_INTERFACE_LINK`` 用于把实体与其接口关联起来的连接类型。)

	  .. _MEDIA-LNK-FL-ANCILLARY-LINK:

	  ``MEDIA_LNK_FL_ANCILLARY_LINK`` for links that represent a physical
	  relationship between two entities. The link may or may not be
	  immutable, so applications must not assume either case.
	  ( ``MEDIA_LNK_FL_ANCILLARY_LINK`` 用于表示两个实体之间物理关系的连接类型。该连接可能是也可能不是不可变的，因此应用不得假定其中任何一种情况。)
