===================================================================================
drm/meson AmLogic Meson Video Processing Unit(drm/meson AmLogic Meson 视频处理单元)
===================================================================================

.. kernel-doc:: drivers/gpu/drm/meson/meson_drv.c
   :doc: Video Processing Unit

Video Processing Unit(视频处理单元)
===================================

The Amlogic Meson Display controller is composed of several components
that are going to be documented below:
(Amlogic Meson 显示控制器由若干组件构成，下面将对它们分别说明：)

.. code::

  DMC|---------------VPU (Video Processing Unit)----------------|------HHI------|
     | vd1   _______     _____________    _________________     |               |
  D  |-------|      |----|            |   |                |    |   HDMI PLL    |
  D  | vd2   | VIU  |    | Video Post |   | Video Encoders |<---|-----VCLK      |
  R  |-------|      |----| Processing |   |                |    |               |
     | osd2  |      |    |            |---| Enci ----------|----|-----VDAC------|
  R  |-------| CSC  |----| Scalers    |   | Encp ----------|----|----HDMI-TX----|
  A  | osd1  |      |    | Blenders   |   | Encl ----------|----|---------------|
  M  |-------|______|----|____________|   |________________|    |               |
  ___|__________________________________________________________|_______________|

Video Input Unit(视频输入单元)
==============================

.. kernel-doc:: drivers/gpu/drm/meson/meson_viu.c
   :doc: Video Input Unit

Video Post Processing(视频后处理)
=================================

.. kernel-doc:: drivers/gpu/drm/meson/meson_vpp.c
   :doc: Video Post Processing

Video Encoder(视频编码器)
=========================

.. kernel-doc:: drivers/gpu/drm/meson/meson_venc.c
   :doc: Video Encoder

Video Clocks(视频时钟)
======================

.. kernel-doc:: drivers/gpu/drm/meson/meson_vclk.c
   :doc: Video Clocks

HDMI Video Output(HDMI 视频输出)
================================

.. kernel-doc:: drivers/gpu/drm/meson/meson_dw_hdmi.c
   :doc: HDMI Output
