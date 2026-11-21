.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2020 Google LLC.

============================================
BPF_MAP_TYPE_CGROUP_STORAGE(cgroup 存储映射)
============================================

The ``BPF_MAP_TYPE_CGROUP_STORAGE`` map type represents a local fix-sized
storage. It is only available with ``CONFIG_CGROUP_BPF``, and to programs that
attach to cgroups; the programs are made available by the same Kconfig. The
storage is identified by the cgroup the program is attached to.
( ``BPF_MAP_TYPE_CGROUP_STORAGE`` 映射类型表示一个本地固定大小的存储。它仅在 ``CONFIG_CGROUP_BPF`` 下可用，并且只提供给附加到 cgroup 的程序使用；这些程序由同一个 Kconfig 提供。存储由程序所附加到的 cgroup 标识。)

The map provide a local storage at the cgroup that the BPF program is attached
to. It provides a faster and simpler access than the general purpose hash
table, which performs a hash table lookups, and requires user to track live
cgroups on their own.
(该映射在 BPF 程序附加到的 cgroup 处提供一个本地存储。与需要执行哈希表查找、并要求用户自己跟踪存活 cgroup 的通用哈希表相比，它提供了更快、更简单的访问。)

This document describes the usage and semantics of the
``BPF_MAP_TYPE_CGROUP_STORAGE`` map type. Some of its behaviors was changed in
Linux 5.9 and this document will describe the differences.
(本文描述 ``BPF_MAP_TYPE_CGROUP_STORAGE`` 映射类型的用法和语义。它的一些行为在 Linux 5.9 中发生了变化，本文将描述这些差异。)

Usage(用法)
===========

The map uses key of type of either ``__u64 cgroup_inode_id`` or
``struct bpf_cgroup_storage_key``, declared in ``linux/bpf.h``::

    struct bpf_cgroup_storage_key {
            __u64 cgroup_inode_id;
            __u32 attach_type;
    };

(该映射使用的键类型要么是 ``__u64 cgroup_inode_id``，要么是 ``struct bpf_cgroup_storage_key``，后者声明在 ``linux/bpf.h`` 中：)

``cgroup_inode_id`` is the inode id of the cgroup directory.
``attach_type`` is the program's attach type.
( ``cgroup_inode_id`` 是 cgroup 目录的 inode id。 ``attach_type`` 是程序的附加类型。)

Linux 5.9 added support for type ``__u64 cgroup_inode_id`` as the key type.
When this key type is used, then all attach types of the particular cgroup and
map will share the same storage. Otherwise, if the type is
``struct bpf_cgroup_storage_key``, then programs of different attach types
be isolated and see different storages.
(Linux 5.9 增加了对 ``__u64 cgroup_inode_id`` 键类型的支持。使用这种键类型时，特定 cgroup 和映射的所有附加类型将共享同一个存储。否则，如果类型是 ``struct bpf_cgroup_storage_key``，则不同附加类型的程序将被隔离，看到不同的存储。)

To access the storage in a program, use ``bpf_get_local_storage``::

    void *bpf_get_local_storage(void *map, u64 flags)

(要在程序中访问该存储，使用 ``bpf_get_local_storage``：)

``flags`` is reserved for future use and must be 0.
( ``flags`` 保留供将来使用，必须为 0。)

There is no implicit synchronization. Storages of ``BPF_MAP_TYPE_CGROUP_STORAGE``
can be accessed by multiple programs across different CPUs, and user should
take care of synchronization by themselves. The bpf infrastructure provides
``struct bpf_spin_lock`` to synchronize the storage. See
``tools/testing/selftests/bpf/progs/test_spin_lock.c``.
(没有隐式同步。 ``BPF_MAP_TYPE_CGROUP_STORAGE`` 的存储可以被不同 CPU 上的多个程序访问，用户需要自己负责同步。BPF 基础设施提供 ``struct bpf_spin_lock`` 来同步存储。参见 ``tools/testing/selftests/bpf/progs/test_spin_lock.c`` 。)

Examples(示例)
==============

Usage with key type as ``struct bpf_cgroup_storage_key``::

    #include <bpf/bpf.h>

    struct {
            __uint(type, BPF_MAP_TYPE_CGROUP_STORAGE);
            __type(key, struct bpf_cgroup_storage_key);
            __type(value, __u32);
    } cgroup_storage SEC(".maps");

    int program(struct __sk_buff *skb)
    {
            __u32 *ptr = bpf_get_local_storage(&cgroup_storage, 0);
            __sync_fetch_and_add(ptr, 1);

            return 0;
    }

(以 ``struct bpf_cgroup_storage_key`` 作为键类型的用法：)

Userspace accessing map declared above::

    #include <linux/bpf.h>
    #include <linux/libbpf.h>

    __u32 map_lookup(struct bpf_map *map, __u64 cgrp, enum bpf_attach_type type)
    {
            struct bpf_cgroup_storage_key = {
                    .cgroup_inode_id = cgrp,
                    .attach_type = type,
            };
            __u32 value;
            bpf_map_lookup_elem(bpf_map__fd(map), &key, &value);
            // error checking omitted
            return value;
    }

(用户空间访问上面声明的映射：)

Alternatively, using just ``__u64 cgroup_inode_id`` as key type::

    #include <bpf/bpf.h>

    struct {
            __uint(type, BPF_MAP_TYPE_CGROUP_STORAGE);
            __type(key, __u64);
            __type(value, __u32);
    } cgroup_storage SEC(".maps");

    int program(struct __sk_buff *skb)
    {
            __u32 *ptr = bpf_get_local_storage(&cgroup_storage, 0);
            __sync_fetch_and_add(ptr, 1);

            return 0;
    }

(或者，仅使用 ``__u64 cgroup_inode_id`` 作为键类型：)

And userspace::

    #include <linux/bpf.h>
    #include <linux/libbpf.h>

    __u32 map_lookup(struct bpf_map *map, __u64 cgrp, enum bpf_attach_type type)
    {
            __u32 value;
            bpf_map_lookup_elem(bpf_map__fd(map), &cgrp, &value);
            // error checking omitted
            return value;
    }

(对应的用户空间代码：)

Semantics(语义)
===============

``BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE`` is a variant of this map type. This
per-CPU variant will have different memory regions for each CPU for each
storage. The non-per-CPU will have the same memory region for each storage.
( ``BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE`` 是此映射类型的一个变体。这个 per-CPU 变体会为每个 CPU 的每个存储分配不同的内存区域。非 per-CPU 变体则为每个存储使用相同的内存区域。)

Prior to Linux 5.9, the lifetime of a storage is precisely per-attachment, and
for a single ``CGROUP_STORAGE`` map, there can be at most one program loaded
that uses the map. A program may be attached to multiple cgroups or have
multiple attach types, and each attach creates a fresh zeroed storage. The
storage is freed upon detach.
(在 Linux 5.9 之前，存储的生命周期精确地对应于每次附加 (per-attachment)，并且对于单个 ``CGROUP_STORAGE`` 映射，最多只能加载一个使用该映射的程序。一个程序可以附加到多个 cgroup 或具有多个附加类型，每次附加都会创建一个全新的清零存储。存储在分离 (detach) 时被释放。)

There is a one-to-one association between the map of each type (per-CPU and
non-per-CPU) and the BPF program during load verification time. As a result,
each map can only be used by one BPF program and each BPF program can only use
one storage map of each type. Because of map can only be used by one BPF
program, sharing of this cgroup's storage with other BPF programs were
impossible.
(在加载验证期间，每种类型(per-CPU 和非 per-CPU)的映射与 BPF 程序之间存在一对一的关联。因此，每个映射只能被一个 BPF 程序使用，每个 BPF 程序也只能使用每种类型的一个存储映射。由于映射只能被一个 BPF 程序使用，与其他 BPF 程序共享此 cgroup 的存储是不可能的。)

Since Linux 5.9, storage can be shared by multiple programs. When a program is
attached to a cgroup, the kernel would create a new storage only if the map
does not already contain an entry for the cgroup and attach type pair, or else
the old storage is reused for the new attachment. If the map is attach type
shared, then attach type is simply ignored during comparison. Storage is freed
only when either the map or the cgroup attached to is being freed. Detaching
will not directly free the storage, but it may cause the reference to the map
to reach zero and indirectly freeing all storage in the map.
(从 Linux 5.9 开始，存储可以被多个程序共享。当一个程序附加到某个 cgroup 时，只有当映射中尚不存在该 cgroup 与附加类型对的条目时，内核才会创建新存储；否则旧存储将被新的附加复用。如果映射是附加类型共享的 (attach type shared)，则在比较时会直接忽略附加类型。只有在映射本身或所附加的 cgroup 被释放时，存储才会被释放。分离不会直接释放存储，但可能导致对该映射的引用降至零，从而间接释放映射中的所有存储。)

The map is not associated with any BPF program, thus making sharing possible.
However, the BPF program can still only associate with one map of each type
(per-CPU and non-per-CPU). A BPF program cannot use more than one
``BPF_MAP_TYPE_CGROUP_STORAGE`` or more than one
``BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE``.
(该映射不再与任何 BPF 程序关联，从而使共享成为可能。不过，BPF 程序仍然只能与每种类型(per-CPU 和非 per-CPU)的一个映射关联。一个 BPF 程序不能使用多于一个 ``BPF_MAP_TYPE_CGROUP_STORAGE`` 或多于一个 ``BPF_MAP_TYPE_PERCPU_CGROUP_STORAGE`` 。)

In all versions, userspace may use the attach parameters of cgroup and
attach type pair in ``struct bpf_cgroup_storage_key`` as the key to the BPF map
APIs to read or update the storage for a given attachment. For Linux 5.9
attach type shared storages, only the first value in the struct, cgroup inode
id, is used during comparison, so userspace may just specify a ``__u64``
directly.
(在所有版本中，用户空间都可以使用 cgroup 与附加类型对(即 ``struct bpf_cgroup_storage_key``)的附加参数作为 BPF 映射 API 的键，来读取或更新给定附加的存储。对于 Linux 5.9 的附加类型共享存储，比较时只使用该结构体的第一个值(cgroup inode id)，因此用户空间可以直接指定一个 ``__u64`` 。)

The storage is bound at attach time. Even if the program is attached to parent
and triggers in child, the storage still belongs to the parent.
(存储在附加时绑定。即使程序附加在父 cgroup 上而在子 cgroup 中触发，存储仍然属于父 cgroup。)

Userspace cannot create a new entry in the map or delete an existing entry.
Program test runs always use a temporary storage.
(用户空间不能在映射中创建新条目或删除已有条目。程序测试运行始终使用临时存储。)
