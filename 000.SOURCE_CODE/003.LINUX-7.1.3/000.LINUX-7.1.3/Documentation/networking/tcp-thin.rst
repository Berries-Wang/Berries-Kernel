.. SPDX-License-Identifier: GPL-2.0

====================
Thin-streams and TCP
====================

(细流 (Thin-streams) 与 TCP)

A wide range of Internet-based services that use reliable transport
protocols display what we call thin-stream properties. This means
that the application sends data with such a low rate that the
retransmission mechanisms of the transport protocol are not fully
effective. In time-dependent scenarios (like online games, control
systems, stock trading etc.) where the user experience depends
on the data delivery latency, packet loss can be devastating for
the service quality. Extreme latencies are caused by TCP's
dependency on the arrival of new data from the application to trigger
retransmissions effectively through fast retransmit instead of
waiting for long timeouts.
(大量使用可靠传输协议的基于互联网的服务都表现出我们所说的细流 (thin-stream) 特性。这意味着应用发送数据的速率非常低，以致传输协议的重传机制不能充分发挥作用。在用户体验取决于数据传输延迟的时间敏感场景(如在线游戏、控制系统、股票交易等)中，丢包可能对服务质量造成毁灭性影响。极端延迟是由 TCP 依赖应用新数据的到来才能通过快速重传有效触发重传、而不是等待长时间超时这一特性造成的。)

After analysing a large number of time-dependent interactive
applications, we have seen that they often produce thin streams
and also stay with this traffic pattern throughout its entire
lifespan. The combination of time-dependency and the fact that the
streams provoke high latencies when using TCP is unfortunate.
(在分析了大量时间敏感的交互式应用之后，我们发现它们经常产生细流，并且在整个生命周期内都保持这种流量模式。时间敏感性与这些流在使用 TCP 时引发高延迟这一事实的结合令人遗憾。)

In order to reduce application-layer latency when packets are lost,
a set of mechanisms has been made, which address these latency issues
for thin streams. In short, if the kernel detects a thin stream,
the retransmission mechanisms are modified in the following manner:
(为了在丢包时降低应用层延迟，我们设计了一套机制，用于解决细流的这些延迟问题。简而言之，如果内核检测到细流，将按以下方式修改重传机制：)

1) If the stream is thin, fast retransmit on the first dupACK.
   (如果流是细流，则在收到第一个重复 ACK (dupACK) 时进行快速重传。)
2) If the stream is thin, do not apply exponential backoff.
   (如果流是细流，则不应用指数退避。)

These enhancements are applied only if the stream is detected as
thin. This is accomplished by defining a threshold for the number
of packets in flight. If there are less than 4 packets in flight,
fast retransmissions can not be triggered, and the stream is prone
to experience high retransmission latencies.
(只有在流被检测为细流时才应用这些增强。这是通过为在途数据包数量定义一个阈值来实现的。当在途数据包少于 4 个时，无法触发快速重传，该流容易出现高重传延迟。)

Since these mechanisms are targeted at time-dependent applications,
they must be specifically activated by the application using the
TCP_THIN_LINEAR_TIMEOUTS and TCP_THIN_DUPACK IOCTLS or the
tcp_thin_linear_timeouts and tcp_thin_dupack sysctls. Both
modifications are turned off by default.
(由于这些机制面向时间敏感的应用，应用必须使用 TCP_THIN_LINEAR_TIMEOUTS 和 TCP_THIN_DUPACK 这两个 IOCTL，或使用 tcp_thin_linear_timeouts 和 tcp_thin_dupack 这两个 sysctl 来显式激活它们。这两项修改默认都是关闭的。)

References
==========
(参考文献)

More information on the modifications, as well as a wide range of
experimental data can be found here:
(关于这些修改的更多信息以及大量实验数据，可在这里找到：)

"Improving latency for interactive, thin-stream applications over
reliable transport"
http://simula.no/research/nd/publications/Simula.nd.477/simula_pdf_file
("Improving latency for interactive, thin-stream applications over reliable transport"，即"通过可靠传输改善交互式细流应用的延迟")
