.. _amdgpu-gc:

=====================================================================
 drm/amdgpu - Graphics and Compute (GC)(drm/amdgpu - 图形与计算 (GC))
=====================================================================

The relationship between the CPU and GPU can be described as the
producer-consumer problem, where the CPU fills out a buffer with operations
(producer) to be executed by the GPU (consumer). The requested operations in
the buffer are called **Command Packets**, which can be summarized as a
compressed way of transmitting command information to the graphics controller.
(CPU 与 GPU 之间的关系可以描述为生产者-消费者问题：CPU 用操作填充缓冲区
(生产者)，由 GPU (消费者) 执行。缓冲区中请求的操作称为 **命令包
(Command Packets)**，可以概括为一种向图形控制器传输命令信息的压缩方式。)

The component that acts as the front end between the CPU and the GPU is called
**Command Processor (CP)**. This component is responsible for providing greater
flexibility to the **Graphics and Compute (GC)** since CP makes it possible to
program various aspects of the GPU pipeline. CP also coordinates the
communication between the CPU and GPU via a mechanism named **Ring Buffers**,
where the CPU appends information to the buffer while the GPU removes
operations. CP is also responsible for handling **Indirect Buffers (IB)**.
(CPU 与 GPU 之间充当前端角色的组件称为 **命令处理器 (Command Processor,
CP)**。该组件负责为 **图形与计算 (Graphics and Compute, GC)** 提供更大的
灵活性，因为 CP 使得可以对 GPU 管线的各个方面进行编程。CP 还通过一种叫做
**环形缓冲区 (Ring Buffers)** 的机制协调 CPU 与 GPU 之间的通信：CPU 向缓冲
区追加信息，而 GPU 移除操作。CP 还负责处理 **间接缓冲区 (Indirect Buffers,
IB)**。)

For reference, internally the CP consists of several sub-blocks (CPC - CP
compute, CPG - CP graphics, and CPF - CP fetcher). Some of these acronyms
appear in register names, but this is more of an implementation detail and not
something that directly impacts driver programming or debugging.
(供参考，CP 内部由若干子块组成 (CPC——CP 计算，CPG——CP 图形，CPF——CP 取指
器)。其中一些缩写出现在寄存器名称中，但这更多是实现细节，并不直接影响
驱动编程或调试。)

Graphics (GFX) and Compute Microcontrollers(图形 (GFX) 与计算微控制器)
----------------------------------------------------------------------

GC is a large block, and as a result, it has multiple firmware associated with
it. Some of them are:
(GC 是一个大块，因此有多个固件与之关联。其中一些是：)

CP (Command Processor)(CP，命令处理器)
    The name for the hardware block that encompasses the front end of the
    GFX/Compute pipeline. Consists mainly of a bunch of microcontrollers
    (PFP, ME, CE, MEC). The firmware that runs on these microcontrollers
    provides the driver interface to interact with the GFX/Compute engine.
    (涵盖 GFX/计算管线前端的硬件块的名称。主要由一组微控制器 (PFP、ME、
    CE、MEC) 组成。运行在这些微控制器上的固件提供与 GFX/计算引擎交互的
    驱动接口。)

    MEC (MicroEngine Compute)(MEC，微引擎计算)
        This is the microcontroller that controls the compute queues on the
        GFX/compute engine.
        (这是控制 GFX/计算引擎上计算队列的微控制器。)

    MES (MicroEngine Scheduler)(MES，微引擎调度器)
        This is the engine for managing queues. For more details check
        :ref:`MicroEngine Scheduler (MES) <amdgpu-mes>`.
        (这是管理队列的引擎。更多细节请查看
        :ref:`MicroEngine Scheduler (MES) <amdgpu-mes>` 。)

RLC (RunList Controller)(RLC，运行列表控制器)
    This is another microcontroller in the GFX/Compute engine. It handles
    power management related functionality within the GFX/Compute engine.
    The name is a vestige of old hardware where it was originally added
    and doesn't really have much relation to what the engine does now.
    (这是 GFX/计算引擎中的另一个微控制器。它处理 GFX/计算引擎内部与电源
    管理相关的功能。这个名字是最初加入它的旧硬件留下的痕迹，与该引擎现在
    的实际功能关系不大。)

.. toctree::

   mes.rst
