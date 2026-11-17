.. SPDX-License-Identifier: GPL-2.0+

==================================================================================================================================================
idpf Linux* Base Driver for the Intel(R) Infrastructure Data Path Function(Intel(R) Infrastructure Data Path Function 的 idpf Linux* 基础驱动程序)
==================================================================================================================================================

Intel idpf Linux driver.
Copyright(C) 2023 Intel Corporation.
(Intel idpf Linux 驱动程序。
版权所有(C) 2023 Intel Corporation。)

.. contents::

The idpf driver serves as both the Physical Function (PF) and Virtual Function
(VF) driver for the Intel(R) Infrastructure Data Path Function.
(idpf 驱动程序既充当 Intel(R) Infrastructure Data Path Function 的物理功能(PF)
驱动程序，也充当其虚拟功能(VF)驱动程序。)

Driver information can be obtained using ethtool, lspci, and ip.
(可以使用 ethtool、lspci 和 ip 获取驱动程序信息。)

For questions related to hardware requirements, refer to the documentation
supplied with your Intel adapter. All hardware requirements listed apply to use
with Linux.
(有关硬件要求相关的问题，请参阅随 Intel 适配器提供的文档。所列出的所有硬件要求均适用
于 Linux 环境。)


Identifying Your Adapter(识别你的适配器)
========================================

For information on how to identify your adapter, and for the latest Intel
network drivers, refer to the Intel Support website:
http://www.intel.com/support
(有关如何识别你的适配器以及获取最新 Intel 网络驱动程序的信息，请参阅 Intel 支持网站：
http://www.intel.com/support)


Additional Features and Configurations(附加功能与配置)
======================================================

ethtool(ethtool)
----------------

The driver utilizes the ethtool interface for driver configuration and
diagnostics, as well as displaying statistical information. The latest ethtool
version is required for this functionality. If you don't have one yet, you can
obtain it at:
https://kernel.org/pub/software/network/ethtool/
(该驱动程序利用 ethtool 接口进行驱动程序配置和诊断，以及显示统计信息。此功能需要最新
版本的 ethtool。如果你还没有，可以从以下地址获取：
https://kernel.org/pub/software/network/ethtool/)


Viewing Link Messages(查看链路消息)
-----------------------------------

Link messages will not be displayed to the console if the distribution is
restricting system messages. In order to see network driver link messages on
your console, set dmesg to eight by entering the following::
(如果发行版限制了系统消息，链路消息将不会显示到控制台上。要在控制台上看到网络驱动
程序的链路消息，请输入以下命令将 dmesg 设置为 8：)

  # dmesg -n 8

.. note::
   This setting is not saved across reboots.
   (该设置在重启后不会保存。)


Jumbo Frames(巨型帧)
--------------------

Jumbo Frames support is enabled by changing the Maximum Transmission Unit (MTU)
to a value larger than the default value of 1500.
(通过将最大传输单元(MTU)改为大于默认值 1500 的数值来启用巨型帧支持。)

Use the ip command to increase the MTU size. For example, enter the following
where <ethX> is the interface number::
(使用 ip 命令增大 MTU 大小。例如，在 <ethX> 为接口编号的地方输入以下命令：)

  # ip link set mtu 9000 dev <ethX>
  # ip link set up dev <ethX>

.. note::
   The maximum MTU setting for jumbo frames is 9706. This corresponds to the
   maximum jumbo frame size of 9728 bytes.
   (巨型帧的最大 MTU 设置为 9706。这对应于 9728 字节的最大巨型帧大小。)

.. note::
   This driver will attempt to use multiple page sized buffers to receive
   each jumbo packet. This should help to avoid buffer starvation issues when
   allocating receive packets.
   (该驱动程序会尝试使用多个页大小的缓冲区来接收每个巨型数据包。这应有助于在分配
   接收数据包时避免缓冲区不足的问题。)

.. note::
   Packet loss may have a greater impact on throughput when you use jumbo
   frames. If you observe a drop in performance after enabling jumbo frames,
   enabling flow control may mitigate the issue.
   (使用巨型帧时，丢包可能对吞吐量造成更大影响。如果启用巨型帧后观察到性能下降，
   启用流控制可能会缓解该问题。)


Performance Optimization(性能优化)
==================================

Driver defaults are meant to fit a wide variety of workloads, but if further
optimization is required, we recommend experimenting with the following
settings.
(驱动程序默认值旨在适应各种工作负载，但如果需要进一步优化，我们建议对以下设置进行
试验。)


Interrupt Rate Limiting(中断速率限制)
-------------------------------------

This driver supports an adaptive interrupt throttle rate (ITR) mechanism that
is tuned for general workloads. The user can customize the interrupt rate
control for specific workloads, via ethtool, adjusting the number of
microseconds between interrupts.
(该驱动程序支持针对一般工作负载进行调优的自适应中断节流率(ITR)机制。用户可以通过
ethtool 针对特定工作负载自定义中断速率控制，调整中断之间的微秒数。)

To set the interrupt rate manually, you must disable adaptive mode::
(要手动设置中断速率，必须禁用自适应模式：)

  # ethtool -C <ethX> adaptive-rx off adaptive-tx off

For lower CPU utilization:
(为了降低 CPU 占用率：)

 - Disable adaptive ITR and lower Rx and Tx interrupts. The examples below
   affect every queue of the specified interface.
   (禁用自适应 ITR 并降低 Rx 和 Tx 中断。以下示例会影响指定接口的每个队列。)

 - Setting rx-usecs and tx-usecs to 80 will limit interrupts to about
   12,500 interrupts per second per queue::
   (将 rx-usecs 和 tx-usecs 设置为 80 会把中断限制为每队列每秒约 12,500 次中断：)

     # ethtool -C <ethX> adaptive-rx off adaptive-tx off rx-usecs 80
     tx-usecs 80

For reduced latency:
(为了降低延迟：)

 - Disable adaptive ITR and ITR by setting rx-usecs and tx-usecs to 0
   using ethtool::
   (禁用自适应 ITR 和 ITR，并通过 ethtool 将 rx-usecs 和 tx-usecs 设置为 0：)

     # ethtool -C <ethX> adaptive-rx off adaptive-tx off rx-usecs 0
     tx-usecs 0

Per-queue interrupt rate settings:
(每队列中断速率设置：)

 - The following examples are for queues 1 and 3, but you can adjust other
   queues.
   (以下示例针对队列 1 和 3，但你可以调整其他队列。)

 - To disable Rx adaptive ITR and set static Rx ITR to 10 microseconds or
   about 100,000 interrupts/second, for queues 1 and 3::
   (要禁用 Rx 自适应 ITR 并将队列 1 和 3 的静态 Rx ITR 设置为 10 微秒(即约
   100,000 次中断/秒)：)

     # ethtool --per-queue <ethX> queue_mask 0xa --coalesce adaptive-rx off
     rx-usecs 10

 - To show the current coalesce settings for queues 1 and 3::
   (要显示队列 1 和 3 的当前中断合并(coalesce)设置：)

     # ethtool --per-queue <ethX> queue_mask 0xa --show-coalesce



Virtualized Environments(虚拟化环境)
------------------------------------

In addition to the other suggestions in this section, the following may be
helpful to optimize performance in VMs.
(除了本节中的其他建议之外，以下措施可能有助于优化 VM 中的性能。)

 - Using the appropriate mechanism (vcpupin) in the VM, pin the CPUs to
   individual LCPUs, making sure to use a set of CPUs included in the
   device's local_cpulist: /sys/class/net/<ethX>/device/local_cpulist.
   (在 VM 中使用适当的机制(vcpupin)将 CPU 绑定到各个 LCPU，确保使用设备的
   local_cpulist 中所包含的一组 CPU：
   /sys/class/net/<ethX>/device/local_cpulist。)

 - Configure as many Rx/Tx queues in the VM as available. (See the idpf driver
   documentation for the number of queues supported.) For example::
   (在 VM 中配置尽可能多的可用 Rx/Tx 队列。(支持的队列数量请参阅 idpf 驱动程序
   文档。)例如：)

     # ethtool -L <virt_interface> rx <max> tx <max>


Support(支持)
=============

For general information, go to the Intel support website at:
http://www.intel.com/support/
(有关一般信息，请访问 Intel 支持网站：
http://www.intel.com/support/)

If an issue is identified with the released source code on a supported kernel
with a supported adapter, email the specific information related to the issue
to intel-wired-lan@lists.osuosl.org.
(如果在受支持的内核上使用受支持的适配器时发现已发布源代码存在问题，请将与该问题相关
的具体信息通过电子邮件发送至 intel-wired-lan@lists.osuosl.org。)


Trademarks(商标)
================

Intel is a trademark or registered trademark of Intel Corporation or its
subsidiaries in the United States and/or other countries.
(Intel 是 Intel Corporation 或其子公司在美国和/或其他国家/地区的商标或注册商标。)

* Other names and brands may be claimed as the property of others.
  (其他名称和品牌可能被视为他人的财产。)
