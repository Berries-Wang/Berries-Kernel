=============================================
 Core Driver Infrastructure(核心驱动基础设施)
=============================================

GPU Hardware Structure(GPU 硬件结构)
====================================

Each ASIC is a collection of hardware blocks.  We refer to them as
"IPs" (Intellectual Property blocks).  Each IP encapsulates certain
functionality. IPs are versioned and can also be mixed and matched.
E.g., you might have two different ASICs that both have System DMA (SDMA) 5.x IPs.
The driver is arranged by IPs.  There are driver components to handle
the initialization and operation of each IP.  There are also a bunch
of smaller IPs that don't really need much if any driver interaction.
Those end up getting lumped into the common stuff in the soc files.
The soc files (e.g., vi.c, soc15.c nv.c) contain code for aspects of
the SoC itself rather than specific IPs.  E.g., things like GPU resets
and register access functions are SoC dependent.
(每个 ASIC 都是硬件块的集合。我们称它们为 "IP" (知识产权块)。每个 IP 封装特定的功能。IP 有版本之分，还可以混合搭配。例如，可能有两个不同的 ASIC 都带有 System DMA (SDMA) 5.x IP。驱动按 IP 组织。有驱动组件负责每个 IP 的初始化与运行。还有一些较小的 IP 基本不需要什么驱动交互，它们最终被归入 soc 文件中的公共部分。soc 文件 (如 vi.c、soc15.c、nv.c) 包含的是 SoC 本身而非特定 IP 相关的代码。例如，GPU 复位与寄存器访问函数都依赖于 SoC。)

An APU contains more than just CPU and GPU, it also contains all of
the platform stuff (audio, usb, gpio, etc.).  Also, a lot of
components are shared between the CPU, platform, and the GPU (e.g.,
SMU, PSP, etc.).  Specific components (CPU, GPU, etc.) usually have
their interface to interact with those common components.  For things
like S0i3 there is a ton of coordination required across all the
components, but that is probably a bit beyond the scope of this
section.
(APU 包含的不只是 CPU 和 GPU，还包含所有平台相关部分 (音频、USB、GPIO 等)。另外，许多组件由 CPU、平台与 GPU 三者共享 (如 SMU、PSP 等)。特定组件 (CPU、GPU 等) 通常有各自与这些公共组件交互的接口。像 S0i3 这类功能需要所有组件间大量的协同，不过这可能略微超出了本节的范围。)

With respect to the GPU, we have the following major IPs:
(就 GPU 而言，我们有以下主要 IP：)

GMC (Graphics Memory Controller)
    This was a dedicated IP on older pre-vega chips, but has since
    become somewhat decentralized on vega and newer chips.  They now
    have dedicated memory hubs for specific IPs or groups of IPs.  We
    still treat it as a single component in the driver however since
    the programming model is still pretty similar.  This is how the
    different IPs on the GPU get the memory (VRAM or system memory).
    It also provides the support for per process GPU virtual address
    spaces.
    (GMC (图形内存控制器)：在较老的 vega 之前芯片上这是一个专用 IP，但在 vega 及更新的芯片上已变得有些去中心化。它们现在为特定 IP 或 IP 组配备了专用内存 hub。不过在驱动中我们仍把它当作单一组件，因为编程模型仍然相当相似。GPU 上不同的 IP 就是这样获得内存 (VRAM 或系统内存) 的。它还提供对每进程 GPU 虚拟地址空间的支持。)

IH (Interrupt Handler)
    This is the interrupt controller on the GPU.  All of the IPs feed
    their interrupts into this IP and it aggregates them into a set of
    ring buffers that the driver can parse to handle interrupts from
    different IPs.
    (IH (中断处理器)：这是 GPU 上的中断控制器。所有 IP 把中断送入这个 IP，它再把它们汇聚成一组环形缓冲区，驱动可以解析这些缓冲区以处理来自不同 IP 的中断。)

PSP (Platform Security Processor)
    This handles security policy for the SoC and executes trusted
    applications, and validates and loads firmwares for other blocks.
    (PSP (平台安全处理器)：负责 SoC 的安全策略，执行可信应用，并为其他块验证和加载固件。)

SMU (System Management Unit)
    This is the power management microcontroller.  It manages the entire
    SoC.  The driver interacts with it to control power management
    features like clocks, voltages, power rails, etc.
    (SMU (系统管理单元)：这是电源管理微控制器。它管理整个 SoC。驱动与它交互以控制时钟、电压、电源轨等电源管理特性。)

DCN (Display Controller Next)
    This is the display controller.  It handles the display hardware.
    It is described in more details in :ref:`Display Core <amdgpu-display-core>`.
    (DCN (下一代显示控制器)：这是显示控制器，负责显示硬件。更多细节见 :ref:`Display Core <amdgpu-display-core>`。)

SDMA (System DMA)
    This is a multi-purpose DMA engine.  The kernel driver uses it for
    various things including paging and GPU page table updates.  It's also
    exposed to userspace for use by user mode drivers (OpenGL, Vulkan,
    etc.)
    (SDMA (系统 DMA)：这是一个多用途 DMA 引擎。内核驱动将其用于多种用途，包括分页 (paging) 与 GPU 页表更新。它也暴露给用户态，供用户态驱动 (OpenGL、Vulkan 等) 使用。)

GC (Graphics and Compute)
    This is the graphics and compute engine, i.e., the block that
    encompasses the 3D pipeline and shader blocks.  This is by far the
    largest block on the GPU.  The 3D pipeline has tons of sub-blocks.  In
    addition to that, it also contains the CP microcontrollers (ME, PFP, CE,
    MEC) and the RLC microcontroller.  It's exposed to userspace for user mode
    drivers (OpenGL, Vulkan, OpenCL, etc.). More details in :ref:`Graphics (GFX)
    and Compute <amdgpu-gc>`.
    (GC (图形与计算)：这是图形与计算引擎，即涵盖 3D 流水线与着色器块的块。这是 GPU 上迄今为止最大的块。3D 流水线有大量子块。除此之外，它还包含 CP 微控制器 (ME、PFP、CE、MEC) 与 RLC 微控制器。它暴露给用户态供用户态驱动 (OpenGL、Vulkan、OpenCL 等) 使用。更多细节见 :ref:`Graphics (GFX)
    and Compute <amdgpu-gc>`。)

VCN (Video Core Next)
    This is the multi-media engine.  It handles video and image encode and
    decode.  It's exposed to userspace for user mode drivers (VA-API,
    OpenMAX, etc.)
    (VCN (下一代视频核心)：这是多媒体引擎，负责视频与图像的编解码。它暴露给用户态供用户态驱动 (VA-API、OpenMAX 等) 使用。)

It is important to note that these blocks can interact with each other. The
picture below illustrates some of the components and their interconnection:
(需要注意的是，这些块之间可以相互交互。下图展示了部分组件及其互连关系：)

.. kernel-figure:: amd_overview_block.svg

In the diagram, memory-related blocks are shown in green. Notice that specific
IPs have a green square that represents a small hardware block named 'hub',
which is responsible for interfacing with memory. All memory hubs are connected
in the UMCs, which in turn are connected to memory blocks. As a note,
pre-vega devices have a dedicated block for the Graphic Memory Controller
(GMC), which was replaced by UMC and hubs in new architectures. In the driver
code, you can identify this component by looking for the suffix hub, for
example: gfxhub, dchub, mmhub, vmhub, etc. Keep in mind that the component's
interaction with the memory block may vary across architectures. For example,
on Navi and newer, GC and SDMA are both attached to GCHUB; on pre-Navi, SDMA
goes through MMHUB; VCN, JPEG, and VPE go through MMHUB; DCN goes through
DCHUB.
(图中与内存相关的块以绿色显示。注意特定 IP 有一个绿色方块，代表一个名为 'hub' 的小硬件块，负责与内存对接。所有内存 hub 都连接在 UMC 中，而 UMC 又连接到内存块。另外说明一下，vega 之前的设备有一个专用的图形内存控制器 (GMC) 块，在新架构中已被 UMC 和 hub 取代。在驱动代码中，可以通过查找 hub 后缀来识别这个组件，例如：gfxhub、dchub、mmhub、vmhub 等。请记住该组件与内存块的交互方式可能因架构而异。例如，在 Navi 及更新的架构上，GC 与 SDMA 都挂在 GCHUB 上；在 Navi 之前，SDMA 走 MMHUB；VCN、JPEG 与 VPE 走 MMHUB；DCN 走 DCHUB。)

There is some protection for certain memory elements, and the PSP plays an
essential role in this area. When a specific firmware is loaded into memory,
the PSP takes steps to ensure it has a valid signature. It also stores firmware
images in a protected memory area named Trusted Memory Area (TMR), so the OS or
driver can't corrupt them at runtime. Another use of PSP is to support Trusted
Applications (TA), which are basically small applications that run on the
trusted processor and handles a trusted operation (e.g., HDCP). PSP is also
used for encrypted memory for content protection via Trusted Memory Zone (TMZ).
(某些内存元素有相应的保护机制，PSP 在这方面扮演关键角色。当特定固件被加载进内存时，PSP 会采取措施确保其签名有效。它还把固件镜像存储在一个名为可信内存区 (Trusted Memory Area, TMR) 的受保护内存区域中，使操作系统或驱动无法在运行时破坏它们。PSP 的另一个用途是支持可信应用 (Trusted Applications, TA)，它们基本上是运行在可信处理器上、执行某项可信操作 (如 HDCP) 的小型应用。PSP 还通过可信内存区 (Trusted Memory Zone, TMZ) 为内容保护提供加密内存。)

Another critical IP is the SMU. It handles reset distribution, as well as
clock, thermal, and power management for all IPs on the SoC. SMU also helps to
balance performance and power consumption.
(另一个关键 IP 是 SMU。它负责复位分发以及 SoC 上所有 IP 的时钟、散热与电源管理。SMU 还帮助平衡性能与功耗。)

.. _pipes-and-queues-description:

GFX, Compute, and SDMA Overall Behavior(GFX、Compute 与 SDMA 总体行为)
======================================================================

.. note:: For simplicity, whenever the term block is used in this section, it
   means GFX, Compute, and SDMA.
   (为简单起见，本节中凡使用 "block" (块) 一词之处，均指 GFX、Compute 与 SDMA。)

GFX, Compute and SDMA share a similar form of operation that can be abstracted
to facilitate understanding of the behavior of these blocks. See the figure
below illustrating the common components of these blocks:
(GFX、Compute 与 SDMA 共享一种相似的操作形式，可以将其抽象以便于理解这些块的行为。参见下图，它展示了这些块的公共组件：)

.. kernel-figure:: pipe_and_queue_abstraction.svg

In the central part of this figure, you can see two hardware elements, one called
**Pipes** and another called **Queues**; it is important to highlight that Queues
must be associated with a Pipe and vice-versa. Every specific hardware IP may have
a different number of Pipes and, in turn, a different number of Queues; for
example, GFX 11 has two Pipes and two Queues per Pipe for the GFX front end.
(在图的中央部分可以看到两个硬件元素：一个叫 **管道 (Pipes)** ，另一个叫 **队列 (Queues)** ；需要强调的是，队列必须与管道关联，反之亦然。每个具体硬件 IP 的管道数量可能不同，队列数量也随之不同；例如，GFX 11 的 GFX 前端有两个管道，每个管道两个队列。)

Pipe is the hardware that processes the instructions available in the Queues;
in other words, it is a thread executing the operations inserted in the Queue.
One crucial characteristic of Pipes is that they can only execute one Queue at
a time; no matter if the hardware has multiple Queues in the Pipe, it only runs
one Queue per Pipe.
(管道是处理队列中可用指令的硬件；换句话说，它是执行插入到队列中的操作的线程。管道的一个关键特性是它一次只能执行一个队列；无论硬件在管道中有多少个队列，每个管道每次只运行一个队列。)

Pipes have the mechanics of swapping between queues at the hardware level.
Nonetheless, they only make use of Queues that are considered mapped. Pipes can
switch between queues based on any of the following inputs:
(管道在硬件层面具备在队列之间切换的机制。不过，它们只会使用被视为已映射 (mapped) 的队列。管道可以基于以下任一输入在队列间切换：)

1. Command Stream;(命令流；)
2. Packet by Packet;(逐包切换；)
3. Other hardware requests the change (e.g., MES).(其他硬件请求切换 (如 MES)。)

Queues within Pipes are defined by the Hardware Queue Descriptors (HQD).
Associated with the HQD concept, we have the Memory Queue Descriptor (MQD),
which is responsible for storing information about the state of each of the
available Queues in the memory. The state of a Queue contains information such
as the GPU virtual address of the queue itself, save areas, doorbell, etc. The
MQD also stores the HQD registers, which are vital for activating or
deactivating a given Queue.  The scheduling firmware (e.g., MES) is responsible
for loading HQDs from MQDs and vice versa.
(管道中的队列由硬件队列描述符 (Hardware Queue Descriptors, HQD) 定义。与 HQD 概念相关联的是内存队列描述符 (Memory Queue Descriptor, MQD)，它负责在内存中存储每个可用队列的状态信息。队列的状态包含诸如队列本身的 GPU 虚拟地址、保存区 (save areas)、doorbell 等信息。MQD 还存储 HQD 寄存器，这些寄存器对激活或停用给定队列至关重要。调度固件 (如 MES) 负责在 MQD 与 HQD 之间相互装载。)

The Queue-switching process can also happen with the firmware requesting the
preemption or unmapping of a Queue. The firmware waits for the HQD_ACTIVE bit
to change to low before saving the state into the MQD. To make a different
Queue become active, the firmware copies the MQD state into the HQD registers
and loads any additional state. Finally, it sets the HQD_ACTIVE bit to high to
indicate that the queue is active.  The Pipe will then execute work from active
Queues.
(队列切换过程也可能由固件请求抢占或解映射某个队列而触发。固件会等待 HQD_ACTIVE 位变为低电平后再把状态保存进 MQD。要让另一个队列变为活动状态，固件把 MQD 状态复制到 HQD 寄存器并装载任何附加状态。最后，它把 HQD_ACTIVE 位置为高电平以表明该队列已激活。随后管道就会执行来自活动队列的工作。)

Driver Structure(驱动结构)
===========================

In general, the driver has a list of all of the IPs on a particular
SoC and for things like init/fini/suspend/resume, more or less just
walks the list and handles each IP.
(总体而言，驱动维护特定 SoC 上所有 IP 的列表，对于 init/fini/suspend/resume 之类的操作，基本上就是遍历该列表并逐个处理每个 IP。)

Some useful constructs:
(一些有用的构造：)

KIQ (Kernel Interface Queue)
    This is a control queue used by the kernel driver to manage other gfx
    and compute queues on the GFX/compute engine.  You can use it to
    map/unmap additional queues, etc.  This is replaced by MES on
    GFX 11 and newer hardware.
    (KIQ (内核接口队列)：内核驱动用来管理 GFX/计算引擎上其他 gfx 与 compute 队列的控制队列。可以用它映射/解映射额外的队列等。在 GFX 11 及更新的硬件上被 MES 取代。)

IB (Indirect Buffer)
    A command buffer for a particular engine.  Rather than writing
    commands directly to the queue, you can write the commands into a
    piece of memory and then put a pointer to the memory into the queue.
    The hardware will then follow the pointer and execute the commands in
    the memory, then returning to the rest of the commands in the ring.
    (IB (间接缓冲)：特定引擎的命令缓冲。无需把命令直接写入队列，而是可以把命令写进一块内存，再把指向该内存的指针放入队列。硬件随后沿指针执行内存中的命令，然后返回环形缓冲区中剩余的命令。)

.. _amdgpu_memory_domains:

Memory Domains(内存域)
======================

.. kernel-doc:: include/uapi/drm/amdgpu_drm.h
   :doc: memory domains

Buffer Objects(缓冲对象)
=========================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_object.c
   :doc: amdgpu_object

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_object.c
   :internal:

PRIME Buffer Sharing(PRIME 缓冲共享)
====================================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_dma_buf.c
   :doc: PRIME Buffer Sharing

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_dma_buf.c
   :internal:

MMU Notifier(MMU 通知器)
========================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_hmm.c
   :doc: MMU Notifier

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_hmm.c
   :internal:

AMDGPU Virtual Memory(AMDGPU 虚拟内存)
======================================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_vm.c
   :doc: GPUVM

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_vm.c
   :internal:

Interrupt Handling(中断处理)
============================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_irq.c
   :doc: Interrupt Handling

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_irq.c
   :internal:

IP Blocks(IP 块)
=================

.. kernel-doc:: drivers/gpu/drm/amd/include/amd_shared.h
   :doc: IP Blocks

.. kernel-doc:: drivers/gpu/drm/amd/include/amd_shared.h
   :identifiers: amd_ip_block_type amd_ip_funcs DC_FEATURE_MASK DC_DEBUG_MASK
