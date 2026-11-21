.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

===================================================================================
BPF_MAP_TYPE_QUEUE and BPF_MAP_TYPE_STACK(BPF_MAP_TYPE_QUEUE 与 BPF_MAP_TYPE_STACK)
===================================================================================

.. note::
   - ``BPF_MAP_TYPE_QUEUE`` and ``BPF_MAP_TYPE_STACK`` were introduced
     in kernel version 4.20
     (``BPF_MAP_TYPE_QUEUE`` 与 ``BPF_MAP_TYPE_STACK`` 在内核版本 4.20 中引入)

``BPF_MAP_TYPE_QUEUE`` provides FIFO storage and ``BPF_MAP_TYPE_STACK``
provides LIFO storage for BPF programs. These maps support peek, pop and
push operations that are exposed to BPF programs through the respective
helpers. These operations are exposed to userspace applications using
the existing ``bpf`` syscall in the following way:
(``BPF_MAP_TYPE_QUEUE`` 为 BPF 程序提供 FIFO 存储，``BPF_MAP_TYPE_STACK``
提供 LIFO 存储。这些 map 支持查看 (peek)、弹出 (pop) 和压入 (push) 操作，
它们通过各自的 helper 暴露给 BPF 程序。这些操作通过现有的 ``bpf`` 系统调用
按以下方式暴露给用户空间应用：)

- ``BPF_MAP_LOOKUP_ELEM`` -> peek
  (``BPF_MAP_LOOKUP_ELEM`` -> 查看 (peek))
- ``BPF_MAP_LOOKUP_AND_DELETE_ELEM`` -> pop
  (``BPF_MAP_LOOKUP_AND_DELETE_ELEM`` -> 弹出 (pop))
- ``BPF_MAP_UPDATE_ELEM`` -> push
  (``BPF_MAP_UPDATE_ELEM`` -> 压入 (push))

``BPF_MAP_TYPE_QUEUE`` and ``BPF_MAP_TYPE_STACK`` do not support
``BPF_F_NO_PREALLOC``.
(``BPF_MAP_TYPE_QUEUE`` 与 ``BPF_MAP_TYPE_STACK`` 不支持
``BPF_F_NO_PREALLOC``。)

Usage(用法)
===========

Kernel BPF(内核 BPF)
--------------------

bpf_map_push_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_push_elem(struct bpf_map *map, const void *value, u64 flags)

An element ``value`` can be added to a queue or stack using the
``bpf_map_push_elem`` helper. The ``flags`` parameter must be set to
``BPF_ANY`` or ``BPF_EXIST``. If ``flags`` is set to ``BPF_EXIST`` then,
when the queue or stack is full, the oldest element will be removed to
make room for ``value`` to be added. Returns ``0`` on success, or
negative error in case of failure.
(可以使用 ``bpf_map_push_elem`` helper 把元素 ``value`` 添加到队列或栈中。
``flags`` 参数必须设置为 ``BPF_ANY`` 或 ``BPF_EXIST``。如果 ``flags`` 设置
为 ``BPF_EXIST``，那么当队列或栈已满时，最旧的元素将被移除，为要添加的
``value`` 腾出空间。成功时返回 ``0``，失败时返回负的错误码。)

bpf_map_peek_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_peek_elem(struct bpf_map *map, void *value)

This helper fetches an element ``value`` from a queue or stack without
removing it. Returns ``0`` on success, or negative error in case of
failure.
(该 helper 从队列或栈中取出一个元素 ``value``，但不移除它。成功时返回
``0``，失败时返回负的错误码。)

bpf_map_pop_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_pop_elem(struct bpf_map *map, void *value)

This helper removes an element into ``value`` from a queue or
stack. Returns ``0`` on success, or negative error in case of failure.
(该 helper 从队列或栈中移除一个元素并放入 ``value``。成功时返回 ``0``，
失败时返回负的错误码。)


Userspace(用户空间)
-------------------

bpf_map_update_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_update_elem (int fd, const void *key, const void *value, __u64 flags)

A userspace program can push ``value`` onto a queue or stack using libbpf's
``bpf_map_update_elem`` function. The ``key`` parameter must be set to
``NULL`` and ``flags`` must be set to ``BPF_ANY`` or ``BPF_EXIST``, with the
same semantics as the ``bpf_map_push_elem`` kernel helper. Returns ``0`` on
success, or negative error in case of failure.
(用户空间程序可以使用 libbpf 的 ``bpf_map_update_elem`` 函数把 ``value``
压入队列或栈。``key`` 参数必须设置为 ``NULL``，``flags`` 必须设置为
``BPF_ANY`` 或 ``BPF_EXIST``，语义与 ``bpf_map_push_elem`` 内核 helper 相同。
成功时返回 ``0``，失败时返回负的错误码。)

bpf_map_lookup_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_lookup_elem (int fd, const void *key, void *value)

A userspace program can peek at the ``value`` at the head of a queue or stack
using the libbpf ``bpf_map_lookup_elem`` function. The ``key`` parameter must be
set to ``NULL``.  Returns ``0`` on success, or negative error in case of
failure.
(用户空间程序可以使用 libbpf 的 ``bpf_map_lookup_elem`` 函数查看队列或栈
头部处的 ``value``。``key`` 参数必须设置为 ``NULL``。成功时返回 ``0``，
失败时返回负的错误码。)

bpf_map_lookup_and_delete_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_lookup_and_delete_elem (int fd, const void *key, void *value)

A userspace program can pop a ``value`` from the head of a queue or stack using
the libbpf ``bpf_map_lookup_and_delete_elem`` function. The ``key`` parameter
must be set to ``NULL``. Returns ``0`` on success, or negative error in case of
failure.
(用户空间程序可以使用 libbpf 的 ``bpf_map_lookup_and_delete_elem`` 函数从
队列或栈头部弹出一个 ``value``。``key`` 参数必须设置为 ``NULL``。成功时
返回 ``0``，失败时返回负的错误码。)

Examples(示例)
==============

Kernel BPF(内核 BPF)
--------------------

This snippet shows how to declare a queue in a BPF program:
(以下代码片段展示了如何在 BPF 程序中声明一个队列：)

.. code-block:: c

    struct {
            __uint(type, BPF_MAP_TYPE_QUEUE);
            __type(value, __u32);
            __uint(max_entries, 10);
    } queue SEC(".maps");


Userspace(用户空间)
-------------------

This snippet shows how to use libbpf's low-level API to create a queue from
userspace:
(以下代码片段展示了如何使用 libbpf 的低层 API 从用户空间创建一个队列：)

.. code-block:: c

    int create_queue()
    {
            return bpf_map_create(BPF_MAP_TYPE_QUEUE,
                                  "sample_queue", /* name */
                                  0,              /* key size, must be zero */
                                  sizeof(__u32),  /* value size */
                                  10,             /* max entries */
                                  NULL);          /* create options */
    }


References(参考)
================

https://lwn.net/ml/netdev/153986858555.9127.14517764371945179514.stgit@kernel/
