.. SPDX-License-Identifier: GPL-2.0

========================================
Bare UDP Tunnelling Module Documentation
========================================

(Bare UDP 隧道模块文档)

There are various L3 encapsulation standards using UDP being discussed to
leverage the UDP based load balancing capability of different networks.
MPLSoUDP (https://tools.ietf.org/html/rfc7510) is one among them.
(目前有多种使用 UDP 的 L3 封装标准正在讨论之中，以利用不同网络基于 UDP 的负载均衡能力。MPLSoUDP (https://tools.ietf.org/html/rfc7510) 就是其中之一。)

The Bareudp tunnel module provides a generic L3 encapsulation support for
tunnelling different L3 protocols like MPLS, IP, NSH etc. inside a UDP tunnel.
(Bareudp 隧道模块提供通用的 L3 封装支持，可在 UDP 隧道内封装 MPLS、IP、NSH 等不同的 L3 协议。)

Special Handling
----------------
(特殊处理)

The bareudp device supports special handling for MPLS & IP as they can have
multiple ethertypes.
The MPLS protocol can have ethertypes ETH_P_MPLS_UC (unicast) & ETH_P_MPLS_MC (multicast).
IP protocol can have ethertypes ETH_P_IP (v4) & ETH_P_IPV6 (v6).
This special handling can be enabled only for ethertypes ETH_P_IP & ETH_P_MPLS_UC
with a flag called multiproto mode.
(bareudp 设备支持对 MPLS 和 IP 的特殊处理，因为它们可以具有多种以太网类型 (ethertype)。
MPLS 协议可以具有 ethertype ETH_P_MPLS_UC(单播)和 ETH_P_MPLS_MC(组播)。
IP 协议可以具有 ethertype ETH_P_IP(v4)和 ETH_P_IPV6(v6)。
这种特殊处理只能通过一个名为 multiproto 模式的标志针对 ethertype ETH_P_IP 和 ETH_P_MPLS_UC 启用。)

Usage
------
(用法)

1) Device creation & deletion
   (创建与删除设备)

    a) ip link add dev bareudp0 type bareudp dstport 6635 ethertype mpls_uc

       This creates a bareudp tunnel device which tunnels L3 traffic with ethertype
       0x8847 (MPLS traffic). The destination port of the UDP header will be set to
       6635.The device will listen on UDP port 6635 to receive traffic.
       (这会创建一个 bareudp 隧道设备，用于将以太网类型为 0x8847(MPLS 流量)的 L3 流量封装进隧道。UDP 头的目的端口将被设置为 6635。该设备将监听 UDP 端口 6635 以接收流量。)

    b) ip link delete bareudp0

2) Device creation with multiproto mode enabled
   (启用 multiproto 模式创建设备)

The multiproto mode allows bareudp tunnels to handle several protocols of the
same family. It is currently only available for IP and MPLS. This mode has to
be enabled explicitly with the "multiproto" flag.
(multiproto 模式允许 bareudp 隧道处理同一协议族的多种协议。目前仅对 IP 和 MPLS 可用。必须通过 "multiproto" 标志显式启用该模式。)

    a) ip link add dev bareudp0 type bareudp dstport 6635 ethertype ipv4 multiproto

       For an IPv4 tunnel the multiproto mode allows the tunnel to also handle
       IPv6.
       (对于 IPv4 隧道，multiproto 模式允许该隧道同时处理 IPv6。)

    b) ip link add dev bareudp0 type bareudp dstport 6635 ethertype mpls_uc multiproto

       For MPLS, the multiproto mode allows the tunnel to handle both unicast
       and multicast MPLS packets.
       (对于 MPLS，multiproto 模式允许该隧道同时处理单播和组播 MPLS 数据包。)

3) Device Usage
   (设备使用)

The bareudp device could be used along with OVS or flower filter in TC.
The OVS or TC flower layer must set the tunnel information in the SKB dst field before
sending the packet buffer to the bareudp device for transmission. On reception, the
bareUDP device extracts and stores the tunnel information in the SKB dst field before
passing the packet buffer to the network stack.
(bareudp 设备可与 OVS 或 TC 中的 flower 过滤器配合使用。在把数据包缓冲区发送给 bareudp 设备进行传输之前，OVS 或 TC flower 层必须在 SKB dst 字段中设置隧道信息。在接收方向，bareUDP 设备在把数据包缓冲区交给网络栈之前，会先提取隧道信息并存入 SKB dst 字段。)
