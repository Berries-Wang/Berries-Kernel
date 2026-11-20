.. SPDX-License-Identifier: GPL-2.0

=============================
fwctl subsystem(fwctl 子系统)
=============================

:Author: Jason Gunthorpe
(作者： Jason Gunthorpe)

Overview(概述)
==============

Modern devices contain extensive amounts of FW, and in many cases, are largely
software-defined pieces of hardware. The evolution of this approach is largely a
reaction to Moore's Law where a chip tape out is now highly expensive, and the
chip design is extremely large. Replacing fixed HW logic with a flexible and
tightly coupled FW/HW combination is an effective risk mitigation against chip
respin. Problems in the HW design can be counteracted in device FW. This is
especially true for devices which present a stable and backwards compatible
interface to the operating system driver (such as NVMe).
(现代设备包含大量的固件 (FW)，并且在很多情况下基本上是软件定义的硬件。
这种做法的演进很大程度上是对摩尔定律的反应：如今一次芯片流片 (tape out)
的成本极高，而且芯片设计规模巨大。用灵活且紧密耦合的 FW/HW 组合取代固定
的 HW 逻辑，是应对芯片重新流片 (respin) 的有效风险缓解手段。HW 设计中的
问题可以在设备 FW 中加以纠正。对于那些向操作系统驱动呈现稳定且向后兼容
接口的设备 (例如 NVMe) 来说尤其如此。)

The FW layer in devices has grown to incredible size and devices frequently
integrate clusters of fast processors to run it. For example, mlx5 devices have
over 30MB of FW code, and big configurations operate with over 1GB of FW managed
runtime state.
(设备中的 FW 层已经增长到惊人的规模，设备经常集成多个快速处理器集群来
运行它。例如，mlx5 设备拥有超过 30MB 的 FW 代码，大型配置会使用超过 1GB
的由 FW 管理的运行时状态。)

The availability of such a flexible layer has created quite a variety in the
industry where single pieces of silicon are now configurable software-defined
devices and can operate in substantially different ways depending on the need.
Further, we often see cases where specific sites wish to operate devices in ways
that are highly specialized and require applications that have been tailored to
their unique configuration.
(这种灵活层的可用性在业界造成了相当大的多样性：单块硅片如今是可配置的
软件定义设备，可以根据需要以截然不同的方式运行。此外，我们经常看到这样
的情况：特定站点希望以高度专门化的方式操作设备，需要为其独特配置量身
定制的应用程序。)

Further, devices have become multi-functional and integrated to the point they
no longer fit neatly into the kernel's division of subsystems. Modern
multi-functional devices have drivers, such as bnxt/ice/mlx5/pds, that span many
subsystems while sharing the underlying hardware using the auxiliary device
system.
(此外，设备已经变得多功能且高度集成，以至于不再能整齐地归入内核的子系统
划分。现代多功能设备拥有跨多个子系统的驱动，例如 bnxt/ice/mlx5/pds，它们
通过辅助设备 (auxiliary device) 机制共享底层硬件。)

All together this creates a challenge for the operating system, where devices
have an expansive FW environment that needs robust device-specific debugging
support, and FW-driven functionality that is not well suited to “generic”
interfaces. fwctl seeks to allow access to the full device functionality from
user space in the areas of debuggability, management, and first-boot/nth-boot
provisioning.
(这一切都给操作系统带来了挑战：设备拥有庞大的 FW 环境，需要健壮的设备
特定调试支持；同时 FW 驱动的功能并不适合 "通用" 接口。fwctl 力图让用户
空间能够在可调试性、管理以及首次/第 N 次启动 (first-boot/nth-boot) 供应
等领域访问设备的全部功能。)

fwctl is aimed at the common device design pattern where the OS and FW
communicate via an RPC message layer constructed with a queue or mailbox scheme.
In this case the driver will typically have some layer to deliver RPC messages
and collect RPC responses from device FW. The in-kernel subsystem drivers that
operate the device for its primary purposes will use these RPCs to build their
drivers, but devices also usually have a set of ancillary RPCs that don't really
fit into any specific subsystem. For example, a HW RAID controller is primarily
operated by the block layer but also comes with a set of RPCs to administer the
construction of drives within the HW RAID.
(fwctl 针对的是一种常见的设备设计模式：操作系统与 FW 通过以队列或邮箱
(mailbox) 方式构造的 RPC 消息层进行通信。在这种情况下，驱动通常会有某一
层用于向设备 FW 递送 RPC 消息并收集 RPC 响应。为设备的主要用途而操作
设备的内核子系统驱动会使用这些 RPC 来构建自身，但设备通常还有一组不属于
任何特定子系统的辅助 RPC。例如，HW RAID 控制器主要由块层操作，但它还
附带一组用于管理 HW RAID 中驱动器构造的 RPC。)

In the past when devices were more single function, individual subsystems would
grow different approaches to solving some of these common problems. For instance,
monitoring device health, manipulating its FLASH, debugging the FW,
provisioning, all have various unique interfaces across the kernel.
(过去，当设备功能更加单一时，各个子系统会发展出不同的方法来解决这些常见
问题中的某些问题。例如，监控设备健康状况、操作其 FLASH、调试 FW、供应
等，在内核中都各自有各种独特的接口。)

fwctl's purpose is to define a common set of limited rules, described below,
that allow user space to securely construct and execute RPCs inside device FW.
The rules serve as an agreement between the operating system and FW on how to
correctly design the RPC interface. As a uAPI the subsystem provides a thin
layer of discovery and a generic uAPI to deliver the RPCs and collect the
response. It supports a system of user space libraries and tools which will
use this interface to control the device using the device native protocols.
(fwctl 的目的是定义一组通用的、有限的规则 (如下所述)，允许用户空间在
设备 FW 内部安全地构造并执行 RPC。这些规则是操作系统与 FW 之间关于如何
正确设计 RPC 接口的约定。作为一个 uAPI，该子系统提供一层薄薄的发现机制
和一个用于递送 RPC 并收集响应的通用 uAPI。它支持一个由用户空间库与工具
组成的系统，这些库与工具将使用此接口、以设备原生协议来控制设备。)

Scope of Action(作用范围)
-------------------------

fwctl drivers are strictly restricted to being a way to operate the device FW.
It is not an avenue to access random kernel internals, or other operating system
SW states.
(fwctl 驱动被严格限定为一种操作设备 FW 的途径。它不是访问任意内核内部
或其他操作系统软件状态的途径。)

fwctl instances must operate on a well-defined device function, and the device
should have a well-defined security model for what scope within the physical
device the function is permitted to access. For instance, the most complex PCIe
device today may broadly have several function-level scopes:
(fwctl 实例必须在一个定义良好的设备功能 (device function) 上操作，并且
设备应当有一个定义良好的安全模型，规定该功能被允许访问物理设备内的什么
范围。例如，如今最复杂的 PCIe 设备大体可以有若干个功能级别的范围：)

 1. A privileged function with full access to the on-device global state and
    configuration
    (拥有完全访问片上全局状态与配置权限的特权功能)

 2. Multiple hypervisor functions with control over itself and child functions
    used with VMs
    (多个 hypervisor 功能，控制自身及用于虚拟机 (VM) 的子功能)

 3. Multiple VM functions tightly scoped within the VM
    (多个严格限定在 VM 范围内的 VM 功能)

The device may create a logical parent/child relationship between these scopes.
For instance, a child VM's FW may be within the scope of the hypervisor FW. It is
quite common in the VFIO world that the hypervisor environment has a complex
provisioning/profiling/configuration responsibility for the function VFIO
assigns to the VM.
(设备可以在这些范围之间创建逻辑上的父/子关系。例如，子 VM 的 FW 可能
位于 hypervisor FW 的范围之内。在 VFIO 世界中，hypervisor 环境对 VFIO
分配给 VM 的功能承担复杂的供应/画像/配置责任，这是很常见的。)

Further, within the function, devices often have RPC commands that fall within
some general scopes of action (see enum fwctl_rpc_scope):
(此外，在功能内部，设备的 RPC 命令通常落在一些通用作用范围内 (参见
enum fwctl_rpc_scope)：)

 1. Access to function & child configuration, FLASH, etc. that becomes live at a
    function reset. Access to function & child runtime configuration that is
    transparent or non-disruptive to any driver or VM.
    (对功能与子功能配置、FLASH 等的访问，这些在功能复位时生效。对功能
    与子功能运行时配置的访问，对任何驱动或 VM 都是透明的或无干扰的。)

 2. Read-only access to function debug information that may report on FW objects
    in the function & child, including FW objects owned by other kernel
    subsystems.
    (对功能调试信息的只读访问，可能报告功能与子功能中的 FW 对象，包括
    其他内核子系统拥有的 FW 对象。)

 3. Write access to function & child debug information strictly compatible with
    the principles of kernel lockdown and kernel integrity protection. Triggers
    a kernel taint.
    (对功能与子功能调试信息的写访问，严格兼容内核锁定 (kernel lockdown)
    与内核完整性保护的原则。触发内核 taint。)

 4. Full debug device access. Triggers a kernel taint, requires CAP_SYS_RAWIO.
    (完全的调试设备访问。触发内核 taint，需要 CAP_SYS_RAWIO。)

User space will provide a scope label on each RPC and the kernel must enforce the
above CAPs and taints based on that scope. A combination of kernel and FW can
enforce that RPCs are placed in the correct scope by user space.
(用户空间会在每个 RPC 上提供一个范围标签，内核必须基于该范围强制执行
上述 CAP 与 taint。内核与 FW 相结合可以强制用户空间把 RPC 放入正确的
范围。)

Disallowed behavior(不允许的行为)
---------------------------------

There are many things this interface must not allow user space to do (without a
taint or CAP), broadly derived from the principles of kernel lockdown. Some
examples:
(此接口在很多方面不允许用户空间去做 (除非有 taint 或 CAP)，这些大体源自
内核锁定的原则。举例如下：)

 1. DMA to/from arbitrary memory, hang the system, compromise FW integrity with
    untrusted code, or otherwise compromise device or system security and
    integrity.
    (对任意内存进行 DMA、挂起系统、用不受信任的代码危害 FW 完整性，或
    以其他方式危害设备或系统的安全与完整性。)

 2. Provide an abnormal “back door” to kernel drivers. No manipulation of kernel
    objects owned by kernel drivers.
    (为内核驱动提供异常的 "后门"。不得操作内核驱动拥有的内核对象。)

 3. Directly configure or otherwise control kernel drivers. A subsystem kernel
    driver can react to the device configuration at function reset/driver load
    time, but otherwise must not be coupled to fwctl.
    (直接配置或以其他方式控制内核驱动。子系统内核驱动可以在功能复位/
    驱动加载时对设备配置做出反应，但除此之外不得与 fwctl 耦合。)

 4. Operate the HW in a way that overlaps with the core purpose of another
    primary kernel subsystem, such as read/write to LBAs, send/receive of
    network packets, or operate an accelerator's data plane.
    (以与另一个主要内核子系统的核心目的相重叠的方式操作 HW，例如读/写
    LBA、发送/接收网络数据包，或操作加速器的数据平面。)

fwctl is not a replacement for device direct access subsystems like uacce or
VFIO.
(fwctl 不是 uacce 或 VFIO 这类设备直接访问子系统的替代品。)

Operations exposed through fwctl's non-tainting interfaces should be fully
sharable with other users of the device. For instance, exposing a RPC through
fwctl should never prevent a kernel subsystem from also concurrently using that
same RPC or hardware unit down the road. In such cases fwctl will be less
important than proper kernel subsystems that eventually emerge. Mistakes in this
area resulting in clashes will be resolved in favour of a kernel implementation.
(通过 fwctl 非污染接口暴露的操作应当可以与设备的其他用户完全共享。例如，
通过 fwctl 暴露一个 RPC 绝不应阻止内核子系统稍后并发地使用同一 RPC 或
硬件单元。在这种情况下，fwctl 的重要性不如最终涌现的恰当内核子系统。
此领域的错误导致冲突时，将按照内核实现优先的原则解决。)

fwctl User API(fwctl 用户 API)
==============================

.. kernel-doc:: include/uapi/fwctl/bnxt.h
.. kernel-doc:: include/uapi/fwctl/fwctl.h
.. kernel-doc:: include/uapi/fwctl/mlx5.h
.. kernel-doc:: include/uapi/fwctl/pds.h

sysfs Class(sysfs 类)
---------------------

fwctl has a sysfs class (/sys/class/fwctl/fwctlNN/) and character devices
(/dev/fwctl/fwctlNN) with a simple numbered scheme. The character device
operates the iotcl uAPI described above.
(fwctl 拥有一个 sysfs 类 (/sys/class/fwctl/fwctlNN/) 和字符设备
(/dev/fwctl/fwctlNN)，采用简单的编号方案。字符设备操作上述的 ioctl
uAPI。)

fwctl devices can be related to driver components in other subsystems through
sysfs::

    $ ls /sys/class/fwctl/fwctl0/device/infiniband/
    ibp0s10f0

    $ ls /sys/class/infiniband/ibp0s10f0/device/fwctl/
    fwctl0/

    $ ls /sys/devices/pci0000:00/0000:00:0a.0/fwctl/fwctl0
    dev  device  power  subsystem  uevent

(fwctl 设备可以通过 sysfs 与其他子系统中的驱动组件关联起来：)

User space Community(用户空间社区)
----------------------------------

Drawing inspiration from nvme-cli, participating in the kernel side must come
with a user space in a common TBD git tree, at a minimum to usefully operate the
kernel driver. Providing such an implementation is a pre-condition to merging a
kernel driver.
(借鉴 nvme-cli 的经验，参与内核一侧的工作必须伴随一个位于共同 TBD git
树中的用户空间，至少要有能实际操作内核驱动的实现。提供这样的实现是合并
内核驱动的前提条件。)

The goal is to build user space community around some of the shared problems
we all have, and ideally develop some common user space programs with some
starting themes of:
(目标是围绕我们都共有的一些问题建立一个用户空间社区，并理想地开发一些
通用的用户空间程序，起步主题包括：)

 - Device in-field debugging
   (设备现场调试)

 - HW provisioning
   (HW 供应)

 - VFIO child device profiling before VM boot
   (VM 启动前对 VFIO 子设备的画像)

 - Confidential Compute topics (attestation, secure provisioning)
   (机密计算主题 (证明 (attestation)、安全供应))

that stretch across all subsystems in the kernel. fwupd is a great example of
how an excellent user space experience can emerge out of kernel-side diversity.
(它们贯穿内核中的所有子系统。fwupd 是一个很好的例子，展示了优秀的用户
空间体验如何从内核侧的多样性中涌现。)

fwctl Kernel API(fwctl 内核 API)
================================

.. kernel-doc:: drivers/fwctl/main.c
   :export:
.. kernel-doc:: include/linux/fwctl.h

fwctl Driver design(fwctl 驱动设计)
-----------------------------------

In many cases a fwctl driver is going to be part of a larger cross-subsystem
device possibly using the auxiliary_device mechanism. In that case several
subsystems are going to be sharing the same device and FW interface layer so the
device design must already provide for isolation and cooperation between kernel
subsystems. fwctl should fit into that same model.
(在很多情况下，fwctl 驱动将是更大的跨子系统设备的一部分，可能使用
auxiliary_device 机制。在这种情况下，多个子系统将共享同一设备和 FW
接口层，因此设备设计必须已经为内核子系统之间的隔离与协作做好了准备。
fwctl 应当融入同一个模型。)

Part of the driver should include a description of how its scope restrictions
and security model work. The driver and FW together must ensure that RPCs
provided by user space are mapped to the appropriate scope. If the validation is
done in the driver then the validation can read a 'command effects' report from
the device, or hardwire the enforcement. If the validation is done in the FW,
then the driver should pass the fwctl_rpc_scope to the FW along with the command.
(驱动的一部分应当包括对其范围限制与安全模型如何工作的描述。驱动与 FW
必须共同确保用户空间提供的 RPC 被映射到适当的范围。如果验证在驱动中
完成，那么验证可以从设备读取一份 '命令效果' 报告，或者硬编码强制执行。
如果验证在 FW 中完成，那么驱动应当把 fwctl_rpc_scope 随命令一起传给
FW。)

The driver and FW must cooperate to ensure that either fwctl cannot allocate
any FW resources, or any resources it does allocate are freed on FD closure.  A
driver primarily constructed around FW RPCs may find that its core PCI function
and RPC layer belongs under fwctl with auxiliary devices connecting to other
subsystems.
(驱动与 FW 必须协作，确保 fwctl 要么不能分配任何 FW 资源，要么其分配
的任何资源都会在 FD 关闭时释放。主要围绕 FW RPC 构建的驱动可能会发现，
其核心 PCI 功能与 RPC 层应当归入 fwctl，而通过辅助设备连接到其他子系统。)

Each device type must be mindful of Linux's philosophy for stable ABI. The FW
RPC interface does not have to meet a strictly stable ABI, but it does need to
meet an expectation that user space tools that are deployed and in significant
use don't needlessly break. FW upgrade and kernel upgrade should keep widely
deployed tooling working.
(每种设备类型都必须谨记 Linux 的稳定 ABI 理念。FW RPC 接口不必满足
严格稳定的 ABI，但需要满足这样的期望：已部署且大量使用的用户空间工具
不会无谓地损坏。FW 升级与内核升级应保持广泛部署的工具继续工作。)

Development and debugging focused RPCs under more permissive scopes can have
less stability if the tools using them are only run under exceptional
circumstances and not for every day use of the device. Debugging tools may even
require exact version matching as they may require something similar to DWARF
debug information from the FW binary.
(更宽松范围下的开发与调试类 RPC 可以有较低的稳定性，前提是使用它们的
工具只在特殊情况下运行、而不是设备的日常使用。调试工具甚至可能要求精确
的版本匹配，因为它们可能需要类似来自 FW 二进制的 DWARF 调试信息。)

Security Response(安全响应)
===========================

The kernel remains the gatekeeper for this interface. If violations of the
scopes, security or isolation principles are found, we have options to let
devices fix them with a FW update, push a kernel patch to parse and block RPC
commands or push a kernel patch to block entire firmware versions/devices.
(内核仍然是此接口的守门人。如果发现违反范围、安全或隔离原则的行为，我们
有一些选择：让设备通过 FW 更新修复、推送内核补丁来解析并阻止 RPC 命令，
或推送内核补丁来阻止整个固件版本/设备。)

While the kernel can always directly parse and restrict RPCs, it is expected
that the existing kernel pattern of allowing drivers to delegate validation to
FW to be a useful design.
(虽然内核始终可以直接解析并限制 RPC，但预计现有的允许驱动把验证委托给
FW 的内核模式将是一种有用的设计。)

Existing Similar Examples(已有的类似示例)
=========================================

The approach described in this document is not a new idea. Direct, or near
direct device access has been offered by the kernel in different areas for
decades. With more devices wanting to follow this design pattern it is becoming
clear that it is not entirely well understood and, more importantly, the
security considerations are not well defined or agreed upon.
(本文档描述的方法并不是什么新想法。几十年来，内核在不同的领域都提供过
直接或近乎直接的设备访问。随着更多设备希望遵循这种设计模式，人们越来越
清楚地认识到这种模式并未被完全理解，更重要的是，安全方面的考虑也没有
得到很好的定义或认同。)

Some examples:
(一些例子：)

 - HW RAID controllers. This includes RPCs to do things like compose drives into
   a RAID volume, configure RAID parameters, monitor the HW and more.
   (HW RAID 控制器。其中包括用于把驱动器组成 RAID 卷、配置 RAID 参数、
   监控 HW 等的 RPC。)

 - Baseboard managers. RPCs for configuring settings in the device and more.
   (基板管理器 (Baseboard managers)。用于配置设备中设置等的 RPC。)

 - NVMe vendor command capsules. nvme-cli provides access to some monitoring
   functions that different products have defined, but more exist.
   (NVMe 厂商命令 capsule。nvme-cli 提供对不同产品定义的某些监控功能的
   访问，但还有更多。)

 - CXL also has a NVMe-like vendor command system.
   (CXL 也有类似 NVMe 的厂商命令系统。)

 - DRM allows user space drivers to send commands to the device via kernel
   mediation.
   (DRM 允许用户空间驱动通过内核中介向设备发送命令。)

 - RDMA allows user space drivers to directly push commands to the device
   without kernel involvement.
   (RDMA 允许用户空间驱动在内核不参与的情况下直接向设备推送命令。)

 - Various “raw” APIs, raw HID (SDL2), raw USB, NVMe Generic Interface, etc.
   (各种 "raw" API，raw HID (SDL2)、raw USB、NVMe 通用接口等。)

The first 4 are examples of areas that fwctl intends to cover. The latter three
are examples of disallowed behavior as they fully overlap with the primary purpose
of a kernel subsystem.
(前 4 个是 fwctl 打算覆盖的领域的例子。后三个是不允许行为的例子，因为
它们与内核子系统的首要目的完全重叠。)

Some key lessons learned from these past efforts are the importance of having a
common user space project to use as a pre-condition for obtaining a kernel
driver. Developing good community around useful software in user space is key to
getting companies to fund participation to enable their products.
(从这些过去的努力中得出的一些关键教训是：拥有一个共同的用户空间项目
作为获得内核驱动的前提条件非常重要。围绕有用的用户空间软件培育良好的
社区，是让公司出资参与、使其产品得以启用的关键。)
