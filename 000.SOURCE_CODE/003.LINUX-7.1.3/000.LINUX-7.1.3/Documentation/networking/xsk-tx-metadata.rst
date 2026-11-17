.. SPDX-License-Identifier: GPL-2.0

====================================
AF_XDP TX Metadata(AF_XDP TX 元数据)
====================================

This document describes how to enable offloads when transmitting packets
via :doc:`af_xdp`. Refer to :doc:`xdp-rx-metadata` on how to access similar
metadata on the receive side.
(本文档描述如何在使用 :doc:`af_xdp` 发送数据包时启用卸载(offload)。关于如何在接收
侧访问类似元数据，请参阅 :doc:`xdp-rx-metadata`。)

General Design(总体设计)
========================

The headroom for the metadata is reserved via ``tx_metadata_len`` and
``XDP_UMEM_TX_METADATA_LEN`` flag in ``struct xdp_umem_reg``. The metadata
length is therefore the same for every socket that shares the same umem.
The metadata layout is a fixed UAPI, refer to ``union xsk_tx_metadata`` in
``include/uapi/linux/if_xdp.h``. Thus, generally, the ``tx_metadata_len``
field above should contain ``sizeof(union xsk_tx_metadata)``.
(元数据的预留空间通过 ``struct xdp_umem_reg`` 中的 ``tx_metadata_len`` 和
``XDP_UMEM_TX_METADATA_LEN`` 标志来保留。因此，对于共享同一 umem 的每个套接字，
元数据长度都相同。元数据布局是固定的 UAPI，参见 ``include/uapi/linux/if_xdp.h``
中的 ``union xsk_tx_metadata``。因此，一般而言，上面的 ``tx_metadata_len`` 字段
应包含 ``sizeof(union xsk_tx_metadata)``。)

Note that in the original implementation the ``XDP_UMEM_TX_METADATA_LEN``
flag was not required. Applications might attempt to create a umem
with a flag first and if it fails, do another attempt without a flag.
(注意，在最初的实现中并不需要 ``XDP_UMEM_TX_METADATA_LEN`` 标志。应用程序可以先
尝试带标志创建 umem，如果失败，再不带标志重试一次。)

The headroom and the metadata itself should be located right before
``xdp_desc->addr`` in the umem frame. Within a frame, the metadata
layout is as follows::

           tx_metadata_len
     /                         \
    +-----------------+---------+----------------------------+
    | xsk_tx_metadata | padding |          payload           |
    +-----------------+---------+----------------------------+
                                ^
                                |
                          xdp_desc->addr

(预留空间和元数据本身应当位于 umem 帧中紧挨 ``xdp_desc->addr`` 之前的位置。在
一个帧内，元数据布局如下：)

An AF_XDP application can request headrooms larger than ``sizeof(struct
xsk_tx_metadata)``. The kernel will ignore the padding (and will still
use ``xdp_desc->addr - tx_metadata_len`` to locate
the ``xsk_tx_metadata``). For the frames that shouldn't carry
any metadata (i.e., the ones that don't have ``XDP_TX_METADATA`` option),
the metadata area is ignored by the kernel as well.
(AF_XDP 应用程序可以请求比 ``sizeof(struct xsk_tx_metadata)`` 更大的预留空间。
内核会忽略填充(padding)(并且仍会使用 ``xdp_desc->addr - tx_metadata_len`` 来
定位 ``xsk_tx_metadata``)。对于不应携带任何元数据的帧(即没有 ``XDP_TX_METADATA``
选项的那些帧)，元数据区域同样会被内核忽略。)

The flags field enables the particular offload:
(flags 字段用于启用特定的卸载：)

- ``XDP_TXMD_FLAGS_TIMESTAMP``: requests the device to put transmission
  timestamp into ``tx_timestamp`` field of ``union xsk_tx_metadata``.
  (``XDP_TXMD_FLAGS_TIMESTAMP``：请求设备把发送时间戳放入 ``union xsk_tx_metadata``
  的 ``tx_timestamp`` 字段。)
- ``XDP_TXMD_FLAGS_CHECKSUM``: requests the device to calculate L4
  checksum. ``csum_start`` specifies byte offset of where the checksumming
  should start and ``csum_offset`` specifies byte offset where the
  device should store the computed checksum.
  (``XDP_TXMD_FLAGS_CHECKSUM``：请求设备计算 L4 校验和。``csum_start`` 指定校验
  和计算起点的字节偏移，``csum_offset`` 指定设备应存放计算出的校验和的字节偏移。)
- ``XDP_TXMD_FLAGS_LAUNCH_TIME``: requests the device to schedule the
  packet for transmission at a pre-determined time called launch time. The
  value of launch time is indicated by ``launch_time`` field of
  ``union xsk_tx_metadata``.
  (``XDP_TXMD_FLAGS_LAUNCH_TIME``：请求设备在称为发射时间(launch time)的预定时刻
  调度数据包进行发送。发射时间的值由 ``union xsk_tx_metadata`` 的
  ``launch_time`` 字段给出。)

Besides the flags above, in order to trigger the offloads, the first
packet's ``struct xdp_desc`` descriptor should set ``XDP_TX_METADATA``
bit in the ``options`` field. Also note that in a multi-buffer packet
only the first chunk should carry the metadata.
(除了上述标志之外，为了触发这些卸载，第一个数据包的 ``struct xdp_desc`` 描述符
应在 ``options`` 字段中设置 ``XDP_TX_METADATA`` 位。还要注意，在多缓冲区
(multi-buffer)数据包中，只有第一个分块应携带元数据。)

Software TX Checksum(软件 TX 校验和)
====================================

For development and testing purposes its possible to pass
``XDP_UMEM_TX_SW_CSUM`` flag to ``XDP_UMEM_REG`` UMEM registration call.
In this case, when running in ``XDK_COPY`` mode, the TX checksum
is calculated on the CPU. Do not enable this option in production because
it will negatively affect performance.
(出于开发和测试的目的，可以向 ``XDP_UMEM_REG`` UMEM 注册调用传递
``XDP_UMEM_TX_SW_CSUM`` 标志。在这种情况下，当以 ``XDK_COPY`` 模式运行时，TX
校验和会在 CPU 上计算。不要在生产环境中启用此选项，因为它会对性能产生负面影响。)

Launch Time(发射时间)
=====================

The value of the requested launch time should be based on the device's PTP
Hardware Clock (PHC) to ensure accuracy. AF_XDP takes a different data path
compared to the ETF queuing discipline, which organizes packets and delays
their transmission. Instead, AF_XDP immediately hands off the packets to
the device driver without rearranging their order or holding them prior to
transmission. Since the driver maintains FIFO behavior and does not perform
packet reordering, a packet with a launch time request will block other
packets in the same Tx Queue until it is sent. Therefore, it is recommended
to allocate separate queue for scheduling traffic that is intended for
future transmission.
(所请求的发射时间的值应基于设备的 PTP 硬件时钟(PHC)，以确保准确性。与 ETF 排队
规则(它会对数据包进行整理并延迟其发送)不同，AF_XDP 走的是另一条数据路径：它会
立即把数据包交给设备驱动程序，而不重新排列其顺序，也不在发送前扣留它们。由于
驱动程序保持 FIFO 行为且不执行数据包重排，一个带有发射时间请求的数据包会阻塞
同一 Tx 队列中的其他数据包，直到它被发送出去。因此，建议分配一个单独的队列来
调度打算在未来发送的流量。)

In scenarios where the launch time offload feature is disabled, the device
driver is expected to disregard the launch time request. For correct
interpretation and meaningful operation, the launch time should never be
set to a value larger than the farthest programmable time in the future
(the horizon). Different devices have different hardware limitations on the
launch time offload feature.
(在发射时间卸载特性被禁用的场景中，设备驱动程序应忽略发射时间请求。为了获得
正确的解释和有意义的行为，发射时间绝不应设置为大于未来最远可编程时刻
(the horizon)的值。不同设备对发射时间卸载特性有不同的硬件限制。)

stmmac driver(stmmac 驱动程序)
------------------------------

For stmmac, TSO and launch time (TBS) features are mutually exclusive for
each individual Tx Queue. By default, the driver configures Tx Queue 0 to
support TSO and the rest of the Tx Queues to support TBS. The launch time
hardware offload feature can be enabled or disabled by using the tc-etf
command to call the driver's ndo_setup_tc() callback.
(对于 stmmac，TSO 和发射时间(TBS)特性对每个单独的 Tx 队列是互斥的。默认情况下，
驱动程序配置 Tx 队列 0 支持 TSO，其余 Tx 队列支持 TBS。可以使用 tc-etf 命令调用
驱动程序的 ndo_setup_tc() 回调来启用或禁用发射时间硬件卸载特性。)

The value of the launch time that is programmed in the Enhanced Normal
Transmit Descriptors is a 32-bit value, where the most significant 8 bits
represent the time in seconds and the remaining 24 bits represent the time
in 256 ns increments. The programmed launch time is compared against the
PTP time (bits[39:8]) and rolls over after 256 seconds. Therefore, the
horizon of the launch time for dwmac4 and dwxlgmac2 is 128 seconds in the
future.
(编程写入增强普通发送描述符(Enhanced Normal Transmit Descriptors)中的发射时间
值是一个 32 位值，其中最高的 8 位表示以秒为单位的时间，其余 24 位表示以
256 ns 为步长的时间。编程写入的发射时间会与 PTP 时间(bits[39:8])进行比较，并
在 256 秒后回绕。因此，dwmac4 和 dwxlgmac2 的发射时间地平线是未来 128 秒。)

igc driver(igc 驱动程序)
------------------------

For igc, all four Tx Queues support the launch time feature. The launch
time hardware offload feature can be enabled or disabled by using the
tc-etf command to call the driver's ndo_setup_tc() callback. When entering
TSN mode, the igc driver will reset the device and create a default Qbv
schedule with a 1-second cycle time, with all Tx Queues open at all times.
(对于 igc，全部四个 Tx 队列都支持发射时间特性。可以使用 tc-etf 命令调用驱动
程序的 ndo_setup_tc() 回调来启用或禁用发射时间硬件卸载特性。进入 TSN 模式时，
igc 驱动程序会复位设备并创建一个默认的 Qbv 调度表，其周期时间为 1 秒，所有
Tx 队列在任何时候都是打开的。)

The value of the launch time that is programmed in the Advanced Transmit
Context Descriptor is a relative offset to the starting time of the Qbv
transmission window of the queue. The Frst flag of the descriptor can be
set to schedule the packet for the next Qbv cycle. Therefore, the horizon
of the launch time for i225 and i226 is the ending time of the next cycle
of the Qbv transmission window of the queue. For example, when the Qbv
cycle time is set to 1 second, the horizon of the launch time ranges
from 1 second to 2 seconds, depending on where the Qbv cycle is currently
running.
(编程写入高级发送上下文描述符(Advanced Transmit Context Descriptor)中的发射
时间值是相对于该队列 Qbv 发送窗口起始时间的偏移量。可以设置描述符的 Frst
标志，把数据包调度到下一个 Qbv 周期。因此，i225 和 i226 的发射时间地平线是该
队列 Qbv 发送窗口下一个周期的结束时刻。例如，当 Qbv 周期时间设置为 1 秒时，
发射时间地平线的范围是 1 秒到 2 秒，取决于 Qbv 周期当前运行到什么位置。)

Querying Device Capabilities(查询设备能力)
==========================================

Every devices exports its offloads capabilities via netlink netdev family.
Refer to ``xsk-flags`` features bitmask in
``Documentation/netlink/specs/netdev.yaml``.
(每个设备都通过 netlink netdev family 导出其卸载能力。参见
``Documentation/netlink/specs/netdev.yaml`` 中的 ``xsk-flags`` 特性位掩码。)

- ``tx-timestamp``: device supports ``XDP_TXMD_FLAGS_TIMESTAMP``
  (``tx-timestamp``：设备支持 ``XDP_TXMD_FLAGS_TIMESTAMP``)
- ``tx-checksum``: device supports ``XDP_TXMD_FLAGS_CHECKSUM``
  (``tx-checksum``：设备支持 ``XDP_TXMD_FLAGS_CHECKSUM``)
- ``tx-launch-time-fifo``: device supports ``XDP_TXMD_FLAGS_LAUNCH_TIME``
  (``tx-launch-time-fifo``：设备支持 ``XDP_TXMD_FLAGS_LAUNCH_TIME``)

See ``tools/net/ynl/samples/netdev.c`` on how to query this information.
(关于如何查询此信息，参见 ``tools/net/ynl/samples/netdev.c``。)

Example(示例)
=============

See ``tools/testing/selftests/bpf/xdp_hw_metadata.c`` for an example
program that handles TX metadata. Also see https://github.com/fomichev/xskgen
for a more bare-bones example.
(处理 TX 元数据的示例程序参见 ``tools/testing/selftests/bpf/xdp_hw_metadata.c``。
更精简的示例另见 https://github.com/fomichev/xskgen。)
