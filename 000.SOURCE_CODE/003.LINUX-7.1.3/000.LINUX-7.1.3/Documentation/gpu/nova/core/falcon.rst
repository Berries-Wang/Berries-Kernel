.. SPDX-License-Identifier: GPL-2.0

=====================================================
Falcon (FAst Logic Controller)(Falcon 快速逻辑控制器)
=====================================================
The following sections describe the Falcon core and the ucode running on it.
The descriptions are based on the Ampere GPU or earlier designs; however, they
should mostly apply to future designs as well, but everything is subject to
change. The overview provided here is mainly tailored towards understanding the
interactions of nova-core driver with the Falcon.
(以下各节描述 Falcon 核心以及运行在其上的 ucode。描述基于 Ampere GPU 或更早的
设计；不过，它们大多也应适用于未来的设计，但一切都可能发生变化。这里提供的
概述主要着眼于理解 nova-core 驱动与 Falcon 的交互。)

NVIDIA GPUs embed small RISC-like microcontrollers called Falcon cores, which
handle secure firmware tasks, initialization, and power management. Modern
NVIDIA GPUs may have multiple such Falcon instances (e.g., GSP (the GPU system
processor) and SEC2 (the security engine)) and also may integrate a RISC-V core.
This core is capable of running both RISC-V and Falcon code.
(NVIDIA GPU 内嵌称为 Falcon 核心的类 RISC 小型微控制器，负责处理安全固件任务、
初始化和电源管理。现代 NVIDIA GPU 可能有多个这样的 Falcon 实例（例如 GSP（GPU
系统处理器）和 SEC2（安全引擎）），还可能集成一个 RISC-V 核心。该核心既能运行
RISC-V 代码，也能运行 Falcon 代码。)

The code running on the Falcon cores is also called 'ucode', and will be
referred to as such in the following sections.
(运行在 Falcon 核心上的代码也称为 "ucode"，后续章节将沿用这一称呼。)

Falcons have separate instruction and data memories (IMEM/DMEM) and provide a
small DMA engine (via the FBIF - "Frame Buffer Interface") to load code from
system memory. The nova-core driver must reset and configure the Falcon, load
its firmware via DMA, and start its CPU.
(Falcon 拥有独立的指令存储器和数据存储器（IMEM/DMEM），并提供一个小型 DMA
引擎（通过 FBIF——"Frame Buffer Interface"（帧缓冲接口））从系统内存加载代码。
nova-core 驱动必须复位并配置 Falcon，通过 DMA 加载其固件，并启动其 CPU。)

Falcon security levels(Falcon 安全级别)
=======================================
Falcons can run in Non-secure (NS), Light Secure (LS), or Heavy Secure (HS)
modes.
(Falcon 可以在非安全（Non-secure，NS）、轻安全（Light Secure，LS）或重安全
（Heavy Secure，HS）模式下运行。)

Heavy Secured (HS) also known as Privilege Level 3 (PL3)(重安全级 (HS)，亦称特权级 3 (PL3))
-------------------------------------------------------------------------------------------
HS ucode is the most trusted code and has access to pretty much everything on
the chip. The HS binary includes a signature in it which is verified at boot.
This signature verification is done by the hardware itself, thus establishing a
root of trust. For example, the FWSEC-FRTS command (see fwsec.rst) runs on the
GSP in HS mode. FRTS, which involves setting up and loading content into the WPR
(Write Protect Region), has to be done by the HS ucode and cannot be done by the
host CPU or LS ucode.
(HS ucode 是最受信任的代码，几乎可以访问芯片上的一切。HS 二进制文件中包含一个
签名，该签名在启动时被验证。这一签名验证由硬件本身完成，从而建立起信任根。例如，
FWSEC-FRTS 命令（见 fwsec.rst）以 HS 模式运行在 GSP 上。FRTS 涉及设置 WPR（写
保护区域）并将内容加载其中，必须由 HS ucode 完成，宿主 CPU 或 LS ucode 都无法
完成。)

Light Secured (LS or PL2) and Non Secured (NS or PL0)(轻安全级 (LS 或 PL2) 与非安全级 (NS 或 PL0))
--------------------------------------------------------------------------------------------------
These modes are less secure than HS. Like HS, the LS or NS ucode binary also
typically includes a signature in it. To load firmware in LS or NS mode onto a
Falcon, another Falcon needs to be running in HS mode, which also establishes the
root of trust. For example, in the case of an Ampere GPU, the CPU runs the "Booter"
ucode in HS mode on the SEC2 Falcon, which then authenticates and runs the
run-time GSP binary (GSP-RM) in LS mode on the GSP Falcon. Similarly, as an
example, after reset on an Ampere, FWSEC runs on the GSP which then loads the
devinit engine onto the PMU in LS mode.
(这些模式的安全性低于 HS。与 HS 一样，LS 或 NS ucode 二进制文件通常也包含
签名。要把固件以 LS 或 NS 模式加载到 Falcon 上，需要另一个以 HS 模式运行的
Falcon，由它建立信任根。例如，在 Ampere GPU 上，CPU 在 SEC2 Falcon 上以 HS
模式运行 "Booter" ucode，由它认证并在 GSP Falcon 上以 LS 模式运行运行时的 GSP
二进制（GSP-RM）。类似地，举例来说，Ampere 复位后 FWSEC 运行在 GSP 上，随后
以 LS 模式把 devinit 引擎加载到 PMU 上。)

Root of trust establishment(信任根的建立)
-----------------------------------------
To establish a root of trust, the code running on a Falcon must be immutable and
hardwired into a read-only memory (ROM). This follows industry norms for
verification of firmware. This code is called the Boot ROM (BROM). The nova-core
driver on the CPU communicates with Falcon's Boot ROM through various Falcon
registers prefixed with "BROM" (see regs.rs).
(要建立信任根，运行在 Falcon 上的代码必须是不可变的，并硬连线到只读存储器
（ROM）中。这符合业界固件验证的惯例。这些代码称为 Boot ROM（BROM）。CPU 上的
nova-core 驱动通过以 "BROM" 为前缀的各个 Falcon 寄存器与 Falcon 的 Boot ROM
通信（见 regs.rs）。)

After nova-core driver reads the necessary ucode from VBIOS, it programs the
BROM and DMA registers to trigger the Falcon to load the HS ucode from the system
memory into the Falcon's IMEM/DMEM. Once the HS ucode is loaded, it is verified
by the Falcon's Boot ROM.
(nova-core 驱动从 VBIOS 读取所需的 ucode 后，会编程 BROM 和 DMA 寄存器，触发
Falcon 把 HS ucode 从系统内存加载到 Falcon 的 IMEM/DMEM 中。HS ucode 一经加载，
就由 Falcon 的 Boot ROM 进行验证。)

Once the verified HS code is running on a Falcon, it can verify and load other
LS/NS ucode binaries onto other Falcons and start them. The process of signature
verification is the same as HS; just in this case, the hardware (BROM) doesn't
compute the signature, but the HS ucode does.
(一旦经过验证的 HS 代码在 Falcon 上运行，它就可以验证其他 LS/NS ucode 二进制
文件、将其加载到其他 Falcon 上并启动它们。签名验证过程与 HS 相同；只是在这种
情况下，计算签名的不是硬件（BROM），而是 HS ucode。)

The root of trust is therefore established as follows:
     Hardware (Boot ROM running on the Falcon) -> HS ucode -> LS/NS ucode.

(因此，信任根按如下方式建立：
     硬件（运行在 Falcon 上的 Boot ROM）-> HS ucode -> LS/NS ucode。)

On an Ampere GPU, for example, the boot verification flow is:
     Hardware (Boot ROM running on the SEC2) ->
          HS ucode (Booter running on the SEC2) ->
               LS ucode (GSP-RM running on the GSP)

(例如，Ampere GPU 的启动验证流程为：
     硬件（运行在 SEC2 上的 Boot ROM）->
          HS ucode（运行在 SEC2 上的 Booter）->
               LS ucode（运行在 GSP 上的 GSP-RM）)

.. note::
     While the CPU can load HS ucode onto a Falcon microcontroller and have it
     verified by the hardware and run, the CPU itself typically does not load
     LS or NS ucode and run it. Loading of LS or NS ucode is done mainly by the
     HS ucode. For example, on an Ampere GPU, after the Booter ucode runs on the
     SEC2 in HS mode and loads the GSP-RM binary onto the GSP, it needs to run
     the "SEC2-RTOS" ucode at runtime. This presents a problem: there is no
     component to load the SEC2-RTOS ucode onto the SEC2. The CPU cannot load
     LS code, and GSP-RM must run in LS mode. To overcome this, the GSP is
     temporarily made to run HS ucode (which is itself loaded by the CPU via
     the nova-core driver using a "GSP-provided sequencer") which then loads
     the SEC2-RTOS ucode onto the SEC2 in LS mode. The GSP then resumes
     running its own GSP-RM LS ucode.
     (注意，虽然 CPU 可以把 HS ucode 加载到 Falcon 微控制器上，让硬件验证并
     运行它，但 CPU 自身通常不加载、也不运行 LS 或 NS ucode。LS 或 NS ucode
     的加载主要由 HS ucode 完成。例如，在 Ampere GPU 上，Booter ucode 以 HS
     模式在 SEC2 上运行并把 GSP-RM 二进制加载到 GSP 之后，它需要在运行时运行
     "SEC2-RTOS" ucode。这就带来一个问题：没有任何组件能把 SEC2-RTOS ucode
     加载到 SEC2 上。CPU 无法加载 LS 代码，而 GSP-RM 又必须以 LS 模式运行。
     为了解决这一问题，GSP 被临时改为运行 HS ucode（它本身由 CPU 通过
     nova-core 驱动、借助 "GSP 提供的序列器" 加载），由它再以 LS 模式把
     SEC2-RTOS ucode 加载到 SEC2 上。随后 GSP 恢复运行自己的 GSP-RM LS
     ucode。)

Falcon memory subsystem and DMA engine(Falcon 内存子系统与 DMA 引擎)
====================================================================
Falcons have separate instruction and data memories (IMEM/DMEM)
and contains a small DMA engine called FBDMA (Framebuffer DMA) which does
DMA transfers to/from the IMEM/DMEM memory inside the Falcon via the FBIF
(Framebuffer Interface), to external memory.
(Falcon 拥有独立的指令存储器和数据存储器（IMEM/DMEM），并包含一个称为 FBDMA
（Framebuffer DMA，帧缓冲 DMA）的小型 DMA 引擎，它经由 FBIF（Framebuffer
Interface，帧缓冲接口）在 Falcon 内部的 IMEM/DMEM 存储器与外部内存之间执行
DMA 传输。)

DMA transfers are possible from the Falcon's memory to both the system memory
and the framebuffer memory (VRAM).
(从 Falcon 的内存既可以向系统内存发起 DMA 传输，也可以向帧缓冲内存（VRAM）
发起。)

To perform a DMA via the FBDMA, the FBIF is configured to decide how the memory
is accessed (also known as aperture type). In the nova-core driver, this is
determined by the `FalconFbifTarget` enum.
(要通过 FBDMA 执行 DMA，需要配置 FBIF 来决定内存的访问方式（也称为 aperture
类型）。在 nova-core 驱动中，这由 `FalconFbifTarget` 枚举决定。)

The IO-PMP block (Input/Output Physical Memory Protection) unit in the Falcon
controls access by the FBDMA to the external memory.
(Falcon 中的 IO-PMP 块（Input/Output Physical Memory Protection，输入/输出
物理内存保护）单元控制 FBDMA 对外部内存的访问。)

Conceptual diagram (not exact) of the Falcon and its memory subsystem is as follows::

               External Memory (Framebuffer / System DRAM)
                              ^  |
                              |  |
                              |  v
     +-----------------------------------------------------+
     |                           |                         |
     |   +---------------+       |                         |
     |   |     FBIF      |-------+                         |  FALCON
     |   | (FrameBuffer  |   Memory Interface              |  PROCESSOR
     |   |  InterFace)   |                                 |
     |   |  Apertures    |                                 |
     |   |  Configures   |                                 |
     |   |  mem access   |                                 |
     |   +-------^-------+                                 |
     |           |                                         |
     |           | FBDMA uses configured FBIF apertures    |
     |           | to access External Memory
     |           |
     |   +-------v--------+      +---------------+
     |   |    FBDMA       |  cfg |     RISC      |
     |   | (FrameBuffer   |<---->|     CORE      |----->. Direct Core Access
     |   |  DMA Engine)   |      |               |      |
     |   | - Master dev.  |      | (can run both |      |
     |   +-------^--------+      | Falcon and    |      |
     |           |        cfg--->| RISC-V code)  |      |
     |           |        /      |               |      |
     |           |        |      +---------------+      |    +------------+
     |           |        |                             |    |   BROM     |
     |           |        |                             <--->| (Boot ROM) |
     |           |       /                              |    +------------+
     |           |      v                               |
     |   +---------------+                              |
     |   |    IO-PMP     | Controls access by FBDMA     |
     |   | (IO Physical  | and other IO Masters         |
     |   | Memory Protect)                              |
     |   +-------^-------+                              |
     |           |                                      |
     |           | Protected Access Path for FBDMA      |
     |           v                                      |
     |   +---------------------------------------+      |
     |   |       Memory                          |      |
     |   |   +---------------+  +------------+   |      |
     |   |   |    IMEM       |  |    DMEM    |   |<-----+
     |   |   | (Instruction  |  |   (Data    |   |
     |   |   |  Memory)      |  |   Memory)  |   |
     |   |   +---------------+  +------------+   |
     |   +---------------------------------------+
     +-----------------------------------------------------+

(Falcon 及其内存子系统的概念图（并非精确图示）如下：)
