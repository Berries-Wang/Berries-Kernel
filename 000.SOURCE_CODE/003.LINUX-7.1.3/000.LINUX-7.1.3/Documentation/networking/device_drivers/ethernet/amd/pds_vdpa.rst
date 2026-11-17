.. SPDX-License-Identifier: GPL-2.0+
.. note: can be edited and viewed with /usr/bin/formiko-vim

==========================================================
PCI vDPA driver for the AMD/Pensando(R) DSC adapter family
==========================================================

(AMD/Pensando(R) DSC 适配器系列的 PCI vDPA 驱动)

AMD/Pensando vDPA VF Device Driver
(AMD/Pensando vDPA VF 设备驱动)

Copyright(c) 2023 Advanced Micro Devices, Inc

Overview
========
(概述)

The ``pds_vdpa`` driver is an auxiliary bus driver that supplies
a vDPA device for use by the virtio network stack.  It is used with
the Pensando Virtual Function devices that offer vDPA and virtio queue
services.  It depends on the ``pds_core`` driver and hardware for the PF
and VF PCI handling as well as for device configuration services.
(``pds_vdpa`` 驱动是一个辅助总线 (auxiliary bus) 驱动，为 virtio 网络栈提供一个 vDPA 设备。它与提供 vDPA 和 virtio 队列服务的 Pensando 虚拟功能设备配合使用。它依赖 ``pds_core`` 驱动及硬件来完成 PF 和 VF 的 PCI 处理以及设备配置服务。)

Using the device
================
(使用设备)

The ``pds_vdpa`` device is enabled via multiple configuration steps and
depends on the ``pds_core`` driver to create and enable SR-IOV Virtual
Function devices.  After the VFs are enabled, we enable the vDPA service
in the ``pds_core`` device to create the auxiliary devices used by pds_vdpa.
(``pds_vdpa`` 设备需通过多个配置步骤来启用，并依赖 ``pds_core`` 驱动来创建和启用 SR-IOV 虚拟功能设备。在 VF 启用之后，我们在 ``pds_core`` 设备中启用 vDPA 服务，以创建 pds_vdpa 所使用的辅助设备。)

Example steps:
(示例步骤：)

.. code-block:: bash

  #!/bin/bash

  modprobe pds_core
  modprobe vdpa
  modprobe pds_vdpa

  PF_BDF=`ls /sys/module/pds_core/drivers/pci\:pds_core/*/sriov_numvfs | awk -F / '{print $7}'`

  # Enable vDPA VF auxiliary device(s) in the PF
  devlink dev param set pci/$PF_BDF name enable_vnet cmode runtime value true

  # Create a VF for vDPA use
  echo 1 > /sys/bus/pci/drivers/pds_core/$PF_BDF/sriov_numvfs

  # Find the vDPA services/devices available
  PDS_VDPA_MGMT=`vdpa mgmtdev show | grep vDPA | head -1 | cut -d: -f1`

  # Create a vDPA device for use in virtio network configurations
  vdpa dev add name vdpa1 mgmtdev $PDS_VDPA_MGMT mac 00:11:22:33:44:55

  # Set up an ethernet interface on the vdpa device
  modprobe virtio_vdpa



Enabling the driver
===================
(启用驱动)

The driver is enabled via the standard kernel configuration system,
using the make command::
(该驱动通过标准内核配置系统启用，使用 make 命令：)

  make oldconfig/menuconfig/etc.

The driver is located in the menu structure at:
(该驱动位于菜单结构中的以下位置：)

  -> Device Drivers
    -> Network device support (NETDEVICES [=y])
      -> Ethernet driver support
        -> Pensando devices
          -> Pensando Ethernet PDS_VDPA Support

Support
=======
(支持)

For general Linux networking support, please use the netdev mailing
list, which is monitored by Pensando personnel::
(如需一般的 Linux 网络支持，请使用由 Pensando 人员关注的 netdev 邮件列表：)

  netdev@vger.kernel.org

For more specific support needs, please use the Pensando driver support
email::
(如需更具体的支持需求，请使用 Pensando 驱动支持邮箱：)

  drivers@pensando.io
