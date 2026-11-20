.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

=================================================================================================
BPF_MAP_TYPE_DEVMAP and BPF_MAP_TYPE_DEVMAP_HASH(BPF_MAP_TYPE_DEVMAP 与 BPF_MAP_TYPE_DEVMAP_HASH)
=================================================================================================

.. note::
   - ``BPF_MAP_TYPE_DEVMAP`` was introduced in kernel version 4.14
   - ``BPF_MAP_TYPE_DEVMAP_HASH`` was introduced in kernel version 5.4

   (``BPF_MAP_TYPE_DEVMAP`` 在内核版本 4.14 中引入；``BPF_MAP_TYPE_DEVMAP_HASH`` 在内核版本 5.4 中引入。)

``BPF_MAP_TYPE_DEVMAP`` and ``BPF_MAP_TYPE_DEVMAP_HASH`` are BPF maps primarily
used as backend maps for the XDP BPF helper call ``bpf_redirect_map()``.
``BPF_MAP_TYPE_DEVMAP`` is backed by an array that uses the key as
the index to lookup a reference to a net device. While ``BPF_MAP_TYPE_DEVMAP_HASH``
is backed by a hash table that uses a key to lookup a reference to a net device.
The user provides either <``key``/ ``ifindex``> or <``key``/ ``struct bpf_devmap_val``>
pairs to update the maps with new net devices.
(``BPF_MAP_TYPE_DEVMAP`` 与 ``BPF_MAP_TYPE_DEVMAP_HASH`` 是主要用作 XDP BPF 辅助函数调用 ``bpf_redirect_map()`` 后端 map 的 BPF map。``BPF_MAP_TYPE_DEVMAP`` 由一个数组支持，以键作为索引来查找对网络设备的引用；而 ``BPF_MAP_TYPE_DEVMAP_HASH``
由哈希表支持，使用键来查找对网络设备的引用。用户提供 <``key``/ ``ifindex``> 或 <``key``/ ``struct bpf_devmap_val``>
对来用新的网络设备更新这些 map。)

.. note::
    - The key to a hash map doesn't have to be an ``ifindex``.
    - While ``BPF_MAP_TYPE_DEVMAP_HASH`` allows for densely packing the net devices
      it comes at the cost of a hash of the key when performing a look up.

    (哈希 map 的键不必是 ``ifindex`` ；虽然 ``BPF_MAP_TYPE_DEVMAP_HASH`` 允许紧凑地打包网络设备，但代价是查找时需要对键做一次哈希。)

The setup and packet enqueue/send code is shared between the two types of
devmap; only the lookup and insertion is different.
(两种 devmap 共享设置与数据包入队/发送代码；只有查找与插入不同。)

Usage(用法)
===========
Kernel BPF(内核 BPF)
--------------------
bpf_redirect_map()
^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_redirect_map(struct bpf_map *map, u32 key, u64 flags)

Redirect the packet to the endpoint referenced by ``map`` at index ``key``.
For ``BPF_MAP_TYPE_DEVMAP`` and ``BPF_MAP_TYPE_DEVMAP_HASH`` this map contains
references to net devices (for forwarding packets through other ports).
(把数据包重定向到 ``map`` 中索引 ``key`` 所引用的端点。对 ``BPF_MAP_TYPE_DEVMAP`` 与 ``BPF_MAP_TYPE_DEVMAP_HASH`` 而言，该 map 包含对网络设备的引用 (用于把数据包从其他端口转发出去)。)

The lower two bits of *flags* are used as the return code if the map lookup
fails. This is so that the return value can be one of the XDP program return
codes up to ``XDP_TX``, as chosen by the caller. The higher bits of ``flags``
can be set to ``BPF_F_BROADCAST`` or ``BPF_F_EXCLUDE_INGRESS`` as defined
below.
(*flags* 的低两位在 map 查找失败时用作返回码。这样返回值可以是由调用者选择的、直到 ``XDP_TX`` 在内的某个 XDP 程序返回码。``flags`` 的高位可以设为 ``BPF_F_BROADCAST`` 或 ``BPF_F_EXCLUDE_INGRESS``，定义见下文。)

With ``BPF_F_BROADCAST`` the packet will be broadcast to all the interfaces
in the map, with ``BPF_F_EXCLUDE_INGRESS`` the ingress interface will be excluded
from the broadcast.
(使用 ``BPF_F_BROADCAST`` 时，数据包将广播给 map 中的所有接口；使用 ``BPF_F_EXCLUDE_INGRESS`` 时，入口接口将被排除在广播之外。)

.. note::
    - The key is ignored if BPF_F_BROADCAST is set.
    - The broadcast feature can also be used to implement multicast forwarding:
      simply create multiple DEVMAPs, each one corresponding to a single multicast group.

    (如果设置了 BPF_F_BROADCAST，则键被忽略；广播特性也可用于实现组播转发：只需创建多个 DEVMAP，每个对应一个组播组。)

This helper will return ``XDP_REDIRECT`` on success, or the value of the two
lower bits of the ``flags`` argument if the map lookup fails.
(成功时该辅助函数返回 ``XDP_REDIRECT``；如果 map 查找失败，则返回 ``flags`` 参数低两位的值。)

More information about redirection can be found :doc:`redirect`
(更多关于重定向的信息见 :doc:`redirect` 。)

bpf_map_lookup_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

   void *bpf_map_lookup_elem(struct bpf_map *map, const void *key)

Net device entries can be retrieved using the ``bpf_map_lookup_elem()``
helper.
(可以使用 ``bpf_map_lookup_elem()`` 辅助函数检索网络设备条目。)

User space(用户态)
------------------
.. note::
    DEVMAP entries can only be updated/deleted from user space and not
    from an eBPF program. Trying to call these functions from a kernel eBPF
    program will result in the program failing to load and a verifier warning.
    (DEVMAP 条目只能从用户态更新/删除，不能从 eBPF 程序中操作。试图从内核 eBPF 程序中调用这些函数会导致程序加载失败并给出验证器警告。)

bpf_map_update_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

   int bpf_map_update_elem(int fd, const void *key, const void *value, __u64 flags);

Net device entries can be added or updated using the ``bpf_map_update_elem()``
helper. This helper replaces existing elements atomically. The ``value`` parameter
can be ``struct bpf_devmap_val`` or a simple ``int ifindex`` for backwards
compatibility.
(可以使用 ``bpf_map_update_elem()`` 辅助函数添加或更新网络设备条目。该辅助函数原子地替换现有元素。``value`` 参数可以是 ``struct bpf_devmap_val``，或者为了向后兼容而使用简单的 ``int ifindex``。)

 .. code-block:: c

    struct bpf_devmap_val {
        __u32 ifindex;   /* device index */
        union {
            int   fd;  /* prog fd on map write */
            __u32 id;  /* prog id on map read */
        } bpf_prog;
    };

The ``flags`` argument can be one of the following:( ``flags`` 参数可以是以下之一： ``BPF_ANY`` 创建新元素或更新现有元素； ``BPF_NOEXIST`` 仅当元素不存在时创建新元素； ``BPF_EXIST`` 更新现有元素。)
  - ``BPF_ANY``: Create a new element or update an existing element.
  - ``BPF_NOEXIST``: Create a new element only if it did not exist.
  - ``BPF_EXIST``: Update an existing element.

DEVMAPs can associate a program with a device entry by adding a ``bpf_prog.fd``
to ``struct bpf_devmap_val``. Programs are run after ``XDP_REDIRECT`` and have
access to both Rx device and Tx device. The  program associated with the ``fd``
must have type XDP with expected attach type ``xdp_devmap``.
When a program is associated with a device index, the program is run on an
``XDP_REDIRECT`` and before the buffer is added to the per-cpu queue. Examples
of how to attach/use xdp_devmap progs can be found in the kernel selftests:
(DEVMAP 可以通过在 ``struct bpf_devmap_val`` 中加入 ``bpf_prog.fd`` 把一个程序与设备条目关联起来。程序在 ``XDP_REDIRECT`` 之后运行，并且可以访问 Rx 设备与 Tx 设备。与 ``fd`` 关联的程序必须是 XDP 类型，且期望的挂接类型为 ``xdp_devmap`` 。
当程序与设备索引关联后，它会在 ``XDP_REDIRECT`` 时、且在缓冲加入 per-cpu 队列之前运行。如何挂接/使用 xdp_devmap 程序的例子见内核自测：)

- ``tools/testing/selftests/bpf/prog_tests/xdp_devmap_attach.c``
- ``tools/testing/selftests/bpf/progs/test_xdp_with_devmap_helpers.c``

bpf_map_lookup_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

.. c:function::
   int bpf_map_lookup_elem(int fd, const void *key, void *value);

Net device entries can be retrieved using the ``bpf_map_lookup_elem()``
helper.
(可以使用 ``bpf_map_lookup_elem()`` 辅助函数检索网络设备条目。)

bpf_map_delete_elem()
^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

.. c:function::
   int bpf_map_delete_elem(int fd, const void *key);

Net device entries can be deleted using the ``bpf_map_delete_elem()``
helper. This helper will return 0 on success, or negative error in case of
failure.
(可以使用 ``bpf_map_delete_elem()`` 辅助函数删除网络设备条目。成功时该辅助函数返回 0，失败时返回负的错误码。)

Examples(示例)
==============

Kernel BPF(内核 BPF)
--------------------

The following code snippet shows how to declare a ``BPF_MAP_TYPE_DEVMAP``
called tx_port.
(下面的代码片段展示如何声明一个名为 tx_port 的 ``BPF_MAP_TYPE_DEVMAP``。)

.. code-block:: c

    struct {
        __uint(type, BPF_MAP_TYPE_DEVMAP);
        __type(key, __u32);
        __type(value, __u32);
        __uint(max_entries, 256);
    } tx_port SEC(".maps");

The following code snippet shows how to declare a ``BPF_MAP_TYPE_DEVMAP_HASH``
called forward_map.
(下面的代码片段展示如何声明一个名为 forward_map 的 ``BPF_MAP_TYPE_DEVMAP_HASH``。)

.. code-block:: c

    struct {
        __uint(type, BPF_MAP_TYPE_DEVMAP_HASH);
        __type(key, __u32);
        __type(value, struct bpf_devmap_val);
        __uint(max_entries, 32);
    } forward_map SEC(".maps");

.. note::

    The value type in the DEVMAP above is a ``struct bpf_devmap_val``
    (上面 DEVMAP 的值类型是 ``struct bpf_devmap_val`` 。)

The following code snippet shows a simple xdp_redirect_map program. This program
would work with a user space program that populates the devmap ``forward_map`` based
on ingress ifindexes. The BPF program (below) is redirecting packets using the
ingress ``ifindex`` as the ``key``.
(下面的代码片段展示一个简单的 xdp_redirect_map 程序。该程序可与一个基于入口 ifindex 填充 devmap ``forward_map`` 的用户态程序配合工作。下面的 BPF 程序使用入口 ``ifindex`` 作为 ``key`` 来重定向数据包。)

.. code-block:: c

    SEC("xdp")
    int xdp_redirect_map_func(struct xdp_md *ctx)
    {
        int index = ctx->ingress_ifindex;

        return bpf_redirect_map(&forward_map, index, 0);
    }

The following code snippet shows a BPF program that is broadcasting packets to
all the interfaces in the ``tx_port`` devmap.
(下面的代码片段展示一个把数据包广播到 ``tx_port`` devmap 中所有接口的 BPF 程序。)

.. code-block:: c

    SEC("xdp")
    int xdp_redirect_map_func(struct xdp_md *ctx)
    {
        return bpf_redirect_map(&tx_port, 0, BPF_F_BROADCAST | BPF_F_EXCLUDE_INGRESS);
    }

User space(用户态)
------------------

The following code snippet shows how to update a devmap called ``tx_port``.
(下面的代码片段展示如何更新名为 ``tx_port`` 的 devmap。)

.. code-block:: c

    int update_devmap(int ifindex, int redirect_ifindex)
    {
        int ret;

        ret = bpf_map_update_elem(bpf_map__fd(tx_port), &ifindex, &redirect_ifindex, 0);
        if (ret < 0) {
            fprintf(stderr, "Failed to update devmap_ value: %s\n",
                strerror(errno));
        }

        return ret;
    }

The following code snippet shows how to update a hash_devmap called ``forward_map``.
(下面的代码片段展示如何更新名为 ``forward_map`` 的 hash_devmap。)

.. code-block:: c

    int update_devmap(int ifindex, int redirect_ifindex)
    {
        struct bpf_devmap_val devmap_val = { .ifindex = redirect_ifindex };
        int ret;

        ret = bpf_map_update_elem(bpf_map__fd(forward_map), &ifindex, &devmap_val, 0);
        if (ret < 0) {
            fprintf(stderr, "Failed to update devmap_ value: %s\n",
                strerror(errno));
        }
        return ret;
    }

References(参考资料)
====================

- https://lwn.net/Articles/728146/
- https://git.kernel.org/pub/scm/linux/kernel/git/bpf/bpf-next.git/commit/?id=6f9d451ab1a33728adb72d7ff66a7b374d665176
- https://elixir.bootlin.com/linux/latest/source/net/core/filter.c#L4106
