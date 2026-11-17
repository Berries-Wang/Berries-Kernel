.. SPDX-License-Identifier: GPL-2.0

============
Rmnet Driver
============

(Rmnet 驱动)

1. Introduction
(1. 简介)
===============

rmnet driver is used for supporting the Multiplexing and aggregation
Protocol (MAP). This protocol is used by all recent chipsets using Qualcomm
Technologies, Inc. modems.
(rmnet 驱动用于支持多路复用与聚合协议 (MAP)。所有近期使用 Qualcomm Technologies, Inc. 调制解调器的芯片组都
使用该协议。)

This driver can be used to register onto any physical network device in
IP mode. Physical transports include USB, HSIC, PCIe and IP accelerator.
(该驱动可用于在任意 IP 模式的物理网络设备上注册。物理传输层包括 USB、HSIC、PCIe 和 IP 加速器。)

Multiplexing allows for creation of logical netdevices (rmnet devices) to
handle multiple private data networks (PDN) like a default internet, tethering,
multimedia messaging service (MMS) or IP media subsystem (IMS). Hardware sends
packets with MAP headers to rmnet. Based on the multiplexer id, rmnet
routes to the appropriate PDN after removing the MAP header.
(多路复用允许创建逻辑网络设备（rmnet 设备）来处理多个私有数据网络 (PDN)，例如默认互联网、网络共享、多媒体消
息服务 (MMS) 或 IP 多媒体子系统 (IMS)。硬件将带有 MAP 头的数据包发送给 rmnet。rmnet 根据多路复用器 ID，
在移除 MAP 头之后路由到相应的 PDN。)

Aggregation is required to achieve high data rates. This involves hardware
sending aggregated bunch of MAP frames. rmnet driver will de-aggregate
these MAP frames and send them to appropriate PDN's.
(要实现高数据速率，需要聚合。这由硬件发送聚合的一批 MAP 帧来完成。rmnet 驱动会解聚这些 MAP 帧，并将其发送到
相应的 PDN。)

2. Packet format
(2. 数据包格式)
================

a. MAP packet v1 (data / control)
(a. MAP 数据包 v1（数据/控制）)
---------------------------------

MAP header fields are in big endian format.
(MAP 头字段采用大端格式。)

Packet format::
(数据包格式：)

  Bit             0             1           2-7      8-15           16-31
  Function   Command / Data   Reserved     Pad   Multiplexer ID    Payload length

  Bit            32-x
  Function      Raw bytes

Command (1)/ Data (0) bit value is to indicate if the packet is a MAP command
or data packet. Command packet is used for transport level flow control. Data
packets are standard IP packets.
(Command (1)/Data (0) 位用于指示该数据包是 MAP 命令包还是数据包。命令包用于传输层流量控制。数据包是标准的
IP 数据包。)

Reserved bits must be zero when sent and ignored when received.
(保留位在发送时必须置零，在接收时被忽略。)

Padding is the number of bytes to be appended to the payload to
ensure 4 byte alignment.
(填充 (Padding) 是为使有效载荷 4 字节对齐而需要附加到其后的字节数。)

Multiplexer ID is to indicate the PDN on which data has to be sent.
(多路复用器 ID 用于指示数据要发送到哪个 PDN。)

Payload length includes the padding length but does not include MAP header
length.
(有效载荷长度包含填充长度，但不包含 MAP 头长度。)

b. Map packet v4 (data / control)
(b. MAP 数据包 v4（数据/控制）)
---------------------------------

MAP header fields are in big endian format.
(MAP 头字段采用大端格式。)

Packet format::
(数据包格式：)

  Bit             0             1           2-7      8-15           16-31
  Function   Command / Data   Reserved     Pad   Multiplexer ID    Payload length

  Bit            32-(x-33)      (x-32)-x
  Function      Raw bytes      Checksum offload header

Command (1)/ Data (0) bit value is to indicate if the packet is a MAP command
or data packet. Command packet is used for transport level flow control. Data
packets are standard IP packets.
(Command (1)/Data (0) 位用于指示该数据包是 MAP 命令包还是数据包。命令包用于传输层流量控制。数据包是标准的
IP 数据包。)

Reserved bits must be zero when sent and ignored when received.
(保留位在发送时必须置零，在接收时被忽略。)

Padding is the number of bytes to be appended to the payload to
ensure 4 byte alignment.
(填充 (Padding) 是为使有效载荷 4 字节对齐而需要附加到其后的字节数。)

Multiplexer ID is to indicate the PDN on which data has to be sent.
(多路复用器 ID 用于指示数据要发送到哪个 PDN。)

Payload length includes the padding length but does not include MAP header
length.
(有效载荷长度包含填充长度，但不包含 MAP 头长度。)

Checksum offload header, has the information about the checksum processing done
by the hardware.Checksum offload header fields are in big endian format.
(校验和卸载头包含硬件所做校验和处理的相关信息。校验和卸载头字段采用大端格式。)

Packet format::
(数据包格式：)

  Bit             0-14        15              16-31
  Function      Reserved   Valid     Checksum start offset

  Bit                31-47                    48-64
  Function      Checksum length           Checksum value

Reserved bits must be zero when sent and ignored when received.
(保留位在发送时必须置零，在接收时被忽略。)

Valid bit indicates whether the partial checksum is calculated and is valid.
Set to 1, if its is valid. Set to 0 otherwise.
(Valid 位指示部分校验和是否已计算且有效。有效则置 1，否则置 0。)

Padding is the number of bytes to be appended to the payload to
ensure 4 byte alignment.
(填充 (Padding) 是为使有效载荷 4 字节对齐而需要附加到其后的字节数。)

Checksum start offset, Indicates the offset in bytes from the beginning of the
IP header, from which modem computed checksum.
(校验和起始偏移 (Checksum start offset)，指示从 IP 头开始的字节偏移量，调制解调器从该位置开始计算校验和。)

Checksum length is the Length in bytes starting from CKSUM_START_OFFSET,
over which checksum is computed.
(校验和长度是从 CKSUM_START_OFFSET 开始、计算校验和所覆盖的字节长度。)

Checksum value, indicates the checksum computed.
(校验和值 (Checksum value)，指示计算得到的校验和。)

c. MAP packet v5 (data / control)
(c. MAP 数据包 v5（数据/控制）)
---------------------------------

MAP header fields are in big endian format.
(MAP 头字段采用大端格式。)

Packet format::
(数据包格式：)

  Bit             0             1         2-7      8-15           16-31
  Function   Command / Data  Next header  Pad   Multiplexer ID   Payload length

  Bit            32-x
  Function      Raw bytes

Command (1)/ Data (0) bit value is to indicate if the packet is a MAP command
or data packet. Command packet is used for transport level flow control. Data
packets are standard IP packets.
(Command (1)/Data (0) 位用于指示该数据包是 MAP 命令包还是数据包。命令包用于传输层流量控制。数据包是标准的
IP 数据包。)

Next header is used to indicate the presence of another header, currently is
limited to checksum header.
(下一头部 (Next header) 用于指示存在另一个头，目前仅限于校验和头。)

Padding is the number of bytes to be appended to the payload to
ensure 4 byte alignment.
(填充 (Padding) 是为使有效载荷 4 字节对齐而需要附加到其后的字节数。)

Multiplexer ID is to indicate the PDN on which data has to be sent.
(多路复用器 ID 用于指示数据要发送到哪个 PDN。)

Payload length includes the padding length but does not include MAP header
length.
(有效载荷长度包含填充长度，但不包含 MAP 头长度。)

d. Checksum offload header v5
(d. 校验和卸载头 v5)
-----------------------------

Checksum offload header fields are in big endian format.
(校验和卸载头字段采用大端格式。)

Packet format::
(数据包格式：)

  Bit            0 - 6          7               8-15              16-31
  Function     Header Type    Next Header     Checksum Valid    Reserved

Header Type is to indicate the type of header, this usually is set to CHECKSUM
(Header Type 用于指示头的类型，通常设置为 CHECKSUM)

Header types
(头类型)

= ===============
0 Reserved
  (保留)
1 Reserved
  (保留)
2 checksum header
  (校验和头)
= ===============

Checksum Valid is to indicate whether the header checksum is valid. Value of 1
implies that checksum is calculated on this packet and is valid, value of 0
indicates that the calculated packet checksum is invalid.
(Checksum Valid 用于指示头校验和是否有效。值为 1 表示已对该数据包计算校验和且校验和有效，值为 0 表示所计算的
数据包校验和无效。)

Reserved bits must be zero when sent and ignored when received.
(保留位在发送时必须置零，在接收时被忽略。)

e. MAP packet v1/v5 (command specific)
(e. MAP 数据包 v1/v5（命令专用）)
--------------------------------------

Packet format::
(数据包格式：)

    Bit             0             1         2-7      8 - 15           16 - 31
    Function   Command         Reserved     Pad   Multiplexer ID    Payload length
    Bit          32 - 39        40 - 45    46 - 47       48 - 63
    Function   Command name    Reserved   Command Type   Reserved
    Bit          64 - 95
    Function   Transaction ID
    Bit          96 - 127
    Function   Command data

Command 1 indicates disabling flow while 2 is enabling flow
(命令 1 表示禁用流，而命令 2 表示启用流)

Command types
(命令类型)

= ==========================================
0 for MAP command request
  (表示 MAP 命令请求)
1 is to acknowledge the receipt of a command
  (用于确认已收到命令)
2 is for unsupported commands
  (用于不支持的命令)
3 is for error during processing of commands
  (用于命令处理过程中出错)
= ==========================================

f. Aggregation
(f. 聚合)
--------------

Aggregation is multiple MAP packets (can be data or command) delivered to
rmnet in a single linear skb. rmnet will process the individual
packets and either ACK the MAP command or deliver the IP packet to the
network stack as needed
(聚合是指多个 MAP 数据包（可以是数据包或命令包）在单个线性 skb 中投递给 rmnet。rmnet 会逐个处理这些数据包，
根据需要对 MAP 命令进行 ACK，或将 IP 数据包投递给网络协议栈。)

Packet format::
(数据包格式：)

  MAP header|IP Packet|Optional padding|MAP header|IP Packet|Optional padding....

  MAP header|IP Packet|Optional padding|MAP header|Command Packet|Optional pad...

3. Userspace configuration
(3. 用户空间配置)
==========================

rmnet userspace configuration is done through netlink using iproute2
https://git.kernel.org/pub/scm/network/iproute2/iproute2.git/
(rmnet 的用户空间配置通过 netlink 使用 iproute2 完成：
https://git.kernel.org/pub/scm/network/iproute2/iproute2.git/)

The driver uses rtnl_link_ops for communication.
(驱动使用 rtnl_link_ops 进行通信。)
