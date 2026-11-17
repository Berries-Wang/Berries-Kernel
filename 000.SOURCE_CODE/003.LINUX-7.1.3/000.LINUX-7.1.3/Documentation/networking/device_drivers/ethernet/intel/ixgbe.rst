.. SPDX-License-Identifier: GPL-2.0+

=================================================================================================================================================
Linux Base Driver for the Intel(R) Ethernet 10 Gigabit PCI Express Adapters(Intel(R) Ethernet 10 Gigabit PCI Express 适配器的 Linux 基础驱动程序)
=================================================================================================================================================

Intel 10 Gigabit Linux driver.
(Intel 10 Gigabit Linux 驱动程序。)
Copyright(c) 1999-2018 Intel Corporation.
(版权所有 (c) 1999-2018 Intel Corporation。)

Contents(目录)
==============

- Identifying Your Adapter
  (识别你的适配器)
- Command Line Parameters
  (命令行参数)
- Additional Configurations
  (附加配置)
- Known Issues
  (已知问题)
- Support
  (支持)

Identifying Your Adapter(识别你的适配器)
========================================

The driver is compatible with devices based on the following:
(该驱动程序兼容基于以下控制器的设备：)

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
(有关如何识别你的适配器以及获取最新 Intel 网络驱动程序的信息，请参阅 Intel 支持网站：https://www.intel.com/support)

SFP+ Devices with Pluggable Optics(带可插拔光模块的 SFP+ 设备)
--------------------------------------------------------------

82599-BASED ADAPTERS(基于 82599 的适配器)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NOTES:
(注意事项：)
- If your 82599-based Intel(R) Network Adapter came with Intel optics or is an
Intel(R) Ethernet Server Adapter X520-2, then it only supports Intel optics
and/or the direct attach cables listed below.
(如果你的基于 82599 的 Intel(R) 网络适配器随附 Intel 光模块，或者是 Intel(R) Ethernet Server Adapter X520-2，那么它仅支持 Intel 光模块和/或下面列出的直连电缆。)
- When 82599-based SFP+ devices are connected back to back, they should be set
to the same Speed setting via ethtool. Results may vary if you mix speed
settings.
(当基于 82599 的 SFP+ 设备背靠背连接时，应当通过 ethtool 将它们设置为相同的速率。如果混合使用不同的速率设置，结果可能会有所不同。)

+---------------+---------------------------------------+------------------+
| Supplier      | Type                                  | Part Numbers     |
+===============+=======================================+==================+
| SR Modules                                                               |
+---------------+---------------------------------------+------------------+
| Intel         | DUAL RATE 1G/10G SFP+ SR (bailed)     | FTLX8571D3BCV-IT |
+---------------+---------------------------------------+------------------+
| Intel         | DUAL RATE 1G/10G SFP+ SR (bailed)     | AFBR-703SDZ-IN2  |
+---------------+---------------------------------------+------------------+
| Intel         | DUAL RATE 1G/10G SFP+ SR (bailed)     | AFBR-703SDDZ-IN1 |
+---------------+---------------------------------------+------------------+
| LR Modules                                                               |
+---------------+---------------------------------------+------------------+
| Intel         | DUAL RATE 1G/10G SFP+ LR (bailed)     | FTLX1471D3BCV-IT |
+---------------+---------------------------------------+------------------+
| Intel         | DUAL RATE 1G/10G SFP+ LR (bailed)     | AFCT-701SDZ-IN2  |
+---------------+---------------------------------------+------------------+
| Intel         | DUAL RATE 1G/10G SFP+ LR (bailed)     | AFCT-701SDDZ-IN1 |
+---------------+---------------------------------------+------------------+

The following is a list of 3rd party SFP+ modules that have received some
testing. Not all modules are applicable to all devices.
(以下是经过部分测试的第三方 SFP+ 模块列表。并非所有模块都适用于所有设备。)

+---------------+---------------------------------------+------------------+
| Supplier      | Type                                  | Part Numbers     |
+===============+=======================================+==================+
| Finisar       | SFP+ SR bailed, 10g single rate       | FTLX8571D3BCL    |
+---------------+---------------------------------------+------------------+
| Avago         | SFP+ SR bailed, 10g single rate       | AFBR-700SDZ      |
+---------------+---------------------------------------+------------------+
| Finisar       | SFP+ LR bailed, 10g single rate       | FTLX1471D3BCL    |
+---------------+---------------------------------------+------------------+
| Finisar       | DUAL RATE 1G/10G SFP+ SR (No Bail)    | FTLX8571D3QCV-IT |
+---------------+---------------------------------------+------------------+
| Avago         | DUAL RATE 1G/10G SFP+ SR (No Bail)    | AFBR-703SDZ-IN1  |
+---------------+---------------------------------------+------------------+
| Finisar       | DUAL RATE 1G/10G SFP+ LR (No Bail)    | FTLX1471D3QCV-IT |
+---------------+---------------------------------------+------------------+
| Avago         | DUAL RATE 1G/10G SFP+ LR (No Bail)    | AFCT-701SDZ-IN1  |
+---------------+---------------------------------------+------------------+
| Finisar       | 1000BASE-T SFP                        | FCLF8522P2BTL    |
+---------------+---------------------------------------+------------------+
| Avago         | 1000BASE-T                            | ABCU-5710RZ      |
+---------------+---------------------------------------+------------------+
| HP            | 1000BASE-SX SFP                       | 453153-001       |
+---------------+---------------------------------------+------------------+

82599-based adapters support all passive and active limiting direct attach
cables that comply with SFF-8431 v4.1 and SFF-8472 v10.4 specifications.
(基于 82599 的适配器支持所有符合 SFF-8431 v4.1 和 SFF-8472 v10.4 规范的无源及有源限距直连电缆。)

Laser turns off for SFP+ when ifconfig ethX down(ifconfig ethX down 时 SFP+ 的激光器会关闭)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
"ifconfig ethX down" turns off the laser for 82599-based SFP+ fiber adapters.
"ifconfig ethX up" turns on the laser.
Alternatively, you can use "ip link set [down/up] dev ethX" to turn the
laser off and on.
("ifconfig ethX down" 会关闭基于 82599 的 SFP+ 光纤适配器上的激光器。"ifconfig ethX up" 会打开激光器。或者，你也可以使用 "ip link set [down/up] dev ethX" 来关闭和打开激光器。)


82599-based QSFP+ Adapters(基于 82599 的 QSFP+ 适配器)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NOTES:
(注意事项：)
- If your 82599-based Intel(R) Network Adapter came with Intel optics, it only
supports Intel optics.
(如果你的基于 82599 的 Intel(R) 网络适配器随附 Intel 光模块，则它仅支持 Intel 光模块。)
- 82599-based QSFP+ adapters only support 4x10 Gbps connections.  1x40 Gbps
connections are not supported. QSFP+ link partners must be configured for
4x10 Gbps.
(基于 82599 的 QSFP+ 适配器仅支持 4x10 Gbps 连接，不支持 1x40 Gbps 连接。QSFP+ 链路伙伴必须配置为 4x10 Gbps。)
- 82599-based QSFP+ adapters do not support automatic link speed detection.
The link speed must be configured to either 10 Gbps or 1 Gbps to match the link
partners speed capabilities. Incorrect speed configurations will result in
failure to link.
(基于 82599 的 QSFP+ 适配器不支持自动链路速率检测。链路速率必须配置为 10 Gbps 或 1 Gbps，以匹配链路伙伴的速率能力。错误的速率配置将导致无法建立链路。)
- Intel(R) Ethernet Converged Network Adapter X520-Q1 only supports the optics
and direct attach cables listed below.
(Intel(R) Ethernet Converged Network Adapter X520-Q1 仅支持下面列出的光模块和直连电缆。)

+---------------+---------------------------------------+------------------+
| Supplier      | Type                                  | Part Numbers     |
+===============+=======================================+==================+
| Intel         | DUAL RATE 1G/10G QSFP+ SRL (bailed)   | E10GQSFPSR       |
+---------------+---------------------------------------+------------------+

82599-based QSFP+ adapters support all passive and active limiting QSFP+
direct attach cables that comply with SFF-8436 v4.1 specifications.
(基于 82599 的 QSFP+ 适配器支持所有符合 SFF-8436 v4.1 规范的无源及有源限距 QSFP+ 直连电缆。)

82598-BASED ADAPTERS(基于 82598 的适配器)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NOTES:
(注意事项：)
- Intel(r) Ethernet Network Adapters that support removable optical modules
only support their original module type (for example, the Intel(R) 10 Gigabit
SR Dual Port Express Module only supports SR optical modules). If you plug in
a different type of module, the driver will not load.
(支持可拆卸光模块的 Intel(r) 以太网网络适配器仅支持其原始模块类型 (例如，Intel(R) 10 Gigabit SR Dual Port Express Module 仅支持 SR 光模块)。如果插入其他类型的模块，驱动程序将无法加载。)
- Hot Swapping/hot plugging optical modules is not supported.
(不支持热交换/热插拔光模块。)
- Only single speed, 10 gigabit modules are supported.
(仅支持单一速率的万兆模块。)
- LAN on Motherboard (LOMs) may support DA, SR, or LR modules. Other module
types are not supported. Please see your system documentation for details.
(板载 LAN (LOM) 可能支持 DA、SR 或 LR 模块。不支持其他模块类型。详情请参阅你的系统文档。)

The following is a list of SFP+ modules and direct attach cables that have
received some testing. Not all modules are applicable to all devices.
(以下是经过部分测试的 SFP+ 模块和直连电缆列表。并非所有模块都适用于所有设备。)

+---------------+---------------------------------------+------------------+
| Supplier      | Type                                  | Part Numbers     |
+===============+=======================================+==================+
| Finisar       | SFP+ SR bailed, 10g single rate       | FTLX8571D3BCL    |
+---------------+---------------------------------------+------------------+
| Avago         | SFP+ SR bailed, 10g single rate       | AFBR-700SDZ      |
+---------------+---------------------------------------+------------------+
| Finisar       | SFP+ LR bailed, 10g single rate       | FTLX1471D3BCL    |
+---------------+---------------------------------------+------------------+

82598-based adapters support all passive direct attach cables that comply with
SFF-8431 v4.1 and SFF-8472 v10.4 specifications. Active direct attach cables
are not supported.
(基于 82598 的适配器支持所有符合 SFF-8431 v4.1 和 SFF-8472 v10.4 规范的无源直连电缆。不支持有源直连电缆。)

Third party optic modules and cables referred to above are listed only for the
purpose of highlighting third party specifications and potential
compatibility, and are not recommendations or endorsements or sponsorship of
any third party's product by Intel. Intel is not endorsing or promoting
products made by any third party and the third party reference is provided
only to share information regarding certain optic modules and cables with the
above specifications. There may be other manufacturers or suppliers, producing
or supplying optic modules and cables with similar or matching descriptions.
Customers must use their own discretion and diligence to purchase optic
modules and cables from any third party of their choice. Customers are solely
responsible for assessing the suitability of the product and/or devices and
for the selection of the vendor for purchasing any product. THE OPTIC MODULES
AND CABLES REFERRED TO ABOVE ARE NOT WARRANTED OR SUPPORTED BY INTEL. INTEL
ASSUMES NO LIABILITY WHATSOEVER, AND INTEL DISCLAIMS ANY EXPRESS OR IMPLIED
WARRANTY, RELATING TO SALE AND/OR USE OF SUCH THIRD PARTY PRODUCTS OR
SELECTION OF VENDOR BY CUSTOMERS.
(上文提到的第三方光模块和电缆的列出仅是为了强调第三方规范及潜在的兼容性，并非 Intel 对任何第三方产品的推荐、认可或赞助。Intel 不为任何第三方制造的产品背书或推广，提供第三方信息仅是为了分享符合上述规范的某些光模块和电缆的相关信息。可能还存在其他生产或供应具有相似或相同描述的光模块和电缆的制造商或供应商。客户在从其自行选择的任何第三方购买光模块和电缆时，必须自行审慎判断并尽到注意义务。客户应自行负责评估产品和/或设备的适用性，并自行负责选择购买任何产品的供应商。上文提到的光模块和电缆不享受 Intel 的保修或支持。对于此类第三方产品的销售和/或使用或客户对供应商的选择，Intel 不承担任何责任，并声明不作出任何明示或默示的保证。)

Command Line Parameters(命令行参数)
===================================

max_vfs
-------
(max_vfs)
:Valid Range: 1-63
(有效范围：1-63)

This parameter adds support for SR-IOV. It causes the driver to spawn up to
max_vfs worth of virtual functions.
If the value is greater than 0 it will also force the VMDq parameter to be 1 or
more.
(此参数增加对 SR-IOV 的支持。它使驱动程序生成最多 max_vfs 个虚拟函数。如果该值大于 0，还会强制 VMDq 参数至少为 1。)

NOTE: This parameter is only used on kernel 3.7.x and below. On kernel 3.8.x
and above, use sysfs to enable VFs. Also, for Red Hat distributions, this
parameter is only used on version 6.6 and older. For version 6.7 and newer, use
sysfs. For example::
(注意：此参数仅在 3.7.x 及更早版本的内核上使用。在 3.8.x 及更高版本的内核上，请使用 sysfs 来启用 VF。此外，对于 Red Hat 发行版，此参数仅在 6.6 及更早版本上使用。对于 6.7 及更新版本，请使用 sysfs。例如：)

  #echo $num_vf_enabled > /sys/class/net/$dev/device/sriov_numvfs // enable VFs
  #echo 0 > /sys/class/net/$dev/device/sriov_numvfs               //disable VFs

The parameters for the driver are referenced by position. Thus, if you have a
dual port adapter, or more than one adapter in your system, and want N virtual
functions per port, you must specify a number for each port with each parameter
separated by a comma. For example::
(驱动程序参数按位置引用。因此，如果你有一个双端口适配器，或者系统中不止一个适配器，并且希望每个端口拥有 N 个虚拟函数，则必须为每个端口各指定一个数字，各参数之间用逗号分隔。例如：)

  modprobe ixgbe max_vfs=4

This will spawn 4 VFs on the first port.
(这会在第一个端口上生成 4 个 VF。)

::

  modprobe ixgbe max_vfs=2,4

This will spawn 2 VFs on the first port and 4 VFs on the second port.
(这会在第一个端口上生成 2 个 VF，在第二个端口上生成 4 个 VF。)

NOTE: Caution must be used in loading the driver with these parameters.
Depending on your system configuration, number of slots, etc., it is impossible
to predict in all cases where the positions would be on the command line.
(注意：使用这些参数加载驱动程序时必须小心。取决于你的系统配置、插槽数量等，并非在所有情况下都能预测各位置在命令行上的对应关系。)

NOTE: Neither the device nor the driver control how VFs are mapped into config
space. Bus layout will vary by operating system. On operating systems that
support it, you can check sysfs to find the mapping.
(注意：设备和驱动程序都不控制 VF 如何映射到配置空间。总线布局因操作系统而异。在支持该功能的操作系统上，你可以查看 sysfs 来找到映射关系。)

NOTE: When either SR-IOV mode or VMDq mode is enabled, hardware VLAN filtering
and VLAN tag stripping/insertion will remain enabled. Please remove the old
VLAN filter before the new VLAN filter is added. For example,
(注意：当 SR-IOV 模式或 VMDq 模式任一被启用时，硬件 VLAN 过滤以及 VLAN 标签的剥离/插入将保持启用。在添加新的 VLAN 过滤器之前，请先移除旧的 VLAN 过滤器。例如，)

::

  ip link set eth0 vf 0 vlan 100 // set VLAN 100 for VF 0
  ip link set eth0 vf 0 vlan 0   // Delete VLAN 100
  ip link set eth0 vf 0 vlan 200 // set a new VLAN 200 for VF 0

With kernel 3.6, the driver supports the simultaneous usage of max_vfs and DCB
features, subject to the constraints described below. Prior to kernel 3.6, the
driver did not support the simultaneous operation of max_vfs greater than 0 and
the DCB features (multiple traffic classes utilizing Priority Flow Control and
Extended Transmission Selection).
(从内核 3.6 开始，驱动程序支持同时使用 max_vfs 和 DCB 功能，但受下述限制条件的约束。在内核 3.6 之前，驱动程序不支持 max_vfs 大于 0 与 DCB 功能 (利用优先级流控制和扩展传输选择的多个流量类别) 同时工作。)

When DCB is enabled, network traffic is transmitted and received through
multiple traffic classes (packet buffers in the NIC). The traffic is associated
with a specific class based on priority, which has a value of 0 through 7 used
in the VLAN tag. When SR-IOV is not enabled, each traffic class is associated
with a set of receive/transmit descriptor queue pairs. The number of queue
pairs for a given traffic class depends on the hardware configuration. When
SR-IOV is enabled, the descriptor queue pairs are grouped into pools. The
Physical Function (PF) and each Virtual Function (VF) is allocated a pool of
receive/transmit descriptor queue pairs. When multiple traffic classes are
configured (for example, DCB is enabled), each pool contains a queue pair from
each traffic class. When a single traffic class is configured in the hardware,
the pools contain multiple queue pairs from the single traffic class.
(启用 DCB 时，网络流量通过多个流量类别 (网卡中的数据包缓冲区) 进行发送和接收。流量根据优先级关联到特定的类别，优先级取值为 0 到 7，用在 VLAN 标签中。当未启用 SR-IOV 时，每个流量类别关联一组接收/发送描述符队列对。给定流量类别的队列对数量取决于硬件配置。启用 SR-IOV 时，描述符队列对被分组到池中。物理函数 (PF) 和每个虚拟函数 (VF) 各分配到一个接收/发送描述符队列对池。当配置了多个流量类别时 (例如启用了 DCB)，每个池包含来自每个流量类别的一个队列对。当硬件中只配置了单个流量类别时，各池包含来自该单个流量类别的多个队列对。)

The number of VFs that can be allocated depends on the number of traffic
classes that can be enabled. The configurable number of traffic classes for
each enabled VF is as follows:
0 - 15 VFs = Up to 8 traffic classes, depending on device support
16 - 31 VFs = Up to 4 traffic classes
32 - 63 VFs = 1 traffic class
(可分配的 VF 数量取决于可启用的流量类别数量。每个已启用的 VF 可配置的流量类别数量如下：
0 - 15 个 VF = 最多 8 个流量类别，取决于设备支持情况
16 - 31 个 VF = 最多 4 个流量类别
32 - 63 个 VF = 1 个流量类别)

When VFs are configured, the PF is allocated one pool as well. The PF supports
the DCB features with the constraint that each traffic class will only use a
single queue pair. When zero VFs are configured, the PF can support multiple
queue pairs per traffic class.
(配置 VF 时，PF 也会被分配一个池。PF 支持 DCB 功能，但限制是每个流量类别只能使用一个队列对。当配置零个 VF 时，PF 可以支持每个流量类别使用多个队列对。)

allow_unsupported_sfp
---------------------
(allow_unsupported_sfp)
:Valid Range: 0,1
(有效范围：0、1)
:Default Value: 0 (disabled)
(默认值：0 (禁用))

This parameter allows unsupported and untested SFP+ modules on 82599-based
adapters, as long as the type of module is known to the driver.
(此参数允许在基于 82599 的适配器上使用未支持且未测试过的 SFP+ 模块，只要驱动程序认识该模块的类型即可。)

debug
-----
(debug)
:Valid Range: 0-16 (0=none,...,16=all)
(有效范围：0-16 (0=无，...,16=全部))
:Default Value: 0
(默认值：0)

This parameter adjusts the level of debug messages displayed in the system
logs.
(此参数用于调整系统日志中显示的调试消息的级别。)


Additional Features and Configurations(附加功能与配置)
======================================================

Flow Control(流控制)
--------------------

Ethernet Flow Control (IEEE 802.3x) can be configured with ethtool to enable
receiving and transmitting pause frames for ixgbe. When transmit is enabled,
pause frames are generated when the receive packet buffer crosses a predefined
threshold. When receive is enabled, the transmit unit will halt for the time
delay specified when a pause frame is received.
(可以使用 ethtool 配置以太网流控制 (IEEE 802.3x)，为 ixgbe 启用暂停帧的接收和发送。启用发送时，当接收数据包缓冲区超过预定义阈值时就会生成暂停帧。启用接收时，收到暂停帧后，发送单元将暂停一段指定的时间延迟。)

NOTE: You must have a flow control capable link partner.
(注意：你必须拥有一个支持流控制的链路伙伴。)

Flow Control is enabled by default.
(流控制默认启用。)

Use ethtool to change the flow control settings. To enable or disable Rx or
Tx Flow Control::
(使用 ethtool 更改流控制设置。要启用或禁用 Rx 或 Tx 流控制：)

  ethtool -A eth? rx <on|off> tx <on|off>

Note: This command only enables or disables Flow Control if auto-negotiation is
disabled. If auto-negotiation is enabled, this command changes the parameters
used for auto-negotiation with the link partner.
(注意：只有在自动协商被禁用时，此命令才会启用或禁用流控制。如果自动协商已启用，此命令会更改与链路伙伴进行自动协商时所使用的参数。)

To enable or disable auto-negotiation::
(要启用或禁用自动协商：)

  ethtool -s eth? autoneg <on|off>

Note: Flow Control auto-negotiation is part of link auto-negotiation. Depending
on your device, you may not be able to change the auto-negotiation setting.
(注意：流控制自动协商是链路自动协商的一部分。取决于你的设备，你可能无法更改自动协商设置。)

NOTE: For 82598 backplane cards entering 1 gigabit mode, flow control default
behavior is changed to off. Flow control in 1 gigabit mode on these devices can
lead to transmit hangs.
(注意：对于进入 1 Gb 模式的 82598 背板卡，流控制的默认行为会变为关闭。这些设备在 1 Gb 模式下的流控制可能导致发送挂起。)

Intel(R) Ethernet Flow Director
-------------------------------
(Intel(R) Ethernet Flow Director)
The Intel Ethernet Flow Director performs the following tasks:
(Intel Ethernet Flow Director 执行以下任务：)

- Directs receive packets according to their flows to different queues.
  (根据流将接收的数据包引导到不同的队列。)
- Enables tight control on routing a flow in the platform.
  (支持对平台上流的路由进行严格控制。)
- Matches flows and CPU cores for flow affinity.
  (将流与 CPU 核心相匹配以实现流亲和性。)
- Supports multiple parameters for flexible flow classification and load
  balancing (in SFP mode only).
  (支持多种参数，实现灵活的流分类和负载均衡 (仅限 SFP 模式)。)

NOTE: Intel Ethernet Flow Director masking works in the opposite manner from
subnet masking. In the following command::
(注意：Intel Ethernet Flow Director 的掩码工作方式与子网掩码相反。在以下命令中：)

  #ethtool -N eth11 flow-type ip4 src-ip 172.4.1.2 m 255.0.0.0 dst-ip \
  172.21.1.1 m 255.128.0.0 action 31

The src-ip value that is written to the filter will be 0.4.1.2, not 172.0.0.0
as might be expected. Similarly, the dst-ip value written to the filter will be
0.21.1.1, not 172.0.0.0.
(写入过滤器的 src-ip 值将是 0.4.1.2，而不是人们可能预期的 172.0.0.0。类似地，写入过滤器的 dst-ip 值将是 0.21.1.1，而不是 172.0.0.0。)

To enable or disable the Intel Ethernet Flow Director::
(要启用或禁用 Intel Ethernet Flow Director：)

  # ethtool -K ethX ntuple <on|off>

When disabling ntuple filters, all the user programmed filters are flushed from
the driver cache and hardware. All needed filters must be re-added when ntuple
is re-enabled.
(禁用 ntuple 过滤器时，所有用户编程的过滤器都会从驱动程序缓存和硬件中刷新掉。重新启用 ntuple 时，必须重新添加所有需要的过滤器。)

To add a filter that directs packet to queue 2, use -U or -N switch::
(要添加将数据包引导到队列 2 的过滤器，请使用 -U 或 -N 开关：)

  # ethtool -N ethX flow-type tcp4 src-ip 192.168.10.1 dst-ip \
  192.168.10.2 src-port 2000 dst-port 2001 action 2 [loc 1]

To see the list of filters currently present::
(要查看当前存在的过滤器列表：)

  # ethtool <-u|-n> ethX

Sideband Perfect Filters(带外完美过滤器 (Sideband Perfect Filters))
-------------------------------------------------------------------

Sideband Perfect Filters are used to direct traffic that matches specified
characteristics. They are enabled through ethtool's ntuple interface. To add a
new filter use the following command::
(带外完美过滤器用于将匹配指定特征的流量引导到指定位置。它们通过 ethtool 的 ntuple 接口启用。要添加新过滤器，请使用以下命令：)

  ethtool -U <device> flow-type <type> src-ip <ip> dst-ip <ip> src-port <port> \
  dst-port <port> action <queue>

Where:
(其中：)

  <device> - the ethernet device to program
  (<device> - 要编程的以太网设备)
  <type> - can be ip4, tcp4, udp4, or sctp4
  (<type> - 可以是 ip4、tcp4、udp4 或 sctp4)
  <ip> - the IP address to match on
  (<ip> - 要匹配的 IP 地址)
  <port> - the port number to match on
  (<port> - 要匹配的端口号)
  <queue> - the queue to direct traffic towards (-1 discards the matched traffic)
  (<queue> - 流量要被引导到的队列 (-1 表示丢弃匹配的流量))

Use the following command to delete a filter::
(使用以下命令删除过滤器：)

  ethtool -U <device> delete <N>

Where <N> is the filter id displayed when printing all the active filters, and
may also have been specified using "loc <N>" when adding the filter.
(其中 <N> 是打印所有活动过滤器时显示的过滤器 ID，在添加过滤器时它也可能已通过 "loc <N>" 指定。)

The following example matches TCP traffic sent from 192.168.0.1, port 5300,
directed to 192.168.0.5, port 80, and sends it to queue 7::
(以下示例匹配从 192.168.0.1 端口 5300 发往 192.168.0.5 端口 80 的 TCP 流量，并将其发送到队列 7：)

  ethtool -U enp130s0 flow-type tcp4 src-ip 192.168.0.1 dst-ip 192.168.0.5 \
  src-port 5300 dst-port 80 action 7

For each flow-type, the programmed filters must all have the same matching
input set. For example, issuing the following two commands is acceptable::
(对于每种流类型，已编程的过滤器都必须具有相同的匹配输入集。例如，执行以下两条命令是可以接受的：)

  ethtool -U enp130s0 flow-type ip4 src-ip 192.168.0.1 src-port 5300 action 7
  ethtool -U enp130s0 flow-type ip4 src-ip 192.168.0.5 src-port 55 action 10

Issuing the next two commands, however, is not acceptable, since the first
specifies src-ip and the second specifies dst-ip::
(但是，执行接下来的两条命令是不可接受的，因为第一条指定了 src-ip，而第二条指定了 dst-ip：)

  ethtool -U enp130s0 flow-type ip4 src-ip 192.168.0.1 src-port 5300 action 7
  ethtool -U enp130s0 flow-type ip4 dst-ip 192.168.0.5 src-port 55 action 10

The second command will fail with an error. You may program multiple filters
with the same fields, using different values, but, on one device, you may not
program two TCP4 filters with different matching fields.
(第二条命令将失败并报错。你可以编程多个使用相同字段但取值不同的过滤器，但在同一设备上，不能编程两个具有不同匹配字段的 TCP4 过滤器。)

Matching on a sub-portion of a field is not supported by the ixgbe driver, thus
partial mask fields are not supported.
(ixgbe 驱动程序不支持对字段的子部分进行匹配，因此不支持部分掩码字段。)

To create filters that direct traffic to a specific Virtual Function, use the
"user-def" parameter. Specify the user-def as a 64 bit value, where the lower 32
bits represents the queue number, while the next 8 bits represent which VF.
Note that 0 is the PF, so the VF identifier is offset by 1. For example::
(要创建将流量引导到特定虚拟函数 (VF) 的过滤器，请使用 "user-def" 参数。将 user-def 指定为 64 位值，其中低 32 位表示队列编号，接下来的 8 位表示是哪个 VF。注意 0 表示 PF，因此 VF 标识符偏移了 1。例如：)

  ... user-def 0x800000002 ...

specifies to direct traffic to Virtual Function 7 (8 minus 1) into queue 2 of
that VF.
(表示将流量引导到虚拟函数 7 (即 8 减 1) 的队列 2 (该 VF 的队列 2)。)

Note that these filters will not break internal routing rules, and will not
route traffic that otherwise would not have been sent to the specified Virtual
Function.
(注意，这些过滤器不会破坏内部路由规则，也不会路由原本不会发送到指定虚拟函数的流量。)

Jumbo Frames(巨型帧)
--------------------

Jumbo Frames support is enabled by changing the Maximum Transmission Unit (MTU)
to a value larger than the default value of 1500.
(将最大传输单元 (MTU) 更改为大于默认值 1500 的数值，即可启用巨型帧支持。)

Use the ifconfig command to increase the MTU size. For example, enter the
following where <x> is the interface number::
(使用 ifconfig 命令增大 MTU 大小。例如，输入以下命令，其中 <x> 为接口编号：)

  ifconfig eth<x> mtu 9000 up

Alternatively, you can use the ip command as follows::
(或者，你可以按如下方式使用 ip 命令：)

  ip link set mtu 9000 dev eth<x>
  ip link set up dev eth<x>

This setting is not saved across reboots. The setting change can be made
permanent by adding 'MTU=9000' to the file::
(此设置在重启后不会保留。可以通过在文件中添加 'MTU=9000' 使该设置更改永久生效：)

  /etc/sysconfig/network-scripts/ifcfg-eth<x> // for RHEL
  /etc/sysconfig/network/<config_file> // for SLES

NOTE: The maximum MTU setting for Jumbo Frames is 9710. This value coincides
with the maximum Jumbo Frames size of 9728 bytes.
(注意：巨型帧的最大 MTU 设置为 9710。该值与 9728 字节的最大巨型帧大小相对应。)

NOTE: This driver will attempt to use multiple page sized buffers to receive
each jumbo packet. This should help to avoid buffer starvation issues when
allocating receive packets.
(注意：此驱动程序将尝试使用多个页面大小的缓冲区来接收每个巨型数据包。这应有助于避免在分配接收数据包时出现缓冲区不足的问题。)

NOTE: For 82599-based network connections, if you are enabling jumbo frames in
a virtual function (VF), jumbo frames must first be enabled in the physical
function (PF). The VF MTU setting cannot be larger than the PF MTU.
(注意：对于基于 82599 的网络连接，如果要在虚拟函数 (VF) 中启用巨型帧，必须先在物理函数 (PF) 中启用巨型帧。VF 的 MTU 设置不能大于 PF 的 MTU。)

NBASE-T Support(NBASE-T 支持)
-----------------------------

The ixgbe driver supports NBASE-T on some devices. However, the advertisement
of NBASE-T speeds is suppressed by default, to accommodate broken network
switches which cannot cope with advertised NBASE-T speeds. Use the ethtool
command to enable advertising NBASE-T speeds on devices which support it::
(ixgbe 驱动程序在某些设备上支持 NBASE-T。但是，为了兼容无法应对所通告 NBASE-T 速率的有缺陷交换机，默认情况下会抑制 NBASE-T 速率的通告。在支持该功能的设备上，使用 ethtool 命令启用 NBASE-T 速率通告：)

  ethtool -s eth? advertise 0x1800000001028

On Linux systems with INTERFACES(5), this can be specified as a pre-up command
in /etc/network/interfaces so that the interface is always brought up with
NBASE-T support, e.g.::
(在带有 INTERFACES(5) 的 Linux 系统上，可以将其指定为 /etc/network/interfaces 中的 pre-up 命令，以便接口在启动时始终带有 NBASE-T 支持，例如：)

  iface eth? inet dhcp
       pre-up ethtool -s eth? advertise 0x1800000001028 || true

Generic Receive Offload, aka GRO(通用接收卸载 (GRO))
----------------------------------------------------

The driver supports the in-kernel software implementation of GRO. GRO has
shown that by coalescing Rx traffic into larger chunks of data, CPU
utilization can be significantly reduced when under large Rx load. GRO is an
evolution of the previously-used LRO interface. GRO is able to coalesce
other protocols besides TCP. It's also safe to use with configurations that
are problematic for LRO, namely bridging and iSCSI.
(驱动程序支持内核内的 GRO 软件实现。GRO 表明，通过将 Rx 流量合并成更大的数据块，在高 Rx 负载下可以显著降低 CPU 利用率。GRO 是先前使用的 LRO 接口的演进版本。GRO 能够合并 TCP 以外的其他协议。对于对 LRO 有问题的配置 (即网桥和 iSCSI)，使用 GRO 也是安全的。)

Data Center Bridging (DCB)(数据中心桥接 (DCB))
----------------------------------------------

NOTE:
(注意：)
The kernel assumes that TC0 is available, and will disable Priority Flow
Control (PFC) on the device if TC0 is not available. To fix this, ensure TC0 is
enabled when setting up DCB on your switch.
(内核假定 TC0 可用，如果 TC0 不可用，将在设备上禁用优先级流控制 (PFC)。要解决此问题，请确保在交换机上设置 DCB 时启用 TC0。)

DCB is a configuration Quality of Service implementation in hardware. It uses
the VLAN priority tag (802.1p) to filter traffic. That means that there are 8
different priorities that traffic can be filtered into. It also enables
priority flow control (802.1Qbb) which can limit or eliminate the number of
dropped packets during network stress. Bandwidth can be allocated to each of
these priorities, which is enforced at the hardware level (802.1Qaz).
(DCB 是硬件中的一种可配置服务质量实现。它使用 VLAN 优先级标签 (802.1p) 来过滤流量，这意味着流量可以被过滤为 8 种不同的优先级。它还启用优先级流控制 (802.1Qbb)，可以在网络压力大时限制或消除丢包数量。可以为每种优先级分配带宽，这由硬件级别强制执行 (802.1Qaz)。)

Adapter firmware implements LLDP and DCBX protocol agents as per 802.1AB and
802.1Qaz respectively. The firmware based DCBX agent runs in willing mode only
and can accept settings from a DCBX capable peer. Software configuration of
DCBX parameters via dcbtool/lldptool are not supported.
(适配器固件分别按照 802.1AB 和 802.1Qaz 实现 LLDP 和 DCBX 协议代理。基于固件的 DCBX 代理仅以 willing 模式运行，可以接受来自具备 DCBX 能力的对等端的设置。不支持通过 dcbtool/lldptool 对 DCBX 参数进行软件配置。)

The ixgbe driver implements the DCB netlink interface layer to allow user-space
to communicate with the driver and query DCB configuration for the port.
(ixgbe 驱动程序实现了 DCB netlink 接口层，允许用户空间与驱动程序通信并查询端口的 DCB 配置。)

ethtool
-------
(ethtool)
The driver utilizes the ethtool interface for driver configuration and
diagnostics, as well as displaying statistical information. The latest ethtool
version is required for this functionality. Download it at:
https://www.kernel.org/pub/software/network/ethtool/
(驱动程序利用 ethtool 接口进行驱动程序配置和诊断，以及显示统计信息。此功能需要最新版本的 ethtool。可从以下地址下载：https://www.kernel.org/pub/software/network/ethtool/)

FCoE
----
(FCoE)
The ixgbe driver supports Fiber Channel over Ethernet (FCoE) and Data Center
Bridging (DCB). This code has no default effect on the regular driver
operation. Configuring DCB and FCoE is outside the scope of this README. Refer
to http://www.open-fcoe.org/ for FCoE project information and contact
ixgbe-eedc@lists.sourceforge.net for DCB information.
(ixgbe 驱动程序支持以太网光纤通道 (FCoE) 和数据中心桥接 (DCB)。此代码对常规驱动程序操作没有默认影响。DCB 和 FCoE 的配置不在本 README 的讨论范围内。FCoE 项目信息请参阅 http://www.open-fcoe.org/ ，DCB 信息请联系 ixgbe-eedc@lists.sourceforge.net。)

MAC and VLAN anti-spoofing feature(MAC 和 VLAN 防欺骗功能)
----------------------------------------------------------

When a malicious driver attempts to send a spoofed packet, it is dropped by the
hardware and not transmitted.
(当恶意驱动程序尝试发送伪造的数据包时，硬件会丢弃该数据包，不将其发送出去。)

An interrupt is sent to the PF driver notifying it of the spoof attempt. When a
spoofed packet is detected, the PF driver will send the following message to
the system log (displayed by the "dmesg" command)::
(会向 PF 驱动程序发送一个中断，通知它发生了欺骗尝试。当检测到伪造数据包时，PF 驱动程序将向系统日志 (由 "dmesg" 命令显示) 发送以下消息：)

  ixgbe ethX: ixgbe_spoof_check: n spoofed packets detected

where "x" is the PF interface number; and "n" is number of spoofed packets.
NOTE: This feature can be disabled for a specific Virtual Function (VF)::
(其中 "x" 是 PF 接口编号，"n" 是伪造数据包的数量。注意：可以针对特定的虚拟函数 (VF) 禁用此功能：)

  ip link set <pf dev> vf <vf id> spoofchk {off|on}

IPsec Offload(IPsec 卸载)
-------------------------

The ixgbe driver supports IPsec Hardware Offload.  When creating Security
Associations with "ip xfrm ..." the 'offload' tag option can be used to
register the IPsec SA with the driver in order to get higher throughput in
the secure communications.
(ixgbe 驱动程序支持 IPsec 硬件卸载。在使用 "ip xfrm ..." 创建安全关联 (SA) 时，可以使用 'offload' 标签选项向驱动程序注册 IPsec SA，以便在安全通信中获得更高的吞吐量。)

The offload is also supported for ixgbe's VFs, but the VF must be set as
'trusted' and the support must be enabled with::
(ixgbe 的 VF 也支持该卸载功能，但 VF 必须被设置为 'trusted'，并且必须使用以下命令启用该支持：)

  ethtool --set-priv-flags eth<x> vf-ipsec on
  ip link set eth<x> vf <y> trust on


Known Issues/Troubleshooting(已知问题/故障排除)
===============================================

Enabling SR-IOV in a 64-bit Microsoft Windows Server 2012/R2 guest OS(在 64 位 Microsoft Windows Server 2012/R2 客户机操作系统中启用 SR-IOV)
--------------------------------------------------------------------------------------------------------------------------------------------

Linux KVM Hypervisor/VMM supports direct assignment of a PCIe device to a VM.
This includes traditional PCIe devices, as well as SR-IOV-capable devices based
on the Intel Ethernet Controller XL710.
(Linux KVM 虚拟机管理程序/VMM 支持将 PCIe 设备直接分配给虚拟机。这包括传统 PCIe 设备，以及基于 Intel Ethernet Controller XL710 的具备 SR-IOV 能力的设备。)


Support(支持)
=============

For general information, go to the Intel support website at:
https://www.intel.com/support/
(一般信息请访问 Intel 支持网站：https://www.intel.com/support/)

If an issue is identified with the released source code on a supported kernel
with a supported adapter, email the specific information related to the issue
to intel-wired-lan@lists.osuosl.org.
(如果在受支持的内核上使用受支持的适配器时发现已发布源代码的问题，请将与该问题相关的具体信息通过电子邮件发送至 intel-wired-lan@lists.osuosl.org。)
