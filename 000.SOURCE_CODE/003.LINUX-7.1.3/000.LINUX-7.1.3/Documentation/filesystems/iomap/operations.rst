.. SPDX-License-Identifier: GPL-2.0
.. _iomap_operations:

..
        Dumb style notes to maintain the author's sanity:
        Please try to start sentences on separate lines so that
        sentence changes don't bleed colors in diff.
        Heading decorations are documented in sphinx.rst.

=========================
Supported File Operations
=========================

(受支持的文件操作)

.. contents:: Table of Contents
   :local:

Below are a discussion of the high level file operations that iomap
implements.
(下面讨论 iomap 所实现的高层文件操作。)

Buffered I/O
(缓冲 I/O)
============

Buffered I/O is the default file I/O path in Linux.
File contents are cached in memory ("pagecache") to satisfy reads and
writes.
Dirty cache will be written back to disk at some point that can be
forced via ``fsync`` and variants.
(缓冲 I/O 是 Linux 中默认的文件 I/O 路径。文件内容被缓存到内存中("pagecache")以满足读取和写入。脏缓存在某个时点会被回写到磁盘，这一过程可以通过 ``fsync`` 及其变体来强制执行。)

iomap implements nearly all the folio and pagecache management that
filesystems have to implement themselves under the legacy I/O model.
This means that the filesystem need not know the details of allocating,
mapping, managing uptodate and dirty state, or writeback of pagecache
folios.
Under the legacy I/O model, this was managed very inefficiently with
linked lists of buffer heads instead of the per-folio bitmaps that iomap
uses.
Unless the filesystem explicitly opts in to buffer heads, they will not
be used, which makes buffered I/O much more efficient, and the pagecache
maintainer much happier.
(iomap 实现了几乎全部的 folio 和页缓存管理工作，而在传统 I/O 模型下这些必须由文件系统自己实现。这意味着文件系统无需了解分配、映射、管理 uptodate 和脏状态以及页缓存 folio 回写的细节。在传统 I/O 模型下，这些工作通过 buffer_head 链表而不是 iomap 所使用的每 folio 位图来管理，效率非常低。除非文件系统显式选择使用 buffer head，否则它们不会被用到，这使得缓冲 I/O 高效得多，页缓存维护者也省心得多。)

``struct address_space_operations``
(结构体 address_space_operations)
-----------------------------------

The following iomap functions can be referenced directly from the
address space operations structure:
(以下 iomap 函数可以直接在地址空间操作结构体中引用：)

 * ``iomap_dirty_folio``
 * ``iomap_release_folio``
 * ``iomap_invalidate_folio``
 * ``iomap_is_partially_uptodate``

The following address space operations can be wrapped easily:
(以下地址空间操作可以被轻松地封装：)

 * ``read_folio``
 * ``readahead``
 * ``writepages``
 * ``bmap``
 * ``swap_activate``

``struct iomap_write_ops``
(结构体 iomap_write_ops)
--------------------------

.. code-block:: c

 struct iomap_write_ops {
     struct folio *(*get_folio)(struct iomap_iter *iter, loff_t pos,
                                unsigned len);
     void (*put_folio)(struct inode *inode, loff_t pos, unsigned copied,
                       struct folio *folio);
     bool (*iomap_valid)(struct inode *inode, const struct iomap *iomap);
     int (*read_folio_range)(const struct iomap_iter *iter,
     			struct folio *folio, loff_t pos, size_t len);
 };

iomap calls these functions:
(iomap 调用这些函数：)

  - ``get_folio``: Called to allocate and return an active reference to
    a locked folio prior to starting a write.
    If this function is not provided, iomap will call
    ``iomap_get_folio``.
    This could be used to `set up per-folio filesystem state
    <https://lore.kernel.org/all/20190429220934.10415-5-agruenba@redhat.com/>`_
    for a write.
    (get_folio：调用它以在开始写入之前分配并返回对一个已锁定 folio 的活动引用。如果未提供此函数，iomap 将调用 ``iomap_get_folio``。这可用于为一次写入 `设置每 folio 的文件系统状态 <https://lore.kernel.org/all/20190429220934.10415-5-agruenba@redhat.com/>`_ 。)

  - ``put_folio``: Called to unlock and put a folio after a pagecache
    operation completes.
    If this function is not provided, iomap will ``folio_unlock`` and
    ``folio_put`` on its own.
    This could be used to `commit per-folio filesystem state
    <https://lore.kernel.org/all/20180619164137.13720-6-hch@lst.de/>`_
    that was set up by ``->get_folio``.
    (put_folio：调用它以在页缓存操作完成后解锁并释放一个 folio。如果未提供此函数，iomap 会自行执行 ``folio_unlock`` 和 ``folio_put``。这可用于 `提交每 folio 的文件系统状态 <https://lore.kernel.org/all/20180619164137.13720-6-hch@lst.de/>`_ ，该状态由 ``->get_folio`` 设置。)

  - ``iomap_valid``: The filesystem may not hold locks between
    ``->iomap_begin`` and ``->iomap_end`` because pagecache operations
    can take folio locks, fault on userspace pages, initiate writeback
    for memory reclamation, or engage in other time-consuming actions.
    If a file's space mapping data are mutable, it is possible that the
    mapping for a particular pagecache folio can `change in the time it
    takes
    <https://lore.kernel.org/all/20221123055812.747923-8-david@fromorbit.com/>`_
    to allocate, install, and lock that folio.
    (iomap_valid：文件系统可能无法在 ``->iomap_begin`` 和 ``->iomap_end`` 之间持有锁，因为页缓存操作可能获取 folio 锁、在用户空间页上产生缺页、为内存回收发起回写，或者进行其他耗时的操作。如果文件的空间映射数据是可变的，那么在 `分配、安装并锁定该 folio 所需的时间内 <https://lore.kernel.org/all/20221123055812.747923-8-david@fromorbit.com/>`_ ，特定页缓存 folio 的映射可能发生变化。)

    For the pagecache, races can happen if writeback doesn't take
    ``i_rwsem`` or ``invalidate_lock`` and updates mapping information.
    Races can also happen if the filesystem allows concurrent writes.
    For such files, the mapping *must* be revalidated after the folio
    lock has been taken so that iomap can manage the folio correctly.
    (对于页缓存，如果回写在不获取 ``i_rwsem`` 或 ``invalidate_lock`` 的情况下更新映射信息，就可能发生竞争。如果文件系统允许并发写入，也可能发生竞争。对于这类文件，在获取 folio 锁之后 *必须* 重新验证映射，以便 iomap 能够正确地管理 folio。)

    fsdax does not need this revalidation because there's no writeback
    and no support for unwritten extents.
    (fsdax 不需要这种重新验证，因为没有回写，也不支持未写入的扩展。)

    Filesystems subject to this kind of race must provide a
    ``->iomap_valid`` function to decide if the mapping is still valid.
    If the mapping is not valid, the mapping will be sampled again.
    (存在这类竞争的文件系统必须提供一个 ``->iomap_valid`` 函数来判断映射是否仍然有效。如果映射无效，将重新采样映射。)

    To support making the validity decision, the filesystem's
    ``->iomap_begin`` function may set ``struct iomap::validity_cookie``
    at the same time that it populates the other iomap fields.
    A simple validation cookie implementation is a sequence counter.
    If the filesystem bumps the sequence counter every time it modifies
    the inode's extent map, it can be placed in the ``struct
    iomap::validity_cookie`` during ``->iomap_begin``.
    If the value in the cookie is found to be different to the value
    the filesystem holds when the mapping is passed back to
    ``->iomap_valid``, then the iomap should considered stale and the
    validation failed.
    (为支持做出有效性判断，文件系统的 ``->iomap_begin`` 函数可以在填充其他 iomap 字段的同时设置 ``struct iomap::validity_cookie``。一种简单的验证 cookie 实现是序列计数器。如果文件系统每次修改 inode 的扩展映射时都递增该序列计数器，那么在 ``->iomap_begin`` 期间就可以把它放入 ``struct iomap::validity_cookie`` 中。当映射被传回 ``->iomap_valid`` 时，如果发现 cookie 中的值与文件系统持有的值不同，则应认为该 iomap 已过期，验证失败。)

  - ``read_folio_range``: Called to synchronously read in the range that will
    be written to. If this function is not provided, iomap will default to
    submitting a bio read request.
    (read_folio_range：调用它以同步读入将要写入的范围。如果未提供此函数，iomap 将默认提交一个 bio 读请求。)

These ``struct kiocb`` flags are significant for buffered I/O with iomap:
(这些 ``struct kiocb`` 标志对使用 iomap 的缓冲 I/O 有重要意义：)

 * ``IOCB_NOWAIT``: Turns on ``IOMAP_NOWAIT``.
   (IOCB_NOWAIT：打开 ``IOMAP_NOWAIT``。)

 * ``IOCB_DONTCACHE``: Turns on ``IOMAP_DONTCACHE``.
   (IOCB_DONTCACHE：打开 ``IOMAP_DONTCACHE``。)

``struct iomap_read_ops``
(结构体 iomap_read_ops)
--------------------------

.. code-block:: c

 struct iomap_read_ops {
     int (*read_folio_range)(const struct iomap_iter *iter,
                             struct iomap_read_folio_ctx *ctx, size_t len);
     void (*submit_read)(struct iomap_read_folio_ctx *ctx);
 };

iomap calls these functions:
(iomap 调用这些函数：)

  - ``read_folio_range``: Called to read in the range. This must be provided
    by the caller. If this succeeds, iomap_finish_folio_read() must be called
    after the range is read in, regardless of whether the read succeeded or
    failed.
    (read_folio_range：调用它以读入该范围。它必须由调用者提供。如果成功，在该范围读入之后必须调用 iomap_finish_folio_read()，无论读取成功还是失败。)

  - ``submit_read``: Submit any pending read requests. This function is
    optional.
    (submit_read：提交任何未决的读请求。此函数是可选的。)

Internal per-Folio State
(每 folio 内部状态)
------------------------

If the fsblock size matches the size of a pagecache folio, it is assumed
that all disk I/O operations will operate on the entire folio.
The uptodate (memory contents are at least as new as what's on disk) and
dirty (memory contents are newer than what's on disk) status of the
folio are all that's needed for this case.
(如果 fsblock 大小与页缓存 folio 的大小相同，则假定所有磁盘 I/O 操作都将作用于整个 folio。在这种情况下，只需要 folio 的 uptodate(内存内容至少与磁盘上的内容一样新)和 dirty(内存内容比磁盘上的内容新)状态就足够了。)

If the fsblock size is less than the size of a pagecache folio, iomap
tracks the per-fsblock uptodate and dirty state itself.
This enables iomap to handle both "bs < ps" `filesystems
<https://lore.kernel.org/all/20230725122932.144426-1-ritesh.list@gmail.com/>`_
and large folios in the pagecache.
(如果 fsblock 大小小于页缓存 folio 的大小，iomap 会自己跟踪每 fsblock 的 uptodate 和 dirty 状态。这使得 iomap 既能处理 "bs < ps" 的 `文件系统 <https://lore.kernel.org/all/20230725122932.144426-1-ritesh.list@gmail.com/>`_ ，也能处理页缓存中的大 folio。)

iomap internally tracks two state bits per fsblock:
(iomap 内部为每个 fsblock 跟踪两个状态位：)

 * ``uptodate``: iomap will try to keep folios fully up to date.
   If there are read(ahead) errors, those fsblocks will not be marked
   uptodate.
   The folio itself will be marked uptodate when all fsblocks within the
   folio are uptodate.
   (uptodate：iomap 会尽量让 folio 保持完全最新。如果出现读(预读)错误，那些 fsblock 将不会被标记为 uptodate。当 folio 中的所有 fsblock 都是 uptodate 时，folio 本身会被标记为 uptodate。)

 * ``dirty``: iomap will set the per-block dirty state when programs
   write to the file.
   The folio itself will be marked dirty when any fsblock within the
   folio is dirty.
   (dirty：当程序写文件时，iomap 会设置每块的 dirty 状态。当 folio 中有任何 fsblock 为 dirty 时，folio 本身会被标记为 dirty。)

iomap also tracks the amount of read and write disk IOs that are in
flight.
This structure is much lighter weight than ``struct buffer_head``
because there is only one per folio, and the per-fsblock overhead is two
bits vs. 104 bytes.
(iomap 还会跟踪正在传输中的读写磁盘 I/O 数量。这个结构比 ``struct buffer_head`` 轻量得多，因为每个 folio 只有一个，而且每 fsblock 的开销是两个位，而 buffer head 是 104 字节。)

Filesystems wishing to turn on large folios in the pagecache should call
``mapping_set_large_folios`` when initializing the incore inode.
(希望在页缓存中启用大 folio 的文件系统，应在初始化内存中(incore)的 inode 时调用 ``mapping_set_large_folios``。)

Buffered Readahead and Reads
(缓冲预读与读取)
----------------------------

The ``iomap_readahead`` function initiates readahead to the pagecache.
The ``iomap_read_folio`` function reads one folio's worth of data into
the pagecache.
The ``flags`` argument to ``->iomap_begin`` will be set to zero.
The pagecache takes whatever locks it needs before calling the
filesystem.
(``iomap_readahead`` 函数向页缓存发起预读。``iomap_read_folio`` 函数把一个 folio 大小的数据读入页缓存。传给 ``->iomap_begin`` 的 ``flags`` 参数将被设为零。页缓存在调用文件系统之前会获取它需要的任何锁。)

Both ``iomap_readahead`` and ``iomap_read_folio`` pass in a ``struct
iomap_read_folio_ctx``:
(``iomap_readahead`` 和 ``iomap_read_folio`` 都会传入一个 ``struct iomap_read_folio_ctx``：)

.. code-block:: c

 struct iomap_read_folio_ctx {
    const struct iomap_read_ops *ops;
    struct folio *cur_folio;
    struct readahead_control *rac;
    void *read_ctx;
 };

``iomap_readahead`` must set:
 (``iomap_readahead`` 必须设置：)
 * ``ops->read_folio_range()`` and ``rac``

``iomap_read_folio`` must set:
 (``iomap_read_folio`` 必须设置：)
 * ``ops->read_folio_range()`` and ``cur_folio``

``ops->submit_read()`` and ``read_ctx`` are optional. ``read_ctx`` is used to
pass in any custom data the caller needs accessible in the ops callbacks for
fulfilling reads.
(``ops->submit_read()`` 和 ``read_ctx`` 是可选的。``read_ctx`` 用于传入调用者需要能够在 ops 回调中访问的任何自定义数据，以完成读取。)

Buffered Writes
(缓冲写入)
---------------

The ``iomap_file_buffered_write`` function writes an ``iocb`` to the
pagecache.
``IOMAP_WRITE`` or ``IOMAP_WRITE`` | ``IOMAP_NOWAIT`` will be passed as
the ``flags`` argument to ``->iomap_begin``.
Callers commonly take ``i_rwsem`` in either shared or exclusive mode
before calling this function.
(``iomap_file_buffered_write`` 函数把一个 ``iocb`` 写入页缓存。``IOMAP_WRITE`` 或 ``IOMAP_WRITE`` | ``IOMAP_NOWAIT`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。调用者通常在调用此函数之前以共享或独占模式获取 ``i_rwsem``。)

mmap Write Faults
(mmap 写入缺页)
~~~~~~~~~~~~~~~~~

The ``iomap_page_mkwrite`` function handles a write fault to a folio in
the pagecache.
``IOMAP_WRITE | IOMAP_FAULT`` will be passed as the ``flags`` argument
to ``->iomap_begin``.
Callers commonly take the mmap ``invalidate_lock`` in shared or
exclusive mode before calling this function.
(``iomap_page_mkwrite`` 函数处理对页缓存中某个 folio 的写入缺页。``IOMAP_WRITE | IOMAP_FAULT`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。调用者通常在调用此函数之前以共享或独占模式获取 mmap 的 ``invalidate_lock``。)

Buffered Write Failures
(缓冲写入失败)
~~~~~~~~~~~~~~~~~~~~~~~

After a short write to the pagecache, the areas not written will not
become marked dirty.
The filesystem must arrange to `cancel
<https://lore.kernel.org/all/20221123055812.747923-6-david@fromorbit.com/>`_
such `reservations
<https://lore.kernel.org/linux-xfs/20220817093627.GZ3600936@dread.disaster.area/>`_
because writeback will not consume the reservation.
The ``iomap_write_delalloc_release`` can be called from a
``->iomap_end`` function to find all the clean areas of the folios
caching a fresh (``IOMAP_F_NEW``) delalloc mapping.
It takes the ``invalidate_lock``.
(对页缓存进行短写之后，未写入的区域不会被标记为 dirty。文件系统必须设法 `取消 <https://lore.kernel.org/all/20221123055812.747923-6-david@fromorbit.com/>`_ 这些 `预留 <https://lore.kernel.org/linux-xfs/20220817093627.GZ3600936@dread.disaster.area/>`_ ，因为回写不会消耗该预留。可以从 ``->iomap_end`` 函数中调用 ``iomap_write_delalloc_release``，来找出缓存着新(``IOMAP_F_NEW``)delalloc 映射的 folio 中所有干净的区域。它会获取 ``invalidate_lock``。)

The filesystem must supply a function ``punch`` to be called for
each file range in this state.
This function must *only* remove delayed allocation reservations, in
case another thread racing with the current thread writes successfully
to the same region and triggers writeback to flush the dirty data out to
disk.
(文件系统必须提供一个 ``punch`` 函数，对处于这种状态的每个文件范围调用它。此函数 *只* 移除延迟分配预留，以防与当前线程竞争的另一个线程成功写入同一区域并触发回写，把脏数据刷出到磁盘。)

Zeroing for File Operations
(文件操作清零)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Filesystems can call ``iomap_zero_range`` to perform zeroing of the
pagecache for non-truncation file operations that are not aligned to
the fsblock size.
``IOMAP_ZERO`` will be passed as the ``flags`` argument to
``->iomap_begin``.
Callers typically hold ``i_rwsem`` and ``invalidate_lock`` in exclusive
mode before calling this function.
(文件系统可以调用 ``iomap_zero_range``，为未按 fsblock 大小对齐的非截断文件操作执行页缓存清零。``IOMAP_ZERO`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。调用者通常在调用此函数之前以独占模式持有 ``i_rwsem`` 和 ``invalidate_lock``。)

Unsharing Reflinked File Data
(对 reflink 文件数据取消共享)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Filesystems can call ``iomap_file_unshare`` to force a file sharing
storage with another file to preemptively copy the shared data to newly
allocate storage.
``IOMAP_WRITE | IOMAP_UNSHARE`` will be passed as the ``flags`` argument
to ``->iomap_begin``.
Callers typically hold ``i_rwsem`` and ``invalidate_lock`` in exclusive
mode before calling this function.
(文件系统可以调用 ``iomap_file_unshare``，来强制一个与其他文件共享存储的文件抢先地把共享数据复制到新分配的存储中。``IOMAP_WRITE | IOMAP_UNSHARE`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。调用者通常在调用此函数之前以独占模式持有 ``i_rwsem`` 和 ``invalidate_lock``。)

Truncation
(截断)
----------

Filesystems can call ``iomap_truncate_page`` to zero the bytes in the
pagecache from EOF to the end of the fsblock during a file truncation
operation.
``truncate_setsize`` or ``truncate_pagecache`` will take care of
everything after the EOF block.
``IOMAP_ZERO`` will be passed as the ``flags`` argument to
``->iomap_begin``.
Callers typically hold ``i_rwsem`` and ``invalidate_lock`` in exclusive
mode before calling this function.
(文件系统可以调用 ``iomap_truncate_page``，在文件截断操作期间把页缓存中从 EOF 到 fsblock 末尾的字节清零。EOF 块之后的一切由 ``truncate_setsize`` 或 ``truncate_pagecache`` 负责处理。``IOMAP_ZERO`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。调用者通常在调用此函数之前以独占模式持有 ``i_rwsem`` 和 ``invalidate_lock``。)

Pagecache Writeback
(页缓存回写)
-------------------

Filesystems can call ``iomap_writepages`` to respond to a request to
write dirty pagecache folios to disk.
The ``mapping`` and ``wbc`` parameters should be passed unchanged.
The ``wpc`` pointer should be allocated by the filesystem and must
be initialized to zero.
(文件系统可以调用 ``iomap_writepages`` 来响应把脏页缓存 folio 写入磁盘的请求。``mapping`` 和 ``wbc`` 参数应原样传递。``wpc`` 指针应由文件系统分配，并且必须初始化为零。)

The pagecache will lock each folio before trying to schedule it for
writeback.
It does not lock ``i_rwsem`` or ``invalidate_lock``.
(页缓存在尝试把每个 folio 调度回写之前，都会先锁定该 folio。它不会锁定 ``i_rwsem`` 或 ``invalidate_lock``。)

The dirty bit will be cleared for all folios run through the
``->writeback_range`` machinery described below even if the writeback fails.
This is to prevent dirty folio clots when storage devices fail; an
``-EIO`` is recorded for userspace to collect via ``fsync``.
(对于所有经由下文所述 ``->writeback_range`` 机制处理的 folio，即使回写失败，其脏位也会被清除。这是为了在存储设备发生故障时防止脏 folio 堆积；会记录一个 ``-EIO`` 供用户空间通过 ``fsync`` 收取。)

The ``ops`` structure must be specified and is as follows:
(``ops`` 结构必须指定，如下所示：)

``struct iomap_writeback_ops``
(结构体 iomap_writeback_ops)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

 struct iomap_writeback_ops {
    int (*writeback_range)(struct iomap_writepage_ctx *wpc,
        struct folio *folio, u64 pos, unsigned int len, u64 end_pos);
    int (*writeback_submit)(struct iomap_writepage_ctx *wpc, int error);
 };

The fields are as follows:
(字段如下：)

  - ``writeback_range``: Sets ``wpc->iomap`` to the space mapping of the file
    range (in bytes) given by ``offset`` and ``len``.
    iomap calls this function for each dirty fs block in each dirty folio,
    though it will `reuse mappings
    <https://lore.kernel.org/all/20231207072710.176093-15-hch@lst.de/>`_
    for runs of contiguous dirty fsblocks within a folio.
    Do not return ``IOMAP_INLINE`` mappings here; the ``->iomap_end``
    function must deal with persisting written data.
    Do not return ``IOMAP_DELALLOC`` mappings here; iomap currently
    requires mapping to allocated space.
    Filesystems can skip a potentially expensive mapping lookup if the
    mappings have not changed.
    This revalidation must be open-coded by the filesystem; it is
    unclear if ``iomap::validity_cookie`` can be reused for this
    purpose.
    (writeback_range：把 ``wpc->iomap`` 设置为由 ``offset`` 和 ``len`` 给出的文件范围(以字节为单位)的空间映射。iomap 会对每个脏 folio 中的每个脏 fs 块调用此函数，不过对于 folio 内连续的脏 fsblock 区段，它会 `重用映射 <https://lore.kernel.org/all/20231207072710.176093-15-hch@lst.de/>`_ 。不要在这里返回 ``IOMAP_INLINE`` 映射；``->iomap_end`` 函数必须负责持久化已写入的数据。不要在这里返回 ``IOMAP_DELALLOC`` 映射；iomap 目前要求映射到已分配的空间。如果映射没有变化，文件系统可以跳过可能代价高昂的映射查找。这种重新验证必须由文件系统自己编写；目前尚不清楚 ``iomap::validity_cookie`` 是否可为此目的复用。)

    If this methods fails to schedule I/O for any part of a dirty folio, it
    should throw away any reservations that may have been made for the write.
    The folio will be marked clean and an ``-EIO`` recorded in the
    pagecache.
    Filesystems can use this callback to `remove
    <https://lore.kernel.org/all/20201029163313.1766967-1-bfoster@redhat.com/>`_
    delalloc reservations to avoid having delalloc reservations for
    clean pagecache.
    This function must be supplied by the filesystem.
    If this succeeds, iomap_finish_folio_write() must be called once writeback
    completes for the range, regardless of whether the writeback succeeded or
    failed.
    (如果此方法未能为脏 folio 的任何部分调度 I/O，它应当丢弃可能已经为该写入建立的任何预留。该 folio 将被标记为干净，并在页缓存中记录一个 ``-EIO``。文件系统可以使用此回调来 `移除 <https://lore.kernel.org/all/20201029163313.1766967-1-bfoster@redhat.com/>`_ delalloc 预留，以避免为干净的页缓存保留 delalloc 预留。文件系统必须提供此函数。如果成功，在该范围的回写完成之后必须调用一次 iomap_finish_folio_write()，无论回写成功还是失败。)

  - ``writeback_submit``: Submit the previous built writeback context.
    Block based file systems should use the iomap_ioend_writeback_submit
    helper, other file system can implement their own.
    File systems can optionally hook into writeback bio submission.
    This might include pre-write space accounting updates, or installing
    a custom ``->bi_end_io`` function for internal purposes, such as
    deferring the ioend completion to a workqueue to run metadata update
    transactions from process context before submitting the bio.
    This function must be supplied by the filesystem.
    (writeback_submit：提交之前构建好的回写上下文。基于块的文件系统应使用 iomap_ioend_writeback_submit 辅助函数，其他文件系统可以实现自己的。文件系统可以选择性地挂钩回写 bio 提交。这可能包括写前的空间记账更新，或者为内部目的安装自定义的 ``->bi_end_io`` 函数，例如在提交 bio 之前，把 ioend 完成推迟到工作队列，以便从进程上下文运行元数据更新事务。文件系统必须提供此函数。)

Pagecache Writeback Completion
(页缓存回写完成)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To handle the bookkeeping that must happen after disk I/O for writeback
completes, iomap creates chains of ``struct iomap_ioend`` objects that
wrap the ``bio`` that is used to write pagecache data to disk.
By default, iomap finishes writeback ioends by clearing the writeback
bit on the folios attached to the ``ioend``.
If the write failed, it will also set the error bits on the folios and
the address space.
This can happen in interrupt or process context, depending on the
storage device.
Filesystems that need to update internal bookkeeping (e.g. unwritten
extent conversions) should set their own bi_end_io on the bios
submitted by ``->submit_writeback``
This function should call ``iomap_finish_ioends`` after finishing its
own work (e.g. unwritten extent conversion).
(为了处理回写的磁盘 I/O 完成后必须进行的簿记工作，iomap 创建了 ``struct iomap_ioend`` 对象链，用来包装把页缓存数据写入磁盘所用的 ``bio``。默认情况下，iomap 通过清除附加到 ``ioend`` 的 folio 上的回写位来完成回写 ioend。如果写入失败，它还会在 folio 和地址空间上设置错误位。这可能发生在中断或进程上下文中，取决于存储设备。需要更新内部簿记(例如未写入扩展转换)的文件系统，应在由 ``->submit_writeback`` 提交的 bio 上设置自己的 bi_end_io。此函数应在完成自己的工作(例如未写入扩展转换)之后调用 ``iomap_finish_ioends``。)

Some filesystems may wish to `amortize the cost of running metadata
transactions
<https://lore.kernel.org/all/20220120034733.221737-1-david@fromorbit.com/>`_
for post-writeback updates by batching them.
They may also require transactions to run from process context, which
implies punting batches to a workqueue.
iomap ioends contain a ``list_head`` to enable batching.
(一些文件系统可能希望通过分批处理来 `摊销为回写后更新而运行元数据事务的成本 <https://lore.kernel.org/all/20220120034733.221737-1-david@fromorbit.com/>`_ 。它们可能还要求事务从进程上下文运行，这意味着要把批次转移到工作队列。iomap ioend 包含一个 ``list_head`` 以支持分批处理。)

Given a batch of ioends, iomap has a few helpers to assist with
amortization:
(给定一批 ioend，iomap 提供了一些辅助函数来协助摊销：)

 * ``iomap_sort_ioends``: Sort all the ioends in the list by file
   offset.
   (iomap_sort_ioends：按文件偏移对列表中的所有 ioend 排序。)

 * ``iomap_ioend_try_merge``: Given an ioend that is not in any list and
   a separate list of sorted ioends, merge as many of the ioends from
   the head of the list into the given ioend.
   ioends can only be merged if the file range and storage addresses are
   contiguous; the unwritten and shared status are the same; and the
   write I/O outcome is the same.
   The merged ioends become their own list.
   (iomap_ioend_try_merge：给定一个不在任何列表中的 ioend 和另一个已排序的 ioend 列表，把列表头部尽可能多的 ioend 合并进给定的 ioend。只有当文件范围和存储地址连续、未写入和共享状态相同、并且写 I/O 结果相同时，ioend 才能合并。合并后的 ioend 自成列表。)

 * ``iomap_finish_ioends``: Finish an ioend that possibly has other
   ioends linked to it.
   (iomap_finish_ioends：完成一个可能链接了其他 ioend 的 ioend。)

Direct I/O
(直接 I/O)
==========

In Linux, direct I/O is defined as file I/O that is issued directly to
storage, bypassing the pagecache.
The ``iomap_dio_rw`` function implements O_DIRECT (direct I/O) reads and
writes for files.
(在 Linux 中，直接 I/O 定义为绕过页缓存、直接下发给存储的文件 I/O。``iomap_dio_rw`` 函数为文件实现 O_DIRECT(直接 I/O)读写。)

.. code-block:: c

 ssize_t iomap_dio_rw(struct kiocb *iocb, struct iov_iter *iter,
                      const struct iomap_ops *ops,
                      const struct iomap_dio_ops *dops,
                      unsigned int dio_flags, void *private,
                      size_t done_before);

The filesystem can provide the ``dops`` parameter if it needs to perform
extra work before or after the I/O is issued to storage.
The ``done_before`` parameter tells the how much of the request has
already been transferred.
It is used to continue a request asynchronously when `part of the
request
<https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=c03098d4b9ad76bca2966a8769dcfe59f7f85103>`_
has already been completed synchronously.
(如果文件系统需要在 I/O 下发给存储之前或之后执行额外的工作，它可以提供 ``dops`` 参数。``done_before`` 参数告知请求中已经传输了多少。当 `请求的一部分 <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=c03098d4b9ad76bca2966a8769dcfe59f7f85103>`_ 已经同步完成时，它用于异步地继续该请求。)

The ``done_before`` parameter should be set if writes for the ``iocb``
have been initiated prior to the call.
The direction of the I/O is determined from the ``iocb`` passed in.
(如果 ``iocb`` 的写入在调用之前已经启动，则应设置 ``done_before`` 参数。I/O 的方向由传入的 ``iocb`` 决定。)

The ``dio_flags`` argument can be set to any combination of the
following values:
(``dio_flags`` 参数可以设置为以下值的任意组合：)

 * ``IOMAP_DIO_FORCE_WAIT``: Wait for the I/O to complete even if the
   kiocb is not synchronous.
   (IOMAP_DIO_FORCE_WAIT：即使 kiocb 不是同步的，也要等待 I/O 完成。)

 * ``IOMAP_DIO_OVERWRITE_ONLY``: Perform a pure overwrite for this range
   or fail with ``-EAGAIN``.
   This can be used by filesystems with complex unaligned I/O
   write paths to provide an optimised fast path for unaligned writes.
   (IOMAP_DIO_OVERWRITE_ONLY：对此范围执行纯覆盖，否则以 ``-EAGAIN`` 失败。具有复杂非对齐 I/O 写路径的文件系统可以用它为非对齐写入提供一个优化过的快速路径。)
   If a pure overwrite can be performed, then serialisation against
   other I/Os to the same filesystem block(s) is unnecessary as there is
   no risk of stale data exposure or data loss.
   (如果可以执行纯覆盖，则不需要针对同一文件系统块的其他 I/O 做串行化，因为不存在暴露过期数据或数据丢失的风险。)
   If a pure overwrite cannot be performed, then the filesystem can
   perform the serialisation steps needed to provide exclusive access
   to the unaligned I/O range so that it can perform allocation and
   sub-block zeroing safely.
   (如果不能执行纯覆盖，文件系统可以执行所需的串行化步骤，以提供对该非对齐 I/O 范围的独占访问，从而安全地执行分配和块内清零。)
   Filesystems can use this flag to try to reduce locking contention,
   but a lot of `detailed checking
   <https://lore.kernel.org/linux-ext4/20230314130759.642710-1-bfoster@redhat.com/>`_
   is required to do it `correctly
   <https://lore.kernel.org/linux-ext4/20230810165559.946222-1-bfoster@redhat.com/>`_.
   (文件系统可以使用此标志来尝试减少锁竞争，但要做到这一点，需要大量的 `细致检查 <https://lore.kernel.org/linux-ext4/20230314130759.642710-1-bfoster@redhat.com/>`_ 才能做得 `正确 <https://lore.kernel.org/linux-ext4/20230810165559.946222-1-bfoster@redhat.com/>`_ 。)

 * ``IOMAP_DIO_PARTIAL``: If a page fault occurs, return whatever
   progress has already been made.
   The caller may deal with the page fault and retry the operation.
   If the caller decides to retry the operation, it should pass the
   accumulated return values of all previous calls as the
   ``done_before`` parameter to the next call.
   (IOMAP_DIO_PARTIAL：如果发生缺页，返回已经取得的任何进度。调用者可以处理该缺页并重试操作。如果调用者决定重试操作，它应当把之前所有调用的返回值累加起来，作为 ``done_before`` 参数传给下一次调用。)

These ``struct kiocb`` flags are significant for direct I/O with iomap:
(这些 ``struct kiocb`` 标志对使用 iomap 的直接 I/O 有重要意义：)

 * ``IOCB_NOWAIT``: Turns on ``IOMAP_NOWAIT``.
   (IOCB_NOWAIT：打开 ``IOMAP_NOWAIT``。)

 * ``IOCB_SYNC``: Ensure that the device has persisted data to disk
   before completing the call.
   In the case of pure overwrites, the I/O may be issued with FUA
   enabled.
   (IOCB_SYNC：确保设备在调用完成之前已把数据持久化到磁盘。对于纯覆盖的情况，I/O 可以在启用 FUA 的情况下下发。)

 * ``IOCB_HIPRI``: Poll for I/O completion instead of waiting for an
   interrupt.
   Only meaningful for asynchronous I/O, and only if the entire I/O can
   be issued as a single ``struct bio``.
   (IOCB_HIPRI：轮询 I/O 完成而不是等待中断。仅对异步 I/O 有意义，并且仅当整个 I/O 能够以单个 ``struct bio`` 下发时才有意义。)

Filesystems should call ``iomap_dio_rw`` from ``->read_iter`` and
``->write_iter``, and set ``FMODE_CAN_ODIRECT`` in the ``->open``
function for the file.
They should not set ``->direct_IO``, which is deprecated.
(文件系统应从 ``->read_iter`` 和 ``->write_iter`` 调用 ``iomap_dio_rw``，并在文件的 ``->open`` 函数中设置 ``FMODE_CAN_ODIRECT``。它们不应设置 ``->direct_IO``，它已被弃用。)

If a filesystem wishes to perform its own work before direct I/O
completion, it should call ``__iomap_dio_rw``.
If its return value is not an error pointer or a NULL pointer, the
filesystem should pass the return value to ``iomap_dio_complete`` after
finishing its internal work.
(如果文件系统希望在直接 I/O 完成之前执行自己的工作，它应调用 ``__iomap_dio_rw``。如果其返回值不是错误指针或 NULL 指针，文件系统应在完成自己的内部工作之后，把该返回值传给 ``iomap_dio_complete``。)

Return Values
(返回值)
-------------

``iomap_dio_rw`` can return one of the following:
(``iomap_dio_rw`` 可以返回以下之一：)

 * A non-negative number of bytes transferred.
   (传输的字节数(非负值)。)

 * ``-ENOTBLK``: Fall back to buffered I/O.
   iomap itself will return this value if it cannot invalidate the page
   cache before issuing the I/O to storage.
   The ``->iomap_begin`` or ``->iomap_end`` functions may also return
   this value.
   (-ENOTBLK：回退到缓冲 I/O。如果 iomap 在向存储下发 I/O 之前无法使页缓存失效，它本身会返回此值。``->iomap_begin`` 或 ``->iomap_end`` 函数也可能返回此值。)

 * ``-EIOCBQUEUED``: The asynchronous direct I/O request has been
   queued and will be completed separately.
   (-EIOCBQUEUED：异步直接 I/O 请求已经排队，将单独完成。)

 * Any of the other negative error codes.
   (任何其他负的错误码。)

Direct Reads
(直接读取)
------------

A direct I/O read initiates a read I/O from the storage device to the
caller's buffer.
Dirty parts of the pagecache are flushed to storage before initiating
the read io.
The ``flags`` value for ``->iomap_begin`` will be ``IOMAP_DIRECT`` with
any combination of the following enhancements:
(直接 I/O 读取发起一次从存储设备到调用者缓冲区的读 I/O。在发起读取 I/O 之前，页缓存的脏部分会先刷新到存储。``->iomap_begin`` 的 ``flags`` 值将为 ``IOMAP_DIRECT``，并可带有以下增强项的任意组合：)

 * ``IOMAP_NOWAIT``, as defined previously.
   (``IOMAP_NOWAIT``，如前文所定义。)

Callers commonly hold ``i_rwsem`` in shared mode before calling this
function.
(调用者通常在调用此函数之前以共享模式持有 ``i_rwsem``。)

Direct Writes
(直接写入)
-------------

A direct I/O write initiates a write I/O to the storage device from the
caller's buffer.
Dirty parts of the pagecache are flushed to storage before initiating
the write io.
The pagecache is invalidated both before and after the write io.
The ``flags`` value for ``->iomap_begin`` will be ``IOMAP_DIRECT |
IOMAP_WRITE`` with any combination of the following enhancements:
(直接 I/O 写入发起一次从调用者缓冲区到存储设备的写 I/O。在发起写入 I/O 之前，页缓存的脏部分会先刷新到存储。页缓存在写入 I/O 之前和之后都会失效。``->iomap_begin`` 的 ``flags`` 值将为 ``IOMAP_DIRECT | IOMAP_WRITE``，并可带有以下增强项的任意组合：)

 * ``IOMAP_NOWAIT``, as defined previously.
   (``IOMAP_NOWAIT``，如前文所定义。)

 * ``IOMAP_OVERWRITE_ONLY``: Allocating blocks and zeroing partial
   blocks is not allowed.
   The entire file range must map to a single written or unwritten
   extent.
   The file I/O range must be aligned to the filesystem block size
   if the mapping is unwritten and the filesystem cannot handle zeroing
   the unaligned regions without exposing stale contents.
   (IOMAP_OVERWRITE_ONLY：不允许分配块和清零部分块。整个文件范围必须映射到单个已写入或未写入的扩展。如果映射是未写入的，并且文件系统无法在不暴露过期内容的情况下处理非对齐区域的清零，那么文件 I/O 范围必须与文件系统块大小对齐。)

 * ``IOMAP_ATOMIC``: This write is being issued with torn-write
   protection.
   Torn-write protection may be provided based on HW-offload or by a
   software mechanism provided by the filesystem.
   (IOMAP_ATOMIC：此写入是在撕裂写保护(torn-write protection)之下下发的。撕裂写保护可以基于硬件卸载(HW-offload)提供，也可以由文件系统提供的软件机制提供。)

   For HW-offload based support, only a single bio can be created for the
   write, and the write must not be split into multiple I/O requests, i.e.
   flag REQ_ATOMIC must be set.
   The file range to write must be aligned to satisfy the requirements
   of both the filesystem and the underlying block device's atomic
   commit capabilities.
   If filesystem metadata updates are required (e.g. unwritten extent
   conversion or copy-on-write), all updates for the entire file range
   must be committed atomically as well.
   Untorn-writes may be longer than a single file block. In all cases,
   the mapping start disk block must have at least the same alignment as
   the write offset.
   The filesystems must set IOMAP_F_ATOMIC_BIO to inform iomap core of an
   untorn-write based on HW-offload.
   (对于基于硬件卸载的支持，只能为该写入创建单个 bio，并且写入不得拆分为多个 I/O 请求，即必须设置 REQ_ATOMIC 标志。要写入的文件范围必须对齐，以便同时满足文件系统和底层块设备原子提交能力的要求。如果需要文件系统元数据更新(例如未写入扩展转换或写时复制)，整个文件范围的所有更新也必须原子地提交。不撕裂写(un-torn write)可以长于单个文件块。在所有情况下，映射起始磁盘块的对齐至少要与写入偏移的对齐相同。文件系统必须设置 IOMAP_F_ATOMIC_BIO，以告知 iomap 核心这是基于硬件卸载的不撕裂写。)

   For untorn-writes based on a software mechanism provided by the
   filesystem, all the disk block alignment and single bio restrictions
   which apply for HW-offload based untorn-writes do not apply.
   The mechanism would typically be used as a fallback for when
   HW-offload based untorn-writes may not be issued, e.g. the range of the
   write covers multiple extents, meaning that it is not possible to issue
   a single bio.
   All filesystem metadata updates for the entire file range must be
   committed atomically as well.
   (对于基于文件系统所提供软件机制的不撕裂写，适用于基于硬件卸载的不撕裂写的所有磁盘块对齐和单 bio 限制都不适用。该机制通常用作基于硬件卸载的不撕裂写可能无法下发时的后备手段，例如写入范围覆盖多个扩展，这意味着无法下发单个 bio。整个文件范围的所有文件系统元数据更新也必须原子地提交。)

Callers commonly hold ``i_rwsem`` in shared or exclusive mode before
calling this function.
(调用者通常在调用此函数之前以共享或独占模式持有 ``i_rwsem``。)

``struct iomap_dio_ops:``
(结构体 iomap_dio_ops)
-------------------------

.. code-block:: c

 struct iomap_dio_ops {
     void (*submit_io)(const struct iomap_iter *iter, struct bio *bio,
                       loff_t file_offset);
     int (*end_io)(struct kiocb *iocb, ssize_t size, int error,
                   unsigned flags);
     struct bio_set *bio_set;
 };

The fields of this structure are as follows:
(该结构体的字段如下：)

  - ``submit_io``: iomap calls this function when it has constructed a
    ``struct bio`` object for the I/O requested, and wishes to submit it
    to the block device.
    If no function is provided, ``submit_bio`` will be called directly.
    Filesystems that would like to perform additional work before (e.g.
    data replication for btrfs) should implement this function.
    (submit_io：当 iomap 为请求的 I/O 构造好 ``struct bio`` 对象并希望把它提交给块设备时，它会调用此函数。如果未提供函数，将直接调用 ``submit_bio``。想要在提交之前执行额外工作(例如 btrfs 的数据复制)的文件系统应实现此函数。)

  - ``end_io``: This is called after the ``struct bio`` completes.
    This function should perform post-write conversions of unwritten
    extent mappings, handle write failures, etc.
    The ``flags`` argument may be set to a combination of the following:
    (end_io：在 ``struct bio`` 完成之后调用。此函数应执行写后的未写入扩展映射转换、处理写失败等。``flags`` 参数可以设置为以下各项的组合：)

    * ``IOMAP_DIO_UNWRITTEN``: The mapping was unwritten, so the ioend
      should mark the extent as written.
      (IOMAP_DIO_UNWRITTEN：映射是未写入的，因此 ioend 应把该扩展标记为已写入。)

    * ``IOMAP_DIO_COW``: Writing to the space in the mapping required a
      copy on write operation, so the ioend should switch mappings.
      (IOMAP_DIO_COW：写入该映射中的空间需要进行写时复制操作，因此 ioend 应切换映射。)

  - ``bio_set``: This allows the filesystem to provide a custom bio_set
    for allocating direct I/O bios.
    This enables filesystems to `stash additional per-bio information
    <https://lore.kernel.org/all/20220505201115.937837-3-hch@lst.de/>`_
    for private use.
    If this field is NULL, generic ``struct bio`` objects will be used.
    (bio_set：这允许文件系统提供自定义的 bio_set 来分配直接 I/O 的 bio。这使文件系统能够 `保存额外的每 bio 信息 <https://lore.kernel.org/all/20220505201115.937837-3-hch@lst.de/>`_ 供私有使用。如果此字段为 NULL，将使用通用的 ``struct bio`` 对象。)

Filesystems that want to perform extra work after an I/O completion
should set a custom ``->bi_end_io`` function via ``->submit_io``.
Afterwards, the custom endio function must call
``iomap_dio_bio_end_io`` to finish the direct I/O.
(想要在 I/O 完成之后执行额外工作的文件系统，应通过 ``->submit_io`` 设置自定义的 ``->bi_end_io`` 函数。之后，自定义的 endio 函数必须调用 ``iomap_dio_bio_end_io`` 来完成直接 I/O。)

DAX I/O
(DAX I/O)
=======

Some storage devices can be directly mapped as memory.
These devices support a new access mode known as "fsdax" that allows
loads and stores through the CPU and memory controller.
(一些存储设备可以直接映射为内存。这些设备支持一种称为 "fsdax" 的新访问模式，允许通过 CPU 和内存控制器进行加载和存储。)

fsdax Reads
(fsdax 读取)
-----------

A fsdax read performs a memcpy from storage device to the caller's
buffer.
The ``flags`` value for ``->iomap_begin`` will be ``IOMAP_DAX`` with any
combination of the following enhancements:
(fsdax 读取执行一次从存储设备到调用者缓冲区的 memcpy。``->iomap_begin`` 的 ``flags`` 值将为 ``IOMAP_DAX``，并可带有以下增强项的任意组合：)

 * ``IOMAP_NOWAIT``, as defined previously.
   (``IOMAP_NOWAIT``，如前文所定义。)

Callers commonly hold ``i_rwsem`` in shared mode before calling this
function.
(调用者通常在调用此函数之前以共享模式持有 ``i_rwsem``。)

fsdax Writes
(fsdax 写入)
-----------

A fsdax write initiates a memcpy to the storage device from the caller's
buffer.
The ``flags`` value for ``->iomap_begin`` will be ``IOMAP_DAX |
IOMAP_WRITE`` with any combination of the following enhancements:
(fsdax 写入发起一次从调用者缓冲区到存储设备的 memcpy。``->iomap_begin`` 的 ``flags`` 值将为 ``IOMAP_DAX | IOMAP_WRITE``，并可带有以下增强项的任意组合：)

 * ``IOMAP_NOWAIT``, as defined previously.
   (``IOMAP_NOWAIT``，如前文所定义。)

 * ``IOMAP_OVERWRITE_ONLY``: The caller requires a pure overwrite to be
   performed from this mapping.
   This requires the filesystem extent mapping to already exist as an
   ``IOMAP_MAPPED`` type and span the entire range of the write I/O
   request.
   If the filesystem cannot map this request in a way that allows the
   iomap infrastructure to perform a pure overwrite, it must fail the
   mapping operation with ``-EAGAIN``.
   (IOMAP_OVERWRITE_ONLY：调用者要求从此映射执行纯覆盖。这要求文件系统扩展映射已经以 ``IOMAP_MAPPED`` 类型存在，并覆盖写 I/O 请求的整个范围。如果文件系统无法以允许 iomap 基础设施执行纯覆盖的方式映射此请求，它必须以 ``-EAGAIN`` 使映射操作失败。)

Callers commonly hold ``i_rwsem`` in exclusive mode before calling this
function.
(调用者通常在调用此函数之前以独占模式持有 ``i_rwsem``。)

fsdax mmap Faults
(fsdax mmap 缺页)
~~~~~~~~~~~~~~~~~

The ``dax_iomap_fault`` function handles read and write faults to fsdax
storage.
For a read fault, ``IOMAP_DAX | IOMAP_FAULT`` will be passed as the
``flags`` argument to ``->iomap_begin``.
For a write fault, ``IOMAP_DAX | IOMAP_FAULT | IOMAP_WRITE`` will be
passed as the ``flags`` argument to ``->iomap_begin``.
(``dax_iomap_fault`` 函数处理对 fsdax 存储的读写缺页。对于读缺页，``IOMAP_DAX | IOMAP_FAULT`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。对于写缺页，``IOMAP_DAX | IOMAP_FAULT | IOMAP_WRITE`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。)

Callers commonly hold the same locks as they do to call their iomap
pagecache counterparts.
(调用者通常持有与调用其 iomap 页缓存对应函数时相同的锁。)

fsdax Truncation, fallocate, and Unsharing
(fsdax 截断、fallocate 与取消共享)
------------------------------------------

For fsdax files, the following functions are provided to replace their
iomap pagecache I/O counterparts.
The ``flags`` argument to ``->iomap_begin`` are the same as the
pagecache counterparts, with ``IOMAP_DAX`` added.
(对于 fsdax 文件，提供了以下函数来替代它们对应的 iomap 页缓存 I/O 函数。传给 ``->iomap_begin`` 的 ``flags`` 参数与页缓存对应函数相同，只是加上了 ``IOMAP_DAX``。)

 * ``dax_file_unshare``
 * ``dax_zero_range``
 * ``dax_truncate_page``

Callers commonly hold the same locks as they do to call their iomap
pagecache counterparts.
(调用者通常持有与调用其 iomap 页缓存对应函数时相同的锁。)

fsdax Deduplication
(fsdax 去重)
-------------------

Filesystems implementing the ``FIDEDUPERANGE`` ioctl must call the
``dax_remap_file_range_prep`` function with their own iomap read ops.
(实现 ``FIDEDUPERANGE`` ioctl 的文件系统，必须用它们自己的 iomap 读操作(ops)来调用 ``dax_remap_file_range_prep`` 函数。)

Seeking Files
(文件定位)
=============

iomap implements the two iterating whence modes of the ``llseek`` system
call.
(iomap 实现了 ``llseek`` 系统调用的两种迭代 whence 模式。)

SEEK_DATA
(SEEK_DATA，查找数据)
---------

The ``iomap_seek_data`` function implements the SEEK_DATA "whence" value
for llseek.
``IOMAP_REPORT`` will be passed as the ``flags`` argument to
``->iomap_begin``.
(``iomap_seek_data`` 函数为 llseek 实现 SEEK_DATA "whence" 值。``IOMAP_REPORT`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。)

For unwritten mappings, the pagecache will be searched.
Regions of the pagecache with a folio mapped and uptodate fsblocks
within those folios will be reported as data areas.
(对于未写入的映射，将搜索页缓存。页缓存中映射了 folio、且其中 fsblock 为 uptodate 的区域，将被报告为数据区域。)

Callers commonly hold ``i_rwsem`` in shared mode before calling this
function.
(调用者通常在调用此函数之前以共享模式持有 ``i_rwsem``。)

SEEK_HOLE
(SEEK_HOLE，查找洞)
---------

The ``iomap_seek_hole`` function implements the SEEK_HOLE "whence" value
for llseek.
``IOMAP_REPORT`` will be passed as the ``flags`` argument to
``->iomap_begin``.
(``iomap_seek_hole`` 函数为 llseek 实现 SEEK_HOLE "whence" 值。``IOMAP_REPORT`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。)

For unwritten mappings, the pagecache will be searched.
Regions of the pagecache with no folio mapped, or a !uptodate fsblock
within a folio will be reported as sparse hole areas.
(对于未写入的映射，将搜索页缓存。页缓存中没有映射 folio 的区域，或 folio 内 !uptodate 的 fsblock 所在的区域，将被报告为稀疏洞区域。)

Callers commonly hold ``i_rwsem`` in shared mode before calling this
function.
(调用者通常在调用此函数之前以共享模式持有 ``i_rwsem``。)

Swap File Activation
(交换文件激活)
====================

The ``iomap_swapfile_activate`` function finds all the base-page aligned
regions in a file and sets them up as swap space.
The file will be ``fsync()``'d before activation.
``IOMAP_REPORT`` will be passed as the ``flags`` argument to
``->iomap_begin``.
All mappings must be mapped or unwritten; cannot be dirty or shared, and
cannot span multiple block devices.
Callers must hold ``i_rwsem`` in exclusive mode; this is already
provided by ``swapon``.
(``iomap_swapfile_activate`` 函数找出文件中所有按基页对齐的区域，并把它们设置为交换空间。文件在激活之前会被 ``fsync()``。``IOMAP_REPORT`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。所有映射必须是已映射或未写入的；不能是脏的或共享的，并且不能跨越多个块设备。调用者必须以独占模式持有 ``i_rwsem``；这已由 ``swapon`` 提供。)

File Space Mapping Reporting
(文件空间映射报告)
============================

iomap implements two of the file space mapping system calls.
(iomap 实现了两个文件空间映射系统调用。)

FS_IOC_FIEMAP
(FS_IOC_FIEMAP，FIEMAP ioctl)
-------------

The ``iomap_fiemap`` function exports file extent mappings to userspace
in the format specified by the ``FS_IOC_FIEMAP`` ioctl.
``IOMAP_REPORT`` will be passed as the ``flags`` argument to
``->iomap_begin``.
Callers commonly hold ``i_rwsem`` in shared mode before calling this
function.
(``iomap_fiemap`` 函数以 ``FS_IOC_FIEMAP`` ioctl 指定的格式把文件扩展映射导出到用户空间。``IOMAP_REPORT`` 将作为 ``flags`` 参数传给 ``->iomap_begin``。调用者通常在调用此函数之前以共享模式持有 ``i_rwsem``。)

FIBMAP (deprecated)
(FIBMAP(已弃用))
-------------------

``iomap_bmap`` implements FIBMAP.
The calling conventions are the same as for FIEMAP.
This function is only provided to maintain compatibility for filesystems
that implemented FIBMAP prior to conversion.
This ioctl is deprecated; do **not** add a FIBMAP implementation to
filesystems that do not have it.
Callers should probably hold ``i_rwsem`` in shared mode before calling
this function, but this is unclear.
(``iomap_bmap`` 实现 FIBMAP。调用约定与 FIEMAP 相同。提供此函数只是为了给在转换之前就实现了 FIBMAP 的文件系统保持兼容性。此 ioctl 已弃用；切勿向尚未拥有它的文件系统添加 FIBMAP 实现。调用者在调用此函数之前或许应以共享模式持有 ``i_rwsem``，但这一点并不明确。)
