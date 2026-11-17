.. SPDX-License-Identifier: GPL-2.0

=======================================
stmmac (synopsys dwmac) devlink support
=======================================

(stmmac (synopsys dwmac) devlink 支持)

This document describes the devlink features implemented by the ``stmmac``
device driver.
(本文档描述 ``stmmac`` 设备驱动所实现的 devlink 功能。)

Parameters
==========
(参数)

The ``stmmac`` driver implements the following driver-specific parameters.
(``stmmac`` 驱动实现以下驱动特定参数。)

.. list-table:: Driver-specific parameters implemented
   :widths: 5 5 5 85

   * - Name (名称)
     - Type (类型)
     - Mode (模式)
     - Description (描述)
   * - ``phc_coarse_adj``
     - Boolean
     - runtime
     - Enable the Coarse timestamping mode, as defined in the DWMAC TRM.
       A detailed explanation of this timestamping mode can be found in the
       Socfpga Functionnal Description [1].
       (启用 DWMAC TRM 中定义的粗略 (Coarse) 时间戳模式。关于该时间戳模式的详细说明可参见 Socfpga 功能描述 [1]。)

       In Coarse mode, the ptp clock is expected to be fed by a high-precision
       clock that is externally adjusted, and the subsecond increment used for
       timestamping is set to 1/ptp_clock_rate.
       (在 Coarse 模式下，PTP 时钟预期由一个经外部调整的高精度时钟提供，用于打时间戳的亚秒增量被设置为 1/ptp_clock_rate。)

       In Fine mode (i.e. Coarse mode == false), the ptp clock frequency is
       continuously adjusted, but the subsecond increment is set to
       2/ptp_clock_rate.
       (在 Fine 模式下(即 Coarse mode == false)，PTP 时钟频率会被持续调整，但亚秒增量被设置为 2/ptp_clock_rate。)

       Coarse mode is suitable for PTP Grand Master operation. If unsure, leave
       the parameter to False.
       (Coarse 模式适合 PTP 主时钟 (Grand Master) 场景。如果不确定，请将该参数保持为 False。)

       [1] https://www.intel.com/content/www/us/en/docs/programmable/683126/21-2/functional-description-of-the-emac.html
