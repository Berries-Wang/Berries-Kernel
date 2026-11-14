.. SPDX-License-Identifier: GPL-2.0

======================================
EROFS - Enhanced Read-Only File System
======================================

(EROFS - 增强型只读文件系统)

Overview
(概述)
========

EROFS filesystem stands for Enhanced Read-Only File System.  It aims to form a
generic read-only filesystem solution for various read-only use cases instead
of just focusing on storage space saving without considering any side effects
of runtime performance.
(EROFS 文件系统代表 Enhanced Read-Only File System(增强型只读文件系统)。它旨在为各种只读用例形成一种通用的只读文件系统解决方案，而不是只关注节省存储空间而不考虑运行时性能方面的任何副作用。)

It is designed to meet the needs of flexibility, feature extendability and user
payload friendly, etc.  Apart from those, it is still kept as a simple
random-access friendly high-performance filesystem to get rid of unneeded I/O
amplification and memory-resident overhead compared to similar approaches.
(它旨在满足灵活性、特性可扩展性和对用户载荷友好等需求。除此之外，与类似方案相比，它仍然保持为一个简单的、对随机访问友好的高性能文件系统，以摆脱不必要的 I/O 放大和内存驻留开销。)

It is implemented to be a better choice for the following scenarios:
(它的实现使其成为以下场景的更好选择：)

 - read-only storage media or
   (只读存储介质，或者)

 - part of a fully trusted read-only solution, which means it needs to be
   immutable and bit-for-bit identical to the official golden image for
   their releases due to security or other considerations and
   (作为完全可信的只读解决方案的一部分，这意味着出于安全或其他方面的考虑，它需要是不可变的，并且与其发布版本的官方黄金镜像逐位一致；以及)

 - hope to minimize extra storage space with guaranteed end-to-end performance
   by using compact layout, transparent file compression and direct access,
   especially for those embedded devices with limited memory and high-density
   hosts with numerous containers.
   (希望通过使用紧凑布局、透明文件压缩和直接访问，在保证端到端性能的同时尽量减少额外的存储空间，尤其是对于内存有限的嵌入式设备和拥有大量容器的高密度主机。)

Here are the main features of EROFS:
(以下是 EROFS 的主要特性：)

 - Little endian on-disk design;
   (小端在盘设计；)

 - Block-based distribution and file-based distribution over fscache are
   supported;
   (支持基于块的分布以及通过 fscache 的基于文件的分布；)

 - Support multiple devices to refer to external blobs, which can be used
   for container images;
   (支持多个设备引用外部 blob，这可用于容器镜像；)

 - 32-bit block addresses for each device, therefore 16TiB address space at
   most with 4KiB block size for now;
   (每个设备 32 位块地址，因此目前使用 4KiB 块大小时地址空间最大为 16TiB；)

 - Two inode layouts for different requirements:
   (针对不同需求的两种 inode 布局：)

   =====================  ============  ======================================
                         compact (v1) (紧凑)  extended (v2) (扩展)
   =====================  ============  ======================================
   Inode metadata size (inode 元数据大小)    32 bytes      64 bytes
   Max file size (最大文件大小)          4 GiB         16 EiB (also limited by max. vol size) (同时受最大卷大小限制)
   Max uids/gids (最大 uid/gid 数)          65536         4294967296
   Per-inode timestamp (每 inode 时间戳)    no            yes (64 + 32-bit timestamp)
   Max hardlinks (最大硬链接数)          65536         4294967296
   Metadata reserved (元数据保留)      8 bytes       18 bytes
   =====================  ============  ======================================

 - Support extended attributes as an option;
   (支持扩展属性作为可选项；)

 - Support a bloom filter that speeds up negative extended attribute lookups;
   (支持加速扩展属性否定查找的布隆过滤器；)

 - Support POSIX.1e ACLs by using extended attributes;
   (通过使用扩展属性支持 POSIX.1e ACL；)

 - Support transparent data compression as an option:
   LZ4, MicroLZMA, DEFLATE and Zstandard algorithms can be used on a per-file
   basis; In addition, inplace decompression is also supported to avoid bounce
   compressed buffers and unnecessary page cache thrashing.
   (支持透明数据压缩作为可选项：LZ4、MicroLZMA、DEFLATE 和 Zstandard 算法可以按文件为基础使用；此外，还支持就地解压，以避免弹跳式压缩缓冲区和不必要的页缓存抖动。)

 - Support chunk-based data deduplication and rolling-hash compressed data
   deduplication;
   (支持基于块(chunk)的数据去重和基于滚动哈希的压缩数据去重；)

 - Support tailpacking inline compared to byte-addressed unaligned metadata
   or smaller block size alternatives;
   (相比于按字节寻址的非对齐元数据或更小块大小的替代方案，支持尾部打包内联；)

 - Support merging tail-end data into a special inode as fragments.
   (支持将尾端数据合并到一个特殊 inode 中作为片段(fragment)。)

 - Support large folios to make use of THPs (Transparent Hugepages);
   (支持大 folio 以利用 THP(透明大页)；)

 - Support direct I/O on uncompressed files to avoid double caching for loop
   devices;
   (支持对未压缩文件的直接 I/O，以避免回环设备的双重缓存；)

 - Support FSDAX on uncompressed images for secure containers and ramdisks in
   order to get rid of unnecessary page cache.
   (支持在未压缩镜像上使用 FSDAX，用于安全容器和 ramdisk，以摆脱不必要的页缓存。)

 - Support file-based on-demand loading with the Fscache infrastructure.
   (支持通过 Fscache 基础设施进行基于文件的按需加载。)

The following git tree provides the file system user-space tools under
development, such as a formatting tool (mkfs.erofs), an on-disk consistency &
compatibility checking tool (fsck.erofs), and a debugging tool (dump.erofs):
(以下 git 树提供了开发中的文件系统用户空间工具，例如格式化工具(mkfs.erofs)、在盘一致性及兼容性检查工具(fsck.erofs)和调试工具(dump.erofs)：)

- git://git.kernel.org/pub/scm/linux/kernel/git/xiang/erofs-utils.git

For more information, please also refer to the documentation site:
(更多信息，另请参阅文档站点：)

- https://erofs.docs.kernel.org

Bugs and patches are welcome, please kindly help us and send to the following
linux-erofs mailing list:
(欢迎缺陷报告和补丁，请帮助我们并发送到以下 linux-erofs 邮件列表：)

- linux-erofs mailing list   <linux-erofs@lists.ozlabs.org>
  (linux-erofs 邮件列表)

Mount options
(挂载选项)
=============

===================    =========================================================
(no)user_xattr         Setup Extended User Attributes. Note: xattr is enabled
                       by default if CONFIG_EROFS_FS_XATTR is selected.
                       (设置扩展用户属性。注意：如果选择了 CONFIG_EROFS_FS_XATTR，xattr 默认启用。)
(no)acl                Setup POSIX Access Control List. Note: acl is enabled
                       by default if CONFIG_EROFS_FS_POSIX_ACL is selected.
                       (设置 POSIX 访问控制列表。注意：如果选择了 CONFIG_EROFS_FS_POSIX_ACL，acl 默认启用。)
cache_strategy=%s      Select a strategy for cached decompression from now on:
                       (从此刻起为缓存解压选择一种策略：)

		       ==========  =============================================
                         disabled  In-place I/O decompression only;
                                   (仅就地 I/O 解压；)
                        readahead  Cache the last incomplete compressed physical
                                   cluster for further reading. It still does
                                   in-place I/O decompression for the rest
                                   compressed physical clusters;
                                   (缓存最后一个不完整的压缩物理簇以供后续读取。对其余压缩物理簇仍进行就地 I/O 解压；)
                       readaround  Cache both ends of incomplete compressed
                                   physical clusters for further reading.
                                   It still does in-place I/O decompression
                                   for the rest compressed physical clusters.
                                   (缓存不完整压缩物理簇的两端以供后续读取。对其余压缩物理簇仍进行就地 I/O 解压。)
		       ==========  =============================================
dax={always,never}     Use direct access (no page cache).  See
                       Documentation/filesystems/dax.rst.
                       (使用直接访问(无页缓存)。参见 Documentation/filesystems/dax.rst。)
dax                    A legacy option which is an alias for ``dax=always``.
                       (一个遗留选项，是 ``dax=always`` 的别名。)
device=%s              Specify a path to an extra device to be used together.
                       (指定一个要一起使用的额外设备的路径。)
directio               (For file-backed mounts) Use direct I/O to access backing
                       files, and asynchronous I/O will be enabled if supported.
                       ((对于文件后端的挂载)使用直接 I/O 访问后端文件，如果支持，还将启用异步 I/O。)
fsid=%s                Specify a filesystem image ID for Fscache back-end.
                       (为 Fscache 后端指定一个文件系统镜像 ID。)
domain_id=%s           Specify a trusted domain ID for fscache mode so that
                       different images with the same blobs, identified by blob IDs,
                       can share storage within the same trusted domain.
                       Also used for different filesystems with inode page sharing
                       enabled to share page cache within the trusted domain.
                       (为 fscache 模式指定一个受信任域 ID，使得由 blob ID 标识的、具有相同 blob 的不同镜像可以在同一受信任域内共享存储。对于启用了 inode 页共享的不同文件系统，它还用于在该受信任域内共享页缓存。)
fsoffset=%llu          Specify block-aligned filesystem offset for the primary device.
                       (为主设备指定块对齐的文件系统偏移量。)
inode_share            Enable inode page sharing for this filesystem.  Inodes with
                       identical content within the same domain ID can share the
                       page cache.
                       (为此文件系统启用 inode 页共享。同一域 ID 内内容相同的 inode 可以共享页缓存。)
===================    =========================================================

Sysfs Entries
(Sysfs 条目)
=============

Information about mounted erofs file systems can be found in /sys/fs/erofs.
Each mounted filesystem will have a directory in /sys/fs/erofs based on its
device name (i.e., /sys/fs/erofs/sda).
(see also Documentation/ABI/testing/sysfs-fs-erofs)
(关于已挂载 erofs 文件系统的信息可以在 /sys/fs/erofs 中找到。每个已挂载的文件系统都会根据其设备名在 /sys/fs/erofs 中有一个目录(即 /sys/fs/erofs/sda)。(另见 Documentation/ABI/testing/sysfs-fs-erofs))

On-disk details
(在盘细节)
===============

Summary
(摘要)
-------
Different from other read-only file systems, an EROFS volume is designed
to be as simple as possible::

                                |-> aligned with the block size
   ____________________________________________________________
  | |SB| | ... | Metadata | ... | Data | Metadata | ... | Data |
  |_|__|_|_____|__________|_____|______|__________|_____|______|
  0 +1K

(与其他只读文件系统不同，EROFS 卷被设计得尽可能简单：)

All data areas should be aligned with the block size, but metadata areas
may not. All metadata can be now observed in two different spaces (views):
(所有数据区域都应与块大小对齐，但元数据区域可以不对齐。现在可以在两个不同的空间(视图)中观察到所有元数据：)

 1. Inode metadata space
    (1. inode 元数据空间)

    Each valid inode should be aligned with an inode slot, which is a fixed
    value (32 bytes) and designed to be kept in line with compact inode size.
    (每个有效 inode 都应与一个 inode 槽对齐，inode 槽是一个固定值(32 字节)，其设计目的是与紧凑型 inode 大小保持一致。)

    Each inode can be directly found with the following formula:
         inode offset = meta_blkaddr * block_size + 32 * nid
    (每个 inode 都可以用以下公式直接找到：)

    ::

                                 |-> aligned with 8B
                                            |-> followed closely
     + meta_blkaddr blocks                                      |-> another slot
       _____________________________________________________________________
     |  ...   | inode |  xattrs  | extents  | data inline | ... | inode ...
     |________|_______|(optional)|(optional)|__(optional)_|_____|__________
              |-> aligned with the inode slot size
                   .                   .
                 .                         .
               .                              .
             .                                    .
           .                                         .
         .                                              .
       .____________________________________________________|-> aligned with 4B
       | xattr_ibody_header | shared xattrs | inline xattrs |
       |____________________|_______________|_______________|
       |->    12 bytes    <-|->x * 4 bytes<-|               .
                           .                .                 .
                     .                      .                   .
                .                           .                     .
            ._______________________________.______________________.
            | id | id | id | id |  ... | id | ent | ... | ent| ... |
            |____|____|____|____|______|____|_____|_____|____|_____|
                                            |-> aligned with 4B
                                                        |-> aligned with 4B

    Inode could be 32 or 64 bytes, which can be distinguished from a common
    field which all inode versions have -- i_format::
    (inode 可以是 32 或 64 字节，可以通过所有 inode 版本都具有的一个公共字段——i_format——来区分：)

        __________________               __________________
       |     i_format     |             |     i_format     |
       |__________________|             |__________________|
       |        ...       |             |        ...       |
       |                  |             |                  |
       |__________________| 32 bytes    |                  |
                                        |                  |
                                        |__________________| 64 bytes

    Xattrs, extents, data inline are placed after the corresponding inode with
    proper alignment, and they could be optional for different data mappings.
    _currently_ total 5 data layouts are supported:
    (xattr、extent、内联数据被放置在相应 inode 之后并适当对齐，对于不同的数据映射它们可以是可选的。_目前_总共支持 5 种数据布局：)

    ==  ====================================================================
     0  flat file data without data inline (no extent);
        (无内联数据的平面文件数据(无 extent)；)
     1  fixed-sized output data compression (with non-compacted indexes);
        (固定大小输出的数据压缩(使用非压缩索引)；)
     2  flat file data with tail packing data inline (no extent);
        (带尾部打包内联数据的平面文件数据(无 extent)；)
     3  fixed-sized output data compression (with compacted indexes, v5.3+);
        (固定大小输出的数据压缩(使用压缩索引，v5.3+)；)
     4  chunk-based file (v5.15+).
        (基于块(chunk)的文件(v5.15+)。)
    ==  ====================================================================

    The size of the optional xattrs is indicated by i_xattr_count in inode
    header. Large xattrs or xattrs shared by many different files can be
    stored in shared xattrs metadata rather than inlined right after inode.
    (可选 xattr 的大小由 inode 头中的 i_xattr_count 指示。大型 xattr 或被许多不同文件共享的 xattr 可以存储在共享 xattr 元数据中，而不是内联在 inode 之后。)

 2. Shared xattrs metadata space
    (2. 共享 xattr 元数据空间)

    Shared xattrs space is similar to the above inode space, started with
    a specific block indicated by xattr_blkaddr, organized one by one with
    proper align.
    (共享 xattr 空间与上述 inode 空间类似，从由 xattr_blkaddr 指示的特定块开始，以适当对齐的方式逐一组织。)

    Each share xattr can also be directly found by the following formula:
         xattr offset = xattr_blkaddr * block_size + 4 * xattr_id
    (每个共享 xattr 也可以用以下公式直接找到：)

::

                           |-> aligned by  4 bytes
    + xattr_blkaddr blocks                     |-> aligned with 4 bytes
     _________________________________________________________________________
    |  ...   | xattr_entry |  xattr data | ... |  xattr_entry | xattr data  ...
    |________|_____________|_____________|_____|______________|_______________

Directories
(目录)
-----------
All directories are now organized in a compact on-disk format. Note that
each directory block is divided into index and name areas in order to support
random file lookup, and all directory entries are _strictly_ recorded in
alphabetical order in order to support improved prefix binary search
algorithm (could refer to the related source code).
(所有目录现在都以紧凑的在盘格式组织。请注意，为了支持随机文件查找，每个目录块被划分为索引区和名字区，并且所有目录项都_严格地_按字母顺序记录，以支持改进的前缀二分查找算法(可以参考相关源代码)。)

::

                  ___________________________
                 /                           |
                /              ______________|________________
               /              /              | nameoff1       | nameoffN-1
  ____________.______________._______________v________________v__________
 | dirent | dirent | ... | dirent | filename | filename | ... | filename |
 |___.0___|____1___|_____|___N-1__|____0_____|____1_____|_____|___N-1____|
      \                           ^
       \                          |                           * could have
        \                         |                             trailing '\0'
         \________________________| nameoff0
                             Directory block

Note that apart from the offset of the first filename, nameoff0 also indicates
the total number of directory entries in this block since it is no need to
introduce another on-disk field at all.
(请注意，除了第一个文件名的偏移量之外，nameoff0 还指示了此块中目录项的总数，因为完全无需引入另一个在盘字段。)

Chunk-based files
(基于块的文件)
-----------------
In order to support chunk-based data deduplication, a new inode data layout has
been supported since Linux v5.15: Files are split in equal-sized data chunks
with ``extents`` area of the inode metadata indicating how to get the chunk
data: these can be simply as a 4-byte block address array or in the 8-byte
chunk index form (see struct erofs_inode_chunk_index in erofs_fs.h for more
details.)
(为了支持基于块的数据去重，自 Linux v5.15 起支持了一种新的 inode 数据布局：文件被分割成大小相等的数据块(chunk)，由 inode 元数据的 ``extents`` 区域指示如何获取块数据：这些可以简单地是 4 字节块地址数组，或者是 8 字节的块索引形式(更多细节参见 erofs_fs.h 中的 struct erofs_inode_chunk_index。)

By the way, chunk-based files are all uncompressed for now.
(顺便说一下，基于块的文件目前都是未压缩的。)

Long extended attribute name prefixes
(长扩展属性名前缀)
-------------------------------------
There are use cases where extended attributes with different values can have
only a few common prefixes (such as overlayfs xattrs).  The predefined prefixes
work inefficiently in both image size and runtime performance in such cases.
(有一些用例中，取值不同的扩展属性可能只有少数几个公共前缀(例如 overlayfs 的 xattr)。在这些情况下，预定义前缀在镜像大小和运行时性能上都很低效。)

The long xattr name prefixes feature is introduced to address this issue.  The
overall idea is that, apart from the existing predefined prefixes, the xattr
entry could also refer to user-specified long xattr name prefixes, e.g.
"trusted.overlay.".
(引入长 xattr 名前缀特性就是为了解决这个问题。总体思路是，除了现有的预定义前缀之外，xattr 项还可以引用用户指定的长 xattr 名前缀，例如 "trusted.overlay."。)

When referring to a long xattr name prefix, the highest bit (bit 7) of
erofs_xattr_entry.e_name_index is set, while the lower bits (bit 0-6) as a whole
represent the index of the referred long name prefix among all long name
prefixes.  Therefore, only the trailing part of the name apart from the long
xattr name prefix is stored in erofs_xattr_entry.e_name, which could be empty if
the full xattr name matches exactly as its long xattr name prefix.
(当引用长 xattr 名前缀时，erofs_xattr_entry.e_name_index 的最高位(位 7)被置位，而较低位(位 0-6)作为一个整体表示被引用的长名前缀在所有长名前缀中的索引。因此，除了长 xattr 名前缀之外，名字的其余尾部部分存储在 erofs_xattr_entry.e_name 中，如果完整的 xattr 名与其长 xattr 名前缀完全匹配，则该字段可以为空。)

All long xattr prefixes are stored one by one in the packed inode as long as
the packed inode is valid, or in the meta inode otherwise.  The
xattr_prefix_count (of the on-disk superblock) indicates the total number of
long xattr name prefixes, while (xattr_prefix_start * 4) indicates the start
offset of long name prefixes in the packed/meta inode.  Note that, long extended
attribute name prefixes are disabled if xattr_prefix_count is 0.
(只要打包 inode(packed inode)有效，所有长 xattr 前缀就逐一存储在打包 inode 中，否则存储在元 inode(meta inode)中。(在盘超级块中的)xattr_prefix_count 指示长 xattr 名前缀的总数，而(xattr_prefix_start * 4)指示长名前缀在打包/元 inode 中的起始偏移。注意，如果 xattr_prefix_count 为 0，则长扩展属性名前缀被禁用。)

Each long name prefix is stored in the format: ALIGN({__le16 len, data}, 4),
where len represents the total size of the data part.  The data part is actually
represented by 'struct erofs_xattr_long_prefix', where base_index represents the
index of the predefined xattr name prefix, e.g. EROFS_XATTR_INDEX_TRUSTED for
"trusted.overlay." long name prefix, while the infix string keeps the string
after stripping the short prefix, e.g. "overlay." for the example above.
(每个长名前缀以 ALIGN({__le16 len, data}, 4) 的格式存储，其中 len 表示数据部分的总大小。数据部分实际由 'struct erofs_xattr_long_prefix' 表示，其中 base_index 表示预定义 xattr 名前缀的索引，例如 "trusted.overlay." 长名前缀对应 EROFS_XATTR_INDEX_TRUSTED，而中缀字符串(infix)保留去掉短前缀之后的字符串，例如上例中的 "overlay."。)

Data compression
(数据压缩)
----------------
EROFS implements fixed-sized output compression which generates fixed-sized
compressed data blocks from variable-sized input in contrast to other existing
fixed-sized input solutions. Relatively higher compression ratios can be gotten
by using fixed-sized output compression since nowadays popular data compression
algorithms are mostly LZ77-based and such fixed-sized output approach can be
benefited from the historical dictionary (aka. sliding window).
(EROFS 实现了固定大小输出的压缩，与现有的其他固定大小输入方案不同，它从可变大小的输入生成固定大小的压缩数据块。使用固定大小输出压缩可以获得相对更高的压缩比，因为如今流行的数据压缩算法大多基于 LZ77，而这种固定大小输出的方式可以从历史字典(又称滑动窗口)中获益。)

In details, original (uncompressed) data is turned into several variable-sized
extents and in the meanwhile, compressed into physical clusters (pclusters).
In order to record each variable-sized extent, logical clusters (lclusters) are
introduced as the basic unit of compress indexes to indicate whether a new
extent is generated within the range (HEAD) or not (NONHEAD). Lclusters are now
fixed in block size, as illustrated below::
(具体而言，原始(未压缩)数据被转换成若干可变大小的 extent，同时被压缩成物理簇(pcluster)。为了记录每个可变大小的 extent，引入了逻辑簇(lcluster)作为压缩索引的基本单位，用于指示在该范围内是(HEAD)否(NONHEAD)生成了新的 extent。lcluster 现在固定为块大小，如下图所示：)

          |<-    variable-sized extent    ->|<-       VLE         ->|
        clusterofs                        clusterofs              clusterofs
          |                                 |                       |
 _________v_________________________________v_______________________v________
 ... |    .         |              |        .     |              |  .   ...
 ____|____._________|______________|________.___ _|______________|__.________
     |-> lcluster <-|-> lcluster <-|-> lcluster <-|-> lcluster <-|
          (HEAD)        (NONHEAD)       (HEAD)        (NONHEAD)    .
           .             CBLKCNT            .                    .
            .                               .                  .
             .                              .                .
       _______._____________________________.______________._________________
          ... |              |              |              | ...
       _______|______________|______________|______________|_________________
              |->      big pcluster       <-|-> pcluster <-|

A physical cluster can be seen as a container of physical compressed blocks
which contains compressed data. Previously, only lcluster-sized (4KB) pclusters
were supported. After big pcluster feature is introduced (available since
Linux v5.13), pcluster can be a multiple of lcluster size.
(物理簇可以看作是包含压缩数据的物理压缩块的容器。以前只支持 lcluster 大小(4KB)的 pcluster。在引入大 pcluster 特性之后(自 Linux v5.13 起可用)，pcluster 可以是 lcluster 大小的倍数。)

For each HEAD lcluster, clusterofs is recorded to indicate where a new extent
starts and blkaddr is used to seek the compressed data. For each NONHEAD
lcluster, delta0 and delta1 are available instead of blkaddr to indicate the
distance to its HEAD lcluster and the next HEAD lcluster. A PLAIN lcluster is
also a HEAD lcluster except that its data is uncompressed. See the comments
around "struct z_erofs_vle_decompressed_index" in erofs_fs.h for more details.
(对于每个 HEAD lcluster，记录 clusterofs 以指示新 extent 从哪里开始，并使用 blkaddr 来定位压缩数据。对于每个 NONHEAD lcluster，则使用 delta0 和 delta1 而不是 blkaddr 来指示到其 HEAD lcluster 以及下一个 HEAD lcluster 的距离。PLAIN lcluster 也是一种 HEAD lcluster，只是它的数据未压缩。更多细节参见 erofs_fs.h 中 "struct z_erofs_vle_decompressed_index" 附近的注释。)

If big pcluster is enabled, pcluster size in lclusters needs to be recorded as
well. Let the delta0 of the first NONHEAD lcluster store the compressed block
count with a special flag as a new called CBLKCNT NONHEAD lcluster. It's easy
to understand its delta0 is constantly 1, as illustrated below::

   __________________________________________________________
  | HEAD |  NONHEAD  | NONHEAD | ... | NONHEAD | HEAD | HEAD |
  |__:___|_(CBLKCNT)_|_________|_____|_________|__:___|____:_|
     |<----- a big pcluster (with CBLKCNT) ------>|<--  -->|
           a lcluster-sized pcluster (without CBLKCNT) ^

(如果启用了大 pcluster，还需要记录以 lcluster 为单位的 pcluster 大小。让第一个 NONHEAD lcluster 的 delta0 带着一个特殊标志存储压缩块计数，作为一种新称为 CBLKCNT 的 NONHEAD lcluster。容易理解，它的 delta0 恒为 1，如下图所示：)

If another HEAD follows a HEAD lcluster, there is no room to record CBLKCNT,
but it's easy to know the size of such pcluster is 1 lcluster as well.
(如果一个 HEAD lcluster 后面紧跟着另一个 HEAD，就没有空间记录 CBLKCNT，但很容易知道这种 pcluster 的大小同样为 1 个 lcluster。)

Since Linux v6.1, each pcluster can be used for multiple variable-sized extents,
therefore it can be used for compressed data deduplication.
(自 Linux v6.1 起，每个 pcluster 可以用于多个可变大小的 extent，因此它可以用于压缩数据去重。)
