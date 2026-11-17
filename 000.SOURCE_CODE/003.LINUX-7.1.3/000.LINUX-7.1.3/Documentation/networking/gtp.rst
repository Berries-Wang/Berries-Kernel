.. SPDX-License-Identifier: GPL-2.0

=====================================
The Linux kernel GTP tunneling module
=====================================

(Linux 内核 GTP 隧道模块)

Documentation by
		 Harald Welte <laforge@gnumonks.org> and
		 Andreas Schultz <aschultz@tpip.net>

(文档作者：Harald Welte <laforge@gnumonks.org> 与 Andreas Schultz <aschultz@tpip.net>)

In 'drivers/net/gtp.c' you are finding a kernel-level implementation
of a GTP tunnel endpoint.
(在 'drivers/net/gtp.c' 中可以找到 GTP 隧道端点的内核级实现。)

What is GTP
(什么是 GTP)
===========

GTP is the Generic Tunnel Protocol, which is a 3GPP protocol used for
tunneling User-IP payload between a mobile station (phone, modem)
and the interconnection between an external packet data network (such
as the internet).
(GTP 即通用隧道协议 (Generic Tunnel Protocol)，是 3GPP 的一种协议，用于在移动台（手机、调制解调器）与外部分组数据网络（例如互联网）之间的互连中隧道传输用户 IP 载荷。)

So when you start a 'data connection' from your mobile phone, the
phone will use the control plane to signal for the establishment of
such a tunnel between that external data network and the phone.  The
tunnel endpoints thus reside on the phone and in the gateway.  All
intermediate nodes just transport the encapsulated packet.
(因此，当你从手机发起"数据连接"时，手机会使用控制面进行信令交互，在该外部数据网络与手机之间建立这样一条隧道。于是隧道端点位于手机和网关之中。所有中间节点只是转发封装后的数据包。)

The phone itself does not implement GTP but uses some other
technology-dependent protocol stack for transmitting the user IP
payload, such as LLC/SNDCP/RLC/MAC.
(手机本身并不实现 GTP，而是使用某种其他依赖于技术的协议栈来传输用户 IP 载荷，例如 LLC/SNDCP/RLC/MAC。)

At some network element inside the cellular operator infrastructure
(SGSN in case of GPRS/EGPRS or classic UMTS, hNodeB in case of a 3G
femtocell, eNodeB in case of 4G/LTE), the cellular protocol stacking
is translated into GTP *without breaking the end-to-end tunnel*.  So
intermediate nodes just perform some specific relay function.
(在蜂窝运营商基础设施内的某个网元处（GPRS/EGPRS 或经典 UMTS 中为 SGSN，3G 家庭基站中为 hNodeB，4G/LTE 中为 eNodeB），蜂窝协议栈会被转换为 GTP，*且不中断端到端隧道*。因此中间节点只执行某种特定的中继功能。)

At some point the GTP packet ends up on the so-called GGSN (GSM/UMTS)
or P-GW (LTE), which terminates the tunnel, decapsulates the packet
and forwards it onto an external packet data network.  This can be
public internet, but can also be any private IP network (or even
theoretically some non-IP network like X.25).
(最终 GTP 数据包会到达所谓的 GGSN（GSM/UMTS）或 P-GW（LTE），由它终止隧道、解封装数据包并将其转发到外部分组数据网络。这个网络可以是公共互联网，也可以是任何私有 IP 网络（甚至理论上可以是诸如 X.25 之类的非 IP 网络）。)

You can find the protocol specification in 3GPP TS 29.060, available
publicly via the 3GPP website at http://www.3gpp.org/DynaReport/29060.htm
(协议规范见 3GPP TS 29.060，可通过 3GPP 网站 http://www.3gpp.org/DynaReport/29060.htm 公开获取。)

A direct PDF link to v13.6.0 is provided for convenience below:
http://www.etsi.org/deliver/etsi_ts/129000_129099/129060/13.06.00_60/ts_129060v130600p.pdf
(为方便起见，下面提供 v13.6.0 的 PDF 直接链接：)

The Linux GTP tunnelling module
(Linux GTP 隧道模块)
===============================

The module implements the function of a tunnel endpoint, i.e. it is
able to decapsulate tunneled IP packets in the uplink originated by
the phone, and encapsulate raw IP packets received from the external
packet network in downlink towards the phone.
(该模块实现隧道端点的功能，即能够在上行方向对手机发出的已隧道化的 IP 数据包进行解封装，并在下行方向对从外部分组网络收到的原始 IP 数据包进行封装后发往手机。)

It *only* implements the so-called 'user plane', carrying the User-IP
payload, called GTP-U.  It does not implement the 'control plane',
which is a signaling protocol used for establishment and teardown of
GTP tunnels (GTP-C).
(它*只*实现所谓的"用户面"，即承载用户 IP 载荷的 GTP-U。它并不实现"控制面"，控制面是用于建立和拆除 GTP 隧道的信令协议（GTP-C）。)

So in order to have a working GGSN/P-GW setup, you will need a
userspace program that implements the GTP-C protocol and which then
uses the netlink interface provided by the GTP-U module in the kernel
to configure the kernel module.
(因此，要搭建一个可工作的 GGSN/P-GW 环境，你需要一个实现 GTP-C 协议的用户空间程序，该程序再使用内核中 GTP-U 模块提供的 netlink 接口来配置内核模块。)

This split architecture follows the tunneling modules of other
protocols, e.g. PPPoE or L2TP, where you also run a userspace daemon
to handle the tunnel establishment, authentication etc. and only the
data plane is accelerated inside the kernel.
(这种分离式架构与其他协议的隧道模块一致，例如 PPPoE 或 L2TP，在那里同样由用户空间守护进程处理隧道建立、认证等事务，只有数据面在内核内加速。)

Don't be confused by terminology:  The GTP User Plane goes through
kernel accelerated path, while the GTP Control Plane goes to
Userspace :)
(不要被术语搞糊涂：GTP 用户面走内核加速路径，而 GTP 控制面则进入用户空间。)

The official homepage of the module is at
https://osmocom.org/projects/linux-kernel-gtp-u/wiki
(该模块的官方主页位于 https://osmocom.org/projects/linux-kernel-gtp-u/wiki 。)

Userspace Programs with Linux Kernel GTP-U support
(支持 Linux 内核 GTP-U 的用户空间程序)
==================================================

At the time of this writing, there are at least two Free Software
implementations that implement GTP-C and can use the netlink interface
to make use of the Linux kernel GTP-U support:
(截至撰写本文时，至少有两个自由软件实现既实现了 GTP-C，又能使用 netlink 接口来利用 Linux 内核的 GTP-U 支持：)

* OpenGGSN (classic 2G/3G GGSN in C):
  (OpenGGSN(用 C 编写的经典 2G/3G GGSN)：)
  https://osmocom.org/projects/openggsn/wiki/OpenGGSN

* ergw (GGSN + P-GW in Erlang):
  (ergw(用 Erlang 编写的 GGSN + P-GW)：)
  https://github.com/travelping/ergw

Userspace Library / Command Line Utilities
(用户空间库 / 命令行工具)
==========================================

There is a userspace library called 'libgtpnl' which is based on
libmnl and which implements a C-language API towards the netlink
interface provided by the Kernel GTP module:
(有一个名为 'libgtpnl' 的用户空间库，它基于 libmnl，针对内核 GTP 模块提供的 netlink 接口实现了 C 语言 API：)

http://git.osmocom.org/libgtpnl/

Protocol Versions
(协议版本)
=================

There are two different versions of GTP-U: v0 [GSM TS 09.60] and v1
[3GPP TS 29.281].  Both are implemented in the Kernel GTP module.
Version 0 is a legacy version, and deprecated from recent 3GPP
specifications.
(GTP-U 有两个不同版本：v0 [GSM TS 09.60] 和 v1 [3GPP TS 29.281]。两者都在内核 GTP 模块中实现。版本 0 是遗留版本，在近期的 3GPP 规范中已被弃用。)

GTP-U uses UDP for transporting PDUs.  The receiving UDP port is 2151
for GTPv1-U and 3386 for GTPv0-U.
(GTP-U 使用 UDP 传输 PDU。GTPv1-U 的接收 UDP 端口为 2151，GTPv0-U 为 3386。)

There are three versions of GTP-C: v0, v1, and v2.  As the kernel
doesn't implement GTP-C, we don't have to worry about this.  It's the
responsibility of the control plane implementation in userspace to
implement that.
(GTP-C 有三个版本：v0、v1 和 v2。由于内核不实现 GTP-C，我们无需为此担心。实现它是用户空间中控制面实现的责任。)

IPv6
(IPv6)
====

The 3GPP specifications indicate either IPv4 or IPv6 can be used both
on the inner (user) IP layer, or on the outer (transport) layer.
(3GPP 规范指出，内层（用户）IP 层和外层（传输）层都可以使用 IPv4 或 IPv6。)

Unfortunately, the Kernel module currently supports IPv6 neither for
the User IP payload, nor for the outer IP layer.  Patches or other
Contributions to fix this are most welcome!
(遗憾的是，内核模块目前对用户 IP 载荷和外层 IP 层都不支持 IPv6。非常欢迎修复此问题的补丁或其他贡献！)

Mailing List
(邮件列表)
============

If you have questions regarding how to use the Kernel GTP module from
your own software, or want to contribute to the code, please use the
osmocom-net-grps mailing list for related discussion. The list can be
reached at osmocom-net-gprs@lists.osmocom.org and the mailman
interface for managing your subscription is at
https://lists.osmocom.org/mailman/listinfo/osmocom-net-gprs
(如果你对如何在自研软件中使用内核 GTP 模块有疑问，或想为代码做贡献，请使用 osmocom-net-grps 邮件列表进行相关讨论。该列表的地址为 osmocom-net-gprs@lists.osmocom.org，管理订阅的 mailman 界面位于 https://lists.osmocom.org/mailman/listinfo/osmocom-net-gprs 。)

Issue Tracker
(问题跟踪系统)
=============

The Osmocom project maintains an issue tracker for the Kernel GTP-U
module at
https://osmocom.org/projects/linux-kernel-gtp-u/issues
(Osmocom 项目在 https://osmocom.org/projects/linux-kernel-gtp-u/issues 为内核 GTP-U 模块维护着一个问题跟踪系统。)

History / Acknowledgements
(历史 / 致谢)
==========================

The Module was originally created in 2012 by Harald Welte, but never
completed.  Pablo came in to finish the mess Harald left behind.  But
doe to a lack of user interest, it never got merged.
(该模块最初由 Harald Welte 于 2012 年创建，但始终未完成。Pablo 接手来收拾 Harald 留下的烂摊子。但由于缺乏用户关注，它一直未被合并。)

In 2015, Andreas Schultz came to the rescue and fixed lots more bugs,
extended it with new features and finally pushed all of us to get it
mainline, where it was merged in 4.7.0.
(2015 年，Andreas Schultz 挺身而出，修复了大量更多的缺陷，为其扩展了新特性，并最终推动我们所有人将其送入主线，于 4.7.0 中合并。)

Architectural Details
(架构细节)
=====================

Local GTP-U entity and tunnel identification
(本地 GTP-U 实体与隧道标识)
--------------------------------------------

GTP-U uses UDP for transporting PDU's. The receiving UDP port is 2152
for GTPv1-U and 3386 for GTPv0-U.
(GTP-U 使用 UDP 传输 PDU。GTPv1-U 的接收 UDP 端口为 2152，GTPv0-U 为 3386。)

There is only one GTP-U entity (and therefore SGSN/GGSN/S-GW/PDN-GW
instance) per IP address. Tunnel Endpoint Identifier (TEID) are unique
per GTP-U entity.
(每个 IP 地址只有一个 GTP-U 实体（因而也只有一个 SGSN/GGSN/S-GW/PDN-GW 实例）。隧道端点标识 (TEID) 在每个 GTP-U 实体内是唯一的。)

A specific tunnel is only defined by the destination entity. Since the
destination port is constant, only the destination IP and TEID define
a tunnel. The source IP and Port have no meaning for the tunnel.
(一条特定的隧道仅由目的实体定义。由于目的端口是固定的，只有目的 IP 和 TEID 共同定义一条隧道。源 IP 和源端口对隧道没有意义。)

Therefore:
(因此：)

  * when sending, the remote entity is defined by the remote IP and
    the tunnel endpoint id. The source IP and port have no meaning and
    can be changed at any time.
    (发送时，远端实体由远端 IP 和隧道端点标识定义。源 IP 和端口没有意义，可以随时改变。)

  * when receiving the local entity is defined by the local
    destination IP and the tunnel endpoint id. The source IP and port
    have no meaning and can change at any time.
    (接收时，本地实体由本地目的 IP 和隧道端点标识定义。源 IP 和端口没有意义，可以随时改变。)

[3GPP TS 29.281] Section 4.3.0 defines this so::
([3GPP TS 29.281] 第 4.3.0 节对此定义如下：)

  The TEID in the GTP-U header is used to de-multiplex traffic
  incoming from remote tunnel endpoints so that it is delivered to the
  User plane entities in a way that allows multiplexing of different
  users, different packet protocols and different QoS levels.
  Therefore no two remote GTP-U endpoints shall send traffic to a
  GTP-U protocol entity using the same TEID value except
  for data forwarding as part of mobility procedures.

The definition above only defines that two remote GTP-U endpoints
*should not* send to the same TEID, it *does not* forbid or exclude
such a scenario. In fact, the mentioned mobility procedures make it
necessary that the GTP-U entity accepts traffic for TEIDs from
multiple or unknown peers.
(上面的定义只是说两个远端 GTP-U 端点*不应*向同一个 TEID 发送流量，它*并未*禁止或排除这种情形。事实上，前述的移动性流程使得 GTP-U 实体必须接受来自多个或未知对端的、针对这些 TEID 的流量。)

Therefore, the receiving side identifies tunnels exclusively based on
TEIDs, not based on the source IP!
(因此，接收侧完全基于 TEID 来识别隧道，而不基于源 IP！)

APN vs. Network Device
(APN 与网络设备)
======================

The GTP-U driver creates a Linux network device for each Gi/SGi
interface.
(GTP-U 驱动为每个 Gi/SGi 接口创建一个 Linux 网络设备。)

[3GPP TS 29.281] calls the Gi/SGi reference point an interface. This
may lead to the impression that the GGSN/P-GW can have only one such
interface.
([3GPP TS 29.281] 把 Gi/SGi 参考点称为一个接口。这可能让人误以为 GGSN/P-GW 只能有一个这样的接口。)

Correct is that the Gi/SGi reference point defines the interworking
between +the 3GPP packet domain (PDN) based on GTP-U tunnel and IP
based networks.
(正确的理解是：Gi/SGi 参考点定义了基于 GTP-U 隧道的 3GPP 分组域 (PDN) 与基于 IP 的网络之间的互通。)

There is no provision in any of the 3GPP documents that limits the
number of Gi/SGi interfaces implemented by a GGSN/P-GW.
(任何 3GPP 文档中都没有限制 GGSN/P-GW 所实现的 Gi/SGi 接口数量的规定。)

[3GPP TS 29.061] Section 11.3 makes it clear that the selection of a
specific Gi/SGi interfaces is made through the Access Point Name
(APN)::
([3GPP TS 29.061] 第 11.3 节阐明了特定 Gi/SGi 接口的选择是通过接入点名称 (APN) 完成的：)

  2. each private network manages its own addressing. In general this
     will result in different private networks having overlapping
     address ranges. A logically separate connection (e.g. an IP in IP
     tunnel or layer 2 virtual circuit) is used between the GGSN/P-GW
     and each private network.

     In this case the IP address alone is not necessarily unique.  The
     pair of values, Access Point Name (APN) and IPv4 address and/or
     IPv6 prefixes, is unique.

In order to support the overlapping address range use case, each APN
is mapped to a separate Gi/SGi interface (network device).
(为了支持地址范围重叠的场景，每个 APN 被映射到一个单独的 Gi/SGi 接口（网络设备）。)

.. note::

   The Access Point Name is purely a control plane (GTP-C) concept.
   At the GTP-U level, only Tunnel Endpoint Identifiers are present in
   GTP-U packets and network devices are known
   (注意：接入点名称 (Access Point Name) 纯粹是控制面 (GTP-C) 的概念。在 GTP-U 层面上，GTP-U 数据包中只出现隧道端点标识，网络设备是已知的。)

Therefore for a given UE the mapping in IP to PDN network is:
(因此，对于一个给定的 UE，从 IP 到 PDN 网络的映射为：)

  * network device + MS IP -> Peer IP + Peer TEID,
    (网络设备 + MS IP -> 对端 IP + 对端 TEID，)

and from PDN to IP network:
(而从 PDN 到 IP 网络则为：)

  * local GTP-U IP + TEID  -> network device
    (本地 GTP-U IP + TEID -> 网络设备)

Furthermore, before a received T-PDU is injected into the network
device the MS IP is checked against the IP recorded in PDP context.
(此外，在将接收到的 T-PDU 注入网络设备之前，会对照 PDP 上下文中记录的 IP 检查 MS IP。)
