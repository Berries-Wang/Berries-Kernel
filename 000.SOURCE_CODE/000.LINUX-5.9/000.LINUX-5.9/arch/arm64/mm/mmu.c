// SPDX-License-Identifier: GPL-2.0-only
/*
 * Based on arch/arm/mm/mmu.c
 *
 * Copyright (C) 1995-2005 Russell King
 * Copyright (C) 2012 ARM Ltd.
 */

#include <linux/cache.h>
#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/kexec.h>
#include <linux/libfdt.h>
#include <linux/mman.h>
#include <linux/nodemask.h>
#include <linux/memblock.h>
#include <linux/memory.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>

#include <asm/barrier.h>
#include <asm/cputype.h>
#include <asm/fixmap.h>
#include <asm/kasan.h>
#include <asm/kernel-pgtable.h>
#include <asm/sections.h>
#include <asm/setup.h>
#include <linux/sizes.h>
#include <asm/tlb.h>
#include <asm/mmu_context.h>
#include <asm/ptdump.h>
#include <asm/tlbflush.h>
#include <asm/pgalloc.h>

#define NO_BLOCK_MAPPINGS	BIT(0)
#define NO_CONT_MAPPINGS	BIT(1)

u64 idmap_t0sz = TCR_T0SZ(VA_BITS);
u64 idmap_ptrs_per_pgd = PTRS_PER_PGD;

u64 __section(".mmuoff.data.write") vabits_actual;
EXPORT_SYMBOL(vabits_actual);

/**
 * kimage_voffset表示内核映像虚拟地址和物理地址之间的偏移量
 * 来源于:  
 * 1). __primary_switched函数 (2.6.6　__primary_switch函数分析) -- 在这个函数中赋值的
 * 2). 这些内核符号表的链接地址在vmalloc区域，即从KIMAGE_VADDR + TEXT_OFFSET开始的虚拟地址，它和内核空间的线性映射区是不相同的
 *     > 2.1.6　案例分析：ARM64的页表映射过程: 图2.11　kimage_voffset的含义 + 那kimage_voffset代表什么意思呢？如图2.11所示，当系统刚初始化时，内核映像通过块映射的方式映射到KIMAGE_VADDR + TEXT_OFFSET的虚拟地址上，因此kimage_voffset表示内核映像虚拟地址和物理地址之间的偏移量，这类似于线性映射之后的PAGE_OFFSET。
 * 
 * 000.LINUX-5.9/arch/arm64/kernel/head.S 会处理kimage_vaddr , kimage_offset 
 */
u64 kimage_voffset __ro_after_init;
EXPORT_SYMBOL(kimage_voffset);

/*
 * Empty_zero_page is a special page that is used for zero-initialized data
 * and COW.
 */
unsigned long empty_zero_page[PAGE_SIZE / sizeof(unsigned long)] __page_aligned_bss;
EXPORT_SYMBOL(empty_zero_page);

static pte_t bm_pte[PTRS_PER_PTE] __page_aligned_bss;
static pmd_t bm_pmd[PTRS_PER_PMD] __page_aligned_bss __maybe_unused;
static pud_t bm_pud[PTRS_PER_PUD] __page_aligned_bss __maybe_unused;

static DEFINE_SPINLOCK(swapper_pgdir_lock);

/**
 * set_swapper_pgd 函数的作用是安全地更新内核的 swapper_pg_dir（内核初始页全局目录）中的某个 PGD 条目
 * 
 * @param pgdp页表项
 * ＠param pgd 物理地址
 * 
 * 
 * 也不只是填充pgd页表项，pud页表项也是如此操作!!!
 */
void set_swapper_pgd(pgd_t *pgdp, pgd_t pgd)
{
	pgd_t *fixmap_pgdp;

	spin_lock(&swapper_pgdir_lock);
	fixmap_pgdp = pgd_set_fixmap(__pa_symbol(pgdp));
	WRITE_ONCE(*fixmap_pgdp, pgd);
	/*
	 * We need dsb(ishst) here to ensure the page-table-walker sees
	 * our new entry before set_p?d() returns. The fixmap's
	 * flush_tlb_kernel_range() via clear_fixmap() does this for us.
	 */
	pgd_clear_fixmap();
	spin_unlock(&swapper_pgdir_lock);
}

pgprot_t phys_mem_access_prot(struct file *file, unsigned long pfn,
			      unsigned long size, pgprot_t vma_prot)
{
	if (!pfn_valid(pfn))
		return pgprot_noncached(vma_prot);
	else if (file->f_flags & O_SYNC)
		return pgprot_writecombine(vma_prot);
	return vma_prot;
}
EXPORT_SYMBOL(phys_mem_access_prot);

static phys_addr_t __init early_pgtable_alloc(int shift)
{
	phys_addr_t phys;
	void *ptr;
        
        /**
         * include/linux/memblock.h: 
         *
         */
	phys = memblock_phys_alloc(PAGE_SIZE, PAGE_SIZE);
	if (!phys)
		panic("Failed to allocate page table page\n");

	/*
	 * The FIX_{PGD,PUD,PMD} slots may be in active use, but the FIX_PTE
	 * slot will be free, so we can (ab)use the FIX_PTE slot to initialise
	 * any level of table.
	 */
	ptr = pte_set_fixmap(phys);

	memset(ptr, 0, PAGE_SIZE);

	/*
	 * Implicit barriers also ensure the zeroed page is visible to the page
	 * table walker
	 */
	pte_clear_fixmap();

	return phys;
}

static bool pgattr_change_is_safe(u64 old, u64 new)
{
	/*
	 * The following mapping attributes may be updated in live
	 * kernel mappings without the need for break-before-make.
	 */
	static const pteval_t mask = PTE_PXN | PTE_RDONLY | PTE_WRITE | PTE_NG;

	/* creating or taking down mappings is always safe */
	if (old == 0 || new == 0)
		return true;

	/* live contiguous mappings may not be manipulated at all */
	if ((old | new) & PTE_CONT)
		return false;

	/* Transitioning from Non-Global to Global is unsafe */
	if (old & ~new & PTE_NG)
		return false;

	return ((old ^ new) & ~mask) == 0;
}

/**
 * 初始化PTE条目(一个个页表，存储的是页帧)，即 填充PTE页表
 * 
 * init_pte()函数最终调用set_pte()来设置PTE内容到物理页表中
 */
static void init_pte(pmd_t *pmdp, unsigned long addr, unsigned long end,
		     phys_addr_t phys, pgprot_t prot)
{
	pte_t *ptep;

	ptep = pte_set_fixmap_offset(pmdp, addr);
	do {
		pte_t old_pte = READ_ONCE(*ptep);

		/**
		 *  填充pmd页表项${pmdp} , 类似于创建PT页表
		 */
		set_pte(ptep, pfn_pte(__phys_to_pfn(phys), prot));

		/*
		 * After the PTE entry has been populated once, we
		 * only allow updates to the permission attributes.
		 */
		BUG_ON(!pgattr_change_is_safe(pte_val(old_pte),
					      READ_ONCE(pte_val(*ptep))));

		// 处理下一个页面
		phys += PAGE_SIZE;

		/**
		 * petp++ 处理下一个PTE页表的页表项
		 */
	} while (ptep++, addr += PAGE_SIZE, addr != end);

	pte_clear_fixmap();
}

/**
 * @param pmdp pmd页表项
 * @param addr 开始地址(虚拟地址)
 * @param end  pmd页表项能覆盖的内存的右边界
 * @param phys 需要映射的内存的物理地址
 * @param 函数指针
 * 
 * cont: Contiguous , 连续的页面
 */
static void alloc_init_cont_pte(pmd_t *pmdp, unsigned long addr,
				unsigned long end, phys_addr_t phys,
				pgprot_t prot,
				phys_addr_t (*pgtable_alloc)(int), int flags)
{
	unsigned long next;
	pmd_t pmd = READ_ONCE(*pmdp);

	BUG_ON(pmd_sect(pmd));
	if (pmd_none(pmd)) {
		phys_addr_t pte_phys;
		BUG_ON(!pgtable_alloc);
		/**
		 * 分配一个空的内存页，用作pte页表
		 */
		pte_phys = pgtable_alloc(PAGE_SHIFT);
		/**
		 * 设置页表项pmdp，即页表项pmdp存储的下一级页表的物理基地址
		 */
		__pmd_populate(pmdp, pte_phys, PMD_TYPE_TABLE);
		pmd = READ_ONCE(*pmdp);
	}
	BUG_ON(pmd_bad(pmd));

	do {
		pgprot_t __prot = prot;
		/**
		 * 为什么不直接处理一个个的page呢?
		 */
		next = pte_cont_addr_end(addr, end);

		/* use a contiguous mapping if the range is suitably aligned */
		if ((((addr | next | phys) & ~CONT_PTE_MASK) == 0) &&
		    (flags & NO_CONT_MAPPINGS) == 0) {
			/**
			 * 不直接调用 init_pte 是为了添加连续的标识吗?
			 */
			__prot = __pgprot(pgprot_val(prot) | PTE_CONT);
		}

		/**
		 * 调用init_pte()来设置PTE: 一个页面一个页面地配置
		 */
		init_pte(pmdp, addr, next, phys, __prot);

		phys += next - addr;
	} while (addr = next, addr != end);
}

/**
 * 处理pmd页表，如下，逐个处理pmd页表项
 * 
 */
static void init_pmd(pud_t *pudp, unsigned long addr, unsigned long end,
		     phys_addr_t phys, pgprot_t prot,
		     phys_addr_t (*pgtable_alloc)(int), int flags)
{
	unsigned long next;
	pmd_t *pmdp;

	pmdp = pmd_set_fixmap_offset(pudp, addr);
	do {
		pmd_t old_pmd = READ_ONCE(*pmdp);

		/**
		 * 以addr为起始，单个pmd页表项能覆盖的内存的右边界
		 */
		next = pmd_addr_end(addr, end);

		/**
		 *  try section mapping first 
		 * 
		 * 如果虚拟区间的开始地址addr、结束地址next和物理地址phys都与SECTION_SIZE大小（2MB）对齐，
		 * 那么直接设置PMD页表项，不需要映射下一级页表。下一级页表等到需要用时再映射也来得及。
		*/
		if (((addr | next | phys) & ~SECTION_MASK) == 0 &&
		    (flags & NO_BLOCK_MAPPINGS) == 0) {
			/**
			 * 直接设置为大页
			 */
			pmd_set_huge(pmdp, phys, prot);

			/*
			 * After the PMD entry has been populated once, we
			 * only allow updates to the permission attributes.
			 */
			BUG_ON(!pgattr_change_is_safe(
				pmd_val(old_pmd), READ_ONCE(pmd_val(*pmdp))));
		} else {
			/**
			 * 如果映射的内存不是和SECTION_SIZE大小对齐的，那么需要通过alloc_init_cont_pte()函数来映射下一级PTE
			 */
			alloc_init_cont_pte(pmdp, addr, next, phys, prot,
					    pgtable_alloc, flags);

			BUG_ON(pmd_val(old_pmd) != 0 &&
			       pmd_val(old_pmd) != READ_ONCE(pmd_val(*pmdp)));
		}
		phys += next - addr;
	} while (pmdp++, addr = next, addr != end);

	pmd_clear_fixmap();
}

/**
 * alloc_init_cont_pmd ()函数用于配置PMD页表
 * 
 * @param pudp pud页表的页表项
 * 
 * 函数名中的cont是什么? 是 Contiguous 即连续的 ， 通过这个函数中转是为了生成大的内存块
 * 》 ARM64 支持 连续页表条目（Contiguous PTEs/PMDs） 的硬件特性，允许将多个相邻的 PMD 条目合并，映射一块更大的连续物理内存（如 64MB），从而减少 TLB 压力和页表遍历开销。
 */
static void alloc_init_cont_pmd(pud_t *pudp, unsigned long addr,
				unsigned long end, phys_addr_t phys,
				pgprot_t prot,
				phys_addr_t (*pgtable_alloc)(int), int flags)
{
	unsigned long next;
	pud_t pud = READ_ONCE(*pudp);

	/*
	 * Check for initial section mappings in the pgd/pud.
	 */
	BUG_ON(pud_sect(pud));
	/**
	 * 判断PUD页表项的内容是否为空。如果为空，表示PUD指向的PMD不存在，
	 * 需要动态分配PMD页表，然后通过__pud_populate()来设置PUD页表项
	 */
	if (pud_none(pud)) {
		phys_addr_t pmd_phys;
		BUG_ON(!pgtable_alloc);
		// 分配一个4K大小的页表，返回物理基址(48位有效地址的情况下)
		pmd_phys = pgtable_alloc(PMD_SHIFT);
		/**
		 * 填充页表项pudp ，即页表项${pudp}存储的物理地址(下一级页表的基地址)是 pmd_phys 
		 * 
		 * 那么,页表项pud的Bit[47,12] == ${pmd_phys} (页面大小为4KB时)
		 * > debug
		 */
		__pud_populate(pudp, pmd_phys, PUD_TYPE_TABLE);
		pud = READ_ONCE(*pudp);
	}
	BUG_ON(pud_bad(pud));

	// 初始化pmd页表
	do {
		pgprot_t __prot = prot;
        
		/**
		 * 以16MB为步长来创建PMD页表?为什么要进行这一步
		 */
		next = pmd_cont_addr_end(addr, end);

		/* use a contiguous mapping if the range is suitably aligned */
		if ((((addr | next | phys) & ~CONT_PMD_MASK) == 0) &&
		    (flags & NO_CONT_MAPPINGS) == 0)
			__prot = __pgprot(pgprot_val(prot) | PTE_CONT);

		/**
		 * 填充pmd页表
		 * 
		 * 调用init_pmd()函数来初始化PT与设置PMD页表项
		*/
		init_pmd(pudp, addr, next, phys, __prot, pgtable_alloc, flags);

		phys += next - addr;
	} while (addr = next, addr != end);
}

static inline bool use_1G_block(unsigned long addr, unsigned long next,
			unsigned long phys)
{
	if (PAGE_SHIFT != 12)
		return false;

	if (((addr | next | phys) & ~PUD_MASK) != 0)
		return false;

	return true;
}

/**
 * 初始化PGD页表项内容和PUD
 *
 * 因为当前内核版本为5.9 , 支持4级页表，所以 pgd 就是 p4d ， 也可以通过代码看出来
 *
 * pgtable_alloc 函数指针,实际是 early_pgtable_alloc  （从 paging_init -> mem_map 分析而来）
 * 
 */
static void alloc_init_pud(pgd_t *pgdp, unsigned long addr, unsigned long end,
			   phys_addr_t phys, pgprot_t prot,
			   phys_addr_t (*pgtable_alloc)(int), int flags)
{
	unsigned long next;
	pud_t *pudp;
	/**
     * include/asm-generic/pgtable-nop4d.h
     * 
     * 因为(1) pgd 的页表项指向的是下一级页表的基地址
     */
	p4d_t *p4dp = p4d_offset(pgdp, addr);
	p4d_t p4d = READ_ONCE(*p4dp);

	/** 
     * arch/arm64/include/asm/pgtable.h : 判断是否初始化过:
     * p4d_none: true：未初始化过;false:已初始化
     */
	if (p4d_none(p4d)) {
		phys_addr_t pud_phys;
		BUG_ON(!pgtable_alloc);
		// 分配一个物理page出来 , 所以(1)这里分配的是一个PUD页表
		pud_phys = pgtable_alloc(PUD_SHIFT);
		// arch/arm64/include/asm/pgalloc.h : 填充P4D的页表项(条目)，指向PUD页表基地
		__p4d_populate(p4dp, pud_phys, PUD_TYPE_TABLE);
		// 重新读取更新后的p4d条目
		p4d = READ_ONCE(*p4dp);
	}

	BUG_ON(p4d_bad(p4d)); // 检查条目合法性

	// 将p4dp映射到fixmap，获取虚拟地址
	pudp = pud_set_fixmap_offset(p4dp, addr);

	// 这个do-while循环就是遍历并填充pgd页表的每个页表项，获取下一个页表项的逻辑在while中
	do {
		// 获取当前pudp条目
		pud_t old_pud = READ_ONCE(*pudp);

		// include/linux/pgtable.h: 计算当前pud能够覆盖的内存的右边界
		next = pud_addr_end(addr, end);

		/**
         * 
         * 这个得结合页表描述符来分析了: 无效的页表项、块类型的页表项、页表类型的页表项 , 这个都是通过描述符的标志位来区分的
         * 内容在: []#2.1.3　页表项描述符
         * > 知道了这个，下面的流程就很好懂了
         * 
		 * For 4K granule only, attempt to put down a 1GB block
		 * (对于仅使用4K粒度的情况，尝试建立一个1GB的大块映射)
		 * 
		 * use_1G_block()函数会判断是否使用1GB大小的内存块来映射
		 * 
		 * 001.UNIX-DOCS/022.内存管理/000.arm64-内核中的页表.md### 页表项描述符
		 */
		if (use_1G_block(addr, next, phys) &&
		    (flags & NO_BLOCK_MAPPINGS) == 0) {
			/**
             * 得结合页表项描述符来分析: []#2.1.3　页表项描述符
             *
             * 设置页表描述符为大页： 设置了页表项描述符的位
             * 如，将 Bit[0] 设置为1（表示有效的描述符），Bit[1]为0表示是一个大的内存块(这个函数不用操作Bit[1])
             */
			pud_set_huge(pudp, phys, prot);

			/**
			 * After the PUD entry has been populated once, we
			 * only allow updates to the permission attributes.
			 * (在填充（配置）PUD（Page Upper Directory，页上级目录）表项后，我们仅允许对其权限属性进行更新。)
			 */
			BUG_ON(!pgattr_change_is_safe(
				pud_val(old_pud), READ_ONCE(pud_val(*pudp))));
		} else {
			// 用于配置PMD页表 (为pud页表里的页表项${pudp}创建pmd页表)
			alloc_init_cont_pmd(pudp, addr, next, phys, prot,
					    pgtable_alloc, flags);

			BUG_ON(pud_val(old_pud) != 0 &&
			       pud_val(old_pud) != READ_ONCE(pud_val(*pudp)));
		}
		phys += next - addr;

		/**
		 * pudp++: 以PUD_SIZE为步长循环处理
		 * >> pudp++ , 即指向下一个 pud_t 
         * 这里其实就是迭代处理每个pgd页表项——pgd页表的页表项就是指向pud页表的物理基地址 
		 */
	} while (pudp++, addr = next, addr != end);

	pud_clear_fixmap();
}

/**
 * 
 * 创建页表映射
 * 
 * create_mapping_noalloc 看一下这个方法的注释 
 *
 * >>> 通过 'map_mem' 分析而来
 * @param pgdir pgd页表的基址
 * @param phys 待映射的内存空间的起始地址(物理地址)
 * @param virt phys的虚拟地址
 * @param size 内存空间大小
 * @param prot PAGE_KERNEL
 *
 * pgd_t 定义在 arch/arm64/include/asm/pgtable_types.h 
 *  typedef struct { pteval_t pte; } pte_t;
 *  typedef struct { pmdval_t pmd; } pmd_t;
 *  typedef struct { pudval_t pud; } pud_t;
 *  typedef struct { pgdval_t pgd; } pgd_t;
 */
static void __create_pgd_mapping(pgd_t *pgdir, phys_addr_t phys,
				 unsigned long virt, phys_addr_t size,
				 pgprot_t prot,
				 phys_addr_t (*pgtable_alloc)(int), int flags)
{
	unsigned long addr, end, next;
	// 计算pgd页表项: 从虚拟地址virt中提取PGD索引
	pgd_t *pgdp = pgd_offset_pgd(pgdir, virt);

	/*
	 * If the virtual and physical address don't have the same offset
	 * within a page, we cannot map the region as the caller expects.
	 */
	if (WARN_ON((phys ^ virt) & ~PAGE_MASK))
		return;

	phys &= PAGE_MASK;
	addr = virt & PAGE_MASK;
	end = PAGE_ALIGN(virt + size);

	do {
		// pgd_addr_end : include/linux/pgtable.h: 即 next表示这个pgd 页表项所能映射的区域的右边界
		next = pgd_addr_end(addr, end);
		// 为pgd页表里的页表项${pgdp}创建pud页表
		alloc_init_pud(pgdp, addr, next, phys, prot, pgtable_alloc,
			       flags);
		phys += next - addr;
		/*指针运算: pgdp++,指向下一个pgd_t区域，即以sizeof(pgd_t)(即:PGDIR_SIZE),遍历内存区域*/
	} while (pgdp++, addr = next, addr != end);
}

static phys_addr_t __pgd_pgtable_alloc(int shift)
{
	void *ptr = (void *)__get_free_page(GFP_PGTABLE_KERNEL);
	BUG_ON(!ptr);

	/* Ensure the zeroed page is visible to the page table walker */
	dsb(ishst);
	return __pa(ptr);
}

static phys_addr_t pgd_pgtable_alloc(int shift)
{
	phys_addr_t pa = __pgd_pgtable_alloc(shift);

	/*
	 * Call proper page table ctor in case later we need to
	 * call core mm functions like apply_to_page_range() on
	 * this pre-allocated page table.
	 *
	 * We don't select ARCH_ENABLE_SPLIT_PMD_PTLOCK if pmd is
	 * folded, and if so pgtable_pmd_page_ctor() becomes nop.
	 */
	if (shift == PAGE_SHIFT)
		BUG_ON(!pgtable_pte_page_ctor(phys_to_page(pa)));
	else if (shift == PMD_SHIFT)
		BUG_ON(!pgtable_pmd_page_ctor(phys_to_page(pa)));

	return pa;
}

/*
 * This function can only be used to modify existing table entries,
 * without allocating new levels of table. Note that this permits the
 * creation of new section or page entries.
 * (此功能仅可用于修改现有表格条目，而不会分配新的表格层级。但请注意，该操作允许创建新的节条目或页面条目)
 * 
 * 将起始物理地址等于phys，大小是size的这一段物理内存mapping到起始虚拟地址是virt的虚拟地址空间
 * 
 */
static void __init create_mapping_noalloc(phys_addr_t phys, unsigned long virt,
				  phys_addr_t size, pgprot_t prot)
{
        // 内核的虚拟地址空间从VMALLOC_START开始，低于这个地址就不对了
	if ((virt >= PAGE_END) && (virt < VMALLOC_START)) {
		pr_warn("BUG: not creating mapping for %pa at 0x%016lx - outside kernel range\n",
			&phys, virt);
		return;
	}
	__create_pgd_mapping(init_mm.pgd, phys, virt, size, prot, NULL,
			     NO_CONT_MAPPINGS);
}

void __init create_pgd_mapping(struct mm_struct *mm, phys_addr_t phys,
			       unsigned long virt, phys_addr_t size,
			       pgprot_t prot, bool page_mappings_only)
{
	int flags = 0;

	BUG_ON(mm == &init_mm);

	if (page_mappings_only)
		flags = NO_BLOCK_MAPPINGS | NO_CONT_MAPPINGS;

	__create_pgd_mapping(mm->pgd, phys, virt, size, prot,
			     pgd_pgtable_alloc, flags);
}

static void update_mapping_prot(phys_addr_t phys, unsigned long virt,
				phys_addr_t size, pgprot_t prot)
{
	if ((virt >= PAGE_END) && (virt < VMALLOC_START)) {
		pr_warn("BUG: not updating mapping for %pa at 0x%016lx - outside kernel range\n",
			&phys, virt);
		return;
	}

	__create_pgd_mapping(init_mm.pgd, phys, virt, size, prot, NULL,
			     NO_CONT_MAPPINGS);

	/* flush the TLBs after updating live kernel mappings */
	flush_tlb_kernel_range(virt, virt + size);
}

static void __init __map_memblock(pgd_t *pgdp, phys_addr_t start,
				  phys_addr_t end, pgprot_t prot, int flags)
{
	__create_pgd_mapping(pgdp, start, __phys_to_virt(start), end - start,
			     prot, early_pgtable_alloc, flags);
}

void __init mark_linear_text_alias_ro(void)
{
	/*
	 * Remove the write permissions from the linear alias of .text/.rodata
	 */
	update_mapping_prot(__pa_symbol(_text), (unsigned long)lm_alias(_text),
			    (unsigned long)__init_begin - (unsigned long)_text,
			    PAGE_KERNEL_RO);
}

/**
 * map_mem(pgdp)：物理内存的线性映射。物理内存会全部线性映射到以PAGE_OFFSET(即页面内偏移量)开始的内核空间的虚拟地址，以加速内核访问内存。
 * 
 * "加速访问"的含义:
 *   通过简单的偏移量实现，虚拟地址 = 物理地址 + 固定偏移（如PAGE_OFFSET）, 提供一种快速访问物理内存的方式，避免了频繁的页表操作
 * 
 * [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]2．map_mem()函数
 */
static void __init map_mem(pgd_t *pgdp)
{
	phys_addr_t kernel_start = __pa_symbol(_text);
	/**
	 * 0000.LINUX-5.9/arch/arm64/kernel/vmlinux.lds.S 中的__init_begin?
	 */
	phys_addr_t kernel_end = __pa_symbol(__init_begin);
	struct memblock_region *reg;
	int flags = 0;

	if (rodata_full || debug_pagealloc_enabled())
		flags = NO_BLOCK_MAPPINGS | NO_CONT_MAPPINGS;

	/**
	 * Take care not to create a writable alias for the
	 * read-only text and rodata sections of the kernel image.
	 * So temporarily mark them as NOMAP to skip mappings in
	 * the following for-loop
	 * (注意不要为内核镜像的只读代码段（text）和只读数据段（rodata）创建可写别名。
	 * 因此需要暂时将它们标记为 NOMAP，以便在接下来的循环映射中跳过这些区域。)
	 */
	memblock_mark_nomap(kernel_start, kernel_end - kernel_start);
#ifdef CONFIG_KEXEC_CORE
	if (crashk_res.end)
		memblock_mark_nomap(crashk_res.start,
				    resource_size(&crashk_res));
#endif

	/** map all the memory banks(映射所有存储体) 
	 * 
	 * for_each_memblock 定义在: 000.LINUX-5.9/include/linux/memblock.h
	 * 这里传入的是 memory , 所以遍历的是所有可用的物理内存 (另一种是保留内存)
	*/
	for_each_memblock(memory, reg) {
		phys_addr_t start = reg->base;
		phys_addr_t end = start + reg->size;

		if (start >= end)
			break;
		if (memblock_is_nomap(reg))
			continue;

		// 创建页表映射,内部是直接调用 _create_pgd_mapping
		__map_memblock(pgdp, start, end, PAGE_KERNEL, flags);
	}

	/**
	 * Map the linear alias of the [_text, __init_begin) interval
	 * as non-executable now, and remove the write permission in
	 * mark_linear_text_alias_ro() below (which will be called after
	 * alternative patching has completed). This makes the contents
	 * of the region accessible to subsystems such as hibernate,
	 * but protects it from inadvertent modification or execution.
	 * Note that contiguous mappings cannot be remapped in this way,
	 * so we should avoid them here.
	 * (现在将 [_text, __init_begin) 区间的线性别名映射为不可执行，并在后续的 mark_linear_text_alias_ro() 函数中移除写权限（该函数将在替代修补完成后调用）。
	 * 这样既能让休眠等子系统访问该区域内容，又能防止意外修改或执行。需要注意的是，连续映射无法以这种方式重新映射，因此我们在此应避免使用连续映射)
	 */
	__map_memblock(pgdp, kernel_start, kernel_end,
		       PAGE_KERNEL, NO_CONT_MAPPINGS);
	// 000.LINUX-5.9/mm/memblock.c
	memblock_clear_nomap(kernel_start, kernel_end - kernel_start);

#ifdef CONFIG_KEXEC_CORE
	/*
	 * Use page-level mappings here so that we can shrink the region
	 * in page granularity and put back unused memory to buddy system
	 * through /sys/kernel/kexec_crash_size interface.
	 */
	if (crashk_res.end) {
		__map_memblock(pgdp, crashk_res.start, crashk_res.end + 1,
			       PAGE_KERNEL,
			       NO_BLOCK_MAPPINGS | NO_CONT_MAPPINGS);
		memblock_clear_nomap(crashk_res.start,
				     resource_size(&crashk_res));
	}
#endif
}

void mark_rodata_ro(void)
{
	unsigned long section_size;

	/*
	 * mark .rodata as read only. Use __init_begin rather than __end_rodata
	 * to cover NOTES and EXCEPTION_TABLE.
	 */
	section_size = (unsigned long)__init_begin - (unsigned long)__start_rodata;
	update_mapping_prot(__pa_symbol(__start_rodata), (unsigned long)__start_rodata,
			    section_size, PAGE_KERNEL_RO);

	debug_checkwx();
}

static void __init map_kernel_segment(pgd_t *pgdp, void *va_start, void *va_end,
				      pgprot_t prot, struct vm_struct *vma,
				      int flags, unsigned long vm_flags)
{
	// __pa_symbol()宏和__pa()宏的作用都是把内核虚拟地址转换为物理地址
	phys_addr_t pa_start = __pa_symbol(va_start);
	unsigned long size = va_end - va_start;

	BUG_ON(!PAGE_ALIGNED(pa_start));
	BUG_ON(!PAGE_ALIGNED(size));

	__create_pgd_mapping(pgdp, pa_start, (unsigned long)va_start, size, prot,
			     early_pgtable_alloc, flags);

	if (!(vm_flags & VM_NO_GUARD))
		size += PAGE_SIZE;

	vma->addr	= va_start;
	vma->phys_addr	= pa_start;
	vma->size	= size;
	vma->flags	= VM_MAP | vm_flags;
	vma->caller	= __builtin_return_address(0);

	vm_area_add_early(vma);
}

static int __init parse_rodata(char *arg)
{
	int ret = strtobool(arg, &rodata_enabled);
	if (!ret) {
		rodata_full = false;
		return 0;
	}

	/* permit 'full' in addition to boolean options */
	if (strcmp(arg, "full"))
		return -EINVAL;

	rodata_enabled = true;
	rodata_full = true;
	return 0;
}
early_param("rodata", parse_rodata);

#ifdef CONFIG_UNMAP_KERNEL_AT_EL0
static int __init map_entry_trampoline(void)
{
	pgprot_t prot = rodata_enabled ? PAGE_KERNEL_ROX : PAGE_KERNEL_EXEC;
	phys_addr_t pa_start = __pa_symbol(__entry_tramp_text_start);

	/* The trampoline is always mapped and can therefore be global */
	pgprot_val(prot) &= ~PTE_NG;

	/* Map only the text into the trampoline page table */
	memset(tramp_pg_dir, 0, PGD_SIZE);
	__create_pgd_mapping(tramp_pg_dir, pa_start, TRAMP_VALIAS, PAGE_SIZE,
			     prot, __pgd_pgtable_alloc, 0);

	/* Map both the text and data into the kernel page table */
	__set_fixmap(FIX_ENTRY_TRAMP_TEXT, pa_start, prot);
	if (IS_ENABLED(CONFIG_RANDOMIZE_BASE)) {
		extern char __entry_tramp_data_start[];

		__set_fixmap(FIX_ENTRY_TRAMP_DATA,
			     __pa_symbol(__entry_tramp_data_start),
			     PAGE_KERNEL_RO);
	}

	return 0;
}
core_initcall(map_entry_trampoline);
#endif

/*
 * Open coded check for BTI, only for use to determine configuration
 * for early mappings for before the cpufeature code has run.
 */
static bool arm64_early_this_cpu_has_bti(void)
{
	u64 pfr1;

	if (!IS_ENABLED(CONFIG_ARM64_BTI_KERNEL))
		return false;

	pfr1 = read_sysreg_s(SYS_ID_AA64PFR1_EL1);
	return cpuid_feature_extract_unsigned_field(pfr1,
						    ID_AA64PFR1_BT_SHIFT);
}

/**
 * Create fine-grained mappings for the kernel.
 * (为内核创建精细的映射)
 */
static void __init map_kernel(pgd_t *pgdp)
{
	static struct vm_struct vmlinux_text, vmlinux_rodata, vmlinux_inittext,
				vmlinux_initdata, vmlinux_data;

	/*
	 * External debuggers may need to write directly to the text
	 * mapping to install SW breakpoints. Allow this (only) when
	 * explicitly requested with rodata=off.
	 */
	pgprot_t text_prot = rodata_enabled ? PAGE_KERNEL_ROX : PAGE_KERNEL_EXEC;

	/*
	 * If we have a CPU that supports BTI and a kernel built for
	 * BTI then mark the kernel executable text as guarded pages
	 * now so we don't have to rewrite the page tables later.
	 */
	if (arm64_early_this_cpu_has_bti())
		text_prot = __pgprot_modify(text_prot, PTE_GP, PTE_GP);

	/*
	 * Only rodata will be remapped with different permissions later on,
	 * all other segments are allowed to use contiguous mappings.
	 */
	map_kernel_segment(pgdp, _text, _etext, text_prot, &vmlinux_text, 0,
			   VM_NO_GUARD);
	map_kernel_segment(pgdp, __start_rodata, __inittext_begin, PAGE_KERNEL,
			   &vmlinux_rodata, NO_CONT_MAPPINGS, VM_NO_GUARD);
	map_kernel_segment(pgdp, __inittext_begin, __inittext_end, text_prot,
			   &vmlinux_inittext, 0, VM_NO_GUARD);
	map_kernel_segment(pgdp, __initdata_begin, __initdata_end, PAGE_KERNEL,
			   &vmlinux_initdata, 0, VM_NO_GUARD);
	map_kernel_segment(pgdp, _data, _end, PAGE_KERNEL, &vmlinux_data, 0, 0);

	if (!READ_ONCE(pgd_val(*pgd_offset_pgd(pgdp, FIXADDR_START)))) {
		/*
		 * The fixmap falls in a separate pgd to the kernel, and doesn't
		 * live in the carveout for the swapper_pg_dir. We can simply
		 * re-use the existing dir for the fixmap.
		 */
		set_pgd(pgd_offset_pgd(pgdp, FIXADDR_START),
			READ_ONCE(*pgd_offset_k(FIXADDR_START)));
	} else if (CONFIG_PGTABLE_LEVELS > 3) {
		pgd_t *bm_pgdp;
		p4d_t *bm_p4dp;
		pud_t *bm_pudp;
		/*
		 * The fixmap shares its top level pgd entry with the kernel
		 * mapping. This can really only occur when we are running
		 * with 16k/4 levels, so we can simply reuse the pud level
		 * entry instead.
		 */
		BUG_ON(!IS_ENABLED(CONFIG_ARM64_16K_PAGES));
		bm_pgdp = pgd_offset_pgd(pgdp, FIXADDR_START);
		bm_p4dp = p4d_offset(bm_pgdp, FIXADDR_START);
		bm_pudp = pud_set_fixmap_offset(bm_p4dp, FIXADDR_START);
		pud_populate(&init_mm, bm_pudp, lm_alias(bm_pmd));
		pud_clear_fixmap();
	} else {
		BUG();
	}

	kasan_copy_shadow(pgdp);
}

/**
 * 页表的创建是由操作系统来完成的，包括页表的创建和填充，但是处理器遍历页表是由处理器的MMU来完成的 in []#2.1.6　案例分析：ARM64的页表映射过程
 *   MMU是硬件设备，所以硬件规定(约定?) swapper_pg_dir 是PGD页表项的基地址，这样好理解了
 *
 * 在paging_init()函数中会对内核空间的多个内存段做重新映射 , 映射到页表
 * 
 * swapper_pg_dir : 内核页表的PGD页表基地址，是虚拟地址，因为在内核启动的汇编代码中会做一次简单的块映射
 * __pa_symbol: 把内核符号的虚拟地址转换为物理地址
 * __pa: 这时物理内存的线性映射还没建立好，因此不能直接使用__pa()宏

 *
 * 看一下[Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#3.3.4　物理内存映射
 *     内核映像映射了两次： 映射到内核空间的虚拟地址；线性映射? 怎么理解? 
 *        从示意图看，两块虚拟地址空间映射的物理内存空间是一样的!
 *          map_kernel: 确保内核自身能继续执行：处理内核代码、数据段的映射，解决 MMU 启用前后的地址连续性问题。(在 MMU 启用瞬间，CPU 仍在通过物理地址执行指令，必须保证内核代码的虚拟地址能立即访问)
 *          map_mem: 建立全局物理内存管理：将所有物理内存（包括内核已占用的部分）映射到线性区域，供内核全局使用。
 * 
 */
void __init paging_init(void)
{
	// 输出 swapper_pg_dir 的值(虚拟&物理地址) kimage_voffset
	printk("swapper_pg_dir is 0x%lx , pa: ox%lx \n", (swapper_pg_dir),(__pa_symbol(swapper_pg_dir)));
	printk("kimage_voffset is 0x%lx \n", (kimage_voffset));

	/**
	 * pgd_set_fixmap()函数做一个固定映射，把swapper_pg_dir页表重新映射到固定映射区域
	 * > 这个映射的原理是什么
     * 
     * 获取PGD页表基地址  []#2.1.6　案例分析：ARM64的页表映射过程
	 * 
     * swapper_pg_dir 就是页表基地址，虚拟地址 ， 使用 __pa_symbol 转为物理地址
	 * > 全局变量swapper_pg_dir是内核页表的PGD页表基地址，可是这是虚拟地址，因为在内核启动的汇编代码中会做一次简单的块映射。
	 * ____>在链接文件: 000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/arch/arm64/kernel/vmlinux.lds.S 
	 * ---->> 索引可以使用__pa_symbol获取到物理地址
	 * 
	 * 内核里面有一个固定映射（fixed mapping）区域，它的范围是0xFFFF 7DFF FE7F 9000～0xFFFF 7DFF FEC0 0000，我们可以把PGD页表映射这个区域，然后才可以使用__pa()宏。
	 * > #图2.9　ARM64在Linux 5.0内核的内存分布
	 * 
	 * pgd_set_fixmap()函数就做这个固定映射的事情，把PGD页表的物理页面映射到固定映射区域，返回PGD页表的虚拟地址。而pgd_clear_fixmap()函数用于取消固定区域的映射
	 */
	pgd_t *pgdp = pgd_set_fixmap(__pa_symbol(swapper_pg_dir));
 
	/**
	 * map_kernel(pgdp)：对内核映像文件的各个块重新映射。
	 * 在head.S文件中，我们对内核映像文件做了块映射，现在需要使用页机制来重新映射。
	 * 
	 * > 保证内核在启用分页后能够正常执行 (在内核启动之初，并没有立马启用分页)
	 */
	map_kernel(pgdp);

	/**
	 * 线性映射区映射,具体看代码注释
	 * 
	 * 为内核进行内存映射，使内核能够通过虚拟地址访问整个物理内存?
	 */
	map_mem(pgdp);

	/**
	 * 取消映射，和 pgd_set_fixmap 对应
	 */
	pgd_clear_fixmap();

	cpu_replace_ttbr1(lm_alias(swapper_pg_dir));

	// 赋值整个内核的pgd
	init_mm.pgd = swapper_pg_dir;

	memblock_free(__pa_symbol(init_pg_dir),
		      __pa_symbol(init_pg_end) - __pa_symbol(init_pg_dir));

	memblock_allow_resize();
}

/*
 * Check whether a kernel address is valid (derived from arch/x86/).
 */
int kern_addr_valid(unsigned long addr)
{
	pgd_t *pgdp;
	p4d_t *p4dp;
	pud_t *pudp, pud;
	pmd_t *pmdp, pmd;
	pte_t *ptep, pte;

	addr = arch_kasan_reset_tag(addr);
	if ((((long)addr) >> VA_BITS) != -1UL)
		return 0;

	pgdp = pgd_offset_k(addr);
	if (pgd_none(READ_ONCE(*pgdp)))
		return 0;

	p4dp = p4d_offset(pgdp, addr);
	if (p4d_none(READ_ONCE(*p4dp)))
		return 0;

	pudp = pud_offset(p4dp, addr);
	pud = READ_ONCE(*pudp);
	if (pud_none(pud))
		return 0;

	if (pud_sect(pud))
		return pfn_valid(pud_pfn(pud));

	pmdp = pmd_offset(pudp, addr);
	pmd = READ_ONCE(*pmdp);
	if (pmd_none(pmd))
		return 0;

	if (pmd_sect(pmd))
		return pfn_valid(pmd_pfn(pmd));

	ptep = pte_offset_kernel(pmdp, addr);
	pte = READ_ONCE(*ptep);
	if (pte_none(pte))
		return 0;

	return pfn_valid(pte_pfn(pte));
}

#ifdef CONFIG_MEMORY_HOTPLUG
static void free_hotplug_page_range(struct page *page, size_t size,
				    struct vmem_altmap *altmap)
{
	if (altmap) {
		vmem_altmap_free(altmap, size >> PAGE_SHIFT);
	} else {
		WARN_ON(PageReserved(page));
		free_pages((unsigned long)page_address(page), get_order(size));
	}
}

static void free_hotplug_pgtable_page(struct page *page)
{
	free_hotplug_page_range(page, PAGE_SIZE, NULL);
}

static bool pgtable_range_aligned(unsigned long start, unsigned long end,
				  unsigned long floor, unsigned long ceiling,
				  unsigned long mask)
{
	start &= mask;
	if (start < floor)
		return false;

	if (ceiling) {
		ceiling &= mask;
		if (!ceiling)
			return false;
	}

	if (end - 1 > ceiling - 1)
		return false;
	return true;
}

static void unmap_hotplug_pte_range(pmd_t *pmdp, unsigned long addr,
				    unsigned long end, bool free_mapped,
				    struct vmem_altmap *altmap)
{
	pte_t *ptep, pte;

	do {
		ptep = pte_offset_kernel(pmdp, addr);
		pte = READ_ONCE(*ptep);
		if (pte_none(pte))
			continue;

		WARN_ON(!pte_present(pte));
		pte_clear(&init_mm, addr, ptep);
		flush_tlb_kernel_range(addr, addr + PAGE_SIZE);
		if (free_mapped)
			free_hotplug_page_range(pte_page(pte),
						PAGE_SIZE, altmap);
	} while (addr += PAGE_SIZE, addr < end);
}

static void unmap_hotplug_pmd_range(pud_t *pudp, unsigned long addr,
				    unsigned long end, bool free_mapped,
				    struct vmem_altmap *altmap)
{
	unsigned long next;
	pmd_t *pmdp, pmd;

	do {
		next = pmd_addr_end(addr, end);
		pmdp = pmd_offset(pudp, addr);
		pmd = READ_ONCE(*pmdp);
		if (pmd_none(pmd))
			continue;

		WARN_ON(!pmd_present(pmd));
		if (pmd_sect(pmd)) {
			pmd_clear(pmdp);

			/*
			 * One TLBI should be sufficient here as the PMD_SIZE
			 * range is mapped with a single block entry.
			 */
			flush_tlb_kernel_range(addr, addr + PAGE_SIZE);
			if (free_mapped)
				free_hotplug_page_range(pmd_page(pmd),
							PMD_SIZE, altmap);
			continue;
		}
		WARN_ON(!pmd_table(pmd));
		unmap_hotplug_pte_range(pmdp, addr, next, free_mapped, altmap);
	} while (addr = next, addr < end);
}

static void unmap_hotplug_pud_range(p4d_t *p4dp, unsigned long addr,
				    unsigned long end, bool free_mapped,
				    struct vmem_altmap *altmap)
{
	unsigned long next;
	pud_t *pudp, pud;

	do {
		next = pud_addr_end(addr, end);
		pudp = pud_offset(p4dp, addr);
		pud = READ_ONCE(*pudp);
		if (pud_none(pud))
			continue;

		WARN_ON(!pud_present(pud));
		if (pud_sect(pud)) {
			pud_clear(pudp);

			/*
			 * One TLBI should be sufficient here as the PUD_SIZE
			 * range is mapped with a single block entry.
			 */
			flush_tlb_kernel_range(addr, addr + PAGE_SIZE);
			if (free_mapped)
				free_hotplug_page_range(pud_page(pud),
							PUD_SIZE, altmap);
			continue;
		}
		WARN_ON(!pud_table(pud));
		unmap_hotplug_pmd_range(pudp, addr, next, free_mapped, altmap);
	} while (addr = next, addr < end);
}

static void unmap_hotplug_p4d_range(pgd_t *pgdp, unsigned long addr,
				    unsigned long end, bool free_mapped,
				    struct vmem_altmap *altmap)
{
	unsigned long next;
	p4d_t *p4dp, p4d;

	do {
		next = p4d_addr_end(addr, end);
		p4dp = p4d_offset(pgdp, addr);
		p4d = READ_ONCE(*p4dp);
		if (p4d_none(p4d))
			continue;

		WARN_ON(!p4d_present(p4d));
		unmap_hotplug_pud_range(p4dp, addr, next, free_mapped, altmap);
	} while (addr = next, addr < end);
}

static void unmap_hotplug_range(unsigned long addr, unsigned long end,
				bool free_mapped, struct vmem_altmap *altmap)
{
	unsigned long next;
	pgd_t *pgdp, pgd;

	/*
	 * altmap can only be used as vmemmap mapping backing memory.
	 * In case the backing memory itself is not being freed, then
	 * altmap is irrelevant. Warn about this inconsistency when
	 * encountered.
	 */
	WARN_ON(!free_mapped && altmap);

	do {
		next = pgd_addr_end(addr, end);
		pgdp = pgd_offset_k(addr);
		pgd = READ_ONCE(*pgdp);
		if (pgd_none(pgd))
			continue;

		WARN_ON(!pgd_present(pgd));
		unmap_hotplug_p4d_range(pgdp, addr, next, free_mapped, altmap);
	} while (addr = next, addr < end);
}

static void free_empty_pte_table(pmd_t *pmdp, unsigned long addr,
				 unsigned long end, unsigned long floor,
				 unsigned long ceiling)
{
	pte_t *ptep, pte;
	unsigned long i, start = addr;

	do {
		ptep = pte_offset_kernel(pmdp, addr);
		pte = READ_ONCE(*ptep);

		/*
		 * This is just a sanity check here which verifies that
		 * pte clearing has been done by earlier unmap loops.
		 */
		WARN_ON(!pte_none(pte));
	} while (addr += PAGE_SIZE, addr < end);

	if (!pgtable_range_aligned(start, end, floor, ceiling, PMD_MASK))
		return;

	/*
	 * Check whether we can free the pte page if the rest of the
	 * entries are empty. Overlap with other regions have been
	 * handled by the floor/ceiling check.
	 */
	ptep = pte_offset_kernel(pmdp, 0UL);
	for (i = 0; i < PTRS_PER_PTE; i++) {
		if (!pte_none(READ_ONCE(ptep[i])))
			return;
	}

	pmd_clear(pmdp);
	__flush_tlb_kernel_pgtable(start);
	free_hotplug_pgtable_page(virt_to_page(ptep));
}

static void free_empty_pmd_table(pud_t *pudp, unsigned long addr,
				 unsigned long end, unsigned long floor,
				 unsigned long ceiling)
{
	pmd_t *pmdp, pmd;
	unsigned long i, next, start = addr;

	do {
		next = pmd_addr_end(addr, end);
		pmdp = pmd_offset(pudp, addr);
		pmd = READ_ONCE(*pmdp);
		if (pmd_none(pmd))
			continue;

		WARN_ON(!pmd_present(pmd) || !pmd_table(pmd) || pmd_sect(pmd));
		free_empty_pte_table(pmdp, addr, next, floor, ceiling);
	} while (addr = next, addr < end);

	if (CONFIG_PGTABLE_LEVELS <= 2)
		return;

	if (!pgtable_range_aligned(start, end, floor, ceiling, PUD_MASK))
		return;

	/*
	 * Check whether we can free the pmd page if the rest of the
	 * entries are empty. Overlap with other regions have been
	 * handled by the floor/ceiling check.
	 */
	pmdp = pmd_offset(pudp, 0UL);
	for (i = 0; i < PTRS_PER_PMD; i++) {
		if (!pmd_none(READ_ONCE(pmdp[i])))
			return;
	}

	pud_clear(pudp);
	__flush_tlb_kernel_pgtable(start);
	free_hotplug_pgtable_page(virt_to_page(pmdp));
}

static void free_empty_pud_table(p4d_t *p4dp, unsigned long addr,
				 unsigned long end, unsigned long floor,
				 unsigned long ceiling)
{
	pud_t *pudp, pud;
	unsigned long i, next, start = addr;

	do {
		next = pud_addr_end(addr, end);
		pudp = pud_offset(p4dp, addr);
		pud = READ_ONCE(*pudp);
		if (pud_none(pud))
			continue;

		WARN_ON(!pud_present(pud) || !pud_table(pud) || pud_sect(pud));
		free_empty_pmd_table(pudp, addr, next, floor, ceiling);
	} while (addr = next, addr < end);

	if (CONFIG_PGTABLE_LEVELS <= 3)
		return;

	if (!pgtable_range_aligned(start, end, floor, ceiling, PGDIR_MASK))
		return;

	/*
	 * Check whether we can free the pud page if the rest of the
	 * entries are empty. Overlap with other regions have been
	 * handled by the floor/ceiling check.
	 */
	pudp = pud_offset(p4dp, 0UL);
	for (i = 0; i < PTRS_PER_PUD; i++) {
		if (!pud_none(READ_ONCE(pudp[i])))
			return;
	}

	p4d_clear(p4dp);
	__flush_tlb_kernel_pgtable(start);
	free_hotplug_pgtable_page(virt_to_page(pudp));
}

static void free_empty_p4d_table(pgd_t *pgdp, unsigned long addr,
				 unsigned long end, unsigned long floor,
				 unsigned long ceiling)
{
	unsigned long next;
	p4d_t *p4dp, p4d;

	do {
		next = p4d_addr_end(addr, end);
		p4dp = p4d_offset(pgdp, addr);
		p4d = READ_ONCE(*p4dp);
		if (p4d_none(p4d))
			continue;

		WARN_ON(!p4d_present(p4d));
		free_empty_pud_table(p4dp, addr, next, floor, ceiling);
	} while (addr = next, addr < end);
}

static void free_empty_tables(unsigned long addr, unsigned long end,
			      unsigned long floor, unsigned long ceiling)
{
	unsigned long next;
	pgd_t *pgdp, pgd;

	do {
		next = pgd_addr_end(addr, end);
		pgdp = pgd_offset_k(addr);
		pgd = READ_ONCE(*pgdp);
		if (pgd_none(pgd))
			continue;

		WARN_ON(!pgd_present(pgd));
		free_empty_p4d_table(pgdp, addr, next, floor, ceiling);
	} while (addr = next, addr < end);
}
#endif

#ifdef CONFIG_SPARSEMEM_VMEMMAP
#if !ARM64_SWAPPER_USES_SECTION_MAPS
int __meminit vmemmap_populate(unsigned long start, unsigned long end, int node,
		struct vmem_altmap *altmap)
{
	return vmemmap_populate_basepages(start, end, node, altmap);
}
#else	/* !ARM64_SWAPPER_USES_SECTION_MAPS */
int __meminit vmemmap_populate(unsigned long start, unsigned long end, int node,
		struct vmem_altmap *altmap)
{
	unsigned long addr = start;
	unsigned long next;
	pgd_t *pgdp;
	p4d_t *p4dp;
	pud_t *pudp;
	pmd_t *pmdp;

	do {
		next = pmd_addr_end(addr, end);

		pgdp = vmemmap_pgd_populate(addr, node);
		if (!pgdp)
			return -ENOMEM;

		p4dp = vmemmap_p4d_populate(pgdp, addr, node);
		if (!p4dp)
			return -ENOMEM;

		pudp = vmemmap_pud_populate(p4dp, addr, node);
		if (!pudp)
			return -ENOMEM;

		pmdp = pmd_offset(pudp, addr);
		if (pmd_none(READ_ONCE(*pmdp))) {
			void *p = NULL;

			p = vmemmap_alloc_block_buf(PMD_SIZE, node, altmap);
			if (!p)
				return -ENOMEM;

			pmd_set_huge(pmdp, __pa(p), __pgprot(PROT_SECT_NORMAL));
		} else
			vmemmap_verify((pte_t *)pmdp, node, addr, next);
	} while (addr = next, addr != end);

	return 0;
}
#endif	/* !ARM64_SWAPPER_USES_SECTION_MAPS */
void vmemmap_free(unsigned long start, unsigned long end,
		struct vmem_altmap *altmap)
{
#ifdef CONFIG_MEMORY_HOTPLUG
	WARN_ON((start < VMEMMAP_START) || (end > VMEMMAP_END));

	unmap_hotplug_range(start, end, true, altmap);
	free_empty_tables(start, end, VMEMMAP_START, VMEMMAP_END);
#endif
}
#endif	/* CONFIG_SPARSEMEM_VMEMMAP */

static inline pud_t * fixmap_pud(unsigned long addr)
{
	pgd_t *pgdp = pgd_offset_k(addr);
	p4d_t *p4dp = p4d_offset(pgdp, addr);
	p4d_t p4d = READ_ONCE(*p4dp);

	BUG_ON(p4d_none(p4d) || p4d_bad(p4d));

	return pud_offset_kimg(p4dp, addr);
}

static inline pmd_t * fixmap_pmd(unsigned long addr)
{
	pud_t *pudp = fixmap_pud(addr);
	pud_t pud = READ_ONCE(*pudp);

	BUG_ON(pud_none(pud) || pud_bad(pud));

	return pmd_offset_kimg(pudp, addr);
}

static inline pte_t * fixmap_pte(unsigned long addr)
{
	return &bm_pte[pte_index(addr)];
}

/*
 * The p*d_populate functions call virt_to_phys implicitly so they can't be used
 * directly on kernel symbols (bm_p*d). This function is called too early to use
 * lm_alias so __p*d_populate functions must be used to populate with the
 * physical address from __pa_symbol.
 */
void __init early_fixmap_init(void)
{
	pgd_t *pgdp;
	p4d_t *p4dp, p4d;
	pud_t *pudp;
	pmd_t *pmdp;
	unsigned long addr = FIXADDR_START;

	pgdp = pgd_offset_k(addr);
	p4dp = p4d_offset(pgdp, addr);
	p4d = READ_ONCE(*p4dp);
	if (CONFIG_PGTABLE_LEVELS > 3 &&
	    !(p4d_none(p4d) || p4d_page_paddr(p4d) == __pa_symbol(bm_pud))) {
		/*
		 * We only end up here if the kernel mapping and the fixmap
		 * share the top level pgd entry, which should only happen on
		 * 16k/4 levels configurations.
		 */
		BUG_ON(!IS_ENABLED(CONFIG_ARM64_16K_PAGES));
		pudp = pud_offset_kimg(p4dp, addr);
	} else {
		if (p4d_none(p4d))
			__p4d_populate(p4dp, __pa_symbol(bm_pud), PUD_TYPE_TABLE);
		pudp = fixmap_pud(addr);
	}
	if (pud_none(READ_ONCE(*pudp)))
		__pud_populate(pudp, __pa_symbol(bm_pmd), PMD_TYPE_TABLE);
	pmdp = fixmap_pmd(addr);
	__pmd_populate(pmdp, __pa_symbol(bm_pte), PMD_TYPE_TABLE);

	/*
	 * The boot-ioremap range spans multiple pmds, for which
	 * we are not prepared:
	 */
	BUILD_BUG_ON((__fix_to_virt(FIX_BTMAP_BEGIN) >> PMD_SHIFT)
		     != (__fix_to_virt(FIX_BTMAP_END) >> PMD_SHIFT));

	if ((pmdp != fixmap_pmd(fix_to_virt(FIX_BTMAP_BEGIN)))
	     || pmdp != fixmap_pmd(fix_to_virt(FIX_BTMAP_END))) {
		WARN_ON(1);
		pr_warn("pmdp %p != %p, %p\n",
			pmdp, fixmap_pmd(fix_to_virt(FIX_BTMAP_BEGIN)),
			fixmap_pmd(fix_to_virt(FIX_BTMAP_END)));
		pr_warn("fix_to_virt(FIX_BTMAP_BEGIN): %08lx\n",
			fix_to_virt(FIX_BTMAP_BEGIN));
		pr_warn("fix_to_virt(FIX_BTMAP_END):   %08lx\n",
			fix_to_virt(FIX_BTMAP_END));

		pr_warn("FIX_BTMAP_END:       %d\n", FIX_BTMAP_END);
		pr_warn("FIX_BTMAP_BEGIN:     %d\n", FIX_BTMAP_BEGIN);
	}
}

/**
 * Unusually, this is also called in IRQ context (ghes_iounmap_irq) so if we
 * ever need to use IPIs for TLB broadcasting, then we're in trouble here.
 * (不寻常的是，该函数（ghes_iounmap_irq）也会在IRQ上下文中被调用。因此，如果我们将来需要使用IPI（处理器间中断）进行TLB广播，这里就会出问题。)
 */
void __set_fixmap(enum fixed_addresses idx, phys_addr_t phys, pgprot_t flags)
{
	unsigned long addr = __fix_to_virt(idx);
	pte_t *ptep;

	BUG_ON(idx <= FIX_HOLE || idx >= __end_of_fixed_addresses);

	ptep = fixmap_pte(addr);

	if (pgprot_val(flags)) {
		set_pte(ptep, pfn_pte(phys >> PAGE_SHIFT, flags));
	} else {
		pte_clear(&init_mm, addr, ptep);
		flush_tlb_kernel_range(addr, addr + PAGE_SIZE);
	}
}

void *__init fixmap_remap_fdt(phys_addr_t dt_phys, int *size, pgprot_t prot)
{
	const u64 dt_virt_base = __fix_to_virt(FIX_FDT);
	int offset;
	void *dt_virt;

	/*
	 * Check whether the physical FDT address is set and meets the minimum
	 * alignment requirement. Since we are relying on MIN_FDT_ALIGN to be
	 * at least 8 bytes so that we can always access the magic and size
	 * fields of the FDT header after mapping the first chunk, double check
	 * here if that is indeed the case.
	 */
	BUILD_BUG_ON(MIN_FDT_ALIGN < 8);
	if (!dt_phys || dt_phys % MIN_FDT_ALIGN)
		return NULL;

	/*
	 * Make sure that the FDT region can be mapped without the need to
	 * allocate additional translation table pages, so that it is safe
	 * to call create_mapping_noalloc() this early.
	 *
	 * On 64k pages, the FDT will be mapped using PTEs, so we need to
	 * be in the same PMD as the rest of the fixmap.
	 * On 4k pages, we'll use section mappings for the FDT so we only
	 * have to be in the same PUD.
	 */
	BUILD_BUG_ON(dt_virt_base % SZ_2M);

	BUILD_BUG_ON(__fix_to_virt(FIX_FDT_END) >> SWAPPER_TABLE_SHIFT !=
		     __fix_to_virt(FIX_BTMAP_BEGIN) >> SWAPPER_TABLE_SHIFT);

	offset = dt_phys % SWAPPER_BLOCK_SIZE;
	dt_virt = (void *)dt_virt_base + offset;

	/* map the first chunk so we can read the size from the header */
	create_mapping_noalloc(round_down(dt_phys, SWAPPER_BLOCK_SIZE),
			dt_virt_base, SWAPPER_BLOCK_SIZE, prot);

	if (fdt_magic(dt_virt) != FDT_MAGIC)
		return NULL;

	*size = fdt_totalsize(dt_virt);
	if (*size > MAX_FDT_SIZE)
		return NULL;

	if (offset + *size > SWAPPER_BLOCK_SIZE)
		create_mapping_noalloc(round_down(dt_phys, SWAPPER_BLOCK_SIZE), dt_virt_base,
			       round_up(offset + *size, SWAPPER_BLOCK_SIZE), prot);

	return dt_virt;
}

int __init arch_ioremap_p4d_supported(void)
{
	return 0;
}

int __init arch_ioremap_pud_supported(void)
{
	/*
	 * Only 4k granule supports level 1 block mappings.
	 * SW table walks can't handle removal of intermediate entries.
	 */
	return IS_ENABLED(CONFIG_ARM64_4K_PAGES) &&
	       !IS_ENABLED(CONFIG_PTDUMP_DEBUGFS);
}

int __init arch_ioremap_pmd_supported(void)
{
	/* See arch_ioremap_pud_supported() */
	return !IS_ENABLED(CONFIG_PTDUMP_DEBUGFS);
}

int pud_set_huge(pud_t *pudp, phys_addr_t phys, pgprot_t prot)
{
	pud_t new_pud = pfn_pud(__phys_to_pfn(phys), mk_pud_sect_prot(prot));

	/* Only allow permission changes for now */
	if (!pgattr_change_is_safe(READ_ONCE(pud_val(*pudp)),
				   pud_val(new_pud)))
		return 0;

	VM_BUG_ON(phys & ~PUD_MASK);
	set_pud(pudp, new_pud);
	return 1;
}

int pmd_set_huge(pmd_t *pmdp, phys_addr_t phys, pgprot_t prot)
{
	pmd_t new_pmd = pfn_pmd(__phys_to_pfn(phys), mk_pmd_sect_prot(prot));

	/* Only allow permission changes for now */
	if (!pgattr_change_is_safe(READ_ONCE(pmd_val(*pmdp)),
				   pmd_val(new_pmd)))
		return 0;

	VM_BUG_ON(phys & ~PMD_MASK);
	set_pmd(pmdp, new_pmd);
	return 1;
}

int pud_clear_huge(pud_t *pudp)
{
	if (!pud_sect(READ_ONCE(*pudp)))
		return 0;
	pud_clear(pudp);
	return 1;
}

int pmd_clear_huge(pmd_t *pmdp)
{
	if (!pmd_sect(READ_ONCE(*pmdp)))
		return 0;
	pmd_clear(pmdp);
	return 1;
}

int pmd_free_pte_page(pmd_t *pmdp, unsigned long addr)
{
	pte_t *table;
	pmd_t pmd;

	pmd = READ_ONCE(*pmdp);

	if (!pmd_table(pmd)) {
		VM_WARN_ON(1);
		return 1;
	}

	table = pte_offset_kernel(pmdp, addr);
	pmd_clear(pmdp);
	__flush_tlb_kernel_pgtable(addr);
	pte_free_kernel(NULL, table);
	return 1;
}

int pud_free_pmd_page(pud_t *pudp, unsigned long addr)
{
	pmd_t *table;
	pmd_t *pmdp;
	pud_t pud;
	unsigned long next, end;

	pud = READ_ONCE(*pudp);

	if (!pud_table(pud)) {
		VM_WARN_ON(1);
		return 1;
	}

	table = pmd_offset(pudp, addr);
	pmdp = table;
	next = addr;
	end = addr + PUD_SIZE;
	do {
		pmd_free_pte_page(pmdp, next);
	} while (pmdp++, next += PMD_SIZE, next != end);

	pud_clear(pudp);
	__flush_tlb_kernel_pgtable(addr);
	pmd_free(NULL, table);
	return 1;
}

int p4d_free_pud_page(p4d_t *p4d, unsigned long addr)
{
	return 0;	/* Don't attempt a block mapping */
}

#ifdef CONFIG_MEMORY_HOTPLUG
static void __remove_pgd_mapping(pgd_t *pgdir, unsigned long start, u64 size)
{
	unsigned long end = start + size;

	WARN_ON(pgdir != init_mm.pgd);
	WARN_ON((start < PAGE_OFFSET) || (end > PAGE_END));

	unmap_hotplug_range(start, end, false, NULL);
	free_empty_tables(start, end, PAGE_OFFSET, PAGE_END);
}

int arch_add_memory(int nid, u64 start, u64 size,
		    struct mhp_params *params)
{
	int ret, flags = 0;

	if (rodata_full || debug_pagealloc_enabled())
		flags = NO_BLOCK_MAPPINGS | NO_CONT_MAPPINGS;

	__create_pgd_mapping(swapper_pg_dir, start, __phys_to_virt(start),
			     size, params->pgprot, __pgd_pgtable_alloc,
			     flags);

	memblock_clear_nomap(start, size);

	ret = __add_pages(nid, start >> PAGE_SHIFT, size >> PAGE_SHIFT,
			   params);
	if (ret)
		__remove_pgd_mapping(swapper_pg_dir,
				     __phys_to_virt(start), size);
	return ret;
}

void arch_remove_memory(int nid, u64 start, u64 size,
			struct vmem_altmap *altmap)
{
	unsigned long start_pfn = start >> PAGE_SHIFT;
	unsigned long nr_pages = size >> PAGE_SHIFT;

	__remove_pages(start_pfn, nr_pages, altmap);
	__remove_pgd_mapping(swapper_pg_dir, __phys_to_virt(start), size);
}

/*
 * This memory hotplug notifier helps prevent boot memory from being
 * inadvertently removed as it blocks pfn range offlining process in
 * __offline_pages(). Hence this prevents both offlining as well as
 * removal process for boot memory which is initially always online.
 * In future if and when boot memory could be removed, this notifier
 * should be dropped and free_hotplug_page_range() should handle any
 * reserved pages allocated during boot.
 */
static int prevent_bootmem_remove_notifier(struct notifier_block *nb,
					   unsigned long action, void *data)
{
	struct mem_section *ms;
	struct memory_notify *arg = data;
	unsigned long end_pfn = arg->start_pfn + arg->nr_pages;
	unsigned long pfn = arg->start_pfn;

	if (action != MEM_GOING_OFFLINE)
		return NOTIFY_OK;

	for (; pfn < end_pfn; pfn += PAGES_PER_SECTION) {
		ms = __pfn_to_section(pfn);
		if (early_section(ms))
			return NOTIFY_BAD;
	}
	return NOTIFY_OK;
}

static struct notifier_block prevent_bootmem_remove_nb = {
	.notifier_call = prevent_bootmem_remove_notifier,
};

static int __init prevent_bootmem_remove_init(void)
{
	return register_memory_notifier(&prevent_bootmem_remove_nb);
}
device_initcall(prevent_bootmem_remove_init);
#endif
