=================================================================
 GPU Power/Thermal Controls and Monitoring(GPU 电源/热控制与监控)
=================================================================

HWMON Interfaces(HWMON 接口)
============================

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: hwmon

GPU sysfs Power State Interfaces(GPU sysfs 电源状态接口)
========================================================

GPU power controls are exposed via sysfs files.
(GPU 电源控制通过 sysfs 文件暴露。)

power_dpm_state(电源 DPM 状态)
------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: power_dpm_state

power_dpm_force_performance_level(电源 DPM 强制性能级别)
--------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: power_dpm_force_performance_level

pp_table(PP 表)
---------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: pp_table

pp_od_clk_voltage(PP 超频时钟电压)
----------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: pp_od_clk_voltage

pp_dpm_*(PP DPM 各项)
---------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: pp_dpm_sclk pp_dpm_mclk pp_dpm_socclk pp_dpm_fclk pp_dpm_dcefclk pp_dpm_pcie

pp_power_profile_mode(PP 电源配置模式)
--------------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: pp_power_profile_mode

pm_policy(电源管理策略)
-----------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: pm_policy

\*_busy_percent(\*_占用百分比)
------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: gpu_busy_percent

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: mem_busy_percent

gpu_metrics(GPU 指标)
---------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: gpu_metrics

fan_curve(风扇曲线)
-------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: fan_curve

acoustic_limit_rpm_threshold(声学上限 RPM 阈值)
-----------------------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: acoustic_limit_rpm_threshold

acoustic_target_rpm_threshold(声学目标 RPM 阈值)
------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: acoustic_target_rpm_threshold

fan_target_temperature(风扇目标温度)
------------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: fan_target_temperature

fan_minimum_pwm(风扇最小 PWM)
-----------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: fan_minimum_pwm

fan_zero_rpm_enable(风扇零转速启用)
-----------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: fan_zero_rpm_enable

fan_zero_rpm_stop_temperature(风扇零转速停止温度)
-------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: fan_zero_rpm_stop_temperature

GFXOFF(GFXOFF 功能)
===================

GFXOFF is a feature found in most recent GPUs that saves power at runtime. The
card's RLC (RunList Controller) firmware powers off the gfx engine
dynamically when there is no workload on gfx or compute pipes. GFXOFF is on by
default on supported GPUs.
(GFXOFF 是大多数最新 GPU 中都有的特性，可在运行时节省功耗。当 gfx 或 compute 管线上没有工作负载时，显卡的 RLC (RunList Controller) 固件会动态地为 gfx 引擎断电。在受支持的 GPU 上，GFXOFF 默认开启。)

Userspace can interact with GFXOFF through a debugfs interface (all values in
`uint32_t`, unless otherwise noted):
(用户空间可以通过一个 debugfs 接口与 GFXOFF 交互(除非另有说明，所有值都是 `uint32_t` 类型)：)

``amdgpu_gfxoff`` (GFXOFF 开关)
-------------------------------

Use it to enable/disable GFXOFF, and to check if it's current enabled/disabled::

  $ xxd -l1 -p /sys/kernel/debug/dri/0/amdgpu_gfxoff
  01

(用它来启用/禁用 GFXOFF，并检查它当前是启用还是禁用：)

- Write 0 to disable it, and 1 to enable it.
  (写入 0 禁用它，写入 1 启用它。)
- Read 0 means it's disabled, 1 it's enabled.
  (读取到 0 表示它已禁用，读取到 1 表示已启用。)

If it's enabled, that means that the GPU is free to enter into GFXOFF mode as
needed. Disabled means that it will never enter GFXOFF mode.
(如果它是启用的，意味着 GPU 可以在需要时自由进入 GFXOFF 模式。禁用则意味着它永远不会进入 GFXOFF 模式。)

``amdgpu_gfxoff_status`` (GFXOFF 状态)
--------------------------------------

Read it to check current GFXOFF's status of a GPU::

  $ xxd -l1 -p /sys/kernel/debug/dri/0/amdgpu_gfxoff_status
  02

(读取它以检查 GPU 当前的 GFXOFF 状态：)

- 0: GPU is in GFXOFF state, the gfx engine is powered down.
  (0: GPU 处于 GFXOFF 状态，gfx 引擎已断电。)
- 1: Transition out of GFXOFF state
  (1: 正在从 GFXOFF 状态转换出来)
- 2: Not in GFXOFF state
  (2: 不处于 GFXOFF 状态)
- 3: Transition into GFXOFF state
  (3: 正在转换进入 GFXOFF 状态)

If GFXOFF is enabled, the value will be transitioning around [0, 3], always
getting into 0 when possible. When it's disabled, it's always at 2. Returns
``-EINVAL`` if it's not supported.
(如果 GFXOFF 已启用，该值会在 [0, 3] 之间不断变化，只要可能就会变为 0。当它被禁用时，它始终为 2。如果不支持，则返回 ``-EINVAL`` 。)

``amdgpu_gfxoff_count`` (GFXOFF 进入次数)
-----------------------------------------

Read it to get the total GFXOFF entry count at the time of query since system
power-up. The value is an `uint64_t` type, however, due to firmware limitations,
it can currently overflow as an `uint32_t`. *Only supported in vangogh*
(读取它以获取自系统上电以来查询时刻的 GFXOFF 进入总次数。该值是 `uint64_t` 类型，但由于固件限制，它目前可能会以 `uint32_t` 溢出。*仅在 vangogh 上受支持*)

``amdgpu_gfxoff_residency`` (GFXOFF 驻留率)
-------------------------------------------

Write 1 to amdgpu_gfxoff_residency to start logging, and 0 to stop. Read it to
get average GFXOFF residency % multiplied by 100 during the last logging
interval. E.g. a value of 7854 means 78.54% of the time in the last logging
interval the GPU was in GFXOFF mode. *Only supported in vangogh*
(向 amdgpu_gfxoff_residency 写入 1 开始记录，写入 0 停止。读取它以获取上一个记录间隔内的平均 GFXOFF 驻留率 (%) 乘以 100 的结果。例如，值 7854 表示在上一个记录间隔中，GPU 有 78.54% 的时间处于 GFXOFF 模式。*仅在 vangogh 上受支持*)
