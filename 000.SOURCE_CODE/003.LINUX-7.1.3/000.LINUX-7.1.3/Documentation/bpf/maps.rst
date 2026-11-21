
==================
BPF maps(BPF 映射)
==================

BPF 'maps' provide generic storage of different types for sharing data between
kernel and user space. There are several storage types available, including
hash, array, bloom filter and radix-tree. Several of the map types exist to
support specific BPF helpers that perform actions based on the map contents. The
maps are accessed from BPF programs via BPF helpers which are documented in the
`man-pages`_ for `bpf-helpers(7)`_.
(BPF "map" 提供不同类型的通用存储，用于在内核与用户空间之间共享数据。
有多种存储类型可用，包括哈希、数组、布隆过滤器 (bloom filter) 和基数
树 (radix-tree)。其中一些 map 类型的存在，是为了支持基于 map 内容执行
操作的特定 BPF 辅助函数。BPF 程序通过 BPF 辅助函数访问 map，这些辅助
函数在 `bpf-helpers(7)`_ 的 `man-pages`_ 中有文档。)

BPF maps are accessed from user space via the ``bpf`` syscall, which provides
commands to create maps, lookup elements, update elements and delete elements.
More details of the BPF syscall are available in `ebpf-syscall`_ and in the
`man-pages`_ for `bpf(2)`_.
(用户空间通过 ``bpf`` 系统调用访问 BPF map，该系统调用提供创建 map、查
找元素、更新元素和删除元素的命令。BPF 系统调用的更多细节见
`ebpf-syscall`_ 以及 `bpf(2)`_ 的 `man-pages`_ 。)

Map Types(map 类型)
===================

.. toctree::
   :maxdepth: 1
   :glob:

   map_*

Usage Notes(使用说明)
=====================

.. c:function::
   int bpf(int command, union bpf_attr *attr, u32 size)

Use the ``bpf()`` system call to perform the operation specified by
``command``. The operation takes parameters provided in ``attr``. The ``size``
argument is the size of the ``union bpf_attr`` in ``attr``.
(使用 ``bpf()`` 系统调用执行 ``command`` 指定的操作。该操作的参数由
``attr`` 提供。 ``size`` 参数是 ``attr`` 中 ``union bpf_attr`` 的大小。)

**BPF_MAP_CREATE**

Create a map with the desired type and attributes in ``attr``:
(在 ``attr`` 中以所需的类型和属性创建一个 map：)

.. code-block:: c

    int fd;
    union bpf_attr attr = {
            .map_type = BPF_MAP_TYPE_ARRAY;  /* mandatory */
            .key_size = sizeof(__u32);       /* mandatory */
            .value_size = sizeof(__u32);     /* mandatory */
            .max_entries = 256;              /* mandatory */
            .map_flags = BPF_F_MMAPABLE;
            .map_name = "example_array";
    };

    fd = bpf(BPF_MAP_CREATE, &attr, sizeof(attr));

Returns a process-local file descriptor on success, or negative error in case of
failure. The map can be deleted by calling ``close(fd)``. Maps held by open
file descriptors will be deleted automatically when a process exits.
(成功时返回一个进程本地的文件描述符，失败时返回负的错误码。可以通过
调用 ``close(fd)`` 删除该 map。当进程退出时，由打开的文件描述符持有的
map 会被自动删除。)

.. note:: Valid characters for ``map_name`` are ``A-Z``, ``a-z``, ``0-9``,
   ``'_'`` and ``'.'``.
   ( ``map_name`` 的有效字符为 ``A-Z`` 、 ``a-z`` 、 ``0-9`` 、
   ``'_'`` 和 ``'.'`` 。)

**BPF_MAP_LOOKUP_ELEM**

Lookup key in a given map using ``attr->map_fd``, ``attr->key``,
``attr->value``. Returns zero and stores found elem into ``attr->value`` on
success, or negative error on failure.
(使用 ``attr->map_fd`` 、 ``attr->key`` 、 ``attr->value`` 在给定 map
中查找键。成功时返回零并把找到的元素存入 ``attr->value`` ，失败时返回
负的错误码。)

**BPF_MAP_UPDATE_ELEM**

Create or update key/value pair in a given map using ``attr->map_fd``, ``attr->key``,
``attr->value``. Returns zero on success or negative error on failure.
(使用 ``attr->map_fd`` 、 ``attr->key`` 、 ``attr->value`` 在给定 map
中创建或更新键/值对。成功时返回零，失败时返回负的错误码。)

**BPF_MAP_DELETE_ELEM**

Find and delete element by key in a given map using ``attr->map_fd``,
``attr->key``. Returns zero on success or negative error on failure.
(使用 ``attr->map_fd`` 、 ``attr->key`` 在给定 map 中按键查找并删除元
素。成功时返回零，失败时返回负的错误码。)

.. Links:
.. _man-pages: https://www.kernel.org/doc/man-pages/
.. _bpf(2): https://man7.org/linux/man-pages/man2/bpf.2.html
.. _bpf-helpers(7): https://man7.org/linux/man-pages/man7/bpf-helpers.7.html
.. _ebpf-syscall: https://docs.kernel.org/userspace-api/ebpf/syscall.html
