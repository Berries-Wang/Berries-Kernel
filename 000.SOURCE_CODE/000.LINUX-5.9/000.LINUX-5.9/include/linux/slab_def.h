/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SLAB_DEF_H
#define	_LINUX_SLAB_DEF_H

#include <linux/reciprocal_div.h>

/**
 * Definitions unique to the original Linux SLAB allocator.（最初Linux SLAB分配器特有的定义）
 *
 * slab 描述符
 */
struct kmem_cache {
	struct array_cache __percpu *cpu_cache; // array_cache数据结构，每个CPU都有一个，表示本地对象缓冲池

/* 1) Cache tunables. Protected by slab_mutex */
	unsigned int batchcount;
	unsigned int limit;
	unsigned int shared;

	unsigned int size;
	struct reciprocal_value reciprocal_buffer_size;
/* 2) touched by every alloc & free from the backend */

	slab_flags_t flags;		/* constant flags */
	unsigned int num;		/* # of objs per slab */

/* 3) cache_grow/shrink */
	/* order of pgs per slab (2^n) */
	unsigned int gfporder;

	/* force GFP flags, e.g. GFP_DMA */
	gfp_t allocflags;

	size_t colour;			/* cache colouring range (一个slab分配器中有多少个不同的高速缓存行，用于着色) */
	unsigned int colour_off;	/* colour offset (一个着色区的长度，和L1高速缓存行大小相同)*/
	struct kmem_cache *freelist_cache; /*用于OFF_SLAB模式的slab分配器，使用额外的内存来保存slab管理区域 */
	/**
	 * 每个对象在freelist管理区中占用1字节，这里指freelist管理区的大小
	 */
	unsigned int freelist_size;

	/**
	 * constructor func
	 * (什么的构造函数)
	 *  */
	void (*ctor)(void *obj);

/* 4) cache creation/removal */
	const char *name;   // slab描述符的名称
	struct list_head list; // 链表节点，用于把slab描述符添加到全局链表slab_caches中
	/**
	 * 用于表示这个slab描述符的引用计数。当创建其他slab描述符并需要引用该描述符时会增加引用计数
	 */
	int refcount;
	/**
	 * 对象的实际大小
	 */
	int object_size;
	/**
	 * 对齐的长度
	 */
	int align;

/* 5) statistics */
#ifdef CONFIG_DEBUG_SLAB
	unsigned long num_active;
	unsigned long num_allocations;
	unsigned long high_mark;
	unsigned long grown;
	unsigned long reaped;
	unsigned long errors;
	unsigned long max_freeable;
	unsigned long node_allocs;
	unsigned long node_frees;
	unsigned long node_overflow;
	atomic_t allochit;
	atomic_t allocmiss;
	atomic_t freehit;
	atomic_t freemiss;

	/*
	 * If debugging is enabled, then the allocator can add additional
	 * fields and/or padding to every object. 'size' contains the total
	 * object size including these internal fields, while 'obj_offset'
	 * and 'object_size' contain the offset to the user object and its
	 * size.
	 */
	int obj_offset;
#endif /* CONFIG_DEBUG_SLAB */

#ifdef CONFIG_KASAN
	struct kasan_cache kasan_info;
#endif

#ifdef CONFIG_SLAB_FREELIST_RANDOM
	unsigned int *random_seq;
#endif
    /**
	 * Usercopy区域的偏移量
	 */
	unsigned int useroffset;	/* Usercopy region offset */
	/**
	 * Usercopy区域的大小
	 */
	unsigned int usersize;		/* Usercopy region size */
    
	/**
	 * slab节点，在NUMA系统中每个节点有一个kmem_cache_node数据结构
	 */
	struct kmem_cache_node *node[MAX_NUMNODES];
};

static inline void *nearest_obj(struct kmem_cache *cache, struct page *page,
				void *x)
{
	void *object = x - (x - page->s_mem) % cache->size;
	void *last_object = page->s_mem + (cache->num - 1) * cache->size;

	if (unlikely(object > last_object))
		return last_object;
	else
		return object;
}

/*
 * We want to avoid an expensive divide : (offset / cache->size)
 *   Using the fact that size is a constant for a particular cache,
 *   we can replace (offset / cache->size) by
 *   reciprocal_divide(offset, cache->reciprocal_buffer_size)
 */
static inline unsigned int obj_to_index(const struct kmem_cache *cache,
					const struct page *page, void *obj)
{
	u32 offset = (obj - page->s_mem);
	return reciprocal_divide(offset, cache->reciprocal_buffer_size);
}

static inline int objs_per_slab_page(const struct kmem_cache *cache,
				     const struct page *page)
{
	return cache->num;
}

#endif	/* _LINUX_SLAB_DEF_H */
