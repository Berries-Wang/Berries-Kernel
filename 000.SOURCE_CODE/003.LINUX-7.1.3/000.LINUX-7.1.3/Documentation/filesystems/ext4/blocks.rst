.. SPDX-License-Identifier: GPL-2.0

Blocks
(块)
------

ext4 allocates storage space in units of “blocks”. A block is a group of
sectors between 1KiB and 64KiB, and the number of sectors must be an
integral power of 2. Blocks are in turn grouped into larger units called
block groups. Block size is specified at mkfs time and typically is
4KiB. You may experience mounting problems if block size is greater than
page size (i.e. 64KiB blocks on a i386 which only has 4KiB memory
pages). By default a filesystem can contain 2^32 blocks; if the '64bit'
feature is enabled, then a filesystem can have 2^64 blocks. The location
of structures is stored in terms of the block number the structure lives
in and not the absolute offset on disk.
(ext4 以“块(block)”为单位分配存储空间。一个块由 1KiB 到 64KiB 之间的若干扇区组成，且扇区数必须是 2 的整数次幂。块又被组成更大的单元，称为块组(block group)。块大小在 mkfs 时指定，通常为 4KiB。如果块大小大于页大小，你可能会遇到挂载问题(例如在只有 4KiB 内存页的 i386 上使用 64KiB 块)。默认情况下，一个文件系统可以包含 2^32 个块；如果启用了 '64bit' 特性，则文件系统可以拥有 2^64 个块。结构的位置以该结构所在块的块号来存储，而不是以磁盘上的绝对偏移来存储。)

For 32-bit filesystems, limits are as follows:
(对于 32 位文件系统，限制如下：)

.. list-table::
   :widths: 1 1 1 1 1
   :header-rows: 1

   * - Item(条目)
     - 1KiB
     - 2KiB
     - 4KiB
     - 64KiB
   * - Blocks(块数)
     - 2^32
     - 2^32
     - 2^32
     - 2^32
   * - Inodes(inode 数)
     - 2^32
     - 2^32
     - 2^32
     - 2^32
   * - File System Size(文件系统大小)
     - 4TiB
     - 8TiB
     - 16TiB
     - 256TiB
   * - Blocks Per Block Group(每块组的块数)
     - 8,192
     - 16,384
     - 32,768
     - 524,288
   * - Inodes Per Block Group(每块组的 inode 数)
     - 8,192
     - 16,384
     - 32,768
     - 524,288
   * - Block Group Size(块组大小)
     - 8MiB
     - 32MiB
     - 128MiB
     - 32GiB
   * - Blocks Per File, Extents(每文件块数，extent 方式)
     - 2^32
     - 2^32
     - 2^32
     - 2^32
   * - Blocks Per File, Block Maps(每文件块数，块映射方式)
     - 16,843,020
     - 134,480,396
     - 1,074,791,436
     - 4,398,314,962,956 (really 2^32 due to field size limitations)(实际为 2^32，因字段大小限制)
   * - File Size, Extents(文件大小，extent 方式)
     - 4TiB
     - 8TiB
     - 16TiB
     - 256TiB
   * - File Size, Block Maps(文件大小，块映射方式)
     - 16GiB
     - 256GiB
     - 4TiB
     - 256TiB

For 64-bit filesystems, limits are as follows:
(对于 64 位文件系统，限制如下：)

.. list-table::
   :widths: 1 1 1 1 1
   :header-rows: 1

   * - Item(条目)
     - 1KiB
     - 2KiB
     - 4KiB
     - 64KiB
   * - Blocks(块数)
     - 2^64
     - 2^64
     - 2^64
     - 2^64
   * - Inodes(inode 数)
     - 2^32
     - 2^32
     - 2^32
     - 2^32
   * - File System Size(文件系统大小)
     - 16ZiB
     - 32ZiB
     - 64ZiB
     - 1YiB
   * - Blocks Per Block Group(每块组的块数)
     - 8,192
     - 16,384
     - 32,768
     - 524,288
   * - Inodes Per Block Group(每块组的 inode 数)
     - 8,192
     - 16,384
     - 32,768
     - 524,288
   * - Block Group Size(块组大小)
     - 8MiB
     - 32MiB
     - 128MiB
     - 32GiB
   * - Blocks Per File, Extents(每文件块数，extent 方式)
     - 2^32
     - 2^32
     - 2^32
     - 2^32
   * - Blocks Per File, Block Maps(每文件块数，块映射方式)
     - 16,843,020
     - 134,480,396
     - 1,074,791,436
     - 4,398,314,962,956 (really 2^32 due to field size limitations)(实际为 2^32，因字段大小限制)
   * - File Size, Extents(文件大小，extent 方式)
     - 4TiB
     - 8TiB
     - 16TiB
     - 256TiB
   * - File Size, Block Maps(文件大小，块映射方式)
     - 16GiB
     - 256GiB
     - 4TiB
     - 256TiB

Note: Files not using extents (i.e. files using block maps) must be
placed within the first 2^32 blocks of a filesystem. Files with extents
must be placed within the first 2^48 blocks of a filesystem. It's not
clear what happens with larger filesystems.
(注意：不使用 extent 的文件(即使用块映射的文件)必须放置在文件系统的前 2^32 个块之内。使用 extent 的文件必须放置在文件系统的前 2^48 个块之内。对于更大的文件系统会发生什么，目前尚不清楚。)
