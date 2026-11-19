.. Copyright 2020 DisplayLink (UK) Ltd.

===============================
Userland interfaces(用户态接口)
===============================

The DRM core exports several interfaces to applications, generally
intended to be used through corresponding libdrm wrapper functions. In
addition, drivers export device-specific interfaces for use by userspace
drivers & device-aware applications through ioctls and sysfs files.
(DRM 核心向应用导出若干接口，通常预期通过相应的 libdrm 封装函数使用它们。
此外，驱动还通过 ioctl 和 sysfs 文件导出设备特定的接口，供用户态驱动以及
具有设备感知能力的应用使用。)

External interfaces include: memory mapping, context management, DMA
operations, AGP management, vblank control, fence management, memory
management, and output management.
(外部接口包括：内存映射、上下文管理、DMA 操作、AGP 管理、vblank 控制、
fence 管理、内存管理以及输出管理。)

Cover generic ioctls and sysfs layout here. We only need high-level
info, since man pages should cover the rest.
(本文只覆盖通用 ioctl 与 sysfs 布局。我们只需要高层信息，其余内容应由
man 手册页覆盖。)

libdrm Device Lookup(libdrm 设备查找)
=====================================

.. kernel-doc:: drivers/gpu/drm/drm_ioctl.c
   :doc: getunique and setversion story


.. _drm_primary_node:

Primary Nodes, DRM Master and Authentication(主节点、DRM Master 与认证)
=======================================================================

.. kernel-doc:: drivers/gpu/drm/drm_auth.c
   :doc: master and authentication

.. kernel-doc:: drivers/gpu/drm/drm_auth.c
   :export:

.. kernel-doc:: include/drm/drm_auth.h
   :internal:


.. _drm_leasing:

DRM Display Resource Leasing(DRM 显示资源租赁)
==============================================

.. kernel-doc:: drivers/gpu/drm/drm_lease.c
   :doc: drm leasing

Open-Source Userspace Requirements(开源用户态的要求)
====================================================

The DRM subsystem has stricter requirements than most other kernel subsystems on
what the userspace side for new uAPI needs to look like. This section here
explains what exactly those requirements are, and why they exist.
(与大多数其他内核子系统相比，DRM 子系统对新 uAPI 的用户态部分有更严格的要求。
本节将说明这些要求具体是什么，以及它们为何存在。)

The short summary is that any addition of DRM uAPI requires corresponding
open-sourced userspace patches, and those patches must be reviewed and ready for
merging into a suitable and canonical upstream project.
(简而言之，任何 DRM uAPI 的新增都必须附带相应的开源用户态补丁，且这些补丁必须
经过评审，并已准备好合并进某个合适的、权威的上游项目。)

GFX devices (both display and render/GPU side) are really complex bits of
hardware, with userspace and kernel by necessity having to work together really
closely.  The interfaces, for rendering and modesetting, must be extremely wide
and flexible, and therefore it is almost always impossible to precisely define
them for every possible corner case. This in turn makes it really practically
infeasible to differentiate between behaviour that's required by userspace, and
which must not be changed to avoid regressions, and behaviour which is only an
accidental artifact of the current implementation.
(GFX 设备（包括显示与渲染/GPU 两方面）是极为复杂的硬件，用户态与内核必须非常
紧密地协同工作。用于渲染和模式设置的接口必须极其宽泛而灵活，因此几乎不可能针对
每一种可能的边角情形都给出精确定义。这进而使得要区分"用户态所依赖、不得更改以
免回归的行为"与"仅仅是当前实现偶然产物"的行为，在实践中几乎不可行。)

Without access to the full source code of all userspace users that means it
becomes impossible to change the implementation details, since userspace could
depend upon the accidental behaviour of the current implementation in minute
details. And debugging such regressions without access to source code is pretty
much impossible. As a consequence this means:
(如果无法访问所有用户态使用者的完整源代码，就意味着无法再更改实现细节，因为用户
态可能依赖于当前实现在细微之处的偶然行为；而且在没有源代码的情况下，调试这类回归
几乎是不可能的。由此得出的结论是：)

- The Linux kernel's "no regression" policy holds in practice only for
  open-source userspace of the DRM subsystem. DRM developers are perfectly fine
  if closed-source blob drivers in userspace use the same uAPI as the open
  drivers, but they must do so in the exact same way as the open drivers.
  Creative (ab)use of the interfaces will, and in the past routinely has, lead
  to breakage.
  (Linux 内核的"无回归"政策在实践中只对 DRM 子系统的开源用户态成立。DRM 开发者
  完全不介意闭源的 blob 用户态驱动与开源驱动使用相同的 uAPI，但它们必须以与开源
  驱动完全相同的方式使用。对接口别出心裁的（滥）用过去就经常导致故障，将来也会
  如此。)

- Any new userspace interface must have an open-source implementation as
  demonstration vehicle.
  (任何新的用户态接口都必须有一个开源实现作为示范载体。)

The other reason for requiring open-source userspace is uAPI review. Since the
kernel and userspace parts of a GFX stack must work together so closely, code
review can only assess whether a new interface achieves its goals by looking at
both sides. Making sure that the interface indeed covers the use-case fully
leads to a few additional requirements:
(要求开源用户态的另一个原因是 uAPI 评审。由于 GFX 栈的内核部分与用户态部分必须
紧密协作，只有同时审视两侧代码，代码评审才能判断新接口是否达成其目标。要确保接口
确实完整覆盖用例，又会引出以下几项额外要求：)

- The open-source userspace must not be a toy/test application, but the real
  thing. Specifically it needs to handle all the usual error and corner cases.
  These are often the places where new uAPI falls apart and hence essential to
  assess the fitness of a proposed interface.
  (开源用户态不能是玩具/测试应用，而必须是真正可用的东西。具体来说，它需要处理
  所有常见的错误与边角情形。这些往往正是新 uAPI 容易崩坏的地方，因而对于评估一个
  提议中的接口是否合格至关重要。)

- The userspace side must be fully reviewed and tested to the standards of that
  userspace project. For e.g. mesa this means piglit testcases and review on the
  mailing list. This is again to ensure that the new interface actually gets the
  job done.  The userspace-side reviewer should also provide an Acked-by on the
  kernel uAPI patch indicating that they believe the proposed uAPI is sound and
  sufficiently documented and validated for userspace's consumption.
  (用户态一侧必须按该用户态项目的标准完成充分的评审与测试。例如对 mesa 而言，
  这意味着 piglit 测试用例和邮件列表评审。这同样是为了确保新接口能真正完成工作。
  用户态侧的评审者还应在内核 uAPI 补丁上给出 Acked-by，表明其认为所提议的 uAPI
  是健全的，并且已为用户态的使用提供了充分的文档与验证。)

- The userspace patches must be against the canonical upstream, not some vendor
  fork. This is to make sure that no one cheats on the review and testing
  requirements by doing a quick fork.
  (用户态补丁必须针对权威的上游项目，而不是某个厂商分支。这是为了防止有人通过
  快速分叉来逃避评审与测试要求。)

- The kernel patch can only be merged after all the above requirements are met,
  but it **must** be merged to either drm-next or drm-misc-next **before** the
  userspace patches land. uAPI always flows from the kernel, doing things the
  other way round risks divergence of the uAPI definitions and header files.
  (只有在上述全部要求都满足之后，内核补丁才可以被合并，但它 **必须** 在用户态
  补丁合入之前先合并到 drm-next 或 drm-misc-next。uAPI 总是从内核侧流向用户态，
  反其道而行会导致 uAPI 定义与头文件出现分歧。)

These are fairly steep requirements, but have grown out from years of shared
pain and experience with uAPI added hastily, and almost always regretted about
just as fast. GFX devices change really fast, requiring a paradigm shift and
entire new set of uAPI interfaces every few years at least. Together with the
Linux kernel's guarantee to keep existing userspace running for 10+ years this
is already rather painful for the DRM subsystem, with multiple different uAPIs
for the same thing co-existing. If we add a few more complete mistakes into the
mix every year it would be entirely unmanageable.
(这些要求相当苛刻，但它们是多年共同痛苦与经验的结晶——仓促加入的 uAPI 几乎总是
在同样短的时间内招致悔恨。GFX 设备迭代极快，至少每隔几年就需要一次范式转变和一整
套全新的 uAPI 接口。再叠加 Linux 内核保证让既有用户态持续运行 10 年以上的承诺，
对 DRM 子系统来说，同一功能的多套 uAPI 并存已经相当痛苦。如果我们每年再掺入几个
彻底的错误，事情将完全无法收拾。)

.. _drm_render_node:

Render nodes(渲染节点)
======================

DRM core provides multiple character-devices for user-space to use.
Depending on which device is opened, user-space can perform a different
set of operations (mainly ioctls). The primary node is always created
and called card<num>. Additionally, a currently unused control node,
called controlD<num> is also created. The primary node provides all
legacy operations and historically was the only interface used by
userspace. With KMS, the control node was introduced. However, the
planned KMS control interface has never been written and so the control
node stays unused to date.
(DRM 核心为用户态提供了多个字符设备。取决于打开的是哪个设备，用户态可以执行
不同的操作集合（主要是 ioctl）。主节点总是会被创建，命名为 card<num>。此外，
还会创建一个目前未被使用的控制节点，名为 controlD<num>。主节点提供所有遗留
操作，并且历史上一直是用户态使用的唯一接口。随着 KMS 的引入，控制节点也应运
而生。然而，计划中的 KMS 控制接口从未被编写出来，因此控制节点至今未被使用。)

With the increased use of offscreen renderers and GPGPU applications,
clients no longer require running compositors or graphics servers to
make use of a GPU. But the DRM API required unprivileged clients to
authenticate to a DRM-Master prior to getting GPU access. To avoid this
step and to grant clients GPU access without authenticating, render
nodes were introduced. Render nodes solely serve render clients, that
is, no modesetting or privileged ioctls can be issued on render nodes.
Only non-global rendering commands are allowed. If a driver supports
render nodes, it must advertise it via the DRIVER_RENDER DRM driver
capability. If not supported, the primary node must be used for render
clients together with the legacy drmAuth authentication procedure.
(随着离屏渲染器与 GPGPU 应用的增多，客户端不再需要运行中的合成器或图形服务器
也能使用 GPU。但 DRM API 要求非特权客户端先向 DRM-Master 认证才能获得 GPU
访问权。为了避免这一步骤、让客户端无需认证即可获得 GPU 访问权，渲染节点应运
而生。渲染节点只服务于渲染客户端，也就是说，在渲染节点上不能发起模式设置或
特权 ioctl，只允许非全局的渲染命令。如果驱动支持渲染节点，就必须通过
DRIVER_RENDER 这一 DRM 驱动能力予以声明。若不支持，渲染客户端就必须使用主
节点，并配合遗留的 drmAuth 认证流程。)

If a driver advertises render node support, DRM core will create a
separate render node called renderD<num>. There will be one render node
per device. No ioctls except PRIME-related ioctls will be allowed on
this node. Especially GEM_OPEN will be explicitly prohibited. For a
complete list of driver-independent ioctls that can be used on render
nodes, see the ioctls marked DRM_RENDER_ALLOW in drm_ioctl.c  Render
nodes are designed to avoid the buffer-leaks, which occur if clients
guess the flink names or mmap offsets on the legacy interface.
Additionally to this basic interface, drivers must mark their
driver-dependent render-only ioctls as DRM_RENDER_ALLOW so render
clients can use them. Driver authors must be careful not to allow any
privileged ioctls on render nodes.
(如果驱动声明支持渲染节点，DRM 核心会创建一个名为 renderD<num> 的独立渲染
节点。每个设备有一个渲染节点。除与 PRIME 相关的 ioctl 外，不允许在该节点上
使用任何 ioctl，尤其是 GEM_OPEN 会被明确禁止。关于可在渲染节点上使用的与
驱动无关的 ioctl 完整列表，参见 drm_ioctl.c 中标注了 DRM_RENDER_ALLOW 的
那些 ioctl。渲染节点的设计目的是避免缓冲区泄漏——如果客户端在遗留接口上猜测
flink 名称或 mmap 偏移，就会发生这类泄漏。除这一基础接口之外，驱动还必须将其
自身依赖于驱动的仅渲染 ioctl 标记为 DRM_RENDER_ALLOW，以便渲染客户端可以
使用它们。驱动作者必须小心，不得允许任何特权 ioctl 出现在渲染节点上。)

With render nodes, user-space can now control access to the render node
via basic file-system access-modes. A running graphics server which
authenticates clients on the privileged primary/legacy node is no longer
required. Instead, a client can open the render node and is immediately
granted GPU access. Communication between clients (or servers) is done
via PRIME. FLINK from render node to legacy node is not supported. New
clients must not use the insecure FLINK interface.
(有了渲染节点，用户态现在可以通过基本的文件系统访问权限来控制对渲染节点的
访问。不再需要由运行中的图形服务器在特权的主节点/遗留节点上为客户端认证。
相反，客户端可以直接打开渲染节点并立即获得 GPU 访问权。客户端（或服务器）
之间的通信通过 PRIME 完成。不支持从渲染节点到遗留节点的 FLINK。新客户端
不得使用不安全的 FLINK 接口。)

Besides dropping all modeset/global ioctls, render nodes also drop the
DRM-Master concept. There is no reason to associate render clients with
a DRM-Master as they are independent of any graphics server. Besides,
they must work without any running master, anyway. Drivers must be able
to run without a master object if they support render nodes. If, on the
other hand, a driver requires shared state between clients which is
visible to user-space and accessible beyond open-file boundaries, they
cannot support render nodes.
(除了去掉所有模式设置/全局 ioctl 之外，渲染节点还去掉了 DRM-Master 概念。
没有理由把渲染客户端与某个 DRM-Master 关联起来，因为它们独立于任何图形
服务器；何况它们本来就必须能在没有任何运行中 master 的情况下工作。如果驱动
支持渲染节点，就必须能够在没有 master 对象的情况下运行。反过来说，如果驱动
需要在客户端之间共享某种对用户态可见、且能跨越 open 文件边界访问的状态，
它就无法支持渲染节点。)

Device Hot-Unplug(设备热拔出)
=============================

.. note::
   The following is the plan. Implementation is not there yet
   (2020 May).
   (以下是规划；截至 2020 年 5 月，实现尚未完成。)

Graphics devices (display and/or render) may be connected via USB (e.g.
display adapters or docking stations) or Thunderbolt (e.g. eGPU). An end
user is able to hot-unplug this kind of devices while they are being
used, and expects that the very least the machine does not crash. Any
damage from hot-unplugging a DRM device needs to be limited as much as
possible and userspace must be given the chance to handle it if it wants
to. Ideally, unplugging a DRM device still lets a desktop continue to
run, but that is going to need explicit support throughout the whole
graphics stack: from kernel and userspace drivers, through display
servers, via window system protocols, and in applications and libraries.
(图形设备（显示和/或渲染）可以通过 USB（例如显示适配器或扩展坞）或
Thunderbolt（例如 eGPU）连接。最终用户能够在这类设备使用过程中将其热拔出，
并期望至少机器不会崩溃。热拔出 DRM 设备造成的任何损害都必须尽可能加以限制，
并且如果用户态愿意处理，就必须给它处理的机会。理想情况下，拔出一个 DRM 设备
之后桌面仍能继续运行，但这需要整个图形栈的显式支持：从内核与用户态驱动，到
显示服务器，经由窗口系统协议，再到应用与库。)

Other scenarios that should lead to the same are: unrecoverable GPU
crash, PCI device disappearing off the bus, or forced unbind of a driver
from the physical device.
(应当导向同样结果的其他场景还包括：不可恢复的 GPU 崩溃、PCI 设备从总线上
消失，或者驱动从物理设备上被强制解绑。)

In other words, from userspace perspective everything needs to keep on
working more or less, until userspace stops using the disappeared DRM
device and closes it completely. Userspace will learn of the device
disappearance from the device removed uevent, ioctls returning ENODEV
(or driver-specific ioctls returning driver-specific things), or open()
returning ENXIO.
(换句话说，从用户态的视角来看，一切都要或多或少继续正常运转，直到用户态停止
使用这个已消失的 DRM 设备并将其彻底关闭为止。用户态将通过设备移除 uevent、
ioctl 返回 ENODEV（或驱动特定 ioctl 返回驱动特定的值），或者 open() 返回
ENXIO，得知设备已消失。)

Only after userspace has closed all relevant DRM device and dmabuf file
descriptors and removed all mmaps, the DRM driver can tear down its
instance for the device that no longer exists. If the same physical
device somehow comes back in the mean time, it shall be a new DRM
device.
(只有当用户态关闭了所有相关的 DRM 设备与 dmabuf 文件描述符、并移除了所有
mmap 之后，DRM 驱动才可以拆除它为这个已不存在的设备所建立的实例。如果同一
物理设备在此期间又回来了，它将被视为一个新的 DRM 设备。)

Similar to PIDs, chardev minor numbers are not recycled immediately. A
new DRM device always picks the next free minor number compared to the
previous one allocated, and wraps around when minor numbers are
exhausted.
(与 PID 类似，字符设备次设备号不会立即被回收使用。新的 DRM 设备总是选取比
上一个已分配的次设备号更大的下一个空闲号码，并在号码耗尽时回绕。)

The goal raises at least the following requirements for the kernel and
drivers.
(这一目标至少对内核和驱动提出了以下要求。)

Requirements for KMS UAPI(KMS UAPI 的要求)
------------------------------------------

- KMS connectors must change their status to disconnected.
  (KMS 连接器必须将其状态改为 disconnected。)

- Legacy modesets and pageflips, and atomic commits, both real and
  TEST_ONLY, and any other ioctls either fail with ENODEV or fake
  success.
  (遗留模式设置与页翻转、原子提交（包括真实提交与 TEST_ONLY），以及其他任何
  ioctl，要么以 ENODEV 失败，要么伪造成功。)

- Pending non-blocking KMS operations deliver the DRM events userspace
  is expecting. This applies also to ioctls that faked success.
  (未完成的非阻塞 KMS 操作必须投递用户态所期待的 DRM 事件。这也适用于伪造
  成功的 ioctl。)

- open() on a device node whose underlying device has disappeared will
  fail with ENXIO.
  (对底层设备已消失的设备节点执行 open() 将以 ENXIO 失败。)

- Attempting to create a DRM lease on a disappeared DRM device will
  fail with ENODEV. Existing DRM leases remain and work as listed
  above.
  (在已消失的 DRM 设备上尝试创建 DRM lease 将以 ENODEV 失败。已存在的
  DRM lease 仍保留并按上文所述继续工作。)

Requirements for Render and Cross-Device UAPI(渲染与跨设备 UAPI 的要求)
-----------------------------------------------------------------------

- All GPU jobs that can no longer run must have their fences
  force-signalled to avoid inflicting hangs on userspace.
  The associated error code is ENODEV.
  (所有无法再运行的 GPU 作业，其 fence 必须被强制置为已发出信号的状态，以免
  给用户态造成挂起。相关的错误码是 ENODEV。)

- Some userspace APIs already define what should happen when the device
  disappears (OpenGL, GL ES: `GL_KHR_robustness`_; `Vulkan`_:
  VK_ERROR_DEVICE_LOST; etc.). DRM drivers are free to implement this
  behaviour the way they see best, e.g. returning failures in
  driver-specific ioctls and handling those in userspace drivers, or
  rely on uevents, and so on.
  (某些用户态 API 已经定义了设备消失时应当发生什么（OpenGL、GL ES 见
  GL_KHR_robustness；Vulkan 见 VK_ERROR_DEVICE_LOST；等等）。DRM 驱动可以
  自行选择以最佳方式实现这一行为，例如在驱动特定的 ioctl 中返回失败并在
  用户态驱动中处理，或者依赖 uevent，等等。)

- dmabuf which point to memory that has disappeared will either fail to
  import with ENODEV or continue to be successfully imported if it would
  have succeeded before the disappearance. See also about memory maps
  below for already imported dmabufs.
  (指向已经消失的内存的 dmabuf，要么以 ENODEV 导入失败，要么如果在消失前
  本来会导入成功则继续成功导入。已导入 dmabuf 的内存映射问题另见下文。)

- Attempting to import a dmabuf to a disappeared device will either fail
  with ENODEV or succeed if it would have succeeded without the
  disappearance.
  (尝试向已消失的设备导入 dmabuf，要么以 ENODEV 失败，要么如果在不发生消失
  的情况下本会成功则成功。)

- open() on a device node whose underlying device has disappeared will
  fail with ENXIO.
  (对底层设备已消失的设备节点执行 open() 将以 ENXIO 失败。)

.. _GL_KHR_robustness: https://www.khronos.org/registry/OpenGL/extensions/KHR/KHR_robustness.txt
.. _Vulkan: https://www.khronos.org/vulkan/

Requirements for Memory Maps(内存映射的要求)
--------------------------------------------

Memory maps have further requirements that apply to both existing maps
and maps created after the device has disappeared. If the underlying
memory disappears, the map is created or modified such that reads and
writes will still complete successfully but the result is undefined.
This applies to both userspace mmap()'d memory and memory pointed to by
dmabuf which might be mapped to other devices (cross-device dmabuf
imports).
(内存映射还有进一步的要求，既适用于已存在的映射，也适用于设备消失之后创建的
映射。如果底层内存消失了，映射会以这样一种方式创建或修改：读写仍能成功完成，
但结果是未定义的。这既适用于用户态 mmap 的内存，也适用于可能被映射到其他设备
的内存（跨设备 dmabuf 导入）所指向的内存。)

Raising SIGBUS is not an option, because userspace cannot realistically
handle it. Signal handlers are global, which makes them extremely
difficult to use correctly from libraries like those that Mesa produces.
Signal handlers are not composable, you can't have different handlers
for GPU1 and GPU2 from different vendors, and a third handler for
mmapped regular files. Threads cause additional pain with signal
handling as well.
(引发 SIGBUS 并不是一个可行的选项，因为用户态实际上无法妥善处理它。信号处理器
是全局的，这使得 Mesa 这类库要正确使用它们极其困难。信号处理器不可组合：你不能
为来自不同厂商的 GPU1 和 GPU2 各设一个处理器，再为 mmap 的普通文件设第三个。
线程也会给信号处理带来额外的痛苦。)

Device reset(设备复位)
======================

The GPU stack is really complex and is prone to errors, from hardware bugs,
faulty applications and everything in between the many layers. Some errors
require resetting the device in order to make the device usable again. This
section describes the expectations for DRM and usermode drivers when a
device resets and how to propagate the reset status.
(GPU 栈非常复杂，从硬件缺陷、有问题的应用，到众多层之间的种种环节，处处都可能
出错。有些错误需要复位设备才能让设备重新可用。本节描述设备复位时对 DRM 与
用户态驱动的要求，以及如何传播复位状态。)

Device resets can not be disabled without tainting the kernel, which can lead to
hanging the entire kernel through shrinkers/mmu_notifiers. Userspace role in
device resets is to propagate the message to the application and apply any
special policy for blocking guilty applications, if any. Corollary is that
debugging a hung GPU context require hardware support to be able to preempt such
a GPU context while it's stopped.
(在不给内核打污点标记的情况下无法禁用设备复位，否则 shrinker/mmu_notifier 可能
导致整个内核挂起。用户态在设备复位中的角色是把消息传播给应用，并执行阻止"过错
方"应用的任何特殊策略（如果有的话）。由此推论，调试一个挂死的 GPU 上下文需要
硬件支持，才能在该上下文停止时抢占它。)

Kernel Mode Driver(内核态驱动)
------------------------------

The KMD is responsible for checking if the device needs a reset, and to perform
it as needed. Usually a hang is detected when a job gets stuck executing.
(KMD 负责检查设备是否需要复位，并按需执行复位。通常当某个作业在执行中卡住时，
就会检测到挂起。)

Propagation of errors to userspace has proven to be tricky since it goes in
the opposite direction of the usual flow of commands. Because of this vendor
independent error handling was added to the &dma_fence object, this way drivers
can add an error code to their fences before signaling them. See function
dma_fence_set_error() on how to do this and for examples of error codes to use.
(向用户态传播错误已被证明是件棘手的事，因为它与通常的命令流向方向相反。正因
如此，与厂商无关的错误处理被加入了 &dma_fence 对象，这样驱动可以在发出信号
之前给 fence 附加一个错误码。具体做法及可用的错误码示例参见
dma_fence_set_error() 函数。)

The DRM scheduler also allows setting error codes on all pending fences when
hardware submissions are restarted after an reset. Error codes are also
forwarded from the hardware fence to the scheduler fence to bubble up errors
to the higher levels of the stack and eventually userspace.
(DRM 调度器还允许在复位之后重新开始硬件提交时，为所有未完成的 fence 设置错误
码。错误码也会从硬件 fence 转发到调度器 fence，让错误一路上浮到栈的更高层，
最终到达用户态。)

Fence errors can be queried by userspace through the generic SYNC_IOC_FILE_INFO
IOCTL as well as through driver specific interfaces.
(用户态可以通过通用的 SYNC_IOC_FILE_INFO IOCTL 以及驱动特定的接口查询 fence
错误。)

Additional to setting fence errors drivers should also keep track of resets per
context, the DRM scheduler provides the drm_sched_entity_error() function as
helper for this use case. After a reset, KMD should reject new command
submissions for affected contexts.
(除了设置 fence 错误之外，驱动还应按上下文跟踪复位情况，DRM 调度器为此用例
提供了 drm_sched_entity_error() 辅助函数。复位之后，KMD 应当拒绝受影响上下文
的新的命令提交。)

User Mode Driver(用户态驱动)
----------------------------

After command submission, UMD should check if the submission was accepted or
rejected. After a reset, KMD should reject submissions, and UMD can issue an
ioctl to the KMD to check the reset status, and this can be checked more often
if the UMD requires it. After detecting a reset, UMD will then proceed to report
it to the application using the appropriate API error code, as explained in the
section below about robustness.
(命令提交之后，UMD 应检查提交是被接受还是被拒绝。复位之后，KMD 应拒绝提交，
UMD 可以向 KMD 发起 ioctl 查询复位状态，而且如果 UMD 需要，还可以更频繁地
检查。检测到复位之后，UMD 将继续使用适当的 API 错误码向应用报告，如下面关于
健壮性的小节所述。)

Robustness(健壮性)
------------------

The only way to try to keep a graphical API context working after a reset is if
it complies with the robustness aspects of the graphical API that it is using.
(要在复位之后让图形 API 上下文继续工作，唯一的办法是让上下文遵守其所用图形
API 的健壮性要求。)

Graphical APIs provide ways to applications to deal with device resets. However,
there is no guarantee that the app will use such features correctly, and a
userspace that doesn't support robust interfaces (like a non-robust
OpenGL context or API without any robustness support like libva) leave the
robustness handling entirely to the userspace driver. There is no strong
community consensus on what the userspace driver should do in that case,
since all reasonable approaches have some clear downsides.
(图形 API 为应用提供了应对设备复位的手段。然而，无法保证应用会正确使用这些
特性；不支持健壮接口的用户态（比如非健壮的 OpenGL 上下文，或完全没有任何健壮
性支持的 API，如 libva）会把健壮性处理完全留给用户态驱动。在这种情况下用户态
驱动应该怎么做，社区并没有强有力的共识，因为所有合理的做法都有明显的缺点。)

OpenGL(OpenGL)
~~~~~~~~~~~~~~

Apps using OpenGL should use the available robust interfaces, like the
extension ``GL_ARB_robustness`` (or ``GL_EXT_robustness`` for OpenGL ES). This
interface tells if a reset has happened, and if so, all the context state is
considered lost and the app proceeds by creating new ones. There's no consensus
on what to do to if robustness is not in use.
(使用 OpenGL 的应用应当使用可用的健壮接口，例如扩展 GL_ARB_robustness
（OpenGL ES 则为 GL_EXT_robustness）。该接口会告知是否发生过复位；如果发生过，
所有上下文状态都视为已丢失，应用随后创建新的上下文。如果不使用健壮性机制该
怎么做，目前没有共识。)

Vulkan(Vulkan)
~~~~~~~~~~~~~~

Apps using Vulkan should check for ``VK_ERROR_DEVICE_LOST`` for submissions.
This error code means, among other things, that a device reset has happened and
it needs to recreate the contexts to keep going.
(使用 Vulkan 的应用应当在提交时检查 VK_ERROR_DEVICE_LOST。这个错误码意味着
（除其他事项外）发生了设备复位，需要重建上下文才能继续。)

Reporting causes of resets(报告复位原因)
----------------------------------------

Apart from propagating the reset through the stack so apps can recover, it's
really useful for driver developers to learn more about what caused the reset in
the first place. For this, drivers can make use of devcoredump to store relevant
information about the reset and send device wedged event with ``none`` recovery
method (as explained in "Device Wedging" chapter) to notify userspace, so this
information can be collected and added to user bug reports.
(除了把复位沿栈向上传播以便应用恢复之外，让驱动开发者进一步了解究竟是什么导致
了复位也非常有用。为此，驱动可以利用 devcoredump 保存与复位相关的信息，并以
``none`` 恢复方法发送 device wedged 事件（如 "Device Wedging"（设备楔死）一章
所述）来通知用户态，这样这些信息就可以被收集起来，加入用户的 bug 报告。)

Device Wedging(设备楔死)
========================

Drivers can optionally make use of device wedged event (implemented as
drm_dev_wedged_event() in DRM subsystem), which notifies userspace of 'wedged'
(hanged/unusable) state of the DRM device through a uevent. This is useful
especially in cases where the device is no longer operating as expected and has
become unrecoverable from driver context. Purpose of this implementation is to
provide drivers a generic way to recover the device with the help of userspace
intervention, without taking any drastic measures (like resetting or
re-enumerating the full bus, on which the underlying physical device is sitting)
in the driver.
(驱动可以选择性地利用 device wedged 事件（在 DRM 子系统中实现为
drm_dev_wedged_event()），它通过 uevent 把 DRM 设备的 "楔死"（挂起/不可用）
状态通知用户态。这在设备不再按预期工作、且从驱动上下文已无法挽回的情况下尤其
有用。该实现的目的是为驱动提供一种通用的方式，借助用户态的干预来恢复设备，而
无需在驱动中采取任何激烈的措施（比如复位或重新枚举底层物理设备所在的整条
总线）。)

A 'wedged' device is basically a device that is declared dead by the driver
after exhausting all possible attempts to recover it from driver context. The
uevent is the notification that is sent to userspace along with a hint about
what could possibly be attempted to recover the device from userspace and bring
it back to usable state. Different drivers may have different ideas of a
'wedged' device depending on hardware implementation of the underlying physical
device, and hence the vendor agnostic nature of the event. It is up to the
drivers to decide when they see the need for device recovery and how they want
to recover from the available methods.
(所谓 "楔死" 的设备，基本上是指驱动在用尽驱动上下文中所有可能的恢复尝试之后
宣告死亡的设备。uevent 是发送给用户态的通知，附带一个提示，说明用户态可以
尝试哪些手段来恢复设备并使其回到可用状态。不同的驱动对 "楔死" 设备的判定可能
不同，取决于底层物理设备的硬件实现，因此该事件是厂商中立的。何时认为需要设备
恢复、以及从可用的方法中如何恢复，由驱动自行决定。)

Driver prerequisites(驱动前提条件)
----------------------------------

The driver, before opting for recovery, needs to make sure that the 'wedged'
device doesn't harm the system as a whole by taking care of the prerequisites.
Necessary actions must include disabling DMA to system memory as well as any
communication channels with other devices. Further, the driver must ensure
that all dma_fences are signalled and any device state that the core kernel
might depend on is cleaned up. All existing mmaps should be invalidated and
page faults should be redirected to a dummy page. Once the event is sent, the
device must be kept in 'wedged' state until the recovery is performed. New
accesses to the device (IOCTLs) should be rejected, preferably with an error
code that resembles the type of failure the device has encountered. This will
signify the reason for wedging, which can be reported to the application if
needed.
(在选择恢复之前，驱动需要通过处理好以下前提条件，确保 "楔死" 的设备不会危害
整个系统。必要的动作必须包括：停用向系统内存的 DMA，以及断开与其他设备的任何
通信通道。此外，驱动必须确保所有 dma_fence 都已发出信号，并且核心内核可能依赖
的任何设备状态都已被清理。所有既有的 mmap 都应被置为无效，缺页应被重定向到
一个哑页。事件发出之后，设备必须保持 "楔死" 状态，直到恢复完成。对设备的新
访问（IOCTL）应被拒绝，最好使用一个能体现设备所遇故障类型的错误码。这将标明
楔死的原因，必要时可报告给应用。)

Recovery(恢复)
--------------

Current implementation defines four recovery methods, out of which, drivers
can use any one, multiple or none. Method(s) of choice will be sent in the
uevent environment as ``WEDGED=<method1>[,..,<methodN>]`` in order of less to
more side-effects. See the section `Vendor Specific Recovery(厂商特定恢复)`_
for ``WEDGED=vendor-specific``. If driver is unsure about recovery or
method is unknown, ``WEDGED=unknown`` will be sent instead.
(当前实现定义了四种恢复方法，驱动可以任选其一、多种或者不用。所选方法将按
副作用从小到大的顺序，通过 uevent 环境变量 ``WEDGED=<method1>[,..,<methodN>]``
发送。``WEDGED=vendor-specific`` 见 `Vendor Specific Recovery(厂商特定恢复)`_
小节。如果驱动不确定如何恢复或方法未知，则发送 ``WEDGED=unknown``。)

Userspace consumers can parse this event and attempt recovery as per the
following expectations.
(用户态消费方可以解析这个事件，并按照以下预期尝试恢复。)

    =============== ========================================
    Recovery method Consumer expectations
    (恢复方法)      (使用者预期)
    =============== ========================================
    none            optional telemetry collection (可选的遥测信息收集)
    rebind          unbind + bind driver (解绑并重新绑定驱动)
    bus-reset       unbind + bus reset/re-enumeration + bind (解绑 + 总线复位/重新枚举 + 绑定)
    vendor-specific vendor specific recovery method (厂商特定的恢复方法)
    unknown         consumer policy (由消费方自行决定)
    =============== ========================================

No Recovery(不恢复)
-------------------

Here ``WEDGED=none`` signifies that no recovery is expected from the consumer
but it can still try to gather telemetry information (devcoredump, syslog) for
debug purpose in order to root cause the hang. This is useful because the first
hang is usually the most critical one which can result in consequential hangs
or complete wedging.
(这里 ``WEDGED=none`` 表示不期望消费方执行任何恢复，但它仍可以尝试收集遥测
信息（devcoredump、syslog）用于调试，以便定位挂起的根本原因。这很有用，因为
第一次挂起通常是最关键的一次，可能引发后续的挂起甚至彻底楔死。)

Vendor Specific Recovery(厂商特定恢复)
--------------------------------------

When ``WEDGED=vendor-specific`` is sent, it indicates that the device requires
a recovery procedure specific to the hardware vendor and is not one of the
standardized approaches.
(发送 ``WEDGED=vendor-specific`` 时，表示设备需要硬件厂商特定的恢复流程，
而不是标准化的方法之一。)

``WEDGED=vendor-specific`` may be used to indicate different cases within a
single vendor driver, each requiring a distinct recovery procedure.
In such scenarios, the vendor driver must provide comprehensive documentation
that describes each case, include additional hints to identify specific case and
outline the corresponding recovery procedure. The documentation includes:
(``WEDGED=vendor-specific`` 可用于表示同一厂商驱动内的不同情形，每种情形需要
各自不同的恢复流程。在这类场景下，厂商驱动必须提供全面的文档，描述每种情形、
给出用于识别具体情形的额外提示，并概述相应的恢复流程。文档内容包括：)

Case - A list of all cases that sends the ``WEDGED=vendor-specific`` recovery method.
(情形：发送 ``WEDGED=vendor-specific`` 恢复方法的全部情形列表。)

Hints - Additional Information to assist the userspace consumer in identifying and
differentiating between different cases. This can be exposed through sysfs, debugfs,
traces, dmesg etc.
(提示：帮助用户态消费方识别并区分不同情形的附加信息。可以通过 sysfs、debugfs、
trace、dmesg 等途径暴露。)

Recovery Procedure - Clear instructions and guidance for recovering each case.
This may include userspace scripts, tools needed for the recovery procedure.
(恢复流程：针对每种情形的清晰指示与指导。可能包括恢复流程所需的用户态脚本
与工具。)

It is the responsibility of the admin/userspace consumer to identify the case and
verify additional identification hints before attempting a recovery procedure.
(识别情形并在尝试恢复流程之前核实附加的识别提示，是管理员/用户态消费方的
责任。)

Example: If the device uses the Xe driver, then userspace consumer should refer to
:ref:`Xe Device Wedging <xe-device-wedging>` for the detailed documentation.
(示例：如果设备使用 Xe 驱动，用户态消费方应参阅
:ref:`Xe Device Wedging <xe-device-wedging>` 以获取详细文档。)

Task information(任务信息)
--------------------------

The information about which application (if any) was involved in the device
wedging is useful for userspace if they want to notify the user about what
happened (e.g. the compositor display a message to the user "The <task name>
caused a graphical error and the system recovered") or to implement policies
(e.g. the daemon may "ban" an task that keeps resetting the device). If the task
information is available, the uevent will display as ``PID=<pid>`` and
``TASK=<task name>``. Otherwise, ``PID`` and ``TASK`` will not appear in the
event string.
(关于哪个应用（如果有的话）与设备楔死有关的信息，对用户态很有用：既可以用来
向用户通报发生了什么（例如合成器向用户显示 "The <task name> caused a graphical
error and the system recovered" 这样的消息），也可以用来实施策略（例如守护进程
可以 "封禁" 不断复位设备的任务）。如果任务信息可用，uevent 将显示为
``PID=<pid>`` 和 ``TASK=<task name>``。否则，``PID`` 和 ``TASK`` 不会出现在
事件字符串中。)

The reliability of this information is driver and hardware specific, and should
be taken with a caution regarding it's precision. To have a big picture of what
really happened, the devcoredump file provides much more detailed information
about the device state and about the event.
(这些信息的可靠性取决于驱动与硬件，对其精确性应持保留态度。要了解实际情况的
全貌，devcoredump 文件提供了关于设备状态与该事件详细得多的信息。)

Consumer prerequisites(消费方前提条件)
--------------------------------------

It is the responsibility of the consumer to make sure that the device or its
resources are not in use by any process before attempting recovery. With IOCTLs
erroring out, all device memory should be unmapped and file descriptors should
be closed to prevent leaks or undefined behaviour. The idea here is to clear the
device of all user context beforehand and set the stage for a clean recovery.
(在尝试恢复之前，确保设备及其资源未被任何进程使用，是消费方的责任。由于
IOCTL 会报错，所有设备内存都应取消映射，文件描述符应被关闭，以防泄漏或未定义
行为。这里的思路是事先清除设备上的所有用户上下文，为一次干净的恢复做好
准备。)

For ``WEDGED=vendor-specific`` recovery method, it is the responsibility of the
consumer to check the driver documentation and the usecase before attempting
a recovery.
(对于 ``WEDGED=vendor-specific`` 恢复方法，在尝试恢复之前查阅驱动文档并了解
使用场景，是消费方的责任。)

Example - rebind(示例：rebind)
------------------------------

Udev rule::

    SUBSYSTEM=="drm", ENV{WEDGED}=="rebind", DEVPATH=="*/drm/card[0-9]",
    RUN+="/path/to/rebind.sh $env{DEVPATH}"

(udev 规则：)

Recovery script::

    #!/bin/sh

    DEVPATH=$(readlink -f /sys/$1/device)
    DEVICE=$(basename $DEVPATH)
    DRIVER=$(readlink -f $DEVPATH/driver)

    echo -n $DEVICE > $DRIVER/unbind
    echo -n $DEVICE > $DRIVER/bind

(恢复脚本：)

Customization(定制)
-------------------

Although basic recovery is possible with a simple script, consumers can define
custom policies around recovery. For example, if the driver supports multiple
recovery methods, consumers can opt for the suitable one depending on scenarios
like repeat offences or vendor specific failures. Consumers can also choose to
have the device available for debugging or telemetry collection and base their
recovery decision on the findings. This is useful especially when the driver is
unsure about recovery or method is unknown.
(虽然用一个简单脚本就能完成基本恢复，但消费方可以围绕恢复定义自定义策略。
例如，如果驱动支持多种恢复方法，消费方可以依据反复出问题或厂商特定故障等场景
选择合适的方法。消费方也可以选择让设备保持可用于调试或遥测收集的状态，并根据
所得发现决定恢复方式。这在驱动不确定如何恢复或方法未知时尤其有用。)

.. _drm_driver_ioctl:

IOCTL Support on Device Nodes(设备节点上的 IOCTL 支持)
======================================================

.. kernel-doc:: drivers/gpu/drm/drm_ioctl.c
   :doc: driver specific ioctls

Recommended IOCTL Return Values(建议的 IOCTL 返回值)
----------------------------------------------------

In theory a driver's IOCTL callback is only allowed to return very few error
codes. In practice it's good to abuse a few more. This section documents common
practice within the DRM subsystem:
(理论上，驱动的 IOCTL 回调只允许返回很少几种错误码。实践中多滥用几个也无妨。
本节记录 DRM 子系统中的常见做法：)

ENOENT:
        Strictly this should only be used when a file doesn't exist e.g. when
        calling the open() syscall. We reuse that to signal any kind of object
        lookup failure, e.g. for unknown GEM buffer object handles, unknown KMS
        object handles and similar cases.
        (严格来说，这个错误码只应在文件不存在时使用，例如调用 open() 系统调用时。
        我们复用它来表示任何形式的对象查找失败，例如未知的 GEM 缓冲对象句柄、
        未知的 KMS 对象句柄等类似情况。)

ENOSPC:
        Some drivers use this to differentiate "out of kernel memory" from "out
        of VRAM". Sometimes also applies to other limited gpu resources used for
        rendering (e.g. when you have a special limited compression buffer).
        Sometimes resource allocation/reservation issues in command submission
        IOCTLs are also signalled through EDEADLK.
        (一些驱动用它来区分 "内核内存耗尽" 与 "显存耗尽"。有时也适用于渲染所用的
        其他受限 GPU 资源（例如拥有一个特殊的、容量有限的压缩缓冲区时）。命令
        提交 IOCTL 中的资源分配/保留问题有时也通过 EDEADLK 报告。)

        Simply running out of kernel/system memory is signalled through ENOMEM.
        (单纯的内核/系统内存耗尽则通过 ENOMEM 报告。)

EPERM/EACCES:
        Returned for an operation that is valid, but needs more privileges.
        E.g. root-only or much more common, DRM master-only operations return
        this when called by unpriviledged clients. There's no clear
        difference between EACCES and EPERM.
        (对合法但需要更多权限的操作返回此错误码。例如仅限 root 的操作，更常见的
        是仅限 DRM master 的操作，在被无特权客户端调用时返回它。EACCES 与
        EPERM 之间没有明确的区别。)

ENODEV:
        The device is not present anymore or is not yet fully initialized.
        (设备已不存在，或尚未完全初始化。)

EOPNOTSUPP:
        Feature (like PRIME, modesetting, GEM) is not supported by the driver.
        (驱动不支持该功能（例如 PRIME、模式设置、GEM）。)

ENXIO:
        Remote failure, either a hardware transaction (like i2c), but also used
        when the exporting driver of a shared dma-buf or fence doesn't support a
        feature needed.
        (远端失败，可以是硬件事务（如 i2c）上的失败；也用于共享 dma-buf 或 fence
        的导出驱动不支持所需特性的情形。)

EINTR:
        DRM drivers assume that userspace restarts all IOCTLs. Any DRM IOCTL can
        return EINTR and in such a case should be restarted with the IOCTL
        parameters left unchanged.
        (DRM 驱动假定用户态会重启所有 IOCTL。任何 DRM IOCTL 都可能返回 EINTR，
        此时应在保持 IOCTL 参数不变的前提下重启该调用。)

EIO:
        The GPU died and couldn't be resurrected through a reset. Modesetting
        hardware failures are signalled through the "link status" connector
        property.
        (GPU 已死亡且无法通过复位复活。模式设置硬件失败通过 "link status"
        连接器属性报告。)

EINVAL:
        Catch-all for anything that is an invalid argument combination which
        cannot work.
        (对任何无法工作的无效参数组合的兜底错误码。)

IOCTL also use other error codes like ETIME, EFAULT, EBUSY, ENOTTY but their
usage is in line with the common meanings. The above list tries to just document
DRM specific patterns. Note that ENOTTY has the slightly unintuitive meaning of
"this IOCTL does not exist", and is used exactly as such in DRM.
(IOCTL 也会使用其他错误码，如 ETIME、EFAULT、EBUSY、ENOTTY，但其用法与通常
含义一致。上面的列表只是试图记录 DRM 特有的模式。注意 ENOTTY 的含义略微反
直觉——"这个 IOCTL 不存在"——DRM 中正是按此含义使用它。)

.. kernel-doc:: include/drm/drm_ioctl.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_ioctl.c
   :export:

.. kernel-doc:: drivers/gpu/drm/drm_ioc32.c
   :export:

Testing and validation(测试与验证)
==================================

Testing Requirements for userspace API(用户态 API 的测试要求)
-------------------------------------------------------------

New cross-driver userspace interface extensions, like new IOCTL, new KMS
properties, new files in sysfs or anything else that constitutes an API change
should have driver-agnostic testcases in IGT for that feature, if such a test
can be reasonably made using IGT for the target hardware.
(新的跨驱动用户态接口扩展，例如新 IOCTL、新 KMS 属性、sysfs 中的新文件，或任何
其他构成 API 变更的东西，如果能够针对目标硬件用 IGT 合理地编写测试，就应当在
IGT 中有与驱动无关的测试用例。)

Validating changes with IGT(用 IGT 验证变更)
--------------------------------------------

There's a collection of tests that aims to cover the whole functionality of
DRM drivers and that can be used to check that changes to DRM drivers or the
core don't regress existing functionality. This test suite is called IGT and
its code and instructions to build and run can be found in
https://gitlab.freedesktop.org/drm/igt-gpu-tools/.
(现有一套测试集合，旨在覆盖 DRM 驱动的全部功能，可用于检查对 DRM 驱动或核心的
改动没有使既有功能发生回归。这套测试套件名为 IGT，其代码以及构建和运行说明见
https://gitlab.freedesktop.org/drm/igt-gpu-tools/ 。)

Using VKMS to test DRM API(用 VKMS 测试 DRM API)
------------------------------------------------

VKMS is a software-only model of a KMS driver that is useful for testing
and for running compositors. VKMS aims to enable a virtual display without
the need for a hardware display capability. These characteristics made VKMS
a perfect tool for validating the DRM core behavior and also support the
compositor developer. VKMS makes it possible to test DRM functions in a
virtual machine without display, simplifying the validation of some of the
core changes.
(VKMS 是 KMS 驱动的纯软件模型，对测试和运行合成器都很有用。VKMS 旨在无需硬件
显示能力的情况下启用一个虚拟显示器。这些特性使 VKMS 成为验证 DRM 核心行为的
完美工具，同时也为合成器开发者提供支持。VKMS 使得在没有显示器的虚拟机中测试
DRM 功能成为可能，从而简化某些核心改动的验证。)

To Validate changes in DRM API with VKMS, start setting the kernel: make
sure to enable VKMS module; compile the kernel with the VKMS enabled and
install it in the target machine. VKMS can be run in a Virtual Machine
(QEMU, virtme or similar). It's recommended the use of KVM with the minimum
of 1GB of RAM and four cores.
(要用 VKMS 验证 DRM API 的变更，先设置内核：确保启用 VKMS 模块；在启用 VKMS
的情况下编译内核，并安装到目标机器上。VKMS 可以在虚拟机（QEMU、virtme 或类似
环境）中运行。推荐使用 KVM，内存至少 1GB，四个核心。)

It's possible to run the IGT-tests in a VM in two ways:
(有两种方式可以在虚拟机中运行 IGT 测试：)

	1. Use IGT inside a VM
	   (在虚拟机内部使用 IGT。)
	2. Use IGT from the host machine and write the results in a shared directory.
	   (在宿主机上使用 IGT，并把结果写到共享目录中。)

Following is an example of using a VM with a shared directory with
the host machine to run igt-tests. This example uses virtme::

	$ virtme-run --rwdir /path/for/shared_dir --kdir=path/for/kernel/directory --mods=auto

(以下示例演示如何使用带有与宿主机共享目录的虚拟机来运行 igt-tests。此示例
使用 virtme。)

Run the igt-tests in the guest machine. This example runs the 'kms_flip'
tests::

	$ /path/for/igt-gpu-tools/scripts/run-tests.sh -p -s -t "kms_flip.*" -v

(在客户机中运行 igt-tests。此示例运行 'kms_flip' 测试。)

In this example, instead of building the igt_runner, Piglit is used
(-p option). It creates an HTML summary of the test results and saves
them in the folder "igt-gpu-tools/results". It executes only the igt-tests
matching the -t option.
(在此示例中，没有构建 igt_runner，而是使用了 Piglit（-p 选项）。它会生成测试
结果的 HTML 摘要，并保存在 "igt-gpu-tools/results" 文件夹中。它只执行与 -t
选项匹配的 igt-tests。)

Display CRC Support(Display CRC 支持)
-------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_debugfs_crc.c
   :doc: CRC ABI

.. kernel-doc:: drivers/gpu/drm/drm_debugfs_crc.c
   :export:

Debugfs Support(Debugfs 支持)
-----------------------------

.. kernel-doc:: include/drm/drm_debugfs.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_debugfs.c
   :export:

Sysfs Support(Sysfs 支持)
=========================

.. kernel-doc:: drivers/gpu/drm/drm_sysfs.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/drm_sysfs.c
   :export:


VBlank event handling(VBlank 事件处理)
======================================

The DRM core exposes two vertical blank related ioctls:
(DRM 核心暴露了两个与垂直消隐相关的 ioctl：)

:c:macro:`DRM_IOCTL_WAIT_VBLANK`
    This takes a struct drm_wait_vblank structure as its argument, and
    it is used to block or request a signal when a specified vblank
    event occurs.
    (它以 struct drm_wait_vblank 结构作为参数，用于在指定的 vblank 事件发生
    时阻塞或请求一个信号。)

:c:macro:`DRM_IOCTL_MODESET_CTL`
    This was only used for user-mode-settind drivers around modesetting
    changes to allow the kernel to update the vblank interrupt after
    mode setting, since on many devices the vertical blank counter is
    reset to 0 at some point during modeset. Modern drivers should not
    call this any more since with kernel mode setting it is a no-op.
    (它原本只供用户态模式设置驱动在模式设置变更前后使用，让内核能在模式设置
    之后更新 vblank 中断，因为在许多设备上，垂直消隐计数器会在模式设置过程中
    的某个时刻被重置为 0。现代驱动不应再调用它，因为有了内核模式设置之后它就是
    空操作。)

Userspace API Structures(用户态 API 结构)
=========================================

.. kernel-doc:: include/uapi/drm/drm_mode.h
   :doc: overview

.. _crtc_index:

CRTC index(CRTC 索引)
---------------------

CRTC's have both an object ID and an index, and they are not the same thing.
The index is used in cases where a densely packed identifier for a CRTC is
needed, for instance a bitmask of CRTC's. The member possible_crtcs of struct
drm_mode_get_plane is an example.
(CRTC 既有对象 ID 也有索引，二者不是一回事。当需要为 CRTC 使用一种紧凑排布的
标识符时（例如 CRTC 位掩码）就会用到索引。struct drm_mode_get_plane 的成员
possible_crtcs 就是一个例子。)

:c:macro:`DRM_IOCTL_MODE_GETRESOURCES` populates a structure with an array of
CRTC ID's, and the CRTC index is its position in this array.
(:c:macro:`DRM_IOCTL_MODE_GETRESOURCES` 会用一个 CRTC ID 数组填充结构体，而
CRTC 索引就是其在该数组中的位置。)

.. kernel-doc:: include/uapi/drm/drm.h
   :internal:

.. kernel-doc:: include/uapi/drm/drm_mode.h
   :internal:


dma-buf interoperability(dma-buf 互操作)
========================================

Please see Documentation/userspace-api/dma-buf-alloc-exchange.rst for
information on how dma-buf is integrated and exposed within DRM.
(关于 dma-buf 如何在 DRM 中集成并暴露，请参阅
Documentation/userspace-api/dma-buf-alloc-exchange.rst。)


Trace events(跟踪事件)
======================

See Documentation/trace/tracepoints.rst for information about using
Linux Kernel Tracepoints.
In the DRM subsystem, some events are considered stable uAPI to avoid
breaking tools (e.g.: GPUVis, umr) relying on them. Stable means that fields
cannot be removed, nor their formatting updated. Adding new fields is
possible, under the normal uAPI requirements.
(关于使用 Linux 内核跟踪点的信息，请参阅 Documentation/trace/tracepoints.rst。
在 DRM 子系统中，一些事件被视为稳定的 uAPI，以免破坏依赖它们的工具（例如
GPUVis、umr）。稳定意味着字段不能被删除，其格式也不能更新。在满足常规 uAPI
要求的前提下，允许新增字段。)

Stable uAPI events(稳定的 uAPI 事件)
------------------------------------

From ``drivers/gpu/drm/scheduler/gpu_scheduler_trace.h``
(来自 ``drivers/gpu/drm/scheduler/gpu_scheduler_trace.h`` 文件。)

.. kernel-doc::  drivers/gpu/drm/scheduler/gpu_scheduler_trace.h
   :doc: uAPI trace events