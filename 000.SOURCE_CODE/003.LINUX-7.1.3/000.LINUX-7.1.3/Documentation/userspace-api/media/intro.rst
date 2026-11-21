.. SPDX-License-Identifier: GPL-2.0

==================
Introduction(引言)
==================

This document covers the Linux Kernel to Userspace API's used by video
and radio streaming devices, including video cameras, analog and digital
TV receiver cards, AM/FM receiver cards, Software Defined Radio (SDR),
streaming capture and output devices, codec devices and remote controllers.
(本文档涵盖视频和广播流设备所使用的 Linux 内核到用户空间 API，包括视频摄像头、
模拟和数字电视接收卡、AM/FM 接收卡、软件定义无线电 (SDR)、流采集与输出设备、
编解码器设备以及遥控器。)

A typical media device hardware is shown at :ref:`typical_media_device`.
(典型媒体设备硬件如 :ref:`typical_media_device` 所示。)

.. _typical_media_device:

.. kernel-figure:: typical_media_device.svg
    :alt:   typical_media_device.svg
    :align: center

    Typical Media Device

The media infrastructure API was designed to control such devices. It is
divided into five parts.
(媒体基础架构 API 旨在控制此类设备。它分为五个部分。)

1. The :ref:`first part <v4l2spec>` covers radio, video capture and output,
   cameras, analog TV devices and codecs.
   (第一部分涵盖广播、视频采集与输出、摄像头、模拟电视设备和编解码器。)

2. The :ref:`second part <dvbapi>` covers the API used for digital TV and
   Internet reception via one of the several digital tv standards. While it is
   called as DVB API, in fact it covers several different video standards
   including DVB-T/T2, DVB-S/S2, DVB-C, ATSC, ISDB-T, ISDB-S, DTMB, etc. The
   complete list of supported standards can be found at
   :c:type:`fe_delivery_system`.
   (第二部分涵盖用于数字电视以及经由多种数字电视标准之一进行 Internet 接收的
   API。虽然称为 DVB API，但实际上它涵盖多种不同的视频标准，包括 DVB-T/T2、
   DVB-S/S2、DVB-C、ATSC、ISDB-T、ISDB-S、DTMB 等。所支持标准的完整列表见
   :c:type:`fe_delivery_system`。)

3. The :ref:`third part <remote_controllers>` covers the Remote Controller API.
   (第三部分涵盖遥控器 API。)

4. The :ref:`fourth part <media_controller>` covers the Media Controller API.
   (第四部分涵盖媒体控制器 API。)

5. The :ref:`fifth part <cec>` covers the CEC (Consumer Electronics Control) API.
   (第五部分涵盖 CEC（消费电子控制）API。)

It should also be noted that a media device may also have audio components, like
mixers, PCM capture, PCM playback, etc, which are controlled via ALSA API.  For
additional information and for the latest development code, see:
`https://linuxtv.org <https://linuxtv.org>`__.  For discussing improvements,
reporting troubles, sending new drivers, etc, please mail to: `Linux Media
Mailing List (LMML) <http://vger.kernel.org/vger-lists.html#linux-media>`__.
(还需注意，媒体设备也可能带有音频组件，如混音器、PCM 采集、PCM 回放等，它们
通过 ALSA API 控制。更多信息及最新开发代码见：
`https://linuxtv.org <https://linuxtv.org>`__。要讨论改进、报告问题、提交新
驱动等，请发邮件至：`Linux Media Mailing List (LMML)
<http://vger.kernel.org/vger-lists.html#linux-media>`__。)
