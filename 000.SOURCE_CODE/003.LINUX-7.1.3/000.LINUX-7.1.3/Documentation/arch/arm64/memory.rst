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

.. code-block::
   ~ # cat /sys/kernel/debug/kernel_page_tables
---[ Linear Mapping start ]---
0xffff000000000000-0xffff000000200000           2M PTE       RW NX SHD AF NG         UXN    MEM/NORMAL-TAGGED
0xffff000000200000-0xffff000002200000          32M PMD       ro NX SHD AF NG     BLK UXN    MEM/NORMAL
0xffff000002200000-0xffff0000022c0000         768K PTE       ro NX SHD AF NG         UXN    MEM/NORMAL
0xffff0000022c0000-0xffff000003a00000       23808K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL-TAGGED
0xffff000003a00000-0xffff000003c00000           2M PTE       ro NX SHD AF NG         UXN    MEM/NORMAL-TAGGED
0xffff000003c00000-0xffff000040000000         964M PTE       RW NX SHD AF NG         UXN    MEM/NORMAL-TAGGED
0xffff000040000000-0xffff008000000000         511G PUD
0xffff008000000000-0xffff800000000000      130560G PGD
---[ Linear Mapping end ]---
---[ Modules start ]---
0xffff800000000000-0xffff800080000000           2G PUD
---[ Modules end ]---
---[ vmalloc() area ]---
0xffff800080000000-0xffff800080004000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080004000-0xffff800080005000           4K PTE
0xffff800080005000-0xffff800080006000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff800080006000-0xffff800080008000           8K PTE
0xffff800080008000-0xffff80008000c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008000c000-0xffff80008000d000           4K PTE
0xffff80008000d000-0xffff80008000e000           4K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008000e000-0xffff800080010000           8K PTE
0xffff800080010000-0xffff800080014000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080014000-0xffff800080015000           4K PTE
0xffff800080015000-0xffff800080016000           4K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080016000-0xffff800080018000           8K PTE
0xffff800080018000-0xffff80008001c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008001c000-0xffff800080020000          16K PTE
0xffff800080020000-0xffff800080030000          64K PTE       RW NX SHD AF NG CON     UXN    DEVICE/nGnRE
0xffff800080030000-0xffff800080035000          20K PTE
0xffff800080035000-0xffff800080036000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff800080036000-0xffff800080038000           8K PTE
0xffff800080038000-0xffff80008003c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008003c000-0xffff80008003d000           4K PTE
0xffff80008003d000-0xffff80008003e000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff80008003e000-0xffff800080040000           8K PTE
0xffff800080040000-0xffff800080050000          64K PTE       RW NX SHD AF NG CON     UXN    DEVICE/nGnRE
0xffff800080050000-0xffff800080058000          32K PTE
0xffff800080058000-0xffff80008005c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008005c000-0xffff800080060000          16K PTE
0xffff800080060000-0xffff800080064000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080064000-0xffff800080068000          16K PTE
0xffff800080068000-0xffff80008006c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008006c000-0xffff800080070000          16K PTE
0xffff800080070000-0xffff800080074000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080074000-0xffff800080078000          16K PTE
0xffff800080078000-0xffff80008007c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008007c000-0xffff800080080000          16K PTE
0xffff800080080000-0xffff800080084000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080084000-0xffff800080088000          16K PTE
0xffff800080088000-0xffff80008008c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008008c000-0xffff800080090000          16K PTE
0xffff800080090000-0xffff800080094000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080094000-0xffff800080098000          16K PTE
0xffff800080098000-0xffff80008009c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008009c000-0xffff8000800a0000          16K PTE
0xffff8000800a0000-0xffff8000800a4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800a4000-0xffff8000800a8000          16K PTE
0xffff8000800a8000-0xffff8000800ac000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800ac000-0xffff8000800b0000          16K PTE
0xffff8000800b0000-0xffff8000800b4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800b4000-0xffff8000800b8000          16K PTE
0xffff8000800b8000-0xffff8000800bc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800bc000-0xffff8000800c0000          16K PTE
0xffff8000800c0000-0xffff8000800c4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800c4000-0xffff8000800c8000          16K PTE
0xffff8000800c8000-0xffff8000800cc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800cc000-0xffff8000800d0000          16K PTE
0xffff8000800d0000-0xffff8000800d4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800d4000-0xffff8000800d8000          16K PTE
0xffff8000800d8000-0xffff8000800dc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800dc000-0xffff8000800e0000          16K PTE
0xffff8000800e0000-0xffff8000800e4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800e4000-0xffff8000800e8000          16K PTE
0xffff8000800e8000-0xffff8000800ec000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800ec000-0xffff8000800f0000          16K PTE
0xffff8000800f0000-0xffff8000800f4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800f4000-0xffff8000800f8000          16K PTE
0xffff8000800f8000-0xffff8000800fc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000800fc000-0xffff800080100000          16K PTE
0xffff800080100000-0xffff800080104000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080104000-0xffff800080108000          16K PTE
0xffff800080108000-0xffff80008010c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008010c000-0xffff800080110000          16K PTE
0xffff800080110000-0xffff800080114000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080114000-0xffff800080118000          16K PTE
0xffff800080118000-0xffff80008011c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008011c000-0xffff800080120000          16K PTE
0xffff800080120000-0xffff800080124000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080124000-0xffff800080128000          16K PTE
0xffff800080128000-0xffff80008012c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008012c000-0xffff800080130000          16K PTE
0xffff800080130000-0xffff800080134000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080134000-0xffff800080138000          16K PTE
0xffff800080138000-0xffff80008013c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008013c000-0xffff800080140000          16K PTE
0xffff800080140000-0xffff800080144000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080144000-0xffff800080145000           4K PTE
0xffff800080145000-0xffff800080146000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff800080146000-0xffff800080148000           8K PTE
0xffff800080148000-0xffff80008014c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008014c000-0xffff80008014d000           4K PTE
0xffff80008014d000-0xffff80008014e000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff80008014e000-0xffff800080150000           8K PTE
0xffff800080150000-0xffff800080154000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080154000-0xffff800080155000           4K PTE
0xffff800080155000-0xffff800080156000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff800080156000-0xffff800080158000           8K PTE
0xffff800080158000-0xffff80008015c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008015c000-0xffff80008015d000           4K PTE
0xffff80008015d000-0xffff80008015e000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff80008015e000-0xffff800080160000           8K PTE
0xffff800080160000-0xffff800080164000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080164000-0xffff800080165000           4K PTE
0xffff800080165000-0xffff800080166000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff800080166000-0xffff800080168000           8K PTE
0xffff800080168000-0xffff80008016c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008016c000-0xffff80008016d000           4K PTE
0xffff80008016d000-0xffff80008016e000           4K PTE       RW NX SHD AF NG         UXN    DEVICE/nGnRE
0xffff80008016e000-0xffff800080170000           8K PTE
0xffff800080170000-0xffff800080174000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080174000-0xffff800080178000          16K PTE
0xffff800080178000-0xffff80008017c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008017c000-0xffff800080180000          16K PTE
0xffff800080180000-0xffff800080184000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080184000-0xffff800080188000          16K PTE
0xffff800080188000-0xffff80008018c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008018c000-0xffff80008018d000           4K PTE
0xffff80008018d000-0xffff80008038d000           2M PTE       ro x  SHD AF NG         UXN    MEM/NORMAL
0xffff80008038d000-0xffff80008038e000           4K PTE
0xffff80008038e000-0xffff8000803ae000         128K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL-NC
0xffff8000803ae000-0xffff8000803b0000           8K PTE
0xffff8000803b0000-0xffff8000803b4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803b4000-0xffff8000803b8000          16K PTE
0xffff8000803b8000-0xffff8000803bc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803bc000-0xffff8000803c0000          16K PTE
0xffff8000803c0000-0xffff8000803c4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803c4000-0xffff8000803c8000          16K PTE
0xffff8000803c8000-0xffff8000803cc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803cc000-0xffff8000803d0000          16K PTE
0xffff8000803d0000-0xffff8000803d4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803d4000-0xffff8000803d8000          16K PTE
0xffff8000803d8000-0xffff8000803dc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803dc000-0xffff8000803e0000          16K PTE
0xffff8000803e0000-0xffff8000803e4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803e4000-0xffff8000803e8000          16K PTE
0xffff8000803e8000-0xffff8000803ec000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803ec000-0xffff8000803f0000          16K PTE
0xffff8000803f0000-0xffff8000803f4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803f4000-0xffff8000803f8000          16K PTE
0xffff8000803f8000-0xffff8000803fc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000803fc000-0xffff800080400000          16K PTE
0xffff800080400000-0xffff800080404000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080404000-0xffff800080408000          16K PTE
0xffff800080408000-0xffff80008040c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008040c000-0xffff800080410000          16K PTE
0xffff800080410000-0xffff800080414000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080414000-0xffff800080418000          16K PTE
0xffff800080418000-0xffff80008041c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008041c000-0xffff800080420000          16K PTE
0xffff800080420000-0xffff800080424000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080424000-0xffff800080428000          16K PTE
0xffff800080428000-0xffff80008042c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008042c000-0xffff800080430000          16K PTE
0xffff800080430000-0xffff800080434000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080434000-0xffff800080438000          16K PTE
0xffff800080438000-0xffff80008043c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008043c000-0xffff800080440000          16K PTE
0xffff800080440000-0xffff800080444000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080444000-0xffff800080448000          16K PTE
0xffff800080448000-0xffff80008044c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008044c000-0xffff800080450000          16K PTE
0xffff800080450000-0xffff800080454000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080454000-0xffff800080458000          16K PTE
0xffff800080458000-0xffff80008045c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008045c000-0xffff800080460000          16K PTE
0xffff800080460000-0xffff800080464000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080464000-0xffff800080468000          16K PTE
0xffff800080468000-0xffff80008046c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008046c000-0xffff800080470000          16K PTE
0xffff800080470000-0xffff800080474000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080474000-0xffff800080478000          16K PTE
0xffff800080478000-0xffff80008047c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008047c000-0xffff800080480000          16K PTE
0xffff800080480000-0xffff800080484000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080484000-0xffff800080488000          16K PTE
0xffff800080488000-0xffff80008048c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008048c000-0xffff800080490000          16K PTE
0xffff800080490000-0xffff800080494000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080494000-0xffff800080495000           4K PTE
0xffff800080495000-0xffff8000804c9000         208K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000804c9000-0xffff8000804ca000           4K PTE
0xffff8000804ca000-0xffff8000804cd000          12K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000804cd000-0xffff8000804d0000          12K PTE
0xffff8000804d0000-0xffff8000804d4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000804d4000-0xffff8000804d8000          16K PTE
0xffff8000804d8000-0xffff8000804dc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000804dc000-0xffff8000804e0000          16K PTE
0xffff8000804e0000-0xffff8000804e4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000804e4000-0xffff8000804e8000          16K PTE
0xffff8000804e8000-0xffff8000804ec000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000804ec000-0xffff8000804f0000          16K PTE
0xffff8000804f0000-0xffff8000804f4000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000804f4000-0xffff8000804f8000          16K PTE
0xffff8000804f8000-0xffff8000804fc000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff8000804fc000-0xffff800080530000         208K PTE
0xffff800080530000-0xffff800080534000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff800080534000-0xffff800080538000          16K PTE
0xffff800080538000-0xffff80008053c000          16K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xffff80008053c000-0xffff800080600000         784K PTE
0xffff800080600000-0xffff800084000000          58M PMD
0xffff800084000000-0xffff800088000000          64M PMD       RW NX SHD AF NG     BLK UXN    DEVICE/nGnRE
0xffff800088000000-0xffff800090000000         128M PMD
0xffff800090000000-0xffff8000a0000000         256M PMD       RW NX SHD AF NG     BLK UXN    DEVICE/nGnRnE
0xffff8000a0000000-0xffff8000a4000000          64M PMD
0xffff8000a4000000-0xffff8000a8000000          64M PMD       RW NX SHD AF NG     BLK UXN    DEVICE/nGnRE
0xffff8000a8000000-0xffff8000c0000000         384M PMD
0xffff8000c0000000-0xffff808000000000         509G PUD
0xffff808000000000-0xffffcb0000000000       76288G PGD
0xffffcb0000000000-0xffffcb7e40000000         505G PUD
0xffffcb7e40000000-0xffffcb7e65800000         600M PMD
0xffffcb7e65800000-0xffffcb7e65810000          64K PTE       ro NX SHD AF NG         UXN    MEM/NORMAL
0xffffcb7e65810000-0xffffcb7e65a00000        1984K PTE       ro x  SHD AF NG CON     UXN    MEM/NORMAL
0xffffcb7e65a00000-0xffffcb7e66a00000          16M PMD       ro x  SHD AF NG     BLK UXN    MEM/NORMAL
0xffffcb7e66a00000-0xffffcb7e66a80000         512K PTE       ro x  SHD AF NG CON     UXN    MEM/NORMAL
0xffffcb7e66a80000-0xffffcb7e66c00000        1536K PTE       ro NX SHD AF NG         UXN    MEM/NORMAL
0xffffcb7e66c00000-0xffffcb7e67800000          12M PMD       ro NX SHD AF NG     BLK UXN    MEM/NORMAL
0xffffcb7e67800000-0xffffcb7e678c0000         768K PTE       ro NX SHD AF NG         UXN    MEM/NORMAL
0xffffcb7e678c0000-0xffffcb7e67a00000        1280K PTE
0xffffcb7e67a00000-0xffffcb7e67c00000           2M PMD
0xffffcb7e67c00000-0xffffcb7e67d30000        1216K PTE
0xffffcb7e67d30000-0xffffcb7e67e00000         832K PTE       RW NX SHD AF NG CON     UXN    MEM/NORMAL
0xffffcb7e67e00000-0xffffcb7e68200000           4M PMD       RW NX SHD AF NG     BLK UXN    MEM/NORMAL
0xffffcb7e68200000-0xffffcb7e68320000        1152K PTE       RW NX SHD AF NG CON     UXN    MEM/NORMAL
0xffffcb7e68320000-0xffffcb7e68400000         896K PTE
0xffffcb7e68400000-0xffffcb7e80000000         380M PMD
0xffffcb7e80000000-0xffffcb8000000000           6G PUD
0xffffcb8000000000-0xfffffd8000000000          50T PGD
0xfffffd8000000000-0xfffffdff80000000         510G PUD
0xfffffdff80000000-0xfffffdffbf600000        1014M PMD
0xfffffdffbf600000-0xfffffdffbf798000        1632K PTE
0xfffffdffbf798000-0xfffffdffbf79b000          12K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xfffffdffbf79b000-0xfffffdffbf7b2000          92K PTE
0xfffffdffbf7b2000-0xfffffdffbf7b5000          12K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xfffffdffbf7b5000-0xfffffdffbf7cc000          92K PTE
0xfffffdffbf7cc000-0xfffffdffbf7cf000          12K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xfffffdffbf7cf000-0xfffffdffbf7e6000          92K PTE
0xfffffdffbf7e6000-0xfffffdffbf7e9000          12K PTE       RW NX SHD AF NG         UXN    MEM/NORMAL
0xfffffdffbf7e9000-0xfffffdffbf800000          92K PTE
---[ vmalloc() end ]---
0xfffffdffbf800000-0xfffffdffc0000000           8M PMD
---[ vmemmap start ]---
0xfffffdffc0000000-0xfffffdffc1000000          16M PMD       RW NX SHD AF NG     BLK UXN    MEM/NORMAL
0xfffffdffc1000000-0xfffffe0000000000        1008M PMD
0xfffffe0000000000-0xffffff8000000000        1536G PGD
0xffffff8000000000-0xffffffffc0000000         511G PUD
---[ vmemmap end ]---
0xffffffffc0000000-0xffffffffc0800000           8M PMD
---[ PCI I/O start ]---
0xffffffffc0800000-0xffffffffc0810000          64K PTE       RW NX SHD AF NG CON     UXN    DEVICE/nGnRE
0xffffffffc0810000-0xffffffffc0a00000        1984K PTE
0xffffffffc0a00000-0xffffffffc1800000          14M PMD
---[ PCI I/O end ]---
0xffffffffc1800000-0xffffffffff200000         986M PMD
0xffffffffff200000-0xffffffffff22d000         180K PTE
---[ Fixmap start ]---
0xffffffffff22d000-0xffffffffff3f3000        1816K PTE
0xffffffffff3f3000-0xffffffffff3f6000          12K PTE       ro x  SHD AF            UXN    MEM/NORMAL
0xffffffffff3f6000-0xffffffffff3f7000           4K PTE       ro NX SHD AF NG         UXN    MEM/NORMAL
0xffffffffff3f7000-0xffffffffff5fe000        2076K PTE
0xffffffffff5fe000-0xffffffffff6fe000           1M PTE       ro NX SHD AF NG         UXN    MEM/NORMAL
0xffffffffff6fe000-0xffffffffff800000        1032K PTE
---[ Fixmap end ]---
0xffffffffff800000-0x0000000000000000           8M PMD
~ # [  198.894563] random: crng init done