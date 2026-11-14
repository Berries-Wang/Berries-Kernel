.. SPDX-License-Identifier: GPL-2.0

Orphan file
(孤儿文件)
-----------

In unix there can inodes that are unlinked from directory hierarchy but that
are still alive because they are open. In case of crash the filesystem has to
clean up these inodes as otherwise they (and the blocks referenced from them)
would leak. Similarly if we truncate or extend the file, we need not be able
to perform the operation in a single journalling transaction. In such case we
track the inode as orphan so that in case of crash extra blocks allocated to
the file get truncated.
(在 unix 中，可能存在已从目录层次结构中解除链接但仍因被打开而存活的 inode。发生崩溃时，文件系统必须清理这些 inode，否则它们(以及从它们引用的块)将会泄漏。类似地，如果我们截断或扩展文件，可能无法在单个日志事务中完成该操作。在这种情况下，我们将该 inode 跟踪为孤儿，以便在发生崩溃时，分配给该文件的额外块会被截断。)

Traditionally ext4 tracks orphan inodes in a form of single linked list where
superblock contains the inode number of the last orphan inode (s_last_orphan
field) and then each inode contains inode number of the previously orphaned
inode (we overload i_dtime inode field for this). However this filesystem
global single linked list is a scalability bottleneck for workloads that result
in heavy creation of orphan inodes. When orphan file feature
(COMPAT_ORPHAN_FILE) is enabled, the filesystem has a special inode
(referenced from the superblock through s_orphan_file_inum) with several
blocks. Each of these blocks has a structure:
(传统上，ext4 以单链表的形式跟踪孤儿 inode，其中超级块包含最后一个孤儿 inode 的编号(s_last_orphan 字段)，然后每个 inode 包含前一个被孤儿化的 inode 的编号(为此我们复用 i_dtime inode 字段)。然而，这种文件系统全局的单链表对于会产生大量孤儿 inode 的工作负载来说是一个可扩展性瓶颈。当启用孤儿文件特性(COMPAT_ORPHAN_FILE)时，文件系统有一个特殊 inode(通过 s_orphan_file_inum 从超级块引用)，它拥有若干个块。其中每个块具有如下结构：)

============= ================ =============== ===============================
Offset (偏移)        Type (类型)             Name (名称)            Description (描述)
============= ================ =============== ===============================
0x0           Array of         Orphan inode    Each __le32 entry is either
              __le32 entries (若干 __le32 组成的数组)   entries (孤儿 inode 条目)         empty (0) or it contains
	                                       inode number of an orphan
					       inode. (每个 __le32 条目要么为空(0)，要么包含一个孤儿 inode 的编号。)
blocksize-8   __le32           ob_magic        Magic value stored in orphan
                                               block tail (0x0b10ca04) (存储在孤儿块尾部的幻数(0x0b10ca04))
blocksize-4   __le32           ob_checksum     Checksum of the orphan block. (孤儿块的校验和。)
============= ================ =============== ===============================

When a filesystem with orphan file feature is writeably mounted, we set
RO_COMPAT_ORPHAN_PRESENT feature in the superblock to indicate there may
be valid orphan entries. In case we see this feature when mounting the
filesystem, we read the whole orphan file and process all orphan inodes found
there as usual. When cleanly unmounting the filesystem we remove the
RO_COMPAT_ORPHAN_PRESENT feature to avoid unnecessary scanning of the orphan
file and also make the filesystem fully compatible with older kernels.
(当带有孤儿文件特性的文件系统以可写方式挂载时，我们会在超级块中设置 RO_COMPAT_ORPHAN_PRESENT 特性，以表明可能存在有效的孤儿条目。如果在挂载文件系统时看到该特性，我们会读取整个孤儿文件并照常处理其中发现的所有孤儿 inode。干净地卸载文件系统时，我们会移除 RO_COMPAT_ORPHAN_PRESENT 特性，以避免对孤儿文件的不必要扫描，同时使文件系统与较旧的内核完全兼容。)
