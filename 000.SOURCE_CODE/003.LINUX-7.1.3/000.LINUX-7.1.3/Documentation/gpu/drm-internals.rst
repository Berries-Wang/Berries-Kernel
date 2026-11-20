===========================
DRM Internals(DRM 内部机制)
===========================

This chapter documents DRM internals relevant to driver authors and
developers working to add support for the latest features to existing
drivers.
(本章记录与驱动作者，以及致力于为现有驱动添加最新特性支持的开发者相关的 DRM
内部机制。)

First, we go over some typical driver initialization requirements, like
setting up command buffers, creating an initial output configuration,
and initializing core services. Subsequent sections cover core internals
in more detail, providing implementation notes and examples.
(首先，我们介绍一些典型的驱动初始化要求，例如设置命令缓冲区、创建初始输出
配置以及初始化核心服务。后续各节更详细地介绍核心内部机制，并提供实现说明与
示例。)

The DRM layer provides several services to graphics drivers, many of
them driven by the application interfaces it provides through libdrm,
the library that wraps most of the DRM ioctls. These include vblank
event handling, memory management, output management, framebuffer
management, command submission & fencing, suspend/resume support, and
DMA services.
(DRM 层为图形驱动程序提供多项服务，其中许多由它通过 libdrm 提供的应用程序
接口驱动——libdrm 封装了大部分 DRM ioctl。这些服务包括 vblank 事件处理、
内存管理、输出管理、帧缓冲管理、命令提交与栅栏 (fencing)、挂起/恢复支持以及
DMA 服务。)

Driver Initialization(驱动程序初始化)
=====================================

At the core of every DRM driver is a :c:type:`struct drm_driver
<drm_driver>` structure. Drivers typically statically initialize
a drm_driver structure, and then pass it to
drm_dev_alloc() to allocate a device instance. After the
device instance is fully initialized it can be registered (which makes
it accessible from userspace) using drm_dev_register().
(每个 DRM 驱动程序的核心都是一个 :c:type:`struct drm_driver
<drm_driver>` 结构。驱动程序通常静态初始化一个 drm_driver 结构，然后把它
传给 drm_dev_alloc() 来分配设备实例。设备实例完全初始化之后，可以使用
drm_dev_register() 注册它(使其可从用户空间访问)。)

The :c:type:`struct drm_driver <drm_driver>` structure
contains static information that describes the driver and features it
supports, and pointers to methods that the DRM core will call to
implement the DRM API. We will first go through the :c:type:`struct
drm_driver <drm_driver>` static information fields, and will
then describe individual operations in details as they get used in later
sections.
(:c:type:`struct drm_driver <drm_driver>` 结构包含描述驱动程序及其所支持
特性的静态信息，以及指向 DRM 核心为实现 DRM API 而调用的方法的指针。我们先
过一遍 :c:type:`struct
drm_driver <drm_driver>` 的静态信息字段，然后在后面各节用到时详细描述各个
操作。)

Driver Information(驱动程序信息)
--------------------------------

Major, Minor and Patchlevel(主、次与补丁级别)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int major; int minor; int patchlevel;
The DRM core identifies driver versions by a major, minor and patch
level triplet. The information is printed to the kernel log at
initialization time and passed to userspace through the
DRM_IOCTL_VERSION ioctl.
(DRM 核心通过主版本号、次版本号与补丁级别的三元组来标识驱动程序版本。该信息
会在初始化时打印到内核日志，并通过 DRM_IOCTL_VERSION ioctl 传给用户空间。)

The major and minor numbers are also used to verify the requested driver
API version passed to DRM_IOCTL_SET_VERSION. When the driver API
changes between minor versions, applications can call
DRM_IOCTL_SET_VERSION to select a specific version of the API. If the
requested major isn't equal to the driver major, or the requested minor
is larger than the driver minor, the DRM_IOCTL_SET_VERSION call will
return an error. Otherwise the driver's set_version() method will be
called with the requested version.
(主版本号和次版本号还用于验证传给 DRM_IOCTL_SET_VERSION 的所请求驱动 API
版本。当驱动 API 在次版本之间发生变化时，应用程序可以调用
DRM_IOCTL_SET_VERSION 来选择 API 的特定版本。如果请求的主版本号不等于驱动
的主版本号，或请求的次版本号大于驱动的次版本号，DRM_IOCTL_SET_VERSION 调用
将返回错误。否则，将带着所请求的版本调用驱动的 set_version() 方法。)

Name and Description(名称与描述)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char \*name; char \*desc; char \*date;
The driver name is printed to the kernel log at initialization time,
used for IRQ registration and passed to userspace through
DRM_IOCTL_VERSION.
(驱动程序名称会在初始化时打印到内核日志，用于 IRQ 注册，并通过
DRM_IOCTL_VERSION 传给用户空间。)

The driver description is a purely informative string passed to
userspace through the DRM_IOCTL_VERSION ioctl and otherwise unused by
the kernel.
(驱动程序描述是一个纯粹的信息性字符串，通过 DRM_IOCTL_VERSION ioctl 传给
用户空间，内核在其他方面不使用它。)

Module Initialization(模块初始化)
---------------------------------

.. kernel-doc:: include/drm/drm_module.h
   :doc: overview

Device Instance and Driver Handling
-----------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_drv.c
   :doc: driver instance overview

.. kernel-doc:: include/drm/drm_device.h
   :internal:

.. kernel-doc:: include/drm/drm_drv.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_drv.c
   :export:

Driver Load(驱动加载)
---------------------

Component Helper Usage(组件辅助工具用法)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: drivers/gpu/drm/drm_drv.c
   :doc: component helper usage recommendations

Memory Manager Initialization(内存管理器初始化)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Every DRM driver requires a memory manager which must be initialized at
load time. DRM currently contains two memory managers, the Translation
Table Manager (TTM) and the Graphics Execution Manager (GEM). This
document describes the use of the GEM memory manager only. See ? for
details.
(每个 DRM 驱动程序都需要一个内存管理器，它必须在加载时初始化。DRM 目前包含
两个内存管理器：转换表管理器 (TTM) 和图形执行管理器 (GEM)。本文档只描述 GEM
内存管理器的使用。详情见 ?。)

Miscellaneous Device Configuration(杂项设备配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Another task that may be necessary for PCI devices during configuration
is mapping the video BIOS. On many devices, the VBIOS describes device
configuration, LCD panel timings (if any), and contains flags indicating
device state. Mapping the BIOS can be done using the pci_map_rom()
call, a convenience function that takes care of mapping the actual ROM,
whether it has been shadowed into memory (typically at address 0xc0000)
or exists on the PCI device in the ROM BAR. Note that after the ROM has
been mapped and any necessary information has been extracted, it should
be unmapped; on many devices, the ROM address decoder is shared with
other BARs, so leaving it mapped could cause undesired behaviour like
hangs or memory corruption.
(PCI 设备在配置期间可能需要的另一项任务是映射视频 BIOS。在许多设备上，VBIOS
描述设备配置、LCD 面板时序(如果有的话)，并包含指示设备状态的标志。可以使用
pci_map_rom() 调用来映射 BIOS；这是一个便捷函数，无论 ROM 是已被影子复制到
内存中(通常在地址 0xc0000 处)，还是存在于 PCI 设备的 ROM BAR 中，它都会负责
映射实际的 ROM。注意，在 ROM 被映射并提取了任何必要的信息之后，应当解除
映射；在许多设备上，ROM 地址解码器与其他 BAR 共享，因此让它保持映射可能导致
挂起或内存损坏等不良行为。)

Managed Resources(托管资源)
---------------------------

.. kernel-doc:: drivers/gpu/drm/drm_managed.c
   :doc: managed resources

.. kernel-doc:: drivers/gpu/drm/drm_managed.c
   :export:

.. kernel-doc:: include/drm/drm_managed.h
   :internal:

Open/Close, File Operations and IOCTLs(打开/关闭、文件操作与 IOCTL)
===================================================================

.. _drm_driver_fops:

File Operations(文件操作)
-------------------------

.. kernel-doc:: drivers/gpu/drm/drm_file.c
   :doc: file operations

.. kernel-doc:: include/drm/drm_file.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_file.c
   :export:

Misc Utilities(杂项工具)
========================

Printer(打印器)
---------------

.. kernel-doc:: include/drm/drm_print.h
   :doc: print

.. kernel-doc:: include/drm/drm_print.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_print.c
   :export:

Utilities(工具函数)
-------------------

.. kernel-doc:: include/drm/drm_util.h
   :doc: drm utils

.. kernel-doc:: include/drm/drm_util.h
   :internal:


Unit testing(单元测试)
======================

KUnit(内核单元测试框架)
-----------------------

KUnit (Kernel unit testing framework) provides a common framework for unit tests
within the Linux kernel.
(KUnit(内核单元测试框架)为 Linux 内核内的单元测试提供了一个通用框架。)

This section covers the specifics for the DRM subsystem. For general information
about KUnit, please refer to Documentation/dev-tools/kunit/start.rst.
(本节介绍 DRM 子系统的专门内容。关于 KUnit 的一般信息，请参阅
Documentation/dev-tools/kunit/start.rst。)

How to run the tests?(如何运行测试？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to facilitate running the test suite, a configuration file is present
in ``drivers/gpu/drm/tests/.kunitconfig``. It can be used by ``kunit.py`` as
follows:
(为了便于运行测试套件，``drivers/gpu/drm/tests/.kunitconfig`` 中提供了一个
配置文件。``kunit.py`` 可以按如下方式使用它：)

.. code-block:: bash

	$ ./tools/testing/kunit/kunit.py run --kunitconfig=drivers/gpu/drm/tests \
		--kconfig_add CONFIG_VIRTIO_UML=y \
		--kconfig_add CONFIG_UML_PCI_OVER_VIRTIO=y

.. note::
	The configuration included in ``.kunitconfig`` should be as generic as
	possible.
	``CONFIG_VIRTIO_UML`` and ``CONFIG_UML_PCI_OVER_VIRTIO`` are not
	included in it because they are only required for User Mode Linux.
	(``.kunitconfig`` 中包含的配置应当尽可能通用。``CONFIG_VIRTIO_UML`` 与
	``CONFIG_UML_PCI_OVER_VIRTIO`` 未包含在其中，因为只有 User Mode Linux
	才需要它们。)

KUnit Coverage Rules(KUnit 覆盖率规则)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

KUnit support is gradually added to the DRM framework and helpers. There's no
general requirement for the framework and helpers to have KUnit tests at the
moment. However, patches that are affecting a function or helper already
covered by KUnit tests must provide tests if the change calls for one.
(KUnit 支持正逐步添加到 DRM 框架和 helper 中。目前对框架和 helper 拥有 KUnit
测试尚无一般性要求。但是，如果补丁改动所涉及的函数或 helper 已经被 KUnit
测试覆盖，且改动确有需要，则补丁必须提供测试。)

Legacy Support Code(遗留支持代码)
=================================

The section very briefly covers some of the old legacy support code
which is only used by old DRM drivers which have done a so-called
shadow-attach to the underlying device instead of registering as a real
driver. This also includes some of the old generic buffer management and
command submission code. Do not use any of this in new and modern
drivers.
(本节非常简要地介绍一部分旧的遗留支持代码，它们只被那些对底层设备执行所谓
shadow-attach(而不是注册为真正的驱动程序)的旧 DRM 驱动程序使用。其中也包括
一些旧的通用缓冲区管理和命令提交代码。不要在新的现代驱动程序中使用这些
内容。)

Legacy Suspend/Resume(遗留的挂起/恢复)
--------------------------------------

The DRM core provides some suspend/resume code, but drivers wanting full
suspend/resume support should provide save() and restore() functions.
These are called at suspend, hibernate, or resume time, and should
perform any state save or restore required by your device across suspend
or hibernate states.
(DRM 核心提供了一些挂起/恢复代码，但想要完整挂起/恢复支持的驱动程序应提供
save() 与 restore() 函数。它们在挂起、休眠或恢复时被调用，应当执行你的设备
在挂起或休眠状态之间所需的任何状态保存或恢复。)

int (\*suspend) (struct drm_device \*, pm_message_t state); int
(\*resume) (struct drm_device \*);
Those are legacy suspend and resume methods which *only* work with the
legacy shadow-attach driver registration functions. New driver should
use the power management interface provided by their bus type (usually
through the :c:type:`struct device_driver <device_driver>`
dev_pm_ops) and set these methods to NULL.
(这些是遗留的挂起与恢复方法，*只* 适用于遗留的 shadow-attach 驱动注册函数。
新的驱动程序应使用其总线类型提供的电源管理接口(通常通过 :c:type:`struct
device_driver <device_driver>` 的 dev_pm_ops)，并将这些方法设为 NULL。)

Legacy DMA Services(遗留 DMA 服务)
----------------------------------

This should cover how DMA mapping etc. is supported by the core. These
functions are deprecated and should not be used.
(本节应说明 DMA 映射等如何由核心提供支持。这些函数已被弃用，不应使用。)
