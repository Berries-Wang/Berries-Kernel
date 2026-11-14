:orphan:

Making Filesystems Exportable
(使文件系统可导出)
=============================

Overview
(概述)
--------

All filesystem operations require a dentry (or two) as a starting
point.  Local applications have a reference-counted hold on suitable
dentries via open file descriptors or cwd/root.  However remote
applications that access a filesystem via a remote filesystem protocol
such as NFS may not be able to hold such a reference, and so need a
different way to refer to a particular dentry.  As the alternative
form of reference needs to be stable across renames, truncates, and
server-reboot (among other things, though these tend to be the most
problematic), there is no simple answer like 'filename'.
(所有文件系统操作都需要一个(或两个)dentry 作为起点。本地应用程序通过打开的文件描述符或 cwd/root 对合适的 dentry 持有引用计数。然而，通过诸如 NFS 这样的远程文件系统协议访问文件系统的远程应用程序可能无法持有这样的引用，因此需要一种不同的方式来指代特定的 dentry。由于这种替代形式的引用需要在重命名、截断和服务器重启(以及其他情形，尽管这些往往是最棘手的)之间保持稳定，所以并不存在诸如'文件名'这样简单的答案。)

The mechanism discussed here allows each filesystem implementation to
specify how to generate an opaque (outside of the filesystem) byte
string for any dentry, and how to find an appropriate dentry for any
given opaque byte string.
This byte string will be called a "filehandle fragment" as it
corresponds to part of an NFS filehandle.
(这里讨论的机制允许每个文件系统实现指定：如何为任意 dentry 生成一个不透明的(文件系统之外的)字节串，以及如何为任意给定的不透明字节串找到合适的 dentry。这个字节串将被称为"文件句柄片段(filehandle fragment)"，因为它对应 NFS 文件句柄的一部分。)

A filesystem which supports the mapping between filehandle fragments
and dentries will be termed "exportable".
(支持在文件句柄片段与 dentry 之间进行映射的文件系统将被称为"可导出的(exportable)"。)



Dcache Issues
(dcache 问题)
-------------

The dcache normally contains a proper prefix of any given filesystem
tree.  This means that if any filesystem object is in the dcache, then
all of the ancestors of that filesystem object are also in the dcache.
As normal access is by filename this prefix is created naturally and
maintained easily (by each object maintaining a reference count on
its parent).
(dcache 通常包含任何给定文件系统树的一个恰当前缀。这意味着如果某个文件系统对象位于 dcache 中，那么该文件系统对象的所有祖先也都在 dcache 中。由于正常访问是按文件名进行的，这个前缀被自然地创建并易于维护(每个对象对其父对象维护一个引用计数)。)

However when objects are included into the dcache by interpreting a
filehandle fragment, there is no automatic creation of a path prefix
for the object.  This leads to two related but distinct features of
the dcache that are not needed for normal filesystem access.
(然而，当通过解释文件句柄片段将对象纳入 dcache 时，不会自动为该对象创建路径前缀。这导致了 dcache 的两个相关但不同的特性，而正常的文件系统访问并不需要它们。)

1. The dcache must sometimes contain objects that are not part of the
   proper prefix. i.e that are not connected to the root.
   (1. dcache 有时必须包含不属于恰当前缀的对象，即未连接到根的对象。)
2. The dcache must be prepared for a newly found (via ->lookup) directory
   to already have a (non-connected) dentry, and must be able to move
   that dentry into place (based on the parent and name in the
   ->lookup).   This is particularly needed for directories as
   it is a dcache invariant that directories only have one dentry.
   (2. dcache 必须准备好应对新发现的(通过 ->lookup)目录已经拥有一个(未连接的)dentry 的情况，并且必须能够把该 dentry 移到相应位置(依据 ->lookup 中的父目录和名字)。这对目录尤为必要，因为"目录只有一个 dentry"是 dcache 的不变量。)

To implement these features, the dcache has:
(为实现这些特性，dcache 具有以下机制：)

a. A dentry flag DCACHE_DISCONNECTED which is set on
   any dentry that might not be part of the proper prefix.
   This is set when anonymous dentries are created, and cleared when a
   dentry is noticed to be a child of a dentry which is in the proper
   prefix.  If the refcount on a dentry with this flag set
   becomes zero, the dentry is immediately discarded, rather than being
   kept in the dcache.  If a dentry that is not already in the dcache
   is repeatedly accessed by filehandle (as NFSD might do), an new dentry
   will be a allocated for each access, and discarded at the end of
   the access.
   (a. 一个 dentry 标志 DCACHE_DISCONNECTED，它被设置在任何可能不属于恰当前缀的 dentry 上。它在匿名 dentry 被创建时设置，并在发现某个 dentry 是位于恰当前缀中的 dentry 的子节点时清除。如果带有此标志的 dentry 的引用计数变为零，该 dentry 会被立即丢弃，而不是保留在 dcache 中。如果一个尚未在 dcache 中的 dentry 被通过文件句柄反复访问(如 NFSD 可能会做的那样)，每次访问都会分配一个新的 dentry，并在访问结束时丢弃。)

   Note that such a dentry can acquire children, name, ancestors, etc.
   without losing DCACHE_DISCONNECTED - that flag is only cleared when
   subtree is successfully reconnected to root.  Until then dentries
   in such subtree are retained only as long as there are references;
   refcount reaching zero means immediate eviction, same as for unhashed
   dentries.  That guarantees that we won't need to hunt them down upon
   umount.
   (注意，这样的 dentry 可以获得子节点、名字、祖先等，而不会失去 DCACHE_DISCONNECTED——该标志只有在子树成功重新连接到根时才会被清除。在此之前，这种子树中的 dentry 只在引用存在期间才被保留；引用计数归零意味着立即逐出，与未哈希的 dentry 相同。这保证了我们不需要在卸载(umount)时去把它们清理掉。)

b. A primitive for creation of secondary roots - d_obtain_root(inode).
   Those do _not_ bear DCACHE_DISCONNECTED.  They are placed on the
   per-superblock list (->s_roots), so they can be located at umount
   time for eviction purposes.
   (b. 用于创建次级根(secondary roots)的原语——d_obtain_root(inode)。它们_不_带有 DCACHE_DISCONNECTED。它们被放在按超级块的列表(->s_roots)上，以便在卸载时能够找到它们进行逐出。)

c. Helper routines to allocate anonymous dentries, and to help attach
   loose directory dentries at lookup time. They are:
   (c. 用于分配匿名 dentry 以及在查找时帮助挂接松散目录 dentry 的辅助例程。它们是：)

    d_obtain_alias(inode) will return a dentry for the given inode.
      If the inode already has a dentry, one of those is returned.
      (d_obtain_alias(inode) 会返回给定 inode 对应的 dentry。如果该 inode 已经拥有一个 dentry，则返回其中之一。)

      If it doesn't, a new anonymous (IS_ROOT and
      DCACHE_DISCONNECTED) dentry is allocated and attached.
      (如果没有，则会分配并挂接一个新的匿名(IS_ROOT 且 DCACHE_DISCONNECTED)dentry。)

      In the case of a directory, care is taken that only one dentry
      can ever be attached.
      (对于目录，会注意确保永远只能挂接一个 dentry。)

    d_splice_alias(inode, dentry) will introduce a new dentry into the tree;
      either the passed-in dentry or a preexisting alias for the given inode
      (such as an anonymous one created by d_obtain_alias), if appropriate.
      It returns NULL when the passed-in dentry is used, following the calling
      convention of ->lookup.
      (d_splice_alias(inode, dentry) 会把一个新 dentry 引入树中；在合适的情况下，要么使用传入的 dentry，要么使用给定 inode 的既有别名(例如由 d_obtain_alias 创建的匿名 dentry)。当使用传入的 dentry 时它返回 NULL，遵循 ->lookup 的调用约定。)

Filesystem Issues
(文件系统方面的问题)
-----------------

For a filesystem to be exportable it must:
(文件系统要可导出，必须做到以下几点：)

   1. provide the filehandle fragment routines described below.
      (1. 提供下文所述的文件句柄片段例程。)
   2. make sure that d_splice_alias is used rather than d_add
      when ->lookup finds an inode for a given parent and name.
      (2. 确保当 ->lookup 为给定的父目录和名字找到 inode 时，使用 d_splice_alias 而不是 d_add。)

      If inode is NULL, d_splice_alias(inode, dentry) is equivalent to::
      (如果 inode 为 NULL，d_splice_alias(inode, dentry) 等价于：)

		d_add(dentry, inode), NULL

      Similarly, d_splice_alias(ERR_PTR(err), dentry) = ERR_PTR(err)
      (类似地，d_splice_alias(ERR_PTR(err), dentry) = ERR_PTR(err))

      Typically the ->lookup routine will simply end with a::
      (通常 ->lookup 例程会直接以下面的代码结束：)

		return d_splice_alias(inode, dentry);
	}



A file system implementation declares that instances of the filesystem
are exportable by setting the s_export_op field in the struct
super_block.  This field must point to a struct export_operations
which has the following members:
(文件系统实现通过设置 struct super_block 中的 s_export_op 字段，来声明该文件系统的实例是可导出的。该字段必须指向一个 struct export_operations，它具有以下成员：)

.. kernel-doc:: include/linux/exportfs.h
   :identifiers: struct export_operations

A filehandle fragment consists of an array of 1 or more 4byte words,
together with a one byte "type".
The decode_fh routine should not depend on the stated size that is
passed to it.  This size may be larger than the original filehandle
generated by encode_fh, in which case it will have been padded with
nuls.  Rather, the encode_fh routine should choose a "type" which
indicates the decode_fh how much of the filehandle is valid, and how
it should be interpreted.
(文件句柄片段由一个包含 1 个或多个 4 字节字的数组以及一个单字节的"type"组成。decode_fh 例程不应依赖于传递给它的所声明大小。该大小可能比 encode_fh 生成的原始文件句柄更大，在这种情况下它会被 nul 填充过。相反，encode_fh 例程应当选择一个"type"，向 decode_fh 表明文件句柄中有多少内容是有效的，以及应当如何解释它。)

Export Operations Flags
(导出操作标志)
-----------------------
In addition to the operation vector pointers, struct export_operations also
contains a "flags" field that allows the filesystem to communicate to nfsd
that it may want to do things differently when dealing with it. The
following flags are defined:
(除了操作向量指针之外，struct export_operations 还包含一个 "flags" 字段，允许文件系统告知 nfsd：在处理它时可能希望采取不同的做法。定义了以下标志：)

  EXPORT_OP_NOWCC - disable NFSv3 WCC attributes on this filesystem
    (EXPORT_OP_NOWCC - 在该文件系统上禁用 NFSv3 WCC 属性)
    RFC 1813 recommends that servers always send weak cache consistency
    (WCC) data to the client after each operation. The server should
    atomically collect attributes about the inode, do an operation on it,
    and then collect the attributes afterward. This allows the client to
    skip issuing GETATTRs in some situations but means that the server
    is calling vfs_getattr for almost all RPCs. On some filesystems
    (particularly those that are clustered or networked) this is expensive
    and atomicity is difficult to guarantee. This flag indicates to nfsd
    that it should skip providing WCC attributes to the client in NFSv3
    replies when doing operations on this filesystem. Consider enabling
    this on filesystems that have an expensive ->getattr inode operation,
    or when atomicity between pre and post operation attribute collection
    is impossible to guarantee.
    (RFC 1813 建议服务器在每次操作之后总是向客户端发送弱缓存一致性(WCC)数据。服务器应当原子地收集关于 inode 的属性、对其执行一个操作、然后再收集其后的属性。这允许客户端在某些情况下省略 GETATTR 请求，但意味着服务器在几乎所有 RPC 中都要调用 vfs_getattr。在某些文件系统(尤其是集群化或网络化的文件系统)上，这开销很大，而且原子性难以保证。该标志向 nfsd 表明：在对该文件系统执行操作时，应在 NFSv3 应答中跳过向客户端提供 WCC 属性。对于拥有昂贵 ->getattr inode 操作的文件系统，或在操作前后的属性收集之间无法保证原子性时，考虑启用该标志。)

  EXPORT_OP_NOSUBTREECHK - disallow subtree checking on this fs
    (EXPORT_OP_NOSUBTREECHK - 不允许在该文件系统上进行子树检查)
    Many NFS operations deal with filehandles, which the server must then
    vet to ensure that they live inside of an exported tree. When the
    export consists of an entire filesystem, this is trivial. nfsd can just
    ensure that the filehandle live on the filesystem. When only part of a
    filesystem is exported however, then nfsd must walk the ancestors of the
    inode to ensure that it's within an exported subtree. This is an
    expensive operation and not all filesystems can support it properly.
    This flag exempts the filesystem from subtree checking and causes
    exportfs to get back an error if it tries to enable subtree checking
    on it.
    (许多 NFS 操作都会处理文件句柄，服务器随后必须审核这些句柄，确保它们位于被导出的树内。当导出的是整个文件系统时，这很简单：nfsd 只需确保文件句柄位于该文件系统上。然而，当只导出文件系统的一部分时，nfsd 必须遍历该 inode 的祖先，以确保它位于被导出的子树内。这是一项开销很大的操作，并非所有文件系统都能正确支持。该标志使文件系统免于子树检查，并且当 exportfs 尝试在其上启用子树检查时会收到错误。)

  EXPORT_OP_CLOSE_BEFORE_UNLINK - always close cached files before unlinking
    (EXPORT_OP_CLOSE_BEFORE_UNLINK - 在取消链接(unlink)之前总是先关闭缓存的文件)
    On some exportable filesystems (such as NFS) unlinking a file that
    is still open can cause a fair bit of extra work. For instance,
    the NFS client will do a "sillyrename" to ensure that the file
    sticks around while it's still open. When reexporting, that open
    file is held by nfsd so we usually end up doing a sillyrename, and
    then immediately deleting the sillyrenamed file just afterward when
    the link count actually goes to zero. Sometimes this delete can race
    with other operations (for instance an rmdir of the parent directory).
    This flag causes nfsd to close any open files for this inode _before_
    calling into the vfs to do an unlink or a rename that would replace
    an existing file.
    (在某些可导出的文件系统(例如 NFS)上，取消链接一个仍被打开的文件会导致相当多的额外工作。例如，NFS 客户端会执行"sillyrename"，以确保文件在仍被打开期间得以保留。在再导出(re-export)时，该打开的文件由 nfsd 持有，所以我们通常最终会执行一次 sillyrename，随后在链接计数真正降为零时立即删除这个被 sillyrename 的文件。有时这个删除操作可能与其他操作产生竞争(例如对父目录的 rmdir)。该标志使 nfsd 在进入 vfs 执行 unlink、或执行会替换现有文件的 rename _之前_，先关闭该 inode 的所有已打开文件。)

  EXPORT_OP_REMOTE_FS - Backing storage for this filesystem is remote
    (EXPORT_OP_REMOTE_FS - 该文件系统的后备存储是远程的)
    PF_LOCAL_THROTTLE exists for loopback NFSD, where a thread needs to
    write to one bdi (the final bdi) in order to free up writes queued
    to another bdi (the client bdi). Such threads get a private balance
    of dirty pages so that dirty pages for the client bdi do not imact
    the daemon writing to the final bdi. For filesystems whose durable
    storage is not local (such as exported NFS filesystems), this
    constraint has negative consequences. EXPORT_OP_REMOTE_FS enables
    an export to disable writeback throttling.
    (PF_LOCAL_THROTTLE 是为环回 NFSD 而存在的：一个线程需要向一个 bdi(最终 bdi)写入，以释放排入另一个 bdi(客户端 bdi)的写操作。这类线程会获得一个私有的脏页配额平衡，使客户端 bdi 的脏页不会影响向最终 bdi 写入的守护进程。对于持久存储不在本地的文件系统(例如被导出的 NFS 文件系统)，这一约束会带来负面影响。EXPORT_OP_REMOTE_FS 使导出可以禁用回写限流。)

  EXPORT_OP_NOATOMIC_ATTR - Filesystem does not update attributes atomically
    (EXPORT_OP_NOATOMIC_ATTR - 文件系统不会原子地更新属性)
    EXPORT_OP_NOATOMIC_ATTR indicates that the exported filesystem
    cannot provide the semantics required by the "atomic" boolean in
    NFSv4's change_info4. This boolean indicates to a client whether the
    returned before and after change attributes were obtained atomically
    with the respect to the requested metadata operation (UNLINK,
    OPEN/CREATE, MKDIR, etc).
    (EXPORT_OP_NOATOMIC_ATTR 表示被导出的文件系统无法提供 NFSv4 的 change_info4 中 "atomic" 布尔值所需的语义。该布尔值向客户端表明：就所请求的元数据操作(UNLINK、OPEN/CREATE、MKDIR 等)而言，返回的变更前与变更后属性是否是原子获取的。)

  EXPORT_OP_FLUSH_ON_CLOSE - Filesystem flushes file data on close(2)
    (EXPORT_OP_FLUSH_ON_CLOSE - 文件系统在 close(2) 时刷新文件数据)
    On most filesystems, inodes can remain under writeback after the
    file is closed. NFSD relies on client activity or local flusher
    threads to handle writeback. Certain filesystems, such as NFS, flush
    all of an inode's dirty data on last close. Exports that behave this
    way should set EXPORT_OP_FLUSH_ON_CLOSE so that NFSD knows to skip
    waiting for writeback when closing such files.
    (在大多数文件系统上，文件关闭之后 inode 可能仍处于回写过程中。NFSD 依靠客户端活动或本地回刷线程来处理回写。某些文件系统(例如 NFS)会在最后一次关闭时刷新 inode 的所有脏数据。以这种方式运作的导出应设置 EXPORT_OP_FLUSH_ON_CLOSE，以便 NFSD 知道在关闭此类文件时跳过等待回写。)

Signed Filehandles
(带签名的文件句柄)
------------------

To protect against filehandle guessing attacks, the Linux NFS server can be
configured to sign filehandles with a Message Authentication Code (MAC).
(为防范文件句柄猜测攻击，可以将 Linux NFS 服务器配置为使用消息认证码(MAC)对文件句柄进行签名。)

Standard NFS filehandles are often predictable. If an attacker can guess
a valid filehandle for a file they do not have permission to access via
directory traversal, they may be able to bypass path-based permissions
(though they still remain subject to inode-level permissions).
(标准 NFS 文件句柄往往是可预测的。如果攻击者能够通过目录遍历猜出某个他们无权访问的文件的有效文件句柄，他们也许能够绕过基于路径的权限(不过他们仍然受 inode 级别权限的约束)。)

Signed filehandles prevent this by appending a MAC to the filehandle
before it is sent to the client. Upon receiving a filehandle back from a
client, the server re-calculates the MAC using its internal key and
verifies it against the one provided. If the signatures do not match,
the server treats the filehandle as invalid (returning NFS[34]ERR_STALE).
(带签名的文件句柄通过在文件句柄被发送给客户端之前附加一个 MAC 来防止这种情况。当从客户端收到文件句柄时，服务器使用其内部密钥重新计算 MAC，并与客户端提供的值进行比对验证。如果签名不匹配，服务器将该文件句柄视为无效(返回 NFS[34]ERR_STALE)。)

Note that signing filehandles provides integrity and authenticity but
not confidentiality. The contents of the filehandle remain visible to
the client; they simply cannot be forged or modified.
(注意，对文件句柄签名提供的是完整性和真实性，而非机密性。文件句柄的内容对客户端仍然可见；只是无法伪造或修改它们。)

Configuration
(配置)
~~~~~~~~~~~~~

To enable signed filehandles, the administrator must provide a signing
key to the kernel and enable the "sign_fh" export option.
(要启用带签名的文件句柄，管理员必须向内核提供一个签名密钥，并启用 "sign_fh" 导出选项。)

1. Providing a Key
   (1. 提供密钥)
   The signing key is managed via the nfsd netlink interface. This key
   is per-network-namespace and must be set before any exports using
   "sign_fh" become active.
   (签名密钥通过 nfsd netlink 接口管理。该密钥按网络命名空间设置，并且必须在使用 "sign_fh" 的任何导出生效之前设置好。)

2. Export Options
   (2. 导出选项)
   The feature is controlled on a per-export basis in /etc/exports:
   (该特性在 /etc/exports 中按导出逐项控制：)

   sign_fh
     Enables signing for all filehandles generated under this export.
     (为该导出下生成的所有文件句柄启用签名。)

   no_sign_fh
     (Default) Disables signing.
     ((默认)禁用签名。)

Key Management and Rotation
(密钥管理与轮换)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The security of this mechanism relies entirely on the secrecy of the
signing key.
(该机制的安全性完全依赖于签名密钥的保密性。)

Initial Setup:
  The key should be generated using a high-quality random source and
  loaded early in the boot process or during the nfs-server startup
  sequence.
  (初始设置：密钥应使用高质量的随机源生成，并在引导过程的早期或 nfs-server 启动序列期间加载。)

Changing Keys:
  If a key is changed while clients have active mounts, existing
  filehandles held by those clients will become invalid, resulting in
  "Stale file handle" errors on the client side.
  (更换密钥：如果在客户端拥有活动挂载的情况下更换密钥，这些客户端持有的现有文件句柄将变为无效，导致客户端一侧出现 "Stale file handle" 错误。)

Safe Rotation:
  Currently, there is no mechanism for "graceful" key rotation
  (maintaining multiple valid keys). Changing the key is an atomic
  operation that immediately invalidates all previous signatures.
  (安全轮换：目前没有"平滑"轮换密钥的机制(即同时维护多个有效密钥)。更换密钥是一个原子操作，会立即使所有先前的签名失效。)

Transitioning Exports
(导出项的切换)
~~~~~~~~~~~~~~~~~~~~~

When adding or removing the "sign_fh" flag from an active export, the
following behaviors should be expected:
(在活动导出上添加或移除 "sign_fh" 标志时，应预期出现以下行为：)

+-------------------+---------------------------------------------------+
| Change (变更)     | Result for Existing Clients (对现有客户端的影响)  |
+===================+===================================================+
| Adding sign_fh    | Clients holding unsigned filehandles will find    |
| (添加 sign_fh)    | them rejected, as the server now expects a        |
|                   | signature. (未签名句柄将被拒，服务器要求签名。)   |
+-------------------+---------------------------------------------------+
| Removing sign_fh  | Clients holding signed filehandles will find them |
| (移除 sign_fh)    | rejected, as the server now expects the           |
|                   | filehandle to end at its traditional boundary     |
|                   | without a MAC. (已签名句柄将被拒，要求无 MAC。)   |
+-------------------+---------------------------------------------------+

Because filehandles are often cached persistently by clients, adding or
removing this option should generally be done during a scheduled maintenance
window involving a NFS client unmount/remount.
(由于客户端常常会持久地缓存文件句柄，添加或移除该选项通常应在计划好的维护窗口内进行，并涉及 NFS 客户端的卸载/重新挂载。)
