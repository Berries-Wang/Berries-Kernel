.. SPDX-License-Identifier: GPL-2.0

About this Book
(关于本书)
===============

This document attempts to describe the on-disk format for ext4
filesystems. The same general ideas should apply to ext2/3 filesystems
as well, though they do not support all the features that ext4 supports,
and the fields will be shorter.
(本文档试图描述 ext4 文件系统的磁盘上格式。同样的总体思路应该也适用于 ext2/3 文件系统，尽管它们不支持 ext4 支持的所有特性，而且字段会更短。)

**NOTE**: This is a work in progress, based on notes that the author
(djwong) made while picking apart a filesystem by hand. The data
structure definitions should be current as of Linux 4.18 and
e2fsprogs-1.44. All comments and corrections are welcome, since there is
undoubtedly plenty of lore that might not be reflected in freshly
created demonstration filesystems.
(**注意**：这是一项进行中的工作，基于作者(djwong)在手工剖析文件系统时所做的笔记。数据结构定义截至 Linux 4.18 和 e2fsprogs-1.44 应为最新。欢迎所有评论和更正，因为毫无疑问有大量背景知识可能不会反映在新创建的演示文件系统中。)

License
(许可证)
-------
This book is licensed under the terms of the GNU Public License, v2.
(本书按照 GNU Public License v2 的条款授权。)

Terminology
(术语)
-----------

ext4 divides a storage device into an array of logical blocks both to
reduce bookkeeping overhead and to increase throughput by forcing larger
transfer sizes. Generally, the block size will be 4KiB (the same size as
pages on x86 and the block layer's default block size), though the
actual size is calculated as 2 ^ (10 + ``sb.s_log_block_size``) bytes.
Throughout this document, disk locations are given in terms of these
logical blocks, not raw LBAs, and not 1024-byte blocks. For the sake of
convenience, the logical block size will be referred to as
``$block_size`` throughout the rest of the document.
(ext4 将存储设备划分为一组逻辑块，这既是为了减少簿记开销，也是为了通过强制更大的传输大小来提高吞吐量。通常，块大小将是 4KiB(与 x86 上的页以及块层的默认块大小相同)，不过实际大小按 2 ^ (10 + ``sb.s_log_block_size``) 字节计算。在本文档中，磁盘位置均以这些逻辑块表示，而不是原始 LBA，也不是 1024 字节块。为方便起见，在文档其余部分中，逻辑块大小将被称为 ``$block_size``。)

When referenced in ``preformatted text`` blocks, ``sb`` refers to fields
in the super block, and ``inode`` refers to fields in an inode table
entry.
(在 ``preformatted text`` 块中被引用时，``sb`` 指超级块中的字段，``inode`` 指 inode 表条目中的字段。)

Other References
(其他参考资料)
----------------

Also see https://www.nongnu.org/ext2-doc/ for quite a collection of
information about ext2/3. Here's another old reference:
http://wiki.osdev.org/Ext2
(另请参见 https://www.nongnu.org/ext2-doc/，那里收录了大量关于 ext2/3 的资料。这是另一个较早的参考资料：http://wiki.osdev.org/Ext2。)
