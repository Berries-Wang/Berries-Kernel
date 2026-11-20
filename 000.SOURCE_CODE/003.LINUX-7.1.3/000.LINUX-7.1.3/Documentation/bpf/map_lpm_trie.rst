.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

=============================================
BPF_MAP_TYPE_LPM_TRIE(最长前缀匹配 Trie 映射)
=============================================

.. note::
   - ``BPF_MAP_TYPE_LPM_TRIE`` was introduced in kernel version 4.11
   (``BPF_MAP_TYPE_LPM_TRIE`` 在内核版本 4.11 中引入)

``BPF_MAP_TYPE_LPM_TRIE`` provides a longest prefix match algorithm that
can be used to match IP addresses to a stored set of prefixes.
Internally, data is stored in an unbalanced trie of nodes that uses
``prefixlen,data`` pairs as its keys. The ``data`` is interpreted in
network byte order, i.e. big endian, so ``data[0]`` stores the most
significant byte.
(``BPF_MAP_TYPE_LPM_TRIE`` 提供最长前缀匹配算法，可用于将 IP 地址匹配到已存储的前缀集合。在内部，数据存储在一棵由节点组成的非平衡 trie 中，以 ``prefixlen,data`` 对作为键。``data`` 按网络字节序(即大端序)解释，因此 ``data[0]`` 存储最高有效字节。)

LPM tries may be created with a maximum prefix length that is a multiple
of 8, in the range from 8 to 2048. The key used for lookup and update
operations is a ``struct bpf_lpm_trie_key_u8``, extended by
``max_prefixlen/8`` bytes.
(创建 LPM trie 时，最大前缀长度必须是 8 的倍数，范围为 8 到 2048。查找与更新操作使用的键是 ``struct bpf_lpm_trie_key_u8``，并由 ``max_prefixlen/8`` 个字节扩展而成。)

- For IPv4 addresses the data length is 4 bytes
  (对于 IPv4 地址，数据长度为 4 字节)
- For IPv6 addresses the data length is 16 bytes
  (对于 IPv6 地址，数据长度为 16 字节)

The value type stored in the LPM trie can be any user defined type.
(存储在 LPM trie 中的值类型可以是任意用户自定义类型。)

.. note::
   When creating a map of type ``BPF_MAP_TYPE_LPM_TRIE`` you must set the
   ``BPF_F_NO_PREALLOC`` flag.
   (创建 ``BPF_MAP_TYPE_LPM_TRIE`` 类型的映射时，必须设置 ``BPF_F_NO_PREALLOC`` 标志。)

Usage(用法)
===========

Kernel BPF(内核侧 BPF)
----------------------

bpf_map_lookup_elem()(查找元素)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void *bpf_map_lookup_elem(struct bpf_map *map, const void *key)

The longest prefix entry for a given data value can be found using the
``bpf_map_lookup_elem()`` helper. This helper returns a pointer to the
value associated with the longest matching ``key``, or ``NULL`` if no
entry was found.
(可以使用 ``bpf_map_lookup_elem()`` 辅助函数找到给定数据值的最长前缀条目。该辅助函数返回与最长匹配 ``key`` 关联的值的指针；若未找到条目则返回 ``NULL`` 。)

The ``key`` should have ``prefixlen`` set to ``max_prefixlen`` when
performing longest prefix lookups. For example, when searching for the
longest prefix match for an IPv4 address, ``prefixlen`` should be set to
``32``.
(执行最长前缀查找时，``key`` 的 ``prefixlen`` 应设为 ``max_prefixlen``。例如，搜索某个 IPv4 地址的最长前缀匹配时，``prefixlen`` 应设为 ``32`` 。)

bpf_map_update_elem()(更新元素)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_update_elem(struct bpf_map *map, const void *key, const void *value, u64 flags)

Prefix entries can be added or updated using the ``bpf_map_update_elem()``
helper. This helper replaces existing elements atomically.
(可以使用 ``bpf_map_update_elem()`` 辅助函数添加或更新前缀条目。该辅助函数以原子方式替换已有元素。)

``bpf_map_update_elem()`` returns ``0`` on success, or negative error in
case of failure.
(``bpf_map_update_elem()`` 成功时返回 ``0``，失败时返回负的错误码。)

 .. note::
    The flags parameter must be one of BPF_ANY, BPF_NOEXIST or BPF_EXIST,
    but the value is ignored, giving BPF_ANY semantics.
    (flags 参数必须是 BPF_ANY、BPF_NOEXIST 或 BPF_EXIST 之一，但该值会被忽略，实际表现为 BPF_ANY 语义。)

bpf_map_delete_elem()(删除元素)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_delete_elem(struct bpf_map *map, const void *key)

Prefix entries can be deleted using the ``bpf_map_delete_elem()``
helper. This helper will return 0 on success, or negative error in case
of failure.
(可以使用 ``bpf_map_delete_elem()`` 辅助函数删除前缀条目。该辅助函数成功时返回 0，失败时返回负的错误码。)

Userspace(用户态)
-----------------

Access from userspace uses libbpf APIs with the same names as above, with
the map identified by ``fd``.
(用户态访问使用与上文同名 libbpf API，通过 ``fd`` 标识映射。)

bpf_map_get_next_key()(获取下一个键)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_get_next_key (int fd, const void *cur_key, void *next_key)

A userspace program can iterate through the entries in an LPM trie using
libbpf's ``bpf_map_get_next_key()`` function. The first key can be
fetched by calling ``bpf_map_get_next_key()`` with ``cur_key`` set to
``NULL``. Subsequent calls will fetch the next key that follows the
current key. ``bpf_map_get_next_key()`` returns ``0`` on success,
``-ENOENT`` if ``cur_key`` is the last key in the trie, or negative
error in case of failure.
(用户态程序可以使用 libbpf 的 ``bpf_map_get_next_key()`` 函数遍历 LPM trie 中的条目。将 ``cur_key`` 设为 ``NULL`` 调用 ``bpf_map_get_next_key()`` 可获取第一个键。后续调用会获取紧跟当前键之后的下一个键。``bpf_map_get_next_key()`` 成功时返回 ``0``；若 ``cur_key`` 是 trie 中最后一个键则返回 ``-ENOENT``；失败时返回负的错误码。)

``bpf_map_get_next_key()`` will iterate through the LPM trie elements
from leftmost leaf first. This means that iteration will return more
specific keys before less specific ones.
(``bpf_map_get_next_key()`` 从最左边的叶子开始遍历 LPM trie 元素。这意味着迭代会先返回更具体(更长)的键，再返回不那么具体的键。)

Examples(示例)
==============

Please see ``tools/testing/selftests/bpf/test_lpm_map.c`` for examples
of LPM trie usage from userspace. The code snippets below demonstrate
API usage.
(用户态使用 LPM trie 的示例见 ``tools/testing/selftests/bpf/test_lpm_map.c``。下面的代码片段演示了 API 的用法。)

Kernel BPF(内核侧 BPF)
----------------------

The following BPF code snippet shows how to declare a new LPM trie for IPv4
address prefixes:
(下面的 BPF 代码片段展示如何为 IPv4 地址前缀声明一个新的 LPM trie：)

.. code-block:: c

    #include <linux/bpf.h>
    #include <bpf/bpf_helpers.h>

    struct ipv4_lpm_key {
            __u32 prefixlen;
            __u32 data;
    };

    struct {
            __uint(type, BPF_MAP_TYPE_LPM_TRIE);
            __type(key, struct ipv4_lpm_key);
            __type(value, __u32);
            __uint(map_flags, BPF_F_NO_PREALLOC);
            __uint(max_entries, 255);
    } ipv4_lpm_map SEC(".maps");

The following BPF code snippet shows how to lookup by IPv4 address:
(下面的 BPF 代码片段展示如何按 IPv4 地址进行查找：)

.. code-block:: c

    void *lookup(__u32 ipaddr)
    {
            struct ipv4_lpm_key key = {
                    .prefixlen = 32,
                    .data = ipaddr
            };

            return bpf_map_lookup_elem(&ipv4_lpm_map, &key);
    }

Userspace
---------

The following snippet shows how to insert an IPv4 prefix entry into an
LPM trie:
(下面的代码片段展示如何向 LPM trie 插入一个 IPv4 前缀条目：)

.. code-block:: c

    int add_prefix_entry(int lpm_fd, __u32 addr, __u32 prefixlen, struct value *value)
    {
            struct ipv4_lpm_key ipv4_key = {
                    .prefixlen = prefixlen,
                    .data = addr
            };
            return bpf_map_update_elem(lpm_fd, &ipv4_key, value, BPF_ANY);
    }

The following snippet shows a userspace program walking through the entries
of an LPM trie:
(下面的代码片段展示了一个遍历 LPM trie 条目的用户态程序：)


.. code-block:: c

    #include <bpf/libbpf.h>
    #include <bpf/bpf.h>

    void iterate_lpm_trie(int map_fd)
    {
            struct ipv4_lpm_key *cur_key = NULL;
            struct ipv4_lpm_key next_key;
            struct value value;
            int err;

            for (;;) {
                    err = bpf_map_get_next_key(map_fd, cur_key, &next_key);
                    if (err)
                            break;

                    bpf_map_lookup_elem(map_fd, &next_key, &value);

                    /* Use key and value here */

                    cur_key = &next_key;
            }
    }
