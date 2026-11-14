===========
NFS LOCALIO
===========
(NFS LOCALIO(本地 IO))

Overview
(概述)
========

The LOCALIO auxiliary RPC protocol allows the Linux NFS client and
server to reliably handshake to determine if they are on the same
host. Select "NFS client and server support for LOCALIO auxiliary
protocol" in menuconfig to enable CONFIG_NFS_LOCALIO in the kernel
config (both CONFIG_NFS_FS and CONFIG_NFSD must also be enabled).
(LOCALIO 辅助 RPC 协议允许 Linux NFS 客户端与服务器可靠地握手，以确定它们是否位于同一主机。在 menuconfig 中选择 "NFS client and server support for LOCALIO auxiliary protocol"，以在内核配置中启用 CONFIG_NFS_LOCALIO(还必须同时启用 CONFIG_NFS_FS 和 CONFIG_NFSD)。)

Once an NFS client and server handshake as "local", the client will
bypass the network RPC protocol for read, write and commit operations.
Due to this XDR and RPC bypass, these operations will operate faster.
(一旦 NFS 客户端与服务器握手为"本地"，客户端在读、写和提交操作上将绕过网络 RPC 协议。由于绕过了 XDR 和 RPC，这些操作会执行得更快。)

The LOCALIO auxiliary protocol's implementation, which uses the same
connection as NFS traffic, follows the pattern established by the NFS
ACL protocol extension.
(LOCALIO 辅助协议的实现使用与 NFS 流量相同的连接，遵循 NFS ACL 协议扩展所确立的模式。)

The LOCALIO auxiliary protocol is needed to allow robust discovery of
clients local to their servers. In a private implementation that
preceded use of this LOCALIO protocol, a fragile sockaddr network
address based match against all local network interfaces was attempted.
But unlike the LOCALIO protocol, the sockaddr-based matching didn't
handle use of iptables or containers.
(需要 LOCALIO 辅助协议来可靠地发现位于其服务器本地的客户端。在使用该 LOCALIO 协议之前的一个私有实现中，曾尝试以脆弱的、基于 sockaddr 网络地址的方式对所有本地网络接口进行匹配。但与 LOCALIO 协议不同，基于 sockaddr 的匹配无法应对 iptables 或容器的使用。)

The robust handshake between local client and server is just the
beginning, the ultimate use case this locality makes possible is the
client is able to open files and issue reads, writes and commits
directly to the server without having to go over the network. The
requirement is to perform these loopback NFS operations as efficiently
as possible, this is particularly useful for container use cases
(e.g. kubernetes) where it is possible to run an IO job local to the
server.
(本地客户端与服务器之间可靠的握手只是一个开始，这种本地性所能实现的最终用例是：客户端能够直接向服务器打开文件并发出读、写和提交操作，而不必经过网络。要求是尽可能高效地执行这些环回 NFS 操作，这对容器用例(例如 kubernetes)尤为有用，因为在这些场景中可以把 IO 作业运行在服务器本地。)

The performance advantage realized from LOCALIO's ability to bypass
using XDR and RPC for reads, writes and commits can be extreme, e.g.:
(LOCALIO 能够在读、写和提交操作中绕过 XDR 和 RPC，由此获得的性能优势可能极其显著，例如：以下为使用 directio、队列深度 8、分别以 16 个和 1 个 libaio 线程运行 fio 20 秒的对比结果：)

fio for 20 secs with directio, qd of 8, 16 libaio threads:
  - With LOCALIO:
    4K read:    IOPS=979k,  BW=3825MiB/s (4011MB/s)(74.7GiB/20002msec)
    4K write:   IOPS=165k,  BW=646MiB/s  (678MB/s)(12.6GiB/20002msec)
    128K read:  IOPS=402k,  BW=49.1GiB/s (52.7GB/s)(982GiB/20002msec)
    128K write: IOPS=11.5k, BW=1433MiB/s (1503MB/s)(28.0GiB/20004msec)

  - Without LOCALIO:
    4K read:    IOPS=79.2k, BW=309MiB/s  (324MB/s)(6188MiB/20003msec)
    4K write:   IOPS=59.8k, BW=234MiB/s  (245MB/s)(4671MiB/20002msec)
    128K read:  IOPS=33.9k, BW=4234MiB/s (4440MB/s)(82.7GiB/20004msec)
    128K write: IOPS=11.5k, BW=1434MiB/s (1504MB/s)(28.0GiB/20011msec)

fio for 20 secs with directio, qd of 8, 1 libaio thread:
  - With LOCALIO:
    4K read:    IOPS=230k,  BW=898MiB/s  (941MB/s)(17.5GiB/20001msec)
    4K write:   IOPS=22.6k, BW=88.3MiB/s (92.6MB/s)(1766MiB/20001msec)
    128K read:  IOPS=38.8k, BW=4855MiB/s (5091MB/s)(94.8GiB/20001msec)
    128K write: IOPS=11.4k, BW=1428MiB/s (1497MB/s)(27.9GiB/20001msec)

  - Without LOCALIO:
    4K read:    IOPS=77.1k, BW=301MiB/s  (316MB/s)(6022MiB/20001msec)
    4K write:   IOPS=32.8k, BW=128MiB/s  (135MB/s)(2566MiB/20001msec)
    128K read:  IOPS=24.4k, BW=3050MiB/s (3198MB/s)(59.6GiB/20001msec)
    128K write: IOPS=11.4k, BW=1430MiB/s (1500MB/s)(27.9GiB/20001msec)

FAQ
(常见问题)
===

1. What are the use cases for LOCALIO?
   (LOCALIO 的用例有哪些？)

   a. Workloads where the NFS client and server are on the same host
      realize improved IO performance. In particular, it is common when
      running containerised workloads for jobs to find themselves
      running on the same host as the knfsd server being used for
      storage.
      (NFS 客户端与服务器位于同一主机上的工作负载可以获得更好的 IO 性能。特别是在运行容器化工作负载时，作业常常会发现自己正运行在用作存储的 knfsd 服务器所在的主机上。)

2. What are the requirements for LOCALIO?
   (LOCALIO 有哪些要求？)

   a. Bypass use of the network RPC protocol as much as possible. This
      includes bypassing XDR and RPC for open, read, write and commit
      operations.
      (尽可能绕过网络 RPC 协议的使用。这包括对打开、读、写和提交操作绕过 XDR 和 RPC。)
   b. Allow client and server to autonomously discover if they are
      running local to each other without making any assumptions about
      the local network topology.
      (允许客户端和服务器自主发现它们是否在彼此本地运行，而无需对本地网络拓扑做任何假设。)
   c. Support the use of containers by being compatible with relevant
      namespaces (e.g. network, user, mount).
      (通过与相关命名空间(例如网络、用户、挂载)兼容来支持容器的使用。)
   d. Support all versions of NFS. NFSv3 is of particular importance
      because it has wide enterprise usage and pNFS flexfiles makes use
      of it for the data path.
      (支持所有版本的 NFS。NFSv3 尤为重要，因为它在企业中被广泛使用，并且 pNFS flexfiles 在数据路径中利用了它。)

3. Why doesn’t LOCALIO just compare IP addresses or hostnames when
   deciding if the NFS client and server are co-located on the same
   host?
   (为什么 LOCALIO 在判断 NFS 客户端与服务器是否共置于同一主机上时，不直接比较 IP 地址或主机名？)

   Since one of the main use cases is containerised workloads, we cannot
   assume that IP addresses will be shared between the client and
   server. This sets up a requirement for a handshake protocol that
   needs to go over the same connection as the NFS traffic in order to
   identify that the client and the server really are running on the
   same host. The handshake uses a secret that is sent over the wire,
   and can be verified by both parties by comparing with a value stored
   in shared kernel memory if they are truly co-located.
   (由于主要用例之一是容器化工作负载，我们不能假设客户端与服务器之间会共享 IP 地址。这就提出了一个要求：握手协议必须经过与 NFS 流量相同的连接，以确认客户端和服务器确实运行在同一主机上。握手使用一个通过网络发送的密钥，如果双方确实共置一处，双方可以通过与存储在共享内核内存中的值进行比较来验证该密钥。)

4. Does LOCALIO improve pNFS flexfiles?
   (LOCALIO 能改进 pNFS flexfiles 吗？)

   Yes, LOCALIO complements pNFS flexfiles by allowing it to take
   advantage of NFS client and server locality.  Policy that initiates
   client IO as closely to the server where the data is stored naturally
   benefits from the data path optimization LOCALIO provides.
   (是的，LOCALIO 通过让 pNFS flexfiles 利用 NFS 客户端与服务器的本地性来对其形成补充。在尽可能靠近数据所在服务器处发起客户端 IO 的策略，自然会从 LOCALIO 提供的数据路径优化中受益。)

5. Why not develop a new pNFS layout to enable LOCALIO?
   (为什么不开发一种新的 pNFS 布局来启用 LOCALIO？)

   A new pNFS layout could be developed, but doing so would put the
   onus on the server to somehow discover that the client is co-located
   when deciding to hand out the layout.
   There is value in a simpler approach (as provided by LOCALIO) that
   allows the NFS client to negotiate and leverage locality without
   requiring more elaborate modeling and discovery of such locality in a
   more centralized manner.
   (可以开发新的 pNFS 布局，但这样做会在决定分发布局时，把设法发现客户端是否共置的责任压到服务器身上。更简单的方案(如 LOCALIO 所提供的)自有其价值：它允许 NFS 客户端协商并利用本地性，而无需以更集中的方式对这种本地性进行更精细的建模和发现。)

6. Why is having the client perform a server-side file OPEN, without
   using RPC, beneficial?  Is the benefit pNFS specific?
   (为什么让客户端在不使用 RPC 的情况下执行服务器端文件 OPEN 是有益的？这一益处是 pNFS 特有的吗？)

   Avoiding the use of XDR and RPC for file opens is beneficial to
   performance regardless of whether pNFS is used. Especially when
   dealing with small files its best to avoid going over the wire
   whenever possible, otherwise it could reduce or even negate the
   benefits of avoiding the wire for doing the small file I/O itself.
   Given LOCALIO's requirements the current approach of having the
   client perform a server-side file open, without using RPC, is ideal.
   If in the future requirements change then we can adapt accordingly.
   (无论是否使用 pNFS，避免在文件打开时使用 XDR 和 RPC 都有益于性能。尤其在处理小文件时，最好尽可能避免走网络，否则可能会削弱甚至抵消为小文件 I/O 本身避免走网络所带来的好处。鉴于 LOCALIO 的要求，当前这种让客户端在不使用 RPC 的情况下执行服务器端文件打开的做法是理想的。如果将来需求发生变化，我们可以相应调整。)

7. Why is LOCALIO only supported with UNIX Authentication (AUTH_UNIX)?
   (为什么 LOCALIO 只支持 UNIX 认证(AUTH_UNIX)？)

   Strong authentication is usually tied to the connection itself. It
   works by establishing a context that is cached by the server, and
   that acts as the key for discovering the authorisation token, which
   can then be passed to rpc.mountd to complete the authentication
   process. On the other hand, in the case of AUTH_UNIX, the credential
   that was passed over the wire is used directly as the key in the
   upcall to rpc.mountd. This simplifies the authentication process, and
   so makes AUTH_UNIX easier to support.
   (强认证通常与连接本身绑定。它的工作方式是建立一个由服务器缓存的上下文，该上下文充当发现授权令牌的密钥，随后授权令牌可被传递给 rpc.mountd 以完成认证过程。另一方面，对 AUTH_UNIX 而言，通过网络传递的凭据被直接用作向 rpc.mountd 发起 upcall 的密钥。这简化了认证过程，因此使 AUTH_UNIX 更容易支持。)

8. How do export options that translate RPC user IDs behave for LOCALIO
   operations (eg. root_squash, all_squash)?
   (转换 RPC 用户 ID 的导出选项(例如 root_squash、all_squash)对 LOCALIO 操作的表现如何？)

   Export options that translate user IDs are managed by nfsd_setuser()
   which is called by nfsd_setuser_and_check_port() which is called by
   __fh_verify().  So they get handled exactly the same way for LOCALIO
   as they do for non-LOCALIO.
   (转换用户 ID 的导出选项由 nfsd_setuser() 管理，它由 nfsd_setuser_and_check_port() 调用，而后者又由 __fh_verify() 调用。因此，无论对 LOCALIO 还是非 LOCALIO，它们的处理方式完全相同。)

9. How does LOCALIO make certain that object lifetimes are managed
   properly given NFSD and NFS operate in different contexts?
   (鉴于 NFSD 和 NFS 在不同上下文中运行，LOCALIO 如何确保对象生命周期得到妥善管理？)

   See the detailed "NFS Client and Server Interlock" section below.
   (参见下文详细的"NFS Client and Server Interlock"(NFS 客户端与服务器互锁)一节。)

RPC
(RPC)
===

The LOCALIO auxiliary RPC protocol consists of a single "UUID_IS_LOCAL"
RPC method that allows the Linux NFS client to verify the local Linux
NFS server can see the nonce (single-use UUID) the client generated and
made available in nfs_common. This protocol isn't part of an IETF
standard, nor does it need to be considering it is Linux-to-Linux
auxiliary RPC protocol that amounts to an implementation detail.
(LOCALIO 辅助 RPC 协议由单一的 "UUID_IS_LOCAL" RPC 方法构成，该方法允许 Linux NFS 客户端验证本地的 Linux NFS 服务器能够看到客户端生成并在 nfs_common 中提供的 nonce(一次性 UUID)。该协议不属于任何 IETF 标准，也不需要属于——考虑到它是 Linux 对 Linux 的辅助 RPC 协议，本质上只是一个实现细节。)

The UUID_IS_LOCAL method encodes the client generated uuid_t in terms of
the fixed UUID_SIZE (16 bytes). The fixed size opaque encode and decode
XDR methods are used instead of the less efficient variable sized
methods.
(UUID_IS_LOCAL 方法按照固定的 UUID_SIZE(16 字节)对客户端生成的 uuid_t 进行编码。这里使用固定大小的 opaque 编解码 XDR 方法，而不是效率较低的变长方法。)

The RPC program number for the NFS_LOCALIO_PROGRAM is 400122 (as assigned
by IANA, see https://www.iana.org/assignments/rpc-program-numbers/ ):
Linux Kernel Organization       400122  nfslocalio
(NFS_LOCALIO_PROGRAM 的 RPC 程序号是 400122(由 IANA 分配，参见 https://www.iana.org/assignments/rpc-program-numbers/ )，对应的登记项为：Linux Kernel Organization       400122  nfslocalio)

The LOCALIO protocol spec in rpcgen syntax is::
(LOCALIO 协议的 rpcgen 语法规范如下：)

  /* raw RFC 9562 UUID */
  #define UUID_SIZE 16
  typedef u8 uuid_t<UUID_SIZE>;

  program NFS_LOCALIO_PROGRAM {
      version LOCALIO_V1 {
          void
              NULL(void) = 0;

          void
              UUID_IS_LOCAL(uuid_t) = 1;
      } = 1;
  } = 400122;

LOCALIO uses the same transport connection as NFS traffic. As such,
LOCALIO is not registered with rpcbind.
(LOCALIO 使用与 NFS 流量相同的传输连接。因此，LOCALIO 不向 rpcbind 注册。)

NFS Common and Client/Server Handshake
(NFS Common 与客户端/服务器握手)
======================================

fs/nfs_common/nfslocalio.c provides interfaces that enable an NFS client
to generate a nonce (single-use UUID) and associated short-lived
nfs_uuid_t struct, register it with nfs_common for subsequent lookup and
verification by the NFS server and if matched the NFS server populates
members in the nfs_uuid_t struct. The NFS client then uses nfs_common to
transfer the nfs_uuid_t from its nfs_uuids to the nn->nfsd_serv
clients_list from the nfs_common's uuids_list.  See:
fs/nfs/localio.c:nfs_local_probe()
(fs/nfs_common/nfslocalio.c 提供了一些接口，使 NFS 客户端能够生成一个 nonce(一次性 UUID)及关联的短生命周期 nfs_uuid_t 结构体，并将其注册到 nfs_common 中，供 NFS 服务器后续查找和验证；如果匹配，NFS 服务器会填充 nfs_uuid_t 结构体中的成员。随后，NFS 客户端使用 nfs_common 把 nfs_uuid_t 从其 nfs_uuids 转移到 nn->nfsd_serv 的 clients_list，也就是从 nfs_common 的 uuids_list 转移过去。参见：fs/nfs/localio.c:nfs_local_probe())

nfs_common's nfs_uuids list is the basis for LOCALIO enablement, as such
it has members that point to nfsd memory for direct use by the client
(e.g. 'net' is the server's network namespace, through it the client can
access nn->nfsd_serv with proper rcu read access). It is this client
and server synchronization that enables advanced usage and lifetime of
objects to span from the host kernel's nfsd to per-container knfsd
instances that are connected to nfs client's running on the same local
host.
(nfs_common 的 nfs_uuids 列表是启用 LOCALIO 的基础，因此它包含一些指向 nfsd 内存、供客户端直接使用的成员(例如 'net' 是服务器的网络命名空间，通过它可以以恰当的 rcu 读访问来访问 nn->nfsd_serv)。正是这种客户端与服务器的同步，使得高级用法和对象的生命周期可以跨越主机内核的 nfsd，延伸到与运行在同一本地主机上的 NFS 客户端相连的每容器 knfsd 实例。)

NFS Client and Server Interlock
(NFS 客户端与服务器互锁)
===============================

LOCALIO provides the nfs_uuid_t object and associated interfaces to
allow proper network namespace (net-ns) and NFSD object refcounting.
(LOCALIO 提供 nfs_uuid_t 对象及相关接口，以实现对网络命名空间(net-ns)和 NFSD 对象的恰当引用计数。)

LOCALIO required the introduction and use of NFSD's percpu nfsd_net_ref
to interlock nfsd_shutdown_net() and nfsd_open_local_fh(), to ensure
each net-ns is not destroyed while in use by nfsd_open_local_fh(), and
warrants a more detailed explanation:
(LOCALIO 需要引入并使用 NFSD 的 percpu 变量 nfsd_net_ref 来对 nfsd_shutdown_net() 和 nfsd_open_local_fh() 进行互锁，以确保每个 net-ns 在被 nfsd_open_local_fh() 使用期间不会被销毁。这值得更详细的解释：)

    nfsd_open_local_fh() uses nfsd_net_try_get() before opening its
    nfsd_file handle and then the caller (NFS client) must drop the
    reference for the nfsd_file and associated net-ns using
    nfsd_file_put_local() once it has completed its IO.
    (nfsd_open_local_fh() 在打开其 nfsd_file 句柄之前会先使用 nfsd_net_try_get()，随后调用方(NFS 客户端)在完成其 IO 之后，必须使用 nfsd_file_put_local() 释放对 nfsd_file 及关联 net-ns 的引用。)

    This interlock working relies heavily on nfsd_open_local_fh() being
    afforded the ability to safely deal with the possibility that the
    NFSD's net-ns (and nfsd_net by association) may have been destroyed
    by nfsd_destroy_serv() via nfsd_shutdown_net().
    (这一互锁之所以能够工作，很大程度上依赖于 nfsd_open_local_fh() 被赋予了安全应对以下可能性的能力：NFSD 的 net-ns(以及关联的 nfsd_net)可能已被 nfsd_destroy_serv() 通过 nfsd_shutdown_net() 销毁。)

This interlock of the NFS client and server has been verified to fix an
easy to hit crash that would occur if an NFSD instance running in a
container, with a LOCALIO client mounted, is shutdown. Upon restart of
the container and associated NFSD, the client would go on to crash due
to NULL pointer dereference that occurred due to the LOCALIO client's
attempting to nfsd_open_local_fh() without having a proper reference on
NFSD's net-ns.
(经验证，NFS 客户端与服务器的这种互锁修复了一个容易触发的崩溃：当运行在容器中并挂载着 LOCALIO 客户端的 NFSD 实例被关闭时会发生该崩溃。在容器及关联的 NFSD 重启之后，客户端会因空指针解引用而继续崩溃，其原因正是 LOCALIO 客户端在没有持有对 NFSD net-ns 的恰当引用的情况下尝试调用 nfsd_open_local_fh()。)

NFS Client issues IO instead of Server
(由 NFS 客户端代替服务器发起 IO)
======================================

Because LOCALIO is focused on protocol bypass to achieve improved IO
performance, alternatives to the traditional NFS wire protocol (SUNRPC
with XDR) must be provided to access the backing filesystem.
(由于 LOCALIO 专注于通过协议绕过来提升 IO 性能，必须提供传统 NFS 网络协议(SUNRPC 与 XDR)之外的替代方式来访问后端文件系统。)

See fs/nfs/localio.c:nfs_local_open_fh() and
fs/nfsd/localio.c:nfsd_open_local_fh() for the interface that makes
focused use of select nfs server objects to allow a client local to a
server to open a file pointer without needing to go over the network.
(参见 fs/nfs/localio.c:nfs_local_open_fh() 和 fs/nfsd/localio.c:nfsd_open_local_fh()：这一接口聚焦地使用选定的 NFS 服务器对象，使位于服务器本地的客户端无需经过网络即可打开文件指针。)

The client's fs/nfs/localio.c:nfs_local_open_fh() will call into the
server's fs/nfsd/localio.c:nfsd_open_local_fh() and carefully access
both the associated nfsd network namespace and nn->nfsd_serv in terms of
RCU. If nfsd_open_local_fh() finds that the client no longer sees valid
nfsd objects (be it struct net or nn->nfsd_serv) it returns -ENXIO
to nfs_local_open_fh() and the client will try to reestablish the
LOCALIO resources needed by calling nfs_local_probe() again. This
recovery is needed if/when an nfsd instance running in a container were
to reboot while a LOCALIO client is connected to it.
(客户端的 fs/nfs/localio.c:nfs_local_open_fh() 会调用服务器的 fs/nfsd/localio.c:nfsd_open_local_fh()，并在 RCU 的保护下谨慎地访问关联的 nfsd 网络命名空间和 nn->nfsd_serv。如果 nfsd_open_local_fh() 发现客户端不再能看到有效的 nfsd 对象(无论是 struct net 还是 nn->nfsd_serv)，它会向 nfs_local_open_fh() 返回 -ENXIO，客户端随后会尝试通过再次调用 nfs_local_probe() 来重建所需的 LOCALIO 资源。当运行在容器中的 nfsd 实例在 LOCALIO 客户端连接着它的情况下重启时，就需要这种恢复。)

Once the client has an open nfsd_file pointer it will issue reads,
writes and commits directly to the underlying local filesystem (normally
done by the nfs server). As such, for these operations, the NFS client
is issuing IO to the underlying local filesystem that it is sharing with
the NFS server. See: fs/nfs/localio.c:nfs_local_doio() and
fs/nfs/localio.c:nfs_local_commit().
(一旦客户端获得已打开的 nfsd_file 指针，它就会直接向底层本地文件系统发出读、写和提交操作(这些通常由 NFS 服务器完成)。因此，对于这些操作，是 NFS 客户端在向它与 NFS 服务器共享的底层本地文件系统发出 IO。参见：fs/nfs/localio.c:nfs_local_doio() 和 fs/nfs/localio.c:nfs_local_commit()。)

With normal NFS that makes use of RPC to issue IO to the server, if an
application uses O_DIRECT the NFS client will bypass the pagecache but
the NFS server will not. The NFS server's use of buffered IO affords
applications to be less precise with their alignment when issuing IO to
the NFS client. But if all applications properly align their IO, LOCALIO
can be configured to use end-to-end O_DIRECT semantics from the NFS
client to the underlying local filesystem, that it is sharing with
the NFS server, by setting the 'localio_O_DIRECT_semantics' nfs module
parameter to Y, e.g.:
(对于使用 RPC 向服务器发出 IO 的普通 NFS，如果应用程序使用 O_DIRECT，NFS 客户端会绕过页缓存，但 NFS 服务器不会。NFS 服务器对缓冲 IO 的使用，使得应用程序在向 NFS 客户端发出 IO 时可以对齐得不那么精确。但如果所有应用程序都正确对齐其 IO，则可以通过将 nfs 模块参数 'localio_O_DIRECT_semantics' 设置为 Y，把 LOCALIO 配置为使用从 NFS 客户端一直到其与 NFS 服务器共享的底层本地文件系统的端到端 O_DIRECT 语义，例如：)

    echo Y > /sys/module/nfs/parameters/localio_O_DIRECT_semantics

Once enabled, it will cause LOCALIO to use end-to-end O_DIRECT semantics
(but again, this may cause IO to fail if applications do not properly
align their IO).
(启用后，它将使 LOCALIO 使用端到端的 O_DIRECT 语义(但再次强调，如果应用程序没有正确对齐其 IO，这可能导致 IO 失败)。)

Security
(安全性)
========

LOCALIO is only supported when UNIX-style authentication (AUTH_UNIX, aka
AUTH_SYS) is used.
(仅在使用 UNIX 风格认证(AUTH_UNIX, 即 AUTH_SYS)时才支持 LOCALIO。)

Care is taken to ensure the same NFS security mechanisms are used
(authentication, etc) regardless of whether LOCALIO or regular NFS
access is used. The auth_domain established as part of the traditional
NFS client access to the NFS server is also used for LOCALIO.
(无论使用的是 LOCALIO 还是常规 NFS 访问，都会注意确保使用相同的 NFS 安全机制(认证等)。作为传统 NFS 客户端访问 NFS 服务器的一部分而建立的 auth_domain 也同样用于 LOCALIO。)

Relative to containers, LOCALIO gives the client access to the network
namespace the server has. This is required to allow the client to access
the server's per-namespace nfsd_net struct. With traditional NFS, the
client is afforded this same level of access (albeit in terms of the NFS
protocol via SUNRPC). No other namespaces (user, mount, etc) have been
altered or purposely extended from the server to the client.
(就容器而言，LOCALIO 让客户端可以访问服务器所拥有的网络命名空间。要允许客户端访问服务器的按命名空间划分的 nfsd_net 结构体，这是必需的。在传统 NFS 下，客户端也被给予同样级别的访问(尽管是以通过 SUNRPC 的 NFS 协议形式)。没有其他命名空间(用户、挂载等)被改动或被有意地从服务器扩展到客户端。)

Module Parameters
(模块参数)
=================

/sys/module/nfs/parameters/localio_enabled (bool)
controls if LOCALIO is enabled, defaults to Y. If client and server are
local but 'localio_enabled' is set to N then LOCALIO will not be used.
(/sys/module/nfs/parameters/localio_enabled (bool)控制是否启用 LOCALIO，默认为 Y。如果客户端和服务器在本地，但 'localio_enabled' 被设置为 N，则不会使用 LOCALIO。)

/sys/module/nfs/parameters/localio_O_DIRECT_semantics (bool)
controls if O_DIRECT extends down to the underlying filesystem, defaults
to N. Application IO must be logical blocksize aligned, otherwise
O_DIRECT will fail.
(/sys/module/nfs/parameters/localio_O_DIRECT_semantics (bool)控制 O_DIRECT 是否向下延伸到底层文件系统，默认为 N。应用程序 IO 必须按逻辑块大小对齐，否则 O_DIRECT 将失败。)

/sys/module/nfsv3/parameters/nfs3_localio_probe_throttle (uint)
controls if NFSv3 read and write IOs will trigger (re)enabling of
LOCALIO every N (nfs3_localio_probe_throttle) IOs, defaults to 0
(disabled). Must be power-of-2, admin keeps all the pieces if they
misconfigure (too low a value or non-power-of-2).
(/sys/module/nfsv3/parameters/nfs3_localio_probe_throttle (uint)控制 NFSv3 读写 IO 是否每 N 个(nfs3_localio_probe_throttle)IO 触发一次(重新)启用 LOCALIO，默认为 0(禁用)。必须是 2 的幂；如果管理员配置错误(值过低或不是 2 的幂)，一切后果自负。)

Testing
(测试)
=======

The LOCALIO auxiliary protocol and associated NFS LOCALIO read, write
and commit access have proven stable against various test scenarios:
(LOCALIO 辅助协议以及相关的 NFS LOCALIO 读、写和提交访问，已在各种测试场景下被证明是稳定的：)

- Client and server both on the same host.
  (客户端和服务器都在同一主机上。)

- All permutations of client and server support enablement for both
  local and remote client and server.
  (针对本地与远程的客户端和服务器，其支持启用的所有排列组合。)

- Testing against NFS storage products that don't support the LOCALIO
  protocol was also performed.
  (也针对不支持 LOCALIO 协议的 NFS 存储产品进行了测试。)

- Client on host, server within a container (for both v3 and v4.2).
  The container testing was in terms of podman managed containers and
  includes successful container stop/restart scenario.
  (客户端在主机上、服务器在容器内(v3 和 v4.2 皆有)。容器测试基于 podman 管理的容器，并包括成功的容器停止/重启场景。)

- Formalizing these test scenarios in terms of existing test
  infrastructure is on-going. Initial regular coverage is provided in
  terms of ktest running xfstests against a LOCALIO-enabled NFS loopback
  mount configuration, and includes lockdep and KASAN coverage, see:
  (利用现有测试基础设施将这些测试场景正式化的工作仍在进行中。初步的常规覆盖由 ktest 提供：针对启用了 LOCALIO 的 NFS 环回挂载配置运行 xfstests，并包括 lockdep 和 KASAN 覆盖，参见：)
  https://evilpiepirate.org/~testdashboard/ci?user=snitzer&branch=snitm-nfs-next
  https://github.com/koverstreet/ktest

- Various kdevops testing (in terms of "Chuck's BuildBot") has been
  performed to regularly verify the LOCALIO changes haven't caused any
  regressions to non-LOCALIO NFS use cases.
  (还进行了各种 kdevops 测试(通过"Chuck 的 BuildBot")，以定期验证 LOCALIO 的改动没有给非 LOCALIO 的 NFS 用例造成任何回退。)

- All of Hammerspace's various sanity tests pass with LOCALIO enabled
  (this includes numerous pNFS and flexfiles tests).
  (在启用 LOCALIO 的情况下，Hammerspace 的各种健全性测试全部通过(这包括大量的 pNFS 和 flexfiles 测试)。)
