.. SPDX-License-Identifier: GPL-2.0

==========================================
Packet MMAP(Packet MMAP，即数据包内存映射)
==========================================

Abstract(摘要)
==============

This file documents the mmap() facility available with the PACKET
socket interface. This type of sockets is used for
(本文档介绍 PACKET 套接字接口所提供的 mmap() 功能。这类套接字用于以下目的)

i) capture network traffic with utilities like tcpdump,
   (i) 使用 tcpdump 之类的工具捕获网络流量，
ii) transmit network traffic, or any other that needs raw
    access to network interface.
    (ii) 发送网络流量，或用于其他需要对网络接口进行原始访问的场景。

Howto can be found at:
(操作指南可在此处找到：)

    https://web.archive.org/web/20220404160947/https://sites.google.com/site/packetmmap/

Please send your comments to
(请将您的意见发送至：)

    - Ulisses Alonso Camaró <uaca@i.hate.spam.alumni.uv.es>
    - Johann Baudy

Why use PACKET_MMAP(为何使用 PACKET_MMAP)
=========================================

Non PACKET_MMAP capture process (plain AF_PACKET) is very
inefficient. It uses very limited buffers and requires one system call to
capture each packet, it requires two if you want to get packet's timestamp
(like libpcap always does).
(非 PACKET_MMAP 的捕获过程 (普通 AF_PACKET) 效率非常低。它使用的缓冲区极为有限，
每捕获一个数据包都需要一次系统调用；如果想同时获得数据包的时间戳，则需要两次系统
调用 (就像 libpcap 一直做的那样)。)

On the other hand PACKET_MMAP is very efficient. PACKET_MMAP provides a size
configurable circular buffer mapped in user space that can be used to either
send or receive packets. This way reading packets just needs to wait for them,
most of the time there is no need to issue a single system call. Concerning
transmission, multiple packets can be sent through one system call to get the
highest bandwidth. By using a shared buffer between the kernel and the user
also has the benefit of minimizing packet copies.
(另一方面，PACKET_MMAP 则非常高效。PACKET_MMAP 提供一个大小可配置的循环缓冲区，
映射到用户空间，既可用于发送也可用于接收数据包。这样，读取数据包时只需等待数据包
到来，大多数时候甚至无需发出任何系统调用。至于发送，可以通过一次系统调用发送多个
数据包，从而获得最高带宽。在内核与用户之间共享缓冲区还有助于尽量减少数据包的拷贝。)

It's fine to use PACKET_MMAP to improve the performance of the capture and
transmission process, but it isn't everything. At least, if you are capturing
at high speeds (this is relative to the cpu speed), you should check if the
device driver of your network interface card supports some sort of interrupt
load mitigation or (even better) if it supports NAPI, also make sure it is
enabled. For transmission, check the MTU (Maximum Transmission Unit) used and
supported by devices of your network. CPU IRQ pinning of your network interface
card can also be an advantage.
(使用 PACKET_MMAP 来提升捕获与发送过程的性能固然不错，但这并非全部。至少，如果您
在进行高速捕获 (这与 CPU 速度相关)，应当检查网卡驱动程序是否支持某种中断负载缓解
机制，或者 (更好的是) 是否支持 NAPI，并确保已将其启用。对于发送，请检查您的网络中
设备所使用和支持的 MTU (最大传输单元)。将网卡的 CPU IRQ 绑定 (pinning) 到特定
处理器也可能带来好处。)

How to use mmap() to improve capture process(如何使用 mmap() 改进捕获过程)
==========================================================================

From the user standpoint, you should use the higher level libpcap library, which
is a de facto standard, portable across nearly all operating systems
including Win32.
(从用户的角度出发，您应当使用更高层的 libpcap 库。它是事实上的标准，可在包括
Win32 在内的几乎所有操作系统之间移植。)

Packet MMAP support was integrated into libpcap around the time of version 1.3.0;
TPACKET_V3 support was added in version 1.5.0
(Packet MMAP 支持大约在 1.3.0 版本时被集成进 libpcap；TPACKET_V3 支持则在 1.5.0
版本中加入。)

How to use mmap() directly to improve capture process(如何直接使用 mmap() 改进捕获过程)
=======================================================================================

From the system calls stand point, the use of PACKET_MMAP involves
the following process::
(从系统调用的角度来看，使用 PACKET_MMAP 涉及以下流程：)


    [setup]     socket() -------> creation of the capture socket
		setsockopt() ---> allocation of the circular buffer (ring)
				  option: PACKET_RX_RING
		mmap() ---------> mapping of the allocated buffer to the
				  user process

    [capture]   poll() ---------> to wait for incoming packets

    [shutdown]  close() --------> destruction of the capture socket and
				  deallocation of all associated
				  resources.


socket creation and destruction is straight forward, and is done
the same way with or without PACKET_MMAP::
(套接字的创建与销毁非常直接，无论是否使用 PACKET_MMAP，做法都相同：)

 int fd = socket(PF_PACKET, mode, htons(ETH_P_ALL));

where mode is SOCK_RAW for the raw interface were link level
information can be captured or SOCK_DGRAM for the cooked
interface where link level information capture is not
supported and a link level pseudo-header is provided
by the kernel.
(其中，SOCK_RAW 用于原始 (raw) 接口，可以捕获链路层信息；SOCK_DGRAM 用于
cooked 接口，不支持捕获链路层信息，由内核提供一个链路层伪头部。)

The destruction of the socket and all associated resources
is done by a simple call to close(fd).
(销毁套接字及其所有关联资源只需简单地调用一次 close(fd)。)

Similarly as without PACKET_MMAP, it is possible to use one socket
for capture and transmission. This can be done by mapping the
allocated RX and TX buffer ring with a single mmap() call.
See "Mapping and use of the circular buffer (ring)".
(与不使用 PACKET_MMAP 时类似，也可以用同一个套接字同时进行捕获和发送。做法是
通过一次 mmap() 调用将已分配的 RX 和 TX 缓冲区环一并映射。参见 "Mapping and
use of the circular buffer (ring)" 一节。)

Next I will describe PACKET_MMAP settings and its constraints,
also the mapping of the circular buffer in the user process and
the use of this buffer.
(接下来我将介绍 PACKET_MMAP 的设置及其约束，以及循环缓冲区在用户进程中的映射
和该缓冲区的使用方法。)

How to use mmap() directly to improve transmission process(如何直接使用 mmap() 改进发送过程)
============================================================================================

Transmission process is similar to capture as shown below::
(发送过程与捕获过程类似，如下所示：)

    [setup]         socket() -------> creation of the transmission socket
		    setsockopt() ---> allocation of the circular buffer (ring)
				      option: PACKET_TX_RING
		    bind() ---------> bind transmission socket with a network interface
		    mmap() ---------> mapping of the allocated buffer to the
				      user process

    [transmission]  poll() ---------> wait for free packets (optional)
		    send() ---------> send all packets that are set as ready in
				      the ring
				      The flag MSG_DONTWAIT can be used to return
				      before end of transfer.

    [shutdown]      close() --------> destruction of the transmission socket and
				      deallocation of all associated resources.

Socket creation and destruction is also straight forward, and is done
the same way as in capturing described in the previous paragraph::
(套接字的创建与销毁同样非常直接，做法与上一段所述的捕获过程相同：)

 int fd = socket(PF_PACKET, mode, 0);

The protocol can optionally be 0 in case we only want to transmit
via this socket, which avoids an expensive call to packet_rcv().
In this case, you also need to bind(2) the TX_RING with sll_protocol = 0
set. Otherwise, htons(ETH_P_ALL) or any other protocol, for example.
(如果我们只想通过该套接字发送数据，协议可以可选地设为 0，从而避免代价较高的
packet_rcv() 调用。在这种情况下，还需要在 sll_protocol = 0 的设置下对 TX_RING
执行 bind(2)。否则，可使用 htons(ETH_P_ALL) 或任意其他协议。)

Binding the socket to your network interface is mandatory (with zero copy) to
know the header size of frames used in the circular buffer.
(在使用零拷贝时，必须将套接字绑定到您的网络接口，以获知循环缓冲区中所用帧的头部
大小。)

As capture, each frame contains two parts::

    --------------------
    | struct tpacket_hdr | Header. It contains the status of
    |                    | of this frame
    |--------------------|
    | data buffer        |
    .                    .  Data that will be sent over the network interface.
    .                    .
    --------------------

 bind() associates the socket to your network interface thanks to
 sll_ifindex parameter of struct sockaddr_ll.
 (bind() 借助 struct sockaddr_ll 的 sll_ifindex 参数将套接字关联到您的网络
 接口。)

 Initialization example::
 (初始化示例：)

    struct sockaddr_ll my_addr;
    struct ifreq s_ifr;
    ...

    strscpy_pad (s_ifr.ifr_name, "eth0", sizeof(s_ifr.ifr_name));

    /* get interface index of eth0 */
    ioctl(this->socket, SIOCGIFINDEX, &s_ifr);

    /* fill sockaddr_ll struct to prepare binding */
    my_addr.sll_family = AF_PACKET;
    my_addr.sll_protocol = htons(ETH_P_ALL);
    my_addr.sll_ifindex =  s_ifr.ifr_ifindex;

    /* bind socket to eth0 */
    bind(this->socket, (struct sockaddr *)&my_addr, sizeof(struct sockaddr_ll));

 A complete tutorial is available at:
 (完整教程可在此处获取：)
 https://web.archive.org/web/20220404160947/https://sites.google.com/site/packetmmap/

(与捕获时一样，每个帧包含两个部分：)

By default, the user should put data at::
(默认情况下，用户应将数据放在：)

 frame base + TPACKET_HDRLEN - sizeof(struct sockaddr_ll)

So, whatever you choose for the socket mode (SOCK_DGRAM or SOCK_RAW),
the beginning of the user data will be at::
(因此，无论您为套接字模式选择 SOCK_DGRAM 还是 SOCK_RAW，用户数据的起始位置都
将在：)

 frame base + TPACKET_ALIGN(sizeof(struct tpacket_hdr))

If you wish to put user data at a custom offset from the beginning of
the frame (for payload alignment with SOCK_RAW mode for instance) you
can set tp_net (with SOCK_DGRAM) or tp_mac (with SOCK_RAW). In order
to make this work it must be enabled previously with setsockopt()
and the PACKET_TX_HAS_OFF option.
(如果您希望将用户数据放在距帧开头自定义的偏移处 (例如为了在 SOCK_RAW 模式下
对齐负载)，可以设置 tp_net (SOCK_DGRAM 模式) 或 tp_mac (SOCK_RAW 模式)。
要使其生效，必须事先通过 setsockopt() 和 PACKET_TX_HAS_OFF 选项将其启用。)

PACKET_MMAP settings(PACKET_MMAP 设置)
======================================

To setup PACKET_MMAP from user level code is done with a call like
(从用户态代码中通过如下调用来设置 PACKET_MMAP)

 - Capture process::
 (捕获过程：)

     setsockopt(fd, SOL_PACKET, PACKET_RX_RING, (void *) &req, sizeof(req))

 - Transmission process::
 (发送过程：)

     setsockopt(fd, SOL_PACKET, PACKET_TX_RING, (void *) &req, sizeof(req))

The most significant argument in the previous call is the req parameter,
this parameter must to have the following structure::
(前述调用中最关键的参数是 req，该参数必须具有如下结构：)

    struct tpacket_req
    {
	unsigned int    tp_block_size;  /* Minimal size of contiguous block */
	unsigned int    tp_block_nr;    /* Number of blocks */
	unsigned int    tp_frame_size;  /* Size of frame */
	unsigned int    tp_frame_nr;    /* Total number of frames */
    };

This structure is defined in /usr/include/linux/if_packet.h and establishes a
circular buffer (ring) of unswappable memory.
Being mapped in the capture process allows reading the captured frames and
related meta-information like timestamps without requiring a system call.
(该结构定义在 /usr/include/linux/if_packet.h 中，它建立一个由不可换出内存组成的
循环缓冲区 (ring)。将其映射到捕获进程中，使得读取已捕获的帧以及时间戳等相关元
信息时无需发出系统调用。)

Frames are grouped in blocks. Each block is a physically contiguous
region of memory and holds tp_block_size/tp_frame_size frames. The total number
of blocks is tp_block_nr. Note that tp_frame_nr is a redundant parameter because::
(帧被分组存放在块 (block) 中。每个块是一段物理连续的内存区域，可容纳
tp_block_size/tp_frame_size 个帧。块的总数为 tp_block_nr。注意 tp_frame_nr 是
一个冗余参数，因为：)

    frames_per_block = tp_block_size/tp_frame_size

indeed, packet_set_ring checks that the following condition is true::
(实际上，packet_set_ring 会检查以下条件是否成立：)

    frames_per_block * tp_block_nr == tp_frame_nr

Lets see an example, with the following values::
(我们来看一个示例，采用如下数值：)

     tp_block_size= 4096
     tp_frame_size= 2048
     tp_block_nr  = 4
     tp_frame_nr  = 8

we will get the following buffer structure::
(我们将得到如下缓冲区结构：)

	    block #1                 block #2
    +---------+---------+    +---------+---------+
    | frame 1 | frame 2 |    | frame 3 | frame 4 |
    +---------+---------+    +---------+---------+

	    block #3                 block #4
    +---------+---------+    +---------+---------+
    | frame 5 | frame 6 |    | frame 7 | frame 8 |
    +---------+---------+    +---------+---------+

A frame can be of any size with the only condition it can fit in a block. A block
can only hold an integer number of frames, or in other words, a frame cannot
be spawned across two blocks, so there are some details you have to take into
account when choosing the frame_size. See "Mapping and use of the circular
buffer (ring)".
(帧的大小可以是任意值，唯一条件是必须能放入一个块中。一个块只能容纳整数个帧，
换句话说，一个帧不能跨越两个块，因此在选择 frame_size 时需要注意一些细节。参见
"Mapping and use of the circular buffer (ring)" 一节。)

PACKET_MMAP setting constraints(PACKET_MMAP 设置约束)
=====================================================

In kernel versions prior to 2.4.26 (for the 2.4 branch) and 2.6.5 (2.6 branch),
the PACKET_MMAP buffer could hold only 32768 frames in a 32 bit architecture or
16384 in a 64 bit architecture.
(在 2.4.26 (2.4 分支) 和 2.6.5 (2.6 分支) 之前的内核版本中，在 32 位体系结构上
PACKET_MMAP 缓冲区最多只能容纳 32768 个帧，在 64 位体系结构上最多只能容纳
16384 个帧。)

Block size limit(块大小限制)
----------------------------

As stated earlier, each block is a contiguous physical region of memory. These
memory regions are allocated with calls to the __get_free_pages() function. As
the name indicates, this function allocates pages of memory, and the second
argument is "order" or a power of two number of pages, that is
(for PAGE_SIZE == 4096) order=0 ==> 4096 bytes, order=1 ==> 8192 bytes,
order=2 ==> 16384 bytes, etc. The maximum size of a
region allocated by __get_free_pages is determined by the MAX_PAGE_ORDER macro.
More precisely the limit can be calculated as::
(如前所述，每个块都是一段物理连续的内存区域。这些内存区域通过调用
__get_free_pages() 函数来分配。顾名思义，该函数按页分配内存，其第二个参数是
"order"，即以 2 的幂计的页数，也就是说 (当 PAGE_SIZE == 4096 时) order=0 ==>
4096 字节，order=1 ==> 8192 字节，order=2 ==> 16384 字节，依此类推。
__get_free_pages 所能分配区域的最大大小由 MAX_PAGE_ORDER 宏决定。更准确地说，
该限制可以按如下方式计算：)

   PAGE_SIZE << MAX_PAGE_ORDER

   In a i386 architecture PAGE_SIZE is 4096 bytes
   In a 2.4/i386 kernel MAX_PAGE_ORDER is 10
   In a 2.6/i386 kernel MAX_PAGE_ORDER is 11

So get_free_pages can allocate as much as 4MB or 8MB in a 2.4/2.6 kernel
respectively, with an i386 architecture.
(因此在 i386 体系结构上，get_free_pages 在 2.4/2.6 内核中分别最多可以分配 4MB
或 8MB。)

User space programs can include /usr/include/sys/user.h and
/usr/include/linux/mmzone.h to get PAGE_SIZE MAX_PAGE_ORDER declarations.
(用户态程序可以包含 /usr/include/sys/user.h 和 /usr/include/linux/mmzone.h
来获得 PAGE_SIZE 和 MAX_PAGE_ORDER 的声明。)

The pagesize can also be determined dynamically with the getpagesize (2)
system call.
(页大小也可以通过 getpagesize (2) 系统调用动态确定。)

Block number limit(块数量限制)
------------------------------

To understand the constraints of PACKET_MMAP, we have to see the structure
used to hold the pointers to each block.
(要理解 PACKET_MMAP 的约束，我们必须了解用于存放指向各个块的指针的结构。)

Currently, this structure is a dynamically allocated vector with kmalloc
called pg_vec, its size limits the number of blocks that can be allocated::

    +---+---+---+---+
    | x | x | x | x |
    +---+---+---+---+

      |   |   |   |
      |   |   |   v
      |   |   v  block #4
      |   v  block #3
      v  block #2
     block #1

(目前，该结构是用 kmalloc 动态分配的向量，名为 pg_vec，其大小限制了可分配块的
数量：)

kmalloc allocates any number of bytes of physically contiguous memory from
a pool of pre-determined sizes. This pool of memory is maintained by the slab
allocator which is at the end the responsible for doing the allocation and
hence which imposes the maximum memory that kmalloc can allocate.
(kmalloc 从一组预先确定大小的内存池中分配任意字节数的物理连续内存。这个内存池
由 slab 分配器维护，最终由它负责完成分配，因此也由它决定 kmalloc 可分配内存的
上限。)

In a 2.4/2.6 kernel and the i386 architecture, the limit is 131072 bytes. The
predetermined sizes that kmalloc uses can be checked in the "size-<bytes>"
entries of /proc/slabinfo
(在 2.4/2.6 内核和 i386 体系结构上，该上限为 131072 字节。kmalloc 使用的预定
大小可以通过 /proc/slabinfo 中的 "size-<bytes>" 条目查看)

In a 32 bit architecture, pointers are 4 bytes long, so the total number of
pointers to blocks is::
(在 32 位体系结构上，指针长度为 4 字节，因此指向块的指针总数为：)

     131072/4 = 32768 blocks

PACKET_MMAP buffer size calculator(PACKET_MMAP 缓冲区大小计算器)
================================================================

Definitions:
(定义：)

==============  ================================================================
<size-max>      is the maximum size of allocable with kmalloc
		(see /proc/slabinfo)
                (即可用 kmalloc 分配的最大大小，参见 /proc/slabinfo)
<pointer size>  depends on the architecture -- ``sizeof(void *)``
                (取决于体系结构，即 ``sizeof(void *)``)
<page size>     depends on the architecture -- PAGE_SIZE or getpagesize (2)
                (取决于体系结构 -- PAGE_SIZE 或 getpagesize (2))
<max-order>     is the value defined with MAX_PAGE_ORDER
                (即 MAX_PAGE_ORDER 所定义的值)
<frame size>    it's an upper bound of frame's capture size (more on this later)
                (它是帧捕获大小的上界，稍后详述)
==============  ================================================================

from these definitions we will derive::
(根据这些定义我们可以推导出：)

	<block number> = <size-max>/<pointer size>
	<block size> = <pagesize> << <max-order>

so, the max buffer size is::
(因此，最大缓冲区大小为：)

	<block number> * <block size>

and, the number of frames be::
(而帧的数量则为：)

	<block number> * <block size> / <frame size>

Suppose the following parameters, which apply for 2.6 kernel and an
i386 architecture::
(假设采用如下参数，它们适用于 2.6 内核和 i386 体系结构：)

	<size-max> = 131072 bytes
	<pointer size> = 4 bytes
	<pagesize> = 4096 bytes
	<max-order> = 11

and a value for <frame size> of 2048 bytes. These parameters will yield::
(且 <frame size> 取值为 2048 字节。这些参数将得到：)

	<block number> = 131072/4 = 32768 blocks
	<block size> = 4096 << 11 = 8 MiB.

and hence the buffer will have a 262144 MiB size. So it can hold
262144 MiB / 2048 bytes = 134217728 frames
(因此缓冲区大小将达到 262144 MiB。于是它可以容纳 262144 MiB / 2048 字节 =
134217728 个帧)

Actually, this buffer size is not possible with an i386 architecture.
Remember that the memory is allocated in kernel space, in the case of
an i386 kernel's memory size is limited to 1GiB.
(实际上，在 i386 体系结构上不可能达到这样的缓冲区大小。请记住，内存是在内核
空间分配的，对 i386 而言内核内存大小被限制在 1GiB 以内。)

All memory allocations are not freed until the socket is closed. The memory
allocations are done with GFP_KERNEL priority, this basically means that
the allocation can wait and swap other process' memory in order to allocate
the necessary memory, so normally limits can be reached.
(所有内存分配在套接字关闭之前都不会释放。内存分配以 GFP_KERNEL 优先级进行，
这基本上意味着分配过程可以等待，并可将其他进程的内存换出以分配所需的内存，
因此通常可以达到上述上限。)

Other constraints(其他约束)
---------------------------

If you check the source code you will see that what I draw here as a frame
is not only the link level frame. At the beginning of each frame there is a
header called struct tpacket_hdr used in PACKET_MMAP to hold link level's frame
meta information like timestamp. So what we draw here a frame it's really
the following (from include/linux/if_packet.h)::
(如果您查看源代码就会发现，我在这里画出的帧并不只是链路层帧。每个帧的开头都有
一个名为 struct tpacket_hdr 的头部，PACKET_MMAP 用它来存放链路层帧的元信息，
例如时间戳。所以我们在这里画出的帧实际上是如下结构 (摘自
include/linux/if_packet.h)：)

 /*
   Frame structure:

   - Start. Frame must be aligned to TPACKET_ALIGNMENT=16
   - struct tpacket_hdr
   - pad to TPACKET_ALIGNMENT=16
   - struct sockaddr_ll
   - Gap, chosen so that packet data (Start+tp_net) aligns to
     TPACKET_ALIGNMENT=16
   - Start+tp_mac: [ Optional MAC header ]
   - Start+tp_net: Packet data, aligned to TPACKET_ALIGNMENT=16.
   - Pad to align to TPACKET_ALIGNMENT=16
 */

The following are conditions that are checked in packet_set_ring
(以下是 packet_set_ring 中检查的各项条件)

   - tp_block_size must be a multiple of PAGE_SIZE (1)
     (tp_block_size 必须是 PAGE_SIZE 的倍数 (1))
   - tp_frame_size must be greater than TPACKET_HDRLEN (obvious)
     (tp_frame_size 必须大于 TPACKET_HDRLEN (显而易见))
   - tp_frame_size must be a multiple of TPACKET_ALIGNMENT
     (tp_frame_size 必须是 TPACKET_ALIGNMENT 的倍数)
   - tp_frame_nr   must be exactly frames_per_block*tp_block_nr
     (tp_frame_nr   必须恰好等于 frames_per_block*tp_block_nr)

Note that tp_block_size should be chosen to be a power of two or there will
be a waste of memory.
(注意，tp_block_size 应选择为 2 的幂，否则会浪费内存。)

Mapping and use of the circular buffer (ring)(循环缓冲区 (ring) 的映射与使用)
-----------------------------------------------------------------------------

The mapping of the buffer in the user process is done with the conventional
mmap function. Even the circular buffer is compound of several physically
discontiguous blocks of memory, they are contiguous to the user space, hence
just one call to mmap is needed::
(缓冲区在用户进程中的映射使用常规的 mmap 函数完成。即使循环缓冲区由多块物理
上不连续的内存块组成，它们在用户空间中也是连续的，因此只需一次 mmap 调用：)

    mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

If tp_frame_size is a divisor of tp_block_size frames will be
contiguously spaced by tp_frame_size bytes. If not, each
tp_block_size/tp_frame_size frames there will be a gap between
the frames. This is because a frame cannot be spawn across two
blocks.
(如果 tp_frame_size 能整除 tp_block_size，各帧之间将以 tp_frame_size 字节连续
排列。如果不能，则每 tp_block_size/tp_frame_size 个帧之间就会有间隙。这是
因为一个帧不能跨越两个块。)

To use one socket for capture and transmission, the mapping of both the
RX and TX buffer ring has to be done with one call to mmap::
(要用一个套接字同时进行捕获和发送，RX 和 TX 缓冲区环的映射必须通过一次 mmap
调用完成：)

    ...
    setsockopt(fd, SOL_PACKET, PACKET_RX_RING, &foo, sizeof(foo));
    setsockopt(fd, SOL_PACKET, PACKET_TX_RING, &bar, sizeof(bar));
    ...
    rx_ring = mmap(0, size * 2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    tx_ring = rx_ring + size;

RX must be the first as the kernel maps the TX ring memory right
after the RX one.
(必须把 RX 放在前面，因为内核会把 TX 环的内存紧接着映射在 RX 之后。)

At the beginning of each frame there is an status field (see
struct tpacket_hdr). If this field is 0 means that the frame is ready
to be used for the kernel, If not, there is a frame the user can read
and the following flags apply:
(每个帧的开头都有一个状态字段 (参见 struct tpacket_hdr)。如果该字段为 0，
表示该帧已就绪，可供内核使用；如果不为 0，则表示有一个帧可供用户读取，此时
适用以下标志：)

Capture process(捕获过程)
^^^^^^^^^^^^^^^^^^^^^^^^^

From include/linux/if_packet.h::
(摘自 include/linux/if_packet.h：)

     #define TP_STATUS_COPY          (1 << 1)
     #define TP_STATUS_LOSING        (1 << 2)
     #define TP_STATUS_CSUMNOTREADY  (1 << 3)
     #define TP_STATUS_CSUM_VALID    (1 << 7)

======================  =======================================================
TP_STATUS_COPY		This flag indicates that the frame (and associated
			meta information) has been truncated because it's
			larger than tp_frame_size. This packet can be
			read entirely with recvfrom().

			In order to make this work it must to be
			enabled previously with setsockopt() and
			the PACKET_COPY_THRESH option.

			The number of frames that can be buffered to
			be read with recvfrom is limited like a normal socket.
			See the SO_RCVBUF option in the socket (7) man page.
			(该标志表明帧 (及其关联的元信息) 因为大于 tp_frame_size
			而被截断。此数据包可以用 recvfrom() 完整读取。
			要使其生效，必须事先通过 setsockopt() 和
			PACKET_COPY_THRESH 选项将其启用。
			可缓冲以待 recvfrom 读取的帧数量受到与普通套接字相同的
			限制。参见 socket (7) 手册页中的 SO_RCVBUF 选项。)

TP_STATUS_LOSING	indicates there were packet drops from last time
			statistics where checked with getsockopt() and
			the PACKET_STATISTICS option.
			(表示自上次通过 getsockopt() 和 PACKET_STATISTICS 选项
			检查统计信息以来发生过丢包。)

TP_STATUS_CSUMNOTREADY	currently it's used for outgoing IP packets which
			its checksum will be done in hardware. So while
			reading the packet we should not try to check the
			checksum.
			(目前它用于即将发出的 IP 数据包，这些数据包的校验和将在
			硬件中完成。因此读取该数据包时不应尝试检查其校验和。)

TP_STATUS_CSUM_VALID	This flag indicates that at least the transport
			header checksum of the packet has been already
			validated on the kernel side. If the flag is not set
			then we are free to check the checksum by ourselves
			provided that TP_STATUS_CSUMNOTREADY is also not set.
			(该标志表明至少该数据包的传输层头部校验和已经在内核侧
			通过验证。如果该标志未置位，且 TP_STATUS_CSUMNOTREADY
			也未置位，那么我们可以自行校验校验和。)
======================  =======================================================

for convenience there are also the following defines::
(为方便起见，还有以下定义：)

     #define TP_STATUS_KERNEL        0
     #define TP_STATUS_USER          1

The kernel initializes all frames to TP_STATUS_KERNEL, when the kernel
receives a packet it puts in the buffer and updates the status with
at least the TP_STATUS_USER flag. Then the user can read the packet,
once the packet is read the user must zero the status field, so the kernel
can use again that frame buffer.
(内核将所有帧初始化为 TP_STATUS_KERNEL。当内核收到一个数据包时，它将其放入
缓冲区，并把状态更新为至少带有 TP_STATUS_USER 标志。然后用户就可以读取该
数据包；读取完毕后，用户必须将状态字段清零，以便内核可以再次使用该帧缓冲区。)

The user can use poll (any other variant should apply too) to check if new
packets are in the ring::
(用户可以使用 poll (其他任何变体应该也适用) 来检查环中是否有新数据包：)

    struct pollfd pfd;

    pfd.fd = fd;
    pfd.revents = 0;
    pfd.events = POLLIN|POLLRDNORM|POLLERR;

    if (status == TP_STATUS_KERNEL)
	retval = poll(&pfd, 1, timeout);

It doesn't incur in a race condition to first check the status value and
then poll for frames.
(先检查状态值再 poll 等待帧，并不会导致竞争条件。)

Transmission process(传输过程)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Those defines are also used for transmission::
(这些定义同样用于发送过程：)

     #define TP_STATUS_AVAILABLE        0 // Frame is available
     #define TP_STATUS_SEND_REQUEST     1 // Frame will be sent on next send()
     #define TP_STATUS_SENDING          2 // Frame is currently in transmission
     #define TP_STATUS_WRONG_FORMAT     4 // Frame format is not correct

First, the kernel initializes all frames to TP_STATUS_AVAILABLE. To send a
packet, the user fills a data buffer of an available frame, sets tp_len to
current data buffer size and sets its status field to TP_STATUS_SEND_REQUEST.
This can be done on multiple frames. Once the user is ready to transmit, it
calls send(). Then all buffers with status equal to TP_STATUS_SEND_REQUEST are
forwarded to the network device. The kernel updates each status of sent
frames with TP_STATUS_SENDING until the end of transfer.
(首先，内核将所有帧初始化为 TP_STATUS_AVAILABLE。要发送一个数据包，用户先填充
某个可用帧的数据缓冲区，将 tp_len 设为当前数据缓冲区的大小，并将其状态字段设为
TP_STATUS_SEND_REQUEST。这一操作可以在多个帧上进行。当用户准备好发送时，调用
send()。随后，所有状态等于 TP_STATUS_SEND_REQUEST 的缓冲区都会被转发给网络
设备。内核将每个已发送帧的状态更新为 TP_STATUS_SENDING，直到传输结束。)

At the end of each transfer, buffer status returns to TP_STATUS_AVAILABLE.
(每次传输结束时，缓冲区状态恢复为 TP_STATUS_AVAILABLE。)

::

    header->tp_len = in_i_size;
    header->tp_status = TP_STATUS_SEND_REQUEST;
    retval = send(this->socket, NULL, 0, 0);

The user can also use poll() to check if a buffer is available:
(用户也可以使用 poll() 来检查缓冲区是否可用：)

(status == TP_STATUS_SENDING)

::

    struct pollfd pfd;
    pfd.fd = fd;
    pfd.revents = 0;
    pfd.events = POLLOUT;
    retval = poll(&pfd, 1, timeout);

What TPACKET versions are available and when to use them?(有哪些 TPACKET 版本可用？何时使用它们？)
==================================================================================================

::

 int val = tpacket_version;
 setsockopt(fd, SOL_PACKET, PACKET_VERSION, &val, sizeof(val));
 getsockopt(fd, SOL_PACKET, PACKET_VERSION, &val, sizeof(val));

where 'tpacket_version' can be TPACKET_V1 (default), TPACKET_V2, TPACKET_V3.
(其中 'tpacket_version' 可以是 TPACKET_V1 (默认)、TPACKET_V2、TPACKET_V3。)

TPACKET_V1:
(TPACKET_V1：)

	- Default if not otherwise specified by setsockopt(2)
	  (若未通过 setsockopt(2) 另行指定，则为默认版本)
	- RX_RING, TX_RING available
	  (支持 RX_RING、TX_RING)

TPACKET_V1 --> TPACKET_V2:
(TPACKET_V1 --> TPACKET_V2：)

	- Made 64 bit clean due to unsigned long usage in TPACKET_V1
	  structures, thus this also works on 64 bit kernel with 32 bit
	  userspace and the like
	  (由于 TPACKET_V1 结构中使用了 unsigned long，本次修改使其做到 64 位
	  安全，因此同样适用于 64 位内核搭配 32 位用户态等组合)
	- Timestamp resolution in nanoseconds instead of microseconds
	  (时间戳分辨率由微秒改为纳秒)
	- RX_RING, TX_RING available
	  (支持 RX_RING、TX_RING)
	- VLAN metadata information available for packets
	  (TP_STATUS_VLAN_VALID, TP_STATUS_VLAN_TPID_VALID),
	  in the tpacket2_hdr structure:
	  (数据包可携带 VLAN 元数据信息 (TP_STATUS_VLAN_VALID、
	  TP_STATUS_VLAN_TPID_VALID)，保存在 tpacket2_hdr 结构中：)

		- TP_STATUS_VLAN_VALID bit being set into the tp_status field indicates
		  that the tp_vlan_tci field has valid VLAN TCI value
		  (tp_status 字段中 TP_STATUS_VLAN_VALID 位被置位，表示
		  tp_vlan_tci 字段具有有效的 VLAN TCI 值)
		- TP_STATUS_VLAN_TPID_VALID bit being set into the tp_status field
		  indicates that the tp_vlan_tpid field has valid VLAN TPID value
		  (tp_status 字段中 TP_STATUS_VLAN_TPID_VALID 位被置位，表示
		  tp_vlan_tpid 字段具有有效的 VLAN TPID 值)

	- How to switch to TPACKET_V2:
	  (如何切换到 TPACKET_V2：)

		1. Replace struct tpacket_hdr by struct tpacket2_hdr
		   (用 struct tpacket2_hdr 替换 struct tpacket_hdr)
		2. Query header len and save
		   (查询头部长度并保存)
		3. Set protocol version to 2, set up ring as usual
		   (将协议版本设为 2，并照常设置环)
		4. For getting the sockaddr_ll,
		   use ``(void *)hdr + TPACKET_ALIGN(hdrlen)`` instead of
		   ``(void *)hdr + TPACKET_ALIGN(sizeof(struct tpacket_hdr))``
		   (要获取 sockaddr_ll，使用 ``(void *)hdr +
		   TPACKET_ALIGN(hdrlen)`` 而不是 ``(void *)hdr +
		   TPACKET_ALIGN(sizeof(struct tpacket_hdr))``)

TPACKET_V2 --> TPACKET_V3:
(TPACKET_V2 --> TPACKET_V3：)

	- Flexible buffer implementation for RX_RING:
	  (RX_RING 的灵活缓冲区实现：)

		1. Blocks can be configured with non-static frame-size
		   (块可以配置为非固定的帧大小)
		2. Read/poll is at a block-level (as opposed to packet-level)
		   (读取/poll 以块为级别进行 (而不是以数据包为级别))
		3. Added poll timeout to avoid indefinite user-space wait
		   on idle links
		   (增加了 poll 超时，以避免用户态在空闲链路上无限期等待)
		4. Added user-configurable knobs:
		   (增加了用户可配置的开关：)

			4.1 block::timeout
			4.2 tpkt_hdr::sk_rxhash

	- RX Hash data available in user space
	  (用户态可获取 RX 哈希数据)
	- TX_RING semantics are conceptually similar to TPACKET_V2;
	  use tpacket3_hdr instead of tpacket2_hdr, and TPACKET3_HDRLEN
	  instead of TPACKET2_HDRLEN. In the current implementation,
	  the tp_next_offset field in the tpacket3_hdr MUST be set to
	  zero, indicating that the ring does not hold variable sized frames.
	  Packets with non-zero values of tp_next_offset will be dropped.
	  (TX_RING 的语义在概念上与 TPACKET_V2 类似；使用 tpacket3_hdr 代替
	  tpacket2_hdr，并用 TPACKET3_HDRLEN 代替 TPACKET2_HDRLEN。在当前实现中，
	  tpacket3_hdr 中的 tp_next_offset 字段必须设置为 0，表示该环不保存
	  可变大小的帧。tp_next_offset 值非零的数据包将被丢弃。)

AF_PACKET fanout mode(AF_PACKET fanout 模式)
============================================

In the AF_PACKET fanout mode, packet reception can be load balanced among
processes. This also works in combination with mmap(2) on packet sockets.
(在 AF_PACKET fanout 模式下，数据包的接收可以在多个进程之间做负载均衡。这
也可以与包套接字上的 mmap(2) 结合使用。)

Currently implemented fanout policies are:
(目前已实现的 fanout 策略有：)

  - PACKET_FANOUT_HASH: schedule to socket by skb's packet hash
    (按 skb 的数据包哈希调度到套接字)
  - PACKET_FANOUT_LB: schedule to socket by round-robin
    (以轮询 (round-robin) 方式调度到套接字)
  - PACKET_FANOUT_CPU: schedule to socket by CPU packet arrives on
    (按数据包到达时所在的 CPU 调度到套接字)
  - PACKET_FANOUT_RND: schedule to socket by random selection
    (随机选择并调度到套接字)
  - PACKET_FANOUT_ROLLOVER: if one socket is full, rollover to another
    (如果一个套接字已满，则滚动切换到另一个)
  - PACKET_FANOUT_QM: schedule to socket by skbs recorded queue_mapping
    (按 skb 记录的 queue_mapping 调度到套接字)

Minimal example code by David S. Miller (try things like "./test eth0 hash",
"./test eth0 lb", etc.)::
(以下是大卫·S·米勒 (David S. Miller) 提供的最小示例代码 (可以尝试
"./test eth0 hash"、"./test eth0 lb" 等用法)：)

    #include <stddef.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>

    #include <sys/types.h>
    #include <sys/wait.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>

    #include <unistd.h>

    #include <linux/if_ether.h>
    #include <linux/if_packet.h>

    #include <net/if.h>

    static const char *device_name;
    static int fanout_type;
    static int fanout_id;

    #ifndef PACKET_FANOUT
    # define PACKET_FANOUT			18
    # define PACKET_FANOUT_HASH		0
    # define PACKET_FANOUT_LB		1
    #endif

    static int setup_socket(void)
    {
	    int err, fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
	    struct sockaddr_ll ll;
	    struct ifreq ifr;
	    int fanout_arg;

	    if (fd < 0) {
		    perror("socket");
		    return EXIT_FAILURE;
	    }

	    memset(&ifr, 0, sizeof(ifr));
	    strcpy(ifr.ifr_name, device_name);
	    err = ioctl(fd, SIOCGIFINDEX, &ifr);
	    if (err < 0) {
		    perror("SIOCGIFINDEX");
		    return EXIT_FAILURE;
	    }

	    memset(&ll, 0, sizeof(ll));
	    ll.sll_family = AF_PACKET;
	    ll.sll_ifindex = ifr.ifr_ifindex;
	    err = bind(fd, (struct sockaddr *) &ll, sizeof(ll));
	    if (err < 0) {
		    perror("bind");
		    return EXIT_FAILURE;
	    }

	    fanout_arg = (fanout_id | (fanout_type << 16));
	    err = setsockopt(fd, SOL_PACKET, PACKET_FANOUT,
			    &fanout_arg, sizeof(fanout_arg));
	    if (err) {
		    perror("setsockopt");
		    return EXIT_FAILURE;
	    }

	    return fd;
    }

    static void fanout_thread(void)
    {
	    int fd = setup_socket();
	    int limit = 10000;

	    if (fd < 0)
		    exit(fd);

	    while (limit-- > 0) {
		    char buf[1600];
		    int err;

		    err = read(fd, buf, sizeof(buf));
		    if (err < 0) {
			    perror("read");
			    exit(EXIT_FAILURE);
		    }
		    if ((limit % 10) == 0)
			    fprintf(stdout, "(%d) \n", getpid());
	    }

	    fprintf(stdout, "%d: Received 10000 packets\n", getpid());

	    close(fd);
	    exit(0);
    }

    int main(int argc, char **argp)
    {
	    int fd, err;
	    int i;

	    if (argc != 3) {
		    fprintf(stderr, "Usage: %s INTERFACE {hash|lb}\n", argp[0]);
		    return EXIT_FAILURE;
	    }

	    if (!strcmp(argp[2], "hash"))
		    fanout_type = PACKET_FANOUT_HASH;
	    else if (!strcmp(argp[2], "lb"))
		    fanout_type = PACKET_FANOUT_LB;
	    else {
		    fprintf(stderr, "Unknown fanout type [%s]\n", argp[2]);
		    exit(EXIT_FAILURE);
	    }

	    device_name = argp[1];
	    fanout_id = getpid() & 0xffff;

	    for (i = 0; i < 4; i++) {
		    pid_t pid = fork();

		    switch (pid) {
		    case 0:
			    fanout_thread();

		    case -1:
			    perror("fork");
			    exit(EXIT_FAILURE);
		    }
	    }

	    for (i = 0; i < 4; i++) {
		    int status;

		    wait(&status);
	    }

	    return 0;
    }

AF_PACKET TPACKET_V3 example(AF_PACKET TPACKET_V3 示例)
=======================================================

AF_PACKET's TPACKET_V3 ring buffer can be configured to use non-static frame
sizes by doing its own memory management. It is based on blocks where polling
works on a per block basis instead of per ring as in TPACKET_V2 and predecessor.
(AF_PACKET 的 TPACKET_V3 环形缓冲区可以通过自行管理内存来配置为使用非固定的
帧大小。它基于块 (block)，poll 以块为单位进行，而不像 TPACKET_V2 及更早版本
那样以整个环为单位。)

It is said that TPACKET_V3 brings the following benefits:
(据称 TPACKET_V3 带来以下好处：)

 * ~15% - 20% reduction in CPU-usage
   (CPU 使用率降低约 15% - 20%)
 * ~20% increase in packet capture rate
   (数据包捕获速率提升约 20%)
 * ~2x increase in packet density
   (数据包密度提升约 2 倍)
 * Port aggregation analysis
   (端口聚合分析)
 * Non static frame size to capture entire packet payload
   (帧大小非固定，可捕获完整的数据包负载)

So it seems to be a good candidate to be used with packet fanout.
(因此它似乎是配合 packet fanout 使用的一个不错的候选。)

Minimal example code by Daniel Borkmann based on Chetan Loke's lolpcap (compile
it with gcc -Wall -O2 blob.c, and try things like "./a.out eth0", etc.)::
(以下是 Daniel Borkmann 基于 Chetan Loke 的 lolpcap 编写的最小示例代码
(用 gcc -Wall -O2 blob.c 编译，然后尝试 "./a.out eth0" 等用法)：)

    /* Written from scratch, but kernel-to-user space API usage
    * dissected from lolpcap:
    *  Copyright 2011, Chetan Loke <loke.chetan@gmail.com>
    *  License: GPL, version 2.0
    */

    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <string.h>
    #include <assert.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <poll.h>
    #include <unistd.h>
    #include <signal.h>
    #include <inttypes.h>
    #include <sys/socket.h>
    #include <sys/mman.h>
    #include <linux/if_packet.h>
    #include <linux/if_ether.h>
    #include <linux/ip.h>

    #ifndef likely
    # define likely(x)		__builtin_expect(!!(x), 1)
    #endif
    #ifndef unlikely
    # define unlikely(x)		__builtin_expect(!!(x), 0)
    #endif

    struct block_desc {
	    uint32_t version;
	    uint32_t offset_to_priv;
	    struct tpacket_hdr_v1 h1;
    };

    struct ring {
	    struct iovec *rd;
	    uint8_t *map;
	    struct tpacket_req3 req;
    };

    static unsigned long packets_total = 0, bytes_total = 0;
    static sig_atomic_t sigint = 0;

    static void sighandler(int num)
    {
	    sigint = 1;
    }

    static int setup_socket(struct ring *ring, char *netdev)
    {
	    int err, i, fd, v = TPACKET_V3;
	    struct sockaddr_ll ll;
	    unsigned int blocksiz = 1 << 22, framesiz = 1 << 11;
	    unsigned int blocknum = 64;

	    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	    if (fd < 0) {
		    perror("socket");
		    exit(1);
	    }

	    err = setsockopt(fd, SOL_PACKET, PACKET_VERSION, &v, sizeof(v));
	    if (err < 0) {
		    perror("setsockopt");
		    exit(1);
	    }

	    memset(&ring->req, 0, sizeof(ring->req));
	    ring->req.tp_block_size = blocksiz;
	    ring->req.tp_frame_size = framesiz;
	    ring->req.tp_block_nr = blocknum;
	    ring->req.tp_frame_nr = (blocksiz * blocknum) / framesiz;
	    ring->req.tp_retire_blk_tov = 60;
	    ring->req.tp_feature_req_word = TP_FT_REQ_FILL_RXHASH;

	    err = setsockopt(fd, SOL_PACKET, PACKET_RX_RING, &ring->req,
			    sizeof(ring->req));
	    if (err < 0) {
		    perror("setsockopt");
		    exit(1);
	    }

	    ring->map = mmap(NULL, ring->req.tp_block_size * ring->req.tp_block_nr,
			    PROT_READ | PROT_WRITE, MAP_SHARED | MAP_LOCKED, fd, 0);
	    if (ring->map == MAP_FAILED) {
		    perror("mmap");
		    exit(1);
	    }

	    ring->rd = malloc(ring->req.tp_block_nr * sizeof(*ring->rd));
	    assert(ring->rd);
	    for (i = 0; i < ring->req.tp_block_nr; ++i) {
		    ring->rd[i].iov_base = ring->map + (i * ring->req.tp_block_size);
		    ring->rd[i].iov_len = ring->req.tp_block_size;
	    }

	    memset(&ll, 0, sizeof(ll));
	    ll.sll_family = PF_PACKET;
	    ll.sll_protocol = htons(ETH_P_ALL);
	    ll.sll_ifindex = if_nametoindex(netdev);
	    ll.sll_hatype = 0;
	    ll.sll_pkttype = 0;
	    ll.sll_halen = 0;

	    err = bind(fd, (struct sockaddr *) &ll, sizeof(ll));
	    if (err < 0) {
		    perror("bind");
		    exit(1);
	    }

	    return fd;
    }

    static void display(struct tpacket3_hdr *ppd)
    {
	    struct ethhdr *eth = (struct ethhdr *) ((uint8_t *) ppd + ppd->tp_mac);
	    struct iphdr *ip = (struct iphdr *) ((uint8_t *) eth + ETH_HLEN);

	    if (eth->h_proto == htons(ETH_P_IP)) {
		    struct sockaddr_in ss, sd;
		    char sbuff[NI_MAXHOST], dbuff[NI_MAXHOST];

		    memset(&ss, 0, sizeof(ss));
		    ss.sin_family = PF_INET;
		    ss.sin_addr.s_addr = ip->saddr;
		    getnameinfo((struct sockaddr *) &ss, sizeof(ss),
				sbuff, sizeof(sbuff), NULL, 0, NI_NUMERICHOST);

		    memset(&sd, 0, sizeof(sd));
		    sd.sin_family = PF_INET;
		    sd.sin_addr.s_addr = ip->daddr;
		    getnameinfo((struct sockaddr *) &sd, sizeof(sd),
				dbuff, sizeof(dbuff), NULL, 0, NI_NUMERICHOST);

		    printf("%s -> %s, ", sbuff, dbuff);
	    }

	    printf("rxhash: 0x%x\n", ppd->hv1.tp_rxhash);
    }

    static void walk_block(struct block_desc *pbd, const int block_num)
    {
	    int num_pkts = pbd->h1.num_pkts, i;
	    unsigned long bytes = 0;
	    struct tpacket3_hdr *ppd;

	    ppd = (struct tpacket3_hdr *) ((uint8_t *) pbd +
					pbd->h1.offset_to_first_pkt);
	    for (i = 0; i < num_pkts; ++i) {
		    bytes += ppd->tp_snaplen;
		    display(ppd);

		    ppd = (struct tpacket3_hdr *) ((uint8_t *) ppd +
						ppd->tp_next_offset);
	    }

	    packets_total += num_pkts;
	    bytes_total += bytes;
    }

    static void flush_block(struct block_desc *pbd)
    {
	    pbd->h1.block_status = TP_STATUS_KERNEL;
    }

    static void teardown_socket(struct ring *ring, int fd)
    {
	    munmap(ring->map, ring->req.tp_block_size * ring->req.tp_block_nr);
	    free(ring->rd);
	    close(fd);
    }

    int main(int argc, char **argp)
    {
	    int fd, err;
	    socklen_t len;
	    struct ring ring;
	    struct pollfd pfd;
	    unsigned int block_num = 0, blocks = 64;
	    struct block_desc *pbd;
	    struct tpacket_stats_v3 stats;

	    if (argc != 2) {
		    fprintf(stderr, "Usage: %s INTERFACE\n", argp[0]);
		    return EXIT_FAILURE;
	    }

	    signal(SIGINT, sighandler);

	    memset(&ring, 0, sizeof(ring));
	    fd = setup_socket(&ring, argp[argc - 1]);
	    assert(fd > 0);

	    memset(&pfd, 0, sizeof(pfd));
	    pfd.fd = fd;
	    pfd.events = POLLIN | POLLERR;
	    pfd.revents = 0;

	    while (likely(!sigint)) {
		    pbd = (struct block_desc *) ring.rd[block_num].iov_base;

		    if ((pbd->h1.block_status & TP_STATUS_USER) == 0) {
			    poll(&pfd, 1, -1);
			    continue;
		    }

		    walk_block(pbd, block_num);
		    flush_block(pbd);
		    block_num = (block_num + 1) % blocks;
	    }

	    len = sizeof(stats);
	    err = getsockopt(fd, SOL_PACKET, PACKET_STATISTICS, &stats, &len);
	    if (err < 0) {
		    perror("getsockopt");
		    exit(1);
	    }

	    fflush(stdout);
	    printf("\nReceived %u packets, %lu bytes, %u dropped, freeze_q_cnt: %u\n",
		stats.tp_packets, bytes_total, stats.tp_drops,
		stats.tp_freeze_q_cnt);

	    teardown_socket(&ring, fd);
	    return 0;
    }

PACKET_QDISC_BYPASS(PACKET_QDISC_BYPASS，绕过排队规则)
======================================================

If there is a requirement to load the network with many packets in a similar
fashion as pktgen does, you might set the following option after socket
creation::
(如果需要像 pktgen 那样用大量数据包对网络加压，可以在创建套接字之后设置以下
选项：)

    int one = 1;
    setsockopt(fd, SOL_PACKET, PACKET_QDISC_BYPASS, &one, sizeof(one));

This has the side-effect, that packets sent through PF_PACKET will bypass the
kernel's qdisc layer and are forcedly pushed to the driver directly. Meaning,
packet are not buffered, tc disciplines are ignored, increased loss can occur
and such packets are also not visible to other PF_PACKET sockets anymore. So,
you have been warned; generally, this can be useful for stress testing various
components of a system.
(这会带来一个副作用：通过 PF_PACKET 发送的数据包将绕过内核的 qdisc 层，被强制
直接推送到驱动程序。这意味着数据包不会被缓冲，tc 排队规则会被忽略，丢包可能
增加，而且这些数据包对其他 PF_PACKET 套接字也不再可见。所以，请自行斟酌；
通常，这对对系统各组件进行压力测试很有用。)

On default, PACKET_QDISC_BYPASS is disabled and needs to be explicitly enabled
on PF_PACKET sockets.
(默认情况下，PACKET_QDISC_BYPASS 是禁用的，需要在 PF_PACKET 套接字上显式启用。)

PACKET_TIMESTAMP(PACKET_TIMESTAMP，数据包时间戳)
================================================

The PACKET_TIMESTAMP setting determines the source of the timestamp in
the packet meta information for mmap(2)ed RX_RING and TX_RINGs.  If your
NIC is capable of timestamping packets in hardware, you can request those
hardware timestamps to be used. Note: you may need to enable the generation
of hardware timestamps with SIOCSHWTSTAMP (see related information from
Documentation/networking/timestamping.rst).
(PACKET_TIMESTAMP 设置决定 mmap(2) 映射的 RX_RING 和 TX_RING 的数据包元信息中
时间戳的来源。如果您的网卡 (NIC) 支持在硬件中对数据包打时间戳，您可以请求
使用这些硬件时间戳。注意：您可能需要通过 SIOCSHWTSTAMP 启用硬件时间戳的生成
(相关信息参见 Documentation/networking/timestamping.rst)。)

PACKET_TIMESTAMP accepts the same integer bit field as SO_TIMESTAMPING::
(PACKET_TIMESTAMP 接受与 SO_TIMESTAMPING 相同的整数位域：)

    int req = SOF_TIMESTAMPING_RAW_HARDWARE;
    setsockopt(fd, SOL_PACKET, PACKET_TIMESTAMP, (void *) &req, sizeof(req))

For the mmap(2)ed ring buffers, such timestamps are stored in the
``tpacket{,2,3}_hdr`` structure's tp_sec and ``tp_{n,u}sec`` members.
To determine what kind of timestamp has been reported, the tp_status field
is binary or'ed with the following possible bits ...
(对于 mmap(2) 映射的环形缓冲区，这些时间戳保存在 ``tpacket{,2,3}_hdr`` 结构的
tp_sec 和 ``tp_{n,u}sec`` 成员中。要确定所报告的是哪种时间戳，tp_status 字段
会与以下可能的位进行二进制或运算……)

::

    TP_STATUS_TS_RAW_HARDWARE
    TP_STATUS_TS_SOFTWARE

... that are equivalent to its ``SOF_TIMESTAMPING_*`` counterparts. For the
RX_RING, if neither is set (i.e. PACKET_TIMESTAMP is not set), then a
software fallback was invoked *within* PF_PACKET's processing code (less
precise).
(……它们等价于 ``SOF_TIMESTAMPING_*`` 中对应的标志。对于 RX_RING，如果两者都
未置位 (即未设置 PACKET_TIMESTAMP)，则表示在 PF_PACKET 的处理代码*内部*调用了
软件回退 (精度较低)。)

Getting timestamps for the TX_RING works as follows: i) fill the ring frames,
ii) call sendto() e.g. in blocking mode, iii) wait for status of relevant
frames to be updated resp. the frame handed over to the application, iv) walk
through the frames to pick up the individual hw/sw timestamps.
(为 TX_RING 获取时间戳的流程如下：i) 填充环中的帧，ii) 调用 sendto()，例如以
阻塞模式，iii) 等待相关帧的状态被更新，或帧被交还给应用程序，iv) 遍历各帧，
取出各自的硬件/软件时间戳。)

Only (!) if transmit timestamping is enabled, then these bits are combined
with binary | with TP_STATUS_AVAILABLE, so you must check for that in your
application (e.g. !(tp_status & (TP_STATUS_SEND_REQUEST | TP_STATUS_SENDING))
in a first step to see if the frame belongs to the application, and then
one can extract the type of timestamp in a second step from tp_status)!
(只有当 (!) 启用了发送时间戳时，这些位才会以二进制 | 与 TP_STATUS_AVAILABLE
组合在一起，因此应用程序中必须对此进行检查 (例如第一步先用
!(tp_status & (TP_STATUS_SEND_REQUEST | TP_STATUS_SENDING)) 判断该帧是否属于
应用程序，然后第二步再从 tp_status 中提取时间戳的类型)！)

If you don't care about them, thus having it disabled, checking for
TP_STATUS_AVAILABLE resp. TP_STATUS_WRONG_FORMAT is sufficient. If in the
TX_RING part only TP_STATUS_AVAILABLE is set, then the tp_sec and tp_{n,u}sec
members do not contain a valid value. For TX_RINGs, by default no timestamp
is generated!
(如果您不关心它们 (即保持禁用)，那么只需检查 TP_STATUS_AVAILABLE 或
TP_STATUS_WRONG_FORMAT 即可。如果 TX_RING 部分只置位了 TP_STATUS_AVAILABLE，
则 tp_sec 和 tp_{n,u}sec 成员不包含有效值。对于 TX_RING，默认情况下不生成
时间戳！)

See include/linux/net_tstamp.h and Documentation/networking/timestamping.rst
for more information on hardware timestamps.
(有关硬件时间戳的更多信息，参见 include/linux/net_tstamp.h 和
Documentation/networking/timestamping.rst。)

Miscellaneous bits(杂项)
========================

- Packet sockets work well together with Linux socket filters, thus you also
  might want to have a look at Documentation/networking/filter.rst
  (包套接字与 Linux 套接字过滤器配合得很好，因此您可能还想看看
  Documentation/networking/filter.rst)

THANKS(致谢)
============

   Jesse Brandeburg, for fixing my grammathical/spelling errors
   (Jesse Brandeburg，感谢他修正我的语法/拼写错误)
