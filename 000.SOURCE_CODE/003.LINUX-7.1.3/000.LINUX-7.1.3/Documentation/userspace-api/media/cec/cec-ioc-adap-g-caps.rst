.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _CEC_ADAP_G_CAPS:

***********************************
ioctl CEC_ADAP_G_CAPS(查询设备能力)
***********************************

Name(名称)
==========

CEC_ADAP_G_CAPS - Query device capabilities
(CEC_ADAP_G_CAPS - 查询设备能力)

Synopsis(概要)
==============

.. c:macro:: CEC_ADAP_G_CAPS

``int ioctl(int fd, CEC_ADAP_G_CAPS, struct cec_caps *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``

Description(描述)
=================

All cec devices must support :ref:`ioctl CEC_ADAP_G_CAPS <CEC_ADAP_G_CAPS>`. To query
device information, applications call the ioctl with a pointer to a
struct :c:type:`cec_caps`. The driver fills the structure and
returns the information to the application. The ioctl never fails.
(所有 cec 设备都必须支持 :ref:`ioctl CEC_ADAP_G_CAPS <CEC_ADAP_G_CAPS>`。
要查询设备信息，应用程序用指向 struct :c:type:`cec_caps` 的指针调用该
ioctl。驱动填充该结构体并把信息返回给应用程序。该 ioctl 从不失败。)

.. tabularcolumns:: |p{1.2cm}|p{2.5cm}|p{13.6cm}|

.. c:type:: cec_caps

.. flat-table:: struct cec_caps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 16

    * - char
      - ``driver[32]``
      - The name of the cec adapter driver.
	(cec 适配器驱动的名称。)
    * - char
      - ``name[32]``
      - The name of this CEC adapter. The combination ``driver`` and
	``name`` must be unique.
	(此 CEC 适配器的名称。``driver`` 与 ``name`` 的组合必须唯一。)
    * - __u32
      - ``available_log_addrs``
      - The maximum number of logical addresses that can be configured.
	(可配置的逻辑地址的最大数量。)
    * - __u32
      - ``capabilities``
      - The capabilities of the CEC adapter, see
	:ref:`cec-capabilities`.
	(CEC 适配器的能力，参见 :ref:`cec-capabilities`。)
    * - __u32
      - ``version``
      - CEC Framework API version, formatted with the ``KERNEL_VERSION()``
	macro.
	(CEC Framework API 版本，用 ``KERNEL_VERSION()`` 宏格式化。)

.. tabularcolumns:: |p{4.4cm}|p{2.5cm}|p{10.4cm}|

.. _cec-capabilities:

.. flat-table:: CEC Capabilities Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 8

    * .. _`CEC-CAP-PHYS-ADDR`:

      - ``CEC_CAP_PHYS_ADDR``
      - 0x00000001
      - Userspace has to configure the physical address by calling
	:ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>`. If
	this capability isn't set, then setting the physical address is
	handled by the kernel whenever the EDID is set (for an HDMI
	receiver) or read (for an HDMI transmitter).
	(用户空间必须通过调用 :ref:`ioctl CEC_ADAP_S_PHYS_ADDR
	<CEC_ADAP_S_PHYS_ADDR>` 来配置物理地址。如果未设置此能力，则每当
	设置 EDID（对 HDMI 接收器而言）或读取 EDID（对 HDMI 发送器而言）
	时，物理地址的设置都由内核处理。)
    * .. _`CEC-CAP-LOG-ADDRS`:

      - ``CEC_CAP_LOG_ADDRS``
      - 0x00000002
      - Userspace has to configure the logical addresses by calling
	:ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`. If
	this capability isn't set, then the kernel will have configured
	this.
	(用户空间必须通过调用 :ref:`ioctl CEC_ADAP_S_LOG_ADDRS
	<CEC_ADAP_S_LOG_ADDRS>` 来配置逻辑地址。如果未设置此能力，则内核
	已完成配置。)
    * .. _`CEC-CAP-TRANSMIT`:

      - ``CEC_CAP_TRANSMIT``
      - 0x00000004
      - Userspace can transmit CEC messages by calling
	:ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>`. This implies that
	userspace can be a follower as well, since being able to transmit
	messages is a prerequisite of becoming a follower. If this
	capability isn't set, then the kernel will handle all CEC
	transmits and process all CEC messages it receives.
	(用户空间可以通过调用 :ref:`ioctl CEC_TRANSMIT <CEC_TRANSMIT>`
	发送 CEC 消息。这也意味着用户空间同样可以作为 follower（跟随者），
	因为能够发送消息是成为 follower 的前提。如果未设置此能力，则内核
	会处理所有 CEC 发送，并处理其收到的所有 CEC 消息。)
    * .. _`CEC-CAP-PASSTHROUGH`:

      - ``CEC_CAP_PASSTHROUGH``
      - 0x00000008
      - Userspace can use the passthrough mode by calling
	:ref:`ioctl CEC_S_MODE <CEC_S_MODE>`.
	(用户空间可以通过调用 :ref:`ioctl CEC_S_MODE <CEC_S_MODE>` 使用
	直通模式。)
    * .. _`CEC-CAP-RC`:

      - ``CEC_CAP_RC``
      - 0x00000010
      - This adapter supports the remote control protocol.
	(此适配器支持遥控协议。)
    * .. _`CEC-CAP-MONITOR-ALL`:

      - ``CEC_CAP_MONITOR_ALL``
      - 0x00000020
      - The CEC hardware can monitor all messages, not just directed and
	broadcast messages.
	(CEC 硬件可以监视所有消息，而不仅仅是指向 (directed) 和广播
	消息。)
    * .. _`CEC-CAP-NEEDS-HPD`:

      - ``CEC_CAP_NEEDS_HPD``
      - 0x00000040
      - The CEC hardware is only active if the HDMI Hotplug Detect pin is
        high. This makes it impossible to use CEC to wake up displays that
	set the HPD pin low when in standby mode, but keep the CEC bus
	alive.
	(只有当 HDMI 热插拔检测 (Hotplug Detect) 引脚为高电平时，CEC 硬件
	才处于活动状态。这使得无法使用 CEC 唤醒那些在待机模式下把 HPD 引脚
	置低、但仍保持 CEC 总线活跃的显示器。)
    * .. _`CEC-CAP-MONITOR-PIN`:

      - ``CEC_CAP_MONITOR_PIN``
      - 0x00000080
      - The CEC hardware can monitor CEC pin changes from low to high voltage
        and vice versa. When in pin monitoring mode the application will
	receive ``CEC_EVENT_PIN_CEC_LOW`` and ``CEC_EVENT_PIN_CEC_HIGH`` events.
	(CEC 硬件可以监视 CEC 引脚从低电压到高电压的变化以及相反的变化。
	在引脚监视模式下，应用程序将收到 ``CEC_EVENT_PIN_CEC_LOW`` 和
	``CEC_EVENT_PIN_CEC_HIGH`` 事件。)
    * .. _`CEC-CAP-CONNECTOR-INFO`:

      - ``CEC_CAP_CONNECTOR_INFO``
      - 0x00000100
      - If this capability is set, then :ref:`CEC_ADAP_G_CONNECTOR_INFO` can
        be used.
	(如果设置了此能力，则可以使用 :ref:`CEC_ADAP_G_CONNECTOR_INFO`。)
    * .. _`CEC-CAP-REPLY-VENDOR-ID`:

      - ``CEC_CAP_REPLY_VENDOR_ID``
      - 0x00000200
      - If this capability is set, then
        :ref:`CEC_MSG_FL_REPLY_VENDOR_ID <cec-msg-flags>` can be used.
	(如果设置了此能力，则可以使用
	:ref:`CEC_MSG_FL_REPLY_VENDOR_ID <cec-msg-flags>`。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
