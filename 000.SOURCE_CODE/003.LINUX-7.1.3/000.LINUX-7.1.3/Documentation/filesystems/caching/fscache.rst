.. SPDX-License-Identifier: GPL-2.0

==========================
General Filesystem Caching
==========================

(通用文件系统缓存)

Overview
(概述)
========

This facility is a general purpose cache for network filesystems, though it
could be used for caching other things such as ISO9660 filesystems too.
(这一设施是一个面向网络文件系统的通用缓存，不过它也可以用于缓存其他东西，比如 ISO9660 文件系统。)

FS-Cache mediates between cache backends (such as CacheFiles) and network
filesystems::
(FS-Cache 在缓存后端(如 CacheFiles)与网络文件系统之间进行中介：)

	+---------+
	|         |                                    +--------------+
	|   NFS   |--+                                 |              |
	|         |  |                             +-->|   CacheFS    |
	+---------+  |               +----------+  |   |  /dev/hda5   |
	             |               |          |  |   +--------------+
	+---------+  +-------------->|          |  |
	|         |      +-------+   |          |--+
	|   AFS   |----->|       |   | FS-Cache |
	|         |      | netfs |-->|          |--+
	+---------+  +-->|  lib  |   |          |  |
	             |   |       |   |          |  |   +--------------+
	+---------+  |   +-------+   +----------+  |   |              |
	|         |  |                             +-->|  CacheFiles  |
	|   9P    |--+                                 |  /var/cache  |
	|         |                                    +--------------+
	+---------+

Or to look at it another way, FS-Cache is a module that provides a caching
facility to a network filesystem such that the cache is transparent to the
user::
(或者换一种方式来看，FS-Cache 是一个为网络文件系统提供缓存设施的模块，使得缓存对用户是透明的：)

	+---------+
	|         |
	| Server  |
	|         |
	+---------+
	     |                  NETWORK
	~~~~~|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	     |
	     |           +----------+
	     V           |          |
	+---------+      |          |
	|         |      |          |
	|   NFS   |----->| FS-Cache |
	|         |      |          |--+
	+---------+      |          |  |   +--------------+   +--------------+
	     |           |          |  |   |              |   |              |
	     V           +----------+  +-->|  CacheFiles  |-->|  Ext3        |
	+---------+                        |  /var/cache  |   |  /dev/sda6   |
	|         |                        +--------------+   +--------------+
	|   VFS   |                                ^                     ^
	|         |                                |                     |
	+---------+                                +--------------+      |
	     |                  KERNEL SPACE                      |      |
	~~~~~|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|~~~~~~|~~~~
	     |                  USER SPACE                        |      |
	     V                                                    |      |
	+---------+                                           +--------------+
	|         |                                           |              |
	| Process |                                           | cachefilesd  |
	|         |                                           |              |
	+---------+                                           +--------------+


FS-Cache does not follow the idea of completely loading every netfs file
opened in its entirety into a cache before permitting it to be accessed and
then serving the pages out of that cache rather than the netfs inode because:
(FS-Cache 并不遵循这样的做法：先把打开的每个 netfs 文件完整地加载到缓存中，然后才允许访问，并从该缓存而不是 netfs inode 提供页面，原因如下：)

 (1) It must be practical to operate without a cache.
     (必须能够在没有缓存的情况下切实可行地工作。)

 (2) The size of any accessible file must not be limited to the size of the
     cache.
     (任何可访问文件的大小都不能被限制为缓存的大小。)

 (3) The combined size of all opened files (this includes mapped libraries)
     must not be limited to the size of the cache.
     (所有已打开文件的总大小(包括映射的库)不能被限制为缓存的大小。)

 (4) The user should not be forced to download an entire file just to do a
     one-off access of a small portion of it (such as might be done with the
     "file" program).
     (不应强迫用户为了访问文件的一小部分而下载整个文件(比如用 "file" 程序时可能做的那样)。)

It instead serves the cache out in chunks as and when requested by the netfs
using it.
(相反，它在使用它的 netfs 提出请求时按块提供缓存。)


FS-Cache provides the following facilities:
(FS-Cache 提供以下设施：)

   * More than one cache can be used at once.  Caches can be selected
     explicitly by use of tags.
     (可以同时使用多个缓存。可以通过使用标签来显式地选择缓存。)

   * Caches can be added / removed at any time, even whilst being accessed.
     (可以在任何时候添加/移除缓存，即使缓存正被访问时也是如此。)

   * The netfs is provided with an interface that allows either party to
     withdraw caching facilities from a file (required for (2)).
     (为 netfs 提供了一个接口，允许任何一方从文件撤回缓存设施((2)所要求的)。)

   * The interface to the netfs returns as few errors as possible, preferring
     rather to let the netfs remain oblivious.
     (面向 netfs 的接口尽可能少地返回错误，而宁愿让 netfs 保持毫不知情。)

   * There are three types of cookie: cache, volume and data file cookies.
     Cache cookies represent the cache as a whole and are not normally visible
     to the netfs; the netfs gets a volume cookie to represent a collection of
     files (typically something that a netfs would get for a superblock); and
     data file cookies are used to cache data (something that would be got for
     an inode).
     (cookie 有三种类型：缓存 cookie、卷 cookie 和数据文件 cookie。缓存 cookie 把缓存作为一个整体来表示，通常对 netfs 不可见；netfs 会得到一个卷 cookie 来表示一组文件(通常是 netfs 为某个超级块获得的东西)；数据文件 cookie 用于缓存数据(通常是为某个 inode 获得的东西)。)

   * Volumes are matched using a key.  This is a printable string that is used
     to encode all the information that might be needed to distinguish one
     superblock, say, from another.  This would be a compound of things like
     cell name or server address, volume name or share path.  It must be a
     valid pathname.
     (卷通过键来匹配。这是一个可打印的字符串，用于编码区分一个超级块与另一个超级块(举例来说)可能需要的所有信息。它可以是诸如 cell 名或服务器地址、卷名或共享路径之类东西的组合。它必须是一个有效的路径名。)

   * Cookies are matched using a key.  This is a binary blob and is used to
     represent the object within a volume (so the volume key need not form
     part of the blob).  This might include things like an inode number and
     uniquifier or a file handle.
     (cookie 通过键来匹配。这是一个二进制数据块，用于表示卷内的对象(因此卷键不必构成该数据块的一部分)。它可能包括 inode 编号和唯一标识符或文件句柄之类的东西。)

   * Cookie resources are set up and pinned by marking the cookie in-use.
     This prevents the backing resources from being culled.  Timed garbage
     collection is employed to eliminate cookies that haven't been used for a
     short while, thereby reducing resource overload.  This is intended to be
     used when a file is opened or closed.
     (cookie 资源通过把 cookie 标记为 in-use 来建立并固定。这可以防止后备资源被剔除。采用定时垃圾回收来清除一段时间未使用的 cookie，从而减少资源过载。它打算在文件被打开或关闭时使用。)

     A cookie can be marked in-use multiple times simultaneously; each mark
     must be unused.
     (一个 cookie 可以同时被多次标记为 in-use；每个标记必须被解除(unused)。)

   * Begin/end access functions are provided to delay cache withdrawal for the
     duration of an operation and prevent structs from being freed whilst
     we're looking at them.
     (提供了 begin/end 访问函数，用于在操作期间延迟缓存的撤除，并防止结构体在我们查看它们时被释放。)

   * Data I/O is done by asynchronous DIO to/from a buffer described by the
     netfs using an iov_iter.
     (数据 I/O 通过异步 DIO 在 netfs 用 iov_iter 描述的缓冲区上进行。)

   * An invalidation facility is available to discard data from the cache and
     to deal with I/O that's in progress that is accessing old data.
     (提供了一个失效(invalidation)设施，用于从缓存丢弃数据，并处理正在访问旧数据的进行中的 I/O。)

   * Cookies can be "retired" upon release, thereby causing the object to be
     removed from the cache.
     (cookie 可以在释放时"退役"(retire)，从而使对象从缓存中被移除。)


The netfs API to FS-Cache can be found in:
(面向 FS-Cache 的 netfs API 可以在以下位置找到：)

	Documentation/filesystems/caching/netfs-api.rst

The cache backend API to FS-Cache can be found in:
(面向 FS-Cache 的缓存后端 API 可以在以下位置找到：)

	Documentation/filesystems/caching/backend-api.rst


Statistical Information
(统计信息)
=======================

If FS-Cache is compiled with the following options enabled::
(如果 FS-Cache 编译时启用了以下选项：)

	CONFIG_FSCACHE_STATS=y

then it will gather certain statistics and display them through:
(它就会收集某些统计信息并通过以下位置显示它们：)

	/proc/fs/fscache/stats

This shows counts of a number of events that can happen in FS-Cache:
(它显示了 FS-Cache 中可能发生的许多事件的计数：)

+--------------+-------+-------------------------------------------------------+
|CLASS         |EVENT  |MEANING                                                |
+==============+=======+=======================================================+
|Cookies       |n=N    |Number of data storage cookies allocated (已分配的数据存储 cookie 数量)               |
+              +-------+-------------------------------------------------------+
|              |v=N    |Number of volume index cookies allocated (已分配的卷索引 cookie 数量)               |
+              +-------+-------------------------------------------------------+
|              |vcol=N |Number of volume index key collisions (卷索引键冲突的数量)                  |
+              +-------+-------------------------------------------------------+
|              |voom=N |Number of OOM events when allocating volume cookies (分配卷 cookie 时的 OOM 事件数量)    |
+--------------+-------+-------------------------------------------------------+
|Acquire       |n=N    |Number of acquire cookie requests seen (见到的获取 cookie 请求数量)                 |
+              +-------+-------------------------------------------------------+
|              |ok=N   |Number of acq reqs succeeded (成功的获取请求数量)                           |
+              +-------+-------------------------------------------------------+
|              |oom=N  |Number of acq reqs failed on ENOMEM (因 ENOMEM 而失败的获取请求数量)                    |
+--------------+-------+-------------------------------------------------------+
|LRU           |n=N    |Number of cookies currently on the LRU (当前位于 LRU 上的 cookie 数量)                 |
+              +-------+-------------------------------------------------------+
|              |exp=N  |Number of cookies expired off of the LRU (从 LRU 过期的 cookie 数量)               |
+              +-------+-------------------------------------------------------+
|              |rmv=N  |Number of cookies removed from the LRU (从 LRU 移除的 cookie 数量)                 |
+              +-------+-------------------------------------------------------+
|              |drp=N  |Number of LRU'd cookies relinquished/withdrawn (被放弃/撤除的 LRU cookie 数量)         |
+              +-------+-------------------------------------------------------+
|              |at=N   |Time till next LRU cull (jiffies) (距下次 LRU 剔除的时间(以 jiffies 为单位))                      |
+--------------+-------+-------------------------------------------------------+
|Invals        |n=N    |Number of invalidations (失效操作的数量)                                |
+--------------+-------+-------------------------------------------------------+
|Updates       |n=N    |Number of update cookie requests seen (见到的更新 cookie 请求数量)                  |
+              +-------+-------------------------------------------------------+
|              |rsz=N  |Number of resize requests (调整大小请求的数量)                              |
+              +-------+-------------------------------------------------------+
|              |rsn=N  |Number of skipped resize requests (被跳过的调整大小请求数量)                      |
+--------------+-------+-------------------------------------------------------+
|Relinqs       |n=N    |Number of relinquish cookie requests seen (见到的放弃 cookie 请求数量)              |
+              +-------+-------------------------------------------------------+
|              |rtr=N  |Number of rlq reqs with retire=true (带 retire=true 的放弃请求数量)                    |
+              +-------+-------------------------------------------------------+
|              |drop=N |Number of cookies no longer blocking re-acquisition (不再阻止重新获取的 cookie 数量)    |
+--------------+-------+-------------------------------------------------------+
|NoSpace       |nwr=N  |Number of write requests refused due to lack of space (因空间不足而被拒绝的写请求数量)  |
+              +-------+-------------------------------------------------------+
|              |ncr=N  |Number of create requests refused due to lack of space (因空间不足而被拒绝的创建请求数量) |
+              +-------+-------------------------------------------------------+
|              |cull=N |Number of objects culled to make space (为腾出空间而被剔除的对象数量)                 |
+--------------+-------+-------------------------------------------------------+
|IO            |rd=N   |Number of read operations in the cache (缓存中的读操作数量)                 |
+              +-------+-------------------------------------------------------+
|              |wr=N   |Number of write operations in the cache (缓存中的写操作数量)                |
+--------------+-------+-------------------------------------------------------+

Netfslib will also add some stats counters of its own.
(Netfslib 还会添加它自己的一些统计计数器。)


Cache List
(缓存列表)
==========

FS-Cache provides a list of cache cookies:
(FS-Cache 提供缓存 cookie 的列表：)

	/proc/fs/fscache/cookies

This will look something like::
(它看起来类似于：)

	# cat /proc/fs/fscache/caches
	CACHE    REF   VOLS  OBJS  ACCES S NAME
	======== ===== ===== ===== ===== = ===============
	00000001     2     1  2123     1 A default

where the columns are:
(其中各列的含义如下：)

	=======	===============================================================
	COLUMN	DESCRIPTION
	=======	===============================================================
	CACHE	Cache cookie debug ID (also appears in traces) (缓存 cookie 调试 ID(也出现在跟踪中))
	REF	Number of references on the cache cookie (缓存 cookie 上的引用数量)
	VOLS	Number of volumes cookies in this cache (此缓存中卷 cookie 的数量)
	OBJS	Number of cache objects in use (正在使用的缓存对象数量)
	ACCES	Number of accesses pinning the cache (固定此缓存的访问数量)
	S	State (状态)
	NAME	Name of the cache. (缓存的名称。)
	=======	===============================================================

The state can be (-) Inactive, (P)reparing, (A)ctive, (E)rror or (W)ithdrawing.
(状态可以是 (-) 非活动(Inactive)、(P) 准备中(Preparing)、(A) 活动(Active)、(E) 错误(Error)或 (W) 正在退出(Withdrawing)。)


Volume List
(卷列表)
===========

FS-Cache provides a list of volume cookies:
(FS-Cache 提供卷 cookie 的列表：)

	/proc/fs/fscache/volumes

This will look something like::
(它看起来类似于：)

	VOLUME   REF   nCOOK ACC FL CACHE           KEY
	======== ===== ===== === == =============== ================
	00000001    55    54   1 00 default         afs,example.com,100058

where the columns are:
(其中各列的含义如下：)

	=======	===============================================================
	COLUMN	DESCRIPTION
	=======	===============================================================
	VOLUME	The volume cookie debug ID (also appears in traces) (卷 cookie 调试 ID(也出现在跟踪中))
	REF	Number of references on the volume cookie (卷 cookie 上的引用数量)
	nCOOK	Number of cookies in the volume (卷中 cookie 的数量)
	ACC	Number of accesses pinning the cache (固定此缓存的访问数量)
	FL	Flags on the volume cookie (卷 cookie 上的标志)
	CACHE	Name of the cache or "-" (缓存的名称或 "-")
	KEY	The indexing key for the volume (卷的索引键)
	=======	===============================================================


Cookie List
(cookie 列表)
===========

FS-Cache provides a list of cookies:
(FS-Cache 提供 cookie 的列表：)

	/proc/fs/fscache/cookies

This will look something like::
(它看起来类似于：)

	# head /proc/fs/fscache/cookies
	COOKIE   VOLUME   REF ACT ACC S FL DEF
	======== ======== === === === = == ================
	00000435 00000001   1   0  -1 - 08 0000000201d080070000000000000000, 0000000000000000
	00000436 00000001   1   0  -1 - 00 0000005601d080080000000000000000, 0000000000000051
	00000437 00000001   1   0  -1 - 08 00023b3001d0823f0000000000000000, 0000000000000000
	00000438 00000001   1   0  -1 - 08 0000005801d0807b0000000000000000, 0000000000000000
	00000439 00000001   1   0  -1 - 08 00023b3201d080a10000000000000000, 0000000000000000
	0000043a 00000001   1   0  -1 - 08 00023b3401d080a30000000000000000, 0000000000000000
	0000043b 00000001   1   0  -1 - 08 00023b3601d080b30000000000000000, 0000000000000000
	0000043c 00000001   1   0  -1 - 08 00023b3801d080b40000000000000000

where the columns are:
(其中各列的含义如下：)

	=======	===============================================================
	COLUMN	DESCRIPTION
	=======	===============================================================
	COOKIE	The cookie debug ID (also appears in traces) (cookie 调试 ID(也出现在跟踪中))
	VOLUME	The parent volume cookie debug ID (父卷 cookie 的调试 ID)
	REF	Number of references on the volume cookie (卷 cookie 上的引用数量)
	ACT	Number of times the cookie is marked for in use (cookie 被标记为 in-use 的次数)
	ACC	Number of access pins in the cookie (cookie 中的访问固定数)
	S	State of the cookie (cookie 的状态)
	FL	Flags on the cookie (cookie 上的标志)
	DEF	Key, auxiliary data (键、辅助数据)
	=======	===============================================================


Debugging
(调试)
=========

If CONFIG_NETFS_DEBUG is enabled, the FS-Cache facility and NETFS support can
have runtime debugging enabled by adjusting the value in::
(如果启用了 CONFIG_NETFS_DEBUG，可以通过调整以下位置中的值来为 FS-Cache 设施和 NETFS 支持启用运行时调试：)

	/sys/module/netfs/parameters/debug

This is a bitmask of debugging streams to enable:
(这是要启用的调试流位掩码：)

	=======	=======	===============================	=======================
	BIT	VALUE	STREAM				POINT
	=======	=======	===============================	=======================
	0	1	Cache management		Function entry trace (缓存管理: 函数进入跟踪)
	1	2					Function exit trace (函数退出跟踪)
	2	4					General (通用)
	3	8	Cookie management		Function entry trace (cookie 管理: 函数进入跟踪)
	4	16					Function exit trace (函数退出跟踪)
	5	32					General (通用)
	6-8						(Not used) (未使用)
	9	512	I/O operation management	Function entry trace (I/O 操作管理: 函数进入跟踪)
	10	1024					Function exit trace (函数退出跟踪)
	11	2048					General (通用)
	=======	=======	===============================	=======================

The appropriate set of values should be OR'd together and the result written to
the control file.  For example::
(应当把相应的一组值用 OR 组合起来，并把结果写入控制文件。例如：)

	echo $((1|8|512)) >/sys/module/netfs/parameters/debug

will turn on all function entry debugging.
(将打开所有函数进入调试。)
