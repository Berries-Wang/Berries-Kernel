.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

===========================================
BPF_MAP_TYPE_SK_STORAGE(BPF 套接字本地存储)
===========================================

.. note::
   - ``BPF_MAP_TYPE_SK_STORAGE`` was introduced in kernel version 5.2
     (``BPF_MAP_TYPE_SK_STORAGE`` 在内核版本 5.2 中引入)

``BPF_MAP_TYPE_SK_STORAGE`` is used to provide socket-local storage for BPF
programs. A map of type ``BPF_MAP_TYPE_SK_STORAGE`` declares the type of storage
to be provided and acts as the handle for accessing the socket-local
storage. The values for maps of type ``BPF_MAP_TYPE_SK_STORAGE`` are stored
locally with each socket instead of with the map. The kernel is responsible for
allocating storage for a socket when requested and for freeing the storage when
either the map or the socket is deleted.
(``BPF_MAP_TYPE_SK_STORAGE`` 用于为 BPF 程序提供套接字本地存储。类型为
``BPF_MAP_TYPE_SK_STORAGE`` 的 map 声明要提供的存储类型，并充当访问套接字本地
存储的句柄。类型为 ``BPF_MAP_TYPE_SK_STORAGE`` 的 map 的值本地存储在每个套接字
上，而不是存储在 map 中。内核负责在收到请求时为套接字分配存储，并在 map 或
套接字被删除时释放该存储。)

.. note::
  - The key type must be ``int`` and ``max_entries`` must be set to ``0``.
    (键类型必须是 ``int``，且 ``max_entries`` 必须设置为 ``0``。)
  - The ``BPF_F_NO_PREALLOC`` flag must be used when creating a map for
    socket-local storage.
    (为套接字本地存储创建 map 时必须使用 ``BPF_F_NO_PREALLOC`` 标志。)

Usage(用法)
===========

Kernel BPF(内核 BPF)
--------------------

bpf_sk_storage_get()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void *bpf_sk_storage_get(struct bpf_map *map, void *sk, void *value, u64 flags)

Socket-local storage for ``map`` can be retrieved from socket ``sk`` using the
``bpf_sk_storage_get()`` helper. If the ``BPF_LOCAL_STORAGE_GET_F_CREATE``
flag is used then ``bpf_sk_storage_get()`` will create the storage for ``sk``
if it does not already exist. ``value`` can be used together with
``BPF_LOCAL_STORAGE_GET_F_CREATE`` to initialize the storage value, otherwise
it will be zero initialized. Returns a pointer to the storage on success, or
``NULL`` in case of failure.
(可以使用 ``bpf_sk_storage_get()`` helper 从套接字 ``sk`` 检索 ``map`` 的套接字
本地存储。如果使用 ``BPF_LOCAL_STORAGE_GET_F_CREATE`` 标志，那么当 ``sk`` 尚不
存在该存储时，``bpf_sk_storage_get()`` 会为其创建存储。``value`` 可与
``BPF_LOCAL_STORAGE_GET_F_CREATE`` 一起使用来初始化存储值，否则存储值将被
初始化为零。成功时返回指向该存储的指针，失败时返回 ``NULL``。)

.. note::
   - ``sk`` is a kernel ``struct sock`` pointer for LSM or tracing programs.
     (对于 LSM 或 tracing 程序，``sk`` 是内核 ``struct sock`` 指针。)
   - ``sk`` is a ``struct bpf_sock`` pointer for other program types.
     (对于其他程序类型，``sk`` 是 ``struct bpf_sock`` 指针。)

bpf_sk_storage_delete()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_sk_storage_delete(struct bpf_map *map, void *sk)

Socket-local storage for ``map`` can be deleted from socket ``sk`` using the
``bpf_sk_storage_delete()`` helper. Returns ``0`` on success, or negative
error in case of failure.
(可以使用 ``bpf_sk_storage_delete()`` helper 从套接字 ``sk`` 删除 ``map`` 的
套接字本地存储。成功时返回 ``0``，失败时返回负的错误码。)

User space(用户空间)
--------------------

bpf_map_update_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_update_elem(int map_fd, const void *key, const void *value, __u64 flags)

Socket-local storage for map ``map_fd`` can be added or updated locally to a
socket using the ``bpf_map_update_elem()`` libbpf function. The socket is
identified by a `socket` ``fd`` stored in the pointer ``key``. The pointer
``value`` has the data to be added or updated to the socket ``fd``. The type
and size of ``value`` should be the same as the value type of the map
definition.
(可以使用 ``bpf_map_update_elem()`` libbpf 函数向套接字本地添加或更新 map
``map_fd`` 的套接字本地存储。套接字由存储在指针 ``key`` 中的 `socket` ``fd``
标识。指针 ``value`` 包含要添加或更新到该套接字 ``fd`` 的数据。``value`` 的
类型和大小应与 map 定义中的值类型相同。)

The ``flags`` parameter can be used to control the update behaviour:
(``flags`` 参数可用于控制更新行为：)

- ``BPF_ANY`` will create storage for `socket` ``fd`` or update existing storage.
  (``BPF_ANY`` 将为 `socket` ``fd`` 创建存储，或更新已有存储。)
- ``BPF_NOEXIST`` will create storage for `socket` ``fd`` only if it did not
  already exist, otherwise the call will fail with ``-EEXIST``.
  (``BPF_NOEXIST`` 仅在 `socket` ``fd`` 的存储尚不存在时才创建存储，否则调用
  将以 ``-EEXIST`` 失败。)
- ``BPF_EXIST`` will update existing storage for `socket` ``fd`` if it already
  exists, otherwise the call will fail with ``-ENOENT``.
  (``BPF_EXIST`` 将更新 `socket` ``fd`` 已存在的存储，否则调用将以
  ``-ENOENT`` 失败。)

Returns ``0`` on success, or negative error in case of failure.
(成功时返回 ``0``，失败时返回负的错误码。)

bpf_map_lookup_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_lookup_elem(int map_fd, const void *key, void *value)

Socket-local storage for map ``map_fd`` can be retrieved from a socket using
the ``bpf_map_lookup_elem()`` libbpf function. The storage is retrieved from
the socket identified by a `socket` ``fd`` stored in the pointer
``key``. Returns ``0`` on success, or negative error in case of failure.
(可以使用 ``bpf_map_lookup_elem()`` libbpf 函数从套接字检索 map ``map_fd`` 的
套接字本地存储。存储从由指针 ``key`` 中的 `socket` ``fd`` 标识的套接字检索。
成功时返回 ``0``，失败时返回负的错误码。)

bpf_map_delete_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int bpf_map_delete_elem(int map_fd, const void *key)

Socket-local storage for map ``map_fd`` can be deleted from a socket using the
``bpf_map_delete_elem()`` libbpf function. The storage is deleted from the
socket identified by a `socket` ``fd`` stored in the pointer ``key``. Returns
``0`` on success, or negative error in case of failure.
(可以使用 ``bpf_map_delete_elem()`` libbpf 函数从套接字删除 map ``map_fd`` 的
套接字本地存储。存储从由指针 ``key`` 中的 `socket` ``fd`` 标识的套接字删除。
成功时返回 ``0``，失败时返回负的错误码。)

Examples(示例)
==============

Kernel BPF(内核 BPF)
--------------------

This snippet shows how to declare socket-local storage in a BPF program:
(以下代码片段展示了如何在 BPF 程序中声明套接字本地存储：)

.. code-block:: c

    struct {
            __uint(type, BPF_MAP_TYPE_SK_STORAGE);
            __uint(map_flags, BPF_F_NO_PREALLOC);
            __type(key, int);
            __type(value, struct my_storage);
    } socket_storage SEC(".maps");

This snippet shows how to retrieve socket-local storage in a BPF program:
(以下代码片段展示了如何在 BPF 程序中检索套接字本地存储：)

.. code-block:: c

    SEC("sockops")
    int _sockops(struct bpf_sock_ops *ctx)
    {
            struct my_storage *storage;
            struct bpf_sock *sk;

            sk = ctx->sk;
            if (!sk)
                    return 1;

            storage = bpf_sk_storage_get(&socket_storage, sk, 0,
                                         BPF_LOCAL_STORAGE_GET_F_CREATE);
            if (!storage)
                    return 1;

            /* Use 'storage' here */

            return 1;
    }


Please see the ``tools/testing/selftests/bpf`` directory for functional
examples.
(功能性示例请参见 ``tools/testing/selftests/bpf`` 目录。)

References(参考)
================

https://lwn.net/ml/netdev/20190426171103.61892-1-kafai@fb.com/
