.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _fe_property_parameters:

************************************************
Digital TV property parameters(数字电视属性参数)
************************************************

There are several different Digital TV parameters that can be used by
:ref:`FE_SET_PROPERTY and FE_GET_PROPERTY ioctls<FE_GET_PROPERTY>`.
This section describes each of them. Please notice, however, that only
a subset of them are needed to setup a frontend.
(有多种不同的数字电视属性可供 :ref:`FE_SET_PROPERTY and FE_GET_PROPERTY ioctls<FE_GET_PROPERTY>` 使用。本节逐一描述它们。但请注意，设置一个前端只需要其中的一部分。)


.. _DTV-UNDEFINED:

DTV_UNDEFINED(未定义)
=====================

Used internally. A GET/SET operation for it won't change or return
anything.
(内部使用。对它的 GET/SET 操作不会改变或返回任何东西。)


.. _DTV-TUNE:

DTV_TUNE(执行调谐)
==================

Interpret the cache of data, build either a traditional frontend
tunerequest so we can pass validation in the ``FE_SET_FRONTEND`` ioctl.
(解读数据缓存，构建一个传统的前端调谐请求，使其能通过 ``FE_SET_FRONTEND`` ioctl 的校验。)


.. _DTV-CLEAR:

DTV_CLEAR(清除缓存)
===================

Reset a cache of data specific to the frontend here. This does not
effect hardware.
(在这里重置该前端特有的数据缓存。这不会影响硬件。)


.. _DTV-FREQUENCY:

DTV_FREQUENCY(频率)
===================

Frequency of the digital TV transponder/channel.
(数字电视转发器/频道的频率。)

.. note::

  #. For satellite delivery systems, the frequency is in kHz.
     (对于卫星传送系统，频率单位是 kHz。)

  #. For cable and terrestrial delivery systems, the frequency is in
     Hz.
     (对于有线和地面传送系统，频率单位是 Hz。)

  #. On most delivery systems, the frequency is the center frequency
     of the transponder/channel. The exception is for ISDB-T, where
     the main carrier has a 1/7 offset from the center.
     (在大多数传送系统中，频率是转发器/频道的中心频率。例外是 ISDB-T，它的主载波相对中心有 1/7 的偏移。)

  #. For ISDB-T, the channels are usually transmitted with an offset of
     about 143kHz. E.g. a valid frequency could be 474,143 kHz. The
     stepping is  bound to the bandwidth of the channel which is
     typically 6MHz.
     (对于 ISDB-T，频道通常以约 143kHz 的偏移发送。例如，一个有效的频率可以是 474,143 kHz。步进与信道带宽绑定，后者通常为 6MHz。)

  #. In ISDB-Tsb, the channel consists of only one or three segments the
     frequency step is 429kHz, 3*429 respectively.
     (在 ISDB-Tsb 中，信道仅由一段或三段组成，频率步进分别为 429kHz 和 3*429。)


.. _DTV-MODULATION:

DTV_MODULATION(调制)
====================

Specifies the frontend modulation type for delivery systems that
support multiple modulations.
(为支持多种调制的传送系统指定前端调制类型。)

The modulation can be one of the types defined by enum :c:type:`fe_modulation`.

Most of the digital TV standards offer more than one possible
modulation type.
(大多数数字电视标准提供的调制类型不止一种。)

The table below presents a summary of the types of modulation types
supported by each delivery system, as currently defined by specs.
(下表汇总了目前规范所定义的每种传送系统支持的调制类型。)

======================= =======================================================
Standard		Modulation types
======================= =======================================================
ATSC (version 1)	8-VSB and 16-VSB.(8-VSB 与 16-VSB。)
DMTB			4-QAM, 16-QAM, 32-QAM, 64-QAM and 4-QAM-NR.(4-QAM、16-QAM、32-QAM、64-QAM 与 4-QAM-NR。)
DVB-C Annex A/C		16-QAM, 32-QAM, 64-QAM and 256-QAM.(16-QAM、32-QAM、64-QAM 与 256-QAM。)
DVB-C Annex B		64-QAM.(64-QAM。)
DVB-C2			QPSK, 16-QAM, 64-QAM, 256-QAM, 1024-QAM and 4096-QAM.(QPSK、16-QAM、64-QAM、256-QAM、1024-QAM 与 4096-QAM。)
DVB-T			QPSK, 16-QAM and 64-QAM.(QPSK、16-QAM 与 64-QAM。)
DVB-T2			QPSK, 16-QAM, 64-QAM and 256-QAM.(QPSK、16-QAM、64-QAM 与 256-QAM。)
DVB-S			No need to set. It supports only QPSK.(无需设置。它只支持 QPSK。)
DVB-S2			QPSK, 8-PSK, 16-APSK and 32-APSK.(QPSK、8-PSK、16-APSK 与 32-APSK。)
DVB-S2X			8-APSK-L, 16-APSK-L, 32-APSK-L, 64-APSK and 64-APSK-L.(8-APSK-L、16-APSK-L、32-APSK-L、64-APSK 与 64-APSK-L。)
ISDB-T			QPSK, DQPSK, 16-QAM and 64-QAM.(QPSK、DQPSK、16-QAM 与 64-QAM。)
ISDB-S			8-PSK, QPSK and BPSK.(8-PSK、QPSK 与 BPSK。)
======================= =======================================================

.. note::

   As DVB-S2X specifies extensions to the DVB-S2 standard, the same
   delivery system enum value is used (SYS_DVBS2).
   (由于 DVB-S2X 是对 DVB-S2 标准的扩展，因此使用相同的传送系统枚举值（SYS_DVBS2）。)

   Please notice that some of the above modulation types may not be
   defined currently at the Kernel. The reason is simple: no driver
   needed such definition yet.
   (请注意，上述某些调制类型目前在内核中可能尚未定义。原因很简单：还没有驱动需要这样的定义。)


.. _DTV-BANDWIDTH-HZ:

DTV_BANDWIDTH_HZ(信道带宽)
==========================

Bandwidth for the channel, in HZ.
(信道带宽，单位为 Hz。)

Should be set only for terrestrial delivery systems.
(只应为地面传送系统设置此属性。)

Possible values: ``1712000``, ``5000000``, ``6000000``, ``7000000``,
``8000000``, ``10000000``.

======================= =======================================================
Terrestrial Standard	Possible values for bandwidth
======================= =======================================================
ATSC (version 1)	No need to set. It is always 6MHz.(无需设置。始终为 6MHz。)
DMTB			No need to set. It is always 8MHz.(无需设置。始终为 8MHz。)
DVB-T			6MHz, 7MHz and 8MHz.(6MHz、7MHz 与 8MHz。)
DVB-T2			1.172 MHz, 5MHz, 6MHz, 7MHz, 8MHz and 10MHz(1.172 MHz、5MHz、6MHz、7MHz、8MHz 与 10MHz)
ISDB-T			5MHz, 6MHz, 7MHz and 8MHz, although most places
			use 6MHz.(5MHz、6MHz、7MHz 与 8MHz，不过大多数地方使用 6MHz。)
======================= =======================================================


.. note::


  #. For ISDB-Tsb, the bandwidth can vary depending on the number of
     connected segments.

     It can be easily derived from other parameters
     (DTV_ISDBT_SB_SEGMENT_IDX, DTV_ISDBT_SB_SEGMENT_COUNT).
     (对于 ISDB-Tsb，带宽可能随相连段的数量而变化。它可以很容易地从其他参数（DTV_ISDBT_SB_SEGMENT_IDX、DTV_ISDBT_SB_SEGMENT_COUNT）推导出来。)

  #. On Satellite and Cable delivery systems, the bandwidth depends on
     the symbol rate. The kernel will silently ignore any :ref:`DTV-BANDWIDTH-HZ`
     setting and overwrites it with bandwidth estimation.

     Such bandwidth estimation takes into account the symbol rate set with
     :ref:`DTV-SYMBOL-RATE`, and the rolloff factor, with is fixed for
     DVB-C and DVB-S.

     For DVB-S2, the rolloff should also be set via :ref:`DTV-ROLLOFF`.
     (在卫星和有线传送系统上，带宽取决于符号率。内核会静默忽略任何 :ref:`DTV-BANDWIDTH-HZ` 设置，并用带宽估计值覆盖它。这种带宽估计会考虑用 :ref:`DTV-SYMBOL-RATE` 设置的符号率，以及滚降系数（rolloff factor）——对 DVB-C 和 DVB-S 而言它是固定的。对于 DVB-S2，滚降系数还应通过 :ref:`DTV-ROLLOFF` 设置。)


.. _DTV-INVERSION:

DTV_INVERSION(频谱反转)
=======================

Specifies if the frontend should do spectral inversion or not.
(指定前端是否应做频谱反转。)

The acceptable values are defined by :c:type:`fe_spectral_inversion`.


.. _DTV-DISEQC-MASTER:

DTV_DISEQC_MASTER(DiSEqC 主设备)
================================

Currently not implemented.
(目前尚未实现。)


.. _DTV-SYMBOL-RATE:

DTV_SYMBOL_RATE(符号率)
=======================

Used on cable and satellite delivery systems.
(用于有线和卫星传送系统。)

Digital TV symbol rate, in bauds (symbols/second).
(数字电视符号率，单位为波特（符号/秒）。)


.. _DTV-INNER-FEC:

DTV_INNER_FEC(内码 FEC)
=======================

Used on cable and satellite delivery systems.
(用于有线和卫星传送系统。)

The acceptable values are defined by :c:type:`fe_code_rate`.


.. _DTV-VOLTAGE:

DTV_VOLTAGE(电压)
=================

Used on satellite delivery systems.
(用于卫星传送系统。)

The voltage is usually used with non-DiSEqC capable LNBs to switch the
polarization (horizontal/vertical). When using DiSEqC equipment this
voltage has to be switched consistently to the DiSEqC commands as
described in the DiSEqC spec.
(该电压通常与不具备 DiSEqC 能力的 LNB 配合使用，用于切换极化方式（水平/垂直）。使用 DiSEqC 设备时，此电压必须按照 DiSEqC 规范的描述，与 DiSEqC 命令一致地切换。)

The acceptable values are defined by :c:type:`fe_sec_voltage`.


.. _DTV-TONE:

DTV_TONE(音调)
==============

Currently not used.
(目前未使用。)


.. _DTV-PILOT:

DTV_PILOT(导频)
===============

Used on DVB-S2.
(用于 DVB-S2。)

Sets DVB-S2 pilot.
(设置 DVB-S2 导频（pilot）。)

The acceptable values are defined by :c:type:`fe_pilot`.


.. _DTV-ROLLOFF:

DTV_ROLLOFF(滚降系数)
=====================

Used on DVB-S2.
(用于 DVB-S2。)

Sets DVB-S2 rolloff.
(设置 DVB-S2 滚降系数。)

The acceptable values are defined by :c:type:`fe_rolloff`.


.. _DTV-DISEQC-SLAVE-REPLY:

DTV_DISEQC_SLAVE_REPLY(DiSEqC 从设备应答)
=========================================

Currently not implemented.
(目前尚未实现。)


.. _DTV-FE-CAPABILITY-COUNT:

DTV_FE_CAPABILITY_COUNT(前端能力计数)
=====================================

Currently not implemented.
(目前尚未实现。)


.. _DTV-FE-CAPABILITY:

DTV_FE_CAPABILITY(前端能力)
===========================

Currently not implemented.
(目前尚未实现。)


.. _DTV-DELIVERY-SYSTEM:

DTV_DELIVERY_SYSTEM(传送系统)
=============================

Specifies the type of the delivery system.
(指定传送系统的类型。)

The acceptable values are defined by :c:type:`fe_delivery_system`.


.. _DTV-ISDBT-PARTIAL-RECEPTION:

DTV_ISDBT_PARTIAL_RECEPTION(ISDB-T 部分接收)
============================================

Used only on ISDB.
(仅用于 ISDB。)

If ``DTV_ISDBT_SOUND_BROADCASTING`` is '0' this bit field represents
whether the channel is in partial reception mode or not.
(如果 ``DTV_ISDBT_SOUND_BROADCASTING`` 为 '0'，此位域表示信道是否处于部分接收模式。)

If '1' ``DTV_ISDBT_LAYERA_*`` values are assigned to the center segment
and ``DTV_ISDBT_LAYERA_SEGMENT_COUNT`` has to be '1'.
(如果为 '1'，``DTV_ISDBT_LAYERA_*`` 的值分配给中央段，且 ``DTV_ISDBT_LAYERA_SEGMENT_COUNT`` 必须为 '1'。)

If in addition ``DTV_ISDBT_SOUND_BROADCASTING`` is '1'
``DTV_ISDBT_PARTIAL_RECEPTION`` represents whether this ISDB-Tsb channel
is consisting of one segment and layer or three segments and two layers.
(此外如果 ``DTV_ISDBT_SOUND_BROADCASTING`` 为 '1'，``DTV_ISDBT_PARTIAL_RECEPTION`` 表示该 ISDB-Tsb 信道是由一段一层组成，还是由三段两层组成。)

Possible values: 0, 1, -1 (AUTO)


.. _DTV-ISDBT-SOUND-BROADCASTING:

DTV_ISDBT_SOUND_BROADCASTING(ISDB-T 声音广播)
=============================================

Used only on ISDB.
(仅用于 ISDB。)

This field represents whether the other DTV_ISDBT_*-parameters are
referring to an ISDB-T and an ISDB-Tsb channel. (See also
``DTV_ISDBT_PARTIAL_RECEPTION``).
(此字段表示其他 DTV_ISDBT_* 参数指向的是 ISDB-T 还是 ISDB-Tsb 信道。（另见 ``DTV_ISDBT_PARTIAL_RECEPTION``）。)

Possible values: 0, 1, -1 (AUTO)


.. _DTV-ISDBT-SB-SUBCHANNEL-ID:

DTV_ISDBT_SB_SUBCHANNEL_ID(ISDB-Tsb 子信道 ID)
==============================================

Used only on ISDB.
(仅用于 ISDB。)

This field only applies if ``DTV_ISDBT_SOUND_BROADCASTING`` is '1'.
(此字段只在 ``DTV_ISDBT_SOUND_BROADCASTING`` 为 '1' 时适用。)

(Note of the author: This might not be the correct description of the
``SUBCHANNEL-ID`` in all details, but it is my understanding of the
technical background needed to program a device)
((作者注：这可能不是对 ``SUBCHANNEL-ID`` 在所有细节上都正确的描述，但这是我理解编程设备所需技术背景后的认识))

An ISDB-Tsb channel (1 or 3 segments) can be broadcasted alone or in a
set of connected ISDB-Tsb channels. In this set of channels every
channel can be received independently. The number of connected ISDB-Tsb
segment can vary, e.g. depending on the frequency spectrum bandwidth
available.
(一个 ISDB-Tsb 信道（1 或 3 段）可以单独广播，也可以在一组相连的 ISDB-Tsb 信道中广播。在这组信道中，每个信道都可以被独立接收。相连 ISDB-Tsb 段的数量可以变化，例如取决于可用的频谱带宽。)

Example: Assume 8 ISDB-Tsb connected segments are broadcasted. The
broadcaster has several possibilities to put those channels in the air:
Assuming a normal 13-segment ISDB-T spectrum he can align the 8 segments
from position 1-8 to 5-13 or anything in between.
(例如：假设广播 8 个相连的 ISDB-Tsb 段。广播者有几种把这些信道放到空中的方式：假定标准的 13 段 ISDB-T 频谱，他可以把这 8 个段对齐到位置 1-8、5-13 或两者之间的任何位置。)

The underlying layer of segments are sub-channels: each segment is
consisting of several sub-channels with a predefined IDs. A sub-channel
is used to help the demodulator to synchronize on the channel.
(段的下层是子信道：每个段由多个具有预定义 ID 的子信道组成。子信道用于帮助解调器在信道上同步。)

An ISDB-T channel is always centered over all sub-channels. As for the
example above, in ISDB-Tsb it is no longer as simple as that.
(一个 ISDB-T 信道的中心总是覆盖所有子信道。至于上面的例子，在 ISDB-Tsb 中就不再这么简单了。)

``The DTV_ISDBT_SB_SUBCHANNEL_ID`` parameter is used to give the
sub-channel ID of the segment to be demodulated.
(参数 ``The DTV_ISDBT_SB_SUBCHANNEL_ID`` 用于给出要解调的段的子信道 ID。)

Possible values: 0 .. 41, -1 (AUTO)


.. _DTV-ISDBT-SB-SEGMENT-IDX:

DTV_ISDBT_SB_SEGMENT_IDX(ISDB-Tsb 段索引)
=========================================

Used only on ISDB.
(仅用于 ISDB。)

This field only applies if ``DTV_ISDBT_SOUND_BROADCASTING`` is '1'.
(此字段只在 ``DTV_ISDBT_SOUND_BROADCASTING`` 为 '1' 时适用。)

``DTV_ISDBT_SB_SEGMENT_IDX`` gives the index of the segment to be
demodulated for an ISDB-Tsb channel where several of them are
transmitted in the connected manner.
(``DTV_ISDBT_SB_SEGMENT_IDX`` 给出要以相连方式传输多个 ISDB-Tsb 信道时、需要解调的段的索引。)

Possible values: 0 .. ``DTV_ISDBT_SB_SEGMENT_COUNT`` - 1

Note: This value cannot be determined by an automatic channel search.
(注意：这个值无法通过自动频道搜索确定。)


.. _DTV-ISDBT-SB-SEGMENT-COUNT:

DTV_ISDBT_SB_SEGMENT_COUNT(ISDB-Tsb 段数量)
===========================================

Used only on ISDB.
(仅用于 ISDB。)

This field only applies if ``DTV_ISDBT_SOUND_BROADCASTING`` is '1'.
(此字段只在 ``DTV_ISDBT_SOUND_BROADCASTING`` 为 '1' 时适用。)

``DTV_ISDBT_SB_SEGMENT_COUNT`` gives the total count of connected
ISDB-Tsb channels.
(``DTV_ISDBT_SB_SEGMENT_COUNT`` 给出相连 ISDB-Tsb 信道的总数。)

Possible values: 1 .. 13

Note: This value cannot be determined by an automatic channel search.
(注意：这个值无法通过自动频道搜索确定。)


.. _isdb-hierq-layers:

DTV-ISDBT-LAYER[A-C] parameters(ISDB-T 层参数)
==============================================

Used only on ISDB.
(仅用于 ISDB。)

ISDB-T channels can be coded hierarchically. As opposed to DVB-T in
ISDB-T hierarchical layers can be decoded simultaneously. For that
reason a ISDB-T demodulator has 3 Viterbi and 3 Reed-Solomon decoders.
(ISDB-T 信道可以分层编码。与 DVB-T 不同，ISDB-T 的分层可以被同时解码。因此，ISDB-T 解调器有 3 个维特比（Viterbi）译码器和 3 个里德-所罗门（Reed-Solomon）译码器。)

ISDB-T has 3 hierarchical layers which each can use a part of the
available segments. The total number of segments over all layers has to
13 in ISDB-T.
(ISDB-T 有 3 个分层，每个分层可以使用可用段的一部分。在 ISDB-T 中，所有层之上的段总数必须为 13。)

There are 3 parameter sets, for Layers A, B and C.
(共有 3 组参数，分别对应层 A、B 和 C。)


.. _DTV-ISDBT-LAYER-ENABLED:

DTV_ISDBT_LAYER_ENABLED(层启用)
-------------------------------

Used only on ISDB.
(仅用于 ISDB。)

Hierarchical reception in ISDB-T is achieved by enabling or disabling
layers in the decoding process. Setting all bits of
``DTV_ISDBT_LAYER_ENABLED`` to '1' forces all layers (if applicable) to
be demodulated. This is the default.
(ISDB-T 中的分层接收是通过在解码过程中启用或禁用层来实现的。把 ``DTV_ISDBT_LAYER_ENABLED`` 的所有位都设为 '1' 会强制（在适用的情况下）解调所有层。这是默认值。)

If the channel is in the partial reception mode
(``DTV_ISDBT_PARTIAL_RECEPTION`` = 1) the central segment can be decoded
independently of the other 12 segments. In that mode layer A has to have
a ``SEGMENT_COUNT`` of 1.
(如果信道处于部分接收模式（``DTV_ISDBT_PARTIAL_RECEPTION`` = 1），中央段可以独立于其他 12 段被解码。在该模式下，层 A 的 ``SEGMENT_COUNT`` 必须为 1。)

In ISDB-Tsb only layer A is used, it can be 1 or 3 in ISDB-Tsb according
to ``DTV_ISDBT_PARTIAL_RECEPTION``. ``SEGMENT_COUNT`` must be filled
accordingly.
(在 ISDB-Tsb 中只使用层 A，依据 ``DTV_ISDBT_PARTIAL_RECEPTION`` 它可以是 1 或 3。``SEGMENT_COUNT`` 必须相应地填写。)

Only the values of the first 3 bits are used. Other bits will be silently ignored:
(只使用前 3 位的值。其他位会被静默忽略：)

``DTV_ISDBT_LAYER_ENABLED`` bit 0: layer A enabled
(``DTV_ISDBT_LAYER_ENABLED`` 第 0 位：启用层 A。)

``DTV_ISDBT_LAYER_ENABLED`` bit 1: layer B enabled
(``DTV_ISDBT_LAYER_ENABLED`` 第 1 位：启用层 B。)

``DTV_ISDBT_LAYER_ENABLED`` bit 2: layer C enabled
(``DTV_ISDBT_LAYER_ENABLED`` 第 2 位：启用层 C。)

``DTV_ISDBT_LAYER_ENABLED`` bits 3-31: unused
(``DTV_ISDBT_LAYER_ENABLED`` 第 3-31 位：未使用。)


.. _DTV-ISDBT-LAYER-FEC:

DTV_ISDBT_LAYER[A-C]_FEC(层 FEC)
--------------------------------

Used only on ISDB.
(仅用于 ISDB。)

The Forward Error Correction mechanism used by a given ISDB Layer, as
defined by :c:type:`fe_code_rate`.
(给定 ISDB 层使用的前向纠错（FEC）机制，由 :c:type:`fe_code_rate` 定义。)


Possible values are: ``FEC_AUTO``, ``FEC_1_2``, ``FEC_2_3``, ``FEC_3_4``,
``FEC_5_6``, ``FEC_7_8``


.. _DTV-ISDBT-LAYER-MODULATION:

DTV_ISDBT_LAYER[A-C]_MODULATION(层调制)
---------------------------------------

Used only on ISDB.
(仅用于 ISDB。)

The modulation used by a given ISDB Layer, as defined by
:c:type:`fe_modulation`.
(给定 ISDB 层使用的调制方式，由 :c:type:`fe_modulation` 定义。)

Possible values are: ``QAM_AUTO``, ``QPSK``, ``QAM_16``, ``QAM_64``, ``DQPSK``

.. note::

   #. If layer C is ``DQPSK``, then layer B has to be ``DQPSK``.
      (如果层 C 是 ``DQPSK``，那么层 B 必须是 ``DQPSK``。)

   #. If layer B is ``DQPSK`` and ``DTV_ISDBT_PARTIAL_RECEPTION``\ = 0,
      then layer has to be ``DQPSK``.
      (如果层 B 是 ``DQPSK`` 且 ``DTV_ISDBT_PARTIAL_RECEPTION``\ = 0，那么层 A 必须是 ``DQPSK``。)


.. _DTV-ISDBT-LAYER-SEGMENT-COUNT:

DTV_ISDBT_LAYER[A-C]_SEGMENT_COUNT(层段数量)
--------------------------------------------

Used only on ISDB.
(仅用于 ISDB。)

Possible values: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -1 (AUTO)

Note: Truth table for ``DTV_ISDBT_SOUND_BROADCASTING`` and
``DTV_ISDBT_PARTIAL_RECEPTION`` and ``LAYER[A-C]_SEGMENT_COUNT``
(注意：``DTV_ISDBT_SOUND_BROADCASTING``、``DTV_ISDBT_PARTIAL_RECEPTION`` 与 ``LAYER[A-C]_SEGMENT_COUNT`` 的真值表)

.. _isdbt-layer_seg-cnt-table:

.. flat-table:: Truth table for ISDB-T Sound Broadcasting
    :header-rows:  1
    :stub-columns: 0


    -  .. row 1

       -  Partial Reception(部分接收)

       -  Sound Broadcasting(声音广播)

       -  Layer A width(层 A 宽度)

       -  Layer B width(层 B 宽度)

       -  Layer C width(层 C 宽度)

       -  total width(总宽度)

    -  .. row 2

       -  0

       -  0

       -  1 .. 13

       -  1 .. 13

       -  1 .. 13

       -  13

    -  .. row 3

       -  1

       -  0

       -  1

       -  1 .. 13

       -  1 .. 13

       -  13

    -  .. row 4

       -  0

       -  1

       -  1

       -  0

       -  0

       -  1

    -  .. row 5

       -  1

       -  1

       -  1

       -  2

       -  0

       -  13



.. _DTV-ISDBT-LAYER-TIME-INTERLEAVING:

DTV_ISDBT_LAYER[A-C]_TIME_INTERLEAVING(层时间交织)
--------------------------------------------------

Used only on ISDB.
(仅用于 ISDB。)

Valid values: 0, 1, 2, 4, -1 (AUTO)

when DTV_ISDBT_SOUND_BROADCASTING is active, value 8 is also valid.
(当 DTV_ISDBT_SOUND_BROADCASTING 生效时，值 8 也是有效的。)

Note: The real time interleaving length depends on the mode (fft-size).
The values here are referring to what can be found in the
TMCC-structure, as shown in the table below.
(注意：实际的时间交织长度取决于模式（fft 大小）。这里的值指的是 TMCC 结构中的取值，如下表所示。)


.. c:type:: isdbt_layer_interleaving_table

.. flat-table:: ISDB-T time interleaving modes
    :header-rows:  1
    :stub-columns: 0


    -  .. row 1

       -  ``DTV_ISDBT_LAYER[A-C]_TIME_INTERLEAVING`` (层时间交织)

       -  Mode 1 (2K FFT)(模式 1（2K FFT）)

       -  Mode 2 (4K FFT)(模式 2（4K FFT）)

       -  Mode 3 (8K FFT)(模式 3（8K FFT）)

    -  .. row 2

       -  0

       -  0

       -  0

       -  0

    -  .. row 3

       -  1

       -  4

       -  2

       -  1

    -  .. row 4

       -  2

       -  8

       -  4

       -  2

    -  .. row 5

       -  4

       -  16

       -  8

       -  4



.. _DTV-ATSCMH-FIC-VER:

DTV_ATSCMH_FIC_VER(ATSC-MH FIC 版本)
------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Version number of the FIC (Fast Information Channel) signaling data.
(FIC（快速信息信道，Fast Information Channel）信令数据的版本号。)

FIC is used for relaying information to allow rapid service acquisition
by the receiver.
(FIC 用于传递信息，使接收机能够快速获取服务。)

Possible values: 0, 1, 2, 3, ..., 30, 31


.. _DTV-ATSCMH-PARADE-ID:

DTV_ATSCMH_PARADE_ID(Parade 编号)
---------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Parade identification number
(Parade（队列）标识编号。)

A parade is a collection of up to eight MH groups, conveying one or two
ensembles.
(一个 parade 是最多八个 MH 组的集合，承载一个或两个 ensemble（集合）。)

Possible values: 0, 1, 2, 3, ..., 126, 127


.. _DTV-ATSCMH-NOG:

DTV_ATSCMH_NOG(每组 MH 组数)
----------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Number of MH groups per MH subframe for a designated parade.
(指定 parade 中每个 MH 子帧的 MH 组数。)

Possible values: 1, 2, 3, 4, 5, 6, 7, 8


.. _DTV-ATSCMH-TNOG:

DTV_ATSCMH_TNOG(MH 组总数)
--------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Total number of MH groups including all MH groups belonging to all MH
parades in one MH subframe.
(一个 MH 子帧中、属于所有 MH parade 的全部 MH 组的总数。)

Possible values: 0, 1, 2, 3, ..., 30, 31


.. _DTV-ATSCMH-SGN:

DTV_ATSCMH_SGN(起始组号)
------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Start group number.
(起始组编号。)

Possible values: 0, 1, 2, 3, ..., 14, 15


.. _DTV-ATSCMH-PRC:

DTV_ATSCMH_PRC(Parade 重复周期)
-------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Parade repetition cycle.
(Parade 重复周期。)

Possible values: 1, 2, 3, 4, 5, 6, 7, 8


.. _DTV-ATSCMH-RS-FRAME-MODE:

DTV_ATSCMH_RS_FRAME_MODE(RS 帧模式)
-----------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Reed Solomon (RS) frame mode.
(里德-所罗门（RS）帧模式。)

The acceptable values are defined by :c:type:`atscmh_rs_frame_mode`.


.. _DTV-ATSCMH-RS-FRAME-ENSEMBLE:

DTV_ATSCMH_RS_FRAME_ENSEMBLE(RS 帧集合)
---------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Reed Solomon (RS) frame ensemble.
(里德-所罗门（RS）帧集合。)

The acceptable values are defined by :c:type:`atscmh_rs_frame_ensemble`.


.. _DTV-ATSCMH-RS-CODE-MODE-PRI:

DTV_ATSCMH_RS_CODE_MODE_PRI(RS 码模式（主）)
--------------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Reed Solomon (RS) code mode (primary).
(里德-所罗门（RS）码模式（主）。)

The acceptable values are defined by :c:type:`atscmh_rs_code_mode`.


.. _DTV-ATSCMH-RS-CODE-MODE-SEC:

DTV_ATSCMH_RS_CODE_MODE_SEC(RS 码模式（次）)
--------------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Reed Solomon (RS) code mode (secondary).
(里德-所罗门（RS）码模式（次）。)

The acceptable values are defined by :c:type:`atscmh_rs_code_mode`.


.. _DTV-ATSCMH-SCCC-BLOCK-MODE:

DTV_ATSCMH_SCCC_BLOCK_MODE(SCCC 块模式)
---------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Series Concatenated Convolutional Code Block Mode.
(串行级联卷积码块模式。)

The acceptable values are defined by :c:type:`atscmh_sccc_block_mode`.


.. _DTV-ATSCMH-SCCC-CODE-MODE-A:

DTV_ATSCMH_SCCC_CODE_MODE_A(SCCC 码率 A)
----------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Series Concatenated Convolutional Code Rate.
(串行级联卷积码码率。)

The acceptable values are defined by :c:type:`atscmh_sccc_code_mode`.

.. _DTV-ATSCMH-SCCC-CODE-MODE-B:

DTV_ATSCMH_SCCC_CODE_MODE_B(SCCC 码率 B)
----------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Series Concatenated Convolutional Code Rate.
(串行级联卷积码码率。)

Possible values are the same as documented on enum
:c:type:`atscmh_sccc_code_mode`.
(可能的取值与枚举 :c:type:`atscmh_sccc_code_mode` 中记录的相同。)


.. _DTV-ATSCMH-SCCC-CODE-MODE-C:

DTV_ATSCMH_SCCC_CODE_MODE_C(SCCC 码率 C)
----------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Series Concatenated Convolutional Code Rate.
(串行级联卷积码码率。)

Possible values are the same as documented on enum
:c:type:`atscmh_sccc_code_mode`.
(可能的取值与枚举 :c:type:`atscmh_sccc_code_mode` 中记录的相同。)


.. _DTV-ATSCMH-SCCC-CODE-MODE-D:

DTV_ATSCMH_SCCC_CODE_MODE_D(SCCC 码率 D)
----------------------------------------

Used only on ATSC-MH.
(仅用于 ATSC-MH。)

Series Concatenated Convolutional Code Rate.
(串行级联卷积码码率。)

Possible values are the same as documented on enum
:c:type:`atscmh_sccc_code_mode`.
(可能的取值与枚举 :c:type:`atscmh_sccc_code_mode` 中记录的相同。)


.. _DTV-API-VERSION:

DTV_API_VERSION(API 版本)
=========================

Returns the major/minor version of the Digital TV API
(返回数字电视 API 的主/次版本号。)


.. _DTV-CODE-RATE-HP:

DTV_CODE_RATE_HP(高优先级码率)
==============================

Used on terrestrial transmissions.
(用于地面传输。)

The acceptable values are defined by :c:type:`fe_transmit_mode`.


.. _DTV-CODE-RATE-LP:

DTV_CODE_RATE_LP(低优先级码率)
==============================

Used on terrestrial transmissions.
(用于地面传输。)

The acceptable values are defined by :c:type:`fe_transmit_mode`.


.. _DTV-GUARD-INTERVAL:

DTV_GUARD_INTERVAL(保护间隔)
============================

The acceptable values are defined by :c:type:`fe_guard_interval`.

.. note::

   #. If ``DTV_GUARD_INTERVAL`` is set the ``GUARD_INTERVAL_AUTO`` the
      hardware will try to find the correct guard interval (if capable) and
      will use TMCC to fill in the missing parameters.
      (如果 ``DTV_GUARD_INTERVAL`` 被设置为 ``GUARD_INTERVAL_AUTO``，硬件会（在有能力时）尝试找出正确的保护间隔，并使用 TMCC 填充缺失的参数。)
   #. Interval ``GUARD_INTERVAL_1_64`` is used only for DVB-C2.
      (间隔 ``GUARD_INTERVAL_1_64`` 只用于 DVB-C2。)
   #. Interval ``GUARD_INTERVAL_1_128`` is used for both DVB-C2 and DVB_T2.
      (间隔 ``GUARD_INTERVAL_1_128`` 同时用于 DVB-C2 和 DVB_T2。)
   #. Intervals ``GUARD_INTERVAL_19_128`` and ``GUARD_INTERVAL_19_256`` are
      used only for DVB-T2.
      (间隔 ``GUARD_INTERVAL_19_128`` 和 ``GUARD_INTERVAL_19_256`` 只用于 DVB-T2。)
   #. Intervals ``GUARD_INTERVAL_PN420``, ``GUARD_INTERVAL_PN595`` and
      ``GUARD_INTERVAL_PN945`` are used only for DMTB at the present.
      On such standard, only those intervals and ``GUARD_INTERVAL_AUTO``
      are valid.
      (间隔 ``GUARD_INTERVAL_PN420``、``GUARD_INTERVAL_PN595`` 和 ``GUARD_INTERVAL_PN945`` 目前只用于 DMTB。在该标准上，只有这些间隔和 ``GUARD_INTERVAL_AUTO`` 是有效的。)

.. _DTV-TRANSMISSION-MODE:

DTV_TRANSMISSION_MODE(传输模式)
===============================


Used only on OFTM-based standards, e. g. DVB-T/T2, ISDB-T, DTMB.
(仅用于基于 OFDM 的标准，例如 DVB-T/T2、ISDB-T、DTMB。)

Specifies the FFT size (with corresponds to the approximate number of
carriers) used by the standard.
(指定该标准使用的 FFT 大小（对应近似载波数）。)

The acceptable values are defined by :c:type:`fe_transmit_mode`.

.. note::

   #. ISDB-T supports three carrier/symbol-size: 8K, 4K, 2K. It is called
      **mode** on such standard, and are numbered from 1 to 3:
      (ISDB-T 支持三种载波/符号大小：8K、4K、2K。在该标准上称为 **mode**，编号从 1 到 3：)

      ====	========	========================
      Mode	FFT size	Transmission mode
      ====	========	========================
      1		2K		``TRANSMISSION_MODE_2K``
      2		4K		``TRANSMISSION_MODE_4K``
      3		8K		``TRANSMISSION_MODE_8K``
      ====	========	========================

   #. If ``DTV_TRANSMISSION_MODE`` is set the ``TRANSMISSION_MODE_AUTO``
      the hardware will try to find the correct FFT-size (if capable) and
      will use TMCC to fill in the missing parameters.
      (如果 ``DTV_TRANSMISSION_MODE`` 被设置为 ``TRANSMISSION_MODE_AUTO``，硬件会（在有能力时）尝试找出正确的 FFT 大小，并使用 TMCC 填充缺失的参数。)

   #. DVB-T specifies 2K and 8K as valid sizes.
      (DVB-T 规定 2K 和 8K 为有效大小。)

   #. DVB-T2 specifies 1K, 2K, 4K, 8K, 16K and 32K.
      (DVB-T2 规定 1K、2K、4K、8K、16K 和 32K。)

   #. DTMB specifies C1 and C3780.
      (DTMB 规定 C1 和 C3780。)


.. _DTV-HIERARCHY:

DTV_HIERARCHY(分层)
===================

Used only on DVB-T and DVB-T2.
(仅用于 DVB-T 和 DVB-T2。)

Frontend hierarchy.
(前端分层。)

The acceptable values are defined by :c:type:`fe_hierarchy`.


.. _DTV-STREAM-ID:

DTV_STREAM_ID(流 ID)
====================

Used on DVB-C2, DVB-S2, DVB-T2 and ISDB-S.
(用于 DVB-C2、DVB-S2、DVB-T2 和 ISDB-S。)

DVB-C2, DVB-S2, DVB-T2 and ISDB-S support the transmission of several
streams on a single transport stream. This property enables the digital
TV driver to handle substream filtering, when supported by the hardware.
By default, substream filtering is disabled.
(DVB-C2、DVB-S2、DVB-T2 和 ISDB-S 支持在单个传输流上传输多个流。此属性使数字电视驱动程序能够在硬件支持时处理子流过滤。默认情况下，子流过滤是禁用的。)

For DVB-C2, DVB-S2 and DVB-T2, the valid substream id range is from 0 to
255.
(对于 DVB-C2、DVB-S2 和 DVB-T2，有效的子流 id 范围是 0 到 255。)

For ISDB, the valid substream id range is from 1 to 65535.
(对于 ISDB，有效的子流 id 范围是 1 到 65535。)

To disable it, you should use the special macro NO_STREAM_ID_FILTER.
(要禁用它，应使用特殊宏 NO_STREAM_ID_FILTER。)

Note: any value outside the id range also disables filtering.
(注意：任何超出 id 范围的值也会禁用过滤。)


.. _DTV-DVBT2-PLP-ID-LEGACY:

DTV_DVBT2_PLP_ID_LEGACY(旧版 PLP ID)
====================================

Obsolete, replaced with DTV_STREAM_ID.
(已废弃，由 DTV_STREAM_ID 取代。)


.. _DTV-ENUM-DELSYS:

DTV_ENUM_DELSYS(枚举传送系统)
=============================

A Multi standard frontend needs to advertise the delivery systems
provided. Applications need to enumerate the provided delivery systems,
before using any other operation with the frontend. Prior to its
introduction, FE_GET_INFO was used to determine a frontend type. A
frontend which provides more than a single delivery system,
FE_GET_INFO doesn't help much. Applications which intends to use a
multistandard frontend must enumerate the delivery systems associated
with it, rather than trying to use FE_GET_INFO. In the case of a
legacy frontend, the result is just the same as with FE_GET_INFO, but
in a more structured format.
(多标准前端需要通告它所提供的传送系统。应用程序在对前端使用任何其他操作之前，需要枚举其提供的传送系统。在此属性引入之前，人们用 FE_GET_INFO 来确定前端类型。对于提供不止一种传送系统的前端，FE_GET_INFO 帮助不大。打算使用多标准前端的应用程序必须枚举与该前端相关联的传送系统，而不是试图使用 FE_GET_INFO。对于传统前端，结果与使用 FE_GET_INFO 相同，只是格式更结构化。)

The acceptable values are defined by :c:type:`fe_delivery_system`.


.. _DTV-INTERLEAVING:

DTV_INTERLEAVING(交织)
======================

Time interleaving to be used.
(要使用的时间交织。)

The acceptable values are defined by :c:type:`fe_interleaving`.


.. _DTV-LNA:

DTV_LNA(低噪声放大器)
=====================

Low-noise amplifier.
(低噪声放大器。)

Hardware might offer controllable LNA which can be set manually using
that parameter. Usually LNA could be found only from terrestrial devices
if at all.
(硬件可能提供可控的 LNA，可以使用该参数手动设置。通常，如果有的话，LNA 一般只在地面设备上才有。)

Possible values: 0, 1, LNA_AUTO

0, LNA off
(0，LNA 关闭。)

1, LNA on
(1，LNA 打开。)

use the special macro LNA_AUTO to set LNA auto
(使用特殊宏 LNA_AUTO 把 LNA 设为自动。)


.. _DTV-SCRAMBLING-SEQUENCE-INDEX:

DTV_SCRAMBLING_SEQUENCE_INDEX(加扰序列索引)
===========================================

Used on DVB-S2.
(用于 DVB-S2。)

This 18 bit field, when present, carries the index of the DVB-S2 physical
layer scrambling sequence as defined in clause 5.5.4 of EN 302 307.
There is no explicit signalling method to convey scrambling sequence index
to the receiver. If S2 satellite delivery system descriptor is available
it can be used to read the scrambling sequence index (EN 300 468 table 41).
(这个 18 位字段（如果存在）承载 EN 302 307 第 5.5.4 节定义的 DVB-S2 物理层加扰序列的索引。没有显式的信令方法把加扰序列索引传递给接收机。如果存在 S2 卫星传送系统描述符，可以用它读取加扰序列索引（EN 300 468 表 41）。)

By default, gold scrambling sequence index 0 is used.
(默认使用 gold 加扰序列索引 0。)

The valid scrambling sequence index range is from 0 to 262142.
(有效的加扰序列索引范围是 0 到 262142。)
