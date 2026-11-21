.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Meta Platforms, Inc. and affiliates.

==================================================
BPF_MAP_TYPE_CGRP_STORAGE(BPF cgroup 本地存储映射)
==================================================

The ``BPF_MAP_TYPE_CGRP_STORAGE`` map type represents a local fix-sized
storage for cgroups. It is only available with ``CONFIG_CGROUPS``.
The programs are made available by the same Kconfig. The
data for a particular cgroup can be retrieved by looking up the map
with that cgroup.
(``BPF_MAP_TYPE_CGRP_STORAGE`` 映射类型表示 cgroup 的本地固定大小存储。
它仅在 ``CONFIG_CGROUPS`` 可用时才存在。相关程序由同一个 Kconfig 提供。
特定 cgroup 的数据可以通过以该 cgroup 查找映射来获取。)

This document describes the usage and semantics of the
``BPF_MAP_TYPE_CGRP_STORAGE`` map type.
(本文描述 ``BPF_MAP_TYPE_CGRP_STORAGE`` 映射类型的用法与语义。)

Usage(用法)
===========

The map key must be ``sizeof(int)`` representing a cgroup fd.
To access the storage in a program, use ``bpf_cgrp_storage_get``::

    void *bpf_cgrp_storage_get(struct bpf_map *map, struct cgroup *cgroup, void *value, u64 flags)

(映射的键必须是表示 cgroup fd 的 ``sizeof(int)``。要在程序中访问该存储，
使用 ``bpf_cgrp_storage_get``：)

``flags`` could be 0 or ``BPF_LOCAL_STORAGE_GET_F_CREATE`` which indicates that
a new local storage will be created if one does not exist.
(``flags`` 可以为 0 或 ``BPF_LOCAL_STORAGE_GET_F_CREATE``，后者表示如果
本地存储尚不存在，将创建一个新的本地存储。)

The local storage can be removed with ``bpf_cgrp_storage_delete``::

    long bpf_cgrp_storage_delete(struct bpf_map *map, struct cgroup *cgroup)

(可以使用 ``bpf_cgrp_storage_delete`` 删除本地存储：)

The map is available to all program types.
(该映射对所有程序类型均可用。)

Examples(示例)
==============

A BPF program example with BPF_MAP_TYPE_CGRP_STORAGE::

    #include <vmlinux.h>
    #include <bpf/bpf_helpers.h>
    #include <bpf/bpf_tracing.h>

    struct {
            __uint(type, BPF_MAP_TYPE_CGRP_STORAGE);
            __uint(map_flags, BPF_F_NO_PREALLOC);
            __type(key, int);
            __type(value, long);
    } cgrp_storage SEC(".maps");

    SEC("tp_btf/sys_enter")
    int BPF_PROG(on_enter, struct pt_regs *regs, long id)
    {
            struct task_struct *task = bpf_get_current_task_btf();
            long *ptr;

            ptr = bpf_cgrp_storage_get(&cgrp_storage, task->cgroups->dfl_cgrp, 0,
                                       BPF_LOCAL_STORAGE_GET_F_CREATE);
            if (ptr)
                __sync_fetch_and_add(ptr, 1);

            return 0;
    }

(一个使用 BPF_MAP_TYPE_CGRP_STORAGE 的 BPF 程序示例：)

Userspace accessing map declared above::

    #include <linux/bpf.h>
    #include <linux/libbpf.h>

    __u32 map_lookup(struct bpf_map *map, int cgrp_fd)
    {
            __u32 *value;
            value = bpf_map_lookup_elem(bpf_map__fd(map), &cgrp_fd);
            if (value)
                return *value;
            return 0;
    }

(用户态访问上述声明的映射：)

Difference Between BPF_MAP_TYPE_CGRP_STORAGE and BPF_MAP_TYPE_CGROUP_STORAGE(BPF_MAP_TYPE_CGRP_STORAGE 与 BPF_MAP_TYPE_CGROUP_STORAGE 的区别)
=============================================================================================================================================

The old cgroup storage map ``BPF_MAP_TYPE_CGROUP_STORAGE`` has been marked as
deprecated (renamed to ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED``). The new
``BPF_MAP_TYPE_CGRP_STORAGE`` map should be used instead. The following
illusates the main difference between ``BPF_MAP_TYPE_CGRP_STORAGE`` and
``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED``.
(旧的 cgroup 存储映射 ``BPF_MAP_TYPE_CGROUP_STORAGE`` 已被标记为弃用
(更名为 ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED``)。应改用新的
``BPF_MAP_TYPE_CGRP_STORAGE`` 映射。下面说明 ``BPF_MAP_TYPE_CGRP_STORAGE``
与 ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` 之间的主要区别。)

(1). ``BPF_MAP_TYPE_CGRP_STORAGE`` can be used by all program types while
     ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` is available only to cgroup program types
     like BPF_CGROUP_INET_INGRESS or BPF_CGROUP_SOCK_OPS, etc.
     (``BPF_MAP_TYPE_CGRP_STORAGE`` 可被所有程序类型使用，而
     ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` 仅对 cgroup 程序类型
     (如 BPF_CGROUP_INET_INGRESS、BPF_CGROUP_SOCK_OPS 等)可用。)

(2). ``BPF_MAP_TYPE_CGRP_STORAGE`` supports local storage for more than one
     cgroup while ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` only supports one cgroup
     which is attached by a BPF program.
     (``BPF_MAP_TYPE_CGRP_STORAGE`` 支持为多个 cgroup 提供本地存储，而
     ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` 仅支持由 BPF 程序附加的
     那一个 cgroup。)

(3). ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` allocates local storage at attach time so
     ``bpf_get_local_storage()`` always returns non-NULL local storage.
     ``BPF_MAP_TYPE_CGRP_STORAGE`` allocates local storage at runtime so
     it is possible that ``bpf_cgrp_storage_get()`` may return null local storage.
     To avoid such null local storage issue, user space can do
     ``bpf_map_update_elem()`` to pre-allocate local storage before a BPF program
     is attached.
     (``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` 在附加时分配本地存储，
     因此 ``bpf_get_local_storage()`` 总是返回非 NULL 的本地存储。
     ``BPF_MAP_TYPE_CGRP_STORAGE`` 在运行时分配本地存储，因此
     ``bpf_cgrp_storage_get()`` 可能返回空的本地存储。为避免这种空本地
     存储问题，用户态可以在 BPF 程序被附加之前调用
     ``bpf_map_update_elem()`` 预分配本地存储。)

(4). ``BPF_MAP_TYPE_CGRP_STORAGE`` supports deleting local storage by a BPF program
     while ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` only deletes storage during
     prog detach time.
     (``BPF_MAP_TYPE_CGRP_STORAGE`` 支持由 BPF 程序删除本地存储，而
     ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` 仅在程序分离时删除存储。)

So overall, ``BPF_MAP_TYPE_CGRP_STORAGE`` supports all ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED``
functionality and beyond. It is recommended to use ``BPF_MAP_TYPE_CGRP_STORAGE``
instead of ``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED``.
(总体而言，``BPF_MAP_TYPE_CGRP_STORAGE`` 支持
``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED`` 的全部功能且有过之而无不及。
建议使用 ``BPF_MAP_TYPE_CGRP_STORAGE`` 而不是
``BPF_MAP_TYPE_CGROUP_STORAGE_DEPRECATED``。)
