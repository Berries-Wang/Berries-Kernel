.. SPDX-License-Identifier: GPL-2.0
.. _atomic_writes:

Atomic Block Writes
(原子块写)
-------------------------

Introduction
(引言)
~~~~~~~~~~~~

Atomic (untorn) block writes ensure that either the entire write is committed
to disk or none of it is. This prevents "torn writes" during power loss or
system crashes. The ext4 filesystem supports atomic writes (only with Direct
I/O) on regular files with extents, provided the underlying storage device
supports hardware atomic writes. This is supported in the following two ways:
(原子(无撕裂)块写确保要么整个写被提交到磁盘，要么完全不提交。这可以防止掉电或系统崩溃期间的"撕裂写"。ext4 文件系统在具有 extents 的普通文件上支持原子写(仅限直接 I/O)，前提是底层存储设备支持硬件原子写。这通过以下两种方式得到支持：)

1. **Single-fsblock Atomic Writes**:
   EXT4 supports atomic write operations with a single filesystem block since
   v6.13. In this the atomic write unit minimum and maximum sizes are both set
   to filesystem blocksize.
   e.g. doing atomic write of 16KB with 16KB filesystem blocksize on 64KB
   pagesize system is possible.
   (单 FS 块原子写：EXT4 自 v6.13 起支持针对单个文件系统块的原子写操作。其中，原子写单元的最小和最大大小都被设置为文件系统块大小。例如，在 64KB 页大小的系统上，使用 16KB 文件系统块大小进行 16KB 的原子写是可能的。)

2. **Multi-fsblock Atomic Writes with Bigalloc**:
   EXT4 now also supports atomic writes spanning multiple filesystem blocks
   using a feature known as bigalloc. The atomic write unit's minimum and
   maximum sizes are determined by the filesystem block size and cluster size,
   based on the underlying device’s supported atomic write unit limits.
   (使用 bigalloc 的多 FS 块原子写：EXT4 现在还支持使用一种称为 bigalloc 的特性进行跨越多个文件系统块的原子写。原子写单元的最小和最大大小由文件系统块大小和簇大小决定，并基于底层设备所支持的原子写单元限制。)

Requirements
(要求)
~~~~~~~~~~~~

Basic requirements for atomic writes in ext4:
(ext4 中原子写的基本要求：)

 1. The extents feature must be enabled (default for ext4)
    (必须启用 extents 特性(ext4 的默认设置))
 2. The underlying block device must support atomic writes
    (底层块设备必须支持原子写)
 3. For single-fsblock atomic writes:
    (对于单 FS 块原子写：)

    1. A filesystem with appropriate block size (up to the page size)
       (具有适当块大小(最大到页大小)的文件系统)
 4. For multi-fsblock atomic writes:
    (对于多 FS 块原子写：)

    1. The bigalloc feature must be enabled
       (必须启用 bigalloc 特性)
    2. The cluster size must be appropriately configured
       (必须适当地配置簇大小)

NOTE: EXT4 does not support software or COW based atomic write, which means
atomic writes on ext4 are only supported if underlying storage device supports
it.
(注意：EXT4 不支持基于软件或 COW 的原子写，这意味着只有当底层存储设备支持原子写时，ext4 上的原子写才受支持。)

Multi-fsblock Implementation Details
(多 FS 块实现细节)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The bigalloc feature changes ext4 to allocate in units of multiple filesystem
blocks, also known as clusters. With bigalloc each bit within block bitmap
represents a cluster (power of 2 number of blocks) rather than individual
filesystem blocks.
EXT4 supports multi-fsblock atomic writes with bigalloc, subject to the
following constraints. The minimum atomic write size is the larger of the fs
block size and the minimum hardware atomic write unit; and the maximum atomic
write size is smaller of the bigalloc cluster size and the maximum hardware
atomic write unit.  Bigalloc ensures that all allocations are aligned to the
cluster size, which satisfies the LBA alignment requirements of the hardware
device if the start of the partition/logical volume is itself aligned correctly.
(bigalloc 特性将 ext4 改为以多个文件系统块为单位进行分配，这也被称为簇。使用 bigalloc 时，块位图中的每一位代表一个簇(2 的幂次个块)，而不是单个文件系统块。EXT4 在满足以下约束的前提下支持带有 bigalloc 的多 FS 块原子写。最小原子写大小是 fs 块大小与最小硬件原子写单元两者中较大者；最大原子写大小是 bigalloc 簇大小与最大硬件原子写单元两者中较小者。bigalloc 确保所有分配都按簇大小对齐，如果分区/逻辑卷的起始位置本身对齐正确，这将满足硬件设备的 LBA 对齐要求。)

Here is the block allocation strategy in bigalloc for atomic writes:
(以下是 bigalloc 中针对原子写的块分配策略：)

 * For regions with fully mapped extents, no additional work is needed
   (对于完全已映射的 extent 区域，不需要额外的工作)
 * For append writes, a new mapped extent is allocated
   (对于追加写，会分配一个新的已映射 extent)
 * For regions that are entirely holes, unwritten extent is created
   (对于完全是洞的区域，会创建一个未写入的 extent)
 * For large unwritten extents, the extent gets split into two unwritten
   extents of appropriate requested size
   (对于大的未写入 extent，该 extent 会被拆分成两个具有适当请求大小的未写入 extent)
 * For mixed mapping regions (combinations of holes, unwritten extents, or
   mapped extents), ext4_map_blocks() is called in a loop with
   EXT4_GET_BLOCKS_ZERO flag to convert the region into a single contiguous
   mapped extent by writing zeroes to it and converting any unwritten extents to
   written, if found within the range.
   (对于混合映射区域(洞、未写入 extent 或已映射 extent 的组合)，会在循环中调用带有 EXT4_GET_BLOCKS_ZERO 标志的 ext4_map_blocks()，通过向该区域写入零并把在该范围内找到的任何未写入 extent 转换为已写入，将该区域转换为单个连续的已映射 extent。)

Note: Writing on a single contiguous underlying extent, whether mapped or
unwritten, is not inherently problematic. However, writing to a mixed mapping
region (i.e. one containing a combination of mapped and unwritten extents)
must be avoided when performing atomic writes.
(注意：在单个连续的底层 extent(无论是已映射的还是未写入的)上写入本身并没有问题。但是，在执行原子写时必须避免向混合映射区域(即包含已映射与未写入 extent 组合的区域)写入。)

The reason is that, atomic writes when issued via pwritev2() with the RWF_ATOMIC
flag, requires that either all data is written or none at all. In the event of
a system crash or unexpected power loss during the write operation, the affected
region (when later read) must reflect either the complete old data or the
complete new data, but never a mix of both.
(原因在于，通过带有 RWF_ATOMIC 标志的 pwritev2() 发出的原子写，要求要么写入所有数据，要么完全不写。如果在写操作期间发生系统崩溃或意外掉电，受影响区域(在之后读取时)必须反映完整的旧数据或完整的新数据，而绝不能是两者的混合。)

To enforce this guarantee, we ensure that the write target is backed by
a single, contiguous extent before any data is written. This is critical because
ext4 defers the conversion of unwritten extents to written extents until the I/O
completion path (typically in ->end_io()). If a write is allowed to proceed over
a mixed mapping region (with mapped and unwritten extents) and a failure occurs
mid-write, the system could observe partially updated regions after reboot, i.e.
new data over mapped areas, and stale (old) data over unwritten extents that
were never marked written. This violates the atomicity and/or torn write
prevention guarantee.
(为了强制实现这一保证，我们在写入任何数据之前确保写目标由单个连续的 extent 支撑。这一点至关重要，因为 ext4 将未写入 extent 到已写入 extent 的转换推迟到 I/O 完成路径(通常在 ->end_io() 中)才进行。如果允许一次写跨越混合映射区域(同时包含已映射与未写入 extent)并在写过程中发生失败，系统在重启后可能观察到部分更新的区域，即已映射区域上是新数据，而从未被标记为已写入的未写入 extent 上是旧的陈旧数据。这违反了原子性和/或撕裂写防护保证。)

To prevent such torn writes, ext4 proactively allocates a single contiguous
extent for the entire requested region in ``ext4_iomap_alloc`` via
``ext4_map_blocks_atomic()``. EXT4 also force commits the current journalling
transaction in case if allocation is done over mixed mapping. This ensures any
pending metadata updates (like unwritten to written extents conversion) in this
range are in consistent state with the file data blocks, before performing the
actual write I/O. If the commit fails, the whole I/O must be aborted to prevent
from any possible torn writes.
Only after this step, the actual data write operation is performed by the iomap.
(为了防止这种撕裂写，ext4 会在 ``ext4_iomap_alloc`` 中通过 ``ext4_map_blocks_atomic()`` 主动为整个请求区域分配单个连续的 extent。如果是在混合映射之上进行分配，EXT4 还会强制提交当前的日志事务。这确保该范围内任何待处理的元数据更新(例如未写入到已写入 extent 的转换)在执行实际写 I/O 之前与文件数据块处于一致状态。如果提交失败，则必须中止整个 I/O，以防止任何可能的撕裂写。只有在此步骤之后，iomap 才执行实际的数据写操作。)

Handling Split Extents Across Leaf Blocks
(处理跨叶块的拆分 extent)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There can be a special edge case where we have logically and physically
contiguous extents stored in separate leaf nodes of the on-disk extent tree.
This occurs because on-disk extent tree merges only happens within the leaf
blocks except for a case where we have 2-level tree which can get merged and
collapsed entirely into the inode.
If such a layout exists and, in the worst case, the extent status cache entries
are reclaimed due to memory pressure, ``ext4_map_blocks()`` may never return
a single contiguous extent for these split leaf extents.
(可能存在一种特殊的边缘情况：逻辑上和物理上都连续的 extent 被存储在磁盘上 extent 树的不同叶节点中。之所以会出现这种情况，是因为磁盘上 extent 树的合并只发生在叶块之内，除非是 2 层树的情形，此时树可以被合并并完全折叠进 inode。如果存在这样的布局，并且在最坏情况下 extent 状态缓存条目因内存压力被回收，``ext4_map_blocks()`` 可能永远无法为这些拆分到不同叶块的 extent 返回单个连续的 extent。)

To address this edge case, a new get block flag
``EXT4_GET_BLOCKS_QUERY_LEAF_BLOCKS flag`` is added to enhance the
``ext4_map_query_blocks()`` lookup behavior.
(为了处理这种边缘情况，添加了一个新的 get block 标志 ``EXT4_GET_BLOCKS_QUERY_LEAF_BLOCKS flag``，以增强 ``ext4_map_query_blocks()`` 的查找行为。)

This new get block flag allows ``ext4_map_blocks()`` to first check if there is
an entry in the extent status cache for the full range.
If not present, it consults the on-disk extent tree using
``ext4_map_query_blocks()``.
If the located extent is at the end of a leaf node, it probes the next logical
block (lblk) to detect a contiguous extent in the adjacent leaf.
(这个新的 get block 标志允许 ``ext4_map_blocks()`` 先检查 extent 状态缓存中是否存在针对完整范围的条目。如果不存在，它使用 ``ext4_map_query_blocks()`` 查询磁盘上的 extent 树。如果定位到的 extent 位于某个叶节点的末尾，它会探测下一个逻辑块(lblk)以检测相邻叶块中是否存在连续的 extent。)

For now only one additional leaf block is queried to maintain efficiency, as
atomic writes are typically constrained to small sizes
(e.g. [blocksize, clustersize]).
(目前为了保持效率，只额外查询一个叶块，因为原子写通常被限制在小尺寸(例如 [blocksize, clustersize])。)


Handling Journal transactions
(处理日志事务)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To support multi-fsblock atomic writes, we ensure enough journal credits are
reserved during:
(为了支持多 FS 块原子写，我们确保在以下期间保留了足够的日志额度：)

 1. Block allocation time in ``ext4_iomap_alloc()``. We first query if there
    could be a mixed mapping for the underlying requested range. If yes, then we
    reserve credits of up to ``m_len``, assuming every alternate block can be
    an unwritten extent followed by a hole.
    (``ext4_iomap_alloc()`` 中的块分配时间。我们首先查询底层请求范围是否可能存在混合映射。如果是，那么我们保留最多 ``m_len`` 的额度，假设每隔一个块就可能是一个未写入 extent 后跟一个洞。)

 2. During ``->end_io()`` call, we make sure a single transaction is started for
    doing unwritten-to-written conversion. The loop for conversion is mainly
    only required to handle a split extent across leaf blocks.
    (在 ``->end_io()`` 调用期间，我们确保为进行未写入到已写入的转换启动单个事务。该转换循环主要是为了处理跨叶块的拆分 extent。)

How to
(如何操作)
~~~~~~

Creating Filesystems with Atomic Write Support
(创建支持原子写的文件系统)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First check the atomic write units supported by block device.
See :ref:`atomic_write_bdev_support` for more details.
(首先检查块设备支持的原子写单元。更多细节参见 :ref:`atomic_write_bdev_support`。)

For single-fsblock atomic writes with a larger block size
(on systems with block size < page size):
(对于使用较大块大小的单 FS 块原子写(在块大小 < 页大小的系统上)：)

.. code-block:: bash

    # Create an ext4 filesystem with a 16KB block size
    # (requires page size >= 16KB)
    mkfs.ext4 -b 16384 /dev/device

For multi-fsblock atomic writes with bigalloc:
(对于使用 bigalloc 的多 FS 块原子写：)

.. code-block:: bash

    # Create an ext4 filesystem with bigalloc and 64KB cluster size
    mkfs.ext4 -F -O bigalloc -b 4096 -C 65536 /dev/device

Where ``-b`` specifies the block size, ``-C`` specifies the cluster size in bytes,
and ``-O bigalloc`` enables the bigalloc feature.
(其中 ``-b`` 指定块大小，``-C`` 以字节为单位指定簇大小，而 ``-O bigalloc`` 启用 bigalloc 特性。)

Application Interface
(应用接口)
^^^^^^^^^^^^^^^^^^^^^

Applications can use the ``pwritev2()`` system call with the ``RWF_ATOMIC`` flag
to perform atomic writes:
(应用程序可以使用带有 ``RWF_ATOMIC`` 标志的 ``pwritev2()`` 系统调用来执行原子写：)

.. code-block:: c

    pwritev2(fd, iov, iovcnt, offset, RWF_ATOMIC);

The write must be aligned to the filesystem's block size and not exceed the
filesystem's maximum atomic write unit size.
See ``generic_atomic_write_valid()`` for more details.
(写必须按文件系统的块大小对齐，且不得超过文件系统的最大原子写单元大小。更多细节参见 ``generic_atomic_write_valid()``。)

``statx()`` system call with ``STATX_WRITE_ATOMIC`` flag can provide following
details:
(带有 ``STATX_WRITE_ATOMIC`` 标志的 ``statx()`` 系统调用可以提供以下细节：)

 * ``stx_atomic_write_unit_min``: Minimum size of an atomic write request.
   (``stx_atomic_write_unit_min``：原子写请求的最小大小。)
 * ``stx_atomic_write_unit_max``: Maximum size of an atomic write request.
   (``stx_atomic_write_unit_max``：原子写请求的最大大小。)
 * ``stx_atomic_write_segments_max``: Upper limit for segments. The number of
   separate memory buffers that can be gathered into a write operation
   (e.g., the iovcnt parameter for IOV_ITER). Currently, this is always set to one.
   (``stx_atomic_write_segments_max``：段数的上限。可以被聚集到一个写操作中的独立内存缓冲区的数量(例如 IOV_ITER 的 iovcnt 参数)。目前，它总是被设置为 1。)

The STATX_ATTR_WRITE_ATOMIC flag in ``statx->attributes`` is set if atomic
writes are supported.
(如果支持原子写，``statx->attributes`` 中的 STATX_ATTR_WRITE_ATOMIC 标志会被置位。)

.. _atomic_write_bdev_support:

Hardware Support
(硬件支持)
~~~~~~~~~~~~~~~~

The underlying storage device must support atomic write operations.
Modern NVMe and SCSI devices often provide this capability.
The Linux kernel exposes this information through sysfs:
(底层存储设备必须支持原子写操作。现代的 NVMe 和 SCSI 设备通常提供此能力。Linux 内核通过 sysfs 公开这些信息：)

* ``/sys/block/<device>/queue/atomic_write_unit_min`` - Minimum atomic write size
  (``/sys/block/<device>/queue/atomic_write_unit_min`` - 最小原子写大小)
* ``/sys/block/<device>/queue/atomic_write_unit_max`` - Maximum atomic write size
  (``/sys/block/<device>/queue/atomic_write_unit_max`` - 最大原子写大小)

Nonzero values for these attributes indicate that the device supports
atomic writes.
(这些属性的非零值表明该设备支持原子写。)

See Also
(另请参阅)
~~~~~~~~

* :doc:`bigalloc` - Documentation on the bigalloc feature
  (:doc:`bigalloc` - 关于 bigalloc 特性的文档)
* :doc:`allocators` - Documentation on block allocation in ext4
  (:doc:`allocators` - 关于 ext4 中块分配的文档)
* Support for atomic block writes in 6.13:
  https://lwn.net/Articles/1009298/
  (6.13 中对原子块写的支持：https://lwn.net/Articles/1009298/)
