.. SPDX-License-Identifier: BSD-3-Clause

=====================================
Introduction to Netlink(Netlink 简介)
=====================================

Netlink is often described as an ioctl() replacement.
It aims to replace fixed-format C structures as supplied
to ioctl() with a format which allows an easy way to add
or extended the arguments.
(Netlink 常被描述为 ioctl() 的替代品。它旨在用一种便于轻松添加或扩展参数的
格式，取代传给 ioctl() 的定长 C 结构体。)

To achieve this Netlink uses a minimal fixed-format metadata header
followed by multiple attributes in the TLV (type, length, value) format.
(为实现这一点，Netlink 使用一个最小的定长元数据头，后跟多个 TLV（类型、长度、
值）格式的属性。)

Unfortunately the protocol has evolved over the years, in an organic
and undocumented fashion, making it hard to coherently explain.
To make the most practical sense this document starts by describing
netlink as it is used today and dives into more "historical" uses
in later sections.
(遗憾的是，该协议多年来以一种自然的、无文档的方式演进，使其难以连贯地解释。
为了让内容最具实用性，本文档先描述 netlink 如今的使用方式，并在后续章节深入
探讨更"历史"的用法。)

Opening a socket(打开套接字)
============================

Netlink communication happens over sockets, a socket needs to be
opened first:
(Netlink 通信通过套接字进行，首先需要打开一个套接字：)

.. code-block:: c

  fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);

The use of sockets allows for a natural way of exchanging information
in both directions (to and from the kernel). The operations are still
performed synchronously when applications send() the request but
a separate recv() system call is needed to read the reply.
(使用套接字为双向（往返内核）交换信息提供了一种自然的方式。当应用程序 send()
请求时，操作仍然是同步执行的，但需要单独的 recv() 系统调用来读取回复。)

A very simplified flow of a Netlink "call" will therefore look
something like:
(因此，一次 Netlink "调用"的极简流程大致如下：)

.. code-block:: c

  fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);

  /* format the request */
  send(fd, &request, sizeof(request));
  n = recv(fd, &response, RSP_BUFFER_SIZE);
  /* interpret the response */

Netlink also provides natural support for "dumping", i.e. communicating
to user space all objects of a certain type (e.g. dumping all network
interfaces).
(Netlink 还天然支持"转储 (dump)"，即把某类类型的所有对象传送给用户空间
（例如转储所有网络接口）。)

.. code-block:: c

  fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);

  /* format the dump request */
  send(fd, &request, sizeof(request));
  while (1) {
    n = recv(fd, &buffer, RSP_BUFFER_SIZE);
    /* one recv() call can read multiple messages, hence the loop below */
    for (nl_msg in buffer) {
      if (nl_msg.nlmsg_type == NLMSG_DONE)
        goto dump_finished;
      /* process the object */
    }
  }
  dump_finished:

The first two arguments of the socket() call require little explanation -
it is opening a Netlink socket, with all headers provided by the user
(hence NETLINK, RAW). The last argument is the protocol within Netlink.
This field used to identify the subsystem with which the socket will
communicate.
(socket() 调用的前两个参数几乎无需解释——它打开一个 Netlink 套接字，所有头
由用户提供（因此是 NETLINK、RAW）。最后一个参数是 Netlink 内的协议。该字段
过去用来标识套接字将要与之通信的子系统。)

Classic vs Generic Netlink(经典 Netlink 与 Generic Netlink)
-----------------------------------------------------------

Initial implementation of Netlink depended on a static allocation
of IDs to subsystems and provided little supporting infrastructure.
Let us refer to those protocols collectively as **Classic Netlink**.
The list of them is defined on top of the ``include/uapi/linux/netlink.h``
file, they include among others - general networking (NETLINK_ROUTE),
iSCSI (NETLINK_ISCSI), and audit (NETLINK_AUDIT).
(Netlink 的最初实现依赖于向子系统静态分配 ID，几乎没有提供配套基础设施。
我们把这类协议统称为 **Classic Netlink**（经典 Netlink）。它们的列表定义在
``include/uapi/linux/netlink.h`` 文件之上，其中包括通用网络 (NETLINK_ROUTE)、
iSCSI (NETLINK_ISCSI) 和审计 (NETLINK_AUDIT) 等。)

**Generic Netlink** (introduced in 2005) allows for dynamic registration of
subsystems (and subsystem ID allocation), introspection and simplifies
implementing the kernel side of the interface.
(**Generic Netlink**（于 2005 年引入）允许动态注册子系统（以及子系统 ID
分配）、内省，并简化接口内核侧的实现。)

The following section describes how to use Generic Netlink, as the
number of subsystems using Generic Netlink outnumbers the older
protocols by an order of magnitude. There are also no plans for adding
more Classic Netlink protocols to the kernel.
Basic information on how communicating with core networking parts of
the Linux kernel (or another of the 20 subsystems using Classic
Netlink) differs from Generic Netlink is provided later in this document.
(下一节描述如何使用 Generic Netlink，因为使用 Generic Netlink 的子系统数量
比老协议多一个数量级。内核也没有计划添加更多 Classic Netlink 协议。
与 Linux 内核核心网络部分（或使用 Classic Netlink 的另外 20 个子系统之一）
通信和 Generic Netlink 有何不同的基本信息，将在本文档稍后提供。)

Generic Netlink(通用 Netlink)
=============================

In addition to the Netlink fixed metadata header each Netlink protocol
defines its own fixed metadata header. (Similarly to how network
headers stack - Ethernet > IP > TCP we have Netlink > Generic N. > Family.)
(除了 Netlink 定长元数据头之外，每个 Netlink 协议还定义自己的定长元数据头。
（与网络头的堆叠类似——Ethernet > IP > TCP，我们这里是 Netlink > Generic N.
> Family。）)

A Netlink message always starts with struct nlmsghdr, which is followed
by a protocol-specific header. In case of Generic Netlink the protocol
header is struct genlmsghdr.
(Netlink 消息总是以 struct nlmsghdr 开头，其后是协议特定的头。对于 Generic
Netlink，协议头是 struct genlmsghdr。)

The practical meaning of the fields in case of Generic Netlink is as follows:
(对于 Generic Netlink，各字段的实际含义如下：)

.. code-block:: c

  struct nlmsghdr {
	__u32	nlmsg_len;	/* Length of message including headers */
	__u16	nlmsg_type;	/* Generic Netlink Family (subsystem) ID */
	__u16	nlmsg_flags;	/* Flags - request or dump */
	__u32	nlmsg_seq;	/* Sequence number */
	__u32	nlmsg_pid;	/* Port ID, set to 0 */
  };
  struct genlmsghdr {
	__u8	cmd;		/* Command, as defined by the Family */
	__u8	version;	/* Irrelevant, set to 1 */
	__u16	reserved;	/* Reserved, set to 0 */
  };
  /* TLV attributes follow... */

In Classic Netlink :c:member:`nlmsghdr.nlmsg_type` used to identify
which operation within the subsystem the message was referring to
(e.g. get information about a netdev). Generic Netlink needs to mux
multiple subsystems in a single protocol so it uses this field to
identify the subsystem, and :c:member:`genlmsghdr.cmd` identifies
the operation instead. (See :ref:`res_fam` for
information on how to find the Family ID of the subsystem of interest.)
Note that the first 16 values (0 - 15) of this field are reserved for
control messages both in Classic Netlink and Generic Netlink.
See :ref:`nl_msg_type` for more details.
(在 Classic Netlink 中， :c:member:`nlmsghdr.nlmsg_type` 用于标识消息所指的
是子系统内的哪个操作（例如获取某个 netdev 的信息）。Generic Netlink 需要在
单个协议中复用多个子系统，因此它用该字段标识子系统，改由
:c:member:`genlmsghdr.cmd` 标识操作。（如何找到目标子系统的 Family ID，参见
:ref:`res_fam`。）注意，在 Classic Netlink 和 Generic Netlink 中，该字段的
前 16 个值（0 - 15）都保留给控制消息。更多细节参见 :ref:`nl_msg_type`。)

There are 3 usual types of message exchanges on a Netlink socket:
(Netlink 套接字上有 3 种常见的消息交换类型：)

 - performing a single action (``do``);
   (执行单个操作（ ``do`` ）。)
 - dumping information (``dump``);
   (转储信息（ ``dump`` ）。)
 - getting asynchronous notifications (``multicast``).
   (获取异步通知（ ``multicast`` ）。)

Classic Netlink is very flexible and presumably allows other types
of exchanges to happen, but in practice those are the three that get
used.
(Classic Netlink 非常灵活，大概也允许发生其他类型的交换，但实际使用的就是
这三种。)

Asynchronous notifications are sent by the kernel and received by
the user sockets which subscribed to them. ``do`` and ``dump`` requests
are initiated by the user. :c:member:`nlmsghdr.nlmsg_flags` should
be set as follows:
(异步通知由内核发送，由订阅了它们的用户套接字接收。 ``do`` 和 ``dump`` 请求
由用户发起。 :c:member:`nlmsghdr.nlmsg_flags` 应按如下设置：)

 - for ``do``: ``NLM_F_REQUEST | NLM_F_ACK``
 - for ``dump``: ``NLM_F_REQUEST | NLM_F_ACK | NLM_F_DUMP``

:c:member:`nlmsghdr.nlmsg_seq` should be a set to a monotonically
increasing value. The value gets echoed back in responses and doesn't
matter in practice, but setting it to an increasing value for each
message sent is considered good hygiene. The purpose of the field is
matching responses to requests. Asynchronous notifications will have
:c:member:`nlmsghdr.nlmsg_seq` of ``0``.
( :c:member:`nlmsghdr.nlmsg_seq` 应设置为一个单调递增的值。该值会在响应中
原样回显，实际并不重要，但为每条发送的消息设置递增值被认为是良好的习惯。
该字段的用途是把响应与请求匹配起来。异步通知的
:c:member:`nlmsghdr.nlmsg_seq` 为 ``0`` 。)

:c:member:`nlmsghdr.nlmsg_pid` is the Netlink equivalent of an address.
This field can be set to ``0`` when talking to the kernel.
See :ref:`nlmsg_pid` for the (uncommon) uses of the field.
( :c:member:`nlmsghdr.nlmsg_pid` 是 Netlink 中地址的等价物。与内核通信时该
字段可以设置为 ``0`` 。该字段（不常见的）用途参见 :ref:`nlmsg_pid` 。)

The expected use for :c:member:`genlmsghdr.version` was to allow
versioning of the APIs provided by the subsystems. No subsystem to
date made significant use of this field, so setting it to ``1`` seems
like a safe bet.
( :c:member:`genlmsghdr.version` 的预期用途是允许对子系统提供的 API 进行
版本管理。迄今为止没有任何子系统大量使用该字段，因此把它设置为 ``1``
似乎是稳妥的选择。)

.. _nl_msg_type:

Netlink message types(Netlink 消息类型)
---------------------------------------

As previously mentioned :c:member:`nlmsghdr.nlmsg_type` carries
protocol specific values but the first 16 identifiers are reserved
(first subsystem specific message type should be equal to
``NLMSG_MIN_TYPE`` which is ``0x10``).
(如前所述， :c:member:`nlmsghdr.nlmsg_type` 携带协议特定的值，但前 16 个
标识符是保留的（第一个子系统特定的消息类型应等于 ``NLMSG_MIN_TYPE``，即
``0x10``）。)

There are only 4 Netlink control messages defined:
(只定义了 4 种 Netlink 控制消息：)

 - ``NLMSG_NOOP`` - ignore the message, not used in practice;
   ( ``NLMSG_NOOP`` - 忽略该消息，实践中未使用。)
 - ``NLMSG_ERROR`` - carries the return code of an operation;
   ( ``NLMSG_ERROR`` - 携带操作的返回码。)
 - ``NLMSG_DONE`` - marks the end of a dump;
   ( ``NLMSG_DONE`` - 标记转储的结束。)
 - ``NLMSG_OVERRUN`` - socket buffer has overflown, not used to date.
   ( ``NLMSG_OVERRUN`` - 套接字缓冲区已溢出，迄今未使用。)

``NLMSG_ERROR`` and ``NLMSG_DONE`` are of practical importance.
They carry return codes for operations. Note that unless
the ``NLM_F_ACK`` flag is set on the request Netlink will not respond
with ``NLMSG_ERROR`` if there is no error. To avoid having to special-case
this quirk it is recommended to always set ``NLM_F_ACK``.
( ``NLMSG_ERROR`` 和 ``NLMSG_DONE`` 具有实际重要性。它们携带操作的返回码。
注意，除非请求上设置了 ``NLM_F_ACK`` 标志，否则在没有错误时 Netlink 不会以
``NLMSG_ERROR`` 响应。为避免不得不对这个怪癖做特殊处理，建议始终设置
``NLM_F_ACK`` 。)

The format of ``NLMSG_ERROR`` is described by struct nlmsgerr::

  ----------------------------------------------
  | struct nlmsghdr - response header          |
  ----------------------------------------------
  |    int error                               |
  ----------------------------------------------
  | struct nlmsghdr - original request header |
  ----------------------------------------------
  | ** optionally (1) payload of the request   |
  ----------------------------------------------
  | ** optionally (2) extended ACK             |
  ----------------------------------------------

( ``NLMSG_ERROR`` 的格式由 struct nlmsgerr 描述：)

There are two instances of struct nlmsghdr here, first of the response
and second of the request. ``NLMSG_ERROR`` carries the information about
the request which led to the error. This could be useful when trying
to match requests to responses or re-parse the request to dump it into
logs.
(这里有两个 struct nlmsghdr 实例，第一个属于响应，第二个属于请求。
``NLMSG_ERROR`` 携带导致错误的那条请求的信息。在尝试把请求与响应匹配、或
重新解析请求以便转储到日志时，这可能很有用。)

The payload of the request is not echoed in messages reporting success
(``error == 0``) or if ``NETLINK_CAP_ACK`` setsockopt() was set.
The latter is common
and perhaps recommended as having to read a copy of every request back
from the kernel is rather wasteful. The absence of request payload
is indicated by ``NLM_F_CAPPED`` in :c:member:`nlmsghdr.nlmsg_flags`.
(在报告成功（ ``error == 0`` ）的消息中，或者当设置了 ``NETLINK_CAP_ACK``
setsockopt() 时，请求的载荷不会被回显。后者很常见，或许也是被推荐的做法，
因为必须从内核读回每条请求的副本相当浪费。请求载荷的缺失通过
:c:member:`nlmsghdr.nlmsg_flags` 中的 ``NLM_F_CAPPED`` 指示。)

The second optional element of ``NLMSG_ERROR`` are the extended ACK
attributes. See :ref:`ext_ack` for more details. The presence
of extended ACK is indicated by ``NLM_F_ACK_TLVS`` in
:c:member:`nlmsghdr.nlmsg_flags`.
( ``NLMSG_ERROR`` 的第二个可选元素是扩展 ACK 属性。更多细节参见
:ref:`ext_ack` 。扩展 ACK 的存在通过 :c:member:`nlmsghdr.nlmsg_flags` 中的
``NLM_F_ACK_TLVS`` 指示。)

``NLMSG_DONE`` is simpler, the request is never echoed but the extended
ACK attributes may be present::

  ----------------------------------------------
  | struct nlmsghdr - response header          |
  ----------------------------------------------
  |    int error                               |
  ----------------------------------------------
  | ** optionally extended ACK                 |
  ----------------------------------------------

( ``NLMSG_DONE`` 更简单，请求从不回显，但扩展 ACK 属性可能存在：)

Note that some implementations may issue custom ``NLMSG_DONE`` messages
in reply to ``do`` action requests. In that case the payload is
implementation-specific and may also be absent.
(注意，一些实现可能以自定义的 ``NLMSG_DONE`` 消息来响应 ``do`` 操作请求。
这种情况下载荷是实现特定的，也可能不存在。)

.. _res_fam:

Resolving the Family ID(解析 Family ID)
---------------------------------------

This section explains how to find the Family ID of a subsystem.
It also serves as an example of Generic Netlink communication.
(本节说明如何找到子系统的 Family ID。它也作为一个 Generic Netlink 通信的
示例。)

Generic Netlink is itself a subsystem exposed via the Generic Netlink API.
To avoid a circular dependency Generic Netlink has a statically allocated
Family ID (``GENL_ID_CTRL`` which is equal to ``NLMSG_MIN_TYPE``).
The Generic Netlink family implements a command used to find out information
about other families (``CTRL_CMD_GETFAMILY``).
(Generic Netlink 本身就是一个通过 Generic Netlink API 暴露的子系统。为避免
循环依赖，Generic Netlink 有一个静态分配的 Family ID（ ``GENL_ID_CTRL``，
等于 ``NLMSG_MIN_TYPE`` ）。Generic Netlink family 实现了一条用于查询其他
family 信息的命令（ ``CTRL_CMD_GETFAMILY`` ）。)

To get information about the Generic Netlink family named for example
``"test1"`` we need to send a message on the previously opened Generic Netlink
socket. The message should target the Generic Netlink Family (1), be a
``do`` (2) call to ``CTRL_CMD_GETFAMILY`` (3). A ``dump`` version of this
call would make the kernel respond with information about *all* the families
it knows about. Last but not least the name of the family in question has
to be specified (4) as an attribute with the appropriate type::

  struct nlmsghdr:
    __u32 nlmsg_len:	32
    __u16 nlmsg_type:	GENL_ID_CTRL               // (1)
    __u16 nlmsg_flags:	NLM_F_REQUEST | NLM_F_ACK  // (2)
    __u32 nlmsg_seq:	1
    __u32 nlmsg_pid:	0

  struct genlmsghdr:
    __u8 cmd:		CTRL_CMD_GETFAMILY         // (3)
    __u8 version:	2 /* or 1, doesn't matter */
    __u16 reserved:	0

  struct nlattr:                                   // (4)
    __u16 nla_len:	10
    __u16 nla_type:	CTRL_ATTR_FAMILY_NAME
    char data: 		test1\0

  (padding:)
    char data:		\0\0

(要获取名为 ``"test1"`` 的 Generic Netlink family 的信息，我们需要在之前
打开的 Generic Netlink 套接字上发送一条消息。该消息应面向 Generic Netlink
Family (1)，是对 ``CTRL_CMD_GETFAMILY`` (3) 的 ``do`` (2) 调用。此调用的
``dump`` 版本会让内核以其知晓的 *所有* family 的信息作为响应。最后同样重要
的是，必须把相关 family 的名称 (4) 作为具有适当类型的属性指定：)

The length fields in Netlink (:c:member:`nlmsghdr.nlmsg_len`
and :c:member:`nlattr.nla_len`) always *include* the header.
Attribute headers in netlink must be aligned to 4 bytes from the start
of the message, hence the extra ``\0\0`` after ``CTRL_ATTR_FAMILY_NAME``.
The attribute lengths *exclude* the padding.
(Netlink 中的长度字段（ :c:member:`nlmsghdr.nlmsg_len`
和 :c:member:`nlattr.nla_len` ）总是 *包含* 头部。netlink 中的属性头必须相对
于消息起始按 4 字节对齐，因此 ``CTRL_ATTR_FAMILY_NAME`` 之后有额外的
``\0\0`` 。属性长度 *不包含* 填充。)

If the family is found kernel will reply with two messages, the response
with all the information about the family::

  /* Message #1 - reply */
  struct nlmsghdr:
    __u32 nlmsg_len:	136
    __u16 nlmsg_type:	GENL_ID_CTRL
    __u16 nlmsg_flags:	0
    __u32 nlmsg_seq:	1    /* echoed from our request */
    __u32 nlmsg_pid:	5831 /* The PID of our user space process */

  struct genlmsghdr:
    __u8 cmd:		CTRL_CMD_GETFAMILY
    __u8 version:	2
    __u16 reserved:	0

  struct nlattr:
    __u16 nla_len:	10
    __u16 nla_type:	CTRL_ATTR_FAMILY_NAME
    char data: 		test1\0

  (padding:)
    data:		\0\0

  struct nlattr:
    __u16 nla_len:	6
    __u16 nla_type:	CTRL_ATTR_FAMILY_ID
    __u16: 		123  /* The Family ID we are after */

  (padding:)
    char data:		\0\0

  struct nlattr:
    __u16 nla_len:	9
    __u16 nla_type:	CTRL_ATTR_FAMILY_VERSION
    __u16: 		1

  /* ... etc, more attributes will follow. */

(如果找到了该 family，内核会回复两条消息，其一是包含该 family 全部信息的
响应：)

And the error code (success) since ``NLM_F_ACK`` had been set on the request::

  /* Message #2 - the ACK */
  struct nlmsghdr:
    __u32 nlmsg_len:	36
    __u16 nlmsg_type:	NLMSG_ERROR
    __u16 nlmsg_flags:	NLM_F_CAPPED /* There won't be a payload */
    __u32 nlmsg_seq:	1    /* echoed from our request */
    __u32 nlmsg_pid:	5831 /* The PID of our user space process */

  int error:		0

  struct nlmsghdr: /* Copy of the request header as we sent it */
    __u32 nlmsg_len:	32
    __u16 nlmsg_type:	GENL_ID_CTRL
    __u16 nlmsg_flags:	NLM_F_REQUEST | NLM_F_ACK
    __u32 nlmsg_seq:	1
    __u32 nlmsg_pid:	0

(其二是错误码（成功）消息——因为请求上设置了 ``NLM_F_ACK``。)

The order of attributes (struct nlattr) is not guaranteed so the user
has to walk the attributes and parse them.
(属性 (struct nlattr) 的顺序没有保证，因此用户必须遍历属性并解析它们。)

Note that Generic Netlink sockets are not associated or bound to a single
family. A socket can be used to exchange messages with many different
families, selecting the recipient family on message-by-message basis using
the :c:member:`nlmsghdr.nlmsg_type` field.
(注意，Generic Netlink 套接字不与单个 family 关联或绑定。一个套接字可以与
许多不同的 family 交换消息，通过 :c:member:`nlmsghdr.nlmsg_type` 字段逐条
消息地选择目标 family。)

.. _ext_ack:

Extended ACK(扩展 ACK)
----------------------

Extended ACK controls reporting of additional error/warning TLVs
in ``NLMSG_ERROR`` and ``NLMSG_DONE`` messages. To maintain backward
compatibility this feature has to be explicitly enabled by setting
the ``NETLINK_EXT_ACK`` setsockopt() to ``1``.
(扩展 ACK 控制在 ``NLMSG_ERROR`` 和 ``NLMSG_DONE`` 消息中报告额外的错误/警告
TLV。为保持向后兼容，必须通过把 ``NETLINK_EXT_ACK`` setsockopt() 设置为
``1`` 来显式启用此特性。)

Types of extended ack attributes are defined in enum nlmsgerr_attrs.
The most commonly used attributes are ``NLMSGERR_ATTR_MSG``,
``NLMSGERR_ATTR_OFFS`` and ``NLMSGERR_ATTR_MISS_*``.
(扩展 ack 属性的类型定义在 enum nlmsgerr_attrs 中。最常用的属性是
``NLMSGERR_ATTR_MSG``、``NLMSGERR_ATTR_OFFS`` 和 ``NLMSGERR_ATTR_MISS_*`` 。)

``NLMSGERR_ATTR_MSG`` carries a message in English describing
the encountered problem. These messages are far more detailed
than what can be expressed thru standard UNIX error codes.
( ``NLMSGERR_ATTR_MSG`` 携带一条用英文描述所遇问题的消息。这些消息远比标准
UNIX 错误码所能表达的信息详细。)

``NLMSGERR_ATTR_OFFS`` points to the attribute which caused the problem.
( ``NLMSGERR_ATTR_OFFS`` 指向导致问题的属性。)

``NLMSGERR_ATTR_MISS_TYPE`` and ``NLMSGERR_ATTR_MISS_NEST``
inform about a missing attribute.
( ``NLMSGERR_ATTR_MISS_TYPE`` 和 ``NLMSGERR_ATTR_MISS_NEST`` 告知缺失了某个
属性。)

Extended ACKs can be reported on errors as well as in case of success.
The latter should be treated as a warning.
(扩展 ACK 可以在出错时报告，也可以在成功时报告。后者应被视为警告。)

Extended ACKs greatly improve the usability of Netlink and should
always be enabled, appropriately parsed and reported to the user.
(扩展 ACK 大大提升了 Netlink 的可用性，应始终启用、恰当解析并向用户报告。)

Advanced topics(高级主题)
=========================

Dump consistency(dump 一致性)
-----------------------------

Some of the data structures kernel uses for storing objects make
it hard to provide an atomic snapshot of all the objects in a dump
(without impacting the fast-paths updating them).
(内核用于存储对象的某些数据结构，使得在转储中提供所有对象的原子快照变得
困难（同时不影响更新它们的快速路径）。)

Kernel may set the ``NLM_F_DUMP_INTR`` flag on any message in a dump
(including the ``NLMSG_DONE`` message) if the dump was interrupted and
may be inconsistent (e.g. missing objects). User space should retry
the dump if it sees the flag set.
(如果转储被中断且可能不一致（例如缺少对象），内核可能在转储的任何消息上
（包括 ``NLMSG_DONE`` 消息）设置 ``NLM_F_DUMP_INTR`` 标志。用户空间看到该
标志置位时应重试转储。)

Introspection(内省)
-------------------

The basic introspection abilities are enabled by access to the Family
object as reported in :ref:`res_fam`. User can query information about
the Generic Netlink family, including which operations are supported
by the kernel and what attributes the kernel understands.
Family information includes the highest ID of an attribute kernel can parse,
a separate command (``CTRL_CMD_GETPOLICY``) provides detailed information
about supported attributes, including ranges of values the kernel accepts.
(基本的内省能力通过访问 :ref:`res_fam` 中所述的 Family 对象开启。用户可以
查询 Generic Netlink family 的信息，包括内核支持哪些操作、内核理解哪些属性。
family 信息包括内核能解析的属性的最高 ID；单独的命令
（ ``CTRL_CMD_GETPOLICY`` ）提供受支持属性的详细信息，包括内核接受的取值
范围。)

Querying family information is useful in cases when user space needs
to make sure that the kernel has support for a feature before issuing
a request.
(当用户空间需要在发出请求前确认内核支持某个特性时，查询 family 信息很有
用处。)

.. _nlmsg_pid:

nlmsg_pid(nlmsg_pid 字段)
-------------------------

:c:member:`nlmsghdr.nlmsg_pid` is the Netlink equivalent of an address.
It is referred to as Port ID, sometimes Process ID because for historical
reasons if the application does not select (bind() to) an explicit Port ID
kernel will automatically assign it the ID equal to its Process ID
(as reported by the getpid() system call).
( :c:member:`nlmsghdr.nlmsg_pid` 是 Netlink 中地址的等价物。它被称为 Port
ID，有时被称为 Process ID，因为出于历史原因，如果应用程序不选择（bind() 到）
一个显式的 Port ID，内核会自动给它分配等于其 Process ID 的 ID（如 getpid()
系统调用所报告的那样）。)

Similarly to the bind() semantics of the TCP/IP network protocols the value
of zero means "assign automatically", hence it is common for applications
to leave the :c:member:`nlmsghdr.nlmsg_pid` field initialized to ``0``.
(与 TCP/IP 网络协议的 bind() 语义类似，零值意味着"自动分配"，因此应用程序
通常让 :c:member:`nlmsghdr.nlmsg_pid` 字段保持初始化为 ``0`` 。)

The field is still used today in rare cases when kernel needs to send
a unicast notification. User space application can use bind() to associate
its socket with a specific PID, it then communicates its PID to the kernel.
This way the kernel can reach the specific user space process.
(如今在内核需要发送单播通知的罕见场景中，该字段仍在使用。用户空间应用程序
可以使用 bind() 把其套接字与特定 PID 关联，从而把它的 PID 告知内核。这样内核
就能到达特定的用户空间进程。)

This sort of communication is utilized in UMH (User Mode Helper)-like
scenarios when kernel needs to trigger user space processing or ask user
space for a policy decision.
(这类通信用于类似 UMH (User Mode Helper) 的场景：内核需要触发用户空间处理，
或就策略决定征询用户空间。)

Multicast notifications(组播通知)
---------------------------------

One of the strengths of Netlink is the ability to send event notifications
to user space. This is a unidirectional form of communication (kernel ->
user) and does not involve any control messages like ``NLMSG_ERROR`` or
``NLMSG_DONE``.
(Netlink 的强项之一是能够向用户空间发送事件通知。这是一种单向通信形式
（内核 -> 用户），不涉及 ``NLMSG_ERROR`` 或 ``NLMSG_DONE`` 之类的控制
消息。)

For example the Generic Netlink family itself defines a set of multicast
notifications about registered families. When a new family is added the
sockets subscribed to the notifications will get the following message::

  struct nlmsghdr:
    __u32 nlmsg_len:	136
    __u16 nlmsg_type:	GENL_ID_CTRL
    __u16 nlmsg_flags:	0
    __u32 nlmsg_seq:	0
    __u32 nlmsg_pid:	0

  struct genlmsghdr:
    __u8 cmd:		CTRL_CMD_NEWFAMILY
    __u8 version:	2
    __u16 reserved:	0

  struct nlattr:
    __u16 nla_len:	10
    __u16 nla_type:	CTRL_ATTR_FAMILY_NAME
    char data: 		test1\0

  (padding:)
    data:		\0\0

  struct nlattr:
    __u16 nla_len:	6
    __u16 nla_type:	CTRL_ATTR_FAMILY_ID
    __u16: 		123  /* The Family ID we are after */

  (padding:)
    char data:		\0\0

  struct nlattr:
    __u16 nla_len:	9
    __u16 nla_type:	CTRL_ATTR_FAMILY_VERSION
    __u16: 		1

  /* ... etc, more attributes will follow. */

(例如，Generic Netlink family 自身为已注册的 family 定义了一组组播通知。当
新 family 被添加时，订阅了该通知的套接字会收到以下消息：)

The notification contains the same information as the response
to the ``CTRL_CMD_GETFAMILY`` request.
(该通知包含的信息与对 ``CTRL_CMD_GETFAMILY`` 请求的响应相同。)

The Netlink headers of the notification are mostly 0 and irrelevant.
The :c:member:`nlmsghdr.nlmsg_seq` may be either zero or a monotonically
increasing notification sequence number maintained by the family.
(该通知的 Netlink 头大多为 0，无关紧要。 :c:member:`nlmsghdr.nlmsg_seq` 可
以为零，也可以是由该 family 维护的单调递增的通知序号。)

To receive notifications the user socket must subscribe to the relevant
notification group. Much like the Family ID, the Group ID for a given
multicast group is dynamic and can be found inside the Family information.
The ``CTRL_ATTR_MCAST_GROUPS`` attribute contains nests with names
(``CTRL_ATTR_MCAST_GRP_NAME``) and IDs (``CTRL_ATTR_MCAST_GRP_ID``) of
the groups family.
(要接收通知，用户套接字必须订阅相关的通知组。与 Family ID 很相似，给定组播
组的 Group ID 是动态的，可以在 family 信息中找到。 ``CTRL_ATTR_MCAST_GROUPS``
属性包含若干嵌套属性，其中有该 family 各组的名称
（ ``CTRL_ATTR_MCAST_GRP_NAME`` ）与 ID（ ``CTRL_ATTR_MCAST_GRP_ID`` ）。)

Once the Group ID is known a setsockopt() call adds the socket to the group:
(知道 Group ID 后，一次 setsockopt() 调用即可把套接字加入该组：)

.. code-block:: c

  unsigned int group_id;

  /* .. find the group ID... */

  setsockopt(fd, SOL_NETLINK, NETLINK_ADD_MEMBERSHIP,
             &group_id, sizeof(group_id));

The socket will now receive notifications.
(此后该套接字就会接收通知了。)

It is recommended to use separate sockets for receiving notifications
and sending requests to the kernel. The asynchronous nature of notifications
means that they may get mixed in with the responses making the message
handling much harder.
(建议使用单独的套接字接收通知和向内核发送请求。通知的异步性质意味着它们可能
与响应混在一起，使消息处理变得困难得多。)

Buffer sizing(缓冲区大小)
-------------------------

Netlink sockets are datagram sockets rather than stream sockets,
meaning that each message must be received in its entirety by a single
recv()/recvmsg() system call. If the buffer provided by the user is too
short, the message will be truncated and the ``MSG_TRUNC`` flag set
in struct msghdr (struct msghdr is the second argument
of the recvmsg() system call, *not* a Netlink header).
(Netlink 套接字是数据报套接字而非流套接字，这意味着每条消息必须由单次
recv()/recvmsg() 系统调用完整接收。如果用户提供的缓冲区太短，消息会被截断，
并在 struct msghdr 中设置 ``MSG_TRUNC`` 标志（struct msghdr 是 recvmsg()
系统调用的第二个参数， *不是* Netlink 头）。)

Upon truncation the remaining part of the message is discarded.
(发生截断时，消息的剩余部分会被丢弃。)

Netlink expects that the user buffer will be at least 8kB or a page
size of the CPU architecture, whichever is bigger. Particular Netlink
families may, however, require a larger buffer. 32kB buffer is recommended
for most efficient handling of dumps (larger buffer fits more dumped
objects and therefore fewer recvmsg() calls are needed).
(Netlink 期望用户缓冲区至少为 8kB 或 CPU 架构的页大小中的较大者。不过，特定
的 Netlink family 可能要求更大的缓冲区。建议使用 32kB 缓冲区以最高效地处理
转储（更大的缓冲区能容纳更多被转储的对象，因此所需的 recvmsg() 调用更
少）。)

.. _classic_netlink:

Classic Netlink(经典 Netlink)
=============================

The main differences between Classic and Generic Netlink are the dynamic
allocation of subsystem identifiers and availability of introspection.
In theory the protocol does not differ significantly, however, in practice
Classic Netlink experimented with concepts which were abandoned in Generic
Netlink (really, they usually only found use in a small corner of a single
subsystem). This section is meant as an explainer of a few of such concepts,
with the explicit goal of giving the Generic Netlink
users the confidence to ignore them when reading the uAPI headers.
(Classic Netlink 与 Generic Netlink 的主要区别在于子系统标识符的动态分配以
及内省能力的可用性。理论上协议并没有显著差异，但在实践中，Classic Netlink
尝试过一些在 Generic Netlink 中被放弃的概念（实际上，它们通常只在单个子系统
的一个小角落里得到使用）。本节旨在解释其中几个概念，明确目标是让 Generic
Netlink 用户在阅读 uAPI 头文件时可以放心地忽略它们。)

Most of the concepts and examples here refer to the ``NETLINK_ROUTE`` family,
which covers much of the configuration of the Linux networking stack.
Real documentation of that family, deserves a chapter (or a book) of its own.
(这里的大部分概念和示例都针对 ``NETLINK_ROUTE`` family，它涵盖了 Linux 网络
栈配置的很大一部分。该 family 的真正文档值得单独用一章（甚至一本书）来
写。)

Families(协议族)
----------------

Netlink refers to subsystems as families. This is a remnant of using
sockets and the concept of protocol families, which are part of message
demultiplexing in ``NETLINK_ROUTE``.
(Netlink 把子系统称为 family（协议族）。这是使用套接字与协议族概念的残留，
协议族是 ``NETLINK_ROUTE`` 中消息分用的组成部分。)

Sadly every layer of encapsulation likes to refer to whatever it's carrying
as "families" making the term very confusing:
(遗憾的是，每一层封装都喜欢把它所承载的东西称为 "family"，使这个术语非常
令人困惑：)

  1. AF_NETLINK is a bona fide socket protocol family
     (1. AF_NETLINK 是一个名副其实的套接字协议族)
  2. AF_NETLINK's documentation refers to what comes after its own
     header (struct nlmsghdr) in a message as a "Family Header"
     (2. AF_NETLINK 的文档把消息中位于其自身头 (struct nlmsghdr) 之后的
     部分称为 "Family Header")
  3. Generic Netlink is a family for AF_NETLINK (struct genlmsghdr follows
     struct nlmsghdr), yet it also calls its users "Families".
     (3. Generic Netlink 是 AF_NETLINK 的一个 family（struct genlmsghdr 跟在
     struct nlmsghdr 之后），但它也把它的用户称为 "Families"。)

Note that the Generic Netlink Family IDs are in a different "ID space"
and overlap with Classic Netlink protocol numbers (e.g. ``NETLINK_CRYPTO``
has the Classic Netlink protocol ID of 21 which Generic Netlink will
happily allocate to one of its families as well).
(注意，Generic Netlink Family ID 处于不同的"ID 空间"中，并与 Classic
Netlink 协议号重叠（例如 ``NETLINK_CRYPTO`` 的 Classic Netlink 协议 ID 是
21，而 Generic Netlink 也会欣然把 21 分配给它自己的某个 family）。)

Strict checking(严格检查)
-------------------------

The ``NETLINK_GET_STRICT_CHK`` socket option enables strict input checking
in ``NETLINK_ROUTE``. It was needed because historically kernel did not
validate the fields of structures it didn't process. This made it impossible
to start using those fields later without risking regressions in applications
which initialized them incorrectly or not at all.
( ``NETLINK_GET_STRICT_CHK`` 套接字选项在 ``NETLINK_ROUTE`` 中启用严格的
输入检查。之所以需要它，是因为历史上内核不校验自己不处理的结构的字段。这使
得日后开始使用这些字段时，无法避免给那些错误初始化（或根本不初始化）它们的
应用程序带来回归风险。)

``NETLINK_GET_STRICT_CHK`` declares that the application is initializing
all fields correctly. It also opts into validating that message does not
contain trailing data and requests that kernel rejects attributes with
type higher than largest attribute type known to the kernel.
( ``NETLINK_GET_STRICT_CHK`` 声明应用程序正确初始化了所有字段。它还选择加入
对"消息不含尾部多余数据"的校验，并请求内核拒绝类型值大于内核已知的最大属性
类型的属性。)

``NETLINK_GET_STRICT_CHK`` is not used outside of ``NETLINK_ROUTE``.
( ``NETLINK_GET_STRICT_CHK`` 不在 ``NETLINK_ROUTE`` 之外使用。)

Unknown attributes(未知属性)
----------------------------

Historically Netlink ignored all unknown attributes. The thinking was that
it would free the application from having to probe what kernel supports.
The application could make a request to change the state and check which
parts of the request "stuck".
(历史上 Netlink 忽略所有未知属性。当时的想法是，这能让应用程序不必探测内核
支持什么。应用程序可以直接发出改变状态的请求，然后检查请求的哪些部分
"生效了"。)

This is no longer the case for new Generic Netlink families and those opting
in to strict checking. See enum netlink_validation for validation types
performed.
(对于新的 Generic Netlink family 以及选择加入严格检查的协议，情况已不再如
此。所执行的校验类型参见 enum netlink_validation。)

Fixed metadata and structures(固定元数据与结构体)
-------------------------------------------------

Classic Netlink made liberal use of fixed-format structures within
the messages. Messages would commonly have a structure with
a considerable number of fields after struct nlmsghdr. It was also
common to put structures with multiple members inside attributes,
without breaking each member into an attribute of its own.
(Classic Netlink 在消息中大量使用定长结构体。消息通常在 struct nlmsghdr
之后带有一个包含相当多字段的结构体。把含多个成员的结构体整个放进属性里、
而不把每个成员拆成独立属性的做法也很常见。)

This has caused problems with validation and extensibility and
therefore using binary structures is actively discouraged for new
attributes.
(这给校验与可扩展性带来了问题，因此对于新属性，强烈不建议使用二进制
结构体。)

Request types(请求类型)
-----------------------

``NETLINK_ROUTE`` categorized requests into 4 types ``NEW``, ``DEL``, ``GET``,
and ``SET``. Each object can handle all or some of those requests
(objects being netdevs, routes, addresses, qdiscs etc.) Request type
is defined by the 2 lowest bits of the message type, so commands for
new objects would always be allocated with a stride of 4.
( ``NETLINK_ROUTE`` 把请求分为 4 种类型 ``NEW``、``DEL``、``GET`` 和
``SET``。每个对象可以处理其中全部或部分请求（对象包括 netdev、路由、地址、
qdisc 等）。请求类型由消息类型的最低 2 个比特定义，因此新对象的命令总是以
4 为步长分配。)

Each object would also have its own fixed metadata shared by all request
types (e.g. struct ifinfomsg for netdev requests, struct ifaddrmsg for address
requests, struct tcmsg for qdisc requests).
(每个对象还有自己的、由所有请求类型共享的固定元数据（例如 netdev 请求用
struct ifinfomsg，地址请求用 struct ifaddrmsg，qdisc 请求用
struct tcmsg）。)

Even though other protocols and Generic Netlink commands often use
the same verbs in their message names (``GET``, ``SET``) the concept
of request types did not find wider adoption.
(尽管其他协议和 Generic Netlink 命令经常在消息名中使用相同的动词
（ ``GET``、``SET`` ），但请求类型这一概念并未得到更广泛的采用。)

Notification echo(通知回显)
---------------------------

``NLM_F_ECHO`` requests for notifications resulting from the request
to be queued onto the requesting socket. This is useful to discover
the impact of the request.
( ``NLM_F_ECHO`` 请求把由该请求产生的通知排入发起请求的套接字的队列。这有
助于了解请求的影响。)

Note that this feature is not universally implemented.
(注意，此特性并未被普遍实现。)

Other request-type-specific flags(其他请求类型专属标志)
-------------------------------------------------------

Classic Netlink defined various flags for its ``GET``, ``NEW``
and ``DEL`` requests in the upper byte of nlmsg_flags in struct nlmsghdr.
Since request types have not been generalized the request type specific
flags are rarely used (and considered deprecated for new families).
(Classic Netlink 在 struct nlmsghdr 的 nlmsg_flags 高字节中为其 ``GET``、
``NEW`` 和 ``DEL`` 请求定义了各种标志。由于请求类型没有被推广，请求类型
专属标志很少使用（并且对新 family 而言被视为已废弃）。)

For ``GET`` - ``NLM_F_ROOT`` and ``NLM_F_MATCH`` are combined into
``NLM_F_DUMP``, and not used separately. ``NLM_F_ATOMIC`` is never used.
(对于 ``GET``—— ``NLM_F_ROOT`` 和 ``NLM_F_MATCH`` 被合并成 ``NLM_F_DUMP``，
不单独使用。 ``NLM_F_ATOMIC`` 从不使用。)

For ``DEL`` - ``NLM_F_NONREC`` is only used by nftables and ``NLM_F_BULK``
only by FDB some operations.
(对于 ``DEL``—— ``NLM_F_NONREC`` 只被 nftables 使用， ``NLM_F_BULK`` 只被
FDB 的某些操作使用。)

The flags for ``NEW`` are used most commonly in classic Netlink. Unfortunately,
the meaning is not crystal clear. The following description is based on the
best guess of the intention of the authors, and in practice all families
stray from it in one way or another. ``NLM_F_REPLACE`` asks to replace
an existing object, if no matching object exists the operation should fail.
``NLM_F_EXCL`` has the opposite semantics and only succeeds if object already
existed.
``NLM_F_CREATE`` asks for the object to be created if it does not
exist, it can be combined with ``NLM_F_REPLACE`` and ``NLM_F_EXCL``.
( ``NEW`` 的标志在 classic Netlink 中使用得最频繁。遗憾的是，其含义并不完全
清晰。以下描述基于对作者意图的最佳猜测，实践中所有 family 都会以这样或那样
的方式偏离它。 ``NLM_F_REPLACE`` 请求替换一个已存在的对象；如果不存在匹配
的对象，操作应当失败。 ``NLM_F_EXCL`` 语义相反，只有在对象已存在时才成功。
``NLM_F_CREATE`` 请求在对象不存在时创建它；它可以与 ``NLM_F_REPLACE`` 和
``NLM_F_EXCL`` 组合。)

A comment in the main Netlink uAPI header states::

   4.4BSD ADD		NLM_F_CREATE|NLM_F_EXCL
   4.4BSD CHANGE	NLM_F_REPLACE

   True CHANGE		NLM_F_CREATE|NLM_F_REPLACE
   Append		NLM_F_CREATE
   Check		NLM_F_EXCL

(主 Netlink uAPI 头文件中的一段注释写道：)

which seems to indicate that those flags predate request types.
``NLM_F_REPLACE`` without ``NLM_F_CREATE`` was initially used instead
of ``SET`` commands.
``NLM_F_EXCL`` without ``NLM_F_CREATE`` was used to check if object exists
without creating it, presumably predating ``GET`` commands.
(这似乎表明这些标志早于请求类型的出现。不带 ``NLM_F_CREATE`` 的
``NLM_F_REPLACE`` 最初被用来代替 ``SET`` 命令。不带 ``NLM_F_CREATE`` 的
``NLM_F_EXCL`` 被用来检查对象是否存在而不创建它，大概早于 ``GET``
命令。)

``NLM_F_APPEND`` indicates that if one key can have multiple objects associated
with it (e.g. multiple next-hop objects for a route) the new object should be
added to the list rather than replacing the entire list.
( ``NLM_F_APPEND`` 表示如果一个键可以关联多个对象（例如一条路由的多个
next-hop 对象），新对象应被加入列表，而不是替换整个列表。)

uAPI reference(uAPI 参考)
=========================

.. kernel-doc:: include/uapi/linux/netlink.h
