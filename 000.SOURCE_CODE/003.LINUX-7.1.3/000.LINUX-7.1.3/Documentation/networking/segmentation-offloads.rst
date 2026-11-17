.. SPDX-License-Identifier: GPL-2.0

===============================
Segmentation Offloads(分段卸载)
===============================


Introduction(简介)
==================

This document describes a set of techniques in the Linux networking stack
to take advantage of segmentation offload capabilities of various NICs.
(本文描述 Linux 网络协议栈中利用各类网卡分段卸载能力的一组技术。)

The following technologies are described:
(本文描述以下技术：)

 * TCP Segmentation Offload - TSO
   (TCP 分段卸载 - TSO)
 * UDP Fragmentation Offload - UFO
   (UDP 分片卸载 - UFO)
 * IPIP, SIT, GRE, and UDP Tunnel Offloads
   (IPIP、SIT、GRE 与 UDP 隧道卸载)
 * Generic Segmentation Offload - GSO
   (通用分段卸载 - GSO)
 * Generic Receive Offload - GRO
   (通用接收卸载 - GRO)
 * Partial Generic Segmentation Offload - GSO_PARTIAL
   (部分通用分段卸载 - GSO_PARTIAL)
 * SCTP acceleration with GSO - GSO_BY_FRAGS
   (用 GSO 加速 SCTP - GSO_BY_FRAGS)


TCP Segmentation Offload(TCP 分段卸载)
======================================

TCP segmentation allows a device to segment a single frame into multiple
frames with a data payload size specified in skb_shinfo()->gso_size.
When TCP segmentation requested the bit for either SKB_GSO_TCPV4 or
SKB_GSO_TCPV6 should be set in skb_shinfo()->gso_type and
skb_shinfo()->gso_size should be set to a non-zero value.
(TCP 分段允许设备将单个帧分割为多个帧，数据载荷大小由 skb_shinfo()->gso_size 指定。请求 TCP 分段时，应在 skb_shinfo()->gso_type 中设置 SKB_GSO_TCPV4 或 SKB_GSO_TCPV6 位，并将 skb_shinfo()->gso_size 设置为非零值。)

TCP segmentation is dependent on support for the use of partial checksum
offload.  For this reason TSO is normally disabled if the Tx checksum
offload for a given device is disabled.
(TCP 分段依赖于部分校验和卸载的支持。因此，当设备的 Tx 校验和卸载被禁用时，TSO 通常也会被禁用。)

In order to support TCP segmentation offload it is necessary to populate
the network and transport header offsets of the skbuff so that the device
drivers will be able determine the offsets of the IP or IPv6 header and the
TCP header.  In addition as CHECKSUM_PARTIAL is required csum_start should
also point to the TCP header of the packet.
(为了支持 TCP 分段卸载，需要填充 skbuff 的网络头和传输头偏移，使设备驱动能够确定 IP 或 IPv6 头以及 TCP 头的偏移。此外，由于要求 CHECKSUM_PARTIAL，csum_start 也应指向数据包的 TCP 头。)

For IPv4 segmentation we support one of two types in terms of the IP ID.
The default behavior is to increment the IP ID with every segment.  If the
GSO type SKB_GSO_TCP_FIXEDID is specified then we will not increment the IP
ID and all segments will use the same IP ID.
(对于 IPv4 分段，在 IP ID 方面我们支持两种类型。默认行为是每生成一个分段就递增 IP ID。如果指定了 GSO 类型 SKB_GSO_TCP_FIXEDID，则不递增 IP ID，所有分段使用相同的 IP ID。)

For encapsulated packets, SKB_GSO_TCP_FIXEDID refers only to the outer header.
SKB_GSO_TCP_FIXEDID_INNER can be used to specify the same for the inner header.
Any combination of these two GSO types is allowed.
(对于封装的数据包，SKB_GSO_TCP_FIXEDID 仅指外层头。SKB_GSO_TCP_FIXEDID_INNER 可用于对内层头指定同样的行为。这两种 GSO 类型可以任意组合。)

If a device has NETIF_F_TSO_MANGLEID set then the IP ID can be ignored when
performing TSO and we will either increment the IP ID for all frames, or leave
it at a static value based on driver preference.  For encapsulated packets,
NETIF_F_TSO_MANGLEID is relevant for both outer and inner headers, unless the
DF bit is not set on the outer header, in which case the device driver must
guarantee that the IP ID field is incremented in the outer header with every
segment.
(如果设备设置了 NETIF_F_TSO_MANGLEID，执行 TSO 时可以忽略 IP ID，我们将对所有帧递增 IP ID，或根据驱动偏好保持其为一个固定值。对于封装的数据包，NETIF_F_TSO_MANGLEID 对外层和内层头都有效，除非外层头未设置 DF 位；此时设备驱动必须保证外层头的 IP ID 字段随每个分段递增。)


UDP Fragmentation Offload(UDP 分片卸载)
=======================================

UDP fragmentation offload allows a device to fragment an oversized UDP
datagram into multiple IPv4 fragments.  Many of the requirements for UDP
fragmentation offload are the same as TSO.  However the IPv4 ID for
fragments should not increment as a single IPv4 datagram is fragmented.
(UDP 分片卸载允许设备将超大的 UDP 数据报分片为多个 IPv4 分片。UDP 分片卸载的许多要求与 TSO 相同。但是，由于是对单个 IPv4 数据报进行分片，各分片的 IPv4 ID 不应递增。)

UFO is deprecated: modern kernels will no longer generate UFO skbs, but can
still receive them from tuntap and similar devices. Offload of UDP-based
tunnel protocols is still supported.
(UFO 已被弃用：现代内核不再生成 UFO skb，但仍可以从 tuntap 及类似设备接收它们。基于 UDP 的隧道协议的卸载仍然受支持。)


IPIP, SIT, GRE, UDP Tunnel, and Remote Checksum Offloads(IPIP、SIT、GRE、UDP 隧道与远程校验和卸载)
==================================================================================================

In addition to the offloads described above it is possible for a frame to
contain additional headers such as an outer tunnel.  In order to account
for such instances an additional set of segmentation offload types were
introduced including SKB_GSO_IPXIP4, SKB_GSO_IPXIP6, SKB_GSO_GRE, and
SKB_GSO_UDP_TUNNEL.  These extra segmentation types are used to identify
cases where there are more than just 1 set of headers.  For example in the
case of IPIP and SIT we should have the network and transport headers moved
from the standard list of headers to "inner" header offsets.
(除上述卸载外，帧中还可能包含额外的头（如外层隧道）。为了处理这种情况，引入了另一组分段卸载类型，包括 SKB_GSO_IPXIP4、SKB_GSO_IPXIP6、SKB_GSO_GRE 和 SKB_GSO_UDP_TUNNEL。这些额外的分段类型用于标识存在不止一组头的情况。例如在 IPIP 和 SIT 的情况下，应将网络头和传输头从标准头列表移到"内层"头偏移中。)

Currently only two levels of headers are supported.  The convention is to
refer to the tunnel headers as the outer headers, while the encapsulated
data is normally referred to as the inner headers.  Below is the list of
calls to access the given headers:
(目前只支持两级头。按照惯例，隧道头被称为外层头，而被封装的数据通常被称为内层头。下面是访问各层头的调用列表：)

IPIP/SIT Tunnel::
(IPIP/SIT 隧道：)

             Outer                  Inner
  MAC        skb_mac_header
  Network    skb_network_header     skb_inner_network_header
  Transport  skb_transport_header

UDP/GRE Tunnel::
(UDP/GRE 隧道：)

             Outer                  Inner
  MAC        skb_mac_header         skb_inner_mac_header
  Network    skb_network_header     skb_inner_network_header
  Transport  skb_transport_header   skb_inner_transport_header

In addition to the above tunnel types there are also SKB_GSO_GRE_CSUM and
SKB_GSO_UDP_TUNNEL_CSUM.  These two additional tunnel types reflect the
fact that the outer header also requests to have a non-zero checksum
included in the outer header.
(除上述隧道类型外，还有 SKB_GSO_GRE_CSUM 和 SKB_GSO_UDP_TUNNEL_CSUM。这两种额外的隧道类型表明外层头还要求在外层头中包含非零校验和。)

Finally there is SKB_GSO_TUNNEL_REMCSUM which indicates that a given tunnel
header has requested a remote checksum offload.  In this case the inner
headers will be left with a partial checksum and only the outer header
checksum will be computed.
(最后还有 SKB_GSO_TUNNEL_REMCSUM，它表示某个隧道头请求了远程校验和卸载。在这种情况下，内层头将保留部分校验和，只计算外层头的校验和。)


Generic Segmentation Offload(通用分段卸载)
==========================================

Generic segmentation offload is a pure software offload that is meant to
deal with cases where device drivers cannot perform the offloads described
above.  What occurs in GSO is that a given skbuff will have its data broken
out over multiple skbuffs that have been resized to match the MSS provided
via skb_shinfo()->gso_size.
(通用分段卸载是一种纯软件卸载，用于处理设备驱动无法执行上述卸载的情况。GSO 的做法是：将给定 skbuff 的数据拆分到多个 skbuff 中，这些 skbuff 会被调整大小以匹配通过 skb_shinfo()->gso_size 提供的 MSS。)

Before enabling any hardware segmentation offload a corresponding software
offload is required in GSO.  Otherwise it becomes possible for a frame to
be re-routed between devices and end up being unable to be transmitted.
(在启用任何硬件分段卸载之前，GSO 中必须存在对应的软件卸载。否则，帧可能在设备之间被重新路由，最终无法发送。)


Generic Receive Offload(通用接收卸载)
=====================================

Generic receive offload is the complement to GSO.  Ideally any frame
assembled by GRO should be segmented to create an identical sequence of
frames using GSO, and any sequence of frames segmented by GSO should be
able to be reassembled back to the original by GRO.
(通用接收卸载是 GSO 的对偶。理想情况下，GRO 聚合出的任何帧都应能用 GSO 分段生成完全相同的帧序列；而 GSO 分段出的任何帧序列也都应能被 GRO 重新聚合回原样。)


Partial Generic Segmentation Offload(部分通用分段卸载)
======================================================

Partial generic segmentation offload is a hybrid between TSO and GSO.  What
it effectively does is take advantage of certain traits of TCP and tunnels
so that instead of having to rewrite the packet headers for each segment
only the inner-most transport header and possibly the outer-most network
header need to be updated.  This allows devices that do not support tunnel
offloads or tunnel offloads with checksum to still make use of segmentation.
(部分通用分段卸载是 TSO 与 GSO 的混合体。它实际上利用了 TCP 和隧道的某些特性，从而不必为每个分段重写所有数据包头，只需更新最内层传输头，可能还有最外层网络头。这使得不支持隧道卸载或带校验和的隧道卸载的设备也能使用分段功能。)

With the partial offload what occurs is that all headers excluding the
inner transport header are updated such that they will contain the correct
values for if the header was simply duplicated.  The one exception to this
is the outer IPv4 ID field.  It is up to the device drivers to guarantee
that the IPv4 ID field is incremented in the case that a given header does
not have the DF bit set.
(在部分卸载中，除内层传输头之外的所有头都会被更新，使其包含与简单复制该头时相同的正确值。唯一的例外是外层 IPv4 ID 字段。当某个头未设置 DF 位时，IPv4 ID 字段是否递增由设备驱动负责保证。)


SCTP acceleration with GSO(用 GSO 加速 SCTP)
============================================

SCTP - despite the lack of hardware support - can still take advantage of
GSO to pass one large packet through the network stack, rather than
multiple small packets.
(尽管缺乏硬件支持，SCTP 仍可利用 GSO 让一个大数据包（而不是多个小数据包）穿过网络协议栈。)

This requires a different approach to other offloads, as SCTP packets
cannot be just segmented to (P)MTU. Rather, the chunks must be contained in
IP segments, padding respected. So unlike regular GSO, SCTP can't just
generate a big skb, set gso_size to the fragmentation point and deliver it
to IP layer.
(这需要与其他卸载不同的处理方式，因为 SCTP 数据包不能简单地按 (P)MTU 分段。相反，块 (chunk) 必须完整地包含在 IP 分段中，并遵守填充。因此与常规 GSO 不同，SCTP 不能只生成一个大 skb、把 gso_size 设为分片点并交给 IP 层。)

Instead, the SCTP protocol layer builds an skb with the segments correctly
padded and stored as chained skbs, and skb_segment() splits based on those.
To signal this, gso_size is set to the special value GSO_BY_FRAGS.
(取而代之的是，SCTP 协议层构建一个 skb，其中各分段被正确填充并以链式 skb 的形式存储，skb_segment() 基于这些链式 skb 进行拆分。为标记这一情况，gso_size 被设为特殊值 GSO_BY_FRAGS。)

Therefore, any code in the core networking stack must be aware of the
possibility that gso_size will be GSO_BY_FRAGS and handle that case
appropriately.
(因此，网络协议栈核心中的任何代码都必须意识到 gso_size 可能是 GSO_BY_FRAGS，并妥善处理这种情况。)

There are some helpers to make this easier:
(有一些辅助函数可以让这件事更容易：)

- skb_is_gso(skb) && skb_is_gso_sctp(skb) is the best way to see if
  an skb is an SCTP GSO skb.
  (skb_is_gso(skb) && skb_is_gso_sctp(skb) 是判断一个 skb 是否为 SCTP GSO skb 的最佳方式。)

- For size checks, the skb_gso_validate_*_len family of helpers correctly
  considers GSO_BY_FRAGS.
  (对于大小检查，skb_gso_validate_*_len 系列辅助函数能正确处理 GSO_BY_FRAGS。)

- For manipulating packets, skb_increase_gso_size and skb_decrease_gso_size
  will check for GSO_BY_FRAGS and WARN if asked to manipulate these skbs.
  (对于操作数据包，skb_increase_gso_size 和 skb_decrease_gso_size 会检查 GSO_BY_FRAGS，若被要求操作这类 skb 则会发出 WARN。)

This also affects drivers with the NETIF_F_FRAGLIST & NETIF_F_GSO_SCTP bits
set. Note also that NETIF_F_GSO_SCTP is included in NETIF_F_GSO_SOFTWARE.
(这也影响设置了 NETIF_F_FRAGLIST 和 NETIF_F_GSO_SCTP 位的驱动。另请注意，NETIF_F_GSO_SCTP 包含在 NETIF_F_GSO_SOFTWARE 之中。)
