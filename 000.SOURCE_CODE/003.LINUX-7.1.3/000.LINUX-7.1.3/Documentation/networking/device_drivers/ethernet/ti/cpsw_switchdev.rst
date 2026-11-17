.. SPDX-License-Identifier: GPL-2.0

======================================================
Texas Instruments CPSW switchdev based ethernet driver
======================================================

(德州仪器基于 CPSW switchdev 的以太网驱动)

:Version: 2.0
(版本：2.0)

Port renaming
(端口重命名)
=============

On older udev versions renaming of ethX to swXpY will not be automatically
supported
(在较旧版本的 udev 上，不会自动支持把 ethX 重命名为 swXpY。)

In order to rename via udev::
(要通过 udev 重命名：)

    ip -d link show dev sw0p1 | grep switchid

    SUBSYSTEM=="net", ACTION=="add", ATTR{phys_switch_id}==<switchid>, \
	    ATTR{phys_port_name}!="", NAME="sw0$attr{phys_port_name}"


Dual mac mode
(双 MAC 模式)
=============

- The new (cpsw_new.c) driver is operating in dual-emac mode by default, thus
  working as 2 individual network interfaces. Main differences from legacy CPSW
  driver are:
  (新驱动 (cpsw_new.c) 默认以 dual-emac 模式运行，即作为 2 个独立的网络接口
  工作。与旧式 CPSW 驱动的主要区别如下：)

 - optimized promiscuous mode: The P0_UNI_FLOOD (both ports) is enabled in
   addition to ALLMULTI (current port) instead of ALE_BYPASS.
   So, Ports in promiscuous mode will keep possibility of mcast and vlan
   filtering, which is provides significant benefits when ports are joined
   to the same bridge, but without enabling "switch" mode, or to different
   bridges.
   (优化后的混杂模式：除 ALLMULTI（当前端口）之外还启用 P0_UNI_FLOOD（两个
   端口），而不是使用 ALE_BYPASS。这样，处于混杂模式的端口仍保留组播和 VLAN
   过滤能力，这在端口加入同一网桥但未启用 "switch" 模式时，或加入不同网桥时
   都能带来显著好处。)
 - learning disabled on ports as it make not too much sense for
   segregated ports - no forwarding in HW.
   (端口上的学习功能被禁用，因为这对被隔离的端口而言意义不大——硬件中不进行
   转发。)
 - enabled basic support for devlink.
   (启用了对 devlink 的基本支持。)

   ::

	devlink dev show
		platform/48484000.switch

	devlink dev param show
	platform/48484000.switch:
	name switch_mode type driver-specific
	values:
		cmode runtime value false
	name ale_bypass type driver-specific
	values:
		cmode runtime value false

Devlink configuration parameters
(devlink 配置参数)
================================

See Documentation/networking/devlink/ti-cpsw-switch.rst
(参见 Documentation/networking/devlink/ti-cpsw-switch.rst。)

Bridging in dual mac mode
(双 MAC 模式下的桥接)
=========================

The dual_mac mode requires two vids to be reserved for internal purposes,
which, by default, equal CPSW Port numbers. As result, bridge has to be
configured in vlan unaware mode or default_pvid has to be adjusted::
(双 mac 模式需要为内部用途保留两个 vid，默认情况下它们等于 CPSW 端口号。因此，网桥必须配置为 vlan unaware 模式，或者必须调整 default_pvid：)

	ip link add name br0 type bridge
	ip link set dev br0 type bridge vlan_filtering 0
	echo 0 > /sys/class/net/br0/bridge/default_pvid
	ip link set dev sw0p1 master br0
	ip link set dev sw0p2 master br0

or::
(或者：)

	ip link add name br0 type bridge
	ip link set dev br0 type bridge vlan_filtering 0
	echo 100 > /sys/class/net/br0/bridge/default_pvid
	ip link set dev br0 type bridge vlan_filtering 1
	ip link set dev sw0p1 master br0
	ip link set dev sw0p2 master br0

Enabling "switch"
(启用 "switch" 模式)
=================

The Switch mode can be enabled by configuring devlink driver parameter
"switch_mode" to 1/true::
(可以通过把 devlink 驱动参数 "switch_mode" 配置为 1/true 来启用 Switch 模式：)

	devlink dev param set platform/48484000.switch \
	name switch_mode value 1 cmode runtime

This can be done regardless of the state of Port's netdev devices - UP/DOWN, but
Port's netdev devices have to be in UP before joining to the bridge to avoid
overwriting of bridge configuration as CPSW switch driver copletly reloads its
configuration when first Port changes its state to UP.
(无论端口的 netdev 设备处于 UP/DOWN 哪种状态都可以这样做，但在加入网桥之前，端口的 netdev 设备必须处于 UP 状态，以免覆盖网桥配置，因为当第一个端口变为 UP 时，CPSW 交换机驱动会完全重新加载其配置。)

When both interfaces have joined the bridge - CPSW switch driver will enable
marking packets with offload_fwd_mark flag unless "ale_bypass=0"
(当两个接口都已加入网桥后——除非 "ale_bypass=0"，CPSW 交换机驱动将启用用 offload_fwd_mark 标志标记数据包的功能。)

All configuration is implemented via switchdev API.
(所有配置均通过 switchdev API 实现。)

Bridge setup
(网桥设置)
============

::

	devlink dev param set platform/48484000.switch \
	name switch_mode value 1 cmode runtime

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
(打开/关闭 STP)
==========

::

	ip link set dev BRDEV type bridge stp_state 1/0

VLAN configuration
(VLAN 配置)
==================

::

  bridge vlan add dev br0 vid 1 pvid untagged self <---- add cpu port to VLAN 1

Note. This step is mandatory for bridge/default_pvid.
(注意。对于网桥的 default_pvid 而言，此步骤是必需的。)

Add extra VLANs
(添加额外的 VLAN)
===============

 1. untagged::
 (1. 未打标签（untagged）：)

	bridge vlan add dev sw0p1 vid 100 pvid untagged master
	bridge vlan add dev sw0p2 vid 100 pvid untagged master
	bridge vlan add dev br0 vid 100 pvid untagged self <---- Add cpu port to VLAN100

 2. tagged::
 (2. 打标签（tagged）：)

	bridge vlan add dev sw0p1 vid 100 master
	bridge vlan add dev sw0p2 vid 100 master
	bridge vlan add dev br0 vid 100 pvid tagged self <---- Add cpu port to VLAN100

FDBs
(FDB（转发数据库条目）)
----

FDBs are automatically added on the appropriate switch port upon detection
(FDB 在检测到时自动添加到相应的交换机端口上。)

Manually adding FDBs::
(手动添加 FDB：)

    bridge fdb add aa:bb:cc:dd:ee:ff dev sw0p1 master vlan 100
    bridge fdb add aa:bb:cc:dd:ee:fe dev sw0p2 master <---- Add on all VLANs

MDBs
(MDB（组播数据库条目）)
----

MDBs are automatically added on the appropriate switch port upon detection
(MDB 在检测到时自动添加到相应的交换机端口上。)

Manually adding MDBs::
(手动添加 MDB：)

  bridge mdb add dev br0 port sw0p1 grp 239.1.1.1 permanent vid 100
  bridge mdb add dev br0 port sw0p1 grp 239.1.1.1 permanent <---- Add on all VLANs

Multicast flooding
(组播泛洪)
==================

CPU port mcast_flooding is always on
(CPU 端口的 mcast_flooding 始终为开启。)

Turning flooding on/off on switch ports:
(在交换机端口上打开/关闭泛洪：)
bridge link set dev sw0p1 mcast_flood on/off

Access and Trunk port
(Access 和 Trunk 端口)
=====================

::

 bridge vlan add dev sw0p1 vid 100 pvid untagged master
 bridge vlan add dev sw0p2 vid 100 master


 bridge vlan add dev br0 vid 100 self
 ip link add link br0 name br0.100 type vlan id 100

Note. Setting PVID on Bridge device itself working only for
default VLAN (default_pvid).
(注意。在网桥设备自身上设置 PVID 只对默认 VLAN (default_pvid) 有效。)

NFS
(NFS)
===

The only way for NFS to work is by chrooting to a minimal environment when
switch configuration that will affect connectivity is needed.
Assuming you are booting NFS with eth1 interface(the script is hacky and
it's just there to prove NFS is doable).
(当需要执行会影响连通性的交换机配置时，让 NFS 正常工作的唯一办法是 chroot 到一个最小环境中。假设你使用 eth1 接口通过 NFS 启动（该脚本有些粗糙，只是用来证明 NFS 是可行的）。)

setup.sh::
(setup.sh：)

	#!/bin/sh
	mkdir proc
	mount -t proc none /proc
	ifconfig br0  > /dev/null
	if [ $? -ne 0 ]; then
		echo "Setting up bridge"
		ip link add name br0 type bridge
		ip link set dev br0 type bridge ageing_time 1000
		ip link set dev br0 type bridge vlan_filtering 1

		ip link set eth1 down
		ip link set eth1 name sw0p1
		ip link set dev sw0p1 up
		ip link set dev sw0p2 up
		ip link set dev sw0p2 master br0
		ip link set dev sw0p1 master br0
		bridge vlan add dev br0 vid 1 pvid untagged self
		ifconfig sw0p1 0.0.0.0
		udhchc -i br0
	fi
	umount /proc

run_nfs.sh:::
(run_nfs.sh：)

	#!/bin/sh
	mkdir /tmp/root/bin -p
	mkdir /tmp/root/lib -p

	cp -r /lib/ /tmp/root/
	cp -r /bin/ /tmp/root/
	cp /sbin/ip /tmp/root/bin
	cp /sbin/bridge /tmp/root/bin
	cp /sbin/ifconfig /tmp/root/bin
	cp /sbin/udhcpc /tmp/root/bin
	cp /path/to/setup.sh /tmp/root/bin
	chroot /tmp/root/ busybox sh /bin/setup.sh

	run ./run_nfs.sh
