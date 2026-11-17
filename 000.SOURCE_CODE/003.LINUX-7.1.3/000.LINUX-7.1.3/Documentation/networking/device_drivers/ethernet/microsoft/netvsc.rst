.. SPDX-License-Identifier: GPL-2.0

========================================
Hyper-V network driver(Hyper-V 网络驱动)
========================================

Compatibility(兼容性)
=====================

This driver is compatible with Windows Server 2012 R2, 2016 and
Windows 10.
(此驱动兼容 Windows Server 2012 R2、2016 和 Windows 10。)

Features(特性)
==============

Checksum offload(校验和卸载)
----------------------------

  The netvsc driver supports checksum offload as long as the
  Hyper-V host version does. Windows Server 2016 and Azure
  support checksum offload for TCP and UDP for both IPv4 and
  IPv6. Windows Server 2012 only supports checksum offload for TCP.
  (只要 Hyper-V 主机版本支持，netvsc 驱动就支持校验和卸载。Windows
  Server 2016 和 Azure 支持 IPv4 与 IPv6 的 TCP 和 UDP 校验和卸载。
  Windows Server 2012 只支持 TCP 校验和卸载。)

Receive Side Scaling(接收侧扩展)
--------------------------------

  Hyper-V supports receive side scaling. For TCP & UDP, packets can
  be distributed among available queues based on IP address and port
  number.
  (Hyper-V 支持接收侧扩展。对于 TCP 和 UDP，数据包可以基于 IP 地址和
  端口号分发到各个可用队列上。)

  For TCP & UDP, we can switch hash level between L3 and L4 by ethtool
  command. TCP/UDP over IPv4 and v6 can be set differently. The default
  hash level is L4. We currently only allow switching TX hash level
  from within the guests.
  (对于 TCP 和 UDP，可以通过 ethtool 命令在 L3 与 L4 之间切换哈希级别。
  IPv4 和 IPv6 上的 TCP/UDP 可以设置不同的哈希级别。默认哈希级别是 L4。
  目前我们只允许在来宾 (guest) 内部切换 TX 哈希级别。)

  On Azure, fragmented UDP packets have high loss rate with L4
  hashing. Using L3 hashing is recommended in this case.
  (在 Azure 上，使用 L4 哈希时，分片的 UDP 数据包丢包率很高。这种情况下
  建议使用 L3 哈希。)

  For example, for UDP over IPv4 on eth0:
  (例如，对于 eth0 上的 IPv4 UDP：)

  To include UDP port numbers in hashing::
  (要在哈希中包含 UDP 端口号：)

	ethtool -N eth0 rx-flow-hash udp4 sdfn

  To exclude UDP port numbers in hashing::
  (要在哈希中排除 UDP 端口号：)

	ethtool -N eth0 rx-flow-hash udp4 sd

  To show UDP hash level::
  (要查看 UDP 哈希级别：)

	ethtool -n eth0 rx-flow-hash udp4

Generic Receive Offload, aka GRO(通用接收卸载，即 GRO)
------------------------------------------------------

  The driver supports GRO and it is enabled by default. GRO coalesces
  like packets and significantly reduces CPU usage under heavy Rx
  load.
  (驱动支持 GRO 且默认启用。GRO 会合并同类数据包，在高接收负载下显著
  降低 CPU 占用。)

Large Receive Offload (LRO), or Receive Side Coalescing (RSC)(大接收卸载 (LRO)，即接收侧合并 (RSC))
---------------------------------------------------------------------------------------------------

  The driver supports LRO/RSC in the vSwitch feature. It reduces the per packet
  processing overhead by coalescing multiple TCP segments when possible. The
  feature is enabled by default on VMs running on Windows Server 2019 and
  later. It may be changed by ethtool command::
  (驱动在 vSwitch 特性中支持 LRO/RSC。它通过在可能时合并多个 TCP 段来
  降低每数据包的处理开销。该特性在运行于 Windows Server 2019 及更高版本
  的 VM 上默认启用。可以通过 ethtool 命令更改：)

	ethtool -K eth0 lro on
	ethtool -K eth0 lro off

SR-IOV support(SR-IOV 支持)
---------------------------

  Hyper-V supports SR-IOV as a hardware acceleration option. If SR-IOV
  is enabled in both the vSwitch and the guest configuration, then the
  Virtual Function (VF) device is passed to the guest as a PCI
  device. In this case, both a synthetic (netvsc) and VF device are
  visible in the guest OS and both NIC's have the same MAC address.
  (Hyper-V 支持 SR-IOV 作为硬件加速选项。如果 SR-IOV 在 vSwitch 和来宾
  配置中都启用，那么虚拟功能 (VF) 设备会作为 PCI 设备传递给来宾。这种
  情况下，来宾操作系统中会同时看到合成 (netvsc) 设备和 VF 设备，且两块
  网卡的 MAC 地址相同。)

  The VF is enslaved by netvsc device.  The netvsc driver will transparently
  switch the data path to the VF when it is available and up.
  Network state (addresses, firewall, etc) should be applied only to the
  netvsc device; the slave device should not be accessed directly in
  most cases.  The exceptions are if some special queue discipline or
  flow direction is desired, these should be applied directly to the
  VF slave device.
  (VF 被 netvsc 设备收编为从属设备。当 VF 可用且已启动时，netvsc 驱动
  会透明地把数据路径切换到 VF。网络状态 (地址、防火墙等) 应只应用到
  netvsc 设备；在大多数情况下不应直接访问从属设备。例外情况是：如果需要
  特殊的排队规则或流导向，这些应直接应用到 VF 从属设备。)

Receive Buffer(接收缓冲区)
--------------------------

  Packets are received into a receive area which is created when device
  is probed. The receive area is broken into MTU sized chunks and each may
  contain one or more packets. The number of receive sections may be changed
  via ethtool Rx ring parameters.
  (数据包被接收到一个接收区中，该接收区在设备被探测时创建。接收区被
  分割成 MTU 大小的块，每块可以包含一个或多个数据包。接收段的数量可以
  通过 ethtool Rx 环参数更改。)

  There is a similar send buffer which is used to aggregate packets
  for sending.  The send area is broken into chunks, typically of 6144
  bytes, each of section may contain one or more packets. Small
  packets are usually transmitted via copy to the send buffer. However,
  if the buffer is temporarily exhausted, or the packet to be transmitted is
  an LSO packet, the driver will provide the host with pointers to the data
  from the SKB. This attempts to achieve a balance between the overhead of
  data copy and the impact of remapping VM memory to be accessible by the
  host.
  (有一个类似的发送缓冲区，用于聚合要发送的数据包。发送区被分割成块，
  通常每块 6144 字节，每段可以包含一个或多个数据包。小数据包通常通过
  复制到发送缓冲区来传输。但是，如果缓冲区暂时耗尽，或要发送的数据包是
  LSO 数据包，驱动会向主机提供指向 SKB 中数据的指针。这试图在数据复制
  开销与重新映射 VM 内存以使主机可访问所造成的影响之间取得平衡。)

XDP support(XDP 支持)
---------------------

  XDP (eXpress Data Path) is a feature that runs eBPF bytecode at the early
  stage when packets arrive at a NIC card. The goal is to increase performance
  for packet processing, reducing the overhead of SKB allocation and other
  upper network layers.
  (XDP (eXpress Data Path) 是一种在数据包到达网卡的早期阶段运行 eBPF
  字节码的特性。其目标是提升数据包处理性能，减少 SKB 分配和其他上层
  网络层的开销。)

  hv_netvsc supports XDP in native mode, and transparently sets the XDP
  program on the associated VF NIC as well.
  (hv_netvsc 以原生模式支持 XDP，并且还会透明地在关联的 VF 网卡上设置
  XDP 程序。)

  Setting / unsetting XDP program on synthetic NIC (netvsc) propagates to
  VF NIC automatically. Setting / unsetting XDP program on VF NIC directly
  is not recommended, also not propagated to synthetic NIC, and may be
  overwritten by setting of synthetic NIC.
  (在合成网卡 (netvsc) 上设置/取消设置 XDP 程序会自动传播到 VF 网卡。
  不建议直接在 VF 网卡上设置/取消设置 XDP 程序，它也不会传播到合成网卡，
  并且可能被合成网卡上的设置所覆盖。)

  XDP program cannot run with LRO (RSC) enabled, so you need to disable LRO
  before running XDP::
  (XDP 程序无法在启用 LRO (RSC) 的情况下运行，因此在运行 XDP 之前需要
  禁用 LRO：)

	ethtool -K eth0 lro off

  XDP_REDIRECT action is not yet supported.
  (尚不支持 XDP_REDIRECT 动作。)
