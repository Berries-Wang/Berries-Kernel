.. SPDX-License-Identifier: GPL-2.0

====================
Interface statistics
====================

(接口统计)

Overview
(概述)
========

This document is a guide to Linux network interface statistics.
(本文档是 Linux 网络接口统计指南。)

There are three main sources of interface statistics in Linux:
(Linux 中接口统计有三个主要来源：)

 - standard interface statistics based on
   :c:type:`struct rtnl_link_stats64 <rtnl_link_stats64>`;
   (基于 struct rtnl_link_stats64 的标准接口统计；)
 - protocol-specific statistics; and
   (协议特定统计；以及)
 - driver-defined statistics available via ethtool.
   (通过 ethtool 提供的驱动自定义统计。)

Standard interface statistics
(标准接口统计)
-----------------------------

There are multiple interfaces to reach the standard statistics.
Most commonly used is the `ip` command from `iproute2`::
(访问标准统计信息有多种接口。最常用的是 iproute2 中的 ip 命令：)

  $ ip -s -s link show dev ens4u1u1
  6: ens4u1u1: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc fq_codel state UP mode DEFAULT group default qlen 1000
    link/ether 48:2a:e3:4c:b1:d1 brd ff:ff:ff:ff:ff:ff
    RX: bytes  packets  errors  dropped overrun mcast
    74327665117 69016965 0       0       0       0
    RX errors: length   crc     frame   fifo    missed
               0        0       0       0       0
    TX: bytes  packets  errors  dropped carrier collsns
    21405556176 44608960 0       0       0       0
    TX errors: aborted  fifo   window heartbeat transns
               0        0       0       0       128
    altname enp58s0u1u1

Note that `-s` has been specified twice to see all members of
:c:type:`struct rtnl_link_stats64 <rtnl_link_stats64>`.
If `-s` is specified once the detailed errors won't be shown.
(注意，-s 需要指定两次才能看到 struct rtnl_link_stats64 的所有成员。如果只指定一次 -s，则不会显示详细的错误统计。)

`ip` supports JSON formatting via the `-j` option.
(ip 支持通过 -j 选项进行 JSON 格式化输出。)

Queue statistics
(队列统计)
~~~~~~~~~~~~~~~~

Queue statistics are accessible via the netdev netlink family.
(队列统计可通过 netdev netlink 族访问。)

Currently no widely distributed CLI exists to access those statistics.
Kernel development tools (ynl) can be used to experiment with them,
see `Documentation/userspace-api/netlink/intro-specs.rst`.
(目前尚无广泛发行的 CLI 可以访问这些统计信息。可以使用内核开发工具 (ynl) 对它们进行试验，参见 Documentation/userspace-api/netlink/intro-specs.rst。)

Protocol-specific statistics
(协议特定统计)
----------------------------

Protocol-specific statistics are exposed via relevant interfaces,
the same interfaces as are used to configure them.
(协议特定统计通过相关接口公开，与用于配置它们的接口相同。)

ethtool
(ethtool)
~~~~~~~

Ethtool exposes common low-level statistics.
All the standard statistics are expected to be maintained
by the device, not the driver (as opposed to driver-defined stats
described in the next section which mix software and hardware stats).
For devices which contain unmanaged
switches (e.g. legacy SR-IOV or multi-host NICs) the events counted
may not pertain exclusively to the packets destined to
the local host interface. In other words the events may
be counted at the network port (MAC/PHY blocks) without separation
for different host side (PCIe) devices. Such ambiguity must not
be present when internal switch is managed by Linux (so called
switchdev mode for NICs).
(Ethtool 公开常见的低层统计信息。所有标准统计都应当由设备而不是驱动来维护（这与下一节描述的驱动自定义统计不同，后者混合了软件与硬件统计）。对于包含非受管交换芯片的设备（例如旧式 SR-IOV 或 multi-host 网卡），所统计的事件可能并不只针对发往本地主机接口的数据包。换言之，事件可能是在网络端口处（MAC/PHY 模块）统计的，未针对不同的主机侧 (PCIe) 设备进行区分。当内部交换芯片由 Linux 管理（即所谓网卡的 switchdev 模式）时，则不得存在这种歧义。)

Standard ethtool statistics can be accessed via the interfaces used
for configuration. For example ethtool interface used
to configure pause frames can report corresponding hardware counters::
(标准 ethtool 统计可通过用于配置的接口访问。例如，用于配置暂停帧 (pause frames) 的 ethtool 接口可以报告相应的硬件计数器：)

  $ ethtool --include-statistics -a eth0
  Pause parameters for eth0:
  Autonegotiate:	on
  RX:			on
  TX:			on
  Statistics:
    tx_pause_frames: 1
    rx_pause_frames: 1

General Ethernet statistics not associated with any particular
functionality are exposed via ``ethtool -S $ifc`` by specifying
the ``--groups`` parameter::
(与任何特定功能都没有关联的一般以太网统计，可通过指定 --groups 参数、用 ethtool -S $ifc 命令公开：)

  $ ethtool -S eth0 --groups eth-phy eth-mac eth-ctrl rmon
  Stats for eth0:
  eth-phy-SymbolErrorDuringCarrier: 0
  eth-mac-FramesTransmittedOK: 1
  eth-mac-FrameTooLongErrors: 1
  eth-ctrl-MACControlFramesTransmitted: 1
  eth-ctrl-MACControlFramesReceived: 0
  eth-ctrl-UnsupportedOpcodesReceived: 1
  rmon-etherStatsUndersizePkts: 1
  rmon-etherStatsJabbers: 0
  rmon-rx-etherStatsPkts64Octets: 1
  rmon-rx-etherStatsPkts65to127Octets: 0
  rmon-rx-etherStatsPkts128to255Octets: 0
  rmon-tx-etherStatsPkts64Octets: 2
  rmon-tx-etherStatsPkts65to127Octets: 3
  rmon-tx-etherStatsPkts128to255Octets: 0

Driver-defined statistics
(驱动自定义统计)
-------------------------

Driver-defined ethtool statistics can be dumped using `ethtool -S $ifc`, e.g.::
(驱动自定义的 ethtool 统计可以用 ethtool -S $ifc 转储，例如：)

  $ ethtool -S ens4u1u1
  NIC statistics:
     tx_single_collisions: 0
     tx_multi_collisions: 0

uAPIs
(用户空间 API)
=====

procfs
(procfs)
------

The historical `/proc/net/dev` text interface gives access to the list
of interfaces as well as their statistics.
(历史上的 /proc/net/dev 文本接口提供对接口列表及其统计信息的访问。)

Note that even though this interface is using
:c:type:`struct rtnl_link_stats64 <rtnl_link_stats64>`
internally it combines some of the fields.
(注意，尽管该接口内部使用 struct rtnl_link_stats64，但它把其中一些字段合并了起来。)

sysfs
(sysfs)
------

Each device directory in sysfs contains a `statistics` directory (e.g.
`/sys/class/net/lo/statistics/`) with files corresponding to
members of :c:type:`struct rtnl_link_stats64 <rtnl_link_stats64>`.
(sysfs 中每个设备目录都包含一个 statistics 目录（例如 /sys/class/net/lo/statistics/），其中的文件对应 struct rtnl_link_stats64 的各个成员。)

This simple interface is convenient especially in constrained/embedded
environments without access to tools. However, it's inefficient when
reading multiple stats as it internally performs a full dump of
:c:type:`struct rtnl_link_stats64 <rtnl_link_stats64>`
and reports only the stat corresponding to the accessed file.
(这种简单接口在无法使用工具的受限/嵌入式环境中尤为方便。然而，读取多项统计时它的效率不高，因为它内部会对 struct rtnl_link_stats64 做一次完整转储，却只报告与被访问文件对应的那一项统计。)

Sysfs files are documented in
Documentation/ABI/testing/sysfs-class-net-statistics.
(sysfs 文件在 Documentation/ABI/testing/sysfs-class-net-statistics 中有文档说明。)


netlink
(netlink)
-------

`rtnetlink` (`NETLINK_ROUTE`) is the preferred method of accessing
:c:type:`struct rtnl_link_stats64 <rtnl_link_stats64>` stats.
(rtnetlink (NETLINK_ROUTE) 是访问 struct rtnl_link_stats64 统计信息的首选方法。)

Statistics are reported both in the responses to link information
requests (`RTM_GETLINK`) and statistic requests (`RTM_GETSTATS`,
when `IFLA_STATS_LINK_64` bit is set in the `.filter_mask` of the request).
(统计信息既在链路信息请求 (RTM_GETLINK) 的响应中报告，也在统计请求 (RTM_GETSTATS，当请求的 .filter_mask 中设置了 IFLA_STATS_LINK_64 位时) 中报告。)

netdev (netlink)
(netdev (netlink))
~~~~~~~~~~~~~~~~

`netdev` generic netlink family allows accessing page pool and per queue
statistics.
(netdev generic netlink 族允许访问页池 (page pool) 和每队列统计。)

ethtool
(ethtool)
-------

Ethtool IOCTL interface allows drivers to report implementation
specific statistics. Historically it has also been used to report
statistics for which other APIs did not exist, like per-device-queue
statistics, or standard-based statistics (e.g. RFC 2863).
(Ethtool IOCTL 接口允许驱动报告实现特定的统计。历史上它还被用于报告那些没有其他 API 的统计，例如每设备队列统计，或基于标准的统计（例如 RFC 2863）。)

Statistics and their string identifiers are retrieved separately.
Identifiers via `ETHTOOL_GSTRINGS` with `string_set` set to `ETH_SS_STATS`,
and values via `ETHTOOL_GSTATS`. User space should use `ETHTOOL_GDRVINFO`
to retrieve the number of statistics (`.n_stats`).
(统计值与其字符串标识符分别获取。标识符通过 ETHTOOL_GSTRINGS 获取（string_set 设为 ETH_SS_STATS），值通过 ETHTOOL_GSTATS 获取。用户空间应使用 ETHTOOL_GDRVINFO 来获取统计的数量 (.n_stats)。)

ethtool-netlink
(ethtool-netlink)
---------------

Ethtool netlink is a replacement for the older IOCTL interface.
(Ethtool netlink 是较旧的 IOCTL 接口的替代品。)

Protocol-related statistics can be requested in get commands by setting
the `ETHTOOL_FLAG_STATS` flag in `ETHTOOL_A_HEADER_FLAGS`. Currently
statistics are supported in the following commands:
(通过在 ETHTOOL_A_HEADER_FLAGS 中设置 ETHTOOL_FLAG_STATS 标志，可以在 get 命令中请求与协议相关的统计。目前以下命令支持统计信息：)

  - `ETHTOOL_MSG_FEC_GET`
    (ETHTOOL_MSG_FEC_GET)
  - `ETHTOOL_MSG_LINKSTATE_GET`
    (ETHTOOL_MSG_LINKSTATE_GET)
  - `ETHTOOL_MSG_MM_GET`
    (ETHTOOL_MSG_MM_GET)
  - `ETHTOOL_MSG_PAUSE_GET`
    (ETHTOOL_MSG_PAUSE_GET)
  - `ETHTOOL_MSG_TSINFO_GET`
    (ETHTOOL_MSG_TSINFO_GET)

debugfs
(debugfs)
-------

Some drivers expose extra statistics via `debugfs`.
(一些驱动通过 debugfs 公开额外的统计信息。)

struct rtnl_link_stats64
(struct rtnl_link_stats64 结构)
========================

.. kernel-doc:: include/uapi/linux/if_link.h
    :identifiers: rtnl_link_stats64

Notes for driver authors
(驱动作者须知)
========================

Drivers should report all statistics which have a matching member in
:c:type:`struct rtnl_link_stats64 <rtnl_link_stats64>` exclusively
via `.ndo_get_stats64`. Reporting such standard stats via ethtool
or debugfs will not be accepted.
(凡在 struct rtnl_link_stats64 中有对应成员的统计，驱动都应当只通过 .ndo_get_stats64 报告。通过 ethtool 或 debugfs 报告这类标准统计将不被接受。)

Drivers must ensure best possible compliance with
:c:type:`struct rtnl_link_stats64 <rtnl_link_stats64>`.
Please note for example that detailed error statistics must be
added into the general `rx_error` / `tx_error` counters.
(驱动必须尽可能符合 struct rtnl_link_stats64。例如请注意，详细的错误统计必须累加进总的 rx_error / tx_error 计数器。)

The `.ndo_get_stats64` callback can not sleep because of accesses
via `/proc/net/dev`. If driver may sleep when retrieving the statistics
from the device it should do so periodically asynchronously and only return
a recent copy from `.ndo_get_stats64`. Ethtool interrupt coalescing interface
allows setting the frequency of refreshing statistics, if needed.
(由于存在经由 /proc/net/dev 的访问，.ndo_get_stats64 回调不能睡眠。如果驱动在从设备获取统计时可能睡眠，它应当周期性地以异步方式进行，而 .ndo_get_stats64 只返回最近的一份副本。如有需要，Ethtool 中断合并接口允许设置统计刷新的频率。)

Retrieving ethtool statistics is a multi-syscall process, drivers are advised
to keep the number of statistics constant to avoid race conditions with
user space trying to read them.
(获取 ethtool 统计是一个需要多次系统调用的过程，建议驱动保持统计数量恒定，以避免与试图读取它们的用户空间发生竞争条件。)

Statistics must persist across routine operations like bringing the interface
down and up.
(统计必须在诸如关闭再打开接口之类的常规操作中保持不变。)

Kernel-internal data structures
(内核内部数据结构)
-------------------------------

The following structures are internal to the kernel, their members are
translated to netlink attributes when dumped. Drivers must not overwrite
the statistics they don't report with 0.
(以下结构为内核内部使用，在转储时其成员会被转换为 netlink 属性。驱动不得用 0 覆盖自己不报告的统计。)

- ethtool_pause_stats()
  (ethtool_pause_stats())
- ethtool_fec_stats()
  (ethtool_fec_stats())
