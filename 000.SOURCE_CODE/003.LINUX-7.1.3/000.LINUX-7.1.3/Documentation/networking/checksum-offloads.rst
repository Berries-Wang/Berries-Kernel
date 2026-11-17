.. SPDX-License-Identifier: GPL-2.0

=================
Checksum Offloads
=================

(校验和卸载)


Introduction
(简介)
============

This document describes a set of techniques in the Linux networking stack to
take advantage of checksum offload capabilities of various NICs.
(本文档描述 Linux 网络栈中利用各种 NIC 的校验和卸载能力的一组技术。)

The following technologies are described:
(本文描述以下技术：)

* TX Checksum Offload
  (TX 校验和卸载)
* LCO: Local Checksum Offload
  (LCO：本地校验和卸载)
* RCO: Remote Checksum Offload
  (RCO：远程校验和卸载)

Things that should be documented here but aren't yet:
(此处应当记录但尚未记录的内容：)

* RX Checksum Offload
  (RX 校验和卸载)
* CHECKSUM_UNNECESSARY conversion
  (CHECKSUM_UNNECESSARY 转换)


TX Checksum Offload
(TX 校验和卸载)
===================

The interface for offloading a transmit checksum to a device is explained in
detail in comments near the top of include/linux/skbuff.h.
(把发送校验和卸载给设备的接口在 include/linux/skbuff.h 顶部的注释中有详细
说明。)

In brief, it allows to request the device fill in a single ones-complement
checksum defined by the sk_buff fields skb->csum_start and skb->csum_offset.
The device should compute the 16-bit ones-complement checksum (i.e. the
'IP-style' checksum) from csum_start to the end of the packet, and fill in the
result at (csum_start + csum_offset).
(简而言之，它允许请求设备填充一个由 sk_buff 字段 skb->csum_start 和
skb->csum_offset 定义的单个二进制反码校验和。设备应计算从 csum_start 到数据包
末尾的 16 位二进制反码校验和(即 "IP 风格"校验和)，并把结果填充到
(csum_start + csum_offset) 处。)

Because csum_offset cannot be negative, this ensures that the previous value of
the checksum field is included in the checksum computation, thus it can be used
to supply any needed corrections to the checksum (such as the sum of the
pseudo-header for UDP or TCP).
(由于 csum_offset 不能为负，这确保了校验和字段的先前值会包含在校验和计算中，
因此它可用于为校验和提供任何所需的修正(例如 UDP 或 TCP 伪头的和)。)

This interface only allows a single checksum to be offloaded.  Where
encapsulation is used, the packet may have multiple checksum fields in
different header layers, and the rest will have to be handled by another
mechanism such as LCO or RCO.
(该接口只允许卸载单个校验和。在使用封装的情况下，数据包可能在不同的头部层中
有多个校验和字段，其余的必须由另一种机制处理，例如 LCO 或 RCO。)

CRC32c can also be offloaded using this interface, by means of filling
skb->csum_start and skb->csum_offset as described above, and setting
skb->csum_not_inet: see skbuff.h comment (section 'D') for more details.
(CRC32c 也可以使用该接口卸载，方法是按如上所述填充 skb->csum_start 和
skb->csum_offset，并设置 skb->csum_not_inet：更多细节参见 skbuff.h 注释
(D 节)。)

No offloading of the IP header checksum is performed; it is always done in
software.  This is OK because when we build the IP header, we obviously have it
in cache, so summing it isn't expensive.  It's also rather short.
(IP 头校验和不执行卸载；它总是在软件中完成。这没有问题，因为当我们构建 IP 头
时，它显然已在缓存中，所以对它求和并不昂贵。而且它也相当短。)

The requirements for GSO are more complicated, because when segmenting an
encapsulated packet both the inner and outer checksums may need to be edited or
recomputed for each resulting segment.  See the skbuff.h comment (section 'E')
for more details.
(GSO 的要求更复杂，因为在对封装的数据包进行分段时，对于每个生成的分段，内层和
外层校验和可能都需要编辑或重新计算。更多细节参见 skbuff.h 注释(E 节)。)

A driver declares its offload capabilities in netdev->hw_features; see
Documentation/networking/netdev-features.rst for more.  Note that a device
which only advertises NETIF_F_IP[V6]_CSUM must still obey the csum_start and
csum_offset given in the SKB; if it tries to deduce these itself in hardware
(as some NICs do) the driver should check that the values in the SKB match
those which the hardware will deduce, and if not, fall back to checksumming in
software instead (with skb_csum_hwoffload_help() or one of the
skb_checksum_help() / skb_crc32c_csum_help functions, as mentioned in
include/linux/skbuff.h).
(驱动程序在 netdev->hw_features 中声明其卸载能力；更多信息参见
Documentation/networking/netdev-features.rst。注意，只通告 NETIF_F_IP[V6]_CSUM
的设备仍必须遵循 SKB 中给出的 csum_start 和 csum_offset；如果它试图在硬件中
自行推断这些值(如某些 NIC 所做的那样)，驱动程序应检查 SKB 中的值与硬件将推断
出的值是否匹配，如果不匹配，则改为回退到软件校验和计算(使用
skb_csum_hwoffload_help() 或 skb_checksum_help() / skb_crc32c_csum_help 函数
之一，如 include/linux/skbuff.h 中所述)。)

The stack should, for the most part, assume that checksum offload is supported
by the underlying device.  The only place that should check is
validate_xmit_skb(), and the functions it calls directly or indirectly.  That
function compares the offload features requested by the SKB (which may include
other offloads besides TX Checksum Offload) and, if they are not supported or
enabled on the device (determined by netdev->features), performs the
corresponding offload in software.  In the case of TX Checksum Offload, that
means calling skb_csum_hwoffload_help(skb, features).
(协议栈在大多数情况下应假定底层设备支持校验和卸载。唯一应当检查的地方是
validate_xmit_skb() 及其直接或间接调用的函数。该函数会比较 SKB 请求的卸载特性
(可能包括 TX 校验和卸载之外的其他卸载)，如果设备不支持或未启用它们(由
netdev->features 决定)，则在软件中执行相应的卸载。就 TX 校验和卸载而言，这意味
着调用 skb_csum_hwoffload_help(skb, features)。)


LCO: Local Checksum Offload
(LCO：本地校验和卸载)
===========================

LCO is a technique for efficiently computing the outer checksum of an
encapsulated datagram when the inner checksum is due to be offloaded.
(LCO 是一种在内层校验和即将被卸载时高效计算封装数据报外层校验和的技术。)

The ones-complement sum of a correctly checksummed TCP or UDP packet is equal
to the complement of the sum of the pseudo header, because everything else gets
'cancelled out' by the checksum field.  This is because the sum was
complemented before being written to the checksum field.
(经过正确校验的 TCP 或 UDP 数据包的二进制反码和等于伪头之和的补码，因为其余所
有内容都被校验和字段"抵消"了。这是因为该和在被写入校验和字段之前先取了补。)

More generally, this holds in any case where the 'IP-style' ones complement
checksum is used, and thus any checksum that TX Checksum Offload supports.
(更一般地说，这在使用 "IP 风格"二进制反码校验和的任何情况下都成立，因而对 TX
校验和卸载所支持的任何校验和都成立。)

That is, if we have set up TX Checksum Offload with a start/offset pair, we
know that after the device has filled in that checksum, the ones complement sum
from csum_start to the end of the packet will be equal to the complement of
whatever value we put in the checksum field beforehand.  This allows us to
compute the outer checksum without looking at the payload: we simply stop
summing when we get to csum_start, then add the complement of the 16-bit word
at (csum_start + csum_offset).
(也就是说，如果我们用一对 start/offset 设置好了 TX 校验和卸载，那么我们知道，
在设备填充该校验和之后，从 csum_start 到数据包末尾的二进制反码和将等于我们预先
放入校验和字段中的任何值的补码。这使我们无需查看载荷就能计算外层校验和：只要在
到达 csum_start 时停止求和，然后加上 (csum_start + csum_offset) 处的 16 位字的
补码即可。)

Then, when the true inner checksum is filled in (either by hardware or by
skb_checksum_help()), the outer checksum will become correct by virtue of the
arithmetic.
(然后，当真正的内层校验和被填充(无论是通过硬件还是通过 skb_checksum_help())
时，外层校验和将凭借这种算术特性自然变为正确。)

LCO is performed by the stack when constructing an outer UDP header for an
encapsulation such as VXLAN or GENEVE, in udp_set_csum().  Similarly for the
IPv6 equivalents, in udp6_set_csum().
(在为 VXLAN 或 GENEVE 等封装构建外层 UDP 头时，协议栈会在 udp_set_csum() 中执行
LCO。对于对应的 IPv6 版本，同样在 udp6_set_csum() 中执行。)

It is also performed when constructing an IPv4 GRE header, in
net/ipv4/ip_gre.c:build_header().  It is *not* currently performed when
constructing an IPv6 GRE header; the GRE checksum is computed over the whole
packet in net/ipv6/ip6_gre.c:ip6gre_xmit2(), but it should be possible to use
LCO here as IPv6 GRE still uses an IP-style checksum.
(在构建 IPv4 GRE 头时也会执行 LCO，位置在 net/ipv4/ip_gre.c:build_header()。
目前在构建 IPv6 GRE 头时并不会执行它；GRE 校验和是在
net/ipv6/ip6_gre.c:ip6gre_xmit2() 中对整个数据包计算的，但由于 IPv6 GRE 仍使用
IP 风格的校验和，在这里使用 LCO 应该是可行的。)

All of the LCO implementations use a helper function lco_csum(), in
include/linux/skbuff.h.
(所有 LCO 实现都使用一个辅助函数 lco_csum()，位于 include/linux/skbuff.h 中。)

LCO can safely be used for nested encapsulations; in this case, the outer
encapsulation layer will sum over both its own header and the 'middle' header.
This does mean that the 'middle' header will get summed multiple times, but
there doesn't seem to be a way to avoid that without incurring bigger costs
(e.g. in SKB bloat).
(LCO 可以安全地用于嵌套封装；在这种情况下，外层封装会把自身头部和"中间"头部
都求和。这确实意味着"中间"头部会被求和多次，但似乎没有不付出更大代价(例如
SKB 膨胀)就能避免这种情况的办法。)


RCO: Remote Checksum Offload
(RCO：远程校验和卸载)
============================

RCO is a technique for eliding the inner checksum of an encapsulated datagram,
allowing the outer checksum to be offloaded.  It does, however, involve a
change to the encapsulation protocols, which the receiver must also support.
For this reason, it is disabled by default.
(RCO 是一种省略封装数据报内层校验和、从而允许卸载外层校验和的技术。不过，它
涉及对封装协议的更改，接收方也必须支持这些更改。因此，它默认是禁用的。)

RCO is detailed in the following Internet-Drafts:
(RCO 在以下 Internet 草案中有详细描述：)

* https://tools.ietf.org/html/draft-herbert-remotecsumoffload-00
* https://tools.ietf.org/html/draft-herbert-vxlan-rco-00

In Linux, RCO is implemented individually in each encapsulation protocol, and
most tunnel types have flags controlling its use.  For instance, VXLAN has the
flag VXLAN_F_REMCSUM_TX (per struct vxlan_rdst) to indicate that RCO should be
used when transmitting to a given remote destination.
(在 Linux 中，RCO 在每个封装协议中单独实现，大多数隧道类型都有控制其使用的
标志。例如，VXLAN 有 VXLAN_F_REMCSUM_TX 标志(每个 struct vxlan_rdst 一个)，
用于指示在向给定远程目的地发送时应使用 RCO。)
