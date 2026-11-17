.. SPDX-License-Identifier: GPL-2.0

========================================================================
Management Component Transport Protocol (MCTP)(管理组件传输协议（MCTP）)
========================================================================

net/mctp/ contains protocol support for MCTP, as defined by DMTF standard
DSP0236. Physical interface drivers ("bindings" in the specification) are
provided in drivers/net/mctp/.
(net/mctp/ 包含 DMTF 标准 DSP0236 所定义的 MCTP 协议支持。物理接口驱动
程序（规范中称为"绑定"，bindings）由 drivers/net/mctp/ 提供。)

The core code provides a socket-based interface to send and receive MCTP
messages, through an AF_MCTP, SOCK_DGRAM socket.
(核心代码通过 AF_MCTP、SOCK_DGRAM 套接字提供基于套接字的 MCTP 消息
收发接口。)

Structure: interfaces & networks(结构：接口与网络)
==================================================

The kernel models the local MCTP topology through two items: interfaces and
networks.
(内核通过两个概念对本地 MCTP 拓扑建模：接口与网络。)

An interface (or "link") is an instance of an MCTP physical transport binding
(as defined by DSP0236, section 3.2.47), likely connected to a specific hardware
device. This is represented as a ``struct netdevice``.
(接口（或称"链路"）是 MCTP 物理传输绑定（DSP0236 第 3.2.47 节定义）的
一个实例，通常连接到某个特定硬件设备。它以 ``struct netdevice`` 表示。)

A network defines a unique address space for MCTP endpoints by endpoint-ID
(described by DSP0236, section 3.2.31). A network has a user-visible identifier
to allow references from userspace. Route definitions are specific to one
network.
(网络按端点 ID（endpoint-ID，DSP0236 第 3.2.31 节描述）为 MCTP 端点定义
一个唯一的地址空间。网络拥有一个用户可见的标识符，以便从用户空间引用。
路由定义是针对单个网络的。)

Interfaces are associated with one network. A network may be associated with one
or more interfaces.
(接口与一个网络关联。一个网络可以与一个或多个接口关联。)

If multiple networks are present, each may contain endpoint IDs (EIDs) that are
also present on other networks.
(如果存在多个网络，每个网络中出现的端点 ID（EID）也可能同时出现在其他
网络中。)

Sockets API(套接字 API)
=======================

Protocol definitions(协议定义)
------------------------------

MCTP uses ``AF_MCTP`` / ``PF_MCTP`` for the address- and protocol- families.
Since MCTP is message-based, only ``SOCK_DGRAM`` sockets are supported.
(MCTP 使用 ``AF_MCTP`` / ``PF_MCTP`` 作为地址族与协议族。由于 MCTP 是
基于消息的，因此只支持 ``SOCK_DGRAM`` 套接字。)

.. code-block:: C

    int sd = socket(AF_MCTP, SOCK_DGRAM, 0);

The only (current) value for the ``protocol`` argument is 0.
(``protocol`` 参数目前唯一的取值是 0。)

As with all socket address families, source and destination addresses are
specified with a ``sockaddr`` type, with a single-byte endpoint address:
(与所有套接字地址族一样，源地址与目的地址都通过 ``sockaddr`` 类型指定，
端点地址为单字节：)

.. code-block:: C

    typedef __u8		mctp_eid_t;

    struct mctp_addr {
            mctp_eid_t		s_addr;
    };

    struct sockaddr_mctp {
            __kernel_sa_family_t smctp_family;
            unsigned int         smctp_network;
            struct mctp_addr     smctp_addr;
            __u8                 smctp_type;
            __u8                 smctp_tag;
    };

    #define MCTP_NET_ANY	0x0
    #define MCTP_ADDR_ANY	0xff


Syscall behaviour(系统调用行为)
-------------------------------

The following sections describe the MCTP-specific behaviours of the standard
socket system calls. These behaviours have been chosen to map closely to the
existing sockets APIs.
(以下各节描述标准套接字系统调用中与 MCTP 相关的行为。这些行为经过精心
选择，与现有的套接字 API 紧密对应。)

``bind()`` : set local socket address(``bind()``：设置本地套接字地址)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Sockets that receive incoming request packets will bind to a local address,
using the ``bind()`` syscall.
(接收入站请求包的套接字会使用 ``bind()`` 系统调用绑定到本地地址。)

.. code-block:: C

    struct sockaddr_mctp addr;

    addr.smctp_family = AF_MCTP;
    addr.smctp_network = MCTP_NET_ANY;
    addr.smctp_addr.s_addr = MCTP_ADDR_ANY;
    addr.smctp_type = MCTP_TYPE_PLDM;
    addr.smctp_tag = MCTP_TAG_OWNER;

    int rc = bind(sd, (struct sockaddr *)&addr, sizeof(addr));

This establishes the local address of the socket. Incoming MCTP messages that
match the network, address, and message type will be received by this socket.
The reference to 'incoming' is important here; a bound socket will only receive
messages with the TO bit set, to indicate an incoming request message, rather
than a response.
(这会建立该套接字的本地地址。凡是网络、地址与消息类型都匹配的入站 MCTP
消息，都将由该套接字接收。这里"入站"的措辞很重要：绑定的套接字只会
接收 TO 位被置位的消息，即入站请求消息，而不是响应。)

The ``smctp_tag`` value will configure the tags accepted from the remote side of
this socket. Given the above, the only valid value is ``MCTP_TAG_OWNER``, which
will result in remotely "owned" tags being routed to this socket. Since
``MCTP_TAG_OWNER`` is set, the 3 least-significant bits of ``smctp_tag`` are not
used; callers must set them to zero.
(``smctp_tag`` 值用于配置该套接字接受来自远端的哪些标签。综上，唯一有效
的值是 ``MCTP_TAG_OWNER``，它会使远端"拥有"的标签被路由到本套接字。由于
``MCTP_TAG_OWNER`` 已置位，``smctp_tag`` 的低 3 位不再使用；调用者必须
将其置为 0。)

A ``smctp_network`` value of ``MCTP_NET_ANY`` will configure the socket to
receive incoming packets from any locally-connected network. A specific network
value will cause the socket to only receive incoming messages from that network.
(``smctp_network`` 为 ``MCTP_NET_ANY`` 时，套接字将接收来自任何本地已连接
网络的入站数据包。指定具体的网络值则使套接字只接收来自该网络的入站
消息。)

The ``smctp_addr`` field specifies a local address to bind to. A value of
``MCTP_ADDR_ANY`` configures the socket to receive messages addressed to any
local destination EID.
(``smctp_addr`` 字段指定要绑定的本地地址。取值 ``MCTP_ADDR_ANY`` 时，
套接字将接收发往任何本地目的 EID 的消息。)

The ``smctp_type`` field specifies which message types to receive. Only the
lower 7 bits of the type is matched on incoming messages (ie., the
most-significant IC bit is not part of the match). This results in the socket
receiving packets with and without a message integrity check footer.
(``smctp_type`` 字段指定接收哪些消息类型。对入站消息只匹配类型的低 7 位
（即最高位的 IC 位不参与匹配）。因此该套接字既会接收带消息完整性校验
尾部的数据包，也会接收不带的数据包。)

``sendto()``, ``sendmsg()``, ``send()`` : transmit an MCTP message(``sendto()``、``sendmsg()``、``send()``：发送 MCTP 消息)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An MCTP message is transmitted using one of the ``sendto()``, ``sendmsg()`` or
``send()`` syscalls. Using ``sendto()`` as the primary example:
(MCTP 消息通过 ``sendto()``、``sendmsg()`` 或 ``send()`` 系统调用之一
发送。下面以 ``sendto()`` 为主要示例：)

.. code-block:: C

    struct sockaddr_mctp addr;
    char buf[14];
    ssize_t len;

    /* set message destination */
    addr.smctp_family = AF_MCTP;
    addr.smctp_network = 0;
    addr.smctp_addr.s_addr = 8;
    addr.smctp_tag = MCTP_TAG_OWNER;
    addr.smctp_type = MCTP_TYPE_ECHO;

    /* arbitrary message to send, with message-type header */
    buf[0] = MCTP_TYPE_ECHO;
    memcpy(buf + 1, "hello, world!", sizeof(buf) - 1);

    len = sendto(sd, buf, sizeof(buf), 0,
                    (struct sockaddr_mctp *)&addr, sizeof(addr));

The network and address fields of ``addr`` define the remote address to send to.
If ``smctp_tag`` has the ``MCTP_TAG_OWNER``, the kernel will ignore any bits set
in ``MCTP_TAG_VALUE``, and generate a tag value suitable for the destination
EID. If ``MCTP_TAG_OWNER`` is not set, the message will be sent with the tag
value as specified. If a tag value cannot be allocated, the system call will
report an errno of ``EAGAIN``.
(``addr`` 的网络与地址字段定义发送目标的远端地址。若 ``smctp_tag`` 带有
``MCTP_TAG_OWNER``，内核会忽略 ``MCTP_TAG_VALUE`` 中置位的任何位，并生成
一个适合该目的 EID 的标签值。若 ``MCTP_TAG_OWNER`` 未置位，消息将按指定
的标签值发送。如果无法分配标签值，系统调用会返回 errno ``EAGAIN``。)

The application must provide the message type byte as the first byte of the
message buffer passed to ``sendto()``. If a message integrity check is to be
included in the transmitted message, it must also be provided in the message
buffer, and the most-significant bit of the message type byte must be 1.
(应用程序必须把消息类型字节作为传给 ``sendto()`` 的消息缓冲区的第一个
字节。如果要随发送的消息附带消息完整性校验，也必须在消息缓冲区中提供，
且消息类型字节的最高位必须为 1。)

The ``sendmsg()`` system call allows a more compact argument interface, and the
message buffer to be specified as a scatter-gather list. At present no ancillary
message types (used for the ``msg_control`` data passed to ``sendmsg()``) are
defined.
(``sendmsg()`` 系统调用提供更紧凑的参数接口，并允许把消息缓冲区指定为
分散-聚集（scatter-gather）列表。目前尚未定义任何辅助消息类型（用于传给
``sendmsg()`` 的 ``msg_control`` 数据）。)

Transmitting a message on an unconnected socket with ``MCTP_TAG_OWNER``
specified will cause an allocation of a tag, if no valid tag is already
allocated for that destination. The (destination-eid,tag) tuple acts as an
implicit local socket address, to allow the socket to receive responses to this
outgoing message. If any previous allocation has been performed (to for a
different remote EID), that allocation is lost.
(在未连接的套接字上以 ``MCTP_TAG_OWNER`` 指定发送消息时，如果该目的地
尚无有效标签被分配，将导致分配一个标签。(目的 EID, 标签) 元组充当隐式
的本地套接字地址，使该套接字能够接收这条外发消息的响应。如果此前已为
（另一个远端 EID）执行过标签分配，该分配将丢失。)

Sockets will only receive responses to requests they have sent (with TO=1) and
may only respond (with TO=0) to requests they have received.
(套接字只会接收其已发送请求（TO=1）的响应，并且只能对其已接收的请求
（TO=0）作出响应。)

``recvfrom()``, ``recvmsg()``, ``recv()`` : receive an MCTP message(``recvfrom()``、``recvmsg()``、``recv()``：接收 MCTP 消息)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

An MCTP message can be received by an application using one of the
``recvfrom()``, ``recvmsg()``, or ``recv()`` system calls. Using ``recvfrom()``
as the primary example:
(应用程序可以使用 ``recvfrom()``、``recvmsg()`` 或 ``recv()`` 系统调用
之一接收 MCTP 消息。下面以 ``recvfrom()`` 为主要示例：)

.. code-block:: C

    struct sockaddr_mctp addr;
    socklen_t addrlen;
    char buf[14];
    ssize_t len;

    addrlen = sizeof(addr);

    len = recvfrom(sd, buf, sizeof(buf), 0,
                    (struct sockaddr_mctp *)&addr, &addrlen);

    /* We can expect addr to describe an MCTP address */
    assert(addrlen >= sizeof(buf));
    assert(addr.smctp_family == AF_MCTP);

    printf("received %zd bytes from remote EID %d\n", rc, addr.smctp_addr);

The address argument to ``recvfrom`` and ``recvmsg`` is populated with the
remote address of the incoming message, including tag value (this will be needed
in order to reply to the message).
(``recvfrom`` 与 ``recvmsg`` 的地址参数会被填充为入站消息的远端地址，
包括标签值（回复该消息时需要用到）。)

The first byte of the message buffer will contain the message type byte. If an
integrity check follows the message, it will be included in the received buffer.
(消息缓冲区的第一个字节将是消息类型字节。如果消息后面跟着完整性校验，
它也会包含在接收缓冲区中。)

The ``recv()`` system call behaves in a similar way, but does not provide a
remote address to the application. Therefore, these are only useful if the
remote address is already known, or the message does not require a reply.
(``recv()`` 系统调用行为类似，但不向应用程序提供远端地址。因此，只有在
远端地址已知、或消息无需回复的情况下，它才有用。)

Like the send calls, sockets will only receive responses to requests they have
sent (TO=1) and may only respond (TO=0) to requests they have received.
(与发送调用一样，套接字只会接收其已发送请求（TO=1）的响应，并且只能对
其已接收的请求（TO=0）作出响应。)

``ioctl(SIOCMCTPALLOCTAG)`` and ``ioctl(SIOCMCTPDROPTAG)``(``ioctl(SIOCMCTPALLOCTAG)`` 与 ``ioctl(SIOCMCTPDROPTAG)``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These tags give applications more control over MCTP message tags, by allocating
(and dropping) tag values explicitly, rather than the kernel automatically
allocating a per-message tag at ``sendmsg()`` time.
(这两个 ioctl 让应用程序能更好地控制 MCTP 消息标签：显式地分配（并
释放）标签值，而不是由内核在 ``sendmsg()`` 时自动为每条消息分配标签。)

In general, you will only need to use these ioctls if your MCTP protocol does
not fit the usual request/response model. For example, if you need to persist
tags across multiple requests, or a request may generate more than one response.
In these cases, the ioctls allow you to decouple the tag allocation (and
release) from individual message send and receive operations.
(一般来说，只有当您的 MCTP 协议不符合常见的请求/响应模型时，才需要使用
这些 ioctl。例如，需要跨多个请求保持标签，或一个请求可能产生多个响应。
在这些情况下，这些 ioctl 允许您把标签的分配（与释放）从单个消息的发送
与接收操作中解耦出来。)

Both ioctls are passed a pointer to a ``struct mctp_ioc_tag_ctl``:
(两个 ioctl 都接收一个指向 ``struct mctp_ioc_tag_ctl`` 的指针：)

.. code-block:: C

    struct mctp_ioc_tag_ctl {
        mctp_eid_t      peer_addr;
        __u8		tag;
        __u16   	flags;
    };

``SIOCMCTPALLOCTAG`` allocates a tag for a specific peer, which an application
can use in future ``sendmsg()`` calls. The application populates the
``peer_addr`` member with the remote EID. Other fields must be zero.
(``SIOCMCTPALLOCTAG`` 为特定对端分配一个标签，应用程序可在后续的
``sendmsg()`` 调用中使用。应用程序用远端 EID 填充 ``peer_addr`` 成员。
其他字段必须为 0。)

On return, the ``tag`` member will be populated with the allocated tag value.
The allocated tag will have the following tag bits set:
(返回时，``tag`` 成员将被填充为已分配的标签值。分配的标签会置位以下
标签位：)

 - ``MCTP_TAG_OWNER``: it only makes sense to allocate tags if you're the tag
   owner
   (``MCTP_TAG_OWNER``：只有当您是标签拥有者时，分配标签才有意义)

 - ``MCTP_TAG_PREALLOC``: to indicate to ``sendmsg()`` that this is a
   preallocated tag.
   (``MCTP_TAG_PREALLOC``：向 ``sendmsg()`` 表明这是一个预分配的标签。)

 - ... and the actual tag value, within the least-significant three bits
   (``MCTP_TAG_MASK``). Note that zero is a valid tag value.
   (……以及实际的标签值，位于最低三位内（``MCTP_TAG_MASK``）。注意 0 是
   有效的标签值。)

The tag value should be used as-is for the ``smctp_tag`` member of ``struct
sockaddr_mctp``.
(该标签值应原样用作 ``struct sockaddr_mctp`` 的 ``smctp_tag`` 成员。)

``SIOCMCTPDROPTAG`` releases a tag that has been previously allocated by a
``SIOCMCTPALLOCTAG`` ioctl. The ``peer_addr`` must be the same as used for the
allocation, and the ``tag`` value must match exactly the tag returned from the
allocation (including the ``MCTP_TAG_OWNER`` and ``MCTP_TAG_PREALLOC`` bits).
The ``flags`` field must be zero.
(``SIOCMCTPDROPTAG`` 释放先前由 ``SIOCMCTPALLOCTAG`` ioctl 分配的标签。
``peer_addr`` 必须与分配时所用值相同，``tag`` 值必须与分配时返回的标签
完全一致（包括 ``MCTP_TAG_OWNER`` 与 ``MCTP_TAG_PREALLOC`` 位）。``flags``
字段必须为 0。)

Kernel internals(内核内部实现)
==============================

There are a few possible packet flows in the MCTP stack:
(MCTP 协议栈中存在几种可能的数据包流转路径：)

1. local TX to remote endpoint, message <= MTU::
   (本地发送到远端端点，消息 <= MTU：)

	sendmsg()
	 -> mctp_local_output()
	    : route lookup
	    -> rt->output() (== mctp_route_output)
	       -> dev_queue_xmit()

2. local TX to remote endpoint, message > MTU::
   (本地发送到远端端点，消息 > MTU：)

	sendmsg()
	-> mctp_local_output()
	    -> mctp_do_fragment_route()
	       : creates packet-sized skbs. For each new skb:
	       -> rt->output() (== mctp_route_output)
	          -> dev_queue_xmit()

3. remote TX to local endpoint, single-packet message::
   (远端发送到本地端点，单包消息：)

	mctp_pkttype_receive()
	: route lookup
	-> rt->output() (== mctp_route_input)
	   : sk_key lookup
	   -> sock_queue_rcv_skb()

4. remote TX to local endpoint, multiple-packet message::
   (远端发送到本地端点，多包消息：)

	mctp_pkttype_receive()
	: route lookup
	-> rt->output() (== mctp_route_input)
	   : sk_key lookup
	   : stores skb in struct sk_key->reasm_head

	mctp_pkttype_receive()
	: route lookup
	-> rt->output() (== mctp_route_input)
	   : sk_key lookup
	   : finds existing reassembly in sk_key->reasm_head
	   : appends new fragment
	   -> sock_queue_rcv_skb()

Key refcounts(键引用计数)
-------------------------

 * keys are refed by:
   (键被以下对象引用：)

   - a skb: during route output, stored in ``skb->cb``.
     (一个 skb：在路由输出期间存储于 ``skb->cb`` 中。)

   - netns and sock lists.
     (netns 与 sock 链表。)

 * keys can be associated with a device, in which case they hold a
   reference to the dev (set through ``key->dev``, counted through
   ``dev->key_count``). Multiple keys can reference the device.
   (键可以与一个设备相关联，此时它们持有对该 dev 的引用（通过
   ``key->dev`` 设置，通过 ``dev->key_count`` 计数）。多个键可以引用
   同一设备。)
