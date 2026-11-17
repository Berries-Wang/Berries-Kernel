.. SPDX-License-Identifier: GPL-2.0+

===================================================================================================================================
OPEN Alliance 10BASE-T1x MAC-PHY Serial Interface (TC6) Framework Support(OPEN Alliance 10BASE-T1x MAC-PHY 串行接口 (TC6) 框架支持)
===================================================================================================================================

Introduction(简介)
------------------

The IEEE 802.3cg project defines two 10 Mbit/s PHYs operating over a
single pair of conductors. The 10BASE-T1L (Clause 146) is a long reach
PHY supporting full duplex point-to-point operation over 1 km of single
balanced pair of conductors. The 10BASE-T1S (Clause 147) is a short reach
PHY supporting full / half duplex point-to-point operation over 15 m of
single balanced pair of conductors, or half duplex multidrop bus
operation over 25 m of single balanced pair of conductors.
(IEEE 802.3cg 项目定义了两种在单对导体上工作的 10 Mbit/s PHY。10BASE-T1L (第 146 章) 是一种长距离 PHY，支持在 1 km 单对平衡导体上进行全双工点对点操作。10BASE-T1S (第 147 章) 是一种短距离 PHY，支持在 15 m 单对平衡导体上进行全双工/半双工点对点操作，或在 25 m 单对平衡导体上进行半双工多点总线操作。)

Furthermore, the IEEE 802.3cg project defines the new Physical Layer
Collision Avoidance (PLCA) Reconciliation Sublayer (Clause 148) meant to
provide improved determinism to the CSMA/CD media access method. PLCA
works in conjunction with the 10BASE-T1S PHY operating in multidrop mode.
(此外，IEEE 802.3cg 项目定义了新的物理层冲突避免 (PLCA) 汇聚子层 (第 148 章)，旨在为 CSMA/CD 介质访问方法提供更好的确定性。PLCA 与工作在多点模式下的 10BASE-T1S PHY 配合工作。)

The aforementioned PHYs are intended to cover the low-speed / low-cost
applications in industrial and automotive environment. The large number
of pins (16) required by the MII interface, which is specified by the
IEEE 802.3 in Clause 22, is one of the major cost factors that need to be
addressed to fulfil this objective.
(上述 PHY 旨在覆盖工业和汽车环境中的低速/低成本应用。IEEE 802.3 第 22 章规定的 MII 接口所需的大量引脚 (16 个) 是实现这一目标所需解决的主要成本因素之一。)

The MAC-PHY solution integrates an IEEE Clause 4 MAC and a 10BASE-T1x PHY
exposing a low pin count Serial Peripheral Interface (SPI) to the host
microcontroller. This also enables the addition of Ethernet functionality
to existing low-end microcontrollers which do not integrate a MAC
controller.
(MAC-PHY 解决方案集成了 IEEE 第 4 章 MAC 和 10BASE-T1x PHY，向主机微控制器暴露一个低引脚数的串行外设接口 (SPI)。这也使得能够为未集成 MAC 控制器的现有低端微控制器添加以太网功能。)

Overview(概述)
--------------

The MAC-PHY is specified to carry both data (Ethernet frames) and control
(register access) transactions over a single full-duplex serial peripheral
interface.
(MAC-PHY 被规范为通过单个全双工串行外设接口承载数据 (以太网帧) 和控制 (寄存器访问) 事务。)

Protocol Overview(协议概述)
---------------------------

Two types of transactions are defined in the protocol: data transactions
for Ethernet frame transfers and control transactions for register
read/write transfers. A chunk is the basic element of data transactions
and is composed of 4 bytes of overhead plus 64 bytes of payload size for
each chunk. Ethernet frames are transferred over one or more data chunks.
Control transactions consist of one or more register read/write control
commands.
(协议中定义了两种事务：用于以太网帧传输的数据事务，以及用于寄存器读/写传输的控制事务。块 (chunk) 是数据事务的基本元素，由 4 字节的开销加上每块 64 字节的有效载荷组成。以太网帧通过一个或多个数据块传输。控制事务由一个或多个寄存器读/写控制命令组成。)

SPI transactions are initiated by the SPI host with the assertion of CSn
low to the MAC-PHY and ends with the deassertion of CSn high. In between
each SPI transaction, the SPI host may need time for additional
processing and to setup the next SPI data or control transaction.
(SPI 事务由 SPI 主机将 CSn 置低 (有效) 发起，并以 CSn 释放 (置高) 结束。在每两次 SPI 事务之间，SPI 主机可能需要时间进行额外的处理，并为下一个 SPI 数据或控制事务做准备。)

SPI data transactions consist of an equal number of transmit (TX) and
receive (RX) chunks. Chunks in both transmit and receive directions may
or may not contain valid frame data independent from each other, allowing
for the simultaneous transmission and reception of different length
frames.
(SPI 数据事务由数量相等的发送 (TX) 块和接收 (RX) 块组成。发送和接收两个方向上的块可以彼此独立地包含或不包含有效帧数据，从而允许同时发送和接收不同长度的帧。)

Each transmit data chunk begins with a 32-bit data header followed by a
data chunk payload on MOSI. The data header indicates whether transmit
frame data is present and provides the information to determine which
bytes of the payload contain valid frame data.
(每个发送数据块以一个 32 位数据头开始，随后是 MOSI 上的数据块有效载荷。数据头指示是否存在发送帧数据，并提供用于确定有效载荷中哪些字节包含有效帧数据的信息。)

In parallel, receive data chunks are received on MISO. Each receive data
chunk consists of a data chunk payload ending with a 32-bit data footer.
The data footer indicates if there is receive frame data present within
the payload or not and provides the information to determine which bytes
of the payload contain valid frame data.
(与此同时，接收数据块在 MISO 上被接收。每个接收数据块由一个数据块有效载荷组成，并以 32 位数据尾结束。数据尾指示有效载荷中是否存在接收帧数据，并提供用于确定有效载荷中哪些字节包含有效帧数据的信息。)

Reference(参考)
---------------

10BASE-T1x MAC-PHY Serial Interface Specification,
(《10BASE-T1x MAC-PHY 串行接口规范》，)

Link: https://opensig.org/download/document/OPEN_Alliance_10BASET1x_MAC-PHY_Serial_Interface_V1.1.pdf
(链接：https://opensig.org/download/document/OPEN_Alliance_10BASET1x_MAC-PHY_Serial_Interface_V1.1.pdf)

Hardware Architecture(硬件架构)
-------------------------------

.. code-block:: none

  +----------+      +-------------------------------------+
  |          |      |                MAC-PHY              |
  |          |<---->| +-----------+  +-------+  +-------+ |
  | SPI Host |      | | SPI Slave |  |  MAC  |  |  PHY  | |
  |          |      | +-----------+  +-------+  +-------+ |
  +----------+      +-------------------------------------+

Software Architecture(软件架构)
-------------------------------

.. code-block:: none

  +----------------------------------------------------------+
  |                 Networking Subsystem                     |
  +----------------------------------------------------------+
            / \                             / \
             |                               |
             |                               |
            \ /                              |
  +----------------------+     +-----------------------------+
  |     MAC Driver       |<--->| OPEN Alliance TC6 Framework |
  +----------------------+     +-----------------------------+
            / \                             / \
             |                               |
             |                               |
             |                              \ /
  +----------------------------------------------------------+
  |                    SPI Subsystem                         |
  +----------------------------------------------------------+
                          / \
                           |
                           |
                          \ /
  +----------------------------------------------------------+
  |                10BASE-T1x MAC-PHY Device                 |
  +----------------------------------------------------------+

Implementation(实现)
--------------------

MAC Driver(MAC 驱动程序)
~~~~~~~~~~~~~~~~~~~~~~~~

- Probed by SPI subsystem.
  (由 SPI 子系统探测。)

- Initializes OA TC6 framework for the MAC-PHY.
  (为 MAC-PHY 初始化 OA TC6 框架。)

- Registers and configures the network device.
  (注册并配置网络设备。)

- Sends the tx ethernet frames from n/w subsystem to OA TC6 framework.
  (将来自网络子系统的待发送以太网帧发送给 OA TC6 框架。)

OPEN Alliance TC6 Framework(OPEN Alliance TC6 框架)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Initializes PHYLIB interface.
  (初始化 PHYLIB 接口。)

- Registers mac-phy interrupt.
  (注册 MAC-PHY 中断。)

- Performs mac-phy register read/write operation using the control
  transaction protocol specified in the OPEN Alliance 10BASE-T1x MAC-PHY
  Serial Interface specification.
  (使用 OPEN Alliance 10BASE-T1x MAC-PHY 串行接口规范中规定的控制事务协议执行 MAC-PHY 寄存器读/写操作。)

- Performs Ethernet frames transaction using the data transaction protocol
  for Ethernet frames specified in the OPEN Alliance 10BASE-T1x MAC-PHY
  Serial Interface specification.
  (使用 OPEN Alliance 10BASE-T1x MAC-PHY 串行接口规范中规定的以太网帧数据事务协议执行以太网帧事务。)

- Forwards the received Ethernet frame from 10Base-T1x MAC-PHY to n/w
  subsystem.
  (将从 10Base-T1x MAC-PHY 接收到的以太网帧转发给网络子系统。)

Data Transaction(数据事务)
~~~~~~~~~~~~~~~~~~~~~~~~~~

The Ethernet frames that are typically transferred from the SPI host to
the MAC-PHY will be converted into multiple transmit data chunks. Each
transmit data chunk will have a 4 bytes header which contains the
information needed to determine the validity and the location of the
transmit frame data within the 64 bytes data chunk payload.
(通常从 SPI 主机传输到 MAC-PHY 的以太网帧将被转换为多个发送数据块。每个发送数据块都有一个 4 字节的头，其中包含确定发送帧数据在 64 字节数据块有效载荷中的有效性和位置所需的信息。)

.. code-block:: none

  +---------------------------------------------------+
  |                     Tx Chunk                      |
  | +---------------------------+  +----------------+ |   MOSI
  | | 64 bytes chunk payload    |  | 4 bytes header | |------------>
  | +---------------------------+  +----------------+ |
  +---------------------------------------------------+

4 bytes header contains the below fields,
(4 字节的头包含以下字段，)

DNC (Bit 31) - Data-Not-Control flag. This flag specifies the type of SPI
               transaction. For TX data chunks, this bit shall be ’1’.
               0 - Control command
               1 - Data chunk
(DNC (位 31) —— 数据-非-控制 (Data-Not-Control) 标志。此标志指定 SPI 事务的类型。对于 TX 数据块，此位应为 "1"。0 - 控制命令；1 - 数据块)

SEQ (Bit 30) - Data Chunk Sequence. This bit is used to indicate an
               even/odd transmit data chunk sequence to the MAC-PHY.
(SEQ (位 30) —— 数据块序列。此位用于向 MAC-PHY 指示发送数据块的偶/奇序列。)

NORX (Bit 29) - No Receive flag. The SPI host may set this bit to prevent
                the MAC-PHY from conveying RX data on the MISO for the
                current chunk (DV = 0 in the footer), indicating that the
                host would not process it. Typically, the SPI host should
                set NORX = 0 indicating that it will accept and process
                any receive frame data within the current chunk.
(NORX (位 29) —— 无接收标志。SPI 主机可以设置此位，以阻止 MAC-PHY 在当前块的 MISO 上传送 RX 数据 (页脚中 DV = 0)，表明主机不会处理它。通常，SPI 主机应将 NORX 设置为 0，表示它将接受并处理当前块内的任何接收帧数据。)

RSVD (Bit 28..24) - Reserved: All reserved bits shall be ‘0’.
(RSVD (位 28..24) —— 保留：所有保留位应为 "0"。)

VS (Bit 23..22) - Vendor Specific. These bits are implementation specific.
                  If the MAC-PHY does not implement these bits, the host
                  shall set them to ‘0’.
(VS (位 23..22) —— 厂商专用。这些位由具体实现决定。如果 MAC-PHY 未实现这些位，主机应将其设置为 "0"。)

DV (Bit 21) - Data Valid flag. The SPI host uses this bit to indicate
              whether the current chunk contains valid transmit frame data
              (DV = 1) or not (DV = 0). When ‘0’, the MAC-PHY ignores the
              chunk payload. Note that the receive path is unaffected by
              the setting of the DV bit in the data header.
(DV (位 21) —— 数据有效标志。SPI 主机使用此位指示当前块是否包含有效的发送帧数据 (DV = 1) 或不包含 (DV = 0)。当为 "0" 时，MAC-PHY 会忽略块有效载荷。注意，接收路径不受数据头中 DV 位设置的影响。)

SV (Bit 20) - Start Valid flag. The SPI host shall set this bit when the
              beginning of an Ethernet frame is present in the current
              transmit data chunk payload. Otherwise, this bit shall be
              zero. This bit is not to be confused with the Start-of-Frame
              Delimiter (SFD) byte described in IEEE 802.3 [2].
(SV (位 20) —— 起始有效标志。当当前发送数据块有效载荷中存在以太网帧的开头时，SPI 主机应设置此位。否则，此位应为零。此位不应与 IEEE 802.3 [2] 中描述的帧起始定界符 (SFD) 字节混淆。)

SWO (Bit 19..16) - Start Word Offset. When SV = 1, this field shall
                   contain the 32-bit word offset into the transmit data
                   chunk payload that points to the start of a new
                   Ethernet frame to be transmitted. The host shall write
                   this field as zero when SV = 0.
(SWO (位 19..16) —— 起始字偏移。当 SV = 1 时，此字段应包含发送数据块有效载荷内的 32 位字偏移，该偏移指向待发送新以太网帧的起始位置。当 SV = 0 时，主机应将此字段写为零。)

RSVD (Bit 15) - Reserved: All reserved bits shall be ‘0’.
(RSVD (位 15) —— 保留：所有保留位应为 "0"。)

EV (Bit 14) - End Valid flag. The SPI host shall set this bit when the end
              of an Ethernet frame is present in the current transmit data
              chunk payload. Otherwise, this bit shall be zero.
(EV (位 14) —— 结束有效标志。当当前发送数据块有效载荷中存在以太网帧的结尾时，SPI 主机应设置此位。否则，此位应为零。)

EBO (Bit 13..8) - End Byte Offset. When EV = 1, this field shall contain
                  the byte offset into the transmit data chunk payload
                  that points to the last byte of the Ethernet frame to
                  transmit. This field shall be zero when EV = 0.
(EBO (位 13..8) —— 结束字节偏移。当 EV = 1 时，此字段应包含发送数据块有效载荷内的字节偏移，该偏移指向待发送以太网帧的最后一个字节。当 EV = 0 时，此字段应为零。)

TSC (Bit 7..6) - Timestamp Capture. Request a timestamp capture when the
                 frame is transmitted onto the network.
                 00 - Do not capture a timestamp
                 01 - Capture timestamp into timestamp capture register A
                 10 - Capture timestamp into timestamp capture register B
                 11 - Capture timestamp into timestamp capture register C
(TSC (位 7..6) —— 时间戳捕获。请求在帧发送到网络时捕获时间戳。00 - 不捕获时间戳；01 - 将时间戳捕获到时间戳捕获寄存器 A；10 - 将时间戳捕获到时间戳捕获寄存器 B；11 - 将时间戳捕获到时间戳捕获寄存器 C)

RSVD (Bit 5..1) - Reserved: All reserved bits shall be ‘0’.
(RSVD (位 5..1) —— 保留：所有保留位应为 "0"。)

P (Bit 0) - Parity. Parity bit calculated over the transmit data header.
            Method used is odd parity.
(P (位 0) —— 奇偶校验。对发送数据头计算的奇偶校验位。采用奇校验方法。)

The number of buffers available in the MAC-PHY to store the incoming
transmit data chunk payloads is represented as transmit credits. The
available transmit credits in the MAC-PHY can be read either from the
Buffer Status Register or footer (Refer below for the footer info)
received from the MAC-PHY. The SPI host should not write more data chunks
than the available transmit credits as this will lead to transmit buffer
overflow error.
(MAC-PHY 中可用于存储传入发送数据块有效载荷的缓冲区数量表示为发送信用 (transmit credits)。MAC-PHY 中可用的发送信用可以从缓冲区状态寄存器 (Buffer Status Register) 或从 MAC-PHY 接收到的页脚中读取 (页脚信息见下文)。SPI 主机写入的数据块数量不应超过可用的发送信用，否则将导致发送缓冲区溢出错误。)

In case the previous data footer had no transmit credits available and
once the transmit credits become available for transmitting transmit data
chunks, the MAC-PHY interrupt is asserted to SPI host. On reception of the
first data header this interrupt will be deasserted and the received
footer for the first data chunk will have the transmit credits available
information.
(如果前一个数据页脚指示没有可用的发送信用，而发送信用一旦变得可用于发送数据块，MAC-PHY 中断就会向 SPI 主机发出。在收到第一个数据头后，该中断将被解除，并且为第一个数据块接收到的页脚将包含可用发送信用信息。)

The Ethernet frames that are typically transferred from MAC-PHY to SPI
host will be sent as multiple receive data chunks. Each receive data
chunk will have 64 bytes of data chunk payload followed by 4 bytes footer
which contains the information needed to determine the validity and the
location of the receive frame data within the 64 bytes data chunk payload.
(通常从 MAC-PHY 传输到 SPI 主机的以太网帧将作为多个接收数据块发送。每个接收数据块有 64 字节的数据块有效载荷，后跟 4 字节的页脚，其中包含确定接收帧数据在 64 字节数据块有效载荷中的有效性和位置所需的信息。)

.. code-block:: none

  +---------------------------------------------------+
  |                     Rx Chunk                      |
  | +----------------+  +---------------------------+ |   MISO
  | | 4 bytes footer |  | 64 bytes chunk payload    | |------------>
  | +----------------+  +---------------------------+ |
  +---------------------------------------------------+

4 bytes footer contains the below fields,
(4 字节的页脚包含以下字段，)

EXST (Bit 31) - Extended Status. This bit is set when any bit in the
                STATUS0 or STATUS1 registers are set and not masked.
(EXST (位 31) —— 扩展状态。当 STATUS0 或 STATUS1 寄存器中有任何位被置位且未被屏蔽时，此位被置位。)

HDRB (Bit 30) - Received Header Bad. When set, indicates that the MAC-PHY
                received a control or data header with a parity error.
(HDRB (位 30) —— 接收头错误。置位时，表示 MAC-PHY 接收到了带有奇偶校验错误的控制头或数据头。)

SYNC (Bit 29) - Configuration Synchronized flag. This bit reflects the
                state of the SYNC bit in the CONFIG0 configuration
                register (see Table 12). A zero indicates that the MAC-PHY
                configuration may not be as expected by the SPI host.
                Following configuration, the SPI host sets the
                corresponding bitin the configuration register which is
                reflected in this field.
(SYNC (位 29) —— 配置已同步标志。此位反映 CONFIG0 配置寄存器中 SYNC 位的状态 (见表 12)。为零表示 MAC-PHY 的配置可能与 SPI 主机预期的不一致。完成配置后，SPI 主机会设置配置寄存器中的相应位，该位会反映在此字段中。)

RCA (Bit 28..24) - Receive Chunks Available. The RCA field indicates to
                   the SPI host the minimum number of additional receive
                   data chunks of frame data that are available for
                   reading beyond the current receive data chunk. This
                   field is zero when there is no receive frame data
                   pending in the MAC-PHY’s buffer for reading.
(RCA (位 28..24) —— 可用接收块。RCA 字段向 SPI 主机指示在当前接收数据块之外尚可供读取的帧数据接收数据块的最少数量。当 MAC-PHY 缓冲区中没有待读取的接收帧数据时，此字段为零。)

VS (Bit 23..22) - Vendor Specific. These bits are implementation specific.
                  If not implemented, the MAC-PHY shall set these bits to
                  ‘0’.
(VS (位 23..22) —— 厂商专用。这些位由具体实现决定。如果未实现，MAC-PHY 应将这些位设置为 "0"。)

DV (Bit 21) - Data Valid flag. The MAC-PHY uses this bit to indicate
              whether the current receive data chunk contains valid
              receive frame data (DV = 1) or not (DV = 0). When ‘0’, the
              SPI host shall ignore the chunk payload.
(DV (位 21) —— 数据有效标志。MAC-PHY 使用此位指示当前接收数据块是否包含有效的接收帧数据 (DV = 1) 或不包含 (DV = 0)。当为 "0" 时，SPI 主机应忽略块有效载荷。)

SV (Bit 20) - Start Valid flag. The MAC-PHY sets this bit when the current
              chunk payload contains the start of an Ethernet frame.
              Otherwise, this bit is zero. The SV bit is not to be
              confused with the Start-of-Frame Delimiter (SFD) byte
              described in IEEE 802.3 [2].
(SV (位 20) —— 起始有效标志。当当前块有效载荷包含以太网帧的开头时，MAC-PHY 设置此位。否则，此位为零。SV 位不应与 IEEE 802.3 [2] 中描述的帧起始定界符 (SFD) 字节混淆。)

SWO (Bit 19..16) - Start Word Offset. When SV = 1, this field contains the
                   32-bit word offset into the receive data chunk payload
                   containing the first byte of a new received Ethernet
                   frame. When a receive timestamp has been added to the
                   beginning of the received Ethernet frame (RTSA = 1)
                   then SWO points to the most significant byte of the
                   timestamp. This field will be zero when SV = 0.
(SWO (位 19..16) —— 起始字偏移。当 SV = 1 时，此字段包含接收数据块有效载荷内的 32 位字偏移，该偏移指向新收到的以太网帧的第一个字节。当接收到的以太网帧开头加入了接收时间戳时 (RTSA = 1)，SWO 指向时间戳的最高有效字节。当 SV = 0 时，此字段将为零。)

FD (Bit 15) - Frame Drop. When set, this bit indicates that the MAC has
              detected a condition for which the SPI host should drop the
              received Ethernet frame. This bit is only valid at the end
              of a received Ethernet frame (EV = 1) and shall be zero at
              all other times.
(FD (位 15) —— 帧丢弃。置位时，此位表示 MAC 检测到了 SPI 主机应丢弃接收以太网帧的情况。此位仅在接收以太网帧的结尾处 (EV = 1) 有效，在其他所有时间为零。)

EV (Bit 14) - End Valid flag. The MAC-PHY sets this bit when the end of a
              received Ethernet frame is present in this receive data
              chunk payload.
(EV (位 14) —— 结束有效标志。当此接收数据块有效载荷中存在接收以太网帧的结尾时，MAC-PHY 设置此位。)

EBO (Bit 13..8) - End Byte Offset: When EV = 1, this field contains the
                  byte offset into the receive data chunk payload that
                  locates the last byte of the received Ethernet frame.
                  This field is zero when EV = 0.
(EBO (位 13..8) —— 结束字节偏移：当 EV = 1 时，此字段包含接收数据块有效载荷内的字节偏移，该偏移定位接收以太网帧的最后一个字节。当 EV = 0 时，此字段为零。)

RTSA (Bit 7) - Receive Timestamp Added. This bit is set when a 32-bit or
               64-bit timestamp has been added to the beginning of the
               received Ethernet frame. The MAC-PHY shall set this bit to
               zero when SV = 0.
(RTSA (位 7) —— 已加入接收时间戳。当 32 位或 64 位时间戳被加入到接收以太网帧的开头时，此位被置位。当 SV = 0 时，MAC-PHY 应将此位设置为零。)

RTSP (Bit 6) - Receive Timestamp Parity. Parity bit calculated over the
               32-bit/64-bit timestamp added to the beginning of the
               received Ethernet frame. Method used is odd parity. The
               MAC-PHY shall set this bit to zero when RTSA = 0.
(RTSP (位 6) —— 接收时间戳奇偶校验。对接收以太网帧开头加入的 32 位/64 位时间戳计算的奇偶校验位。采用奇校验方法。当 RTSA = 0 时，MAC-PHY 应将此位设置为零。)

TXC (Bit 5..1) - Transmit Credits. This field contains the minimum number
                 of transmit data chunks of frame data that the SPI host
                 can write in a single transaction without incurring a
                 transmit buffer overflow error.
(TXC (位 5..1) —— 发送信用。此字段包含 SPI 主机在单个事务中可以写入而不会引发发送缓冲区溢出错误的帧数据发送数据块的最少数量。)

P (Bit 0) - Parity. Parity bit calculated over the receive data footer.
            Method used is odd parity.
(P (位 0) —— 奇偶校验。对接收数据页脚计算的奇偶校验位。采用奇校验方法。)

SPI host will initiate the data receive transaction based on the receive
chunks available in the MAC-PHY which is provided in the receive chunk
footer (RCA - Receive Chunks Available). SPI host will create data invalid
transmit data chunks (empty chunks) or data valid transmit data chunks in
case there are valid Ethernet frames to transmit to the MAC-PHY. The
receive chunks available in MAC-PHY can be read either from the Buffer
Status Register or footer.
(SPI 主机将根据 MAC-PHY 中可用的接收块 (由接收块页脚提供，即 RCA —— 可用接收块) 发起数据接收事务。如果有有效的以太网帧要发送到 MAC-PHY，SPI 主机将创建数据无效的发送数据块 (空块) 或数据有效的发送数据块。MAC-PHY 中可用的接收块可以从缓冲区状态寄存器或页脚中读取。)

In case the previous data footer had no receive data chunks available and
once the receive data chunks become available again for reading, the
MAC-PHY interrupt is asserted to SPI host. On reception of the first data
header this interrupt will be deasserted and the received footer for the
first data chunk will have the receive chunks available information.
(如果前一个数据页脚指示没有可用的接收数据块，而接收数据块一旦再次变得可供读取，MAC-PHY 中断就会向 SPI 主机发出。在收到第一个数据头后，该中断将被解除，并且为第一个数据块接收到的页脚将包含可用接收块信息。)

MAC-PHY Interrupt(MAC-PHY 中断)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The MAC-PHY interrupt is asserted when the following conditions are met.
(当满足以下条件时，MAC-PHY 中断会被发出。)

Receive chunks available - This interrupt is asserted when the previous
data footer had no receive data chunks available and once the receive
data chunks become available for reading. On reception of the first data
header this interrupt will be deasserted.
(可用接收块 —— 当前一个数据页脚指示没有可用的接收数据块，而接收数据块一旦变得可供读取时，此中断会被发出。在收到第一个数据头后，此中断将被解除。)

Transmit chunk credits available - This interrupt is asserted when the
previous data footer indicated no transmit credits available and once the
transmit credits become available for transmitting transmit data chunks.
On reception of the first data header this interrupt will be deasserted.
(可用发送块信用 —— 当前一个数据页脚指示没有可用的发送信用，而发送信用一旦变得可用于发送数据块时，此中断会被发出。在收到第一个数据头后，此中断将被解除。)

Extended status event - This interrupt is asserted when the previous data
footer indicated no extended status and once the extended event become
available. In this case the host should read status #0 register to know
the corresponding error/event. On reception of the first data header this
interrupt will be deasserted.
(扩展状态事件 —— 当前一个数据页脚指示没有扩展状态，而扩展事件一旦变得可用时，此中断会被发出。在这种情况下，主机应读取 status #0 寄存器以了解相应的错误/事件。在收到第一个数据头后，此中断将被解除。)

Control Transaction(控制事务)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

4 bytes control header contains the below fields,
(4 字节的控制头包含以下字段，)

DNC (Bit 31) - Data-Not-Control flag. This flag specifies the type of SPI
               transaction. For control commands, this bit shall be ‘0’.
               0 - Control command
               1 - Data chunk
(DNC (位 31) —— 数据-非-控制 (Data-Not-Control) 标志。此标志指定 SPI 事务的类型。对于控制命令，此位应为 "0"。0 - 控制命令；1 - 数据块)

HDRB (Bit 30) - Received Header Bad. When set by the MAC-PHY, indicates
                that a header was received with a parity error. The SPI
                host should always clear this bit. The MAC-PHY ignores the
                HDRB value sent by the SPI host on MOSI.
(HDRB (位 30) —— 接收头错误。由 MAC-PHY 置位时，表示接收到了带有奇偶校验错误的头。SPI 主机应始终清除此位。MAC-PHY 会忽略 SPI 主机在 MOSI 上发送的 HDRB 值。)

WNR (Bit 29) - Write-Not-Read. This bit indicates if data is to be written
               to registers (when set) or read from registers
               (when clear).
(WNR (位 29) —— 写-非-读。此位指示数据是要写入寄存器 (置位时) 还是从寄存器读取 (清零时)。)

AID (Bit 28) - Address Increment Disable. When clear, the address will be
               automatically post-incremented by one following each
               register read or write. When set, address auto increment is
               disabled allowing successive reads and writes to occur at
               the same register address.
(AID (位 28) —— 地址递增禁用。清零时，每次寄存器读或写之后地址将自动加一。置位时，地址自动递增被禁用，使连续的读写可以在同一寄存器地址上进行。)

MMS (Bit 27..24) - Memory Map Selector. This field selects the specific
                   register memory map to access.
(MMS (位 27..24) —— 内存映射选择器。此字段选择要访问的特定寄存器内存映射。)

ADDR (Bit 23..8) - Address. Address of the first register within the
                   selected memory map to access.
(ADDR (位 23..8) —— 地址。所选内存映射中要访问的第一个寄存器的地址。)

LEN (Bit 7..1) - Length. Specifies the number of registers to read/write.
                 This field is interpreted as the number of registers
                 minus 1 allowing for up to 128 consecutive registers read
                 or written starting at the address specified in ADDR. A
                 length of zero shall read or write a single register.
(LEN (位 7..1) —— 长度。指定要读/写的寄存器数量。此字段被解释为寄存器数量减 1，从而允许从 ADDR 指定的地址开始连续读取或写入最多 128 个寄存器。长度为零时将读取或写入单个寄存器。)

P (Bit 0) - Parity. Parity bit calculated over the control command header.
            Method used is odd parity.
(P (位 0) —— 奇偶校验。对控制命令头计算的奇偶校验位。采用奇校验方法。)

Control transactions consist of one or more control commands. Control
commands are used by the SPI host to read and write registers within the
MAC-PHY. Each control commands are composed of a 4 bytes control command
header followed by register write data in case of control write command.
(控制事务由一个或多个控制命令组成。控制命令由 SPI 主机用于读取和写入 MAC-PHY 内的寄存器。每个控制命令由一个 4 字节的控制命令头组成，在控制写命令的情况下后跟寄存器写数据。)

The MAC-PHY ignores the final 4 bytes of data from the SPI host at the end
of the control write command. The control write command is also echoed
from the MAC-PHY back to the SPI host to identify which register write
failed in case of any bus errors. The echoed Control write command will
have the first 4 bytes unused value to be ignored by the SPI host
followed by 4 bytes echoed control header followed by echoed register
write data. Control write commands can write either a single register or
multiple consecutive registers. When multiple consecutive registers are
written, the address is automatically post-incremented by the MAC-PHY.
Writing to any unimplemented or undefined registers shall be ignored and
yield no effect.
(MAC-PHY 会忽略控制写命令末尾来自 SPI 主机的最后 4 字节数据。控制写命令还会从 MAC-PHY 回显给 SPI 主机，以便在出现任何总线错误时识别哪个寄存器写失败。回显的控制写命令的前 4 字节为无用值 (由 SPI 主机忽略)，随后是 4 字节回显的控制头，再后跟回显的寄存器写数据。控制写命令可以写单个寄存器或多个连续寄存器。写入多个连续寄存器时，地址由 MAC-PHY 自动递增。写入任何未实现或未定义的寄存器应被忽略且不产生任何效果。)

The MAC-PHY ignores all data from the SPI host following the control
header for the remainder of the control read command. The control read
command is also echoed from the MAC-PHY back to the SPI host to identify
which register read is failed in case of any bus errors. The echoed
Control read command will have the first 4 bytes of unused value to be
ignored by the SPI host followed by 4 bytes echoed control header followed
by register read data. Control read commands can read either a single
register or multiple consecutive registers. When multiple consecutive
registers are read, the address is automatically post-incremented by the
MAC-PHY. Reading any unimplemented or undefined registers shall return
zero.
(MAC-PHY 会忽略控制读命令其余部分中控制头之后来自 SPI 主机的所有数据。控制读命令还会从 MAC-PHY 回显给 SPI 主机，以便在出现任何总线错误时识别哪个寄存器读失败。回显的控制读命令的前 4 字节为无用值 (由 SPI 主机忽略)，随后是 4 字节回显的控制头，再后跟寄存器读数据。控制读命令可以读单个寄存器或多个连续寄存器。读取多个连续寄存器时，地址由 MAC-PHY 自动递增。读取任何未实现或未定义的寄存器应返回零。)

Device drivers API(设备驱动程序 API)
====================================

The include/linux/oa_tc6.h defines the following functions:
(include/linux/oa_tc6.h 定义了以下函数：)

.. c:function:: struct oa_tc6 *oa_tc6_init(struct spi_device *spi, \
                                           struct net_device *netdev)

Initialize OA TC6 lib.
(初始化 OA TC6 库。)

.. c:function:: void oa_tc6_exit(struct oa_tc6 *tc6)

Free allocated OA TC6 lib.
(释放已分配的 OA TC6 库。)

.. c:function:: int oa_tc6_write_register(struct oa_tc6 *tc6, u32 address, \
                                          u32 value)

Write a single register in the MAC-PHY.
(在 MAC-PHY 中写入单个寄存器。)

.. c:function:: int oa_tc6_write_registers(struct oa_tc6 *tc6, u32 address, \
                                           u32 value[], u8 length)

Writing multiple consecutive registers starting from @address in the MAC-PHY.
Maximum of 128 consecutive registers can be written starting at @address.
(在 MAC-PHY 中从 @address 开始写入多个连续寄存器。从 @address 开始最多可以写入 128 个连续寄存器。)

.. c:function:: int oa_tc6_read_register(struct oa_tc6 *tc6, u32 address, \
                                         u32 *value)

Read a single register in the MAC-PHY.
(在 MAC-PHY 中读取单个寄存器。)

.. c:function:: int oa_tc6_read_registers(struct oa_tc6 *tc6, u32 address, \
                                          u32 value[], u8 length)

Reading multiple consecutive registers starting from @address in the MAC-PHY.
Maximum of 128 consecutive registers can be read starting at @address.
(在 MAC-PHY 中从 @address 开始读取多个连续寄存器。从 @address 开始最多可以读取 128 个连续寄存器。)

.. c:function:: netdev_tx_t oa_tc6_start_xmit(struct oa_tc6 *tc6, \
                                              struct sk_buff *skb);

The transmit Ethernet frame in the skb is or going to be transmitted through
the MAC-PHY.
(skb 中的待发送以太网帧正在或将要通过 MAC-PHY 发送。)

.. c:function:: int oa_tc6_zero_align_receive_frame_enable(struct oa_tc6 *tc6);

Zero align receive frame feature can be enabled to align all receive ethernet
frames data to start at the beginning of any receive data chunk payload with a
start word offset (SWO) of zero.
(可以启用零对齐接收帧特性，使所有接收以太网帧数据对齐为从任意接收数据块有效载荷的起始处开始，其起始字偏移 (SWO) 为零。)
