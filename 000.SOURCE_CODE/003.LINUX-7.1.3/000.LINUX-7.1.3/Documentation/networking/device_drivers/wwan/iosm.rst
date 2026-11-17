.. SPDX-License-Identifier: GPL-2.0-only

.. Copyright (C) 2020-21 Intel Corporation

.. _iosm_driver_doc:

===========================================
IOSM Driver for Intel M.2 PCIe based Modems
===========================================

(面向基于 Intel M.2 PCIe 的调制解调器的 IOSM 驱动)

The IOSM (IPC over Shared Memory) driver is a WWAN PCIe host driver developed
for linux or chrome platform for data exchange over PCIe interface between
Host platform & Intel M.2 Modem. The driver exposes interface conforming to the
MBIM protocol [1]. Any front end application ( eg: Modem Manager) could easily
manage the MBIM interface to enable data communication towards WWAN.
(IOSM (IPC over Shared Memory，基于共享内存的 IPC) 驱动是一个为 Linux 或
Chrome 平台开发的 WWAN PCIe 主机驱动，用于在主机平台与 Intel M.2 调制解调器
之间通过 PCIe 接口进行数据交换。该驱动暴露符合 MBIM 协议 [1] 的接口。任何
前端应用 (例如 Modem Manager) 都可以轻松地管理 MBIM 接口，以启用面向 WWAN
的数据通信。)

Basic usage
(基本用法)
===========

MBIM functions are inactive when unmanaged. The IOSM driver only provides a
userspace interface MBIM "WWAN PORT" representing MBIM control channel and does
not play any role in managing the functionality. It is the job of a userspace
application to detect port enumeration and enable MBIM functionality.
(MBIM 功能在未被管理时处于非活动状态。IOSM 驱动只提供一个表示 MBIM 控制
信道的用户空间接口 MBIM "WWAN PORT"，在管理这些功能方面不起任何作用。
检测端口枚举并启用 MBIM 功能是用户空间应用的职责。)

Examples of few such userspace application are:
(此类用户空间应用的几个示例如下：)

- mbimcli (included with the libmbim [2] library), and
  (mbimcli (包含在 libmbim [2] 库中)，以及)
- Modem Manager [3]
  (Modem Manager [3])

Management Applications to carry out below required actions for establishing
MBIM IP session:
(管理应用需要执行以下必要操作来建立 MBIM IP 会话：)

- open the MBIM control channel
  (打开 MBIM 控制信道)
- configure network connection settings
  (配置网络连接设置)
- connect to network
  (连接到网络)
- configure IP network interface
  (配置 IP 网络接口)

Management application development
(管理应用开发)
==================================

The driver and userspace interfaces are described below. The MBIM protocol is
described in [1] Mobile Broadband Interface Model v1.0 Errata-1.
(驱动与用户空间接口在下面描述。MBIM 协议在 [1] Mobile Broadband Interface
Model v1.0 Errata-1 中描述。)

MBIM control channel userspace ABI
(MBIM 控制信道用户空间 ABI)
----------------------------------

/dev/wwan0mbim0 character device
(/dev/wwan0mbim0 字符设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The driver exposes an MBIM interface to the MBIM function by implementing
MBIM WWAN Port. The userspace end of the control channel pipe is a
/dev/wwan0mbim0 character device. Application shall use this interface for
MBIM protocol communication.
(驱动通过实现 MBIM WWAN Port 来向 MBIM 功能暴露 MBIM 接口。控制信道管道
的用户空间端是 /dev/wwan0mbim0 字符设备。应用应使用此接口进行 MBIM 协议
通信。)

Fragmentation
(分片)
~~~~~~~~~~~~~

The userspace application is responsible for all control message fragmentation
and defragmentation as per MBIM specification.
(用户空间应用负责按照 MBIM 规范完成所有控制消息的分片与重组。)

/dev/wwan0mbim0 write()
~~~~~~~~~~~~~~~~~~~~~~~
The MBIM control messages from the management application must not exceed the
negotiated control message size.
(来自管理应用的 MBIM 控制消息不得超过协商的控制消息大小。)

/dev/wwan0mbim0 read()
~~~~~~~~~~~~~~~~~~~~~~
The management application must accept control messages of up the negotiated
control message size.
(管理应用必须接受最大不超过协商控制消息大小的控制消息。)

MBIM data channel userspace ABI
(MBIM 数据信道用户空间 ABI)
-------------------------------

wwan0-X network device
(wwan0-X 网络设备)
~~~~~~~~~~~~~~~~~~~~~~
The IOSM driver exposes IP link interface "wwan0-X" of type "wwan" for IP
traffic. Iproute network utility is used for creating "wwan0-X" network
interface and for associating it with MBIM IP session. The Driver supports
up to 8 IP sessions for simultaneous IP communication.
(IOSM 驱动暴露类型为 "wwan" 的 IP 链路接口 "wwan0-X"，用于 IP 流量。
使用 Iproute 网络工具创建 "wwan0-X" 网络接口，并将其与 MBIM IP 会话
关联。驱动最多支持 8 个 IP 会话以进行并发的 IP 通信。)

The userspace management application is responsible for creating new IP link
prior to establishing MBIM IP session where the SessionId is greater than 0.
(在建立 SessionId 大于 0 的 MBIM IP 会话之前，用户空间管理应用负责创建
新的 IP 链路。)

For example, creating new IP link for a MBIM IP session with SessionId 1:
(例如，为 SessionId 为 1 的 MBIM IP 会话创建新 IP 链路：)

  ip link add dev wwan0-1 parentdev-name wwan0 type wwan linkid 1

The driver will automatically map the "wwan0-1" network device to MBIM IP
session 1.
(驱动会自动把 "wwan0-1" 网络设备映射到 MBIM IP 会话 1。)

References
(参考资料)
==========

[1] "MBIM (Mobile Broadband Interface Model) Errata-1"
      - https://www.usb.org/document-library/

[2] libmbim - "a glib-based library for talking to WWAN modems and
      devices which speak the Mobile Interface Broadband Model (MBIM)
      protocol"
      - http://www.freedesktop.org/wiki/Software/libmbim/
      (libmbim —— "一个基于 glib 的库，用于与 WWAN 调制解调器以及使用
      Mobile Interface Broadband Model (MBIM) 协议的设备通信")

[3] Modem Manager - "a DBus-activated daemon which controls mobile
      broadband (2G/3G/4G) devices and connections"
      - http://www.freedesktop.org/wiki/Software/ModemManager/
      (Modem Manager —— "一个由 DBus 激活的守护进程，用于控制移动宽带
      (2G/3G/4G) 设备与连接")
