.. SPDX-License-Identifier: GPL-2.0

Special inodes
(特殊 inode)
--------------

ext4 reserves some inode for special features, as follows:
(ext4 为特殊功能保留了一些 inode，如下所示：)

.. list-table::
   :widths: 6 70
   :header-rows: 1

   * - inode Number (inode 编号)
     - Purpose (用途)
   * - 0
     - Doesn't exist; there is no inode 0. (不存在；没有 inode 0。)
   * - 1
     - List of defective blocks. (坏块列表。)
   * - 2
     - Root directory. (根目录。)
   * - 3
     - User quota. (用户配额。)
   * - 4
     - Group quota. (组配额。)
   * - 5
     - Boot loader. (引导加载程序。)
   * - 6
     - Undelete directory. (反删除目录。)
   * - 7
     - Reserved group descriptors inode. (“resize inode”) (保留的组描述符 inode。(“resize inode”))
   * - 8
     - Journal inode. (日志 inode。)
   * - 9
     - The “exclude” inode, for snapshots(?) (“exclude” inode，用于快照(?))
   * - 10
     - Replica inode, used for some non-upstream feature? (副本 inode，用于某些未进入上游的功能？)
   * - 11
     - Traditional first non-reserved inode. Usually this is the lost+found directory. See s_first_ino in the superblock. (传统的第一个非保留 inode。通常是 lost+found 目录。参见超级块中的 s_first_ino。)

Note that there are also some inodes allocated from non-reserved inode numbers
for other filesystem features which are not referenced from standard directory
hierarchy. These are generally reference from the superblock. They are:
(请注意，还有一些 inode 是从非保留 inode 编号中分配的，用于其他文件系统功能，它们不被标准目录层次结构引用。这些 inode 通常由超级块引用。它们是：)

.. list-table::
   :widths: 20 50
   :header-rows: 1

   * - Superblock field (超级块字段)
     - Description (描述)

   * - s_lpf_ino
     - Inode number of lost+found directory. (lost+found 目录的 inode 编号。)
   * - s_prj_quota_inum
     - Inode number of quota file tracking project quotas (跟踪项目配额的配额文件的 inode 编号)
   * - s_orphan_file_inum
     - Inode number of file tracking orphan inodes. (跟踪孤儿 inode 的文件的 inode 编号。)
