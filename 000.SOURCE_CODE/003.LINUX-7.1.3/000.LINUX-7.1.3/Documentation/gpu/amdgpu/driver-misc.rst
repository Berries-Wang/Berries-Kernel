====================================================
 Misc AMDGPU driver information(AMDGPU 驱动杂项信息)
====================================================

GPU Product Information(GPU 产品信息)
=====================================

Information about the GPU can be obtained on certain cards
via sysfs
(在某些卡上，可以通过 sysfs 获取 GPU 的相关信息。)

product_name(产品名称)
----------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_fru_eeprom.c
   :doc: product_name

product_number(产品编号)
------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_fru_eeprom.c
   :doc: product_number

serial_number(序列号)
---------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_fru_eeprom.c
   :doc: serial_number

fru_id(FRU ID)
--------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_fru_eeprom.c
   :doc: fru_id

manufacturer(制造商)
--------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_fru_eeprom.c
   :doc: manufacturer

unique_id(唯一 ID)
------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: unique_id

board_info(板卡信息)
--------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_device.c
   :doc: board_info

GPU Memory Usage Information(GPU 内存用量信息)
==============================================

Various memory accounting can be accessed via sysfs
(可以通过 sysfs 访问各种内存统计信息。)

mem_info_vram_total(VRAM 总量)
------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_vram_mgr.c
   :doc: mem_info_vram_total

mem_info_vram_used(已用 VRAM)
-----------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_vram_mgr.c
   :doc: mem_info_vram_used

mem_info_vis_vram_total(可见 VRAM 总量)
---------------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_vram_mgr.c
   :doc: mem_info_vis_vram_total

mem_info_vis_vram_used(已用可见 VRAM)
-------------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_vram_mgr.c
   :doc: mem_info_vis_vram_used

mem_info_gtt_total(GTT 总量)
----------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_gtt_mgr.c
   :doc: mem_info_gtt_total

mem_info_gtt_used(已用 GTT)
---------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_gtt_mgr.c
   :doc: mem_info_gtt_used

PCIe Accounting Information(PCIe 统计信息)
==========================================

pcie_bw(PCIe 带宽)
------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: pcie_bw

pcie_replay_count(PCIe 重放计数)
--------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_device.c
   :doc: pcie_replay_count

GPU SmartShift Information(GPU SmartShift 信息)
===============================================

GPU SmartShift information via sysfs
(通过 sysfs 获取 GPU SmartShift 信息。)

smartshift_apu_power(APU 功率)
------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: smartshift_apu_power

smartshift_dgpu_power(dGPU 功率)
--------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: smartshift_dgpu_power

smartshift_bias(SmartShift 偏置)
--------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/pm/amdgpu_pm.c
   :doc: smartshift_bias

UMA Carveout(UMA 划分)
======================

Some versions of Atom ROM expose available options for the VRAM carveout sizes,
and allow changes to the carveout size via the ATCS function code 0xA on supported
BIOS implementations.
(某些版本的 Atom ROM 会公开可用的 VRAM 划分 (carveout) 大小选项，并允许在受
支持的 BIOS 实现上通过 ATCS 功能码 0xA 更改划分大小。)

For those platforms, users can use the following files under uma/ to set the
carveout size, in a way similar to what Windows users can do in the "Tuning"
tab in AMD Adrenalin.
(对于这些平台，用户可以在 uma/ 下使用以下文件来设置划分大小，其方式类似于
Windows 用户在 AMD Adrenalin 的 "Tuning"（调整）选项卡中所做的操作。)

Note that for BIOS implementations that don't support this, these files will not
be created at all.
(注意，对于不支持此功能的 BIOS 实现，这些文件根本不会被创建。)

uma/carveout_options(UMA 划分选项)
----------------------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_device.c
   :doc: uma/carveout_options

uma/carveout(UMA 划分)
----------------------

.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_device.c
   :doc: uma/carveout
