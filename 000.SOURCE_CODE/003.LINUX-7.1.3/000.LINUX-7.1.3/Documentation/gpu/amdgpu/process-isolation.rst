.. _amdgpu-process-isolation:
.. SPDX-License-Identifier: GPL-2.0

==========================================
 AMDGPU Process Isolation(AMDGPU 进程隔离)
==========================================

The AMDGPU driver includes a feature that enables automatic process isolation on the graphics engine. This feature serializes access to the graphics engine and adds a cleaner shader which clears the Local Data Store (LDS) and General Purpose Registers (GPRs) between jobs. All processes using the GPU, including both graphics and compute workloads, are serialized when this feature is enabled. On GPUs that support partitionable graphics engines, this feature can be enabled on a per-partition basis.
(AMDGPU 驱动包含一项可在图形引擎上启用自动进程隔离的功能。该功能对图形引擎
的访问进行串行化，并添加一个清理着色器 (cleaner shader)，在作业之间清除本地
数据存储 (LDS) 和通用寄存器 (GPR)。启用该功能后，所有使用 GPU 的进程——包括
图形与计算负载——都会被串行化。在支持可分区图形引擎的 GPU 上，该功能可以按
分区启用。)

In addition, there is an interface to manually run the cleaner shader when the use of the GPU is complete. This may be preferable in some use cases, such as a single-user system where the login manager triggers the cleaner shader when the user logs out.
(此外，还有一个在 GPU 使用完毕后手动运行清理着色器的接口。在某些使用场景下
这可能是更好的选择，例如单用户系统中，登录管理器在用户注销时触发清理着色
器。)

Process Isolation(进程隔离)
===========================

The `run_cleaner_shader` and `enforce_isolation` sysfs interfaces allow users to manually execute the cleaner shader and control the process isolation feature, respectively.
(`run_cleaner_shader` 与 `enforce_isolation` 这两个 sysfs 接口分别允许用户
手动执行清理着色器，以及控制进程隔离功能。)

Partition Handling(分区处理)
----------------------------

The `enforce_isolation` file in sysfs can be used to enable process isolation and automatic shader cleanup between processes. On GPUs that support graphics engine partitioning, this can be enabled per partition. The partition and its current setting (0 disabled, 1 enabled) can be read from sysfs. On GPUs that do not support graphics engine partitioning, only a single partition will be present. Writing 1 to the partition position enables enforce isolation, writing 0 disables it.
(sysfs 中的 `enforce_isolation` 文件可用于启用进程隔离以及进程间的自动着色
器清理。在支持图形引擎分区的 GPU 上，可以按分区启用。分区及其当前设置
(0 表示禁用，1 表示启用) 可以从 sysfs 读取。在不支持图形引擎分区的 GPU 上，
只会存在单个分区。向分区位置写入 1 启用强制隔离 (enforce isolation)，写入
0 禁用它。)

Example of enabling enforce isolation on a GPU with multiple partitions:
(在具有多个分区的 GPU 上启用强制隔离的示例：)

.. code-block:: console

    $ echo 1 0 1 0 > /sys/class/drm/card0/device/enforce_isolation
    $ cat /sys/class/drm/card0/device/enforce_isolation
    1 0 1 0

The output indicates that enforce isolation is enabled on zeroth and second partition and disabled on first and third partition.
(该输出表示强制隔离在第 0 和第 2 分区上启用，在第 1 和第 3 分区上禁用。)

For devices with a single partition or those that do not support partitions, there will be only one element:
(对于只有单个分区或不支持分区的设备，将只有一个元素：)

.. code-block:: console

    $ echo 1 > /sys/class/drm/card0/device/enforce_isolation
    $ cat /sys/class/drm/card0/device/enforce_isolation
    1

Cleaner Shader Execution(清理着色器执行)
========================================

The driver can trigger a cleaner shader to clean up the LDS and GPR state on the graphics engine. When process isolation is enabled, this happens automatically between processes. In addition, there is a sysfs file to manually trigger cleaner shader execution.
(驱动可以触发一个清理着色器来清理图形引擎上的 LDS 和 GPR 状态。启用进程
隔离时，这会在进程之间自动进行。此外，还有一个 sysfs 文件用于手动触发清理
着色器的执行。)

To manually trigger the execution of the cleaner shader, write `0` to the `run_cleaner_shader` sysfs file:
(要手动触发清理着色器的执行，向 `run_cleaner_shader` sysfs 文件写入 `0` ：)

.. code-block:: console

    $ echo 0 > /sys/class/drm/card0/device/run_cleaner_shader

For multi-partition devices, you can specify the partition index when triggering the cleaner shader:
(对于多分区设备，触发清理着色器时可以指定分区索引：)

.. code-block:: console

    $ echo 0 > /sys/class/drm/card0/device/run_cleaner_shader # For partition 0
    $ echo 1 > /sys/class/drm/card0/device/run_cleaner_shader # For partition 1
    $ echo 2 > /sys/class/drm/card0/device/run_cleaner_shader # For partition 2
    # ... and so on for each partition

This command initiates the cleaner shader, which will run and complete before any new tasks are scheduled on the GPU.
(此命令会启动清理着色器，它会在任何新任务被调度到 GPU 之前运行并完成。)
