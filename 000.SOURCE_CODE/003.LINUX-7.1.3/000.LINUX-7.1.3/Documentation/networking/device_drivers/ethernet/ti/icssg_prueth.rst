.. SPDX-License-Identifier: GPL-2.0

==============================================
Texas Instruments ICSSG PRUETH ethernet driver
==============================================

(德州仪器 ICSSG PRUETH 以太网驱动)

:Version: 1.0
          (版本: 1.0)

ICSSG Firmware
==============
(ICSSG 固件)

Every ICSSG core has two Programmable Real-Time Unit(PRUs), two auxiliary
Real-Time Transfer Unit (RTUs), and two Transmit Real-Time Transfer Units
(TX_PRUs). Each one of these runs its own firmware. The firmwares combnined are
referred as ICSSG Firmware.
(每个 ICSSG 核心都有两个可编程实时单元 (PRU)、两个辅助实时传输单元 (RTU) 和两个发送实时传输单元 (TX_PRU)。它们各自运行自己的固件。这些固件合起来统称为 ICSSG 固件。)

Firmware Statistics
===================
(固件统计信息)

The ICSSG firmware maintains certain statistics which are dumped by the driver
via ``ethtool -S <interface>``
(ICSSG 固件维护一些统计信息，驱动通过 ``ethtool -S <interface>`` 将其转储出来)

These statistics are as follows,
(这些统计信息如下：)

 - ``FW_RTU_PKT_DROP``: Diagnostic error counter which increments when RTU drops a locally injected packet due to port being disabled or rule violation.
   (诊断错误计数器，当 RTU 因端口被禁用或违反规则而丢弃本地注入的数据包时递增。)
 - ``FW_Q0_OVERFLOW``: TX overflow counter for queue0
   (queue0 的 TX 溢出计数器)
 - ``FW_Q1_OVERFLOW``: TX overflow counter for queue1
   (queue1 的 TX 溢出计数器)
 - ``FW_Q2_OVERFLOW``: TX overflow counter for queue2
   (queue2 的 TX 溢出计数器)
 - ``FW_Q3_OVERFLOW``: TX overflow counter for queue3
   (queue3 的 TX 溢出计数器)
 - ``FW_Q4_OVERFLOW``: TX overflow counter for queue4
   (queue4 的 TX 溢出计数器)
 - ``FW_Q5_OVERFLOW``: TX overflow counter for queue5
   (queue5 的 TX 溢出计数器)
 - ``FW_Q6_OVERFLOW``: TX overflow counter for queue6
   (queue6 的 TX 溢出计数器)
 - ``FW_Q7_OVERFLOW``: TX overflow counter for queue7
   (queue7 的 TX 溢出计数器)
 - ``FW_DROPPED_PKT``: This counter is incremented when a packet is dropped at PRU because of rule violation.
   (当数据包在 PRU 处因违反规则而被丢弃时，该计数器递增。)
 - ``FW_RX_ERROR``: Incremented if there was a CRC error or Min/Max frame error at PRU
   (当 PRU 处出现 CRC 错误或最小/最大帧错误时递增)
 - ``FW_RX_DS_INVALID``: Incremented when RTU detects Data Status invalid condition
   (当 RTU 检测到数据状态 (Data Status) 无效时递增)
 - ``FW_TX_DROPPED_PACKET``: Counter for packets dropped via TX Port
   (经 TX 端口丢弃的数据包计数器)
 - ``FW_TX_TS_DROPPED_PACKET``: Counter for packets with TS flag dropped via TX Port
   (经 TX 端口丢弃的带 TS 标志的数据包计数器)
 - ``FW_INF_PORT_DISABLED``: Incremented when RX frame is dropped due to port being disabled
   (当 RX 帧因端口被禁用而被丢弃时递增)
 - ``FW_INF_SAV``: Incremented when RX frame is dropped due to Source Address violation
   (当 RX 帧因源地址违规而被丢弃时递增)
 - ``FW_INF_SA_DL``: Incremented when RX frame is dropped due to Source Address being in the denylist
   (当 RX 帧因源地址在拒绝列表中而被丢弃时递增)
 - ``FW_INF_PORT_BLOCKED``: Incremented when RX frame is dropped due to port being blocked and frame being a special frame
   (当 RX 帧因端口被阻塞且该帧为特殊帧而被丢弃时递增)
 - ``FW_INF_DROP_TAGGED`` : Incremented when RX frame is dropped for being tagged
   (当 RX 帧因带标签 (tagged) 而被丢弃时递增)
 - ``FW_INF_DROP_PRIOTAGGED``: Incremented when RX frame is dropped for being priority tagged
   (当 RX 帧因优先级标签 (priority tagged) 而被丢弃时递增)
 - ``FW_INF_DROP_NOTAG``: Incremented when RX frame is dropped for being untagged
   (当 RX 帧因不带标签 (untagged) 而被丢弃时递增)
 - ``FW_INF_DROP_NOTMEMBER``: Incremented when RX frame is dropped for port not being member of VLAN
   (当 RX 帧因端口不是该 VLAN 成员而被丢弃时递增)
 - ``FW_RX_EOF_SHORT_FRMERR``: Incremented if End Of Frame (EOF) task is scheduled without seeing RX_B1
   (如果在未看到 RX_B1 的情况下就调度了帧结束 (EOF) 任务则递增)
 - ``FW_RX_B0_DROP_EARLY_EOF``: Incremented when frame is dropped due to Early EOF
   (当帧因过早 EOF (Early EOF) 而被丢弃时递增)
 - ``FW_TX_JUMBO_FRM_CUTOFF``: Incremented when frame is cut off to prevent packet size > 2000 Bytes
   (当为防止数据包大小超过 2000 字节而截断帧时递增)
 - ``FW_RX_EXP_FRAG_Q_DROP``: Incremented when express frame is received in the same queue as the previous fragment
   (当快速帧 (express frame) 与前一个分片在同一个队列中被接收时递增)
 - ``FW_RX_FIFO_OVERRUN``: RX fifo overrun counter
   (RX FIFO 溢出计数器)
 - ``FW_CUT_THR_PKT``: Incremented when a packet is forwarded using Cut-Through forwarding method
   (当数据包采用直通 (Cut-Through) 方式转发时递增)
 - ``FW_HOST_RX_PKT_CNT``: Number of valid packets sent by Rx PRU to Host on PSI
   (Rx PRU 通过 PSI 发送到主机的有效数据包数量)
 - ``FW_HOST_TX_PKT_CNT``: Number of valid packets copied by RTU0 to Tx queues
   (RTU0 复制到 Tx 队列的有效数据包数量)
 - ``FW_HOST_EGRESS_Q_PRE_OVERFLOW``: Host Egress Q (Pre-emptible) Overflow Counter
   (主机出口队列 (可抢占) 溢出计数器)
 - ``FW_HOST_EGRESS_Q_EXP_OVERFLOW``: Host Egress Q (Pre-emptible) Overflow Counter
   (主机出口队列 (可抢占) 溢出计数器)
