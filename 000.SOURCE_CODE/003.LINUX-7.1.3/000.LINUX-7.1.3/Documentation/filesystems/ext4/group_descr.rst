.. SPDX-License-Identifier: GPL-2.0

Block Group Descriptors
(块组描述符)
-----------------------

Each block group on the filesystem has one of these descriptors
associated with it. As noted in the Layout section above, the group
descriptors (if present) are the second item in the block group. The
standard configuration is for each block group to contain a full copy of
the block group descriptor table unless the sparse_super feature flag
is set.
(文件系统上的每个块组都有一个这样的描述符与之关联。如上文 Layout 一节所述，块组描述符(如果存在)是块组中的第二项。标准配置是每个块组都包含块组描述符表的一份完整副本，除非设置了 sparse_super 特性标志。)

Notice how the group descriptor records the location of both bitmaps and
the inode table (i.e. they can float). This means that within a block
group, the only data structures with fixed locations are the superblock
and the group descriptor table. The flex_bg mechanism uses this
property to group several block groups into a flex group and lay out all
of the groups' bitmaps and inode tables into one long run in the first
group of the flex group.
(注意块组描述符是如何记录位图和 inode 表两者位置的(即它们可以浮动)。这意味着在块组内部，只有超级块和块组描述符表是具有固定位置的数据结构。flex_bg 机制利用这一特性，把若干块组组合成一个 flex 组，并将所有这些组的位图和 inode 表在 flex 组的第一个组中排成一段连续的长区域。)

If the meta_bg feature flag is set, then several block groups are
grouped together into a meta group. Note that in the meta_bg case,
however, the first and last two block groups within the larger meta
group contain only group descriptors for the groups inside the meta
group.
(如果设置了 meta_bg 特性标志，那么若干块组会被组合成一个 meta 组。不过要注意，在 meta_bg 的情况下，较大 meta 组中的第一个块组和最后两个块组只包含 meta 组内部各组的块组描述符。)

flex_bg and meta_bg do not appear to be mutually exclusive features.
(flex_bg 和 meta_bg 看起来并不是互斥的特性。)

In ext2, ext3, and ext4 (when the 64bit feature is not enabled), the
block group descriptor was only 32 bytes long and therefore ends at
bg_checksum. On an ext4 filesystem with the 64bit feature enabled, the
block group descriptor expands to at least the 64 bytes described below;
the size is stored in the superblock.
(在 ext2、ext3 和 ext4(未启用 64bit 特性时)中，块组描述符只有 32 字节长，因此在 bg_checksum 处结束。在启用了 64bit 特性的 ext4 文件系统上，块组描述符会扩展到至少下文描述的 64 字节；其大小存储在超级块中。)

If gdt_csum is set and metadata_csum is not set, the block group
checksum is the crc16 of the FS UUID, the group number, and the group
descriptor structure. If metadata_csum is set, then the block group
checksum is the lower 16 bits of the checksum of the FS UUID, the group
number, and the group descriptor structure. Both block and inode bitmap
checksums are calculated against the FS UUID, the group number, and the
entire bitmap.
(如果设置了 gdt_csum 且未设置 metadata_csum，块组校验和就是 FS UUID、块组编号和块组描述符结构的 crc16。如果设置了 metadata_csum，则块组校验和是 FS UUID、块组编号和块组描述符结构的校验和的低 16 位。块位图和 inode 位图的校验和都是针对 FS UUID、块组编号和整个位图来计算的。)

The block group descriptor is laid out in ``struct ext4_group_desc``.
(块组描述符的布局由 ``struct ext4_group_desc`` 定义。)

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
     - bg_block_bitmap_lo
     - Lower 32-bits of location of block bitmap.
       (块位图位置的低 32 位。)
   * - 0x4
     - __le32
     - bg_inode_bitmap_lo
     - Lower 32-bits of location of inode bitmap.
       (inode 位图位置的低 32 位。)
   * - 0x8
     - __le32
     - bg_inode_table_lo
     - Lower 32-bits of location of inode table.
       (inode 表位置的低 32 位。)
   * - 0xC
     - __le16
     - bg_free_blocks_count_lo
     - Lower 16-bits of free block count.
       (空闲块计数的低 16 位。)
   * - 0xE
     - __le16
     - bg_free_inodes_count_lo
     - Lower 16-bits of free inode count.
       (空闲 inode 计数的低 16 位。)
   * - 0x10
     - __le16
     - bg_used_dirs_count_lo
     - Lower 16-bits of directory count.
       (目录计数的低 16 位。)
   * - 0x12
     - __le16
     - bg_flags
     - Block group flags. See the bgflags_ table below.
       (块组标志。参见下文的 bgflags_ 表。)
   * - 0x14
     - __le32
     - bg_exclude_bitmap_lo
     - Lower 32-bits of location of snapshot exclusion bitmap.
       (快照排除位图位置的低 32 位。)
   * - 0x18
     - __le16
     - bg_block_bitmap_csum_lo
     - Lower 16-bits of the block bitmap checksum.
       (块位图校验和的低 16 位。)
   * - 0x1A
     - __le16
     - bg_inode_bitmap_csum_lo
     - Lower 16-bits of the inode bitmap checksum.
       (inode 位图校验和的低 16 位。)
   * - 0x1C
     - __le16
     - bg_itable_unused_lo
     - Lower 16-bits of unused inode count. If set, we needn't scan past the
       ``(sb.s_inodes_per_group - gdt.bg_itable_unused)`` th entry in the
       inode table for this group.
       (未使用 inode 计数的低 16 位。如果设置了此值，我们就无需扫描该组 inode 表中第 ``(sb.s_inodes_per_group - gdt.bg_itable_unused)`` 个表项之后的内容。)
   * - 0x1E
     - __le16
     - bg_checksum
     - Group descriptor checksum; crc16(sb_uuid+group_num+bg_desc) if the
       RO_COMPAT_GDT_CSUM feature is set, or
       crc32c(sb_uuid+group_num+bg_desc) & 0xFFFF if the
       RO_COMPAT_METADATA_CSUM feature is set.  The bg_checksum
       field in bg_desc is skipped when calculating crc16 checksum,
       and set to zero if crc32c checksum is used.
       (块组描述符校验和；如果设置了 RO_COMPAT_GDT_CSUM 特性，则为 crc16(sb_uuid+group_num+bg_desc)；如果设置了 RO_COMPAT_METADATA_CSUM 特性，则为 crc32c(sb_uuid+group_num+bg_desc) & 0xFFFF。在计算 crc16 校验和时会跳过 bg_desc 中的 bg_checksum 字段；如果使用 crc32c 校验和，则将其设置为零。)
   * -
     -
     -
     - These fields only exist if the 64bit feature is enabled and s_desc_size
       > 32.
       (仅当启用了 64bit 特性且 s_desc_size > 32 时，这些字段才存在。)
   * - 0x20
     - __le32
     - bg_block_bitmap_hi
     - Upper 32-bits of location of block bitmap.
       (块位图位置的高 32 位。)
   * - 0x24
     - __le32
     - bg_inode_bitmap_hi
     - Upper 32-bits of location of inodes bitmap.
       (inode 位图位置的高 32 位。)
   * - 0x28
     - __le32
     - bg_inode_table_hi
     - Upper 32-bits of location of inodes table.
       (inode 表位置的高 32 位。)
   * - 0x2C
     - __le16
     - bg_free_blocks_count_hi
     - Upper 16-bits of free block count.
       (空闲块计数的高 16 位。)
   * - 0x2E
     - __le16
     - bg_free_inodes_count_hi
     - Upper 16-bits of free inode count.
       (空闲 inode 计数的高 16 位。)
   * - 0x30
     - __le16
     - bg_used_dirs_count_hi
     - Upper 16-bits of directory count.
       (目录计数的高 16 位。)
   * - 0x32
     - __le16
     - bg_itable_unused_hi
     - Upper 16-bits of unused inode count.
       (未使用 inode 计数的高 16 位。)
   * - 0x34
     - __le32
     - bg_exclude_bitmap_hi
     - Upper 32-bits of location of snapshot exclusion bitmap.
       (快照排除位图位置的高 32 位。)
   * - 0x38
     - __le16
     - bg_block_bitmap_csum_hi
     - Upper 16-bits of the block bitmap checksum.
       (块位图校验和的高 16 位。)
   * - 0x3A
     - __le16
     - bg_inode_bitmap_csum_hi
     - Upper 16-bits of the inode bitmap checksum.
       (inode 位图校验和的高 16 位。)
   * - 0x3C
     - __u32
     - bg_reserved
     - Padding to 64 bytes.
       (填充至 64 字节。)

.. _bgflags:

Block group flags can be any combination of the following:
(块组标志可以是以下各项的任意组合：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value
       (值)
     - Description
       (描述)
   * - 0x1
     - inode table and bitmap are not initialized (EXT4_BG_INODE_UNINIT).
       (inode 表和位图未初始化(EXT4_BG_INODE_UNINIT)。)
   * - 0x2
     - block bitmap is not initialized (EXT4_BG_BLOCK_UNINIT).
       (块位图未初始化(EXT4_BG_BLOCK_UNINIT)。)
   * - 0x4
     - inode table is zeroed (EXT4_BG_INODE_ZEROED).
       (inode 表已被清零(EXT4_BG_INODE_ZEROED)。)
