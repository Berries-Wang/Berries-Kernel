.. SPDX-License-Identifier: GPL-2.0+

=====================================================
Flexcan CAN Controller driver(Flexcan CAN 控制器驱动)
=====================================================

Authors: Marc Kleine-Budde <mkl@pengutronix.de>,
Dario Binacchi <dario.binacchi@amarulasolutions.com>
(作者: Marc Kleine-Budde <mkl@pengutronix.de>、
Dario Binacchi <dario.binacchi@amarulasolutions.com>)

On/off RTR frames reception(RTR 帧接收的开启/关闭)
==================================================

For most flexcan IP cores the driver supports 2 RX modes:
(对于大多数 flexcan IP 核心，该驱动支持 2 种 RX 模式：)

- FIFO
  (FIFO 模式)
- mailbox
  (邮箱模式)

The older flexcan cores (integrated into the i.MX25, i.MX28, i.MX35
and i.MX53 SOCs) only receive RTR frames if the controller is
configured for RX-FIFO mode.
(较旧的 flexcan 核心(集成在 i.MX25、i.MX28、i.MX35 和 i.MX53 SOC 中)只有在控制器被配置为 RX-FIFO 模式时才能接收 RTR 帧。)

The RX FIFO mode uses a hardware FIFO with a depth of 6 CAN frames,
while the mailbox mode uses a software FIFO with a depth of up to 62
CAN frames. With the help of the bigger buffer, the mailbox mode
performs better under high system load situations.
(RX FIFO 模式使用深度为 6 个 CAN 帧的硬件 FIFO,而邮箱模式使用深度最多可达 62 个 CAN 帧的软件 FIFO。借助更大的缓冲区，邮箱模式在高系统负载情况下表现更好。)

As reception of RTR frames is part of the CAN standard, all flexcan
cores come up in a mode where RTR reception is possible.
(由于 RTR 帧的接收是 CAN 标准的一部分，所有 flexcan 核心默认处于可以接收 RTR 的模式。)

With the "rx-rtr" private flag the ability to receive RTR frames can
be waived at the expense of losing the ability to receive RTR
messages. This trade off is beneficial in certain use cases.
(通过 "rx-rtr" 私有标志，可以放弃接收 RTR 帧的能力，代价是失去接收 RTR 消息的能力。这种取舍在某些使用场景中是有益的。)

"rx-rtr" on
("rx-rtr" 开启)

  Receive RTR frames. (default)
  (接收 RTR 帧。(默认))

  The CAN controller can and will receive RTR frames.
  (CAN 控制器能够且将会接收 RTR 帧。)

  On some IP cores the controller cannot receive RTR frames in the
  more performant "RX mailbox" mode and will use "RX FIFO" mode
  instead.
  (在某些 IP 核心上，控制器在性能更好的 "RX mailbox" 模式下无法接收 RTR 帧，将改用 "RX FIFO" 模式。)

"rx-rtr" off
("rx-rtr" 关闭)

  Waive ability to receive RTR frames. (not supported on all IP cores)
  (放弃接收 RTR 帧的能力。(并非所有 IP 核心都支持))

  This mode activates the "RX mailbox mode" for better performance, on
  some IP cores RTR frames cannot be received anymore.
  (该模式为获得更好的性能而激活 "RX mailbox mode",在某些 IP 核心上将无法再接收 RTR 帧。)

The setting can only be changed if the interface is down::
(只有在接口处于 down 状态时才能更改该设置：)

    ip link set dev can0 down
    ethtool --set-priv-flags can0 rx-rtr {off|on}
    ip link set dev can0 up
