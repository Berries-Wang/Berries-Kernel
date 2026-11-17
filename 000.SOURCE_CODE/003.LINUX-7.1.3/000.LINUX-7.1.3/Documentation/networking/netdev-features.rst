.. SPDX-License-Identifier: GPL-2.0

=====================================================
Netdev features mess and how to get out from it alive
=====================================================

(Netdev 特性乱局及其生还之道)

Author:
	(作者：)
	Michał Mirosław <mirq-linux@rere.qmqm.pl>



Part I: Feature sets
(第一部分：特性集合)
====================

Long gone are the days when a network card would just take and give packets
verbatim.  Today's devices add multiple features and bugs (read: offloads)
that relieve an OS of various tasks like generating and checking checksums,
splitting packets, classifying them.  Those capabilities and their state
are commonly referred to as netdev features in Linux kernel world.
(网卡只会原封不动地收发包的时代早已一去不复返。如今的设备增加了多种特性与缺陷（即各种卸载），替操作系统分担
了诸如生成和校验校验和、拆分数据包、对数据包分类等任务。这些能力及其状态在 Linux 内核界通常被称为 netdev
特性 (netdev features)。)

There are currently three sets of features relevant to the driver, and
one used internally by network core:
(目前有三组特性与驱动相关，另有一组由网络核心内部使用：)

 1. netdev->hw_features set contains features whose state may possibly
    be changed (enabled or disabled) for a particular device by user's
    request.  This set should be initialized in ndo_init callback and not
    changed later.
    (netdev->hw_features 集合包含其状态在特定设备上可能因用户请求而改变（启用或禁用）的特性。该集合应在
    ndo_init 回调中初始化，之后不再更改。)

 2. netdev->features set contains features which are currently enabled
    for a device.  This should be changed only by network core or in
    error paths of ndo_set_features callback.
    (netdev->features 集合包含设备当前已启用的特性。它只能由网络核心更改，或在 ndo_set_features 回调的错误
    路径中更改。)

 3. netdev->vlan_features set contains features whose state is inherited
    by child VLAN devices (limits netdev->features set).  This is currently
    used for all VLAN devices whether tags are stripped or inserted in
    hardware or software.
    (netdev->vlan_features 集合包含其状态被子 VLAN 设备继承的特性（是对 netdev->features 集合的约束）。
    目前它用于所有 VLAN 设备，无论标签是由硬件还是软件剥离或插入。)

 4. netdev->wanted_features set contains feature set requested by user.
    This set is filtered by ndo_fix_features callback whenever it or
    some device-specific conditions change. This set is internal to
    networking core and should not be referenced in drivers.
    (netdev->wanted_features 集合包含用户请求的特性集。每当该集合或某些设备特定条件变化时，ndo_fix_features
    回调都会对其进行过滤。该集合属于网络核心内部，驱动不应引用它。)



Part II: Controlling enabled features
(第二部分：控制已启用的特性)
=====================================

When current feature set (netdev->features) is to be changed, new set
is calculated and filtered by calling ndo_fix_features callback
and netdev_fix_features(). If the resulting set differs from current
set, it is passed to ndo_set_features callback and (if the callback
returns success) replaces value stored in netdev->features.
NETDEV_FEAT_CHANGE notification is issued after that whenever current
set might have changed.
(当要更改当前特性集 (netdev->features) 时，会通过调用 ndo_fix_features 回调和 netdev_fix_features() 来计
算并过滤出新集合。如果所得集合与当前集合不同，则将其传给 ndo_set_features 回调，并且（若该回调返回成功）替
换存储在 netdev->features 中的值。之后，只要当前集合可能已发生变化，就会发出 NETDEV_FEAT_CHANGE 通知。)

The following events trigger recalculation:
(以下事件会触发重新计算：)

 1. device's registration, after ndo_init returned success
    (设备注册，且在 ndo_init 返回成功之后)
 2. user requested changes in features state
    (用户请求更改特性状态)
 3. netdev_update_features() is called
    (调用 netdev_update_features())

ndo_*_features callbacks are called with rtnl_lock held. Missing callbacks
are treated as always returning success.
(ndo_*_features 回调在持有 rtnl_lock 的情况下被调用。缺失的回调被视为总是返回成功。)

A driver that wants to trigger recalculation must do so by calling
netdev_update_features() while holding rtnl_lock. This should not be done
from ndo_*_features callbacks. netdev->features should not be modified by
driver except by means of ndo_fix_features callback.
(想要触发重新计算的驱动必须持有 rtnl_lock 并调用 netdev_update_features() 来完成。不应从 ndo_*_features
回调中这样做。除通过 ndo_fix_features 回调之外，驱动不应修改 netdev->features。)



Part III: Implementation hints
(第三部分：实现提示)
==============================

 * ndo_fix_features:
   (ndo_fix_features：)

All dependencies between features should be resolved here. The resulting
set can be reduced further by networking core imposed limitations (as coded
in netdev_fix_features()). For this reason it is safer to disable a feature
when its dependencies are not met instead of forcing the dependency on.
(特性之间的所有依赖关系都应在这里解决。所得集合还可能被网络核心施加的限制进一步削减（如 netdev_fix_features()
中的代码所示）。因此，当特性依赖未满足时，更安全的做法是禁用该特性，而不是强行开启其依赖。)

This callback should not modify hardware nor driver state (should be
stateless).  It can be called multiple times between successive
ndo_set_features calls.
(此回调不应修改硬件或驱动状态（应当是无状态的）。在相邻两次 ndo_set_features 调用之间，它可能被调用多次。)

Callback must not alter features contained in NETIF_F_SOFT_FEATURES or
NETIF_F_NEVER_CHANGE sets. The exception is NETIF_F_VLAN_CHALLENGED but
care must be taken as the change won't affect already configured VLANs.
(回调不得更改 NETIF_F_SOFT_FEATURES 或 NETIF_F_NEVER_CHANGE 集合中的特性。例外是
NETIF_F_VLAN_CHALLENGED，但必须谨慎，因为此更改不会影响已配置的 VLAN。)

 * ndo_set_features:
   (ndo_set_features：)

Hardware should be reconfigured to match passed feature set. The set
should not be altered unless some error condition happens that can't
be reliably detected in ndo_fix_features. In this case, the callback
should update netdev->features to match resulting hardware state.
Errors returned are not (and cannot be) propagated anywhere except dmesg.
(Note: successful return is zero, >0 means silent error.)
(应重新配置硬件以匹配传入的特性集。除非发生了在 ndo_fix_features 中无法可靠检测的错误情况，否则不应改动该集
合。在这种情况下，回调应更新 netdev->features 以匹配最终的硬件状态。返回的错误不会（也不能）传播到 dmesg
之外的任何地方。（注意：成功返回 0，大于 0 表示静默错误。）)



Part IV: Features
(第四部分：各特性)
=================

For current list of features, see include/linux/netdev_features.h.
This section describes semantics of some of them.
(特性的当前列表参见 include/linux/netdev_features.h。本节描述其中一些特性的语义。)

 * Transmit checksumming
   (发送校验和计算)

For complete description, see comments near the top of include/linux/skbuff.h.
(完整描述参见 include/linux/skbuff.h 顶部附近的注释。)

Note: NETIF_F_HW_CSUM is a superset of NETIF_F_IP_CSUM + NETIF_F_IPV6_CSUM.
It means that device can fill TCP/UDP-like checksum anywhere in the packets
whatever headers there might be.
(注意：NETIF_F_HW_CSUM 是 NETIF_F_IP_CSUM + NETIF_F_IPV6_CSUM 的超集。它意味着设备可以在数据包中的任意
位置填充类似 TCP/UDP 的校验和，无论其中包含什么样的头。)

 * Transmit TCP segmentation offload
   (发送 TCP 分段卸载)

NETIF_F_TSO_ECN means that hardware can properly split packets with CWR bit
set, be it TCPv4 (when NETIF_F_TSO is enabled) or TCPv6 (NETIF_F_TSO6).
(NETIF_F_TSO_ECN 表示硬件能够正确拆分设置了 CWR 位的数据包，无论是 TCPv4（当 NETIF_F_TSO 启用时）还是
TCPv6（NETIF_F_TSO6）。)

 * Transmit UDP segmentation offload
   (发送 UDP 分段卸载)

NETIF_F_GSO_UDP_L4 accepts a single UDP header with a payload that exceeds
gso_size. On segmentation, it segments the payload on gso_size boundaries and
replicates the network and UDP headers (fixing up the last one if less than
gso_size).
(NETIF_F_GSO_UDP_L4 接受一个载荷超过 gso_size 的单个 UDP 头。在分段时，它按 gso_size 边界切分载荷，并复
制网络头和 UDP 头（若最后一段小于 gso_size，则修正最后一个头）。)

 * Transmit DMA from high memory
   (从高端内存进行发送 DMA)

On platforms where this is relevant, NETIF_F_HIGHDMA signals that
ndo_start_xmit can handle skbs with frags in high memory.
(在适用的平台上，NETIF_F_HIGHDMA 表示 ndo_start_xmit 能够处理 frag 位于高端内存中的 skb。)

 * Transmit scatter-gather
   (发送分散聚合)

Those features say that ndo_start_xmit can handle fragmented skbs:
NETIF_F_SG --- paged skbs (skb_shinfo()->frags), NETIF_F_FRAGLIST ---
chained skbs (skb->next/prev list).
(这些特性表示 ndo_start_xmit 能够处理分片的 skb：NETIF_F_SG —— 分页 skb (skb_shinfo()->frags)，
NETIF_F_FRAGLIST —— 链式 skb (skb->next/prev 链表)。)

 * Software features
   (软件特性)

Features contained in NETIF_F_SOFT_FEATURES are features of networking
stack. Driver should not change behaviour based on them.
(NETIF_F_SOFT_FEATURES 中包含的特性是网络协议栈的特性。驱动不应基于它们改变自身行为。)

 * VLAN challenged
   (VLAN 受限)

NETIF_F_VLAN_CHALLENGED should be set for devices which can't cope with VLAN
headers. Some drivers set this because the cards can't handle the bigger MTU.
[FIXME: Those cases could be fixed in VLAN code by allowing only reduced-MTU
VLANs. This may be not useful, though.]
(无法处理 VLAN 头的设备应设置 NETIF_F_VLAN_CHALLENGED。有些驱动设置此标志是因为网卡无法处理更大的 MTU。
[FIXME：这些情况本可以在 VLAN 代码中修复，只允许 MTU 缩减的 VLAN。不过这可能没什么用处。])

*  rx-fcs
   (rx-fcs)

This requests that the NIC append the Ethernet Frame Checksum (FCS)
to the end of the skb data.  This allows sniffers and other tools to
read the CRC recorded by the NIC on receipt of the packet.
(此项请求网卡将以太网帧校验和 (FCS) 附加到 skb 数据的末尾。这使嗅探器和其他工具能够读取网卡在接收数据包时记
录的 CRC。)

*  rx-all
   (rx-all)

This requests that the NIC receive all possible frames, including errored
frames (such as bad FCS, etc).  This can be helpful when sniffing a link with
bad packets on it.  Some NICs may receive more packets if also put into normal
PROMISC mode.
(此项请求网卡接收所有可能的帧，包括出错的帧（如 FCS 错误等）。在嗅探一条存在坏包的链路时会有所帮助。某些网卡
在同时置于普通 PROMISC（混杂）模式时可能会接收到更多数据包。)

*  rx-gro-hw
   (rx-gro-hw)

This requests that the NIC enables Hardware GRO (generic receive offload).
Hardware GRO is basically the exact reverse of TSO, and is generally
stricter than Hardware LRO.  A packet stream merged by Hardware GRO must
be re-segmentable by GSO or TSO back to the exact original packet stream.
Hardware GRO is dependent on RXCSUM since every packet successfully merged
by hardware must also have the checksum verified by hardware.
(此项请求网卡启用硬件 GRO（通用接收卸载）。硬件 GRO 基本上就是 TSO 的精确逆过程，通常比硬件 LRO 更严格。
由硬件 GRO 合并的数据包流必须能够由 GSO 或 TSO 重新分段，还原为与原始数据包流完全一致的形式。硬件 GRO 依赖
RXCSUM，因为每个由硬件成功合并的数据包也必须由硬件校验过校验和。)

* hsr-tag-ins-offload
  (hsr-tag-ins-offload)

This should be set for devices which insert an HSR (High-availability Seamless
Redundancy) or PRP (Parallel Redundancy Protocol) tag automatically.
(自动插入 HSR（高可用无缝冗余）或 PRP（并行冗余协议）标签的设备应设置此项。)

* hsr-tag-rm-offload
  (hsr-tag-rm-offload)

This should be set for devices which remove HSR (High-availability Seamless
Redundancy) or PRP (Parallel Redundancy Protocol) tags automatically.
(自动移除 HSR（高可用无缝冗余）或 PRP（并行冗余协议）标签的设备应设置此项。)

* hsr-fwd-offload
  (hsr-fwd-offload)

This should be set for devices which forward HSR (High-availability Seamless
Redundancy) frames from one port to another in hardware.
(在硬件中将 HSR（高可用无缝冗余）帧从一个端口转发到另一个端口的设备应设置此项。)

* hsr-dup-offload
  (hsr-dup-offload)

This should be set for devices which duplicate outgoing HSR (High-availability
Seamless Redundancy) or PRP (Parallel Redundancy Protocol) tags automatically
frames in hardware.
(在硬件中自动为发出的 HSR（高可用无缝冗余）或 PRP（并行冗余协议）帧复制标签的设备应设置此项。)

* netmem-tx
  (netmem-tx)

This should be set for devices which support netmem TX. See
Documentation/networking/netmem.rst
(支持 netmem TX 的设备应设置此项。参见
Documentation/networking/netmem.rst)
