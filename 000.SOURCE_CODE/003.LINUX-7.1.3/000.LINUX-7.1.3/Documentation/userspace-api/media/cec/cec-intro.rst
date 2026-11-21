.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _cec-intro:

Introduction(引言)
==================

HDMI connectors provide a single pin for use by the Consumer Electronics
Control protocol. This protocol allows different devices connected by an
HDMI cable to communicate. The protocol for CEC version 1.4 is defined
in supplements 1 (CEC) and 2 (HEAC or HDMI Ethernet and Audio Return
Channel) of the HDMI 1.4a (:ref:`hdmi`) specification and the
extensions added to CEC version 2.0 are defined in chapter 11 of the
HDMI 2.0 (:ref:`hdmi2`) specification.
(HDMI 连接器为消费电子控制（CEC）协议提供单个引脚。该协议允许通过 HDMI 电缆
连接的不同设备相互通信。CEC 1.4 版协议定义在 HDMI 1.4a (:ref:`hdmi`) 规范的
补充文件 1（CEC）和 2（HEAC，即 HDMI 以太网和音频回传信道）中，CEC 2.0 版
新增的扩展则定义在 HDMI 2.0 (:ref:`hdmi2`) 规范的第 11 章中。)

The bitrate is very slow (effectively no more than 36 bytes per second)
and is based on the ancient AV.link protocol used in old SCART
connectors. The protocol closely resembles a crazy Rube Goldberg
contraption and is an unholy mix of low and high level messages. Some
messages, especially those part of the HEAC protocol layered on top of
CEC, need to be handled by the kernel, others can be handled either by
the kernel or by userspace.
(其比特率非常慢（实际上每秒不超过 36 字节），并基于旧式 SCART 连接器所用的
古老 AV.link 协议。该协议酷似一台疯狂的"鲁布·戈德堡机械"，是底层与高层
消息的离奇混合。有些消息——尤其是叠加在 CEC 之上的 HEAC 协议的一部分——
必须由内核处理，另一些则可由内核或用户空间处理。)

In addition, CEC can be implemented in HDMI receivers, transmitters and
in USB devices that have an HDMI input and an HDMI output and that
control just the CEC pin.
(此外，CEC 可以实现于 HDMI 接收器、发送器，以及带有 HDMI 输入和 HDMI 输出、
且仅控制 CEC 引脚的 USB 设备之中。)

Drivers that support CEC will create a CEC device node (/dev/cecX) to
give userspace access to the CEC adapter. The
:ref:`CEC_ADAP_G_CAPS` ioctl will tell userspace what it is allowed to do.
(支持 CEC 的驱动会创建一个 CEC 设备节点 (/dev/cecX)，让用户空间可以访问
CEC 适配器。:ref:`CEC_ADAP_G_CAPS` ioctl 会告诉用户空间它被允许做什么。)

In order to check the support and test it, it is suggested to download
the `v4l-utils <https://git.linuxtv.org/v4l-utils.git/>`_ package. It
provides three tools to handle CEC:
(为检查并测试其支持情况，建议下载
`v4l-utils <https://git.linuxtv.org/v4l-utils.git/>`_ 软件包。它提供三个
处理 CEC 的工具：)

- cec-ctl: the Swiss army knife of CEC. Allows you to configure, transmit
  and monitor CEC messages.
  (cec-ctl：CEC 的瑞士军刀。允许配置、发送和监听 CEC 消息。)

- cec-compliance: does a CEC compliance test of a remote CEC device to
  determine how compliant the CEC implementation is.
  (cec-compliance：对远程 CEC 设备做一致性测试，以判断 CEC 实现的符合程度。)

- cec-follower: emulates a CEC follower.
  (cec-follower：模拟一个 CEC 跟随者。)
