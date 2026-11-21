==================================
Scheduler debugfs(调度器 debugfs)
==================================

Booting a kernel with debugfs enabled will give access to
scheduler specific debug files under /sys/kernel/debug/sched. Some of
those files are described below.
(以启用 debugfs 的方式启动内核后，即可访问 /sys/kernel/debug/sched 下的
调度器专用调试文件。下面对其中一些文件进行说明。)

numa_balancing
==============

`numa_balancing` directory is used to hold files to control NUMA
balancing feature.  If the system overhead from the feature is too
high then the rate the kernel samples for NUMA hinting faults may be
controlled by the `scan_period_min_ms, scan_delay_ms,
scan_period_max_ms, scan_size_mb` files.
(`numa_balancing` 目录用于存放控制 NUMA 平衡功能的文件。如果该功能带来的
系统开销过高，那么内核对 NUMA 提示缺页的采样速率可以通过
`scan_period_min_ms, scan_delay_ms,
scan_period_max_ms, scan_size_mb` 文件来控制。)


scan_period_min_ms, scan_delay_ms, scan_period_max_ms, scan_size_mb
-------------------------------------------------------------------

Automatic NUMA balancing scans tasks address space and unmaps pages to
detect if pages are properly placed or if the data should be migrated to a
memory node local to where the task is running.  Every "scan delay" the task
scans the next "scan size" number of pages in its address space. When the
end of the address space is reached the scanner restarts from the beginning.
(自动 NUMA 平衡会扫描任务地址空间并解除页面映射，以检测页面放置是否恰当，
或数据是否应迁移到任务运行位置本地的内存节点。每隔一个"扫描延迟"，任务会
扫描其地址空间中接下来的"扫描大小"个页面。到达地址空间末尾后，扫描器从头
重新开始。)

In combination, the "scan delay" and "scan size" determine the scan rate.
When "scan delay" decreases, the scan rate increases.  The scan delay and
hence the scan rate of every task is adaptive and depends on historical
behaviour. If pages are properly placed then the scan delay increases,
otherwise the scan delay decreases.  The "scan size" is not adaptive but
the higher the "scan size", the higher the scan rate.
("扫描延迟"与"扫描大小"共同决定扫描速率。当"扫描延迟"减小时，扫描速率
增加。每个任务的扫描延迟以及扫描速率都是自适应的，取决于历史行为。如果
页面放置恰当，扫描延迟会增加；否则扫描延迟会减小。"扫描大小"不是自适应
的，但"扫描大小"越大，扫描速率越高。)

Higher scan rates incur higher system overhead as page faults must be
trapped and potentially data must be migrated. However, the higher the scan
rate, the more quickly a tasks memory is migrated to a local node if the
workload pattern changes and minimises performance impact due to remote
memory accesses. These files control the thresholds for scan delays and
the number of pages scanned.
(更高的扫描速率会带来更高的系统开销，因为必须陷入缺页异常，并且可能需要
迁移数据。不过，扫描速率越高，当负载模式变化时，任务内存迁移到本地节点的
速度就越快，从而把远程内存访问造成的性能影响降到最低。这些文件控制扫描
延迟的阈值以及扫描的页面数量。)

``scan_period_min_ms`` is the minimum time in milliseconds to scan a
tasks virtual memory. It effectively controls the maximum scanning
rate for each task.
(``scan_period_min_ms`` 是扫描一个任务虚拟内存的最小时间 (毫秒)。它实际上
控制每个任务的最大扫描速率。)

``scan_delay_ms`` is the starting "scan delay" used for a task when it
initially forks.
(``scan_delay_ms`` 是任务最初 fork 时使用的起始"扫描延迟"。)

``scan_period_max_ms`` is the maximum time in milliseconds to scan a
tasks virtual memory. It effectively controls the minimum scanning
rate for each task.
(``scan_period_max_ms`` 是扫描一个任务虚拟内存的最大时间 (毫秒)。它实际上
控制每个任务的最小扫描速率。)

``scan_size_mb`` is how many megabytes worth of pages are scanned for
a given scan.
(``scan_size_mb`` 是一次扫描中扫描的页面相当于多少兆字节。)
