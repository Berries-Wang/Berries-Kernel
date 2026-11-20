.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

=================================================================================================
BPF_MAP_TYPE_ARRAY and BPF_MAP_TYPE_PERCPU_ARRAY(BPF_MAP_TYPE_ARRAY 与 BPF_MAP_TYPE_PERCPU_ARRAY)
=================================================================================================

.. note::
   - ``BPF_MAP_TYPE_ARRAY`` was introduced in kernel version 3.19
     (``BPF_MAP_TYPE_ARRAY`` 在内核版本 3.19 中引入)
   - ``BPF_MAP_TYPE_PERCPU_ARRAY`` was introduced in version 4.6
     (``BPF_MAP_TYPE_PERCPU_ARRAY`` 在版本 4.6 中引入)

``BPF_MAP_TYPE_ARRAY`` and ``BPF_MAP_TYPE_PERCPU_ARRAY`` provide generic array
storage. The key type is an unsigned 32-bit integer (4 bytes) and the map is
of constant size. The size of the array is defined in ``max_entries`` at
creation time. All array elements are pre-allocated and zero initialized when
created. ``BPF_MAP_TYPE_PERCPU_ARRAY`` uses a different memory region for each
CPU whereas ``BPF_MAP_TYPE_ARRAY`` uses the same memory region. The value
stored can be of any size for ``BPF_MAP_TYPE_ARRAY`` and not more than
``PCPU_MIN_UNIT_SIZE`` (32 kB) for ``BPF_MAP_TYPE_PERCPU_ARRAY``. All
array elements are aligned to 8 bytes.
(``BPF_MAP_TYPE_ARRAY`` 与 ``BPF_MAP_TYPE_PERCPU_ARRAY`` 提供通用的数组存储。
键类型是 32 位无符号整数 (4 字节)，map 大小固定。数组大小在创建时由
``max_entries`` 定义。所有数组元素都预先分配并在创建时初始化为零。
``BPF_MAP_TYPE_PERCPU_ARRAY`` 为每个 CPU 使用不同的内存区域，而
``BPF_MAP_TYPE_ARRAY`` 使用同一块内存区域。对 ``BPF_MAP_TYPE_ARRAY`` 而言，
存储的值可以是任意大小；对 ``BPF_MAP_TYPE_PERCPU_ARRAY`` 则不能超过
``PCPU_MIN_UNIT_SIZE`` (32 kB)。所有数组元素都按 8 字节对齐。)

Since kernel 5.5, memory mapping may be enabled for ``BPF_MAP_TYPE_ARRAY`` by
setting the flag ``BPF_F_MMAPABLE``. The map definition is page-aligned and
starts on the first page. Sufficient page-sized and page-aligned blocks of
memory are allocated to store all array values, starting on the second page,
which in some cases will result in over-allocation of memory. The benefit of
using this is increased performance and ease of use since userspace programs
would not be required to use helper functions to access and mutate data.
(自内核 5.5 起，可以通过设置 ``BPF_F_MMAPABLE`` 标志为 ``BPF_MAP_TYPE_ARRAY``
启用内存映射。map 定义按页对齐并从第一页开始。从第二页开始，会分配足够多的
页大小且页对齐的内存块来存储所有数组值，某些情况下这会导致内存超量分配。
这样做的好处是性能更好、使用更方便，因为用户空间程序不再需要借助 helper
函数来访问和修改数据。)

Usage(用法)
===========

Kernel BPF(内核 BPF)
--------------------

bpf_map_lookup_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void *bpf_map_lookup_elem(struct bpf_map *map, const void *key)

Array elements can be retrieved using the ``bpf_map_lookup_elem()`` helper.
This helper returns a pointer into the array element, so to avoid data races
with userspace reading the value, the user must use primitives like
``__sync_fetch_and_add()`` when updating the value in-place.
(可以使用 ``bpf_map_lookup_elem()`` helper 检索数组元素。该 helper 返回指向
数组元素内部的指针，因此为避免与读取该值的用户空间发生数据竞争，用户在就地
更新值时必须使用 ``__sync_fetch_and_add()`` 之类的原语。)

bpf_map_update_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   long bpf_map_update_elem(struct bpf_map *map, const void *key, const void *value, u64 flags)

Array elements can be updated using the ``bpf_map_update_elem()`` helper.
(可以使用 ``bpf_map_update_elem()`` helper 更新数组元素。)

``bpf_map_update_elem()`` returns 0 on success, or negative error in case of
failure.
(``bpf_map_update_elem()`` 成功时返回 0，失败时返回负的错误码。)

Since the array is of constant size, ``bpf_map_delete_elem()`` is not supported.
To clear an array element, you may use ``bpf_map_update_elem()`` to insert a
zero value to that index.
(由于数组大小固定，不支持 ``bpf_map_delete_elem()``。要清空一个数组元素，可以
用 ``bpf_map_update_elem()`` 向该索引插入零值。)

Per CPU Array(每 CPU 数组)
--------------------------

Values stored in ``BPF_MAP_TYPE_ARRAY`` can be accessed by multiple programs
across different CPUs. To restrict storage to a single CPU, you may use a
``BPF_MAP_TYPE_PERCPU_ARRAY``.
(存储在 ``BPF_MAP_TYPE_ARRAY`` 中的值可以被跨不同 CPU 的多个程序访问。要把
存储限制在单个 CPU 上，可以使用 ``BPF_MAP_TYPE_PERCPU_ARRAY``。)

When using a ``BPF_MAP_TYPE_PERCPU_ARRAY`` the ``bpf_map_update_elem()`` and
``bpf_map_lookup_elem()`` helpers automatically access the slot for the current
CPU.
(使用 ``BPF_MAP_TYPE_PERCPU_ARRAY`` 时，``bpf_map_update_elem()`` 与
``bpf_map_lookup_elem()`` helper 会自动访问当前 CPU 的槽位。)

bpf_map_lookup_percpu_elem()(原名)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   void *bpf_map_lookup_percpu_elem(struct bpf_map *map, const void *key, u32 cpu)

The ``bpf_map_lookup_percpu_elem()`` helper can be used to lookup the array
value for a specific CPU. Returns value on success , or ``NULL`` if no entry was
found or ``cpu`` is invalid.
(``bpf_map_lookup_percpu_elem()`` helper 可用于查询特定 CPU 的数组值。成功时
返回该值；未找到条目或 ``cpu`` 无效时返回 ``NULL``。)

Concurrency(并发)
-----------------

Since kernel version 5.1, the BPF infrastructure provides ``struct bpf_spin_lock``
to synchronize access.
(自内核版本 5.1 起，BPF 基础设施提供 ``struct bpf_spin_lock`` 来同步访问。)

Userspace(用户空间)
-------------------

Access from userspace uses libbpf APIs with the same names as above, with
the map identified by its ``fd``.
(来自用户空间的访问使用与上文同名的 libbpf API，以 ``fd`` 标识 map。)

Examples(示例)
==============

Please see the ``tools/testing/selftests/bpf`` directory for functional
examples. The code samples below demonstrate API usage.
(功能性示例见 ``tools/testing/selftests/bpf`` 目录。下面的示例代码演示 API 的
用法。)

Kernel BPF(内核 BPF)
--------------------

This snippet shows how to declare an array in a BPF program.
(这段代码片段展示如何在 BPF 程序中声明一个数组。)

.. code-block:: c

    struct {
            __uint(type, BPF_MAP_TYPE_ARRAY);
            __type(key, u32);
            __type(value, long);
            __uint(max_entries, 256);
    } my_map SEC(".maps");


This example BPF program shows how to access an array element.
(这个示例 BPF 程序展示如何访问一个数组元素。)

.. code-block:: c

    int bpf_prog(struct __sk_buff *skb)
    {
            struct iphdr ip;
            int index;
            long *value;

            if (bpf_skb_load_bytes(skb, ETH_HLEN, &ip, sizeof(ip)) < 0)
                    return 0;

            index = ip.protocol;
            value = bpf_map_lookup_elem(&my_map, &index);
            if (value)
                    __sync_fetch_and_add(value, skb->len);

            return 0;
    }

Userspace(用户空间)
-------------------

BPF_MAP_TYPE_ARRAY(BPF_MAP_TYPE_ARRAY)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This snippet shows how to create an array, using ``bpf_map_create_opts`` to
set flags.
(这段代码片段展示如何创建一个数组，使用 ``bpf_map_create_opts`` 设置标志。)

.. code-block:: c

    #include <bpf/libbpf.h>
    #include <bpf/bpf.h>

    int create_array()
    {
            int fd;
            LIBBPF_OPTS(bpf_map_create_opts, opts, .map_flags = BPF_F_MMAPABLE);

            fd = bpf_map_create(BPF_MAP_TYPE_ARRAY,
                                "example_array",       /* name */
                                sizeof(__u32),         /* key size */
                                sizeof(long),          /* value size */
                                256,                   /* max entries */
                                &opts);                /* create opts */
            return fd;
    }

This snippet shows how to initialize the elements of an array.
(这段代码片段展示如何初始化一个数组的元素。)

.. code-block:: c

    int initialize_array(int fd)
    {
            __u32 i;
            long value;
            int ret;

            for (i = 0; i < 256; i++) {
                    value = i;
                    ret = bpf_map_update_elem(fd, &i, &value, BPF_ANY);
                    if (ret < 0)
                            return ret;
            }

            return ret;
    }

This snippet shows how to retrieve an element value from an array.
(这段代码片段展示如何从数组中检索一个元素的值。)

.. code-block:: c

    int lookup(int fd)
    {
            __u32 index = 42;
            long value;
            int ret;

            ret = bpf_map_lookup_elem(fd, &index, &value);
            if (ret < 0)
                    return ret;

            /* use value here */
            assert(value == 42);

            return ret;
    }

BPF_MAP_TYPE_PERCPU_ARRAY(BPF_MAP_TYPE_PERCPU_ARRAY)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This snippet shows how to initialize the elements of a per CPU array.
(这段代码片段展示如何初始化一个每 CPU 数组的元素。)

.. code-block:: c

    int initialize_array(int fd)
    {
            int ncpus = libbpf_num_possible_cpus();
            long values[ncpus];
            __u32 i, j;
            int ret;

            for (i = 0; i < 256 ; i++) {
                    for (j = 0; j < ncpus; j++)
                            values[j] = i;
                    ret = bpf_map_update_elem(fd, &i, &values, BPF_ANY);
                    if (ret < 0)
                            return ret;
            }

            return ret;
    }

This snippet shows how to access the per CPU elements of an array value.
(这段代码片段展示如何访问一个数组值的各 CPU 元素。)

.. code-block:: c

    int lookup(int fd)
    {
            int ncpus = libbpf_num_possible_cpus();
            __u32 index = 42, j;
            long values[ncpus];
            int ret;

            ret = bpf_map_lookup_elem(fd, &index, &values);
            if (ret < 0)
                    return ret;

            for (j = 0; j < ncpus; j++) {
                    /* Use per CPU value here */
                    assert(values[j] == 42);
            }

            return ret;
    }

Semantics(语义)
===============

As shown in the example above, when accessing a ``BPF_MAP_TYPE_PERCPU_ARRAY``
in userspace, each value is an array with ``ncpus`` elements.
(如上面的例子所示，在用户空间访问 ``BPF_MAP_TYPE_PERCPU_ARRAY`` 时，每个值都是
一个包含 ``ncpus`` 个元素的数组。)

When calling ``bpf_map_update_elem()`` the flag ``BPF_NOEXIST`` can not be used
for these maps.
(调用 ``bpf_map_update_elem()`` 时，这类 map 不能使用 ``BPF_NOEXIST`` 标志。)
