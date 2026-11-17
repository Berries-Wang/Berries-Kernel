.. SPDX-License-Identifier: GPL-2.0

======================================
RxRPC Network Protocol(RxRPC 网络协议)
======================================

The RxRPC protocol driver provides a reliable two-phase transport on top of UDP
that can be used to perform RxRPC remote operations.  This is done over sockets
of AF_RXRPC family, using sendmsg() and recvmsg() with control data to send and
receive data, aborts and errors.
(RxRPC 协议驱动程序在 UDP 之上提供了一个可靠的两阶段传输，可用于执行 RxRPC 远程操
作。这是通过 AF_RXRPC 协议族的套接字完成的，使用带控制数据的 sendmsg() 和 recvmsg()
来发送和接收数据、中止与错误。)

Contents of this document:
(本文档的内容：)

 (#) Overview.
     (概述。)

 (#) RxRPC protocol summary.
     (RxRPC 协议概要。)

 (#) AF_RXRPC driver model.
     (AF_RXRPC 驱动程序模型。)

 (#) Control messages.
     (控制消息。)

 (#) Socket options.
     (套接字选项。)

 (#) Security.
     (安全性。)

 (#) Example client usage.
     (客户端使用示例。)

 (#) Example server usage.
     (服务器使用示例。)

 (#) AF_RXRPC kernel interface.
     (AF_RXRPC 内核接口。)

 (#) Configurable parameters.
     (可配置参数。)


Overview(概述)
==============

RxRPC is a two-layer protocol.  There is a session layer which provides
reliable virtual connections using UDP over IPv4 (or IPv6) as the transport
layer, but implements a real network protocol; and there's the presentation
layer which renders structured data to binary blobs and back again using XDR
(as does SunRPC)::

		+-------------+
		| Application |
		+-------------+
		|     XDR     |		Presentation
		+-------------+
		|    RxRPC    |		Session
		+-------------+
		|     UDP     |		Transport
		+-------------+

(RxRPC 是一个两层协议。会话层使用基于 IPv4 (或 IPv6) 的 UDP 作为传输层来提供可靠的虚
拟连接，但它本身实现为一个真正的网络协议；表示层则使用 XDR 将结构化数据渲染为二进制
数据块或反向转换 (与 SunRPC 相同)：)

AF_RXRPC provides:
(AF_RXRPC 提供：)

 (1) Part of an RxRPC facility for both kernel and userspace applications by
     making the session part of it a Linux network protocol (AF_RXRPC).
     (通过将其会话部分做成 Linux 网络协议 (AF_RXRPC)，为内核和用户空间应用程序提供
     RxRPC 设施的一部分。)

 (2) A two-phase protocol.  The client transmits a blob (the request) and then
     receives a blob (the reply), and the server receives the request and then
     transmits the reply.
     (一种两阶段协议。客户端发送一个数据块 (请求)，然后接收一个数据块 (回复)；服务器
     接收请求，然后发送回复。)

 (3) Retention of the reusable bits of the transport system set up for one call
     to speed up subsequent calls.
     (保留为某次调用而建立的传输系统中可复用的部分，以加速后续调用。)

 (4) A secure protocol, using the Linux kernel's key retention facility to
     manage security on the client end.  The server end must of necessity be
     more active in security negotiations.
     (一种安全协议，使用 Linux 内核的密钥保留设施来管理客户端一侧的安全。服务器一侧
     在安全协商中必然要更为主动。)

AF_RXRPC does not provide XDR marshalling/presentation facilities.  That is
left to the application.  AF_RXRPC only deals in blobs.  Even the operation ID
is just the first four bytes of the request blob, and as such is beyond the
kernel's interest.
(AF_RXRPC 不提供 XDR 编组/表示设施。这留给应用程序来完成。AF_RXRPC 只处理数据块。甚
至操作 ID 也只是请求数据块的前四个字节，因此内核对此并不关心。)


Sockets of AF_RXRPC family are:
(AF_RXRPC 协议族的套接字：)

 (1) created as type SOCK_DGRAM;
     (以 SOCK_DGRAM 类型创建；)

 (2) provided with a protocol of the type of underlying transport they're going
     to use - currently only PF_INET is supported.
     (被赋予一个指明其将使用的底层传输类型的协议——目前仅支持 PF_INET。)


The Andrew File System (AFS) is an example of an application that uses this and
that has both kernel (filesystem) and userspace (utility) components.
(Andrew 文件系统 (AFS) 是使用它的一个应用程序示例，它同时拥有内核 (文件系统) 和用户
空间 (工具) 组件。)


RxRPC Protocol Summary(RxRPC 协议概要)
======================================

An overview of the RxRPC protocol:
(RxRPC 协议概览：)

 (#) RxRPC sits on top of another networking protocol (UDP is the only option
     currently), and uses this to provide network transport.  UDP ports, for
     example, provide transport endpoints.
     (RxRPC 位于另一个网络协议之上 (UDP 是目前唯一的选择)，并使用它来提供网络传输。
     例如，UDP 端口提供传输端点。)

 (#) RxRPC supports multiple virtual "connections" from any given transport
     endpoint, thus allowing the endpoints to be shared, even to the same
     remote endpoint.
     (RxRPC 支持从任何给定的传输端点发起多个虚拟 "连接"，从而允许共享端点，甚至共享
     到同一个远程端点。)

 (#) Each connection goes to a particular "service".  A connection may not go
     to multiple services.  A service may be considered the RxRPC equivalent of
     a port number.  AF_RXRPC permits multiple services to share an endpoint.
     (每条连接都通向一个特定的 "服务"。一条连接不能通向多个服务。服务可视为 RxRPC 中
     与端口号对应的概念。AF_RXRPC 允许多个服务共享一个端点。)

 (#) Client-originating packets are marked, thus a transport endpoint can be
     shared between client and server connections (connections have a
     direction).
     (客户端发起的数据包会被打上标记，因此一个传输端点可以在客户端连接和服务器连接之
     间共享 (连接具有方向性)。)

 (#) Up to a billion connections may be supported concurrently between one
     local transport endpoint and one service on one remote endpoint.  An RxRPC
     connection is described by seven numbers::
     (在一个本地传输端点与一个远程端点上的一个服务之间，最多可并发支持十亿条连接。一
     条 RxRPC 连接由七个数字描述：)

	Local address	}
	Local port	} Transport (UDP) address
	Remote address	}
	Remote port	}
	Direction
	Connection ID
	Service ID

 (#) Each RxRPC operation is a "call".  A connection may make up to four
     billion calls, but only up to four calls may be in progress on a
     connection at any one time.
     (每个 RxRPC 操作都是一个 "调用 (call)"。一条连接最多可以发起四十亿次调用，但任
     何时候一条连接上最多只能有四个调用同时进行。)

 (#) Calls are two-phase and asymmetric: the client sends its request data,
     which the service receives; then the service transmits the reply data
     which the client receives.
     (调用是两阶段且非对称的：客户端发送其请求数据，由服务接收；然后服务发送回复数
     据，由客户端接收。)

 (#) The data blobs are of indefinite size, the end of a phase is marked with a
     flag in the packet.  The number of packets of data making up one blob may
     not exceed 4 billion, however, as this would cause the sequence number to
     wrap.
     (数据块的大小不定，阶段的结束由数据包中的一个标志标记。不过，组成一个数据块的数
     据包数量不能超过 40 亿，否则会导致序列号回绕。)

 (#) The first four bytes of the request data are the service operation ID.
     (请求数据的前四个字节是服务操作 ID。)

 (#) Security is negotiated on a per-connection basis.  The connection is
     initiated by the first data packet on it arriving.  If security is
     requested, the server then issues a "challenge" and then the client
     replies with a "response".  If the response is successful, the security is
     set for the lifetime of that connection, and all subsequent calls made
     upon it use that same security.  In the event that the server lets a
     connection lapse before the client, the security will be renegotiated if
     the client uses the connection again.
     (安全性按连接逐条协商。连接由其上第一个数据包到达时发起。如果请求了安全性，服务
     器随后发出一个 "challenge" (质询)，然后客户端以 "response" (响应) 回答。如果响
     应成功，则在该连接的生命周期内安全设置即告确定，其后在该连接上发起的所有调用都使
     用相同的安全机制。如果服务器在客户端之前让连接失效，当客户端再次使用该连接时，将
     重新协商安全性。)

 (#) Calls use ACK packets to handle reliability.  Data packets are also
     explicitly sequenced per call.
     (调用使用 ACK 数据包来处理可靠性。数据包也按调用显式排序。)

 (#) There are two types of positive acknowledgment: hard-ACKs and soft-ACKs.
     A hard-ACK indicates to the far side that all the data received to a point
     has been received and processed; a soft-ACK indicates that the data has
     been received but may yet be discarded and re-requested.  The sender may
     not discard any transmittable packets until they've been hard-ACK'd.
     (肯定确认有两种类型：硬 ACK (hard-ACK) 和软 ACK (soft-ACK)。硬 ACK 告知对端，到
     某一点为止的所有数据都已接收并处理完毕；软 ACK 表示数据已接收，但仍可能被丢弃并
     重新请求。在数据包被硬 ACK 之前，发送方不得丢弃任何可发送的数据包。)

 (#) Reception of a reply data packet implicitly hard-ACK's all the data
     packets that make up the request.
     (接收到回复数据包即隐式地对组成请求的所有数据包进行了硬 ACK。)

 (#) An call is complete when the request has been sent, the reply has been
     received and the final hard-ACK on the last packet of the reply has
     reached the server.
     (当请求已发送、回复已接收，且对回复最后一个数据包的最终硬 ACK 已到达服务器时，
     调用即告完成。)

 (#) An call may be aborted by either end at any time up to its completion.
     (在调用完成之前的任何时刻，任一端都可以中止该调用。)


AF_RXRPC Driver Model(AF_RXRPC 驱动程序模型)
============================================

About the AF_RXRPC driver:
(关于 AF_RXRPC 驱动程序：)

 (#) The AF_RXRPC protocol transparently uses internal sockets of the transport
     protocol to represent transport endpoints.
     (AF_RXRPC 协议透明地使用传输协议的内部套接字来表示传输端点。)

 (#) AF_RXRPC sockets map onto RxRPC connection bundles.  Actual RxRPC
     connections are handled transparently.  One client socket may be used to
     make multiple simultaneous calls to the same service.  One server socket
     may handle calls from many clients.
     (AF_RXRPC 套接字映射到 RxRPC 连接束上。实际的 RxRPC 连接被透明地处理。一个客户
     端套接字可用于向同一服务发起多个并发调用。一个服务器套接字可以处理来自多个客户端
     的调用。)

 (#) Additional parallel client connections will be initiated to support extra
     concurrent calls, up to a tunable limit.
     (将发起额外的并行客户端连接以支持更多并发调用，上限可调。)

 (#) Each connection is retained for a certain amount of time [tunable] after
     the last call currently using it has completed in case a new call is made
     that could reuse it.
     (在当前使用它的最后一个调用完成之后，每条连接会保留一段时间 [可调]，以防有可以
     复用它的新调用发起。)

 (#) Each internal UDP socket is retained [tunable] for a certain amount of
     time [tunable] after the last connection using it discarded, in case a new
     connection is made that could use it.
     (在最后一个使用它的连接被丢弃之后，每个内部 UDP 套接字会保留一段时间 [可调]，
     以防有可以使用它的新连接建立。)

 (#) A client-side connection is only shared between calls if they have
     the same key struct describing their security (and assuming the calls
     would otherwise share the connection).  Non-secured calls would also be
     able to share connections with each other.
     (客户端一侧的连接只有在各调用拥有描述其安全性的相同 key 结构时才会在调用之间共
     享 (且假设这些调用在其他方面可共享连接)。未加密的调用之间也可以共享连接。)

 (#) A server-side connection is shared if the client says it is.
     (服务器一侧的连接在客户端声明可共享时即被共享。)

 (#) ACK'ing is handled by the protocol driver automatically, including ping
     replying.
     (ACK 由协议驱动程序自动处理，包括对 ping 的应答。)

 (#) SO_KEEPALIVE automatically pings the other side to keep the connection
     alive [TODO].
     (SO_KEEPALIVE 自动 ping 对端以保持连接存活 [TODO]。)

 (#) If an ICMP error is received, all calls affected by that error will be
     aborted with an appropriate network error passed through recvmsg().
     (如果收到 ICMP 错误，所有受该错误影响的调用都将被中止，并通过 recvmsg() 传递一
     个相应的网络错误。)


Interaction with the user of the RxRPC socket:
(与 RxRPC 套接字使用者的交互：)

 (#) A socket is made into a server socket by binding an address with a
     non-zero service ID.
     (通过绑定一个带有非零服务 ID 的地址，可将套接字变为服务器套接字。)

 (#) In the client, sending a request is achieved with one or more sendmsgs,
     followed by the reply being received with one or more recvmsgs.
     (在客户端，发送请求通过一次或多次 sendmsg 完成，随后通过一次或多次 recvmsg 接
     收回复。)

 (#) The first sendmsg for a request to be sent from a client contains a tag to
     be used in all other sendmsgs or recvmsgs associated with that call.  The
     tag is carried in the control data.
     (客户端发送请求的第一次 sendmsg 包含一个标签 (tag)，该标签将用于与该调用相关的
     所有其他 sendmsg 或 recvmsg。标签携带在控制数据中。)

 (#) connect() is used to supply a default destination address for a client
     socket.  This may be overridden by supplying an alternate address to the
     first sendmsg() of a call (struct msghdr::msg_name).
     (connect() 用于为客户端套接字提供默认目的地址。可以通过向调用的第一次 sendmsg()
     提供替代地址 (struct msghdr::msg_name) 来覆盖它。)

 (#) If connect() is called on an unbound client, a random local port will
     bound before the operation takes place.
     (如果对未绑定的客户端调用 connect()，在操作进行之前会先绑定一个随机本地端口。)

 (#) A server socket may also be used to make client calls.  To do this, the
     first sendmsg() of the call must specify the target address.  The server's
     transport endpoint is used to send the packets.
     (服务器套接字也可以用来发起客户端调用。为此，调用的第一次 sendmsg() 必须指定目
     标地址。数据包将使用服务器的传输端点发送。)

 (#) Once the application has received the last message associated with a call,
     the tag is guaranteed not to be seen again, and so it can be used to pin
     client resources.  A new call can then be initiated with the same tag
     without fear of interference.
     (一旦应用程序收到了与某调用相关的最后一条消息，该标签就保证不会再出现，因此可以
     用它来固定 (pin) 客户端资源。随后可以用同一标签发起新调用，而不必担心干扰。)

 (#) In the server, a request is received with one or more recvmsgs, then the
     the reply is transmitted with one or more sendmsgs, and then the final ACK
     is received with a last recvmsg.
     (在服务器端，请求通过一次或多次 recvmsg 接收，然后回复通过一次或多次 sendmsg 发
     送，最后通过一次 recvmsg 接收最终 ACK。)

 (#) When sending data for a call, sendmsg is given MSG_MORE if there's more
     data to come on that call.
     (发送调用的数据时，如果该调用还有更多数据要来，则给 sendmsg 传入 MSG_MORE。)

 (#) When receiving data for a call, recvmsg flags MSG_MORE if there's more
     data to come for that call.
     (接收调用的数据时，如果该调用还有更多数据要来，recvmsg 会设置 MSG_MORE 标志。)

 (#) When receiving data or messages for a call, MSG_EOR is flagged by recvmsg
     to indicate the terminal message for that call.
     (接收调用的数据或消息时，recvmsg 会设置 MSG_EOR 标志，以指示该调用的终结消息。)

 (#) A call may be aborted by adding an abort control message to the control
     data.  Issuing an abort terminates the kernel's use of that call's tag.
     Any messages waiting in the receive queue for that call will be discarded.
     (通过向控制数据添加一个中止控制消息可以中止调用。发出中止会终止内核对该调用标签
     的使用。该调用在接收队列中等待的所有消息都将被丢弃。)

 (#) Aborts, busy notifications and challenge packets are delivered by recvmsg,
     and control data messages will be set to indicate the context.  Receiving
     an abort or a busy message terminates the kernel's use of that call's tag.
     (中止、忙通知和质询数据包通过 recvmsg 传递，控制数据消息会被设置以指示上下文。
     接收到中止或忙消息会终止内核对该调用标签的使用。)

 (#) The control data part of the msghdr struct is used for a number of things:
     (msghdr 结构的控制数据部分用于多种用途：)

     (#) The tag of the intended or affected call.
         (目标或受影响调用的标签。)

     (#) Sending or receiving errors, aborts and busy notifications.
         (发送或接收错误、中止和忙通知。)

     (#) Notifications of incoming calls.
         (传入调用的通知。)

     (#) Sending debug requests and receiving debug replies [TODO].
         (发送调试请求和接收调试回复 [TODO]。)

 (#) When the kernel has received and set up an incoming call, it sends a
     message to server application to let it know there's a new call awaiting
     its acceptance [recvmsg reports a special control message].  The server
     application then uses sendmsg to assign a tag to the new call.  Once that
     is done, the first part of the request data will be delivered by recvmsg.
     (当内核收到并设置好一个传入调用后，它会向服务器应用程序发送一条消息，告知有一个
     新调用等待其接受 [recvmsg 报告一个特殊的控制消息]。然后服务器应用程序使用 sendmsg
     为新调用分配一个标签。完成后，请求数据的第一部分将通过 recvmsg 传递。)

 (#) The server application has to provide the server socket with a keyring of
     secret keys corresponding to the security types it permits.  When a secure
     connection is being set up, the kernel looks up the appropriate secret key
     in the keyring and then sends a challenge packet to the client and
     receives a response packet.  The kernel then checks the authorisation of
     the packet and either aborts the connection or sets up the security.
     (服务器应用程序必须向服务器套接字提供一个密钥环，其中包含与其允许的安全类型相对
     应的秘密密钥。在建立安全连接时，内核会在密钥环中查找相应的秘密密钥，然后向客户端
     发送质询数据包并接收响应数据包。随后内核检查该数据包的授权，要么中止连接，要么设
     置好安全性。)

 (#) The name of the key a client will use to secure its communications is
     nominated by a socket option.
     (客户端用来保护其通信的密钥名称由一个套接字选项指定。)


Notes on sendmsg:
(关于 sendmsg 的说明：)

 (#) MSG_WAITALL can be set to tell sendmsg to ignore signals if the peer is
     making progress at accepting packets within a reasonable time such that we
     manage to queue up all the data for transmission.  This requires the
     client to accept at least one packet per 2*RTT time period.
     (可以设置 MSG_WAITALL，告诉 sendmsg 在对端以合理时间进度接收数据包、从而使我们
     能够将所有待发数据排入队列时忽略信号。这要求客户端每 2*RTT 时间段至少接收一个数
     据包。)

     If this isn't set, sendmsg() will return immediately, either returning
     EINTR/ERESTARTSYS if nothing was consumed or returning the amount of data
     consumed.
     (如果未设置此标志，sendmsg() 将立即返回：若没有消耗任何数据则返回
     EINTR/ERESTARTSYS，否则返回已消耗的数据量。)


Notes on recvmsg:
(关于 recvmsg 的说明：)

 (#) If there's a sequence of data messages belonging to a particular call on
     the receive queue, then recvmsg will keep working through them until:
     (如果接收队列中存在属于某个特定调用的一系列数据消息，recvmsg 将持续处理它们，直
     到：)

     (a) it meets the end of that call's received data,
         (遇到该调用已接收数据的末尾，)

     (b) it meets a non-data message,
         (遇到一条非数据消息，)

     (c) it meets a message belonging to a different call, or
         (遇到一条属于其他调用的消息，或)

     (d) it fills the user buffer.
         (填满用户缓冲区。)

     If recvmsg is called in blocking mode, it will keep sleeping, awaiting the
     reception of further data, until one of the above four conditions is met.
     (如果以阻塞模式调用 recvmsg，它将持续睡眠、等待接收更多数据，直到满足上述四个条
     件之一。)

 (2) MSG_PEEK operates similarly, but will return immediately if it has put any
     data in the buffer rather than sleeping until it can fill the buffer.
     (MSG_PEEK 的行为类似，但如果它已向缓冲区放入了任何数据，将立即返回，而不是睡眠
     等待直到填满缓冲区。)

 (3) If a data message is only partially consumed in filling a user buffer,
     then the remainder of that message will be left on the front of the queue
     for the next taker.  MSG_TRUNC will never be flagged.
     (如果一条数据消息在填充用户缓冲区时只被部分消耗，该消息的剩余部分将留在队列头部
     供下一个取用者处理。MSG_TRUNC 永远不会被设置。)

 (4) If there is more data to be had on a call (it hasn't copied the last byte
     of the last data message in that phase yet), then MSG_MORE will be
     flagged.
     (如果调用上还有更多数据可获得 (尚未复制该阶段最后一条数据消息的最后一个字节)，
     则会设置 MSG_MORE 标志。)


Control Messages(控制消息)
==========================

AF_RXRPC makes use of control messages in sendmsg() and recvmsg() to multiplex
calls, to invoke certain actions and to report certain conditions.  These are:
(AF_RXRPC 利用 sendmsg() 和 recvmsg() 中的控制消息来复用调用、触发某些动作并报告某些
状况。它们是：)

	=======================	=== ===========	===============================
	MESSAGE ID		SRT DATA	MEANING (含义)
	=======================	=== ===========	===============================
	RXRPC_USER_CALL_ID	sr- User ID	App's call specifier (应用的调用标识符)
	RXRPC_ABORT		srt Abort code	Abort code to issue/received (要发出/已接收的中止码)
	RXRPC_ACK		-rt n/a		Final ACK received (已收到最终 ACK)
	RXRPC_NET_ERROR		-rt error num	Network error on call (调用上的网络错误)
	RXRPC_BUSY		-rt n/a		Call rejected (server busy) (调用被拒绝 (服务器忙))
	RXRPC_LOCAL_ERROR	-rt error num	Local error encountered (遇到本地错误)
	RXRPC_NEW_CALL		-r- n/a		New call received (收到新调用)
	RXRPC_ACCEPT		s-- n/a		Accept new call (接受新调用)
	RXRPC_EXCLUSIVE_CALL	s-- n/a		Make an exclusive client call (发起独占的客户端调用)
	RXRPC_UPGRADE_SERVICE	s-- n/a		Client call can be upgraded (客户端调用可被升级)
	RXRPC_TX_LENGTH		s-- data len	Total length of Tx data (发送数据的总长度)
	=======================	=== ===========	===============================

	(SRT = usable in Sendmsg / delivered by Recvmsg / Terminal message)
	(SRT = 可用于 Sendmsg / 由 Recvmsg 传递 / 终结消息)

 (#) RXRPC_USER_CALL_ID

     This is used to indicate the application's call ID.  It's an unsigned long
     that the app specifies in the client by attaching it to the first data
     message or in the server by passing it in association with an RXRPC_ACCEPT
     message.  recvmsg() passes it in conjunction with all messages except
     those of the RXRPC_NEW_CALL message.
     (用于指示应用程序的调用 ID。它是一个无符号长整型，客户端通过将它附加到第一条数
     据消息上来指定，服务器则通过将其与一条 RXRPC_ACCEPT 消息一起传递来指定。
     recvmsg() 会随除 RXRPC_NEW_CALL 消息之外的所有消息一起传递它。)

 (#) RXRPC_ABORT

     This is can be used by an application to abort a call by passing it to
     sendmsg, or it can be delivered by recvmsg to indicate a remote abort was
     received.  Either way, it must be associated with an RXRPC_USER_CALL_ID to
     specify the call affected.  If an abort is being sent, then error EBADSLT
     will be returned if there is no call with that user ID.
     (应用程序可以将它传递给 sendmsg 来中止一个调用；recvmsg 也可能传递它，表示收到
     了远端的中止。无论哪种方式，它都必须与一个 RXRPC_USER_CALL_ID 相关联，以指明受
     影响的调用。如果正在发送中止，且不存在具有该用户 ID 的调用，则将返回错误
     EBADSLT。)

 (#) RXRPC_ACK

     This is delivered to a server application to indicate that the final ACK
     of a call was received from the client.  It will be associated with an
     RXRPC_USER_CALL_ID to indicate the call that's now complete.
     (它被传递给服务器应用程序，表示已从客户端收到某个调用的最终 ACK。它将与一个
     RXRPC_USER_CALL_ID 相关联，以指明现已完成的调用。)

 (#) RXRPC_NET_ERROR

     This is delivered to an application to indicate that an ICMP error message
     was encountered in the process of trying to talk to the peer.  An
     errno-class integer value will be included in the control message data
     indicating the problem, and an RXRPC_USER_CALL_ID will indicate the call
     affected.
     (它被传递给应用程序，表示在尝试与对端通信的过程中遇到了 ICMP 错误消息。控制消息
     数据中会包含一个 errno 类的整数值以指明问题，并由一个 RXRPC_USER_CALL_ID 指明受
     影响的调用。)

 (#) RXRPC_BUSY

     This is delivered to a client application to indicate that a call was
     rejected by the server due to the server being busy.  It will be
     associated with an RXRPC_USER_CALL_ID to indicate the rejected call.
     (它被传递给客户端应用程序，表示某调用因服务器忙而被服务器拒绝。它将与一个
     RXRPC_USER_CALL_ID 相关联，以指明被拒绝的调用。)

 (#) RXRPC_LOCAL_ERROR

     This is delivered to an application to indicate that a local error was
     encountered and that a call has been aborted because of it.  An
     errno-class integer value will be included in the control message data
     indicating the problem, and an RXRPC_USER_CALL_ID will indicate the call
     affected.
     (它被传递给应用程序，表示遇到了本地错误，且某个调用因此被中止。控制消息数据中会
     包含一个 errno 类的整数值以指明问题，并由一个 RXRPC_USER_CALL_ID 指明受影响的调
     用。)

 (#) RXRPC_NEW_CALL

     This is delivered to indicate to a server application that a new call has
     arrived and is awaiting acceptance.  No user ID is associated with this,
     as a user ID must subsequently be assigned by doing an RXRPC_ACCEPT.
     (它被传递给服务器应用程序，表示有一个新调用已到达并等待接受。它不关联任何用户
     ID，因为用户 ID 必须随后通过执行一次 RXRPC_ACCEPT 来分配。)

 (#) RXRPC_ACCEPT

     This is used by a server application to attempt to accept a call and
     assign it a user ID.  It should be associated with an RXRPC_USER_CALL_ID
     to indicate the user ID to be assigned.  If there is no call to be
     accepted (it may have timed out, been aborted, etc.), then sendmsg will
     return error ENODATA.  If the user ID is already in use by another call,
     then error EBADSLT will be returned.
     (服务器应用程序用它来尝试接受一个调用并为其分配用户 ID。它应当与一个
     RXRPC_USER_CALL_ID 相关联，以指明要分配的用户 ID。如果没有可接受的调用 (可能已
     超时、被中止等)，sendmsg 将返回错误 ENODATA。如果该用户 ID 已被另一个调用使用，
     则将返回错误 EBADSLT。)

 (#) RXRPC_EXCLUSIVE_CALL

     This is used to indicate that a client call should be made on a one-off
     connection.  The connection is discarded once the call has terminated.
     (用于表示客户端调用应当在一条一次性的连接上进行。调用终止后该连接即被丢弃。)

 (#) RXRPC_UPGRADE_SERVICE

     This is used to make a client call to probe if the specified service ID
     may be upgraded by the server.  The caller must check msg_name returned to
     recvmsg() for the service ID actually in use.  The operation probed must
     be one that takes the same arguments in both services.
     (用于发起一个客户端调用，以探测指定的服务 ID 是否可被服务器升级。调用者必须检查
     返回给 recvmsg() 的 msg_name 中实际使用的服务 ID。被探测的操作在两个服务中必须
     接受相同的参数。)

     Once this has been used to establish the upgrade capability (or lack
     thereof) of the server, the service ID returned should be used for all
     future communication to that server and RXRPC_UPGRADE_SERVICE should no
     longer be set.
     (一旦用它确定了服务器的升级能力 (或没有该能力)，之后与该服务器的所有通信都应使
     用返回的服务 ID，并且不应再设置 RXRPC_UPGRADE_SERVICE。)

 (#) RXRPC_TX_LENGTH

     This is used to inform the kernel of the total amount of data that is
     going to be transmitted by a call (whether in a client request or a
     service response).  If given, it allows the kernel to encrypt from the
     userspace buffer directly to the packet buffers, rather than copying into
     the buffer and then encrypting in place.  This may only be given with the
     first sendmsg() providing data for a call.  EMSGSIZE will be generated if
     the amount of data actually given is different.
     (用于告知内核某个调用将要发送的数据总量 (无论是客户端请求还是服务响应)。如果提
     供了它，内核就可以直接从用户空间缓冲区加密到数据包缓冲区，而不必先复制到缓冲区再
     原地加密。它只能随为调用提供数据的第一个 sendmsg() 一起给出。如果实际给出的数据
     量与此不同，将产生 EMSGSIZE。)

     This takes a parameter of __s64 type that indicates how much will be
     transmitted.  This may not be less than zero.
     (它接受一个 __s64 类型的参数，指明将要发送的数据量。该值不能小于零。)

The symbol RXRPC__SUPPORTED is defined as one more than the highest control
message type supported.  At run time this can be queried by means of the
RXRPC_SUPPORTED_CMSG socket option (see below).
(符号 RXRPC__SUPPORTED 被定义为所支持的最高控制消息类型加一。在运行时可以通过
RXRPC_SUPPORTED_CMSG 套接字选项查询它 (见下文)。)


Socket Options(套接字选项)
==========================

AF_RXRPC sockets support a few socket options at the SOL_RXRPC level:
(AF_RXRPC 套接字在 SOL_RXRPC 层级支持一些套接字选项：)

 (#) RXRPC_SECURITY_KEY

     This is used to specify the description of the key to be used.  The key is
     extracted from the calling process's keyrings with request_key() and
     should be of "rxrpc" type.
     (用于指定要使用的密钥的描述。密钥通过 request_key() 从调用进程的密钥环中提取，
     其类型应为 "rxrpc"。)

     The optval pointer points to the description string, and optlen indicates
     how long the string is, without the NUL terminator.
     (optval 指针指向描述字符串，optlen 指明字符串的长度，不包括 NUL 终止符。)

 (#) RXRPC_SECURITY_KEYRING

     Similar to above but specifies a keyring of server secret keys to use (key
     type "keyring").  See the "Security" section.
     (与上面类似，但它指定的是要使用的服务器秘密密钥的密钥环 (密钥类型
     "keyring")。参见 "Security" 一节。)

 (#) RXRPC_EXCLUSIVE_CONNECTION

     This is used to request that new connections should be used for each call
     made subsequently on this socket.  optval should be NULL and optlen 0.
     (用于请求在此套接字上随后发起的每个调用都使用新连接。optval 应为 NULL，optlen
     为 0。)

 (#) RXRPC_MIN_SECURITY_LEVEL

     This is used to specify the minimum security level required for calls on
     this socket.  optval must point to an int containing one of the following
     values:
     (用于指定此套接字上的调用所需的最低安全级别。optval 必须指向一个 int，其中包含
     以下值之一：)

     (a) RXRPC_SECURITY_PLAIN

	 Encrypted checksum only.
	 (仅加密校验和。)

     (b) RXRPC_SECURITY_AUTH

	 Encrypted checksum plus packet padded and first eight bytes of packet
	 encrypted - which includes the actual packet length.
	 (加密校验和，加上数据包填充并加密数据包的前八个字节——其中包含实际的包长。)

     (c) RXRPC_SECURITY_ENCRYPT

	 Encrypted checksum plus entire packet padded and encrypted, including
	 actual packet length.
	 (加密校验和，加上整个数据包填充并加密，包括实际包长。)

 (#) RXRPC_UPGRADEABLE_SERVICE

     This is used to indicate that a service socket with two bindings may
     upgrade one bound service to the other if requested by the client.  optval
     must point to an array of two unsigned short ints.  The first is the
     service ID to upgrade from and the second the service ID to upgrade to.
     (用于表示一个带有两个绑定的服务套接字，可以在客户端请求时把一个已绑定的服务升级
     为另一个。optval 必须指向一个包含两个 unsigned short int 的数组。第一个是要从中
     升级的服务 ID，第二个是要升级到的服务 ID。)

 (#) RXRPC_SUPPORTED_CMSG

     This is a read-only option that writes an int into the buffer indicating
     the highest control message type supported.
     (这是一个只读选项，它向缓冲区写入一个 int，指示所支持的最高控制消息类型。)


Security(安全性)
================

Currently, only the kerberos 4 equivalent protocol has been implemented
(security index 2 - rxkad).  This requires the rxkad module to be loaded and,
on the client, tickets of the appropriate type to be obtained from the AFS
kaserver or the kerberos server and installed as "rxrpc" type keys.  This is
normally done using the klog program.  An example simple klog program can be
found at:
(目前只实现了 kerberos 4 的等价协议 (安全索引 2 - rxkad)。这要求加载 rxkad 模块，并
且在客户端一侧，要从 AFS kaserver 或 kerberos 服务器获取适当类型的票据，并安装为
"rxrpc" 类型的密钥。这通常使用 klog 程序完成。一个简单的 klog 程序示例可以在以下地
址找到：)

	http://people.redhat.com/~dhowells/rxrpc/klog.c

The payload provided to add_key() on the client should be of the following
form::
(客户端提供给 add_key() 的载荷应具有如下形式：)

	struct rxrpc_key_sec2_v1 {
		uint16_t	security_index;	/* 2 */
		uint16_t	ticket_length;	/* length of ticket[] */
		uint32_t	expiry;		/* time at which expires */
		uint8_t		kvno;		/* key version number */
		uint8_t		__pad[3];
		uint8_t		session_key[8];	/* DES session key */
		uint8_t		ticket[0];	/* the encrypted ticket */
	};

Where the ticket blob is just appended to the above structure.
(其中票据数据块 (ticket blob) 直接附加在上述结构之后。)


For the server, keys of type "rxrpc_s" must be made available to the server.
They have a description of "<serviceID>:<securityIndex>" (eg: "52:2" for an
rxkad key for the AFS VL service).  When such a key is created, it should be
given the server's secret key as the instantiation data (see the example
below).
(对服务器而言，必须向服务器提供 "rxrpc_s" 类型的密钥。它们的描述为
"<serviceID>:<securityIndex>" (例如，AFS VL 服务的 rxkad 密钥为 "52:2")。创建此类
密钥时，应将服务器的秘密密钥作为实例化数据提供给它 (参见下面的示例)。)

	add_key("rxrpc_s", "52:2", secret_key, 8, keyring);

A keyring is passed to the server socket by naming it in a sockopt.  The server
socket then looks the server secret keys up in this keyring when secure
incoming connections are made.  This can be seen in an example program that can
be found at:
(密钥环通过在一个 sockopt 中指明其名称而传递给服务器套接字。当有安全的传入连接建立
时，服务器套接字随后会在此密钥环中查找服务器的秘密密钥。在一个示例程序中可以看到这一
点，该程序可在以下地址找到：)

	http://people.redhat.com/~dhowells/rxrpc/listen.c


Example Client Usage(客户端使用示例)
====================================

A client would issue an operation by:
(客户端按如下方式发起一个操作：)

 (1) An RxRPC socket is set up by::
     (按如下方式建立一个 RxRPC 套接字：)

	client = socket(AF_RXRPC, SOCK_DGRAM, PF_INET);

     Where the third parameter indicates the protocol family of the transport
     socket used - usually IPv4 but it can also be IPv6 [TODO].
     (其中第三个参数指明所使用的传输套接字的协议族——通常是 IPv4，但也可以是 IPv6
     [TODO]。)

 (2) A local address can optionally be bound::
     (可以选择性地绑定一个本地地址：)

	struct sockaddr_rxrpc srx = {
		.srx_family	= AF_RXRPC,
		.srx_service	= 0,  /* we're a client */
		.transport_type	= SOCK_DGRAM,	/* type of transport socket */
		.transport.sin_family	= AF_INET,
		.transport.sin_port	= htons(7000), /* AFS callback */
		.transport.sin_address	= 0,  /* all local interfaces */
	};
	bind(client, &srx, sizeof(srx));

     This specifies the local UDP port to be used.  If not given, a random
     non-privileged port will be used.  A UDP port may be shared between
     several unrelated RxRPC sockets.  Security is handled on a basis of
     per-RxRPC virtual connection.
     (这里指定要使用的本地 UDP 端口。如果不指定，将使用一个随机的非特权端口。一个
     UDP 端口可以在多个互不相关的 RxRPC 套接字之间共享。安全性按 RxRPC 虚拟连接逐条
     处理。)

 (3) The security is set::
     (设置安全性：)

	const char *key = "AFS:cambridge.redhat.com";
	setsockopt(client, SOL_RXRPC, RXRPC_SECURITY_KEY, key, strlen(key));

     This issues a request_key() to get the key representing the security
     context.  The minimum security level can be set::
     (这会发起一次 request_key() 来获取代表安全上下文的密钥。最低安全级别可以按如下
     方式设置：)

	unsigned int sec = RXRPC_SECURITY_ENCRYPT;
	setsockopt(client, SOL_RXRPC, RXRPC_MIN_SECURITY_LEVEL,
		   &sec, sizeof(sec));

 (4) The server to be contacted can then be specified (alternatively this can
     be done through sendmsg)::
     (然后可以指定要联系的服务器 (也可以改用 sendmsg 完成)：)

	struct sockaddr_rxrpc srx = {
		.srx_family	= AF_RXRPC,
		.srx_service	= VL_SERVICE_ID,
		.transport_type	= SOCK_DGRAM,	/* type of transport socket */
		.transport.sin_family	= AF_INET,
		.transport.sin_port	= htons(7005), /* AFS volume manager */
		.transport.sin_address	= ...,
	};
	connect(client, &srx, sizeof(srx));

 (5) The request data should then be posted to the server socket using a series
     of sendmsg() calls, each with the following control message attached:
     (随后应通过一系列 sendmsg() 调用把请求数据投递到服务器套接字，每次调用都附带以
     下控制消息：)

	==================	===================================
	RXRPC_USER_CALL_ID	specifies the user ID for this call (指定此调用的用户 ID)
	==================	===================================

     MSG_MORE should be set in msghdr::msg_flags on all but the last part of
     the request.  Multiple requests may be made simultaneously.
     (除请求的最后一部分外，其余部分都应在 msghdr::msg_flags 中设置 MSG_MORE。可以
     同时发起多个请求。)

     An RXRPC_TX_LENGTH control message can also be specified on the first
     sendmsg() call.
     (在第一次 sendmsg() 调用上还可以指定 RXRPC_TX_LENGTH 控制消息。)

     If a call is intended to go to a destination other than the default
     specified through connect(), then msghdr::msg_name should be set on the
     first request message of that call.
     (如果调用打算发往 connect() 所指定默认目的地之外的目的地，则应在该调用的第一条请
     求消息上设置 msghdr::msg_name。)

 (6) The reply data will then be posted to the server socket for recvmsg() to
     pick up.  MSG_MORE will be flagged by recvmsg() if there's more reply data
     for a particular call to be read.  MSG_EOR will be set on the terminal
     read for a call.
     (回复数据随后会被投递到服务器套接字，供 recvmsg() 读取。如果某个特定调用还有更多
     回复数据可读，recvmsg() 会设置 MSG_MORE 标志。在一次调用的最后一次读取上会设置
     MSG_EOR。)

     All data will be delivered with the following control message attached:
     (所有数据都会附带以下控制消息一起传递：)

	RXRPC_USER_CALL_ID	- specifies the user ID for this call (指定此调用的用户 ID)

     If an abort or error occurred, this will be returned in the control data
     buffer instead, and MSG_EOR will be flagged to indicate the end of that
     call.
     (如果发生了中止或错误，则会改为在控制数据缓冲区中返回它，并设置 MSG_EOR 标志以指
     示该调用结束。)

A client may ask for a service ID it knows and ask that this be upgraded to a
better service if one is available by supplying RXRPC_UPGRADE_SERVICE on the
first sendmsg() of a call.  The client should then check srx_service in the
msg_name filled in by recvmsg() when collecting the result.  srx_service will
hold the same value as given to sendmsg() if the upgrade request was ignored by
the service - otherwise it will be altered to indicate the service ID the
server upgraded to.  Note that the upgraded service ID is chosen by the server.
The caller has to wait until it sees the service ID in the reply before sending
any more calls (further calls to the same destination will be blocked until the
probe is concluded).
(客户端可以请求把它所知道的某个服务 ID 升级为更好的服务 (如果有可用的话)，做法是在
调用的第一次 sendmsg() 中提供 RXRPC_UPGRADE_SERVICE。之后，客户端在收集结果时应检查
由 recvmsg() 填入 msg_name 的 srx_service。如果升级请求被服务忽略，srx_service 将保
持与传给 sendmsg() 的值相同——否则它会被改变，以指示服务器升级到的服务 ID。注意，升
级后的服务 ID 由服务器选择。调用者必须等到在回复中看到服务 ID 之后才能发送更多调用
(在探测结束之前，发往同一目的地的后续调用会被阻塞)。)


Example Server Usage(服务器使用示例)
====================================

A server would be set up to accept operations in the following manner:
(服务器按如下方式进行设置以接受操作：)

 (1) An RxRPC socket is created by::
     (按如下方式创建一个 RxRPC 套接字：)

	server = socket(AF_RXRPC, SOCK_DGRAM, PF_INET);

     Where the third parameter indicates the address type of the transport
     socket used - usually IPv4.
     (其中第三个参数指明所使用的传输套接字的地址类型——通常是 IPv4。)

 (2) Security is set up if desired by giving the socket a keyring with server
     secret keys in it::
     (如需要，可通过给套接字一个装有服务器秘密密钥的密钥环来设置安全性：)

	keyring = add_key("keyring", "AFSkeys", NULL, 0,
			  KEY_SPEC_PROCESS_KEYRING);

	const char secret_key[8] = {
		0xa7, 0x83, 0x8a, 0xcb, 0xc7, 0x83, 0xec, 0x94 };
	add_key("rxrpc_s", "52:2", secret_key, 8, keyring);

	setsockopt(server, SOL_RXRPC, RXRPC_SECURITY_KEYRING, "AFSkeys", 7);

     The keyring can be manipulated after it has been given to the socket. This
     permits the server to add more keys, replace keys, etc. while it is live.
     (密钥环在交给套接字之后仍可以操作。这允许服务器在运行期间添加更多密钥、替换密钥
     等。)

 (3) A local address must then be bound::
     (随后必须绑定一个本地地址：)

	struct sockaddr_rxrpc srx = {
		.srx_family	= AF_RXRPC,
		.srx_service	= VL_SERVICE_ID, /* RxRPC service ID */
		.transport_type	= SOCK_DGRAM,	/* type of transport socket */
		.transport.sin_family	= AF_INET,
		.transport.sin_port	= htons(7000), /* AFS callback */
		.transport.sin_address	= 0,  /* all local interfaces */
	};
	bind(server, &srx, sizeof(srx));

     More than one service ID may be bound to a socket, provided the transport
     parameters are the same.  The limit is currently two.  To do this, bind()
     should be called twice.
     (在传输参数相同的前提下，一个套接字可以绑定多个服务 ID。目前的上限是两个。为
     此，应调用 bind() 两次。)

 (4) If service upgrading is required, first two service IDs must have been
     bound and then the following option must be set::
     (如果需要服务升级，必须先绑定两个服务 ID，然后设置以下选项：)

	unsigned short service_ids[2] = { from_ID, to_ID };
	setsockopt(server, SOL_RXRPC, RXRPC_UPGRADEABLE_SERVICE,
		   service_ids, sizeof(service_ids));

     This will automatically upgrade connections on service from_ID to service
     to_ID if they request it.  This will be reflected in msg_name obtained
     through recvmsg() when the request data is delivered to userspace.
     (这会在连接请求时自动把 from_ID 服务上的连接升级为 to_ID 服务。当请求数据传递到
     用户空间时，这会反映在通过 recvmsg() 获取的 msg_name 中。)

 (5) The server is then set to listen out for incoming calls::
     (然后将服务器设置为监听传入的调用：)

	listen(server, 100);

 (6) The kernel notifies the server of pending incoming connections by sending
     it a message for each.  This is received with recvmsg() on the server
     socket.  It has no data, and has a single dataless control message
     attached::
     (内核通过为每个待处理的传入连接发送一条消息来通知服务器。该消息在服务器套接字上
     用 recvmsg() 接收。它没有数据，只附带一个无数据的控制消息：)

	RXRPC_NEW_CALL

     The address that can be passed back by recvmsg() at this point should be
     ignored since the call for which the message was posted may have gone by
     the time it is accepted - in which case the first call still on the queue
     will be accepted.
     (此时 recvmsg() 传回的地址应当被忽略，因为发布该消息所针对的调用在接受时可能已经
     消失了——在这种情况下，被接受的是队列中仍在的第一个调用。)

 (7) The server then accepts the new call by issuing a sendmsg() with two
     pieces of control data and no actual data:
     (然后，服务器通过发出一个带有两段控制数据、无实际数据的 sendmsg() 来接受新调
     用：)

	==================	==============================
	RXRPC_ACCEPT		indicate connection acceptance (指示接受连接)
	RXRPC_USER_CALL_ID	specify user ID for this call (指定此调用的用户 ID)
	==================	==============================

 (8) The first request data packet will then be posted to the server socket for
     recvmsg() to pick up.  At that point, the RxRPC address for the call can
     be read from the address fields in the msghdr struct.
     (之后，第一个请求数据包会被投递到服务器套接字，供 recvmsg() 读取。此时，可以从
     msghdr 结构的地址字段中读取该调用的 RxRPC 地址。)

     Subsequent request data will be posted to the server socket for recvmsg()
     to collect as it arrives.  All but the last piece of the request data will
     be delivered with MSG_MORE flagged.
     (后续的请求数据会在到达时投递到服务器套接字，由 recvmsg() 收取。除请求数据的最后
     一段外，其余各段都会在设置了 MSG_MORE 标志的情况下传递。)

     All data will be delivered with the following control message attached:
     (所有数据都会附带以下控制消息一起传递：)


	==================	===================================
	RXRPC_USER_CALL_ID	specifies the user ID for this call (指定此调用的用户 ID)
	==================	===================================

 (9) The reply data should then be posted to the server socket using a series
     of sendmsg() calls, each with the following control messages attached:
     (随后应通过一系列 sendmsg() 调用把回复数据投递到服务器套接字，每次调用都附带以
     下控制消息：)

	==================	===================================
	RXRPC_USER_CALL_ID	specifies the user ID for this call (指定此调用的用户 ID)
	==================	===================================

     MSG_MORE should be set in msghdr::msg_flags on all but the last message
     for a particular call.
     (对某个特定调用，除最后一条消息外，其余消息都应在 msghdr::msg_flags 中设置
     MSG_MORE。)

(10) The final ACK from the client will be posted for retrieval by recvmsg()
     when it is received.  It will take the form of a dataless message with two
     control messages attached:
     (客户端的最终 ACK 在收到后会被投递出来，供 recvmsg() 获取。它表现为一条无数据的
     消息，附带两段控制消息：)

	==================	===================================
	RXRPC_USER_CALL_ID	specifies the user ID for this call (指定此调用的用户 ID)
	RXRPC_ACK		indicates final ACK (no data) (指示最终 ACK (无数据))
	==================	===================================

     MSG_EOR will be flagged to indicate that this is the final message for
     this call.
     (会设置 MSG_EOR 标志，以表明这是该调用的最后一条消息。)

(11) Up to the point the final packet of reply data is sent, the call can be
     aborted by calling sendmsg() with a dataless message with the following
     control messages attached:
     (在回复数据的最后一个数据包发出之前的任何时候，都可以通过用一个附带以下控制消息
     的无数据消息调用 sendmsg() 来中止该调用：)

	==================	===================================
	RXRPC_USER_CALL_ID	specifies the user ID for this call (指定此调用的用户 ID)
	RXRPC_ABORT		indicates abort code (4 byte data) (指示中止码 (4 字节数据))
	==================	===================================

     Any packets waiting in the socket's receive queue will be discarded if
     this is issued.
     (如果发出此消息，套接字接收队列中等待的所有数据包都将被丢弃。)

Note that all the communications for a particular service take place through
the one server socket, using control messages on sendmsg() and recvmsg() to
determine the call affected.
(注意，与某个特定服务的所有通信都通过这一个服务器套接字进行，利用 sendmsg() 和
recvmsg() 上的控制消息来确定受影响的调用。)


AF_RXRPC Kernel Interface(AF_RXRPC 内核接口)
============================================

The AF_RXRPC module also provides an interface for use by in-kernel utilities
such as the AFS filesystem.  This permits such a utility to:
(AF_RXRPC 模块还提供了一个供内核内工具 (例如 AFS 文件系统) 使用的接口。它允许这样的工
具：)

 (1) Use different keys directly on individual client calls on one socket
     rather than having to open a whole slew of sockets, one for each key it
     might want to use.
     (在同一个套接字上直接对各个客户端调用使用不同的密钥，而不必为它可能想用的每个密
     钥都打开一大堆套接字。)

 (2) Avoid having RxRPC call request_key() at the point of issue of a call or
     opening of a socket.  Instead the utility is responsible for requesting a
     key at the appropriate point.  AFS, for instance, would do this during VFS
     operations such as open() or unlink().  The key is then handed through
     when the call is initiated.
     (避免让 RxRPC 在发起调用或打开套接字时调用 request_key()。改为由该工具负责在适
     当的时机请求密钥。例如，AFS 会在诸如 open() 或 unlink() 之类的 VFS 操作期间这样
     做。随后在调用发起时把密钥传递进来。)

 (3) Request the use of something other than GFP_KERNEL to allocate memory.
     (请求使用 GFP_KERNEL 以外的内存分配标志。)

 (4) Avoid the overhead of using the recvmsg() call.  RxRPC messages can be
     intercepted before they get put into the socket Rx queue and the socket
     buffers manipulated directly.
     (避免使用 recvmsg() 调用的开销。RxRPC 消息可以在进入套接字 Rx 队列之前被截获，
     并直接操作套接字缓冲区。)

To use the RxRPC facility, a kernel utility must still open an AF_RXRPC socket,
bind an address as appropriate and listen if it's to be a server socket, but
then it passes this to the kernel interface functions.
(要使用 RxRPC 设施，内核工具仍然必须打开一个 AF_RXRPC 套接字、按需绑定地址，如果要
作为服务器套接字还要监听，然后把套接字传给内核接口函数。)

The kernel interface functions are as follows:
(内核接口函数如下：)

 (#) Begin a new client call::
     (发起新的客户端调用：)

	struct rxrpc_call *
	rxrpc_kernel_begin_call(struct socket *sock,
				struct sockaddr_rxrpc *srx,
				struct key *key,
				unsigned long user_call_ID,
				s64 tx_total_len,
				gfp_t gfp,
				rxrpc_notify_rx_t notify_rx,
				bool upgrade,
				bool intr,
				unsigned int debug_id);

     This allocates the infrastructure to make a new RxRPC call and assigns
     call and connection numbers.  The call will be made on the UDP port that
     the socket is bound to.  The call will go to the destination address of a
     connected client socket unless an alternative is supplied (srx is
     non-NULL).
     (它为发起新的 RxRPC 调用分配基础设施，并分配调用号和连接号。调用将在该套接字绑
     定的 UDP 端口上进行。除非提供了替代地址 (srx 非 NULL)，调用将发往已连接客户端套
     接字的目的地址。)

     If a key is supplied then this will be used to secure the call instead of
     the key bound to the socket with the RXRPC_SECURITY_KEY sockopt.  Calls
     secured in this way will still share connections if at all possible.
     (如果提供了密钥，则将使用它来保护调用，而不是通过 RXRPC_SECURITY_KEY 套接字选
     项绑定到套接字上的密钥。以这种方式保护的调用在可能的情况下仍会共享连接。)

     The user_call_ID is equivalent to that supplied to sendmsg() in the
     control data buffer.  It is entirely feasible to use this to point to a
     kernel data structure.
     (user_call_ID 等价于在控制数据缓冲区中提供给 sendmsg() 的那个。用它指向一个内核
     数据结构是完全可行的。)

     tx_total_len is the amount of data the caller is intending to transmit
     with this call (or -1 if unknown at this point).  Setting the data size
     allows the kernel to encrypt directly to the packet buffers, thereby
     saving a copy.  The value may not be less than -1.
     (tx_total_len 是调用者打算通过本次调用发送的数据量 (如果此时未知则为 -1)。设置
     数据大小可以让内核直接加密到数据包缓冲区，从而省去一次拷贝。该值不能小于 -1。)

     notify_rx is a pointer to a function to be called when events such as
     incoming data packets or remote aborts happen.
     (notify_rx 是一个函数指针，在发生诸如收到数据包或远端中止之类的事件时被调用。)

     upgrade should be set to true if a client operation should request that
     the server upgrade the service to a better one.  The resultant service ID
     is returned by rxrpc_kernel_recv_data().
     (如果客户端操作应请求服务器把服务升级为更好的一个，则应将 upgrade 设为 true。
     最终的服务 ID 由 rxrpc_kernel_recv_data() 返回。)

     intr should be set to true if the call should be interruptible.  If this
     is not set, this function may not return until a channel has been
     allocated; if it is set, the function may return -ERESTARTSYS.
     (如果调用应当可中断，则应将 intr 设为 true。如果不设置，该函数在分配到信道之前
     可能不会返回；如果设置了，该函数可能返回 -ERESTARTSYS。)

     debug_id is the call debugging ID to be used for tracing.  This can be
     obtained by atomically incrementing rxrpc_debug_id.
     (debug_id 是用于跟踪的调用调试 ID。可以通过对 rxrpc_debug_id 进行原子递增来获
     得。)

     If this function is successful, an opaque reference to the RxRPC call is
     returned.  The caller now holds a reference on this and it must be
     properly ended.
     (如果此函数成功，将返回对 RxRPC 调用的一个不透明引用。调用者此时持有对它的引
     用，并且必须将其正确地结束。)

 (#) Shut down a client call::
     (关闭一个客户端调用：)

	void rxrpc_kernel_shutdown_call(struct socket *sock,
					struct rxrpc_call *call);

     This is used to shut down a previously begun call.  The user_call_ID is
     expunged from AF_RXRPC's knowledge and will not be seen again in
     association with the specified call.
     (它用于关闭一个先前已发起的调用。user_call_ID 会从 AF_RXRPC 的记录中清除，此后
     不会再与指定的调用相关联地出现。)

 (#) Release the ref on a client call::
     (释放对客户端调用的引用：)

	void rxrpc_kernel_put_call(struct socket *sock,
				   struct rxrpc_call *call);

     This is used to release the caller's ref on an rxrpc call.
     (它用于释放调用者对一个 rxrpc 调用的引用。)

 (#) Send data through a call::
     (通过调用发送数据：)

	typedef void (*rxrpc_notify_end_tx_t)(struct sock *sk,
					      unsigned long user_call_ID,
					      struct sk_buff *skb);

	int rxrpc_kernel_send_data(struct socket *sock,
				   struct rxrpc_call *call,
				   struct msghdr *msg,
				   size_t len,
				   rxrpc_notify_end_tx_t notify_end_rx);

     This is used to supply either the request part of a client call or the
     reply part of a server call.  msg.msg_iovlen and msg.msg_iov specify the
     data buffers to be used.  msg_iov may not be NULL and must point
     exclusively to in-kernel virtual addresses.  msg.msg_flags may be given
     MSG_MORE if there will be subsequent data sends for this call.
     (它用于提供客户端调用的请求部分或服务器调用的回复部分。msg.msg_iovlen 和
     msg.msg_iov 指定要使用的数据缓冲区。msg_iov 不能为 NULL，并且必须只指向内核内虚
     拟地址。如果此调用后续还有数据要发送，可以在 msg.msg_flags 中给出 MSG_MORE。)

     The msg must not specify a destination address, control data or any flags
     other than MSG_MORE.  len is the total amount of data to transmit.
     (msg 不得指定目的地址、控制数据或 MSG_MORE 之外的任何标志。len 是要发送的数据总
     量。)

     notify_end_rx can be NULL or it can be used to specify a function to be
     called when the call changes state to end the Tx phase.  This function is
     called with a spinlock held to prevent the last DATA packet from being
     transmitted until the function returns.
     (notify_end_rx 可以为 NULL，也可以用来指定一个函数，当调用改变状态以结束 Tx 阶
     段时调用它。该函数在持有自旋锁的情况下被调用，以防止最后一个 DATA 数据包在该函数
     返回之前被发送出去。)

 (#) Receive data from a call::
     (从调用接收数据：)

	int rxrpc_kernel_recv_data(struct socket *sock,
				   struct rxrpc_call *call,
				   void *buf,
				   size_t size,
				   size_t *_offset,
				   bool want_more,
				   u32 *_abort,
				   u16 *_service)

      This is used to receive data from either the reply part of a client call
      or the request part of a service call.  buf and size specify how much
      data is desired and where to store it.  *_offset is added on to buf and
      subtracted from size internally; the amount copied into the buffer is
      added to *_offset before returning.
      (它用于从客户端调用的回复部分或服务调用的请求部分接收数据。buf 和 size 指明需
      要多少数据以及存储到哪里。内部会把 *_offset 加到 buf 上并从 size 中减去；返回之
      前，复制到缓冲区中的数量会加到 *_offset 上。)

      want_more should be true if further data will be required after this is
      satisfied and false if this is the last item of the receive phase.
      (如果在此满足之后还需要更多数据，want_more 应为 true；如果这是接收阶段的最后一
      项，则为 false。)

      There are three normal returns: 0 if the buffer was filled and want_more
      was true; 1 if the buffer was filled, the last DATA packet has been
      emptied and want_more was false; and -EAGAIN if the function needs to be
      called again.
      (有三种正常返回值：如果缓冲区已填满且 want_more 为 true，返回 0；如果缓冲区已
      填满、最后一个 DATA 数据包已取空且 want_more 为 false，返回 1；如果需要再次调
      用该函数，返回 -EAGAIN。)

      If the last DATA packet is processed but the buffer contains less than
      the amount requested, EBADMSG is returned.  If want_more wasn't set, but
      more data was available, EMSGSIZE is returned.
      (如果最后一个 DATA 数据包已处理完毕，但缓冲区中的数据少于请求的数量，则返回
      EBADMSG。如果未设置 want_more 但还有更多数据可用，则返回 EMSGSIZE。)

      If a remote ABORT is detected, the abort code received will be stored in
      ``*_abort`` and ECONNABORTED will be returned.
      (如果检测到远端 ABORT，收到的中止码将存储在 ``*_abort`` 中，并返回
      ECONNABORTED。)

      The service ID that the call ended up with is returned into *_service.
      This can be used to see if a call got a service upgrade.
      (该调用最终使用的服务 ID 会返回到 *_service 中。这可以用来查看调用是否获得了服
      务升级。)

 (#) Abort a call??
     (中止一个调用？？)

     ::

	void rxrpc_kernel_abort_call(struct socket *sock,
				     struct rxrpc_call *call,
				     u32 abort_code);

     This is used to abort a call if it's still in an abortable state.  The
     abort code specified will be placed in the ABORT message sent.
     (它用于在调用仍处于可中止状态时中止该调用。指定的中止码将被放入所发送的 ABORT
     消息中。)

 (#) Intercept received RxRPC messages::
     (截获接收到的 RxRPC 消息：)

	typedef void (*rxrpc_interceptor_t)(struct sock *sk,
					    unsigned long user_call_ID,
					    struct sk_buff *skb);

	void
	rxrpc_kernel_intercept_rx_messages(struct socket *sock,
					   rxrpc_interceptor_t interceptor);

     This installs an interceptor function on the specified AF_RXRPC socket.
     All messages that would otherwise wind up in the socket's Rx queue are
     then diverted to this function.  Note that care must be taken to process
     the messages in the right order to maintain DATA message sequentiality.
     (它在指定的 AF_RXRPC 套接字上安装一个截获 (interceptor) 函数。原本会进入套接字
     Rx 队列的所有消息此后都会转送到该函数。注意，必须小心按正确的顺序处理消息，以维持
     DATA 消息的顺序性。)

     The interceptor function itself is provided with the address of the socket
     and handling the incoming message, the ID assigned by the kernel utility
     to the call and the socket buffer containing the message.
     (截获函数本身会获得该套接字与所处理传入消息的地址、内核工具分配给该调用的 ID，以
     及包含该消息的套接字缓冲区。)

     The skb->mark field indicates the type of message:
     (skb->mark 字段指示消息的类型：)

	===============================	=======================================
	Mark				Meaning (含义)
	===============================	=======================================
	RXRPC_SKB_MARK_DATA		Data message (数据消息)
	RXRPC_SKB_MARK_FINAL_ACK	Final ACK received for an incoming call (为传入调用收到最终 ACK)
	RXRPC_SKB_MARK_BUSY		Client call rejected as server busy (客户端调用因服务器忙被拒绝)
	RXRPC_SKB_MARK_REMOTE_ABORT	Call aborted by peer (调用被对端中止)
	RXRPC_SKB_MARK_NET_ERROR	Network error detected (检测到网络错误)
	RXRPC_SKB_MARK_LOCAL_ERROR	Local error encountered (遇到本地错误)
	RXRPC_SKB_MARK_NEW_CALL		New incoming call awaiting acceptance (等待接受的新传入调用)
	===============================	=======================================

     The remote abort message can be probed with rxrpc_kernel_get_abort_code().
     The two error messages can be probed with rxrpc_kernel_get_error_number().
     A new call can be accepted with rxrpc_kernel_accept_call().
     (远端中止消息可以用 rxrpc_kernel_get_abort_code() 探测。两种错误消息可以用
     rxrpc_kernel_get_error_number() 探测。新调用可以用 rxrpc_kernel_accept_call()
     接受。)

     Data messages can have their contents extracted with the usual bunch of
     socket buffer manipulation functions.  A data message can be determined to
     be the last one in a sequence with rxrpc_kernel_is_data_last().  When a
     data message has been used up, rxrpc_kernel_data_consumed() should be
     called on it.
     (数据消息的内容可以用常用的那组套接字缓冲区操作函数来提取。可以用
     rxrpc_kernel_is_data_last() 判断一条数据消息是否是序列中的最后一条。当一条数据消
     息用完之后，应对它调用 rxrpc_kernel_data_consumed()。)

     Messages should be handled to rxrpc_kernel_free_skb() to dispose of.  It
     is possible to get extra refs on all types of message for later freeing,
     but this may pin the state of a call until the message is finally freed.
     (消息应交给 rxrpc_kernel_free_skb() 处置。可以对所有类型的消息获取额外的引用以
     便稍后释放，但这可能会把调用的状态固定住，直到该消息最终被释放。)

 (#) Accept an incoming call::
     (接受一个传入调用：)

	struct rxrpc_call *
	rxrpc_kernel_accept_call(struct socket *sock,
				 unsigned long user_call_ID);

     This is used to accept an incoming call and to assign it a call ID.  This
     function is similar to rxrpc_kernel_begin_call() and calls accepted must
     be ended in the same way.
     (它用于接受一个传入调用并为其分配调用 ID。此函数与 rxrpc_kernel_begin_call()
     类似，被接受的调用必须以相同的方式结束。)

     If this function is successful, an opaque reference to the RxRPC call is
     returned.  The caller now holds a reference on this and it must be
     properly ended.
     (如果此函数成功，将返回对 RxRPC 调用的一个不透明引用。调用者此时持有对它的引
     用，并且必须将其正确地结束。)

 (#) Reject an incoming call::
     (拒绝一个传入调用：)

	int rxrpc_kernel_reject_call(struct socket *sock);

     This is used to reject the first incoming call on the socket's queue with
     a BUSY message.  -ENODATA is returned if there were no incoming calls.
     Other errors may be returned if the call had been aborted (-ECONNABORTED)
     or had timed out (-ETIME).
     (它用于用一条 BUSY 消息拒绝套接字队列上的第一个传入调用。如果没有传入调用，则返
     回 -ENODATA。如果调用已被中止 (-ECONNABORTED) 或已超时 (-ETIME)，则可能返回其他
     错误。)

 (#) Allocate a null key for doing anonymous security::
     (分配一个用于匿名安全的空密钥：)

	struct key *rxrpc_get_null_key(const char *keyname);

     This is used to allocate a null RxRPC key that can be used to indicate
     anonymous security for a particular domain.
     (它用于分配一个空 (null) RxRPC 密钥，该密钥可用于为特定域指示匿名安全。)

 (#) Get the peer address of a call::
     (获取调用的对端地址：)

	void rxrpc_kernel_get_peer(struct socket *sock, struct rxrpc_call *call,
				   struct sockaddr_rxrpc *_srx);

     This is used to find the remote peer address of a call.
     (它用于查找调用的远端对端地址。)

 (#) Set the total transmit data size on a call::
     (设置调用要发送的数据总量：)

	void rxrpc_kernel_set_tx_length(struct socket *sock,
					struct rxrpc_call *call,
					s64 tx_total_len);

     This sets the amount of data that the caller is intending to transmit on a
     call.  It's intended to be used for setting the reply size as the request
     size should be set when the call is begun.  tx_total_len may not be less
     than zero.
     (它设置调用者打算在某次调用上发送的数据量。它打算用于设置回复大小，因为请求大小
     应在调用开始时设置。tx_total_len 不能小于零。)

 (#) Get call RTT::
     (获取调用 RTT：)

	u64 rxrpc_kernel_get_rtt(struct socket *sock, struct rxrpc_call *call);

     Get the RTT time to the peer in use by a call.  The value returned is in
     nanoseconds.
     (获取某调用当前所用对端的 RTT 时间。返回值以纳秒为单位。)

 (#) Check call still alive::
     (检查调用是否仍然存活：)

	bool rxrpc_kernel_check_life(struct socket *sock,
				     struct rxrpc_call *call,
				     u32 *_life);
	void rxrpc_kernel_probe_life(struct socket *sock,
				     struct rxrpc_call *call);

     The first function passes back in ``*_life`` a number that is updated when
     ACKs are received from the peer (notably including PING RESPONSE ACKs
     which we can elicit by sending PING ACKs to see if the call still exists
     on the server).  The caller should compare the numbers of two calls to see
     if the call is still alive after waiting for a suitable interval.  It also
     returns true as long as the call hasn't yet reached the completed state.
     (第一个函数通过 ``*_life`` 传回一个数字，每当从对端收到 ACK 时该数字就会更新
     (其中尤其包括 PING RESPONSE ACK——我们可以通过发送 PING ACK 来探查该调用在服务
     器上是否仍然存在，从而引出这类 ACK)。调用者应等待一段合适的时间后比较两次调用的数
     字，以判断调用是否仍然存活。只要调用尚未达到完成状态，它也返回 true。)

     This allows the caller to work out if the server is still contactable and
     if the call is still alive on the server while waiting for the server to
     process a client operation.
     (这使调用者能够在等待服务器处理客户端操作的同时，判断服务器是否仍可联系、调用在服
     务器上是否仍然存活。)

     The second function causes a ping ACK to be transmitted to try to provoke
     the peer into responding, which would then cause the value returned by the
     first function to change.  Note that this must be called in TASK_RUNNING
     state.
     (第二个函数会发送一个 ping ACK，试图促使对端作出响应，从而让第一个函数返回的值发
     生变化。注意，必须在 TASK_RUNNING 状态下调用它。)

 (#) Apply the RXRPC_MIN_SECURITY_LEVEL sockopt to a socket from within in the
     kernel::
     (从内核内部对套接字应用 RXRPC_MIN_SECURITY_LEVEL 套接字选项：)

       int rxrpc_sock_set_min_security_level(struct sock *sk,
					     unsigned int val);

     This specifies the minimum security level required for calls on this
     socket.
     (它指定此套接字上的调用所需的最低安全级别。)


Configurable Parameters(可配置参数)
===================================

The RxRPC protocol driver has a number of configurable parameters that can be
adjusted through sysctls in /proc/net/rxrpc/:
(RxRPC 协议驱动程序有许多可配置参数，可以通过 /proc/net/rxrpc/ 中的 sysctl 进行调
整：)

 (#) req_ack_delay

     The amount of time in milliseconds after receiving a packet with the
     request-ack flag set before we honour the flag and actually send the
     requested ack.
     (在收到带有 request-ack 标志的数据包之后，经过多少毫秒我们才理会该标志并实际发
     送所请求的 ack。)

     Usually the other side won't stop sending packets until the advertised
     reception window is full (to a maximum of 255 packets), so delaying the
     ACK permits several packets to be ACK'd in one go.
     (通常，在对端宣告的接收窗口填满 (最多 255 个数据包) 之前，它不会停止发送数据
     包，因此延迟发送 ACK 可以一次性确认多个数据包。)

 (#) soft_ack_delay

     The amount of time in milliseconds after receiving a new packet before we
     generate a soft-ACK to tell the sender that it doesn't need to resend.
     (在收到一个新数据包之后，经过多少毫秒我们才生成一个软 ACK (soft-ACK)，告知发送
     方无需重发。)

 (#) idle_ack_delay

     The amount of time in milliseconds after all the packets currently in the
     received queue have been consumed before we generate a hard-ACK to tell
     the sender it can free its buffers, assuming no other reason occurs that
     we would send an ACK.
     (在接收队列中当前的所有数据包都被消费完之后，经过多少毫秒我们才生成一个硬 ACK
     (hard-ACK)，告知发送方可以释放其缓冲区——前提是没有发生其他需要我们发送 ACK 的
     事由。)

 (#) resend_timeout

     The amount of time in milliseconds after transmitting a packet before we
     transmit it again, assuming no ACK is received from the receiver telling
     us they got it.
     (在发送一个数据包之后，假设没有收到接收方告知已收到的 ACK，经过多少毫秒我们会再
     次发送它。)

 (#) max_call_lifetime

     The maximum amount of time in seconds that a call may be in progress
     before we preemptively kill it.
     (一个调用可以持续进行的最大秒数，超过之后我们会抢先将其杀死。)

 (#) dead_call_expiry

     The amount of time in seconds before we remove a dead call from the call
     list.  Dead calls are kept around for a little while for the purpose of
     repeating ACK and ABORT packets.
     (经过多少秒我们才从调用列表中移除一个死调用。死调用会保留一小段时间，用于重发
     ACK 和 ABORT 数据包。)

 (#) connection_expiry

     The amount of time in seconds after a connection was last used before we
     remove it from the connection list.  While a connection is in existence,
     it serves as a placeholder for negotiated security; when it is deleted,
     the security must be renegotiated.
     (一条连接在最后一次使用之后经过多少秒我们才把它从连接列表中移除。连接存在期间，
     它充当已协商安全性的占位符；删除之后，安全性必须重新协商。)

 (#) transport_expiry

     The amount of time in seconds after a transport was last used before we
     remove it from the transport list.  While a transport is in existence, it
     serves to anchor the peer data and keeps the connection ID counter.
     (一个传输端在最后一次使用之后经过多少秒我们才把它从传输列表中移除。传输端存在期
     间，它用于锚定对端数据并维护连接 ID 计数器。)

 (#) rxrpc_rx_window_size

     The size of the receive window in packets.  This is the maximum number of
     unconsumed received packets we're willing to hold in memory for any
     particular call.
     (接收窗口的大小，以数据包数计。这是我们对任何一个特定调用愿意在内存中保留的未消
     费已接收数据包的最大数量。)

 (#) rxrpc_rx_mtu

     The maximum packet MTU size that we're willing to receive in bytes.  This
     indicates to the peer whether we're willing to accept jumbo packets.
     (我们愿意接收的最大数据包 MTU 大小，以字节计。它向对端表明我们是否愿意接受巨型
     数据包。)

 (#) rxrpc_rx_jumbo_max

     The maximum number of packets that we're willing to accept in a jumbo
     packet.  Non-terminal packets in a jumbo packet must contain a four byte
     header plus exactly 1412 bytes of data.  The terminal packet must contain
     a four byte header plus any amount of data.  In any event, a jumbo packet
     may not exceed rxrpc_rx_mtu in size.
     (我们愿意在一个巨型数据包 (jumbo packet) 中接受的最大数据包数。巨型数据包中的非
     末尾数据包必须包含一个 4 字节头加上恰好 1412 字节的数据。末尾数据包必须包含一个
     4 字节头加上任意数量的数据。无论如何，巨型数据包的大小不得超过 rxrpc_rx_mtu。)


API Function Reference(API 函数参考)
====================================

.. kernel-doc:: net/rxrpc/af_rxrpc.c
.. kernel-doc:: net/rxrpc/call_object.c
.. kernel-doc:: net/rxrpc/key.c
.. kernel-doc:: net/rxrpc/oob.c
.. kernel-doc:: net/rxrpc/peer_object.c
.. kernel-doc:: net/rxrpc/recvmsg.c
.. kernel-doc:: net/rxrpc/rxgk.c
.. kernel-doc:: net/rxrpc/rxkad.c
.. kernel-doc:: net/rxrpc/sendmsg.c
.. kernel-doc:: net/rxrpc/server_key.c
