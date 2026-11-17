.. SPDX-License-Identifier: GPL-2.0

====================
Timestamping(时间戳)
====================


1. Control Interfaces(1. 控制接口)
==================================

The interfaces for receiving network packages timestamps are:
(用于接收网络数据包时间戳的接口有：)

SO_TIMESTAMP
(SO_TIMESTAMP)

  Generates a timestamp for each incoming packet in (not necessarily
  monotonic) system time. Reports the timestamp via recvmsg() in a
  control message in usec resolution.
  SO_TIMESTAMP is defined as SO_TIMESTAMP_NEW or SO_TIMESTAMP_OLD
  based on the architecture type and time_t representation of libc.
  Control message format is in struct __kernel_old_timeval for
  SO_TIMESTAMP_OLD and in struct __kernel_sock_timeval for
  SO_TIMESTAMP_NEW options respectively.
  (为每个传入的数据包以 (不一定是单调的) 系统时间生成一个时间戳。通过
  recvmsg() 在控制消息中以微秒分辨率报告该时间戳。SO_TIMESTAMP 根据体系结构
  类型和 libc 的 time_t 表示方式，被定义为 SO_TIMESTAMP_NEW 或 SO_TIMESTAMP_OLD。
  控制消息格式在 SO_TIMESTAMP_OLD 选项下为 struct __kernel_old_timeval，在
  SO_TIMESTAMP_NEW 选项下为 struct __kernel_sock_timeval。)

SO_TIMESTAMPNS
(SO_TIMESTAMPNS)

  Same timestamping mechanism as SO_TIMESTAMP, but reports the
  timestamp as struct timespec in nsec resolution.
  SO_TIMESTAMPNS is defined as SO_TIMESTAMPNS_NEW or SO_TIMESTAMPNS_OLD
  based on the architecture type and time_t representation of libc.
  Control message format is in struct timespec for SO_TIMESTAMPNS_OLD
  and in struct __kernel_timespec for SO_TIMESTAMPNS_NEW options
  respectively.
  (与 SO_TIMESTAMP 相同的时间戳机制，但以纳秒分辨率把时间戳报告为
  struct timespec。SO_TIMESTAMPNS 根据体系结构类型和 libc 的 time_t 表示方式，
  被定义为 SO_TIMESTAMPNS_NEW 或 SO_TIMESTAMPNS_OLD。控制消息格式在
  SO_TIMESTAMPNS_OLD 选项下为 struct timespec，在 SO_TIMESTAMPNS_NEW 选项下为
  struct __kernel_timespec。)

IP_MULTICAST_LOOP + SO_TIMESTAMP[NS]
(IP_MULTICAST_LOOP + SO_TIMESTAMP[NS])

  Only for multicast:approximate transmit timestamp obtained by
  reading the looped packet receive timestamp.
  (仅用于组播：通过读取回环数据包的接收时间戳获得的近似发送时间戳。)

SO_TIMESTAMPING
(SO_TIMESTAMPING)

  Generates timestamps on reception, transmission or both. Supports
  multiple timestamp sources, including hardware. Supports generating
  timestamps for stream sockets.
  (在接收、发送或两者兼有的情况下生成时间戳。支持多种时间戳来源，包括硬件。
  支持为流式套接字生成时间戳。)


1.1 SO_TIMESTAMP (also SO_TIMESTAMP_OLD and SO_TIMESTAMP_NEW)(1.1 SO_TIMESTAMP (亦即 SO_TIMESTAMP_OLD 和 SO_TIMESTAMP_NEW))
---------------------------------------------------------------------------------------------------------------------------

This socket option enables timestamping of datagrams on the reception
path. Because the destination socket, if any, is not known early in
the network stack, the feature has to be enabled for all packets. The
same is true for all early receive timestamp options.
(该套接字选项对接收路径上的数据报启用时间戳功能。由于在网络栈的早期阶段还
不知道目的套接字 (如果有的话)，因此必须对所有数据包启用该特性。所有早期接收
时间戳选项都是如此。)

For interface details, see `man 7 socket`.
(接口细节参见 `man 7 socket`。)

Always use SO_TIMESTAMP_NEW timestamp to always get timestamp in
struct __kernel_sock_timeval format.
(请始终使用 SO_TIMESTAMP_NEW 时间戳，以始终获得 struct __kernel_sock_timeval
格式的时间戳。)

SO_TIMESTAMP_OLD returns incorrect timestamps after the year 2038
on 32 bit machines.
(SO_TIMESTAMP_OLD 在 32 位机器上于 2038 年之后会返回不正确的时间戳。)

1.2 SO_TIMESTAMPNS (also SO_TIMESTAMPNS_OLD and SO_TIMESTAMPNS_NEW)(1.2 SO_TIMESTAMPNS (亦即 SO_TIMESTAMPNS_OLD 和 SO_TIMESTAMPNS_NEW))
---------------------------------------------------------------------------------------------------------------------------------------

This option is identical to SO_TIMESTAMP except for the returned data type.
Its struct timespec allows for higher resolution (ns) timestamps than the
timeval of SO_TIMESTAMP (ms).
(该选项与 SO_TIMESTAMP 相同，只是返回的数据类型不同。它的 struct timespec 允许
比 SO_TIMESTAMP 的 timeval (毫秒) 更高分辨率 (纳秒) 的时间戳。)

Always use SO_TIMESTAMPNS_NEW timestamp to always get timestamp in
struct __kernel_timespec format.
(请始终使用 SO_TIMESTAMPNS_NEW 时间戳，以始终获得 struct __kernel_timespec
格式的时间戳。)

SO_TIMESTAMPNS_OLD returns incorrect timestamps after the year 2038
on 32 bit machines.
(SO_TIMESTAMPNS_OLD 在 32 位机器上于 2038 年之后会返回不正确的时间戳。)

1.3 SO_TIMESTAMPING (also SO_TIMESTAMPING_OLD and SO_TIMESTAMPING_NEW)(1.3 SO_TIMESTAMPING (亦即 SO_TIMESTAMPING_OLD 和 SO_TIMESTAMPING_NEW))
---------------------------------------------------------------------------------------------------------------------------------------------

Supports multiple types of timestamp requests. As a result, this
socket option takes a bitmap of flags, not a boolean. In::
(支持多种类型的时间戳请求。因此，该套接字选项接受的是标志位图，而不是布尔值。
如下所示：)

  err = setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));

val is an integer with any of the following bits set. Setting other
bit returns EINVAL and does not change the current state.
(其中 val 是一个整数，可以设置以下任意位。设置其他位将返回 EINVAL，并且不会
改变当前状态。)

The socket option configures timestamp generation for individual
sk_buffs (1.3.1), timestamp reporting to the socket's error
queue (1.3.2) and options (1.3.3). Timestamp generation can also
be enabled for individual sendmsg calls using cmsg (1.3.4).
(该套接字选项为各个 sk_buff 配置时间戳生成 (1.3.1)、向套接字错误队列的时间戳
报告 (1.3.2) 以及选项 (1.3.3)。也可以使用 cmsg 为单个 sendmsg 调用启用时间戳
生成 (1.3.4)。)


1.3.1 Timestamp Generation(1.3.1 时间戳生成)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some bits are requests to the stack to try to generate timestamps. Any
combination of them is valid. Changes to these bits apply to newly
created packets, not to packets already in the stack. As a result, it
is possible to selectively request timestamps for a subset of packets
(e.g., for sampling) by embedding an send() call within two setsockopt
calls, one to enable timestamp generation and one to disable it.
Timestamps may also be generated for reasons other than being
requested by a particular socket, such as when receive timestamping is
enabled system wide, as explained earlier.
(其中一些位是要求协议栈尝试生成时间戳的请求。它们的任意组合都是有效的。对这些
位的修改只适用于新建的数据包，而不适用于已经在栈中的数据包。因此，可以通过把
一次 send() 调用嵌入两次 setsockopt 调用之间 (一次启用时间戳生成，一次禁用它)，
来有选择地为一部分数据包请求时间戳 (例如用于采样)。时间戳也可能因其他原因而
生成，而非因为某个特定套接字的请求，例如系统范围内启用了接收时间戳功能时，
如前文所述。)

SOF_TIMESTAMPING_RX_HARDWARE:
(SOF_TIMESTAMPING_RX_HARDWARE)

  Request rx timestamps generated by the network adapter.
  (请求由网络适配器生成的接收 (rx) 时间戳。)

SOF_TIMESTAMPING_RX_SOFTWARE:
(SOF_TIMESTAMPING_RX_SOFTWARE)

  Request rx timestamps when data enters the kernel. These timestamps
  are generated just after a device driver hands a packet to the
  kernel receive stack.
  (请求在数据进入内核时的接收 (rx) 时间戳。这些时间戳在设备驱动程序把数据包
  交给内核接收栈之后立即生成。)

SOF_TIMESTAMPING_TX_HARDWARE:
(SOF_TIMESTAMPING_TX_HARDWARE)

  Request tx timestamps generated by the network adapter. This flag
  can be enabled via both socket options and control messages.
  (请求由网络适配器生成的发送 (tx) 时间戳。该标志既可以通过套接字选项启用，
  也可以通过控制消息启用。)

SOF_TIMESTAMPING_TX_SOFTWARE:
(SOF_TIMESTAMPING_TX_SOFTWARE)

  Request tx timestamps when data leaves the kernel. These timestamps
  are generated in the device driver as close as possible, but always
  prior to, passing the packet to the network interface. Hence, they
  require driver support and may not be available for all devices.
  This flag can be enabled via both socket options and control messages.
  (请求在数据离开内核时的发送 (tx) 时间戳。这些时间戳在设备驱动程序中生成，
  尽可能靠近把数据包传递给网络接口的时机，但总是在传递之前。因此，它们需要
  驱动程序支持，并非对所有设备都可用。该标志既可以通过套接字选项启用，也可以
  通过控制消息启用。)

SOF_TIMESTAMPING_TX_SCHED:
(SOF_TIMESTAMPING_TX_SCHED)

  Request tx timestamps prior to entering the packet scheduler. Kernel
  transmit latency is, if long, often dominated by queuing delay. The
  difference between this timestamp and one taken at
  SOF_TIMESTAMPING_TX_SOFTWARE will expose this latency independent
  of protocol processing. The latency incurred in protocol
  processing, if any, can be computed by subtracting a userspace
  timestamp taken immediately before send() from this timestamp. On
  machines with virtual devices where a transmitted packet travels
  through multiple devices and, hence, multiple packet schedulers,
  a timestamp is generated at each layer. This allows for fine
  grained measurement of queuing delay. This flag can be enabled
  via both socket options and control messages.
  (请求在进入数据包调度器之前的发送 (tx) 时间戳。内核发送延迟如果较长，往往
  主要由排队延迟造成。该时间戳与在 SOF_TIMESTAMPING_TX_SOFTWARE 处取得的时间
  戳之差，可以在不涉及协议处理的情况下揭示这部分延迟。协议处理造成的延迟
  (如果有的话)，可以用该时间戳减去紧挨 send() 之前取得的用户空间时间戳来计算。
  在具有虚拟设备的机器上，发送的数据包会经过多个设备 (因而经过多个数据包调度
  器)，每一层都会生成一个时间戳。这使排队延迟可以得到细粒度的测量。该标志既
  可以通过套接字选项启用，也可以通过控制消息启用。)

SOF_TIMESTAMPING_TX_ACK:
(SOF_TIMESTAMPING_TX_ACK)

  Request tx timestamps when all data in the send buffer has been
  acknowledged. This only makes sense for reliable protocols. It is
  currently only implemented for TCP. For that protocol, it may
  over-report measurement, because the timestamp is generated when all
  data up to and including the buffer at send() was acknowledged: the
  cumulative acknowledgment. The mechanism ignores SACK and FACK.
  This flag can be enabled via both socket options and control messages.
  (请求在发送缓冲区中的所有数据都被确认时的发送 (tx) 时间戳。这只对可靠协议
  有意义。目前仅为 TCP 实现。对于该协议，它可能会过度报告测量结果，因为时间戳
  是在直到并包含 send() 时那个缓冲区在内的所有数据都被确认时生成的：即累计
  确认。该机制忽略 SACK 和 FACK。该标志既可以通过套接字选项启用，也可以通过
  控制消息启用。)

SOF_TIMESTAMPING_TX_COMPLETION:
(SOF_TIMESTAMPING_TX_COMPLETION)

  Request tx timestamps on packet tx completion.  The completion
  timestamp is generated by the kernel when it receives packet a
  completion report from the hardware. Hardware may report multiple
  packets at once, and completion timestamps reflect the timing of the
  report and not actual tx time. This flag can be enabled via both
  socket options and control messages.
  (请求在数据包发送完成时的发送 (tx) 时间戳。完成时间戳由内核在收到硬件的
  数据包完成报告时生成。硬件可能一次报告多个数据包，完成时间戳反映的是报告
  的时刻而非实际发送时刻。该标志既可以通过套接字选项启用，也可以通过控制消息
  启用。)


1.3.2 Timestamp Reporting(1.3.2 时间戳报告)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The other three bits control which timestamps will be reported in a
generated control message. Changes to the bits take immediate
effect at the timestamp reporting locations in the stack. Timestamps
are only reported for packets that also have the relevant timestamp
generation request set.
(另外三位控制生成的控制消息中报告哪些时间戳。对这些位的修改会在栈中的时间戳
报告位置立即生效。时间戳只为同时设置了相关时间戳生成请求的数据包报告。)

SOF_TIMESTAMPING_SOFTWARE:
(SOF_TIMESTAMPING_SOFTWARE)

  Report any software timestamps when available.
  (在可用时报告任何软件时间戳。)

SOF_TIMESTAMPING_SYS_HARDWARE:
(SOF_TIMESTAMPING_SYS_HARDWARE)

  This option is deprecated and ignored.
  (该选项已废弃并被忽略。)

SOF_TIMESTAMPING_RAW_HARDWARE:
(SOF_TIMESTAMPING_RAW_HARDWARE)

  Report hardware timestamps as generated by
  SOF_TIMESTAMPING_TX_HARDWARE or SOF_TIMESTAMPING_RX_HARDWARE
  when available.
  (在可用时报告由 SOF_TIMESTAMPING_TX_HARDWARE 或
  SOF_TIMESTAMPING_RX_HARDWARE 生成的硬件时间戳。)


1.3.3 Timestamp Options(1.3.3 时间戳选项)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The interface supports the options
(该接口支持以下选项)

SOF_TIMESTAMPING_OPT_ID:
(SOF_TIMESTAMPING_OPT_ID)

  Generate a unique identifier along with each packet. A process can
  have multiple concurrent timestamping requests outstanding. Packets
  can be reordered in the transmit path, for instance in the packet
  scheduler. In that case timestamps will be queued onto the error
  queue out of order from the original send() calls. It is not always
  possible to uniquely match timestamps to the original send() calls
  based on timestamp order or payload inspection alone, then.
  (为每个数据包生成一个唯一标识符。一个进程可以有多个并发未决的时间戳请求。
  数据包在发送路径上可能被重新排序，例如在数据包调度器中。这种情况下，时间戳
  进入错误队列的顺序将与原始 send() 调用的顺序不一致。于是，仅凭时间戳顺序或
  载荷检查，并不总是能够把时间戳与原始 send() 调用唯一地匹配起来。)

  This option associates each packet at send() with a unique
  identifier and returns that along with the timestamp. The identifier
  is derived from a per-socket u32 counter (that wraps). For datagram
  sockets, the counter increments with each sent packet. For stream
  sockets, it increments with every byte. For stream sockets, also set
  SOF_TIMESTAMPING_OPT_ID_TCP, see the section below.
  (该选项在 send() 时为每个数据包关联一个唯一标识符，并随时间戳一起返回。
  该标识符源自一个按套接字计数的 u32 计数器 (会回绕)。对于数据报套接字，计数器
  随每个发送的数据包递增；对于流式套接字，随每个字节递增。对于流式套接字，还要
  设置 SOF_TIMESTAMPING_OPT_ID_TCP，参见下文的小节。)

  The counter starts at zero. It is initialized the first time that
  the socket option is enabled. It is reset each time the option is
  enabled after having been disabled. Resetting the counter does not
  change the identifiers of existing packets in the system.
  (计数器从零开始。它在第一次启用该套接字选项时初始化，并且在禁用后再启用的
  每次都会重置。重置计数器不会改变系统中既有数据包的标识符。)

  This option is implemented only for transmit timestamps. There, the
  timestamp is always looped along with a struct sock_extended_err.
  The option modifies field ee_data to pass an id that is unique
  among all possibly concurrently outstanding timestamp requests for
  that socket.
  (该选项仅为发送时间戳实现。在那里，时间戳总是与一个 struct
  sock_extended_err 一起回环。该选项修改 ee_data 字段，以传递一个在该套接字
  所有可能并发未决的时间戳请求中唯一的 id。)

  The process can optionally override the default generated ID, by
  passing a specific ID with control message SCM_TS_OPT_ID (not
  supported for TCP sockets)::
  (进程可以通过控制消息 SCM_TS_OPT_ID 传入特定的 ID，从而可选地覆盖默认生成的
  ID (TCP 套接字不支持)：)

    struct msghdr *msg;
    ...
    cmsg			 = CMSG_FIRSTHDR(msg);
    cmsg->cmsg_level		 = SOL_SOCKET;
    cmsg->cmsg_type		 = SCM_TS_OPT_ID;
    cmsg->cmsg_len		 = CMSG_LEN(sizeof(__u32));
    *((__u32 *) CMSG_DATA(cmsg)) = opt_id;
    err = sendmsg(fd, msg, 0);


SOF_TIMESTAMPING_OPT_ID_TCP:
(SOF_TIMESTAMPING_OPT_ID_TCP)

  Pass this modifier along with SOF_TIMESTAMPING_OPT_ID for new TCP
  timestamping applications. SOF_TIMESTAMPING_OPT_ID defines how the
  counter increments for stream sockets, but its starting point is
  not entirely trivial. This option fixes that.
  (对于新的 TCP 时间戳应用程序，请将此修饰符与 SOF_TIMESTAMPING_OPT_ID 一起
  传入。SOF_TIMESTAMPING_OPT_ID 定义了流式套接字计数器的递增方式，但其起点
  并非完全直观，该选项修复了这个问题。)

  For stream sockets, if SOF_TIMESTAMPING_OPT_ID is set, this should
  always be set too. On datagram sockets the option has no effect.
  (对于流式套接字，如果设置了 SOF_TIMESTAMPING_OPT_ID，就应当始终同时设置
  这一项。在数据报套接字上该选项无效。)

  A reasonable expectation is that the counter is reset to zero with
  the system call, so that a subsequent write() of N bytes generates
  a timestamp with counter N-1. SOF_TIMESTAMPING_OPT_ID_TCP
  implements this behavior under all conditions.
  (一个合理的预期是：计数器随该系统调用重置为零，这样随后的 N 字节 write() 会
  生成计数器值为 N-1 的时间戳。SOF_TIMESTAMPING_OPT_ID_TCP 在所有条件下都
  实现了这一行为。)

  SOF_TIMESTAMPING_OPT_ID without modifier often reports the same,
  especially when the socket option is set when no data is in
  transmission. If data is being transmitted, it may be off by the
  length of the output queue (SIOCOUTQ).
  (不带修饰符的 SOF_TIMESTAMPING_OPT_ID 通常也报告同样的结果，特别是在没有
  数据正在传输时设置该套接字选项的情况下。如果有数据正在传输，它可能偏离输出
  队列 (SIOCOUTQ) 的长度。)

  The difference is due to being based on snd_una versus write_seq.
  snd_una is the offset in the stream acknowledged by the peer. This
  depends on factors outside of process control, such as network RTT.
  write_seq is the last byte written by the process. This offset is
  not affected by external inputs.
  (这一差异源于基于 snd_una 还是 write_seq。snd_una 是流中被对端确认的偏移量，
  这取决于进程控制之外的因素，例如网络 RTT。write_seq 是进程写入的最后一个
  字节，该偏移量不受外部输入影响。)

  The difference is subtle and unlikely to be noticed when configured
  at initial socket creation, when no data is queued or sent. But
  SOF_TIMESTAMPING_OPT_ID_TCP behavior is more robust regardless of
  when the socket option is set.
  (这种差异很细微，如果在初始创建套接字、尚无数据排队或发送时进行配置，通常
  察觉不到。但无论何时设置该套接字选项，SOF_TIMESTAMPING_OPT_ID_TCP 的行为
  都更加稳健。)

SOF_TIMESTAMPING_OPT_CMSG:
(SOF_TIMESTAMPING_OPT_CMSG)

  Support recv() cmsg for all timestamped packets. Control messages
  are already supported unconditionally on all packets with receive
  timestamps and on IPv6 packets with transmit timestamp. This option
  extends them to IPv4 packets with transmit timestamp. One use case
  is to correlate packets with their egress device, by enabling socket
  option IP_PKTINFO simultaneously.
  (为所有带时间戳的数据包支持 recv() cmsg。对于带接收时间戳的所有数据包以及
  带发送时间戳的 IPv6 数据包，控制消息已经无条件地受支持。该选项将其扩展到
  带发送时间戳的 IPv4 数据包。一个用例是同时启用套接字选项 IP_PKTINFO，把
  数据包与其发送设备关联起来。)


SOF_TIMESTAMPING_OPT_TSONLY:
(SOF_TIMESTAMPING_OPT_TSONLY)

  Applies to transmit timestamps only. Makes the kernel return the
  timestamp as a cmsg alongside an empty packet, as opposed to
  alongside the original packet. This reduces the amount of memory
  charged to the socket's receive budget (SO_RCVBUF) and delivers
  the timestamp even if sysctl net.core.tstamp_allow_data is 0.
  This option disables SOF_TIMESTAMPING_OPT_CMSG.
  (仅适用于发送时间戳。使内核把时间戳作为 cmsg 附带在一个空数据包上返回，
  而不是附带在原始数据包上。这减少了计入套接字接收预算 (SO_RCVBUF) 的内存量，
  并且即使 sysctl 项 net.core.tstamp_allow_data 为 0 也能交付时间戳。该选项会
  禁用 SOF_TIMESTAMPING_OPT_CMSG。)

SOF_TIMESTAMPING_OPT_STATS:
(SOF_TIMESTAMPING_OPT_STATS)

  Optional stats that are obtained along with the transmit timestamps.
  It must be used together with SOF_TIMESTAMPING_OPT_TSONLY. When the
  transmit timestamp is available, the stats are available in a
  separate control message of type SCM_TIMESTAMPING_OPT_STATS, as a
  list of TLVs (struct nlattr) of types. These stats allow the
  application to associate various transport layer stats with
  the transmit timestamps, such as how long a certain block of
  data was limited by peer's receiver window.
  (与发送时间戳一起获取的可选统计信息。必须与 SOF_TIMESTAMPING_OPT_TSONLY
  一起使用。当发送时间戳可用时，这些统计信息在类型为
  SCM_TIMESTAMPING_OPT_STATS 的单独控制消息中提供，形式为类型化的 TLV
  (struct nlattr) 列表。这些统计信息允许应用程序将各种传输层统计与发送时间戳
  关联起来，例如某段数据被对端接收窗口限制了多长时间。)

SOF_TIMESTAMPING_OPT_PKTINFO:
(SOF_TIMESTAMPING_OPT_PKTINFO)

  Enable the SCM_TIMESTAMPING_PKTINFO control message for incoming
  packets with hardware timestamps. The message contains struct
  scm_ts_pktinfo, which supplies the index of the real interface which
  received the packet and its length at layer 2. A valid (non-zero)
  interface index will be returned only if CONFIG_NET_RX_BUSY_POLL is
  enabled and the driver is using NAPI. The struct contains also two
  other fields, but they are reserved and undefined.
  (为带有硬件时间戳的传入数据包启用 SCM_TIMESTAMPING_PKTINFO 控制消息。该
  消息包含 struct scm_ts_pktinfo，提供实际接收该数据包的接口索引及其第 2 层
  长度。只有当 CONFIG_NET_RX_BUSY_POLL 已启用且驱动程序使用 NAPI 时，才会
  返回有效 (非零) 的接口索引。该结构还包含另外两个字段，但它们是保留且未定义
  的。)

SOF_TIMESTAMPING_OPT_TX_SWHW:
(SOF_TIMESTAMPING_OPT_TX_SWHW)

  Request both hardware and software timestamps for outgoing packets
  when SOF_TIMESTAMPING_TX_HARDWARE and SOF_TIMESTAMPING_TX_SOFTWARE
  are enabled at the same time. If both timestamps are generated,
  two separate messages will be looped to the socket's error queue,
  each containing just one timestamp.
  (当 SOF_TIMESTAMPING_TX_HARDWARE 和 SOF_TIMESTAMPING_TX_SOFTWARE 同时启用
  时，为发出的数据包同时请求硬件和软件时间戳。如果两种时间戳都生成了，将有两
  条单独的消息回环到套接字的错误队列，每条只包含一个时间戳。)

SOF_TIMESTAMPING_OPT_RX_FILTER:
(SOF_TIMESTAMPING_OPT_RX_FILTER)

  Filter out spurious receive timestamps: report a receive timestamp
  only if the matching timestamp generation flag is enabled.
  (过滤掉虚假的接收时间戳：只有在匹配的时间戳生成标志被启用时才报告接收
  时间戳。)

  Receive timestamps are generated early in the ingress path, before a
  packet's destination socket is known. If any socket enables receive
  timestamps, packets for all socket will receive timestamped packets.
  Including those that request timestamp reporting with
  SOF_TIMESTAMPING_SOFTWARE and/or SOF_TIMESTAMPING_RAW_HARDWARE, but
  do not request receive timestamp generation. This can happen when
  requesting transmit timestamps only.
  (接收时间戳在入站路径的早期生成，此时还不知道数据包的目的套接字。如果任何
  套接字启用了接收时间戳，所有套接字的数据包都会收到带时间戳的数据包，包括
  那些通过 SOF_TIMESTAMPING_SOFTWARE 和/或 SOF_TIMESTAMPING_RAW_HARDWARE
  请求时间戳报告、但未请求接收时间戳生成的套接字。只在请求发送时间戳时也可能
  出现这种情况。)

  Receiving spurious timestamps is generally benign. A process can
  ignore the unexpected non-zero value. But it makes behavior subtly
  dependent on other sockets. This flag isolates the socket for more
  deterministic behavior.
  (收到虚假时间戳通常无害，进程可以忽略这个意外的非零值。但这使行为微妙地
  依赖于其他套接字。该标志将套接字隔离出来，以获得更确定的行为。)

New applications are encouraged to pass SOF_TIMESTAMPING_OPT_ID to
disambiguate timestamps and SOF_TIMESTAMPING_OPT_TSONLY to operate
regardless of the setting of sysctl net.core.tstamp_allow_data.
(鼓励新应用程序传入 SOF_TIMESTAMPING_OPT_ID 以消除时间戳歧义，并传入
SOF_TIMESTAMPING_OPT_TSONLY，以便无论 sysctl 项 net.core.tstamp_allow_data
如何设置都能正常工作。)

An exception is when a process needs additional cmsg data, for
instance SOL_IP/IP_PKTINFO to detect the egress network interface.
Then pass option SOF_TIMESTAMPING_OPT_CMSG. This option depends on
having access to the contents of the original packet, so cannot be
combined with SOF_TIMESTAMPING_OPT_TSONLY.
(一个例外是进程需要额外的 cmsg 数据的情况，例如用 SOL_IP/IP_PKTINFO 来检测
发送网络接口。此时应传入选项 SOF_TIMESTAMPING_OPT_CMSG。该选项依赖于能够
访问原始数据包的内容，因此不能与 SOF_TIMESTAMPING_OPT_TSONLY 组合使用。)


1.3.4. Enabling timestamps via control messages(1.3.4. 通过控制消息启用时间戳)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In addition to socket options, timestamp generation can be requested
per write via cmsg, only for SOF_TIMESTAMPING_TX_* (see Section 1.3.1).
Using this feature, applications can sample timestamps per sendmsg()
without paying the overhead of enabling and disabling timestamps via
setsockopt::
(除了套接字选项之外，还可以通过 cmsg 为每次写操作请求生成时间戳，仅适用于
SOF_TIMESTAMPING_TX_* (见 1.3.1 节)。利用这一特性，应用程序可以按 sendmsg()
逐次采样时间戳，而无需承担通过 setsockopt 启用/禁用时间戳的开销：)

  struct msghdr *msg;
  ...
  cmsg			       = CMSG_FIRSTHDR(msg);
  cmsg->cmsg_level	       = SOL_SOCKET;
  cmsg->cmsg_type	       = SO_TIMESTAMPING;
  cmsg->cmsg_len	       = CMSG_LEN(sizeof(__u32));
  *((__u32 *) CMSG_DATA(cmsg)) = SOF_TIMESTAMPING_TX_SCHED |

				 SOF_TIMESTAMPING_TX_SOFTWARE |
				 SOF_TIMESTAMPING_TX_ACK;

  err = sendmsg(fd, msg, 0);

The SOF_TIMESTAMPING_TX_* flags set via cmsg will override
the SOF_TIMESTAMPING_TX_* flags set via setsockopt.
(通过 cmsg 设置的 SOF_TIMESTAMPING_TX_* 标志将覆盖通过 setsockopt 设置的
SOF_TIMESTAMPING_TX_* 标志。)

Moreover, applications must still enable timestamp reporting via
setsockopt to receive timestamps::
(此外，应用程序仍必须通过 setsockopt 启用时间戳报告才能收到时间戳：)

  __u32 val = SOF_TIMESTAMPING_SOFTWARE |

	      SOF_TIMESTAMPING_OPT_ID /* or any other flag */;

  err = setsockopt(fd, SOL_SOCKET, SO_TIMESTAMPING, &val, sizeof(val));


1.4 Bytestream Timestamps(1.4 字节流时间戳)
-------------------------------------------

The SO_TIMESTAMPING interface supports timestamping of bytes in a
bytestream. Each request is interpreted as a request for when the
entire contents of the buffer has passed a timestamping point. That
is, for streams option SOF_TIMESTAMPING_TX_SOFTWARE will record
when all bytes have reached the device driver, regardless of how
many packets the data has been converted into.
(SO_TIMESTAMPING 接口支持对字节流中的字节打时间戳。每个请求都被解释为对缓冲
区全部内容经过某个时间戳打点时刻的请求。也就是说，对于流式套接字，选项
SOF_TIMESTAMPING_TX_SOFTWARE 记录的是所有字节都已到达设备驱动程序的时刻，
无论这些数据已被转换成多少个数据包。)

In general, bytestreams have no natural delimiters and therefore
correlating a timestamp with data is non-trivial. A range of bytes
may be split across segments, any segments may be merged (possibly
coalescing sections of previously segmented buffers associated with
independent send() calls). Segments can be reordered and the same
byte range can coexist in multiple segments for protocols that
implement retransmissions.
(一般而言，字节流没有天然的定界符，因此把时间戳与数据关联起来并非易事。一段
字节可能被拆分到多个段中，任何段都可能被合并 (可能把与各自独立的 send() 调用
关联的、先前已分段的缓冲区片段拼接在一起)。段可能被重新排序，而且对于实现了
重传的协议，同一字节范围可能同时存在于多个段中。)

It is essential that all timestamps implement the same semantics,
regardless of these possible transformations, as otherwise they are
incomparable. Handling "rare" corner cases differently from the
simple case (a 1:1 mapping from buffer to skb) is insufficient
because performance debugging often needs to focus on such outliers.
(关键在于，所有时间戳都必须实现相同的语义，而不受上述可能变换的影响，否则它们
之间无法相互比较。对 "罕见" 边界情况采用与简单情况 (缓冲区到 skb 的 1:1 映射)
不同的处理方式是不够的，因为性能调试往往需要聚焦于这类离群情况。)

In practice, timestamps can be correlated with segments of a
bytestream consistently, if both semantics of the timestamp and the
timing of measurement are chosen correctly. This challenge is no
different from deciding on a strategy for IP fragmentation. There, the
definition is that only the first fragment is timestamped. For
bytestreams, we chose that a timestamp is generated only when all
bytes have passed a point. SOF_TIMESTAMPING_TX_ACK as defined is easy to
implement and reason about. An implementation that has to take into
account SACK would be more complex due to possible transmission holes
and out of order arrival.
(在实践中，只要时间戳的语义和测量时机的选择都正确，时间戳就能与字节流的各段
一致地关联起来。这一挑战与为 IP 分片确定策略并无不同。在那里，定义是只有第一
个分片被打时间戳。对于字节流，我们选择只在所有字节都通过某一点时才生成时间戳。
如定义所示，SOF_TIMESTAMPING_TX_ACK 易于实现和推理。必须考虑 SACK 的实现会
更复杂，因为可能出现传输空洞和乱序到达。)

On the host, TCP can also break the simple 1:1 mapping from buffer to
skbuff as a result of Nagle, cork, autocork, segmentation and GSO. The
implementation ensures correctness in all cases by tracking the
individual last byte passed to send(), even if it is no longer the
last byte after an skbuff extend or merge operation. It stores the
relevant sequence number in skb_shinfo(skb)->tskey. Because an skbuff
has only one such field, only one timestamp can be generated.
(在主机上，TCP 也可能因 Nagle、cork、autocork、分段和 GSO 而打破缓冲区到
skbuff 的简单 1:1 映射。本实现通过跟踪传递给 send() 的最后一个字节来保证在
所有情况下都正确，即使经过 skbuff 扩展或合并操作后它不再是最后一个字节。
相关序号保存在 skb_shinfo(skb)->tskey 中。由于一个 skbuff 只有一个这样的
字段，因此只能生成一个时间戳。)

In rare cases, a timestamp request can be missed if two requests are
collapsed onto the same skb. A process can detect this situation by
enabling SOF_TIMESTAMPING_OPT_ID and comparing the byte offset at
send time with the value returned for each timestamp. It can prevent
the situation by always flushing the TCP stack in between requests,
for instance by enabling TCP_NODELAY and disabling TCP_CORK and
autocork. After linux-4.7, a better way to prevent coalescing is
to use MSG_EOR flag at sendmsg() time.
(在极少数情况下，如果两个请求被合并到同一个 skb 上，时间戳请求可能会丢失。
进程可以通过启用 SOF_TIMESTAMPING_OPT_ID，并把发送时的字节偏移与每个时间戳
返回的值进行比较来检测这种情况。要避免这种情况，可以在两次请求之间总是刷新
TCP 栈，例如启用 TCP_NODELAY 并禁用 TCP_CORK 和 autocork。在 linux-4.7 之后，
防止合并的更好方法是在 sendmsg() 时使用 MSG_EOR 标志。)

These precautions ensure that the timestamp is generated only when all
bytes have passed a timestamp point, assuming that the network stack
itself does not reorder the segments. The stack indeed tries to avoid
reordering. The one exception is under administrator control: it is
possible to construct a packet scheduler configuration that delays
segments from the same stream differently. Such a setup would be
unusual.
(这些预防措施确保只有当所有字节都经过时间戳打点时才生成时间戳，前提是网络栈
自身不重排各个段。栈确实会尽量避免重排。唯一的例外处于管理员控制之下：可能
构造出一个对来自同一流的各段施加不同延迟的数据包调度器配置，不过这样的设置
并不常见。)


2 Data Interfaces(2 数据接口)
=============================

Timestamps are read using the ancillary data feature of recvmsg().
See `man 3 cmsg` for details of this interface. The socket manual
page (`man 7 socket`) describes how timestamps generated with
SO_TIMESTAMP and SO_TIMESTAMPNS records can be retrieved.
(时间戳通过 recvmsg() 的辅助数据特性读取。该接口的细节参见 `man 3 cmsg`。
套接字手册页 (`man 7 socket`) 描述了如何读取以 SO_TIMESTAMP 和
SO_TIMESTAMPNS 记录生成的时间戳。)


2.1 SCM_TIMESTAMPING records(2.1 SCM_TIMESTAMPING 记录)
-------------------------------------------------------

These timestamps are returned in a control message with cmsg_level
SOL_SOCKET, cmsg_type SCM_TIMESTAMPING, and payload of type
(这些时间戳通过一条 cmsg_level 为 SOL_SOCKET、cmsg_type 为 SCM_TIMESTAMPING
的控制消息返回，其载荷类型如下)

For SO_TIMESTAMPING_OLD::
(对于 SO_TIMESTAMPING_OLD：)

	struct scm_timestamping {
		struct timespec ts[3];
	};

For SO_TIMESTAMPING_NEW::
(对于 SO_TIMESTAMPING_NEW：)

	struct scm_timestamping64 {
		struct __kernel_timespec ts[3];

Always use SO_TIMESTAMPING_NEW timestamp to always get timestamp in
struct scm_timestamping64 format.
(请始终使用 SO_TIMESTAMPING_NEW 时间戳，以始终获得 struct scm_timestamping64
格式的时间戳。)

SO_TIMESTAMPING_OLD returns incorrect timestamps after the year 2038
on 32 bit machines.
(SO_TIMESTAMPING_OLD 在 32 位机器上于 2038 年之后会返回不正确的时间戳。)

The structure can return up to three timestamps. This is a legacy
feature. At least one field is non-zero at any time. Most timestamps
are passed in ts[0]. Hardware timestamps are passed in ts[2].
(该结构最多可返回三个时间戳。这是一个历史遗留特性。任何时刻至少有一个字段
非零。大多数时间戳在 ts[0] 中传递。硬件时间戳在 ts[2] 中传递。)

ts[1] used to hold hardware timestamps converted to system time.
Instead, expose the hardware clock device on the NIC directly as
a HW PTP clock source, to allow time conversion in userspace and
optionally synchronize system time with a userspace PTP stack such
as linuxptp. For the PTP clock API, see Documentation/driver-api/ptp.rst.
(ts[1] 曾用于保存换算成系统时间的硬件时间戳。现在，应改为把网卡上的硬件时钟
设备直接作为硬件 PTP 时钟源暴露出来，以便在用户空间进行时间换算，并可选择
使用 linuxptp 等用户空间 PTP 协议栈来同步系统时间。PTP 时钟 API 参见
Documentation/driver-api/ptp.rst。)

Note that if the SO_TIMESTAMP or SO_TIMESTAMPNS option is enabled
together with SO_TIMESTAMPING using SOF_TIMESTAMPING_SOFTWARE, a false
software timestamp will be generated in the recvmsg() call and passed
in ts[0] when a real software timestamp is missing. This happens also
on hardware transmit timestamps.
(注意，如果 SO_TIMESTAMP 或 SO_TIMESTAMPNS 选项与使用
SOF_TIMESTAMPING_SOFTWARE 的 SO_TIMESTAMPING 一起启用，那么当缺少真正的软件
时间戳时，recvmsg() 调用中会生成一个虚假的软件时间戳并在 ts[0] 中传递。在硬件
发送时间戳上也会发生这种情况。)

2.1.1 Transmit timestamps with MSG_ERRQUEUE(2.1.1 通过 MSG_ERRQUEUE 获取发送时间戳)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For transmit timestamps the outgoing packet is looped back to the
socket's error queue with the send timestamp(s) attached. A process
receives the timestamps by calling recvmsg() with flag MSG_ERRQUEUE
set and with a msg_control buffer sufficiently large to receive the
relevant metadata structures. The recvmsg call returns the original
outgoing data packet with two ancillary messages attached.
(对于发送时间戳，发出的数据包会附带发送时间戳回环到套接字的错误队列。进程在
置位 MSG_ERRQUEUE 标志并准备一个足以容纳相关元数据结构的 msg_control 缓冲区
的情况下调用 recvmsg()，即可接收这些时间戳。recvmsg 调用返回原始的发出数据包，
附带两条辅助消息。)

A message of cm_level SOL_IP(V6) and cm_type IP(V6)_RECVERR
embeds a struct sock_extended_err. This defines the error type. For
timestamps, the ee_errno field is ENOMSG. The other ancillary message
will have cm_level SOL_SOCKET and cm_type SCM_TIMESTAMPING. This
embeds the struct scm_timestamping.
(一条 cm_level 为 SOL_IP(V6)、cm_type 为 IP(V6)_RECVERR 的消息内嵌一个
struct sock_extended_err，它定义了错误类型。对于时间戳，ee_errno 字段为
ENOMSG。另一条辅助消息的 cm_level 为 SOL_SOCKET、cm_type 为 SCM_TIMESTAMPING，
其中内嵌 struct scm_timestamping。)


2.1.1.2 Timestamp types(2.1.1.2 时间戳类型)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The semantics of the three struct timespec are defined by field
ee_info in the extended error structure. It contains a value of
type SCM_TSTAMP_* to define the actual timestamp passed in
scm_timestamping.
(三个 struct timespec 的语义由扩展错误结构中的 ee_info 字段定义。它包含一个
SCM_TSTAMP_* 类型的值，用于定义在 scm_timestamping 中传递的实际时间戳。)

The SCM_TSTAMP_* types are 1:1 matches to the SOF_TIMESTAMPING_*
control fields discussed previously, with one exception. For legacy
reasons, SCM_TSTAMP_SND is equal to zero and can be set for both
SOF_TIMESTAMPING_TX_HARDWARE and SOF_TIMESTAMPING_TX_SOFTWARE. It
is the first if ts[2] is non-zero, the second otherwise, in which
case the timestamp is stored in ts[0].
(SCM_TSTAMP_* 类型与前文讨论的 SOF_TIMESTAMPING_* 控制字段一一对应，只有
一个例外。由于历史原因，SCM_TSTAMP_SND 等于零，可同时对应
SOF_TIMESTAMPING_TX_HARDWARE 和 SOF_TIMESTAMPING_TX_SOFTWARE 两者。若
ts[2] 非零则为前者，否则为后者，此时时间戳保存在 ts[0] 中。)


2.1.1.3 Fragmentation(2.1.1.3 分片)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Fragmentation of outgoing datagrams is rare, but is possible, e.g., by
explicitly disabling PMTU discovery. If an outgoing packet is fragmented,
then only the first fragment is timestamped and returned to the sending
socket.
(发出数据报的分片很少见，但确实可能发生，例如显式禁用 PMTU 发现时。如果发出
的数据包被分片，那么只有第一个分片会被打时间戳并返回给发送套接字。)


2.1.1.4 Packet Payload(2.1.1.4 数据包载荷)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The calling application is often not interested in receiving the whole
packet payload that it passed to the stack originally: the socket
error queue mechanism is just a method to piggyback the timestamp on.
In this case, the application can choose to read datagrams with a
smaller buffer, possibly even of length 0. The payload is truncated
accordingly. Until the process calls recvmsg() on the error queue,
however, the full packet is queued, taking up budget from SO_RCVBUF.
(调用应用程序通常并不关心接收当初交给协议栈的完整数据包载荷：套接字错误队列
机制只是搭车传递时间戳的一种手段。这种情况下，应用程序可以选择用更小的缓冲区
(甚至长度为 0) 来读取数据报，载荷会被相应截断。但在进程对错误队列调用
recvmsg() 之前，完整的数据包都会被排队，占用 SO_RCVBUF 的额度。)


2.1.1.5 Blocking Read(2.1.1.5 阻塞读取)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Reading from the error queue is always a non-blocking operation. To
block waiting on a timestamp, use poll or select. poll() will return
POLLERR in pollfd.revents if any data is ready on the error queue.
There is no need to pass this flag in pollfd.events. This flag is
ignored on request. See also `man 2 poll`.
(从错误队列读取始终是非阻塞操作。要阻塞等待时间戳，请使用 poll 或 select。
只要错误队列上有数据就绪，poll() 就会在 pollfd.revents 中返回 POLLERR。无需
在 pollfd.events 中传入该标志，它在请求时会被忽略。另见 `man 2 poll`。)


2.1.2 Receive timestamps(2.1.2 接收时间戳)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On reception, there is no reason to read from the socket error queue.
The SCM_TIMESTAMPING ancillary data is sent along with the packet data
on a normal recvmsg(). Since this is not a socket error, it is not
accompanied by a message SOL_IP(V6)/IP(V6)_RECVERROR. In this case,
the meaning of the three fields in struct scm_timestamping is
implicitly defined. ts[0] holds a software timestamp if set, ts[1]
is again deprecated and ts[2] holds a hardware timestamp if set.
(在接收方向，没有必要从套接字错误队列读取。SCM_TIMESTAMPING 辅助数据在普通
recvmsg() 中随数据包数据一起送达。由于这不是套接字错误，因此不会伴随
SOL_IP(V6)/IP(V6)_RECVERROR 消息。这种情况下，struct scm_timestamping 三个
字段的含义是隐式定义的：ts[0] 在置位时保存软件时间戳，ts[1] 同样已废弃，
ts[2] 在置位时保存硬件时间戳。)


3. Hardware Timestamping configuration: ETHTOOL_MSG_TSCONFIG_SET/GET(3. 硬件时间戳配置：ETHTOOL_MSG_TSCONFIG_SET/GET)
=====================================================================================================================

Hardware time stamping must also be initialized for each device driver
that is expected to do hardware time stamping. The parameter is defined in
include/uapi/linux/net_tstamp.h as::
(对于预期要进行硬件时间戳打点的每个设备驱动程序，也必须初始化硬件时间戳。
该参数定义在 include/uapi/linux/net_tstamp.h 中，如下所示：)

	struct hwtstamp_config {
		int flags;	/* no flags defined right now, must be zero */
		int tx_type;	/* HWTSTAMP_TX_* */
		int rx_filter;	/* HWTSTAMP_FILTER_* */
	};

Desired behavior is passed into the kernel and to a specific device by
calling the tsconfig netlink socket ``ETHTOOL_MSG_TSCONFIG_SET``.
The ``ETHTOOL_A_TSCONFIG_TX_TYPES``, ``ETHTOOL_A_TSCONFIG_RX_FILTERS`` and
``ETHTOOL_A_TSCONFIG_HWTSTAMP_FLAGS`` netlink attributes are then used to set
the struct hwtstamp_config accordingly.
(期望的行为通过调用 tsconfig netlink 套接字 ``ETHTOOL_MSG_TSCONFIG_SET`` 传入
内核和特定设备。随后使用 ``ETHTOOL_A_TSCONFIG_TX_TYPES``、
``ETHTOOL_A_TSCONFIG_RX_FILTERS`` 和 ``ETHTOOL_A_TSCONFIG_HWTSTAMP_FLAGS`` 这些
netlink 属性来相应地设置 struct hwtstamp_config。)

The ``ETHTOOL_A_TSCONFIG_HWTSTAMP_PROVIDER`` netlink nested attribute is used
to select the source of the hardware time stamping. It is composed of an index
for the device source and a qualifier for the type of time stamping.
(netlink 嵌套属性 ``ETHTOOL_A_TSCONFIG_HWTSTAMP_PROVIDER`` 用于选择硬件时间戳
的来源。它由设备源的索引和时间戳类型的限定符组成。)

Drivers are free to use a more permissive configuration than the requested
configuration. It is expected that drivers should only implement directly the
most generic mode that can be supported. For example if the hardware can
support HWTSTAMP_FILTER_PTP_V2_EVENT, then it should generally always upscale
HWTSTAMP_FILTER_PTP_V2_L2_SYNC, and so forth, as HWTSTAMP_FILTER_PTP_V2_EVENT
is more generic (and more useful to applications).
(驱动程序可以使用比所请求配置更宽松的配置。预期驱动程序应当只直接实现所能
支持的最通用模式。例如，如果硬件可以支持 HWTSTAMP_FILTER_PTP_V2_EVENT，那么
它通常应当始终把 HWTSTAMP_FILTER_PTP_V2_L2_SYNC 等向上兼容处理，因为
HWTSTAMP_FILTER_PTP_V2_EVENT 更通用 (对应用程序也更有用)。)

A driver which supports hardware time stamping shall update the struct
with the actual, possibly more permissive configuration. If the
requested packets cannot be time stamped, then nothing should be
changed and ERANGE shall be returned (in contrast to EINVAL, which
indicates that SIOCSHWTSTAMP is not supported at all).
(支持硬件时间戳的驱动程序应当用实际的、可能更宽松的配置来更新该结构体。如果
所请求的数据包无法被打时间戳，则不应做任何更改，并返回 ERANGE (与之相对，
EINVAL 表示完全不支持 SIOCSHWTSTAMP)。)

Only a processes with admin rights may change the configuration. User
space is responsible to ensure that multiple processes don't interfere
with each other and that the settings are reset.
(只有具有管理员权限的进程才可以更改配置。用户空间负责确保多个进程之间互不
干扰，并负责重置设置。)

Any process can read the actual configuration by requesting tsconfig netlink
socket ``ETHTOOL_MSG_TSCONFIG_GET``.
(任何进程都可以通过请求 tsconfig netlink 套接字 ``ETHTOOL_MSG_TSCONFIG_GET``
来读取实际配置。)

The legacy configuration is the use of the ioctl(SIOCSHWTSTAMP) with a pointer
to a struct ifreq whose ifr_data points to a struct hwtstamp_config.
The tx_type and rx_filter are hints to the driver what it is expected to do.
If the requested fine-grained filtering for incoming packets is not
supported, the driver may time stamp more than just the requested types
of packets. ioctl(SIOCGHWTSTAMP) is used in the same way as the
ioctl(SIOCSHWTSTAMP). However, this has not been implemented in all drivers.
(旧式配置方式是使用 ioctl(SIOCSHWTSTAMP)，传入一个指向 struct ifreq 的指针，
其 ifr_data 指向一个 struct hwtstamp_config。tx_type 和 rx_filter 是给驱动
程序的提示，告知它预期要做什么。如果不支持所请求的传入数据包细粒度过滤，驱动
程序可能对超出所请求类型的数据包也打时间戳。ioctl(SIOCGHWTSTAMP) 的用法与
ioctl(SIOCSHWTSTAMP) 相同。不过，并非所有驱动程序都实现了它。)

::

    /* possible values for hwtstamp_config->tx_type */
    enum {
	    /*
	    * no outgoing packet will need hardware time stamping;
	    * should a packet arrive which asks for it, no hardware
	    * time stamping will be done
	    */
	    HWTSTAMP_TX_OFF,

	    /*
	    * enables hardware time stamping for outgoing packets;
	    * the sender of the packet decides which are to be
	    * time stamped by setting SOF_TIMESTAMPING_TX_SOFTWARE
	    * before sending the packet
	    */
	    HWTSTAMP_TX_ON,
    };

    /* possible values for hwtstamp_config->rx_filter */
    enum {
	    /* time stamp no incoming packet at all */
	    HWTSTAMP_FILTER_NONE,

	    /* time stamp any incoming packet */
	    HWTSTAMP_FILTER_ALL,

	    /* return value: time stamp all packets requested plus some others */
	    HWTSTAMP_FILTER_SOME,

	    /* PTP v1, UDP, any kind of event packet */
	    HWTSTAMP_FILTER_PTP_V1_L4_EVENT,

	    /* for the complete list of values, please check
	    * the include file include/uapi/linux/net_tstamp.h
	    */
    };

3.1 Hardware Timestamping Implementation: Device Drivers(3.1 硬件时间戳实现：设备驱动程序)
------------------------------------------------------------------------------------------

A driver which supports hardware time stamping must support the
ndo_hwtstamp_set NDO and update the supplied struct hwtstamp_config with
the actual values as described in the section on SIOCSHWTSTAMP. It
should also support ndo_hwtstamp_get NDO to retrieve configuration.
(支持硬件时间戳的驱动程序必须支持 ndo_hwtstamp_set NDO，并按照 SIOCSHWTSTAMP
一节所述，用实际值更新传入的 struct hwtstamp_config。它还应支持
ndo_hwtstamp_get NDO 以获取配置。)

Time stamps for received packets must be stored in the skb. To get a pointer
to the shared time stamp structure of the skb call skb_hwtstamps(). Then
set the time stamps in the structure::
(接收数据包的时间戳必须保存在 skb 中。调用 skb_hwtstamps() 可获得指向该 skb
共享时间戳结构的指针，然后在结构中设置时间戳：)

    struct skb_shared_hwtstamps {
	    /* hardware time stamp transformed into duration
	    * since arbitrary point in time
	    */
	    ktime_t	hwtstamp;
    };

Time stamps for outgoing packets are to be generated as follows:
(发出数据包的时间戳应按如下方式生成：)

- In hard_start_xmit(), check if (skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP)
  is set no-zero. If yes, then the driver is expected to do hardware time
  stamping.
  (在 hard_start_xmit() 中，检查 (skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP)
  是否非零。如果是，则表示驱动程序需要进行硬件时间戳打点。)
- If this is possible for the skb and requested, then declare
  that the driver is doing the time stamping by setting the flag
  SKBTX_IN_PROGRESS in skb_shinfo(skb)->tx_flags , e.g. with::
  (如果该 skb 可以这样做且已被请求，则通过在 skb_shinfo(skb)->tx_flags 中设置
  SKBTX_IN_PROGRESS 标志来声明驱动程序正在进行时间戳打点，例如：)

      skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;

  You might want to keep a pointer to the associated skb for the next step
  and not free the skb. A driver not supporting hardware time stamping doesn't
  do that. A driver must never touch sk_buff::tstamp! It is used to store
  software generated time stamps by the network subsystem.
  (您可能想在下一步中保留指向相关 skb 的指针而不释放该 skb。不支持硬件时间戳
  的驱动程序不会这样做。驱动程序绝不能碰 sk_buff::tstamp！它被网络子系统用来
  存储软件生成的时间戳。)
- Driver should call skb_tx_timestamp() as close to passing sk_buff to hardware
  as possible. skb_tx_timestamp() provides a software time stamp if requested
  and hardware timestamping is not possible (SKBTX_IN_PROGRESS not set).
  (驱动程序应尽可能在靠近把 sk_buff 传递给硬件的时机调用 skb_tx_timestamp()。
  如果已请求且无法进行硬件时间戳打点 (未设置 SKBTX_IN_PROGRESS)，则
  skb_tx_timestamp() 会提供软件时间戳。)
- As soon as the driver has sent the packet and/or obtained a
  hardware time stamp for it, it passes the time stamp back by
  calling skb_tstamp_tx() with the original skb, the raw
  hardware time stamp. skb_tstamp_tx() clones the original skb and
  adds the timestamps, therefore the original skb has to be freed now.
  If obtaining the hardware time stamp somehow fails, then the driver
  should not fall back to software time stamping. The rationale is that
  this would occur at a later time in the processing pipeline than other
  software time stamping and therefore could lead to unexpected deltas
  between time stamps.
  (一旦驱动程序发送了数据包和/或为它取得了硬件时间戳，就应通过以原始 skb 和
  原始硬件时间戳为参数调用 skb_tstamp_tx() 把时间戳传回。skb_tstamp_tx() 会
  克隆原始 skb 并添加时间戳，因此此时必须释放原始 skb。如果获取硬件时间戳因
  某种原因失败，驱动程序不应回退到软件时间戳。理由是：这将发生在处理流水线中
  比其他软件时间戳更靠后的时刻，因而可能导致时间戳之间出现意外的偏差。)

3.2 Special considerations for stacked PTP Hardware Clocks(3.2 堆叠 PTP 硬件时钟的特殊注意事项)
-----------------------------------------------------------------------------------------------

There are situations when there may be more than one PHC (PTP Hardware Clock)
in the data path of a packet. The kernel has no explicit mechanism to allow the
user to select which PHC to use for timestamping Ethernet frames. Instead, the
assumption is that the outermost PHC is always the most preferable, and that
kernel drivers collaborate towards achieving that goal. Currently there are 3
cases of stacked PHCs, detailed below:
(在某些情况下，数据包的传输路径上可能存在不止一个 PHC (PTP 硬件时钟)。内核没有
提供让用户选择用哪个 PHC 给以太网帧打时间戳的显式机制，而是假定最外层的 PHC
总是最优选择，并由各内核驱动程序协作实现这一目标。目前有 3 种堆叠 PHC 的情况，
详述如下：)

3.2.1 DSA (Distributed Switch Architecture) switches(3.2.1 DSA (分布式交换机架构) 交换机)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These are Ethernet switches which have one of their ports connected to an
(otherwise completely unaware) host Ethernet interface, and perform the role of
a port multiplier with optional forwarding acceleration features.  Each DSA
switch port is visible to the user as a standalone (virtual) network interface,
and its network I/O is performed, under the hood, indirectly through the host
interface (redirecting to the host port on TX, and intercepting frames on RX).
(这类以太网交换机将其中的一个端口连接到 (对此完全不知情的) 主机以太网接口上，
扮演端口复用器 (port multiplier) 的角色，并可选地带有转发加速特性。每个 DSA
交换机端口对用户而言都是一个独立的 (虚拟) 网络接口，其网络 I/O 在底层通过主机
接口间接完成 (TX 方向重定向到主机端口，RX 方向拦截帧)。)

When a DSA switch is attached to a host port, PTP synchronization has to
suffer, since the switch's variable queuing delay introduces a path delay
jitter between the host port and its PTP partner. For this reason, some DSA
switches include a timestamping clock of their own, and have the ability to
perform network timestamping on their own MAC, such that path delays only
measure wire and PHY propagation latencies. Timestamping DSA switches are
supported in Linux and expose the same ABI as any other network interface (save
for the fact that the DSA interfaces are in fact virtual in terms of network
I/O, they do have their own PHC).  It is typical, but not mandatory, for all
interfaces of a DSA switch to share the same PHC.
(当 DSA 交换机连接到主机端口时，PTP 同步质量会受到影响，因为交换机可变的排队
延迟会在主机端口与其 PTP 对端之间引入路径延迟抖动。因此，一些 DSA 交换机自带
时间戳时钟，并有能力在自己的 MAC 上执行网络时间戳打点，使路径延迟只度量线路
和 PHY 的传播延迟。Linux 支持带时间戳功能的 DSA 交换机，它们暴露与其他任何
网络接口相同的 ABI (尽管 DSA 接口在网络 I/O 方面实际上是虚拟的，但它们确实
拥有自己的 PHC)。DSA 交换机的所有接口共享同一个 PHC 是常见做法，但并非强制。)

By design, PTP timestamping with a DSA switch does not need any special
handling in the driver for the host port it is attached to.  However, when the
host port also supports PTP timestamping, DSA will take care of intercepting
the ``.ndo_eth_ioctl`` calls towards the host port, and block attempts to enable
hardware timestamping on it. This is because the SO_TIMESTAMPING API does not
allow the delivery of multiple hardware timestamps for the same packet, so
anybody else except for the DSA switch port must be prevented from doing so.
(按照设计，对 DSA 交换机进行 PTP 时间戳打点时，其所在主机端口的驱动程序无需
任何特殊处理。然而，当主机端口也支持 PTP 时间戳打点时，DSA 会负责拦截发往
主机端口的 ``.ndo_eth_ioctl`` 调用，并阻止在该端口上启用硬件时间戳的尝试。
这是因为 SO_TIMESTAMPING API 不允许为同一个数据包交付多个硬件时间戳，因此
必须防止除 DSA 交换机端口之外的任何实体这样做。)

In the generic layer, DSA provides the following infrastructure for PTP
timestamping:
(在通用层，DSA 为 PTP 时间戳打点提供了以下基础设施：)

- ``.port_txtstamp()``: a hook called prior to the transmission of
  packets with a hardware TX timestamping request from user space.
  This is required for two-step timestamping, since the hardware
  timestamp becomes available after the actual MAC transmission, so the
  driver must be prepared to correlate the timestamp with the original
  packet so that it can re-enqueue the packet back into the socket's
  error queue. To save the packet for when the timestamp becomes
  available, the driver can call ``skb_clone_sk`` , save the clone pointer
  in skb->cb and enqueue a tx skb queue. Typically, a switch will have a
  PTP TX timestamp register (or sometimes a FIFO) where the timestamp
  becomes available. In case of a FIFO, the hardware might store
  key-value pairs of PTP sequence ID/message type/domain number and the
  actual timestamp. To perform the correlation correctly between the
  packets in a queue waiting for timestamping and the actual timestamps,
  drivers can use a BPF classifier (``ptp_classify_raw``) to identify
  the PTP transport type, and ``ptp_parse_header`` to interpret the PTP
  header fields. There may be an IRQ that is raised upon this
  timestamp's availability, or the driver might have to poll after
  invoking ``dev_queue_xmit()`` towards the host interface.
  One-step TX timestamping do not require packet cloning, since there is
  no follow-up message required by the PTP protocol (because the
  TX timestamp is embedded into the packet by the MAC), and therefore
  user space does not expect the packet annotated with the TX timestamp
  to be re-enqueued into its socket's error queue.
  (``.port_txtstamp()``：在发送带有用户空间硬件 TX 时间戳请求的数据包之前调用
  的钩子。两步 (two-step) 时间戳需要它，因为硬件时间戳要等到实际 MAC 发送之后
  才可用，所以驱动程序必须准备好把时间戳与原始数据包关联起来，以便把该数据包
  重新入队到套接字的错误队列。为了在时间戳变为可用时保存该数据包，驱动程序
  可以调用 ``skb_clone_sk``，把克隆指针保存在 skb->cb 中，并加入一个 tx skb
  队列。通常，交换机会有一个 PTP TX 时间戳寄存器 (有时是 FIFO)，时间戳在其中
  变为可用。对于 FIFO，硬件可能存储 PTP 序列 ID/消息类型/域号与实际时间戳的
  键值对。为了在队列中等待时间戳的数据包与实际时间戳之间正确完成关联，驱动
  程序可以使用 BPF 分类器 (``ptp_classify_raw``) 识别 PTP 传输类型，并用
  ``ptp_parse_header`` 解析 PTP 头部字段。可能有中断在该时间戳可用时触发，
  也可能驱动程序需要在向主机接口调用 ``dev_queue_xmit()`` 之后轮询。单步
  (one-step) TX 时间戳不需要克隆数据包，因为 PTP 协议不需要后续报文 (TX 时间戳
  由 MAC 嵌入数据包中)，因此用户空间不会期望带 TX 时间戳注释的数据包被重新
  入队到其套接字的错误队列。)

- ``.port_rxtstamp()``: On RX, the BPF classifier is run by DSA to
  identify PTP event messages (any other packets, including PTP general
  messages, are not timestamped). The original (and only) timestampable
  skb is provided to the driver, for it to annotate it with a timestamp,
  if that is immediately available, or defer to later. On reception,
  timestamps might either be available in-band (through metadata in the
  DSA header, or attached in other ways to the packet), or out-of-band
  (through another RX timestamping FIFO). Deferral on RX is typically
  necessary when retrieving the timestamp needs a sleepable context. In
  that case, it is the responsibility of the DSA driver to call
  ``netif_rx()`` on the freshly timestamped skb.
  (``.port_rxtstamp()``：在 RX 方向，DSA 会运行 BPF 分类器来识别 PTP 事件
  报文 (其他任何数据包，包括 PTP 通用报文，都不打时间戳)。原始的 (且唯一的)
  可打时间戳的 skb 会提供给驱动程序，让它为该 skb 加上时间戳注释——如果时间戳
  立即可用的话——或者推迟到稍后处理。在接收方向，时间戳可能是带内的 (通过 DSA
  头部中的元数据，或以其他方式附在数据包上)，也可能是带外的 (通过另一个 RX
  时间戳 FIFO)。当获取时间戳需要可睡眠上下文时，RX 方向通常需要推迟处理。
  那种情况下，由 DSA 驱动程序负责对打好时间戳的新 skb 调用 ``netif_rx()``。)

3.2.2 Ethernet PHYs(3.2.2 以太网 PHY)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These are devices that typically fulfill a Layer 1 role in the network stack,
hence they do not have a representation in terms of a network interface as DSA
switches do. However, PHYs may be able to detect and timestamp PTP packets, for
performance reasons: timestamps taken as close as possible to the wire have the
potential to yield a more stable and precise synchronization.
(这类设备通常在网络栈中承担第 1 层的角色，因此不像 DSA 交换机那样拥有网络
接口形式的表示。不过，出于性能原因，PHY 可能能够检测 PTP 数据包并为其打时间
戳：在尽可能靠近线路处取得的时间戳，有望带来更稳定、更精确的同步。)

A PHY driver that supports PTP timestamping must create a ``struct
mii_timestamper`` and add a pointer to it in ``phydev->mii_ts``. The presence
of this pointer will be checked by the networking stack.
(支持 PTP 时间戳的 PHY 驱动程序必须创建一个 ``struct mii_timestamper``，并把
指向它的指针加入 ``phydev->mii_ts``。网络栈会检查该指针是否存在。)

Since PHYs do not have network interface representations, the timestamping and
ethtool ioctl operations for them need to be mediated by their respective MAC
driver.  Therefore, as opposed to DSA switches, modifications need to be done
to each individual MAC driver for PHY timestamping support. This entails:
(由于 PHY 没有网络接口表示，其时间戳和 ethtool ioctl 操作需要由各自的 MAC
驱动程序来中转。因此与 DSA 交换机不同，要支持 PHY 时间戳，需要对每个 MAC
驱动程序单独进行修改。这包括：)

- Checking, in ``.ndo_eth_ioctl``, whether ``phy_has_hwtstamp(netdev->phydev)``
  is true or not. If it is, then the MAC driver should not process this request
  but instead pass it on to the PHY using ``phy_mii_ioctl()``.
  (在 ``.ndo_eth_ioctl`` 中检查 ``phy_has_hwtstamp(netdev->phydev)`` 是否为真。
  如果为真，MAC 驱动程序不应自己处理该请求，而应使用 ``phy_mii_ioctl()`` 把它
  转交给 PHY。)

- On RX, special intervention may or may not be needed, depending on the
  function used to deliver skb's up the network stack. In the case of plain
  ``netif_rx()`` and similar, MAC drivers must check whether
  ``skb_defer_rx_timestamp(skb)`` is necessary or not - and if it is, don't
  call ``netif_rx()`` at all.  If ``CONFIG_NETWORK_PHY_TIMESTAMPING`` is
  enabled, and ``skb->dev->phydev->mii_ts`` exists, its ``.rxtstamp()`` hook
  will be called now, to determine, using logic very similar to DSA, whether
  deferral for RX timestamping is necessary.  Again like DSA, it becomes the
  responsibility of the PHY driver to send the packet up the stack when the
  timestamp is available.
  (在 RX 方向，是否需要特殊干预取决于用哪个函数把 skb 沿网络栈向上交付。对于
  简单的 ``netif_rx()`` 及类似函数，MAC 驱动程序必须检查是否需要
  ``skb_defer_rx_timestamp(skb)``——如果需要，就完全不要调用 ``netif_rx()``。
  如果 ``CONFIG_NETWORK_PHY_TIMESTAMPING`` 已启用且 ``skb->dev->phydev->mii_ts``
  存在，此时会调用其 ``.rxtstamp()`` 钩子，用与 DSA 非常相似的逻辑判断是否需要
  推迟 RX 时间戳打点。与 DSA 一样，在时间戳可用时把数据包沿栈向上交付就成为
  PHY 驱动程序的责任。)

  For other skb receive functions, such as ``napi_gro_receive`` and
  ``netif_receive_skb``, the stack automatically checks whether
  ``skb_defer_rx_timestamp()`` is necessary, so this check is not needed inside
  the driver.
  (对于其他 skb 接收函数，例如 ``napi_gro_receive`` 和 ``netif_receive_skb``，
  栈会自动检查是否需要 ``skb_defer_rx_timestamp()``，因此驱动程序内部无需做
  这个检查。)

- On TX, again, special intervention might or might not be needed.  The
  function that calls the ``mii_ts->txtstamp()`` hook is named
  ``skb_clone_tx_timestamp()``. This function can either be called directly
  (case in which explicit MAC driver support is indeed needed), but the
  function also piggybacks from the ``skb_tx_timestamp()`` call, which many MAC
  drivers already perform for software timestamping purposes. Therefore, if a
  MAC supports software timestamping, it does not need to do anything further
  at this stage.
  (在 TX 方向，同样可能需要、也可能不需要特殊干预。调用 ``mii_ts->txtstamp()``
  钩子的函数名为 ``skb_clone_tx_timestamp()``。该函数可以被直接调用 (这种情况下
  确实需要 MAC 驱动程序显式支持)，但它也会搭 ``skb_tx_timestamp()`` 调用的便车
  被间接调用——许多 MAC 驱动程序出于软件时间戳的目的已经在调用它。因此，如果
  MAC 支持软件时间戳，在这一阶段无需再做任何其他事情。)

3.2.3 MII bus snooping devices(3.2.3 MII 总线侦听设备)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These perform the same role as timestamping Ethernet PHYs, save for the fact
that they are discrete devices and can therefore be used in conjunction with
any PHY even if it doesn't support timestamping. In Linux, they are
discoverable and attachable to a ``struct phy_device`` through Device Tree, and
for the rest, they use the same mii_ts infrastructure as those. See
Documentation/devicetree/bindings/ptp/timestamper.txt for more details.
(这类设备与带时间戳功能的以太网 PHY 扮演相同的角色，区别在于它们是独立设备，
因此即使 PHY 本身不支持时间戳，也可以与之配合使用。在 Linux 中，它们可以通过
设备树 (Device Tree) 发现并附加到 ``struct phy_device``，除此之外，它们使用的
是与 PHY 相同的 mii_ts 基础设施。更多信息参见
Documentation/devicetree/bindings/ptp/timestamper.txt。)

3.2.4 Other caveats for MAC drivers(3.2.4 MAC 驱动程序的其他注意事项)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The use of stacked PHCs may uncover MAC driver bugs which were impossible to
trigger without them. One example has to do with this line of code, already
presented earlier::
(使用堆叠 PHC 可能暴露出在没有它们时无法触发的 MAC 驱动程序缺陷。一个例子与
前面已经展示过的这行代码有关：)

      skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS;

Any TX timestamping logic, be it a plain MAC driver, a DSA switch driver, a PHY
driver or a MII bus snooping device driver, should set this flag.
But a MAC driver that is unaware of PHC stacking might get tripped up by
somebody other than itself setting this flag, and deliver a duplicate
timestamp.
For example, a typical driver design for TX timestamping might be to split the
transmission part into 2 portions:
(任何 TX 时间戳逻辑——无论是普通 MAC 驱动程序、DSA 交换机驱动程序、PHY 驱动
程序还是 MII 总线侦听设备驱动程序——都应当设置这个标志。但一个不了解 PHC 堆叠
的 MAC 驱动程序，可能会被自己以外的其他实体设置该标志这件事绊倒，从而交付重复
的时间戳。例如，一个典型的 TX 时间戳驱动程序设计可能把发送部分拆成 2 个环节：)

1. "TX": checks whether PTP timestamping has been previously enabled through
   the ``.ndo_eth_ioctl`` ("``priv->hwtstamp_tx_enabled == true``") and the
   current skb requires a TX timestamp ("``skb_shinfo(skb)->tx_flags &
   SKBTX_HW_TSTAMP``"). If this is true, it sets the
   "``skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS``" flag. Note: as
   described above, in the case of a stacked PHC system, this condition should
   never trigger, as this MAC is certainly not the outermost PHC. But this is
   not where the typical issue is.  Transmission proceeds with this packet.
   (1. "TX"：检查 PTP 时间戳是否已经通过 ``.ndo_eth_ioctl`` 事先启用
   ("``priv->hwtstamp_tx_enabled == true``")，以及当前 skb 是否需要 TX 时间戳
   ("``skb_shinfo(skb)->tx_flags & SKBTX_HW_TSTAMP``")。如果都为真，则设置
   "``skb_shinfo(skb)->tx_flags |= SKBTX_IN_PROGRESS``" 标志。注意：如上所述，
   在堆叠 PHC 系统中，这个条件绝不应触发，因为这个 MAC 肯定不是最外层的 PHC。
   但典型问题并不在这里。该数据包继续正常发送。)

2. "TX confirmation": Transmission has finished. The driver checks whether it
   is necessary to collect any TX timestamp for it. Here is where the typical
   issues are: the MAC driver takes a shortcut and only checks whether
   "``skb_shinfo(skb)->tx_flags & SKBTX_IN_PROGRESS``" was set. With a stacked
   PHC system, this is incorrect because this MAC driver is not the only entity
   in the TX data path who could have enabled SKBTX_IN_PROGRESS in the first
   place.
   (2. "TX 确认"：发送已完成。驱动程序检查是否需要为它收集 TX 时间戳。典型的
   问题正出在这里：MAC 驱动程序走了捷径，只检查
   "``skb_shinfo(skb)->tx_flags & SKBTX_IN_PROGRESS``" 是否被设置。在堆叠 PHC
   系统中，这是不正确的，因为最先设置 SKBTX_IN_PROGRESS 的实体并非只有这个 MAC
   驱动程序，TX 数据路径上还有别的实体可以这样做。)

The correct solution for this problem is for MAC drivers to have a compound
check in their "TX confirmation" portion, not only for
"``skb_shinfo(skb)->tx_flags & SKBTX_IN_PROGRESS``", but also for
"``priv->hwtstamp_tx_enabled == true``". Because the rest of the system ensures
that PTP timestamping is not enabled for anything other than the outermost PHC,
this enhanced check will avoid delivering a duplicated TX timestamp to user
space.
(这个问题的正确解法是，MAC 驱动程序在其 "TX 确认" 部分做复合检查，不仅检查
"``skb_shinfo(skb)->tx_flags & SKBTX_IN_PROGRESS``"，还要检查
"``priv->hwtstamp_tx_enabled == true``"。由于系统的其余部分保证了除最外层 PHC
之外不会启用 PTP 时间戳，这一增强检查将避免向用户空间交付重复的 TX 时间戳。)
