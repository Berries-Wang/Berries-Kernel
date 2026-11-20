.. SPDX-License-Identifier: GPL-2.0

============================================
Linux Color Pipeline API(Linux 色彩管线 API)
============================================

What problem are we solving?(我们要解决什么问题？)
==================================================

We would like to support pre-, and post-blending complex color
transformations in display controller hardware in order to allow for
HW-supported HDR use-cases, as well as to provide support to
color-managed applications, such as video or image editors.
(我们希望支持在显示控制器硬件中进行混合前 (pre-blending) 与混合后
(post-blending) 的复杂色彩变换，以便支持硬件加速的 HDR 用例，同时也为
色彩管理型应用 (例如视频或图像编辑器) 提供支持。)

It is possible to support an HDR output on HW supporting the Colorspace
and HDR Metadata drm_connector properties, but that requires the
compositor or application to render and compose the content into one
final buffer intended for display. Doing so is costly.
(在支持 Colorspace 与 HDR Metadata 这两个 drm_connector 属性的硬件上
支持 HDR 输出是可能的，但这要求合成器或应用程序将内容渲染并合成为一个
用于显示的最终缓冲区。这样做代价高昂。)

Most modern display HW offers various 1D LUTs, 3D LUTs, matrices, and other
operations to support color transformations. These operations are often
implemented in fixed-function HW and therefore much more power efficient than
performing similar operations via shaders or CPU.
(大多数现代显示硬件提供各种 1D LUT、3D LUT、矩阵以及其他支持色彩变换的
操作。这些操作通常以固定功能硬件实现，因此比通过着色器或 CPU 执行类似
操作省电得多。)

We would like to make use of this HW functionality to support complex color
transformations with no, or minimal CPU or shader load. The switch between HW
fixed-function blocks and shaders/CPU must be seamless with no visible
difference when fallback to shaders/CPU is neceesary at any time.
(我们希望利用这一硬件功能来支持复杂的色彩变换，同时 CPU 或着色器负载为
零或极小。硬件固定功能块与着色器/CPU 之间的切换必须无缝，在任意时刻需要
回退到着色器/CPU 时都不应产生可见差异。)


How are other OSes solving this problem?(其他操作系统如何解决这一问题？)
========================================================================

The most widely supported use-cases regard HDR content, whether video or
gaming.
(获得最广泛支持的用例与 HDR 内容有关，无论是视频还是游戏。)

Most OSes will specify the source content format (color gamut, encoding transfer
function, and other metadata, such as max and average light levels) to a driver.
Drivers will then program their fixed-function HW accordingly to map from a
source content buffer's space to a display's space.
(大多数操作系统会向驱动指定源内容格式 (色域、编码传递函数以及其他元数据，
例如最大与平均亮度电平)。驱动随后据此编程其固定功能硬件，把源内容缓冲区
的色彩空间映射到显示器的色彩空间。)

When fixed-function HW is not available the compositor will assemble a shader to
ask the GPU to perform the transformation from the source content format to the
display's format.
(当固定功能硬件不可用时，合成器会组装一个着色器，让 GPU 执行从源内容格式
到显示器格式的变换。)

A compositor's mapping function and a driver's mapping function are usually
entirely separate concepts. On OSes where a HW vendor has no insight into
closed-source compositor code such a vendor will tune their color management
code to visually match the compositor's. On other OSes, where both mapping
functions are open to an implementer they will ensure both mappings match.
(合成器的映射函数与驱动的映射函数通常是完全独立的概念。在硬件厂商无法查看
闭源合成器代码的操作系统上，厂商会将自家色彩管理代码调校到与合成器在视觉
上一致。在其他操作系统上，两种映射函数都向实现者开放，他们会确保二者相互
吻合。)

This results in mapping algorithm lock-in, meaning that no-one alone can
experiment with or introduce new mapping algorithms and achieve
consistent results regardless of which implementation path is taken.
(这导致了映射算法的锁定，意味着没有任何一方能够独自试验或引入新的映射
算法，并在无论采用哪条实现路径的情况下都取得一致的结果。)

Why is Linux different?(Linux 为何不同？)
=========================================

Unlike other OSes, where there is one compositor for one or more drivers, on
Linux we have a many-to-many relationship. Many compositors; many drivers.
In addition each compositor vendor or community has their own view of how
color management should be done. This is what makes Linux so beautiful.
(与其他操作系统 (一个合成器对应一个或多个驱动) 不同，在 Linux 上我们是
多对多的关系：许多合成器，许多驱动。此外，每个合成器厂商或社区对色彩
管理应当如何进行都有各自的见解。这正是 Linux 如此美妙之处。)

This means that a HW vendor can now no longer tune their driver to one
compositor, as tuning it to one could make it look fairly different from
another compositor's color mapping.
(这意味着硬件厂商如今无法再针对单一合成器调校驱动，因为针对某一家的调校
可能使其与另一家合成器的色彩映射相差甚远。)

We need a better solution.
(我们需要一个更好的解决方案。)


Descriptive API(描述式 API)
===========================

An API that describes the source and destination colorspaces is a descriptive
API. It describes the input and output color spaces but does not describe
how precisely they should be mapped. Such a mapping includes many minute
design decision that can greatly affect the look of the final result.
(描述源与目标色彩空间的 API 是描述式 API。它描述输入与输出色彩空间，但不
描述它们应如何精确映射。这类映射包含许多细微的设计决策，会极大影响最终
结果的观感。)

It is not feasible to describe such mapping with enough detail to ensure the
same result from each implementation. In fact, these mappings are a very active
research area.
(要足够细致地描述这类映射、从而保证每种实现都得到相同结果，并不现实。
事实上，这些映射是一个非常活跃的研究领域。)


Prescriptive API(规定式 API)
============================

A prescriptive API describes not the source and destination colorspaces. It
instead prescribes a recipe for how to manipulate pixel values to arrive at the
desired outcome.
(规定式 API 并不描述源与目标色彩空间，而是规定一套操作像素值以达到预期
结果的方法。)

This recipe is generally an ordered list of straight-forward operations,
with clear mathematical definitions, such as 1D LUTs, 3D LUTs, matrices,
or other operations that can be described in a precise manner.
(这套方法通常是一系列直接明了、具有清晰数学定义的操作的有序列表，例如
1D LUT、3D LUT、矩阵，或其他能够精确描述的操作。)


The Color Pipeline API(色彩管线 API)
====================================

HW color management pipelines can significantly differ between HW
vendors in terms of availability, ordering, and capabilities of HW
blocks. This makes a common definition of color management blocks and
their ordering nigh impossible. Instead we are defining an API that
allows user space to discover the HW capabilities in a generic manner,
agnostic of specific drivers and hardware.
(不同硬件厂商的色彩管理管线在硬件块的可用性、顺序与能力方面可能差异
巨大。这使得对色彩管理块及其顺序做出统一定义几乎不可能。因此我们定义的
是一个让用户空间以通用方式发现硬件能力的 API，与具体驱动和硬件无关。)


drm_colorop Object(drm_colorop 对象)
====================================

To support the definition of color pipelines we define the DRM core
object type drm_colorop. Individual drm_colorop objects will be chained
via the NEXT property of a drm_colorop to constitute a color pipeline.
Each drm_colorop object is unique, i.e., even if multiple color
pipelines have the same operation they won't share the same drm_colorop
object to describe that operation.
(为支持色彩管线的定义，我们定义 DRM 核心对象类型 drm_colorop。各个
drm_colorop 对象通过 drm_colorop 的 NEXT 属性链接起来，构成一条色彩
管线。每个 drm_colorop 对象都是唯一的，即即使多条色彩管线拥有相同的
操作，它们也不会共享同一个 drm_colorop 对象来描述该操作。)

Note that drivers are not expected to map drm_colorop objects statically
to specific HW blocks. The mapping of drm_colorop objects is entirely a
driver-internal detail and can be as dynamic or static as a driver needs
it to be. See more in the Driver Implementation Guide section below.
(请注意，并不要求驱动将 drm_colorop 对象静态地映射到特定硬件块。
drm_colorop 对象的映射完全是驱动内部细节，可以按驱动需要任意动态或
静态。更多信息见下文的 Driver Implementation Guide 一节。)

Each drm_colorop has three core properties:
(每个 drm_colorop 都有三个核心属性：)

TYPE: An enumeration property, defining the type of transformation, such as
* enumerated curve
* custom (uniform) 1D LUT
* 3x3 matrix
* 3x4 matrix
* 3D LUT
* etc.
(TYPE：枚举属性，定义变换的类型，例如 enumerated curve (枚举曲线)、
custom (uniform) 1D LUT (自定义 (uniform) 1D LUT)、3x3 matrix (3x3
矩阵)、3x4 matrix (3x4 矩阵)、3D LUT、etc. (等等)。)

Depending on the type of transformation other properties will describe
more details.
(视变换类型而定，其他属性会描述更多细节。)

BYPASS: A boolean property that can be used to easily put a block into
bypass mode. The BYPASS property is not mandatory for a colorop, as long
as the entire pipeline can get bypassed by setting the COLOR_PIPELINE on
a plane to '0'.
(BYPASS：布尔属性，可用于轻松地把一个块置于旁路 (bypass) 模式。只要把
平面上的 COLOR_PIPELINE 设为 '0' 就能旁路整条管线，BYPASS 属性对
colorop 而言并非强制。)

NEXT: The ID of the next drm_colorop in a color pipeline, or 0 if this
drm_colorop is the last in the chain.
(NEXT：色彩管线中下一个 drm_colorop 的 ID，若该 drm_colorop 是链中
最后一个则为 0。)

An example of a drm_colorop object might look like one of these::

    /* 1D enumerated curve */
    Color operation 42
    ├─ "TYPE": immutable enum {1D enumerated curve, 1D LUT, 3x3 matrix, 3x4 matrix, 3D LUT, etc.} = 1D enumerated curve
    ├─ "BYPASS": bool {true, false}
    ├─ "CURVE_1D_TYPE": enum {sRGB EOTF, sRGB inverse EOTF, PQ EOTF, PQ inverse EOTF, …}
    └─ "NEXT": immutable color operation ID = 43

    /* custom 4k entry 1D LUT */
    Color operation 52
    ├─ "TYPE": immutable enum {1D enumerated curve, 1D LUT, 3x3 matrix, 3x4 matrix, 3D LUT, etc.} = 1D LUT
    ├─ "BYPASS": bool {true, false}
    ├─ "SIZE": immutable range = 4096
    ├─ "DATA": blob
    └─ "NEXT": immutable color operation ID = 0

    /* 17^3 3D LUT */
    Color operation 72
    ├─ "TYPE": immutable enum {1D enumerated curve, 1D LUT, 3x3 matrix, 3x4 matrix, 3D LUT, etc.} = 3D LUT
    ├─ "BYPASS": bool {true, false}
    ├─ "SIZE": immutable range = 17
    ├─ "DATA": blob
    └─ "NEXT": immutable color operation ID = 73

(一个 drm_colorop 对象的示例可能形如下列之一：)

drm_colorop extensibility(drm_colorop 的可扩展性)
-------------------------------------------------

Unlike existing DRM core objects, like &drm_plane, drm_colorop is not
extensible. This simplifies implementations and keeps all functionality
for managing &drm_colorop objects in the DRM core.
(与现有 DRM 核心对象 (如 &drm_plane) 不同，drm_colorop 不可扩展。这
简化了实现，并把管理 &drm_colorop 对象的全部功能保留在 DRM 核心中。)

If there is a need one may introduce a simple &drm_colorop_funcs
function table in the future, for example to support an IN_FORMATS
property on a &drm_colorop.
(如有需要，将来可以引入一个简单的 &drm_colorop_funcs 函数表，例如用于
支持 &drm_colorop 上的 IN_FORMATS 属性。)

If a driver requires the ability to create a driver-specific colorop
object they will need to add &drm_colorop func table support with
support for the usual functions, like destroy, atomic_duplicate_state,
and atomic_destroy_state.
(如果驱动需要创建驱动专属 colorop 对象的能力，则需要添加 &drm_colorop
函数表支持，并实现常用的函数，如 destroy、atomic_duplicate_state 和
atomic_destroy_state。)


COLOR_PIPELINE Plane Property(COLOR_PIPELINE 平面属性)
======================================================

Color Pipelines are created by a driver and advertised via a new
COLOR_PIPELINE enum property on each plane. Values of the property
always include object id 0, which is the default and means all color
processing is disabled. Additional values will be the object IDs of the
first drm_colorop in a pipeline. A driver can create and advertise none,
one, or more possible color pipelines. A DRM client will select a color
pipeline by setting the COLOR PIPELINE to the respective value.
(色彩管线由驱动创建，并通过每个平面上新增的 COLOR_PIPELINE 枚举属性对外
通告。该属性的取值总是包含对象 ID 0，即默认值，表示所有色彩处理都被
禁用。其余取值是管线中第一个 drm_colorop 的对象 ID。驱动可以创建并通告
零个、一个或多个可用的色彩管线。DRM 客户端通过把 COLOR PIPELINE 设为
相应取值来选择一条色彩管线。)

NOTE: Many DRM clients will set enumeration properties via the string
value, often hard-coding it. Since this enumeration is generated based
on the colorop object IDs it is important to perform the Color Pipeline
Discovery, described below, instead of hard-coding color pipeline
assignment. Drivers might generate the enum strings dynamically.
Hard-coded strings might only work for specific drivers on a specific
pieces of HW. Color Pipeline Discovery can work universally, as long as
drivers implement the required color operations.
(NOTE：许多 DRM 客户端会通过字符串取值设置枚举属性，并且常常将其硬编码。
由于这一枚举是依据 colorop 对象 ID 生成的，重要的一点是执行下文所述的
Color Pipeline Discovery (色彩管线发现)，而不是硬编码色彩管线的指派。
驱动可能动态生成枚举字符串。硬编码的字符串可能只对特定驱动、特定硬件
有效。只要驱动实现了所需的色彩操作，Color Pipeline Discovery 就能通用
适用。)

The COLOR_PIPELINE property is only exposed when the
DRM_CLIENT_CAP_PLANE_COLOR_PIPELINE is set. Drivers shall ignore any
existing pre-blend color operations when this cap is set, such as
COLOR_RANGE and COLOR_ENCODING. If drivers want to support COLOR_RANGE
or COLOR_ENCODING functionality when the color pipeline client cap is
set, they are expected to expose colorops in the pipeline to allow for
the appropriate color transformation.
(仅当设置了 DRM_CLIENT_CAP_PLANE_COLOR_PIPELINE 时才会暴露
COLOR_PIPELINE 属性。设置该 cap 后，驱动应忽略既有的混合前色彩操作，
例如 COLOR_RANGE 与 COLOR_ENCODING。如果驱动希望在设置了色彩管线客户端
cap 的情况下仍支持 COLOR_RANGE 或 COLOR_ENCODING 功能，则应在管线中
暴露相应的 colorop，以实现适当的色彩变换。)

Setting of the COLOR_PIPELINE plane property or drm_colorop properties
is only allowed for userspace that sets this client cap.
(只有设置了该客户端 cap 的用户空间才允许设置 COLOR_PIPELINE 平面属性或
drm_colorop 属性。)

An example of a COLOR_PIPELINE property on a plane might look like this::

    Plane 10
    ├─ "TYPE": immutable enum {Overlay, Primary, Cursor} = Primary
    ├─ …
    └─ "COLOR_PIPELINE": enum {0, 42, 52} = 0

(平面上 COLOR_PIPELINE 属性的一个示例如下：)


Color Pipeline Discovery(色彩管线发现)
======================================

A DRM client wanting color management on a drm_plane will:
(想要在 drm_plane 上进行色彩管理的 DRM 客户端会：)

1. Get the COLOR_PIPELINE property of the plane
   (获取该平面的 COLOR_PIPELINE 属性)
2. iterate all COLOR_PIPELINE enum values
   (遍历 COLOR_PIPELINE 的所有枚举取值)
3. for each enum value walk the color pipeline (via the NEXT pointers)
   and see if the available color operations are suitable for the
   desired color management operations
   (对每个枚举取值，沿色彩管线 (通过 NEXT 指针) 遍历，检查可用的色彩
   操作是否适合所需的色彩管理操作)

If userspace encounters an unknown or unsuitable color operation during
discovery it does not need to reject the entire color pipeline outright,
as long as the unknown or unsuitable colorop has a "BYPASS" property.
Drivers will ensure that a bypassed block does not have any effect.
(如果用户空间在发现过程中遇到未知或不适合的色彩操作，只要该未知或不适合
的 colorop 带有 "BYPASS" 属性，就无需直接拒绝整条色彩管线。驱动会确保被
旁路的块不产生任何效果。)

An example of chained properties to define an AMD pre-blending color
pipeline might look like this::

    Plane 10
    ├─ "TYPE" (immutable) = Primary
    └─ "COLOR_PIPELINE": enum {0, 44} = 0

    Color operation 44
    ├─ "TYPE" (immutable) = 1D enumerated curve
    ├─ "BYPASS": bool
    ├─ "CURVE_1D_TYPE": enum {sRGB EOTF, PQ EOTF} = sRGB EOTF
    └─ "NEXT" (immutable) = 45

    Color operation 45
    ├─ "TYPE" (immutable) = 3x4 Matrix
    ├─ "BYPASS": bool
    ├─ "DATA": blob
    └─ "NEXT" (immutable) = 46

    Color operation 46
    ├─ "TYPE" (immutable) = 1D enumerated curve
    ├─ "BYPASS": bool
    ├─ "CURVE_1D_TYPE": enum {sRGB Inverse EOTF, PQ Inverse EOTF} = sRGB EOTF
    └─ "NEXT" (immutable) = 47

    Color operation 47
    ├─ "TYPE" (immutable) = 1D LUT
    ├─ "SIZE": immutable range = 4096
    ├─ "DATA": blob
    └─ "NEXT" (immutable) = 48

    Color operation 48
    ├─ "TYPE" (immutable) = 3D LUT
    ├─ "DATA": blob
    └─ "NEXT" (immutable) = 49

    Color operation 49
    ├─ "TYPE" (immutable) = 1D enumerated curve
    ├─ "BYPASS": bool
    ├─ "CURVE_1D_TYPE": enum {sRGB EOTF, PQ EOTF} = sRGB EOTF
    └─ "NEXT" (immutable) = 0

(一个用链式属性定义 AMD 混合前色彩管线的示例可能形如下：)


Color Pipeline Programming(色彩管线编程)
========================================

Once a DRM client has found a suitable pipeline it will:
(DRM 客户端找到合适的管线后会：)

1. Set the COLOR_PIPELINE enum value to the one pointing at the first
   drm_colorop object of the desired pipeline
   (把 COLOR_PIPELINE 枚举取值设为指向所需管线第一个 drm_colorop 对象的
   那个取值)
2. Set the properties for all drm_colorop objects in the pipeline to the
   desired values, setting BYPASS to true for unused drm_colorop blocks,
   and false for enabled drm_colorop blocks
   (把管线中所有 drm_colorop 对象的属性设为期望值：未使用的 drm_colorop
   块把 BYPASS 设为 true，启用的 drm_colorop 块设为 false)
3. Perform (TEST_ONLY or not) atomic commit with all the other KMS
   states it wishes to change
   (连同其他希望修改的 KMS 状态一起执行 (或不执行 TEST_ONLY 的) 原子提交)

To configure the pipeline for an HDR10 PQ plane and blending in linear
space, a compositor might perform an atomic commit with the following
property values::

    Plane 10
    └─ "COLOR_PIPELINE" = 42

    Color operation 42
    └─ "BYPASS" = true

    Color operation 44
    └─ "BYPASS" = true

    Color operation 45
    └─ "BYPASS" = true

    Color operation 46
    └─ "BYPASS" = true

    Color operation 47
    ├─ "DATA" = Gamut mapping + tone mapping + night mode
    └─ "BYPASS" = false

    Color operation 48
    ├─ "CURVE_1D_TYPE" = PQ EOTF
    └─ "BYPASS" = false

(为了给 HDR10 PQ 平面配置管线并在线性空间中混合，合成器可能以如下属性
取值执行原子提交：)


Driver Implementer's Guide(驱动实现者指南)
==========================================

What does this all mean for driver implementations? As noted above the
colorops can map to HW directly but don't need to do so. Here are some
suggestions on how to think about creating your color pipelines:
(这一切对驱动实现意味着什么？如上所述，colorop 可以直接映射到硬件，但
并非必须。以下是关于如何构思创建你的色彩管线的一些建议：)

- Try to expose pipelines that use already defined colorops, even if
  your hardware pipeline is split differently. This allows existing
  userspace to immediately take advantage of the hardware.
  (尽量暴露使用已定义 colorop 的管线，即使你的硬件管线划分方式不同。
  这能让现有用户空间立即利用上硬件。)

- Additionally, try to expose your actual hardware blocks as colorops.
  Define new colorop types where you believe it can offer significant
  benefits if userspace learns to program them.
  (另外，尽量把你的实际硬件块作为 colorop 暴露出来。在你认为用户空间
  学会编程它们能带来显著收益的地方，定义新的 colorop 类型。)

- Avoid defining new colorops for compound operations with very narrow
  scope. If you have a hardware block for a special operation that
  cannot be split further, you can expose that as a new colorop type.
  However, try to not define colorops for "use cases", especially if
  they require you to combine multiple hardware blocks.
  (避免为适用范围极窄的复合操作定义新 colorop。如果你有一个执行无法
  进一步拆分的特殊操作的硬件块，可以将其作为一种新 colorop 类型暴露。
  但尽量不要为 "use cases" (用例) 定义 colorop，尤其是当这需要组合
  多个硬件块时。)

- Design new colorops as prescriptive, not descriptive; by the
  mathematical formula, not by the assumed input and output.
  (把新 colorop 设计成规定式而非描述式：按数学公式，而不是按假定的
  输入与输出。)

A defined colorop type must be deterministic. The exact behavior of the
colorop must be documented entirely, whether via a mathematical formula
or some other description. Its operation can depend only on its
properties and input and nothing else, allowed error tolerance
notwithstanding.
(已定义的 colorop 类型必须是确定性的。colorop 的确切行为必须被完整地
文档化，无论是通过数学公式还是其他描述。其操作只能取决于其属性与输入，
别无其他——允许的误差容限除外。)


Driver Forward/Backward Compatibility(驱动前向/向后兼容性)
==========================================================

As this is uAPI drivers can't regress color pipelines that have been
introduced for a given HW generation. New HW generations are free to
abandon color pipelines advertised for previous generations.
Nevertheless, it can be beneficial to carry support for existing color
pipelines forward as those will likely already have support in DRM
clients.
(由于这是 uAPI，驱动不能让为某一硬件世代引入的色彩管线发生回退。新的
硬件世代可以随意放弃为上一代硬件通告的色彩管线。尽管如此，继续支持既有
色彩管线仍可能有益，因为 DRM 客户端很可能早已支持它们。)

Introducing new colorops to a pipeline is fine, as long as they can be
bypassed or are purely informational. DRM clients implementing support
for the pipeline can always skip unknown properties as long as they can
be confident that doing so will not cause unexpected results.
(在管线中引入新 colorop 没有问题，只要它们可以被旁路或纯粹是信息性的。
实现该管线支持的 DRM 客户端总是可以跳过未知属性，只要它们确信这样做
不会导致意外结果。)

If a new colorop doesn't fall into one of the above categories
(bypassable or informational) the modified pipeline would be unusable
for user space. In this case a new pipeline should be defined.
(如果新 colorop 不属于上述类别之一 (可旁路或信息性)，修改后的管线对
用户空间将不可用。此时应定义一条新管线。)


References(参考资料)
====================

1. https://lore.kernel.org/dri-devel/QMers3awXvNCQlyhWdTtsPwkp5ie9bze_hD5nAccFW7a_RXlWjYB7MoUW_8CKLT2bSQwIXVi5H6VULYIxCdgvryZoAoJnC5lZgyK1QWn488=@emersion.fr/
