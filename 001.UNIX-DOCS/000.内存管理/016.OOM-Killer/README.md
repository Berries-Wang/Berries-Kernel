# OOM-Killer

## 参考代码
```c
// 000.SOURCE_CODE/000.LINUX-5.9/000.LINUX-5.9/mm/page_alloc.c

static inline struct page *
__alloc_pages_slowpath(gfp_t gfp_mask, unsigned int order,
						struct alloc_context *ac)
{
   // ...

   	/** 
	 * Reclaim has failed us, start killing things
	 * 内存回收失败，开始终止进程
	 * OOM Killer？ YES ， OOM Killer代码就在里面
	 *  */
	page = __alloc_pages_may_oom(gfp_mask, order, ac, &did_some_progress);

   // ...
}
```