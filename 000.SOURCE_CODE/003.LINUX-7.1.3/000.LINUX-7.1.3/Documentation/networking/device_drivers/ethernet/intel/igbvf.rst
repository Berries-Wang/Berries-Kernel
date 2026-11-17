.. SPDX-License-Identifier: GPL-2.0+

========================================================================================================
Linux Base Virtual Function Driver for Intel(R) 1G Ethernet(Intel(R) 1G 以太网的 Linux 基础虚拟功能驱动)
========================================================================================================

Intel Gigabit Virtual Function Linux driver.
(Intel 千兆虚拟功能 Linux 驱动。)
Copyright(c) 1999-2018 Intel Corporation.

Contents(目录)
==============

- Identifying Your Adapter
  (识别你的适配器)
- Additional Configurations
  (其他配置)
- Support
  (支持)

This driver supports Intel 82576-based virtual function devices-based virtual
function devices that can only be activated on kernels that support SR-IOV.
(该驱动支持基于 Intel 82576 的虚拟功能设备，这些设备只能在支持 SR-IOV 的内核上激活。)

SR-IOV requires the correct platform and OS support.
(SR-IOV 需要正确的平台和操作系统支持。)

The guest OS loading this driver must support MSI-X interrupts.
(加载该驱动的客户操作系统必须支持 MSI-X 中断。)

For questions related to hardware requirements, refer to the documentation
supplied with your Intel adapter. All hardware requirements listed apply to use
with Linux.
(有关硬件要求的问题，请参考随 Intel 适配器提供的文档。所列出的所有硬件要求均适用于 Linux 环境。)

Driver information can be obtained using ethtool, lspci, and ifconfig.
Instructions on updating ethtool can be found in the section Additional
Configurations later in this document.
(可以使用 ethtool、lspci 和 ifconfig 获取驱动信息。关于更新 ethtool 的说明见本文档后面的 Additional Configurations 一节。)

NOTE: There is a limit of a total of 32 shared VLANs to 1 or more VFs.
(注意：共享给一个或多个 VF 的 VLAN 总数上限为 32 个。)


Identifying Your Adapter(识别你的适配器)
========================================

For information on how to identify your adapter, and for the latest Intel
network drivers, refer to the Intel Support website:
https://www.intel.com/support
(有关如何识别你的适配器的信息以及最新的 Intel 网络驱动，请参阅 Intel 支持网站：https://www.intel.com/support)


Additional Features and Configurations(其他特性与配置)
======================================================

ethtool
-------
(ethtool)

The driver utilizes the ethtool interface for driver configuration and
diagnostics, as well as displaying statistical information. The latest ethtool
version is required for this functionality. Download it at:
(该驱动利用 ethtool 接口进行驱动配置和诊断，以及显示统计信息。此功能需要最新版本的 ethtool。可从以下地址下载：)

https://www.kernel.org/pub/software/network/ethtool/


Support(支持)
=============

For general information, go to the Intel support website at:
https://www.intel.com/support/
(一般信息请访问 Intel 支持网站：https://www.intel.com/support/)

If an issue is identified with the released source code on a supported kernel
with a supported adapter, email the specific information related to the issue
to intel-wired-lan@lists.osuosl.org.
(如果在受支持的内核上使用受支持的适配器时发现已发布源码存在问题，请将与该问题相关的具体信息发送邮件至 intel-wired-lan@lists.osuosl.org。)
