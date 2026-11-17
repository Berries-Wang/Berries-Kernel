.. SPDX-License-Identifier: GPL-2.0

==========================================================
The UDP-Lite protocol (RFC 3828)(UDP-Lite 协议 (RFC 3828))
==========================================================


  UDP-Lite is a Standards-Track IETF transport protocol whose characteristic
  is a variable-length checksum. This has advantages for transport of multimedia
  (video, VoIP) over wireless networks, as partly damaged packets can still be
  fed into the codec instead of being discarded due to a failed checksum test.
  (UDP-Lite 是一种处于标准进程中的 IETF 传输协议，其特点是校验和长度可变。这对于在无线网络上传输多媒体（视频、VoIP）很有好处，因为部分受损的数据包仍可送入编解码器，而不必因校验和校验失败而被丢弃。)

  This file briefly describes the existing kernel support and the socket API.
  For in-depth information, you can consult:
  (本文件简要描述现有的内核支持与套接字 API。如需深入了解，可查阅：)

   - The UDP-Lite Homepage:
     http://web.archive.org/web/%2E/http://www.erg.abdn.ac.uk/users/gerrit/udp-lite/
     (UDP-Lite 主页：)

     From here you can also download some example application source code.
     (从这里还可以下载一些示例应用程序的源代码。)

   - The UDP-Lite HOWTO on
     http://web.archive.org/web/%2E/http://www.erg.abdn.ac.uk/users/gerrit/udp-lite/files/UDP-Lite-HOWTO.txt
     (UDP-Lite HOWTO，见上方链接。)

   - The Wireshark UDP-Lite WiKi (with capture files):
     https://wiki.wireshark.org/Lightweight_User_Datagram_Protocol
     (Wireshark UDP-Lite WiKi（含抓包文件）：)

   - The Protocol Spec, RFC 3828, http://www.ietf.org/rfc/rfc3828.txt
     (协议规范 RFC 3828。)


1. Applications(1. 应用)
========================

  Several applications have been ported successfully to UDP-Lite. Ethereal
  (now called wireshark) has UDP-Litev4/v6 support by default.
  (已有若干应用程序被成功移植到 UDP-Lite。Ethereal（现称 wireshark）默认支持 UDP-Litev4/v6。)

  Porting applications to UDP-Lite is straightforward: only socket level and
  IPPROTO need to be changed; senders additionally set the checksum coverage
  length (default = header length = 8). Details are in the next section.
  (将应用程序移植到 UDP-Lite 很直接：只需修改套接字级别和 IPPROTO；发送方另外还要设置校验和覆盖长度（默认 = 头长度 = 8）。详情见下一节。)

2. Programming API(2. 编程 API)
===============================

  UDP-Lite provides a connectionless, unreliable datagram service and hence
  uses the same socket type as UDP. In fact, porting from UDP to UDP-Lite is
  very easy: simply add ``IPPROTO_UDPLITE`` as the last argument of the
  socket(2) call so that the statement looks like::
  (UDP-Lite 提供无连接、不可靠的数据报服务，因此使用与 UDP 相同的套接字类型。事实上，从 UDP 移植到 UDP-Lite 非常容易：只需把 ``IPPROTO_UDPLITE`` 作为 socket(2) 调用的最后一个参数，使语句变为如下形式：)

      s = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDPLITE);

  or, respectively,
  (或者，相应地：)

  ::

      s = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDPLITE);

  With just the above change you are able to run UDP-Lite services or connect
  to UDP-Lite servers. The kernel will assume that you are not interested in
  using partial checksum coverage and so emulate UDP mode (full coverage).
  (仅做上述修改，就可以运行 UDP-Lite 服务或连接到 UDP-Lite 服务器。内核会假定你不打算使用部分校验和覆盖，从而模拟 UDP 模式（完全覆盖）。)

  To make use of the partial checksum coverage facilities requires setting a
  single socket option, which takes an integer specifying the coverage length:
  (要使用部分校验和覆盖功能，只需设置一个套接字选项，它接受一个整数来指定覆盖长度：)

    * Sender checksum coverage: UDPLITE_SEND_CSCOV
      (发送方校验和覆盖：UDPLITE_SEND_CSCOV)

      For example::
      (例如：)

	int val = 20;
	setsockopt(s, SOL_UDPLITE, UDPLITE_SEND_CSCOV, &val, sizeof(int));

      sets the checksum coverage length to 20 bytes (12b data + 8b header).
      Of each packet only the first 20 bytes (plus the pseudo-header) will be
      checksummed. This is useful for RTP applications which have a 12-byte
      base header.
      (这会把校验和覆盖长度设为 20 字节（12 字节数据 + 8 字节头）。每个数据包只有前 20 字节（加上伪头）会被计算校验和。这对拥有 12 字节基本头的 RTP 应用很有用。)


    * Receiver checksum coverage: UDPLITE_RECV_CSCOV
      (接收方校验和覆盖：UDPLITE_RECV_CSCOV)

      This option is the receiver-side analogue. It is truly optional, i.e. not
      required to enable traffic with partial checksum coverage. Its function is
      that of a traffic filter: when enabled, it instructs the kernel to drop
      all packets which have a coverage _less_ than this value. For example, if
      RTP and UDP headers are to be protected, a receiver can enforce that only
      packets with a minimum coverage of 20 are admitted::
      (该选项是接收方的对应版本。它完全是可选的，即启用具有部分校验和覆盖的流量并不需要它。它的作用相当于一个流量过滤器：启用后，内核会丢弃所有覆盖长度小于该值的数据包。例如，如果要保护 RTP 和 UDP 头，接收方可以强制只接受覆盖长度最小为 20 的数据包：)

	int min = 20;
	setsockopt(s, SOL_UDPLITE, UDPLITE_RECV_CSCOV, &min, sizeof(int));

  The calls to getsockopt(2) are analogous. Being an extension and not a stand-
  alone protocol, all socket options known from UDP can be used in exactly the
  same manner as before, e.g. UDP_CORK or UDP_ENCAP.
  (getsockopt(2) 的调用与之类似。由于 UDP-Lite 是扩展而非独立协议，UDP 中已知的所有套接字选项都可以完全按照原先的方式使用，例如 UDP_CORK 或 UDP_ENCAP。)

  A detailed discussion of UDP-Lite checksum coverage options is in section IV.
  (关于 UDP-Lite 校验和覆盖选项的详细讨论见第 IV 节。)

3. Header Files(3. 头文件)
==========================

  The socket API requires support through header files in /usr/include:
  (套接字 API 需要通过 /usr/include 中的头文件来提供支持：)

    * /usr/include/netinet/in.h
      to define IPPROTO_UDPLITE
      (用于定义 IPPROTO_UDPLITE)

    * /usr/include/netinet/udplite.h
      for UDP-Lite header fields and protocol constants
      (包含 UDP-Lite 头部字段和协议常量)

  For testing purposes, the following can serve as a ``mini`` header file::
  (出于测试目的，可以使用下面这个 ``mini`` 头文件：)

    #define IPPROTO_UDPLITE       136
    #define SOL_UDPLITE           136
    #define UDPLITE_SEND_CSCOV     10
    #define UDPLITE_RECV_CSCOV     11

  Ready-made header files for various distros are in the UDP-Lite tarball.
  (适用于各种发行版的现成头文件包含在 UDP-Lite tarball 中。)

4. Kernel Behaviour with Regards to the Various Socket Options(4. 内核对各种套接字选项的处理行为)
=================================================================================================


  To enable debugging messages, the log level need to be set to 8, as most
  messages use the KERN_DEBUG level (7).
  (要启用调试消息，需要把日志级别设为 8，因为大多数消息使用 KERN_DEBUG 级别 (7)。)

  1) Sender Socket Options
  (1) 发送方套接字选项)

  If the sender specifies a value of 0 as coverage length, the module
  assumes full coverage, transmits a packet with coverage length of 0
  and according checksum.  If the sender specifies a coverage < 8 and
  different from 0, the kernel assumes 8 as default value.  Finally,
  if the specified coverage length exceeds the packet length, the packet
  length is used instead as coverage length.
  (如果发送方将覆盖长度指定为 0，模块会假定完全覆盖，发送一个覆盖长度为 0 的数据包及相应的校验和。如果发送方指定的覆盖小于 8 且不为 0，内核会采用默认值 8。最后，如果指定的覆盖长度超过数据包长度，则改用数据包长度作为覆盖长度。)

  2) Receiver Socket Options
  (2) 接收方套接字选项)

  The receiver specifies the minimum value of the coverage length it
  is willing to accept.  A value of 0 here indicates that the receiver
  always wants the whole of the packet covered. In this case, all
  partially covered packets are dropped and an error is logged.
  (接收方指定它愿意接受的覆盖长度最小值。这里的 0 表示接收方总是希望整个数据包都被覆盖。在这种情况下，所有部分覆盖的数据包都会被丢弃，并记录一条错误日志。)

  It is not possible to specify illegal values (<0 and <8); in these
  cases the default of 8 is assumed.
  (不允许指定非法值（<0 和 <8）；在这些情况下会采用默认值 8。)

  All packets arriving with a coverage value less than the specified
  threshold are discarded, these events are also logged.
  (所有到达时覆盖值小于指定阈值的数据包都会被丢弃，这些事件也会被记录。)

  3) Disabling the Checksum Computation
  (3) 禁用校验和计算)

  On both sender and receiver, checksumming will always be performed
  and cannot be disabled using SO_NO_CHECK. Thus::
  (无论在发送方还是接收方，校验和计算总是会被执行，并且无法用 SO_NO_CHECK 禁用。因此：)

	setsockopt(sockfd, SOL_SOCKET, SO_NO_CHECK,  ... );

  will always will be ignored, while the value of::
  (将总是被忽略，而下面这个调用的返回值：)

	getsockopt(sockfd, SOL_SOCKET, SO_NO_CHECK, &value, ...);

  is meaningless (as in TCP). Packets with a zero checksum field are
  illegal (cf. RFC 3828, sec. 3.1) and will be silently discarded.
  (是没有意义的（如同 TCP）。校验和字段为零的数据包是非法的（参见 RFC 3828 第 3.1 节），会被静默丢弃。)

  4) Fragmentation
  (4) 分片)

  The checksum computation respects both buffersize and MTU. The size
  of UDP-Lite packets is determined by the size of the send buffer. The
  minimum size of the send buffer is 2048 (defined as SOCK_MIN_SNDBUF
  in include/net/sock.h), the default value is configurable as
  net.core.wmem_default or via setting the SO_SNDBUF socket(7)
  option. The maximum upper bound for the send buffer is determined
  by net.core.wmem_max.
  (校验和计算同时考虑缓冲区大小和 MTU。UDP-Lite 数据包的大小由发送缓冲区的大小决定。发送缓冲区的最小值为 2048（定义为 include/net/sock.h 中的 SOCK_MIN_SNDBUF），默认值可通过 net.core.wmem_default 配置，或通过设置 SO_SNDBUF socket(7) 选项来调整。发送缓冲区的最大上限由 net.core.wmem_max 决定。)

  Given a payload size larger than the send buffer size, UDP-Lite will
  split the payload into several individual packets, filling up the
  send buffer size in each case.
  (当载荷大于发送缓冲区大小时，UDP-Lite 会把载荷拆分成若干独立的数据包，每个都按发送缓冲区大小填满。)

  The precise value also depends on the interface MTU. The interface MTU,
  in turn, may trigger IP fragmentation. In this case, the generated
  UDP-Lite packet is split into several IP packets, of which only the
  first one contains the L4 header.
  (具体数值还取决于接口 MTU。而接口 MTU 又可能触发 IP 分片。在这种情况下，生成的 UDP-Lite 数据包会被拆成多个 IP 数据包，其中只有第一个包含 L4 头。)

  The send buffer size has implications on the checksum coverage length.
  Consider the following example::
  (发送缓冲区的大小会影响校验和覆盖长度。请看以下示例：)

    Payload: 1536 bytes          Send Buffer:     1024 bytes
    MTU:     1500 bytes          Coverage Length:  856 bytes

  UDP-Lite will ship the 1536 bytes in two separate packets::
  (UDP-Lite 将把 1536 字节分装在两个单独的数据包中发送：)

    Packet 1: 1024 payload + 8 byte header + 20 byte IP header = 1052 bytes
    Packet 2:  512 payload + 8 byte header + 20 byte IP header =  540 bytes

  The coverage packet covers the UDP-Lite header and 848 bytes of the
  payload in the first packet, the second packet is fully covered. Note
  that for the second packet, the coverage length exceeds the packet
  length. The kernel always re-adjusts the coverage length to the packet
  length in such cases.
  (覆盖范围涵盖第一个数据包中的 UDP-Lite 头及其中 848 字节载荷，第二个数据包则被完全覆盖。注意，对第二个数据包而言，覆盖长度超过了数据包长度。在这种情况下，内核总是会把覆盖长度重新调整为数据包长度。)

  As an example of what happens when one UDP-Lite packet is split into
  several tiny fragments, consider the following example::

    Payload: 1024 bytes            Send buffer size: 1024 bytes
    MTU:      300 bytes            Coverage length:   575 bytes

    +-+-----------+--------------+--------------+--------------+
    |8|    272    |      280     |     280      |     280      |
    +-+-----------+--------------+--------------+--------------+

  (要了解一个 UDP-Lite 数据包被拆成多个小分片时会发生什么，请看以下示例：)

		280            560            840           1032
					^
    *****checksum coverage*************

  The UDP-Lite module generates one 1032 byte packet (1024 + 8 byte
  header). According to the interface MTU, these are split into 4 IP
  packets (280 byte IP payload + 20 byte IP header). The kernel module
  sums the contents of the entire first two packets, plus 15 bytes of
  the last packet before releasing the fragments to the IP module.
  (UDP-Lite 模块生成一个 1032 字节的数据包（1024 + 8 字节头）。根据接口 MTU，它被拆分为 4 个 IP 数据包（280 字节 IP 载荷 + 20 字节 IP 头）。内核模块会对前两个数据包的全部内容加上最后一个数据包的 15 字节求出校验和，然后才把分片交给 IP 模块。)

  To see the analogous case for IPv6 fragmentation, consider a link
  MTU of 1280 bytes and a write buffer of 3356 bytes. If the checksum
  coverage is less than 1232 bytes (MTU minus IPv6/fragment header
  lengths), only the first fragment needs to be considered. When using
  larger checksum coverage lengths, each eligible fragment needs to be
  checksummed. Suppose we have a checksum coverage of 3062. The buffer
  of 3356 bytes will be split into the following fragments::
  (要了解 IPv6 分片的类似情况，设想链路 MTU 为 1280 字节、写缓冲区为 3356 字节。如果校验和覆盖小于 1232 字节（MTU 减去 IPv6/分片头长度），则只需考虑第一个分片。当使用更大的校验和覆盖长度时，每个符合范围要求的分片都需要计算校验和。假设校验和覆盖为 3062，那么 3356 字节的缓冲区将被拆分成以下分片：)

    Fragment 1: 1280 bytes carrying  1232 bytes of UDP-Lite data
    Fragment 2: 1280 bytes carrying  1232 bytes of UDP-Lite data
    Fragment 3:  948 bytes carrying   900 bytes of UDP-Lite data

  The first two fragments have to be checksummed in full, of the last
  fragment only 598 (= 3062 - 2*1232) bytes are checksummed.
  (前两个分片必须完整计算校验和，最后一个分片只有 598（= 3062 - 2*1232）字节被计算校验和。)

  While it is important that such cases are dealt with correctly, they
  are (annoyingly) rare: UDP-Lite is designed for optimising multimedia
  performance over wireless (or generally noisy) links and thus smaller
  coverage lengths are likely to be expected.
  (虽然正确处理这些情况很重要，但它们（令人恼火地）很少出现：UDP-Lite 的设计目标是在无线（或泛指有噪声的）链路上优化多媒体性能，因此预计会使用较小的覆盖长度。)

5. UDP-Lite Runtime Statistics and their Meaning(5. UDP-Lite 运行时统计及其含义)
================================================================================

  Exceptional and error conditions are logged to syslog at the KERN_DEBUG
  level.  Live statistics about UDP-Lite are available in /proc/net/snmp
  and can (with newer versions of netstat) be viewed using::
  (异常和错误情况会以 KERN_DEBUG 级别记录到 syslog。UDP-Lite 的实时统计可在 /proc/net/snmp 中获取，并且（在较新版本的 netstat 上）可以用以下命令查看：)

			    netstat -svu

  This displays UDP-Lite statistics variables, whose meaning is as follows.
  (这里显示 UDP-Lite 的统计变量，其含义如下。)

   ============     =====================================================
   InDatagrams      The total number of datagrams delivered to users.
                    (交付给用户的全部数据报数量。)

   NoPorts          Number of packets received to an unknown port.
		    These cases are counted separately (not as InErrors).
                    (接收到未知端口的包数。这些情况被单独统计（不计为 InErrors）。)

   InErrors         Number of erroneous UDP-Lite packets. Errors include:

		      * internal socket queue receive errors
		      * packet too short (less than 8 bytes or stated
			coverage length exceeds received length)
		      * xfrm4_policy_check() returned with error
		      * application has specified larger min. coverage
			length than that of incoming packet
		      * checksum coverage violated
		      * bad checksum
                    (错误的 UDP-Lite 数据包数量。错误包括：内部套接字队列接收错误；数据包过短（少于 8 字节，或所述覆盖长度超过接收长度）；xfrm4_policy_check() 返回错误；应用指定的最小覆盖长度大于到达数据包的覆盖长度；违反校验和覆盖；校验和不正确。)

   OutDatagrams     Total number of sent datagrams.
                    (发送的数据报总数。)
   ============     =====================================================

   These statistics derive from the UDP MIB (RFC 2013).
   (这些统计源自 UDP MIB (RFC 2013)。)

6. IPtables(6. IPtables)
========================

  There is packet match support for UDP-Lite as well as support for the LOG target.
  If you copy and paste the following line into /etc/protocols::
  (对 UDP-Lite 也提供了数据包匹配支持以及对 LOG 目标的支持。如果你把下面这一行复制粘贴到 /etc/protocols 中：)

    udplite 136     UDP-Lite        # UDP-Lite [RFC 3828]

  then::
  (然后执行：)

	      iptables -A INPUT -p udplite -j LOG

  will produce logging output to syslog. Dropping and rejecting packets also works.
  (将产生输出到 syslog 的日志记录。丢弃和拒绝数据包也同样可行。)

7. Maintainer Address(7. 维护者地址)
====================================

  The UDP-Lite patch was developed at
  (UDP-Lite 补丁由以下机构开发：)

		    University of Aberdeen
		    Electronics Research Group
		    Department of Engineering
		    Fraser Noble Building
		    Aberdeen AB24 3UE; UK

  The current maintainer is Gerrit Renker, <gerrit@erg.abdn.ac.uk>. Initial
  code was developed by William  Stanislaus, <william@erg.abdn.ac.uk>.
  (当前维护者是 Gerrit Renker，<gerrit@erg.abdn.ac.uk>。最初的代码由 William Stanislaus 开发，<william@erg.abdn.ac.uk>。)
