.. SPDX-License-Identifier: GPL-2.0

Inline Data
(内联数据)
-----------

The inline data feature was designed to handle the case that a file's
data is so tiny that it readily fits inside the inode, which
(theoretically) reduces disk block consumption and reduces seeks. If the
file is smaller than 60 bytes, then the data are stored inline in
``inode.i_block``. If the rest of the file would fit inside the extended
attribute space, then it might be found as an extended attribute
“system.data” within the inode body (“ibody EA”). This of course
constrains the amount of extended attributes one can attach to an inode.
If the data size increases beyond i_block + ibody EA, a regular block
is allocated and the contents moved to that block.
(内联数据特性旨在处理文件的数据非常小、可以轻松放入 inode 的情况，这(在理论上)减少了磁盘块的消耗并减少了寻道。如果文件小于 60 字节，则数据内联存储在 ``inode.i_block`` 中。如果文件的剩余部分可以放入扩展属性空间，则它可能会作为 inode 主体中的扩展属性 “system.data”(“ibody EA”)被找到。这当然限制了可以附加到一个 inode 的扩展属性数量。如果数据大小超过 i_block + ibody EA，则会分配一个常规块，并将内容移动到该块。)

Pending a change to compact the extended attribute key used to store
inline data, one ought to be able to store 160 bytes of data in a
256-byte inode (as of June 2015, when i_extra_isize is 28). Prior to
that, the limit was 156 bytes due to inefficient use of inode space.
(在压缩用于存储内联数据的扩展属性键的变更完成之前，一个 256 字节的 inode 应该能够存储 160 字节的数据(截至 2015 年 6 月，此时 i_extra_isize 为 28)。在此之前，由于 inode 空间使用低效，限制是 156 字节。)

The inline data feature requires the presence of an extended attribute
for “system.data”, even if the attribute value is zero length.
(内联数据特性要求存在针对 “system.data” 的扩展属性，即使属性值长度为零。)

Inline Directories
(内联目录)
~~~~~~~~~~~~~~~~~~

The first four bytes of i_block are the inode number of the parent
directory. Following that is a 56-byte space for an array of directory
entries; see ``struct ext4_dir_entry``. If there is a “system.data”
attribute in the inode body, the EA value is an array of
``struct ext4_dir_entry`` as well. Note that for inline directories, the
i_block and EA space are treated as separate dirent blocks; directory
entries cannot span the two.
(i_block 的前四个字节是父目录的 inode 编号。其后是一个 56 字节的空间，用于存放目录项数组；参见 ``struct ext4_dir_entry``。如果 inode 主体中存在 “system.data” 属性，则 EA 值同样是一个 ``struct ext4_dir_entry`` 数组。注意，对于内联目录，i_block 和 EA 空间被视为独立的 dirent 块；目录项不能跨越这两者。)

Inline directory entries are not checksummed, as the inode checksum
should protect all inline data contents.
(内联目录项不进行校验和计算，因为 inode 校验和应该保护所有内联数据内容。)
