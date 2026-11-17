.. SPDX-License-Identifier: GPL-2.0

==============
Devlink Params
==============

(Devlink 参数)

``devlink`` provides capability for a driver to expose device parameters for low
level device functionality. Since devlink can operate at the device-wide
level, it can be used to provide configuration that may affect multiple
ports on a single device.
(``devlink`` 为驱动程序提供了暴露设备底层功能参数的能力。由于 devlink 可以在设备级层面上
操作，因此它可以用于提供可能影响单个设备上多个端口的配置。)

This document describes a number of generic parameters that are supported
across multiple drivers. Each driver is also free to add their own
parameters. Each driver must document the specific parameters they support,
whether generic or not.
(本文档描述了多个驱动程序都支持的一些通用参数。每个驱动程序也可以自由添加自己的参数。每个驱动
程序都必须记录其所支持的特定参数，无论是否为通用参数。)

Configuration modes
(配置模式)
===================

Parameters may be set in different configuration modes.
(参数可以在不同的配置模式下设置。)

.. list-table:: Possible configuration modes
   :widths: 5 90

   * - Name (名称)
     - Description (描述)
   * - ``runtime``
     - set while the driver is running, and takes effect immediately. No
       reset is required.
       (在驱动程序运行时设置，并立即生效。无需复位。)
   * - ``driverinit``
     - applied while the driver initializes. Requires the user to restart
       the driver using the ``devlink`` reload command.
       (在驱动程序初始化时应用。需要用户使用 ``devlink`` reload 命令重启驱动程序。)
   * - ``permanent``
     - written to the device's non-volatile memory. A hard reset is required
       for it to take effect.
       (写入设备的非易失性存储器。需要硬复位才能生效。)

Reloading
(重新加载)
---------

In order for ``driverinit`` parameters to take effect, the driver must
support reloading via the ``devlink-reload`` command. This command will
request a reload of the device driver.
(要使 ``driverinit`` 参数生效，驱动程序必须支持通过 ``devlink-reload`` 命令进行重新加载。
该命令将请求重新加载设备驱动程序。)

Default parameter values
(默认参数值)
=========================

Drivers may optionally export default values for parameters of cmode
``runtime`` and ``permanent``. For ``driverinit`` parameters, the last
value set by the driver will be used as the default value. Drivers can
also support resetting params with cmode ``runtime`` and ``permanent``
to their default values. Resetting ``driverinit`` params is supported
by devlink core without additional driver support needed.
(驱动程序可以选择为 cmode 为 ``runtime`` 和 ``permanent`` 的参数导出默认值。对于
``driverinit`` 参数，驱动程序最后设置的值将用作默认值。驱动程序也可以支持将 cmode 为
``runtime`` 和 ``permanent`` 的参数重置为其默认值。重置 ``driverinit`` 参数由 devlink
核心支持，无需额外的驱动程序支持。)

.. _devlink_params_generic:

Generic configuration parameters
(通用配置参数)
================================

The following is a list of generic configuration parameters that drivers may
add. Use of generic parameters is preferred over each driver creating their
own name.
(以下是驱动程序可以添加的通用配置参数列表。优先使用通用参数，而不是由每个驱动程序自行创建
各自的名称。)

.. list-table:: List of generic parameters
   :widths: 5 5 90

   * - Name (名称)
     - Type (类型)
     - Description (描述)
   * - ``enable_sriov``
     - Boolean
     - Enable Single Root I/O Virtualization (SRIOV) in the device.
       (在设备中启用单根 I/O 虚拟化(SRIOV)。)
   * - ``ignore_ari``
     - Boolean
     - Ignore Alternative Routing-ID Interpretation (ARI) capability. If
       enabled, the adapter will ignore ARI capability even when the
       platform has support enabled. The device will create the same number
       of partitions as when the platform does not support ARI.
       (忽略替代路由 ID 解释(ARI)能力。如果启用，即使平台已开启对该能力的支持，适配器也会
       忽略 ARI 能力。设备将创建与平台不支持 ARI 时相同数量的分区。)
   * - ``msix_vec_per_pf_max``
     - u32
     - Provides the maximum number of MSI-X interrupts that a device can
       create. Value is the same across all physical functions (PFs) in the
       device.
       (提供设备可以创建的 MSI-X 中断的最大数量。该值在设备中的所有物理功能(PF)上都相同。)
   * - ``msix_vec_per_pf_min``
     - u32
     - Provides the minimum number of MSI-X interrupts required for the
       device to initialize. Value is the same across all physical functions
       (PFs) in the device.
       (提供设备初始化所需的 MSI-X 中断的最小数量。该值在设备中的所有物理功能(PF)上都相同。)
   * - ``fw_load_policy``
     - u8
     - Control the device's firmware loading policy.
       (控制设备的固件加载策略。)

        - ``DEVLINK_PARAM_FW_LOAD_POLICY_VALUE_DRIVER`` (0)
          Load firmware version preferred by the driver.
          (加载驱动程序偏好的固件版本。)
        - ``DEVLINK_PARAM_FW_LOAD_POLICY_VALUE_FLASH`` (1)
          Load firmware currently stored in flash.
          (加载当前存储在 flash 中的固件。)
        - ``DEVLINK_PARAM_FW_LOAD_POLICY_VALUE_DISK`` (2)
          Load firmware currently available on host's disk.
          (加载当前主机磁盘上可用的固件。)
   * - ``reset_dev_on_drv_probe``
     - u8
     - Controls the device's reset policy on driver probe.
       (控制在驱动程序探测时设备的复位策略。)

        - ``DEVLINK_PARAM_RESET_DEV_ON_DRV_PROBE_VALUE_UNKNOWN`` (0)
          Unknown or invalid value.
          (未知或无效的值。)
        - ``DEVLINK_PARAM_RESET_DEV_ON_DRV_PROBE_VALUE_ALWAYS`` (1)
          Always reset device on driver probe.
          (驱动程序探测时总是复位设备。)
        - ``DEVLINK_PARAM_RESET_DEV_ON_DRV_PROBE_VALUE_NEVER`` (2)
          Never reset device on driver probe.
          (驱动程序探测时从不复位设备。)
        - ``DEVLINK_PARAM_RESET_DEV_ON_DRV_PROBE_VALUE_DISK`` (3)
          Reset the device only if firmware can be found in the filesystem.
          (仅当在文件系统中能找到固件时才复位设备。)
   * - ``enable_roce``
     - Boolean
     - Enable handling of RoCE traffic in the device.
       (在设备中启用 RoCE 流量的处理。)
   * - ``enable_eth``
     - Boolean
     - When enabled, the device driver will instantiate Ethernet specific
       auxiliary device of the devlink device.
       (启用后，设备驱动程序将实例化 devlink 设备的以太网专用辅助设备。)
   * - ``enable_rdma``
     - Boolean
     - When enabled, the device driver will instantiate RDMA specific
       auxiliary device of the devlink device.
       (启用后，设备驱动程序将实例化 devlink 设备的 RDMA 专用辅助设备。)
   * - ``enable_vnet``
     - Boolean
     - When enabled, the device driver will instantiate VDPA networking
       specific auxiliary device of the devlink device.
       (启用后，设备驱动程序将实例化 devlink 设备的 VDPA 网络专用辅助设备。)
   * - ``enable_iwarp``
     - Boolean
     - Enable handling of iWARP traffic in the device.
       (在设备中启用 iWARP 流量的处理。)
   * - ``internal_err_reset``
     - Boolean
     - When enabled, the device driver will reset the device on internal
       errors.
       (启用后，设备驱动程序将在发生内部错误时复位设备。)
   * - ``max_macs``
     - u32
     - Typically macvlan, vlan net devices mac are also programmed in their
       parent netdevice's Function rx filter. This parameter limit the
       maximum number of unicast mac address filters to receive traffic from
       per ethernet port of this device.
       (通常情况下，macvlan、vlan 等网络设备的 mac 地址也会被编程到其父网络设备的 Function
       rx 过滤器中。此参数限制了该设备每个以太网端口可用于接收流量的单播 mac 地址过滤器的
       最大数量。)
   * - ``region_snapshot_enable``
     - Boolean
     - Enable capture of ``devlink-region`` snapshots.
       (启用 ``devlink-region`` 快照的捕获。)
   * - ``enable_remote_dev_reset``
     - Boolean
     - Enable device reset by remote host. When cleared, the device driver
       will NACK any attempt of other host to reset the device. This parameter
       is useful for setups where a device is shared by different hosts, such
       as multi-host setup.
       (允许远程主机复位设备。清除后，设备驱动程序将对其他主机复位设备的任何尝试回复 NACK。
       此参数适用于设备由不同主机共享的场景，例如多主机设置。)
   * - ``io_eq_size``
     - u32
     - Control the size of I/O completion EQs.
       (控制 I/O 完成 EQ 的大小。)
   * - ``event_eq_size``
     - u32
     - Control the size of asynchronous control events EQ.
       (控制异步控制事件 EQ 的大小。)
   * - ``enable_phc``
     - Boolean
     - Enable PHC (PTP Hardware Clock) functionality in the device.
       (在设备中启用 PHC(PTP 硬件时钟)功能。)
   * - ``clock_id``
     - u64
     - Clock ID used by the device for registering DPLL devices and pins.
       (设备注册 DPLL 设备和引脚时所使用的时钟 ID。)
   * - ``total_vfs``
     - u32
     - The max number of Virtual Functions (VFs) exposed by the PF.
       after reboot/pci reset, 'sriov_totalvfs' entry under the device's sysfs
       directory will report this value.
       (PF 暴露的虚拟功能(VF)的最大数量。重启/pci 复位后，设备 sysfs 目录下的
       'sriov_totalvfs' 条目将报告此值。)
   * - ``num_doorbells``
     - u32
     - Controls the number of doorbells used by the device.
       (控制设备使用的门铃(doorbell)数量。)
   * - ``max_mac_per_vf``
     - u32
     - Controls the maximum number of MAC address filters that can be assigned
       to a Virtual Function (VF).
       (控制可以分配给虚拟功能(VF)的 MAC 地址过滤器的最大数量。)
