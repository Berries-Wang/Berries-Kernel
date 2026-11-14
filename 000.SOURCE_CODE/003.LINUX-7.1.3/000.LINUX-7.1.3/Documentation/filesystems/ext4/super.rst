.. SPDX-License-Identifier: GPL-2.0

Super Block
(超级块)
-----------

The superblock records various information about the enclosing
filesystem, such as block counts, inode counts, supported features,
maintenance information, and more.
(超级块记录了所在文件系统的各种信息，例如块计数、inode 计数、支持的特性、维护信息等等。)

If the sparse_super feature flag is set, redundant copies of the
superblock and group descriptors are kept only in the groups whose group
number is either 0 or a power of 3, 5, or 7. If the flag is not set,
redundant copies are kept in all groups.
(如果设置了 sparse_super 特性标志，超级块和组描述符的冗余副本仅保存在组号为 0 或 3、5、7 的幂的组中。如果未设置该标志，则在所有组中都会保存冗余副本。)

The superblock checksum is calculated against the superblock structure,
which includes the FS UUID.
(超级块校验和是针对超级块结构计算的，其中包括文件系统 UUID。)

The ext4 superblock is laid out as follows in
``struct ext4_super_block``:
(ext4 超级块在 ``struct ext4_super_block`` 中的布局如下：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移)
     - Size
       (大小)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le32
     - s_inodes_count
     - Total inode count.
       (inode 总数。)
   * - 0x4
     - __le32
     - s_blocks_count_lo
     - Total block count.
       (块总数。)
   * - 0x8
     - __le32
     - s_r_blocks_count_lo
     - This number of blocks can only be allocated by the super-user.
       (此数量的块只能由超级用户分配。)
   * - 0xC
     - __le32
     - s_free_blocks_count_lo
     - Free block count.
       (空闲块数。)
   * - 0x10
     - __le32
     - s_free_inodes_count
     - Free inode count.
       (空闲 inode 数。)
   * - 0x14
     - __le32
     - s_first_data_block
     - First data block. This must be at least 1 for 1k-block filesystems and
       is typically 0 for all other block sizes.
       (第一个数据块。对于 1k 块的文件系统该值必须至少为 1，对于所有其他块大小通常为 0。)
   * - 0x18
     - __le32
     - s_log_block_size
     - Block size is 2 ^ (10 + s_log_block_size).
       (块大小为 2 ^ (10 + s_log_block_size)。)
   * - 0x1C
     - __le32
     - s_log_cluster_size
     - Cluster size is 2 ^ (10 + s_log_cluster_size) blocks if bigalloc is
       enabled. Otherwise s_log_cluster_size must equal s_log_block_size.
       (如果启用了 bigalloc，簇大小为 2 ^ (10 + s_log_cluster_size) 个块。否则 s_log_cluster_size 必须等于 s_log_block_size。)
   * - 0x20
     - __le32
     - s_blocks_per_group
     - Blocks per group.
       (每组块数。)
   * - 0x24
     - __le32
     - s_clusters_per_group
     - Clusters per group, if bigalloc is enabled. Otherwise
       s_clusters_per_group must equal s_blocks_per_group.
       (每组簇数，如果启用了 bigalloc。否则 s_clusters_per_group 必须等于 s_blocks_per_group。)
   * - 0x28
     - __le32
     - s_inodes_per_group
     - Inodes per group.
       (每组 inode 数。)
   * - 0x2C
     - __le32
     - s_mtime
     - Mount time, in seconds since the epoch.
       (挂载时间，自纪元起的秒数。)
   * - 0x30
     - __le32
     - s_wtime
     - Write time, in seconds since the epoch.
       (写入时间，自纪元起的秒数。)
   * - 0x34
     - __le16
     - s_mnt_count
     - Number of mounts since the last fsck.
       (自上次 fsck 以来的挂载次数。)
   * - 0x36
     - __le16
     - s_max_mnt_count
     - Number of mounts beyond which a fsck is needed.
       (超过此挂载次数就需要进行 fsck。)
   * - 0x38
     - __le16
     - s_magic
     - Magic signature, 0xEF53
       (魔数签名，0xEF53)
   * - 0x3A
     - __le16
     - s_state
     - File system state. See super_state_ for more info.
       (文件系统状态。更多信息参见 super_state_。)
   * - 0x3C
     - __le16
     - s_errors
     - Behaviour when detecting errors. See super_errors_ for more info.
       (检测到错误时的行为。更多信息参见 super_errors_。)
   * - 0x3E
     - __le16
     - s_minor_rev_level
     - Minor revision level.
       (次修订级别。)
   * - 0x40
     - __le32
     - s_lastcheck
     - Time of last check, in seconds since the epoch.
       (上次检查时间，自纪元起的秒数。)
   * - 0x44
     - __le32
     - s_checkinterval
     - Maximum time between checks, in seconds.
       (两次检查之间的最大时间间隔，以秒为单位。)
   * - 0x48
     - __le32
     - s_creator_os
     - Creator OS. See the table super_creator_ for more info.
       (创建者的操作系统。更多信息参见 super_creator_ 表。)
   * - 0x4C
     - __le32
     - s_rev_level
     - Revision level. See the table super_revision_ for more info.
       (修订级别。更多信息参见 super_revision_ 表。)
   * - 0x50
     - __le16
     - s_def_resuid
     - Default uid for reserved blocks.
       (保留块的默认 uid。)
   * - 0x52
     - __le16
     - s_def_resgid
     - Default gid for reserved blocks.
       (保留块的默认 gid。)
   * -
     -
     -
     - These fields are for EXT4_DYNAMIC_REV superblocks only.
       (这些字段仅适用于 EXT4_DYNAMIC_REV 超级块。)
       
       Note: the difference between the compatible feature set and the
       incompatible feature set is that if there is a bit set in the
       incompatible feature set that the kernel doesn't know about, it should
       refuse to mount the filesystem.
       (注意：兼容特性集与不兼容特性集的区别在于，如果不兼容特性集中有某个位被设置而内核不认识它，内核就应当拒绝挂载该文件系统。)
       
       e2fsck's requirements are more strict; if it doesn't know
       about a feature in either the compatible or incompatible feature set, it
       must abort and not try to meddle with things it doesn't understand...
       (e2fsck 的要求更为严格；如果它不了解兼容或不兼容特性集中的某个特性，它必须中止，而不要试图去干预它不理解的东西……)
   * - 0x54
     - __le32
     - s_first_ino
     - First non-reserved inode.
       (第一个非保留 inode。)
   * - 0x58
     - __le16
     - s_inode_size
     - Size of inode structure, in bytes.
       (inode 结构的大小，以字节为单位。)
   * - 0x5A
     - __le16
     - s_block_group_nr
     - Block group # of this superblock.
       (此超级块所在的块组编号。)
   * - 0x5C
     - __le32
     - s_feature_compat
     - Compatible feature set flags. Kernel can still read/write this fs even
       if it doesn't understand a flag; fsck should not do that. See the
       super_compat_ table for more info.
       (兼容特性集标志。即使内核不理解某个标志，也仍然可以读写此文件系统；fsck 不应这样做。更多信息参见 super_compat_ 表。)
   * - 0x60
     - __le32
     - s_feature_incompat
     - Incompatible feature set. If the kernel or fsck doesn't understand one
       of these bits, it should stop. See the super_incompat_ table for more
       info.
       (不兼容特性集。如果内核或 fsck 不理解其中某个位，就应当停止。更多信息参见 super_incompat_ 表。)
   * - 0x64
     - __le32
     - s_feature_ro_compat
     - Readonly-compatible feature set. If the kernel doesn't understand one of
       these bits, it can still mount read-only. See the super_rocompat_ table
       for more info.
       (只读兼容特性集。如果内核不理解其中某个位，仍可以以只读方式挂载。更多信息参见 super_rocompat_ 表。)
   * - 0x68
     - __u8
     - s_uuid[16]
     - 128-bit UUID for volume.
       (卷的 128 位 UUID。)
   * - 0x78
     - char
     - s_volume_name[16]
     - Volume label.
       (卷标。)
   * - 0x88
     - char
     - s_last_mounted[64]
     - Directory where filesystem was last mounted.
       (文件系统最后一次挂载到的目录。)
   * - 0xC8
     - __le32
     - s_algorithm_usage_bitmap
     - For compression (Not used in e2fsprogs/Linux)
       (用于压缩(e2fsprogs/Linux 中未使用))
   * -
     -
     -
     - Performance hints.  Directory preallocation should only happen if the
       EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is on.
       (性能提示。只有当 EXT4_FEATURE_COMPAT_DIR_PREALLOC 标志打开时才应进行目录预分配。)
   * - 0xCC
     - __u8
     - s_prealloc_blocks
     - #. of blocks to try to preallocate for ... files? (Not used in
       e2fsprogs/Linux)
       (尝试为……文件预分配的块数？(e2fsprogs/Linux 中未使用))
   * - 0xCD
     - __u8
     - s_prealloc_dir_blocks
     - #. of blocks to preallocate for directories. (Not used in
       e2fsprogs/Linux)
       (为目录预分配的块数。(e2fsprogs/Linux 中未使用))
   * - 0xCE
     - __le16
     - s_reserved_gdt_blocks
     - Number of reserved GDT entries for future filesystem expansion.
       (为将来文件系统扩展而保留的 GDT 项数。)
   * -
     -
     -
     - Journalling support is valid only if EXT4_FEATURE_COMPAT_HAS_JOURNAL is
       set.
       (只有设置了 EXT4_FEATURE_COMPAT_HAS_JOURNAL 时，日志支持才有效。)
   * - 0xD0
     - __u8
     - s_journal_uuid[16]
     - UUID of journal superblock
       (日志超级块的 UUID)
   * - 0xE0
     - __le32
     - s_journal_inum
     - inode number of journal file.
       (日志文件的 inode 号。)
   * - 0xE4
     - __le32
     - s_journal_dev
     - Device number of journal file, if the external journal feature flag is
       set.
       (日志文件的设备号，如果设置了外部日志特性标志。)
   * - 0xE8
     - __le32
     - s_last_orphan
     - Start of list of orphaned inodes to delete.
       (待删除孤儿 inode 链表的起始。)
   * - 0xEC
     - __le32
     - s_hash_seed[4]
     - HTREE hash seed.
       (HTREE 哈希种子。)
   * - 0xFC
     - __u8
     - s_def_hash_version
     - Default hash algorithm to use for directory hashes. See super_def_hash_
       for more info.
       (用于目录哈希的默认哈希算法。更多信息参见 super_def_hash_。)
   * - 0xFD
     - __u8
     - s_jnl_backup_type
     - If this value is 0 or EXT3_JNL_BACKUP_BLOCKS (1), then the
       ``s_jnl_blocks`` field contains a duplicate copy of the inode's
       ``i_block[]`` array and ``i_size``.
       (如果此值为 0 或 EXT3_JNL_BACKUP_BLOCKS (1)，则 ``s_jnl_blocks`` 字段包含 inode 的 ``i_block[]`` 数组和 ``i_size`` 的副本。)
   * - 0xFE
     - __le16
     - s_desc_size
     - Size of group descriptors, in bytes, if the 64bit incompat feature flag
       is set.
       (组描述符的大小，以字节为单位，如果设置了 64bit 不兼容特性标志。)
   * - 0x100
     - __le32
     - s_default_mount_opts
     - Default mount options. See the super_mountopts_ table for more info.
       (默认挂载选项。更多信息参见 super_mountopts_ 表。)
   * - 0x104
     - __le32
     - s_first_meta_bg
     - First metablock block group, if the meta_bg feature is enabled.
       (第一个元块块组，如果启用了 meta_bg 特性。)
   * - 0x108
     - __le32
     - s_mkfs_time
     - When the filesystem was created, in seconds since the epoch.
       (文件系统创建的时间，自纪元起的秒数。)
   * - 0x10C
     - __le32
     - s_jnl_blocks[17]
     - Backup copy of the journal inode's ``i_block[]`` array in the first 15
       elements and i_size_high and i_size in the 16th and 17th elements,
       respectively.
       (日志 inode 的 ``i_block[]`` 数组的备份副本位于前 15 个元素中，i_size_high 和 i_size 分别位于第 16 和第 17 个元素中。)
   * -
     -
     -
     - 64bit support is valid only if EXT4_FEATURE_COMPAT_64BIT is set.
       (只有设置了 EXT4_FEATURE_COMPAT_64BIT 时，64bit 支持才有效。)
   * - 0x150
     - __le32
     - s_blocks_count_hi
     - High 32-bits of the block count.
       (块数的高 32 位。)
   * - 0x154
     - __le32
     - s_r_blocks_count_hi
     - High 32-bits of the reserved block count.
       (保留块数的高 32 位。)
   * - 0x158
     - __le32
     - s_free_blocks_count_hi
     - High 32-bits of the free block count.
       (空闲块数的高 32 位。)
   * - 0x15C
     - __le16
     - s_min_extra_isize
     - All inodes have at least # bytes.
       (所有 inode 至少有 # 字节。)
   * - 0x15E
     - __le16
     - s_want_extra_isize
     - New inodes should reserve # bytes.
       (新 inode 应保留 # 字节。)
   * - 0x160
     - __le32
     - s_flags
     - Miscellaneous flags. See the super_flags_ table for more info.
       (杂项标志。更多信息参见 super_flags_ 表。)
   * - 0x164
     - __le16
     - s_raid_stride
     - RAID stride. This is the number of logical blocks read from or written
       to the disk before moving to the next disk. This affects the placement
       of filesystem metadata, which will hopefully make RAID storage faster.
       (RAID stride。这是在移动到下一块磁盘之前从磁盘读取或写入磁盘的逻辑块数。这会影响文件系统元数据的放置，有望使 RAID 存储更快。)
   * - 0x166
     - __le16
     - s_mmp_interval
     - #. seconds to wait in multi-mount prevention (MMP) checking. In theory,
       MMP is a mechanism to record in the superblock which host and device
       have mounted the filesystem, in order to prevent multiple mounts. This
       feature does not seem to be implemented...
       (多挂载预防(MMP)检查中等待的秒数。理论上，MMP 是一种在超级块中记录哪个主机和设备已挂载文件系统的机制，以防止多次挂载。这个特性似乎并没有实现……)
   * - 0x168
     - __le64
     - s_mmp_block
     - Block # for multi-mount protection data.
       (多挂载保护数据的块号。)
   * - 0x170
     - __le32
     - s_raid_stripe_width
     - RAID stripe width. This is the number of logical blocks read from or
       written to the disk before coming back to the current disk. This is used
       by the block allocator to try to reduce the number of read-modify-write
       operations in a RAID5/6.
       (RAID 条带宽度。这是在回到当前磁盘之前从磁盘读取或写入磁盘的逻辑块数。块分配器使用它来尝试减少 RAID5/6 中读-改-写操作的次数。)
   * - 0x174
     - __u8
     - s_log_groups_per_flex
     - Size of a flexible block group is 2 ^ ``s_log_groups_per_flex``.
       (灵活块组的大小为 2 ^ ``s_log_groups_per_flex``。)
   * - 0x175
     - __u8
     - s_checksum_type
     - Metadata checksum algorithm type. The only valid value is 1 (crc32c).
       (元数据校验和算法类型。唯一有效的值是 1 (crc32c)。)
   * - 0x176
     - \_\_u8
     - s\_encryption\_level
     - Versioning level for encryption.
       (加密的版本级别。)
   * - 0x177
     - \_\_u8
     - s\_reserved\_pad
     - Padding to next 32bits.
       (填充到下一个 32 位。)
   * - 0x178
     - __le64
     - s_kbytes_written
     - Number of KiB written to this filesystem over its lifetime.
       (此文件系统生命周期内写入的 KiB 数。)
   * - 0x180
     - __le32
     - s_snapshot_inum
     - inode number of active snapshot. (Not used in e2fsprogs/Linux.)
       (活动快照的 inode 号。(e2fsprogs/Linux 中未使用。))
   * - 0x184
     - __le32
     - s_snapshot_id
     - Sequential ID of active snapshot. (Not used in e2fsprogs/Linux.)
       (活动快照的顺序 ID。(e2fsprogs/Linux 中未使用。))
   * - 0x188
     - __le64
     - s_snapshot_r_blocks_count
     - Number of blocks reserved for active snapshot's future use. (Not used in
       e2fsprogs/Linux.)
       (为活动快照将来使用而保留的块数。(e2fsprogs/Linux 中未使用。))
   * - 0x190
     - __le32
     - s_snapshot_list
     - inode number of the head of the on-disk snapshot list. (Not used in
       e2fsprogs/Linux.)
       (磁盘上快照链表头的 inode 号。(e2fsprogs/Linux 中未使用。))
   * - 0x194
     - __le32
     - s_error_count
     - Number of errors seen.
       (看到的错误数量。)
   * - 0x198
     - __le32
     - s_first_error_time
     - First time an error happened, in seconds since the epoch.
       (第一次发生错误的时间，自纪元起的秒数。)
   * - 0x19C
     - __le32
     - s_first_error_ino
     - inode involved in first error.
       (第一次错误涉及的 inode。)
   * - 0x1A0
     - __le64
     - s_first_error_block
     - Number of block involved of first error.
       (第一次错误涉及的块号。)
   * - 0x1A8
     - __u8
     - s_first_error_func[32]
     - Name of function where the error happened.
       (发生错误的函数名。)
   * - 0x1C8
     - __le32
     - s_first_error_line
     - Line number where error happened.
       (发生错误的行号。)
   * - 0x1CC
     - __le32
     - s_last_error_time
     - Time of most recent error, in seconds since the epoch.
       (最近一次错误的时间，自纪元起的秒数。)
   * - 0x1D0
     - __le32
     - s_last_error_ino
     - inode involved in most recent error.
       (最近一次错误涉及的 inode。)
   * - 0x1D4
     - __le32
     - s_last_error_line
     - Line number where most recent error happened.
       (最近一次错误发生的行号。)
   * - 0x1D8
     - __le64
     - s_last_error_block
     - Number of block involved in most recent error.
       (最近一次错误涉及的块号。)
   * - 0x1E0
     - __u8
     - s_last_error_func[32]
     - Name of function where the most recent error happened.
       (发生最近一次错误的函数名。)
   * - 0x200
     - __u8
     - s_mount_opts[64]
     - ASCIIZ string of mount options.
       (挂载选项的 ASCIIZ 字符串。)
   * - 0x240
     - __le32
     - s_usr_quota_inum
     - Inode number of user `quota <quota>`__ file.
       (用户 `quota <quota>`__ 文件的 inode 号。)
   * - 0x244
     - __le32
     - s_grp_quota_inum
     - Inode number of group `quota <quota>`__ file.
       (组 `quota <quota>`__ 文件的 inode 号。)
   * - 0x248
     - __le32
     - s_overhead_blocks
     - Overhead blocks/clusters in fs. (Huh? This field is always zero, which
       means that the kernel calculates it dynamically.)
       (文件系统中的开销块/簇。(咦？此字段始终为零，这意味着内核会动态计算它。))
   * - 0x24C
     - __le32
     - s_backup_bgs[2]
     - Block groups containing superblock backups (if sparse_super2)
       (包含超级块备份的块组(如果是 sparse_super2))
   * - 0x254
     - __u8
     - s_encrypt_algos[4]
     - Encryption algorithms in use. There can be up to four algorithms in use
       at any time; valid algorithm codes are given in the super_encrypt_ table
       below.
       (正在使用的加密算法。任何时候最多可以有四个算法在使用；有效的算法代码见下面的 super_encrypt_ 表。)
   * - 0x258
     - __u8
     - s_encrypt_pw_salt[16]
     - Salt for the string2key algorithm for encryption.
       (加密的 string2key 算法所用的盐值。)
   * - 0x268
     - __le32
     - s_lpf_ino
     - Inode number of lost+found
       (lost+found 的 inode 号)
   * - 0x26C
     - __le32
     - s_prj_quota_inum
     - Inode that tracks project quotas.
       (跟踪项目配额的 inode。)
   * - 0x270
     - __le32
     - s_checksum_seed
     - Checksum seed used for metadata_csum calculations. This value is
       crc32c(~0, $orig_fs_uuid).
       (用于 metadata_csum 计算的校验和种子。此值为 crc32c(~0, $orig_fs_uuid)。)
   * - 0x274
     - __u8
     - s_wtime_hi
     - Upper 8 bits of the s_wtime field.
       (s_wtime 字段的高 8 位。)
   * - 0x275
     - __u8
     - s_mtime_hi
     - Upper 8 bits of the s_mtime field.
       (s_mtime 字段的高 8 位。)
   * - 0x276
     - __u8
     - s_mkfs_time_hi
     - Upper 8 bits of the s_mkfs_time field.
       (s_mkfs_time 字段的高 8 位。)
   * - 0x277
     - __u8
     - s_lastcheck_hi
     - Upper 8 bits of the s_lastcheck field.
       (s_lastcheck 字段的高 8 位。)
   * - 0x278
     - __u8
     - s_first_error_time_hi
     - Upper 8 bits of the s_first_error_time field.
       (s_first_error_time 字段的高 8 位。)
   * - 0x279
     - __u8
     - s_last_error_time_hi
     - Upper 8 bits of the s_last_error_time field.
       (s_last_error_time 字段的高 8 位。)
   * - 0x27A
     - \_\_u8
     - s\_first\_error\_errcode
     -
   * - 0x27B
     - \_\_u8
     - s\_last\_error\_errcode
     -
   * - 0x27C
     - __le16
     - s_encoding
     - Filename charset encoding.
       (文件名字符集编码。)
   * - 0x27E
     - __le16
     - s_encoding_flags
     - Filename charset encoding flags.
       (文件名字符集编码标志。)
   * - 0x280
     - __le32
     - s_orphan_file_inum
     - Orphan file inode number.
       (孤儿文件的 inode 号。)
   * - 0x284
     - __le32
     - s_reserved[94]
     - Padding to the end of the block.
       (填充到块的末尾。)
   * - 0x3FC
     - __le32
     - s_checksum
     - Superblock checksum.
       (超级块校验和。)

.. _super_state:

The superblock state is some combination of the following:
(超级块状态是以下值的某种组合：)

.. list-table::
   :widths: 8 72
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x0001
     - Cleanly umounted
       (干净地卸载)
   * - 0x0002
     - Errors detected
       (检测到错误)
   * - 0x0004
     - Orphans being recovered
       (正在恢复孤儿)

.. _super_errors:

The superblock error policy is one of the following:
(超级块错误策略是以下之一：)

.. list-table::
   :widths: 8 72
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 1
     - Continue
       (继续)
   * - 2
     - Remount read-only
       (以只读方式重新挂载)
   * - 3
     - Panic
       (恐慌)

.. _super_creator:

The filesystem creator is one of the following:
(文件系统创建者是以下之一：)

.. list-table::
   :widths: 8 72
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0
     - Linux
   * - 1
     - Hurd
   * - 2
     - Masix
   * - 3
     - FreeBSD
   * - 4
     - Lites

.. _super_revision:

The superblock revision is one of the following:
(超级块修订版本是以下之一：)

.. list-table::
   :widths: 8 72
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0
     - Original format
       (原始格式)
   * - 1
     - v2 format w/ dynamic inode sizes
       (带动态 inode 大小的 v2 格式)

Note that ``EXT4_DYNAMIC_REV`` refers to a revision 1 or newer filesystem.
(注意，``EXT4_DYNAMIC_REV`` 指的是修订版本 1 或更新的文件系统。)

.. _super_compat:

The superblock compatible features field is a combination of any of the
following:
(超级块兼容特性字段是以下任意项的组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - Directory preallocation (COMPAT_DIR_PREALLOC).
       (目录预分配 (COMPAT_DIR_PREALLOC)。)
   * - 0x2
     - “imagic inodes”. Not clear from the code what this does
       (COMPAT_IMAGIC_INODES).
       (“imagic inodes”。从代码中不清楚它的作用 (COMPAT_IMAGIC_INODES)。)
   * - 0x4
     - Has a journal (COMPAT_HAS_JOURNAL).
       (有日志 (COMPAT_HAS_JOURNAL)。)
   * - 0x8
     - Supports extended attributes (COMPAT_EXT_ATTR).
       (支持扩展属性 (COMPAT_EXT_ATTR)。)
   * - 0x10
     - Has reserved GDT blocks for filesystem expansion
       (COMPAT_RESIZE_INODE). Requires RO_COMPAT_SPARSE_SUPER.
       (为文件系统扩展保留了 GDT 块 (COMPAT_RESIZE_INODE)。需要 RO_COMPAT_SPARSE_SUPER。)
   * - 0x20
     - Has directory indices (COMPAT_DIR_INDEX).
       (有目录索引 (COMPAT_DIR_INDEX)。)
   * - 0x40
     - “Lazy BG”. Not in Linux kernel, seems to have been for uninitialized
       block groups? (COMPAT_LAZY_BG)
       (“Lazy BG”。不在 Linux 内核中，似乎是用于未初始化的块组？(COMPAT_LAZY_BG))
   * - 0x80
     - “Exclude inode”. Not used. (COMPAT_EXCLUDE_INODE).
       (“Exclude inode”。未使用。(COMPAT_EXCLUDE_INODE)。)
   * - 0x100
     - “Exclude bitmap”. Seems to be used to indicate the presence of
       snapshot-related exclude bitmaps? Not defined in kernel or used in
       e2fsprogs (COMPAT_EXCLUDE_BITMAP).
       (“Exclude bitmap”。似乎用于指示与快照相关的排除位图的存在？未在内核中定义，也未在 e2fsprogs 中使用 (COMPAT_EXCLUDE_BITMAP)。)
   * - 0x200
     - Sparse Super Block, v2. If this flag is set, the SB field s_backup_bgs
       points to the two block groups that contain backup superblocks
       (COMPAT_SPARSE_SUPER2).
       (稀疏超级块，v2。如果设置了此标志，SB 字段 s_backup_bgs 指向包含备份超级块的两个块组 (COMPAT_SPARSE_SUPER2)。)
   * - 0x400
     - Fast commits supported. Although fast commits blocks are
       backward incompatible, fast commit blocks are not always
       present in the journal. If fast commit blocks are present in
       the journal, JBD2 incompat feature
       (JBD2_FEATURE_INCOMPAT_FAST_COMMIT) gets
       set (COMPAT_FAST_COMMIT).
       (支持快速提交。虽然快速提交块是向后不兼容的，但快速提交块并不总是存在于日志中。如果日志中存在快速提交块，则会设置 JBD2 不兼容特性 (JBD2_FEATURE_INCOMPAT_FAST_COMMIT) (COMPAT_FAST_COMMIT)。)
   * - 0x1000
     - Orphan file allocated. This is the special file for more efficient
       tracking of unlinked but still open inodes. When there may be any
       entries in the file, we additionally set proper rocompat feature
       (RO_COMPAT_ORPHAN_PRESENT).
       (已分配孤儿文件。这是用于更高效地跟踪已取消链接但仍处于打开状态的 inode 的特殊文件。当文件中可能存在任何条目时，我们还会设置相应的只读兼容特性 (RO_COMPAT_ORPHAN_PRESENT)。)

.. _super_incompat:

The superblock incompatible features field is a combination of any of the
following:
(超级块不兼容特性字段是以下任意项的组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - Compression (INCOMPAT_COMPRESSION).
       (压缩 (INCOMPAT_COMPRESSION)。)
   * - 0x2
     - Directory entries record the file type. See ext4_dir_entry_2 below
       (INCOMPAT_FILETYPE).
       (目录项记录文件类型。参见下面的 ext4_dir_entry_2 (INCOMPAT_FILETYPE)。)
   * - 0x4
     - Filesystem needs recovery (INCOMPAT_RECOVER).
       (文件系统需要恢复 (INCOMPAT_RECOVER)。)
   * - 0x8
     - Filesystem has a separate journal device (INCOMPAT_JOURNAL_DEV).
       (文件系统有单独的日志设备 (INCOMPAT_JOURNAL_DEV)。)
   * - 0x10
     - Meta block groups. See the earlier discussion of this feature
       (INCOMPAT_META_BG).
       (元块组。参见前文对此特性的讨论 (INCOMPAT_META_BG)。)
   * - 0x40
     - Files in this filesystem use extents (INCOMPAT_EXTENTS).
       (此文件系统中的文件使用 extent (INCOMPAT_EXTENTS)。)
   * - 0x80
     - Enable a filesystem size of 2^64 blocks (INCOMPAT_64BIT).
       (启用 2^64 个块的文件系统大小 (INCOMPAT_64BIT)。)
   * - 0x100
     - Multiple mount protection (INCOMPAT_MMP).
       (多挂载保护 (INCOMPAT_MMP)。)
   * - 0x200
     - Flexible block groups. See the earlier discussion of this feature
       (INCOMPAT_FLEX_BG).
       (灵活块组。参见前文对此特性的讨论 (INCOMPAT_FLEX_BG)。)
   * - 0x400
     - Inodes can be used to store large extended attribute values
       (INCOMPAT_EA_INODE).
       (inode 可用于存储大型扩展属性值 (INCOMPAT_EA_INODE)。)
   * - 0x1000
     - Data in directory entry (INCOMPAT_DIRDATA). (Not implemented?)
       (目录项中的数据 (INCOMPAT_DIRDATA)。(未实现？))
   * - 0x2000
     - Metadata checksum seed is stored in the superblock. This feature enables
       the administrator to change the UUID of a metadata_csum filesystem
       while the filesystem is mounted; without it, the checksum definition
       requires all metadata blocks to be rewritten (INCOMPAT_CSUM_SEED).
       (元数据校验和种子存储在超级块中。此特性使管理员能够在文件系统挂载时更改 metadata_csum 文件系统的 UUID；没有它，校验和的定义要求重写所有元数据块 (INCOMPAT_CSUM_SEED)。)
   * - 0x4000
     - Large directory >2GB or 3-level htree (INCOMPAT_LARGEDIR). Prior to
       this feature, directories could not be larger than 4GiB and could not
       have an htree more than 2 levels deep. If this feature is enabled,
       directories can be larger than 4GiB and have a maximum htree depth of 3.
       (大于 2GB 的大目录或 3 级 htree (INCOMPAT_LARGEDIR)。在此特性之前，目录不能大于 4GiB，htree 的深度不能超过 2 级。如果启用了此特性，目录可以大于 4GiB，htree 的最大深度为 3。)
   * - 0x8000
     - Data in inode (INCOMPAT_INLINE_DATA).
       (inode 中的数据 (INCOMPAT_INLINE_DATA)。)
   * - 0x10000
     - Encrypted inodes can be present. (INCOMPAT_ENCRYPT).
       (可能存在加密的 inode。(INCOMPAT_ENCRYPT)。)
   * - 0x20000
     - Directories can be marked case-insensitive. (INCOMPAT_CASEFOLD).
       (目录可以标记为大小写不敏感。(INCOMPAT_CASEFOLD)。)

.. _super_rocompat:

The superblock read-only compatible features field is a combination of any of
the following:
(超级块只读兼容特性字段是以下任意项的组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - Sparse superblocks. See the earlier discussion of this feature
       (RO_COMPAT_SPARSE_SUPER).
       (稀疏超级块。参见前文对此特性的讨论 (RO_COMPAT_SPARSE_SUPER)。)
   * - 0x2
     - This filesystem has been used to store a file greater than 2GiB
       (RO_COMPAT_LARGE_FILE).
       (此文件系统曾被用来存储大于 2GiB 的文件 (RO_COMPAT_LARGE_FILE)。)
   * - 0x4
     - Not used in kernel or e2fsprogs (RO_COMPAT_BTREE_DIR).
       (在内核或 e2fsprogs 中未使用 (RO_COMPAT_BTREE_DIR)。)
   * - 0x8
     - This filesystem has files whose sizes are represented in units of
       logical blocks, not 512-byte sectors. This implies a very large file
       indeed! (RO_COMPAT_HUGE_FILE)
       (此文件系统的文件大小以逻辑块为单位表示，而不是 512 字节扇区。这确实意味着非常大的文件！(RO_COMPAT_HUGE_FILE))
   * - 0x10
     - Group descriptors have checksums. In addition to detecting corruption,
       this is useful for lazy formatting with uninitialized groups
       (RO_COMPAT_GDT_CSUM).
       (组描述符带有校验和。除了检测损坏之外，这对带有未初始化组的延迟格式化也很有用 (RO_COMPAT_GDT_CSUM)。)
   * - 0x20
     - Indicates that the old ext3 32,000 subdirectory limit no longer applies
       (RO_COMPAT_DIR_NLINK). A directory's i_links_count will be set to 1
       if it is incremented past 64,999.
       (表示旧的 ext3 32,000 个子目录的限制不再适用 (RO_COMPAT_DIR_NLINK)。如果目录的 i_links_count 增加超过 64,999，它将被设置为 1。)
   * - 0x40
     - Indicates that large inodes exist on this filesystem
       (RO_COMPAT_EXTRA_ISIZE).
       (表示此文件系统上存在大 inode (RO_COMPAT_EXTRA_ISIZE)。)
   * - 0x80
     - This filesystem has a snapshot (RO_COMPAT_HAS_SNAPSHOT).
       (此文件系统有一个快照 (RO_COMPAT_HAS_SNAPSHOT)。)
   * - 0x100
     - `Quota <Quota>`__ (RO_COMPAT_QUOTA).
       (`Quota <Quota>`__ (配额) (RO_COMPAT_QUOTA)。)
   * - 0x200
     - This filesystem supports “bigalloc”, which means that file extents are
       tracked in units of clusters (of blocks) instead of blocks
       (RO_COMPAT_BIGALLOC).
       (此文件系统支持“bigalloc”，这意味着文件 extent 以簇(块的簇)为单位而不是以块为单位进行跟踪 (RO_COMPAT_BIGALLOC)。)
   * - 0x400
     - This filesystem supports metadata checksumming.
       (RO_COMPAT_METADATA_CSUM; implies RO_COMPAT_GDT_CSUM, though
       GDT_CSUM must not be set)
       (此文件系统支持元数据校验和。(RO_COMPAT_METADATA_CSUM；隐含 RO_COMPAT_GDT_CSUM，但不得设置 GDT_CSUM))
   * - 0x800
     - Filesystem supports replicas. This feature is neither in the kernel nor
       e2fsprogs. (RO_COMPAT_REPLICA)
       (文件系统支持副本。此特性既不在内核中也不在 e2fsprogs 中。(RO_COMPAT_REPLICA))
   * - 0x1000
     - Read-only filesystem image; the kernel will not mount this image
       read-write and most tools will refuse to write to the image.
       (RO_COMPAT_READONLY)
       (只读文件系统镜像；内核不会以读写方式挂载此镜像，大多数工具将拒绝写入该镜像。(RO_COMPAT_READONLY))
   * - 0x2000
     - Filesystem tracks project quotas. (RO_COMPAT_PROJECT)
       (文件系统跟踪项目配额。(RO_COMPAT_PROJECT))
   * - 0x8000
     - Verity inodes may be present on the filesystem. (RO_COMPAT_VERITY)
       (文件系统上可能存在 verity inode。(RO_COMPAT_VERITY))
   * - 0x10000
     - Indicates orphan file may have valid orphan entries and thus we need
       to clean them up when mounting the filesystem
       (RO_COMPAT_ORPHAN_PRESENT).
       (表示孤儿文件可能含有有效的孤儿条目，因此我们需要在挂载文件系统时清理它们 (RO_COMPAT_ORPHAN_PRESENT)。)

.. _super_def_hash:

The ``s_def_hash_version`` field is one of the following:
(``s_def_hash_version`` 字段是以下之一：)

.. list-table::
   :widths: 8 72
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x0
     - Legacy.
       (传统。)
   * - 0x1
     - Half MD4.
   * - 0x2
     - Tea.
   * - 0x3
     - Legacy, unsigned.
       (传统，无符号。)
   * - 0x4
     - Half MD4, unsigned.
       (Half MD4，无符号。)
   * - 0x5
     - Tea, unsigned.
       (Tea，无符号。)

.. _super_mountopts:

The ``s_default_mount_opts`` field is any combination of the following:
(``s_default_mount_opts`` 字段是以下选项的任意组合：)

.. list-table::
   :widths: 8 72
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x0001
     - Print debugging info upon (re)mount. (EXT4_DEFM_DEBUG)
       ((重新)挂载时打印调试信息。(EXT4_DEFM_DEBUG))
   * - 0x0002
     - New files take the gid of the containing directory (instead of the fsgid
       of the current process). (EXT4_DEFM_BSDGROUPS)
       (新文件采用所在目录的 gid(而不是当前进程的 fsgid)。(EXT4_DEFM_BSDGROUPS))
   * - 0x0004
     - Support userspace-provided extended attributes. (EXT4_DEFM_XATTR_USER)
       (支持用户空间提供的扩展属性。(EXT4_DEFM_XATTR_USER))
   * - 0x0008
     - Support POSIX access control lists (ACLs). (EXT4_DEFM_ACL)
       (支持 POSIX 访问控制列表(ACL)。(EXT4_DEFM_ACL))
   * - 0x0010
     - Do not support 32-bit UIDs. (EXT4_DEFM_UID16)
       (不支持 32 位 UID。(EXT4_DEFM_UID16))
   * - 0x0020
     - All data and metadata are committed to the journal.
       (EXT4_DEFM_JMODE_DATA)
       (所有数据和元数据都提交到日志。(EXT4_DEFM_JMODE_DATA))
   * - 0x0040
     - All data are flushed to the disk before metadata are committed to the
       journal. (EXT4_DEFM_JMODE_ORDERED)
       (在元数据提交到日志之前，所有数据都先刷写到磁盘。(EXT4_DEFM_JMODE_ORDERED))
   * - 0x0060
     - Data ordering is not preserved; data may be written after the metadata
       has been written. (EXT4_DEFM_JMODE_WBACK)
       (不保持数据顺序；数据可能在元数据写入之后再写入。(EXT4_DEFM_JMODE_WBACK))
   * - 0x0100
     - Disable write flushes. (EXT4_DEFM_NOBARRIER)
       (禁用写入刷新。(EXT4_DEFM_NOBARRIER))
   * - 0x0200
     - Track which blocks in a filesystem are metadata and therefore should not
       be used as data blocks. This option will be enabled by default on 3.18,
       hopefully. (EXT4_DEFM_BLOCK_VALIDITY)
       (跟踪文件系统中哪些块是元数据，因而不应用作数据块。希望此选项在 3.18 中默认启用。(EXT4_DEFM_BLOCK_VALIDITY))
   * - 0x0400
     - Enable DISCARD support, where the storage device is told about blocks
       becoming unused. (EXT4_DEFM_DISCARD)
       (启用 DISCARD 支持，即告知存储设备哪些块变为未使用。(EXT4_DEFM_DISCARD))
   * - 0x0800
     - Disable delayed allocation. (EXT4_DEFM_NODELALLOC)
       (禁用延迟分配。(EXT4_DEFM_NODELALLOC))

.. _super_flags:

The ``s_flags`` field is any combination of the following:
(``s_flags`` 字段是以下标志的任意组合：)

.. list-table::
   :widths: 8 72
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x0001
     - Signed directory hash in use.
       (使用带符号的目录哈希。)
   * - 0x0002
     - Unsigned directory hash in use.
       (使用无符号的目录哈希。)
   * - 0x0004
     - To test development code.
       (用于测试开发中的代码。)

.. _super_encrypt:

The ``s_encrypt_algos`` list can contain any of the following:
(``s_encrypt_algos`` 列表可以包含以下任意一项：)

.. list-table::
   :widths: 8 72
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0
     - Invalid algorithm (ENCRYPTION_MODE_INVALID).
       (无效算法 (ENCRYPTION_MODE_INVALID)。)
   * - 1
     - 256-bit AES in XTS mode (ENCRYPTION_MODE_AES_256_XTS).
       (XTS 模式的 256 位 AES (ENCRYPTION_MODE_AES_256_XTS)。)
   * - 2
     - 256-bit AES in GCM mode (ENCRYPTION_MODE_AES_256_GCM).
       (GCM 模式的 256 位 AES (ENCRYPTION_MODE_AES_256_GCM)。)
   * - 3
     - 256-bit AES in CBC mode (ENCRYPTION_MODE_AES_256_CBC).
       (CBC 模式的 256 位 AES (ENCRYPTION_MODE_AES_256_CBC)。)

Total size of the superblock is 1024 bytes.
(超级块的总大小为 1024 字节。)
