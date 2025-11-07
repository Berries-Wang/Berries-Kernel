/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Based on arch/arm/include/asm/memory.h
 *
 * Copyright (C) 2000-2002 Russell King
 * Copyright (C) 2012 ARM Ltd.
 *
 * Note: this file should not be included by non-asm/.h files
 */
#ifndef __ASM_MEMORY_H
#define __ASM_MEMORY_H

#include <linux/const.h>
#include <linux/sizes.h>
#include <asm/page-def.h>

/*
 * Size of the PCI I/O space. This must remain a power of two so that
 * IO_SPACE_LIMIT acts as a mask for the low bits of I/O addresses.
 */
#define PCI_IO_SIZE		SZ_16M

/**
 * VMEMMAP_SIZE - allows the whole linear region to be covered by a struct page array
 *                 - (使得整个线性地址区域都能被一个 struct page 数组覆盖)
 * If we are configured with a 52-bit kernel VA then our VMEMMAP_SIZE
 * needs to cover the memory region from the beginning of the 52-bit
 * PAGE_OFFSET all the way to PAGE_END for 48-bit. This allows us to
 * keep a constant PAGE_OFFSET and "fallback" to using the higher end
 * of the VMEMMAP where 52-bit support is not available in hardware.
 * 若内核配置为使用52位虚拟地址（VA），则我们的 VMEMMAP_SIZE 必须覆盖从52位 PAGE_OFFSET 起始直至48位 PAGE_END 的整个内存区域。这种设计使我们能够：
 *   保持恒定的 PAGE_OFFSET 值 在硬件不支持52位地址时，自动"回退"到使用 VMEMMAP 区域的高端地址空间
 * 
 * 
 * (_PAGE_END(VA_BITS_MIN) - PAGE_OFFSET) = (0xFFFF800000000000 - 0xFFFF000000000000) 
 *  = 内核可管理的物理内存范围(线性映射区域的总大小（字节）)
 * 
 * >> (PAGE_SHIFT - STRUCT_PAGE_MAX_SHIFT)) = (2^12 / 2^6) = 2^(12-6) ， 因为进行移位操作，所以直接(12-6)
 * = 表示一个物理页面可以存储多少个struct page
 * 
 * > 6哪来的?当sizeof(struct page) = 64时
 * 
 * VMEMMAP_SIZE 那这个就表示在 0xFFFF800000000000 - 0xFFFF000000000000 范围内，page的数量咯
 * 结合上面的注释，这个其实就是 mem_map[]的长度吗?即 有多少个页面
 */
#define VMEMMAP_SIZE ((_PAGE_END(VA_BITS_MIN) - PAGE_OFFSET) \
			>> (PAGE_SHIFT - STRUCT_PAGE_MAX_SHIFT))

/**
 * PAGE_OFFSET - the virtual address of the start of the linear map, at the start of the TTBR1 address space.
 *                (线性映射起始位置的虚拟地址，位于 TTBR1 地址空间的开始处。)
 * PAGE_END - the end of the linear map, where all other kernel mappings begin.
 * KIMAGE_VADDR - the virtual address of the start of the kernel image.
 * VA_BITS - the maximum number of bits for virtual addresses.
 */
#define VA_BITS			(CONFIG_ARM64_VA_BITS)
/**
 * PAGE_OFFSET: 
 *    以48位虚拟地址为例，_PAGE_OFFSET(48) = -(1 << 48) = -2^48 = 0xFFFF000000000000 即内核地址空间的起始点
 * 
 * 宏展开: 怎么展开？见 000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/000.Kernel_Build.sh
 * 
 * KIMAGE_VADDR: '(((((((-((((1UL))) << ((((48))) - 1))))) + (0x08000000))) + (0x08000000)))'
 * > 003.TEST-SPACE/009.KIMAGE_VADDR.c
 * > 0XFFFF800010000000 ， 还是要和书中示意图中的值区分开,这是虚拟地址
 * > 这个值还和 ‘KASAN’ 是否开启有关！
 * PHYS_OFFSET: '({ ((void)(sizeof(( long)(memstart_addr & 1)))); memstart_addr; })'
 */
#define _PAGE_OFFSET(va)	(-(UL(1) << (va)))
#define PAGE_OFFSET		(_PAGE_OFFSET(VA_BITS))
#define KIMAGE_VADDR		(MODULES_END)
#define BPF_JIT_REGION_START	(KASAN_SHADOW_END)
#define BPF_JIT_REGION_SIZE	(SZ_128M)
#define BPF_JIT_REGION_END	(BPF_JIT_REGION_START + BPF_JIT_REGION_SIZE)
#define MODULES_END		(MODULES_VADDR + MODULES_VSIZE)
#define MODULES_VADDR		(BPF_JIT_REGION_END)
#define MODULES_VSIZE		(SZ_128M)
/**
 * 是不是很奇怪，大小怎么能当做一个起始地址呢?
 * 
 * 换个角度, 这个减数究竟是谁呢 是 “0xFFFF800000000000” 即  _PAGE_END(VA_BITS_MIN) ， 你瞅瞅，VMEMMAP_SIZE是怎么算出来的
 * 以及 ， 结合图来分析:  [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#图2.9　ARM64在Linux 5.0内核的内存分布
 * 
 * 示意图: 001.UNIX-DOCS/022.内存管理/999.IMGS/wechat_2025-08-16_120801_349.png
 */
#define VMEMMAP_START		(-VMEMMAP_SIZE - SZ_2M)
#define VMEMMAP_END		(VMEMMAP_START + VMEMMAP_SIZE)
#define PCI_IO_END		(VMEMMAP_START - SZ_2M)
#define PCI_IO_START		(PCI_IO_END - PCI_IO_SIZE)
#define FIXADDR_TOP		(PCI_IO_START - SZ_2M)

#if VA_BITS > 48
#define VA_BITS_MIN		(48)
#else
#define VA_BITS_MIN		(VA_BITS)
#endif

/**
 * 宏 _PAGE_END(va) 用于计算内核虚拟地址空间的结束地址，其值为 -(1 << (va - 1))，表示在 va 位虚拟地址配置下，内核地址空间的最高有效边界。
 * 
 * _PAGE_END(48) = -(1 << 47) = -2^47 = 0xFFFF800000000000
 */
#define _PAGE_END(va)		(-(UL(1) << ((va) - 1)))

#define KERNEL_START		_text
#define KERNEL_END		_end

/**
 * Generic and tag-based KASAN require 1/8th and 1/16th of the kernel virtual
 * address space for the shadow region respectively. They can bloat the stack
 * significantly, so double the (minimum) stack size when they are in use.
 * (通用KASAN和基于标签的KASAN分别需要占用内核虚拟地址空间的1/8和1/16作为影子内存区域。
 * 这两种模式都会显著增加堆栈体积，因此当启用它们时，需要将（最小）堆栈大小扩大一倍。)
 */
#ifdef CONFIG_KASAN
#define KASAN_SHADOW_OFFSET	_AC(CONFIG_KASAN_SHADOW_OFFSET, UL)
#define KASAN_SHADOW_END	((UL(1) << (64 - KASAN_SHADOW_SCALE_SHIFT)) \
					+ KASAN_SHADOW_OFFSET)
#define KASAN_THREAD_SHIFT	1
#else
#define KASAN_THREAD_SHIFT	0
#define KASAN_SHADOW_END	(_PAGE_END(VA_BITS_MIN))
#endif /* CONFIG_KASAN */

#define MIN_THREAD_SHIFT	(14 + KASAN_THREAD_SHIFT)

/*
 * VMAP'd stacks are allocated at page granularity, so we must ensure that such
 * stacks are a multiple of page size.
 */
#if defined(CONFIG_VMAP_STACK) && (MIN_THREAD_SHIFT < PAGE_SHIFT)
#define THREAD_SHIFT		PAGE_SHIFT
#else
#define THREAD_SHIFT		MIN_THREAD_SHIFT
#endif

#if THREAD_SHIFT >= PAGE_SHIFT
#define THREAD_SIZE_ORDER	(THREAD_SHIFT - PAGE_SHIFT)
#endif

#define THREAD_SIZE		(UL(1) << THREAD_SHIFT)

/*
 * By aligning VMAP'd stacks to 2 * THREAD_SIZE, we can detect overflow by
 * checking sp & (1 << THREAD_SHIFT), which we can do cheaply in the entry
 * assembly.
 */
#ifdef CONFIG_VMAP_STACK
#define THREAD_ALIGN		(2 * THREAD_SIZE)
#else
#define THREAD_ALIGN		THREAD_SIZE
#endif

#define IRQ_STACK_SIZE		THREAD_SIZE

#define OVERFLOW_STACK_SIZE	SZ_4K

/*
 * Alignment of kernel segments (e.g. .text, .data).
 *
 *  4 KB granule:  16 level 3 entries, with contiguous bit
 * 16 KB granule:   4 level 3 entries, without contiguous bit
 * 64 KB granule:   1 level 3 entry
 */
#define SEGMENT_ALIGN		SZ_64K

/**
 * 内存属性
 * Memory types available.
 */
#define MT_DEVICE_nGnRnE	0      /*设备内存属性，不支持聚合操作，不支持指令重排，不支持提前写应答*/
#define MT_DEVICE_nGnRE		1      /*设备内存属性，不支持聚合操作，不支持指令重排，支持提前写应答*/
#define MT_DEVICE_GRE		2      /*设备内存属性，支持聚合操作，支持指令重排，支持提前写应答*/
#define MT_NORMAL_NC		3      /*普通内存属性，关闭高速缓存，其中NC是Non-Cacheable的意思*/
#define MT_NORMAL	    	4      /*普通内存属性*/
#define MT_NORMAL_WT		5      /*普通内存属性，高速缓存的回写策略为直写（write through）策略*/

/*
 * Memory types for Stage-2 translation
 */
#define MT_S2_NORMAL		0xf
#define MT_S2_DEVICE_nGnRE	0x1

/*
 * Memory types for Stage-2 translation when ID_AA64MMFR2_EL1.FWB is 0001
 * Stage-2 enforces Normal-WB and Device-nGnRE
 */
#define MT_S2_FWB_NORMAL	6
#define MT_S2_FWB_DEVICE_nGnRE	1

#ifdef CONFIG_ARM64_4K_PAGES
#define IOREMAP_MAX_ORDER	(PUD_SHIFT)
#else
#define IOREMAP_MAX_ORDER	(PMD_SHIFT)
#endif

#ifndef __ASSEMBLY__

#include <linux/bitops.h>
#include <linux/compiler.h>
#include <linux/mmdebug.h>
#include <linux/types.h>
#include <asm/bug.h>

extern u64			vabits_actual;
#define PAGE_END		(_PAGE_END(vabits_actual))

extern s64			physvirt_offset;
extern s64			memstart_addr;
/* PHYS_OFFSET - the physical address of the start of memory. */
#define PHYS_OFFSET		({ VM_BUG_ON(memstart_addr & 1); memstart_addr; })

/* the virtual base of the kernel image (minus TEXT_OFFSET (在arch/arm64/Makefile中)) */
extern u64			kimage_vaddr;

/* the offset between the kernel virtual and physical mappings */
extern u64			kimage_voffset;

static inline unsigned long kaslr_offset(void)
{
	return kimage_vaddr - KIMAGE_VADDR;
}

/*
 * Allow all memory at the discovery stage. We will clip it later.
 */
#define MIN_MEMBLOCK_ADDR	0
#define MAX_MEMBLOCK_ADDR	U64_MAX

/*
 * PFNs are used to describe any physical page; this means
 * PFN 0 == physical address 0.
 *
 * This is the PFN of the first RAM page in the kernel
 * direct-mapped view.  We assume this is the first page
 * of RAM in the mem_map as well.
 */
#define PHYS_PFN_OFFSET	(PHYS_OFFSET >> PAGE_SHIFT)

/*
 * When dealing with data aborts, watchpoints, or instruction traps we may end
 * up with a tagged userland pointer. Clear the tag to get a sane pointer to
 * pass on to access_ok(), for instance.
 */
#define __untagged_addr(addr)	\
	((__force __typeof__(addr))sign_extend64((__force u64)(addr), 55))

#define untagged_addr(addr)	({					\
	u64 __addr = (__force u64)(addr);					\
	__addr &= __untagged_addr(__addr);				\
	(__force __typeof__(addr))__addr;				\
})

#ifdef CONFIG_KASAN_SW_TAGS
#define __tag_shifted(tag)	((u64)(tag) << 56)
#define __tag_reset(addr)	__untagged_addr(addr)
#define __tag_get(addr)		(__u8)((u64)(addr) >> 56)
#else
#define __tag_shifted(tag)	0UL
#define __tag_reset(addr)	(addr)
#define __tag_get(addr)		0
#endif /* CONFIG_KASAN_SW_TAGS */

static inline const void *__tag_set(const void *addr, u8 tag)
{
	u64 __addr = (u64)addr & ~__tag_shifted(0xff);
	return (const void *)(__addr | __tag_shifted(tag));
}

/*
 * Physical vs virtual RAM address space conversion.  These are
 * private definitions which should NOT be used outside memory.h
 * files.  Use virt_to_phys/phys_to_virt/__pa/__va instead.
 */


/*
 * The linear kernel range starts at the bottom of the virtual address
 * space. Testing the top bit for the start of the region is a
 * sufficient check and avoids having to worry about the tag.
 */
#define __is_lm_address(addr)	(!(((u64)addr) & BIT(vabits_actual - 1)))

#define __lm_to_phys(addr)	(((addr) + physvirt_offset))

/**
 * __kimg_to_phys 执行从内核映像虚拟地址（Kernel Image Virtual Address）到物理地址的转换。
 * 这种转换在内核初始化的早期阶段非常有用，因为在页表完全建立之前，
 * 虚拟地址和物理地址之间的映射是固定的线性关系。
 */
#define __kimg_to_phys(addr)	((addr) - kimage_voffset)

#define __virt_to_phys_nodebug(x) ({					\
	phys_addr_t __x = (phys_addr_t)(__tag_reset(x));		\
	__is_lm_address(__x) ? __lm_to_phys(__x) : __kimg_to_phys(__x);	\
})

#define __pa_symbol_nodebug(x)	__kimg_to_phys((phys_addr_t)(x))

#ifdef CONFIG_DEBUG_VIRTUAL
extern phys_addr_t __virt_to_phys(unsigned long x);
extern phys_addr_t __phys_addr_symbol(unsigned long x);
#else
#define __virt_to_phys(x)	__virt_to_phys_nodebug(x)
#define __phys_addr_symbol(x)	__pa_symbol_nodebug(x)
#endif /* CONFIG_DEBUG_VIRTUAL */

#define __phys_to_virt(x)	((unsigned long)((x) - physvirt_offset))
#define __phys_to_kimg(x)	((unsigned long)((x) + kimage_voffset))

/*
 * Convert a page to/from a physical address
 */
#define page_to_phys(page)	(__pfn_to_phys(page_to_pfn(page)))
#define phys_to_page(phys)	(pfn_to_page(__phys_to_pfn(phys)))

/*
 * Note: Drivers should NOT use these.  They are the wrong
 * translation for translating DMA addresses.  Use the driver
 * DMA support - see dma-mapping.h.
 */
#define virt_to_phys virt_to_phys
static inline phys_addr_t virt_to_phys(const volatile void *x)
{
	return __virt_to_phys((unsigned long)(x));
}

#define phys_to_virt phys_to_virt
static inline void *phys_to_virt(phys_addr_t x)
{
	return (void *)(__phys_to_virt(x));
}

/**
 * Drivers should NOT use these either.
 * 
 * __pa_symbol: 把内核符号的虚拟地址转换为物理地址
 * __pa: 把内核虚拟地址转换为物理地址 (__pa()宏用于根据内核中线性映射的虚拟地址计算对应的物理地址)
 * 
 * __va()宏用于根据内核线性映射中物理地址计算对应的虚拟地址
 * 
 */
#define __pa(x)			__virt_to_phys((unsigned long)(x))
#define __pa_symbol(x)		__phys_addr_symbol(RELOC_HIDE((unsigned long)(x), 0))
#define __pa_nodebug(x)		__virt_to_phys_nodebug((unsigned long)(x))
#define __va(x)			((void *)__phys_to_virt((phys_addr_t)(x)))
#define pfn_to_kaddr(pfn)	__va((pfn) << PAGE_SHIFT)
#define virt_to_pfn(x)		__phys_to_pfn(__virt_to_phys((unsigned long)(x)))
#define sym_to_pfn(x)		__phys_to_pfn(__pa_symbol(x))

/*
 *  virt_to_page(x)	convert a _valid_ virtual address to struct page *
 *  virt_addr_valid(x)	indicates whether a virtual address is valid
 */
#define ARCH_PFN_OFFSET		((unsigned long)PHYS_PFN_OFFSET)

#if !defined(CONFIG_SPARSEMEM_VMEMMAP) || defined(CONFIG_DEBUG_VIRTUAL)
#define virt_to_page(x)		pfn_to_page(virt_to_pfn(x))
#else
#define page_to_virt(x)	({						\
	__typeof__(x) __page = x;					\
	u64 __idx = ((u64)__page - VMEMMAP_START) / sizeof(struct page);\
	u64 __addr = PAGE_OFFSET + (__idx * PAGE_SIZE);			\
	(void *)__tag_set((const void *)__addr, page_kasan_tag(__page));\
})

#define virt_to_page(x)	({						\
	u64 __idx = (__tag_reset((u64)x) - PAGE_OFFSET) / PAGE_SIZE;	\
	u64 __addr = VMEMMAP_START + (__idx * sizeof(struct page));	\
	(struct page *)__addr;						\
})
#endif /* !CONFIG_SPARSEMEM_VMEMMAP || CONFIG_DEBUG_VIRTUAL */

#define virt_addr_valid(addr)	({					\
	__typeof__(addr) __addr = addr;					\
	__is_lm_address(__addr) && pfn_valid(virt_to_pfn(__addr));	\
})

void dump_mem_limit(void);
#endif /* !ASSEMBLY */

/*
 * Given that the GIC architecture permits ITS implementations that can only be
 * configured with a LPI table address once, GICv3 systems with many CPUs may
 * end up reserving a lot of different regions after a kexec for their LPI
 * tables (one per CPU), as we are forced to reuse the same memory after kexec
 * (and thus reserve it persistently with EFI beforehand)
 */
#if defined(CONFIG_EFI) && defined(CONFIG_ARM_GIC_V3_ITS)
# define INIT_MEMBLOCK_RESERVED_REGIONS	(INIT_MEMBLOCK_REGIONS + NR_CPUS + 1)
#endif

#include <asm-generic/memory_model.h>

#endif /* __ASM_MEMORY_H */
