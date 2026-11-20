.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _CEC_MODE:
.. _CEC_G_MODE:
.. _CEC_S_MODE:

*****************************************************************
ioctls CEC_G_MODE and CEC_S_MODE(ioctls CEC_G_MODE 与 CEC_S_MODE)
*****************************************************************

CEC_G_MODE, CEC_S_MODE - Get or set exclusive use of the CEC adapter
(CEC_G_MODE、CEC_S_MODE - 获取或设置对 CEC 适配器的独占使用)

Synopsis(概要)
==============

.. c:macro:: CEC_G_MODE

``int ioctl(int fd, CEC_G_MODE, __u32 *argp)``

.. c:macro:: CEC_S_MODE

``int ioctl(int fd, CEC_S_MODE, __u32 *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 open() 返回的文件描述符。)

``argp``
    Pointer to CEC mode.
    (指向 CEC 模式的指针。)

Description(描述)
=================

By default any filehandle can use :ref:`CEC_TRANSMIT`, but in order to prevent
applications from stepping on each others toes it must be possible to
obtain exclusive access to the CEC adapter. This ioctl sets the
filehandle to initiator and/or follower mode which can be exclusive
depending on the chosen mode. The initiator is the filehandle that is
used to initiate messages, i.e. it commands other CEC devices. The
follower is the filehandle that receives messages sent to the CEC
adapter and processes them. The same filehandle can be both initiator
and follower, or this role can be taken by two different filehandles.
(默认情况下，任何文件句柄都可以使用 CEC_TRANSMIT；但为了防止应用程序之间
互相干扰，必须能够获取对 CEC 适配器的独占访问。该 ioctl 将文件句柄设置为
发起者 (initiator) 和/或跟随者 (follower) 模式，根据所选模式这可以是独占
的。发起者是用于发起消息的文件句柄，即它向其他 CEC 设备下达命令。跟随者
是接收发送到 CEC 适配器的消息并处理它们的文件句柄。同一个文件句柄既可以
是发起者又是跟随者，或者这两个角色也可以由两个不同的文件句柄分别承担。)

When a CEC message is received, then the CEC framework will decide how
it will be processed. If the message is a reply to an earlier
transmitted message, then the reply is sent back to the filehandle that
is waiting for it. In addition the CEC framework will process it.
(当接收到一条 CEC 消息时，CEC 框架会决定如何处理它。如果该消息是对先前
发送消息的应答，那么应答会被送回等待它的文件句柄。此外，CEC 框架也会
处理它。)

If the message is not a reply, then the CEC framework will process it
first. If there is no follower, then the message is just discarded and a
feature abort is sent back to the initiator if the framework couldn't
process it. If there is a follower, then the message is passed on to the
follower who will use :ref:`ioctl CEC_RECEIVE <CEC_RECEIVE>` to dequeue
the new message. The framework expects the follower to make the right
decisions.
(如果该消息不是应答，那么 CEC 框架会先处理它。如果没有跟随者，那么该
消息会被直接丢弃，并且当框架无法处理它时，会向发起者回发一条 feature
abort。如果有跟随者，那么该消息会被传递给跟随者，由它使用 CEC_RECEIVE
将新消息出队。框架期望跟随者做出正确的决策。)

The CEC framework will process core messages unless requested otherwise
by the follower. The follower can enable the passthrough mode. In that
case, the CEC framework will pass on most core messages without
processing them and the follower will have to implement those messages.
There are some messages that the core will always process, regardless of
the passthrough mode. See :ref:`cec-core-processing` for details.
(除非跟随者另有要求，CEC 框架会处理核心消息。跟随者可以启用直通
(passthrough) 模式。在这种情况下，CEC 框架会原样传递大多数核心消息而不
做处理，跟随者将不得不自己实现这些消息。有些消息无论直通模式如何，核心
都会始终处理。详情参见 cec-core-processing 一节。)

If there is no initiator, then any CEC filehandle can use
:ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>`. If there is an exclusive
initiator then only that initiator can call
:ref:`CEC_TRANSMIT`. The follower can of course
always call :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>`.
(如果没有发起者，那么任何 CEC 文件句柄都可以使用 CEC_TRANSMIT。如果存在
独占发起者，那么只有该发起者可以调用 CEC_TRANSMIT。当然，跟随者始终可以
调用 CEC_TRANSMIT。)

Available initiator modes are:
(可用的发起者模式如下：)

.. tabularcolumns:: |p{5.6cm}|p{0.9cm}|p{10.8cm}|

.. _cec-mode-initiator_e:

.. flat-table:: Initiator Modes
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 16

    * .. _`CEC-MODE-NO-INITIATOR`:

      - ``CEC_MODE_NO_INITIATOR``
      - 0x0
      - This is not an initiator, i.e. it cannot transmit CEC messages or
	make any other changes to the CEC adapter. (这不是发起者，即它不能发送
	CEC 消息，也不能对 CEC 适配器做出任何其他更改。)
    * .. _`CEC-MODE-INITIATOR`:

      - ``CEC_MODE_INITIATOR``
      - 0x1
      - This is an initiator (the default when the device is opened) and
	it can transmit CEC messages and make changes to the CEC adapter,
	unless there is an exclusive initiator. (这是发起者 (打开设备时的默认
	值)，它可以发送 CEC 消息并对 CEC 适配器做出更改，除非存在独占发起者。)
    * .. _`CEC-MODE-EXCL-INITIATOR`:

      - ``CEC_MODE_EXCL_INITIATOR``
      - 0x2
      - This is an exclusive initiator and this file descriptor is the
	only one that can transmit CEC messages and make changes to the
	CEC adapter. If someone else is already the exclusive initiator
	then an attempt to become one will return the ``EBUSY`` error code
	error. (这是独占发起者，此文件描述符是唯一可以发送 CEC 消息并对 CEC
	适配器做出更改的文件描述符。如果别人已经是独占发起者，那么试图成为
	独占发起者将返回 ``EBUSY`` 错误码。)

Available follower modes are:

.. tabularcolumns:: |p{6.6cm}|p{0.9cm}|p{9.8cm}|

.. _cec-mode-follower_e:

.. cssclass:: longtable

.. flat-table:: Follower Modes
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 16

    * .. _`CEC-MODE-NO-FOLLOWER`:

      - ``CEC_MODE_NO_FOLLOWER``
      - 0x00
      - This is not a follower (the default when the device is opened).
	(这不是跟随者 (打开设备时的默认值)。)
    * .. _`CEC-MODE-FOLLOWER`:

      - ``CEC_MODE_FOLLOWER``
      - 0x10
      - This is a follower and it will receive CEC messages unless there
	is an exclusive follower. You cannot become a follower if
	:ref:`CEC_CAP_TRANSMIT <CEC-CAP-TRANSMIT>` is not set or if :ref:`CEC_MODE_NO_INITIATOR <CEC-MODE-NO-INITIATOR>`
	was specified, the ``EINVAL`` error code is returned in that case.
	(这是跟随者，除非存在独占跟随者，否则它将接收 CEC 消息。如果
	CEC_CAP_TRANSMIT 未设置或指定了 CEC_MODE_NO_INITIATOR，则不能成为
	跟随者，此时返回 ``EINVAL`` 错误码。)
    * .. _`CEC-MODE-EXCL-FOLLOWER`:

      - ``CEC_MODE_EXCL_FOLLOWER``
      - 0x20
      - This is an exclusive follower and only this file descriptor will
	receive CEC messages for processing. If someone else is already
	the exclusive follower then an attempt to become one will return
	the ``EBUSY`` error code. You cannot become a follower if
	:ref:`CEC_CAP_TRANSMIT <CEC-CAP-TRANSMIT>` is not set or if :ref:`CEC_MODE_NO_INITIATOR <CEC-MODE-NO-INITIATOR>`
	was specified, the ``EINVAL`` error code is returned in that case.
	(这是独占跟随者，只有此文件描述符会接收 CEC 消息进行处理。如果别人
	已经是独占跟随者，那么试图成为独占跟随者将返回 ``EBUSY`` 错误码。如果
	CEC_CAP_TRANSMIT 未设置或指定了 CEC_MODE_NO_INITIATOR，则不能成为
	跟随者，此时返回 ``EINVAL`` 错误码。)
    * .. _`CEC-MODE-EXCL-FOLLOWER-PASSTHRU`:

      - ``CEC_MODE_EXCL_FOLLOWER_PASSTHRU``
      - 0x30
      - This is an exclusive follower and only this file descriptor will
	receive CEC messages for processing. In addition it will put the
	CEC device into passthrough mode, allowing the exclusive follower
	to handle most core messages instead of relying on the CEC
	framework for that. If someone else is already the exclusive
	follower then an attempt to become one will return the ``EBUSY`` error
	code. You cannot become a follower if :ref:`CEC_CAP_TRANSMIT <CEC-CAP-TRANSMIT>`
	is not set or if :ref:`CEC_MODE_NO_INITIATOR <CEC-MODE-NO-INITIATOR>` was specified,
	the ``EINVAL`` error code is returned in that case.
	(这是独占跟随者，只有此文件描述符会接收 CEC 消息进行处理。此外，它会把
	CEC 设备置入直通模式，让独占跟随者处理大多数核心消息，而不是依赖 CEC
	框架来处理。如果别人已经是独占跟随者，那么试图成为独占跟随者将返回
	``EBUSY`` 错误码。如果 CEC_CAP_TRANSMIT 未设置或指定了
	CEC_MODE_NO_INITIATOR，则不能成为跟随者，此时返回 ``EINVAL`` 错误码。)
    * .. _`CEC-MODE-MONITOR-PIN`:

      - ``CEC_MODE_MONITOR_PIN``
      - 0xd0
      - Put the file descriptor into pin monitoring mode. Can only be used in
	combination with :ref:`CEC_MODE_NO_INITIATOR <CEC-MODE-NO-INITIATOR>`,
	otherwise the ``EINVAL`` error code will be returned.
	This mode requires that the :ref:`CEC_CAP_MONITOR_PIN <CEC-CAP-MONITOR-PIN>`
	capability is set, otherwise the ``EINVAL`` error code is returned.
	While in pin monitoring mode this file descriptor can receive the
	``CEC_EVENT_PIN_CEC_LOW`` and ``CEC_EVENT_PIN_CEC_HIGH`` events to see the
	low-level CEC pin transitions. This is very useful for debugging.
	This mode is only allowed if the process has the ``CAP_NET_ADMIN``
	capability. If that is not set, then the ``EPERM`` error code is returned.
	(把文件描述符置入引脚监控 (pin monitoring) 模式。只能与
	CEC_MODE_NO_INITIATOR 组合使用，否则将返回 ``EINVAL`` 错误码。该模式
	要求设置了 CEC_CAP_MONITOR_PIN 能力，否则返回 ``EINVAL`` 错误码。处于
	引脚监控模式时，此文件描述符可以接收 CEC_EVENT_PIN_CEC_LOW 与
	CEC_EVENT_PIN_CEC_HIGH 事件，以观察底层 CEC 引脚的电平变化。这对调试
	非常有用。只有进程拥有 ``CAP_NET_ADMIN`` 能力时才允许此模式；如果未
	设置，则返回 ``EPERM`` 错误码。)
    * .. _`CEC-MODE-MONITOR`:

      - ``CEC_MODE_MONITOR``
      - 0xe0
      - Put the file descriptor into monitor mode. Can only be used in
	combination with :ref:`CEC_MODE_NO_INITIATOR <CEC-MODE-NO-INITIATOR>`,
	otherwise the ``EINVAL`` error code will be returned.
	In monitor mode all messages this CEC
	device transmits and all messages it receives (both broadcast
	messages and directed messages for one its logical addresses) will
	be reported. This is very useful for debugging. This is only
	allowed if the process has the ``CAP_NET_ADMIN`` capability. If
	that is not set, then the ``EPERM`` error code is returned.
	(把文件描述符置入监控 (monitor) 模式。只能与 CEC_MODE_NO_INITIATOR
	组合使用，否则将返回 ``EINVAL`` 错误码。在监控模式下，此 CEC 设备发送
	的所有消息以及它接收到的所有消息 (既包括广播消息，也包括发给其某个
	逻辑地址的定向消息) 都会被上报。这对调试非常有用。只有进程拥有
	``CAP_NET_ADMIN`` 能力时才允许此模式；如果未设置，则返回 ``EPERM``
	错误码。)
    * .. _`CEC-MODE-MONITOR-ALL`:

      - ``CEC_MODE_MONITOR_ALL``
      - 0xf0
      - Put the file descriptor into 'monitor all' mode. Can only be used
	in combination with :ref:`CEC_MODE_NO_INITIATOR <CEC-MODE-NO-INITIATOR>`, otherwise
	the ``EINVAL`` error code will be returned. In 'monitor all' mode all messages
	this CEC device transmits and all messages it receives, including
	directed messages for other CEC devices, will be reported. This is
	very useful for debugging, but not all devices support this. This
	mode requires that the :ref:`CEC_CAP_MONITOR_ALL <CEC-CAP-MONITOR-ALL>` capability is set,
	otherwise the ``EINVAL`` error code is returned. This is only allowed if
	the process has the ``CAP_NET_ADMIN`` capability. If that is not
	set, then the ``EPERM`` error code is returned.
	(把文件描述符置入 'monitor all' 模式。只能与 CEC_MODE_NO_INITIATOR
	组合使用，否则将返回 ``EINVAL`` 错误码。在 'monitor all' 模式下，此
	CEC 设备发送的所有消息以及它接收到的所有消息 (包括发给其他 CEC 设备的
	定向消息) 都会被上报。这对调试非常有用，但并非所有设备都支持。该模式
	要求设置了 CEC_CAP_MONITOR_ALL 能力，否则返回 ``EINVAL`` 错误码。只有
	进程拥有 ``CAP_NET_ADMIN`` 能力时才允许此模式；如果未设置，则返回
	``EPERM`` 错误码。)

Core message processing details:
(核心消息处理细节：)

.. tabularcolumns:: |p{6.6cm}|p{10.9cm}|

.. _cec-core-processing:

.. flat-table:: Core Message Processing
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 8

    * .. _`CEC-MSG-GET-CEC-VERSION`:

      - ``CEC_MSG_GET_CEC_VERSION``
      - The core will return the CEC version that was set with
	:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`,
	except when in passthrough mode. In passthrough mode the core
	does nothing and this message has to be handled by a follower
	instead.
	(核心将返回通过 CEC_ADAP_S_LOG_ADDRS 设置的 CEC 版本，直通模式除外。
	在直通模式下核心不做任何事，该消息必须改由跟随者处理。)
    * .. _`CEC-MSG-GIVE-DEVICE-VENDOR-ID`:

      - ``CEC_MSG_GIVE_DEVICE_VENDOR_ID``
      - The core will return the vendor ID that was set with
	:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`,
	except when in passthrough mode. In passthrough mode the core
	does nothing and this message has to be handled by a follower
	instead.
	(核心将返回通过 CEC_ADAP_S_LOG_ADDRS 设置的厂商 ID，直通模式除外。
	在直通模式下核心不做任何事，该消息必须改由跟随者处理。)
    * .. _`CEC-MSG-ABORT`:

      - ``CEC_MSG_ABORT``
      - The core will return a Feature Abort message with reason
        'Feature Refused' as per the specification, except when in
        passthrough mode. In passthrough mode the core does nothing
        and this message has to be handled by a follower instead.
	(核心将按照规范返回一条原因为 'Feature Refused' 的 Feature Abort
	消息，直通模式除外。在直通模式下核心不做任何事，该消息必须改由跟随
	者处理。)
    * .. _`CEC-MSG-GIVE-PHYSICAL-ADDR`:

      - ``CEC_MSG_GIVE_PHYSICAL_ADDR``
      - The core will report the current physical address, except when
        in passthrough mode. In passthrough mode the core does nothing
        and this message has to be handled by a follower instead.
	(核心将报告当前物理地址，直通模式除外。在直通模式下核心不做任何事，
	该消息必须改由跟随者处理。)
    * .. _`CEC-MSG-GIVE-OSD-NAME`:

      - ``CEC_MSG_GIVE_OSD_NAME``
      - The core will report the current OSD name that was set with
	:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`,
	except when in passthrough mode. In passthrough mode the core
	does nothing and this message has to be handled by a follower
	instead.
	(核心将报告通过 CEC_ADAP_S_LOG_ADDRS 设置的当前 OSD 名称，直通模式
	除外。在直通模式下核心不做任何事，该消息必须改由跟随者处理。)
    * .. _`CEC-MSG-GIVE-FEATURES`:

      - ``CEC_MSG_GIVE_FEATURES``
      - The core will do nothing if the CEC version is older than 2.0,
        otherwise it will report the current features that were set with
	:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`,
	except when in passthrough mode. In passthrough mode the core
	does nothing (for any CEC version) and this message has to be handled
	by a follower instead.
	(如果 CEC 版本早于 2.0，核心不做任何事；否则它将报告通过
	CEC_ADAP_S_LOG_ADDRS 设置的当前特性，直通模式除外。在直通模式下核心
	不做任何事 (对任何 CEC 版本均如此)，该消息必须改由跟随者处理。)
    * .. _`CEC-MSG-USER-CONTROL-PRESSED`:

      - ``CEC_MSG_USER_CONTROL_PRESSED``
      - If :ref:`CEC_CAP_RC <CEC-CAP-RC>` is set and if
        :ref:`CEC_LOG_ADDRS_FL_ALLOW_RC_PASSTHRU <CEC-LOG-ADDRS-FL-ALLOW-RC-PASSTHRU>`
        is set, then generate a remote control key
	press. This message is always passed on to the follower(s).
	(如果设置了 CEC_CAP_RC 并且设置了
	CEC_LOG_ADDRS_FL_ALLOW_RC_PASSTHRU，则生成一次遥控器按键按下。该
	消息始终会被传递给跟随者。)
    * .. _`CEC-MSG-USER-CONTROL-RELEASED`:

      - ``CEC_MSG_USER_CONTROL_RELEASED``
      - If :ref:`CEC_CAP_RC <CEC-CAP-RC>` is set and if
        :ref:`CEC_LOG_ADDRS_FL_ALLOW_RC_PASSTHRU <CEC-LOG-ADDRS-FL-ALLOW-RC-PASSTHRU>`
        is set, then generate a remote control key
	release. This message is always passed on to the follower(s).
	(如果设置了 CEC_CAP_RC 并且设置了
	CEC_LOG_ADDRS_FL_ALLOW_RC_PASSTHRU，则生成一次遥控器按键释放。该
	消息始终会被传递给跟随者。)
    * .. _`CEC-MSG-REPORT-PHYSICAL-ADDR`:

      - ``CEC_MSG_REPORT_PHYSICAL_ADDR``
      - The CEC framework will make note of the reported physical address
	and then just pass the message on to the follower(s).
	(CEC 框架会记录所上报的物理地址，然后把该消息继续传递给跟随者。)


Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1 并相应地设置 ``errno`` 变量。通用错误码在
Generic Error Codes 一章中描述。)

The :ref:`ioctl CEC_S_MODE <CEC_S_MODE>` can return the following
error codes:
(CEC_S_MODE ioctl 可能返回以下错误码：)

EINVAL
    The requested mode is invalid.
    (请求的模式无效。)

EPERM
    Monitor mode is requested, but the process does have the ``CAP_NET_ADMIN``
    capability.
    (请求了监控模式，但进程没有 ``CAP_NET_ADMIN`` 能力。)

EBUSY
    Someone else is already an exclusive follower or initiator.
    (别人已经是独占跟随者或发起者。)
