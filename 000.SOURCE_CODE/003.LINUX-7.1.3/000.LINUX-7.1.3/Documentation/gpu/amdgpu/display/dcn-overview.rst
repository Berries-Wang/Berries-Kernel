.. _dcn_overview:

=============================================
Display Core Next (DCN)(显示核心下一代 (DCN))
=============================================

To equip our readers with the basic knowledge of how AMD Display Core Next
(DCN) works, we need to start with an overview of the hardware pipeline. Below
you can see a picture that provides a DCN overview, keep in mind that this is a
generic diagram, and we have variations per ASIC.
(为了让读者掌握 AMD Display Core Next (DCN) 工作原理的基础知识，我们需要从硬件管线概览开始。下面你可以看到一幅提供 DCN 概览的图，请记住这是一张通用示意图，每个 ASIC 都有自己的变化。)

.. kernel-figure:: dc_pipeline_overview.svg

Based on this diagram, we can pass through each block and briefly describe
them:
(基于此图，我们可以逐一过一遍每个块并简要描述它们：)

* **Display Controller Hub (DCHUB)**: This is the gateway between the Scalable
  Data Port (SDP) and DCN. This component has multiple features, such as memory
  arbitration, rotation, and cursor manipulation.
  (**Display Controller Hub (DCHUB)** ：这是可扩展数据端口 (SDP) 与 DCN 之间的网关。该组件有多种特性，例如内存仲裁、旋转与光标操作。)

* **Display Pipe and Plane (DPP)**: This block provides pre-blend pixel
  processing such as color space conversion, linearization of pixel data, tone
  mapping, and gamut mapping.
  (**Display Pipe and Plane (DPP)** ：该块提供混合前的像素处理，例如色彩空间转换、像素数据线性化、色调映射与色域映射。)

* **Multiple Pipe/Plane Combined (MPC)**: This component performs blending of
  multiple planes, using global or per-pixel alpha.
  (**Multiple Pipe/Plane Combined (MPC)** ：该组件使用全局或逐像素 alpha 对多个平面进行混合。)

* **Output Pixel Processing (OPP)**: Process and format pixels to be sent to
  the display.
  (**Output Pixel Processing (OPP)** ：处理并格式化要发送到显示器的像素。)

* **Output Pipe Timing Combiner (OPTC)**: It generates time output to combine
  streams or divide capabilities. CRC values are generated in this block.
  (**Output Pipe Timing Combiner (OPTC)** ：它生成时序输出以合并流或分拆能力。CRC 值在该块中生成。)

* **Display Output (DIO)**: Codify the output to the display connected to our
  GPU.
  (**Display Output (DIO)** ：把输出编码给连接到我们 GPU 的显示器。)

* **Display Writeback (DWB)**: It provides the ability to write the output of
  the display pipe back to memory as video frames.
  (**Display Writeback (DWB)** ：它提供把显示管线的输出作为视频帧写回内存的能力。)

* **Multi-Media HUB (MMHUBBUB)**: Memory controller interface for DMCUB and DWB
  (Note that DWB is not hooked yet).
  (**Multi-Media HUB (MMHUBBUB)** ：供 DMCUB 与 DWB 使用的内存控制器接口 (注意 DWB 尚未挂接)。)

* **DCN Management Unit (DMU)**: It provides registers with access control and
  interrupts the controller to the SOC host interrupt unit. This block includes
  the Display Micro-Controller Unit - version B (DMCUB), which is handled via
  firmware.
  (**DCN Management Unit (DMU)** ：它提供带访问控制的寄存器，并把中断控制器接到 SOC 宿主中断单元。该块包含显示微控制器单元 B 版 (DMCUB)，由固件处理。)

* **DCN Clock Generator Block (DCCG)**: It provides the clocks and resets
  for all of the display controller clock domains.
  (**DCN Clock Generator Block (DCCG)** ：它为所有显示控制器时钟域提供时钟与复位。)

* **Azalia (AZ)**: Audio engine.
  (**Azalia (AZ)** ：音频引擎。)

The above diagram is an architecture generalization of DCN, which means that
every ASIC has variations around this base model. Notice that the display
pipeline is connected to the Scalable Data Port (SDP) via DCHUB; you can see
the SDP as the element from our Data Fabric that feeds the display pipe.
(上图是 DCN 的架构泛化，这意味着每个 ASIC 都围绕这一基础模型有所变化。注意显示管线通过 DCHUB 连接到可扩展数据端口 (SDP)；可以把 SDP 看作我们 Data Fabric 中为显示管线供数的元素。)

Always approach the DCN architecture as something flexible that can be
configured and reconfigured in multiple ways; in other words, each block can be
setup or ignored accordingly with userspace demands. For example, if we
want to drive an 8k@60Hz with a DSC enabled, our DCN may require 4 DPP and 2
OPP. It is DC's responsibility to drive the best configuration for each
specific scenario. Orchestrate all of these components together requires a
sophisticated communication interface which is highlighted in the diagram by
the edges that connect each block; from the chart, each connection between
these blocks represents:
(请始终把 DCN 架构视为可以通过多种方式配置与重配置的灵活之物；换句话说，每个块都可以根据用户态的需求被设置或忽略。例如，如果我们想在启用 DSC 的情况下驱动 8k@60Hz，我们的 DCN 可能需要 4 个 DPP 与 2 个 OPP。为每个具体场景驱动最佳配置是 DC 的职责。要把所有这些组件协调在一起，需要一个复杂的通信接口，图中连接各块的边即突出了这一点；从图上看，这些块之间的每种连接代表：)

1. Pixel data interface (red): Represents the pixel data flow;
   (1. 像素数据接口 (红色)：代表像素数据流；)
2. Global sync signals (green): It is a set of synchronization signals composed
   by VStartup, VUpdate, and VReady;
   (2. 全局同步信号 (绿色)：这是一组由 VStartup、VUpdate 与 VReady 构成的同步信号；)
3. Config interface: Responsible to configure blocks;
   (3. 配置接口：负责配置各个块；)
4. Sideband signals: All other signals that do not fit the previous one.
   (4. 边带信号：所有不属于上述类别的其他信号。)

These signals are essential and play an important role in DCN. Nevertheless,
the Global Sync deserves an extra level of detail described in the next
section.
(这些信号至关重要，在 DCN 中扮演重要角色。不过，全局同步 (Global Sync) 值得更细致的介绍，见下一节。)

All of these components are represented by a data structure named dc_state.
From DCHUB to MPC, we have a representation called dc_plane; from MPC to OPTC,
we have dc_stream, and the output (DIO) is handled by dc_link. Keep in mind
that HUBP accesses a surface using a specific format read from memory, and our
dc_plane should work to convert all pixels in the plane to something that can
be sent to the display via dc_stream and dc_link.
(所有这些组件都由一个名为 dc_state 的数据结构表示。从 DCHUB 到 MPC，我们有一种叫 dc_plane 的表示；从 MPC 到 OPTC，我们有 dc_stream；输出 (DIO) 则由 dc_link 处理。请记住，HUBP 使用从内存中读取的特定格式访问表面 (surface)，而我们的 dc_plane 应负责把平面中的所有像素转换成可以通过 dc_stream 与 dc_link 发往显示器的东西。)

Front End and Back End(前端与后端)
----------------------------------

Display pipeline can be broken down into two components that are usually
referred as **Front End (FE)** and **Back End (BE)**, where FE consists of:
(显示管线可以分解为通常称为 **Front End (FE)** (前端) 与 **Back End (BE)** (后端) 的两个部分，其中 FE 由以下组成：)

* DCHUB (Mainly referring to a subcomponent named HUBP)
  (DCHUB (主要指名为 HUBP 的子组件))
* DPP
* MPC

On the other hand, BE consist of
(另一方面，BE 由以下组成：)

* OPP
* OPTC
* DIO (DP/HDMI stream encoder and link encoder)
  (DIO (DP/HDMI 流编码器与链路编码器))

OPP and OPTC are two joining blocks between FE and BE. On a side note, this is
a one-to-one mapping of the link encoder to PHY, but we can configure the DCN
to choose which link encoder to connect to which PHY. FE's main responsibility
is to change, blend and compose pixel data, while BE's job is to frame a
generic pixel stream to a specific display's pixel stream.
(OPP 与 OPTC 是 FE 与 BE 之间衔接的两个块。附带一提，链路编码器与 PHY 是一对一映射，但我们可以配置 DCN 来选择把哪个链路编码器连接到哪个 PHY。FE 的主要职责是改变、混合与合成像素数据，而 BE 的工作是把通用像素流成帧为特定显示器的像素流。)

Data Flow(数据流)
-----------------

Initially, data is passed in from VRAM through Data Fabric (DF) in native pixel
formats. Such data format stays through till HUBP in DCHUB, where HUBP unpacks
different pixel formats and outputs them to DPP in uniform streams through 4
channels (1 for alpha + 3 for colors).
(最初，数据以原生像素格式从 VRAM 经由 Data Fabric (DF) 传入。这种数据格式一直保持到 DCHUB 中的 HUBP；HUBP 在此解包不同的像素格式，并通过 4 个通道 (1 个用于 alpha + 3 个用于颜色) 以统一的流输出给 DPP。)

The Converter and Cursor (CNVC) in DPP would then normalize the data
representation and convert them to a DCN specific floating-point format (i.e.,
different from the IEEE floating-point format). In the process, CNVC also
applies a degamma function to transform the data from non-linear to linear
space to relax the floating-point calculations following. Data would stay in
this floating-point format from DPP to OPP.
(DPP 中的转换器与光标单元 (CNVC) 随后会把数据表示规范化，并转换为 DCN 特定的浮点格式 (即不同于 IEEE 浮点格式)。在此过程中，CNVC 还会应用去伽马 (degamma) 函数，把数据从非线性空间变换到线性空间，以简化后续的浮点计算。数据从 DPP 到 OPP 都保持这种浮点格式。)

Starting OPP, because color transformation and blending have been completed
(i.e alpha can be dropped), and the end sinks do not require the precision and
dynamic range that floating points provide (i.e. all displays are in integer
depth format), bit-depth reduction/dithering would kick in. In OPP, we would
also apply a regamma function to introduce the gamma removed earlier back.
Eventually, we output data in integer format at DIO.
(从 OPP 开始，由于颜色变换与混合已经完成 (即 alpha 可以丢弃)，且末端接收器不需要浮点提供的精度与动态范围 (即所有显示器都是整数深度格式)，位深缩减/抖动 (dithering) 就会启动。在 OPP 中，我们还会应用再伽马 (regamma) 函数，把之前去除的伽马重新引入。最终，我们在 DIO 以整数格式输出数据。)

AMD Hardware Pipeline(AMD 硬件管线)
------------------------------------

When discussing graphics on Linux, the **pipeline** term can sometimes be
overloaded with multiple meanings, so it is important to define what we mean
when we say **pipeline**. In the DCN driver, we use the term **hardware
pipeline** or **pipeline** or just **pipe** as an abstraction to indicate a
sequence of DCN blocks instantiated to address some specific configuration. DC
core treats DCN blocks as individual resources, meaning we can build a pipeline
by taking resources for all individual hardware blocks to compose one pipeline.
In actuality, we can't connect an arbitrary block from one pipe to a block from
another pipe; they are routed linearly, except for DSC, which can be
arbitrarily assigned as needed. We have this pipeline concept for trying to
optimize bandwidth utilization.
(在讨论 Linux 图形时， **pipeline** 一词有时承载多重含义，因此定义我们说 **pipeline** 时的意思很重要。在 DCN 驱动中，我们用 **hardware
pipeline** 、 **pipeline** 或简称 **pipe** 作为抽象，表示为解决某个特定配置而实例化的一串 DCN 块。DC core 把 DCN 块当作独立资源，这意味着我们可以通过为所有单独的硬件块取用资源来组成一条管线。实际上，我们不能把一条 pipe 中的任意块连接到另一条 pipe 的块；它们是线性布线的，只有 DSC 例外，可以按需任意指派。我们引入这个管线概念是为了尝试优化带宽利用。)

.. kernel-figure:: pipeline_4k_no_split.svg

Additionally, let's take a look at parts of the DTN log (see
'Documentation/gpu/amdgpu/display/dc-debug.rst' for more information) since
this log can help us to see part of this pipeline behavior in real-time::

 HUBP:  format  addr_hi  width  height ...
 [ 0]:      8h      81h   3840    2160
 [ 1]:      0h       0h      0       0
 [ 2]:      0h       0h      0       0
 [ 3]:      0h       0h      0       0
 [ 4]:      0h       0h      0       0
 ...
 MPCC:  OPP  DPP ...
 [ 0]:   0h   0h ...

(另外，让我们看看 DTN 日志的部分内容 (更多信息见
'Documentation/gpu/amdgpu/display/dc-debug.rst')，因为它可以帮助我们实时观察这一管线的部分行为。)

The first thing to notice from the diagram and DTN log it is the fact that we
have different clock domains for each part of the DCN blocks. In this example,
we have just a single **pipeline** where the data flows from DCHUB to DIO, as
we intuitively expect. Nonetheless, DCN is flexible, as mentioned before, and
we can split this single pipe differently, as described in the below diagram:
(从图与 DTN 日志中首先要注意到的是：DCN 块的每一部分都有不同的时钟域。在本例中，我们只有一条 **pipeline** ，数据像我们直观期望的那样从 DCHUB 流向 DIO。不过，如前所述 DCN 是灵活的，我们可以用不同方式拆分这一条 pipe，如下图所示：)

.. kernel-figure:: pipeline_4k_split.svg

Now, if we inspect the DTN log again we can see some interesting changes::

 HUBP:  format  addr_hi  width  height ...
 [ 0]:      8h      81h   1920    2160 ...
 ...
 [ 4]:      0h       0h      0       0 ...
 [ 5]:      8h      81h   1920    2160 ...
 ...
 MPCC:  OPP  DPP ...
 [ 0]:   0h   0h ...
 [ 5]:   0h   5h ...

(现在，如果我们再次检查 DTN 日志，可以看到一些有趣的变化。)

From the above example, we now split the display pipeline into two vertical
parts of 1920x2160 (i.e., 3440x2160), and as a result, we could reduce the
clock frequency in the DPP part. This is not only useful for saving power but
also to better handle the required throughput. The idea to keep in mind here is
that the pipe configuration can vary a lot according to the display
configuration, and it is the DML's responsibility to set up all required
configuration parameters for multiple scenarios supported by our hardware.
(从上面的例子可以看到，我们现在把显示管线拆分为两个 1920x2160 的垂直部分 (即 3440x2160)，因此我们可以降低 DPP 部分的时钟频率。这不仅有助于省电，也能更好地处理所需的吞吐量。这里要记住的要点是：pipe 配置会随显示配置有很大变化，由 DML 负责为我们的硬件支持的多种场景设置所有必需的配置参数。)

Global Sync(全局同步)
---------------------

Many DCN registers are double buffered, most importantly the surface address.
This allows us to update DCN hardware atomically for page flips, as well as
for most other updates that don't require enabling or disabling of new pipes.
(许多 DCN 寄存器是双缓冲的，最重要的是表面地址。这使我们能够以原子方式更新 DCN 硬件以完成页翻转，以及大多数其他不需要启用或禁用新 pipe 的更新。)

(Note: There are many scenarios when DC will decide to reserve extra pipes
in order to support outputs that need a very high pixel clock, or for
power saving purposes.)
(注意：在很多场景下，DC 会决定保留额外的 pipe，以支持需要极高像素时钟的输出，或出于省电目的。)

These atomic register updates are driven by global sync signals in DCN. In
order to understand how atomic updates interact with DCN hardware, and how DCN
signals page flip and vblank events it is helpful to understand how global sync
is programmed.
(这些原子寄存器更新由 DCN 中的全局同步信号驱动。要理解原子更新如何与 DCN 硬件交互、DCN 如何发出页翻转与 vblank 事件信号，了解全局同步是如何编程的会很有帮助。)

Global sync consists of three signals, VSTARTUP, VUPDATE, and VREADY. These are
calculated by the Display Mode Library - DML (drivers/gpu/drm/amd/display/dc/dml)
based on a large number of parameters and ensure our hardware is able to feed
the DCN pipeline without underflows or hangs in any given system configuration.
The global sync signals always happen during VBlank, are independent from the
VSync signal, and do not overlap each other.
(全局同步由三个信号组成：VSTARTUP、VUPDATE 与 VREADY。它们由显示模式库 DML (drivers/gpu/drm/amd/display/dc/dml) 基于大量参数计算得出，确保我们的硬件在任何给定的系统配置下都能为 DCN 管线供数而不发生下溢 (underflow) 或挂起。全局同步信号总是发生在 VBlank 期间，独立于 VSync 信号，且彼此不重叠。)

VUPDATE is the only signal that is of interest to the rest of the driver stack
or userspace clients as it signals the point at which hardware latches to
atomically programmed (i.e. double buffered) registers. Even though it is
independent of the VSync signal we use VUPDATE to signal the VSync event as it
provides the best indication of how atomic commits and hardware interact.
(VUPDATE 是驱动栈其余部分或用户态客户端唯一关心的信号，因为它标志着硬件锁存原子编程 (即双缓冲) 寄存器的时点。尽管它独立于 VSync 信号，我们仍用 VUPDATE 来发出 VSync 事件信号，因为它最恰当地反映了原子提交与硬件的交互。)

Since DCN hardware is double-buffered the DC driver is able to program the
hardware at any point during the frame.
(由于 DCN 硬件是双缓冲的，DC 驱动可以在帧内的任意时刻对硬件编程。)

The below picture illustrates the global sync signals:
(下图展示了全局同步信号：)

.. kernel-figure:: global_sync_vblank.svg

These signals affect core DCN behavior. Programming them incorrectly will lead
to a number of negative consequences, most of them quite catastrophic.
(这些信号影响 DCN 的核心行为。对它们的编程出错会导致许多负面后果，其中大多数相当灾难性。)

The following picture shows how global sync allows for a mailbox style of
updates, i.e. it allows for multiple re-configurations between VUpdate
events where only the last configuration programmed before the VUpdate signal
becomes effective.
(下图展示了全局同步如何支持邮箱 (mailbox) 式的更新，即它允许在 VUpdate 事件之间进行多次重新配置，只有 VUpdate 信号发出前最后一次编程的配置会生效。)

.. kernel-figure:: config_example.svg
