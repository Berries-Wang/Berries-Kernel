.. SPDX-License-Identifier: GPL-2.0

=============
NFSD IO MODES
=============

(NFSD IO 模式)

Overview
(概述)
========

NFSD has historically always used buffered IO when servicing READ and
WRITE operations. BUFFERED is NFSD's default IO mode, but it is possible
to override that default to use either DONTCACHE or DIRECT IO modes.
(NFSD 在历史上处理 READ 和 WRITE 操作时一直使用缓冲 IO。BUFFERED 是 NFSD 的默认 IO 模式，但可以覆盖该默认值，改用 DONTCACHE 或 DIRECT IO 模式。)

Experimental NFSD debugfs interfaces are available to allow the NFSD IO
mode used for READ and WRITE to be configured independently. See both:
(有一些实验性的 NFSD debugfs 接口，允许将 READ 和 WRITE 所使用的 NFSD IO 模式独立地加以配置。请同时参阅以下两者：)

- /sys/kernel/debug/nfsd/io_cache_read
- /sys/kernel/debug/nfsd/io_cache_write

The default value for both io_cache_read and io_cache_write reflects
NFSD's default IO mode (which is NFSD_IO_BUFFERED=0).
(io_cache_read 和 io_cache_write 的默认值都反映了 NFSD 的默认 IO 模式(即 NFSD_IO_BUFFERED=0)。)

Based on the configured settings, NFSD's IO will either be:
(根据所配置的设置，NFSD 的 IO 将是以下几种情况之一：)

- cached using page cache (NFSD_IO_BUFFERED=0)
  (使用页缓存进行缓存(NFSD_IO_BUFFERED=0)。)
- cached but removed from page cache on completion (NFSD_IO_DONTCACHE=1)
  (进行缓存，但在完成时从页缓存中移除(NFSD_IO_DONTCACHE=1)。)
- not cached stable_how=NFS_UNSTABLE (NFSD_IO_DIRECT=2)
  (不缓存，stable_how=NFS_UNSTABLE(NFSD_IO_DIRECT=2)。)

To set an NFSD IO mode, write a supported value (0 - 2) to the
corresponding IO operation's debugfs interface, e.g.::
(要设置 NFSD IO 模式，请向相应 IO 操作的 debugfs 接口写入一个受支持的值(0 - 2)，例如：)

  echo 2 > /sys/kernel/debug/nfsd/io_cache_read
  echo 2 > /sys/kernel/debug/nfsd/io_cache_write

To check which IO mode NFSD is using for READ or WRITE, simply read the
corresponding IO operation's debugfs interface, e.g.::
(要检查 NFSD 对 READ 或 WRITE 使用的是哪种 IO 模式，只需读取相应 IO 操作的 debugfs 接口，例如：)

  cat /sys/kernel/debug/nfsd/io_cache_read
  cat /sys/kernel/debug/nfsd/io_cache_write

If you experiment with NFSD's IO modes on a recent kernel and have
interesting results, please report them to linux-nfs@vger.kernel.org
(如果你在较新的内核上试验 NFSD 的 IO 模式并得到了有趣的结果，请将它们报告给 linux-nfs@vger.kernel.org。)

NFSD DONTCACHE
(NFSD DONTCACHE(不缓存))
==============

DONTCACHE offers a hybrid approach to servicing IO that aims to offer
the benefits of using DIRECT IO without any of the strict alignment
requirements that DIRECT IO imposes. To achieve this buffered IO is used
but the IO is flagged to "drop behind" (meaning associated pages are
dropped from the page cache) when IO completes.
(DONTCACHE 提供了一种处理 IO 的混合方式，旨在提供 DIRECT IO 的好处，同时不受 DIRECT IO 施加的任何严格对齐要求。为此，它使用缓冲 IO，但在 IO 完成时将该 IO 标记为 "drop behind"(意思是把相关页面从页缓存中丢弃)。)

DONTCACHE aims to avoid what has proven to be a fairly significant
limition of Linux's memory management subsystem if/when large amounts of
data is infrequently accessed (e.g. read once _or_ written once but not
read until much later). Such use-cases are particularly problematic
because the page cache will eventually become a bottleneck to servicing
new IO requests.
(DONTCACHE 旨在避免 Linux 内存管理子系统一个已被证明相当显著的局限：当大量数据很少被访问时(例如只读一次 _或_ 只写一次、很久之后才被读取)。这类用例的问题尤其突出，因为页缓存最终会成为处理新 IO 请求的瓶颈。)

For more context on DONTCACHE, please see these Linux commit headers:
(有关 DONTCACHE 的更多背景信息，请参阅以下 Linux 提交说明：)

- Overview:  9ad6344568cc3 ("mm/filemap: change filemap_create_folio()
  to take a struct kiocb")
  (概述：9ad6344568cc3 ("mm/filemap: change filemap_create_folio() to take a struct kiocb")。)
- for READ:  8026e49bff9b1 ("mm/filemap: add read support for
  RWF_DONTCACHE")
  (关于 READ：8026e49bff9b1 ("mm/filemap: add read support for RWF_DONTCACHE")。)
- for WRITE: 974c5e6139db3 ("xfs: flag as supporting FOP_DONTCACHE")
  (关于 WRITE：974c5e6139db3 ("xfs: flag as supporting FOP_DONTCACHE")。)

NFSD_IO_DONTCACHE will fall back to NFSD_IO_BUFFERED if the underlying
filesystem doesn't indicate support by setting FOP_DONTCACHE.
(如果底层文件系统没有通过设置 FOP_DONTCACHE 来表明支持，NFSD_IO_DONTCACHE 将回退到 NFSD_IO_BUFFERED。)

NFSD DIRECT
(NFSD DIRECT(直接 IO))
===========

DIRECT IO doesn't make use of the page cache, as such it is able to
avoid the Linux memory management's page reclaim scalability problems
without resorting to the hybrid use of page cache that DONTCACHE does.
(DIRECT IO 不使用页缓存，因此它能够避免 Linux 内存管理在页回收方面的可伸缩性问题，而无需像 DONTCACHE 那样混合使用页缓存。)

Some workloads benefit from NFSD avoiding the page cache, particularly
those with a working set that is significantly larger than available
system memory. The pathological worst-case workload that NFSD DIRECT has
proven to help most is: NFS client issuing large sequential IO to a file
that is 2-3 times larger than the NFS server's available system memory.
The reason for such improvement is NFSD DIRECT eliminates a lot of work
that the memory management subsystem would otherwise be required to
perform (e.g. page allocation, dirty writeback, page reclaim). When
using NFSD DIRECT, kswapd and kcompactd are no longer commanding CPU
time trying to find adequate free pages so that forward IO progress can
be made.
(某些工作负载得益于 NFSD 避开页缓存，尤其是那些工作集明显大于可用系统内存的工作负载。事实证明，NFSD DIRECT 帮助最大的最坏情况工作负载是：NFS 客户端对一个大小为 NFS 服务器可用系统内存 2-3 倍的文件发起大量顺序 IO。带来这种改进的原因是，NFSD DIRECT 消除了内存管理子系统原本需要执行的许多工作(例如页分配、脏页回写、页回收)。使用 NFSD DIRECT 时，kswapd 和 kcompactd 不再占用 CPU 时间去寻找足够的空闲页以使 IO 得以继续推进。)

The performance win associated with using NFSD DIRECT was previously
discussed on linux-nfs, see:
https://lore.kernel.org/linux-nfs/aEslwqa9iMeZjjlV@kernel.org/
(与使用 NFSD DIRECT 相关的性能收益此前曾在 linux-nfs 邮件列表上讨论过，参见：https://lore.kernel.org/linux-nfs/aEslwqa9iMeZjjlV@kernel.org/)

But in summary:
(但概括而言：)

- NFSD DIRECT can significantly reduce memory requirements
  (NFSD DIRECT 可以显著降低内存需求。)
- NFSD DIRECT can reduce CPU load by avoiding costly page reclaim work
  (NFSD DIRECT 可以通过避免代价高昂的页回收工作来降低 CPU 负载。)
- NFSD DIRECT can offer more deterministic IO performance
  (NFSD DIRECT 可以提供更具确定性的 IO 性能。)

As always, your mileage may vary and so it is important to carefully
consider if/when it is beneficial to make use of NFSD DIRECT. When
assessing comparative performance of your workload please be sure to log
relevant performance metrics during testing (e.g. memory usage, cpu
usage, IO performance). Using perf to collect perf data that may be used
to generate a "flamegraph" for work Linux must perform on behalf of your
test is a really meaningful way to compare the relative health of the
system and how switching NFSD's IO mode changes what is observed.
(一如既往，实际效果因人而异，因此仔细考虑在何时以及是否有益地使用 NFSD DIRECT 是很重要的。在评估工作负载的相对性能时，请务必在测试期间记录相关的性能指标(例如内存使用、CPU 使用、IO 性能)。使用 perf 收集 perf 数据(可用于为 Linux 必须代表你的测试所执行的工作生成"火焰图")是一种非常有意义的方式，可以用来比较系统的相对健康状况，以及切换 NFSD 的 IO 模式如何改变所观察到的情况。)

If NFSD_IO_DIRECT is specified by writing 2 (or 3 and 4 for WRITE) to
NFSD's debugfs interfaces, ideally the IO will be aligned relative to
the underlying block device's logical_block_size. Also the memory buffer
used to store the READ or WRITE payload must be aligned relative to the
underlying block device's dma_alignment.
(如果通过向 NFSD 的 debugfs 接口写入 2(WRITE 则为 3 和 4)来指定 NFSD_IO_DIRECT，理想情况下 IO 应相对底层块设备的 logical_block_size 对齐。此外，用于存储 READ 或 WRITE 载荷的内存缓冲区必须相对底层块设备的 dma_alignment 对齐。)

But NFSD DIRECT does handle misaligned IO in terms of O_DIRECT as best
it can:
(但 NFSD DIRECT 确实会尽其所能按 O_DIRECT 的方式处理非对齐 IO：)

Misaligned READ:
    If NFSD_IO_DIRECT is used, expand any misaligned READ to the next
    DIO-aligned block (on either end of the READ). The expanded READ is
    verified to have proper offset/len (logical_block_size) and
    dma_alignment checking.
    (非对齐 READ：如果使用 NFSD_IO_DIRECT，则将任何非对齐的 READ 扩展到下一个 DIO 对齐的块(在 READ 的任一端)。扩展后的 READ 会经过适当的 offset/len(logical_block_size)校验和 dma_alignment 检查。)

Misaligned WRITE:
    If NFSD_IO_DIRECT is used, split any misaligned WRITE into a start,
    middle and end as needed. The large middle segment is DIO-aligned
    and the start and/or end are misaligned. Buffered IO is used for the
    misaligned segments and O_DIRECT is used for the middle DIO-aligned
    segment. DONTCACHE buffered IO is _not_ used for the misaligned
    segments because using normal buffered IO offers significant RMW
    performance benefit when handling streaming misaligned WRITEs.
    (非对齐 WRITE：如果使用 NFSD_IO_DIRECT，则按需将任何非对齐的 WRITE 拆分为起始、中间和末尾三段。较大的中间段是 DIO 对齐的，而起始和/或末尾是非对齐的。非对齐段使用缓冲 IO，中间 DIO 对齐段使用 O_DIRECT。非对齐段 _不_ 使用 DONTCACHE 缓冲 IO，因为在处理流式非对齐 WRITE 时，普通缓冲 IO 能带来显著的 RMW 性能收益。)

Tracing:
    The nfsd_read_direct trace event shows how NFSD expands any
    misaligned READ to the next DIO-aligned block (on either end of the
    original READ, as needed).
    (跟踪(Tracing)：nfsd_read_direct 跟踪事件显示 NFSD 如何将任何非对齐的 READ 扩展到下一个 DIO 对齐的块(按需在原始 READ 的任一端进行)。)

    This combination of trace events is useful for READs::
    (以下这组跟踪事件对 READ 很有用：)

      echo 1 > /sys/kernel/tracing/events/nfsd/nfsd_read_vector/enable
      echo 1 > /sys/kernel/tracing/events/nfsd/nfsd_read_direct/enable
      echo 1 > /sys/kernel/tracing/events/nfsd/nfsd_read_io_done/enable
      echo 1 > /sys/kernel/tracing/events/xfs/xfs_file_direct_read/enable

    The nfsd_write_direct trace event shows how NFSD splits a given
    misaligned WRITE into a DIO-aligned middle segment.
    (nfsd_write_direct 跟踪事件显示 NFSD 如何将给定的非对齐 WRITE 拆分为一个 DIO 对齐的中间段。)

    This combination of trace events is useful for WRITEs::
    (以下这组跟踪事件对 WRITE 很有用：)

      echo 1 > /sys/kernel/tracing/events/nfsd/nfsd_write_opened/enable
      echo 1 > /sys/kernel/tracing/events/nfsd/nfsd_write_direct/enable
      echo 1 > /sys/kernel/tracing/events/nfsd/nfsd_write_io_done/enable
      echo 1 > /sys/kernel/tracing/events/xfs/xfs_file_direct_write/enable
