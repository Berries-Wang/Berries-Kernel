.. SPDX-License-Identifier: GPL-2.0

Extended Attributes
(扩展属性)
-------------------

Extended attributes (xattrs) are typically stored in a separate data
block on the disk and referenced from inodes via ``inode.i_file_acl*``.
The first use of extended attributes seems to have been for storing file
ACLs and other security data (selinux). With the ``user_xattr`` mount
option it is possible for users to store extended attributes so long as
all attribute names begin with “user”; this restriction seems to have
disappeared as of Linux 3.0.
(扩展属性(xattr)通常存储在磁盘上单独的数据块中，并通过 ``inode.i_file_acl*`` 从 inode 引用。扩展属性最早的用途似乎是存储文件 ACL 和其他安全数据(selinux)。使用 ``user_xattr`` 挂载选项，用户可以存储扩展属性，只要所有属性名都以 “user” 开头即可；这一限制在 Linux 3.0 之后似乎已经消失。)

There are two places where extended attributes can be found. The first
place is between the end of each inode entry and the beginning of the
next inode entry. For example, if inode.i_extra_isize = 28 and
sb.inode_size = 256, then there are 256 - (128 + 28) = 100 bytes
available for in-inode extended attribute storage. The second place
where extended attributes can be found is in the block pointed to by
``inode.i_file_acl``. As of Linux 3.11, it is not possible for this
block to contain a pointer to a second extended attribute block (or even
the remaining blocks of a cluster). In theory it is possible for each
attribute's value to be stored in a separate data block, though as of
Linux 3.11 the code does not permit this.
(有两个地方可以找到扩展属性。第一个地方是每个 inode 表项的结尾与下一个 inode 表项的开头之间。例如，如果 inode.i_extra_isize = 28 且 sb.inode_size = 256，那么有 256 - (128 + 28) = 100 字节可用于 inode 内扩展属性存储。第二个可以找到扩展属性的地方是 ``inode.i_file_acl`` 所指向的块。自 Linux 3.11 起，这个块不可能包含指向第二个扩展属性块的指针(甚至是簇中剩余的块)。理论上每个属性的值可以存储在单独的数据块中，不过截至 Linux 3.11，代码不允许这样做。)

Keys are generally assumed to be ASCIIZ strings, whereas values can be
strings or binary data.
(键通常被假定为 ASCIIZ 字符串，而值可以是字符串或二进制数据。)

Extended attributes, when stored after the inode, have a header
``ext4_xattr_ibody_header`` that is 4 bytes long:
(扩展属性在存储于 inode 之后时，拥有一个长度为 4 字节的头部 ``ext4_xattr_ibody_header``：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Type
       (类型)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le32
     - h_magic
     - Magic number for identification, 0xEA020000. This value is set by the
       Linux driver, though e2fsprogs doesn't seem to check it(?)
       (用于识别的魔数，0xEA020000。该值由 Linux 驱动程序设置，不过 e2fsprogs 似乎并不检查它(?))

The beginning of an extended attribute block is in
``struct ext4_xattr_header``, which is 32 bytes long:
(扩展属性块的开头是 ``struct ext4_xattr_header``，其长度为 32 字节：)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Type
       (类型)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __le32
     - h_magic
     - Magic number for identification, 0xEA020000.
       (用于识别的魔数，0xEA020000。)
   * - 0x4
     - __le32
     - h_refcount
     - Reference count.
       (引用计数。)
   * - 0x8
     - __le32
     - h_blocks
     - Number of disk blocks used.
       (使用的磁盘块数。)
   * - 0xC
     - __le32
     - h_hash
     - Hash value of all attributes.
       (所有属性的哈希值。)
   * - 0x10
     - __le32
     - h_checksum
     - Checksum of the extended attribute block.
       (扩展属性块的校验和。)
   * - 0x14
     - __u32
     - h_reserved[3]
     - Zero.
       (零。)

The checksum is calculated against the FS UUID, the 64-bit block number
of the extended attribute block, and the entire block (header +
entries).
(校验和是针对 FS UUID、扩展属性块的 64 位块号，以及整个块(头部 + 表项)来计算的。)

Following the ``struct ext4_xattr_header`` or
``struct ext4_xattr_ibody_header`` is an array of
``struct ext4_xattr_entry``; each of these entries is at least 16 bytes
long. When stored in an external block, the ``struct ext4_xattr_entry``
entries must be stored in sorted order. The sort order is
``e_name_index``, then ``e_name_len``, and finally ``e_name``.
Attributes stored inside an inode do not need be stored in sorted order.
(紧跟在 ``struct ext4_xattr_header`` 或 ``struct ext4_xattr_ibody_header`` 之后的是一个 ``struct ext4_xattr_entry`` 数组；这些表项中的每一个都至少长 16 字节。当存储在外部块中时，``struct ext4_xattr_entry`` 表项必须按排序后的顺序存储。排序顺序是 ``e_name_index``，然后是 ``e_name_len``，最后是 ``e_name``。存储在 inode 内部的属性不需要按排序顺序存储。)

.. list-table::
   :widths: 8 8 24 40
   :header-rows: 1

   * - Offset
       (偏移量)
     - Type
       (类型)
     - Name
       (名称)
     - Description
       (描述)
   * - 0x0
     - __u8
     - e_name_len
     - Length of name.
       (名称的长度。)
   * - 0x1
     - __u8
     - e_name_index
     - Attribute name index. There is a discussion of this below.
       (属性名称索引。下文有关于此的讨论。)
   * - 0x2
     - __le16
     - e_value_offs
     - Location of this attribute's value on the disk block where it is stored.
       Multiple attributes can share the same value. For an inode attribute
       this value is relative to the start of the first entry; for a block this
       value is relative to the start of the block (i.e. the header).
       (此属性的值在其所在磁盘块上的位置。多个属性可以共享同一个值。对于 inode 属性，此值相对于第一个表项的起始处；对于块，此值相对于块的起始处(即头部)。)
   * - 0x4
     - __le32
     - e_value_inum
     - The inode where the value is stored. Zero indicates the value is in the
       same block as this entry. This field is only used if the
       INCOMPAT_EA_INODE feature is enabled.
       (存储该值的 inode。零表示该值与此表项在同一个块中。仅当启用了 INCOMPAT_EA_INODE 特性时才使用此字段。)
   * - 0x8
     - __le32
     - e_value_size
     - Length of attribute value.
       (属性值的长度。)
   * - 0xC
     - __le32
     - e_hash
     - Hash value of attribute name and attribute value. The kernel doesn't
       update the hash for in-inode attributes, so for that case this value
       must be zero, because e2fsck validates any non-zero hash regardless of
       where the xattr lives.
       (属性名和属性值的哈希值。内核不会为 inode 内属性更新哈希，因此对于这种情况此值必须为零，因为 e2fsck 会验证任何非零哈希，无论 xattr 位于何处。)
   * - 0x10
     - char
     - e_name[e_name_len]
     - Attribute name. Does not include trailing NULL.
       (属性名。不包含结尾的 NULL。)

Attribute values can follow the end of the entry table. There appears to
be a requirement that they be aligned to 4-byte boundaries. The values
are stored starting at the end of the block and grow towards the
xattr_header/xattr_entry table. When the two collide, the overflow is
put into a separate disk block. If the disk block fills up, the
filesystem returns -ENOSPC.
(属性值可以跟在表项表的末尾之后。似乎有一个要求：它们必须按 4 字节边界对齐。值从块的末尾开始存储，并向 xattr_header/xattr_entry 表的方向增长。当两者相遇时，溢出部分会被放入一个单独的磁盘块。如果磁盘块被填满，文件系统将返回 -ENOSPC。)

The first four fields of the ``ext4_xattr_entry`` are set to zero to
mark the end of the key list.
(``ext4_xattr_entry`` 的前四个字段被设置为零，以标记键列表的结束。)

Attribute Name Indices
(属性名称索引)
~~~~~~~~~~~~~~~~~~~~~~

Logically speaking, extended attributes are a series of key=value pairs.
The keys are assumed to be NULL-terminated strings. To reduce the amount
of on-disk space that the keys consume, the beginning of the key string
is matched against the attribute name index. If a match is found, the
attribute name index field is set, and matching string is removed from
the key name. Here is a map of name index values to key prefixes:
(从逻辑上讲，扩展属性是一系列 key=value 对。键被假定为以 NULL 结尾的字符串。为了减少键在磁盘上占用的空间，键字符串的开头会与属性名称索引进行匹配。如果找到匹配，则设置属性名称索引字段，并将匹配到的字符串从键名中移除。以下是名称索引值到键前缀的映射：)

.. list-table::
   :widths: 16 64
   :header-rows: 1

   * - Name Index
       (名称索引)
     - Key Prefix
       (键前缀)
   * - 0
     - (no prefix)
       (无前缀)
   * - 1
     - “user.”
   * - 2
     - “system.posix_acl_access”
   * - 3
     - “system.posix_acl_default”
   * - 4
     - “trusted.”
   * - 6
     - “security.”
   * - 7
     - “system.” (inline_data only?)
       (仅 inline_data？)
   * - 8
     - “system.richacl” (SuSE kernels only?)
       (仅 SuSE 内核？)

For example, if the attribute key is “user.fubar”, the attribute name
index is set to 1 and the “fubar” name is recorded on disk.
(例如，如果属性键是 “user.fubar”，则属性名称索引被设置为 1，并且 “fubar” 这个名称会被记录在磁盘上。)

POSIX ACLs
(POSIX ACL)
~~~~~~~~~~

POSIX ACLs are stored in a reduced version of the Linux kernel (and
libacl's) internal ACL format. The key difference is that the version
number is different (1) and the ``e_id`` field is only stored for named
user and group ACLs.
(POSIX ACL 以 Linux 内核(以及 libacl)内部 ACL 格式的精简版本存储。关键区别在于版本号不同(1)，并且 ``e_id`` 字段仅对命名用户和命名组的 ACL 存储。)
