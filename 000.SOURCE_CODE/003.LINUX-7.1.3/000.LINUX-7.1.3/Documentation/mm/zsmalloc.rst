=========================
zsmalloc(zsmalloc 分配器)
=========================

This allocator is designed for use with zram. Thus, the allocator is
supposed to work well under low memory conditions. In particular, it
never attempts higher order page allocation which is very likely to
fail under memory pressure. On the other hand, if we just use single
(0-order) pages, it would suffer from very high fragmentation --
any object of size PAGE_SIZE/2 or larger would occupy an entire page.
This was one of the major issues with its predecessor (xvmalloc).
(这种分配器专为 zram 设计。因此，它应当在低内存条件下也能良好工作。特别是，
它从不尝试高阶页分配，因为那在内存压力下很容易失败。另一方面，如果只使用单个
(0 阶) 页，又会因严重的碎片化而受损——任何大小达到 PAGE_SIZE/2 或更大的对象
都将占据一整页。这是其前身 (xvmalloc) 的主要问题之一。)

To overcome these issues, zsmalloc allocates a bunch of 0-order pages
and links them together using various 'struct page' fields. These linked
pages act as a single higher-order page i.e. an object can span 0-order
page boundaries. The code refers to these linked pages as a single entity
called zspage.
(为克服这些问题，zsmalloc 分配一批 0 阶页，并使用各种 'struct page' 字段把它们
链接在一起。这些链接起来的页表现为一个单独的高阶页，也就是说，对象可以跨越
0 阶页的边界。代码把这些链接起来的页当作一个单独的实体，称为 zspage。)

For simplicity, zsmalloc can only allocate objects of size up to PAGE_SIZE
since this satisfies the requirements of all its current users (in the
worst case, page is incompressible and is thus stored "as-is" i.e. in
uncompressed form). For allocation requests larger than this size, failure
is returned (see zs_malloc).
(为简单起见，zsmalloc 只能分配大小不超过 PAGE_SIZE 的对象，因为这已经满足其
现有全部使用者的需求(最坏情况下，页不可压缩，于是按"原样"即未压缩形式存储)。
对于超过该大小的分配请求，将返回失败(见 zs_malloc)。)

Additionally, zs_malloc() does not return a dereferenceable pointer.
Instead, it returns an opaque handle (unsigned long) which encodes actual
location of the allocated object. The reason for this indirection is that
zsmalloc does not keep zspages permanently mapped since that would cause
issues on 32-bit systems where the VA region for kernel space mappings
is very small. So, using the allocated memory should be done through the
proper handle-based APIs.
(此外，zs_malloc() 并不返回可解引用的指针，而是返回一个不透明的句柄
(unsigned long)，其中编码了所分配对象的实际位置。之所以采用这种间接方式，
是因为 zsmalloc 不会永久映射 zspage——否则在 32 位系统上会出问题，因为那些
系统的内核空间映射 VA 区域非常小。所以，使用分配到的内存应当通过恰当的基于
句柄的 API 完成。)

stat(统计信息)
==============

With CONFIG_ZSMALLOC_STAT, we could see zsmalloc internal information via
``/sys/kernel/debug/zsmalloc/<user name>``. Here is a sample of stat output::

 # cat /sys/kernel/debug/zsmalloc/zram0/classes

 class  size       10%       20%       30%       40%       50%       60%       70%       80%       90%       99%      100% obj_allocated   obj_used pages_used pages_per_zspage freeable
    ...
    ...
    30   512         0        12         4         1         0         1         0         0         1         0       414          3464       3346        433                1       14
    31   528         2         7         2         2         1         0         1         0         0         2       117          4154       3793        536                4       44
    32   544         6         3         4         1         2         1         0         0         0         1       260          4170       3965        556                2       26
    ...
    ...

(启用 CONFIG_ZSMALLOC_STAT 后，我们可以通过
``/sys/kernel/debug/zsmalloc/<user name>`` 查看 zsmalloc 的内部信息。上面是
一份统计输出的示例。)


class
(类)

	index
	(zspage 类的索引。)
size
(大小)

	object size zspage stores
	(zspage 存储的对象大小。)
10%
	the number of zspages with usage ratio less than 10% (see below)
	(使用率低于 10% 的 zspage 数量(见下文)。)
20%
	the number of zspages with usage ratio between 10% and 20%
	(使用率在 10% 到 20% 之间的 zspage 数量。)
30%
	the number of zspages with usage ratio between 20% and 30%
	(使用率在 20% 到 30% 之间的 zspage 数量。)
40%
	the number of zspages with usage ratio between 30% and 40%
	(使用率在 30% 到 40% 之间的 zspage 数量。)
50%
	the number of zspages with usage ratio between 40% and 50%
	(使用率在 40% 到 50% 之间的 zspage 数量。)
60%
	the number of zspages with usage ratio between 50% and 60%
	(使用率在 50% 到 60% 之间的 zspage 数量。)
70%
	the number of zspages with usage ratio between 60% and 70%
	(使用率在 60% 到 70% 之间的 zspage 数量。)
80%
	the number of zspages with usage ratio between 70% and 80%
	(使用率在 70% 到 80% 之间的 zspage 数量。)
90%
	the number of zspages with usage ratio between 80% and 90%
	(使用率在 80% 到 90% 之间的 zspage 数量。)
99%
	the number of zspages with usage ratio between 90% and 99%
	(使用率在 90% 到 99% 之间的 zspage 数量。)
100%
	the number of zspages with usage ratio 100%
	(使用率为 100% 的 zspage 数量。)
obj_allocated
(已分配对象数)

	the number of objects allocated
	(已分配的对象数量。)
obj_used
(使用中对象数)

	the number of objects allocated to the user
	(分配给用户的对象数量。)
pages_used
(已用页数)

	the number of pages allocated for the class
	(为该类分配的页数。)
pages_per_zspage
(每 zspage 页数)

	the number of 0-order pages to make a zspage
	(构成一个 zspage 所需的 0-order 页数。)
freeable
(可释放页数)

	the approximate number of pages class compaction can free
	(类压缩 (class compaction) 大致能够释放的页数。)

Each zspage maintains inuse counter which keeps track of the number of
objects stored in the zspage.  The inuse counter determines the zspage's
"fullness group" which is calculated as the ratio of the "inuse" objects to
the total number of objects the zspage can hold (objs_per_zspage). The
closer the inuse counter is to objs_per_zspage, the better.
(每个 zspage 都维护一个 inuse 计数器，用于跟踪存储在该 zspage 中的对象数量。
inuse 计数器决定 zspage 的 "fullness group"(填充度分组)，其取值为 "inuse"
对象数与该 zspage 可容纳对象总数 (objs_per_zspage) 之比。inuse 计数器越接近
objs_per_zspage 越好。)

Internals(内部实现)
===================

zsmalloc has 255 size classes, each of which can hold a number of zspages.
Each zspage can contain up to ZSMALLOC_CHAIN_SIZE physical (0-order) pages.
The optimal zspage chain size for each size class is calculated during the
creation of the zsmalloc pool (see calculate_zspage_chain_size()).
(zsmalloc 有 255 个大小类 (size class)，每个类可以容纳若干 zspage。每个
zspage 最多可包含 ZSMALLOC_CHAIN_SIZE 个物理 (0-order) 页。每个大小类的最优
zspage 链长度是在创建 zsmalloc 池时计算的(见
calculate_zspage_chain_size())。)

As an optimization, zsmalloc merges size classes that have similar
characteristics in terms of the number of pages per zspage and the number
of objects that each zspage can store.
(作为一项优化，zsmalloc 会合并那些在"每 zspage 页数"与"每个 zspage 可存储
对象数"方面特征相近的大小类。)

For instance, consider the following size classes:::

  class  size       10%   ....    100% obj_allocated   obj_used pages_used pages_per_zspage freeable
  ...
     94  1536        0    ....       0             0          0          0                3        0
    100  1632        0    ....       0             0          0          0                2        0
  ...

(例如，考虑下面这些大小类：)


Size classes #95-99 are merged with size class #100. This means that when we
need to store an object of size, say, 1568 bytes, we end up using size class
#100 instead of size class #96. Size class #100 is meant for objects of size
1632 bytes, so each object of size 1568 bytes wastes 1632-1568=64 bytes.
(大小类 #95-99 与大小类 #100 合并。这意味着当我们需要存储一个大小为 1568 字节
的对象时，最终使用的是大小类 #100 而不是大小类 #96。大小类 #100 面向 1632
字节的对象，所以每个 1568 字节的对象会浪费 1632-1568=64 字节。)

Size class #100 consists of zspages with 2 physical pages each, which can
hold a total of 5 objects. If we need to store 13 objects of size 1568, we
end up allocating three zspages, or 6 physical pages.
(大小类 #100 由每个含 2 个物理页的 zspage 组成，总共可容纳 5 个对象。如果我们
需要存储 13 个大小为 1568 的对象，最终要分配三个 zspage，即 6 个物理页。)

However, if we take a closer look at size class #96 (which is meant for
objects of size 1568 bytes) and trace `calculate_zspage_chain_size()`, we
find that the most optimal zspage configuration for this class is a chain
of 5 physical pages:::

    pages per zspage      wasted bytes     used%
           1                  960           76
           2                  352           95
           3                 1312           89
           4                  704           95
           5                   96           99

(然而，如果我们仔细观察大小类 #96(面向 1568 字节对象)并跟踪
`calculate_zspage_chain_size()`，就会发现该类最优的 zspage 配置是一条由 5 个
物理页组成的链：)

This means that a class #96 configuration with 5 physical pages can store 13
objects of size 1568 in a single zspage, using a total of 5 physical pages.
This is more efficient than the class #100 configuration, which would use 6
physical pages to store the same number of objects.
(这意味着把 #96 类配置为 5 个物理页时，可以在单个 zspage 中存储 13 个 1568
字节的对象，总共只用 5 个物理页。这比 #100 类的配置更高效，后者需要 6 个物理页
才能存储同样数量的对象。)

As the zspage chain size for class #96 increases, its key characteristics
such as pages per-zspage and objects per-zspage also change. This leads to
dewer class mergers, resulting in a more compact grouping of classes, which
reduces memory wastage.
(随着 #96 类的 zspage 链长度增大，其"每 zspage 页数"、"每 zspage 对象数"等关键
特性也会随之变化。这使类合并的情况变少，类的分组更加紧凑，从而减少内存浪费。)

Let's take a closer look at the bottom of `/sys/kernel/debug/zsmalloc/zramX/classes`:::

  class  size       10%   ....    100% obj_allocated   obj_used pages_used pages_per_zspage freeable

  ...
    202  3264         0   ..         0             0          0          0                4        0
    254  4096         0   ..         0             0          0          0                1        0
  ...

(让我们仔细看看 `/sys/kernel/debug/zsmalloc/zramX/classes` 的底部：)

Size class #202 stores objects of size 3264 bytes and has a maximum of 4 pages
per zspage. Any object larger than 3264 bytes is considered huge and belongs
to size class #254, which stores each object in its own physical page (objects
in huge classes do not share pages).
(大小类 #202 存储 3264 字节的对象，每个 zspage 最多 4 页。任何大于 3264 字节
的对象都被视为"巨大"(huge)，归入大小类 #254，该类把每个对象存储在独立的物理页
中(巨大类中的对象不共享页)。)

Increasing the size of the chain of zspages also results in a higher watermark
for the huge size class and fewer huge classes overall. This allows for more
efficient storage of large objects.
(增大 zspage 链的长度还会提高巨大类的水位线，并减少巨大类的总量。这使得大对象
的存储更加高效。)

For zspage chain size of 8, huge class watermark becomes 3632 bytes:::

  class  size       10%   ....    100% obj_allocated   obj_used pages_used pages_per_zspage freeable

  ...
    202  3264         0   ..         0             0          0          0                4        0
    211  3408         0   ..         0             0          0          0                5        0
    217  3504         0   ..         0             0          0          0                6        0
    222  3584         0   ..         0             0          0          0                7        0
    225  3632         0   ..         0             0          0          0                8        0
    254  4096         0   ..         0             0          0          0                1        0
  ...

(当 zspage 链长度为 8 时，巨大类水位线变为 3632 字节：)

For zspage chain size of 16, huge class watermark becomes 3840 bytes:::

  class  size       10%   ....    100% obj_allocated   obj_used pages_used pages_per_zspage freeable

  ...
    202  3264         0   ..         0             0          0          0                4        0
    206  3328         0   ..         0             0          0          0               13        0
    207  3344         0   ..         0             0          0          0                9        0
    208  3360         0   ..         0             0          0          0               14        0
    211  3408         0   ..         0             0          0          0                5        0
    212  3424         0   ..         0             0          0          0               16        0
    214  3456         0   ..         0             0          0          0               11        0
    217  3504         0   ..         0             0          0          0                6        0
    219  3536         0   ..         0             0          0          0               13        0
    222  3584         0   ..         0             0          0          0                7        0
    223  3600         0   ..         0             0          0          0               15        0
    225  3632         0   ..         0             0          0          0                8        0
    228  3680         0   ..         0             0          0          0                9        0
    230  3712         0   ..         0             0          0          0               10        0
    232  3744         0   ..         0             0          0          0               11        0
    234  3776         0   ..         0             0          0          0               12        0
    235  3792         0   ..         0             0          0          0               13        0
    236  3808         0   ..         0             0          0          0               14        0
    238  3840         0   ..         0             0          0          0               15        0
    254  4096         0   ..         0             0          0          0                1        0
  ...

(当 zspage 链长度为 16 时，巨大类水位线变为 3840 字节：)

Overall the combined zspage chain size effect on zsmalloc pool configuration:::

  pages per zspage   number of size classes (clusters)   huge size class watermark
         4                        69                               3264
         5                        86                               3408
         6                        93                               3504
         7                       112                               3584
         8                       123                               3632
         9                       140                               3680
        10                       143                               3712
        11                       159                               3744
        12                       164                               3776
        13                       180                               3792
        14                       183                               3808
        15                       188                               3840
        16                       191                               3840

(总体来看，zspage 链长度对 zsmalloc 池配置的综合影响如下：)


A synthetic test(一次合成测试)
------------------------------

zram as a build artifacts storage (Linux kernel compilation).
(zram 用作构建产物的存储(Linux 内核编译)。)

* `CONFIG_ZSMALLOC_CHAIN_SIZE=4`

  zsmalloc classes stats:::

    class  size       10%   ....    100% obj_allocated   obj_used pages_used pages_per_zspage freeable

    ...
    Total              13   ..        51        413836     412973     159955                         3

  (CONFIG_ZSMALLOC_CHAIN_SIZE=4 时的 zsmalloc 类统计：)

  zram mm_stat:::

   1691783168 628083717 655175680        0 655175680       60        0    34048    34049

  (以及对应的 zram mm_stat：)


* `CONFIG_ZSMALLOC_CHAIN_SIZE=8`

  zsmalloc classes stats:::

    class  size       10%   ....    100% obj_allocated   obj_used pages_used pages_per_zspage freeable

    ...
    Total              18   ..        87        414852     412978     156666                         0

  (CONFIG_ZSMALLOC_CHAIN_SIZE=8 时的 zsmalloc 类统计：)

  zram mm_stat:::

    1691803648 627793930 641703936        0 641703936       60        0    33591    33591

  (以及对应的 zram mm_stat：)

Using larger zspage chains may result in using fewer physical pages, as seen
in the example where the number of physical pages used decreased from 159955
to 156666, at the same time maximum zsmalloc pool memory usage went down from
655175680 to 641703936 bytes.
(使用更大的 zspage 链可能减少物理页的用量，正如示例中所见：物理页使用量从
159955 降至 156666，同时 zsmalloc 池的最大内存占用也从 655175680 字节降至
641703936 字节。)

However, this advantage may be offset by the potential for increased system
memory pressure (as some zspages have larger chain sizes) in cases where there
is heavy internal fragmentation and zspool compaction is unable to relocate
objects and release zspages. In these cases, it is recommended to decrease
the limit on the size of the zspage chains (as specified by the
CONFIG_ZSMALLOC_CHAIN_SIZE option).
(然而，在内部碎片严重、zspool 压缩无法迁移对象并释放 zspage 的场景中，这一优势
可能被系统内存压力的增加所抵消(因为部分 zspage 的链更长)。在这些情况下，建议
降低 zspage 链长度的上限(即 CONFIG_ZSMALLOC_CHAIN_SIZE 选项)。)

Functions(函数)
===============

.. kernel-doc:: mm/zsmalloc.c
