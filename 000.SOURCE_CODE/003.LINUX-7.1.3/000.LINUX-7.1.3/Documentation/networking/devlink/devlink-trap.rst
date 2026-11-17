.. SPDX-License-Identifier: GPL-2.0

================================
Devlink Trap(Devlink Trap(陷阱))
================================

Background(背景)
================

Devices capable of offloading the kernel's datapath and perform functions such
as bridging and routing must also be able to send specific packets to the
kernel (i.e., the CPU) for processing.
(能够对内核数据路径进行卸载并执行桥接、路由等功能的设备，还必须能够将
特定的数据包送往内核(即 CPU)进行处理。)

For example, a device acting as a multicast-aware bridge must be able to send
IGMP membership reports to the kernel for processing by the bridge module.
Without processing such packets, the bridge module could never populate its
MDB.
(例如，作为支持组播的网桥工作的设备，必须能够将 IGMP 成员关系报告送往
内核，由网桥模块进行处理。若不处理这类数据包，网桥模块将永远无法填充其
MDB。)

As another example, consider a device acting as router which has received an IP
packet with a TTL of 1. Upon routing the packet the device must send it to the
kernel so that it will route it as well and generate an ICMP Time Exceeded
error datagram. Without letting the kernel route such packets itself, utilities
such as ``traceroute`` could never work.
(再举一例：设想一台作为路由器工作的设备收到了一个 TTL 为 1 的 IP 数据包。
在路由该数据包之后，设备必须把它送往内核，让内核也对其进行路由并生成
ICMP Time Exceeded 差错数据报。如果不让内核亲自路由这类数据包，
``traceroute`` 之类的工具将永远无法工作。)

The fundamental ability of sending certain packets to the kernel for processing
is called "packet trapping".
(将特定数据包送往内核进行处理这一基本能力称为 "数据包陷阱"
(packet trapping)。)

Overview(概述)
==============

The ``devlink-trap`` mechanism allows capable device drivers to register their
supported packet traps with ``devlink`` and report trapped packets to
``devlink`` for further analysis.
(``devlink-trap`` 机制允许具备能力的设备驱动将其支持的数据包陷阱
(packet traps) 注册到 ``devlink``，并将被捕获的数据包上报给 ``devlink``
作进一步分析。)

Upon receiving trapped packets, ``devlink`` will perform a per-trap packets and
bytes accounting and potentially report the packet to user space via a netlink
event along with all the provided metadata (e.g., trap reason, timestamp, input
port). This is especially useful for drop traps (see :ref:`Trap-Types`)
as it allows users to obtain further visibility into packet drops that would
otherwise be invisible.
(收到被捕获的数据包后，``devlink`` 会按陷阱对数据包数和字节数进行统计，
并可能通过 netlink 事件将该数据包连同提供的全部元数据(例如陷阱原因、
时间戳、输入端口)一并上报给用户空间。这对丢弃型陷阱 (drop traps)
(参见 :ref:`Trap-Types`) 尤其有用，因为它让用户能够进一步洞察原本不可见
的数据包丢弃情况。)

The following diagram provides a general overview of ``devlink-trap``::

                                    Netlink event: Packet w/ metadata
                                                   Or a summary of recent drops
                                  ^
                                  |
         Userspace                |
        +---------------------------------------------------+
         Kernel                   |
                                  |
                          +-------+--------+
                          |                |
                          |  drop_monitor  |
                          |                |
                          +-------^--------+
                                  |
                                  | Non-control traps
                                  |
                             +----+----+
                             |         |      Kernel's Rx path
                             | devlink |      (non-drop traps)
                             |         |
                             +----^----+      ^
                                  |           |
                                  +-----------+
                                  |
                          +-------+-------+
                          |               |
                          | Device driver |
                          |               |
                          +-------^-------+
         Kernel                   |
        +---------------------------------------------------+
         Hardware                 |
                                  | Trapped packet
                                  |
                               +--+---+
                               |      |
                               | ASIC |
                               |      |
                               (下图给出了 ``devlink-trap`` 的总体概览：)
                               +------+

.. _Trap-Types:

Trap Types(陷阱类型)
====================

The ``devlink-trap`` mechanism supports the following packet trap types:
(``devlink-trap`` 机制支持以下数据包陷阱类型：)

  * ``drop``: Trapped packets were dropped by the underlying device. Packets
    are only processed by ``devlink`` and not injected to the kernel's Rx path.
    The trap action (see :ref:`Trap-Actions`) can be changed.
    (``drop``：被捕获的数据包已被底层设备丢弃。数据包仅由 ``devlink``
    处理，不会被注入内核的接收路径。其陷阱动作 (trap action)
    (参见 :ref:`Trap-Actions`) 可以更改。)
  * ``exception``: Trapped packets were not forwarded as intended by the
    underlying device due to an exception (e.g., TTL error, missing neighbour
    entry) and trapped to the control plane for resolution. Packets are
    processed by ``devlink`` and injected to the kernel's Rx path. Changing the
    action of such traps is not allowed, as it can easily break the control
    plane.
    (``exception``：由于异常(例如 TTL 错误、缺少邻居表项)，被捕获的数据包
    未能按底层设备的预期进行转发，而被送往控制平面以求解决。数据包由
    ``devlink`` 处理，并注入内核的接收路径。不允许更改此类陷阱的动作，
    因为这很容易破坏控制平面。)
  * ``control``: Trapped packets were trapped by the device because these are
    control packets required for the correct functioning of the control plane.
    For example, ARP request and IGMP query packets. Packets are injected to
    the kernel's Rx path, but not reported to the kernel's drop monitor.
    Changing the action of such traps is not allowed, as it can easily break
    the control plane.
    (``control``：设备捕获这些数据包，是因为它们是控制平面正常运转所需的
    控制数据包，例如 ARP 请求和 IGMP 查询数据包。数据包会注入内核的接收
    路径，但不会上报给内核的 drop monitor。不允许更改此类陷阱的动作，
    因为这很容易破坏控制平面。)

.. _Trap-Actions:

Trap Actions(陷阱动作)
======================

The ``devlink-trap`` mechanism supports the following packet trap actions:
(``devlink-trap`` 机制支持以下数据包陷阱动作：)

  * ``trap``: The sole copy of the packet is sent to the CPU.
    (``trap``：数据包的唯一一份副本被送往 CPU。)
  * ``drop``: The packet is dropped by the underlying device and a copy is not
    sent to the CPU.
    (``drop``：数据包被底层设备丢弃，不会有副本送往 CPU。)
  * ``mirror``: The packet is forwarded by the underlying device and a copy is
    sent to the CPU.
    (``mirror``：数据包由底层设备转发，同时一份副本被送往 CPU。)

Generic Packet Traps(通用数据包陷阱)
====================================

Generic packet traps are used to describe traps that trap well-defined packets
or packets that are trapped due to well-defined conditions (e.g., TTL error).
Such traps can be shared by multiple device drivers and their description must
be added to the following table:
(通用数据包陷阱用于描述捕获明确定义的数据包、或因明确定义的条件
(例如 TTL 错误)而捕获数据包的陷阱。这类陷阱可由多个设备驱动共享，其
描述必须添加到下表中：)

.. list-table:: List of Generic Packet Traps
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``source_mac_is_multicast``
     - ``drop``
     - Traps incoming packets that the device decided to drop because of a
       multicast source MAC
       (捕获设备因组播源 MAC 而决定丢弃的入站数据包)
   * - ``vlan_tag_mismatch``
     - ``drop``
     - Traps incoming packets that the device decided to drop in case of VLAN
       tag mismatch: The ingress bridge port is not configured with a PVID and
       the packet is untagged or prio-tagged
       (捕获设备在 VLAN 标签不匹配时决定丢弃的入站数据包：入站网桥端口
       未配置 PVID，且数据包未打标签或仅带优先级标签)
   * - ``ingress_vlan_filter``
     - ``drop``
     - Traps incoming packets that the device decided to drop in case they are
       tagged with a VLAN that is not configured on the ingress bridge port
       (捕获设备在数据包所带 VLAN 未在入站网桥端口上配置时决定丢弃的
       入站数据包)
   * - ``ingress_spanning_tree_filter``
     - ``drop``
     - Traps incoming packets that the device decided to drop in case the STP
       state of the ingress bridge port is not "forwarding"
       (捕获设备在入站网桥端口的 STP 状态不是 "forwarding"(转发)时决定
       丢弃的入站数据包)
   * - ``port_list_is_empty``
     - ``drop``
     - Traps packets that the device decided to drop in case they need to be
       flooded (e.g., unknown unicast, unregistered multicast) and there are
       no ports the packets should be flooded to
       (捕获设备在数据包需要泛洪(例如未知单播、未注册组播)而却无端口
       可供泛洪时决定丢弃的数据包)
   * - ``port_loopback_filter``
     - ``drop``
     - Traps packets that the device decided to drop in case after layer 2
       forwarding the only port from which they should be transmitted through
       is the port from which they were received
       (捕获设备在第 2 层转发后、若数据包唯一应从中发出的端口恰是其
       接收端口时决定丢弃的数据包)
   * - ``blackhole_route``
     - ``drop``
     - Traps packets that the device decided to drop in case they hit a
       blackhole route
       (捕获设备在数据包命中黑洞路由 (blackhole route) 时决定丢弃的
       数据包)
   * - ``ttl_value_is_too_small``
     - ``exception``
     - Traps unicast packets that should be forwarded by the device whose TTL
       was decremented to 0 or less
       (捕获应由设备转发、但 TTL 已被递减至 0 或更小的单播数据包)
   * - ``tail_drop``
     - ``drop``
     - Traps packets that the device decided to drop because they could not be
       enqueued to a transmission queue which is full
       (捕获设备因传输队列已满而无法入队、故决定丢弃的数据包)
   * - ``non_ip``
     - ``drop``
     - Traps packets that the device decided to drop because they need to
       undergo a layer 3 lookup, but are not IP or MPLS packets
       (捕获设备因数据包需要进行第 3 层查找却既非 IP 也非 MPLS 数据包而
       决定丢弃的数据包)
   * - ``uc_dip_over_mc_dmac``
     - ``drop``
     - Traps packets that the device decided to drop because they need to be
       routed and they have a unicast destination IP and a multicast destination
       MAC
       (捕获设备因数据包需要路由、且其目的 IP 为单播而目的 MAC 为组播而
       决定丢弃的数据包)
   * - ``dip_is_loopback_address``
     - ``drop``
     - Traps packets that the device decided to drop because they need to be
       routed and their destination IP is the loopback address (i.e., 127.0.0.0/8
       and ::1/128)
       (捕获设备因数据包需要路由、且其目的 IP 为环回地址(即 127.0.0.0/8
       和 ::1/128)而决定丢弃的数据包)
   * - ``sip_is_mc``
     - ``drop``
     - Traps packets that the device decided to drop because they need to be
       routed and their source IP is multicast (i.e., 224.0.0.0/8 and ff::/8)
       (捕获设备因数据包需要路由、且其源 IP 为组播地址(即 224.0.0.0/8 和
       ff::/8)而决定丢弃的数据包)
   * - ``sip_is_loopback_address``
     - ``drop``
     - Traps packets that the device decided to drop because they need to be
       routed and their source IP is the loopback address (i.e., 127.0.0.0/8 and ::1/128)
       (捕获设备因数据包需要路由、且其源 IP 为环回地址(即 127.0.0.0/8 和
       ::1/128)而决定丢弃的数据包)
   * - ``ip_header_corrupted``
     - ``drop``
     - Traps packets that the device decided to drop because they need to be
       routed and their IP header is corrupted: wrong checksum, wrong IP version
       or too short Internet Header Length (IHL)
       (捕获设备因数据包需要路由、且其 IP 头已损坏(校验和错误、IP 版本
       错误或网际报头长度 (IHL) 过短)而决定丢弃的数据包)
   * - ``ipv4_sip_is_limited_bc``
     - ``drop``
     - Traps packets that the device decided to drop because they need to be
       routed and their source IP is limited broadcast (i.e., 255.255.255.255/32)
       (捕获设备因数据包需要路由、且其源 IP 为受限广播地址
       (即 255.255.255.255/32)而决定丢弃的数据包)
   * - ``ipv6_mc_dip_reserved_scope``
     - ``drop``
     - Traps IPv6 packets that the device decided to drop because they need to
       be routed and their IPv6 multicast destination IP has a reserved scope
       (i.e., ffx0::/16)
       (捕获设备因 IPv6 数据包需要路由、且其 IPv6 组播目的 IP 的作用域为
       保留作用域(即 ffx0::/16)而决定丢弃的数据包)
   * - ``ipv6_mc_dip_interface_local_scope``
     - ``drop``
     - Traps IPv6 packets that the device decided to drop because they need to
       be routed and their IPv6 multicast destination IP has an interface-local scope
       (i.e., ffx1::/16)
       (捕获设备因 IPv6 数据包需要路由、且其 IPv6 组播目的 IP 的作用域为
       接口本地作用域(即 ffx1::/16)而决定丢弃的数据包)
   * - ``mtu_value_is_too_small``
     - ``exception``
     - Traps packets that should have been routed by the device, but were bigger
       than the MTU of the egress interface
       (捕获本应由设备路由、但大小超过出站接口 MTU 的数据包)
   * - ``unresolved_neigh``
     - ``exception``
     - Traps packets that did not have a matching IP neighbour after routing
       (捕获路由后没有匹配 IP 邻居的数据包)
   * - ``mc_reverse_path_forwarding``
     - ``exception``
     - Traps multicast IP packets that failed reverse-path forwarding (RPF)
       check during multicast routing
       (捕获在组播路由期间未通过反向路径转发 (RPF) 检查的组播 IP 数据包)
   * - ``reject_route``
     - ``exception``
     - Traps packets that hit reject routes (i.e., "unreachable", "prohibit")
       (捕获命中拒绝路由 (reject route)(即 "unreachable"、"prohibit")
       的数据包)
   * - ``ipv4_lpm_miss``
     - ``exception``
     - Traps unicast IPv4 packets that did not match any route
       (捕获未匹配任何路由的 IPv4 单播数据包)
   * - ``ipv6_lpm_miss``
     - ``exception``
     - Traps unicast IPv6 packets that did not match any route
       (捕获未匹配任何路由的 IPv6 单播数据包)
   * - ``non_routable_packet``
     - ``drop``
     - Traps packets that the device decided to drop because they are not
       supposed to be routed. For example, IGMP queries can be flooded by the
       device in layer 2 and reach the router. Such packets should not be
       routed and instead dropped
       (捕获设备因其本不应被路由而决定丢弃的数据包。例如，IGMP 查询可由
       设备在第 2 层泛洪并到达路由器。这类数据包不应被路由，而应被丢弃)
   * - ``decap_error``
     - ``exception``
     - Traps NVE and IPinIP packets that the device decided to drop because of
       failure during decapsulation (e.g., packet being too short, reserved
       bits set in VXLAN header)
       (捕获设备因解封装过程中出现失败(例如数据包过短、VXLAN 头中保留位
       被置位)而决定丢弃的 NVE 与 IPinIP 数据包)
   * - ``overlay_smac_is_mc``
     - ``drop``
     - Traps NVE packets that the device decided to drop because their overlay
       source MAC is multicast
       (捕获设备因其叠加网 (overlay) 源 MAC 为组播而决定丢弃的 NVE
       数据包)
   * - ``ingress_flow_action_drop``
     - ``drop``
     - Traps packets dropped during processing of ingress flow action drop
       (捕获在处理入站 flow action drop 期间被丢弃的数据包)
   * - ``egress_flow_action_drop``
     - ``drop``
     - Traps packets dropped during processing of egress flow action drop
       (捕获在处理出站 flow action drop 期间被丢弃的数据包)
   * - ``stp``
     - ``control``
     - Traps STP packets
       (捕获 STP 数据包)
   * - ``lacp``
     - ``control``
     - Traps LACP packets
       (捕获 LACP 数据包)
   * - ``lldp``
     - ``control``
     - Traps LLDP packets
       (捕获 LLDP 数据包)
   * - ``igmp_query``
     - ``control``
     - Traps IGMP Membership Query packets
       (捕获 IGMP 成员关系查询 (Membership Query) 数据包)
   * - ``igmp_v1_report``
     - ``control``
     - Traps IGMP Version 1 Membership Report packets
       (捕获 IGMP 版本 1 成员关系报告数据包)
   * - ``igmp_v2_report``
     - ``control``
     - Traps IGMP Version 2 Membership Report packets
       (捕获 IGMP 版本 2 成员关系报告数据包)
   * - ``igmp_v3_report``
     - ``control``
     - Traps IGMP Version 3 Membership Report packets
       (捕获 IGMP 版本 3 成员关系报告数据包)
   * - ``igmp_v2_leave``
     - ``control``
     - Traps IGMP Version 2 Leave Group packets
       (捕获 IGMP 版本 2 离开组 (Leave Group) 数据包)
   * - ``mld_query``
     - ``control``
     - Traps MLD Multicast Listener Query packets
       (捕获 MLD 组播侦听查询 (Multicast Listener Query) 数据包)
   * - ``mld_v1_report``
     - ``control``
     - Traps MLD Version 1 Multicast Listener Report packets
       (捕获 MLD 版本 1 组播侦听报告数据包)
   * - ``mld_v2_report``
     - ``control``
     - Traps MLD Version 2 Multicast Listener Report packets
       (捕获 MLD 版本 2 组播侦听报告数据包)
   * - ``mld_v1_done``
     - ``control``
     - Traps MLD Version 1 Multicast Listener Done packets
       (捕获 MLD 版本 1 组播侦听完成 (Done) 数据包)
   * - ``ipv4_dhcp``
     - ``control``
     - Traps IPv4 DHCP packets
       (捕获 IPv4 DHCP 数据包)
   * - ``ipv6_dhcp``
     - ``control``
     - Traps IPv6 DHCP packets
       (捕获 IPv6 DHCP 数据包)
   * - ``arp_request``
     - ``control``
     - Traps ARP request packets
       (捕获 ARP 请求数据包)
   * - ``arp_response``
     - ``control``
     - Traps ARP response packets
       (捕获 ARP 响应数据包)
   * - ``arp_overlay``
     - ``control``
     - Traps NVE-decapsulated ARP packets that reached the overlay network.
       This is required, for example, when the address that needs to be
       resolved is a local address
       (捕获经 NVE 解封装后到达叠加网络的 ARP 数据包。例如，当需要解析的
       地址是本地地址时就需要此陷阱)
   * - ``ipv6_neigh_solicit``
     - ``control``
     - Traps IPv6 Neighbour Solicitation packets
       (捕获 IPv6 邻居请求 (Neighbour Solicitation) 数据包)
   * - ``ipv6_neigh_advert``
     - ``control``
     - Traps IPv6 Neighbour Advertisement packets
       (捕获 IPv6 邻居通告 (Neighbour Advertisement) 数据包)
   * - ``ipv4_bfd``
     - ``control``
     - Traps IPv4 BFD packets
       (捕获 IPv4 BFD 数据包)
   * - ``ipv6_bfd``
     - ``control``
     - Traps IPv6 BFD packets
       (捕获 IPv6 BFD 数据包)
   * - ``ipv4_ospf``
     - ``control``
     - Traps IPv4 OSPF packets
       (捕获 IPv4 OSPF 数据包)
   * - ``ipv6_ospf``
     - ``control``
     - Traps IPv6 OSPF packets
       (捕获 IPv6 OSPF 数据包)
   * - ``ipv4_bgp``
     - ``control``
     - Traps IPv4 BGP packets
       (捕获 IPv4 BGP 数据包)
   * - ``ipv6_bgp``
     - ``control``
     - Traps IPv6 BGP packets
       (捕获 IPv6 BGP 数据包)
   * - ``ipv4_vrrp``
     - ``control``
     - Traps IPv4 VRRP packets
       (捕获 IPv4 VRRP 数据包)
   * - ``ipv6_vrrp``
     - ``control``
     - Traps IPv6 VRRP packets
       (捕获 IPv6 VRRP 数据包)
   * - ``ipv4_pim``
     - ``control``
     - Traps IPv4 PIM packets
       (捕获 IPv4 PIM 数据包)
   * - ``ipv6_pim``
     - ``control``
     - Traps IPv6 PIM packets
       (捕获 IPv6 PIM 数据包)
   * - ``uc_loopback``
     - ``control``
     - Traps unicast packets that need to be routed through the same layer 3
       interface from which they were received. Such packets are routed by the
       kernel, but also cause it to potentially generate ICMP redirect packets
       (捕获需要通过其接收时所用的同一第 3 层接口进行路由的单播数据包。
       这类数据包由内核路由，但也可能促使内核生成 ICMP 重定向数据包)
   * - ``local_route``
     - ``control``
     - Traps unicast packets that hit a local route and need to be locally
       delivered
       (捕获命中本地路由 (local route)、需要本地交付的单播数据包)
   * - ``external_route``
     - ``control``
     - Traps packets that should be routed through an external interface (e.g.,
       management interface) that does not belong to the same device (e.g.,
       switch ASIC) as the ingress interface
       (捕获应当通过与入站接口不属于同一设备(例如交换 ASIC)的外部接口
       (例如管理接口)进行路由的数据包)
   * - ``ipv6_uc_dip_link_local_scope``
     - ``control``
     - Traps unicast IPv6 packets that need to be routed and have a destination
       IP address with a link-local scope (i.e., fe80::/10). The trap allows
       device drivers to avoid programming link-local routes, but still receive
       packets for local delivery
       (捕获需要路由、且目的 IP 地址具有链路本地作用域(即 fe80::/10)的
       单播 IPv6 数据包。该陷阱允许设备驱动不必下发链路本地路由，但仍能
       收到需要本地交付的数据包)
   * - ``ipv6_dip_all_nodes``
     - ``control``
     - Traps IPv6 packets that their destination IP address is the "All Nodes
       Address" (i.e., ff02::1)
       (捕获目的 IP 地址为 "所有节点地址" (All Nodes Address)
       (即 ff02::1)的 IPv6 数据包)
   * - ``ipv6_dip_all_routers``
     - ``control``
     - Traps IPv6 packets that their destination IP address is the "All Routers
       Address" (i.e., ff02::2)
       (捕获目的 IP 地址为 "所有路由器地址" (All Routers Address)
       (即 ff02::2)的 IPv6 数据包)
   * - ``ipv6_router_solicit``
     - ``control``
     - Traps IPv6 Router Solicitation packets
       (捕获 IPv6 路由器请求 (Router Solicitation) 数据包)
   * - ``ipv6_router_advert``
     - ``control``
     - Traps IPv6 Router Advertisement packets
       (捕获 IPv6 路由器通告 (Router Advertisement) 数据包)
   * - ``ipv6_redirect``
     - ``control``
     - Traps IPv6 Redirect Message packets
       (捕获 IPv6 重定向消息 (Redirect Message) 数据包)
   * - ``ipv4_router_alert``
     - ``control``
     - Traps IPv4 packets that need to be routed and include the Router Alert
       option. Such packets need to be locally delivered to raw sockets that
       have the IP_ROUTER_ALERT socket option set
       (捕获需要路由且包含路由器告警 (Router Alert) 选项的 IPv4 数据包。
       这类数据包需要本地交付给设置了 IP_ROUTER_ALERT 套接字选项的原始
       套接字)
   * - ``ipv6_router_alert``
     - ``control``
     - Traps IPv6 packets that need to be routed and include the Router Alert
       option in their Hop-by-Hop extension header. Such packets need to be
       locally delivered to raw sockets that have the IPV6_ROUTER_ALERT socket
       option set
       (捕获需要路由、且在其逐跳 (Hop-by-Hop) 扩展头中包含路由器告警选项
       的 IPv6 数据包。这类数据包需要本地交付给设置了 IPV6_ROUTER_ALERT
       套接字选项的原始套接字)
   * - ``ptp_event``
     - ``control``
     - Traps PTP time-critical event messages (Sync, Delay_req, Pdelay_Req and
       Pdelay_Resp)
       (捕获 PTP 时间关键事件消息(Sync、Delay_req、Pdelay_Req 和
       Pdelay_Resp))
   * - ``ptp_general``
     - ``control``
     - Traps PTP general messages (Announce, Follow_Up, Delay_Resp,
       Pdelay_Resp_Follow_Up, management and signaling)
       (捕获 PTP 通用消息(Announce、Follow_Up、Delay_Resp、
       Pdelay_Resp_Follow_Up、管理消息和信令消息))
   * - ``flow_action_sample``
     - ``control``
     - Traps packets sampled during processing of flow action sample (e.g., via
       tc's sample action)
       (捕获在处理 flow action sample 期间被采样的数据包(例如通过 tc 的
       sample 动作))
   * - ``flow_action_trap``
     - ``control``
     - Traps packets logged during processing of flow action trap (e.g., via
       tc's trap action)
       (捕获在处理 flow action trap 期间被记录的数据包(例如通过 tc 的
       trap 动作))
   * - ``early_drop``
     - ``drop``
     - Traps packets dropped due to the RED (Random Early Detection) algorithm
       (i.e., early drops)
       (捕获因 RED (随机早期检测) 算法而丢弃的数据包(即早期丢弃))
   * - ``vxlan_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the VXLAN header parsing which
       might be because of packet truncation or the I flag is not set.
       (捕获因 VXLAN 头解析出错而丢弃的数据包，原因可能是数据包被截断或
       I 标志未置位。)
   * - ``llc_snap_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the LLC+SNAP header parsing
       (捕获因 LLC+SNAP 头解析出错而丢弃的数据包)
   * - ``vlan_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the VLAN header parsing. Could
       include unexpected packet truncation.
       (捕获因 VLAN 头解析出错而丢弃的数据包。可能包括意外的数据包截断。)
   * - ``pppoe_ppp_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the PPPoE+PPP header parsing.
       This could include finding a session ID of 0xFFFF (which is reserved and
       not for use), a PPPoE length which is larger than the frame received or
       any common error on this type of header
       (捕获因 PPPoE+PPP 头解析出错而丢弃的数据包。这可能包括发现会话 ID
       为 0xFFFF(该值保留、不可使用)、PPPoE 长度大于所收到的帧，或此类
       报头的其他常见错误)
   * - ``mpls_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the MPLS header parsing which
       could include unexpected header truncation
       (捕获因 MPLS 头解析出错而丢弃的数据包，其中可能包括意外的报头
       截断)
   * - ``arp_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the ARP header parsing
       (捕获因 ARP 头解析出错而丢弃的数据包)
   * - ``ip_1_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the first IP header parsing.
       This packet trap could include packets which do not pass an IP checksum
       check, a header length check (a minimum of 20 bytes), which might suffer
       from packet truncation thus the total length field exceeds the received
       packet length etc
       (捕获因第一个 IP 头解析出错而丢弃的数据包。该陷阱可能包括未通过
       IP 校验和检查、报头长度检查(最少 20 字节)的数据包，这些数据包可能
       因截断而导致总长度字段超过所收到的数据包长度等)
   * - ``ip_n_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the parsing of the last IP
       header (the inner one in case of an IP over IP tunnel). The same common
       error checking is performed here as for the ip_1_parsing trap
       (捕获因最后一个 IP 头解析出错而丢弃的数据包(IP over IP 隧道情形下
       即为内层头)。此处执行与 ip_1_parsing 陷阱相同的常见错误检查)
   * - ``gre_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the GRE header parsing
       (捕获因 GRE 头解析出错而丢弃的数据包)
   * - ``udp_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the UDP header parsing.
       This packet trap could include checksum errors, an improper UDP
       length detected (smaller than 8 bytes) or detection of header
       truncation.
       (捕获因 UDP 头解析出错而丢弃的数据包。该陷阱可能包括校验和错误、
       检测到不当的 UDP 长度(小于 8 字节)或检测到报头截断。)
   * - ``tcp_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the TCP header parsing.
       This could include TCP checksum errors, improper combination of SYN, FIN
       and/or RESET etc.
       (捕获因 TCP 头解析出错而丢弃的数据包。这可能包括 TCP 校验和错误、
       SYN、FIN 和/或 RESET 的不当组合等。)
   * - ``ipsec_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the IPSEC header parsing
       (捕获因 IPSEC 头解析出错而丢弃的数据包)
   * - ``sctp_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the SCTP header parsing.
       This would mean that port number 0 was used or that the header is
       truncated.
       (捕获因 SCTP 头解析出错而丢弃的数据包。这意味着使用了端口号 0 或
       报头被截断。)
   * - ``dccp_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the DCCP header parsing
       (捕获因 DCCP 头解析出错而丢弃的数据包)
   * - ``gtp_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the GTP header parsing
       (捕获因 GTP 头解析出错而丢弃的数据包)
   * - ``esp_parsing``
     - ``drop``
     - Traps packets dropped due to an error in the ESP header parsing
       (捕获因 ESP 头解析出错而丢弃的数据包)
   * - ``blackhole_nexthop``
     - ``drop``
     - Traps packets that the device decided to drop in case they hit a
       blackhole nexthop
       (捕获设备在数据包命中黑洞下一跳 (blackhole nexthop) 时决定丢弃的
       数据包)
   * - ``dmac_filter``
     - ``drop``
     - Traps incoming packets that the device decided to drop because
       the destination MAC is not configured in the MAC table and
       the interface is not in promiscuous mode
       (捕获设备因目的 MAC 未在 MAC 表中配置且接口不处于混杂模式而决定
       丢弃的入站数据包)
   * - ``eapol``
     - ``control``
     - Traps "Extensible Authentication Protocol over LAN" (EAPOL) packets
       specified in IEEE 802.1X
       (捕获 IEEE 802.1X 中规定的 "局域网可扩展认证协议" (EAPOL) 数据包)
   * - ``locked_port``
     - ``drop``
     - Traps packets that the device decided to drop because they failed the
       locked bridge port check. That is, packets that were received via a
       locked port and whose {SMAC, VID} does not correspond to an FDB entry
       pointing to the port
       (捕获设备因未通过锁定网桥端口检查而决定丢弃的数据包。即：经由
       锁定端口收到、且其 {SMAC, VID} 不对应指向该端口的 FDB 表项的
       数据包)

Driver-specific Packet Traps(驱动特定的数据包陷阱)
==================================================

Device drivers can register driver-specific packet traps, but these must be
clearly documented. Such traps can correspond to device-specific exceptions and
help debug packet drops caused by these exceptions. The following list includes
links to the description of driver-specific traps registered by various device
drivers:
(设备驱动可以注册驱动特定的数据包陷阱，但这些陷阱必须有清晰的文档。此类
陷阱可以对应设备特定的异常，并有助于调试由这些异常导致的数据包丢弃。
以下列表给出了各设备驱动所注册的驱动特定陷阱的描述链接：)

  * Documentation/networking/devlink/netdevsim.rst
  * Documentation/networking/devlink/mlxsw.rst
  * Documentation/networking/devlink/prestera.rst

.. _Generic-Packet-Trap-Groups:

Generic Packet Trap Groups(通用数据包陷阱组)
============================================

Generic packet trap groups are used to aggregate logically related packet
traps. These groups allow the user to batch operations such as setting the trap
action of all member traps. In addition, ``devlink-trap`` can report aggregated
per-group packets and bytes statistics, in case per-trap statistics are too
narrow. The description of these groups must be added to the following table:
(通用数据包陷阱组用于聚合逻辑上相关的数据包陷阱。这些组允许用户批量执行
操作，例如设置所有成员陷阱的动作。此外，在按陷阱统计的粒度太细的情况下，
``devlink-trap`` 还可以上报按组聚合的数据包数与字节数统计。这些组的描述
必须添加到下表中：)

.. list-table:: List of Generic Packet Trap Groups
   :widths: 10 90

   * - Name (名称)
     - Description (描述)
   * - ``l2_drops``
     - Contains packet traps for packets that were dropped by the device during
       layer 2 forwarding (i.e., bridge)
       (包含设备在第 2 层转发(即网桥)期间丢弃的数据包的数据包陷阱)
   * - ``l3_drops``
     - Contains packet traps for packets that were dropped by the device during
       layer 3 forwarding
       (包含设备在第 3 层转发期间丢弃的数据包的数据包陷阱)
   * - ``l3_exceptions``
     - Contains packet traps for packets that hit an exception (e.g., TTL
       error) during layer 3 forwarding
       (包含在第 3 层转发期间遭遇异常(例如 TTL 错误)的数据包的数据包
       陷阱)
   * - ``buffer_drops``
     - Contains packet traps for packets that were dropped by the device due to
       an enqueue decision
       (包含设备因入队决策而丢弃的数据包的数据包陷阱)
   * - ``tunnel_drops``
     - Contains packet traps for packets that were dropped by the device during
       tunnel encapsulation / decapsulation
       (包含设备在隧道封装/解封装期间丢弃的数据包的数据包陷阱)
   * - ``acl_drops``
     - Contains packet traps for packets that were dropped by the device during
       ACL processing
       (包含设备在 ACL 处理期间丢弃的数据包的数据包陷阱)
   * - ``stp``
     - Contains packet traps for STP packets
       (包含 STP 数据包的数据包陷阱)
   * - ``lacp``
     - Contains packet traps for LACP packets
       (包含 LACP 数据包的数据包陷阱)
   * - ``lldp``
     - Contains packet traps for LLDP packets
       (包含 LLDP 数据包的数据包陷阱)
   * - ``mc_snooping``
     - Contains packet traps for IGMP and MLD packets required for multicast
       snooping
       (包含组播侦听 (multicast snooping) 所需的 IGMP 与 MLD 数据包的
       数据包陷阱)
   * - ``dhcp``
     - Contains packet traps for DHCP packets
       (包含 DHCP 数据包的数据包陷阱)
   * - ``neigh_discovery``
     - Contains packet traps for neighbour discovery packets (e.g., ARP, IPv6
       ND)
       (包含邻居发现数据包(例如 ARP、IPv6 ND)的数据包陷阱)
   * - ``bfd``
     - Contains packet traps for BFD packets
       (包含 BFD 数据包的数据包陷阱)
   * - ``ospf``
     - Contains packet traps for OSPF packets
       (包含 OSPF 数据包的数据包陷阱)
   * - ``bgp``
     - Contains packet traps for BGP packets
       (包含 BGP 数据包的数据包陷阱)
   * - ``vrrp``
     - Contains packet traps for VRRP packets
       (包含 VRRP 数据包的数据包陷阱)
   * - ``pim``
     - Contains packet traps for PIM packets
       (包含 PIM 数据包的数据包陷阱)
   * - ``uc_loopback``
     - Contains a packet trap for unicast loopback packets (i.e.,
       ``uc_loopback``). This trap is singled-out because in cases such as
       one-armed router it will be constantly triggered. To limit the impact on
       the CPU usage, a packet trap policer with a low rate can be bound to the
       group without affecting other traps
       (包含针对单播环回数据包(即 ``uc_loopback``)的数据包陷阱。将此
       陷阱单独列出，是因为在单臂路由器等场景下它会被持续触发。为了限制
       对 CPU 占用的影响，可以将一个低速率的数据包陷阱策略器绑定到该组，
       而不影响其他陷阱)
   * - ``local_delivery``
     - Contains packet traps for packets that should be locally delivered after
       routing, but do not match more specific packet traps (e.g.,
       ``ipv4_bgp``)
       (包含应在路由后本地交付、但不匹配更具体的数据包陷阱(例如
       ``ipv4_bgp``)的数据包的数据包陷阱)
   * - ``external_delivery``
     - Contains packet traps for packets that should be routed through an
       external interface (e.g., management interface) that does not belong to
       the same device (e.g., switch ASIC) as the ingress interface
       (包含应当通过与入站接口不属于同一设备(例如交换 ASIC)的外部接口
       (例如管理接口)进行路由的数据包的数据包陷阱)
   * - ``ipv6``
     - Contains packet traps for various IPv6 control packets (e.g., Router
       Advertisements)
       (包含各种 IPv6 控制数据包(例如路由器通告)的数据包陷阱)
   * - ``ptp_event``
     - Contains packet traps for PTP time-critical event messages (Sync,
       Delay_req, Pdelay_Req and Pdelay_Resp)
       (包含 PTP 时间关键事件消息(Sync、Delay_req、Pdelay_Req 和
       Pdelay_Resp)的数据包陷阱)
   * - ``ptp_general``
     - Contains packet traps for PTP general messages (Announce, Follow_Up,
       Delay_Resp, Pdelay_Resp_Follow_Up, management and signaling)
       (包含 PTP 通用消息(Announce、Follow_Up、Delay_Resp、
       Pdelay_Resp_Follow_Up、管理消息和信令消息)的数据包陷阱)
   * - ``acl_sample``
     - Contains packet traps for packets that were sampled by the device during
       ACL processing
       (包含设备在 ACL 处理期间采样的数据包的数据包陷阱)
   * - ``acl_trap``
     - Contains packet traps for packets that were trapped (logged) by the
       device during ACL processing
       (包含设备在 ACL 处理期间捕获(记录)的数据包的数据包陷阱)
   * - ``parser_error_drops``
     - Contains packet traps for packets that were marked by the device during
       parsing as erroneous
       (包含设备在解析期间标记为出错的数据包的数据包陷阱)
   * - ``eapol``
     - Contains packet traps for "Extensible Authentication Protocol over LAN"
       (EAPOL) packets specified in IEEE 802.1X
       (包含 IEEE 802.1X 中规定的 "局域网可扩展认证协议" (EAPOL) 数据包
       的数据包陷阱)

Packet Trap Policers(数据包陷阱策略器)
======================================

As previously explained, the underlying device can trap certain packets to the
CPU for processing. In most cases, the underlying device is capable of handling
packet rates that are several orders of magnitude higher compared to those that
can be handled by the CPU.
(如前所述，底层设备可以将某些数据包送往 CPU 处理。在大多数情况下，底层
设备能够处理的数据包速率比 CPU 所能处理的速率高出几个数量级。)

Therefore, in order to prevent the underlying device from overwhelming the CPU,
devices usually include packet trap policers that are able to police the
trapped packets to rates that can be handled by the CPU.
(因此，为防止底层设备压垮 CPU，设备通常包含数据包陷阱策略器
(policer)，能够将被捕获的数据包限制到 CPU 可以处理的速率。)

The ``devlink-trap`` mechanism allows capable device drivers to register their
supported packet trap policers with ``devlink``. The device driver can choose
to associate these policers with supported packet trap groups (see
:ref:`Generic-Packet-Trap-Groups`) during its initialization, thereby exposing
its default control plane policy to user space.
(``devlink-trap`` 机制允许具备能力的设备驱动将其支持的数据包陷阱策略器
注册到 ``devlink``。设备驱动可以选择在初始化期间将这些策略器与受支持的
数据包陷阱组(参见 :ref:`Generic-Packet-Trap-Groups`)关联起来，从而向
用户空间暴露其默认的控制平面策略。)

Device drivers should allow user space to change the parameters of the policers
(e.g., rate, burst size) as well as the association between the policers and
trap groups by implementing the relevant callbacks.
(设备驱动应当通过实现相关回调，允许用户空间更改策略器的参数(例如速率、
突发大小)以及策略器与陷阱组之间的关联。)

If possible, device drivers should implement a callback that allows user space
to retrieve the number of packets that were dropped by the policer because its
configured policy was violated.
(如果可能，设备驱动应当实现一个回调，允许用户空间获取因违反策略器所配置
的策略而被其丢弃的数据包数量。)

Testing(测试)
=============

See ``tools/testing/selftests/drivers/net/netdevsim/devlink_trap.sh`` for a
test covering the core infrastructure. Test cases should be added for any new
functionality.
(覆盖核心基础设施的测试见
``tools/testing/selftests/drivers/net/netdevsim/devlink_trap.sh``。任何
新功能都应添加相应的测试用例。)

Device drivers should focus their tests on device-specific functionality, such
as the triggering of supported packet traps.
(设备驱动应将其测试集中在设备特定的功能上，例如所支持的数据包陷阱的
触发。)
