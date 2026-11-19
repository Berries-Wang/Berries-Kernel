=============================================
Kernel Mode Setting (KMS)(内核模式设置 (KMS))
=============================================

Drivers must initialize the mode setting core by calling
drmm_mode_config_init() on the DRM device. The function
initializes the :c:type:`struct drm_device <drm_device>`
mode_config field and never fails. Once done, mode configuration must
be setup by initializing the following fields.
(驱动必须通过在 DRM 设备上调用 drmm_mode_config_init() 来初始化模式设置
核心。该函数初始化 :c:type:`struct drm_device <drm_device>` 的 mode_config
字段且从不失败。完成后，必须通过初始化以下字段来设置模式配置。)

-  int min_width, min_height; int max_width, max_height;
   Minimum and maximum width and height of the frame buffers in pixel
   units.
   (帧缓冲的最小与最大宽度和高度，以像素为单位。)

-  struct drm_mode_config_funcs \*funcs;
   Mode setting functions.
   (模式设置函数。)

Overview(概述)
==============

.. kernel-render:: DOT
   :alt: KMS Display Pipeline
   :caption: KMS Display Pipeline Overview

   digraph "KMS" {
      node [shape=box]

      subgraph cluster_static {
          style=dashed
          label="Static Objects"

          node [bgcolor=grey style=filled]
          "drm_plane A" -> "drm_crtc"
          "drm_plane B" -> "drm_crtc"
          "drm_crtc" -> "drm_encoder A"
          "drm_crtc" -> "drm_encoder B"
      }

      subgraph cluster_user_created {
          style=dashed
          label="Userspace-Created"

          node [shape=oval]
          "drm_framebuffer 1" -> "drm_plane A"
          "drm_framebuffer 2" -> "drm_plane B"
      }

      subgraph cluster_connector {
          style=dashed
          label="Hotpluggable"

          "drm_encoder A" -> "drm_connector A"
          "drm_encoder B" -> "drm_connector B"
      }
   }

The basic object structure KMS presents to userspace is fairly simple.
Framebuffers (represented by :c:type:`struct drm_framebuffer <drm_framebuffer>`,
see `Frame Buffer Abstraction(帧缓冲抽象)`_) feed into planes. Planes are represented by
:c:type:`struct drm_plane <drm_plane>`, see `Plane Abstraction(Plane 抽象)`_ for more
details. One or more (or even no) planes feed their pixel data into a CRTC
(represented by :c:type:`struct drm_crtc <drm_crtc>`, see `CRTC Abstraction(CRTC 抽象)`_)
for blending. The precise blending step is explained in more detail in `Plane
Composition Properties(Plane 合成属性)`_ and related chapters.
(KMS 呈现给用户空间的基本对象结构相当简单。帧缓冲（由
:c:type:`struct drm_framebuffer <drm_framebuffer>` 表示，参见
`Frame Buffer Abstraction(帧缓冲抽象)`_）输入到 plane。plane 由
:c:type:`struct drm_plane <drm_plane>` 表示，更多细节参见
`Plane Abstraction(Plane 抽象)`_。一个或多个（甚至没有）plane 把它们的像素
数据输入到一个 CRTC（由 :c:type:`struct drm_crtc <drm_crtc>` 表示，参见
`CRTC Abstraction(CRTC 抽象)`_）进行混合。精确的混合步骤在
`Plane Composition Properties(Plane 合成属性)`_ 及相关章节中有更详细的
解释。)

For the output routing the first step is encoders (represented by
:c:type:`struct drm_encoder <drm_encoder>`, see `Encoder Abstraction(Encoder 抽象)`_). Those
are really just internal artifacts of the helper libraries used to implement KMS
drivers. Besides that they make it unnecessarily more complicated for userspace
to figure out which connections between a CRTC and a connector are possible, and
what kind of cloning is supported, they serve no purpose in the userspace API.
Unfortunately encoders have been exposed to userspace, hence can't remove them
at this point.  Furthermore the exposed restrictions are often wrongly set by
drivers, and in many cases not powerful enough to express the real restrictions.
A CRTC can be connected to multiple encoders, and for an active CRTC there must
be at least one encoder.
(对于输出路由，第一步是 encoder（由 :c:type:`struct drm_encoder
<drm_encoder>` 表示，参见 `Encoder Abstraction(Encoder 抽象)`_）。它们实际上
只是用于实现 KMS 驱动的辅助库的内部产物。除了让用户空间不必要地更难弄清
CRTC 与 connector 之间哪些连接是可能的、支持何种克隆之外，它们在用户空间
API 中没有任何用处。遗憾的是 encoder 已经暴露给用户空间，因此此时无法移除
它们。此外，暴露的限制常常被驱动错误地设置，而且在很多情况下不足以表达真实
的限制。一个 CRTC 可以连接多个 encoder，而对于活跃的 CRTC 必须至少有一个
encoder。)

The final, and real, endpoint in the display chain is the connector (represented
by :c:type:`struct drm_connector <drm_connector>`, see `Connector
Abstraction(Connector 抽象)`_). Connectors can have different possible encoders, but the kernel
driver selects which encoder to use for each connector. The use case is DVI,
which could switch between an analog and a digital encoder. Encoders can also
drive multiple different connectors. There is exactly one active connector for
every active encoder.
(显示链路中最终的、真实的端点是 connector（由 :c:type:`struct
drm_connector <drm_connector>` 表示，参见
`Connector Abstraction(Connector 抽象)`_）。connector 可以有不同的候选
encoder，但内核驱动为每个 connector 选择使用哪个 encoder。使用场景是 DVI，
它可以在模拟 encoder 与数字 encoder 之间切换。encoder 也可以驱动多个不同
的 connector。每个活跃的 encoder 恰好有一个活跃的 connector。)

Internally the output pipeline is a bit more complex and matches today's
hardware more closely:
(在内部，输出管线要更复杂一些，也更贴近如今的硬件：)

.. kernel-render:: DOT
   :alt: KMS Output Pipeline
   :caption: KMS Output Pipeline

   digraph "Output Pipeline" {
      node [shape=box]

      subgraph {
          "drm_crtc" [bgcolor=grey style=filled]
      }

      subgraph cluster_internal {
          style=dashed
          label="Internal Pipeline"
          {
              node [bgcolor=grey style=filled]
              "drm_encoder A";
              "drm_encoder B";
              "drm_encoder C";
          }

          {
              node [bgcolor=grey style=filled]
              "drm_encoder B" -> "drm_bridge B"
              "drm_encoder C" -> "drm_bridge C1"
              "drm_bridge C1" -> "drm_bridge C2";
          }
      }

      "drm_crtc" -> "drm_encoder A"
      "drm_crtc" -> "drm_encoder B"
      "drm_crtc" -> "drm_encoder C"


      subgraph cluster_output {
          style=dashed
          label="Outputs"

          "drm_encoder A" -> "drm_connector A";
          "drm_bridge B" -> "drm_connector B";
          "drm_bridge C2" -> "drm_connector C";

          "drm_panel"
      }
   }

Internally two additional helper objects come into play. First, to be able to
share code for encoders (sometimes on the same SoC, sometimes off-chip) one or
more :ref:`drm_bridges` (represented by :c:type:`struct drm_bridge
<drm_bridge>`) can be linked to an encoder. This link is static and cannot be
changed, which means the cross-bar (if there is any) needs to be mapped between
the CRTC and any encoders. Often for drivers with bridges there's no code left
at the encoder level. Atomic drivers can leave out all the encoder callbacks to
essentially only leave a dummy routing object behind, which is needed for
backwards compatibility since encoders are exposed to userspace.
(在内部还有两个额外的辅助对象。首先，为了能够在多个 encoder（有时在同一
SoC 上，有时在片外）之间共享代码，一个或多个 :ref:`drm_bridges`（由
:c:type:`struct drm_bridge <drm_bridge>` 表示）可以链接到一个 encoder。
此链接是静态的、不可更改的，这意味着交叉开关 (cross-bar)（如果存在）需要
映射在 CRTC 与任何 encoder 之间。对于带 bridge 的驱动，encoder 层级往往
已经没有剩余代码。原子驱动可以省略所有 encoder 回调，实质上只留下一个哑
路由对象——由于 encoder 暴露给用户空间，为了向后兼容需要保留它。)

The second object is for panels, represented by :c:type:`struct drm_panel
<drm_panel>`, see :ref:`drm_panel_helper`. Panels do not have a fixed binding
point, but are generally linked to the driver private structure that embeds
:c:type:`struct drm_connector <drm_connector>`.
(第二个对象用于 panel，由 :c:type:`struct drm_panel <drm_panel>` 表示，
参见 :ref:`drm_panel_helper`。panel 没有固定的绑定位置，通常链接到嵌入了
:c:type:`struct drm_connector <drm_connector>` 的驱动私有结构。)

Note that currently the bridge chaining and interactions with connectors and
panels are still in-flux and not really fully sorted out yet.
(注意，目前 bridge 链接以及与 connector 和 panel 的交互仍在变化中，还没有
真正完全理清。)

KMS Core Structures and Functions(KMS 核心结构与函数)
=====================================================

.. kernel-doc:: include/drm/drm_mode_config.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_mode_config.c
   :export:

.. _kms_base_object_abstraction:

Modeset Base Object Abstraction(模式设置基础对象抽象)
=====================================================

.. kernel-render:: DOT
   :alt: Mode Objects and Properties
   :caption: Mode Objects and Properties

   digraph {
      node [shape=box]

      "drm_property A" -> "drm_mode_object A"
      "drm_property A" -> "drm_mode_object B"
      "drm_property B" -> "drm_mode_object A"
   }

The base structure for all KMS objects is :c:type:`struct drm_mode_object
<drm_mode_object>`. One of the base services it provides is tracking properties,
which are especially important for the atomic IOCTL (see `Atomic Mode
Setting(原子模式设置)`_). The somewhat surprising part here is that properties are not
directly instantiated on each object, but free-standing mode objects themselves,
represented by :c:type:`struct drm_property <drm_property>`, which only specify
the type and value range of a property. Any given property can be attached
multiple times to different objects using drm_object_attach_property().
(所有 KMS 对象的基础结构是 :c:type:`struct drm_mode_object
<drm_mode_object>`。它提供的基本服务之一是跟踪属性 (property)，这对原子
IOCTL 尤其重要（参见 `Atomic Mode Setting(原子模式设置)`_）。这里有点令人
意外的是，属性并不直接实例化在每个对象上，而是自由独立 (free-standing) 的
模式对象本身，由 :c:type:`struct drm_property <drm_property>` 表示，它只
规定属性的类型与取值范围。任何给定的属性都可以用 drm_object_attach_property()
多次附加到不同的对象上。)

.. kernel-doc:: include/drm/drm_mode_object.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_mode_object.c
   :export:

Atomic Mode Setting(原子模式设置)
=================================


.. kernel-render:: DOT
   :alt: Mode Objects and Properties
   :caption: Mode Objects and Properties

   digraph {
      node [shape=box]

      subgraph cluster_state {
          style=dashed
          label="Free-standing state"

          "drm_atomic_state" -> "duplicated drm_plane_state A"
          "drm_atomic_state" -> "duplicated drm_plane_state B"
          "drm_atomic_state" -> "duplicated drm_crtc_state"
          "drm_atomic_state" -> "duplicated drm_connector_state"
          "drm_atomic_state" -> "duplicated driver private state"
      }

      subgraph cluster_current {
          style=dashed
          label="Current state"

          "drm_device" -> "drm_plane A"
          "drm_device" -> "drm_plane B"
          "drm_device" -> "drm_crtc"
          "drm_device" -> "drm_connector"
          "drm_device" -> "driver private object"

          "drm_plane A" -> "drm_plane_state A"
          "drm_plane B" -> "drm_plane_state B"
          "drm_crtc" -> "drm_crtc_state"
          "drm_connector" -> "drm_connector_state"
          "driver private object" -> "driver private state"
      }

      "drm_atomic_state" -> "drm_device" [label="atomic_commit"]
      "duplicated drm_plane_state A" -> "drm_device"[style=invis]
   }

Atomic provides transactional modeset (including planes) updates, but a
bit differently from the usual transactional approach of try-commit and
rollback:
(Atomic 提供事务式的模式设置（包括 plane）更新，但与通常的"尝试-提交-回滚"
事务方法略有不同：)

- Firstly, no hardware changes are allowed when the commit would fail. This
  allows us to implement the DRM_MODE_ATOMIC_TEST_ONLY mode, which allows
  userspace to explore whether certain configurations would work or not.
  (首先，当提交会失败时不允许任何硬件更改。这让我们得以实现
  DRM_MODE_ATOMIC_TEST_ONLY 模式，让用户空间可以探索某些配置是否可行。)

- This would still allow setting and rollback of just the software state,
  simplifying conversion of existing drivers. But auditing drivers for
  correctness of the atomic_check code becomes really hard with that: Rolling
  back changes in data structures all over the place is hard to get right.
  (这仍然允许只设置和回滚软件状态，从而简化现有驱动的转换。但这样一来，审计
  驱动中 atomic_check 代码的正确性就变得非常困难：回滚散布在各处的数据结构
  更改很难做对。)

- Lastly, for backwards compatibility and to support all use-cases, atomic
  updates need to be incremental and be able to execute in parallel. Hardware
  doesn't always allow it, but where possible plane updates on different CRTCs
  should not interfere, and not get stalled due to output routing changing on
  different CRTCs.
  (最后，为了向后兼容并支持所有使用场景，原子更新需要是增量的，并且能够
  并行执行。硬件并不总是允许这一点，但在可能的情况下，不同 CRTC 上的 plane
  更新不应相互干扰，也不应因不同 CRTC 上的输出路由变化而停滞。)

Taken all together there's two consequences for the atomic design:
(综合起来，原子设计有两个后果：)

- The overall state is split up into per-object state structures:
  :c:type:`struct drm_plane_state <drm_plane_state>` for planes, :c:type:`struct
  drm_crtc_state <drm_crtc_state>` for CRTCs and :c:type:`struct
  drm_connector_state <drm_connector_state>` for connectors. These are the only
  objects with userspace-visible and settable state. For internal state drivers
  can subclass these structures through embedding, or add entirely new state
  structures for their globally shared hardware functions, see :c:type:`struct
  drm_private_state<drm_private_state>`.
  (整体状态被拆分为按对象划分的状态结构：plane 用
  :c:type:`struct drm_plane_state <drm_plane_state>`，CRTC 用
  :c:type:`struct drm_crtc_state <drm_crtc_state>`，connector 用
  :c:type:`struct drm_connector_state <drm_connector_state>`。这些是仅有
  的、拥有用户空间可见且可设置的状态的对象。对于内部状态，驱动可以通过嵌入
  来子类化这些结构，也可以为其全局共享的硬件功能添加全新的状态结构，参见
  :c:type:`struct drm_private_state<drm_private_state>`。)

- An atomic update is assembled and validated as an entirely free-standing pile
  of structures within the :c:type:`drm_atomic_state <drm_atomic_state>`
  container. Driver private state structures are also tracked in the same
  structure; see the next chapter.  Only when a state is committed is it applied
  to the driver and modeset objects. This way rolling back an update boils down
  to releasing memory and unreferencing objects like framebuffers.
  (原子更新作为完全自由独立的一堆结构体在 :c:type:`drm_atomic_state
  <drm_atomic_state>` 容器内组装并校验。驱动私有状态结构也在同一结构中
  跟踪；参见下一章。只有当状态被提交时，它才会被应用到驱动与模式设置对象
  上。这样，回滚一次更新就归结为释放内存并解除对帧缓冲等对象的引用。)

Locking of atomic state structures is internally using :c:type:`struct
drm_modeset_lock <drm_modeset_lock>`. As a general rule the locking shouldn't be
exposed to drivers, instead the right locks should be automatically acquired by
any function that duplicates or peeks into a state, like e.g.
drm_atomic_get_crtc_state().  Locking only protects the software data
structure, ordering of committing state changes to hardware is sequenced using
:c:type:`struct drm_crtc_commit <drm_crtc_commit>`.
(原子状态结构的锁定在内部使用 :c:type:`struct drm_modeset_lock
<drm_modeset_lock>`。一般规则是：锁定不应暴露给驱动，正确的锁应由任何复制
或窥视状态的函数自动获取，例如 drm_atomic_get_crtc_state()。锁定只保护软件
数据结构；向硬件提交状态更改的顺序由 :c:type:`struct drm_crtc_commit
<drm_crtc_commit>` 排定。)

Read on in this chapter, and also in :ref:`drm_atomic_helper` for more detailed
coverage of specific topics.
(请继续阅读本章，更多主题的详细内容还可参见 :ref:`drm_atomic_helper`。)

Handling Driver Private State(处理驱动私有状态)
-----------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_atomic.c
   :doc: handling driver private state

Atomic Mode Setting Function Reference
--------------------------------------

.. kernel-doc:: include/drm/drm_atomic.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_atomic.c
   :export:

Atomic Mode Setting IOCTL and UAPI Functions
--------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_atomic_uapi.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/drm_atomic_uapi.c
   :export:

CRTC Abstraction(CRTC 抽象)
===========================

.. kernel-doc:: drivers/gpu/drm/drm_crtc.c
   :doc: overview

CRTC Functions Reference(CRTC 函数参考)
---------------------------------------

.. kernel-doc:: include/drm/drm_crtc.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_crtc.c
   :export:

Color Management Functions Reference(颜色管理函数参考)
------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_color_mgmt.c
   :export:

.. kernel-doc:: include/drm/drm_color_mgmt.h
   :internal:

Frame Buffer Abstraction(帧缓冲抽象)
====================================

.. kernel-doc:: drivers/gpu/drm/drm_framebuffer.c
   :doc: overview

Frame Buffer Functions Reference(帧缓冲函数参考)
------------------------------------------------

.. kernel-doc:: include/drm/drm_framebuffer.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_framebuffer.c
   :export:

DRM Format Handling(DRM 格式处理)
=================================

.. kernel-doc:: include/uapi/drm/drm_fourcc.h
   :doc: overview

Format Functions Reference(格式函数参考)
----------------------------------------

.. kernel-doc:: include/drm/drm_fourcc.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_fourcc.c
   :export:

.. _kms_dumb_buffer_objects:

Dumb Buffer Objects(Dumb Buffer 对象)
=====================================

.. kernel-doc:: drivers/gpu/drm/drm_dumb_buffers.c
   :doc: overview

Plane Abstraction(Plane 抽象)
=============================

.. kernel-doc:: drivers/gpu/drm/drm_plane.c
   :doc: overview

Plane Functions Reference(Plane 函数参考)
-----------------------------------------

.. kernel-doc:: include/drm/drm_plane.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_plane.c
   :export:

Plane Composition Functions Reference(Plane 合成函数参考)
---------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_blend.c
   :export:

Plane Damage Tracking Functions Reference(Plane 损坏跟踪函数参考)
-----------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_damage_helper.c
   :export:

.. kernel-doc:: include/drm/drm_damage_helper.h
   :internal:

Plane Panic Feature(Plane Panic 特性)
-------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_panic.c
   :doc: overview

Plane Panic Functions Reference(Plane Panic 函数参考)
-----------------------------------------------------

.. kernel-doc:: include/drm/drm_panic.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_panic.c
   :export:

Colorop Abstraction(Colorop 抽象)
=================================

.. kernel-doc:: drivers/gpu/drm/drm_colorop.c
   :doc: overview

Colorop Functions Reference(Colorop 函数参考)
---------------------------------------------

.. kernel-doc:: include/drm/drm_colorop.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_colorop.c
   :export:

Display Modes Function Reference(显示模式函数参考)
==================================================

.. kernel-doc:: include/drm/drm_modes.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_modes.c
   :export:

Connector Abstraction(Connector 抽象)
=====================================

.. kernel-doc:: drivers/gpu/drm/drm_connector.c
   :doc: overview

Connector Functions Reference(Connector 函数参考)
-------------------------------------------------

.. kernel-doc:: include/drm/drm_connector.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_connector.c
   :export:

Writeback Connectors(回写 Connector)
------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_writeback.c
  :doc: overview

.. kernel-doc:: include/drm/drm_writeback.h
  :internal:

.. kernel-doc:: drivers/gpu/drm/drm_writeback.c
  :export:

Encoder Abstraction(Encoder 抽象)
=================================

.. kernel-doc:: drivers/gpu/drm/drm_encoder.c
   :doc: overview

Encoder Functions Reference(Encoder 函数参考)
---------------------------------------------

.. kernel-doc:: include/drm/drm_encoder.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_encoder.c
   :export:

KMS Locking(KMS 锁定)
=====================

.. kernel-doc:: drivers/gpu/drm/drm_modeset_lock.c
   :doc: kms locking

.. kernel-doc:: include/drm/drm_modeset_lock.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_modeset_lock.c
   :export:

KMS Properties(KMS 属性)
========================

This section of the documentation is primarily aimed at user-space developers.
For the driver APIs, see the other sections.
(文档的这一部分主要面向用户空间开发者。驱动 API 请参阅其他章节。)

Requirements(要求)
------------------

KMS drivers might need to add extra properties to support new features. Each
new property introduced in a driver needs to meet a few requirements, in
addition to the one mentioned above:
(KMS 驱动可能需要添加额外的属性来支持新特性。除了上文提到的要求之外，驱动中
引入的每个新属性都需要满足以下几点要求：)

* It must be standardized, documenting:
  (它必须被标准化，并记录：)

  * The full, exact, name string;
    (完整、精确的名称字符串；)
  * If the property is an enum, all the valid value name strings;
    (如果属性是枚举类型，所有有效值的名称字符串；)
  * What values are accepted, and what these values mean;
    (接受哪些取值，以及这些取值的含义；)
  * What the property does and how it can be used;
    (该属性做什么以及如何使用；)
  * How the property might interact with other, existing properties.
    (该属性可能如何与其他既有属性交互。)

* It must provide a generic helper in the core code to register that
  property on the object it attaches to.
  (它必须在核心代码中提供一个通用辅助函数，把该属性注册到其附加的对象
  上。)

* Its content must be decoded by the core and provided in the object's
  associated state structure. That includes anything drivers might want
  to precompute, like struct drm_clip_rect for planes.
  (其内容必须由核心解码，并在对象的关联状态结构中提供。这包括驱动可能想
  预先计算的任何内容，例如 plane 的 struct drm_clip_rect。)

* Its initial state must match the behavior prior to the property
  introduction. This might be a fixed value matching what the hardware
  does, or it may be inherited from the state the firmware left the
  system in during boot.
  (其初始状态必须与该属性引入之前的行为一致。这可以是与硬件行为一致的
  固定值，也可以继承自固件在引导期间留给系统的状态。)

* An IGT test must be submitted where reasonable.
  (在合理的情况下必须提交 IGT 测试。)

For historical reasons, non-standard, driver-specific properties exist. If a KMS
driver wants to add support for one of those properties, the requirements for
new properties apply where possible. Additionally, the documented behavior must
match the de facto semantics of the existing property to ensure compatibility.
Developers of the driver that first added the property should help with those
tasks and must ACK the documented behavior if possible.
(出于历史原因，存在非标准的、驱动特有的属性。如果某个 KMS 驱动想为其中某个
属性添加支持，则在可能的情况下适用新属性的各项要求。此外，所记录的行为必须
与既有属性的事实语义一致，以确保兼容性。最初添加该属性的驱动的开发者应协助
完成这些工作，并在可能的情况下对所记录的行为给出 ACK。)

Property Types and Blob Property Support(属性类型与 Blob 属性支持)
------------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_property.c
   :doc: overview

.. kernel-doc:: include/drm/drm_property.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_property.c
   :export:

.. _standard_connector_properties:

Standard Connector Properties(标准 Connector 属性)
--------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_connector.c
   :doc: standard connector properties

HDMI Specific Connector Properties(HDMI 专属 Connector 属性)
------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_connector.c
   :doc: HDMI connector properties

Analog TV Specific Connector Properties(模拟电视专属 Connector 属性)
--------------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_connector.c
   :doc: Analog TV Connector Properties

Standard CRTC Properties(标准 CRTC 属性)
----------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_crtc.c
   :doc: standard CRTC properties

Standard Plane Properties(标准 Plane 属性)
------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_plane.c
   :doc: standard plane properties

.. _plane_composition_properties:

Plane Composition Properties(Plane 合成属性)
--------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_blend.c
   :doc: overview

.. _damage_tracking_properties:

Damage Tracking Properties(损坏跟踪属性)
----------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_plane.c
   :doc: damage tracking

Color Management Properties(颜色管理属性)
-----------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_color_mgmt.c
   :doc: overview

Tile Group Property(Tile Group 属性)
------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_connector.c
   :doc: Tile group

Explicit Fencing Properties(显式同步 (Fencing) 属性)
----------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_atomic_uapi.c
   :doc: explicit fencing properties


Variable Refresh Properties(可变刷新率属性)
-------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_connector.c
   :doc: Variable refresh properties

Cursor Hotspot Properties(光标热点 (Hotspot) 属性)
--------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_plane.c
   :doc: hotspot properties

Existing KMS Properties(既有 KMS 属性)
--------------------------------------

The following table gives description of drm properties exposed by various
modules/drivers. Because this table is very unwieldy, do not add any new
properties here. Instead document them in a section above.
(下表描述各模块/驱动暴露的 drm 属性。由于此表非常庞大，请不要在此添加任何
新属性，而应在上文的相应章节中文档化它们。)

.. csv-table::
   :header-rows: 1
   :file: kms-properties.csv

Vertical Blanking(垂直消隐)
===========================

.. kernel-doc:: drivers/gpu/drm/drm_vblank.c
   :doc: vblank handling

Vertical Blanking and Interrupt Handling Functions Reference(垂直消隐与中断处理函数参考)
----------------------------------------------------------------------------------------

.. kernel-doc:: include/drm/drm_vblank.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_vblank.c
   :export:

Vertical Blank Work(垂直消隐工作)
=================================

.. kernel-doc:: drivers/gpu/drm/drm_vblank_work.c
   :doc: vblank works

Vertical Blank Work Functions Reference(垂直消隐工作函数参考)
-------------------------------------------------------------

.. kernel-doc:: include/drm/drm_vblank_work.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_vblank_work.c
   :export:
