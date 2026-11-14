.. SPDX-License-Identifier: GPL-2.0

Directory Entries
(目录项)
-----------------

In an ext4 filesystem, a directory is more or less a flat file that maps
an arbitrary byte string (usually ASCII) to an inode number on the
filesystem. There can be many directory entries across the filesystem
that reference the same inode number--these are known as hard links, and
that is why hard links cannot reference files on other filesystems. As
such, directory entries are found by reading the data block(s)
associated with a directory file for the particular directory entry that
is desired.
(在 ext4 文件系统中，目录或多或少是一个扁平文件，它将任意字节串(通常是 ASCII)映射到文件系统上的 inode 号。文件系统各处可以有许多引用同一 inode 号的目录项——这些被称为硬链接，这也是硬链接不能引用其他文件系统上的文件的原因。因此，查找目录项的方式就是读取与目录文件相关联的数据块，从中找到所需的特定目录项。)

Linear (Classic) Directories
(线性(经典)目录)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By default, each directory lists its entries in an “almost-linear”
array. I write “almost” because it's not a linear array in the memory
sense because directory entries are not split across filesystem blocks.
Therefore, it is more accurate to say that a directory is a series of
data blocks and that each block contains a linear array of directory
entries. The end of each per-block array is signified by reaching the
end of the block; the last entry in the block has a record length that
takes it all the way to the end of the block. The end of the entire
directory is of course signified by reaching the end of the file. Unused
directory entries are signified by inode = 0. By default the filesystem
uses ``struct ext4_dir_entry_2`` for directory entries unless the
“filetype” feature flag is not set, in which case it uses
``struct ext4_dir_entry``.
(默认情况下，每个目录在一个“几乎线性”的数组中列出其条目。我写“几乎”是因为它不是内存意义上的线性数组，因为目录项不会跨文件系统块拆分。因此，更准确的说法是：目录是一系列数据块，每个块包含一个目录项的线性数组。每个块内数组的结束以到达块末尾为标志；块中最后一个条目的记录长度会一直延伸到块的末尾。整个目录的结束当然以到达文件末尾为标志。未使用的目录项以 inode = 0 表示。默认情况下，文件系统对目录项使用 ``struct ext4_dir_entry_2``，除非“filetype”特性标志未设置，在这种情况下它使用 ``struct ext4_dir_entry``。)

The original directory entry format is ``struct ext4_dir_entry``, which
is at most 263 bytes long, though on disk you'll need to reference
``dirent.rec_len`` to know for sure.
(原始的目录项格式是 ``struct ext4_dir_entry``，其长度最多为 263 字节，不过在磁盘上你需要参考 ``dirent.rec_len`` 才能确定。)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset (偏移)
     - Size (大小)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - __le32
     - inode
     - Number of the inode that this directory entry points to. (此目录项所指向的 inode 的编号。)
   * - 0x4
     - __le16
     - rec_len
     - Length of this directory entry. Must be a multiple of 4. (此目录项的长度。必须是 4 的倍数。)
   * - 0x6
     - __le16
     - name_len
     - Length of the file name. (文件名的长度。)
   * - 0x8
     - char
     - name[EXT4_NAME_LEN]
     - File name. (文件名。)

Since file names cannot be longer than 255 bytes, the new directory
entry format shortens the name_len field and uses the space for a file
type flag, probably to avoid having to load every inode during directory
tree traversal. This format is ``ext4_dir_entry_2``, which is at most
263 bytes long, though on disk you'll need to reference
``dirent.rec_len`` to know for sure.
(由于文件名不能超过 255 字节，新的目录项格式缩短了 name_len 字段，并将腾出的空间用作文件类型标志，这大概是为了避免在目录树遍历期间不得不加载每个 inode。这种格式是 ``ext4_dir_entry_2``，其长度最多为 263 字节，不过在磁盘上你需要参考 ``dirent.rec_len`` 才能确定。)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset (偏移)
     - Size (大小)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - __le32
     - inode
     - Number of the inode that this directory entry points to. (此目录项所指向的 inode 的编号。)
   * - 0x4
     - __le16
     - rec_len
     - Length of this directory entry. (此目录项的长度。)
   * - 0x6
     - __u8
     - name_len
     - Length of the file name. (文件名的长度。)
   * - 0x7
     - __u8
     - file_type
     - File type code, see ftype_ table below. (文件类型代码，参见下文的 ftype_ 表。)
   * - 0x8
     - char
     - name[EXT4_NAME_LEN]
     - File name. (文件名。)

.. _ftype:

The directory file type is one of the following values:
(目录文件类型是以下值之一：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value (值)
     - Description (描述)
   * - 0x0
     - Unknown. (未知。)
   * - 0x1
     - Regular file. (常规文件。)
   * - 0x2
     - Directory. (目录。)
   * - 0x3
     - Character device file. (字符设备文件。)
   * - 0x4
     - Block device file. (块设备文件。)
   * - 0x5
     - FIFO. (FIFO。)
   * - 0x6
     - Socket. (套接字。)
   * - 0x7
     - Symbolic link. (符号链接。)

To support directories that are both encrypted and casefolded directories, we
must also include hash information in the directory entry. We append
``ext4_extended_dir_entry_2`` to ``ext4_dir_entry_2`` except for the entries
for dot and dotdot, which are kept the same. The structure follows immediately
after ``name`` and is included in the size listed by ``rec_len`` If a directory
entry uses this extension, it may be up to 271 bytes.
(为了支持既是加密目录又是大小写折叠目录的情况，我们还必须在目录项中包含哈希信息。我们在 ``ext4_dir_entry_2`` 之后附加 ``ext4_extended_dir_entry_2``，但 dot 和 dotdot 条目除外，它们保持不变。该结构紧跟在 ``name`` 之后，并计入 ``rec_len`` 所列的大小中。如果目录项使用了此扩展，其长度可达 271 字节。)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset (偏移)
     - Size (大小)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - __le32
     - hash
     - The hash of the directory name (目录名的哈希)
   * - 0x4
     - __le32
     - minor_hash
     - The minor hash of the directory name (目录名的次哈希)


In order to add checksums to these classic directory blocks, a phony
``struct ext4_dir_entry`` is placed at the end of each leaf block to
hold the checksum. The directory entry is 12 bytes long. The inode
number and name_len fields are set to zero to fool old software into
ignoring an apparently empty directory entry, and the checksum is stored
in the place where the name normally goes. The structure is
``struct ext4_dir_entry_tail``:
(为了向这些经典目录块添加校验和，会在每个叶块的末尾放置一个假的 ``struct ext4_dir_entry`` 来保存校验和。该目录项长 12 字节。inode 号和 name_len 字段被设置为零，以骗过旧软件使其忽略这个表面为空的目录项，而校验和则存储在通常存放名称的位置。该结构是 ``struct ext4_dir_entry_tail``：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset (偏移)
     - Size (大小)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - __le32
     - det_reserved_zero1
     - Inode number, which must be zero. (inode 号，必须为零。)
   * - 0x4
     - __le16
     - det_rec_len
     - Length of this directory entry, which must be 12. (此目录项的长度，必须为 12。)
   * - 0x6
     - __u8
     - det_reserved_zero2
     - Length of the file name, which must be zero. (文件名的长度，必须为零。)
   * - 0x7
     - __u8
     - det_reserved_ft
     - File type, which must be 0xDE. (文件类型，必须为 0xDE。)
   * - 0x8
     - __le32
     - det_checksum
     - Directory leaf block checksum. (目录叶块校验和。)

The leaf directory block checksum is calculated against the FS UUID (or
the checksum seed, if that feature is enabled for the fs), the directory's
inode number, the directory's inode generation number, and the entire
directory entry block up to (but not including) the fake directory entry.
(目录叶块校验和是针对 FS UUID(如果该文件系统启用了校验和种子特性，则用校验和种子)、目录的 inode 号、目录的 inode 代数，以及直到(但不包括)假目录项的整个目录项块来计算的。)

Hash Tree Directories
(哈希树目录)
~~~~~~~~~~~~~~~~~~~~~

A linear array of directory entries isn't great for performance, so a
new feature was added to ext3 to provide a faster (but peculiar)
balanced tree keyed off a hash of the directory entry name. If the
EXT4_INDEX_FL (0x1000) flag is set in the inode, this directory uses a
hashed btree (htree) to organize and find directory entries. For
backwards read-only compatibility with ext2, interior tree nodes are actually
hidden inside the directory file, masquerading as “empty” directory entries
spanning the whole block. It was stated previously that directory entries
with the inode set to 0 are treated as unused entries; this is (ab)used to
fool the old linear-scan algorithm into skipping over those blocks containing
the interior tree node data.
(目录项的线性数组对性能并不友好，因此 ext3 增加了一个新特性，提供一种以目录项名称的哈希为键的更快的(但颇为奇特的)平衡树。如果 inode 中设置了 EXT4_INDEX_FL (0x1000) 标志，该目录就使用哈希 btree(htree)来组织和查找目录项。为了向后只读兼容 ext2，树的内部节点实际上隐藏在目录文件内，伪装成横跨整个块的“空”目录项。前文说过，inode 设置为 0 的目录项会被视为未使用的条目；这里(滥)用了这一点来骗过旧的线性扫描算法，使其跳过包含内部树节点数据的块。)

The root of the tree always lives in the first data block of the
directory. By ext2 custom, the '.' and '..' entries must appear at the
beginning of this first block, so they are put here as two
``struct ext4_dir_entry_2`` s and not stored in the tree. The rest of
the root node contains metadata about the tree and finally a hash->block
map to find nodes that are lower in the htree. If
``dx_root.info.indirect_levels`` is non-zero then the htree has that many
levels and the blocks pointed to by the root node's map are interior nodes.
These interior nodes have a zeroed out ``struct ext4_dir_entry_2`` followed by
a hash->block map to find nodes of the next level. Leaf nodes look like
classic linear directory blocks, but all of its entries have a hash value
equal or greater than the indicated hash of the parent node.
(树的根总是位于目录的第一个数据块中。按照 ext2 的惯例，'.' 和 '..' 条目必须出现在第一个块的开头，所以它们作为两个 ``struct ext4_dir_entry_2`` 放在这里，而不存储在树中。根节点的其余部分包含关于树的元数据，最后是一个 hash->block 映射，用于查找 htree 中更低层的节点。如果 ``dx_root.info.indirect_levels`` 非零，则 htree 就有那么多个层级，根节点映射所指向的块是内部节点。这些内部节点由一个清零的 ``struct ext4_dir_entry_2`` 组成，后跟一个 hash->block 映射，用于查找下一层的节点。叶节点看起来像经典的线性目录块，但其所有条目的哈希值都大于或等于父节点所指示的哈希值。)

The actual hash value for an entry name is only 31 bits, the least-significant
bit is set to 0. However, if there is a hash collision between directory
entries, the least-significant bit may get set to 1 on interior nodes in the
case where these two (or more) hash-colliding entries do not fit into one leaf
node and must be split across multiple nodes.
(条目名称的实际哈希值只有 31 位，最低有效位被设置为 0。但是，如果目录项之间存在哈希冲突，当这两个(或多个)哈希冲突的条目无法放入同一个叶节点而必须拆分到多个节点时，内部节点上的最低有效位可能被设置为 1。)

To look up a name in such a htree, the code calculates the hash of the desired
file name and uses it to find the leaf node with the range of hash values the
calculated hash falls into (in other words, a lookup works basically the same
as it would in a B-Tree keyed by the hash value), and possibly also scanning
the leaf nodes that follow (in tree order) in case of hash collisions.
(要在此类 htree 中查找名称，代码会计算所需文件名的哈希，并用它找到计算出的哈希值所落入的哈希值范围对应的叶节点(换句话说，查找的工作方式基本上与在以哈希值为键的 B 树中相同)，并且在发生哈希冲突时，可能还要(按树顺序)扫描其后的叶节点。)

To traverse the directory as a linear array (such as the old code does),
the code simply reads every data block in the directory. The blocks used
for the htree will appear to have no entries (aside from '.' and '..')
and so only the leaf nodes will appear to have any interesting content.
(要将目录作为线性数组遍历(如旧代码所做的)，代码只需读取目录中的每个数据块。用于 htree 的块看起来没有任何条目('.' 和 '..' 除外)，因此只有叶节点看起来才有任何有意义的内容。)

The root of the htree is in ``struct dx_root``, which is the full length
of a data block:
(htree 的根位于 ``struct dx_root`` 中，其长度为一个完整的数据块：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset (偏移)
     - Type (类型)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - __le32
     - dot.inode
     - inode number of this directory. (此目录的 inode 号。)
   * - 0x4
     - __le16
     - dot.rec_len
     - Length of this record, 12. (此记录的长度，12。)
   * - 0x6
     - u8
     - dot.name_len
     - Length of the name, 1. (名称的长度，1。)
   * - 0x7
     - u8
     - dot.file_type
     - File type of this entry, 0x2 (directory) (if the feature flag is set). (此条目的文件类型，0x2(目录)(如果设置了特性标志)。)
   * - 0x8
     - char
     - dot.name[4]
     - “.\0\0\0”
   * - 0xC
     - __le32
     - dotdot.inode
     - inode number of parent directory. (父目录的 inode 号。)
   * - 0x10
     - __le16
     - dotdot.rec_len
     - block_size - 12. The record length is long enough to cover all htree
       data. (block_size - 12。记录长度足以覆盖所有 htree 数据。)
   * - 0x12
     - u8
     - dotdot.name_len
     - Length of the name, 2. (名称的长度，2。)
   * - 0x13
     - u8
     - dotdot.file_type
     - File type of this entry, 0x2 (directory) (if the feature flag is set). (此条目的文件类型，0x2(目录)(如果设置了特性标志)。)
   * - 0x14
     - char
     - dotdot_name[4]
     - “..\0\0”
   * - 0x18
     - __le32
     - struct dx_root_info.reserved_zero
     - Zero. (零。)
   * - 0x1C
     - u8
     - struct dx_root_info.hash_version
     - Hash type, see dirhash_ table below. (哈希类型，参见下文的 dirhash_ 表。)
   * - 0x1D
     - u8
     - struct dx_root_info.info_length
     - Length of the tree information, 0x8. (树信息的长度，0x8。)
   * - 0x1E
     - u8
     - struct dx_root_info.indirect_levels
     - Depth of the htree. Cannot be larger than 3 if the INCOMPAT_LARGEDIR
       feature is set; cannot be larger than 2 otherwise. (htree 的深度。如果设置了 INCOMPAT_LARGEDIR 特性则不能大于 3；否则不能大于 2。)
   * - 0x1F
     - u8
     - struct dx_root_info.unused_flags
     -
   * - 0x20
     - __le16
     - limit
     - Maximum number of dx_entries that can follow this header, plus 1 for
       the header itself. (可以跟随此头的 dx_entries 的最大数量，再加上头本身的 1。)
   * - 0x22
     - __le16
     - count
     - Actual number of dx_entries that follow this header, plus 1 for the
       header itself. (跟随此头的 dx_entries 的实际数量，再加上头本身的 1。)
   * - 0x24
     - __le32
     - block
     - The block number (within the directory file) that lead to the left-most
       leaf node, i.e. the leaf containing entries with the lowest hash values. (指向最左侧叶节点(即包含最低哈希值条目的叶)的块号(在目录文件内)。)
   * - 0x28
     - struct dx_entry
     - entries[0]
     - As many 8-byte ``struct dx_entry`` as fits in the rest of the data block. (数据块剩余空间中能容纳多少个 8 字节的 ``struct dx_entry`` 就有多少个。)

.. _dirhash:

The directory hash is one of the following values:
(目录哈希是以下值之一：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Value (值)
     - Description (描述)
   * - 0x0
     - Legacy. (传统。)
   * - 0x1
     - Half MD4. (Half MD4。)
   * - 0x2
     - Tea. (Tea。)
   * - 0x3
     - Legacy, unsigned. (传统，无符号。)
   * - 0x4
     - Half MD4, unsigned. (Half MD4，无符号。)
   * - 0x5
     - Tea, unsigned. (Tea，无符号。)
   * - 0x6
     - Siphash. (Siphash。)

Interior nodes of an htree are recorded as ``struct dx_node``, which is
also the full length of a data block:
(htree 的内部节点记录为 ``struct dx_node``，它同样具有一个完整数据块的长度：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset (偏移)
     - Type (类型)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - __le32
     - fake.inode
     - Zero, to make it look like this entry is not in use. (零，使这个条目看起来未被使用。)
   * - 0x4
     - __le16
     - fake.rec_len
     - The size of the block, in order to hide all of the dx_node data. (块的大小，以便隐藏所有 dx_node 数据。)
   * - 0x6
     - u8
     - name_len
     - Zero. There is no name for this “unused” directory entry. (零。这个“未使用”的目录项没有名称。)
   * - 0x7
     - u8
     - file_type
     - Zero. There is no file type for this “unused” directory entry. (零。这个“未使用”的目录项没有文件类型。)
   * - 0x8
     - __le16
     - limit
     - Maximum number of dx_entries that can follow this header, plus 1 for
       the header itself. (可以跟随此头的 dx_entries 的最大数量，再加上头本身的 1。)
   * - 0xA
     - __le16
     - count
     - Actual number of dx_entries that follow this header, plus 1 for the
       header itself. (跟随此头的 dx_entries 的实际数量，再加上头本身的 1。)
   * - 0xE
     - __le32
     - block
     - The block number (within the directory file) that goes with the lowest
       hash value of this block. This value is stored in the parent block. (与本块的最低哈希值相对应的块号(在目录文件内)。此值存储在父块中。)
   * - 0x12
     - struct dx_entry
     - entries[0]
     - As many 8-byte ``struct dx_entry`` as fits in the rest of the data block. (数据块剩余空间中能容纳多少个 8 字节的 ``struct dx_entry`` 就有多少个。)

The hash maps that exist in both ``struct dx_root`` and
``struct dx_node`` are recorded as ``struct dx_entry``, which is 8 bytes
long:
(同时存在于 ``struct dx_root`` 和 ``struct dx_node`` 中的哈希映射记录为 ``struct dx_entry``，其长度为 8 字节：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset (偏移)
     - Type (类型)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - __le32
     - hash
     - Hash code. (哈希码。)
   * - 0x4
     - __le32
     - block
     - Block number (within the directory file, not filesystem blocks) of the
       next node in the htree. (htree 中下一个节点的块号(在目录文件内，而非文件系统块)。)

(If you think this is all quite clever and peculiar, so does the
author.)
(如果你觉得这一切相当聪明而奇特，作者也这么觉得。)

If metadata checksums are enabled, the last 8 bytes of the directory
block (precisely the length of one dx_entry) are used to store a
``struct dx_tail``, which contains the checksum. The ``limit`` and
``count`` entries in the dx_root/dx_node structures are adjusted as
necessary to fit the dx_tail into the block. If there is no space for
the dx_tail, the user is notified to run e2fsck -D to rebuild the
directory index (which will ensure that there's space for the checksum.
The dx_tail structure is 8 bytes long and looks like this:
(如果启用了元数据校验和，目录块的最后 8 字节(恰好是一个 dx_entry 的长度)会被用来存储包含校验和的 ``struct dx_tail``。dx_root/dx_node 结构中的 ``limit`` 和 ``count`` 条目会根据需要进行调整，以便将 dx_tail 放入块中。如果没有空间容纳 dx_tail，会通知用户运行 e2fsck -D 来重建目录索引(这将确保有空间存放校验和)。dx_tail 结构长 8 字节，如下所示：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset (偏移)
     - Type (类型)
     - Name (名称)
     - Description (描述)
   * - 0x0
     - u32
     - dt_reserved
     - Unused (but still part of the checksum curiously). (未使用(但奇怪的是它仍是校验和的一部分)。)
   * - 0x4
     - __le32
     - dt_checksum
     - Checksum of the htree directory block. (htree 目录块的校验和。)

The checksum is calculated against the FS UUID, the htree index header
(dx_root or dx_node), all of the htree indices (dx_entry) that are in
use, and the tail block (dx_tail) with the dt_checksum initially set to 0.
(校验和是针对 FS UUID、htree 索引头(dx_root 或 dx_node)、所有正在使用的 htree 索引(dx_entry)，以及 dt_checksum 初始设置为 0 的尾部块(dx_tail)来计算的。)
