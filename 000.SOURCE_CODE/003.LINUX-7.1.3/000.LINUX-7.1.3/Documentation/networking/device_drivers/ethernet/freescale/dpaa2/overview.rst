.. include:: <isonum.txt>

=========================================================
DPAA2 (Data Path Acceleration Architecture Gen2) Overview
=========================================================

(DPAA2 (数据路径加速体系结构第二代) 概述)

:Copyright: |copy| 2015 Freescale Semiconductor Inc.
(|copy| 2015 Freescale Semiconductor Inc. 版权所有)
:Copyright: |copy| 2018 NXP
(|copy| 2018 NXP 版权所有)

This document provides an overview of the Freescale DPAA2 architecture
and how it is integrated into the Linux kernel.
(本文档概述 Freescale DPAA2 体系结构及其如何集成到 Linux 内核中。)

Introduction
(简介)
============

DPAA2 is a hardware architecture designed for high-speeed network
packet processing.  DPAA2 consists of sophisticated mechanisms for
processing Ethernet packets, queue management, buffer management,
autonomous L2 switching, virtual Ethernet bridging, and accelerator
(e.g. crypto) sharing.
(DPAA2 是一种为高速网络数据包处理而设计的硬件体系结构。DPAA2 由用于处理
以太网数据包、队列管理、缓冲区管理、自主 L2 交换、虚拟以太网桥接以及加速器
(如加密) 共享的完善机制组成。)

A DPAA2 hardware component called the Management Complex (or MC) manages the
DPAA2 hardware resources.  The MC provides an object-based abstraction for
software drivers to use the DPAA2 hardware.
The MC uses DPAA2 hardware resources such as queues, buffer pools, and
network ports to create functional objects/devices such as network
interfaces, an L2 switch, or accelerator instances.
The MC provides memory-mapped I/O command interfaces (MC portals)
which DPAA2 software drivers use to operate on DPAA2 objects.
(一个名为管理复合体 (Management Complex，简称 MC) 的 DPAA2 硬件组件负责管理
DPAA2 硬件资源。MC 为软件驱动提供基于对象的抽象，以便使用 DPAA2 硬件。
MC 使用队列、缓冲区池、网络端口等 DPAA2 硬件资源来创建功能性对象/设备，例如
网络接口、L2 交换机或加速器实例。
MC 提供内存映射 I/O 命令接口 (MC portal)，DPAA2 软件驱动通过这些接口对
DPAA2 对象进行操作。)

The diagram below shows an overview of the DPAA2 resource management
architecture::
(下图展示了 DPAA2 资源管理体系的概况：)

	+--------------------------------------+

	|                  OS                  |

	|                        DPAA2 drivers |

	|                             |        |

	+-----------------------------|--------+

	                              |

	                              | (create,discover,connect

	                              |  config,use,destroy)

	                              |

	                 DPAA2        |

	+------------------------| mc portal |-+

	|                             |        |

	|   +- - - - - - - - - - - - -V- - -+  |

	|   |                               |  |

	|   |   Management Complex (MC)     |  |

	|   |                               |  |

	|   +- - - - - - - - - - - - - - - -+  |

	|                                      |

	| Hardware                  Hardware   |

	| Resources                 Objects    |

	| ---------                 -------    |

	| -queues                   -DPRC      |

	| -buffer pools             -DPMCP     |

	| -Eth MACs/ports           -DPIO      |

	| -network interface        -DPNI      |

	|  profiles                 -DPMAC     |

	| -queue portals            -DPBP      |

	| -MC portals                ...       |

	|  ...                                 |

	|                                      |

	+--------------------------------------+


The MC mediates operations such as create, discover,
connect, configuration, and destroy.  Fast-path operations
on data, such as packet transmit/receive, are not mediated by
the MC and are done directly using memory mapped regions in
DPIO objects.
(MC 负责调解创建 (create)、发现 (discover)、连接 (connect)、配置
(configuration) 与销毁 (destroy) 等操作。数据上的快速路径操作，例如数据包的
发送/接收，不经 MC 调解，而是直接使用 DPIO 对象中的内存映射区域完成。)

Overview of DPAA2 Objects
(DPAA2 对象概述)
=========================

The section provides a brief overview of some key DPAA2 objects.
A simple scenario is described illustrating the objects involved
in creating a network interfaces.
(本节简要概述几个关键的 DPAA2 对象，并通过一个简单场景说明创建网络接口时所
涉及的对象。)

DPRC (Datapath Resource Container)
(DPRC，数据路径资源容器)
----------------------------------

A DPRC is a container object that holds all the other
types of DPAA2 objects.  In the example diagram below there
are 8 objects of 5 types (DPMCP, DPIO, DPBP, DPNI, and DPMAC)
in the container.
(DPRC 是一个容纳所有其他类型 DPAA2 对象的容器对象。在下面的示例图中，容器内
有 5 种类型共 8 个对象 (DPMCP、DPIO、DPBP、DPNI 和 DPMAC)。)

::

	+---------------------------------------------------------+
	| DPRC                                                    |
	|                                                         |
	|  +-------+  +-------+  +-------+  +-------+  +-------+  |
	|  | DPMCP |  | DPIO  |  | DPBP  |  | DPNI  |  | DPMAC |  |
	|  +-------+  +-------+  +-------+  +---+---+  +---+---+  |
	|  | DPMCP |  | DPIO  |                                   |
	|  +-------+  +-------+                                   |
	|  | DPMCP |                                              |
	|  +-------+                                              |
	|                                                         |
	+---------------------------------------------------------+

From the point of view of an OS, a DPRC behaves similar to a plug and
play bus, like PCI.  DPRC commands can be used to enumerate the contents
of the DPRC, discover the hardware objects present (including mappable
regions and interrupts).
(从操作系统角度看，DPRC 的行为类似于即插即用总线，例如 PCI。可以使用 DPRC
命令枚举 DPRC 的内容，发现其中存在的硬件对象 (包括可映射区域和中断)。)

::

	DPRC.1 (bus)
	   |
	   +--+--------+-------+-------+-------+
	      |        |       |       |       |
	    DPMCP.1  DPIO.1  DPBP.1  DPNI.1  DPMAC.1
	    DPMCP.2  DPIO.2
	    DPMCP.3

Hardware objects can be created and destroyed dynamically, providing
the ability to hot plug/unplug objects in and out of the DPRC.
(硬件对象可以动态创建和销毁，从而支持在 DPRC 中热插拔对象。)

A DPRC has a mappable MMIO region (an MC portal) that can be used
to send MC commands.  It has an interrupt for status events (like
hotplug).
All objects in a container share the same hardware "isolation context".
This means that with respect to an IOMMU the isolation granularity
is at the DPRC (container) level, not at the individual object
level.
(每个 DPRC 拥有一个可用于发送 MC 命令的可映射 MMIO 区域 (即一个 MC portal)。
它还有一个用于状态事件 (如热插拔) 的中断。
容器中的所有对象共享相同的硬件 "隔离上下文" (isolation context)。这意味着就
IOMMU 而言，隔离粒度是 DPRC (容器) 级别的，而不是单个对象级别的。)

DPRCs can be defined statically and populated with objects
via a config file passed to the MC when firmware starts it.
(DPRC 可以静态定义，并在固件启动 MC 时通过传递给 MC 的配置文件来填充对象。)

DPAA2 Objects for an Ethernet Network Interface
(以太网网络接口的 DPAA2 对象)
-----------------------------------------------

A typical Ethernet NIC is monolithic-- the NIC device contains TX/RX
queuing mechanisms, configuration mechanisms, buffer management,
physical ports, and interrupts.  DPAA2 uses a more granular approach
utilizing multiple hardware objects.  Each object provides specialized
functions. Groups of these objects are used by software to provide
Ethernet network interface functionality.  This approach provides
efficient use of finite hardware resources, flexibility, and
performance advantages.
(典型的以太网 NIC 是一体化的——NIC 设备包含 TX/RX 排队机制、配置机制、缓冲区
管理、物理端口和中断。DPAA2 采用更细粒度的方法，使用多个硬件对象，每个对象
提供专门的功能。软件将这些对象组合起来，以提供以太网网络接口功能。这种方法
能够高效利用有限的硬件资源，并带来灵活性与性能优势。)

The diagram below shows the objects needed for a simple
network interface configuration on a system with 2 CPUs.
(下图展示了在具有 2 个 CPU 的系统上配置简单网络接口所需的对象。)

::

	+---+---+ +---+---+
	   CPU0     CPU1
	+---+---+ +---+---+
	    |         |
	+---+---+ +---+---+
	   DPIO     DPIO
	+---+---+ +---+---+
	    \     /
	     \   /
	      \ /
	   +---+---+
	      DPNI  --- DPBP,DPMCP
	   +---+---+
	       |
	       |
	   +---+---+
	     DPMAC
	   +---+---+
	       |
	   port/PHY

Below the objects are described.  For each object a brief description
is provided along with a summary of the kinds of operations the object
supports and a summary of key resources of the object (MMIO regions
and IRQs).
(下面对这些对象逐一描述。每个对象都附有简要说明、该对象支持的操作类型摘要，
以及对象关键资源 (MMIO 区域与 IRQ) 的摘要。)

DPMAC (Datapath Ethernet MAC)
(DPMAC，数据路径以太网 MAC)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Represents an Ethernet MAC, a hardware device that connects to an Ethernet
PHY and allows physical transmission and reception of Ethernet frames.
(表示以太网 MAC，即连接到以太网 PHY、允许以物理方式发送与接收以太网帧的
硬件设备。)

- MMIO regions: none
  (MMIO 区域：无)
- IRQs: DPNI link change
  (IRQ：DPNI 链路变更)
- commands: set link up/down, link config, get stats,
  IRQ config, enable, reset
  (命令：设置链路 up/down、链路配置、获取统计信息、IRQ 配置、使能、复位)

DPNI (Datapath Network Interface)
(DPNI，数据路径网络接口)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Contains TX/RX queues, network interface configuration, and RX buffer pool
configuration mechanisms.  The TX/RX queues are in memory and are identified
by queue number.
(包含 TX/RX 队列、网络接口配置以及 RX 缓冲区池配置机制。TX/RX 队列位于内存
中，通过队列编号标识。)

- MMIO regions: none
  (MMIO 区域：无)
- IRQs: link state
  (IRQ：链路状态)
- commands: port config, offload config, queue config,
  parse/classify config, IRQ config, enable, reset
  (命令：端口配置、卸载配置、队列配置、解析/分类配置、IRQ 配置、使能、复位)

DPIO (Datapath I/O)
(DPIO，数据路径 I/O)
~~~~~~~~~~~~~~~~~~~

Provides interfaces to enqueue and dequeue
packets and do hardware buffer pool management operations.  The DPAA2
architecture separates the mechanism to access queues (the DPIO object)
from the queues themselves.  The DPIO provides an MMIO interface to
enqueue/dequeue packets.  To enqueue something a descriptor is written
to the DPIO MMIO region, which includes the target queue number.
There will typically be one DPIO assigned to each CPU.  This allows all
CPUs to simultaneously perform enqueue/dequeued operations.  DPIOs are
expected to be shared by different DPAA2 drivers.
(提供数据包入队与出队的接口，以及硬件缓冲区池管理操作。DPAA2 体系结构把
访问队列的机制 (即 DPIO 对象) 与队列本身分离。DPIO 提供用于数据包入队/出队
的 MMIO 接口。要执行入队，需要向 DPIO 的 MMIO 区域写入一个描述符，其中包含
目标队列编号。通常每个 CPU 分配一个 DPIO，这样所有 CPU 可以同时执行入队/出队
操作。DPIO 预期由不同的 DPAA2 驱动共享。)

- MMIO regions: queue operations, buffer management
  (MMIO 区域：队列操作、缓冲区管理)
- IRQs: data availability, congestion notification, buffer
  pool depletion
  (IRQ：数据可用、拥塞通知、缓冲区池耗尽)
- commands: IRQ config, enable, reset
  (命令：IRQ 配置、使能、复位)

DPBP (Datapath Buffer Pool)
(DPBP，数据路径缓冲区池)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Represents a hardware buffer pool.
(表示一个硬件缓冲区池。)

- MMIO regions: none
  (MMIO 区域：无)
- IRQs: none
  (IRQ：无)
- commands: enable, reset
  (命令：使能、复位)

DPMCP (Datapath MC Portal)
(DPMCP，数据路径 MC portal)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Provides an MC command portal.
Used by drivers to send commands to the MC to manage
objects.
(提供一个 MC 命令 portal。供驱动向 MC 发送命令以管理对象。)

- MMIO regions: MC command portal
  (MMIO 区域：MC 命令 portal)
- IRQs: command completion
  (IRQ：命令完成)
- commands: IRQ config, enable, reset
  (命令：IRQ 配置、使能、复位)

Object Connections
(对象连接)
==================

Some objects have explicit relationships that must
be configured:
(某些对象之间存在必须加以配置的显式关系：)

- DPNI <--> DPMAC
  (DPNI 与 DPMAC 相连)
- DPNI <--> DPNI
  (DPNI 与 DPNI 相连)
- DPNI <--> L2-switch-port
  (DPNI 与 L2 交换机端口相连)

    A DPNI must be connected to something such as a DPMAC,
    another DPNI, or L2 switch port.  The DPNI connection
    is made via a DPRC command.
    (DPNI 必须连接到某个对象，例如 DPMAC、另一个 DPNI 或 L2 交换机端口。
    DPNI 连接通过 DPRC 命令建立。)

::

              +-------+  +-------+
              | DPNI  |  | DPMAC |
              +---+---+  +---+---+
                  |          |
                  +==========+

- DPNI <--> DPBP
  (DPNI 与 DPBP 相连)

    A network interface requires a 'buffer pool' (DPBP
    object) which provides a list of pointers to memory
    where received Ethernet data is to be copied.  The
    Ethernet driver configures the DPBPs associated with
    the network interface.
    (网络接口需要一个 "缓冲区池" (DPBP 对象)，它提供一组指向内存的指针，接收
    到的以太网数据将被复制到这些内存中。以太网驱动配置与该网络接口关联的
    DPBP。)

Interrupts
(中断)
==========

All interrupts generated by DPAA2 objects are message
interrupts.  At the hardware level message interrupts
generated by devices will normally have 3 components--
1) a non-spoofable 'device-id' expressed on the hardware
bus, 2) an address, 3) a data value.
(DPAA2 对象产生的所有中断都是消息中断。在硬件层面，设备产生的消息中断通常
有 3 个组成部分——1) 通过硬件总线表达的不可伪造的 "device-id"；2) 一个地址；
3) 一个数据值。)

In the case of DPAA2 devices/objects, all objects in the
same container/DPRC share the same 'device-id'.
For ARM-based SoC this is the same as the stream ID.
(对 DPAA2 设备/对象而言，同一容器/DPRC 中的所有对象共享相同的
"device-id"。对于基于 ARM 的 SoC，它与 stream ID 相同。)


DPAA2 Linux Drivers Overview
(DPAA2 Linux 驱动概述)
============================

This section provides an overview of the Linux kernel drivers for
DPAA2-- 1) the bus driver and associated "DPAA2 infrastructure"
drivers and 2) functional object drivers (such as Ethernet).
(本节概述 DPAA2 的 Linux 内核驱动——1) 总线驱动及相关的 "DPAA2 基础设施"
驱动；2) 功能对象驱动 (如以太网)。)

As described previously, a DPRC is a container that holds the other
types of DPAA2 objects.  It is functionally similar to a plug-and-play
bus controller.
Each object in the DPRC is a Linux "device" and is bound to a driver.
The diagram below shows the Linux drivers involved in a networking
scenario and the objects bound to each driver.  A brief description
of each driver follows.
(如前所述，DPRC 是一个容纳其他类型 DPAA2 对象的容器。它在功能上类似于
即插即用总线控制器。
DPRC 中的每个对象都是一个 Linux "设备"，并绑定到一个驱动。下图展示了网络
场景中涉及的 Linux 驱动以及绑定到每个驱动的对象。随后是对每个驱动的简要
说明。)

::

	                                     +------------+
	                                     | OS Network |
	                                     |   Stack    |
	         +------------+              +------------+
	         | Allocator  |. . . . . . . |  Ethernet  |
	         |(DPMCP,DPBP)|              |   (DPNI)   |
	         +-.----------+              +---+---+----+
	          .          .                   ^   |
	         .            .     <data avail, |   | <enqueue,
	        .              .     tx confirm> |   | dequeue>
	+-------------+         .                |   |
	| DPRC driver |          .           +---+---V----+     +---------+
	|   (DPRC)    |           . . . . . .| DPIO driver|     |   MAC   |
	+----------+--+                      |  (DPIO)    |     | (DPMAC) |
	           |                         +------+-----+     +-----+---+
	           |<dev add/remove>                |                 |
	           |                                |                 |
	  +--------+----------+                     |              +--+---+
	  |   MC-bus driver   |                     |              | PHY  |
	  |                   |                     |              |driver|
	  |   /bus/fsl-mc     |                     |              +--+---+
	  +-------------------+                     |                 |
	                                            |                 |
	========================= HARDWARE =========|=================|======
	                                          DPIO                |
	                                            |                 |
	                                          DPNI---DPBP         |
	                                            |                 |
	                                          DPMAC               |
	                                            |                 |
	                                           PHY ---------------+
	============================================|========================

A brief description of each driver is provided below.
(下面对每个驱动做简要说明。)

MC-bus driver
(MC 总线驱动)
-------------

The MC-bus driver is a platform driver and is probed from a
node in the device tree (compatible "fsl,qoriq-mc") passed in by boot
firmware.  It is responsible for bootstrapping the DPAA2 kernel
infrastructure.
Key functions include:
(MC 总线驱动是一个平台驱动，由引导固件传入的设备树节点 (compatible
"fsl,qoriq-mc") 探测得到。它负责引导 DPAA2 内核基础设施的启动。
其关键功能包括：)

- registering a new bus type named "fsl-mc" with the kernel,
  and implementing bus call-backs (e.g. match/uevent/dev_groups)
  (向内核注册名为 "fsl-mc" 的新总线类型，并实现总线回调
  (如 match/uevent/dev_groups))
- implementing APIs for DPAA2 driver registration and for device
  add/remove
  (实现用于 DPAA2 驱动注册以及设备添加/移除的 API)
- creates an MSI IRQ domain
  (创建 MSI IRQ 域)
- doing a 'device add' to expose the 'root' DPRC, in turn triggering
  a bind of the root DPRC to the DPRC driver
  (执行 "device add" 以暴露 "根" DPRC，进而触发根 DPRC 与 DPRC 驱动的绑定)

The binding for the MC-bus device-tree node can be consulted at
*Documentation/devicetree/bindings/misc/fsl,qoriq-mc.yaml*.
The sysfs bind/unbind interfaces for the MC-bus can be consulted at
*Documentation/ABI/testing/sysfs-bus-fsl-mc*.
(MC 总线设备树节点的绑定文档可参阅
*Documentation/devicetree/bindings/misc/fsl,qoriq-mc.yaml*。
MC 总线的 sysfs bind/unbind 接口可参阅
*Documentation/ABI/testing/sysfs-bus-fsl-mc*。)

DPRC driver
(DPRC 驱动)
-----------

The DPRC driver is bound to DPRC objects and does runtime management
of a bus instance.  It performs the initial bus scan of the DPRC
and handles interrupts for container events such as hot plug by
re-scanning the DPRC.
(DPRC 驱动绑定到 DPRC 对象，负责总线实例的运行时管理。它对 DPRC 执行初始
总线扫描，并通过重新扫描 DPRC 来处理热插拔等容器事件的中断。)

Allocator
(分配器)
---------

Certain objects such as DPMCP and DPBP are generic and fungible,
and are intended to be used by other drivers.  For example,
the DPAA2 Ethernet driver needs:
(某些对象 (如 DPMCP 和 DPBP) 是通用且可互换的，供其他驱动使用。例如，DPAA2
以太网驱动需要：)

- DPMCPs to send MC commands, to configure network interfaces
  (DPMCP：用于发送 MC 命令，以配置网络接口)
- DPBPs for network buffer pools
  (DPBP：用作网络缓冲区池)

The allocator driver registers for these allocatable object types
and those objects are bound to the allocator when the bus is probed.
The allocator maintains a pool of objects that are available for
allocation by other DPAA2 drivers.
(分配器驱动注册这些可分配的对象类型，总线被探测时这些对象便绑定到分配器。
分配器维护一个对象池，供其他 DPAA2 驱动分配使用。)

DPIO driver
(DPIO 驱动)
-----------

The DPIO driver is bound to DPIO objects and provides services that allow
other drivers such as the Ethernet driver to enqueue and dequeue data for
their respective objects.
Key services include:
(DPIO 驱动绑定到 DPIO 对象，提供服务让以太网驱动等其他驱动能够为其各自的
对象执行数据入队与出队。
关键服务包括：)

- data availability notifications
  (数据可用通知)
- hardware queuing operations (enqueue and dequeue of data)
  (硬件排队操作 (数据入队与出队))
- hardware buffer pool management
  (硬件缓冲区池管理)

To transmit a packet the Ethernet driver puts data on a queue and
invokes a DPIO API.  For receive, the Ethernet driver registers
a data availability notification callback.  To dequeue a packet
a DPIO API is used.
There is typically one DPIO object per physical CPU for optimum
performance, allowing different CPUs to simultaneously enqueue
and dequeue data.
(发送数据包时，以太网驱动把数据放入队列并调用一个 DPIO API。接收时，以太网
驱动注册一个数据可用通知回调。出队数据包则使用 DPIO API。
通常每个物理 CPU 有一个 DPIO 对象以获得最佳性能，使不同 CPU 能够同时执行
入队与出队操作。)

The DPIO driver operates on behalf of all DPAA2 drivers
active in the kernel--  Ethernet, crypto, compression,
etc.
(DPIO 驱动代表内核中所有处于活动状态的 DPAA2 驱动工作——以太网、加密、压缩
等。)

Ethernet driver
(以太网驱动)
---------------

The Ethernet driver is bound to a DPNI and implements the kernel
interfaces needed to connect the DPAA2 network interface to
the network stack.
Each DPNI corresponds to a Linux network interface.
(以太网驱动绑定到一个 DPNI，实现将 DPAA2 网络接口连接到网络协议栈所需的
内核接口。
每个 DPNI 对应一个 Linux 网络接口。)

MAC driver
(MAC 驱动)
----------

An Ethernet PHY is an off-chip, board specific component and is managed
by the appropriate PHY driver via an mdio bus.  The MAC driver
plays a role of being a proxy between the PHY driver and the
MC.  It does this proxy via the MC commands to a DPMAC object.
If the PHY driver signals a link change, the MAC driver notifies
the MC via a DPMAC command.  If a network interface is brought
up or down, the MC notifies the DPMAC driver via an interrupt and
the driver can take appropriate action.
(以太网 PHY 是片外、板级特定的组件，由相应的 PHY 驱动通过 mdio 总线管理。
MAC 驱动充当 PHY 驱动与 MC 之间的代理，它通过向 DPMAC 对象发送 MC 命令来
实现这一代理。如果 PHY 驱动发出链路变更信号，MAC 驱动会通过 DPMAC 命令通知
MC。如果网络接口被启用或关闭，MC 会通过中断通知 DPMAC 驱动，驱动即可采取
相应措施。)
