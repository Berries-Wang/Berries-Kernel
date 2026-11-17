.. SPDX-License-Identifier: GPL-2.0

==================================
Netmem Support for Network Drivers
==================================

(网络驱动的 netmem 支持)

This document outlines the requirements for network drivers to support netmem,
an abstract memory type that enables features like device memory TCP. By
supporting netmem, drivers can work with various underlying memory types
with little to no modification.
(本文档概述网络驱动支持 netmem 所需满足的要求。netmem 是一种抽象内存
类型，可启用诸如设备内存 TCP (device memory TCP) 之类的特性。通过支持
netmem，驱动几乎无需修改即可与各种底层内存类型协同工作。)

Benefits of Netmem :
(netmem 的优点：)

* Flexibility: Netmem can be backed by different memory types (e.g., struct
  page, DMA-buf), allowing drivers to support various use cases such as device
  memory TCP.
  (灵活性： netmem 可以由不同的内存类型支持 (例如 struct page、DMA-buf)，
  使驱动能够支持诸如设备内存 TCP 等各种用例。)
* Future-proof: Drivers with netmem support are ready for upcoming
  features that rely on it.
  (面向未来： 支持 netmem 的驱动已为将来依赖它的特性做好准备。)
* Simplified Development: Drivers interact with a consistent API,
  regardless of the underlying memory implementation.
  (简化开发： 无论底层内存实现如何，驱动都与一致的 API 交互。)

Driver RX Requirements
(驱动 RX 要求)
======================

1. The driver must support page_pool.
   (驱动必须支持 page_pool。)

2. The driver must support the tcp-data-split ethtool option.
   (驱动必须支持 tcp-data-split ethtool 选项。)

3. The driver must use the page_pool netmem APIs for payload memory. The netmem
   APIs currently 1-to-1 correspond with page APIs. Conversion to netmem should
   be achievable by switching the page APIs to netmem APIs and tracking memory
   via netmem_refs in the driver rather than struct page * :
   (驱动必须使用 page_pool netmem API 来管理载荷 (payload) 内存。netmem
   API 目前与 page API 一一对应。只要把 page API 换成 netmem API，并在
   驱动中通过 netmem_refs 而不是 struct page * 来跟踪内存，就应该能够
   完成向 netmem 的转换：)

   - page_pool_alloc -> page_pool_alloc_netmem
   - page_pool_get_dma_addr -> page_pool_get_dma_addr_netmem
   - page_pool_put_page -> page_pool_put_netmem

   Not all page APIs have netmem equivalents at the moment. If your driver
   relies on a missing netmem API, feel free to add and propose to netdev@, or
   reach out to the maintainers and/or almasrymina@google.com for help adding
   the netmem API.
   (目前并非所有 page API 都有 netmem 等价物。如果你的驱动依赖某个缺失
   的 netmem API，欢迎自行添加并提议到 netdev@，或联系维护者和/或
   almasrymina@google.com 寻求添加该 netmem API 的帮助。)

4. The driver must use the following PP_FLAGS:
   (驱动必须使用以下 PP_FLAGS：)

   - PP_FLAG_DMA_MAP: netmem is not dma-mappable by the driver. The driver
     must delegate the dma mapping to the page_pool, which knows when
     dma-mapping is (or is not) appropriate.
     (PP_FLAG_DMA_MAP： netmem 不能由驱动进行 dma 映射。驱动必须把 dma
     映射委托给 page_pool，它知道何时适合 (或不适合) 进行 dma 映射。)
   - PP_FLAG_DMA_SYNC_DEV: netmem dma addr is not necessarily dma-syncable
     by the driver. The driver must delegate the dma syncing to the page_pool,
     which knows when dma-syncing is (or is not) appropriate.
     (PP_FLAG_DMA_SYNC_DEV： netmem 的 dma 地址不一定能由驱动进行 dma
     同步。驱动必须把 dma 同步委托给 page_pool，它知道何时适合 (或不适合)
     进行 dma 同步。)
   - PP_FLAG_ALLOW_UNREADABLE_NETMEM. The driver must specify this flag iff
     tcp-data-split is enabled.
     (PP_FLAG_ALLOW_UNREADABLE_NETMEM。当且仅当启用了 tcp-data-split 时，
     驱动必须指定此标志。)

5. The driver must not assume the netmem is readable and/or backed by pages.
   The netmem returned by the page_pool may be unreadable, in which case
   netmem_address() will return NULL. The driver must correctly handle
   unreadable netmem, i.e. don't attempt to handle its contents when
   netmem_address() is NULL.
   (驱动不得假定 netmem 是可读的和/或由 page 支撑的。page_pool 返回的
   netmem 可能不可读，此时 netmem_address() 会返回 NULL。驱动必须正确
   处理不可读的 netmem，即当 netmem_address() 为 NULL 时不要尝试处理其
   内容。)

   Ideally, drivers should not have to check the underlying netmem type via
   helpers like netmem_is_net_iov() or convert the netmem to any of its
   underlying types via netmem_to_page() or netmem_to_net_iov(). In most cases,
   netmem or page_pool helpers that abstract this complexity are provided
   (and more can be added).
   (理想情况下，驱动不应需要通过 netmem_is_net_iov() 之类的辅助函数去
   检查底层 netmem 类型，也不应通过 netmem_to_page() 或
   netmem_to_net_iov() 把 netmem 转换为其任何底层类型。在大多数情况下，
   已经提供了能够抽象这些复杂性的 netmem 或 page_pool 辅助函数 (并且
   可以添加更多)。)

6. The driver must use page_pool_dma_sync_netmem_for_cpu() in lieu of
   dma_sync_single_range_for_cpu(). For some memory providers, dma_syncing for
   CPU will be done by the page_pool, for others (particularly dmabuf memory
   provider), dma syncing for CPU is the responsibility of the userspace using
   dmabuf APIs. The driver must delegate the entire dma-syncing operation to
   the page_pool which will do it correctly.
   (驱动必须使用 page_pool_dma_sync_netmem_for_cpu() 来代替
   dma_sync_single_range_for_cpu()。对于某些内存提供者，针对 CPU 的
   dma 同步会由 page_pool 完成；对于其他提供者 (特别是 dmabuf 内存
   提供者)，针对 CPU 的 dma 同步是使用 dmabuf API 的用户空间的职责。
   驱动必须把整个 dma 同步操作委托给 page_pool，它会正确地完成。)

7. Avoid implementing driver-specific recycling on top of the page_pool. Drivers
   cannot hold onto a struct page to do their own recycling as the netmem may
   not be backed by a struct page. However, you may hold onto a page_pool
   reference with page_pool_fragment_netmem() or page_pool_ref_netmem() for
   that purpose, but be mindful that some netmem types might have longer
   circulation times, such as when userspace holds a reference in zerocopy
   scenarios.
   (避免在 page_pool 之上实现驱动特有的回收机制。驱动不能持有 struct page
   来做自己的回收，因为 netmem 可能不由 struct page 支撑。不过，你可以
   为此目的通过 page_pool_fragment_netmem() 或 page_pool_ref_netmem()
   持有 page_pool 引用，但要注意某些 netmem 类型的流转时间可能更长，
   例如在 zerocopy 场景中用户空间持有引用时。)

Driver TX Requirements
(驱动 TX 要求)
======================

1. The Driver must not pass the netmem dma_addr to any of the dma-mapping APIs
   directly. This is because netmem dma_addrs may come from a source like
   dma-buf that is not compatible with the dma-mapping APIs.
   (驱动不得把 netmem dma_addr 直接传给任何 dma 映射 API。这是因为
   netmem dma_addr 可能来自与 dma 映射 API 不兼容的来源，例如 dma-buf。)

   Helpers like netmem_dma_unmap_page_attrs() & netmem_dma_unmap_addr_set()
   should be used in lieu of dma_unmap_page[_attrs](), dma_unmap_addr_set().
   The netmem variants will handle netmem dma_addrs correctly regardless of the
   source, delegating to the dma-mapping APIs when appropriate.
   (应使用 netmem_dma_unmap_page_attrs() 与 netmem_dma_unmap_addr_set()
   之类的辅助函数来代替 dma_unmap_page[_attrs]()、dma_unmap_addr_set()。
   无论来源如何，netmem 变体都会正确处理 netmem dma_addr，并在适当时
   委托给 dma 映射 API。)

   Not all dma-mapping APIs have netmem equivalents at the moment. If your
   driver relies on a missing netmem API, feel free to add and propose to
   netdev@, or reach out to the maintainers and/or almasrymina@google.com for
   help adding the netmem API.
   (目前并非所有 dma 映射 API 都有 netmem 等价物。如果你的驱动依赖某个
   缺失的 netmem API，欢迎自行添加并提议到 netdev@，或联系维护者和/或
   almasrymina@google.com 寻求添加该 netmem API 的帮助。)

2. Driver should declare support by setting `netdev->netmem_tx = true`
   (驱动应通过设置 `netdev->netmem_tx = true` 来声明支持)
