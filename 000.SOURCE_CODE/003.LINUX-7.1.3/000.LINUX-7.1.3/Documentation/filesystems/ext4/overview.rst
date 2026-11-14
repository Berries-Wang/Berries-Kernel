.. SPDX-License-Identifier: GPL-2.0

High Level Design
(高层设计)
=================

An ext4 file system is split into a series of block groups. To reduce
performance difficulties due to fragmentation, the block allocator tries
very hard to keep each file's blocks within the same group, thereby
reducing seek times. The size of a block group is specified in
``sb.s_blocks_per_group`` blocks, though it can also calculated as 8 *
``block_size_in_bytes``. With the default block size of 4KiB, each group
will contain 32,768 blocks, for a length of 128MiB. The number of block
groups is the size of the device divided by the size of a block group.
(ext4 文件系统被划分为一系列块组。为了减少碎片化导致的性能问题，块分配器竭力将每个文件的块保持在同一个块组内，从而减少寻道时间。块组的大小以 ``sb.s_blocks_per_group`` 个块指定，不过也可以按 8 * ``block_size_in_bytes`` 计算。在默认块大小 4KiB 的情况下，每个块组将包含 32,768 个块，长度为 128MiB。块组的数量是设备大小除以块组大小。)

All fields in ext4 are written to disk in little-endian order. HOWEVER,
all fields in jbd2 (the journal) are written to disk in big-endian
order.
(ext4 中的所有字段都以小端序写入磁盘。但是，jbd2(日志)中的所有字段都以大端序写入磁盘。)

.. toctree::

   blocks
   blockgroup
   special_inodes
   allocators
   checksums
   bigalloc
   inlinedata
   eainode
   verity
   atomic_writes
