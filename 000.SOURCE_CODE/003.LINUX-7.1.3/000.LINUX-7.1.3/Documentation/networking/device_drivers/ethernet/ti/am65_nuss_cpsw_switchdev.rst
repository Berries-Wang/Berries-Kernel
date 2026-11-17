.. SPDX-License-Identifier: GPL-2.0

===================================================================
Texas Instruments K3 AM65 CPSW NUSS switchdev based ethernet driver
===================================================================

(德州仪器 K3 AM65 CPSW NUSS 基于 switchdev 的以太网驱动程序)

:Version: 1.0

(版本: 1.0)

Port renaming
(端口重命名)
=============

In order to rename via udev::
(要通过 udev 重命名，可按如下方式操作：)

    ip -d link show dev sw0p1 | grep switchid

    SUBSYSTEM=="net", ACTION=="add", ATTR{phys_switch_id}==<switchid>, \
	    ATTR{phys_port_name}!="", NAME="sw0$attr{phys_port_name}"


Multi mac mode
(多 MAC 模式)
==============

- The driver is operating in multi-mac mode by default, thus
  working as N individual network interfaces.
  (驱动程序默认以 multi-mac 模式运行，即作为 N 个独立的网络接口工作。)

Devlink configuration parameters
(Devlink 配置参数)
================================

See Documentation/networking/devlink/am65-nuss-cpsw-switch.rst
(参见 Documentation/networking/devlink/am65-nuss-cpsw-switch.rst)

Enabling "switch"
(启用 "switch")
=================

The Switch mode can be enabled by configuring devlink driver parameter
"switch_mode" to 1/true::
(可以通过把 devlink 驱动程序参数 "switch_mode" 配置为 1/true 来启用 Switch
模式：)

        devlink dev param set platform/c000000.ethernet \
        name switch_mode value true cmode runtime

This can be done regardless of the state of Port's netdev devices - UP/DOWN, but
Port's netdev devices have to be in UP before joining to the bridge to avoid
overwriting of bridge configuration as CPSW switch driver completely reloads its
configuration when first port changes its state to UP.
(无论端口的 netdev 设备处于 UP/DOWN 什么状态都可以这样做，但在加入网桥之前，端口的
netdev 设备必须处于 UP 状态，以避免覆盖网桥配置，因为当第一个端口将其状态变为 UP
时，CPSW switch 驱动程序会完全重新加载其配置。)

When both interfaces have joined the bridge - CPSW switch driver will enable
marking packets with offload_fwd_mark flag.
(当两个接口都加入网桥后——CPSW switch 驱动程序将启用用 offload_fwd_mark 标志
标记数据包的功能。)

All configuration is implemented via switchdev API.
(所有配置都通过 switchdev API 实现。)

Bridge setup
(网桥设置)
============

::

        devlink dev param set platform/c000000.ethernet \
        name switch_mode value true cmode runtime

	ip link add name br0 type bridge
	ip link set dev br0 type bridge ageing_time 1000
	ip link set dev sw0p1 up
	ip link set dev sw0p2 up
	ip link set dev sw0p1 master br0
	ip link set dev sw0p2 master br0

	[*] bridge vlan add dev br0 vid 1 pvid untagged self

	[*] if vlan_filtering=1. where default_pvid=1

	Note. Steps [*] are mandatory.


On/off STP
(开/关 STP)
==========

::

	ip link set dev BRDEV type bridge stp_state 1/0

VLAN configuration
(VLAN 配置)
==================

::

  bridge vlan add dev br0 vid 1 pvid untagged self <---- add cpu port to VLAN 1

Note. This step is mandatory for bridge/default_pvid.
(注意。对于 bridge/default_pvid，此步骤是强制性的。)

Add extra VLANs
(添加额外 VLAN)
===============

 1. untagged::
    (未打标签：)

	bridge vlan add dev sw0p1 vid 100 pvid untagged master
	bridge vlan add dev sw0p2 vid 100 pvid untagged master
	bridge vlan add dev br0 vid 100 pvid untagged self <---- Add cpu port to VLAN100

 2. tagged::
    (已打标签：)

	bridge vlan add dev sw0p1 vid 100 master
	bridge vlan add dev sw0p2 vid 100 master
	bridge vlan add dev br0 vid 100 pvid tagged self <---- Add cpu port to VLAN100

FDBs
(FDB 条目)
----

FDBs are automatically added on the appropriate switch port upon detection
(检测到后会自动在相应的交换机端口上添加 FDB)

Manually adding FDBs::
(手动添加 FDB：)

    bridge fdb add aa:bb:cc:dd:ee:ff dev sw0p1 master vlan 100
    bridge fdb add aa:bb:cc:dd:ee:fe dev sw0p2 master <---- Add on all VLANs

MDBs
(MDB 条目)
----

MDBs are automatically added on the appropriate switch port upon detection
(检测到后会自动在相应的交换机端口上添加 MDB)

Manually adding MDBs::
(手动添加 MDB：)

  bridge mdb add dev br0 port sw0p1 grp 239.1.1.1 permanent vid 100
  bridge mdb add dev br0 port sw0p1 grp 239.1.1.1 permanent <---- Add on all VLANs

Multicast flooding
(组播泛洪)
==================

CPU port mcast_flooding is always on
(CPU 端口的 mcast_flooding 始终开启)

Turning flooding on/off on switch ports:
bridge link set dev sw0p1 mcast_flood on/off
(在交换机端口上开启/关闭泛洪：
bridge link set dev sw0p1 mcast_flood on/off)

Access and Trunk port
(Access 与 Trunk 端口)
=====================

::

 bridge vlan add dev sw0p1 vid 100 pvid untagged master
 bridge vlan add dev sw0p2 vid 100 master


 bridge vlan add dev br0 vid 100 self
 ip link add link br0 name br0.100 type vlan id 100

Note. Setting PVID on Bridge device itself works only for
default VLAN (default_pvid).
(注意。在网桥设备本身上设置 PVID 仅对默认 VLAN(default_pvid)有效。)
