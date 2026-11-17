.. SPDX-License-Identifier: GPL-2.0

======================================================
Virtual eXtensible Local Area Networking documentation
======================================================

(虚拟可扩展局域网 (VXLAN) 文档)

The VXLAN protocol is a tunnelling protocol designed to solve the
problem of limited VLAN IDs (4096) in IEEE 802.1q.  With VXLAN the
size of the identifier is expanded to 24 bits (16777216).
(VXLAN 协议是一种隧道协议，旨在解决 IEEE 802.1q 中 VLAN ID 有限(4096)的问题。使用 VXLAN 后，标识符的长度扩展到 24 位(16777216)。)

VXLAN is described by IETF RFC 7348, and has been implemented by a
number of vendors.  The protocol runs over UDP using a single
destination port.  This document describes the Linux kernel tunnel
device, there is also a separate implementation of VXLAN for
Openvswitch.
(VXLAN 由 IETF RFC 7348 描述，并已被多家厂商实现。该协议运行在 UDP 之上，使用单一目的端口。本文档描述 Linux 内核的隧道设备；Openvswitch 中另有独立的 VXLAN 实现。)

Unlike most tunnels, a VXLAN is a 1 to N network, not just point to
point. A VXLAN device can learn the IP address of the other endpoint
either dynamically in a manner similar to a learning bridge, or make
use of statically-configured forwarding entries.
(与大多数隧道不同，VXLAN 是一个 1 对 N 的网络，而不仅仅是点对点。VXLAN 设备既可以通过类似学习网桥的方式动态学习对端的 IP 地址，也可以使用静态配置的转发条目。)

The management of vxlan is done in a manner similar to its two closest
neighbors GRE and VLAN. Configuring VXLAN requires the version of
iproute2 that matches the kernel release where VXLAN was first merged
upstream.
(vxlan 的管理方式与其最接近的两种同类技术 GRE 和 VLAN 类似。配置 VXLAN 需要使用与 VXLAN 首次合入上游时对应内核版本相匹配的 iproute2 版本。)

1. Create vxlan device::
   (创建 vxlan 设备：)

    # ip link add vxlan0 type vxlan id 42 group 239.1.1.1 dev eth1 dstport 4789

This creates a new device named vxlan0.  The device uses the multicast
group 239.1.1.1 over eth1 to handle traffic for which there is no
entry in the forwarding table.  The destination port number is set to
the IANA-assigned value of 4789.  The Linux implementation of VXLAN
pre-dates the IANA's selection of a standard destination port number
and uses the Linux-selected value by default to maintain backwards
compatibility.
(这会创建一个名为 vxlan0 的新设备。该设备通过 eth1 上的组播组 239.1.1.1 来处理转发表中没有对应条目的流量。目的端口号设置为 IANA 分配的 4789。VXLAN 的 Linux 实现早于 IANA 选定标准目的端口号，为保持向后兼容，默认仍使用 Linux 自行选定的值。)

2. Delete vxlan device::
   (删除 vxlan 设备：)

    # ip link delete vxlan0

3. Show vxlan info::
   (显示 vxlan 信息：)

    # ip -d link show vxlan0

It is possible to create, destroy and display the vxlan
forwarding table using the new bridge command.
(可以使用新的 bridge 命令来创建、删除和显示 vxlan 转发表。)

1. Create forwarding table entry::
   (创建转发表条目：)

    # bridge fdb add to 00:17:42:8a:b4:05 dst 192.19.0.2 dev vxlan0

2. Delete forwarding table entry::
   (删除转发表条目：)

    # bridge fdb delete 00:17:42:8a:b4:05 dev vxlan0

3. Show forwarding table::
   (显示转发表：)

    # bridge fdb show dev vxlan0

The following NIC features may indicate support for UDP tunnel-related
offloads (most commonly VXLAN features, but support for a particular
encapsulation protocol is NIC specific):
(以下网卡 (NIC) 特性可能表明支持与 UDP 隧道相关的卸载(最常见的是 VXLAN 特性，但对特定封装协议的支持因网卡而异)：)

 - `tx-udp_tnl-segmentation`
 - `tx-udp_tnl-csum-segmentation`
    ability to perform TCP segmentation offload of UDP encapsulated frames
    (对 UDP 封装帧执行 TCP 分段卸载的能力)

 - `rx-udp_tunnel-port-offload`
    receive side parsing of UDP encapsulated frames which allows NICs to
    perform protocol-aware offloads, like checksum validation offload of
    inner frames (only needed by NICs without protocol-agnostic offloads)
    (接收方向对 UDP 封装帧的解析，使网卡能够执行协议感知的卸载，例如对内层帧的校验和验证卸载(只有不具备协议无关卸载能力的网卡才需要))

For devices supporting `rx-udp_tunnel-port-offload` the list of currently
offloaded ports can be interrogated with `ethtool`::
(对于支持 `rx-udp_tunnel-port-offload` 的设备，可以使用 `ethtool` 查询当前已卸载端口的列表：)

  $ ethtool --show-tunnels eth0
  Tunnel information for eth0:
    UDP port table 0:
      Size: 4
      Types: vxlan
      No entries
    UDP port table 1:
      Size: 4
      Types: geneve, vxlan-gpe
      Entries (1):
          port 1230, vxlan-gpe
