/*
 * fixmap.h: compile-time virtual memory allocation
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1998 Ingo Molnar
 *
 * Support of BIGMEM added by Gerhard Wichert, Siemens AG, July 1999
 * x86_32 and x86_64 integration by Gustavo F. Padovan, February 2009
 * Break out common bits to asm-generic by Mark Salter, November 2013
 */

#ifndef __ASM_GENERIC_FIXMAP_H
#define __ASM_GENERIC_FIXMAP_H

#include <linux/bug.h>
#include <linux/mm_types.h>

/**
 * fix 是什么? fixed区域？ YES
 * 
 */
#define __fix_to_virt(x)	(FIXADDR_TOP - ((x) << PAGE_SHIFT))
#define __virt_to_fix(x)	((FIXADDR_TOP - ((x)&PAGE_MASK)) >> PAGE_SHIFT)

#ifndef __ASSEMBLY__
/*
 * 'index to address' translation. If anyone tries to use the idx
 * directly without translation, we catch the bug with a NULL-deference
 * kernel oops. Illegal ranges of incoming indices are caught too.
 * (索引到地址”的转换。如果有人试图不经过转换而直接使用索引（idx），
 * 我们会通过“空指针解引用”（NULL-deference）内核崩溃（kernel oops）来捕获该错误。此外，传入索引的非法范围也会被拦截)
 */
static __always_inline unsigned long fix_to_virt(const unsigned int idx)
{
	BUILD_BUG_ON(idx >= __end_of_fixed_addresses);
	return __fix_to_virt(idx);
}

static inline unsigned long virt_to_fix(const unsigned long vaddr)
{
	BUG_ON(vaddr >= FIXADDR_TOP || vaddr < FIXADDR_START);
	return __virt_to_fix(vaddr);
}

/*
 * Provide some reasonable defaults for page flags.
 * Not all architectures use all of these different types and some
 * architectures use different names.
 */
#ifndef FIXMAP_PAGE_NORMAL
#define FIXMAP_PAGE_NORMAL PAGE_KERNEL
#endif
#if !defined(FIXMAP_PAGE_RO) && defined(PAGE_KERNEL_RO)
#define FIXMAP_PAGE_RO PAGE_KERNEL_RO
#endif
#ifndef FIXMAP_PAGE_NOCACHE
#define FIXMAP_PAGE_NOCACHE PAGE_KERNEL_NOCACHE
#endif
#ifndef FIXMAP_PAGE_IO
#define FIXMAP_PAGE_IO PAGE_KERNEL_IO
#endif
#ifndef FIXMAP_PAGE_CLEAR
#define FIXMAP_PAGE_CLEAR __pgprot(0)
#endif

#ifndef set_fixmap
#define set_fixmap(idx, phys)				\
	__set_fixmap(idx, phys, FIXMAP_PAGE_NORMAL)
#endif

#ifndef clear_fixmap
#define clear_fixmap(idx)			\
	__set_fixmap(idx, 0, FIXMAP_PAGE_CLEAR)
#endif

/**
 *  Return a pointer with offset calculated 
 *  (返回一个计算偏移量后的指针)
 * 
 * 怎么分析这几行代码:
 *  - __set_fixmap(idx, phys, flags); 完成虚拟地址到物理地址的映射: 将虚拟地址发送给CPU,CPU能通过MMU访问到实际的物理地址 (因为基本页表项构建好了,只需要填充PTE即可)
 *  - fix_to_virt(idx) + ((phys) & (PAGE_SIZE - 1));  计算物理地址对应的虚拟地址(固定映射区域)
 * */
#define __set_fixmap_offset(idx, phys, flags)				\
({									\
	unsigned long ________addr;					\
	__set_fixmap(idx, phys, flags);					\
	________addr = fix_to_virt(idx) + ((phys) & (PAGE_SIZE - 1));	\
	________addr;							\
})

#define set_fixmap_offset(idx, phys) \
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_NORMAL)

/*
 * Some hardware wants to get fixmapped without caching.
 */
#define set_fixmap_nocache(idx, phys) \
	__set_fixmap(idx, phys, FIXMAP_PAGE_NOCACHE)

#define set_fixmap_offset_nocache(idx, phys) \
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_NOCACHE)

/*
 * Some fixmaps are for IO
 */
#define set_fixmap_io(idx, phys) \
	__set_fixmap(idx, phys, FIXMAP_PAGE_IO)

#define set_fixmap_offset_io(idx, phys) \
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_IO)

#endif /* __ASSEMBLY__ */
#endif /* __ASM_GENERIC_FIXMAP_H */
