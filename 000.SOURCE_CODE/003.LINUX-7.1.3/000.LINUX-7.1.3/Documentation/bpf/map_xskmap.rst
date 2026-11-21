.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

======================================
BPF_MAP_TYPE_XSKMAP(AF_XDP 套接字映射)
======================================

.. note::
   - ``BPF_MAP_TYPE_XSKMAP`` was introduced in kernel version 4.18
   (``BPF_MAP_TYPE_XSKMAP`` 在内核版本 4.18 中引入)

The ``BPF_MAP_TYPE_XSKMAP`` is used as a backend map for XDP BPF helper
call ``bpf_redirect_map()`` and ``XDP_REDIRECT`` action, like 'devmap' and 'cpumap'.
This map type redirects raw XDP frames to `AF_XDP`_ sockets (XSKs), a new type of
address family in the kernel that allows redirection of frames from a driver to
user space without having to traverse the full network stack. An AF_XDP socket
binds to a single netdev queue. A mapping of XSKs to queues is shown below:
(``BPF_MAP_TYPE_XSKMAP`` 用作 XDP BPF 辅助函数调用 ``bpf_redirect_map()`` 与 ``XDP_REDIRECT`` 动作的后端映射，就像 'devmap' 与 'cpumap' 一样。这种映射类型把原始 XDP 帧重定向到 `AF_XDP`_ 套接字 (XSK)。AF_XDP 是内核中一种新的地址族，允许把帧从驱动重定向到用户空间，而无需穿越完整的网络栈。一个 AF_XDP 套接字绑定到单个 netdev 队列。XSK 到队列的映射如下图所示：)

.. code-block:: none

    +---------------------------------------------------+
    |     xsk A      |     xsk B       |      xsk C     |<---+ User space
    =========================================================|==========
    |    Queue 0     |     Queue 1     |     Queue 2    |    |  Kernel
    +---------------------------------------------------+    |
    |                  Netdev eth0                      |    |
    +---------------------------------------------------+    |
    |                            +=============+        |    |
    |                            | key |  xsk  |        |    |
    |  +---------+               +=============+        |    |
    |  |         |               |  0  | xsk A |        |    |
    |  |         |               +-------------+        |    |
    |  |         |               |  1  | xsk B |        |    |
    |  | BPF     |-- redirect -->+-------------+-------------+
    |  | prog    |               |  2  | xsk C |        |
    |  |         |               +-------------+        |
    |  |         |                                      |
    |  |         |                                      |
    |  +---------+                                      |
    |                                                   |
    +---------------------------------------------------+

.. note::
    An AF_XDP socket that is bound to a certain <netdev/queue_id> will *only*
    accept XDP frames from that <netdev/queue_id>. If an XDP program tries to redirect
    from a <netdev/queue_id> other than what the socket is bound to, the frame will
    not be received on the socket.
    (绑定到某个特定 <netdev/queue_id> 的 AF_XDP 套接字*只*会接受来自该 <netdev/queue_id> 的 XDP 帧。如果 XDP 程序尝试从与套接字所绑定的 <netdev/queue_id> 不同的队列重定向，该帧将不会被套接字接收。)

Typically an XSKMAP is created per netdev. This map contains an array of XSK File
Descriptors (FDs). The number of array elements is typically set or adjusted using
the ``max_entries`` map parameter. For AF_XDP ``max_entries`` is equal to the number
of queues supported by the netdev.
(通常每个 netdev 会创建一个 XSKMAP。该映射包含一个由 XSK 文件描述符 (FD) 组成的数组。数组元素个数通常通过 ``max_entries`` 映射参数设置或调整。对 AF_XDP 而言，``max_entries`` 等于该 netdev 支持的队列数。)

.. note::
    Both the map key and map value size must be 4 bytes.
    (映射的键与值大小都必须是 4 字节。)

Usage(用法)
===========

Kernel BPF(内核侧 BPF)
----------------------
bpf_redirect_map()(重定向数据包)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    long bpf_redirect_map(struct bpf_map *map, u32 key, u64 flags)

Redirect the packet to the endpoint referenced by ``map`` at index ``key``.
For ``BPF_MAP_TYPE_XSKMAP`` this map contains references to XSK FDs
for sockets attached to a netdev's queues.
(把数据包重定向到 ``map`` 在索引 ``key`` 处引用的端点。对 ``BPF_MAP_TYPE_XSKMAP`` 而言，该映射包含对绑定到 netdev 各队列的套接字的 XSK FD 引用。)

.. note::
    If the map is empty at an index, the packet is dropped. This means that it is
    necessary to have an XDP program loaded with at least one XSK in the
    XSKMAP to be able to get any traffic to user space through the socket.
    (如果映射在某个索引处为空，数据包将被丢弃。这意味着必须加载一个 XDP 程序，且 XSKMAP 中至少有一个 XSK，才能通过套接字把流量送达用户空间。)

bpf_map_lookup_elem()(查找元素)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    void *bpf_map_lookup_elem(struct bpf_map *map, const void *key)

XSK entry references of type ``struct xdp_sock *`` can be retrieved using the
``bpf_map_lookup_elem()`` helper.
(可以使用 ``bpf_map_lookup_elem()`` 辅助函数获取 ``struct xdp_sock *`` 类型的 XSK 条目引用。)

User space(用户态)
------------------
.. note::
    XSK entries can only be updated/deleted from user space and not from
    a BPF program. Trying to call these functions from a kernel BPF program will
    result in the program failing to load and a verifier warning.
    (XSK 条目只能从用户空间更新/删除，不能从 BPF 程序中操作。从内核 BPF 程序中尝试调用这些函数会导致程序加载失败并产生验证器警告。)

bpf_map_update_elem()(更新元素)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

	int bpf_map_update_elem(int fd, const void *key, const void *value, __u64 flags)

XSK entries can be added or updated using the ``bpf_map_update_elem()``
helper. The ``key`` parameter is equal to the queue_id of the queue the XSK
is attaching to. And the ``value`` parameter is the FD value of that socket.
(可以使用 ``bpf_map_update_elem()`` 辅助函数添加或更新 XSK 条目。``key`` 参数等于 XSK 所绑定队列的 queue_id。``value`` 参数是该套接字的 FD 值。)

Under the hood, the XSKMAP update function uses the XSK FD value to retrieve the
associated ``struct xdp_sock`` instance.
(在底层，XSKMAP 的更新函数会用 XSK FD 值取回关联的 ``struct xdp_sock`` 实例。)

The flags argument can be one of the following:
(flags 参数可以是以下之一：)

- BPF_ANY: Create a new element or update an existing element.
  (BPF_ANY：创建新元素或更新已有元素。)
- BPF_NOEXIST: Create a new element only if it did not exist.
  (BPF_NOEXIST：仅当元素不存在时创建新元素。)
- BPF_EXIST: Update an existing element.
  (BPF_EXIST：更新已有元素。)

bpf_map_lookup_elem()(查找元素)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    int bpf_map_lookup_elem(int fd, const void *key, void *value)

Returns ``struct xdp_sock *`` or negative error in case of failure.
(成功返回 ``struct xdp_sock *``，失败时返回负的错误码。)

bpf_map_delete_elem()(删除元素)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
.. code-block:: c

    int bpf_map_delete_elem(int fd, const void *key)

XSK entries can be deleted using the ``bpf_map_delete_elem()``
helper. This helper will return 0 on success, or negative error in case of
failure.
(可以使用 ``bpf_map_delete_elem()`` 辅助函数删除 XSK 条目。该辅助函数成功时返回 0，失败时返回负的错误码。)

.. note::
    When `libxdp`_ deletes an XSK it also removes the associated socket
    entry from the XSKMAP.
    (当 `libxdp`_ 删除一个 XSK 时，它同时会把对应的套接字条目从 XSKMAP 中移除。)

Examples(示例)
==============
Kernel(内核)
------------

The following code snippet shows how to declare a ``BPF_MAP_TYPE_XSKMAP`` called
``xsks_map`` and how to redirect packets to an XSK.
(下面的代码片段展示如何声明一个名为 ``xsks_map`` 的 ``BPF_MAP_TYPE_XSKMAP``，以及如何把数据包重定向到 XSK。)

.. code-block:: c

	struct {
		__uint(type, BPF_MAP_TYPE_XSKMAP);
		__type(key, __u32);
		__type(value, __u32);
		__uint(max_entries, 64);
	} xsks_map SEC(".maps");


	SEC("xdp")
	int xsk_redir_prog(struct xdp_md *ctx)
	{
		__u32 index = ctx->rx_queue_index;

		if (bpf_map_lookup_elem(&xsks_map, &index))
			return bpf_redirect_map(&xsks_map, index, 0);
		return XDP_PASS;
	}

User space
----------

The following code snippet shows how to update an XSKMAP with an XSK entry.
(下面的代码片段展示如何用一个 XSK 条目更新 XSKMAP。)

.. code-block:: c

	int update_xsks_map(struct bpf_map *xsks_map, int queue_id, int xsk_fd)
	{
		int ret;

		ret = bpf_map_update_elem(bpf_map__fd(xsks_map), &queue_id, &xsk_fd, 0);
		if (ret < 0)
			fprintf(stderr, "Failed to update xsks_map: %s\n", strerror(errno));

		return ret;
	}

For an example on how create AF_XDP sockets, please see the AF_XDP-example and
AF_XDP-forwarding programs in the `bpf-examples`_ directory in the `libxdp`_ repository.
For a detailed explanation of the AF_XDP interface please see:
(关于如何创建 AF_XDP 套接字的示例，请参阅 `libxdp`_ 仓库中 `bpf-examples`_ 目录下的 AF_XDP-example 与 AF_XDP-forwarding 程序。关于 AF_XDP 接口的详细说明，请参阅：)

- `libxdp-readme`_.
  (libxdp 的 README 说明。)
- `AF_XDP`_ kernel documentation.
  (`AF_XDP`_ 内核文档。)

.. note::
    The most comprehensive resource for using XSKMAPs and AF_XDP is `libxdp`_.
    (使用 XSKMAP 与 AF_XDP 最全面的资料是 `libxdp`_。)

.. _libxdp: https://github.com/xdp-project/xdp-tools/tree/master/lib/libxdp
.. _AF_XDP: https://www.kernel.org/doc/html/latest/networking/af_xdp.html
.. _bpf-examples: https://github.com/xdp-project/bpf-examples
.. _libxdp-readme: https://github.com/xdp-project/xdp-tools/tree/master/lib/libxdp#using-af_xdp-sockets
