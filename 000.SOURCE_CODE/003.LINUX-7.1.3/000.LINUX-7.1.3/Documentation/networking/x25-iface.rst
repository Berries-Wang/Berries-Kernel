.. SPDX-License-Identifier: GPL-2.0

X.25 Device Driver Interface
(X.25 设备驱动接口)
============================

Version 1.1

			   Jonathan Naylor 26.12.96

This is a description of the messages to be passed between the X.25 Packet
Layer and the X.25 device driver. They are designed to allow for the easy
setting of the LAPB mode from within the Packet Layer.
(本文描述在 X.25 分组层 (Packet Layer) 与 X.25 设备驱动之间传递的消息。这些消息的设计目的是让分组层能够方便地设置 LAPB 模式。)

The X.25 device driver will be coded normally as per the Linux device driver
standards. Most X.25 device drivers will be moderately similar to the
already existing Ethernet device drivers. However unlike those drivers, the
X.25 device driver has a state associated with it, and this information
needs to be passed to and from the Packet Layer for proper operation.
(X.25 设备驱动通常按照 Linux 设备驱动标准编写。大多数 X.25 设备驱动与现有的以太网设备驱动有一定程度的相似。但与那些驱动不同的是，X.25 设备驱动带有与之关联的状态，为正常工作，该信息需要在分组层与设备驱动之间双向传递。)

All messages are held in sk_buff's just like real data to be transmitted
over the LAPB link. The first byte of the skbuff indicates the meaning of
the rest of the skbuff, if any more information does exist.
(所有消息都保存在 sk_buff 中，就像要通过 LAPB 链路发送的真实数据一样。如果还存在更多信息，skbuff 的第一个字节指示 skbuff 其余部分的含义。)


Packet Layer to Device Driver
(分组层到设备驱动)
-----------------------------

First Byte = 0x00 (X25_IFACE_DATA)
(第一字节 = 0x00 (X25_IFACE_DATA))

This indicates that the rest of the skbuff contains data to be transmitted
over the LAPB link. The LAPB link should already exist before any data is
passed down.
(这表示 skbuff 的其余部分包含要通过 LAPB 链路发送的数据。在传递任何数据之前，LAPB 链路应当已经建立。)

First Byte = 0x01 (X25_IFACE_CONNECT)
(第一字节 = 0x01 (X25_IFACE_CONNECT))

Establish the LAPB link. If the link is already established then the connect
confirmation message should be returned as soon as possible.
(建立 LAPB 链路。如果链路已经建立，则应尽快返回连接确认 (connect confirmation) 消息。)

First Byte = 0x02 (X25_IFACE_DISCONNECT)
(第一字节 = 0x02 (X25_IFACE_DISCONNECT))

Terminate the LAPB link. If it is already disconnected then the disconnect
confirmation message should be returned as soon as possible.
(终止 LAPB 链路。如果链路已经断开，则应尽快返回断开确认 (disconnect confirmation) 消息。)

First Byte = 0x03 (X25_IFACE_PARAMS)
(第一字节 = 0x03 (X25_IFACE_PARAMS))

LAPB parameters. To be defined.
(LAPB 参数。待定义。)


Device Driver to Packet Layer
(设备驱动到分组层)
-----------------------------

First Byte = 0x00 (X25_IFACE_DATA)
(第一字节 = 0x00 (X25_IFACE_DATA))

This indicates that the rest of the skbuff contains data that has been
received over the LAPB link.
(这表示 skbuff 的其余部分包含通过 LAPB 链路接收到的数据。)

First Byte = 0x01 (X25_IFACE_CONNECT)
(第一字节 = 0x01 (X25_IFACE_CONNECT))

LAPB link has been established. The same message is used for both a LAPB
link connect_confirmation and a connect_indication.
(LAPB 链路已建立。同一消息既用于 LAPB 链路的 connect_confirmation，也用于 connect_indication。)

First Byte = 0x02 (X25_IFACE_DISCONNECT)
(第一字节 = 0x02 (X25_IFACE_DISCONNECT))

LAPB link has been terminated. This same message is used for both a LAPB
link disconnect_confirmation and a disconnect_indication.
(LAPB 链路已终止。同一消息既用于 LAPB 链路的 disconnect_confirmation，也用于 disconnect_indication。)

First Byte = 0x03 (X25_IFACE_PARAMS)
(第一字节 = 0x03 (X25_IFACE_PARAMS))

LAPB parameters. To be defined.
(LAPB 参数。待定义。)


Requirements for the device driver
(对设备驱动的要求)
----------------------------------

Packets should not be reordered or dropped when delivering between the
Packet Layer and the device driver.
(在分组层与设备驱动之间传递数据包时，不应重排或丢弃数据包。)

To avoid packets from being reordered or dropped when delivering from
the device driver to the Packet Layer, the device driver should not
call "netif_rx" to deliver the received packets. Instead, it should
call "netif_receive_skb_core" from softirq context to deliver them.
(为避免从设备驱动向分组层传递时数据包被重排或丢弃，设备驱动不应调用 "netif_rx" 来传递收到的数据包，而应在 softirq 上下文中调用 "netif_receive_skb_core" 来传递它们。)
