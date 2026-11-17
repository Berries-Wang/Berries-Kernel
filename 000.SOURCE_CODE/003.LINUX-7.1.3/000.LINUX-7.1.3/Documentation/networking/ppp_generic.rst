.. SPDX-License-Identifier: GPL-2.0

========================================
PPP Generic Driver and Channel Interface
========================================

(PPP 通用驱动与通道接口)

			   Paul Mackerras
			   (作者: Paul Mackerras)
			   paulus@samba.org
			   (电子邮件: paulus@samba.org)

			      7 Feb 2002
			      (2002年2月7日)

The generic PPP driver in linux-2.4 provides an implementation of the
functionality which is of use in any PPP implementation, including:
(linux-2.4 中的通用 PPP 驱动实现了对任何 PPP 实现都有用的功能，包括：)

* the network interface unit (ppp0 etc.)
  (网络接口单元（ppp0 等）)
* the interface to the networking code
  (与网络代码之间的接口)
* PPP multilink: splitting datagrams between multiple links, and
  ordering and combining received fragments
  (PPP 多链路：把数据报拆分到多条链路上，并对接收到的分片进行排序与重组)
* the interface to pppd, via a /dev/ppp character device
  (通过 /dev/ppp 字符设备提供的与 pppd 的接口)
* packet compression and decompression
  (数据包压缩与解压)
* TCP/IP header compression and decompression
  (TCP/IP 头压缩与解压)
* detecting network traffic for demand dialling and for idle timeouts
  (为按需拨号和空闲超时检测网络流量)
* simple packet filtering
  (简单的数据包过滤)

For sending and receiving PPP frames, the generic PPP driver calls on
the services of PPP ``channels``.  A PPP channel encapsulates a
mechanism for transporting PPP frames from one machine to another.  A
PPP channel implementation can be arbitrarily complex internally but
has a very simple interface with the generic PPP code: it merely has
to be able to send PPP frames, receive PPP frames, and optionally
handle ioctl requests.  Currently there are PPP channel
implementations for asynchronous serial ports, synchronous serial
ports, and for PPP over ethernet.
(在发送和接收 PPP 帧时，通用 PPP 驱动会借助 PPP ``通道`` 的服务。一个 PPP 通道
封装了把 PPP 帧从一台机器传输到另一台机器的机制。PPP 通道实现在内部可以任意复
杂，但与通用 PPP 代码之间的接口非常简单：它只需要能够发送 PPP 帧、接收 PPP 帧，
并可选地处理 ioctl 请求。目前已有针对异步串口、同步串口以及以太网上的 PPP
（PPP over ethernet）的 PPP 通道实现。)

This architecture makes it possible to implement PPP multilink in a
natural and straightforward way, by allowing more than one channel to
be linked to each ppp network interface unit.  The generic layer is
responsible for splitting datagrams on transmit and recombining them
on receive.
(这种架构让 PPP 多链路能够以自然、直接的方式实现：允许多于一个的通道连接到每个
ppp 网络接口单元。通用层负责在发送时拆分数据报，并在接收时将其重新组合。)


PPP channel API
(PPP 通道 API)
---------------

See include/linux/ppp_channel.h for the declaration of the types and
functions used to communicate between the generic PPP layer and PPP
channels.
(通用 PPP 层与 PPP 通道之间通信所用的类型和函数的声明见
include/linux/ppp_channel.h。)

Each channel has to provide two functions to the generic PPP layer,
via the ppp_channel.ops pointer:
(每个通道必须通过 ppp_channel.ops 指针向通用 PPP 层提供两个函数：)

* start_xmit() is called by the generic layer when it has a frame to
  send.  The channel has the option of rejecting the frame for
  flow-control reasons.  In this case, start_xmit() should return 0
  and the channel should call the ppp_output_wakeup() function at a
  later time when it can accept frames again, and the generic layer
  will then attempt to retransmit the rejected frame(s).  If the frame
  is accepted, the start_xmit() function should return 1.
  (当通用层有帧要发送时会调用 start_xmit()。通道可以出于流控原因拒绝该帧。这种
  情况下，start_xmit() 应返回 0，并且通道应在其能够再次接受帧之后的某个时刻调用
  ppp_output_wakeup() 函数，届时通用层会尝试重传被拒绝的帧。如果接受该帧，
  start_xmit() 函数应返回 1。)

* ioctl() provides an interface which can be used by a user-space
  program to control aspects of the channel's behaviour.  This
  procedure will be called when a user-space program does an ioctl
  system call on an instance of /dev/ppp which is bound to the
  channel.  (Usually it would only be pppd which would do this.)
  (ioctl() 提供一个接口，用户空间程序可以用它来控制通道行为的各个方面。当用户
  空间程序对绑定到该通道的某个 /dev/ppp 实例执行 ioctl 系统调用时，此过程会被
  调用。（通常只有 pppd 会这么做。）)

The generic PPP layer provides seven functions to channels:
(通用 PPP 层向通道提供七个函数：)

* ppp_register_channel() is called when a channel has been created, to
  notify the PPP generic layer of its presence.  For example, setting
  a serial port to the PPPDISC line discipline causes the ppp_async
  channel code to call this function.
  (ppp_register_channel() 在通道创建完成时被调用，将其存在通知通用 PPP 层。
  例如，把串口设置为 PPPDISC 线路规程会使 ppp_async 通道代码调用该函数。)

* ppp_unregister_channel() is called when a channel is to be
  destroyed.  For example, the ppp_async channel code calls this when
  a hangup is detected on the serial port.
  (ppp_unregister_channel() 在通道即将被销毁时被调用。例如，ppp_async 通道代码
  在检测到串口挂断时会调用它。)

* ppp_output_wakeup() is called by a channel when it has previously
  rejected a call to its start_xmit function, and can now accept more
  packets.
  (通道此前拒绝了对其 start_xmit 函数的调用、而现在又可以接受更多数据包时，
  会调用 ppp_output_wakeup()。)

* ppp_input() is called by a channel when it has received a complete
  PPP frame.
  (通道接收到一个完整的 PPP 帧时会调用 ppp_input()。)

* ppp_input_error() is called by a channel when it has detected that a
  frame has been lost or dropped (for example, because of a FCS (frame
  check sequence) error).
  (通道检测到某个帧丢失或被丢弃时（例如由于 FCS（帧校验序列）错误），会调用
  ppp_input_error()。)

* ppp_channel_index() returns the channel index assigned by the PPP
  generic layer to this channel.  The channel should provide some way
  (e.g. an ioctl) to transmit this back to user-space, as user-space
  will need it to attach an instance of /dev/ppp to this channel.
  (ppp_channel_index() 返回通用 PPP 层分配给该通道的通道索引。通道应提供某种
  途径（例如 ioctl）把它传回用户空间，因为用户空间需要用它把一个 /dev/ppp 实例
  附接到该通道。)

* ppp_unit_number() returns the unit number of the ppp network
  interface to which this channel is connected, or -1 if the channel
  is not connected.
  (ppp_unit_number() 返回该通道所连接的 ppp 网络接口的单元号；如果通道未连接，
  则返回 -1。)

Connecting a channel to the ppp generic layer is initiated from the
channel code, rather than from the generic layer.  The channel is
expected to have some way for a user-level process to control it
independently of the ppp generic layer.  For example, with the
ppp_async channel, this is provided by the file descriptor to the
serial port.
(把通道连接到通用 PPP 层是由通道代码发起的，而不是由通用层发起。通道应当提供
某种方式，让用户级进程可以独立于通用 PPP 层来控制它。例如，对 ppp_async 通道
而言，这是由指向串口的文件描述符提供的。)

Generally a user-level process will initialize the underlying
communications medium and prepare it to do PPP.  For example, with an
async tty, this can involve setting the tty speed and modes, issuing
modem commands, and then going through some sort of dialog with the
remote system to invoke PPP service there.  We refer to this process
as ``discovery``.  Then the user-level process tells the medium to
become a PPP channel and register itself with the generic PPP layer.
The channel then has to report the channel number assigned to it back
to the user-level process.  From that point, the PPP negotiation code
in the PPP daemon (pppd) can take over and perform the PPP
negotiation, accessing the channel through the /dev/ppp interface.
(通常，用户级进程会先初始化底层通信介质并准备好进行 PPP。例如，对异步 tty 而
言，这可能包括设置 tty 的速度与模式、发出调制解调器命令，然后与远端系统进行某
种对话以在那边启用 PPP 服务。我们把这一过程称为 ``discovery`` (发现)。随后，
用户级进程让该介质成为一个 PPP 通道并向通用 PPP 层注册自己。接着，通道必须把
分配给它的通道号报告给用户级进程。从那时起，PPP 守护进程（pppd）中的 PPP 协商
代码就可以接管并执行 PPP 协商，通过 /dev/ppp 接口访问该通道。)

At the interface to the PPP generic layer, PPP frames are stored in
skbuff structures and start with the two-byte PPP protocol number.
The frame does *not* include the 0xff ``address`` byte or the 0x03
``control`` byte that are optionally used in async PPP.  Nor is there
any escaping of control characters, nor are there any FCS or framing
characters included.  That is all the responsibility of the channel
code, if it is needed for the particular medium.  That is, the skbuffs
presented to the start_xmit() function contain only the 2-byte
protocol number and the data, and the skbuffs presented to ppp_input()
must be in the same format.
(在与通用 PPP 层的接口处，PPP 帧存储在 skbuff 结构中，并以两字节的 PPP 协议号
开头。帧并不包含异步 PPP 中可选使用的 0xff ``address`` 字节或 0x03
``control`` 字节，也没有任何控制字符转义，更不包含任何 FCS 或成帧字符。如果
特定介质需要这些，那全是通道代码的责任。也就是说，提交给 start_xmit() 函数的
skbuff 只包含 2 字节的协议号和数据，而提交给 ppp_input() 的 skbuff 必须采用相同
的格式。)

The channel must provide an instance of a ppp_channel struct to
represent the channel.  The channel is free to use the ``private`` field
however it wishes.  The channel should initialize the ``mtu`` and
``hdrlen`` fields before calling ppp_register_channel() and not change
them until after ppp_unregister_channel() returns.  The ``mtu`` field
represents the maximum size of the data part of the PPP frames, that
is, it does not include the 2-byte protocol number.
(通道必须提供一个 ppp_channel 结构实例来代表自己。通道可以随意使用 ``private``
字段。通道应在调用 ppp_register_channel() 之前初始化 ``mtu`` 和 ``hdrlen``
字段，并在 ppp_unregister_channel() 返回之前不得更改它们。``mtu`` 字段表示 PPP
帧数据部分的最大尺寸，即不包括 2 字节的协议号。)

If the channel needs some headroom in the skbuffs presented to it for
transmission (i.e., some space free in the skbuff data area before the
start of the PPP frame), it should set the ``hdrlen`` field of the
ppp_channel struct to the amount of headroom required.  The generic
PPP layer will attempt to provide that much headroom but the channel
should still check if there is sufficient headroom and copy the skbuff
if there isn't.
(如果通道需要提交给它的发送 skbuff 留有一些头部空间（即在 skbuff 数据区中、PPP
帧开始之前有一些空闲空间），应把 ppp_channel 结构的 ``hdrlen`` 字段设置为所需
的头部空间大小。通用 PPP 层会尝试提供这么多头部空间，但通道仍应检查头部空间
是否充足，不足时应复制 skbuff。)

On the input side, channels should ideally provide at least 2 bytes of
headroom in the skbuffs presented to ppp_input().  The generic PPP
code does not require this but will be more efficient if this is done.
(在接收一侧，理想情况下通道提交给 ppp_input() 的 skbuff 应预留至少 2 字节的
头部空间。通用 PPP 代码并不强制要求这一点，但如果这样做，效率会更高。)


Buffering and flow control
(缓冲与流量控制)
--------------------------

The generic PPP layer has been designed to minimize the amount of data
that it buffers in the transmit direction.  It maintains a queue of
transmit packets for the PPP unit (network interface device) plus a
queue of transmit packets for each attached channel.  Normally the
transmit queue for the unit will contain at most one packet; the
exceptions are when pppd sends packets by writing to /dev/ppp, and
when the core networking code calls the generic layer's start_xmit()
function with the queue stopped, i.e. when the generic layer has
called netif_stop_queue(), which only happens on a transmit timeout.
The start_xmit function always accepts and queues the packet which it
is asked to transmit.
(通用 PPP 层的设计目标是尽量减少它在发送方向上缓冲的数据量。它为 PPP 单元
（网络接口设备）维护一个发送数据包队列，并为每个已连接的通道各维护一个发送
数据包队列。通常，单元的发送队列最多包含一个数据包；例外情况是：pppd 通过写
/dev/ppp 发送数据包时，以及核心网络代码在队列停用状态下调用通用层的
start_xmit() 函数时——即通用层已调用 netif_stop_queue() 的情况，这只在发送
超时时发生。start_xmit 函数总是会接受并排队它被要求发送的数据包。)

Transmit packets are dequeued from the PPP unit transmit queue and
then subjected to TCP/IP header compression and packet compression
(Deflate or BSD-Compress compression), as appropriate.  After this
point the packets can no longer be reordered, as the decompression
algorithms rely on receiving compressed packets in the same order that
they were generated.
(发送数据包从 PPP 单元的发送队列中出队，然后视情况接受 TCP/IP 头压缩和数据包
压缩（Deflate 或 BSD-Compress 压缩）。在此之后，数据包就不能再被重排了，因为
解压算法要求按压缩包生成时的顺序接收它们。)

If multilink is not in use, this packet is then passed to the attached
channel's start_xmit() function.  If the channel refuses to take
the packet, the generic layer saves it for later transmission.  The
generic layer will call the channel's start_xmit() function again
when the channel calls  ppp_output_wakeup() or when the core
networking code calls the generic layer's start_xmit() function
again.  The generic layer contains no timeout and retransmission
logic; it relies on the core networking code for that.
(如果不使用多链路，该数据包随后会被交给所连接通道的 start_xmit() 函数。如果
通道拒绝接收该数据包，通用层会把它保存起来以供稍后发送。当通道调用
ppp_output_wakeup() 时，或者当核心网络代码再次调用通用层的 start_xmit() 函数
时，通用层会再次调用通道的 start_xmit() 函数。通用层本身不包含超时与重传逻辑；
这部分依赖核心网络代码。)

If multilink is in use, the generic layer divides the packet into one
or more fragments and puts a multilink header on each fragment.  It
decides how many fragments to use based on the length of the packet
and the number of channels which are potentially able to accept a
fragment at the moment.  A channel is potentially able to accept a
fragment if it doesn't have any fragments currently queued up for it
to transmit.  The channel may still refuse a fragment; in this case
the fragment is queued up for the channel to transmit later.  This
scheme has the effect that more fragments are given to higher-
bandwidth channels.  It also means that under light load, the generic
layer will tend to fragment large packets across all the channels,
thus reducing latency, while under heavy load, packets will tend to be
transmitted as single fragments, thus reducing the overhead of
fragmentation.
(如果使用多链路，通用层会把数据包拆成一个或多个分片，并为每个分片加上一个多
链路头。它根据数据包的长度以及当前潜在能够接受分片的通道数量来决定使用多少个
分片。如果某个通道当前没有为它排队的待发送分片，它就潜在地能够接受一个分片。
通道仍可能拒绝某个分片；这种情况下，该分片会被排队，供该通道稍后发送。这种方
案的效果是，带宽更高的通道会分到更多分片。它还意味着：在轻负载下，通用层倾向
于把大数据包拆分到所有通道上，从而降低延迟；而在重负载下，数据包倾向于作为单
个分片发送，从而降低分片的开销。)


SMP safety
(SMP 安全性)
----------

The PPP generic layer has been designed to be SMP-safe.  Locks are
used around accesses to the internal data structures where necessary
to ensure their integrity.  As part of this, the generic layer
requires that the channels adhere to certain requirements and in turn
provides certain guarantees to the channels.  Essentially the channels
are required to provide the appropriate locking on the ppp_channel
structures that form the basis of the communication between the
channel and the generic layer.  This is because the channel provides
the storage for the ppp_channel structure, and so the channel is
required to provide the guarantee that this storage exists and is
valid at the appropriate times.
(PPP 通用层被设计为 SMP 安全的。在必要之处，对内部数据结构的访问会使用锁来保
证其完整性。作为其中的一部分，通用层要求通道遵守某些要求，反过来又向通道提供
某些保证。本质上，通道需要为构成通道与通用层之间通信基础的 ppp_channel 结构
提供适当的加锁。这是因为 ppp_channel 结构的存储由通道提供，因此通道必须保证
该存储在适当的时刻存在且有效。)

The generic layer requires these guarantees from the channel:
(通用层要求通道提供以下保证：)

* The ppp_channel object must exist from the time that
  ppp_register_channel() is called until after the call to
  ppp_unregister_channel() returns.
  (ppp_channel 对象必须从调用 ppp_register_channel() 之时起一直存在，直到
  ppp_unregister_channel() 调用返回之后。)

* No thread may be in a call to any of ppp_input(), ppp_input_error(),
  ppp_output_wakeup(), ppp_channel_index() or ppp_unit_number() for a
  channel at the time that ppp_unregister_channel() is called for that
  channel.
  (对某个通道调用 ppp_unregister_channel() 时，不得有任何线程正处于针对该通道
  的 ppp_input()、ppp_input_error()、ppp_output_wakeup()、ppp_channel_index()
  或 ppp_unit_number() 调用之中。)

* ppp_register_channel() and ppp_unregister_channel() must be called
  from process context, not interrupt or softirq/BH context.
  (ppp_register_channel() 和 ppp_unregister_channel() 必须从进程上下文调用，
  而不能在中断或 softirq/BH 上下文调用。)

* The remaining generic layer functions may be called at softirq/BH
  level but must not be called from a hardware interrupt handler.
  (其余通用层函数可以在 softirq/BH 级别调用，但不得在硬件中断处理程序中调用。)

* The generic layer may call the channel start_xmit() function at
  softirq/BH level but will not call it at interrupt level.  Thus the
  start_xmit() function may not block.
  (通用层可以在 softirq/BH 级别调用通道的 start_xmit() 函数，但不会在中断级别
  调用它。因此 start_xmit() 函数不得阻塞。)

* The generic layer will only call the channel ioctl() function in
  process context.
  (通用层只会在进程上下文中调用通道的 ioctl() 函数。)

The generic layer provides these guarantees to the channels:
(通用层向通道提供以下保证：)

* The generic layer will not call the start_xmit() function for a
  channel while any thread is already executing in that function for
  that channel.
  (当已有线程正在执行某个通道的 start_xmit() 函数时，通用层不会再次为该通道
  调用该函数。)

* The generic layer will not call the ioctl() function for a channel
  while any thread is already executing in that function for that
  channel.
  (当已有线程正在执行某个通道的 ioctl() 函数时，通用层不会再次为该通道调用
  该函数。)

* By the time a call to ppp_unregister_channel() returns, no thread
  will be executing in a call from the generic layer to that channel's
  start_xmit() or ioctl() function, and the generic layer will not
  call either of those functions subsequently.
  (到 ppp_unregister_channel() 调用返回时，将不再有线程正在执行通用层对该通道
  start_xmit() 或 ioctl() 函数的调用，而且通用层此后也不会再调用这两个函数。)


Interface to pppd
(与 pppd 的接口)
-----------------

The PPP generic layer exports a character device interface called
/dev/ppp.  This is used by pppd to control PPP interface units and
channels.  Although there is only one /dev/ppp, each open instance of
/dev/ppp acts independently and can be attached either to a PPP unit
or a PPP channel.  This is achieved using the file->private_data field
to point to a separate object for each open instance of /dev/ppp.  In
this way an effect similar to Solaris' clone open is obtained,
allowing us to control an arbitrary number of PPP interfaces and
channels without having to fill up /dev with hundreds of device names.
(PPP 通用层导出了一个名为 /dev/ppp 的字符设备接口。pppd 用它来控制 PPP 接口
单元和通道。虽然只有一个 /dev/ppp，但每个打开的 /dev/ppp 实例都独立运作，可以
被附接到一个 PPP 单元或一个 PPP 通道。这是通过 file->private_data 字段为每个
打开的 /dev/ppp 实例指向一个单独的对象来实现的。这样获得了类似 Solaris 克隆打
开（clone open）的效果，使我们能够控制任意数量的 PPP 接口和通道，而不必用数百
个设备名把 /dev 填满。)

When /dev/ppp is opened, a new instance is created which is initially
unattached.  Using an ioctl call, it can then be attached to an
existing unit, attached to a newly-created unit, or attached to an
existing channel.  An instance attached to a unit can be used to send
and receive PPP control frames, using the read() and write() system
calls, along with poll() if necessary.  Similarly, an instance
attached to a channel can be used to send and receive PPP frames on
that channel.
(打开 /dev/ppp 时会创建一个初始未附接的新实例。随后可以通过 ioctl 调用把它附
接到一个现有单元、一个新建单元，或一个现有通道。附接到单元的实例可以用来发送
和接收 PPP 控制帧，使用 read() 和 write() 系统调用，必要时还可配合 poll()。
类似地，附接到通道的实例可以用来在该通道上发送和接收 PPP 帧。)

In multilink terms, the unit represents the bundle, while the channels
represent the individual physical links.  Thus, a PPP frame sent by a
write to the unit (i.e., to an instance of /dev/ppp attached to the
unit) will be subject to bundle-level compression and to fragmentation
across the individual links (if multilink is in use).  In contrast, a
PPP frame sent by a write to the channel will be sent as-is on that
channel, without any multilink header.
(用多链路的术语来说，单元代表捆束（bundle），而通道代表各个物理链路。因此，通
过对单元执行写操作（即写入附接到该单元的 /dev/ppp 实例）发送的 PPP 帧会经受
捆束级压缩，并在使用多链路时被分片到各条链路上。相比之下，通过写通道发送的
PPP 帧会按原样在该通道上发送，不带任何多链路头。)

A channel is not initially attached to any unit.  In this state it can
be used for PPP negotiation but not for the transfer of data packets.
It can then be connected to a PPP unit with an ioctl call, which
makes it available to send and receive data packets for that unit.
(通道初始时未附接到任何单元。在这种状态下，它可以用于 PPP 协商，但不能用于传
输数据包。随后可以通过 ioctl 调用把它连接到一个 PPP 单元，使它能够为该单元发
送和接收数据包。)

The ioctl calls which are available on an instance of /dev/ppp depend
on whether it is unattached, attached to a PPP interface, or attached
to a PPP channel.  The ioctl calls which are available on an
unattached instance are:
(一个 /dev/ppp 实例上可用的 ioctl 调用取决于它是未附接、附接到 PPP 接口，还是
附接到 PPP 通道。未附接的实例上可用的 ioctl 调用有：)

* PPPIOCNEWUNIT creates a new PPP interface and makes this /dev/ppp
  instance the "owner" of the interface.  The argument should point to
  an int which is the desired unit number if >= 0, or -1 to assign the
  lowest unused unit number.  Being the owner of the interface means
  that the interface will be shut down if this instance of /dev/ppp is
  closed.
  (PPPIOCNEWUNIT 创建一个新的 PPP 接口，并使该 /dev/ppp 实例成为该接口的
  "所有者"。参数应指向一个 int：若 >= 0 则为期望的单元号，若为 -1 则分配最小
  的未用单元号。作为接口的所有者意味着：如果这个 /dev/ppp 实例被关闭，该接口
  将被关闭。)

* PPPIOCATTACH attaches this instance to an existing PPP interface.
  The argument should point to an int containing the unit number.
  This does not make this instance the owner of the PPP interface.
  (PPPIOCATTACH 把该实例附接到一个现有的 PPP 接口。参数应指向一个包含单元号的
  int。这不会使该实例成为该 PPP 接口的所有者。)

* PPPIOCATTCHAN attaches this instance to an existing PPP channel.
  The argument should point to an int containing the channel number.
  (PPPIOCATTCHAN 把该实例附接到一个现有的 PPP 通道。参数应指向一个包含通道号的
  int。)

The ioctl calls available on an instance of /dev/ppp attached to a
channel are:
(附接到通道的 /dev/ppp 实例上可用的 ioctl 调用有：)

* PPPIOCCONNECT connects this channel to a PPP interface.  The
  argument should point to an int containing the interface unit
  number.  It will return an EINVAL error if the channel is already
  connected to an interface, or ENXIO if the requested interface does
  not exist.
  (PPPIOCCONNECT 把该通道连接到一个 PPP 接口。参数应指向一个包含接口单元号的
  int。如果该通道已连接到某个接口，会返回 EINVAL 错误；如果所请求的接口不存在，
  则返回 ENXIO。)

* PPPIOCDISCONN disconnects this channel from the PPP interface that
  it is connected to.  It will return an EINVAL error if the channel
  is not connected to an interface.
  (PPPIOCDISCONN 把该通道从它所连接的 PPP 接口断开。如果该通道未连接到任何
  接口，会返回 EINVAL 错误。)

* PPPIOCBRIDGECHAN bridges a channel with another. The argument should
  point to an int containing the channel number of the channel to bridge
  to. Once two channels are bridged, frames presented to one channel by
  ppp_input() are passed to the bridge instance for onward transmission.
  This allows frames to be switched from one channel into another: for
  example, to pass PPPoE frames into a PPPoL2TP session. Since channel
  bridging interrupts the normal ppp_input() path, a given channel may
  not be part of a bridge at the same time as being part of a unit.
  This ioctl will return an EALREADY error if the channel is already
  part of a bridge or unit, or ENXIO if the requested channel does not
  exist.
  (PPPIOCBRIDGECHAN 把一个通道与另一个通道桥接。参数应指向一个 int，其中包含
  要桥接到的通道的通道号。两个通道一旦桥接，通过 ppp_input() 提交给其中一个
  通道的帧会被传递给桥接实例以继续传输。这使得帧可以从一个通道切换到另一个
  通道，例如把 PPPoE 帧传入一个 PPPoL2TP 会话。由于通道桥接会中断正常的
  ppp_input() 路径，给定的通道不能同时属于一个桥和一个单元。如果该通道已经属
  于某个桥或单元，该 ioctl 会返回 EALREADY 错误；如果所请求的通道不存在，则
  返回 ENXIO。)

* PPPIOCUNBRIDGECHAN performs the inverse of PPPIOCBRIDGECHAN, unbridging
  a channel pair.  This ioctl will return an EINVAL error if the channel
  does not form part of a bridge.
  (PPPIOCUNBRIDGECHAN 执行 PPPIOCBRIDGECHAN 的逆操作，解除一对通道的桥接。如果
  该通道不属于任何桥，该 ioctl 会返回 EINVAL 错误。)

* All other ioctl commands are passed to the channel ioctl() function.
  (所有其他 ioctl 命令都会传递给通道的 ioctl() 函数。)

The ioctl calls that are available on an instance that is attached to
an interface unit are:
(附接到接口单元的实例上可用的 ioctl 调用有：)

* PPPIOCSMRU sets the MRU (maximum receive unit) for the interface.
  The argument should point to an int containing the new MRU value.
  (PPPIOCSMRU 设置接口的 MRU（最大接收单元）。参数应指向一个包含新 MRU 值的
  int。)

* PPPIOCSFLAGS sets flags which control the operation of the
  interface.  The argument should be a pointer to an int containing
  the new flags value.  The bits in the flags value that can be set
  are:
  (PPPIOCSFLAGS 设置控制接口操作的标志。参数应为一个指向 int 的指针，其中包含
  新的标志值。标志值中可以设置的位有：)

	================	========================================
	SC_COMP_TCP		enable transmit TCP header compression
				(启用发送方向的 TCP 头部压缩)
	SC_NO_TCP_CCID		disable connection-id compression for
				TCP header compression
				(禁用 TCP 头部压缩的连接 ID 压缩)
	SC_REJ_COMP_TCP		disable receive TCP header decompression
				(禁用接收方向的 TCP 头部解压)
	SC_CCP_OPEN		Compression Control Protocol (CCP) is
				open, so inspect CCP packets
				(压缩控制协议 (CCP) 处于打开状态，因此要检查 CCP 数据包)
	SC_CCP_UP		CCP is up, may (de)compress packets
				(CCP 已就绪，可以（解）压缩数据包)
	SC_LOOP_TRAFFIC		send IP traffic to pppd
				(把 IP 流量发送给 pppd)
	SC_MULTILINK		enable PPP multilink fragmentation on
				transmitted packets
				(对发送的数据包启用 PPP 多链路分片)
	SC_MP_SHORTSEQ		expect short multilink sequence
				numbers on received multilink fragments
				(在接收到的多链路分片上期望短的多链路序号)
	SC_MP_XSHORTSEQ		transmit short multilink sequence nos.
				(发送短的多链路序号)
	================	========================================

  The values of these flags are defined in <linux/ppp-ioctl.h>.  Note
  that the values of the SC_MULTILINK, SC_MP_SHORTSEQ and
  SC_MP_XSHORTSEQ bits are ignored if the CONFIG_PPP_MULTILINK option
  is not selected.
  (这些标志的值定义在 <linux/ppp-ioctl.h> 中。注意，如果未选择
  CONFIG_PPP_MULTILINK 选项，则 SC_MULTILINK、SC_MP_SHORTSEQ 和
  SC_MP_XSHORTSEQ 位的值会被忽略。)

* PPPIOCGFLAGS returns the value of the status/control flags for the
  interface unit.  The argument should point to an int where the ioctl
  will store the flags value.  As well as the values listed above for
  PPPIOCSFLAGS, the following bits may be set in the returned value:
  (PPPIOCGFLAGS 返回接口单元的状态/控制标志的值。参数应指向一个 int，ioctl 会
  把标志值存储在其中。除了上面 PPPIOCSFLAGS 列出的值之外，返回值中还可能设置
  以下位：)

	================	=========================================
	SC_COMP_RUN		CCP compressor is running
				(CCP 压缩器正在运行)
	SC_DECOMP_RUN		CCP decompressor is running
				(CCP 解压器正在运行)
	SC_DC_ERROR		CCP decompressor detected non-fatal error
				(CCP 解压器检测到非致命错误)
	SC_DC_FERROR		CCP decompressor detected fatal error
				(CCP 解压器检测到致命错误)
	================	=========================================

* PPPIOCSCOMPRESS sets the parameters for packet compression or
  decompression.  The argument should point to a ppp_option_data
  structure (defined in <linux/ppp-ioctl.h>), which contains a
  pointer/length pair which should describe a block of memory
  containing a CCP option specifying a compression method and its
  parameters.  The ppp_option_data struct also contains a ``transmit``
  field.  If this is 0, the ioctl will affect the receive path,
  otherwise the transmit path.
  (PPPIOCSCOMPRESS 设置数据包压缩或解压的参数。参数应指向一个 ppp_option_data
  结构（定义于 <linux/ppp-ioctl.h>），其中包含一个指针/长度对，应描述一块内存，
  其内容为一个指定压缩方法及其参数的 CCP 选项。ppp_option_data 结构还包含一个
  ``transmit`` 字段。若其为 0，该 ioctl 作用于接收路径，否则作用于发送路径。)

* PPPIOCGUNIT returns, in the int pointed to by the argument, the unit
  number of this interface unit.
  (PPPIOCGUNIT 在参数所指向的 int 中返回该接口单元的单元号。)

* PPPIOCSDEBUG sets the debug flags for the interface to the value in
  the int pointed to by the argument.  Only the least significant bit
  is used; if this is 1 the generic layer will print some debug
  messages during its operation.  This is only intended for debugging
  the generic PPP layer code; it is generally not helpful for working
  out why a PPP connection is failing.
  (PPPIOCSDEBUG 将接口的调试标志设置为参数所指向的 int 中的值。只有最低有效位
  会被使用；若其为 1，通用层在运行过程中会打印一些调试消息。它仅用于调试通用
  PPP 层代码；对排查 PPP 连接失败的原因通常没有帮助。)

* PPPIOCGDEBUG returns the debug flags for the interface in the int
  pointed to by the argument.
  (PPPIOCGDEBUG 在参数所指向的 int 中返回接口的调试标志。)

* PPPIOCGIDLE returns the time, in seconds, since the last data
  packets were sent and received.  The argument should point to a
  ppp_idle structure (defined in <linux/ppp_defs.h>).  If the
  CONFIG_PPP_FILTER option is enabled, the set of packets which reset
  the transmit and receive idle timers is restricted to those which
  pass the ``active`` packet filter.
  Two versions of this command exist, to deal with user space
  expecting times as either 32-bit or 64-bit time_t seconds.
  (PPPIOCGIDLE 返回自上次收发数据包以来的时间（以秒为单位）。参数应指向一个
  ppp_idle 结构（定义于 <linux/ppp_defs.h>）。如果启用了 CONFIG_PPP_FILTER
  选项，能重置发送与接收空闲计时器的数据包集合被限定为通过 ``active`` 数据包
  过滤器的那些。此命令存在两个版本，用于应对用户空间把时间当作 32 位或 64 位
  time_t 秒的情况。)

* PPPIOCSMAXCID sets the maximum connection-ID parameter (and thus the
  number of connection slots) for the TCP header compressor and
  decompressor.  The lower 16 bits of the int pointed to by the
  argument specify the maximum connection-ID for the compressor.  If
  the upper 16 bits of that int are non-zero, they specify the maximum
  connection-ID for the decompressor, otherwise the decompressor's
  maximum connection-ID is set to 15.
  (PPPIOCSMAXCID 为 TCP 头压缩器和解压器设置最大连接 ID 参数（并由此决定连接
  槽位的数量）。参数所指向的 int 的低 16 位指定压缩器的最大连接 ID。如果该 int
  的高 16 位非零，则它们指定解压器的最大连接 ID；否则解压器的最大连接 ID 被设
  为 15。)

* PPPIOCSNPMODE sets the network-protocol mode for a given network
  protocol.  The argument should point to an npioctl struct (defined
  in <linux/ppp-ioctl.h>).  The ``protocol`` field gives the PPP protocol
  number for the protocol to be affected, and the ``mode`` field
  specifies what to do with packets for that protocol:
  (PPPIOCSNPMODE 设置给定网络协议的网络协议模式。参数应指向一个 npioctl 结构
  （定义于 <linux/ppp-ioctl.h>）。``protocol`` 字段给出要影响的协议的 PPP 协议
  号，``mode`` 字段指定对该协议的数据包如何处理：)

	=============	==============================================
	NPMODE_PASS	normal operation, transmit and receive packets
			(正常操作，发送并接收数据包)
	NPMODE_DROP	silently drop packets for this protocol
			(静默丢弃该协议的数据包)
	NPMODE_ERROR	drop packets and return an error on transmit
			(丢弃数据包，并在发送时返回错误)
	NPMODE_QUEUE	queue up packets for transmit, drop received
			packets
			(将待发送的数据包排队，丢弃接收到的数据包)
	=============	==============================================

  At present NPMODE_ERROR and NPMODE_QUEUE have the same effect as
  NPMODE_DROP.
  (目前 NPMODE_ERROR 和 NPMODE_QUEUE 的效果与 NPMODE_DROP 相同。)

* PPPIOCGNPMODE returns the network-protocol mode for a given
  protocol.  The argument should point to an npioctl struct with the
  ``protocol`` field set to the PPP protocol number for the protocol of
  interest.  On return the ``mode`` field will be set to the network-
  protocol mode for that protocol.
  (PPPIOCGNPMODE 返回给定协议的网络协议模式。参数应指向一个 npioctl 结构，其
  ``protocol`` 字段设置为所关注协议的 PPP 协议号。返回时 ``mode`` 字段会被设
  置为该协议的网络协议模式。)

* PPPIOCSPASS and PPPIOCSACTIVE set the ``pass`` and ``active`` packet
  filters.  These ioctls are only available if the CONFIG_PPP_FILTER
  option is selected.  The argument should point to a sock_fprog
  structure (defined in <linux/filter.h>) containing the compiled BPF
  instructions for the filter.  Packets are dropped if they fail the
  ``pass`` filter; otherwise, if they fail the ``active`` filter they are
  passed but they do not reset the transmit or receive idle timer.
  (PPPIOCSPASS 和 PPPIOCSACTIVE 设置 ``pass`` 与 ``active`` 数据包过滤器。仅当
  选择了 CONFIG_PPP_FILTER 选项时这些 ioctl 才可用。参数应指向一个 sock_fprog
  结构（定义于 <linux/filter.h>），其中包含过滤器的已编译 BPF 指令。未通过
  ``pass`` 过滤器的数据包会被丢弃；否则，如果它们未通过 ``active`` 过滤器，
  仍会被传递，但不会重置发送或接收空闲计时器。)

* PPPIOCSMRRU enables or disables multilink processing for received
  packets and sets the multilink MRRU (maximum reconstructed receive
  unit).  The argument should point to an int containing the new MRRU
  value.  If the MRRU value is 0, processing of received multilink
  fragments is disabled.  This ioctl is only available if the
  CONFIG_PPP_MULTILINK option is selected.
  (PPPIOCSMRRU 启用或禁用对接收数据包的多链路处理，并设置多链路 MRRU（最大重
  组接收单元）。参数应指向一个包含新 MRRU 值的 int。如果 MRRU 值为 0，则禁用
  对接收到的多链路分片的处理。仅当选择了 CONFIG_PPP_MULTILINK 选项时此 ioctl
  才可用。)

Last modified: 7-feb-2002
(最后修改：2002 年 2 月 7 日。)
