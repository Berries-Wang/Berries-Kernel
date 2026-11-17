=================
The UCAN Protocol
=================

(UCAN 协议)

UCAN is the protocol used by the microcontroller-based USB-CAN
adapter that is integrated on System-on-Modules from Theobroma Systems
and that is also available as a standalone USB stick.
(UCAN 是基于微控制器的 USB-CAN 适配器所使用的协议。该适配器集成于
Theobroma Systems 的系统级模块（System-on-Modules）之上，同时也以独立的
USB 棒状设备形态单独提供。)

The UCAN protocol has been designed to be hardware-independent.
It is modeled closely after how Linux represents CAN devices
internally. All multi-byte integers are encoded as Little Endian.
(UCAN 协议的设计做到了与硬件无关。它的建模方式紧贴 Linux 内部表示 CAN
设备的方式。所有多字节整数均采用小端序（Little Endian）编码。)

All structures mentioned in this document are defined in
``drivers/net/can/usb/ucan.c``.
(本文档中提到的所有结构体均定义在 ``drivers/net/can/usb/ucan.c`` 中。)

USB Endpoints
(USB 端点)
=============

UCAN devices use three USB endpoints:
(UCAN 设备使用三个 USB 端点：)

CONTROL endpoint
(CONTROL 端点)

  The driver sends device management commands on this endpoint
  (驱动程序通过该端点发送设备管理命令。)

IN endpoint
(IN 端点)

  The device sends CAN data frames and CAN error frames
  (设备通过该端点发送 CAN 数据帧和 CAN 错误帧。)

OUT endpoint
(OUT 端点)

  The driver sends CAN data frames on the out endpoint
  (驱动程序通过该 out 端点发送 CAN 数据帧。)


CONTROL Messages
(CONTROL 消息)
================

UCAN devices are configured using vendor requests on the control pipe.
(UCAN 设备通过控制管道上的厂商请求（vendor requests）进行配置。)

To support multiple CAN interfaces in a single USB device all
configuration commands target the corresponding interface in the USB
descriptor.
(为了在单个 USB 设备中支持多个 CAN 接口，所有配置命令都以 USB 描述符中
对应的接口为目标。)

The driver uses ``ucan_ctrl_command_in/out`` and
``ucan_device_request_in`` to deliver commands to the device.
(驱动程序使用 ``ucan_ctrl_command_in/out`` 和 ``ucan_device_request_in``
向设备传递命令。)

Setup Packet
(建立包 Setup Packet)
------------

=================  =====================================================
``bmRequestType``  Direction | Vendor | (Interface or Device) (方向 | 厂商 | 接口或设备)
``bRequest``       Command Number (命令编号)
``wValue``         Subcommand Number (16 Bit) or 0 if not used (子命令编号（16 位），未使用时为 0)
``wIndex``         USB Interface Index (0 for device commands) (USB 接口索引，设备命令时为 0)
``wLength``        * Host to Device - Number of bytes to transmit
                   * Device to Host - Maximum Number of bytes to
                     receive. If the device send less. Common ZLP
                     semantics are used. (主机到设备：要发送的字节数；设备到主机：可接收的最大字节数。如果设备发送的数据较少，则遵循常见的 ZLP 语义。)
=================  =====================================================

Error Handling
(错误处理)
--------------

The device indicates failed control commands by stalling the
pipe.
(设备通过停顿（stall）控制管道来指示控制命令失败。)

Device Commands
(设备命令)
---------------

UCAN_DEVICE_GET_FW_STRING
(UCAN_DEVICE_GET_FW_STRING，获取固件字符串)
~~~~~~~~~~~~~~~~~~~~~~~~~

*Dev2Host; optional*
(Dev2Host；可选)

Request the device firmware string.
(请求设备固件字符串。)


Interface Commands
(接口命令)
------------------

UCAN_COMMAND_START
(UCAN_COMMAND_START，启动)
~~~~~~~~~~~~~~~~~~

*Host2Dev; mandatory*
(Host2Dev；强制)

Bring the CAN interface up.
(使 CAN 接口进入工作状态。)

Payload Format
(载荷格式)

  ``ucan_ctl_payload_t.cmd_start``

====  ============================
mode  or mask of ``UCAN_MODE_*`` (UCAN_MODE_* 值的按位或组合)
====  ============================

UCAN_COMMAND_STOP
(UCAN_COMMAND_STOP，停止)
~~~~~~~~~~~~~~~~~~

*Host2Dev; mandatory*
(Host2Dev；强制)

Stop the CAN interface
(停止 CAN 接口。)

Payload Format
(载荷格式)

  *empty*
  (空。)

UCAN_COMMAND_RESET
(UCAN_COMMAND_RESET，复位)
~~~~~~~~~~~~~~~~~~

*Host2Dev; mandatory*
(Host2Dev；强制)

Reset the CAN controller (including error counters)
(复位 CAN 控制器（包括错误计数器）。)

Payload Format
(载荷格式)

  *empty*
  (空。)

UCAN_COMMAND_GET
(UCAN_COMMAND_GET，获取)
~~~~~~~~~~~~~~~~

*Host2Dev; mandatory*
(Host2Dev；强制)

Get Information from the Device
(从设备获取信息。)

Subcommands
(子命令)
^^^^^^^^^^^

UCAN_COMMAND_GET_INFO
(UCAN_COMMAND_GET_INFO，获取信息)

  Request the device information structure ``ucan_ctl_payload_t.device_info``.
  (请求设备信息结构 ``ucan_ctl_payload_t.device_info``。)

  See the ``device_info`` field for details, and
  ``uapi/linux/can/netlink.h`` for an explanation of the
  ``can_bittiming fields``.
  (详见 ``device_info`` 字段，以及 ``uapi/linux/can/netlink.h`` 中对
  ``can_bittiming fields`` 各字段的说明。)

  Payload Format
  (载荷格式)

    ``ucan_ctl_payload_t.device_info``

UCAN_COMMAND_GET_PROTOCOL_VERSION
(UCAN_COMMAND_GET_PROTOCOL_VERSION，获取协议版本)

  Request the device protocol version
  ``ucan_ctl_payload_t.protocol_version``. The current protocol version is 3.
  (请求设备的协议版本 ``ucan_ctl_payload_t.protocol_version``。
  当前协议版本为 3。)

  Payload Format
  (载荷格式)

    ``ucan_ctl_payload_t.protocol_version``

.. note:: Devices that do not implement this command use the old
          protocol version 1
          (注意：未实现该命令的设备使用旧的协议版本 1。)

UCAN_COMMAND_SET_BITTIMING
(UCAN_COMMAND_SET_BITTIMING，设置位时序)
~~~~~~~~~~~~~~~~~~~~~~~~~~

*Host2Dev; mandatory*
(Host2Dev；强制)

Setup bittiming by sending the structure
``ucan_ctl_payload_t.cmd_set_bittiming`` (see ``struct bittiming`` for
details)
(通过发送结构体 ``ucan_ctl_payload_t.cmd_set_bittiming`` 来设置位时序
（详见 ``struct bittiming``）。)

Payload Format
(载荷格式)

  ``ucan_ctl_payload_t.cmd_set_bittiming``.

UCAN_SLEEP/WAKE
(UCAN_SLEEP/WAKE，睡眠/唤醒)
~~~~~~~~~~~~~~~

*Host2Dev; optional*
(Host2Dev；可选)

Configure sleep and wake modes. Not yet supported by the driver.
(配置睡眠与唤醒模式。驱动程序尚未支持该命令。)

UCAN_FILTER
(UCAN_FILTER，过滤器)
~~~~~~~~~~~

*Host2Dev; optional*
(Host2Dev；可选)

Setup hardware CAN filters. Not yet supported by the driver.
(设置硬件 CAN 过滤器。驱动程序尚未支持该命令。)

Allowed interface commands
(允许的接口命令)
--------------------------

==================  ===================  ==================
Legal Device State  Command (命令)       New Device State (新状态)
==================  ===================  ==================
stopped (停止)      SET_BITTIMING        stopped (停止)
stopped (停止)      START (启动)         started (启动)
started (启动)      STOP or RESET        stopped (停止)
stopped (停止)      STOP or RESET        stopped (停止)
started (启动)      RESTART (重新启动)   started (启动)
any (任意)          GET (获取)           *no change* (无变化)
==================  ===================  ==================

IN Message Format
(IN 消息格式)
=================

A data packet on the USB IN endpoint contains one or more
``ucan_message_in`` values. If multiple messages are batched in a USB
data packet, the ``len`` field can be used to jump to the next
``ucan_message_in`` value (take care to sanity-check the ``len`` value
against the actual data size).
(USB IN 端点上的一个数据包含有一个或多个 ``ucan_message_in`` 值。如果多个
消息被打包进同一个 USB 数据包，可以使用 ``len`` 字段跳转到下一个
``ucan_message_in`` 值（务必根据实际数据大小对 ``len`` 值做完整性检查）。)

.. _can_ucan_in_message_len:

``len`` field
(``len`` 字段)
-------------

Each ``ucan_message_in`` must be aligned to a 4-byte boundary (relative
to the start of the start of the data buffer). That means that there
may be padding bytes between multiple ``ucan_message_in`` values:
(每个 ``ucan_message_in`` 都必须按 4 字节边界对齐（相对于数据缓冲区的
起始位置）。这意味着多个 ``ucan_message_in`` 值之间可能存在填充字节：)

.. code::

    +----------------------------+ < 0
    |                            |
    |   struct ucan_message_in   |
    |                            |
    +----------------------------+ < len
              [padding]
    +----------------------------+ < round_up(len, 4)
    |                            |
    |   struct ucan_message_in   |
    |                            |
    +----------------------------+
                [...]

``type`` field
(``type`` 字段)
--------------

The ``type`` field specifies the type of the message.
(``type`` 字段指定消息的类型。)

UCAN_IN_RX
(UCAN_IN_RX，接收)
~~~~~~~~~~

``subtype``
(子类型)

  zero
  (值为 0。)

Data received from the CAN bus (ID + payload).
(从 CAN 总线接收到的数据（ID + 有效载荷）。)

UCAN_IN_TX_COMPLETE
(UCAN_IN_TX_COMPLETE，发送完成)
~~~~~~~~~~~~~~~~~~~

``subtype``
(子类型)

  zero
  (值为 0。)

The CAN device has sent a message to the CAN bus. It answers with a
list of tuples <echo-ids, flags>.
(CAN 设备已向 CAN 总线发送了一条消息。它会以一个 <echo-ids, flags>
元组列表作为应答。)

The echo-id identifies the frame from (echos the id from a previous
UCAN_OUT_TX message). The flag indicates the result of the
transmission. Whereas a set Bit 0 indicates success. All other bits
are reserved and set to zero.
(echo-id 用于标识对应的帧（即回显之前某条 UCAN_OUT_TX 消息中的 id）。
标志位指示发送结果，其中第 0 位被置位表示发送成功。其余所有位均保留
并置为 0。)

Flow Control
(流控)
------------

When receiving CAN messages there is no flow control on the USB
buffer. The driver has to handle inbound message quickly enough to
avoid drops. I case the device buffer overflow the condition is
reported by sending corresponding error frames (see
:ref:`can_ucan_error_handling`)
(接收 CAN 消息时，USB 缓冲区上没有任何流控机制。驱动程序必须足够快地
处理入站消息，以避免丢包。当设备缓冲区溢出时，会通过发送相应的错误帧
来报告该情况（参见 :ref:`can_ucan_error_handling`）。)


OUT Message Format
(OUT 消息格式)
==================

A data packet on the USB OUT endpoint contains one or more ``struct
ucan_message_out`` values. If multiple messages are batched into one
data packet, the device uses the ``len`` field to jump to the next
ucan_message_out value. Each ucan_message_out must be aligned to 4
bytes (relative to the start of the data buffer). The mechanism is
same as described in :ref:`can_ucan_in_message_len`.
(USB OUT 端点上的一个数据包含有一个或多个 ``struct ucan_message_out``
值。如果多个消息被打包进同一个数据包，设备会使用 ``len`` 字段跳转到
下一个 ucan_message_out 值。每个 ucan_message_out 必须按 4 字节对齐
（相对于数据缓冲区的起始位置）。其机制与 :ref:`can_ucan_in_message_len`
中所述的相同。)

.. code::

    +----------------------------+ < 0
    |                            |
    |   struct ucan_message_out  |
    |                            |
    +----------------------------+ < len
              [padding]
    +----------------------------+ < round_up(len, 4)
    |                            |
    |   struct ucan_message_out  |
    |                            |
    +----------------------------+
                [...]

``type`` field
(``type`` 字段)
--------------

In protocol version 3 only ``UCAN_OUT_TX`` is defined, others are used
only by legacy devices (protocol version 1).
(在协议版本 3 中仅定义了 ``UCAN_OUT_TX``，其余类型仅由旧版设备
（协议版本 1）使用。)

UCAN_OUT_TX
(UCAN_OUT_TX，发送)
~~~~~~~~~~~

``subtype``
(子类型)

  echo id to be replied within a CAN_IN_TX_COMPLETE message
  (在 CAN_IN_TX_COMPLETE 消息中应答的 echo id)

Transmit a CAN frame. (parameters: ``id``, ``data``)
(发送一个 CAN 帧。参数：``id``、``data``。)

Flow Control
(流控)
------------

When the device outbound buffers are full it starts sending *NAKs* on
the *OUT* pipe until more buffers are available. The driver stops the
queue when a certain threshold of out packets are incomplete.
(当设备的出站缓冲区已满时，它会在 *OUT* 管道上开始发送 NAKs，直到有更多
缓冲区可用为止。当未完成的外发数据包达到一定阈值时，驱动程序会停止
队列。)

.. _can_ucan_error_handling:

CAN Error Handling
(CAN 错误处理)
==================

If error reporting is turned on the device encodes errors into CAN
error frames (see ``uapi/linux/can/error.h``) and sends it using the
IN endpoint. The driver updates its error statistics and forwards
it.
(如果开启了错误上报，设备会把错误编码进 CAN 错误帧
（参见 ``uapi/linux/can/error.h``），并通过 IN 端点发送出去。驱动程序会
更新自身的错误统计信息并将其向上转发。)

Although UCAN devices can suppress error frames completely, in Linux
the driver is always interested. Hence, the device is always started with
the ``UCAN_MODE_BERR_REPORT`` set. Filtering those messages for the
user space is done by the driver.
(尽管 UCAN 设备可以完全抑制错误帧，但在 Linux 中驱动程序始终需要这些
信息。因此，设备总是在设置了 ``UCAN_MODE_BERR_REPORT`` 的情况下启动。
针对用户空间对这些消息的过滤由驱动程序完成。)

Bus OFF
(总线关闭)
-------

- The device does not recover from bus of automatically.
  (设备不会自动从总线关闭状态中恢复。)
- Bus OFF is indicated by an error frame (see ``uapi/linux/can/error.h``)
  (总线关闭状态由错误帧指示（参见 ``uapi/linux/can/error.h``）。)
- Bus OFF recovery is started by ``UCAN_COMMAND_RESTART``
  (总线关闭恢复由 ``UCAN_COMMAND_RESTART`` 命令启动。)
- Once Bus OFF recover is completed the device sends an error frame
  indicating that it is on ERROR-ACTIVE state.
  (一旦总线关闭恢复完成，设备会发送一个错误帧，表明其已处于
  ERROR-ACTIVE 状态。)
- During Bus OFF no frames are sent by the device.
  (在总线关闭期间，设备不会发送任何帧。)
- During Bus OFF transmission requests from the host are completed
  immediately with the success bit left unset.
  (在总线关闭期间，来自主机的发送请求会被立即完成，且成功位保持未置位
  状态。)

Example Conversation
(示例交互流程)
====================

#) Device is connected to USB
   (设备连接到 USB。)
#) Host sends command ``UCAN_COMMAND_RESET``, subcmd 0
   (主机发送命令 ``UCAN_COMMAND_RESET``，子命令为 0。)
#) Host sends command ``UCAN_COMMAND_GET``, subcmd ``UCAN_COMMAND_GET_INFO``
   (主机发送命令 ``UCAN_COMMAND_GET``，子命令为
   ``UCAN_COMMAND_GET_INFO``。)
#) Device sends ``UCAN_IN_DEVICE_INFO``
   (设备发送 ``UCAN_IN_DEVICE_INFO``。)
#) Host sends command ``UCAN_OUT_SET_BITTIMING``
   (主机发送命令 ``UCAN_OUT_SET_BITTIMING``。)
#) Host sends command ``UCAN_COMMAND_START``, subcmd 0, mode ``UCAN_MODE_BERR_REPORT``
   (主机发送命令 ``UCAN_COMMAND_START``，子命令为 0，模式为
   ``UCAN_MODE_BERR_REPORT``。)
