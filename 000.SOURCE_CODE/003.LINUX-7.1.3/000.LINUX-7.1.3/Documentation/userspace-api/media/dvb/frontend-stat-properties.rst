.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _frontend-stat-properties:

**********************************************
Frontend statistics indicators(前端统计指标)
**********************************************

The values are returned via ``dtv_property.stat``. If the property is
supported, ``dtv_property.stat.len`` is bigger than zero.
(这些值通过 ``dtv_property.stat`` 返回。如果该属性受支持，``dtv_property.stat.len`` 会大于零。)

For most delivery systems, ``dtv_property.stat.len`` will be 1 if the
stats is supported, and the properties will return a single value for
each parameter.
(对大多数传输系统而言，如果统计受支持，``dtv_property.stat.len`` 将为 1，且各属性会为每个参数返回单个值。)

It should be noted, however, that new OFDM delivery systems like ISDB
can use different modulation types for each group of carriers. On such
standards, up to 3 groups of statistics can be provided, and
``dtv_property.stat.len`` is updated to reflect the "global" metrics,
plus one metric per each carrier group (called "layer" on ISDB).
(不过应当注意，像 ISDB 这样的新 OFDM 传输系统可能对每组载波使用不同的调制类型。在这类标准上，最多可以提供 3 组统计，且 ``dtv_property.stat.len`` 会更新为反映 "global" (全局) 度量加上每载波组一个度量 (在 ISDB 中称为 "layer" (层))。)

So, in order to be consistent with other delivery systems, the first
value at :c:type:`dtv_property.stat.dtv_stats <dtv_stats>` array refers
to the global metric. The other elements of the array represent each
layer, starting from layer A(index 1), layer B (index 2) and so on.
(因此，为了与其他传输系统保持一致，:c:type:`dtv_property.stat.dtv_stats <dtv_stats>` 数组的首个值指的是全局度量。数组的其余元素依次表示每一层，从层 A (索引 1)、层 B (索引 2) 开始，依此类推。)

The number of filled elements are stored at ``dtv_property.stat.len``.
(已填充元素的数量存储在 ``dtv_property.stat.len`` 中。)

Each element of the ``dtv_property.stat.dtv_stats`` array consists on
two elements:
(``dtv_property.stat.dtv_stats`` 数组的每个元素由两个部分组成：)

-  ``svalue`` or ``uvalue``, where ``svalue`` is for signed values of
   the measure (dB measures) and ``uvalue`` is for unsigned values
   (counters, relative scale)
   (``svalue`` 或 ``uvalue`` ：``svalue`` 用于度量值的有符号数 (dB 度量)，``uvalue`` 用于无符号数 (计数器、相对刻度)。)

-  ``scale`` - Scale for the value. It can be:
   (``scale`` - 数值的刻度。可以是：)

   -  ``FE_SCALE_NOT_AVAILABLE`` - The parameter is supported by the
      frontend, but it was not possible to collect it (could be a
      transitory or permanent condition)
      (``FE_SCALE_NOT_AVAILABLE`` - 前端支持该参数，但无法采集到它 (可能是暂时性或永久性的状况)。)

   -  ``FE_SCALE_DECIBEL`` - parameter is a signed value, measured in
      1/1000 dB
      (``FE_SCALE_DECIBEL`` - 参数是有符号值，以 1/1000 dB 为单位度量。)

   -  ``FE_SCALE_RELATIVE`` - parameter is a unsigned value, where 0
      means 0% and 65535 means 100%.
      (``FE_SCALE_RELATIVE`` - 参数是无符号值，0 表示 0%，65535 表示 100%。)

   -  ``FE_SCALE_COUNTER`` - parameter is a unsigned value that counts
      the occurrence of an event, like bit error, block error, or lapsed
      time.
      (``FE_SCALE_COUNTER`` - 参数是无符号值，统计某个事件的发生次数，如比特错误、块错误或流逝时间。)


.. _DTV-STAT-SIGNAL-STRENGTH:

DTV_STAT_SIGNAL_STRENGTH
========================

Indicates the signal strength level at the analog part of the tuner or
of the demod.
(指示调谐器或解调器模拟部分的信号强度电平。)

Possible scales for this metric are:
(该度量可能的刻度有：)

-  ``FE_SCALE_NOT_AVAILABLE`` - it failed to measure it, or the
   measurement was not complete yet.
   (``FE_SCALE_NOT_AVAILABLE`` - 测量失败，或测量尚未完成。)

-  ``FE_SCALE_DECIBEL`` - signal strength is in 0.001 dBm units, power
   measured in miliwatts. This value is generally negative.
   (``FE_SCALE_DECIBEL`` - 信号强度以 0.001 dBm 为单位，功率以毫瓦度量。该值通常为负。)

-  ``FE_SCALE_RELATIVE`` - The frontend provides a 0% to 100%
   measurement for power (actually, 0 to 65535).
   (``FE_SCALE_RELATIVE`` - 前端提供功率的 0% 到 100% 度量 (实际是 0 到 65535)。)


.. _DTV-STAT-CNR:

DTV_STAT_CNR
============

Indicates the Signal to Noise ratio for the main carrier.
(指示主载波的信噪比。)

Possible scales for this metric are:
(该度量可能的刻度有：)

-  ``FE_SCALE_NOT_AVAILABLE`` - it failed to measure it, or the
   measurement was not complete yet.
   (``FE_SCALE_NOT_AVAILABLE`` - 测量失败，或测量尚未完成。)

-  ``FE_SCALE_DECIBEL`` - Signal/Noise ratio is in 0.001 dB units.
   (``FE_SCALE_DECIBEL`` - 信噪比以 0.001 dB 为单位。)

-  ``FE_SCALE_RELATIVE`` - The frontend provides a 0% to 100%
   measurement for Signal/Noise (actually, 0 to 65535).
   (``FE_SCALE_RELATIVE`` - 前端提供信噪比的 0% 到 100% 度量 (实际是 0 到 65535)。)


.. _DTV-STAT-PRE-ERROR-BIT-COUNT:

DTV_STAT_PRE_ERROR_BIT_COUNT
============================

Measures the number of bit errors before the forward error correction
(FEC) on the inner coding block (before Viterbi, LDPC or other inner
code).
(度量内层编码块中前向纠错 (FEC) 之前的比特错误数 (在 Viterbi、LDPC 或其他内码之前)。)

This measure is taken during the same interval as
``DTV_STAT_PRE_TOTAL_BIT_COUNT``.
(该度量与 ``DTV_STAT_PRE_TOTAL_BIT_COUNT`` 在相同的时间间隔内采集。)

In order to get the BER (Bit Error Rate) measurement, it should be
divided by
:ref:`DTV_STAT_PRE_TOTAL_BIT_COUNT <DTV-STAT-PRE-TOTAL-BIT-COUNT>`.
(要得到 BER (比特误码率) 度量，应将其除以
:ref:`DTV_STAT_PRE_TOTAL_BIT_COUNT <DTV-STAT-PRE-TOTAL-BIT-COUNT>`。)

This measurement is monotonically increased, as the frontend gets more
bit count measurements. The frontend may reset it when a
channel/transponder is tuned.
(该度量随前端获得更多比特计数而单调递增。调谐到新的频道/转发器时，前端可能会将其复位。)

Possible scales for this metric are:
(该度量可能的刻度有：)

-  ``FE_SCALE_NOT_AVAILABLE`` - it failed to measure it, or the
   measurement was not complete yet.
   (``FE_SCALE_NOT_AVAILABLE`` - 测量失败，或测量尚未完成。)

-  ``FE_SCALE_COUNTER`` - Number of error bits counted before the inner
   coding.
   (``FE_SCALE_COUNTER`` - 在内层编码之前统计的错误比特数。)


.. _DTV-STAT-PRE-TOTAL-BIT-COUNT:

DTV_STAT_PRE_TOTAL_BIT_COUNT
============================

Measures the amount of bits received before the inner code block, during
the same period as
:ref:`DTV_STAT_PRE_ERROR_BIT_COUNT <DTV-STAT-PRE-ERROR-BIT-COUNT>`
measurement was taken.
(度量内层编码块之前接收到的比特数，采集周期与
:ref:`DTV_STAT_PRE_ERROR_BIT_COUNT <DTV-STAT-PRE-ERROR-BIT-COUNT>`
度量相同。)

It should be noted that this measurement can be smaller than the total
amount of bits on the transport stream, as the frontend may need to
manually restart the measurement, losing some data between each
measurement interval.
(应注意该度量可能小于传输流上的总比特数，因为前端可能需要手动重启测量，从而在每个测量间隔之间丢失一些数据。)

This measurement is monotonically increased, as the frontend gets more
bit count measurements. The frontend may reset it when a
channel/transponder is tuned.
(该度量随前端获得更多比特计数而单调递增。调谐到新的频道/转发器时，前端可能会将其复位。)

Possible scales for this metric are:
(该度量可能的刻度有：)

-  ``FE_SCALE_NOT_AVAILABLE`` - it failed to measure it, or the
   measurement was not complete yet.
   (``FE_SCALE_NOT_AVAILABLE`` - 测量失败，或测量尚未完成。)

-  ``FE_SCALE_COUNTER`` - Number of bits counted while measuring
   :ref:`DTV_STAT_PRE_ERROR_BIT_COUNT <DTV-STAT-PRE-ERROR-BIT-COUNT>`.
   (``FE_SCALE_COUNTER`` - 测量
   :ref:`DTV_STAT_PRE_ERROR_BIT_COUNT <DTV-STAT-PRE-ERROR-BIT-COUNT>`
   时统计的比特数。)


.. _DTV-STAT-POST-ERROR-BIT-COUNT:

DTV_STAT_POST_ERROR_BIT_COUNT
=============================

Measures the number of bit errors after the forward error correction
(FEC) done by inner code block (after Viterbi, LDPC or other inner
code).
(度量内层编码块完成前向纠错 (FEC) 之后的比特错误数 (在 Viterbi、LDPC 或其他内码之后)。)

This measure is taken during the same interval as
``DTV_STAT_POST_TOTAL_BIT_COUNT``.
(该度量与 ``DTV_STAT_POST_TOTAL_BIT_COUNT`` 在相同的时间间隔内采集。)

In order to get the BER (Bit Error Rate) measurement, it should be
divided by
:ref:`DTV_STAT_POST_TOTAL_BIT_COUNT <DTV-STAT-POST-TOTAL-BIT-COUNT>`.
(要得到 BER (比特误码率) 度量，应将其除以
:ref:`DTV_STAT_POST_TOTAL_BIT_COUNT <DTV-STAT-POST-TOTAL-BIT-COUNT>`。)

This measurement is monotonically increased, as the frontend gets more
bit count measurements. The frontend may reset it when a
channel/transponder is tuned.
(该度量随前端获得更多比特计数而单调递增。调谐到新的频道/转发器时，前端可能会将其复位。)

Possible scales for this metric are:
(该度量可能的刻度有：)

-  ``FE_SCALE_NOT_AVAILABLE`` - it failed to measure it, or the
   measurement was not complete yet.
   (``FE_SCALE_NOT_AVAILABLE`` - 测量失败，或测量尚未完成。)

-  ``FE_SCALE_COUNTER`` - Number of error bits counted after the inner
   coding.
   (``FE_SCALE_COUNTER`` - 在内层编码之后统计的错误比特数。)


.. _DTV-STAT-POST-TOTAL-BIT-COUNT:

DTV_STAT_POST_TOTAL_BIT_COUNT
=============================

Measures the amount of bits received after the inner coding, during the
same period as
:ref:`DTV_STAT_POST_ERROR_BIT_COUNT <DTV-STAT-POST-ERROR-BIT-COUNT>`
measurement was taken.
(度量内层编码之后接收到的比特数，采集周期与
:ref:`DTV_STAT_POST_ERROR_BIT_COUNT <DTV-STAT-POST-ERROR-BIT-COUNT>`
度量相同。)

It should be noted that this measurement can be smaller than the total
amount of bits on the transport stream, as the frontend may need to
manually restart the measurement, losing some data between each
measurement interval.
(应注意该度量可能小于传输流上的总比特数，因为前端可能需要手动重启测量，从而在每个测量间隔之间丢失一些数据。)

This measurement is monotonically increased, as the frontend gets more
bit count measurements. The frontend may reset it when a
channel/transponder is tuned.
(该度量随前端获得更多比特计数而单调递增。调谐到新的频道/转发器时，前端可能会将其复位。)

Possible scales for this metric are:
(该度量可能的刻度有：)

-  ``FE_SCALE_NOT_AVAILABLE`` - it failed to measure it, or the
   measurement was not complete yet.
   (``FE_SCALE_NOT_AVAILABLE`` - 测量失败，或测量尚未完成。)

-  ``FE_SCALE_COUNTER`` - Number of bits counted while measuring
   :ref:`DTV_STAT_POST_ERROR_BIT_COUNT <DTV-STAT-POST-ERROR-BIT-COUNT>`.
   (``FE_SCALE_COUNTER`` - 测量
   :ref:`DTV_STAT_POST_ERROR_BIT_COUNT <DTV-STAT-POST-ERROR-BIT-COUNT>`
   时统计的比特数。)


.. _DTV-STAT-ERROR-BLOCK-COUNT:

DTV_STAT_ERROR_BLOCK_COUNT
==========================

Measures the number of block errors after the outer forward error
correction coding (after Reed-Solomon or other outer code).
(度量外层前向纠错编码之后的块错误数 (在 Reed-Solomon 或其他外码之后)。)

This measurement is monotonically increased, as the frontend gets more
bit count measurements. The frontend may reset it when a
channel/transponder is tuned.
(该度量随前端获得更多比特计数而单调递增。调谐到新的频道/转发器时，前端可能会将其复位。)

Possible scales for this metric are:
(该度量可能的刻度有：)

-  ``FE_SCALE_NOT_AVAILABLE`` - it failed to measure it, or the
   measurement was not complete yet.
   (``FE_SCALE_NOT_AVAILABLE`` - 测量失败，或测量尚未完成。)

-  ``FE_SCALE_COUNTER`` - Number of error blocks counted after the outer
   coding.
   (``FE_SCALE_COUNTER`` - 在外层编码之后统计的错误块数。)


.. _DTV-STAT-TOTAL-BLOCK-COUNT:

DTV-STAT_TOTAL_BLOCK_COUNT
==========================

Measures the total number of blocks received during the same period as
:ref:`DTV_STAT_ERROR_BLOCK_COUNT <DTV-STAT-ERROR-BLOCK-COUNT>`
measurement was taken.
(度量接收到的块总数，采集周期与
:ref:`DTV_STAT_ERROR_BLOCK_COUNT <DTV-STAT-ERROR-BLOCK-COUNT>`
度量相同。)

It can be used to calculate the PER indicator, by dividing
:ref:`DTV_STAT_ERROR_BLOCK_COUNT <DTV-STAT-ERROR-BLOCK-COUNT>` by
:ref:`DTV-STAT-TOTAL-BLOCK-COUNT`.
(可用它计算 PER 指标：把
:ref:`DTV_STAT_ERROR_BLOCK_COUNT <DTV-STAT-ERROR-BLOCK-COUNT>` 除以
:ref:`DTV-STAT-TOTAL-BLOCK-COUNT`。)

Possible scales for this metric are:
(该度量可能的刻度有：)

-  ``FE_SCALE_NOT_AVAILABLE`` - it failed to measure it, or the
   measurement was not complete yet.
   (``FE_SCALE_NOT_AVAILABLE`` - 测量失败，或测量尚未完成。)

-  ``FE_SCALE_COUNTER`` - Number of blocks counted while measuring
   :ref:`DTV_STAT_ERROR_BLOCK_COUNT <DTV-STAT-ERROR-BLOCK-COUNT>`.
   (``FE_SCALE_COUNTER`` - 测量
   :ref:`DTV_STAT_ERROR_BLOCK_COUNT <DTV-STAT-ERROR-BLOCK-COUNT>`
   时统计的块数。)
