.. SPDX-License-Identifier: GPL-2.0

Index Nodes
(索引节点)
-----------

In a regular UNIX filesystem, the inode stores all the metadata
pertaining to the file (time stamps, block maps, extended attributes,
etc), not the directory entry. To find the information associated with a
file, one must traverse the directory files to find the directory entry
associated with a file, then load the inode to find the metadata for
that file. ext4 appears to cheat (for performance reasons) a little bit
by storing a copy of the file type (normally stored in the inode) in the
directory entry. (Compare all this to FAT, which stores all the file
information directly in the directory entry, but does not support hard
links and is in general more seek-happy than ext4 due to its simpler
block allocator and extensive use of linked lists.)
(在常规的 UNIX 文件系统中，inode 存储与文件相关的所有元数据(时间戳、块映射、扩展属性等)，而不存储目录项。要查找与某个文件关联的信息，必须遍历目录文件以找到与该文件关联的目录项，然后加载 inode 以找到该文件的元数据。ext4 似乎(出于性能原因)作了一点弊：在目录项中存储了一份文件类型(通常存储在 inode 中)的副本。(把这一切与 FAT 比较一下：FAT 将所有文件信息直接存储在目录项中，但不支持硬链接，并且由于其更简单的块分配器和大量使用链表，总体上比 ext4 更热衷于寻道。))

The inode table is a linear array of ``struct ext4_inode``. The table is
sized to have enough blocks to store at least
``sb.s_inode_size * sb.s_inodes_per_group`` bytes. The number of the
block group containing an inode can be calculated as
``(inode_number - 1) / sb.s_inodes_per_group``, and the offset into the
group's table is ``(inode_number - 1) % sb.s_inodes_per_group``. There
is no inode 0.
(inode 表是 ``struct ext4_inode`` 的线性数组。该表的大小被设定为拥有足以存储至少 ``sb.s_inode_size * sb.s_inodes_per_group`` 字节的块。包含某个 inode 的块组编号可以计算为 ``(inode_number - 1) / sb.s_inodes_per_group``，而它在该组表中的偏移量为 ``(inode_number - 1) % sb.s_inodes_per_group``。不存在 inode 0。)

The inode checksum is calculated against the FS UUID, the inode number,
and the inode structure itself.
(inode 校验和是针对 FS UUID、inode 编号以及 inode 结构本身计算的。)

The inode table entry is laid out in ``struct ext4_inode``.
(inode 表项的布局由 ``struct ext4_inode`` 定义。)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1
   :class: longtable

   * - Offset
       (偏移量)
     - Size
       (大小)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le16
     - i_mode
     - File mode. See the table i_mode_ below.
       (文件模式。参见下文的 i_mode_ 表。)
   * - 0x2
     - __le16
     - i_uid
     - Lower 16-bits of Owner UID.
       (所有者 UID 的低 16 位。)
   * - 0x4
     - __le32
     - i_size_lo
     - Lower 32-bits of size in bytes.
       (以字节为单位的大小值的低 32 位。)
   * - 0x8
     - __le32
     - i_atime
     - Last access time, in seconds since the epoch. However, if the EA_INODE
       inode flag is set, this inode stores an extended attribute value and
       this field contains the checksum of the value.
       (最后访问时间，自纪元起的秒数。但是，如果设置了 EA_INODE inode 标志，此 inode 存储的是一个扩展属性值，此字段包含该值的校验和。)
   * - 0xC
     - __le32
     - i_ctime
     - Last inode change time, in seconds since the epoch. However, if the
       EA_INODE inode flag is set, this inode stores an extended attribute
       value and this field contains the lower 32 bits of the attribute value's
       reference count.
       (inode 最后变更时间，自纪元起的秒数。但是，如果设置了 EA_INODE inode 标志，此 inode 存储的是一个扩展属性值，此字段包含该属性值引用计数的低 32 位。)
   * - 0x10
     - __le32
     - i_mtime
     - Last data modification time, in seconds since the epoch. However, if the
       EA_INODE inode flag is set, this inode stores an extended attribute
       value and this field contains the number of the inode that owns the
       extended attribute.
       (最后数据修改时间，自纪元起的秒数。但是，如果设置了 EA_INODE inode 标志，此 inode 存储的是一个扩展属性值，此字段包含拥有该扩展属性的 inode 的编号。)
   * - 0x14
     - __le32
     - i_dtime
     - Deletion Time, in seconds since the epoch.
       (删除时间，自纪元起的秒数。)
   * - 0x18
     - __le16
     - i_gid
     - Lower 16-bits of GID.
       (GID 的低 16 位。)
   * - 0x1A
     - __le16
     - i_links_count
     - Hard link count. Normally, ext4 does not permit an inode to have more
       than 65,000 hard links. This applies to files as well as directories,
       which means that there cannot be more than 64,998 subdirectories in a
       directory (each subdirectory's '..' entry counts as a hard link, as does
       the '.' entry in the directory itself). With the DIR_NLINK feature
       enabled, ext4 supports more than 64,998 subdirectories by setting this
       field to 1 to indicate that the number of hard links is not known.
       (硬链接计数。通常，ext4 不允许一个 inode 拥有超过 65,000 个硬链接。这不仅适用于文件，也适用于目录，这意味着一个目录中不能有超过 64,998 个子目录(每个子目录的 '..' 条目都计作一个硬链接，目录自身的 '.' 条目也是如此)。在启用 DIR_NLINK 特性的情况下，ext4 通过将此字段设置为 1 来表示硬链接数量未知，从而支持多于 64,998 个子目录。)
   * - 0x1C
     - __le32
     - i_blocks_lo
     - Lower 32-bits of “block” count. If the huge_file feature flag is not
       set on the filesystem, the file consumes ``i_blocks_lo`` 512-byte blocks
       on disk. If huge_file is set and EXT4_HUGE_FILE_FL is NOT set in
       ``inode.i_flags``, then the file consumes ``i_blocks_lo + (i_blocks_hi
       << 32)`` 512-byte blocks on disk. If huge_file is set and
       EXT4_HUGE_FILE_FL IS set in ``inode.i_flags``, then this file
       consumes (``i_blocks_lo + i_blocks_hi`` << 32) filesystem blocks on
       disk.
       (“块” 计数的低 32 位。如果文件系统上未设置 huge_file 特性标志，文件在磁盘上消耗 ``i_blocks_lo`` 个 512 字节块。如果设置了 huge_file 且 ``inode.i_flags`` 中未设置 EXT4_HUGE_FILE_FL，则文件在磁盘上消耗 ``i_blocks_lo + (i_blocks_hi << 32)`` 个 512 字节块。如果设置了 huge_file 且 ``inode.i_flags`` 中设置了 EXT4_HUGE_FILE_FL，则此文件在磁盘上消耗 (``i_blocks_lo + i_blocks_hi`` << 32) 个文件系统块。)
   * - 0x20
     - __le32
     - i_flags
     - Inode flags. See the table i_flags_ below.
       (inode 标志。参见下文的 i_flags_ 表。)
   * - 0x24
     - 4 bytes
     - i_osd1
     - See the table i_osd1_ for more details.
       (更多详情请参见 i_osd1_ 表。)
   * - 0x28
     - 60 bytes
     - i_block[EXT4_N_BLOCKS=15]
     - Block map or extent tree. See the section “The Contents of inode.i_block”.
       (块映射或 extent 树。参见 “The Contents of inode.i_block”(inode.i_block 的内容)一节。)
   * - 0x64
     - __le32
     - i_generation
     - File version (for NFS).
       (文件版本(用于 NFS)。)
   * - 0x68
     - __le32
     - i_file_acl_lo
     - Lower 32-bits of extended attribute block. ACLs are of course one of
       many possible extended attributes; I think the name of this field is a
       result of the first use of extended attributes being for ACLs.
       (扩展属性块的低 32 位。当然，ACL 只是众多可能的扩展属性之一；我认为这个字段的命名源于扩展属性的首次使用是为 ACL。)
   * - 0x6C
     - __le32
     - i_size_high / i_dir_acl
     - Upper 32-bits of file/directory size. In ext2/3 this field was named
       i_dir_acl, though it was usually set to zero and never used.
       (文件/目录大小的高 32 位。在 ext2/3 中此字段名为 i_dir_acl，不过它通常被设为零且从未使用。)
   * - 0x70
     - __le32
     - i_obso_faddr
     - (Obsolete) fragment address.
       ((已废弃)碎片地址。)
   * - 0x74
     - 12 bytes
     - i_osd2
     - See the table i_osd2_ for more details.
       (更多详情请参见 i_osd2_ 表。)
   * - 0x80
     - __le16
     - i_extra_isize
     - Size of this inode - 128. Alternately, the size of the extended inode
       fields beyond the original ext2 inode, including this field.
       (此 inode 的大小减 128。或者说是超出原始 ext2 inode 的扩展 inode 字段的大小，包括此字段。)
   * - 0x82
     - __le16
     - i_checksum_hi
     - Upper 16-bits of the inode checksum.
       (inode 校验和的高 16 位。)
   * - 0x84
     - __le32
     - i_ctime_extra
     - Extra change time bits. This provides sub-second precision. See Inode
       Timestamps section.
       (额外的变更时间位。提供亚秒级精度。参见 Inode Timestamps 一节。)
   * - 0x88
     - __le32
     - i_mtime_extra
     - Extra modification time bits. This provides sub-second precision.
       (额外的修改时间位。提供亚秒级精度。)
   * - 0x8C
     - __le32
     - i_atime_extra
     - Extra access time bits. This provides sub-second precision.
       (额外的访问时间位。提供亚秒级精度。)
   * - 0x90
     - __le32
     - i_crtime
     - File creation time, in seconds since the epoch.
       (文件创建时间，自纪元起的秒数。)
   * - 0x94
     - __le32
     - i_crtime_extra
     - Extra file creation time bits. This provides sub-second precision.
       (额外的文件创建时间位。提供亚秒级精度。)
   * - 0x98
     - __le32
     - i_version_hi
     - Upper 32-bits for version number.
       (版本号的高 32 位。)
   * - 0x9C
     - __le32
     - i_projid
     - Project ID.
       (项目 ID。)

.. _i_mode:

The ``i_mode`` value is a combination of the following flags:
(``i_mode`` 的值是以下标志的组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - S_IXOTH (Others may execute)
       (S_IXOTH (其他人可执行))
   * - 0x2
     - S_IWOTH (Others may write)
       (S_IWOTH (其他人可写))
   * - 0x4
     - S_IROTH (Others may read)
       (S_IROTH (其他人可读))
   * - 0x8
     - S_IXGRP (Group members may execute)
       (S_IXGRP (组成员可执行))
   * - 0x10
     - S_IWGRP (Group members may write)
       (S_IWGRP (组成员可写))
   * - 0x20
     - S_IRGRP (Group members may read)
       (S_IRGRP (组成员可读))
   * - 0x40
     - S_IXUSR (Owner may execute)
       (S_IXUSR (所有者可执行))
   * - 0x80
     - S_IWUSR (Owner may write)
       (S_IWUSR (所有者可写))
   * - 0x100
     - S_IRUSR (Owner may read)
       (S_IRUSR (所有者可读))
   * - 0x200
     - S_ISVTX (Sticky bit)
       (S_ISVTX (粘滞位))
   * - 0x400
     - S_ISGID (Set GID)
       (S_ISGID (设置 GID))
   * - 0x800
     - S_ISUID (Set UID)
       (S_ISUID (设置 UID))
   * -
     - These are mutually-exclusive file types:
       (这些是互斥的文件类型：)
   * - 0x1000
     - S_IFIFO (FIFO)
       (S_IFIFO (FIFO))
   * - 0x2000
     - S_IFCHR (Character device)
       (S_IFCHR (字符设备))
   * - 0x4000
     - S_IFDIR (Directory)
       (S_IFDIR (目录))
   * - 0x6000
     - S_IFBLK (Block device)
       (S_IFBLK (块设备))
   * - 0x8000
     - S_IFREG (Regular file)
       (S_IFREG (常规文件))
   * - 0xA000
     - S_IFLNK (Symbolic link)
       (S_IFLNK (符号链接))
   * - 0xC000
     - S_IFSOCK (Socket)
       (S_IFSOCK (套接字))

.. _i_flags:

The ``i_flags`` field is a combination of these values:
(``i_flags`` 字段是以下值的组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - This file requires secure deletion (EXT4_SECRM_FL). (not implemented)
       (此文件需要安全删除 (EXT4_SECRM_FL)。(未实现))
   * - 0x2
     - This file should be preserved, should undeletion be desired
       (EXT4_UNRM_FL). (not implemented)
       (如果希望撤销删除，此文件应被保留 (EXT4_UNRM_FL)。(未实现))
   * - 0x4
     - File is compressed (EXT4_COMPR_FL). (not really implemented)
       (文件已压缩 (EXT4_COMPR_FL)。(并未真正实现))
   * - 0x8
     - All writes to the file must be synchronous (EXT4_SYNC_FL).
       (对该文件的所有写入必须是同步的 (EXT4_SYNC_FL)。)
   * - 0x10
     - File is immutable (EXT4_IMMUTABLE_FL).
       (文件不可变 (EXT4_IMMUTABLE_FL)。)
   * - 0x20
     - File can only be appended (EXT4_APPEND_FL).
       (文件只能追加 (EXT4_APPEND_FL)。)
   * - 0x40
     - The dump(1) utility should not dump this file (EXT4_NODUMP_FL).
       (dump(1) 工具不应转储此文件 (EXT4_NODUMP_FL)。)
   * - 0x80
     - Do not update access time (EXT4_NOATIME_FL).
       (不更新访问时间 (EXT4_NOATIME_FL)。)
   * - 0x100
     - Dirty compressed file (EXT4_DIRTY_FL). (not used)
       (已压缩文件为脏 (EXT4_DIRTY_FL)。(未使用))
   * - 0x200
     - File has one or more compressed clusters (EXT4_COMPRBLK_FL). (not used)
       (文件有一个或多个压缩簇 (EXT4_COMPRBLK_FL)。(未使用))
   * - 0x400
     - Do not compress file (EXT4_NOCOMPR_FL). (not used)
       (不压缩文件 (EXT4_NOCOMPR_FL)。(未使用))
   * - 0x800
     - Encrypted inode (EXT4_ENCRYPT_FL). This bit value previously was
       EXT4_ECOMPR_FL (compression error), which was never used.
       (加密的 inode (EXT4_ENCRYPT_FL)。此位的值以前是 EXT4_ECOMPR_FL (压缩错误)，从未使用过。)
   * - 0x1000
     - Directory has hashed indexes (EXT4_INDEX_FL).
       (目录拥有哈希索引 (EXT4_INDEX_FL)。)
   * - 0x2000
     - AFS magic directory (EXT4_IMAGIC_FL).
       (AFS 魔术目录 (EXT4_IMAGIC_FL)。)
   * - 0x4000
     - File data must always be written through the journal
       (EXT4_JOURNAL_DATA_FL).
       (文件数据必须始终通过日志写入 (EXT4_JOURNAL_DATA_FL)。)
   * - 0x8000
     - File tail should not be merged (EXT4_NOTAIL_FL). (not used by ext4)
       (文件尾部不应被合并 (EXT4_NOTAIL_FL)。(ext4 不使用))
   * - 0x10000
     - All directory entry data should be written synchronously (see
       ``dirsync``) (EXT4_DIRSYNC_FL).
       (所有目录项数据都应同步写入 (参见 ``dirsync``) (EXT4_DIRSYNC_FL)。)
   * - 0x20000
     - Top of directory hierarchy (EXT4_TOPDIR_FL).
       (目录层次结构的顶层 (EXT4_TOPDIR_FL)。)
   * - 0x40000
     - This is a huge file (EXT4_HUGE_FILE_FL).
       (这是一个大文件 (EXT4_HUGE_FILE_FL)。)
   * - 0x80000
     - Inode uses extents (EXT4_EXTENTS_FL).
       (inode 使用 extent (EXT4_EXTENTS_FL)。)
   * - 0x100000
     - Verity protected file (EXT4_VERITY_FL).
       (受 verity 保护的文件 (EXT4_VERITY_FL)。)
   * - 0x200000
     - Inode stores a large extended attribute value in its data blocks
       (EXT4_EA_INODE_FL).
       (inode 在其数据块中存储一个大的扩展属性值 (EXT4_EA_INODE_FL)。)
   * - 0x400000
     - This file has blocks allocated past EOF (EXT4_EOFBLOCKS_FL).
       (deprecated)
       (此文件分配了超出 EOF 的块 (EXT4_EOFBLOCKS_FL)。(已废弃))
   * - 0x01000000
     - Inode is a snapshot (``EXT4_SNAPFILE_FL``). (not in mainline)
       (inode 是一个快照 (``EXT4_SNAPFILE_FL``)。(不在主线中))
   * - 0x04000000
     - Snapshot is being deleted (``EXT4_SNAPFILE_DELETED_FL``). (not in
       mainline)
       (快照正在被删除 (``EXT4_SNAPFILE_DELETED_FL``)。(不在主线中))
   * - 0x08000000
     - Snapshot shrink has completed (``EXT4_SNAPFILE_SHRUNK_FL``). (not in
       mainline)
       (快照收缩已完成 (``EXT4_SNAPFILE_SHRUNK_FL``)。(不在主线中))
   * - 0x10000000
     - Inode has inline data (EXT4_INLINE_DATA_FL).
       (inode 拥有内联数据 (EXT4_INLINE_DATA_FL)。)
   * - 0x20000000
     - Create children with the same project ID (EXT4_PROJINHERIT_FL).
       (使用相同项目 ID 创建子项 (EXT4_PROJINHERIT_FL)。)
   * - 0x40000000
     - Use case-insensitive lookups for directory contents (EXT4_CASEFOLD_FL).
       (对目录内容使用大小写不敏感的查找 (EXT4_CASEFOLD_FL)。)
   * - 0x80000000
     - Reserved for ext4 library (EXT4_RESERVED_FL).
       (为 ext4 库保留 (EXT4_RESERVED_FL)。)
   * -
     - Aggregate flags:
       (聚合标志：)
   * - 0x705BDFFF
     - User-visible flags.
       (用户可见的标志。)
   * - 0x604BC0FF
     - User-modifiable flags. Note that while EXT4_JOURNAL_DATA_FL and
       EXT4_EXTENTS_FL can be set with setattr, they are not in the kernel's
       EXT4_FL_USER_MODIFIABLE mask, since it needs to handle the setting of
       these flags in a special manner and they are masked out of the set of
       flags that are saved directly to i_flags.
       (用户可修改的标志。注意，虽然 EXT4_JOURNAL_DATA_FL 和 EXT4_EXTENTS_FL 可以通过 setattr 设置，但它们不在内核的 EXT4_FL_USER_MODIFIABLE 掩码中，因为内核需要以特殊方式处理这些标志的设置，所以它们被从直接保存到 i_flags 的标志集合中屏蔽掉了。)

.. _i_osd1:

The ``osd1`` field has multiple meanings depending on the creator:
(``osd1`` 字段根据创建者的不同有多种含义：)

Linux:
(Linux:)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Size
       (大小)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le32
     - l_i_version
     - Inode version. However, if the EA_INODE inode flag is set, this inode
       stores an extended attribute value and this field contains the upper 32
       bits of the attribute value's reference count.
       (inode 版本。但是，如果设置了 EA_INODE inode 标志，此 inode 存储的是一个扩展属性值，此字段包含该属性值引用计数的高 32 位。)

Hurd:
(Hurd:)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Size
       (大小)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le32
     - h_i_translator
     - ??

Masix:
(Masix:)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Size
       (大小)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le32
     - m_i_reserved
     - ??

.. _i_osd2:

The ``osd2`` field has multiple meanings depending on the filesystem creator:
(``osd2`` 字段根据文件系统创建者的不同有多种含义：)

Linux:
(Linux:)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Size
       (大小)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le16
     - l_i_blocks_high
     - Upper 16-bits of the block count. Please see the note attached to
       i_blocks_lo.
       (块计数的高 16 位。请参见 i_blocks_lo 处的附注。)
   * - 0x2
     - __le16
     - l_i_file_acl_high
     - Upper 16-bits of the extended attribute block (historically, the file
       ACL location). See the Extended Attributes section below.
       (扩展属性块的高 16 位(历史上是文件 ACL 所在位置)。参见下文的 Extended Attributes 一节。)
   * - 0x4
     - __le16
     - l_i_uid_high
     - Upper 16-bits of the Owner UID.
       (所有者 UID 的高 16 位。)
   * - 0x6
     - __le16
     - l_i_gid_high
     - Upper 16-bits of the GID.
       (GID 的高 16 位。)
   * - 0x8
     - __le16
     - l_i_checksum_lo
     - Lower 16-bits of the inode checksum.
       (inode 校验和的低 16 位。)
   * - 0xA
     - __le16
     - l_i_reserved
     - Unused.
       (未使用。)

Hurd:
(Hurd:)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Size
       (大小)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le16
     - h_i_reserved1
     - ??
   * - 0x2
     - __u16
     - h_i_mode_high
     - Upper 16-bits of the file mode.
       (文件模式的高 16 位。)
   * - 0x4
     - __le16
     - h_i_uid_high
     - Upper 16-bits of the Owner UID.
       (所有者 UID 的高 16 位。)
   * - 0x6
     - __le16
     - h_i_gid_high
     - Upper 16-bits of the GID.
       (GID 的高 16 位。)
   * - 0x8
     - __u32
     - h_i_author
     - Author code?
       (作者代码？)

Masix:
(Masix:)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Size
       (大小)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le16
     - h_i_reserved1
     - ??
   * - 0x2
     - __u16
     - m_i_file_acl_high
     - Upper 16-bits of the extended attribute block (historically, the file
       ACL location).
       (扩展属性块的高 16 位(历史上是文件 ACL 所在位置)。)
   * - 0x4
     - __u32
     - m_i_reserved2[2]
     - ??

Inode Size
(inode 大小)
~~~~~~~~~~

In ext2 and ext3, the inode structure size was fixed at 128 bytes
(``EXT2_GOOD_OLD_INODE_SIZE``) and each inode had a disk record size of
128 bytes. Starting with ext4, it is possible to allocate a larger
on-disk inode at format time for all inodes in the filesystem to provide
space beyond the end of the original ext2 inode. The on-disk inode
record size is recorded in the superblock as ``s_inode_size``. The
number of bytes actually used by struct ext4_inode beyond the original
128-byte ext2 inode is recorded in the ``i_extra_isize`` field for each
inode, which allows struct ext4_inode to grow for a new kernel without
having to upgrade all of the on-disk inodes. Access to fields beyond
EXT2_GOOD_OLD_INODE_SIZE should be verified to be within
``i_extra_isize``. By default, ext4 inode records are 256 bytes, and (as
of August 2019) the inode structure is 160 bytes
(``i_extra_isize = 32``). The extra space between the end of the inode
structure and the end of the inode record can be used to store extended
attributes. Each inode record can be as large as the filesystem block
size, though this is not terribly efficient.
(在 ext2 和 ext3 中，inode 结构大小固定为 128 字节(``EXT2_GOOD_OLD_INODE_SIZE``)，并且每个 inode 在磁盘上的记录大小为 128 字节。从 ext4 开始，可以在格式化时为文件系统中的所有 inode 分配一个更大的在盘 inode，以在原始 ext2 inode 末尾之外提供空间。在盘 inode 记录大小记录在超级块中的 ``s_inode_size`` 里。struct ext4_inode 在原始 128 字节 ext2 inode 之外实际使用的字节数记录在每个 inode 的 ``i_extra_isize`` 字段中，这使得 struct ext4_inode 可以随新内核增长而无需升级所有在盘 inode。对超出 EXT2_GOOD_OLD_INODE_SIZE 的字段的访问，应验证其是否位于 ``i_extra_isize`` 范围内。默认情况下，ext4 inode 记录为 256 字节，并且(截至 2019 年 8 月)inode 结构为 160 字节(``i_extra_isize = 32``)。inode 结构末尾与 inode 记录末尾之间的额外空间可用于存储扩展属性。每个 inode 记录最大可以与文件系统块大小一样大，不过这并不十分高效。)

Finding an Inode
(查找一个 inode)
~~~~~~~~~~~~~~~~

Each block group contains ``sb->s_inodes_per_group`` inodes. Because
inode 0 is defined not to exist, this formula can be used to find the
block group that an inode lives in:
``bg = (inode_num - 1) / sb->s_inodes_per_group``. The particular inode
can be found within the block group's inode table at
``index = (inode_num - 1) % sb->s_inodes_per_group``. To get the byte
address within the inode table, use
``offset = index * sb->s_inode_size``.
(每个块组包含 ``sb->s_inodes_per_group`` 个 inode。由于 inode 0 被定义为不存在，可以使用以下公式找到某个 inode 所在的块组：``bg = (inode_num - 1) / sb->s_inodes_per_group``。特定 inode 可以在块组的 inode 表中通过 ``index = (inode_num - 1) % sb->s_inodes_per_group`` 找到。要获得 inode 表内的字节地址，使用 ``offset = index * sb->s_inode_size``。)

Inode Timestamps
(inode 时间戳)
~~~~~~~~~~~~~~~~

Four timestamps are recorded in the lower 128 bytes of the inode
structure -- inode change time (ctime), access time (atime), data
modification time (mtime), and deletion time (dtime). The four fields
are 32-bit signed integers that represent seconds since the Unix epoch
(1970-01-01 00:00:00 GMT), which means that the fields will overflow in
January 2038. If the filesystem does not have orphan_file feature, inodes
that are not linked from any directory but are still open (orphan inodes) have
the dtime field overloaded for use with the orphan list. The superblock field
``s_last_orphan`` points to the first inode in the orphan list; dtime is then
the number of the next orphaned inode, or zero if there are no more orphans.
(inode 结构的低 128 字节中记录了四个时间戳——inode 变更时间 (ctime)、访问时间 (atime)、数据修改时间 (mtime) 以及删除时间 (dtime)。这四个字段是 32 位有符号整数，表示自 Unix 纪元(1970-01-01 00:00:00 GMT)以来的秒数，这意味着这些字段将在 2038 年 1 月溢出。如果文件系统没有 orphan_file 特性，那些未从任何目录链接但仍处于打开状态的 inode(孤儿 inode)会将 dtime 字段挪用于孤儿列表。超级块字段 ``s_last_orphan`` 指向孤儿列表中的第一个 inode；此时 dtime 是下一个孤儿 inode 的编号，如果没有更多孤儿则为零。)

If the inode structure size ``sb->s_inode_size`` is larger than 128
bytes and the ``i_inode_extra`` field is large enough to encompass the
respective ``i_[cma]time_extra`` field, the ctime, atime, and mtime
inode fields are widened to 64 bits. Within this “extra” 32-bit field,
the lower two bits are used to extend the 32-bit seconds field to be 34
bit wide; the upper 30 bits are used to provide nanosecond timestamp
accuracy. Therefore, timestamps should not overflow until May 2446.
dtime was not widened. There is also a fifth timestamp to record inode
creation time (crtime); this field is 64-bits wide and decoded in the
same manner as 64-bit [cma]time. Neither crtime nor dtime are accessible
through the regular stat() interface, though debugfs will report them.
(如果 inode 结构大小 ``sb->s_inode_size`` 大于 128 字节，并且 ``i_inode_extra`` 字段足够大以涵盖相应的 ``i_[cma]time_extra`` 字段，则 ctime、atime 和 mtime inode 字段被扩展为 64 位。在这个“额外的” 32 位字段中，低两位用于将 32 位的秒字段扩展为 34 位宽；高 30 位用于提供纳秒级时间戳精度。因此，时间戳到 2446 年 5 月之前不应溢出。dtime 未被扩展。还有第五个时间戳用于记录 inode 创建时间 (crtime)；该字段为 64 位宽，其解码方式与 64 位 [cma]time 相同。虽然 debugfs 会报告 crtime 和 dtime，但它们都无法通过常规的 stat() 接口访问。)

We use the 32-bit signed time value plus (2^32 * (extra epoch bits)).
In other words:
(我们使用 32 位有符号时间值加上 (2^32 * (额外纪元位))。换句话说：)

.. list-table::
   :widths: 20 20 20 20 20
   :header-rows: 1

   * - Extra epoch bits
       (额外的纪元位)
     - MSB of 32-bit time
       (32 位时间的 MSB)
     - Adjustment for signed 32-bit to 64-bit tv_sec
       (从带符号 32 位调整到 64 位 tv_sec 的调整量)
     - Decoded 64-bit tv_sec
       (解码后的 64 位 tv_sec)
     - valid time range
       (有效时间范围)
   * - 0 0
     - 1
     - 0
     - ``-0x80000000 - -0x00000001``
     - 1901-12-13 to 1969-12-31
       (1901-12-13 至 1969-12-31)
   * - 0 0
     - 0
     - 0
     - ``0x000000000 - 0x07fffffff``
     - 1970-01-01 to 2038-01-19
       (1970-01-01 至 2038-01-19)
   * - 0 1
     - 1
     - 0x100000000
     - ``0x080000000 - 0x0ffffffff``
     - 2038-01-19 to 2106-02-07
       (2038-01-19 至 2106-02-07)
   * - 0 1
     - 0
     - 0x100000000
     - ``0x100000000 - 0x17fffffff``
     - 2106-02-07 to 2174-02-25
       (2106-02-07 至 2174-02-25)
   * - 1 0
     - 1
     - 0x200000000
     - ``0x180000000 - 0x1ffffffff``
     - 2174-02-25 to 2242-03-16
       (2174-02-25 至 2242-03-16)
   * - 1 0
     - 0
     - 0x200000000
     - ``0x200000000 - 0x27fffffff``
     - 2242-03-16 to 2310-04-04
       (2242-03-16 至 2310-04-04)
   * - 1 1
     - 1
     - 0x300000000
     - ``0x280000000 - 0x2ffffffff``
     - 2310-04-04 to 2378-04-22
       (2310-04-04 至 2378-04-22)
   * - 1 1
     - 0
     - 0x300000000
     - ``0x300000000 - 0x37fffffff``
     - 2378-04-22 to 2446-05-10
       (2378-04-22 至 2446-05-10)

This is a somewhat odd encoding since there are effectively seven times
as many positive values as negative values. There have also been
long-standing bugs decoding and encoding dates beyond 2038, which don't
seem to be fixed as of kernel 3.12 and e2fsprogs 1.42.8. 64-bit kernels
incorrectly use the extra epoch bits 1,1 for dates between 1901 and
1970. At some point the kernel will be fixed and e2fsck will fix this
situation, assuming that it is run before 2310.
(这是一种有些奇怪的编码，因为正值数量实际上是负值的七倍。在解码和编码 2038 年以后的日期方面还存在长期存在的 bug，截至内核 3.12 和 e2fsprogs 1.42.8 似乎仍未修复。64 位内核对 1901 年至 1970 年之间的日期错误地使用了额外纪元位 1,1。在将来的某个时点内核会被修复，e2fsck 也会修复这种情况，前提是它在 2310 年之前运行。)
