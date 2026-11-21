.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _dvb_introduction:

******************
Introduction(引言)
******************


.. _requisites:

What you need to know(你需要了解的知识)
=======================================

The reader of this document is required to have some knowledge in the
area of digital video broadcasting (Digital TV) and should be familiar with
part I of the MPEG2 specification ISO/IEC 13818 (aka ITU-T H.222), i.e
you should know what a program/transport stream (PS/TS) is and what is
meant by a packetized elementary stream (PES) or an I-frame.
(本文要求读者具备数字视频广播 (Digital TV) 领域的一些知识，并熟悉 MPEG2 规范 ISO/IEC 13818(又称 ITU-T H.222)的第一部分，即你应该知道什么是节目流/传输流 (PS/TS)，以及打包基本流 (PES) 和 I 帧 (I-frame) 的含义。)

Various Digital TV standards documents are available for download at:
(各类数字电视标准文档可从以下地址下载：)

- European standards (DVB): http://www.dvb.org and/or http://www.etsi.org.
- American standards (ATSC): https://www.atsc.org/standards/
- Japanese standards (ISDB): http://www.dibeg.org/

It is also necessary to know how to access Linux devices and how to
use ioctl calls. This also includes the knowledge of C or C++.
(还需要知道如何访问 Linux 设备以及如何使用 ioctl 调用。这也包括掌握 C 或 C++ 语言。)


.. _history:

History(历史)
=============

The first API for Digital TV cards we used at Convergence in late 1999 was an
extension of the Video4Linux API which was primarily developed for frame
grabber cards. As such it was not really well suited to be used for Digital
TV cards and their new features like recording MPEG streams and filtering
several section and PES data streams at the same time.
(1999 年底我们在 Convergence 使用的第一版数字电视卡 API 是 Video4Linux API 的扩展，而后者主要是为帧采集卡开发的。因此它并不真正适合数字电视卡及其新特性，例如录制 MPEG 流、同时过滤多个 section 与 PES 数据流。)

In early 2000, Convergence was approached by Nokia with a proposal for a new
standard Linux Digital TV API. As a commitment to the development of terminals
based on open standards, Nokia and Convergence made it available to all
Linux developers and published it on https://linuxtv.org in September
2000. With the Linux driver for the Siemens/Hauppauge DVB PCI card,
Convergence provided a first implementation of the Linux Digital TV API.
Convergence was the maintainer of the Linux Digital TV API in the early
days.
(2000 年初，Nokia 向 Convergence 提出了一个新标准 Linux Digital TV API 的提案。作为对基于开放标准的终端开发的承诺，Nokia 与 Convergence 把它开放给所有 Linux 开发者，并于 2000 年 9 月发布在 https://linuxtv.org 上。随附 Siemens/Hauppauge DVB PCI 卡的 Linux 驱动，Convergence 提供了 Linux Digital TV API 的第一个实现。早期由 Convergence 担任 Linux Digital TV API 的维护者。)

Now, the API is maintained by the LinuxTV community (i.e. you, the reader
of this document). The Linux  Digital TV API is constantly reviewed and
improved together with the improvements at the subsystem's core at the
Kernel.
(如今，该 API 由 LinuxTV 社区维护(也就是你，本文的读者)。Linux Digital TV API 持续接受审查与改进，并与内核中子系统核心的改进同步推进。)


.. _overview:

Overview(概览)
==============


.. _stb_components:

.. kernel-figure:: dvbstb.svg
    :alt:   dvbstb.svg
    :align: center

    Components of a Digital TV card/STB

A Digital TV card or set-top-box (STB) usually consists of the
following main hardware components:
(数字电视卡或机顶盒 (STB) 通常由以下主要硬件组件构成：)

Frontend consisting of tuner and digital TV demodulator
(前端，由调谐器与数字电视解调器组成)

   Here the raw signal reaches the digital TV hardware from a satellite dish or
   antenna or directly from cable. The frontend down-converts and
   demodulates this signal into an MPEG transport stream (TS). In case
   of a satellite frontend, this includes a facility for satellite
   equipment control (SEC), which allows control of LNB polarization,
   multi feed switches or dish rotors.
   (原始信号在此处从卫星锅面、天线或直接从电缆到达数字电视硬件。前端把该信号下变频并解调为 MPEG 传输流 (TS)。对于卫星前端，还包括卫星设备控制 (SEC) 设施，可用于控制 LNB 极化、多馈源切换器或天线旋转器。)

Conditional Access (CA) hardware like CI adapters and smartcard slots
(条件接收 (CA) 硬件，如 CI 适配器与智能卡插槽)

   The complete TS is passed through the CA hardware. Programs to which
   the user has access (controlled by the smart card) are decoded in
   real time and re-inserted into the TS.
   (完整的 TS 会经过 CA 硬件。用户有权访问的节目(由智能卡控制)被实时解码并重新插入 TS。)

   .. note::

      Not every digital TV hardware provides conditional access hardware.
      (并非每种数字电视硬件都提供条件接收硬件。)

Demultiplexer which filters the incoming Digital TV MPEG-TS stream
(对输入的数字电视 MPEG-TS 流进行过滤的解复用器)

   The demultiplexer splits the TS into its components like audio and
   video streams. Besides usually several of such audio and video
   streams it also contains data streams with information about the
   programs offered in this or other streams of the same provider.
   (解复用器把 TS 拆分成其组成成分，例如音频与视频流。除通常若干路这样的音视频流之外，其中还包含一些数据流，载有关于此流或同一提供商其他流中所提供节目的信息。)

Audio and video decoder
(音频与视频解码器)

   The main targets of the demultiplexer are audio and video
   decoders. After decoding, they pass on the uncompressed audio and
   video to the computer screen or to a TV set.
   (解复用器的主要输出对象是音频与视频解码器。解码之后，它们把未压缩的音频与视频送往计算机屏幕或电视机。)

   .. note::

      Modern hardware usually doesn't have a separate decoder hardware, as
      such functionality can be provided by the main CPU, by the graphics
      adapter of the system or by a signal processing hardware embedded on
      a Systems on a Chip (SoC) integrated circuit.
      (现代硬件通常没有独立的解码器硬件，因为这类功能可以由主 CPU、系统的图形适配器或片上系统 (SoC) 集成电路内嵌的信号处理硬件提供。)

      It may also not be needed for certain usages (e.g. for data-only
      uses like "internet over satellite").
      (某些用途也可能不需要解码器(例如"经卫星上网"这类纯数据用途)。)

:ref:`stb_components` shows a crude schematic of the control and data
flow between those components.
(:ref:`stb_components` 给出了这些组件之间控制流与数据流的粗略示意图。)



.. _dvb_devices:

Linux Digital TV Devices(Linux Digital TV 设备)
===============================================

The Linux Digital TV API lets you control these hardware components through
currently six Unix-style character devices for video, audio, frontend,
demux, CA and IP-over-DVB networking. The video and audio devices
control the MPEG2 decoder hardware, the frontend device the tuner and
the Digital TV demodulator. The demux device gives you control over the PES
and section filters of the hardware. If the hardware does not support
filtering, these filters can be implemented in software. Finally, the CA
device controls all the conditional access capabilities of the hardware.
It can depend on the individual security requirements of the platform,
if and how many of the CA functions are made available to the
application through this device.
(Linux Digital TV API 让你通过目前六类 Unix 风格的字符设备来控制这些硬件组件，分别用于 video、audio、frontend、demux、CA 与 IP-over-DVB 网络。video 与 audio 设备控制 MPEG2 解码器硬件，frontend 设备控制调谐器与数字电视解调器。demux 设备让你掌控硬件的 PES 与 section 过滤器。如果硬件不支持过滤，这些过滤器可以用软件实现。最后，CA 设备控制硬件的全部条件接收能力。平台各自的安全需求决定了是否以及有多少 CA 功能通过该设备开放给应用。)

All devices can be found in the ``/dev`` tree under ``/dev/dvb``. The
individual devices are called:
(所有设备都位于 ``/dev`` 树的 ``/dev/dvb`` 之下。各设备名称为：)

-  ``/dev/dvb/adapterN/audioM``,

-  ``/dev/dvb/adapterN/videoM``,

-  ``/dev/dvb/adapterN/frontendM``,

-  ``/dev/dvb/adapterN/netM``,

-  ``/dev/dvb/adapterN/demuxM``,

-  ``/dev/dvb/adapterN/dvrM``,

-  ``/dev/dvb/adapterN/caM``,

where ``N`` enumerates the Digital TV cards in a system starting from 0, and
``M`` enumerates the devices of each type within each adapter, starting
from 0, too. We will omit the "``/dev/dvb/adapterN/``\ " in the further
discussion of these devices.
(其中 ``N`` 从 0 开始枚举系统中的数字电视卡，``M`` 同样从 0 开始枚举每个适配器内各类型的设备。在后文讨论这些设备时，我们将省略 "``/dev/dvb/adapterN/``\ " 前缀。)

More details about the data structures and function calls of all the
devices are described in the following chapters.
(所有设备的数据结构与函数调用将在后续章节中详细描述。)


.. _include_files:

API include files(API 头文件)
=============================

For each of the Digital TV devices a corresponding include file exists. The
Digital TV API include files should be included in application sources with a
partial path like:
(每种数字电视设备都有对应的头文件。应用源码中应按如下部分路径包含这些 Digital TV API 头文件：)


.. code-block:: c

	#include <linux/dvb/ca.h>

	#include <linux/dvb/dmx.h>

	#include <linux/dvb/frontend.h>

	#include <linux/dvb/net.h>


To enable applications to support different API version, an additional
include file ``linux/dvb/version.h`` exists, which defines the constant
``DVB_API_VERSION``. This document describes ``DVB_API_VERSION 5.10``.
(为了让应用能够支持不同的 API 版本，还存在一个额外的头文件 ``linux/dvb/version.h``，它定义了常量 ``DVB_API_VERSION``。本文档描述的是 ``DVB_API_VERSION 5.10``。)
