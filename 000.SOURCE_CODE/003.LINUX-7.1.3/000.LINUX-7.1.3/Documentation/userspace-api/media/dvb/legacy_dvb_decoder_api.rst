.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later OR GPL-2.0

.. _legacy_dvb_decoder_api:

======================================================
Legacy DVB MPEG Decoder APIs(旧式 DVB MPEG 解码器 API)
======================================================

.. _legacy_dvb_decoder_notes:

General Notes(总体说明)
=======================

This API has originally been designed for DVB only and is therefore limited to
the :ref:`legacy_dvb_decoder_formats` used in such digital TV-broadcastsystems.
(此 API 最初只为 DVB 设计，因此仅限于此类数字电视广播系统中使用的
:ref:`legacy_dvb_decoder_formats` 。)

To circumvent this limitations the more versatile :ref:`V4L2 <v4l2spec>` API has
been designed. Which replaces this part of the DVB API.
(为绕开这些限制，人们设计了更加通用的 :ref:`V4L2 <v4l2spec>` API，它取代了
DVB API 的这一部分。)

Nevertheless there have been projects build around this API.
To ensure compatibility this API is kept as it is.
(不过，仍有一些项目围绕此 API 构建。为保证兼容性，此 API 保持原样。)

.. attention:: Do **not** use this API in new drivers!

    For audio and video use the :ref:`V4L2 <v4l2spec>` and ALSA APIs.
    (音频和视频请使用 :ref:`V4L2 <v4l2spec>` 和 ALSA API。)

    Pipelines should be set up using the :ref:`Media Controller  API<media_controller>`.
    (管线应使用 :ref:`Media Controller  API<media_controller>` 搭建。)

Practically the decoders seem to be treated differently. The application typically
knows which decoder is in use or it is specially written for one decoder type.
Querying capabilities are rarely used because they are already known.
(实践中，各解码器似乎受到不同对待。应用通常知道正在使用的是哪个解码器，
或者它就是专门为某种解码器类型编写的。查询能力很少被用到，因为它们早已
被知晓。)


.. _legacy_dvb_decoder_formats:

Data Formats(数据格式)
======================

The API has been designed for DVB and compatible broadcastsystems.
Because of that fact the only supported data formats are ISO/IEC 13818-1
compatible MPEG streams. The supported payloads may vary depending on the
used decoder.
(此 API 是为 DVB 及兼容的广播系统设计的。因此，唯一支持的数据格式是与
ISO/IEC 13818-1 兼容的 MPEG 流。支持的载荷可能因所用解码器而异。)

Timestamps are always MPEG PTS as defined in ITU T-REC-H.222.0 /
ISO/IEC 13818-1, if not otherwise noted.
(除非另有说明，时间戳始终是 ITU T-REC-H.222.0 / ISO/IEC 13818-1 中定义的
MPEG PTS。)

For storing recordings typically TS streams are used, in lesser extent PES.
Both variants are commonly accepted for playback, but it may be driver dependent.
(存储录像通常使用 TS 流，PES 的使用较少。两种变体通常都被接受用于播放，
但可能因驱动而异。)




Table of Contents(目录)
=======================

.. toctree::
    :maxdepth: 2

    legacy_dvb_video
    legacy_dvb_audio
    legacy_dvb_osd
