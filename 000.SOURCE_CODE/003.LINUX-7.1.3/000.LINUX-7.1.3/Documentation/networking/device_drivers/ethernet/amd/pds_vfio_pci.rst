.. SPDX-License-Identifier: GPL-2.0+
.. note: can be edited and viewed with /usr/bin/formiko-vim

==========================================================================================================
PCI VFIO driver for the AMD/Pensando(R) DSC adapter family(AMD/Pensando(R) DSC 适配器系列的 PCI VFIO 驱动)
==========================================================================================================

AMD/Pensando Linux VFIO PCI Device Driver
(AMD/Pensando Linux VFIO PCI 设备驱动)
Copyright(c) 2023 Advanced Micro Devices, Inc.

Overview(概述)
==============

The ``pds-vfio-pci`` module is a PCI driver that supports Live Migration
capable Virtual Function (VF) devices in the DSC hardware.
(``pds-vfio-pci`` 模块是一个 PCI 驱动，支持 DSC 硬件中具备实时迁移 (Live Migration) 能力的虚拟功能 (VF) 设备。)

Using the device(使用设备)
==========================

The pds-vfio-pci device is enabled via multiple configuration steps and
depends on the ``pds_core`` driver to create and enable SR-IOV Virtual
Function devices.
(pds-vfio-pci 设备需通过多个配置步骤来启用，并依赖 ``pds_core`` 驱动来创建和启用 SR-IOV 虚拟功能设备。)

Shown below are the steps to bind the driver to a VF and also to the
associated auxiliary device created by the ``pds_core`` driver. This
example assumes the pds_core and pds-vfio-pci modules are already
loaded.
(下面展示把该驱动绑定到 VF 以及绑定到由 ``pds_core`` 驱动创建的相应辅助设备的步骤。本示例假定 pds_core 和 pds-vfio-pci 模块已经加载。)

.. code-block:: bash
  :name: example-setup-script

  #!/bin/bash

  PF_BUS="0000:60"
  PF_BDF="0000:60:00.0"
  VF_BDF="0000:60:00.1"

  # Prevent non-vfio VF driver from probing the VF device
  echo 0 > /sys/class/pci_bus/$PF_BUS/device/$PF_BDF/sriov_drivers_autoprobe

  # Create single VF for Live Migration via pds_core
  echo 1 > /sys/bus/pci/drivers/pds_core/$PF_BDF/sriov_numvfs

  # Allow the VF to be bound to the pds-vfio-pci driver
  echo "pds-vfio-pci" > /sys/class/pci_bus/$PF_BUS/device/$VF_BDF/driver_override

  # Bind the VF to the pds-vfio-pci driver
  echo "$VF_BDF" > /sys/bus/pci/drivers/pds-vfio-pci/bind

After performing the steps above, a file in /dev/vfio/<iommu_group>
should have been created.
(执行上述步骤之后，/dev/vfio/<iommu_group> 中应当已创建一个文件。)


Enabling the driver(启用驱动)
=============================

The driver is enabled via the standard kernel configuration system,
using the make command::
(该驱动通过标准内核配置系统启用，使用 make 命令：)

  make oldconfig/menuconfig/etc.

The driver is located in the menu structure at:
(该驱动位于菜单结构中的以下位置：)

  -> Device Drivers
    -> VFIO Non-Privileged userspace driver framework
      -> VFIO support for PDS PCI devices

Support(支持)
=============

For general Linux networking support, please use the netdev mailing
list, which is monitored by Pensando personnel::
(如需一般的 Linux 网络支持，请使用由 Pensando 人员关注的 netdev 邮件列表：)

  netdev@vger.kernel.org

For more specific support needs, please use the Pensando driver support
email::
(如需更具体的支持需求，请使用 Pensando 驱动支持邮箱：)

  drivers@pensando.io
