.. SPDX-License-Identifier: GPL-2.0-or-later

.. _kho-concepts:

==========================================
Kexec Handover Subsystem(Kexec 交接子系统)
==========================================

Overview(概述)
==============

Kexec HandOver (KHO) is a mechanism that allows Linux to preserve memory
regions, which could contain serialized system states, across kexec.
(Kexec HandOver (KHO) 是一种允许 Linux 跨 kexec 保留内存区域的机制，
这些区域可能包含序列化的系统状态。)

KHO uses :ref:`flattened device tree (FDT) <kho_fdt>` to pass information about
the preserved state from pre-exec kernel to post-kexec kernel and :ref:`scratch
memory regions <kho_scratch>` to ensure integrity of the preserved memory.
(KHO 使用 :ref:`flattened device tree (FDT) <kho_fdt>` 把有关已保留
状态的信息从 pre-exec 内核传递给 kexec 后的内核，并使用 :ref:`scratch
memory regions <kho_scratch>` 来确保被保留内存的完整性。)

.. _kho_fdt:

KHO FDT(KHO 扁平设备树)
=======================
Every KHO kexec carries a KHO specific flattened device tree (FDT) blob that
describes the preserved state. The FDT includes properties describing preserved
memory regions and nodes that hold subsystem specific state.
(每个 KHO kexec 都携带一个 KHO 特有的扁平设备树(FDT)blob，用于描述
被保留的状态。FDT 包含描述被保留内存区域的属性，以及保存子系统特有
状态的节点。)

The preserved memory regions contain either serialized subsystem states, or
in-memory data that shall not be touched across kexec. After KHO, subsystems
can retrieve and restore the preserved state from KHO FDT.
(被保留的内存区域要么包含序列化的子系统状态，要么包含在 kexec 之间
不应被触及的内存数据。KHO 之后，子系统可以从 KHO FDT 检索并恢复被
保留的状态。)

Subsystems participating in KHO can define their own format for state
serialization and preservation.
(参与 KHO 的子系统可以自定义用于状态序列化与保留的格式。)

KHO FDT and structures defined by the subsystems form an ABI between pre-kexec
and post-kexec kernels. This ABI is defined by header files in
``include/linux/kho/abi`` directory.
(KHO FDT 与子系统定义的结构构成了 pre-kexec 与 post-kexec 内核之间
的 ABI。该 ABI 由 ``include/linux/kho/abi`` 目录中的头文件定义。)

.. toctree::
   :maxdepth: 1

   abi.rst

.. _kho_scratch:

Scratch Regions(暂存区域)
=========================

To boot into kexec, we need to have a physically contiguous memory range that
contains no handed over memory. Kexec then places the target kernel and initrd
into that region. The new kernel exclusively uses this region for memory
allocations before during boot up to the initialization of the page allocator.
(要引导进入 kexec，我们需要一个物理连续且不包含任何被交接内存的内存
范围。kexec 随后把目标内核和 initrd 放入该区域。新内核在引导期间直到
页分配器初始化之前，都独占使用这个区域进行内存分配。)

We guarantee that we always have such regions through the scratch regions: On
first boot KHO allocates several physically contiguous memory regions. Since
after kexec these regions will be used by early memory allocations, there is a
scratch region per NUMA node plus a scratch region to satisfy allocations
requests that do not require particular NUMA node assignment.
By default, size of the scratch region is calculated based on amount of memory
allocated during boot. The ``kho_scratch`` kernel command line option may be
used to explicitly define size of the scratch regions.
The scratch regions are declared as CMA when page allocator is initialized so
that their memory can be used during system lifetime. CMA gives us the
guarantee that no handover pages land in that region, because handover pages
must be at a static physical memory location and CMA enforces that only
movable pages can be located inside.
(我们通过 scratch(暂存)区域保证始终有这样的区域：首次启动时 KHO
会分配几个物理连续的内存区域。由于 kexec 之后这些区域将被早期的内存
分配使用，因此每个 NUMA 节点有一个 scratch 区域，另加一个 scratch
区域来满足不需要特定 NUMA 节点指派的分配请求。默认情况下，scratch
区域的大小根据引导期间分配的内存量计算。可以使用 ``kho_scratch``
内核命令行选项显式定义 scratch 区域的大小。当页分配器初始化时，
scratch 区域被声明为 CMA，以便其内存可以在系统生命周期内使用。
CMA 保证没有交接页落入该区域，因为交接页必须位于静态物理内存位置，
而 CMA 强制只有可移动页才能位于其中。)

After KHO kexec, we ignore the ``kho_scratch`` kernel command line option and
instead reuse the exact same region that was originally allocated. This allows
us to recursively execute any amount of KHO kexecs. Because we used this region
for boot memory allocations and as target memory for kexec blobs, some parts
of that memory region may be reserved. These reservations are irrelevant for
the next KHO, because kexec can overwrite even the original kernel.
(KHO kexec 之后，我们会忽略 ``kho_scratch`` 内核命令行选项，转而复用
最初分配的那个区域。这使我们能够递归执行任意次数的 KHO kexec。因为
我们把这个区域用于引导内存分配，并作为 kexec blob 的目标内存，该内存
区域的部分内容可能已被保留。这些保留对下一次 KHO 无关紧要，因为
kexec 甚至可以覆盖原始内核。)

Kexec Handover Radix Tree(Kexec 交接基数树)
===========================================

.. kernel-doc:: include/linux/kho_radix_tree.h
  :doc: Kexec Handover Radix Tree

Public API(公开 API)
====================

.. kernel-doc:: kernel/liveupdate/kexec_handover.c
  :export:

See Also(参见)
==============

- :doc:`/admin-guide/mm/kho`
