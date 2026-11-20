==============================
AMDGPU Glossary(AMDGPU 术语表)
==============================

Here you can find some generic acronyms used in the amdgpu driver. Notice that
we have a dedicated glossary for Display Core at
'Documentation/gpu/amdgpu/display/dc-glossary.rst'.
(在这里可以找到 amdgpu 驱动中使用的一些通用缩写词。注意，我们在
'Documentation/gpu/amdgpu/display/dc-glossary.rst' 处有一个专门针对
Display Core 的术语表。)

.. glossary::

    active_cu_number
      The number of CUs that are active on the system.  The number of active
      CUs may be less than SE * SH * CU depending on the board configuration.
      (系统上处于活动状态的 CU 数量。活动 CU 的数量可能小于
      SE * SH * CU，取决于板卡配置。)

    BACO
      Bus Alive, Chip Off (总线保持活动，芯片关闭)

    BOCO
      Bus Off, Chip Off (总线关闭，芯片关闭)

    CE
      Constant Engine (常量引擎)

    CIK
      Sea Islands (海岛系列)

    CB
      Color Buffer (颜色缓冲)

    CP
      Command Processor (命令处理器)

    CPC
      Command Processor Compute (命令处理器 - 计算)

    CPF
      Command Processor Fetch (命令处理器 - 取指)

    CPG
      Command Processor Graphics (命令处理器 - 图形)

    CPLIB
      Content Protection Library (内容保护库)

    CS
      Command Submission (命令提交)

    CSB
      Clear State Indirect Buffer (清除状态间接缓冲)

    CU
      Compute Unit (计算单元)

    DB
      Depth Buffer (深度缓冲)

    DFS
      Digital Frequency Synthesizer (数字频率合成器)

    ECP
      Enhanced Content Protection (增强内容保护)

    EOP
      End Of Pipe/Pipeline (管线末端)

    FLR
      Function Level Reset (功能级复位)

    GART
      Graphics Address Remapping Table.  This is the name we use for the GPUVM
      page table used by the GPU kernel driver.  It remaps system resources
      (memory or MMIO space) into the GPU's address space so the GPU can access
      them.  The name GART harkens back to the days of AGP when the platform
      provided an MMU that the GPU could use to get a contiguous view of
      scattered pages for DMA.  The MMU has since moved on to the GPU, but the
      name stuck.
      (图形地址重映射表 (Graphics Address Remapping Table)。这是我们给
      GPU 内核驱动所使用的 GPUVM 页表起的名字。它把系统资源 (内存或 MMIO
      空间) 重映射到 GPU 的地址空间，以便 GPU 能够访问它们。GART 这个名字
      可以追溯到 AGP 时代，当时由平台提供一个 MMU，GPU 可以用它获得对分散
      页进行 DMA 的连续视图。MMU 后来移到了 GPU 上，但这个名字保留了下来。)

    GC
      Graphics and Compute (图形与计算)

    GDS
      Global Data Share (全局数据共享)

    GE
      Geometry Engine (几何引擎)

    GMC
      Graphic Memory Controller (图形内存控制器)

    GPR
      General Purpose Register (通用寄存器)

    GPUVM
      GPU Virtual Memory.  This is the GPU's MMU.  The GPU supports multiple
      virtual address spaces that can be in flight at any given time.  These
      allow the GPU to remap VRAM and system resources into GPU virtual address
      spaces for use by the GPU kernel driver and applications using the GPU.
      These provide memory protection for different applications using the GPU.
      (GPU 虚拟内存 (GPU Virtual Memory)。这是 GPU 的 MMU。GPU 支持多个
      可以同时在途的虚拟地址空间。这些机制让 GPU 能够把 VRAM 和系统资源
      重映射到 GPU 虚拟地址空间，供 GPU 内核驱动和使用 GPU 的应用程序
      使用。它们为使用 GPU 的不同应用程序提供内存保护。)

    GTT
      Graphics Translation Tables.  This is a memory pool managed through TTM
      which provides access to system resources (memory or MMIO space) for
      use by the GPU. These addresses can be mapped into the "GART" GPUVM page
      table for use by the kernel driver or into per process GPUVM page tables
      for application usage.
      (图形转换表 (Graphics Translation Tables)。这是通过 TTM 管理的一个
      内存池，提供对系统资源 (内存或 MMIO 空间) 的访问以供 GPU 使用。这些
      地址可以被映射到 "GART" GPUVM 页表中供内核驱动使用，或映射到每进程
      的 GPUVM 页表中供应用程序使用。)

    GWS
      Global Wave Sync (全局 wave 同步)

    IH
      Interrupt Handler (中断处理器)

    IV
      Interrupt Vector (中断向量)

    HQD
      Hardware Queue Descriptor (硬件队列描述符)

    IB
      Indirect Buffer (间接缓冲)

    IMU
      Integrated Management Unit (Power Management support)
      (集成管理单元 (电源管理支持))

    IP
        Intellectual Property blocks
        (知识产权块 (Intellectual Property blocks))

    KCQ
      Kernel Compute Queue (内核计算队列)

    KFD
      Kernel Fusion Driver (内核融合驱动)

    KGQ
      Kernel Graphics Queue (内核图形队列)

    KIQ
      Kernel Interface Queue (内核接口队列)

    MC
      Memory Controller (内存控制器)

    MCBP
      Mid Command Buffer Preemption (命令缓冲区中途抢占)

    ME
      MicroEngine (Graphics) (微引擎 (图形))

    MEC
      MicroEngine Compute (微引擎 (计算))

    MES
      MicroEngine Scheduler (微引擎调度器)

    MMHUB
      Multi-Media HUB (多媒体 HUB)

    MQD
      Memory Queue Descriptor (内存队列描述符)

    PA
      Primitive Assembler / Physical Address (图元装配器 / 物理地址)

    PDE
      Page Directory Entry (页目录项)

    PFP
      Pre-Fetch Parser (Graphics) (预取解析器 (图形))

    PPLib
      PowerPlay Library - PowerPlay is the power management component.
      (PowerPlay 库 - PowerPlay 是电源管理组件。)

    PRT
      Partially Resident Texture (also known as sparse residency)
      (部分驻留纹理 (也称为稀疏驻留))

    PSP
        Platform Security Processor
        (平台安全处理器)

    PTE
      Page Table Entry (页表项)

    RB
      Render Backends. Some people called it ROPs.
      (渲染后端 (Render Backends)。有些人称之为 ROP。)

    RLC
      RunList Controller. This name is a remnant of past ages and doesn't have
      much meaning today. It's a group of general-purpose helper engines for
      the GFX block. It's involved in GFX power management and SR-IOV, among
      other things.
      (运行列表控制器 (RunList Controller)。这个名字是过去时代的遗迹，
      今天已没有太多含义。它是 GFX 块的一组通用辅助引擎。它参与 GFX 电源
      管理与 SR-IOV 等。)

    SC
      Scan Converter (扫描转换器)

    SDMA
      System DMA (系统 DMA)

    SE
      Shader Engine (着色器引擎)

    SGPR
      Scalar General-Purpose Registers (标量通用寄存器)

    SH
      SHader array (着色器阵列 (SHader array))

    SI
      Southern Islands (南方群岛系列)

    SMU/SMC
      System Management Unit / System Management Controller
      (系统管理单元 / 系统管理控制器)

    SPI (AMDGPU)
      Shader Processor Input (着色器处理器输入)

    SRLC
      Save/Restore List Control (保存/恢复列表控制)

    SRLG
      Save/Restore List GPM_MEM (保存/恢复列表 GPM_MEM)

    SRLS
      Save/Restore List SRM_MEM (保存/恢复列表 SRM_MEM)

    SS
      Spread Spectrum (扩频)

    SX
      Shader Export (着色器导出)

    TA
      Trusted Application (可信应用)

    TC
      Texture Cache (纹理缓存)

    TCP (AMDGPU)
      Texture Cache per Pipe. Even though the name "Texture" is part of this
      acronym, the TCP represents the path to memory shaders; i.e., it is not
      related to texture. The name is a leftover from older designs where shader
      stages had different cache designs; it refers to the L1 cache in older
      architectures.
      (每管线纹理缓存 (Texture Cache per Pipe)。尽管 "Texture" 是这个缩写
      的一部分，但 TCP 表示到内存着色器 (memory shaders) 的路径；也就是
      说，它与纹理无关。这个名字是旧设计的遗留物 —— 当时各着色器阶段有
      不同的缓存设计；它指的是旧架构中的 L1 缓存。)

    TMR
      Trusted Memory Region (可信内存区域)

    TMZ
      Trusted Memory Zone (可信内存区)

    TOC
      Table of Contents (目录表)

    UMC
      Unified Memory Controller (统一内存控制器)

    UMSCH
      User Mode Scheduler (用户模式调度器)

    UTC (AMDGPU)
      Unified Translation Cache. UTC is equivalent to TLB. You might see a
      variation of this acronym with L at the end, i.e., UTCL followed by a
      number; L means the cache level (e.g., UTCL1 and UTCL2).
      (统一转换缓存 (Unified Translation Cache)。UTC 等价于 TLB。你可能
      会看到这个缩写末尾带 L 的变体，即 UTCL 后跟一个数字；L 表示缓存
      级别 (例如 UTCL1 与 UTCL2)。)

    UVD
      Unified Video Decoder (统一视频解码器)

    VCE
      Video Compression Engine (视频压缩引擎)

    VCN
      Video Codec Next (下一代视频编解码器 (Video Codec Next))

    VGPR
      Vector General-Purpose Registers (向量通用寄存器)

    VMID
      Virtual Memory ID (虚拟内存 ID)

    VPE
      Video Processing Engine (视频处理引擎)

    XCC
      Accelerator Core Complex (加速器核心复合体)

    XCP
      Accelerator Core Partition (加速器核心分区)
