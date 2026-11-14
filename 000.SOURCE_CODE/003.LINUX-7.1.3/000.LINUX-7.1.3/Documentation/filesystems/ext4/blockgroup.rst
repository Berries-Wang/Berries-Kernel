.. SPDX-License-Identifier: GPL-2.0

Block Groups
(块组)
------------

Layout
(布局)
~~~~~~

The layout of a standard block group is approximately as follows (each
of these fields is discussed in a separate section below):
(标准块组的布局大致如下(以下每个字段都会在后面的单独小节中讨论)：)

.. list-table::
   :widths: 1 1 1 1 1 1 1 1
   :header-rows: 1

   * - Group 0 Padding(组 0 填充)
     - ext4 Super Block(ext4 超级块)
     - Group Descriptors(组描述符)
     - Reserved GDT Blocks(保留 GDT 块)
     - Data Block Bitmap(数据块位图)
     - inode Bitmap(inode 位图)
     - inode Table(inode 表)
     - Data Blocks(数据块)
   * - 1024 bytes(1024 字节)
     - 1 block(1 个块)
     - many blocks(多个块)
     - many blocks(多个块)
     - 1 block(1 个块)
     - 1 block(1 个块)
     - many blocks(多个块)
     - many more blocks(更多的块)

For the special case of block group 0, the first 1024 bytes are unused,
to allow for the installation of x86 boot sectors and other oddities.
The superblock will start at offset 1024 bytes, whichever block that
happens to be (usually 0). However, if for some reason the block size =
1024, then block 0 is marked in use and the superblock goes in block 1.
For all other block groups, there is no padding.
(对于块组 0 这一特殊情况，最前面的 1024 字节未被使用，以便安装 x86 引导扇区和其他内容。超级块将从偏移 1024 字节处开始，无论那恰好是哪个块(通常为 0)。但是，如果由于某种原因块大小 = 1024，那么块 0 将被标记为已使用，超级块则放入块 1。对于所有其他块组，没有填充。)

The ext4 driver primarily works with the superblock and the group
descriptors that are found in block group 0. Redundant copies of the
superblock and group descriptors are written to some of the block groups
across the disk in case the beginning of the disk gets trashed, though
not all block groups necessarily host a redundant copy (see following
paragraph for more details). If the group does not have a redundant
copy, the block group begins with the data block bitmap. Note also that
when the filesystem is freshly formatted, mkfs will allocate “reserve
GDT block” space after the block group descriptors and before the start
of the block bitmaps to allow for future expansion of the filesystem. By
default, a filesystem is allowed to increase in size by a factor of
1024x over the original filesystem size.
(ext4 驱动主要使用块组 0 中的超级块和组描述符。超级块和组描述符的冗余副本会被写入磁盘上的一些块组中，以防磁盘开头被破坏，不过并非所有块组都必定承载一个冗余副本(详见下一段)。如果该组没有冗余副本，块组将从数据块位图开始。另请注意，当文件系统刚被格式化时，mkfs 会在块组描述符之后、块位图开始之前分配“保留 GDT 块”空间，以便将来扩展文件系统。默认情况下，文件系统的大小允许在原始文件系统大小的基础上增加 1024 倍。)

The location of the inode table is given by ``grp.bg_inode_table_*``. It
is continuous range of blocks large enough to contain
``sb.s_inodes_per_group * sb.s_inode_size`` bytes.
(inode 表的位置由 ``grp.bg_inode_table_*`` 给出。它是一段连续的块区间，大小足以容纳 ``sb.s_inodes_per_group * sb.s_inode_size`` 字节。)

As for the ordering of items in a block group, it is generally
established that the super block and the group descriptor table, if
present, will be at the beginning of the block group. The bitmaps and
the inode table can be anywhere, and it is quite possible for the
bitmaps to come after the inode table, or for both to be in different
groups (flex_bg). Leftover space is used for file data blocks, indirect
block maps, extent tree blocks, and extended attributes.
(至于块组中各项的排列顺序，通常约定：超级块和组描述符表(如果存在)位于块组的开头。位图和 inode 表可以位于任何位置，位图完全可能出现在 inode 表之后，或者两者位于不同的组中(flex_bg)。剩余空间用于文件数据块、间接块映射、extent 树块和扩展属性。)

Flexible Block Groups
(灵活块组)
~~~~~~~~~~~~~~~~~~~~~

Starting in ext4, there is a new feature called flexible block groups
(flex_bg). In a flex_bg, several block groups are tied together as one
logical block group; the bitmap spaces and the inode table space in the
first block group of the flex_bg are expanded to include the bitmaps
and inode tables of all other block groups in the flex_bg. For example,
if the flex_bg size is 4, then group 0 will contain (in order) the
superblock, group descriptors, data block bitmaps for groups 0-3, inode
bitmaps for groups 0-3, inode tables for groups 0-3, and the remaining
space in group 0 is for file data. The effect of this is to group the
block group metadata close together for faster loading, and to enable
large files to be continuous on disk. Backup copies of the superblock
and group descriptors are always at the beginning of block groups, even
if flex_bg is enabled. The number of block groups that make up a
flex_bg is given by 2 ^ ``sb.s_log_groups_per_flex``.
(从 ext4 开始，出现了一项名为灵活块组(flex_bg)的新特性。在 flex_bg 中，若干块组被捆绑为一个逻辑块组；flex_bg 中第一个块组的位图空间和 inode 表空间会被扩展，以包含 flex_bg 中所有其他块组的位图和 inode 表。例如，如果 flex_bg 的大小为 4，那么组 0 将(按顺序)包含超级块、组描述符、组 0-3 的数据块位图、组 0-3 的 inode 位图、组 0-3 的 inode 表，而组 0 中剩余的空间用于文件数据。这样做的效果是把块组元数据聚集在一起以加快加载速度，并使大文件能够在磁盘上连续存放。超级块和组描述符的备份副本始终位于块组的开头，即使启用了 flex_bg 也是如此。组成一个 flex_bg 的块组数量由 2 ^ ``sb.s_log_groups_per_flex`` 给出。)

Meta Block Groups
(元块组)
~~~~~~~~~~~~~~~~~

Without the option META_BG, for safety concerns, all block group
descriptors copies are kept in the first block group. Given the default
128MiB(2^27 bytes) block group size and 64-byte group descriptors, ext4
can have at most 2^27/64 = 2^21 block groups. This limits the entire
filesystem size to 2^21 * 2^27 = 2^48bytes or 256TiB.
(在不使用 META_BG 选项的情况下，出于安全考虑，所有块组描述符副本都保存在第一个块组中。鉴于默认 128MiB(2^27 字节)的块组大小和 64 字节的组描述符，ext4 最多只能有 2^27/64 = 2^21 个块组。这将整个文件系统的大小限制为 2^21 * 2^27 = 2^48 字节，即 256TiB。)

The solution to this problem is to use the metablock group feature
(META_BG), which is already in ext3 for all 2.6 releases. With the
META_BG feature, ext4 filesystems are partitioned into many metablock
groups. Each metablock group is a cluster of block groups whose group
descriptor structures can be stored in a single disk block. For ext4
filesystems with 4 KB block size, a single metablock group partition
includes 64 block groups, or 8 GiB of disk space. The metablock group
feature moves the location of the group descriptors from the congested
first block group of the whole filesystem into the first group of each
metablock group itself. The backups are in the second and last group of
each metablock group. This increases the 2^21 maximum block groups limit
to the hard limit 2^32, allowing support for a 512PiB filesystem.
(解决这一问题的办法是使用元块组特性(META_BG)，该特性已经存在于所有 2.6 版本的 ext3 中。借助 META_BG 特性，ext4 文件系统被划分为许多元块组。每个元块组是若干块组组成的一个集群，其组描述符结构可以存储在单个磁盘块中。对于块大小为 4 KB 的 ext4 文件系统，单个元块组分区包含 64 个块组，即 8 GiB 的磁盘空间。元块组特性将组描述符的位置从整个文件系统中拥挤的第一个块组移到每个元块组自身的第一个组。备份位于每个元块组的第二个组和最后一个组。这将 2^21 的最大块组数限制提高到硬性限制 2^32，从而支持最大 512PiB 的文件系统。)

The change in the filesystem format replaces the current scheme where
the superblock is followed by a variable-length set of block group
descriptors. Instead, the superblock and a single block group descriptor
block is placed at the beginning of the first, second, and last block
groups in a meta-block group. A meta-block group is a collection of
block groups which can be described by a single block group descriptor
block. Since the size of the block group descriptor structure is 64
bytes, a meta-block group contains 16 block groups for filesystems with
a 1KB block size, and 64 block groups for filesystems with a 4KB
blocksize. Filesystems can either be created using this new block group
descriptor layout, or existing filesystems can be resized on-line, and
the field s_first_meta_bg in the superblock will indicate the first
block group using this new layout.
(文件系统格式的这一改变取代了当前超级块后跟一组可变长度块组描述符的方案。取而代之的是，超级块和单个块组描述符块被放置在元块组中第一个、第二个和最后一个块组的开头。元块组是可以由单个块组描述符块描述的块组集合。由于块组描述符结构的大小为 64 字节，对于块大小为 1KB 的文件系统，一个元块组包含 16 个块组；对于块大小为 4KB 的文件系统，一个元块组包含 64 个块组。文件系统既可以在创建时就采用这种新的块组描述符布局，也可以对现有文件系统进行在线扩容，超级块中的 s_first_meta_bg 字段将指示第一个使用这种新布局的块组。)

Please see an important note about ``BLOCK_UNINIT`` in the section about
block and inode bitmaps.
(关于 ``BLOCK_UNINIT`` 的重要说明，请参见关于块位图与 inode 位图的小节。)

Lazy Block Group Initialization
(惰性块组初始化)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A new feature for ext4 are three block group descriptor flags that
enable mkfs to skip initializing other parts of the block group
metadata. Specifically, the INODE_UNINIT and BLOCK_UNINIT flags mean
that the inode and block bitmaps for that group can be calculated and
therefore the on-disk bitmap blocks are not initialized. This is
generally the case for an empty block group or a block group containing
only fixed-location block group metadata. The INODE_ZEROED flag means
that the inode table has been initialized; mkfs will unset this flag and
rely on the kernel to initialize the inode tables in the background.
(ext4 的一项新特性是三个块组描述符标志，它们使 mkfs 能够跳过初始化块组元数据的其他部分。具体而言，INODE_UNINIT 和 BLOCK_UNINIT 标志意味着该组的 inode 位图和块位图可以被计算出来，因此磁盘上的位图块无需初始化。对于空的块组或仅包含固定位置块组元数据的块组，通常就是这种情况。INODE_ZEROED 标志表示 inode 表已被初始化；mkfs 会清除该标志，并依靠内核在后台初始化 inode 表。)

By not writing zeroes to the bitmaps and inode table, mkfs time is
reduced considerably. Note the feature flag is RO_COMPAT_GDT_CSUM,
but the dumpe2fs output prints this as “uninit_bg”. They are the same
thing.
(由于不向位图和 inode 表写零，mkfs 的耗时大幅缩短。注意该特性标志是 RO_COMPAT_GDT_CSUM，但 dumpe2fs 的输出将其打印为“uninit_bg”。它们是同一回事。)
