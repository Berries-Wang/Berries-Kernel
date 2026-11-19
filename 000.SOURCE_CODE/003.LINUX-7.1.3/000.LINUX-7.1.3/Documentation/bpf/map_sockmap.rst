.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright Red Hat

========================================================================
BPF_MAP_TYPE_SOCKMAP and BPF_MAP_TYPE_SOCKHASH(sockmap 与 sockhash 映射)
========================================================================

.. note::
   - ``BPF_MAP_TYPE_SOCKMAP`` was introduced in kernel version 4.14
     ( ``BPF_MAP_TYPE_SOCKMAP`` 在内核版本 4.14 中引入。)
   - ``BPF_MAP_TYPE_SOCKHASH`` was introduced in kernel version 4.18
     ( ``BPF_MAP_TYPE_SOCKHASH`` 在内核版本 4.18 中引入。)

``BPF_MAP_TYPE_SOCKMAP`` and ``BPF_MAP_TYPE_SOCKHASH`` maps can be used to
redirect skbs between sockets or to apply policy at the socket level based on
the result of a BPF (verdict) program with the help of the BPF helpers
``bpf_sk_redirect_map()``, ``bpf_sk_redirect_hash()``,
``bpf_msg_redirect_map()`` and ``bpf_msg_redirect_hash()``.
( ``BPF_MAP_TYPE_SOCKMAP`` 与 ``BPF_MAP_TYPE_SOCKHASH`` 映射可以在套接字之间
重定向 skb，或者借助 BPF 辅助函数 ``bpf_sk_redirect_map()``、
``bpf_sk_redirect_hash()``、``bpf_msg_redirect_map()`` 和
``bpf_msg_redirect_hash()``，根据 BPF (裁定) 程序的结果在套接字层面实施策略。)

``BPF_MAP_TYPE_SOCKMAP`` is backed by an array that uses an integer key as the
index to look up a reference to a ``struct sock``. The map values are socket
descriptors. Similarly, ``BPF_MAP_TYPE_SOCKHASH`` is a hash backed BPF map that
holds references to sockets via their socket descriptors.
( ``BPF_MAP_TYPE_SOCKMAP`` 由一个数组支撑，该数组使用整数键作为索引来查找指向
``struct sock`` 的引用。映射值是套接字描述符。类似地， ``BPF_MAP_TYPE_SOCKHASH``
是一个以哈希为支撑的 BPF 映射，通过套接字描述符持有对套接字的引用。)

.. note::
    The value type is either __u32 or __u64; the latter (__u64) is to support
    returning socket cookies to userspace. Returning the ``struct sock *`` that
    the map holds to user-space is neither safe nor useful.
    (值的类型是 __u32 或 __u64；后者 (__u64) 用于支持向用户空间返回 socket
    cookie。把映射持有的 ``struct sock *`` 返回给用户空间既不安全也无用处。)

These maps may have BPF programs attached to them, specifically a parser program
and a verdict program. The parser program determines how much data has been
parsed and therefore how much data needs to be queued to come to a verdict. The
verdict program is essentially the redirect program and can return a verdict
of ``__SK_DROP``, ``__SK_PASS``, or ``__SK_REDIRECT``.
(这些映射可以挂接 BPF 程序，具体是一个解析器 (parser) 程序和一个裁定 (verdict)
程序。解析器程序确定已解析了多少数据，从而确定要排队多少数据才能得出裁定。
裁定程序本质上就是重定向程序，可以返回 ``__SK_DROP`` 、 ``__SK_PASS`` 或
``__SK_REDIRECT`` 裁定。)

When a socket is inserted into one of these maps, its socket callbacks are
replaced and a ``struct sk_psock`` is attached to it. Additionally, this
``sk_psock`` inherits the programs that are attached to the map.
(当套接字被插入其中一个映射时，它的套接字回调会被替换，并会有一个
``struct sk_psock`` 挂接到它上面。此外，这个 sk_psock 会继承挂接到该映射上的
程序。)

A sock object may be in multiple maps, but can only inherit a single
parse or verdict program. If adding a sock object to a map would result
in having multiple parser programs the update will return an EBUSY error.
(一个 sock 对象可以位于多个映射中，但只能继承单个解析或裁定程序。如果向映射
添加 sock 对象会导致出现多个解析器程序，更新将返回 EBUSY 错误。)

The supported programs to attach to these maps are:
(可挂接到这些映射的程序如下：)

.. code-block:: c

	struct sk_psock_progs {
		struct bpf_prog *msg_parser;
		struct bpf_prog *stream_parser;
		struct bpf_prog *stream_verdict;
		struct bpf_prog	*skb_verdict;
	};

.. note::
    Users are not allowed to attach ``stream_verdict`` and ``skb_verdict``
    programs to the same map.
    (不允许用户把 ``stream_verdict`` 和 ``skb_verdict`` 程序挂接到同一个
    映射上。)

The attach types for the map programs are:
(映射程序的挂接类型如下：)

- ``msg_parser`` program - ``BPF_SK_MSG_VERDICT``.
  ( ``msg_parser`` 程序对应 ``BPF_SK_MSG_VERDICT`` 。)
- ``stream_parser`` program - ``BPF_SK_SKB_STREAM_PARSER``.
  ( ``stream_parser`` 程序对应 ``BPF_SK_SKB_STREAM_PARSER`` 。)
- ``stream_verdict`` program - ``BPF_SK_SKB_STREAM_VERDICT``.
  ( ``stream_verdict`` 程序对应 ``BPF_SK_SKB_STREAM_VERDICT`` 。)
- ``skb_verdict`` program - ``BPF_SK_SKB_VERDICT``.
  ( ``skb_verdict`` 程序对应 ``BPF_SK_SKB_VERDICT`` 。)

There are additional helpers available to use with the parser and verdict
programs: ``bpf_msg_apply_bytes()`` and ``bpf_msg_cork_bytes()``. With
``bpf_msg_apply_bytes()`` BPF programs can tell the infrastructure how many
bytes the given verdict should apply to. The helper ``bpf_msg_cork_bytes()``
handles a different case where a BPF program cannot reach a verdict on a msg
until it receives more bytes AND the program doesn't want to forward the packet
until it is known to be good.
(还有一些辅助函数可与解析器和裁定程序配合使用： ``bpf_msg_apply_bytes()`` 与
``bpf_msg_cork_bytes()``。通过 ``bpf_msg_apply_bytes()``，BPF 程序可以告诉
基础设施给定的裁定适用于多少字节。辅助函数 ``bpf_msg_cork_bytes()`` 处理另一
种情形：BPF 程序在收到更多字节之前无法对 msg 得出裁定，并且程序在确认数据
无误之前不想转发数据包。)

Finally, the helpers ``bpf_msg_pull_data()`` and ``bpf_msg_push_data()`` are
available to ``BPF_PROG_TYPE_SK_MSG`` BPF programs to pull in data and set the
start and end pointers to given values or to add metadata to the ``struct
sk_msg_buff *msg``.
(最后，辅助函数 ``bpf_msg_pull_data()`` 与 ``bpf_msg_push_data()`` 可供
``BPF_PROG_TYPE_SK_MSG`` 类型的 BPF 程序使用，用于拉取数据并把起始和结束
指针设置为给定值，或向 ``struct sk_msg_buff *msg`` 添加元数据。)

All these helpers will be described in more detail below.
(下面会更详细地描述所有这些辅助函数。)

Usage(用法)
===========
Kernel BPF(内核 BPF)
--------------------
bpf_msg_redirect_map()
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

	long bpf_msg_redirect_map(struct sk_msg_buff *msg, struct bpf_map *map, u32 key, u64 flags)

This helper is used in programs implementing policies at the socket level. If
the message ``msg`` is allowed to pass (i.e., if the verdict BPF program
returns ``SK_PASS``), redirect it to the socket referenced by ``map`` (of type
``BPF_MAP_TYPE_SOCKMAP``) at index ``key``. Both ingress and egress interfaces
can be used for redirection. The ``BPF_F_INGRESS`` value in ``flags`` is used
to select the ingress path otherwise the egress path is selected. This is the
only flag supported for now.

Returns ``SK_PASS`` on success, or ``SK_DROP`` on error.
(该辅助函数用于在套接字层面实现策略的程序中。如果消息 ``msg`` 被允许通过
(即裁定 BPF 程序返回 ``SK_PASS`` )，则把它重定向到 ``map`` (类型为
``BPF_MAP_TYPE_SOCKMAP``) 中索引 ``key`` 所引用的套接字。入口和出口接口都
可用于重定向。 ``flags`` 中的 ``BPF_F_INGRESS`` 值用于选择入口路径，否则选择
出口路径。这是目前唯一支持的标志。

(成功时返回 ``SK_PASS`` ，出错时返回 ``SK_DROP`` 。)

bpf_sk_redirect_map()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_sk_redirect_map(struct sk_buff *skb, struct bpf_map *map, u32 key u64 flags)

Redirect the packet to the socket referenced by ``map`` (of type
``BPF_MAP_TYPE_SOCKMAP``) at index ``key``. Both ingress and egress interfaces
can be used for redirection. The ``BPF_F_INGRESS`` value in ``flags`` is used
to select the ingress path otherwise the egress path is selected. This is the
only flag supported for now.

Returns ``SK_PASS`` on success, or ``SK_DROP`` on error.
(把数据包重定向到 ``map`` (类型为 ``BPF_MAP_TYPE_SOCKMAP``) 中索引 ``key``
所引用的套接字。入口和出口接口都可用于重定向。 ``flags`` 中的
``BPF_F_INGRESS`` 值用于选择入口路径，否则选择出口路径。这是目前唯一支持的
标志。

(成功时返回 ``SK_PASS`` ，出错时返回 ``SK_DROP`` 。)

bpf_map_lookup_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    void *bpf_map_lookup_elem(struct bpf_map *map, const void *key)

socket entries of type ``struct sock *`` can be retrieved using the
``bpf_map_lookup_elem()`` helper.
(可以使用 ``bpf_map_lookup_elem()`` 辅助函数检索 ``struct sock *`` 类型的
套接字表项。)

bpf_sock_map_update()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_sock_map_update(struct bpf_sock_ops *skops, struct bpf_map *map, void *key, u64 flags)

Add an entry to, or update a ``map`` referencing sockets. The ``skops`` is used
as a new value for the entry associated to ``key``. The ``flags`` argument can
be one of the following:
(向引用套接字的 ``map`` 添加表项或更新它。 ``skops`` 用作与 ``key`` 关联的表项
的新值。 ``flags`` 参数可以是以下之一：)

- ``BPF_ANY``: Create a new element or update an existing element.
  (BPF_ANY：创建新元素或更新现有元素。)
- ``BPF_NOEXIST``: Create a new element only if it did not exist.
  (BPF_NOEXIST：仅当元素不存在时才创建新元素。)
- ``BPF_EXIST``: Update an existing element.
  (BPF_EXIST：更新现有元素。)

If the ``map`` has BPF programs (parser and verdict), those will be inherited
by the socket being added. If the socket is already attached to BPF programs,
this results in an error.

Returns 0 on success, or a negative error in case of failure.
(如果 ``map`` 拥有 BPF 程序 (解析器和裁定程序)，这些程序将被被添加的套接字
继承。如果该套接字已挂接了 BPF 程序，则会导致错误。

(成功时返回 0，失败时返回负的错误码。)

bpf_sock_hash_update()
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_sock_hash_update(struct bpf_sock_ops *skops, struct bpf_map *map, void *key, u64 flags)

Add an entry to, or update a sockhash ``map`` referencing sockets. The ``skops``
is used as a new value for the entry associated to ``key``.
(向引用套接字的 sockhash ``map`` 添加表项或更新它。 ``skops`` 用作与 ``key``
关联的表项的新值。)

The ``flags`` argument can be one of the following:
( ``flags`` 参数可以是以下之一：)

- ``BPF_ANY``: Create a new element or update an existing element.
  (BPF_ANY：创建新元素或更新现有元素。)
- ``BPF_NOEXIST``: Create a new element only if it did not exist.
  (BPF_NOEXIST：仅当元素不存在时才创建新元素。)
- ``BPF_EXIST``: Update an existing element.
  (BPF_EXIST：更新现有元素。)

If the ``map`` has BPF programs (parser and verdict), those will be inherited
by the socket being added. If the socket is already attached to BPF programs,
this results in an error.

Returns 0 on success, or a negative error in case of failure.
(如果 ``map`` 拥有 BPF 程序 (解析器和裁定程序)，这些程序将被被添加的套接字
继承。如果该套接字已挂接了 BPF 程序，则会导致错误。

(成功时返回 0，失败时返回负的错误码。)

bpf_msg_redirect_hash()
^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_msg_redirect_hash(struct sk_msg_buff *msg, struct bpf_map *map, void *key, u64 flags)

This helper is used in programs implementing policies at the socket level. If
the message ``msg`` is allowed to pass (i.e., if the verdict BPF program returns
``SK_PASS``), redirect it to the socket referenced by ``map`` (of type
``BPF_MAP_TYPE_SOCKHASH``) using hash ``key``. Both ingress and egress
interfaces can be used for redirection. The ``BPF_F_INGRESS`` value in
``flags`` is used to select the ingress path otherwise the egress path is
selected. This is the only flag supported for now.

Returns ``SK_PASS`` on success, or ``SK_DROP`` on error.
(该辅助函数用于在套接字层面实现策略的程序中。如果消息 ``msg`` 被允许通过
(即裁定 BPF 程序返回 ``SK_PASS`` )，则使用哈希 ``key`` 把它重定向到 ``map``
(类型为 ``BPF_MAP_TYPE_SOCKHASH``) 所引用的套接字。入口和出口接口都可用于
重定向。 ``flags`` 中的 ``BPF_F_INGRESS`` 值用于选择入口路径，否则选择出口
路径。这是目前唯一支持的标志。

(成功时返回 ``SK_PASS`` ，出错时返回 ``SK_DROP`` 。)

bpf_sk_redirect_hash()
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_sk_redirect_hash(struct sk_buff *skb, struct bpf_map *map, void *key, u64 flags)

This helper is used in programs implementing policies at the skb socket level.
If the sk_buff ``skb`` is allowed to pass (i.e., if the verdict BPF program
returns ``SK_PASS``), redirect it to the socket referenced by ``map`` (of type
``BPF_MAP_TYPE_SOCKHASH``) using hash ``key``. Both ingress and egress
interfaces can be used for redirection. The ``BPF_F_INGRESS`` value in
``flags`` is used to select the ingress path otherwise the egress path is
selected. This is the only flag supported for now.

Returns ``SK_PASS`` on success, or ``SK_DROP`` on error.
(该辅助函数用于在 skb 套接字层面实现策略的程序中。如果 sk_buff ``skb`` 被允许
通过 (即裁定 BPF 程序返回 ``SK_PASS`` )，则使用哈希 ``key`` 把它重定向到
``map`` (类型为 ``BPF_MAP_TYPE_SOCKHASH``) 所引用的套接字。入口和出口接口都
可用于重定向。 ``flags`` 中的 ``BPF_F_INGRESS`` 值用于选择入口路径，否则选择
出口路径。这是目前唯一支持的标志。

(成功时返回 ``SK_PASS`` ，出错时返回 ``SK_DROP`` 。)

bpf_msg_apply_bytes()
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_msg_apply_bytes(struct sk_msg_buff *msg, u32 bytes)

For socket policies, apply the verdict of the BPF program to the next (number
of ``bytes``) of message ``msg``. For example, this helper can be used in the
following cases:
(对于套接字策略，把 BPF 程序的裁定应用到消息 ``msg`` 的接下来的 ( ``bytes``
个) 字节上。例如，该辅助函数可用于以下情形：)

- A single ``sendmsg()`` or ``sendfile()`` system call contains multiple
  logical messages that the BPF program is supposed to read and for which it
  should apply a verdict.
  (单个 ``sendmsg()`` 或 ``sendfile()`` 系统调用包含多条逻辑消息，BPF 程序
  应当读取它们并对其给出裁定。)
- A BPF program only cares to read the first ``bytes`` of a ``msg``. If the
  message has a large payload, then setting up and calling the BPF program
  repeatedly for all bytes, even though the verdict is already known, would
  create unnecessary overhead.
  (BPF 程序只关心读取 ``msg`` 的前 ``bytes`` 字节。如果消息的载荷很大，那么
  尽管裁定已经可知，却要对所有字节反复设置并调用 BPF 程序，会产生不必要的
  开销。)

Returns 0
(返回 0。)

bpf_msg_cork_bytes()
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_msg_cork_bytes(struct sk_msg_buff *msg, u32 bytes)

For socket policies, prevent the execution of the verdict BPF program for
message ``msg`` until the number of ``bytes`` have been accumulated.
(对于套接字策略，在累积到 ``bytes`` 字节之前，阻止对消息 ``msg`` 执行裁定
BPF 程序。)

This can be used when one needs a specific number of bytes before a verdict can
be assigned, even if the data spans multiple ``sendmsg()`` or ``sendfile()``
calls.
(当需要先凑齐特定数量的字节才能给出裁定，即使数据跨越多个 ``sendmsg()`` 或
``sendfile()`` 调用时，可以使用它。)

Returns 0
(返回 0。)

bpf_msg_pull_data()
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_msg_pull_data(struct sk_msg_buff *msg, u32 start, u32 end, u64 flags)

For socket policies, pull in non-linear data from user space for ``msg`` and set
pointers ``msg->data`` and ``msg->data_end`` to ``start`` and ``end`` bytes
offsets into ``msg``, respectively.
(对于套接字策略，为 ``msg`` 从用户空间拉取非线性数据，并把指针 ``msg->data``
和 ``msg->data_end`` 分别设置为指向 ``msg`` 中 ``start`` 和 ``end`` 字节
偏移处。)

If a program of type ``BPF_PROG_TYPE_SK_MSG`` is run on a ``msg`` it can only
parse data that the (``data``, ``data_end``) pointers have already consumed.
For ``sendmsg()`` hooks this is likely the first scatterlist element. But for
calls relying on MSG_SPLICE_PAGES (e.g., ``sendfile()``) this will be the
range (**0**, **0**) because the data is shared with user space and by default
the objective is to avoid allowing user space to modify data while (or after)
BPF verdict is being decided. This helper can be used to pull in data and to
set the start and end pointers to given values. Data will be copied if
necessary (i.e., if data was not linear and if start and end pointers do not
point to the same chunk).
(如果在 ``msg`` 上运行 ``BPF_PROG_TYPE_SK_MSG`` 类型的程序，它只能解析
(``data`` 、 ``data_end``) 指针已经消费过的数据。对于 ``sendmsg()`` 钩子，
这通常是第一个散布列表 (scatterlist) 元素。但对于依赖 MSG_SPLICE_PAGES 的
调用 (例如 ``sendfile()`` )，这将是 (**0**, **0**) 范围，因为数据与用户空间
共享，默认目标是避免在 BPF 裁定决策期间 (或之后) 允许用户空间修改数据。该
辅助函数可用于拉取数据并把起始和结束指针设置为给定值。如有必要，数据将被
复制 (即数据不是线性的，且起始和结束指针不指向同一个块)。)

A call to this helper is susceptible to change the underlying packet buffer.
Therefore, at load time, all checks on pointers previously done by the verifier
are invalidated and must be performed again, if the helper is used in
combination with direct packet access.
(对此辅助函数的调用可能改变底层数据包缓冲区。因此，在加载时，如果该辅助函数
与直接数据包访问结合使用，验证器之前对指针所做的所有检查都将失效，必须重新
执行。)

All values for ``flags`` are reserved for future usage, and must be left at
zero.
( ``flags`` 的所有取值都保留供将来使用，必须保持为零。)

Returns 0 on success, or a negative error in case of failure.
(成功时返回 0，失败时返回负的错误码。)

bpf_map_lookup_elem()
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: c

	void *bpf_map_lookup_elem(struct bpf_map *map, const void *key)

Look up a socket entry in the sockmap or sockhash map.
(在 sockmap 或 sockhash 映射中查找套接字表项。)

Returns the socket entry associated to ``key``, or NULL if no entry was found.
(返回与 ``key`` 关联的套接字表项，如果未找到表项则返回 NULL。)

bpf_map_update_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

	long bpf_map_update_elem(struct bpf_map *map, const void *key, const void *value, u64 flags)

Add or update a socket entry in a sockmap or sockhash.
(在 sockmap 或 sockhash 中添加或更新套接字表项。)

The flags argument can be one of the following:
( flags 参数可以是以下之一：)

- BPF_ANY: Create a new element or update an existing element.
  (BPF_ANY：创建新元素或更新现有元素。)
- BPF_NOEXIST: Create a new element only if it did not exist.
  (BPF_NOEXIST：仅当元素不存在时才创建新元素。)
- BPF_EXIST: Update an existing element.
  (BPF_EXIST：更新现有元素。)

Returns 0 on success, or a negative error in case of failure.
(成功时返回 0，失败时返回负的错误码。)

bpf_map_delete_elem()
^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_map_delete_elem(struct bpf_map *map, const void *key)

Delete a socket entry from a sockmap or a sockhash.
(从 sockmap 或 sockhash 中删除套接字表项。)

Returns	0 on success, or a negative error in case of failure.
(成功时返回 0，失败时返回负的错误码。)

User space(用户空间)
--------------------
bpf_map_update_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

	int bpf_map_update_elem(int fd, const void *key, const void *value, __u64 flags)

Sockmap entries can be added or updated using the ``bpf_map_update_elem()``
function. The ``key`` parameter is the index value of the sockmap array. And the
``value`` parameter is the FD value of that socket.
(可以使用 ``bpf_map_update_elem()`` 函数添加或更新 sockmap 表项。 ``key``
参数是 sockmap 数组的索引值。 ``value`` 参数是该套接字的 FD 值。)

Under the hood, the sockmap update function uses the socket FD value to
retrieve the associated socket and its attached psock.
(在底层，sockmap 更新函数使用套接字 FD 值来检索关联的套接字及其挂接的
psock。)

The flags argument can be one of the following:
( flags 参数可以是以下之一：)

- BPF_ANY: Create a new element or update an existing element.
  (BPF_ANY：创建新元素或更新现有元素。)
- BPF_NOEXIST: Create a new element only if it did not exist.
  (BPF_NOEXIST：仅当元素不存在时才创建新元素。)
- BPF_EXIST: Update an existing element.
  (BPF_EXIST：更新现有元素。)

bpf_map_lookup_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    int bpf_map_lookup_elem(int fd, const void *key, void *value)

Sockmap entries can be retrieved using the ``bpf_map_lookup_elem()`` function.
(可以使用 ``bpf_map_lookup_elem()`` 函数检索 sockmap 表项。)

.. note::
	The entry returned is a socket cookie rather than a socket itself.
	(返回的表项是 socket cookie，而不是套接字本身。)

bpf_map_delete_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    int bpf_map_delete_elem(int fd, const void *key)

Sockmap entries can be deleted using the ``bpf_map_delete_elem()``
function.
(可以使用 ``bpf_map_delete_elem()`` 函数删除 sockmap 表项。)

Returns 0 on success, or negative error in case of failure.
(成功时返回 0，失败时返回负的错误码。)

Examples(示例)
==============

Kernel BPF(内核 BPF)
--------------------
Several examples of the use of sockmap APIs can be found in:
(sockmap API 的使用示例可在以下位置找到：)

- `tools/testing/selftests/bpf/progs/test_sockmap_kern.h`_
- `tools/testing/selftests/bpf/progs/sockmap_parse_prog.c`_
- `tools/testing/selftests/bpf/progs/sockmap_verdict_prog.c`_
- `tools/testing/selftests/bpf/progs/test_sockmap_listen.c`_
- `tools/testing/selftests/bpf/progs/test_sockmap_update.c`_

The following code snippet shows how to declare a sockmap.
(以下代码片段展示如何声明一个 sockmap。)

.. code-block:: c

	struct {
		__uint(type, BPF_MAP_TYPE_SOCKMAP);
		__uint(max_entries, 1);
		__type(key, __u32);
		__type(value, __u64);
	} sock_map_rx SEC(".maps");

The following code snippet shows a sample parser program.
(以下代码片段展示一个解析器程序示例。)

.. code-block:: c

	SEC("sk_skb/stream_parser")
	int bpf_prog_parser(struct __sk_buff *skb)
	{
		return skb->len;
	}

The following code snippet shows a simple verdict program that interacts with a
sockmap to redirect traffic to another socket based on the local port.
(以下代码片段展示一个简单的裁定程序，它与 sockmap 交互，根据本地端口把流量
重定向到另一个套接字。)

.. code-block:: c

	SEC("sk_skb/stream_verdict")
	int bpf_prog_verdict(struct __sk_buff *skb)
	{
		__u32 lport = skb->local_port;
		__u32 idx = 0;

		if (lport == 10000)
			return bpf_sk_redirect_map(skb, &sock_map_rx, idx, 0);

		return SK_PASS;
	}

The following code snippet shows how to declare a sockhash map.
(以下代码片段展示如何声明一个 sockhash 映射。)

.. code-block:: c

	struct socket_key {
		__u32 src_ip;
		__u32 dst_ip;
		__u32 src_port;
		__u32 dst_port;
	};

	struct {
		__uint(type, BPF_MAP_TYPE_SOCKHASH);
		__uint(max_entries, 1);
		__type(key, struct socket_key);
		__type(value, __u64);
	} sock_hash_rx SEC(".maps");

The following code snippet shows a simple verdict program that interacts with a
sockhash to redirect traffic to another socket based on a hash of some of the
skb parameters.
(以下代码片段展示一个简单的裁定程序，它与 sockhash 交互，根据部分 skb 参数的
哈希把流量重定向到另一个套接字。)

.. code-block:: c

	static inline
	void extract_socket_key(struct __sk_buff *skb, struct socket_key *key)
	{
		key->src_ip = skb->remote_ip4;
		key->dst_ip = skb->local_ip4;
		key->src_port = skb->remote_port >> 16;
		key->dst_port = (bpf_htonl(skb->local_port)) >> 16;
	}

	SEC("sk_skb/stream_verdict")
	int bpf_prog_verdict(struct __sk_buff *skb)
	{
		struct socket_key key;

		extract_socket_key(skb, &key);

		return bpf_sk_redirect_hash(skb, &sock_hash_rx, &key, 0);
	}

User space(用户空间)
--------------------
Several examples of the use of sockmap APIs can be found in:
(sockmap API 的使用示例可在以下位置找到：)

- `tools/testing/selftests/bpf/prog_tests/sockmap_basic.c`_
- `tools/testing/selftests/bpf/test_sockmap.c`_
- `tools/testing/selftests/bpf/test_maps.c`_

The following code sample shows how to create a sockmap, attach a parser and
verdict program, as well as add a socket entry.
(以下代码示例展示如何创建 sockmap、挂接解析器和裁定程序，以及添加套接字
表项。)

.. code-block:: c

	int create_sample_sockmap(int sock, int parse_prog_fd, int verdict_prog_fd)
	{
		int index = 0;
		int map, err;

		map = bpf_map_create(BPF_MAP_TYPE_SOCKMAP, NULL, sizeof(int), sizeof(int), 1, NULL);
		if (map < 0) {
			fprintf(stderr, "Failed to create sockmap: %s\n", strerror(errno));
			return -1;
		}

		err = bpf_prog_attach(parse_prog_fd, map, BPF_SK_SKB_STREAM_PARSER, 0);
		if (err){
			fprintf(stderr, "Failed to attach_parser_prog_to_map: %s\n", strerror(errno));
			goto out;
		}

		err = bpf_prog_attach(verdict_prog_fd, map, BPF_SK_SKB_STREAM_VERDICT, 0);
		if (err){
			fprintf(stderr, "Failed to attach_verdict_prog_to_map: %s\n", strerror(errno));
			goto out;
		}

		err = bpf_map_update_elem(map, &index, &sock, BPF_NOEXIST);
		if (err) {
			fprintf(stderr, "Failed to update sockmap: %s\n", strerror(errno));
			goto out;
		}

	out:
		close(map);
		return err;
	}

References(参考资料)
====================

- https://github.com/jrfastab/linux-kernel-xdp/commit/c89fd73cb9d2d7f3c716c3e00836f07b1aeb261f
- https://lwn.net/Articles/731133/
- http://vger.kernel.org/lpc_net2018_talks/ktls_bpf_paper.pdf
- https://lwn.net/Articles/748628/
- https://lore.kernel.org/bpf/20200218171023.844439-7-jakub@cloudflare.com/

.. _`tools/testing/selftests/bpf/progs/test_sockmap_kern.h`: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/progs/test_sockmap_kern.h
.. _`tools/testing/selftests/bpf/progs/sockmap_parse_prog.c`: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/progs/sockmap_parse_prog.c
.. _`tools/testing/selftests/bpf/progs/sockmap_verdict_prog.c`: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/progs/sockmap_verdict_prog.c
.. _`tools/testing/selftests/bpf/prog_tests/sockmap_basic.c`: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/prog_tests/sockmap_basic.c
.. _`tools/testing/selftests/bpf/test_sockmap.c`: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/test_sockmap.c
.. _`tools/testing/selftests/bpf/test_maps.c`: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/test_maps.c
.. _`tools/testing/selftests/bpf/progs/test_sockmap_listen.c`: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/progs/test_sockmap_listen.c
.. _`tools/testing/selftests/bpf/progs/test_sockmap_update.c`: https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/progs/test_sockmap_update.c
