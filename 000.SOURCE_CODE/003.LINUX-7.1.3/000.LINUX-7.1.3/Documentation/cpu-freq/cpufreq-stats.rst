.. SPDX-License-Identifier: GPL-2.0

========================================================================
General Description of sysfs CPUFreq Stats(sysfs CPUFreq 统计的一般描述)
========================================================================

information for users
(面向用户的信息)


Author: Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>
(作者: Venkatesh Pallipadi <venkatesh.pallipadi@intel.com>)

.. Contents

   1. Introduction
   2. Statistics Provided (with example)
   3. Configuring cpufreq-stats


1. Introduction(1. 引言)
========================

cpufreq-stats is a driver that provides CPU frequency statistics for each CPU.
These statistics are provided in /sysfs as a bunch of read_only interfaces. This
interface (when configured) will appear in a separate directory under cpufreq
in /sysfs (<sysfs root>/devices/system/cpu/cpuX/cpufreq/stats/) for each CPU.
Various statistics will form read_only files under this directory.
(cpufreq-stats 是一个为每个 CPU 提供 CPU 频率统计信息的驱动。这些统计信息以一组只读接口的形式在 /sysfs 中提供。配置后，该接口会在 /sysfs 中 cpufreq 下的一个单独目录（<sysfs root>/devices/system/cpu/cpuX/cpufreq/stats/）中为每个 CPU 出现。各种统计信息将构成该目录下的只读文件。)

This driver is designed to be independent of any particular cpufreq_driver
that may be running on your CPU. So, it will work with any cpufreq_driver.
(该驱动的设计独立于你的 CPU 上可能运行的任何特定 cpufreq_driver。因此，它可以与任何 cpufreq_driver 配合工作。)


2. Statistics Provided (with example)(2. 提供的统计信息（附示例）)
==================================================================

cpufreq stats provides following statistics (explained in detail below).
(cpufreq stats 提供以下统计信息（下文将详细说明）。)

-  time_in_state
-  total_trans
-  trans_table

All the statistics will be from the time the stats driver has been inserted
(or the time the stats were reset) to the time when a read of a particular
statistic is done. Obviously, stats driver will not have any information
about the frequency transitions before the stats driver insertion.
(所有统计信息的时间范围都是从 stats 驱动被插入之时（或统计被重置之时）到读取某个具体统计信息之时。显然，stats 驱动不会拥有 stats 驱动插入之前的任何频率转换信息。)

::

    <mysystem>:/sys/devices/system/cpu/cpu0/cpufreq/stats # ls -l
    total 0
    drwxr-xr-x  2 root root    0 May 14 16:06 .
    drwxr-xr-x  3 root root    0 May 14 15:58 ..
    --w-------  1 root root 4096 May 14 16:06 reset
    -r--r--r--  1 root root 4096 May 14 16:06 time_in_state
    -r--r--r--  1 root root 4096 May 14 16:06 total_trans
    -r--r--r--  1 root root 4096 May 14 16:06 trans_table

- **reset**

Write-only attribute that can be used to reset the stat counters. This can be
useful for evaluating system behaviour under different governors without the
need for a reboot.
(只写属性，可用于重置统计计数器。这对于在无需重启的情况下评估系统在不同调频 governor 下的行为很有用。)

- **time_in_state**

This gives the amount of time spent in each of the frequencies supported by
this CPU. The cat output will have "<frequency> <time>" pair in each line, which
will mean this CPU spent <time> usertime units of time at <frequency>. Output
will have one line for each of the supported frequencies. usertime units here
is 10mS (similar to other time exported in /proc).
(给出在 CPU 支持的每个频率上所花费的时间。cat 输出每行包含 "<frequency> <time>" 对，表示该 CPU 在 <frequency> 频率上花费了 <time> 个 usertime 单位的时间。输出对每个支持的频率各有一行。这里的 usertime 单位是 10mS（类似于 /proc 中导出的其他时间）。)

::

    <mysystem>:/sys/devices/system/cpu/cpu0/cpufreq/stats # cat time_in_state
    3600000 2089
    3400000 136
    3200000 34
    3000000 67
    2800000 172488


- **total_trans**

This gives the total number of frequency transitions on this CPU. The cat
output will have a single count which is the total number of frequency
transitions.
(给出此 CPU 上频率转换的总次数。cat 输出只有一个计数，即频率转换的总数。)

::

    <mysystem>:/sys/devices/system/cpu/cpu0/cpufreq/stats # cat total_trans
    20

- **trans_table**

This will give a fine grained information about all the CPU frequency
transitions. The cat output here is a two dimensional matrix, where an entry
<i,j> (row i, column j) represents the count of number of transitions from
Freq_i to Freq_j. Freq_i rows and Freq_j columns follow the sorting order in
which the driver has provided the frequency table initially to the cpufreq core
and so can be sorted (ascending or descending) or unsorted.  The output here
also contains the actual freq values for each row and column for better
readability.
(给出关于所有 CPU 频率转换的细粒度信息。cat 输出是一个二维矩阵，其中条目 <i,j>（第 i 行，第 j 列）表示从 Freq_i 转换到 Freq_j 的次数。Freq_i 行与 Freq_j 列遵循驱动程序最初向 cpufreq 核心提供频率表时的排序顺序，因此可以是有序（升序或降序）或无序的。为了更好的可读性，这里的输出还包含每行每列的实际频率值。)

If the transition table is bigger than PAGE_SIZE, reading this will
return an -EFBIG error.
(如果转换表大于 PAGE_SIZE，读取它将返回 -EFBIG 错误。)

::

    <mysystem>:/sys/devices/system/cpu/cpu0/cpufreq/stats # cat trans_table
    From  :    To
	    :   3600000   3400000   3200000   3000000   2800000
    3600000:         0         5         0         0         0
    3400000:         4         0         2         0         0
    3200000:         0         1         0         2         0
    3000000:         0         0         1         0         3
    2800000:         0         0         0         2         0

3. Configuring cpufreq-stats(3. 配置 cpufreq-stats)
===================================================

To configure cpufreq-stats in your kernel::

	Config Main Menu
		Power management options (ACPI, APM)  --->
			CPU Frequency scaling  --->
				[*] CPU Frequency scaling
				[*]   CPU frequency translation statistics

(要在你的内核中配置 cpufreq-stats：)


"CPU Frequency scaling" (CONFIG_CPU_FREQ) should be enabled to configure
cpufreq-stats.
("CPU Frequency scaling"（CONFIG_CPU_FREQ）应当被启用，才能配置 cpufreq-stats。)

"CPU frequency translation statistics" (CONFIG_CPU_FREQ_STAT) provides the
statistics which includes time_in_state, total_trans and trans_table.
("CPU frequency translation statistics"（CONFIG_CPU_FREQ_STAT）提供统计信息，包括 time_in_state 、 total_trans 和 trans_table 。)

Once this option is enabled and your CPU supports cpufrequency, you
will be able to see the CPU frequency statistics in /sysfs.
(一旦启用该选项且你的 CPU 支持调频，你就能在 /sysfs 中看到 CPU 频率统计信息。)
