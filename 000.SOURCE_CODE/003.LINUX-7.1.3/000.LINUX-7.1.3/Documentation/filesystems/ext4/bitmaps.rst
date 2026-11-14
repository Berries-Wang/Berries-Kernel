.. SPDX-License-Identifier: GPL-2.0

Block and inode Bitmaps
(块与 inode 位图)
-----------------------

The data block bitmap tracks the usage of data blocks within the block
group.
(数据块位图跟踪块组内数据块的使用情况。)

The inode bitmap records which entries in the inode table are in use.
(inode 位图记录 inode 表中哪些条目正在使用。)

As with most bitmaps, one bit represents the usage status of one data
block or inode table entry. This implies a block group size of 8 *
number_of_bytes_in_a_logical_block.
(与大多数位图一样，一个位代表一个数据块或 inode 表条目的使用状态。这意味着块组大小为 8 * number_of_bytes_in_a_logical_block。)

NOTE: If ``BLOCK_UNINIT`` is set for a given block group, various parts
of the kernel and e2fsprogs code pretends that the block bitmap contains
zeros (i.e. all blocks in the group are free). However, it is not
necessarily the case that no blocks are in use -- if ``meta_bg`` is set,
the bitmaps and group descriptor live inside the group. Unfortunately,
ext2fs_test_block_bitmap2() will return '0' for those locations,
which produces confusing debugfs output.
(注意：如果为给定块组设置了 ``BLOCK_UNINIT``，内核和 e2fsprogs 代码的多个部分会假装块位图包含全零(即组内所有块都是空闲的)。然而，这并不一定意味着没有块在使用 —— 如果设置了 ``meta_bg``，位图和组描述符就位于该组内部。遗憾的是，ext2fs_test_block_bitmap2() 对那些位置会返回 '0'，这会产生令人困惑的 debugfs 输出。)
