.. SPDX-License-Identifier: GPL-2.0

=============
Page Pool API
=============

(页池 (Page Pool) API)

.. kernel-doc:: include/net/page_pool/helpers.h
   :doc: page_pool allocator

Architecture overview
(架构概览)
=====================

.. code-block:: none

    +------------------+
    |       Driver     |
    +------------------+
            ^
            |
            |
            |
            v
    +--------------------------------------------+
    |                request memory              |
    +--------------------------------------------+
        ^                                  ^
        |                                  |
        | Pool empty                       | Pool has entries
        |                                  |
        v                                  v
    +-----------------------+     +------------------------+
    | alloc (and map) pages |     |  get page from cache   |
    +-----------------------+     +------------------------+
                                    ^                    ^
                                    |                    |
                                    | cache available    | No entries, refill
                                    |                    | from ptr-ring
                                    |                    |
                                    v                    v
                          +-----------------+     +------------------+
                          |   Fast cache    |     |  ptr-ring cache  |
                          +-----------------+     +------------------+

Monitoring
(监控)
==========
Information about page pools on the system can be accessed via the netdev
genetlink family (see Documentation/netlink/specs/netdev.yaml).
(系统中页池的相关信息可以通过 netdev generic netlink family 访问（参见
Documentation/netlink/specs/netdev.yaml）。)

API interface
(API 接口)
=============
The number of pools created **must** match the number of hardware queues
unless hardware restrictions make that impossible. This would otherwise beat the
purpose of page pool, which is allocate pages fast from cache without locking.
This lockless guarantee naturally comes from running under a NAPI softirq.
The protection doesn't strictly have to be NAPI, any guarantee that allocating
a page will cause no race conditions is enough.
(创建的页池数量**必须** 与硬件队列数量一致，除非硬件限制使其无法实现。否则就违背了页池的设计初衷——无锁地
从缓存中快速分配页。这种无锁保证自然来自在 NAPI softirq 下的运行。保护机制并不严格要求是 NAPI，任何能保证
分配页不会引发竞态条件的机制都足够了。)

.. kernel-doc:: net/core/page_pool.c
   :identifiers: page_pool_create

.. kernel-doc:: include/net/page_pool/types.h
   :identifiers: struct page_pool_params

.. kernel-doc:: include/net/page_pool/helpers.h
   :identifiers: page_pool_put_page page_pool_put_full_page
		 page_pool_recycle_direct page_pool_free_va
		 page_pool_dev_alloc_pages page_pool_dev_alloc_frag
		 page_pool_dev_alloc page_pool_dev_alloc_va
		 page_pool_get_dma_addr page_pool_get_dma_dir

.. kernel-doc:: net/core/page_pool.c
   :identifiers: page_pool_put_page_bulk page_pool_get_stats

DMA sync
(DMA 同步)
--------
Driver is always responsible for syncing the pages for the CPU.
Drivers may choose to take care of syncing for the device as well
or set the ``PP_FLAG_DMA_SYNC_DEV`` flag to request that pages
allocated from the page pool are already synced for the device.
(驱动始终负责为 CPU 侧同步页。驱动也可以选择同时负责设备侧的同步，或者设置 ``PP_FLAG_DMA_SYNC_DEV`` 标
志，要求从页池分配的页已针对设备完成同步。)

If ``PP_FLAG_DMA_SYNC_DEV`` is set, the driver must inform the core what portion
of the buffer has to be synced. This allows the core to avoid syncing the entire
page when the drivers knows that the device only accessed a portion of the page.
(如果设置了 ``PP_FLAG_DMA_SYNC_DEV`` ，驱动必须告知核心缓冲区的哪一部分需要同步。这样，当驱动知道设备只访
问了页的一部分时，核心就可以避免同步整个页。)

Most drivers will reserve headroom in front of the frame. This part
of the buffer is not touched by the device, so to avoid syncing
it drivers can set the ``offset`` field in struct page_pool_params
appropriately.
(大多数驱动会在帧前面预留 headroom（头部预留空间）。缓冲区的这一部分不会被设备触碰，因此为避免对其同步，驱
动可以恰当地设置 struct page_pool_params 中的 ``offset`` 字段。)

For pages recycled on the XDP xmit and skb paths the page pool will
use the ``max_len`` member of struct page_pool_params to decide how
much of the page needs to be synced (starting at ``offset``).
When directly freeing pages in the driver (page_pool_put_page())
the ``dma_sync_size`` argument specifies how much of the buffer needs
to be synced.
(对于在 XDP xmit 和 skb 路径上回收的页，页池将使用 struct page_pool_params 的 ``max_len`` 成员来决定页的
多大范围需要同步（从 ``offset`` 开始）。当在驱动中直接释放页（page_pool_put_page()）时，``dma_sync_size``
参数指定缓冲区需要同步的范围。)

If in doubt set ``offset`` to 0, ``max_len`` to ``PAGE_SIZE`` and
pass -1 as ``dma_sync_size``. That combination of arguments is always
correct.
(如有疑问，可将 ``offset`` 设为 0、``max_len`` 设为 ``PAGE_SIZE`` ，并将 -1 作为 ``dma_sync_size`` 传
入。这一参数组合始终是正确的。)

Note that the syncing parameters are for the entire page.
This is important to remember when using fragments (``PP_FLAG_PAGE_FRAG``),
where allocated buffers may be smaller than a full page.
Unless the driver author really understands page pool internals
it's recommended to always use ``offset = 0``, ``max_len = PAGE_SIZE``
with fragmented page pools.
(注意，这些同步参数是针对整个页的。在使用分片 (``PP_FLAG_PAGE_FRAG``) 时要牢记这一点，此时分配的缓冲区可能
小于整个页。除非驱动作者真正理解页池的内部机制，否则建议在分片页池中始终使用 ``offset = 0`` 和
``max_len = PAGE_SIZE`` 。)

Stats API and structures
(统计 API 与结构体)
------------------------
If the kernel is configured with ``CONFIG_PAGE_POOL_STATS=y``, the API
page_pool_get_stats() and structures described below are available.
It takes a  pointer to a ``struct page_pool`` and a pointer to a struct
page_pool_stats allocated by the caller.
(如果内核配置了 ``CONFIG_PAGE_POOL_STATS=y`` ，则可以使用下文描述的 page_pool_get_stats() API 及相关结构
体。它接受一个指向 ``struct page_pool`` 的指针和一个由调用者分配的 struct page_pool_stats 指针。)

Older drivers expose page pool statistics via ethtool or debugfs.
The same statistics are accessible via the netlink netdev family
in a driver-independent fashion.
(较老的驱动通过 ethtool 或 debugfs 暴露页池统计信息。同样的统计信息也可以通过 netlink netdev family 以与
驱动无关的方式获取。)

.. kernel-doc:: include/net/page_pool/types.h
   :identifiers: struct page_pool_recycle_stats
		 struct page_pool_alloc_stats
		 struct page_pool_stats

Coding examples
(编码示例)
===============

Registration
(注册)
------------

.. code-block:: c

    /* Page pool registration */
    struct page_pool_params pp_params = { 0 };
    struct xdp_rxq_info xdp_rxq;
    int err;

    pp_params.order = 0;
    /* internal DMA mapping in page_pool */
    pp_params.flags = PP_FLAG_DMA_MAP;
    pp_params.pool_size = DESC_NUM;
    pp_params.nid = NUMA_NO_NODE;
    pp_params.dev = priv->dev;
    pp_params.napi = napi; /* only if locking is tied to NAPI */
    pp_params.dma_dir = xdp_prog ? DMA_BIDIRECTIONAL : DMA_FROM_DEVICE;
    page_pool = page_pool_create(&pp_params);

    err = xdp_rxq_info_reg(&xdp_rxq, ndev, 0);
    if (err)
        goto err_out;

    err = xdp_rxq_info_reg_mem_model(&xdp_rxq, MEM_TYPE_PAGE_POOL, page_pool);
    if (err)
        goto err_out;

NAPI poller
(NAPI 轮询器)
-----------


.. code-block:: c

    /* NAPI Rx poller */
    enum dma_data_direction dma_dir;

    dma_dir = page_pool_get_dma_dir(dring->page_pool);
    while (done < budget) {
        if (some error)
            page_pool_recycle_direct(page_pool, page);
        if (packet_is_xdp) {
            if XDP_DROP:
                page_pool_recycle_direct(page_pool, page);
        } else (packet_is_skb) {
            skb_mark_for_recycle(skb);
            new_page = page_pool_dev_alloc_pages(page_pool);
        }
    }

Stats
(统计)
-----

.. code-block:: c

	#ifdef CONFIG_PAGE_POOL_STATS
	/* retrieve stats */
	struct page_pool_stats stats = { 0 };
	if (page_pool_get_stats(page_pool, &stats)) {
		/* perhaps the driver reports statistics with ethool */
		ethtool_print_allocation_stats(&stats.alloc_stats);
		ethtool_print_recycle_stats(&stats.recycle_stats);
	}
	#endif

Driver unload
(驱动卸载)
-------------

.. code-block:: c

    /* Driver unload */
    page_pool_put_full_page(page_pool, page, false);
    xdp_rxq_info_unreg(&xdp_rxq);
