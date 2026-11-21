.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc-read:

***********************
LIRC read()(LIRC 读取)
***********************

Name(名称)
==========

lirc-read - Read from a LIRC device(lirc-read - 从 LIRC 设备读取)

Synopsis(概要)
==============

.. code-block:: c

    #include <unistd.h>

.. c:function:: ssize_t read( int fd, void *buf, size_t count )

Arguments(参数)
===============

``fd``
    File descriptor returned by ``open()``.
    (由 ``open()`` 返回的文件描述符。)

``buf``
   Buffer to be filled
   (待填充的缓冲区)

``count``
   Max number of bytes to read
   (最多读取的字节数)

Description(描述)
=================

:c:func:`read()` attempts to read up to ``count`` bytes from file
descriptor ``fd`` into the buffer starting at ``buf``.  If ``count`` is zero,
:c:func:`read()` returns zero and has no other results. If ``count``
is greater than ``SSIZE_MAX``, the result is unspecified.
(:c:func:`read()` 尝试从文件描述符 ``fd`` 读取最多 ``count`` 个字节到从
``buf`` 开始的缓冲区中。如果 ``count`` 为零，:c:func:`read()` 返回零且没有
其他结果。如果 ``count`` 大于 ``SSIZE_MAX``，则结果未定义。)

The exact format of the data depends on what :ref:`lirc_modes` a driver
uses. Use :ref:`lirc_get_features` to get the supported mode, and use
:ref:`lirc_set_rec_mode` set the current active mode.
(数据的具体格式取决于驱动使用哪种 :ref:`lirc_modes`。使用
:ref:`lirc_get_features` 获取支持的模式，使用 :ref:`lirc_set_rec_mode`
设置当前活动模式。)

The mode :ref:`LIRC_MODE_MODE2 <lirc-mode-mode2>` is for raw IR,
in which packets containing an unsigned int value describing an IR signal are
read from the chardev.
(:ref:`LIRC_MODE_MODE2 <lirc-mode-mode2>` 模式用于原始 IR，此模式下会从
字符设备读取包含描述 IR 信号的无符号整数值的数据包。)

Alternatively, :ref:`LIRC_MODE_SCANCODE <lirc-mode-scancode>` can be available,
in this mode scancodes which are either decoded by software decoders, or
by hardware decoders. The :c:type:`rc_proto` member is set to the
:ref:`IR protocol <Remote_controllers_Protocols>`
used for transmission, and ``scancode`` to the decoded scancode,
and the ``keycode`` set to the keycode or ``KEY_RESERVED``.
(此外也可以使用 :ref:`LIRC_MODE_SCANCODE <lirc-mode-scancode>` 模式，此模式
下的扫描码由软件解码器或硬件解码器解码。:c:type:`rc_proto` 成员被设置为
传输所用的 :ref:`IR protocol <Remote_controllers_Protocols>` ，
``scancode`` 为解码得到的扫描码，``keycode`` 为按键码或
``KEY_RESERVED``。)

Return Value(返回值)
====================

On success, the number of bytes read is returned. It is not an error if
this number is smaller than the number of bytes requested, or the amount
of data required for one frame.  On error, -1 is returned, and the ``errno``
variable is set appropriately.
(成功时返回读取的字节数。该数字小于请求的字节数或一帧所需的数据量并不算
错误。出错时返回 -1，并相应设置 ``errno`` 变量。)
