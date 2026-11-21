============================================================
 drm/vc4 Broadcom VC4 Graphics Driver(Broadcom VC4 图形驱动)
============================================================

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_drv.c
   :doc: Broadcom VC4 Graphics Driver

Display Hardware Handling(显示硬件处理)
=======================================

This section covers everything related to the display hardware including
the mode setting infrastructure, plane, sprite and cursor handling and
display, output probing and related topics.
(本节涵盖与显示硬件相关的一切内容，包括模式设置基础设施、平面
(plane)、精灵(sprite)与光标处理、显示、输出探测及相关主题。)

Pixel Valve (DRM CRTC)(像素阀(DRM CRTC))
----------------------------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_crtc.c
   :doc: VC4 CRTC module

HVS(HVS)
--------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_hvs.c
   :doc: VC4 HVS module.

HVS planes(HVS 平面)
--------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_plane.c
   :doc: VC4 plane module

HDMI encoder(HDMI 编码器)
-------------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_hdmi.c
   :doc: VC4 Falcon HDMI module

DSI encoder(DSI 编码器)
-----------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_dsi.c
   :doc: VC4 DSI0/DSI1 module

DPI encoder(DPI 编码器)
-----------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_dpi.c
   :doc: VC4 DPI module

VEC (Composite TV out) encoder(VEC(复合电视输出)编码器)
-------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_vec.c
   :doc: VC4 SDTV module

KUnit Tests(KUnit 测试)
=======================

The VC4 Driver uses KUnit to perform driver-specific unit and
integration tests.
(VC4 驱动使用 KUnit 执行驱动特定的单元测试与集成测试。)

These tests are using a mock driver and can be ran using the
command below, on either arm or arm64 architectures,
(这些测试使用模拟(mock)驱动，可在 arm 或 arm64 架构上用下面的
命令运行：)

.. code-block:: bash

	$ ./tools/testing/kunit/kunit.py run \
		--kunitconfig=drivers/gpu/drm/vc4/tests/.kunitconfig \
		--cross_compile aarch64-linux-gnu- --arch arm64

Parts of the driver that are currently covered by tests are:
(目前被测试覆盖的驱动部分包括：)
 * The HVS to PixelValve dynamic FIFO assignment, for the BCM2835-7
   and BCM2711.
   (HVS 到 PixelValve 的动态 FIFO 分配，适用于 BCM2835-7 和 BCM2711。)

Memory Management and 3D Command Submission(内存管理与 3D 命令提交)
===================================================================

This section covers the GEM implementation in the vc4 driver.
(本节介绍 vc4 驱动中的 GEM 实现。)

GPU buffer object (BO) management(GPU 缓冲区对象(BO)管理)
---------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_bo.c
   :doc: VC4 GEM BO management support

V3D binner command list (BCL) validation(V3D binner 命令列表(BCL)校验)
----------------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_validate.c
   :doc: Command list validator for VC4.

V3D render command list (RCL) generation(V3D 渲染命令列表(RCL)生成)
-------------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_render_cl.c
   :doc: Render command list generation

Shader validator for VC4(VC4 着色器校验器)
------------------------------------------
.. kernel-doc:: drivers/gpu/drm/vc4/vc4_validate_shaders.c
   :doc: Shader validator for VC4.

V3D Interrupts(V3D 中断)
------------------------

.. kernel-doc:: drivers/gpu/drm/vc4/vc4_irq.c
   :doc: Interrupt management for the V3D engine
