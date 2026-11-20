.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

===================
Task List(任务清单)
===================

Tasks may have the following fields:
(任务可能带有以下字段：)

- ``Complexity``: Describes the required familiarity with Rust and / or the
  corresponding kernel APIs or subsystems. There are four different complexities,
  ``Beginner``, ``Intermediate``, ``Advanced`` and ``Expert``.
  (``Complexity``：描述对 Rust 以及/或相应内核 API 或子系统所需的熟悉程度。
  共有四种复杂度级别：``Beginner``、``Intermediate``、``Advanced`` 和
  ``Expert``。)
- ``Reference``: References to other tasks.
  (``Reference``：对其他任务的引用。)
- ``Link``: Links to external resources.
  (``Link``：指向外部资源的链接。)
- ``Contact``: The person that can be contacted for further information about
  the task.
  (``Contact``：可就任务的进一步信息进行联系的人。)

A task might have `[ABCD]` code after its name. This code can be used to grep
into the code for `TODO` entries related to it.
(任务名称后面可能带有 `` `[ABCD]` `` 代码。可以用它在代码中 grep 与该任务
相关的 `TODO` 条目。)

Enablement (Rust)(Rust 使能类任务)
==================================

Tasks that are not directly related to nova-core, but are preconditions in terms
of required APIs.
(与 nova-core 没有直接关系、但属于所需 API 前置条件的任务。)

FromPrimitive API [FPRI](FromPrimitive API [FPRI])
--------------------------------------------------

Sometimes the need arises to convert a number to a value of an enum or a
structure.
(有时需要把一个数字转换为某个枚举或结构的值。)

A good example from nova-core would be the ``Chipset`` enum type, which defines
the value ``AD102``. When probing the GPU the value ``0x192`` can be read from a
certain register indication the chipset AD102. Hence, the enum value ``AD102``
should be derived from the number ``0x192``. Currently, nova-core uses a custom
implementation (``Chipset::from_u32`` for this.
(nova-core 的一个好例子是 ``Chipset`` 枚举类型，它定义了值 ``AD102``。探测
GPU 时可以从某个寄存器读到值 ``0x192``，表明芯片型号是 AD102。因此，枚举值
``AD102`` 应当能够从数字 ``0x192`` 推导出来。目前，nova-core 为此使用了一个
自定义实现 (``Chipset::from_u32``。)

Instead, it would be desirable to have something like the ``FromPrimitive``
trait [1] from the num crate.
(更好的做法是拥有类似 num crate 中 ``FromPrimitive`` trait [1] 这样的东西。)

Having this generalization also helps with implementing a generic macro that
automatically generates the corresponding mappings between a value and a number.
(拥有这种泛化还有助于实现一个通用宏，自动生成值与数字之间的对应映射。)

FromPrimitive support has been worked on in the past, but hasn't been followed
since then [1].
(FromPrimitive 支持过去曾经有人做过，但此后就没有再跟进 [1]。)

There also have been considerations of ToPrimitive [2].
(也考虑过 ToPrimitive [2]。)

| Complexity: Beginner
| Link: https://docs.rs/num/latest/num/trait.FromPrimitive.html
| Link: https://lore.kernel.org/all/cover.1750689857.git.y.j3ms.n@gmail.com/ [1]
| Link: https://rust-for-linux.zulipchat.com/#narrow/channel/288089-General/topic/Implement.20.60FromPrimitive.60.20trait.20.2B.20derive.20macro.20for.20nova-core/with/541971854 [2]

Numerical operations [NUMM](数值运算 [NUMM])
--------------------------------------------

Nova uses integer operations that are not part of the standard library (or not
implemented in an optimized way for the kernel). These include:
(Nova 使用一些不属于标准库的整数运算 (或者内核中没有做优化实现的运算)。
这些包括：)

- The "Find Last Set Bit" (`fls` function of the C part of the kernel)
  operation.
  ("Find Last Set Bit" 运算 (即内核 C 部分的 `fls` 函数)。)

A `num` core kernel module is being designed to provide these operations.
(目前正在设计一个 `num` 核心内核模块来提供这些运算。)

| Complexity: Intermediate
| Contact: Alexandre Courbot

Page abstraction for foreign pages(foreign pages 的页抽象)
-----------------------------------------------------------

Rust abstractions for pages not created by the Rust page abstraction without
direct ownership.
(为不由 Rust 页抽象创建、且没有直接所有权的页提供的 Rust 抽象。)

There is active onging work from Abdiel Janulgue [1] and Lina [2].
(Abdiel Janulgue [1] 和 Lina [2] 正在积极进行相关工作。)

| Complexity: Advanced
| Link: https://lore.kernel.org/linux-mm/20241119112408.779243-1-abdiel.janulgue@gmail.com/ [1]
| Link: https://lore.kernel.org/rust-for-linux/20250202-rust-page-v1-0-e3170d7fe55e@asahilina.net/ [2]

PCI MISC APIs(PCI 杂项 API)
---------------------------

Extend the existing PCI device / driver abstractions by SR-IOV, capability, MSI
API abstractions.
(通过 SR-IOV、capability、MSI API 抽象来扩展现有的 PCI 设备 / 驱动抽象。)

SR-IOV [1] is work in progress.
(SR-IOV [1] 的工作正在进行中。)

| Complexity: Beginner
| Link: https://lore.kernel.org/all/20251119-rust-pci-sriov-v1-0-883a94599a97@redhat.com/ [1]

GPU (general)(GPU 通用)
=======================

Initial Devinit support(初始 Devinit 支持)
------------------------------------------

Implement BIOS Device Initialization, i.e. memory sizing, waiting, PLL
configuration.
(实现 BIOS 设备初始化，即内存大小探测、等待与 PLL 配置。)

| Contact: Dave Airlie
| Complexity: Beginner

MMU / PT management(MMU / 页表管理)
-----------------------------------

Work out the architecture for MMU / page table management.
(确定 MMU / 页表管理的架构。)

We need to consider that nova-drm will need rather fine-grained control,
especially in terms of locking, in order to be able to implement asynchronous
Vulkan queues.
(需要考虑 nova-drm 将需要相当细粒度的控制，尤其是在锁方面，以便能够实现
异步 Vulkan 队列。)

While generally sharing the corresponding code is desirable, it needs to be
evaluated how (and if at all) sharing the corresponding code is expedient.
(虽然通常来说共享相应代码是可取的，但仍需评估如何共享 (以及是否值得共享)
相应代码才是明智之举。)

| Complexity: Expert

VRAM memory allocator(VRAM 内存分配器)
--------------------------------------

Investigate options for a VRAM memory allocator.
(调研 VRAM 内存分配器的可选方案。)

Some possible options:
(一些可能的选项：)

  - Rust abstractions for
    (为以下对象提供 Rust 抽象：)

    - RB tree (interval tree) / drm_mm
      (红黑树 (区间树) / drm_mm)
    - maple_tree
  - native Rust collections
    (Rust 原生集合)

There is work in progress for using drm_buddy [1].
(使用 drm_buddy [1] 的工作正在进行中。)

| Complexity: Advanced
| Link: https://lore.kernel.org/all/20251219203805.1246586-4-joelagnelf@nvidia.com/ [1]

Instance Memory(实例内存)
-------------------------

Implement support for instmem (bar2) used to store page tables.
(实现用于存储页表的 instmem (bar2) 支持。)

| Complexity: Intermediate
| Contact: Dave Airlie

GPU System Processor (GSP)(GPU 系统处理器)
==========================================

Export GSP log buffers(导出 GSP 日志缓冲区)
-------------------------------------------

Recent patches from Timur Tabi [1] added support to expose GSP-RM log buffers
(even after failure to probe the driver) through debugfs.
(Timur Tabi [1] 最近的补丁增加了通过 debugfs 暴露 GSP-RM 日志缓冲区的支持
(即使在驱动探测失败之后也能如此)。)

This is also an interesting feature for nova-core, especially in the early days.
(这对 nova-core 来说也是一个有意思的特性，尤其是在早期阶段。)

| Link: https://lore.kernel.org/nouveau/20241030202952.694055-2-ttabi@nvidia.com/ [1]
| Reference: Debugfs abstractions
| Complexity: Intermediate

GSP firmware abstraction(GSP 固件抽象)
--------------------------------------

The GSP-RM firmware API is unstable and may incompatibly change from version to
version, in terms of data structures and semantics.
(GSP-RM 固件 API 不稳定，在数据结构和语义方面可能随版本更迭而发生不兼容
的变化。)

This problem is one of the big motivations for using Rust for nova-core, since
it turns out that Rust's procedural macro feature provides a rather elegant way
to address this issue:
(这个问题是 nova-core 选用 Rust 的重要动机之一，因为事实证明 Rust 的过程宏
(procedural macro) 特性为解决该问题提供了一种相当优雅的方式：)

1. generate Rust structures from the C headers in a separate namespace per version
   (从 C 头文件生成 Rust 结构体，每个版本一个独立命名空间。)
2. build abstraction structures (within a generic namespace) that implement the
   firmware interfaces; annotate the differences in implementation with version
   identifiers
   (构建实现固件接口的抽象结构体 (位于一个通用命名空间内)；用版本标识符标注
   实现上的差异。)
3. use a procedural macro to generate the actual per version implementation out
   of this abstraction
   (用过程宏从这个抽象生成实际的按版本实现。)
4. instantiate the correct version type one on runtime (can be sure that all
   have the same interface because it's defined by a common trait)
   (在运行时实例化正确版本的类型 (可以确信它们都具有相同的接口，因为接口由
   一个公共 trait 定义)。)

There is a PoC implementation of this pattern, in the context of the nova-core
PoC driver.
(在 nova-core PoC 驱动的上下文中，已有这种模式的一个 PoC 实现。)

This task aims at refining the feature and ideally generalize it, to be usable
by other drivers as well.
(本任务旨在完善该特性，并在理想情况下将其泛化，使之也能被其他驱动使用。)

| Complexity: Expert

GSP message queue(GSP 消息队列)
-------------------------------

Implement low level GSP message queue (command, status) for communication
between the kernel driver and GSP.
(实现内核驱动与 GSP 之间通信所需的底层 GSP 消息队列 (命令、状态)。)

| Complexity: Advanced
| Contact: Dave Airlie

Bootstrap GSP(GSP 引导启动)
---------------------------

Call the boot firmware to boot the GSP processor; execute initial control
messages.
(调用引导固件来启动 GSP 处理器；执行初始控制消息。)

| Complexity: Intermediate
| Contact: Dave Airlie

Client / Device APIs(客户端 / 设备 API)
---------------------------------------

Implement the GSP message interface for client / device allocation and the
corresponding client and device allocation APIs.
(实现用于客户端 / 设备分配的 GSP 消息接口，以及相应的客户端与设备分配
API。)

| Complexity: Intermediate
| Contact: Dave Airlie

Bar PDE handling(BAR PDE 处理)
------------------------------

Synchronize page table handling for BARs between the kernel driver and GSP.
(在内核驱动与 GSP 之间同步 BAR 的页表处理。)

| Complexity: Beginner
| Contact: Dave Airlie

FIFO engine(FIFO 引擎)
----------------------

Implement support for the FIFO engine, i.e. the corresponding GSP message
interface and provide an API for chid allocation and channel handling.
(实现 FIFO 引擎支持，即相应的 GSP 消息接口，并提供用于 chid 分配与通道
处理的 API。)

| Complexity: Advanced
| Contact: Dave Airlie

GR engine(GR 引擎)
------------------

Implement support for the graphics engine, i.e. the corresponding GSP message
interface and provide an API for (golden) context creation and promotion.
(实现图形引擎支持，即相应的 GSP 消息接口，并提供用于 (golden) 上下文创建
与提升的 API。)

| Complexity: Advanced
| Contact: Dave Airlie

CE engine(CE 引擎)
------------------

Implement support for the copy engine, i.e. the corresponding GSP message
interface.
(实现拷贝引擎支持，即相应的 GSP 消息接口。)

| Complexity: Intermediate
| Contact: Dave Airlie

VFN IRQ controller(VFN 中断控制器)
----------------------------------

Support for the VFN interrupt controller.
(支持 VFN 中断控制器。)

| Complexity: Intermediate
| Contact: Dave Airlie

External APIs(外部 API)
=======================

nova-core base API(nova-core 基础 API)
--------------------------------------

Work out the common pieces of the API to connect 2nd level drivers, i.e. vGPU
manager and nova-drm.
(确定连接第二层驱动 (即 vGPU 管理器与 nova-drm) 所需 API 的公共部分。)

| Complexity: Advanced

vGPU manager API(vGPU 管理器 API)
---------------------------------

Work out the API parts required by the vGPU manager, which are not covered by
the base API.
(确定 vGPU 管理器所需而基础 API 未覆盖的 API 部分。)

| Complexity: Advanced

nova-core C API(nova-core 的 C API)
-----------------------------------

Implement a C wrapper for the APIs required by the vGPU manager driver.
(为 vGPU 管理器驱动所需的 API 实现 C 封装。)

| Complexity: Intermediate

Testing(测试)
=============

CI pipeline(CI 流水线)
----------------------

Investigate option for continuous integration testing.
(调研持续集成测试的可选方案。)

This can go from as simple as running KUnit tests over running (graphics) CTS to
booting up (multiple) guest VMs to test VFIO use-cases.
(方案可以简单到只是运行 KUnit 测试，也可以是运行 (图形) CTS，直至启动
(多个) 客户虚拟机来测试 VFIO 用例。)

It might also be worth to consider the introduction of a new test suite directly
sitting on top of the uAPI for more targeted testing and debugging. There may be
options for collaboration / shared code with the Mesa project.
(也可以考虑引入直接构建在 uAPI 之上的新测试套件，以进行更有针对性的测试
与调试。与 Mesa 项目之间可能存在协作 / 共享代码的机会。)

| Complexity: Advanced
