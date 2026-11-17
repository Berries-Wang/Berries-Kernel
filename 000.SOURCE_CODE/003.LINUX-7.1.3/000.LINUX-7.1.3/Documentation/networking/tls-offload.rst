.. SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)

==================
Kernel TLS offload
==================

(内核 TLS 卸载)

Kernel TLS operation
(内核 TLS 操作)
====================

Linux kernel provides TLS connection offload infrastructure. Once a TCP
connection is in ``ESTABLISHED`` state user space can enable the TLS Upper
Layer Protocol (ULP) and install the cryptographic connection state.
For details regarding the user-facing interface refer to the TLS
documentation in :ref:`Documentation/networking/tls.rst <kernel_tls>`.
(Linux 内核提供 TLS 连接卸载基础设施。一旦 TCP 连接进入 ``ESTABLISHED``
状态，用户空间就可以启用 TLS 上层协议 (ULP) 并安装加密连接状态。关于
用户侧接口的详细信息，参见 :ref:`Documentation/networking/tls.rst
<kernel_tls>` 中的 TLS 文档。)

``ktls`` can operate in three modes:
(``ktls`` 可以在三种模式下工作：)

 * Software crypto mode (``TLS_SW``) - CPU handles the cryptography.
   In most basic cases only crypto operations synchronous with the CPU
   can be used, but depending on calling context CPU may utilize
   asynchronous crypto accelerators. The use of accelerators introduces extra
   latency on socket reads (decryption only starts when a read syscall
   is made) and additional I/O load on the system.
   (软件加密模式 (``TLS_SW``) - 由 CPU 处理加密。在最基本的情况下，只能
   使用与 CPU 同步的加密操作，但根据调用上下文，CPU 可以利用异步加密加速
   器。使用加速器会在套接字读取时引入额外延迟 (只有当发出读系统调用时才
   开始解密)，并给系统带来额外的 I/O 负载。)
 * Packet-based NIC offload mode (``TLS_HW``) - the NIC handles crypto
   on a packet by packet basis, provided the packets arrive in order.
   This mode integrates best with the kernel stack and is described in detail
   in the remaining part of this document
   (``ethtool`` flags ``tls-hw-tx-offload`` and ``tls-hw-rx-offload``).
   (基于数据包的网卡卸载模式 (``TLS_HW``) - 由网卡逐包处理加密，前提是
   数据包按序到达。该模式与内核协议栈集成得最好，本文其余部分将详细描述
   它 (``ethtool`` 标志 ``tls-hw-tx-offload`` 和 ``tls-hw-rx-offload``)。)
 * Full TCP NIC offload mode (``TLS_HW_RECORD``) - mode of operation where
   NIC driver and firmware replace the kernel networking stack
   with its own TCP handling, it is not usable in production environments
   making use of the Linux networking stack for example any firewalling
   abilities or QoS and packet scheduling (``ethtool`` flag ``tls-hw-record``).
   (完全 TCP 网卡卸载模式 (``TLS_HW_RECORD``) - 网卡驱动和固件用自身的
   TCP 处理取代内核网络协议栈的工作模式；在使用 Linux 网络协议栈能力
   (例如防火墙或 QoS 与数据包调度) 的生产环境中不可用 (``ethtool`` 标志
   ``tls-hw-record``)。)

The operation mode is selected automatically based on device configuration,
offload opt-in or opt-out on per-connection basis is not currently supported.
(操作模式根据设备配置自动选择，目前尚不支持按连接选择加入或退出卸载。)

TX
(发送)
--

At a high level user write requests are turned into a scatter list, the TLS ULP
intercepts them, inserts record framing, performs encryption (in ``TLS_SW``
mode) and then hands the modified scatter list to the TCP layer. From this
point on the TCP stack proceeds as normal.
(从较高层次看，用户的写请求被转换成分散列表 (scatter list)，TLS ULP
拦截它们，插入记录框架，执行加密 (在 ``TLS_SW`` 模式下)，然后把修改后
的分散列表交给 TCP 层。从这一点开始，TCP 协议栈照常继续处理。)

In ``TLS_HW`` mode the encryption is not performed in the TLS ULP.
Instead packets reach a device driver, the driver will mark the packets
for crypto offload based on the socket the packet is attached to,
and send them to the device for encryption and transmission.
(在 ``TLS_HW`` 模式下，加密不在 TLS ULP 中执行。数据包会到达设备驱动，
驱动根据数据包所属的套接字将其标记为需要加密卸载，并送往设备进行加密和
传输。)

RX
(接收)
--

On the receive side, if the device handled decryption and authentication
successfully, the driver will set the decrypted bit in the associated
:c:type:`struct sk_buff <sk_buff>`. The packets reach the TCP stack and
are handled normally. ``ktls`` is informed when data is queued to the socket
and the ``strparser`` mechanism is used to delineate the records. Upon read
request, records are retrieved from the socket and passed to decryption routine.
If device decrypted all the segments of the record the decryption is skipped,
otherwise software path handles decryption.
(在接收侧，如果设备成功完成了解密和认证，驱动会在相应的
:c:type:`struct sk_buff <sk_buff>` 中设置 decrypted 位。数据包到达 TCP
协议栈后照常处理。当数据入队到套接字时 ``ktls`` 会得到通知，并使用
``strparser`` 机制来划分记录边界。收到读请求时，记录从套接字取出并交给
解密例程。如果设备已解密记录的所有分段，则跳过解密，否则由软件路径处理
解密。)

.. kernel-figure::  tls-offload-layers.svg
   :alt:	TLS offload layers
   :align:	center
   :figwidth:	28em

   Layers of Kernel TLS stack
   (内核 TLS 协议栈的层次)

Device configuration
(设备配置)
====================

During driver initialization device sets the ``NETIF_F_HW_TLS_RX`` and
``NETIF_F_HW_TLS_TX`` features and installs its
:c:type:`struct tlsdev_ops <tlsdev_ops>`
pointer in the :c:member:`tlsdev_ops` member of the
:c:type:`struct net_device <net_device>`.
(在驱动初始化期间，设备会设置 ``NETIF_F_HW_TLS_RX`` 和
``NETIF_F_HW_TLS_TX`` 特性，并把它的 :c:type:`struct tlsdev_ops
<tlsdev_ops>` 指针安装到 :c:type:`struct net_device <net_device>` 的
:c:member:`tlsdev_ops` 成员中。)

When TLS cryptographic connection state is installed on a ``ktls`` socket
(note that it is done twice, once for RX and once for TX direction,
and the two are completely independent), the kernel checks if the underlying
network device is offload-capable and attempts the offload. In case offload
fails the connection is handled entirely in software using the same mechanism
as if the offload was never tried.
(当在 ``ktls`` 套接字上安装 TLS 加密连接状态时 (注意这一操作会执行两次，
RX 和 TX 方向各一次，且二者完全独立)，内核会检查底层网络设备是否具备
卸载能力并尝试卸载。如果卸载失败，该连接将完全以软件方式处理，使用的
机制与从未尝试过卸载时相同。)

Offload request is performed via the :c:member:`tls_dev_add` callback of
:c:type:`struct tlsdev_ops <tlsdev_ops>`:
(卸载请求通过 :c:type:`struct tlsdev_ops <tlsdev_ops>` 的
:c:member:`tls_dev_add` 回调执行：)

.. code-block:: c

	int (*tls_dev_add)(struct net_device *netdev, struct sock *sk,
			   enum tls_offload_ctx_dir direction,
			   struct tls_crypto_info *crypto_info,
			   u32 start_offload_tcp_sn);

``direction`` indicates whether the cryptographic information is for
the received or transmitted packets. Driver uses the ``sk`` parameter
to retrieve the connection 5-tuple and socket family (IPv4 vs IPv6).
Cryptographic information in ``crypto_info`` includes the key, iv, salt
as well as TLS record sequence number. ``start_offload_tcp_sn`` indicates
which TCP sequence number corresponds to the beginning of the record with
sequence number from ``crypto_info``. The driver can add its state
at the end of kernel structures (see :c:member:`driver_state` members
in ``include/net/tls.h``) to avoid additional allocations and pointer
dereferences.
(``direction`` 指示加密信息是针对接收的还是发送的数据包。驱动使用
``sk`` 参数获取连接的五元组和套接字协议族 (IPv4 还是 IPv6)。
``crypto_info`` 中的加密信息包括密钥、iv、盐值 (salt) 以及 TLS 记录
序列号。``start_offload_tcp_sn`` 指示哪个 TCP 序列号对应于以
``crypto_info`` 中序列号开头的记录的起始位置。驱动可以把自身状态添加到
内核结构的末尾 (见 ``include/net/tls.h`` 中的 :c:member:`driver_state`
成员)，以避免额外的分配和指针解引用。)

TX
(发送)
--

After TX state is installed, the stack guarantees that the first segment
of the stream will start exactly at the ``start_offload_tcp_sn`` sequence
number, simplifying TCP sequence number matching.
(TX 状态安装后，协议栈保证流的第一个分段恰好从 ``start_offload_tcp_sn``
序列号开始，从而简化 TCP 序列号匹配。)

TX offload being fully initialized does not imply that all segments passing
through the driver and which belong to the offloaded socket will be after
the expected sequence number and will have kernel record information.
In particular, already encrypted data may have been queued to the socket
before installing the connection state in the kernel.
(TX 卸载完全初始化并不意味着所有经过驱动且属于已卸载套接字的分段都位于
预期序列号之后并带有内核记录信息。特别是，在内核中安装连接状态之前，
可能已经有已加密的数据入队到套接字。)

RX
(接收)
--

In the RX direction, the local networking stack has little control over
segmentation, so the initial records' TCP sequence number may be anywhere
inside the segment.
(在 RX 方向上，本地网络协议栈对分段几乎没有控制权，因此初始记录的 TCP
序列号可能位于分段内的任何位置。)

Normal operation
(正常操作)
================

At the minimum the device maintains the following state for each connection, in
each direction:
(设备至少要为每个连接、每个方向维护以下状态：)

 * crypto secrets (key, iv, salt)
   (加密密钥材料 (key、iv、salt))
 * crypto processing state (partial blocks, partial authentication tag, etc.)
   (加密处理状态 (不完整的数据块、不完整的认证标签等))
 * record metadata (sequence number, processing offset and length)
   (记录元数据 (序列号、处理偏移量与长度))
 * expected TCP sequence number
   (期望的 TCP 序列号)

There are no guarantees on record length or record segmentation. In particular
segments may start at any point of a record and contain any number of records.
Assuming segments are received in order, the device should be able to perform
crypto operations and authentication regardless of segmentation. For this
to be possible, the device has to keep a small amount of segment-to-segment
state. This includes at least:
(记录长度或记录的分段方式没有任何保证。特别是，分段可能从记录的任意
位置开始，并包含任意数量的记录。假设分段按序到达，设备应能不论分段方式
如何都执行加密操作和认证。要做到这一点，设备必须保存少量分段与分段之间
的状态。这至少包括：)

 * partial headers (if a segment carried only a part of the TLS header)
   (不完整的头部 (如果某个分段只携带了 TLS 头部的一部分))
 * partial data block
   (不完整的数据块)
 * partial authentication tag (all data had been seen but part of the
   authentication tag has to be written or read from the subsequent segment)
   (不完整的认证标签 (所有数据都已读取，但认证标签的一部分必须写入
   或读取自后续分段))

Record reassembly is not necessary for TLS offload. If the packets arrive
in order the device should be able to handle them separately and make
forward progress.
(对于 TLS 卸载来说，记录重组并非必需。如果数据包按序到达，设备应能分别
处理它们并持续推进。)

TX
(发送)
--

The kernel stack performs record framing reserving space for the authentication
tag and populating all other TLS header and tailer fields.
(内核协议栈执行记录组帧，为认证标签预留空间，并填充所有其他 TLS 头部
和尾部字段。)

Both the device and the driver maintain expected TCP sequence numbers
due to the possibility of retransmissions and the lack of software fallback
once the packet reaches the device.
For segments passed in order, the driver marks the packets with
a connection identifier (note that a 5-tuple lookup is insufficient to identify
packets requiring HW offload, see the :ref:`5tuple_problems` section)
and hands them to the device. The device identifies the packet as requiring
TLS handling and confirms the sequence number matches its expectation.
The device performs encryption and authentication of the record data.
It replaces the authentication tag and TCP checksum with correct values.
(由于可能存在重传，且数据包一旦到达设备后就没有软件回退手段，设备和驱动
都要维护期望的 TCP 序列号。对于按序传递的分段，驱动用连接标识符标记
数据包 (注意，五元组查找不足以识别需要硬件卸载的数据包，参见
:ref:`5tuple_problems` 一节)，并把它们交给设备。设备识别出该数据包需要
TLS 处理，并确认序列号与自己的期望相符。设备对记录数据执行加密和认证，
用正确的值替换认证标签和 TCP 校验和。)

RX
(接收)
--

Before a packet is DMAed to the host (but after NIC's embedded switching
and packet transformation functions) the device validates the Layer 4
checksum and performs a 5-tuple lookup to find any TLS connection the packet
may belong to (technically a 4-tuple
lookup is sufficient - IP addresses and TCP port numbers, as the protocol
is always TCP). If the packet is matched to a connection, the device confirms
if the TCP sequence number is the expected one and proceeds to TLS handling
(record delineation, decryption, authentication for each record in the packet).
The device leaves the record framing unmodified, the stack takes care of record
decapsulation. Device indicates successful handling of TLS offload in the
per-packet context (descriptor) passed to the host.
(在数据包被 DMA 到主机之前 (但在网卡的内置交换和数据包变换功能之后)，
设备会校验第 4 层校验和，并执行五元组查找，以找到该数据包可能所属的
TLS 连接 (严格来说四元组查找就足够了——IP 地址和 TCP 端口号，因为协议
总是 TCP)。如果数据包匹配到某个连接，设备会确认 TCP 序列号是否为期望
值，然后进行 TLS 处理 (对包中的每个记录进行记录划分、解密、认证)。设备
不修改记录框架，由协议栈负责记录解封装。设备在传给主机的每包上下文
(描述符) 中指示 TLS 卸载处理成功。)

Upon reception of a TLS offloaded packet, the driver sets
the :c:member:`decrypted` mark in :c:type:`struct sk_buff <sk_buff>`
corresponding to the segment. Networking stack makes sure decrypted
and non-decrypted segments do not get coalesced (e.g. by GRO or socket layer)
and takes care of partial decryption.
(收到 TLS 卸载的数据包后，驱动会在对应分段的
:c:type:`struct sk_buff <sk_buff>` 中设置 :c:member:`decrypted` 标记。
网络协议栈确保已解密与未解密的分段不会被合并 (例如被 GRO 或套接字层
合并)，并负责处理部分解密的情况。)

Resync handling
(重新同步处理)
===============

In presence of packet drops or network packet reordering, the device may lose
synchronization with the TLS stream, and require a resync with the kernel's
TCP stack.
(在出现丢包或网络数据包乱序的情况下，设备可能失去与 TLS 流的同步，
需要与内核的 TCP 协议栈重新同步。)

Note that resync is only attempted for connections which were successfully
added to the device table and are in TLS_HW mode. For example,
if the table was full when cryptographic state was installed in the kernel,
such connection will never get offloaded. Therefore the resync request
does not carry any cryptographic connection state.
(注意，只会对已成功添加到设备表中且处于 TLS_HW 模式的连接尝试重新同步。
例如，如果在内核中安装加密状态时表已满，这样的连接将永远不会被卸载。
因此重新同步请求不携带任何加密连接状态。)

TX
(发送)
--

Segments transmitted from an offloaded socket can get out of sync
in similar ways to the receive side-retransmissions - local drops
are possible, though network reorders are not. There are currently
two mechanisms for dealing with out of order segments.
(从已卸载套接字发送的分段可能以与接收侧类似的方式失去同步——重传是
可能的，本地丢包也可能发生，但网络乱序不会。目前有两种处理乱序分段的
机制。)

Crypto state rebuilding
(加密状态重建)
~~~~~~~~~~~~~~~~~~~~~~~

Whenever an out of order segment is transmitted the driver provides
the device with enough information to perform cryptographic operations.
This means most likely that the part of the record preceding the current
segment has to be passed to the device as part of the packet context,
together with its TCP sequence number and TLS record number. The device
can then initialize its crypto state, process and discard the preceding
data (to be able to insert the authentication tag) and move onto handling
the actual packet.
(每当发送乱序分段时，驱动都会向设备提供足够的信息以执行加密操作。这
很可能意味着记录中位于当前分段之前的部分必须作为数据包上下文的一部分
传给设备，同时传给设备的还有其 TCP 序列号和 TLS 记录号。设备随后可以
初始化自己的加密状态，处理并丢弃之前的数据 (以便插入认证标签)，然后
转向处理实际的数据包。)

In this mode depending on the implementation the driver can either ask
for a continuation with the crypto state and the new sequence number
(next expected segment is the one after the out of order one), or continue
with the previous stream state - assuming that the out of order segment
was just a retransmission. The former is simpler, and does not require
retransmission detection therefore it is the recommended method until
such time it is proven inefficient.
(在这种模式下，取决于具体实现，驱动既可以请求以该加密状态和新序列号
继续 (下一个期望分段是乱序分段之后的那一个)，也可以按之前的流状态
继续——假定乱序分段只是一次重传。前者更简单，且不需要重传检测，因此
在被证明低效之前，它是推荐的方法。)

Next record sync
(下一记录同步)
~~~~~~~~~~~~~~~~

Whenever an out of order segment is detected the driver requests
that the ``ktls`` software fallback code encrypt it. If the segment's
sequence number is lower than expected the driver assumes retransmission
and doesn't change device state. If the segment is in the future, it
may imply a local drop, the driver asks the stack to sync the device
to the next record state and falls back to software.
(每当检测到乱序分段，驱动就请求 ``ktls`` 软件回退代码对其进行加密。
如果该分段的序列号低于期望值，驱动假定发生了重传，不改变设备状态。
如果分段在期望值之后，则可能意味着本地发生了丢包，驱动请求协议栈把
设备同步到下一记录状态，并回退到软件处理。)

Resync request is indicated with:
(重新同步请求通过以下函数指示：)

.. code-block:: c

  void tls_offload_tx_resync_request(struct sock *sk, u32 got_seq, u32 exp_seq)

Until resync is complete driver should not access its expected TCP
sequence number (as it will be updated from a different context).
Following helper should be used to test if resync is complete:
(在重新同步完成之前，驱动不应访问其期望的 TCP 序列号 (因为它会在另一个
上下文中被更新)。应使用以下辅助函数来测试重新同步是否完成：)

.. code-block:: c

  bool tls_offload_tx_resync_pending(struct sock *sk)

Next time ``ktls`` pushes a record it will first send its TCP sequence number
and TLS record number to the driver. Stack will also make sure that
the new record will start on a segment boundary (like it does when
the connection is initially added).
(下一次 ``ktls`` 推送记录时，会先把它的 TCP 序列号和 TLS 记录号发送给
驱动。协议栈还会确保新记录从一个分段边界开始 (就像连接最初添加时
那样)。)

RX
(接收)
--

A small amount of RX reorder events may not require a full resynchronization.
In particular the device should not lose synchronization
when record boundary can be recovered:
(少量的 RX 乱序事件可能不需要完整的重新同步。特别是，当记录边界可以
恢复时，设备不应失去同步：)

.. kernel-figure::  tls-offload-reorder-good.svg
   :alt:	reorder of non-header segment
   :align:	center

   Reorder of non-header segment
   (非头部分段的乱序)

Green segments are successfully decrypted, blue ones are passed
as received on wire, red stripes mark start of new records.
(绿色分段已被成功解密，蓝色分段按线上接收到的原样传递，红色条纹标记
新记录的开始。)

In above case segment 1 is received and decrypted successfully.
Segment 2 was dropped so 3 arrives out of order. The device knows
the next record starts inside 3, based on record length in segment 1.
Segment 3 is passed untouched, because due to lack of data from segment 2
the remainder of the previous record inside segment 3 cannot be handled.
The device can, however, collect the authentication algorithm's state
and partial block from the new record in segment 3 and when 4 and 5
arrive continue decryption. Finally when 2 arrives it's completely outside
of expected window of the device so it's passed as is without special
handling. ``ktls`` software fallback handles the decryption of record
spanning segments 1, 2 and 3. The device did not get out of sync,
even though two segments did not get decrypted.
(在上图的情况中，分段 1 被成功接收并解密。分段 2 被丢弃，因此 3 乱序
到达。设备根据分段 1 中的记录长度知道下一个记录从分段 3 内部开始。
分段 3 原样传递，因为由于缺少分段 2 的数据，分段 3 内部上一个记录的
剩余部分无法处理。不过，设备可以从分段 3 中的新记录收集认证算法的
状态和不完整的数据块，当 4 和 5 到达时继续解密。最后，当 2 到达时，
它完全在设备的期望窗口之外，因此按原样传递而不做特殊处理。跨越分段
1、2 和 3 的记录的解密由 ``ktls`` 软件回退处理。即使有两个分段没有
得到解密，设备也没有失去同步。)

Kernel synchronization may be necessary if the lost segment contained
a record header and arrived after the next record header has already passed:
(如果丢失的分段包含记录头部，并且是在下一个记录头部已经过去之后才到达，
则可能需要内核同步：)

.. kernel-figure::  tls-offload-reorder-bad.svg
   :alt:	reorder of header segment
   :align:	center

   Reorder of segment with a TLS header
   (含 TLS 头部的分段乱序)

In this example segment 2 gets dropped, and it contains a record header.
Device can only detect that segment 4 also contains a TLS header
if it knows the length of the previous record from segment 2. In this case
the device will lose synchronization with the stream.
(在本例中，分段 2 被丢弃，而它包含一个记录头部。设备只有知道分段 2 中
上一个记录的长度，才能检测出分段 4 也包含一个 TLS 头部。这种情况下，
设备将失去与流的同步。)

Stream scan resynchronization
(流扫描重新同步)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the device gets out of sync and the stream reaches TCP sequence
numbers more than a max size record past the expected TCP sequence number,
the device starts scanning for a known header pattern. For example
for TLS 1.2 and TLS 1.3 subsequent bytes of value ``0x03 0x03`` occur
in the SSL/TLS version field of the header. Once pattern is matched
the device continues attempting parsing headers at expected locations
(based on the length fields at guessed locations).
Whenever the expected location does not contain a valid header the scan
is restarted.
(当设备失去同步，且流到达的 TCP 序列号超过期望 TCP 序列号一个最大尺寸
记录以上时，设备开始扫描已知的头部模式。例如对 TLS 1.2 和 TLS 1.3
而言，头部的 SSL/TLS 版本字段中会出现连续的字节值 ``0x03 0x03``。一旦
匹配到该模式，设备就继续尝试在预期位置解析头部 (基于猜测位置处的长度
字段)。每当预期位置不含有效头部时，就重新开始扫描。)

When the header is matched the device sends a confirmation request
to the kernel, asking if the guessed location is correct (if a TLS record
really starts there), and which record sequence number the given header had.
(当头部匹配成功时，设备向内核发送确认请求，询问猜测的位置是否正确
(该处是否真的开始了一个 TLS 记录)，以及该头部对应的记录序列号是多少。)

The asynchronous resync process is coordinated on the kernel side using
struct tls_offload_resync_async, which tracks and manages the resync request.
(异步重新同步过程在内核侧使用 struct tls_offload_resync_async 来协调，
它跟踪并管理重新同步请求。)

Helper functions to manage struct tls_offload_resync_async:
(用于管理 struct tls_offload_resync_async 的辅助函数：)

``tls_offload_rx_resync_async_request_start()``
Initializes an asynchronous resync attempt by specifying the sequence range to
monitor and resetting internal state in the struct.
(通过指定要监控的序列号范围并重置结构体中的内部状态，初始化一次异步
重新同步尝试。)

``tls_offload_rx_resync_async_request_end()``
Retains the device's guessed TCP sequence number for comparison with current or
future logged ones. It also clears the RESYNC_REQ_ASYNC flag from the resync
request, indicating that the device has submitted its guessed sequence number.
(保留设备猜测的 TCP 序列号，以便与当前或未来记录的序列号进行比较。
它还会清除重新同步请求中的 RESYNC_REQ_ASYNC 标志，表明设备已提交其
猜测的序列号。)

``tls_offload_rx_resync_async_request_cancel()``
Cancels any in-progress resync attempt, clearing the request state.
(取消任何进行中的重新同步尝试，清除请求状态。)

When the kernel processes an RX segment that begins a new TLS record, it
examines the current status of the asynchronous resynchronization request.
(当内核处理一个开始新 TLS 记录的 RX 分段时，它会检查异步重新同步请求的
当前状态。)

If the device is still waiting to provide its guessed TCP sequence number
(the async state), the kernel records the sequence number of this segment so
that it can later be compared once the device's guess becomes available.
(如果设备仍在等待提交其猜测的 TCP 序列号 (异步状态)，内核会记录该分段
的序列号，以便在设备的猜测变得可用之后再进行比较。)

If the device has already submitted its guessed sequence number (the non-async
state), the kernel now tries to match that guess against the sequence numbers of
all TLS record headers that have been logged since the resync request
started.
(如果设备已经提交了其猜测的序列号 (非异步状态)，内核此时会尝试把该
猜测与自重新同步请求开始以来记录的所有 TLS 记录头部的序列号进行匹配。)

The kernel confirms the guessed location was correct and tells the device
the record sequence number. Meanwhile, the device had been parsing
and counting all records since the just-confirmed one, it adds the number
of records it had seen to the record number provided by the kernel.
At this point the device is in sync and can resume decryption at next
segment boundary.
(内核确认猜测的位置正确，并告知设备记录序列号。与此同时，设备一直在
解析并计数自刚确认的记录以来的所有记录，它把自己看到的记录数加上内核
提供的记录号。此时设备已恢复同步，可以在下一个分段边界恢复解密。)

In a pathological case the device may latch onto a sequence of matching
headers and never hear back from the kernel (there is no negative
confirmation from the kernel). The implementation may choose to periodically
restart scan. Given how unlikely falsely-matching stream is, however,
periodic restart is not deemed necessary.
(在病态情况下，设备可能锁定在一系列匹配的头部上，且始终得不到内核的
回应 (内核不会给出否定确认)。实现可以选择周期性地重启扫描。不过，鉴于
流被错误匹配的可能性极低，周期性重启并不被认为是必要的。)

Special care has to be taken if the confirmation request is passed
asynchronously to the packet stream and record may get processed
by the kernel before the confirmation request.
(如果确认请求与数据包流异步传递，记录可能在确认请求之前就被内核处理，
这种情况需要特别注意。)

Stack-driven resynchronization
(由协议栈驱动的重新同步)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The driver may also request the stack to perform resynchronization
whenever it sees the records are no longer getting decrypted.
If the connection is configured in this mode the stack automatically
schedules resynchronization after it has received two completely encrypted
records.
(当驱动发现记录不再被解密时，也可以请求协议栈执行重新同步。如果连接
配置为此模式，协议栈在收到两个完全加密的记录后会自动安排重新同步。)

The stack waits for the socket to drain and informs the device about
the next expected record number and its TCP sequence number. If the
records continue to be received fully encrypted stack retries the
synchronization with an exponential back off (first after 2 encrypted
records, then after 4 records, after 8, after 16... up until every
128 records).
(协议栈等待套接字排空，然后把下一个期望的记录号及其 TCP 序列号告知
设备。如果记录继续以完全加密的形式到达，协议栈会以指数退避的方式重试
同步 (先是在 2 个加密记录之后，然后是 4 个、8 个、16 个……直到每
128 个记录)。)

Error handling
(错误处理)
==============

TX
(发送)
--

Packets may be redirected or rerouted by the stack to a different
device than the selected TLS offload device. The stack will handle
such condition using the :c:func:`sk_validate_xmit_skb` helper
(TLS offload code installs :c:func:`tls_validate_xmit_skb` at this hook).
Offload maintains information about all records until the data is
fully acknowledged, so if skbs reach the wrong device they can be handled
by software fallback.
(协议栈可能把数据包重定向或重新路由到与所选 TLS 卸载设备不同的设备。
协议栈会使用 :c:func:`sk_validate_xmit_skb` 辅助函数处理这种情况
(TLS 卸载代码在这个钩子上安装了 :c:func:`tls_validate_xmit_skb`)。
卸载会维护所有记录的信息直到数据被完全确认，因此如果 skb 到达了错误的
设备，它们可以由软件回退处理。)

Any device TLS offload handling error on the transmission side must result
in the packet being dropped. For example if a packet got out of order
due to a bug in the stack or the device, reached the device and can't
be encrypted such packet must be dropped.
(发送侧任何设备 TLS 卸载处理错误都必须导致数据包被丢弃。例如，如果由于
协议栈或设备的缺陷导致某个数据包乱序、到达设备且无法被加密，这样的
数据包必须被丢弃。)

RX
(接收)
--

If the device encounters any problems with TLS offload on the receive
side it should pass the packet to the host's networking stack as it was
received on the wire.
(如果设备在接收侧遇到任何 TLS 卸载问题，它应把数据包按线上接收到的
原样传给主机的网络协议栈。)

For example authentication failure for any record in the segment should
result in passing the unmodified packet to the software fallback. This means
packets should not be modified "in place". Splitting segments to handle partial
decryption is not advised. In other words either all records in the packet
had been handled successfully and authenticated or the packet has to be passed
to the host's stack as it was on the wire (recovering original packet in the
driver if device provides precise error is sufficient).
(例如，分段中任何一个记录认证失败都应导致把未修改的数据包传给软件回退。
这意味着数据包不应被"就地"修改。不建议通过拆分分段来处理部分解密。
换言之，要么包中的所有记录都被成功处理并通过认证，要么把数据包按线上
的原样传给主机的协议栈 (如果设备能提供精确的错误信息，由驱动恢复原始
数据包即可)。)

The Linux networking stack does not provide a way of reporting per-packet
decryption and authentication errors, packets with errors must simply not
have the :c:member:`decrypted` mark set.
(Linux 网络协议栈没有提供报告每包解密和认证错误的途径，有错误的数据包
只需不设置 :c:member:`decrypted` 标记即可。)

A packet should also not be handled by the TLS offload if it contains
incorrect checksums.
(如果数据包包含不正确的校验和，也不应由 TLS 卸载处理。)

Performance metrics
(性能指标)
===================

TLS offload can be characterized by the following basic metrics:
(TLS 卸载可以用以下基本指标来衡量：)

 * max connection count
   (最大连接数)
 * connection installation rate
   (连接建立速率)
 * connection installation latency
   (连接建立延迟)
 * total cryptographic performance
   (总加密性能)

Note that each TCP connection requires a TLS session in both directions,
the performance may be reported treating each direction separately.
(注意，每条 TCP 连接都需要两个方向各一个 TLS 会话，性能可以按每个方向
分别报告。)

Max connection count
(最大连接数)
--------------------

The number of connections device can support can be exposed via
``devlink resource`` API.
(设备可支持的连接数可以通过 ``devlink resource`` API 暴露。)

Total cryptographic performance
(总加密性能)
-------------------------------

Offload performance may depend on segment and record size.
(卸载性能可能取决于分段和记录的大小。)

Overload of the cryptographic subsystem of the device should not have
significant performance impact on non-offloaded streams.
(设备加密子系统的过载不应对未卸载的流产生显著的性能影响。)

Statistics
(统计信息)
==========

Following minimum set of TLS-related statistics should be reported
by the driver:
(驱动应报告以下最小集合的 TLS 相关统计信息：)

 * ``rx_tls_decrypted_packets`` - number of successfully decrypted RX packets
   which were part of a TLS stream.
   (成功解密且属于某个 TLS 流的 RX 数据包数量。)
 * ``rx_tls_decrypted_bytes`` - number of TLS payload bytes in RX packets
   which were successfully decrypted.
   (RX 数据包中被成功解密的 TLS 载荷字节数。)
 * ``rx_tls_ctx`` - number of TLS RX HW offload contexts added to device for
   decryption.
   (为解密而添加到设备的 TLS RX 硬件卸载上下文数量。)
 * ``rx_tls_del`` - number of TLS RX HW offload contexts deleted from device
   (connection has finished).
   (从设备删除的 TLS RX 硬件卸载上下文数量 (连接已结束)。)
 * ``rx_tls_resync_req_pkt`` - number of received TLS packets with a resync
    request.
    (带有重新同步请求的已接收 TLS 数据包数量。)
 * ``rx_tls_resync_req_start`` - number of times the TLS async resync request
    was started.
    (TLS 异步重新同步请求启动的次数。)
 * ``rx_tls_resync_req_end`` - number of times the TLS async resync request
    properly ended with providing the HW tracked tcp-seq.
    (TLS 异步重新同步请求正常结束 (提供了硬件跟踪的 tcp-seq) 的次数。)
 * ``rx_tls_resync_req_skip`` - number of times the TLS async resync request
    procedure was started but not properly ended.
    (TLS 异步重新同步请求过程启动但未正常结束的次数。)
 * ``rx_tls_resync_res_ok`` - number of times the TLS resync response call to
    the driver was successfully handled.
    (对驱动的 TLS 重新同步响应调用被成功处理的次数。)
 * ``rx_tls_resync_res_skip`` - number of times the TLS resync response call to
    the driver was terminated unsuccessfully.
    (对驱动的 TLS 重新同步响应调用被非正常终止的次数。)
 * ``rx_tls_err`` - number of RX packets which were part of a TLS stream
   but were not decrypted due to unexpected error in the state machine.
   (属于某个 TLS 流但由于状态机意外错误而未被解密的 RX 数据包数量。)
 * ``tx_tls_encrypted_packets`` - number of TX packets passed to the device
   for encryption of their TLS payload.
   (为加密其 TLS 载荷而传给设备的 TX 数据包数量。)
 * ``tx_tls_encrypted_bytes`` - number of TLS payload bytes in TX packets
   passed to the device for encryption.
   (为加密而传给设备的 TX 数据包中的 TLS 载荷字节数。)
 * ``tx_tls_ctx`` - number of TLS TX HW offload contexts added to device for
   encryption.
   (为加密而添加到设备的 TLS TX 硬件卸载上下文数量。)
 * ``tx_tls_ooo`` - number of TX packets which were part of a TLS stream
   but did not arrive in the expected order.
   (属于某个 TLS 流但未按期望顺序到达的 TX 数据包数量。)
 * ``tx_tls_skip_no_sync_data`` - number of TX packets which were part of
   a TLS stream and arrived out-of-order, but skipped the HW offload routine
   and went to the regular transmit flow as they were retransmissions of the
   connection handshake.
   (属于某个 TLS 流且乱序到达、但跳过了硬件卸载例程而进入常规发送流程的
   TX 数据包数量，因为它们是连接握手的重传。)
 * ``tx_tls_drop_no_sync_data`` - number of TX packets which were part of
   a TLS stream dropped, because they arrived out of order and associated
   record could not be found.
   (属于某个 TLS 流且因乱序到达、找不到关联记录而被丢弃的 TX 数据包
   数量。)
 * ``tx_tls_drop_bypass_req`` - number of TX packets which were part of a TLS
   stream dropped, because they contain both data that has been encrypted by
   software and data that expects hardware crypto offload.
   (属于某个 TLS 流且因同时包含已由软件加密的数据和期望硬件加密卸载的
   数据而被丢弃的 TX 数据包数量。)

Notable corner cases, exceptions and additional requirements
(值得注意的边界情况、异常与附加要求)
============================================================

.. _5tuple_problems:

5-tuple matching limitations
(五元组匹配的局限)
----------------------------

The device can only recognize received packets based on the 5-tuple
of the socket. Current ``ktls`` implementation will not offload sockets
routed through software interfaces such as those used for tunneling
or virtual networking. However, many packet transformations performed
by the networking stack (most notably any BPF logic) do not require
any intermediate software device, therefore a 5-tuple match may
consistently miss at the device level. In such cases the device
should still be able to perform TX offload (encryption) and should
fallback cleanly to software decryption (RX).
(设备只能基于套接字的五元组来识别收到的数据包。当前的 ``ktls`` 实现
不会卸载经过软件接口路由的套接字，例如用于隧道或虚拟网络的接口。
然而，网络协议栈执行的许多数据包变换 (最典型的是任何 BPF 逻辑) 并不
需要任何中间软件设备，因此五元组匹配可能在设备层持续失效。在这种
情况下，设备仍应能够执行 TX 卸载 (加密)，并应干净地回退到软件解密
(RX)。)

Out of order
(乱序)
------------

Introducing extra processing in NICs should not cause packets to be
transmitted or received out of order, for example pure ACK packets
should not be reordered with respect to data segments.
(在网卡中引入额外处理不应导致数据包的发送或接收乱序，例如纯 ACK 数据包
不应相对于数据分段被重排。)

Ingress reorder
(入方向重排)
---------------

A device is permitted to perform packet reordering for consecutive
TCP segments (i.e. placing packets in the correct order) but any form
of additional buffering is disallowed.
(允许设备对连续的 TCP 分段执行数据包重排 (即把数据包放到正确的顺序)，
但不允许任何形式的额外缓冲。)

Coexistence with standard networking offload features
(与标准网络卸载特性共存)
-----------------------------------------------------

Offloaded ``ktls`` sockets should support standard TCP stack features
transparently. Enabling device TLS offload should not cause any difference
in packets as seen on the wire.
(已卸载的 ``ktls`` 套接字应透明地支持标准 TCP 协议栈特性。启用设备 TLS
卸载不应导致线上所见数据包的任何差异。)

Transport layer transparency
(传输层透明性)
----------------------------

For the purpose of simplifying TLS offload, the device should not modify any
packet headers.
(为了简化 TLS 卸载，设备不应修改任何数据包头部。)

The device should not depend on any packet headers beyond what is strictly
necessary for TLS offload.
(设备不应依赖于 TLS 卸载严格必需范围之外的任何数据包头部。)

Segment drops
(分段丢弃)
-------------

Dropping packets is acceptable only in the event of catastrophic
system errors and should never be used as an error handling mechanism
in cases arising from normal operation. In other words, reliance
on TCP retransmissions to handle corner cases is not acceptable.
(只有在发生灾难性系统错误时才允许丢弃数据包，绝不应把丢包用作正常运作
中出现问题的错误处理机制。换言之，依靠 TCP 重传来处理边界情况是不可
接受的。)

TLS device features
(TLS 设备特性)
-------------------

Drivers should ignore the changes to the TLS device feature flags.
These flags will be acted upon accordingly by the core ``ktls`` code.
TLS device feature flags only control adding of new TLS connection
offloads, old connections will remain active after flags are cleared.
(驱动应忽略 TLS 设备特性标志的变化。核心 ``ktls`` 代码会相应地处理这些
标志。TLS 设备特性标志只控制添加新的 TLS 连接卸载，标志被清除后旧连接
仍保持活动。)

TLS encryption cannot be offloaded to devices without checksum calculation
offload. Hence, TLS TX device feature flag requires TX csum offload being set.
Disabling the latter implies clearing the former. Disabling TX checksum offload
should not affect old connections, and drivers should make sure checksum
calculation does not break for them.
Similarly, device-offloaded TLS decryption implies doing RXCSUM. If the user
does not want to enable RX csum offload, TLS RX device feature is disabled
as well.
(TLS 加密无法卸载到不具备校验和计算卸载能力的设备上。因此，TLS TX 设备
特性标志要求已设置 TX csum 卸载。禁用后者意味着清除前者。禁用 TX 校验
和卸载不应影响旧连接，驱动应确保对旧连接而言校验和计算不会失效。类似
地，设备卸载的 TLS 解密意味着要做 RXCSUM。如果用户不想启用 RX csum
卸载，TLS RX 设备特性也会被禁用。)
