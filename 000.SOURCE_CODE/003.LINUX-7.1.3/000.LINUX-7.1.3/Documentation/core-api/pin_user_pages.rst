.. SPDX-License-Identifier: GPL-2.0

===============================================================
pin_user_pages() and related calls(pin_user_pages() 及相关调用)
===============================================================

.. contents:: :local:

Overview(概述)
==============

This document describes the following functions::

 pin_user_pages()
 pin_user_pages_fast()
 pin_user_pages_remote()

(本文档描述以下函数：)

Basic description of FOLL_PIN(FOLL_PIN 的基本描述)
==================================================

FOLL_PIN and FOLL_LONGTERM are flags that can be passed to the get_user_pages*()
("gup") family of functions. FOLL_PIN has significant interactions and
interdependencies with FOLL_LONGTERM, so both are covered here.
(FOLL_PIN 与 FOLL_LONGTERM 是可以传递给 get_user_pages*() ("gup") 函数族
的标志。FOLL_PIN 与 FOLL_LONGTERM 之间有重要的交互与相互依赖关系，所以
这里把两者都涵盖。)

FOLL_PIN is internal to gup, meaning that it should not appear at the gup call
sites. This allows the associated wrapper functions  (pin_user_pages*() and
others) to set the correct combination of these flags, and to check for problems
as well.
(FOLL_PIN 是 gup 内部使用的，这意味着它不应出现在 gup 调用点。这使得相关
的包装函数 (pin_user_pages*() 等) 能够设置这些标志的正确组合，同时也能
检查问题。)

FOLL_LONGTERM, on the other hand, *is* allowed to be set at the gup call sites.
This is in order to avoid creating a large number of wrapper functions to cover
all combinations of get*(), pin*(), FOLL_LONGTERM, and more. Also, the
pin_user_pages*() APIs are clearly distinct from the get_user_pages*() APIs, so
that's a natural dividing line, and a good point to make separate wrapper calls.
In other words, use pin_user_pages*() for DMA-pinned pages, and
get_user_pages*() for other cases. There are five cases described later on in
this document, to further clarify that concept.
(另一方面，FOLL_LONGTERM *是* 允许在 gup 调用点设置的。这是为了避免为
覆盖 get*()、pin*()、FOLL_LONGTERM 等的所有组合而创建大量包装函数。此外，
pin_user_pages*() API 与 get_user_pages*() API 明显不同，这是一个自然的
分界线，也是分开做包装调用的好时机。换句话说，对被 DMA 固定的页使用
pin_user_pages*()，其他情况使用 get_user_pages*()。本文档稍后描述了五种
情况，以进一步澄清这一概念。)

FOLL_PIN and FOLL_GET are mutually exclusive for a given gup call. However,
multiple threads and call sites are free to pin the same struct pages, via both
FOLL_PIN and FOLL_GET. It's just the call site that needs to choose one or the
other, not the struct page(s).
(对于给定的某次 gup 调用，FOLL_PIN 与 FOLL_GET 互斥。但是，多个线程和
调用点可以自由地同时通过 FOLL_PIN 和 FOLL_GET 固定相同的 struct page。
需要二选一的只是调用点，而不是 struct page 本身。)

The FOLL_PIN implementation is nearly the same as FOLL_GET, except that FOLL_PIN
uses a different reference counting technique.
(FOLL_PIN 的实现与 FOLL_GET 几乎相同，只是 FOLL_PIN 使用了不同的引用
计数技术。)

FOLL_PIN is a prerequisite to FOLL_LONGTERM. Another way of saying that is,
FOLL_LONGTERM is a specific case, more restrictive case of FOLL_PIN.
(FOLL_PIN 是 FOLL_LONGTERM 的前提条件。换一种说法：FOLL_LONGTERM 是
FOLL_PIN 的一个更特殊的、限制更严格的用例。)

Which flags are set by each wrapper(每个 wrapper 设置哪些标志)
==============================================================

For these pin_user_pages*() functions, FOLL_PIN is OR'd in with whatever gup
flags the caller provides. The caller is required to pass in a non-null struct
pages* array, and the function then pins pages by incrementing each by a special
value: GUP_PIN_COUNTING_BIAS.
(对于这些 pin_user_pages*() 函数，FOLL_PIN 会与调用者提供的任何 gup
标志做按位或。调用者必须传入一个非空的 struct pages* 数组，然后函数通过
把每页递增一个特殊值来固定页：GUP_PIN_COUNTING_BIAS。)

For large folios, the GUP_PIN_COUNTING_BIAS scheme is not used. Instead,
the extra space available in the struct folio is used to store the
pincount directly.
(对于大 folio，不使用 GUP_PIN_COUNTING_BIAS 方案。而是利用 struct
folio 中可用的额外空间直接存储 pincount。)

This approach for large folios avoids the counting upper limit problems
that are discussed below. Those limitations would have been aggravated
severely by huge pages, because each tail page adds a refcount to the
head page. And in fact, testing revealed that, without a separate pincount
field, refcount overflows were seen in some huge page stress tests.
(这种针对大 folio 的做法避免了下文讨论的计数上限问题。否则，这些限制会
因巨页而严重恶化，因为每个尾页 (tail page) 都会给头页增加一个引用计数。
事实上，测试表明，在没有独立 pincount 字段的情况下，某些巨页压力测试中
出现了引用计数溢出。)

This also means that huge pages and large folios do not suffer
from the false positives problem that is mentioned below.::

 Function
 --------
 pin_user_pages          FOLL_PIN is always set internally by this function.
 pin_user_pages_fast     FOLL_PIN is always set internally by this function.
 pin_user_pages_remote   FOLL_PIN is always set internally by this function.

(这也意味着巨页和大 folio 不会遭受下文提到的误报问题。)

For these get_user_pages*() functions, FOLL_GET might not even be specified.
Behavior is a little more complex than above. If FOLL_GET was *not* specified,
but the caller passed in a non-null struct pages* array, then the function
sets FOLL_GET for you, and proceeds to pin pages by incrementing the refcount
of each page by +1.::

 Function
 --------
 get_user_pages           FOLL_GET is sometimes set internally by this function.
 get_user_pages_fast      FOLL_GET is sometimes set internally by this function.
 get_user_pages_remote    FOLL_GET is sometimes set internally by this function.

(对于这些 get_user_pages*() 函数，甚至可能根本没有指定 FOLL_GET。行为
比上面稍微复杂一点。如果 *没有* 指定 FOLL_GET，但调用者传入了非空的
struct pages* 数组，那么函数会替你设置 FOLL_GET，并通过把每页的引用计数
+1 来固定页。)

Tracking dma-pinned pages(跟踪 dma-pinned 页)
=============================================

Some of the key design constraints, and solutions, for tracking dma-pinned
pages:
(以下是跟踪 dma-pinned 页的一些关键设计约束与相应的解决方案：)

* An actual reference count, per struct page, is required. This is because
  multiple processes may pin and unpin a page.
  (每个 struct page 需要一个真正的引用计数。这是因为多个进程可能固定并
  解除固定同一个页。)

* False positives (reporting that a page is dma-pinned, when in fact it is not)
  are acceptable, but false negatives are not.
  (误报 (报告某页是 dma-pinned 的，而实际上并不是) 是可以接受的，但漏报
  不行。)

* struct page may not be increased in size for this, and all fields are already
  used.
  (struct page 的大小不能为此而增加，而且它的所有字段都已被占用。)

* Given the above, we can overload the page->_refcount field by using, sort of,
  the upper bits in that field for a dma-pinned count. "Sort of", means that,
  rather than dividing page->_refcount into bit fields, we simple add a medium-
  large value (GUP_PIN_COUNTING_BIAS, initially chosen to be 1024: 10 bits) to
  page->_refcount. This provides fuzzy behavior: if a page has get_page() called
  on it 1024 times, then it will appear to have a single dma-pinned count.
  And again, that's acceptable.
  (鉴于上述情况，我们可以复用 page->_refcount 字段，把该字段中的高位用作
  dma-pinned 计数。所谓 "复用"，是指不是把 page->_refcount 划分为位字段，
  而是简单地给它加上一个中等偏大的值 (GUP_PIN_COUNTING_BIAS，初始选择为
  1024，即 10 位)。这提供了模糊的行为：如果某页被调用了 1024 次
  get_page()，那么它看起来就像有一个 dma-pinned 计数。同样，这是可以
  接受的。)

This also leads to limitations: there are only 31-10==21 bits available for a
counter that increments 10 bits at a time.
(这也导致了限制：一个每次递增 10 位的计数器，只有 31-10==21 位可用。)

* Because of that limitation, special handling is applied to the zero pages
  when using FOLL_PIN.  We only pretend to pin a zero page - we don't alter its
  refcount or pincount at all (it is permanent, so there's no need).  The
  unpinning functions also don't do anything to a zero page.  This is
  transparent to the caller.
  (由于这一限制，使用 FOLL_PIN 时对零页 (zero page) 应用了特殊处理。我们
  只是假装固定了零页 —— 我们完全不改变其 refcount 或 pincount (它是永久
  的，所以没有必要)。解除固定的函数对零页也不做任何事情。这对调用者是
  透明的。)

* Callers must specifically request "dma-pinned tracking of pages". In other
  words, just calling get_user_pages() will not suffice; a new set of functions,
  pin_user_page() and related, must be used.
  (调用者必须显式请求 "对页进行 dma-pinned 跟踪"。换句话说，仅仅调用
  get_user_pages() 是不够的；必须使用新的函数集 pin_user_page() 及相关
  函数。)

FOLL_PIN, FOLL_GET, FOLL_LONGTERM: when to use which flags(FOLL_PIN、FOLL_GET、FOLL_LONGTERM：何时使用哪个标志)
================================================================================================================

Thanks to Jan Kara, Vlastimil Babka and several other -mm people, for describing
these categories:
(感谢 Jan Kara、Vlastimil Babka 以及其他几位 -mm 人士描述了这些类别：)

CASE 1: Direct IO (DIO)(情况 1：直接 IO (DIO))
----------------------------------------------
There are GUP references to pages that are serving
as DIO buffers. These buffers are needed for a relatively short time (so they
are not "long term"). No special synchronization with folio_mkclean() or
munmap() is provided. Therefore, flags to set at the call site are: ::

    FOLL_PIN

(存在指向充当 DIO 缓冲区的页的 GUP 引用。这些缓冲区所需的时间相对较短
(所以它们不是 "长期" 的)。不提供与 folio_mkclean() 或 munmap() 的特殊
同步。因此，调用点要设置的标志是：)

...but rather than setting FOLL_PIN directly, call sites should use one of
the pin_user_pages*() routines that set FOLL_PIN.
(……但与其直接设置 FOLL_PIN，调用点应当使用会设置 FOLL_PIN 的
pin_user_pages*() 例程之一。)

CASE 2: RDMA(情况 2：RDMA)
--------------------------
There are GUP references to pages that are serving as DMA
buffers. These buffers are needed for a long time ("long term"). No special
synchronization with folio_mkclean() or munmap() is provided. Therefore, flags
to set at the call site are: ::

    FOLL_PIN | FOLL_LONGTERM

(存在指向充当 DMA 缓冲区的页的 GUP 引用。这些缓冲区需要长期存在
("长期")。不提供与 folio_mkclean() 或 munmap() 的特殊同步。因此，调用点
要设置的标志是：)

NOTE: Some pages, such as DAX pages, cannot be pinned with longterm pins. That's
because DAX pages do not have a separate page cache, and so "pinning" implies
locking down file system blocks, which is not (yet) supported in that way.
(注意：某些页 (例如 DAX 页) 不能用长期固定来固定。这是因为 DAX 页没有
单独的页缓存，所以 "固定" 意味着锁定文件系统块，这种方式目前尚不
支持。)

.. _mmu-notifier-registration-case:

CASE 3: MMU notifier registration, with or without page faulting hardware(情况 3：MMU 通知器注册，带或不带页错误硬件)
---------------------------------------------------------------------------------------------------------------------
Device drivers can pin pages via get_user_pages*(), and register for mmu
notifier callbacks for the memory range. Then, upon receiving a notifier
"invalidate range" callback , stop the device from using the range, and unpin
the pages. There may be other possible schemes, such as for example explicitly
synchronizing against pending IO, that accomplish approximately the same thing.
(设备驱动可以通过 get_user_pages*() 固定页，并为该内存范围注册 mmu
notifier 回调。然后，在收到通知器的 "invalidate range" 回调时，停止设备
使用该范围，并解除对这些页的固定。也可能有其他可行的方案，例如与挂起的
IO 显式同步，从而实现大致相同的效果。)

Or, if the hardware supports replayable page faults, then the device driver can
avoid pinning entirely (this is ideal), as follows: register for mmu notifier
callbacks as above, but instead of stopping the device and unpinning in the
callback, simply remove the range from the device's page tables.
(或者，如果硬件支持可重放的页错误 (page fault)，那么设备驱动可以完全
避免固定页 (这是理想情况)，方法如下：像上面那样注册 mmu notifier 回调，
但在回调中不是停止设备并解除固定，而是简单地从设备的页表中移除该范围。)

Either way, as long as the driver unpins the pages upon mmu notifier callback,
then there is proper synchronization with both filesystem and mm
(folio_mkclean(), munmap(), etc). Therefore, neither flag needs to be set.
(无论哪种方式，只要驱动在 mmu notifier 回调时解除对页的固定，就与文件
系统和 mm (folio_mkclean()、munmap() 等) 有了正确的同步。因此，两个标志
都不需要设置。)

CASE 4: Pinning for struct page manipulation only(情况 4：仅为操作 struct page 而固定)
--------------------------------------------------------------------------------------
If only struct page data (as opposed to the actual memory contents that a page
is tracking) is affected, then normal GUP calls are sufficient, and neither flag
needs to be set.
(如果只影响 struct page 数据 (而不是页所跟踪的实际内存内容)，那么普通
的 GUP 调用就足够了，两个标志都不需要设置。)

CASE 5: Pinning in order to write to the data within the page(情况 5：为写入页内数据而固定)
-------------------------------------------------------------------------------------------
Even though neither DMA nor Direct IO is involved, just a simple case of "pin,
write to a page's data, unpin" can cause a problem. Case 5 may be considered a
superset of Case 1, plus Case 2, plus anything that invokes that pattern. In
other words, if the code is neither Case 1 nor Case 2, it may still require
FOLL_PIN, for patterns like this:
(即使既不涉及 DMA 也不涉及 Direct IO，仅仅是 "固定、写入页的数据、解除
固定" 这种简单情形也可能引发问题。情况 5 可以视为情况 1、情况 2 以及
任何触发该模式的情形的超集。换句话说，如果代码既不是情况 1 也不是情况
2，它仍然可能需要 FOLL_PIN，用于如下的模式：)

Correct (uses FOLL_PIN calls):(正确 (使用 FOLL_PIN 调用)：)
    pin_user_pages()
    write to the data within the pages
    unpin_user_pages()

INCORRECT (uses FOLL_GET calls):(不正确 (使用 FOLL_GET 调用)：)
    get_user_pages()
    write to the data within the pages
    put_page()

folio_maybe_dma_pinned(): the whole point of pinning(folio_maybe_dma_pinned()：固定的全部意义)
==============================================================================================

The whole point of marking folios as "DMA-pinned" or "gup-pinned" is to be able
to query, "is this folio DMA-pinned?" That allows code such as folio_mkclean()
(and file system writeback code in general) to make informed decisions about
what to do when a folio cannot be unmapped due to such pins.
(把 folio 标记为 "DMA-pinned" 或 "gup-pinned" 的全部意义在于能够查询：
"这个 folio 是否被 DMA 固定？" 这使得 folio_mkclean() 之类的代码 (以及
一般的文件系统回写代码) 能够在 folio 因这类固定而无法取消映射时，做出
明智的决定。)

What to do in those cases is the subject of a years-long series of discussions
and debates (see the References at the end of this document). It's a TODO item
here: fill in the details once that's worked out. Meanwhile, it's safe to say
that having this available: ::

        static inline bool folio_maybe_dma_pinned(struct folio *folio)

(在这些情况下该怎么做，是长达数年的讨论与辩论的主题 (参见本文档末尾的
参考资料)。这里是一项 TODO：等讨论出结果后再补充细节。同时，可以肯定地
说，拥有下面这个可用的：)

...is a prerequisite to solving the long-running gup+DMA problem.
(……是解决长期悬而未决的 gup+DMA 问题的前提条件。)

Another way of thinking about FOLL_GET, FOLL_PIN, and FOLL_LONGTERM(理解 FOLL_GET、FOLL_PIN 与 FOLL_LONGTERM 的另一种方式)
==========================================================================================================================

Another way of thinking about these flags is as a progression of restrictions:
FOLL_GET is for struct page manipulation, without affecting the data that the
struct page refers to. FOLL_PIN is a *replacement* for FOLL_GET, and is for
short term pins on pages whose data *will* get accessed. As such, FOLL_PIN is
a "more severe" form of pinning. And finally, FOLL_LONGTERM is an even more
restrictive case that has FOLL_PIN as a prerequisite: this is for pages that
will be pinned longterm, and whose data will be accessed.
(思考这些标志的另一种方式是把它们看作限制的递进：FOLL_GET 用于 struct
page 操作，不影响 struct page 所指的数据。FOLL_PIN 是 FOLL_GET 的
*替代品*，用于其数据 *将* 被访问的页的短期固定。因此，FOLL_PIN 是一种
"更严重" 的固定形式。最后，FOLL_LONGTERM 是一个更加受限的用例，以
FOLL_PIN 为前提：它用于将被长期固定、且其数据将被访问的页。)

Unit testing(单元测试)
======================
This file::

 tools/testing/selftests/mm/gup_test.c

(这个文件：)

has the following new calls to exercise the new pin*() wrapper functions:
(包含以下新调用，用于演练新的 pin*() 包装函数：)

* PIN_FAST_BENCHMARK (./gup_test -a)
* PIN_BASIC_TEST (./gup_test -b)

You can monitor how many total dma-pinned pages have been acquired and released
since the system was booted, via two new /proc/vmstat entries: ::

    /proc/vmstat/nr_foll_pin_acquired
    /proc/vmstat/nr_foll_pin_released

(你可以通过两个新的 /proc/vmstat 条目，监控系统启动以来总共获取并释放了
多少 dma-pinned 页：)

Under normal conditions, these two values will be equal unless there are any
long-term [R]DMA pins in place, or during pin/unpin transitions.
(在正常情况下，这两个值应当相等，除非存在长期 [R]DMA 固定，或者正处于
固定/解除固定的转换过程中。)

* nr_foll_pin_acquired: This is the number of logical pins that have been
  acquired since the system was powered on. For huge pages, the head page is
  pinned once for each page (head page and each tail page) within the huge page.
  This follows the same sort of behavior that get_user_pages() uses for huge
  pages: the head page is refcounted once for each tail or head page in the huge
  page, when get_user_pages() is applied to a huge page.
  (nr_foll_pin_acquired：这是系统上电以来已获取的逻辑固定数量。对于巨页，
  巨页内的每一页 (头页和每个尾页) 都会对头页固定一次。这与
  get_user_pages() 对巨页的做法类似：当 get_user_pages() 应用于巨页时，
  头页会为巨页中的每个尾页或头页被引用计数一次。)

* nr_foll_pin_released: The number of logical pins that have been released since
  the system was powered on. Note that pages are released (unpinned) on a
  PAGE_SIZE granularity, even if the original pin was applied to a huge page.
  Becaused of the pin count behavior described above in "nr_foll_pin_acquired",
  the accounting balances out, so that after doing this::

    pin_user_pages(huge_page);
    for (each page in huge_page)
        unpin_user_page(page);

  (nr_foll_pin_released：系统上电以来已释放的逻辑固定数量。注意，页按
  PAGE_SIZE 粒度释放 (解除固定)，即使最初的固定是应用于巨页的。由于上文
  "nr_foll_pin_acquired" 中描述的固定计数行为，账目会最终持平，所以在
  执行下面操作之后：)

...the following is expected::

    nr_foll_pin_released == nr_foll_pin_acquired

(……预期会得到如下结果：)

(...unless it was already out of balance due to a long-term RDMA pin being in
place.)
(……除非由于存在长期 RDMA 固定而早已失衡。)

Other diagnostics(其他诊断)
===========================

dump_page() has been enhanced slightly to handle these new counting
fields, and to better report on large folios in general.  Specifically,
for large folios, the exact pincount is reported.
(dump_page() 已被稍微增强，以处理这些新的计数字段，并更好地报告大
folio。具体来说，对于大 folio，会报告精确的 pincount。)

References(参考资料)
====================

* `Some slow progress on get_user_pages() (Apr 2, 2019) <https://lwn.net/Articles/784574/>`_ (关于 get_user_pages() 的一些缓慢进展 (2019 年 4 月 2 日))
* `DMA and get_user_pages() (LPC: Dec 12, 2018) <https://lwn.net/Articles/774411/>`_ (DMA 与 get_user_pages() (LPC：2018 年 12 月 12 日))
* `The trouble with get_user_pages() (Apr 30, 2018) <https://lwn.net/Articles/753027/>`_ (get_user_pages() 的麻烦 (2018 年 4 月 30 日))
* `LWN kernel index: get_user_pages() <https://lwn.net/Kernel/Index/#Memory_management-get_user_pages>`_ (LWN 内核索引：get_user_pages())

John Hubbard, October, 2019
(John Hubbard，2019 年 10 月)
