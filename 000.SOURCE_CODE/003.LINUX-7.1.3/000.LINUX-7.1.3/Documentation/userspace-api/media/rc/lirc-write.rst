.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc-write:

************
LIRC write()
************

Name(名称)
==========

lirc-write - Write to a LIRC device
(lirc-write - 写入一个 LIRC 设备。)

Synopsis(概要)
==============

.. code-block:: c

    #include <unistd.h>

.. c:function:: ssize_t write( int fd, void *buf, size_t count )

Arguments(参数)
===============

``fd``
    File descriptor returned by ``open()``.
    (由 ``open()`` 返回的文件描述符。)

``buf``
    Buffer with data to be written
    (包含待写入数据的缓冲区。)

``count``
    Number of bytes at the buffer
    (缓冲区中的字节数。)

Description(描述)
=================

:c:func:`write()` writes up to ``count`` bytes to the device
referenced by the file descriptor ``fd`` from the buffer starting at
``buf``.
( :c:func:`write()` 把最多 ``count`` 字节的数据，从 ``buf`` 起始的缓冲
区写入 ``fd`` 所引用的设备。)

The exact format of the data depends on what mode a driver is in, use
:ref:`lirc_get_features` to get the supported modes and use
:ref:`lirc_set_send_mode` set the mode.
(数据的确切格式取决于驱动程序所处的模式；使用 :ref:`lirc_get_features`
获取支持的模式，并使用 :ref:`lirc_set_send_mode` 设置模式。)

When in :ref:`LIRC_MODE_PULSE <lirc-mode-PULSE>` mode, the data written to
the chardev is a pulse/space sequence of integer values. Pulses and spaces
are only marked implicitly by their position. The data must start and end
with a pulse, therefore, the data must always include an uneven number of
samples. The write function blocks until the data has been transmitted
by the hardware. If more data is provided than the hardware can send, the
driver returns ``EINVAL``.
(在 :ref:`LIRC_MODE_PULSE <lirc-mode-PULSE>` 模式下，写入字符设备的数
据是一个整数值的脉冲/空隙 (pulse/space) 序列。脉冲与空隙只由其位置隐
式标记。数据必须以脉冲开始并以脉冲结束，因此数据必须始终包含奇数个
样本。write 函数会阻塞，直到数据已被硬件发送。如果提供的数据多于硬件
能发送的量，驱动程序返回 ``EINVAL``。)

When in :ref:`LIRC_MODE_SCANCODE <lirc-mode-scancode>` mode, one
``struct lirc_scancode`` must be written to the chardev at a time, else
``EINVAL`` is returned. Set the desired scancode in the ``scancode`` member,
and the :ref:`IR protocol <Remote_controllers_Protocols>` in the
:c:type:`rc_proto`: member. All other members must be
set to 0, else ``EINVAL`` is returned. If there is no protocol encoder
for the protocol or the scancode is not valid for the specified protocol,
``EINVAL`` is returned. The write function blocks until the scancode
is transmitted by the hardware.
(在 :ref:`LIRC_MODE_SCANCODE <lirc-mode-scancode>` 模式下，一次只能向
字符设备写入一个 ``struct lirc_scancode``，否则返回 ``EINVAL``。把所
需的扫描码设置到 ``scancode`` 成员中，并把
:ref:`IR protocol <Remote_controllers_Protocols>` 设置到
:c:type:`rc_proto`: 成员中。所有其他成员必须设置为 0，否则返回
``EINVAL``。如果没有该协议的编码器，或扫描码对指定的协议无效，则返回
``EINVAL``。write 函数会阻塞，直到扫描码被硬件发送。)

Return Value(返回值)
====================

On success, the number of bytes written is returned. It is not an error if
this number is smaller than the number of bytes requested, or the amount
of data required for one frame.  On error, -1 is returned, and the ``errno``
variable is set appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回写入的字节数。该数字小于所请求的字节数或一帧所需的数据量
并不算错误。出错时返回 -1，并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)
