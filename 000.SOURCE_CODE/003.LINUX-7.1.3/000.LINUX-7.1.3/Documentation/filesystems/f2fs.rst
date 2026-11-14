.. SPDX-License-Identifier: GPL-2.0

=================================
Flash-Friendly File System (F2FS)
=================================

(闪存友好文件系统 (F2FS))

Overview
(概述)
========

NAND flash memory-based storage devices, such as SSD, eMMC, and SD cards, have
been equipped on a variety systems ranging from mobile to server systems. Since
they are known to have different characteristics from the conventional rotating
disks, a file system, an upper layer to the storage device, should adapt to the
changes from the sketch in the design level.
(基于 NAND 闪存的存储设备，例如 SSD、eMMC 和 SD 卡，已被配备到从移动系统到服务器系统的各种系统上。由于人们已知这类设备与传统旋转磁盘具有不同的特性，作为存储设备上层的文件系统，应当在设计层面上从头开始适应这些变化。)

F2FS is a file system exploiting NAND flash memory-based storage devices, which
is based on Log-structured File System (LFS). The design has been focused on
addressing the fundamental issues in LFS, which are snowball effect of wandering
tree and high cleaning overhead.
(F2FS 是一种利用基于 NAND 闪存的存储设备的文件系统，它基于日志结构文件系统(LFS)。其设计一直聚焦于解决 LFS 的根本问题，即漫游树的雪球效应和高昂的清理开销。)

Since a NAND flash memory-based storage device shows different characteristic
according to its internal geometry or flash memory management scheme, namely FTL,
F2FS and its tools support various parameters not only for configuring on-disk
layout, but also for selecting allocation and cleaning algorithms.
(基于 NAND 闪存的存储设备会因其内部几何结构或闪存管理方案(即 FTL)的不同而呈现不同的特性，因此 F2FS 及其工具支持各种参数，不仅可用于配置磁盘上(on-disk)布局，还可用于选择分配和清理算法。)

The following git tree provides the file system formatting tool (mkfs.f2fs),
a consistency checking tool (fsck.f2fs), and a debugging tool (dump.f2fs).
(以下 git 树提供了文件系统格式化工具(mkfs.f2fs)、一致性检查工具(fsck.f2fs)和调试工具(dump.f2fs)。)

- git://git.kernel.org/pub/scm/linux/kernel/git/jaegeuk/f2fs-tools.git

For sending patches, please use the following mailing list:
(提交补丁时，请使用以下邮件列表：)

- linux-f2fs-devel@lists.sourceforge.net

For reporting bugs, please use the following f2fs bug tracker link:
(报告 bug 时，请使用以下 f2fs bug 跟踪器链接：)

- https://bugzilla.kernel.org/enter_bug.cgi?product=File%20System&component=f2fs

Background and Design issues
(背景与设计问题)
============================

Log-structured File System (LFS)
(日志结构文件系统(LFS))
--------------------------------
"A log-structured file system writes all modifications to disk sequentially in
a log-like structure, thereby speeding up  both file writing and crash recovery.
The log is the only structure on disk; it contains indexing information so that
files can be read back from the log efficiently. In order to maintain large free
areas on disk for fast writing, we divide  the log into segments and use a
segment cleaner to compress the live information from heavily fragmented
segments." from Rosenblum, M. and Ousterhout, J. K., 1992, "The design and
implementation of a log-structured file system", ACM Trans. Computer Systems
10, 1, 26–52.
("日志结构文件系统将所有修改以类似日志的结构顺序地写入磁盘，从而同时加快了文件写入和崩溃恢复的速度。日志是磁盘上唯一的结构；它包含索引信息，使得文件可以高效地从日志中读回。为了在磁盘上维护大片空闲区域以便快速写入，我们将日志划分为段，并使用段清理器来压缩严重碎片化的段中的存活信息。"摘自 Rosenblum, M. and Ousterhout, J. K., 1992, "The design and implementation of a log-structured file system", ACM Trans. Computer Systems 10, 1, 26–52。)

Wandering Tree Problem
(漫游树问题)
----------------------
In LFS, when a file data is updated and written to the end of log, its direct
pointer block is updated due to the changed location. Then the indirect pointer
block is also updated due to the direct pointer block update. In this manner,
the upper index structures such as inode, inode map, and checkpoint block are
also updated recursively. This problem is called as wandering tree problem [1],
and in order to enhance the performance, it should eliminate or relax the update
propagation as much as possible.
(在 LFS 中，当文件数据被更新并写到日志末尾时，其直接指针块会因位置改变而被更新。接着，间接指针块也会因直接指针块的更新而被更新。以这种方式，上层的索引结构，例如 inode、inode 映射和检查点块，也会被递归地更新。这个问题被称为漫游树问题[1]，为了提升性能，应当尽可能地消除或放松这种更新传播。)

[1] Bityutskiy, A. 2005. JFFS3 design issues. http://www.linux-mtd.infradead.org/

Cleaning Overhead
(清理开销)
-----------------
Since LFS is based on out-of-place writes, it produces so many obsolete blocks
scattered across the whole storage. In order to serve new empty log space, it
needs to reclaim these obsolete blocks seamlessly to users. This job is called
as a cleaning process.
(由于 LFS 基于异位写入(out-of-place writes)，它会产生大量散布在整个存储设备上的过时块。为了提供新的空日志空间，需要无缝地向用户回收这些过时块。这项工作被称为清理(clean)过程。)

The process consists of three operations as follows.
(该过程由以下三个操作组成。)

1. A victim segment is selected through referencing segment usage table.
   (通过引用段使用表来选择受害段。)
2. It loads parent index structures of all the data in the victim identified by
   segment summary blocks.
   (它加载受害段中由段摘要块标识的所有数据的父索引结构。)
3. It checks the cross-reference between the data and its parent index structure.
   (它检查数据与其父索引结构之间的交叉引用。)
4. It moves valid data selectively.
   (它有选择地迁移有效数据。)

This cleaning job may cause unexpected long delays, so the most important goal
is to hide the latencies to users. And also definitely, it should reduce the
amount of valid data to be moved, and move them quickly as well.
(这种清理工作可能导致意外的长延迟，因此最重要的目标是向用户隐藏这些延迟。此外，可以肯定的是，它还应减少要迁移的有效数据量，并快速地迁移它们。)

Key Features
(关键特性)
============

Flash Awareness
(闪存感知)
---------------

- Enlarge the random write area for better performance, but provide the high
  spatial locality
  (扩大随机写入区域以获得更好的性能，同时提供高空间局部性。)
- Align FS data structures to the operational units in FTL as best efforts
  (尽力将 FS 数据结构与 FTL 的操作单元对齐。)

Wandering Tree Problem
(漫游树问题)
----------------------

- Use a term, “node”, that represents inodes as well as various pointer blocks
  (使用一个术语“node”(节点)，它既代表 inode，也代表各种指针块。)
- Introduce Node Address Table (NAT) containing the locations of all the “node”
  blocks; this will cut off the update propagation.
  (引入包含所有“node”块位置的节点地址表(NAT)；这将切断更新传播。)

Cleaning Overhead
(清理开销)
-----------------

- Support a background cleaning process
  (支持后台清理过程。)
- Support greedy and cost-benefit algorithms for victim selection policies
  (为受害段选择策略支持贪婪(greedy)和代价收益(cost-benefit)算法。)
- Support multi-head logs for static/dynamic hot and cold data separation
  (支持多头部日志(multi-head logs)以实现静态/动态热冷数据分离。)
- Introduce adaptive logging for efficient block allocation
  (引入自适应日志(adaptive logging)以实现高效的块分配。)

Mount Options
(挂载选项)
=============


======================== ============================================================
background_gc=%s	 Turn on/off cleaning operations, namely garbage
			 collection, triggered in background when I/O subsystem is
			 idle. If background_gc=on, it will turn on the garbage
			 collection and if background_gc=off, garbage collection
			 will be turned off. If background_gc=sync, it will turn
			 on synchronous garbage collection running in background.
			 Default value for this option is on. So garbage
			 collection is on by default. (打开/关闭清理操作，即垃圾回收(garbage collection)，在 I/O 子系统空闲时于后台触发。如果 background_gc=on，将打开垃圾回收；如果 background_gc=off，垃圾回收将被关闭。如果 background_gc=sync，将打开在后台运行的同步垃圾回收。该选项的默认值为 on。因此垃圾回收默认是开启的。)
gc_merge		 When background_gc is on, this option can be enabled to
			 let background GC thread to handle foreground GC requests,
			 it can eliminate the sluggish issue caused by slow foreground
			 GC operation when GC is triggered from a process with limited
			 I/O and CPU resources. (当 background_gc 开启时，可以启用此选项，让后台 GC 线程处理前台 GC 请求，从而消除当从 I/O 和 CPU 资源受限的进程触发 GC 时，由缓慢的前台 GC 操作导致的迟滞问题。)
nogc_merge		 Disable GC merge feature. (禁用 GC 合并特性。)
disable_roll_forward	 Disable the roll-forward recovery routine (禁用前滚恢复(roll-forward recovery)例程。)
norecovery		 Disable the roll-forward recovery routine, mounted read-
			 only (i.e., -o ro,disable_roll_forward) (禁用前滚恢复例程，以只读方式挂载(即 -o ro,disable_roll_forward)。)
discard/nodiscard	 Enable/disable real-time discard in f2fs, if discard is
			 enabled, f2fs will issue discard/TRIM commands when a
			 segment is cleaned. (在 f2fs 中启用/禁用实时 discard；如果启用了 discard，f2fs 会在段被清理时发出 discard/TRIM 命令。)
heap/no_heap		 Deprecated. (已弃用。)
nouser_xattr		 Disable Extended User Attributes. Note: xattr is enabled
			 by default if CONFIG_F2FS_FS_XATTR is selected. (禁用扩展用户属性。注意：如果选择了 CONFIG_F2FS_FS_XATTR，xattr 默认启用。)
noacl			 Disable POSIX Access Control List. Note: acl is enabled
			 by default if CONFIG_F2FS_FS_POSIX_ACL is selected. (禁用 POSIX 访问控制列表。注意：如果选择了 CONFIG_F2FS_FS_POSIX_ACL，acl 默认启用。)
active_logs=%u		 Support configuring the number of active logs. In the
			 current design, f2fs supports only 2, 4, and 6 logs.
			 Default number is 6. (支持配置活动日志的数量。在当前设计中，f2fs 仅支持 2、4 和 6 个日志。默认数量为 6。)
disable_ext_identify	 Disable the extension list configured by mkfs, so f2fs
			 is not aware of cold files such as media files. (禁用由 mkfs 配置的扩展名列表，因此 f2fs 将无法感知诸如媒体文件之类的冷文件。)
inline_xattr		 Enable the inline xattrs feature. (启用内联 xattr 特性。)
noinline_xattr		 Disable the inline xattrs feature. (禁用内联 xattr 特性。)
inline_xattr_size=%u	 Support configuring inline xattr size, it depends on
			 flexible inline xattr feature. (支持配置内联 xattr 大小，它依赖于灵活内联 xattr 特性。)
inline_data		 Enable the inline data feature: Newly created small (<~3.4k)
			 files can be written into inode block. (启用内联数据特性：新创建的小文件(<~3.4k)可以写入 inode 块。)
inline_dentry		 Enable the inline dir feature: data in newly created
			 directory entries can be written into inode block. The
			 space of inode block which is used to store inline
			 dentries is limited to ~3.4k. (启用内联目录项特性：新建目录项中的数据可以写入 inode 块。用于存储内联 dentry 的 inode 块空间被限制在 ~3.4k 以内。)
noinline_dentry		 Disable the inline dentry feature. (禁用内联 dentry 特性。)
flush_merge		 Merge concurrent cache_flush commands as much as possible
			 to eliminate redundant command issues. If the underlying
			 device handles the cache_flush command relatively slowly,
			 recommend to enable this option. (尽可能合并并发的 cache_flush 命令，以消除冗余的命令下发。如果底层设备处理 cache_flush 命令相对较慢，建议启用此选项。)
nobarrier		 This option can be used if underlying storage guarantees
			 its cached data should be written to the novolatile area.
			 If this option is set, no cache_flush commands are issued
			 but f2fs still guarantees the write ordering of all the
			 data writes. (如果底层存储保证其缓存数据应被写入非易失区域，可以使用此选项。如果设置了此选项，将不会发出 cache_flush 命令，但 f2fs 仍会保证所有数据写入的写入顺序。)
barrier			 If this option is set, cache_flush commands are allowed to be
			 issued. (如果设置了此选项，则允许发出 cache_flush 命令。)
fastboot		 This option is used when a system wants to reduce mount
			 time as much as possible, even though normal performance
			 can be sacrificed. (当系统希望尽可能缩短挂载时间时使用此选项，即使可能会牺牲正常性能。)
extent_cache		 Enable an extent cache based on rb-tree, it can cache
			 as many as extent which map between contiguous logical
			 address and physical address per inode, resulting in
			 increasing the cache hit ratio. Set by default. (启用基于 rb-tree 的 extent 缓存，它可以缓存每个 inode 上尽可能多的、介于连续逻辑地址与物理地址之间的 extent，从而提高缓存命中率。默认设置。)
noextent_cache		 Disable an extent cache based on rb-tree explicitly, see
			 the above extent_cache mount option. (显式禁用基于 rb-tree 的 extent 缓存，参见上面的 extent_cache 挂载选项。)
noinline_data		 Disable the inline data feature, inline data feature is
			 enabled by default. (禁用内联数据特性；内联数据特性默认启用。)
data_flush		 Enable data flushing before checkpoint in order to
			 persist data of regular and symlink. (在检查点之前启用数据刷写，以持久化普通文件和符号链接的数据。)
reserve_root=%d		 Support configuring reserved space which is used for
			 allocation from a privileged user with specified uid or
			 gid, unit: 4KB, the default limit is 12.5% of user blocks. (支持配置保留空间，供具有指定 uid 或 gid 的特权用户分配使用，单位：4KB，默认上限为用户块的 12.5%。)
reserve_node=%d		 Support configuring reserved nodes which are used for
			 allocation from a privileged user with specified uid or
			 gid, the default limit is 12.5% of all nodes. (支持配置保留节点，供具有指定 uid 或 gid 的特权用户分配使用，默认上限为所有节点的 12.5%。)
resuid=%d		 The user ID which may use the reserved blocks and nodes. (可以使用保留块和保留节点的用户 ID。)
resgid=%d		 The group ID which may use the reserved blocks and nodes. (可以使用保留块和保留节点的组 ID。)
fault_injection=%d	 Enable fault injection in all supported types with
			 specified injection rate. (以指定的注入率启用所有支持类型的故障注入。)
fault_type=%d		 Support configuring fault injection type, should be
			 enabled with fault_injection option, fault type value
			 is shown below, it supports single or combined type. (支持配置故障注入类型，应与 fault_injection 选项一同启用，故障类型值如下所示，支持单一或组合类型。)

			 .. code-block:: none

			     ===========================      ==========
			     Type_Name                        Type_Value
			     ===========================      ==========
			     FAULT_KMALLOC                    0x00000001
			     FAULT_KVMALLOC                   0x00000002
			     FAULT_PAGE_ALLOC                 0x00000004
			     FAULT_PAGE_GET                   0x00000008
			     FAULT_ALLOC_BIO                  0x00000010 (obsolete)
			     FAULT_ALLOC_NID                  0x00000020
			     FAULT_ORPHAN                     0x00000040
			     FAULT_BLOCK                      0x00000080
			     FAULT_DIR_DEPTH                  0x00000100
			     FAULT_EVICT_INODE                0x00000200
			     FAULT_TRUNCATE                   0x00000400
			     FAULT_READ_IO                    0x00000800
			     FAULT_CHECKPOINT                 0x00001000
			     FAULT_DISCARD                    0x00002000 (obsolete)
			     FAULT_WRITE_IO                   0x00004000
			     FAULT_SLAB_ALLOC                 0x00008000
			     FAULT_DQUOT_INIT                 0x00010000
			     FAULT_LOCK_OP                    0x00020000
			     FAULT_BLKADDR_VALIDITY           0x00040000
			     FAULT_BLKADDR_CONSISTENCE        0x00080000
			     FAULT_NO_SEGMENT                 0x00100000
			     FAULT_INCONSISTENT_FOOTER        0x00200000
			     FAULT_ATOMIC_TIMEOUT             0x00400000 (1000ms)
			     FAULT_VMALLOC                    0x00800000
			     FAULT_LOCK_TIMEOUT               0x01000000 (1000ms)
			     FAULT_SKIP_WRITE                 0x02000000
			     ===========================      ==========
mode=%s			 Control block allocation mode which supports "adaptive"
			 and "lfs". In "lfs" mode, there should be no random
			 writes towards main area.
			 "fragment:segment" and "fragment:block" are newly added here.
			 These are developer options for experiments to simulate filesystem
			 fragmentation/after-GC situation itself. The developers use these
			 modes to understand filesystem fragmentation/after-GC condition well,
			 and eventually get some insights to handle them better.
			 In "fragment:segment", f2fs allocates a new segment in random
			 position. With this, we can simulate the after-GC condition.
			 In "fragment:block", we can scatter block allocation with
			 "max_fragment_chunk" and "max_fragment_hole" sysfs nodes.
			 We added some randomness to both chunk and hole size to make
			 it close to realistic IO pattern. So, in this mode, f2fs will allocate
			 1..<max_fragment_chunk> blocks in a chunk and make a hole in the
			 length of 1..<max_fragment_hole> by turns. With this, the newly
			 allocated blocks will be scattered throughout the whole partition.
			 Note that "fragment:block" implicitly enables "fragment:segment"
			 option for more randomness.
			 Please, use these options for your experiments and we strongly
			 recommend to re-format the filesystem after using these options. (控制块分配模式，支持 "adaptive" 和 "lfs"。在 "lfs" 模式下，对 main 区域不应有随机写入。"fragment:segment" 和 "fragment:block" 是此处新增的。这些是开发者用于实验以模拟文件系统碎片化/GC 后状况的选项。开发者使用这些模式以便很好地理解文件系统碎片化/GC 后的状况，并最终获得一些洞察以更好地处理它们。在 "fragment:segment" 中，f2fs 在随机位置分配新段。这样我们可以模拟 GC 后的状况。在 "fragment:block" 中，我们可以通过 "max_fragment_chunk" 和 "max_fragment_hole" sysfs 节点来分散块分配。我们为块(chunk)和空洞(hole)的大小都加入了一些随机性，使其接近真实的 IO 模式。因此，在此模式下，f2fs 会轮流在一个 chunk 中分配 1..<max_fragment_chunk> 个块，并产生长度为 1..<max_fragment_hole> 的空洞。这样，新分配的块将散布在整个分区中。注意 "fragment:block" 会隐式启用 "fragment:segment" 选项以获得更多随机性。请在你的实验中使用这些选项，我们强烈建议在使用这些选项之后重新格式化文件系统。)
usrquota		 Enable plain user disk quota accounting. (启用普通用户磁盘配额统计。)
grpquota		 Enable plain group disk quota accounting. (启用普通组磁盘配额统计。)
prjquota		 Enable plain project quota accounting. (启用普通项目磁盘配额统计。)
usrjquota=<file>	 Appoint specified file and type during mount, so that quota
grpjquota=<file>	 information can be properly updated during recovery flow,
prjjquota=<file>	 <quota file>: must be in root directory;
jqfmt=<quota type>	 <quota type>: [vfsold,vfsv0,vfsv1]. (挂载时指定特定的文件和类型，以便在恢复流程中正确更新配额信息；<quota file>: 必须位于根目录；<quota type>: [vfsold,vfsv0,vfsv1]。)
usrjquota=		 Turn off user journalled quota. (关闭用户日志式配额(journalled quota)。)
grpjquota=		 Turn off group journalled quota. (关闭组日志式配额。)
prjjquota=		 Turn off project journalled quota. (关闭项目日志式配额。)
quota			 Enable plain user disk quota accounting. (启用普通用户磁盘配额统计。)
noquota			 Disable all plain disk quota option. (禁用所有普通磁盘配额选项。)
alloc_mode=%s		 Adjust block allocation policy, which supports "reuse"
			 and "default". (调整块分配策略，支持 "reuse" 和 "default"。)
fsync_mode=%s		 Control the policy of fsync. Currently supports "posix",
			 "strict", and "nobarrier". In "posix" mode, which is
			 default, fsync will follow POSIX semantics and does a
			 light operation to improve the filesystem performance.
			 In "strict" mode, fsync will be heavy and behaves in line
			 with xfs, ext4 and btrfs, where xfstest generic/342 will
			 pass, but the performance will regress. "nobarrier" is
			 based on "posix", but doesn't issue flush command for
			 non-atomic files likewise "nobarrier" mount option. (控制 fsync 的策略。当前支持 "posix"、"strict" 和 "nobarrier"。在 "posix" 模式(默认)下，fsync 遵循 POSIX 语义，执行轻量操作以提高文件系统性能。在 "strict" 模式下，fsync 将是重型的，行为与 xfs、ext4 和 btrfs 一致，xfstest generic/342 将通过，但性能会回退。"nobarrier" 基于 "posix"，但对非原子文件不发出 flush 命令，类似于 "nobarrier" 挂载选项。)
test_dummy_encryption
test_dummy_encryption=%s
			 Enable dummy encryption, which provides a fake fscrypt
			 context. The fake fscrypt context is used by xfstests.
			 The argument may be either "v1" or "v2", in order to
			 select the corresponding fscrypt policy version. (启用哑加密(dummy encryption)，它提供一个伪造的 fscrypt 上下文。伪造的 fscrypt 上下文由 xfstests 使用。参数可以是 "v1" 或 "v2"，以选择相应的 fscrypt 策略版本。)
checkpoint=%s[:%u[%]]	 Set to "disable" to turn off checkpointing. Set to "enable"
			 to re-enable checkpointing. Is enabled by default. While
			 disabled, any unmounting or unexpected shutdowns will cause
			 the filesystem contents to appear as they did when the
			 filesystem was mounted with that option.
			 While mounting with checkpoint=disable, the filesystem must
			 run garbage collection to ensure that all available space can
			 be used. If this takes too much time, the mount may return
			 EAGAIN. You may optionally add a value to indicate how much
			 of the disk you would be willing to temporarily give up to
			 avoid additional garbage collection. This can be given as a
			 number of blocks, or as a percent. For instance, mounting
			 with checkpoint=disable:100% would always succeed, but it may
			 hide up to all remaining free space. The actual space that
			 would be unusable can be viewed at /sys/fs/f2fs/<disk>/unusable
			 This space is reclaimed once checkpoint=enable. (设置为 "disable" 以关闭检查点。设置为 "enable" 以重新启用检查点。默认启用。在禁用期间，任何卸载或意外关机都会使文件系统内容呈现为使用该选项挂载文件系统时的状态。以 checkpoint=disable 挂载时，文件系统必须运行垃圾回收，以确保所有可用空间都可以被使用。如果耗时过长，挂载可能返回 EAGAIN。你可以选择添加一个值，表示你愿意临时放弃多少磁盘空间以避免额外的垃圾回收。该值可以以块数或百分比形式给出。例如，以 checkpoint=disable:100% 挂载将总是成功，但它可能隐藏最多全部剩余的空闲空间。实际将不可用的空间可以在 /sys/fs/f2fs/<disk>/unusable 处查看。一旦 checkpoint=enable，该空间即被回收。)
checkpoint_merge	 When checkpoint is enabled, this can be used to create a kernel
			 daemon and make it to merge concurrent checkpoint requests as
			 much as possible to eliminate redundant checkpoint issues. Plus,
			 we can eliminate the sluggish issue caused by slow checkpoint
			 operation when the checkpoint is done in a process context in
			 a cgroup having low i/o budget and cpu shares. To make this
			 do better, we set the default i/o priority of the kernel daemon
			 to "3", to give one higher priority than other kernel threads.
			 This is the same way to give a I/O priority to the jbd2
			 journaling thread of ext4 filesystem. (当检查点启用时，可以使用此选项创建一个内核守护进程，使其尽可能合并并发的检查点请求，以消除冗余的检查点下发。此外，当检查点在 I/O 带宽和 CPU 份额较低的 cgroup 中的进程上下文里执行时，我们还可以消除由缓慢的检查点操作导致的迟滞问题。为了使其效果更好，我们将该内核守护进程的默认 I/O 优先级设置为 "3"，使其比其他内核线程高一个优先级。这与给 ext4 文件系统的 jbd2 日志线程赋予 I/O 优先级的方式相同。)
nocheckpoint_merge	 Disable checkpoint merge feature. (禁用检查点合并特性。)
compress_algorithm=%s	 Control compress algorithm, currently f2fs supports "lzo",
			 "lz4", "zstd" and "lzo-rle" algorithm. (控制压缩算法，当前 f2fs 支持 "lzo"、"lz4"、"zstd" 和 "lzo-rle" 算法。)
compress_algorithm=%s:%d Control compress algorithm and its compress level, now, only
			 "lz4" and "zstd" support compress level config (控制压缩算法及其压缩级别，现在仅 "lz4" 和 "zstd" 支持压缩级别配置)::

				 =========      ===========
				 algorithm      level range
				 =========      ===========
				 lz4            3 - 16
				 zstd           1 - 22
				 =========      ===========

compress_log_size=%u	 Support configuring compress cluster size. The size will
			 be 4KB * (1 << %u). The default and minimum sizes are 16KB. (支持配置压缩簇(cluster)大小。大小将为 4KB * (1 << %u)。默认大小和最小大小为 16KB。)
compress_extension=%s	 Support adding specified extension, so that f2fs can enable
			 compression on those corresponding files, e.g. if all files
			 with '.ext' has high compression rate, we can set the '.ext'
			 on compression extension list and enable compression on
			 these file by default rather than to enable it via ioctl.
			 For other files, we can still enable compression via ioctl.
			 Note that, there is one reserved special extension '*', it
			 can be set to enable compression for all files. (支持添加指定的扩展名，使 f2fs 能够对那些对应的文件启用压缩。例如，如果所有带 '.ext' 的文件都有较高的压缩率，我们可以将 '.ext' 设置到压缩扩展名列表中，并默认对这些文件启用压缩，而不是通过 ioctl 来启用。对于其他文件，我们仍然可以通过 ioctl 启用压缩。注意，有一个保留的特殊扩展名 '*'，可以设置它以为所有文件启用压缩。)
nocompress_extension=%s	 Support adding specified extension, so that f2fs can disable
			 compression on those corresponding files, just contrary to compression extension.
			 If you know exactly which files cannot be compressed, you can use this.
			 The same extension name can't appear in both compress and nocompress
			 extension at the same time.
			 If the compress extension specifies all files, the types specified by the
			 nocompress extension will be treated as special cases and will not be compressed.
			 Don't allow use '*' to specifie all file in nocompress extension.
			 After add nocompress_extension, the priority should be:
			 dir_flag < comp_extention,nocompress_extension < comp_file_flag,no_comp_file_flag.
			 See more in compression sections. (支持添加指定的扩展名，使 f2fs 能够对那些对应的文件禁用压缩，与压缩扩展名正好相反。如果你确切知道哪些文件无法压缩，可以使用它。相同的扩展名不能同时出现在 compress 和 nocompress 扩展名中。如果压缩扩展名指定了所有文件，则由 nocompress 扩展名指定的类型将被视为特例，不会被压缩。不允许在 nocompress 扩展名中使用 '*' 来指定所有文件。添加 nocompress_extension 之后，优先级应为：dir_flag < comp_extention,nocompress_extension < comp_file_flag,no_comp_file_flag。更多内容见压缩相关章节。)

compress_chksum		 Support verifying chksum of raw data in compressed cluster. (支持校验压缩簇中原始数据的 chksum。)
compress_mode=%s	 Control file compression mode. This supports "fs" and "user"
			 modes. In "fs" mode (default), f2fs does automatic compression
			 on the compression enabled files. In "user" mode, f2fs disables
			 the automaic compression and gives the user discretion of
			 choosing the target file and the timing. The user can do manual
			 compression/decompression on the compression enabled files using
			 ioctls. (控制文件压缩模式。支持 "fs" 和 "user" 模式。在 "fs" 模式(默认)下，f2fs 对已启用压缩的文件进行自动压缩。在 "user" 模式下，f2fs 禁用自动压缩，让用户自行决定目标文件和时机。用户可以使用 ioctl 对已启用压缩的文件进行手动压缩/解压。)
compress_cache		 Support to use address space of a filesystem managed inode to
			 cache compressed block, in order to improve cache hit ratio of
			 random read. (支持使用文件系统管理的 inode 的地址空间来缓存压缩块，以提高随机读的缓存命中率。)
inlinecrypt		 When possible, encrypt/decrypt the contents of encrypted
			 files using the blk-crypto framework rather than
			 filesystem-layer encryption. This allows the use of
			 inline encryption hardware. The on-disk format is
			 unaffected. For more details, see
			 Documentation/block/inline-encryption.rst. (在可能的情况下，使用 blk-crypto 框架而非文件系统层加密来加密/解密加密文件的内容。这允许使用内联加密硬件。磁盘上格式不受影响。更多细节请参见 Documentation/block/inline-encryption.rst。)
atgc			 Enable age-threshold garbage collection, it provides high
			 effectiveness and efficiency on background GC. (启用基于年龄阈值的垃圾回收(age-threshold garbage collection)，它在后台 GC 上提供高效能和高效率。)
discard_unit=%s		 Control discard unit, the argument can be "block", "segment"
			 and "section", issued discard command's offset/size will be
			 aligned to the unit, by default, "discard_unit=block" is set,
			 so that small discard functionality is enabled.
			 For blkzoned device, "discard_unit=section" will be set by
			 default, it is helpful for large sized SMR or ZNS devices to
			 reduce memory cost by getting rid of fs metadata supports small
			 discard. (控制 discard 单元，参数可以是 "block"、"segment" 和 "section"，下发的 discard 命令的 offset/size 将与该单元对齐。默认设置 "discard_unit=block"，从而启用小 discard 功能。对于 blkzoned 设备，默认将设置 "discard_unit=section"，这有助于大容量 SMR 或 ZNS 设备摆脱支持小 discard 的文件系统元数据，从而降低内存开销。)
memory=%s		 Control memory mode. This supports "normal" and "low" modes.
			 "low" mode is introduced to support low memory devices.
			 Because of the nature of low memory devices, in this mode, f2fs
			 will try to save memory sometimes by sacrificing performance.
			 "normal" mode is the default mode and same as before. (控制内存模式。支持 "normal" 和 "low" 模式。"low" 模式是为支持低内存设备而引入的。由于低内存设备的特性，在此模式下，f2fs 有时会通过牺牲性能来节省内存。"normal" 模式是默认模式，与之前相同。)
age_extent_cache	 Enable an age extent cache based on rb-tree. It records
			 data block update frequency of the extent per inode, in
			 order to provide better temperature hints for data block
			 allocation. (启用基于 rb-tree 的年龄 extent 缓存。它记录每个 inode 的 extent 数据块更新频率，以便为数据块分配提供更好的温度提示。)
errors=%s		 Specify f2fs behavior on critical errors. This supports modes:
			 "panic", "continue" and "remount-ro", respectively, trigger
			 panic immediately, continue without doing anything, and remount
			 the partition in read-only mode. By default it uses "continue"
			 mode. (指定 f2fs 在严重错误时的行为。支持以下模式："panic"、"continue" 和 "remount-ro"，分别表示立即触发 panic、不做任何处理继续运行，以及以只读模式重新挂载分区。默认使用 "continue" 模式。)

			 .. code-block:: none

			     ====================== =============== =============== ========
			     mode                   continue        remount-ro      panic
			     ====================== =============== =============== ========
			     access ops             normal          normal          N/A
			     syscall errors         -EIO            -EROFS          N/A
			     mount option           rw              ro              N/A
			     pending dir write      keep            keep            N/A
			     pending non-dir write  drop            keep            N/A
			     pending node write     drop            keep            N/A
			     pending meta write     keep            keep            N/A
			     ====================== =============== =============== ========
nat_bits		 Enable nat_bits feature to enhance full/empty nat blocks access,
			 by default it's disabled. (启用 nat_bits 特性以增强对全/空 nat 块的访问，默认情况下它是禁用的。)
lookup_mode=%s		 Control the directory lookup behavior for casefolded
			 directories. This option has no effect on directories
			 that do not have the casefold feature enabled. (控制大小写折叠(casefolded)目录的目录查找行为。此选项对未启用 casefold 特性的目录没有影响。)

			 .. code-block:: none

			     ================== ========================================
			     Value              Description
			     ================== ========================================
			     perf               (Default) Enforces a hash-only lookup.
					        The linear search fallback is always
					        disabled, ignoring the on-disk flag.
			     compat             Enables the linear search fallback for
					        compatibility with directory entries
					        created by older kernel that used a
					        different case-folding algorithm.
					        This mode ignores the on-disk flag.
			     auto               F2FS determines the mode based on the
					        on-disk `SB_ENC_NO_COMPAT_FALLBACK_FL`
					        flag.
			     ================== ========================================
======================== ============================================================

Debugfs Entries
(Debugfs 条目)
===============

/sys/kernel/debug/f2fs/ contains information about all the partitions mounted as
f2fs. Each file shows the whole f2fs information.
(/sys/kernel/debug/f2fs/ 包含所有以 f2fs 挂载的分区的信息。每个文件显示完整的 f2fs 信息。)

/sys/kernel/debug/f2fs/status includes:
(/sys/kernel/debug/f2fs/status 包括：)

 - major file system information managed by f2fs currently
   (当前由 f2fs 管理的主要文件系统信息)
 - average SIT information about whole segments
   (关于所有段的平均 SIT 信息)
 - current memory footprint consumed by f2fs.
   (f2fs 当前消耗的内存占用。)

Sysfs Entries
(Sysfs 条目)
=============

Information about mounted f2fs file systems can be found in
/sys/fs/f2fs.  Each mounted filesystem will have a directory in
/sys/fs/f2fs based on its device name (i.e., /sys/fs/f2fs/sda).
The files in each per-device directory are shown in table below.
(关于已挂载的 f2fs 文件系统的信息可以在 /sys/fs/f2fs 中找到。每个已挂载的文件系统都会基于其设备名(即 /sys/fs/f2fs/sda)在 /sys/fs/f2fs 中拥有一个目录。每个设备目录中的文件如下表所示。)

Files in /sys/fs/f2fs/<devname>
(see also Documentation/ABI/testing/sysfs-fs-f2fs)
(/sys/fs/f2fs/<devname> 中的文件。另请参见 Documentation/ABI/testing/sysfs-fs-f2fs。)

Usage
(用法)
=====

1. Download userland tools and compile them.
   (下载用户态工具并编译它们。)

2. Skip, if f2fs was compiled statically inside kernel.
   Otherwise, insert the f2fs.ko module (若 f2fs 已被静态编译进内核，则跳过此步。否则，插入 f2fs.ko 模块)::

	# insmod f2fs.ko

3. Create a directory to use when mounting (创建一个用于挂载的目录)::

	# mkdir /mnt/f2fs

4. Format the block device, and then mount as f2fs (格式化块设备，然后将其挂载为 f2fs)::

	# mkfs.f2fs -l label /dev/block_device
	# mount -t f2fs /dev/block_device /mnt/f2fs

mkfs.f2fs
---------
The mkfs.f2fs is for the use of formatting a partition as the f2fs filesystem,
which builds a basic on-disk layout.
(mkfs.f2fs 用于将一个分区格式化为 f2fs 文件系统，它会构建基本的磁盘上(on-disk)布局。)

The quick options consist of:
(快速选项包括：)

===============    ===========================================================
``-l [label]``     Give a volume label, up to 512 unicode name. (给出卷标，最多 512 个 unicode 字符名。)
``-a [0 or 1]``    Split start location of each area for heap-based allocation. (为基于堆(heap)的分配拆分各区域的起始位置。)

                   1 is set by default, which performs this. (默认设置为 1，即执行此操作。)
``-o [int]``       Set overprovision ratio in percent over volume size. (以卷大小百分比形式设置超额供应(overprovision)比率。)

                   5 is set by default. (默认设置为 5。)
``-s [int]``       Set the number of segments per section. (设置每个 section 的段数。)

                   1 is set by default. (默认设置为 1。)
``-z [int]``       Set the number of sections per zone. (设置每个 zone 的 section 数。)

                   1 is set by default. (默认设置为 1。)
``-e [str]``       Set basic extension list. e.g. "mp3,gif,mov" (设置基本扩展名列表。例如 "mp3,gif,mov"。)
``-t [0 or 1]``    Disable discard command or not. (是否禁用 discard 命令。)

                   1 is set by default, which conducts discard. (默认设置为 1，即执行 discard。)
===============    ===========================================================

Note: please refer to the manpage of mkfs.f2fs(8) to get full option list.
(注意：完整选项列表请参考 mkfs.f2fs(8) 的 manpage。)

fsck.f2fs
---------
The fsck.f2fs is a tool to check the consistency of an f2fs-formatted
partition, which examines whether the filesystem metadata and user-made data
are cross-referenced correctly or not.
Note that, initial version of the tool does not fix any inconsistency.
(fsck.f2fs 是一个用于检查 f2fs 格式化分区一致性的工具，它检查文件系统元数据和用户制作的数据之间是否被正确地交叉引用。注意，该工具的初始版本不会修复任何不一致。)

The quick options consist of (快速选项如下)::

  -d debug level [default:0]

Note: please refer to the manpage of fsck.f2fs(8) to get full option list.
(注意：完整选项列表请参考 fsck.f2fs(8) 的 manpage。)

dump.f2fs
---------
The dump.f2fs shows the information of specific inode and dumps SSA and SIT to
file. Each file is dump_ssa and dump_sit.
(dump.f2fs 显示特定 inode 的信息，并将 SSA 和 SIT 转储到文件。每个文件分别是 dump_ssa 和 dump_sit。)

The dump.f2fs is used to debug on-disk data structures of the f2fs filesystem.
It shows on-disk inode information recognized by a given inode number, and is
able to dump all the SSA and SIT entries into predefined files, ./dump_ssa and
./dump_sit respectively.
(dump.f2fs 用于调试 f2fs 文件系统的磁盘上数据结构。它显示由给定 inode 号识别的磁盘上 inode 信息，并能够将所有 SSA 和 SIT 条目转储到预定义的文件 ./dump_ssa 和 ./dump_sit 中。)

The options consist of (选项如下)::

  -d debug level [default:0]
  -i inode no (hex)
  -s [SIT dump segno from #1~#2 (decimal), for all 0~-1]
  -a [SSA dump segno from #1~#2 (decimal), for all 0~-1]

Examples (示例)::

    # dump.f2fs -i [ino] /dev/sdx
    # dump.f2fs -s 0~-1 /dev/sdx (SIT dump)
    # dump.f2fs -a 0~-1 /dev/sdx (SSA dump)

Note: please refer to the manpage of dump.f2fs(8) to get full option list.
(注意：完整选项列表请参考 dump.f2fs(8) 的 manpage。)

sload.f2fs
----------
The sload.f2fs gives a way to insert files and directories in the existing disk
image. This tool is useful when building f2fs images given compiled files.
(sload.f2fs 提供了一种向现有磁盘映像中插入文件和目录的方法。在给定已编译文件的情况下构建 f2fs 映像时，该工具非常有用。)

Note: please refer to the manpage of sload.f2fs(8) to get full option list.
(注意：完整选项列表请参考 sload.f2fs(8) 的 manpage。)

resize.f2fs
-----------
The resize.f2fs lets a user resize the f2fs-formatted disk image, while preserving
all the files and directories stored in the image.
(resize.f2fs 允许用户调整 f2fs 格式化磁盘映像的大小，同时保留映像中存储的所有文件和目录。)

Note: please refer to the manpage of resize.f2fs(8) to get full option list.
(注意：完整选项列表请参考 resize.f2fs(8) 的 manpage。)

defrag.f2fs
-----------
The defrag.f2fs can be used to defragment scattered written data as well as
filesystem metadata across the disk. This can improve the write speed by giving
more free consecutive space.
(defrag.f2fs 可用于对散布的已写入数据以及整个磁盘上的文件系统元数据进行碎片整理。这可以通过提供更多空闲连续空间来提高写入速度。)

Note: please refer to the manpage of defrag.f2fs(8) to get full option list.
(注意：完整选项列表请参考 defrag.f2fs(8) 的 manpage。)

f2fs_io
-------
The f2fs_io is a simple tool to issue various filesystem APIs as well as
f2fs-specific ones, which is very useful for QA tests.
(f2fs_io 是一个简单的工具，用于下发各种文件系统 API 以及 f2fs 特有的 API，这对 QA 测试非常有用。)

Note: please refer to the manpage of f2fs_io(8) to get full option list.
(注意：完整选项列表请参考 f2fs_io(8) 的 manpage。)

Design
(设计)
======

On-disk Layout
(磁盘上布局)
--------------

F2FS divides the whole volume into a number of segments, each of which is fixed
to 2MB in size. A section is composed of consecutive segments, and a zone
consists of a set of sections. By default, section and zone sizes are set to one
segment size identically, but users can easily modify the sizes by mkfs.
(F2FS 将整个卷划分为若干段(segment)，每段的大小固定为 2MB。一个 section 由连续的段组成，一个 zone 由一组 section 组成。默认情况下，section 和 zone 的大小被设置为与一个段的大小相同，但用户可以通过 mkfs 轻松修改这些大小。)

F2FS splits the entire volume into six areas, and all the areas except superblock
consist of multiple segments as described below (F2FS 将整个卷拆分为六个区域，除超级块外，所有区域都由多个段组成，如下所述)::

                                            align with the zone size <-|
                 |-> align with the segment size
     _________________________________________________________________________
    |            |            |   Segment   |    Node     |   Segment  |      |
    | Superblock | Checkpoint |    Info.    |   Address   |   Summary  | Main |
    |    (SB)    |   (CP)     | Table (SIT) | Table (NAT) | Area (SSA) |      |
    |____________|_____2______|______N______|______N______|______N_____|__N___|
                                                                       .      .
                                                             .                .
                                                 .                            .
                                    ._________________________________________.
                                    |_Segment_|_..._|_Segment_|_..._|_Segment_|
                                    .           .
                                    ._________._________
                                    |_section_|__...__|_
                                    .            .
		                    .________.
	                            |__zone__|

- Superblock (SB)
   (超级块 (SB))
   It is located at the beginning of the partition, and there exist two copies
   to avoid file system crash. It contains basic partition information and some
   default parameters of f2fs.
   (它位于分区的起始处，存在两份副本以避免文件系统崩溃。它包含基本的分区信息和 f2fs 的一些默认参数。)

- Checkpoint (CP)
   (检查点 (CP))
   It contains file system information, bitmaps for valid NAT/SIT sets, orphan
   inode lists, and summary entries of current active segments.
   (它包含文件系统信息、有效 NAT/SIT 集的位图、孤立 inode 列表，以及当前活动段的摘要条目。)

- Segment Information Table (SIT)
   (段信息表 (SIT))
   It contains segment information such as valid block count and bitmap for the
   validity of all the blocks.
   (它包含段信息，例如有效块计数以及所有块有效性的位图。)

- Node Address Table (NAT)
   (节点地址表 (NAT))
   It is composed of a block address table for all the node blocks stored in
   Main area.
   (它由一个块地址表组成，对应存储在 Main area 中的所有节点块。)

- Segment Summary Area (SSA)
   (段摘要区域 (SSA))
   It contains summary entries which contains the owner information of all the
   data and node blocks stored in Main area.
   (它包含摘要条目，这些条目包含存储在 Main area 中的所有数据块和节点块的属主信息。)

- Main Area
   (主要区域 (Main Area))
   It contains file and directory data including their indices.
   (它包含文件和目录数据及其索引。)

In order to avoid misalignment between file system and flash-based storage, F2FS
aligns the start block address of CP with the segment size. Also, it aligns the
start block address of Main area with the zone size by reserving some segments
in SSA area.
(为了避免文件系统与基于闪存的存储之间出现不对齐，F2FS 将 CP 的起始块地址与段大小对齐。此外，它通过在 SSA 区域中保留一些段，使 Main area 的起始块地址与 zone 大小对齐。)

Reference the following survey for additional technical details.
https://wiki.linaro.org/WorkingGroups/Kernel/Projects/FlashCardSurvey
(更多技术细节请参考以下调查报告。)

File System Metadata Structure
(文件系统元数据结构)
------------------------------

F2FS adopts the checkpointing scheme to maintain file system consistency. At
mount time, F2FS first tries to find the last valid checkpoint data by scanning
CP area. In order to reduce the scanning time, F2FS uses only two copies of CP.
One of them always indicates the last valid data, which is called as shadow copy
mechanism. In addition to CP, NAT and SIT also adopt the shadow copy mechanism.
(F2FS 采用检查点(checkpointing)方案来维护文件系统一致性。挂载时，F2FS 首先通过扫描 CP 区域尝试找到最后一个有效的检查点数据。为了减少扫描时间，F2FS 仅使用两份 CP 副本。其中一份始终指示最后的有效数据，这被称为影子副本(shadow copy)机制。除 CP 外，NAT 和 SIT 也采用影子副本机制。)

For file system consistency, each CP points to which NAT and SIT copies are
valid, as shown as below (出于文件系统一致性的考虑，每个 CP 指向哪些 NAT 和 SIT 副本是有效的，如下所示)::

  +--------+----------+---------+
  |   CP   |    SIT   |   NAT   |
  +--------+----------+---------+
  .         .          .          .
  .            .              .              .
  .               .                 .                 .
  +-------+-------+--------+--------+--------+--------+
  | CP #0 | CP #1 | SIT #0 | SIT #1 | NAT #0 | NAT #1 |
  +-------+-------+--------+--------+--------+--------+
     |             ^                          ^
     |             |                          |
     `----------------------------------------'

Index Structure
(索引结构)
---------------

The key data structure to manage the data locations is a "node". Similar to
traditional file structures, F2FS has three types of node: inode, direct node,
indirect node. F2FS assigns 4KB to an inode block which contains 923 data block
indices, two direct node pointers, two indirect node pointers, and one double
indirect node pointer as described below. One direct node block contains 1018
data blocks, and one indirect node block contains also 1018 node blocks. Thus,
one inode block (i.e., a file) covers (管理数据位置的关键数据结构是“node”。与传统文件结构类似，F2FS 有三种节点类型：inode、直接节点、间接节点。F2FS 为一个 inode 块分配 4KB，其中包含 923 个数据块索引、两个直接节点指针、两个间接节点指针和一个双间接节点指针，如下所述。一个直接节点块包含 1018 个数据块，一个间接节点块同样包含 1018 个节点块。因此，一个 inode 块(即一个文件)可覆盖的范围如下)::

  4KB * (923 + 2 * 1018 + 2 * 1018 * 1018 + 1018 * 1018 * 1018) := 3.94TB.

   Inode block (4KB)
     |- data (923)
     |- direct node (2)
     |          `- data (1018)
     |- indirect node (2)
     |            `- direct node (1018)
     |                       `- data (1018)
     `- double indirect node (1)
                         `- indirect node (1018)
			              `- direct node (1018)
	                                         `- data (1018)

Note that all the node blocks are mapped by NAT which means the location of
each node is translated by the NAT table. In the consideration of the wandering
tree problem, F2FS is able to cut off the propagation of node updates caused by
leaf data writes.
(注意，所有节点块都由 NAT 映射，这意味着每个节点的位置由 NAT 表进行转换。考虑到漫游树问题，F2FS 能够切断由叶子数据写入引起的节点更新传播。)

Directory Structure
(目录结构)
-------------------

A directory entry occupies 11 bytes, which consists of the following attributes.
(一个目录项占用 11 个字节，由以下属性组成。)

- hash		hash value of the file name
  (hash: 文件名的哈希值)
- ino		inode number
  (ino: inode 号)
- len		the length of file name
  (len: 文件名的长度)
- type		file type such as directory, symlink, etc
  (type: 文件类型，例如目录、符号链接等)

A dentry block consists of 214 dentry slots and file names. Therein a bitmap is
used to represent whether each dentry is valid or not. A dentry block occupies
4KB with the following composition.
(一个 dentry 块由 214 个 dentry 槽位和文件名组成。其中使用一个位图来表示每个 dentry 是否有效。一个 dentry 块占用 4KB，其组成如下。)

::

  Dentry Block(4 K) = bitmap (27 bytes) + reserved (3 bytes) +
	              dentries(11 * 214 bytes) + file name (8 * 214 bytes)

                         [Bucket]
             +--------------------------------+
             |dentry block 1 | dentry block 2 |
             +--------------------------------+
             .               .
       .                             .
  .       [Dentry Block Structure: 4KB]       .
  +--------+----------+----------+------------+
  | bitmap | reserved | dentries | file names |
  +--------+----------+----------+------------+
  [Dentry Block: 4KB] .   .
		 .               .
            .                          .
            +------+------+-----+------+
            | hash | ino  | len | type |
            +------+------+-----+------+
            [Dentry Structure: 11 bytes]

F2FS implements multi-level hash tables for directory structure. Each level has
a hash table with dedicated number of hash buckets as shown below. Note that
"A(2B)" means a bucket includes 2 data blocks.
(F2FS 为目录结构实现了多级哈希表。每一级都有一个具有专用数量哈希桶的哈希表，如下所示。注意，“A(2B)”表示一个桶包含 2 个数据块。)

::

    ----------------------
    A : bucket
    B : block
    N : MAX_DIR_HASH_DEPTH
    ----------------------

    level #0   | A(2B)
	    |
    level #1   | A(2B) - A(2B)
	    |
    level #2   | A(2B) - A(2B) - A(2B) - A(2B)
	.     |   .       .       .       .
    level #N/2 | A(2B) - A(2B) - A(2B) - A(2B) - A(2B) - ... - A(2B)
	.     |   .       .       .       .
    level #N   | A(4B) - A(4B) - A(4B) - A(4B) - A(4B) - ... - A(4B)

The number of blocks and buckets are determined by (块数和桶数由以下方式确定)::

                            ,- 2, if n < MAX_DIR_HASH_DEPTH / 2,
  # of blocks in level #n = |
                            `- 4, Otherwise

                             ,- 2^(n + dir_level),
			     |        if n + dir_level < MAX_DIR_HASH_DEPTH / 2,
  # of buckets in level #n = |
                             `- 2^((MAX_DIR_HASH_DEPTH / 2) - 1),
			              Otherwise

When F2FS finds a file name in a directory, at first a hash value of the file
name is calculated. Then, F2FS scans the hash table in level #0 to find the
dentry consisting of the file name and its inode number. If not found, F2FS
scans the next hash table in level #1. In this way, F2FS scans hash tables in
each levels incrementally from 1 to N. In each level F2FS needs to scan only
one bucket determined by the following equation, which shows O(log(# of files))
complexity (当 F2FS 在目录中查找一个文件名时，首先计算该文件名的哈希值。然后，F2FS 扫描 level #0 中的哈希表，以找到由该文件名及其 inode 号组成的 dentry。如果未找到，F2FS 将扫描 level #1 中的下一个哈希表。以此方式，F2FS 从 1 到 N 逐级递增地扫描各级哈希表。在每一级中，F2FS 只需扫描由以下等式确定的一个桶，这展现了 O(log(文件数)) 的复杂度)::

  bucket number to scan in level #n = (hash value) % (# of buckets in level #n)

In the case of file creation, F2FS finds empty consecutive slots that cover the
file name. F2FS searches the empty slots in the hash tables of whole levels from
1 to N in the same way as the lookup operation.
(在创建文件的情况下，F2FS 会找到覆盖该文件名的空连续槽位。F2FS 以与查找操作相同的方式，在整个 1 到 N 级的哈希表中搜索空槽位。)

The following figure shows an example of two cases holding children (下图显示了包含子项的两种情况的示例)::

       --------------> Dir <--------------
       |                                 |
    child                             child

    child - child                     [hole] - child

    child - child - child             [hole] - [hole] - child

   Case 1:                           Case 2:
   Number of children = 6,           Number of children = 3,
   File size = 7                     File size = 7

Default Block Allocation
(默认块分配)
------------------------

At runtime, F2FS manages six active logs inside "Main" area: Hot/Warm/Cold node
and Hot/Warm/Cold data.
(在运行时，F2FS 在“Main”区域内管理六个活动日志：热/温/冷节点以及热/温/冷数据。)

- Hot node	contains direct node blocks of directories.
  (热节点: 包含目录的直接节点块。)
- Warm node	contains direct node blocks except hot node blocks.
  (温节点: 包含除热节点块之外的直接节点块。)
- Cold node	contains indirect node blocks
  (冷节点: 包含间接节点块。)
- Hot data	contains dentry blocks
  (热数据: 包含 dentry 块。)
- Warm data	contains data blocks except hot and cold data blocks
  (温数据: 包含除热和冷数据块之外的数据块。)
- Cold data	contains multimedia data or migrated data blocks
  (冷数据: 包含多媒体数据或迁移的数据块。)

LFS has two schemes for free space management: threaded log and copy-and-compac-
tion. The copy-and-compaction scheme which is known as cleaning, is well-suited
for devices showing very good sequential write performance, since free segments
are served all the time for writing new data. However, it suffers from cleaning
overhead under high utilization. Contrarily, the threaded log scheme suffers
from random writes, but no cleaning process is needed. F2FS adopts a hybrid
scheme where the copy-and-compaction scheme is adopted by default, but the
policy is dynamically changed to the threaded log scheme according to the file
system status.
(LFS 有两种空闲空间管理方案：线程化日志(threaded log)和复制压缩(copy-and-compaction)。复制压缩方案即所谓的清理，非常适合顺序写入性能非常好的设备，因为空闲段始终可用于写入新数据。然而，在高利用率下它存在清理开销问题。相反，线程化日志方案存在随机写入问题，但不需要清理过程。F2FS 采用混合方案：默认采用复制压缩方案，但会根据文件系统状态将策略动态地改为线程化日志方案。)

In order to align F2FS with underlying flash-based storage, F2FS allocates a
segment in a unit of section. F2FS expects that the section size would be the
same as the unit size of garbage collection in FTL. Furthermore, with respect
to the mapping granularity in FTL, F2FS allocates each section of the active
logs from different zones as much as possible, since FTL can write the data in
the active logs into one allocation unit according to its mapping granularity.
(为了使 F2FS 与底层基于闪存的存储对齐，F2FS 以 section 为单位分配段。F2FS 期望 section 大小与 FTL 中垃圾回收的单位大小相同。此外，考虑到 FTL 中的映射粒度，F2FS 尽可能从不同的 zone 分配活动日志的各个 section，因为 FTL 可以根据其映射粒度把活动日志中的数据写入一个分配单元。)

Cleaning process
(清理过程)
----------------

F2FS does cleaning both on demand and in the background. On-demand cleaning is
triggered when there are not enough free segments to serve VFS calls. Background
cleaner is operated by a kernel thread, and triggers the cleaning job when the
system is idle.
(F2FS 同时进行按需清理和后台清理。当没有足够的空闲段来服务 VFS 调用时，会触发按需清理。后台清理器由一个内核线程运行，并在系统空闲时触发清理工作。)

F2FS supports two victim selection policies: greedy and cost-benefit algorithms.
In the greedy algorithm, F2FS selects a victim segment having the smallest number
of valid blocks. In the cost-benefit algorithm, F2FS selects a victim segment
according to the segment age and the number of valid blocks in order to address
log block thrashing problem in the greedy algorithm. F2FS adopts the greedy
algorithm for on-demand cleaner, while background cleaner adopts cost-benefit
algorithm.
(F2FS 支持两种受害段选择策略：贪婪(greedy)和代价收益(cost-benefit)算法。在贪婪算法中，F2FS 选择有效块数量最少的受害段。在代价收益算法中，F2FS 根据段的年龄和有效块数量来选择受害段，以解决贪婪算法中的日志块抖动(thrashing)问题。F2FS 对按需清理器采用贪婪算法，而后台清理器采用代价收益算法。)

In order to identify whether the data in the victim segment are valid or not,
F2FS manages a bitmap. Each bit represents the validity of a block, and the
bitmap is composed of a bit stream covering whole blocks in main area.
(为了识别受害段中的数据是否有效，F2FS 管理一个位图。每一位代表一个块的有效性，该位图由覆盖 main 区域中所有块的位流组成。)

Write-hint Policy
(写提示策略)
-----------------

F2FS sets the whint all the time with the below policy.
(F2FS 始终按照以下策略设置 whint。)

===================== ======================== ===================
User                  F2FS                     Block
===================== ======================== ===================
N/A                   META                     WRITE_LIFE_NONE|REQ_META
N/A                   HOT_NODE                 WRITE_LIFE_NONE
N/A                   WARM_NODE                WRITE_LIFE_MEDIUM
N/A                   COLD_NODE                WRITE_LIFE_LONG
ioctl(COLD)           COLD_DATA                WRITE_LIFE_EXTREME
extension list        "                        " (扩展列表)

-- buffered io (缓冲 IO)
------------------------------------------------------------------
N/A                   COLD_DATA                WRITE_LIFE_EXTREME
N/A                   HOT_DATA                 WRITE_LIFE_SHORT
N/A                   WARM_DATA                WRITE_LIFE_NOT_SET

-- direct io (直接 IO)
------------------------------------------------------------------
WRITE_LIFE_EXTREME    COLD_DATA                WRITE_LIFE_EXTREME
WRITE_LIFE_SHORT      HOT_DATA                 WRITE_LIFE_SHORT
WRITE_LIFE_NOT_SET    WARM_DATA                WRITE_LIFE_NOT_SET
WRITE_LIFE_NONE       "                        WRITE_LIFE_NONE
WRITE_LIFE_MEDIUM     "                        WRITE_LIFE_MEDIUM
WRITE_LIFE_LONG       "                        WRITE_LIFE_LONG
===================== ======================== ===================

Fallocate(2) Policy
(Fallocate(2) 策略)
-------------------

The default policy follows the below POSIX rule.
(默认策略遵循以下 POSIX 规则。)

Allocating disk space (分配磁盘空间)
    The default operation (i.e., mode is zero) of fallocate() allocates
    the disk space within the range specified by offset and len.  The
    file size (as reported by stat(2)) will be changed if offset+len is
    greater than the file size.  Any subregion within the range specified
    by offset and len that did not contain data before the call will be
    initialized to zero.  This default behavior closely resembles the
    behavior of the posix_fallocate(3) library function, and is intended
    as a method of optimally implementing that function.
    (fallocate() 的默认操作(即 mode 为零)会在 offset 和 len 指定的范围内分配磁盘空间。如果 offset+len 大于文件大小，文件大小(由 stat(2) 报告)将会改变。offset 和 len 指定范围内在调用之前不包含数据的任何子区域都将被初始化为零。这种默认行为非常接近 posix_fallocate(3) 库函数的行为，旨在作为最优实现该函数的一种方法。)

However, once F2FS receives ioctl(fd, F2FS_IOC_SET_PIN_FILE) in prior to
fallocate(fd, DEFAULT_MODE), it allocates on-disk block addresses having
zero or random data, which is useful to the below scenario where:
(但是，一旦 F2FS 在 fallocate(fd, DEFAULT_MODE) 之前接收到 ioctl(fd, F2FS_IOC_SET_PIN_FILE)，它就会分配具有零数据或随机数据的磁盘上块地址，这对以下场景很有用：)

 1. create(fd)
 2. ioctl(fd, F2FS_IOC_SET_PIN_FILE)
 3. fallocate(fd, 0, 0, size)
 4. address = fibmap(fd, offset)
 5. open(blkdev)
 6. write(blkdev, address)

Compression implementation
(压缩实现)
--------------------------

- New term named cluster is defined as basic unit of compression, file can
  be divided into multiple clusters logically. One cluster includes 4 << n
  (n >= 0) logical pages, compression size is also cluster size, each of
  cluster can be compressed or not.
  (定义了一个名为 cluster(簇)的新术语，作为压缩的基本单位，文件在逻辑上可以划分为多个簇。一个簇包含 4 << n (n >= 0) 个逻辑页，压缩大小也是簇大小，每个簇可以被压缩也可以不被压缩。)

- In cluster metadata layout, one special block address is used to indicate
  a cluster is a compressed one or normal one; for compressed cluster, following
  metadata maps cluster to [1, 4 << n - 1] physical blocks, in where f2fs
  stores data including compress header and compressed data.
  (在簇元数据布局中，使用一个特殊的块地址来指示簇是压缩簇还是普通簇；对于压缩簇，后续元数据将簇映射到 [1, 4 << n - 1] 个物理块，f2fs 在其中存储包括压缩头和压缩数据在内的数据。)

- In order to eliminate write amplification during overwrite, F2FS only
  support compression on write-once file, data can be compressed only when
  all logical blocks in cluster contain valid data and compress ratio of
  cluster data is lower than specified threshold.
  (为了消除覆写期间的写放大(write amplification)，F2FS 仅支持对一次性写入的文件进行压缩，只有当簇中的所有逻辑块都包含有效数据且簇数据的压缩比低于指定阈值时，才可以压缩数据。)

- To enable compression on regular inode, there are four ways:
  (要在普通 inode 上启用压缩，有四种方式：)

  * chattr +c file
  * chattr +c dir; touch dir/file
  * mount w/ -o compress_extension=ext; touch file.ext
  * mount w/ -o compress_extension=*; touch any_file

- To disable compression on regular inode, there are two ways:
  (要在普通 inode 上禁用压缩，有两种方式：)

  * chattr -c file
  * mount w/ -o nocompress_extension=ext; touch file.ext

- Priority in between FS_COMPR_FL, FS_NOCOMP_FS, extensions:
  (FS_COMPR_FL、FS_NOCOMP_FS 与扩展名之间的优先级：)

  * compress_extension=so; nocompress_extension=zip; chattr +c dir; touch
    dir/foo.so; touch dir/bar.zip; touch dir/baz.txt; then foo.so and baz.txt
    should be compresse, bar.zip should be non-compressed. chattr +c dir/bar.zip
    can enable compress on bar.zip.
    (则 foo.so 和 baz.txt 应被压缩，bar.zip 应为非压缩。chattr +c dir/bar.zip 可以启用 bar.zip 的压缩。)
  * compress_extension=so; nocompress_extension=zip; chattr -c dir; touch
    dir/foo.so; touch dir/bar.zip; touch dir/baz.txt; then foo.so should be
    compresse, bar.zip and baz.txt should be non-compressed.
    chattr+c dir/bar.zip; chattr+c dir/baz.txt; can enable compress on bar.zip
    and baz.txt.
    (则 foo.so 应被压缩，bar.zip 和 baz.txt 应为非压缩。chattr+c dir/bar.zip; chattr+c dir/baz.txt; 可以启用 bar.zip 和 baz.txt 的压缩。)

- At this point, compression feature doesn't expose compressed space to user
  directly in order to guarantee potential data updates later to the space.
  Instead, the main goal is to reduce data writes to flash disk as much as
  possible, resulting in extending disk life time as well as relaxing IO
  congestion. Alternatively, we've added ioctl(F2FS_IOC_RELEASE_COMPRESS_BLOCKS)
  interface to reclaim compressed space and show it to user after setting a
  special flag to the inode. Once the compressed space is released, the flag
  will block writing data to the file until either the compressed space is
  reserved via ioctl(F2FS_IOC_RESERVE_COMPRESS_BLOCKS) or the file size is
  truncated to zero.
  (目前，压缩特性并不直接向用户暴露压缩后的空间，以保证之后可能对该空间进行的数据更新。相反，其主要目标是尽可能减少对闪存盘的数据写入，从而延长磁盘寿命并缓解 IO 拥塞。另外，我们添加了 ioctl(F2FS_IOC_RELEASE_COMPRESS_BLOCKS) 接口，用于回收压缩空间，并在向 inode 设置一个特殊标志之后将其展示给用户。一旦压缩空间被释放，该标志将阻止向文件写入数据，直到通过 ioctl(F2FS_IOC_RESERVE_COMPRESS_BLOCKS) 保留了压缩空间，或者文件大小被截断为零为止。)

Compress metadata layout (压缩元数据布局)::

				[Dnode Structure]
		+-----------------------------------------------+
		| cluster 1 | cluster 2 | ......... | cluster N |
		+-----------------------------------------------+
		.           .                       .           .
	  .                      .                .                      .
    .         Compressed Cluster       .        .        Normal Cluster            .
    +----------+---------+---------+---------+  +---------+---------+---------+---------+
    |compr flag| block 1 | block 2 | block 3 |  | block 1 | block 2 | block 3 | block 4 |
    +----------+---------+---------+---------+  +---------+---------+---------+---------+
	       .                             .
	    .                                           .
	.                                                           .
	+-------------+-------------+----------+----------------------------+
	| data length | data chksum | reserved |      compressed data       |
	+-------------+-------------+----------+----------------------------+

Compression mode
(压缩模式)
--------------------------

f2fs supports "fs" and "user" compression modes with "compression_mode" mount option.
With this option, f2fs provides a choice to select the way how to compress the
compression enabled files (refer to "Compression implementation" section for how to
enable compression on a regular inode).
(f2fs 通过 "compression_mode" 挂载选项支持 "fs" 和 "user" 两种压缩模式。使用此选项，f2fs 提供了选择如何压缩已启用压缩的文件的方式(如何对普通 inode 启用压缩，请参见“Compression implementation”一节)。)

1) compress_mode=fs

   This is the default option. f2fs does automatic compression in the writeback of the
   compression enabled files.
   (这是默认选项。f2fs 在已启用压缩的文件的回写过程中进行自动压缩。)

2) compress_mode=user

   This disables the automatic compression and gives the user discretion of choosing the
   target file and the timing. The user can do manual compression/decompression on the
   compression enabled files using F2FS_IOC_DECOMPRESS_FILE and F2FS_IOC_COMPRESS_FILE
   ioctls like the below.
   (这将禁用自动压缩，让用户自行决定目标文件和时机。用户可以使用 F2FS_IOC_DECOMPRESS_FILE 和 F2FS_IOC_COMPRESS_FILE ioctl 对已启用压缩的文件进行手动压缩/解压，如下所示。)

To decompress a file (解压一个文件)::

  fd = open(filename, O_WRONLY, 0);
  ret = ioctl(fd, F2FS_IOC_DECOMPRESS_FILE);

To compress a file (压缩一个文件)::

  fd = open(filename, O_WRONLY, 0);
  ret = ioctl(fd, F2FS_IOC_COMPRESS_FILE);

NVMe Zoned Namespace devices
(NVMe Zoned Namespace 设备)
----------------------------

- ZNS defines a per-zone capacity which can be equal or less than the
  zone-size. Zone-capacity is the number of usable blocks in the zone.
  F2FS checks if zone-capacity is less than zone-size, if it is, then any
  segment which starts after the zone-capacity is marked as not-free in
  the free segment bitmap at initial mount time. These segments are marked
  as permanently used so they are not allocated for writes and
  consequently are not needed to be garbage collected. In case the
  zone-capacity is not aligned to default segment size(2MB), then a segment
  can start before the zone-capacity and span across zone-capacity boundary.
  Such spanning segments are also considered as usable segments. All blocks
  past the zone-capacity are considered unusable in these segments.
  (ZNS 定义了每个 zone 的容量(zone capacity)，它可以等于或小于 zone 大小(zone-size)。zone-capacity 是 zone 中可用块的数量。F2FS 会检查 zone-capacity 是否小于 zone-size，如果是，则在初始挂载时，任何在 zone-capacity 之后开始的段都会在空闲段位图中被标记为非空闲。这些段被标记为永久使用，因此不会被分配用于写入，因而也无需对其进行垃圾回收。如果 zone-capacity 未与默认段大小(2MB)对齐，则某个段可以在 zone-capacity 之前开始并跨越 zone-capacity 边界。此类跨越的段也被视为可用段。在这些段中，超过 zone-capacity 的所有块都被视为不可用。)

Device aliasing feature
(设备别名特性)
-----------------------

f2fs can utilize a special file called a "device aliasing file." This file allows
the entire storage device to be mapped with a single, large extent, not using
the usual f2fs node structures. This mapped area is pinned and primarily intended
for holding the space.
(f2fs 可以利用一种称为“设备别名文件(device aliasing file)”的特殊文件。该文件允许使用单个大 extent 来映射整个存储设备，而不使用通常的 f2fs 节点结构。这个被映射的区域是被固定的(pinned)，其主要目的是占用该空间。)

Essentially, this mechanism allows a portion of the f2fs area to be temporarily
reserved and used by another filesystem or for different purposes. Once that
external usage is complete, the device aliasing file can be deleted, releasing
the reserved space back to F2FS for its own use.
(本质上，该机制允许 f2fs 区域的一部分被临时保留下来，供另一个文件系统或用于不同目的。一旦该外部使用完成，就可以删除设备别名文件，从而把保留的空间释放回 F2FS 供其自身使用。)

.. code-block::

   # ls /dev/vd*
   /dev/vdb (32GB) /dev/vdc (32GB)
   # mkfs.ext4 /dev/vdc
   # mkfs.f2fs -c /dev/vdc@vdc.file /dev/vdb
   # mount /dev/vdb /mnt/f2fs
   # ls -l /mnt/f2fs
   vdc.file
   # df -h
   /dev/vdb                            64G   33G   32G  52% /mnt/f2fs

   # mount -o loop /dev/vdc /mnt/ext4
   # df -h
   /dev/vdb                            64G   33G   32G  52% /mnt/f2fs
   /dev/loop7                          32G   24K   30G   1% /mnt/ext4
   # umount /mnt/ext4

   # f2fs_io getflags /mnt/f2fs/vdc.file
   get a flag on /mnt/f2fs/vdc.file ret=0, flags=nocow(pinned),immutable
   # f2fs_io setflags noimmutable /mnt/f2fs/vdc.file
   get a flag on noimmutable ret=0, flags=800010
   set a flag on /mnt/f2fs/vdc.file ret=0, flags=noimmutable
   # rm /mnt/f2fs/vdc.file
   # df -h
   /dev/vdb                            64G  753M   64G   2% /mnt/f2fs

So, the key idea is, user can do any file operations on /dev/vdc, and
reclaim the space after the use, while the space is counted as /data.
That doesn't require modifying partition size and filesystem format.
(因此，其核心思想是：用户可以在 /dev/vdc 上进行任何文件操作，并在使用之后回收该空间，同时该空间被计入 /data。这不需要修改分区大小和文件系统格式。)

Per-file Read-Only Large Folio Support
(每文件只读 large folio 支持)
--------------------------------------

F2FS implements large folio support on the read path to leverage high-order
page allocation for significant performance gains. To minimize code complexity,
this support is currently excluded from the write path, which requires handling
complex optimizations such as compression and block allocation modes.
(F2FS 在读路径上实现了 large folio 支持，以利用高阶页分配来获得显著的性能提升。为了尽量降低代码复杂度，该支持目前被排除在写路径之外，因为写路径需要处理压缩和块分配模式等复杂的优化。)

This optional feature is triggered only when a file's immutable bit is set.
Consequently, F2FS will return EOPNOTSUPP if a user attempts to open a cached
file with write permissions, even immediately after clearing the bit. Write
access is only restored once the cached inode is dropped. The usage flow is
demonstrated below:
(此可选特性仅在该文件的 immutable 位被设置时触发。因此，如果用户尝试以写权限打开一个已被缓存的文件，即使是在清除该位之后立即进行，F2FS 也会返回 EOPNOTSUPP。只有当被缓存的 inode 被丢弃之后，写访问才会恢复。使用流程如下所示：)

.. code-block::

   # f2fs_io setflags immutable /data/testfile_read_seq

   /* flush and reload the inode to enable the large folio */
   # sync && echo 3 > /proc/sys/vm/drop_caches

   /* mmap(MAP_POPULATE) + mlock() */
   # f2fs_io read 128 0 1024 mmap 1 0 /data/testfile_read_seq

   /* mmap() + fadvise(POSIX_FADV_WILLNEED) + mlock() */
   # f2fs_io read 128 0 1024 fadvise 1 0 /data/testfile_read_seq

   /* mmap() + mlock2(MLOCK_ONFAULT) + madvise(MADV_POPULATE_READ) */
   # f2fs_io read 128 0 1024 madvise 1 0 /data/testfile_read_seq

   # f2fs_io clearflags immutable /data/testfile_read_seq

   # f2fs_io write 1 0 1 zero buffered /data/testfile_read_seq
   Failed to open /mnt/test/test: Operation not supported

   /* flush and reload the inode to disable the large folio */
   # sync && echo 3 > /proc/sys/vm/drop_caches

   # f2fs_io write 1 0 1 zero buffered /data/testfile_read_seq
   Written 4096 bytes with pattern = zero, total_time = 29 us, max_latency = 28 us

   # rm /data/testfile_read_seq
