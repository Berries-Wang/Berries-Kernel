=================================
PHY Abstraction Layer(PHY 抽象层)
=================================

Purpose(目的)
=============

Most network devices consist of set of registers which provide an interface
to a MAC layer, which communicates with the physical connection through a
PHY.  The PHY concerns itself with negotiating link parameters with the link
partner on the other side of the network connection (typically, an ethernet
cable), and provides a register interface to allow drivers to determine what
settings were chosen, and to configure what settings are allowed.
(大多数网络设备由一组寄存器组成，这些寄存器提供通往 MAC 层的接口，MAC 层再通过 PHY 与物理连接进行通信。PHY 负责与网络连接另一端 (通常是以太网电缆) 上的链路伙伴协商链路参数，并提供寄存器接口，让驱动程序能够确定选择了哪些设置，以及配置允许哪些设置。)

While these devices are distinct from the network devices, and conform to a
standard layout for the registers, it has been common practice to integrate
the PHY management code with the network driver.  This has resulted in large
amounts of redundant code.  Also, on embedded systems with multiple (and
sometimes quite different) ethernet controllers connected to the same
management bus, it is difficult to ensure safe use of the bus.
(尽管这些设备与网络设备不同，并且其寄存器布局符合标准，但将 PHY 管理代码与网络驱动程序集成在一起一直是常见的做法。这导致了大量冗余代码。此外，在具有多个 (有时差别很大) 以太网控制器连接到同一管理总线的嵌入式系统上，很难确保总线的安全使用。)

Since the PHYs are devices, and the management busses through which they are
accessed are, in fact, busses, the PHY Abstraction Layer (PAL) treats them as such.
In doing so, it has these goals:
(由于 PHY 是设备，而且访问它们的管理总线实际上就是总线，PHY 抽象层 (PAL) 也据此来对待它们。为此，它有以下目标：)

#. Increase code-reuse
   (提高代码复用率)
#. Increase overall code-maintainability
   (提高整体代码可维护性)
#. Speed development time for new network drivers, and for new systems
   (加快新网络驱动程序和新系统的开发速度)

Basically, this layer is meant to provide an interface to PHY devices which
allows network driver writers to write as little code as possible, while
still providing a full feature set.
(基本上，此层旨在提供通往 PHY 设备的接口，让网络驱动程序编写者能够编写尽可能少的代码，同时仍提供完整的功能集。)

The MDIO bus(MDIO 总线)
=======================

Most network devices are connected to a PHY by means of a management bus.
Different devices use different busses (though some share common interfaces).
In order to take advantage of the PAL, each bus interface needs to be
registered as a distinct device.
(大多数网络设备通过管理总线连接到 PHY。不同的设备使用不同的总线 (尽管有些共享通用接口)。为了利用 PAL，每个总线接口都需要作为一个独立的设备进行注册。)

#. read and write functions must be implemented. Their prototypes are::
   (必须实现读和写函数。它们的原型如下：)

	int write(struct mii_bus *bus, int mii_id, int regnum, u16 value);
	int read(struct mii_bus *bus, int mii_id, int regnum);

   mii_id is the address on the bus for the PHY, and regnum is the register
   number.  These functions are guaranteed not to be called from interrupt
   time, so it is safe for them to block, waiting for an interrupt to signal
   the operation is complete
   (mii_id 是 PHY 在总线上的地址，regnum 是寄存器编号。保证这些函数不会在中断上下文中被调用，因此它们可以安全地阻塞，等待中断来指示操作完成)

#. A reset function is optional. This is used to return the bus to an
   initialized state.
   (复位函数是可选的。它用于将总线恢复到初始化状态。)

#. A probe function is needed.  This function should set up anything the bus
   driver needs, setup the mii_bus structure, and register with the PAL using
   mdiobus_register.  Similarly, there's a remove function to undo all of
   that (use mdiobus_unregister).
   (需要一个探测函数。此函数应设置总线驱动程序所需的任何东西，设置 mii_bus 结构，并使用 mdiobus_register 向 PAL 注册。类似地，还有一个 remove 函数用于撤销所有这些操作 (使用 mdiobus_unregister)。)

#. Like any driver, the device_driver structure must be configured, and init
   exit functions are used to register the driver.
   (与任何驱动程序一样，必须配置 device_driver 结构，并使用 init/exit 函数来注册驱动程序。)

#. The bus must also be declared somewhere as a device, and registered.
   (还必须在某处将总线声明为一个设备并进行注册。)

As an example for how one driver implemented an mdio bus driver, see
drivers/net/ethernet/freescale/fsl_pq_mdio.c and an associated DTS file
for one of the users. (e.g. "git grep fsl,.*-mdio arch/powerpc/boot/dts/")
(有关某个驱动程序如何实现 mdio 总线驱动程序的示例，请参阅 drivers/net/ethernet/freescale/fsl_pq_mdio.c 以及其某个使用者关联的 DTS 文件 (例如 "git grep fsl,.*-mdio arch/powerpc/boot/dts/")。)

(RG)MII/electrical interface considerations((RG)MII/电气接口注意事项)
=====================================================================

The Reduced Gigabit Medium Independent Interface (RGMII) is a 12-pin
electrical signal interface using a synchronous 125Mhz clock signal and several
data lines. Due to this design decision, a 1.5ns to 2ns delay must be added
between the clock line (RXC or TXC) and the data lines to let the PHY (clock
sink) have a large enough setup and hold time to sample the data lines correctly. The
PHY library offers different types of PHY_INTERFACE_MODE_RGMII* values to let
the PHY driver and optionally the MAC driver, implement the required delay. The
values of phy_interface_t must be understood from the perspective of the PHY
device itself, leading to the following:
(简化千兆介质无关接口 (RGMII) 是一种 12 引脚的电气信号接口，使用同步的 125MHz 时钟信号和若干数据线。由于这一设计决策，必须在时钟线 (RXC 或 TXC) 与数据线之间增加 1.5ns 至 2ns 的延迟，以使 PHY (时钟接收端) 拥有足够大的建立和保持时间来正确采样数据线。PHY 库提供不同类型的 PHY_INTERFACE_MODE_RGMII* 值，让 PHY 驱动程序 (以及可选的 MAC 驱动程序) 实现所需的延迟。phy_interface_t 的各个值必须从 PHY 设备自身的角度来理解，由此得出以下几种情况：)

* PHY_INTERFACE_MODE_RGMII: the PHY is not responsible for inserting any
  internal delay by itself, it assumes that either the Ethernet MAC (if capable)
  or the PCB traces insert the correct 1.5-2ns delay
  (PHY_INTERFACE_MODE_RGMII：PHY 自身不负责插入任何内部延迟，它假定由以太网 MAC (如果具备能力) 或 PCB 走线插入正确的 1.5-2ns 延迟)

* PHY_INTERFACE_MODE_RGMII_TXID: the PHY should insert an internal delay
  for the transmit data lines (TXD[3:0]) processed by the PHY device
  (PHY_INTERFACE_MODE_RGMII_TXID：PHY 应为 PHY 设备处理的发送数据线 (TXD[3:0]) 插入内部延迟)

* PHY_INTERFACE_MODE_RGMII_RXID: the PHY should insert an internal delay
  for the receive data lines (RXD[3:0]) processed by the PHY device
  (PHY_INTERFACE_MODE_RGMII_RXID：PHY 应为 PHY 设备处理的接收数据线 (RXD[3:0]) 插入内部延迟)

* PHY_INTERFACE_MODE_RGMII_ID: the PHY should insert internal delays for
  both transmit AND receive data lines from/to the PHY device
  (PHY_INTERFACE_MODE_RGMII_ID：PHY 应为 PHY 设备的发送和接收数据线 (双向) 都插入内部延迟)

Whenever possible, use the PHY side RGMII delay for these reasons:
(出于以下原因，应尽可能使用 PHY 侧的 RGMII 延迟：)

* PHY devices may offer sub-nanosecond granularity in how they allow a
  receiver/transmitter side delay (e.g: 0.5, 1.0, 1.5ns) to be specified. Such
  precision may be required to account for differences in PCB trace lengths
  (PHY 设备在允许指定接收/发送侧延迟 (例如 0.5、1.0、1.5ns) 方面可能提供亚纳秒级的粒度。这种精度可能是补偿 PCB 走线长度差异所必需的)

* PHY devices are typically qualified for a large range of applications
  (industrial, medical, automotive...), and they provide a constant and
  reliable delay across temperature/pressure/voltage ranges
  (PHY 设备通常针对大范围的应用 (工业、医疗、汽车等) 通过了认证，并且它们在温度/压力/电压范围内提供恒定且可靠的延迟)

* PHY device drivers in PHYLIB being reusable by nature, being able to
  configure correctly a specified delay enables more designs with similar delay
  requirements to be operated correctly
  (PHYLIB 中的 PHY 设备驱动程序天生可复用，能够正确配置指定的延迟可以让更多具有相似延迟要求的设计正确工作)

For cases where the PHY is not capable of providing this delay, but the
Ethernet MAC driver is capable of doing so, the correct phy_interface_t value
should be PHY_INTERFACE_MODE_RGMII, and the Ethernet MAC driver should be
configured correctly in order to provide the required transmit and/or receive
side delay from the perspective of the PHY device. Conversely, if the Ethernet
MAC driver looks at the phy_interface_t value, for any other mode but
PHY_INTERFACE_MODE_RGMII, it should make sure that the MAC-level delays are
disabled.
(对于 PHY 无法提供此延迟但以太网 MAC 驱动程序能够做到的情况，正确的 phy_interface_t 值应为 PHY_INTERFACE_MODE_RGMII，并且应正确配置以太网 MAC 驱动程序，以从 PHY 设备的角度提供所需的发送和/或接收侧延迟。反之，如果以太网 MAC 驱动程序查看 phy_interface_t 值，那么对于除 PHY_INTERFACE_MODE_RGMII 以外的任何模式，它都应确保 MAC 级延迟被禁用。)

In case neither the Ethernet MAC, nor the PHY are capable of providing the
required delays, as defined per the RGMII standard, several options may be
available:
(如果以太网 MAC 和 PHY 都无法提供 RGMII 标准所定义的所需延迟，则可能有以下几种可选方案：)

* Some SoCs may offer a pin pad/mux/controller capable of configuring a given
  set of pins' strength, delays, and voltage; and it may be a suitable
  option to insert the expected 2ns RGMII delay.
  (一些 SoC 可能提供能够配置一组引脚的驱动强度、延迟和电压的引脚焊盘/复用/控制器；用它来插入预期的 2ns RGMII 延迟可能是一个合适的选择。)

* Modifying the PCB design to include a fixed delay (e.g: using a specifically
  designed serpentine), which may not require software configuration at all.
  (修改 PCB 设计以包含固定延迟 (例如使用专门设计的蛇形走线)，这可能完全不需要软件配置。)

Common problems with RGMII delay mismatch(RGMII 延迟不匹配的常见问题)
---------------------------------------------------------------------

When there is a RGMII delay mismatch between the Ethernet MAC and the PHY, this
will most likely result in the clock and data line signals to be unstable when
the PHY or MAC take a snapshot of these signals to translate them into logical
1 or 0 states and reconstruct the data being transmitted/received. Typical
symptoms include:
(当以太网 MAC 与 PHY 之间存在 RGMII 延迟不匹配时，当 PHY 或 MAC 对这些信号进行快照以将其转换为逻辑 1 或 0 状态并重建正在发送/接收的数据时，时钟线和数据线信号很可能不稳定。典型症状包括：)

* Transmission/reception partially works, and there is frequent or occasional
  packet loss observed
  (发送/接收部分正常工作，但观察到频繁或偶发的丢包)

* Ethernet MAC may report some or all packets ingressing with a FCS/CRC error,
  or just discard them all
  (以太网 MAC 可能报告部分或所有入站数据包存在 FCS/CRC 错误，或者直接将其全部丢弃)

* Switching to lower speeds such as 10/100Mbits/sec makes the problem go away
  (since there is enough setup/hold time in that case)
  (切换到较低的速率 (如 10/100Mbit/s) 会使问题消失 (因为在这种情况下有足够的建立/保持时间))

Connecting to a PHY(连接到 PHY)
===============================

Sometime during startup, the network driver needs to establish a connection
between the PHY device, and the network device.  At this time, the PHY's bus
and drivers need to all have been loaded, so it is ready for the connection.
At this point, there are several ways to connect to the PHY:
(在启动过程中的某个时刻，网络驱动程序需要在 PHY 设备与网络设备之间建立连接。此时，PHY 的总线和驱动程序都应当已经加载，从而准备好进行连接。此时，有几种连接到 PHY 的方式：)

#. The PAL handles everything, and only calls the network driver when
   the link state changes, so it can react.
   (PAL 处理所有事务，只在链路状态发生变化时才调用网络驱动程序，以便其作出反应。)

#. The PAL handles everything except interrupts (usually because the
   controller has the interrupt registers).
   (PAL 处理除中断之外的所有事务 (通常是因为控制器拥有中断寄存器)。)

#. The PAL handles everything, but checks in with the driver every second,
   allowing the network driver to react first to any changes before the PAL
   does.
   (PAL 处理所有事务，但每秒与驱动程序核对一次，让网络驱动程序能够先于 PAL 对任何变化作出反应。)

#. The PAL serves only as a library of functions, with the network device
   manually calling functions to update status, and configure the PHY
   (PAL 仅作为函数库使用，由网络设备手动调用函数来更新状态和配置 PHY)


Letting the PHY Abstraction Layer do Everything(让 PHY 抽象层处理一切)
======================================================================

If you choose option 1 (The hope is that every driver can, but to still be
useful to drivers that can't), connecting to the PHY is simple:
(如果选择方式 1 (希望每个驱动程序都能这样做，但为了对做不到的驱动程序仍然有用)，连接到 PHY 非常简单：)

First, you need a function to react to changes in the link state.  This
function follows this protocol::
(首先，你需要一个对链路状态变化作出反应的函数。此函数遵循以下原型：)

	static void adjust_link(struct net_device *dev);

Next, you need to know the device name of the PHY connected to this device.
The name will look something like, "0:00", where the first number is the
bus id, and the second is the PHY's address on that bus.  Typically,
the bus is responsible for making its ID unique.
(接下来，你需要知道连接到此设备的 PHY 的设备名称。该名称形如 "0:00"，第一个数字是总线 ID，第二个是 PHY 在该总线上的地址。通常，由总线负责确保其 ID 的唯一性。)

Now, to connect, just call this function::
(现在，要建立连接，只需调用此函数：)

	phydev = phy_connect(dev, phy_name, &adjust_link, interface);

*phydev* is a pointer to the phy_device structure which represents the PHY.
If phy_connect is successful, it will return the pointer.  dev, here, is the
pointer to your net_device.  Once done, this function will have started the
PHY's software state machine, and registered for the PHY's interrupt, if it
has one.  The phydev structure will be populated with information about the
current state, though the PHY will not yet be truly operational at this
point.
(*phydev* 是指向代表 PHY 的 phy_device 结构的指针。如果 phy_connect 成功，它将返回该指针。这里的 dev 是指向你的 net_device 的指针。完成后，此函数将已启动 PHY 的软件状态机，并注册了 PHY 的中断 (如果有的话)。phydev 结构中会填充有关当前状态的信息，不过此时 PHY 还不会真正开始工作。)

PHY-specific flags should be set in phydev->dev_flags prior to the call
to phy_connect() such that the underlying PHY driver can check for flags
and perform specific operations based on them.
This is useful if the system has put hardware restrictions on
the PHY/controller, of which the PHY needs to be aware.
(应在调用 phy_connect() 之前在 phydev->dev_flags 中设置 PHY 特定的标志，以便底层的 PHY 驱动程序能够检查这些标志并基于它们执行特定操作。如果系统对 PHY/控制器施加了硬件限制，而 PHY 需要知晓这些限制，那么这会很有用。)

*interface* is a u32 which specifies the connection type used
between the controller and the PHY.  Examples are GMII, MII,
RGMII, and SGMII.  See "PHY interface mode" below.  For a full
list, see include/linux/phy.h
(*interface* 是一个 u32，指定控制器与 PHY 之间使用的连接类型。例如 GMII、MII、RGMII 和 SGMII。参见下文 "PHY interface mode"。完整列表见 include/linux/phy.h)

Now just make sure that phydev->supported and phydev->advertising have any
values pruned from them which don't make sense for your controller (a 10/100
controller may be connected to a gigabit capable PHY, so you would need to
mask off SUPPORTED_1000baseT*).  See include/linux/ethtool.h for definitions
for these bitfields. Note that you should not SET any bits, except the
SUPPORTED_Pause and SUPPORTED_AsymPause bits (see below), or the PHY may get
put into an unsupported state.
(现在只需确保从 phydev->supported 和 phydev->advertising 中剪除对你的控制器没有意义的值 (10/100 控制器可能连接到支持千兆的 PHY，因此你需要屏蔽掉 SUPPORTED_1000baseT*)。这些位字段的定义见 include/linux/ethtool.h。注意，除 SUPPORTED_Pause 和 SUPPORTED_AsymPause 位 (见下文) 外，你不应设置 (SET) 任何位，否则 PHY 可能进入不受支持的状态。)

Lastly, once the controller is ready to handle network traffic, you call
phy_start(phydev).  This tells the PAL that you are ready, and configures the
PHY to connect to the network. If the MAC interrupt of your network driver
also handles PHY status changes, just set phydev->irq to PHY_MAC_INTERRUPT
before you call phy_start and use phy_mac_interrupt() from the network
driver. If you don't want to use interrupts, set phydev->irq to PHY_POLL.
phy_start() enables the PHY interrupts (if applicable) and starts the
phylib state machine.
(最后，一旦控制器准备好处理网络流量，就调用 phy_start(phydev)。这会告诉 PAL 你已就绪，并配置 PHY 连接到网络。如果你的网络驱动程序的 MAC 中断也处理 PHY 状态变化，只需在调用 phy_start 之前将 phydev->irq 设置为 PHY_MAC_INTERRUPT，并在网络驱动程序中使用 phy_mac_interrupt()。如果不想使用中断，则将 phydev->irq 设置为 PHY_POLL。phy_start() 会启用 PHY 中断 (如适用) 并启动 phylib 状态机。)

When you want to disconnect from the network (even if just briefly), you call
phy_stop(phydev). This function also stops the phylib state machine and
disables PHY interrupts.
(当你想与网络断开连接时 (即使只是短暂的)，调用 phy_stop(phydev)。此函数还会停止 phylib 状态机并禁用 PHY 中断。)

PHY interface modes(PHY 接口模式)
=================================

The PHY interface mode supplied in the phy_connect() family of functions
defines the initial operating mode of the PHY interface.  This is not
guaranteed to remain constant; there are PHYs which dynamically change
their interface mode without software interaction depending on the
negotiation results.
(phy_connect() 系列函数中提供的 PHY 接口模式定义了 PHY 接口的初始工作模式。这不保证保持不变；有些 PHY 会根据协商结果在没有软件干预的情况下动态更改其接口模式。)

Some of the interface modes are described below:
(下面描述了其中一些接口模式：)

``PHY_INTERFACE_MODE_SMII``
(SMII 模式)

    This is serial MII, clocked at 125MHz, supporting 100M and 10M speeds.
    Some details can be found in
    https://opencores.org/ocsvn/smii/smii/trunk/doc/SMII.pdf
    (这是串行 MII，以 125MHz 时钟运行，支持 100M 和 10M 速率。部分细节可在 https://opencores.org/ocsvn/smii/smii/trunk/doc/SMII.pdf 中找到)

``PHY_INTERFACE_MODE_1000BASEX``
(1000BASE-X 模式)

    This defines the 1000BASE-X single-lane serdes link as defined by the
    802.3 standard section 36.  The link operates at a fixed bit rate of
    1.25Gbaud using a 10B/8B encoding scheme, resulting in an underlying
    data rate of 1Gbps.  Embedded in the data stream is a 16-bit control
    word which is used to negotiate the duplex and pause modes with the
    remote end.  This does not include "up-clocked" variants such as 2.5Gbps
    speeds (see below.)
    (这定义了 802.3 标准第 36 章所定义的 1000BASE-X 单通道 serdes 链路。该链路以固定的 1.25Gbaud 位速率工作，采用 10B/8B 编码方案，从而产生 1Gbps 的底层数据速率。数据流中嵌入了一个 16 位控制字，用于与远端协商双工和暂停模式。这不包括 "提升时钟" 的变体，例如 2.5Gbps 速率 (见下文)。)

``PHY_INTERFACE_MODE_2500BASEX``
(2500BASE-X 模式)

    This defines a variant of 1000BASE-X which is clocked 2.5 times as fast
    as the 802.3 standard, giving a fixed bit rate of 3.125Gbaud.
    (这定义了 1000BASE-X 的一种变体，其时钟频率是 802.3 标准的 2.5 倍，固定的位速率为 3.125Gbaud。)

``PHY_INTERFACE_MODE_SGMII``
(SGMII 模式)

    This is used for Cisco SGMII, which is a modification of 1000BASE-X
    as defined by the 802.3 standard.  The SGMII link consists of a single
    serdes lane running at a fixed bit rate of 1.25Gbaud with 10B/8B
    encoding.  The underlying data rate is 1Gbps, with the slower speeds of
    100Mbps and 10Mbps being achieved through replication of each data symbol.
    The 802.3 control word is re-purposed to send the negotiated speed and
    duplex information from to the MAC, and for the MAC to acknowledge
    receipt.  This does not include "up-clocked" variants such as 2.5Gbps
    speeds.
    (这用于 Cisco SGMII，它是 802.3 标准所定义 1000BASE-X 的一种改进。SGMII 链路由单个 serdes 通道组成，以固定的 1.25Gbaud 位速率运行，采用 10B/8B 编码。底层数据速率为 1Gbps，较慢的 100Mbps 和 10Mbps 速率通过对每个数据符号进行复制来实现。802.3 控制字被重新用于从 PHY 向 MAC 发送协商的速率和双工信息，并供 MAC 确认收到。这不包括 "提升时钟" 的变体，例如 2.5Gbps 速率。)

    Note: mismatched SGMII vs 1000BASE-X configuration on a link can
    successfully pass data in some circumstances, but the 16-bit control
    word will not be correctly interpreted, which may cause mismatches in
    duplex, pause or other settings.  This is dependent on the MAC and/or
    PHY behaviour.
    (注意：链路上不匹配的 SGMII 与 1000BASE-X 配置在某些情况下可以成功传递数据，但 16 位控制字不会被正确解释，这可能导致双工、暂停或其他设置不匹配。这取决于 MAC 和/或 PHY 的行为。)

``PHY_INTERFACE_MODE_5GBASER``
(5GBASE-R 模式)

    This is the IEEE 802.3 Clause 129 defined 5GBASE-R protocol. It is
    identical to the 10GBASE-R protocol defined in Clause 49, with the
    exception that it operates at half the frequency. Please refer to the
    IEEE standard for the definition.
    (这是 IEEE 802.3 第 129 章定义的 5GBASE-R 协议。它与第 49 章定义的 10GBASE-R 协议相同，只是工作频率为其一半。定义请参阅 IEEE 标准。)

``PHY_INTERFACE_MODE_10GBASER``
(10GBASE-R 模式)

    This is the IEEE 802.3 Clause 49 defined 10GBASE-R protocol used with
    various different mediums. Please refer to the IEEE standard for a
    definition of this.
    (这是 IEEE 802.3 第 49 章定义的 10GBASE-R 协议，可与各种不同的介质一起使用。其定义请参阅 IEEE 标准。)

    Note: 10GBASE-R is just one protocol that can be used with XFI and SFI.
    XFI and SFI permit multiple protocols over a single SERDES lane, and
    also defines the electrical characteristics of the signals with a host
    compliance board plugged into the host XFP/SFP connector. Therefore,
    XFI and SFI are not PHY interface types in their own right.
    (注意：10GBASE-R 只是与 XFI 和 SFI 一起使用的协议之一。XFI 和 SFI 允许在单个 SERDES 通道上运行多种协议，并且还定义了在主机 XFP/SFP 连接器中插入主机一致性测试板时信号的电气特性。因此，XFI 和 SFI 本身并不是 PHY 接口类型。)

``PHY_INTERFACE_MODE_10GKR``
(10GBase-KR 模式)

    This is the IEEE 802.3 Clause 49 defined 10GBASE-R with Clause 73
    autonegotiation. Please refer to the IEEE standard for further
    information.
    (这是 IEEE 802.3 第 49 章定义的、带第 73 章自协商的 10GBASE-R。更多信息请参阅 IEEE 标准。)

    Note: due to legacy usage, some 10GBASE-R usage incorrectly makes
    use of this definition.
    (注意：由于历史遗留用法，一些 10GBASE-R 的使用错误地使用了这个定义。)

``PHY_INTERFACE_MODE_25GBASER``
(25GBASE-R 模式)

    This is the IEEE 802.3 PCS Clause 107 defined 25GBASE-R protocol.
    The PCS is identical to 10GBASE-R, i.e. 64B/66B encoded
    running 2.5 as fast, giving a fixed bit rate of 25.78125 Gbaud.
    Please refer to the IEEE standard for further information.
    (这是 IEEE 802.3 PCS 第 107 章定义的 25GBASE-R 协议。PCS 与 10GBASE-R 相同，即 64B/66B 编码，但运行速度为其 2.5 倍，固定的位速率为 25.78125 Gbaud。更多信息请参阅 IEEE 标准。)

``PHY_INTERFACE_MODE_100BASEX``
(100BASE-X 模式)

    This defines IEEE 802.3 Clause 24.  The link operates at a fixed data
    rate of 125Mpbs using a 4B/5B encoding scheme, resulting in an underlying
    data rate of 100Mpbs.
    (这定义了 IEEE 802.3 第 24 章。该链路以固定的 125Mpbs 数据速率工作，采用 4B/5B 编码方案，从而产生 100Mpbs 的底层数据速率。)

``PHY_INTERFACE_MODE_QUSGMII``
(QUSGMII 模式)

    This defines the Cisco the Quad USGMII mode, which is the Quad variant of
    the USGMII (Universal SGMII) link. It's very similar to QSGMII, but uses
    a Packet Control Header (PCH) instead of the 7 bytes preamble to carry not
    only the port id, but also so-called "extensions". The only documented
    extension so-far in the specification is the inclusion of timestamps, for
    PTP-enabled PHYs. This mode isn't compatible with QSGMII, but offers the
    same capabilities in terms of link speed and negotiation.
    (这定义了 Cisco 的 Quad USGMII 模式，它是 USGMII (通用 SGMII) 链路的四通道变体。它与 QSGMII 非常相似，但使用数据包控制头 (PCH) 代替 7 字节前导码，不仅承载端口 ID，还承载所谓的 "扩展"。规范中迄今为止唯一有文档记载的扩展是为支持 PTP 的 PHY 加入时间戳。此模式与 QSGMII 不兼容，但在链路速率和协商方面提供相同的能力。)

``PHY_INTERFACE_MODE_1000BASEKX``
(1000BASE-KX 模式)

    This is 1000BASE-X as defined by IEEE 802.3 Clause 36 with Clause 73
    autonegotiation. Generally, it will be used with a Clause 70 PMD. To
    contrast with the 1000BASE-X phy mode used for Clause 38 and 39 PMDs, this
    interface mode has different autonegotiation and only supports full duplex.
    (这是 IEEE 802.3 第 36 章定义的、带第 73 章自协商的 1000BASE-X。通常，它将与第 70 章 PMD 一起使用。与用于第 38 和 39 章 PMD 的 1000BASE-X phy 模式相比，此接口模式具有不同的自协商，并且仅支持全双工。)

``PHY_INTERFACE_MODE_PSGMII``
(PSGMII 模式)

    This is the Penta SGMII mode, it is similar to QSGMII but it combines 5
    SGMII lines into a single link compared to 4 on QSGMII.
    (这是五通道 SGMII 模式，它与 QSGMII 相似，但它将 5 条 SGMII 线合并为一条链路，而 QSGMII 是 4 条。)

``PHY_INTERFACE_MODE_10G_QXGMII``
(10G-QXGMII 模式)

    Represents the 10G-QXGMII PHY-MAC interface as defined by the Cisco USXGMII
    Multiport Copper Interface document. It supports 4 ports over a 10.3125 GHz
    SerDes lane, each port having speeds of 2.5G / 1G / 100M / 10M achieved
    through symbol replication. The PCS expects the standard USXGMII code word.
    (表示 Cisco USXGMII 多端口铜缆接口文档所定义的 10G-QXGMII PHY-MAC 接口。它在一条 10.3125 GHz 的 SerDes 通道上支持 4 个端口，每个端口通过符号复制实现 2.5G / 1G / 100M / 10M 的速率。PCS 期望标准的 USXGMII 码字。)

``PHY_INTERFACE_MODE_MIILITE``
(MII Lite 模式)

    Non-standard, simplified MII mode, without TXER, RXER, CRS and COL signals
    as defined for the MII. The absence of COL signal makes half-duplex link
    modes impossible but does not interfere with BroadR-Reach link modes on
    Broadcom (and other two-wire Ethernet) PHYs, because they are full-duplex
    only.
    (非标准的简化 MII 模式，没有 MII 所定义的 TXER、RXER、CRS 和 COL 信号。COL 信号的缺失使半双工链路模式无法实现，但这并不影响 Broadcom (及其他两线以太网) PHY 上的 BroadR-Reach 链路模式，因为它们仅支持全双工。)

Pause frames / flow control(暂停帧 / 流控制)
============================================

The PHY does not participate directly in flow control/pause frames except by
making sure that the SUPPORTED_Pause and SUPPORTED_AsymPause bits are set in
MII_ADVERTISE to indicate towards the link partner that the Ethernet MAC
controller supports such a thing. Since flow control/pause frames generation
involves the Ethernet MAC driver, it is recommended that this driver takes care
of properly indicating advertisement and support for such features by setting
the SUPPORTED_Pause and SUPPORTED_AsymPause bits accordingly. This can be done
either before or after phy_connect() and/or as a result of implementing the
ethtool::set_pauseparam feature.
(PHY 并不直接参与流控制/暂停帧，只是确保在 MII_ADVERTISE 中设置 SUPPORTED_Pause 和 SUPPORTED_AsymPause 位，以向链路伙伴表明以太网 MAC 控制器支持此类功能。由于流控制/暂停帧的生成涉及以太网 MAC 驱动程序，建议由该驱动程序负责通过相应地设置 SUPPORTED_Pause 和 SUPPORTED_AsymPause 位来正确通告对此类特性的支持和通告。这可以在 phy_connect() 之前或之后完成，和/或作为实现 ethtool::set_pauseparam 特性的结果来完成。)


Keeping Close Tabs on the PAL(密切关注 PAL)
===========================================

It is possible that the PAL's built-in state machine needs a little help to
keep your network device and the PHY properly in sync.  If so, you can
register a helper function when connecting to the PHY, which will be called
every second before the state machine reacts to any changes.  To do this, you
need to manually call phy_attach() and phy_prepare_link(), and then call
phy_start_machine() with the second argument set to point to your special
handler.
(有可能 PAL 内置的状态机需要一点帮助，才能让你的网络设备与 PHY 保持正确的同步。如果是这样，你可以在连接到 PHY 时注册一个辅助函数，该函数将在状态机对任何变化作出反应之前每秒被调用一次。为此，你需要手动调用 phy_attach() 和 phy_prepare_link()，然后调用 phy_start_machine()，并将第二个参数设置为指向你的特殊处理函数。)

Currently there are no examples of how to use this functionality, and testing
on it has been limited because the author does not have any drivers which use
it (they all use option 1).  So Caveat Emptor.
(目前没有关于如何使用此功能的示例，对它的测试也很有限，因为作者没有任何使用它的驱动程序 (它们都使用方式 1)。所以请使用者自担风险。)

Doing it all yourself(全部自己动手)
===================================

There's a remote chance that the PAL's built-in state machine cannot track
the complex interactions between the PHY and your network device.  If this is
so, you can simply call phy_attach(), and not call phy_start_machine or
phy_prepare_link().  This will mean that phydev->state is entirely yours to
handle (phy_start and phy_stop toggle between some of the states, so you
might need to avoid them).
(有一种很小的可能：PAL 内置的状态机无法跟踪 PHY 与你的网络设备之间的复杂交互。如果是这样，你可以只调用 phy_attach()，而不调用 phy_start_machine 或 phy_prepare_link()。这将意味着 phydev->state 完全由你来处理 (phy_start 和 phy_stop 会在某些状态之间切换，所以你可能需要避免使用它们)。)

An effort has been made to make sure that useful functionality can be
accessed without the state-machine running, and most of these functions are
descended from functions which did not interact with a complex state-machine.
However, again, no effort has been made so far to test running without the
state machine, so tryer beware.
(我们已经努力确保在不运行状态机的情况下也能访问有用的功能，这些函数中的大多数源自不与复杂状态机交互的函数。不过，同样地，迄今为止尚未付出任何努力来测试在没有状态机的情况下运行，所以尝试者须自担风险。)

Here is a brief rundown of the functions::
(以下是这些函数的简要说明：)

 int phy_read(struct phy_device *phydev, u16 regnum);
 int phy_write(struct phy_device *phydev, u16 regnum, u16 val);

Simple read/write primitives.  They invoke the bus's read/write function
pointers.
(简单的读/写原语。它们调用总线的读/写函数指针。)
::

 void phy_print_status(struct phy_device *phydev);

A convenience function to print out the PHY status neatly.
(一个方便的函数，用于整洁地打印 PHY 状态。)
::

 void phy_request_interrupt(struct phy_device *phydev);

Requests the IRQ for the PHY interrupts.
(为 PHY 中断请求 IRQ。)
::

 struct phy_device * phy_attach(struct net_device *dev, const char *phy_id,
		                phy_interface_t interface);

Attaches a network device to a particular PHY, binding the PHY to a generic
driver if none was found during bus initialization.
(将网络设备连接到特定的 PHY，如果总线初始化期间未找到驱动程序，则将 PHY 绑定到通用驱动程序。)
::

 int phy_start_aneg(struct phy_device *phydev);

Using variables inside the phydev structure, either configures advertising
and resets autonegotiation, or disables autonegotiation, and configures
forced settings.
(使用 phydev 结构内部的变量，要么配置通告并重置自协商，要么禁用自协商并配置强制设置。)
::

 static inline int phy_read_status(struct phy_device *phydev);

Fills the phydev structure with up-to-date information about the current
settings in the PHY.
(用 PHY 中当前设置的最新信息填充 phydev 结构。)
::

 int phy_ethtool_ksettings_set(struct phy_device *phydev,
                               const struct ethtool_link_ksettings *cmd);

Ethtool convenience functions.
(ethtool 便利函数。)
::

 int phy_mii_ioctl(struct phy_device *phydev,
                   struct mii_ioctl_data *mii_data, int cmd);

The MII ioctl.  Note that this function will completely screw up the state
machine if you write registers like BMCR, BMSR, ADVERTISE, etc.  Best to
use this only to write registers which are not standard, and don't set off
a renegotiation.
(MII ioctl。注意，如果你写入 BMCR、BMSR、ADVERTISE 等寄存器，此函数会完全搞乱状态机。最好仅用它写入非标准寄存器，并且不要触发重新协商。)

PHY Device Drivers(PHY 设备驱动程序)
====================================

With the PHY Abstraction Layer, adding support for new PHYs is
quite easy. In some cases, no work is required at all! However,
many PHYs require a little hand-holding to get up-and-running.
(有了 PHY 抽象层，为新的 PHY 添加支持相当容易。在某些情况下，完全不需要做任何工作！但是，许多 PHY 需要一些悉心照料才能启动并运行。)

Generic PHY driver(通用 PHY 驱动程序)
-------------------------------------

If the desired PHY doesn't have any errata, quirks, or special
features you want to support, then it may be best to not add
support, and let the PHY Abstraction Layer's Generic PHY Driver
do all of the work.
(如果所需的 PHY 没有任何勘误、怪癖或你想支持的特性，那么最好不要添加支持，而让 PHY 抽象层的通用 PHY 驱动程序完成所有工作。)

Writing a PHY driver(编写 PHY 驱动程序)
---------------------------------------

If you do need to write a PHY driver, the first thing to do is
make sure it can be matched with an appropriate PHY device.
This is done during bus initialization by reading the device's
UID (stored in registers 2 and 3), then comparing it to each
driver's phy_id field by ANDing it with each driver's
phy_id_mask field.  Also, it needs a name.  Here's an example::
(如果你确实需要编写 PHY 驱动程序，首先要做的是确保它能够与合适的 PHY 设备相匹配。这是在总线初始化期间完成的：读取设备的 UID (存储在寄存器 2 和 3 中)，然后将其与每个驱动程序的 phy_id 字段进行按位与 (AND) 运算并与每个驱动程序的 phy_id_mask 字段进行比较。此外，它还需要一个名称。下面是一个示例：)

   static struct phy_driver dm9161_driver = {
         .phy_id         = 0x0181b880,
	 .name           = "Davicom DM9161E",
	 .phy_id_mask    = 0x0ffffff0,
	 ...
   }

Next, you need to specify what features (speed, duplex, autoneg,
etc) your PHY device and driver support.  Most PHYs support
PHY_BASIC_FEATURES, but you can look in include/mii.h for other
features.
(接下来，你需要指定你的 PHY 设备和驱动程序支持哪些特性 (速率、双工、自协商等)。大多数 PHY 支持 PHY_BASIC_FEATURES，但你可以查看 include/mii.h 以了解其他特性。)

Each driver consists of a number of function pointers, documented
in include/linux/phy.h under the phy_driver structure.
(每个驱动程序由若干函数指针组成，这些函数指针在 include/linux/phy.h 中 phy_driver 结构下有文档说明。)

Of these, only config_aneg and read_status are required to be
assigned by the driver code.  The rest are optional.  Also, it is
preferred to use the generic phy driver's versions of these two
functions if at all possible: genphy_read_status and
genphy_config_aneg.  If this is not possible, it is likely that
you only need to perform some actions before and after invoking
these functions, and so your functions will wrap the generic
ones.
(其中，只有 config_aneg 和 read_status 是驱动程序代码必须赋值的。其余都是可选的。另外，如果可能的话，最好使用通用 phy 驱动程序提供的这两个函数的版本：genphy_read_status 和 genphy_config_aneg。如果做不到这一点，很可能你只需要在调用这些函数之前和之后执行一些操作，这样你的函数将包装通用函数。)

Feel free to look at the Marvell, Cicada, and Davicom drivers in
drivers/net/phy/ for examples (the lxt and qsemi drivers have
not been tested as of this writing).
(可以随意查看 drivers/net/phy/ 中的 Marvell、Cicada 和 Davicom 驱动程序作为示例 (撰写本文时，lxt 和 qsemi 驱动程序尚未经过测试)。)

The PHY's MMD register accesses are handled by the PAL framework
by default, but can be overridden by a specific PHY driver if
required. This could be the case if a PHY was released for
manufacturing before the MMD PHY register definitions were
standardized by the IEEE. Most modern PHYs will be able to use
the generic PAL framework for accessing the PHY's MMD registers.
An example of such usage is for Energy Efficient Ethernet support,
implemented in the PAL. This support uses the PAL to access MMD
registers for EEE query and configuration if the PHY supports
the IEEE standard access mechanisms, or can use the PHY's specific
access interfaces if overridden by the specific PHY driver. See
the Micrel driver in drivers/net/phy/ for an example of how this
can be implemented.
(默认情况下，PHY 的 MMD 寄存器访问由 PAL 框架处理，但如果需要，可以由特定的 PHY 驱动程序重写。如果某个 PHY 在 IEEE 将 MMD PHY 寄存器定义标准化之前就已投产发布，就可能出现这种情况。大多数现代 PHY 都能够使用通用 PAL 框架来访问 PHY 的 MMD 寄存器。这种用法的一个例子是在 PAL 中实现的节能以太网 (Energy Efficient Ethernet) 支持。如果 PHY 支持 IEEE 标准访问机制，该支持就使用 PAL 访问 MMD 寄存器以进行 EEE 查询和配置；如果被特定的 PHY 驱动程序重写，则可以使用该 PHY 特定的访问接口。有关如何实现这一点，请参阅 drivers/net/phy/ 中的 Micrel 驱动程序示例。)

Board Fixups(板级修正 (Board Fixups))
=====================================

Sometimes the specific interaction between the platform and the PHY requires
special handling.  For instance, to change where the PHY's clock input is,
or to add a delay to account for latency issues in the data path.  In order
to support such contingencies, the PHY Layer allows platform code to register
fixups to be run when the PHY is brought up (or subsequently reset).
(有时，平台与 PHY 之间的特定交互需要特殊处理。例如，改变 PHY 时钟输入的位置，或者增加延迟以解决数据路径中的延迟问题。为了支持这类突发情况，PHY 层允许平台代码注册修正 (fixup)，在 PHY 被启动 (或随后被复位) 时运行。)

When the PHY Layer brings up a PHY it checks to see if there are any fixups
registered for it, matching based on UID (contained in the PHY device's phy_id
field) and the bus identifier (contained in phydev->dev.bus_id).  Both must
match, however two constants, PHY_ANY_ID and PHY_ANY_UID, are provided as
wildcards for the bus ID and UID, respectively.
(当 PHY 层启动一个 PHY 时，它会检查是否有为它注册的修正，匹配基于 UID (包含在 PHY 设备的 phy_id 字段中) 和总线标识符 (包含在 phydev->dev.bus_id 中)。两者都必须匹配，不过提供了两个常量 PHY_ANY_ID 和 PHY_ANY_UID，分别作为总线 ID 和 UID 的通配符。)

When a match is found, the PHY layer will invoke the run function associated
with the fixup.  This function is passed a pointer to the phy_device of
interest.  It should therefore only operate on that PHY.
(找到匹配项后，PHY 层将调用与该修正关联的 run 函数。此函数会收到一个指向相关 phy_device 的指针。因此，它应当只对该 PHY 进行操作。)

The platform code can register the fixup using one of::
(平台代码可以使用以下函数之一注册修正：)

 int phy_register_fixup_for_uid(u32 phy_uid, u32 phy_uid_mask,
		int (*run)(struct phy_device *));
 int phy_register_fixup_for_id(const char *phy_id,
		int (*run)(struct phy_device *));

Standards(标准)
===============

IEEE Standard 802.3: CSMA/CD Access Method and Physical Layer Specifications, Section Two:
http://standards.ieee.org/getieee802/download/802.3-2008_section2.pdf
(IEEE 802.3 标准：CSMA/CD 访问方法与物理层规范，第二部分)

RGMII v1.3:
http://web.archive.org/web/20160303212629/http://www.hp.com/rnd/pdfs/RGMIIv1_3.pdf
(RGMII v1.3 规范)

RGMII v2.0:
http://web.archive.org/web/20160303171328/http://www.hp.com/rnd/pdfs/RGMIIv2_0_final_hp.pdf
(RGMII v2.0 规范)
