.. SPDX-License-Identifier: GPL-2.0

Inode Table
(inode 表)
-----------

Inode tables are statically allocated at mkfs time.  Each block group
descriptor points to the start of the table, and the superblock records
the number of inodes per group.  See :doc:`inode documentation <inodes>`
for more information on inode table layout.
(inode 表在 mkfs 时静态分配。每个块组描述符指向该表的起始位置，超级块记录每组的 inode 数量。有关 inode 表布局的更多信息，参见 :doc:`inode documentation <inodes>`。)
