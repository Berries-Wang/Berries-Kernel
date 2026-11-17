.. SPDX-License-Identifier: GPL-2.0

========================================================
TCP Authentication Option Linux implementation (RFC5925)
========================================================

TCP Authentication Option (TCP-AO) provides a TCP extension aimed at verifying
segments between trusted peers. It adds a new TCP header option with
a Message Authentication Code (MAC). MACs are produced from the content
of a TCP segment using a hashing function with a password known to both peers.
The intent of TCP-AO is to deprecate TCP-MD5 providing better security,
key rotation and support for a variety of hashing algorithms.
(TCP 认证选项 (TCP-AO) 提供了一个 TCP 扩展，旨在验证可信对等端之间的报文段。
它添加了一个带消息认证码 (MAC) 的新 TCP 头选项。MAC 由哈希函数根据 TCP 报文段
的内容生成，所用的口令为通信双方所共知。TCP-AO 的意图是取代 TCP-MD5，提供更好
的安全性、密钥轮换以及对多种哈希算法的支持。)

1. Introduction(1. 简介)
========================

.. table:: Short and Limited Comparison of TCP-AO and TCP-MD5

 +----------------------+------------------------+-----------------------+
 |                      |TCP-MD5                 |TCP-AO                 |
 +======================+========================+=======================+
 |Supported hashing     |MD5                     |Must support HMAC-SHA1 |
 |algorithms            |(cryptographically weak)|(chosen-prefix attacks)|
 |                      |                        |and CMAC-AES-128 (only |
 |                      |                        |side-channel attacks). |
 |                      |                        |May support any hashing|
 |                      |                        |algorithm.             |
 |(支持的哈希算法)      |(MD5，加密学上薄弱)     |(必须支持 HMAC-SHA1（易|
 |                      |                        |受前缀选择攻击），以及 |
 |                      |                        |CMAC-AES-128（仅受侧信 |
 |                      |                        |道攻击）。可以支持任何 |
 |                      |                        |哈希算法。)            |
 +----------------------+------------------------+-----------------------+
 |Length of MACs (bytes)|16                      |Typically 12-16.       |
 |                      |                        |Other variants that fit|
 |                      |                        |TCP header permitted.  |
 |(MAC 长度（字节）)    |                        |(通常为 12-16。允许其他|
 |                      |                        |能放入 TCP 头的变体。) |
 +----------------------+------------------------+-----------------------+
 |Number of keys per    |1                       |Many                   |
 |TCP connection        |                        |                       |
 |(每条 TCP 连接的密钥数|                        |(许多)                 |
 |量)                   |                        |                       |
 +----------------------+------------------------+-----------------------+
 |Possibility to change |Non-practical (both     |Supported by protocol  |
 |an active key         |peers have to change    |                       |
 |                      |them during MSL)        |                       |
 |(能否更换正在使用的密 |(不切实际（通信双方必须 |(协议支持)             |
 |钥)                   |在 MSL 期间同时更换）)  |                       |
 +----------------------+------------------------+-----------------------+
 |Protection against    |No                      |Yes: ignoring them     |
 |ICMP 'hard errors'    |                        |by default on          |
 |                      |                        |established connections|
 |(针对                 |(无)                    |(有：在已建立的连接上默|
 |ICMP 'hard errors'（  |                        |认忽略它们)            |
 |硬错误）的防护)       |                        |                       |
 +----------------------+------------------------+-----------------------+
 |Protection against    |No                      |Yes: pseudo-header     |
 |traffic-crossing      |                        |includes TCP ports.    |
 |attack                |                        |                       |
 |(针对跨流量攻击的防护)|(无)                    |(有：伪首部包含 TCP 端 |
 |                      |                        |口。)                  |
 +----------------------+------------------------+-----------------------+
 |Protection against    |No                      |Sequence Number        |
 |replayed TCP segments |                        |Extension (SNE) and    |
 |                      |                        |Initial Sequence       |
 |                      |                        |Numbers (ISNs)         |
 |(针对重放 TCP 报文段的|(无)                    |(序列号扩展 (SNE) 与初 |
 |防护)                 |                        |始序列号 (ISN))        |
 +----------------------+------------------------+-----------------------+
 |Supports              |Yes                     |No. ISNs+SNE are needed|
 |Connectionless Resets |                        |to correctly sign RST. |
 |(支持无连接复位)      |(是)                    |(否。要正确签署 RST 需 |
 |                      |                        |要 ISN+SNE。)          |
 +----------------------+------------------------+-----------------------+
 |Standards             |RFC 2385                |RFC 5925, RFC 5926     |
 |(标准)                |                        |                       |
 +----------------------+------------------------+-----------------------+


1.1 Frequently Asked Questions (FAQ) with references to RFC 5925(1.1 常见问题解答 (FAQ) 及 RFC 5925 相关引文)
-------------------------------------------------------------------------------------------------------------

Q: Can either SendID or RecvID be non-unique for the same 4-tuple
(srcaddr, srcport, dstaddr, dstport)?
(问：对于同一个四元组 (srcaddr, srcport, dstaddr, dstport)，SendID 或 RecvID
可以不唯一吗？)

A: No [3.1]::
(答：否 [3.1]：)

   >> The IDs of MKTs MUST NOT overlap where their TCP connection
   identifiers overlap.

Q: Can Master Key Tuple (MKT) for an active connection be removed?
(问：处于活动状态的连接的 Master Key Tuple (MKT) 可以被移除吗？)

A: No, unless it's copied to Transport Control Block (TCB) [3.1]::
(答：否，除非它已被复制到传输控制块 (TCB) 中 [3.1]：)

   It is presumed that an MKT affecting a particular connection cannot
   be destroyed during an active connection -- or, equivalently, that
   its parameters are copied to an area local to the connection (i.e.,
   instantiated) and so changes would affect only new connections.

Q: If an old MKT needs to be deleted, how should it be done in order
to not remove it for an active connection? (As it can be still in use
at any moment later)
(问：如果需要删除一个旧的 MKT，应该怎么做才能不从处于活动状态的连接上移除
它？（因为它在此后的任何时刻都可能仍在使用中）)

A: Not specified by RFC 5925, seems to be a problem for key management
to ensure that no one uses such MKT before trying to remove it.
(答：RFC 5925 未作规定。在尝试移除之前确保没有人在使用这样的 MKT，这似乎是
密钥管理需要解决的问题。)

Q: Can an old MKT exist forever and be used by another peer?
(问：旧的 MKT 可以永远存在并被另一个对等端使用吗？)

A: It can, it's a key management task to decide when to remove an old key [6.1]::
(答：可以。何时移除旧密钥属于密钥管理的任务 [6.1]：)

   Deciding when to start using a key is a performance issue. Deciding
   when to remove an MKT is a security issue. Invalid MKTs are expected
   to be removed. TCP-AO provides no mechanism to coordinate their removal,
   as we consider this a key management operation.

also [6.1]::
(另见 [6.1]：)

   The only way to avoid reuse of previously used MKTs is to remove the MKT
   when it is no longer considered permitted.

Linux TCP-AO will try its best to prevent you from removing a key that's
being used, considering it a key management failure. But since keeping
an outdated key may become a security issue and as a peer may
unintentionally prevent the removal of an old key by always setting
it as RNextKeyID - a forced key removal mechanism is provided, where
userspace has to supply KeyID to use instead of the one that's being removed
and the kernel will atomically delete the old key, even if the peer is
still requesting it. There are no guarantees for force-delete as the peer
may yet not have the new key - the TCP connection may just break.
Alternatively, one may choose to shut down the socket.
(Linux TCP-AO 会尽力阻止你移除正在使用的密钥，并把这视为密钥管理上的失败。
但由于保留过期的密钥可能成为安全问题，而且对等端可能会通过总是把它设置为
RNextKeyID 而无意中阻止旧密钥的移除，因此提供了一种强制移除密钥的机制：用户
空间必须提供用来替代被移除密钥的 KeyID，内核会原子地删除旧密钥，即使对等端
仍在请求它。强制删除没有任何保证，因为对等端可能还没有新密钥——TCP 连接可能
就此中断。另一种选择是直接关闭套接字。)

Q: What happens when a packet is received on a new connection with no known
MKT's RecvID?
(问：当新连接上收到一个数据包、而没有任何已知 MKT 的 RecvID 与之匹配时，
会发生什么？)

A: RFC 5925 specifies that by default it is accepted with a warning logged, but
the behaviour can be configured by the user [7.5.1.a]::
(答：RFC 5925 规定默认接受并记录一条警告日志，但该行为可由用户配置
[7.5.1.a]：)

   If the segment is a SYN, then this is the first segment of a new
   connection. Find the matching MKT for this segment, using the segment's
   socket pair and its TCP-AO KeyID, matched against the MKT's TCP connection
   identifier and the MKT's RecvID.

      i. If there is no matching MKT, remove TCP-AO from the segment.
         Proceed with further TCP handling of the segment.
         NOTE: this presumes that connections that do not match any MKT
         should be silently accepted, as noted in Section 7.3.

[7.3]::
(另见 [7.3]：)

   >> A TCP-AO implementation MUST allow for configuration of the behavior
   of segments with TCP-AO but that do not match an MKT. The initial default
   of this configuration SHOULD be to silently accept such connections.
   If this is not the desired case, an MKT can be included to match such
   connections, or the connection can indicate that TCP-AO is required.
   Alternately, the configuration can be changed to discard segments with
   the AO option not matching an MKT.

[10.2.b]::
(另见 [10.2.b]：)

   Connections not matching any MKT do not require TCP-AO. Further, incoming
   segments with TCP-AO are not discarded solely because they include
   the option, provided they do not match any MKT.

Note that Linux TCP-AO implementation differs in this aspect. Currently, TCP-AO
segments with unknown key signatures are discarded with warnings logged.
(注意，Linux TCP-AO 实现在这方面有所不同。目前，带未知密钥签名的 TCP-AO 报文段
会被丢弃并记录警告日志。)

Q: Does the RFC imply centralized kernel key management in any way?
(i.e. that a key on all connections MUST be rotated at the same time?)
(问：RFC 是否以任何方式暗示了集中式的内核密钥管理，即所有连接上的密钥必须
同时轮换？)

A: Not specified. MKTs can be managed in userspace, the only relevant part to
key changes is [7.3]::
(答：未作规定。MKT 可以在用户空间中管理，与密钥变更相关的唯一部分是 [7.3]：)

   >> All TCP segments MUST be checked against the set of MKTs for matching
   TCP connection identifiers.

Q: What happens when RNextKeyID requested by a peer is unknown? Should
the connection be reset?
(问：当对等端请求的 RNextKeyID 未知时会发生什么？应该重置连接吗？)

A: It should not, no action needs to be performed [7.5.2.e]::
(答：不应该，无需执行任何操作 [7.5.2.e]：)

   ii. If they differ, determine whether the RNextKeyID MKT is ready.

       1. If the MKT corresponding to the segment’s socket pair and RNextKeyID
       is not available, no action is required (RNextKeyID of a received
       segment needs to match the MKT’s SendID).

Q: How is current_key set, and when does it change? Is it a user-triggered
change, or is it triggered by a request from the remote peer? Is it set by the
user explicitly, or by a matching rule?
(问：current_key 是如何设置、何时改变的？它是用户触发的变更，还是由远端对等端
的请求触发？它是被用户显式设置，还是由匹配规则设置？)

A: current_key is set by RNextKeyID [6.1]::
(答：current_key 由 RNextKeyID 设置 [6.1]：)

   Rnext_key is changed only by manual user intervention or MKT management
   protocol operation. It is not manipulated by TCP-AO. Current_key is updated
   by TCP-AO when processing received TCP segments as discussed in the segment
   processing description in Section 7.5. Note that the algorithm allows
   the current_key to change to a new MKT, then change back to a previously
   used MKT (known as "backing up"). This can occur during an MKT change when
   segments are received out of order, and is considered a feature of TCP-AO,
   because reordering does not result in drops.

[7.5.2.e.ii]::
(另见 [7.5.2.e.ii]：)

   2. If the matching MKT corresponding to the segment’s socket pair and
   RNextKeyID is available:

      a. Set current_key to the RNextKeyID MKT.

Q: If both peers have multiple MKTs matching the connection's socket pair
(with different KeyIDs), how should the sender/receiver pick KeyID to use?
(问：如果通信双方都有多个匹配该连接套接字对的 MKT（KeyID 各不相同），发送方/
接收方应如何挑选要使用的 KeyID？)

A: Some mechanism should pick the "desired" MKT [3.3]::
(答：应当由某种机制挑选"期望的" MKT [3.3]：)

   Multiple MKTs may match a single outgoing segment, e.g., when MKTs
   are being changed. Those MKTs cannot have conflicting IDs (as noted
   elsewhere), and some mechanism must determine which MKT to use for each
   given outgoing segment.

   >> An outgoing TCP segment MUST match at most one desired MKT, indicated
   by the segment’s socket pair. The segment MAY match multiple MKTs, provided
   that exactly one MKT is indicated as desired. Other information in
   the segment MAY be used to determine the desired MKT when multiple MKTs
   match; such information MUST NOT include values in any TCP option fields.

Q: Can TCP-MD5 connection migrate to TCP-AO (and vice-versa):
(问：TCP-MD5 连接可以迁移到 TCP-AO 吗（反之亦然）：)

A: No [1]::
(答：否 [1]：)

   TCP MD5-protected connections cannot be migrated to TCP-AO because TCP MD5
   does not support any changes to a connection’s security algorithm
   once established.

Q: If all MKTs are removed on a connection, can it become a non-TCP-AO signed
connection?
(问：如果一条连接上的所有 MKT 都被移除，它能变成非 TCP-AO 签名的连接吗？)

A: [7.5.2] doesn't have the same choice as SYN packet handling in [7.5.1.i]
that would allow accepting segments without a sign (which would be insecure).
While switching to non-TCP-AO connection is not prohibited directly, it seems
what the RFC means. Also, there's a requirement for TCP-AO connections to
always have one current_key [3.3]::
(答：[7.5.2] 不像 [7.5.1.i] 中对 SYN 数据包的处理那样，提供了接受无签名报文段
的选项（那将是不安全的）。虽然 RFC 没有直接禁止切换到非 TCP-AO 连接，但这似乎
就是它的本意。此外，TCP-AO 连接还要求始终有一个 current_key [3.3]：)

   TCP-AO requires that every protected TCP segment match exactly one MKT.

[3.3]::
(另见 [3.3]：)

   >> An incoming TCP segment including TCP-AO MUST match exactly one MKT,
   indicated solely by the segment’s socket pair and its TCP-AO KeyID.

[4.4]::
(另见 [4.4]：)

   One or more MKTs. These are the MKTs that match this connection’s
   socket pair.

Q: Can a non-TCP-AO connection become a TCP-AO-enabled one?
(问：非 TCP-AO 连接可以变成启用 TCP-AO 的连接吗？)

A: No: for an already established non-TCP-AO connection it would be impossible
to switch to using TCP-AO, as the traffic key generation requires the initial
sequence numbers. Paraphrasing, starting using TCP-AO would require
re-establishing the TCP connection.
(答：否：对于已经建立的非 TCP-AO 连接，无法切换为使用 TCP-AO，因为流量密钥的
生成需要初始序列号。换句话说，开始使用 TCP-AO 需要重新建立 TCP 连接。)

2. In-kernel MKTs database vs database in userspace(2. 内核内 MKT 数据库与用户空间数据库的对比)
===============================================================================================

Linux TCP-AO support is implemented using ``setsockopt()s``, in a similar way
to TCP-MD5. It means that a userspace application that wants to use TCP-AO
should perform ``setsockopt()`` on a TCP socket when it wants to add,
remove or rotate MKTs. This approach moves the key management responsibility
to userspace as well as decisions on corner cases, i.e. what to do if
the peer doesn't respect RNextKeyID; moving more code to userspace, especially
responsible for the policy decisions. Besides, it's flexible and scales well
(with less locking needed than in the case of an in-kernel database). One also
should keep in mind that mainly intended users are BGP processes, not any
random applications, which means that compared to IPsec tunnels,
no transparency is really needed and modern BGP daemons already have
``setsockopt()s`` for TCP-MD5 support.
(Linux TCP-AO 支持以与 TCP-MD5 类似的方式，通过 ``setsockopt()s`` 实现。这意味着
想使用 TCP-AO 的用户空间应用，应当在需要添加、移除或轮换 MKT 时对 TCP 套接字
执行 ``setsockopt()``。这种做法把密钥管理的责任以及各种边角情况的决定（例如
对等端不遵守 RNextKeyID 时该怎么办）都移交给用户空间，把更多代码（尤其是负责
策略决定的代码）移入用户空间。此外，它灵活且伸缩性良好（与内核内数据库相比
所需的加锁更少）。还应记住，其主要目标用户是 BGP 进程而非任意应用，这意味着
与 IPsec 隧道相比，并不真正需要透明性，而且现代 BGP 守护进程已经拥有了支持
TCP-MD5 的 ``setsockopt()s``。)

.. table:: Considered pros and cons of the approaches

 +----------------------+------------------------+-----------------------+
 |                      |``setsockopt()``        |in-kernel DB           |
 +======================+========================+=======================+
 |Extendability         |``setsockopt()``        |Netlink messages are   |
 |                      |commands should be      |simple and extendable  |
 |                      |extendable syscalls     |                       |
 |(可扩展性)            |(``setsockopt()`` 命令应|(Netlink 消息简单且可扩|
 |                      |当是可扩展的系统调用)   |展)                    |
 +----------------------+------------------------+-----------------------+
 |Required userspace    |BGP or any application  |could be transparent   |
 |changes               |that wants TCP-AO needs |as tunnels, providing  |
 |                      |to perform              |something like         |
 |                      |``setsockopt()s``       |``ip tcpao add key``   |
 |                      |and do key management   |(delete/show/rotate)   |
 |(所需的用户空间改动)  |(BGP 或任何想使用 TCP-AO|(可以像隧道一样透明，提|
 |                      |的应用都需要执行        |供类似                 |
 |                      |``setsockopt()s`` 并自  |``ip tcpao add key`` ( |
 |                      |行管理密钥)             |删除/显示/轮换) 之类的 |
 |                      |                        |功能)                  |
 +----------------------+------------------------+-----------------------+
 |MKTs removal or adding|harder for userspace    |harder for kernel      |
 |(MKT 的删除或添加)    |(对用户空间更难)        |(对内核更难)           |
 +----------------------+------------------------+-----------------------+
 |Dump-ability          |``getsockopt()``        |Netlink .dump()        |
 |                      |                        |callback               |
 |(可转储性)            |                        |(Netlink .dump() 回调) |
 +----------------------+------------------------+-----------------------+
 |Limits on kernel      |equal                                           |
 |resources/memory      |                                                |
 |(内核资源/内存的限制) |(相等)                                          |
 +----------------------+------------------------+-----------------------+
 |Scalability           |contention on           |contention on          |
 |                      |``TCP_LISTEN`` sockets  |the whole database     |
 |(伸缩性)              |(``TCP_LISTEN`` 套接字上|(整个数据库上的竞争)   |
 |                      |的竞争)                 |                       |
 +----------------------+------------------------+-----------------------+
 |Monitoring & warnings |``TCP_DIAG``            |same Netlink socket    |
 |(监控与告警)          |                        |(相同的 Netlink 套接字)|
 +----------------------+------------------------+-----------------------+
 |Matching of MKTs      |half-problem: only      |hard                   |
 |                      |listen sockets          |                       |
 |(MKT 匹配)            |(半个问题：仅监听套接字)|(困难)                 |
 +----------------------+------------------------+-----------------------+


3. uAPI(3. uAPI)
================

Linux provides a set of ``setsockopt()s`` and ``getsockopt()s`` that let
userspace manage TCP-AO on a per-socket basis. In order to add/delete MKTs
``TCP_AO_ADD_KEY`` and ``TCP_AO_DEL_KEY`` TCP socket options must be used.
It is not allowed to add a key on an established non-TCP-AO connection
as well as to remove the last key from TCP-AO connection.
(Linux 提供了一组 ``setsockopt()s`` 和 ``getsockopt()s``，让用户空间能够按套接字
管理 TCP-AO。要添加/删除 MKT，必须使用 ``TCP_AO_ADD_KEY`` 和 ``TCP_AO_DEL_KEY``
这两个 TCP 套接字选项。不允许在已建立的非 TCP-AO 连接上添加密钥，也不允许从
TCP-AO 连接上移除最后一个密钥。)

``setsockopt(TCP_AO_DEL_KEY)`` command may specify ``tcp_ao_del::current_key``
+ ``tcp_ao_del::set_current`` and/or ``tcp_ao_del::rnext``
+ ``tcp_ao_del::set_rnext`` which makes such delete "forced": it
provides userspace a way to delete a key that's being used and atomically set
another one instead. This is not intended for normal use and should be used
only when the peer ignores RNextKeyID and keeps requesting/using an old key.
It provides a way to force-delete a key that's not trusted but may break
the TCP-AO connection.
(``setsockopt(TCP_AO_DEL_KEY)`` 命令可以指定 ``tcp_ao_del::current_key`` +
``tcp_ao_del::set_current`` 和/或 ``tcp_ao_del::rnext`` +
``tcp_ao_del::set_rnext``，这使得该删除成为"强制"的：它为用户空间提供了一种
删除正在使用的密钥并原子地设置另一个密钥来替代的方法。这并非为常规使用而设，
仅应在对等端忽略 RNextKeyID 并持续请求/使用旧密钥时使用。它提供了一种强制删除
不受信任密钥的途径，但可能会破坏 TCP-AO 连接。)

The usual/normal key-rotation can be performed with ``setsockopt(TCP_AO_INFO)``.
It also provides a uAPI to change per-socket TCP-AO settings, such as
ignoring ICMPs, as well as clear per-socket TCP-AO packet counters.
The corresponding ``getsockopt(TCP_AO_INFO)`` can be used to get those
per-socket TCP-AO settings.
(常规/正常的密钥轮换可以通过 ``setsockopt(TCP_AO_INFO)`` 完成。它还提供了更改
每套接字 TCP-AO 设置（例如忽略 ICMP）的 uAPI，以及清除每套接字 TCP-AO 数据包
计数器。对应的 ``getsockopt(TCP_AO_INFO)`` 可用于获取这些每套接字的 TCP-AO
设置。)

Another useful command is ``getsockopt(TCP_AO_GET_KEYS)``. One can use it
to list all MKTs on a TCP socket or use a filter to get keys for a specific
peer and/or sndid/rcvid, VRF L3 interface or get current_key/rnext_key.
(另一个有用的命令是 ``getsockopt(TCP_AO_GET_KEYS)``。可以用它列出一个 TCP 套接字
上的所有 MKT，或使用过滤器获取特定对等端和/或 sndid/rcvid、VRF L3 接口的密钥，
或获取 current_key/rnext_key。)

To repair TCP-AO connections ``setsockopt(TCP_AO_REPAIR)`` is available,
provided that the user previously has checkpointed/dumped the socket with
``getsockopt(TCP_AO_REPAIR)``.
(要修复 TCP-AO 连接，可以使用 ``setsockopt(TCP_AO_REPAIR)``，前提是用户之前已用
``getsockopt(TCP_AO_REPAIR)`` 对该套接字做过检查点/转储。)

A tip here for scaled TCP_LISTEN sockets, that may have some thousands TCP-AO
keys, is: use filters in ``getsockopt(TCP_AO_GET_KEYS)`` and asynchronous
delete with ``setsockopt(TCP_AO_DEL_KEY)``.
(对于可能拥有数千个 TCP-AO 密钥的大规模 TCP_LISTEN 套接字，这里有个技巧：在
``getsockopt(TCP_AO_GET_KEYS)`` 中使用过滤器，并用 ``setsockopt(TCP_AO_DEL_KEY)``
进行异步删除。)

Linux TCP-AO also provides a bunch of segment counters that can be helpful
with troubleshooting/debugging issues. Every MKT has good/bad counters
that reflect how many packets passed/failed verification.
Each TCP-AO socket has the following counters:
(Linux TCP-AO 还提供了一组报文段计数器，有助于排障/调试问题。每个 MKT 都有
好/坏计数器，反映有多少数据包通过/未通过验证。每个 TCP-AO 套接字都有以下
计数器：)

- for good segments (properly signed)
  (针对完好报文段（签名正确）的计数器)
- for bad segments (failed TCP-AO verification)
  (针对坏报文段（未通过 TCP-AO 验证）的计数器)
- for segments with unknown keys
  (针对密钥未知的报文段的计数器)
- for segments where an AO signature was expected, but wasn't found
  (针对期望有 AO 签名却未找到的报文段的计数器)
- for the number of ignored ICMPs
  (针对被忽略的 ICMP 数量的计数器)

TCP-AO per-socket counters are also duplicated with per-netns counters,
exposed with SNMP. Those are ``TCPAOGood``, ``TCPAOBad``, ``TCPAOKeyNotFound``,
``TCPAORequired`` and ``TCPAODroppedIcmps``.
(TCP-AO 的每套接字计数器还以每 netns 计数器的形式重复一份，通过 SNMP 暴露。
它们是 ``TCPAOGood``、``TCPAOBad``、``TCPAOKeyNotFound``、``TCPAORequired`` 和
``TCPAODroppedIcmps``。)

For monitoring purposes, there are following TCP-AO trace events:
``tcp_hash_bad_header``, ``tcp_hash_ao_required``, ``tcp_ao_handshake_failure``,
``tcp_ao_wrong_maclen``, ``tcp_ao_wrong_maclen``, ``tcp_ao_key_not_found``,
``tcp_ao_rnext_request``, ``tcp_ao_synack_no_key``, ``tcp_ao_snd_sne_update``,
``tcp_ao_rcv_sne_update``. It's possible to separately enable any of them and
one can filter them by net-namespace, 4-tuple, family, L3 index, and TCP header
flags. If a segment has a TCP-AO header, the filters may also include
keyid, rnext, and maclen. SNE updates include the rolled-over numbers.
(用于监控目的，有以下 TCP-AO trace 事件：``tcp_hash_bad_header``、
``tcp_hash_ao_required``、``tcp_ao_handshake_failure``、``tcp_ao_wrong_maclen``、
``tcp_ao_wrong_maclen``、``tcp_ao_key_not_found``、``tcp_ao_rnext_request``、
``tcp_ao_synack_no_key``、``tcp_ao_snd_sne_update``、
``tcp_ao_rcv_sne_update``。可以单独启用其中任何一个，并且可以按网络命名空间、
四元组、协议族、L3 索引和 TCP 头标志进行过滤。如果报文段带有 TCP-AO 头，
过滤器还可以包括 keyid、rnext 和 maclen。SNE 更新包含回绕后的数字。)

RFC 5925 very permissively specifies how TCP port matching can be done for
MKTs::
(RFC 5925 对 MKT 的 TCP 端口匹配方式的规定非常宽松：)

   TCP connection identifier. A TCP socket pair, i.e., a local IP
   address, a remote IP address, a TCP local port, and a TCP remote port.
   Values can be partially specified using ranges (e.g., 2-30), masks
   (e.g., 0xF0), wildcards (e.g., "*"), or any other suitable indication.

Currently Linux TCP-AO implementation doesn't provide any TCP port matching.
Probably, port ranges are the most flexible for uAPI, but so far
not implemented.
(目前的 Linux TCP-AO 实现不提供任何 TCP 端口匹配。就 uAPI 而言，端口范围可能
是最灵活的方式，但截至目前尚未实现。)

4. ``setsockopt()`` vs ``accept()`` race(4. ``setsockopt()`` 与 ``accept()`` 的竞态)
====================================================================================

In contrast with an established TCP-MD5 connection which has just one key,
TCP-AO connections may have many keys, which means that accepted connections
on a listen socket may have any amount of keys as well. As copying all those
keys on a first properly signed SYN would make the request socket bigger, that
would be undesirable. Currently, the implementation doesn't copy keys
to request sockets, but rather look them up on the "parent" listener socket.
(与只有单个密钥的已建立 TCP-MD5 连接不同，TCP-AO 连接可以拥有许多密钥，这意味
着监听套接字上接受的连接同样可以拥有任意数量的密钥。由于在第一个签名正确的 SYN
上复制所有这些密钥会使 request socket（请求套接字）变大，这是不可取的。目前的
实现不把密钥复制到 request socket，而是在"父"监听套接字上查找它们。)

The result is that when userspace removes TCP-AO keys, that may break
not-yet-established connections on request sockets as well as not removing
keys from sockets that were already established, but not yet ``accept()``'ed,
hanging in the accept queue.
(其结果是：当用户空间移除 TCP-AO 密钥时，可能会破坏 request socket 上尚未建立
的连接；同时也不会从那些已经建立、但尚未被 ``accept()``、仍挂在 accept 队列中
的套接字上移除密钥。)

The reverse is valid as well: if userspace adds a new key for a peer on
a listener socket, the established sockets in the accept queue won't
have the new keys.
(反过来的情形同样成立：如果用户空间在监听套接字上为某个对等端添加了新密钥，
accept 队列中已建立的套接字不会拥有这些新密钥。)

At this moment, the resolution for the two races:
``setsockopt(TCP_AO_ADD_KEY)`` vs ``accept()``
and ``setsockopt(TCP_AO_DEL_KEY)`` vs ``accept()`` is delegated to userspace.
This means that it's expected that userspace would check the MKTs on the socket
that was returned by ``accept()`` to verify that any key rotation that
happened on the listen socket is reflected on the newly established connection.
(目前，这两类竞态——``setsockopt(TCP_AO_ADD_KEY)`` 与 ``accept()`` 的竞态、
以及 ``setsockopt(TCP_AO_DEL_KEY)`` 与 ``accept()`` 的竞态——的解决办法交由
用户空间处理。这意味着，预期用户空间会检查 ``accept()`` 返回的套接字上的 MKT，
以验证监听套接字上发生的任何密钥轮换都已反映到新建立的连接上。)

This is a similar "do-nothing" approach to TCP-MD5 from the kernel side and
may be changed later by introducing new flags to ``tcp_ao_add``
and ``tcp_ao_del``.
(这与 TCP-MD5 的内核侧"什么都不做"的做法类似，后续可能会通过为 ``tcp_ao_add``
和 ``tcp_ao_del`` 引入新标志而改变。)

Note that this race is rare for it needs TCP-AO key rotation to happen
during the 3-way handshake for the new TCP connection.
(注意，这种竞态很少见，因为它需要 TCP-AO 密钥轮换恰好发生在新 TCP 连接的 3 次
握手期间。)

5. Interaction with TCP-MD5(5. 与 TCP-MD5 的交互)
=================================================

A TCP connection can not migrate between TCP-AO and TCP-MD5 options. The
established sockets that have either AO or MD5 keys are restricted for
adding keys of the other option.
(TCP 连接不能在 TCP-AO 与 TCP-MD5 选项之间迁移。已经建立的、拥有 AO 或 MD5
密钥的套接字会被限制添加另一种选项的密钥。)

For listening sockets the picture is different: BGP server may want to receive
both TCP-AO and (deprecated) TCP-MD5 clients. As a result, both types of keys
may be added to TCP_CLOSED or TCP_LISTEN sockets. It's not allowed to add
different types of keys for the same peer.
(对于监听套接字，情况则不同：BGP 服务器可能希望同时接收 TCP-AO 和（已废弃的）
TCP-MD5 客户端。因此，两种类型的密钥都可以添加到 TCP_CLOSED 或 TCP_LISTEN
套接字上。但不允许为同一个对等端添加不同类型的密钥。)

6. SNE Linux implementation(6. SNE 的 Linux 实现)
=================================================

RFC 5925 [6.2] describes the algorithm of how to extend TCP sequence numbers
with SNE.  In short: TCP has to track the previous sequence numbers and set
sne_flag when the current SEQ number rolls over. The flag is cleared when
both current and previous SEQ numbers cross 0x7fff, which is 32Kb.
(RFC 5925 [6.2] 描述了如何用 SNE 扩展 TCP 序列号的算法。简而言之：TCP 必须
跟踪之前的序列号，并在当前 SEQ 号回绕时设置 sne_flag。当当前与之前的 SEQ 号都
越过 0x7fff（即 32Kb）时，该标志被清除。)

In times when sne_flag is set, the algorithm compares SEQ for each packet with
0x7fff and if it's higher than 32Kb, it assumes that the packet should be
verified with SNE before the increment. As a result, there's
this [0; 32Kb] window, when packets with (SNE - 1) can be accepted.
(在 sne_flag 置位期间，算法会把每个数据包的 SEQ 与 0x7fff 比较，如果高于 32Kb，
就假定该数据包应当使用 SNE 在递增之前进行验证。因此存在这样一个 [0; 32Kb]
窗口，期间带 (SNE - 1) 的数据包是可以被接受的。)

Linux implementation simplifies this a bit: as the network stack already tracks
the first SEQ byte that ACK is wanted for (snd_una) and the next SEQ byte that
is wanted (rcv_nxt) - that's enough information for a rough estimation
on where in the 4GB SEQ number space both sender and receiver are.
When they roll over to zero, the corresponding SNE gets incremented.
(Linux 实现对此稍作简化：由于网络栈已经跟踪了希望获得 ACK 的第一个 SEQ 字节
(snd_una) 以及所期望的下一个 SEQ 字节 (rcv_nxt)，这些信息已足以粗略估计发送方
和接收方在 4GB SEQ 号空间中的位置。当它们回绕到零时，相应的 SNE 就会递增。)

tcp_ao_compute_sne() is called for each TCP-AO segment. It compares SEQ numbers
from the segment with snd_una or rcv_nxt and fits the result into a 2GB window around them,
detecting SEQ numbers rolling over. That simplifies the code a lot and only
requires SNE numbers to be stored on every TCP-AO socket.
(每个 TCP-AO 报文段都会调用 tcp_ao_compute_sne()。它把报文段中的 SEQ 号与
snd_una 或 rcv_nxt 比较，并把结果放入围绕它们的一个 2GB 窗口内，以此检测 SEQ
号的回绕。这大大简化了代码，只需要在每个 TCP-AO 套接字上存储 SNE 号。)

The 2GB window at first glance seems much more permissive compared to
RFC 5926. But that is only used to pick the correct SNE before/after
a rollover. It allows more TCP segment replays, but yet all regular
TCP checks in tcp_sequence() are applied on the verified segment.
So, it trades a bit more permissive acceptance of replayed/retransmitted
segments for the simplicity of the algorithm and what seems better behaviour
for large TCP windows.
(乍看之下，这个 2GB 窗口似乎比 RFC 5926 宽松得多。但它只用于在回绕前/后选择
正确的 SNE。它允许更多的 TCP 报文段重放，不过所有常规的 TCP 检查（
tcp_sequence() 中）仍会施加在已验证的报文段上。因此，这是以对重放/重传报文段
稍微更宽松的接受，换取算法的简洁性以及对大 TCP 窗口看似更好的行为。)

7. Links(7. 相关链接)
=====================

RFC 5925 The TCP Authentication Option
   https://www.rfc-editor.org/rfc/pdfrfc/rfc5925.txt.pdf

(RFC 5925 TCP 认证选项)

RFC 5926 Cryptographic Algorithms for the TCP Authentication Option (TCP-AO)
   https://www.rfc-editor.org/rfc/pdfrfc/rfc5926.txt.pdf

(RFC 5926 TCP 认证选项 (TCP-AO) 的密码算法)

Draft "SHA-2 Algorithm for the TCP Authentication Option (TCP-AO)"
   https://datatracker.ietf.org/doc/html/draft-nayak-tcp-sha2-03

(草案 "SHA-2 Algorithm for the TCP Authentication Option (TCP-AO)")

RFC 2385 Protection of BGP Sessions via the TCP MD5 Signature Option
   https://www.rfc-editor.org/rfc/pdfrfc/rfc2385.txt.pdf

(RFC 2385 通过 TCP MD5 签名选项保护 BGP 会话)

:Author: Dmitry Safonov <dima@arista.com>
  (作者: Dmitry Safonov <dima@arista.com>)
