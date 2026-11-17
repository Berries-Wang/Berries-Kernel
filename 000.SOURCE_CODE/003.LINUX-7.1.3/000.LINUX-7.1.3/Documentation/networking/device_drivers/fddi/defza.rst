.. SPDX-License-Identifier: GPL-2.0

=================================================================================================
Notes on the DEC FDDIcontroller 700 (DEFZA-xx) driver(DEC FDDIcontroller 700 (DEFZA-xx) 驱动说明)
=================================================================================================

:Version: v.1.1.4
           (版本: v.1.1.4)


DEC FDDIcontroller 700 is DEC's first-generation TURBOchannel FDDI
network card, designed in 1990 specifically for the DECstation 5000
model 200 workstation.  The board is a single attachment station and
it was manufactured in two variations, both of which are supported.
(DEC FDDIcontroller 700 是 DEC 的第一代 TURBOchannel FDDI 网卡，于 1990 年专为 DECstation 5000 model 200 工作站设计。该板卡是单连接站 (single attachment station)，曾生产两种变体，两者均受支持。)

First is the SAS MMF DEFZA-AA option, the original design implementing
the standard MMF-PMD, however with a pair of ST connectors rather than
the usual MIC connector.  The other one is the SAS ThinWire/STP DEFZA-CA
option, denoted 700-C, with the network medium selectable by a switch
between the DEC proprietary ThinWire-PMD using a BNC connector and the
standard STP-PMD using a DE-9F connector.  This option can interface to
a DECconcentrator 500 device and, in the case of the STP-PMD, also other
FDDI equipment and was designed to make it easier to transition from
existing IEEE 802.3 10BASE2 Ethernet and IEEE 802.5 Token Ring networks
by providing means to reuse existing cabling.
(第一种是 SAS MMF DEFZA-AA 选项，即实现标准 MMF-PMD 的原始设计，但使用一对 ST 连接器而非常见的 MIC 连接器。另一种是 SAS ThinWire/STP DEFZA-CA 选项，标记为 700-C，可通过开关在网络介质之间选择：使用 BNC 连接器的 DEC 专有 ThinWire-PMD，或使用 DE-9F 连接器的标准 STP-PMD。该选项可以连接 DECconcentrator 500 设备，在采用 STP-PMD 的情况下还可以连接其他 FDDI 设备；其设计目的是通过提供复用现有布线的手段，使从现有 IEEE 802.3 10BASE2 以太网和 IEEE 802.5 令牌环网络的过渡更加容易。)

This driver handles any number of cards installed in a single system.
They get fddi0, fddi1, etc. interface names assigned in the order of
increasing TURBOchannel slot numbers.
(该驱动可处理安装在同一系统中的任意数量的网卡。网卡按 TURBOchannel 槽位号递增的顺序被分配 fddi0、fddi1 等接口名。)

The board only supports DMA on the receive side.  Transmission involves
the use of PIO.  As a result under a heavy transmission load there will
be a significant impact on system performance.
(该板卡仅在接收方向支持 DMA。发送则要使用 PIO。因此在高发送负载下会对系统性能产生显著影响。)

The board supports a 64-entry CAM for matching destination addresses.
Two entries are preoccupied by the Directed Beacon and Ring Purger
multicast addresses and the rest is used as a multicast filter.  An
all-multi mode is also supported for LLC frames and it is used if
requested explicitly or if the CAM overflows.  The promiscuous mode
supports separate enables for LLC and SMT frames, but this driver
doesn't support changing them individually.
(该板卡支持用于匹配目的地址的 64 项 CAM。其中两项被 Directed Beacon 和 Ring Purger 组播地址预先占用，其余用作组播过滤器。对 LLC 帧还支持全组播 (all-multi) 模式，在显式请求或 CAM 溢出时使用。混杂模式支持对 LLC 和 SMT 帧分别启用，但该驱动不支持单独更改它们。)


Known problems:
(已知问题：)

None.
(无。)


To do:
(待办事项：)

5. MAC address change.  The card does not support changing the Media
   Access Controller's address registers but a similar effect can be
   achieved by adding an alias to the CAM.  There is no way to disable
   matching against the original address though.
   (5. MAC 地址修改。网卡不支持修改媒体访问控制器的地址寄存器，但可以通过向 CAM 添加别名来达到类似效果。不过，没有办法禁用对原始地址的匹配。)

7. Queueing incoming/outgoing SMT frames in the driver if the SMT
   receive/RMC transmit ring is full. (?)
   (7. 当 SMT 接收/RMC 发送环已满时，在驱动中排队收发的 SMT 帧。(?))

8. Retrieving/reporting FDDI/SNMP stats.
   (8. 获取/报告 FDDI/SNMP 统计信息。)


Both success and failure reports are welcome.
(无论成功还是失败的报告都欢迎。)

Maciej W. Rozycki  <macro@orcam.me.uk>
