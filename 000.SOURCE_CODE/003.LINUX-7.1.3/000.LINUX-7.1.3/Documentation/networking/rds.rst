.. SPDX-License-Identifier: GPL-2.0

===
RDS
===

(RDS——可靠数据报套接字)

Overview
(概述)
========

This readme tries to provide some background on the hows and whys of RDS,
and will hopefully help you find your way around the code.
(本自述文件试图提供一些关于 RDS 实现方式与设计缘由的背景信息，希望能帮助你更好
地读懂代码。)

In addition, please see this email about RDS origins:
http://oss.oracle.com/pipermail/rds-devel/2007-November/000228.html
(此外，请参阅这封关于 RDS 起源的邮件：
http://oss.oracle.com/pipermail/rds-devel/2007-November/000228.html。)

RDS Architecture
(RDS 架构)
================

RDS provides reliable, ordered datagram delivery by using a single
reliable connection between any two nodes in the cluster. This allows
applications to use a single socket to talk to any other process in the
cluster - so in a cluster with N processes you need N sockets, in contrast
to N*N if you use a connection-oriented socket transport like TCP.
(RDS 通过在集群中任意两个节点之间使用单一可靠连接，提供可靠且有序的数据报传送。
这使得应用程序可以使用一个套接字与集群中的任何其他进程通信——因此在一个有 N
个进程的集群中只需要 N 个套接字；相比之下，如果使用 TCP 这类面向连接的套接字
传输，则需要 N*N 个。)

RDS is not Infiniband-specific; it was designed to support different
transports.  The current implementation used to support RDS over TCP as well
as IB.
(RDS 并非 Infiniband 专用；它的设计本就支持多种传输。当前实现同时支持基于 TCP
的 RDS 与基于 IB 的 RDS。)

The high-level semantics of RDS from the application's point of view are
(从应用程序的角度看，RDS 的高层语义如下)

 *	Addressing
	(寻址)

	RDS uses IPv4 addresses and 16bit port numbers to identify
	the end point of a connection. All socket operations that involve
	passing addresses between kernel and user space generally
	use a struct sockaddr_in.
	(RDS 使用 IPv4 地址和 16 位端口号来标识连接的端点。所有在内核与用户空间
	之间传递地址的套接字操作，一般都使用 struct sockaddr_in。)

	The fact that IPv4 addresses are used does not mean the underlying
	transport has to be IP-based. In fact, RDS over IB uses a
	reliable IB connection; the IP address is used exclusively to
	locate the remote node's GID (by ARPing for the given IP).
	(使用 IPv4 地址并不意味着底层传输必须基于 IP。事实上，基于 IB 的 RDS 使用
	可靠的 IB 连接；IP 地址专门用于定位远端节点的 GID（通过对给定 IP 发起 ARP
	查询）。)

	The port space is entirely independent of UDP, TCP or any other
	protocol.
	(其端口空间完全独立于 UDP、TCP 或任何其他协议。)

 *	Socket interface
	(套接字接口)

	RDS sockets work *mostly* as you would expect from a BSD
	socket. The next section will cover the details. At any rate,
	all I/O is performed through the standard BSD socket API.
	Some additions like zerocopy support are implemented through
	control messages, while other extensions use the getsockopt/
	setsockopt calls.
	(RDS 套接字的工作方式与你对 BSD 套接字的预期大体一致。下一节将介绍细节。
	无论如何，所有 I/O 都通过标准 BSD 套接字 API 执行。零拷贝支持等一些附加
	功能通过控制消息实现，而其他扩展则使用 getsockopt/setsockopt 调用。)

	Sockets must be bound before you can send or receive data.
	This is needed because binding also selects a transport and
	attaches it to the socket. Once bound, the transport assignment
	does not change. RDS will tolerate IPs moving around (eg in
	a active-active HA scenario), but only as long as the address
	doesn't move to a different transport.
	(在发送或接收数据之前，套接字必须先绑定。之所以需要这样做，是因为绑定还会
	选择一个传输并将其挂接到套接字上。一旦绑定，传输的指派就不会再改变。RDS 能
	容忍 IP 地址漂移（例如在 active-active HA 场景中），但前提是地址没有漂移到
	另一个传输上。)

 *	sysctls
	(sysctl 参数)

	RDS supports a number of sysctls in /proc/sys/net/rds
	(RDS 在 /proc/sys/net/rds 中支持若干 sysctl。)


Socket Interface
(套接字接口)
================

  AF_RDS, PF_RDS, SOL_RDS
	AF_RDS and PF_RDS are the domain type to be used with socket(2)
	to create RDS sockets. SOL_RDS is the socket-level to be used
	with setsockopt(2) and getsockopt(2) for RDS specific socket
	options.
	(AF_RDS 和 PF_RDS 是与 socket(2) 一起使用、用于创建 RDS 套接字的域类型。
	SOL_RDS 是在 setsockopt(2) 和 getsockopt(2) 中用于 RDS 特有套接字选项的
	套接字层级。)

  fd = socket(PF_RDS, SOCK_SEQPACKET, 0);
	This creates a new, unbound RDS socket.
	(这会创建一个新的、未绑定的 RDS 套接字。)

  setsockopt(SOL_SOCKET): send and receive buffer size
	(setsockopt(SOL_SOCKET)：发送与接收缓冲区大小)

	RDS honors the send and receive buffer size socket options.
	You are not allowed to queue more than SO_SNDSIZE bytes to
	a socket. A message is queued when sendmsg is called, and
	it leaves the queue when the remote system acknowledges
	its arrival.
	(RDS 遵循发送与接收缓冲区大小这两个套接字选项。不允许向套接字排队超过
	SO_SNDSIZE 字节的数据。调用 sendmsg 时消息进入队列；当远端系统确认其到达
	后，消息离开队列。)

	The SO_RCVSIZE option controls the maximum receive queue length.
	This is a soft limit rather than a hard limit - RDS will
	continue to accept and queue incoming messages, even if that
	takes the queue length over the limit. However, it will also
	mark the port as "congested" and send a congestion update to
	the source node. The source node is supposed to throttle any
	processes sending to this congested port.
	(SO_RCVSIZE 选项控制最大接收队列长度。这是一个软限制而非硬限制——RDS 会
	继续接收并排队传入消息，即使这会使队列长度超过限制。但它同时会把该端口
	标记为 "congested"（拥塞），并向源节点发送拥塞更新。源节点应当对向该拥塞
	端口发送数据的所有进程进行限流。)

  bind(fd, &sockaddr_in, ...)
	This binds the socket to a local IP address and port, and a
	transport, if one has not already been selected via the
	SO_RDS_TRANSPORT socket option
	(这会把套接字绑定到本地 IP 地址和端口，并绑定一个传输——如果尚未通过
	SO_RDS_TRANSPORT 套接字选项选定传输的话)

  sendmsg(fd, ...)
	Sends a message to the indicated recipient. The kernel will
	transparently establish the underlying reliable connection
	if it isn't up yet.
	(向指定的接收者发送消息。如果底层可靠连接尚未建立，内核会透明地建立它。)

	An attempt to send a message that exceeds SO_SNDSIZE will
	return with -EMSGSIZE
	(尝试发送超过 SO_SNDSIZE 的消息会返回 -EMSGSIZE。)

	An attempt to send a message that would take the total number
	of queued bytes over the SO_SNDSIZE threshold will return
	EAGAIN.
	(尝试发送会使排队字节总数超过 SO_SNDSIZE 阈值的消息会返回 EAGAIN。)

	An attempt to send a message to a destination that is marked
	as "congested" will return ENOBUFS.
	(尝试向被标记为 "congested"（拥塞）的目的地发送消息会返回 ENOBUFS。)

  recvmsg(fd, ...)
	Receives a message that was queued to this socket. The sockets
	recv queue accounting is adjusted, and if the queue length
	drops below SO_SNDSIZE, the port is marked uncongested, and
	a congestion update is sent to all peers.
	(接收排队到该套接字的消息。套接字的接收队列统计会相应调整；如果队列长度
	降到 SO_SNDSIZE 以下，端口会被标记为无拥塞，并向所有对等端发送拥塞更新。)

	Applications can ask the RDS kernel module to receive
	notifications via control messages (for instance, there is a
	notification when a congestion update arrived, or when a RDMA
	operation completes). These notifications are received through
	the msg.msg_control buffer of struct msghdr. The format of the
	messages is described in manpages.
	(应用程序可以请求 RDS 内核模块通过控制消息接收通知（例如，拥塞更新到达
	时或 RDMA 操作完成时会有通知）。这些通知通过 struct msghdr 的
	msg.msg_control 缓冲区接收。消息的格式在手册页中描述。)

  poll(fd)
	RDS supports the poll interface to allow the application
	to implement async I/O.
	(RDS 支持 poll 接口，让应用程序能够实现异步 I/O。)

	POLLIN handling is pretty straightforward. When there's an
	incoming message queued to the socket, or a pending notification,
	we signal POLLIN.
	(POLLIN 的处理相当直接。当有传入消息排队到该套接字或有待处理的通知时，
	我们发出 POLLIN 信号。)

	POLLOUT is a little harder. Since you can essentially send
	to any destination, RDS will always signal POLLOUT as long as
	there's room on the send queue (ie the number of bytes queued
	is less than the sendbuf size).
	(POLLOUT 稍微复杂一些。由于你基本上可以向任何目的地发送，只要发送队列
	还有空间（即已排队的字节数小于 sendbuf 大小），RDS 就总是发出 POLLOUT
	信号。)

	However, the kernel will refuse to accept messages to
	a destination marked congested - in this case you will loop
	forever if you rely on poll to tell you what to do.
	This isn't a trivial problem, but applications can deal with
	this - by using congestion notifications, and by checking for
	ENOBUFS errors returned by sendmsg.
	(但是，内核会拒绝接受发往被标记为拥塞的目的地的消息——在这种情况下，
	如果你依赖 poll 来告诉你该做什么，就会永远循环下去。这不是一个简单的问题，
	但应用程序可以应对它——方法是使用拥塞通知，并检查 sendmsg 返回的
	ENOBUFS 错误。)

  setsockopt(SOL_RDS, RDS_CANCEL_SENT_TO, &sockaddr_in)
	This allows the application to discard all messages queued to a
	specific destination on this particular socket.
	(这允许应用程序丢弃在该套接字上排队发往特定目的地的所有消息。)

	This allows the application to cancel outstanding messages if
	it detects a timeout. For instance, if it tried to send a message,
	and the remote host is unreachable, RDS will keep trying forever.
	The application may decide it's not worth it, and cancel the
	operation. In this case, it would use RDS_CANCEL_SENT_TO to
	nuke any pending messages.
	(这允许应用程序在检测到超时时取消尚未完成的消息。例如，如果它尝试发送
	一条消息，而远端主机不可达，RDS 会一直尝试下去。应用程序可能认为这不值得，
	于是取消该操作。此时它会使用 RDS_CANCEL_SENT_TO 来清除所有未决消息。)

  ``setsockopt(fd, SOL_RDS, SO_RDS_TRANSPORT, (int *)&transport ..), getsockopt(fd, SOL_RDS, SO_RDS_TRANSPORT, (int *)&transport ..)``
	Set or read an integer defining  the underlying
	encapsulating transport to be used for RDS packets on the
	socket. When setting the option, integer argument may be
	one of RDS_TRANS_TCP or RDS_TRANS_IB. When retrieving the
	value, RDS_TRANS_NONE will be returned on an unbound socket.
	This socket option may only be set exactly once on the socket,
	prior to binding it via the bind(2) system call. Attempts to
	set SO_RDS_TRANSPORT on a socket for which the transport has
	been previously attached explicitly (by SO_RDS_TRANSPORT) or
	implicitly (via bind(2)) will return an error of EOPNOTSUPP.
	An attempt to set SO_RDS_TRANSPORT to RDS_TRANS_NONE will
	always return EINVAL.
	(设置或读取一个整数，该整数定义此套接字上的 RDS 数据包所使用的底层封装
	传输。设置该选项时，整数参数可以是 RDS_TRANS_TCP 或 RDS_TRANS_IB 之一。
	读取该值时，未绑定的套接字会返回 RDS_TRANS_NONE。此套接字选项在套接字上
	只能设置一次，且必须在通过 bind(2) 系统调用绑定之前设置。对传输已被显式
	（通过 SO_RDS_TRANSPORT）或隐式（通过 bind(2)）挂接的套接字设置
	SO_RDS_TRANSPORT，会返回 EOPNOTSUPP 错误。尝试把 SO_RDS_TRANSPORT 设置为
	RDS_TRANS_NONE 总是返回 EINVAL。)

RDMA for RDS
(RDS 的 RDMA)
============

  see rds-rdma(7) manpage (available in rds-tools)
  (参见 rds-rdma(7) 手册页（可在 rds-tools 中找到）)


Congestion Notifications
(拥塞通知)
========================

  see rds(7) manpage
  (参见 rds(7) 手册页)


RDS Protocol
(RDS 协议)
============

  Message header
  (消息头)

    The message header is a 'struct rds_header' (see rds.h):
    (消息头是一个 'struct rds_header'（见 rds.h）：)

    Fields:
    (字段如下：)

      h_sequence:
	  per-packet sequence number
	  (每包序列号)
      h_ack:
	  piggybacked acknowledgment of last packet received
	  (对最后收到的数据包的捎带确认)
      h_len:
	  length of data, not including header
	  (数据长度，不含头部)
      h_sport:
	  source port
	  (源端口)
      h_dport:
	  destination port
	  (目的端口)
      h_flags:
	  Can be:
	  (可以是：)

	  =============  ==================================
	  CONG_BITMAP    this is a congestion update bitmap (拥塞更新位图)
	  ACK_REQUIRED   receiver must ack this packet (接收方必须确认此数据包)
	  RETRANSMITTED  packet has previously been sent (该数据包此前已被发送过)
	  =============  ==================================

      h_credit:
	  indicate to other end of connection that
	  it has more credits available (i.e. there is
	  more send room)
	  (向连接的另一端表明它有更多可用信用（即有更多发送空间）)
      h_padding[4]:
	  unused, for future use
	  (未使用，留作将来使用)
      h_csum:
	  header checksum
	  (头部校验和)
      h_exthdr:
	  optional data can be passed here. This is currently used for
	  passing RDMA-related information.
	  (可以在此传递可选数据。目前用于传递 RDMA 相关信息。)

  ACK and retransmit handling
  (ACK 与重传处理)

      One might think that with reliable IB connections you wouldn't need
      to ack messages that have been received.  The problem is that IB
      hardware generates an ack message before it has DMAed the message
      into memory.  This creates a potential message loss if the HCA is
      disabled for any reason between when it sends the ack and before
      the message is DMAed and processed.  This is only a potential issue
      if another HCA is available for fail-over.
      (有人可能会认为，既然有了可靠的 IB 连接，就不需要对接收到的消息进行
      确认。问题在于 IB 硬件在把消息 DMA 进内存之前就会生成确认消息。如果 HCA
      在发出确认之后、消息被 DMA 并处理之前因任何原因被禁用，就可能造成消息
      丢失。只有在还有另一个 HCA 可用于故障切换时，这才是一个潜在的问题。)

      Sending an ack immediately would allow the sender to free the sent
      message from their send queue quickly, but could cause excessive
      traffic to be used for acks. RDS piggybacks acks on sent data
      packets.  Ack-only packets are reduced by only allowing one to be
      in flight at a time, and by the sender only asking for acks when
      its send buffers start to fill up. All retransmissions are also
      acked.
      (立即发送确认可以让发送方快速从其发送队列中释放已发送的消息，但可能
      导致过多的流量被用于确认。RDS 把确认捎带在已发送的数据包上。通过一次
      只允许一个纯确认包在途，并且发送方只在其发送缓冲区开始填满时才请求
      确认，纯确认包的数量得以减少。所有重传也都会被确认。)

  Flow Control
  (流量控制)

      RDS's IB transport uses a credit-based mechanism to verify that
      there is space in the peer's receive buffers for more data. This
      eliminates the need for hardware retries on the connection.
      (RDS 的 IB 传输使用基于信用的机制来验证对端的接收缓冲区中是否有空间
      容纳更多数据。这消除了在连接上进行硬件重试的需要。)

  Congestion
  (拥塞)

      Messages waiting in the receive queue on the receiving socket
      are accounted against the sockets SO_RCVBUF option value.  Only
      the payload bytes in the message are accounted for.  If the
      number of bytes queued equals or exceeds rcvbuf then the socket
      is congested.  All sends attempted to this socket's address
      should return block or return -EWOULDBLOCK.
      (接收套接字接收队列中等待的消息会计入该套接字 SO_RCVBUF 选项的值，
      且只统计消息中的载荷字节。如果排队字节数等于或超过 rcvbuf，则该套接字
      处于拥塞状态。向该套接字地址发起的所有发送都应当阻塞或返回
      -EWOULDBLOCK。)

      Applications are expected to be reasonably tuned such that this
      situation very rarely occurs.  An application encountering this
      "back-pressure" is considered a bug.
      (应用程序应当经过合理调优，使这种情况极少发生。遇到这种 "back-pressure"
      （背压）的应用程序被视为存在 bug。)

      This is implemented by having each node maintain bitmaps which
      indicate which ports on bound addresses are congested.  As the
      bitmap changes it is sent through all the connections which
      terminate in the local address of the bitmap which changed.
      (其实现方式是让每个节点维护位图，标记已绑定地址上哪些端口处于拥塞状态。
      位图一旦变化，就会通过所有终结于该位图所对应本地地址的连接发送出去。)

      The bitmaps are allocated as connections are brought up.  This
      avoids allocation in the interrupt handling path which queues
      messages on sockets.  The dense bitmaps let transports send the
      entire bitmap on any bitmap change reasonably efficiently.  This
      is much easier to implement than some finer-grained
      communication of per-port congestion.  The sender does a very
      inexpensive bit test to test if the port it's about to send to
      is congested or not.
      (位图在连接建立时分配，这避免了在向套接字排队消息的中断处理路径中进行
      内存分配。稠密位图让传输层能够在位图发生任何变化时较为高效地发送整个
      位图。这比某些更细粒度的逐端口拥塞通信要容易实现得多。发送方只需做一次
      非常廉价的位测试，即可判断它即将发送到的端口是否拥塞。)


RDS Transport Layer
(RDS 传输层)
===================

  As mentioned above, RDS is not IB-specific. Its code is divided
  into a general RDS layer and a transport layer.
  (如上所述，RDS 并非 IB 专用。其代码分为通用 RDS 层和传输层。)

  The general layer handles the socket API, congestion handling,
  loopback, stats, usermem pinning, and the connection state machine.
  (通用层负责套接字 API、拥塞处理、环回、统计、用户内存固定（usermem
  pinning）以及连接状态机。)

  The transport layer handles the details of the transport. The IB
  transport, for example, handles all the queue pairs, work requests,
  CM event handlers, and other Infiniband details.
  (传输层负责传输的具体细节。例如，IB 传输处理所有队列对、工作请求、CM
  事件处理程序以及其他 Infiniband 细节。)


RDS Kernel Structures
(RDS 内核结构)
=====================

  struct rds_message
    aka possibly "rds_outgoing", the generic RDS layer copies data to
    be sent and sets header fields as needed, based on the socket API.
    This is then queued for the individual connection and sent by the
    connection's transport.
    (又可能称作 "rds_outgoing"。通用 RDS 层根据套接字 API 把要发送的数据
    复制进来，并按需设置头部字段。随后它被排队到各个连接，由该连接的传输层
    发送。)

  struct rds_incoming
    a generic struct referring to incoming data that can be handed from
    the transport to the general code and queued by the general code
    while the socket is awoken. It is then passed back to the transport
    code to handle the actual copy-to-user.
    (一个引用传入数据的通用结构，可以由传输层交给通用代码，并在套接字被
    唤醒期间由通用代码排队。之后它被传回传输层代码，以执行实际的复制到
    用户空间操作。)

  struct rds_socket
    per-socket information
    (每套接字信息)

  struct rds_connection
    per-connection information
    (每连接信息)

  struct rds_transport
    pointers to transport-specific functions
    (指向传输层特定函数的指针)

  struct rds_statistics
    non-transport-specific statistics
    (非传输层特定的统计信息)

  struct rds_cong_map
    wraps the raw congestion bitmap, contains rbnode, waitq, etc.
    (封装原始拥塞位图，包含 rbnode、waitq 等)

Connection management
(连接管理)
=====================

  Connections may be in UP, DOWN, CONNECTING, DISCONNECTING, and
  ERROR states.
  (连接可以处于 UP、DOWN、CONNECTING、DISCONNECTING 和 ERROR 状态。)

  The first time an attempt is made by an RDS socket to send data to
  a node, a connection is allocated and connected. That connection is
  then maintained forever -- if there are transport errors, the
  connection will be dropped and re-established.
  (当 RDS 套接字第一次尝试向某个节点发送数据时，会分配并连接一个连接。此后
  该连接会被一直维护——如果发生传输错误，连接会被断开并重新建立。)

  Dropping a connection while packets are queued will cause queued or
  partially-sent datagrams to be retransmitted when the connection is
  re-established.
  (在数据包仍处于排队状态时断开连接，会导致已排队或部分发送的数据报在连接
  重新建立后被重传。)


The send path
(发送路径)
=============

  rds_sendmsg()
    - struct rds_message built from incoming data
      (由传入数据构建 struct rds_message)
    - CMSGs parsed (e.g. RDMA ops)
      (解析 CMSG（例如 RDMA 操作）)
    - transport connection allocated and connected if not already
      (分配并连接传输连接（如果尚未建立）)
    - rds_message placed on send queue
      (把 rds_message 放入发送队列)
    - send worker awoken
      (唤醒发送工作线程)

  rds_send_worker()
    - calls rds_send_xmit() until queue is empty
      (调用 rds_send_xmit() 直到队列为空)

  rds_send_xmit()
    - transmits congestion map if one is pending
      (如果拥塞映射待发送，则发送它)
    - may set ACK_REQUIRED
      (可能设置 ACK_REQUIRED)
    - calls transport to send either non-RDMA or RDMA message
      (RDMA ops never retransmitted)
      (调用传输层发送非 RDMA 或 RDMA 消息（RDMA 操作从不重传）)

  rds_ib_xmit()
    - allocs work requests from send ring
      (从发送环分配工作请求)
    - adds any new send credits available to peer (h_credits)
      (把所有新的可用发送信用加给对端（h_credits）)
    - maps the rds_message's sg list
      (映射 rds_message 的 sg 列表)
    - piggybacks ack
      (捎带确认)
    - populates work requests
      (填充工作请求)
    - post send to connection's queue pair
      (把发送投递到连接的队列对)

The recv path
(接收路径)
=============

  rds_ib_recv_cq_comp_handler()
    - looks at write completions
      (检查写完成事件)
    - unmaps recv buffer from device
      (从设备解除接收缓冲区的映射)
    - no errors, call rds_ib_process_recv()
      (若无错误，调用 rds_ib_process_recv())
    - refill recv ring
      (补充接收环)

  rds_ib_process_recv()
    - validate header checksum
      (校验头部校验和)
    - copy header to rds_ib_incoming struct if start of a new datagram
      (如果这是新数据报的开头，把头部复制到 rds_ib_incoming 结构)
    - add to ibinc's fraglist
      (加入 ibinc 的 fraglist)
    - if completed datagram:
      (如果数据报已完成：)

	 - update cong map if datagram was cong update
	   (如果数据报是拥塞更新，则更新拥塞映射)
	 - call rds_recv_incoming() otherwise
	   (否则调用 rds_recv_incoming())
	 - note if ack is required
	   (记录是否需要确认)

  rds_recv_incoming()
    - drop duplicate packets
      (丢弃重复的数据包)
    - respond to pings
      (响应 ping)
    - find the sock associated with this datagram
      (找到与此数据报关联的 sock)
    - add to sock queue
      (加入 sock 队列)
    - wake up sock
      (唤醒 sock)
    - do some congestion calculations
      (做一些拥塞计算)
  rds_recvmsg
    - copy data into user iovec
      (把数据复制到用户 iovec)
    - handle CMSGs
      (处理 CMSG)
    - return to application
      (返回给应用程序)

Multipath RDS (mprds)
(多路径 RDS (mprds))
=====================

  Mprds is multipathed-RDS, primarily intended for RDS-over-TCP
  (though the concept can be extended to other transports). The classical
  implementation of RDS-over-TCP is implemented by demultiplexing multiple
  PF_RDS sockets between any 2 endpoints (where endpoint == [IP address,
  port]) over a single TCP socket between the 2 IP addresses involved. This
  has the limitation that it ends up funneling multiple RDS flows over a
  single TCP flow, thus it is
  (a) upper-bounded to the single-flow bandwidth,
  (b) suffers from head-of-line blocking for all the RDS sockets.
  (Mprds 即多路径 RDS，主要面向 RDS-over-TCP（不过该概念可以扩展到其他传输）。
  经典的 RDS-over-TCP 实现方式是：在所涉及的两个 IP 地址之间的单个 TCP 套接字
  上，对任意 2 个端点（端点 == [IP 地址, 端口]）之间的多个 PF_RDS 套接字进行
  分用。其局限在于，它最终把多条 RDS 流汇聚到单条 TCP 流上，因此：
  (a) 吞吐量上限受限于单条流的带宽；
  (b) 所有 RDS 套接字都会受到队头阻塞的影响。)

  Better throughput (for a fixed small packet size, MTU) can be achieved
  by having multiple TCP/IP flows per rds/tcp connection, i.e., multipathed
  RDS (mprds).  Each such TCP/IP flow constitutes a path for the rds/tcp
  connection. RDS sockets will be attached to a path based on some hash
  (e.g., of local address and RDS port number) and packets for that RDS
  socket will be sent over the attached path using TCP to segment/reassemble
  RDS datagrams on that path.
  (更好的吞吐量（对于固定的小数据包大小 MTU）可以通过让每个 rds/tcp 连接拥有
  多条 TCP/IP 流来实现，即多路径 RDS（mprds）。每条这样的 TCP/IP 流构成
  rds/tcp 连接的一个路径（path）。RDS 套接字会依据某种哈希（例如本地地址与
  RDS 端口号）附着到某个路径上，该 RDS 套接字的数据包将通过附着的路径发送，
  由 TCP 在该路径上对 RDS 数据报进行分段/重组。)

  Multipathed RDS is implemented by splitting the struct rds_connection into
  a common (to all paths) part, and a per-path struct rds_conn_path. All
  I/O workqs and reconnect threads are driven from the rds_conn_path.
  Transports such as TCP that are multipath capable may then set up a
  TCP socket per rds_conn_path, and this is managed by the transport via
  the transport private cp_transport_data pointer.
  (多路径 RDS 的实现方式是把 struct rds_connection 拆分为一个公共（对所有路径
  共用）部分和一个每路径的 struct rds_conn_path。所有 I/O 工作队列和重连线程
  都由 rds_conn_path 驱动。像 TCP 这样支持多路径的传输随后可以为每个
  rds_conn_path 建立一个 TCP 套接字，传输层通过传输私有的 cp_transport_data
  指针来管理它。)

  Transports announce themselves as multipath capable by setting the
  t_mp_capable bit during registration with the rds core module. When the
  transport is multipath-capable, rds_sendmsg() hashes outgoing traffic
  across multiple paths. The outgoing hash is computed based on the
  local address and port that the PF_RDS socket is bound to.
  (传输层在向 rds 核心模块注册时通过设置 t_mp_capable 位来宣告自己支持多路径。
  当传输层支持多路径时，rds_sendmsg() 会把出向流量哈希到多条路径上。出向哈希
  根据 PF_RDS 套接字绑定的本地地址和端口计算。)

  Additionally, even if the transport is MP capable, we may be
  peering with some node that does not support mprds, or supports
  a different number of paths. As a result, the peering nodes need
  to agree on the number of paths to be used for the connection.
  This is done by sending out a control packet exchange before the
  first data packet. The control packet exchange must have completed
  prior to outgoing hash completion in rds_sendmsg() when the transport
  is multipath capable.
  (此外，即使传输层支持多路径，与之对等的节点也可能不支持 mprds，或支持的
  路径数不同。因此，对等节点需要就连接要使用的路径数达成一致。这是在第一个
  数据包之前通过交换控制数据包来完成的。当传输层支持多路径时，控制数据包的
  交换必须在 rds_sendmsg() 中完成出向哈希之前结束。)

  The control packet is an RDS ping packet (i.e., packet to rds dest
  port 0) with the ping packet having a rds extension header option  of
  type RDS_EXTHDR_NPATHS, length 2 bytes, and the value is the
  number of paths supported by the sender. The "probe" ping packet will
  get sent from some reserved port, RDS_FLAG_PROBE_PORT (in <linux/rds.h>)
  The receiver of a ping from RDS_FLAG_PROBE_PORT will thus immediately
  be able to compute the min(sender_paths, rcvr_paths). The pong
  sent in response to a probe-ping should contain the rcvr's npaths
  when the rcvr is mprds-capable.
  (控制数据包是一个 RDS ping 数据包（即发往 rds 目的端口 0 的数据包），该 ping
  数据包带有一个 rds 扩展头选项，类型为 RDS_EXTHDR_NPATHS，长度 2 字节，其值
  为发送方支持的路径数。"probe"（探测）ping 数据包将从某个保留端口
  RDS_FLAG_PROBE_PORT（位于 <linux/rds.h> 中）发出。因此，从 RDS_FLAG_PROBE_PORT
  收到 ping 的接收方可以立即计算出 min(sender_paths, rcvr_paths)。当接收方
  支持 mprds 时，对探测 ping 的回应 pong 应包含接收方的 npaths。)

  If the rcvr is not mprds-capable, the exthdr in the ping will be
  ignored.  In this case the pong will not have any exthdrs, so the sender
  of the probe-ping can default to single-path mprds.
  (如果接收方不支持 mprds，ping 中的扩展头将被忽略。此时 pong 将不携带任何
  扩展头，因此探测 ping 的发送方可以默认使用单路径 mprds。)

