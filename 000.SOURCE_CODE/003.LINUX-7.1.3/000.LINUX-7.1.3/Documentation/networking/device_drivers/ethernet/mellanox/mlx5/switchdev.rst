.. SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
.. include:: <isonum.txt>

=========
Switchdev
=========

(Switchdev)

:Copyright: |copy| 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
(版权所有 |copy| 2023, NVIDIA CORPORATION & AFFILIATES。保留所有权利。)

.. _mlx5_bridge_offload:

Bridge offload(桥卸载)
======================

The mlx5 driver implements support for offloading bridge rules when in switchdev
mode. Linux bridge FDBs are automatically offloaded when mlx5 switchdev
representor is attached to bridge.
(mlx5 驱动实现了在 switchdev 模式下卸载桥规则的支持。当 mlx5 switchdev 代表 (representor) 被挂接到桥上时，Linux 桥 FDB 会被自动卸载。)

- Change device to switchdev mode::
  (把设备改为 switchdev 模式：)

    $ devlink dev eswitch set pci/0000:06:00.0 mode switchdev

- Attach mlx5 switchdev representor 'enp8s0f0' to bridge netdev 'bridge1'::
  (把 mlx5 switchdev 代表 'enp8s0f0' 挂接到桥网络设备 'bridge1'：)

    $ ip link set enp8s0f0 master bridge1

VLANs(VLAN)
-----------

Following bridge VLAN functions are supported by mlx5:
(mlx5 支持以下桥 VLAN 功能：)

- VLAN filtering (including multiple VLANs per port)::
  (VLAN 过滤（包括每端口多个 VLAN）：)

    $ ip link set bridge1 type bridge vlan_filtering 1
    $ bridge vlan add dev enp8s0f0 vid 2-3

- VLAN push on bridge ingress::
  (在桥入口推入 VLAN：)

    $ bridge vlan add dev enp8s0f0 vid 3 pvid

- VLAN pop on bridge egress::
  (在桥出口弹出 VLAN：)

    $ bridge vlan add dev enp8s0f0 vid 3 untagged

Subfunction(子功能)
===================

Subfunction which are spawned over the E-switch are created only with devlink
device, and by default all the SF auxiliary devices are disabled.
This will allow user to configure the SF before the SF have been fully probed,
which will save time.
(通过 E-switch 产生的子功能 (Subfunction) 只能随 devlink 设备一起创建，并且默认所有 SF 辅助设备都是禁用的。这使用户可以在 SF 被完全探测之前先对 SF 进行配置，从而节省时间。)

Usage example:
(用法示例：)

- Create SF::
  (创建 SF：)

    $ devlink port add pci/0000:08:00.0 flavour pcisf pfnum 0 sfnum 11
    $ devlink port function set pci/0000:08:00.0/32768 hw_addr 00:00:00:00:00:11 state active

- Enable ETH auxiliary device::
  (启用 ETH 辅助设备：)

    $ devlink dev param set auxiliary/mlx5_core.sf.1 name enable_eth value true cmode driverinit

- Now, in order to fully probe the SF, use devlink reload::
  (现在，为了完全探测 SF，使用 devlink reload：)

    $ devlink dev reload auxiliary/mlx5_core.sf.1

mlx5 supports ETH,rdma and vdpa (vnet) auxiliary devices devlink params (see :ref:`Documentation/networking/devlink/devlink-params.rst <devlink_params_generic>`).
(mlx5 支持 ETH、rdma 和 vdpa (vnet) 辅助设备的 devlink 参数（参见 :ref:`Documentation/networking/devlink/devlink-params.rst <devlink_params_generic>` ）。)

mlx5 supports subfunction management using devlink port (see :ref:`Documentation/networking/devlink/devlink-port.rst <devlink_port>`) interface.
(mlx5 支持使用 devlink port（参见 :ref:`Documentation/networking/devlink/devlink-port.rst <devlink_port>` ）接口进行子功能管理。)

A subfunction has its own function capabilities and its own resources. This
means a subfunction has its own dedicated queues (txq, rxq, cq, eq). These
queues are neither shared nor stolen from the parent PCI function.
(子功能拥有自己的函数能力和自己的资源。这意味着子功能拥有自己的专用队列（txq、rxq、cq、eq）。这些队列既不与父 PCI 函数共享，也不会从中抢占。)

When a subfunction is RDMA capable, it has its own QP1, GID table, and RDMA
resources neither shared nor stolen from the parent PCI function.
(当子功能具备 RDMA 能力时，它拥有自己的 QP1、GID 表和 RDMA 资源，这些资源既不与父 PCI 函数共享，也不会从中抢占。)

A subfunction has a dedicated window in PCI BAR space that is not shared
with the other subfunctions or the parent PCI function. This ensures that all
devices (netdev, rdma, vdpa, etc.) of the subfunction accesses only assigned
PCI BAR space.
(子功能在 PCI BAR 空间中拥有一个专用窗口，该窗口不与其他子功能或父 PCI 函数共享。这确保子功能的所有设备（netdev、rdma、vdpa 等）只访问分配给它的 PCI BAR 空间。)

A subfunction supports eswitch representation through which it supports tc
offloads. The user configures eswitch to send/receive packets from/to
the subfunction port.
(子功能通过 eswitch 表示 (representation) 来支持 tc 卸载。用户配置 eswitch 与子功能端口之间收发数据包。)

Subfunctions share PCI level resources such as PCI MSI-X IRQs with
other subfunctions and/or with its parent PCI function.
(子功能与其他子功能和/或其父 PCI 函数共享 PCI 层面的资源，例如 PCI MSI-X 中断。)

Example mlx5 software, system, and device view::

       _______
      | admin |
      | user  |----------
      |_______|         |
          |             |
      ____|____       __|______            _________________
     |         |     |         |          |                 |
     | devlink |     | tc tool |          |    user         |
     | tool    |     |_________|          | applications    |
     |_________|         |                |_________________|
           |             |                   |          |
           |             |                   |          |         Userspace
 +---------|-------------|-------------------|----------|--------------------+
           |             |           +----------+   +----------+   Kernel
           |             |           |  netdev  |   | rdma dev |
           |             |           +----------+   +----------+
   (devlink port add/del |              ^               ^
    port function set)   |              |               |
           |             |              +---------------|
      _____|___          |              |        _______|_______
     |         |         |              |       | mlx5 class    |
     | devlink |   +------------+       |       |   drivers     |
     | kernel  |   | rep netdev |       |       |(mlx5_core,ib) |
     |_________|   +------------+       |       |_______________|
           |             |              |               ^
   (devlink ops)         |              |          (probe/remove)
  _________|________     |              |           ____|________
 | subfunction      |    |     +---------------+   | subfunction |
 | management driver|-----     | subfunction   |---|  driver     |
 | (mlx5_core)      |          | auxiliary dev |   | (mlx5_core) |
 |__________________|          +---------------+   |_____________|
           |                                            ^
  (sf add/del, vhca events)                             |
           |                                      (device add/del)
      _____|____                                    ____|________
     |          |                                  | subfunction |
     |  PCI NIC |--- activate/deactivate events--->| host driver |
     |__________|                                  | (mlx5_core) |
                                                   |_____________|

(mlx5 软件、系统和设备视图示例：)

Subfunction is created using devlink port interface.
(子功能使用 devlink port 接口创建。)

- Change device to switchdev mode::
  (把设备改为 switchdev 模式：)

    $ devlink dev eswitch set pci/0000:06:00.0 mode switchdev

- Add a devlink port of subfunction flavour::
  (添加一个 subfunction 类型的 devlink 端口：)

    $ devlink port add pci/0000:06:00.0 flavour pcisf pfnum 0 sfnum 88
    pci/0000:06:00.0/32768: type eth netdev eth6 flavour pcisf controller 0 pfnum 0 sfnum 88 external false splittable false
      function:
        hw_addr 00:00:00:00:00:00 state inactive opstate detached

- Show a devlink port of the subfunction::
  (显示子功能的 devlink 端口：)

    $ devlink port show pci/0000:06:00.0/32768
    pci/0000:06:00.0/32768: type eth netdev enp6s0pf0sf88 flavour pcisf pfnum 0 sfnum 88
      function:
        hw_addr 00:00:00:00:00:00 state inactive opstate detached

- Delete a devlink port of subfunction after use::
  (使用完毕后删除子功能的 devlink 端口：)

    $ devlink port del pci/0000:06:00.0/32768

Function attributes(函数属性)
=============================

The mlx5 driver provides a mechanism to setup PCI VF/SF function attributes in
a unified way for SmartNIC and non-SmartNIC.
(mlx5 驱动提供一种为 SmartNIC 和非 SmartNIC 以统一方式设置 PCI VF/SF 函数属性的机制。)

This is supported only when the eswitch mode is set to switchdev. Port function
configuration of the PCI VF/SF is supported through devlink eswitch port.
(仅在 eswitch 模式设为 switchdev 时才支持此功能。PCI VF/SF 的端口函数配置通过 devlink eswitch 端口支持。)

Port function attributes should be set before PCI VF/SF is enumerated by the
driver.
(应在驱动枚举 PCI VF/SF 之前设置端口函数属性。)

MAC address setup(MAC 地址设置)
-------------------------------

mlx5 driver support devlink port function attr mechanism to setup MAC
address. (refer to Documentation/networking/devlink/devlink-port.rst)
(mlx5 驱动支持 devlink 端口函数属性机制来设置 MAC 地址。（参见 Documentation/networking/devlink/devlink-port.rst）)

RoCE capability setup(RoCE 能力设置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Not all mlx5 PCI devices/SFs require RoCE capability.
(并非所有 mlx5 PCI 设备/SF 都需要 RoCE 能力。)

When RoCE capability is disabled, it saves 1 Mbytes worth of system memory per
PCI devices/SF.
(当 RoCE 能力被禁用时，每个 PCI 设备/SF 可节省 1 MB 的系统内存。)

mlx5 driver support devlink port function attr mechanism to setup RoCE
capability. (refer to Documentation/networking/devlink/devlink-port.rst)
(mlx5 驱动支持 devlink 端口函数属性机制来设置 RoCE 能力。（参见 Documentation/networking/devlink/devlink-port.rst）)

migratable capability setup(可迁移 (migratable) 能力设置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

User who wants mlx5 PCI VFs to be able to perform live migration need to
explicitly enable the VF migratable capability.
(希望 mlx5 PCI VF 能够执行实时迁移的用户需要显式启用 VF 的 migratable 能力。)

mlx5 driver support devlink port function attr mechanism to setup migratable
capability. (refer to Documentation/networking/devlink/devlink-port.rst)
(mlx5 驱动支持 devlink 端口函数属性机制来设置 migratable 能力。（参见 Documentation/networking/devlink/devlink-port.rst）)

IPsec crypto capability setup(IPsec 加密 (crypto) 能力设置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

User who wants mlx5 PCI VFs to be able to perform IPsec crypto offloading need
to explicitly enable the VF ipsec_crypto capability. Enabling IPsec capability
for VFs is supported starting with ConnectX6dx devices and above. When a VF has
IPsec capability enabled, any IPsec offloading is blocked on the PF.
(希望 mlx5 PCI VF 能够执行 IPsec 加密卸载的用户需要显式启用 VF 的 ipsec_crypto 能力。从 ConnectX6dx 及更高版本的设备开始支持为 VF 启用 IPsec 能力。当某个 VF 启用了 IPsec 能力时，PF 上的任何 IPsec 卸载都会被阻止。)

mlx5 driver support devlink port function attr mechanism to setup ipsec_crypto
capability. (refer to Documentation/networking/devlink/devlink-port.rst)
(mlx5 驱动支持 devlink 端口函数属性机制来设置 ipsec_crypto 能力。（参见 Documentation/networking/devlink/devlink-port.rst）)

IPsec packet capability setup(IPsec 包 (packet) 能力设置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

User who wants mlx5 PCI VFs to be able to perform IPsec packet offloading need
to explicitly enable the VF ipsec_packet capability. Enabling IPsec capability
for VFs is supported starting with ConnectX6dx devices and above. When a VF has
IPsec capability enabled, any IPsec offloading is blocked on the PF.
(希望 mlx5 PCI VF 能够执行 IPsec 包卸载的用户需要显式启用 VF 的 ipsec_packet 能力。从 ConnectX6dx 及更高版本的设备开始支持为 VF 启用 IPsec 能力。当某个 VF 启用了 IPsec 能力时，PF 上的任何 IPsec 卸载都会被阻止。)

mlx5 driver support devlink port function attr mechanism to setup ipsec_packet
capability. (refer to Documentation/networking/devlink/devlink-port.rst)
(mlx5 驱动支持 devlink 端口函数属性机制来设置 ipsec_packet 能力。（参见 Documentation/networking/devlink/devlink-port.rst）)

SF state setup(SF 状态设置)
---------------------------

To use the SF, the user must activate the SF using the SF function state
attribute.
(要使用 SF，用户必须使用 SF 函数状态属性来激活该 SF。)

- Get the state of the SF identified by its unique devlink port index::
  (获取由其唯一 devlink 端口索引所标识的 SF 的状态：)

   $ devlink port show ens2f0npf0sf88
   pci/0000:06:00.0/32768: type eth netdev ens2f0npf0sf88 flavour pcisf controller 0 pfnum 0 sfnum 88 external false splittable false
     function:
       hw_addr 00:00:00:00:88:88 state inactive opstate detached

- Activate the function and verify its state is active::
  (激活该函数并验证其状态为 active：)

   $ devlink port function set ens2f0npf0sf88 state active

   $ devlink port show ens2f0npf0sf88
   pci/0000:06:00.0/32768: type eth netdev ens2f0npf0sf88 flavour pcisf controller 0 pfnum 0 sfnum 88 external false splittable false
     function:
       hw_addr 00:00:00:00:88:88 state active opstate detached

Upon function activation, the PF driver instance gets the event from the device
that a particular SF was activated. It's the cue to put the device on bus, probe
it and instantiate the devlink instance and class specific auxiliary devices
for it.
(函数激活后，PF 驱动实例会从设备收到某个特定 SF 已被激活的事件。这是把设备放到总线上、对其进行探测并为其实例化 devlink 实例和特定类别的辅助设备的提示。)

- Show the auxiliary device and port of the subfunction::
  (显示子功能的辅助设备和端口：)

    $ devlink dev show
    devlink dev show auxiliary/mlx5_core.sf.4

    $ devlink port show auxiliary/mlx5_core.sf.4/1
    auxiliary/mlx5_core.sf.4/1: type eth netdev p0sf88 flavour virtual port 0 splittable false

    $ rdma link show mlx5_0/1
    link mlx5_0/1 state ACTIVE physical_state LINK_UP netdev p0sf88

    $ rdma dev show
    8: rocep6s0f1: node_type ca fw 16.29.0550 node_guid 248a:0703:00b3:d113 sys_image_guid 248a:0703:00b3:d112
    13: mlx5_0: node_type ca fw 16.29.0550 node_guid 0000:00ff:fe00:8888 sys_image_guid 248a:0703:00b3:d112

- Subfunction auxiliary device and class device hierarchy::
  (子功能辅助设备和类设备层次结构：)

                 mlx5_core.sf.4
          (subfunction auxiliary device)
                       /\
                      /  \
                     /    \
                    /      \
                   /        \
      mlx5_core.eth.4     mlx5_core.rdma.4
     (sf eth aux dev)     (sf rdma aux dev)
         |                      |
         |                      |
      p0sf88                  mlx5_0
     (sf netdev)          (sf rdma device)

Additionally, the SF port also gets the event when the driver attaches to the
auxiliary device of the subfunction. This results in changing the operational
state of the function. This provides visibility to the user to decide when is it
safe to delete the SF port for graceful termination of the subfunction.
(此外，当驱动挂接到子功能的辅助设备时，SF 端口也会收到事件。这会导致函数的操作状态发生变化。这为用户提供了可见性，使其能够判断何时删除 SF 端口是安全的，从而优雅地终止子功能。)

- Show the SF port operational state::
  (显示 SF 端口的操作状态：)

    $ devlink port show ens2f0npf0sf88
    pci/0000:06:00.0/32768: type eth netdev ens2f0npf0sf88 flavour pcisf controller 0 pfnum 0 sfnum 88 external false splittable false
      function:
        hw_addr 00:00:00:00:88:88 state active opstate attached
