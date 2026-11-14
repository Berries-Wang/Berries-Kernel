.. SPDX-License-Identifier: GPL-2.0


==============================
The Second Extended Filesystem
==============================

(第二扩展文件系统)

ext2 was originally released in January 1993.  Written by R\'emy Card,
Theodore Ts'o and Stephen Tweedie, it was a major rewrite of the
Extended Filesystem.  It is currently still (April 2001) the predominant
filesystem in use by Linux.  There are also implementations available
for NetBSD, FreeBSD, the GNU HURD, Windows 95/98/NT, OS/2 and RISC OS.
(ext2 最初于 1993 年 1 月发布。由 R\'emy Card、Theodore Ts'o 和 Stephen Tweedie 编写，它是对扩展文件系统(Extended Filesystem)的一次重大重写。目前它仍然(2001 年 4 月)是 Linux 使用中最主要的文件系统。NetBSD、FreeBSD、GNU HURD、Windows 95/98/NT、OS/2 和 RISC OS 也有可用的实现。)

Options
(选项)
=======

Most defaults are determined by the filesystem superblock, and can be
set using tune2fs(8). Kernel-determined defaults are indicated by (*).
(大多数默认值由文件系统超级块决定，并且可以使用 tune2fs(8) 设置。由内核决定的默认值用 (*) 标示。)

====================    ===     ================================================
bsddf			(*)	Makes ``df`` act like BSD. (使 ``df`` 表现得像 BSD。)
minixdf				Makes ``df`` act like Minix. (使 ``df`` 表现得像 Minix。)

check=none, nocheck	(*)	Don't do extra checking of bitmaps on mount
				(check=normal and check=strict options removed) (挂载时不做额外的位图检查(check=normal 和 check=strict 选项已被移除))

dax				Use direct access (no page cache).  See
				Documentation/filesystems/dax.rst. (使用直接访问(无页缓存)。参见 Documentation/filesystems/dax.rst。)

debug				Extra debugging information is sent to the
				kernel syslog.  Useful for developers. (额外的调试信息被发送到内核 syslog。对开发者有用。)

errors=continue			Keep going on a filesystem error. (文件系统出错时继续运行。)
errors=remount-ro		Remount the filesystem read-only on an error. (出错时将文件系统重新以只读方式挂载。)
errors=panic			Panic and halt the machine if an error occurs. (如果发生错误则 panic 并使机器停机。)

grpid, bsdgroups		Give objects the same group ID as their parent. (让对象与其父对象具有相同的组 ID。)
nogrpid, sysvgroups		New objects have the group ID of their creator. (新对象具有其创建者的组 ID。)

nouid32				Use 16-bit UIDs and GIDs. (使用 16 位 UID 和 GID。)

oldalloc			Enable the old block allocator. Orlov should
				have better performance, we'd like to get some
				feedback if it's the contrary for you. (启用旧的块分配器。Orlov 应该有更好的性能，如果在你那里情况相反，我们希望得到一些反馈。)
orlov			(*)	Use the Orlov block allocator.
				(See http://lwn.net/Articles/14633/ and
				http://lwn.net/Articles/14446/.) (使用 Orlov 块分配器。(参见 http://lwn.net/Articles/14633/ 和 http://lwn.net/Articles/14446/。))

resuid=n			The user ID which may use the reserved blocks. (可以使用保留块的用户 ID。)
resgid=n			The group ID which may use the reserved blocks. (可以使用保留块的组 ID。)

sb=n				Use alternate superblock at this location. (在此位置使用备用超级块。)

user_xattr			Enable "user." POSIX Extended Attributes
				(requires CONFIG_EXT2_FS_XATTR). (启用 "user." POSIX 扩展属性(需要 CONFIG_EXT2_FS_XATTR)。)
nouser_xattr			Don't support "user." extended attributes. (不支持 "user." 扩展属性。)

acl				Enable POSIX Access Control Lists support
				(requires CONFIG_EXT2_FS_POSIX_ACL). (启用 POSIX 访问控制列表支持(需要 CONFIG_EXT2_FS_POSIX_ACL)。)
noacl				Don't support POSIX ACLs. (不支持 POSIX ACL。)

quota, usrquota			Enable user disk quota support
				(requires CONFIG_QUOTA). (启用用户磁盘配额支持(需要 CONFIG_QUOTA)。)

grpquota			Enable group disk quota support
				(requires CONFIG_QUOTA). (启用组磁盘配额支持(需要 CONFIG_QUOTA)。)
====================    ===     ================================================

noquota option ls silently ignored by ext2.
(noquota 选项被 ext2 静默忽略。)


Specification
(规格说明)
=============

ext2 shares many properties with traditional Unix filesystems.  It has
the concepts of blocks, inodes and directories.  It has space in the
specification for Access Control Lists (ACLs), fragments, undeletion and
compression though these are not yet implemented (some are available as
separate patches).  There is also a versioning mechanism to allow new
features (such as journalling) to be added in a maximally compatible
manner.
(ext2 与传统 Unix 文件系统共享许多特性。它具有块、inode 和目录的概念。它在规格说明中为访问控制列表(ACL)、片段(fragment)、反删除和压缩保留了空间，尽管这些尚未实现(其中一些可以作为单独的补丁获得)。还有一个版本机制，允许以最大程度兼容的方式添加新特性(例如日志)。)

Blocks
(块)
------

The space in the device or file is split up into blocks.  These are
a fixed size, of 1024, 2048 or 4096 bytes (8192 bytes on Alpha systems),
which is decided when the filesystem is created.  Smaller blocks mean
less wasted space per file, but require slightly more accounting overhead,
and also impose other limits on the size of files and the filesystem.
(设备或文件中的空间被划分成块。这些块具有固定大小，为 1024、2048 或 4096 字节(在 Alpha 系统上为 8192 字节)，这是在创建文件系统时决定的。较小的块意味着每个文件浪费的空间更少，但需要稍微多一点的记账开销，并且还对文件和文件系统的大小施加了其他限制。)

Block Groups
(块组)
------------

Blocks are clustered into block groups in order to reduce fragmentation
and minimise the amount of head seeking when reading a large amount
of consecutive data.  Information about each block group is kept in a
descriptor table stored in the block(s) immediately after the superblock.
Two blocks near the start of each group are reserved for the block usage
bitmap and the inode usage bitmap which show which blocks and inodes
are in use.  Since each bitmap is limited to a single block, this means
that the maximum size of a block group is 8 times the size of a block.
(块被聚集成块组，以减少碎片并在读取大量连续数据时尽量减少磁头寻道。关于每个块组的信息保存在一个描述符表中，该表存储在紧随超级块之后的块中。每组开头附近的两个块被保留用于块使用位图和 inode 使用位图，它们显示哪些块和 inode 正在使用中。由于每个位图被限制为单个块，这意味着块组的最大大小是块大小的 8 倍。)

The block(s) following the bitmaps in each block group are designated
as the inode table for that block group and the remainder are the data
blocks.  The block allocation algorithm attempts to allocate data blocks
in the same block group as the inode which contains them.
(每个块组中位图之后的块被指定为该块组的 inode 表，其余的是数据块。块分配算法尝试在与包含它们的 inode 相同的块组中分配数据块。)

The Superblock
(超级块)
--------------

The superblock contains all the information about the configuration of
the filing system.  The primary copy of the superblock is stored at an
offset of 1024 bytes from the start of the device, and it is essential
to mounting the filesystem.  Since it is so important, backup copies of
the superblock are stored in block groups throughout the filesystem.
The first version of ext2 (revision 0) stores a copy at the start of
every block group, along with backups of the group descriptor block(s).
Because this can consume a considerable amount of space for large
filesystems, later revisions can optionally reduce the number of backup
copies by only putting backups in specific groups (this is the sparse
superblock feature).  The groups chosen are 0, 1 and powers of 3, 5 and 7.
(超级块包含关于文件系统配置的所有信息。超级块的主副本存储在距设备起始处偏移 1024 字节的位置，它对挂载文件系统至关重要。由于它如此重要，超级块的备份副本被存储在整个文件系统的各个块组中。ext2 的第一个版本(revision 0)在每个块组的开头存储一个副本，以及组描述符块的备份。因为这会为大型文件系统消耗相当多的空间，后续修订版可以选择减少备份副本的数量，只将备份放在特定的组中(这就是稀疏超级块特性)。选择的组是 0、1 以及 3、5、7 的幂。)

The information in the superblock contains fields such as the total
number of inodes and blocks in the filesystem and how many are free,
how many inodes and blocks are in each block group, when the filesystem
was mounted (and if it was cleanly unmounted), when it was modified,
what version of the filesystem it is (see the Revisions section below)
and which OS created it.
(超级块中的信息包含诸如文件系统中 inode 和块的总数及有多少空闲、每个块组中有多少 inode 和块、文件系统上次挂载的时间(以及它是否被干净地卸载)、上次修改的时间、文件系统是什么版本(参见下面的 Revisions 章节)以及是哪个操作系统创建的等字段。)

If the filesystem is revision 1 or higher, then there are extra fields,
such as a volume name, a unique identification number, the inode size,
and space for optional filesystem features to store configuration info.
(如果文件系统是 revision 1 或更高，则会有额外的字段，例如卷名、唯一的标识号、inode 大小，以及供可选文件系统特性存储配置信息的空间。)

All fields in the superblock (as in all other ext2 structures) are stored
on the disc in little endian format, so a filesystem is portable between
machines without having to know what machine it was created on.
(超级块中的所有字段(与所有其他 ext2 结构一样)都以小端格式存储在磁盘上，因此文件系统可以在机器之间移植，而不必知道它是在什么机器上创建的。)

Inodes
(inode)
------

The inode (index node) is a fundamental concept in the ext2 filesystem.
Each object in the filesystem is represented by an inode.  The inode
structure contains pointers to the filesystem blocks which contain the
data held in the object and all of the metadata about an object except
its name.  The metadata about an object includes the permissions, owner,
group, flags, size, number of blocks used, access time, change time,
modification time, deletion time, number of links, fragments, version
(for NFS) and extended attributes (EAs) and/or Access Control Lists (ACLs).
(inode(索引节点)是 ext2 文件系统中的一个基本概念。文件系统中的每个对象都由一个 inode 表示。inode 结构包含指向包含对象所持有数据的文件系统块的指针，以及除名称之外关于对象的所有元数据。关于对象的元数据包括权限、所有者、组、标志、大小、已使用的块数、访问时间、变更时间、修改时间、删除时间、链接数、片段、版本(用于 NFS)以及扩展属性(EA)和/或访问控制列表(ACL)。)

There are some reserved fields which are currently unused in the inode
structure and several which are overloaded.  One field is reserved for the
directory ACL if the inode is a directory and alternately for the top 32
bits of the file size if the inode is a regular file (allowing file sizes
larger than 2GB).  The translator field is unused under Linux, but is used
by the HURD to reference the inode of a program which will be used to
interpret this object.  Most of the remaining reserved fields have been
used up for both Linux and the HURD for larger owner and group fields,
The HURD also has a larger mode field so it uses another of the remaining
fields to store the extra more bits.
(inode 结构中有一些目前未使用的保留字段，还有几个被复用的字段。如果 inode 是目录，则有一个字段保留用于目录 ACL；如果 inode 是普通文件，则该字段转而用于文件大小的高 32 位(允许大于 2GB 的文件大小)。translator 字段在 Linux 下未使用，但 HURD 用它来引用一个程序的 inode，该程序将用于解释此对象。剩余的保留字段大部分已被 Linux 和 HURD 用作更大的所有者和组字段，HURD 还有更大的 mode 字段，因此它使用另一个剩余字段来存储额外的高位。)

There are pointers to the first 12 blocks which contain the file's data
in the inode.  There is a pointer to an indirect block (which contains
pointers to the next set of blocks), a pointer to a doubly-indirect
block (which contains pointers to indirect blocks) and a pointer to a
trebly-indirect block (which contains pointers to doubly-indirect blocks).
(inode 中有指向包含文件数据的前 12 个块的指针。还有一个指向一级间接块的指针(它包含指向下一组块的指针)、一个指向二级间接块的指针(它包含指向一级间接块的指针)以及一个指向三级间接块的指针(它包含指向二级间接块的指针)。)

The flags field contains some ext2-specific flags which aren't catered
for by the standard chmod flags.  These flags can be listed with lsattr
and changed with the chattr command, and allow specific filesystem
behaviour on a per-file basis.  There are flags for secure deletion,
undeletable, compression, synchronous updates, immutability, append-only,
dumpable, no-atime, indexed directories, and data-journaling.  Not all
of these are supported yet.
(flags 字段包含一些标准 chmod 标志未涵盖的 ext2 特有标志。这些标志可以用 lsattr 列出，并用 chattr 命令更改，并允许按文件设置特定的文件系统行为。有用于安全删除、不可删除、压缩、同步更新、不可变、仅追加、可转储、无 atime、索引目录和数据日志的标志。这些标志并非全部都已得到支持。)

Directories
(目录)
-----------

A directory is a filesystem object and has an inode just like a file.
It is a specially formatted file containing records which associate
each name with an inode number.  Later revisions of the filesystem also
encode the type of the object (file, directory, symlink, device, fifo,
socket) to avoid the need to check the inode itself for this information
(support for taking advantage of this feature does not yet exist in
Glibc 2.2).
(目录是一个文件系统对象，和文件一样拥有一个 inode。它是一个特殊格式的文件，包含将每个名称与一个 inode 号关联起来的记录。文件系统的后续修订版还编码对象的类型(文件、目录、符号链接、设备、fifo、套接字)，以避免需要检查 inode 本身来获取此信息(Glibc 2.2 中尚不存在利用此特性的支持)。)

The inode allocation code tries to assign inodes which are in the same
block group as the directory in which they are first created.
(inode 分配代码尝试分配与首次创建它们的目录位于同一块组中的 inode。)

The current implementation of ext2 uses a singly-linked list to store
the filenames in the directory; a pending enhancement uses hashing of the
filenames to allow lookup without the need to scan the entire directory.
(当前的 ext2 实现使用单向链表来存储目录中的文件名; 一项待实现的增强使用文件名的哈希，从而无需扫描整个目录即可进行查找。)

The current implementation never removes empty directory blocks once they
have been allocated to hold more files.
(当前的实现一旦为容纳更多文件而分配了目录块，就从不移除空的目录块。)

Special files
(特殊文件)
-------------

Symbolic links are also filesystem objects with inodes.  They deserve
special mention because the data for them is stored within the inode
itself if the symlink is less than 60 bytes long.  It uses the fields
which would normally be used to store the pointers to data blocks.
This is a worthwhile optimisation as it we avoid allocating a full
block for the symlink, and most symlinks are less than 60 characters long.
(符号链接也是拥有 inode 的文件系统对象。它们值得特别一提，因为如果符号链接的长度小于 60 字节，其数据就存储在 inode 本身内。它使用通常用于存储指向数据块的指针的字段。这是一项值得做的优化，因为我们避免了为符号链接分配一个完整的块，而大多数符号链接的长度都小于 60 个字符。)

Character and block special devices never have data blocks assigned to
them.  Instead, their device number is stored in the inode, again reusing
the fields which would be used to point to the data blocks.
(字符设备和块设备永远不会被分配数据块。相反，它们的设备号存储在 inode 中，同样复用了原本用于指向数据块的字段。)

Reserved Space
(保留空间)
--------------

In ext2, there is a mechanism for reserving a certain number of blocks
for a particular user (normally the super-user).  This is intended to
allow for the system to continue functioning even if non-privileged users
fill up all the space available to them (this is independent of filesystem
quotas).  It also keeps the filesystem from filling up entirely which
helps combat fragmentation.
(在 ext2 中，有一种为特定用户(通常是超级用户)保留一定数量块的机制。这是为了让系统即使在非特权用户填满了他们可用的所有空间时也能继续运行(这与文件系统配额无关)。它还防止文件系统被完全填满，这有助于对抗碎片化。)

Filesystem check
(文件系统检查)
----------------

At boot time, most systems run a consistency check (e2fsck) on their
filesystems.  The superblock of the ext2 filesystem contains several
fields which indicate whether fsck should actually run (since checking
the filesystem at boot can take a long time if it is large).  fsck will
run if the filesystem was not cleanly unmounted, if the maximum mount
count has been exceeded or if the maximum time between checks has been
exceeded.
(在启动时，大多数系统会对它们的文件系统运行一致性检查(e2fsck)。ext2 文件系统的超级块包含几个字段，用于指示 fsck 是否真的应该运行(因为如果文件系统很大，在启动时检查它可能需要很长时间)。如果文件系统未被干净地卸载、超过了最大挂载次数或超过了检查之间的最大时间间隔，fsck 将会运行。)

Feature Compatibility
(特性兼容性)
---------------------

The compatibility feature mechanism used in ext2 is sophisticated.
It safely allows features to be added to the filesystem, without
unnecessarily sacrificing compatibility with older versions of the
filesystem code.  The feature compatibility mechanism is not supported by
the original revision 0 (EXT2_GOOD_OLD_REV) of ext2, but was introduced in
revision 1.  There are three 32-bit fields, one for compatible features
(COMPAT), one for read-only compatible (RO_COMPAT) features and one for
incompatible (INCOMPAT) features.
(ext2 使用的特性兼容机制相当精巧。它安全地允许向文件系统添加特性，而不必牺牲与旧版本文件系统代码的兼容性。ext2 最初的 revision 0(EXT2_GOOD_OLD_REV)不支持特性兼容机制，它是在 revision 1 中引入的。有三个 32 位字段，一个用于兼容特性(COMPAT)，一个用于只读兼容(RO_COMPAT)特性，一个用于不兼容(INCOMPAT)特性。)

These feature flags have specific meanings for the kernel as follows:
(这些特性标志对内核具有如下特定含义:)

A COMPAT flag indicates that a feature is present in the filesystem,
but the on-disk format is 100% compatible with older on-disk formats, so
a kernel which didn't know anything about this feature could read/write
the filesystem without any chance of corrupting the filesystem (or even
making it inconsistent).  This is essentially just a flag which says
"this filesystem has a (hidden) feature" that the kernel or e2fsck may
want to be aware of (more on e2fsck and feature flags later).  The ext3
HAS_JOURNAL feature is a COMPAT flag because the ext3 journal is simply
a regular file with data blocks in it so the kernel does not need to
take any special notice of it if it doesn't understand ext3 journaling.
(COMPAT 标志表示文件系统中存在某个特性，但其磁盘格式与旧的磁盘格式 100% 兼容，因此对此特性一无所知的内核可以读写该文件系统，而不会有损坏文件系统(或甚至使其不一致)的任何可能。这本质上只是一个表示"此文件系统有一个(隐藏的)特性"的标志，内核或 e2fsck 可能需要留意它(e2fsck 和特性标志的更多信息见后文)。ext3 的 HAS_JOURNAL 特性是一个 COMPAT 标志，因为 ext3 日志只是一个包含数据块的普通文件，所以如果内核不理解 ext3 日志，它不需要对它加以任何特别注意。)

An RO_COMPAT flag indicates that the on-disk format is 100% compatible
with older on-disk formats for reading (i.e. the feature does not change
the visible on-disk format).  However, an old kernel writing to such a
filesystem would/could corrupt the filesystem, so this is prevented. The
most common such feature, SPARSE_SUPER, is an RO_COMPAT feature because
sparse groups allow file data blocks where superblock/group descriptor
backups used to live, and ext2_free_blocks() refuses to free these blocks,
which would leading to inconsistent bitmaps.  An old kernel would also
get an error if it tried to free a series of blocks which crossed a group
boundary, but this is a legitimate layout in a SPARSE_SUPER filesystem.
(RO_COMPAT 标志表示磁盘格式在读取方面与旧的磁盘格式 100% 兼容(即该特性不改变可见的磁盘格式)。然而，旧内核向这样的文件系统写入会/可能损坏文件系统，因此这是被阻止的。最常见的此类特性 SPARSE_SUPER 是一个 RO_COMPAT 特性，因为稀疏组允许在超级块/组描述符备份曾经所在的位置放置文件数据块，而 ext2_free_blocks() 拒绝释放这些块，否则会导致位图不一致。旧内核在尝试释放跨越组边界的一系列块时也会得到错误，但这是 SPARSE_SUPER 文件系统中的合法布局。)

An INCOMPAT flag indicates the on-disk format has changed in some
way that makes it unreadable by older kernels, or would otherwise
cause a problem if an old kernel tried to mount it.  FILETYPE is an
INCOMPAT flag because older kernels would think a filename was longer
than 256 characters, which would lead to corrupt directory listings.
The COMPRESSION flag is an obvious INCOMPAT flag - if the kernel
doesn't understand compression, you would just get garbage back from
read() instead of it automatically decompressing your data.  The ext3
RECOVER flag is needed to prevent a kernel which does not understand the
ext3 journal from mounting the filesystem without replaying the journal.
(INCOMPAT 标志表示磁盘格式发生了某种改变，使得旧内核无法读取它，或者旧内核尝试挂载它时会引发问题。FILETYPE 是一个 INCOMPAT 标志，因为旧内核会认为文件名长于 256 个字符，这会导致目录列表损坏。COMPRESSION 标志是一个显而易见的 INCOMPAT 标志——如果内核不理解压缩，你从 read() 得到的将只是垃圾数据，而不是自动解压你的数据。ext3 的 RECOVER 标志用于阻止不理解 ext3 日志的内核在没有重放日志的情况下挂载文件系统。)

For e2fsck, it needs to be more strict with the handling of these
flags than the kernel.  If it doesn't understand ANY of the COMPAT,
RO_COMPAT, or INCOMPAT flags it will refuse to check the filesystem,
because it has no way of verifying whether a given feature is valid
or not.  Allowing e2fsck to succeed on a filesystem with an unknown
feature is a false sense of security for the user.  Refusing to check
a filesystem with unknown features is a good incentive for the user to
update to the latest e2fsck.  This also means that anyone adding feature
flags to ext2 also needs to update e2fsck to verify these features.
(对于 e2fsck 来说，它需要比内核更严格地处理这些标志。如果它不理解任何 COMPAT、RO_COMPAT 或 INCOMPAT 标志，它将拒绝检查该文件系统，因为它无法验证给定的特性是否有效。让 e2fsck 在带有未知特性的文件系统上成功通过，会给用户一种虚假的安全感。拒绝检查带有未知特性的文件系统是促使用户更新到最新 e2fsck 的好动力。这也意味着任何向 ext2 添加特性标志的人还需要更新 e2fsck 以验证这些特性。)

Metadata
(元数据)
--------

It is frequently claimed that the ext2 implementation of writing
asynchronous metadata is faster than the ffs synchronous metadata
scheme but less reliable.  Both methods are equally resolvable by their
respective fsck programs.
(人们经常声称 ext2 写异步元数据的实现比 ffs 的同步元数据方案更快但可靠性较低。这两种方法都可以通过各自的 fsck 程序同等地解决。)

If you're exceptionally paranoid, there are 3 ways of making metadata
writes synchronous on ext2:
(如果你格外偏执，有 3 种方法可以让 ext2 上的元数据写入变成同步的:)

- per-file if you have the program source: use the O_SYNC flag to open()
  (按文件(如果你有程序源码): 使用 O_SYNC 标志调用 open())
- per-file if you don't have the source: use "chattr +S" on the file
  (按文件(如果你没有源码): 对该文件使用 "chattr +S")
- per-filesystem: add the "sync" option to mount (or in /etc/fstab)
  (按文件系统: 在 mount 中添加 "sync" 选项(或在 /etc/fstab 中))

the first and last are not ext2 specific but do force the metadata to
be written synchronously.  See also Journaling below.
(第一种和最后一种不是 ext2 特有的，但它们确实会强制元数据被同步写入。另请参见下面的 Journaling 一节。)

Limitations
(限制)
-----------

There are various limits imposed by the on-disk layout of ext2.  Other
limits are imposed by the current implementation of the kernel code.
Many of the limits are determined at the time the filesystem is first
created, and depend upon the block size chosen.  The ratio of inodes to
data blocks is fixed at filesystem creation time, so the only way to
increase the number of inodes is to increase the size of the filesystem.
No tools currently exist which can change the ratio of inodes to blocks.
(ext2 的磁盘布局施加了各种限制。其他限制则由内核代码的当前实现施加。许多限制是在文件系统首次创建时确定的，并取决于所选择的块大小。inode 与数据块的比例在文件系统创建时是固定的，因此增加 inode 数量的唯一方法是增大文件系统的大小。目前没有任何工具可以更改 inode 与块的比例。)

Most of these limits could be overcome with slight changes in the on-disk
format and using a compatibility flag to signal the format change (at
the expense of some compatibility).
(这些限制中的大多数可以通过对磁盘格式的轻微改动并使用一个兼容性标志来发出格式更改的信号来克服(以牺牲一些兼容性为代价)。)

=====================  =======    =======    =======   ========
Filesystem block size      1kB        2kB        4kB        8kB (文件系统块大小)
=====================  =======    =======    =======   ========
File size limit           16GB      256GB     2048GB     2048GB (文件大小限制)
Filesystem size limit   2047GB     8192GB    16384GB    32768GB (文件系统大小限制)
=====================  =======    =======    =======   ========

There is a 2.4 kernel limit of 2048GB for a single block device, so no
filesystem larger than that can be created at this time.  There is also
an upper limit on the block size imposed by the page size of the kernel,
so 8kB blocks are only allowed on Alpha systems (and other architectures
which support larger pages).
(单个块设备有 2.4 内核 2048GB 的限制，因此目前无法创建大于该值的文件系统。内核页大小也对块大小施加了上限，因此 8kB 的块只允许在 Alpha 系统(以及其他支持更大页的架构)上使用。)

There is an upper limit of 32000 subdirectories in a single directory.
(单个目录中子目录数量的上限为 32000。)

There is a "soft" upper limit of about 10-15k files in a single directory
with the current linear linked-list directory implementation.  This limit
stems from performance problems when creating and deleting (and also
finding) files in such large directories.  Using a hashed directory index
(under development) allows 100k-1M+ files in a single directory without
performance problems (although RAM size becomes an issue at this point).
(使用当前的线性链表目录实现，单个目录中的文件数有一个约 10-15k 的"软"上限。此限制源于在这种大目录中创建和删除(以及查找)文件时的性能问题。使用哈希目录索引(开发中)可以在单个目录中容纳 100k-1M+ 个文件而没有性能问题(尽管此时内存大小会成为问题)。)

The (meaningless) absolute upper limit of files in a single directory
(imposed by the file size, the realistic limit is obviously much less)
is over 130 trillion files.  It would be higher except there are not
enough 4-character names to make up unique directory entries, so they
have to be 8 character filenames, even then we are fairly close to
running out of unique filenames.
(单个目录中文件数的(无意义的)绝对上限(由文件大小决定，现实中的限制显然要小得多)超过 130 万亿个文件。本来还可以更高，只是 4 字符的名称不足以构成唯一的目录项，所以必须使用 8 字符的文件名，即便如此我们也已相当接近用尽唯一的文件名了。)

Journaling
(日志)
----------

A journaling extension to the ext2 code has been developed by Stephen
Tweedie.  It avoids the risks of metadata corruption and the need to
wait for e2fsck to complete after a crash, without requiring a change
to the on-disk ext2 layout.  In a nutshell, the journal is a regular
file which stores whole metadata (and optionally data) blocks that have
been modified, prior to writing them into the filesystem.  This means
it is possible to add a journal to an existing ext2 filesystem without
the need for data conversion.
(Stephen Tweedie 为 ext2 代码开发了日志扩展。它避免了元数据损坏的风险以及崩溃后等待 e2fsck 完成的需要，而无需更改 ext2 的磁盘布局。简而言之，日志是一个普通文件，它在将修改过的完整元数据(以及可选的数据)块写入文件系统之前存储它们。这意味着可以向现有的 ext2 文件系统添加日志，而无需数据转换。)

When changes to the filesystem (e.g. a file is renamed) they are stored in
a transaction in the journal and can either be complete or incomplete at
the time of a crash.  If a transaction is complete at the time of a crash
(or in the normal case where the system does not crash), then any blocks
in that transaction are guaranteed to represent a valid filesystem state,
and are copied into the filesystem.  If a transaction is incomplete at
the time of the crash, then there is no guarantee of consistency for
the blocks in that transaction so they are discarded (which means any
filesystem changes they represent are also lost).
Check Documentation/filesystems/ext4/ if you want to read more about
ext4 and journaling.
(当对文件系统进行更改时(例如重命名一个文件)，它们会被存储在日志中的一个事务里，并且在崩溃发生时可以是完整的或不完整的。如果事务在崩溃发生时是完整的(或者在系统不崩溃的正常情况下)，则该事务中的任何块都保证代表一个有效的文件系统状态，并被复制进文件系统。如果事务在崩溃发生时不完整，则无法保证该事务中块的连贯性，因此它们会被丢弃(这意味着它们所代表的任何文件系统更改也将丢失)。如果你想阅读更多关于 ext4 和日志的内容，请查看 Documentation/filesystems/ext4/。)

References
(参考资料)
==========

=======================	===============================================
The kernel source	file:/usr/src/linux/fs/ext2/ (内核源码)
e2fsprogs (e2fsck)	http://e2fsprogs.sourceforge.net/
Design & Implementation	http://e2fsprogs.sourceforge.net/ext2intro.html (设计与实现)
Journaling (ext3)	ftp://ftp.uk.linux.org/pub/linux/sct/fs/jfs/ (日志(ext3))
Filesystem Resizing	http://ext2resize.sourceforge.net/ (文件系统调整大小)
Compression [1]_	http://e2compr.sourceforge.net/ (压缩)
=======================	===============================================

Implementations for:
(以下平台的实现：)

=======================	===========================================================
Windows 95/98/NT/2000	http://www.chrysocome.net/explore2fs
Windows 95 [1]_		http://www.yipton.net/content/fsdext2/
DOS client [1]_		ftp://metalab.unc.edu/pub/Linux/system/filesystems/ext2/ (DOS 客户端)
OS/2 [2]_		ftp://metalab.unc.edu/pub/Linux/system/filesystems/ext2/
RISC OS client		http://www.esw-heim.tu-clausthal.de/~marco/smorbrod/IscaFS/ (RISC OS 客户端)
=======================	===========================================================

.. [1] no longer actively developed/supported (as of Apr 2001)
       (不再被积极开发/支持(截至 2001 年 4 月))
.. [2] no longer actively developed/supported (as of Mar 2009)
       (不再被积极开发/支持(截至 2009 年 3 月))
