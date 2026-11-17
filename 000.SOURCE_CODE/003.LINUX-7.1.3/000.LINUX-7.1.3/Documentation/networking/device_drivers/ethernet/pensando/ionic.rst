.. SPDX-License-Identifier: GPL-2.0+

========================================================
Linux Driver for the Pensando(R) Ethernet adapter family
========================================================

(Pensando(R) 以太网适配器系列的 Linux 驱动程序)

Pensando Linux Ethernet driver.
Copyright(c) 2019 Pensando Systems, Inc
(Pensando Linux 以太网驱动程序。
版权所有(c) 2019 Pensando Systems, Inc)

Contents
(目录)
========

- Identifying the Adapter
  (识别适配器)
- Enabling the driver
  (启用驱动程序)
- Configuring the driver
  (配置驱动程序)
- RDMA Support via Auxiliary Device
  (通过辅助设备支持 RDMA)
- Statistics
  (统计信息)
- Support
  (支持)

Identifying the Adapter
(识别适配器)
=======================

To find if one or more Pensando PCI Ethernet devices are installed on the
host, check for the PCI devices::
(要查看主机上是否安装了一个或多个 Pensando PCI 以太网设备，请检查 PCI
设备：)

  $ lspci -d 1dd8:
  b5:00.0 Ethernet controller: Device 1dd8:1002
  b6:00.0 Ethernet controller: Device 1dd8:1002

If such devices are listed as above, then the ionic.ko driver should find
and configure them for use.  There should be log entries in the kernel
messages such as these::
(如果如上列出了这类设备，ionic.ko 驱动程序就应当能够找到并配置它们供
使用。内核消息中应该会有类似如下的日志条目：)

  $ dmesg | grep ionic
  ionic 0000:b5:00.0: 126.016 Gb/s available PCIe bandwidth (8.0 GT/s PCIe x16 link)
  ionic 0000:b5:00.0 enp181s0: renamed from eth0
  ionic 0000:b5:00.0 enp181s0: Link up - 100 Gbps
  ionic 0000:b6:00.0: 126.016 Gb/s available PCIe bandwidth (8.0 GT/s PCIe x16 link)
  ionic 0000:b6:00.0 enp182s0: renamed from eth0
  ionic 0000:b6:00.0 enp182s0: Link up - 100 Gbps

Driver and firmware version information can be gathered with either of
ethtool or devlink tools::
(驱动程序与固件版本信息可以通过 ethtool 或 devlink 工具获取：)

  $ ethtool -i enp181s0
  driver: ionic
  version: 5.7.0
  firmware-version: 1.8.0-28
  ...

  $ devlink dev info pci/0000:b5:00.0
  pci/0000:b5:00.0:
    driver ionic
    serial_number FLM18420073
    versions:
        fixed:
          asic.id 0x0
          asic.rev 0x0
        running:
          fw 1.8.0-28

See Documentation/networking/devlink/ionic.rst for more information
on the devlink dev info data.
(有关 devlink dev info 数据的更多信息，请参阅
Documentation/networking/devlink/ionic.rst。)

Enabling the driver
(启用驱动程序)
===================

The driver is enabled via the standard kernel configuration system,
using the make command::
(驱动程序通过标准内核配置系统启用，使用 make 命令：)

  make oldconfig/menuconfig/etc.

The driver is located in the menu structure at:
(该驱动程序位于菜单结构中的以下位置：)

  -> Device Drivers
    -> Network device support (NETDEVICES [=y])
      -> Ethernet driver support
        -> Pensando devices
          -> Pensando Ethernet IONIC Support

Configuring the Driver
(配置驱动程序)
======================

MTU
(MTU)
---

Jumbo frame support is available with a maximum size of 9194 bytes.
(支持巨型帧，最大尺寸为 9194 字节。)

Interrupt coalescing
(中断合并)
--------------------

Interrupt coalescing can be configured by changing the rx-usecs value with
the "ethtool -C" command.  The rx-usecs range is 0-190.  The tx-usecs value
reflects the rx-usecs value as they are tied together on the same interrupt.
(可以通过 "ethtool -C" 命令修改 rx-usecs 值来配置中断合并。rx-usecs 的
取值范围为 0-190。tx-usecs 值与 rx-usecs 值绑定在同一个中断上，因此二者
保持一致。)

SR-IOV
(SR-IOV)
------

Minimal SR-IOV support is currently offered and can be enabled by setting
the sysfs 'sriov_numvfs' value, if supported by your particular firmware
configuration.
(目前仅提供最基本的 SR-IOV 支持：如果您的特定固件配置支持，可以通过
设置 sysfs 的 'sriov_numvfs' 值来启用。)

XDP
(XDP)
---

Support for XDP includes the basics, plus Jumbo frames, Redirect and
ndo_xmit.  There is no current support for zero-copy sockets or HW offload.
(XDP 支持包括基本功能，外加巨型帧、重定向（Redirect）和 ndo_xmit。目前
不支持零拷贝套接字或硬件卸载。)

RDMA Support via Auxiliary Device
(通过辅助设备支持 RDMA)
=================================

The ionic driver supports RDMA (Remote Direct Memory Access) functionality
through the Linux auxiliary device framework when advertised by the firmware.
RDMA capability is detected during device initialization, and if supported,
the ethernet driver will create an auxiliary device that allows the RDMA
driver to bind and provide InfiniBand/RoCE functionality.
(当固件宣告支持时，ionic 驱动程序通过 Linux 辅助设备（auxiliary device）
框架支持 RDMA（远程直接内存访问）功能。RDMA 能力在设备初始化期间检测；
如果受支持，以太网驱动程序会创建一个辅助设备，允许 RDMA 驱动程序绑定
其上并提供 InfiniBand/RoCE 功能。)

Statistics
(统计信息)
==========

Basic hardware stats
(基本硬件统计)
--------------------

The commands ``netstat -i``, ``ip -s link show``, and ``ifconfig`` show
a limited set of statistics taken directly from firmware.  For example::
(``netstat -i``、``ip -s link show`` 和 ``ifconfig`` 命令显示直接取自
固件的有限一组统计信息。例如：)

  $ ip -s link show enp181s0
  7: enp181s0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq state UP mode DEFAULT group default qlen 1000
      link/ether 00:ae:cd:00:07:68 brd ff:ff:ff:ff:ff:ff
      RX: bytes  packets  errors  dropped overrun mcast
      414        5        0       0       0       0
      TX: bytes  packets  errors  dropped carrier collsns
      1384       18       0       0       0       0

ethtool -S
(ethtool -S)
----------

The statistics shown from the ``ethtool -S`` command includes a combination of
driver counters and firmware counters, including port and queue specific values.
The driver values are counters computed by the driver, and the firmware values
are gathered by the firmware from the port hardware and passed through the
driver with no further interpretation.
(``ethtool -S`` 命令显示的统计信息是驱动程序计数器与固件计数器的组合，
包括端口和队列专属的值。驱动程序值是由驱动程序计算的计数器，固件值则
由固件从端口硬件采集，并经驱动程序原样传递，不做进一步解释。)

Driver port specific::
(驱动程序端口专属计数：)

     tx_packets: 12
     tx_bytes: 964
     rx_packets: 5
     rx_bytes: 414
     tx_tso: 0
     tx_tso_bytes: 0
     tx_csum_none: 12
     tx_csum: 0
     rx_csum_none: 0
     rx_csum_complete: 3
     rx_csum_error: 0
     xdp_drop: 0
     xdp_aborted: 0
     xdp_pass: 0
     xdp_tx: 0
     xdp_redirect: 0
     xdp_frames: 0

Driver queue specific::
(驱动程序队列专属计数：)

     tx_0_pkts: 3
     tx_0_bytes: 294
     tx_0_clean: 3
     tx_0_dma_map_err: 0
     tx_0_linearize: 0
     tx_0_frags: 0
     tx_0_tso: 0
     tx_0_tso_bytes: 0
     tx_0_hwstamp_valid: 0
     tx_0_hwstamp_invalid: 0
     tx_0_csum_none: 3
     tx_0_csum: 0
     tx_0_vlan_inserted: 0
     tx_0_xdp_frames: 0
     rx_0_pkts: 2
     rx_0_bytes: 120
     rx_0_dma_map_err: 0
     rx_0_alloc_err: 0
     rx_0_csum_none: 0
     rx_0_csum_complete: 0
     rx_0_csum_error: 0
     rx_0_hwstamp_valid: 0
     rx_0_hwstamp_invalid: 0
     rx_0_dropped: 0
     rx_0_vlan_stripped: 0
     rx_0_xdp_drop: 0
     rx_0_xdp_aborted: 0
     rx_0_xdp_pass: 0
     rx_0_xdp_tx: 0
     rx_0_xdp_redirect: 0

Firmware port specific::
(固件端口专属计数：)

     hw_tx_dropped: 0
     hw_rx_dropped: 0
     hw_rx_over_errors: 0
     hw_rx_missed_errors: 0
     hw_tx_aborted_errors: 0
     frames_rx_ok: 15
     frames_rx_all: 15
     frames_rx_bad_fcs: 0
     frames_rx_bad_all: 0
     octets_rx_ok: 1290
     octets_rx_all: 1290
     frames_rx_unicast: 10
     frames_rx_multicast: 5
     frames_rx_broadcast: 0
     frames_rx_pause: 0
     frames_rx_bad_length: 0
     frames_rx_undersized: 0
     frames_rx_oversized: 0
     frames_rx_fragments: 0
     frames_rx_jabber: 0
     frames_rx_pripause: 0
     frames_rx_stomped_crc: 0
     frames_rx_too_long: 0
     frames_rx_vlan_good: 3
     frames_rx_dropped: 0
     frames_rx_less_than_64b: 0
     frames_rx_64b: 4
     frames_rx_65b_127b: 11
     frames_rx_128b_255b: 0
     frames_rx_256b_511b: 0
     frames_rx_512b_1023b: 0
     frames_rx_1024b_1518b: 0
     frames_rx_1519b_2047b: 0
     frames_rx_2048b_4095b: 0
     frames_rx_4096b_8191b: 0
     frames_rx_8192b_9215b: 0
     frames_rx_other: 0
     frames_tx_ok: 31
     frames_tx_all: 31
     frames_tx_bad: 0
     octets_tx_ok: 2614
     octets_tx_total: 2614
     frames_tx_unicast: 8
     frames_tx_multicast: 21
     frames_tx_broadcast: 2
     frames_tx_pause: 0
     frames_tx_pripause: 0
     frames_tx_vlan: 0
     frames_tx_less_than_64b: 0
     frames_tx_64b: 4
     frames_tx_65b_127b: 27
     frames_tx_128b_255b: 0
     frames_tx_256b_511b: 0
     frames_tx_512b_1023b: 0
     frames_tx_1024b_1518b: 0
     frames_tx_1519b_2047b: 0
     frames_tx_2048b_4095b: 0
     frames_tx_4096b_8191b: 0
     frames_tx_8192b_9215b: 0
     frames_tx_other: 0
     frames_tx_pri_0: 0
     frames_tx_pri_1: 0
     frames_tx_pri_2: 0
     frames_tx_pri_3: 0
     frames_tx_pri_4: 0
     frames_tx_pri_5: 0
     frames_tx_pri_6: 0
     frames_tx_pri_7: 0
     frames_rx_pri_0: 0
     frames_rx_pri_1: 0
     frames_rx_pri_2: 0
     frames_rx_pri_3: 0
     frames_rx_pri_4: 0
     frames_rx_pri_5: 0
     frames_rx_pri_6: 0
     frames_rx_pri_7: 0
     tx_pripause_0_1us_count: 0
     tx_pripause_1_1us_count: 0
     tx_pripause_2_1us_count: 0
     tx_pripause_3_1us_count: 0
     tx_pripause_4_1us_count: 0
     tx_pripause_5_1us_count: 0
     tx_pripause_6_1us_count: 0
     tx_pripause_7_1us_count: 0
     rx_pripause_0_1us_count: 0
     rx_pripause_1_1us_count: 0
     rx_pripause_2_1us_count: 0
     rx_pripause_3_1us_count: 0
     rx_pripause_4_1us_count: 0
     rx_pripause_5_1us_count: 0
     rx_pripause_6_1us_count: 0
     rx_pripause_7_1us_count: 0
     rx_pause_1us_count: 0
     frames_tx_truncated: 0


Support
(支持)
=======

For general Linux networking support, please use the netdev mailing
list, which is monitored by Pensando personnel::
(一般的 Linux 网络支持请使用 netdev 邮件列表，Pensando 人员会关注
该列表：)

  netdev@vger.kernel.org

For more specific support needs, please use the Pensando driver support
email::
(更具体的支持需求，请使用 Pensando 驱动支持邮箱：)

  drivers@pensando.io
