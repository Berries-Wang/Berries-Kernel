.. SPDX-License-Identifier: GPL-2.0

=============================================
Kernel Connection Multiplexor(内核连接复用器)
=============================================

Kernel Connection Multiplexor (KCM) is a mechanism that provides a message based
interface over TCP for generic application protocols. With KCM an application
can efficiently send and receive application protocol messages over TCP using
datagram sockets.
(内核连接复用器 (KCM) 是一种在 TCP 之上为通用应用协议提供基于消息接口的机制。借助 KCM，应用程序可以使用数据报套接字在 TCP 上高效地发送和接收应用协议消息。)

KCM implements an NxM multiplexor in the kernel as diagrammed below::

    +------------+   +------------+   +------------+   +------------+
    | KCM socket |   | KCM socket |   | KCM socket |   | KCM socket |
    +------------+   +------------+   +------------+   +------------+
	|                 |               |                |
	+-----------+     |               |     +----------+
		    |     |               |     |
		+----------------------------------+
		|           Multiplexor            |
		+----------------------------------+
		    |   |           |           |  |
	+---------+   |           |           |  ------------+
	|             |           |           |              |
    +----------+  +----------+  +----------+  +----------+ +----------+
    |  Psock   |  |  Psock   |  |  Psock   |  |  Psock   | |  Psock   |
    +----------+  +----------+  +----------+  +----------+ +----------+
	|              |           |            |             |
    +----------+  +----------+  +----------+  +----------+ +----------+
    | TCP sock |  | TCP sock |  | TCP sock |  | TCP sock | | TCP sock |
    +----------+  +----------+  +----------+  +----------+ +----------+

(KCM 在内核中实现了一个 NxM 复用器，如下图所示：)

KCM sockets(KCM 套接字)
=======================

The KCM sockets provide the user interface to the multiplexor. All the KCM sockets
bound to a multiplexor are considered to have equivalent function, and I/O
operations in different sockets may be done in parallel without the need for
synchronization between threads in userspace.
(KCM 套接字提供复用器的用户接口。绑定到同一复用器的所有 KCM 套接字被视为具有等同功能，不同套接字上的 I/O 操作可以并行进行，而无需用户态线程之间做同步。)

Multiplexor(复用器)
===================

The multiplexor provides the message steering. In the transmit path, messages
written on a KCM socket are sent atomically on an appropriate TCP socket.
Similarly, in the receive path, messages are constructed on each TCP socket
(Psock) and complete messages are steered to a KCM socket.
(复用器提供消息导向。在发送路径上，写入 KCM 套接字的消息会在合适的 TCP 套接字上原子地发送。类似地，在接收路径上，会在每个 TCP 套接字（Psock）上构造消息，完整的消息被导向到某个 KCM 套接字。)

TCP sockets & Psocks(TCP 套接字与 Psock)
========================================

TCP sockets may be bound to a KCM multiplexor. A Psock structure is allocated
for each bound TCP socket, this structure holds the state for constructing
messages on receive as well as other connection specific information for KCM.
(TCP 套接字可以绑定到 KCM 复用器。每个被绑定的 TCP 套接字都会分配一个 Psock 结构，该结构保存接收方向上构造消息的状态以及 KCM 的其他连接相关信息。)

Connected mode semantics(连接模式语义)
======================================

Each multiplexor assumes that all attached TCP connections are to the same
destination and can use the different connections for load balancing when
transmitting. The normal send and recv calls (include sendmmsg and recvmmsg)
can be used to send and receive messages from the KCM socket.
(每个复用器假定所有挂接的 TCP 连接都指向同一目的地，并且可以在发送时利用不同的连接做负载均衡。普通的 send 和 recv 调用（包括 sendmmsg 和 recvmmsg）都可用于在 KCM 套接字上发送和接收消息。)

Socket types(套接字类型)
========================

KCM supports SOCK_DGRAM and SOCK_SEQPACKET socket types.
(KCM 支持 SOCK_DGRAM 和 SOCK_SEQPACKET 套接字类型。)

Message delineation(消息定界)
-----------------------------

Messages are sent over a TCP stream with some application protocol message
format that typically includes a header which frames the messages. The length
of a received message can be deduced from the application protocol header
(often just a simple length field).
(消息以某种应用协议消息格式在 TCP 流上发送，该格式通常包含一个用来框定消息的头。接收到的消息长度可以从应用协议头推知（通常只是一个简单的长度字段）。)

A TCP stream must be parsed to determine message boundaries. Berkeley Packet
Filter (BPF) is used for this. When attaching a TCP socket to a multiplexor a
BPF program must be specified. The program is called at the start of receiving
a new message and is given an skbuff that contains the bytes received so far.
It parses the message header and returns the length of the message. Given this
information, KCM will construct the message of the stated length and deliver it
to a KCM socket.
(必须对 TCP 流进行解析才能确定消息边界。这使用伯克利包过滤器 (BPF) 来完成。将 TCP 套接字挂载到复用器时必须指定一个 BPF 程序。该程序在开始接收新消息时被调用，并得到一个包含到目前为止已接收字节的 skbuff。它解析消息头并返回消息的长度。得到这一信息后，KCM 会构造出所述长度的消息并将其交付给某个 KCM 套接字。)

TCP socket management(TCP 套接字管理)
-------------------------------------

When a TCP socket is attached to a KCM multiplexor data ready (POLLIN) and
write space available (POLLOUT) events are handled by the multiplexor. If there
is a state change (disconnection) or other error on a TCP socket, an error is
posted on the TCP socket so that a POLLERR event happens and KCM discontinues
using the socket. When the application gets the error notification for a
TCP socket, it should unattach the socket from KCM and then handle the error
condition (the typical response is to close the socket and create a new
connection if necessary).
(当 TCP 套接字挂接到 KCM 复用器后，数据就绪 (POLLIN) 和可写空间可用 (POLLOUT) 事件都由复用器处理。如果某个 TCP 套接字发生状态变化（断开）或其他错误，就会在该 TCP 套接字上报告一个错误，从而触发 POLLERR 事件，KCM 会停止使用该套接字。当应用程序收到某个 TCP 套接字的错误通知时，应先将该套接字从 KCM 上解除挂接，然后再处理错误状况（典型响应是关闭该套接字，必要时建立新连接）。)

KCM limits the maximum receive message size to be the size of the receive
socket buffer on the attached TCP socket (the socket buffer size can be set by
SO_RCVBUF). If the length of a new message reported by the BPF program is
greater than this limit a corresponding error (EMSGSIZE) is posted on the TCP
socket. The BPF program may also enforce a maximum messages size and report an
error when it is exceeded.
(KCM 将最大接收消息大小限制为所挂接 TCP 套接字的接收套接字缓冲区大小（缓冲区大小可通过 SO_RCVBUF 设置）。如果 BPF 程序报告的新消息长度超过此限制，就会在该 TCP 套接字上报告相应错误 (EMSGSIZE)。BPF 程序也可以自行强制一个最大消息大小，并在超出时报错。)

A timeout may be set for assembling messages on a receive socket. The timeout
value is taken from the receive timeout of the attached TCP socket (this is set
by SO_RCVTIMEO). If the timer expires before assembly is complete an error
(ETIMEDOUT) is posted on the socket.
(可以为接收套接字上的消息组装设置超时。超时值取自所挂接 TCP 套接字的接收超时（通过 SO_RCVTIMEO 设置）。如果在组装完成之前计时器到期，就会在该套接字上报告错误 (ETIMEDOUT)。)

User interface(用户接口)
========================

Creating a multiplexor(创建复用器)
----------------------------------

A new multiplexor and initial KCM socket is created by a socket call::
(通过一次 socket 调用即可创建新的复用器和初始 KCM 套接字：)

  socket(AF_KCM, type, protocol)

- type is either SOCK_DGRAM or SOCK_SEQPACKET
  (type 为 SOCK_DGRAM 或 SOCK_SEQPACKET)
- protocol is KCMPROTO_CONNECTED
  (protocol 为 KCMPROTO_CONNECTED)

Cloning KCM sockets(克隆 KCM 套接字)
------------------------------------

After the first KCM socket is created using the socket call as described
above, additional sockets for the multiplexor can be created by cloning
a KCM socket. This is accomplished by an ioctl on a KCM socket::
(按照上文所述用 socket 调用创建第一个 KCM 套接字之后，可以通过克隆一个 KCM 套接字来为该复用器创建更多套接字。这通过对 KCM 套接字执行一个 ioctl 来实现：)

  /* From linux/kcm.h */
  struct kcm_clone {
	int fd;
  };

  struct kcm_clone info;

  memset(&info, 0, sizeof(info));

  err = ioctl(kcmfd, SIOCKCMCLONE, &info);

  if (!err)
    newkcmfd = info.fd;

Attach transport sockets(挂载传输套接字)
----------------------------------------

Attaching of transport sockets to a multiplexor is performed by calling an
ioctl on a KCM socket for the multiplexor. e.g.::
(将传输套接字挂载到复用器，是通过对该复用器的一个 KCM 套接字调用 ioctl 完成的。例如：)

  /* From linux/kcm.h */
  struct kcm_attach {
	int fd;
	int bpf_fd;
  };

  struct kcm_attach info;

  memset(&info, 0, sizeof(info));

  info.fd = tcpfd;
  info.bpf_fd = bpf_prog_fd;

  ioctl(kcmfd, SIOCKCMATTACH, &info);

The kcm_attach structure contains:
(kcm_attach 结构包含：)

  - fd: file descriptor for TCP socket being attached
    (fd：被挂载的 TCP 套接字的文件描述符)
  - bpf_prog_fd: file descriptor for compiled BPF program downloaded
    (bpf_prog_fd：已下载的编译后 BPF 程序的文件描述符)

Unattach transport sockets(卸载传输套接字)
------------------------------------------

Unattaching a transport socket from a multiplexor is straightforward. An
"unattach" ioctl is done with the kcm_unattach structure as the argument::
(将传输套接字从复用器上卸载非常简单。执行 "unattach" ioctl 时，以 kcm_unattach 结构作为参数：)

  /* From linux/kcm.h */
  struct kcm_unattach {
	int fd;
  };

  struct kcm_unattach info;

  memset(&info, 0, sizeof(info));

  info.fd = cfd;

  ioctl(fd, SIOCKCMUNATTACH, &info);

Disabling receive on KCM socket(禁用 KCM 套接字上的接收)
--------------------------------------------------------

A setsockopt is used to disable or enable receiving on a KCM socket.
When receive is disabled, any pending messages in the socket's
receive buffer are moved to other sockets. This feature is useful
if an application thread knows that it will be doing a lot of
work on a request and won't be able to service new messages for a
while. Example use::
(使用一个 setsockopt 即可禁用或启用 KCM 套接字上的接收。当接收被禁用时，套接字接收缓冲区中所有待处理的消息都会被移到其他套接字。如果应用程序线程知道自己将要进行大量处理、一段时间内无法服务新消息，这一功能会很有用。用法示例：)

  int val = 1;

  setsockopt(kcmfd, SOL_KCM, KCM_RECV_DISABLE, &val, sizeof(val))

BPF programs for message delineation(用于消息定界的 BPF 程序)
-------------------------------------------------------------

BPF programs can be compiled using the BPF LLVM backend. For example,
the BPF program for parsing Thrift is::
(BPF 程序可以使用 BPF LLVM 后端进行编译。例如，用于解析 Thrift 的 BPF 程序如下：)

  #include "bpf.h" /* for __sk_buff */
  #include "bpf_helpers.h" /* for load_word intrinsic */

  SEC("socket_kcm")
  int bpf_prog1(struct __sk_buff *skb)
  {
       return load_word(skb, 0) + 4;
  }

  char _license[] SEC("license") = "GPL";

Use in applications(在应用程序中使用)
=====================================

KCM accelerates application layer protocols. Specifically, it allows
applications to use a message based interface for sending and receiving
messages. The kernel provides necessary assurances that messages are sent
and received atomically. This relieves much of the burden applications have
in mapping a message based protocol onto the TCP stream. KCM also make
application layer messages a unit of work in the kernel for the purposes of
steering and scheduling, which in turn allows a simpler networking model in
multithreaded applications.
(KCM 为应用层协议加速。具体而言，它允许应用程序使用基于消息的接口来发送和接收消息。内核提供必要的保证，确保消息被原子地发送和接收。这减轻了应用程序在把基于消息的协议映射到 TCP 流时的大部分负担。KCM 还使应用层消息成为内核中导向与调度方面的工作单元，这进而使多线程应用程序中的网络模型更加简单。)

Configurations(配置)
--------------------

In an Nx1 configuration, KCM logically provides multiple socket handles
to the same TCP connection. This allows parallelism between in I/O
operations on the TCP socket (for instance copyin and copyout of data is
parallelized). In an application, a KCM socket can be opened for each
processing thread and inserted into the epoll (similar to how SO_REUSEPORT
is used to allow multiple listener sockets on the same port).
(在 Nx1 配置中，KCM 逻辑上为同一条 TCP 连接提供多个套接字句柄。这允许 TCP 套接字上的 I/O 操作并行进行（例如数据的拷入和拷出被并行化）。在应用程序中，可以为每个处理线程打开一个 KCM 套接字，并将其加入 epoll（类似于用 SO_REUSEPORT 在同一端口上允许多个监听套接字的做法）。)

In a MxN configuration, multiple connections are established to the
same destination. These are used for simple load balancing.
(在 MxN 配置中，会建立多条指向同一目的地的连接。它们用于简单的负载均衡。)

Message batching(消息批处理)
----------------------------

The primary purpose of KCM is load balancing between KCM sockets and hence
threads in a nominal use case. Perfect load balancing, that is steering
each received message to a different KCM socket or steering each sent
message to a different TCP socket, can negatively impact performance
since this doesn't allow for affinities to be established. Balancing
based on groups, or batches of messages, can be beneficial for performance.
(KCM 的首要目的是在典型使用场景中于各个 KCM 套接字（进而是各线程）之间做负载均衡。完美的负载均衡——即把每条收到的消息导向到不同的 KCM 套接字，或把每条发送的消息导向到不同的 TCP 套接字——可能对性能有负面影响，因为这样无法建立亲和性。基于组或消息批次的均衡则可能有利于性能。)

On transmit, there are three ways an application can batch (pipeline)
messages on a KCM socket.
(在发送方向上，应用程序有三种方式可以在 KCM 套接字上对消息进行批处理（流水线化）。)

  1) Send multiple messages in a single sendmmsg.
     (在单次 sendmmsg 中发送多条消息。)
  2) Send a group of messages each with a sendmsg call, where all messages
     except the last have MSG_BATCH in the flags of sendmsg call.
     (对每条消息各调用一次 sendmsg 来发送一组消息，其中除最后一条外，所有消息都在 sendmsg 调用的 flags 中带有 MSG_BATCH。)
  3) Create "super message" composed of multiple messages and send this
     with a single sendmsg.
     (把多条消息组成一条"超级消息"，然后用单次 sendmsg 发送。)

On receive, the KCM module attempts to queue messages received on the
same KCM socket during each TCP ready callback. The targeted KCM socket
changes at each receive ready callback on the KCM socket. The application
does not need to configure this.
(在接收方向上，KCM 模块会尝试把每次 TCP 就绪回调期间在同一 KCM 套接字上收到的消息排入队列。目标 KCM 套接字会在该 KCM 套接字的每次接收就绪回调时更换。应用程序无需对此进行配置。)

Error handling(错误处理)
------------------------

An application should include a thread to monitor errors raised on
the TCP connection. Normally, this will be done by placing each
TCP socket attached to a KCM multiplexor in epoll set for POLLERR
event. If an error occurs on an attached TCP socket, KCM sets an EPIPE
on the socket thus waking up the application thread. When the application
sees the error (which may just be a disconnect) it should unattach the
socket from KCM and then close it. It is assumed that once an error is
posted on the TCP socket the data stream is unrecoverable (i.e. an error
may have occurred in the middle of receiving a message).
(应用程序应当包含一个线程来监控 TCP 连接上引发的错误。通常做法是把每个挂接到 KCM 复用器的 TCP 套接字都放入 epoll 集合以监听 POLLERR 事件。如果某个被挂接的 TCP 套接字发生错误，KCM 会在该套接字上设置 EPIPE，从而唤醒应用程序线程。当应用程序看到该错误（可能只是断开连接）时，应将该套接字从 KCM 上解除挂接，然后将其关闭。这里假定一旦在 TCP 套接字上出现错误，数据流便不可恢复（即错误可能发生在接收某条消息的中途）。)

TCP connection monitoring(TCP 连接监控)
---------------------------------------

In KCM there is no means to correlate a message to the TCP socket that
was used to send or receive the message (except in the case there is
only one attached TCP socket). However, the application does retain
an open file descriptor to the socket so it will be able to get statistics
from the socket which can be used in detecting issues (such as high
retransmissions on the socket).
(在 KCM 中，无法把某条消息关联到发送或接收它的 TCP 套接字（只有一条挂接 TCP 套接字的情况除外）。不过，应用程序仍然保留着该套接字的打开文件描述符，因此能够获取该套接字的统计信息，用于发现问题（例如该套接字上的高重传率）。)
