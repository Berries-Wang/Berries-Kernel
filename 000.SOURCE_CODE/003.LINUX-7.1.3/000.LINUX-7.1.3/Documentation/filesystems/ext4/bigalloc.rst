.. SPDX-License-Identifier: GPL-2.0

Bigalloc
(bigalloc)
--------

At the moment, the default size of a block is 4KiB, which is a commonly
supported page size on most MMU-capable hardware. This is fortunate, as
ext4 code is not prepared to handle the case where the block size
exceeds the page size. However, for a filesystem of mostly huge files,
it is desirable to be able to allocate disk blocks in units of multiple
blocks to reduce both fragmentation and metadata overhead. The
bigalloc feature provides exactly this ability.
(目前，块的默认大小是 4KiB，这是大多数支持 MMU 的硬件普遍支持的页大小。这是幸运的，因为 ext4 代码尚未准备好处理块大小超过页大小的情况。不过，对于一个主要由大文件组成的文件系统，希望能够以多个块为单位分配磁盘块，以同时减少碎片和元数据开销。bigalloc 特性恰好提供了这种能力。)

The bigalloc feature (EXT4_FEATURE_RO_COMPAT_BIGALLOC) changes ext4 to
use clustered allocation, so that each bit in the ext4 block allocation
bitmap addresses a power of two number of blocks. For example, if the
file system is mainly going to be storing large files in the 4-32
megabyte range, it might make sense to set a cluster size of 1 megabyte.
This means that each bit in the block allocation bitmap now addresses
256 4k blocks. This shrinks the total size of the block allocation
bitmaps for a 2T file system from 64 megabytes to 256 kilobytes. It also
means that a block group addresses 32 gigabytes instead of 128 megabytes,
also shrinking the amount of file system overhead for metadata.
(bigalloc 特性(EXT4_FEATURE_RO_COMPAT_BIGALLOC)将 ext4 改为使用簇分配，使得 ext4 块分配位图中的每一位对应二的幂个块。例如，如果文件系统主要存储 4-32 MB 范围的大文件，那么将簇大小设置为 1 MB 可能是合理的。这意味着块分配位图中的每一位现在对应 256 个 4k 块。这将 2T 文件系统的块分配位图总大小从 64 MB 缩减到 256 KB。这也意味着一个块组对应 32 GB 而不是 128 MB，同时减少了文件系统的元数据开销。)

The administrator can set a block cluster size at mkfs time (which is
stored in the s_log_cluster_size field in the superblock); from then
on, the block bitmaps track clusters, not individual blocks. This means
that block groups can be several gigabytes in size (instead of just
128MiB); however, the minimum allocation unit becomes a cluster, not a
block, even for directories. TaoBao had a patchset to extend the “use
units of clusters instead of blocks” to the extent tree, though it is
not clear where those patches went-- they eventually morphed into
“extent tree v2” but that code has not landed as of May 2015.
(管理员可以在 mkfs 时设置块簇大小(存储在超级块的 s_log_cluster_size 字段中)；从那时起，块位图跟踪的是簇而不是单个块。这意味着块组的大小可以达到数 GB(而不仅仅是 128MiB)；但是，最小分配单位变成了簇而不是块，即使对目录也是如此。淘宝曾有一个补丁集将“使用簇而不是块作为单位”扩展到 extent 树，不过这些补丁的去向并不清楚 —— 它们最终演变成了“extent tree v2”，但截至 2015 年 5 月，该代码尚未合入。)

