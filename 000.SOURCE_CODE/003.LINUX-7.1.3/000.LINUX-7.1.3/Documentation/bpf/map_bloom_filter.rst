.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

=========================================
BPF_MAP_TYPE_BLOOM_FILTER(布隆过滤器映射)
=========================================

.. note::
   - ``BPF_MAP_TYPE_BLOOM_FILTER`` was introduced in kernel version 5.16
     (``BPF_MAP_TYPE_BLOOM_FILTER`` 在内核版本 5.16 中引入)

``BPF_MAP_TYPE_BLOOM_FILTER`` provides a BPF bloom filter map. Bloom
filters are a space-efficient probabilistic data structure used to
quickly test whether an element exists in a set. In a bloom filter,
false positives are possible whereas false negatives are not.
( ``BPF_MAP_TYPE_BLOOM_FILTER`` 提供了一种 BPF 布隆过滤器 (bloom filter) 映射。布隆过滤器是一种节省空间的概率数据结构，用于快速测试某个元素是否存在于集合中。在布隆过滤器中，误报 (false positive) 是可能的，而漏报 (false negative) 则不可能发生。)

The bloom filter map does not have keys, only values. When the bloom
filter map is created, it must be created with a ``key_size`` of 0.  The
bloom filter map supports two operations:
(布隆过滤器映射没有键，只有值。创建布隆过滤器映射时，必须以 ``key_size`` 为 0 来创建。布隆过滤器映射支持两种操作：)

- push: adding an element to the map
  (push: 将一个元素添加到映射中)
- peek: determining whether an element is present in the map
  (peek: 判断映射中是否存在某个元素)

BPF programs must use ``bpf_map_push_elem`` to add an element to the
bloom filter map and ``bpf_map_peek_elem`` to query the map. These
operations are exposed to userspace applications using the existing
``bpf`` syscall in the following way:
(BPF 程序必须使用 ``bpf_map_push_elem`` 向布隆过滤器映射添加元素，并使用 ``bpf_map_peek_elem`` 查询该映射。这些操作通过现有的 ``bpf`` 系统调用以下列方式暴露给用户空间应用程序：)

- ``BPF_MAP_UPDATE_ELEM`` -> push
  ( ``BPF_MAP_UPDATE_ELEM`` -> push，即添加元素)
- ``BPF_MAP_LOOKUP_ELEM`` -> peek
  ( ``BPF_MAP_LOOKUP_ELEM`` -> peek，即查询元素)

The ``max_entries`` size that is specified at map creation time is used
to approximate a reasonable bitmap size for the bloom filter, and is not
otherwise strictly enforced. If the user wishes to insert more entries
into the bloom filter than ``max_entries``, this may lead to a higher
false positive rate.
(在映射创建时指定的 ``max_entries`` 大小用于近似估计布隆过滤器合理的位图大小，除此之外并不会被严格执行。如果用户希望向布隆过滤器插入超过 ``max_entries`` 的条目数，这可能导致更高的误报率。)

The number of hashes to use for the bloom filter is configurable using
the lower 4 bits of ``map_extra`` in ``union bpf_attr`` at map creation
time. If no number is specified, the default used will be 5 hash
functions. In general, using more hashes decreases both the false
positive rate and the speed of a lookup.
(布隆过滤器使用的哈希函数数量是可配置的：在映射创建时通过 ``union bpf_attr`` 中 ``map_extra`` 的低 4 位设置。如果未指定数字，默认将使用 5 个哈希函数。一般来说，使用更多的哈希函数会同时降低误报率和查找速度。)

It is not possible to delete elements from a bloom filter map. A bloom
filter map may be used as an inner map. The user is responsible for
synchronising concurrent updates and lookups to ensure no false negative
lookups occur.
(无法从布隆过滤器映射中删除元素。布隆过滤器映射可以用作内层映射 (inner map)。用户需要负责同步并发的更新和查找，以确保不发生漏报的查找。)

Usage(用法)
===========

Kernel BPF(内核 BPF)
--------------------

bpf_map_push_elem()
~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_push_elem(struct bpf_map *map, const void *value, u64 flags)

A ``value`` can be added to a bloom filter using the
``bpf_map_push_elem()`` helper. The ``flags`` parameter must be set to
``BPF_ANY`` when adding an entry to the bloom filter. This helper
returns ``0`` on success, or negative error in case of failure.
(可以使用 ``bpf_map_push_elem()`` 辅助函数向布隆过滤器添加一个 ``value`` 。向布隆过滤器添加条目时， ``flags`` 参数必须设置为 ``BPF_ANY`` 。该辅助函数成功时返回 ``0`` ，失败时返回负的错误码。)

bpf_map_peek_elem()
~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_peek_elem(struct bpf_map *map, void *value)

The ``bpf_map_peek_elem()`` helper is used to determine whether
``value`` is present in the bloom filter map. This helper returns ``0``
if ``value`` is probably present in the map, or ``-ENOENT`` if ``value``
is definitely not present in the map.
( ``bpf_map_peek_elem()`` 辅助函数用于判断 ``value`` 是否存在于布隆过滤器映射中。如果 ``value`` 可能存在于映射中，该辅助函数返回 ``0`` ；如果 ``value`` 肯定不存在于映射中，则返回 ``-ENOENT`` 。)

Userspace(用户空间)
-------------------

bpf_map_update_elem()
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_update_elem (int fd, const void *key, const void *value, __u64 flags)

A userspace program can add a ``value`` to a bloom filter using libbpf's
``bpf_map_update_elem`` function. The ``key`` parameter must be set to
``NULL`` and ``flags`` must be set to ``BPF_ANY``. Returns ``0`` on
success, or negative error in case of failure.
(用户空间程序可以使用 libbpf 的 ``bpf_map_update_elem`` 函数向布隆过滤器添加一个 ``value`` 。 ``key`` 参数必须设置为 ``NULL`` ， ``flags`` 必须设置为 ``BPF_ANY`` 。成功时返回 ``0`` ，失败时返回负的错误码。)

bpf_map_lookup_elem()
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_lookup_elem (int fd, const void *key, void *value)

A userspace program can determine the presence of ``value`` in a bloom
filter using libbpf's ``bpf_map_lookup_elem`` function. The ``key``
parameter must be set to ``NULL``. Returns ``0`` if ``value`` is
probably present in the map, or ``-ENOENT`` if ``value`` is definitely
not present in the map.
(用户空间程序可以使用 libbpf 的 ``bpf_map_lookup_elem`` 函数判断 ``value`` 是否存在于布隆过滤器中。 ``key`` 参数必须设置为 ``NULL`` 。如果 ``value`` 可能存在于映射中则返回 ``0`` ，如果 ``value`` 肯定不存在于映射中则返回 ``-ENOENT`` 。)

Examples(示例)
==============

Kernel BPF(内核 BPF)
--------------------

This snippet shows how to declare a bloom filter in a BPF program:
(此代码片段展示如何在 BPF 程序中声明一个布隆过滤器：)

.. code-block:: c

    struct {
            __uint(type, BPF_MAP_TYPE_BLOOM_FILTER);
            __type(value, __u32);
            __uint(max_entries, 1000);
            __uint(map_extra, 3);
    } bloom_filter SEC(".maps");

This snippet shows how to determine presence of a value in a bloom
filter in a BPF program:
(此代码片段展示如何在 BPF 程序中判断某个值是否存在于布隆过滤器中：)

.. code-block:: c

    void *lookup(__u32 key)
    {
            if (bpf_map_peek_elem(&bloom_filter, &key) == 0) {
                    /* Verify not a false positive and fetch an associated
                     * value using a secondary lookup, e.g. in a hash table
                     */
                    return bpf_map_lookup_elem(&hash_table, &key);
            }
            return 0;
    }

Userspace(用户空间)
-------------------

This snippet shows how to use libbpf to create a bloom filter map from
userspace:
(此代码片段展示如何从用户空间使用 libbpf 创建一个布隆过滤器映射：)

.. code-block:: c

    int create_bloom()
    {
            LIBBPF_OPTS(bpf_map_create_opts, opts,
                        .map_extra = 3);             /* number of hashes */

            return bpf_map_create(BPF_MAP_TYPE_BLOOM_FILTER,
                                  "ipv6_bloom",      /* name */
                                  0,                 /* key size, must be zero */
                                  sizeof(ipv6_addr), /* value size */
                                  10000,             /* max entries */
                                  &opts);            /* create options */
    }

This snippet shows how to add an element to a bloom filter from
userspace:
(此代码片段展示如何从用户空间向布隆过滤器添加一个元素：)

.. code-block:: c

    int add_element(struct bpf_map *bloom_map, __u32 value)
    {
            int bloom_fd = bpf_map__fd(bloom_map);
            return bpf_map_update_elem(bloom_fd, NULL, &value, BPF_ANY);
    }

References(参考)
================

https://lwn.net/ml/bpf/20210831225005.2762202-1-joannekoong@fb.com/
