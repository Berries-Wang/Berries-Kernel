.. SPDX-License-Identifier: GPL-2.0

================
OCFS2 filesystem
================

(OCFS2 文件系统)

OCFS2 is a general purpose extent based shared disk cluster file
system with many similarities to ext3. It supports 64 bit inode
numbers, and has automatically extending metadata groups which may
also make it attractive for non-clustered use.
(OCFS2 是一种通用的基于区段(extent)的共享磁盘集群文件系统，与 ext3 有许多相似之处。它支持 64 位 inode 号，并拥有可自动扩展的元数据组，这也可能使其对非集群场景具有吸引力。)

You'll want to install the ocfs2-tools package in order to at least
get "mount.ocfs2" and "ocfs2_hb_ctl".
(你需要安装 ocfs2-tools 软件包，至少可以从中得到 "mount.ocfs2" 和 "ocfs2_hb_ctl"。)

Project web page:    http://ocfs2.wiki.kernel.org
Tools git tree:      https://github.com/markfasheh/ocfs2-tools
OCFS2 mailing lists: https://subspace.kernel.org/lists.linux.dev.html
(项目网页: http://ocfs2.wiki.kernel.org；工具 git 树: https://github.com/markfasheh/ocfs2-tools；OCFS2 邮件列表: https://subspace.kernel.org/lists.linux.dev.html)

All code copyright 2005 Oracle except when otherwise noted.
(除另有注明外，所有代码版权归 2005 Oracle 所有。)

Credits
(致谢)
=======

Lots of code taken from ext3 and other projects.
(大量代码取自 ext3 及其他项目。)

Authors in alphabetical order:
(作者按字母顺序排列：)

- Joel Becker   <joel.becker@oracle.com>
- Zach Brown    <zach.brown@oracle.com>
- Mark Fasheh   <mfasheh@suse.com>
- Kurt Hackel   <kurt.hackel@oracle.com>
- Tao Ma        <tao.ma@oracle.com>
- Sunil Mushran <sunil.mushran@oracle.com>
- Manish Singh  <manish.singh@oracle.com>
- Tiger Yang    <tiger.yang@oracle.com>

Caveats
(注意事项)
=======
Features which OCFS2 does not support yet:
(OCFS2 尚不支持的功能：)

	- Directory change notification (F_NOTIFY)
	  (目录更改通知(F_NOTIFY))
	- Distributed Caching (F_SETLEASE/F_GETLEASE/break_lease)
	  (分布式缓存(F_SETLEASE/F_GETLEASE/break_lease))

Mount options
(挂载选项)
=============

OCFS2 supports the following mount options:
(OCFS2 支持以下挂载选项：)

(*) == default
((*) 表示默认值)

======================= ========================================================
barrier=1		This enables/disables barriers. barrier=0 disables it,
			barrier=1 enables it.(启用/禁用屏障。barrier=0 禁用它，barrier=1 启用它。)
errors=remount-ro(*)	Remount the filesystem read-only on an error.(发生错误时以只读方式重新挂载文件系统。)
errors=panic		Panic and halt the machine if an error occurs.(如果发生错误则触发恐慌并停机。)
intr		(*)	Allow signals to interrupt cluster operations.(允许信号中断集群操作。)
nointr			Do not allow signals to interrupt cluster
			operations.(不允许信号中断集群操作。)
noatime			Do not update access time.(不更新访问时间。)
relatime(*)		Update atime if the previous atime is older than
			mtime or ctime(如果先前的 atime 早于 mtime 或 ctime，则更新 atime)
strictatime		Always update atime, but the minimum update interval
			is specified by atime_quantum.(总是更新 atime，但最小更新间隔由 atime_quantum 指定。)
atime_quantum=60(*)	OCFS2 will not update atime unless this number
			of seconds has passed since the last update.
			Set to zero to always update atime. This option need
			work with strictatime.(OCFS2 不会更新 atime，除非自上次更新以来已过此秒数。设为零则总是更新 atime。此选项需与 strictatime 配合使用。)
data=ordered	(*)	All data are forced directly out to the main file
			system prior to its metadata being committed to the
			journal.(所有数据在其元数据提交到日志之前被直接强制写出到主文件系统。)
data=writeback		Data ordering is not preserved, data may be written
			into the main file system after its metadata has been
			committed to the journal.(不保持数据顺序，数据可能在其元数据已提交到日志之后才被写入主文件系统。)
preferred_slot=0(*)	During mount, try to use this filesystem slot first. If
			it is in use by another node, the first empty one found
			will be chosen. Invalid values will be ignored.(挂载时，首先尝试使用此文件系统槽位。如果它正被另一个节点使用，则选择找到的第一个空槽位。无效的值将被忽略。)
commit=nrsec	(*)	Ocfs2 can be told to sync all its data and metadata
			every 'nrsec' seconds. The default value is 5 seconds.
			This means that if you lose your power, you will lose
			as much as the latest 5 seconds of work (your
			filesystem will not be damaged though, thanks to the
			journaling).  This default value (or any low value)
			will hurt performance, but it's good for data-safety.
			Setting it to 0 will have the same effect as leaving
			it at the default (5 seconds).
			Setting it to very large values will improve
			performance.(可以告知 Ocfs2 每隔 'nrsec' 秒同步其所有数据和元数据。默认值为 5 秒。这意味着如果你断电，你最多会丢失最近 5 秒的工作(不过得益于日志，你的文件系统不会损坏)。此默认值(或任何较低的值)会损害性能，但有利于数据安全。将其设为 0 的效果与保持默认值(5 秒)相同。将其设为非常大的值将提高性能。)
localalloc=8(*)		Allows custom localalloc size in MB. If the value is too
			large, the fs will silently revert it to the default.(允许以 MB 为单位自定义 localalloc 大小。如果该值太大，文件系统将静默地将其恢复为默认值。)
localflocks		This disables cluster aware flock.(这会禁用集群感知的 flock。)
inode64			Indicates that Ocfs2 is allowed to create inodes at
			any location in the filesystem, including those which
			will result in inode numbers occupying more than 32
			bits of significance.(表示允许 Ocfs2 在文件系统中的任何位置创建 inode，包括那些会导致 inode 号占用超过 32 个有效位的位置。)
user_xattr	(*)	Enables Extended User Attributes.(启用扩展用户属性。)
nouser_xattr		Disables Extended User Attributes.(禁用扩展用户属性。)
acl			Enables POSIX Access Control Lists support.(启用 POSIX 访问控制列表支持。)
noacl		(*)	Disables POSIX Access Control Lists support.(禁用 POSIX 访问控制列表支持。)
resv_level=2	(*)	Set how aggressive allocation reservations will be.
			Valid values are between 0 (reservations off) to 8
			(maximum space for reservations).(设置分配预留的激进程度。有效值介于 0(关闭预留)到 8(预留的最大空间)之间。)
dir_resv_level=	(*)	By default, directory reservations will scale with file
			reservations - users should rarely need to change this
			value. If allocation reservations are turned off, this
			option will have no effect.(默认情况下，目录预留会随文件预留而伸缩——用户应该很少需要更改此值。如果关闭了分配预留，此选项将不起作用。)
coherency=full  (*)	Disallow concurrent O_DIRECT writes, cluster inode
			lock will be taken to force other nodes drop cache,
			therefore full cluster coherency is guaranteed even
			for O_DIRECT writes.(禁止并发 O_DIRECT 写入，将获取集群 inode 锁以强制其他节点丢弃缓存，因此即使对于 O_DIRECT 写入也能保证完全的集群一致性。)
coherency=buffered	Allow concurrent O_DIRECT writes without EX lock among
			nodes, which gains high performance at risk of getting
			stale data on other nodes.(允许节点之间在没有 EX 锁的情况下进行并发 O_DIRECT 写入，这获得了高性能，但存在在其他节点上读到陈旧数据的风险。)
journal_async_commit	Commit block can be written to disk without waiting
			for descriptor blocks. If enabled older kernels cannot
			mount the device. This will enable 'journal_checksum'
			internally.(提交块可以在不等待描述符块的情况下写入磁盘。如果启用，较旧的内核将无法挂载该设备。这会在内部启用 'journal_checksum'。)
======================= ========================================================
