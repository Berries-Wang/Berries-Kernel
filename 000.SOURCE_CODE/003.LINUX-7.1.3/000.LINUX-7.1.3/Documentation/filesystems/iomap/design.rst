.. SPDX-License-Identifier: GPL-2.0
.. _iomap_design:

..
        Dumb style notes to maintain the author's sanity:
        Please try to start sentences on separate lines so that
        sentence changes don't bleed colors in diff.
        Heading decorations are documented in sphinx.rst.

==============
Library Design
==============

(库设计)

.. contents:: Table of Contents
   :local:

Introduction
(简介)
============

iomap is a filesystem library for handling common file operations.
The library has two layers:
(iomap 是一个用于处理常见文件操作的文件系统库。该库分为两层：)

 1. A lower layer that provides an iterator over ranges of file offsets.
    This layer tries to obtain mappings of each file ranges to storage
    from the filesystem, but the storage information is not necessarily
    required.
    (较低层为文件偏移量的范围提供一个迭代器。该层尝试从文件系统获取每个文件范围到存储的映射，但不一定需要存储信息。)

 2. An upper layer that acts upon the space mappings provided by the
    lower layer iterator.
    (上层作用于由较低层迭代器提供的空间映射。)

The iteration can involve mappings of file's logical offset ranges to
physical extents, but the storage layer information is not necessarily
required, e.g. for walking cached file information.
The library exports various APIs for implementing file operations such
as:
(迭代可以涉及文件的逻辑偏移量范围到物理段的映射，但不一定需要存储层信息，例如用于遍历缓存的文件信息。该库导出多种 API 用于实现文件操作，例如：)

 * Pagecache reads and writes
   (页缓存读写)
 * Folio write faults to the pagecache
   (向页缓存发起的 folio 写缺页)
 * Writeback of dirty folios
   (脏 folio 的回写)
 * Direct I/O reads and writes
   (直接 I/O 读写)
 * fsdax I/O reads, writes, loads, and stores
   (fsdax I/O 读、写、加载和存储)
 * FIEMAP
   (FIEMAP)
 * lseek ``SEEK_DATA`` and ``SEEK_HOLE``
   (lseek ``SEEK_DATA`` 和 ``SEEK_HOLE``)
 * swapfile activation
   (交换文件激活)

This origins of this library is the file I/O path that XFS once used; it
has now been extended to cover several other operations.
(本库起源于 XFS 曾经使用的文件 I/O 路径；如今它已被扩展到涵盖若干其他操作。)

Who Should Read This?
(谁应该阅读本文？)
=====================

The target audience for this document are filesystem, storage, and
pagecache programmers and code reviewers.
(本文档的目标读者是文件系统、存储和页缓存方面的程序员以及代码评审者。)

If you are working on PCI, machine architectures, or device drivers, you
are most likely in the wrong place.
(如果你从事的是 PCI、机器体系结构或设备驱动方面的工作，那么你很可能来错了地方。)

How Is This Better?
(这为何更好？)
===================

Unlike the classic Linux I/O model which breaks file I/O into small
units (generally memory pages or blocks) and looks up space mappings on
the basis of that unit, the iomap model asks the filesystem for the
largest space mappings that it can create for a given file operation and
initiates operations on that basis.
This strategy improves the filesystem's visibility into the size of the
operation being performed, which enables it to combat fragmentation with
larger space allocations when possible.
Larger space mappings improve runtime performance by amortizing the cost
of mapping function calls into the filesystem across a larger amount of
data.
(与经典的 Linux I/O 模型不同——后者将文件 I/O 拆分为小单元(通常是内存页或块)并基于该单元查找空间映射——iomap 模型会向文件系统请求它能为给定文件操作创建的最大空间映射，并在此基础上发起操作。该策略提高了文件系统对所执行操作大小的可见性，使其能够在可能的情况下通过更大的空间分配来对抗碎片化。更大的空间映射通过将映射函数调用进入文件系统的开销分摊到更大量的数据上，从而提高运行时性能。)

At a high level, an iomap operation `looks like this
<https://lore.kernel.org/all/ZGbVaewzcCysclPt@dread.disaster.area/>`_:
(从高层次上看，一个 iomap 操作`看起来像这样
<https://lore.kernel.org/all/ZGbVaewzcCysclPt@dread.disaster.area/>`_:)

1. For each byte in the operation range...
   (对操作范围内的每个字节执行以下操作……)

   1. Obtain a space mapping via ``->iomap_begin``
      (通过 ``->iomap_begin`` 获取一个空间映射)

   2. For each sub-unit of work...
      (对每个工作子单元执行以下操作……)

      1. Revalidate the mapping and go back to (1) above, if necessary.
         So far only the pagecache operations need to do this.
         (必要时重新验证该映射，并视需要返回上面的(1)。到目前为止只有页缓存操作需要这样做。)

      2. Do the work
         (执行该工作)

   3. Increment operation cursor
      (递增操作游标)

   4. Release the mapping via ``->iomap_end``, if necessary
      (必要时通过 ``->iomap_end`` 释放该映射)

Each iomap operation will be covered in more detail below.
This library was covered previously by an `LWN article
<https://lwn.net/Articles/935934/>`_ and a `KernelNewbies page
<https://kernelnewbies.org/KernelProjects/iomap>`_.
(每个 iomap 操作将在下文更详细地介绍。本库此前已有一篇 LWN 文章
<https://lwn.net/Articles/935934/> 和一个 KernelNewbies 页面
<https://kernelnewbies.org/KernelProjects/iomap> 介绍过。)

The goal of this document is to provide a brief discussion of the
design and capabilities of iomap, followed by a more detailed catalog
of the interfaces presented by iomap.
If you change iomap, please update this design document.
(本文档的目标是先简要讨论 iomap 的设计与能力，然后更详细地列出 iomap 所提供的接口。如果你修改了 iomap，请更新这份设计文档。)

File Range Iterator
(文件范围迭代器)
===================

Definitions
(定义)
-----------

 * **buffer head**: Shattered remnants of the old buffer cache.
   (**buffer head**: 旧缓冲缓存的残存碎片。)

 * ``fsblock``: The block size of a file, also known as ``i_blocksize``.
   (``fsblock``: 文件的块大小，也称为 ``i_blocksize``。)

 * ``i_rwsem``: The VFS ``struct inode`` rwsemaphore.
   Processes hold this in shared mode to read file state and contents.
   Some filesystems may allow shared mode for writes.
   Processes often hold this in exclusive mode to change file state and
   contents.
   (``i_rwsem``: VFS ``struct inode`` 的读写信号量。进程以共享模式持有它来读取文件状态和内容。某些文件系统可能允许以共享模式进行写。进程通常以独占模式持有它来更改文件状态和内容。)

 * ``invalidate_lock``: The pagecache ``struct address_space``
   rwsemaphore that protects against folio insertion and removal for
   filesystems that support punching out folios below EOF.
   Processes wishing to insert folios must hold this lock in shared
   mode to prevent removal, though concurrent insertion is allowed.
   Processes wishing to remove folios must hold this lock in exclusive
   mode to prevent insertions.
   Concurrent removals are not allowed.
   (``invalidate_lock``: 页缓存 ``struct address_space`` 的读写信号量，对于支持在 EOF 之下打孔移除 folio 的文件系统，它防止 folio 的插入与移除相互冲突。希望插入 folio 的进程必须以共享模式持有此锁以防移除，不过允许并发插入。希望移除 folio 的进程必须以独占模式持有此锁以防插入。不允许并发移除。)

 * ``dax_read_lock``: The RCU read lock that dax takes to prevent a
   device pre-shutdown hook from returning before other threads have
   released resources.
   (``dax_read_lock``: dax 获取的 RCU 读锁，用于防止设备预关闭钩子在其他线程释放资源之前返回。)

 * **filesystem mapping lock**: This synchronization primitive is
   internal to the filesystem and must protect the file mapping data
   from updates while a mapping is being sampled.
   The filesystem author must determine how this coordination should
   happen; it does not need to be an actual lock.
   (**filesystem mapping lock**: 该同步原语位于文件系统内部，必须在采样映射期间保护文件映射数据不被更新。文件系统作者必须决定这种协调应如何进行；它不必是一把真正的锁。)

 * **iomap internal operation lock**: This is a general term for
   synchronization primitives that iomap functions take while holding a
   mapping.
   A specific example would be taking the folio lock while reading or
   writing the pagecache.
   (**iomap internal operation lock**: 这是对 iomap 函数在持有映射期间获取的同步原语的统称。一个具体的例子是在读或写页缓存时获取 folio 锁。)

 * **pure overwrite**: A write operation that does not require any
   metadata or zeroing operations to perform during either submission
   or completion.
   This implies that the filesystem must have already allocated space
   on disk as ``IOMAP_MAPPED`` and the filesystem must not place any
   constraints on IO alignment or size.
   The only constraints on I/O alignment are device level (minimum I/O
   size and alignment, typically sector size).
   (**pure overwrite**: 一种在提交或完成阶段都不需要执行任何元数据或清零操作的写操作。这意味着文件系统必须已经以 ``IOMAP_MAPPED`` 的形式在磁盘上分配了空间，且文件系统不得对 I/O 对齐或大小施加任何约束。对 I/O 对齐的唯一约束来自设备层(最小 I/O 大小和对齐，通常是扇区大小)。)

``struct iomap``
(``struct iomap``)
----------------

The filesystem communicates to the iomap iterator the mapping of
byte ranges of a file to byte ranges of a storage device with the
structure below:
(文件系统通过下面的结构体，将文件字节范围到存储设备字节范围的映射告知给 iomap 迭代器：)

.. code-block:: c

 struct iomap {
     u64                 addr;
     loff_t              offset;
     u64                 length;
     u16                 type;
     u16                 flags;
     struct block_device *bdev;
     struct dax_device   *dax_dev;
     void                *inline_data;
     void                *private;
     u64                 validity_cookie;
 };

The fields are as follows:
(各字段含义如下：)

 * ``offset`` and ``length`` describe the range of file offsets, in
   bytes, covered by this mapping.
   These fields must always be set by the filesystem.
   (``offset`` 和 ``length`` 描述此映射所覆盖的文件偏移量范围，以字节为单位。这些字段必须始终由文件系统设置。)

 * ``type`` describes the type of the space mapping:
   (``type`` 描述空间映射的类型：)

   * **IOMAP_HOLE**: No storage has been allocated.
     This type must never be returned in response to an ``IOMAP_WRITE``
     operation because writes must allocate and map space, and return
     the mapping.
     The ``addr`` field must be set to ``IOMAP_NULL_ADDR``.
     iomap does not support writing (whether via pagecache or direct
     I/O) to a hole.
     (**IOMAP_HOLE**: 尚未分配任何存储。绝不能在响应 ``IOMAP_WRITE`` 操作时返回此类型，因为写操作必须分配并映射空间，并返回该映射。``addr`` 字段必须设置为 ``IOMAP_NULL_ADDR``。iomap 不支持向空洞写入(无论是通过页缓存还是直接 I/O)。)

   * **IOMAP_DELALLOC**: A promise to allocate space at a later time
     ("delayed allocation").
     If the filesystem returns IOMAP_F_NEW here and the write fails, the
     ``->iomap_end`` function must delete the reservation.
     The ``addr`` field must be set to ``IOMAP_NULL_ADDR``.
     (**IOMAP_DELALLOC**: 承诺在稍后分配空间("延迟分配")。如果文件系统在此处返回 IOMAP_F_NEW 而写操作失败，``->iomap_end`` 函数必须删除该预留。``addr`` 字段必须设置为 ``IOMAP_NULL_ADDR``。)

   * **IOMAP_MAPPED**: The file range maps to specific space on the
     storage device.
     The device is returned in ``bdev`` or ``dax_dev``.
     The device address, in bytes, is returned via ``addr``.
     (**IOMAP_MAPPED**: 文件范围映射到存储设备上的特定空间。设备通过 ``bdev`` 或 ``dax_dev`` 返回。设备地址以字节为单位通过 ``addr`` 返回。)

   * **IOMAP_UNWRITTEN**: The file range maps to specific space on the
     storage device, but the space has not yet been initialized.
     The device is returned in ``bdev`` or ``dax_dev``.
     The device address, in bytes, is returned via ``addr``.
     Reads from this type of mapping will return zeroes to the caller.
     For a write or writeback operation, the ioend should update the
     mapping to MAPPED.
     Refer to the sections about ioends for more details.
     (**IOMAP_UNWRITTEN**: 文件范围映射到存储设备上的特定空间，但该空间尚未初始化。设备通过 ``bdev`` 或 ``dax_dev`` 返回。设备地址以字节为单位通过 ``addr`` 返回。从此类映射读取将向调用者返回零。对于写或回写操作，ioend 应将该映射更新为 MAPPED。更多细节请参阅关于 ioend 的章节。)

   * **IOMAP_INLINE**: The file range maps to the memory buffer
     specified by ``inline_data``.
     For write operation, the ``->iomap_end`` function presumably
     handles persisting the data.
     The ``addr`` field must be set to ``IOMAP_NULL_ADDR``.
     (**IOMAP_INLINE**: 文件范围映射到 ``inline_data`` 指定的内存缓冲。对于写操作，``->iomap_end`` 函数大概会负责持久化这些数据。``addr`` 字段必须设置为 ``IOMAP_NULL_ADDR``。)

 * ``flags`` describe the status of the space mapping.
   These flags should be set by the filesystem in ``->iomap_begin``:
   (``flags`` 描述空间映射的状态。这些标志应由文件系统在 ``->iomap_begin`` 中设置：)

   * **IOMAP_F_NEW**: The space under the mapping is newly allocated.
     Areas that will not be written to must be zeroed.
     If a write fails and the mapping is a space reservation, the
     reservation must be deleted.
     (**IOMAP_F_NEW**: 映射下的空间是新分配的。不会被写入的区域必须清零。如果写操作失败且该映射是一个空间预留，则必须删除该预留。)

   * **IOMAP_F_DIRTY**: The inode will have uncommitted metadata needed
     to access any data written.
     fdatasync is required to commit these changes to persistent
     storage.
     This needs to take into account metadata changes that *may* be made
     at I/O completion, such as file size updates from direct I/O.
     (**IOMAP_F_DIRTY**: inode 将持有访问任何已写入数据所需的未提交元数据。需要 fdatasync 才能将这些更改提交到持久存储。这需要考虑到*可能*在 I/O 完成时进行的元数据更改，例如来自直接 I/O 的文件大小更新。)

   * **IOMAP_F_SHARED**: The space under the mapping is shared.
     Copy on write is necessary to avoid corrupting other file data.
     (**IOMAP_F_SHARED**: 映射下的空间是共享的。需要写时复制以避免破坏其他文件数据。)

   * **IOMAP_F_BUFFER_HEAD**: This mapping requires the use of buffer
     heads for pagecache operations.
     Do not add more uses of this.
     (**IOMAP_F_BUFFER_HEAD**: 此映射要求在页缓存操作中使用 buffer head。不要再为此添加更多用例。)

   * **IOMAP_F_MERGED**: Multiple contiguous block mappings were
     coalesced into this single mapping.
     This is only useful for FIEMAP.
     (**IOMAP_F_MERGED**: 多个连续的块映射被合并为这一个映射。这仅对 FIEMAP 有用。)

   * **IOMAP_F_XATTR**: The mapping is for extended attribute data, not
     regular file data.
     This is only useful for FIEMAP.
     (**IOMAP_F_XATTR**: 此映射针对的是扩展属性数据，而不是常规文件数据。这仅对 FIEMAP 有用。)

   * **IOMAP_F_BOUNDARY**: This indicates I/O and its completion must not be
     merged with any other I/O or completion. Filesystems must use this when
     submitting I/O to devices that cannot handle I/O crossing certain LBAs
     (e.g. ZNS devices). This flag applies only to buffered I/O writeback; all
     other functions ignore it.
     (**IOMAP_F_BOUNDARY**: 此标志表明该 I/O 及其完成不得与任何其他 I/O 或完成相合并。在向无法处理跨越某些 LBA 的 I/O 的设备(例如 ZNS 设备)提交 I/O 时，文件系统必须使用此标志。此标志仅适用于缓冲 I/O 回写；所有其他函数都会忽略它。)

   * **IOMAP_F_PRIVATE**: This flag is reserved for filesystem private use.
     (**IOMAP_F_PRIVATE**: 此标志保留供文件系统私有使用。)

   * **IOMAP_F_ANON_WRITE**: Indicates that (write) I/O does not have a target
     block assigned to it yet and the file system will do that in the bio
     submission handler, splitting the I/O as needed.
     (**IOMAP_F_ANON_WRITE**: 表明(写)I/O 尚未有分配给它的目标块，文件系统将在 bio 提交处理程序中完成此事，并按需拆分 I/O。)

   * **IOMAP_F_ATOMIC_BIO**: This indicates write I/O must be submitted with the
     ``REQ_ATOMIC`` flag set in the bio. Filesystems need to set this flag to
     inform iomap that the write I/O operation requires torn-write protection
     based on HW-offload mechanism. They must also ensure that mapping updates
     upon the completion of the I/O must be performed in a single metadata
     update.
     (**IOMAP_F_ATOMIC_BIO**: 此标志表明写 I/O 必须在设置了 ``REQ_ATOMIC`` 标志的 bio 中提交。文件系统需要设置此标志以告知 iomap：该写 I/O 操作需要基于硬件卸载机制的 torn-write 保护。它们还必须确保在 I/O 完成时进行的映射更新必须在单次元数据更新中完成。)

   These flags can be set by iomap itself during file operations.
   The filesystem should supply an ``->iomap_end`` function if it needs
   to observe these flags:
   (这些标志可以由 iomap 本身在文件操作期间设置。如果文件系统需要观察这些标志，则应提供 ``->iomap_end`` 函数：)

   * **IOMAP_F_SIZE_CHANGED**: The file size has changed as a result of
     using this mapping.
     (**IOMAP_F_SIZE_CHANGED**: 由于使用了此映射，文件大小发生了变化。)

   * **IOMAP_F_STALE**: The mapping was found to be stale.
     iomap will call ``->iomap_end`` on this mapping and then
     ``->iomap_begin`` to obtain a new mapping.
     (**IOMAP_F_STALE**: 发现该映射已过期。iomap 将对此映射调用 ``->iomap_end``，然后调用 ``->iomap_begin`` 以获取新映射。)

   Currently, these flags are only set by pagecache operations.
   (目前，这些标志仅由页缓存操作设置。)

 * ``addr`` describes the device address, in bytes.
   (``addr`` 描述设备地址，以字节为单位。)

 * ``bdev`` describes the block device for this mapping.
   This only needs to be set for mapped or unwritten operations.
   (``bdev`` 描述此映射的块设备。只需为 mapped 或 unwritten 操作设置。)

 * ``dax_dev`` describes the DAX device for this mapping.
   This only needs to be set for mapped or unwritten operations, and
   only for a fsdax operation.
   (``dax_dev`` 描述此映射的 DAX 设备。只需为 mapped 或 unwritten 操作设置，并且仅用于 fsdax 操作。)

 * ``inline_data`` points to a memory buffer for I/O involving
   ``IOMAP_INLINE`` mappings.
   This value is ignored for all other mapping types.
   (``inline_data`` 指向用于涉及 ``IOMAP_INLINE`` 映射的 I/O 的内存缓冲。对于所有其他映射类型，此值被忽略。)

 * ``private`` is a pointer to `filesystem-private information
   <https://lore.kernel.org/all/20180619164137.13720-7-hch@lst.de/>`_.
   This value will be passed unchanged to ``->iomap_end``.
   (``private`` 是指向`文件系统私有信息
   <https://lore.kernel.org/all/20180619164137.13720-7-hch@lst.de/>`_ 的指针。此值将原封不动地传递给 ``->iomap_end``。)

 * ``validity_cookie`` is a magic freshness value set by the filesystem
   that should be used to detect stale mappings.
   For pagecache operations this is critical for correct operation
   because page faults can occur, which implies that filesystem locks
   should not be held between ``->iomap_begin`` and ``->iomap_end``.
   Filesystems with completely static mappings need not set this value.
   Only pagecache operations revalidate mappings; see the section about
   ``iomap_valid`` for details.
   (``validity_cookie`` 是由文件系统设置的一个用于检测过期映射的神奇新鲜度值。对于页缓存操作，它对正确运行至关重要，因为可能发生缺页，这意味着在 ``->iomap_begin`` 和 ``->iomap_end`` 之间不应持有文件系统锁。具有完全静态映射的文件系统无需设置此值。只有页缓存操作会重新验证映射；详情请参见关于 ``iomap_valid`` 的章节。)

``struct iomap_ops``
(``struct iomap_ops``)
--------------------

Every iomap function requires the filesystem to pass an operations
structure to obtain a mapping and (optionally) to release the mapping:
(每个 iomap 函数都要求文件系统传入一个操作结构体，用于获取映射以及(可选地)释放映射：)

.. code-block:: c

 struct iomap_ops {
     int (*iomap_begin)(struct inode *inode, loff_t pos, loff_t length,
                        unsigned flags, struct iomap *iomap,
                        struct iomap *srcmap);

     int (*iomap_end)(struct inode *inode, loff_t pos, loff_t length,
                      ssize_t written, unsigned flags,
                      struct iomap *iomap);
 };

``->iomap_begin``
(``->iomap_begin``)
~~~~~~~~~~~~~~~~~

iomap operations call ``->iomap_begin`` to obtain one file mapping for
the range of bytes specified by ``pos`` and ``length`` for the file
``inode``.
This mapping should be returned through the ``iomap`` pointer.
The mapping must cover at least the first byte of the supplied file
range, but it does not need to cover the entire requested range.
(iomap 操作调用 ``->iomap_begin`` 来为文件 ``inode`` 获取由 ``pos`` 和 ``length`` 指定的字节范围的一个文件映射。该映射应通过 ``iomap`` 指针返回。映射必须至少覆盖所给定文件范围的第一个字节，但不必覆盖整个请求的范围。)

Each iomap operation describes the requested operation through the
``flags`` argument.
The exact value of ``flags`` will be documented in the
operation-specific sections below.
These flags can, at least in principle, apply generally to iomap
operations:
(每个 iomap 操作通过 ``flags`` 参数描述所请求的操作。``flags`` 的确切取值将在下文的操作专属章节中文档化。这些标志至少在原则上可以普遍适用于 iomap 操作：)

 * ``IOMAP_DIRECT`` is set when the caller wishes to issue file I/O to
   block storage.
   (当调用者希望向块存储发出文件 I/O 时，会设置 ``IOMAP_DIRECT``。)

 * ``IOMAP_DAX`` is set when the caller wishes to issue file I/O to
   memory-like storage.
   (当调用者希望向类内存存储发出文件 I/O 时，会设置 ``IOMAP_DAX``。)

 * ``IOMAP_NOWAIT`` is set when the caller wishes to perform a best
   effort attempt to avoid any operation that would result in blocking
   the submitting task.
   This is similar in intent to ``O_NONBLOCK`` for network APIs - it is
   intended for asynchronous applications to keep doing other work
   instead of waiting for the specific unavailable filesystem resource
   to become available.
   Filesystems implementing ``IOMAP_NOWAIT`` semantics need to use
   trylock algorithms.
   They need to be able to satisfy the entire I/O request range with a
   single iomap mapping.
   They need to avoid reading or writing metadata synchronously.
   They need to avoid blocking memory allocations.
   They need to avoid waiting on transaction reservations to allow
   modifications to take place.
   They probably should not be allocating new space.
   And so on.
   If there is any doubt in the filesystem developer's mind as to
   whether any specific ``IOMAP_NOWAIT`` operation may end up blocking,
   then they should return ``-EAGAIN`` as early as possible rather than
   start the operation and force the submitting task to block.
   ``IOMAP_NOWAIT`` is often set on behalf of ``IOCB_NOWAIT`` or
   ``RWF_NOWAIT``.
   (当调用者希望尽最大努力避免任何会导致提交任务阻塞的操作时，会设置 ``IOMAP_NOWAIT``。其意图类似于网络 API 中的 ``O_NONBLOCK``——它旨在让异步应用程序继续做其他工作，而不是等待某个特定而不可用的文件系统资源变为可用。实现 ``IOMAP_NOWAIT`` 语义的文件系统需要使用 trylock 算法。它们需要能够用单个 iomap 映射满足整个 I/O 请求范围。它们需要避免同步地读取或写入元数据。它们需要避免阻塞式的内存分配。它们需要避免等待事务预留以允许修改发生。它们或许不应该分配新空间。诸如此类。如果文件系统开发者对任何特定的 ``IOMAP_NOWAIT`` 操作是否可能最终发生阻塞心存疑虑，那么他们应该尽早返回 ``-EAGAIN``，而不是开始该操作并迫使提交任务阻塞。``IOMAP_NOWAIT`` 通常是代表 ``IOCB_NOWAIT`` 或 ``RWF_NOWAIT`` 而设置的。)

 * ``IOMAP_DONTCACHE`` is set when the caller wishes to perform a
   buffered file I/O and would like the kernel to drop the pagecache
   after the I/O completes, if it isn't already being used by another
   thread.
   (当调用者希望执行缓冲文件 I/O，并希望内核在 I/O 完成后丢弃页缓存(如果它尚未被另一个线程使用)时，会设置 ``IOMAP_DONTCACHE``。)

If it is necessary to read existing file contents from a `different
<https://lore.kernel.org/all/20191008071527.29304-9-hch@lst.de/>`_
device or address range on a device, the filesystem should return that
information via ``srcmap``.
Only pagecache and fsdax operations support reading from one mapping and
writing to another.
(如果需要从设备上`不同的
<https://lore.kernel.org/all/20191008071527.29304-9-hch@lst.de/>`_
设备或地址范围读取现有文件内容，文件系统应通过 ``srcmap`` 返回该信息。只有页缓存和 fsdax 操作支持从一个映射读取并写入另一个映射。)

``->iomap_end``
(``->iomap_end``)
~~~~~~~~~~~~~~~

After the operation completes, the ``->iomap_end`` function, if present,
is called to signal that iomap is finished with a mapping.
Typically, implementations will use this function to tear down any
context that were set up in ``->iomap_begin``.
For example, a write might wish to commit the reservations for the bytes
that were operated upon and unreserve any space that was not operated
upon.
``written`` might be zero if no bytes were touched.
``flags`` will contain the same value passed to ``->iomap_begin``.
iomap ops for reads are not likely to need to supply this function.
(操作完成后，如果存在 ``->iomap_end`` 函数，则会调用它以表明 iomap 已用完该映射。通常，实现会用此函数来拆除在 ``->iomap_begin`` 中建立的任何上下文。例如，写操作可能希望提交对被操作字节的预留，并取消预留任何未被操作的空间。如果没有触碰任何字节，``written`` 可能为零。``flags`` 将包含传递给 ``->iomap_begin`` 的相同值。用于读操作的 iomap ops 不太可能需要提供此函数。)

Both functions should return a negative errno code on error, or zero on
success.
(两个函数在出错时应返回负的 errno 代码，成功时返回零。)

Preparing for File Operations
(为文件操作做准备)
=============================

iomap only handles mapping and I/O.
Filesystems must still call out to the VFS to check input parameters
and file state before initiating an I/O operation.
It does not handle obtaining filesystem freeze protection, updating of
timestamps, stripping privileges, or access control.
(iomap 只处理映射和 I/O。在发起 I/O 操作之前，文件系统仍必须调用 VFS 来检查输入参数和文件状态。它不负责获取文件系统冻结保护、更新时间戳、剥离特权或访问控制。)

Locking Hierarchy
(锁定层级)
=================

iomap requires that filesystems supply their own locking model.
There are three categories of synchronization primitives, as far as
iomap is concerned:
(iomap 要求文件系统提供自己的锁定模型。就 iomap 而言，同步原语有三类：)

 * The **upper** level primitive is provided by the filesystem to
   coordinate access to different iomap operations.
   The exact primitive is specific to the filesystem and operation,
   but is often a VFS inode, pagecache invalidation, or folio lock.
   For example, a filesystem might take ``i_rwsem`` before calling
   ``iomap_file_buffered_write`` and ``iomap_file_unshare`` to prevent
   these two file operations from clobbering each other.
   Pagecache writeback may lock a folio to prevent other threads from
   accessing the folio until writeback is underway.
   (**upper** 层原语由文件系统提供，用于协调对不同 iomap 操作的访问。具体使用哪种原语取决于文件系统和操作，但通常是 VFS inode、页缓存失效或 folio 锁。例如，文件系统可能在调用 ``iomap_file_buffered_write`` 和 ``iomap_file_unshare`` 之前先获取 ``i_rwsem``，以防止这两个文件操作相互破坏。页缓存回写可能会锁定一个 folio，以防止其他线程在回写进行之前访问该 folio。)

   * The **lower** level primitive is taken by the filesystem in the
     ``->iomap_begin`` and ``->iomap_end`` functions to coordinate
     access to the file space mapping information.
     The fields of the iomap object should be filled out while holding
     this primitive.
     The upper level synchronization primitive, if any, remains held
     while acquiring the lower level synchronization primitive.
     For example, XFS takes ``ILOCK_EXCL`` and ext4 takes ``i_data_sem``
     while sampling mappings.
     Filesystems with immutable mapping information may not require
     synchronization here.
     (**lower** 层原语由文件系统在 ``->iomap_begin`` 和 ``->iomap_end`` 函数中获取，用于协调对文件空间映射信息的访问。应在持有此原语期间填写 iomap 对象的各字段。在获取较低层同步原语期间，上层同步原语(如果有)保持持有。例如，XFS 在采样映射时获取 ``ILOCK_EXCL``，而 ext4 获取 ``i_data_sem``。映射信息不可变的文件系统可能不需要在此处进行同步。)

   * The **operation** primitive is taken by an iomap operation to
     coordinate access to its own internal data structures.
     The upper level synchronization primitive, if any, remains held
     while acquiring this primitive.
     The lower level primitive is not held while acquiring this
     primitive.
     For example, pagecache write operations will obtain a file mapping,
     then grab and lock a folio to copy new contents.
     It may also lock an internal folio state object to update metadata.
     (**operation** 原语由 iomap 操作获取，用于协调对其自身内部数据结构的访问。在获取此原语期间，上层同步原语(如果有)保持持有。获取此原语期间不持有较低层原语。例如，页缓存写操作会先获取一个文件映射，然后抓取并锁定一个 folio 以复制新内容。它还可能锁定一个内部 folio 状态对象以更新元数据。)

The exact locking requirements are specific to the filesystem; for
certain operations, some of these locks can be elided.
All further mentions of locking are *recommendations*, not mandates.
Each filesystem author must figure out the locking for themself.
(确切的锁定要求因文件系统而异；对于某些操作，其中一些锁可以被省略。后文所有关于锁定的提及都是*建议*，而非强制要求。每个文件系统作者必须自己弄清楚适合自己的锁定方案。)

Bugs and Limitations
(缺陷与限制)
====================

 * No support for fscrypt.
   (不支持 fscrypt。)
 * No support for compression.
   (不支持压缩。)
 * No support for fsverity yet.
   (尚不支持 fsverity。)
 * Strong assumptions that IO should work the way it does on XFS.
   (强烈假设 I/O 应当像它在 XFS 上那样工作。)
 * Does iomap *actually* work for non-regular file data?
   (iomap 对于非常规文件数据*真的*能工作吗？)

Patches welcome!
(欢迎提交补丁！)
