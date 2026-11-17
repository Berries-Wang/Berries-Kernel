.. SPDX-License-Identifier: GPL-2.0

============================================================
Linux kernel driver for Elastic Network Adapter (ENA) family
============================================================

(Elastic Network Adapter (ENA) 系列的 Linux 内核驱动程序)

Overview
(概述)
========

ENA is a networking interface designed to make good use of modern CPU
features and system architectures.
(ENA 是一种旨在充分利用现代 CPU 特性和系统架构的网络接口。)

The ENA device exposes a lightweight management interface with a
minimal set of memory mapped registers and extendible command set
through an Admin Queue.
(ENA 设备通过管理队列 (Admin Queue) 暴露一个轻量级管理接口，其中只包含
最少量的内存映射寄存器和可扩展的命令集。)

The driver supports a range of ENA devices, is link-speed independent
(i.e., the same driver is used for 10GbE, 25GbE, 40GbE, etc), and has
a negotiated and extendible feature set.
(该驱动程序支持多种 ENA 设备，与链路速度无关 (即 10GbE、25GbE、40GbE 等
使用同一个驱动程序)，并拥有一套经协商且可扩展的特性集。)

Some ENA devices support SR-IOV. This driver is used for both the
SR-IOV Physical Function (PF) and Virtual Function (VF) devices.
(一些 ENA 设备支持 SR-IOV。该驱动程序既用于 SR-IOV 物理功能 (PF) 设备，
也用于虚拟功能 (VF) 设备。)

ENA devices enable high speed and low overhead network traffic
processing by providing multiple Tx/Rx queue pairs (the maximum number
is advertised by the device via the Admin Queue), a dedicated MSI-X
interrupt vector per Tx/Rx queue pair, adaptive interrupt moderation,
and CPU cacheline optimized data placement.
(ENA 设备通过提供多个 Tx/Rx 队列对 (最大数量由设备通过管理队列通告)、
每个 Tx/Rx 队列对专用的 MSI-X 中断向量、自适应中断调节以及针对 CPU 缓存行
优化的数据放置，实现高速、低开销的网络流量处理。)

The ENA driver supports industry standard TCP/IP offload features such as
checksum offload. Receive-side scaling (RSS) is supported for multi-core
scaling.
(ENA 驱动程序支持业界标准的 TCP/IP 卸载特性，例如校验和卸载。支持接收侧
扩展 (RSS) 以实现多核扩展。)

The ENA driver and its corresponding devices implement health
monitoring mechanisms such as watchdog, enabling the device and driver
to recover in a manner transparent to the application, as well as
debug logs.
(ENA 驱动程序及其对应的设备实现了看门狗等健康监控机制，使设备和驱动程序
能够以对应用透明的方式恢复，并提供调试日志。)

Some of the ENA devices support a working mode called Low-latency
Queue (LLQ), which saves several more microseconds.
(部分 ENA 设备支持一种称为低延迟队列 (LLQ) 的工作模式，它可以再节省数个
微秒的时间。)

ENA Source Code Directory Structure
(ENA 源代码目录结构)
===================================

=================   ======================================================
ena_com.[ch]        Management communication layer. This layer is
                    responsible for the handling all the management
                    (admin) communication between the device and the
                    driver.
                    (管理通信层。该层负责处理设备与驱动程序之间的所有管理
                    (admin) 通信。)
ena_eth_com.[ch]    Tx/Rx data path.
                    (发送/接收数据路径。)
ena_admin_defs.h    Definition of ENA management interface.
                    (ENA 管理接口的定义。)
ena_eth_io_defs.h   Definition of ENA data path interface.
                    (ENA 数据路径接口的定义。)
ena_common_defs.h   Common definitions for ena_com layer.
                    (ena_com 层的通用定义。)
ena_regs_defs.h     Definition of ENA PCI memory-mapped (MMIO) registers.
                    (ENA PCI 内存映射 (MMIO) 寄存器的定义。)
ena_netdev.[ch]     Main Linux kernel driver.
                    (Linux 内核主驱动程序。)
ena_ethtool.c       ethtool callbacks.
                    (ethtool 回调。)
ena_xdp.[ch]        XDP files
                    (XDP 文件)
ena_pci_id_tbl.h    Supported device IDs.
                    (支持的设备 ID。)
ena_phc.[ch]        PTP hardware clock infrastructure (see `PHC`_ for more info)
                    (PTP 硬件时钟基础设施 (更多信息参见 `PHC`_))
ena_devlink.[ch]    devlink files.
                    (devlink 文件。)
ena_debugfs.[ch]    debugfs files.
                    (debugfs 文件。)
=================   ======================================================

Management Interface:
(管理接口:)
=====================

ENA management interface is exposed by means of:
(ENA 管理接口通过以下方式暴露：)

- PCIe Configuration Space
  (PCIe 配置空间)
- Device Registers
  (设备寄存器)
- Admin Queue (AQ) and Admin Completion Queue (ACQ)
  (管理队列 (AQ) 和管理完成队列 (ACQ))
- Asynchronous Event Notification Queue (AENQ)
  (异步事件通知队列 (AENQ))

ENA device MMIO Registers are accessed only during driver
initialization and are not used during further normal device
operation.
(ENA 设备的 MMIO 寄存器仅在驱动程序初始化期间被访问，在之后的正常设备
运行期间不再使用。)

AQ is used for submitting management commands, and the
results/responses are reported asynchronously through ACQ.
(AQ 用于提交管理命令，结果/响应通过 ACQ 异步上报。)

ENA introduces a small set of management commands with room for
vendor-specific extensions. Most of the management operations are
framed in a generic Get/Set feature command.
(ENA 引入了一小组管理命令，并为厂商特定扩展预留了空间。大多数管理操作都
被封装在一个通用的 Get/Set feature 命令中。)

The following admin queue commands are supported:
(支持以下管理队列命令：)

- Create I/O submission queue
  (创建 I/O 提交队列)
- Create I/O completion queue
  (创建 I/O 完成队列)
- Destroy I/O submission queue
  (销毁 I/O 提交队列)
- Destroy I/O completion queue
  (销毁 I/O 完成队列)
- Get feature
  (获取特性)
- Set feature
  (设置特性)
- Configure AENQ
  (配置 AENQ)
- Get statistics
  (获取统计信息)

Refer to ena_admin_defs.h for the list of supported Get/Set Feature
properties.
(支持的 Get/Set Feature 属性列表请参见 ena_admin_defs.h。)

The Asynchronous Event Notification Queue (AENQ) is a uni-directional
queue used by the ENA device to send to the driver events that cannot
be reported using ACQ. AENQ events are subdivided into groups. Each
group may have multiple syndromes, as shown below
(异步事件通知队列 (AENQ) 是一个单向队列，ENA 设备用它向驱动程序发送无法
通过 ACQ 上报的事件。AENQ 事件被细分为多个组。每组可能有多个症候
(syndrome)，如下所示)

The events are:
(事件如下：)

====================    ===============
Group (组别)            Syndrome (症候)
====================    ===============
Link state change       **X**
Fatal error             **X**
Notification (通知)     Suspend traffic
                        (暂停流量)
Notification (通知)     Resume traffic
                        (恢复流量)
Keep-Alive (保活)       **X**
====================    ===============

ACQ and AENQ share the same MSI-X vector.
(ACQ 与 AENQ 共享同一个 MSI-X 中断向量。)

Keep-Alive is a special mechanism that allows monitoring the device's health.
A Keep-Alive event is delivered by the device every second.
The driver maintains a watchdog (WD) handler which logs the current state and
statistics. If the keep-alive events aren't delivered as expected the WD resets
the device and the driver.
(Keep-Alive 是一种用于监控设备健康状况的特殊机制。设备每秒发送一次
Keep-Alive 事件。驱动程序维护一个看门狗 (WD) 处理程序，用于记录当前状态和
统计信息。如果 keep-alive 事件未按预期送达，WD 将复位设备和驱动程序。)

Data Path Interface
(数据路径接口)
===================

I/O operations are based on Tx and Rx Submission Queues (Tx SQ and Rx
SQ correspondingly). Each SQ has a completion queue (CQ) associated
with it.
(I/O 操作基于发送和接收提交队列 (分别为 Tx SQ 和 Rx SQ)。每个 SQ 都有一个
与之关联的完成队列 (CQ)。)

The SQs and CQs are implemented as descriptor rings in contiguous
physical memory.
(SQ 和 CQ 实现为连续物理内存中的描述符环。)

The ENA driver supports two Queue Operation modes for Tx SQs:
(ENA 驱动程序为 Tx SQ 支持两种队列操作模式：)

- **Regular mode:**
  In this mode the Tx SQs reside in the host's memory. The ENA
  device fetches the ENA Tx descriptors and packet data from host
  memory.
  (**常规模式：**
  在此模式下，Tx SQ 驻留在主机的内存中。ENA 设备从主机内存中获取 ENA Tx
  描述符和数据包数据。)

- **Low Latency Queue (LLQ) mode or "push-mode":**
  In this mode the driver pushes the transmit descriptors and the
  first 96 bytes of the packet directly to the ENA device memory
  space. The rest of the packet payload is fetched by the
  device. For this operation mode, the driver uses a dedicated PCI
  device memory BAR, which is mapped with write-combine capability.
  (**低延迟队列 (LLQ) 模式，或称 "push-mode"：**
  在此模式下，驱动程序将发送描述符和数据包的前 96 字节直接推送到 ENA 设备
  内存空间。数据包负载的其余部分由设备获取。对于这种操作模式，驱动程序使用
  一个专用的 PCI 设备内存 BAR，它以写合并 (write-combine) 能力进行映射。)

  **Note that** not all ENA devices support LLQ, and this feature is negotiated
  with the device upon initialization. If the ENA device does not
  support LLQ mode, the driver falls back to the regular mode.
  (**请注意，** 并非所有 ENA 设备都支持 LLQ，该特性在初始化时与设备协商。
  如果 ENA 设备不支持 LLQ 模式，驱动程序将回退到常规模式。)

The Rx SQs support only the regular mode.
(Rx SQ 仅支持常规模式。)

The driver supports multi-queue for both Tx and Rx. This has various
benefits:
(驱动程序为 Tx 和 Rx 都支持多队列。这带来多种好处：)

- Reduced CPU/thread/process contention on a given Ethernet interface.
  (降低给定以太网接口上的 CPU/线程/进程争用。)
- Cache miss rate on completion is reduced, particularly for data
  cache lines that hold the sk_buff structures.
  (完成时的缓存未命中率降低，对于保存 sk_buff 结构的数据缓存行尤其如此。)
- Increased process-level parallelism when handling received packets.
  (处理接收数据包时提高进程级并行度。)
- Increased data cache hit rate, by steering kernel processing of
  packets to the CPU, where the application thread consuming the
  packet is running.
  (通过将内核的数据包处理引导到正在运行消费该数据包的应用线程的 CPU 上，
  提高数据缓存命中率。)
- In hardware interrupt re-direction.
  (硬件中断重定向。)

Interrupt Modes
(中断模式)
===============

The driver assigns a single MSI-X vector per queue pair (for both Tx
and Rx directions). The driver assigns an additional dedicated MSI-X vector
for management (for ACQ and AENQ).
(驱动程序为每个队列对分配单个 MSI-X 中断向量 (同时用于 Tx 和 Rx 方向)。
驱动程序还额外分配一个专用的 MSI-X 向量用于管理 (用于 ACQ 和 AENQ)。)

Management interrupt registration is performed when the Linux kernel
probes the adapter, and it is de-registered when the adapter is
removed. I/O queue interrupt registration is performed when the Linux
interface of the adapter is opened, and it is de-registered when the
interface is closed.
(管理中断在 Linux 内核探测适配器时注册，在适配器被移除时注销。I/O 队列
中断在适配器的 Linux 接口打开时注册，在接口关闭时注销。)

The management interrupt is named::
(管理中断的名称为：)

   ena-mgmnt@pci:<PCI domain:bus:slot.function>

and for each queue pair, an interrupt is named::
(而对于每个队列对，中断的名称为：)

   <interface name>-Tx-Rx-<queue index>

The ENA device operates in auto-mask and auto-clear interrupt
modes. That is, once MSI-X is delivered to the host, its Cause bit is
automatically cleared and the interrupt is masked. The interrupt is
unmasked by the driver after NAPI processing is complete.
(ENA 设备工作在自动屏蔽 (auto-mask) 和自动清除 (auto-clear) 中断模式下。
也就是说，一旦 MSI-X 中断被送达主机，其 Cause 位会被自动清除，且该中断被
屏蔽。驱动程序在 NAPI 处理完成后解除屏蔽。)

Interrupt Moderation
(中断调节)
====================

ENA driver and device can operate in conventional or adaptive interrupt
moderation mode.
(ENA 驱动程序和设备可以在常规或自适应中断调节模式下工作。)

**In conventional mode** the driver instructs device to postpone interrupt
posting according to static interrupt delay value. The interrupt delay
value can be configured through `ethtool(8)`. The following `ethtool`
parameters are supported by the driver: ``tx-usecs``, ``rx-usecs``
(**在常规模式下** ，驱动程序指示设备按照静态中断延迟值推迟中断的提交。
中断延迟值可以通过 `ethtool(8)` 配置。驱动程序支持以下 `ethtool` 参数：
``tx-usecs`` 、 ``rx-usecs``)

**In adaptive interrupt** moderation mode the interrupt delay value is
updated by the driver dynamically and adjusted every NAPI cycle
according to the traffic nature.
(**在自适应中断** 调节模式下，中断延迟值由驱动程序动态更新，并根据流量
特性在每个 NAPI 周期进行调整。)

Adaptive coalescing can be switched on/off through `ethtool(8)`'s
:code:`adaptive_rx on|off` parameter.
(自适应聚合可以通过 `ethtool(8)` 的 :code:`adaptive_rx on|off` 参数开启/
关闭。)

More information about Adaptive Interrupt Moderation (DIM) can be found in
Documentation/networking/net_dim.rst
(关于自适应中断调节 (DIM) 的更多信息，请参见
Documentation/networking/net_dim.rst)

.. _`RX copybreak`:

RX copybreak
(RX copybreak)
============

The rx_copybreak is initialized by default to ENA_DEFAULT_RX_COPYBREAK
and can be configured by the ETHTOOL_STUNABLE command of the
SIOCETHTOOL ioctl.
(rx_copybreak 默认初始化为 ENA_DEFAULT_RX_COPYBREAK，并可通过
SIOCETHTOOL ioctl 的 ETHTOOL_STUNABLE 命令进行配置。)

This option controls the maximum packet length for which the RX
descriptor it was received on would be recycled. When a packet smaller
than RX copybreak bytes is received, it is copied into a new memory
buffer and the RX descriptor is returned to HW.
(此选项控制 RX 描述符可被回收的最大数据包长度。当收到小于 RX copybreak
字节数的数据包时，它会先被复制到新的内存缓冲区中，然后 RX 描述符被归还给
硬件。)

.. _`PHC`:

PTP Hardware Clock (PHC)
(PTP 硬件时钟 (PHC))
========================
.. _`ptp-userspace-api`: https://docs.kernel.org/driver-api/ptp.html#ptp-hardware-clock-user-space-api
.. _`testptp`: https://elixir.bootlin.com/linux/latest/source/tools/testing/selftests/ptp/testptp.c

ENA Linux driver supports PTP hardware clock providing timestamp reference to achieve nanosecond resolution.
(ENA Linux 驱动程序支持 PTP 硬件时钟，提供时间戳基准以达到纳秒级分辨率。)

**PHC support**
(PHC 支持)

PHC depends on the PTP module, which needs to be either loaded as a module or compiled into the kernel.
(PHC 依赖于 PTP 模块，该模块需要以模块形式加载或编译进内核。)

Verify if the PTP module is present:
(验证 PTP 模块是否存在：)

.. code-block:: shell

  grep -w '^CONFIG_PTP_1588_CLOCK=[ym]' /boot/config-`uname -r`

- If no output is provided, the ENA driver cannot be loaded with PHC support.
  (如果没有输出，则 ENA 驱动程序无法在支持 PHC 的情况下加载。)

**PHC activation**
(PHC 激活)

The feature is turned off by default, in order to turn the feature on, the ENA driver
can be loaded in the following way:
(该特性默认关闭。要开启该特性，可以按以下方式加载 ENA 驱动程序：)

- devlink:
  (devlink:)

.. code-block:: shell

  sudo devlink dev param set pci/<domain:bus:slot.function> name enable_phc value true cmode driverinit
  sudo devlink dev reload pci/<domain:bus:slot.function>
  # for example:
  sudo devlink dev param set pci/0000:00:06.0 name enable_phc value true cmode driverinit
  sudo devlink dev reload pci/0000:00:06.0

All available PTP clock sources can be tracked here:
(所有可用的 PTP 时钟源都可以在这里查看：)

.. code-block:: shell

  ls /sys/class/ptp

PHC support and capabilities can be verified using ethtool:
(PHC 支持与能力可以使用 ethtool 验证：)

.. code-block:: shell

  ethtool -T <interface>

**PHC timestamp**
(PHC 时间戳)

To retrieve PHC timestamp, use `ptp-userspace-api`_, usage example using `testptp`_:
(要获取 PHC 时间戳，请使用 `ptp-userspace-api`_ ，使用 `testptp`_ 的示例：)

.. code-block:: shell

  testptp -d /dev/ptp$(ethtool -T <interface> | awk '/PTP Hardware Clock:/ {print $NF}') -k 1

PHC get time requests should be within reasonable bounds,
avoid excessive utilization to ensure optimal performance and efficiency.
The ENA device restricts the frequency of PHC get time requests to a maximum
of 125 requests per second. If this limit is surpassed, the get time request
will fail, leading to an increment in the phc_err_ts statistic.
(PHC 获取时间请求应保持在合理范围内，避免过度使用以确保最佳性能和效率。
ENA 设备将 PHC 获取时间请求的频率限制为每秒最多 125 次。如果超过此限制，
获取时间请求将失败，导致 phc_err_ts 统计值增加。)

**PHC statistics**
(PHC 统计)

PHC can be monitored using debugfs (if mounted):
(可以使用 debugfs 监控 PHC (如果已挂载)：)

.. code-block:: shell

  sudo cat /sys/kernel/debug/<domain:bus:slot.function>/phc_stats

  # for example:
  sudo cat /sys/kernel/debug/0000:00:06.0/phc_stats

PHC errors must remain below 1% of all PHC requests to maintain the desired level of accuracy and reliability
(PHC 错误必须保持在所有 PHC 请求的 1% 以下，以维持预期的精度和可靠性水平)

=================   ======================================================
**phc_cnt**         | Number of successful retrieved timestamps (below expire timeout).
                    | (成功获取时间戳的次数 (低于 expire 超时)。)
**phc_exp**         | Number of expired retrieved timestamps (above expire timeout).
                    | (已过期的获取时间戳次数 (超过 expire 超时)。)
**phc_skp**         | Number of skipped get time attempts (during block period).
                    | (被跳过的获取时间尝试次数 (阻塞期间)。)
**phc_err_dv**      | Number of failed get time attempts due to device errors (entering into block state).
                    | (因设备错误而失败的获取时间尝试次数 (进入阻塞状态)。)
**phc_err_ts**      | Number of failed get time attempts due to timestamp errors (entering into block state),
                    | This occurs if driver exceeded the request limit or device received an invalid timestamp.
                    | (因时间戳错误而失败的获取时间尝试次数 (进入阻塞状态)，
                    | 若驱动程序超出请求限制或设备收到无效时间戳，就会发生这种情况。)
=================   ======================================================

PHC timeouts:
(PHC 超时：)

=================   ======================================================
**expire**          | Max time for a valid timestamp retrieval, passing this threshold will fail
                    | the get time request and block new requests until block timeout.
                    | (有效获取时间戳的最长时间，超过此阈值将使获取时间请求失败，
                    | 并阻塞新请求直到 block 超时结束。)
**block**           | Blocking period starts once get time request expires or fails,
                    | all get time requests during block period will be skipped.
                    | (一旦获取时间请求过期或失败，阻塞期即开始，
                    | 阻塞期间的所有获取时间请求都会被跳过。)
=================   ======================================================

Statistics
(统计)
==========

The user can obtain ENA device and driver statistics using `ethtool`.
The driver can collect regular or extended statistics (including
per-queue stats) from the device.
(用户可以使用 `ethtool` 获取 ENA 设备和驱动程序的统计信息。驱动程序可以从
设备收集常规或扩展统计信息 (包括每队列统计)。)

In addition the driver logs the stats to syslog upon device reset.
(此外，驱动程序会在设备复位时将统计信息记录到 syslog 中。)

On supported instance types, the statistics will also include the
ENA Express data (fields prefixed with `ena_srd`). For a complete
documentation of ENA Express data refer to
https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ena-express.html#ena-express-monitor
(在受支持的实例类型上，统计信息还将包含 ENA Express 数据 (以 `ena_srd` 为
前缀的字段)。有关 ENA Express 数据的完整文档，请参阅
https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/ena-express.html#ena-express-monitor)

MTU
===
(MTU)

The driver supports an arbitrarily large MTU with a maximum that is
negotiated with the device. The driver configures MTU using the
SetFeature command (ENA_ADMIN_MTU property). The user can change MTU
via `ip(8)` and similar legacy tools.
(驱动程序支持任意大的 MTU，其最大值与设备协商确定。驱动程序使用
SetFeature 命令 (ENA_ADMIN_MTU 属性) 配置 MTU。用户可以通过 `ip(8)` 和
类似的传统工具更改 MTU。)

Stateless Offloads
(无状态卸载)
==================

The ENA driver supports:
(ENA 驱动程序支持：)

- IPv4 header checksum offload
  (IPv4 头校验和卸载)
- TCP/UDP over IPv4/IPv6 checksum offloads
  (IPv4/IPv6 上的 TCP/UDP 校验和卸载)

RSS
===
(RSS)

- The ENA device supports RSS that allows flexible Rx traffic
  steering.
  (ENA 设备支持 RSS，可实现灵活的 Rx 流量引导。)
- Toeplitz and CRC32 hash functions are supported.
  (支持 Toeplitz 和 CRC32 哈希函数。)
- Different combinations of L2/L3/L4 fields can be configured as
  inputs for hash functions.
  (可以配置 L2/L3/L4 字段的不同组合作为哈希函数的输入。)
- The driver configures RSS settings using the AQ SetFeature command
  (ENA_ADMIN_RSS_HASH_FUNCTION, ENA_ADMIN_RSS_HASH_INPUT and
  ENA_ADMIN_RSS_INDIRECTION_TABLE_CONFIG properties).
  (驱动程序使用 AQ SetFeature 命令 (ENA_ADMIN_RSS_HASH_FUNCTION、
  ENA_ADMIN_RSS_HASH_INPUT 和
  ENA_ADMIN_RSS_INDIRECTION_TABLE_CONFIG 属性) 配置 RSS 设置。)
- If the NETIF_F_RXHASH flag is set, the 32-bit result of the hash
  function delivered in the Rx CQ descriptor is set in the received
  SKB.
  (如果设置了 NETIF_F_RXHASH 标志，则在 Rx CQ 描述符中交付的哈希函数
  32 位结果会被设置到接收的 SKB 中。)
- The user can provide a hash key, hash function, and configure the
  indirection table through `ethtool(8)`.
  (用户可以通过 `ethtool(8)` 提供哈希密钥、哈希函数并配置间接表。)

DEVLINK SUPPORT
(DEVLINK 支持)
===============
.. _`devlink`: https://www.kernel.org/doc/html/latest/networking/devlink/index.html

`devlink`_ supports reloading the driver and initiating re-negotiation with the ENA device
(`devlink`_ 支持重新加载驱动程序并重新发起与 ENA 设备的协商)

.. code-block:: shell

  sudo devlink dev reload pci/<domain:bus:slot.function>
  # for example:
  sudo devlink dev reload pci/0000:00:06.0

DATA PATH
(数据路径)
=========

Tx
--
(Tx)

:code:`ena_start_xmit()` is called by the stack. This function does the following:
(协议栈会调用 :code:`ena_start_xmit()`。该函数执行以下操作：)

- Maps data buffers (``skb->data`` and frags).
  (映射数据缓冲区 (``skb->data`` 和 frags)。)
- Populates ``ena_buf`` for the push buffer (if the driver and device are
  in push mode).
  (为 push 缓冲区填充 ``ena_buf`` (如果驱动程序和设备处于 push 模式)。)
- Prepares ENA bufs for the remaining frags.
  (为其余 frags 准备 ENA buf。)
- Allocates a new request ID from the empty ``req_id`` ring. The request
  ID is the index of the packet in the Tx info. This is used for
  out-of-order Tx completions.
  (从空闲的 ``req_id`` 环中分配一个新的请求 ID。请求 ID 是该数据包在 Tx
  info 中的索引。它用于乱序 (out-of-order) 的 Tx 完成。)
- Adds the packet to the proper place in the Tx ring.
  (将数据包添加到 Tx 环中的适当位置。)
- Calls :code:`ena_com_prepare_tx()`, an ENA communication layer that converts
  the ``ena_bufs`` to ENA descriptors (and adds meta ENA descriptors as
  needed).
  (调用 :code:`ena_com_prepare_tx()` ，这是 ENA 通信层，它将 ``ena_bufs``
  转换为 ENA 描述符 (并根据需要添加元 ENA 描述符)。)

  * This function also copies the ENA descriptors and the push buffer
    to the Device memory space (if in push mode).
    (该函数还会将 ENA 描述符和 push 缓冲区复制到设备内存空间 (如果处于
    push 模式)。)

- Writes a doorbell to the ENA device.
  (向 ENA 设备写入门铃 (doorbell)。)
- When the ENA device finishes sending the packet, a completion
  interrupt is raised.
  (当 ENA 设备完成数据包的发送时，会产生一个完成中断。)
- The interrupt handler schedules NAPI.
  (中断处理程序调度 NAPI。)
- The :code:`ena_clean_tx_irq()` function is called. This function handles the
  completion descriptors generated by the ENA, with a single
  completion descriptor per completed packet.
  (:code:`ena_clean_tx_irq()` 函数被调用。该函数处理 ENA 生成的完成描述符，
  每个已完成的数据包对应一个完成描述符。)

  * ``req_id`` is retrieved from the completion descriptor. The ``tx_info`` of
    the packet is retrieved via the ``req_id``. The data buffers are
    unmapped and ``req_id`` is returned to the empty ``req_id`` ring.
    (从完成描述符中取出 ``req_id`` 。数据包的 ``tx_info`` 通过 ``req_id``
    取回。数据缓冲区被解除映射， ``req_id`` 被归还到空闲的 ``req_id``
    环中。)
  * The function stops when the completion descriptors are completed or
    the budget is reached.
    (当完成描述符处理完毕或达到预算 (budget) 时，该函数停止。)

Rx
--
(Rx)

- When a packet is received from the ENA device.
  (当从 ENA 设备收到一个数据包时。)
- The interrupt handler schedules NAPI.
  (中断处理程序调度 NAPI。)
- The :code:`ena_clean_rx_irq()` function is called. This function calls
  :code:`ena_com_rx_pkt()`, an ENA communication layer function, which returns the
  number of descriptors used for a new packet, and zero if
  no new packet is found.
  (:code:`ena_clean_rx_irq()` 函数被调用。该函数调用
  :code:`ena_com_rx_pkt()` (一个 ENA 通信层函数)，它返回用于新数据包的
  描述符数量，如果没有找到新数据包则返回零。)
- :code:`ena_rx_skb()` checks packet length:
  (:code:`ena_rx_skb()` 检查数据包长度：)

  * If the packet is small (len < rx_copybreak), the driver allocates
    a SKB for the new packet, and copies the packet payload into the
    SKB data buffer.
    (如果数据包较小 (len < rx_copybreak)，驱动程序为新数据包分配一个 SKB，
    并将数据包负载复制到 SKB 数据缓冲区中。)

    - In this way the original data buffer is not passed to the stack
      and is reused for future Rx packets.
      (这样，原始数据缓冲区不会被传递给协议栈，而是被重用于后续的 Rx
      数据包。)

  * Otherwise the function unmaps the Rx buffer, sets the first
    descriptor as `skb`'s linear part and the other descriptors as the
    `skb`'s frags.
    (否则，该函数解除 Rx 缓冲区的映射，将第一个描述符设置为 `skb` 的线性
    部分，将其他描述符设置为 `skb` 的 frags。)

- The new SKB is updated with the necessary information (protocol,
  checksum hw verify result, etc), and then passed to the network
  stack, using the NAPI interface function :code:`napi_gro_receive()`.
  (新的 SKB 被更新以包含必要的信息 (协议、校验和硬件验证结果等)，然后通过
  NAPI 接口函数 :code:`napi_gro_receive()` 传递给网络协议栈。)

Dynamic RX Buffers (DRB)
(动态 RX 缓冲区 (DRB))
------------------------

Each RX descriptor in the RX ring is a single memory page (which is either 4KB
or 16KB long depending on system's configurations).
To reduce the memory allocations required when dealing with a high rate of small
packets, the driver tries to reuse the remaining RX descriptor's space if more
than 2KB of this page remain unused.
(RX 环中的每个 RX 描述符都是一个单独的内存页 (根据系统配置，其长度为 4KB 或
16KB)。为了减少处理高速率小包时所需的内存分配，如果该页有超过 2KB 的空间
尚未使用，驱动程序会尝试重用 RX 描述符的剩余空间。)

A simple example of this mechanism is the following sequence of events:
(该机制的一个简单示例是以下事件序列：)

::

        1. Driver allocates page-sized RX buffer and passes it to hardware
                +----------------------+
                |4KB RX Buffer         |
                +----------------------+

        2. A 300Bytes packet is received on this buffer

        3. The driver increases the ref count on this page and returns it back to
           HW as an RX buffer of size 4KB - 300Bytes = 3796 Bytes
               +----+--------------------+
               |****|3796 Bytes RX Buffer|
               +----+--------------------+

This mechanism isn't used when an XDP program is loaded, or when the
RX packet is less than rx_copybreak bytes (in which case the packet is
copied out of the RX buffer into the linear part of a new skb allocated
for it and the RX buffer remains the same size, see `RX copybreak`_).
(当已加载 XDP 程序，或 RX 数据包小于 rx_copybreak 字节时，不会使用此机制：
在后一种情况下，数据包会从 RX 缓冲区复制到为其新分配的 skb 的线性部分，
且 RX 缓冲区保持原大小不变，参见 `RX copybreak`_ 。)
