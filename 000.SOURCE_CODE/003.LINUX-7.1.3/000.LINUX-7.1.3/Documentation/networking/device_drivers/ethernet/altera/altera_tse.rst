.. SPDX-License-Identifier: GPL-2.0

.. include:: <isonum.txt>

=======================================
Altera Triple-Speed Ethernet MAC driver
=======================================

(Altera 三速以太网 MAC 驱动)

Copyright |copy| 2008-2014 Altera Corporation
(版权所有 |copy| 2008-2014 Altera Corporation。)

This is the driver for the Altera Triple-Speed Ethernet (TSE) controllers
using the SGDMA and MSGDMA soft DMA IP components. The driver uses the
platform bus to obtain component resources. The designs used to test this
driver were built for a Cyclone(R) V SOC FPGA board, a Cyclone(R) V FPGA board,
and tested with ARM and NIOS processor hosts separately. The anticipated use
cases are simple communications between an embedded system and an external peer
for status and simple configuration of the embedded system.
(这是 Altera 三速以太网 (TSE) 控制器的驱动，使用 SGDMA 和 MSGDMA 软 DMA IP 组件。驱动使用平台总线获取组件资源。用于测试本驱动的设计构建于 Cyclone(R) V SOC FPGA 板和 Cyclone(R) V FPGA 板，并分别以 ARM 和 NIOS 处理器主机进行了测试。预期的使用场景是嵌入式系统与外部对端之间的简单通信，用于获取状态和对嵌入式系统进行简单配置。)

For more information visit www.altera.com and www.rocketboards.org. Support
forums for the driver may be found on www.rocketboards.org, and a design used
to test this driver may be found there as well. Support is also available from
the maintainer of this driver, found in MAINTAINERS.
(更多信息请访问 www.altera.com 和 www.rocketboards.org。驱动的支持论坛可在 www.rocketboards.org 上找到，测试本驱动所用的设计也可以在那里找到。此外，还可以从本驱动的维护者那里获得支持，维护者信息见 MAINTAINERS。)

The Triple-Speed Ethernet, SGDMA, and MSGDMA components are all soft IP
components that can be assembled and built into an FPGA using the Altera
Quartus toolchain. Quartus 13.1 and 14.0 were used to build the design that
this driver was tested against. The sopc2dts tool is used to create the
device tree for the driver, and may be found at rocketboards.org.
(三速以太网、SGDMA 和 MSGDMA 组件都是软 IP 组件，可以使用 Altera Quartus 工具链装配并构建到 FPGA 中。本驱动所测试的设计是使用 Quartus 13.1 和 14.0 构建的。sopc2dts 工具用于为驱动创建设备树，该工具可在 rocketboards.org 上找到。)

The driver probe function examines the device tree and determines if the
Triple-Speed Ethernet instance is using an SGDMA or MSGDMA component. The
probe function then installs the appropriate set of DMA routines to
initialize, setup transmits, receives, and interrupt handling primitives for
the respective configurations.
(驱动的 probe 函数会检查设备树，判断三速以太网实例使用的是 SGDMA 还是 MSGDMA 组件。随后 probe 函数会安装相应的 DMA 例程集，为相应的配置提供初始化、发送设置、接收设置和中断处理原语。)

The SGDMA component is to be deprecated in the near future (over the next 1-2
years as of this writing in early 2014) in favor of the MSGDMA component.
SGDMA support is included for existing designs and reference in case a
developer wishes to support their own soft DMA logic and driver support. Any
new designs should not use the SGDMA.
(SGDMA 组件在不久的将来会被弃用（按撰写本文时即 2014 年初的估计，在未来 1-2 年内），由 MSGDMA 组件取代。为兼容既有设计并供参考，这里仍包含 SGDMA 支持，供希望支持自己软 DMA 逻辑及驱动支持的开发者参考。任何新设计都不应使用 SGDMA。)

The SGDMA supports only a single transmit or receive operation at a time, and
therefore will not perform as well compared to the MSGDMA soft IP. Please
visit www.altera.com for known, documented SGDMA errata.
(SGDMA 一次只支持单个发送或接收操作，因此性能不如 MSGDMA 软 IP。有关 SGDMA 已知的、有记载的勘误，请访问 www.altera.com。)

Scatter-gather DMA is not supported by the SGDMA or MSGDMA at this time.
Scatter-gather DMA will be added to a future maintenance update to this
driver.
(SGDMA 和 MSGDMA 目前都不支持分散-聚集 DMA (Scatter-gather DMA)。分散-聚集 DMA 将在本驱动未来的维护更新中加入。)

Jumbo frames are not supported at this time.
(目前不支持巨型帧 (Jumbo frames)。)

The driver limits PHY operations to 10/100Mbps, and has not yet been fully
tested for 1Gbps. This support will be added in a future maintenance update.
(驱动将 PHY 操作限制在 10/100Mbps，尚未对 1Gbps 进行充分测试。该支持将在未来的维护更新中加入。)

1. Kernel Configuration
(1. 内核配置)
=======================

The kernel configuration option is ALTERA_TSE:
(内核配置选项为 ALTERA_TSE：)

 Device Drivers ---> Network device support ---> Ethernet driver support --->
 Altera Triple-Speed Ethernet MAC support (ALTERA_TSE)
 (设备驱动 ---> 网络设备支持 ---> 以太网驱动支持 ---> Altera 三速以太网 MAC 支持 (ALTERA_TSE))

2. Driver parameters list
(2. 驱动参数列表)
=========================

	- debug: message level (0: no output, 16: all);
	- dma_rx_num: Number of descriptors in the RX list (default is 64);
	- dma_tx_num: Number of descriptors in the TX list (default is 64).
	(调试：消息级别（0：无输出，16：全部）；
	RX 列表中的描述符数量（默认为 64）；
	TX 列表中的描述符数量（默认为 64）。)

3. Command line options
(3. 命令行选项)
=======================

Driver parameters can be also passed in command line by using::
(驱动参数也可以通过命令行传递，方式是使用：)

	altera_tse=dma_rx_num:128,dma_tx_num:512

4. Driver information and notes
(4. 驱动信息与说明)
===============================

4.1. Transmit process
(4.1. 发送过程)
---------------------

When the driver's transmit routine is called by the kernel, it sets up a
transmit descriptor by calling the underlying DMA transmit routine (SGDMA or
MSGDMA), and initiates a transmit operation. Once the transmit is complete, an
interrupt is driven by the transmit DMA logic. The driver handles the transmit
completion in the context of the interrupt handling chain by recycling
resource required to send and track the requested transmit operation.
(当内核调用驱动的发送例程时，它会通过调用底层 DMA 发送例程（SGDMA 或 MSGDMA）设置一个发送描述符，并启动一次发送操作。发送完成后，发送 DMA 逻辑会产生一个中断。驱动在中断处理链的上下文中处理发送完成事件，回收发送和跟踪所请求发送操作所需的资源。)

4.2. Receive process
(4.2. 接收过程)
--------------------

The driver will post receive buffers to the receive DMA logic during driver
initialization. Receive buffers may or may not be queued depending upon the
underlying DMA logic (MSGDMA is able queue receive buffers, SGDMA is not able
to queue receive buffers to the SGDMA receive logic). When a packet is
received, the DMA logic generates an interrupt. The driver handles a receive
interrupt by obtaining the DMA receive logic status, reaping receive
completions until no more receive completions are available.
(驱动会在初始化期间把接收缓冲区投递给接收 DMA 逻辑。接收缓冲区是否排队取决于底层 DMA 逻辑（MSGDMA 能够让接收缓冲区排队，SGDMA 则无法把接收缓冲区排队到 SGDMA 接收逻辑）。当收到一个数据包时，DMA 逻辑会产生一个中断。驱动通过获取 DMA 接收逻辑的状态来处理接收中断，持续收取接收完成事件，直到没有更多可用的接收完成为止。)

4.3. Interrupt Mitigation
(4.3. 中断缓解)
-------------------------

The driver is able to mitigate the number of its DMA interrupts
using NAPI for receive operations. Interrupt mitigation is not yet supported
for transmit operations, but will be added in a future maintenance release.
(驱动能够在接收操作上使用 NAPI 来减少其 DMA 中断的数量。发送操作尚不支持中断缓解，但将在未来的维护版本中加入。)

4.4) Ethtool support
(4.4) Ethtool 支持)
--------------------

Ethtool is supported. Driver statistics and internal errors can be taken using:
ethtool -S ethX command. It is possible to dump registers etc.
(支持 Ethtool。可以使用 ethtool -S ethX 命令获取驱动统计信息和内部错误。还可以转储寄存器等。)

4.5) PHY Support
(4.5) PHY 支持)
----------------

The driver is compatible with PAL to work with PHY and GPHY devices.
(该驱动与 PAL 兼容，可与 PHY 和 GPHY 设备协同工作。)

4.7) List of source files:
(4.7) 源文件列表：)
--------------------------

 - Kconfig
 - Makefile
 - altera_tse_main.c: main network device driver
   (主网络设备驱动)
 - altera_tse_ethtool.c: ethtool support
   (ethtool 支持)
 - altera_tse.h: private driver structure and common definitions
   (驱动私有结构与公共定义)
 - altera_msgdma.h: MSGDMA implementation function definitions
   (MSGDMA 实现的函数定义)
 - altera_sgdma.h: SGDMA implementation function definitions
   (SGDMA 实现的函数定义)
 - altera_msgdma.c: MSGDMA implementation
   (MSGDMA 实现)
 - altera_sgdma.c: SGDMA implementation
   (SGDMA 实现)
 - altera_sgdmahw.h: SGDMA register and descriptor definitions
   (SGDMA 寄存器与描述符定义)
 - altera_msgdmahw.h: MSGDMA register and descriptor definitions
   (MSGDMA 寄存器与描述符定义)
 - altera_utils.c: Driver utility functions
   (驱动工具函数)
 - altera_utils.h: Driver utility function definitions
   (驱动工具函数定义)

5. Debug Information
(5. 调试信息)
====================

The driver exports debug information such as internal statistics,
debug information, MAC and DMA registers etc.
(驱动导出调试信息，例如内部统计、调试信息、MAC 与 DMA 寄存器等。)

A user may use the ethtool support to get statistics:
e.g. using: ethtool -S ethX (that shows the statistics counters)
or sees the MAC registers: e.g. using: ethtool -d ethX
(用户可以使用 ethtool 支持来获取统计信息：例如使用 ethtool -S ethX（显示统计计数器），或查看 MAC 寄存器：例如使用 ethtool -d ethX。)

The developer can also use the "debug" module parameter to get
further debug information.
(开发者还可以使用 "debug" 模块参数来获取更多调试信息。)

6. Statistics Support
(6. 统计支持)
=====================

The controller and driver support a mix of IEEE standard defined statistics,
RFC defined statistics, and driver or Altera defined statistics. The four
specifications containing the standard definitions for these statistics are
as follows:
(控制器和驱动支持的统计是 IEEE 标准定义的统计、RFC 定义的统计以及驱动或 Altera 定义的统计的混合。包含这些统计的标准定义的四个规范如下：)

 - IEEE 802.3-2012 - IEEE Standard for Ethernet.
   (IEEE 802.3-2012 —— IEEE 以太网标准。)
 - RFC 2863 found at http://www.rfc-editor.org/rfc/rfc2863.txt.
   (RFC 2863，见 http://www.rfc-editor.org/rfc/rfc2863.txt。)
 - RFC 2819 found at http://www.rfc-editor.org/rfc/rfc2819.txt.
   (RFC 2819，见 http://www.rfc-editor.org/rfc/rfc2819.txt。)
 - Altera Triple Speed Ethernet User Guide, found at http://www.altera.com
   (Altera 三速以太网用户指南，见 http://www.altera.com)

The statistics supported by the TSE and the device driver are as follows:
(TSE 和设备驱动支持的统计如下：)

"tx_packets" is equivalent to aFramesTransmittedOK defined in IEEE 802.3-2012,
Section 5.2.2.1.2. This statistics is the count of frames that are successfully
transmitted.
("tx_packets" 等价于 IEEE 802.3-2012 第 5.2.2.1.2 节定义的 aFramesTransmittedOK。该统计是成功发送的帧的计数。)

"rx_packets" is equivalent to aFramesReceivedOK defined in IEEE 802.3-2012,
Section 5.2.2.1.5. This statistic is the count of frames that are successfully
received. This count does not include any error packets such as CRC errors,
length errors, or alignment errors.
("rx_packets" 等价于 IEEE 802.3-2012 第 5.2.2.1.5 节定义的 aFramesReceivedOK。该统计是成功接收的帧的计数。该计数不包括任何错误数据包，例如 CRC 错误、长度错误或对齐错误。)

"rx_crc_errors" is equivalent to aFrameCheckSequenceErrors defined in IEEE
802.3-2012, Section 5.2.2.1.6. This statistic is the count of frames that are
an integral number of bytes in length and do not pass the CRC test as the frame
is received.
("rx_crc_errors" 等价于 IEEE 802.3-2012 第 5.2.2.1.6 节定义的 aFrameCheckSequenceErrors。该统计是长度为整数字节、但在接收时未通过 CRC 测试的帧的计数。)

"rx_align_errors" is equivalent to aAlignmentErrors defined in IEEE 802.3-2012,
Section 5.2.2.1.7. This statistic is the count of frames that are not an
integral number of bytes in length and do not pass the CRC test as the frame is
received.
("rx_align_errors" 等价于 IEEE 802.3-2012 第 5.2.2.1.7 节定义的 aAlignmentErrors。该统计是长度不是整数字节、且在接收时未通过 CRC 测试的帧的计数。)

"tx_bytes" is equivalent to aOctetsTransmittedOK defined in IEEE 802.3-2012,
Section 5.2.2.1.8. This statistic is the count of data and pad bytes
successfully transmitted from the interface.
("tx_bytes" 等价于 IEEE 802.3-2012 第 5.2.2.1.8 节定义的 aOctetsTransmittedOK。该统计是从接口成功发送的数据和填充字节的计数。)

"rx_bytes" is equivalent to aOctetsReceivedOK defined in IEEE 802.3-2012,
Section 5.2.2.1.14. This statistic is the count of data and pad bytes
successfully received by the controller.
("rx_bytes" 等价于 IEEE 802.3-2012 第 5.2.2.1.14 节定义的 aOctetsReceivedOK。该统计是控制器成功接收的数据和填充字节的计数。)

"tx_pause" is equivalent to aPAUSEMACCtrlFramesTransmitted defined in IEEE
802.3-2012, Section 30.3.4.2. This statistic is a count of PAUSE frames
transmitted from the network controller.
("tx_pause" 等价于 IEEE 802.3-2012 第 30.3.4.2 节定义的 aPAUSEMACCtrlFramesTransmitted。该统计是从网络控制器发送的 PAUSE 帧的计数。)

"rx_pause" is equivalent to aPAUSEMACCtrlFramesReceived defined in IEEE
802.3-2012, Section 30.3.4.3. This statistic is a count of PAUSE frames
received by the network controller.
("rx_pause" 等价于 IEEE 802.3-2012 第 30.3.4.3 节定义的 aPAUSEMACCtrlFramesReceived。该统计是网络控制器接收到的 PAUSE 帧的计数。)

"rx_errors" is equivalent to ifInErrors defined in RFC 2863. This statistic is
a count of the number of packets received containing errors that prevented the
packet from being delivered to a higher level protocol.
("rx_errors" 等价于 RFC 2863 定义的 ifInErrors。该统计是接收到的、含有妨碍数据包被交付给更高层协议的错误的包数量的计数。)

"tx_errors" is equivalent to ifOutErrors defined in RFC 2863. This statistic
is a count of the number of packets that could not be transmitted due to errors.
("tx_errors" 等价于 RFC 2863 定义的 ifOutErrors。该统计是由于错误而无法发送的数据包数量的计数。)

"rx_unicast" is equivalent to ifInUcastPkts defined in RFC 2863. This
statistic is a count of the number of packets received that were not addressed
to the broadcast address or a multicast group.
("rx_unicast" 等价于 RFC 2863 定义的 ifInUcastPkts。该统计是接收到的、并非寻址到广播地址或某个组播组的包数量的计数。)

"rx_multicast" is equivalent to ifInMulticastPkts defined in RFC 2863. This
statistic is a count of the number of packets received that were addressed to
a multicast address group.
("rx_multicast" 等价于 RFC 2863 定义的 ifInMulticastPkts。该统计是接收到的、寻址到某个组播地址组的包数量的计数。)

"rx_broadcast" is equivalent to ifInBroadcastPkts defined in RFC 2863. This
statistic is a count of the number of packets received that were addressed to
the broadcast address.
("rx_broadcast" 等价于 RFC 2863 定义的 ifInBroadcastPkts。该统计是接收到的、寻址到广播地址的包数量的计数。)

"tx_discards" is equivalent to ifOutDiscards defined in RFC 2863. This
statistic is the number of outbound packets not transmitted even though an
error was not detected. An example of a reason this might occur is to free up
internal buffer space.
("tx_discards" 等价于 RFC 2863 定义的 ifOutDiscards。该统计是即使未检测到错误也未被发送的出站包数量。出现这种情况的一个可能原因是腾出内部缓冲空间。)

"tx_unicast" is equivalent to ifOutUcastPkts defined in RFC 2863. This
statistic counts the number of packets transmitted that were not addressed to
a multicast group or broadcast address.
("tx_unicast" 等价于 RFC 2863 定义的 ifOutUcastPkts。该统计是发送的、并非寻址到某个组播组或广播地址的包数量的计数。)

"tx_multicast" is equivalent to ifOutMulticastPkts defined in RFC 2863. This
statistic counts the number of packets transmitted that were addressed to a
multicast group.
("tx_multicast" 等价于 RFC 2863 定义的 ifOutMulticastPkts。该统计是发送的、寻址到某个组播组的包数量的计数。)

"tx_broadcast" is equivalent to ifOutBroadcastPkts defined in RFC 2863. This
statistic counts the number of packets transmitted that were addressed to a
broadcast address.
("tx_broadcast" 等价于 RFC 2863 定义的 ifOutBroadcastPkts。该统计是发送的、寻址到广播地址的包数量的计数。)

"ether_drops" is equivalent to etherStatsDropEvents defined in RFC 2819.
This statistic counts the number of packets dropped due to lack of internal
controller resources.
("ether_drops" 等价于 RFC 2819 定义的 etherStatsDropEvents。该统计是因内部控制器资源不足而被丢弃的包数量的计数。)

"rx_total_bytes" is equivalent to etherStatsOctets defined in RFC 2819.
This statistic counts the total number of bytes received by the controller,
including error and discarded packets.
("rx_total_bytes" 等价于 RFC 2819 定义的 etherStatsOctets。该统计是控制器接收到的字节总数，包括错误和被丢弃的数据包。)

"rx_total_packets" is equivalent to etherStatsPkts defined in RFC 2819.
This statistic counts the total number of packets received by the controller,
including error, discarded, unicast, multicast, and broadcast packets.
("rx_total_packets" 等价于 RFC 2819 定义的 etherStatsPkts。该统计是控制器接收到的包总数，包括错误、被丢弃、单播、组播和广播数据包。)

"rx_undersize" is equivalent to etherStatsUndersizePkts defined in RFC 2819.
This statistic counts the number of correctly formed packets received less
than 64 bytes long.
("rx_undersize" 等价于 RFC 2819 定义的 etherStatsUndersizePkts。该统计是接收到的、格式正确但长度小于 64 字节的包数量的计数。)

"rx_oversize" is equivalent to etherStatsOversizePkts defined in RFC 2819.
This statistic counts the number of correctly formed packets greater than 1518
bytes long.
("rx_oversize" 等价于 RFC 2819 定义的 etherStatsOversizePkts。该统计是接收到的、格式正确但长度大于 1518 字节的包数量的计数。)

"rx_64_bytes" is equivalent to etherStatsPkts64Octets defined in RFC 2819.
This statistic counts the total number of packets received that were 64 octets
in length.
("rx_64_bytes" 等价于 RFC 2819 定义的 etherStatsPkts64Octets。该统计是接收到的、长度为 64 个八位组的包总数。)

"rx_65_127_bytes" is equivalent to etherStatsPkts65to127Octets defined in RFC
2819. This statistic counts the total number of packets received that were
between 65 and 127 octets in length inclusive.
("rx_65_127_bytes" 等价于 RFC 2819 定义的 etherStatsPkts65to127Octets。该统计是接收到的、长度在 65 到 127 个八位组之间（含）的包总数。)

"rx_128_255_bytes" is equivalent to etherStatsPkts128to255Octets defined in
RFC 2819. This statistic is the total number of packets received that were
between 128 and 255 octets in length inclusive.
("rx_128_255_bytes" 等价于 RFC 2819 定义的 etherStatsPkts128to255Octets。该统计是接收到的、长度在 128 到 255 个八位组之间（含）的包总数。)

"rx_256_511_bytes" is equivalent to etherStatsPkts256to511Octets defined in
RFC 2819. This statistic is the total number of packets received that were
between 256 and 511 octets in length inclusive.
("rx_256_511_bytes" 等价于 RFC 2819 定义的 etherStatsPkts256to511Octets。该统计是接收到的、长度在 256 到 511 个八位组之间（含）的包总数。)

"rx_512_1023_bytes" is equivalent to etherStatsPkts512to1023Octets defined in
RFC 2819. This statistic is the total number of packets received that were
between 512 and 1023 octets in length inclusive.
("rx_512_1023_bytes" 等价于 RFC 2819 定义的 etherStatsPkts512to1023Octets。该统计是接收到的、长度在 512 到 1023 个八位组之间（含）的包总数。)

"rx_1024_1518_bytes" is equivalent to etherStatsPkts1024to1518Octets define
in RFC 2819. This statistic is the total number of packets received that were
between 1024 and 1518 octets in length inclusive.
("rx_1024_1518_bytes" 等价于 RFC 2819 定义的 etherStatsPkts1024to1518Octets。该统计是接收到的、长度在 1024 到 1518 个八位组之间（含）的包总数。)

"rx_gte_1519_bytes" is a statistic defined specific to the behavior of the
Altera TSE. This statistics counts the number of received good and errored
frames between the length of 1519 and the maximum frame length configured
in the frm_length register. See the Altera TSE User Guide for More details.
("rx_gte_1519_bytes" 是针对 Altera TSE 行为专门定义的统计。该统计是接收到的、长度介于 1519 与 frm_length 寄存器中所配置最大帧长之间的好帧和错误帧数量的计数。更多细节请参阅 Altera TSE 用户指南。)

"rx_jabbers" is equivalent to etherStatsJabbers defined in RFC 2819. This
statistic is the total number of packets received that were longer than 1518
octets, and had either a bad CRC with an integral number of octets (CRC Error)
or a bad CRC with a non-integral number of octets (Alignment Error).
("rx_jabbers" 等价于 RFC 2819 定义的 etherStatsJabbers。该统计是接收到的、长度超过 1518 个八位组，且或者是整数字节数的坏 CRC（CRC 错误）、或者是非整数字节数的坏 CRC（对齐错误）的包总数。)

"rx_runts" is equivalent to etherStatsFragments defined in RFC 2819. This
statistic is the total number of packets received that were less than 64 octets
in length and had either a bad CRC with an integral number of octets (CRC
error) or a bad CRC with a non-integral number of octets (Alignment Error).
("rx_runts" 等价于 RFC 2819 定义的 etherStatsFragments。该统计是接收到的、长度小于 64 个八位组，且或者是整数字节数的坏 CRC（CRC 错误）、或者是非整数字节数的坏 CRC（对齐错误）的包总数。)
