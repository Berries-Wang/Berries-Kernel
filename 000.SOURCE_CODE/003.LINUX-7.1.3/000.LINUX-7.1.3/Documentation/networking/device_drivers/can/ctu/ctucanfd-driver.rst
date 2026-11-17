.. SPDX-License-Identifier: GPL-2.0-or-later

CTU CAN FD Driver
=================

(CTU CAN FD 驱动)

Author: Martin Jerabek <martin.jerabek01@gmail.com>
(作者: Martin Jerabek <martin.jerabek01@gmail.com>)


About CTU CAN FD IP Core
------------------------

(关于 CTU CAN FD IP 核)

`CTU CAN FD <https://gitlab.fel.cvut.cz/canbus/ctucanfd_ip_core>`_
is an open source soft core written in VHDL.
It originated in 2015 as Ondrej Ille's project
at the `Department of Measurement <https://meas.fel.cvut.cz/>`_
of `FEE <http://www.fel.cvut.cz/en/>`_ at `CTU <https://www.cvut.cz/en>`_.
(CTU CAN FD 是一个用 VHDL 编写的开源软核。它起源于 2015 年 Ondrej Ille 在
布拉格捷克理工大学 (CTU) 电气工程学院 (FEE) 测量系开展的项目。)

The SocketCAN driver for Xilinx Zynq SoC based MicroZed board
`Vivado integration <https://gitlab.fel.cvut.cz/canbus/zynq/zynq-can-sja1000-top>`_
and Intel Cyclone V 5CSEMA4U23C6 based DE0-Nano-SoC Terasic board
`QSys integration <https://gitlab.fel.cvut.cz/canbus/intel-soc-ctucanfd>`_
has been developed as well as support for
`PCIe integration <https://gitlab.fel.cvut.cz/canbus/pcie-ctucanfd>`_ of the core.
(针对基于 Xilinx Zynq SoC 的 MicroZed 板卡的 Vivado 集成、基于 Intel
Cyclone V 5CSEMA4U23C6 的 DE0-Nano-SoC Terasic 板卡的 QSys 集成的
SocketCAN 驱动均已开发完成，此外还开发了对该核的 PCIe 集成的支持。)

In the case of Zynq, the core is connected via the APB system bus, which does
not have enumeration support, and the device must be specified in Device Tree.
This kind of devices is called platform device in the kernel and is
handled by a platform device driver.
(就 Zynq 而言，该核通过 APB 系统总线连接，该总线不支持枚举，因此设备必须
在设备树 (Device Tree) 中指定。这类设备在内核中称为平台设备 (platform
device)，由平台设备驱动处理。)

The basic functional model of the CTU CAN FD peripheral has been
accepted into QEMU mainline. See QEMU `CAN emulation support <https://www.qemu.org/docs/master/system/devices/can.html>`_
for CAN FD buses, host connection and CTU CAN FD core emulation. The development
version of emulation support can be cloned from ctu-canfd branch of QEMU local
development `repository <https://gitlab.fel.cvut.cz/canbus/qemu-canbus>`_.
(CTU CAN FD 外设的基本功能模型已被接受进入 QEMU 主线。关于 CAN FD 总线、
主机连接以及 CTU CAN FD 核的仿真，参见 QEMU 的 CAN 仿真支持文档。仿真
支持的开发版可从 QEMU 本地开发仓库的 ctu-canfd 分支克隆。)


About SocketCAN
---------------

(关于 SocketCAN)

SocketCAN is a standard common interface for CAN devices in the Linux
kernel. As the name suggests, the bus is accessed via sockets, similarly
to common network devices. The reasoning behind this is in depth
described in `Linux SocketCAN <https://www.kernel.org/doc/html/latest/networking/can.html>`_.
In short, it offers a
natural way to implement and work with higher layer protocols over CAN,
in the same way as, e.g., UDP/IP over Ethernet.
(SocketCAN 是 Linux 内核中 CAN 设备的标准通用接口。顾名思义，总线通过
套接字访问，与常见网络设备类似。其背后的设计缘由在 Linux SocketCAN 文档
中有深入阐述。简而言之，它提供了一种在 CAN 之上实现和使用更高层协议的
自然方式，就像以太网之上的 UDP/IP 一样。)

Device probe
~~~~~~~~~~~~

(设备探测)

Before going into detail about the structure of a CAN bus device driver,
let's reiterate how the kernel gets to know about the device at all.
Some buses, like PCI or PCIe, support device enumeration. That is, when
the system boots, it discovers all the devices on the bus and reads
their configuration. The kernel identifies the device via its vendor ID
and device ID, and if there is a driver registered for this identifier
combination, its probe method is invoked to populate the driver's
instance for the given hardware. A similar situation goes with USB, only
it allows for device hot-plug.
(在深入讨论 CAN 总线设备驱动的结构之前，先重申一下内核究竟是如何得知
设备的。某些总线(如 PCI 或 PCIe)支持设备枚举，即系统启动时，会发现总线
上的所有设备并读取其配置。内核通过厂商 ID 和设备 ID 来识别设备；如果
针对这一标识符组合注册了驱动，就会调用其 probe 方法为给定硬件填充驱动
实例。USB 的情况类似，只是它还支持设备热插拔。)

The situation is different for peripherals which are directly embedded
in the SoC and connected to an internal system bus (AXI, APB, Avalon,
and others). These buses do not support enumeration, and thus the kernel
has to learn about the devices from elsewhere. This is exactly what the
Device Tree was made for.
(对于直接嵌入 SoC 并连接到内部系统总线(AXI、APB、Avalon 等)的外设，
情况则不同。这些总线不支持枚举，因此内核必须从别处获知设备的存在。设备树
(Device Tree) 正是为此而生。)

Device tree
~~~~~~~~~~~

(设备树)

An entry in device tree states that a device exists in the system, how
it is reachable (on which bus it resides) and its configuration –
registers address, interrupts and so on. An example of such a device
tree is given in .
(设备树中的一个条目声明了系统中存在某设备、如何访问它(它位于哪条总线
上)以及它的配置——寄存器地址、中断等。此类设备树的一个示例见 。)

::

           / {
               /* ... */
               amba: amba {
                   #address-cells = <1>;
                   #size-cells = <1>;
                   compatible = "simple-bus";

                   CTU_CAN_FD_0: CTU_CAN_FD@43c30000 {
                       compatible = "ctu,ctucanfd";
                       interrupt-parent = <&intc>;
                       interrupts = <0 30 4>;
                       clocks = <&clkc 15>;
                       reg = <0x43c30000 0x10000>;
                   };
               };
           };


.. _sec:socketcan:drv:

Driver structure
~~~~~~~~~~~~~~~~

(驱动结构)

The driver can be divided into two parts – platform-dependent device
discovery and set up, and platform-independent CAN network device
implementation.
(驱动可以分为两部分——平台相关的设备发现与建立，以及平台无关的 CAN
网络设备实现。)

.. _sec:socketcan:platdev:

Platform device driver
^^^^^^^^^^^^^^^^^^^^^^

(平台设备驱动)

In the case of Zynq, the core is connected via the AXI system bus, which
does not have enumeration support, and the device must be specified in
Device Tree. This kind of devices is called *platform device* in the
kernel and is handled by a *platform device driver*\  [1]_.
(就 Zynq 而言，该核通过 AXI 系统总线连接，该总线不支持枚举，因此设备必须
在设备树中指定。这类设备在内核中称为 platform device(平台设备)，由
platform device driver(平台设备驱动)处理 [1]_。)

A platform device driver provides the following things:
(平台设备驱动提供以下内容：)

-  A *probe* function
   (一个 probe(探测)函数)

-  A *remove* function
   (一个 remove(移除)函数)

-  A table of *compatible* devices that the driver can handle
   (一张驱动所能处理的 compatible(兼容)设备表)

The *probe* function is called exactly once when the device appears (or
the driver is loaded, whichever happens later). If there are more
devices handled by the same driver, the *probe* function is called for
each one of them. Its role is to allocate and initialize resources
required for handling the device, as well as set up low-level functions
for the platform-independent layer, e.g., *read_reg* and *write_reg*.
After that, the driver registers the device to a higher layer, in our
case as a *network device*.
(probe 函数在设备出现时(或驱动被加载时，以较晚发生者为准)恰好被调用
一次。如果同一驱动处理多个设备，则会对每个设备各调用一次 probe 函数。
它的作用是分配并初始化处理该设备所需的资源，同时为平台无关层建立低层
函数，例如 read_reg 和 write_reg。此后，驱动把设备注册到更高层——在我们的
场景中，注册为网络设备 (network device)。)

The *remove* function is called when the device disappears, or the
driver is about to be unloaded. It serves to free the resources
allocated in *probe* and to unregister the device from higher layers.
(remove 函数在设备消失或驱动即将被卸载时调用。它用于释放在 probe 中
分配的资源，并把设备从更高层注销。)

Finally, the table of *compatible* devices states which devices the
driver can handle. The Device Tree entry ``compatible`` is matched
against the tables of all *platform drivers*.
(最后，compatible 设备表声明了驱动能处理哪些设备。设备树中的
``compatible`` 条目会与所有平台驱动的表进行匹配。)

.. code:: c

           /* Match table for OF platform binding */
           static const struct of_device_id ctucan_of_match[] = {
               { .compatible = "ctu,canfd-2", },
               { .compatible = "ctu,ctucanfd", },
               { /* end of list */ },
           };
           MODULE_DEVICE_TABLE(of, ctucan_of_match);

           static int ctucan_probe(struct platform_device *pdev);
           static int ctucan_remove(struct platform_device *pdev);

           static struct platform_driver ctucanfd_driver = {
               .probe  = ctucan_probe,
               .remove = ctucan_remove,
               .driver = {
                   .name = DRIVER_NAME,
                   .of_match_table = ctucan_of_match,
               },
           };
           module_platform_driver(ctucanfd_driver);


.. _sec:socketcan:netdev:

Network device driver
^^^^^^^^^^^^^^^^^^^^^

(网络设备驱动)

Each network device must support at least these operations:
(每个网络设备至少必须支持以下操作：)

-  Bring the device up: ``ndo_open``
   (启用设备：``ndo_open``)

-  Bring the device down: ``ndo_close``
   (停用设备：``ndo_close``)

-  Submit TX frames to the device: ``ndo_start_xmit``
   (向设备提交发送 (TX) 帧：``ndo_start_xmit``)

-  Signal TX completion and errors to the network subsystem: ISR
   (向网络子系统通告发送完成与错误：ISR)

-  Submit RX frames to the network subsystem: ISR and NAPI
   (向网络子系统提交接收 (RX) 帧：ISR 与 NAPI)

There are two possible event sources: the device and the network
subsystem. Device events are usually signaled via an interrupt, handled
in an Interrupt Service Routine (ISR). Handlers for the events
originating in the network subsystem are then specified in
``struct net_device_ops``.
(事件来源有两种：设备和网络子系统。设备事件通常通过中断通告，由中断
服务程序 (ISR) 处理。源自网络子系统的事件的处理函数则指定在
``struct net_device_ops`` 中。)

When the device is brought up, e.g., by calling ``ip link set can0 up``,
the driver’s function ``ndo_open`` is called. It should validate the
interface configuration and configure and enable the device. The
analogous opposite is ``ndo_close``, called when the device is being
brought down, be it explicitly or implicitly.
(设备被启用时(例如通过调用 ``ip link set can0 up``)，会调用驱动的
``ndo_open`` 函数。它应当校验接口配置，并对设备进行配置和启用。与之相对
的是 ``ndo_close``，在设备被停用时调用，无论是显式还是隐式停用。)

When the system should transmit a frame, it does so by calling
``ndo_start_xmit``, which enqueues the frame into the device. If the
device HW queue (FIFO, mailboxes or whatever the implementation is)
becomes full, the ``ndo_start_xmit`` implementation informs the network
subsystem that it should stop the TX queue (via ``netif_stop_queue``).
It is then re-enabled later in ISR when the device has some space
available again and is able to enqueue another frame.
(系统要发送帧时，通过调用 ``ndo_start_xmit`` 完成，该函数把帧入队到设备
中。如果设备硬件队列(FIFO、邮箱或任何实现形式)变满，``ndo_start_xmit``
的实现会通知网络子系统停止发送 (TX) 队列(通过 ``netif_stop_queue``)。
之后当设备又有可用空间、能够再入队一帧时，会在 ISR 中重新启用该队列。)

All the device events are handled in ISR, namely:
(所有设备事件都在 ISR 中处理，具体包括：)

#. **TX completion**. When the device successfully finishes transmitting
   a frame, the frame is echoed locally. On error, an informative error
   frame [2]_ is sent to the network subsystem instead. In both cases,
   the software TX queue is resumed so that more frames may be sent.
   (TX 完成。设备成功完成一帧的发送后，该帧会在本地回环。出错时，则会向
   网络子系统发送一个信息性错误帧 [2]_。两种情况下，软件 TX 队列都会被
   恢复，以便发送更多帧。)

#. **Error condition**. If something goes wrong (e.g., the device goes
   bus-off or RX overrun happens), error counters are updated, and
   informative error frames are enqueued to SW RX queue.
   (错误状况。如果出了问题(例如设备进入 bus-off 状态或发生 RX 溢出)，
   错误计数器会被更新，信息性错误帧会被入队到软件 RX 队列。)

#. **RX buffer not empty**. In this case, read the RX frames and enqueue
   them to SW RX queue. Usually NAPI is used as a middle layer (see ).
   (RX 缓冲区非空。此时读取 RX 帧并入队到软件 RX 队列。通常使用 NAPI
   作为中间层(参见 )。)

.. _sec:socketcan:napi:

NAPI
~~~~

(NAPI)

The frequency of incoming frames can be high and the overhead to invoke
the interrupt service routine for each frame can cause significant
system load. There are multiple mechanisms in the Linux kernel to deal
with this situation. They evolved over the years of Linux kernel
development and enhancements. For network devices, the current standard
is NAPI – *the New API*. It is similar to classical top-half/bottom-half
interrupt handling in that it only acknowledges the interrupt in the ISR
and signals that the rest of the processing should be done in softirq
context. On top of that, it offers the possibility to *poll* for new
frames for a while. This has a potential to avoid the costly round of
enabling interrupts, handling an incoming IRQ in ISR, re-enabling the
softirq and switching context back to softirq.
(接收帧的频率可能很高，为每帧调用一次中断服务程序的开销会造成可观的系统
负载。Linux 内核中有多种应对这一情况的机制，它们随 Linux 内核多年的发展
与改进而演化。对网络设备而言，当前的标准是 NAPI——新 API (the New API)。
它与经典的上半部/下半部中断处理类似：在 ISR 中仅应答中断，并发出信号
表示其余处理应在 softirq 上下文中完成。除此之外，它还提供了在一段时间内
轮询 (poll) 新帧的可能。这有望避免代价高昂的一整圈流程：启用中断、在
ISR 中处理到来的 IRQ、重新启用 softirq、再把上下文切换回 softirq。)

See :ref:`Documentation/networking/napi.rst <napi>` for more information.
(更多信息参见 :ref:`Documentation/networking/napi.rst <napi>`。)

Integrating the core to Xilinx Zynq
-----------------------------------

(将核集成到 Xilinx Zynq)

The core interfaces a simple subset of the Avalon
(search for Intel **Avalon Interface Specifications**)
bus as it was originally used on
Alterra FPGA chips, yet Xilinx natively interfaces with AXI
(search for ARM **AMBA AXI and ACE Protocol Specification AXI3,
AXI4, and AXI4-Lite, ACE and ACE-Lite**).
The most obvious solution would be to use
an Avalon/AXI bridge or implement some simple conversion entity.
However, the core’s interface is half-duplex with no handshake
signaling, whereas AXI is full duplex with two-way signaling. Moreover,
even AXI-Lite slave interface is quite resource-intensive, and the
flexibility and speed of AXI are not required for a CAN core.
(该核的接口是 Avalon 总线(可查阅 Intel **Avalon Interface
Specifications**)的一个简单子集——Avalon 最初用于 Altera FPGA 芯片，而
Xilinx 原生对接的是 AXI(可查阅 ARM **AMBA AXI and ACE Protocol
Specification AXI3, AXI4, and AXI4-Lite, ACE and ACE-Lite**)。最显而易见
的方案是使用 Avalon/AXI 桥，或实现某个简单的转换实体。然而，该核的接口
是半双工的、没有握手信号，而 AXI 是带双向信号的全双工接口。此外，即使是
AXI-Lite 从设备接口也相当耗费资源，而且 CAN 核并不需要 AXI 的灵活性与
速度。)

Thus a much simpler bus was chosen – APB (Advanced Peripheral Bus)
(search for ARM **AMBA APB Protocol Specification**).
APB-AXI bridge is directly available in
Xilinx Vivado, and the interface adaptor entity is just a few simple
combinatorial assignments.
(于是选择了一条简单得多的总线——APB (Advanced Peripheral Bus，高级外设
总线)(可查阅 ARM **AMBA APB Protocol Specification**)。APB-AXI 桥在
Xilinx Vivado 中可以直接使用，接口适配实体只是几条简单的组合逻辑
赋值。)

Finally, to be able to include the core in a block diagram as a custom
IP, the core, together with the APB interface, has been packaged as a
Vivado component.
(最后，为了能把该核作为自定义 IP 放进框图，该核连同 APB 接口一起被封装
成了 Vivado 组件。)

CTU CAN FD Driver design
------------------------

(CTU CAN FD 驱动设计)

The general structure of a CAN device driver has already been examined
in . The next paragraphs provide a more detailed description of the CTU
CAN FD core driver in particular.
(CAN 设备驱动的一般结构已在前文讨论过。接下来几段将对 CTU CAN FD 核驱动
作更详细的描述。)

Low-level driver
~~~~~~~~~~~~~~~~

(低层驱动)

The core is not intended to be used solely with SocketCAN, and thus it
is desirable to have an OS-independent low-level driver. This low-level
driver can then be used in implementations of OS driver or directly
either on bare metal or in a user-space application. Another advantage
is that if the hardware slightly changes, only the low-level driver
needs to be modified.
(该核并非只打算配合 SocketCAN 使用，因此最好有一个与操作系统无关的低层
驱动。这个低层驱动随后既可用于各操作系统的驱动实现，也可以直接用于裸机
或用户空间应用。另一个好处是，如果硬件稍有改动，只需修改低层驱动即可。)

The code [3]_ is in part automatically generated and in part written
manually by the core author, with contributions of the thesis’ author.
The low-level driver supports operations such as: set bit timing, set
controller mode, enable/disable, read RX frame, write TX frame, and so
on.
(代码 [3]_ 一部分自动生成，一部分由核作者手工编写，论文作者亦有贡献。
低层驱动支持的操作包括：设置位时序、设置控制器模式、启用/禁用、读取
RX 帧、写入 TX 帧等。)

Configuring bit timing
~~~~~~~~~~~~~~~~~~~~~~

(配置位时序)

On CAN, each bit is divided into four segments: SYNC, PROP, PHASE1, and
PHASE2. Their duration is expressed in multiples of a Time Quantum
(details in `CAN Specification, Version 2.0 <http://esd.cs.ucr.edu/webres/can20.pdf>`_, chapter 8).
When configuring
bitrate, the durations of all the segments (and time quantum) must be
computed from the bitrate and Sample Point. This is performed
independently for both the Nominal bitrate and Data bitrate for CAN FD.
(在 CAN 上，每个位被划分为四个段：SYNC、PROP、PHASE1 和 PHASE2。其时长
以时间量子 (Time Quantum) 的倍数表示(详见 `CAN Specification, Version
2.0` 第 8 章)。配置比特率时，所有段(以及时间量子)的时长必须根据比特率
和采样点 (Sample Point) 计算得出。对 CAN FD 而言，标称比特率 (Nominal
bitrate) 与数据比特率 (Data bitrate) 各自独立进行这一计算。)

SocketCAN is fairly flexible and offers either highly customized
configuration by setting all the segment durations manually, or a
convenient configuration by setting just the bitrate and sample point
(and even that is chosen automatically per Bosch recommendation if not
specified). However, each CAN controller may have different base clock
frequency and different width of segment duration registers. The
algorithm thus needs the minimum and maximum values for the durations
(and clock prescaler) and tries to optimize the numbers to fit both the
constraints and the requested parameters.
(SocketCAN 相当灵活：既可以通过手动设置所有段时长进行高度定制的配置，
也可以只设置比特率和采样点来进行便捷配置(若不指定，采样点甚至会按
Bosch 建议自动选取)。然而，每个 CAN 控制器可能有不同的基准时钟频率和
不同位宽的段时长寄存器。因此，该算法需要各时长(以及时钟预分频器)的
最小值和最大值，并尝试优化这些数值，使其既满足约束又符合所请求的
参数。)

.. code:: c

           struct can_bittiming_const {
               char name[16];      /* Name of the CAN controller hardware */
               __u32 tseg1_min;    /* Time segment 1 = prop_seg + phase_seg1 */
               __u32 tseg1_max;
               __u32 tseg2_min;    /* Time segment 2 = phase_seg2 */
               __u32 tseg2_max;
               __u32 sjw_max;      /* Synchronisation jump width */
               __u32 brp_min;      /* Bit-rate prescaler */
               __u32 brp_max;
               __u32 brp_inc;
           };


[lst:can_bittiming_const]

A curious reader will notice that the durations of the segments PROP_SEG
and PHASE_SEG1 are not determined separately but rather combined and
then, by default, the resulting TSEG1 is evenly divided between PROP_SEG
and PHASE_SEG1. In practice, this has virtually no consequences as the
sample point is between PHASE_SEG1 and PHASE_SEG2. In CTU CAN FD,
however, the duration registers ``PROP`` and ``PH1`` have different
widths (6 and 7 bits, respectively), so the auto-computed values might
overflow the shorter register and must thus be redistributed among the
two [4]_.

Handling RX
~~~~~~~~~~~

(接收处理)

Frame reception is handled in NAPI queue, which is enabled from ISR when
the RXNE (RX FIFO Not Empty) bit is set. Frames are read one by one
until either no frame is left in the RX FIFO or the maximum work quota
has been reached for the NAPI poll run (see ). Each frame is then passed
to the network interface RX queue.
(帧的接收在 NAPI 队列中处理。当 RXNE (RX FIFO Not Empty，接收 FIFO 非空)
位被置位时，ISR 会启用该队列。然后一帧一帧地读取，直到 RX FIFO 中不再
有帧，或本次 NAPI 轮询已达到最大工作量配额(参见 )为止。每一帧随后被
传递给网络接口的 RX 队列。)

An incoming frame may be either a CAN 2.0 frame or a CAN FD frame. The
way to distinguish between these two in the kernel is to allocate either
``struct can_frame`` or ``struct canfd_frame``, the two having different
sizes. In the controller, the information about the frame type is stored
in the first word of RX FIFO.
(到来的帧可能是 CAN 2.0 帧或 CAN FD 帧。在内核中区分这两种帧的方式是：
根据帧类型分配 ``struct can_frame`` 或 ``struct canfd_frame``——二者大小
不同。在控制器中，帧类型信息存储在 RX FIFO 的第一个字中。)

This brings us a chicken-egg problem: we want to allocate the ``skb``
for the frame, and only if it succeeds, fetch the frame from FIFO;
otherwise keep it there for later. But to be able to allocate the
correct ``skb``, we have to fetch the first work of FIFO. There are
several possible solutions:
(这就带来了一个先有鸡还是先有蛋的问题：我们想为该帧分配 ``skb``，只有
分配成功才从 FIFO 取出帧，否则把它留在原地稍后再取。但要分配正确的
``skb``，又必须先取出 FIFO 的第一个字。有几种可行的解决方案：)

#. Read the word, then allocate. If it fails, discard the rest of the
   frame. When the system is low on memory, the situation is bad anyway.
   (先读出该字，再分配。若分配失败，则丢弃帧的其余部分。系统内存不足时，
   情况反正已经很糟。)

#. Always allocate ``skb`` big enough for an FD frame beforehand. Then
   tweak the ``skb`` internals to look like it has been allocated for
   the smaller CAN 2.0 frame.
   (预先分配足以容纳 FD 帧的 ``skb``。然后调整 ``skb`` 内部结构，使其
   看起来像是为较小的 CAN 2.0 帧分配的。)

#. Add option to peek into the FIFO instead of consuming the word.
   (增加一个窥视 (peek) FIFO 的选项，而不消耗该字。)

#. If the allocation fails, store the read word into driver’s data. On
   the next try, use the stored word instead of reading it again.
   (若分配失败，把读出的字保存到驱动数据中。下次尝试时使用保存的字，而
   不重新读取。)

Option 1 is simple enough, but not very satisfying if we could do
better. Option 2 is not acceptable, as it would require modifying the
private state of an integral kernel structure. The slightly higher
memory consumption is just a virtual cherry on top of the “cake”. Option
3 requires non-trivial HW changes and is not ideal from the HW point of
view.
(方案 1 足够简单，但如果能做得更好就不太令人满意。方案 2 不可接受，因为
它需要修改一个内核核心结构的私有状态。略高的内存消耗不过是"蛋糕"上
虚设的樱桃。方案 3 需要不小的硬件改动，从硬件角度看并不理想。)

Option 4 seems like a good compromise, with its disadvantage being that
a partial frame may stay in the FIFO for a prolonged time. Nonetheless,
there may be just one owner of the RX FIFO, and thus no one else should
see the partial frame (disregarding some exotic debugging scenarios).
Basides, the driver resets the core on its initialization, so the
partial frame cannot be “adopted” either. In the end, option 4 was
selected [5]_.
(方案 4 看似是一个不错的折中，其缺点是残帧可能会在 FIFO 中停留较长时间。
尽管如此，RX FIFO 只可能有一个所有者，因此不应有其他人看到这个残帧
(忽略某些特殊的调试场景)。此外，驱动会在初始化时复位该核，所以残帧也
不可能被"收养"。最终选择了方案 4 [5]_。)

.. _subsec:ctucanfd:rxtimestamp:

Timestamping RX frames
^^^^^^^^^^^^^^^^^^^^^^

(接收帧的时间戳)

The CTU CAN FD core reports the exact timestamp when the frame has been
received. The timestamp is by default captured at the sample point of
the last bit of EOF but is configurable to be captured at the SOF bit.
The timestamp source is external to the core and may be up to 64 bits
wide. At the time of writing, passing the timestamp from kernel to
userspace is not yet implemented, but is planned in the future.
(CTU CAN FD 核会报告帧被接收的精确时间戳。默认情况下，时间戳在 EOF 最后
一个位的采样点处捕获，但也可配置为在 SOF 位处捕获。时间戳源位于核外部，
宽度最多可达 64 位。撰写本文时，从内核向用户空间传递时间戳尚未实现，
但已计划在将来实现。)

Handling TX
~~~~~~~~~~~

(发送处理)

The CTU CAN FD core has 4 independent TX buffers, each with its own
state and priority. When the core wants to transmit, a TX buffer in
Ready state with the highest priority is selected.
(CTU CAN FD 核有 4 个独立的 TX 缓冲区，每个都有自己的状态和优先级。当核
要发送时，会选中处于 Ready 状态且优先级最高的 TX 缓冲区。)

The priorities are 3bit numbers in register TX_PRIORITY
(nibble-aligned). This should be flexible enough for most use cases.
SocketCAN, however, supports only one FIFO queue for outgoing
frames [6]_. The buffer priorities may be used to simulate the FIFO
behavior by assigning each buffer a distinct priority and *rotating* the
priorities after a frame transmission is completed.
(优先级是寄存器 TX_PRIORITY 中的 3 位数值(按半字节对齐)。这对大多数使用
场景应该足够灵活。然而，SocketCAN 对发送的帧只支持一个 FIFO 队列 [6]_。
可以为每个缓冲区分配不同的优先级，并在一帧发送完成后轮转 (rotating)
优先级，以此利用缓冲区优先级来模拟 FIFO 行为。)

In addition to priority rotation, the SW must maintain head and tail
pointers into the FIFO formed by the TX buffers to be able to determine
which buffer should be used for next frame (``txb_head``) and which
should be the first completed one (``txb_tail``). The actual buffer
indices are (obviously) modulo 4 (number of TX buffers), but the
pointers must be at least one bit wider to be able to distinguish
between FIFO full and FIFO empty – in this situation,
:math:`txb\_head \equiv txb\_tail\ (\textrm{mod}\ 4)`. An example of how
the FIFO is maintained, together with priority rotation, is depicted in
(除了优先级轮转之外，软件还必须维护由 TX 缓冲区构成的 FIFO 的头指针和尾
指针，以便确定下一帧应使用哪个缓冲区(``txb_head``)，以及哪个应是第一个
完成的(``txb_tail``)。缓冲区实际索引(显然)按 4 (TX 缓冲区数量)取模，
但指针必须至少多出一位，才能区分 FIFO 满与 FIFO 空——此时有
txb_head ≡ txb_tail (mod 4)。下面以示例形式描绘了 FIFO 的维护方式，以及
优先级的轮转：)

|

+------+---+---+---+---+
| TXB# | 0 | 1 | 2 | 3 |
+======+===+===+===+===+
| Seq  | A | B | C |   |
+------+---+---+---+---+
| Prio | 7 | 6 | 5 | 4 |
+------+---+---+---+---+
|      |   | T |   | H |
+------+---+---+---+---+

|

+------+---+---+---+---+
| TXB# | 0 | 1 | 2 | 3 |
+======+===+===+===+===+
| Seq  |   | B | C |   |
+------+---+---+---+---+
| Prio | 4 | 7 | 6 | 5 |
+------+---+---+---+---+
|      |   | T |   | H |
+------+---+---+---+---+

|

+------+---+---+---+---+----+
| TXB# | 0 | 1 | 2 | 3 | 0’ |
+======+===+===+===+===+====+
| Seq  | E | B | C | D |    |
+------+---+---+---+---+----+
| Prio | 4 | 7 | 6 | 5 |    |
+------+---+---+---+---+----+
|      |   | T |   |   | H  |
+------+---+---+---+---+----+

|

.. kernel-figure:: fsm_txt_buffer_user.svg

   TX Buffer states with possible transitions

.. _subsec:ctucanfd:txtimestamp:

Timestamping TX frames
^^^^^^^^^^^^^^^^^^^^^^

(发送帧的时间戳)

When submitting a frame to a TX buffer, one may specify the timestamp at
which the frame should be transmitted. The frame transmission may start
later, but not sooner. Note that the timestamp does not participate in
buffer prioritization – that is decided solely by the mechanism
described above.
(向 TX 缓冲区提交帧时，可以指定该帧应在何时发送。帧的发送可以晚于该时刻
开始，但不会更早。注意，时间戳不参与缓冲区的优先级排序——那完全由前文
所述机制决定。)

Support for time-based packet transmission was recently merged to Linux
v4.19 `Time-based packet transmission <https://lwn.net/Articles/748879/>`_,
but it remains yet to be researched
whether this functionality will be practical for CAN.
(基于时间的数据包发送支持最近已合并到 Linux v4.19 `Time-based packet
transmission` 中，但该功能对 CAN 是否实用仍有待研究。)

Also similarly to retrieving the timestamp of RX frames, the core
supports retrieving the timestamp of TX frames – that is the time when
the frame was successfully delivered. The particulars are very similar
to timestamping RX frames and are described in .
(与获取 RX 帧时间戳类似，该核也支持获取 TX 帧的时间戳——即帧被成功送达
的时间。其细节与 RX 帧时间戳非常相似，在中有所描述。)

Handling RX buffer overrun
~~~~~~~~~~~~~~~~~~~~~~~~~~

(处理 RX 缓冲区溢出)

When a received frame does no more fit into the hardware RX FIFO in its
entirety, RX FIFO overrun flag (STATUS[DOR]) is set and Data Overrun
Interrupt (DOI) is triggered. When servicing the interrupt, care must be
taken first to clear the DOR flag (via COMMAND[CDO]) and after that
clear the DOI interrupt flag. Otherwise, the interrupt would be
immediately [7]_ rearmed.
(当收到的帧再也无法完整放入硬件 RX FIFO 时，会置位 RX FIFO 溢出标志
(STATUS[DOR]) 并触发数据溢出中断 (DOI)。处理该中断时必须注意：先清除
DOR 标志(通过 COMMAND[CDO])，然后再清除 DOI 中断标志。否则，中断会
立即 [7]_ 被重新触发。)

**Note**: During development, it was discussed whether the internal HW
pipelining cannot disrupt this clear sequence and whether an additional
dummy cycle is necessary between clearing the flag and the interrupt. On
the Avalon interface, it indeed proved to be the case, but APB being
safe because it uses 2-cycle transactions. Essentially, the DOR flag
would be cleared, but DOI register’s Preset input would still be high
the cycle when the DOI clear request would also be applied (by setting
the register’s Reset input high). As Set had higher priority than Reset,
the DOI flag would not be reset. This has been already fixed by swapping
the Set/Reset priority (see issue #187).
(注意：在开发过程中曾讨论过，内部硬件流水线是否会破坏这一清除顺序，以及
在清除标志与清除中断之间是否需要额外插入一个空转周期。在 Avalon 接口上
确实如此；而 APB 之所以安全，是因为它使用 2 周期事务。本质上，DOR 标志
会被清除，但在同时施加 DOI 清除请求的那个周期里(通过把寄存器的 Reset
输入拉高)，DOI 寄存器的 Preset 输入仍为高。由于 Set 的优先级高于 Reset，
DOI 标志不会被复位。通过交换 Set/Reset 的优先级，此问题已得到修复
(参见 issue #187)。)

Reporting Error Passive and Bus Off conditions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

(上报 Error Passive 与 Bus Off 状态)

It may be desirable to report when the node reaches *Error Passive*,
*Error Warning*, and *Bus Off* conditions. The driver is notified about
error state change by an interrupt (EPI, EWLI), and then proceeds to
determine the core’s error state by reading its error counters.
(当节点进入 Error Passive(错误被动)、Error Warning(错误警告)和
Bus Off(总线关闭)状态时，最好能予以上报。驱动通过中断 (EPI、EWLI)
获知错误状态变化，然后读取错误计数器来确定核的错误状态。)

There is, however, a slight race condition here – there is a delay
between the time when the state transition occurs (and the interrupt is
triggered) and when the error counters are read. When EPI is received,
the node may be either *Error Passive* or *Bus Off*. If the node goes
*Bus Off*, it obviously remains in the state until it is reset.
Otherwise, the node is *or was* *Error Passive*. However, it may happen
that the read state is *Error Warning* or even *Error Active*. It may be
unclear whether and what exactly to report in that case, but I
personally entertain the idea that the past error condition should still
be reported. Similarly, when EWLI is received but the state is later
detected to be *Error Passive*, *Error Passive* should be reported.
(不过，这里存在一个轻微的竞态条件——从状态转换发生(并触发中断)到读取
错误计数器之间有一段延迟。收到 EPI 时，节点可能处于 Error Passive 或
Bus Off。如果节点进入 Bus Off，它显然会保持该状态直到被复位。否则，节点
现在是(或曾经是)Error Passive。然而，读取到的状态也可能是 Error Warning
甚至 Error Active。这种情况下究竟是否上报、上报什么，可能并不明确，但我
个人倾向于认为仍应上报曾经出现的错误状态。类似地，当收到 EWLI 而之后
检测到的状态却是 Error Passive 时，应上报 Error Passive。)


CTU CAN FD Driver Sources Reference
-----------------------------------

(CTU CAN FD 驱动源码参考)

.. kernel-doc:: drivers/net/can/ctucanfd/ctucanfd.h
   :internal:

.. kernel-doc:: drivers/net/can/ctucanfd/ctucanfd_base.c
   :internal:

.. kernel-doc:: drivers/net/can/ctucanfd/ctucanfd_pci.c
   :internal:

.. kernel-doc:: drivers/net/can/ctucanfd/ctucanfd_platform.c
   :internal:

CTU CAN FD IP Core and Driver Development Acknowledgment
---------------------------------------------------------

(CTU CAN FD IP 核与驱动开发致谢)

* Odrej Ille <ondrej.ille@gmail.com>

  * started the project as student at Department of Measurement, FEE, CTU
    (以 CTU FEE 测量系学生的身份启动了本项目)
  * invested great amount of personal time and enthusiasm to the project over years
    (多年来为项目投入了大量个人时间与热情)
  * worked on more funded tasks
    (承担了更多获资助的任务)

* `Department of Measurement <https://meas.fel.cvut.cz/>`_,
  `Faculty of Electrical Engineering <http://www.fel.cvut.cz/en/>`_,
  `Czech Technical University <https://www.cvut.cz/en>`_

  * is the main investor into the project over many years
    (多年来是该项目的主要资助方)
  * uses project in their CAN/CAN FD diagnostics framework for `Skoda Auto <https://www.skoda-auto.cz/>`_
    (在其为 Skoda Auto 开发的 CAN/CAN FD 诊断框架中使用本项目)

* `Digiteq Automotive <https://www.digiteqautomotive.com/en>`_

  * funding of the project CAN FD Open Cores Support Linux Kernel Based Systems
    (资助项目 "CAN FD Open Cores Support Linux Kernel Based Systems")
  * negotiated and paid CTU to allow public access to the project
    (与 CTU 协商并付费，使项目得以向公众开放)
  * provided additional funding of the work
    (为相关工作提供了额外资助)

* `Department of Control Engineering <https://control.fel.cvut.cz/en>`_,
  `Faculty of Electrical Engineering <http://www.fel.cvut.cz/en/>`_,
  `Czech Technical University <https://www.cvut.cz/en>`_

  * solving the project CAN FD Open Cores Support Linux Kernel Based Systems
    (承担项目 "CAN FD Open Cores Support Linux Kernel Based Systems" 的
    研究)
  * providing GitLab management
    (提供 GitLab 管理)
  * virtual servers and computational power for continuous integration
    (为持续集成提供虚拟服务器与算力)
  * providing hardware for HIL continuous integration tests
    (为 HIL 持续集成测试提供硬件)

* `PiKRON Ltd. <http://pikron.com/>`_

  * minor funding to initiate preparation of the project open-sourcing
    (为启动项目开源的准备工作提供了小额资助)

* Petr Porazil <porazil@pikron.com>

  * design of PCIe transceiver addon board and assembly of boards
    (设计 PCIe 收发器扩展板并组装板卡)
  * design and assembly of MZ_APO baseboard for MicroZed/Zynq based system
    (设计并组装面向 MicroZed/Zynq 系统的 MZ_APO 底板)

* Martin Jerabek <martin.jerabek01@gmail.com>

  * Linux driver development
    (Linux 驱动开发)
  * continuous integration platform architect and GHDL updates
    (持续集成平台架构设计与 GHDL 更新)
  * thesis `Open-source and Open-hardware CAN FD Protocol Support <https://dspace.cvut.cz/bitstream/handle/10467/80366/F3-DP-2019-Jerabek-Martin-Jerabek-thesis-2019-canfd.pdf>`_
    (学位论文 `Open-source and Open-hardware CAN FD Protocol Support`)

* Jiri Novak <jnovak@fel.cvut.cz>

  * project initiation, management and use at Department of Measurement, FEE, CTU
    (在 CTU FEE 测量系发起、管理项目并在其中使用)

* Pavel Pisa <pisa@cmp.felk.cvut.cz>

  * initiate open-sourcing, project coordination, management at Department of Control Engineering, FEE, CTU
    (发起开源工作，并在 CTU FEE 控制工程系进行项目协调与管理)

* Jaroslav Beran<jara.beran@gmail.com>

 * system integration for Intel SoC, core and driver testing and updates
   (面向 Intel SoC 的系统集成，核与驱动的测试和更新)

* Carsten Emde (`OSADL <https://www.osadl.org/>`_)

 * provided OSADL expertise to discuss IP core licensing
   (提供 OSADL 的专业知识，参与 IP 核许可的讨论)
 * pointed to possible deadlock for LGPL and CAN bus possible patent case which lead to relicense IP core design to BSD like license
   (指出 LGPL 可能造成的僵局以及 CAN 总线潜在的专利风险，促成 IP 核设计
   改用类 BSD 许可证重新授权)

* Reiner Zitzmann and Holger Zeltwanger (`CAN in Automation <https://www.can-cia.org/>`_)

 * provided suggestions and help to inform community about the project and invited us to events focused on CAN bus future development directions
   (提供向社区宣传项目的建议与帮助，并邀请我们参加聚焦 CAN 总线未来发展
   方向的活动)

* Jan Charvat

 * implemented CTU CAN FD functional model for QEMU which has been integrated into QEMU mainline (`docs/system/devices/can.rst <https://www.qemu.org/docs/master/system/devices/can.html>`_)
   (为 QEMU 实现了 CTU CAN FD 功能模型，该模型已被并入 QEMU 主线)
 * Bachelor thesis Model of CAN FD Communication Controller for QEMU Emulator
   (学士学位论文 "Model of CAN FD Communication Controller for QEMU
   Emulator")

Notes
-----

(注释)


.. [1]
   Other buses have their own specific driver interface to set up the
   device.
   (其他总线有各自特定的驱动接口来建立设备。)

.. [2]
   Not to be mistaken with CAN Error Frame. This is a ``can_frame`` with
   ``CAN_ERR_FLAG`` set and some error info in its ``data`` field.
   (不要与 CAN 错误帧 (Error Frame) 混淆。这是设置了 ``CAN_ERR_FLAG`` 并在
   其 ``data`` 字段中带有一些错误信息的 ``can_frame``。)

.. [3]
   Available in CTU CAN FD repository
   `<https://gitlab.fel.cvut.cz/canbus/ctucanfd_ip_core>`_
   (可在 CTU CAN FD 仓库中获取。)

.. [4]
   As is done in the low-level driver functions
   ``ctucan_hw_set_nom_bittiming`` and
   ``ctucan_hw_set_data_bittiming``.
   (正如低层驱动函数 ``ctucan_hw_set_nom_bittiming`` 和
   ``ctucan_hw_set_data_bittiming`` 中所做的那样。)

.. [5]
   At the time of writing this thesis, option 1 is still being used and
   the modification is queued in gitlab issue #222
   (撰写本论文时，仍在使用方案 1，相应修改已排入 gitlab issue #222。)

.. [6]
   Strictly speaking, multiple CAN TX queues are supported since v4.19
   `can: enable multi-queue for SocketCAN devices <https://lore.kernel.org/patchwork/patch/913526/>`_ but no mainline driver is using
   them yet.
   (严格来说，自 v4.19 起已支持多个 CAN TX 队列 `can: enable multi-queue
   for SocketCAN devices`，但尚无主线驱动使用它们。)

.. [7]
   Or rather in the next clock cycle
   (或者更确切地说，是在下一个时钟周期。)
