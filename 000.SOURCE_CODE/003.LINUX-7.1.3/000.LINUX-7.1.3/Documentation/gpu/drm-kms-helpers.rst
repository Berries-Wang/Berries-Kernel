===============================================
Mode Setting Helper Functions(模式设置辅助函数)
===============================================

The DRM subsystem aims for a strong separation between core code and helper
libraries. Core code takes care of general setup and teardown and decoding
userspace requests to kernel internal objects. Everything else is handled by a
large set of helper libraries, which can be combined freely to pick and choose
for each driver what fits, and avoid shared code where special behaviour is
needed.
( DRM 子系统致力于核心代码与辅助库之间的强分离。核心代码负责总体搭建与拆除，
以及把用户空间请求解码为内核内部对象。其余一切由一大组辅助库处理，这些辅助
库可以自由组合，为每个驱动挑选合适的部分，并在需要特殊行为的地方避免使用
共享代码。)

This distinction between core code and helpers is especially strong in the
modesetting code, where there's a shared userspace ABI for all drivers. This is
in contrast to the render side, where pretty much everything (with very few
exceptions) can be considered optional helper code.
(核心代码与辅助之间的这种区分在模式设置代码中尤为突出，因为所有驱动共享同一
套用户空间 ABI。这与渲染 (render) 侧形成对照——在那里，几乎所有东西 (只有极
少数例外) 都可以视为可选的辅助代码。)

There are a few areas these helpers can grouped into:
(这些辅助函数可以大致归为几类：)

* Helpers to implement modesetting. The important ones here are the atomic
  helpers. Old drivers still often use the legacy CRTC helpers. They both share
  the same set of common helper vtables. For really simple drivers (anything
  that would have been a great fit in the deprecated fbdev subsystem) there's
  also the simple display pipe helpers.
  (用于实现模式设置的辅助函数。这里重要的是原子 (atomic) 辅助函数。老的驱动
  通常仍在使用遗留 CRTC 辅助函数。二者共享同一组通用辅助 vtable。对于非常
  简单的驱动 (任何本可以很好地适配已弃用的 fbdev 子系统的驱动)，还有简单显示
  管道 (simple display pipe) 辅助函数。)

* There's a big pile of helpers for handling outputs. First the generic bridge
  helpers for handling encoder and transcoder IP blocks. Second the panel helpers
  for handling panel-related information and logic. Plus then a big set of
  helpers for the various sink standards (DisplayPort, HDMI, MIPI DSI). Finally
  there's also generic helpers for handling output probing, and for dealing with
  EDIDs.
  (有一大堆用于处理输出的辅助函数。首先是用于处理编码器和转码器 IP 块的通用
  桥接 (bridge) 辅助函数。其次是处理面板 (panel) 相关信息与逻辑的面板辅助
  函数。此外还有一整套针对各种接收端标准 (DisplayPort、HDMI、MIPI DSI) 的
  辅助函数。最后还有用于处理输出探测以及 EDID 的通用辅助函数。)

* The last group of helpers concerns itself with the frontend side of a display
  pipeline: Planes, handling rectangles for visibility checking and scissoring,
  flip queues and assorted bits.
  (最后一类辅助函数关注显示管道的前端部分：plane、用于可见性检查与剪裁的
  矩形处理、翻转队列以及其他杂项。)

Modeset Helper Reference for Common Vtables(公共 vtable 的 modeset 辅助参考)
============================================================================

.. kernel-doc:: include/drm/drm_modeset_helper_vtables.h
   :doc: overview

.. kernel-doc:: include/drm/drm_modeset_helper_vtables.h
   :internal:

.. _drm_atomic_helper:

Atomic Modeset Helper Functions Reference(原子模式设置辅助函数参考)
===================================================================

Overview(概述)
--------------

.. kernel-doc:: drivers/gpu/drm/drm_atomic_helper.c
   :doc: overview

Implementing Asynchronous Atomic Commit(实现异步原子提交)
---------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_atomic_helper.c
   :doc: implementing nonblocking commit

Helper Functions Reference(辅助函数参考)
----------------------------------------

.. kernel-doc:: include/drm/drm_atomic_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_atomic_helper.c
   :export:

Atomic State Reset and Initialization(原子状态复位与初始化)
-----------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_atomic_state_helper.c
   :doc: atomic state reset and initialization

Atomic State Helper Reference(原子状态辅助参考)
-----------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_atomic_state_helper.c
   :export:

GEM Atomic Helper Reference(GEM 原子辅助参考)
---------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_gem_atomic_helper.c
   :doc: overview

.. kernel-doc:: include/drm/drm_gem_atomic_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_gem_atomic_helper.c
   :export:

VBLANK Helper Reference(VBLANK 辅助参考)
----------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_vblank_helper.c
   :doc: overview

.. kernel-doc:: include/drm/drm_vblank_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_vblank_helper.c
   :export:

fbdev Helper Functions Reference(fbdev 辅助函数参考)
====================================================

.. kernel-doc:: drivers/gpu/drm/drm_fb_helper.c
   :doc: fbdev helpers

.. kernel-doc:: include/drm/drm_fb_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_fb_helper.c
   :export:

format Helper Functions Reference(format 辅助函数参考)
======================================================

.. kernel-doc:: drivers/gpu/drm/drm_format_helper.c
   :export:

Framebuffer DMA Helper Functions Reference(帧缓冲 DMA 辅助函数参考)
===================================================================

.. kernel-doc:: drivers/gpu/drm/drm_fb_dma_helper.c
   :doc: framebuffer dma helper functions

.. kernel-doc:: drivers/gpu/drm/drm_fb_dma_helper.c
   :export:

Framebuffer GEM Helper Reference(帧缓冲 GEM 辅助参考)
=====================================================

.. kernel-doc:: drivers/gpu/drm/drm_gem_framebuffer_helper.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/drm_gem_framebuffer_helper.c
   :export:

.. _drm_bridges:

Bridges(桥接)
=============

Overview
--------

.. kernel-doc:: drivers/gpu/drm/drm_bridge.c
   :doc: overview

Display Driver Integration(显示驱动集成)
----------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_bridge.c
   :doc: display driver integration

Special Care with MIPI-DSI bridges(MIPI-DSI 桥接的特别注意事项)
---------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_bridge.c
   :doc: special care dsi

Bridge Operations(桥接操作)
---------------------------

.. kernel-doc:: drivers/gpu/drm/drm_bridge.c
   :doc: bridge operations

Bridge Connector Helper(桥接连接器辅助)
---------------------------------------

.. kernel-doc:: drivers/gpu/drm/display/drm_bridge_connector.c
   :doc: overview


Bridge Helper Reference(桥接辅助参考)
-------------------------------------

.. kernel-doc:: include/drm/drm_bridge.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_bridge.c
   :export:

MIPI-DSI bridge operation(MIPI-DSI 桥接操作)
--------------------------------------------

.. kernel-doc:: drivers/gpu/drm/drm_bridge.c
   :doc: dsi bridge operations


Bridge Connector Helper Reference(桥接连接器辅助参考)
-----------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/display/drm_bridge_connector.c
   :export:

Panel-Bridge Helper Reference(Panel-Bridge 辅助参考)
----------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/bridge/panel.c
   :export:

.. _drm_panel_helper:

Panel Helper Reference(面板辅助参考)
====================================

.. kernel-doc:: drivers/gpu/drm/drm_panel.c
   :doc: drm panel

.. kernel-doc:: include/drm/drm_panel.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_panel.c
   :export:

.. kernel-doc:: drivers/gpu/drm/drm_panel_orientation_quirks.c
   :export:

.. kernel-doc:: drivers/gpu/drm/drm_panel_backlight_quirks.c
   :export:

Panel Self Refresh Helper Reference(面板自刷新辅助参考)
=======================================================

.. kernel-doc:: drivers/gpu/drm/drm_self_refresh_helper.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/drm_self_refresh_helper.c
   :export:

HDMI Atomic State Helpers(HDMI 原子状态辅助)
============================================

Overview
--------

.. kernel-doc:: drivers/gpu/drm/display/drm_hdmi_state_helper.c
   :doc: hdmi helpers

Functions Reference(函数参考)
-----------------------------

.. kernel-doc:: drivers/gpu/drm/display/drm_hdmi_state_helper.c
   :export:

HDCP Helper Functions Reference(HDCP 辅助函数参考)
==================================================

.. kernel-doc:: drivers/gpu/drm/display/drm_hdcp_helper.c
   :export:

Display Port Helper Functions Reference(Display Port 辅助函数参考)
==================================================================

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_helper.c
   :doc: dp helpers

.. kernel-doc:: include/drm/display/drm_dp.h
   :internal:

.. kernel-doc:: include/drm/display/drm_dp_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_helper.c
   :export:

Display Port CEC Helper Functions Reference(Display Port CEC 辅助函数参考)
==========================================================================

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_cec.c
   :doc: dp cec helpers

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_cec.c
   :export:

Display Port Dual Mode Adaptor Helper Functions Reference(Display Port 双模式适配器辅助函数参考)
================================================================================================

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_dual_mode_helper.c
   :doc: dp dual mode helpers

.. kernel-doc:: include/drm/display/drm_dp_dual_mode_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_dual_mode_helper.c
   :export:

Display Port MST Helpers(Display Port MST 辅助)
===============================================

Overview
--------

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_mst_topology.c
   :doc: dp mst helper

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_mst_topology.c
   :doc: Branch device and port refcounting

Functions Reference
-------------------

.. kernel-doc:: include/drm/display/drm_dp_mst_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_mst_topology.c
   :export:

Topology Lifetime Internals(拓扑生命期内部机制)
-----------------------------------------------

These functions aren't exported to drivers, but are documented here to help make
the MST topology helpers easier to understand
(这些函数并不导出给驱动，但在此处加以记录，以便让 MST 拓扑辅助函数更易于
理解。)

.. kernel-doc:: drivers/gpu/drm/display/drm_dp_mst_topology.c
   :functions: drm_dp_mst_topology_try_get_mstb drm_dp_mst_topology_get_mstb
               drm_dp_mst_topology_put_mstb
               drm_dp_mst_topology_try_get_port drm_dp_mst_topology_get_port
               drm_dp_mst_topology_put_port
               drm_dp_mst_get_mstb_malloc drm_dp_mst_put_mstb_malloc

MIPI DBI Helper Functions Reference(MIPI DBI 辅助函数参考)
==========================================================

.. kernel-doc:: drivers/gpu/drm/drm_mipi_dbi.c
   :doc: overview

.. kernel-doc:: include/drm/drm_mipi_dbi.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_mipi_dbi.c
   :export:

MIPI DSI Helper Functions Reference(MIPI DSI 辅助函数参考)
==========================================================

.. kernel-doc:: drivers/gpu/drm/drm_mipi_dsi.c
   :doc: dsi helpers

.. kernel-doc:: include/drm/drm_mipi_dsi.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_mipi_dsi.c
   :export:

Display Stream Compression Helper Functions Reference(显示流压缩辅助函数参考)
=============================================================================

.. kernel-doc:: drivers/gpu/drm/display/drm_dsc_helper.c
   :doc: dsc helpers

.. kernel-doc:: include/drm/display/drm_dsc.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/display/drm_dsc_helper.c
   :export:

Output Probing Helper Functions Reference(输出探测辅助函数参考)
===============================================================

.. kernel-doc:: drivers/gpu/drm/drm_probe_helper.c
   :doc: output probing helper overview

.. kernel-doc:: drivers/gpu/drm/drm_probe_helper.c
   :export:

EDID Helper Functions Reference(EDID 辅助函数参考)
==================================================

.. kernel-doc:: include/drm/drm_edid.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_edid.c
   :export:

.. kernel-doc:: include/drm/drm_eld.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_eld.c
   :export:

SCDC Helper Functions Reference(SCDC 辅助函数参考)
==================================================

.. kernel-doc:: drivers/gpu/drm/display/drm_scdc_helper.c
   :doc: scdc helpers

.. kernel-doc:: include/drm/display/drm_scdc_helper.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/display/drm_scdc_helper.c
   :export:

HDMI Infoframes Helper Reference(HDMI Infoframe 辅助参考)
=========================================================

Strictly speaking this is not a DRM helper library but generally usable
by any driver interfacing with HDMI outputs like v4l or alsa drivers.
But it nicely fits into the overall topic of mode setting helper
libraries and hence is also included here.

.. kernel-doc:: include/linux/hdmi.h
   :internal:

.. kernel-doc:: drivers/video/hdmi.c
   :export:

Rectangle Utilities Reference(矩形工具参考)
===========================================

.. kernel-doc:: include/drm/drm_rect.h
   :doc: rect utils

.. kernel-doc:: include/drm/drm_rect.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_rect.c
   :export:

Flip-work Helper Reference(Flip-work 辅助参考)
==============================================

.. kernel-doc:: include/drm/drm_flip_work.h
   :doc: flip utils

.. kernel-doc:: include/drm/drm_flip_work.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_flip_work.c
   :export:

Auxiliary Modeset Helpers(附加模式设置辅助函数)
===============================================

.. kernel-doc:: drivers/gpu/drm/drm_modeset_helper.c
   :doc: aux kms helpers

.. kernel-doc:: drivers/gpu/drm/drm_modeset_helper.c
   :export:

OF/DT Helpers(OF/DT 辅助函数)
=============================

.. kernel-doc:: drivers/gpu/drm/drm_of.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/drm_of.c
   :export:

Legacy Plane Helper Reference(传统 plane 辅助参考)
==================================================

.. kernel-doc:: drivers/gpu/drm/drm_plane_helper.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/drm_plane_helper.c
   :export:

Legacy CRTC/Modeset Helper Functions Reference(传统 CRTC/Modeset 辅助函数参考)
==============================================================================

.. kernel-doc:: drivers/gpu/drm/drm_crtc_helper.c
   :doc: overview

.. kernel-doc:: drivers/gpu/drm/drm_crtc_helper.c
   :export:

Privacy-screen class(隐私屏类)
==============================

.. kernel-doc:: drivers/gpu/drm/drm_privacy_screen.c
   :doc: overview

.. kernel-doc:: include/drm/drm_privacy_screen_driver.h
   :internal:

.. kernel-doc:: include/drm/drm_privacy_screen_machine.h
   :internal:

.. kernel-doc:: drivers/gpu/drm/drm_privacy_screen.c
   :export:
