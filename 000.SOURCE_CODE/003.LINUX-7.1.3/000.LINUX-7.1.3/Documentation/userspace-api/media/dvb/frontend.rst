.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _dvb_frontend:

#######################################
Digital TV Frontend API(数字电视前端 API)
#######################################

The Digital TV frontend API was designed to support three groups of delivery
systems: Terrestrial, cable and Satellite. Currently, the following
delivery systems are supported:
(数字电视前端 API 旨在支持三组传送系统：地面、有线和卫星。目前支持以下
传送系统：)

-  Terrestrial systems: DVB-T, DVB-T2, ATSC, ATSC M/H, ISDB-T, DVB-H,
   DTMB, CMMB
   (地面系统：DVB-T、DVB-T2、ATSC、ATSC M/H、ISDB-T、DVB-H、DTMB、CMMB)

-  Cable systems: DVB-C Annex A/C, ClearQAM (DVB-C Annex B)
   (有线系统：DVB-C Annex A/C、ClearQAM (DVB-C Annex B))

-  Satellite systems: DVB-S, DVB-S2, DVB Turbo, ISDB-S, DSS
   (卫星系统：DVB-S、DVB-S2、DVB Turbo、ISDB-S、DSS)

The Digital TV frontend controls several sub-devices including:
(数字电视前端控制若干子设备，包括：)

-  Tuner
   (调谐器)

-  Digital TV demodulator
   (数字电视解调器)

-  Low noise amplifier (LNA)
   (低噪声放大器 (LNA))

-  Satellite Equipment Control (SEC) [#f1]_.
   (卫星设备控制 (SEC) [#f1]_ 。)

The frontend can be accessed through ``/dev/dvb/adapter?/frontend?``.
Data types and ioctl definitions can be accessed by including
``linux/dvb/frontend.h`` in your application.
(前端可以通过 ``/dev/dvb/adapter?/frontend?`` 访问。在应用中包含
``linux/dvb/frontend.h`` 即可使用其中的数据类型和 ioctl 定义。)

.. note::

   Transmission via the internet (DVB-IP) and MMT (MPEG Media Transport)
   is not yet handled by this API but a future extension is possible.
   (通过互联网的传输 (DVB-IP) 和 MMT (MPEG Media Transport) 尚未由此 API
   处理，但将来可以扩展。)

.. [#f1]

   On Satellite systems, the API support for the Satellite Equipment
   Control (SEC) allows to power control and to send/receive signals to
   control the antenna subsystem, selecting the polarization and choosing
   the Intermediate Frequency IF) of the Low Noise Block Converter Feed
   Horn (LNBf). It supports the DiSEqC and V-SEC protocols. The DiSEqC
   (digital SEC) specification is available at
   `Eutelsat <http://www.eutelsat.com/satellites/4_5_5.html>`__.
   (在卫星系统上，API 对卫星设备控制 (SEC) 的支持允许进行电源控制以及
   发送/接收信号来控制天线子系统、选择极化方式，并为低噪声下变频器馈源
   (LNBf) 选择中频 (IF)。它支持 DiSEqC 和 V-SEC 协议。DiSEqC (数字 SEC)
   规范可从 `Eutelsat <http://www.eutelsat.com/satellites/4_5_5.html>`__
   获取。)


.. toctree::
    :maxdepth: 1

    query-dvb-frontend-info
    dvb-fe-read-status
    dvbproperty
    frontend_fcalls
