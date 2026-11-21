.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

================
Guidelines(指南)
================

This documents contains the guidelines for nova-core. Additionally, all common
guidelines of the Nova project do apply.
(本文档包含 nova-core 的准则。此外，Nova 项目的所有通用准则同样适用。)

Driver API(驱动 API)
====================

One main purpose of nova-core is to implement the abstraction around the
firmware interface of GSP and provide a firmware (version) independent API for
2nd level drivers, such as nova-drm or the vGPU manager VFIO driver.
(nova-core 的一个主要目的是实现围绕 GSP 固件接口的抽象，并为二级驱动 (例如 nova-drm 或 vGPU manager VFIO 驱动) 提供与固件 (版本) 无关的 API。)

Therefore, it is not permitted to leak firmware (version) specifics, through the
driver API, to 2nd level drivers.
(因此，不允许通过驱动 API 把固件 (版本) 特有的细节泄露给二级驱动。)

Acceptance Criteria(验收标准)
==============================

- To the extend possible, patches submitted to nova-core must be tested for
  regressions with all 2nd level drivers.
  (在可行的范围内，提交到 nova-core 的补丁必须针对所有二级驱动进行回归测试。)
