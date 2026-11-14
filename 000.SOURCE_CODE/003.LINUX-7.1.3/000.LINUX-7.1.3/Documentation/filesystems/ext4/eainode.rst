.. SPDX-License-Identifier: GPL-2.0

Large Extended Attribute Values
(大扩展属性值)
-------------------------------

To enable ext4 to store extended attribute values that do not fit in the
inode or in the single extended attribute block attached to an inode,
the EA_INODE feature allows us to store the value in the data blocks of
a regular file inode. This “EA inode” is linked only from the extended
attribute name index and must not appear in a directory entry. The
inode's i_atime field is used to store a checksum of the xattr value;
and i_ctime/i_version store a 64-bit reference count, which enables
sharing of large xattr values between multiple owning inodes. For
backward compatibility with older versions of this feature, the
i_mtime/i_generation *may* store a back-reference to the inode number
and i_generation of the **one** owning inode (in cases where the EA
inode is not referenced by multiple inodes) to verify that the EA inode
is the correct one being accessed.
(为了使 ext4 能够存储不适合放入 inode 或附加到 inode 的单个扩展属性块中的扩展属性值，EA_INODE 特性允许我们将该值存储在常规文件 inode 的数据块中。这种“EA inode”仅从扩展属性名称索引链接，不得出现在目录项中。inode 的 i_atime 字段用于存储 xattr 值的校验和；而 i_ctime/i_version 存储一个 64 位引用计数，这使得可以在多个属主 inode 之间共享大型 xattr 值。为了与该特性的旧版本向后兼容，i_mtime/i_generation *可以* 存储对**唯一**属主 inode 的编号和 i_generation 的反向引用(在 EA inode 未被多个 inode 引用的情况下)，以验证正在访问的 EA inode 是正确的那个。)
