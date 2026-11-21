====================================
 AMDGPU RAS Support(AMDGPU RAS 支持)
====================================

The AMDGPU RAS interfaces are exposed via sysfs (for informational queries) and
debugfs (for error injection).
(AMDGPU RAS 接口通过 sysfs (用于信息查询) 和 debugfs (用于错误注入) 暴露。)

RAS debugfs/sysfs Control and Error Injection Interfaces(RAS debugfs/sysfs 控制与错误注入接口)
==============================================================================================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_ras.c
   :doc: AMDGPU RAS debugfs control interface

RAS Reboot Behavior for Unrecoverable Errors(不可恢复错误的 RAS 重启行为)
=========================================================================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_ras.c
   :doc: AMDGPU RAS Reboot Behavior for Unrecoverable Errors

RAS Error Count sysfs Interface(RAS 错误计数 sysfs 接口)
========================================================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_ras.c
   :doc: AMDGPU RAS sysfs Error Count Interface

RAS EEPROM debugfs Interface(RAS EEPROM debugfs 接口)
=====================================================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_ras.c
   :doc: AMDGPU RAS debugfs EEPROM table reset interface

RAS VRAM Bad Pages sysfs Interface(RAS VRAM 坏页 sysfs 接口)
============================================================

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_ras.c
   :doc: AMDGPU RAS sysfs gpu_vram_bad_pages Interface

Sample Code(示例代码)
==========================================================
Sample code for testing error injection can be found here:
https://cgit.freedesktop.org/mesa/drm/tree/tests/amdgpu/ras_tests.c
(用于测试错误注入的示例代码可以在这里找到：
https://cgit.freedesktop.org/mesa/drm/tree/tests/amdgpu/ras_tests.c)

This is part of the libdrm amdgpu unit tests which cover several areas of the GPU.
There are four sets of tests:
(它属于 libdrm amdgpu 单元测试的一部分，覆盖 GPU 的多个方面。共有四组测试：)

RAS Basic Test
(RAS 基本测试)

The test verifies the RAS feature enabled status and makes sure the necessary sysfs and debugfs files
are present.
(该测试验证 RAS 功能的启用状态，并确保必要的 sysfs 和 debugfs 文件存在。)

RAS Query Test
(RAS 查询测试)

This test checks the RAS availability and enablement status for each supported IP block as well as
the error counts.
(该测试检查每个受支持 IP 块的 RAS 可用性与启用状态以及错误计数。)

RAS Inject Test
(RAS 注入测试)

This test injects errors for each IP.
(该测试为每个 IP 注入错误。)

RAS Disable Test
(RAS 禁用测试)

This test tests disabling of RAS features for each IP block.
(该测试测试对每个 IP 块禁用 RAS 功能。)
