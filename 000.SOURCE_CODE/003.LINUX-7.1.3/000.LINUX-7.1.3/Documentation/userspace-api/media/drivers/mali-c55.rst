.. SPDX-License-Identifier: GPL-2.0-only

Arm Mali-C55 ISP driver(Arm Mali-C55 ISP 驱动)
==============================================

The Arm Mali-C55 ISP driver implements a single driver-specific control:
(Arm Mali-C55 ISP 驱动实现了一个驱动专属控制：)

``V4L2_CID_MALI_C55_CAPABILITIES (bitmask)``
    Detail the capabilities of the ISP by giving detail about the fitted blocks.
    (通过给出已装配模块的细节来说明 ISP 的能力。)

    .. flat-table:: Bitmask meaning definitions
	:header-rows: 1
	:widths: 2 4 8

	* - Bit(位)
	  - Macro
	  - Meaning(含义)
        * - 0
          - MALI_C55_PONG
          - Pong configuration space is fitted in the ISP(ISP 装配了 Pong 配置空间)
        * - 1
          - MALI_C55_WDR
          - WDR Framestitch, offset and gain is fitted in the ISP(ISP 装配了 WDR 帧拼接、偏移与增益)
        * - 2
          - MALI_C55_COMPRESSION
          - Temper compression is fitted in the ISP(ISP 装配了 Temper 压缩)
        * - 3
          - MALI_C55_TEMPER
          - Temper is fitted in the ISP(ISP 装配了 Temper)
        * - 4
          - MALI_C55_SINTER_LITE
          - Sinter Lite is fitted in the ISP instead of the full Sinter version(ISP 装配的是 Sinter Lite 而不是完整版 Sinter)
        * - 5
          - MALI_C55_SINTER
          - Sinter is fitted in the ISP(ISP 装配了 Sinter)
        * - 6
          - MALI_C55_IRIDIX_LTM
          - Iridix local tone mappine is fitted in the ISP(ISP 装配了 Iridix 局部色调映射)
        * - 7
          - MALI_C55_IRIDIX_GTM
          - Iridix global tone mapping is fitted in the ISP(ISP 装配了 Iridix 全局色调映射)
        * - 8
          - MALI_C55_CNR
          - Colour noise reduction is fitted in the ISP(ISP 装配了彩色降噪)
        * - 9
          - MALI_C55_FRSCALER
          - The full resolution pipe scaler is fitted in the ISP(ISP 装配了全分辨率管线缩放器)
        * - 10
          - MALI_C55_DS_PIPE
          - The downscale pipe is fitted in the ISP(ISP 装配了降采样管线)

    The Mali-C55 ISP can be configured in a number of ways to include or exclude
    blocks which may not be necessary. This control provides a way for the
    driver to communicate to userspace which of the blocks are fitted in the
    design.
    (Mali-C55 ISP 可以用多种方式配置，以包含或排除可能不必要的模块。此控件为
    驱动提供了一种向用户态告知设计中装配了哪些模块的方式。)