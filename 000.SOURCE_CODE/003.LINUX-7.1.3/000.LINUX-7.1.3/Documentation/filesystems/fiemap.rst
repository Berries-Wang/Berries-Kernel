.. SPDX-License-Identifier: GPL-2.0

============
Fiemap Ioctl
============

(Fiemap ioctl)

The fiemap ioctl is an efficient method for userspace to get file
extent mappings. Instead of block-by-block mapping (such as bmap), fiemap
returns a list of extents.
(fiemap ioctl 是用户空间获取文件 extent 映射的一种高效方法。fiemap 不采用逐块映射(如 bmap)，而是返回一个 extent 列表。)


Request Basics
(请求基础)
--------------

A fiemap request is encoded within struct fiemap:
(fiemap 请求被编码在 struct fiemap 中：)

.. kernel-doc:: include/uapi/linux/fiemap.h
   :identifiers: fiemap

fm_start, and fm_length specify the logical range within the file
which the process would like mappings for. Extents returned mirror
those on disk - that is, the logical offset of the 1st returned extent
may start before fm_start, and the range covered by the last returned
extent may end after fm_length. All offsets and lengths are in bytes.
(fm_start 和 fm_length 指定进程希望获得映射的文件内的逻辑范围。返回的 extent 与磁盘上的相镜像——也就是说，第一个返回的 extent 的逻辑偏移可以在 fm_start 之前开始，而最后一个返回的 extent 所覆盖的范围可以在 fm_length 之后结束。所有偏移和长度都以字节为单位。)

Certain flags to modify the way in which mappings are looked up can be
set in fm_flags. If the kernel doesn't understand some particular
flags, it will return EBADR and the contents of fm_flags will contain
the set of flags which caused the error. If the kernel is compatible
with all flags passed, the contents of fm_flags will be unmodified.
It is up to userspace to determine whether rejection of a particular
flag is fatal to its operation. This scheme is intended to allow the
fiemap interface to grow in the future but without losing
compatibility with old software.
(可以在 fm_flags 中设置某些标志来修改查找映射的方式。如果内核不理解某些特定的标志，它将返回 EBADR，并且 fm_flags 的内容将包含导致该错误的标志集合。如果内核与传入的所有标志都兼容，则 fm_flags 的内容将保持不变。由用户空间自行判断某个特定标志被拒绝是否对其操作是致命的。这一方案旨在让 fiemap 接口在未来得以扩展，同时不失去与旧软件的兼容性。)

fm_extent_count specifies the number of elements in the fm_extents[] array
that can be used to return extents.  If fm_extent_count is zero, then the
fm_extents[] array is ignored (no extents will be returned), and the
fm_mapped_extents count will hold the number of extents needed in
fm_extents[] to hold the file's current mapping.  Note that there is
nothing to prevent the file from changing between calls to FIEMAP.
(fm_extent_count 指定可用于返回 extent 的 fm_extents[] 数组中的元素数量。如果 fm_extent_count 为零，则 fm_extents[] 数组将被忽略(不会返回任何 extent)，而 fm_mapped_extents 计数将保存在 fm_extents[] 中容纳该文件当前映射所需的 extent 数量。注意，没有任何机制可以防止文件在对 FIEMAP 的调用之间发生变化。)

The following flags can be set in fm_flags:
(以下标志可以在 fm_flags 中设置：)

FIEMAP_FLAG_SYNC
  If this flag is set, the kernel will sync the file before mapping extents.
  (如果设置了此标志，内核将在映射 extent 之前同步该文件。)

FIEMAP_FLAG_XATTR
  If this flag is set, the extents returned will describe the inodes
  extended attribute lookup tree, instead of its data tree.
  (如果设置了此标志，返回的 extent 将描述 inode 的扩展属性查找树，而不是其数据树。)

FIEMAP_FLAG_CACHE
  This flag requests caching of the extents.
  (此标志请求对 extent 进行缓存。)

Extent Mapping
(extent 映射)
--------------

Extent information is returned within the embedded fm_extents array
which userspace must allocate along with the fiemap structure. The
number of elements in the fiemap_extents[] array should be passed via
fm_extent_count. The number of extents mapped by kernel will be
returned via fm_mapped_extents. If the number of fiemap_extents
allocated is less than would be required to map the requested range,
the maximum number of extents that can be mapped in the fm_extent[]
array will be returned and fm_mapped_extents will be equal to
fm_extent_count. In that case, the last extent in the array will not
complete the requested range and will not have the FIEMAP_EXTENT_LAST
flag set (see the next section on extent flags).
(extent 信息在内嵌的 fm_extents 数组中返回，用户空间必须与 fiemap 结构一起分配该数组。fiemap_extents[] 数组中的元素数量应通过 fm_extent_count 传入。内核映射的 extent 数量将通过 fm_mapped_extents 返回。如果分配的 fiemap_extents 数量少于映射请求范围所需的数量，则将返回 fm_extent[] 数组中可映射的最大 extent 数量，并且 fm_mapped_extents 将等于 fm_extent_count。在这种情况下，数组中的最后一个 extent 将无法完成所请求的范围，并且不会设置 FIEMAP_EXTENT_LAST 标志(参见下一节关于 extent 标志的内容)。)

Each extent is described by a single fiemap_extent structure as
returned in fm_extents:
(每个 extent 由 fm_extents 中返回的一个单独的 fiemap_extent 结构描述：)

.. kernel-doc:: include/uapi/linux/fiemap.h
    :identifiers: fiemap_extent

All offsets and lengths are in bytes and mirror those on disk.  It is valid
for an extents logical offset to start before the request or its logical
length to extend past the request.  Unless FIEMAP_EXTENT_NOT_ALIGNED is
returned, fe_logical, fe_physical, and fe_length will be aligned to the
block size of the file system.  With the exception of extents flagged as
FIEMAP_EXTENT_MERGED, adjacent extents will not be merged.
(所有偏移和长度都以字节为单位，并与磁盘上的相镜像。extent 的逻辑偏移在请求之前开始、或其逻辑长度延伸到请求之后，都是有效的。除非返回了 FIEMAP_EXTENT_NOT_ALIGNED，否则 fe_logical、fe_physical 和 fe_length 将按文件系统的块大小对齐。除了被标记为 FIEMAP_EXTENT_MERGED 的 extent 之外，相邻的 extent 不会被合并。)

The fe_flags field contains flags which describe the extent returned.
A special flag, FIEMAP_EXTENT_LAST is always set on the last extent in
the file so that the process making fiemap calls can determine when no
more extents are available, without having to call the ioctl again.
(fe_flags 字段包含描述所返回 extent 的标志。一个特殊的标志 FIEMAP_EXTENT_LAST 总是被设置在文件中的最后一个 extent 上，这样进行 fiemap 调用的进程就可以判断何时不再有更多可用的 extent，而不必再次调用 ioctl。)

Some flags are intentionally vague and will always be set in the
presence of other more specific flags. This way a program looking for
a general property does not have to know all existing and future flags
which imply that property.
(有些标志被有意设计得比较模糊，只要存在其他更具体的标志，它们就总会被设置。这样，寻找某个一般性属性的程序就不必知道所有现在和将来暗示该属性的标志。)

For example, if FIEMAP_EXTENT_DATA_INLINE or FIEMAP_EXTENT_DATA_TAIL
are set, FIEMAP_EXTENT_NOT_ALIGNED will also be set. A program looking
for inline or tail-packed data can key on the specific flag. Software
which simply cares not to try operating on non-aligned extents
however, can just key on FIEMAP_EXTENT_NOT_ALIGNED, and not have to
worry about all present and future flags which might imply unaligned
data. Note that the opposite is not true - it would be valid for
FIEMAP_EXTENT_NOT_ALIGNED to appear alone.
(例如，如果设置了 FIEMAP_EXTENT_DATA_INLINE 或 FIEMAP_EXTENT_DATA_TAIL，则也会设置 FIEMAP_EXTENT_NOT_ALIGNED。寻找内联数据或尾部打包数据的程序可以关注特定的标志。而只关心不去尝试对非对齐 extent 进行操作的软件，可以只关注 FIEMAP_EXTENT_NOT_ALIGNED，而不必担心所有现在和将来可能暗示非对齐数据的标志。注意，反过来并不成立——FIEMAP_EXTENT_NOT_ALIGNED 单独出现也是有效的。)

FIEMAP_EXTENT_LAST
  This is generally the last extent in the file. A mapping attempt past
  this extent may return nothing. Some implementations set this flag to
  indicate this extent is the last one in the range queried by the user
  (via fiemap->fm_length).
  (这通常是文件中的最后一个 extent。对该 extent 之后进行映射尝试可能什么也返回不了。一些实现设置此标志以表明该 extent 是用户所查询范围(通过 fiemap->fm_length)中的最后一个。)

FIEMAP_EXTENT_UNKNOWN
  The location of this extent is currently unknown. This may indicate
  the data is stored on an inaccessible volume or that no storage has
  been allocated for the file yet.
  (这个 extent 的位置目前未知。这可能表明数据存储在一个不可访问的卷上，或者尚未为该文件分配存储。)

FIEMAP_EXTENT_DELALLOC
  This will also set FIEMAP_EXTENT_UNKNOWN.
  (这也会设置 FIEMAP_EXTENT_UNKNOWN。)

  Delayed allocation - while there is data for this extent, its
  physical location has not been allocated yet.
  (延迟分配——虽然该 extent 已有数据，但其物理位置尚未被分配。)

FIEMAP_EXTENT_ENCODED
  This extent does not consist of plain filesystem blocks but is
  encoded (e.g. encrypted or compressed).  Reading the data in this
  extent via I/O to the block device will have undefined results.
  (这个 extent 不是由普通文件系统块组成，而是经过编码的(例如加密或压缩)。通过对块设备进行 I/O 来读取该 extent 中的数据将产生未定义的结果。)

Note that it is *always* undefined to try to update the data
in-place by writing to the indicated location without the
assistance of the filesystem, or to access the data using the
information returned by the FIEMAP interface while the filesystem
is mounted.  In other words, user applications may only read the
extent data via I/O to the block device while the filesystem is
unmounted, and then only if the FIEMAP_EXTENT_ENCODED flag is
clear; user applications must not try reading or writing to the
filesystem via the block device under any other circumstances.
(注意，在没有文件系统协助的情况下，尝试通过向所指示的位置写入来就地更新数据，或者在文件系统被挂载时使用 FIEMAP 接口返回的信息访问数据，*总是*会产生未定义的结果。换言之，用户应用程序只能在文件系统被卸载时通过对块设备的 I/O 读取 extent 数据，而且仅当 FIEMAP_EXTENT_ENCODED 标志为清除状态时才行；在任何其他情况下，用户应用程序都不得尝试通过块设备读取或写入文件系统。)

FIEMAP_EXTENT_DATA_ENCRYPTED
  This will also set FIEMAP_EXTENT_ENCODED
  The data in this extent has been encrypted by the file system.
  (这也会设置 FIEMAP_EXTENT_ENCODED。此 extent 中的数据已被文件系统加密。)

FIEMAP_EXTENT_NOT_ALIGNED
  Extent offsets and length are not guaranteed to be block aligned.
  (extent 的偏移和长度不保证按块对齐。)

FIEMAP_EXTENT_DATA_INLINE
  This will also set FIEMAP_EXTENT_NOT_ALIGNED
  Data is located within a meta data block.
  (这也会设置 FIEMAP_EXTENT_NOT_ALIGNED。数据位于一个元数据块内。)

FIEMAP_EXTENT_DATA_TAIL
  This will also set FIEMAP_EXTENT_NOT_ALIGNED
  Data is packed into a block with data from other files.
  (这也会设置 FIEMAP_EXTENT_NOT_ALIGNED。数据与来自其他文件的数据一起打包进一个块中。)

FIEMAP_EXTENT_UNWRITTEN
  Unwritten extent - the extent is allocated but its data has not been
  initialized.  This indicates the extent's data will be all zero if read
  through the filesystem but the contents are undefined if read directly from
  the device.
  (未写入 extent——该 extent 已被分配但其数据尚未被初始化。这表明如果通过文件系统读取，该 extent 的数据将全为零，但如果直接从设备读取，其内容则是未定义的。)

FIEMAP_EXTENT_MERGED
  This will be set when a file does not support extents, i.e., it uses a block
  based addressing scheme.  Since returning an extent for each block back to
  userspace would be highly inefficient, the kernel will try to merge most
  adjacent blocks into 'extents'.
  (当文件不支持 extents 时(即它使用基于块的寻址方案)会设置此标志。由于把每个块作为一个 extent 返回给用户空间效率极低，内核会尝试将大多数相邻块合并成 "extent"。)

FIEMAP_EXTENT_SHARED
  This flag is set to request that space be shared with other files.
  (设置此标志表示请求与其他文件共享空间。)

VFS -> File System Implementation
(VFS -> 文件系统实现)
---------------------------------

File systems wishing to support fiemap must implement a ->fiemap callback on
their inode_operations structure. The fs ->fiemap call is responsible for
defining its set of supported fiemap flags, and calling a helper function on
each discovered extent::
(希望支持 fiemap 的文件系统必须在其 inode_operations 结构上实现 ->fiemap 回调。文件系统的 ->fiemap 调用负责定义其支持的 fiemap 标志集合，并对每个发现的 extent 调用一个辅助函数：)

  struct inode_operations {
       ...

       int (*fiemap)(struct inode *, struct fiemap_extent_info *, u64 start,
                     u64 len);

->fiemap is passed struct fiemap_extent_info which describes the
fiemap request:
(->fiemap 被传入 struct fiemap_extent_info，它描述了 fiemap 请求：)

.. kernel-doc:: include/linux/fiemap.h
    :identifiers: fiemap_extent_info

It is intended that the file system should not need to access any of this
structure directly. Filesystem handlers should be tolerant to signals and return
EINTR once fatal signal received.
(本意是文件系统不需要直接访问该结构的任何成员。文件系统处理程序应能容忍信号，并在收到致命信号后返回 EINTR。)


Flag checking should be done at the beginning of the ->fiemap callback via the
fiemap_prep() helper::
(标志检查应在 ->fiemap 回调的开始处通过 fiemap_prep() 辅助函数完成：)

  int fiemap_prep(struct inode *inode, struct fiemap_extent_info *fieinfo,
		  u64 start, u64 *len, u32 supported_flags);

The struct fieinfo should be passed in as received from ioctl_fiemap(). The
set of fiemap flags which the fs understands should be passed via fs_flags. If
fiemap_prep finds invalid user flags, it will place the bad values in
fieinfo->fi_flags and return -EBADR. If the file system gets -EBADR, from
fiemap_prep(), it should immediately exit, returning that error back to
ioctl_fiemap().  Additionally the range is validate against the supported
maximum file size.
(struct fieinfo 应按从 ioctl_fiemap() 接收到的方式传入。文件系统所理解的 fiemap 标志集合应通过 fs_flags 传入。如果 fiemap_prep 发现无效的用户标志，它会把错误的值放入 fieinfo->fi_flags 并返回 -EBADR。如果文件系统从 fiemap_prep() 得到 -EBADR，它应立即退出，将该错误返回给 ioctl_fiemap()。此外，还会根据所支持的最大文件大小来校验该范围。)


For each extent in the request range, the file system should call
the helper function, fiemap_fill_next_extent()::
(对于请求范围中的每个 extent，文件系统应调用辅助函数 fiemap_fill_next_extent()：)

  int fiemap_fill_next_extent(struct fiemap_extent_info *info, u64 logical,
			      u64 phys, u64 len, u32 flags, u32 dev);

fiemap_fill_next_extent() will use the passed values to populate the
next free extent in the fm_extents array. 'General' extent flags will
automatically be set from specific flags on behalf of the calling file
system so that the userspace API is not broken.
(fiemap_fill_next_extent() 将使用传入的值来填充 fm_extents 数组中的下一个空闲 extent。"一般性" extent 标志将代表调用它的文件系统根据具体标志自动设置，以免破坏用户空间 API。)

fiemap_fill_next_extent() returns 0 on success, and 1 when the
user-supplied fm_extents array is full. If an error is encountered
while copying the extent to user memory, -EFAULT will be returned.
(fiemap_fill_next_extent() 成功时返回 0，当用户提供的 fm_extents 数组已满时返回 1。如果在将 extent 复制到用户内存的过程中遇到错误，将返回 -EFAULT。)
