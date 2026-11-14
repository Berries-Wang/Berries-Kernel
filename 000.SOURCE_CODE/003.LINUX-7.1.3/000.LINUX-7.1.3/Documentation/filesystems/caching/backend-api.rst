.. SPDX-License-Identifier: GPL-2.0

=================
Cache Backend API
=================

(缓存后端 API)

The FS-Cache system provides an API by which actual caches can be supplied to
FS-Cache for it to then serve out to network filesystems and other interested
parties.  This API is used by::

	#include <linux/fscache-cache.h>.

(FS-Cache 系统提供了一种 API，通过它可以向 FS-Cache 提供实际的缓存，再由 FS-Cache 将其提供给网络文件系统和其他相关方使用。以下模块使用该 API:)


Overview
(概述)
========

Interaction with the API is handled on three levels: cache, volume and data
storage, and each level has its own type of cookie object:
(与该 API 的交互在三个层次上处理：缓存、卷和数据存储，每个层次都有自己类型的 cookie 对象：)

	=======================	=======================
	COOKIE			C TYPE
	=======================	=======================
	Cache cookie		struct fscache_cache (缓存 cookie)
	Volume cookie		struct fscache_volume (卷 cookie)
	Data storage cookie	struct fscache_cookie (数据存储 cookie)
	=======================	=======================

Cookies are used to provide some filesystem data to the cache, manage state and
pin the cache during access in addition to acting as reference points for the
API functions.  Each cookie has a debugging ID that is included in trace points
to make it easier to correlate traces.  Note, though, that debugging IDs are
simply allocated from incrementing counters and will eventually wrap.
(cookie 用于向缓存提供一些文件系统数据、管理状态并在访问期间固定住缓存，同时还作为 API 函数的引用点。每个 cookie 都有一个调试 ID，包含在跟踪点中，以便更容易关联各条跟踪记录。不过要注意，调试 ID 只是从递增计数器中分配的，最终会回绕。)

The cache backend and the network filesystem can both ask for cache cookies -
and if they ask for one of the same name, they'll get the same cookie.  Volume
and data cookies, however, are created at the behest of the filesystem only.
(缓存后端和网络文件系统都可以请求缓存 cookie——如果它们请求同名的一个，将得到同一个 cookie。然而，卷和数据 cookie 只根据文件系统的请求创建。)


Cache Cookies
(缓存 cookie)
=============

Caches are represented in the API by cache cookies.  These are objects of
type::

	struct fscache_cache {
		void		*cache_priv;
		unsigned int	debug_id;
		char		*name;
		...
	};

(API 中用缓存 cookie 来表示缓存。它们是如下类型的对象:)

There are a few fields that the cache backend might be interested in.  The
``debug_id`` can be used in tracing to match lines referring to the same cache
and ``name`` is the name the cache was registered with.  The ``cache_priv``
member is private data provided by the cache when it is brought online.  The
other fields are for internal use.
(有几个字段是缓存后端可能感兴趣的。``debug_id`` 可用于跟踪中以匹配引用同一缓存的行，``name`` 是缓存注册时使用的名称。``cache_priv`` 成员是缓存在上线时提供的私有数据。其他字段供内部使用。)


Registering a Cache
(注册缓存)
===================

When a cache backend wants to bring a cache online, it should first register
the cache name and that will get it a cache cookie.  This is done with::

	struct fscache_cache *fscache_acquire_cache(const char *name);

(当缓存后端想让缓存上线时，应先注册缓存名，这将得到一个缓存 cookie。通过以下调用完成:)

This will look up and potentially create a cache cookie.  The cache cookie may
have already been created by a network filesystem looking for it, in which case
that cache cookie will be used.  If the cache cookie is not in use by another
cache, it will be moved into the preparing state, otherwise it will return
busy.
(这会查找并可能创建一个缓存 cookie。该缓存 cookie 可能已被某个正在寻找它的网络文件系统创建，此时将使用那个缓存 cookie。如果该缓存 cookie 未被另一个缓存使用，它将被移入 preparing 状态，否则将返回忙。)

If successful, the cache backend can then start setting up the cache.  In the
event that the initialisation fails, the cache backend should call::

	void fscache_relinquish_cache(struct fscache_cache *cache);

(如果成功，缓存后端随后就可以开始设置缓存。万一初始化失败，缓存后端应调用:)

to reset and discard the cookie.
(以重置并丢弃该 cookie。)


Bringing a Cache Online
(使缓存上线)
=======================

Once the cache is set up, it can be brought online by calling::

	int fscache_add_cache(struct fscache_cache *cache,
			      const struct fscache_cache_ops *ops,
			      void *cache_priv);

(缓存设置完成后，可以通过调用以下函数使其上线:)

This stores the cache operations table pointer and cache private data into the
cache cookie and moves the cache to the active state, thereby allowing accesses
to take place.
(这会把缓存操作表指针和缓存私有数据存入缓存 cookie，并把缓存移至 active 状态，从而允许访问发生。)


Withdrawing a Cache From Service
(将缓存撤出服务)
================================

The cache backend can withdraw a cache from service by calling this function::

	void fscache_withdraw_cache(struct fscache_cache *cache);

(缓存后端可以通过调用以下函数把缓存撤出服务:)

This moves the cache to the withdrawn state to prevent new cache- and
volume-level accesses from starting and then waits for outstanding cache-level
accesses to complete.
(这会把缓存移至 withdrawn 状态，阻止新的缓存级和卷级访问启动，然后等待未完成的缓存级访问完成。)

The cache must then go through the data storage objects it has and tell fscache
to withdraw them, calling::

	void fscache_withdraw_cookie(struct fscache_cookie *cookie);

(然后缓存必须遍历它拥有的数据存储对象并告诉 fscache 撤回它们，即对每个对象所属的 cookie 调用:)

on the cookie that each object belongs to.  This schedules the specified cookie
for withdrawal.  This gets offloaded to a workqueue.  The cache backend can
wait for completion by calling::

	void fscache_wait_for_objects(struct fscache_cache *cache);

(这会把指定的 cookie 排定撤回。该过程被转交给一个工作队列处理。缓存后端可以通过调用以下函数等待完成:)

Once all the cookies are withdrawn, a cache backend can withdraw all the
volumes, calling::

	void fscache_withdraw_volume(struct fscache_volume *volume);

(一旦所有 cookie 都被撤回，缓存后端就可以撤回所有卷，调用:)

to tell fscache that a volume has been withdrawn.  This waits for all
outstanding accesses on the volume to complete before returning.
(以告诉 fscache 某个卷已被撤回。它在返回之前会等待该卷上所有未完成的访问完成。)

When the cache is completely withdrawn, fscache should be notified by
calling::

	void fscache_relinquish_cache(struct fscache_cache *cache);

(当缓存完全撤回后，应通过以下调用通知 fscache:)

to clear fields in the cookie and discard the caller's ref on it.
(以清除 cookie 中的字段并放弃调用者对它的引用。)


Volume Cookies
(卷 cookie)
==============

Within a cache, the data storage objects are organised into logical volumes.
These are represented in the API as objects of type::

	struct fscache_volume {
		struct fscache_cache		*cache;
		void				*cache_priv;
		unsigned int			debug_id;
		char				*key;
		unsigned int			key_hash;
		...
		u8				coherency_len;
		u8				coherency[];
	};

(在缓存内，数据存储对象被组织成逻辑卷。API 中以如下类型的对象来表示它们:)

There are a number of fields here that are of interest to the caching backend:
(这里有许多缓存后端感兴趣的字段：)

   * ``cache`` - The parent cache cookie.
     (``cache`` - 父缓存 cookie。)

   * ``cache_priv`` - A place for the cache to stash private data.
     (``cache_priv`` - 供缓存存放私有数据的地方。)

   * ``debug_id`` - A debugging ID for logging in tracepoints.
     (``debug_id`` - 用于在跟踪点中记录日志的调试 ID。)

   * ``key`` - A printable string with no '/' characters in it that represents
     the index key for the volume.  The key is NUL-terminated and padded out to
     a multiple of 4 bytes.
     (``key`` - 一个不含 '/' 字符的可打印字符串，表示该卷的索引键。键以 NUL 结尾并填充到 4 字节的倍数。)

   * ``key_hash`` - A hash of the index key.  This should work out the same, no
     matter the cpu arch and endianness.
     (``key_hash`` - 索引键的哈希。无论 CPU 架构和字节序如何，它都应得出相同的结果。)

   * ``coherency`` - A piece of coherency data that should be checked when the
     volume is bound to in the cache.
     (``coherency`` - 一致性数据，在卷绑定到缓存时应进行检查。)

   * ``coherency_len`` - The amount of data in the coherency buffer.
     (``coherency_len`` - 一致性缓冲区中的数据量。)


Data Storage Cookies
(数据存储 cookie)
====================

A volume is a logical group of data storage objects, each of which is
represented to the network filesystem by a cookie.  Cookies are represented in
the API as objects of type::

	struct fscache_cookie {
		struct fscache_volume		*volume;
		void				*cache_priv;
		unsigned long			flags;
		unsigned int			debug_id;
		unsigned int			inval_counter;
		loff_t				object_size;
		u8				advice;
		u32				key_hash;
		u8				key_len;
		u8				aux_len;
		...
	};

(卷是数据存储对象的逻辑分组，每个对象通过一个 cookie 表示给网络文件系统。API 中以如下类型的对象来表示 cookie:)

The fields in the cookie that are of interest to the cache backend are:
(cookie 中缓存后端感兴趣的字段有：)

   * ``volume`` - The parent volume cookie.
     (``volume`` - 父卷 cookie。)

   * ``cache_priv`` - A place for the cache to stash private data.
     (``cache_priv`` - 供缓存存放私有数据的地方。)

   * ``flags`` - A collection of bit flags, including:
     (``flags`` - 一组位标志，包括：)

      * FSCACHE_COOKIE_NO_DATA_TO_READ - There is no data available in the
	cache to be read as the cookie has been created or invalidated.
        (FSCACHE_COOKIE_NO_DATA_TO_READ - 缓存中没有可供读取的数据，因为该 cookie 已被创建或已失效。)

      * FSCACHE_COOKIE_NEEDS_UPDATE - The coherency data and/or object size has
	been changed and needs committing.
        (FSCACHE_COOKIE_NEEDS_UPDATE - 一致性数据和/或对象大小已更改，需要提交。)

      * FSCACHE_COOKIE_LOCAL_WRITE - The netfs's data has been modified
	locally, so the cache object may be in an incoherent state with respect
	to the server.
        (FSCACHE_COOKIE_LOCAL_WRITE - netfs 的数据已在本地修改，因此缓存对象相对于服务器可能处于不一致状态。)

      * FSCACHE_COOKIE_HAVE_DATA - The backend should set this if it
	successfully stores data into the cache.
        (FSCACHE_COOKIE_HAVE_DATA - 如果后端成功地把数据存入了缓存，应设置此标志。)

      * FSCACHE_COOKIE_RETIRED - The cookie was invalidated when it was
	relinquished and the cached data should be discarded.
        (FSCACHE_COOKIE_RETIRED - 该 cookie 在被放弃时已失效，缓存的数据应被丢弃。)

   * ``debug_id`` - A debugging ID for logging in tracepoints.
     (``debug_id`` - 用于在跟踪点中记录日志的调试 ID。)

   * ``inval_counter`` - The number of invalidations done on the cookie.
     (``inval_counter`` - 对该 cookie 执行失效操作的次数。)

   * ``advice`` - Information about how the cookie is to be used.
     (``advice`` - 关于该 cookie 将如何使用的信息。)

   * ``key_hash`` - A hash of the index key.  This should work out the same, no
     matter the cpu arch and endianness.
     (``key_hash`` - 索引键的哈希。无论 CPU 架构和字节序如何，它都应得出相同的结果。)

   * ``key_len`` - The length of the index key.
     (``key_len`` - 索引键的长度。)

   * ``aux_len`` - The length of the coherency data buffer.
     (``aux_len`` - 一致性数据缓冲区的长度。)

Each cookie has an index key, which may be stored inline to the cookie or
elsewhere.  A pointer to this can be obtained by calling::

	void *fscache_get_key(struct fscache_cookie *cookie);

(每个 cookie 都有一个索引键，它可以内联存储在 cookie 中，也可以存放在别处。可以通过调用以下函数获得指向它的指针:)

The index key is a binary blob, the storage for which is padded out to a
multiple of 4 bytes.
(索引键是一个二进制块，其存储空间被填充到 4 字节的倍数。)

Each cookie also has a buffer for coherency data.  This may also be inline or
detached from the cookie and a pointer is obtained by calling::

	void *fscache_get_aux(struct fscache_cookie *cookie);

(每个 cookie 还有一个用于一致性数据的缓冲区。它同样可能内联或与 cookie 分离，通过调用以下函数获得其指针:)



Cookie Accounting
(cookie 计数)
=================

Data storage cookies are counted and this is used to block cache withdrawal
completion until all objects have been destroyed.  The following functions are
provided to the cache to deal with that::

	void fscache_count_object(struct fscache_cache *cache);
	void fscache_uncount_object(struct fscache_cache *cache);
	void fscache_wait_for_objects(struct fscache_cache *cache);

(数据存储 cookie 会被计数，这用于阻止缓存撤回完成，直到所有对象都被销毁。提供给缓存处理此事的函数如下:)

The count function records the allocation of an object in a cache and the
uncount function records its destruction.  Warning: by the time the uncount
function returns, the cache may have been destroyed.
(count 函数记录对象在缓存中的分配，uncount 函数记录其销毁。警告：到 uncount 函数返回时，缓存可能已被销毁。)

The wait function can be used during the withdrawal procedure to wait for
fscache to finish withdrawing all the objects in the cache.  When it completes,
there will be no remaining objects referring to the cache object or any volume
objects.
(wait 函数可在撤回过程中使用，用于等待 fscache 完成撤回缓存中的所有对象。当它完成时，将不再有任何引用该缓存对象或任何卷对象的对象残留。)


Cache Management API
(缓存管理 API)
====================

The cache backend implements the cache management API by providing a table of
operations that fscache can use to manage various aspects of the cache.  These
are held in a structure of type::

	struct fscache_cache_ops {
		const char *name;
		...
	};

(缓存后端通过提供一张 fscache 可用来管理缓存方方面面的操作表来实现缓存管理 API。这些操作保存在如下类型的结构中:)

This contains a printable name for the cache backend driver plus a number of
pointers to methods to allow fscache to request management of the cache:
(其中包含缓存后端驱动程序的可打印名称，以及若干方法指针，允许 fscache 请求对缓存进行管理：)

   * Set up a volume cookie [optional]::

	void (*acquire_volume)(struct fscache_volume *volume);

     (设置一个卷 cookie [可选]:)

     This method is called when a volume cookie is being created.  The caller
     holds a cache-level access pin to prevent the cache from going away for
     the duration.  This method should set up the resources to access a volume
     in the cache and should not return until it has done so.
     (该方法在卷 cookie 正在创建时被调用。调用者在整个期间持有缓存级访问 pin，以防止缓存在此期间消失。该方法应设置好访问缓存中某个卷所需的资源，并且直到完成才应返回。)

     If successful, it can set ``cache_priv`` to its own data.
     (如果成功，它可以把 ``cache_priv`` 设置为自己的数据。)


   * Clean up volume cookie [optional]::

       void (*free_volume)(struct fscache_volume *volume);

     (清理卷 cookie [可选]:)

     This method is called when a volume cookie is being released if
     ``cache_priv`` is set.
     (当卷 cookie 被释放且 ``cache_priv`` 已设置时，会调用该方法。)


   * Look up a cookie in the cache [mandatory]::

	bool (*lookup_cookie)(struct fscache_cookie *cookie);

     (在缓存中查找一个 cookie [必需]:)

     This method is called to look up/create the resources needed to access the
     data storage for a cookie.  It is called from a worker thread with a
     volume-level access pin in the cache to prevent it from being withdrawn.
     (调用该方法是为了查找/创建访问某个 cookie 的数据存储所需的资源。它从一个工作线程中被调用，此时持有缓存中的卷级访问 pin，以防止缓存被撤回。)

     True should be returned if successful and false otherwise.  If false is
     returned, the withdraw_cookie op (see below) will be called.
     (成功时应返回 true，否则返回 false。如果返回 false，将调用 withdraw_cookie 操作(见下文)。)

     If lookup fails, but the object could still be created (e.g. it hasn't
     been cached before), then::

		void fscache_cookie_lookup_negative(
			struct fscache_cookie *cookie);

     (如果查找失败，但对象仍然可以创建(例如它之前没有被缓存过)，那么:)

     can be called to let the network filesystem proceed and start downloading
     stuff whilst the cache backend gets on with the job of creating things.
     (可以调用它让网络文件系统继续推进并开始下载内容，同时缓存后端继续着手创建工作。)

     If successful, ``cookie->cache_priv`` can be set.
     (如果成功，可以设置 ``cookie->cache_priv``。)


   * Withdraw an object without any cookie access counts held [mandatory]::

	void (*withdraw_cookie)(struct fscache_cookie *cookie);

     (撤回一个未持有任何 cookie 访问计数的对象 [必需]:)

     This method is called to withdraw a cookie from service.  It will be
     called when the cookie is relinquished by the netfs, withdrawn or culled
     by the cache backend or closed after a period of non-use by fscache.
     (调用该方法是为了把 cookie 撤出服务。它会在 cookie 被 netfs 放弃、被缓存后端撤回或剔除、或在一段时间未使用后被 fscache 关闭时被调用。)

     The caller doesn't hold any access pins, but it is called from a
     non-reentrant work item to manage races between the various ways
     withdrawal can occur.
     (调用者不持有任何访问 pin，但它从一个不可重入的工作项中被调用，以管理各种撤回途径之间的竞争。)

     The cookie will have the ``FSCACHE_COOKIE_RETIRED`` flag set on it if the
     associated data is to be removed from the cache.
     (如果相关数据要从缓存中移除，cookie 将被设置 ``FSCACHE_COOKIE_RETIRED`` 标志。)


   * Change the size of a data storage object [mandatory]::

	void (*resize_cookie)(struct netfs_cache_resources *cres,
			      loff_t new_size);

     (更改数据存储对象的大小 [必需]:)

     This method is called to inform the cache backend of a change in size of
     the netfs file due to local truncation.  The cache backend should make all
     of the changes it needs to make before returning as this is done under the
     netfs inode mutex.
     (调用该方法是为了把 netfs 文件因本地截断而发生的大小变化通知缓存后端。缓存后端应在返回之前完成它需要做的所有更改，因为这是在 netfs inode 互斥锁保护下进行的。)

     The caller holds a cookie-level access pin to prevent a race with
     withdrawal and the netfs must have the cookie marked in-use to prevent
     garbage collection or culling from removing any resources.
     (调用者持有 cookie 级访问 pin 以防止与撤回产生竞争，并且 netfs 必须已把 cookie 标记为 in-use，以防止垃圾回收或剔除移除任何资源。)


   * Invalidate a data storage object [mandatory]::

	bool (*invalidate_cookie)(struct fscache_cookie *cookie);

     (使一个数据存储对象失效 [必需]:)

     This is called when the network filesystem detects a third-party
     modification or when an O_DIRECT write is made locally.  This requests
     that the cache backend should throw away all the data in the cache for
     this object and start afresh.  It should return true if successful and
     false otherwise.
     (当网络文件系统检测到第三方修改、或在本地进行 O_DIRECT 写入时会调用此方法。它请求缓存后端丢弃该对象在缓存中的所有数据并从头开始。成功时应返回 true，否则返回 false。)

     On entry, new I O/operations are blocked.  Once the cache is in a position
     to accept I/O again, the backend should release the block by calling::

	void fscache_resume_after_invalidation(struct fscache_cookie *cookie);

     (进入时，新的 I/O 操作被阻塞。一旦缓存能够再次接受 I/O，后端应通过调用以下函数解除阻塞:)

     If the method returns false, caching will be withdrawn for this cookie.
     (如果该方法返回 false，此 cookie 的缓存将被撤回。)


   * Prepare to make local modifications to the cache [mandatory]::

	void (*prepare_to_write)(struct fscache_cookie *cookie);

     (准备对缓存进行本地修改 [必需]:)

     This method is called when the network filesystem finds that it is going
     to need to modify the contents of the cache due to local writes or
     truncations.  This gives the cache a chance to note that a cache object
     may be incoherent with respect to the server and may need writing back
     later.  This may also cause the cached data to be scrapped on later
     rebinding if not properly committed.
     (当网络文件系统发现由于本地写入或截断而将需要修改缓存内容时会调用此方法。这给缓存一个机会去记录：某个缓存对象相对于服务器可能不一致，之后可能需要回写。如果没有被正确提交，这也可能导致缓存的数据在之后重新绑定时被废弃。)


   * Begin an operation for the netfs lib [mandatory]::

	bool (*begin_operation)(struct netfs_cache_resources *cres,
				enum fscache_want_state want_state);

     (为 netfs 库开始一个操作 [必需]:)

     This method is called when an I/O operation is being set up (read, write
     or resize).  The caller holds an access pin on the cookie and must have
     marked the cookie as in-use.
     (当正在设置一个 I/O 操作(读、写或调整大小)时会调用此方法。调用者持有该 cookie 上的访问 pin，并且必须已把 cookie 标记为 in-use。)

     If it can, the backend should attach any resources it needs to keep around
     to the netfs_cache_resources object and return true.
     (如果可以，后端应把它需要保留的任何资源附加到 netfs_cache_resources 对象上，并返回 true。)

     If it can't complete the setup, it should return false.
     (如果无法完成设置，应返回 false。)

     The want_state parameter indicates the state the caller needs the cache
     object to be in and what it wants to do during the operation:
     (want_state 参数指示调用者需要缓存对象处于的状态，以及它在操作期间想做的事情：)

	* ``FSCACHE_WANT_PARAMS`` - The caller just wants to access cache
	  object parameters; it doesn't need to do data I/O yet.
	  (``FSCACHE_WANT_PARAMS`` - 调用者只想访问缓存对象参数；还不需要进行数据 I/O。)

	* ``FSCACHE_WANT_READ`` - The caller wants to read data.
	  (``FSCACHE_WANT_READ`` - 调用者想要读取数据。)

	* ``FSCACHE_WANT_WRITE`` - The caller wants to write to or resize the
          cache object.
	  (``FSCACHE_WANT_WRITE`` - 调用者想要写入或调整缓存对象的大小。)

     Note that there won't necessarily be anything attached to the cookie's
     cache_priv yet if the cookie is still being created.
     (注意，如果 cookie 仍在创建过程中，cookie 的 cache_priv 上不一定已经附加了任何东西。)


Data I/O API
(数据 I/O API)
============

A cache backend provides a data I/O API by through the netfs library's ``struct
netfs_cache_ops`` attached to a ``struct netfs_cache_resources`` by the
``begin_operation`` method described above.
(缓存后端通过 netfs 库的 ``struct netfs_cache_ops`` 提供数据 I/O API，该结构由上文描述的 ``begin_operation`` 方法附加到 ``struct netfs_cache_resources`` 上。)

See the Documentation/filesystems/netfs_library.rst for a description.
(描述参见 Documentation/filesystems/netfs_library.rst。)


Miscellaneous Functions
(杂项函数)
=======================

FS-Cache provides some utilities that a cache backend may make use of:
(FS-Cache 提供了一些缓存后端可能会用到的实用工具：)

   * Note occurrence of an I/O error in a cache::

	void fscache_io_error(struct fscache_cache *cache);

     (记录缓存中发生了一次 I/O 错误:)

     This tells FS-Cache that an I/O error occurred in the cache.  This
     prevents any new I/O from being started on the cache.
     (这告诉 FS-Cache 缓存中发生了一次 I/O 错误。这会阻止在该缓存上启动任何新的 I/O。)

     This does not actually withdraw the cache.  That must be done separately.
     (这并不会真正撤回缓存。撤回必须单独进行。)

   * Note cessation of caching on a cookie due to failure::

	void fscache_caching_failed(struct fscache_cookie *cookie);

     (记录因失败而停止对某个 cookie 的缓存:)

     This notes that a the caching that was being done on a cookie failed in
     some way, for instance the backing storage failed to be created or
     invalidation failed and that no further I/O operations should take place
     on it until the cache is reset.
     (这记录了对某个 cookie 正在进行的缓存因某种原因而失败，例如后备存储创建失败或失效失败，并且在缓存被重置之前，不应再对该 cookie 进行任何 I/O 操作。)

   * Count I/O requests::

	void fscache_count_read(void);
	void fscache_count_write(void);

     (对 I/O 请求计数:)

     These record reads and writes from/to the cache.  The numbers are
     displayed in /proc/fs/fscache/stats.
     (这些函数记录对缓存的读和写。数字显示在 /proc/fs/fscache/stats 中。)

   * Count out-of-space errors::

	void fscache_count_no_write_space(void);
	void fscache_count_no_create_space(void);

     (对空间不足错误计数:)

     These record ENOSPC errors in the cache, divided into failures of data
     writes and failures of filesystem object creations (e.g. mkdir).
     (这些函数记录缓存中的 ENOSPC 错误，分为数据写失败和文件系统对象创建失败(例如 mkdir)两类。)

   * Count objects culled::

	void fscache_count_culled(void);

     (对被剔除的对象计数:)

     This records the culling of an object.
     (这记录了一个对象被剔除。)

   * Get the cookie from a set of cache resources::

	struct fscache_cookie *fscache_cres_cookie(struct netfs_cache_resources *cres)

     (从一组缓存资源中获取 cookie:)

     Pull a pointer to the cookie from the cache resources.  This may return a
     NULL cookie if no cookie was set.
     (从缓存资源中取出指向 cookie 的指针。如果未设置 cookie，可能返回 NULL cookie。)


API Function Reference
(API 函数参考)
======================

.. kernel-doc:: include/linux/fscache-cache.h
