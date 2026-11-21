.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

====================================================================================
BPF_MAP_TYPE_ARRAY_OF_MAPS and BPF_MAP_TYPE_HASH_OF_MAPS(BPF 的 map-in-map 映射类型)
====================================================================================

.. note::
   - ``BPF_MAP_TYPE_ARRAY_OF_MAPS`` and ``BPF_MAP_TYPE_HASH_OF_MAPS`` were
     introduced in kernel version 4.12
     (``BPF_MAP_TYPE_ARRAY_OF_MAPS`` 和 ``BPF_MAP_TYPE_HASH_OF_MAPS`` 在内核版本 4.12 中引入)

``BPF_MAP_TYPE_ARRAY_OF_MAPS`` and ``BPF_MAP_TYPE_HASH_OF_MAPS`` provide general
purpose support for map in map storage. One level of nesting is supported, where
an outer map contains instances of a single type of inner map, for example
``array_of_maps->sock_map``.
(``BPF_MAP_TYPE_ARRAY_OF_MAPS`` 和 ``BPF_MAP_TYPE_HASH_OF_MAPS`` 为 map-in-map 存储提供通用支持。支持一层嵌套，即外层 map 包含单一类型内层 map 的实例，例如 ``array_of_maps->sock_map`` 。)

When creating an outer map, an inner map instance is used to initialize the
metadata that the outer map holds about its inner maps. This inner map has a
separate lifetime from the outer map and can be deleted after the outer map has
been created.
(创建外层 map 时，会使用一个内层 map 实例来初始化外层 map 关于其内层 map 所保存的元数据。这个内层 map 与外层 map 的生命周期相互独立，可以在外层 map 创建之后删除。)

The outer map supports element lookup, update and delete from user space using
the syscall API. A BPF program is only allowed to do element lookup in the outer
map.
(外层 map 支持从用户空间使用系统调用 API 进行元素查找、更新和删除。BPF 程序只允许在外层 map 中进行元素查找。)

.. note::
   - Multi-level nesting is not supported.
     (不支持多级嵌套。)
   - Any BPF map type can be used as an inner map, except for
     ``BPF_MAP_TYPE_PROG_ARRAY``.
     (任何 BPF map 类型都可以用作内层 map，但 ``BPF_MAP_TYPE_PROG_ARRAY`` 除外。)
   - A BPF program cannot update or delete outer map entries.
     (BPF 程序不能更新或删除外层 map 的条目。)

For ``BPF_MAP_TYPE_ARRAY_OF_MAPS`` the key is an unsigned 32-bit integer index
into the array. The array is a fixed size with ``max_entries`` elements that are
zero initialized when created.
(对于 ``BPF_MAP_TYPE_ARRAY_OF_MAPS`` ，键是数组中的无符号 32 位整数索引。数组大小固定，包含 ``max_entries`` 个元素，创建时被初始化为零。)

For ``BPF_MAP_TYPE_HASH_OF_MAPS`` the key type can be chosen when defining the
map. The kernel is responsible for allocating and freeing key/value pairs, up to
the max_entries limit that you specify. Hash maps use pre-allocation of hash
table elements by default. The ``BPF_F_NO_PREALLOC`` flag can be used to disable
pre-allocation when it is too memory expensive.
(对于 ``BPF_MAP_TYPE_HASH_OF_MAPS`` ，键类型可以在定义 map 时选择。内核负责分配和释放键/值对，上限为你指定的 max_entries。哈希 map 默认预分配哈希表元素。当预分配的内存代价过高时，可以使用 ``BPF_F_NO_PREALLOC`` 标志来禁用预分配。)

Usage(用法)
===========

Kernel BPF Helper(内核 BPF 辅助函数)
------------------------------------

bpf_map_lookup_elem()
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void *bpf_map_lookup_elem(struct bpf_map *map, const void *key)

Inner maps can be retrieved using the ``bpf_map_lookup_elem()`` helper. This
helper returns a pointer to the inner map, or ``NULL`` if no entry was found.
(可以使用 ``bpf_map_lookup_elem()`` 辅助函数检索内层 map。该辅助函数返回指向内层 map 的指针，如果未找到条目则返回 ``NULL`` 。)

Examples(示例)
==============

Kernel BPF Example(内核 BPF 示例)
---------------------------------

This snippet shows how to create and initialise an array of devmaps in a BPF
program. Note that the outer array can only be modified from user space using
the syscall API.
(这段代码片段展示了如何在 BPF 程序中创建并初始化一个 devmap 数组。注意，外层数组只能从用户空间使用系统调用 API 修改。)

.. code-block:: c

    struct inner_map {
            __uint(type, BPF_MAP_TYPE_DEVMAP);
            __uint(max_entries, 10);
            __type(key, __u32);
            __type(value, __u32);
    } inner_map1 SEC(".maps"), inner_map2 SEC(".maps");

    struct {
            __uint(type, BPF_MAP_TYPE_ARRAY_OF_MAPS);
            __uint(max_entries, 2);
            __type(key, __u32);
            __array(values, struct inner_map);
    } outer_map SEC(".maps") = {
            .values = { &inner_map1,
                        &inner_map2 }
    };

See ``progs/test_btf_map_in_map.c`` in ``tools/testing/selftests/bpf`` for more
examples of declarative initialisation of outer maps.
(更多外层 map 声明式初始化的示例参见 ``tools/testing/selftests/bpf`` 中的 ``progs/test_btf_map_in_map.c`` 。)

User Space(用户空间)
--------------------

This snippet shows how to create an array based outer map:
(这段代码片段展示了如何创建一个基于数组的外层 map：)

.. code-block:: c

    int create_outer_array(int inner_fd) {
            LIBBPF_OPTS(bpf_map_create_opts, opts, .inner_map_fd = inner_fd);
            int fd;

            fd = bpf_map_create(BPF_MAP_TYPE_ARRAY_OF_MAPS,
                                "example_array",       /* name */
                                sizeof(__u32),         /* key size */
                                sizeof(__u32),         /* value size */
                                256,                   /* max entries */
                                &opts);                /* create opts */
            return fd;
    }


This snippet shows how to add an inner map to an outer map:
(这段代码片段展示了如何向外层 map 添加一个内层 map：)

.. code-block:: c

    int add_devmap(int outer_fd, int index, const char *name) {
            int fd;

            fd = bpf_map_create(BPF_MAP_TYPE_DEVMAP, name,
                                sizeof(__u32), sizeof(__u32), 256, NULL);
            if (fd < 0)
                    return fd;

            return bpf_map_update_elem(outer_fd, &index, &fd, BPF_ANY);
    }

References(参考)
================

- https://lore.kernel.org/netdev/20170322170035.923581-3-kafai@fb.com/
- https://lore.kernel.org/netdev/20170322170035.923581-4-kafai@fb.com/
