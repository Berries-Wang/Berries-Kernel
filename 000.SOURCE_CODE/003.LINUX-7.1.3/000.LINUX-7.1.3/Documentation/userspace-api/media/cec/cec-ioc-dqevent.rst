.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _CEC_DQEVENT:

********************************
ioctl CEC_DQEVENT(出队 CEC 事件)
********************************

Name(名称)
==========

CEC_DQEVENT - Dequeue a CEC event(CEC_DQEVENT - 出队一个 CEC 事件)

Synopsis(概要)
===============

.. c:macro:: CEC_DQEVENT

``int ioctl(int fd, CEC_DQEVENT, struct cec_event *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``

Description(描述)
=================

CEC devices can send asynchronous events. These can be retrieved by
calling :c:func:`CEC_DQEVENT`. If the file descriptor is in
non-blocking mode and no event is pending, then it will return -1 and
set errno to the ``EAGAIN`` error code.
(CEC 设备可以发送异步事件。可以通过调用 :c:func:`CEC_DQEVENT` 来获取这些事件。如果文件描述符处于非阻塞模式且没有待处理的事件，则返回 -1 并把 errno 设为 ``EAGAIN`` 错误码。)

The internal event queues are per-filehandle and per-event type. If
there is no more room in a queue then the last event is overwritten with
the new one. This means that intermediate results can be thrown away but
that the latest event is always available. This also means that is it
possible to read two successive events that have the same value (e.g.
two :ref:`CEC_EVENT_STATE_CHANGE <CEC-EVENT-STATE-CHANGE>` events with
the same state). In that case the intermediate state changes were lost but
it is guaranteed that the state did change in between the two events.
(内部事件队列按文件句柄与事件类型划分。如果队列没有更多空间，最后一个事件会被新事件覆盖。这意味着中间结果可能被丢弃，但最新的事件总是可用。这也意味着可能读到两个值相同的连续事件 (例如两个状态相同的
:ref:`CEC_EVENT_STATE_CHANGE <CEC-EVENT-STATE-CHANGE>` 事件)。这种情况下中间的状态变化丢失了，但可以保证两个事件之间状态确实发生过变化。)

.. tabularcolumns:: |p{1.2cm}|p{2.9cm}|p{13.2cm}|

.. c:type:: cec_event_state_change

.. flat-table:: struct cec_event_state_change
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 8

    * - __u16
      - ``phys_addr``
      - The current physical address. This is ``CEC_PHYS_ADDR_INVALID`` if no
        valid physical address is set.
        (当前物理地址。若未设置有效的物理地址，则为 ``CEC_PHYS_ADDR_INVALID``。)
    * - __u16
      - ``log_addr_mask``
      - The current set of claimed logical addresses. This is 0 if no logical
        addresses are claimed or if ``phys_addr`` is ``CEC_PHYS_ADDR_INVALID``.
	If bit 15 is set (``1 << CEC_LOG_ADDR_UNREGISTERED``) then this device
	has the unregistered logical address. In that case all other bits are 0.
        (当前已声明的逻辑地址集合。若未声明逻辑地址或 ``phys_addr`` 为 ``CEC_PHYS_ADDR_INVALID`` ，则为 0。若第 15 位被置位 (``1 << CEC_LOG_ADDR_UNREGISTERED``)，则该设备拥有未注册逻辑地址；此时其余各位均为 0。)
    * - __u16
      - ``have_conn_info``
      - If non-zero, then HDMI connector information is available.
        This field is only valid if ``CEC_CAP_CONNECTOR_INFO`` is set. If that
        capability is set and ``have_conn_info`` is zero, then that indicates
        that the HDMI connector device is not instantiated, either because
        the HDMI driver is still configuring the device or because the HDMI
        device was unbound.
        (若非零，则 HDMI 连接器信息可用。该字段仅在设置了 ``CEC_CAP_CONNECTOR_INFO`` 时有效。若设置了该能力而 ``have_conn_info`` 为零，则表示 HDMI 连接器设备未实例化，原因可能是 HDMI 驱动仍在配置设备，或 HDMI 设备已解绑。)

.. c:type:: cec_event_lost_msgs

.. tabularcolumns:: |p{1.0cm}|p{2.0cm}|p{14.3cm}|

.. flat-table:: struct cec_event_lost_msgs
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 16

    * - __u32
      - ``lost_msgs``
      - Set to the number of lost messages since the filehandle was opened
	or since the last time this event was dequeued for this
	filehandle. The messages lost are the oldest messages. So when a
	new message arrives and there is no more room, then the oldest
	message is discarded to make room for the new one. The internal
	size of the message queue guarantees that all messages received in
	the last two seconds will be stored. Since messages should be
	replied to within a second according to the CEC specification,
	this is more than enough.
        (设置为自文件句柄打开以来、或自上次为该文件句柄出队此事件以来丢失的消息数。丢失的是最旧的消息：当新消息到达且没有空间时，最旧的消息会被丢弃以腾出空间。消息队列的内部大小保证最近两秒内收到的所有消息都会被存储。由于按 CEC 规范消息应在一秒内回复，这已绰绰有余。)

.. tabularcolumns:: |p{1.0cm}|p{4.4cm}|p{2.5cm}|p{9.2cm}|

.. c:type:: cec_event

.. flat-table:: struct cec_event
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 8

    * - __u64
      - ``ts``
      - Timestamp of the event in ns.

	The timestamp has been taken from the ``CLOCK_MONOTONIC`` clock.

	To access the same clock from userspace use :c:func:`clock_gettime`.
        (事件的时间戳，单位 ns。该时间戳取自 ``CLOCK_MONOTONIC`` 时钟。用户态要访问同一时钟可使用 :c:func:`clock_gettime`。)
    * - __u32
      - ``event``
      - The CEC event type, see :ref:`cec-events`. (CEC 事件类型，见 :ref:`cec-events`。)
    * - __u32
      - ``flags``
      - Event flags, see :ref:`cec-event-flags`. (事件标志，见 :ref:`cec-event-flags`。)
    * - union {
      - (anonymous)
    * - struct cec_event_state_change
      - ``state_change``
      - The new adapter state as sent by the :ref:`CEC_EVENT_STATE_CHANGE <CEC-EVENT-STATE-CHANGE>`
	event.
        (由 :ref:`CEC_EVENT_STATE_CHANGE <CEC-EVENT-STATE-CHANGE>` 事件发送的新适配器状态。)
    * - struct cec_event_lost_msgs
      - ``lost_msgs``
      - The number of lost messages as sent by the :ref:`CEC_EVENT_LOST_MSGS <CEC-EVENT-LOST-MSGS>`
	event.
        (由 :ref:`CEC_EVENT_LOST_MSGS <CEC-EVENT-LOST-MSGS>` 事件发送的丢失消息数。)
    * - }
      -

.. tabularcolumns:: |p{5.6cm}|p{0.9cm}|p{10.8cm}|

.. _cec-events:

.. flat-table:: CEC Events Types
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 16

    * .. _`CEC-EVENT-STATE-CHANGE`:

      - ``CEC_EVENT_STATE_CHANGE``
      - 1
      - Generated when the CEC Adapter's state changes. When open() is
	called an initial event will be generated for that filehandle with
	the CEC Adapter's state at that time.
        (在 CEC 适配器状态变化时产生。调用 open() 时会为该文件句柄产生一个携带当时 CEC 适配器状态的初始事件。)
    * .. _`CEC-EVENT-LOST-MSGS`:

      - ``CEC_EVENT_LOST_MSGS``
      - 2
      - Generated if one or more CEC messages were lost because the
	application didn't dequeue CEC messages fast enough.
        (当因应用未能足够快地出队 CEC 消息而丢失一条或多条 CEC 消息时产生。)
    * .. _`CEC-EVENT-PIN-CEC-LOW`:

      - ``CEC_EVENT_PIN_CEC_LOW``
      - 3
      - Generated if the CEC pin goes from a high voltage to a low voltage.
        Only applies to adapters that have the ``CEC_CAP_MONITOR_PIN``
	capability set.
        (在 CEC 引脚从高电平变为低电平时产生。只适用于设置了 ``CEC_CAP_MONITOR_PIN`` 能力的适配器。)
    * .. _`CEC-EVENT-PIN-CEC-HIGH`:

      - ``CEC_EVENT_PIN_CEC_HIGH``
      - 4
      - Generated if the CEC pin goes from a low voltage to a high voltage.
        Only applies to adapters that have the ``CEC_CAP_MONITOR_PIN``
	capability set.
        (在 CEC 引脚从低电平变为高电平时产生。只适用于设置了 ``CEC_CAP_MONITOR_PIN`` 能力的适配器。)
    * .. _`CEC-EVENT-PIN-HPD-LOW`:

      - ``CEC_EVENT_PIN_HPD_LOW``
      - 5
      - Generated if the HPD pin goes from a high voltage to a low voltage.
	Only applies to adapters that have the ``CEC_CAP_MONITOR_PIN``
	capability set. When open() is called, the HPD pin can be read and
	if the HPD is low, then an initial event will be generated for that
	filehandle.
        (在 HPD 引脚从高电平变为低电平时产生。只适用于设置了 ``CEC_CAP_MONITOR_PIN`` 能力的适配器。调用 open() 时可以读取 HPD 引脚，若 HPD 为低，则会为该文件句柄产生一个初始事件。)
    * .. _`CEC-EVENT-PIN-HPD-HIGH`:

      - ``CEC_EVENT_PIN_HPD_HIGH``
      - 6
      - Generated if the HPD pin goes from a low voltage to a high voltage.
	Only applies to adapters that have the ``CEC_CAP_MONITOR_PIN``
	capability set. When open() is called, the HPD pin can be read and
	if the HPD is high, then an initial event will be generated for that
	filehandle.
        (在 HPD 引脚从低电平变为高电平时产生。只适用于设置了 ``CEC_CAP_MONITOR_PIN`` 能力的适配器。调用 open() 时可以读取 HPD 引脚，若 HPD 为高，则会为该文件句柄产生一个初始事件。)
    * .. _`CEC-EVENT-PIN-5V-LOW`:

      - ``CEC_EVENT_PIN_5V_LOW``
      - 6
      - Generated if the 5V pin goes from a high voltage to a low voltage.
	Only applies to adapters that have the ``CEC_CAP_MONITOR_PIN``
	capability set. When open() is called, the 5V pin can be read and
	if the 5V is low, then an initial event will be generated for that
	filehandle.
        (在 5V 引脚从高电平变为低电平时产生。只适用于设置了 ``CEC_CAP_MONITOR_PIN`` 能力的适配器。调用 open() 时可以读取 5V 引脚，若 5V 为低，则会为该文件句柄产生一个初始事件。)
    * .. _`CEC-EVENT-PIN-5V-HIGH`:

      - ``CEC_EVENT_PIN_5V_HIGH``
      - 7
      - Generated if the 5V pin goes from a low voltage to a high voltage.
	Only applies to adapters that have the ``CEC_CAP_MONITOR_PIN``
	capability set. When open() is called, the 5V pin can be read and
	if the 5V is high, then an initial event will be generated for that
	filehandle.
        (在 5V 引脚从低电平变为高电平时产生。只适用于设置了 ``CEC_CAP_MONITOR_PIN`` 能力的适配器。调用 open() 时可以读取 5V 引脚，若 5V 为高，则会为该文件句柄产生一个初始事件。)

.. tabularcolumns:: |p{6.0cm}|p{0.6cm}|p{10.7cm}|

.. _cec-event-flags:

.. flat-table:: CEC Event Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 8

    * .. _`CEC-EVENT-FL-INITIAL-STATE`:

      - ``CEC_EVENT_FL_INITIAL_STATE``
      - 1
      - Set for the initial events that are generated when the device is
	opened. See the table above for which events do this. This allows
	applications to learn the initial state of the CEC adapter at
	open() time.
        (为设备打开时产生的初始事件设置。哪些事件会这样做见上表。这使应用能够在 open() 时获知 CEC 适配器的初始状态。)
    * .. _`CEC-EVENT-FL-DROPPED-EVENTS`:

      - ``CEC_EVENT_FL_DROPPED_EVENTS``
      - 2
      - Set if one or more events of the given event type have been dropped.
        This is an indication that the application cannot keep up.
        (当给定事件类型的一个或多个事件被丢弃时设置。这表明应用已跟不上处理速度。)


Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

The :ref:`ioctl CEC_DQEVENT <CEC_DQEVENT>` can return the following
error codes:
(:ref:`ioctl CEC_DQEVENT <CEC_DQEVENT>` 可返回以下错误码：)

EAGAIN
    This is returned when the filehandle is in non-blocking mode and there
    are no pending events.
    (文件句柄处于非阻塞模式且没有待处理事件时返回。)

ERESTARTSYS
    An interrupt (e.g. Ctrl-C) arrived while in blocking mode waiting for
    events to arrive.
    (在阻塞模式等待事件到达时收到中断 (例如 Ctrl-C)。)
