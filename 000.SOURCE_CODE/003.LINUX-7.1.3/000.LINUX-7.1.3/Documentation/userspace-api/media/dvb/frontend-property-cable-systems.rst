.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _frontend-property-cable-systems:

*****************************************************************
Properties used on cable delivery systems(用于有线传送系统的属性)
*****************************************************************


.. _dvbc-params:

DVB-C delivery system(DVB-C 传送系统)
=====================================

The DVB-C Annex-A is the widely used cable standard. Transmission uses
QAM modulation.
(DVB-C Annex-A 是广泛使用的有线标准。传输使用 QAM 调制。)

The DVB-C Annex-C is optimized for 6MHz, and is used in Japan. It
supports a subset of the Annex A modulation types, and a roll-off of
0.13, instead of 0.15
(DVB-C Annex-C 针对 6MHz 优化，用于日本。它支持 Annex A 调制类型的一
个子集，滚降系数 (roll-off) 为 0.13 而非 0.15。)

The following parameters are valid for DVB-C Annex A/C:
(以下参数对 DVB-C Annex A/C 有效：)

-  :ref:`DTV_API_VERSION <DTV-API-VERSION>`

-  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`

-  :ref:`DTV_TUNE <DTV-TUNE>`

-  :ref:`DTV_CLEAR <DTV-CLEAR>`

-  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>`

-  :ref:`DTV_MODULATION <DTV-MODULATION>`

-  :ref:`DTV_INVERSION <DTV-INVERSION>`

-  :ref:`DTV_SYMBOL_RATE <DTV-SYMBOL-RATE>`

-  :ref:`DTV_INNER_FEC <DTV-INNER-FEC>`

-  :ref:`DTV_LNA <DTV-LNA>`

In addition, the :ref:`DTV QoS statistics <frontend-stat-properties>`
are also valid.
(此外， :ref:`DTV QoS statistics <frontend-stat-properties>` 也是有效
的。)


.. _dvbc-annex-b-params:

DVB-C Annex B delivery system(DVB-C Annex B 传送系统)
=====================================================

The DVB-C Annex-B is only used on a few Countries like the United
States.
(DVB-C Annex-B 只在少数几个国家使用，例如美国。)

The following parameters are valid for DVB-C Annex B:
(以下参数对 DVB-C Annex B 有效：)

-  :ref:`DTV_API_VERSION <DTV-API-VERSION>`

-  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>`

-  :ref:`DTV_TUNE <DTV-TUNE>`

-  :ref:`DTV_CLEAR <DTV-CLEAR>`

-  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>`

-  :ref:`DTV_MODULATION <DTV-MODULATION>`

-  :ref:`DTV_INVERSION <DTV-INVERSION>`

-  :ref:`DTV_LNA <DTV-LNA>`

In addition, the :ref:`DTV QoS statistics <frontend-stat-properties>`
are also valid.
(此外， :ref:`DTV QoS statistics <frontend-stat-properties>` 也是有效
的。)
