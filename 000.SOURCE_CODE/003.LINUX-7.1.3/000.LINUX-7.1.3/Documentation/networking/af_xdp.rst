.. SPDX-License-Identifier: GPL-2.0

==================================================
AF_XDP(AF_XDP，针对高性能数据包处理而优化的地址族)
==================================================

Overview(概述)
==============

AF_XDP is an address family that is optimized for high performance
packet processing.
(AF_XDP 是一种针对高性能数据包处理而优化的地址族。)

This document assumes that the reader is familiar with BPF and XDP. If
not, the Cilium project has an excellent reference guide at
http://cilium.readthedocs.io/en/latest/bpf/.
(本文假定读者已经熟悉 BPF 和 XDP。如果不熟悉，Cilium 项目有一份很好的参考
指南：http://cilium.readthedocs.io/en/latest/bpf/。)

Using the XDP_REDIRECT action from an XDP program, the program can
redirect ingress frames to other XDP enabled netdevs, using the
bpf_redirect_map() function. AF_XDP sockets enable the possibility for
XDP programs to redirect frames to a memory buffer in a user-space
application.
(通过 XDP 程序中的 XDP_REDIRECT 动作，程序可以使用 bpf_redirect_map() 函数
把入站帧重定向到其他启用了 XDP 的网络设备。AF_XDP 套接字则使 XDP 程序能够把
帧重定向到用户空间应用程序中的一块内存缓冲区。)

An AF_XDP socket (XSK) is created with the normal socket()
syscall. Associated with each XSK are two rings: the RX ring and the
TX ring. A socket can receive packets on the RX ring and it can send
packets on the TX ring. These rings are registered and sized with the
setsockopts XDP_RX_RING and XDP_TX_RING, respectively. It is mandatory
to have at least one of these rings for each socket. An RX or TX
descriptor ring points to a data buffer in a memory area called a
UMEM. RX and TX can share the same UMEM so that a packet does not have
to be copied between RX and TX. Moreover, if a packet needs to be kept
for a while due to a possible retransmit, the descriptor that points
to that packet can be changed to point to another and reused right
away. This again avoids copying data.
(AF_XDP 套接字 (XSK) 通过普通的 socket() 系统调用创建。每个 XSK 关联两个环：
RX 环和 TX 环。套接字可以在 RX 环上接收数据包，也可以在 TX 环上发送数据包。
这些环分别通过 setsockopt 选项 XDP_RX_RING 和 XDP_TX_RING 注册并设定大小。
每个套接字必须至少拥有这两个环之一。RX 或 TX 描述符环指向一个被称为 UMEM 的
内存区域中的数据缓冲区。RX 和 TX 可以共享同一个 UMEM，这样数据包就无需在 RX
与 TX 之间拷贝。此外，如果某个数据包因为可能重传而需要保留一段时间，可以把
指向该数据包的描述符改为指向另一个缓冲区并立即复用，这同样避免了数据拷贝。)

The UMEM consists of a number of equally sized chunks. A descriptor in
one of the rings references a frame by referencing its addr. The addr
is simply an offset within the entire UMEM region. The user space
allocates memory for this UMEM using whatever means it feels is most
appropriate (malloc, mmap, huge pages, etc). This memory area is then
registered with the kernel using the new setsockopt XDP_UMEM_REG. The
UMEM also has two rings: the FILL ring and the COMPLETION ring. The
FILL ring is used by the application to send down addr for the kernel
to fill in with RX packet data. References to these frames will then
appear in the RX ring once each packet has been received. The
COMPLETION ring, on the other hand, contains frame addr that the
kernel has transmitted completely and can now be used again by user
space, for either TX or RX. Thus, the frame addrs appearing in the
COMPLETION ring are addrs that were previously transmitted using the
TX ring. In summary, the RX and FILL rings are used for the RX path
and the TX and COMPLETION rings are used for the TX path.
(UMEM 由若干大小相同的块 (chunk) 组成。环中的描述符通过引用帧的 addr 来引用
一个帧。addr 就是整个 UMEM 区域内的一个偏移量。用户空间可以用它认为最合适的
任何方式 (malloc、mmap、大页等) 为这个 UMEM 分配内存，然后通过新的 setsockopt
选项 XDP_UMEM_REG 把这块内存区域注册到内核。UMEM 也有两个环：FILL 环和
COMPLETION 环。应用程序使用 FILL 环向内核下发 addr，供内核填入 RX 数据包数据。
每个数据包接收完成后，对这些帧的引用就会出现在 RX 环中。而 COMPLETION 环则
包含内核已完全发送完毕的帧 addr，这些帧现在可以重新被用户空间使用，无论是用于
TX 还是 RX。因此，出现在 COMPLETION 环中的帧 addr 就是之前通过 TX 环发送过的
addr。总而言之，RX 环和 FILL 环用于 RX 路径，TX 环和 COMPLETION 环用于 TX 路径。)

The socket is then finally bound with a bind() call to a device and a
specific queue id on that device, and it is not until bind is
completed that traffic starts to flow.
(最后，套接字通过 bind() 调用绑定到一个设备以及该设备上的特定队列号，并且只有
在 bind 完成之后流量才会开始流动。)

The UMEM can be shared between processes, if desired. If a process
wants to do this, it simply skips the registration of the UMEM and its
corresponding two rings, sets the XDP_SHARED_UMEM flag in the bind
call and submits the XSK of the process it would like to share UMEM
with as well as its own newly created XSK socket. The new process will
then receive frame addr references in its own RX ring that point to
this shared UMEM. Note that since the ring structures are
single-consumer / single-producer (for performance reasons), the new
process has to create its own socket with associated RX and TX rings,
since it cannot share this with the other process. This is also the
reason that there is only one set of FILL and COMPLETION rings per
UMEM. It is the responsibility of a single process to handle the UMEM.
(如果需要，UMEM 可以在多个进程之间共享。进程想要这样做时，只需跳过 UMEM 及其
对应两个环的注册，在 bind 调用中设置 XDP_SHARED_UMEM 标志，并提交想要共享
UMEM 的那个进程的 XSK 以及自己新建的 XSK 套接字。之后，新进程会在自己的 RX 环
中收到指向这个共享 UMEM 的帧 addr 引用。注意，由于环结构是单消费者/单生产者的
(出于性能原因)，新进程必须创建自己的套接字以及关联的 RX 和 TX 环，因为它无法
与其他进程共享这些环。这也是每个 UMEM 只有一组 FILL 和 COMPLETION 环的原因：
由单个进程负责处理该 UMEM。)

How is then packets distributed from an XDP program to the XSKs? There
is a BPF map called XSKMAP (or BPF_MAP_TYPE_XSKMAP in full). The
user-space application can place an XSK at an arbitrary place in this
map. The XDP program can then redirect a packet to a specific index in
this map and at this point XDP validates that the XSK in that map was
indeed bound to that device and ring number. If not, the packet is
dropped. If the map is empty at that index, the packet is also
dropped. This also means that it is currently mandatory to have an XDP
program loaded (and one XSK in the XSKMAP) to be able to get any
traffic to user space through the XSK.
(那么，数据包是如何从 XDP 程序分发到各个 XSK 的呢？这里有一个名为 XSKMAP
(全称 BPF_MAP_TYPE_XSKMAP) 的 BPF 映射。用户空间应用程序可以把一个 XSK 放到
该映射的任意位置。XDP 程序随后可以把数据包重定向到该映射中的特定索引，此时
XDP 会验证该映射中该索引处的 XSK 是否确实绑定到了那个设备和环号。如果不是，
数据包将被丢弃。如果该索引处的映射为空，数据包同样会被丢弃。这也意味着，目前
必须加载一个 XDP 程序 (并且 XSKMAP 中有一个 XSK)，才能通过 XSK 把任何流量送达
用户空间。)

AF_XDP can operate in two different modes: XDP_SKB and XDP_DRV. If the
driver does not have support for XDP, or XDP_SKB is explicitly chosen
when loading the XDP program, XDP_SKB mode is employed that uses SKBs
together with the generic XDP support and copies out the data to user
space. A fallback mode that works for any network device. On the other
hand, if the driver has support for XDP, it will be used by the AF_XDP
code to provide better performance, but there is still a copy of the
data into user space.
(AF_XDP 可以在两种不同的模式下工作：XDP_SKB 和 XDP_DRV。如果驱动程序不支持
XDP，或者在加载 XDP 程序时显式选择了 XDP_SKB，就会采用 XDP_SKB 模式，它将
SKB 与通用 XDP 支持配合使用，并把数据拷贝到用户空间。这是一种对任何网络设备
都有效的回退模式。另一方面，如果驱动程序支持 XDP，AF_XDP 代码就会利用它来
提供更好的性能，但数据仍然要拷贝到用户空间。)

Concepts(基本概念)
==================

In order to use an AF_XDP socket, a number of associated objects need
to be setup. These objects and their options are explained in the
following sections.
(要使用 AF_XDP 套接字，需要设置若干关联对象。这些对象及其选项将在接下来的
几节中说明。)

For an overview on how AF_XDP works, you can also take a look at the
Linux Plumbers paper from 2018 on the subject:
http://vger.kernel.org/lpc_net2018_talks/lpc18_paper_af_xdp_perf-v2.pdf. Do
NOT consult the paper from 2017 on "AF_PACKET v4", the first attempt
at AF_XDP. Nearly everything changed since then. Jonathan Corbet has
also written an excellent article on LWN, "Accelerating networking
with AF_XDP". It can be found at https://lwn.net/Articles/750845/.
(关于 AF_XDP 工作原理的概览，还可以参阅 2018 年 Linux Plumbers 大会上关于该
主题的论文：
http://vger.kernel.org/lpc_net2018_talks/lpc18_paper_af_xdp_perf-v2.pdf。
请不要参阅 2017 年那篇关于 "AF_PACKET v4" (AF_XDP 的第一次尝试) 的论文，此后
几乎所有内容都改变了。Jonathan Corbet 还在 LWN 上写过一篇出色的文章
"Accelerating networking with AF_XDP"，可在 https://lwn.net/Articles/750845/
找到。)

UMEM(UMEM 内存区域)
-------------------

UMEM is a region of virtual contiguous memory, divided into
equal-sized frames. An UMEM is associated to a netdev and a specific
queue id of that netdev. It is created and configured (chunk size,
headroom, start address and size) by using the XDP_UMEM_REG setsockopt
system call. A UMEM is bound to a netdev and queue id, via the bind()
system call.
(UMEM 是一块虚拟连续内存区域，被划分为大小相同的帧 (frame)。一个 UMEM 关联到
一个网络设备以及该设备的特定队列号。它通过 XDP_UMEM_REG setsockopt 系统调用
创建和配置 (块大小、预留头部空间、起始地址和大小)。UMEM 通过 bind() 系统调用
绑定到一个网络设备和队列号。)

An AF_XDP is socket linked to a single UMEM, but one UMEM can have
multiple AF_XDP sockets. To share an UMEM created via one socket A,
the next socket B can do this by setting the XDP_SHARED_UMEM flag in
struct sockaddr_xdp member sxdp_flags, and passing the file descriptor
of A to struct sockaddr_xdp member sxdp_shared_umem_fd.
(一个 AF_XDP 套接字链接到单个 UMEM，但一个 UMEM 可以拥有多个 AF_XDP 套接字。
要共享经由套接字 A 创建的 UMEM，套接字 B 可以通过设置 struct sockaddr_xdp
成员 sxdp_flags 中的 XDP_SHARED_UMEM 标志，并把 A 的文件描述符传递给
struct sockaddr_xdp 成员 sxdp_shared_umem_fd 来实现。)

The UMEM has two single-producer/single-consumer rings that are used
to transfer ownership of UMEM frames between the kernel and the
user-space application.
(UMEM 有两个单生产者/单消费者环，用于在内核与用户空间应用程序之间转移 UMEM
帧的所有权。)

Rings(环)
---------

There are a four different kind of rings: FILL, COMPLETION, RX and
TX. All rings are single-producer/single-consumer, so the user-space
application need explicit synchronization of multiple
processes/threads are reading/writing to them.
(共有四种不同的环：FILL、COMPLETION、RX 和 TX。所有环都是单生产者/单消费者的，
因此当多个进程/线程读写这些环时，用户空间应用程序需要显式同步。)

The UMEM uses two rings: FILL and COMPLETION. Each socket associated
with the UMEM must have an RX queue, TX queue or both. Say, that there
is a setup with four sockets (all doing TX and RX). Then there will be
one FILL ring, one COMPLETION ring, four TX rings and four RX rings.
(UMEM 使用两个环：FILL 和 COMPLETION。与该 UMEM 关联的每个套接字必须拥有 RX
队列、TX 队列或两者兼有。假设有一个包含四个套接字 (全部同时做 TX 和 RX) 的
配置，那么将有一个 FILL 环、一个 COMPLETION 环、四个 TX 环和四个 RX 环。)

The rings are head(producer)/tail(consumer) based rings. A producer
writes the data ring at the index pointed out by struct xdp_ring
producer member, and increasing the producer index. A consumer reads
the data ring at the index pointed out by struct xdp_ring consumer
member, and increasing the consumer index.
(这些环是基于头 (生产者)/尾 (消费者) 的环。生产者在 struct xdp_ring 的
producer 成员所指向的索引处写入数据环，并递增生产者索引；消费者在
struct xdp_ring 的 consumer 成员所指向的索引处读取数据环，并递增消费者索引。)

The rings are configured and created via the _RING setsockopt system
calls and mmapped to user-space using the appropriate offset to mmap()
(XDP_PGOFF_RX_RING, XDP_PGOFF_TX_RING, XDP_UMEM_PGOFF_FILL_RING and
XDP_UMEM_PGOFF_COMPLETION_RING).
(环通过 _RING setsockopt 系统调用进行配置和创建，并使用 mmap() 的相应偏移量
(XDP_PGOFF_RX_RING、XDP_PGOFF_TX_RING、XDP_UMEM_PGOFF_FILL_RING 和
XDP_UMEM_PGOFF_COMPLETION_RING) 映射到用户空间。)

The size of the rings need to be of size power of two.
(环的大小必须是 2 的幂。)

UMEM Fill Ring(UMEM FILL 环)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The FILL ring is used to transfer ownership of UMEM frames from
user-space to kernel-space. The UMEM addrs are passed in the ring. As
an example, if the UMEM is 64k and each chunk is 4k, then the UMEM has
16 chunks and can pass addrs between 0 and 64k.
(FILL 环用于把 UMEM 帧的所有权从用户空间转移到内核空间。UMEM 的 addr 通过该
环传递。举例来说，如果 UMEM 为 64k 且每个块为 4k，那么 UMEM 有 16 个块，可以
传递 0 到 64k 之间的 addr。)

Frames passed to the kernel are used for the ingress path (RX rings).
(传递给内核的帧用于入站路径 (RX 环)。)

The user application produces UMEM addrs to this ring. Note that, if
running the application with aligned chunk mode, the kernel will mask
the incoming addr.  E.g. for a chunk size of 2k, the log2(2048) LSB of
the addr will be masked off, meaning that 2048, 2050 and 3000 refers
to the same chunk. If the user application is run in the unaligned
chunks mode, then the incoming addr will be left untouched.
(用户应用程序向该环生产 UMEM addr。注意，如果应用程序以对齐块模式运行，内核
会对传入的 addr 做掩码处理。例如，当块大小为 2k 时，addr 的低 log2(2048) 位
会被屏蔽，这意味着 2048、2050 和 3000 指向同一个块。如果用户应用程序以非对齐
块模式运行，传入的 addr 将保持原样。)


UMEM Completion Ring(UMEM COMPLETION 环)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The COMPLETION Ring is used transfer ownership of UMEM frames from
kernel-space to user-space. Just like the FILL ring, UMEM indices are
used.
(COMPLETION 环用于把 UMEM 帧的所有权从内核空间转移到用户空间。与 FILL 环一样，
这里使用的也是 UMEM 索引。)

Frames passed from the kernel to user-space are frames that has been
sent (TX ring) and can be used by user-space again.
(从内核传递到用户空间的帧是已经发送过的帧 (TX 环)，可以再次被用户空间使用。)

The user application consumes UMEM addrs from this ring.
(用户应用程序从该环消费 UMEM addr。)


RX Ring(RX 环)
~~~~~~~~~~~~~~

The RX ring is the receiving side of a socket. Each entry in the ring
is a struct xdp_desc descriptor. The descriptor contains UMEM offset
(addr) and the length of the data (len).
(RX 环是套接字的接收端。环中的每个条目都是一个 struct xdp_desc 描述符。描述符
包含 UMEM 偏移量 (addr) 和数据长度 (len)。)

If no frames have been passed to kernel via the FILL ring, no
descriptors will (or can) appear on the RX ring.
(如果没有通过 FILL 环把帧传递给内核，RX 环上将不会 (也不可能) 出现描述符。)

The user application consumes struct xdp_desc descriptors from this
ring.
(用户应用程序从该环消费 struct xdp_desc 描述符。)

TX Ring(TX 环)
~~~~~~~~~~~~~~

The TX ring is used to send frames. The struct xdp_desc descriptor is
filled (index, length and offset) and passed into the ring.
(TX 环用于发送帧。struct xdp_desc 描述符被填充 (索引、长度和偏移量) 后放入
该环。)

To start the transfer a sendmsg() system call is required. This might
be relaxed in the future.
(要启动传输，需要一次 sendmsg() 系统调用。这一要求将来可能会放宽。)

The user application produces struct xdp_desc descriptors to this
ring.
(用户应用程序向该环生产 struct xdp_desc 描述符。)

Libbpf(Libbpf 库)
=================

Libbpf is a helper library for eBPF and XDP that makes using these
technologies a lot simpler. It also contains specific helper functions
in tools/testing/selftests/bpf/xsk.h for facilitating the use of
AF_XDP. It contains two types of functions: those that can be used to
make the setup of AF_XDP socket easier and ones that can be used in the
data plane to access the rings safely and quickly.
(Libbpf 是一个面向 eBPF 和 XDP 的辅助库，让这些技术的使用简单得多。它还在
tools/testing/selftests/bpf/xsk.h 中包含专门用于简化 AF_XDP 使用的辅助函数。
它包含两类函数：一类用于简化 AF_XDP 套接字的设置，另一类可在数据平面中安全、
快速地访问各个环。)

We recommend that you use this library unless you have become a power
user. It will make your program a lot simpler.
(我们建议您使用这个库，除非您已经成为高级用户。它会让您的程序简单得多。)

XSKMAP / BPF_MAP_TYPE_XSKMAP(XSKMAP / BPF_MAP_TYPE_XSKMAP，XSK 套接字映射)
==========================================================================

On XDP side there is a BPF map type BPF_MAP_TYPE_XSKMAP (XSKMAP) that
is used in conjunction with bpf_redirect_map() to pass the ingress
frame to a socket.
(在 XDP 一侧有一个 BPF 映射类型 BPF_MAP_TYPE_XSKMAP (XSKMAP)，它与
bpf_redirect_map() 配合使用，把入站帧传递给套接字。)

The user application inserts the socket into the map, via the bpf()
system call.
(用户应用程序通过 bpf() 系统调用把套接字插入该映射。)

Note that if an XDP program tries to redirect to a socket that does
not match the queue configuration and netdev, the frame will be
dropped. E.g. an AF_XDP socket is bound to netdev eth0 and
queue 17. Only the XDP program executing for eth0 and queue 17 will
successfully pass data to the socket. Please refer to the sample
application (samples/bpf/) in for an example.
(注意，如果 XDP 程序试图重定向到一个与队列配置和网络设备不匹配的套接字，该帧
将被丢弃。例如，一个 AF_XDP 套接字绑定到网络设备 eth0 的队列 17，那么只有为
eth0 和队列 17 执行的 XDP 程序才能成功地把数据传给该套接字。示例请参考示例
应用程序 (samples/bpf/)。)

Configuration Flags and Socket Options(配置标志与套接字选项)
============================================================

These are the various configuration flags that can be used to control
and monitor the behavior of AF_XDP sockets.
(以下是可用于控制和监视 AF_XDP 套接字行为的各种配置标志。)

XDP_COPY and XDP_ZEROCOPY bind flags(XDP_COPY 与 XDP_ZEROCOPY 绑定标志)
-----------------------------------------------------------------------

When you bind to a socket, the kernel will first try to use zero-copy
copy. If zero-copy is not supported, it will fall back on using copy
mode, i.e. copying all packets out to user space. But if you would
like to force a certain mode, you can use the following flags. If you
pass the XDP_COPY flag to the bind call, the kernel will force the
socket into copy mode. If it cannot use copy mode, the bind call will
fail with an error. Conversely, the XDP_ZEROCOPY flag will force the
socket into zero-copy mode or fail.
(绑定套接字时，内核会首先尝试使用零拷贝。如果不支持零拷贝，将回退到拷贝模式，
即把所有数据包拷贝到用户空间。但如果您想强制使用某种模式，可以使用以下标志。
如果您在 bind 调用中传入 XDP_COPY 标志，内核将强制该套接字进入拷贝模式；如果
无法使用拷贝模式，bind 调用将报错失败。相反，XDP_ZEROCOPY 标志会强制该套接字
进入零拷贝模式，否则失败。)

XDP_SHARED_UMEM bind flag(XDP_SHARED_UMEM 绑定标志)
---------------------------------------------------

This flag enables you to bind multiple sockets to the same UMEM. It
works on the same queue id, between queue ids and between
netdevs/devices. In this mode, each socket has their own RX and TX
rings as usual, but you are going to have one or more FILL and
COMPLETION ring pairs. You have to create one of these pairs per
unique netdev and queue id tuple that you bind to.
(该标志使您能够把多个套接字绑定到同一个 UMEM。它可以在相同队列号上工作，也
可以跨队列号、跨网络设备/设备工作。在这种模式下，每个套接字照常拥有自己的 RX
和 TX 环，但您将拥有一对或多对 FILL 和 COMPLETION 环。您必须为绑定的每个唯一
的 (netdev, queue id) 组合创建一对这样的环。)

Starting with the case were we would like to share a UMEM between
sockets bound to the same netdev and queue id. The UMEM (tied to the
fist socket created) will only have a single FILL ring and a single
COMPLETION ring as there is only on unique netdev,queue_id tuple that
we have bound to. To use this mode, create the first socket and bind
it in the normal way. Create a second socket and create an RX and a TX
ring, or at least one of them, but no FILL or COMPLETION rings as the
ones from the first socket will be used. In the bind call, set he
XDP_SHARED_UMEM option and provide the initial socket's fd in the
sxdp_shared_umem_fd field. You can attach an arbitrary number of extra
sockets this way.
(先看在绑定到相同 netdev 和队列号的套接字之间共享 UMEM 的情况。由于只绑定了
一个唯一的 netdev,queue_id 组合，该 UMEM (绑定到最先创建的套接字) 将只有单个
FILL 环和单个 COMPLETION 环。要使用这种模式，先创建第一个套接字并按常规方式
绑定它。再创建第二个套接字，为其创建 RX 和 TX 环 (或至少其中之一)，但不要创建
FILL 或 COMPLETION 环，因为将使用第一个套接字的那些环。在 bind 调用中，设置
XDP_SHARED_UMEM 选项，并在 sxdp_shared_umem_fd 字段中提供初始套接字的 fd。
用这种方式可以附加任意数量的额外套接字。)

What socket will then a packet arrive on? This is decided by the XDP
program. Put all the sockets in the XSK_MAP and just indicate which
index in the array you would like to send each packet to. A simple
round-robin example of distributing packets is shown below:
(那么数据包会到达哪个套接字呢？这由 XDP 程序决定。把所有套接字都放入 XSK_MAP，
只需指明您想把每个数据包发送到数组中的哪个索引即可。下面展示了一个简单的轮询
(round-robin) 式数据包分发示例：)

.. code-block:: c

   #include <linux/bpf.h>
   #include "bpf_helpers.h"

   #define MAX_SOCKS 16

   struct {
       __uint(type, BPF_MAP_TYPE_XSKMAP);
       __uint(max_entries, MAX_SOCKS);
       __uint(key_size, sizeof(int));
       __uint(value_size, sizeof(int));
   } xsks_map SEC(".maps");

   static unsigned int rr;

   SEC("xdp_sock") int xdp_sock_prog(struct xdp_md *ctx)
   {
       rr = (rr + 1) & (MAX_SOCKS - 1);

       return bpf_redirect_map(&xsks_map, rr, XDP_DROP);
   }

Note, that since there is only a single set of FILL and COMPLETION
rings, and they are single producer, single consumer rings, you need
to make sure that multiple processes or threads do not use these rings
concurrently. There are no synchronization primitives in the
libbpf code that protects multiple users at this point in time.
(注意，由于只有一组 FILL 和 COMPLETION 环，而且它们是单生产者、单消费者环，
您必须确保多个进程或线程不会并发使用这些环。目前的 libbpf 代码中没有任何
保护多用户的同步原语。)

Libbpf uses this mode if you create more than one socket tied to the
same UMEM. However, note that you need to supply the
XSK_LIBBPF_FLAGS__INHIBIT_PROG_LOAD libbpf_flag with the
xsk_socket__create calls and load your own XDP program as there is no
built in one in libbpf that will route the traffic for you.
(如果您创建多个绑定到同一 UMEM 的套接字，libbpf 就会使用这种模式。不过要注意，
您需要在 xsk_socket__create 调用中提供 XSK_LIBBPF_FLAGS__INHIBIT_PROG_LOAD
这个 libbpf 标志，并加载自己的 XDP 程序，因为 libbpf 内置的程序无法为您路由
流量。)

The second case is when you share a UMEM between sockets that are
bound to different queue ids and/or netdevs. In this case you have to
create one FILL ring and one COMPLETION ring for each unique
netdev,queue_id pair. Let us say you want to create two sockets bound
to two different queue ids on the same netdev. Create the first socket
and bind it in the normal way. Create a second socket and create an RX
and a TX ring, or at least one of them, and then one FILL and
COMPLETION ring for this socket. Then in the bind call, set he
XDP_SHARED_UMEM option and provide the initial socket's fd in the
sxdp_shared_umem_fd field as you registered the UMEM on that
socket. These two sockets will now share one and the same UMEM.
(第二种情况是在绑定到不同队列号和/或不同 netdev 的套接字之间共享 UMEM。这种
情况下，您必须为每个唯一的 netdev,queue_id 组合创建一个 FILL 环和一个
COMPLETION 环。假设您想创建两个绑定到同一 netdev 上两个不同队列号的套接字：
先创建第一个套接字并按常规方式绑定；再创建第二个套接字，为其创建 RX 和 TX 环
(或至少其中之一)，然后为该套接字创建一个 FILL 环和一个 COMPLETION 环。接着在
bind 调用中设置 XDP_SHARED_UMEM 选项，并在 sxdp_shared_umem_fd 字段中提供初始
套接字的 fd (因为 UMEM 注册在那个套接字上)。现在这两个套接字将共享同一个
UMEM。)

There is no need to supply an XDP program like the one in the previous
case where sockets were bound to the same queue id and
device. Instead, use the NIC's packet steering capabilities to steer
the packets to the right queue. In the previous example, there is only
one queue shared among sockets, so the NIC cannot do this steering. It
can only steer between queues.
(无需像前一种情况那样提供 XDP 程序，因为那时套接字绑定的是相同的队列号和设备。
这里应改用网卡的包引导 (steering) 能力把数据包引导到正确的队列。在前面的示例
中，套接字之间只共享一个队列，因此网卡无法执行这种引导，它只能在队列之间引导。)

In libbpf, you need to use the xsk_socket__create_shared() API as it
takes a reference to a FILL ring and a COMPLETION ring that will be
created for you and bound to the shared UMEM. You can use this
function for all the sockets you create, or you can use it for the
second and following ones and use xsk_socket__create() for the first
one. Both methods yield the same result.
(在 libbpf 中，您需要使用 xsk_socket__create_shared() API，因为它接受对 FILL
环和 COMPLETION 环的引用，会为您创建它们并绑定到共享的 UMEM。您可以对自己
创建的所有套接字都使用这个函数，也可以只对第二个及之后的套接字使用它，而对
第一个套接字使用 xsk_socket__create()。两种做法结果相同。)

Note that a UMEM can be shared between sockets on the same queue id
and device, as well as between queues on the same device and between
devices at the same time.
(注意，UMEM 可以同时在相同队列号和设备的套接字之间、同一设备的不同队列之间
以及不同设备之间共享。)

XDP_USE_NEED_WAKEUP bind flag(XDP_USE_NEED_WAKEUP 绑定标志)
-----------------------------------------------------------

This option adds support for a new flag called need_wakeup that is
present in the FILL ring and the TX ring, the rings for which user
space is a producer. When this option is set in the bind call, the
need_wakeup flag will be set if the kernel needs to be explicitly
woken up by a syscall to continue processing packets. If the flag is
zero, no syscall is needed.
(该选项增加对一个名为 need_wakeup 的新标志的支持，该标志存在于 FILL 环和 TX
环 (即用户空间作为生产者的环) 中。在 bind 调用中设置该选项后，如果内核需要被
系统调用显式唤醒才能继续处理数据包，need_wakeup 标志就会被置位。如果该标志为
零，则不需要任何系统调用。)

If the flag is set on the FILL ring, the application needs to call
poll() to be able to continue to receive packets on the RX ring. This
can happen, for example, when the kernel has detected that there are no
more buffers on the FILL ring and no buffers left on the RX HW ring of
the NIC. In this case, interrupts are turned off as the NIC cannot
receive any packets (as there are no buffers to put them in), and the
need_wakeup flag is set so that user space can put buffers on the
FILL ring and then call poll() so that the kernel driver can put these
buffers on the HW ring and start to receive packets.
(如果该标志在 FILL 环上置位，应用程序需要调用 poll() 才能继续在 RX 环上接收
数据包。例如，当内核检测到 FILL 环上没有更多缓冲区、且网卡的 RX 硬件环上也
没有剩余缓冲区时，就会发生这种情况。此时中断被关闭，因为网卡无法接收任何数据包
(没有缓冲区可存放它们)，同时 need_wakeup 标志被置位，以便用户空间可以往 FILL
环放入缓冲区，然后调用 poll()，让内核驱动程序把这些缓冲区放到硬件环上并开始
接收数据包。)

If the flag is set for the TX ring, it means that the application
needs to explicitly notify the kernel to send any packets put on the
TX ring. This can be accomplished either by a poll() call, as in the
RX path, or by calling sendto().
(如果该标志在 TX 环上置位，则表示应用程序需要显式通知内核去发送放到 TX 环上
的数据包。这可以像 RX 路径那样通过调用 poll() 来完成，也可以通过调用 sendto()
来完成。)

An example with the use of libbpf helpers would look like this for the
TX path:
(对于 TX 路径，使用 libbpf 辅助函数的示例如下：)

.. code-block:: c

   if (xsk_ring_prod__needs_wakeup(&my_tx_ring))
       sendto(xsk_socket__fd(xsk_handle), NULL, 0, MSG_DONTWAIT, NULL, 0);

I.e., only use the syscall if the flag is set.
(也就是说，只有在该标志置位时才使用系统调用。)

We recommend that you always enable this mode as it usually leads to
better performance especially if you run the application and the
driver on the same core, but also if you use different cores for the
application and the kernel driver, as it reduces the number of
syscalls needed for the TX path.
(我们建议您始终启用这种模式，因为它通常会带来更好的性能，尤其是在应用程序与
驱动程序运行在同一个核心上时；即使应用程序与内核驱动程序使用不同的核心，它
也能减少 TX 路径所需的系统调用次数。)

XDP_{RX|TX|UMEM_FILL|UMEM_COMPLETION}_RING setsockopts(XDP_{RX|TX|UMEM_FILL|UMEM_COMPLETION}_RING setsockopt 选项)
------------------------------------------------------------------------------------------------------------------

These setsockopts sets the number of descriptors that the RX, TX,
FILL, and COMPLETION rings respectively should have. It is mandatory
to set the size of at least one of the RX and TX rings. If you set
both, you will be able to both receive and send traffic from your
application, but if you only want to do one of them, you can save
resources by only setting up one of them. Both the FILL ring and the
COMPLETION ring are mandatory as you need to have a UMEM tied to your
socket. But if the XDP_SHARED_UMEM flag is used, any socket after the
first one does not have a UMEM and should in that case not have any
FILL or COMPLETION rings created as the ones from the shared UMEM will
be used. Note, that the rings are single-producer single-consumer, so
do not try to access them from multiple processes at the same
time. See the XDP_SHARED_UMEM section.
(这些 setsockopt 选项分别设置 RX、TX、FILL 和 COMPLETION 环应拥有的描述符
数量。必须至少设置 RX 和 TX 环之一的大小。如果两者都设置，您的应用程序就
既能接收又能发送流量；如果只想做其中一种，可以只设置一个以节省资源。FILL 环
和 COMPLETION 环都是必需的，因为您需要有 UMEM 绑定到套接字。但如果使用了
XDP_SHARED_UMEM 标志，第一个套接字之后的任何套接字都没有 UMEM，在这种情况下
不应为它们创建任何 FILL 或 COMPLETION 环，因为将使用共享 UMEM 的那些环。注意，
这些环是单生产者单消费者的，不要尝试从多个进程同时访问它们。参见 XDP_SHARED_UMEM
一节。)

In libbpf, you can create Rx-only and Tx-only sockets by supplying
NULL to the rx and tx arguments, respectively, to the
xsk_socket__create function.
(在 libbpf 中，向 xsk_socket__create 函数的 rx 和 tx 参数分别传入 NULL，即可
创建仅 Rx 和仅 Tx 的套接字。)

If you create a Tx-only socket, we recommend that you do not put any
packets on the fill ring. If you do this, drivers might think you are
going to receive something when you in fact will not, and this can
negatively impact performance.
(如果您创建仅 Tx 的套接字，我们建议不要往 FILL 环放任何数据包。否则，驱动程序
可能以为您将要接收数据，而实际上并不会，这会对性能产生负面影响。)

XDP_UMEM_REG setsockopt(XDP_UMEM_REG setsockopt 选项)
-----------------------------------------------------

This setsockopt registers a UMEM to a socket. This is the area that
contain all the buffers that packet can reside in. The call takes a
pointer to the beginning of this area and the size of it. Moreover, it
also has parameter called chunk_size that is the size that the UMEM is
divided into. It can only be 2K or 4K at the moment. If you have an
UMEM area that is 128K and a chunk size of 2K, this means that you
will be able to hold a maximum of 128K / 2K = 64 packets in your UMEM
area and that your largest packet size can be 2K.
(该 setsockopt 把一个 UMEM 注册到套接字。这是存放数据包所能驻留的全部缓冲区
的区域。该调用接受指向该区域开头的指针及其大小。此外，它还有一个名为
chunk_size 的参数，即 UMEM 被划分成的块大小。目前只能是 2K 或 4K。如果您有
一个 128K 的 UMEM 区域且块大小为 2K，这意味着您的 UMEM 区域最多能容纳
128K / 2K = 64 个数据包，最大数据包大小可以是 2K。)

There is also an option to set the headroom of each single buffer in
the UMEM. If you set this to N bytes, it means that the packet will
start N bytes into the buffer leaving the first N bytes for the
application to use. The final option is the flags field, but it will
be dealt with in separate sections for each UMEM flag.
(还有一个选项用于设置 UMEM 中每个缓冲区的预留头部空间 (headroom)。如果把它设
为 N 字节，则意味着数据包将从缓冲区内偏移 N 字节处开始，前 N 字节留给应用
程序使用。最后一个选项是 flags 字段，将在每个 UMEM 标志各自的章节中分别介绍。)

SO_BINDTODEVICE setsockopt(SO_BINDTODEVICE setsockopt 选项)
-----------------------------------------------------------

This is a generic SOL_SOCKET option that can be used to tie AF_XDP
socket to a particular network interface.  It is useful when a socket
is created by a privileged process and passed to a non-privileged one.
Once the option is set, kernel will refuse attempts to bind that socket
to a different interface.  Updating the value requires CAP_NET_RAW.
(这是一个通用的 SOL_SOCKET 选项，可用于把 AF_XDP 套接字绑定到特定的网络接口。
当套接字由特权进程创建并传递给非特权进程时非常有用。一旦设置了该选项，内核将
拒绝把该套接字绑定到其他接口的尝试。更新该值需要 CAP_NET_RAW 权限。)

XDP_MAX_TX_SKB_BUDGET setsockopt(XDP_MAX_TX_SKB_BUDGET setsockopt 选项)
-----------------------------------------------------------------------

This setsockopt sets the maximum number of descriptors that can be handled
and passed to the driver at one send syscall. It is applied in the copy
mode to allow application to tune the per-socket maximum iteration for
better throughput and less frequency of send syscall.
Allowed range is [32, xs->tx->nentries].
(该 setsockopt 设置一次 send 系统调用所能处理并传递给驱动程序的描述符数量
上限。它应用于拷贝模式，允许应用程序调整每套接字的最大迭代次数，以获得更高的
吞吐量和更低的 send 系统调用频率。允许范围为 [32, xs->tx->nentries]。)

XDP_STATISTICS getsockopt(XDP_STATISTICS getsockopt 选项)
---------------------------------------------------------

Gets drop statistics of a socket that can be useful for debug
purposes. The supported statistics are shown below:
(获取套接字的丢包统计信息，对调试很有用。支持的统计项如下所示：)

.. code-block:: c

   struct xdp_statistics {
       __u64 rx_dropped; /* Dropped for reasons other than invalid desc */
       __u64 rx_invalid_descs; /* Dropped due to invalid descriptor */
       __u64 tx_invalid_descs; /* Dropped due to invalid descriptor */
   };

XDP_OPTIONS getsockopt(XDP_OPTIONS getsockopt 选项)
---------------------------------------------------

Gets options from an XDP socket. The only one supported so far is
XDP_OPTIONS_ZEROCOPY which tells you if zero-copy is on or not.
(从 XDP 套接字获取选项。目前唯一受支持的是 XDP_OPTIONS_ZEROCOPY，它告诉您
零拷贝是否开启。)

Multi-Buffer Support(多缓冲支持)
================================

With multi-buffer support, programs using AF_XDP sockets can receive
and transmit packets consisting of multiple buffers both in copy and
zero-copy mode. For example, a packet can consist of two
frames/buffers, one with the header and the other one with the data,
or a 9K Ethernet jumbo frame can be constructed by chaining together
three 4K frames.
(借助多缓冲支持，使用 AF_XDP 套接字的程序在拷贝模式和零拷贝模式下都可以接收
和发送由多个缓冲区组成的数据包。例如，一个数据包可以由两个帧/缓冲区组成，一个
放头部，另一个放数据；也可以把三个 4K 帧串联起来构成一个 9K 的以太网巨型帧。)

Some definitions:
(一些定义：)

* A packet consists of one or more frames
  (一个数据包由一个或多个帧组成)

* A descriptor in one of the AF_XDP rings always refers to a single
  frame. In the case the packet consists of a single frame, the
  descriptor refers to the whole packet.
  (AF_XDP 各环中的一个描述符总是只引用单个帧。当数据包只由单个帧组成时，该
  描述符即引用整个数据包。)

To enable multi-buffer support for an AF_XDP socket, use the new bind
flag XDP_USE_SG. If this is not provided, all multi-buffer packets
will be dropped just as before. Note that the XDP program loaded also
needs to be in multi-buffer mode. This can be accomplished by using
"xdp.frags" as the section name of the XDP program used.
(要为 AF_XDP 套接字启用多缓冲支持，请使用新的绑定标志 XDP_USE_SG。如果不提供
该标志，所有多缓冲数据包都会像以前一样被丢弃。注意，所加载的 XDP 程序也必须
处于多缓冲模式，这可以通过把 "xdp.frags" 用作所用 XDP 程序的段名来实现。)

To represent a packet consisting of multiple frames, a new flag called
XDP_PKT_CONTD is introduced in the options field of the Rx and Tx
descriptors. If it is true (1) the packet continues with the next
descriptor and if it is false (0) it means this is the last descriptor
of the packet. Why the reverse logic of end-of-packet (eop) flag found
in many NICs? Just to preserve compatibility with non-multi-buffer
applications that have this bit set to false for all packets on Rx,
and the apps set the options field to zero for Tx, as anything else
will be treated as an invalid descriptor.
(为了表示由多个帧组成的数据包，在 Rx 和 Tx 描述符的 options 字段中引入了一个
名为 XDP_PKT_CONTD 的新标志。若它为真 (1)，表示数据包在下一个描述符中继续；
若它为假 (0)，则表示这是该数据包的最后一个描述符。为什么与许多网卡中常见的
包结束 (eop) 标志的逻辑相反？只是为了与不支持多缓冲的应用程序保持兼容——这些
应用程序在 Rx 方向对所有数据包都把该位置为假，而在 Tx 方向把 options 字段设为
零，因为任何其他值都会被视为无效描述符。)

These are the semantics for producing packets onto AF_XDP Tx ring
consisting of multiple frames:
(以下是向 AF_XDP Tx 环生产由多个帧组成的数据包时的语义：)

* When an invalid descriptor is found, all the other
  descriptors/frames of this packet are marked as invalid and not
  completed. The next descriptor is treated as the start of a new
  packet, even if this was not the intent (because we cannot guess
  the intent). As before, if your program is producing invalid
  descriptors you have a bug that must be fixed.
  (当发现无效描述符时，该数据包的所有其他描述符/帧都会被标记为无效且不完成。
  下一个描述符将被视为新数据包的开始，即使这并非本意 (因为我们无法猜测意图)。
  与之前一样，如果您的程序产生了无效描述符，说明存在必须修复的缺陷。)

* Zero length descriptors are treated as invalid descriptors.
  (零长度描述符被视为无效描述符。)

* For copy mode, the maximum supported number of frames in a packet is
  equal to CONFIG_MAX_SKB_FRAGS + 1. If it is exceeded, all
  descriptors accumulated so far are dropped and treated as
  invalid. To produce an application that will work on any system
  regardless of this config setting, limit the number of frags to 18,
  as the minimum value of the config is 17.
  (对于拷贝模式，一个数据包支持的最大帧数等于 CONFIG_MAX_SKB_FRAGS + 1。如果
  超过该值，目前累积的所有描述符都会被丢弃并视为无效。要编写一个在任何系统上
  都能正常工作、不受该配置项影响的应用程序，请把 frag 数量限制在 18 以内，因为
  该配置的最小值是 17。)

* For zero-copy mode, the limit is up to what the NIC HW
  supports. Usually at least five on the NICs we have checked. We
  consciously chose to not enforce a rigid limit (such as
  CONFIG_MAX_SKB_FRAGS + 1) for zero-copy mode, as it would have
  resulted in copy actions under the hood to fit into what limit the
  NIC supports. Kind of defeats the purpose of zero-copy mode. How to
  probe for this limit is explained in the "probe for multi-buffer
  support" section.
  (对于零拷贝模式，上限取决于网卡硬件所支持的程度。在我们检查过的网卡上通常
  至少为 5。我们有意不为零拷贝模式强制一个僵化的上限 (例如
  CONFIG_MAX_SKB_FRAGS + 1)，否则为了迁就网卡支持的限额，底层将不得不执行拷贝
  操作，这多少违背了零拷贝模式的初衷。如何探测该限额，请参见 "probe for
  multi-buffer support" 一节。)

On the Rx path in copy-mode, the xsk core copies the XDP data into
multiple descriptors, if needed, and sets the XDP_PKT_CONTD flag as
detailed before. Zero-copy mode works the same, though the data is not
copied. When the application gets a descriptor with the XDP_PKT_CONTD
flag set to one, it means that the packet consists of multiple buffers
and it continues with the next buffer in the following
descriptor. When a descriptor with XDP_PKT_CONTD == 0 is received, it
means that this is the last buffer of the packet. AF_XDP guarantees
that only a complete packet (all frames in the packet) is sent to the
application. If there is not enough space in the AF_XDP Rx ring, all
frames of the packet will be dropped.
(在拷贝模式的 Rx 路径上，xsk 核心会在需要时把 XDP 数据拷贝到多个描述符中，并
按前述方式设置 XDP_PKT_CONTD 标志。零拷贝模式的工作方式相同，只是数据不被
拷贝。当应用程序收到一个 XDP_PKT_CONTD 标志为 1 的描述符时，表示该数据包由
多个缓冲区组成，并且会在下一个描述符中继续下一个缓冲区。当收到
XDP_PKT_CONTD == 0 的描述符时，表示这是该数据包的最后一个缓冲区。AF_XDP 保证
只会把完整的数据包 (数据包的全部帧) 交付给应用程序。如果 AF_XDP Rx 环中没有
足够的空间，该数据包的所有帧都会被丢弃。)

If application reads a batch of descriptors, using for example the libxdp
interfaces, it is not guaranteed that the batch will end with a full
packet. It might end in the middle of a packet and the rest of the
buffers of that packet will arrive at the beginning of the next batch,
since the libxdp interface does not read the whole ring (unless you
have an enormous batch size or a very small ring size).
(如果应用程序一次读取一批描述符 (例如使用 libxdp 接口)，并不保证这一批会以
完整的数据包结尾。它可能停在某个数据包的中间，该数据包剩余的缓冲区会在下一批
的开头到达，因为 libxdp 接口并不会读取整个环 (除非您的批大小极大或环大小极小)。)

An example program each for Rx and Tx multi-buffer support can be found
later in this document.
(Rx 和 Tx 多缓冲支持各自的示例程序见本文档后面的内容。)

Usage(用法)
-----------

In order to use AF_XDP sockets two parts are needed. The user-space
application and the XDP program. For a complete setup and usage example,
please refer to the xdp-project at
https://github.com/xdp-project/bpf-examples/tree/main/AF_XDP-example.
(使用 AF_XDP 套接字需要两部分：用户空间应用程序和 XDP 程序。完整的设置和
使用示例请参考 xdp-project：
https://github.com/xdp-project/bpf-examples/tree/main/AF_XDP-example。)

The XDP code sample is the following:
(XDP 代码示例如下：)

.. code-block:: c

   SEC("xdp_sock") int xdp_sock_prog(struct xdp_md *ctx)
   {
       int index = ctx->rx_queue_index;

       // A set entry here means that the corresponding queue_id
       // has an active AF_XDP socket bound to it.
       if (bpf_map_lookup_elem(&xsks_map, &index))
           return bpf_redirect_map(&xsks_map, index, 0);

       return XDP_PASS;
   }

A simple but not so performance ring dequeue and enqueue could look
like this:
(一个简单但性能不佳的环出队/入队实现可能如下：)

.. code-block:: c

    // struct xdp_rxtx_ring {
    //     __u32 *producer;
    //     __u32 *consumer;
    //     struct xdp_desc *desc;
    // };

    // struct xdp_umem_ring {
    //     __u32 *producer;
    //     __u32 *consumer;
    //     __u64 *desc;
    // };

    // typedef struct xdp_rxtx_ring RING;
    // typedef struct xdp_umem_ring RING;

    // typedef struct xdp_desc RING_TYPE;
    // typedef __u64 RING_TYPE;

    int dequeue_one(RING *ring, RING_TYPE *item)
    {
        __u32 entries = *ring->producer - *ring->consumer;

        if (entries == 0)
            return -1;

        // read-barrier!

        *item = ring->desc[*ring->consumer & (RING_SIZE - 1)];
        (*ring->consumer)++;
        return 0;
    }

    int enqueue_one(RING *ring, const RING_TYPE *item)
    {
        u32 free_entries = RING_SIZE - (*ring->producer - *ring->consumer);

        if (free_entries == 0)
            return -1;

        ring->desc[*ring->producer & (RING_SIZE - 1)] = *item;

        // write-barrier!

        (*ring->producer)++;
        return 0;
    }

But please use the libbpf functions as they are optimized and ready to
use. Will make your life easier.
(但请使用 libbpf 的函数，它们经过优化且开箱即用，会让您的生活更轻松。)

Usage Multi-Buffer Rx(多缓冲 Rx 的用法)
---------------------------------------

Here is a simple Rx path pseudo-code example (using libxdp interfaces
for simplicity). Error paths have been excluded to keep it short:
(下面是一个简单的 Rx 路径伪代码示例 (为简单起见使用 libxdp 接口)。为保持
简短，省略了错误处理路径：)

.. code-block:: c

    void rx_packets(struct xsk_socket_info *xsk)
    {
        static bool new_packet = true;
        u32 idx_rx = 0, idx_fq = 0;
        static char *pkt;

        int rcvd = xsk_ring_cons__peek(&xsk->rx, opt_batch_size, &idx_rx);

        xsk_ring_prod__reserve(&xsk->umem->fq, rcvd, &idx_fq);

        for (int i = 0; i < rcvd; i++) {
            struct xdp_desc *desc = xsk_ring_cons__rx_desc(&xsk->rx, idx_rx++);
            char *frag = xsk_umem__get_data(xsk->umem->buffer, desc->addr);
            bool eop = !(desc->options & XDP_PKT_CONTD);

            if (new_packet)
                pkt = frag;
            else
                add_frag_to_pkt(pkt, frag);

            if (eop)
                process_pkt(pkt);

            new_packet = eop;

            *xsk_ring_prod__fill_addr(&xsk->umem->fq, idx_fq++) = desc->addr;
        }

        xsk_ring_prod__submit(&xsk->umem->fq, rcvd);
        xsk_ring_cons__release(&xsk->rx, rcvd);
    }

Usage Multi-Buffer Tx(多缓冲 Tx 的用法)
---------------------------------------

Here is an example Tx path pseudo-code (using libxdp interfaces for
simplicity) ignoring that the umem is finite in size, and that we
eventually will run out of packets to send. Also assumes pkts.addr
points to a valid location in the umem.
(下面是一个 Tx 路径伪代码示例 (为简单起见使用 libxdp 接口)，它忽略了 umem
大小有限、发送的数据包最终会耗尽的情况，并假定 pkts.addr 指向 umem 中的有效
位置。)

.. code-block:: c

    void tx_packets(struct xsk_socket_info *xsk, struct pkt *pkts,
                    int batch_size)
    {
        u32 idx, i, pkt_nb = 0;

        xsk_ring_prod__reserve(&xsk->tx, batch_size, &idx);

        for (i = 0; i < batch_size;) {
            u64 addr = pkts[pkt_nb].addr;
            u32 len = pkts[pkt_nb].size;

            do {
                struct xdp_desc *tx_desc;

                tx_desc = xsk_ring_prod__tx_desc(&xsk->tx, idx + i++);
                tx_desc->addr = addr;

                if (len > xsk_frame_size) {
                    tx_desc->len = xsk_frame_size;
                    tx_desc->options = XDP_PKT_CONTD;
                } else {
                    tx_desc->len = len;
                    tx_desc->options = 0;
                    pkt_nb++;
                }
                len -= tx_desc->len;
                addr += xsk_frame_size;

                if (i == batch_size) {
                    /* Remember len, addr, pkt_nb for next iteration.
                     * Skipped for simplicity.
                     */
                    break;
                }
            } while (len);
        }

        xsk_ring_prod__submit(&xsk->tx, i);
    }

Probing for Multi-Buffer Support(探测多缓冲支持)
------------------------------------------------

To discover if a driver supports multi-buffer AF_XDP in SKB or DRV
mode, use the XDP_FEATURES feature of netlink in linux/netdev.h to
query for NETDEV_XDP_ACT_RX_SG support. This is the same flag as for
querying for XDP multi-buffer support. If XDP supports multi-buffer in
a driver, then AF_XDP will also support that in SKB and DRV mode.
(要探测驱动程序在 SKB 或 DRV 模式下是否支持多缓冲 AF_XDP，可使用
linux/netdev.h 中 netlink 的 XDP_FEATURES 特性查询 NETDEV_XDP_ACT_RX_SG
支持。该标志与查询 XDP 多缓冲支持所用的标志相同。如果 XDP 在某个驱动程序中
支持多缓冲，那么 AF_XDP 在 SKB 和 DRV 模式下也会支持它。)

To discover if a driver supports multi-buffer AF_XDP in zero-copy
mode, use XDP_FEATURES and first check the NETDEV_XDP_ACT_XSK_ZEROCOPY
flag. If it is set, it means that at least zero-copy is supported and
you should go and check the netlink attribute
NETDEV_A_DEV_XDP_ZC_MAX_SEGS in linux/netdev.h. An unsigned integer
value will be returned stating the max number of frags that are
supported by this device in zero-copy mode. These are the possible
return values:
(要探测驱动程序在零拷贝模式下是否支持多缓冲 AF_XDP，同样使用 XDP_FEATURES，
先检查 NETDEV_XDP_ACT_XSK_ZEROCOPY 标志。如果它已置位，说明至少支持零拷贝，
接着应检查 linux/netdev.h 中的 netlink 属性 NETDEV_A_DEV_XDP_ZC_MAX_SEGS。
将返回一个无符号整数值，表明该设备在零拷贝模式下支持的最大 frag 数量。可能的
返回值如下：)

1: Multi-buffer for zero-copy is not supported by this device, as max
   one fragment supported means that multi-buffer is not possible.
   (返回 1：该设备不支持零拷贝多缓冲，因为最多只支持一个分片就意味着无法实现
   多缓冲。)

>=2: Multi-buffer is supported in zero-copy mode for this device. The
     returned number signifies the max number of frags supported.
     (返回值 >=2：该设备在零拷贝模式下支持多缓冲。返回的数字表示所支持的最大
     frag 数量。)

For an example on how these are used through libbpf, please take a
look at tools/testing/selftests/bpf/xskxceiver.c.
(有关如何在 libbpf 中使用这些接口的示例，请参阅
tools/testing/selftests/bpf/xskxceiver.c。)

Multi-Buffer Support for Zero-Copy Drivers(零拷贝驱动程序的多缓冲支持)
----------------------------------------------------------------------

Zero-copy drivers usually use the batched APIs for Rx and Tx
processing. Note that the Tx batch API guarantees that it will provide
a batch of Tx descriptors that ends with full packet at the end. This
to facilitate extending a zero-copy driver with multi-buffer support.
(零拷贝驱动程序通常使用成批的 API 进行 Rx 和 Tx 处理。注意，Tx 批量 API 保证
提供的一批 Tx 描述符会以完整的数据包结尾。这是为了便于为零拷贝驱动程序扩展
多缓冲支持。)

Sample application(示例应用程序)
================================

There is a xdpsock benchmarking/test application that can be found at
https://github.com/xdp-project/bpf-examples/tree/main/AF_XDP-example
that demonstrates how to use AF_XDP sockets with private
UMEMs. Say that you would like your UDP traffic from port 4242 to end
up in queue 16, that we will enable AF_XDP on. Here, we use ethtool
for this::
(有一个 xdpsock 基准测试/测试应用程序，位于
https://github.com/xdp-project/bpf-examples/tree/main/AF_XDP-example，它演示了
如何使用带私有 UMEM 的 AF_XDP 套接字。假设您希望来自端口 4242 的 UDP 流量
进入队列 16，而我们将在该队列上启用 AF_XDP。为此我们使用 ethtool：)

      ethtool -N p3p2 rx-flow-hash udp4 fn
      ethtool -N p3p2 flow-type udp4 src-port 4242 dst-port 4242 \

          action 16

Running the rxdrop benchmark in XDP_DRV mode can then be done
using::
(随后即可使用如下命令以 XDP_DRV 模式运行 rxdrop 基准测试：)

      samples/bpf/xdpsock -i p3p2 -q 16 -r -N

For XDP_SKB mode, use the switch "-S" instead of "-N" and all options
can be displayed with "-h", as usual.
(对于 XDP_SKB 模式，请用开关 "-S" 代替 "-N"，所有选项照常可以用 "-h" 显示。)

This sample application uses libbpf to make the setup and usage of
AF_XDP simpler. If you want to know how the raw uapi of AF_XDP is
really used to make something more advanced, take a look at the libbpf
code in tools/testing/selftests/bpf/xsk.[ch].
(该示例应用程序使用 libbpf 来简化 AF_XDP 的设置和使用。如果您想了解 AF_XDP
的原始 uapi 究竟如何用于实现更高级的功能，请查看 tools/testing/selftests/bpf/
xsk.[ch] 中的 libbpf 代码。)

FAQ(常见问题解答)
=================

Q: I am not seeing any traffic on the socket. What am I doing wrong?
(Q：我在套接字上看不到任何流量。我做错了什么？)

A: When a netdev of a physical NIC is initialized, Linux usually
   allocates one RX and TX queue pair per core. So on a 8 core system,
   queue ids 0 to 7 will be allocated, one per core. In the AF_XDP
   bind call or the xsk_socket__create libbpf function call, you
   specify a specific queue id to bind to and it is only the traffic
   towards that queue you are going to get on you socket. So in the
   example above, if you bind to queue 0, you are NOT going to get any
   traffic that is distributed to queues 1 through 7. If you are
   lucky, you will see the traffic, but usually it will end up on one
   of the queues you have not bound to.
   (A：当物理网卡的 netdev 初始化时，Linux 通常为每个核心分配一对 RX 和 TX
   队列。因此在 8 核系统上，将分配队列号 0 到 7，每个核心一个。在 AF_XDP 的
   bind 调用或 libbpf 的 xsk_socket__create 函数调用中，您指定要绑定的特定
   队列号，而您的套接字只会收到发往该队列的流量。所以在上面的例子中，如果您
   绑定到队列 0，您将不会收到任何被分发到队列 1 至 7 的流量。运气好的话您可能
   看到一些流量，但通常它们会落在您没有绑定的某个队列上。)

   There are a number of ways to solve the problem of getting the
   traffic you want to the queue id you bound to. If you want to see
   all the traffic, you can force the netdev to only have 1 queue, queue
   id 0, and then bind to queue 0. You can use ethtool to do this::
   (有多种方法可以把您想要的流量引导到您绑定的队列号上。如果您想看到所有
   流量，可以强制该 netdev 只保留 1 个队列 (队列号 0)，然后绑定到队列 0。
   可以使用 ethtool 来完成：)

     sudo ethtool -L <interface> combined 1

   If you want to only see part of the traffic, you can program the
   NIC through ethtool to filter out your traffic to a single queue id
   that you can bind your XDP socket to. Here is one example in which
   UDP traffic to and from port 4242 are sent to queue 2::
   (如果您只想看到部分流量，可以通过 ethtool 对网卡编程，把您的流量过滤到
   某个单一队列号上，然后将 XDP 套接字绑定到该队列。下面是一个示例，其中
   端口 4242 收发的 UDP 流量被送往队列 2：)

     sudo ethtool -N <interface> rx-flow-hash udp4 fn
     sudo ethtool -N <interface> flow-type udp4 src-port 4242 dst-port \
     4242 action 2

   A number of other ways are possible all up to the capabilities of
   the NIC you have.
   (还有许多其他可行的方式，取决于您所拥有网卡的能力。)

Q: Can I use the XSKMAP to implement a switch between different umems
   in copy mode?
   (Q：在拷贝模式下，我可以使用 XSKMAP 在不同 umem 之间实现交换吗？)

A: The short answer is no, that is not supported at the moment. The
   XSKMAP can only be used to switch traffic coming in on queue id X
   to sockets bound to the same queue id X. The XSKMAP can contain
   sockets bound to different queue ids, for example X and Y, but only
   traffic goming in from queue id Y can be directed to sockets bound
   to the same queue id Y. In zero-copy mode, you should use the
   switch, or other distribution mechanism, in your NIC to direct
   traffic to the correct queue id and socket.
   (A：简短的回答是不能，目前不支持这种做法。XSKMAP 只能用于把从队列号 X
   进入的流量交换到绑定在相同队列号 X 上的套接字。XSKMAP 中可以包含绑定到
   不同队列号 (例如 X 和 Y) 的套接字，但只有从队列号 Y 进入的流量才能被引导到
   绑定在相同队列号 Y 上的套接字。在零拷贝模式下，您应当使用网卡上的交换
   (switch) 功能或其他分发机制，把流量引导到正确的队列号和套接字。)

Q: My packets are sometimes corrupted. What is wrong?
   (Q：我的数据包有时会损坏。出了什么问题？)

A: Care has to be taken not to feed the same buffer in the UMEM into
   more than one ring at the same time. If you for example feed the
   same buffer into the FILL ring and the TX ring at the same time, the
   NIC might receive data into the buffer at the same time it is
   sending it. This will cause some packets to become corrupted. Same
   thing goes for feeding the same buffer into the FILL rings
   belonging to different queue ids or netdevs bound with the
   XDP_SHARED_UMEM flag.
   (A：必须注意，不要把 UMEM 中的同一个缓冲区同时放入多个环。例如，如果您把
   同一个缓冲区同时放入 FILL 环和 TX 环，网卡可能会在发送该缓冲区数据的同时
   又往其中接收数据，这会导致部分数据包损坏。把同一个缓冲区放入属于不同队列号
   或不同 netdev (通过 XDP_SHARED_UMEM 标志绑定) 的 FILL 环也会导致同样的
   问题。)

Credits(致谢)
=============

- Björn Töpel (AF_XDP core)
  (AF_XDP 核心)
- Magnus Karlsson (AF_XDP core)
  (AF_XDP 核心)
- Alexander Duyck
- Alexei Starovoitov
- Daniel Borkmann
- Jesper Dangaard Brouer
- John Fastabend
- Jonathan Corbet (LWN coverage)
  (LWN 报道)
- Michael S. Tsirkin
- Qi Z Zhang
- Willem de Bruijn
