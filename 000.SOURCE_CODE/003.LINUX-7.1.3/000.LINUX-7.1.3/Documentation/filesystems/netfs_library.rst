.. SPDX-License-Identifier: GPL-2.0

===================================
Network Filesystem Services Library
===================================

(网络文件系统服务库)

.. Contents:

 - Overview.
   - Requests and streams.
   - Subrequests.
   - Result collection and retry.
   - Local caching.
   - Content encryption (fscrypt).
 - Per-inode context.
   - Inode context helper functions.
   - Inode locking.
   - Inode writeback.
 - High-level VFS API.
   - Unlocked read/write iter.
   - Pre-locked read/write iter.
   - Monolithic files API.
   - Memory-mapped I/O API.
 - High-level VM API.
   - Deprecated PG_private2 API.
 - I/O request API.
   - Request structure.
   - Stream structure.
   - Subrequest structure.
   - Filesystem methods.
   - Terminating a subrequest.
   - Local cache API.
 - API function reference.


Overview
(概述)
========

The network filesystem services library, netfslib, is a set of functions
designed to aid a network filesystem in implementing VM/VFS API operations.  It
takes over the normal buffered read, readahead, write and writeback and also
handles unbuffered and direct I/O.
(网络文件系统服务库 netfslib 是一组函数，旨在帮助网络文件系统实现 VM/VFS API 操作。它接管正常的缓冲读取、预读(readahead)、写入和回写(writeback)，并处理无缓冲 I/O 和直接 I/O。)

The library provides support for (re-)negotiation of I/O sizes and retrying
failed I/O as well as local caching and will, in the future, provide content
encryption.
(该库提供对 I/O 大小(重新)协商和失败 I/O 重试的支持，以及本地缓存，并且将来会提供内容加密。)

It insulates the filesystem from VM interface changes as much as possible and
handles VM features such as large multipage folios.  The filesystem basically
just has to provide a way to perform read and write RPC calls.
(它尽可能地将文件系统与 VM 接口的变化隔离开来，并处理诸如大多页 folio 之类的 VM 特性。文件系统基本上只需提供一种执行读和写 RPC 调用的方式。)

The way I/O is organised inside netfslib consists of a number of objects:
(netfslib 内部组织 I/O 的方式由若干对象组成：)

 * A *request*.  A request is used to track the progress of the I/O overall and
   to hold on to resources.  The collection of results is done at the request
   level.  The I/O within a request is divided into a number of parallel
   streams of subrequests.
   (一个 *request* (请求)。请求用于跟踪整体 I/O 的进度并持有资源。结果的收集在请求级别完成。一个请求内的 I/O 被划分为多个并行的子请求(subrequest)流。)

 * A *stream*.  A non-overlapping series of subrequests.  The subrequests
   within a stream do not have to be contiguous.
   (一个 *stream* (流)。一系列互不重叠的子请求。一个流内的子请求不必是连续的。)

 * A *subrequest*.  This is the basic unit of I/O.  It represents a single RPC
   call or a single cache I/O operation.  The library passes these to the
   filesystem and the cache to perform.
   (一个 *subrequest* (子请求)。这是 I/O 的基本单位。它代表单个 RPC 调用或单个缓存 I/O 操作。库将这些子请求传递给文件系统和缓存去执行。)

Requests and Streams
(请求与流)
--------------------

When actually performing I/O (as opposed to just copying into the pagecache),
netfslib will create one or more requests to track the progress of the I/O and
to hold resources.
(在实际执行 I/O 时(相对于仅仅复制到页缓存而言)，netfslib 会创建一个或多个请求来跟踪 I/O 的进度并持有资源。)

A read operation will have a single stream and the subrequests within that
stream may be of mixed origins, for instance mixing RPC subrequests and cache
subrequests.
(读操作将只有一个流，该流内的子请求可以具有混合的来源，例如混合 RPC 子请求和缓存子请求。)

On the other hand, a write operation may have multiple streams, where each
stream targets a different destination.  For instance, there may be one stream
writing to the local cache and one to the server.  Currently, only two streams
are allowed, but this could be increased if parallel writes to multiple servers
is desired.
(另一方面，写操作可能有多个流，每个流指向不同的目标。例如，可能有一个流写入本地缓存，另一个流写入服务器。目前只允许两个流，但如果需要对多个服务器进行并行写入，这一数量可以增加。)

The subrequests within a write stream do not need to match alignment or size
with the subrequests in another write stream and netfslib performs the tiling
of subrequests in each stream over the source buffer independently.  Further,
each stream may contain holes that don't correspond to holes in the other
stream.
(一个写流内的子请求不需要在对齐或大小上与另一个写流中的子请求匹配，netfslib 会在每个流中独立地在源缓冲区上对子请求进行平铺(tiling)。此外，每个流可以包含与另一个流中的空洞不对应的空洞。)

In addition, the subrequests do not need to correspond to the boundaries of the
folios or vectors in the source/destination buffer.  The library handles the
collection of results and the wrangling of folio flags and references.
(此外，子请求不需要对应源/目标缓冲区中 folio 或向量的边界。该库会处理结果的收集以及 folio 标志和引用的打理。)

Subrequests
(子请求)
-----------

Subrequests are at the heart of the interaction between netfslib and the
filesystem using it.  Each subrequest is expected to correspond to a single
read or write RPC or cache operation.  The library will stitch together the
results from a set of subrequests to provide a higher level operation.
(子请求是 netfslib 与使用它的文件系统之间交互的核心。每个子请求预期对应单个读或写 RPC 或缓存操作。该库会将一组子请求的结果拼接起来，以提供更高级别的操作。)

Netfslib has two interactions with the filesystem or the cache when setting up
a subrequest.  First, there's an optional preparatory step that allows the
filesystem to negotiate the limits on the subrequest, both in terms of maximum
number of bytes and maximum number of vectors (e.g. for RDMA).  This may
involve negotiating with the server (e.g. cifs needing to acquire credits).
(在设置子请求时，netfslib 与文件系统或缓存有两种交互。首先，有一个可选的准备步骤，允许文件系统协商子请求的限制，包括最大字节数和最大向量数(例如 RDMA)。这可能涉及与服务器协商(例如 cifs 需要获取 credits)。)

And, secondly, there's the issuing step in which the subrequest is handed off
to the filesystem to perform.
(其次，是下发步骤，在该步骤中子请求被移交给文件系统去执行。)

Note that these two steps are done slightly differently between read and write:
(注意，这两个步骤在读和写之间的处理略有不同：)

 * For reads, the VM/VFS tells us how much is being requested up front, so the
   library can preset maximum values that the cache and then the filesystem can
   then reduce.  The cache also gets consulted first on whether it wants to do
   a read before the filesystem is consulted.
   (对于读，VM/VFS 会预先告诉我们请求的量有多大，因此库可以预设最大值，缓存和文件系统随后可以将其减小。在咨询文件系统之前，也会先咨询缓存是否想要进行读取。)

 * For writeback, it is unknown how much there will be to write until the
   pagecache is walked, so no limit is set by the library.
   (对于回写(writeback)，在遍历页缓存之前无法知道将有多少内容要写，因此库不设置限制。)

Once a subrequest is completed, the filesystem or cache informs the library of
the completion and then collection is invoked.  Depending on whether the
request is synchronous or asynchronous, the collection of results will be done
in either the application thread or in a work queue.
(一旦子请求完成，文件系统或缓存会将完成情况通知该库，然后调用收集(collection)。根据请求是同步还是异步，结果收集将在应用程序线程或工作队列中完成。)

Result Collection and Retry
(结果收集与重试)
---------------------------

As subrequests complete, the results are collected and collated by the library
and folio unlocking is performed progressively (if appropriate).  Once the
request is complete, async completion will be invoked (again, if appropriate).
It is possible for the filesystem to provide interim progress reports to the
library to cause folio unlocking to happen earlier if possible.
(随着子请求完成，结果由该库收集和整理，并(在适当时)逐步执行 folio 解锁。一旦请求完成，将调用异步完成(同样，如果适当时)。文件系统可以向该库提供中期进度报告，以便在可能的情况下更早地解锁 folio。)

If any subrequests fail, netfslib can retry them.  It will wait until all
subrequests are completed, offer the filesystem the opportunity to fiddle with
the resources/state held by the request and poke at the subrequests before
re-preparing and re-issuing the subrequests.
(如果任何子请求失败，netfslib 可以重试它们。它会等到所有子请求都完成，让文件系统有机会调整请求所持有的资源/状态，并在重新准备和重新下发子请求之前修改这些子请求。)

This allows the tiling of contiguous sets of failed subrequest within a stream
to be changed, adding more subrequests or ditching excess as necessary (for
instance, if the network sizes change or the server decides it wants smaller
chunks).
(这允许改变一个流内连续失败子请求集合的平铺方式，根据需要添加更多子请求或丢弃多余的子请求(例如，如果网络大小发生变化，或者服务器决定需要更小的块)。)

Further, if one or more contiguous cache-read subrequests fail, the library
will pass them to the filesystem to perform instead, renegotiating and retiling
them as necessary to fit with the filesystem's parameters rather than those of
the cache.
(此外，如果一个或多个连续的缓存读子请求失败，该库会将它们转交给文件系统来执行，并根据文件系统的参数(而非缓存的参数)按需重新协商和重新平铺它们。)

Local Caching
(本地缓存)
-------------

One of the services netfslib provides, via ``fscache``, is the option to cache
on local disk a copy of the data obtained from/written to a network filesystem.
The library will manage the storing, retrieval and some invalidation of data
automatically on behalf of the filesystem if a cookie is attached to the
``netfs_inode``.
(netfslib 通过 ``fscache`` 提供的服务之一，是可选择把从网络文件系统获取/写入的数据的一份副本缓存到本地磁盘上。如果一个 cookie 被附加到 ``netfs_inode`` 上，该库将代表文件系统自动管理数据的存储、检索和某些失效处理。)

Note that local caching used to use the PG_private_2 (aliased as PG_fscache) to
keep track of a page that was being written to the cache, but this is now
deprecated as PG_private_2 will be removed.
(注意，本地缓存过去使用 PG_private_2(别名为 PG_fscache)来跟踪正在写入缓存的页，但由于 PG_private_2 将被移除，这种方式现已被弃用。)

Instead, folios that are read from the server for which there was no data in
the cache will be marked as dirty and will have ``folio->private`` set to a
special value (``NETFS_FOLIO_COPY_TO_CACHE``) and left to writeback to write.
If the folio is modified before that happened, the special value will be
cleared and the write will become normally dirty.
(相反，从服务器读取的、缓存中没有其数据的 folio 将被标记为脏，并且 ``folio->private`` 会被设置为一个特殊值(``NETFS_FOLIO_COPY_TO_CACHE``)，并留给回写去写入。如果 folio 在此发生之前被修改，该特殊值将被清除，写入将变为正常的脏页。)

When writeback occurs, folios that are so marked will only be written to the
cache and not to the server.  Writeback handles mixed cache-only writes and
server-and-cache writes by using two streams, sending one to the cache and one
to the server.  The server stream will have gaps in it corresponding to those
folios.
(当回写发生时，如此标记的 folio 将只写入缓存而不写入服务器。回写通过使用两个流来处理混合的仅缓存写入和服务器与缓存同时写入，一个流发送到缓存，另一个流发送到服务器。服务器流中将存在与这些 folio 对应的空隙。)

Content Encryption (fscrypt)
(内容加密 (fscrypt))
----------------------------

Though it does not do so yet, at some point netfslib will acquire the ability
to do client-side content encryption on behalf of the network filesystem (Ceph,
for example).  fscrypt can be used for this if appropriate (it may not be -
cifs, for example).
(虽然目前尚未这么做，但 netfslib 终将在某个时刻获得代表网络文件系统(例如 Ceph)执行客户端内容加密的能力。如果合适，可以使用 fscrypt 来实现(也可能不合适，例如 cifs)。)

The data will be stored encrypted in the local cache using the same manner of
encryption as the data written to the server and the library will impose bounce
buffering and RMW cycles as necessary.
(数据将以与写入服务器的数据相同的加密方式加密存储在本地缓存中，并且该库会根据需要施加缓冲中转(bounce buffering)和读改写(RMW)循环。)


Per-Inode Context
(每 inode 上下文)
=================

The network filesystem helper library needs a place to store a bit of state for
its use on each netfs inode it is helping to manage.  To this end, a context
structure is defined (网络文件系统辅助库需要一个地方，来为其协助管理的每个 netfs inode 存储一些供其使用的状态。为此，定义了一个上下文结构)::

	struct netfs_inode {
		struct inode inode;
		const struct netfs_request_ops *ops;
		struct fscache_cookie * cache;
		loff_t remote_i_size;
		unsigned long flags;
		...
	};

A network filesystem that wants to use netfslib must place one of these in its
inode wrapper struct instead of the VFS ``struct inode``.  This can be done in
a way similar to the following (想要使用 netfslib 的网络文件系统必须在它的 inode 包装结构中放置这样一个结构，以取代 VFS 的 ``struct inode``。这可以通过类似如下的方式完成)::

	struct my_inode {
		struct netfs_inode netfs; /* Netfslib context and vfs inode */
		...
	};

This allows netfslib to find its state by using ``container_of()`` from the
inode pointer, thereby allowing the netfslib helper functions to be pointed to
directly by the VFS/VM operation tables.
(这使得 netfslib 能够通过使用来自 inode 指针的 ``container_of()`` 来找到其状态，从而允许 VFS/VM 操作表直接指向 netfslib 辅助函数。)

The structure contains the following fields that are of interest to the
filesystem:
(该结构包含以下文件系统感兴趣的域：)

 * ``inode``

   The VFS inode structure.
   (VFS inode 结构。)

 * ``ops``

   The set of operations provided by the network filesystem to netfslib.
   (由网络文件系统提供给 netfslib 的操作集合。)

 * ``cache``

   Local caching cookie, or NULL if no caching is enabled.  This field does not
   exist if fscache is disabled.
   (本地缓存 cookie，如果未启用缓存则为 NULL。如果 fscache 被禁用，则该域不存在。)

 * ``remote_i_size``

   The size of the file on the server.  This differs from inode->i_size if
   local modifications have been made but not yet written back.
   (文件在服务器上的大小。如果已有本地修改但尚未回写，则它与 inode->i_size 不同。)

 * ``flags``

   A set of flags, some of which the filesystem might be interested in:
   (一组标志，其中一些文件系统可能会感兴趣：)

   * ``NETFS_ICTX_MODIFIED_ATTR``

     Set if netfslib modifies mtime/ctime.  The filesystem is free to ignore
     this or clear it.
     (如果 netfslib 修改了 mtime/ctime 则设置。文件系统可以随意忽略它或清除它。)

   * ``NETFS_ICTX_UNBUFFERED``

     Do unbuffered I/O upon the file.  Like direct I/O but without the
     alignment limitations.  RMW will be performed if necessary.  The pagecache
     will not be used unless mmap() is also used.
     (对文件执行无缓冲 I/O。类似于直接 I/O 但没有对齐限制。必要时会执行读改写(RMW)。除非同时使用 mmap()，否则不会使用页缓存。)

   * ``NETFS_ICTX_WRITETHROUGH``

     Do writethrough caching upon the file.  I/O will be set up and dispatched
     as buffered writes are made to the page cache.  mmap() does the normal
     writeback thing.
     (对文件执行写穿(writethrough)缓存。当向页缓存进行缓冲写入时，I/O 会被建立并下发。mmap() 则执行正常的回写。)

   * ``NETFS_ICTX_SINGLE_NO_UPLOAD``

     Set if the file has a monolithic content that must be read entirely in a
     single go and must not be written back to the server, though it can be
     cached (e.g. AFS directories).
     (如果文件具有必须一次性完整读取的单体内容、且不得写回服务器，则设置此标志，尽管它可以被缓存(例如 AFS 目录)。)

Inode Context Helper Functions
(inode 上下文辅助函数)
------------------------------

To help deal with the per-inode context, a number helper functions are
provided.  Firstly, a function to perform basic initialisation on a context and
set the operations table pointer (为帮助处理每 inode 上下文，提供了多个辅助函数。首先，是一个对上下文执行基本初始化并设置操作表指针的函数)::

	void netfs_inode_init(struct netfs_inode *ctx,
			      const struct netfs_request_ops *ops);

then a function to cast from the VFS inode structure to the netfs context (然后是一个从 VFS inode 结构转换为 netfs 上下文的函数)::

	struct netfs_inode *netfs_inode(struct inode *inode);

and finally, a function to get the cache cookie pointer from the context
attached to an inode (or NULL if fscache is disabled) (最后，是一个从附加到 inode 的上下文获取缓存 cookie 指针的函数(如果 fscache 被禁用则为 NULL))::

	struct fscache_cookie *netfs_i_cookie(struct netfs_inode *ctx);

Inode Locking
(inode 锁定)
-------------

A number of functions are provided to manage the locking of i_rwsem for I/O and
to effectively extend it to provide more separate classes of exclusion (提供了一些函数来管理用于 I/O 的 i_rwsem 锁定，并有效地扩展它以提供更多单独的排除类别)::

	int netfs_start_io_read(struct inode *inode);
	void netfs_end_io_read(struct inode *inode);
	int netfs_start_io_write(struct inode *inode);
	void netfs_end_io_write(struct inode *inode);
	int netfs_start_io_direct(struct inode *inode);
	void netfs_end_io_direct(struct inode *inode);

The exclusion breaks down into four separate classes:
(排除关系分为四个单独的类别：)

 1) Buffered reads and writes.
    (缓冲读和缓冲写。)

    Buffered reads can run concurrently each other and with buffered writes,
    but buffered writes cannot run concurrently with each other.
    (缓冲读可以彼此并发运行，也可以与缓冲写并发运行，但缓冲写不能彼此并发运行。)

 2) Direct reads and writes.
    (直接读和直接写。)

    Direct (and unbuffered) reads and writes can run concurrently since they do
    not share local buffering (i.e. the pagecache) and, in a network
    filesystem, are expected to have exclusion managed on the server (though
    this may not be the case for, say, Ceph).
    (直接(和无缓冲)读和写可以并发运行，因为它们不共享本地缓冲(即页缓存)，并且对于网络文件系统，预期由服务器管理排除(尽管对于例如 Ceph 来说可能并非如此)。)

 3) Other major inode modifying operations (e.g. truncate, fallocate).
    (其他主要的 inode 修改操作(例如 truncate、fallocate)。)

    These should just access i_rwsem directly.
    (这些应该直接访问 i_rwsem。)

 4) mmap().
    (mmap()。)

    mmap'd accesses might operate concurrently with any of the other classes.
    They might form the buffer for an intra-file loopback DIO read/write.  They
    might be permitted on unbuffered files.
    (mmap 的访问可以与任何其他类别并发操作。它们可能构成文件内回环 DIO 读/写的缓冲区。它们可能在无缓冲文件上被允许。)

Inode Writeback
(inode 回写)
---------------

Netfslib will pin resources on an inode for future writeback (such as pinning
use of an fscache cookie) when an inode is dirtied.  However, this pinning
needs careful management.  To manage the pinning, the following sequence
occurs:
(当 inode 变脏时，netfslib 会为将来的回写在 inode 上固定(pin)资源(例如固定 fscache cookie 的使用)。然而，这种固定需要仔细管理。为了管理固定，会发生以下序列：)

 1) An inode state flag ``I_PINNING_NETFS_WB`` is set by netfslib when the
    pinning begins (when a folio is dirtied, for example) if the cache is
    active to stop the cache structures from being discarded and the cache
    space from being culled.  This also prevents re-getting of cache resources
    if the flag is already set.
    (当固定开始时(例如当一个 folio 变脏时)，如果缓存处于活动状态，netfslib 会设置 inode 状态标志 ``I_PINNING_NETFS_WB``，以阻止缓存结构被丢弃和缓存空间被清除。如果该标志已被设置，这也防止重新获取缓存资源。)

 2) This flag then cleared inside the inode lock during inode writeback in the
    VM - and the fact that it was set is transferred to ``->unpinned_netfs_wb``
    in ``struct writeback_control``.
    (然后在 VM 中的 inode 回写期间，该标志会在 inode 锁内被清除——并且它已被设置这一事实会被传递给 ``struct writeback_control`` 中的 ``->unpinned_netfs_wb``。)

 3) If ``->unpinned_netfs_wb`` is now set, the write_inode procedure is forced.
    (如果 ``->unpinned_netfs_wb`` 此时被设置，则强制执行 write_inode 过程。)

 4) The filesystem's ``->write_inode()`` function is invoked to do the cleanup.
    (调用文件系统的 ``->write_inode()`` 函数来进行清理。)

 5) The filesystem invokes netfs to do its cleanup.
    (文件系统调用 netfs 来进行其清理。)

To do the cleanup, netfslib provides a function to do the resource unpinning (为了进行清理，netfslib 提供了一个执行资源解固定(unpinning)的函数)::

	int netfs_unpin_writeback(struct inode *inode, struct writeback_control *wbc);

If the filesystem doesn't need to do anything else, this may be set as a its
``.write_inode`` method.
(如果文件系统不需要做任何其他事情，可以将其设置为它的 ``.write_inode`` 方法。)

Further, if an inode is deleted, the filesystem's write_inode method may not
get called, so (此外，如果 inode 被删除，文件系统的 write_inode 方法可能不会被调用，因此)::

	void netfs_clear_inode_writeback(struct inode *inode, const void *aux);

must be called from ``->evict_inode()`` *before* ``clear_inode()`` is called.
(必须在调用 ``clear_inode()`` *之前*，从 ``->evict_inode()`` 中调用它。)


High-Level VFS API
(高层 VFS API)
==================

Netfslib provides a number of sets of API calls for the filesystem to delegate
VFS operations to.  Netfslib, in turn, will call out to the filesystem and the
cache to negotiate I/O sizes, issue RPCs and provide places for it to intervene
at various times.
(netfslib 提供了多组 API 调用，供文件系统将 VFS 操作委托给它。而 netfslib 反过来会调用文件系统和缓存来协商 I/O 大小、下发 RPC，并提供可在不同时间介入的位置。)

Unlocked Read/Write Iter
(未锁定读/写 iter)
------------------------

The first API set is for the delegation of operations to netfslib when the
filesystem is called through the standard VFS read/write_iter methods (第一组 API 用于当文件系统通过标准 VFS read/write_iter 方法被调用时，将操作委托给 netfslib)::

	ssize_t netfs_file_read_iter(struct kiocb *iocb, struct iov_iter *iter);
	ssize_t netfs_file_write_iter(struct kiocb *iocb, struct iov_iter *from);
	ssize_t netfs_buffered_read_iter(struct kiocb *iocb, struct iov_iter *iter);
	ssize_t netfs_unbuffered_read_iter(struct kiocb *iocb, struct iov_iter *iter);
	ssize_t netfs_unbuffered_write_iter(struct kiocb *iocb, struct iov_iter *from);

They can be assigned directly to ``.read_iter`` and ``.write_iter``.  They
perform the inode locking themselves and the first two will switch between
buffered I/O and DIO as appropriate.
(它们可以直接赋给 ``.read_iter`` 和 ``.write_iter``。它们自己执行 inode 锁定，并且前两个会在缓冲 I/O 和 DIO 之间酌情切换。)

Pre-Locked Read/Write Iter
(预锁定读/写 iter)
--------------------------

The second API set is for the delegation of operations to netfslib when the
filesystem is called through the standard VFS methods, but needs to do some
other stuff before or after calling netfslib whilst still inside locked section
(e.g. Ceph negotiating caps).  The unbuffered read function is (第二组 API 用于当文件系统通过标准 VFS 方法被调用、但仍需要在锁定区域内，在调用 netfslib 之前或之后做一些其他事情(例如 Ceph 协商 caps)的情况。无缓冲读函数是)::

	ssize_t netfs_unbuffered_read_iter_locked(struct kiocb *iocb, struct iov_iter *iter);

This must not be assigned directly to ``.read_iter`` and the filesystem is
responsible for performing the inode locking before calling it.  In the case of
buffered read, the filesystem should use ``filemap_read()``.
(这不得直接赋给 ``.read_iter``，文件系统负责在调用它之前执行 inode 锁定。对于缓冲读，文件系统应使用 ``filemap_read()``。)

There are three functions for writes (有三个用于写的函数)::

	ssize_t netfs_buffered_write_iter_locked(struct kiocb *iocb, struct iov_iter *from,
						 struct netfs_group *netfs_group);
	ssize_t netfs_perform_write(struct kiocb *iocb, struct iov_iter *iter,
				    struct netfs_group *netfs_group);
	ssize_t netfs_unbuffered_write_iter_locked(struct kiocb *iocb, struct iov_iter *iter,
						   struct netfs_group *netfs_group);

These must not be assigned directly to ``.write_iter`` and the filesystem is
responsible for performing the inode locking before calling them.
(这些不得直接赋给 ``.write_iter``，文件系统负责在调用它们之前执行 inode 锁定。)

The first two functions are for buffered writes; the first just adds some
standard write checks and jumps to the second, but if the filesystem wants to
do the checks itself, it can use the second directly.  The third function is
for unbuffered or DIO writes.
(前两个函数用于缓冲写；第一个只是添加一些标准的写检查并跳转到第二个，但如果文件系统想自己进行检查，可以直接使用第二个。第三个函数用于无缓冲或 DIO 写。)

On all three write functions, there is a writeback group pointer (which should
be NULL if the filesystem doesn't use this).  Writeback groups are set on
folios when they're modified.  If a folio to-be-modified is already marked with
a different group, it is flushed first.  The writeback API allows writing back
of a specific group.
(在所有这三个写函数上，都有一个回写组指针(如果文件系统不使用它，则应为 NULL)。回写组在 folio 被修改时设置到 folio 上。如果要修改的 folio 已被标记了不同的组，则会先将其刷写。回写 API 允许写回特定的组。)

Memory-Mapped I/O API
(内存映射 I/O API)
---------------------

An API for support of mmap()'d I/O is provided (提供了用于支持 mmap I/O 的 API)::

	vm_fault_t netfs_page_mkwrite(struct vm_fault *vmf, struct netfs_group *netfs_group);

This allows the filesystem to delegate ``.page_mkwrite`` to netfslib.  The
filesystem should not take the inode lock before calling it, but, as with the
locked write functions above, this does take a writeback group pointer.  If the
page to be made writable is in a different group, it will be flushed first.
(这允许文件系统将 ``.page_mkwrite`` 委托给 netfslib。文件系统在调用它之前不应获取 inode 锁，但与上面的锁定写函数一样，它确实接受一个回写组指针。如果将要变为可写的页位于不同的组中，则会先将其刷写。)

Monolithic Files API
(单体文件 API)
--------------------

There is also a special API set for files for which the content must be read in
a single RPC (and not written back) and is maintained as a monolithic blob
(e.g. an AFS directory), though it can be stored and updated in the local cache (还有一组特殊的 API，用于那些内容必须在单个 RPC 中读取(且不写回)、并作为单体 blob 维护(例如 AFS 目录)的文件，不过它可以被存储并更新到本地缓存中)::

	ssize_t netfs_read_single(struct inode *inode, struct file *file, struct iov_iter *iter);
	void netfs_single_mark_inode_dirty(struct inode *inode);
	int netfs_writeback_single(struct address_space *mapping,
				   struct writeback_control *wbc,
				   struct iov_iter *iter);

The first function reads from a file into the given buffer, reading from the
cache in preference if the data is cached there; the second function allows the
inode to be marked dirty, causing a later writeback; and the third function can
be called from the writeback code to write the data to the cache, if there is
one.
(第一个函数从一个文件读取到给定的缓冲区中，如果数据已缓存在其中，则优先从缓存读取；第二个函数允许将 inode 标记为脏，从而导致稍后的回写；第三个函数可以从回写代码中调用，以将数据写入缓存(如果存在缓存)。)

The inode should be marked ``NETFS_ICTX_SINGLE_NO_UPLOAD`` if this API is to be
used.  The writeback function requires the buffer to be of ITER_FOLIOQ type.
(如果要使用此 API，inode 应被标记为 ``NETFS_ICTX_SINGLE_NO_UPLOAD``。回写函数要求缓冲区为 ITER_FOLIOQ 类型。)

High-Level VM API
(高层 VM API)
==================

Netfslib also provides a number of sets of API calls for the filesystem to
delegate VM operations to.  Again, netfslib, in turn, will call out to the
filesystem and the cache to negotiate I/O sizes, issue RPCs and provide places
for it to intervene at various times (netfslib 还提供了多组 API 调用，供文件系统将 VM 操作委托给它。同样，netfslib 反过来会调用文件系统和缓存来协商 I/O 大小、下发 RPC，并提供可在不同时间介入的位置)::

	void netfs_readahead(struct readahead_control *);
	int netfs_read_folio(struct file *, struct folio *);
	int netfs_writepages(struct address_space *mapping,
			     struct writeback_control *wbc);
	bool netfs_dirty_folio(struct address_space *mapping, struct folio *folio);
	void netfs_invalidate_folio(struct folio *folio, size_t offset, size_t length);
	bool netfs_release_folio(struct folio *folio, gfp_t gfp);

These are ``address_space_operations`` methods and can be set directly in the
operations table.
(这些是 ``address_space_operations`` 方法，可以直接在操作表中设置。)

Deprecated PG_private_2 API
(已弃用的 PG_private_2 API)
---------------------------

There is also a deprecated function for filesystems that still use the
``->write_begin`` method (还有一个已弃用的函数，供仍在使用 ``->write_begin`` 方法的文件系统使用)::

	int netfs_write_begin(struct netfs_inode *inode, struct file *file,
			      struct address_space *mapping, loff_t pos, unsigned int len,
			      struct folio **_folio, void **_fsdata);

It uses the deprecated PG_private_2 flag and so should not be used.
(它使用已弃用的 PG_private_2 标志，因此不应使用。)


I/O Request API
(I/O 请求 API)
===============

The I/O request API comprises a number of structures and a number of functions
that the filesystem may need to use.
(I/O 请求 API 包含若干结构以及文件系统可能需要使用的若干函数。)

Request Structure
(请求结构)
-----------------

The request structure manages the request as a whole, holding some resources
and state on behalf of the filesystem and tracking the collection of results (请求结构对请求进行整体管理，代表文件系统持有一些资源和状态，并跟踪结果的收集)::

	struct netfs_io_request {
		enum netfs_io_origin	origin;
		struct inode		*inode;
		struct address_space	*mapping;
		struct netfs_group	*group;
		struct netfs_io_stream	io_streams[];
		void			*netfs_priv;
		void			*netfs_priv2;
		unsigned long long	start;
		unsigned long long	len;
		unsigned long long	i_size;
		unsigned int		debug_id;
		unsigned long		flags;
		...
	};

Many of the fields are for internal use, but the fields shown here are of
interest to the filesystem:
(许多域供内部使用，但这里显示的域是文件系统感兴趣的：)

 * ``origin``

   The origin of the request (readahead, read_folio, DIO read, writeback, ...).
   (请求的来源(readahead、read_folio、DIO 读、回写、...)。)

 * ``inode``
 * ``mapping``

   The inode and the address space of the file being read from.  The mapping
   may or may not point to inode->i_data.
   (被读取文件的 inode 和地址空间。mapping 可能指向也可能不指向 inode->i_data。)

 * ``group``

   The writeback group this request is dealing with or NULL.  This holds a ref
   on the group.
   (此请求正在处理的回写组，或 NULL。它持有对该组的一个引用。)

 * ``io_streams``

   The parallel streams of subrequests available to the request.  Currently two
   are available, but this may be made extensible in future.  ``NR_IO_STREAMS``
   indicates the size of the array.
   (请求可用的并行子请求流。目前有两个可用，但将来可能变为可扩展。``NR_IO_STREAMS`` 指示数组的大小。)

 * ``netfs_priv``
 * ``netfs_priv2``

   The network filesystem's private data.  The value for this can be passed in
   to the helper functions or set during the request.
   (网络文件系统的私有数据。其值可以传入辅助函数，或在请求期间设置。)

 * ``start``
 * ``len``

   The file position of the start of the read request and the length.  These
   may be altered by the ->expand_readahead() op.
   (读请求起始处的文件位置和长度。它们可能被 ->expand_readahead() 操作改变。)

 * ``i_size``

   The size of the file at the start of the request.
   (请求开始时文件的大小。)

 * ``debug_id``

   A number allocated to this operation that can be displayed in trace lines
   for reference.
   (分配给此操作的一个编号，可以显示在跟踪行中以供参考。)

 * ``flags``

   Flags for managing and controlling the operation of the request.  Some of
   these may be of interest to the filesystem:
   (用于管理和控制请求操作的标志。其中一些可能为文件系统所关注：)

   * ``NETFS_RREQ_RETRYING``

     Netfslib sets this when generating retries.
     (netfslib 在生成重试时设置此项。)

   * ``NETFS_RREQ_PAUSE``

     The filesystem can set this to request to pause the library's subrequest
     issuing loop - but care needs to be taken as netfslib may also set it.
     (文件系统可以设置此项，以请求暂停库的子请求下发循环——但需要小心，因为 netfslib 也可能设置它。)

   * ``NETFS_RREQ_NONBLOCK``
   * ``NETFS_RREQ_BLOCKED``

     Netfslib sets the first to indicate that non-blocking mode was set by the
     caller and the filesystem can set the second to indicate that it would
     have had to block.
     (netfslib 设置第一个以指示调用者设置了非阻塞模式，文件系统可以设置第二个以指示它本将不得不阻塞。)

   * ``NETFS_RREQ_USE_PGPRIV2``

     The filesystem can set this if it wants to use PG_private_2 to track
     whether a folio is being written to the cache.  This is deprecated as
     PG_private_2 is going to go away.
     (如果文件系统想使用 PG_private_2 来跟踪 folio 是否正在写入缓存，可以设置此项。由于 PG_private_2 即将被移除，此项已弃用。)

If the filesystem wants more private data than is afforded by this structure,
then it should wrap it and provide its own allocator.
(如果文件系统需要比此结构所能提供的更多的私有数据，则应包装该结构并提供自己的分配器。)

Stream Structure
(流结构)
----------------

A request is comprised of one or more parallel streams and each stream may be
aimed at a different target.
(一个请求由一个或多个并行流组成，每个流可以指向不同的目标。)

For read requests, only stream 0 is used.  This can contain a mixture of
subrequests aimed at different sources.  For write requests, stream 0 is used
for the server and stream 1 is used for the cache.  For buffered writeback,
stream 0 is not enabled unless a normal dirty folio is encountered, at which
point ->begin_writeback() will be invoked and the filesystem can mark the
stream available.
(对于读请求，仅使用流 0。它可以包含指向不同来源的子请求的混合。对于写请求，流 0 用于服务器，流 1 用于缓存。对于缓冲回写，流 0 不会启用，除非遇到一个正常的脏 folio，此时将调用 ->begin_writeback()，文件系统可以将该流标记为可用。)

The stream struct looks like (流结构如下所示)::

	struct netfs_io_stream {
		unsigned char		stream_nr;
		bool			avail;
		size_t			sreq_max_len;
		unsigned int		sreq_max_segs;
		unsigned int		submit_extendable_to;
		...
	};

A number of members are available for access/use by the filesystem:
(以下成员可供文件系统访问/使用：)

 * ``stream_nr``

   The number of the stream within the request.
   (请求中该流的编号。)

 * ``avail``

   True if the stream is available for use.  The filesystem should set this on
   stream zero if in ->begin_writeback().
   (如果该流可供使用则为 true。如果在 ->begin_writeback() 中，文件系统应对流 0 设置此项。)

 * ``sreq_max_len``
 * ``sreq_max_segs``

   These are set by the filesystem or the cache in ->prepare_read() or
   ->prepare_write() for each subrequest to indicate the maximum number of
   bytes and, optionally, the maximum number of segments (if not 0) that that
   subrequest can support.
   (这些由文件系统或缓存在 ->prepare_read() 或 ->prepare_write() 中为每个子请求设置，以指示该子请求可以支持的最大字节数，以及(如果不为 0)最大段数。)

 * ``submit_extendable_to``

   The size that a subrequest can be rounded up to beyond the EOF, given the
   available buffer.  This allows the cache to work out if it can do a DIO read
   or write that straddles the EOF marker.
   (在给定可用缓冲区的情况下，子请求可以向上取整超出 EOF 的部分的大小。这允许缓存判断它是否可以执行跨越 EOF 标记的 DIO 读或写。)

Subrequest Structure
(子请求结构)
--------------------

Individual units of I/O are managed by the subrequest structure.  These
represent slices of the overall request and run independently (I/O 的各个单独单元由子请求结构管理。它们代表整体请求的切片并独立运行)::

	struct netfs_io_subrequest {
		struct netfs_io_request *rreq;
		struct iov_iter		io_iter;
		unsigned long long	start;
		size_t			len;
		size_t			transferred;
		unsigned long		flags;
		short			error;
		unsigned short		debug_index;
		unsigned char		stream_nr;
		...
	};

Each subrequest is expected to access a single source, though the library will
handle falling back from one source type to another.  The members are:
(每个子请求预期访问单一来源，不过该库会处理从一种来源类型到另一种来源类型的回退。其成员如下：)

 * ``rreq``

   A pointer to the read request.
   (指向读请求的指针。)

 * ``io_iter``

   An I/O iterator representing a slice of the buffer to be read into or
   written from.
   (一个 I/O 迭代器，表示要读入或写出的缓冲区的一个切片。)

 * ``start``
 * ``len``

   The file position of the start of this slice of the read request and the
   length.
   (读请求此切片起始处的文件位置和长度。)

 * ``transferred``

   The amount of data transferred so far for this subrequest.  This should be
   added to with the length of the transfer made by this issuance of the
   subrequest.  If this is less than ``len`` then the subrequest may be
   reissued to continue.
   (此子请求到目前为止已传输的数据量。应该加上本次下发该子请求所传输的长度。如果它小于 ``len``，则可以重新下发该子请求以继续。)

 * ``flags``

   Flags for managing the subrequest.  There are a number of interest to the
   filesystem or cache:
   (用于管理子请求的标志。其中有一些是文件系统或缓存关注的：)

   * ``NETFS_SREQ_MADE_PROGRESS``

     Set by the filesystem to indicates that at least one byte of data was read
     or written.
     (由文件系统设置，以指示至少读取或写入了一个字节的数据。)

   * ``NETFS_SREQ_HIT_EOF``

     The filesystem should set this if a read hit the EOF on the file (in which
     case ``transferred`` should stop at the EOF).  Netfslib may expand the
     subrequest out to the size of the folio containing the EOF on the off
     chance that a third party change happened or a DIO read may have asked for
     more than is available.  The library will clear any excess pagecache.
     (如果一次读取命中了文件的 EOF，文件系统应设置此项(在这种情况下 ``transferred`` 应停在 EOF 处)。netfslib 可能会把子请求扩展到包含 EOF 的 folio 的大小，以防万一发生了第三方更改，或者 DIO 读可能请求了超过可用量的数据。该库会清除任何多余的页缓存。)

   * ``NETFS_SREQ_CLEAR_TAIL``

     The filesystem can set this to indicate that the remainder of the slice,
     from transferred to len, should be cleared.  Do not set if HIT_EOF is set.
     (文件系统可以设置此项，以指示应清除切片的剩余部分(从 transferred 到 len)。如果设置了 HIT_EOF 则不要设置。)

   * ``NETFS_SREQ_NEED_RETRY``

     The filesystem can set this to tell netfslib to retry the subrequest.
     (文件系统可以设置此项，以告诉 netfslib 重试该子请求。)

   * ``NETFS_SREQ_BOUNDARY``

     This can be set by the filesystem on a subrequest to indicate that it ends
     at a boundary with the filesystem structure (e.g. at the end of a Ceph
     object).  It tells netfslib not to retile subrequests across it.
     (文件系统可以在子请求上设置此项，以指示它在文件系统结构的边界处结束(例如在 Ceph 对象的末尾)。它告诉 netfslib 不要跨越它重新平铺子请求。)

 * ``error``

   This is for the filesystem to store result of the subrequest.  It should be
   set to 0 if successful and a negative error code otherwise.
   (这是供文件系统存储子请求结果之用。成功时应设置为 0，否则设置为负错误码。)

 * ``debug_index``
 * ``stream_nr``

   A number allocated to this slice that can be displayed in trace lines for
   reference and the number of the request stream that it belongs to.
   (分配给此切片的一个编号，可以显示在跟踪行中以供参考，以及它所属的请求流的编号。)

If necessary, the filesystem can get and put extra refs on the subrequest it is
given (如有必要，文件系统可以对其被给予的子请求获取和放置额外的引用)::

	void netfs_get_subrequest(struct netfs_io_subrequest *subreq,
				  enum netfs_sreq_ref_trace what);
	void netfs_put_subrequest(struct netfs_io_subrequest *subreq,
				  enum netfs_sreq_ref_trace what);

using netfs trace codes to indicate the reason.  Care must be taken, however,
as once control of the subrequest is returned to netfslib, the same subrequest
can be reissued/retried.
(使用 netfs 跟踪代码来指示原因。但必须小心，因为一旦子请求的控制权返回给 netfslib，同一个子请求可能会被重新下发/重试。)

Filesystem Methods
(文件系统方法)
------------------

The filesystem sets a table of operations in ``netfs_inode`` for netfslib to
use (文件系统在 ``netfs_inode`` 中设置一个操作表供 netfslib 使用)::

	struct netfs_request_ops {
		mempool_t *request_pool;
		mempool_t *subrequest_pool;
		int (*init_request)(struct netfs_io_request *rreq, struct file *file);
		void (*free_request)(struct netfs_io_request *rreq);
		void (*free_subrequest)(struct netfs_io_subrequest *rreq);
		void (*expand_readahead)(struct netfs_io_request *rreq);
		int (*prepare_read)(struct netfs_io_subrequest *subreq);
		void (*issue_read)(struct netfs_io_subrequest *subreq);
		void (*done)(struct netfs_io_request *rreq);
		void (*update_i_size)(struct inode *inode, loff_t i_size);
		void (*post_modify)(struct inode *inode);
		void (*begin_writeback)(struct netfs_io_request *wreq);
		void (*prepare_write)(struct netfs_io_subrequest *subreq);
		void (*issue_write)(struct netfs_io_subrequest *subreq);
		void (*retry_request)(struct netfs_io_request *wreq,
				      struct netfs_io_stream *stream);
		void (*invalidate_cache)(struct netfs_io_request *wreq);
	};

The table starts with a pair of optional pointers to memory pools from which
requests and subrequests can be allocated.  If these are not given, netfslib
has default pools that it will use instead.  If the filesystem wraps the netfs
structs in its own larger structs, then it will need to use its own pools.
Netfslib will allocate directly from the pools.
(该表以一对可选的内存池指针开始，请求和子请求可以从中分配。如果没有给出这些指针，netfslib 有它将使用的默认池。如果文件系统把 netfs 结构包装在它自己的更大结构中，则需要使用它自己的池。netfslib 将直接从这些池中分配。)

The methods defined in the table are:
(表中定义的方法如下：)

 * ``init_request()``
 * ``free_request()``
 * ``free_subrequest()``

   [Optional] A filesystem may implement these to initialise or clean up any
   resources that it attaches to the request or subrequest.
   ([可选] 文件系统可以实现这些方法，来初始化或清理它附加到请求或子请求上的任何资源。)

 * ``expand_readahead()``

   [Optional] This is called to allow the filesystem to expand the size of a
   readahead request.  The filesystem gets to expand the request in both
   directions, though it must retain the initial region as that may represent
   an allocation already made.  If local caching is enabled, it gets to expand
   the request first.
   ([可选] 调用此方法以允许文件系统扩展预读(readahead)请求的大小。文件系统可以在两个方向上扩展请求，但它必须保留初始区域，因为该区域可能代表一个已经完成的分配。如果启用了本地缓存，则由缓存先扩展请求。)

   Expansion is communicated by changing ->start and ->len in the request
   structure.  Note that if any change is made, ->len must be increased by at
   least as much as ->start is reduced.
   (扩展通过更改请求结构中的 ->start 和 ->len 来传达。注意，如果进行了任何更改，->len 的增加量必须至少与 ->start 的减少量相当。)

 * ``prepare_read()``

   [Optional] This is called to allow the filesystem to limit the size of a
   subrequest.  It may also limit the number of individual regions in iterator,
   such as required by RDMA.  This information should be set on stream zero in ([可选] 调用此方法以允许文件系统限制子请求的大小。它还可以限制迭代器中单独区域的数量，例如 RDMA 所要求的。此信息应设置在流 0 上的以下位置)::

	rreq->io_streams[0].sreq_max_len
	rreq->io_streams[0].sreq_max_segs

   The filesystem can use this, for example, to chop up a request that has to
   be split across multiple servers or to put multiple reads in flight.
   (文件系统可以使用它，例如，将必须跨多个服务器拆分的请求切分开，或者让多个读取同时在途。)

   Zero should be returned on success and an error code otherwise.
   (成功时应返回 0，否则返回错误码。)

 * ``issue_read()``

   [Required] Netfslib calls this to dispatch a subrequest to the server for
   reading.  In the subrequest, ->start, ->len and ->transferred indicate what
   data should be read from the server and ->io_iter indicates the buffer to be
   used.
   ([必需] netfslib 调用此方法将一个子请求下发到服务器进行读取。在子请求中，->start、->len 和 ->transferred 指示应从服务器读取哪些数据，->io_iter 指示要使用的缓冲区。)

   There is no return value; the ``netfs_read_subreq_terminated()`` function
   should be called to indicate that the subrequest completed either way.
   ->error, ->transferred and ->flags should be updated before completing.  The
   termination can be done asynchronously.
   (没有返回值；应调用 ``netfs_read_subreq_terminated()`` 函数来指示子请求无论如何都已完成。->error、->transferred 和 ->flags 应在完成之前更新。终止可以异步进行。)

   Note: the filesystem must not deal with setting folios uptodate, unlocking
   them or dropping their refs - the library deals with this as it may have to
   stitch together the results of multiple subrequests that variously overlap
   the set of folios.
   (注意：文件系统不必处理把 folio 设置为最新(up-to-date)、解锁它们或放置它们的引用——该库会处理这些，因为它可能必须拼接多个子请求的结果，这些子请求以各种方式与 folio 集合重叠。)

 * ``done()``

   [Optional] This is called after the folios in a read request have all been
   unlocked (and marked uptodate if applicable).
   ([可选] 在读请求中的所有 folio 都已解锁(并且如果适用，已标记为最新)之后调用此方法。)

 * ``update_i_size()``

   [Optional] This is invoked by netfslib at various points during the write
   paths to ask the filesystem to update its idea of the file size.  If not
   given, netfslib will set i_size and i_blocks and update the local cache
   cookie.
   ([可选] netfslib 在写路径的各个点调用此方法，要求文件系统更新其对文件大小的认识。如果未提供，netfslib 将设置 i_size 和 i_blocks 并更新本地缓存 cookie。)
   
 * ``post_modify()``

   [Optional] This is called after netfslib writes to the pagecache or when it
   allows an mmap'd page to be marked as writable.
   ([可选] 在 netfslib 写入页缓存之后，或当它允许一个 mmap 的页被标记为可写时，调用此方法。)
   
 * ``begin_writeback()``

   [Optional] Netfslib calls this when processing a writeback request if it
   finds a dirty page that isn't simply marked NETFS_FOLIO_COPY_TO_CACHE,
   indicating it must be written to the server.  This allows the filesystem to
   only set up writeback resources when it knows it's going to have to perform
   a write.
   ([可选] netfslib 在处理一个回写请求时，如果发现一个并非仅标记为 NETFS_FOLIO_COPY_TO_CACHE 的脏页(即它必须被写入服务器)，则调用此方法。这允许文件系统仅在知道它将不得不执行写入时，才设置回写资源。)
   
 * ``prepare_write()``

   [Optional] This is called to allow the filesystem to limit the size of a
   subrequest.  It may also limit the number of individual regions in iterator,
   such as required by RDMA.  This information should be set on stream to which
   the subrequest belongs ([可选] 调用此方法以允许文件系统限制子请求的大小。它还可以限制迭代器中单独区域的数量，例如 RDMA 所要求的。此信息应设置在该子请求所属的流上)::

	rreq->io_streams[subreq->stream_nr].sreq_max_len
	rreq->io_streams[subreq->stream_nr].sreq_max_segs

   The filesystem can use this, for example, to chop up a request that has to
   be split across multiple servers or to put multiple writes in flight.
   (文件系统可以使用它，例如，将必须跨多个服务器拆分的请求切分开，或者让多个写入同时在途。)

   This is not permitted to return an error.  Instead, in the event of failure,
   ``netfs_prepare_write_failed()`` must be called.
   (不允许它返回错误。相反，在失败的情况下，必须调用 ``netfs_prepare_write_failed()``。)

 * ``issue_write()``

   [Required] This is used to dispatch a subrequest to the server for writing.
   In the subrequest, ->start, ->len and ->transferred indicate what data
   should be written to the server and ->io_iter indicates the buffer to be
   used.
   ([必需] 此方法用于将一个子请求下发到服务器进行写入。在子请求中，->start、->len 和 ->transferred 指示应将哪些数据写入服务器，->io_iter 指示要使用的缓冲区。)

   There is no return value; the ``netfs_write_subreq_terminated()`` function
   should be called to indicate that the subrequest completed either way.
   ->error, ->transferred and ->flags should be updated before completing.  The
   termination can be done asynchronously.
   (没有返回值；应调用 ``netfs_write_subreq_terminated()`` 函数来指示子请求无论如何都已完成。->error、->transferred 和 ->flags 应在完成之前更新。终止可以异步进行。)

   Note: the filesystem must not deal with removing the dirty or writeback
   marks on folios involved in the operation and should not take refs or pins
   on them, but should leave retention to netfslib.
   (注意：文件系统不必处理移除操作所涉及的 folio 上的脏标记或回写标记，并且不应获取对它们的引用或固定，而应把保留工作留给 netfslib。)

 * ``retry_request()``

   [Optional] Netfslib calls this at the beginning of a retry cycle.  This
   allows the filesystem to examine the state of the request, the subrequests
   in the indicated stream and of its own data and make adjustments or
   renegotiate resources.
   ([可选] netfslib 在重试周期的开始调用此方法。这允许文件系统检查请求的状态、指示流中的子请求的状态以及它自身数据的状态，并进行调整或重新协商资源。)
   
 * ``invalidate_cache()``

   [Optional] This is called by netfslib to invalidate data stored in the local
   cache in the event that writing to the local cache fails, providing updated
   coherency data that netfs can't provide.
   ([可选] 在写入本地缓存失败的情况下，netfslib 调用此方法来使存储在本地缓存中的数据失效，提供 netfs 无法提供的更新后的一致性数据。)

Terminating a subrequest
(终止子请求)
------------------------

When a subrequest completes, there are a number of functions that the cache or
subrequest can call to inform netfslib of the status change.  One function is
provided to terminate a write subrequest at the preparation stage and acts
synchronously:
(当子请求完成时，缓存或子请求可以调用一些函数将状态变化通知 netfslib。提供了一个函数在准备阶段终止写子请求，并且它是同步起作用的：)

 * ``void netfs_prepare_write_failed(struct netfs_io_subrequest *subreq);``

   Indicate that the ->prepare_write() call failed.  The ``error`` field should
   have been updated.
   (指示 ->prepare_write() 调用失败。``error`` 域应该已被更新。)

Note that ->prepare_read() can return an error as a read can simply be aborted.
Dealing with writeback failure is trickier.
(注意，->prepare_read() 可以返回错误，因为读可以简单地中止。处理回写失败则更为棘手。)

The other functions are used for subrequests that got as far as being issued:
(其他函数用于已经下发出去的子请求：)

 * ``void netfs_read_subreq_terminated(struct netfs_io_subrequest *subreq);``

   Tell netfslib that a read subrequest has terminated.  The ``error``,
   ``flags`` and ``transferred`` fields should have been updated.
   (告诉 netfslib 一个读子请求已终止。``error``、``flags`` 和 ``transferred`` 域应该已被更新。)

 * ``void netfs_write_subrequest_terminated(void *_op, ssize_t transferred_or_error);``

   Tell netfslib that a write subrequest has terminated.  Either the amount of
   data processed or the negative error code can be passed in.  This is
   can be used as a kiocb completion function.
   (告诉 netfslib 一个写子请求已终止。可以传入已处理的数据量或负错误码。这可以用作 kiocb 完成函数。)

 * ``void netfs_read_subreq_progress(struct netfs_io_subrequest *subreq);``

   This is provided to optionally update netfslib on the incremental progress
   of a read, allowing some folios to be unlocked early and does not actually
   terminate the subrequest.  The ``transferred`` field should have been
   updated.
   (提供此函数是为了可选择地向 netfslib 更新一次读取的增量进度，允许一些 folio 提前解锁，并且它实际上并不终止子请求。``transferred`` 域应该已被更新。)

Local Cache API
(本地缓存 API)
---------------

Netfslib provides a separate API for a local cache to implement, though it
provides some somewhat similar routines to the filesystem request API.
(netfslib 为本地缓存提供了一个单独的 API 来实现，不过它提供了一些与文件系统请求 API 略微相似的例程。)

Firstly, the netfs_io_request object contains a place for the cache to hang its
state (首先，netfs_io_request 对象包含一个供缓存存放其状态的位置)::

	struct netfs_cache_resources {
		const struct netfs_cache_ops	*ops;
		void				*cache_priv;
		void				*cache_priv2;
		unsigned int			debug_id;
		unsigned int			inval_counter;
	};

This contains an operations table pointer and two private pointers plus the
debug ID of the fscache cookie for tracing purposes and an invalidation counter
that is cranked by calls to ``fscache_invalidate()`` allowing cache subrequests
to be invalidated after completion.
(它包含一个操作表指针和两个私有指针，外加用于跟踪目的的 fscache cookie 调试 ID，以及一个失效计数器——该计数器由对 ``fscache_invalidate()`` 的调用而递增，允许缓存在完成之后使缓存子请求失效。)

The cache operation table looks like the following (缓存操作表如下所示)::

	struct netfs_cache_ops {
		void (*end_operation)(struct netfs_cache_resources *cres);
		void (*expand_readahead)(struct netfs_cache_resources *cres,
					 loff_t *_start, size_t *_len, loff_t i_size);
		enum netfs_io_source (*prepare_read)(struct netfs_io_subrequest *subreq,
						     loff_t i_size);
		int (*read)(struct netfs_cache_resources *cres,
			    loff_t start_pos,
			    struct iov_iter *iter,
			    bool seek_data,
			    netfs_io_terminated_t term_func,
			    void *term_func_priv);
		void (*prepare_write_subreq)(struct netfs_io_subrequest *subreq);
		void (*issue_write)(struct netfs_io_subrequest *subreq);
	};

With a termination handler function pointer (还有一个终止处理函数指针)::

	typedef void (*netfs_io_terminated_t)(void *priv,
					      ssize_t transferred_or_error,
					      bool was_async);

The methods defined in the table are:
(表中定义的方法如下：)

 * ``end_operation()``

   [Required] Called to clean up the resources at the end of the read request.
   ([必需] 调用此方法以在读请求结束时清理资源。)

 * ``expand_readahead()``

   [Optional] Called at the beginning of a readahead operation to allow the
   cache to expand a request in either direction.  This allows the cache to
   size the request appropriately for the cache granularity.
   ([可选] 在预读(readahead)操作开始时调用，以允许缓存在任一方向上扩展请求。这允许缓存根据缓存粒度适当地确定请求的大小。)

 * ``prepare_read()``

   [Required] Called to configure the next slice of a request.  ->start and
   ->len in the subrequest indicate where and how big the next slice can be;
   the cache gets to reduce the length to match its granularity requirements.
   ([必需] 调用此方法以配置请求的下一个切片。子请求中的 ->start 和 ->len 指示下一个切片的位置和大小；缓存可以把长度缩减以匹配其粒度要求。)

   The function is passed pointers to the start and length in its parameters,
   plus the size of the file for reference, and adjusts the start and length
   appropriately.  It should return one of:
   (该函数在其参数中被传入指向起始位置和长度的指针，外加文件大小以供参考，并适当地调整起始位置和长度。它应返回以下之一：)

   * ``NETFS_FILL_WITH_ZEROES``
   * ``NETFS_DOWNLOAD_FROM_SERVER``
   * ``NETFS_READ_FROM_CACHE``
   * ``NETFS_INVALID_READ``

   to indicate whether the slice should just be cleared or whether it should be
   downloaded from the server or read from the cache - or whether slicing
   should be given up at the current point.
   (以指示该切片是应当被直接清除，还是应当从服务器下载或从缓存读取——或者是否应当在当前点放弃切片。)

 * ``read()``

   [Required] Called to read from the cache.  The start file offset is given
   along with an iterator to read to, which gives the length also.  It can be
   given a hint requesting that it seek forward from that start position for
   data.
   ([必需] 调用此方法以从缓存读取。给定了起始文件偏移量以及要读取到的迭代器，后者也给出了长度。可以给它一个提示，请求它从该起始位置向后寻找数据。)

   Also provided is a pointer to a termination handler function and private
   data to pass to that function.  The termination function should be called
   with the number of bytes transferred or an error code, plus a flag
   indicating whether the termination is definitely happening in the caller's
   context.
   (还提供了一个指向终止处理函数的指针以及要传递给该函数的私有数据。终止函数应以已传输的字节数或错误码被调用，外加一个指示终止是否确实发生在调用者上下文中的标志。)

 * ``prepare_write_subreq()``

   [Required] This is called to allow the cache to limit the size of a
   subrequest.  It may also limit the number of individual regions in iterator,
   such as required by DIO/DMA.  This information should be set on stream to
   which the subrequest belongs ([必需] 调用此方法以允许缓存限制子请求的大小。它还可以限制迭代器中单独区域的数量，例如 DIO/DMA 所要求的。此信息应设置在该子请求所属的流上)::

	rreq->io_streams[subreq->stream_nr].sreq_max_len
	rreq->io_streams[subreq->stream_nr].sreq_max_segs

   The filesystem can use this, for example, to chop up a request that has to
   be split across multiple servers or to put multiple writes in flight.
   (文件系统可以使用它，例如，将必须跨多个服务器拆分的请求切分开，或者让多个写入同时在途。)

   This is not permitted to return an error.  In the event of failure,
   ``netfs_prepare_write_failed()`` must be called.
   (不允许它返回错误。在失败的情况下，必须调用 ``netfs_prepare_write_failed()``。)

 * ``issue_write()``

   [Required] This is used to dispatch a subrequest to the cache for writing.
   In the subrequest, ->start, ->len and ->transferred indicate what data
   should be written to the cache and ->io_iter indicates the buffer to be
   used.
   ([必需] 此方法用于将一个子请求下发到缓存进行写入。在子请求中，->start、->len 和 ->transferred 指示应将哪些数据写入缓存，->io_iter 指示要使用的缓冲区。)

   There is no return value; the ``netfs_write_subreq_terminated()`` function
   should be called to indicate that the subrequest completed either way.
   ->error, ->transferred and ->flags should be updated before completing.  The
   termination can be done asynchronously.
   (没有返回值；应调用 ``netfs_write_subreq_terminated()`` 函数来指示子请求无论如何都已完成。->error、->transferred 和 ->flags 应在完成之前更新。终止可以异步进行。)


API Function Reference
(API 函数参考)
======================

.. kernel-doc:: include/linux/netfs.h
.. kernel-doc:: fs/netfs/buffered_read.c
