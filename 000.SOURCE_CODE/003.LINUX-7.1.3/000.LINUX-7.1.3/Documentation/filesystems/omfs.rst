.. SPDX-License-Identifier: GPL-2.0

================================
Optimized MPEG Filesystem (OMFS)
================================

(优化的 MPEG 文件系统(OMFS))

Overview
(概述)
========

OMFS is a filesystem created by SonicBlue for use in the ReplayTV DVR
and Rio Karma MP3 player.  The filesystem is extent-based, utilizing
block sizes from 2k to 8k, with hash-based directories.  This
filesystem driver may be used to read and write disks from these
devices.
(OMFS 是 SonicBlue 为 ReplayTV DVR 和 Rio Karma MP3 播放器创建的文件系统。该文件系统基于区段(extent)，使用 2k 到 8k 的块大小，并采用基于哈希的目录。此文件系统驱动程序可用于读写这些设备的磁盘。)

Note, it is not recommended that this FS be used in place of a general
filesystem for your own streaming media device.  Native Linux filesystems
will likely perform better.
(注意，不建议在你的流媒体设备上用此文件系统替代通用文件系统。原生的 Linux 文件系统性能可能更好。)

More information is available at:
(更多信息可在以下地址获取：)

    http://linux-karma.sf.net/

Various utilities, including mkomfs and omfsck, are included with
omfsprogs, available at:
(包括 mkomfs 和 omfsck 在内的各种实用工具包含在 omfsprogs 中，可从以下地址获取：)

    https://bobcopeland.com/karma/

Instructions are included in its README.
(其 README 中包含使用说明。)

Options
(选项)
=======

OMFS supports the following mount-time options:
(OMFS 支持以下挂载时选项：)

    ============   ========================================
    uid=n          make all files owned by specified user (使所有文件归指定用户所有)
    gid=n          make all files owned by specified group (使所有文件归指定组所有)
    umask=xxx      set permission umask to xxx (将权限 umask 设置为 xxx)
    fmask=xxx      set umask to xxx for files (对文件将 umask 设置为 xxx)
    dmask=xxx      set umask to xxx for directories (对目录将 umask 设置为 xxx)
    ============   ========================================

Disk format
(磁盘格式)
===========

OMFS discriminates between "sysblocks" and normal data blocks.  The sysblock
group consists of super block information, file metadata, directory structures,
and extents.  Each sysblock has a header containing CRCs of the entire
sysblock, and may be mirrored in successive blocks on the disk.  A sysblock may
have a smaller size than a data block, but since they are both addressed by the
same 64-bit block number, any remaining space in the smaller sysblock is
unused.
(OMFS 区分 "sysblock" 和普通数据块。sysblock 组由超级块信息、文件元数据、目录结构和区段(extent)组成。每个 sysblock 都有一个包含整个 sysblock 的 CRC 的头部，并且可能会在磁盘上后续的块中进行镜像。sysblock 的大小可以小于数据块，但由于两者都由相同的 64 位块号寻址，较小的 sysblock 中任何剩余的空间都不会被使用。)

Sysblock header information::
(sysblock 头部信息：)

    struct omfs_header {
	    __be64 h_self;                  /* FS block where this is located */
	    __be32 h_body_size;             /* size of useful data after header */
	    __be16 h_crc;                   /* crc-ccitt of body_size bytes */
	    char h_fill1[2];
	    u8 h_version;                   /* version, always 1 */
	    char h_type;                    /* OMFS_INODE_X */
	    u8 h_magic;                     /* OMFS_IMAGIC */
	    u8 h_check_xor;                 /* XOR of header bytes before this */
	    __be32 h_fill2;
    };

Files and directories are both represented by omfs_inode::
(文件和目录都由 omfs_inode 表示：)

    struct omfs_inode {
	    struct omfs_header i_head;      /* header */
	    __be64 i_parent;                /* parent containing this inode */
	    __be64 i_sibling;               /* next inode in hash bucket */
	    __be64 i_ctime;                 /* ctime, in milliseconds */
	    char i_fill1[35];
	    char i_type;                    /* OMFS_[DIR,FILE] */
	    __be32 i_fill2;
	    char i_fill3[64];
	    char i_name[OMFS_NAMELEN];      /* filename */
	    __be64 i_size;                  /* size of file, in bytes */
    };

Directories in OMFS are implemented as a large hash table.  Filenames are
hashed then prepended into the bucket list beginning at OMFS_DIR_START.
Lookup requires hashing the filename, then seeking across i_sibling pointers
until a match is found on i_name.  Empty buckets are represented by block
pointers with all-1s (~0).
(OMFS 中的目录实现为一个大的哈希表。文件名先被哈希，然后前插到从 OMFS_DIR_START 开始的桶链表中。查找需要先对文件名进行哈希，然后沿着 i_sibling 指针查找，直到在 i_name 上找到匹配项。空桶用全 1(~0)的块指针表示。)

A file is an omfs_inode structure followed by an extent table beginning at
OMFS_EXTENT_START::
(文件是一个 omfs_inode 结构，后跟一个从 OMFS_EXTENT_START 开始的区段(extent)表：)

    struct omfs_extent_entry {
	    __be64 e_cluster;               /* start location of a set of blocks */
	    __be64 e_blocks;                /* number of blocks after e_cluster */
    };

    struct omfs_extent {
	    __be64 e_next;                  /* next extent table location */
	    __be32 e_extent_count;          /* total # extents in this table */
	    __be32 e_fill;
	    struct omfs_extent_entry e_entry;       /* start of extent entries */
    };

Each extent holds the block offset followed by number of blocks allocated to
the extent.  The final extent in each table is a terminator with e_cluster
being ~0 and e_blocks being ones'-complement of the total number of blocks
in the table.
(每个区段(extent)保存块偏移量，随后是分配给该区段的块数。每个表中的最后一个区段是终止符，其中 e_cluster 为 ~0，e_blocks 为表中块总数的反码。)

If this table overflows, a continuation inode is written and pointed to by
e_next.  These have a header but lack the rest of the inode structure.
(如果此表溢出，则会写入一个续接 inode，并由 e_next 指向它。这些续接 inode 有一个头部，但缺少 inode 结构的其余部分。)

