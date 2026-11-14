.. SPDX-License-Identifier: GPL-2.0

======
NILFS2
======

(NILFS2)

NILFS2 is a log-structured file system (LFS) supporting continuous
snapshotting.  In addition to versioning capability of the entire file
system, users can even restore files mistakenly overwritten or
destroyed just a few seconds ago.  Since NILFS2 can keep consistency
like conventional LFS, it achieves quick recovery after system
crashes.
(NILFS2 是一种支持持续快照(snapshotting)的日志结构文件系统(LFS)。除了对整个文件系统的版本管理能力之外，用户甚至可以恢复几秒钟之前被误覆盖或误删除的文件。由于 NILFS2 能够像传统的 LFS 一样保持一致性，它在系统崩溃之后可以实现快速恢复。)

NILFS2 creates a number of checkpoints every few seconds or per
synchronous write basis (unless there is no change).  Users can select
significant versions among continuously created checkpoints, and can
change them into snapshots which will be preserved until they are
changed back to checkpoints.
(NILFS2 每隔几秒、或按每次同步写(除非没有变化)创建若干检查点(checkpoint)。用户可以从持续创建的检查点中挑选出重要的版本，并把它们变成快照(snapshot)；快照会被保留下来，直到它们被改回检查点。)

There is no limit on the number of snapshots until the volume gets
full.  Each snapshot is mountable as a read-only file system
concurrently with its writable mount, and this feature is convenient
for online backup.
(在卷被写满之前，快照的数量没有限制。每个快照都可以作为一个只读文件系统挂载，并且与它的可写挂载并发存在，这一特性对在线备份非常方便。)

The userland tools are included in nilfs-utils package, which is
available from the following download page.  At least "mkfs.nilfs2",
"mount.nilfs2", "umount.nilfs2", and "nilfs_cleanerd" (so called
cleaner or garbage collector) are required.  Details on the tools are
described in the man pages included in the package.
(用户态工具包含在 nilfs-utils 软件包中，可从下面的下载页面获得。至少需要 "mkfs.nilfs2"、"mount.nilfs2"、"umount.nilfs2" 以及 "nilfs_cleanerd"(即所谓的 cleaner 或垃圾回收器/GC)。有关这些工具的详细信息在软件包附带的手册页中描述。)

:Project web page:    https://nilfs.sourceforge.io/
   (项目主页: https://nilfs.sourceforge.io/)
:Download page:       https://nilfs.sourceforge.io/en/download.html
   (下载页面: https://nilfs.sourceforge.io/en/download.html)
:List info:           http://vger.kernel.org/vger-lists.html#linux-nilfs
   (邮件列表信息: http://vger.kernel.org/vger-lists.html#linux-nilfs)

Caveats
(注意事项)
=======

Features which NILFS2 does not support yet:
(NILFS2 尚不支持的特性：)

	- atime
	  (atime(访问时间))
	- extended attributes
	  (扩展属性)
	- POSIX ACLs
	  (POSIX 访问控制列表(ACL))
	- quotas
	  (配额)
	- fsck
	  (fsck(文件系统检查))
	- defragmentation
	  (碎片整理)

Mount options
(挂载选项)
=============

NILFS2 supports the following mount options:
(NILFS2 支持以下挂载选项：)
(*) == default
(其中 (*) 表示默认值。)

======================= =======================================================
barrier(*)		This enables/disables the use of write barriers.  This
nobarrier		requires an IO stack which can support barriers, and
			if nilfs gets an error on a barrier write, it will
			disable again with a warning. (启用/禁用写屏障(write barriers)的使用。这需要一个能够支持屏障的 IO 栈；如果 nilfs 在一次屏障写上遇到错误，它会再次禁用该功能并给出警告。)
errors=continue		Keep going on a filesystem error. (发生文件系统错误时继续运行。)
errors=remount-ro(*)	Remount the filesystem read-only on an error. (发生错误时把文件系统重新挂载为只读。)
errors=panic		Panic and halt the machine if an error occurs. (如果发生错误则 panic 并使机器停机。)
cp=n			Specify the checkpoint-number of the snapshot to be
			mounted.  Checkpoints and snapshots are listed by lscp
			user command.  Only the checkpoints marked as snapshot
			are mountable with this option.  Snapshot is read-only,
			so a read-only mount option must be specified together. (指定要挂载的快照的检查点编号(checkpoint-number)。检查点和快照由用户命令 lscp 列出。只有被标记为 snapshot 的检查点才能用此选项挂载。快照是只读的，因此必须同时指定只读挂载选项。)
order=relaxed(*)	Apply relaxed order semantics that allows modified data
			blocks to be written to disk without making a
			checkpoint if no metadata update is going.  This mode
			is equivalent to the ordered data mode of the ext3
			filesystem except for the updates on data blocks still
			conserve atomicity.  This will improve synchronous
			write performance for overwriting. (应用宽松的顺序语义，允许在没有元数据更新进行时，不创建检查点就把修改过的数据块写入磁盘。这种模式等价于 ext3 文件系统的 ordered 数据模式，区别在于对数据块的更新仍然保持原子性。这会改善覆盖写入时的同步写性能。)
order=strict		Apply strict in-order semantics that preserves sequence
			of all file operations including overwriting of data
			blocks.  That means, it is guaranteed that no
			overtaking of events occurs in the recovered file
			system after a crash. (应用严格的按序语义，保留包括覆盖数据块在内的所有文件操作的先后顺序。也就是说，可以保证崩溃后恢复出来的文件系统中不会发生事件超车(乱序超越)。)
norecovery		Disable recovery of the filesystem on mount.
			This disables every write access on the device for
			read-only mounts or snapshots.  This option will fail
			for r/w mounts on an unclean volume. (在挂载时禁用文件系统的恢复。这会禁用对该设备的每一次写访问，用于只读挂载或快照。对于未干净卷上的读/写挂载，该选项会导致失败。)
discard			This enables/disables the use of discard/TRIM commands.
nodiscard(*)		The discard/TRIM commands are sent to the underlying
			block device when blocks are freed.  This is useful
			for SSD devices and sparse/thinly-provisioned LUNs. (启用/禁用 discard/TRIM 命令的使用。当块被释放时，discard/TRIM 命令会被发送到底层块设备。这对 SSD 设备和稀疏/精简配置(sparse/thinly-provisioned)的 LUN 很有用。)
======================= =======================================================

Ioctls
(Ioctls)
======

There is some NILFS2 specific functionality which can be accessed by applications
through the system call interfaces. The list of all NILFS2 specific ioctls are
shown in the table below.
(有一些 NILFS2 特有的功能，应用程序可以通过系统调用接口访问它们。所有 NILFS2 特有 ioctl 的列表见下表。)

Table of NILFS2 specific ioctls:
(NILFS2 特有 ioctl 的列表：)

 ============================== ===============================================
 Ioctl			        Description (描述)
 ============================== ===============================================
 NILFS_IOCTL_CHANGE_CPMODE      Change mode of given checkpoint between
			        checkpoint and snapshot state. This ioctl is
			        used in chcp and mkcp utilities. (在检查点状态和快照状态之间改变给定检查点的模式。该 ioctl 用于 chcp 和 mkcp 工具中。)

 NILFS_IOCTL_DELETE_CHECKPOINT  Remove checkpoint from NILFS2 file system.
			        This ioctl is used in rmcp utility. (从 NILFS2 文件系统中移除检查点。该 ioctl 用于 rmcp 工具中。)

 NILFS_IOCTL_GET_CPINFO         Return info about requested checkpoints. This
			        ioctl is used in lscp utility and by
			        nilfs_cleanerd daemon. (返回所请求检查点的信息。该 ioctl 用于 lscp 工具中，也被 nilfs_cleanerd 守护进程使用。)

 NILFS_IOCTL_GET_CPSTAT         Return checkpoints statistics. This ioctl is
			        used by lscp, rmcp utilities and by
			        nilfs_cleanerd daemon. (返回检查点统计信息。该 ioctl 被 lscp、rmcp 工具以及 nilfs_cleanerd 守护进程使用。)

 NILFS_IOCTL_GET_SUINFO         Return segment usage info about requested
			        segments. This ioctl is used in lssu,
			        nilfs_resize utilities and by nilfs_cleanerd
			        daemon. (返回所请求段(segment)的段使用信息。该 ioctl 用于 lssu、nilfs_resize 工具中，也被 nilfs_cleanerd 守护进程使用。)

 NILFS_IOCTL_SET_SUINFO         Modify segment usage info of requested
					segments. This ioctl is used by
					nilfs_cleanerd daemon to skip unnecessary
					cleaning operation of segments and reduce
					performance penalty or wear of flash device
					due to redundant move of in-use blocks. (修改所请求段的段使用信息。该 ioctl 被 nilfs_cleanerd 守护进程用来跳过不必要的段清理操作，并减少由于对使用中块的冗余搬移而造成的性能损失或闪存设备磨损。)

 NILFS_IOCTL_GET_SUSTAT         Return segment usage statistics. This ioctl
			        is used in lssu, nilfs_resize utilities and
			        by nilfs_cleanerd daemon. (返回段使用统计信息。该 ioctl 用于 lssu、nilfs_resize 工具中，也被 nilfs_cleanerd 守护进程使用。)

 NILFS_IOCTL_GET_VINFO          Return information on virtual block addresses.
			        This ioctl is used by nilfs_cleanerd daemon. (返回关于虚拟块地址的信息。该 ioctl 被 nilfs_cleanerd 守护进程使用。)

 NILFS_IOCTL_GET_BDESCS         Return information about descriptors of disk
			        block numbers. This ioctl is used by
			        nilfs_cleanerd daemon. (返回关于磁盘块号描述符的信息。该 ioctl 被 nilfs_cleanerd 守护进程使用。)

 NILFS_IOCTL_CLEAN_SEGMENTS     Do garbage collection operation in the
			        environment of requested parameters from
			        userspace. This ioctl is used by
			        nilfs_cleanerd daemon. (在来自用户空间的请求参数所构成的环境中执行垃圾回收(GC)操作。该 ioctl 被 nilfs_cleanerd 守护进程使用。)

 NILFS_IOCTL_SYNC               Make a checkpoint. This ioctl is used in
			        mkcp utility. (创建一个检查点。该 ioctl 用于 mkcp 工具中。)

 NILFS_IOCTL_RESIZE             Resize NILFS2 volume. This ioctl is used
			        by nilfs_resize utility. (调整 NILFS2 卷的大小。该 ioctl 由 nilfs_resize 工具使用。)

 NILFS_IOCTL_SET_ALLOC_RANGE    Define lower limit of segments in bytes and
			        upper limit of segments in bytes. This ioctl
			        is used by nilfs_resize utility. (以字节为单位定义段的下限和上限。该 ioctl 由 nilfs_resize 工具使用。)
 ============================== ===============================================

NILFS2 usage
(NILFS2 的使用)
============

To use nilfs2 as a local file system, simply::
(要把 nilfs2 用作本地文件系统，只需执行以下命令：)

 # mkfs -t nilfs2 /dev/block_device
 # mount -t nilfs2 /dev/block_device /dir

This will also invoke the cleaner through the mount helper program
(mount.nilfs2).
(这还会通过挂载辅助程序(mount.nilfs2)调用清理器(cleaner)。)

Checkpoints and snapshots are managed by the following commands.
Their manpages are included in the nilfs-utils package above.
(检查点和快照由以下命令管理。它们的手册页包含在上面的 nilfs-utils 软件包中。)

  ====     ===========================================================
  lscp     list checkpoints or snapshots. (列出检查点或快照。)
  mkcp     make a checkpoint or a snapshot. (创建一个检查点或快照。)
  chcp     change an existing checkpoint to a snapshot or vice versa. (把现有的检查点改成快照，或者反过来。)
  rmcp     invalidate specified checkpoint(s). (使指定的检查点失效。)
  ====     ===========================================================

To mount a snapshot::
(要挂载一个快照，执行以下命令：)

 # mount -t nilfs2 -r -o cp=<cno> /dev/block_device /snap_dir

where <cno> is the checkpoint number of the snapshot.
(其中 <cno> 是该快照的检查点编号。)

To unmount the NILFS2 mount point or snapshot, simply::
(要卸载 NILFS2 挂载点或快照，只需执行以下命令：)

 # umount /dir

Then, the cleaner daemon is automatically shut down by the umount
helper program (umount.nilfs2).
(随后，清理器守护进程会被卸载辅助程序(umount.nilfs2)自动关闭。)

Disk format
(磁盘格式)
===========

A nilfs2 volume is equally divided into a number of segments except
for the super block (SB) and segment #0.  A segment is the container
of logs.  Each log is composed of summary information blocks, payload
blocks, and an optional super root block (SR)::

   ______________________________________________________
  | |SB| | Segment | Segment | Segment | ... | Segment | |
  |_|__|_|____0____|____1____|____2____|_____|____N____|_|
  0 +1K +4K       +8M       +16M      +24M  +(8MB x N)
       .             .            (Typical offsets for 4KB-block)
    .                  .
  .______________________.
  | log | log |... | log |
  |__1__|__2__|____|__m__|
        .       .
      .               .
    .                       .
  .______________________________.
  | Summary | Payload blocks  |SR|
  |_blocks__|_________________|__|

(nilfs2 卷被平均划分为多个段(segment)，超级块(SB)和段 #0 除外。段是日志(log)的容器。每个日志由摘要信息块、有效载荷(payload)块和一个可选的超级根块(super root block, SR)组成：)

The payload blocks are organized per file, and each file consists of
data blocks and B-tree node blocks::

    |<---       File-A        --->|<---       File-B        --->|
   _______________________________________________________________
    | Data blocks | B-tree blocks | Data blocks | B-tree blocks | ...
   _|_____________|_______________|_____________|_______________|_

(有效载荷块按文件组织，每个文件由数据块和 B 树节点块组成：)


Since only the modified blocks are written in the log, it may have
files without data blocks or B-tree node blocks.
(由于日志中只写入被修改的块，因此可能存在没有数据块或 B 树节点块的文件。)

The organization of the blocks is recorded in the summary information
blocks, which contains a header structure (nilfs_segment_summary), per
file structures (nilfs_finfo), and per block structures (nilfs_binfo)::

  _________________________________________________________________________
 | Summary | finfo | binfo | ... | binfo | finfo | binfo | ... | binfo |...
 |_blocks__|___A___|_(A,1)_|_____|(A,Na)_|___B___|_(B,1)_|_____|(B,Nb)_|___

(块的布局组织记录在摘要信息块中，其中包含一个头部结构(nilfs_segment_summary)、每文件结构(nilfs_finfo)以及每块结构(nilfs_binfo)：)

The logs include regular files, directory files, symbolic link files
and several meta data files.  The meta data files are the files used
to maintain file system meta data.  The current version of NILFS2 uses
the following meta data files::
(日志中包括常规文件、目录文件、符号链接文件以及若干元数据文件。元数据文件是用于维护文件系统元数据的文件。当前版本的 NILFS2 使用以下元数据文件：)

 1) Inode file (ifile)             -- Stores on-disk inodes
    (inode 文件(ifile)——存储磁盘上的 inode)
 2) Checkpoint file (cpfile)       -- Stores checkpoints
    (检查点文件(cpfile)——存储检查点)
 3) Segment usage file (sufile)    -- Stores allocation state of segments
    (段使用文件(sufile)——存储段的分配状态)
 4) Data address translation file  -- Maps virtual block numbers to usual
    (DAT)                             block numbers.  This file serves to
                                      make on-disk blocks relocatable.
    (数据地址转换文件(DAT)——把虚拟块号映射为常规块号。该文件的作用是使磁盘上的块可以被重新定位。)

The following figure shows a typical organization of the logs::

  _________________________________________________________________________
 | Summary | regular file | file  | ... | ifile | cpfile | sufile | DAT |SR|
 |_blocks__|_or_directory_|_______|_____|_______|________|________|_____|__|

(下图展示了日志的一种典型组织方式：)

To stride over segment boundaries, this sequence of files may be split
into multiple logs.  The sequence of logs that should be treated as
logically one log, is delimited with flags marked in the segment
summary.  The recovery code of nilfs2 looks this boundary information
to ensure atomicity of updates.
(为了跨过段的边界，这一文件序列可能会被拆分成多个日志。应当被视为逻辑上单个日志的那一日志序列，用标记在段摘要中的标志来界定。nilfs2 的恢复代码会查看这一边界信息，以确保更新的原子性。)

The super root block is inserted for every checkpoints.  It includes
three special inodes, inodes for the DAT, cpfile, and sufile.  Inodes
of regular files, directories, symlinks and other special files, are
included in the ifile.  The inode of ifile itself is included in the
corresponding checkpoint entry in the cpfile.  Thus, the hierarchy
among NILFS2 files can be depicted as follows::
(超级根块会为每个检查点插入。它包括三个特殊的 inode，即 DAT、cpfile 和 sufile 的 inode。常规文件、目录、符号链接以及其他特殊文件的 inode 都包含在 ifile 中。ifile 自身的 inode 则包含在 cpfile 中对应的检查点条目里。因此，NILFS2 各文件之间的层级关系可以描绘如下：)

  Super block (SB)
       |
       v
  Super root block (the latest cno=xx)
       |-- DAT
       |-- sufile
       `-- cpfile
              |-- ifile (cno=c1)
              |-- ifile (cno=c2) ---- file (ino=i1)
              :        :          |-- file (ino=i2)
              `-- ifile (cno=xx)  |-- file (ino=i3)
                                  :        :
                                  `-- file (ino=yy)
                                    ( regular file, directory, or symlink )

For detail on the format of each file, please see nilfs2_ondisk.h
located at include/uapi/linux directory.
(有关每个文件格式的详细信息，请参见位于 include/uapi/linux 目录下的 nilfs2_ondisk.h。)

There are no patents or other intellectual property that we protect
with regard to the design of NILFS2.  It is allowed to replicate the
design in hopes that other operating systems could share (mount, read,
write, etc.) data stored in this format.
(就 NILFS2 的设计而言，我们没有用任何专利或其他知识产权来加以保护。允许复制这一设计，以期其他操作系统能够共享(挂载、读取、写入等)以这种格式存储的数据。)
