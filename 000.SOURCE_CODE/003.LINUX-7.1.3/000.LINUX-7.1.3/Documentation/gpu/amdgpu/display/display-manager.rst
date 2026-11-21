=========================================
AMDgpu Display Manager(AMDgpu 显示管理器)
=========================================

.. contents:: Table of Contents
    :depth: 3

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm.h
   :internal:

Lifecycle(生命周期)
===================

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm.c
   :doc: DM Lifecycle

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm.c
   :functions: dm_hw_init dm_hw_fini

Interrupts(中断)
================

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm_irq.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm_irq.c
   :internal:

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm.c
   :functions: register_hpd_handlers dm_crtc_high_irq dm_pflip_high_irq

Atomic Implementation(Atomic 实现)
==================================

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm.c
   :doc: atomic

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm.c
   :functions: amdgpu_dm_atomic_check amdgpu_dm_atomic_commit_tail

Color Management Properties(颜色管理属性)
=========================================

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm_color.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/amd/display/amdgpu_dm/amdgpu_dm_color.c
   :internal:


DC Color Capabilities between DCN generations(不同 DCN 世代的 DC 颜色能力)
--------------------------------------------------------------------------

DRM/KMS framework defines three CRTC color correction properties: degamma,
color transformation matrix (CTM) and gamma, and two properties for degamma and
gamma LUT sizes. AMD DC programs some of the color correction features
pre-blending but DRM/KMS has not per-plane color correction properties.
(DRM/KMS 框架定义了三个 CRTC 颜色校正属性：degamma、颜色变换矩阵 (CTM) 与 gamma，以及两个用于 degamma 与 gamma LUT 大小的属性。AMD DC 会在混合前 (pre-blending) 编程一部分颜色校正特性，但 DRM/KMS 没有按平面 (per-plane) 的颜色校正属性。)

In general, the DRM CRTC color properties are programmed to DC, as follows:
CRTC gamma after blending, and CRTC degamma pre-blending. Although CTM is
programmed after blending, it is mapped to DPP hw blocks (pre-blending). Other
color caps available in the hw is not currently exposed by DRM interface and
are bypassed.
(总体而言，DRM CRTC 颜色属性按如下方式编程到 DC：CRTC gamma 在混合之后生效，CRTC degamma 在混合之前生效。CTM 虽在混合之后编程，但会被映射到 DPP 硬件块(混合前)。硬件中可用的其他颜色能力目前未通过 DRM 接口暴露，而是被旁路。)

.. kernel-doc:: drivers/gpu/drm/amd/display/dc/dc.h
   :doc: color-management-caps

.. kernel-doc:: drivers/gpu/drm/amd/display/dc/dc.h
   :internal:

The color pipeline has undergone major changes between DCN hardware
generations. What's possible to do before and after blending depends on
hardware capabilities, as illustrated below by the DCN 2.0 and DCN 3.0 families
schemas.
(颜色管线在 DCN 硬件各世代之间经历了重大变化。混合前后能做什么取决于硬件能力，下文以 DCN 2.0 与 DCN 3.0 家族的示意图加以说明。)

**DCN 2.0 family color caps and mapping**
(**DCN 2.0 家族颜色能力与映射**)

.. kernel-figure:: dcn2_cm_drm_current.svg

**DCN 3.0 family color caps and mapping**
(**DCN 3.0 家族颜色能力与映射**)

.. kernel-figure:: dcn3_cm_drm_current.svg

Blend Mode Properties(混合模式属性)
===================================

Pixel blend mode is a DRM plane composition property of :c:type:`drm_plane` used to
describes how pixels from a foreground plane (fg) are composited with the
background plane (bg). Here, we present main concepts of DRM blend mode to help
to understand how this property is mapped to AMD DC interface. See more about
this DRM property and the alpha blending equations in :ref:`DRM Plane
Composition Properties <plane_composition_properties>`.
(像素混合模式 (pixel blend mode) 是 :c:type:`drm_plane` 的一个 DRM 平面合成属性，用于描述来自前景平面 (fg) 的像素如何与背景平面 (bg) 合成。这里我们介绍 DRM 混合模式的主要概念，以帮助理解该属性如何映射到 AMD DC 接口。关于此 DRM 属性与 alpha 混合等式的更多信息，见 :ref:`DRM Plane Composition Properties <plane_composition_properties>`。)

Basically, a blend mode sets the alpha blending equation for plane
composition that fits the mode in which the alpha channel affects the state of
pixel color values and, therefore, the resulted pixel color. For
example, consider the following elements of the alpha blending equation:
(大体上，混合模式为平面合成设置 alpha 混合等式，以契合 alpha 通道影响像素颜色值状态、进而影响最终像素颜色的方式。例如，考虑 alpha 混合等式中的以下元素：)

- *fg.rgb*: Each of the RGB component values from the foreground's pixel.
  (*fg.rgb*: 来自前景像素的各 RGB 分量值。)
- *fg.alpha*: Alpha component value from the foreground's pixel.
  (*fg.alpha*: 来自前景像素的 alpha 分量值。)
- *bg.rgb*: Each of the RGB component values from the background.
  (*bg.rgb*: 来自背景的各 RGB 分量值。)
- *plane_alpha*: Plane alpha value set by the **plane "alpha" property**, see
  more in :ref:`DRM Plane Composition Properties <plane_composition_properties>`.
  (*plane_alpha*: 由 **plane "alpha" 属性** 设置的平面 alpha 值，详见 :ref:`DRM Plane Composition Properties <plane_composition_properties>`。)

in the basic alpha blending equation::

   out.rgb = alpha * fg.rgb + (1 - alpha) * bg.rgb

(在如下基本 alpha 混合等式中：)

the alpha channel value of each pixel in a plane is ignored and only the plane
alpha affects the resulted pixel color values.
(平面中每个像素的 alpha 通道值都会被忽略，只有 plane alpha 会影响最终得到的像素颜色值。)

DRM has three blend mode to define the blend formula in the plane composition:
(DRM 有三种混合模式用于定义平面合成中的混合公式：)

* **None**: Blend formula that ignores the pixel alpha.
  (* **None**: 忽略像素 alpha 的混合公式。)

* **Pre-multiplied**: Blend formula that assumes the pixel color values in a
  plane was already pre-multiplied by its own alpha channel before storage.
  (* **Pre-multiplied**: 假设平面中的像素颜色值在存储之前已与自身 alpha 通道预相乘的混合公式。)

* **Coverage**: Blend formula that assumes the pixel color values were not
  pre-multiplied with the alpha channel values.
  (* **Coverage**: 假设像素颜色值未与 alpha 通道值预相乘的混合公式。)

and pre-multiplied is the default pixel blend mode, that means, when no blend
mode property is created or defined, DRM considers the plane's pixels has
pre-multiplied color values. On IGT GPU tools, the kms_plane_alpha_blend test
provides a set of subtests to verify plane alpha and blend mode properties.
(而 pre-multiplied 是默认的像素混合模式，也就是说，当未创建或未定义混合模式属性时，DRM 认为平面的像素是预相乘颜色值。在 IGT GPU 工具中，kms_plane_alpha_blend 测试提供了一组子测试，用于验证 plane alpha 与混合模式属性。)

The DRM blend mode and its elements are then mapped by AMDGPU display manager
(DM) to program the blending configuration of the Multiple Pipe/Plane Combined
(MPC), as follows:
(AMDGPU 显示管理器 (DM) 随后把 DRM 混合模式及其元素映射为多管道/平面合并 (MPC) 的混合配置编程，如下所示：)

.. kernel-doc:: drivers/gpu/drm/amd/display/dc/inc/hw/mpc.h
   :identifiers: mpcc_blnd_cfg

Therefore, the blending configuration for a single MPCC instance on the MPC
tree is defined by :c:type:`mpcc_blnd_cfg`, where
:c:type:`pre_multiplied_alpha` is the alpha pre-multiplied mode flag used to
set :c:type:`MPCC_ALPHA_MULTIPLIED_MODE`. It controls whether alpha is
multiplied (true/false), being only true for DRM pre-multiplied blend mode.
:c:type:`mpcc_alpha_blend_mode` defines the alpha blend mode regarding pixel
alpha and plane alpha values. It sets one of the three modes for
:c:type:`MPCC_ALPHA_BLND_MODE`, as described below.
(因此，MPC 树上单个 MPCC 实例的混合配置由 :c:type:`mpcc_blnd_cfg` 定义，其中 :c:type:`pre_multiplied_alpha` 是用于设置 :c:type:`MPCC_ALPHA_MULTIPLIED_MODE` 的 alpha 预相乘模式标志。它控制 alpha 是否相乘(true/false)，仅对 DRM pre-multiplied 混合模式为 true。:c:type:`mpcc_alpha_blend_mode` 定义关于像素 alpha 与 plane alpha 值的 alpha 混合模式。它为 :c:type:`MPCC_ALPHA_BLND_MODE` 设置三种模式之一，如下所述。)

.. kernel-doc:: drivers/gpu/drm/amd/display/dc/inc/hw/mpc.h
   :identifiers: mpcc_alpha_blend_mode

DM then maps the elements of `enum mpcc_alpha_blend_mode` to those in the DRM
blend formula, as follows:
(DM 随后把 enum mpcc_alpha_blend_mode 中的元素映射到 DRM 混合公式中的元素，如下所示：)

* *MPC pixel alpha* matches *DRM fg.alpha* as the alpha component value
  from the plane's pixel
  (*MPC pixel alpha* 对应 *DRM fg.alpha*，即来自平面像素的 alpha 分量值)
* *MPC global alpha* matches *DRM plane_alpha* when the pixel alpha should
  be ignored and, therefore, pixel values are not pre-multiplied
  (*MPC global alpha* 对应 *DRM plane_alpha*，此时应忽略像素 alpha，因而像素值不做预相乘)
* *MPC global gain* assumes *MPC global alpha* value when both *DRM
  fg.alpha* and *DRM plane_alpha* participate in the blend equation
  (*MPC global gain* 在 *DRM fg.alpha* 与 *DRM plane_alpha* 都参与混合等式时，采用 *MPC global alpha* 的值)

In short, *fg.alpha* is ignored by selecting
:c:type:`MPCC_ALPHA_BLEND_MODE_GLOBAL_ALPHA`. On the other hand, (plane_alpha *
fg.alpha) component becomes available by selecting
:c:type:`MPCC_ALPHA_BLEND_MODE_PER_PIXEL_ALPHA_COMBINED_GLOBAL_GAIN`. And the
:c:type:`MPCC_ALPHA_MULTIPLIED_MODE` defines if the pixel color values are
pre-multiplied by alpha or not.
(简而言之，选择 :c:type:`MPCC_ALPHA_BLEND_MODE_GLOBAL_ALPHA` 即可忽略 fg.alpha。另一方面，选择 :c:type:`MPCC_ALPHA_BLEND_MODE_PER_PIXEL_ALPHA_COMBINED_GLOBAL_GAIN` 则可启用 (plane_alpha * fg.alpha) 分量。而 :c:type:`MPCC_ALPHA_MULTIPLIED_MODE` 定义像素颜色值是否按 alpha 预相乘。)

Blend configuration flow(混合配置流程)
--------------------------------------

The alpha blending equation is configured from DRM to DC interface by the
following path:
(alpha 混合等式经由以下路径从 DRM 配置到 DC 接口：)

1. When updating a :c:type:`drm_plane_state <drm_plane_state>`, DM calls
   :c:type:`amdgpu_dm_plane_fill_blending_from_plane_state()` that maps
   :c:type:`drm_plane_state <drm_plane_state>` attributes to
   :c:type:`dc_plane_info <dc_plane_info>` struct to be handled in the
   OS-agnostic component (DC).
   (1. 更新 :c:type:`drm_plane_state <drm_plane_state>` 时，DM 调用 :c:type:`amdgpu_dm_plane_fill_blending_from_plane_state()`，把 :c:type:`drm_plane_state <drm_plane_state>` 的属性映射到 :c:type:`dc_plane_info <dc_plane_info>` 结构体，交由与操作系统无关的组件 (DC) 处理。)

2. On DC interface, :c:type:`struct mpcc_blnd_cfg <mpcc_blnd_cfg>` programs the
   MPCC blend configuration considering the :c:type:`dc_plane_info
   <dc_plane_info>` input from DPP.
   (2. 在 DC 接口上，:c:type:`struct mpcc_blnd_cfg <mpcc_blnd_cfg>` 综合 DPP 输入的 :c:type:`dc_plane_info <dc_plane_info>`，对 MPCC 混合配置进行编程。)
