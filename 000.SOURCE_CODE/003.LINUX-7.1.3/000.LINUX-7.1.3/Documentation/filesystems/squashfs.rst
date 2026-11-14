.. SPDX-License-Identifier: GPL-2.0

=======================
Squashfs 4.0 Filesystem
=======================

(Squashfs 4.0 文件系统)

Squashfs is a compressed read-only filesystem for Linux.
(Squashfs 是一个面向 Linux 的压缩只读文件系统。)

It uses zlib, lz4, lzo, xz or zstd compression to compress files, inodes and
directories.  Inodes in the system are very small and all blocks are packed to
minimise data overhead. Block sizes greater than 4K are supported up to a
maximum of 1Mbytes (default block size 128K).
(它使用 zlib、lz4、lzo、xz 或 zstd 压缩来压缩文件、inode 和目录。系统中的 inode 非常小，并且所有块都被紧密打包以将数据开销降到最低。支持大于 4K 的块大小，最大可达 1MB(默认块大小 128K)。)

Squashfs is intended for general read-only filesystem use, for archival
use (i.e. in cases where a .tar.gz file may be used), and in constrained
block device/memory systems (e.g. embedded systems) where low overhead is
needed.
(Squashfs 旨在用于一般的只读文件系统场景、归档用途(即可以使用 .tar.gz 文件的场合)，以及需要低开销的受限块设备/内存系统(例如嵌入式系统)。)

Mailing list (kernel code): linux-fsdevel@vger.kernel.org
(邮件列表(内核代码)：linux-fsdevel@vger.kernel.org)
Web site: github.com/plougher/squashfs-tools
(网站：github.com/plougher/squashfs-tools)

1. Filesystem Features
(1. 文件系统特性)
----------------------

Squashfs filesystem features versus Cramfs:
(Squashfs 文件系统特性与 Cramfs 对比：)

============================== 	=========		==========
				Squashfs		Cramfs
============================== 	=========		==========
Max filesystem size		2^64			256 MiB (最大文件系统大小)
Max file size			~ 2 TiB			16 MiB (最大文件大小)
Max files			unlimited		unlimited (最大文件数)
Max directories			unlimited		unlimited (最大目录数)
Max entries per directory	unlimited		unlimited (每目录最大条目数)
Max block size			1 MiB			4 KiB (最大块大小)
Metadata compression		yes			no (元数据压缩)
Directory indexes		yes			no (目录索引)
Sparse file support		yes			no (稀疏文件支持)
Tail-end packing (fragments)	yes			no (尾部打包(碎片))
Exportable (NFS etc.)		yes			no (可导出(NFS 等))
Hard link support		yes			no (硬链接支持)
"." and ".." in readdir		yes			no (readdir 中的 "." 与 "..")
Real inode numbers		yes			no (真实 inode 编号)
32-bit uids/gids		yes			no (32 位 uid/gid)
File creation time		yes			no (文件创建时间)
Xattr support			yes			no (xattr 支持)
ACL support			no			no (ACL 支持)
============================== 	=========		==========

Squashfs compresses data, inodes and directories.  In addition, inode and
directory data are highly compacted, and packed on byte boundaries.  Each
compressed inode is on average 8 bytes in length (the exact length varies on
file type, i.e. regular file, directory, symbolic link, and block/char device
inodes have different sizes).
(Squashfs 压缩数据、inode 和目录。此外，inode 和目录数据被高度紧缩，并按字节边界打包。每个压缩后的 inode 平均长度为 8 字节(确切长度因文件类型而异，即普通文件、目录、符号链接以及块/字符设备 inode 具有不同的大小)。)

2. Using Squashfs
(2. 使用 Squashfs)
-----------------

As squashfs is a read-only filesystem, the mksquashfs program must be used to
create populated squashfs filesystems.  This and other squashfs utilities
are very likely packaged by your linux distribution (called squashfs-tools).
The source code can be obtained from github.com/plougher/squashfs-tools.
Usage instructions can also be obtained from this site.
(由于 squashfs 是只读文件系统，必须使用 mksquashfs 程序来创建填充了内容的 squashfs 文件系统。这个工具以及其他 squashfs 实用程序很可能已由你的 linux 发行版打包(称为 squashfs-tools)。源代码可以从 github.com/plougher/squashfs-tools 获取。使用说明也可以从该站点获取。)

2.1 Mount options
(2.1 挂载选项)
-----------------
===================    =========================================================
errors=%s              Specify whether squashfs errors trigger a kernel panic
                       or not (指定 squashfs 错误是否触发内核 panic)

		       ==========  =============================================
                         continue  errors don't trigger a panic (default) (错误不触发 panic(默认))
                            panic  trigger a panic when errors are encountered,
                                   similar to several other filesystems (e.g.
                                   btrfs, ext4, f2fs, GFS2, jfs, ntfs, ubifs) (遇到错误时触发 panic，类似于其他一些文件系统(例如 btrfs、ext4、f2fs、GFS2、jfs、ntfs、ubifs))

                                   This allows a kernel dump to be saved,
                                   useful for analyzing and debugging the
                                   corruption. (这允许保存内核转储，有助于分析和调试损坏问题。)
                       ==========  =============================================
threads=%s             Select the decompression mode or the number of threads (选择解压模式或线程数)

                       If SQUASHFS_CHOICE_DECOMP_BY_MOUNT is set: (如果设置了 SQUASHFS_CHOICE_DECOMP_BY_MOUNT：)

		       ==========  =============================================
                           single  use single-threaded decompression (default) (使用单线程解压(默认))

                                   Only one block (data or metadata) can be
                                   decompressed at any one time. This limits
                                   CPU and memory usage to a minimum, but it
                                   also gives poor performance on parallel I/O
                                   workloads when using multiple CPU machines
                                   due to waiting on decompressor availability. (同一时刻只能解压一个块(数据或元数据)。这将 CPU 和内存使用限制在最低，但由于需要等待解压器可用，在多 CPU 机器上的并行 I/O 工作负载下性能较差。)
                            multi  use up to two parallel decompressors per core (每个核心最多使用两个并行解压器)

                                   If you have a parallel I/O workload and your
                                   system has enough memory, using this option
                                   may improve overall I/O performance. It
                                   dynamically allocates decompressors on a
                                   demand basis. (如果你有并行 I/O 工作负载且系统内存充足，使用此选项可能会提高整体 I/O 性能。它会按需动态分配解压器。)
                           percpu  use a maximum of one decompressor per core (每个核心最多使用一个解压器)

                                   It uses percpu variables to ensure
                                   decompression is load-balanced across the
                                   cores. (它使用 percpu 变量来确保解压在各核心之间负载均衡。)
                        1|2|3|...  configure the number of threads used for
                                   decompression (配置用于解压的线程数)

                                   The upper limit is num_online_cpus() * 2. (上限为 num_online_cpus() * 2。)
                       ==========  =============================================

                       If SQUASHFS_CHOICE_DECOMP_BY_MOUNT is **not** set and
                       SQUASHFS_DECOMP_MULTI, SQUASHFS_MOUNT_DECOMP_THREADS are
                       both set: (如果 **未** 设置 SQUASHFS_CHOICE_DECOMP_BY_MOUNT，且 SQUASHFS_DECOMP_MULTI 与 SQUASHFS_MOUNT_DECOMP_THREADS 两者都被设置：)

		       ==========  =============================================
                          2|3|...  configure the number of threads used for
                                   decompression (配置用于解压的线程数)

                                   The upper limit is num_online_cpus() * 2. (上限为 num_online_cpus() * 2。)
                       ==========  =============================================

===================    =========================================================

3. Squashfs Filesystem Design
(3. Squashfs 文件系统设计)
-----------------------------

A squashfs filesystem consists of a maximum of nine parts, packed together on a
byte alignment::
(squashfs 文件系统最多由九个部分组成，按字节对齐打包在一起：)

	 ---------------
	|  superblock 	|
	|---------------|
	|  compression  |
	|    options    |
	|---------------|
	|  datablocks   |
	|  & fragments  |
	|---------------|
	|  inode table	|
	|---------------|
	|   directory	|
	|     table     |
	|---------------|
	|   fragment	|
	|    table      |
	|---------------|
	|    export     |
	|    table      |
	|---------------|
	|    uid/gid	|
	|  lookup table	|
	|---------------|
	|     xattr     |
	|     table	|
	 ---------------

Compressed data blocks are written to the filesystem as files are read from
the source directory, and checked for duplicates.  Once all file data has been
written the completed inode, directory, fragment, export, uid/gid lookup and
xattr tables are written.
(压缩的数据块在从源目录读取文件时被写入文件系统，并进行重复检查。一旦所有文件数据都写入完毕，就会写入完整的 inode 表、目录表、碎片表、导出表、uid/gid 查找表和 xattr 表。)

3.1 Compression options
(3.1 压缩选项)
-----------------------

Compressors can optionally support compression specific options (e.g.
dictionary size).  If non-default compression options have been used, then
these are stored here.
(压缩器可以有选择地支持压缩相关的选项(例如字典大小)。如果使用了非默认的压缩选项，那么这些选项就被存储在这里。)

3.2 Inodes
(3.2 Inode)
----------

Metadata (inodes and directories) are compressed in 8Kbyte blocks.  Each
compressed block is prefixed by a two byte length, the top bit is set if the
block is uncompressed.  A block will be uncompressed if the -noI option is set,
or if the compressed block was larger than the uncompressed block.
(元数据(inode 和目录)以 8K 字节的块进行压缩。每个压缩块都以两字节的长度作为前缀，如果该块未被压缩，则最高位会被置位。如果设置了 -noI 选项，或者压缩后的块比未压缩的块更大，那么该块将不被压缩。)

Inodes are packed into the metadata blocks, and are not aligned to block
boundaries, therefore inodes overlap compressed blocks.  Inodes are identified
by a 48-bit number which encodes the location of the compressed metadata block
containing the inode, and the byte offset into that block where the inode is
placed (<block, offset>).
(inode 被打包进元数据块中，并且不按块边界对齐，因此 inode 会跨越压缩块。inode 由一个 48 位数字标识，该数字编码了包含该 inode 的压缩元数据块的位置，以及该 inode 在该块中所处位置的字节偏移(<block, offset>)。)

To maximise compression there are different inodes for each file type
(regular file, directory, device, etc.), the inode contents and length
varying with the type.
(为了最大化压缩率，每种文件类型(普通文件、目录、设备等)都有不同的 inode，inode 的内容和长度随类型而变化。)

To further maximise compression, two types of regular file inode and
directory inode are defined: inodes optimised for frequently occurring
regular files and directories, and extended types where extra
information has to be stored.
(为了进一步最大化压缩率，定义了两种类型的普通文件 inode 和目录 inode：针对频繁出现的普通文件和目录进行优化的 inode，以及需要存储额外信息的扩展类型。)

3.3 Directories
(3.3 目录)
---------------

Like inodes, directories are packed into compressed metadata blocks, stored
in a directory table.  Directories are accessed using the start address of
the metablock containing the directory and the offset into the
decompressed block (<block, offset>).
(与 inode 一样，目录被打包进压缩的元数据块中，存储在目录表中。目录通过使用包含该目录的元数据块的起始地址以及解压后块内的偏移(<block, offset>)来访问。)

Directories are organised in a slightly complex way, and are not simply
a list of file names.  The organisation takes advantage of the
fact that (in most cases) the inodes of the files will be in the same
compressed metadata block, and therefore, can share the start block.
Directories are therefore organised in a two level list, a directory
header containing the shared start block value, and a sequence of directory
entries, each of which share the shared start block.  A new directory header
is written once/if the inode start block changes.  The directory
header/directory entry list is repeated as many times as necessary.
(目录的组织方式稍微复杂一些，并不是简单的文件名列表。这种组织方式利用了这样一个事实：(在大多数情况下)各文件的 inode 会位于同一个压缩元数据块中，因此可以共享起始块。所以目录被组织成一个两级列表：一个包含共享起始块值的目录头，以及一系列目录项，每个目录项都共享该起始块。一旦/只要 inode 起始块发生变化，就会写入一个新的目录头。目录头/目录项列表根据需要重复任意多次。)

Directories are sorted, and can contain a directory index to speed up
file lookup.  Directory indexes store one entry per metablock, each entry
storing the index/filename mapping to the first directory header
in each metadata block.  Directories are sorted in alphabetical order,
and at lookup the index is scanned linearly looking for the first filename
alphabetically larger than the filename being looked up.  At this point the
location of the metadata block the filename is in has been found.
The general idea of the index is to ensure only one metadata block needs to be
decompressed to do a lookup irrespective of the length of the directory.
This scheme has the advantage that it doesn't require extra memory overhead
and doesn't require much extra storage on disk.
(目录是经过排序的，并且可以包含一个目录索引来加速文件查找。目录索引在每个元数据块中存储一个条目，每个条目存储映射到每个元数据块中第一个目录头的索引/文件名映射。目录按字母顺序排序，在查找时，索引被线性扫描，寻找按字母顺序大于被查找文件名的第一个文件名。此时，该文件名所在的元数据块的位置就已经找到了。索引的总体思想是确保无论目录有多长，进行一次查找只需要解压一个元数据块。这种方案的优点是它不需要额外的内存开销，也不需要在磁盘上占用太多额外存储空间。)

3.4 File data
(3.4 文件数据)
-------------

Regular files consist of a sequence of contiguous compressed blocks, and/or a
compressed fragment block (tail-end packed block).   The compressed size
of each datablock is stored in a block list contained within the
file inode.
(普通文件由一系列连续的压缩块和/或一个压缩的碎片块(尾部打包块)组成。每个数据块的压缩后大小存储在文件 inode 内包含的块列表中。)

To speed up access to datablocks when reading 'large' files (256 Mbytes or
larger), the code implements an index cache that caches the mapping from
block index to datablock location on disk.
(为了在读取"大"文件(256MB 或更大)时加速对数据块的访问，代码实现了一个索引缓存，用于缓存从块索引到磁盘上数据块位置的映射。)

The index cache allows Squashfs to handle large files (up to 1.75 TiB) while
retaining a simple and space-efficient block list on disk.  The cache
is split into slots, caching up to eight 224 GiB files (128 KiB blocks).
Larger files use multiple slots, with 1.75 TiB files using all 8 slots.
The index cache is designed to be memory efficient, and by default uses
16 KiB.
(索引缓存使 Squashfs 能够处理大文件(最大 1.75 TiB)，同时在磁盘上保留一个简单且节省空间的块列表。缓存被划分成多个槽位，最多可缓存八个 224 GiB 的文件(128 KiB 块)。更大的文件会使用多个槽位，1.75 TiB 的文件会使用全部 8 个槽位。索引缓存的设计注重内存效率，默认使用 16 KiB。)

3.5 Fragment lookup table
(3.5 碎片查找表)
-------------------------

Regular files can contain a fragment index which is mapped to a fragment
location on disk and compressed size using a fragment lookup table.  This
fragment lookup table is itself stored compressed into metadata blocks.
A second index table is used to locate these.  This second index table for
speed of access (and because it is small) is read at mount time and cached
in memory.
(普通文件可以包含一个碎片索引，该索引通过碎片查找表映射到磁盘上的碎片位置和压缩后大小。这个碎片查找表本身也以压缩形式存储在元数据块中。使用第二个索引表来定位这些内容。第二个索引表为了访问速度(并且因为它很小)会在挂载时读取并缓存在内存中。)

3.6 Uid/gid lookup table
(3.6 Uid/gid 查找表)
------------------------

For space efficiency regular files store uid and gid indexes, which are
converted to 32-bit uids/gids using an id look up table.  This table is
stored compressed into metadata blocks.  A second index table is used to
locate these.  This second index table for speed of access (and because it
is small) is read at mount time and cached in memory.
(为了节省空间，普通文件存储 uid 和 gid 索引，这些索引通过一个 id 查找表转换为 32 位的 uid/gid。该表以压缩形式存储在元数据块中。使用第二个索引表来定位这些内容。第二个索引表为了访问速度(并且因为它很小)会在挂载时读取并缓存在内存中。)

3.7 Export table
(3.7 导出表)
----------------

To enable Squashfs filesystems to be exportable (via NFS etc.) filesystems
can optionally (disabled with the -no-exports Mksquashfs option) contain
an inode number to inode disk location lookup table.  This is required to
enable Squashfs to map inode numbers passed in filehandles to the inode
location on disk, which is necessary when the export code reinstantiates
expired/flushed inodes.
(为了使 Squashfs 文件系统可导出(通过 NFS 等)，文件系统可以有选择地(通过 -no-exports Mksquashfs 选项禁用)包含一个从 inode 编号到 inode 磁盘位置的查找表。这是让 Squashfs 能够把文件句柄中传入的 inode 编号映射到磁盘上的 inode 位置所必需的，当导出代码重新实例化过期/已刷写的 inode 时，这一点是必要的。)

This table is stored compressed into metadata blocks.  A second index table is
used to locate these.  This second index table for speed of access (and because
it is small) is read at mount time and cached in memory.
(该表以压缩形式存储在元数据块中。使用第二个索引表来定位这些内容。第二个索引表为了访问速度(并且因为它很小)会在挂载时读取并缓存在内存中。)

3.8 Xattr table
(3.8 Xattr 表)
---------------

The xattr table contains extended attributes for each inode.  The xattrs
for each inode are stored in a list, each list entry containing a type,
name and value field.  The type field encodes the xattr prefix
("user.", "trusted." etc) and it also encodes how the name/value fields
should be interpreted.  Currently the type indicates whether the value
is stored inline (in which case the value field contains the xattr value),
or if it is stored out of line (in which case the value field stores a
reference to where the actual value is stored).  This allows large values
to be stored out of line improving scanning and lookup performance and it
also allows values to be de-duplicated, the value being stored once, and
all other occurrences holding an out of line reference to that value.
(xattr 表包含每个 inode 的扩展属性。每个 inode 的 xattr 存储在一个列表中，每个列表项包含一个类型字段、名称字段和值字段。类型字段编码了 xattr 前缀("user."、"trusted." 等)，它还编码了名称/值字段应如何解释。目前，类型指示该值是内联存储的(此时值字段包含 xattr 值)，还是外置存储的(此时值字段存储一个指向实际值存储位置的引用)。这允许将大值外置存储，从而提高扫描和查找性能，并且还允许对值去重——该值只存储一次，所有其他出现的地方都持有指向该值的外置引用。)

The xattr lists are packed into compressed 8K metadata blocks.
To reduce overhead in inodes, rather than storing the on-disk
location of the xattr list inside each inode, a 32-bit xattr id
is stored.  This xattr id is mapped into the location of the xattr
list using a second xattr id lookup table.
(xattr 列表被打包进压缩的 8K 元数据块中。为了减少 inode 中的开销，不在每个 inode 内存储 xattr 列表的磁盘位置，而是存储一个 32 位的 xattr id。这个 xattr id 通过第二个 xattr id 查找表映射到 xattr 列表的位置。)

4. TODOs and Outstanding Issues
(4. TODO 与未决问题)
-------------------------------

4.1 TODO list
(4.1 TODO 列表)
-------------

Implement ACL support.
(实现 ACL 支持。)

4.2 Squashfs Internal Cache
(4.2 Squashfs 内部缓存)
---------------------------

Blocks in Squashfs are compressed.  To avoid repeatedly decompressing
recently accessed data Squashfs uses two small metadata and fragment caches.
(Squashfs 中的块是压缩的。为了避免对最近访问的数据反复解压，Squashfs 使用两个小的元数据缓存和碎片缓存。)

The cache is not used for file datablocks, these are decompressed and cached in
the page-cache in the normal way.  The cache is used to temporarily cache
fragment and metadata blocks which have been read as a result of a metadata
(i.e. inode or directory) or fragment access.  Because metadata and fragments
are packed together into blocks (to gain greater compression) the read of a
particular piece of metadata or fragment will retrieve other metadata/fragments
which have been packed with it, these because of locality-of-reference may be
read in the near future. Temporarily caching them ensures they are available
for near future access without requiring an additional read and decompress.
(该缓存不用于文件数据块，数据块按常规方式解压并缓存在页缓存中。该缓存用于临时缓存因元数据(即 inode 或目录)访问或碎片访问而被读取的碎片块和元数据块。由于元数据和碎片被一起打包进块中(以获得更高的压缩率)，读取某个特定的元数据或碎片会连带取回与它打包在一起的其他元数据/碎片，由于引用的局部性，这些内容可能在不久的将来被读取。临时缓存它们可以确保它们在不久之后的访问中可用，而不需要额外的读取和解压。)

In the future this internal cache may be replaced with an implementation which
uses the kernel page cache.  Because the page cache operates on page sized
units this may introduce additional complexity in terms of locking and
associated race conditions.
(将来，这个内部缓存可能会被一个使用内核页缓存的实现所取代。由于页缓存以页大小的单位进行操作，这可能会在锁定及相关竞争条件方面带来额外的复杂性。)
