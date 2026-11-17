.. SPDX-License-Identifier: GPL-2.0

=====================
Multipath TCP (MPTCP)
=====================

(多路径 TCP(MPTCP))

Introduction
(简介)
============

Multipath TCP or MPTCP is an extension to the standard TCP and is described in
`RFC 8684 (MPTCPv1) <https://www.rfc-editor.org/rfc/rfc8684.html>`_. It allows a
device to make use of multiple interfaces at once to send and receive TCP
packets over a single MPTCP connection. MPTCP can aggregate the bandwidth of
multiple interfaces or prefer the one with the lowest latency. It also allows a
fail-over if one path is down, and the traffic is seamlessly reinjected on other
paths.
(多路径 TCP(MPTCP)是对标准 TCP 的扩展，在
`RFC 8684 (MPTCPv1) <https://www.rfc-editor.org/rfc/rfc8684.html>`_ 中有描述。它
允许设备同时利用多个接口，通过单个 MPTCP 连接发送和接收 TCP 数据包。MPTCP 可以聚合
多个接口的带宽，或者优先选择延迟最低的接口。它还支持在某条路径失效时进行故障切换，
将流量无缝地重新注入到其他路径上。)

For more details about Multipath TCP in the Linux kernel, please see the
official website: `mptcp.dev <https://www.mptcp.dev>`_.
(有关 Linux 内核中多路径 TCP 的更多细节，请参阅官方网站：
`mptcp.dev <https://www.mptcp.dev>`_ 。)


Use cases
(使用场景)
=========

Thanks to MPTCP, being able to use multiple paths in parallel or simultaneously
brings new use-cases, compared to TCP:
(得益于 MPTCP，与 TCP 相比，能够并行或同时使用多条路径带来了新的使用场景：)

- Seamless handovers: switching from one path to another while preserving
  established connections, e.g. to be used in mobility use-cases, like on
  smartphones.
  (无缝切换：在保持已建立连接的同时从一条路径切换到另一条路径，例如用于移动性场景，
  如智能手机上。)
- Best network selection: using the "best" available path depending on some
  conditions, e.g. latency, losses, cost, bandwidth, etc.
  (最佳网络选择：根据某些条件(如延迟、丢包、成本、带宽等)使用"最佳"的可用路径。)
- Network aggregation: using multiple paths at the same time to have a higher
  throughput, e.g. to combine fixed and mobile networks to send files faster.
  (网络聚合：同时使用多条路径以获得更高的吞吐量，例如组合固定网络和移动网络以更快
  地发送文件。)


Concepts
(概念)
========

Technically, when a new socket is created with the ``IPPROTO_MPTCP`` protocol
(Linux-specific), a *subflow* (or *path*) is created. This *subflow* consists of
a regular TCP connection that is used to transmit data through one interface.
Additional *subflows* can be negotiated later between the hosts. For the remote
host to be able to detect the use of MPTCP, a new field is added to the TCP
*option* field of the underlying TCP *subflow*. This field contains, amongst
other things, a ``MP_CAPABLE`` option that tells the other host to use MPTCP if
it is supported. If the remote host or any middlebox in between does not support
it, the returned ``SYN+ACK`` packet will not contain MPTCP options in the TCP
*option* field. In that case, the connection will be "downgraded" to plain TCP,
and it will continue with a single path.
(从技术上讲，当使用 ``IPPROTO_MPTCP`` 协议(Linux 特有)创建新套接字时，会创建一个
*子流* (subflow，或称 *路径*，path)。这个 *子流* 由一条普通的 TCP 连接组成，用于通过
一个接口传输数据。之后可以在主机之间协商更多的 *子流*。为了让远端主机能够检测到
MPTCP 的使用，在底层 TCP *子流* 的 TCP *option* 字段中添加了一个新字段。该字段除其他
内容外，还包含一个 ``MP_CAPABLE`` 选项，告诉对方主机在支持的情况下使用 MPTCP。如果
远端主机或其间的任何中间盒不支持它，返回的 ``SYN+ACK`` 数据包将在 TCP *option* 字段
中不包含 MPTCP 选项。在这种情况下，连接将被"降级"为普通 TCP，并继续使用单条路径。)

This behavior is made possible by two internal components: the path manager, and
the packet scheduler.
(这一行为由两个内部组件实现：路径管理器(path manager)和数据包调度器(packet
scheduler)。)

Path Manager
(路径管理器)
------------

The Path Manager is in charge of *subflows*, from creation to deletion, and also
address announcements. Typically, it is the client side that initiates subflows,
and the server side that announces additional addresses via the ``ADD_ADDR`` and
``REMOVE_ADDR`` options.
(路径管理器负责 *子流* (subflows)，从创建到删除，还负责地址通告。通常，由客户端一侧
发起子流，由服务器一侧通过 ``ADD_ADDR`` 和 ``REMOVE_ADDR`` 选项通告额外的地址。)

Path managers are controlled by the ``net.mptcp.path_manager`` sysctl knob --
see mptcp-sysctl.rst. There are two types: the in-kernel one (``kernel``) where
the same rules are applied for all the connections (see: ``ip mptcp``) ; and the
userspace one (``userspace``), controlled by a userspace daemon (i.e. `mptcpd
<https://mptcpd.mptcp.dev/>`_) where different rules can be applied for each
connection. The path managers can be controlled via a Netlink API; see
../netlink/specs/mptcp_pm.rst.
(路径管理器由 ``net.mptcp.path_manager`` sysctl 开关控制——参见 mptcp-sysctl.rst。
有两种类型：内核内类型(``kernel``)，对所有连接应用相同的规则(参见 ``ip mptcp``)；
以及用户空间类型(``userspace``)，由用户空间守护进程(即 `mptcpd
<https://mptcpd.mptcp.dev/>`_)控制，可以为每个连接应用不同的规则。路径管理器可以通过
Netlink API 进行控制；参见 ../netlink/specs/mptcp_pm.rst。)

To be able to use multiple IP addresses on a host to create multiple *subflows*
(paths), the default in-kernel MPTCP path-manager needs to know which IP
addresses can be used. This can be configured with ``ip mptcp endpoint`` for
example.
(为了能够在一台主机上使用多个 IP 地址来创建多个 *子流* (路径)，默认的内核内 MPTCP
路径管理器需要知道可以使用哪些 IP 地址。例如，可以用 ``ip mptcp endpoint`` 来配置。)

Packet Scheduler
(数据包调度器)
----------------

The Packet Scheduler is in charge of selecting which available *subflow(s)* to
use to send the next data packet. It can decide to maximize the use of the
available bandwidth, only to pick the path with the lower latency, or any other
policy depending on the configuration.
(数据包调度器负责选择使用哪个可用的 *子流* 来发送下一个数据包。它可以决定最大化利用
可用带宽，只选择延迟较低的路径，或者根据配置采用任何其他策略。)

Packet schedulers are controlled by the ``net.mptcp.scheduler`` sysctl knob --
see mptcp-sysctl.rst.
(数据包调度器由 ``net.mptcp.scheduler`` sysctl 开关控制——参见 mptcp-sysctl.rst。)


Sockets API
(套接字 API)
===========

Creating MPTCP sockets
(创建 MPTCP 套接字)
----------------------

On Linux, MPTCP can be used by selecting MPTCP instead of TCP when creating the
``socket``:
(在 Linux 上，创建 ``socket`` 时选择 MPTCP 而不是 TCP 即可使用 MPTCP：)

.. code-block:: C

    int sd = socket(AF_INET(6), SOCK_STREAM, IPPROTO_MPTCP);

Note that ``IPPROTO_MPTCP`` is defined as ``262``.
(注意，``IPPROTO_MPTCP`` 被定义为 ``262``。)

If MPTCP is not supported, ``errno`` will be set to:
(如果不支持 MPTCP，``errno`` 将被设置为：)

- ``EINVAL``: (*Invalid argument*): MPTCP is not available, on kernels < 5.6.
  (``EINVAL``：(*无效参数*)：MPTCP 不可用，见于 < 5.6 的内核。)
- ``EPROTONOSUPPORT`` (*Protocol not supported*): MPTCP has not been compiled,
  on kernels >= v5.6.
  (``EPROTONOSUPPORT`` (*协议不受支持*)：MPTCP 未被编译进去，见于 >= v5.6 的
  内核。)
- ``ENOPROTOOPT`` (*Protocol not available*): MPTCP has been disabled using
  ``net.mptcp.enabled`` sysctl knob; see mptcp-sysctl.rst.
  (``ENOPROTOOPT`` (*协议不可用*)：MPTCP 已通过 ``net.mptcp.enabled`` sysctl
  开关被禁用；参见 mptcp-sysctl.rst。)

MPTCP is then opt-in: applications need to explicitly request it. Note that
applications can be forced to use MPTCP with different techniques, e.g.
``LD_PRELOAD`` (see ``mptcpize``), eBPF (see ``mptcpify``), SystemTAP,
``GODEBUG`` (``GODEBUG=multipathtcp=1``), etc.
(因此 MPTCP 是需要显式选择加入的：应用程序需要明确请求它。注意，可以用多种技术强制
应用程序使用 MPTCP，例如 ``LD_PRELOAD`` (参见 ``mptcpize``)、eBPF (参见
``mptcpify``)、SystemTAP、``GODEBUG`` (``GODEBUG=multipathtcp=1``) 等。)

Switching to ``IPPROTO_MPTCP`` instead of ``IPPROTO_TCP`` should be as
transparent as possible for the userspace applications.
(对于用户空间应用程序来说，改用 ``IPPROTO_MPTCP`` 而不是 ``IPPROTO_TCP`` 应当尽可能
透明。)

Socket options
(套接字选项)
--------------

MPTCP supports most socket options handled by TCP. It is possible some less
common options are not supported, but contributions are welcome.
(MPTCP 支持 TCP 所处理的大多数套接字选项。某些较少见的选项可能不受支持，但欢迎贡献。)

Generally, the same value is propagated to all subflows, including the ones
created after the calls to ``setsockopt()``. eBPF can be used to set different
values per subflow.
(通常，同一个值会传播到所有子流，包括在调用 ``setsockopt()`` 之后创建的那些子流。
eBPF 可用于为每个子流设置不同的值。)

There are some MPTCP specific socket options at the ``SOL_MPTCP`` (284) level to
retrieve info. They fill the ``optval`` buffer of the ``getsockopt()`` system
call:
(在 ``SOL_MPTCP`` (284)层级上有一些 MPTCP 特有的套接字选项，用于获取信息。它们会
填充 ``getsockopt()`` 系统调用的 ``optval`` 缓冲区：)

- ``MPTCP_INFO``: Uses ``struct mptcp_info``.
  (``MPTCP_INFO``：使用 ``struct mptcp_info``。)
- ``MPTCP_TCPINFO``: Uses ``struct mptcp_subflow_data``, followed by an array of
  ``struct tcp_info``.
  (``MPTCP_TCPINFO``：使用 ``struct mptcp_subflow_data``，后跟一个由
  ``struct tcp_info`` 组成的数组。)
- ``MPTCP_SUBFLOW_ADDRS``: Uses ``struct mptcp_subflow_data``, followed by an
  array of ``mptcp_subflow_addrs``.
  (``MPTCP_SUBFLOW_ADDRS``：使用 ``struct mptcp_subflow_data``，后跟一个由
  ``mptcp_subflow_addrs`` 组成的数组。)
- ``MPTCP_FULL_INFO``: Uses ``struct mptcp_full_info``, with one pointer to an
  array of ``struct mptcp_subflow_info`` (including the
  ``struct mptcp_subflow_addrs``), and one pointer to an array of
  ``struct tcp_info``, followed by the content of ``struct mptcp_info``.
  (``MPTCP_FULL_INFO``：使用 ``struct mptcp_full_info``，其中一个指针指向
  ``struct mptcp_subflow_info`` 数组(其中包括 ``struct mptcp_subflow_addrs``)，
  另一个指针指向 ``struct tcp_info`` 数组，后跟 ``struct mptcp_info`` 的内容。)

Note that at the TCP level, ``TCP_IS_MPTCP`` socket option can be used to know
if MPTCP is currently being used: the value will be set to 1 if it is.
(注意，在 TCP 层级上，可以使用 ``TCP_IS_MPTCP`` 套接字选项来了解当前是否正在使用
MPTCP：如果是，该值将被设置为 1。)


Design choices
(设计选择)
==============

A new socket type has been added for MPTCP for the userspace-facing socket. The
kernel is in charge of creating subflow sockets: they are TCP sockets where the
behavior is modified using TCP-ULP.
(为面向用户空间的套接字新增了一种 MPTCP 套接字类型。内核负责创建子流套接字：它们是
TCP 套接字，其行为通过 TCP-ULP 进行修改。)

MPTCP listen sockets will create "plain" *accepted* TCP sockets if the
connection request from the client didn't ask for MPTCP, making the performance
impact minimal when MPTCP is enabled by default.
(如果来自客户端的连接请求没有请求 MPTCP，MPTCP 监听套接字将创建"普通"的
*accepted* TCP 套接字，从而在默认启用 MPTCP 时把性能影响降到最低。)
