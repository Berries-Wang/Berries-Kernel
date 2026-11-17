.. SPDX-License-Identifier: GPL-2.0+

==========================================================================================================
Linux Base Virtual Function Driver for Intel(R) 10G Ethernet(Intel(R) 10G 以太网的 Linux 基础虚拟功能驱动)
==========================================================================================================

Intel 10 Gigabit Virtual Function Linux driver.
(Intel 万兆虚拟功能 Linux 驱动。)
Copyright(c) 1999-2018 Intel Corporation.

Contents(目录)
==============

- Identifying Your Adapter
  (识别你的适配器)
- Known Issues
  (已知问题)
- Support
  (支持)

This driver supports 82599, X540, X550, and X552-based virtual function devices
that can only be activated on kernels that support SR-IOV.
(该驱动支持基于 82599、X540、X550 和 X552 的虚拟功能设备，这些设备只能在支持 SR-IOV 的内核上激活。)

For questions related to hardware requirements, refer to the documentation
supplied with your Intel adapter. All hardware requirements listed apply to use
with Linux.
(有关硬件要求的问题，请参考随 Intel 适配器提供的文档。所列出的所有硬件要求均适用于 Linux 环境。)


Identifying Your Adapter(识别你的适配器)
========================================

The driver is compatible with devices based on the following:
(该驱动与基于以下产品的设备兼容：)

  * Intel(R) Ethernet Controller 82598
  * Intel(R) Ethernet Controller 82599
  * Intel(R) Ethernet Controller X520
  * Intel(R) Ethernet Controller X540
  * Intel(R) Ethernet Controller x550
  * Intel(R) Ethernet Controller X552
  * Intel(R) Ethernet Controller X553

For information on how to identify your adapter, and for the latest Intel
network drivers, refer to the Intel Support website:
https://www.intel.com/support
(有关如何识别你的适配器的信息以及最新的 Intel 网络驱动，请参阅 Intel 支持网站：https://www.intel.com/support)

Known Issues/Troubleshooting(已知问题/故障排查)
===============================================

SR-IOV requires the correct platform and OS support.
(SR-IOV 需要正确的平台和操作系统支持。)

The guest OS loading this driver must support MSI-X interrupts.
(加载该驱动的客户操作系统必须支持 MSI-X 中断。)

This driver is only supported as a loadable module at this time. Intel is not
supplying patches against the kernel source to allow for static linking of the
drivers.
(目前该驱动仅支持以可加载模块的形式使用。Intel 不提供针对内核源码的补丁来支持将这些驱动静态链接进内核。)

VLANs: There is a limit of a total of 64 shared VLANs to 1 or more VFs.
(VLAN：共享给一个或多个 VF 的 VLAN 总数上限为 64 个。)


Support(支持)
=============

For general information, go to the Intel support website at:
https://www.intel.com/support/
(一般信息请访问 Intel 支持网站：https://www.intel.com/support/)

If an issue is identified with the released source code on a supported kernel
with a supported adapter, email the specific information related to the issue
to intel-wired-lan@lists.osuosl.org.
(如果在受支持的内核上使用受支持的适配器时发现已发布源码存在问题，请将与该问题相关的具体信息发送邮件至 intel-wired-lan@lists.osuosl.org。)
