=================================
DC Programming Model(DC 编程模型)
=================================

In the :ref:`Display Core Next (DCN) <dcn_overview>` and :ref:`DCN Block
<dcn_blocks>` pages, you learned about the hardware components and how they
interact with each other. On this page, the focus is shifted to the display
code architecture. Hence, it is reasonable to remind the reader that the code
in DC is shared with other OSes; for this reason, DC provides a set of
abstractions and operations to connect different APIs with the hardware
configuration. See DC as a service available for a Display Manager (amdgpu_dm)
to access and configure DCN/DCE hardware (DCE is also part of DC, but for
simplicity's sake, this documentation only examines DCN).
(在 :ref:`Display Core Next (DCN) <dcn_overview>` 和 :ref:`DCN Block <dcn_blocks>` 页面中，你了解了硬件组件以及它们之间如何交互。本页将重点转向显示代码架构。因此，有必要提醒读者：DC 中的代码与其他操作系统共享；为此，DC 提供了一组抽象和操作，用于把不同的 API 与硬件配置连接起来。可以把 DC 看作一项供显示管理器 (Display Manager，即 amdgpu_dm) 用来访问和配置 DCN/DCE 硬件的服务(DCE 也是 DC 的一部分，但为了简单起见，本文档只考察 DCN)。)

.. note::
   For this page, we will use the term GPU to refers to dGPU and APU.
   (在本页中，我们将使用术语 GPU 来指代 dGPU 和 APU。)

Overview(概述)
==============

From the display hardware perspective, it is plausible to expect that if a
problem is well-defined, it will probably be implemented at the hardware level.
On the other hand, when there are multiple ways of achieving something without
a very well-defined scope, the solution is usually implemented as a policy at
the DC level. In other words, some policies are defined in the DC core
(resource management, power optimization, image quality, etc.), and the others
implemented in hardware are enabled via DC configuration.
(从显示硬件的角度来看，可以合理地预期：如果一个问题定义良好，它很可能会在硬件层面实现。另一方面，当有多种方式可以实现某个目标且范围没有很好的界定时，解决方案通常作为策略在 DC 层面实现。换句话说，一些策略在 DC 核心中定义(资源管理、功耗优化、图像质量等)，而另一些在硬件中实现的策略则通过 DC 配置来启用。)

In terms of hardware management, DCN has multiple instances of the same block
(e.g., HUBP, DPP, MPC, etc), and during the driver execution, it might be
necessary to use some of these instances. The core has policies in place for
handling those instances. Regarding resource management, the DC objective is
quite simple: minimize the hardware shuffle when the driver performs some
actions. When the state changes from A to B, the transition is considered
easier to maneuver if the hardware resource is still used for the same set of
driver objects. Usually, adding and removing a resource to a `pipe_ctx` (more
details below) is not a problem; however, moving a resource from one `pipe_ctx`
to another should be avoided.
(在硬件管理方面，DCN 的同一个块有多个实例(例如 HUBP、DPP、MPC 等)，在驱动执行期间，可能需要使用其中一些实例。核心已有处理这些实例的策略。就资源管理而言，DC 的目标非常简单：在驱动执行某些操作时，尽量减少硬件的挪动 (shuffle)。当状态从 A 变为 B 时，如果硬件资源仍被同一组驱动对象使用，则认为这种转换更容易操作。通常，向 `pipe_ctx` 添加或移除资源(更多细节见下文)不是问题；但是，应避免把资源从一个 `pipe_ctx` 移到另一个。)

Another area of influence for DC is power optimization, which has a myriad of
arrangement possibilities. In some way, just displaying an image via DCN should
be relatively straightforward; however, showing it with the best power
footprint is more desirable, but it has many associated challenges.
Unfortunately, there is no straight-forward analytic way to determine if a
configuration is the best one for the context due to the enormous variety of
variables related to this problem (e.g., many different DCN/DCE hardware
versions, different displays configurations, etc.) for this reason DC
implements a dedicated library for trying some configuration and verify if it
is possible to support it or not. This type of policy is extremely complex to
create and maintain, and amdgpu driver relies on Display Mode Library (DML) to
generate the best decisions.
(DC 影响的另一个领域是功耗优化，它有无数种可能的安排。从某种意义上说，仅通过 DCN 显示一幅图像应该相对直接；然而，以最佳功耗表现来显示它则更可取，但这伴随着许多挑战。遗憾的是，由于与该问题相关的变量繁多(例如，许多不同的 DCN/DCE 硬件版本、不同的显示器配置等)，没有直接的分析方法来确定某个配置对当前上下文而言是否最佳。因此，DC 实现了一个专用库，用于尝试某些配置并验证是否可以支持。这种类型的策略创建和维护起来极其复杂，amdgpu 驱动依赖显示模式库 (Display Mode Library，DML) 来生成最佳决策。)

In summary, DC must deal with the complexity of handling multiple scenarios and
determine policies to manage them. All of the above information is conveyed to
give the reader some idea about the complexity of driving a display from the
driver's perspective. This page hopes to allow the reader to better navigate
over the amdgpu display code.
(总之，DC 必须应对处理多种场景的复杂性，并确定管理它们的策略。提供以上所有信息，是为了让读者对从驱动角度驱动一台显示器所需的复杂性有一些概念。本页希望帮助读者更好地浏览 amdgpu 显示代码。)

Display Driver Architecture Overview(显示驱动架构概述)
======================================================

The diagram below provides an overview of the display driver architecture;
notice it illustrates the software layers adopted by DC:
(下图提供了显示驱动架构的概览；注意它展示了 DC 采用的软件层次：)

.. kernel-figure:: dc-components.svg

The first layer of the diagram is the high-level DC API represented by the
`dc.h` file; below it are two big blocks represented by Core and Link. Next is
the hardware configuration block; the main file describing it is
the`hw_sequencer.h`, where the implementation of the callbacks can be found in
the hardware sequencer folder. Almost at the end, you can see the block level
API (`dc/inc/hw`), which represents each DCN low-level block, such as HUBP,
DPP, MPC, OPTC, etc. Notice on the left side of the diagram that we have a
different set of layers representing the interaction with the DMUB
microcontroller.
(图的第一层是由 `dc.h` 文件代表的高层 DC API；其下是由 Core 和 Link 代表的两个大块。接下来是硬件配置块；描述它的主要文件是 `hw_sequencer.h`，其回调的实现可以在硬件 sequencer 文件夹中找到。几乎在最后，你可以看到块级 API (`dc/inc/hw`)，它代表每个 DCN 低层块，例如 HUBP、DPP、MPC、OPTC 等。注意在图的左侧，我们有一组不同的层次，代表与 DMUB 微控制器的交互。)

Basic Objects(基本对象)
-----------------------

The below diagram outlines the basic display objects. In particular, pay
attention to the names in the boxes since they represent a data structure in
the driver:
(下面的图勾勒了基本的显示对象。请特别留意方框中的名称，因为它们代表驱动中的数据结构：)

.. kernel-figure:: dc-arch-overview.svg

Let's start with the central block in the image, `dc`. The `dc` struct is
initialized per GPU; for example, one GPU has one `dc` instance, two GPUs have
two `dc` instances, and so forth. In other words we have one 'dc' per 'amdgpu'
instance. In some ways, this object behaves like the `Singleton` pattern.
(让我们从图中的中心块 `dc` 开始。 `dc` 结构按 GPU 初始化；例如，一个 GPU 有一个 `dc` 实例，两个 GPU 有两个 `dc` 实例，依此类推。换句话说，每个 'amdgpu' 实例有一个 'dc'。在某些方面，这个对象的行为类似于 `Singleton` (单例) 模式。)

After the `dc` block in the diagram, you can see the `dc_link` component, which
is a low-level abstraction for the connector. One interesting aspect of the
image is that connectors are not part of the DCN block; they are defined by the
platform/board and not by the SoC. The `dc_link` struct is the high-level data
container with information such as connected sinks, connection status, signal
types, etc. After `dc_link`, there is the `dc_sink`, which is the object that
represents the connected display.
(在图中的 `dc` 块之后，你可以看到 `dc_link` 组件，它是连接器 (connector) 的低层抽象。这张图的一个有趣之处是，连接器不属于 DCN 块；它们由平台/开发板定义，而不是由 SoC 定义。 `dc_link` 结构是高层的数据容器，包含诸如已连接的 sink、连接状态、信号类型等信息。在 `dc_link` 之后是 `dc_sink`，它是代表已连接显示器的对象。)

.. note::
   For historical reasons, we used the name `dc_link`, which gives the
   wrong impression that this abstraction only deals with physical connections
   that the developer can easily manipulate. However, this also covers
   connections like eDP or cases where the output is connected to other devices.
   (由于历史原因，我们使用了 `dc_link` 这个名字，这给人错误的印象，以为
   这个抽象只处理开发者可以轻松操作的物理连接。然而，它也涵盖诸如 eDP
   之类的连接，或输出连接到其他设备的情况。)

There are two structs that are not represented in the diagram since they were
elaborated in the DCN overview page  (check the DCN block diagram :ref:`Display
Core Next (DCN) <dcn_overview>`); still, it is worth bringing back for this
overview which is `dc_stream` and `dc_state`. The `dc_stream` stores many
properties associated with the data transmission, but most importantly, it
represents the data flow from the connector to the display. Next we have
`dc_state`, which represents the logic state within the hardware at the moment;
`dc_state` is composed of `dc_stream` and `dc_plane`. The `dc_stream` is the DC
version of `drm_crtc` and represents the post-blending pipeline.
(有两个结构未在图中表示，因为它们已在 DCN 概述页面中详细阐述(参见 DCN 框图 :ref:`Display Core Next (DCN) <dcn_overview>`)；不过，在这个概述中仍值得回顾一下，它们是 `dc_stream` 和 `dc_state`。 `dc_stream` 存储许多与数据传输相关的属性，但最重要的是，它代表从连接器到显示器的数据流。接下来是 `dc_state`，它表示此刻硬件内部的逻辑状态； `dc_state` 由 `dc_stream` 和 `dc_plane` 组成。 `dc_stream` 是 `drm_crtc` 的 DC 版本，代表混合 (blending) 之后的流水线。)

Speaking of the `dc_plane` data structure (first part of the diagram), you can
think about it as an abstraction similar to `drm_plane` that represents the
pre-blending portion of the pipeline. This image was probably processed by GFX
and is ready to be composited under a `dc_stream`. Normally, the driver may
have one or more `dc_plane` connected to the same `dc_stream`, which defines a
composition at the DC level.
(说到 `dc_plane` 数据结构(图的第一部分)，你可以把它想象成一个类似于 `drm_plane` 的抽象，代表流水线中混合之前的部分。这幅图像可能已经由 GFX 处理，准备好在一个 `dc_stream` 之下进行合成。通常，驱动可能有一个或多个 `dc_plane` 连接到同一个 `dc_stream`，这定义了 DC 层面的一次合成。)

Basic Operations(基本操作)
--------------------------

Now that we have covered the basic objects, it is time to examine some of the
basic hardware/software operations. Let's start with the `dc_create()`
function, which directly works with the `dc` data struct; this function behaves
like a constructor responsible for the basic software initialization and
preparing for enabling other parts of the API. It is important to highlight
that this operation does not touch any hardware configuration; it is only a
software initialization.
(在了解了基本对象之后，是时候考察一些基本的硬件/软件操作了。让我们从 `dc_create()` 函数开始，它直接与 `dc` 数据结构打交道；这个函数的行为类似于构造函数，负责基本的软件初始化并为启用 API 的其他部分做准备。需要强调的是，这个操作不会触碰任何硬件配置；它只是软件初始化。)

Next, we have the `dc_hardware_init()`, which also relies on the `dc` data
struct. Its main function is to put the hardware in a valid state. It is worth
highlighting that the hardware might initialize in an unknown state, and it is
a requirement to put it in a valid state; this function has multiple callbacks
for the hardware-specific initialization, whereas `dc_hardware_init` does the
hardware initialization and is the first point where we touch hardware.
(接下来是 `dc_hardware_init()`，它同样依赖于 `dc` 数据结构。它的主要功能是把硬件置于有效状态。值得强调的是，硬件可能以未知状态完成初始化，因此必须把它置于有效状态；该函数有多个用于硬件特定初始化的回调，而 `dc_hardware_init` 执行硬件初始化，是我们第一次触碰硬件的地方。)

The `dc_get_link_at_index` is an operation that depends on the `dc_link` data
structure. This function retrieves and enumerates all the `dc_links` available
on the device; this is required since this information is not part of the SoC
definition but depends on the board configuration. As soon as the `dc_link` is
initialized, it is useful to figure out if any of them are already connected to
the display by using the `dc_link_detect()` function. After the driver figures
out if any display is connected to the device, the challenging phase starts:
configuring the monitor to show something. Nonetheless, dealing with the ideal
configuration is not a DC task since this is the Display Manager (`amdgpu_dm`)
responsibility which in turn is responsible for dealing with the atomic
commits. The only interface DC provides to the configuration phase is the
function `dc_validate_with_context` that receives the configuration information
and, based on that, validates whether the hardware can support it or not. It is
important to add that even if the display supports some specific configuration,
it does not mean the DCN hardware can support it.
( `dc_get_link_at_index` 是一个依赖于 `dc_link` 数据结构的操作。此函数检索并枚举设备上所有可用的 `dc_links`；这是必需的，因为此信息不是 SoC 定义的一部分，而是取决于开发板配置。一旦 `dc_link` 被初始化，就可以使用 `dc_link_detect()` 函数来判断它们之中是否有已经连接到显示器的。在驱动弄清是否有显示器连接到设备之后，具有挑战性的阶段开始了：配置显示器以显示内容。尽管如此，处理理想配置并不是 DC 的任务，因为这是显示管理器 (`amdgpu_dm`) 的职责，后者又负责处理原子提交 (atomic commit)。DC 为配置阶段提供的唯一接口是函数 `dc_validate_with_context`，它接收配置信息，并据此验证硬件是否可以支持。需要补充的是，即使显示器支持某个特定配置，也不意味着 DCN 硬件可以支持它。)

After the DM and DC agree upon the configuration, the stream configuration
phase starts. This task activates one or more `dc_stream` at this phase, and in
the best-case scenario, you might be able to turn the display on with a black
screen (it does not show anything yet since it does not have any plane
associated with it). The final step would be to call the
`dc_update_planes_and_stream,` which will add or remove planes.
(在 DM 与 DC 就配置达成一致后，流 (stream) 配置阶段开始。此任务在这一阶段激活一个或多个 `dc_stream`；在最好的情况下，你可能能够点亮显示器并显示黑屏(它尚未显示任何内容，因为它还没有关联任何 plane)。最后一步是调用 `dc_update_planes_and_stream,`，它将添加或移除 plane。)

