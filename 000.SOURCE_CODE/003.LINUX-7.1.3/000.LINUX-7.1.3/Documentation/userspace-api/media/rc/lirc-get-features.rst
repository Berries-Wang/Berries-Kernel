.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_get_features:

************************************************
ioctl LIRC_GET_FEATURES(ioctl LIRC_GET_FEATURES)
************************************************

Name(名称)
==========

LIRC_GET_FEATURES - Get the underlying hardware device's features
(LIRC_GET_FEATURES - 获取底层硬件设备的特性)

Synopsis(概要)
==============

.. c:macro:: LIRC_GET_FEATURES

``int ioctl(int fd, LIRC_GET_FEATURES, __u32 *features)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``features``
    Bitmask with the LIRC features.
    (包含 LIRC 特性的位掩码。)

Description(描述)
=================

Get the underlying hardware device's features. If a driver does not
announce support of certain features, calling of the corresponding ioctls
is undefined.
(获取底层硬件设备的特性。如果驱动未声明支持某些特性，则调用相应 ioctl 的行为是未定义的。)

LIRC features(LIRC 特性)
========================

.. _LIRC-CAN-REC-RAW:

``LIRC_CAN_REC_RAW``

    Unused. Kept just to avoid breaking uAPI.
    (未使用。保留它只是为了避免破坏 uAPI。)

.. _LIRC-CAN-REC-PULSE:

``LIRC_CAN_REC_PULSE``

    Unused. Kept just to avoid breaking uAPI.
    :ref:`LIRC_MODE_PULSE <lirc-mode-pulse>` can only be used for transmitting.
    (未使用。保留它只是为了避免破坏 uAPI。 :ref:`LIRC_MODE_PULSE <lirc-mode-pulse>` 只能用于发送。)

.. _LIRC-CAN-REC-MODE2:

``LIRC_CAN_REC_MODE2``

    This is raw IR driver for receiving. This means that
    :ref:`LIRC_MODE_MODE2 <lirc-mode-MODE2>` is used. This also implies
    that :ref:`LIRC_MODE_SCANCODE <lirc-mode-SCANCODE>` is also supported,
    as long as the kernel is recent enough. Use the
    :ref:`lirc_set_rec_mode` to switch modes.
    (这是用于接收的原始红外 (raw IR) 驱动。这意味着使用的是 :ref:`LIRC_MODE_MODE2 <lirc-mode-MODE2>` 。这也意味着，只要内核足够新， :ref:`LIRC_MODE_SCANCODE <lirc-mode-SCANCODE>` 也受支持。使用 :ref:`lirc_set_rec_mode` 来切换模式。)

.. _LIRC-CAN-REC-LIRCCODE:

``LIRC_CAN_REC_LIRCCODE``

    Unused. Kept just to avoid breaking uAPI.
    (未使用。保留它只是为了避免破坏 uAPI。)

.. _LIRC-CAN-REC-SCANCODE:

``LIRC_CAN_REC_SCANCODE``

    This is a scancode driver for receiving. This means that
    :ref:`LIRC_MODE_SCANCODE <lirc-mode-SCANCODE>` is used.
    (这是用于接收的扫描码驱动。这意味着使用的是 :ref:`LIRC_MODE_SCANCODE <lirc-mode-SCANCODE>` 。)

.. _LIRC-CAN-SET-SEND-CARRIER:

``LIRC_CAN_SET_SEND_CARRIER``

    The driver supports changing the modulation frequency via
    :ref:`ioctl LIRC_SET_SEND_CARRIER <LIRC_SET_SEND_CARRIER>`.
    (驱动支持通过 :ref:`ioctl LIRC_SET_SEND_CARRIER <LIRC_SET_SEND_CARRIER>` 更改调制频率。)

.. _LIRC-CAN-SET-SEND-DUTY-CYCLE:

``LIRC_CAN_SET_SEND_DUTY_CYCLE``

    The driver supports changing the duty cycle using
    :ref:`ioctl LIRC_SET_SEND_DUTY_CYCLE <LIRC_SET_SEND_DUTY_CYCLE>`.
    (驱动支持使用 :ref:`ioctl LIRC_SET_SEND_DUTY_CYCLE <LIRC_SET_SEND_DUTY_CYCLE>` 更改占空比。)

.. _LIRC-CAN-SET-TRANSMITTER-MASK:

``LIRC_CAN_SET_TRANSMITTER_MASK``

    The driver supports changing the active transmitter(s) using
    :ref:`ioctl LIRC_SET_TRANSMITTER_MASK <LIRC_SET_TRANSMITTER_MASK>`.
    (驱动支持使用 :ref:`ioctl LIRC_SET_TRANSMITTER_MASK <LIRC_SET_TRANSMITTER_MASK>` 更改活动的发送器。)

.. _LIRC-CAN-SET-REC-CARRIER:

``LIRC_CAN_SET_REC_CARRIER``

    The driver supports setting the receive carrier frequency using
    :ref:`ioctl LIRC_SET_REC_CARRIER <LIRC_SET_REC_CARRIER>`.
    (驱动支持使用 :ref:`ioctl LIRC_SET_REC_CARRIER <LIRC_SET_REC_CARRIER>` 设置接收载波频率。)

.. _LIRC-CAN-SET-REC-CARRIER-RANGE:

``LIRC_CAN_SET_REC_CARRIER_RANGE``

    The driver supports
    :ref:`ioctl LIRC_SET_REC_CARRIER_RANGE <LIRC_SET_REC_CARRIER_RANGE>`.
    (驱动支持 :ref:`ioctl LIRC_SET_REC_CARRIER_RANGE <LIRC_SET_REC_CARRIER_RANGE>` 。)

.. _LIRC-CAN-GET-REC-RESOLUTION:

``LIRC_CAN_GET_REC_RESOLUTION``

    The driver supports
    :ref:`ioctl LIRC_GET_REC_RESOLUTION <LIRC_GET_REC_RESOLUTION>`.
    (驱动支持 :ref:`ioctl LIRC_GET_REC_RESOLUTION <LIRC_GET_REC_RESOLUTION>` 。)

.. _LIRC-CAN-SET-REC-TIMEOUT:

``LIRC_CAN_SET_REC_TIMEOUT``

    The driver supports
    :ref:`ioctl LIRC_SET_REC_TIMEOUT <LIRC_SET_REC_TIMEOUT>`.
    (驱动支持 :ref:`ioctl LIRC_SET_REC_TIMEOUT <LIRC_SET_REC_TIMEOUT>` 。)

.. _LIRC-CAN-MEASURE-CARRIER:

``LIRC_CAN_MEASURE_CARRIER``

    The driver supports measuring of the modulation frequency using
    :ref:`ioctl LIRC_SET_MEASURE_CARRIER_MODE <LIRC_SET_MEASURE_CARRIER_MODE>`.
    (驱动支持使用 :ref:`ioctl LIRC_SET_MEASURE_CARRIER_MODE <LIRC_SET_MEASURE_CARRIER_MODE>` 测量调制频率。)

.. _LIRC-CAN-USE-WIDEBAND-RECEIVER:

``LIRC_CAN_USE_WIDEBAND_RECEIVER``

    The driver supports learning mode using
    :ref:`ioctl LIRC_SET_WIDEBAND_RECEIVER <LIRC_SET_WIDEBAND_RECEIVER>`.
    (驱动支持使用 :ref:`ioctl LIRC_SET_WIDEBAND_RECEIVER <LIRC_SET_WIDEBAND_RECEIVER>` 的学习模式。)

.. _LIRC-CAN-SEND-RAW:

``LIRC_CAN_SEND_RAW``

    Unused. Kept just to avoid breaking uAPI.
    (未使用。保留它只是为了避免破坏 uAPI。)

.. _LIRC-CAN-SEND-PULSE:

``LIRC_CAN_SEND_PULSE``

    The driver supports sending (also called as IR blasting or IR TX) using
    :ref:`LIRC_MODE_PULSE <lirc-mode-pulse>`. This implies that
    :ref:`LIRC_MODE_SCANCODE <lirc-mode-SCANCODE>` is also supported for
    transmit, as long as the kernel is recent enough. Use the
    :ref:`lirc_set_send_mode` to switch modes.
    (驱动支持使用 :ref:`LIRC_MODE_PULSE <lirc-mode-pulse>` 进行发送(也称为 IR blasting 或 IR TX)。这也意味着，只要内核足够新， :ref:`LIRC_MODE_SCANCODE <lirc-mode-SCANCODE>` 也支持发送。使用 :ref:`lirc_set_send_mode` 来切换模式。)

.. _LIRC-CAN-SEND-MODE2:

``LIRC_CAN_SEND_MODE2``

    Unused. Kept just to avoid breaking uAPI.
    :ref:`LIRC_MODE_MODE2 <lirc-mode-mode2>` can only be used for receiving.
    (未使用。保留它只是为了避免破坏 uAPI。 :ref:`LIRC_MODE_MODE2 <lirc-mode-mode2>` 只能用于接收。)

.. _LIRC-CAN-SEND-LIRCCODE:

``LIRC_CAN_SEND_LIRCCODE``

    Unused. Kept just to avoid breaking uAPI.
    (未使用。保留它只是为了避免破坏 uAPI。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
