.. SPDX-License-Identifier: GPL-2.0

The Contents of inode.i_block
(inode.i_block 的内容)
------------------------------

Depending on the type of file an inode describes, the 60 bytes of
storage in ``inode.i_block`` can be used in different ways. In general,
regular files and directories will use it for file block indexing
information, and special files will use it for special purposes.
(根据 inode 所描述的文件类型，``inode.i_block`` 中的 60 字节存储空间可以以不同的方式使用。一般来说，常规文件和目录会将其用于文件块索引信息，而特殊文件会将其用于特殊用途。)

Symbolic Links
(符号链接)
~~~~~~~~~~~~~~

The target of a symbolic link will be stored in this field if the target
string is less than 60 bytes long. Otherwise, either extents or block
maps will be used to allocate data blocks to store the link target.
(如果符号链接的目标字符串长度小于 60 字节，链接目标将存储在此字段中。否则，将使用 extent 或块映射来分配数据块以存储链接目标。)

Direct/Indirect Block Addressing
(直接/间接块寻址)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In ext2/3, file block numbers were mapped to logical block numbers by
means of an (up to) three level 1-1 block map. To find the logical block
that stores a particular file block, the code would navigate through
this increasingly complicated structure. Notice that there is neither a
magic number nor a checksum to provide any level of confidence that the
block isn't full of garbage.
(在 ext2/3 中，文件块号通过(最多)三层的 1-1 块映射被映射到逻辑块号。要找到存储某个特定文件块的逻辑块，代码需要遍历这个日益复杂的结构。注意，既没有魔数也没有校验和来提供任何程度的信心，保证该块没有充满垃圾数据。)

.. ifconfig:: builder != 'latex'

   .. include:: blockmap.rst

.. ifconfig:: builder == 'latex'

   [Table omitted because LaTeX doesn't support nested tables.]

Note that with this block mapping scheme, it is necessary to fill out a
lot of mapping data even for a large contiguous file! This inefficiency
led to the creation of the extent mapping scheme, discussed below.
(注意，在这种块映射方案下，即使对于一个大的连续文件，也必须填写大量的映射数据！这种低效导致了 extent 映射方案的产生，下文将对此进行讨论。)

Notice also that a file using this mapping scheme cannot be placed
higher than 2^32 blocks.
(还要注意，使用这种映射方案的文件不能被放置在高于 2^32 块的位置。)

Extent Tree
(extent 树)
~~~~~~~~~~~

In ext4, the file to logical block map has been replaced with an extent
tree. Under the old scheme, allocating a contiguous run of 1,000 blocks
requires an indirect block to map all 1,000 entries; with extents, the
mapping is reduced to a single ``struct ext4_extent`` with
``ee_len = 1000``. If flex_bg is enabled, it is possible to allocate
very large files with a single extent, at a considerable reduction in
metadata block use, and some improvement in disk efficiency. The inode
must have the extents flag (0x80000) flag set for this feature to be in
use.
(在 ext4 中，文件到逻辑块的映射已被 extent 树取代。在旧方案下，分配一段连续的 1000 个块需要一个间接块来映射全部 1000 个表项；而使用 extent 时，映射被缩减为一个 ``ee_len = 1000`` 的 ``struct ext4_extent``。如果启用了 flex_bg，就可以用单个 extent 分配非常大的文件，从而大幅减少元数据块的使用，并一定程度地提高磁盘效率。inode 必须设置 extents 标志(0x80000)，此特性才能生效。)

Extents are arranged as a tree. Each node of the tree begins with a
``struct ext4_extent_header``. If the node is an interior node
(``eh.eh_depth`` > 0), the header is followed by ``eh.eh_entries``
instances of ``struct ext4_extent_idx``; each of these index entries
points to a block containing more nodes in the extent tree. If the node
is a leaf node (``eh.eh_depth == 0``), then the header is followed by
``eh.eh_entries`` instances of ``struct ext4_extent``; these instances
point to the file's data blocks. The root node of the extent tree is
stored in ``inode.i_block``, which allows for the first four extents to
be recorded without the use of extra metadata blocks.
(extent 以树的形式组织。树的每个节点都以一个 ``struct ext4_extent_header`` 开头。如果该节点是内部节点(``eh.eh_depth`` > 0)，头部之后跟着 ``eh.eh_entries`` 个 ``struct ext4_extent_idx`` 实例；这些索引项中的每一个都指向一个包含 extent 树中更多节点的块。如果该节点是叶子节点(``eh.eh_depth`` == 0)，则头部之后跟着 ``eh.eh_entries`` 个 ``struct ext4_extent`` 实例；这些实例指向文件的数据块。extent 树的根节点存储在 ``inode.i_block`` 中，这使得前四个 extent 无需使用额外的元数据块即可记录。)

The extent tree header is recorded in ``struct ext4_extent_header``,
which is 12 bytes long:
(extent 树头部记录在 ``struct ext4_extent_header`` 中，其长度为 12 字节：)

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
     - eh_magic
     - Magic number, 0xF30A.
       (魔数，0xF30A。)
   * - 0x2
     - __le16
     - eh_entries
     - Number of valid entries following the header.
       (头部之后的有效表项数量。)
   * - 0x4
     - __le16
     - eh_max
     - Maximum number of entries that could follow the header.
       (头部之后可能跟随的表项的最大数量。)
   * - 0x6
     - __le16
     - eh_depth
     - Depth of this extent node in the extent tree. 0 = this extent node
       points to data blocks; otherwise, this extent node points to other
       extent nodes. The extent tree can be at most 5 levels deep: a logical
       block number can be at most ``2^32``, and the smallest ``n`` that
       satisfies ``4*(((blocksize - 12)/12)^n) >= 2^32`` is 5.
       (此 extent 节点在 extent 树中的深度。0 = 此 extent 节点指向数据块；否则，此 extent 节点指向其他 extent 节点。extent 树最多可以有 5 层深：逻辑块号最大为 ``2^32``，而满足 ``4*(((blocksize - 12)/12)^n) >= 2^32`` 的最小 ``n`` 是 5。)
   * - 0x8
     - __le32
     - eh_generation
     - Generation of the tree. (Used by Lustre, but not standard ext4).
       (树的世代号。(由 Lustre 使用，但不是标准 ext4 的一部分)。)

Internal nodes of the extent tree, also known as index nodes, are
recorded as ``struct ext4_extent_idx``, and are 12 bytes long:
(extent 树的内部节点也称为索引节点，记录为 ``struct ext4_extent_idx``，长度为 12 字节：)

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
     - ei_block
     - This index node covers file blocks from 'block' onward.
       (此索引节点覆盖从 'block' 开始的文件块。)
   * - 0x4
     - __le32
     - ei_leaf_lo
     - Lower 32-bits of the block number of the extent node that is the next
       level lower in the tree. The tree node pointed to can be either another
       internal node or a leaf node, described below.
       (树中下一层更低一级的 extent 节点的块号的低 32 位。所指向的树节点可以是另一个内部节点，也可以是下文描述的叶子节点。)
   * - 0x8
     - __le16
     - ei_leaf_hi
     - Upper 16-bits of the previous field.
       (前一个字段的高 16 位。)
   * - 0xA
     - __u16
     - ei_unused
     -

Leaf nodes of the extent tree are recorded as ``struct ext4_extent``,
and are also 12 bytes long:
(extent 树的叶子节点记录为 ``struct ext4_extent``，长度同样为 12 字节：)

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
     - ee_block
     - First file block number that this extent covers.
       (此 extent 覆盖的第一个文件块号。)
   * - 0x4
     - __le16
     - ee_len
     - Number of blocks covered by extent. If the value of this field is <=
       32768, the extent is initialized. If the value of the field is > 32768,
       the extent is uninitialized and the actual extent length is ``ee_len`` -
       32768. Therefore, the maximum length of a initialized extent is 32768
       blocks, and the maximum length of an uninitialized extent is 32767.
       (extent 覆盖的块数。如果此字段的值 <= 32768，则该 extent 是已初始化的。如果此字段的值 > 32768，则该 extent 是未初始化的，实际 extent 长度为 ``ee_len`` - 32768。因此，已初始化 extent 的最大长度是 32768 个块，而未初始化 extent 的最大长度是 32767。)
   * - 0x6
     - __le16
     - ee_start_hi
     - Upper 16-bits of the block number to which this extent points.
       (此 extent 所指向的块号的高 16 位。)
   * - 0x8
     - __le32
     - ee_start_lo
     - Lower 32-bits of the block number to which this extent points.
       (此 extent 所指向的块号的低 32 位。)

Prior to the introduction of metadata checksums, the extent header +
extent entries always left at least 4 bytes of unallocated space at the
end of each extent tree data block (because (2^x % 12) >= 4). Therefore,
the 32-bit checksum is inserted into this space. The 4 extents in the
inode do not need checksumming, since the inode is already checksummed.
The checksum is calculated against the FS UUID, the inode number, the
inode generation, and the entire extent block leading up to (but not
including) the checksum itself.
(在引入元数据校验和之前，extent 头部 + extent 表项总是在每个 extent 树数据块的末尾留下至少 4 字节未分配的空间(因为 (2^x % 12) >= 4)。因此，32 位校验和被插入到这个空间中。inode 中的 4 个 extent 不需要计算校验和，因为 inode 本身已经有校验和了。校验和是针对 FS UUID、inode 编号、inode 世代号，以及从块开头直到(但不包括)校验和本身的整个 extent 块来计算的。)

``struct ext4_extent_tail`` is 4 bytes long:
(``struct ext4_extent_tail`` 的长度为 4 字节：)

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
     - eb_checksum
     - Checksum of the extent block, crc32c(uuid+inum+igeneration+extentblock)
       (extent 块的校验和，crc32c(uuid+inum+igeneration+extentblock))

Inline Data
(内联数据)
~~~~~~~~~~~

If the inline data feature is enabled for the filesystem and the flag is
set for the inode, it is possible that the first 60 bytes of the file
data are stored here.
(如果文件系统启用了内联数据特性，并且该 inode 设置了相应的标志，那么文件数据的前 60 字节就有可能存储在这里。)
