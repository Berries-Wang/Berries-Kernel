.. SPDX-License-Identifier: GPL-2.0

.. include:: <isonum.txt>

ASPEED video driver(ASPEED 视频驱动)
====================================

ASPEED Video Engine found on AST2400/2500/2600 SoC supports high performance
video compressions with a wide range of video quality and compression ratio
options. The adopted compressing algorithm is a modified JPEG algorithm.
(AST2400/2500/2600 SoC 上的 ASPEED Video Engine 支持高性能视频压缩，可提供
广泛的视频质量与压缩比选项。所采用的压缩算法是改进的 JPEG 算法。)

There are 2 types of compressions in this IP.
(此 IP 中有两种压缩类型。)

* JPEG JFIF standard mode: for single frame and management compression
  (JPEG JFIF 标准模式：用于单帧与管理压缩。)
* ASPEED proprietary mode: for multi-frame and differential compression.
  Support 2-pass (high quality) video compression scheme (Patent pending by
  ASPEED). Provide visually lossless video compression quality or to reduce
  the network average loading under intranet KVM applications.
  (ASPEED 私有模式：用于多帧与差分压缩。支持两遍 (高质量) 视频压缩方案
  (ASPEED 专利申请中)。提供视觉无损的视频压缩质量，或者在内网 KVM 应用下
  降低网络的平均负载。)

VIDIOC_S_FMT can be used to choose which format you want. V4L2_PIX_FMT_JPEG
stands for JPEG JFIF standard mode; V4L2_PIX_FMT_AJPG stands for ASPEED
proprietary mode.
(VIDIOC_S_FMT 可用于选择想要的格式。V4L2_PIX_FMT_JPEG 表示 JPEG JFIF 标准模
式；V4L2_PIX_FMT_AJPG 表示 ASPEED 私有模式。)

More details on the ASPEED video hardware operations can be found in
*chapter 6.2.16 KVM Video Driver* of SDK_User_Guide which available on
`github <https://github.com/AspeedTech-BMC/openbmc/releases/>`__.
(关于 ASPEED 视频硬件操作的更多细节，可在可从
`github <https://github.com/AspeedTech-BMC/openbmc/releases/>`__ 获取的
SDK_User_Guide 的 *chapter 6.2.16 KVM Video Driver* 一章中找到。)

The ASPEED video driver implements the following driver-specific control:
(ASPEED 视频驱动实现了以下驱动专属控制：)

``V4L2_CID_ASPEED_HQ_MODE``(高质量模式)
------------------------------------------------------------------------
    Enable/Disable ASPEED's High quality mode. This is a private control
    that can be used to enable high quality for aspeed proprietary mode.
    (启用/禁用 ASPEED 的高质量 (High quality) 模式。这是一个私有控制项，
    可用于为 aspeed 私有模式启用高质量。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 4

    * - ``(0)``
      - ASPEED HQ mode is disabled.(禁用 ASPEED HQ 模式。)
    * - ``(1)``
      - ASPEED HQ mode is enabled.(启用 ASPEED HQ 模式。)

``V4L2_CID_ASPEED_HQ_JPEG_QUALITY``(高质量 JPEG 质量)
-------------------------------------------------------------------------------
    Define the quality of ASPEED's High quality mode. This is a private control
    that can be used to decide compression quality if High quality mode enabled
    . Higher the value, better the quality and bigger the size.
    (定义 ASPEED 高质量模式的质量。这是一个私有控制项，用于在启用高质量模式
    时决定压缩质量。值越大，质量越好，体积也越大。)

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 4

    * - ``(1)``
      - minimum(最小值)
    * - ``(12)``
      - maximum(最大值)
    * - ``(1)``
      - step(步长)
    * - ``(1)``
      - default(默认值)

**Copyright** |copy| 2022 ASPEED Technology Inc.
