.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _CEC_TRANSMIT:
.. _CEC_RECEIVE:

**********************************************************************
ioctls CEC_RECEIVE and CEC_TRANSMIT(ioctl CEC_RECEIVE 与 CEC_TRANSMIT)
**********************************************************************

Name(名称)
==========

CEC_RECEIVE, CEC_TRANSMIT - Receive or transmit a CEC message
(CEC_RECEIVE、CEC_TRANSMIT —— 接收或发送一条 CEC 消息)

Synopsis(概要)
==============

.. c:macro:: CEC_RECEIVE

``int ioctl(int fd, CEC_RECEIVE, struct cec_msg *argp)``

.. c:macro:: CEC_TRANSMIT

``int ioctl(int fd, CEC_TRANSMIT, struct cec_msg *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct cec_msg.
    (指向 struct cec_msg 的指针。)

Description(描述)
=================

To receive a CEC message the application has to fill in the
``timeout`` field of struct :c:type:`cec_msg` and pass it to
:ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>`.
If the file descriptor is in non-blocking mode and there are no received
messages pending, then it will return -1 and set errno to the ``EAGAIN``
error code. If the file descriptor is in blocking mode and ``timeout``
is non-zero and no message arrived within ``timeout`` milliseconds, then
it will return -1 and set errno to the ``ETIMEDOUT`` error code.
(要接收 CEC 消息，应用程序需填写 struct :c:type:`cec_msg` 的 ``timeout`` 字段，然后传给 :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` 。如果文件描述符处于非阻塞模式且没有待处理的已接收消息，则返回 -1 并把 errno 设为 ``EAGAIN`` 错误码。如果文件描述符处于阻塞模式且 ``timeout`` 非 0，而在 ``timeout`` 毫秒内没有消息到达，则返回 -1 并把 errno 设为 ``ETIMEDOUT`` 错误码。)

A received message can be:
(收到的消息可以是：)

1. a message received from another CEC device (the ``sequence`` field will
   be 0, ``tx_status`` will be 0 and ``rx_status`` will be non-zero).
2. the transmit result of an earlier non-blocking transmit (the ``sequence``
   field will be non-zero, ``tx_status`` will be non-zero and ``rx_status``
   will be 0).
3. the reply to an earlier non-blocking transmit (the ``sequence`` field will
   be non-zero, ``tx_status`` will be 0 and ``rx_status`` will be non-zero).
   (1. 从另一台 CEC 设备接收到的消息(``sequence`` 字段为 0，``tx_status`` 为 0，``rx_status`` 非 0)。 2. 此前一次非阻塞发送的结果(``sequence`` 字段非 0，``tx_status`` 非 0，``rx_status`` 为 0)。 3. 对此前一次非阻塞发送的应答(``sequence`` 字段非 0，``tx_status`` 为 0，``rx_status`` 非 0)。)

To send a CEC message the application has to fill in the struct
:c:type:`cec_msg` and pass it to :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>`.
The :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` is only available if
``CEC_CAP_TRANSMIT`` is set. If there is no more room in the transmit
queue, then it will return -1 and set errno to the ``EBUSY`` error code.
The transmit queue has enough room for 18 messages (about 1 second worth
of 2-byte messages). Note that the CEC kernel framework will also reply
to core messages (see :ref:`cec-core-processing`), so it is not a good
idea to fully fill up the transmit queue.
(要发送 CEC 消息，应用程序需填写 struct :c:type:`cec_msg` 并传给 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 。 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 只在设置了 ``CEC_CAP_TRANSMIT`` 时可用。如果发送队列已满，则返回 -1 并把 errno 设为 ``EBUSY`` 错误码。发送队列有容纳 18 条消息的空间(约相当于 1 秒的 2 字节消息)。注意，CEC 内核框架也会对核心消息作出应答(参见 :ref:`cec-core-processing` )，所以最好不要把发送队列完全填满。)

If the file descriptor is in non-blocking mode then the transmit will
return 0 and the result of the transmit will be available via
:ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` once the transmit has finished.
If a non-blocking transmit also specified waiting for a reply, then
the reply will arrive in a later message. The ``sequence`` field can
be used to associate both transmit results and replies with the original
transmit.
(如果文件描述符处于非阻塞模式，则发送会立即返回 0，发送结果可在发送完成后通过 :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` 获取。如果非阻塞发送还指定了等待应答，则应答会在稍后的消息中到达。``sequence`` 字段可用于把发送结果和应答与最初的发送关联起来。)

Normally calling :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` when the physical
address is invalid (due to e.g. a disconnect) will return ``ENONET``.
(通常，当物理地址无效时(例如因断开连接)，调用 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 会返回 ``ENONET``。)

However, the CEC specification allows sending messages from 'Unregistered' to
'TV' when the physical address is invalid since some TVs pull the hotplug detect
pin of the HDMI connector low when they go into standby, or when switching to
another input.
(不过，CEC 规范允许在物理地址无效时从 'Unregistered' 向 'TV' 发送消息，因为某些电视在进入待机或切换到其他输入源时，会把 HDMI 连接器的热插拔检测引脚拉低。)

When the hotplug detect pin goes low the EDID disappears, and thus the
physical address, but the cable is still connected and CEC still works.
In order to detect/wake up the device it is allowed to send poll and 'Image/Text
View On' messages from initiator 0xf ('Unregistered') to destination 0 ('TV').
(当热插拔检测引脚变低时，EDID 会消失，物理地址随之消失，但线缆仍然连接着，CEC 仍然可以工作。为了探测/唤醒设备，允许从发起方 0xf ('Unregistered') 向目标 0 ('TV') 发送 poll 与 'Image/Text View On' 消息。)

.. tabularcolumns:: |p{1.0cm}|p{3.5cm}|p{12.8cm}|

.. c:type:: cec_msg

.. cssclass:: longtable

.. flat-table:: struct cec_msg
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 16

    * - __u64
      - ``tx_ts``
      - Timestamp in ns of when the last byte of the message was transmitted.
	The timestamp has been taken from the ``CLOCK_MONOTONIC`` clock. To access
	the same clock from userspace use :c:func:`clock_gettime`. (消息最后一个字节被发送时刻的时间戳，单位为 ns。时间戳取自 ``CLOCK_MONOTONIC`` 时钟。在用户空间访问同一时钟可使用 :c:func:`clock_gettime` 。)
    * - __u64
      - ``rx_ts``
      - Timestamp in ns of when the last byte of the message was received.
	The timestamp has been taken from the ``CLOCK_MONOTONIC`` clock. To access
	the same clock from userspace use :c:func:`clock_gettime`. (消息最后一个字节被接收时刻的时间戳，单位为 ns。时间戳取自 ``CLOCK_MONOTONIC`` 时钟。在用户空间访问同一时钟可使用 :c:func:`clock_gettime` 。)
    * - __u32
      - ``len``
      - The length of the message. For :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` this is filled in
	by the application. The driver will fill this in for
	:ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>`. For :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` it will be
	filled in by the driver with the length of the reply message if ``reply`` was set. (消息的长度。对 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 而言由应用程序填写；驱动会为 :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` 填写该字段。对 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 而言，如果设置了 ``reply``，则驱动会用应答消息的长度来填写它。)
    * - __u32
      - ``timeout``
      - The timeout in milliseconds. This is the time the device will wait
	for a message to be received before timing out. If it is set to 0,
	then it will wait indefinitely when it is called by :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>`.
	If it is 0 and it is called by :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>`,
	then it will be replaced by 1000 if the ``reply`` is non-zero or
	ignored if ``reply`` is 0. (以毫秒为单位的超时时间。这是设备在超时前等待收到消息的时间。若设为 0，则由 :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` 调用时会无限等待。若为 0 且由 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 调用，则当 ``reply`` 非 0 时会被替换为 1000，``reply`` 为 0 时被忽略。)
    * - __u32
      - ``sequence``
      - A non-zero sequence number is automatically assigned by the CEC framework
	for all transmitted messages. It is used by the CEC framework when it queues
	the transmit result for a non-blocking transmit. This allows the application
	to associate the received message with the original transmit.

	In addition, if a non-blocking transmit will wait for a reply (ii.e. ``timeout``
	was not 0), then the ``sequence`` field of the reply will be set to the sequence
	value of the original transmit. This allows the application to associate the
	received message with the original transmit. (对于所有已发送的消息，CEC 框架会自动分配一个非 0 的序号。在对非阻塞发送的结果排队时，CEC 框架会使用该序号，使应用程序能把收到的消息与最初的发送关联起来。

	此外，如果非阻塞发送要等待应答(即 ``timeout`` 非 0)，则应答的 ``sequence`` 字段会被设为最初发送的序号值。这同样使应用程序能把收到的消息与最初的发送关联起来。)
    * - __u32
      - ``flags``
      - Flags. See :ref:`cec-msg-flags` for a list of available flags. (标志位。可用标志列表参见 :ref:`cec-msg-flags` 。)
    * - __u8
      - ``msg[16]``
      - The message payload. For :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` this is filled in by the
	application. The driver will fill this in for :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>`.
	For :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` it will be filled in by the driver with
	the payload of the reply message if ``timeout`` was set. (消息载荷。对 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 而言由应用程序填写；驱动会为 :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` 填写该字段。对 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 而言，如果设置了 ``timeout``，则驱动会用应答消息的载荷来填写它。)
    * - __u8
      - ``reply``
      - Wait until this message is replied. If ``reply`` is 0 and the
	``timeout`` is 0, then don't wait for a reply but return after
	transmitting the message. Ignored by :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>`.
	The case where ``reply`` is 0 (this is the opcode for the Feature Abort
	message) and ``timeout`` is non-zero is specifically allowed to make it
	possible to send a message and wait up to ``timeout`` milliseconds for a
	Feature Abort reply. In this case ``rx_status`` will either be set
	to :ref:`CEC_RX_STATUS_TIMEOUT <CEC-RX-STATUS-TIMEOUT>` or
	:ref:`CEC_RX_STATUS_FEATURE_ABORT <CEC-RX-STATUS-FEATURE-ABORT>`.

	If the transmitter message is ``CEC_MSG_INITIATE_ARC`` then the ``reply``
	values ``CEC_MSG_REPORT_ARC_INITIATED`` and ``CEC_MSG_REPORT_ARC_TERMINATED``
	are processed differently: either value will match both possible replies.
	The reason is that the ``CEC_MSG_INITIATE_ARC`` message is the only CEC
	message that has two possible replies other than Feature Abort. The
	``reply`` field will be updated with the actual reply so that it is
	synchronized with the contents of the received message. (等待此消息被应答。如果 ``reply`` 为 0 且 ``timeout`` 为 0，则不等待应答，而是在发出消息后即返回。:ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` 会忽略该字段。 ``reply`` 为 0(即 Feature Abort 消息的操作码)且 ``timeout`` 非 0 的情形是被专门允许的，以便发送一条消息并最多等待 ``timeout`` 毫秒来接收 Feature Abort 应答。此时 ``rx_status`` 要么被设为 :ref:`CEC_RX_STATUS_TIMEOUT <CEC-RX-STATUS-TIMEOUT>` ，要么被设为 :ref:`CEC_RX_STATUS_FEATURE_ABORT <CEC-RX-STATUS-FEATURE-ABORT>` 。

	如果发送的消息是 ``CEC_MSG_INITIATE_ARC``，则 ``reply`` 值 ``CEC_MSG_REPORT_ARC_INITIATED`` 与 ``CEC_MSG_REPORT_ARC_TERMINATED`` 会被区别处理：任一值都会匹配两种可能的应答。原因在于 ``CEC_MSG_INITIATE_ARC`` 是除 Feature Abort 之外唯一有两种可能应答的 CEC 消息。 ``reply`` 字段会被更新为实际的应答值，以使其与收到的消息内容同步。)
    * - __u8
      - ``rx_status``
      - The status bits of the received message. See
	:ref:`cec-rx-status` for the possible status values. (所收消息的状态位。可能的状态值参见 :ref:`cec-rx-status` 。)
    * - __u8
      - ``tx_status``
      - The status bits of the transmitted message. See
	:ref:`cec-tx-status` for the possible status values.
	When calling :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` in non-blocking mode,
	this field will be 0 if the transmit started, or non-0 if the transmit
	result is known immediately. The latter would be the case when attempting
	to transmit a Poll message to yourself. That results in a
	:ref:`CEC_TX_STATUS_NACK <CEC-TX-STATUS-NACK>` without ever actually
	transmitting the Poll message. (已发消息的状态位。可能的状态值参见 :ref:`cec-tx-status` 。以非阻塞模式调用 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 时，如果发送已启动，则该字段为 0；如果发送结果立即可知，则为非 0。后者的一种情形是试图向自己发送 Poll 消息，这会导致 :ref:`CEC_TX_STATUS_NACK <CEC-TX-STATUS-NACK>`，而 Poll 消息实际上从未被发送。)
    * - __u8
      - ``tx_arb_lost_cnt``
      - A counter of the number of transmit attempts that resulted in the
	Arbitration Lost error. This is only set if the hardware supports
	this, otherwise it is always 0. This counter is only valid if the
	:ref:`CEC_TX_STATUS_ARB_LOST <CEC-TX-STATUS-ARB-LOST>` status bit is set. (因仲裁丢失 (Arbitration Lost) 错误而失败的发送尝试次数计数。仅在硬件支持时才会设置，否则始终为 0。只有当 :ref:`CEC_TX_STATUS_ARB_LOST <CEC-TX-STATUS-ARB-LOST>` 状态位被置位时，该计数才有效。)
    * - __u8
      - ``tx_nack_cnt``
      - A counter of the number of transmit attempts that resulted in the
	Not Acknowledged error. This is only set if the hardware supports
	this, otherwise it is always 0. This counter is only valid if the
	:ref:`CEC_TX_STATUS_NACK <CEC-TX-STATUS-NACK>` status bit is set. (因未应答 (Not Acknowledged) 错误而失败的发送尝试次数计数。仅在硬件支持时才会设置，否则始终为 0。只有当 :ref:`CEC_TX_STATUS_NACK <CEC-TX-STATUS-NACK>` 状态位被置位时，该计数才有效。)
    * - __u8
      - ``tx_low_drive_cnt``
      - A counter of the number of transmit attempts that resulted in the
	Arbitration Lost error. This is only set if the hardware supports
	this, otherwise it is always 0. This counter is only valid if the
	:ref:`CEC_TX_STATUS_LOW_DRIVE <CEC-TX-STATUS-LOW-DRIVE>` status bit is set. (因仲裁丢失 (Arbitration Lost) 错误而失败的发送尝试次数计数。仅在硬件支持时才会设置，否则始终为 0。只有当 :ref:`CEC_TX_STATUS_LOW_DRIVE <CEC-TX-STATUS-LOW-DRIVE>` 状态位被置位时，该计数才有效。)
    * - __u8
      - ``tx_error_cnt``
      - A counter of the number of transmit errors other than Arbitration
	Lost or Not Acknowledged. This is only set if the hardware
	supports this, otherwise it is always 0. This counter is only
	valid if the :ref:`CEC_TX_STATUS_ERROR <CEC-TX-STATUS-ERROR>` status bit is set. (仲裁丢失与未应答之外的其他发送错误计数。仅在硬件支持时才会设置，否则始终为 0。只有当 :ref:`CEC_TX_STATUS_ERROR <CEC-TX-STATUS-ERROR>` 状态位被置位时，该计数才有效。)

.. tabularcolumns:: |p{6.2cm}|p{1.0cm}|p{10.1cm}|

.. _cec-msg-flags:

.. flat-table:: Flags for struct cec_msg
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * .. _`CEC-MSG-FL-REPLY-TO-FOLLOWERS`:

      - ``CEC_MSG_FL_REPLY_TO_FOLLOWERS``
      - 1
      - If a CEC transmit expects a reply, then by default that reply is only sent to
	the filehandle that called :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>`. If this
	flag is set, then the reply is also sent to all followers, if any. If the
	filehandle that called :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` is also a
	follower, then that filehandle will receive the reply twice: once as the
	result of the :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>`, and once via
	:ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>`. (如果 CEC 发送期待一个应答，则默认情况下该应答只会发给调用 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 的文件句柄。如果设置了此标志，则应答还会发给所有关注者 (follower)(如果有)。如果调用 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 的文件句柄本身也是关注者，则该文件句柄会收到应答两次：一次作为 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 的结果，一次经由 :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` 。)

    * .. _`CEC-MSG-FL-RAW`:

      - ``CEC_MSG_FL_RAW``
      - 2
      - Normally CEC messages are validated before transmitting them. If this
        flag is set when :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` is called,
	then no validation takes place and the message is transmitted as-is.
	This is useful when debugging CEC issues.
	This flag is only allowed if the process has the ``CAP_SYS_RAWIO``
	capability. If that is not set, then the ``EPERM`` error code is
	returned. (通常 CEC 消息在发送前会被校验。如果在调用 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 时设置了此标志，则不进行任何校验，消息按原样发送。这在调试 CEC 问题时很有用。仅当进程具有 ``CAP_SYS_RAWIO`` 能力时才允许使用此标志；如果未设置，则返回 ``EPERM`` 错误码。)

    * .. _`CEC-MSG-FL-REPLY-VENDOR-ID`:

      - ``CEC_MSG_FL_REPLY_VENDOR_ID``
      - 4
      - This flag is only available if the ``CEC_CAP_REPLY_VENDOR_ID`` capability
	is set. If this flag is set, then the reply is expected to consist of
	the ``CEC_MSG_VENDOR_COMMAND_WITH_ID`` opcode followed by the Vendor ID
	(in bytes 1-4 of the message), followed by the ``struct cec_msg``
	``reply`` field.

	Note that this assumes that the byte after the Vendor ID is a
	vendor-specific opcode.

	This flag makes it easier to wait for replies to vendor commands.
	(仅当设置了 ``CEC_CAP_REPLY_VENDOR_ID`` 能力时此标志才可用。如果设置了此标志，则应答预期由 ``CEC_MSG_VENDOR_COMMAND_WITH_ID`` 操作码、其后跟随的 Vendor ID(位于消息的 1-4 字节)、再其后跟随的 ``struct cec_msg`` ``reply`` 字段组成。

	注意，这里假定 Vendor ID 之后的字节是一个厂商特定操作码。

	此标志让等待厂商命令的应答变得更加容易。)

.. tabularcolumns:: |p{5.6cm}|p{0.9cm}|p{10.8cm}|

.. _cec-tx-status:

.. flat-table:: CEC Transmit Status
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 16

    * .. _`CEC-TX-STATUS-OK`:

      - ``CEC_TX_STATUS_OK``
      - 0x01
      - The message was transmitted successfully. This is mutually
	exclusive with :ref:`CEC_TX_STATUS_MAX_RETRIES <CEC-TX-STATUS-MAX-RETRIES>`.
	Other bits can still be set if earlier attempts met with failure before
	the transmit was eventually successful. (消息发送成功。与 :ref:`CEC_TX_STATUS_MAX_RETRIES <CEC-TX-STATUS-MAX-RETRIES>` 互斥。如果早先的尝试在最终成功之前遭遇过失败，其他位仍可能被置位。)
    * .. _`CEC-TX-STATUS-ARB-LOST`:

      - ``CEC_TX_STATUS_ARB_LOST``
      - 0x02
      - CEC line arbitration was lost, i.e. another transmit started at the
        same time with a higher priority. Optional status, not all hardware
	can detect this error condition. (CEC 线仲裁失败，即另一发送方以更高优先级同时开始发送。可选状态，并非所有硬件都能检测这种错误状况。)
    * .. _`CEC-TX-STATUS-NACK`:

      - ``CEC_TX_STATUS_NACK``
      - 0x04
      - Message was not acknowledged. Note that some hardware cannot tell apart
        a 'Not Acknowledged' status from other error conditions, i.e. the result
	of a transmit is just OK or FAIL. In that case this status will be
	returned when the transmit failed. (消息未得到应答。注意，某些硬件无法把 'Not Acknowledged' 状态与其他错误状况区分开，即发送结果只有 OK 或 FAIL 两种。在这种情况下，只要发送失败就会返回此状态。)
    * .. _`CEC-TX-STATUS-LOW-DRIVE`:

      - ``CEC_TX_STATUS_LOW_DRIVE``
      - 0x08
      - Low drive was detected on the CEC bus. This indicates that a
	follower detected an error on the bus and requests a
	retransmission. Optional status, not all hardware can detect this
	error condition. (在 CEC 总线上检测到低电平驱动 (low drive)。这表明某个关注者在总线上检测到错误并请求重传。可选状态，并非所有硬件都能检测这种错误状况。)
    * .. _`CEC-TX-STATUS-ERROR`:

      - ``CEC_TX_STATUS_ERROR``
      - 0x10
      - Some error occurred. This is used for any errors that do not fit
	``CEC_TX_STATUS_ARB_LOST`` or ``CEC_TX_STATUS_LOW_DRIVE``, either because
	the hardware could not tell which error occurred, or because the hardware
	tested for other conditions besides those two. Optional status. (发生了某种错误。用于所有不属于 ``CEC_TX_STATUS_ARB_LOST`` 或 ``CEC_TX_STATUS_LOW_DRIVE`` 的错误——可能因为硬件无法分辨发生了哪种错误，也可能因为硬件除这两种之外还测试了其他状况。可选状态。)
    * .. _`CEC-TX-STATUS-MAX-RETRIES`:

      - ``CEC_TX_STATUS_MAX_RETRIES``
      - 0x20
      - The transmit failed after one or more retries. This status bit is
	mutually exclusive with :ref:`CEC_TX_STATUS_OK <CEC-TX-STATUS-OK>`.
	Other bits can still be set to explain which failures were seen. (发送在一次或多次重试后失败。此状态位与 :ref:`CEC_TX_STATUS_OK <CEC-TX-STATUS-OK>` 互斥。其他位仍可能被置位，用来说明曾出现哪些失败。)
    * .. _`CEC-TX-STATUS-ABORTED`:

      - ``CEC_TX_STATUS_ABORTED``
      - 0x40
      - The transmit was aborted due to an HDMI disconnect, or the adapter
        was unconfigured, or a transmit was interrupted, or the driver
	returned an error when attempting to start a transmit. (发送因 HDMI 断开连接、适配器未配置、发送被打断，或驱动在尝试启动发送时返回错误而中止。)
    * .. _`CEC-TX-STATUS-TIMEOUT`:

      - ``CEC_TX_STATUS_TIMEOUT``
      - 0x80
      - The transmit timed out. This should not normally happen and this
	indicates a driver problem. (发送超时。这通常不应发生，它表明驱动存在问题。)

.. tabularcolumns:: |p{5.6cm}|p{0.9cm}|p{10.8cm}|

.. _cec-rx-status:

.. flat-table:: CEC Receive Status
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 16

    * .. _`CEC-RX-STATUS-OK`:

      - ``CEC_RX_STATUS_OK``
      - 0x01
      - The message was received successfully. (消息接收成功。)
    * .. _`CEC-RX-STATUS-TIMEOUT`:

      - ``CEC_RX_STATUS_TIMEOUT``
      - 0x02
      - The reply to an earlier transmitted message timed out. (对先前所发消息的应答等待超时。)
    * .. _`CEC-RX-STATUS-FEATURE-ABORT`:

      - ``CEC_RX_STATUS_FEATURE_ABORT``
      - 0x04
      - The message was received successfully but the reply was
	``CEC_MSG_FEATURE_ABORT``. This status is only set if this message
	was the reply to an earlier transmitted message. (消息接收成功，但应答是 ``CEC_MSG_FEATURE_ABORT``。只有当本消息是对先前所发消息的应答时，才会设置此状态。)
    * .. _`CEC-RX-STATUS-ABORTED`:

      - ``CEC_RX_STATUS_ABORTED``
      - 0x08
      - The wait for a reply to an earlier transmitted message was aborted
        because the HDMI cable was disconnected, the adapter was unconfigured
	or the :ref:`CEC_TRANSMIT <CEC_RECEIVE>` that waited for a
	reply was interrupted. (对先前所发消息的应答等待被中止，原因是 HDMI 线缆断开、适配器未配置，或等待应答的 :ref:`CEC_TRANSMIT <CEC_RECEIVE>` 被打断。)


Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并把 ``errno`` 变量设置为相应的值。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

The :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` can return the following
error codes:

(:ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` 可以返回以下错误码：)

EAGAIN
    No messages are in the receive queue, and the filehandle is in non-blocking mode.
    (接收队列中没有消息，且文件句柄处于非阻塞模式。)

ETIMEDOUT
    The ``timeout`` was reached while waiting for a message.
    (等待消息时超过了 ``timeout``。)

ERESTARTSYS
    The wait for a message was interrupted (e.g. by Ctrl-C).
    (对消息的等待被打断(例如被 Ctrl-C)。)

The :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` can return the following
error codes:

(:ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>` 可以返回以下错误码：)

ENOTTY
    The ``CEC_CAP_TRANSMIT`` capability wasn't set, so this ioctl is not supported.
    (未设置 ``CEC_CAP_TRANSMIT`` 能力，因此不支持该 ioctl。)

EPERM
    The CEC adapter is not configured, i.e. :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`
    has never been called, or ``CEC_MSG_FL_RAW`` was used from a process that
    did not have the ``CAP_SYS_RAWIO`` capability.
    (CEC 适配器未配置，即从未调用过 :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>` ；或者 ``CEC_MSG_FL_RAW`` 被一个不具备 ``CAP_SYS_RAWIO`` 能力的进程使用。)

ENONET
    The CEC adapter is not configured, i.e. :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`
    was called, but the physical address is invalid so no logical address was claimed.
    An exception is made in this case for transmits from initiator 0xf ('Unregistered')
    to destination 0 ('TV'). In that case the transmit will proceed as usual.
    (CEC 适配器未配置，即调用过 :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`，但物理地址无效，因而没有声明任何逻辑地址。此种情形下，从发起方 0xf ('Unregistered') 到目标 0 ('TV') 的发送属于例外，将照常进行。)

EBUSY
    Another filehandle is in exclusive follower or initiator mode, or the filehandle
    is in mode ``CEC_MODE_NO_INITIATOR``. This is also returned if the transmit
    queue is full.
    (另一个文件句柄处于独占关注者或发起方模式，或者本文件句柄处于 ``CEC_MODE_NO_INITIATOR`` 模式。发送队列已满时也会返回此错误码。)

EINVAL
    The contents of struct :c:type:`cec_msg` is invalid.
    (struct :c:type:`cec_msg` 的内容无效。)

ERESTARTSYS
    The wait for a successful transmit was interrupted (e.g. by Ctrl-C).
    (对成功发送的等待被打断(例如被 Ctrl-C)。)
