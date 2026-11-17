.. SPDX-License-Identifier: GPL-2.0-only

===================================
PSP Security Protocol(PSP 安全协议)
===================================

Protocol(协议)
==============

PSP Security Protocol (PSP) was defined at Google and published in:

https://raw.githubusercontent.com/google/psp/main/doc/PSP_Arch_Spec.pdf

(PSP 安全协议 (PSP) 由 Google 定义并发布于：
https://raw.githubusercontent.com/google/psp/main/doc/PSP_Arch_Spec.pdf)

This section briefly covers protocol aspects crucial for understanding
the kernel API. Refer to the protocol specification for further details.
(本节简要介绍理解内核 API 至关重要的协议内容。更多细节请参阅协议规范。)

Note that the kernel implementation and documentation uses the term
"device key" in place of "master key", it is both less confusing
to an average developer and is less likely to run afoul any naming
guidelines.
(注意，内核实现和文档使用"设备密钥" (device key) 一词代替"主密钥" (master key)，这既不容易让普通开发者混淆，也不太可能违反各种命名准则。)

Derived Rx keys(派生 Rx 密钥)
-----------------------------

PSP borrows some terms and mechanisms from IPsec. PSP was designed
with HW offloads in mind. The key feature of PSP is that Rx keys for every
connection do not have to be stored by the receiver but can be derived
from device key and information present in packet headers.
This makes it possible to implement receivers which require a constant
amount of memory regardless of the number of connections (``O(1)`` scaling).
(PSP 借用了 IPsec 的一些术语和机制。PSP 的设计以硬件卸载为出发点。PSP 的关键特性在于：每个连接的 Rx 密钥不必由接收方存储，而是可以从设备密钥和数据包头中的信息推导得出。这使得实现接收方所需的内存量与连接数无关成为可能（``O(1)`` 扩展性）。)

Tx keys have to be stored like with any other protocol, but Tx is much
less latency sensitive than Rx, and delays in fetching keys from slow
memory is less likely to cause packet drops. Preferably, the Tx keys
should be provided with the packet (e.g. as part of the descriptors).
(Tx 密钥与其他协议一样需要存储，但 Tx 对延迟的敏感度远低于 Rx，从慢速内存取密钥的延迟不太可能导致丢包。更可取的做法是将 Tx 密钥随数据包一起提供（例如作为描述符的一部分）。)

Key rotation(密钥轮换)
----------------------

The device key known only to the receiver is fundamental to the design.
Per specification this state cannot be directly accessible (it must be
impossible to read it out of the hardware of the receiver NIC).
Moreover, it has to be "rotated" periodically (usually daily). Rotation
means that new device key gets generated (by a random number generator
of the device), and used for all new connections. To avoid disrupting
old connections the old device key remains in the NIC. A phase bit
carried in the packet headers indicates which generation of device key
the packet has been encrypted with.
(只有接收方知道的设备密钥是设计的基础。按照规范，该状态不能被直接访问（必须无法从接收方网卡的硬件中读出）。此外，它必须定期"轮换"（通常每天一次）。轮换意味着生成新的设备密钥（由设备的随机数生成器生成）并用于所有新连接。为避免干扰旧连接，旧设备密钥仍保留在网卡中。数据包头中携带的一个相位位 (phase bit) 指示该数据包是用哪一代设备密钥加密的。)

User facing API(面向用户 API)
=============================

PSP is designed primarily for hardware offloads. There is currently
no software fallback for systems which do not have PSP capable NICs.
There is also no standard (or otherwise defined) way of establishing
a PSP-secured connection or exchanging the symmetric keys.
(PSP 主要为硬件卸载而设计。对于没有支持 PSP 的网卡的系统，目前没有软件回退方案。也没有标准（或其他已定义）的方式来建立 PSP 安全连接或交换对称密钥。)

The expectation is that higher layer protocols will take care of
protocol and key negotiation. For example one may use TLS key exchange,
announce the PSP capability, and switch to PSP if both endpoints
are PSP-capable.
(预期由更高层的协议负责协议与密钥协商。例如可以使用 TLS 密钥交换，宣告 PSP 能力，并在两端都支持 PSP 时切换到 PSP。)

All configuration of PSP is performed via the PSP netlink family.
(PSP 的所有配置都通过 PSP netlink 族执行。)

Device discovery(设备发现)
--------------------------

The PSP netlink family defines operations to retrieve information
about the PSP devices available on the system, configure them and
access PSP related statistics.
(PSP netlink 族定义了检索系统上可用 PSP 设备信息、配置这些设备以及访问 PSP 相关统计信息的操作。)

Securing a connection(保护一个连接)
-----------------------------------

PSP encryption is currently only supported for TCP connections.
Rx and Tx keys are allocated separately. First the ``rx-assoc``
Netlink command needs to be issued, specifying a target TCP socket.
Kernel will allocate a new PSP Rx key from the NIC and associate it
with given socket. At this stage socket will accept both PSP-secured
and plain text TCP packets.
(目前仅支持对 TCP 连接进行 PSP 加密。Rx 和 Tx 密钥分开分配。首先需要发出 ``rx-assoc`` netlink 命令，指定目标 TCP 套接字。内核将从网卡分配一个新的 PSP Rx 密钥并关联到给定的套接字。在此阶段，套接字将同时接受经 PSP 保护的和明文的 TCP 数据包。)

Tx keys are installed using the ``tx-assoc`` Netlink command.
Once the Tx keys are installed, all data read from the socket will
be PSP-secured. In other words act of installing Tx keys has a secondary
effect on the Rx direction.
(Tx 密钥使用 ``tx-assoc`` netlink 命令安装。一旦 Tx 密钥安装完毕，从套接字读取的所有数据都将受 PSP 保护。换句话说，安装 Tx 密钥的动作对 Rx 方向有附带影响。)

There is an intermediate period after ``tx-assoc`` successfully
returns and before the TCP socket encounters it's first PSP
authenticated packet, where the TCP stack will allow certain nondata
packets, i.e. ACKs, FINs, and RSTs, to enter TCP receive processing
even if not PSP authenticated. During the ``tx-assoc`` call, the TCP
socket's ``rcv_nxt`` field is recorded. At this point, ACKs and RSTs
will be accepted with any sequence number, while FINs will only be
accepted at the latched value of ``rcv_nxt``. Once the TCP stack
encounters the first TCP packet containing PSP authenticated data, the
other end of the connection must have executed the ``tx-assoc``
command, so any TCP packet, including those without data, will be
dropped before receive processing if it is not successfully
authenticated. This is summarized in the table below. The
aforementioned state of rejecting all non-PSP packets is labeled "PSP
Full".
(在 ``tx-assoc`` 成功返回之后、TCP 套接字遇到第一个经 PSP 认证的数据包之前，存在一个中间阶段：在此阶段，TCP 协议栈允许某些非数据包（即 ACK、FIN 和 RST）即使未通过 PSP 认证也进入 TCP 接收处理。在 ``tx-assoc`` 调用期间，TCP 套接字的 ``rcv_nxt`` 字段会被记录。此时，ACK 和 RST 可以以任意序号被接受，而 FIN 只能在锁存的 ``rcv_nxt`` 值处被接受。一旦 TCP 协议栈遇到第一个包含经 PSP 认证数据的 TCP 数据包，对端必然已执行了 ``tx-assoc`` 命令，因此任何 TCP 数据包（包括不含数据的包）如果未通过认证，都会在接收处理之前被丢弃。下表对此进行了总结。上述拒绝所有非 PSP 数据包的状态被标记为 "PSP Full"（完全 PSP）。)

+----------------+------------+------------+-------------+-------------+
| Event (事件)   | Normal TCP | Rx PSP (接)| Tx PSP(发送)| PSP Full(全)|
+================+============+============+=============+=============+
| Rx plain (接收)| accept     | accept     | drop        | drop        |
| (data)         | (接受)     | (接受)     | (丢弃)      | (丢弃)      |
+----------------+------------+------------+-------------+-------------+
| Rx plain (接收)| accept     | accept     | accept      | drop        |
| (ACK|FIN|RST)  | (接受)     | (接受)     | (接受)      | (丢弃)      |
+----------------+------------+------------+-------------+-------------+
| Rx PSP (good)  | drop (丢)  | accept (接)| accept(接受)| accept(接受)|
+----------------+------------+------------+-------------+-------------+
| Rx PSP (bad    | drop (丢)  | drop (丢)  | drop(丢弃)  | drop(丢弃)  |
| crypt, !=SPI)  |            |            |             |             |
+----------------+------------+------------+-------------+-------------+
| Tx (发送)      | plain text | plain text | encrypted   | encrypted   |
|                | (明文)     | (明文)     | (excl. rtx) | (excl. rtx) |
+----------------+------------+------------+-------------+-------------+

To ensure that any data read from the socket after the ``tx-assoc``
call returns success has been authenticated, the kernel will scan the
receive and ofo queues of the socket at ``tx-assoc`` time. If any
enqueued packet was received in clear text, the Tx association will
fail, and the application should retry installing the Tx key after
draining the socket (this should not be necessary if both endpoints
are well behaved).
(为确保 ``tx-assoc`` 调用成功返回之后从套接字读取的任何数据都经过认证，内核会在 ``tx-assoc`` 时扫描套接字的接收队列和 ofo 队列。如果有任何已入队的数据包是以明文接收的，Tx 关联将失败，应用程序应在排空套接字后重试安装 Tx 密钥（如果两端都行为良好，这应该不是必需的）。)

Because TCP sequence numbers are not integrity protected prior to
upgrading to PSP, it is possible that a MITM could offset sequence
numbers in a way that deletes a prefix of the PSP protected part of
the TCP stream. If userspace cares to mitigate this type of attack, a
special "start of PSP" message should be exchanged after ``tx-assoc``.
(由于在升级到 PSP 之前 TCP 序号没有完整性保护，中间人 (MITM) 有可能以某种方式偏移序号，从而删除 TCP 流中受 PSP 保护部分的前缀。如果用户态想缓解这类攻击，应在 ``tx-assoc`` 之后交换一条特殊的 "start of PSP" 消息。)

Rotation notifications(轮换通知)
--------------------------------

The rotations of device key happen asynchronously and are usually
performed by management daemons, not under application control.
The PSP netlink family will generate a notification whenever keys
are rotated. The applications are expected to re-establish connections
before keys are rotated again.
(设备密钥的轮换是异步发生的，通常由管理守护进程执行，不受应用程序控制。每当密钥轮换时，PSP netlink 族都会生成一个通知。应用程序应在密钥再次轮换之前重建连接。)

Kernel implementation(内核实现)
===============================

Driver notes(驱动须知)
----------------------

Drivers are expected to start with no PSP enabled (``psp-versions-ena``
in ``dev-get`` set to ``0``) whenever possible. The user space should
not depend on this behavior, as future extension may necessitate creation
of devices with PSP already enabled, nonetheless drivers should not enable
PSP by default. Enabling PSP should be the responsibility of the system
component which also takes care of key rotation.
(驱动应尽可能以未启用 PSP 的状态启动（``dev-get`` 中的 ``psp-versions-ena`` 设为 ``0`` ）。用户态不应依赖这一行为，因为未来的扩展可能需要创建已启用 PSP 的设备；尽管如此，驱动不应默认启用 PSP。启用 PSP 应由同时负责密钥轮换的系统组件来承担。)

Note that ``psp-versions-ena`` is expected to be used only for enabling
receive processing. The device is not expected to reject transmit requests
after ``psp-versions-ena`` has been disabled. User may also disable
``psp-versions-ena`` while there are active associations, which will
break all PSP Rx processing.
(注意，``psp-versions-ena`` 应只用于启用接收处理。在 ``psp-versions-ena`` 被禁用后，设备不应拒绝发送请求。用户也可以在存在活动关联时禁用 ``psp-versions-ena``，这会中断所有 PSP Rx 处理。)

Drivers are expected to ensure that a device key is usable and secure
upon init, without explicit key rotation by the user space. It must be
possible to allocate working keys, and that no duplicate keys must be
generated. If the device allows the host to request the key for an
arbitrary SPI - driver should discard both device keys (rotate the
device key twice), to avoid potentially using a SPI+key which previous
OS instance already had access to.
(驱动应确保在初始化时无需用户态显式轮换密钥，设备密钥即已可用且安全。必须能够分配可用的密钥，且不得生成重复密钥。如果设备允许主机为任意 SPI 请求密钥 - 驱动应丢弃两个设备密钥（把设备密钥轮换两次），以避免潜在地使用先前 OS 实例已经接触过的 SPI+密钥组合。)

Drivers must use ``psp_skb_get_assoc_rcu()`` to check if PSP Tx offload
was requested for given skb. On Rx drivers should allocate and populate
the ``SKB_EXT_PSP`` skb extension, and set the skb->decrypted bit to 1.
(驱动必须使用 ``psp_skb_get_assoc_rcu()`` 来检查给定 skb 是否请求了 PSP Tx 卸载。在 Rx 方向，驱动应分配并填充 ``SKB_EXT_PSP`` skb 扩展，并将 skb->decrypted 位设为 1。)

Kernel implementation notes(内核实现须知)
-----------------------------------------

PSP implementation follows the TLS offload more closely than the IPsec
offload, with per-socket state, and the use of skb->decrypted to prevent
clear text leaks.
(PSP 实现更接近 TLS 卸载而不是 IPsec 卸载：使用每套接字状态，并利用 skb->decrypted 来防止明文泄露。)

PSP device is separate from netdev, to make it possible to "delegate"
PSP offload capabilities to software devices (e.g. ``veth``).
(PSP 设备与 netdev 相互独立，以便能够把 PSP 卸载能力"委托"给软件设备（例如 ``veth`` ）。)
