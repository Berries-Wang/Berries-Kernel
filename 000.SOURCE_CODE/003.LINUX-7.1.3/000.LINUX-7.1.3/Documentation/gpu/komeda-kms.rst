.. SPDX-License-Identifier: GPL-2.0

=======================================================
 drm/komeda Arm display driver(drm/komeda Arm 显示驱动)
=======================================================
The drm/komeda driver supports the Arm display processor D71 and later products,
this document gives a brief overview of driver design: how it works and why
design it like that.
( drm/komeda 驱动支持 Arm 显示处理器 D71 及后续产品。本文档简要概述驱动的设计：
它如何工作，以及为什么这样设计。)

Overview of D71 like display IPs(D71 类显示 IP 概览)
====================================================

From D71, Arm display IP begins to adopt a flexible and modularized
architecture. A display pipeline is made up of multiple individual and
functional pipeline stages called components, and every component has some
specific capabilities that can give the flowed pipeline pixel data a
particular processing.
(从 D71 开始，Arm 显示 IP 开始采用灵活的模块化架构。一条显示管道由多个独立
的、具有特定功能的管道级组成，这些管道级称为组件 (component)；每个组件都拥有
一些特定能力，可以为流经管道的像素数据提供特定处理。)

Typical D71 components:
(典型的 D71 组件：)

Layer(图层)
-----------
Layer is the first pipeline stage, which prepares the pixel data for the next
stage. It fetches the pixel from memory, decodes it if it's AFBC, rotates the
source image, unpacks or converts YUV pixels to the device internal RGB pixels,
then adjusts the color_space of pixels if needed.
( Layer 是第一级管道，为下一级准备像素数据。它从内存中取出像素，如果是 AFBC
则解码，旋转源图像，把 YUV 像素解包或转换为设备内部的 RGB 像素，然后按需调整
像素的 color_space。)

Scaler(缩放器)
--------------
As its name suggests, scaler takes responsibility for scaling, and D71 also
supports image enhancements by scaler.
The usage of scaler is very flexible and can be connected to layer output
for layer scaling, or connected to compositor and scale the whole display
frame and then feed the output data into wb_layer which will then write it
into memory.
(顾名思义，缩放器 (scaler) 负责缩放，D71 还支持通过缩放器进行图像增强。
缩放器的用法非常灵活：可以连接到图层输出做图层缩放，也可以连接到合成器对
整个显示帧进行缩放，再把输出数据送入 wb_layer，由它写回内存。)

Compositor (compiz)(合成器 (compiz))
------------------------------------
Compositor blends multiple layers or pixel data flows into one single display
frame. its output frame can be fed into post image processor for showing it on
the monitor or fed into wb_layer and written to memory at the same time.
user can also insert a scaler between compositor and wb_layer to down scale
the display frame first and then write to memory.
(合成器把多个图层或像素数据流混合为一帧单一的显示画面。其输出帧可以送入后
图像处理器显示在显示器上，也可以同时送入 wb_layer 并写入内存。用户还可以在
合成器与 wb_layer 之间插入一个缩放器，先把显示帧缩小再写入内存。)

Writeback Layer (wb_layer)(回写层 (wb_layer))
---------------------------------------------
Writeback layer does the opposite things of Layer, which connects to compiz
and writes the composition result to memory.
(回写层做的事情与 Layer 相反：它连接到 compoz，把合成结果写入内存。)

Post image processor (improc)(后图像处理器 (improc))
-----------------------------------------------------
Post image processor adjusts frame data like gamma and color space to fit the
requirements of the monitor.
(后图像处理器调整帧数据 (如伽马和色彩空间)，以适配显示器的要求。)

Timing controller (timing_ctrlr)(时序控制器 (timing_ctrlr))
-----------------------------------------------------------
Final stage of display pipeline, Timing controller is not for the pixel
handling, but only for controlling the display timing.
(显示管道的最后一级。时序控制器不负责像素处理，只负责控制显示时序。)

Merger(合并器)
--------------
D71 scaler mostly only has the half horizontal input/output capabilities
compared with Layer, like if Layer supports 4K input size, the scaler only can
support 2K input/output in the same time. To achieve the ful frame scaling, D71
introduces Layer Split, which splits the whole image to two half parts and feeds
them to two Layers A and B, and does the scaling independently. After scaling
the result need to be fed to merger to merge two part images together, and then
output merged result to compiz.
(与 Layer 相比，D71 缩放器的水平输入/输出能力大多只有一半：例如 Layer 支持
4K 输入尺寸，而缩放器同一时间只能支持 2K 输入/输出。为实现整幅图像的缩放，
D71 引入了 Layer Split，它把整幅图像拆成两个半幅，分别送入 Layer A 和 B 各自
独立缩放。缩放之后，结果需要送入合并器 (merger) 把两幅图像合在一起，再把
合并后的结果输出给 compoz。)

Splitter(拆分器)
----------------
Similar to Layer Split, but Splitter is used for writeback, which splits the
compiz result to two parts and then feed them to two scalers.
(与 Layer Split 类似，但拆分器 (splitter) 用于回写：它把 compoz 的结果拆成
两部分，然后送给两个缩放器。)

Possible D71 Pipeline usage(D71 管道的可能用法)
===============================================

Benefitting from the modularized architecture, D71 pipelines can be easily
adjusted to fit different usages. And D71 has two pipelines, which support two
types of working mode:
(得益于模块化架构，D71 管道可以轻松调整以适应不同用途。D71 有两条管道，支持
两种工作模式：)

-   Dual display mode
    Two pipelines work independently and separately to drive two display outputs.
    (双显示模式：两条管道相互独立、分开工作，驱动两个显示输出。)

-   Single display mode
    Two pipelines work together to drive only one display output.
    (单显示模式：两条管道协同工作，只驱动一个显示输出。)

    On this mode, pipeline_B doesn't work independently, but outputs its
    composition result into pipeline_A, and its pixel timing also derived from
    pipeline_A.timing_ctrlr. The pipeline_B works just like a "slave" of
    pipeline_A(master)
    (在此模式下，pipeline_B 不独立工作，而是把它的合成结果输出到 pipeline_A，
    其像素时序也来自 pipeline_A.timing_ctrlr。 pipeline_B 就像 pipeline_A
    (master) 的 "slave"。)

Single pipeline data flow(单管道数据流)
---------------------------------------

.. kernel-render:: DOT
   :alt: Single pipeline digraph
   :caption: Single pipeline data flow

   digraph single_ppl {
      rankdir=LR;

      subgraph {
         "Memory";
         "Monitor";
      }

      subgraph cluster_pipeline {
          style=dashed
          node [shape=box]
          {
              node [bgcolor=grey style=dashed]
              "Scaler-0";
              "Scaler-1";
              "Scaler-0/1"
          }

         node [bgcolor=grey style=filled]
         "Layer-0" -> "Scaler-0"
         "Layer-1" -> "Scaler-0"
         "Layer-2" -> "Scaler-1"
         "Layer-3" -> "Scaler-1"

         "Layer-0" -> "Compiz"
         "Layer-1" -> "Compiz"
         "Layer-2" -> "Compiz"
         "Layer-3" -> "Compiz"
         "Scaler-0" -> "Compiz"
         "Scaler-1" -> "Compiz"

         "Compiz" -> "Scaler-0/1" -> "Wb_layer"
         "Compiz" -> "Improc" -> "Timing Controller"
      }

      "Wb_layer" -> "Memory"
      "Timing Controller" -> "Monitor"
   }

Dual pipeline with Slave enabled(启用 Slave 的双管道)
-----------------------------------------------------

.. kernel-render:: DOT
   :alt: Slave pipeline digraph
   :caption: Slave pipeline enabled data flow

   digraph slave_ppl {
      rankdir=LR;

      subgraph {
         "Memory";
         "Monitor";
      }
      node [shape=box]
      subgraph cluster_pipeline_slave {
          style=dashed
          label="Slave Pipeline_B"
          node [shape=box]
          {
              node [bgcolor=grey style=dashed]
              "Slave.Scaler-0";
              "Slave.Scaler-1";
          }

         node [bgcolor=grey style=filled]
         "Slave.Layer-0" -> "Slave.Scaler-0"
         "Slave.Layer-1" -> "Slave.Scaler-0"
         "Slave.Layer-2" -> "Slave.Scaler-1"
         "Slave.Layer-3" -> "Slave.Scaler-1"

         "Slave.Layer-0" -> "Slave.Compiz"
         "Slave.Layer-1" -> "Slave.Compiz"
         "Slave.Layer-2" -> "Slave.Compiz"
         "Slave.Layer-3" -> "Slave.Compiz"
         "Slave.Scaler-0" -> "Slave.Compiz"
         "Slave.Scaler-1" -> "Slave.Compiz"
      }

      subgraph cluster_pipeline_master {
          style=dashed
          label="Master Pipeline_A"
          node [shape=box]
          {
              node [bgcolor=grey style=dashed]
              "Scaler-0";
              "Scaler-1";
              "Scaler-0/1"
          }

         node [bgcolor=grey style=filled]
         "Layer-0" -> "Scaler-0"
         "Layer-1" -> "Scaler-0"
         "Layer-2" -> "Scaler-1"
         "Layer-3" -> "Scaler-1"

         "Slave.Compiz" -> "Compiz"
         "Layer-0" -> "Compiz"
         "Layer-1" -> "Compiz"
         "Layer-2" -> "Compiz"
         "Layer-3" -> "Compiz"
         "Scaler-0" -> "Compiz"
         "Scaler-1" -> "Compiz"

         "Compiz" -> "Scaler-0/1" -> "Wb_layer"
         "Compiz" -> "Improc" -> "Timing Controller"
      }

      "Wb_layer" -> "Memory"
      "Timing Controller" -> "Monitor"
   }

Sub-pipelines for input and output
----------------------------------

A complete display pipeline can be easily divided into three sub-pipelines
according to the in/out usage.

Layer(input) pipeline(输入管道)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-render:: DOT
   :alt: Layer data digraph
   :caption: Layer (input) data flow

   digraph layer_data_flow {
      rankdir=LR;
      node [shape=box]

      {
         node [bgcolor=grey style=dashed]
           "Scaler-n";
      }

      "Layer-n" -> "Scaler-n" -> "Compiz"
   }

.. kernel-render:: DOT
   :alt: Layer Split digraph
   :caption: Layer Split pipeline

   digraph layer_data_flow {
      rankdir=LR;
      node [shape=box]

      "Layer-0/1" -> "Scaler-0" -> "Merger"
      "Layer-2/3" -> "Scaler-1" -> "Merger"
      "Merger" -> "Compiz"
   }

Writeback(output) pipeline(回写管道)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. kernel-render:: DOT
   :alt: writeback digraph
   :caption: Writeback(output) data flow

   digraph writeback_data_flow {
      rankdir=LR;
      node [shape=box]

      {
         node [bgcolor=grey style=dashed]
           "Scaler-n";
      }

      "Compiz" -> "Scaler-n" -> "Wb_layer"
   }

.. kernel-render:: DOT
   :alt: split writeback digraph
   :caption: Writeback(output) Split data flow

   digraph writeback_data_flow {
      rankdir=LR;
      node [shape=box]

      "Compiz" -> "Splitter"
      "Splitter" -> "Scaler-0" -> "Merger"
      "Splitter" -> "Scaler-1" -> "Merger"
      "Merger" -> "Wb_layer"
   }

Display output pipeline(显示输出管道)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
.. kernel-render:: DOT
   :alt: display digraph
   :caption: display output data flow

   digraph single_ppl {
      rankdir=LR;
      node [shape=box]

      "Compiz" -> "Improc" -> "Timing Controller"
   }

In the following section we'll see these three sub-pipelines will be handled
by KMS-plane/wb_conn/crtc respectively.
(在下面的小节中，我们将看到这三个子管道分别由 KMS-plane/wb_conn/crtc 来
处理。)

Komeda Resource abstraction(Komeda 资源抽象)
============================================

struct komeda_pipeline/component
--------------------------------

To fully utilize and easily access/configure the HW, the driver side also uses
a similar architecture: Pipeline/Component to describe the HW features and
capabilities, and a specific component includes two parts:
(为了充分利用并方便地访问/配置硬件，驱动侧也采用类似的 Pipeline/Component
架构来描述硬件特性与能力。一个具体组件包括两部分：)

-  Data flow controlling.
   (数据流控制。)
-  Specific component capabilities and features.
   (组件特有的能力与特性。)

So the driver defines a common header struct komeda_component to describe the
data flow control and all specific components are a subclass of this base
structure.
(因此，驱动定义了一个公共头结构 struct komeda_component 来描述数据流控制，
所有具体组件都是这个基础结构的子类。)

.. kernel-doc:: drivers/gpu/drm/arm/display/komeda/komeda_pipeline.h
   :internal:

Resource discovery and initialization(资源发现与初始化)
=======================================================

Pipeline and component are used to describe how to handle the pixel data. We
still need a @struct komeda_dev to describe the whole view of the device, and
the control-abilites of device.
(管道和组件用于描述如何处理像素数据。我们还需要一个 @struct komeda_dev 来
描述设备的整体视图以及设备的控制能力。)

We have &komeda_dev, &komeda_pipeline, &komeda_component. Now fill devices with
pipelines. Since komeda is not for D71 only but also intended for later products,
of course we’d better share as much as possible between different products. To
achieve this, split the komeda device into two layers: CORE and CHIP.
(我们已有 &komeda_dev、&komeda_pipeline、&komeda_component。现在用管道填充
设备。由于 komeda 不仅面向 D71，也面向后续产品，当然最好在不同产品之间尽可
能多地共享。为此，把 komeda 设备拆成两层：CORE 和 CHIP。)

-   CORE: for common features and capabilities handling.
    ( CORE：负责通用特性与能力的处理。)
-   CHIP: for register programming and HW specific feature (limitation) handling.
    ( CHIP：负责寄存器编程和硬件特定特性 (限制) 的处理。)

CORE can access CHIP by three chip function structures:
( CORE 通过三个芯片函数结构访问 CHIP：)

-   struct komeda_dev_funcs
-   struct komeda_pipeline_funcs
-   struct komeda_component_funcs

.. kernel-doc:: drivers/gpu/drm/arm/display/komeda/komeda_dev.h
   :internal:

Format handling(格式处理)
==========================

.. kernel-doc:: drivers/gpu/drm/arm/display/komeda/komeda_format_caps.h
   :internal:
.. kernel-doc:: drivers/gpu/drm/arm/display/komeda/komeda_framebuffer.h
   :internal:

Attach komeda_dev to DRM-KMS(将 komeda_dev 挂接到 DRM-KMS)
==========================================================

Komeda abstracts resources by pipeline/component, but DRM-KMS uses
crtc/plane/connector. One KMS-obj cannot represent only one single component,
since the requirements of a single KMS object cannot simply be achieved by a
single component, usually that needs multiple components to fit the requirement.
Like set mode, gamma, ctm for KMS all target on CRTC-obj, but komeda needs
compiz, improc and timing_ctrlr to work together to fit these requirements.
And a KMS-Plane may require multiple komeda resources: layer/scaler/compiz.
( Komeda 以 pipeline/component 抽象资源，而 DRM-KMS 使用
crtc/plane/connector。一个 KMS 对象不能只表示单一的组件，因为单个 KMS 对象
的需求通常无法由单个组件简单满足，往往需要多个组件配合。例如 KMS 的设置模式、
gamma、ctm 都作用于 CRTC 对象，但 komeda 需要 compoz、improc 与 timing_ctrlr
协同工作才能满足这些需求。而一个 KMS-Plane 可能需要多个 komeda 资源：
layer/scaler/compiz。)

So, one KMS-Obj represents a sub-pipeline of komeda resources.
(所以，一个 KMS 对象代表 komeda 资源的一个子管道。)

-   Plane: `Layer(input) pipeline(输入管道)`_
    ( Plane：对应输入管道。)
-   Wb_connector: `Writeback(output) pipeline(回写管道)`_
    ( Wb_connector：对应回写管道。)
-   Crtc: `Display output pipeline(显示输出管道)`_
    ( Crtc：对应显示输出管道。)

So, for komeda, we treat KMS crtc/plane/connector as users of pipeline and
component, and at any one time a pipeline/component only can be used by one
user. And pipeline/component will be treated as private object of DRM-KMS; the
state will be managed by drm_atomic_state as well.
(因此，对 komeda 而言，我们把 KMS 的 crtc/plane/connector 视为管道和组件的
使用者，且任一时刻一个 pipeline/component 只能被一个使用者使用。
pipeline/component 将被视为 DRM-KMS 的私有对象；其状态同样由
drm_atomic_state 管理。)

How to map plane to Layer(input) pipeline(如何把 plane 映射到 Layer(input) 管道)
--------------------------------------------------------------------------------

Komeda has multiple Layer input pipelines, see:
( Komeda 有多个 Layer 输入管道，参见：)
-   `Single pipeline data flow(单管道数据流)`_
-   `Dual pipeline with Slave enabled(启用 Slave 的双管道)`_

The easiest way is binding a plane to a fixed Layer pipeline, but consider the
komeda capabilities:
(最简单的方式是把 plane 绑定到固定的 Layer 管道，但考虑到 komeda 的能力：)

-   Layer Split, See `Layer(input) pipeline(输入管道)`_

    Layer_Split is quite complicated feature, which splits a big image into two
    parts and handles it by two layers and two scalers individually. But it
    imports an edge problem or effect in the middle of the image after the split.
    To avoid such a problem, it needs a complicated Split calculation and some
    special configurations to the layer and scaler. We'd better hide such HW
    related complexity to user mode.
    ( Layer Split 是相当复杂的特性，它把一幅大图像拆成两部分，分别由两个
    layer 和两个 scaler 独立处理。但拆分后会在图像中部引入边缘问题或效应。
    为避免该问题，需要进行复杂的拆分计算，并对 layer 和 scaler 做一些特殊
    配置。我们最好把这类与硬件相关的复杂性对用户态隐藏起来。)

-   Slave pipeline, See `Dual pipeline with Slave enabled(启用 Slave 的双管道)`_

    Since the compiz component doesn't output alpha value, the slave pipeline
    only can be used for bottom layers composition. The komeda driver wants to
    hide this limitation to the user. The way to do this is to pick a suitable
    Layer according to plane_state->zpos.
    (由于 compoz 组件不输出 alpha 值，slave 管道只能用于底层 (bottom) 图层的
    合成。komeda 驱动希望向用户隐藏这一限制，做法是根据 plane_state->zpos
    挑选合适的 Layer。)

So for komeda, the KMS-plane doesn't represent a fixed komeda layer pipeline,
but multiple Layers with same capabilities. Komeda will select one or more
Layers to fit the requirement of one KMS-plane.
(因此对 komeda 来说，KMS-plane 代表的不是固定的 komeda layer 管道，而是多个
具有相同能力的 Layer。Komeda 会选择一个或多个 Layer 来满足一个 KMS-plane 的
需求。)

Make component/pipeline to be drm_private_obj(把 component/pipeline 变为 drm_private_obj)
------------------------------------------------------------------------------------------

Add :c:type:`drm_private_obj` to :c:type:`komeda_component`, :c:type:`komeda_pipeline`

.. code-block:: c

    struct komeda_component {
        struct drm_private_obj obj;
        ...
    }

    struct komeda_pipeline {
        struct drm_private_obj obj;
        ...
    }

Tracking component_state/pipeline_state by drm_atomic_state(由 drm_atomic_state 跟踪 component_state/pipeline_state)
--------------------------------------------------------------------------------------------------------------------

Add :c:type:`drm_private_state` and user to :c:type:`komeda_component_state`,
:c:type:`komeda_pipeline_state`

.. code-block:: c

    struct komeda_component_state {
        struct drm_private_state obj;
        void *binding_user;
        ...
    }

    struct komeda_pipeline_state {
        struct drm_private_state obj;
        struct drm_crtc *crtc;
        ...
    }

komeda component validation(komeda 组件校验)
--------------------------------------------

Komeda has multiple types of components, but the process of validation are
similar, usually including the following steps:
( Komeda 有多种类型的组件，但校验过程是类似的，通常包括以下步骤：)

.. code-block:: c

    int komeda_xxxx_validate(struct komeda_component_xxx xxx_comp,
                struct komeda_component_output *input_dflow,
                struct drm_plane/crtc/connector *user,
                struct drm_plane/crtc/connector_state, *user_state)
    {
         setup 1: check if component is needed, like the scaler is optional depending
                  on the user_state; if unneeded, just return, and the caller will
                  put the data flow into next stage.
         Setup 2: check user_state with component features and capabilities to see
                  if requirements can be met; if not, return fail.
         Setup 3: get component_state from drm_atomic_state, and try set to set
                  user to component; fail if component has been assigned to another
                  user already.
         Setup 3: configure the component_state, like set its input component,
                  convert user_state to component specific state.
         Setup 4: adjust the input_dflow and prepare it for the next stage.
    }

komeda_kms Abstraction(komeda_kms 抽象)
---------------------------------------

.. kernel-doc:: drivers/gpu/drm/arm/display/komeda/komeda_kms.h
   :internal:

komde_kms Functions(komde_kms 函数)
-----------------------------------
.. kernel-doc:: drivers/gpu/drm/arm/display/komeda/komeda_crtc.c
   :internal:
.. kernel-doc:: drivers/gpu/drm/arm/display/komeda/komeda_plane.c
   :internal:

Build komeda to be a Linux module driver(将 komeda 构建为 Linux 模块驱动)
==========================================================================

Now we have two level devices:
(现在我们有了两级设备：)

-   komeda_dev: describes the real display hardware.
    ( komeda_dev：描述真实的显示硬件。)
-   komeda_kms_dev: attaches or connects komeda_dev to DRM-KMS.
    ( komeda_kms_dev：把 komeda_dev 挂接/连接到 DRM-KMS。)

All komeda operations are supplied or operated by komeda_dev or komeda_kms_dev,
the module driver is only a simple wrapper to pass the Linux command
(probe/remove/pm) into komeda_dev or komeda_kms_dev.
(所有 komeda 操作都由 komeda_dev 或 komeda_kms_dev 提供或执行，模块驱动只是
一个简单的包装，把 Linux 命令 (probe/remove/pm) 传入 komeda_dev 或
komeda_kms_dev。)
