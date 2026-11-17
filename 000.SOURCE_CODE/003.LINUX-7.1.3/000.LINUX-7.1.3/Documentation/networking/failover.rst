.. SPDX-License-Identifier: GPL-2.0

===========================
FAILOVER(FAILOVER 故障切换)
===========================

Overview(概述)
==============

The failover module provides a generic interface for paravirtual drivers
to register a netdev and a set of ops with a failover instance. The ops
are used as event handlers that get called to handle netdev register/
unregister/link change/name change events on slave pci ethernet devices
with the same mac address as the failover netdev.
(failover 模块为半虚拟化驱动提供了一个通用接口，用于向 failover 实例注册
一个 netdev 及一组操作（ops）。这些操作作为事件处理程序被调用，以处理与
failover netdev 具有相同 MAC 地址的从属 PCI 以太网设备上的 netdev 注册/
注销/链路变更/名称变更事件。)

This enables paravirtual drivers to use a VF as an accelerated low latency
datapath. It also allows live migration of VMs with direct attached VFs by
failing over to the paravirtual datapath when the VF is unplugged.
(这使半虚拟化驱动能够将 VF 用作加速的低时延数据路径。当 VF 被拔出时，通过
故障切换到半虚拟化数据路径，它还支持直接挂载 VF 的虚拟机进行实时迁移。)
