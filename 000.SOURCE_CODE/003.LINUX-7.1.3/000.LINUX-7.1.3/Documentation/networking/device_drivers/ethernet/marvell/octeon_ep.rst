.. SPDX-License-Identifier: GPL-2.0+

====================================================================
Linux kernel networking driver for Marvell's Octeon PCI Endpoint NIC
====================================================================

(Marvell Octeon PCI Endpoint 网卡的 Linux 内核网络驱动)

Network driver for Marvell's Octeon PCI EndPoint NIC.
(Marvell Octeon PCI EndPoint 网卡的网络驱动。)
Copyright (c) 2020 Marvell International Ltd.

Contents
========
(目录)

- `Overview`_
  (概述)
- `Supported Devices`_
  (支持的设备)
- `Interface Control`_
  (接口控制)

Overview
========
(概述)

This driver implements networking functionality of Marvell's Octeon PCI
EndPoint NIC.
(该驱动实现 Marvell Octeon PCI EndPoint 网卡的网络功能。)

Supported Devices
=================
(支持的设备)

Currently, this driver support following devices:
(目前，该驱动支持以下设备：)
 * Network controller: Cavium, Inc. Device b100
 * Network controller: Cavium, Inc. Device b200
 * Network controller: Cavium, Inc. Device b400
 * Network controller: Cavium, Inc. Device b900
 * Network controller: Cavium, Inc. Device ba00
 * Network controller: Cavium, Inc. Device bc00
 * Network controller: Cavium, Inc. Device bd00

Interface Control
=================
(接口控制)

Network Interface control like changing mtu, link speed, link down/up are
done by writing command to mailbox command queue, a mailbox interface
implemented through a reserved region in BAR4.
This driver writes the commands into the mailbox and the firmware on the
Octeon device processes them. The firmware also sends unsolicited notifications
to driver for events suchs as link change, through notification queue
implemented as part of mailbox interface.
(诸如更改 MTU、链路速率、链路 down/up 之类的网络接口控制操作，都是通过向邮箱 (mailbox) 命令队列写入命令来完成的；邮箱接口通过 BAR4 中的一个保留区域实现。该驱动把命令写入邮箱，Octeon 设备上的固件对它们进行处理。固件还会通过作为邮箱接口一部分的通知队列，向驱动发送诸如链路变化等事件的主动通知。)
