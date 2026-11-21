========================
 Ring Buffer(环形缓冲区)
========================

To handle communication between user space and kernel space, AMD GPUs use a
ring buffer design to feed the engines (GFX, Compute, SDMA, UVD, VCE, VCN, VPE,
etc.). See the figure below that illustrates how this communication works:
(为了处理用户空间与内核空间之间的通信，AMD GPU 使用环形缓冲区(ring
buffer)设计来向各个引擎(GFX、Compute、SDMA、UVD、VCE、VCN、VPE 等)
供给工作。请看下面说明这种通信如何工作的图示：)

.. kernel-figure:: ring_buffers.svg

Ring buffers in the amdgpu work as a producer-consumer model, where userspace
acts as the producer, constantly filling the ring buffer with GPU commands to
be executed. Meanwhile, the GPU retrieves the information from the ring, parses
it, and distributes the specific set of instructions between the different
amdgpu blocks.
(amdgpu 中的环形缓冲区按照生产者-消费者模型工作，其中用户空间充当
生产者，不断用要执行的 GPU 命令填充环形缓冲区。与此同时，GPU 从 ring
中取出信息，解析它，并把特定的一组指令分配给不同的 amdgpu 模块。)

Notice from the diagram that the ring has a Read Pointer (rptr), which
indicates where the engine is currently reading packets from the ring, and a
Write Pointer (wptr), which indicates how many packets software has added to
the ring. When the rptr and wptr are equal, the ring is idle. When software
adds packets to the ring, it updates the wptr, this causes the engine to start
fetching and processing packets. As the engine processes packets, the rptr gets
updates until the rptr catches up to the wptr and they are equal again.
(从图中注意，ring 有一个读指针(rptr)，指示引擎当前正从 ring 的哪里
读取数据包；还有一个写指针(wptr)，指示软件已向 ring 添加了多少数据
包。当 rptr 与 wptr 相等时，ring 处于空闲状态。当软件向 ring 添加数据
包时，它会更新 wptr，这会使引擎开始取指并处理数据包。随着引擎处理
数据包，rptr 会不断更新，直到 rptr 追上 wptr，两者再次相等。)

Usually, ring buffers in the driver have a limited size (search for occurrences
of `amdgpu_ring_init()`). One of the reasons for the small ring buffer size is
that CP (Command Processor) is capable of following addresses inserted into the
ring; this is illustrated in the image by the reference to the IB (Indirect
Buffer). The IB gives userspace the possibility to have an area in memory that
CP can read and feed the hardware with extra instructions.
(通常，驱动中的环形缓冲区大小有限(搜索 amdgpu_ring_init() 的出现
位置)。环形缓冲区较小的原因之一是 CP(命令处理器)能够跟随插入 ring
的地址；图中对 IB(间接缓冲区，Indirect Buffer)的引用说明了这一点。
IB 让用户空间可以在内存中拥有一个 CP 能读取并从中向硬件提供额外指令
的区域。)

All ASICs pre-GFX11 use what is called a kernel queue, which means
the ring is allocated in kernel space and has some restrictions, such as not
being able to be :ref:`preempted directly by the scheduler<amdgpu-mes>`. GFX11
and newer support kernel queues, but also provide a new mechanism named
:ref:`user queues<amdgpu-userq>`, where the queue is moved to the user space
and can be mapped and unmapped via the scheduler. In practice, both queues
insert user-space-generated GPU commands from different jobs into the requested
component ring.
(GFX11 之前的所有 ASIC 都使用所谓的内核队列(kernel queue)，即 ring
分配在内核空间中并有一些限制，例如不能被 :ref:`调度器直接抢占<amdgpu-mes>`。
GFX11 及更新版本支持内核队列，但也提供一种名为 :ref:`用户队列<amdgpu-userq>`
的新机制，其中队列被移到用户空间，并可以通过调度器进行映射和取消映射。
实际上，两种队列都会把来自不同作业的用户空间生成的 GPU 命令插入到
所请求组件的 ring 中。)

Enforce Isolation(强制隔离)
===========================

.. note:: After reading this section, you might want to check the
   :ref:`Process Isolation<amdgpu-process-isolation>` page for more details.
   (阅读完本节后，你可能想查看 :ref:`Process Isolation<amdgpu-process-isolation>`
   页面以了解更多细节。)

Before examining the Enforce Isolation mechanism in the ring buffer context, it
is helpful to briefly discuss how instructions from the ring buffer are
processed in the graphics pipeline. Let’s expand on this topic by checking the
diagram below that illustrates the graphics pipeline:
(在环形缓冲区上下文中考察 Enforce Isolation 机制之前，先简要讨论一下
来自环形缓冲区的指令在图形管线中是如何处理的会很有帮助。让我们通过
查看下面说明图形管线的图来展开这个主题：)

.. kernel-figure:: gfx_pipeline_seq.svg

In terms of executing instructions, the GFX pipeline follows the sequence:
Shader Export (SX), Geometry Engine (GE), Shader Process or Input (SPI), Scan
Converter (SC), Primitive Assembler (PA), and cache manipulation (which may
vary across ASICs). Another common way to describe the pipeline is to use Pixel
Shader (PS), raster, and Vertex Shader (VS) to symbolize the two shader stages.
Now, with this pipeline in mind, let's assume that Job B causes a hang issue,
but Job C's instruction might already be executing, leading developers to
incorrectly identify Job C as the problematic one. This problem can be
mitigated on multiple levels; the diagram below illustrates how to minimize
part of this problem:
(在指令执行方面，GFX 管线遵循这样的顺序：Shader Export (SX)、
Geometry Engine (GE)、Shader Process 或 Input (SPI)、Scan Converter
(SC)、Primitive Assembler (PA)，以及缓存操作(不同 ASIC 之间可能有所
不同)。描述该管线的另一种常见方式是用 Pixel Shader (PS)、光栅化
(raster)和 Vertex Shader (VS) 来表示两个着色器阶段。现在，记住这个
管线，我们假设作业 B 导致了一个挂起(hang)问题，但作业 C 的指令可能
已经在执行，这会导致开发者错误地把作业 C 认定为目标问题作业。这个
问题可以在多个层面上缓解；下图说明了如何把该问题的一部分影响降到
最低：)

.. kernel-figure:: no_enforce_isolation.svg

Note from the diagram that there is no guarantee of order or a clear separation
between instructions, which is not a problem most of the time, and is also good
for performance. Furthermore, notice some circles between jobs in the diagram
that represent a **fence wait** used to avoid overlapping work in the ring. At
the end of the fence, a cache flush occurs, ensuring that when the next job
starts, it begins in a clean state and, if issues arise, the developer can
pinpoint the problematic process more precisely.
(从图中注意，指令之间没有顺序保证，也没有清晰的分隔——这在大多数
时候并不是问题，而且对性能也有好处。此外，注意图中作业之间的一些
圆圈，它们代表用于避免 ring 中工作重叠的 **fence wait** (栅栏等待)。
在 fence 结束时会进行一次缓存刷新，确保下一个作业开始时处于干净的
状态，并且如果出现问题，开发者可以更精确地定位有问题的进程。)

To increase the level of isolation between jobs, there is the "Enforce
Isolation" method described in the picture below:
(为了提高作业之间的隔离程度，有下图所描述的 "Enforce Isolation"
(强制隔离)方法：)

.. kernel-figure:: enforce_isolation.svg

As shown in the diagram, enforcing isolation introduces ordering between
submissions, since the access to GFX/Compute is serialized, think about it as
single process at a time mode for gfx/compute. Notice that this approach has a
significant performance impact, as it allows only one job to submit commands at
a time. However, this option can help pinpoint the job that caused the problem.
Although enforcing isolation improves the situation, it does not fully resolve
the issue of precisely pinpointing bad jobs, since isolation might mask the
problem. In summary, identifying which job caused the issue may not be precise,
but enforcing isolation might help with the debugging.
(如图所示，强制隔离在提交之间引入了顺序，因为对 GFX/Compute 的访问
被串行化了——可以把它想象成 gfx/compute 的单进程模式。注意，这种
方法有显著的性能影响，因为它一次只允许一个作业提交命令。不过，这个
选项有助于定位引发问题的作业。尽管强制隔离改善了情况，但它并没有
完全解决精确锁定坏作业的问题，因为隔离可能掩盖问题。总之，确定是
哪个作业引发的问题可能并不精确，但强制隔离可能有助于调试。)

Ring Operations(环形缓冲区操作)
===============================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_ring.c
   :internal:

