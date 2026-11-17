=============================================
Broadcom Starfighter 2 Ethernet switch driver
=============================================

(博通 Starfighter 2 以太网交换机驱动)

Broadcom's Starfighter 2 Ethernet switch hardware block is commonly found and
deployed in the following products:
(博通的 Starfighter 2 以太网交换机硬件块常见于并部署在以下产品中：)

- xDSL gateways such as BCM63138
  (诸如 BCM63138 之类的 xDSL 网关)
- streaming/multimedia Set Top Box such as BCM7445
  (诸如 BCM7445 之类的流媒体/多媒体机顶盒)
- Cable Modem/residential gateways such as BCM7145/BCM3390
  (诸如 BCM7145/BCM3390 之类的电缆调制解调器/住宅网关)

The switch is typically deployed in a configuration involving between 5 to 13
ports, offering a range of built-in and customizable interfaces:
(该交换机通常部署在包含 5 到 13 个端口的配置中，提供一系列内置和可定制
的接口：)

- single integrated Gigabit PHY
  (单个集成千兆 PHY)
- quad integrated Gigabit PHY
  (四个集成千兆 PHY)
- quad external Gigabit PHY w/ MDIO multiplexer
  (带 MDIO 复用器的四个外部千兆 PHY)
- integrated MoCA PHY
  (集成 MoCA PHY)
- several external MII/RevMII/GMII/RGMII interfaces
  (若干外部 MII/RevMII/GMII/RGMII 接口)

The switch also supports specific congestion control features which allow MoCA
fail-over not to lose packets during a MoCA role re-election, as well as out of
band back-pressure to the host CPU network interface when downstream interfaces
are connected at a lower speed.
(该交换机还支持特定的拥塞控制特性，使 MoCA 故障转移在 MoCA 角色重新选举
期间不丢包，并在下游接口以较低速率连接时向主机 CPU 网络接口提供带外
背压。)

The switch hardware block is typically interfaced using MMIO accesses and
contains a bunch of sub-blocks/registers:
(该交换机硬件块通常通过 MMIO 访问来交互，包含许多子块/寄存器：)

- ``SWITCH_CORE``: common switch registers
  (``SWITCH_CORE``： 通用交换机寄存器)
- ``SWITCH_REG``: external interfaces switch register
  (``SWITCH_REG``： 外部接口交换寄存器)
- ``SWITCH_MDIO``: external MDIO bus controller (there is another one in SWITCH_CORE,
  which is used for indirect PHY accesses)
  (``SWITCH_MDIO``： 外部 MDIO 总线控制器 (SWITCH_CORE 中还有另一个，
  用于间接 PHY 访问))
- ``SWITCH_INDIR_RW``: 64-bits wide register helper block
  (``SWITCH_INDIR_RW``： 64 位宽寄存器辅助块)
- ``SWITCH_INTRL2_0/1``: Level-2 interrupt controllers
  (``SWITCH_INTRL2_0/1``： 二级中断控制器)
- ``SWITCH_ACB``: Admission control block
  (``SWITCH_ACB``： 准入控制块)
- ``SWITCH_FCB``: Fail-over control block
  (``SWITCH_FCB``： 故障转移控制块)

Implementation details
(实现细节)
======================

The driver is located in ``drivers/net/dsa/bcm_sf2.c`` and is implemented as a DSA
driver; see ``Documentation/networking/dsa/dsa.rst`` for details on the subsystem
and what it provides.
(驱动位于 ``drivers/net/dsa/bcm_sf2.c``，并实现为一个 DSA 驱动；有关该
子系统及其提供功能的详情，参见 ``Documentation/networking/dsa/dsa.rst``。)

The SF2 switch is configured to enable a Broadcom specific 4-bytes switch tag
which gets inserted by the switch for every packet forwarded to the CPU
interface, conversely, the CPU network interface should insert a similar tag for
packets entering the CPU port. The tag format is described in
``net/dsa/tag_brcm.c``.
(SF2 交换机被配置为启用 Broadcom 特有的 4 字节交换机标签，交换机会为每个
转发到 CPU 接口的数据包插入该标签；相反，CPU 网络接口也应为进入 CPU
端口的数据包插入类似的标签。标签格式在 ``net/dsa/tag_brcm.c`` 中描述。)

Overall, the SF2 driver is a fairly regular DSA driver; there are a few
specifics covered below.
(总体而言，SF2 驱动是一个相当常规的 DSA 驱动；下面介绍其中的几个特有
之处。)

Device Tree probing
(设备树探测)
-------------------

The DSA platform device driver is probed using a specific compatible string
provided in ``net/dsa/dsa.c``. The reason for that is because the DSA subsystem gets
registered as a platform device driver currently. DSA will provide the needed
device_node pointers which are then accessible by the switch driver setup
function to setup resources such as register ranges and interrupts. This
currently works very well because none of the of_* functions utilized by the
driver require a struct device to be bound to a struct device_node, but things
may change in the future.
(DSA 平台设备驱动使用 ``net/dsa/dsa.c`` 中提供的特定 compatible 字符串
进行探测。原因在于 DSA 子系统目前被注册为平台设备驱动。DSA 会提供所需的
device_node 指针，交换机驱动的 setup 函数随后可以访问这些指针，以设置
寄存器范围和中断等资源。目前这种方式工作得很好，因为驱动所使用的 of_*
函数都不要求 struct device 绑定到 struct device_node，但将来情况可能会
改变。)

MDIO indirect accesses
(MDIO 间接访问)
----------------------

Due to a limitation in how Broadcom switches have been designed, external
Broadcom switches connected to a SF2 require the use of the DSA user MDIO bus
in order to properly configure them. By default, the SF2 pseudo-PHY address, and
an external switch pseudo-PHY address will both be snooping for incoming MDIO
transactions, since they are at the same address (30), resulting in some kind of
"double" programming. Using DSA, and setting ``ds->phys_mii_mask`` accordingly, we
selectively divert reads and writes towards external Broadcom switches
pseudo-PHY addresses. Newer revisions of the SF2 hardware have introduced a
configurable pseudo-PHY address which circumvents the initial design limitation.
(由于 Broadcom 交换机设计方式的限制，连接到 SF2 的外部 Broadcom 交换机
需要使用 DSA user MDIO 总线才能正确配置它们。默认情况下，SF2 伪 PHY 地址
和外部交换机伪 PHY 地址都会侦听传入的 MDIO 事务，因为它们处于相同的地址
(30)，从而导致某种 "双重" 编程。通过使用 DSA 并相应地设置
``ds->phys_mii_mask``，我们可以有选择地把读写导向外部 Broadcom 交换机的
伪 PHY 地址。较新版本的 SF2 硬件引入了可配置的伪 PHY 地址，从而规避了
最初的设计限制。)

Multimedia over CoAxial (MoCA) interfaces
(同轴多媒体 (MoCA) 接口)
-----------------------------------------

MoCA interfaces are fairly specific and require the use of a firmware blob which
gets loaded onto the MoCA processor(s) for packet processing. The switch
hardware contains logic which will assert/de-assert link states accordingly for
the MoCA interface whenever the MoCA coaxial cable gets disconnected or the
firmware gets reloaded. The SF2 driver relies on such events to properly set its
MoCA interface carrier state and properly report this to the networking stack.
(MoCA 接口相当特殊，需要使用固件 blob，固件会被加载到 MoCA 处理器上用于
数据包处理。交换机硬件包含相应的逻辑，每当 MoCA 同轴电缆断开或固件被重新
加载时，它都会相应地置位/清除 MoCA 接口的链路状态。SF2 驱动依赖这类事件
来正确设置其 MoCA 接口的载波状态，并把它正确报告给网络协议栈。)

The MoCA interfaces are supported using the PHY library's fixed PHY/emulated PHY
device and the switch driver registers a ``fixed_link_update`` callback for such
PHYs which reflects the link state obtained from the interrupt handler.
(MoCA 接口通过 PHY 库的固定 PHY/模拟 PHY 设备来支持，交换机驱动为此类
PHY 注册一个 ``fixed_link_update`` 回调，该回调反映从中断处理程序获得的
链路状态。)


Power Management
(电源管理)
----------------

Whenever possible, the SF2 driver tries to minimize the overall switch power
consumption by applying a combination of:
(只要可能，SF2 驱动都会尝试通过组合应用以下措施来最小化交换机的整体
功耗：)

- turning off internal buffers/memories
  (关闭内部缓冲区/存储器)
- disabling packet processing logic
  (禁用数据包处理逻辑)
- putting integrated PHYs in IDDQ/low-power
  (把集成 PHY 置于 IDDQ/低功耗状态)
- reducing the switch core clock based on the active port count
  (根据活动端口数降低交换机核心时钟)
- enabling and advertising EEE
  (启用并通告 EEE)
- turning off RGMII data processing logic when the link goes down
  (在链路断开时关闭 RGMII 数据处理逻辑)

Wake-on-LAN
(网络唤醒)
-----------

Wake-on-LAN is currently implemented by utilizing the host processor Ethernet
MAC controller wake-on logic. Whenever Wake-on-LAN is requested, an intersection
between the user request and the supported host Ethernet interface WoL
capabilities is done and the intersection result gets configured. During
system-wide suspend/resume, only ports not participating in Wake-on-LAN are
disabled.
(Wake-on-LAN 目前通过利用主机处理器以太网 MAC 控制器的唤醒逻辑来实现。
每当请求 Wake-on-LAN 时，会在用户请求与所支持的主机以太网接口 WoL 能力
之间取交集，并对交集结果进行配置。在系统级挂起/恢复期间，只有未参与
Wake-on-LAN 的端口会被禁用。)
