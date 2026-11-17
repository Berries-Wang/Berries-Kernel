=========================
SNMP counter(SNMP 计数器)
=========================

This document explains the meaning of SNMP counters.
(本文档解释 SNMP 计数器的含义。)

General IPv4 counters(常规 IPv4 计数器)
=======================================
All layer 4 packets and ICMP packets will change these counters, but
these counters won't be changed by layer 2 packets (such as STP) or
ARP packets.
(所有第 4 层数据包和 ICMP 数据包都会改变这些计数器，但第 2 层数据包(例如 STP)或 ARP 数据包不会改变它们。)

* IpInReceives

Defined in `RFC1213 ipInReceives`_
(定义见 `RFC1213 ipInReceives`_)

.. _RFC1213 ipInReceives: https://tools.ietf.org/html/rfc1213#page-26

The number of packets received by the IP layer. It gets increasing at the
beginning of ip_rcv function, always be updated together with
IpExtInOctets. It will be increased even if the packet is dropped
later (e.g. due to the IP header is invalid or the checksum is wrong
and so on).  It indicates the number of aggregated segments after
GRO/LRO.
(IP 层接收到的数据包数量。它在 ip_rcv 函数的开头就开始递增，并且总是与 IpExtInOctets 一起更新。即使数据包随后被丢弃(例如由于 IP 头无效或校验和错误等)，该计数器也会增加。它表示经过 GRO/LRO 聚合之后的报段数量。)

* IpInDelivers

Defined in `RFC1213 ipInDelivers`_
(定义见 `RFC1213 ipInDelivers`_)

.. _RFC1213 ipInDelivers: https://tools.ietf.org/html/rfc1213#page-28

The number of packets delivers to the upper layer protocols. E.g. TCP, UDP,
ICMP and so on. If no one listens on a raw socket, only kernel
supported protocols will be delivered, if someone listens on the raw
socket, all valid IP packets will be delivered.
(交付给上层协议的数据包数量，例如 TCP、UDP、ICMP 等。如果没有任何进程监听原始套接字 (raw socket)，只有内核支持的协议会被交付；如果有进程监听原始套接字，则所有有效的 IP 数据包都会被交付。)

* IpOutRequests

Defined in `RFC1213 ipOutRequests`_
(定义见 `RFC1213 ipOutRequests`_)

.. _RFC1213 ipOutRequests: https://tools.ietf.org/html/rfc1213#page-28

The number of packets sent via IP layer, for both single cast and
multicast packets, and would always be updated together with
IpExtOutOctets.
(经 IP 层发送的数据包数量，包括单播和组播数据包，并且总是与 IpExtOutOctets 一起更新。)

* IpExtInOctets and IpExtOutOctets
  (IpExtInOctets 和 IpExtOutOctets)

They are Linux kernel extensions, no RFC definitions. Please note,
RFC1213 indeed defines ifInOctets  and ifOutOctets, but they
are different things. The ifInOctets and ifOutOctets include the MAC
layer header size but IpExtInOctets and IpExtOutOctets don't, they
only include the IP layer header and the IP layer data.
(它们是 Linux 内核的扩展，没有 RFC 定义。请注意，RFC1213 确实定义了 ifInOctets 和 ifOutOctets，但它们是不同的东西。ifInOctets 和 ifOutOctets 包含 MAC 层头部的大小，而 IpExtInOctets 和 IpExtOutOctets 不包含，它们只包含 IP 层头部和 IP 层数据。)

* IpExtInNoECTPkts, IpExtInECT1Pkts, IpExtInECT0Pkts, IpExtInCEPkts
  (IpExtInNoECTPkts、IpExtInECT1Pkts、IpExtInECT0Pkts 和 IpExtInCEPkts)

They indicate the number of four kinds of ECN IP packets, please refer
`Explicit Congestion Notification`_ for more details.
(它们表示四种 ECN IP 数据包各自的数量，更多细节请参阅 `Explicit Congestion Notification`_。)

.. _Explicit Congestion Notification: https://tools.ietf.org/html/rfc3168#page-6

These 4 counters calculate how many packets received per ECN
status. They count the real frame number regardless the LRO/GRO. So
for the same packet, you might find that IpInReceives count 1, but
IpExtInNoECTPkts counts 2 or more.
(这 4 个计数器按 ECN 状态统计接收到的数据包数量。它们统计的是真实的帧数，不考虑 LRO/GRO。因此对于同一个数据包，你可能发现 IpInReceives 计数为 1，而 IpExtInNoECTPkts 计数为 2 或更多。)

* IpInHdrErrors

Defined in `RFC1213 ipInHdrErrors`_. It indicates the packet is
dropped due to the IP header error. It might happen in both IP input
and IP forward paths.
(定义见 `RFC1213 ipInHdrErrors`_。它表示数据包因 IP 头错误而被丢弃。这种情况可能发生在 IP 输入和 IP 转发两条路径中。)

.. _RFC1213 ipInHdrErrors: https://tools.ietf.org/html/rfc1213#page-27

* IpInAddrErrors

Defined in `RFC1213 ipInAddrErrors`_. It will be increased in two
scenarios: (1) The IP address is invalid. (2) The destination IP
address is not a local address and IP forwarding is not enabled
(定义见 `RFC1213 ipInAddrErrors`_。它在两种场景下会增加：(1) IP 地址无效。(2) 目的 IP 地址不是本地地址且未启用 IP 转发。)

.. _RFC1213 ipInAddrErrors: https://tools.ietf.org/html/rfc1213#page-27

* IpExtInNoRoutes

This counter means the packet is dropped when the IP stack receives a
packet and can't find a route for it from the route table. It might
happen when IP forwarding is enabled and the destination IP address is
not a local address and there is no route for the destination IP
address.
(该计数器表示 IP 协议栈收到数据包后在路由表中找不到它的路由，因而丢弃该数据包。当启用了 IP 转发、目的 IP 地址不是本地地址、并且目的 IP 地址没有路由时，就可能发生这种情况。)

* IpInUnknownProtos

Defined in `RFC1213 ipInUnknownProtos`_. It will be increased if the
layer 4 protocol is unsupported by kernel. If an application is using
raw socket, kernel will always deliver the packet to the raw socket
and this counter won't be increased.
(定义见 `RFC1213 ipInUnknownProtos`_。当内核不支持该第 4 层协议时，此计数器会增加。如果有应用程序在使用原始套接字，内核总是会把数据包交付给原始套接字，此时该计数器不会增加。)

.. _RFC1213 ipInUnknownProtos: https://tools.ietf.org/html/rfc1213#page-27

* IpExtInTruncatedPkts

For IPv4 packet, it means the actual data size is smaller than the
"Total Length" field in the IPv4 header.
(对 IPv4 数据包而言，它表示实际数据大小小于 IPv4 头中的 "Total Length" 字段。)

* IpInDiscards

Defined in `RFC1213 ipInDiscards`_. It indicates the packet is dropped
in the IP receiving path and due to kernel internal reasons (e.g. no
enough memory).
(定义见 `RFC1213 ipInDiscards`_。它表示数据包在 IP 接收路径中由于内核内部原因(例如内存不足)而被丢弃。)

.. _RFC1213 ipInDiscards: https://tools.ietf.org/html/rfc1213#page-28

* IpOutDiscards

Defined in `RFC1213 ipOutDiscards`_. It indicates the packet is
dropped in the IP sending path and due to kernel internal reasons.
(定义见 `RFC1213 ipOutDiscards`_。它表示数据包在 IP 发送路径中由于内核内部原因而被丢弃。)

.. _RFC1213 ipOutDiscards: https://tools.ietf.org/html/rfc1213#page-28

* IpOutNoRoutes

Defined in `RFC1213 ipOutNoRoutes`_. It indicates the packet is
dropped in the IP sending path and no route is found for it.
(定义见 `RFC1213 ipOutNoRoutes`_。它表示数据包在 IP 发送路径中由于找不到路由而被丢弃。)

.. _RFC1213 ipOutNoRoutes: https://tools.ietf.org/html/rfc1213#page-29

ICMP counters(ICMP 计数器)
==========================

* IcmpInMsgs and IcmpOutMsgs
  (IcmpInMsgs 和 IcmpOutMsgs)

Defined by `RFC1213 icmpInMsgs`_ and `RFC1213 icmpOutMsgs`_
(由 `RFC1213 icmpInMsgs`_ 和 `RFC1213 icmpOutMsgs`_ 定义)

.. _RFC1213 icmpInMsgs: https://tools.ietf.org/html/rfc1213#page-41
.. _RFC1213 icmpOutMsgs: https://tools.ietf.org/html/rfc1213#page-43

As mentioned in the RFC1213, these two counters include errors, they
would be increased even if the ICMP packet has an invalid type. The
ICMP output path will check the header of a raw socket, so the
IcmpOutMsgs would still be updated if the IP header is constructed by
a userspace program.
(如 RFC1213 所述，这两个计数器包含出错的报文，即使 ICMP 数据包的类型无效，它们也会增加。ICMP 输出路径会检查原始套接字的头部，因此即使 IP 头是由用户态程序构造的，IcmpOutMsgs 仍会被更新。)

* ICMP named types
  (ICMP 命名类型)

| These counters include most of common ICMP types, they are:
| (这些计数器涵盖大多数常见的 ICMP 类型，它们是：)
| IcmpInDestUnreachs: `RFC1213 icmpInDestUnreachs`_
| IcmpInTimeExcds: `RFC1213 icmpInTimeExcds`_
| IcmpInParmProbs: `RFC1213 icmpInParmProbs`_
| IcmpInSrcQuenchs: `RFC1213 icmpInSrcQuenchs`_
| IcmpInRedirects: `RFC1213 icmpInRedirects`_
| IcmpInEchos: `RFC1213 icmpInEchos`_
| IcmpInEchoReps: `RFC1213 icmpInEchoReps`_
| IcmpInTimestamps: `RFC1213 icmpInTimestamps`_
| IcmpInTimestampReps: `RFC1213 icmpInTimestampReps`_
| IcmpInAddrMasks: `RFC1213 icmpInAddrMasks`_
| IcmpInAddrMaskReps: `RFC1213 icmpInAddrMaskReps`_
| IcmpOutDestUnreachs: `RFC1213 icmpOutDestUnreachs`_
| IcmpOutTimeExcds: `RFC1213 icmpOutTimeExcds`_
| IcmpOutParmProbs: `RFC1213 icmpOutParmProbs`_
| IcmpOutSrcQuenchs: `RFC1213 icmpOutSrcQuenchs`_
| IcmpOutRedirects: `RFC1213 icmpOutRedirects`_
| IcmpOutEchos: `RFC1213 icmpOutEchos`_
| IcmpOutEchoReps: `RFC1213 icmpOutEchoReps`_
| IcmpOutTimestamps: `RFC1213 icmpOutTimestamps`_
| IcmpOutTimestampReps: `RFC1213 icmpOutTimestampReps`_
| IcmpOutAddrMasks: `RFC1213 icmpOutAddrMasks`_
| IcmpOutAddrMaskReps: `RFC1213 icmpOutAddrMaskReps`_

.. _RFC1213 icmpInDestUnreachs: https://tools.ietf.org/html/rfc1213#page-41
.. _RFC1213 icmpInTimeExcds: https://tools.ietf.org/html/rfc1213#page-41
.. _RFC1213 icmpInParmProbs: https://tools.ietf.org/html/rfc1213#page-42
.. _RFC1213 icmpInSrcQuenchs: https://tools.ietf.org/html/rfc1213#page-42
.. _RFC1213 icmpInRedirects: https://tools.ietf.org/html/rfc1213#page-42
.. _RFC1213 icmpInEchos: https://tools.ietf.org/html/rfc1213#page-42
.. _RFC1213 icmpInEchoReps: https://tools.ietf.org/html/rfc1213#page-42
.. _RFC1213 icmpInTimestamps: https://tools.ietf.org/html/rfc1213#page-42
.. _RFC1213 icmpInTimestampReps: https://tools.ietf.org/html/rfc1213#page-43
.. _RFC1213 icmpInAddrMasks: https://tools.ietf.org/html/rfc1213#page-43
.. _RFC1213 icmpInAddrMaskReps: https://tools.ietf.org/html/rfc1213#page-43

.. _RFC1213 icmpOutDestUnreachs: https://tools.ietf.org/html/rfc1213#page-44
.. _RFC1213 icmpOutTimeExcds: https://tools.ietf.org/html/rfc1213#page-44
.. _RFC1213 icmpOutParmProbs: https://tools.ietf.org/html/rfc1213#page-44
.. _RFC1213 icmpOutSrcQuenchs: https://tools.ietf.org/html/rfc1213#page-44
.. _RFC1213 icmpOutRedirects: https://tools.ietf.org/html/rfc1213#page-44
.. _RFC1213 icmpOutEchos: https://tools.ietf.org/html/rfc1213#page-45
.. _RFC1213 icmpOutEchoReps: https://tools.ietf.org/html/rfc1213#page-45
.. _RFC1213 icmpOutTimestamps: https://tools.ietf.org/html/rfc1213#page-45
.. _RFC1213 icmpOutTimestampReps: https://tools.ietf.org/html/rfc1213#page-45
.. _RFC1213 icmpOutAddrMasks: https://tools.ietf.org/html/rfc1213#page-45
.. _RFC1213 icmpOutAddrMaskReps: https://tools.ietf.org/html/rfc1213#page-46

Every ICMP type has two counters: 'In' and 'Out'. E.g., for the ICMP
Echo packet, they are IcmpInEchos and IcmpOutEchos. Their meanings are
straightforward. The 'In' counter means kernel receives such a packet
and the 'Out' counter means kernel sends such a packet.
(每种 ICMP 类型都有两个计数器："In" 和 "Out"。例如对 ICMP Echo 数据包而言，它们分别是 IcmpInEchos 和 IcmpOutEchos。它们的含义很直白："In" 计数器表示内核收到了这样一个数据包，"Out" 计数器表示内核发送了这样一个数据包。)

* ICMP numeric types
  (ICMP 数字类型)

They are IcmpMsgInType[N] and IcmpMsgOutType[N], the [N] indicates the
ICMP type number. These counters track all kinds of ICMP packets. The
ICMP type number definition could be found in the `ICMP parameters`_
document.
(它们是 IcmpMsgInType[N] 和 IcmpMsgOutType[N]，[N] 表示 ICMP 类型编号。这些计数器跟踪所有类型的 ICMP 数据包。ICMP 类型编号的定义可在 `ICMP parameters`_ 文档中找到。)

.. _ICMP parameters: https://www.iana.org/assignments/icmp-parameters/icmp-parameters.xhtml

For example, if the Linux kernel sends an ICMP Echo packet, the
IcmpMsgOutType8 would increase 1. And if kernel gets an ICMP Echo Reply
packet, IcmpMsgInType0 would increase 1.
(例如，如果 Linux 内核发送一个 ICMP Echo 数据包，IcmpMsgOutType8 会增加 1。如果内核收到一个 ICMP Echo Reply 数据包，IcmpMsgInType0 会增加 1。)

* IcmpInCsumErrors

This counter indicates the checksum of the ICMP packet is
wrong. Kernel verifies the checksum after updating the IcmpInMsgs and
before updating IcmpMsgInType[N]. If a packet has bad checksum, the
IcmpInMsgs would be updated but none of IcmpMsgInType[N] would be updated.
(该计数器表示 ICMP 数据包的校验和有误。内核在更新 IcmpInMsgs 之后、更新 IcmpMsgInType[N] 之前校验校验和。如果数据包校验和有误，IcmpInMsgs 会被更新，但 IcmpMsgInType[N] 都不会被更新。)

* IcmpInErrors and IcmpOutErrors
  (IcmpInErrors 和 IcmpOutErrors)

Defined by `RFC1213 icmpInErrors`_ and `RFC1213 icmpOutErrors`_
(由 `RFC1213 icmpInErrors`_ 和 `RFC1213 icmpOutErrors`_ 定义)

.. _RFC1213 icmpInErrors: https://tools.ietf.org/html/rfc1213#page-41
.. _RFC1213 icmpOutErrors: https://tools.ietf.org/html/rfc1213#page-43

When an error occurs in the ICMP packet handler path, these two
counters would be updated. The receiving packet path use IcmpInErrors
and the sending packet path use IcmpOutErrors. When IcmpInCsumErrors
is increased, IcmpInErrors would always be increased too.
(当 ICMP 数据包处理路径中发生错误时，这两个计数器会被更新。接收数据包路径使用 IcmpInErrors，发送数据包路径使用 IcmpOutErrors。当 IcmpInCsumErrors 增加时，IcmpInErrors 也总是会增加。)

relationship of the ICMP counters(ICMP 计数器之间的关系)
--------------------------------------------------------
The sum of IcmpMsgOutType[N] is always equal to IcmpOutMsgs, as they
are updated at the same time. The sum of IcmpMsgInType[N] plus
IcmpInErrors should be equal or larger than IcmpInMsgs. When kernel
receives an ICMP packet, kernel follows below logic:
(IcmpMsgOutType[N] 之和总是等于 IcmpOutMsgs，因为它们是同时更新的。IcmpMsgInType[N] 之和加上 IcmpInErrors 应当等于或大于 IcmpInMsgs。内核收到一个 ICMP 数据包时，会遵循以下逻辑：)

1. increase IcmpInMsgs
   (增加 IcmpInMsgs)
2. if has any error, update IcmpInErrors and finish the process
   (如果有任何错误，更新 IcmpInErrors 并结束处理)
3. update IcmpMsgOutType[N]
   (更新 IcmpMsgOutType[N])
4. handle the packet depending on the type, if has any error, update
   IcmpInErrors and finish the process
   (根据类型处理该数据包，如果有任何错误，更新 IcmpInErrors 并结束处理)

So if all errors occur in step (2), IcmpInMsgs should be equal to the
sum of IcmpMsgOutType[N] plus IcmpInErrors. If all errors occur in
step (4), IcmpInMsgs should be equal to the sum of
IcmpMsgOutType[N]. If the errors occur in both step (2) and step (4),
IcmpInMsgs should be less than the sum of IcmpMsgOutType[N] plus
IcmpInErrors.
(因此，如果所有错误都发生在步骤 (2)，IcmpInMsgs 应等于 IcmpMsgOutType[N] 之和加上 IcmpInErrors。如果所有错误都发生在步骤 (4)，IcmpInMsgs 应等于 IcmpMsgOutType[N] 之和。如果错误同时发生在步骤 (2) 和步骤 (4)，IcmpInMsgs 应小于 IcmpMsgOutType[N] 之和加上 IcmpInErrors。)

General TCP counters(常规 TCP 计数器)
=====================================
* TcpInSegs

Defined in `RFC1213 tcpInSegs`_
(定义见 `RFC1213 tcpInSegs`_)

.. _RFC1213 tcpInSegs: https://tools.ietf.org/html/rfc1213#page-48

The number of packets received by the TCP layer. As mentioned in
RFC1213, it includes the packets received in error, such as checksum
error, invalid TCP header and so on. Only one error won't be included:
if the layer 2 destination address is not the NIC's layer 2
address. It might happen if the packet is a multicast or broadcast
packet, or the NIC is in promiscuous mode. In these situations, the
packets would be delivered to the TCP layer, but the TCP layer will discard
these packets before increasing TcpInSegs. The TcpInSegs counter
isn't aware of GRO. So if two packets are merged by GRO, the TcpInSegs
counter would only increase 1.
(TCP 层接收到的数据包数量。如 RFC1213 所述，它包含接收出错的报文，例如校验和错误、TCP 头无效等。只有一种错误不包含在内：即第 2 层目的地址不是网卡的第 2 层地址。当数据包是组播或广播数据包，或者网卡处于混杂模式时，就可能发生这种情况。在这些情况下，数据包会被交付给 TCP 层，但 TCP 层会在增加 TcpInSegs 之前丢弃这些数据包。TcpInSegs 计数器不感知 GRO。因此如果两个数据包被 GRO 合并，TcpInSegs 计数器只会增加 1。)

* TcpOutSegs

Defined in `RFC1213 tcpOutSegs`_
(定义见 `RFC1213 tcpOutSegs`_)

.. _RFC1213 tcpOutSegs: https://tools.ietf.org/html/rfc1213#page-48

The number of packets sent by the TCP layer. As mentioned in RFC1213,
it excludes the retransmitted packets. But it includes the SYN, ACK
and RST packets. Doesn't like TcpInSegs, the TcpOutSegs is aware of
GSO, so if a packet would be split to 2 by GSO, TcpOutSegs will
increase 2.
(TCP 层发送的数据包数量。如 RFC1213 所述，它不包含重传的数据包，但包含 SYN、ACK 和 RST 数据包。与 TcpInSegs 不同，TcpOutSegs 感知 GSO，因此如果一个数据包会被 GSO 拆分成 2 个，TcpOutSegs 将增加 2。)

* TcpActiveOpens

Defined in `RFC1213 tcpActiveOpens`_
(定义见 `RFC1213 tcpActiveOpens`_)

.. _RFC1213 tcpActiveOpens: https://tools.ietf.org/html/rfc1213#page-47

It means the TCP layer sends a SYN, and come into the SYN-SENT
state. Every time TcpActiveOpens increases 1, TcpOutSegs should always
increase 1.
(它表示 TCP 层发送了一个 SYN，并进入 SYN-SENT 状态。每当 TcpActiveOpens 增加 1 时，TcpOutSegs 也应当总是增加 1。)

* TcpPassiveOpens

Defined in `RFC1213 tcpPassiveOpens`_
(定义见 `RFC1213 tcpPassiveOpens`_)

.. _RFC1213 tcpPassiveOpens: https://tools.ietf.org/html/rfc1213#page-47

It means the TCP layer receives a SYN, replies a SYN+ACK, come into
the SYN-RCVD state.
(它表示 TCP 层收到一个 SYN，回复一个 SYN+ACK，并进入 SYN-RCVD 状态。)

* TcpExtTCPRcvCoalesce

When packets are received by the TCP layer and are not be read by the
application, the TCP layer will try to merge them. This counter
indicate how many packets are merged in such situation. If GRO is
enabled, lots of packets would be merged by GRO, these packets
wouldn't be counted to TcpExtTCPRcvCoalesce.
(当 TCP 层收到数据包而应用程序尚未读取时，TCP 层会尝试合并它们。该计数器表示在这种情况下合并了多少个数据包。如果启用了 GRO，大量数据包会被 GRO 合并，这些数据包不会计入 TcpExtTCPRcvCoalesce。)

* TcpExtTCPAutoCorking

When sending packets, the TCP layer will try to merge small packets to
a bigger one. This counter increase 1 for every packet merged in such
situation. Please refer to the LWN article for more details:
https://lwn.net/Articles/576263/
(发送数据包时，TCP 层会尝试把小数据包合并成更大的数据包。在这种情况下，每合并一个数据包，该计数器就增加 1。更多细节请参阅 LWN 文章：https://lwn.net/Articles/576263/)

* TcpExtTCPOrigDataSent

This counter is explained by kernel commit f19c29e3e391, I pasted the
explanation below::
(该计数器的解释来自内核提交 f19c29e3e391，我把解释粘贴在下面：)

  TCPOrigDataSent: number of outgoing packets with original data (excluding
  retransmission but including data-in-SYN). This counter is different from
  TcpOutSegs because TcpOutSegs also tracks pure ACKs. TCPOrigDataSent is
  more useful to track the TCP retransmission rate.

* TCPSynRetrans

This counter is explained by kernel commit f19c29e3e391, I pasted the
explanation below::
(该计数器的解释来自内核提交 f19c29e3e391，我把解释粘贴在下面：)

  TCPSynRetrans: number of SYN and SYN/ACK retransmits to break down
  retransmissions into SYN, fast-retransmits, timeout retransmits, etc.

* TCPFastOpenActiveFail

This counter is explained by kernel commit f19c29e3e391, I pasted the
explanation below::
(该计数器的解释来自内核提交 f19c29e3e391，我把解释粘贴在下面：)

  TCPFastOpenActiveFail: Fast Open attempts (SYN/data) failed because
  the remote does not accept it or the attempts timed out.

* TcpExtListenOverflows and TcpExtListenDrops
  (TcpExtListenOverflows 和 TcpExtListenDrops)

When kernel receives a SYN from a client, and if the TCP accept queue
is full, kernel will drop the SYN and add 1 to TcpExtListenOverflows.
At the same time kernel will also add 1 to TcpExtListenDrops. When a
TCP socket is in LISTEN state, and kernel need to drop a packet,
kernel would always add 1 to TcpExtListenDrops. So increase
TcpExtListenOverflows would let TcpExtListenDrops increasing at the
same time, but TcpExtListenDrops would also increase without
TcpExtListenOverflows increasing, e.g. a memory allocation fail would
also let TcpExtListenDrops increase.
(当内核收到来自客户端的 SYN 时，如果 TCP accept 队列已满，内核会丢弃该 SYN 并给 TcpExtListenOverflows 加 1。同时内核也会给 TcpExtListenDrops 加 1。当一个 TCP 套接字处于 LISTEN 状态且内核需要丢弃数据包时，内核总会给 TcpExtListenDrops 加 1。因此 TcpExtListenOverflows 增加时 TcpExtListenDrops 会同时增加，但 TcpExtListenDrops 也可能在 TcpExtListenOverflows 没有增加的情况下增加，例如内存分配失败也会使 TcpExtListenDrops 增加。)

Note: The above explanation is based on kernel 4.10 or above version, on
an old kernel, the TCP stack has different behavior when TCP accept
queue is full. On the old kernel, TCP stack won't drop the SYN, it
would complete the 3-way handshake. As the accept queue is full, TCP
stack will keep the socket in the TCP half-open queue. As it is in the
half open queue, TCP stack will send SYN+ACK on an exponential backoff
timer, after client replies ACK, TCP stack checks whether the accept
queue is still full, if it is not full, moves the socket to the accept
queue, if it is full, keeps the socket in the half-open queue, at next
time client replies ACK, this socket will get another chance to move
to the accept queue.
(注意：以上解释基于内核 4.10 及以上版本；在旧内核上，当 TCP accept 队列已满时，TCP 协议栈的行为有所不同。在旧内核上，TCP 协议栈不会丢弃 SYN，而是会完成三次握手。由于 accept 队列已满，TCP 协议栈会把套接字保存在 TCP 半连接 (half-open) 队列中。套接字位于半连接队列中时，TCP 协议栈会按指数退避定时器发送 SYN+ACK；客户端回复 ACK 后，TCP 协议栈检查 accept 队列是否仍然已满：如果未满，就把套接字移入 accept 队列；如果已满，就让套接字继续留在半连接队列中，下次客户端回复 ACK 时，该套接字会再次获得移入 accept 队列的机会。)


TCP Fast Open
=============
* TcpEstabResets

Defined in `RFC1213 tcpEstabResets`_.
(定义见 `RFC1213 tcpEstabResets`_。)

.. _RFC1213 tcpEstabResets: https://tools.ietf.org/html/rfc1213#page-48

* TcpAttemptFails

Defined in `RFC1213 tcpAttemptFails`_.
(定义见 `RFC1213 tcpAttemptFails`_。)

.. _RFC1213 tcpAttemptFails: https://tools.ietf.org/html/rfc1213#page-48

* TcpOutRsts

Defined in `RFC1213 tcpOutRsts`_. The RFC says this counter indicates
the 'segments sent containing the RST flag', but in linux kernel, this
counter indicates the segments kernel tried to send. The sending
process might be failed due to some errors (e.g. memory alloc failed).
(定义见 `RFC1213 tcpOutRsts`_。RFC 说该计数器表示"包含 RST 标志的已发送报段"，但在 Linux 内核中，该计数器表示内核尝试发送的报段。发送过程可能因某些错误(例如内存分配失败)而失败。)

.. _RFC1213 tcpOutRsts: https://tools.ietf.org/html/rfc1213#page-52

* TcpExtTCPSpuriousRtxHostQueues

When the TCP stack wants to retransmit a packet, and finds that packet
is not lost in the network, but the packet is not sent yet, the TCP
stack would give up the retransmission and update this counter. It
might happen if a packet stays too long time in a qdisc or driver
queue.
(当 TCP 协议栈想重传一个数据包，却发现该数据包并没有在网络中丢失、只是尚未发出时，TCP 协议栈会放弃重传并更新该计数器。当数据包在 qdisc 或驱动队列中停留时间过长时，就可能发生这种情况。)

* TcpEstabResets

The socket receives a RST packet in Establish or CloseWait state.
(套接字在 Establish 或 CloseWait 状态下收到一个 RST 数据包。)

* TcpExtTCPKeepAlive

This counter indicates many keepalive packets were sent. The keepalive
won't be enabled by default. A userspace program could enable it by
setting the SO_KEEPALIVE socket option.
(该计数器表示发送了许多 keepalive 数据包。keepalive 默认不会启用。用户态程序可以通过设置 SO_KEEPALIVE 套接字选项来启用它。)

* TcpExtTCPSpuriousRTOs

The spurious retransmission timeout detected by the `F-RTO`_
algorithm.
(由 `F-RTO`_ 算法检测到的伪重传超时。)

.. _F-RTO: https://tools.ietf.org/html/rfc5682

TCP Fast Path(TCP 快速路径)
===========================
When kernel receives a TCP packet, it has two paths to handler the
packet, one is fast path, another is slow path. The comment in kernel
code provides a good explanation of them, I pasted them below::
(内核收到一个 TCP 数据包后，有两条路径可以处理它：一条是快速路径，另一条是慢速路径。内核代码中的注释对它们有很好的解释，我把它粘贴在下面：)

  It is split into a fast path and a slow path. The fast path is
  disabled when:

  - A zero window was announced from us
  - zero window probing
    is only handled properly on the slow path.
  - Out of order segments arrived.
  - Urgent data is expected.
  - There is no buffer space left
  - Unexpected TCP flags/window values/header lengths are received
    (detected by checking the TCP header against pred_flags)
  - Data is sent in both directions. The fast path only supports pure senders
    or pure receivers (this means either the sequence number or the ack
    value must stay constant)
  - Unexpected TCP option.

Kernel will try to use fast path unless any of the above conditions
are satisfied. If the packets are out of order, kernel will handle
them in slow path, which means the performance might be not very
good. Kernel would also come into slow path if the "Delayed ack" is
used, because when using "Delayed ack", the data is sent in both
directions. When the TCP window scale option is not used, kernel will
try to enable fast path immediately when the connection comes into the
established state, but if the TCP window scale option is used, kernel
will disable the fast path at first, and try to enable it after kernel
receives packets.
(除非满足了上述某个条件，否则内核会尽量使用快速路径。如果数据包乱序到达，内核会在慢速路径中处理它们，这意味着性能可能不太好。如果使用了"延迟确认 (Delayed ack)"，内核也会进入慢速路径，因为使用"延迟确认"时数据是双向发送的。当未使用 TCP 窗口缩放 (window scale) 选项时，连接一进入 established 状态内核就会立即尝试启用快速路径；但如果使用了 TCP 窗口缩放选项，内核会先禁用快速路径，并在收到数据包后再尝试启用它。)

* TcpExtTCPPureAcks and TcpExtTCPHPAcks
  (TcpExtTCPPureAcks 和 TcpExtTCPHPAcks)

If a packet set ACK flag and has no data, it is a pure ACK packet, if
kernel handles it in the fast path, TcpExtTCPHPAcks will increase 1,
if kernel handles it in the slow path, TcpExtTCPPureAcks will
increase 1.
(如果一个数据包设置了 ACK 标志且没有数据，它就是纯 ACK 数据包。如果内核在快速路径中处理它，TcpExtTCPHPAcks 会增加 1；如果内核在慢速路径中处理它，TcpExtTCPPureAcks 会增加 1。)

* TcpExtTCPHPHits

If a TCP packet has data (which means it is not a pure ACK packet),
and this packet is handled in the fast path, TcpExtTCPHPHits will
increase 1.
(如果一个 TCP 数据包带有数据(即它不是纯 ACK 数据包)，并且该数据包在快速路径中被处理，TcpExtTCPHPHits 会增加 1。)


TCP abort(TCP 连接中止)
=======================
* TcpExtTCPAbortOnData

It means TCP layer has data in flight, but need to close the
connection. So TCP layer sends a RST to the other side, indicate the
connection is not closed very graceful. An easy way to increase this
counter is using the SO_LINGER option. Please refer to the SO_LINGER
section of the `socket man page`_:
(它表示 TCP 层还有在途数据，但需要关闭连接。因此 TCP 层向对端发送 RST，表明连接没有得到优雅的关闭。增加该计数器的一个简单方法是使用 SO_LINGER 选项。请参阅 `socket man page`_ 的 SO_LINGER 一节：)

.. _socket man page: http://man7.org/linux/man-pages/man7/socket.7.html

By default, when an application closes a connection, the close function
will return immediately and kernel will try to send the in-flight data
async. If you use the SO_LINGER option, set l_onoff to 1, and l_linger
to a positive number, the close function won't return immediately, but
wait for the in-flight data are acked by the other side, the max wait
time is l_linger seconds. If set l_onoff to 1 and set l_linger to 0,
when the application closes a connection, kernel will send a RST
immediately and increase the TcpExtTCPAbortOnData counter.
(默认情况下，当应用程序关闭连接时，close 函数会立即返回，内核会尝试异步发送在途数据。如果使用 SO_LINGER 选项，把 l_onoff 设为 1、把 l_linger 设为正数，close 函数就不会立即返回，而是等待在途数据被对端确认，最长等待 l_linger 秒。如果设置 l_onoff 为 1 且设置 l_linger 为 0，当应用程序关闭连接时，内核会立即发送 RST 并增加 TcpExtTCPAbortOnData 计数器。)

* TcpExtTCPAbortOnClose

This counter means the application has unread data in the TCP layer when
the application wants to close the TCP connection. In such a situation,
kernel will send a RST to the other side of the TCP connection.
(该计数器表示当应用程序想要关闭 TCP 连接时，TCP 层中还有应用程序未读取的数据。在这种情况下，内核会向 TCP 连接的对端发送 RST。)

* TcpExtTCPAbortOnMemory

When an application closes a TCP connection, kernel still need to track
the connection, let it complete the TCP disconnect process. E.g. an
app calls the close method of a socket, kernel sends fin to the other
side of the connection, then the app has no relationship with the
socket any more, but kernel need to keep the socket, this socket
becomes an orphan socket, kernel waits for the reply of the other side,
and would come to the TIME_WAIT state finally. When kernel has no
enough memory to keep the orphan socket, kernel would send an RST to
the other side, and delete the socket, in such situation, kernel will
increase 1 to the TcpExtTCPAbortOnMemory. Two conditions would trigger
TcpExtTCPAbortOnMemory:
(当应用程序关闭一个 TCP 连接时，内核仍需要跟踪该连接，让它完成 TCP 断开过程。例如，应用程序调用套接字的 close 方法，内核向连接的对端发送 fin，此后应用程序与该套接字就没有关系了，但内核需要保留该套接字，这个套接字就成为孤儿套接字 (orphan socket)，内核等待对端的回复，最终会进入 TIME_WAIT 状态。当内核没有足够的内存来保留孤儿套接字时，内核会向对端发送 RST 并删除该套接字，在这种情况下，内核会给 TcpExtTCPAbortOnMemory 加 1。有两种条件会触发 TcpExtTCPAbortOnMemory：)

1. the memory used by the TCP protocol is higher than the third value of
the tcp_mem. Please refer the tcp_mem section in the `TCP man page`_:
(TCP 协议使用的内存高于 tcp_mem 的第三个值。请参阅 `TCP man page`_ 中的 tcp_mem 一节：)

.. _TCP man page: http://man7.org/linux/man-pages/man7/tcp.7.html

2. the orphan socket count is higher than net.ipv4.tcp_max_orphans
   (孤儿套接字数量高于 net.ipv4.tcp_max_orphans)


* TcpExtTCPAbortOnTimeout

This counter will increase when any of the TCP timers expire. In such
situation, kernel won't send RST, just give up the connection.
(当任意一个 TCP 定时器到期时，该计数器会增加。在这种情况下，内核不会发送 RST，只是放弃该连接。)

* TcpExtTCPAbortOnLinger

When a TCP connection comes into FIN_WAIT_2 state, instead of waiting
for the fin packet from the other side, kernel could send a RST and
delete the socket immediately. This is not the default behavior of
Linux kernel TCP stack. By configuring the TCP_LINGER2 socket option,
you could let kernel follow this behavior.
(当 TCP 连接进入 FIN_WAIT_2 状态时，内核可以不等待对端的 fin 数据包，而是立即发送 RST 并删除套接字。这不是 Linux 内核 TCP 协议栈的默认行为。通过配置 TCP_LINGER2 套接字选项，你可以让内核采用这种行为。)

* TcpExtTCPAbortFailed

The kernel TCP layer will send RST if the `RFC2525 2.17 section`_ is
satisfied. If an internal error occurs during this process,
TcpExtTCPAbortFailed will be increased.
(如果满足 `RFC2525 2.17 section`_，内核 TCP 层会发送 RST。如果在此过程中发生内部错误，TcpExtTCPAbortFailed 会增加。)

.. _RFC2525 2.17 section: https://tools.ietf.org/html/rfc2525#page-50

TCP Hybrid Slow Start(TCP 混合慢启动)
=====================================
The Hybrid Slow Start algorithm is an enhancement of the traditional
TCP congestion window Slow Start algorithm. It uses two pieces of
information to detect whether the max bandwidth of the TCP path is
approached. The two pieces of information are ACK train length and
increase in packet delay. For detail information, please refer the
`Hybrid Slow Start paper`_. Either ACK train length or packet delay
hits a specific threshold, the congestion control algorithm will come
into the Congestion Avoidance state. Until v4.20, two congestion
control algorithms are using Hybrid Slow Start, they are cubic (the
default congestion control algorithm) and cdg. Four snmp counters
relate with the Hybrid Slow Start algorithm.
(混合慢启动 (Hybrid Slow Start) 算法是对传统 TCP 拥塞窗口慢启动算法的增强。它使用两项信息来检测是否已接近 TCP 路径的最大带宽，这两项信息是 ACK 列车长度 (ACK train length) 和数据包延迟的增加量。详细信息请参阅 `Hybrid Slow Start paper`_。只要 ACK 列车长度或数据包延迟达到特定阈值，拥塞控制算法就会进入拥塞避免 (Congestion Avoidance) 状态。直到 v4.20，有两个拥塞控制算法使用了混合慢启动，它们是 cubic(默认拥塞控制算法)和 cdg。有四个 snmp 计数器与混合慢启动算法相关。)

.. _Hybrid Slow Start paper: https://pdfs.semanticscholar.org/25e9/ef3f03315782c7f1cbcd31b587857adae7d1.pdf

* TcpExtTCPHystartTrainDetect

How many times the ACK train length threshold is detected
(检测到 ACK 列车长度阈值的次数)

* TcpExtTCPHystartTrainCwnd

The sum of CWND detected by ACK train length. Dividing this value by
TcpExtTCPHystartTrainDetect is the average CWND which detected by the
ACK train length.
(由 ACK 列车长度检测到的 CWND 之和。该值除以 TcpExtTCPHystartTrainDetect 就是 ACK 列车长度检测到的平均 CWND。)

* TcpExtTCPHystartDelayDetect

How many times the packet delay threshold is detected.
(检测到数据包延迟阈值的次数。)

* TcpExtTCPHystartDelayCwnd

The sum of CWND detected by packet delay. Dividing this value by
TcpExtTCPHystartDelayDetect is the average CWND which detected by the
packet delay.
(由数据包延迟检测到的 CWND 之和。该值除以 TcpExtTCPHystartDelayDetect 就是数据包延迟检测到的平均 CWND。)

TCP retransmission and congestion control(TCP 重传与拥塞控制)
=============================================================
The TCP protocol has two retransmission mechanisms: SACK and fast
recovery. They are exclusive with each other. When SACK is enabled,
the kernel TCP stack would use SACK, or kernel would use fast
recovery. The SACK is a TCP option, which is defined in `RFC2018`_,
the fast recovery is defined in `RFC6582`_, which is also called
'Reno'.
(TCP 协议有两种重传机制：SACK 和快速恢复 (fast recovery)。它们彼此互斥。启用 SACK 时，内核 TCP 协议栈会使用 SACK，否则内核会使用快速恢复。SACK 是一个 TCP 选项，定义于 `RFC2018`_；快速恢复定义于 `RFC6582`_，也称为 "Reno"。)

The TCP congestion control is a big and complex topic. To understand
the related snmp counter, we need to know the states of the congestion
control state machine. There are 5 states: Open, Disorder, CWR,
Recovery and Loss. For details about these states, please refer page 5
and page 6 of this document:
https://pdfs.semanticscholar.org/0e9c/968d09ab2e53e24c4dca5b2d67c7f7140f8e.pdf
(TCP 拥塞控制是一个庞大而复杂的话题。要理解相关的 snmp 计数器，我们需要了解拥塞控制状态机的各个状态。共有 5 个状态：Open、Disorder、CWR、Recovery 和 Loss。关于这些状态的详细信息，请参阅该文档的第 5 页和第 6 页：https://pdfs.semanticscholar.org/0e9c/968d09ab2e53e24c4dca5b2d67c7f7140f8e.pdf)

.. _RFC2018: https://tools.ietf.org/html/rfc2018
.. _RFC6582: https://tools.ietf.org/html/rfc6582

* TcpExtTCPRenoRecovery and TcpExtTCPSackRecovery
  (TcpExtTCPRenoRecovery 和 TcpExtTCPSackRecovery)

When the congestion control comes into Recovery state, if sack is
used, TcpExtTCPSackRecovery increases 1, if sack is not used,
TcpExtTCPRenoRecovery increases 1. These two counters mean the TCP
stack begins to retransmit the lost packets.
(当拥塞控制进入 Recovery 状态时，如果使用了 SACK，TcpExtTCPSackRecovery 增加 1；如果没有使用 SACK，TcpExtTCPRenoRecovery 增加 1。这两个计数器表示 TCP 协议栈开始重传丢失的数据包。)

* TcpExtTCPSACKReneging

A packet was acknowledged by SACK, but the receiver has dropped this
packet, so the sender needs to retransmit this packet. In this
situation, the sender adds 1 to TcpExtTCPSACKReneging. A receiver
could drop a packet which has been acknowledged by SACK, although it is
unusual, it is allowed by the TCP protocol. The sender doesn't really
know what happened on the receiver side. The sender just waits until
the RTO expires for this packet, then the sender assumes this packet
has been dropped by the receiver.
(一个数据包已被 SACK 确认，但接收方却丢弃了它，因此发送方需要重传该数据包。在这种情况下，发送方给 TcpExtTCPSACKReneging 加 1。接收方可以丢弃一个已被 SACK 确认的数据包，虽然这不常见，但 TCP 协议允许这样做。发送方并不真正知道接收方那边发生了什么，它只是等待该数据包的 RTO 到期，然后认定该数据包已被接收方丢弃。)

* TcpExtTCPRenoReorder

The reorder packet is detected by fast recovery. It would only be used
if SACK is disabled. The fast recovery algorithm detects recorder by
the duplicate ACK number. E.g., if retransmission is triggered, and
the original retransmitted packet is not lost, it is just out of
order, the receiver would acknowledge multiple times, one for the
retransmitted packet, another for the arriving of the original out of
order packet. Thus the sender would find more ACks than its
expectation, and the sender knows out of order occurs.
(乱序数据包由快速恢复检测到。只有在禁用 SACK 时才会使用快速恢复。快速恢复算法通过重复 ACK 的数量来检测乱序。例如，如果触发了重传，而最初被重传的数据包并没有丢失、只是乱序了，接收方会确认多次：一次是针对重传的数据包，另一次是针对原本乱序的数据包的到达。这样发送方会发现收到的 ACK 比预期的多，从而知道发生了乱序。)

* TcpExtTCPTSReorder

The reorder packet is detected when a hole is filled. E.g., assume the
sender sends packet 1,2,3,4,5, and the receiving order is
1,2,4,5,3. When the sender receives the ACK of packet 3 (which will
fill the hole), two conditions will let TcpExtTCPTSReorder increase
1: (1) if the packet 3 is not re-retransmitted yet. (2) if the packet
3 is retransmitted but the timestamp of the packet 3's ACK is earlier
than the retransmission timestamp.
(当空洞被填补时检测到乱序数据包。例如，假设发送方发送数据包 1、2、3、4、5，接收顺序为 1、2、4、5、3。当发送方收到数据包 3 的 ACK(它将填补空洞)时，有两种情况会让 TcpExtTCPTSReorder 增加 1：(1) 数据包 3 尚未被再次重传。(2) 数据包 3 已被重传，但数据包 3 的 ACK 的时间戳早于重传的时间戳。)

* TcpExtTCPSACKReorder

The reorder packet detected by SACK. The SACK has two methods to
detect reorder: (1) DSACK is received by the sender. It means the
sender sends the same packet more than one times. And the only reason
is the sender believes an out of order packet is lost so it sends the
packet again. (2) Assume packet 1,2,3,4,5 are sent by the sender, and
the sender has received SACKs for packet 2 and 5, now the sender
receives SACK for packet 4 and the sender doesn't retransmit the
packet yet, the sender would know packet 4 is out of order. The TCP
stack of kernel will increase TcpExtTCPSACKReorder for both of the
above scenarios.
(由 SACK 检测到的乱序数据包。SACK 有两种检测乱序的方法：(1) 发送方收到了 DSACK。这意味着发送方把同一个数据包发送了不止一次，而唯一的原因是发送方认为某个乱序数据包丢失了，所以又发送了一次。(2) 假设发送方发送了数据包 1、2、3、4、5，并且已收到数据包 2 和 5 的 SACK，现在发送方收到了数据包 4 的 SACK 而尚未重传该数据包，发送方就会知道数据包 4 乱序了。对于上述两种场景，内核的 TCP 协议栈都会增加 TcpExtTCPSACKReorder。)

* TcpExtTCPSlowStartRetrans

The TCP stack wants to retransmit a packet and the congestion control
state is 'Loss'.
(TCP 协议栈想重传一个数据包，且拥塞控制状态为 "Loss"。)

* TcpExtTCPFastRetrans

The TCP stack wants to retransmit a packet and the congestion control
state is not 'Loss'.
(TCP 协议栈想重传一个数据包，且拥塞控制状态不是 "Loss"。)

* TcpExtTCPLostRetransmit

A SACK points out that a retransmission packet is lost again.
(一个 SACK 指出某个重传数据包又一次丢失了。)

* TcpExtTCPRetransFail

The TCP stack tries to deliver a retransmission packet to lower layers
but the lower layers return an error.
(TCP 协议栈尝试把一个重传数据包交付给下层，但下层返回了错误。)

* TcpExtTCPSynRetrans

The TCP stack retransmits a SYN packet.
(TCP 协议栈重传一个 SYN 数据包。)

DSACK
=====
The DSACK is defined in `RFC2883`_. The receiver uses DSACK to report
duplicate packets to the sender. There are two kinds of
duplications: (1) a packet which has been acknowledged is
duplicate. (2) an out of order packet is duplicate. The TCP stack
counts these two kinds of duplications on both receiver side and
sender side.
(DSACK 定义于 `RFC2883`_。接收方使用 DSACK 向发送方报告重复的数据包。重复有两种情况：(1) 一个已被确认的数据包是重复的。(2) 一个乱序数据包是重复的。TCP 协议栈在接收方和发送方两侧都会对这两种重复情况进行计数。)

.. _RFC2883 : https://tools.ietf.org/html/rfc2883

* TcpExtTCPDSACKOldSent

The TCP stack receives a duplicate packet which has been acked, so it
sends a DSACK to the sender.
(TCP 协议栈收到一个已被确认过的重复数据包，因此向发送方发送一个 DSACK。)

* TcpExtTCPDSACKOfoSent

The TCP stack receives an out of order duplicate packet, so it sends a
DSACK to the sender.
(TCP 协议栈收到一个乱序的重复数据包，因此向发送方发送一个 DSACK。)

* TcpExtTCPDSACKRecv

The TCP stack receives a DSACK, which indicates an acknowledged
duplicate packet is received.
(TCP 协议栈收到一个 DSACK，表明收到了一个已被确认过的重复数据包。)

* TcpExtTCPDSACKOfoRecv

The TCP stack receives a DSACK, which indicate an out of order
duplicate packet is received.
(TCP 协议栈收到一个 DSACK，表明收到了一个乱序的重复数据包。)

invalid SACK and DSACK(无效的 SACK 与 DSACK)
============================================
When a SACK (or DSACK) block is invalid, a corresponding counter would
be updated. The validation method is base on the start/end sequence
number of the SACK block. For more details, please refer the comment
of the function tcp_is_sackblock_valid in the kernel source code. A
SACK option could have up to 4 blocks, they are checked
individually. E.g., if 3 blocks of a SACk is invalid, the
corresponding counter would be updated 3 times. The comment of commit
18f02545a9a1 ("[TCP] MIB: Add counters for discarded SACK blocks")
has additional explanation:
(当一个 SACK(或 DSACK)块无效时，相应的计数器会被更新。校验方法基于 SACK 块的起始/结束序列号。更多细节请参阅内核源码中 tcp_is_sackblock_valid 函数的注释。一个 SACK 选项最多可以有 4 个块，它们会被逐个检查。例如，如果一个 SACk 有 3 个块无效，相应的计数器就会被更新 3 次。提交 18f02545a9a1("[TCP] MIB: Add counters for discarded SACK blocks")的注释有补充说明：)

* TcpExtTCPSACKDiscard

This counter indicates how many SACK blocks are invalid. If the invalid
SACK block is caused by ACK recording, the TCP stack will only ignore
it and won't update this counter.
(该计数器表示有多少个 SACK 块无效。如果无效的 SACK 块是由 ACK 记录 (ACK recording) 导致的，TCP 协议栈只会忽略它，而不会更新该计数器。)

* TcpExtTCPDSACKIgnoredOld and TcpExtTCPDSACKIgnoredNoUndo
  (TcpExtTCPDSACKIgnoredOld 和 TcpExtTCPDSACKIgnoredNoUndo)

When a DSACK block is invalid, one of these two counters would be
updated. Which counter will be updated depends on the undo_marker flag
of the TCP socket. If the undo_marker is not set, the TCP stack isn't
likely to re-transmit any packets, and we still receive an invalid
DSACK block, the reason might be that the packet is duplicated in the
middle of the network. In such scenario, TcpExtTCPDSACKIgnoredNoUndo
will be updated. If the undo_marker is set, TcpExtTCPDSACKIgnoredOld
will be updated. As implied in its name, it might be an old packet.
(当一个 DSACK 块无效时，这两个计数器之一会被更新。更新哪一个取决于 TCP 套接字的 undo_marker 标志。如果 undo_marker 未设置，TCP 协议栈不太可能重传任何数据包，而我们仍然收到了一个无效的 DSACK 块，原因可能是数据包在网络中间被复制了。在这种情况下，TcpExtTCPDSACKIgnoredNoUndo 会被更新。如果 undo_marker 已设置，TcpExtTCPDSACKIgnoredOld 会被更新。正如其名字所暗示的，它可能是一个旧数据包。)

SACK shift(SACK 移位)
=====================
The linux networking stack stores data in sk_buff struct (skb for
short). If a SACK block acrosses multiple skb, the TCP stack will try
to re-arrange data in these skb. E.g. if a SACK block acknowledges seq
10 to 15, skb1 has seq 10 to 13, skb2 has seq 14 to 20. The seq 14 and
15 in skb2 would be moved to skb1. This operation is 'shift'. If a
SACK block acknowledges seq 10 to 20, skb1 has seq 10 to 13, skb2 has
seq 14 to 20. All data in skb2 will be moved to skb1, and skb2 will be
discard, this operation is 'merge'.
(Linux 网络协议栈把数据存储在 sk_buff 结构(简称 skb)中。如果一个 SACK 块跨越多个 skb，TCP 协议栈会尝试重新整理这些 skb 中的数据。例如，一个 SACK 块确认了序列号 10 到 15，skb1 拥有序列号 10 到 13，skb2 拥有序列号 14 到 20，那么 skb2 中序列号 14 和 15 的数据会被移到 skb1 中。这个操作叫 "shift"(移位)。如果一个 SACK 块确认了序列号 10 到 20，skb1 拥有序列号 10 到 13，skb2 拥有序列号 14 到 20，那么 skb2 中的所有数据都会被移到 skb1 中，skb2 会被丢弃，这个操作叫 "merge"(合并)。)

* TcpExtTCPSackShifted

A skb is shifted
(一个 skb 被移位了)

* TcpExtTCPSackMerged

A skb is merged
(一个 skb 被合并了)

* TcpExtTCPSackShiftFallback

A skb should be shifted or merged, but the TCP stack doesn't do it for
some reasons.
(一个 skb 本应被移位或合并，但 TCP 协议栈由于某些原因没有这样做。)

TCP out of order(TCP 乱序)
==========================
* TcpExtTCPOFOQueue

The TCP layer receives an out of order packet and has enough memory
to queue it.
(TCP 层收到一个乱序数据包，并且有足够的内存将其排队。)

* TcpExtTCPOFODrop

The TCP layer receives an out of order packet but doesn't have enough
memory, so drops it. Such packets won't be counted into
TcpExtTCPOFOQueue.
(TCP 层收到一个乱序数据包但没有足够的内存，因此将其丢弃。这类数据包不会计入 TcpExtTCPOFOQueue。)

* TcpExtTCPOFOMerge

The received out of order packet has an overlay with the previous
packet. the overlay part will be dropped. All of TcpExtTCPOFOMerge
packets will also be counted into TcpExtTCPOFOQueue.
(收到的乱序数据包与前一个数据包有重叠，重叠部分会被丢弃。所有计入 TcpExtTCPOFOMerge 的数据包也会被计入 TcpExtTCPOFOQueue。)

TCP PAWS
========
PAWS (Protection Against Wrapped Sequence numbers) is an algorithm
which is used to drop old packets. It depends on the TCP
timestamps. For detail information, please refer the `timestamp wiki`_
and the `RFC of PAWS`_.
(PAWS (Protection Against Wrapped Sequence numbers，防止序列号回绕) 是一种用于丢弃旧数据包的算法。它依赖于 TCP 时间戳。详细信息请参阅 `timestamp wiki`_ 和 `RFC of PAWS`_。)

.. _RFC of PAWS: https://tools.ietf.org/html/rfc1323#page-17
.. _timestamp wiki: https://en.wikipedia.org/wiki/Transmission_Control_Protocol#TCP_timestamps

* TcpExtPAWSActive

Packets are dropped by PAWS in Syn-Sent status.
(在 Syn-Sent 状态下被 PAWS 丢弃的数据包。)

* TcpExtPAWSEstab

Packets are dropped by PAWS in any status other than Syn-Sent.
(在 Syn-Sent 以外的任何状态下被 PAWS 丢弃的数据包。)

TCP ACK skip(TCP ACK 跳过)
==========================
In some scenarios, kernel would avoid sending duplicate ACKs too
frequently. Please find more details in the tcp_invalid_ratelimit
section of the `sysctl document`_. When kernel decides to skip an ACK
due to tcp_invalid_ratelimit, kernel would update one of below
counters to indicate the ACK is skipped in which scenario. The ACK
would only be skipped if the received packet is either a SYN packet or
it has no data.
(在某些场景下，内核会避免过于频繁地发送重复 ACK。更多细节请参阅 `sysctl document`_ 中 tcp_invalid_ratelimit 一节。当内核因 tcp_invalid_ratelimit 决定跳过一个 ACK 时，会更新下列计数器之一，以表明该 ACK 是在哪种场景下被跳过的。只有当收到的数据包是 SYN 数据包或者不含数据时，ACK 才会被跳过。)

.. _sysctl document: https://www.kernel.org/doc/Documentation/networking/ip-sysctl.rst

* TcpExtTCPACKSkippedSynRecv

The ACK is skipped in Syn-Recv status. The Syn-Recv status means the
TCP stack receives a SYN and replies SYN+ACK. Now the TCP stack is
waiting for an ACK. Generally, the TCP stack doesn't need to send ACK
in the Syn-Recv status. But in several scenarios, the TCP stack need
to send an ACK. E.g., the TCP stack receives the same SYN packet
repeately, the received packet does not pass the PAWS check, or the
received packet sequence number is out of window. In these scenarios,
the TCP stack needs to send ACK. If the ACk sending frequency is higher than
tcp_invalid_ratelimit allows, the TCP stack will skip sending ACK and
increase TcpExtTCPACKSkippedSynRecv.
(在 Syn-Recv 状态下跳过的 ACK。Syn-Recv 状态表示 TCP 协议栈收到一个 SYN 并回复了 SYN+ACK，此时 TCP 协议栈正在等待 ACK。一般来说，TCP 协议栈在 Syn-Recv 状态下不需要发送 ACK。但有几种场景需要发送 ACK，例如：TCP 协议栈重复收到同一个 SYN 数据包、收到的数据包未通过 PAWS 检查，或者收到数据包的序列号超出窗口。在这些场景下，TCP 协议栈需要发送 ACK。如果 ACK 的发送频率高于 tcp_invalid_ratelimit 所允许的值，TCP 协议栈将跳过发送 ACK 并增加 TcpExtTCPACKSkippedSynRecv。)


* TcpExtTCPACKSkippedPAWS

The ACK is skipped due to PAWS (Protect Against Wrapped Sequence
numbers) check fails. If the PAWS check fails in Syn-Recv, Fin-Wait-2
or Time-Wait statuses, the skipped ACK would be counted to
TcpExtTCPACKSkippedSynRecv, TcpExtTCPACKSkippedFinWait2 or
TcpExtTCPACKSkippedTimeWait. In all other statuses, the skipped ACK
would be counted to TcpExtTCPACKSkippedPAWS.
(因 PAWS (Protect Against Wrapped Sequence numbers) 检查失败而跳过的 ACK。如果 PAWS 检查在 Syn-Recv、Fin-Wait-2 或 Time-Wait 状态下失败，被跳过的 ACK 会分别计入 TcpExtTCPACKSkippedSynRecv、TcpExtTCPACKSkippedFinWait2 或 TcpExtTCPACKSkippedTimeWait。在所有其他状态下，被跳过的 ACK 计入 TcpExtTCPACKSkippedPAWS。)

* TcpExtTCPACKSkippedSeq

The sequence number is out of window and the timestamp passes the PAWS
check and the TCP status is not Syn-Recv, Fin-Wait-2, and Time-Wait.
(序列号超出窗口、时间戳通过了 PAWS 检查，且 TCP 状态不是 Syn-Recv、Fin-Wait-2 和 Time-Wait。)

* TcpExtTCPACKSkippedFinWait2

The ACK is skipped in Fin-Wait-2 status, the reason would be either
PAWS check fails or the received sequence number is out of window.
(在 Fin-Wait-2 状态下跳过的 ACK，原因可能是 PAWS 检查失败，或者收到的序列号超出窗口。)

* TcpExtTCPACKSkippedTimeWait

The ACK is skipped in Time-Wait status, the reason would be either
PAWS check failed or the received sequence number is out of window.
(在 Time-Wait 状态下跳过的 ACK，原因可能是 PAWS 检查失败，或者收到的序列号超出窗口。)

* TcpExtTCPACKSkippedChallenge

The ACK is skipped if the ACK is a challenge ACK. The RFC 5961 defines
3 kind of challenge ACK, please refer `RFC 5961 section 3.2`_,
`RFC 5961 section 4.2`_ and `RFC 5961 section 5.2`_. Besides these
three scenarios, In some TCP status, the linux TCP stack would also
send challenge ACKs if the ACK number is before the first
unacknowledged number (more strict than `RFC 5961 section 5.2`_).
(当该 ACK 是挑战 ACK (challenge ACK) 时会被跳过。RFC 5961 定义了 3 种挑战 ACK，请参阅 `RFC 5961 section 3.2`_、`RFC 5961 section 4.2`_ 和 `RFC 5961 section 5.2`_。除了这三种场景之外，在某些 TCP 状态下，如果 ACK 号小于第一个未确认的序号，Linux TCP 协议栈也会发送挑战 ACK(比 `RFC 5961 section 5.2`_ 更严格)。)

.. _RFC 5961 section 3.2: https://tools.ietf.org/html/rfc5961#page-7
.. _RFC 5961 section 4.2: https://tools.ietf.org/html/rfc5961#page-9
.. _RFC 5961 section 5.2: https://tools.ietf.org/html/rfc5961#page-11

TCP receive window(TCP 接收窗口)
================================
* TcpExtTCPWantZeroWindowAdv

Depending on current memory usage, the TCP stack tries to set receive
window to zero. But the receive window might still be a no-zero
value. For example, if the previous window size is 10, and the TCP
stack receives 3 bytes, the current window size would be 7 even if the
window size calculated by the memory usage is zero.
(根据当前内存使用情况，TCP 协议栈会尝试把接收窗口设为零。但接收窗口可能仍是非零值。例如，如果之前的窗口大小是 10，TCP 协议栈又收到 3 个字节，那么即使按内存使用量计算出的窗口大小为零，当前窗口大小也会是 7。)

* TcpExtTCPToZeroWindowAdv

The TCP receive window is set to zero from a no-zero value.
(TCP 接收窗口从一个非零值被设置为零。)

* TcpExtTCPFromZeroWindowAdv

The TCP receive window is set to no-zero value from zero.
(TCP 接收窗口从零被设置为非零值。)


Delayed ACK(延迟 ACK)
=====================
The TCP Delayed ACK is a technique which is used for reducing the
packet count in the network. For more details, please refer the
`Delayed ACK wiki`_
(TCP 延迟确认 (Delayed ACK) 是一种用于减少网络中数据包数量的技术。更多细节请参阅 `Delayed ACK wiki`_)

.. _Delayed ACK wiki: https://en.wikipedia.org/wiki/TCP_delayed_acknowledgment

* TcpExtDelayedACKs

A delayed ACK timer expires. The TCP stack will send a pure ACK packet
and exit the delayed ACK mode.
(延迟确认定时器到期。TCP 协议栈将发送一个纯 ACK 数据包并退出延迟确认模式。)

* TcpExtDelayedACKLocked

A delayed ACK timer expires, but the TCP stack can't send an ACK
immediately due to the socket is locked by a userspace program. The
TCP stack will send a pure ACK later (after the userspace program
unlock the socket). When the TCP stack sends the pure ACK later, the
TCP stack will also update TcpExtDelayedACKs and exit the delayed ACK
mode.
(延迟确认定时器到期，但由于套接字被用户态程序锁定，TCP 协议栈无法立即发送 ACK。TCP 协议栈稍后会发送纯 ACK(在用户态程序解锁套接字之后)。当 TCP 协议栈稍后发送纯 ACK 时，也会更新 TcpExtDelayedACKs 并退出延迟确认模式。)

* TcpExtDelayedACKLost

It will be updated when the TCP stack receives a packet which has been
ACKed. A Delayed ACK loss might cause this issue, but it would also be
triggered by other reasons, such as a packet is duplicated in the
network.
(当 TCP 协议栈收到一个已被 ACK 过的数据包时，该计数器会被更新。延迟确认丢失可能导致这种情况，但它也可能由其他原因触发，例如数据包在网络中被复制。)

Tail Loss Probe (TLP)(尾丢失探测 (TLP))
=======================================
TLP is an algorithm which is used to detect TCP packet loss. For more
details, please refer the `TLP paper`_.
(TLP 是一种用于检测 TCP 数据包丢失的算法。更多细节请参阅 `TLP paper`_。)

.. _TLP paper: https://tools.ietf.org/html/draft-dukkipati-tcpm-tcp-loss-probe-01

* TcpExtTCPLossProbes

A TLP probe packet is sent.
(发送了一个 TLP 探测数据包。)

* TcpExtTCPLossProbeRecovery

A packet loss is detected and recovered by TLP.
(通过 TLP 检测到一次数据包丢失并将其恢复。)

TCP Fast Open description(TCP Fast Open 描述)
=============================================
TCP Fast Open is a technology which allows data transfer before the
3-way handshake complete. Please refer the `TCP Fast Open wiki`_ for a
general description.
(TCP Fast Open 是一种允许在三次握手完成之前进行数据传输的技术。一般性描述请参阅 `TCP Fast Open wiki`_。)

.. _TCP Fast Open wiki: https://en.wikipedia.org/wiki/TCP_Fast_Open

* TcpExtTCPFastOpenActive

When the TCP stack receives an ACK packet in the SYN-SENT status, and
the ACK packet acknowledges the data in the SYN packet, the TCP stack
understand the TFO cookie is accepted by the other side, then it
updates this counter.
(当 TCP 协议栈在 SYN-SENT 状态下收到一个 ACK 数据包，且该 ACK 确认了 SYN 数据包中的数据时，TCP 协议栈便知道 TFO cookie 已被对端接受，随后更新该计数器。)

* TcpExtTCPFastOpenActiveFail

This counter indicates that the TCP stack initiated a TCP Fast Open,
but it failed. This counter would be updated in three scenarios: (1)
the other side doesn't acknowledge the data in the SYN packet. (2) The
SYN packet which has the TFO cookie is timeout at least once. (3)
after the 3-way handshake, the retransmission timeout happens
net.ipv4.tcp_retries1 times, because some middle-boxes may black-hole
fast open after the handshake.
(该计数器表示 TCP 协议栈发起了一次 TCP Fast Open 但失败了。该计数器会在三种场景下被更新：(1) 对端没有确认 SYN 数据包中的数据。(2) 携带 TFO cookie 的 SYN 数据包至少超时一次。(3) 三次握手完成后，重传超时发生了 net.ipv4.tcp_retries1 次，因为某些中间设备可能在握手后对快速打开 (fast open) 进行黑洞处理。)

* TcpExtTCPFastOpenPassive

This counter indicates how many times the TCP stack accepts the fast
open request.
(该计数器表示 TCP 协议栈接受快速打开请求的次数。)

* TcpExtTCPFastOpenPassiveFail

This counter indicates how many times the TCP stack rejects the fast
open request. It is caused by either the TFO cookie is invalid or the
TCP stack finds an error during the socket creating process.
(该计数器表示 TCP 协议栈拒绝快速打开请求的次数。原因要么是 TFO cookie 无效，要么是 TCP 协议栈在创建套接字的过程中发现错误。)

* TcpExtTCPFastOpenListenOverflow

When the pending fast open request number is larger than
fastopenq->max_qlen, the TCP stack will reject the fast open request
and update this counter. When this counter is updated, the TCP stack
won't update TcpExtTCPFastOpenPassive or
TcpExtTCPFastOpenPassiveFail. The fastopenq->max_qlen is set by the
TCP_FASTOPEN socket operation and it could not be larger than
net.core.somaxconn. For example:
(当挂起的快速打开请求数量大于 fastopenq->max_qlen 时，TCP 协议栈会拒绝快速打开请求并更新该计数器。该计数器被更新时，TCP 协议栈不会更新 TcpExtTCPFastOpenPassive 或 TcpExtTCPFastOpenPassiveFail。fastopenq->max_qlen 由 TCP_FASTOPEN 套接字选项设置，且不能大于 net.core.somaxconn。例如：)

setsockopt(sfd, SOL_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen));

* TcpExtTCPFastOpenCookieReqd

This counter indicates how many times a client wants to request a TFO
cookie.
(该计数器表示客户端想要请求 TFO cookie 的次数。)

SYN cookies
===========
SYN cookies are used to mitigate SYN flood, for details, please refer
the `SYN cookies wiki`_.
(SYN cookies 用于缓解 SYN flood(洪泛)攻击，详细信息请参阅 `SYN cookies wiki`_。)

.. _SYN cookies wiki: https://en.wikipedia.org/wiki/SYN_cookies

* TcpExtSyncookiesSent

It indicates how many SYN cookies are sent.
(它表示发送了多少个 SYN cookie。)

* TcpExtSyncookiesRecv

How many reply packets of the SYN cookies the TCP stack receives.
(TCP 协议栈收到了多少个 SYN cookie 的应答数据包。)

* TcpExtSyncookiesFailed

The MSS decoded from the SYN cookie is invalid. When this counter is
updated, the received packet won't be treated as a SYN cookie and the
TcpExtSyncookiesRecv counter won't be updated.
(从 SYN cookie 中解出的 MSS 无效。该计数器被更新时，收到的数据包不会被当作 SYN cookie 处理，TcpExtSyncookiesRecv 计数器也不会被更新。)

Challenge ACK(挑战 ACK)
=======================
For details of challenge ACK, please refer the explanation of
TcpExtTCPACKSkippedChallenge.
(关于挑战 ACK 的详细信息，请参阅 TcpExtTCPACKSkippedChallenge 的解释。)

* TcpExtTCPChallengeACK

The number of challenge acks sent.
(发送的挑战 ACK 的数量。)

* TcpExtTCPSYNChallenge

The number of challenge acks sent in response to SYN packets. After
updates this counter, the TCP stack might send a challenge ACK and
update the TcpExtTCPChallengeACK counter, or it might also skip to
send the challenge and update the TcpExtTCPACKSkippedChallenge.
(为响应 SYN 数据包而发送的挑战 ACK 的数量。更新该计数器后，TCP 协议栈可能发送一个挑战 ACK 并更新 TcpExtTCPChallengeACK 计数器，也可能跳过发送挑战并更新 TcpExtTCPACKSkippedChallenge。)

prune(修剪)
===========
When a socket is under memory pressure, the TCP stack will try to
reclaim memory from the receiving queue and out of order queue. One of
the reclaiming method is 'collapse', which means allocate a big skb,
copy the contiguous skbs to the single big skb, and free these
contiguous skbs.
(当套接字面临内存压力时，TCP 协议栈会尝试从接收队列和乱序队列中回收内存。回收方法之一是 "collapse"(合并压缩)，即分配一个大 skb，把连续的 skb 复制到这个大 skb 中，然后释放这些连续的 skb。)

* TcpExtPruneCalled

The TCP stack tries to reclaim memory for a socket. After updates this
counter, the TCP stack will try to collapse the out of order queue and
the receiving queue. If the memory is still not enough, the TCP stack
will try to discard packets from the out of order queue (and update the
TcpExtOfoPruned counter)
(TCP 协议栈尝试为一个套接字回收内存。更新该计数器后，TCP 协议栈会尝试对乱序队列和接收队列进行 collapse(合并压缩)。如果内存仍然不够，TCP 协议栈会尝试从乱序队列中丢弃数据包(并更新 TcpExtOfoPruned 计数器))

* TcpExtOfoPruned

The TCP stack tries to discard packet on the out of order queue.
(TCP 协议栈尝试丢弃乱序队列中的数据包。)

* TcpExtRcvPruned

After 'collapse' and discard packets from the out of order queue, if
the actually used memory is still larger than the max allowed memory,
this counter will be updated. It means the 'prune' fails.
(在执行 "collapse" 并从乱序队列丢弃数据包之后，如果实际使用的内存仍然大于允许的最大内存，该计数器会被更新。它表示 "prune"(修剪)失败了。)

* TcpExtTCPRcvCollapsed

This counter indicates how many skbs are freed during 'collapse'.
(该计数器表示在 "collapse" 过程中释放了多少个 skb。)

examples(示例)
==============

ping test(ping 测试)
--------------------
Run the ping command against the public dns server 8.8.8.8::
(对公共 DNS 服务器 8.8.8.8 执行 ping 命令：)

  nstatuser@nstat-a:~$ ping 8.8.8.8 -c 1
  PING 8.8.8.8 (8.8.8.8) 56(84) bytes of data.
  64 bytes from 8.8.8.8: icmp_seq=1 ttl=119 time=17.8 ms

  --- 8.8.8.8 ping statistics ---
  1 packets transmitted, 1 received, 0% packet loss, time 0ms
  rtt min/avg/max/mdev = 17.875/17.875/17.875/0.000 ms

The nstayt result::
(nstat 结果如下：)

  nstatuser@nstat-a:~$ nstat
  #kernel
  IpInReceives                    1                  0.0
  IpInDelivers                    1                  0.0
  IpOutRequests                   1                  0.0
  IcmpInMsgs                      1                  0.0
  IcmpInEchoReps                  1                  0.0
  IcmpOutMsgs                     1                  0.0
  IcmpOutEchos                    1                  0.0
  IcmpMsgInType0                  1                  0.0
  IcmpMsgOutType8                 1                  0.0
  IpExtInOctets                   84                 0.0
  IpExtOutOctets                  84                 0.0
  IpExtInNoECTPkts                1                  0.0

The Linux server sent an ICMP Echo packet, so IpOutRequests,
IcmpOutMsgs, IcmpOutEchos and IcmpMsgOutType8 were increased 1. The
server got ICMP Echo Reply from 8.8.8.8, so IpInReceives, IcmpInMsgs,
IcmpInEchoReps and IcmpMsgInType0 were increased 1. The ICMP Echo Reply
was passed to the ICMP layer via IP layer, so IpInDelivers was
increased 1. The default ping data size is 48, so an ICMP Echo packet
and its corresponding Echo Reply packet are constructed by:
(Linux 服务器发送了一个 ICMP Echo 数据包，因此 IpOutRequests、IcmpOutMsgs、IcmpOutEchos 和 IcmpMsgOutType8 各增加 1。服务器从 8.8.8.8 收到 ICMP Echo Reply，因此 IpInReceives、IcmpInMsgs、IcmpInEchoReps 和 IcmpMsgInType0 各增加 1。ICMP Echo Reply 经 IP 层传递给 ICMP 层，因此 IpInDelivers 增加 1。ping 的默认数据大小是 48 字节，因此一个 ICMP Echo 数据包及其对应的 Echo Reply 数据包由以下部分构成：)

* 14 bytes MAC header
  (14 字节 MAC 头)
* 20 bytes IP header
  (20 字节 IP 头)
* 16 bytes ICMP header
  (16 字节 ICMP 头)
* 48 bytes data (default value of the ping command)
  (48 字节数据(ping 命令的默认值))

So the IpExtInOctets and IpExtOutOctets are 20+16+48=84.
(所以 IpExtInOctets 和 IpExtOutOctets 为 20+16+48=84。)

tcp 3-way handshake(TCP 三次握手)
---------------------------------
On server side, we run::
(在服务器端，我们运行：)

  nstatuser@nstat-b:~$ nc -lknv 0.0.0.0 9000
  Listening on [0.0.0.0] (family 0, port 9000)

On client side, we run::
(在客户端，我们运行：)

  nstatuser@nstat-a:~$ nc -nv 192.168.122.251 9000
  Connection to 192.168.122.251 9000 port [tcp/*] succeeded!

The server listened on tcp 9000 port, the client connected to it, they
completed the 3-way handshake.
(服务器监听 TCP 9000 端口，客户端连接到它，它们完成了三次握手。)

On server side, we can find below nstat output::
(在服务器端，我们可以看到如下 nstat 输出：)

  nstatuser@nstat-b:~$ nstat | grep -i tcp
  TcpPassiveOpens                 1                  0.0
  TcpInSegs                       2                  0.0
  TcpOutSegs                      1                  0.0
  TcpExtTCPPureAcks               1                  0.0

On client side, we can find below nstat output::
(在客户端，我们可以看到如下 nstat 输出：)

  nstatuser@nstat-a:~$ nstat | grep -i tcp
  TcpActiveOpens                  1                  0.0
  TcpInSegs                       1                  0.0
  TcpOutSegs                      2                  0.0

When the server received the first SYN, it replied a SYN+ACK, and came into
SYN-RCVD state, so TcpPassiveOpens increased 1. The server received
SYN, sent SYN+ACK, received ACK, so server sent 1 packet, received 2
packets, TcpInSegs increased 2, TcpOutSegs increased 1. The last ACK
of the 3-way handshake is a pure ACK without data, so
TcpExtTCPPureAcks increased 1.
(服务器收到第一个 SYN 后回复了一个 SYN+ACK，并进入 SYN-RCVD 状态，因此 TcpPassiveOpens 增加 1。服务器收到了 SYN、发送了 SYN+ACK、又收到了 ACK，所以服务器发送了 1 个数据包、接收了 2 个数据包，TcpInSegs 增加 2，TcpOutSegs 增加 1。三次握手的最后一个 ACK 是不含数据的纯 ACK，因此 TcpExtTCPPureAcks 增加 1。)

When the client sent SYN, the client came into the SYN-SENT state, so
TcpActiveOpens increased 1, the client sent SYN, received SYN+ACK, sent
ACK, so client sent 2 packets, received 1 packet, TcpInSegs increased
1, TcpOutSegs increased 2.
(客户端发送 SYN 后进入 SYN-SENT 状态，因此 TcpActiveOpens 增加 1。客户端发送了 SYN、收到了 SYN+ACK、又发送了 ACK，所以客户端发送了 2 个数据包、接收了 1 个数据包，TcpInSegs 增加 1，TcpOutSegs 增加 2。)

TCP normal traffic(TCP 正常流量)
--------------------------------
Run nc on server::
(在服务器上运行 nc：)

  nstatuser@nstat-b:~$ nc -lkv 0.0.0.0 9000
  Listening on [0.0.0.0] (family 0, port 9000)

Run nc on client::
(在客户端上运行 nc：)

  nstatuser@nstat-a:~$ nc -v nstat-b 9000
  Connection to nstat-b 9000 port [tcp/*] succeeded!

Input a string in the nc client ('hello' in our example)::
(在 nc 客户端输入一个字符串(我们的例子中是 'hello')：)

  nstatuser@nstat-a:~$ nc -v nstat-b 9000
  Connection to nstat-b 9000 port [tcp/*] succeeded!
  hello

The client side nstat output::
(客户端的 nstat 输出：)

  nstatuser@nstat-a:~$ nstat
  #kernel
  IpInReceives                    1                  0.0
  IpInDelivers                    1                  0.0
  IpOutRequests                   1                  0.0
  TcpInSegs                       1                  0.0
  TcpOutSegs                      1                  0.0
  TcpExtTCPPureAcks               1                  0.0
  TcpExtTCPOrigDataSent           1                  0.0
  IpExtInOctets                   52                 0.0
  IpExtOutOctets                  58                 0.0
  IpExtInNoECTPkts                1                  0.0

The server side nstat output::
(服务器端的 nstat 输出：)

  nstatuser@nstat-b:~$ nstat
  #kernel
  IpInReceives                    1                  0.0
  IpInDelivers                    1                  0.0
  IpOutRequests                   1                  0.0
  TcpInSegs                       1                  0.0
  TcpOutSegs                      1                  0.0
  IpExtInOctets                   58                 0.0
  IpExtOutOctets                  52                 0.0
  IpExtInNoECTPkts                1                  0.0

Input a string in nc client side again ('world' in our example)::
(在 nc 客户端再次输入一个字符串(我们的例子中是 'world')：)

  nstatuser@nstat-a:~$ nc -v nstat-b 9000
  Connection to nstat-b 9000 port [tcp/*] succeeded!
  hello
  world

Client side nstat output::
(客户端 nstat 输出：)

  nstatuser@nstat-a:~$ nstat
  #kernel
  IpInReceives                    1                  0.0
  IpInDelivers                    1                  0.0
  IpOutRequests                   1                  0.0
  TcpInSegs                       1                  0.0
  TcpOutSegs                      1                  0.0
  TcpExtTCPHPAcks                 1                  0.0
  TcpExtTCPOrigDataSent           1                  0.0
  IpExtInOctets                   52                 0.0
  IpExtOutOctets                  58                 0.0
  IpExtInNoECTPkts                1                  0.0


Server side nstat output::
(服务器端 nstat 输出：)

  nstatuser@nstat-b:~$ nstat
  #kernel
  IpInReceives                    1                  0.0
  IpInDelivers                    1                  0.0
  IpOutRequests                   1                  0.0
  TcpInSegs                       1                  0.0
  TcpOutSegs                      1                  0.0
  TcpExtTCPHPHits                 1                  0.0
  IpExtInOctets                   58                 0.0
  IpExtOutOctets                  52                 0.0
  IpExtInNoECTPkts                1                  0.0

Compare the first client-side nstat and the second client-side nstat,
we could find one difference: the first one had a 'TcpExtTCPPureAcks',
but the second one had a 'TcpExtTCPHPAcks'. The first server-side
nstat and the second server-side nstat had a difference too: the
second server-side nstat had a TcpExtTCPHPHits, but the first
server-side nstat didn't have it. The network traffic patterns were
exactly the same: the client sent a packet to the server, the server
replied an ACK. But kernel handled them in different ways. When the
TCP window scale option is not used, kernel will try to enable fast
path immediately when the connection comes into the established state,
but if the TCP window scale option is used, kernel will disable the
fast path at first, and try to enable it after kernel receives
packets. We could use the 'ss' command to verify whether the window
scale option is used. e.g. run below command on either server or
client::
(比较第一次客户端 nstat 与第二次客户端 nstat，可以发现一个差异：第一次有 'TcpExtTCPPureAcks'，而第二次有 'TcpExtTCPHPAcks'。第一次服务器端 nstat 与第二次服务器端 nstat 也有差异：第二次服务器端 nstat 有 TcpExtTCPHPHits，而第一次没有。两次的网络流量模式完全相同：客户端向服务器发送一个数据包，服务器回复一个 ACK。但内核处理它们的方式不同。当未使用 TCP 窗口缩放 (window scale) 选项时，连接一进入 established 状态内核就会立即尝试启用快速路径；但如果使用了 TCP 窗口缩放选项，内核会先禁用快速路径，并在收到数据包后再尝试启用它。我们可以用 'ss' 命令来验证是否使用了窗口缩放选项，例如在服务器或客户端上运行以下命令：)

  nstatuser@nstat-a:~$ ss -o state established -i '( dport = :9000 or sport = :9000 )
  Netid    Recv-Q     Send-Q            Local Address:Port             Peer Address:Port
  tcp      0          0               192.168.122.250:40654         192.168.122.251:9000
             ts sack cubic wscale:7,7 rto:204 rtt:0.98/0.49 mss:1448 pmtu:1500 rcvmss:536 advmss:1448 cwnd:10 bytes_acked:1 segs_out:2 segs_in:1 send 118.2Mbps lastsnd:46572 lastrcv:46572 lastack:46572 pacing_rate 236.4Mbps rcv_space:29200 rcv_ssthresh:29200 minrtt:0.98

The 'wscale:7,7' means both server and client set the window scale
option to 7. Now we could explain the nstat output in our test:
('wscale:7,7' 表示服务器和客户端都把窗口缩放选项设为了 7。现在我们可以解释测试中的 nstat 输出了：)

In the first nstat output of client side, the client sent a packet, server
reply an ACK, when kernel handled this ACK, the fast path was not
enabled, so the ACK was counted into 'TcpExtTCPPureAcks'.
(在客户端的第一次 nstat 输出中，客户端发送了一个数据包，服务器回复了一个 ACK。内核处理这个 ACK 时，快速路径尚未启用，因此该 ACK 被计入 'TcpExtTCPPureAcks'。)

In the second nstat output of client side, the client sent a packet again,
and received another ACK from the server, in this time, the fast path is
enabled, and the ACK was qualified for fast path, so it was handled by
the fast path, so this ACK was counted into TcpExtTCPHPAcks.
(在客户端的第二次 nstat 输出中，客户端又发送了一个数据包，并收到了服务器的另一个 ACK。这一次，快速路径已启用，且该 ACK 符合快速路径的条件，因此由快速路径处理，该 ACK 被计入 TcpExtTCPHPAcks。)

In the first nstat output of server side, fast path was not enabled,
so there was no 'TcpExtTCPHPHits'.
(在服务器端的第一次 nstat 输出中，快速路径尚未启用，因此没有 'TcpExtTCPHPHits'。)

In the second nstat output of server side, the fast path was enabled,
and the packet received from client qualified for fast path, so it
was counted into 'TcpExtTCPHPHits'.
(在服务器端的第二次 nstat 输出中，快速路径已启用，且从客户端收到的数据包符合快速路径的条件，因此它被计入 'TcpExtTCPHPHits'。)

TcpExtTCPAbortOnClose
---------------------
On the server side, we run below python script::
(在服务器端，我们运行以下 Python 脚本：)

  import socket
  import time

  port = 9000

  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.bind(('0.0.0.0', port))
  s.listen(1)
  sock, addr = s.accept()
  while True:
      time.sleep(9999999)

This python script listen on 9000 port, but doesn't read anything from
the connection.
(这个 Python 脚本监听 9000 端口，但不从连接中读取任何数据。)

On the client side, we send the string "hello" by nc::
(在客户端，我们用 nc 发送字符串 "hello"：)

  nstatuser@nstat-a:~$ echo "hello" | nc nstat-b 9000

Then, we come back to the server side, the server has received the "hello"
packet, and the TCP layer has acked this packet, but the application didn't
read it yet. We type Ctrl-C to terminate the server script. Then we
could find TcpExtTCPAbortOnClose increased 1 on the server side::
(然后我们回到服务器端：服务器已经收到了 "hello" 数据包，TCP 层已确认了该数据包，但应用程序还没有读取它。我们键入 Ctrl-C 终止服务器脚本，然后可以发现服务器端的 TcpExtTCPAbortOnClose 增加了 1：)

  nstatuser@nstat-b:~$ nstat | grep -i abort
  TcpExtTCPAbortOnClose           1                  0.0

If we run tcpdump on the server side, we could find the server sent a
RST after we type Ctrl-C.
(如果我们在服务器端运行 tcpdump，可以发现键入 Ctrl-C 之后服务器发送了一个 RST。)

TcpExtTCPAbortOnMemory and TcpExtTCPAbortOnTimeout(TcpExtTCPAbortOnMemory 和 TcpExtTCPAbortOnTimeout)
-----------------------------------------------------------------------------------------------------
Below is an example which let the orphan socket count be higher than
net.ipv4.tcp_max_orphans.
Change tcp_max_orphans to a smaller value on client::
(下面是一个让孤儿套接字数量高于 net.ipv4.tcp_max_orphans 的例子。在客户端把 tcp_max_orphans 改成一个较小的值：)

  sudo bash -c "echo 10 > /proc/sys/net/ipv4/tcp_max_orphans"

Client code (create 64 connection to server)::
(客户端代码(创建到服务器的 64 个连接)：)

  nstatuser@nstat-a:~$ cat client_orphan.py
  import socket
  import time

  server = 'nstat-b' # server address
  port = 9000

  count = 64

  connection_list = []

  for i in range(64):
      s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      s.connect((server, port))
      connection_list.append(s)
      print("connection_count: %d" % len(connection_list))

  while True:
      time.sleep(99999)

Server code (accept 64 connection from client)::
(服务器代码(接受来自客户端的 64 个连接)：)

  nstatuser@nstat-b:~$ cat server_orphan.py
  import socket
  import time

  port = 9000
  count = 64

  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.bind(('0.0.0.0', port))
  s.listen(count)
  connection_list = []
  while True:
      sock, addr = s.accept()
      connection_list.append((sock, addr))
      print("connection_count: %d" % len(connection_list))

Run the python scripts on server and client.
(在服务器和客户端上运行这些 Python 脚本。)

On server::
(在服务器上：)

  python3 server_orphan.py

On client::
(在客户端上：)

  python3 client_orphan.py

Run iptables on server::
(在服务器上运行 iptables：)

  sudo iptables -A INPUT -i ens3 -p tcp --destination-port 9000 -j DROP

Type Ctrl-C on client, stop client_orphan.py.
(在客户端键入 Ctrl-C，停止 client_orphan.py。)

Check TcpExtTCPAbortOnMemory on client::
(在客户端检查 TcpExtTCPAbortOnMemory：)

  nstatuser@nstat-a:~$ nstat | grep -i abort
  TcpExtTCPAbortOnMemory          54                 0.0

Check orphaned socket count on client::
(在客户端检查孤儿套接字数量：)

  nstatuser@nstat-a:~$ ss -s
  Total: 131 (kernel 0)
  TCP:   14 (estab 1, closed 0, orphaned 10, synrecv 0, timewait 0/0), ports 0

  Transport Total     IP        IPv6
  *         0         -         -
  RAW       1         0         1
  UDP       1         1         0
  TCP       14        13        1
  INET      16        14        2
  FRAG      0         0         0

The explanation of the test: after run server_orphan.py and
client_orphan.py, we set up 64 connections between server and
client. Run the iptables command, the server will drop all packets from
the client, type Ctrl-C on client_orphan.py, the system of the client
would try to close these connections, and before they are closed
gracefully, these connections became orphan sockets. As the iptables
of the server blocked packets from the client, the server won't receive fin
from the client, so all connection on clients would be stuck on FIN_WAIT_1
stage, so they will keep as orphan sockets until timeout. We have echo
10 to /proc/sys/net/ipv4/tcp_max_orphans, so the client system would
only keep 10 orphan sockets, for all other orphan sockets, the client
system sent RST for them and delete them. We have 64 connections, so
the 'ss -s' command shows the system has 10 orphan sockets, and the
value of TcpExtTCPAbortOnMemory was 54.
(对这个测试的解释：运行 server_orphan.py 和 client_orphan.py 之后，我们在服务器和客户端之间建立了 64 个连接。运行 iptables 命令后，服务器会丢弃来自客户端的所有数据包；在 client_orphan.py 上键入 Ctrl-C，客户端系统会尝试关闭这些连接，在它们被优雅地关闭之前，这些连接成为了孤儿套接字。由于服务器的 iptables 阻断了来自客户端的数据包，服务器收不到客户端的 fin，因此客户端上的所有连接都会卡在 FIN_WAIT_1 阶段，它们将一直保持为孤儿套接字直到超时。我们已经向 /proc/sys/net/ipv4/tcp_max_orphans 写入(echo)了 10，因此客户端系统只会保留 10 个孤儿套接字，对其余的孤儿套接字，客户端系统为它们发送 RST 并删除。我们有 64 个连接，所以 'ss -s' 命令显示系统有 10 个孤儿套接字，而 TcpExtTCPAbortOnMemory 的值是 54。)

An additional explanation about orphan socket count: You could find the
exactly orphan socket count by the 'ss -s' command, but when kernel
decide whither increases TcpExtTCPAbortOnMemory and sends RST, kernel
doesn't always check the exactly orphan socket count. For increasing
performance, kernel checks an approximate count firstly, if the
approximate count is more than tcp_max_orphans, kernel checks the
exact count again. So if the approximate count is less than
tcp_max_orphans, but exactly count is more than tcp_max_orphans, you
would find TcpExtTCPAbortOnMemory is not increased at all. If
tcp_max_orphans is large enough, it won't occur, but if you decrease
tcp_max_orphans to a small value like our test, you might find this
issue. So in our test, the client set up 64 connections although the
tcp_max_orphans is 10. If the client only set up 11 connections, we
can't find the change of TcpExtTCPAbortOnMemory.
(关于孤儿套接字数量的补充说明：你可以用 'ss -s' 命令找到孤儿套接字的准确数量，但当内核决定是否增加 TcpExtTCPAbortOnMemory 并发送 RST 时，并不总是检查准确的孤儿套接字数量。为了提高性能，内核先检查一个近似计数，如果近似计数大于 tcp_max_orphans，内核才会再次检查准确的计数。因此，如果近似计数小于 tcp_max_orphans 而准确计数大于 tcp_max_orphans，你会发现 TcpExtTCPAbortOnMemory 完全没有增加。如果 tcp_max_orphans 足够大，这不会发生；但如果你像本测试那样把 tcp_max_orphans 减小到一个很小的值，就可能遇到这个问题。所以在本测试中，尽管 tcp_max_orphans 是 10，客户端仍然建立了 64 个连接。如果客户端只建立 11 个连接，我们就看不到 TcpExtTCPAbortOnMemory 的变化。)

Continue the previous test, we wait for several minutes. Because of the
iptables on the server blocked the traffic, the server wouldn't receive
fin, and all the client's orphan sockets would timeout on the
FIN_WAIT_1 state finally. So we wait for a few minutes, we could find
10 timeout on the client::
(继续前面的测试，我们等待几分钟。由于服务器上的 iptables 阻断了流量，服务器收不到 fin，客户端所有的孤儿套接字最终都会在 FIN_WAIT_1 状态下超时。因此等待几分钟后，我们可以在客户端看到 10 次超时：)

  nstatuser@nstat-a:~$ nstat | grep -i abort
  TcpExtTCPAbortOnTimeout         10                 0.0

TcpExtTCPAbortOnLinger
----------------------
The server side code::
(服务器端代码：)

  nstatuser@nstat-b:~$ cat server_linger.py
  import socket
  import time

  port = 9000

  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.bind(('0.0.0.0', port))
  s.listen(1)
  sock, addr = s.accept()
  while True:
      time.sleep(9999999)

The client side code::
(客户端代码：)

  nstatuser@nstat-a:~$ cat client_linger.py
  import socket
  import struct

  server = 'nstat-b' # server address
  port = 9000

  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack('ii', 1, 10))
  s.setsockopt(socket.SOL_TCP, socket.TCP_LINGER2, struct.pack('i', -1))
  s.connect((server, port))
  s.close()

Run server_linger.py on server::
(在服务器上运行 server_linger.py：)

  nstatuser@nstat-b:~$ python3 server_linger.py

Run client_linger.py on client::
(在客户端上运行 client_linger.py：)

  nstatuser@nstat-a:~$ python3 client_linger.py

After run client_linger.py, check the output of nstat::
(运行 client_linger.py 之后，检查 nstat 的输出：)

  nstatuser@nstat-a:~$ nstat | grep -i abort
  TcpExtTCPAbortOnLinger          1                  0.0

TcpExtTCPRcvCoalesce
--------------------
On the server, we run a program which listen on TCP port 9000, but
doesn't read any data::
(在服务器上，我们运行一个监听 TCP 端口 9000 但不读取任何数据的程序：)

  import socket
  import time
  port = 9000
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.bind(('0.0.0.0', port))
  s.listen(1)
  sock, addr = s.accept()
  while True:
      time.sleep(9999999)

Save the above code as server_coalesce.py, and run::
(把上面的代码保存为 server_coalesce.py，然后运行：)

  python3 server_coalesce.py

On the client, save below code as client_coalesce.py::
(在客户端，把下面的代码保存为 client_coalesce.py：)

  import socket
  server = 'nstat-b'
  port = 9000
  s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  s.connect((server, port))

Run::
(运行：)

  nstatuser@nstat-a:~$ python3 -i client_coalesce.py

We use '-i' to come into the interactive mode, then a packet::
(我们用 '-i' 进入交互模式，然后发送一个数据包：)

  >>> s.send(b'foo')
  3

Send a packet again::
(再发送一个数据包：)

  >>> s.send(b'bar')
  3

On the server, run nstat::
(在服务器上运行 nstat：)

  ubuntu@nstat-b:~$ nstat
  #kernel
  IpInReceives                    2                  0.0
  IpInDelivers                    2                  0.0
  IpOutRequests                   2                  0.0
  TcpInSegs                       2                  0.0
  TcpOutSegs                      2                  0.0
  TcpExtTCPRcvCoalesce            1                  0.0
  IpExtInOctets                   110                0.0
  IpExtOutOctets                  104                0.0
  IpExtInNoECTPkts                2                  0.0

The client sent two packets, server didn't read any data. When
the second packet arrived at server, the first packet was still in
the receiving queue. So the TCP layer merged the two packets, and we
could find the TcpExtTCPRcvCoalesce increased 1.
(客户端发送了两个数据包，服务器没有读取任何数据。当第二个数据包到达服务器时，第一个数据包还在接收队列中。因此 TCP 层合并了这两个数据包，我们可以看到 TcpExtTCPRcvCoalesce 增加了 1。)

TcpExtListenOverflows and TcpExtListenDrops(TcpExtListenOverflows 和 TcpExtListenDrops)
---------------------------------------------------------------------------------------
On server, run the nc command, listen on port 9000::
(在服务器上运行 nc 命令，监听 9000 端口：)

  nstatuser@nstat-b:~$ nc -lkv 0.0.0.0 9000
  Listening on [0.0.0.0] (family 0, port 9000)

On client, run 3 nc commands in different terminals::
(在客户端上，在不同的终端中运行 3 个 nc 命令：)

  nstatuser@nstat-a:~$ nc -v nstat-b 9000
  Connection to nstat-b 9000 port [tcp/*] succeeded!

The nc command only accepts 1 connection, and the accept queue length
is 1. On current linux implementation, set queue length to n means the
actual queue length is n+1. Now we create 3 connections, 1 is accepted
by nc, 2 in accepted queue, so the accept queue is full.
(nc 命令只接受 1 个连接，accept 队列长度为 1。在当前的 Linux 实现中，把队列长度设为 n 意味着实际队列长度是 n+1。现在我们创建 3 个连接：1 个被 nc 接受，2 个在 accept 队列中，因此 accept 队列已满。)

Before running the 4th nc, we clean the nstat history on the server::
(在运行第 4 个 nc 之前，我们先清除服务器上的 nstat 历史：)

  nstatuser@nstat-b:~$ nstat -n

Run the 4th nc on the client::
(在客户端上运行第 4 个 nc：)

  nstatuser@nstat-a:~$ nc -v nstat-b 9000

If the nc server is running on kernel 4.10 or higher version, you
won't see the "Connection to ... succeeded!" string, because kernel
will drop the SYN if the accept queue is full. If the nc client is running
on an old kernel, you would see that the connection is succeeded,
because kernel would complete the 3 way handshake and keep the socket
on half open queue. I did the test on kernel 4.15. Below is the nstat
on the server::
(如果 nc 服务器运行在 4.10 或更高版本的内核上，你不会看到 "Connection to ... succeeded!" 字样，因为当 accept 队列已满时内核会丢弃 SYN。如果 nc 客户端运行在旧内核上，你会看到连接成功，因为内核会完成三次握手并把套接字保存在半连接队列中。我在 4.15 内核上做了这个测试。下面是服务器上的 nstat 输出：)

  nstatuser@nstat-b:~$ nstat
  #kernel
  IpInReceives                    4                  0.0
  IpInDelivers                    4                  0.0
  TcpInSegs                       4                  0.0
  TcpExtListenOverflows           4                  0.0
  TcpExtListenDrops               4                  0.0
  IpExtInOctets                   240                0.0
  IpExtInNoECTPkts                4                  0.0

Both TcpExtListenOverflows and TcpExtListenDrops were 4. If the time
between the 4th nc and the nstat was longer, the value of
TcpExtListenOverflows and TcpExtListenDrops would be larger, because
the SYN of the 4th nc was dropped, the client was retrying.
(TcpExtListenOverflows 和 TcpExtListenDrops 都是 4。如果第 4 个 nc 与 nstat 之间的时间间隔更长，TcpExtListenOverflows 和 TcpExtListenDrops 的值会更大，因为第 4 个 nc 的 SYN 被丢弃后客户端在不断重试。)

IpInAddrErrors, IpExtInNoRoutes and IpOutNoRoutes(IpInAddrErrors、IpExtInNoRoutes 和 IpOutNoRoutes)
---------------------------------------------------------------------------------------------------
server A IP address: 192.168.122.250
server B IP address: 192.168.122.251
Prepare on server A, add a route to server B::
(服务器 A 的 IP 地址：192.168.122.250；服务器 B 的 IP 地址：192.168.122.251。在服务器 A 上做准备：添加一条到服务器 B 的路由：)

  $ sudo ip route add 8.8.8.8/32 via 192.168.122.251

Prepare on server B, disable send_redirects for all interfaces::
(在服务器 B 上做准备：对所有接口禁用 send_redirects：)

  $ sudo sysctl -w net.ipv4.conf.all.send_redirects=0
  $ sudo sysctl -w net.ipv4.conf.ens3.send_redirects=0
  $ sudo sysctl -w net.ipv4.conf.lo.send_redirects=0
  $ sudo sysctl -w net.ipv4.conf.default.send_redirects=0

We want to let sever A send a packet to 8.8.8.8, and route the packet
to server B. When server B receives such packet, it might send a ICMP
Redirect message to server A, set send_redirects to 0 will disable
this behavior.
(我们想让服务器 A 向 8.8.8.8 发送数据包，并把数据包路由到服务器 B。当服务器 B 收到这样的数据包时，它可能会向服务器 A 发送 ICMP Redirect 报文，把 send_redirects 设为 0 可以禁用这一行为。)

First, generate InAddrErrors. On server B, we disable IP forwarding::
(第一步，制造 InAddrErrors。在服务器 B 上，我们禁用 IP 转发：)

  $ sudo sysctl -w net.ipv4.conf.all.forwarding=0

On server A, we send packets to 8.8.8.8::
(在服务器 A 上，我们向 8.8.8.8 发送数据包：)

  $ nc -v 8.8.8.8 53

On server B, we check the output of nstat::
(在服务器 B 上，我们检查 nstat 的输出：)

  $ nstat
  #kernel
  IpInReceives                    3                  0.0
  IpInAddrErrors                  3                  0.0
  IpExtInOctets                   180                0.0
  IpExtInNoECTPkts                3                  0.0

As we have let server A route 8.8.8.8 to server B, and we disabled IP
forwarding on server B, Server A sent packets to server B, then server B
dropped packets and increased IpInAddrErrors. As the nc command would
re-send the SYN packet if it didn't receive a SYN+ACK, we could find
multiple IpInAddrErrors.
(由于我们让服务器 A 把发往 8.8.8.8 的数据包路由到服务器 B，并且在服务器 B 上禁用了 IP 转发，服务器 A 把数据包发给了服务器 B，服务器 B 丢弃了这些数据包并增加 IpInAddrErrors。由于 nc 命令在收不到 SYN+ACK 时会重发 SYN 数据包，我们可以看到多个 IpInAddrErrors。)

Second, generate IpExtInNoRoutes. On server B, we enable IP
forwarding::
(第二步，制造 IpExtInNoRoutes。在服务器 B 上，我们启用 IP 转发：)

  $ sudo sysctl -w net.ipv4.conf.all.forwarding=1

Check the route table of server B and remove the default route::
(查看服务器 B 的路由表并删除默认路由：)

  $ ip route show
  default via 192.168.122.1 dev ens3 proto static
  192.168.122.0/24 dev ens3 proto kernel scope link src 192.168.122.251
  $ sudo ip route delete default via 192.168.122.1 dev ens3 proto static

On server A, we contact 8.8.8.8 again::
(在服务器 A 上，我们再次联系 8.8.8.8：)

  $ nc -v 8.8.8.8 53
  nc: connect to 8.8.8.8 port 53 (tcp) failed: Network is unreachable

On server B, run nstat::
(在服务器 B 上运行 nstat：)

  $ nstat
  #kernel
  IpInReceives                    1                  0.0
  IpOutRequests                   1                  0.0
  IcmpOutMsgs                     1                  0.0
  IcmpOutDestUnreachs             1                  0.0
  IcmpMsgOutType3                 1                  0.0
  IpExtInNoRoutes                 1                  0.0
  IpExtInOctets                   60                 0.0
  IpExtOutOctets                  88                 0.0
  IpExtInNoECTPkts                1                  0.0

We enabled IP forwarding on server B, when server B received a packet
which destination IP address is 8.8.8.8, server B will try to forward
this packet. We have deleted the default route, there was no route for
8.8.8.8, so server B increase IpExtInNoRoutes and sent the "ICMP
Destination Unreachable" message to server A.
(我们在服务器 B 上启用了 IP 转发，当服务器 B 收到一个目的 IP 地址为 8.8.8.8 的数据包时，服务器 B 会尝试转发该数据包。我们已经删除了默认路由，8.8.8.8 没有路由，因此服务器 B 增加 IpExtInNoRoutes 并向服务器 A 发送了 "ICMP Destination Unreachable"(ICMP 目标不可达)报文。)

Third, generate IpOutNoRoutes. Run ping command on server B::
(第三步，制造 IpOutNoRoutes。在服务器 B 上运行 ping 命令：)

  $ ping -c 1 8.8.8.8
  connect: Network is unreachable

Run nstat on server B::
(在服务器 B 上运行 nstat：)

  $ nstat
  #kernel
  IpOutNoRoutes                   1                  0.0

We have deleted the default route on server B. Server B couldn't find
a route for the 8.8.8.8 IP address, so server B increased
IpOutNoRoutes.
(我们已经删除了服务器 B 上的默认路由。服务器 B 找不到通往 8.8.8.8 这个 IP 地址的路由，因此服务器 B 增加 IpOutNoRoutes。)

TcpExtTCPACKSkippedSynRecv
--------------------------
In this test, we send 3 same SYN packets from client to server. The
first SYN will let server create a socket, set it to Syn-Recv status,
and reply a SYN/ACK. The second SYN will let server reply the SYN/ACK
again, and record the reply time (the duplicate ACK reply time). The
third SYN will let server check the previous duplicate ACK reply time,
and decide to skip the duplicate ACK, then increase the
TcpExtTCPACKSkippedSynRecv counter.
(在这个测试中，我们从客户端向服务器发送 3 个相同的 SYN 数据包。第一个 SYN 会让服务器创建一个套接字，把它置为 Syn-Recv 状态，并回复一个 SYN/ACK。第二个 SYN 会让服务器再次回复 SYN/ACK，并记录回复时间(即重复 ACK 的回复时间)。第三个 SYN 会让服务器检查上一次重复 ACK 的回复时间，并决定跳过这个重复 ACK，然后增加 TcpExtTCPACKSkippedSynRecv 计数器。)

Run tcpdump to capture a SYN packet::
(运行 tcpdump 抓取一个 SYN 数据包：)

  nstatuser@nstat-a:~$ sudo tcpdump -c 1 -w /tmp/syn.pcap port 9000
  tcpdump: listening on ens3, link-type EN10MB (Ethernet), capture size 262144 bytes

Open another terminal, run nc command::
(打开另一个终端，运行 nc 命令：)

  nstatuser@nstat-a:~$ nc nstat-b 9000

As the nstat-b didn't listen on port 9000, it should reply a RST, and
the nc command exited immediately. It was enough for the tcpdump
command to capture a SYN packet. A linux server might use hardware
offload for the TCP checksum, so the checksum in the /tmp/syn.pcap
might be not correct. We call tcprewrite to fix it::
(由于 nstat-b 没有监听 9000 端口，它应该会回复一个 RST，nc 命令随即退出。这对 tcpdump 命令抓取一个 SYN 数据包来说已经足够。Linux 服务器可能对 TCP 校验和使用硬件卸载 (offload)，因此 /tmp/syn.pcap 中的校验和可能不正确。我们调用 tcprewrite 来修复它：)

  nstatuser@nstat-a:~$ tcprewrite --infile=/tmp/syn.pcap --outfile=/tmp/syn_fixcsum.pcap --fixcsum

On nstat-b, we run nc to listen on port 9000::
(在 nstat-b 上，我们运行 nc 监听 9000 端口：)

  nstatuser@nstat-b:~$ nc -lkv 9000
  Listening on [0.0.0.0] (family 0, port 9000)

On nstat-a, we blocked the packet from port 9000, or nstat-a would send
RST to nstat-b::
(在 nstat-a 上，我们阻断来自 9000 端口的数据包，否则 nstat-a 会向 nstat-b 发送 RST：)

  nstatuser@nstat-a:~$ sudo iptables -A INPUT -p tcp --sport 9000 -j DROP

Send 3 SYN repeatedly to nstat-b::
(向 nstat-b 重复发送 3 次 SYN：)

  nstatuser@nstat-a:~$ for i in {1..3}; do sudo tcpreplay -i ens3 /tmp/syn_fixcsum.pcap; done

Check snmp counter on nstat-b::
(在 nstat-b 上检查 snmp 计数器：)

  nstatuser@nstat-b:~$ nstat | grep -i skip
  TcpExtTCPACKSkippedSynRecv      1                  0.0

As we expected, TcpExtTCPACKSkippedSynRecv is 1.
(正如我们所预期的，TcpExtTCPACKSkippedSynRecv 是 1。)

TcpExtTCPACKSkippedPAWS
-----------------------
To trigger PAWS, we could send an old SYN.
(为了触发 PAWS，我们可以发送一个旧的 SYN。)

On nstat-b, let nc listen on port 9000::
(在 nstat-b 上，让 nc 监听 9000 端口：)

  nstatuser@nstat-b:~$ nc -lkv 9000
  Listening on [0.0.0.0] (family 0, port 9000)

On nstat-a, run tcpdump to capture a SYN::
(在 nstat-a 上，运行 tcpdump 抓取一个 SYN：)

  nstatuser@nstat-a:~$ sudo tcpdump -w /tmp/paws_pre.pcap -c 1 port 9000
  tcpdump: listening on ens3, link-type EN10MB (Ethernet), capture size 262144 bytes

On nstat-a, run nc as a client to connect nstat-b::
(在 nstat-a 上，以客户端身份运行 nc 连接 nstat-b：)

  nstatuser@nstat-a:~$ nc -v nstat-b 9000
  Connection to nstat-b 9000 port [tcp/*] succeeded!

Now the tcpdump has captured the SYN and exit. We should fix the
checksum::
(现在 tcpdump 已经抓到了 SYN 并退出。我们需要修复校验和：)

  nstatuser@nstat-a:~$ tcprewrite --infile /tmp/paws_pre.pcap --outfile /tmp/paws.pcap --fixcsum

Send the SYN packet twice::
(把这个 SYN 数据包发送两次：)

  nstatuser@nstat-a:~$ for i in {1..2}; do sudo tcpreplay -i ens3 /tmp/paws.pcap; done

On nstat-b, check the snmp counter::
(在 nstat-b 上，检查 snmp 计数器：)

  nstatuser@nstat-b:~$ nstat | grep -i skip
  TcpExtTCPACKSkippedPAWS         1                  0.0

We sent two SYN via tcpreplay, both of them would let PAWS check
failed, the nstat-b replied an ACK for the first SYN, skipped the ACK
for the second SYN, and updated TcpExtTCPACKSkippedPAWS.
(我们通过 tcpreplay 发送了两个 SYN，它们都会导致 PAWS 检查失败。nstat-b 为第一个 SYN 回复了一个 ACK，跳过了第二个 SYN 的 ACK，并更新了 TcpExtTCPACKSkippedPAWS。)

TcpExtTCPACKSkippedSeq
----------------------
To trigger TcpExtTCPACKSkippedSeq, we send packets which have valid
timestamp (to pass PAWS check) but the sequence number is out of
window. The linux TCP stack would avoid to skip if the packet has
data, so we need a pure ACK packet. To generate such a packet, we
could create two sockets: one on port 9000, another on port 9001. Then
we capture an ACK on port 9001, change the source/destination port
numbers to match the port 9000 socket. Then we could trigger
TcpExtTCPACKSkippedSeq via this packet.
(为了触发 TcpExtTCPACKSkippedSeq，我们发送带有有效时间戳(以便通过 PAWS 检查)但序列号超出窗口的数据包。如果数据包带有数据，Linux TCP 协议栈会避免跳过 ACK，因此我们需要一个纯 ACK 数据包。为了生成这样的数据包，我们可以创建两个套接字：一个在 9000 端口，另一个在 9001 端口。然后我们在 9001 端口上抓取一个 ACK，把源/目的端口号改成与 9000 端口的套接字相匹配。这样我们就可以用这个数据包触发 TcpExtTCPACKSkippedSeq。)

On nstat-b, open two terminals, run two nc commands to listen on both
port 9000 and port 9001::
(在 nstat-b 上，打开两个终端，运行两个 nc 命令，分别监听 9000 端口和 9001 端口：)

  nstatuser@nstat-b:~$ nc -lkv 9000
  Listening on [0.0.0.0] (family 0, port 9000)

  nstatuser@nstat-b:~$ nc -lkv 9001
  Listening on [0.0.0.0] (family 0, port 9001)

On nstat-a, run two nc clients::
(在 nstat-a 上，运行两个 nc 客户端：)

  nstatuser@nstat-a:~$ nc -v nstat-b 9000
  Connection to nstat-b 9000 port [tcp/*] succeeded!

  nstatuser@nstat-a:~$ nc -v nstat-b 9001
  Connection to nstat-b 9001 port [tcp/*] succeeded!

On nstat-a, run tcpdump to capture an ACK::
(在 nstat-a 上，运行 tcpdump 抓取一个 ACK：)

  nstatuser@nstat-a:~$ sudo tcpdump -w /tmp/seq_pre.pcap -c 1 dst port 9001
  tcpdump: listening on ens3, link-type EN10MB (Ethernet), capture size 262144 bytes

On nstat-b, send a packet via the port 9001 socket. E.g. we sent a
string 'foo' in our example::
(在 nstat-b 上，通过 9001 端口的套接字发送一个数据包。例如在我们的例子中发送了字符串 'foo'：)

  nstatuser@nstat-b:~$ nc -lkv 9001
  Listening on [0.0.0.0] (family 0, port 9001)
  Connection from nstat-a 42132 received!
  foo

On nstat-a, the tcpdump should have captured the ACK. We should check
the source port numbers of the two nc clients::
(在 nstat-a 上，tcpdump 应该已经抓到了 ACK。我们需要检查两个 nc 客户端的源端口号：)

  nstatuser@nstat-a:~$ ss -ta '( dport = :9000 || dport = :9001 )' | tee
  State  Recv-Q   Send-Q         Local Address:Port           Peer Address:Port
  ESTAB  0        0            192.168.122.250:50208       192.168.122.251:9000
  ESTAB  0        0            192.168.122.250:42132       192.168.122.251:9001

Run tcprewrite, change port 9001 to port 9000, change port 42132 to
port 50208::
(运行 tcprewrite，把端口 9001 改成 9000，把端口 42132 改成 50208：)

  nstatuser@nstat-a:~$ tcprewrite --infile /tmp/seq_pre.pcap --outfile /tmp/seq.pcap -r 9001:9000 -r 42132:50208 --fixcsum

Now the /tmp/seq.pcap is the packet we need. Send it to nstat-b::
(现在 /tmp/seq.pcap 就是我们需要的数据包。把它发送给 nstat-b：)

  nstatuser@nstat-a:~$ for i in {1..2}; do sudo tcpreplay -i ens3 /tmp/seq.pcap; done

Check TcpExtTCPACKSkippedSeq on nstat-b::
(在 nstat-b 上检查 TcpExtTCPACKSkippedSeq：)

  nstatuser@nstat-b:~$ nstat | grep -i skip
  TcpExtTCPACKSkippedSeq          1                  0.0
