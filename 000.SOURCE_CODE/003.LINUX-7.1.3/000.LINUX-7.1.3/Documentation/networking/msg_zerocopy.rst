
============
MSG_ZEROCOPY
============

(MSG_ZEROCOPY)

Intro
(简介)
=====

The MSG_ZEROCOPY flag enables copy avoidance for socket send calls.
The feature is currently implemented for TCP, UDP and VSOCK (with
virtio transport) sockets.
(MSG_ZEROCOPY 标志为套接字发送调用启用免拷贝 (copy avoidance)。该特性目前为 TCP、UDP 和 VSOCK（使用 virtio 传输）套接字实现。)


Opportunity and Caveats
(机遇与注意事项)
-----------------------

Copying large buffers between user process and kernel can be
expensive. Linux supports various interfaces that eschew copying,
such as sendfile and splice. The MSG_ZEROCOPY flag extends the
underlying copy avoidance mechanism to common socket send calls.
(在用户进程与内核之间拷贝大缓冲区的开销可能很大。Linux 支持多种避免拷贝的接口，例如 sendfile 和 splice。MSG_ZEROCOPY 标志把底层的免拷贝机制扩展到了常用的套接字发送调用。)

Copy avoidance is not a free lunch. As implemented, with page pinning,
it replaces per byte copy cost with page accounting and completion
notification overhead. As a result, MSG_ZEROCOPY is generally only
effective at writes over around 10 KB.
(免拷贝并非免费的午餐。按其实现方式（页面固定 (page pinning)），它把逐字节的拷贝开销换成了页面记账和完成通知的开销。因此，MSG_ZEROCOPY 通常只对大约 10 KB 以上的写入才有效。)

Page pinning also changes system call semantics. It temporarily shares
the buffer between process and network stack. Unlike with copying, the
process cannot immediately overwrite the buffer after system call
return without possibly modifying the data in flight. Kernel integrity
is not affected, but a buggy program can possibly corrupt its own data
stream.
(页面固定还改变了系统调用的语义。它在进程与网络栈之间临时共享缓冲区。与拷贝不同，进程不能在系统调用返回后立即覆写缓冲区，否则可能修改仍在传输途中的数据。内核完整性不受影响，但有缺陷的程序可能会破坏自己的数据流。)

The kernel returns a notification when it is safe to modify data.
Converting an existing application to MSG_ZEROCOPY is not always as
trivial as just passing the flag, then.
(当可以安全修改数据时，内核会返回一个通知。因此，把现有应用改造为 MSG_ZEROCOPY 并不总是像只传个标志那样简单。)


More Info
(更多信息)
---------

Much of this document was derived from a longer paper presented at
netdev 2.1. For more in-depth information see that paper and talk,
the excellent reporting over at LWN.net or read the original code.
(本文档的很多内容源自 netdev 2.1 会议上发表的一篇较长的论文。欲深入了解，请阅读那篇论文和演讲、LWN.net 上的精彩报道，或直接阅读原始代码。)

  paper, slides, video
  (论文、幻灯片、视频)

    https://netdevconf.org/2.1/session.html?debruijn

  LWN article
  (LWN 文章)

    https://lwn.net/Articles/726917/

  patchset
  (补丁集)

    [PATCH net-next v4 0/9] socket sendmsg MSG_ZEROCOPY
    https://lore.kernel.org/netdev/20170803202945.70750-1-willemdebruijn.kernel@gmail.com


Interface
(接口)
=========

Passing the MSG_ZEROCOPY flag is the most obvious step to enable copy
avoidance, but not the only one.
(传递 MSG_ZEROCOPY 标志是启用免拷贝最显而易见的一步，但不是唯一一步。)

Socket Setup
(套接字设置)
------------

The kernel is permissive when applications pass undefined flags to the
send system call. By default it simply ignores these. To avoid enabling
copy avoidance mode for legacy processes that accidentally already pass
this flag, a process must first signal intent by setting a socket option:
(当应用程序向 send 系统调用传入未定义的标志时，内核是宽容的：默认只是忽略它们。为避免为碰巧已经传入此标志的遗留进程意外启用免拷贝模式，进程必须首先通过设置一个套接字选项来表明意图：)

::

	if (setsockopt(fd, SOL_SOCKET, SO_ZEROCOPY, &one, sizeof(one)))
		error(1, errno, "setsockopt zerocopy");

Transmission
(发送)
------------

The change to send (or sendto, sendmsg, sendmmsg) itself is trivial.
Pass the new flag.
(对 send（或 sendto、sendmsg、sendmmsg）本身的改动很简单。传入新标志即可。)

::

	ret = send(fd, buf, sizeof(buf), MSG_ZEROCOPY);

A zerocopy failure will return -1 with errno ENOBUFS. This happens if
the socket exceeds its optmem limit or the user exceeds their ulimit on
locked pages.
(零拷贝失败时会返回 -1 且 errno 为 ENOBUFS。当套接字超出其 optmem 限制，或用户超出锁定页面的 ulimit 时会发生这种情况。)


Mixing copy avoidance and copying
(混合使用免拷贝与拷贝)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Many workloads have a mixture of large and small buffers. Because copy
avoidance is more expensive than copying for small packets, the
feature is implemented as a flag. It is safe to mix calls with the flag
with those without.
(许多工作负载同时有大缓冲区和小缓冲区。由于对小数据包而言免拷贝比拷贝更昂贵，该特性被实现为一个标志。带标志的调用与不带标志的调用混用是安全的。)


Notifications
(通知)
-------------

The kernel has to notify the process when it is safe to reuse a
previously passed buffer. It queues completion notifications on the
socket error queue, akin to the transmit timestamping interface.
(内核必须在可以安全重用先前传递的缓冲区时通知进程。它把完成通知排入套接字错误队列，类似于发送时间戳接口。)

The notification itself is a simple scalar value. Each socket
maintains an internal unsigned 32-bit counter. Each send call with
MSG_ZEROCOPY that successfully sends data increments the counter. The
counter is not incremented on failure or if called with length zero.
The counter counts system call invocations, not bytes. It wraps after
UINT_MAX calls.
(通知本身是一个简单的标量值。每个套接字维护一个内部的无符号 32 位计数器。每个带 MSG_ZEROCOPY 且成功发送数据的 send 调用都会使该计数器递增。失败时或以零长度调用时不递增。该计数器统计的是系统调用次数，而不是字节数。它在 UINT_MAX 次调用后回绕。)


Notification Reception
(通知的接收)
~~~~~~~~~~~~~~~~~~~~~~

The below snippet demonstrates the API. In the simplest case, each
send syscall is followed by a poll and recvmsg on the error queue.
(下面的代码片段演示了该 API。在最简单的情况下，每次 send 系统调用之后跟随一次 poll 和一次对错误队列的 recvmsg。)

Reading from the error queue is always a non-blocking operation. The
poll call is there to block until an error is outstanding. It will set
POLLERR in its output flags. That flag does not have to be set in the
events field. Errors are signaled unconditionally.
(从错误队列读取始终是非阻塞操作。poll 调用用于阻塞等待，直到有一个错误待处理。它会在输出标志中置位 POLLERR。该标志不必在 events 字段中设置。错误是无条件上报的。)

::

	pfd.fd = fd;
	pfd.events = 0;
	if (poll(&pfd, 1, -1) != 1 || pfd.revents & POLLERR == 0)
		error(1, errno, "poll");

	ret = recvmsg(fd, &msg, MSG_ERRQUEUE);
	if (ret == -1)
		error(1, errno, "recvmsg");

	read_notification(msg);

The example is for demonstration purpose only. In practice, it is more
efficient to not wait for notifications, but read without blocking
every couple of send calls.
(该示例仅用于演示。实践中，更高效的做法是不要等待通知，而是每几次 send 调用之后无阻塞地读一次。)

Notifications can be processed out of order with other operations on
the socket. A socket that has an error queued would normally block
other operations until the error is read. Zerocopy notifications have
a zero error code, however, to not block send and recv calls.
(通知可以与套接字上的其他操作乱序处理。排队了错误的套接字通常会阻塞其他操作，直到该错误被读取。而零拷贝通知的错误码为零，因此不会阻塞 send 和 recv 调用。)


Notification Batching
(通知批处理)
~~~~~~~~~~~~~~~~~~~~~

Multiple outstanding packets can be read at once using the recvmmsg
call. This is often not needed. In each message the kernel returns not
a single value, but a range. It coalesces consecutive notifications
while one is outstanding for reception on the error queue.
(可以使用 recvmmsg 调用一次读取多个未完成的数据包。这通常没有必要。在每条消息中，内核返回的不是单个值，而是一个范围。当错误队列上已有通知待接收时，内核会把连续的通知合并。)

When a new notification is about to be queued, it checks whether the
new value extends the range of the notification at the tail of the
queue. If so, it drops the new notification packet and instead increases
the range upper value of the outstanding notification.
(当新通知即将入队时，它会检查新值是否扩展了队尾通知的范围。如果是，则丢弃新通知包，改为增大待处理通知的范围上限。)

For protocols that acknowledge data in-order, like TCP, each
notification can be squashed into the previous one, so that no more
than one notification is outstanding at any one point.
(对于按序确认数据的协议（如 TCP），每个通知都可以并入前一个通知，这样任何时刻最多只有一个通知待处理。)

Ordered delivery is the common case, but not guaranteed. Notifications
may arrive out of order on retransmission and socket teardown.
(有序送达是常见情形，但并不保证。在重传和套接字拆除时，通知可能乱序到达。)


Notification Parsing
(通知解析)
~~~~~~~~~~~~~~~~~~~~

The below snippet demonstrates how to parse the control message: the
read_notification() call in the previous snippet. A notification
is encoded in the standard error format, sock_extended_err.
(下面的代码片段演示如何解析控制消息：即上一段代码中的 read_notification() 调用。通知以标准错误格式 sock_extended_err 编码。)

The level and type fields in the control data are protocol family
specific, IP_RECVERR or IPV6_RECVERR (for TCP or UDP socket).
For VSOCK socket, cmsg_level will be SOL_VSOCK and cmsg_type will be
VSOCK_RECVERR.
(控制数据中的 level 和 type 字段取决于协议族，为 IP_RECVERR 或 IPV6_RECVERR（对 TCP 或 UDP 套接字）。对 VSOCK 套接字，cmsg_level 将为 SOL_VSOCK，cmsg_type 将为 VSOCK_RECVERR。)

Error origin is the new type SO_EE_ORIGIN_ZEROCOPY. ee_errno is zero,
as explained before, to avoid blocking read and write system calls on
the socket.
(错误来源 (origin) 是新类型 SO_EE_ORIGIN_ZEROCOPY。如前文所述，ee_errno 为零，以避免阻塞该套接字上的读写系统调用。)

The 32-bit notification range is encoded as [ee_info, ee_data]. This
range is inclusive. Other fields in the struct must be treated as
undefined, bar for ee_code, as discussed below.
(32 位通知范围编码为 [ee_info, ee_data]。该范围是闭区间。除了下文讨论的 ee_code 之外，结构中的其他字段都必须视为未定义。)

::

	struct sock_extended_err *serr;
	struct cmsghdr *cm;

	cm = CMSG_FIRSTHDR(msg);
	if (cm->cmsg_level != SOL_IP &&
	    cm->cmsg_type != IP_RECVERR)
		error(1, 0, "cmsg");

	serr = (void *) CMSG_DATA(cm);
	if (serr->ee_errno != 0 ||
	    serr->ee_origin != SO_EE_ORIGIN_ZEROCOPY)
		error(1, 0, "serr");

	printf("completed: %u..%u\n", serr->ee_info, serr->ee_data);


Deferred copies
(延迟拷贝)
~~~~~~~~~~~~~~~

Passing flag MSG_ZEROCOPY is a hint to the kernel to apply copy
avoidance, and a contract that the kernel will queue a completion
notification. It is not a guarantee that the copy is elided.
(传入 MSG_ZEROCOPY 标志是给内核的一个提示，要求其应用免拷贝，同时也是一个契约：内核会排队一个完成通知。它并不保证拷贝一定被省略。)

Copy avoidance is not always feasible. Devices that do not support
scatter-gather I/O cannot send packets made up of kernel generated
protocol headers plus zerocopy user data. A packet may need to be
converted to a private copy of data deep in the stack, say to compute
a checksum.
(免拷贝并不总是可行。不支持分散-聚集 I/O 的设备无法发送由内核生成的协议头加上零拷贝用户数据组成的数据包。数据包可能需要在协议栈深处转换为数据的私有副本，例如为了计算校验和。)

In all these cases, the kernel returns a completion notification when
it releases its hold on the shared pages. That notification may arrive
before the (copied) data is fully transmitted. A zerocopy completion
notification is not a transmit completion notification, therefore.
(在所有这些情况下，内核在释放对共享页面的持有时都会返回完成通知。该通知可能在（已拷贝的）数据完全发送之前到达。因此，零拷贝完成通知并不是发送完成通知。)

Deferred copies can be more expensive than a copy immediately in the
system call, if the data is no longer warm in the cache. The process
also incurs notification processing cost for no benefit. For this
reason, the kernel signals if data was completed with a copy, by
setting flag SO_EE_CODE_ZEROCOPY_COPIED in field ee_code on return.
A process may use this signal to stop passing flag MSG_ZEROCOPY on
subsequent requests on the same socket.
(如果数据已不在缓存中，延迟拷贝可能比在系统调用中立即拷贝更昂贵。进程还会白白承担通知处理成本。因此，当数据是通过拷贝完成时，内核会在返回时通过在 ee_code 字段中设置 SO_EE_CODE_ZEROCOPY_COPIED 标志来告知。进程可以利用这一信号，停止在同一套接字的后续请求中传递 MSG_ZEROCOPY 标志。)


Implementation
(实现)
==============

Loopback
(环回)
--------

For TCP and UDP:
Data sent to local sockets can be queued indefinitely if the receive
process does not read its socket. Unbound notification latency is not
acceptable. For this reason all packets generated with MSG_ZEROCOPY
that are looped to a local socket will incur a deferred copy. This
includes looping onto packet sockets (e.g., tcpdump) and tun devices.
(对于 TCP 和 UDP：如果接收进程不读取其套接字，发送到本地套接字的数据可能被无限期排队。无上界的通知延迟是不可接受的。因此，所有用 MSG_ZEROCOPY 生成且被回环到本地套接字的数据包都会经历一次延迟拷贝。这包括回环到包套接字（例如 tcpdump）和 tun 设备。)

For VSOCK:
Data path sent to local sockets is the same as for non-local sockets.
(对于 VSOCK：发送到本地套接字的数据路径与非本地套接字相同。)

Testing
(测试)
=======

More realistic example code can be found in the kernel source under
tools/testing/selftests/net/msg_zerocopy.c.
(更贴近实际的示例代码可在内核源码的 tools/testing/selftests/net/msg_zerocopy.c 中找到。)

Be cognizant of the loopback constraint. The test can be run between
a pair of hosts. But if run between a local pair of processes, for
instance when run with msg_zerocopy.sh between a veth pair across
namespaces, the test will not show any improvement. For testing, the
loopback restriction can be temporarily relaxed by making
skb_orphan_frags_rx identical to skb_orphan_frags.
(请留意环回限制。该测试可以在一对主机之间运行。但如果在一对本地进程之间运行，例如用 msg_zerocopy.sh 在跨命名空间的 veth 对之间运行，测试将不会显示任何改进。测试时，可以通过让 skb_orphan_frags_rx 与 skb_orphan_frags 完全相同来临时放宽环回限制。)

For VSOCK type of socket example can be found in
tools/testing/vsock/vsock_test_zerocopy.c.
(VSOCK 类型套接字的示例见 tools/testing/vsock/vsock_test_zerocopy.c。)
