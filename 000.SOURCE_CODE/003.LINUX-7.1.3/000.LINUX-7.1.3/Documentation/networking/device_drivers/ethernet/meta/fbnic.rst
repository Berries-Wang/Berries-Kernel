.. SPDX-License-Identifier: GPL-2.0+

=====================================
Meta Platforms Host Network Interface
=====================================

(Meta Platforms 主机网络接口)

Firmware Versions
(固件版本)
-----------------

fbnic has three components stored on the flash which are provided in one PLDM
image:
(固件包含三个存储在闪存中的组件，由一个 PLDM 镜像提供：)

1. fw - The control firmware used to view and modify firmware settings, request
   firmware actions, and retrieve firmware counters outside of the data path.
   This is the firmware which fbnic_fw.c interacts with.
   (fw - 控制固件，用于查看和修改固件设置、请求固件操作，以及获取数据路径之外的固件计数器。fbnic_fw.c 与该固件交互。)
2. bootloader - The firmware which validate firmware security and control basic
   operations including loading and updating the firmware. This is also known
   as the cmrt firmware.
   (bootloader - 校验固件安全性并控制基本操作（包括加载和更新固件）的固件。它也被称为 cmrt 固件。)
3. undi - This is the UEFI driver which is based on the Linux driver.
   (undi - 基于 Linux 驱动实现的 UEFI 驱动。)

fbnic stores two copies of these three components on flash. This allows fbnic
to fall back to an older version of firmware automatically in case firmware
fails to boot. Version information for both is provided as running and stored.
The undi is only provided in stored as it is not actively running once the Linux
driver takes over.
(这三个组件在闪存中保存两份副本，这样在固件启动失败时 fbnic 可以自动回退到较旧的固件版本。两者都提供 running（运行中）和 stored（已存储）两种版本信息。Linux 驱动接管后 undi 便不再活动运行，因此只提供 stored 版本信息。)

devlink dev info provides version information for all three components. In
addition to the version the hg commit hash of the build is included as a
separate entry.
(devlink dev info 提供所有三个组件的版本信息。除版本号外，构建所用的 hg 提交哈希也作为单独条目提供。)

Configuration
(配置)
-------------

Ringparams (ethtool -g / -G)
(环形缓冲区参数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

fbnic has two submission (host -> device) rings for every completion
(device -> host) ring. The three ring objects together form a single
"queue" as used by higher layer software (a Rx, or a Tx queue).
(每个完成（device -> host）环对应两个提交（host -> device）环。这三个环对象共同构成高层软件所使用的一个"队列"（一个 Rx 或 Tx 队列）。)

For Rx the two submission rings are used to pass empty pages to the NIC.
Ring 0 is the Header Page Queue (HPQ), NIC will use its pages to place
L2-L4 headers (or full frames if frame is not header-data split).
Ring 1 is the Payload Page Queue (PPQ) and used for packet payloads.
The completion ring is used to receive packet notifications / metadata.
ethtool ``rx`` ringparam maps to the size of the completion ring,
``rx-mini`` to the HPQ, and ``rx-jumbo`` to the PPQ.
(对于 Rx，两个提交环用于向网卡传递空页。环 0 是头部页队列 (HPQ)，网卡使用其中的页存放 L2-L4 头部（若报文未做头数据分离则存放完整帧）。环 1 是载荷页队列 (PPQ)，用于存放数据包载荷。完成环用于接收数据包通知/元数据。ethtool 的 ``rx`` 环形参数对应完成环的大小，``rx-mini`` 对应 HPQ，``rx-jumbo`` 对应 PPQ。)

For Tx both submission rings can be used to submit packets, the completion
ring carries notifications for both. fbnic uses one of the submission
rings for normal traffic from the stack and the second one for XDP frames.
ethtool ``tx`` ringparam controls both the size of the submission rings
and the completion ring.
(对于 Tx，两个提交环都可用于提交数据包，完成环为两者承载通知。fbnic 将其中一个提交环用于来自协议栈的正常流量，另一个用于 XDP 帧。ethtool 的 ``tx`` 环形参数同时控制提交环和完成环的大小。)

Every single entry on the HPQ and PPQ (``rx-mini``, ``rx-jumbo``)
corresponds to 4kB of allocated memory, while entries on the remaining
rings are in units of descriptors (8B). The ideal ratio of submission
and completion ring sizes will depend on the workload, as for small packets
multiple packets will fit into a single page.
(HPQ 和 PPQ（``rx-mini``、``rx-jumbo``）上的每个条目对应 4kB 已分配内存，而其余环上的条目以描述符为单位（8B）。提交环与完成环大小的理想比例取决于工作负载，因为对于小数据包，单个页内可容纳多个数据包。)

Upgrading Firmware
(升级固件)
------------------

fbnic supports updating firmware using signed PLDM images with devlink dev
flash. PLDM images are written into the flash. Flashing does not interrupt
the operation of the device.
(fbnic 支持通过 devlink dev flash 使用带签名的 PLDM 镜像更新固件。PLDM 镜像会被写入闪存。烧写过程不会中断设备的运行。)

On host boot the latest UEFI driver is always used, no explicit activation
is required. Firmware activation is required to run new control firmware. cmrt
firmware can only be activated by power cycling the NIC.
(主机启动时始终使用最新的 UEFI 驱动，无需显式激活。运行新的控制固件需要固件激活。cmrt 固件只能通过给网卡断电重启来激活。)

Health reporters
(健康报告器)
----------------

fw reporter
(fw 报告器)
~~~~~~~~~~~

The ``fw`` health reporter tracks FW crashes. Dumping the reporter will
show the core dump of the most recent FW crash, and if no FW crash has
happened since power cycle - a snapshot of the FW memory. Diagnose callback
shows FW uptime based on the most recently received heartbeat message
(the crashes are detected by checking if uptime goes down).
(``fw`` 健康报告器跟踪固件崩溃。转储该报告器会显示最近一次固件崩溃的核心转储；如果自断电重启以来没有发生过固件崩溃，则显示固件内存的快照。诊断回调基于最近收到的心跳消息显示固件运行时长（崩溃是通过检查运行时长是否回退来检测的）。)

otp reporter
(otp 报告器)
~~~~~~~~~~~~

OTP memory ("fuses") are used for secure boot and anti-rollback
protection. The OTP memory is ECC protected, ECC errors indicate
either manufacturing defect or part deteriorating with age.
(OTP 存储器（"熔丝"）用于安全启动和防回滚保护。OTP 存储器受 ECC 保护，ECC 错误表明存在制造缺陷或部件随时间老化。)

Statistics
(统计)
----------

TX MAC Interface
(TX MAC 接口)
~~~~~~~~~~~~~~~~

 - ``ptp_illegal_req``: packets sent to the NIC with PTP request bit set but routed to BMC/FW
   (发送到网卡且置位了 PTP 请求位、却被路由到 BMC/FW 的数据包。)
 - ``ptp_good_ts``: packets successfully routed to MAC with PTP request bit set
   (置位 PTP 请求位且被成功路由到 MAC 的数据包。)
 - ``ptp_bad_ts``: packets destined for MAC with PTP request bit set but aborted because of some error (e.g., DMA read error)
   (目的地为 MAC 且置位 PTP 请求位、但因某种错误（如 DMA 读取错误）而中止的数据包。)

TX Extension (TEI) Interface (TTI)
(TX 扩展 (TEI) 接口 (TTI))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 - ``tti_cm_drop``: control messages dropped at the TX Extension (TEI) Interface because of credit starvation
   (在 TX 扩展 (TEI) 接口处因信用额度不足而丢弃的控制消息。)
 - ``tti_frame_drop``: packets dropped at the TX Extension (TEI) Interface because of credit starvation
   (在 TX 扩展 (TEI) 接口处因信用额度不足而丢弃的数据包。)
 - ``tti_tbi_drop``: packets dropped at the TX BMC Interface (TBI) because of credit starvation
   (在 TX BMC 接口 (TBI) 处因信用额度不足而丢弃的数据包。)

RXB (RX Buffer) Enqueue
(RXB（RX 缓冲区）入队)
~~~~~~~~~~~~~~~~~~~~~~~

 - ``rxb_integrity_err[i]``: frames enqueued with integrity errors (e.g., multi-bit ECC errors) on RXB input i
   (在 RXB 输入 i 上入队时带有完整性错误（如多位 ECC 错误）的帧。)
 - ``rxb_mac_err[i]``: frames enqueued with MAC end-of-frame errors (e.g., bad FCS) on RXB input i
   (在 RXB 输入 i 上入队时带有 MAC 帧结束错误（如 FCS 不良）的帧。)
 - ``rxb_parser_err[i]``: frames experienced RPC parser errors
   (经历 RPC 解析器错误的帧。)
 - ``rxb_frm_err[i]``: frames experienced signaling errors (e.g., missing end-of-packet/start-of-packet) on RXB input i
   (在 RXB 输入 i 上经历信号错误（如缺少包结束/包开始）的帧。)
 - ``rxb_drbo[i]_frames``: frames received at RXB input i
   (在 RXB 输入 i 收到的帧数。)
 - ``rxb_drbo[i]_bytes``: bytes received at RXB input i
   (在 RXB 输入 i 收到的字节数。)

RXB (RX Buffer) FIFO
(RXB 缓冲区 FIFO)
~~~~~~~~~~~~~~~~~~~~

 - ``rxb_fifo[i]_drop``: transitions into the drop state on RXB pool i
   (RXB 池 i 进入丢弃状态的次数。)
 - ``rxb_fifo[i]_dropped_frames``: frames dropped on RXB pool i
   (在 RXB 池 i 上丢弃的帧数。)
 - ``rxb_fifo[i]_ecn``: transitions into the ECN mark state on RXB pool i
   (RXB 池 i 进入 ECN 标记状态的次数。)
 - ``rxb_fifo[i]_level``: current occupancy of RXB pool i
   (RXB 池 i 的当前占用程度。)

RXB (RX Buffer) Dequeue
(RXB（RX 缓冲区）出队)
~~~~~~~~~~~~~~~~~~~~~~~

   - ``rxb_intf[i]_frames``: frames sent to the output i
     (发送到输出 i 的帧数。)
   - ``rxb_intf[i]_bytes``: bytes sent to the output i
     (发送到输出 i 的字节数。)
   - ``rxb_pbuf[i]_frames``: frames sent to output i from the perspective of internal packet buffer
     (从内部数据包缓冲区的角度统计的发送到输出 i 的帧数。)
   - ``rxb_pbuf[i]_bytes``: bytes sent to output i from the perspective of internal packet buffer
     (从内部数据包缓冲区的角度统计的发送到输出 i 的字节数。)

RPC (Rx parser)
(RPC 解析器)
~~~~~~~~~~~~~~~

 - ``rpc_unkn_etype``: frames containing unknown EtherType
   (包含未知 EtherType 的帧。)
 - ``rpc_unkn_ext_hdr``: frames containing unknown IPv6 extension header
   (包含未知 IPv6 扩展头的帧。)
 - ``rpc_ipv4_frag``: frames containing IPv4 fragment
   (包含 IPv4 分片的帧。)
 - ``rpc_ipv6_frag``: frames containing IPv6 fragment
   (包含 IPv6 分片的帧。)
 - ``rpc_ipv4_esp``: frames with IPv4 ESP encapsulation
   (采用 IPv4 ESP 封装的帧。)
 - ``rpc_ipv6_esp``: frames with IPv6 ESP encapsulation
   (采用 IPv6 ESP 封装的帧。)
 - ``rpc_tcp_opt_err``: frames which encountered TCP option parsing error
   (遇到 TCP 选项解析错误的帧。)
 - ``rpc_out_of_hdr_err``: frames where header was larger than parsable region
   (头部超出可解析区域的帧。)
 - ``ovr_size_err``: oversized frames
   (超长帧。)

Hardware Queues
(硬件队列)
~~~~~~~~~~~~~~~

1. RX DMA Engine:
   (1. RX DMA 引擎：)

 - ``rde_[i]_pkt_err``: packets with MAC EOP, RPC parser, RXB truncation, or RDE frame truncation errors. These error are flagged in the packet metadata because of cut-through support but the actual drop happens once PCIE/RDE is reached.
   (带有 MAC EOP、RPC 解析器、RXB 截断或 RDE 帧截断错误的数据包。由于支持直通（cut-through），这些错误会在数据包元数据中标记，但实际的丢弃发生在到达 PCIE/RDE 时。)
 - ``rde_[i]_pkt_cq_drop``: packets dropped because RCQ is full
   (因 RCQ 已满而丢弃的数据包。)
 - ``rde_[i]_pkt_bdq_drop``: packets dropped because HPQ or PPQ ran out of host buffer
   (因 HPQ 或 PPQ 耗尽主机缓冲区而丢弃的数据包。)

PCIe
~~~~

The fbnic driver exposes PCIe hardware performance statistics through debugfs
(``pcie_stats``). These statistics provide insights into PCIe transaction
behavior and potential performance bottlenecks.
(fbnic 驱动通过 debugfs（``pcie_stats`` ）公开 PCIe 硬件性能统计信息。这些统计信息有助于洞察 PCIe 事务行为和潜在的性能瓶颈。)

1. PCIe Transaction Counters:
   (1. PCIe 事务计数器：)

   These counters track PCIe transaction activity:
   (这些计数器跟踪 PCIe 事务活动：)

        - ``pcie_ob_rd_tlp``: Outbound read Transaction Layer Packets count
          (出站读事务层包 (TLP) 计数。)
        - ``pcie_ob_rd_dword``: DWORDs transferred in outbound read transactions
          (出站读事务中传输的 DWORD 数。)
        - ``pcie_ob_wr_tlp``: Outbound write Transaction Layer Packets count
          (出站写事务层包 (TLP) 计数。)
        - ``pcie_ob_wr_dword``: DWORDs transferred in outbound write
	  transactions
          (出站写事务中传输的 DWORD 数。)
        - ``pcie_ob_cpl_tlp``: Outbound completion TLP count
          (出站完成 TLP 计数。)
        - ``pcie_ob_cpl_dword``: DWORDs transferred in outbound completion TLPs
          (出站完成 TLP 中传输的 DWORD 数。)

2. PCIe Resource Monitoring:
   (2. PCIe 资源监控：)

   These counters indicate PCIe resource exhaustion events:
   (这些计数器指示 PCIe 资源耗尽事件：)

        - ``pcie_ob_rd_no_tag``: Read requests dropped due to tag unavailability
          (因标签不可用而丢弃的读请求。)
        - ``pcie_ob_rd_no_cpl_cred``: Read requests dropped due to completion
	  credit exhaustion
          (因完成信用额度耗尽而丢弃的读请求。)
        - ``pcie_ob_rd_no_np_cred``: Read requests dropped due to non-posted
	  credit exhaustion
          (因非 posted 信用额度耗尽而丢弃的读请求。)

XDP Length Error:
(XDP 长度错误：)
~~~~~~~~~~~~~~~~~

For XDP programs without frags support, fbnic tries to make sure that MTU fits
into a single buffer. If an oversized frame is received and gets fragmented,
it is dropped and the following netlink counters are updated
(对于不支持 frags 的 XDP 程序，fbnic 会尽量确保 MTU 能容纳在单个缓冲区中。如果收到超长帧并被分片，该帧会被丢弃，同时更新以下 netlink 计数器：)

   - ``rx-length``: number of frames dropped due to lack of fragmentation
     support in the attached XDP program
     (因所挂载的 XDP 程序不支持分片而丢弃的帧数。)
   - ``rx-errors``: total number of packets with errors received on the interface
     (接口上收到的出错数据包总数。)
