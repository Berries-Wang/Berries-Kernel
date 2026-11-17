.. SPDX-License-Identifier: GPL-2.0

===========================
L2TP(L2TP (第二层隧道协议))
===========================

Layer 2 Tunneling Protocol (L2TP) allows L2 frames to be tunneled over
an IP network.
(第二层隧道协议 (L2TP) 允许在 IP 网络上隧道传输 L2 帧。)

This document covers the kernel's L2TP subsystem. It documents kernel
APIs for application developers who want to use the L2TP subsystem and
it provides some technical details about the internal implementation
which may be useful to kernel developers and maintainers.
(本文档介绍内核的 L2TP 子系统。它为想要使用 L2TP 子系统的应用程序开发者记录了内核
API，并提供一些关于内部实现的技术细节，这些内容可能对内核开发者和维护者有所帮助。)

Overview(概述)
==============

The kernel's L2TP subsystem implements the datapath for L2TPv2 and
L2TPv3. L2TPv2 is carried over UDP. L2TPv3 is carried over UDP or
directly over IP (protocol 115).
(内核的 L2TP 子系统实现了 L2TPv2 和 L2TPv3 的数据路径。L2TPv2 承载于 UDP 之上。
L2TPv3 承载于 UDP 之上，或直接承载于 IP (协议 115) 之上。)

The L2TP RFCs define two basic kinds of L2TP packets: control packets
(the "control plane"), and data packets (the "data plane"). The kernel
deals only with data packets. The more complex control packets are
handled by user space.
(L2TP RFC 定义了两种基本的 L2TP 数据包：控制包 ("控制平面") 和数据包 ("数据平面")。
内核只处理数据包。更为复杂的控制包由用户空间处理。)

An L2TP tunnel carries one or more L2TP sessions. Each tunnel is
associated with a socket. Each session is associated with a virtual
netdevice, e.g. ``pppN``, ``l2tpethN``, through which data frames pass
to/from L2TP. Fields in the L2TP header identify the tunnel or session
and whether it is a control or data packet. When tunnels and sessions
are set up using the Linux kernel API, we're just setting up the L2TP
data path. All aspects of the control protocol are to be handled by
user space.
(一个 L2TP 隧道承载一个或多个 L2TP 会话。每个隧道与一个套接字相关联。每个会话与一个
虚拟网络设备 (netdevice) 相关联，例如 ``pppN`` 和 ``l2tpethN`` ，数据帧通过它们进出
L2TP。L2TP 头部中的字段用于标识隧道或会话，以及它是控制包还是数据包。当使用 Linux
内核 API 建立隧道和会话时，我们只是在建立 L2TP 数据路径。控制协议的所有方面都由
用户空间处理。)

This split in responsibilities leads to a natural sequence of
operations when establishing tunnels and sessions. The procedure looks
like this:
(这种职责划分使得建立隧道和会话时有一个自然的操作顺序。该过程如下：)

    1) Create a tunnel socket. Exchange L2TP control protocol messages
       with the peer over that socket in order to establish a tunnel.
       (创建一个隧道套接字。通过该套接字与对端交换 L2TP 控制协议消息，以建立隧道。)

    2) Create a tunnel context in the kernel, using information
       obtained from the peer using the control protocol messages.
       (使用通过控制协议消息从对端获取的信息，在内核中创建隧道上下文。)

    3) Exchange L2TP control protocol messages with the peer over the
       tunnel socket in order to establish a session.
       (通过隧道套接字与对端交换 L2TP 控制协议消息，以建立一个会话。)

    4) Create a session context in the kernel using information
       obtained from the peer using the control protocol messages.
       (使用通过控制协议消息从对端获取的信息，在内核中创建会话上下文。)

L2TP APIs(L2TP API)
===================

This section documents each userspace API of the L2TP subsystem.
(本节介绍 L2TP 子系统的各个用户空间 API。)

Tunnel Sockets(隧道套接字)
--------------------------

L2TPv2 always uses UDP. L2TPv3 may use UDP or IP encapsulation.
(L2TPv2 总是使用 UDP。L2TPv3 可以使用 UDP 或 IP 封装。)

To create a tunnel socket for use by L2TP, the standard POSIX
socket API is used.
(要创建供 L2TP 使用的隧道套接字，需使用标准的 POSIX 套接字 API。)

For example, for a tunnel using IPv4 addresses and UDP encapsulation::
(例如，对于使用 IPv4 地址和 UDP 封装的隧道：)

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

Or for a tunnel using IPv6 addresses and IP encapsulation::
(或者，对于使用 IPv6 地址和 IP 封装的隧道：)

    int sockfd = socket(AF_INET6, SOCK_DGRAM, IPPROTO_L2TP);

UDP socket programming doesn't need to be covered here.
(UDP 套接字编程无需在此赘述。)

IPPROTO_L2TP is an IP protocol type implemented by the kernel's L2TP
subsystem. The L2TPIP socket address is defined in struct
sockaddr_l2tpip and struct sockaddr_l2tpip6 at
`include/uapi/linux/l2tp.h`_. The address includes the L2TP tunnel
(connection) id. To use L2TP IP encapsulation, an L2TPv3 application
should bind the L2TPIP socket using the locally assigned
tunnel id. When the peer's tunnel id and IP address is known, a
connect must be done.
(IPPROTO_L2TP 是由内核 L2TP 子系统实现的一种 IP 协议类型。L2TPIP 套接字地址定义在
`include/uapi/linux/l2tp.h`_ 处的 struct sockaddr_l2tpip 和 struct
sockaddr_l2tpip6 中。该地址包含 L2TP 隧道 (connection) id。要使用 L2TP IP 封装，
L2TPv3 应用程序应使用本地分配的隧道 id 绑定 L2TPIP 套接字。当对端的隧道 id 和 IP
地址已知时，必须执行 connect。)

If the L2TP application needs to handle L2TPv3 tunnel setup requests
from peers using L2TPIP, it must open a dedicated L2TPIP
socket to listen for those requests and bind the socket using tunnel
id 0 since tunnel setup requests are addressed to tunnel id 0.
(如果 L2TP 应用程序需要处理来自使用 L2TPIP 的对端的 L2TPv3 隧道建立请求，它必须
打开一个专用的 L2TPIP 套接字来监听这些请求，并使用隧道 id 0 绑定该套接字，因为隧道
建立请求以隧道 id 0 为目的地址。)

An L2TP tunnel and all of its sessions are automatically closed when
its tunnel socket is closed.
(当隧道套接字关闭时，L2TP 隧道及其所有会话会自动关闭。)

Netlink API(Netlink API)
------------------------

L2TP applications use netlink to manage L2TP tunnel and session
instances in the kernel. The L2TP netlink API is defined in
`include/uapi/linux/l2tp.h`_.
(L2TP 应用程序使用 netlink 来管理内核中的 L2TP 隧道和会话实例。L2TP netlink API
定义在 `include/uapi/linux/l2tp.h`_ 中。)

L2TP uses `Generic Netlink`_ (GENL). Several commands are defined:
Create, Delete, Modify and Get for tunnel and session
instances, e.g. ``L2TP_CMD_TUNNEL_CREATE``. The API header lists the
netlink attribute types that can be used with each command.
(L2TP 使用 `Generic Netlink`_ (GENL)。定义了若干命令：针对隧道和会话实例的
Create、Delete、Modify 和 Get，例如 ``L2TP_CMD_TUNNEL_CREATE`` 。API 头文件列出了
可与每个命令一起使用的 netlink 属性类型。)

Tunnel and session instances are identified by a locally unique
32-bit id.  L2TP tunnel ids are given by ``L2TP_ATTR_CONN_ID`` and
``L2TP_ATTR_PEER_CONN_ID`` attributes and L2TP session ids are given
by ``L2TP_ATTR_SESSION_ID`` and ``L2TP_ATTR_PEER_SESSION_ID``
attributes. If netlink is used to manage L2TPv2 tunnel and session
instances, the L2TPv2 16-bit tunnel/session id is cast to a 32-bit
value in these attributes.
(隧道和会话实例由本地唯一的 32 位 id 标识。L2TP 隧道 id 由 ``L2TP_ATTR_CONN_ID``
和 ``L2TP_ATTR_PEER_CONN_ID`` 属性给出，L2TP 会话 id 由
``L2TP_ATTR_SESSION_ID`` 和 ``L2TP_ATTR_PEER_SESSION_ID`` 属性给出。如果使用
netlink 管理 L2TPv2 隧道和会话实例，则 L2TPv2 的 16 位隧道/会话 id 在这些属性中被
强制转换为 32 位值。)

In the ``L2TP_CMD_TUNNEL_CREATE`` command, ``L2TP_ATTR_FD`` tells the
kernel the tunnel socket fd being used. If not specified, the kernel
creates a kernel socket for the tunnel, using IP parameters set in
``L2TP_ATTR_IP[6]_SADDR``, ``L2TP_ATTR_IP[6]_DADDR``,
``L2TP_ATTR_UDP_SPORT``, ``L2TP_ATTR_UDP_DPORT`` attributes. Kernel
sockets are used to implement unmanaged L2TPv3 tunnels (iproute2's "ip
l2tp" commands). If ``L2TP_ATTR_FD`` is given, it must be a socket fd
that is already bound and connected. There is more information about
unmanaged tunnels later in this document.
(在 ``L2TP_CMD_TUNNEL_CREATE`` 命令中，``L2TP_ATTR_FD`` 告诉内核正在使用的隧道套接字
fd。如果未指定，内核会使用 ``L2TP_ATTR_IP[6]_SADDR`` 、 ``L2TP_ATTR_IP[6]_DADDR`` 、
``L2TP_ATTR_UDP_SPORT`` 、 ``L2TP_ATTR_UDP_DPORT`` 属性中设置的 IP 参数为该隧道创建
一个内核套接字。内核套接字用于实现非托管 (unmanaged) L2TPv3 隧道 (iproute2 的
"ip l2tp" 命令)。如果给出了 ``L2TP_ATTR_FD`` ，它必须是一个已经绑定并连接的套接字
fd。关于非托管隧道的更多信息见本文档后文。)

``L2TP_CMD_TUNNEL_CREATE`` attributes:-
(``L2TP_CMD_TUNNEL_CREATE`` 的属性：-)

================== ======== ===
Attribute (属性)   Required Use (用途)
================== ======== ===
CONN_ID            Y        Sets the tunnel (connection) id. (设置隧道 (connection) id。)
PEER_CONN_ID       Y        Sets the peer tunnel (connection) id. (设置对端隧道 (connection) id。)
PROTO_VERSION      Y        Protocol version. 2 or 3. (协议版本。2 或 3。)
ENCAP_TYPE         Y        Encapsulation type: UDP or IP. (封装类型：UDP 或 IP。)
FD                 N        Tunnel socket file descriptor. (隧道套接字文件描述符。)
UDP_CSUM           N        Enable IPv4 UDP checksums. Used only if FD is
                            not set. (启用 IPv4 UDP 校验和。仅在未设置 FD 时使用。)
UDP_ZERO_CSUM6_TX  N        Zero IPv6 UDP checksum on transmit. Used only
                            if FD is not set. (发送时将 IPv6 UDP 校验和置零。仅在未设置 FD 时使用。)
UDP_ZERO_CSUM6_RX  N        Zero IPv6 UDP checksum on receive. Used only if
                            FD is not set. (接收时将 IPv6 UDP 校验和置零。仅在未设置 FD 时使用。)
IP_SADDR           N        IPv4 source address. Used only if FD is not
                            set. (IPv4 源地址。仅在未设置 FD 时使用。)
IP_DADDR           N        IPv4 destination address. Used only if FD is
                            not set. (IPv4 目的地址。仅在未设置 FD 时使用。)
UDP_SPORT          N        UDP source port. Used only if FD is not set. (UDP 源端口。仅在未设置 FD 时使用。)
UDP_DPORT          N        UDP destination port. Used only if FD is not
                            set. (UDP 目的端口。仅在未设置 FD 时使用。)
IP6_SADDR          N        IPv6 source address. Used only if FD is not
                            set. (IPv6 源地址。仅在未设置 FD 时使用。)
IP6_DADDR          N        IPv6 destination address. Used only if FD is
                            not set. (IPv6 目的地址。仅在未设置 FD 时使用。)
DEBUG              N        Debug flags. (调试标志。)
================== ======== ===

``L2TP_CMD_TUNNEL_DESTROY`` attributes:-
(``L2TP_CMD_TUNNEL_DESTROY`` 的属性：-)

================== ======== ===
Attribute (属性)   Required Use (用途)
================== ======== ===
CONN_ID            Y        Identifies the tunnel id to be destroyed. (标识要销毁的隧道 id。)
================== ======== ===

``L2TP_CMD_TUNNEL_MODIFY`` attributes:-
(``L2TP_CMD_TUNNEL_MODIFY`` 的属性：-)

================== ======== ===
Attribute (属性)   Required Use (用途)
================== ======== ===
CONN_ID            Y        Identifies the tunnel id to be modified. (标识要修改的隧道 id。)
DEBUG              N        Debug flags. (调试标志。)
================== ======== ===

``L2TP_CMD_TUNNEL_GET`` attributes:-
(``L2TP_CMD_TUNNEL_GET`` 的属性：-)

================== ======== ===
Attribute (属性)   Required Use (用途)
================== ======== ===
CONN_ID            N        Identifies the tunnel id to be queried.
                            Ignored in DUMP requests. (标识要查询的隧道 id。在 DUMP 请求中被忽略。)
================== ======== ===

``L2TP_CMD_SESSION_CREATE`` attributes:-
(``L2TP_CMD_SESSION_CREATE`` 的属性：-)

================== ======== ===
Attribute (属性)   Required Use (用途)
================== ======== ===
CONN_ID            Y        The parent tunnel id. (父隧道 id。)
SESSION_ID         Y        Sets the session id. (设置会话 id。)
PEER_SESSION_ID    Y        Sets the parent session id. (设置父会话 id。)
PW_TYPE            Y        Sets the pseudowire type. (设置伪线 (pseudowire) 类型。)
DEBUG              N        Debug flags. (调试标志。)
RECV_SEQ           N        Enable rx data sequence numbers. (启用接收方向的数据序列号。)
SEND_SEQ           N        Enable tx data sequence numbers. (启用发送方向的数据序列号。)
LNS_MODE           N        Enable LNS mode (auto-enable data sequence
                            numbers). (启用 LNS 模式 (自动启用数据序列号)。)
RECV_TIMEOUT       N        Timeout to wait when reordering received
                            packets. (对接收到的数据包重新排序时的等待超时。)
L2SPEC_TYPE        N        Sets layer2-specific-sublayer type (L2TPv3
                            only). (设置 layer2-specific-sublayer 类型 (仅限 L2TPv3)。)
COOKIE             N        Sets optional cookie (L2TPv3 only). (设置可选的 cookie (仅限 L2TPv3)。)
PEER_COOKIE        N        Sets optional peer cookie (L2TPv3 only). (设置可选的对端 cookie (仅限 L2TPv3)。)
IFNAME             N        Sets interface name (L2TPv3 only). (设置接口名 (仅限 L2TPv3)。)
================== ======== ===

For Ethernet session types, this will create an l2tpeth virtual
interface which can then be configured as required. For PPP session
types, a PPPoL2TP socket must also be opened and connected, mapping it
onto the new session. This is covered in "PPPoL2TP Sockets" later.
(对于以太网会话类型，这将创建一个 l2tpeth 虚拟接口，之后可以按需对其进行配置。对于
PPP 会话类型，还必须打开并连接一个 PPPoL2TP 套接字，将其映射到新会话上。这将在后文
"PPPoL2TP Sockets" 一节中介绍。)

``L2TP_CMD_SESSION_DESTROY`` attributes:-
(``L2TP_CMD_SESSION_DESTROY`` 的属性：-)

================== ======== ===
Attribute (属性)   Required Use (用途)
================== ======== ===
CONN_ID            Y        Identifies the parent tunnel id of the session
                            to be destroyed. (标识要销毁的会话的父隧道 id。)
SESSION_ID         Y        Identifies the session id to be destroyed. (标识要销毁的会话 id。)
IFNAME             N        Identifies the session by interface name. If
                            set, this overrides any CONN_ID and SESSION_ID
                            attributes. Currently supported for L2TPv3
                            Ethernet sessions only. (通过接口名标识会话。如果设置，将覆盖任何 CONN_ID 和 SESSION_ID 属性。目前仅支持 L2TPv3 以太网会话。)
================== ======== ===

``L2TP_CMD_SESSION_MODIFY`` attributes:-
(``L2TP_CMD_SESSION_MODIFY`` 的属性：-)

================== ======== ===
Attribute (属性)   Required Use (用途)
================== ======== ===
CONN_ID            Y        Identifies the parent tunnel id of the session
                            to be modified. (标识要修改的会话的父隧道 id。)
SESSION_ID         Y        Identifies the session id to be modified. (标识要修改的会话 id。)
IFNAME             N        Identifies the session by interface name. If
                            set, this overrides any CONN_ID and SESSION_ID
                            attributes. Currently supported for L2TPv3
                            Ethernet sessions only. (通过接口名标识会话。如果设置，将覆盖任何 CONN_ID 和 SESSION_ID 属性。目前仅支持 L2TPv3 以太网会话。)
DEBUG              N        Debug flags. (调试标志。)
RECV_SEQ           N        Enable rx data sequence numbers. (启用接收方向的数据序列号。)
SEND_SEQ           N        Enable tx data sequence numbers. (启用发送方向的数据序列号。)
LNS_MODE           N        Enable LNS mode (auto-enable data sequence
                            numbers). (启用 LNS 模式 (自动启用数据序列号)。)
RECV_TIMEOUT       N        Timeout to wait when reordering received
                            packets. (对接收到的数据包重新排序时的等待超时。)
================== ======== ===

``L2TP_CMD_SESSION_GET`` attributes:-
(``L2TP_CMD_SESSION_GET`` 的属性：-)

================== ======== ===
Attribute (属性)   Required Use (用途)
================== ======== ===
CONN_ID            N        Identifies the tunnel id to be queried.
                            Ignored for DUMP requests. (标识要查询的隧道 id。在 DUMP 请求中被忽略。)
SESSION_ID         N        Identifies the session id to be queried.
                            Ignored for DUMP requests. (标识要查询的会话 id。在 DUMP 请求中被忽略。)
IFNAME             N        Identifies the session by interface name.
                            If set, this overrides any CONN_ID and
                            SESSION_ID attributes. Ignored for DUMP
                            requests. Currently supported for L2TPv3
                            Ethernet sessions only. (通过接口名标识会话。如果设置，将覆盖任何 CONN_ID 和 SESSION_ID 属性。在 DUMP 请求中被忽略。目前仅支持 L2TPv3 以太网会话。)
================== ======== ===

Application developers should refer to `include/uapi/linux/l2tp.h`_ for
netlink command and attribute definitions.
(应用程序开发者应参考 `include/uapi/linux/l2tp.h`_ 以了解 netlink 命令和属性的定义。)

Sample userspace code using libmnl_:
(使用 libmnl_ 的用户空间示例代码：)

  - Open L2TP netlink socket::
    (打开 L2TP netlink 套接字：)

        struct nl_sock *nl_sock;
        int l2tp_nl_family_id;

        nl_sock = nl_socket_alloc();
        genl_connect(nl_sock);
        genl_id = genl_ctrl_resolve(nl_sock, L2TP_GENL_NAME);

  - Create a tunnel::
    (创建一个隧道：)

        struct nlmsghdr *nlh;
        struct genlmsghdr *gnlh;

        nlh = mnl_nlmsg_put_header(buf);
        nlh->nlmsg_type = genl_id; /* assigned to genl socket */
        nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
        nlh->nlmsg_seq = seq;

        gnlh = mnl_nlmsg_put_extra_header(nlh, sizeof(*gnlh));
        gnlh->cmd = L2TP_CMD_TUNNEL_CREATE;
        gnlh->version = L2TP_GENL_VERSION;
        gnlh->reserved = 0;

        mnl_attr_put_u32(nlh, L2TP_ATTR_FD, tunl_sock_fd);
        mnl_attr_put_u32(nlh, L2TP_ATTR_CONN_ID, tid);
        mnl_attr_put_u32(nlh, L2TP_ATTR_PEER_CONN_ID, peer_tid);
        mnl_attr_put_u8(nlh, L2TP_ATTR_PROTO_VERSION, protocol_version);
        mnl_attr_put_u16(nlh, L2TP_ATTR_ENCAP_TYPE, encap);

  - Create a session::
    (创建一个会话：)

        struct nlmsghdr *nlh;
        struct genlmsghdr *gnlh;

        nlh = mnl_nlmsg_put_header(buf);
        nlh->nlmsg_type = genl_id; /* assigned to genl socket */
        nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
        nlh->nlmsg_seq = seq;

        gnlh = mnl_nlmsg_put_extra_header(nlh, sizeof(*gnlh));
        gnlh->cmd = L2TP_CMD_SESSION_CREATE;
        gnlh->version = L2TP_GENL_VERSION;
        gnlh->reserved = 0;

        mnl_attr_put_u32(nlh, L2TP_ATTR_CONN_ID, tid);
        mnl_attr_put_u32(nlh, L2TP_ATTR_PEER_CONN_ID, peer_tid);
        mnl_attr_put_u32(nlh, L2TP_ATTR_SESSION_ID, sid);
        mnl_attr_put_u32(nlh, L2TP_ATTR_PEER_SESSION_ID, peer_sid);
        mnl_attr_put_u16(nlh, L2TP_ATTR_PW_TYPE, pwtype);
        /* there are other session options which can be set using netlink
         * attributes during session creation -- see l2tp.h
         */

  - Delete a session::
    (删除一个会话：)

        struct nlmsghdr *nlh;
        struct genlmsghdr *gnlh;

        nlh = mnl_nlmsg_put_header(buf);
        nlh->nlmsg_type = genl_id; /* assigned to genl socket */
        nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
        nlh->nlmsg_seq = seq;

        gnlh = mnl_nlmsg_put_extra_header(nlh, sizeof(*gnlh));
        gnlh->cmd = L2TP_CMD_SESSION_DELETE;
        gnlh->version = L2TP_GENL_VERSION;
        gnlh->reserved = 0;

        mnl_attr_put_u32(nlh, L2TP_ATTR_CONN_ID, tid);
        mnl_attr_put_u32(nlh, L2TP_ATTR_SESSION_ID, sid);

  - Delete a tunnel and all of its sessions (if any)::
    (删除一个隧道及其所有会话 (如有)：)

        struct nlmsghdr *nlh;
        struct genlmsghdr *gnlh;

        nlh = mnl_nlmsg_put_header(buf);
        nlh->nlmsg_type = genl_id; /* assigned to genl socket */
        nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
        nlh->nlmsg_seq = seq;

        gnlh = mnl_nlmsg_put_extra_header(nlh, sizeof(*gnlh));
        gnlh->cmd = L2TP_CMD_TUNNEL_DELETE;
        gnlh->version = L2TP_GENL_VERSION;
        gnlh->reserved = 0;

        mnl_attr_put_u32(nlh, L2TP_ATTR_CONN_ID, tid);

PPPoL2TP Session Socket API(PPPoL2TP 会话套接字 API)
----------------------------------------------------

For PPP session types, a PPPoL2TP socket must be opened and connected
to the L2TP session.
(对于 PPP 会话类型，必须打开一个 PPPoL2TP 套接字并连接到该 L2TP 会话。)

When creating PPPoL2TP sockets, the application provides information
to the kernel about the tunnel and session in a socket connect()
call. Source and destination tunnel and session ids are provided, as
well as the file descriptor of a UDP or L2TPIP socket. See struct
pppol2tp_addr in `include/linux/if_pppol2tp.h`_. For historical reasons,
there are unfortunately slightly different address structures for
L2TPv2/L2TPv3 IPv4/IPv6 tunnels and userspace must use the appropriate
structure that matches the tunnel socket type.
(创建 PPPoL2TP 套接字时，应用程序在套接字 connect() 调用中向内核提供关于隧道和会话
的信息。需要提供源和目的隧道与 会话 id，以及 UDP 或 L2TPIP 套接字的文件描述符。参见
`include/linux/if_pppol2tp.h`_ 中的 struct pppol2tp_addr。由于历史原因，L2TPv2/
L2TPv3 IPv4/IPv6 隧道的地址结构不幸地略有不同，用户空间必须使用与隧道套接字类型
匹配的相应结构。)

Userspace may control behavior of the tunnel or session using
setsockopt and ioctl on the PPPoX socket. The following socket
options are supported:-
(用户空间可以通过在 PPPoX 套接字上执行 setsockopt 和 ioctl 来控制隧道或会话的行为。
支持以下套接字选项：-)

=========   ===========================================================
DEBUG       bitmask of debug message categories. See below.
            (调试消息类别的位掩码。见下文。)
SENDSEQ     - 0 => don't send packets with sequence numbers
            - 1 => send packets with sequence numbers
            (0 表示不发送带序列号的数据包；1 表示发送带序列号的数据包。)
RECVSEQ     - 0 => receive packet sequence numbers are optional
            - 1 => drop receive packets without sequence numbers
            (0 表示接收数据包的序列号是可选的；1 表示丢弃不带序列号的接收数据包。)
LNSMODE     - 0 => act as LAC.
            - 1 => act as LNS.
            (0 表示作为 LAC 运行；1 表示作为 LNS 运行。)
REORDERTO   reorder timeout (in millisecs). If 0, don't try to reorder.
            (重排序超时 (毫秒)。如果为 0，则不尝试重排序。)
=========   ===========================================================

In addition to the standard PPP ioctls, a PPPIOCGL2TPSTATS is provided
to retrieve tunnel and session statistics from the kernel using the
PPPoX socket of the appropriate tunnel or session.
(除了标准的 PPP ioctl 之外，还提供了 PPPIOCGL2TPSTATS，用于通过相应隧道或会话的
PPPoX 套接字从内核获取隧道和会话统计信息。)

Sample userspace code:
(用户空间示例代码：)

  - Create session PPPoX data socket::
    (创建会话 PPPoX 数据套接字：)

        /* Input: the L2TP tunnel UDP socket `tunnel_fd`, which needs to be
         * bound already (both sockname and peername), otherwise it will not be
         * ready.
         */

        struct sockaddr_pppol2tp sax;
        int session_fd;
        int ret;

        session_fd = socket(AF_PPPOX, SOCK_DGRAM, PX_PROTO_OL2TP);
        if (session_fd < 0)
                return -errno;

        sax.sa_family = AF_PPPOX;
        sax.sa_protocol = PX_PROTO_OL2TP;
        sax.pppol2tp.fd = tunnel_fd;
        sax.pppol2tp.addr.sin_addr.s_addr = addr->sin_addr.s_addr;
        sax.pppol2tp.addr.sin_port = addr->sin_port;
        sax.pppol2tp.addr.sin_family = AF_INET;
        sax.pppol2tp.s_tunnel  = tunnel_id;
        sax.pppol2tp.s_session = session_id;
        sax.pppol2tp.d_tunnel  = peer_tunnel_id;
        sax.pppol2tp.d_session = peer_session_id;

        /* session_fd is the fd of the session's PPPoL2TP socket.
         * tunnel_fd is the fd of the tunnel UDP / L2TPIP socket.
         */
        ret = connect(session_fd, (struct sockaddr *)&sax, sizeof(sax));
        if (ret < 0 ) {
                close(session_fd);
                return -errno;
        }

        return session_fd;

L2TP control packets will still be available for read on `tunnel_fd`.
(L2TP 控制包仍然可以在 `tunnel_fd` 上读取。)

  - Create PPP channel::
    (创建 PPP 通道：)

        /* Input: the session PPPoX data socket `session_fd` which was created
         * as described above.
         */

        int ppp_chan_fd;
        int chindx;
        int ret;

        ret = ioctl(session_fd, PPPIOCGCHAN, &chindx);
        if (ret < 0)
                return -errno;

        ppp_chan_fd = open("/dev/ppp", O_RDWR);
        if (ppp_chan_fd < 0)
                return -errno;

        ret = ioctl(ppp_chan_fd, PPPIOCATTCHAN, &chindx);
        if (ret < 0) {
                close(ppp_chan_fd);
                return -errno;
        }

        return ppp_chan_fd;

LCP PPP frames will be available for read on `ppp_chan_fd`.
(LCP PPP 帧仍然可以在 `ppp_chan_fd` 上读取。)

  - Create PPP interface::
    (创建 PPP 接口：)

        /* Input: the PPP channel `ppp_chan_fd` which was created as described
         * above.
         */

        int ifunit = -1;
        int ppp_if_fd;
        int ret;

        ppp_if_fd = open("/dev/ppp", O_RDWR);
        if (ppp_if_fd < 0)
                return -errno;

        ret = ioctl(ppp_if_fd, PPPIOCNEWUNIT, &ifunit);
        if (ret < 0) {
                close(ppp_if_fd);
                return -errno;
        }

        ret = ioctl(ppp_chan_fd, PPPIOCCONNECT, &ifunit);
        if (ret < 0) {
                close(ppp_if_fd);
                return -errno;
        }

        return ppp_if_fd;

IPCP/IPv6CP PPP frames will be available for read on `ppp_if_fd`.
(IPCP/IPv6CP PPP 帧仍然可以在 `ppp_if_fd` 上读取。)

The ppp<ifunit> interface can then be configured as usual with netlink's
RTM_NEWLINK, RTM_NEWADDR, RTM_NEWROUTE, or ioctl's SIOCSIFMTU, SIOCSIFADDR,
SIOCSIFDSTADDR, SIOCSIFNETMASK, SIOCSIFFLAGS, or with the `ip` command.
(之后即可照常配置 ppp<ifunit> 接口：使用 netlink 的 RTM_NEWLINK、RTM_NEWADDR、
RTM_NEWROUTE，或 ioctl 的 SIOCSIFMTU、SIOCSIFADDR、SIOCSIFDSTADDR、SIOCSIFNETMASK、
SIOCSIFFLAGS，或使用 `ip` 命令。)

  - Bridging L2TP sessions which have PPP pseudowire types (this is also called
    L2TP tunnel switching or L2TP multihop) is supported by bridging the PPP
    channels of the two L2TP sessions to be bridged::
    (要桥接具有 PPP 伪线类型的 L2TP 会话 (这也称为 L2TP 隧道交换或 L2TP 多跳)，可以
    将要桥接的两个 L2TP 会话的 PPP 通道桥接起来：)

        /* Input: the session PPPoX data sockets `session_fd1` and `session_fd2`
         * which were created as described further above.
         */

        int ppp_chan_fd;
        int chindx1;
        int chindx2;
        int ret;

        ret = ioctl(session_fd1, PPPIOCGCHAN, &chindx1);
        if (ret < 0)
                return -errno;

        ret = ioctl(session_fd2, PPPIOCGCHAN, &chindx2);
        if (ret < 0)
                return -errno;

        ppp_chan_fd = open("/dev/ppp", O_RDWR);
        if (ppp_chan_fd < 0)
                return -errno;

        ret = ioctl(ppp_chan_fd, PPPIOCATTCHAN, &chindx1);
        if (ret < 0) {
                close(ppp_chan_fd);
                return -errno;
        }

        ret = ioctl(ppp_chan_fd, PPPIOCBRIDGECHAN, &chindx2);
        close(ppp_chan_fd);
        if (ret < 0)
                return -errno;

        return 0;

It can be noted that when bridging PPP channels, the PPP session is not locally
terminated, and no local PPP interface is created.  PPP frames arriving on one
channel are directly passed to the other channel, and vice versa.
(可以注意到，在桥接 PPP 通道时，PPP 会话不会在本地终结，也不会创建本地 PPP 接口。
从一个通道到达的 PPP 帧被直接传递到另一个通道，反之亦然。)

The PPP channel does not need to be kept open.  Only the session PPPoX data
sockets need to be kept open.
(PPP 通道不需要保持打开。只需要保持会话 PPPoX 数据套接字打开。)

More generally, it is also possible in the same way to e.g. bridge a PPPoL2TP
PPP channel with other types of PPP channels, such as PPPoE.
(更一般地说，同样可以用这种方式将 PPPoL2TP PPP 通道与其他类型的 PPP 通道桥接，
例如 PPPoE。)

See more details for the PPP side in ppp_generic.rst.
(PPP 一侧的更多细节见 ppp_generic.rst。)

Old L2TPv2-only API(旧版仅 L2TPv2 的 API)
-----------------------------------------

When L2TP was first added to the Linux kernel in 2.6.23, it
implemented only L2TPv2 and did not include a netlink API. Instead,
tunnel and session instances in the kernel were managed directly using
only PPPoL2TP sockets. The PPPoL2TP socket is used as described in
section "PPPoL2TP Session Socket API" but tunnel and session instances
are automatically created on a connect() of the socket instead of
being created by a separate netlink request:
(当 L2TP 在 2.6.23 中首次加入 Linux 内核时，它只实现了 L2TPv2，并且没有包含
netlink API。当时内核中的隧道和会话实例仅使用 PPPoL2TP 套接字直接管理。
PPPoL2TP 套接字的使用方式如 "PPPoL2TP Session Socket API" 一节所述，但隧道和会话
实例是在套接字 connect() 时自动创建的，而不是通过单独的 netlink 请求创建：)

    - Tunnels are managed using a tunnel management socket which is a
      dedicated PPPoL2TP socket, connected to (invalid) session
      id 0. The L2TP tunnel instance is created when the PPPoL2TP
      tunnel management socket is connected and is destroyed when the
      socket is closed.
      (隧道通过一个隧道管理套接字进行管理，该套接字是一个专用的 PPPoL2TP 套接字，
      连接到 (无效的) 会话 id 0。L2TP 隧道实例在 PPPoL2TP 隧道管理套接字连接时创建，
      并在套接字关闭时销毁。)

    - Session instances are created in the kernel when a PPPoL2TP
      socket is connected to a non-zero session id. Session parameters
      are set using setsockopt. The L2TP session instance is destroyed
      when the socket is closed.
      (当 PPPoL2TP 套接字连接到非零会话 id 时，会在内核中创建会话实例。会话参数通过
      setsockopt 设置。L2TP 会话实例在套接字关闭时销毁。)

This API is still supported but its use is discouraged. Instead, new
L2TPv2 applications should use netlink to first create the tunnel and
session, then create a PPPoL2TP socket for the session.
(此 API 仍然受支持，但不鼓励使用。相反，新的 L2TPv2 应用程序应使用 netlink 先创建
隧道和会话，然后为该会话创建一个 PPPoL2TP 套接字。)

Unmanaged L2TPv3 tunnels(非托管 L2TPv3 隧道)
--------------------------------------------

The kernel L2TP subsystem also supports static (unmanaged) L2TPv3
tunnels. Unmanaged tunnels have no userspace tunnel socket, and
exchange no control messages with the peer to set up the tunnel; the
tunnel is configured manually at each end of the tunnel. All
configuration is done using netlink. There is no need for an L2TP
userspace application in this case -- the tunnel socket is created by
the kernel and configured using parameters sent in the
``L2TP_CMD_TUNNEL_CREATE`` netlink request. The ``ip`` utility of
``iproute2`` has commands for managing static L2TPv3 tunnels; do ``ip
l2tp help`` for more information.
(内核 L2TP 子系统还支持静态 (非托管) L2TPv3 隧道。非托管隧道没有用户空间隧道套接字，
也不与对端交换控制消息来建立隧道；隧道在每一端手动配置。所有配置都通过 netlink 完成。
在这种情况下不需要 L2TP 用户空间应用程序 —— 隧道套接字由内核创建，并使用
``L2TP_CMD_TUNNEL_CREATE`` netlink 请求中发送的参数进行配置。 ``iproute2`` 的
``ip`` 工具提供了管理静态 L2TPv3 隧道的命令；执行 ``ip l2tp help`` 以了解更多信息。)

Debugging(调试)
---------------

The L2TP subsystem offers a range of debugging interfaces through the
debugfs filesystem.
(L2TP 子系统通过 debugfs 文件系统提供了一系列调试接口。)

To access these interfaces, the debugfs filesystem must first be mounted::
(要访问这些接口，必须先挂载 debugfs 文件系统：)

    # mount -t debugfs debugfs /debug

Files under the l2tp directory can then be accessed, providing a summary
of the current population of tunnel and session contexts existing in the
kernel::
(之后即可访问 l2tp 目录下的文件，其中汇总了内核中当前存在的隧道和会话上下文：)

    # cat /debug/l2tp/tunnels

The debugfs files should not be used by applications to obtain L2TP
state information because the file format is subject to change. It is
implemented to provide extra debug information to help diagnose
problems. Applications should instead use the netlink API.
(应用程序不应使用这些 debugfs 文件来获取 L2TP 状态信息，因为文件格式可能会变化。
实现这些文件是为了提供额外的调试信息以帮助诊断问题。应用程序应改用 netlink API。)

In addition the L2TP subsystem implements tracepoints using the standard
kernel event tracing API.  The available L2TP events can be reviewed as
follows::
(此外，L2TP 子系统使用标准的内核事件跟踪 API 实现了跟踪点 (tracepoints)。可按如下
方式查看可用的 L2TP 事件：)

    # find /debug/tracing/events/l2tp

Finally, /proc/net/pppol2tp is also provided for backwards compatibility
with the original pppol2tp code. It lists information about L2TPv2
tunnels and sessions only. Its use is discouraged.
(最后，还提供了 /proc/net/pppol2tp 以向后兼容原始的 pppol2tp 代码。它只列出关于
L2TPv2 隧道和会话的信息。不鼓励使用它。)

Internal Implementation(内部实现)
=================================

This section is for kernel developers and maintainers.
(本节面向内核开发者和维护者。)

Sockets(套接字)
---------------

UDP sockets are implemented by the networking core. When an L2TP
tunnel is created using a UDP socket, the socket is set up as an
encapsulated UDP socket by setting encap_rcv and encap_destroy
callbacks on the UDP socket. l2tp_udp_encap_recv is called when
packets are received on the socket. l2tp_udp_encap_destroy is called
when userspace closes the socket.
(UDP 套接字由网络核心实现。当使用 UDP 套接字创建 L2TP 隧道时，通过在 UDP 套接字上
设置 encap_rcv 和 encap_destroy 回调，将该套接字设置为封装式 UDP 套接字。当在该
套接字上收到数据包时调用 l2tp_udp_encap_recv。当用户空间关闭套接字时调用
l2tp_udp_encap_destroy。)

L2TPIP sockets are implemented in `net/l2tp/l2tp_ip.c`_ and
`net/l2tp/l2tp_ip6.c`_.
(L2TPIP 套接字实现于 `net/l2tp/l2tp_ip.c`_ 和 `net/l2tp/l2tp_ip6.c`_ 中。)

Tunnels(隧道)
-------------

The kernel keeps a struct l2tp_tunnel context per L2TP tunnel. The
l2tp_tunnel is always associated with a UDP or L2TP/IP socket and
keeps a list of sessions in the tunnel. When a tunnel is first
registered with L2TP core, the reference count on the socket is
increased. This ensures that the socket cannot be removed while L2TP's
data structures reference it.
(内核为每个 L2TP 隧道保留一个 struct l2tp_tunnel 上下文。l2tp_tunnel 总是与一个
UDP 或 L2TP/IP 套接字相关联，并保存该隧道中会话的列表。当隧道首次注册到 L2TP 核心
时，套接字的引用计数会增加。这确保了在 L2TP 的数据结构引用该套接字期间，套接字不会被
移除。)

Tunnels are identified by a unique tunnel id. The id is 16-bit for
L2TPv2 and 32-bit for L2TPv3. Internally, the id is stored as a 32-bit
value.
(隧道由唯一的隧道 id 标识。L2TPv2 的 id 为 16 位，L2TPv3 的 id 为 32 位。在内部，
id 以 32 位值存储。)

Tunnels are kept in a per-net list, indexed by tunnel id. The
tunnel id namespace is shared by L2TPv2 and L2TPv3.
(隧道保存在每个网络命名空间的列表中，并以隧道 id 为索引。隧道 id 命名空间由 L2TPv2
和 L2TPv3 共享。)

Handling tunnel socket close is perhaps the most tricky part of the
L2TP implementation. If userspace closes a tunnel socket, the L2TP
tunnel and all of its sessions must be closed and destroyed. Since the
tunnel context holds a ref on the tunnel socket, the socket's
sk_destruct won't be called until the tunnel sock_put's its
socket. For UDP sockets, when userspace closes the tunnel socket, the
socket's encap_destroy handler is invoked, which L2TP uses to initiate
its tunnel close actions. For L2TPIP sockets, the socket's close
handler initiates the same tunnel close actions. All sessions are
first closed. Each session drops its tunnel ref. When the tunnel ref
reaches zero, the tunnel drops its socket ref.
(处理隧道套接字的关闭也许是 L2TP 实现中最棘手的部分。如果用户空间关闭一个隧道套接字，
L2TP 隧道及其所有会话必须被关闭并销毁。由于隧道上下文持有对隧道套接字的引用，在隧道
对其套接字执行 sock_put 之前，套接字的 sk_destruct 不会被调用。对于 UDP 套接字，当
用户空间关闭隧道套接字时，会调用套接字的 encap_destroy 处理程序，L2TP 利用它来启动
其隧道关闭操作。对于 L2TPIP 套接字，套接字的 close 处理程序会启动相同的隧道关闭
操作。首先关闭所有会话。每个会话释放其对隧道的引用。当隧道引用计数降为零时，隧道释放
其对套接字的引用。)

Sessions(会话)
--------------

The kernel keeps a struct l2tp_session context for each session.  Each
session has private data which is used for data specific to the
session type. With L2TPv2, the session always carries PPP
traffic. With L2TPv3, the session can carry Ethernet frames (Ethernet
pseudowire) or other data types such as PPP, ATM, HDLC or Frame
Relay. Linux currently implements only Ethernet and PPP session types.
(内核为每个会话保留一个 struct l2tp_session 上下文。每个会话都有私有数据，用于存放
该会话类型特有的数据。对于 L2TPv2，会话总是承载 PPP 流量。对于 L2TPv3，会话可以承载
以太网帧 (以太网伪线) 或其他数据类型，如 PPP、ATM、HDLC 或帧中继 (Frame Relay)。
Linux 目前只实现了以太网和 PPP 会话类型。)

Some L2TP session types also have a socket (PPP pseudowires) while
others do not (Ethernet pseudowires).
(一些 L2TP 会话类型还有自己的套接字 (PPP 伪线)，而另一些则没有 (以太网伪线)。)

Like tunnels, L2TP sessions are identified by a unique
session id. Just as with tunnel ids, the session id is 16-bit for
L2TPv2 and 32-bit for L2TPv3. Internally, the id is stored as a 32-bit
value.
(与隧道一样，L2TP 会话由唯一的会话 id 标识。与隧道 id 一样，L2TPv2 的会话 id 为
16 位，L2TPv3 的会话 id 为 32 位。在内部，id 以 32 位值存储。)

Sessions hold a ref on their parent tunnel to ensure that the tunnel
stays extant while one or more sessions references it.
(会话持有对其父隧道的引用，以确保在一个或多个会话引用该隧道期间，隧道保持存在。)

Sessions are kept in a per-net list. L2TPv2 sessions and L2TPv3
sessions are stored in separate lists. L2TPv2 sessions are keyed
by a 32-bit key made up of the 16-bit tunnel ID and 16-bit
session ID. L2TPv3 sessions are keyed by the 32-bit session ID, since
L2TPv3 session ids are unique across all tunnels.
(会话保存在每个网络命名空间的列表中。L2TPv2 会话和 L2TPv3 会话存储在不同的列表中。
L2TPv2 会话以由 16 位隧道 ID 和 16 位会话 ID 组成的 32 位键为键。L2TPv3 会话以
32 位会话 ID 为键，因为 L2TPv3 会话 id 在所有隧道之间是唯一的。)

Although the L2TPv3 RFC specifies that L2TPv3 session ids are not
scoped by the tunnel, the Linux implementation has historically
allowed this. Such session id collisions are supported using a per-net
hash table keyed by sk and session ID. When looking up L2TPv3
sessions, the list entry may link to multiple sessions with that
session ID, in which case the session matching the given sk (tunnel)
is used.
(尽管 L2TPv3 RFC 规定 L2TPv3 会话 id 不以隧道为作用域，但 Linux 实现历来允许这种
情况。这种会话 id 冲突通过一个每个网络命名空间的、以 sk 和会话 ID 为键的哈希表来
支持。在查找 L2TPv3 会话时，列表项可能链接到多个具有该会话 ID 的会话，此时使用与
给定 sk (隧道) 匹配的会话。)

PPP(PPP)
--------

`net/l2tp/l2tp_ppp.c`_ implements the PPPoL2TP socket family. Each PPP
session has a PPPoL2TP socket.
(`net/l2tp/l2tp_ppp.c`_ 实现 PPPoL2TP 套接字族。每个 PPP 会话都有一个 PPPoL2TP
套接字。)

The PPPoL2TP socket's sk_user_data references the l2tp_session.
(PPPoL2TP 套接字的 sk_user_data 引用 l2tp_session。)

Userspace sends and receives PPP packets over L2TP using a PPPoL2TP
socket. Only PPP control frames pass over this socket: PPP data
packets are handled entirely by the kernel, passing between the L2TP
session and its associated ``pppN`` netdev through the PPP channel
interface of the kernel PPP subsystem.
(用户空间使用 PPPoL2TP 套接字通过 L2TP 发送和接收 PPP 数据包。只有 PPP 控制帧经过
该套接字：PPP 数据包完全由内核处理，在 L2TP 会话与其关联的 ``pppN`` 网络设备之间
通过内核 PPP 子系统的 PPP 通道接口传递。)

The L2TP PPP implementation handles the closing of a PPPoL2TP socket
by closing its corresponding L2TP session. This is complicated because
it must consider racing with netlink session create/destroy requests
and pppol2tp_connect trying to reconnect with a session that is in the
process of being closed. PPP sessions hold a ref on their associated
socket in order that the socket remains extants while the session
references it.
(L2TP PPP 实现通过关闭相应的 L2TP 会话来处理 PPPoL2TP 套接字的关闭。这很复杂，
因为它必须考虑与 netlink 会话创建/销毁请求的竞态，以及 pppol2tp_connect 尝试重连
一个正处于关闭过程中的会话的情况。PPP 会话持有对其关联套接字的引用，以确保在会话
引用该套接字期间套接字保持存在。)

Ethernet(以太网)
----------------

`net/l2tp/l2tp_eth.c`_ implements L2TPv3 Ethernet pseudowires. It
manages a netdev for each session.
(`net/l2tp/l2tp_eth.c`_ 实现 L2TPv3 以太网伪线。它为每个会话管理一个 netdev。)

L2TP Ethernet sessions are created and destroyed by netlink request,
or are destroyed when the tunnel is destroyed. Unlike PPP sessions,
Ethernet sessions do not have an associated socket.
(L2TP 以太网会话通过 netlink 请求创建和销毁，或者在隧道被销毁时随之销毁。与 PPP
会话不同，以太网会话没有关联的套接字。)

Miscellaneous(杂项)
===================

RFCs(RFC)
---------

The kernel code implements the datapath features specified in the
following RFCs:
(内核代码实现了以下 RFC 中规定的数据路径功能：)

======= =============== ===================================
RFC2661 L2TPv2          https://tools.ietf.org/html/rfc2661
RFC3931 L2TPv3          https://tools.ietf.org/html/rfc3931
RFC4719 L2TPv3 Ethernet https://tools.ietf.org/html/rfc4719
======= =============== ===================================

Implementations(实现)
---------------------

A number of open source applications use the L2TP kernel subsystem:
(许多开源应用程序使用 L2TP 内核子系统：)

============ ==============================================
iproute2     https://github.com/shemminger/iproute2
go-l2tp      https://github.com/katalix/go-l2tp
tunneldigger https://github.com/wlanslovenija/tunneldigger
xl2tpd       https://github.com/xelerance/xl2tpd
============ ==============================================

Limitations(局限性)
-------------------

The current implementation has a number of limitations:
(当前实现有一些局限性：)

  1) Interfacing with openvswitch is not yet implemented. It may be
     useful to map OVS Ethernet and VLAN ports into L2TPv3 tunnels.
     (尚未实现与 openvswitch 的对接。将 OVS 以太网和 VLAN 端口映射到 L2TPv3 隧道
     中可能会有用。)

  2) VLAN pseudowires are implemented using an ``l2tpethN`` interface
     configured with a VLAN sub-interface. Since L2TPv3 VLAN
     pseudowires carry one and only one VLAN, it may be better to use
     a single netdevice rather than an ``l2tpethN`` and ``l2tpethN``:M
     pair per VLAN session. The netlink attribute
     ``L2TP_ATTR_VLAN_ID`` was added for this, but it was never
     implemented.
     (VLAN 伪线使用配置了 VLAN 子接口的 ``l2tpethN`` 接口实现。由于 L2TPv3 VLAN
     伪线只承载一个且仅一个 VLAN，因此每个 VLAN 会话使用单个网络设备可能比使用
     ``l2tpethN`` 和 ``l2tpethN``:M 对更好。为此添加了 netlink 属性
     ``L2TP_ATTR_VLAN_ID`` ，但它从未被实现。)

Testing(测试)
-------------

Unmanaged L2TPv3 Ethernet features are tested by the kernel's built-in
selftests. See `tools/testing/selftests/net/l2tp.sh`_.
(非托管 L2TPv3 以太网功能由内核内置的自测 (selftests) 进行测试。参见
`tools/testing/selftests/net/l2tp.sh`_。)

Another test suite, l2tp-ktest_, covers all
of the L2TP APIs and tunnel/session types. This may be integrated into
the kernel's built-in L2TP selftests in the future.
(另一个测试套件 l2tp-ktest_ 覆盖了所有 L2TP API 以及隧道/会话类型。将来它可能会被
集成到内核内置的 L2TP 自测中。)

.. Links
.. _Generic Netlink: generic_netlink.html
.. _libmnl: https://www.netfilter.org/projects/libmnl
.. _include/uapi/linux/l2tp.h: ../../../include/uapi/linux/l2tp.h
.. _include/linux/if_pppol2tp.h: ../../../include/linux/if_pppol2tp.h
.. _net/l2tp/l2tp_ip.c: ../../../net/l2tp/l2tp_ip.c
.. _net/l2tp/l2tp_ip6.c: ../../../net/l2tp/l2tp_ip6.c
.. _net/l2tp/l2tp_ppp.c: ../../../net/l2tp/l2tp_ppp.c
.. _net/l2tp/l2tp_eth.c: ../../../net/l2tp/l2tp_eth.c
.. _tools/testing/selftests/net/l2tp.sh: ../../../tools/testing/selftests/net/l2tp.sh
.. _l2tp-ktest: https://github.com/katalix/l2tp-ktest
