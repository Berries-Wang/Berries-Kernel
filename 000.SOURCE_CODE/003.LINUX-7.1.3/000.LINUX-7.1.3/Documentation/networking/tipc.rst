.. SPDX-License-Identifier: GPL-2.0

=================
Linux Kernel TIPC
=================

(Linux 内核 TIPC)

Introduction
(简介)
============

TIPC (Transparent Inter Process Communication) is a protocol that is specially
designed for intra-cluster communication. It can be configured to transmit
messages either on UDP or directly across Ethernet. Message delivery is
sequence guaranteed, loss free and flow controlled. Latency times are shorter
than with any other known protocol, while maximal throughput is comparable to
that of TCP.
(TIPC（透明进程间通信）是一种专为集群内部通信设计的协议。它可以配置为通过 UDP 或直接通过以太网传输消息。消息
的投递是有序、无损且受流量控制的。其时延比任何其他已知协议都短，而最大吞吐量与 TCP 相当。)

TIPC Features
(TIPC 特性)
-------------

- Cluster wide IPC service
  (集群范围的 IPC 服务)

  Have you ever wished you had the convenience of Unix Domain Sockets even when
  transmitting data between cluster nodes? Where you yourself determine the
  addresses you want to bind to and use? Where you don't have to perform DNS
  lookups and worry about IP addresses? Where you don't have to start timers
  to monitor the continuous existence of peer sockets? And yet without the
  downsides of that socket type, such as the risk of lingering inodes?
  (你是否曾希望即使在集群节点之间传输数据时，也能拥有 Unix 域套接字的便利？由你自己决定要绑定和使用的地址？
  无需执行 DNS 查询、不必操心 IP 地址？无需启动定时器来监视对端套接字的持续存在？而且还不会有那种套接字类型
  的缺点，比如 inode 残留的风险？)

  Welcome to the Transparent Inter Process Communication service, TIPC in short,
  which gives you all of this, and a lot more.
  (欢迎使用透明进程间通信服务——简称 TIPC——它为你提供所有这些，甚至更多。)

- Service Addressing
  (服务寻址)

  A fundamental concept in TIPC is that of Service Addressing which makes it
  possible for a programmer to chose his own address, bind it to a server
  socket and let client programs use only that address for sending messages.
  (TIPC 的一个基本概念是服务寻址 (Service Addressing)，它让程序员可以选择自己的地址，将其绑定到服务器套接
  字上，并让客户端程序仅使用该地址来发送消息。)

- Service Tracking
  (服务跟踪)

  A client wanting to wait for the availability of a server, uses the Service
  Tracking mechanism to subscribe for binding and unbinding/close events for
  sockets with the associated service address.
  (想要等待服务器可用的客户端，可以使用服务跟踪机制，订阅具有相关服务地址的套接字的绑定与解绑/关闭事件。)

  The service tracking mechanism can also be used for Cluster Topology Tracking,
  i.e., subscribing for availability/non-availability of cluster nodes.
  (服务跟踪机制还可以用于集群拓扑跟踪，即订阅集群节点的可用/不可用事件。)

  Likewise, the service tracking mechanism can be used for Cluster Connectivity
  Tracking, i.e., subscribing for up/down events for individual links between
  cluster nodes.
  (同样，服务跟踪机制还可以用于集群连接性跟踪，即订阅集群节点之间各条链路的通/断事件。)

- Transmission Modes
  (传输模式)

  Using a service address, a client can send datagram messages to a server socket.
  (通过服务地址，客户端可以向服务器套接字发送数据报消息。)

  Using the same address type, it can establish a connection towards an accepting
  server socket.
  (使用同样的地址类型，它也可以与一个接受连接的服务器套接字建立连接。)

  It can also use a service address to create and join a Communication Group,
  which is the TIPC manifestation of a brokerless message bus.
  (它还可以使用服务地址来创建并加入一个通信组 (Communication Group)，这是 TIPC 对无代理消息总线的具体体现。)

  Multicast with very good performance and scalability is available both in
  datagram mode and in communication group mode.
  (无论是数据报模式还是通信组模式，都提供了性能和可扩展性俱佳的组播。)

- Inter Node Links
  (节点间链路)

  Communication between any two nodes in a cluster is maintained by one or two
  Inter Node Links, which both guarantee data traffic integrity and monitor
  the peer node's availability.
  (集群中任意两个节点之间的通信由一条或两条节点间链路 (Inter Node Link) 维护，它们既保证数据流量的完整性，又
  监视对端节点的可用性。)

- Cluster Scalability
  (集群可扩展性)

  By applying the Overlapping Ring Monitoring algorithm on the inter node links
  it is possible to scale TIPC clusters up to 1000 nodes with a maintained
  neighbor failure discovery time of 1-2 seconds. For smaller clusters this
  time can be made much shorter.
  (通过在节点间链路上应用重叠环监控 (Overlapping Ring Monitoring) 算法，TIPC 集群可以扩展到 1000 个节点，同
  时保持 1-2 秒的邻居故障发现时间。对于较小的集群，这一时间还可以短得多。)

- Neighbor Discovery
  (邻居发现)

  Neighbor Node Discovery in the cluster is done by Ethernet broadcast or UDP
  multicast, when any of those services are available. If not, configured peer
  IP addresses can be used.
  (当这些服务可用时，集群内的邻居节点发现通过以太网广播或 UDP 组播完成。如果不可用，则可以使用预先配置的对端
  IP 地址。)

- Configuration
  (配置)

  When running TIPC in single node mode no configuration whatsoever is needed.
  When running in cluster mode TIPC must as a minimum be given a node address
  (before Linux 4.17) and told which interface to attach to. The "tipc"
  configuration tool makes is possible to add and maintain many more
  configuration parameters.
  (以单节点模式运行 TIPC 时，完全无需任何配置。以集群模式运行时，TIPC 至少必须被赋予一个节点地址（Linux 4.17
  之前），并被告知要附加到哪个接口。"tipc" 配置工具使得添加和维护更多的配置参数成为可能。)

- Performance
  (性能)

  TIPC message transfer latency times are better than in any other known protocol.
  Maximal byte throughput for inter-node connections is still somewhat lower than
  for TCP, while they are superior for intra-node and inter-container throughput
  on the same host.
  (TIPC 的消息传输时延优于任何其他已知协议。节点间连接的最大字节吞吐量仍略低于 TCP，但在同一主机上的节点内与
  容器间吞吐量方面则更为出色。)

- Language Support
  (语言支持)

  The TIPC user API has support for C, Python, Perl, Ruby, D and Go.
  (TIPC 用户 API 支持 C、Python、Perl、Ruby、D 和 Go。)

More Information
(更多信息)
----------------

- How to set up TIPC:
  (如何搭建 TIPC：)

  http://tipc.io/getting_started.html

- How to program with TIPC:
  (如何使用 TIPC 编程：)

  http://tipc.io/programming.html

- How to contribute to TIPC:
  (如何为 TIPC 做贡献：)

  http://tipc.io/contacts.html

- More details about TIPC specification:
  (关于 TIPC 规范的更多细节：)

  http://tipc.io/protocol.html


Implementation
(实现)
==============

TIPC is implemented as a kernel module in net/tipc/ directory.
(TIPC 以内核模块的形式实现在 net/tipc/ 目录中。)

TIPC Base Types
(TIPC 基础类型)
---------------

.. kernel-doc:: net/tipc/subscr.h
   :internal:

.. kernel-doc:: net/tipc/bearer.h
   :internal:

.. kernel-doc:: net/tipc/name_table.h
   :internal:

.. kernel-doc:: net/tipc/name_distr.h
   :internal:

.. kernel-doc:: net/tipc/bcast.c
   :internal:

TIPC Bearer Interfaces
(TIPC Bearer 接口)
----------------------

.. kernel-doc:: net/tipc/bearer.c
   :internal:

.. kernel-doc:: net/tipc/udp_media.c
   :internal:

TIPC Crypto Interfaces
(TIPC Crypto 接口)
----------------------

.. kernel-doc:: net/tipc/crypto.c
   :internal:

TIPC Discoverer Interfaces
(TIPC Discoverer 接口)
--------------------------

.. kernel-doc:: net/tipc/discover.c
   :internal:

TIPC Link Interfaces
(TIPC Link 接口)
--------------------

.. kernel-doc:: net/tipc/link.c
   :internal:

TIPC msg Interfaces
(TIPC msg 接口)
-------------------

.. kernel-doc:: net/tipc/msg.c
   :internal:

TIPC Name Interfaces
(TIPC Name 接口)
--------------------

.. kernel-doc:: net/tipc/name_table.c
   :internal:

.. kernel-doc:: net/tipc/name_distr.c
   :internal:

TIPC Node Management Interfaces
(TIPC 节点管理接口)
-------------------------------

.. kernel-doc:: net/tipc/node.c
   :internal:

TIPC Socket Interfaces
(TIPC Socket 接口)
----------------------

.. kernel-doc:: net/tipc/socket.c
   :internal:

TIPC Network Topology Interfaces
(TIPC 网络拓扑接口)
--------------------------------

.. kernel-doc:: net/tipc/subscr.c
   :internal:

TIPC Server Interfaces
(TIPC 服务器接口)
----------------------

.. kernel-doc:: net/tipc/topsrv.c
   :internal:

TIPC Trace Interfaces
(TIPC Trace 接口)
---------------------

.. kernel-doc:: net/tipc/trace.c
   :internal:
