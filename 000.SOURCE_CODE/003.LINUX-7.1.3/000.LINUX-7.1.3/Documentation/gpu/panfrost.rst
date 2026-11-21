.. SPDX-License-Identifier: GPL-2.0+

=================================================
 drm/Panfrost Mali Driver(drm/Panfrost Mali 驱动)
=================================================

.. _panfrost-usage-stats:

Panfrost DRM client usage stats implementation(Panfrost DRM 客户端使用统计实现)
===============================================================================

The drm/Panfrost driver implements the DRM client usage stats specification as
documented in :ref:`drm-client-usage-stats`.
(drm/Panfrost 驱动实现了 :ref:`drm-client-usage-stats` 中描述的 DRM 客户端
使用统计规范。)

Example of the output showing the implemented key value pairs and entirety of
the currently possible format options:

::
      pos:    0
      flags:  02400002
      mnt_id: 27
      ino:    531
      drm-driver:     panfrost
      drm-client-id:  14
      drm-engine-fragment:    1846584880 ns
      drm-cycles-fragment:    1424359409
      drm-maxfreq-fragment:   799999987 Hz
      drm-curfreq-fragment:   799999987 Hz
      drm-engine-vertex-tiler:        71932239 ns
      drm-cycles-vertex-tiler:        52617357
      drm-maxfreq-vertex-tiler:       799999987 Hz
      drm-curfreq-vertex-tiler:       799999987 Hz
      drm-total-memory:       290 MiB
      drm-shared-memory:      0 MiB
      drm-active-memory:      226 MiB
      drm-resident-memory:    36496 KiB
      drm-purgeable-memory:   128 KiB

(下面是输出的示例，展示了已实现的键值对以及当前所有可能的格式选项：)

Possible `drm-engine-` key names are: `fragment`, and  `vertex-tiler`.
`drm-curfreq-` values convey the current operating frequency for that engine.
(`drm-engine-` 可能的键名有：`fragment` 和 `vertex-tiler` 。
`drm-curfreq-` 的值表示该引擎当前的工作频率。)

Users must bear in mind that engine and cycle sampling are disabled by default,
because of power saving concerns. `fdinfo` users and benchmark applications which
query the fdinfo file must make sure to toggle the job profiling status of the
driver by writing into the appropriate sysfs node::

    echo <N> > /sys/bus/platform/drivers/panfrost/[a-f0-9]*.gpu/profiling

(用户必须记住，出于节能考虑，引擎与周期采样默认是禁用的。查询 fdinfo 文件的
`fdinfo` 用户和基准测试应用必须确保通过写入相应的 sysfs 节点来切换驱动的
作业分析 (job profiling) 状态：)

(其中 `N` 为 `0` 或 `1`，取决于期望的启用状态。)

Where `N` is either `0` or `1`, depending on the desired enablement status.
