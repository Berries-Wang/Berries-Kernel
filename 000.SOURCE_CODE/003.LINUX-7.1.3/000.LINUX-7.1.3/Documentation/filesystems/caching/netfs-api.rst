.. SPDX-License-Identifier: GPL-2.0

==============================
Network Filesystem Caching API
==============================

(网络文件系统缓存 API)

Fscache provides an API by which a network filesystem can make use of local
caching facilities.  The API is arranged around a number of principles:
(Fscache 提供了一个 API，使网络文件系统能够使用本地缓存设施。该 API 围绕若干原则而构建：)

 (1) A cache is logically organised into volumes and data storage objects
     within those volumes.
     (缓存按逻辑组织成卷，以及各卷内的数据存储对象。)

 (2) Volumes and data storage objects are represented by various types of
     cookie.
     (卷和数据存储对象由各种类型的 cookie 表示。)

 (3) Cookies have keys that distinguish them from their peers.
     (cookie 拥有将其与同类区分开来的键。)

 (4) Cookies have coherency data that allows a cache to determine if the
     cached data is still valid.
     (cookie 拥有一致性数据，使缓存能够判断缓存的数据是否仍然有效。)

 (5) I/O is done asynchronously where possible.
     (I/O 在可能的情况下异步完成。)

This API is used by::

	#include <linux/fscache.h>.

(使用此 API 的方式如下：)

.. This document contains the following sections:

	 (1) Overview
	 (2) Volume registration
	 (3) Data file registration
	 (4) Declaring a cookie to be in use
	 (5) Resizing a data file (truncation)
	 (6) Data I/O API
	 (7) Data file coherency
	 (8) Data file invalidation
	 (9) Write back resource management
	(10) Caching of local modifications
	(11) Page release and invalidation


Overview
(概述)
========

The fscache hierarchy is organised on two levels from a network filesystem's
point of view.  The upper level represents "volumes" and the lower level
represents "data storage objects".  These are represented by two types of
cookie, hereafter referred to as "volume cookies" and "cookies".
(从网络文件系统的视角看，fscache 层级按两个级别组织。上级表示"卷"，下级表示"数据存储对象"。它们由两种类型的 cookie 表示，以下简称"volume cookie"(卷 cookie)和"cookie"。)

A network filesystem acquires a volume cookie for a volume using a volume key,
which represents all the information that defines that volume (e.g. cell name
or server address, volume ID or share name).  This must be rendered as a
printable string that can be used as a directory name (ie. no '/' characters
and shouldn't begin with a '.').  The maximum name length is one less than the
maximum size of a filename component (allowing the cache backend one char for
its own purposes).
(网络文件系统使用卷键(volume key)为一个卷获取卷 cookie，卷键代表了定义该卷的全部信息(例如 cell 名称或服务器地址、卷 ID 或共享名)。它必须呈现为一个可用作目录名的可打印字符串(即不含 '/' 字符，且不应以 '.' 开头)。名称的最大长度比文件名组件的最大长度小一(为缓存后端自身的用途保留一个字符)。)

A filesystem would typically have a volume cookie for each superblock.
(一个文件系统通常会为每个超级块拥有一个卷 cookie。)

The filesystem then acquires a cookie for each file within that volume using an
object key.  Object keys are binary blobs and only need to be unique within
their parent volume.  The cache backend is responsible for rendering the binary
blob into something it can use and may employ hash tables, trees or whatever to
improve its ability to find an object.  This is transparent to the network
filesystem.
(然后，文件系统使用对象键(object key)为该卷内的每个文件获取一个 cookie。对象键是二进制数据块，只需在其父卷内唯一即可。由缓存后端负责将该二进制数据块渲染成它可以使用的东西，并可以采用哈希表、树或其他任何手段来提高其查找对象的能力。这对网络文件系统是透明的。)

A filesystem would typically have a cookie for each inode, and would acquire it
in iget and relinquish it when evicting the cookie.
(文件系统通常会为每个 inode 拥有一个 cookie，并在 iget 时获取它，在驱逐该 cookie 时将其放弃。)

Once it has a cookie, the filesystem needs to mark the cookie as being in use.
This causes fscache to send the cache backend off to look up/create resources
for the cookie in the background, to check its coherency and, if necessary, to
mark the object as being under modification.
(一旦拥有了 cookie，文件系统就需要将该 cookie 标记为使用中。这会使 fscache 在后台派缓存后端去查找/创建该 cookie 的资源，检查其一致性，并在必要时将该对象标记为正在修改中。)

A filesystem would typically "use" the cookie in its file open routine and
unuse it in file release and it needs to use the cookie around calls to
truncate the cookie locally.  It *also* needs to use the cookie when the
pagecache becomes dirty and unuse it when writeback is complete.  This is
slightly tricky, and provision is made for it.
(文件系统通常会在其文件打开例程中"use"该 cookie，并在文件释放中"unuse"它；此外，在本地截断 cookie 的调用前后也需要使用该 cookie。当页缓存变脏时，它*还*需要使用该 cookie，并在回写完成时 unuse 它。这稍微有些棘手，为此提供了相应的支持。)

When performing a read, write or resize on a cookie, the filesystem must first
begin an operation.  This copies the resources into a holding struct and puts
extra pins into the cache to stop cache withdrawal from tearing down the
structures being used.  The actual operation can then be issued and conflicting
invalidations can be detected upon completion.
(在对 cookie 执行读、写或调整大小操作之前，文件系统必须先开始一个操作。这会将资源复制到一个持有结构中，并在缓存中放置额外的钉住引用，以防止缓存退出时拆毁正在使用的结构。然后就可以发出实际操作，并在完成时检测到相互冲突的失效。)

The filesystem is expected to use netfslib to access the cache, but that's not
actually required and it can use the fscache I/O API directly.
(文件系统应使用 netfslib 来访问缓存，但这并非硬性要求，它也可以直接使用 fscache I/O API。)


Volume Registration
(卷注册)
===================

The first step for a network filesystem is to acquire a volume cookie for the
volume it wants to access::

	struct fscache_volume *
	fscache_acquire_volume(const char *volume_key,
			       const char *cache_name,
			       const void *coherency_data,
			       size_t coherency_len);

(网络文件系统的第一步是为其想要访问的卷获取一个卷 cookie：)

This function creates a volume cookie with the specified volume key as its name
and notes the coherency data.
(该函数以指定的卷键作为名称创建一个卷 cookie，并记录一致性数据。)

The volume key must be a printable string with no '/' characters in it.  It
should begin with the name of the filesystem and should be no longer than 254
characters.  It should uniquely represent the volume and will be matched with
what's stored in the cache.
(卷键必须是一个不含 '/' 字符的可打印字符串。它应以文件系统的名称开头，且长度不得超过 254 个字符。它应当唯一地代表该卷，并将与缓存中存储的内容进行匹配。)

The caller may also specify the name of the cache to use.  If specified,
fscache will look up or create a cache cookie of that name and will use a cache
of that name if it is online or comes online.  If no cache name is specified,
it will use the first cache that comes to hand and set the name to that.
(调用者还可以指定要使用的缓存名称。如果指定了，fscache 将查找或创建该名称的缓存 cookie，并在该名称的缓存在线或上线时使用它。如果未指定缓存名称，它将使用第一个可用的缓存，并将名称设置为该缓存的名称。)

The specified coherency data is stored in the cookie and will be matched
against coherency data stored on disk.  The data pointer may be NULL if no data
is provided.  If the coherency data doesn't match, the entire cache volume will
be invalidated.
(指定的一致性数据存储在 cookie 中，并将与磁盘上存储的一致性数据进行匹配。如果未提供数据，数据指针可以为 NULL。如果一致性数据不匹配，整个缓存卷将被失效。)

This function can return errors such as EBUSY if the volume key is already in
use by an acquired volume or ENOMEM if an allocation failure occurred.  It may
also return a NULL volume cookie if fscache is not enabled.  It is safe to
pass a NULL cookie to any function that takes a volume cookie.  This will
cause that function to do nothing.
(如果卷键已被已获取的卷使用，该函数可能返回诸如 EBUSY 的错误；如果发生分配失败，则返回 ENOMEM。如果 fscache 未启用，它也可能返回 NULL 卷 cookie。将 NULL cookie 传递给任何接受卷 cookie 的函数都是安全的，这会使该函数什么都不做。)


When the network filesystem has finished with a volume, it should relinquish it
by calling::

	void fscache_relinquish_volume(struct fscache_volume *volume,
				       const void *coherency_data,
				       bool invalidate);

(当网络文件系统用完一个卷后，应通过以下调用放弃它：)

This will cause the volume to be committed or removed, and if sealed the
coherency data will be set to the value supplied.  The amount of coherency data
must match the length specified when the volume was acquired.  Note that all
data cookies obtained in this volume must be relinquished before the volume is
relinquished.
(这将导致该卷被提交或移除，并且如果该卷已封存，一致性数据将被设置为所提供的值。一致性数据的数量必须与获取该卷时指定的长度相匹配。注意，在此卷中获取的所有数据 cookie 必须在放弃该卷之前全部放弃。)


Data File Registration
(数据文件注册)
======================

Once it has a volume cookie, a network filesystem can use it to acquire a
cookie for data storage::

	struct fscache_cookie *
	fscache_acquire_cookie(struct fscache_volume *volume,
			       u8 advice,
			       const void *index_key,
			       size_t index_key_len,
			       const void *aux_data,
			       size_t aux_data_len,
			       loff_t object_size)

(拥有了卷 cookie 之后，网络文件系统可以使用它为数据存储获取一个 cookie：)

This creates the cookie in the volume using the specified index key.  The index
key is a binary blob of the given length and must be unique for the volume.
This is saved into the cookie.  There are no restrictions on the content, but
its length shouldn't exceed about three quarters of the maximum filename length
to allow for encoding.
(这会使用指定的索引键在卷中创建 cookie。索引键是给定长度的二进制数据块，且必须在该卷内唯一。它会被保存到 cookie 中。对其内容没有限制，但其长度不应超过最大文件名长度的约四分之三，以便为编码留出空间。)

The caller should also pass in a piece of coherency data in aux_data.  A buffer
of size aux_data_len will be allocated and the coherency data copied in.  It is
assumed that the size is invariant over time.  The coherency data is used to
check the validity of data in the cache.  Functions are provided by which the
coherency data can be updated.
(调用者还应通过 aux_data 传入一段一致性数据。将分配一个大小为 aux_data_len 的缓冲，并将一致性数据复制进去。假定该大小不随时间变化。一致性数据用于检查缓存中数据的有效性。提供了可用于更新一致性数据的函数。)

The file size of the object being cached should also be provided.  This may be
used to trim the data and will be stored with the coherency data.
(还应提供被缓存对象的文件大小。它可用于裁剪数据，并将与一致性数据一起存储。)

This function never returns an error, though it may return a NULL cookie on
allocation failure or if fscache is not enabled.  It is safe to pass in a NULL
volume cookie and pass the NULL cookie returned to any function that takes it.
This will cause that function to do nothing.
(此函数从不返回错误，但在分配失败或 fscache 未启用时可能返回 NULL cookie。传入 NULL 卷 cookie 是安全的，将返回的 NULL cookie 传递给任何接受它的函数也是安全的。这会使该函数什么都不做。)


When the network filesystem has finished with a cookie, it should relinquish it
by calling::

	void fscache_relinquish_cookie(struct fscache_cookie *cookie,
				       bool retire);

(当网络文件系统用完一个 cookie 后，应通过以下调用放弃它：)

This will cause fscache to either commit the storage backing the cookie or
delete it.
(这将使 fscache 要么提交支撑该 cookie 的存储，要么将其删除。)


Marking A Cookie In-Use
(将 cookie 标记为使用中)
=======================

Once a cookie has been acquired by a network filesystem, the filesystem should
tell fscache when it intends to use the cookie (typically done on file open)
and should say when it has finished with it (typically on file close)::

	void fscache_use_cookie(struct fscache_cookie *cookie,
				bool will_modify);
	void fscache_unuse_cookie(struct fscache_cookie *cookie,
				  const void *aux_data,
				  const loff_t *object_size);

(网络文件系统获取 cookie 之后，应在打算使用该 cookie 时(通常在文件打开时)告知 fscache，并在用完它时(通常在文件关闭时)予以说明：)

The *use* function tells fscache that it will use the cookie and, additionally,
indicate if the user is intending to modify the contents locally.  If not yet
done, this will trigger the cache backend to go and gather the resources it
needs to access/store data in the cache.  This is done in the background, and
so may not be complete by the time the function returns.
(*use* 函数告知 fscache 它将使用该 cookie，并额外说明使用者是否打算在本地修改内容。如果尚未完成，这将触发缓存后端去收集在缓存中访问/存储数据所需的资源。这是在后台进行的，因此在该函数返回时可能尚未完成。)

The *unuse* function indicates that a filesystem has finished using a cookie.
It optionally updates the stored coherency data and object size and then
decreases the in-use counter.  When the last user unuses the cookie, it is
scheduled for garbage collection.  If not reused within a short time, the
resources will be released to reduce system resource consumption.
(*unuse* 函数表明文件系统已用完一个 cookie。它可选择性地更新所存储的一致性数据和对象大小，然后递减使用中计数。当最后一个使用者 unuse 该 cookie 时，它会被安排进行垃圾回收。如果短时间内未被重新使用，资源将被释放以减少系统资源消耗。)

A cookie must be marked in-use before it can be accessed for read, write or
resize - and an in-use mark must be kept whilst there is dirty data in the
pagecache in order to avoid an oops due to trying to open a file during process
exit.
(cookie 必须先标记为使用中，然后才能对其进行读、写或调整大小——并且在页缓存中存在脏数据期间必须保持使用中标记，以避免在进程退出期间尝试打开文件而导致 oops。)

Note that in-use marks are cumulative.  For each time a cookie is marked
in-use, it must be unused.
(注意，使用中标记是累积的。cookie 每被标记一次使用中，就必须相应地被 unuse 一次。)


Resizing A Data File (Truncation)
(调整数据文件大小(截断))
=================================

If a network filesystem file is resized locally by truncation, the following
should be called to notify the cache::

	void fscache_resize_cookie(struct fscache_cookie *cookie,
				   loff_t new_size);

(如果网络文件系统文件在本地通过截断调整了大小，应调用以下函数来通知缓存：)

The caller must have first marked the cookie in-use.  The cookie and the new
size are passed in and the cache is synchronously resized.  This is expected to
be called from ``->setattr()`` inode operation under the inode lock.
(调用者必须已先将 cookie 标记为使用中。传入 cookie 和新的大小，缓存将被同步调整大小。该函数应在持有 inode 锁的情况下从 ``->setattr()`` inode 操作中调用。)


Data I/O API
(数据 I/O API)
============

To do data I/O operations directly through a cookie, the following functions
are available::

	int fscache_begin_read_operation(struct netfs_cache_resources *cres,
					 struct fscache_cookie *cookie);
	int fscache_read(struct netfs_cache_resources *cres,
			 loff_t start_pos,
			 struct iov_iter *iter,
			 enum netfs_read_from_hole read_hole,
			 netfs_io_terminated_t term_func,
			 void *term_func_priv);
	int fscache_write(struct netfs_cache_resources *cres,
			  loff_t start_pos,
			  struct iov_iter *iter,
			  netfs_io_terminated_t term_func,
			  void *term_func_priv);

(要通过 cookie 直接执行数据 I/O 操作，可以使用以下函数：)

The *begin* function sets up an operation, attaching the resources required to
the cache resources block from the cookie.  Assuming it doesn't return an error
(for instance, it will return -ENOBUFS if given a NULL cookie, but otherwise do
nothing), then one of the other two functions can be issued.
(*begin* 函数设置一个操作，把所需的资源从 cookie 附加到缓存资源块上。假设它没有返回错误(例如，如果传入 NULL cookie，它会返回 -ENOBUFS，但除此之外什么都不做)，那么就可以发出另外两个函数之一。)

The *read* and *write* functions initiate a direct-IO operation.  Both take the
previously set up cache resources block, an indication of the start file
position, and an I/O iterator that describes buffer and indicates the amount of
data.
(*read* 和 *write* 函数发起一个直接 I/O 操作。两者都接受先前设置的缓存资源块、起始文件位置的指示，以及一个描述缓冲并指示数据量的 I/O 迭代器。)

The read function also takes a parameter to indicate how it should handle a
partially populated region (a hole) in the disk content.  This may be to ignore
it, skip over an initial hole and place zeros in the buffer or give an error.
(read 函数还接受一个参数，用于指示它应如何处理磁盘内容中部分填充的区域(洞)。可以是忽略它、跳过起始的洞并在缓冲中填零，或者报错。)

The read and write functions can be given an optional termination function that
will be run on completion::

	typedef
	void (*netfs_io_terminated_t)(void *priv, ssize_t transferred_or_error,
				      bool was_async);

(可以为 read 和 write 函数提供一个可选的终止函数，它将在完成时运行：)

If a termination function is given, the operation will be run asynchronously
and the termination function will be called upon completion.  If not given, the
operation will be run synchronously.  Note that in the asynchronous case, it is
possible for the operation to complete before the function returns.
(如果给了终止函数，操作将异步运行，并且终止函数将在完成时被调用。如果没有给出，操作将同步运行。注意，在异步情况下，操作有可能在函数返回之前就已完成。)

Both the read and write functions end the operation when they complete,
detaching any pinned resources.
(read 和 write 函数在完成时会结束该操作，分离任何被钉住的资源。)

The read operation will fail with ESTALE if invalidation occurred whilst the
operation was ongoing.
(如果在操作进行期间发生了失效，read 操作将以 ESTALE 失败。)


Data File Coherency
(数据文件一致性)
===================

To request an update of the coherency data and file size on a cookie, the
following should be called::

	void fscache_update_cookie(struct fscache_cookie *cookie,
				   const void *aux_data,
				   const loff_t *object_size);

(要请求更新 cookie 上的一致性数据和文件大小，应调用以下函数：)

This will update the cookie's coherency data and/or file size.
(这将更新 cookie 的一致性数据和/或文件大小。)


Data File Invalidation
(数据文件失效)
======================

Sometimes it will be necessary to invalidate an object that contains data.
Typically this will be necessary when the server informs the network filesystem
of a remote third-party change - at which point the filesystem has to throw
away the state and cached data that it had for an file and reload from the
server.
(有时有必要使一个包含数据的对象失效。通常在服务器通知网络文件系统发生了远程第三方更改时需要这样做——此时文件系统必须丢弃它为一个文件所保有的状态和缓存数据，并从服务器重新加载。)

To indicate that a cache object should be invalidated, the following should be
called::

	void fscache_invalidate(struct fscache_cookie *cookie,
				const void *aux_data,
				loff_t size,
				unsigned int flags);

(要指示应使一个缓存对象失效，应调用以下函数：)

This increases the invalidation counter in the cookie to cause outstanding
reads to fail with -ESTALE, sets the coherency data and file size from the
information supplied, blocks new I/O on the cookie and dispatches the cache to
go and get rid of the old data.
(这会增加 cookie 中的失效计数器，使未完成的读操作以 -ESTALE 失败；根据所提供的信息设置一致性数据和文件大小；阻止对 cookie 的新 I/O；并派发缓存去清除旧数据。)

Invalidation runs asynchronously in a worker thread so that it doesn't block
too much.
(失效在工作者线程中异步运行，因此不会造成太多阻塞。)


Write-Back Resource Management
(回写资源管理)
==============================

To write data to the cache from network filesystem writeback, the cache
resources required need to be pinned at the point the modification is made (for
instance when the page is marked dirty) as it's not possible to open a file in
a thread that's exiting.
(要将数据从网络文件系统回写写入缓存，需要在做出修改时(例如页被标记为脏时)就钉住所需的缓存资源，因为无法在正在退出的线程中打开文件。)

The following facilities are provided to manage this:
(为此提供了以下设施：)

 * An inode flag, ``I_PINNING_FSCACHE_WB``, is provided to indicate that an
   in-use is held on the cookie for this inode.  It can only be changed if the
   the inode lock is held.
   (提供了一个 inode 标志 ``I_PINNING_FSCACHE_WB``，用于表明此 inode 的 cookie 上持有一个使用中标记。只有在持有 inode 锁的情况下才能更改它。)

 * A flag, ``unpinned_fscache_wb`` is placed in the ``writeback_control``
   struct that gets set if ``__writeback_single_inode()`` clears
   ``I_PINNING_FSCACHE_WB`` because all the dirty pages were cleared.
   (``writeback_control`` 结构中放置了一个标志 ``unpinned_fscache_wb``，当 ``__writeback_single_inode()`` 因为所有脏页都被清除而清除 ``I_PINNING_FSCACHE_WB`` 时，该标志会被设置。)

To support this, the following functions are provided::

	bool fscache_dirty_folio(struct address_space *mapping,
				 struct folio *folio,
				 struct fscache_cookie *cookie);
	void fscache_unpin_writeback(struct writeback_control *wbc,
				     struct fscache_cookie *cookie);
	void fscache_clear_inode_writeback(struct fscache_cookie *cookie,
					   struct inode *inode,
					   const void *aux);

(为支持这一点，提供了以下函数：)

The *set* function is intended to be called from the filesystem's
``dirty_folio`` address space operation.  If ``I_PINNING_FSCACHE_WB`` is not
set, it sets that flag and increments the use count on the cookie (the caller
must already have called ``fscache_use_cookie()``).
(*set* 函数应由文件系统的 ``dirty_folio`` 地址空间操作调用。如果 ``I_PINNING_FSCACHE_WB`` 尚未设置，它会设置该标志并递增 cookie 的使用计数(调用者必须已经调用过 ``fscache_use_cookie()``)。)

The *unpin* function is intended to be called from the filesystem's
``write_inode`` superblock operation.  It cleans up after writing by unusing
the cookie if unpinned_fscache_wb is set in the writeback_control struct.
(*unpin* 函数应由文件系统的 ``write_inode`` 超级块操作调用。它在写完成后进行清理：如果 writeback_control 结构中设置了 unpinned_fscache_wb，则 unuse 该 cookie。)

The *clear* function is intended to be called from the netfs's ``evict_inode``
superblock operation.  It must be called *after*
``truncate_inode_pages_final()``, but *before* ``clear_inode()``.  This cleans
up any hanging ``I_PINNING_FSCACHE_WB``.  It also allows the coherency data to
be updated.
(*clear* 函数应由 netfs 的 ``evict_inode`` 超级块操作调用。调用时机必须在 ``truncate_inode_pages_final()`` *之后*、``clear_inode()`` *之前*。这会清理任何遗留的 ``I_PINNING_FSCACHE_WB``。它还允许更新一致性数据。)


Caching of Local Modifications
(本地修改的缓存)
==============================

If a network filesystem has locally modified data that it wants to write to the
cache, it needs to mark the pages to indicate that a write is in progress, and
if the mark is already present, it needs to wait for it to be removed first
(presumably due to an already in-progress operation).  This prevents multiple
competing DIO writes to the same storage in the cache.
(如果网络文件系统有想要写入缓存的本地修改数据，它需要标记这些页以表明写正在进行中；如果标记已存在，则需要先等待其被移除(大概是由于已经在进行中的操作)。这可以防止多个相互竞争的 DIO 写同时写缓存中的同一存储。)

Firstly, the netfs should determine if caching is available by doing something
like::

	bool caching = fscache_cookie_enabled(cookie);

(首先，netfs 应通过执行如下操作来判断缓存是否可用：)

If caching is to be attempted, pages should be waited for and then marked using
the following functions provided by the netfs helper library::

	void set_page_fscache(struct page *page);
	void wait_on_page_fscache(struct page *page);
	int wait_on_page_fscache_killable(struct page *page);

(如果要尝试缓存，应先等待页，然后使用 netfs 帮助库提供的以下函数进行标记：)

Once all the pages in the span are marked, the netfs can ask fscache to
schedule a write of that region::

	void fscache_write_to_cache(struct fscache_cookie *cookie,
				    struct address_space *mapping,
				    loff_t start, size_t len, loff_t i_size,
				    netfs_io_terminated_t term_func,
				    void *term_func_priv,
				    bool caching)

(一旦跨度中的所有页都被标记，netfs 就可以请求 fscache 调度对该区域的写入：)

And if an error occurs before that point is reached, the marks can be removed
by calling::

	void fscache_clear_page_bits(struct address_space *mapping,
				     loff_t start, size_t len,
				     bool caching)

(如果在到达该点之前发生错误，可以通过调用以下函数来移除标记：)

In these functions, a pointer to the mapping to which the source pages are
attached is passed in and start and len indicate the size of the region that's
going to be written (it doesn't have to align to page boundaries necessarily,
but it does have to align to DIO boundaries on the backing filesystem).  The
caching parameter indicates if caching should be skipped, and if false, the
functions do nothing.
(在这些函数中，传入一个指向源页所挂接的映射的指针，start 和 len 指示将要写入的区域的大小(它不一定要按页边界对齐，但必须按底层文件系统的 DIO 边界对齐)。caching 参数指示是否应跳过缓存，若为 false，这些函数什么都不做。)

The write function takes some additional parameters: the cookie representing
the cache object to be written to, i_size indicates the size of the netfs file
and term_func indicates an optional completion function, to which
term_func_priv will be passed, along with the error or amount written.
(write 函数接受一些额外的参数：代表要写入的缓存对象的 cookie；i_size 指示 netfs 文件的大小；term_func 指示一个可选的完成函数，term_func_priv 将连同错误或写入量一起传递给它。)

Note that the write function will always run asynchronously and will unmark all
the pages upon completion before calling term_func.
(注意，write 函数始终异步运行，并会在完成时、调用 term_func 之前，取消所有页的标记。)


Page Release and Invalidation
(页释放与失效)
=============================

Fscache keeps track of whether we have any data in the cache yet for a cache
object we've just created.  It knows it doesn't have to do any reading until it
has done a write and then the page it wrote from has been released by the VM,
after which it *has* to look in the cache.
(Fscache 会跟踪我们刚创建的缓存对象在缓存中是否已有数据。它知道在完成一次写、并且所写的页已被 VM 释放之前，不必进行任何读取；此后它就*必须*查找缓存了。)

To inform fscache that a page might now be in the cache, the following function
should be called from the ``release_folio`` address space op::

	void fscache_note_page_release(struct fscache_cookie *cookie);

(要告知 fscache 某页现在可能已在缓存中，应从 ``release_folio`` 地址空间操作调用以下函数：)

if the page has been released (ie. release_folio returned true).
(如果该页已被释放(即 release_folio 返回 true)。)

Page release and page invalidation should also wait for any mark left on the
page to say that a DIO write is underway from that page::

	void wait_on_page_fscache(struct page *page);
	int wait_on_page_fscache_killable(struct page *page);

(页释放和页失效还应等待页上残留的任何标记，这些标记表明从该页发起的 DIO 写正在进行：)


API Function Reference
(API 函数参考)
======================

.. kernel-doc:: include/linux/fscache.h
