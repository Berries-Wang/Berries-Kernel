.. SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB
.. include:: <isonum.txt>

================
Ethtool counters
================

(Ethtool 计数器)

:Copyright: |copy| 2023, NVIDIA CORPORATION & AFFILIATES. All rights reserved.

(版权： |copy| 2023，NVIDIA CORPORATION & AFFILIATES。保留所有权利。)

Contents
(目录)
========

- `Overview`_
  (概览)
- `Groups`_
  (分组)
- `Types`_
  (类型)
- `Descriptions`_
  (描述)

Overview
(概览)
========

There are several counter groups based on where the counter is being counted. In
addition, each group of counters may have different counter types.
(根据计数发生的位置不同，存在若干计数器组。此外，每组计数器可能有不同的计
数器类型。)

These counter groups are based on which component in a networking setup,
illustrated below, that they describe::
(这些计数器组基于网络部署中它们所描述的组件，如下图所示：)

                                                  ----------------------------------------

                                                  |                                      |

    ----------------------------------------    ---------------------------------------- |

    |              Hypervisor              |    |                  VM                  | |

    |                                      |    |                                      | |

    | -------------------  --------------- |    | -------------------  --------------- | |

    | | Ethernet driver |  | RDMA driver | |    | | Ethernet driver |  | RDMA driver | | |

    | -------------------  --------------- |    | -------------------  --------------- | |

    |           |                 |        |    |           |                 |        | |

    |           -------------------        |    |           -------------------        | |

    |                   |                  |    |                   |                  |--

    ----------------------------------------    ----------------------------------------

                        |                                           |

            -------------               -----------------------------

            |                           |

         ------                      ------ ------ ------         ------      ------      ------

    -----| PF |----------------------| VF |-| VF |-| VF |-----  --| PF |--- --| PF |--- --| PF |---

    |    ------                      ------ ------ ------    |  | ------  | | ------  | | ------  |

    |                                                        |  |         | |         | |         |

    |                                                        |  |         | |         | |         |

    |                                                        |  |         | |         | |         |

    | eSwitch                                                |  | eSwitch | | eSwitch | | eSwitch |

    ----------------------------------------------------------  ----------- ----------- -----------

               -------------------------------------------------------------------------------

               |                                                                             |

               |                                                                             |

               | Uplink (no counters)                                                        |

               -------------------------------------------------------------------------------

                       ---------------------------------------------------------------

                       |                                                             |

                       |                                                             |

                       | MPFS (no counters)                                          |

                       ---------------------------------------------------------------

                                                     |

                                                     |

                                                     | Port

Groups
(分组)
======

Ring
(Ring)

  Software counters populated by the driver stack.
  (由驱动栈填充的软件计数器。)

Netdev
(Netdev)

  An aggregation of software ring counters.
  (软件 ring 计数器的聚合。)

vPort counters
(vPort 计数器)

  Traffic counters and drops due to steering or no buffers. May indicate issues
  with NIC. These counters include Ethernet traffic counters (including Raw
  Ethernet) and RDMA/RoCE traffic counters.
  (流量计数器以及因导流或无缓冲区而丢弃的数据包计数。可能预示 NIC 存在问题。
  这些计数器包括以太网流量计数器（包括 Raw Ethernet）和 RDMA/RoCE 流量计数
  器。)

Physical port counters
(物理端口计数器)

  Counters that collect statistics about the PFs and VFs. May indicate issues
  with NIC, link, or network. This measuring point holds information on
  standardized counters like IEEE 802.3, RFC2863, RFC 2819, RFC 3635 and
  additional counters like flow control, FEC and more. Physical port counters
  are not exposed to virtual machines.
  (收集关于 PF 和 VF 统计信息的计数器。可能预示 NIC、链路或网络存在问题。此
  测量点保存 IEEE 802.3、RFC2863、RFC 2819、RFC 3635 等标准化计数器以及流控、
  FEC 等其他计数器的信息。物理端口计数器不会暴露给虚拟机。)

Priority Port Counters
(优先级端口计数器)

  A set of the physical port counters, per priority per port.
  (物理端口计数器的一个集合，按每端口每优先级划分。)

Types
(类型)
=====

Counters are divided into three types.
(计数器分为三种类型。)

Traffic Informative Counters
(流量信息型计数器)

  Counters which count traffic. These counters can be used for load estimation
  or for general debug.
  (对流经的流量进行计数的计数器。这些计数器可用于负载估计或一般性调试。)

Traffic Acceleration Counters
(流量加速型计数器)

  Counters which count traffic that was accelerated by Mellanox driver or by
  hardware. The counters are an additional layer to the informative counter set,
  and the same traffic is counted in both informative and acceleration counters.
  (对由 Mellanox 驱动或硬件加速的流量进行计数的计数器。这些计数器是信息型计
  数器集合之外的附加层，同一流量会同时被信息型计数器和加速型计数器计数。)

.. [#accel] Traffic acceleration counter.
           (流量加速型计数器。)

Error Counters
(错误计数器)

  Increment of these counters might indicate a problem. Each of these counters
  has an explanation and correction action.
  (这些计数器的增长可能预示存在问题。每个此类计数器都有相应的解释和纠正措
  施。)

Statistic can be fetched via the `ip link` or `ethtool` commands. `ethtool`
provides more detailed information.::
(可以通过 `ip link` 或 `ethtool` 命令获取统计信息。`ethtool` 提供更详细的
信息：)

    ip –s link show <if-name>
    ethtool -S <if-name>

Descriptions
(描述)
============

XSK, PTP, and QoS counters that are similar to counters defined previously will
not be separately listed. For example, `ptp_tx[i]_packets` will not be
explicitly documented since `tx[i]_packets` describes the behavior of both
counters, except `ptp_tx[i]_packets` is only counted when precision time
protocol is used.
(与前面定义的计数器类似的 XSK、PTP 和 QoS 计数器不再单独列出。例如，
`ptp_tx[i]_packets` 不会被单独说明，因为 `tx[i]_packets` 已描述了两者的行为，
只是 `ptp_tx[i]_packets` 仅在使用精确时间协议时才会计数。)

Ring / Netdev Counter
(Ring / Netdev 计数器)
----------------------------
The following counters are available per ring or software port.
(以下计数器按每个 ring 或软件端口提供。)

These counters provide information on the amount of traffic that was accelerated
by the NIC. The counters are counting the accelerated traffic in addition to the
standard counters which counts it (i.e. accelerated traffic is counted twice).
(这些计数器提供被 NIC 加速的流量数量的信息。除了已对该流量计数的标准计数器
之外，这些计数器还会对加速流量进行计数（即加速流量会被计数两次）。)

The counter names in the table below refers to both ring and port counters. The
notation for ring counters includes the [i] index without the braces. The
notation for port counters doesn't include the [i]. A counter name
`rx[i]_packets` will be printed as `rx0_packets` for ring 0 and `rx_packets` for
the software port.
(下表中的计数器名称同时指代 ring 计数器和端口计数器。ring 计数器的记法包含
不带花括号的 [i] 索引；端口计数器的记法不含 [i]。计数器名 `rx[i]_packets` 在
ring 0 上会打印为 `rx0_packets`，在软件端口上则打印为 `rx_packets`。)

.. flat-table:: Ring / Software Port Counter Table
   :widths: 2 3 1

   * - Counter
       (计数器)
     - Description
       (描述)
     - Type
       (类型)

   * - `rx[i]_packets`
     - The number of packets received on ring i.
       (在 ring i 上接收到的数据包数量。)
     - Informative (信息型)

   * - `rx[i]_bytes`
     - The number of bytes received on ring i.
       (在 ring i 上接收到的字节数。)
     - Informative (信息型)

   * - `tx[i]_packets`
     - The number of packets transmitted on ring i.
       (在 ring i 上发送的数据包数量。)
     - Informative (信息型)

   * - `tx[i]_bytes`
     - The number of bytes transmitted on ring i.
       (在 ring i 上发送的字节数。)
     - Informative (信息型)

   * - `tx[i]_recover`
     - The number of times the SQ was recovered.
       (SQ 被恢复的次数。)
     - Error (错误型)

   * - `tx[i]_cqes`
     - Number of CQEs events on SQ issued on ring i.
       (在 ring i 上发出的 SQ 的 CQE 事件数量。)
     - Informative (信息型)

   * - `tx[i]_cqe_err`
     - The number of error CQEs encountered on the SQ for ring i.
       (在 ring i 的 SQ 上遇到的错误 CQE 数量。)
     - Error (错误型)

   * - `tx[i]_tso_packets`
     - The number of TSO packets transmitted on ring i [#accel]_.
       (在 ring i 上发送的 TSO 数据包数量。)
     - Acceleration (加速型)

   * - `tx[i]_tso_bytes`
     - The number of TSO bytes transmitted on ring i [#accel]_.
       (在 ring i 上发送的 TSO 字节数。)
     - Acceleration (加速型)

   * - `tx[i]_tso_inner_packets`
     - The number of TSO packets which are indicated to be carry internal
       encapsulation transmitted on ring i [#accel]_.
       (在 ring i 上发送的、被指示携带内部封装的 TSO 数据包数量。)
     - Acceleration (加速型)

   * - `tx[i]_tso_inner_bytes`
     - The number of TSO bytes which are indicated to be carry internal
       encapsulation transmitted on ring i [#accel]_.
       (在 ring i 上发送的、被指示携带内部封装的 TSO 字节数。)
     - Acceleration (加速型)

   * - `rx[i]_gro_packets`
     - Number of received packets processed using hardware-accelerated GRO. The
       number of hardware GRO offloaded packets received on ring i. Only true GRO
       packets are counted: only packets that are in an SKB with a GRO count > 1.
       (使用硬件加速 GRO 处理的接收数据包数量。在 ring i 上接收到的经硬件 GRO
       卸载的数据包数量。只统计真正的 GRO 数据包：即 GRO 计数 > 1 的 SKB 中的
       数据包。)
     - Acceleration (加速型)

   * - `rx[i]_gro_bytes`
     - Number of received bytes processed using hardware-accelerated GRO. The
       number of hardware GRO offloaded bytes received on ring i. Only true GRO
       packets are counted: only packets that are in an SKB with a GRO count > 1.
       (使用硬件加速 GRO 处理的接收字节数。在 ring i 上接收到的经硬件 GRO 卸载
       的字节数。只统计真正的 GRO 数据包：即 GRO 计数 > 1 的 SKB 中的数据包。)
     - Acceleration (加速型)

   * - `rx[i]_gro_skbs`
     - The number of GRO SKBs constructed from hardware-accelerated GRO. Only SKBs
       with a GRO count > 1 are counted.
       (由硬件加速 GRO 构造出的 GRO SKB 数量。只统计 GRO 计数 > 1 的 SKB。)
     - Informative (信息型)

   * - `rx[i]_gro_large_hds`
     - Number of receive packets using hardware-accelerated GRO that have large
       headers that require additional memory to be allocated.
       (使用硬件加速 GRO、且具有需要额外分配内存的大报头的接收数据包数量。)
     - Informative (信息型)

   * - `rx[i]_hds_nodata_packets`
     - Number of header only packets in header/data split mode [#accel]_.
       (header/data split 模式下的纯报头数据包数量。)
     - Informative (信息型)

   * - `rx[i]_hds_nodata_bytes`
     - Number of bytes for header only packets in header/data split mode
       [#accel]_.
       (header/data split 模式下纯报头数据包的字节数。)
     - Informative (信息型)

   * - `rx[i]_hds_nosplit_packets`
     - Number of packets that were not split in header/data split mode. A
       packet will not get split when the hardware does not support its
       protocol splitting. An example such a protocol is ICMPv4/v6. Currently
       TCP and UDP with IPv4/IPv6 are supported for header/data split
       [#accel]_.
       (header/data split 模式下未被拆分的数据包数量。当硬件不支持某个数据包
       协议的拆分时，该数据包不会被拆分。此类协议的一个例子是 ICMPv4/v6。目
       前 header/data split 支持 IPv4/IPv6 的 TCP 和 UDP。)
     - Informative (信息型)

   * - `rx[i]_hds_nosplit_bytes`
     - Number of bytes for packets that were not split in header/data split
       mode. A packet will not get split when the hardware does not support its
       protocol splitting. An example such a protocol is ICMPv4/v6. Currently
       TCP and UDP with IPv4/IPv6 are supported for header/data split
       [#accel]_.
       (header/data split 模式下未被拆分的数据包的字节数。当硬件不支持某个数
       据包协议的拆分时，该数据包不会被拆分。此类协议的一个例子是 ICMPv4/v6。
       目前 header/data split 支持 IPv4/IPv6 的 TCP 和 UDP。)
     - Informative (信息型)

   * - `rx[i]_lro_packets`
     - The number of LRO packets received on ring i [#accel]_.
       (在 ring i 上接收到的 LRO 数据包数量。)
     - Acceleration (加速型)

   * - `rx[i]_lro_bytes`
     - The number of LRO bytes received on ring i [#accel]_.
       (在 ring i 上接收到的 LRO 字节数。)
     - Acceleration (加速型)

   * - `rx[i]_ecn_mark`
     - The number of received packets where the ECN mark was turned on.
       (ECN 标记被置位的接收数据包数量。)
     - Informative (信息型)

   * - `rx_oversize_pkts_buffer`
     - The number of dropped received packets due to length which arrived to RQ
       and exceed software buffer size allocated by the device for incoming
       traffic. It might imply that the device MTU is larger than the software
       buffers size.
       (因长度超过设备为传入流量分配的软件缓冲区大小、在到达 RQ 后被丢弃的接
       收数据包数量。这可能意味着设备 MTU 大于软件缓冲区大小。)
     - Error (错误型)

   * - `rx_oversize_pkts_sw_drop`
     - Number of received packets dropped in software because the CQE data is
       larger than the MTU size.
       (因 CQE 数据大于 MTU 大小而在软件中丢弃的接收数据包数量。)
     - Error (错误型)

   * - `rx[i]_csum_unnecessary`
     - Packets received with a `CHECKSUM_UNNECESSARY` on ring i [#accel]_.
       (在 ring i 上接收到的、带 `CHECKSUM_UNNECESSARY` 的数据包。)
     - Acceleration (加速型)

   * - `rx[i]_csum_unnecessary_inner`
     - Packets received with inner encapsulation with a `CHECKSUM_UNNECESSARY`
       on ring i [#accel]_.
       (在 ring i 上接收到的、内层封装带 `CHECKSUM_UNNECESSARY` 的数据包。)
     - Acceleration (加速型)

   * - `rx[i]_csum_none`
     - Packets received with a `CHECKSUM_NONE` on ring i [#accel]_.
       (在 ring i 上接收到的、带 `CHECKSUM_NONE` 的数据包。)
     - Acceleration (加速型)

   * - `rx[i]_csum_complete`
     - Packets received with a `CHECKSUM_COMPLETE` on ring i [#accel]_.
       (在 ring i 上接收到的、带 `CHECKSUM_COMPLETE` 的数据包。)
     - Acceleration (加速型)

   * - `rx[i]_csum_complete_tail`
     - Number of received packets that had checksum calculation computed,
       potentially needed padding, and were able to do so with
       `CHECKSUM_PARTIAL`.
       (已完成校验和计算、可能需要填充、并能以 `CHECKSUM_PARTIAL` 方式完成的
       接收数据包数量。)
     - Informative (信息型)

   * - `rx[i]_csum_complete_tail_slow`
     - Number of received packets that need padding larger than eight bytes for
       the checksum.
       (校验和需要大于八字节填充的接收数据包数量。)
     - Informative (信息型)

   * - `tx[i]_csum_partial`
     - Packets transmitted with a `CHECKSUM_PARTIAL` on ring i [#accel]_.
       (在 ring i 上发送的、带 `CHECKSUM_PARTIAL` 的数据包。)
     - Acceleration (加速型)

   * - `tx[i]_csum_partial_inner`
     - Packets transmitted with inner encapsulation with a `CHECKSUM_PARTIAL` on
       ring i [#accel]_.
       (在 ring i 上发送的、内层封装带 `CHECKSUM_PARTIAL` 的数据包。)
     - Acceleration (加速型)

   * - `tx[i]_csum_none`
     - Packets transmitted with no hardware checksum acceleration on ring i.
       (在 ring i 上发送的、无硬件校验和加速的数据包。)
     - Informative (信息型)

   * - `tx[i]_stopped` / `tx_queue_stopped` [#ring_global]_
     - Events where SQ was full on ring i. If this counter is increased, check
       the amount of buffers allocated for transmission.
       (ring i 上 SQ 已满的事件。如果此计数器增长，请检查为发送分配的缓冲区
       数量。)
     - Informative (信息型)

   * - `tx[i]_wake` / `tx_queue_wake` [#ring_global]_
     - Events where SQ was full and has become not full on ring i.
       (ring i 上 SQ 从已满变为未满的事件。)
     - Informative (信息型)

   * - `tx[i]_dropped` / `tx_queue_dropped` [#ring_global]_
     - Packets transmitted that were dropped due to DMA mapping failure on
       ring i. If this counter is increased, check the amount of buffers
       allocated for transmission.
       (在 ring i 上发送的、因 DMA 映射失败而被丢弃的数据包。如果此计数器增
       长，请检查为发送分配的缓冲区数量。)
     - Error (错误型)

   * - `tx[i]_nop`
     - The number of nop WQEs (empty WQEs) inserted to the SQ (related to
       ring i) due to the reach of the end of the cyclic buffer. When reaching
       near to the end of cyclic buffer the driver may add those empty WQEs to
       avoid handling a state the a WQE start in the end of the queue and ends
       in the beginning of the queue. This is a normal condition.
       (因到达循环缓冲区末尾而插入到 SQ（与 ring i 相关）的 nop WQE（空 WQE）
       数量。当接近循环缓冲区末尾时，驱动可能会添加这些空 WQE，以避免处理
       WQE 起于队列末尾、终于队列开头的状态。这是正常情况。)
     - Informative (信息型)

   * - `tx[i]_timestamps`
     - Transmitted packets that were hardware timestamped at the device's DMA
       layer.
       (在设备 DMA 层被硬件加时间戳的已发送数据包。)
     - Informative (信息型)

   * - `tx[i]_added_vlan_packets`
     - The number of packets sent where vlan tag insertion was offloaded to the
       hardware.
       (vlan 标签插入被卸载到硬件的已发送数据包数量。)
     - Acceleration (加速型)

   * - `rx[i]_removed_vlan_packets`
     - The number of packets received where vlan tag stripping was offloaded to
       the hardware.
       (vlan 标签剥离被卸载到硬件的已接收数据包数量。)
     - Acceleration (加速型)

   * - `rx[i]_wqe_err`
     - The number of wrong opcodes received on ring i.
       (在 ring i 上接收到的错误操作码数量。)
     - Error (错误型)

   * - `rx[i]_mpwqe_frag`
     - The number of WQEs that failed to allocate compound page and hence
       fragmented MPWQE’s (Multi Packet WQEs) were used on ring i. If this
       counter raise, it may suggest that there is no enough memory for large
       pages, the driver allocated fragmented pages. This is not abnormal
       condition.
       (未能分配复合页、因此在 ring i 上使用了分片 MPWQE（多包 WQE）的 WQE 数
       量。如果此计数器增长，可能表明没有足够的大页内存，驱动分配了分片页。
       这并非异常情况。)
     - Informative (信息型)

   * - `rx[i]_mpwqe_filler_cqes`
     - The number of filler CQEs events that were issued on ring i.
       (在 ring i 上发出的填充 CQE 事件数量。)
     - Informative (信息型)

   * - `rx[i]_mpwqe_filler_strides`
     - The number of strides consumed by filler CQEs on ring i.
       (在 ring i 上被填充 CQE 消耗的 stride 数量。)
     - Informative (信息型)

   * - `tx[i]_mpwqe_blks`
     - The number of send blocks processed from Multi-Packet WQEs (mpwqe).
       (从多包 WQE（mpwqe）处理的发送块数量。)
     - Informative (信息型)

   * - `tx[i]_mpwqe_pkts`
     - The number of send packets processed from Multi-Packet WQEs (mpwqe).
       (从多包 WQE（mpwqe）处理的发送数据包数量。)
     - Informative (信息型)

   * - `rx[i]_cqe_compress_blks`
     - The number of receive blocks with CQE compression on ring i [#accel]_.
       (在 ring i 上带 CQE 压缩的接收块数量。)
     - Acceleration (加速型)

   * - `rx[i]_cqe_compress_pkts`
     - The number of receive packets with CQE compression on ring i [#accel]_.
       (在 ring i 上带 CQE 压缩的接收数据包数量。)
     - Acceleration (加速型)

   * - `rx[i]_arfs_add`
     - The number of aRFS flow rules added to the device for direct RQ steering
       on ring i [#accel]_.
       (为在 ring i 上直接导流到 RQ 而添加到设备的 aRFS 流规则数量。)
     - Acceleration (加速型)

   * - `rx[i]_arfs_request_in`
     - Number of flow rules that have been requested to move into ring i for
       direct RQ steering [#accel]_.
       (被请求移入 ring i 以直接导流到 RQ 的流规则数量。)
     - Acceleration (加速型)

   * - `rx[i]_arfs_request_out`
     - Number of flow rules that have been requested to move out of ring i [#accel]_.
       (被请求移出 ring i 的流规则数量。)
     - Acceleration (加速型)

   * - `rx[i]_arfs_expired`
     - Number of flow rules that have been expired and removed [#accel]_.
       (已过期并被移除的流规则数量。)
     - Acceleration (加速型)

   * - `rx[i]_arfs_err`
     - Number of flow rules that failed to be added to the flow table.
       (未能添加到流表中的流规则数量。)
     - Error (错误型)

   * - `rx[i]_recover`
     - The number of times the RQ was recovered.
       (RQ 被恢复的次数。)
     - Error (错误型)

   * - `tx[i]_xmit_more`
     - The number of packets sent with `xmit_more` indication set on the skbuff
       (no doorbell).
       (在 skbuff 上设置了 `xmit_more` 标志（不敲响门铃）而发送的数据包数量。)
     - Acceleration (加速型)

   * - `ch[i]_poll`
     - The number of invocations of NAPI poll of channel i.
       (通道 i 的 NAPI poll 调用次数。)
     - Informative (信息型)

   * - `ch[i]_arm`
     - The number of times the NAPI poll function completed and armed the
       completion queues on channel i.
       (NAPI poll 函数完成并使通道 i 上的完成队列重新进入待命（arm）状态的次
       数。)
     - Informative (信息型)

   * - `ch[i]_aff_change`
     - The number of times the NAPI poll function explicitly stopped execution
       on a CPU due to a change in affinity, on channel i.
       (在通道 i 上，NAPI poll 函数因亲和性变化而显式停止在某个 CPU 上执行的
       次数。)
     - Informative (信息型)

   * - `ch[i]_events`
     - The number of hard interrupt events on the completion queues of channel i.
       (通道 i 完成队列上的硬中断事件数量。)
     - Informative (信息型)

   * - `ch[i]_eq_rearm`
     - The number of times the EQ was recovered.
       (EQ 被恢复的次数。)
     - Error (错误型)

   * - `ch[i]_force_irq`
     - Number of times NAPI is triggered by XSK wakeups by posting a NOP to
       ICOSQ.
       (通过向 ICOSQ 发布 NOP 而由 XSK 唤醒触发 NAPI 的次数。)
     - Acceleration (加速型)

   * - `rx[i]_congst_umr`
     - The number of times an outstanding UMR request is delayed due to
       congestion, on ring i.
       (ring i 上未完成的 UMR 请求因拥塞而被延迟的次数。)
     - Informative (信息型)

   * - `rx_pp_alloc_fast`
     - Number of successful fast path allocations.
       (快速路径分配成功的次数。)
     - Informative (信息型)

   * - `rx_pp_alloc_slow`
     - Number of slow path order-0 allocations.
       (慢速路径 order-0 分配的次数。)
     - Informative (信息型)

   * - `rx_pp_alloc_slow_high_order`
     - Number of slow path high order allocations.
       (慢速路径高阶（high order）分配的次数。)
     - Informative (信息型)

   * - `rx_pp_alloc_empty`
     - Counter is incremented when ptr ring is empty, so a slow path allocation
       was forced.
       (ptr ring 为空、因此被迫进行慢速路径分配时递增的计数器。)
     - Informative (信息型)

   * - `rx_pp_alloc_refill`
     - Counter is incremented when an allocation which triggered a refill of the
       cache.
       (分配触发了缓存补充（refill）时递增的计数器。)
     - Informative (信息型)

   * - `rx_pp_alloc_waive`
     - Counter is incremented when pages obtained from the ptr ring that cannot
       be added to the cache due to a NUMA mismatch.
       (从 ptr ring 获取的页因 NUMA 不匹配而无法加入缓存时递增的计数器。)
     - Informative (信息型)

   * - `rx_pp_recycle_cached`
     - Counter is incremented when recycling placed page in the page pool cache.
       (回收机制将页放入页池缓存时递增的计数器。)
     - Informative (信息型)

   * - `rx_pp_recycle_cache_full`
     - Counter is incremented when page pool cache was full.
       (页池缓存已满时递增的计数器。)
     - Informative (信息型)

   * - `rx_pp_recycle_ring`
     - Counter is incremented when page placed into the ptr ring.
       (页被放入 ptr ring 时递增的计数器。)
     - Informative (信息型)

   * - `rx_pp_recycle_ring_full`
     - Counter is incremented when page released from page pool because the ptr
       ring was full.
       (因 ptr ring 已满而将页从页池释放时递增的计数器。)
     - Informative (信息型)

   * - `rx_pp_recycle_released_ref`
     - Counter is incremented when page released (and not recycled) because
       refcnt > 1.
       (因 refcnt > 1 而释放（而非回收）页时递增的计数器。)
     - Informative (信息型)

   * - `rx[i]_xsk_buff_alloc_err`
     - The number of times allocating an skb or XSK buffer failed in the XSK RQ
       context.
       (在 XSK RQ 上下文中分配 skb 或 XSK 缓冲区失败的次数。)
     - Error (错误型)

   * - `rx[i]_xdp_tx_xmit`
     - The number of packets forwarded back to the port due to XDP program
       `XDP_TX` action (bouncing). these packets are not counted by other
       software counters. These packets are counted by physical port and vPort
       counters.
       (因 XDP 程序的 `XDP_TX` 动作（弹回）而被转发回端口的数据包数量。这些数
       据包不被其他软件计数器计数，但会被物理端口和 vPort 计数器计数。)
     - Informative (信息型)

   * - `rx[i]_xdp_tx_mpwqe`
     - Number of multi-packet WQEs transmitted by the netdev and `XDP_TX`-ed by
       the netdev during the RQ context.
       (netdev 在 RQ 上下文期间发送并由 netdev 进行 `XDP_TX` 的多包 WQE 数量。)
     - Acceleration (加速型)

   * - `rx[i]_xdp_tx_inlnw`
     - Number of WQE data segments transmitted where the data could be inlined
       in the WQE and then `XDP_TX`-ed during the RQ context.
       (在 RQ 上下文期间发送的、数据可内联在 WQE 中随后被 `XDP_TX` 的 WQE 数据
       段数量。)
     - Acceleration (加速型)

   * - `rx[i]_xdp_tx_nops`
     - Number of NOP WQEBBs (WQE building blocks) received posted to the XDP SQ.
       (发布到 XDP SQ 的 NOP WQEBB（WQE 构建块）数量。)
     - Acceleration (加速型)

   * - `rx[i]_xdp_tx_full`
     - The number of packets that should have been forwarded back to the port
       due to `XDP_TX` action but were dropped due to full tx queue. These packets
       are not counted by other software counters. These packets are counted by
       physical port and vPort counters. You may open more rx queues and spread
       traffic rx over all queues and/or increase rx ring size.
       (本应因 `XDP_TX` 动作被转发回端口、但因发送队列已满而被丢弃的数据包数
       量。这些数据包不被其他软件计数器计数，但会被物理端口和 vPort 计数器计
       数。你可以开启更多 rx 队列并将接收流量分散到所有队列上，和/或增大 rx
       ring 大小。)
     - Error (错误型)

   * - `rx[i]_xdp_tx_err`
     - The number of times an `XDP_TX` error such as frame too long and frame
       too short occurred on `XDP_TX` ring of RX ring.
       (在 RX ring 的 `XDP_TX` ring 上发生 `XDP_TX` 错误（如帧过长、帧过短）
       的次数。)
     - Error (错误型)

   * - `rx[i]_xdp_tx_cqes` / `rx_xdp_tx_cqe` [#ring_global]_
     - The number of completions received on the CQ of the `XDP_TX` ring.
       (在 `XDP_TX` ring 的 CQ 上收到的完成数量。)
     - Informative (信息型)

   * - `rx[i]_xdp_drop`
     - The number of packets dropped due to XDP program `XDP_DROP` action. these
       packets are not counted by other software counters. These packets are
       counted by physical port and vPort counters.
       (因 XDP 程序的 `XDP_DROP` 动作而丢弃的数据包数量。这些数据包不被其他
       软件计数器计数，但会被物理端口和 vPort 计数器计数。)
     - Informative (信息型)

   * - `rx[i]_xdp_redirect`
     - The number of times an XDP redirect action was triggered on ring i.
       (在 ring i 上触发 XDP 重定向动作的次数。)
     - Acceleration (加速型)

   * - `tx[i]_xdp_xmit`
     - The number of packets redirected to the interface(due to XDP redirect).
       These packets are not counted by other software counters. These packets
       are counted by physical port and vPort counters.
       (（因 XDP 重定向）被重定向到接口的数据包数量。这些数据包不被其他软件
       计数器计数，但会被物理端口和 vPort 计数器计数。)
     - Informative (信息型)

   * - `tx[i]_xdp_full`
     - The number of packets redirected to the interface(due to XDP redirect),
       but were dropped due to full tx queue. these packets are not counted by
       other software counters. you may enlarge tx queues.
       (（因 XDP 重定向）被重定向到接口、但因发送队列已满而被丢弃的数据包数
       量。这些数据包不被其他软件计数器计数。你可以增大发送队列。)
     - Informative (信息型)

   * - `tx[i]_xdp_mpwqe`
     - Number of multi-packet WQEs offloaded onto the NIC that were
       `XDP_REDIRECT`-ed from other netdevs.
       (被卸载到 NIC 的、由其他 netdev `XDP_REDIRECT` 而来的多包 WQE 数量。)
     - Acceleration (加速型)

   * - `tx[i]_xdp_inlnw`
     - Number of WQE data segments where the data could be inlined in the WQE
       where the data segments were `XDP_REDIRECT`-ed from other netdevs.
       (数据可内联在 WQE 中的 WQE 数据段数量，这些数据段由其他 netdev
       `XDP_REDIRECT` 而来。)
     - Acceleration (加速型)

   * - `tx[i]_xdp_nops`
     - Number of NOP WQEBBs (WQE building blocks) posted to the SQ that were
       `XDP_REDIRECT`-ed from other netdevs.
       (发布到 SQ 的、由其他 netdev `XDP_REDIRECT` 而来的 NOP WQEBB（WQE 构建
       块）数量。)
     - Acceleration (加速型)

   * - `tx[i]_xdp_err`
     - The number of packets redirected to the interface(due to XDP redirect)
       but were dropped due to error such as frame too long and frame too short.
       (（因 XDP 重定向）被重定向到接口、但因帧过长、帧过短等错误而被丢弃的
       数据包数量。)
     - Error (错误型)

   * - `tx[i]_xdp_cqes`
     - The number of completions received for packets redirected to the
       interface(due to XDP redirect) on the CQ.
       (在 CQ 上收到的、（因 XDP 重定向）被重定向到接口的数据包的完成数量。)
     - Informative (信息型)

   * - `tx[i]_xsk_xmit`
     - The number of packets transmitted using XSK zerocopy functionality.
       (使用 XSK 零拷贝功能发送的数据包数量。)
     - Acceleration (加速型)

   * - `tx[i]_xsk_mpwqe`
     - Number of multi-packet WQEs offloaded onto the NIC that were
       `XDP_REDIRECT`-ed from other netdevs.
       (被卸载到 NIC 的、由其他 netdev `XDP_REDIRECT` 而来的多包 WQE 数量。)
     - Acceleration (加速型)

   * - `tx[i]_xsk_inlnw`
     - Number of WQE data segments where the data could be inlined in the WQE
       that are transmitted using XSK zerocopy.
       (使用 XSK 零拷贝发送的、数据可内联在 WQE 中的 WQE 数据段数量。)
     - Acceleration (加速型)

   * - `tx[i]_xsk_full`
     - Number of times doorbell is rung in XSK zerocopy mode when SQ is full.
       (在 XSK 零拷贝模式下 SQ 已满时敲响门铃的次数。)
     - Error (错误型)

   * - `tx[i]_xsk_err`
     - Number of errors that occurred in XSK zerocopy mode such as if the data
       size is larger than the MTU size.
       (XSK 零拷贝模式下发生的错误数量，例如数据大小大于 MTU 大小。)
     - Error (错误型)

   * - `tx[i]_xsk_cqes`
     - Number of CQEs processed in XSK zerocopy mode.
       (在 XSK 零拷贝模式下处理的 CQE 数量。)
     - Acceleration (加速型)

   * - `tx_tls_ctx`
     - Number of TLS TX HW offload contexts added to device for encryption.
       (为加密而添加到设备的 TLS TX 硬件卸载上下文数量。)
     - Acceleration (加速型)

   * - `tx_tls_del`
     - Number of TLS TX HW offload contexts removed from device (connection
       closed).
       (从设备移除的 TLS TX 硬件卸载上下文数量（连接已关闭）。)
     - Acceleration (加速型)

   * - `tx_tls_pool_alloc`
     - Number of times a unit of work is successfully allocated in the TLS HW
       offload pool.
       (在 TLS 硬件卸载池中成功分配一个工作单元的次数。)
     - Acceleration (加速型)

   * - `tx_tls_pool_free`
     - Number of times a unit of work is freed in the TLS HW offload pool.
       (在 TLS 硬件卸载池中释放一个工作单元的次数。)
     - Acceleration (加速型)

   * - `rx_tls_ctx`
     - Number of TLS RX HW offload contexts added to device for decryption.
       (为解密而添加到设备的 TLS RX 硬件卸载上下文数量。)
     - Acceleration (加速型)

   * - `rx_tls_del`
     - Number of TLS RX HW offload contexts deleted from device (connection has
       finished).
       (从设备删除的 TLS RX 硬件卸载上下文数量（连接已结束）。)
     - Acceleration (加速型)

   * - `rx[i]_tls_decrypted_packets`
     - Number of successfully decrypted RX packets which were part of a TLS
       stream.
       (属于某个 TLS 流的成功解密的 RX 数据包数量。)
     - Acceleration (加速型)

   * - `rx[i]_tls_decrypted_bytes`
     - Number of TLS payload bytes in RX packets which were successfully
       decrypted.
       (成功解密的 RX 数据包中的 TLS 载荷字节数。)
     - Acceleration (加速型)

   * - `rx[i]_tls_resync_req_pkt`
     - Number of received TLS packets with a resync request.
       (带重同步请求的接收 TLS 数据包数量。)
     - Acceleration (加速型)

   * - `rx[i]_tls_resync_req_start`
     - Number of times the TLS async resync request was started.
       (TLS 异步重同步请求启动的次数。)
     - Acceleration (加速型)

   * - `rx[i]_tls_resync_req_end`
     - Number of times the TLS async resync request properly ended with
       providing the HW tracked tcp-seq.
       (TLS 异步重同步请求正常结束并提供硬件跟踪的 tcp-seq 的次数。)
     - Acceleration (加速型)

   * - `rx[i]_tls_resync_req_skip`
     - Number of times the TLS async resync request procedure was started but
       not properly ended.
       (TLS 异步重同步请求过程已启动但未正常结束的次数。)
     - Error (错误型)

   * - `rx[i]_tls_resync_res_ok`
     - Number of times the TLS resync response call to the driver was
       successfully handled.
       (对驱动的 TLS 重同步响应调用被成功处理的次数。)
     - Acceleration (加速型)

   * - `rx[i]_tls_resync_res_retry`
     - Number of times the TLS resync response call to the driver was
       reattempted when ICOSQ is full.
       (因 ICOSQ 已满而重试对驱动的 TLS 重同步响应调用的次数。)
     - Error (错误型)

   * - `rx[i]_tls_resync_res_skip`
     - Number of times the TLS resync response call to the driver was terminated
       unsuccessfully.
       (对驱动的 TLS 重同步响应调用未成功终止的次数。)
     - Error (错误型)

   * - `rx[i]_tls_err`
     - Number of times when CQE TLS offload was problematic.
       (CQE TLS 卸载出现问题的次数。)
     - Error (错误型)

   * - `tx[i]_tls_encrypted_packets`
     - The number of send packets that are TLS encrypted by the kernel.
       (由内核进行 TLS 加密的发送数据包数量。)
     - Acceleration (加速型)

   * - `tx[i]_tls_encrypted_bytes`
     - The number of send bytes that are TLS encrypted by the kernel.
       (由内核进行 TLS 加密的发送字节数。)
     - Acceleration (加速型)

   * - `tx[i]_tls_ooo`
     - Number of times out of order TLS SQE fragments were handled on ring i.
       (在 ring i 上处理乱序 TLS SQE 片段的次数。)
     - Acceleration (加速型)

   * - `tx[i]_tls_dump_packets`
     - Number of TLS decrypted packets copied over from NIC over DMA.
       (通过 DMA 从 NIC 复制过来的 TLS 解密数据包数量。)
     - Acceleration (加速型)

   * - `tx[i]_tls_dump_bytes`
     - Number of TLS decrypted bytes copied over from NIC over DMA.
       (通过 DMA 从 NIC 复制过来的 TLS 解密字节数。)
     - Acceleration (加速型)

   * - `tx[i]_tls_resync_bytes`
     - Number of TLS bytes requested to be resynchronized in order to be
       decrypted.
       (为解密而请求重新同步的 TLS 字节数。)
     - Acceleration (加速型)

   * - `tx[i]_tls_skip_no_sync_data`
     - Number of TLS send data that can safely be skipped / do not need to be
       decrypted.
       (可安全跳过 / 无需解密的 TLS 发送数据量。)
     - Acceleration (加速型)

   * - `tx[i]_tls_drop_no_sync_data`
     - Number of TLS send data that were dropped due to retransmission of TLS
       data.
       (因 TLS 数据重传而被丢弃的 TLS 发送数据量。)
     - Acceleration (加速型)

   * - `ptp_cq[i]_abort`
     - Number of times a CQE has to be skipped in precision time protocol due to
       a skew between the port timestamp and CQE timestamp being greater than
       128 seconds.
       (在精确时间协议中，因端口时间戳与 CQE 时间戳之间的偏差大于 128 秒而不
       得不跳过 CQE 的次数。)
     - Error (错误型)

   * - `ptp_cq[i]_abort_abs_diff_ns`
     - Accumulation of time differences between the port timestamp and CQE
       timestamp when the difference is greater than 128 seconds in precision
       time protocol.
       (精确时间协议中，当端口时间戳与 CQE 时间戳之差大于 128 秒时，两者时间
       差的累计值。)
     - Error (错误型)

   * - `ptp_cq[i]_late_cqe`
     - Number of times a CQE has been delivered on the PTP timestamping CQ when
       the CQE was not expected since a certain amount of time had elapsed where
       the device typically ensures not posting the CQE.
       (在设备通常确保不再投递 CQE 的时间段已流逝后、CQE 仍被投递到 PTP 时间
       戳 CQ 的次数。)
     - Error (错误型)

   * - `ptp_cq[i]_lost_cqe`
     - Number of times a CQE is expected to not be delivered on the PTP
       timestamping CQE by the device due to a time delta elapsing. If such a
       CQE is somehow delivered, `ptp_cq[i]_late_cqe` is incremented.
       (由于时间增量流逝、设备预期不会在 PTP 时间戳 CQ 上投递 CQE 的情况次
       数。如果这样的 CQE 仍被投递，`ptp_cq[i]_late_cqe` 会递增。)
     - Error (错误型)

.. [#ring_global] The corresponding ring and global counters do not share the
                  same name (i.e. do not follow the common naming scheme).
                  (对应的 ring 计数器与全局计数器名称并不相同（即不遵循通用命
                  名规则）。)

vPort Counters
(vPort 计数器)
--------------
Counters on the NIC port that is connected to a eSwitch.
(连接到 eSwitch 的 NIC 端口上的计数器。)

.. flat-table:: vPort Counter Table
   :widths: 2 3 1

   * - Counter
       (计数器)
     - Description
       (描述)
     - Type
       (类型)

   * - `rx_vport_unicast_packets`
     - Unicast packets received, steered to a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (接收到的、被导流到端口的单播数据包，包括 Raw Ethernet QP/DPDK 流量，
       不包括 RDMA 流量。)
     - Informative (信息型)

   * - `rx_vport_unicast_bytes`
     - Unicast bytes received, steered to a port including Raw Ethernet QP/DPDK
       traffic, excluding RDMA traffic.
       (接收到的、被导流到端口的单播字节数，包括 Raw Ethernet QP/DPDK 流量，
       不包括 RDMA 流量。)
     - Informative (信息型)

   * - `tx_vport_unicast_packets`
     - Unicast packets transmitted, steered from a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (发送的、从端口导流出的单播数据包，包括 Raw Ethernet QP/DPDK 流量，不
       包括 RDMA 流量。)
     - Informative (信息型)

   * - `tx_vport_unicast_bytes`
     - Unicast bytes transmitted, steered from a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (发送的、从端口导流出的单播字节数，包括 Raw Ethernet QP/DPDK 流量，不
       包括 RDMA 流量。)
     - Informative (信息型)

   * - `rx_vport_multicast_packets`
     - Multicast packets received, steered to a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (接收到的、被导流到端口的组播数据包，包括 Raw Ethernet QP/DPDK 流量，
       不包括 RDMA 流量。)
     - Informative (信息型)

   * - `rx_vport_multicast_bytes`
     - Multicast bytes received, steered to a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (接收到的、被导流到端口的组播字节数，包括 Raw Ethernet QP/DPDK 流量，
       不包括 RDMA 流量。)
     - Informative (信息型)

   * - `tx_vport_multicast_packets`
     - Multicast packets transmitted, steered from a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (发送的、从端口导流出的组播数据包，包括 Raw Ethernet QP/DPDK 流量，不
       包括 RDMA 流量。)
     - Informative (信息型)

   * - `tx_vport_multicast_bytes`
     - Multicast bytes transmitted, steered from a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (发送的、从端口导流出的组播字节数，包括 Raw Ethernet QP/DPDK 流量，不
       包括 RDMA 流量。)
     - Informative (信息型)

   * - `rx_vport_broadcast_packets`
     - Broadcast packets received, steered to a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (接收到的、被导流到端口的广播数据包，包括 Raw Ethernet QP/DPDK 流量，
       不包括 RDMA 流量。)
     - Informative (信息型)

   * - `rx_vport_broadcast_bytes`
     - Broadcast bytes received, steered to a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (接收到的、被导流到端口的广播字节数，包括 Raw Ethernet QP/DPDK 流量，
       不包括 RDMA 流量。)
     - Informative (信息型)

   * - `tx_vport_broadcast_packets`
     - Broadcast packets transmitted, steered from a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (发送的、从端口导流出的广播数据包，包括 Raw Ethernet QP/DPDK 流量，不
       包括 RDMA 流量。)
     - Informative (信息型)

   * - `tx_vport_broadcast_bytes`
     - Broadcast bytes transmitted, steered from a port including Raw Ethernet
       QP/DPDK traffic, excluding RDMA traffic.
       (发送的、从端口导流出的广播字节数，包括 Raw Ethernet QP/DPDK 流量，不
       包括 RDMA 流量。)
     - Informative (信息型)

   * - `rx_vport_rdma_unicast_packets`
     - RDMA unicast packets received, steered to a port (counters counts
       RoCE/UD/RC traffic) [#accel]_.
       (接收到的、被导流到端口的 RDMA 单播数据包（计数器统计 RoCE/UD/RC 流
       量）。)
     - Acceleration (加速型)

   * - `rx_vport_rdma_unicast_bytes`
     - RDMA unicast bytes received, steered to a port (counters counts
       RoCE/UD/RC traffic) [#accel]_.
       (接收到的、被导流到端口的 RDMA 单播字节数（计数器统计 RoCE/UD/RC 流
       量）。)
     - Acceleration (加速型)

   * - `tx_vport_rdma_unicast_packets`
     - RDMA unicast packets transmitted, steered from a port (counters counts
       RoCE/UD/RC traffic) [#accel]_.
       (发送的、从端口导流出的 RDMA 单播数据包（计数器统计 RoCE/UD/RC 流
       量）。)
     - Acceleration (加速型)

   * - `tx_vport_rdma_unicast_bytes`
     - RDMA unicast bytes transmitted, steered from a port (counters counts
       RoCE/UD/RC traffic) [#accel]_.
       (发送的、从端口导流出的 RDMA 单播字节数（计数器统计 RoCE/UD/RC 流
       量）。)
     - Acceleration (加速型)

   * - `rx_vport_rdma_multicast_packets`
     - RDMA multicast packets received, steered to a port (counters counts
       RoCE/UD/RC traffic) [#accel]_.
       (接收到的、被导流到端口的 RDMA 组播数据包（计数器统计 RoCE/UD/RC 流
       量）。)
     - Acceleration (加速型)

   * - `rx_vport_rdma_multicast_bytes`
     - RDMA multicast bytes received, steered to a port (counters counts
       RoCE/UD/RC traffic) [#accel]_.
       (接收到的、被导流到端口的 RDMA 组播字节数（计数器统计 RoCE/UD/RC 流
       量）。)
     - Acceleration (加速型)

   * - `tx_vport_rdma_multicast_packets`
     - RDMA multicast packets transmitted, steered from a port (counters counts
       RoCE/UD/RC traffic) [#accel]_.
       (发送的、从端口导流出的 RDMA 组播数据包（计数器统计 RoCE/UD/RC 流
       量）。)
     - Acceleration (加速型)

   * - `tx_vport_rdma_multicast_bytes`
     - RDMA multicast bytes transmitted, steered from a port (counters counts
       RoCE/UD/RC traffic) [#accel]_.
       (发送的、从端口导流出的 RDMA 组播字节数（计数器统计 RoCE/UD/RC 流
       量）。)
     - Acceleration (加速型)

   * - `vport_loopback_packets`
     - Unicast, multicast and broadcast packets that were loop-back (received
       and transmitted), IB/Eth  [#accel]_.
       (环回（既接收又发送）的单播、组播和广播数据包，IB/Eth。)
     - Acceleration (加速型)

   * - `vport_loopback_bytes`
     - Unicast, multicast and broadcast bytes that were loop-back (received
       and transmitted), IB/Eth  [#accel]_.
       (环回（既接收又发送）的单播、组播和广播字节数，IB/Eth。)
     - Acceleration (加速型)

   * - `rx_steer_missed_packets`
     - Number of packets that was received by the NIC, however was discarded
       because it did not match any flow in the NIC flow table.
       (NIC 收到、但因不匹配 NIC 流表中的任何流而被丢弃的数据包数量。)
     - Error (错误型)

   * - `rx_packets`
     - Representor only: packets received, that were handled by the hypervisor.
       (仅限 representor：接收到的、由 hypervisor 处理的数据包。)
     - Informative (信息型)

   * - `rx_bytes`
     - Representor only: bytes received, that were handled by the hypervisor.
       (仅限 representor：接收到的、由 hypervisor 处理的字节数。)
     - Informative (信息型)

   * - `tx_packets`
     - Representor only: packets transmitted, that were handled by the
       hypervisor.
       (仅限 representor：发送的、由 hypervisor 处理的数据包。)
     - Informative (信息型)

   * - `tx_bytes`
     - Representor only: bytes transmitted, that were handled by the hypervisor.
       (仅限 representor：发送的、由 hypervisor 处理的字节数。)
     - Informative (信息型)

   * - `dev_internal_queue_oob`
     - The number of dropped packets due to lack of receive WQEs for an internal
       device RQ.
       (因内部设备 RQ 缺少接收 WQE 而丢弃的数据包数量。)
     - Error (错误型)

Physical Port Counters
(物理端口计数器)
----------------------
The physical port counters are the counters on the external port connecting the
adapter to the network. This measuring point holds information on standardized
counters like IEEE 802.3, RFC2863, RFC 2819, RFC 3635 and additional counters
like flow control, FEC and more.
(物理端口计数器是连接适配器与网络的外部端口上的计数器。此测量点保存 IEEE
802.3、RFC2863、RFC 2819、RFC 3635 等标准化计数器以及流控、FEC 等其他计数器
的信息。)

.. flat-table:: Physical Port Counter Table
   :widths: 2 3 1

   * - Counter
       (计数器)
     - Description
       (描述)
     - Type
       (类型)

   * - `rx_packets_phy`
     - The number of packets received on the physical port. This counter doesn’t
       include packets that were discarded due to FCS, frame size and similar
       errors.
       (在物理端口上接收到的数据包数量。此计数器不包括因 FCS、帧大小及类似错
       误而被丢弃的数据包。)
     - Informative (信息型)

   * - `tx_packets_phy`
     - The number of packets transmitted on the physical port.
       (在物理端口上发送的数据包数量。)
     - Informative (信息型)

   * - `rx_bytes_phy`
     - The number of bytes received on the physical port, including Ethernet
       header and FCS.
       (在物理端口上接收到的字节数，包括以太网报头和 FCS。)
     - Informative (信息型)

   * - `tx_bytes_phy`
     - The number of bytes transmitted on the physical port.
       (在物理端口上发送的字节数。)
     - Informative (信息型)

   * - `rx_multicast_phy`
     - The number of multicast packets received on the physical port.
       (在物理端口上接收到的组播数据包数量。)
     - Informative (信息型)

   * - `tx_multicast_phy`
     - The number of multicast packets transmitted on the physical port.
       (在物理端口上发送的组播数据包数量。)
     - Informative (信息型)

   * - `rx_broadcast_phy`
     - The number of broadcast packets received on the physical port.
       (在物理端口上接收到的广播数据包数量。)
     - Informative (信息型)

   * - `tx_broadcast_phy`
     - The number of broadcast packets transmitted on the physical port.
       (在物理端口上发送的广播数据包数量。)
     - Informative (信息型)

   * - `rx_crc_errors_phy`
     - The number of dropped received packets due to FCS (Frame Check Sequence)
       error on the physical port. If this counter is increased in high rate,
       check the link quality using `rx_symbol_error_phy` and
       `rx_corrected_bits_phy` counters below.
       (物理端口上因 FCS（帧校验序列）错误而丢弃的接收数据包数量。如果此计数
       器高速增长，请使用下方的 `rx_symbol_error_phy` 和 `rx_corrected_bits_phy`
       计数器检查链路质量。)
     - Error (错误型)

   * - `rx_in_range_len_errors_phy`
     - The number of received packets dropped due to length/type errors on a
       physical port.
       (物理端口上因长度/类型错误而丢弃的接收数据包数量。)
     - Error (错误型)

   * - `rx_out_of_range_len_phy`
     - The number of received packets dropped due to length greater than allowed
       on a physical port. If this counter is increasing, it implies that the
       peer connected to the adapter has a larger MTU configured. Using same MTU
       configuration shall resolve this issue.
       (物理端口上因长度大于允许值而丢弃的接收数据包数量。如果此计数器持续增
       长，说明与适配器相连的对端配置了更大的 MTU。使用相同的 MTU 配置即可解
       决此问题。)
     - Error (错误型)

   * - `rx_oversize_pkts_phy`
     - The number of dropped received packets due to length which exceed MTU
       size on a physical port. If this counter is increasing, it implies that
       the peer connected to the adapter has a larger MTU configured. Using same
       MTU configuration shall resolve this issue.
       (物理端口上因长度超过 MTU 大小而丢弃的接收数据包数量。如果此计数器持
       续增长，说明与适配器相连的对端配置了更大的 MTU。使用相同的 MTU 配置即
       可解决此问题。)
     - Error (错误型)

   * - `rx_symbol_err_phy`
     - The number of received packets dropped due to physical coding errors
       (symbol errors) on a physical port.
       (物理端口上因物理编码错误（符号错误）而丢弃的接收数据包数量。)
     - Error (错误型)

   * - `rx_mac_control_phy`
     - The number of MAC control packets received on the physical port.
       (在物理端口上接收到的 MAC 控制数据包数量。)
     - Informative (信息型)

   * - `tx_mac_control_phy`
     - The number of MAC control packets transmitted on the physical port.
       (在物理端口上发送的 MAC 控制数据包数量。)
     - Informative (信息型)

   * - `rx_pause_ctrl_phy`
     - The number of link layer pause packets received on a physical port. If
       this counter is increasing, it implies that the network is congested and
       cannot absorb the traffic coming from to the adapter.
       (物理端口上接收到的链路层 pause 数据包数量。如果此计数器持续增长，说
       明网络拥塞、无法吸收来自适配器的流量。)
     - Informative (信息型)

   * - `tx_pause_ctrl_phy`
     - The number of link layer pause packets transmitted on a physical port. If
       this counter is increasing, it implies that the NIC is congested and
       cannot absorb the traffic coming from the network.
       (物理端口上发送的链路层 pause 数据包数量。如果此计数器持续增长，说明
       NIC 拥塞、无法吸收来自网络的流量。)
     - Informative (信息型)

   * - `rx_unsupported_op_phy`
     - The number of MAC control packets received with unsupported opcode on a
       physical port.
       (物理端口上接收到的、操作码不受支持的 MAC 控制数据包数量。)
     - Error (错误型)

   * - `rx_discards_phy`
     - The number of received packets dropped due to lack of buffers on a
       physical port. If this counter is increasing, it implies that the adapter
       is congested and cannot absorb the traffic coming from the network.
       (物理端口上因缺少缓冲区而丢弃的接收数据包数量。如果此计数器持续增长，
       说明适配器拥塞、无法吸收来自网络的流量。)
     - Error (错误型)

   * - `tx_discards_phy`
     - The number of packets which were discarded on transmission, even no
       errors were detected. the drop might occur due to link in down state,
       head of line drop, pause from the network, etc.
       (即使未检测到错误、在发送时仍被丢弃的数据包数量。丢包可能因链路处于
       down 状态、队头丢弃、来自网络的 pause 等而发生。)
     - Error (错误型)

   * - `tx_errors_phy`
     - The number of transmitted packets dropped due to a length which exceed
       MTU size on a physical port.
       (物理端口上因长度超过 MTU 大小而丢弃的已发送数据包数量。)
     - Error (错误型)

   * - `rx_undersize_pkts_phy`
     - The number of received packets dropped due to length which is shorter
       than 64 bytes on a physical port. If this counter is increasing, it
       implies that the peer connected to the adapter has a non-standard MTU
       configured or malformed packet had arrived.
       (物理端口上因长度短于 64 字节而丢弃的接收数据包数量。如果此计数器持续
       增长，说明与适配器相连的对端配置了非标准 MTU，或收到了畸形数据包。)
     - Error (错误型)

   * - `rx_fragments_phy`
     - The number of received packets dropped due to a length which is shorter
       than 64 bytes and has FCS error on a physical port. If this counter is
       increasing, it implies that the peer connected to the adapter has a
       non-standard MTU configured.
       (物理端口上因长度短于 64 字节且带 FCS 错误而丢弃的接收数据包数量。如果
       此计数器持续增长，说明与适配器相连的对端配置了非标准 MTU。)
     - Error (错误型)

   * - `rx_jabbers_phy`
     - The number of received packets d due to a length which is longer than 64
       bytes and had FCS error on a physical port.
       (物理端口上因长度长于 64 字节且带 FCS 错误而被丢弃的接收数据包数量。)
     - Error (错误型)

   * - `rx_64_bytes_phy`
     - The number of packets received on the physical port with size of 64 bytes.
       (在物理端口上接收到的、大小为 64 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_65_to_127_bytes_phy`
     - The number of packets received on the physical port with size of 65 to
       127 bytes.
       (在物理端口上接收到的、大小为 65 至 127 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_128_to_255_bytes_phy`
     - The number of packets received on the physical port with size of 128 to
       255 bytes.
       (在物理端口上接收到的、大小为 128 至 255 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_256_to_511_bytes_phy`
     - The number of packets received on the physical port with size of 256 to
       512 bytes.
       (在物理端口上接收到的、大小为 256 至 512 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_512_to_1023_bytes_phy`
     - The number of packets received on the physical port with size of 512 to
       1023 bytes.
       (在物理端口上接收到的、大小为 512 至 1023 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_1024_to_1518_bytes_phy`
     - The number of packets received on the physical port with size of 1024 to
       1518 bytes.
       (在物理端口上接收到的、大小为 1024 至 1518 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_1519_to_2047_bytes_phy`
     - The number of packets received on the physical port with size of 1519 to
       2047 bytes.
       (在物理端口上接收到的、大小为 1519 至 2047 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_2048_to_4095_bytes_phy`
     - The number of packets received on the physical port with size of 2048 to
       4095 bytes.
       (在物理端口上接收到的、大小为 2048 至 4095 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_4096_to_8191_bytes_phy`
     - The number of packets received on the physical port with size of 4096 to
       8191 bytes.
       (在物理端口上接收到的、大小为 4096 至 8191 字节的数据包数量。)
     - Informative (信息型)

   * - `rx_8192_to_10239_bytes_phy`
     - The number of packets received on the physical port with size of 8192 to
       10239 bytes.
       (在物理端口上接收到的、大小为 8192 至 10239 字节的数据包数量。)
     - Informative (信息型)

   * - `link_down_events_phy`
     - The number of times where the link operative state changed to down. In
       case this counter is increasing it may imply on port flapping. You may
       need to replace the cable/transceiver.
       (链路运行状态变为 down 的次数。如果此计数器持续增长，可能预示端口抖动
       （flapping）。你可能需要更换线缆/收发器。)
     - Error (错误型)

   * - `total_success_recovery_phy`
     - The number of total successful recovery events of any type during
       ports reset cycle.
       (端口复位周期内任何类型的成功恢复事件总数。)
     - Error (错误型)

   * - `rx_out_of_buffer`
     - Number of times receive queue had no software buffers allocated for the
       adapter's incoming traffic.
       (接收队列没有为适配器的传入流量分配软件缓冲区的次数。)
     - Error (错误型)

   * - `module_bus_stuck`
     - The number of times that module's I\ :sup:`2`\C bus (data or clock)
       short-wire was detected. You may need to replace the cable/transceiver.
       (检测到模块 I2C 总线（数据线或时钟线）短接故障的次数。你可能需要更换
       线缆/收发器。)
     - Error (错误型)

   * - `module_high_temp`
     - The number of times that the module temperature was too high. If this
       issue persist, you may need to check the ambient temperature or replace
       the cable/transceiver module.
       (模块温度过高的次数。如果问题持续存在，你可能需要检查环境温度或更换线
       缆/收发器模块。)
     - Error (错误型)

   * - `module_bad_shorted`
     - The number of times that the module cables were shorted. You may need to
       replace the cable/transceiver module.
       (模块线缆被短接的次数。你可能需要更换线缆/收发器模块。)
     - Error (错误型)

   * - `module_unplug`
     - The number of times that module was ejected.
       (模块被拔出的次数。)
     - Informative (信息型)

   * - `rx_buffer_passed_thres_phy`
     - The number of events where the port receive buffer was over 85% full.
       (端口接收缓冲区占用超过 85% 的事件次数。)
     - Informative (信息型)

   * - `tx_pause_storm_warning_events`
     - The number of times the device was sending pauses for a long period of
       time.
       (设备长时间持续发送 pause 帧的次数。)
     - Informative (信息型)

   * - `tx_pause_storm_error_events`
     - The number of times the device was sending pauses for a long period of
       time, reaching time out and disabling transmission of pause frames. on
       the period where pause frames were disabled, drop could have been
       occurred.
       (设备长时间持续发送 pause 帧、最终超时并停用 pause 帧发送的次数。在
       pause 帧被停用期间，可能发生丢包。)
     - Error (错误型)

   * - `rx[i]_buff_alloc_err`
     - Failed to allocate a buffer to received packet (or SKB) on ring i.
       (在 ring i 上为接收到的数据包（或 SKB）分配缓冲区失败。)
     - Error (错误型)

   * - `rx_bits_phy`
     - This counter provides information on the total amount of traffic that
       could have been received and can be used as a guideline to measure the
       ratio of errored traffic in `rx_pcs_symbol_err_phy` and
       `rx_corrected_bits_phy`.
       (此计数器提供本可接收的流量总量信息，可用作衡量 `rx_pcs_symbol_err_phy`
       和 `rx_corrected_bits_phy` 中错误流量比例的基准。)
     - Informative (信息型)

   * - `rx_pcs_symbol_err_phy`
     - This counter counts the number of symbol errors that wasn’t corrected by
       FEC correction algorithm or that FEC algorithm was not active on this
       interface. If this counter is increasing, it implies that the link
       between the NIC and the network is suffering from high BER, and that
       traffic is lost. You may need to replace the cable/transceiver. The error
       rate is the number of `rx_pcs_symbol_err_phy` divided by the number of
       `rx_bits_phy` on a specific time frame.
       (此计数器统计未被 FEC 纠错算法纠正、或 FEC 算法在此接口上未启用的符号
       错误数量。如果此计数器持续增长，说明 NIC 与网络之间的链路正遭受高误码
       率（BER），且有流量丢失。你可能需要更换线缆/收发器。错误率为特定时间
       范围内 `rx_pcs_symbol_err_phy` 的数值除以 `rx_bits_phy` 的数值。)
     - Error (错误型)

   * - `rx_corrected_bits_phy`
     - The number of corrected bits on this port according to active FEC
       (RS/FC). If this counter is increasing, it implies that the link between
       the NIC and the network is suffering from high BER. The corrected bit
       rate is the number of `rx_corrected_bits_phy` divided by the number of
       `rx_bits_phy` on a specific time frame.
       (此端口上按活动 FEC（RS/FC）纠正的比特数。如果此计数器持续增长，说明
       NIC 与网络之间的链路正遭受高误码率（BER）。纠正比特率为特定时间范围内
       `rx_corrected_bits_phy` 的数值除以 `rx_bits_phy` 的数值。)
     - Error (错误型)

   * - `rx_err_lane_[l]_phy`
     - This counter counts the number of physical raw errors per lane l index.
       The counter counts errors before FEC corrections. If this counter is
       increasing, it implies that the link between the NIC and the network is
       suffering from high BER, and that traffic might be lost. You may need to
       replace the cable/transceiver. Please check in accordance with
       `rx_corrected_bits_phy`.
       (此计数器统计每个 lane（索引 l）的物理原始错误数量。该计数器统计 FEC
       纠正之前的错误。如果此计数器持续增长，说明 NIC 与网络之间的链路正遭受
       高误码率（BER），流量可能丢失。你可能需要更换线缆/收发器。请结合
       `rx_corrected_bits_phy` 进行检查。)
     - Error (错误型)

   * - `rx_global_pause`
     - The number of pause packets received on the physical port. If this
       counter is increasing, it implies that the network is congested and
       cannot absorb the traffic coming from the adapter. Note: This counter is
       only enabled when global pause mode is enabled.
       (物理端口上接收到的 pause 数据包数量。如果此计数器持续增长，说明网络
       拥塞、无法吸收来自适配器的流量。注意：仅当全局 pause 模式启用时，此计
       数器才启用。)
     - Informative (信息型)

   * - `rx_global_pause_duration`
     - The duration of pause received (in microSec) on the physical port. The
       counter represents the time the port did not send any traffic. If this
       counter is increasing, it implies that the network is congested and
       cannot absorb the traffic coming from the adapter. Note: This counter is
       only enabled when global pause mode is enabled.
       (物理端口上接收到的 pause 持续时长（微秒）。该计数器表示端口未发送任何
       流量的时间。如果此计数器持续增长，说明网络拥塞、无法吸收来自适配器的
       流量。注意：仅当全局 pause 模式启用时，此计数器才启用。)
     - Informative (信息型)

   * - `tx_global_pause`
     - The number of pause packets transmitted on a physical port. If this
       counter is increasing, it implies that the adapter is congested and
       cannot absorb the traffic coming from the network. Note: This counter is
       only enabled when global pause mode is enabled.
       (物理端口上发送的 pause 数据包数量。如果此计数器持续增长，说明适配器
       拥塞、无法吸收来自网络的流量。注意：仅当全局 pause 模式启用时，此计数
       器才启用。)
     - Informative (信息型)

   * - `tx_global_pause_duration`
     - The duration of pause transmitter (in microSec) on the physical port.
       Note: This counter is only enabled when global pause mode is enabled.
       (物理端口上发送端 pause 的持续时长（微秒）。注意：仅当全局 pause 模式
       启用时，此计数器才启用。)
     - Informative (信息型)

   * - `rx_global_pause_transition`
     - The number of times a transition from Xoff to Xon on the physical port
       has occurred. Note: This counter is only enabled when global pause mode
       is enabled.
       (物理端口上从 Xoff 到 Xon 的状态转换发生次数。注意：仅当全局 pause 模
       式启用时，此计数器才启用。)
     - Informative (信息型)

   * - `rx_if_down_packets`
     - The number of received packets that were dropped due to interface down.
       (因接口 down 而被丢弃的接收数据包数量。)
     - Informative (信息型)

Priority Port Counters
(优先级端口计数器)
----------------------
The following counters are physical port counters that are counted per L2
priority (0-7).
(以下计数器是按 L2 优先级（0-7）分别计数的物理端口计数器。)

**Note:** `p` in the counter name represents the priority.
( **注意：** 计数器名称中的 `p` 代表优先级。)

.. flat-table:: Priority Port Counter Table
   :widths: 2 3 1

   * - Counter
       (计数器)
     - Description
       (描述)
     - Type
       (类型)

   * - `rx_prio[p]_bytes`
     - The number of bytes received with priority p on the physical port.
       (物理端口上以优先级 p 接收到的字节数。)
     - Informative (信息型)

   * - `rx_prio[p]_packets`
     - The number of packets received with priority p on the physical port.
       (物理端口上以优先级 p 接收到的数据包数量。)
     - Informative (信息型)

   * - `tx_prio[p]_bytes`
     - The number of bytes transmitted on priority p on the physical port.
       (物理端口上以优先级 p 发送的字节数。)
     - Informative (信息型)

   * - `tx_prio[p]_packets`
     - The number of packets transmitted on priority p on the physical port.
       (物理端口上以优先级 p 发送的数据包数量。)
     - Informative (信息型)

   * - `rx_prio[p]_pause`
     - The number of pause packets received with priority p on a physical port.
       If this counter is increasing, it implies that the network is congested
       and cannot absorb the traffic coming from the adapter. Note: This counter
       is available only if PFC was enabled on priority p.
       (物理端口上以优先级 p 接收到的 pause 数据包数量。如果此计数器持续增
       长，说明网络拥塞、无法吸收来自适配器的流量。注意：仅当优先级 p 上启用
       了 PFC 时，此计数器才可用。)
     - Informative (信息型)

   * - `rx_prio[p]_pause_duration`
     - The duration of pause received (in microSec) on priority p on the
       physical port. The counter represents the time the port did not send any
       traffic on this priority. If this counter is increasing, it implies that
       the network is congested and cannot absorb the traffic coming from the
       adapter. Note: This counter is available only if PFC was enabled on
       priority p.
       (物理端口上优先级 p 接收到的 pause 持续时长（微秒）。该计数器表示端口
       在此优先级上未发送任何流量的时间。如果此计数器持续增长，说明网络拥塞、
       无法吸收来自适配器的流量。注意：仅当优先级 p 上启用了 PFC 时，此计数器
       才可用。)
     - Informative (信息型)

   * - `rx_prio[p]_pause_transition`
     - The number of times a transition from Xoff to Xon on priority p on the
       physical port has occurred. Note: This counter is available only if PFC
       was enabled on priority p.
       (物理端口上优先级 p 从 Xoff 到 Xon 的状态转换发生次数。注意：仅当优先
       级 p 上启用了 PFC 时，此计数器才可用。)
     - Informative (信息型)

   * - `tx_prio[p]_pause`
     - The number of pause packets transmitted on priority p on a physical port.
       If this counter is increasing, it implies that the adapter is congested
       and cannot absorb the traffic coming from the network. Note: This counter
       is available only if PFC was enabled on priority p.
       (物理端口上以优先级 p 发送的 pause 数据包数量。如果此计数器持续增长，
       说明适配器拥塞、无法吸收来自网络的流量。注意：仅当优先级 p 上启用了
       PFC 时，此计数器才可用。)
     - Informative (信息型)

   * - `tx_prio[p]_pause_duration`
     - The duration of pause transmitter (in microSec) on priority p on the
       physical port. Note: This counter is available only if PFC was enabled on
       priority p.
       (物理端口上优先级 p 发送端 pause 的持续时长（微秒）。注意：仅当优先级
       p 上启用了 PFC 时，此计数器才可用。)
     - Informative (信息型)

   * - `rx_prio[p]_buf_discard`
     - The number of packets discarded by device due to lack of per host receive
       buffers.
       (设备因缺少每主机接收缓冲区而丢弃的数据包数量。)
     - Informative (信息型)

   * - `rx_prio[p]_cong_discard`
     - The number of packets discarded by device due to per host congestion.
       (设备因每主机拥塞而丢弃的数据包数量。)
     - Informative (信息型)

   * - `rx_prio[p]_marked`
     - The number of packets ecn marked by device due to per host congestion.
       (设备因每主机拥塞而进行 ECN 标记的数据包数量。)
     - Informative (信息型)

   * - `rx_prio[p]_discards`
     - The number of packets discarded by device due to lack of receive buffers.
       (设备因缺少接收缓冲区而丢弃的数据包数量。)
     - Informative (信息型)

Device Counters
(设备计数器)
---------------

.. flat-table:: Device Counter Table
   :widths: 2 3 1

   * - Counter
       (计数器)
     - Description
       (描述)
     - Type
       (类型)

   * - `rx_pci_signal_integrity`
     - Counts physical layer PCIe signal integrity errors, the number of
       transitions to recovery due to Framing errors and CRC (dlp and tlp). If
       this counter is raising, try moving the adapter card to a different slot
       to rule out a bad PCI slot. Validate that you are running with the latest
       firmware available and latest server BIOS version.
       (统计物理层 PCIe 信号完整性错误，即因帧错误和 CRC（dlp 与 tlp）而进入
       恢复状态（recovery）的转换次数。如果此计数器不断升高，请尝试将适配卡
       移到其他插槽以排除 PCI 插槽故障。请确认运行的是最新的可用固件以及最新
       的服务器 BIOS 版本。)
     - Error (错误型)

   * - `tx_pci_signal_integrity`
     - Counts physical layer PCIe signal integrity errors, the number of
       transition to recovery initiated by the other side (moving to recovery
       due to getting TS/EIEOS). If this counter is raising, try moving the
       adapter card to a different slot to rule out a bad PCI slot. Validate
       that you are running with the latest firmware available and latest server
       BIOS version.
       (统计物理层 PCIe 信号完整性错误，即由对端发起的进入恢复状态的转换（因
       收到 TS/EIEOS 而进入 recovery）次数。如果此计数器不断升高，请尝试将适
       配卡移到其他插槽以排除 PCI 插槽故障。请确认运行的是最新的可用固件以及
       最新的服务器 BIOS 版本。)
     - Error (错误型)

   * - `outbound_pci_buffer_overflow`
     - The number of packets dropped due to pci buffer overflow. If this counter
       is raising in high rate, it might indicate that the receive traffic rate
       for a host is larger than the PCIe bus and therefore a congestion occurs.
       (因 PCI 缓冲区溢出而丢弃的数据包数量。如果此计数器高速增长，可能表明
       某主机的接收流量速率超过了 PCIe 总线能力，从而发生拥塞。)
     - Informative (信息型)

   * - `outbound_pci_stalled_rd`
     - The percentage (in the range 0...100) of time within the last second that
       the NIC had outbound non-posted reads requests but could not perform the
       operation due to insufficient posted credits.
       (过去一秒内，NIC 有出方向的 non-posted 读请求但因 posted credit 不足
       而无法执行操作的时间百分比（范围 0...100）。)
     - Informative (信息型)

   * - `outbound_pci_stalled_wr`
     - The percentage (in the range 0...100) of time within the last second that
       the NIC had outbound posted writes requests but could not perform the
       operation due to insufficient posted credits.
       (过去一秒内，NIC 有出方向的 posted 写请求但因 posted credit 不足而无
       法执行操作的时间百分比（范围 0...100）。)
     - Informative (信息型)

   * - `outbound_pci_stalled_rd_events`
     - The number of seconds where `outbound_pci_stalled_rd` was above 30%.
       (`outbound_pci_stalled_rd` 高于 30% 的秒数。)
     - Informative (信息型)

   * - `outbound_pci_stalled_wr_events`
     - The number of seconds where `outbound_pci_stalled_wr` was above 30%.
       (`outbound_pci_stalled_wr` 高于 30% 的秒数。)
     - Informative (信息型)

   * - `dev_out_of_buffer`
     - The number of times the device owned queue had not enough buffers
       allocated.
       (设备拥有的队列没有分配到足够缓冲区的次数。)
     - Error (错误型)

   * - `pci_bw_inbound_high`
     - The number of times the device crossed the high inbound pcie bandwidth
       threshold. To be compared to pci_bw_inbound_low to check if the device
       is in a congested state.
       If pci_bw_inbound_high == pci_bw_inbound_low then the device is not congested.
       If pci_bw_inbound_high > pci_bw_inbound_low then the device is congested.
       (设备越过入方向高 PCIe 带宽阈值的次数。应与 pci_bw_inbound_low 比较，
       以检查设备是否处于拥塞状态。若 pci_bw_inbound_high ==
       pci_bw_inbound_low，则设备未拥塞；若 pci_bw_inbound_high >
       pci_bw_inbound_low，则设备拥塞。)
     - Informative (信息型)

   * - `pci_bw_inbound_low`
     - The number of times the device crossed the low inbound PCIe bandwidth
       threshold. To be compared to pci_bw_inbound_high to check if the device
       is in a congested state.
       If pci_bw_inbound_high == pci_bw_inbound_low then the device is not congested.
       If pci_bw_inbound_high > pci_bw_inbound_low then the device is congested.
       (设备越过入方向低 PCIe 带宽阈值的次数。应与 pci_bw_inbound_high 比较，
       以检查设备是否处于拥塞状态。若 pci_bw_inbound_high ==
       pci_bw_inbound_low，则设备未拥塞；若 pci_bw_inbound_high >
       pci_bw_inbound_low，则设备拥塞。)
     - Informative (信息型)

   * - `pci_bw_outbound_high`
     - The number of times the device crossed the high outbound pcie bandwidth
       threshold. To be compared to pci_bw_outbound_low to check if the device
       is in a congested state.
       If pci_bw_outbound_high == pci_bw_outbound_low then the device is not congested.
       If pci_bw_outbound_high > pci_bw_outbound_low then the device is congested.
       (设备越过出方向高 PCIe 带宽阈值的次数。应与 pci_bw_outbound_low 比较，
       以检查设备是否处于拥塞状态。若 pci_bw_outbound_high ==
       pci_bw_outbound_low，则设备未拥塞；若 pci_bw_outbound_high >
       pci_bw_outbound_low，则设备拥塞。)
     - Informative (信息型)

   * - `pci_bw_outbound_low`
     - The number of times the device crossed the low outbound PCIe bandwidth
       threshold. To be compared to pci_bw_outbound_high to check if the device
       is in a congested state.
       If pci_bw_outbound_high == pci_bw_outbound_low then the device is not congested.
       If pci_bw_outbound_high > pci_bw_outbound_low then the device is congested.
       (设备越过出方向低 PCIe 带宽阈值的次数。应与 pci_bw_outbound_high 比较，
       以检查设备是否处于拥塞状态。若 pci_bw_outbound_high ==
       pci_bw_outbound_low，则设备未拥塞；若 pci_bw_outbound_high >
       pci_bw_outbound_low，则设备拥塞。)
     - Informative (信息型)

   * - `pci_bw_stale_event`
     - The number of times the device fired a PCIe congestion event but on query
       there was no change in state.
       (设备触发了 PCIe 拥塞事件、但查询时状态没有变化的次数。)
     - Informative
