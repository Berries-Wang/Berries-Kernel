.. SPDX-License-Identifier: GPL-2.0

Checksums
(校验和)
---------

Starting in early 2012, metadata checksums were added to all major ext4
and jbd2 data structures. The associated feature flag is metadata_csum.
The desired checksum algorithm is indicated in the superblock, though as
of October 2012 the only supported algorithm is crc32c. Some data
structures did not have space to fit a full 32-bit checksum, so only the
lower 16 bits are stored. Enabling the 64bit feature increases the data
structure size so that full 32-bit checksums can be stored for many data
structures. However, existing 32-bit filesystems cannot be extended to
enable 64bit mode, at least not without the experimental resize2fs
patches to do so.
(从 2012 年初开始，所有主要的 ext4 和 jbd2 数据结构都加入了元数据校验和。相关的特性标志是 metadata_csum。所需的校验和算法在超级块中指明，不过截至 2012 年 10 月，唯一受支持的算法是 crc32c。一些数据结构没有空间容纳完整的 32 位校验和，因此只存储低 16 位。启用 64bit 特性会增大这些数据结构的大小，以便为许多数据结构存储完整的 32 位校验和。但是，现有的 32 位文件系统无法扩展为启用 64bit 模式，至少没有实验性的 resize2fs 补丁是无法做到的。)

Existing filesystems can have checksumming added by running
``tune2fs -O metadata_csum`` against the underlying device. If tune2fs
encounters directory blocks that lack sufficient empty space to add a
checksum, it will request that you run ``e2fsck -D`` to have the
directories rebuilt with checksums. This has the added benefit of
removing slack space from the directory files and rebalancing the htree
indexes. If you _ignore_ this step, your directories will not be
protected by a checksum!
(通过对底层设备运行 ``tune2fs -O metadata_csum``，可以为现有文件系统添加校验和。如果 tune2fs 遇到缺乏足够空白空间来添加校验和的目录块，它会要求你运行 ``e2fsck -D`` 来重建带校验和的目录。这还有一个额外好处，即移除目录文件中的松弛空间并重新平衡 htree 索引。如果你 _忽略_ 这一步，你的目录将不会受到校验和的保护！)

The following table describes the data elements that go into each type
of checksum. The checksum function is whatever the superblock describes
(crc32c as of October 2013) unless noted otherwise.
(下表描述了每种类型的校验和所包含的数据元素。除非另有说明，校验和函数就是超级块所描述的函数(截至 2013 年 10 月为 crc32c)。)

.. list-table::
   :widths: 20 8 50
   :header-rows: 1

   * - Metadata (元数据)
     - Length (长度)
     - Ingredients (组成要素)
   * - Superblock
     - __le32
     - The entire superblock up to the checksum field. The UUID lives inside
       the superblock. (直到校验和字段为止的整个超级块。UUID 位于超级块内部。)
   * - MMP
     - __le32
     - UUID + the entire MMP block up to the checksum field. (UUID + 直到校验和字段为止的整个 MMP 块。)
   * - Extended Attributes
     - __le32
     - UUID + the entire extended attribute block. The checksum field is set to
       zero. (UUID + 整个扩展属性块。校验和字段被设为零。)
   * - Directory Entries
     - __le32
     - UUID + inode number + inode generation + the directory block up to the
       fake entry enclosing the checksum field. (UUID + inode 编号 + inode 代数 + 直到包含校验和字段的伪条目为止的目录块。)
   * - HTREE Nodes
     - __le32
     - UUID + inode number + inode generation + all valid extents + HTREE tail.
       The checksum field is set to zero. (UUID + inode 编号 + inode 代数 + 所有有效 extent + HTREE 尾部。校验和字段被设为零。)
   * - Extents
     - __le32
     - UUID + inode number + inode generation + the entire extent block up to
       the checksum field. (UUID + inode 编号 + inode 代数 + 直到校验和字段为止的整个 extent 块。)
   * - Bitmaps
     - __le32 or __le16
     - UUID + the entire bitmap. Checksums are stored in the group descriptor,
       and truncated if the group descriptor size is 32 bytes (i.e. ^64bit) (UUID + 整个位图。校验和存储在组描述符中，如果组描述符大小为 32 字节(即 ^64bit)则被截断)
   * - Inodes
     - __le32
     - UUID + inode number + inode generation + the entire inode. The checksum
       field is set to zero. Each inode has its own checksum. (UUID + inode 编号 + inode 代数 + 整个 inode。校验和字段被设为零。每个 inode 有自己的校验和。)
   * - Group Descriptors
     - __le16
     - If metadata_csum, then UUID + group number + the entire descriptor;
       else if gdt_csum, then crc16(UUID + group number + the entire
       descriptor). In all cases, only the lower 16 bits are stored. (如果启用 metadata_csum，则为 UUID + 组编号 + 整个描述符；否则如果启用 gdt_csum，则为 crc16(UUID + 组编号 + 整个描述符)。所有情况下都只存储低 16 位。)

