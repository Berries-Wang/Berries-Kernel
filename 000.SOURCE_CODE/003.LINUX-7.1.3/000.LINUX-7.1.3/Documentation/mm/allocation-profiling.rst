.. SPDX-License-Identifier: GPL-2.0

=========================================
MEMORY ALLOCATION PROFILING(内存分配统计)
=========================================

Low overhead (suitable for production) accounting of all memory allocations,
tracked by file and line number.
(所有内存分配的低开销（适合生产环境）统计，按文件和行号进行跟踪。)

Usage: (用法：)
kconfig options: (kconfig 选项：)

- CONFIG_MEM_ALLOC_PROFILING

- CONFIG_MEM_ALLOC_PROFILING_ENABLED_BY_DEFAULT

- CONFIG_MEM_ALLOC_PROFILING_DEBUG
  adds warnings for allocations that weren't accounted because of a
  missing annotation
  (为因缺少注记而未被统计的分配添加警告)

Boot parameter: (启动参数：)
  sysctl.vm.mem_profiling={0|1|never}[,compressed]

  When set to "never", memory allocation profiling overhead is minimized and it
  cannot be enabled at runtime (sysctl becomes read-only).
  When CONFIG_MEM_ALLOC_PROFILING_ENABLED_BY_DEFAULT=y, default value is "1".
  When CONFIG_MEM_ALLOC_PROFILING_ENABLED_BY_DEFAULT=n, default value is "never".
  "compressed" optional parameter will try to store page tag references in a
  compact format, avoiding page extensions. This results in improved performance
  and memory consumption, however it might fail depending on system configuration.
  If compression fails, a warning is issued and memory allocation profiling gets
  disabled.
  (设置为 "never" 时，内存分配统计的开销被降到最低，且无法在运行时启用（sysctl 变为只读）。当 CONFIG_MEM_ALLOC_PROFILING_ENABLED_BY_DEFAULT=y 时，默认值为 "1"。当 CONFIG_MEM_ALLOC_PROFILING_ENABLED_BY_DEFAULT=n 时，默认值为 "never"。"compressed" 可选参数将尝试以紧凑格式存储页标签引用，避免使用页扩展。这会改善性能和内存消耗，但可能会因系统配置而失败。如果压缩失败，会发出警告并禁用内存分配统计。)

sysctl: (sysctl：)
  /proc/sys/vm/mem_profiling

  1: Enable memory profiling.
  (1: 启用内存统计。)

  0: Disable memory profiling.
  (0: 禁用内存统计。)

  The default value depends on CONFIG_MEM_ALLOC_PROFILING_ENABLED_BY_DEFAULT.
  (默认值取决于 CONFIG_MEM_ALLOC_PROFILING_ENABLED_BY_DEFAULT。)

  When CONFIG_MEM_ALLOC_PROFILING_DEBUG=y, this control is read-only to avoid
  warnings produced by allocations made while profiling is disabled and freed
  when it's enabled.
  (当 CONFIG_MEM_ALLOC_PROFILING_DEBUG=y 时，此控件为只读，以避免统计被禁用期间所做的分配、以及统计被启用时的释放所产生的警告。)

Runtime info: (运行时信息：)
  /proc/allocinfo

Example output::

  root@moria-kvm:~# sort -g /proc/allocinfo|tail|numfmt --to=iec
        2.8M    22648 fs/kernfs/dir.c:615 func:__kernfs_new_node
        3.8M      953 mm/memory.c:4214 func:alloc_anon_folio
        4.0M     1010 drivers/staging/ctagmod/ctagmod.c:20 [ctagmod] func:ctagmod_start
        4.1M        4 net/netfilter/nf_conntrack_core.c:2567 func:nf_ct_alloc_hashtable
        6.0M     1532 mm/filemap.c:1919 func:__filemap_get_folio
        8.8M     2785 kernel/fork.c:307 func:alloc_thread_stack_node
         13M      234 block/blk-mq.c:3421 func:blk_mq_alloc_rqs
         14M     3520 mm/mm_init.c:2530 func:alloc_large_system_hash
         15M     3656 mm/readahead.c:247 func:page_cache_ra_unbounded
         55M     4887 mm/slub.c:2259 func:alloc_slab_page
        122M    31168 mm/page_ext.c:270 func:alloc_page_ext

(示例输出：)

Theory of operation(工作原理)
=============================

Memory allocation profiling builds off of code tagging, which is a library for
declaring static structs (that typically describe a file and line number in
some way, hence code tagging) and then finding and operating on them at runtime,
- i.e. iterating over them to print them in debugfs/procfs.

To add accounting for an allocation call, we replace it with a macro
invocation, alloc_hooks(), that
(要为一个分配调用添加统计，我们用一个宏调用 alloc_hooks() 替换它，该宏会：)

- declares a code tag
  (声明一个代码标记)
- stashes a pointer to it in task_struct
  (把指向它的指针存入 task_struct)
- calls the real allocation function
  (调用真正的分配函数)
- and finally, restores the task_struct alloc tag pointer to its previous value.
  (最后，把 task_struct 的 alloc 标签指针恢复为之前的值。)

This allows for alloc_hooks() calls to be nested, with the most recent one
taking effect. This is important for allocations internal to the mm/ code that
do not properly belong to the outer allocation context and should be counted
separately: for example, slab object extension vectors, or when the slab
allocates pages from the page allocator.
(这允许 alloc_hooks() 调用嵌套，由最近的一个生效。这对 mm/ 代码内部的分配很重要，这些分配并不真正属于外层的分配上下文，应单独计数：例如 slab 对象扩展向量，或者当 slab 从页分配器分配页时。)

Thus, proper usage requires determining which function in an allocation call
stack should be tagged. There are many helper functions that essentially wrap
e.g. kmalloc() and do a little more work, then are called in multiple places;
we'll generally want the accounting to happen in the callers of these helpers,
not in the helpers themselves.
(因此，正确的用法需要确定分配调用栈中应当标记哪个函数。有许多辅助函数本质上是对 kmalloc() 等的封装并做一些额外工作，然后在多处被调用；我们通常希望在这些辅助函数的调用者中进行统计，而不是在辅助函数本身中。)

To fix up a given helper, for example foo(), do the following:
(要修整某个给定的辅助函数（例如 foo()），请执行以下操作：)

- switch its allocation call to the _noprof() version, e.g. kmalloc_noprof()
  (把它的分配调用切换为 _noprof() 版本，例如 kmalloc_noprof())

- rename it to foo_noprof()
  (把它重命名为 foo_noprof())

- define a macro version of foo() like so:
  (如下定义 foo() 的宏版本：)

  #define foo(...) alloc_hooks(foo_noprof(__VA_ARGS__))

It's also possible to stash a pointer to an alloc tag in your own data structures.
(也可以把指向 alloc 标签的指针存入你自己的数据结构中。)

Do this when you're implementing a generic data structure that does allocations
"on behalf of" some other code - for example, the rhashtable code. This way,
instead of seeing a large line in /proc/allocinfo for rhashtable.c, we can
break it out by rhashtable type.
(当你实现一个"代表"其他代码进行分配的通用数据结构时——例如 rhashtable 代码——就应这样做。这样，我们不是在 /proc/allocinfo 中看到 rhashtable.c 的很大一行，而是可以按 rhashtable 类型细分。)

To do so:
(为此：)

- Hook your data structure's init function, like any other allocation function.
  (像对待任何其他分配函数一样，挂接你数据结构的 init 函数。)

- Within your init function, use the convenience macro alloc_tag_record() to
  record alloc tag in your data structure.
  (在你的 init 函数中，使用便捷宏 alloc_tag_record() 把 alloc 标签记录到你的数据结构中。)

- Then, use the following form for your allocations:
  (然后，对你的分配使用以下形式：)
  alloc_hooks_tag(ht->your_saved_tag, kmalloc_noprof(...))
