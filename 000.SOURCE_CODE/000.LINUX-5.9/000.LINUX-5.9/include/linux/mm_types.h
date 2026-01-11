/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MM_TYPES_H
#define _LINUX_MM_TYPES_H

#include <linux/mm_types_task.h>

#include <linux/auxvec.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/rbtree.h>
#include <linux/rwsem.h>
#include <linux/completion.h>
#include <linux/cpumask.h>
#include <linux/uprobes.h>
#include <linux/page-flags-layout.h>
#include <linux/workqueue.h>

#include <asm/mmu.h>

#ifndef AT_VECTOR_SIZE_ARCH
#define AT_VECTOR_SIZE_ARCH 0
#endif
#define AT_VECTOR_SIZE (2*(AT_VECTOR_SIZE_ARCH + AT_VECTOR_SIZE_BASE + 1))


struct address_space;
struct mem_cgroup;

/*
 * Each physical page in the system has a struct page associated with
 * it to keep track of whatever it is we are using the page for at the
 * moment. Note that we have no way to track which tasks are using
 * a page, though if it is a pagecache page, rmap structures can tell us
 * who is mapping it.
 *
 * If you allocate the page using alloc_pages(), you can use some of the
 * space in struct page for your own purposes.  The five words in the main
 * union are available, except for bit 0 of the first word which must be
 * kept clear.  Many users use this word to store a pointer to an object
 * which is guaranteed to be aligned.  If you use the same storage as
 * page->mapping, you must restore it to NULL before freeing the page.
 *
 * If your page will not be mapped to userspace, you can also use the four
 * bytes in the mapcount union, but you must call page_mapcount_reset()
 * before freeing it.
 *
 * If you want to use the refcount field, it must be used in such a way
 * that other CPUs temporarily incrementing and then decrementing the
 * refcount does not cause problems.  On receiving the page from
 * alloc_pages(), the refcount will be positive.
 *
 * If you allocate pages of order > 0, you can use some of the fields
 * in each subpage, but you may need to restore some of their values
 * afterwards.
 *
 * SLUB uses cmpxchg_double() to atomically update its freelist and
 * counters.  That requires that freelist & counters be adjacent and
 * double-word aligned.  We align all struct pages to double-word
 * boundaries, and ensure that 'freelist' is aligned within the
 * struct.
 */
#ifdef CONFIG_HAVE_ALIGNED_STRUCT_PAGE
#define _struct_page_alignment	__aligned(2 * sizeof(unsigned long))
#else
#define _struct_page_alignment
#endif

/**
 * 
 * [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#5.1.1　page数据结构
 * [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#5.2　RMAP       -- Reverse Mapping Map （反向映射）,确定页面是否被某个进程映射
 * [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#5.3 页面回收     -- 页交换(swapping) 、页回收(page reclaim)
 * [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#5.3.1　LRU链表   -- 页交换算法(Linux内核中采用的页交换算法主要是经典LRU链表算法和第二次机会（second chance）法)
 * 
 * 
 * 页面回收:
 *   [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#5.3.3　触发页面回收
 */
struct page {
	unsigned long flags;		/* Atomic flags, some possibly
					 * updated asynchronously */
	/*
	 * Five words (20/40 bytes) are available in this union.
	 * WARNING: bit 0 of the first word is used for PageTail(). That
	 * means the other users of this union MUST NOT use the bit to
	 * avoid collision and false-positive PageTail().
	 */
	union {
		struct {	/* Page cache and anonymous pages */
			/**
			 * @lru: Pageout list, eg. active_list protected by
			 * pgdat->lru_lock.  Sometimes used as a generic list
			 * by the page owner.
			 * lru: LRU链表节点，匿名页面或文件映射页面会通过该成员添加到LRU链表中
			 */
			struct list_head lru;
			/** See page-flags.h for PAGE_MAPPING_FLAGS
			 * 
			 * mapping成员表示页面所指向的地址空间 , 见[Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#5.1.5　mapping成员的妙用
			 * 
			 */
			struct address_space *mapping;
			/**
			 * 看代码:
			 * static inline void set_pcppage_migratetype(struct page *page, int migratetype)
             *  {
             *  	page->index = migratetype;
             *  }
			 * 存储的是 migratetype 
			 * 
			 * index: 表示这个页面在一个映射中的序号或偏移量???
			 */
			pgoff_t index;		/* Our offset within mapping. */
			/**
			 * @private: 指向私有数据的指针
			 * @private: Mapping-private opaque data.(映射私有不透明数据)
			 * Usually used for buffer_heads if PagePrivate.
			 * Used for swp_entry_t if PageSwapCache.
			 * Indicates order in the buddy system if PageBuddy.
			 */
			unsigned long private;
		};
		struct {	/* page_pool used by netstack */
			/**
			 * @dma_addr: might require a 64-bit value even on
			 * 32-bit architectures.
			 */
			dma_addr_t dma_addr;
		};
		struct {	/* slab, slob and slub */
			union {
				/**
				 * slab_list: slab链表节点
				 */
				struct list_head slab_list;
				struct {	/* Partial pages： 部分页， 内存页没有完全被填满*/
					/**
					 * next: 在slub分配器中使用
					 */
					struct page *next;
#ifdef CONFIG_64BIT
					int pages;	/* Nr of pages left */
					int pobjects;	/* Approximate count */
#else
					short int pages;
					short int pobjects;
#endif
				};
			};
			/**
			 * slab_alloc 从该函数开始分析，slab.c
			 * 页面对应的slab描述符，
			 * 即 当前的page是被slab_cache所使用的
			 * 
			 * slab_cache: slab缓存描述符，slab分配器中的第一个物理页面的page数据结构中的slab_cache指向slab缓存描述符
			 */
			struct kmem_cache *slab_cache; /* not slob */
			/**
			 *  Double-word boundary 
			 * slab的管理信息
			 * freelist: 管理区。管理区可以看作一个数组，数组的每个成员占用1字节，每个成员代表一个slab对象
			 * 
			*/
			void *freelist;		/* first free object */
			union {
				/**
				 * s_sem: 在slab分配器中用来指向第一个slab对象的起始地址:
				 * 参考: static inline void *index_to_obj(struct kmem_cache *cache, struct page *page,
				 unsigned int idx); 由 cache_alloc_refill 函数调用
				 */
				void *s_mem;	/* slab: first object */
				unsigned long counters;		/* SLUB */
				struct {			/* SLUB */
					unsigned inuse:16;
					unsigned objects:15;
					unsigned frozen:1;
				};
			};
		};
		struct {	/* Tail pages of compound page */
			unsigned long compound_head;	/* Bit zero is set */

			/* First tail page only */
			unsigned char compound_dtor;
			unsigned char compound_order;
			atomic_t compound_mapcount;
			unsigned int compound_nr; /* 1 << compound_order */
		};
		struct {	/* Second tail page of compound page */
			unsigned long _compound_pad_1;	/* compound_head */
			atomic_t hpage_pinned_refcount;
			/* For both global and memcg */
			struct list_head deferred_list;
		};
		struct {	/* Page table pages */
			unsigned long _pt_pad_1;	/* compound_head */
			pgtable_t pmd_huge_pte; /* protected by page->ptl */
			unsigned long _pt_pad_2;	/* mapping */
			union {
				struct mm_struct *pt_mm; /* x86 pgds only */
				atomic_t pt_frag_refcount; /* powerpc */
			};
#if ALLOC_SPLIT_PTLOCKS
			spinlock_t *ptl;
#else
			spinlock_t ptl;
#endif
		};
		struct {	/* ZONE_DEVICE pages */
			/** @pgmap: Points to the hosting device page map. */
			struct dev_pagemap *pgmap;
			void *zone_device_data;
			/*
			 * ZONE_DEVICE private pages are counted as being
			 * mapped so the next 3 words hold the mapping, index,
			 * and private fields from the source anonymous or
			 * page cache page while the page is migrated to device
			 * private memory.
			 * ZONE_DEVICE MEMORY_DEVICE_FS_DAX pages also
			 * use the mapping, index, and private fields when
			 * pmem backed DAX files are mapped.
			 */
		};

		/** @rcu_head: You can use this to free a page by RCU. */
		struct rcu_head rcu_head;
	};

	union {		/* This union is 4 bytes in size. */
		/*
		 * If the page can be mapped to userspace, encodes the number
		 * of times this page is referenced by a page table.
		 */
		/**
		 * 表示这个页面被进程映射的个数，即已经映射了多少个用户PTE
		 * [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#5.1.3　_mapcount的应用
		 *   若_mapcount等于−1，表示没有PTE映射到页面。
         *   若_mapcount等于0，表示只有父进程映射到页面
		 *   _mapcount大于0，表示除了父进程外还有其他进程映射到这个页面
		 */
		atomic_t _mapcount;

		/*
		 * If the page is neither PageSlab nor mappable to userspace,
		 * the value stored here may help determine what this page
		 * is used for.  See page-flags.h for a list of page types
		 * which are currently stored here.
		 */
		unsigned int page_type;

		/**
		 * 表示slab分配器中活跃对象的数量。当为0时，表示这个slab分配器中没有活跃对象，可以销毁这个slab分配器。活跃对象就是已经被迁移到对象缓冲池中的对象
		 */
		unsigned int active;		/* SLAB */
		int units;			/* SLOB */
	};

	/* Usage count. *DO NOT USE DIRECTLY*. See page_ref.h */
	// 表示内核引用该页面的次数,用于跟踪页面使用情况: 不是被进程映射的个数
	atomic_t _refcount;

#ifdef CONFIG_MEMCG
	union {
		struct mem_cgroup *mem_cgroup;
		struct obj_cgroup **obj_cgroups;
	};
#endif

	/*
	 * On machines where all RAM is mapped into kernel address space,
	 * we can simply calculate the virtual address. On machines with
	 * highmem some memory is mapped into kernel virtual memory
	 * dynamically, so we need a place to store that address.
	 * Note that this field could be 16 bits on x86 ... ;)
	 *
	 * Architectures with slow multiplication can define
	 * WANT_PAGE_VIRTUAL in asm/page.h
	 */
#if defined(WANT_PAGE_VIRTUAL)
	void *virtual;			/* Kernel virtual address (NULL if
					   not kmapped, ie. highmem) */
#endif /* WANT_PAGE_VIRTUAL */

#ifdef LAST_CPUPID_NOT_IN_PAGE_FLAGS
	int _last_cpupid;
#endif
} _struct_page_alignment;

static inline atomic_t *compound_mapcount_ptr(struct page *page)
{
	return &page[1].compound_mapcount;
}

static inline atomic_t *compound_pincount_ptr(struct page *page)
{
	return &page[2].hpage_pinned_refcount;
}

/**
 * Used for sizing the vmemmap region on some architectures
 * 
 * order_base_2(n):内核提供的函数，计算不小于 n 的最小 2 的幂次的对数
 */
#define STRUCT_PAGE_MAX_SHIFT	(order_base_2(sizeof(struct page)))

#define PAGE_FRAG_CACHE_MAX_SIZE	__ALIGN_MASK(32768, ~PAGE_MASK)
#define PAGE_FRAG_CACHE_MAX_ORDER	get_order(PAGE_FRAG_CACHE_MAX_SIZE)

#define page_private(page)		((page)->private)

static inline void set_page_private(struct page *page, unsigned long private)
{
	page->private = private;
}

struct page_frag_cache {
	void * va;
#if (PAGE_SIZE < PAGE_FRAG_CACHE_MAX_SIZE)
	__u16 offset;
	__u16 size;
#else
	__u32 offset;
#endif
	/* we maintain a pagecount bias, so that we dont dirty cache line
	 * containing page->_refcount every time we allocate a fragment.
	 */
	unsigned int		pagecnt_bias;
	bool pfmemalloc;
};

typedef unsigned long vm_flags_t;

/*
 * A region containing a mapping of a non-memory backed file under NOMMU
 * conditions.  These are held in a global tree and are pinned by the VMAs that
 * map parts of them.
 */
struct vm_region {
	struct rb_node	vm_rb;		/* link in global region tree */
	vm_flags_t	vm_flags;	/* VMA vm_flags */
	unsigned long	vm_start;	/* start address of region */
	unsigned long	vm_end;		/* region initialised to here */
	unsigned long	vm_top;		/* region allocated to here */
	unsigned long	vm_pgoff;	/* the offset in vm_file corresponding to vm_start */
	struct file	*vm_file;	/* the backing file or NULL */

	int		vm_usage;	/* region usage count (access under nommu_region_sem) */
	bool		vm_icache_flushed : 1; /* true if the icache has been flushed for
						* this region */
};

#ifdef CONFIG_USERFAULTFD
#define NULL_VM_UFFD_CTX ((struct vm_userfaultfd_ctx) { NULL, })
struct vm_userfaultfd_ctx {
	struct userfaultfd_ctx *ctx;
};
#else /* CONFIG_USERFAULTFD */
#define NULL_VM_UFFD_CTX ((struct vm_userfaultfd_ctx) {})
struct vm_userfaultfd_ctx {};
#endif /* CONFIG_USERFAULTFD */

/**
 * This struct describes a virtual memory area. There is one of these
 * per VM-area/task. A VM area is any part of the process virtual memory
 * space that has a special rule for the page-fault handlers (ie a shared
 * library, the executable area etc).
 * (该结构体描述了一个虚拟内存区域（Virtual Memory Area, VMA）。每个虚拟内存区域或每个任务（进程）都对应一个这样的结构。
 * 虚拟内存区域是指进程虚拟内存空间中，任何对**页面异常处理程序（page-fault handlers）**有特殊规则的特定部分（例如：共享库、可执行代码区等）)
 * 
 * VMA
 * 
 * 从进程的角度看 VMA：[Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#图4.20　从进程角度看VMA
 */
struct vm_area_struct {
	/* The first cache line has the info for VMA tree walking. */
    /**
	 * vm_start和vm_end：指定VMA在进程地址空间的起始地址和结束地址 ?
	 * 这里的进程地址空间是什么? 看图就很容易理解了
	 * - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#图4.20　从进程角度看VMA
	 */ 
	unsigned long vm_start;		/* Our start address within vm_mm. */
	unsigned long vm_end;		/* The first byte after our end address
					   within vm_mm. */

	/**
	 *  linked list of VM areas per task, sorted by address
	 * vm_next和vm_prev：进程的VMA都连接成一个链表。
	 */
	struct vm_area_struct *vm_next, *vm_prev;

	/**
	 * vm_rb：VMA作为一个节点加入红黑树，每个进程的mm_struct数据结构中都有一棵红黑树——mm->mm_rb
	 */
	struct rb_node vm_rb;

	/*
	 * Largest free memory gap in bytes to the left of this VMA.
	 * Either between this VMA and vma->vm_prev, or between one of the
	 * VMAs below us in the VMA rbtree and its ->vm_prev. This helps
	 * get_unmapped_area find a free area of the right size.
	 */
	unsigned long rb_subtree_gap;

	/* Second cache line starts here. */

	struct mm_struct *vm_mm;	/* The address space we belong to. (vm_mm：指向该VMA所属进程的mm_struct数据结构。)*/

	/*
	 * Access permissions of this VMA.
	 * See vmf_insert_mixed_prot() for discussion.
	 * 
	 * vm_page_prot：VMA的访问权限
	 * 
	 * VMA属性的标志位可以任意组合，`但是最终要落实到硬件机制上`，即页表项的属性中:  (IN [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#图4.21　VMA属性到页表项属性的转换)
	 * 转换的必要性：VMA的属性是操作系统抽象出来的，与具体硬件无关。
	 *      但是，当操作系统为VMA建立页表映射时，必须将VMA的属性转换为具体处理器架构的页表项属性。
	 *      这是因为硬件在访问内存时，会根据页表项的属性来进行权限检查和控制   
	 * 
	 * 即： 访问属性机制还是通过硬件来实现的，如： 缺页异常机制...
	 * 
	 * 且，不同的硬件平台，机制可能不一致
	 * 
	 * 转换函数: vm_get_page_prot  [000.LINUX-5.9/mm/mmap.c]
	 */
	pgprot_t vm_page_prot;
	/**
	 * vm_flags：描述该VMA的一组标志位, 即 页面的属性，最终还是得落实到硬件机制上，见:
	 * - [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#图4.21　VMA属性到页表项属性的转换
	 * <mm/mmap.c> # pgprot_t vm_get_page_prot(unsigned long vm_flags)
	 * 
	 */
	unsigned long vm_flags;		/* Flags, see mm.h. */

	/*
	 * For areas with an address space and backing store,
	 * linkage into the address_space->i_mmap interval tree.
	 */
	struct {
		struct rb_node rb;
		unsigned long rb_subtree_last;
	} shared;

	/**
	 * [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#5.2　RMAP
	 * 
	 * RMAP系统中有两个重要的数据结构：一个是anon_vma，简称AV；另一个是anon_vma_chain，简称AVC。
	 * 
	 * A file's MAP_PRIVATE vma can be in both i_mmap tree and anon_vma
	 * list, after a COW of one of the file pages.	A MAP_SHARED vma
	 * can only be in the i_mmap tree.  An anonymous MAP_PRIVATE, stack
	 * or brk vma (with NULL file) can only be in an anon_vma list.
	 */
	struct list_head anon_vma_chain; /* Serialized by mmap_lock &
					  * page_table_lock */
	struct anon_vma *anon_vma;	/* Serialized by page_table_lock */

	/** Function pointers to deal with this struct. 
	 * 指向许多方法的集合，这些方法用于在VMA中执行各种操作，通常用于文件映射
	*/
	const struct vm_operations_struct *vm_ops;

	/** Information about our backing store: 
	 * vm_pgoff：指定文件映射的偏移量，这个变量的单位不是字节，而是页面的大小（PAGE_SIZE）。对于匿名页面来说，它的值可以是0或者vm_addr/PAGE_SIZE。
	*/
	unsigned long vm_pgoff;		/* Offset (within vm_file) in PAGE_SIZE  units */
	/**
	  *  vm_file：指向file的实例，描述一个被映射的文件
	*/
	struct file *vm_file; /* File we map to (can be NULL). */
	void * vm_private_data;		/* was vm_pte (shared mem) */

#ifdef CONFIG_SWAP
	atomic_long_t swap_readahead_info;
#endif
#ifndef CONFIG_MMU
	struct vm_region *vm_region;	/* NOMMU mapping region */
#endif
#ifdef CONFIG_NUMA
	struct mempolicy *vm_policy;	/* NUMA policy for the VMA */
#endif
	struct vm_userfaultfd_ctx vm_userfaultfd_ctx;
} __randomize_layout;

struct core_thread {
	struct task_struct *task;
	struct core_thread *next;
};

struct core_state {
	atomic_t nr_threads;
	struct core_thread dumper;
	struct completion startup;
};

struct kioctx_table;
/**
 * Linux内核需要管理每个进程所有的内存区域以及它们对应的页表映射，所以必须抽象出一个数据结构，这就是mm_struct数据结构
 * > 参考:[001.UNIX-DOCS/000.内存管理/013.进程内存管理/README.md]: 进程可以通过内核的内存管理机制动态地添加和删除这些内存区域，这些内存区域在Linux内核采用VMA数据结构(struct vm_area_struct , struct mm_struct成员)来抽象描述
 * 
 * > [Run Linux Kernel (2nd Edition) Volume 1: Infrastructure.epub]#图4.19　mm_struct数据结构
 *   - 该结构体映射一个完整的虚拟地址空间 
 */
struct mm_struct {
	struct {
		struct vm_area_struct *mmap;		/* list of VMAs  VMA 链表: 管理 vm_area_struct 的数据结构之一*/
		/**
		 * 管理 vm_area_struct 的数据结构之二: VMA红黑树的根节点
		 * 
		 * 所以，通过 mmap 或 mm_rb 都可以遍历和查找所有的VMA
		 */
		struct rb_root mm_rb;
		u64 vmacache_seqnum;                   /* per-thread vmacache */
#ifdef CONFIG_MMU
		  /**
           * get_unmapped_area：用于判断虚拟内存空间是否有足够的空间，返回一段没有映射过的空间的起始地址，这个函数会使用具体的处理器架构的实现，如对于ARM架构，Linux内核就有相应的函数实现。
          */
		unsigned long (*get_unmapped_area)(struct file *filp,
						   unsigned long addr,
						   unsigned long len,
						   unsigned long pgoff,
						   unsigned long flags);
#endif
        /**
		 * mmap_base：指向mmap空间的起始地址。在32位处理器中，mmap空间的起始地址是0x4000 0000
		 */
		unsigned long mmap_base;	/* base of mmap area */
		unsigned long mmap_legacy_base;	/* base of mmap area in bottom-up allocations */
#ifdef CONFIG_HAVE_ARCH_COMPAT_MMAP_BASES
		/* Base adresses for compatible mmap() */
		unsigned long mmap_compat_base;
		unsigned long mmap_compat_legacy_base;
#endif
		unsigned long task_size;	/* size of task vm space */
		unsigned long highest_vm_end;	/* highest vma end address */
		/**
		 * pgd：指向进程的PGD（一级页表）
		 * 
		 * 当CPU第一次访问虚拟地址空间时会触发缺页异常。
		 * 在缺页异常处理中，分配物理页面，利用分配的物理页面来创建页表项并且填充页表，完成虚拟地址到物理地址的映射关系的建立。
		 * 
		 * 参考: 001.UNIX-DOCS/000.内存管理/000.arm64-内核中的页表.md
		 * --> 每个进程的pgd都不一样
		 */
		pgd_t * pgd;

#ifdef CONFIG_MEMBARRIER
		/**
		 * @membarrier_state: Flags controlling membarrier behavior.
		 *
		 * This field is close to @pgd to hopefully fit in the same
		 * cache-line, which needs to be touched by switch_mm().
		 */
		atomic_t membarrier_state;
#endif

		/**
		 * @mm_users: The number of users including userspace.
		 *
		 * Use mmget()/mmget_not_zero()/mmput() to modify. When this
		 * drops to 0 (i.e. when the task exits and there are no other
		 * temporary reference holders), we also release a reference on
		 * @mm_count (which may then free the &struct mm_struct if
		 * @mm_count also drops to 0).
		 * mm_users：记录正在使用该进程地址空间的进程数目，如果两个线程共享该地址空间，那么mm_users的值等于2。
		 */
		atomic_t mm_users;

		/**
		 * @mm_count: The number of references to &struct mm_struct
		 * (@mm_users count as 1).
		 *
		 * Use mmgrab()/mmdrop() to modify. When this drops to 0, the
		 * &struct mm_struct is freed.
		 * mm_count：mm_struct结构体的主引用计数。
		 */
		atomic_t mm_count;

		/**
		 * @has_pinned: Whether this mm has pinned any pages.  This can
		 * be either replaced in the future by @pinned_vm when it
		 * becomes stable, or grow into a counter on its own. We're
		 * aggresive on this bit now - even if the pinned pages were
		 * unpinned later on, we'll still keep this bit set for the
		 * lifecycle of this mm just for simplicity.
		 */
		atomic_t has_pinned;

#ifdef CONFIG_MMU
		atomic_long_t pgtables_bytes;	/* PTE page table pages */
#endif
		int map_count;			/* number of VMAs */

		spinlock_t page_table_lock; /* Protects page tables and some
					     * counters
					     */
		struct rw_semaphore mmap_lock;
        
		/**
		 * mmlist：所有的mm_struct数据结构都连接到一个双向链表中，该链表的头是init_mm内存描述符，它是init进程的地址空间。
		 */
		struct list_head mmlist; /* List of maybe swapped mm's.	These
					  * are globally strung together off
					  * init_mm.mmlist, and are protected
					  * by mmlist_lock
					  */


		unsigned long hiwater_rss; /* High-watermark of RSS usage */
		unsigned long hiwater_vm;  /* High-water virtual memory usage */

		unsigned long total_vm;	   /* Total pages mapped : total_vm：已经使用的进程地址空间总和。*/
		unsigned long locked_vm;   /* Pages that have PG_mlocked set */
		atomic64_t    pinned_vm;   /* Refcount permanently increased */
		unsigned long data_vm;	   /* VM_WRITE & ~VM_SHARED & ~VM_STACK */
		unsigned long exec_vm;	   /* VM_EXEC & ~VM_WRITE & ~VM_STACK */
		unsigned long stack_vm;	   /* VM_STACK */
		unsigned long def_flags;

		spinlock_t arg_lock; /* protect the below fields */
		/**
		 * start_code, end_code：代码段的起始地址和结束地址。
		 * start_data, end_data：数据段的起始地址和结束地址。
		 */
		unsigned long start_code, end_code, start_data, end_data;
		/**
		 * start_brk：堆空间的起始地址
		 * brk：表示当前堆中的VMA的结束地址
		 */
		unsigned long start_brk, brk, start_stack;
		unsigned long arg_start, arg_end, env_start, env_end;

		unsigned long saved_auxv[AT_VECTOR_SIZE]; /* for /proc/PID/auxv */

		/*
		 * Special counters, in some configurations protected by the
		 * page_table_lock, in other configurations by being atomic.
		 */
		struct mm_rss_stat rss_stat;

		struct linux_binfmt *binfmt;

		/** Architecture-specific MM context */
		mm_context_t context;

		unsigned long flags; /* Must use atomic bitops to access */

		struct core_state *core_state; /* coredumping support */

#ifdef CONFIG_AIO
		spinlock_t			ioctx_lock;
		struct kioctx_table __rcu	*ioctx_table;
#endif
#ifdef CONFIG_MEMCG
		/*
		 * "owner" points to a task that is regarded as the canonical
		 * user/owner of this mm. All of the following must be true in
		 * order for it to be changed:
		 *
		 * current == mm->owner
		 * current->mm != mm
		 * new_owner->mm == mm
		 * new_owner->alloc_lock is held
		 */
		struct task_struct __rcu *owner;
#endif
		struct user_namespace *user_ns;

		/* store ref to file /proc/<pid>/exe symlink points to */
		struct file __rcu *exe_file;
#ifdef CONFIG_MMU_NOTIFIER
		struct mmu_notifier_subscriptions *notifier_subscriptions;
#endif
#if defined(CONFIG_TRANSPARENT_HUGEPAGE) && !USE_SPLIT_PMD_PTLOCKS
		pgtable_t pmd_huge_pte; /* protected by page_table_lock */
#endif
#ifdef CONFIG_NUMA_BALANCING
		/*
		 * numa_next_scan is the next time that the PTEs will be marked
		 * pte_numa. NUMA hinting faults will gather statistics and
		 * migrate pages to new nodes if necessary.
		 */
		unsigned long numa_next_scan;

		/* Restart point for scanning and setting pte_numa */
		unsigned long numa_scan_offset;

		/* numa_scan_seq prevents two threads setting pte_numa */
		int numa_scan_seq;
#endif
		/*
		 * An operation with batched TLB flushing is going on. Anything
		 * that can move process memory needs to flush the TLB when
		 * moving a PROT_NONE or PROT_NUMA mapped page.
		 */
		atomic_t tlb_flush_pending;
#ifdef CONFIG_ARCH_WANT_BATCHED_UNMAP_TLB_FLUSH
		/* See flush_tlb_batched_pending() */
		bool tlb_flush_batched;
#endif
		struct uprobes_state uprobes_state;
#ifdef CONFIG_HUGETLB_PAGE
		atomic_long_t hugetlb_usage;
#endif
		struct work_struct async_put_work;
	} __randomize_layout;

	/*
	 * The mm_cpumask needs to be at the end of mm_struct, because it
	 * is dynamically sized based on nr_cpu_ids.
	 */
	unsigned long cpu_bitmap[];
};

extern struct mm_struct init_mm;

/* Pointer magic because the dynamic array size confuses some compilers. */
static inline void mm_init_cpumask(struct mm_struct *mm)
{
	unsigned long cpu_bitmap = (unsigned long)mm;

	cpu_bitmap += offsetof(struct mm_struct, cpu_bitmap);
	cpumask_clear((struct cpumask *)cpu_bitmap);
}

/* Future-safe accessor for struct mm_struct's cpu_vm_mask. */
static inline cpumask_t *mm_cpumask(struct mm_struct *mm)
{
	return (struct cpumask *)&mm->cpu_bitmap;
}

struct mmu_gather;
extern void tlb_gather_mmu(struct mmu_gather *tlb, struct mm_struct *mm,
				unsigned long start, unsigned long end);
extern void tlb_finish_mmu(struct mmu_gather *tlb,
				unsigned long start, unsigned long end);

static inline void init_tlb_flush_pending(struct mm_struct *mm)
{
	atomic_set(&mm->tlb_flush_pending, 0);
}

static inline void inc_tlb_flush_pending(struct mm_struct *mm)
{
	atomic_inc(&mm->tlb_flush_pending);
	/*
	 * The only time this value is relevant is when there are indeed pages
	 * to flush. And we'll only flush pages after changing them, which
	 * requires the PTL.
	 *
	 * So the ordering here is:
	 *
	 *	atomic_inc(&mm->tlb_flush_pending);
	 *	spin_lock(&ptl);
	 *	...
	 *	set_pte_at();
	 *	spin_unlock(&ptl);
	 *
	 *				spin_lock(&ptl)
	 *				mm_tlb_flush_pending();
	 *				....
	 *				spin_unlock(&ptl);
	 *
	 *	flush_tlb_range();
	 *	atomic_dec(&mm->tlb_flush_pending);
	 *
	 * Where the increment if constrained by the PTL unlock, it thus
	 * ensures that the increment is visible if the PTE modification is
	 * visible. After all, if there is no PTE modification, nobody cares
	 * about TLB flushes either.
	 *
	 * This very much relies on users (mm_tlb_flush_pending() and
	 * mm_tlb_flush_nested()) only caring about _specific_ PTEs (and
	 * therefore specific PTLs), because with SPLIT_PTE_PTLOCKS and RCpc
	 * locks (PPC) the unlock of one doesn't order against the lock of
	 * another PTL.
	 *
	 * The decrement is ordered by the flush_tlb_range(), such that
	 * mm_tlb_flush_pending() will not return false unless all flushes have
	 * completed.
	 */
}

static inline void dec_tlb_flush_pending(struct mm_struct *mm)
{
	/*
	 * See inc_tlb_flush_pending().
	 *
	 * This cannot be smp_mb__before_atomic() because smp_mb() simply does
	 * not order against TLB invalidate completion, which is what we need.
	 *
	 * Therefore we must rely on tlb_flush_*() to guarantee order.
	 */
	atomic_dec(&mm->tlb_flush_pending);
}

static inline bool mm_tlb_flush_pending(struct mm_struct *mm)
{
	/*
	 * Must be called after having acquired the PTL; orders against that
	 * PTLs release and therefore ensures that if we observe the modified
	 * PTE we must also observe the increment from inc_tlb_flush_pending().
	 *
	 * That is, it only guarantees to return true if there is a flush
	 * pending for _this_ PTL.
	 */
	return atomic_read(&mm->tlb_flush_pending);
}

static inline bool mm_tlb_flush_nested(struct mm_struct *mm)
{
	/*
	 * Similar to mm_tlb_flush_pending(), we must have acquired the PTL
	 * for which there is a TLB flush pending in order to guarantee
	 * we've seen both that PTE modification and the increment.
	 *
	 * (no requirement on actually still holding the PTL, that is irrelevant)
	 */
	return atomic_read(&mm->tlb_flush_pending) > 1;
}

struct vm_fault;

/**
 * typedef vm_fault_t - Return type for page fault handlers.
 *
 * Page fault handlers return a bitmask of %VM_FAULT values.
 */
typedef __bitwise unsigned int vm_fault_t;

/**
 * enum vm_fault_reason - Page fault handlers return a bitmask of
 * these values to tell the core VM what happened when handling the
 * fault. Used to decide whether a process gets delivered SIGBUS or
 * just gets major/minor fault counters bumped up.
 *
 * @VM_FAULT_OOM:		Out Of Memory
 * @VM_FAULT_SIGBUS:		Bad access
 * @VM_FAULT_MAJOR:		Page read from storage
 * @VM_FAULT_WRITE:		Special case for get_user_pages
 * @VM_FAULT_HWPOISON:		Hit poisoned small page
 * @VM_FAULT_HWPOISON_LARGE:	Hit poisoned large page. Index encoded
 *				in upper bits
 * @VM_FAULT_SIGSEGV:		segmentation fault
 * @VM_FAULT_NOPAGE:		->fault installed the pte, not return page
 * @VM_FAULT_LOCKED:		->fault locked the returned page
 * @VM_FAULT_RETRY:		->fault blocked, must retry
 * @VM_FAULT_FALLBACK:		huge page fault failed, fall back to small
 * @VM_FAULT_DONE_COW:		->fault has fully handled COW
 * @VM_FAULT_NEEDDSYNC:		->fault did not modify page tables and needs
 *				fsync() to complete (for synchronous page faults
 *				in DAX)
 * @VM_FAULT_HINDEX_MASK:	mask HINDEX value
 *
 */
enum vm_fault_reason {
	VM_FAULT_OOM            = (__force vm_fault_t)0x000001,
	VM_FAULT_SIGBUS         = (__force vm_fault_t)0x000002,
	VM_FAULT_MAJOR          = (__force vm_fault_t)0x000004,
	VM_FAULT_WRITE          = (__force vm_fault_t)0x000008,
	VM_FAULT_HWPOISON       = (__force vm_fault_t)0x000010,
	VM_FAULT_HWPOISON_LARGE = (__force vm_fault_t)0x000020,
	VM_FAULT_SIGSEGV        = (__force vm_fault_t)0x000040,
	VM_FAULT_NOPAGE         = (__force vm_fault_t)0x000100,
	VM_FAULT_LOCKED         = (__force vm_fault_t)0x000200,
	VM_FAULT_RETRY          = (__force vm_fault_t)0x000400,
	VM_FAULT_FALLBACK       = (__force vm_fault_t)0x000800,
	VM_FAULT_DONE_COW       = (__force vm_fault_t)0x001000,
	VM_FAULT_NEEDDSYNC      = (__force vm_fault_t)0x002000,
	VM_FAULT_HINDEX_MASK    = (__force vm_fault_t)0x0f0000,
};

/* Encode hstate index for a hwpoisoned large page */
#define VM_FAULT_SET_HINDEX(x) ((__force vm_fault_t)((x) << 16))
#define VM_FAULT_GET_HINDEX(x) (((__force unsigned int)(x) >> 16) & 0xf)

#define VM_FAULT_ERROR (VM_FAULT_OOM | VM_FAULT_SIGBUS |	\
			VM_FAULT_SIGSEGV | VM_FAULT_HWPOISON |	\
			VM_FAULT_HWPOISON_LARGE | VM_FAULT_FALLBACK)

#define VM_FAULT_RESULT_TRACE \
	{ VM_FAULT_OOM,                 "OOM" },	\
	{ VM_FAULT_SIGBUS,              "SIGBUS" },	\
	{ VM_FAULT_MAJOR,               "MAJOR" },	\
	{ VM_FAULT_WRITE,               "WRITE" },	\
	{ VM_FAULT_HWPOISON,            "HWPOISON" },	\
	{ VM_FAULT_HWPOISON_LARGE,      "HWPOISON_LARGE" },	\
	{ VM_FAULT_SIGSEGV,             "SIGSEGV" },	\
	{ VM_FAULT_NOPAGE,              "NOPAGE" },	\
	{ VM_FAULT_LOCKED,              "LOCKED" },	\
	{ VM_FAULT_RETRY,               "RETRY" },	\
	{ VM_FAULT_FALLBACK,            "FALLBACK" },	\
	{ VM_FAULT_DONE_COW,            "DONE_COW" },	\
	{ VM_FAULT_NEEDDSYNC,           "NEEDDSYNC" }

struct vm_special_mapping {
	const char *name;	/* The name, e.g. "[vdso]". */

	/*
	 * If .fault is not provided, this points to a
	 * NULL-terminated array of pages that back the special mapping.
	 *
	 * This must not be NULL unless .fault is provided.
	 */
	struct page **pages;

	/*
	 * If non-NULL, then this is called to resolve page faults
	 * on the special mapping.  If used, .pages is not checked.
	 */
	vm_fault_t (*fault)(const struct vm_special_mapping *sm,
				struct vm_area_struct *vma,
				struct vm_fault *vmf);

	int (*mremap)(const struct vm_special_mapping *sm,
		     struct vm_area_struct *new_vma);
};

enum tlb_flush_reason {
	TLB_FLUSH_ON_TASK_SWITCH,
	TLB_REMOTE_SHOOTDOWN,
	TLB_LOCAL_SHOOTDOWN,
	TLB_LOCAL_MM_SHOOTDOWN,
	TLB_REMOTE_SEND_IPI,
	NR_TLB_FLUSH_REASONS,
};

 /*
  * A swap entry has to fit into a "unsigned long", as the entry is hidden
  * in the "index" field of the swapper address space.
  */
typedef struct {
	unsigned long val;
} swp_entry_t;

#endif /* _LINUX_MM_TYPES_H */
