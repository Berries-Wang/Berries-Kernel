.. SPDX-License-Identifier: GPL-2.0

========================
Devlink Shared Instances
========================

(devlink 共享实例)

Overview
(概述)
========

Shared devlink instances allow multiple physical functions (PFs) on the same
chip to share a devlink instance for chip-wide operations.
(共享 devlink 实例允许同一芯片上的多个物理功能 (PF) 共享一个 devlink
实例，用于芯片级的操作。)

Multiple PFs may reside on the same physical chip, running a single firmware.
Some of the resources and configurations may be shared among these PFs. The
shared devlink instance provides an object to pin configuration knobs on.
(多个 PF 可以驻留在同一物理芯片上，运行单一固件。某些资源和配置可能在这些
PF 之间共享。共享 devlink 实例提供了一个可以在其上固定配置旋钮的对象。)

There are two possible usage models:
(有两种可能的使用模型：)

1. The shared devlink instance is used alongside individual PF devlink
   instances, providing chip-wide configuration in addition to per-PF
   configuration.
   (共享 devlink 实例与各个 PF 的 devlink 实例一起使用，在每 PF 配置之外
   提供芯片级配置。)
2. The shared devlink instance is the only devlink instance, without
   per-PF instances.
   (共享 devlink 实例是唯一的 devlink 实例，没有每 PF 实例。)

It is up to the driver to decide which usage model to use.
(由驱动决定使用哪种使用模型。)

The shared devlink instance is not backed by any struct *device*.
(共享 devlink 实例不由任何 struct *device* 支撑。)

Implementation
(实现)
==============

Architecture
(架构)
------------

The implementation uses:
(该实现使用了：)

* **Chip identification**: PFs are grouped by chip using a driver-specific identifier
  (**芯片标识**： 使用驱动特定的标识符把 PF 按芯片分组)
* **Shared instance management**: Global list of shared instances with reference counting
  (**共享实例管理**： 带引用计数的共享实例全局列表)

API Functions
(API 函数)
-------------

The following functions are provided for managing shared devlink instances:
(提供以下函数用于管理共享 devlink 实例：)

* ``devlink_shd_get()``: Get or create a shared devlink instance identified by a string ID
  (``devlink_shd_get()``： 获取或创建由字符串 ID 标识的共享 devlink 实例)
* ``devlink_shd_put()``: Release a reference on a shared devlink instance
  (``devlink_shd_put()``： 释放共享 devlink 实例上的一个引用)
* ``devlink_shd_get_priv()``: Get private data from shared devlink instance
  (``devlink_shd_get_priv()``： 从共享 devlink 实例获取私有数据)

Initialization Flow
(初始化流程)
-------------------

1. **PF calls shared devlink init** during driver probe
   (**PF 调用共享 devlink 初始化**) 在驱动探测期间
2. **Chip identification** using driver-specific method to determine device identity
   (**芯片识别**) 使用驱动特定的方法来确定设备身份
3. **Get or create shared instance** using ``devlink_shd_get()``:
   (**获取或创建共享实例**) 使用 ``devlink_shd_get()``：

   * The function looks up existing instance by identifier
     (该函数按标识符查找现有实例)
   * If none exists, creates new instance:
     (如果不存在，则创建新实例：)

     - Allocates and registers devlink instance
       (分配并注册 devlink 实例)
     - Adds to global shared instances list
       (加入全局共享实例列表)
     - Increments reference count
       (递增引用计数)

4. **Set nested devlink instance** for the PF devlink instance using
   ``devl_nested_devlink_set()`` before registering the PF devlink instance
   (**设置嵌套 devlink 实例**) 在注册 PF devlink 实例之前，使用
   ``devl_nested_devlink_set()`` 为 PF devlink 实例设置嵌套 devlink 实例

Cleanup Flow
(清理流程)
------------

1. **Cleanup** when PF is removed
   (**清理**) 当 PF 被移除时
2. **Call** ``devlink_shd_put()`` to release reference (decrements reference count)
   (**调用**) ``devlink_shd_put()`` 以释放引用 (递减引用计数)
3. **Shared instance is automatically destroyed** when the last PF removes (reference count reaches zero)
   (**共享实例被自动销毁**) 当最后一个 PF 移除时 (引用计数达到零)

Chip Identification
(芯片识别)
-------------------

PFs belonging to the same chip are identified using a driver-specific method.
The driver is free to choose any identifier that is suitable for determining
whether two PFs are part of the same device. Examples include:
(属于同一芯片的 PF 通过驱动特定的方法来识别。驱动可以自由选择任何适合
判断两个 PF 是否属于同一设备的标识符。示例包括：)

* **PCI VPD serial numbers**: Extract from PCI VPD
  (**PCI VPD 序列号**： 从 PCI VPD 中提取)
* **Device tree properties**: Read chip identifier from device tree
  (**设备树属性**： 从设备树读取芯片标识符)
* **Other hardware-specific identifiers**: Any unique identifier that groups PFs by chip
  (**其他硬件特定标识符**： 任何能按芯片对 PF 进行分组的唯一标识符)

Locking
(加锁)
-------

A global mutex (``shd_mutex``) protects the shared instances list during registration/deregistration.
(一个全局互斥锁 (``shd_mutex``) 在注册/注销期间保护共享实例列表。)

Similarly to other nested devlink instance relationships, devlink lock of
the shared instance should be always taken after the devlink lock of PF.
(与其他嵌套 devlink 实例关系类似，共享实例的 devlink 锁应当总是在 PF
的 devlink 锁之后获取。)

Reference Counting
(引用计数)
------------------

Each shared devlink instance maintains a reference count (``refcount_t refcount``).
The reference count is incremented when ``devlink_shd_get()`` is called and decremented
when ``devlink_shd_put()`` is called. When the reference count reaches zero, the shared
instance is automatically destroyed.
(每个共享 devlink 实例都维护一个引用计数 (``refcount_t refcount``)。
调用 ``devlink_shd_get()`` 时引用计数递增，调用 ``devlink_shd_put()``
时递减。当引用计数达到零时，共享实例会被自动销毁。)
