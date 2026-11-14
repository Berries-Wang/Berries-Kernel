.. SPDX-License-Identifier: GPL-2.0

============================
Ceph Distributed File System
============================

(Ceph 分布式文件系统)

Ceph is a distributed network file system designed to provide good
performance, reliability, and scalability.
(Ceph 是一个分布式网络文件系统，旨在提供良好的性能、可靠性和可扩展性。)

Basic features include:
(基本特性包括：)

 * POSIX semantics
   (POSIX 语义)
 * Seamless scaling from 1 to many thousands of nodes
   (从 1 个节点到数千个节点的无缝扩展)
 * High availability and reliability.  No single point of failure.
   (高可用性与高可靠性。没有单点故障。)
 * N-way replication of data across storage nodes
   (数据跨存储节点进行 N 路复制)
 * Fast recovery from node failures
   (从节点故障中快速恢复)
 * Automatic rebalancing of data on node addition/removal
   (在添加/移除节点时自动再平衡数据)
 * Easy deployment: most FS components are userspace daemons
   (易于部署：大多数 FS 组件都是用户空间守护进程)

Also,
(此外，)

 * Flexible snapshots (on any directory)
   (灵活的快照(在任何目录上))
 * Recursive accounting (nested files, directories, bytes)
   (递归统计(嵌套的文件、目录、字节数))

In contrast to cluster filesystems like GFS, OCFS2, and GPFS that rely
on symmetric access by all clients to shared block devices, Ceph
separates data and metadata management into independent server
clusters, similar to Lustre.  Unlike Lustre, however, metadata and
storage nodes run entirely as user space daemons.  File data is striped
across storage nodes in large chunks to distribute workload and
facilitate high throughputs.  When storage nodes fail, data is
re-replicated in a distributed fashion by the storage nodes themselves
(with some minimal coordination from a cluster monitor), making the
system extremely efficient and scalable.
(与 GFS、OCFS2 和 GPFS 这类依赖所有客户端对共享块设备进行对称访问的集群文件系统不同，Ceph 将数据与元数据管理分离到独立的服务器集群中，这一点类似于 Lustre。不过，与 Lustre 不同的是，元数据和存储节点完全作为用户空间守护进程运行。文件数据以大块为单位跨存储节点进行条带化，以分散工作负载并实现高吞吐量。当存储节点发生故障时，数据由存储节点自己以分布式方式重新复制(只需集群 monitor 进行少量协调)，使系统极为高效且可扩展。)

Metadata servers effectively form a large, consistent, distributed
in-memory cache above the file namespace that is extremely scalable,
dynamically redistributes metadata in response to workload changes,
and can tolerate arbitrary (well, non-Byzantine) node failures.  The
metadata server takes a somewhat unconventional approach to metadata
storage to significantly improve performance for common workloads.  In
particular, inodes with only a single link are embedded in
directories, allowing entire directories of dentries and inodes to be
loaded into its cache with a single I/O operation.  The contents of
extremely large directories can be fragmented and managed by
independent metadata servers, allowing scalable concurrent access.
(元数据服务器实际上在文件命名空间之上形成一个大型的、一致的、分布式的内存缓存，它具有极高的可扩展性，能够根据工作负载的变化动态地重新分配元数据，并且可以容忍任意的(好吧，是非拜占庭式的)节点故障。元数据服务器在元数据存储上采用了一种有些非传统的方式，以显著提高常见工作负载的性能。特别是，只有单一链接的 inode 被内嵌到目录中，使得整个目录的 dentry 和 inode 可以通过一次 I/O 操作加载到其缓存中。极大目录的内容可以被分片并由独立的元数据服务器管理，从而支持可扩展的并发访问。)

The system offers automatic data rebalancing/migration when scaling
from a small cluster of just a few nodes to many hundreds, without
requiring an administrator carve the data set into static volumes or
go through the tedious process of migrating data between servers.
When the file system approaches full, new nodes can be easily added
and things will "just work."
(当从只有几个节点的小集群扩展到数百个节点时，系统提供自动的数据再平衡/迁移，无需管理员将数据集划分成静态卷，也无需经历在服务器之间迁移数据的繁琐过程。当文件系统接近写满时，可以轻松添加新节点，一切将"自动正常工作"。)

Ceph includes flexible snapshot mechanism that allows a user to create
a snapshot on any subdirectory (and its nested contents) in the
system.  Snapshot creation and deletion are as simple as 'mkdir
.snap/foo' and 'rmdir .snap/foo'.
(Ceph 包含灵活的快照机制，允许用户在系统中的任何子目录(及其嵌套内容)上创建快照。快照的创建和删除就像 'mkdir .snap/foo' 和 'rmdir .snap/foo' 一样简单。)

Snapshot names have two limitations:
(快照名称有两个限制：)

* They can not start with an underscore ('_'), as these names are reserved
  for internal usage by the MDS.
  (它们不能以下划线('_')开头，因为这些名称保留供 MDS 内部使用。)
* They can not exceed 240 characters in size.  This is because the MDS makes
  use of long snapshot names internally, which follow the format:
  `_<SNAPSHOT-NAME>_<INODE-NUMBER>`.  Since filenames in general can't have
  more than 255 characters, and `<node-id>` takes 13 characters, the long
  snapshot names can take as much as 255 - 1 - 1 - 13 = 240.
  (它们的大小不能超过 240 个字符。这是因为 MDS 内部会使用长的快照名称，其遵循以下格式：`_<SNAPSHOT-NAME>_<INODE-NUMBER>`。由于文件名一般不能超过 255 个字符，而 `<node-id>` 占用 13 个字符，所以长快照名称最多可占用 255 - 1 - 1 - 13 = 240。)

Ceph also provides some recursive accounting on directories for nested files
and bytes.  You can run the commands::
(Ceph 还对目录提供针对嵌套文件和字节数的一些递归统计。你可以运行以下命令：)

 getfattr -n ceph.dir.rfiles /some/dir
 getfattr -n ceph.dir.rbytes /some/dir

to get the total number of nested files and their combined size, respectively.
This makes the identification of large disk space consumers relatively quick,
as no 'du' or similar recursive scan of the file system is required.
(分别用于获取嵌套文件的总数及其总大小。这使得识别占用大量磁盘空间的文件变得相对快捷，因为不需要 'du' 或类似的文件系统递归扫描。)

Finally, Ceph also allows quotas to be set on any directory in the system.
The quota can restrict the number of bytes or the number of files stored
beneath that point in the directory hierarchy.  Quotas can be set using
extended attributes 'ceph.quota.max_files' and 'ceph.quota.max_bytes', eg::
(最后，Ceph 还允许在系统中的任何目录上设置配额。配额可以限制在目录层级中该位置之下存储的字节数或文件数。配额可以使用扩展属性 'ceph.quota.max_files' 和 'ceph.quota.max_bytes' 来设置，例如：)

 setfattr -n ceph.quota.max_bytes -v 100000000 /some/dir
 getfattr -n ceph.quota.max_bytes /some/dir

A limitation of the current quotas implementation is that it relies on the
cooperation of the client mounting the file system to stop writers when a
limit is reached.  A modified or adversarial client cannot be prevented
from writing as much data as it needs.
(当前配额实现的一个局限是，它依赖于挂载该文件系统的客户端的配合，以便在达到限制时阻止写入者。无法阻止一个被修改过的或怀有恶意的客户端写入它所需要的任意多数据。)

Mount Syntax
(挂载语法)
============

The basic mount syntax is::
(基本的挂载语法是：)

 # mount -t ceph user@fsid.fs_name=/[subdir] mnt -o mon_addr=monip1[:port][/monip2[:port]]

You only need to specify a single monitor, as the client will get the
full list when it connects.  (However, if the monitor you specify
happens to be down, the mount won't succeed.)  The port can be left
off if the monitor is using the default.  So if the monitor is at
1.2.3.4::
(你只需要指定一个 monitor，因为客户端在连接时会获得完整的列表。(不过，如果你指定的 monitor 恰好宕机了，挂载将不会成功。)如果 monitor 使用的是默认端口，则可以省略端口。因此，如果 monitor 位于 1.2.3.4：)

 # mount -t ceph cephuser@07fe3187-00d9-42a3-814b-72a4d5e7d5be.cephfs=/ /mnt/ceph -o mon_addr=1.2.3.4

is sufficient.  If /sbin/mount.ceph is installed, a hostname can be
used instead of an IP address and the cluster FSID can be left out
(as the mount helper will fill it in by reading the ceph configuration
file)::
(就足够了。如果安装了 /sbin/mount.ceph，则可以使用主机名代替 IP 地址，并且可以省略集群 FSID(因为挂载助手会通过读取 ceph 配置文件来填写它)：)

  # mount -t ceph cephuser@cephfs=/ /mnt/ceph -o mon_addr=mon-addr

Multiple monitor addresses can be passed by separating each address with a slash (`/`)::
(可以通过用斜杠(`/`)分隔各个地址来传递多个 monitor 地址：)

  # mount -t ceph cephuser@cephfs=/ /mnt/ceph -o mon_addr=192.168.1.100/192.168.1.101

When using the mount helper, monitor address can be read from ceph
configuration file if available. Note that, the cluster FSID (passed as part
of the device string) is validated by checking it with the FSID reported by
the monitor.
(使用挂载助手时，如果 ceph 配置文件可用，monitor 地址可以从配置文件中读取。注意，集群 FSID(作为设备字符串的一部分传入)会通过与 monitor 报告的 FSID 进行比对来验证。)

Mount Options
(挂载选项)
=============

  mon_addr=ip_address[:port][/ip_address[:port]]
	Monitor address to the cluster. This is used to bootstrap the
        connection to the cluster. Once connection is established, the
        monitor addresses in the monitor map are followed.
	(集群的 monitor 地址。它用于引导建立到集群的连接。连接建立之后，将遵循 monitor map 中的 monitor 地址。)

  fsid=cluster-id
	FSID of the cluster (from `ceph fsid` command).
	(集群的 FSID(来自 `ceph fsid` 命令)。)

  ip=A.B.C.D[:N]
	Specify the IP and/or port the client should bind to locally.
	There is normally not much reason to do this.  If the IP is not
	specified, the client's IP address is determined by looking at the
	address its connection to the monitor originates from.
	(指定客户端应在本地绑定到的 IP 和/或端口。通常没有太多理由这样做。如果未指定 IP，则通过查看客户端到 monitor 的连接所源自的地址来确定客户端的 IP 地址。)

  wsize=X
	Specify the maximum write size in bytes.  Default: 64 MB.
	(指定最大写大小，以字节为单位。默认：64 MB。)

  rsize=X
	Specify the maximum read size in bytes.  Default: 64 MB.
	(指定最大读大小，以字节为单位。默认：64 MB。)

  rasize=X
	Specify the maximum readahead size in bytes.  Default: 8 MB.
	(指定最大预读大小，以字节为单位。默认：8 MB。)

  mount_timeout=X
	Specify the timeout value for mount (in seconds), in the case
	of a non-responsive Ceph file system.  The default is 60
	seconds.
	(指定挂载的超时值(以秒为单位)，用于 Ceph 文件系统无响应的情况。默认为 60 秒。)

  caps_max=X
	Specify the maximum number of caps to hold. Unused caps are released
	when number of caps exceeds the limit. The default is 0 (no limit)
	(指定要持有的最大 caps 数量。当 caps 数量超过该限制时，未使用的 caps 会被释放。默认值为 0(无限制))

  rbytes
	When stat() is called on a directory, set st_size to 'rbytes',
	the summation of file sizes over all files nested beneath that
	directory.  This is the default.
	(当对目录调用 stat() 时，将 st_size 设置为 'rbytes'，即嵌套在该目录下的所有文件的大小之和。这是默认行为。)

  norbytes
	When stat() is called on a directory, set st_size to the
	number of entries in that directory.
	(当对目录调用 stat() 时，将 st_size 设置为该目录中的条目数量。)

  nocrc
	Disable CRC32C calculation for data writes.  If set, the storage node
	must rely on TCP's error correction to detect data corruption
	in the data payload.
	(禁用对数据写的 CRC32C 计算。如果设置了此选项，存储节点必须依靠 TCP 的纠错来检测数据载荷中的数据损坏。)

  dcache
        Use the dcache contents to perform negative lookups and
        readdir when the client has the entire directory contents in
        its cache.  (This does not change correctness; the client uses
        cached metadata only when a lease or capability ensures it is
        valid.)
        (当客户端的缓存中拥有整个目录内容时，使用 dcache 内容来执行否定查找和 readdir。(这不会改变正确性；只有当租约或能力确保其有效时，客户端才会使用缓存的元数据。))

  nodcache
        Do not use the dcache as above.  This avoids a significant amount of
        complex code, sacrificing performance without affecting correctness,
        and is useful for tracking down bugs.
        (不按上面的方式使用 dcache。这避免了大量复杂的代码，在不影响正确性的情况下牺牲了性能，并且有助于查找缺陷。)

  noasyncreaddir
	Do not use the dcache as above for readdir.
	(对于 readdir，不按上面的方式使用 dcache。)

  noquotadf
        Report overall filesystem usage in statfs instead of using the root
        directory quota.
        (在 statfs 中报告整个文件系统的使用情况，而不是使用根目录配额。)

  nocopyfrom
        Don't use the RADOS 'copy-from' operation to perform remote object
        copies.  Currently, it's only used in copy_file_range, which will revert
        to the default VFS implementation if this option is used.
        (不要使用 RADOS 的 'copy-from' 操作来执行远程对象复制。目前，它只用于 copy_file_range，如果使用了此选项，copy_file_range 将回退到默认的 VFS 实现。)

  recover_session=<no|clean>
	Set auto reconnect mode in the case where the client is blocklisted. The
	available modes are "no" and "clean". The default is "no".
	(在客户端被列入黑名单的情况下设置自动重连模式。可用的模式为 "no" 和 "clean"。默认值为 "no"。)

	* no: never attempt to reconnect when client detects that it has been
	  blocklisted. Operations will generally fail after being blocklisted.
	  (no：当客户端检测到自己被列入黑名单时，绝不尝试重连。被列入黑名单后，操作通常会失败。)

	* clean: client reconnects to the ceph cluster automatically when it
	  detects that it has been blocklisted. During reconnect, client drops
	  dirty data/metadata, invalidates page caches and writable file handles.
	  After reconnect, file locks become stale because the MDS loses track
	  of them. If an inode contains any stale file locks, read/write on the
	  inode is not allowed until applications release all stale file locks.
	  (clean：当客户端检测到自己被列入黑名单时，自动重连到 ceph 集群。在重连期间，客户端会丢弃脏数据/元数据，使页缓存和可写文件句柄失效。重连之后，由于 MDS 失去了对这些锁的跟踪，文件锁会变得陈旧。如果某个 inode 包含任何陈旧的文件锁，则在应用程序释放所有陈旧文件锁之前，不允许对该 inode 进行读/写。)

More Information
(更多信息)
================

For more information on Ceph, see the home page at
(有关 Ceph 的更多信息，请参见其主页：)
	https://ceph.com/

The Linux kernel client source tree is available at
(Linux 内核客户端源代码树位于：)
	- https://github.com/ceph/ceph-client.git

and the source for the full system is at
(完整系统的源代码位于：)
	https://github.com/ceph/ceph.git
