.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

****************************
Compressed Formats(压缩格式)
****************************


.. _compressed-formats:

.. raw:: latex

    \small

.. tabularcolumns:: |p{5.8cm}|p{1.2cm}|p{10.3cm}|

.. cssclass:: longtable

.. flat-table:: Compressed Image Formats
    :header-rows:  1
    :stub-columns: 0
    :widths:       3 1 4

    * - Identifier(标识符)
      - Code(代码)
      - Details(详情)
    * .. _V4L2-PIX-FMT-JPEG:

      - ``V4L2_PIX_FMT_JPEG``
      - 'JPEG'
      - TBD. See also :ref:`VIDIOC_G_JPEGCOMP <VIDIOC_G_JPEGCOMP>`,
	:ref:`VIDIOC_S_JPEGCOMP <VIDIOC_G_JPEGCOMP>`.
	(待定。另参见 VIDIOC_G_JPEGCOMP 与 VIDIOC_S_JPEGCOMP。)
    * .. _V4L2-PIX-FMT-MPEG:

      - ``V4L2_PIX_FMT_MPEG``
      - 'MPEG'
      - MPEG multiplexed stream. The actual format is determined by
	extended control ``V4L2_CID_MPEG_STREAM_TYPE``, see
	:ref:`mpeg-control-id`.
	(MPEG 复用流。实际格式由扩展控制项 ``V4L2_CID_MPEG_STREAM_TYPE``
	决定，参见 mpeg-control-id 一节。)
    * .. _V4L2-PIX-FMT-H264:

      - ``V4L2_PIX_FMT_H264``
      - 'H264'
      - H264 Access Unit.
	The decoder expects one Access Unit per buffer.
	The encoder generates one Access Unit per buffer.
	If :ref:`VIDIOC_ENUM_FMT` reports ``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``
	then the decoder has no requirements since it can parse all the
	information from the raw bytestream.
	(H264 访问单元 (Access Unit)。解码器期望每个缓冲区一个访问单元。
	编码器每个缓冲区生成一个访问单元。如果 VIDIOC_ENUM_FMT 报告
	``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``，那么解码器没有任何要求，
	因为它可以从原始字节流中解析出全部信息。)
    * .. _V4L2-PIX-FMT-H264-NO-SC:

      - ``V4L2_PIX_FMT_H264_NO_SC``
      - 'AVC1'
      - H264 video elementary stream without start codes.
	(不带起始码的 H264 视频基本流。)
    * .. _V4L2-PIX-FMT-H264-MVC:

      - ``V4L2_PIX_FMT_H264_MVC``
      - 'M264'
      - H264 MVC video elementary stream.
	(H264 MVC 视频基本流。)
    * .. _V4L2-PIX-FMT-H264-SLICE:

      - ``V4L2_PIX_FMT_H264_SLICE``
      - 'S264'
      - H264 parsed slice data, including slice headers, either with or
	without the start code, as extracted from the H264 bitstream.
	This format is adapted for stateless video decoders that implement an
	H264 pipeline with the :ref:`stateless_decoder`.
	This pixelformat has two modifiers that must be set at least once
	through the ``V4L2_CID_STATELESS_H264_DECODE_MODE``
        and ``V4L2_CID_STATELESS_H264_START_CODE`` controls.
	In addition, metadata associated with the frame to decode are
	required to be passed through the ``V4L2_CID_STATELESS_H264_SPS``,
	``V4L2_CID_STATELESS_H264_PPS``,
	``V4L2_CID_STATELESS_H264_SCALING_MATRIX``,
	``V4L2_CID_STATELESS_H264_SLICE_PARAMS`` and
	``V4L2_CID_STATELESS_H264_DECODE_PARAMS`` controls.  See the
	:ref:`associated Codec Control IDs <v4l2-codec-stateless-h264>`.
        Exactly one output and one capture buffer must be provided for use
	with this pixel format. The output buffer must contain the
	appropriate number of macroblocks to decode a full
	corresponding frame to the matching capture buffer.
	(H264 解析后的条带 (slice) 数据，包括条带头，带或不带起始码均可，
	从 H264 位流中提取。该格式适用于借助无状态解码器 (stateless
	decoder) 实现 H264 管线的无状态视频解码器。此像素格式有两个修饰符，
	必须至少通过 ``V4L2_CID_STATELESS_H264_DECODE_MODE`` 与
	``V4L2_CID_STATELESS_H264_START_CODE`` 控制项设置一次。此外，与待
	解码帧相关联的元数据必须通过 ``V4L2_CID_STATELESS_H264_SPS``、
	``V4L2_CID_STATELESS_H264_PPS``、
	``V4L2_CID_STATELESS_H264_SCALING_MATRIX``、
	``V4L2_CID_STATELESS_H264_SLICE_PARAMS`` 与
	``V4L2_CID_STATELESS_H264_DECODE_PARAMS`` 控制项传递。参见相关的
	Codec Control ID 一节。使用此像素格式时必须恰好提供一个输出缓冲区和
	一个采集缓冲区。输出缓冲区必须包含适当数量的宏块，以便把一个完整的
	对应帧解码到匹配的采集缓冲区。)

	The syntax for this format is documented in :ref:`h264`, section
	7.3.2.8 "Slice layer without partitioning RBSP syntax" and the following
	sections.
	(此格式的语法在 h264 一节的 7.3.2.8 节 "Slice layer without
	partitioning RBSP syntax" 及后续小节中记录。)

    * .. _V4L2-PIX-FMT-H263:

      - ``V4L2_PIX_FMT_H263``
      - 'H263'
      - H263 video elementary stream.
	(H263 视频基本流。)
    * .. _V4L2-PIX-FMT-SPK:

      - ``V4L2_PIX_FMT_SPK``
      - 'SPK0'
      - Sorenson Spark is an implementation of H.263 for use in Flash Video and Adobe Flash files
	(Sorenson Spark 是 H.263 的一种实现，用于 Flash Video 与 Adobe
	Flash 文件)
    * .. _V4L2-PIX-FMT-MPEG1:

      - ``V4L2_PIX_FMT_MPEG1``
      - 'MPG1'
      - MPEG1 Picture. Each buffer starts with a Picture header, followed
	by other headers as needed and ending with the Picture data.
	If :ref:`VIDIOC_ENUM_FMT` reports ``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``
	then the decoder has no requirements since it can parse all the
	information from the raw bytestream.
	(MPEG1 图像 (Picture)。每个缓冲区以一个图像头开始，随后是根据需要
	的其他头，最后是图像数据。如果 VIDIOC_ENUM_FMT 报告
	``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``，那么解码器没有任何要求，
	因为它可以从原始字节流中解析出全部信息。)
    * .. _V4L2-PIX-FMT-MPEG2:

      - ``V4L2_PIX_FMT_MPEG2``
      - 'MPG2'
      - MPEG2 Picture. Each buffer starts with a Picture header, followed
	by other headers as needed and ending with the Picture data.
	If :ref:`VIDIOC_ENUM_FMT` reports ``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``
	then the decoder has no requirements since it can parse all the
	information from the raw bytestream.
	(MPEG2 图像。每个缓冲区以一个图像头开始，随后是根据需要的其他头，
	最后是图像数据。如果 VIDIOC_ENUM_FMT 报告
	``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``，那么解码器没有任何要求，
	因为它可以从原始字节流中解析出全部信息。)
    * .. _V4L2-PIX-FMT-MPEG2-SLICE:

      - ``V4L2_PIX_FMT_MPEG2_SLICE``
      - 'MG2S'
      - MPEG-2 parsed slice data, as extracted from the MPEG-2 bitstream.
	This format is adapted for stateless video decoders that implement a
	MPEG-2 pipeline with the :ref:`stateless_decoder`.
	Metadata associated with the frame to decode is required to be passed
	through the ``V4L2_CID_STATELESS_MPEG2_SEQUENCE`` and
        ``V4L2_CID_STATELESS_MPEG2_PICTURE`` controls.
        Quantisation matrices can optionally be specified through the
	``V4L2_CID_STATELESS_MPEG2_QUANTISATION`` control.
	See the :ref:`associated Codec Control IDs <v4l2-codec-stateless-mpeg2>`.
	Exactly one output and one capture buffer must be provided for use with
	this pixel format. The output buffer must contain the appropriate number
	of macroblocks to decode a full corresponding frame to the matching
	capture buffer.
	(MPEG-2 解析后的条带数据，从 MPEG-2 位流中提取。该格式适用于借助
	无状态解码器实现 MPEG-2 管线的无状态视频解码器。与待解码帧相关联的
	元数据必须通过 ``V4L2_CID_STATELESS_MPEG2_SEQUENCE`` 与
	``V4L2_CID_STATELESS_MPEG2_PICTURE`` 控制项传递。量化矩阵可以可选地
	通过 ``V4L2_CID_STATELESS_MPEG2_QUANTISATION`` 控制项指定。参见相关
	的 Codec Control ID 一节。使用此像素格式时必须恰好提供一个输出缓冲区
	和一个采集缓冲区。输出缓冲区必须包含适当数量的宏块，以便把一个完整
	的对应帧解码到匹配的采集缓冲区。)
    * .. _V4L2-PIX-FMT-MPEG4:

      - ``V4L2_PIX_FMT_MPEG4``
      - 'MPG4'
      - MPEG4 video elementary stream.
	(MPEG4 视频基本流。)
    * .. _V4L2-PIX-FMT-XVID:

      - ``V4L2_PIX_FMT_XVID``
      - 'XVID'
      - Xvid video elementary stream.
	(Xvid 视频基本流。)
    * .. _V4L2-PIX-FMT-VC1-ANNEX-G:

      - ``V4L2_PIX_FMT_VC1_ANNEX_G``
      - 'VC1G'
      - VC1, SMPTE 421M Annex G compliant stream.
	(符合 SMPTE 421M 附件 G 的 VC1 流。)
    * .. _V4L2-PIX-FMT-VC1-ANNEX-L:

      - ``V4L2_PIX_FMT_VC1_ANNEX_L``
      - 'VC1L'
      - VC1, SMPTE 421M Annex L compliant stream.
	(符合 SMPTE 421M 附件 L 的 VC1 流。)
    * .. _V4L2-PIX-FMT-VP8:

      - ``V4L2_PIX_FMT_VP8``
      - 'VP80'
      - VP8 compressed video frame. The encoder generates one
	compressed frame per buffer, and the decoder requires one
	compressed frame per buffer.
	(VP8 压缩视频帧。编码器每个缓冲区生成一个压缩帧，解码器要求每个
	缓冲区一个压缩帧。)
    * .. _V4L2-PIX-FMT-VP8-FRAME:

      - ``V4L2_PIX_FMT_VP8_FRAME``
      - 'VP8F'
      - VP8 parsed frame, including the frame header, as extracted from the container.
	This format is adapted for stateless video decoders that implement an
	VP8 pipeline with the :ref:`stateless_decoder`.
	Metadata associated with the frame to decode is required to be passed
	through the ``V4L2_CID_STATELESS_VP8_FRAME`` control.
	See the :ref:`associated Codec Control IDs <v4l2-codec-stateless-vp8>`.
	Exactly one output and one capture buffer must be provided for use with
	this pixel format. The output buffer must contain the appropriate number
	of macroblocks to decode a full corresponding frame to the matching
	capture buffer.
	(VP8 解析后的帧，包括帧头，从容器中提取。该格式适用于借助无状态
	解码器实现 VP8 管线的无状态视频解码器。与待解码帧相关联的元数据必须
	通过 ``V4L2_CID_STATELESS_VP8_FRAME`` 控制项传递。参见相关的 Codec
	Control ID 一节。使用此像素格式时必须恰好提供一个输出缓冲区和一个
	采集缓冲区。输出缓冲区必须包含适当数量的宏块，以便把一个完整的对应
	帧解码到匹配的采集缓冲区。)

    * .. _V4L2-PIX-FMT-VP9:

      - ``V4L2_PIX_FMT_VP9``
      - 'VP90'
      - VP9 compressed video frame. The encoder generates one
	compressed frame per buffer, and the decoder requires one
	compressed frame per buffer.
	(VP9 压缩视频帧。编码器每个缓冲区生成一个压缩帧，解码器要求每个
	缓冲区一个压缩帧。)
    * .. _V4L2-PIX-FMT-VP9-FRAME:

      - ``V4L2_PIX_FMT_VP9_FRAME``
      - 'VP9F'
      - VP9 parsed frame, including the frame header, as extracted from the container.
	This format is adapted for stateless video decoders that implement a
	VP9 pipeline with the :ref:`stateless_decoder`.
	Metadata associated with the frame to decode is required to be passed
	through the ``V4L2_CID_STATELESS_VP9_FRAME`` and
	the ``V4L2_CID_STATELESS_VP9_COMPRESSED_HDR`` controls.
	See the :ref:`associated Codec Control IDs <v4l2-codec-stateless-vp9>`.
	Exactly one output and one capture buffer must be provided for use with
	this pixel format. The output buffer must contain the appropriate number
	of macroblocks to decode a full corresponding frame to the matching
	capture buffer.
	(VP9 解析后的帧，包括帧头，从容器中提取。该格式适用于借助无状态
	解码器实现 VP9 管线的无状态视频解码器。与待解码帧相关联的元数据必须
	通过 ``V4L2_CID_STATELESS_VP9_FRAME`` 与
	``V4L2_CID_STATELESS_VP9_COMPRESSED_HDR`` 控制项传递。参见相关的
	Codec Control ID 一节。使用此像素格式时必须恰好提供一个输出缓冲区和
	一个采集缓冲区。输出缓冲区必须包含适当数量的宏块，以便把一个完整的
	对应帧解码到匹配的采集缓冲区。)
    * .. _V4L2-PIX-FMT-HEVC:

      - ``V4L2_PIX_FMT_HEVC``
      - 'HEVC'
      - HEVC/H.265 Access Unit.
	The decoder expects one Access Unit per buffer.
	The encoder generates one Access Unit per buffer.
	If :ref:`VIDIOC_ENUM_FMT` reports ``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``
	then the decoder has no	requirements since it can parse all the
	information from the raw bytestream.
	(HEVC/H.265 访问单元。解码器期望每个缓冲区一个访问单元。编码器每个
	缓冲区生成一个访问单元。如果 VIDIOC_ENUM_FMT 报告
	``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``，那么解码器没有任何要求，
	因为它可以从原始字节流中解析出全部信息。)
    * .. _V4L2-PIX-FMT-HEVC-SLICE:

      - ``V4L2_PIX_FMT_HEVC_SLICE``
      - 'S265'
      - HEVC parsed slice data, as extracted from the HEVC bitstream.
	This format is adapted for stateless video decoders that implement a
	HEVC pipeline (using the :ref:`mem2mem` and :ref:`media-request-api`).
	This pixelformat has two modifiers that must be set at least once
	through the ``V4L2_CID_MPEG_VIDEO_HEVC_DECODE_MODE``
        and ``V4L2_CID_MPEG_VIDEO_HEVC_START_CODE`` controls.
	Metadata associated with the frame to decode is required to be passed
	through the following controls:
        ``V4L2_CID_MPEG_VIDEO_HEVC_SPS``,
        ``V4L2_CID_MPEG_VIDEO_HEVC_PPS``, and
        ``V4L2_CID_MPEG_VIDEO_HEVC_SLICE_PARAMS``.
	See the :ref:`associated Codec Control IDs <v4l2-codec-stateless-hevc>`.
	Buffers associated with this pixel format must contain the appropriate
	number of macroblocks to decode a full corresponding frame.
	(HEVC 解析后的条带数据，从 HEVC 位流中提取。该格式适用于 (使用
	mem2mem 与 media-request-api) 实现 HEVC 管线的无状态视频解码器。
	此像素格式有两个修饰符，必须至少通过
	``V4L2_CID_MPEG_VIDEO_HEVC_DECODE_MODE`` 与
	``V4L2_CID_MPEG_VIDEO_HEVC_START_CODE`` 控制项设置一次。与待解码帧
	相关联的元数据必须通过以下控制项传递：
	``V4L2_CID_MPEG_VIDEO_HEVC_SPS``、
	``V4L2_CID_MPEG_VIDEO_HEVC_PPS`` 与
	``V4L2_CID_MPEG_VIDEO_HEVC_SLICE_PARAMS``。参见相关的 Codec
	Control ID 一节。与此像素格式相关联的缓冲区必须包含适当数量的宏块，
	以解码一个完整的对应帧。)
    * .. _V4L2-PIX-FMT-FWHT:

      - ``V4L2_PIX_FMT_FWHT``
      - 'FWHT'
      - Video elementary stream using a codec based on the Fast Walsh Hadamard
        Transform. This codec is implemented by the vicodec ('Virtual Codec')
	driver. See the codec-fwht.h header for more details.
	:ref:`VIDIOC_ENUM_FMT` reports ``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``
	since the decoder can parse all the information from the raw bytestream.
	(使用基于快速沃尔什-阿达玛变换 (Fast Walsh Hadamard Transform) 的
	编解码器的视频基本流。该编解码器由 vicodec ("虚拟编解码器") 驱动
	实现。更多细节参见 codec-fwht.h 头文件。VIDIOC_ENUM_FMT 报告
	``V4L2_FMT_FLAG_CONTINUOUS_BYTESTREAM``，因为解码器可以从原始字节
	流中解析出全部信息。)
    * .. _V4L2-PIX-FMT-FWHT-STATELESS:

      - ``V4L2_PIX_FMT_FWHT_STATELESS``
      - 'SFWH'
      - Same format as V4L2_PIX_FMT_FWHT but requires stateless codec implementation.
        Metadata associated with the frame to decode is required to be passed
        through the ``V4L2_CID_STATELESS_FWHT_PARAMS`` control.
	See the :ref:`associated Codec Control ID <codec-stateless-fwht>`.
	(与 V4L2_PIX_FMT_FWHT 相同的格式，但要求无状态编解码器实现。与待
	解码帧相关联的元数据必须通过 ``V4L2_CID_STATELESS_FWHT_PARAMS``
	控制项传递。参见相关的 Codec Control ID 一节。)
    * .. _V4L2-PIX-FMT-RV30:

      - ``V4L2_PIX_FMT_RV30``
      - 'RV30'
      - RealVideo, or also spelled as Real Video, is a suite of
        proprietary video compression formats developed by
        RealNetworks - the specific format changes with the version.
        RealVideo codecs are identified by four-character codes.
        RV30 corresponds to RealVideo 8, suspected to be based
        largely on an early draft of H.264
	(RealVideo (也写作 Real Video) 是 RealNetworks 开发的一组专有视频
	压缩格式 —— 具体格式随版本而变化。RealVideo 编解码器用四字符码
	标识。RV30 对应 RealVideo 8，据信在很大程度上基于 H.264 的早期
	草案)
    * .. _V4L2-PIX-FMT-RV40:

      - ``V4L2_PIX_FMT_RV40``
      - 'RV40'
      - RV40 represents RealVideo 9 and RealVideo 10.
        RealVideo 9, suspected to be based on H.264.
        RealVideo 10, aka RV9 EHQ, This refers to an improved encoder
        for the RV9 format that is fully backwards compatible with
        RV9 players - the format and decoder did not change, only
        the encoder did. As a result, it uses the same FourCC.
	(RV40 代表 RealVideo 9 与 RealVideo 10。RealVideo 9 据信基于
	H.264。RealVideo 10 又称 RV9 EHQ，指的是 RV9 格式的一种改进编码器，
	与 RV9 播放器完全向后兼容 —— 格式和解码器没有变，只有编码器变了。
	因此它使用相同的 FourCC。)

    * .. _V4L2-PIX-FMT-AV1-FRAME:

      - ``V4L2_PIX_FMT_AV1_FRAME``
      - 'AV1F'
      - AV1 parsed frame, including the frame header, as extracted from the container.
        This format is adapted for stateless video decoders that implement a AV1
        pipeline with the :ref:`stateless_decoder`. Metadata associated with the
        frame to decode is required to be passed through the
        ``V4L2_CID_STATELESS_AV1_SEQUENCE``, ``V4L2_CID_STATELESS_AV1_FRAME``,
        and ``V4L2_CID_STATELESS_AV1_TILE_GROUP_ENTRY`` controls.
        See the :ref:`associated Codec Control IDs <v4l2-codec-stateless-av1>`.
        Exactly one output and one capture buffer must be provided for use with
        this pixel format. The output buffer must contain the appropriate number
        of macroblocks to decode a full corresponding frame to the matching
        capture buffer.
	(AV1 解析后的帧，包括帧头，从容器中提取。该格式适用于借助无状态
	解码器实现 AV1 管线的无状态视频解码器。与待解码帧相关联的元数据
	必须通过 ``V4L2_CID_STATELESS_AV1_SEQUENCE``、
	``V4L2_CID_STATELESS_AV1_FRAME`` 与
	``V4L2_CID_STATELESS_AV1_TILE_GROUP_ENTRY`` 控制项传递。参见相关的
	Codec Control ID 一节。使用此像素格式时必须恰好提供一个输出缓冲区
	和一个采集缓冲区。输出缓冲区必须包含适当数量的宏块，以便把一个完整
	的对应帧解码到匹配的采集缓冲区。)

    * .. _V4L2-PIX-FMT-AV1:

      - ``V4L2_PIX_FMT_AV1``
      - 'AV01'
      - AV1 compressed video frame. This format is adapted for implementing AV1
        pipeline. The decoder implements stateful video decoder and expects one
        temporal unit per buffer in OBU stream format.
        The encoder generates one Temporal Unit per buffer.
	(AV1 压缩视频帧。该格式适用于实现 AV1 管线。解码器实现有状态视频
	解码器，并期望每个缓冲区一个时间单元 (temporal unit)，采用 OBU 流
	格式。编码器每个缓冲区生成一个时间单元。)
.. raw:: latex

    \normalsize
