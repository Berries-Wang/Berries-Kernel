.. SPDX-License-Identifier: GPL-2.0

==================================================
Allocating dma-buf using heaps(使用堆分配 dma-buf)
==================================================

Dma-buf Heaps are a way for userspace to allocate dma-buf objects. They are
typically used to allocate buffers from a specific allocation pool, or to share
buffers across frameworks.
(Dma-buf 堆 (heap) 是用户空间分配 dma-buf 对象的一种方式。它们通常用
于从特定的分配池分配缓冲区，或在各个框架之间共享缓冲区。)

Heaps(堆)
=========

A heap represents a specific allocator. The Linux kernel currently supports the
following heaps:
(一个堆代表一个特定的分配器。Linux 内核当前支持以下堆：)

 - The ``system`` heap allocates virtually contiguous, cacheable, buffers.
   ( ``system`` 堆分配虚拟连续、可缓存的缓冲区。)

 - The ``system_cc_shared`` heap allocates virtually contiguous, cacheable,
   buffers using shared (decrypted) memory. It is only present on
   confidential computing (CoCo) VMs where memory encryption is active
   (e.g., AMD SEV, Intel TDX). The allocated pages have the encryption
   bit cleared, making them accessible for device DMA without TDISP
   support. On non-CoCo VM configurations, this heap is not registered.
   ( ``system_cc_shared`` 堆使用共享的 (解密的) 内存分配虚拟连续、可缓
   存的缓冲区。它只存在于启用了内存加密的机密计算 (CoCo) VM 上 (例如
   AMD SEV、Intel TDX)。所分配页面的加密位被清除，使其无需 TDISP 支持
   即可供设备 DMA 访问。在非 CoCo VM 配置上，此堆不会被注册。)

 - The ``default_cma_region`` heap allocates physically contiguous,
   cacheable, buffers. Only present if a CMA region is present. Such a
   region is usually created either through the kernel commandline
   through the ``cma`` parameter, a memory region Device-Tree node with
   the ``linux,cma-default`` property set, or through the
   ``CMA_SIZE_MBYTES`` or ``CMA_SIZE_PERCENTAGE`` Kconfig options. Prior
   to Linux 6.17, its name wasn't stable and could be called
   ``reserved``, ``linux,cma``, or ``default-pool``, depending on the
   platform.
   ( ``default_cma_region`` 堆分配物理连续、可缓存的缓冲区。仅当存在
   CMA 区域时才存在。这样的区域通常通过内核命令行的 ``cma`` 参数、设
   置了 ``linux,cma-default`` 属性的内存区域 Device-Tree 节点，或通过
   ``CMA_SIZE_MBYTES`` 或 ``CMA_SIZE_PERCENTAGE`` Kconfig 选项创建。
   在 Linux 6.17 之前，它的名称并不稳定，可能被称作 ``reserved`` 、
   ``linux,cma`` 或 ``default-pool`` ，具体取决于平台。)

 - A heap will be created for each reusable region in the device tree
   with the ``shared-dma-pool`` compatible, using the full device tree
   node name as its name. The buffer semantics are identical to
   ``default-cma-region``.
   (对于设备树中每个带有 ``shared-dma-pool`` compatible 的可重用区域，
   都会创建一个堆，并以完整的设备树节点名作为其名称。缓冲区语义与
   ``default-cma-region`` 相同。)

Naming Convention(命名约定)
===========================

``dma-buf`` heaps name should meet a number of constraints:
( ``dma-buf`` 堆的名称应满足许多约束：)

- The name must be stable, and must not change from one version to the other.
  Userspace identifies heaps by their name, so if the names ever change, we
  would be likely to introduce regressions.
  (名称必须稳定，并且不能随版本改变。用户空间通过名称来标识堆，因此
  如果名称发生变化，我们很可能会引入回归。)

- The name must describe the memory region the heap will allocate from, and
  must uniquely identify it in a given platform. Since userspace applications
  use the heap name as the discriminant, it must be able to tell which heap it
  wants to use reliably if there's multiple heaps.
  (名称必须描述堆将从中分配的内存区域，并且必须在给定平台上唯一地标
  识它。由于用户空间应用程序使用堆名称作为判别依据，因此当存在多个
  堆时，它必须能够可靠地判断要使用哪个堆。)

- The name must not mention implementation details, such as the allocator. The
  heap driver will change over time, and implementation details when it was
  introduced might not be relevant in the future.
  (名称不得提及实现细节，例如分配器。堆驱动程序会随时间变化，引入它
  时的实现细节在未来可能不再相关。)

- The name should describe properties of the buffers that would be allocated.
  Doing so will make heap identification easier for userspace. Such properties
  are:
  (名称应描述将分配的缓冲区的属性。这样做可以使用户空间更容易识别
  堆。此类属性有：)

  - ``contiguous`` for physically contiguous buffers;
    ( ``contiguous`` 表示物理连续的缓冲区；)

  - ``protected`` for encrypted buffers not accessible the OS;
    ( ``protected`` 表示 OS 无法访问的加密缓冲区；)

- The name may describe intended usage. Doing so will make heap identification
  easier for userspace applications and users.
  (名称可以描述预期用途。这样做可以使用户空间应用程序和用户更容易
  识别堆。)

For example, assuming a platform with a reserved memory region located
at the RAM address 0x42000000, intended to allocate video framebuffers,
physically contiguous, and backed by the CMA kernel allocator, good
names would be ``memory@42000000-contiguous`` or ``video@42000000``, but
``cma-video`` wouldn't.
(例如，假设某个平台有一个位于 RAM 地址 0x42000000 的保留内存区域，用
于分配视频帧缓冲区、物理连续、并由 CMA 内核分配器支持，那么好的名称
是 ``memory@42000000-contiguous`` 或 ``video@42000000`` ，而
``cma-video`` 则不是。)
