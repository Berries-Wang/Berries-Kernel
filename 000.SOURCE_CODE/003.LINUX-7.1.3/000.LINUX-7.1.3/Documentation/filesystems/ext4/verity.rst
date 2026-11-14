.. SPDX-License-Identifier: GPL-2.0

Verity files
(verity 文件)
------------

ext4 supports fs-verity, which is a filesystem feature that provides
Merkle tree based hashing for individual readonly files.  Most of
fs-verity is common to all filesystems that support it; see
:ref:`Documentation/filesystems/fsverity.rst <fsverity>` for the
fs-verity documentation.  However, the on-disk layout of the verity
metadata is filesystem-specific.  On ext4, the verity metadata is
stored after the end of the file data itself, in the following format:
(ext4 支持 fs-verity，这是一种为单个只读文件提供基于 Merkle 树的哈希的文件系统特性。fs-verity 的大部分内容对所有支持它的文件系统都是通用的；fs-verity 文档见 :ref:`Documentation/filesystems/fsverity.rst <fsverity>`。不过，verity 元数据的磁盘上布局是文件系统特定的。在 ext4 上，verity 元数据存储在文件数据本身结束之后，格式如下：)

- Zero-padding to the next 65536-byte boundary.  This padding need not
  actually be allocated on-disk, i.e. it may be a hole.
  (填充零直至下一个 65536 字节边界。这些填充实际上不必在磁盘上分配，即它可以是一个空洞。)

- The Merkle tree, as documented in
  :ref:`Documentation/filesystems/fsverity.rst
  <fsverity_merkle_tree>`, with the tree levels stored in order from
  root to leaf, and the tree blocks within each level stored in their
  natural order.
  (Merkle 树，如 :ref:`Documentation/filesystems/fsverity.rst
  <fsverity_merkle_tree>` 中所述，树层级按从根到叶的顺序存储，每层内的树块按其自然顺序存储。)

- Zero-padding to the next filesystem block boundary.
  (填充零直至下一个文件系统块边界。)

- The verity descriptor, as documented in
  :ref:`Documentation/filesystems/fsverity.rst <fsverity_descriptor>`,
  with optionally appended signature blob.
  (verity 描述符，如 :ref:`Documentation/filesystems/fsverity.rst <fsverity_descriptor>` 中所述，后面可选地附加签名 blob。)

- Zero-padding to the next offset that is 4 bytes before a filesystem
  block boundary.
  (填充零直至下一个偏移量，该偏移量位于文件系统块边界之前 4 字节处。)

- The size of the verity descriptor in bytes, as a 4-byte little
  endian integer.
  (verity 描述符的大小(以字节为单位)，为一个 4 字节小端整数。)

Verity inodes have EXT4_VERITY_FL set, and they must use extents, i.e.
EXT4_EXTENTS_FL must be set and EXT4_INLINE_DATA_FL must be clear.
They can have EXT4_ENCRYPT_FL set, in which case the verity metadata
is encrypted as well as the data itself.
(verity inode 设置了 EXT4_VERITY_FL，并且它们必须使用 extent，即必须设置 EXT4_EXTENTS_FL 且必须清除 EXT4_INLINE_DATA_FL。它们可以设置 EXT4_ENCRYPT_FL，在这种情况下，verity 元数据与数据本身一样被加密。)

Verity files cannot have blocks allocated past the end of the verity
metadata.
(verity 文件不能在 verity 元数据结束之后分配块。)

Verity and DAX are not compatible and attempts to set both of these flags
on a file will fail.
(verity 与 DAX 不兼容，尝试在同一个文件上设置这两个标志将会失败。)
