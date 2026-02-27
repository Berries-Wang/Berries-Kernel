/* SPDX-License-Identifier: GPL-2.0
 *
 * fixmap.h: compile-time virtual memory allocation
 *
 * Copyright (C) 1998 Ingo Molnar
 *
 * Support of BIGMEM added by Gerhard Wichert, Siemens AG, July 1999
 */

#ifndef _ASM_FIXMAP_H
#define _ASM_FIXMAP_H

#include <linux/kernel.h>
#include <linux/threads.h>
#include <asm/page.h>
#ifdef CONFIG_HIGHMEM
#include <asm/kmap_types.h>
#endif

/**
 * Fixmap（固定映射） 的设计哲学: 
 * 
 * Here we define all the compile-time 'special' virtual
 * addresses. The point is to have a constant address at
 * compile time, but to set the physical address only
 * in the boot process. We allocate these special  addresses
 * from the end of P3 backwards.
 * Also this lets us do fail-safe vmalloc(), we
 * can guarantee that these special addresses and
 * vmalloc()-ed addresses never overlap.
 * (在这里，我们定义了所有编译时确定的‘特殊’虚拟地址。其核心意义在于：
 *   在编译阶段就拥有一个常量地址，但直到启动过程中才去设定其对应的物理地址。
 *   我们从 P3（三级页表）的末尾开始反向分配这些特殊地址。
 *   此外，这种机制还实现了‘防错式’的 vmalloc()，能够确保这些特殊地址与 vmalloc() 分配的地址永远不会发生重叠。)
 *
 * these 'compile-time allocated' memory buffers are
 * fixed-size 4k pages. (or larger if used with an increment
 * highger than 1) use fixmap_set(idx,phys) to associate
 * physical memory with fixmap indices.
 * (这些‘编译时分配’的内存缓冲区是固定大小为 4KB 的页（如果使用的增量大于 1，
 * 则可以是更大的页）。你可以使用 fixmap_set(idx, phys) 函数将特定的物理内存地址与固定映射索引（fixmap indices）关联起来)
 *
 * TLB entries of such buffers will not be flushed across
 * task switches.
 */

/*
 * on UP currently we will have no trace of the fixmap mechanizm,
 * no page table allocations, etc. This might change in the
 * future, say framebuffers for the console driver(s) could be
 * fix-mapped?
 * (在当前的单处理器（UP）环境下，我们还看不到固定映射（fixmap）机制的痕迹，也没有页表分配等操作。
 * 但在未来这种情况可能会改变，例如控制台驱动的帧缓冲（framebuffers）或许可以采用固定映射？)
 */
enum fixed_addresses {
	/*
	 * The FIX_CMAP entries are used by kmap_coherent() to get virtual
	 * addresses which are of a known color, and so their values are
	 * important. __fix_to_virt(FIX_CMAP_END - n) must give an address
	 * which is the same color as a page (n<<PAGE_SHIFT).
	 * (FIX_CMAP 索引项被 kmap_coherent() 用于获取具有‘特定颜色（known color）’的虚拟地址，
	 * 因此它们的值至关重要。__fix_to_virt(FIX_CMAP_END - n) 必须返回一个与页面 (n << PAGE_SHIFT) 具有相同颜色的地址)
	 */
#define FIX_N_COLOURS 8
	FIX_CMAP_BEGIN,
	FIX_CMAP_END = FIX_CMAP_BEGIN + (FIX_N_COLOURS * NR_CPUS) - 1,

#ifdef CONFIG_HIGHMEM
	FIX_KMAP_BEGIN,	/* reserved pte's for temporary kernel mappings */
	FIX_KMAP_END = FIX_KMAP_BEGIN + (KM_TYPE_NR * NR_CPUS) - 1,
#endif

#ifdef CONFIG_IOREMAP_FIXED
	/*
	 * FIX_IOREMAP entries are useful for mapping physical address
	 * space before ioremap() is useable, e.g. really early in boot
	 * before kmalloc() is working.
	 */
#define FIX_N_IOREMAPS	32
	FIX_IOREMAP_BEGIN,
	FIX_IOREMAP_END = FIX_IOREMAP_BEGIN + FIX_N_IOREMAPS - 1,
#endif

	__end_of_fixed_addresses
};

extern void __set_fixmap(enum fixed_addresses idx,
			 unsigned long phys, pgprot_t flags);
extern void __clear_fixmap(enum fixed_addresses idx, pgprot_t flags);

/*
 * used by vmalloc.c.
 *
 * Leave one empty page between vmalloc'ed areas and
 * the start of the fixmap, and leave one page empty
 * at the top of mem..
 */
#define FIXADDR_TOP	(P4SEG - PAGE_SIZE)
#define FIXADDR_SIZE	(__end_of_fixed_addresses << PAGE_SHIFT)
#define FIXADDR_START	(FIXADDR_TOP - FIXADDR_SIZE)

#define FIXMAP_PAGE_NOCACHE PAGE_KERNEL_NOCACHE

#include <asm-generic/fixmap.h>

#endif
