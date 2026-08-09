==============================
Memory Layout on AArch64 Linux
==============================
> AArch64 Linux 内存布局
> ======================

Author: Catalin Marinas <catalin.marinas@arm.com>
> 作者：Catalin Marinas <catalin.marinas@arm.com>

This document describes the virtual memory layout used by the AArch64
Linux kernel. The architecture allows up to 4 levels of translation
tables with a 4KB page size and up to 3 levels with a 64KB page size.
> 本文档描述了 AArch64 Linux 内核使用的虚拟内存布局。该架构在 4KB 页大小下最多支持 4 级页表，在 64KB 页大小下最多支持 3 级页表。

AArch64 Linux uses either 3 levels or 4 levels of translation tables
with the 4KB page configuration, allowing 39-bit (512GB) or 48-bit
(256TB) virtual addresses, respectively, for both user and kernel. With
64KB pages, only 2 levels of translation tables, allowing 42-bit (4TB)
virtual address, are used but the memory layout is the same.
> AArch64 Linux 在 4KB 页配置下使用 3 级或 4 级页表，分别支持 39 位（512GB）或 48 位（256TB）的虚拟地址（用户空间和内核空间均适用）。在 64KB 页配置下，仅使用 2 级页表，支持 42 位（4TB）虚拟地址，但内存布局相同。

ARMv8.2 adds optional support for Large Virtual Address space. This is
only available when running with a 64KB page size and expands the
number of descriptors in the first level of translation.
> ARMv8.2 增加了对大型虚拟地址空间的可选支持。该特性仅在 64KB 页大小时可用，并扩展了第一级页表的描述符数量。

TTBRx selection is given by bit 55 of the virtual address. The
swapper_pg_dir contains only kernel (global) mappings while the user pgd
contains only user (non-global) mappings.  The swapper_pg_dir address is
written to TTBR1 and never written to TTBR0.
> TTBRx 的选择由虚拟地址的第 55 位决定。`swapper_pg_dir` 只包含内核（全局）映射，而用户 pgd 只包含用户（非全局）映射。`swapper_pg_dir` 的地址被写入 TTBR1，永远不会写入 TTBR0。

When using KVM without the Virtualization Host Extensions, the
hypervisor maps kernel pages in EL2 at a fixed (and potentially
random) offset from the linear mapping. See the kern_hyp_va macro and
kvm_update_va_mask function for more details. MMIO devices such as
GICv2 gets mapped next to the HYP idmap page, as do vectors when
ARM64_SPECTRE_V3A is enabled for particular CPUs.
> 当使用不带虚拟化主机扩展（VHE）的 KVM 时，hypervisor 在 EL2 中以相对于线性映射的固定（且可能随机化）偏移来映射内核页面。详情请参见 `kern_hyp_va` 宏和 `kvm_update_va_mask` 函数。MMIO 设备（如 GICv2）被映射到 HYP idmap 页的旁边；当特定 CPU 启用 `ARM64_SPECTRE_V3A` 时，向量表也同样映射到该区域。

When using KVM with the Virtualization Host Extensions, no additional
mappings are created, since the host kernel runs directly in EL2.
> 当使用带虚拟化主机扩展（VHE）的 KVM 时，不需要创建额外的映射，因为主机内核直接在 EL2 中运行。


52-bit VA support in the kernel
-------------------------------
> 内核中的 52 位虚拟地址支持
> ---------------------------

If the ARMv8.2-LVA optional feature is present, and we are running
with a 64KB page size; then it is possible to use 52-bits of address
space for both userspace and kernel addresses. However, any kernel
binary that supports 52-bit must also be able to fall back to 48-bit
at early boot time if the hardware feature is not present.
> 如果 ARMv8.2-LVA 可选特性存在，且我们以 64KB 页大小运行，则用户空间和内核地址都可以使用 52 位地址空间。但是，任何支持 52 位的内核二进制文件也必须在硬件特性不存在时，能够在早期引导阶段回退到 48 位。

This fallback mechanism necessitates the kernel .text to be in the
higher addresses such that they are invariant to 48/52-bit VAs. Due
to the kasan shadow being a fraction of the entire kernel VA space,
the end of the kasan shadow must also be in the higher half of the
kernel VA space for both 48/52-bit. (Switching from 48-bit to 52-bit,
the end of the kasan shadow is invariant and dependent on ~0UL,
whilst the start address will "grow" towards the lower addresses).
> 这种回退机制要求内核 .text 段位于较高的地址，使其对 48/52 位 VA 保持不变。由于 kasan shadow 区域是整个内核 VA 空间的一部分，kasan shadow 的结束地址也必须位于内核 VA 空间的高半部分（无论 48 位还是 52 位都如此）。（从 48 位切换到 52 位时，kasan shadow 的结束地址保持不变，依赖于 `~0UL`，而起始地址将向低地址方向"增长"）。

In order to optimise phys_to_virt and virt_to_phys, the PAGE_OFFSET
is kept constant at 0xFFF0000000000000 (corresponding to 52-bit),
this obviates the need for an extra variable read. The physvirt
offset and vmemmap offsets are computed at early boot to enable
this logic.
> 为了优化 `phys_to_virt` 和 `virt_to_phys`，`PAGE_OFFSET` 保持为常量 `0xFFF0000000000000`（对应 52 位），这消除了一次额外的变量读取。physvirt 偏移和 vmemmap 偏移在早期引导阶段被计算出来以支持此逻辑。

As a single binary will need to support both 48-bit and 52-bit VA
spaces, the VMEMMAP must be sized large enough for 52-bit VAs and
also must be sized large enough to accommodate a fixed PAGE_OFFSET.
> 由于单个二进制文件需要同时支持 48 位和 52 位 VA 空间，`VMEMMAP` 的尺寸必须足够大以容纳 52 位 VA，同时也必须足够大以容纳固定的 `PAGE_OFFSET`。

Most code in the kernel should not need to consider the VA_BITS, for
code that does need to know the VA size the variables are
defined as follows:
> 内核中的大多数代码不需要关心 `VA_BITS`；对于确实需要知道 VA 大小的代码，相关变量定义如下：

VA_BITS         constant        the *maximum* VA space size
> `VA_BITS`       常量            *最大* VA 空间大小

VA_BITS_MIN     constant        the *minimum* VA space size
> `VA_BITS_MIN`   常量            *最小* VA 空间大小

vabits_actual   variable        the *actual* VA space size
> `vabits_actual` 变量            *实际* VA 空间大小


Maximum and minimum sizes can be useful to ensure that buffers are
sized large enough or that addresses are positioned close enough for
the "worst" case.
> 最大和最小尺寸可用于确保缓冲区尺寸足够大，或确保地址位置在"最坏"情况下足够接近。


52-bit userspace VAs
--------------------
> 52 位用户空间虚拟地址
> ---------------------

To maintain compatibility with software that relies on the ARMv8.0
VA space maximum size of 48-bits, the kernel will, by default,
return virtual addresses to userspace from a 48-bit range.
> 为了与依赖 ARMv8.0 的 48 位最大 VA 空间尺寸的软件保持兼容，内核默认从 48 位范围内向用户空间返回虚拟地址。

Software can "opt-in" to receiving VAs from a 52-bit space by
specifying an mmap hint parameter that is larger than 48-bit.

For example:

.. code-block:: c

   maybe_high_address = mmap(~0UL, size, prot, flags,...);
> 软件可以通过指定一个大于 48 位的 mmap hint 参数来"主动选择"接收来自 52 位空间的 VA。
>
> 例如：
>
> .. code-block:: c
>
>    maybe_high_address = mmap(~0UL, size, prot, flags,...);

It is also possible to build a debug kernel that returns addresses
from a 52-bit space by enabling the following kernel config options:

.. code-block:: sh

   CONFIG_EXPERT=y && CONFIG_ARM64_FORCE_52BIT=y

Note that this option is only intended for debugging applications
and should not be used in production.
> 也可以通过启用以下内核配置选项，构建一个从 52 位空间返回地址的调试内核：
>
> .. code-block:: sh
>
>    CONFIG_EXPERT=y && CONFIG_ARM64_FORCE_52BIT=y
>
> 请注意，此选项仅用于调试应用程序，不应在生产环境中使用。


Why the Detailed Memory Layout Tables Were Removed
--------------------------------------------------
> 为什么删除了详细内存布局表
> -------------------------

Prior versions of this document (before Linux v6.15) included detailed
virtual memory layout tables showing the start/end/size/use of each
kernel virtual memory region for 4KB+4-level and 64KB+3-level
configurations, along with translation table walk diagrams. These were
removed by commit ``fd10f08cb57b`` ("Documentation: arm64: Remove stale
and redundant virtual memory diagrams") for the following reasons:
> 本文档的早期版本（Linux v6.15 之前）包含了详细的内核虚拟内存布局表，列出了 4KB+4 级和 64KB+3 级配置下每个内核虚拟内存区域的起始地址、结束地址、尺寸和用途，以及页表遍历示意图。这些内容在提交 ``fd10f08cb57b``（"Documentation: arm64: Remove stale and redundant virtual memory diagrams"）中被删除，原因如下：

1. **Frequently outdated (频繁过时):** The virtual memory map changes
   relatively frequently across kernel releases, and the tables were
   almost impossible to keep in sync with the code.
> 1. **频繁过时：** 虚拟内存布局在不同内核版本之间变化较为频繁，表格几乎不可能与代码保持同步。

2. **Incomplete configuration coverage (配置覆盖不全):** The tables only
   documented 4KB+4-level and 64KB+3-level setups. The kernel now supports
   configurations the original docs never anticipated, including 16KB pages
   and various combinations of VA bits and page table levels, each producing
   a different layout.
> 2. **配置覆盖不全：** 表格只记录了 4KB+4 级和 64KB+3 级两种配置。如今内核还支持 16KB 页面以及多种 VA 位数与页表级别的组合，每种配置产生的布局都不同，原始文档根本无法覆盖。

3. **Better authoritative sources (有更好的权威信息来源):**
   The Arm Architecture Reference Manual (Arm ARM) is the definitive
   resource for translation table walk procedures — there is no point
   duplicating it in kernel documentation.
> 3. **有更好的权威信息来源：** Arm 架构参考手册（Arm ARM）是页表遍历流程的权威资源——没必要在内核文档中重复它。


How to Inspect the Runtime Memory Layout
-----------------------------------------
> 如何查看运行时内存布局
> ---------------------

Instead of relying on a static table that may be out of date, use the
kernel's page-table dumping facility to inspect the **actual** memory
layout of a running kernel:
> 与其依赖可能已过时的静态表格，不如使用内核的页表导出功能来检查正在运行的内核的**实际**内存布局：

1. Enable the kernel config option::
> 1. 启用内核配置选项：

   CONFIG_PTDUMP_DEBUGFS=y
   >    CONFIG_PTDUMP_DEBUGFS=y

2. Make sure debugfs is mounted (most distributions mount it
   automatically at ``/sys/kernel/debug``)::
> 2. 确保 debugfs 已挂载（大多数发行版会自动挂载到 ``/sys/kernel/debug``）：

   mount -t debugfs none /sys/kernel/debug
   >    mount -t debugfs none /sys/kernel/debug

3. Read the page table dump::
> 3. 读取页表导出信息：

   cat /sys/kernel/debug/kernel_page_tables
   >    cat /sys/kernel/debug/kernel_page_tables

This shows the actual layout as programmed into the hardware page
tables, including section names (e.g. "Linear Mapping", "vmalloc() area"),
address ranges, sizes, page sizes, and permissions.
> 该命令会显示硬件页表中编程的实际布局，包括区域名称（如 "Linear Mapping"、"vmalloc() area"）、地址范围、尺寸、页大小以及权限信息。

Example output::
> 示例输出：

   ---[ Linear Mapping ]---
   0xffff000000000000-0xffff000010000000    256M   PMD   RW NX SHD ...
   ...
   ---[ vmalloc() area ]---
   0xffffa00010000000-0xffffa00010020000    128K   PTE   RW NX ...
   ...

Note: the kernel stopped printing the memory layout at boot time
(commit ``071929dbdd86`` "arm64: Stop printing the virtual memory layout")
for security reasons. ``CONFIG_PTDUMP_DEBUGFS`` is the recommended
replacement for development and debugging purposes.
> 注意：出于安全考虑，内核已停止在启动时打印内存布局（提交 ``071929dbdd86`` "arm64: Stop printing the virtual memory layout"）。``CONFIG_PTDUMP_DEBUGFS`` 是推荐的开发和调试替代方案。
