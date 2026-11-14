.. SPDX-License-Identifier: GPL-2.0

=======================
NFSv4 client identifier
=======================

(NFSv4 客户端标识符)

This document explains how the NFSv4 protocol identifies client
instances in order to maintain file open and lock state during
system restarts. A special identifier and principal are maintained
on each client. These can be set by administrators, scripts
provided by site administrators, or tools provided by Linux
distributors.
(本文档解释 NFSv4 协议如何标识客户端实例，以便在系统重启期间维护文件打开和锁定状态。每个客户端上会维护一个特殊的标识符和 principal。它们可以由管理员、站点管理员提供的脚本或 Linux 发行版提供的工具来设置。)

There are risks if a client's NFSv4 identifier and its principal
are not chosen carefully.
(如果客户端的 NFSv4 标识符及其 principal 未经慎重选择，则会存在风险。)


Introduction
(简介)
------------

The NFSv4 protocol uses "lease-based file locking". Leases help
NFSv4 servers provide file lock guarantees and manage their
resources.
(NFSv4 协议使用"基于租约(lease)的文件锁定"。租约帮助 NFSv4 服务器提供文件锁保证并管理其资源。)

Simply put, an NFSv4 server creates a lease for each NFSv4 client.
The server collects each client's file open and lock state under
the lease for that client.
(简而言之，NFSv4 服务器会为每个 NFSv4 客户端创建一个租约。服务器将每个客户端的文件打开和锁定状态收集在该客户端的租约之下。)

The client is responsible for periodically renewing its leases.
While a lease remains valid, the server holding that lease
guarantees the file locks the client has created remain in place.
(客户端负责定期续租其租约。只要租约仍然有效，持有该租约的服务器就保证客户端已创建的文件锁保持不变。)

If a client stops renewing its lease (for example, if it crashes),
the NFSv4 protocol allows the server to remove the client's open
and lock state after a certain period of time. When a client
restarts, it indicates to servers that open and lock state
associated with its previous leases is no longer valid and can be
destroyed immediately.
(如果客户端停止续租其租约(例如发生崩溃)，NFSv4 协议允许服务器在一段时间之后移除该客户端的打开和锁定状态。当客户端重启时，它会向服务器表明：与其先前租约相关联的打开和锁定状态已不再有效，可以立即销毁。)

In addition, each NFSv4 server manages a persistent list of client
leases. When the server restarts and clients attempt to recover
their state, the server uses this list to distinguish amongst
clients that held state before the server restarted and clients
sending fresh OPEN and LOCK requests. This enables file locks to
persist safely across server restarts.
(此外，每个 NFSv4 服务器都管理一个持久的客户端租约列表。当服务器重启且客户端尝试恢复其状态时，服务器使用该列表来区分在服务器重启前持有状态的客户端与发送全新 OPEN 和 LOCK 请求的客户端。这使文件锁能够在服务器重启期间安全地持续存在。)

NFSv4 client identifiers
(NFSv4 客户端标识符)
------------------------

Each NFSv4 client presents an identifier to NFSv4 servers so that
they can associate the client with its lease. Each client's
identifier consists of two elements:
(每个 NFSv4 客户端都会向 NFSv4 服务器出示一个标识符，以便服务器能够将该客户端与其租约关联起来。每个客户端的标识符由两个元素组成：)

  - co_ownerid: An arbitrary but fixed string.
    (co_ownerid: 一个任意但固定的字符串。)

  - boot verifier: A 64-bit incarnation verifier that enables a
    server to distinguish successive boot epochs of the same client.
    (boot verifier: 一个 64 位的化身验证器，使服务器能够区分同一客户端先后不同的启动世代。)

The NFSv4.0 specification refers to these two items as an
"nfs_client_id4". The NFSv4.1 specification refers to these two
items as a "client_owner4".
(NFSv4.0 规范将这两项称为 "nfs_client_id4"。NFSv4.1 规范将这两项称为 "client_owner4"。)

NFSv4 servers tie this identifier to the principal and security
flavor that the client used when presenting it. Servers use this
principal to authorize subsequent lease modification operations
sent by the client. Effectively this principal is a third element of
the identifier.
(NFSv4 服务器将此标识符与客户端出示它时所使用的 principal 和安全机制(security flavor)绑定在一起。服务器使用该 principal 来授权客户端后续发送的租约修改操作。实际上，该 principal 就是标识符的第三个元素。)

As part of the identity presented to servers, a good
"co_ownerid" string has several important properties:
(作为向服务器出示的身份的一部分，一个良好的 "co_ownerid" 字符串具有几个重要特性：)

  - The "co_ownerid" string identifies the client during reboot
    recovery, therefore the string is persistent across client
    reboots.
    ("co_ownerid" 字符串在重启恢复期间标识客户端，因此该字符串在客户端重启后保持不变。)
  - The "co_ownerid" string helps servers distinguish the client
    from others, therefore the string is globally unique. Note
    that there is no central authority that assigns "co_ownerid"
    strings.
    ("co_ownerid" 字符串帮助服务器将该客户端与其他客户端区分开来，因此该字符串必须全局唯一。注意，没有任何中央机构来分配 "co_ownerid" 字符串。)
  - Because it often appears on the network in the clear, the
    "co_ownerid" string does not reveal private information about
    the client itself.
    (由于 "co_ownerid" 字符串常常以明文形式出现在网络上，它不会泄露关于客户端自身的私有信息。)
  - The content of the "co_ownerid" string is set and unchanging
    before the client attempts NFSv4 mounts after a restart.
    ("co_ownerid" 字符串的内容在客户端重启后尝试 NFSv4 挂载(mount)之前就已设定且保持不变。)
  - The NFSv4 protocol places a 1024-byte limit on the size of the
    "co_ownerid" string.
    (NFSv4 协议对 "co_ownerid" 字符串的大小施加了 1024 字节的限制。)

Protecting NFSv4 lease state
(保护 NFSv4 租约状态)
----------------------------

NFSv4 servers utilize the "client_owner4" as described above to
assign a unique lease to each client. Under this scheme, there are
circumstances where clients can interfere with each other. This is
referred to as "lease stealing".
(NFSv4 服务器如上所述利用 "client_owner4" 为每个客户端分配一个唯一的租约。在这种方案下，存在客户端之间可能相互干扰的情况。这被称为"租约窃取"(lease stealing)。)

If distinct clients present the same "co_ownerid" string and use
the same principal (for example, AUTH_SYS and UID 0), a server is
unable to tell that the clients are not the same. Each distinct
client presents a different boot verifier, so it appears to the
server as if there is one client that is rebooting frequently.
Neither client can maintain open or lock state in this scenario.
(如果不同的客户端出示相同的 "co_ownerid" 字符串并使用相同的 principal(例如 AUTH_SYS 和 UID 0)，服务器将无法分辨这些客户端并非同一个。每个不同的客户端出示不同的 boot verifier，因此在服务器看来，就像有一个客户端在频繁重启。在这种场景下，两个客户端都无法维护打开或锁定状态。)

If distinct clients present the same "co_ownerid" string and use
distinct principals, the server is likely to allow the first client
to operate normally but reject subsequent clients with the same
"co_ownerid" string.
(如果不同的客户端出示相同的 "co_ownerid" 字符串但使用不同的 principal，服务器很可能允许第一个客户端正常运行，而拒绝后续使用相同 "co_ownerid" 字符串的客户端。)

If a client's "co_ownerid" string or principal are not stable,
state recovery after a server or client reboot is not guaranteed.
If a client unexpectedly restarts but presents a different
"co_ownerid" string or principal to the server, the server orphans
the client's previous open and lock state. This blocks access to
locked files until the server removes the orphaned state.
(如果客户端的 "co_ownerid" 字符串或 principal 不稳定，则无法保证服务器或客户端重启后的状态恢复。如果客户端意外重启，但向服务器出示了不同的 "co_ownerid" 字符串或 principal，服务器会将该客户端先前的打开和锁定状态弃置为孤儿状态。这会阻止对被锁定文件的访问，直到服务器移除这些被弃置的状态。)

If the server restarts and a client presents a changed "co_ownerid"
string or principal to the server, the server will not allow the
client to reclaim its open and lock state, and may give those locks
to other clients in the meantime. This is referred to as "lock
stealing".
(如果服务器重启，而客户端向服务器出示了已更改的 "co_ownerid" 字符串或 principal，服务器将不允许该客户端回收其打开和锁定状态，并且在此期间可能把这些锁授予其他客户端。这被称为"锁窃取"(lock stealing)。)

Lease stealing and lock stealing increase the potential for denial
of service and in rare cases even data corruption.
(租约窃取和锁窃取会增加拒绝服务的可能性，在极少数情况下甚至会导致数据损坏。)

Selecting an appropriate client identifier
(选择合适的客户端标识符)
------------------------------------------

By default, the Linux NFSv4 client implementation constructs its
"co_ownerid" string starting with the words "Linux NFS" followed by
the client's UTS node name (the same node name, incidentally, that
is used as the "machine name" in an AUTH_SYS credential). In small
deployments, this construction is usually adequate. Often, however,
the node name by itself is not adequately unique, and can change
unexpectedly. Problematic situations include:
(默认情况下，Linux NFSv4 客户端实现构造其 "co_ownerid" 字符串的方式是：以 "Linux NFS" 开头，后跟客户端的 UTS 节点名(顺带一提，与 AUTH_SYS 凭证中用作"机器名"的是同一个节点名)。在小型部署中，这种构造通常已经足够。然而，节点名本身常常不具有足够的唯一性，并且可能意外变化。有问题的情形包括：)

  - NFS-root (diskless) clients, where the local DHCP server (or
    equivalent) does not provide a unique host name.
    (NFS-root(无盘)客户端：本地 DHCP 服务器(或类似设施)未提供唯一的主机名。)

  - "Containers" within a single Linux host.  If each container has
    a separate network namespace, but does not use the UTS namespace
    to provide a unique host name, then there can be multiple NFS
    client instances with the same host name.
    (单个 Linux 主机内的"容器"：如果每个容器拥有独立的网络命名空间，但未使用 UTS 命名空间来提供唯一的主机名，那么就可能存在多个具有相同主机名的 NFS 客户端实例。)

  - Clients across multiple administrative domains that access a
    common NFS server. If hostnames are not assigned centrally
    then uniqueness cannot be guaranteed unless a domain name is
    included in the hostname.
    (访问同一 NFS 服务器的跨多个管理域的客户端：如果主机名不是集中分配的，那么除非主机名中包含域名，否则无法保证唯一性。)

Linux provides two mechanisms to add uniqueness to its "co_ownerid"
string:
(Linux 提供了两种机制来增强其 "co_ownerid" 字符串的唯一性：)

    nfs.nfs4_unique_id
      This module parameter can set an arbitrary uniquifier string
      via the kernel command line, or when the "nfs" module is
      loaded.
      (此模块参数可以通过内核命令行或在加载 "nfs" 模块时设置一个任意的唯一化后缀字符串。)

    /sys/fs/nfs/net/nfs_client/identifier
      This virtual file, available since Linux 5.3, is local to the
      network namespace in which it is accessed and so can provide
      distinction between network namespaces (containers) when the
      hostname remains uniform.
      (此虚拟文件自 Linux 5.3 起可用，它局限于访问它所在的网络命名空间，因此当主机名保持一致时，可以在不同网络命名空间(容器)之间提供区分。)

Note that this file is empty on name-space creation. If the
container system has access to some sort of per-container identity
then that uniquifier can be used. For example, a uniquifier might
be formed at boot using the container's internal identifier:
(注意，此文件在命名空间创建时为空。如果容器系统能够访问某种每容器标识，那么可以使用该唯一化后缀。例如，可以在启动时使用容器的内部标识符来构成一个唯一化后缀：)

    sha256sum /etc/machine-id | awk '{print $1}' \\
        > /sys/fs/nfs/net/nfs_client/identifier

Security considerations
(安全考虑)
-----------------------

The use of cryptographic security for lease management operations
is strongly encouraged.
(强烈建议在租约管理操作中使用加密安全机制。)

If NFS with Kerberos is not configured, a Linux NFSv4 client uses
AUTH_SYS and UID 0 as the principal part of its client identity.
This configuration is not only insecure, it increases the risk of
lease and lock stealing. However, it might be the only choice for
client configurations that have no local persistent storage.
"co_ownerid" string uniqueness and persistence is critical in this
case.
(如果未配置基于 Kerberos 的 NFS，Linux NFSv4 客户端会使用 AUTH_SYS 和 UID 0 作为其客户端身份的 principal 部分。这种配置不仅不安全，还会增加租约和锁被窃取的风险。然而，对于没有本地持久存储的客户端配置来说，这可能是唯一的选择。在这种情况下，"co_ownerid" 字符串的唯一性和持久性至关重要。)

When a Kerberos keytab is present on a Linux NFS client, the client
attempts to use one of the principals in that keytab when
identifying itself to servers. The "sec=" mount option does not
control this behavior. Alternately, a single-user client with a
Kerberos principal can use that principal in place of the client's
host principal.
(当 Linux NFS 客户端上存在 Kerberos keytab 时，客户端在向服务器表明身份时会尝试使用该 keytab 中的某个 principal。"sec=" 挂载选项并不控制此行为。另外，拥有 Kerberos principal 的单用户客户端可以使用该 principal 代替客户端的主机 principal。)

Using Kerberos for this purpose enables the client and server to
use the same lease for operations covered by all "sec=" settings.
Additionally, the Linux NFS client uses the RPCSEC_GSS security
flavor with Kerberos and the integrity QOS to prevent in-transit
modification of lease modification requests.
(为此目的使用 Kerberos 可以让客户端和服务器对所有 "sec=" 设置所涵盖的操作使用同一个租约。此外，Linux NFS 客户端将 RPCSEC_GSS 安全机制(security flavor)与 Kerberos 以及完整性 QOS 结合使用，以防止租约修改请求在传输途中被篡改。)

Additional notes
(其他说明)
----------------
The Linux NFSv4 client establishes a single lease on each NFSv4
server it accesses. NFSv4 mounts from a Linux NFSv4 client of a
particular server then share that lease.
(Linux NFSv4 客户端在其访问的每个 NFSv4 服务器上建立一个租约。来自 Linux NFSv4 客户端对特定服务器的 NFSv4 挂载随后会共享该租约。)

Once a client establishes open and lock state, the NFSv4 protocol
enables lease state to transition to other servers, following data
that has been migrated. This hides data migration completely from
running applications. The Linux NFSv4 client facilitates state
migration by presenting the same "client_owner4" to all servers it
encounters.
(一旦客户端建立了打开和锁定状态，NFSv4 协议就允许租约状态随已迁移的数据一起迁移到其他服务器。这对正在运行的应用程序完全隐藏了数据迁移。Linux NFSv4 客户端通过向其接触到的所有服务器出示相同的 "client_owner4" 来促进状态迁移。)

========
See Also
========

(参见)

  - nfs(5)
  - kerberos(7)
  - RFC 7530 for the NFSv4.0 specification
    (NFSv4.0 规范，即 RFC 7530。)
  - RFC 8881 for the NFSv4.1 specification.
    (NFSv4.1 规范，即 RFC 8881。)
