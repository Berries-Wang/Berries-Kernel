.. SPDX-License-Identifier: GPL-2.0

============
NET_FAILOVER
============

(NET_FAILOVER)

Overview
(概述)
========

The net_failover driver provides an automated failover mechanism via APIs
to create and destroy a failover master netdev and manages a primary and
standby slave netdevs that get registered via the generic failover
infrastructure.
(net_failover 驱动通过 API 提供自动故障切换 (failover) 机制，用于创建和销毁 failover 主 netdev，并管理通过通用 failover 基础设施注册的主 (primary) 与备用 (standby) slave netdev。)

The failover netdev acts a master device and controls 2 slave devices. The
original paravirtual interface is registered as 'standby' slave netdev and
a passthru/vf device with the same MAC gets registered as 'primary' slave
netdev. Both 'standby' and 'failover' netdevs are associated with the same
'pci' device. The user accesses the network interface via 'failover' netdev.
The 'failover' netdev chooses 'primary' netdev as default for transmits when
it is available with link up and running.
(failover netdev 充当主设备并控制 2 个 slave 设备。原始的半虚拟化接口注册为 'standby' slave netdev，而具有相同 MAC 的直通 (passthru)/VF 设备注册为 'primary' slave netdev。'standby' 和 'failover' 这两个 netdev 关联到同一个 'pci' 设备。用户通过 'failover' netdev 访问网络接口。当 'primary' netdev 可用且链路 up 并处于运行状态时，'failover' netdev 默认选择它进行发送。)

This can be used by paravirtual drivers to enable an alternate low latency
datapath. It also enables hypervisor controlled live migration of a VM with
direct attached VF by failing over to the paravirtual datapath when the VF
is unplugged.
(半虚拟化驱动可以用它来启用另一条低延迟数据路径。当 VF 被拔出时，通过故障切换到半虚拟化数据路径，它还支持由 hypervisor 控制的、带直连 VF 的虚拟机的实时迁移。)

virtio-net accelerated datapath: STANDBY mode
(virtio-net 加速数据路径：STANDBY 模式)
=============================================

net_failover enables hypervisor controlled accelerated datapath to virtio-net
enabled VMs in a transparent manner with no/minimal guest userspace changes.
(net_failover 以透明方式为启用 virtio-net 的虚拟机提供由 hypervisor 控制的加速数据路径，客户机用户态无需修改或只需极少量修改。)

To support this, the hypervisor needs to enable VIRTIO_NET_F_STANDBY
feature on the virtio-net interface and assign the same MAC address to both
virtio-net and VF interfaces.
(为支持这一点，hypervisor 需要在 virtio-net 接口上启用 VIRTIO_NET_F_STANDBY 特性，并为 virtio-net 和 VF 接口分配相同的 MAC 地址。)

Here is an example libvirt XML snippet that shows such configuration:
(下面是一个展示此类配置的 libvirt XML 片段示例：)
::

  <interface type='network'>
    <mac address='52:54:00:00:12:53'/>
    <source network='enp66s0f0_br'/>
    <target dev='tap01'/>
    <model type='virtio'/>
    <driver name='vhost' queues='4'/>
    <link state='down'/>
    <teaming type='persistent'/>
    <alias name='ua-backup0'/>
  </interface>
  <interface type='hostdev' managed='yes'>
    <mac address='52:54:00:00:12:53'/>
    <source>
      <address type='pci' domain='0x0000' bus='0x42' slot='0x02' function='0x5'/>
    </source>
    <teaming type='transient' persistent='ua-backup0'/>
  </interface>

In this configuration, the first device definition is for the virtio-net
interface and this acts as the 'persistent' device indicating that this
interface will always be plugged in. This is specified by the 'teaming' tag with
required attribute type having value 'persistent'. The link state for the
virtio-net device is set to 'down' to ensure that the 'failover' netdev prefers
the VF passthrough device for normal communication. The virtio-net device will
be brought UP during live migration to allow uninterrupted communication.
(在此配置中，第一个设备定义对应 virtio-net 接口，它充当 'persistent' 设备，表示该接口将始终处于插入状态。这是通过 'teaming' 标签指定的，其必需属性 type 的值为 'persistent'。virtio-net 设备的链路状态被设为 'down'，以确保 'failover' netdev 在正常通信时优先使用 VF 直通设备。在实时迁移期间，virtio-net 设备会被置为 UP，以保证通信不中断。)

The second device definition is for the VF passthrough interface. Here the
'teaming' tag is provided with type 'transient' indicating that this device may
periodically be unplugged. A second attribute - 'persistent' is provided and
points to the alias name declared for the virtio-net device.
(第二个设备定义用于 VF 直通接口。这里 'teaming' 标签的类型为 'transient'，表示该设备可能被周期性地拔出。还提供了第二个属性 - 'persistent'，指向为 virtio-net 设备声明的别名。)

Booting a VM with the above configuration will result in the following 3
interfaces created in the VM:
(使用上述配置启动虚拟机后，虚拟机中将创建以下 3 个接口：)
::

  4: ens10: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc noqueue state UP group default qlen 1000
      link/ether 52:54:00:00:12:53 brd ff:ff:ff:ff:ff:ff
      inet 192.168.12.53/24 brd 192.168.12.255 scope global dynamic ens10
         valid_lft 42482sec preferred_lft 42482sec
      inet6 fe80::97d8:db2:8c10:b6d6/64 scope link
         valid_lft forever preferred_lft forever
  5: ens10nsby: <BROADCAST,MULTICAST> mtu 1500 qdisc fq_codel master ens10 state DOWN group default qlen 1000
      link/ether 52:54:00:00:12:53 brd ff:ff:ff:ff:ff:ff
  7: ens11: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc mq master ens10 state UP group default qlen 1000
      link/ether 52:54:00:00:12:53 brd ff:ff:ff:ff:ff:ff

Here, ens10 is the 'failover' master interface, ens10nsby is the slave 'standby'
virtio-net interface, and ens11 is the slave 'primary' VF passthrough interface.
(这里，ens10 是 'failover' 主接口，ens10nsby 是 slave 'standby' virtio-net 接口，ens11 是 slave 'primary' VF 直通接口。)

One point to note here is that some user space network configuration daemons
like systemd-networkd, ifupdown, etc, do not understand the 'net_failover'
device; and on the first boot, the VM might end up with both 'failover' device
and VF acquiring IP addresses (either same or different) from the DHCP server.
This will result in lack of connectivity to the VM. So some tweaks might be
needed to these network configuration daemons to make sure that an IP is
received only on the 'failover' device.
(这里需要注意的一点是，一些用户态网络配置守护进程（如 systemd-networkd、ifupdown 等）并不理解 'net_failover' 设备；在首次启动时，虚拟机可能会出现 'failover' 设备和 VF 都从 DHCP 服务器获取 IP 地址（相同或不同）的情况。这会导致虚拟机失去连接。因此可能需要对这些网络配置守护进程做一些调整，确保只在 'failover' 设备上获取 IP。)

Below is the patch snippet used with 'cloud-ifupdown-helper' script found on
Debian cloud images::
(下面是与 Debian 云镜像中 'cloud-ifupdown-helper' 脚本配合使用的补丁片段：)

  @@ -27,6 +27,8 @@ do_setup() {
       local working="$cfgdir/.$INTERFACE"
       local final="$cfgdir/$INTERFACE"

  +    if [ -d "/sys/class/net/${INTERFACE}/master" ]; then exit 0; fi
  +
       if ifup --no-act "$INTERFACE" > /dev/null 2>&1; then
           # interface is already known to ifupdown, no need to generate cfg
           log "Skipping configuration generation for $INTERFACE"


Live Migration of a VM with SR-IOV VF & virtio-net in STANDBY mode
(STANDBY 模式下带 SR-IOV VF 与 virtio-net 的虚拟机实时迁移)
==================================================================

net_failover also enables hypervisor controlled live migration to be supported
with VMs that have direct attached SR-IOV VF devices by automatic failover to
the paravirtual datapath when the VF is unplugged.
(当 VF 被拔出时自动切换到半虚拟化数据路径，net_failover 还能让带有直连 SR-IOV VF 设备的虚拟机支持由 hypervisor 控制的实时迁移。)

Here is a sample script that shows the steps to initiate live migration from
the source hypervisor. Note: It is assumed that the VM is connected to a
software bridge 'br0' which has a single VF attached to it along with the vnet
device to the VM. This is not the VF that was passthrough'd to the VM (seen in
the vf.xml file).
(下面是一个示例脚本，展示从源 hypervisor 发起实时迁移的步骤。注意：这里假设虚拟机连接到软件网桥 'br0'，该网桥上挂着一个 VF 以及通往虚拟机的 vnet 设备。这不是直通给虚拟机的那个 VF（见 vf.xml 文件）。)
::

  # cat vf.xml
  <interface type='hostdev' managed='yes'>
    <mac address='52:54:00:00:12:53'/>
    <source>
      <address type='pci' domain='0x0000' bus='0x42' slot='0x02' function='0x5'/>
    </source>
    <teaming type='transient' persistent='ua-backup0'/>
  </interface>

  # Source Hypervisor migrate.sh
  #!/bin/bash

  DOMAIN=vm-01
  PF=ens6np0
  VF=ens6v1             # VF attached to the bridge.
  VF_NUM=1
  TAP_IF=vmtap01        # virtio-net interface in the VM.
  VF_XML=vf.xml

  MAC=52:54:00:00:12:53
  ZERO_MAC=00:00:00:00:00:00

  # Set the virtio-net interface up.
  virsh domif-setlink $DOMAIN $TAP_IF up

  # Remove the VF that was passthrough'd to the VM.
  virsh detach-device --live --config $DOMAIN $VF_XML

  ip link set $PF vf $VF_NUM mac $ZERO_MAC

  # Add FDB entry for traffic to continue going to the VM via
  # the VF -> br0 -> vnet interface path.
  bridge fdb add $MAC dev $VF
  bridge fdb add $MAC dev $TAP_IF master

  # Migrate the VM
  virsh migrate --live --persistent $DOMAIN qemu+ssh://$REMOTE_HOST/system

  # Clean up FDB entries after migration completes.
  bridge fdb del $MAC dev $VF
  bridge fdb del $MAC dev $TAP_IF master

On the destination hypervisor, a shared bridge 'br0' is created before migration
starts, and a VF from the destination PF is added to the bridge. Similarly an
appropriate FDB entry is added.
(在目的 hypervisor 上，会在迁移开始之前创建共享网桥 'br0'，并把来自目的 PF 的一个 VF 加入该网桥。同样，也会添加相应的 FDB 表项。)

The following script is executed on the destination hypervisor once migration
completes, and it reattaches the VF to the VM and brings down the virtio-net
interface::
(迁移完成后，以下脚本会在目的 hypervisor 上执行，它把 VF 重新挂接回虚拟机，并将 virtio-net 接口置为 down：)

  # reattach-vf.sh
  #!/bin/bash

  bridge fdb del 52:54:00:00:12:53 dev ens36v0
  bridge fdb del 52:54:00:00:12:53 dev vmtap01 master
  virsh attach-device --config --live vm01 vf.xml
  virsh domif-setlink vm01 vmtap01 down
