.. SPDX-License-Identifier: GPL-2.0

=============================================================================
DAMON Moniting Interval Parameters Tuning Example(DAMON 监测间隔参数调优示例)
=============================================================================

DAMON's monitoring parameters need tuning based on given workload and the
monitoring purpose.  There is a :ref:`tuning guide
<damon_design_monitoring_params_tuning_guide>` for that.  This document
provides an example tuning based on the guide.
(DAMON 的监测参数需要根据给定的工作负载和监测目的进行调优。为此有一个 :ref:`调优指南
<damon_design_monitoring_params_tuning_guide>`。本文档提供一个基于该指南的调优
示例。)

Setup(环境准备)
===============

For below example, DAMON of Linux kernel v6.11 and `damo
<https://github.com/damonitor/damo>`_ (DAMON user-space tool) v2.5.9 was used to
monitor and visualize access patterns on the physical address space of a system
running a real-world server workload.
(在下面的示例中，使用 Linux 内核 v6.11 的 DAMON 和 v2.5.9 版的 `damo
<https://github.com/damonitor/damo>`_ (DAMON 用户空间工具)，对一台运行真实
服务器工作负载的系统的物理地址空间进行访问模式监测与可视化。)

5ms/100ms intervals: Too Short Interval(5ms/100ms 间隔：间隔过短)
=================================================================

Let's start by capturing the access pattern snapshot on the physical address
space of the system using DAMON, with the default interval parameters (5
milliseconds and 100 milliseconds for the sampling and the aggregation
intervals, respectively).  Wait ten minutes between the start of DAMON and
the capturing of the snapshot, to show a meaningful time-wise access patterns.
::

    # damo start
    # sleep 600
    # damo record --snapshot 0 1
    # damo stop

(我们先使用 DAMON 以默认间隔参数(采样间隔与聚合间隔分别为 5 毫秒和 100
毫秒)捕获系统物理地址空间的访问模式快照。在 DAMON 启动与快照捕获之间等待
十分钟，以便展示有意义的随时间变化的访问模式。)

Then, list the DAMON-found regions of different access patterns, sorted by the
"access temperature".  "Access temperature" is a metric representing the
access-hotness of a region.  It is calculated as a weighted sum of the access
frequency and the age of the region.  If the access frequency is 0 %, the
temperature is multiplied by minus one.  That is, if a region is not accessed,
it gets minus temperature and it gets lower as not accessed for longer time.
The sorting is in temperature-ascendint order, so the region at the top of the
list is the coldest, and the one at the bottom is the hottest one. ::

    # damo report access --sort_regions_by temperature
    0   addr 16.052 GiB   size 5.985 GiB   access 0 %   age 5.900 s    # coldest
    1   addr 22.037 GiB   size 6.029 GiB   access 0 %   age 5.300 s
    2   addr 28.065 GiB   size 6.045 GiB   access 0 %   age 5.200 s
    3   addr 10.069 GiB   size 5.983 GiB   access 0 %   age 4.500 s
    4   addr 4.000 GiB    size 6.069 GiB   access 0 %   age 4.400 s
    5   addr 62.008 GiB   size 3.992 GiB   access 0 %   age 3.700 s
    6   addr 56.795 GiB   size 5.213 GiB   access 0 %   age 3.300 s
    7   addr 39.393 GiB   size 6.096 GiB   access 0 %   age 2.800 s
    8   addr 50.782 GiB   size 6.012 GiB   access 0 %   age 2.800 s
    9   addr 34.111 GiB   size 5.282 GiB   access 0 %   age 2.300 s
    10  addr 45.489 GiB   size 5.293 GiB   access 0 %   age 1.800 s    # hottest
    total size: 62.000 GiB

(然后，按"访问温度 (access temperature)"列出 DAMON 发现的具有不同访问模式的
区域。"访问温度"是表示一个区域访问热度高低的指标，计算方式是访问频率与该区域
年龄 (age) 的加权和。如果访问频率为 0%，温度则乘以负一。也就是说，如果一个
区域未被访问，它就会得到负温度，且未访问时间越长温度越低。排序按温度升序
进行，因此列表顶部的区域最冷，底部的区域最热。)

The list shows not seemingly hot regions, and only minimum access pattern
diversity.  Every region has zero access frequency.  The number of region is
10, which is the default ``min_nr_regions value``.  Size of each region is also
nearly identical.  We can suspect this is because “adaptive regions adjustment”
mechanism was not well working.  As the guide suggested, we can get relative
hotness of regions using ``age`` as the recency information.  That would be
better than nothing, but given the fact that the longest age is only about 6
seconds while we waited about ten minutes, it is unclear how useful this will
be.
(该列表没有显示出明显热的区域，访问模式的多样性也很有限。每个区域的访问频率
都是 0。区域数量为 10，即默认的 ``min_nr_regions value``。每个区域的大小也几乎
相同。我们可以怀疑这是"自适应区域调整 (adaptive regions adjustment)"机制没有
很好地工作。正如指南所建议的，我们可以使用 ``age`` 作为新近性 (recency)
信息来获得区域的相对热度。这总比没有好，但考虑到最长的 age 也只有约 6 秒而
我们等待了约十分钟，很难说这有多大用处。)

The temperature ranges to total size of regions of each range histogram
visualization of the results also shows no interesting distribution pattern. ::

    # damo report access --style temperature-sz-hist
    <temperature> <total size>
    [-,590,000,000, -,549,000,000) 5.985 GiB  |**********          |
    [-,549,000,000, -,508,000,000) 12.074 GiB |********************|
    [-,508,000,000, -,467,000,000) 0 B        |                    |
    [-,467,000,000, -,426,000,000) 12.052 GiB |********************|
    [-,426,000,000, -,385,000,000) 0 B        |                    |
    [-,385,000,000, -,344,000,000) 3.992 GiB  |*******             |
    [-,344,000,000, -,303,000,000) 5.213 GiB  |*********           |
    [-,303,000,000, -,262,000,000) 12.109 GiB |********************|
    [-,262,000,000, -,221,000,000) 5.282 GiB  |*********           |
    [-,221,000,000, -,180,000,000) 0 B        |                    |
    [-,180,000,000, -,139,000,000) 5.293 GiB  |*********           |
    total size: 62.000 GiB

(把各温度区间内区域的总大小做成直方图进行可视化，结果同样没有显示出有意思的
分布模式。)

In short, the parameters provide poor quality monitoring results for hot
regions detection. According to the :ref:`guide
<damon_design_monitoring_params_tuning_guide>`, this is due to the too short
aggregation interval.
(简而言之，这些参数为热区域检测提供的监测结果质量很差。根据 :ref:`guide
<damon_design_monitoring_params_tuning_guide>`，这是由于聚合间隔过短所致。)

100ms/2s intervals: Starts Showing Small Hot Regions(100ms/2s 间隔：开始显现小的热区域)
=======================================================================================

Following the guide, increase the interval 20 times (100 milliseocnds and 2
seconds for sampling and aggregation intervals, respectively). ::

    # damo start -s 100ms -a 2s
    # sleep 600
    # damo record --snapshot 0 1
    # damo stop
    # damo report access --sort_regions_by temperature
    0   addr 10.180 GiB   size 6.117 GiB   access 0 %   age 7 m 8 s    # coldest
    1   addr 49.275 GiB   size 6.195 GiB   access 0 %   age 6 m 14 s
    2   addr 62.421 GiB   size 3.579 GiB   access 0 %   age 6 m 4 s
    3   addr 40.154 GiB   size 6.127 GiB   access 0 %   age 5 m 40 s
    4   addr 16.296 GiB   size 6.182 GiB   access 0 %   age 5 m 32 s
    5   addr 34.254 GiB   size 5.899 GiB   access 0 %   age 5 m 24 s
    6   addr 46.281 GiB   size 2.995 GiB   access 0 %   age 5 m 20 s
    7   addr 28.420 GiB   size 5.835 GiB   access 0 %   age 5 m 6 s
    8   addr 4.000 GiB    size 6.180 GiB   access 0 %   age 4 m 16 s
    9   addr 22.478 GiB   size 5.942 GiB   access 0 %   age 3 m 58 s
    10  addr 55.470 GiB   size 915.645 MiB access 0 %   age 3 m 6 s
    11  addr 56.364 GiB   size 6.056 GiB   access 0 %   age 2 m 8 s
    12  addr 56.364 GiB   size 4.000 KiB   access 95 %  age 16 s
    13  addr 49.275 GiB   size 4.000 KiB   access 100 % age 8 m 24 s   # hottest
    total size: 62.000 GiB
    # damo report access --style temperature-sz-hist
    <temperature> <total size>
    [-42,800,000,000, -33,479,999,000) 22.018 GiB |*****************   |
    [-33,479,999,000, -24,159,998,000) 27.090 GiB |********************|
    [-24,159,998,000, -14,839,997,000) 6.836 GiB  |******              |
    [-14,839,997,000, -5,519,996,000)  6.056 GiB  |*****               |
    [-5,519,996,000, 3,800,005,000)    4.000 KiB  |*                   |
    [3,800,005,000, 13,120,006,000)    0 B        |                    |
    [13,120,006,000, 22,440,007,000)   0 B        |                    |
    [22,440,007,000, 31,760,008,000)   0 B        |                    |
    [31,760,008,000, 41,080,009,000)   0 B        |                    |
    [41,080,009,000, 50,400,010,000)   0 B        |                    |
    [50,400,010,000, 59,720,011,000)   4.000 KiB  |*                   |
    total size: 62.000 GiB

(按照指南，把间隔增大 20 倍(采样间隔与聚合间隔分别为 100 毫秒和 2 秒)。)

DAMON found two distinct 4 KiB regions that pretty hot.  The regions are also
well aged.  The hottest 4 KiB region was keeping the access frequency for about
8 minutes, and the coldest region was keeping no access for about 7 minutes.
The distribution on the histogram also looks like having a pattern.
(DAMON 发现了两个截然不同的、相当热的 4 KiB 区域。这两个区域也经过了充分的
老化：最热的 4 KiB 区域保持其访问频率约 8 分钟，最冷的区域保持无访问约
7 分钟。直方图上的分布看起来也有了模式。)

Especially, the finding of the 4 KiB regions among the 62 GiB total memory
shows DAMON’s adaptive regions adjustment is working as designed.
(尤其是，在总共 62 GiB 内存中找到这些 4 KiB 区域，表明 DAMON 的自适应区域
调整机制正如设计预期的那样工作。)

Still the number of regions is close to the ``min_nr_regions``, and sizes of
cold regions are similar, though.  Apparently it is improved, but it still has
rooms to improve.
(不过，区域数量仍然接近 ``min_nr_regions``，冷区域的大小仍然相似。情况显然
有所改善，但仍有改进空间。)

400ms/8s intervals: Pretty Improved Results(400ms/8s 间隔：结果已相当改善)
==========================================================================

Increase the intervals four times (400 milliseconds and 8 seconds
for sampling and aggregation intervals, respectively). ::

    # damo start -s 400ms -a 8s
    # sleep 600
    # damo record --snapshot 0 1
    # damo stop
    # damo report access --sort_regions_by temperature
    0   addr 64.492 GiB   size 1.508 GiB   access 0 %   age 6 m 48 s    # coldest
    1   addr 21.749 GiB   size 5.674 GiB   access 0 %   age 6 m 8 s
    2   addr 27.422 GiB   size 5.801 GiB   access 0 %   age 6 m
    3   addr 49.431 GiB   size 8.675 GiB   access 0 %   age 5 m 28 s
    4   addr 33.223 GiB   size 5.645 GiB   access 0 %   age 5 m 12 s
    5   addr 58.321 GiB   size 6.170 GiB   access 0 %   age 5 m 4 s
    [...]
    25  addr 6.615 GiB    size 297.531 MiB access 15 %  age 0 ns
    26  addr 9.513 GiB    size 12.000 KiB  access 20 %  age 0 ns
    27  addr 9.511 GiB    size 108.000 KiB access 25 %  age 0 ns
    28  addr 9.513 GiB    size 20.000 KiB  access 25 %  age 0 ns
    29  addr 9.511 GiB    size 12.000 KiB  access 30 %  age 0 ns
    30  addr 9.520 GiB    size 4.000 KiB   access 40 %  age 0 ns
    [...]
    41  addr 9.520 GiB    size 4.000 KiB   access 80 %  age 56 s
    42  addr 9.511 GiB    size 12.000 KiB  access 100 % age 6 m 16 s
    43  addr 58.321 GiB   size 4.000 KiB   access 100 % age 6 m 24 s
    44  addr 9.512 GiB    size 4.000 KiB   access 100 % age 6 m 48 s
    45  addr 58.106 GiB   size 4.000 KiB   access 100 % age 6 m 48 s    # hottest
    total size: 62.000 GiB
    # damo report access --style temperature-sz-hist
    <temperature> <total size>
    [-40,800,000,000, -32,639,999,000) 21.657 GiB  |********************|
    [-32,639,999,000, -24,479,998,000) 17.938 GiB  |*****************   |
    [-24,479,998,000, -16,319,997,000) 16.885 GiB  |****************    |
    [-16,319,997,000, -8,159,996,000)  586.879 MiB |*                   |
    [-8,159,996,000, 5,000)            4.946 GiB   |*****               |
    [5,000, 8,160,006,000)             260.000 KiB |*                   |
    [8,160,006,000, 16,320,007,000)    0 B         |                    |
    [16,320,007,000, 24,480,008,000)   0 B         |                    |
    [24,480,008,000, 32,640,009,000)   0 B         |                    |
    [32,640,009,000, 40,800,010,000)   16.000 KiB  |*                   |
    [40,800,010,000, 48,960,011,000)   8.000 KiB   |*                   |
    total size: 62.000 GiB

(把间隔再增大 4 倍(采样间隔与聚合间隔分别为 400 毫秒和 8 秒)。)

The number of regions having different access patterns has significantly
increased.  Size of each region is also more varied. Total size of non-zero
access frequency regions is also significantly increased. Maybe this is already
good enough to make some meaningful memory management efficiency changes.
(具有不同访问模式的区域数量显著增加。每个区域的大小也更加多样。非零访问频率
区域的总大小也显著增加。也许这已经足以支持做出一些有意义的内存管理效率
调整。)

800ms/16s intervals: Another bias(800ms/16s 间隔：另一种偏差)
=============================================================

Further double the intervals (800 milliseconds and 16 seconds for sampling
and aggregation intervals, respectively).  The results is more improved for the
hot regions detection, but starts looking degrading cold regions detection. ::

    # damo start -s 800ms -a 16s
    # sleep 600
    # damo record --snapshot 0 1
    # damo stop
    # damo report access --sort_regions_by temperature
    0   addr 64.781 GiB   size 1.219 GiB   access 0 %   age 4 m 48 s
    1   addr 24.505 GiB   size 2.475 GiB   access 0 %   age 4 m 16 s
    2   addr 26.980 GiB   size 504.273 MiB access 0 %   age 4 m
    3   addr 29.443 GiB   size 2.462 GiB   access 0 %   age 4 m
    4   addr 37.264 GiB   size 5.645 GiB   access 0 %   age 4 m
    5   addr 31.905 GiB   size 5.359 GiB   access 0 %   age 3 m 44 s
    [...]
    20  addr 8.711 GiB    size 40.000 KiB  access 5 %   age 2 m 40 s
    21  addr 27.473 GiB   size 1.970 GiB   access 5 %   age 4 m
    22  addr 48.185 GiB   size 4.625 GiB   access 5 %   age 4 m
    23  addr 47.304 GiB   size 902.117 MiB access 10 %  age 4 m
    24  addr 8.711 GiB    size 4.000 KiB   access 100 % age 4 m
    25  addr 20.793 GiB   size 3.713 GiB   access 5 %   age 4 m 16 s
    26  addr 8.773 GiB    size 4.000 KiB   access 100 % age 4 m 16 s
    total size: 62.000 GiB
    # damo report access --style temperature-sz-hist
    <temperature> <total size>
    [-28,800,000,000, -23,359,999,000) 12.294 GiB  |*****************   |
    [-23,359,999,000, -17,919,998,000) 9.753 GiB   |*************       |
    [-17,919,998,000, -12,479,997,000) 15.131 GiB  |********************|
    [-12,479,997,000, -7,039,996,000)  0 B         |                    |
    [-7,039,996,000, -1,599,995,000)   7.506 GiB   |**********          |
    [-1,599,995,000, 3,840,006,000)    6.127 GiB   |*********           |
    [3,840,006,000, 9,280,007,000)     0 B         |                    |
    [9,280,007,000, 14,720,008,000)    136.000 KiB |*                   |
    [14,720,008,000, 20,160,009,000)   40.000 KiB  |*                   |
    [20,160,009,000, 25,600,010,000)   11.188 GiB  |***************     |
    [25,600,010,000, 31,040,011,000)   4.000 KiB   |*                   |
    total size: 62.000 GiB

(再把间隔加倍(采样间隔与聚合间隔分别为 800 毫秒和 16 秒)。结果在热区域检测
方面进一步改善，但开始显出冷区域检测退化的迹象。)

It found more non-zero access frequency regions. The number of regions is still
much higher than the ``min_nr_regions``, but it is reduced from that of the
previous setup. And apparently the distribution seems bit biased to hot
regions.
(它找到了更多非零访问频率的区域。区域数量仍然远高于 ``min_nr_regions``，但比
上一个设置有所减少。而且分布看起来明显偏向热区域。)

Conclusion(结论)
================

With the above experimental tuning results, we can conclude the theory and the
guide makes sense to at least this workload, and could be applied to similar
cases.
(基于上述实验性调优结果，我们可以得出结论：该理论和指南至少对这一工作负载是
有意义的，并且可以应用于类似的场景。)
