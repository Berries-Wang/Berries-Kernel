.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.
.. Copyright (C) 2022-2023 Isovalent, Inc.

========================================================================================
BPF_MAP_TYPE_HASH, with PERCPU and LRU Variants(BPF_MAP_TYPE_HASH 及其 PERCPU、LRU 变体)
========================================================================================

.. note::
   - ``BPF_MAP_TYPE_HASH`` was introduced in kernel version 3.19
     (``BPF_MAP_TYPE_HASH`` 在内核版本 3.19 中引入)
   - ``BPF_MAP_TYPE_PERCPU_HASH`` was introduced in version 4.6
     (``BPF_MAP_TYPE_PERCPU_HASH`` 在版本 4.6 中引入)
   - Both ``BPF_MAP_TYPE_LRU_HASH`` and ``BPF_MAP_TYPE_LRU_PERCPU_HASH``
     were introduced in version 4.10
     (``BPF_MAP_TYPE_LRU_HASH`` 与 ``BPF_MAP_TYPE_LRU_PERCPU_HASH`` 均在版本
     4.10 中引入)

``BPF_MAP_TYPE_HASH`` and ``BPF_MAP_TYPE_PERCPU_HASH`` provide general
purpose hash map storage. Both the key and the value can be structs,
allowing for composite keys and values.
(``BPF_MAP_TYPE_HASH`` 与 ``BPF_MAP_TYPE_PERCPU_HASH`` 提供通用的哈希 map
存储。键和值都可以是结构体，从而支持组合键与组合值。)

The kernel is responsible for allocating and freeing key/value pairs, up
to the max_entries limit that you specify. Hash maps use pre-allocation
of hash table elements by default. The ``BPF_F_NO_PREALLOC`` flag can be
used to disable pre-allocation when it is too memory expensive.
(内核负责分配和释放键/值对，上限是你指定的 max_entries。哈希 map 默认对哈希表
元素进行预分配。当预分配的内存代价过高时，可以使用 ``BPF_F_NO_PREALLOC`` 标志
禁用预分配。)

``BPF_MAP_TYPE_PERCPU_HASH`` provides a separate value slot per
CPU. The per-cpu values are stored internally in an array.
(``BPF_MAP_TYPE_PERCPU_HASH`` 为每个 CPU 提供一个独立的值槽。每 CPU 的值在内
部以数组形式存储。)

The ``BPF_MAP_TYPE_LRU_HASH`` and ``BPF_MAP_TYPE_LRU_PERCPU_HASH``
variants add LRU semantics to their respective hash tables. An LRU hash
will automatically evict the least recently used entries when the hash
table reaches capacity. An LRU hash maintains an internal LRU list that
is used to select elements for eviction. This internal LRU list is
shared across CPUs but it is possible to request a per CPU LRU list with
the ``BPF_F_NO_COMMON_LRU`` flag when calling ``bpf_map_create``.  The
following table outlines the properties of LRU maps depending on the a
map type and the flags used to create the map.
(``BPF_MAP_TYPE_LRU_HASH`` 与 ``BPF_MAP_TYPE_LRU_PERCPU_HASH`` 变体为各自的
哈希表增加了 LRU 语义。当哈希表达到容量上限时，LRU 哈希会自动驱逐最近最少使用
的条目。LRU 哈希维护一个用于挑选驱逐元素的内部 LRU 链表。该内部 LRU 链表在各
CPU 之间共享，但也可以在调用 ``bpf_map_create`` 时通过 ``BPF_F_NO_COMMON_LRU``
标志请求每 CPU 独立的 LRU 链表。下表概述了 LRU map 依据 map 类型及创建所用
标志而具有的不同性质。)

======================== ========================= ================================
Flag(标志)               ``BPF_MAP_TYPE_LRU_HASH`` ``BPF_MAP_TYPE_LRU_PERCPU_HASH``
======================== ========================= ================================
**BPF_F_NO_COMMON_LRU**  Per-CPU LRU, global map   Per-CPU LRU, per-cpu map
                         (每 CPU 的 LRU，全局映射)  (每 CPU 的 LRU，每 CPU 映射)
**!BPF_F_NO_COMMON_LRU** Global LRU, global map    Global LRU, per-cpu map
                         (全局 LRU，全局映射)       (全局 LRU，每 CPU 映射)
======================== ========================= ================================

Usage(用法)
===========

Kernel BPF(内核 BPF)
--------------------

bpf_map_update_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_update_elem(struct bpf_map *map, const void *key, const void *value, u64 flags)

Hash entries can be added or updated using the ``bpf_map_update_elem()``
helper. This helper replaces existing elements atomically. The ``flags``
parameter can be used to control the update behaviour:
(可以使用 ``bpf_map_update_elem()`` helper 添加或更新哈希条目。该 helper 以
原子方式替换已有元素。``flags`` 参数可用于控制更新行为：)

- ``BPF_ANY`` will create a new element or update an existing element
  (``BPF_ANY``：创建新元素，或更新已有元素)
- ``BPF_NOEXIST`` will create a new element only if one did not already
  exist
  (``BPF_NOEXIST``：仅当尚不存在时才创建新元素)
- ``BPF_EXIST`` will update an existing element
  (``BPF_EXIST``：更新已有元素)

``bpf_map_update_elem()`` returns 0 on success, or negative error in
case of failure.
(``bpf_map_update_elem()`` 成功时返回 0，失败时返回负的错误码。)

bpf_map_lookup_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void *bpf_map_lookup_elem(struct bpf_map *map, const void *key)

Hash entries can be retrieved using the ``bpf_map_lookup_elem()``
helper. This helper returns a pointer to the value associated with
``key``, or ``NULL`` if no entry was found.
(可以使用 ``bpf_map_lookup_elem()`` helper 检索哈希条目。该 helper 返回指向与
``key`` 关联的值的指针；未找到条目时返回 ``NULL``。)

bpf_map_delete_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_delete_elem(struct bpf_map *map, const void *key)

Hash entries can be deleted using the ``bpf_map_delete_elem()``
helper. This helper will return 0 on success, or negative error in case
of failure.
(可以使用 ``bpf_map_delete_elem()`` helper 删除哈希条目。该 helper 成功时返回
0，失败时返回负的错误码。)

Per CPU Hashes(每 CPU 哈希)
---------------------------

For ``BPF_MAP_TYPE_PERCPU_HASH`` and ``BPF_MAP_TYPE_LRU_PERCPU_HASH``
the ``bpf_map_update_elem()`` and ``bpf_map_lookup_elem()`` helpers
automatically access the hash slot for the current CPU.
(对 ``BPF_MAP_TYPE_PERCPU_HASH`` 与 ``BPF_MAP_TYPE_LRU_PERCPU_HASH``，
``bpf_map_update_elem()`` 与 ``bpf_map_lookup_elem()`` helper 会自动访问当前
CPU 的哈希槽。)

bpf_map_lookup_percpu_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void *bpf_map_lookup_percpu_elem(struct bpf_map *map, const void *key, u32 cpu)

The ``bpf_map_lookup_percpu_elem()`` helper can be used to lookup the
value in the hash slot for a specific CPU. Returns value associated with
``key`` on ``cpu`` , or ``NULL`` if no entry was found or ``cpu`` is
invalid.
(``bpf_map_lookup_percpu_elem()`` helper 可用于查询特定 CPU 哈希槽中的值。
返回 ``cpu`` 上与 ``key`` 关联的值；未找到条目或 ``cpu`` 无效时返回 ``NULL``。)

Concurrency(并发)
-----------------

Values stored in ``BPF_MAP_TYPE_HASH`` can be accessed concurrently by
programs running on different CPUs.  Since Kernel version 5.1, the BPF
infrastructure provides ``struct bpf_spin_lock`` to synchronise access.
See ``tools/testing/selftests/bpf/progs/test_spin_lock.c``.
(存储在 ``BPF_MAP_TYPE_HASH`` 中的值可能被运行在不同 CPU 上的程序并发访问。
自内核版本 5.1 起，BPF 基础设施提供 ``struct bpf_spin_lock`` 来同步访问。
参见 ``tools/testing/selftests/bpf/progs/test_spin_lock.c``。)

Userspace(用户空间)
-------------------

bpf_map_get_next_key()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_get_next_key(int fd, const void *cur_key, void *next_key)

In userspace, it is possible to iterate through the keys of a hash using
libbpf's ``bpf_map_get_next_key()`` function. The first key can be fetched by
calling ``bpf_map_get_next_key()`` with ``cur_key`` set to
``NULL``. Subsequent calls will fetch the next key that follows the
current key. ``bpf_map_get_next_key()`` returns 0 on success, -ENOENT if
cur_key is the last key in the hash, or negative error in case of
failure.
(在用户空间，可以使用 libbpf 的 ``bpf_map_get_next_key()`` 函数遍历哈希中的
键。把 ``cur_key`` 设为 ``NULL`` 调用 ``bpf_map_get_next_key()`` 即可取得第一个
键。后续调用会取得当前键之后的下一个键。``bpf_map_get_next_key()`` 成功时返回
0，cur_key 是哈希中最后一个键时返回 -ENOENT，失败时返回负的错误码。)

Note that if ``cur_key`` gets deleted then ``bpf_map_get_next_key()``
will instead return the *first* key in the hash table which is
undesirable. It is recommended to use batched lookup if there is going
to be key deletion intermixed with ``bpf_map_get_next_key()``.
(注意，如果 ``cur_key`` 被删除，``bpf_map_get_next_key()`` 将改为返回哈希表中
的*第一个* 键，这并不符合期望。如果键删除操作会与 ``bpf_map_get_next_key()``
交错进行，建议改用批量查找 (batched lookup)。)

Examples(示例)
==============

Please see the ``tools/testing/selftests/bpf`` directory for functional
examples.  The code snippets below demonstrates API usage.
(功能性示例见 ``tools/testing/selftests/bpf`` 目录。下面的代码片段演示 API 的
用法。)

This example shows how to declare an LRU Hash with a struct key and a
struct value.
(本例展示如何声明一个带结构体键和结构体值的 LRU 哈希。)

.. code-block:: c

    #include <linux/bpf.h>
    #include <bpf/bpf_helpers.h>

    struct key {
        __u32 srcip;
    };

    struct value {
        __u64 packets;
        __u64 bytes;
    };

    struct {
            __uint(type, BPF_MAP_TYPE_LRU_HASH);
            __uint(max_entries, 32);
            __type(key, struct key);
            __type(value, struct value);
    } packet_stats SEC(".maps");

This example shows how to create or update hash values using atomic
instructions:
(本例展示如何使用原子指令创建或更新哈希值：)

.. code-block:: c

    static void update_stats(__u32 srcip, int bytes)
    {
            struct key key = {
                    .srcip = srcip,
            };
            struct value *value = bpf_map_lookup_elem(&packet_stats, &key);

            if (value) {
                    __sync_fetch_and_add(&value->packets, 1);
                    __sync_fetch_and_add(&value->bytes, bytes);
            } else {
                    struct value newval = { 1, bytes };

                    bpf_map_update_elem(&packet_stats, &key, &newval, BPF_NOEXIST);
            }
    }

Userspace walking the map elements from the map declared above:
(用户空间遍历上面声明的 map 中的元素：)

.. code-block:: c

    #include <bpf/libbpf.h>
    #include <bpf/bpf.h>

    static void walk_hash_elements(int map_fd)
    {
            struct key *cur_key = NULL;
            struct key next_key;
            struct value value;
            int err;

            for (;;) {
                    err = bpf_map_get_next_key(map_fd, cur_key, &next_key);
                    if (err)
                            break;

                    bpf_map_lookup_elem(map_fd, &next_key, &value);

                    // Use key and value here

                    cur_key = &next_key;
            }
    }

Internals(内部实现)
===================

This section of the document is targeted at Linux developers and describes
aspects of the map implementations that are not considered stable ABI. The
following details are subject to change in future versions of the kernel.
(本文档的这一节面向 Linux 开发者，描述 map 实现中不被视为稳定 ABI 的方面。
以下细节在未来的内核版本中可能发生变化。)

``BPF_MAP_TYPE_LRU_HASH`` and variants(``BPF_MAP_TYPE_LRU_HASH`` 及其变体)
--------------------------------------------------------------------------

Updating elements in LRU maps may trigger eviction behaviour when the capacity
of the map is reached. There are various steps that the update algorithm
attempts in order to enforce the LRU property which have increasing impacts on
other CPUs involved in the following operation attempts:
(当 map 达到容量上限时，更新 LRU map 中的元素可能触发驱逐行为。为了维持 LRU
性质，更新算法会依次尝试下面这些操作，它们对所涉及的其他 CPU 的影响逐级
加大：)

- Attempt to use CPU-local state to batch operations
  (尝试利用 CPU 本地状态对操作进行批处理)
- Attempt to fetch ``target_free`` free nodes from global lists
  (尝试从全局链表获取 ``target_free`` 个空闲节点)
- Attempt to pull any node from a global list and remove it from the hashmap
  (尝试从全局链表取出任意一个节点并把它从哈希 map 中移除)
- Attempt to pull any node from any CPU's list and remove it from the hashmap
  (尝试从任意 CPU 的链表取出任意一个节点并把它从哈希 map 中移除)

The number of nodes to borrow from the global list in a batch, ``target_free``,
depends on the size of the map. Larger batch size reduces lock contention, but
may also exhaust the global structure. The value is computed at map init to
avoid exhaustion, by limiting aggregate reservation by all CPUs to half the map
size. With a minimum of a single element and maximum budget of 128 at a time.
(每批从全局链表借用的节点数量 ``target_free`` 取决于 map 的大小。更大的批量
可以减少锁争用，但也可能耗尽全局结构。该值在 map 初始化时计算以避免耗尽：把
所有 CPU 的预留总量限制在 map 大小的一半。下限为单个元素，每次的预算上限为
128。)

This algorithm is described visually in the following diagram. See the
description in commit 3a08c2fd7634 ("bpf: LRU List") for a full explanation of
the corresponding operations:
(下面的示意图直观地描述了该算法。相应操作的完整解释见提交
3a08c2fd7634 ("bpf: LRU List") 的说明：)

.. kernel-figure::  map_lru_hash_update.dot
   :alt:    Diagram outlining the LRU eviction steps taken during map update.

   LRU hash eviction during map update for ``BPF_MAP_TYPE_LRU_HASH`` and
   variants. See the dot file source for kernel function name code references.
   (``BPF_MAP_TYPE_LRU_HASH`` 及其变体在 map 更新期间的 LRU 驱逐过程。内核
   函数名的代码引用见 dot 文件源。)

Map updates start from the oval in the top right "begin ``bpf_map_update()``"
and progress through the graph towards the bottom where the result may be
either a successful update or a failure with various error codes. The key in
the top right provides indicators for which locks may be involved in specific
operations. This is intended as a visual hint for reasoning about how map
contention may impact update operations, though the map type and flags may
impact the actual contention on those locks, based on the logic described in
the table above. For instance, if the map is created with type
``BPF_MAP_TYPE_LRU_PERCPU_HASH`` and flags ``BPF_F_NO_COMMON_LRU`` then all map
properties would be per-cpu.
(Map 更新从右上角的椭圆 "begin ``bpf_map_update()``" 开始，沿图向下推进，最终
结果可能是成功更新，或是带有各种错误码的失败。右上角的图例标出了哪些锁可能
参与特定操作。这是为了直观提示如何推理 map 争用对更新操作的影响；不过，依据
上表所述的逻辑，map 类型与标志也可能影响这些锁上的实际争用情况。例如，如果
以 ``BPF_MAP_TYPE_LRU_PERCPU_HASH`` 类型加 ``BPF_F_NO_COMMON_LRU`` 标志创建
map，那么所有 map 属性都将是每 CPU 独立的。)
