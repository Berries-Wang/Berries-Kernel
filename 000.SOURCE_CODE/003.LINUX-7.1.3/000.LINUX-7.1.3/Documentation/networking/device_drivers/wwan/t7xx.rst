.. SPDX-License-Identifier: GPL-2.0-only

.. Copyright (C) 2020-21 Intel Corporation

.. _t7xx_driver_doc:

=================================================================================================
t7xx driver for MTK PCIe based T700 5G modem(用于基于 MTK PCIe 的 T700 5G 调制解调器的 t7xx 驱动)
=================================================================================================

The t7xx driver is a WWAN PCIe host driver developed for linux or Chrome OS
platforms for data exchange over PCIe interface between Host platform &
MediaTek's T700 5G modem.
(t7xx 驱动是为 Linux 或 Chrome OS 平台开发的 WWAN PCIe 主机驱动，用于在主机平台与联发科 T700 5G 调制解
调器之间通过 PCIe 接口进行数据交换。)
The driver exposes an interface conforming to the MBIM protocol [1]. Any front
end application (e.g. Modem Manager) could easily manage the MBIM interface to
enable data communication towards WWAN. The driver also provides an interface
to interact with the MediaTek's modem via AT commands.
(该驱动暴露一个符合 MBIM 协议 [1] 的接口。任何前端应用（例如 Modem Manager）都可以轻松管理该 MBIM 接口，
以实现面向 WWAN 的数据通信。该驱动还提供了一个通过 AT 命令与联发科调制解调器交互的接口。)

Basic usage(基本用法)
=====================

MBIM & AT functions are inactive when unmanaged. The t7xx driver provides
WWAN port userspace interfaces representing MBIM & AT control channels and does
not play any role in managing their functionality. It is the job of a userspace
application to detect port enumeration and enable MBIM & AT functionalities.
(在无人管理时，MBIM 与 AT 功能处于非活动状态。t7xx 驱动提供代表 MBIM 与 AT 控制信道的 WWAN 端口用户空间接
口，但并不参与管理它们的功能。检测端口枚举并启用 MBIM 与 AT 功能是用户空间应用的职责。)

Examples of few such userspace applications are:
(此类用户空间应用的一些示例：)

- mbimcli (included with the libmbim [2] library), and
  (mbimcli（包含在 libmbim [2] 库中），以及)
- Modem Manager [3]
  (Modem Manager [3])

Management Applications to carry out below required actions for establishing
MBIM IP session:
(管理应用需执行以下必要操作来建立 MBIM IP 会话：)

- open the MBIM control channel
  (打开 MBIM 控制信道)
- configure network connection settings
  (配置网络连接设置)
- connect to network
  (连接到网络)
- configure IP network interface
  (配置 IP 网络接口)

Management Applications to carry out below required actions for send an AT
command and receive response:
(管理应用需执行以下必要操作来发送 AT 命令并接收响应：)

- open the AT control channel using a UART tool or a special user tool
  (使用 UART 工具或专用用户工具打开 AT 控制信道)

Sysfs(sysfs)
============

The driver provides sysfs interfaces to userspace.
(该驱动向用户空间提供 sysfs 接口。)

t7xx_mode(t7xx_mode)
--------------------
The sysfs interface provides userspace with access to the device mode, this
interface supports read and write operations.
(该 sysfs 接口为用户空间提供对设备模式的访问，支持读和写操作。)

Device mode:
(设备模式：)

- ``unknown`` represents that device in unknown status
  (``unknown`` 表示设备处于未知状态)
- ``ready`` represents that device in ready status
  (``ready`` 表示设备处于就绪状态)
- ``reset`` represents that device in reset status
  (``reset`` 表示设备处于复位状态)
- ``fastboot_switching`` represents that device in fastboot switching status
  (``fastboot_switching`` 表示设备处于 fastboot 切换状态)
- ``fastboot_download`` represents that device in fastboot download status
  (``fastboot_download`` 表示设备处于 fastboot 下载状态)
- ``fastboot_dump`` represents that device in fastboot dump status
  (``fastboot_dump`` 表示设备处于 fastboot 转储状态)

Read from userspace to get the current device mode.
(从用户空间读取以获取当前设备模式。)

::
  $ cat /sys/bus/pci/devices/${bdf}/t7xx_mode

Write from userspace to set the device mode.
(从用户空间写入以设置设备模式。)

::
  $ echo fastboot_switching > /sys/bus/pci/devices/${bdf}/t7xx_mode

t7xx_debug_ports(t7xx_debug_ports)
----------------------------------
The sysfs interface provides userspace with access to enable/disable the debug
ports, this interface supports read and write operations.
(该 sysfs 接口为用户空间提供启用/禁用调试端口的访问，支持读和写操作。)

Debug port status:
(调试端口状态：)

- ``1`` represents enable debug ports
  (``1`` 表示启用调试端口)
- ``0`` represents disable debug ports
  (``0`` 表示禁用调试端口)

Currently supported debug ports (ADB/MIPC).
(当前支持的调试端口（ADB/MIPC）。)

Read from userspace to get the current debug ports status.
(从用户空间读取以获取当前调试端口状态。)

::
  $ cat /sys/bus/pci/devices/${bdf}/t7xx_debug_ports

Write from userspace to set the debug ports status.
(从用户空间写入以设置调试端口状态。)

::
  $ echo 1 > /sys/bus/pci/devices/${bdf}/t7xx_debug_ports

Management application development(管理应用开发)
================================================

The driver and userspace interfaces are described below. The MBIM protocol is
described in [1] Mobile Broadband Interface Model v1.0 Errata-1.
(下文描述驱动与用户空间接口。MBIM 协议在 [1] Mobile Broadband Interface Model v1.0 Errata-1 中描述。)

MBIM control channel userspace ABI(MBIM 控制信道用户空间 ABI)
-------------------------------------------------------------

/dev/wwan0mbim0 character device(/dev/wwan0mbim0 字符设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The driver exposes an MBIM interface to the MBIM function by implementing
MBIM WWAN Port. The userspace end of the control channel pipe is a
/dev/wwan0mbim0 character device. Application shall use this interface for
MBIM protocol communication.
(该驱动通过实现 MBIM WWAN Port，向 MBIM 功能暴露一个 MBIM 接口。控制信道管道的用户空间端是 /dev/wwan0mbim0
字符设备。应用应使用此接口进行 MBIM 协议通信。)

Fragmentation(分片)
~~~~~~~~~~~~~~~~~~~
The userspace application is responsible for all control message fragmentation
and defragmentation as per MBIM specification.
(用户空间应用负责按照 MBIM 规范完成所有控制消息的分片与重组。)

/dev/wwan0mbim0 write()(/dev/wwan0mbim0 write())
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The MBIM control messages from the management application must not exceed the
negotiated control message size.
(来自管理应用的 MBIM 控制消息不得超过协商的控制消息大小。)

/dev/wwan0mbim0 read()(/dev/wwan0mbim0 read())
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The management application must accept control messages of up the negotiated
control message size.
(管理应用必须能够接受最大至协商控制消息大小的控制消息。)

MBIM data channel userspace ABI(MBIM 数据信道用户空间 ABI)
----------------------------------------------------------

wwan0-X network device(wwan0-X 网络设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The t7xx driver exposes IP link interface "wwan0-X" of type "wwan" for IP
traffic. Iproute network utility is used for creating "wwan0-X" network
interface and for associating it with MBIM IP session.
(t7xx 驱动暴露类型为 "wwan" 的 IP 链路接口 "wwan0-X" 用于 IP 流量。iproute 网络工具用于创建 "wwan0-X" 网
络接口，并将其与 MBIM IP 会话关联。)

The userspace management application is responsible for creating new IP link
prior to establishing MBIM IP session where the SessionId is greater than 0.
(用户空间管理应用负责在建立 SessionId 大于 0 的 MBIM IP 会话之前创建新的 IP 链路。)

For example, creating new IP link for a MBIM IP session with SessionId 1:
(例如，为 SessionId 为 1 的 MBIM IP 会话创建新的 IP 链路：)

  ip link add dev wwan0-1 parentdev wwan0 type wwan linkid 1

The driver will automatically map the "wwan0-1" network device to MBIM IP
session 1.
(驱动会自动将 "wwan0-1" 网络设备映射到 MBIM IP 会话 1。)

AT port userspace ABI(AT 端口用户空间 ABI)
------------------------------------------

/dev/wwan0at0 character device(/dev/wwan0at0 字符设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The driver exposes an AT port by implementing AT WWAN Port.
The userspace end of the control port is a /dev/wwan0at0 character
device. Application shall use this interface to issue AT commands.
(该驱动通过实现 AT WWAN Port 暴露一个 AT 端口。控制端口的用户空间端是 /dev/wwan0at0 字符设备。应用应使用
此接口下发 AT 命令。)

fastboot port userspace ABI(fastboot 端口用户空间 ABI)
------------------------------------------------------

/dev/wwan0fastboot0 character device(/dev/wwan0fastboot0 字符设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The driver exposes a fastboot protocol interface by implementing
fastboot WWAN Port. The userspace end of the fastboot channel pipe is a
/dev/wwan0fastboot0 character device. Application shall use this interface for
fastboot protocol communication.
(该驱动通过实现 fastboot WWAN Port 暴露一个 fastboot 协议接口。fastboot 信道管道的用户空间端是
/dev/wwan0fastboot0 字符设备。应用应使用此接口进行 fastboot 协议通信。)

Please note that driver needs to be reloaded to export /dev/wwan0fastboot0
port, because device needs a cold reset after enter ``fastboot_switching``
mode.
(请注意，需要重新加载驱动才能导出 /dev/wwan0fastboot0 端口，因为设备在进入 ``fastboot_switching`` 模式后需
要一次冷复位。)

ADB port userspace ABI(ADB 端口用户空间 ABI)
--------------------------------------------

/dev/wwan0adb0 character device(/dev/wwan0adb0 字符设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The driver exposes a ADB protocol interface by implementing ADB WWAN Port.
The userspace end of the ADB channel pipe is a /dev/wwan0adb0 character device.
Application shall use this interface for ADB protocol communication.
(该驱动通过实现 ADB WWAN Port 暴露一个 ADB 协议接口。ADB 信道管道的用户空间端是 /dev/wwan0adb0 字符设备。
应用应使用此接口进行 ADB 协议通信。)

MIPC port userspace ABI(MIPC 端口用户空间 ABI)
----------------------------------------------

/dev/wwan0mipc0 character device(/dev/wwan0mipc0 字符设备)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The driver exposes a diagnostic interface by implementing MIPC (Modem
Information Process Center) WWAN Port. The userspace end of the MIPC channel
pipe is a /dev/wwan0mipc0 character device.
Application shall use this interface for MTK modem diagnostic communication.
(该驱动通过实现 MIPC（调制解调器信息处理中心）WWAN Port 暴露一个诊断接口。MIPC 信道管道的用户空间端是
/dev/wwan0mipc0 字符设备。应用应使用此接口进行 MTK 调制解调器诊断通信。)

The MediaTek's T700 modem supports the 3GPP TS 27.007 [4] specification.
(联发科 T700 调制解调器支持 3GPP TS 27.007 [4] 规范。)

References(参考资料)
====================

[1] *MBIM (Mobile Broadband Interface Model) Errata-1*
([1] MBIM（移动宽带接口模型）勘误-1)

- https://www.usb.org/document-library/

[2] *libmbim "a glib-based library for talking to WWAN modems and devices which
speak the Mobile Interface Broadband Model (MBIM) protocol"*
([2] libmbim "一个基于 glib 的库，用于与支持移动接口宽带模型 (MBIM) 协议的 WWAN 调制解调器和设备通信")

- http://www.freedesktop.org/wiki/Software/libmbim/

[3] *Modem Manager "a DBus-activated daemon which controls mobile broadband
(2G/3G/4G/5G) devices and connections"*
([3] Modem Manager "一个由 DBus 激活的守护进程，用于控制移动宽带 (2G/3G/4G/5G) 设备和连接")

- http://www.freedesktop.org/wiki/Software/ModemManager/

[4] *Specification # 27.007 - 3GPP*
([4] 规范 # 27.007 - 3GPP)

- https://www.3gpp.org/DynaReport/27007.htm

[5] *fastboot "a mechanism for communicating with bootloaders"*
([5] fastboot "一种与引导加载程序通信的机制")

- https://android.googlesource.com/platform/system/core/+/refs/heads/main/fastboot/README.md

[6] *ADB (Android Debug Bridge) "a mechanism to keep track of Android devices
and emulators instances connected to or running on a given host developer
machine with ADB protocol"*
([6] ADB（Android 调试桥）"一种通过 ADB 协议跟踪连接到给定主机开发机或在
其上运行的 Android 设备与模拟器实例的机制"

- https://android.googlesource.com/platform/packages/modules/adb/+/refs/heads/main/README.md
