.. SPDX-License-Identifier: GPL-2.0+

========================================================================================================================
Linux Base Driver for Intel(R) Ethernet Multi-host Controller(Intel(R) Ethernet Multi-host Controller 的 Linux 基础驱动)
========================================================================================================================

August 20, 2018
Copyright(c) 2015-2018 Intel Corporation.
(2018 年 8 月 20 日
版权所有 (c) 2015-2018 Intel Corporation。)

Contents(目录)
==============

- Identifying Your Adapter
  (识别你的适配器)
- Additional Configurations
  (其他配置)
- Performance Tuning
  (性能调优)
- Known Issues
  (已知问题)
- Support
  (支持)

Identifying Your Adapter(识别你的适配器)
========================================

The driver in this release is compatible with devices based on the Intel(R)
Ethernet Multi-host Controller.
(本版本中的驱动兼容基于 Intel(R) Ethernet Multi-host Controller 的设备。)

For information on how to identify your adapter, and for the latest Intel
network drivers, refer to the Intel Support website:
https://www.intel.com/support
(有关如何识别你的适配器的信息以及最新的 Intel 网络驱动，请参阅 Intel 支持
网站： https://www.intel.com/support)


Flow Control(流控)
------------------

The Intel(R) Ethernet Switch Host Interface Driver does not support Flow
Control. It will not send pause frames. This may result in dropped frames.
(Intel(R) Ethernet Switch Host Interface 驱动不支持流控。它不会发送暂停帧。
这可能导致丢帧。)


Virtual Functions (VFs)(虚拟功能 (VFs))
---------------------------------------

Use sysfs to enable VFs.
Valid Range: 0-64
(使用 sysfs 启用 VF。有效范围： 0-64)

For example::
(例如：)

    echo $num_vf_enabled > /sys/class/net/$dev/device/sriov_numvfs //enable VFs
    echo 0 > /sys/class/net/$dev/device/sriov_numvfs //disable VFs

NOTE: Neither the device nor the driver control how VFs are mapped into config
space. Bus layout will vary by operating system. On operating systems that
support it, you can check sysfs to find the mapping.
(注意： 设备和驱动都不控制 VF 如何映射到配置空间。总线布局因操作系统而异。
在支持该功能的操作系统上，可以检查 sysfs 来找到映射。)

NOTE: When SR-IOV mode is enabled, hardware VLAN filtering and VLAN tag
stripping/insertion will remain enabled. Please remove the old VLAN filter
before the new VLAN filter is added. For example::
(注意： 当 SR-IOV 模式启用时，硬件 VLAN 过滤与 VLAN 标签剥离/插入将保持
启用状态。请在添加新的 VLAN 过滤器之前先移除旧的 VLAN 过滤器。例如：)

    ip link set eth0 vf 0 vlan 100	// set vlan 100 for VF 0
    ip link set eth0 vf 0 vlan 0	// Delete vlan 100
    ip link set eth0 vf 0 vlan 200	// set a new vlan 200 for VF 0


Additional Features and Configurations(其他特性与配置)
======================================================

Jumbo Frames(巨型帧)
--------------------

Jumbo Frames support is enabled by changing the Maximum Transmission Unit (MTU)
to a value larger than the default value of 1500.
(将最大传输单元 (MTU) 改为大于默认值 1500 即可启用巨型帧支持。)

Use the ifconfig command to increase the MTU size. For example, enter the
following where <x> is the interface number::
(使用 ifconfig 命令增大 MTU 大小。例如，在 <x> 为接口号处输入以下命令：)

    ifconfig eth<x> mtu 9000 up

Alternatively, you can use the ip command as follows::
(或者，也可以按如下方式使用 ip 命令：)

    ip link set mtu 9000 dev eth<x>
    ip link set up dev eth<x>

This setting is not saved across reboots. The setting change can be made
permanent by adding 'MTU=9000' to the file:
(此设置在重启后不会保留。可以通过在以下文件中添加 'MTU=9000' 使该设置
永久生效：)

- For RHEL: /etc/sysconfig/network-scripts/ifcfg-eth<x>
  (对于 RHEL： /etc/sysconfig/network-scripts/ifcfg-eth<x>)
- For SLES: /etc/sysconfig/network/<config_file>
  (对于 SLES： /etc/sysconfig/network/<config_file>)

NOTE: The maximum MTU setting for Jumbo Frames is 15342. This value coincides
with the maximum Jumbo Frames size of 15364 bytes.
(注意： 巨型帧的最大 MTU 设置为 15342。该值与 15364 字节的最大巨型帧大小
相对应。)

NOTE: This driver will attempt to use multiple page sized buffers to receive
each jumbo packet. This should help to avoid buffer starvation issues when
allocating receive packets.
(注意： 此驱动会尝试使用多个页面大小的缓冲区来接收每个巨型数据包。这应当
有助于避免在分配接收数据包时出现缓冲区不足的问题。)


Generic Receive Offload, aka GRO(通用接收卸载，即 GRO)
------------------------------------------------------

The driver supports the in-kernel software implementation of GRO. GRO has
shown that by coalescing Rx traffic into larger chunks of data, CPU
utilization can be significantly reduced when under large Rx load. GRO is an
evolution of the previously-used LRO interface. GRO is able to coalesce
other protocols besides TCP. It's also safe to use with configurations that
are problematic for LRO, namely bridging and iSCSI.
(驱动支持内核内软件实现的 GRO。实践表明，通过把接收 (Rx) 流量合并成更大的
数据块，在高接收负载下可以显著降低 CPU 利用率。GRO 是先前使用的 LRO 接口
的演进版本。GRO 能够合并 TCP 以外的其他协议。对于对 LRO 有问题的配置
(即网桥和 iSCSI)，使用 GRO 也是安全的。)



Supported ethtool Commands and Options for Filtering(支持的用于过滤的 ethtool 命令与选项)
-----------------------------------------------------------------------------------------

-n --show-nfc
  Retrieves the receive network flow classification configurations.
  (获取接收网络流分类配置。)

rx-flow-hash tcp4|udp4|ah4|esp4|sctp4|tcp6|udp6|ah6|esp6|sctp6
  Retrieves the hash options for the specified network traffic type.
  (获取指定网络流量类型的哈希选项。)

-N --config-nfc
  Configures the receive network flow classification.
  (配置接收网络流分类。)

rx-flow-hash tcp4|udp4|ah4|esp4|sctp4|tcp6|udp6|ah6|esp6|sctp6 m|v|t|s|d|f|n|r
  Configures the hash options for the specified network traffic type.
  (配置指定网络流量类型的哈希选项。)

- udp4: UDP over IPv4
  (udp4: IPv4 上的 UDP)
- udp6: UDP over IPv6
  (udp6: IPv6 上的 UDP)
- f Hash on bytes 0 and 1 of the Layer 4 header of the rx packet.
  (f 对接收数据包第 4 层头部的第 0 和第 1 字节进行哈希。)
- n Hash on bytes 2 and 3 of the Layer 4 header of the rx packet.
  (n 对接收数据包第 4 层头部的第 2 和第 3 字节进行哈希。)


Known Issues/Troubleshooting(已知问题/故障排查)
===============================================

Enabling SR-IOV in a 64-bit Microsoft Windows Server 2012/R2 guest OS under Linux KVM(在 Linux KVM 下的 64 位 Microsoft Windows Server 2012/R2 客户机操作系统中启用 SR-IOV)
---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

KVM Hypervisor/VMM supports direct assignment of a PCIe device to a VM. This
includes traditional PCIe devices, as well as SR-IOV-capable devices based on
the Intel Ethernet Controller XL710.
(KVM Hypervisor/VMM 支持将 PCIe 设备直接分配给虚拟机。这包括传统 PCIe 设备，
以及基于 Intel Ethernet Controller XL710 的支持 SR-IOV 的设备。)


Support(支持)
=============

For general information, go to the Intel support website at:
https://www.intel.com/support/
(一般性信息请访问 Intel 支持网站： https://www.intel.com/support/)

If an issue is identified with the released source code on a supported kernel
with a supported adapter, email the specific information related to the issue
to intel-wired-lan@lists.osuosl.org.
(如果在受支持的内核上配合受支持的适配器使用已发布的源代码时发现某问题，
请将与该问题相关的具体信息通过邮件发送到 intel-wired-lan@lists.osuosl.org。)
