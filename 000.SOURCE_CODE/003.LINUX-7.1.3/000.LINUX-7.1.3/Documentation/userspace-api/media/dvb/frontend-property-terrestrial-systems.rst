.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _frontend-property-terrestrial-systems:

***********************************************************************
Properties used on terrestrial delivery systems(地面传送系统使用的属性)
***********************************************************************


.. _dvbt-params:

DVB-T delivery system(DVB-T 传送系统)
=====================================

The following parameters are valid for DVB-T:
(以下参数对 DVB-T 有效：)

-  :ref:`DTV_API_VERSION <DTV-API-VERSION>`

-  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`

-  :ref:`DTV_TUNE <DTV-TUNE>`

-  :ref:`DTV_CLEAR <DTV-CLEAR>`

-  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>`

-  :ref:`DTV_MODULATION <DTV-MODULATION>`

-  :ref:`DTV_BANDWIDTH_HZ <DTV-BANDWIDTH-HZ>`

-  :ref:`DTV_INVERSION <DTV-INVERSION>`

-  :ref:`DTV_CODE_RATE_HP <DTV-CODE-RATE-HP>`

-  :ref:`DTV_CODE_RATE_LP <DTV-CODE-RATE-LP>`

-  :ref:`DTV_GUARD_INTERVAL <DTV-GUARD-INTERVAL>`

-  :ref:`DTV_TRANSMISSION_MODE <DTV-TRANSMISSION-MODE>`

-  :ref:`DTV_HIERARCHY <DTV-HIERARCHY>`

-  :ref:`DTV_LNA <DTV-LNA>`

In addition, the :ref:`DTV QoS statistics <frontend-stat-properties>`
are also valid.
(此外，DTV QoS 统计信息也是有效的。)


.. _dvbt2-params:

DVB-T2 delivery system(DVB-T2 传送系统)
========================================

DVB-T2 support is currently in the early stages of development, so
expect that this section may grow and become more detailed with time.
(对 DVB-T2 的支持目前还处于开发的早期阶段，因此可以预期本节内容会随着
时间推移而增长并变得更加详细。)

The following parameters are valid for DVB-T2:
(以下参数对 DVB-T2 有效：)

-  :ref:`DTV_API_VERSION <DTV-API-VERSION>`

-  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`

-  :ref:`DTV_TUNE <DTV-TUNE>`

-  :ref:`DTV_CLEAR <DTV-CLEAR>`

-  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>`

-  :ref:`DTV_MODULATION <DTV-MODULATION>`

-  :ref:`DTV_BANDWIDTH_HZ <DTV-BANDWIDTH-HZ>`

-  :ref:`DTV_INVERSION <DTV-INVERSION>`

-  :ref:`DTV_CODE_RATE_HP <DTV-CODE-RATE-HP>`

-  :ref:`DTV_CODE_RATE_LP <DTV-CODE-RATE-LP>`

-  :ref:`DTV_GUARD_INTERVAL <DTV-GUARD-INTERVAL>`

-  :ref:`DTV_TRANSMISSION_MODE <DTV-TRANSMISSION-MODE>`

-  :ref:`DTV_HIERARCHY <DTV-HIERARCHY>`

-  :ref:`DTV_STREAM_ID <DTV-STREAM-ID>`

-  :ref:`DTV_LNA <DTV-LNA>`

In addition, the :ref:`DTV QoS statistics <frontend-stat-properties>`
are also valid.
(此外，DTV QoS 统计信息也是有效的。)


.. _isdbt:

ISDB-T delivery system(ISDB-T 传送系统)
=======================================

This ISDB-T/ISDB-Tsb API extension should reflect all information needed
to tune any ISDB-T/ISDB-Tsb hardware. Of course it is possible that some
very sophisticated devices won't need certain parameters to tune.
(这一 ISDB-T/ISDB-Tsb API 扩展应当反映调谐任何 ISDB-T/ISDB-Tsb 硬件所需的
全部信息。当然，某些非常复杂的设备在调谐时可能不需要其中的某些参数。)

The information given here should help application writers to know how
to handle ISDB-T and ISDB-Tsb hardware using the Linux Digital TV API.
(这里给出的信息应当能帮助应用程序编写者了解如何使用 Linux 数字电视 API
来操作 ISDB-T 与 ISDB-Tsb 硬件。)

The details given here about ISDB-T and ISDB-Tsb are just enough to
basically show the dependencies between the needed parameter values, but
surely some information is left out. For more detailed information see
the following documents:
(这里给出的关于 ISDB-T 与 ISDB-Tsb 的细节只够基本地展示所需参数值之间的
依赖关系，但肯定遗漏了一些信息。更详细的信息参见以下文档：)

ARIB STD-B31 - "Transmission System for Digital Terrestrial Television
Broadcasting" and
(ARIB STD-B31 - "Transmission System for Digital Terrestrial Television
Broadcasting"（《数字地面电视广播传输系统》）以及)

ARIB TR-B14 - "Operational Guidelines for Digital Terrestrial Television
Broadcasting".
(ARIB TR-B14 - "Operational Guidelines for Digital Terrestrial Television
Broadcasting"（《数字地面电视广播运营指南》）。)

In order to understand the ISDB specific parameters, one has to have
some knowledge the channel structure in ISDB-T and ISDB-Tsb. I.e. it has
to be known to the reader that an ISDB-T channel consists of 13
segments, that it can have up to 3 layer sharing those segments, and
things like that.
(要理解 ISDB 特有的参数，必须对 ISDB-T 与 ISDB-Tsb 的信道结构有所了解。
也就是说，读者必须知道一个 ISDB-T 信道由 13 个段 (segment) 组成、最多
可以有 3 个层 (layer) 共享这些段之类的知识。)

The following parameters are valid for ISDB-T:
(以下参数对 ISDB-T 有效：)

-  :ref:`DTV_API_VERSION <DTV-API-VERSION>`

-  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`

-  :ref:`DTV_TUNE <DTV-TUNE>`

-  :ref:`DTV_CLEAR <DTV-CLEAR>`

-  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>`

-  :ref:`DTV_BANDWIDTH_HZ <DTV-BANDWIDTH-HZ>`

-  :ref:`DTV_INVERSION <DTV-INVERSION>`

-  :ref:`DTV_GUARD_INTERVAL <DTV-GUARD-INTERVAL>`

-  :ref:`DTV_TRANSMISSION_MODE <DTV-TRANSMISSION-MODE>`

-  :ref:`DTV_ISDBT_LAYER_ENABLED <DTV-ISDBT-LAYER-ENABLED>`

-  :ref:`DTV_ISDBT_PARTIAL_RECEPTION <DTV-ISDBT-PARTIAL-RECEPTION>`

-  :ref:`DTV_ISDBT_SOUND_BROADCASTING <DTV-ISDBT-SOUND-BROADCASTING>`

-  :ref:`DTV_ISDBT_SB_SUBCHANNEL_ID <DTV-ISDBT-SB-SUBCHANNEL-ID>`

-  :ref:`DTV_ISDBT_SB_SEGMENT_IDX <DTV-ISDBT-SB-SEGMENT-IDX>`

-  :ref:`DTV_ISDBT_SB_SEGMENT_COUNT <DTV-ISDBT-SB-SEGMENT-COUNT>`

-  :ref:`DTV_ISDBT_LAYERA_FEC <DTV-ISDBT-LAYER-FEC>`

-  :ref:`DTV_ISDBT_LAYERA_MODULATION <DTV-ISDBT-LAYER-MODULATION>`

-  :ref:`DTV_ISDBT_LAYERA_SEGMENT_COUNT <DTV-ISDBT-LAYER-SEGMENT-COUNT>`

-  :ref:`DTV_ISDBT_LAYERA_TIME_INTERLEAVING <DTV-ISDBT-LAYER-TIME-INTERLEAVING>`

-  :ref:`DTV_ISDBT_LAYERB_FEC <DTV-ISDBT-LAYER-FEC>`

-  :ref:`DTV_ISDBT_LAYERB_MODULATION <DTV-ISDBT-LAYER-MODULATION>`

-  :ref:`DTV_ISDBT_LAYERB_SEGMENT_COUNT <DTV-ISDBT-LAYER-SEGMENT-COUNT>`

-  :ref:`DTV_ISDBT_LAYERB_TIME_INTERLEAVING <DTV-ISDBT-LAYER-TIME-INTERLEAVING>`

-  :ref:`DTV_ISDBT_LAYERC_FEC <DTV-ISDBT-LAYER-FEC>`

-  :ref:`DTV_ISDBT_LAYERC_MODULATION <DTV-ISDBT-LAYER-MODULATION>`

-  :ref:`DTV_ISDBT_LAYERC_SEGMENT_COUNT <DTV-ISDBT-LAYER-SEGMENT-COUNT>`

-  :ref:`DTV_ISDBT_LAYERC_TIME_INTERLEAVING <DTV-ISDBT-LAYER-TIME-INTERLEAVING>`

In addition, the :ref:`DTV QoS statistics <frontend-stat-properties>`
are also valid.
(此外，DTV QoS 统计信息也是有效的。)


.. _atsc-params:

ATSC delivery system(ATSC 传送系统)
===================================

The following parameters are valid for ATSC:
(以下参数对 ATSC 有效：)

-  :ref:`DTV_API_VERSION <DTV-API-VERSION>`

-  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`

-  :ref:`DTV_TUNE <DTV-TUNE>`

-  :ref:`DTV_CLEAR <DTV-CLEAR>`

-  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>`

-  :ref:`DTV_MODULATION <DTV-MODULATION>`

-  :ref:`DTV_BANDWIDTH_HZ <DTV-BANDWIDTH-HZ>`

In addition, the :ref:`DTV QoS statistics <frontend-stat-properties>`
are also valid.
(此外，DTV QoS 统计信息也是有效的。)


.. _atscmh-params:

ATSC-MH delivery system(ATSC-MH 传送系统)
=========================================

The following parameters are valid for ATSC-MH:
(以下参数对 ATSC-MH 有效：)

-  :ref:`DTV_API_VERSION <DTV-API-VERSION>`

-  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`

-  :ref:`DTV_TUNE <DTV-TUNE>`

-  :ref:`DTV_CLEAR <DTV-CLEAR>`

-  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>`

-  :ref:`DTV_BANDWIDTH_HZ <DTV-BANDWIDTH-HZ>`

-  :ref:`DTV_ATSCMH_FIC_VER <DTV-ATSCMH-FIC-VER>`

-  :ref:`DTV_ATSCMH_PARADE_ID <DTV-ATSCMH-PARADE-ID>`

-  :ref:`DTV_ATSCMH_NOG <DTV-ATSCMH-NOG>`

-  :ref:`DTV_ATSCMH_TNOG <DTV-ATSCMH-TNOG>`

-  :ref:`DTV_ATSCMH_SGN <DTV-ATSCMH-SGN>`

-  :ref:`DTV_ATSCMH_PRC <DTV-ATSCMH-PRC>`

-  :ref:`DTV_ATSCMH_RS_FRAME_MODE <DTV-ATSCMH-RS-FRAME-MODE>`

-  :ref:`DTV_ATSCMH_RS_FRAME_ENSEMBLE <DTV-ATSCMH-RS-FRAME-ENSEMBLE>`

-  :ref:`DTV_ATSCMH_RS_CODE_MODE_PRI <DTV-ATSCMH-RS-CODE-MODE-PRI>`

-  :ref:`DTV_ATSCMH_RS_CODE_MODE_SEC <DTV-ATSCMH-RS-CODE-MODE-SEC>`

-  :ref:`DTV_ATSCMH_SCCC_BLOCK_MODE <DTV-ATSCMH-SCCC-BLOCK-MODE>`

-  :ref:`DTV_ATSCMH_SCCC_CODE_MODE_A <DTV-ATSCMH-SCCC-CODE-MODE-A>`

-  :ref:`DTV_ATSCMH_SCCC_CODE_MODE_B <DTV-ATSCMH-SCCC-CODE-MODE-B>`

-  :ref:`DTV_ATSCMH_SCCC_CODE_MODE_C <DTV-ATSCMH-SCCC-CODE-MODE-C>`

-  :ref:`DTV_ATSCMH_SCCC_CODE_MODE_D <DTV-ATSCMH-SCCC-CODE-MODE-D>`

In addition, the :ref:`DTV QoS statistics <frontend-stat-properties>`
are also valid.
(此外，DTV QoS 统计信息也是有效的。)


.. _dtmb-params:

DTMB delivery system(DTMB 传送系统)
===================================

The following parameters are valid for DTMB:
(以下参数对 DTMB 有效：)

-  :ref:`DTV_API_VERSION <DTV-API-VERSION>`

-  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`

-  :ref:`DTV_TUNE <DTV-TUNE>`

-  :ref:`DTV_CLEAR <DTV-CLEAR>`

-  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>`

-  :ref:`DTV_MODULATION <DTV-MODULATION>`

-  :ref:`DTV_BANDWIDTH_HZ <DTV-BANDWIDTH-HZ>`

-  :ref:`DTV_INVERSION <DTV-INVERSION>`

-  :ref:`DTV_INNER_FEC <DTV-INNER-FEC>`

-  :ref:`DTV_GUARD_INTERVAL <DTV-GUARD-INTERVAL>`

-  :ref:`DTV_TRANSMISSION_MODE <DTV-TRANSMISSION-MODE>`

-  :ref:`DTV_INTERLEAVING <DTV-INTERLEAVING>`

-  :ref:`DTV_LNA <DTV-LNA>`

In addition, the :ref:`DTV QoS statistics <frontend-stat-properties>`
are also valid.
(此外，DTV QoS 统计信息也是有效的。)
