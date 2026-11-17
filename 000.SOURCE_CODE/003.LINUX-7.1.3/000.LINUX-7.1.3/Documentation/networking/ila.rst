.. SPDX-License-Identifier: GPL-2.0

=============================================================
Identifier Locator Addressing (ILA)(标识符-定位符寻址（ILA）)
=============================================================


Introduction(简介)
==================

Identifier-locator addressing (ILA) is a technique used with IPv6 that
differentiates between location and identity of a network node. Part of an
address expresses the immutable identity of the node, and another part
indicates the location of the node which can be dynamic. Identifier-locator
addressing can be used to efficiently implement overlay networks for
network virtualization as well as solutions for use cases in mobility.
(标识符-定位符寻址（ILA）是一种配合 IPv6 使用的技术，用于区分网络节点的
位置与身份。地址的一部分表达节点不可变的身份，另一部分指示节点的位置，
后者可以是动态的。标识符-定位符寻址可用于高效地实现面向网络虚拟化的
覆盖网络，以及面向移动性用例的解决方案。)

ILA can be thought of as means to implement an overlay network without
encapsulation. This is accomplished by performing network address
translation on destination addresses as a packet traverses a network. To
the network, an ILA translated packet appears to be no different than any
other IPv6 packet. For instance, if the transport protocol is TCP then an
ILA translated packet looks like just another TCP/IPv6 packet. The
advantage of this is that ILA is transparent to the network so that
optimizations in the network, such as ECMP, RSS, GRO, GSO, etc., just work.
(可以把 ILA 看作一种无需封装即可实现覆盖网络的手段。其实现方式是在
数据包穿越网络时对目的地址执行网络地址转换。对网络而言，经过 ILA 转换
的数据包与任何其他 IPv6 数据包看起来并无二致。例如，若传输协议是 TCP，
那么经过 ILA 转换的数据包看起来就只是又一个 TCP/IPv6 数据包。这样做
的好处是 ILA 对网络透明，因此网络中的各种优化机制——如 ECMP、RSS、
GRO、GSO 等——都能照常工作。)

The ILA protocol is described in Internet-Draft draft-herbert-intarea-ila.
(ILA 协议在 Internet-Draft draft-herbert-intarea-ila 中描述。)


ILA terminology(ILA 术语)
=========================

  - Identifier
		(标识符)

		A number that identifies an addressable node in the network
		independent of its location. ILA identifiers are sixty-four
		bit values.
		(在网络中标识一个可寻址节点的编号，与该节点的位置无关。
		ILA 标识符是 64 位值。)

  - Locator
		(定位符)

		A network prefix that routes to a physical host. Locators
		provide the topological location of an addressed node. ILA
		locators are sixty-four bit prefixes.
		(路由到某个物理主机的网络前缀。定位符提供被寻址节点的
		拓扑位置。ILA 定位符是 64 位前缀。)

  - ILA mapping
		(ILA 映射)

		A mapping of an ILA identifier to a locator (or to a
		locator and meta data). An ILA domain maintains a database
		that contains mappings for all destinations in the domain.
		(ILA 标识符到定位符（或定位符与元数据）的映射。ILA 域
		维护一个数据库，其中包含该域内所有目的地的映射。)

  - SIR address
		(SIR 地址)

		An IPv6 address composed of a SIR prefix (upper sixty-
		four bits) and an identifier (lower sixty-four bits).
		SIR addresses are visible to applications and provide a
		means for them to address nodes independent of their
		location.
		(由 SIR 前缀（高 64 位）和标识符（低 64 位）组成的
		IPv6 地址。SIR 地址对应用可见，为应用提供一种与节点
		位置无关的寻址手段。)

  - ILA address
		(ILA 地址)

		An IPv6 address composed of a locator (upper sixty-four
		bits) and an identifier (low order sixty-four bits). ILA
		addresses are never visible to an application.
		(由定位符（高 64 位）和标识符（低 64 位）组成的 IPv6
		地址。ILA 地址永远不会对应用可见。)

  - ILA host
		(ILA 主机)

		An end host that is capable of performing ILA translations
		on transmit or receive.
		(能够在发送或接收时执行 ILA 转换的终端主机。)

  - ILA router
		(ILA 路由器)

		A network node that performs ILA translation and forwarding
		of translated packets.
		(执行 ILA 转换并转发已转换数据包的网络节点。)

  - ILA forwarding cache
		(ILA 转发缓存)

		A type of ILA router that only maintains a working set
		cache of mappings.
		(一种只维护映射工作集缓存的 ILA 路由器。)

  - ILA node
		(ILA 节点)

		A network node capable of performing ILA translations. This
		can be an ILA router, ILA forwarding cache, or ILA host.
		(能够执行 ILA 转换的网络节点。可以是 ILA 路由器、ILA
		转发缓存或 ILA 主机。)


Operation(操作)
===============

There are two fundamental operations with ILA:
(ILA 有两种基本操作：)

  - Translate a SIR address to an ILA address. This is performed on ingress
    to an ILA overlay.
    (把 SIR 地址转换为 ILA 地址。这在进入 ILA 覆盖网络的入口处执行。)

  - Translate an ILA address to a SIR address. This is performed on egress
    from the ILA overlay.
    (把 ILA 地址转换为 SIR 地址。这在离开 ILA 覆盖网络的出口处执行。)

ILA can be deployed either on end hosts or intermediate devices in the
network; these are provided by "ILA hosts" and "ILA routers" respectively.
Configuration and datapath for these two points of deployment is somewhat
different.
(ILA 可以部署在终端主机上，也可以部署在网络中的中间设备上；它们分别由
"ILA 主机"和"ILA 路由器"提供。这两种部署点的配置与数据路径略有不同。)

The diagram below illustrates the flow of packets through ILA as well
as showing ILA hosts and routers::
(下图展示了数据包流经 ILA 的过程，同时标示出 ILA 主机与 ILA 路由器：)

    +--------+                                                +--------+
    | Host A +-+                                         +--->| Host B |
    |        | |              (2) ILA                   (')   |        |
    +--------+ |            ...addressed....           (   )  +--------+
	       V  +---+--+  .  packet      .  +---+--+  (_)
   (1) SIR     |  | ILA  |----->-------->---->| ILA  |   |   (3) SIR
    addressed  +->|router|  .              .  |router|->-+    addressed
    packet        +---+--+  .     IPv6     .  +---+--+        packet
		   /        .    Network   .
		  /         .              .   +--+-++--------+
    +--------+   /          .              .   |ILA ||  Host  |
    |  Host  +--+           .              .- -|host||        |
    |        |              .              .   +--+-++--------+
    +--------+              ................


Transport checksum handling(传输层校验和处理)
=============================================

When an address is translated by ILA, an encapsulated transport checksum
that includes the translated address in a pseudo header may be rendered
incorrect on the wire. This is a problem for intermediate devices,
including checksum offload in NICs, that process the checksum. There are
three options to deal with this:
(当 ILA 转换地址时，伪头部（pseudo header）中包含被转换地址的封装传输层
校验和在链路上可能会变得不正确。这对处理校验和的中间设备——包括网卡
中的校验和卸载——是个问题。有三种应对方案：)

- no action	Allow the checksum to be incorrect on the wire. Before
		a receiver verifies a checksum the ILA to SIR address
		translation must be done.
		(不采取动作：允许校验和在链路上不正确。接收方在验证校验
		和之前，必须先完成 ILA 地址到 SIR 地址的转换。)

- adjust transport checksum
		When ILA translation is performed the packet is parsed
		and if a transport layer checksum is found then it is
		adjusted to reflect the correct checksum per the
		translated address.
		(调整传输层校验和：执行 ILA 转换时会解析数据包，如果发现
		传输层校验和，则对其进行调整，使其反映按转换后地址计算的
		正确校验和。)

- checksum neutral mapping
		When an address is translated the difference can be offset
		elsewhere in a part of the packet that is covered by
		the checksum. The low order sixteen bits of the identifier
		are used. This method is preferred since it doesn't require
		parsing a packet beyond the IP header and in most cases the
		adjustment can be precomputed and saved with the mapping.
		(校验和中性映射：转换地址时，其差值可以在数据包中校验和
		覆盖范围内的其他部分加以抵消，使用的是标识符的低 16 位。
		该方法不需要解析 IP 头之外的内容，且大多数情况下调整值
		可以预先计算并随映射一起保存，因此是首选方法。)

Note that the checksum neutral adjustment affects the low order sixteen
bits of the identifier. When ILA to SIR address translation is done on
egress the low order bits are restored to the original value which
restores the identifier as it was originally sent.
(注意，校验和中性调整会影响标识符的低 16 位。在出口处执行 ILA 地址到
SIR 地址的转换时，低位会恢复为原值，从而使标识符恢复为最初发送时的
样子。)


Identifier types(标识符类型)
============================

ILA defines different types of identifiers for different use cases.
(ILA 针对不同用例定义了不同类型的标识符。)

The defined types are:
(已定义的类型如下：)

      0: interface identifier
      (0：接口标识符)

      1: locally unique identifier
      (1：本地唯一标识符)

      2: virtual networking identifier for IPv4 address
      (2：IPv4 地址的虚拟网络标识符)

      3: virtual networking identifier for IPv6 unicast address
      (3：IPv6 单播地址的虚拟网络标识符)

      4: virtual networking identifier for IPv6 multicast address
      (4：IPv6 组播地址的虚拟网络标识符)

      5: non-local address identifier
      (5：非本地地址标识符)

In the current implementation of kernel ILA only locally unique identifiers
(LUID) are supported. LUID allows for a generic, unformatted 64 bit
identifier.
(在内核 ILA 的当前实现中，仅支持本地唯一标识符（LUID）。LUID 允许使用
通用的、无格式的 64 位标识符。)


Identifier formats(标识符格式)
==============================

Kernel ILA supports two optional fields in an identifier for formatting:
"C-bit" and "identifier type". The presence of these fields is determined
by configuration as demonstrated below.
(内核 ILA 在标识符中支持两个可选的格式化字段："C-bit" 与 "identifier
type"。这些字段是否存在由配置决定，如下所示。)

If the identifier type is present it occupies the three highest order
bits of an identifier. The possible values are given in the above list.
(如果存在标识符类型字段，它会占用标识符的最高三位。可能的取值见上文
列表。)

If the C-bit is present,  this is used as an indication that checksum
neutral mapping has been done. The C-bit can only be set in an
ILA address, never a SIR address.
(如果存在 C 位，则用于指示已完成校验和中性映射。C 位只能在 ILA 地址中
设置，绝不能在 SIR 地址中设置。)

In the simplest format the identifier types, C-bit, and checksum
adjustment value are not present so an identifier is considered an
unstructured sixty-four bit value::

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                            Identifier                         |
     +                                                               +
     |                                                               |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

(在最简单的格式中，标识符类型、C 位与校验和调整值都不存在，此时标识符
被视为一个无结构的 64 位值：)

The checksum neutral adjustment may be configured to always be
present using neutral-map-auto. In this case there is no C-bit, but the
checksum adjustment is in the low order 16 bits. The identifier is
still sixty-four bits::

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                            Identifier                         |
     |                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                               |  Checksum-neutral adjustment  |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

(可以通过 neutral-map-auto 把校验和中性调整配置为始终存在。这种情况下
没有 C 位，但校验和调整位于低 16 位。标识符仍然是 64 位：)

The C-bit may used to explicitly indicate that checksum neutral
mapping has been applied to an ILA address. The format is::

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |     |C|                    Identifier                         |
     |     +-+                       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                               |  Checksum-neutral adjustment  |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

(C 位可用于显式指示已对 ILA 地址应用校验和中性映射。格式如下：)

The identifier type field may be present to indicate the identifier
type. If it is not present then the type is inferred based on mapping
configuration. The checksum neutral adjustment may automatically
used with the identifier type as illustrated below::

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     | Type|                      Identifier                         |
     +-+-+-+                         +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                               |  Checksum-neutral adjustment  |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

(标识符类型字段可以存在，用于指示标识符类型。如果不存在，则类型根据
映射配置推断。校验和中性调整可以与标识符类型自动配合使用，如下图
所示：)

If the identifier type and the C-bit can be present simultaneously so
the identifier format would be::

     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     | Type|C|                    Identifier                         |
     +-+-+-+-+                       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     |                               |  Checksum-neutral adjustment  |
     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

(如果标识符类型与 C 位同时存在，标识符格式将为：)

Configuration(配置)
===================

There are two methods to configure ILA mappings. One is by using LWT routes
and the other is ila_xlat (called from NFHOOK PREROUTING hook). ila_xlat
is intended to be used in the receive path for ILA hosts .
(配置 ILA 映射有两种方法：一种是使用 LWT 路由，另一种是 ila_xlat（从
NFHOOK PREROUTING 钩子调用）。ila_xlat 供 ILA 主机在接收路径中使用。)

An ILA router has also been implemented in XDP. Description of that is
outside the scope of this document.
(XDP 中也实现了一个 ILA 路由器，其描述超出本文档的范围。)

The usage of for ILA LWT routes is:
(ILA LWT 路由的用法如下：)

ip route add DEST/128 encap ila LOC csum-mode MODE ident-type TYPE via ADDR

Destination (DEST) can either be a SIR address (for an ILA host or ingress
ILA router) or an ILA address (egress ILA router). LOC is the sixty-four
bit locator (with format W:X:Y:Z) that overwrites the upper sixty-four
bits of the destination address.  Checksum MODE is one of "no-action",
"adj-transport", "neutral-map", and "neutral-map-auto". If neutral-map is
set then the C-bit will be present. Identifier TYPE one of "luid" or
"use-format." In the case of use-format, the identifier type field is
present and the effective type is taken from that.
(目的地址（DEST）可以是 SIR 地址（用于 ILA 主机或入口 ILA 路由器），也
可以是 ILA 地址（用于出口 ILA 路由器）。LOC 是 64 位定位符（格式为
W:X:Y:Z），它覆写目的地址的高 64 位。校验和 MODE 是 "no-action"、
"adj-transport"、"neutral-map" 与 "neutral-map-auto" 之一。若设置了
neutral-map，则 C 位将存在。标识符 TYPE 是 "luid" 或 "use-format" 之一。
在 use-format 的情况下，标识符类型字段存在，有效类型取自该字段。)

The usage of ila_xlat is:
(ila_xlat 的用法如下：)

ip ila add loc_match MATCH loc LOC csum-mode MODE ident-type TYPE

MATCH indicates the incoming locator that must be matched to apply
a the translaiton. LOC is the locator that overwrites the upper
sixty-four bits of the destination address. MODE and TYPE have the
same meanings as described above.
(MATCH 指示必须匹配才能应用转换的入站定位符。LOC 是覆写目的地址高 64
位的定位符。MODE 与 TYPE 的含义与上文所述相同。)


Some examples(一些示例)
=======================

::

     # Configure an ILA route that uses checksum neutral mapping as well
     # as type field. Note that the type field is set in the SIR address
     # (the 2000 implies type is 1 which is LUID).
     ip route add 3333:0:0:1:2000:0:1:87/128 encap ila 2001:0:87:0 \
	  csum-mode neutral-map ident-type use-format

     # Configure an ILA LWT route that uses auto checksum neutral mapping
     # (no C-bit) and configure identifier type to be LUID so that the
     # identifier type field will not be present.
     ip route add 3333:0:0:1:2000:0:2:87/128 encap ila 2001:0:87:1 \
	  csum-mode neutral-map-auto ident-type luid

     ila_xlat configuration

     # Configure an ILA to SIR mapping that matches a locator and overwrites
     # it with a SIR address (3333:0:0:1 in this example). The C-bit and
     # identifier field are used.
     ip ila add loc_match 2001:0:119:0 loc 3333:0:0:1 \
	 csum-mode neutral-map-auto ident-type use-format

     # Configure an ILA to SIR mapping where checksum neutral is automatically
     # set without the C-bit and the identifier type is configured to be LUID
     # so that the identifier type field is not present.
     ip ila add loc_match 2001:0:119:0 loc 3333:0:0:1 \
	 csum-mode neutral-map-auto ident-type use-format
