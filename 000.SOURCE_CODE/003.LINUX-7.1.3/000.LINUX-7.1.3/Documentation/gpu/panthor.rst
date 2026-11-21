.. SPDX-License-Identifier: GPL-2.0+

=============================================
 drm/Panthor CSF driver(drm/Panthor CSF 驱动)
=============================================

.. _panthor-usage-stats:

Panthor DRM client usage stats implementation(Panthor DRM 客户端使用统计实现)
=============================================================================

The drm/Panthor driver implements the DRM client usage stats specification as
documented in :ref:`drm-client-usage-stats`.
(drm/Panthor 驱动实现了 :ref:`drm-client-usage-stats` 中描述的 DRM 客户端
使用统计规范。)

Example of the output showing the implemented key value pairs and entirety of
the currently possible format options:

::
     pos:    0
     flags:  02400002
     mnt_id: 29
     ino:    491
     drm-driver:     panthor
     drm-client-id:  10
     drm-engine-panthor:     111110952750 ns
     drm-cycles-panthor:     94439687187
     drm-maxfreq-panthor:    1000000000 Hz
     drm-curfreq-panthor:    1000000000 Hz
     panthor-resident-memory:        10396 KiB
     panthor-active-memory:  10396 KiB
     drm-total-memory:       16480 KiB
     drm-shared-memory:      0
     drm-active-memory:      16200 KiB
     drm-resident-memory:    16480 KiB
     drm-purgeable-memory:   0

(下面是输出的示例，展示了已实现的键值对以及当前所有可能的格式选项：)

Possible `drm-engine-` key names are: `panthor`.
`drm-curfreq-` values convey the current operating frequency for that engine.
(`drm-engine-` 可能的键名有：`panthor` 。
`drm-curfreq-` 的值表示该引擎当前的工作频率。)

Users must bear in mind that engine and cycle sampling are disabled by default,
because of power saving concerns. `fdinfo` users and benchmark applications which
query the fdinfo file must make sure to toggle the job profiling status of the
driver by writing into the appropriate sysfs node::

    echo <N> > /sys/bus/platform/drivers/panthor/[a-f0-9]*.gpu/profiling

(用户必须记住，出于节能考虑，引擎与周期采样默认是禁用的。查询 fdinfo 文件的
`fdinfo` 用户和基准测试应用必须确保通过写入相应的 sysfs 节点来切换驱动的
作业分析 (job profiling) 状态：)

(其中 `N` 是一个位掩码，周期采样与时间戳采样分别由第 1 位和第 2 位启用。)

Where `N` is a bit mask where cycle and timestamp sampling are respectively
enabled by the first and second bits.

Possible `panthor-*-memory` keys are: `active` and `resident`.
These values convey the sizes of the internal driver-owned shmem BO's that
aren't exposed to user-space through a DRM handle, like queue ring buffers,
sync object arrays and heap chunks. Because they are all allocated and pinned
at creation time, only `panthor-resident-memory` is necessary to tell us their
size. `panthor-active-memory` shows the size of kernel BO's associated with
VM's and groups currently being scheduled for execution by the GPU.
(`panthor-*-memory` 可能的键有：`active` 和 `resident` 。这些值表示未通过
DRM 句柄暴露给用户态的、驱动内部自有的 shmem BO 的大小，例如队列环形缓冲
区、同步对象数组和堆块 (heap chunk)。因为它们都是在创建时分配并固定的，
所以只需要 `panthor-resident-memory` 就能告诉我们它们的大小。
`panthor-active-memory` 显示当前正被 GPU 调度执行的 VM 与组所关联的内核
BO 的大小。)
