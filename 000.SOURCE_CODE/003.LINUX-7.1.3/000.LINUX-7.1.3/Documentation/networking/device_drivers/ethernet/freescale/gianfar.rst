.. SPDX-License-Identifier: GPL-2.0

===============================================
The Gianfar Ethernet Driver(Gianfar 以太网驱动)
===============================================

:Author: Andy Fleming <afleming@freescale.com>
         (作者: Andy Fleming <afleming@freescale.com>)
:Updated: 2005-07-28
         (更新于: 2005-07-28)


Checksum Offloading(校验和卸载)
===============================

The eTSEC controller (first included in parts from late 2005 like
the 8548) has the ability to perform TCP, UDP, and IP checksums
in hardware.  The Linux kernel only offloads the TCP and UDP
checksums (and always performs the pseudo header checksums), so
the driver only supports checksumming for TCP/IP and UDP/IP
packets.  Use ethtool to enable or disable this feature for RX
and TX.
(eTSEC 控制器(最早包含于 2005 年末的芯片中，例如 8548)具备在硬件中计算 TCP、UDP 和 IP 校验和的能力。Linux 内核只卸载 TCP 和 UDP 校验和(并且总是自行计算伪头校验和)，因此该驱动只支持对 TCP/IP 和 UDP/IP 数据包进行校验和计算。可以使用 ethtool 为 RX 和 TX 启用或禁用该特性。)

VLAN
====
(VLAN)

In order to use VLAN, please consult Linux documentation on
configuring VLANs.  The gianfar driver supports hardware insertion and
extraction of VLAN headers, but not filtering.  Filtering will be
done by the kernel.
(要使用 VLAN，请查阅 Linux 中关于配置 VLAN 的文档。gianfar 驱动支持以硬件方式插入和提取 VLAN 头，但不支持过滤。过滤将由内核完成。)

Multicasting(组播)
==================

The gianfar driver supports using the group hash table on the
TSEC (and the extended hash table on the eTSEC) for multicast
filtering.  On the eTSEC, the exact-match MAC registers are used
before the hash tables.  See Linux documentation on how to join
multicast groups.
(gianfar 驱动支持使用 TSEC 上的组哈希表(以及 eTSEC 上的扩展哈希表)进行组播过滤。在 eTSEC 上，会先使用精确匹配的 MAC 寄存器，然后才使用哈希表。关于如何加入组播组，请参阅 Linux 文档。)

Padding(填充)
=============

The gianfar driver supports padding received frames with 2 bytes
to align the IP header to a 16-byte boundary, when supported by
hardware.
(在硬件支持的情况下，gianfar 驱动支持对接收到的帧填充 2 个字节，使 IP 头对齐到 16 字节边界。)

Ethtool
=======
(Ethtool)

The gianfar driver supports the use of ethtool for many
configuration options.  You must run ethtool only on currently
open interfaces.  See ethtool documentation for details.
(gianfar 驱动支持使用 ethtool 进行多种配置。只能在当前已打开的接口上运行 ethtool。详情请参阅 ethtool 文档。)
