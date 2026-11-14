=========
RPC Cache
=========

(RPC 缓存)

This document gives a brief introduction to the caching
mechanisms in the sunrpc layer that is used, in particular,
for NFS authentication.
(本文档简要介绍 sunrpc 层中的缓存机制，该机制尤其用于 NFS 认证。)

Caches
(缓存)
======

The caching replaces the old exports table and allows for
a wide variety of values to be caches.
(该缓存机制取代了旧的导出(export)表，并允许缓存各种各样的值。)

There are a number of caches that are similar in structure though
quite possibly very different in content and use.  There is a corpus
of common code for managing these caches.
(有许多在结构上相似、但内容和用途很可能截然不同的缓存。有一套用于管理这些缓存的公共代码。)

Examples of caches that are likely to be needed are:
(可能需要的缓存示例如下：)

  - mapping from IP address to client name
    (将 IP 地址映射到客户端名称。)
  - mapping from client name and filesystem to export options
    (将客户端名称和文件系统映射到导出选项。)
  - mapping from UID to list of GIDs, to work around NFS's limitation
    of 16 gids.
    (将 UID 映射到 GID 列表，以规避 NFS 只支持 16 个 gid 的限制。)
  - mappings between local UID/GID and remote UID/GID for sites that
    do not have uniform uid assignment
    (在本地 UID/GID 与远程 UID/GID 之间建立映射，适用于 uid 分配不统一的站点。)
  - mapping from network identify to public key for crypto authentication.
    (将网络标识映射到公钥，用于加密认证。)

The common code handles such things as:
(公共代码处理以下这些事项：)

   - general cache lookup with correct locking
     (带正确加锁的通用缓存查找。)
   - supporting 'NEGATIVE' as well as positive entries
     (除正向条目外，还支持 'NEGATIVE'(负)条目。)
   - allowing an EXPIRED time on cache items, and removing
     items after they expire, and are no longer in-use.
     (允许在缓存项上设置 EXPIRED 时间，并在条目过期且不再被使用后将其移除。)
   - making requests to user-space to fill in cache entries
     (向用户空间发起请求以填充缓存条目。)
   - allowing user-space to directly set entries in the cache
     (允许用户空间直接设置缓存中的条目。)
   - delaying RPC requests that depend on as-yet incomplete
     cache entries, and replaying those requests when the cache entry
     is complete.
     (延迟依赖尚未完成的缓存条目的 RPC 请求，并在该缓存条目完成后重放这些请求。)
   - clean out old entries as they expire.
     (在旧条目过期时将其清除。)

Creating a Cache
(创建缓存)
----------------

-  A cache needs a datum to store.  This is in the form of a
   structure definition that must contain a struct cache_head
   as an element, usually the first.
   It will also contain a key and some content.
   Each cache element is reference counted and contains
   expiry and update times for use in cache management.
   (缓存需要一个要存储的数据。它以一个结构定义的形式存在，该结构必须包含一个 struct cache_head 作为其元素，通常还是第一个元素。它还将包含一个键和一些内容。每个缓存元素都采用引用计数，并包含用于缓存管理的过期时间和更新时间。)
-  A cache needs a "cache_detail" structure that
   describes the cache.  This stores the hash table, some
   parameters for cache management, and some operations detailing how
   to work with particular cache items.
   (缓存需要一个 "cache_detail" 结构来描述该缓存。其中存储了哈希表、一些用于缓存管理的参数，以及一些详细说明如何处理特定缓存项的操作。)

   The operations are:
   (这些操作包括：)

    struct cache_head \*alloc(void)
      This simply allocates appropriate memory and returns
      a pointer to the cache_detail embedded within the
      structure
      (它只是分配适当的内存，并返回一个指针，指向嵌入在该结构内的 cache_detail。)

    void cache_put(struct kref \*)
      This is called when the last reference to an item is
      dropped.  The pointer passed is to the 'ref' field
      in the cache_head.  cache_put should release any
      references create by 'cache_init' and, if CACHE_VALID
      is set, any references created by cache_update.
      It should then release the memory allocated by
      'alloc'.
      (当对一个项的最后一个引用被释放时调用此方法。传入的指针指向 cache_head 中的 'ref' 字段。cache_put 应释放由 'cache_init' 创建的所有引用，并且如果设置了 CACHE_VALID，还要释放由 cache_update 创建的所有引用。然后它应释放由 'alloc' 分配的内存。)

    int match(struct cache_head \*orig, struct cache_head \*new)
      test if the keys in the two structures match.  Return
      1 if they do, 0 if they don't.
      (测试两个结构中的键是否匹配。匹配则返回 1，不匹配则返回 0。)

    void init(struct cache_head \*orig, struct cache_head \*new)
      Set the 'key' fields in 'new' from 'orig'.  This may
      include taking references to shared objects.
      (从 'orig' 设置 'new' 中的 'key' 字段。这可能包括获取对共享对象的引用。)

    void update(struct cache_head \*orig, struct cache_head \*new)
      Set the 'content' fields in 'new' from 'orig'.
      (从 'orig' 设置 'new' 中的 'content' 字段。)

    int cache_show(struct seq_file \*m, struct cache_detail \*cd, struct cache_head \*h)
      Optional.  Used to provide a /proc file that lists the
      contents of a cache.  This should show one item,
      usually on just one line.
      (可选。用于提供一个列出缓存内容的 /proc 文件。它应显示一个项，通常只占一行。)

    int cache_request(struct cache_detail \*cd, struct cache_head \*h, char \*\*bpp, int \*blen)
      Format a request to be send to user-space for an item
      to be instantiated.  \*bpp is a buffer of size \*blen.
      bpp should be moved forward over the encoded message,
      and  \*blen should be reduced to show how much free
      space remains.  Return 0 on success or <0 if not
      enough room or other problem.
      (格式化一个要发送给用户空间的请求，以便实例化一个项。\*bpp 是一个大小为 \*blen 的缓冲区。bpp 应向前移动越过已编码的消息，\*blen 应相应减小以表明还剩多少空闲空间。成功时返回 0，空间不足或出现其他问题时返回 <0。)

    int cache_parse(struct cache_detail \*cd, char \*buf, int len)
      A message from user space has arrived to fill out a
      cache entry.  It is in 'buf' of length 'len'.
      cache_parse should parse this, find the item in the
      cache with sunrpc_cache_lookup_rcu, and update the item
      with sunrpc_cache_update.
      (一条来自用户空间的消息已到达，用于填充一个缓存条目。它位于长度为 'len' 的 'buf' 中。cache_parse 应对其进行分析，用 sunrpc_cache_lookup_rcu 在缓存中找到该项，并用 sunrpc_cache_update 更新该项。)


-  A cache needs to be registered using cache_register().  This
   includes it on a list of caches that will be regularly
   cleaned to discard old data.
   (缓存需要使用 cache_register() 进行注册。这会把它加入一个缓存列表，该列表会被定期清理以丢弃旧数据。)

Using a cache
(使用缓存)
-------------

To find a value in a cache, call sunrpc_cache_lookup_rcu passing a pointer
to the cache_head in a sample item with the 'key' fields filled in.
This will be passed to ->match to identify the target entry.  If no
entry is found, a new entry will be create, added to the cache, and
marked as not containing valid data.
(要在缓存中查找一个值，可调用 sunrpc_cache_lookup_rcu，并传入一个样本项中指向 cache_head 的指针，其中 'key' 字段已填写完毕。该指针会被传递给 ->match 以识别目标条目。如果没有找到条目，则会创建一个新条目、加入缓存，并被标记为不包含有效数据。)

The item returned is typically passed to cache_check which will check
if the data is valid, and may initiate an up-call to get fresh data.
cache_check will return -ENOENT in the entry is negative or if an up
call is needed but not possible, -EAGAIN if an upcall is pending,
or 0 if the data is valid;
(返回的项通常会被传递给 cache_check，由它检查数据是否有效，并可能发起一次上行调用(up-call)以获取新数据。如果条目为负条目，或者需要上行调用但无法进行，cache_check 将返回 -ENOENT；如果上行调用仍在等待中，则返回 -EAGAIN；如果数据有效，则返回 0；)

cache_check can be passed a "struct cache_req\*".  This structure is
typically embedded in the actual request and can be used to create a
deferred copy of the request (struct cache_deferred_req).  This is
done when the found cache item is not uptodate, but the is reason to
believe that userspace might provide information soon.  When the cache
item does become valid, the deferred copy of the request will be
revisited (->revisit).  It is expected that this method will
reschedule the request for processing.
(可以向 cache_check 传递一个 "struct cache_req*"。该结构通常嵌入在实际请求中，可用于创建该请求的延迟副本(struct cache_deferred_req)。当找到的缓存项尚非最新、但有理由相信用户空间可能很快提供信息时，就会这样做。当该缓存项真正变为有效时，请求的延迟副本会被重新访问(->revisit)。预期该方法会重新调度该请求进行处理。)

The value returned by sunrpc_cache_lookup_rcu can also be passed to
sunrpc_cache_update to set the content for the item.  A second item is
passed which should hold the content.  If the item found by _lookup
has valid data, then it is discarded and a new item is created.  This
saves any user of an item from worrying about content changing while
it is being inspected.  If the item found by _lookup does not contain
valid data, then the content is copied across and CACHE_VALID is set.
(sunrpc_cache_lookup_rcu 返回的值也可以传递给 sunrpc_cache_update，用于设置该项的内容。还会传入第二个项，它应当保存该内容。如果 _lookup 找到的项已含有有效数据，则该项会被丢弃并创建一个新项。这样，项的任何使用者都无需担心在自己检查该项期间内容发生变化。如果 _lookup 找到的项不包含有效数据，则内容会被复制过去，并设置 CACHE_VALID。)

Populating a cache
(填充缓存)
------------------

Each cache has a name, and when the cache is registered, a directory
with that name is created in /proc/net/rpc
(每个缓存都有一个名称；当缓存被注册时，会在 /proc/net/rpc 中创建一个以该名称命名的目录。)

This directory contains a file called 'channel' which is a channel
for communicating between kernel and user for populating the cache.
This directory may later contain other files of interacting
with the cache.
(该目录中包含一个名为 'channel' 的文件，它是内核与用户之间为填充缓存而通信的通道。此目录之后还可能包含其他用于与缓存交互的文件。)

The 'channel' works a bit like a datagram socket. Each 'write' is
passed as a whole to the cache for parsing and interpretation.
Each cache can treat the write requests differently, but it is
expected that a message written will contain:
('channel' 的工作方式有点像数据报套接字。每次 '写入' 都会作为一个整体传递给缓存进行解析和解释。每个缓存可以用不同的方式处理写入请求，但预期写入的消息将包含：)

  - a key
    (一个键。)
  - an expiry time
    (一个过期时间。)
  - a content.
    (一份内容。)

with the intention that an item in the cache with the give key
should be create or updated to have the given content, and the
expiry time should be set on that item.
(其意图是：缓存中具有给定键的项应当被创建或更新为具有给定内容，并在该项上设置过期时间。)

Reading from a channel is a bit more interesting.  When a cache
lookup fails, or when it succeeds but finds an entry that may soon
expire, a request is lodged for that cache item to be updated by
user-space.  These requests appear in the channel file.
(从通道读取则更有意思一些。当缓存查找失败，或者查找成功但发现一个可能很快过期的条目时，会登记一个请求，要求用户空间更新该缓存项。这些请求会出现在通道文件中。)

Successive reads will return successive requests.
If there are no more requests to return, read will return EOF, but a
select or poll for read will block waiting for another request to be
added.
(连续读取会依次返回各个请求。如果没有更多请求可返回，read 将返回 EOF，但针对读取的 select 或 poll 会阻塞，等待另一个请求被加入。)

Thus a user-space helper is likely to::
(因此，用户空间助手程序通常类似于：)

  open the channel.
    select for readable
    read a request
    write a response
  loop.

If it dies and needs to be restarted, any requests that have not been
answered will still appear in the file and will be read by the new
instance of the helper.
(如果它死掉并需要重启，任何尚未被应答的请求仍会出现在文件中，并将被助手程序的新实例读取。)

Each cache should define a "cache_parse" method which takes a message
written from user-space and processes it.  It should return an error
(which propagates back to the write syscall) or 0.
(每个缓存都应定义一个 "cache_parse" 方法，它接收一条从用户空间写入的消息并加以处理。它应返回一个错误(该错误会传播回 write 系统调用)或 0。)

Each cache should also define a "cache_request" method which
takes a cache item and encodes a request into the buffer
provided.
(每个缓存还应定义一个 "cache_request" 方法，它接收一个缓存项，并将一个请求编码到所提供的缓冲区中。)

.. note::
  If a cache has no active readers on the channel, and has had not
  active readers for more than 60 seconds, further requests will not be
  added to the channel but instead all lookups that do not find a valid
  entry will fail.  This is partly for backward compatibility: The
  previous nfs exports table was deemed to be authoritative and a
  failed lookup meant a definite 'no'.
  (如果某个缓存在通道上没有活跃的读取者，并且已超过 60 秒没有任何活跃的读取者，则不会再有请求被加入通道，相反，所有未找到有效条目的查找都将失败。这部分是出于向后兼容的考虑：以前的 nfs 导出表被视为权威，查找失败就意味着明确的 '否'。)

request/response format
(请求/响应格式)
-----------------------

While each cache is free to use its own format for requests
and responses over channel, the following is recommended as
appropriate and support routines are available to help:
Each request or response record should be printable ASCII
with precisely one newline character which should be at the end.
Fields within the record should be separated by spaces, normally one.
If spaces, newlines, or nul characters are needed in a field they
much be quoted.  two mechanisms are available:
(虽然每个缓存都可以自由使用自己的格式来通过通道进行请求和响应，但推荐使用以下格式，因为它比较合适，并且有辅助例程可供使用：每个请求或响应记录应当是可打印的 ASCII，且恰好有一个换行符，并位于末尾。记录中的字段应以空格分隔，通常为一个空格。如果字段中需要空格、换行符或 nul 字符，则必须加引号。有两种机制可用：)

-  If a field begins '\x' then it must contain an even number of
   hex digits, and pairs of these digits provide the bytes in the
   field.
   (如果字段以 '\x' 开头，则它必须包含偶数个十六进制数字，这些数字成对地提供该字段中的字节。)
-  otherwise a \ in the field must be followed by 3 octal digits
   which give the code for a byte.  Other characters are treated
   as them selves.  At the very least, space, newline, nul, and
   '\' must be quoted in this way.
   (否则，字段中的 \ 后面必须跟 3 个八进制数字，以给出一个字节的编码。其他字符按其自身处理。至少，空格、换行符、nul 和 '\' 必须以这种方式加引号。)
