.. SPDX-License-Identifier: GPL-2.0

=================
Ethernet Bridging
=================

(以太网桥接)

Introduction
(简介)
============

The IEEE 802.1Q-2022 (Bridges and Bridged Networks) standard defines the
operation of bridges in computer networks. A bridge, in the context of this
standard, is a device that connects two or more network segments and operates
at the data link layer (Layer 2) of the OSI (Open Systems Interconnection)
model. The purpose of a bridge is to filter and forward frames between
different segments based on the destination MAC (Media Access Control) address.
(IEEE 802.1Q-2022 (网桥与桥接网络) 标准定义了计算机网络中网桥的工作方式。在该标准的语境中，网桥是连接两个或更多网段、工作在 OSI (开放系统互连) 模型数据链路层 (第 2 层) 的设备。网桥的目的是根据目的 MAC (媒体访问控制) 地址在不同网段之间过滤和转发帧。)

Bridge kAPI
(网桥内核态 API (kAPI))
===========

Here are some core structures of bridge code. Note that the kAPI is *unstable*,
and can be changed at any time.
(下面是网桥代码的一些核心结构。注意，内核态 API (kAPI) 是 *不稳定* 的，可能随时变更。)

.. kernel-doc:: net/bridge/br_private.h
   :identifiers: net_bridge_vlan

Bridge uAPI
(网桥用户态 API (uAPI))
===========

Modern Linux bridge uAPI is accessed via Netlink interface. You can find
below files where the bridge and bridge port netlink attributes are defined.
(现代 Linux 网桥的用户态 API (uAPI) 通过 Netlink 接口访问。你可以在下面列出的文件中找到网桥和网桥端口 netlink 属性的定义。)

Bridge netlink attributes
(网桥 netlink 属性)
-------------------------

.. kernel-doc:: include/uapi/linux/if_link.h
   :doc: Bridge enum definition

Bridge port netlink attributes
(网桥端口 netlink 属性)
------------------------------

.. kernel-doc:: include/uapi/linux/if_link.h
   :doc: Bridge port enum definition

Bridge sysfs
(网桥 sysfs)
------------

The sysfs interface is deprecated and should not be extended if new
options are added.
(sysfs 接口已被弃用，不应在添加新选项时对其进行扩展。)

STP
(生成树协议 (STP))
===

The STP (Spanning Tree Protocol) implementation in the Linux bridge driver
is a critical feature that helps prevent loops and broadcast storms in
Ethernet networks by identifying and disabling redundant links. In a Linux
bridge context, STP is crucial for network stability and availability.
(STP (生成树协议) 在 Linux 网桥驱动程序中的实现是一项关键特性，它通过识别并禁用冗余链路来帮助防止以太网网络中的环路和广播风暴。在 Linux 网桥环境中，STP 对网络的稳定性和可用性至关重要。)

STP is a Layer 2 protocol that operates at the Data Link Layer of the OSI
model. It was originally developed as IEEE 802.1D and has since evolved into
multiple versions, including Rapid Spanning Tree Protocol (RSTP) and
`Multiple Spanning Tree Protocol (MSTP)
<https://lore.kernel.org/netdev/20220316150857.2442916-1-tobias@waldekranz.com/>`_.
(STP 是一种工作在 OSI 模型数据链路层的第 2 层协议。它最初是作为 IEEE 802.1D 开发的，此后演化出多个版本，包括快速生成树协议 (RSTP) 和多生成树协议 (MSTP)。)

The 802.1D-2004 removed the original Spanning Tree Protocol, instead
incorporating the Rapid Spanning Tree Protocol (RSTP). By 2014, all the
functionality defined by IEEE 802.1D has been incorporated into either
IEEE 802.1Q (Bridges and Bridged Networks) or IEEE 802.1AC (MAC Service
Definition). 802.1D has been officially withdrawn in 2022.
(802.1D-2004 移除了最初的生成树协议，转而纳入快速生成树协议 (RSTP)。到 2014 年，IEEE 802.1D 定义的所有功能都已被并入 IEEE 802.1Q (网桥与桥接网络) 或 IEEE 802.1AC (MAC 服务定义)。802.1D 已于 2022 年被正式撤销。)

Bridge Ports and STP States
(网桥端口与 STP 状态)
---------------------------

In the context of STP, bridge ports can be in one of the following states:
(在 STP 的语境中，网桥端口可以处于以下几种状态之一：)

  * Blocking: The port is disabled for data traffic and only listens for
    BPDUs (Bridge Protocol Data Units) from other devices to determine the
    network topology.
    (Blocking (阻塞)：端口对数据流量禁用，只监听来自其他设备的 BPDU (桥接协议数据单元) 以确定网络拓扑。)
  * Listening: The port begins to participate in the STP process and listens
    for BPDUs.
    (Listening (侦听)：端口开始参与 STP 过程并监听 BPDU。)
  * Learning: The port continues to listen for BPDUs and begins to learn MAC
    addresses from incoming frames but does not forward data frames.
    (Learning (学习)：端口继续监听 BPDU，并开始从收到的帧中学习 MAC 地址，但不转发数据帧。)
  * Forwarding: The port is fully operational and forwards both BPDUs and
    data frames.
    (Forwarding (转发)：端口完全正常工作，同时转发 BPDU 和数据帧。)
  * Disabled: The port is administratively disabled and does not participate
    in the STP process. The data frames forwarding are also disabled.
    (Disabled (禁用)：端口被管理性禁用，不参与 STP 过程。数据帧转发也被禁用。)

Root Bridge and Convergence
(根网桥与收敛)
---------------------------

In the context of networking and Ethernet bridging in Linux, the root bridge
is a designated switch in a bridged network that serves as a reference point
for the spanning tree algorithm to create a loop-free topology.
(在 Linux 网络与以太网桥接的语境中，根网桥是桥接网络中被指定的一台交换机，作为生成树算法创建无环拓扑的参考点。)

Here's how the STP works and root bridge is chosen:
(下面介绍 STP 的工作方式以及根网桥的选举过程：)

  1. Bridge Priority: Each bridge running a spanning tree protocol, has a
     configurable Bridge Priority value. The lower the value, the higher the
     priority. By default, the Bridge Priority is set to a standard value
     (e.g., 32768).
     (网桥优先级 (Bridge Priority)：每个运行生成树协议的网桥都有一个可配置的网桥优先级值。值越低，优先级越高。默认情况下，网桥优先级被设为标准值 (例如 32768)。)
  2. Bridge ID: The Bridge ID is composed of two components: Bridge Priority
     and the MAC address of the bridge. It uniquely identifies each bridge
     in the network. The Bridge ID is used to compare the priorities of
     different bridges.
     (网桥 ID (Bridge ID)：网桥 ID 由两部分组成：网桥优先级和网桥的 MAC 地址。它唯一地标识网络中的每个网桥。网桥 ID 用于比较不同网桥的优先级。)
  3. Bridge Election: When the network starts, all bridges initially assume
     that they are the root bridge. They start advertising Bridge Protocol
     Data Units (BPDU) to their neighbors, containing their Bridge ID and
     other information.
     (网桥选举 (Bridge Election)：网络启动时，所有网桥最初都假定自己是根网桥。它们开始向邻居通告桥接协议数据单元 (BPDU)，其中包含自己的网桥 ID 和其他信息。)
  4. BPDU Comparison: Bridges exchange BPDUs to determine the root bridge.
     Each bridge examines the received BPDUs, including the Bridge Priority
     and Bridge ID, to determine if it should adjust its own priorities.
     The bridge with the lowest Bridge ID will become the root bridge.
     (BPDU 比较 (BPDU Comparison)：网桥之间交换 BPDU 以确定根网桥。每个网桥检查收到的 BPDU (包括网桥优先级和网桥 ID)，以确定是否应调整自己的优先级。网桥 ID 最低的网桥将成为根网桥。)
  5. Root Bridge Announcement: Once the root bridge is determined, it sends
     BPDUs with information about the root bridge to all other bridges in the
     network. This information is used by other bridges to calculate the
     shortest path to the root bridge and, in doing so, create a loop-free
     topology.
     (根网桥通告 (Root Bridge Announcement)：一旦确定了根网桥，它就会向网络中所有其他网桥发送包含根网桥信息的 BPDU。其他网桥利用这些信息计算到根网桥的最短路径，从而创建无环拓扑。)
  6. Forwarding Ports: After the root bridge is selected and the spanning tree
     topology is established, each bridge determines which of its ports should
     be in the forwarding state (used for data traffic) and which should be in
     the blocking state (used to prevent loops). The root bridge's ports are
     all in the forwarding state. while other bridges have some ports in the
     blocking state to avoid loops.
     (转发端口 (Forwarding Ports)：在选出根网桥并建立生成树拓扑之后，每个网桥会确定自己的哪些端口应处于转发状态 (用于数据流量)、哪些应处于阻塞状态 (用于防止环路)。根网桥的所有端口都处于转发状态，而其他网桥则有一些端口处于阻塞状态以避免环路。)
  7. Root Ports: After the root bridge is selected and the spanning tree
     topology is established, each non-root bridge processes incoming
     BPDUs and determines which of its ports provides the shortest path to the
     root bridge based on the information in the received BPDUs. This port is
     designated as the root port. And it is in the Forwarding state, allowing
     it to actively forward network traffic.
     (根端口 (Root Ports)：在选出根网桥并建立生成树拓扑之后，每个非根网桥会处理收到的 BPDU，并根据收到的 BPDU 中的信息确定自己的哪个端口提供了到根网桥的最短路径。该端口被指定为根端口，它处于转发状态，可以主动转发网络流量。)
  8. Designated ports: A designated port is the port through which the non-root
     bridge will forward traffic towards the designated segment. Designated ports
     are placed in the Forwarding state. All other ports on the non-root
     bridge that are not designated for specific segments are placed in the
     Blocking state to prevent network loops.
     (指定端口 (Designated ports)：指定端口是非根网桥向指定网段转发流量的端口。指定端口处于转发状态。非根网桥上未指定给特定网段的所有其他端口都置于阻塞状态，以防止网络环路。)

STP ensures network convergence by calculating the shortest path and disabling
redundant links. When network topology changes occur (e.g., a link failure),
STP recalculates the network topology to restore connectivity while avoiding loops.
(STP 通过计算最短路径并禁用冗余链路来确保网络收敛。当网络拓扑发生变化时 (例如链路故障)，STP 会重新计算网络拓扑，在避免环路的同时恢复连通性。)

Proper configuration of STP parameters, such as the bridge priority, can
influence network performance, path selection and which bridge becomes the
Root Bridge.
(正确配置 STP 参数 (例如网桥优先级) 可以影响网络性能、路径选择以及哪个网桥成为根网桥。)

User space STP helper
(用户态 STP 辅助程序)
---------------------

The user space STP helper *bridge-stp* is a program to control whether to use
user mode spanning tree. The ``/sbin/bridge-stp <bridge> <start|stop>`` is
called by the kernel when STP is enabled/disabled on a bridge
(via ``brctl stp <bridge> <on|off>`` or ``ip link set <bridge> type bridge
stp_state <0|1>``).  The kernel enables user_stp mode if that command returns
0, or enables kernel_stp mode if that command returns any other value.
(用户态 STP 辅助程序 *bridge-stp* 是一个用于控制是否使用用户态生成树的程序。当在网桥上启用/禁用 STP 时 (通过 ``brctl stp <bridge> <on|off>`` 或 ``ip link set <bridge> type bridge stp_state <0|1>`` )，内核会调用 ``/sbin/bridge-stp <bridge> <start|stop>`` 。如果该命令返回 0，内核启用 user_stp 模式；如果返回任何其他值，则启用 kernel_stp 模式。)

STP mode selection
(STP 模式选择)
------------------

The ``IFLA_BR_STP_MODE`` bridge attribute allows explicit control over how
STP operates when enabled, bypassing the ``/sbin/bridge-stp`` helper
entirely for the ``user`` and ``kernel`` modes.
(``IFLA_BR_STP_MODE`` 网桥属性允许显式控制启用 STP 时它的运行方式，对 ``user`` 和 ``kernel`` 模式完全绕过 ``/sbin/bridge-stp`` 辅助程序。)

.. kernel-doc:: include/uapi/linux/if_link.h
   :doc: Bridge STP mode values

The default mode is ``BR_STP_MODE_AUTO``, which preserves the traditional
behavior of invoking the ``/sbin/bridge-stp`` helper. The ``user`` and
``kernel`` modes are particularly useful in network namespace environments
where the helper mechanism is not available, as ``call_usermodehelper()``
is restricted to the initial network namespace.
(默认模式是 ``BR_STP_MODE_AUTO``，它保留了调用 ``/sbin/bridge-stp`` 辅助程序的传统行为。``user`` 和 ``kernel`` 模式在辅助程序机制不可用的网络命名空间环境中特别有用，因为 ``call_usermodehelper()`` 被限制在初始网络命名空间中。)

Example::
(示例：)

  ip link set dev br0 type bridge stp_mode user stp_state 1

The mode can only be changed while STP is disabled.
(只能在 STP 被禁用时更改模式。)

VLAN
(虚拟局域网 (VLAN))
====

A LAN (Local Area Network) is a network that covers a small geographic area,
typically within a single building or a campus. LANs are used to connect
computers, servers, printers, and other networked devices within a localized
area. LANs can be wired (using Ethernet cables) or wireless (using Wi-Fi).
(局域网 (LAN) 是覆盖较小地理区域的网络，通常局限于单栋建筑或园区内。LAN 用于连接局部区域内的计算机、服务器、打印机和其他网络设备。LAN 可以是有线的 (使用以太网电缆)，也可以是无线的 (使用 Wi-Fi)。)

A VLAN (Virtual Local Area Network) is a logical segmentation of a physical
network into multiple isolated broadcast domains. VLANs are used to divide
a single physical LAN into multiple virtual LANs, allowing different groups of
devices to communicate as if they were on separate physical networks.
(虚拟局域网 (VLAN) 是把物理网络在逻辑上划分为多个隔离广播域的技术。VLAN 用于把单个物理 LAN 划分为多个虚拟 LAN，让不同的设备组之间可以像在独立的物理网络上一样通信。)

Typically there are two VLAN implementations, IEEE 802.1Q and IEEE 802.1ad
(also known as QinQ). IEEE 802.1Q is a standard for VLAN tagging in Ethernet
networks. It allows network administrators to create logical VLANs on a
physical network and tag Ethernet frames with VLAN information, which is
called *VLAN-tagged frames*. IEEE 802.1ad, commonly known as QinQ or Double
VLAN, is an extension of the IEEE 802.1Q standard. QinQ allows for the
stacking of multiple VLAN tags within a single Ethernet frame. The Linux
bridge supports both the IEEE 802.1Q and `802.1AD
<https://lore.kernel.org/netdev/1402401565-15423-1-git-send-email-makita.toshiaki@lab.ntt.co.jp/>`_
protocol for VLAN tagging.
(通常有两种 VLAN 实现：IEEE 802.1Q 和 IEEE 802.1ad (也称为 QinQ)。IEEE 802.1Q 是以太网中 VLAN 标记的标准。它允许网络管理员在物理网络上创建逻辑 VLAN，并给以太网帧打上 VLAN 信息标签，这样的帧被称为 *VLAN 标记帧*。IEEE 802.1ad 通常称为 QinQ 或双 VLAN，是 IEEE 802.1Q 标准的扩展。QinQ 允许在单个以太网帧内堆叠多个 VLAN 标签。Linux 网桥同时支持用于 VLAN 标记的 IEEE 802.1Q 和 802.1AD 协议。)

`VLAN filtering <https://lore.kernel.org/netdev/1360792820-14116-1-git-send-email-vyasevic@redhat.com/>`_
on a bridge is disabled by default. After enabling VLAN filtering on a bridge,
it will start forwarding frames to appropriate destinations based on their
destination MAC address and VLAN tag (both must match).
(网桥上的 VLAN 过滤 (VLAN filtering) 默认是禁用的。在网桥上启用 VLAN 过滤之后，它将开始根据目的 MAC 地址和 VLAN 标签 (两者必须都匹配) 把帧转发到相应的目的地。)

Multicast
(组播)
=========

The Linux bridge driver has multicast support allowing it to process Internet
Group Management Protocol (IGMP) or Multicast Listener Discovery (MLD)
messages, and to efficiently forward multicast data packets. The bridge
driver supports IGMPv2/IGMPv3 and MLDv1/MLDv2.
(Linux 网桥驱动程序支持组播，能够处理 Internet 组管理协议 (IGMP) 或组播侦听发现 (MLD) 消息，并高效地转发组播数据包。该网桥驱动程序支持 IGMPv2/IGMPv3 和 MLDv1/MLDv2。)

Multicast snooping
(组播侦听 (snooping))
------------------

Multicast snooping is a networking technology that allows network switches
to intelligently manage multicast traffic within a local area network (LAN).
(组播侦听 (multicast snooping) 是一种网络技术，它让网络交换机能够智能地管理局域网 (LAN) 中的组播流量。)

The switch maintains a multicast group table, which records the association
between multicast group addresses and the ports where hosts have joined these
groups. The group table is dynamically updated based on the IGMP/MLD messages
received. With the multicast group information gathered through snooping, the
switch optimizes the forwarding of multicast traffic. Instead of blindly
broadcasting the multicast traffic to all ports, it sends the multicast
traffic based on the destination MAC address only to ports which have
subscribed the respective destination multicast group.
(交换机维护一张组播组表，记录组播组地址与主机加入这些组的端口之间的关联。组播组表根据收到的 IGMP/MLD 消息动态更新。利用通过侦听收集到的组播组信息，交换机可以优化组播流量的转发：它不再把组播流量盲目地广播到所有端口，而是根据目的 MAC 地址，只把组播流量发送到已订阅相应目的组播组的端口。)

When created, the Linux bridge devices have multicast snooping enabled by
default. It maintains a Multicast forwarding database (MDB) which keeps track
of port and group relationships.
(Linux 网桥设备创建时默认启用组播侦听。它维护一个组播转发数据库 (MDB)，用于跟踪端口和组的关系。)

IGMPv3/MLDv2 EHT support
(IGMPv3/MLDv2 EHT 支持)
------------------------

The Linux bridge supports IGMPv3/MLDv2 EHT (Explicit Host Tracking), which
was added by `474ddb37fa3a ("net: bridge: multicast: add EHT allow/block handling")
<https://lore.kernel.org/netdev/20210120145203.1109140-1-razor@blackwall.org/>`_

The explicit host tracking enables the device to keep track of each
individual host that is joined to a particular group or channel. The main
benefit of the explicit host tracking in IGMP is to allow minimal leave
latencies when a host leaves a multicast group or channel.
(显式主机跟踪 (explicit host tracking) 使设备能够跟踪加入特定组或信道的每一台主机。IGMP 中显式主机跟踪的主要好处是，当主机离开组播组或信道时，可以将离开延迟降到最低。)

The length of time between a host wanting to leave and a device stopping
traffic forwarding is called the IGMP leave latency. A device configured
with IGMPv3 or MLDv2 and explicit tracking can immediately stop forwarding
traffic if the last host to request to receive traffic from the device
indicates that it no longer wants to receive traffic. The leave latency
is thus bound only by the packet transmission latencies in the multiaccess
network and the processing time in the device.
(从主机想要离开到设备停止转发流量之间的时长称为 IGMP 离开延迟。配置了 IGMPv3 或 MLDv2 以及显式跟踪的设备，如果最后一个请求从设备接收流量的主机表示不再想接收流量，就可以立即停止转发流量。因此，离开延迟仅受多路访问网络中数据包传输延迟和设备处理时间的限制。)

Other multicast features
(其他组播特性)
------------------------

The Linux bridge also supports `per-VLAN multicast snooping
<https://lore.kernel.org/netdev/20210719170637.435541-1-razor@blackwall.org/>`_,
which is disabled by default but can be enabled. And `Multicast Router Discovery
<https://lore.kernel.org/netdev/20190121062628.2710-1-linus.luessing@c0d3.blue/>`_,
which help identify the location of multicast routers.
(Linux 网桥还支持 per-VLAN 组播侦听 (默认禁用，但可以启用)，以及组播路由器发现 (Multicast Router Discovery)，后者有助于识别组播路由器的位置。)

Switchdev
(Switchdev)
=========

Linux Bridge Switchdev is a feature in the Linux kernel that extends the
capabilities of the traditional Linux bridge to work more efficiently with
hardware switches that support switchdev. With Linux Bridge Switchdev, certain
networking functions like forwarding, filtering, and learning of Ethernet
frames can be offloaded to a hardware switch. This offloading reduces the
burden on the Linux kernel and CPU, leading to improved network performance
and lower latency.
(Linux Bridge Switchdev 是 Linux 内核中的一项特性，它扩展了传统 Linux 网桥的能力，使其能够更高效地与支持 switchdev 的硬件交换机协同工作。借助 Linux Bridge Switchdev，诸如以太网帧的转发、过滤和学习之类的网络功能可以卸载 (offload) 到硬件交换机上。这种卸载减轻了 Linux 内核和 CPU 的负担，从而带来更好的网络性能和更低的延迟。)

To use Linux Bridge Switchdev, you need hardware switches that support the
switchdev interface. This means that the switch hardware needs to have the
necessary drivers and functionality to work in conjunction with the Linux
kernel.
(要使用 Linux Bridge Switchdev，你需要支持 switchdev 接口的硬件交换机。这意味着交换机硬件需要具备必要的驱动程序和功能，才能与 Linux 内核协同工作。)

Please see the :ref:`switchdev` document for more details.
(更多细节请参阅 :ref:`switchdev` 文档。)

Netfilter
(Netfilter)
==========

The bridge netfilter module is a legacy feature that allows to filter bridged
packets with iptables and ip6tables. Its use is discouraged. Users should
consider using nftables for packet filtering.
(网桥 netfilter 模块是一项遗留特性，允许使用 iptables 和 ip6tables 过滤被桥接的数据包。不建议使用它。用户应考虑使用 nftables 进行数据包过滤。)

The older ebtables tool is more feature-limited compared to nftables, but
just like nftables it doesn't need this module either to function.
(较旧的 ebtables 工具与 nftables 相比功能较为有限，但和 nftables 一样，它不需要这个模块也能工作。)

The br_netfilter module intercepts packets entering the bridge, performs
minimal sanity tests on ipv4 and ipv6 packets and then pretends that
these packets are being routed, not bridged. br_netfilter then calls
the ip and ipv6 netfilter hooks from the bridge layer, i.e. ip(6)tables
rulesets will also see these packets.
(br_netfilter 模块拦截进入网桥的数据包，对 ipv4 和 ipv6 数据包执行最小限度的完整性检查，然后假装这些数据包正在被路由而不是被桥接。br_netfilter 随后从网桥层调用 ip 和 ipv6 netfilter 钩子，也就是说 ip(6)tables 规则集也会看到这些数据包。)

br_netfilter is also the reason for the iptables *physdev* match:
This match is the only way to reliably tell routed and bridged packets
apart in an iptables ruleset.
(br_netfilter 也是 iptables *physdev* 匹配存在的原因：这个匹配是在 iptables 规则集中可靠区分被路由数据包和被桥接数据包的唯一方法。)

Note that ebtables and nftables will work fine without the br_netfilter module.
iptables/ip6tables/arptables do not work for bridged traffic because they
plug in the routing stack. nftables rules in ip/ip6/inet/arp families won't
see traffic that is forwarded by a bridge either, but that's very much how it
should be.
(注意，没有 br_netfilter 模块，ebtables 和 nftables 也能正常工作。iptables/ip6tables/arptables 对被桥接的流量不起作用，因为它们挂在路由协议栈上。ip/ip6/inet/arp 族中的 nftables 规则也看不到由网桥转发的流量，但这本来就是理应如此的行为。)

Historically the feature set of ebtables was very limited (it still is),
this module was added to pretend packets are routed and invoke the ipv4/ipv6
netfilter hooks from the bridge so users had access to the more feature-rich
iptables matching capabilities (including conntrack). nftables doesn't have
this limitation, pretty much all features work regardless of the protocol family.
(历史上，ebtables 的特性集非常有限 (现在依然如此)，添加这个模块是为了假装数据包正在被路由，并从网桥调用 ipv4/ipv6 netfilter 钩子，从而让用户能够使用功能更丰富的 iptables 匹配能力 (包括 conntrack)。nftables 没有这个限制，几乎所有特性都与协议族无关地正常工作。)

So, br_netfilter is only needed if users, for some reason, need to use
ip(6)tables to filter packets forwarded by the bridge, or NAT bridged
traffic. For pure link layer filtering, this module isn't needed.
(因此，只有当用户出于某种原因需要使用 ip(6)tables 过滤由网桥转发的数据包，或对被桥接的流量做 NAT 时，才需要 br_netfilter。对于纯链路层过滤，不需要这个模块。)

Other Features
(其他特性)
==============

The Linux bridge also supports `IEEE 802.11 Proxy ARP
<https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=958501163ddd6ea22a98f94fa0e7ce6d4734e5c4>`_,
`Media Redundancy Protocol (MRP)
<https://lore.kernel.org/netdev/20200426132208.3232-1-horatiu.vultur@microchip.com/>`_,
`Media Redundancy Protocol (MRP) LC mode
<https://lore.kernel.org/r/20201124082525.273820-1-horatiu.vultur@microchip.com>`_,
`IEEE 802.1X port authentication
<https://lore.kernel.org/netdev/20220218155148.2329797-1-schultz.hans+netdev@gmail.com/>`_,
and `MAC Authentication Bypass (MAB)
<https://lore.kernel.org/netdev/20221101193922.2125323-2-idosch@nvidia.com/>`_.
(Linux 网桥还支持 IEEE 802.11 代理 ARP、媒体冗余协议 (MRP)、媒体冗余协议 (MRP) LC 模式、IEEE 802.1X 端口认证以及 MAC 认证旁路 (MAB)。)

FAQ
(常见问题解答)
===

What does a bridge do?
(网桥的作用是什么？)
----------------------

A bridge transparently forwards traffic between multiple network interfaces.
In plain English this means that a bridge connects two or more physical
Ethernet networks, to form one larger (logical) Ethernet network.
(网桥在多个网络接口之间透明地转发流量。用通俗的话说，网桥把两个或更多物理以太网网络连接起来，形成一个更大的 (逻辑) 以太网网络。)

Is it L3 protocol independent?
(它与第 3 层协议无关吗？)
------------------------------

Yes. The bridge sees all frames, but it *uses* only L2 headers/information.
As such, the bridging functionality is protocol independent, and there should
be no trouble forwarding IPX, NetBEUI, IP, IPv6, etc.
(是。网桥能看到所有帧，但它只 *使用* L2 头部/信息。因此，桥接功能与协议无关，转发 IPX、NetBEUI、IP、IPv6 等都不应有问题。)

Contact Info
(联系信息)
============

The code is currently maintained by Roopa Prabhu <roopa@nvidia.com> and
Nikolay Aleksandrov <razor@blackwall.org>. Bridge bugs and enhancements
are discussed on the linux-netdev mailing list netdev@vger.kernel.org and
bridge@lists.linux.dev.
(代码目前由 Roopa Prabhu <roopa@nvidia.com> 和 Nikolay Aleksandrov <razor@blackwall.org> 维护。网桥的缺陷和增强功能在 linux-netdev 邮件列表 netdev@vger.kernel.org 和 bridge@lists.linux.dev 上讨论。)

The list is open to anyone interested: http://vger.kernel.org/vger-lists.html#netdev
(该列表向所有感兴趣的人开放： http://vger.kernel.org/vger-lists.html#netdev 。)

External Links
(外部链接)
==============

The old Documentation for Linux bridging is on:
https://wiki.linuxfoundation.org/networking/bridge
(Linux 桥接的旧文档位于： https://wiki.linuxfoundation.org/networking/bridge 。)
