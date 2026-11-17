.. SPDX-License-Identifier: GPL-2.0+

==============================================================
Linux kernel driver for Compute Engine Virtual Ethernet (gve):
==============================================================

(Linux 内核 Compute Engine 虚拟以太网 (gve) 驱动：)

Supported Hardware
(支持的硬件)
===================

The GVE driver binds to a single PCI device id used by the virtual
Ethernet device found in some Compute Engine VMs.
(GVE 驱动绑定到单一 PCI 设备 ID，该 ID 属于某些 Compute Engine 虚拟机中的虚拟以太网设备。)

+--------------+----------+---------+
|Field (字段)  | Value(值)| Comments|
+==============+==========+=========+
|Vendor ID     | `0x1AE0` | Google  |
+--------------+----------+---------+
|Device ID     | `0x0042` |         |
+--------------+----------+---------+
|Sub-vendor ID | `0x1AE0` | Google  |
+--------------+----------+---------+
|Sub-device ID | `0x0058` |         |
+--------------+----------+---------+
|Revision ID   | `0x0`    |         |
+--------------+----------+---------+
|Device Class  | `0x200`  | Ethernet|
+--------------+----------+---------+

PCI Bars
========

The gVNIC PCI device exposes three 32-bit memory BARS:
(gVNIC PCI 设备暴露三个 32 位内存 BAR：)

- Bar0 - Device configuration and status registers.
  (Bar0 - 设备配置和状态寄存器。)
- Bar1 - MSI-X vector table
  (Bar1 - MSI-X 向量表)
- Bar2 - IRQ, RX and TX doorbells
  (Bar2 - IRQ、RX 和 TX 门铃 (doorbell))

Device Interactions
(设备交互)
===================

The driver interacts with the device in the following ways:
(驱动通过以下方式与设备交互：)

 - Registers
   (寄存器)

    - A block of MMIO registers
      (一块 MMIO 寄存器)
    - See gve_register.h for more detail
      (更多细节参见 gve_register.h)
 - Admin Queue
   (管理队列)

    - See description below
      (参见下文描述)
 - Reset
   (复位)

    - At any time the device can be reset
      (设备随时都可以被复位)
 - Interrupts
   (中断)

    - See supported interrupts below
      (参见下文受支持的中断)
 - Transmit and Receive Queues
   (发送和接收队列)

    - See description below
      (参见下文描述)

Descriptor Formats
(描述符格式)
------------------

GVE supports two descriptor formats: GQI and DQO. These two formats have
entirely different descriptors, which will be described below.
(GVE 支持两种描述符格式：GQI 和 DQO。这两种格式的描述符完全不同，将在下文描述。)

Addressing Mode
(寻址模式)
------------------

GVE supports two addressing modes: QPL and RDA.
QPL ("queue-page-list") mode communicates data through a set of
pre-registered pages.
(GVE 支持两种寻址模式：QPL 和 RDA。QPL（"队列页列表"）模式通过一组预先注册的页来传递数据。)

For RDA ("raw DMA addressing") mode, the set of pages is dynamic.
Therefore, the packet buffers can be anywhere in guest memory.
(对于 RDA（"原始 DMA 寻址"）模式，页集合是动态的。因此，数据包缓冲区可以位于客户机内存的任何位置。)

Registers
(寄存器)
---------

All registers are MMIO.
(所有寄存器都是 MMIO。)

The registers are used for initializing and configuring the device as well as
querying device status in response to management interrupts.
(这些寄存器用于初始化和配置设备，以及响应管理中断时查询设备状态。)

Endianness
(字节序)
----------

- Admin Queue messages and registers are all Big Endian.
  (管理队列消息和寄存器都是大端序。)
- GQI descriptors and datapath registers are Big Endian.
  (GQI 描述符和数据路径寄存器是大端序。)
- DQO descriptors and datapath registers are Little Endian.
  (DQO 描述符和数据路径寄存器是小端序。)

Admin Queue (AQ)
(管理队列 (AQ))
----------------

The Admin Queue is a PAGE_SIZE memory block, treated as an array of AQ
commands, used by the driver to issue commands to the device and set up
resources.The driver and the device maintain a count of how many commands
have been submitted and executed. To issue AQ commands, the driver must do
the following (with proper locking):
(管理队列 (Admin Queue) 是一个 PAGE_SIZE 大小的内存块，被视为 AQ 命令的数组，供驱动向设备下发命令并建立资源。驱动和设备各自维护一个已提交和已执行命令数的计数。要下发 AQ 命令，驱动必须（在正确的加锁保护下）执行以下操作：)

1)  Copy new commands into next available slots in the AQ array
    (1. 将新命令复制到 AQ 数组中下一个可用的槽位)
2)  Increment its counter by he number of new commands
    (2. 将自己的计数器按新命令的数量递增)
3)  Write the counter into the GVE_ADMIN_QUEUE_DOORBELL register
    (3. 将该计数写入 GVE_ADMIN_QUEUE_DOORBELL 寄存器)
4)  Poll the ADMIN_QUEUE_EVENT_COUNTER register until it equals
    the value written to the doorbell, or until a timeout.
    (4. 轮询 ADMIN_QUEUE_EVENT_COUNTER 寄存器，直到它等于写入门铃的值，或直到超时。)

The device will update the status field in each AQ command reported as
executed through the ADMIN_QUEUE_EVENT_COUNTER register.
(设备会更新每个 AQ 命令中的状态字段，以报告其已通过 ADMIN_QUEUE_EVENT_COUNTER 寄存器执行完毕。)

Device Resets
(设备复位)
-------------

A device reset is triggered by writing 0x0 to the AQ PFN register.
This causes the device to release all resources allocated by the
driver, including the AQ itself.
(向 AQ PFN 寄存器写入 0x0 会触发设备复位。这会使设备释放驱动分配的所有资源，包括 AQ 本身。)

Interrupts
(中断)
----------

The following interrupts are supported by the driver:
(驱动支持以下中断：)

Management Interrupt
(管理中断)
~~~~~~~~~~~~~~~~~~~~

The management interrupt is used by the device to tell the driver to
look at the GVE_DEVICE_STATUS register.
(设备使用管理中断通知驱动查看 GVE_DEVICE_STATUS 寄存器。)

The handler for the management irq simply queues the service task in
the workqueue to check the register and acks the irq.
(管理中断的处理程序只是把服务任务排入 workqueue 以检查该寄存器，并确认 (ack) 该中断。)

Notification Block Interrupts
(通知块中断)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The notification block interrupts are used to tell the driver to poll
the queues associated with that interrupt.
(通知块中断用于通知驱动轮询与该中断关联的队列。)

The handler for these irqs schedule the napi for that block to run
and poll the queues.
(这些中断的处理程序会调度该通知块的 napi 运行并轮询队列。)

GQI Traffic Queues
(GQI 流量队列)
------------------

GQI queues are composed of a descriptor ring and a buffer and are assigned to a
notification block.
(GQI 队列由一个描述符环和一个缓冲区组成，并被分配给一个通知块。)

The descriptor rings are power-of-two-sized ring buffers consisting of
fixed-size descriptors. They advance their head pointer using a __be32
doorbell located in Bar2. The tail pointers are advanced by consuming
descriptors in-order and updating a __be32 counter. Both the doorbell
and the counter overflow to zero.
(描述符环是大小为 2 的幂的环形缓冲区，由固定大小的描述符组成。它们使用位于 Bar2 中的 __be32 门铃来推进头指针。尾指针通过按顺序消费描述符并更新一个 __be32 计数器来推进。门铃和计数器都会溢出归零。)

Each queue's buffers must be registered in advance with the device as a
queue page list, and packet data can only be put in those pages.
(每个队列的缓冲区必须事先以队列页列表 (queue page list) 的形式向设备注册，数据包数据只能放在这些页中。)

Transmit
(发送)
~~~~~~~~

gve maps the buffers for transmit rings into a FIFO and copies the packets
into the FIFO before sending them to the NIC.
(gve 将发送环的缓冲区映射成一个 FIFO，并在把数据包发送给网卡之前将其复制到该 FIFO 中。)

Receive
(接收)
~~~~~~~

The buffers for receive rings are put into a data ring that is the same
length as the descriptor ring and the head and tail pointers advance over
the rings together.
(接收环的缓冲区被放入一个与描述符环等长的数据环中，头指针和尾指针在两个环上同步推进。)

DQO Traffic Queues
(DQO 流量队列)
------------------

- Every TX and RX queue is assigned a notification block.
  (每个 TX 和 RX 队列都被分配一个通知块。)

- TX and RX buffers queues, which send descriptors to the device, use MMIO
  doorbells to notify the device of new descriptors.
  (向设备发送描述符的 TX 和 RX 缓冲队列使用 MMIO 门铃通知设备有新描述符。)

- RX and TX completion queues, which receive descriptors from the device, use a
  "generation bit" to know when a descriptor was populated by the device. The
  driver initializes all bits with the "current generation". The device will
  populate received descriptors with the "next generation" which is inverted
  from the current generation. When the ring wraps, the current/next generation
  are swapped.
  (从设备接收描述符的 RX 和 TX 完成队列使用"代次位" (generation bit) 来判断描述符何时由设备填入。驱动用"当前代次"初始化所有位。设备会用"下一代次"（当前代次的反转）来填入接收到的描述符。当环回绕时，当前/下一代次相互交换。)

- It's the driver's responsibility to ensure that the RX and TX completion
  queues are not overrun. This can be accomplished by limiting the number of
  descriptors posted to HW.
  (确保 RX 和 TX 完成队列不被 overrun 是驱动的责任。这可以通过限制提交给硬件的描述符数量来实现。)

- TX packets have a 16 bit completion_tag and RX buffers have a 16 bit
  buffer_id. These will be returned on the TX completion and RX queues
  respectively to let the driver know which packet/buffer was completed.
  (TX 数据包有一个 16 位 completion_tag，RX 缓冲区有一个 16 位 buffer_id。它们会分别在 TX 完成队列和 RX 队列上返回，让驱动知道哪个数据包/缓冲区已完成。)

Transmit
(发送)
~~~~~~~~

A packet's buffers are DMA mapped for the device to access before transmission.
After the packet was successfully transmitted, the buffers are unmapped.
(在传输之前，数据包的缓冲区被 DMA 映射以供设备访问。数据包成功发送后，缓冲区被解除映射。)

Receive
(接收)
~~~~~~~

The driver posts fixed sized buffers to HW on the RX buffer queue. The packet
received on the associated RX queue may span multiple descriptors.
(驱动在 RX 缓冲队列上向硬件提交固定大小的缓冲区。在关联的 RX 队列上收到的数据包可能跨越多个描述符。)
