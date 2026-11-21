.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later

.. _lirc_dev_intro:

******************
Introduction(简介)
******************
LIRC stands for Linux Infrared Remote Control. The LIRC device interface is
a bi-directional interface for transporting raw IR and decoded scancodes
data between userspace and kernelspace. Fundamentally, it is just a chardev
(/dev/lircX, for X = 0, 1, 2, ...), with a number of standard struct
file_operations defined on it. With respect to transporting raw IR and
decoded scancodes to and fro, the essential fops are read, write and ioctl.
(LIRC 是 Linux Infrared Remote Control 的缩写。LIRC 设备接口是一个双向接口，用于在用户空间与内核空间之间传输原始红外 (raw IR) 数据和已解码的扫描码 (scancode) 数据。从根本上说，它只是一个字符设备 (/dev/lircX，其中 X = 0、1、2、...)，其上定义了一些标准的 struct file_operations。就原始红外与已解码扫描码的往返传输而言，最基本的 fops 是 read、write 和 ioctl。)

It is also possible to attach a BPF program to a LIRC device for decoding
raw IR into scancodes.
(还可以将一个 BPF 程序附加到 LIRC 设备上，用于把原始红外数据解码为扫描码。)

Example dmesg output upon a driver registering w/LIRC:
(驱动注册 LIRC 时的 dmesg 输出示例：)

.. code-block:: none

    $ dmesg |grep lirc_dev
    rc rc0: lirc_dev: driver mceusb registered at minor = 0, raw IR receiver, raw IR transmitter

What you should see for a chardev:
(对于一个字符设备，你应该看到的情况如下：)

.. code-block:: none

    $ ls -l /dev/lirc*
    crw-rw---- 1 root root 248, 0 Jul 2 22:20 /dev/lirc0

Note that the package `v4l-utils <https://git.linuxtv.org/v4l-utils.git/>`_
contains tools for working with LIRC devices:
(注意，`v4l-utils <https://git.linuxtv.org/v4l-utils.git/>`_ 软件包包含用于操作 LIRC 设备的工具：)

 - ir-ctl: can receive raw IR and transmit IR, as well as query LIRC
   device features.

 - ir-keytable: can load keymaps; allows you to set IR kernel protocols; load
   BPF IR decoders and test IR decoding. Some BPF IR decoders are also
   provided.

.. _lirc_modes:

*********************
LIRC modes(LIRC 模式)
*********************
LIRC supports some modes of receiving and sending IR codes, as shown
on the following table.
(LIRC 支持若干接收和发送红外码的模式，如下表所示。)

.. _lirc-mode-scancode:
.. _lirc-scancode-flag-toggle:
.. _lirc-scancode-flag-repeat:

``LIRC_MODE_SCANCODE``
(扫描码模式)

    This mode is for both sending and receiving IR.
    (此模式既用于发送也用于接收红外数据。)

    For transmitting (aka sending), create a struct lirc_scancode with
    the desired scancode set in the ``scancode`` member, :c:type:`rc_proto`
    set to the :ref:`IR protocol <Remote_controllers_Protocols>`, and all other
    members set to 0. Write this struct to the lirc device.
    (对于发送 (transmitting，即 sending)，创建一个 struct lirc_scancode，在 ``scancode`` 成员中设置所需的扫描码，将 :c:type:`rc_proto` 设置为 :ref:`IR protocol <Remote_controllers_Protocols>` ，并把所有其他成员设置为 0。然后将这个结构写入 lirc 设备。)

    For receiving, you read struct lirc_scancode from the LIRC device.
    The ``scancode`` field is set to the received scancode and the
    :ref:`IR protocol <Remote_controllers_Protocols>` is set in
    :c:type:`rc_proto`. If the scancode maps to a valid key code, this is set
    in the ``keycode`` field, else it is set to ``KEY_RESERVED``.
    (对于接收，你从 LIRC 设备读取 struct lirc_scancode。 ``scancode`` 字段被设置为接收到的扫描码，而 :ref:`IR protocol <Remote_controllers_Protocols>` 被设置在 :c:type:`rc_proto` 中。如果该扫描码映射到一个有效的键码，则键码会设置在 ``keycode`` 字段中，否则 ``keycode`` 会被设置为 ``KEY_RESERVED`` 。)

    The ``flags`` can have ``LIRC_SCANCODE_FLAG_TOGGLE`` set if the toggle
    bit is set in protocols that support it (e.g. rc-5 and rc-6), or
    ``LIRC_SCANCODE_FLAG_REPEAT`` for when a repeat is received for protocols
    that support it (e.g. nec).
    (如果支持该位的协议(例如 rc-5 和 rc-6)中设置了切换位 (toggle bit)，则 ``flags`` 中会设置 ``LIRC_SCANCODE_FLAG_TOGGLE`` ；对于支持该特性的协议(例如 nec)收到重复 (repeat) 时，则设置 ``LIRC_SCANCODE_FLAG_REPEAT`` 。)

    In the Sanyo and NEC protocol, if you hold a button on remote, rather than
    repeating the entire scancode, the remote sends a shorter message with
    no scancode, which just means button is held, a "repeat". When this is
    received, the ``LIRC_SCANCODE_FLAG_REPEAT`` is set and the scancode and
    keycode is repeated.
    (在 Sanyo 和 NEC 协议中，如果你按住遥控器上的某个按钮，遥控器不会重复整个扫描码，而是发送一条不带扫描码的较短消息，它只表示按钮被按住，即一次 "repeat" (重复)。当收到这样的消息时， ``LIRC_SCANCODE_FLAG_REPEAT`` 会被设置，扫描码和键码沿用之前收到的值。)

    With nec, there is no way to distinguish "button hold" from "repeatedly
    pressing the same button". The rc-5 and rc-6 protocols have a toggle bit.
    When a button is released and pressed again, the toggle bit is inverted.
    If the toggle bit is set, the ``LIRC_SCANCODE_FLAG_TOGGLE`` is set.
    (对于 nec，无法区分 "button hold" (按住按钮) 与 "repeatedly pressing the same button" (反复按同一按钮)。rc-5 和 rc-6 协议有一个切换位。当按钮被释放并再次按下时，切换位会反转。如果切换位被置位，则 ``LIRC_SCANCODE_FLAG_TOGGLE`` 会被设置。)

    The ``timestamp`` field is filled with the time nanoseconds
    (in ``CLOCK_MONOTONIC``) when the scancode was decoded.
    ( ``timestamp`` 字段填充扫描码被解码时的时间，单位为纳秒(基于 ``CLOCK_MONOTONIC`` 时钟)。)

.. _lirc-mode-mode2:

``LIRC_MODE_MODE2``
(模式 2，脉冲/空闲码模式)

    The driver returns a sequence of pulse and space codes to userspace,
    as a series of u32 values.
    (驱动以一系列 u32 值的形式向用户空间返回一串脉冲 (pulse) 与空闲 (space) 码。)

    This mode is used only for IR receive.
    (此模式仅用于红外接收。)

    The upper 8 bits determine the packet type, and the lower 24 bits
    the payload. Use ``LIRC_VALUE()`` macro to get the payload, and
    the macro ``LIRC_MODE2()`` will give you the type, which
    is one of:
    (高 8 位决定数据包类型，低 24 位是负载 (payload)。使用 ``LIRC_VALUE()`` 宏获取负载，宏 ``LIRC_MODE2()`` 会给出类型，它可能是以下之一：)

    ``LIRC_MODE2_PULSE``
    (脉冲)

        Signifies the presence of IR in microseconds, also known as *flash*.
        (表示存在红外信号，以微秒为单位，也称为 *flash* (闪烁)。)

    ``LIRC_MODE2_SPACE``
    (空闲)

        Signifies absence of IR in microseconds, also known as *gap*.
        (表示没有红外信号，以微秒为单位，也称为 *gap* (间隙)。)

    ``LIRC_MODE2_FREQUENCY``
    (载波频率)

        If measurement of the carrier frequency was enabled with
        :ref:`lirc_set_measure_carrier_mode` then this packet gives you
        the carrier frequency in Hertz.
        (如果通过 :ref:`lirc_set_measure_carrier_mode` 启用了载波频率测量，那么此数据包给出以赫兹为单位的载波频率。)

    ``LIRC_MODE2_TIMEOUT``
    (超时)

        When the timeout set with :ref:`lirc_set_rec_timeout` expires due
        to no IR being detected, this packet will be sent, with the number
        of microseconds with no IR.
        (当通过 :ref:`lirc_set_rec_timeout` 设置的超时时间因未检测到红外信号而到期时，将发送此数据包，其中包含没有红外信号的微秒数。)

    ``LIRC_MODE2_OVERFLOW``
    (溢出)

        Signifies that the IR receiver encounter an overflow, and some IR
        is missing. The IR data after this should be correct again. The
        actual value is not important, but this is set to 0xffffff by the
        kernel for compatibility with lircd.
        (表示红外接收器遇到了溢出 (overflow)，部分红外数据丢失。此数据包之后的红外数据应当重新变得正确。实际值并不重要，但为了与 lircd 兼容，内核将其设置为 0xffffff。)

.. _lirc-mode-pulse:

``LIRC_MODE_PULSE``
(脉冲模式)

    In pulse mode, a sequence of pulse/space integer values are written to the
    lirc device using :ref:`lirc-write`.
    (在脉冲模式下，使用 :ref:`lirc-write` 将一串脉冲/空闲整数值写入 lirc 设备。)

    The values are alternating pulse and space lengths, in microseconds. The
    first and last entry must be a pulse, so there must be an odd number
    of entries.
    (这些值交替表示脉冲和空闲的长度，单位为微秒。第一个和最后一个条目必须是脉冲，因此条目数必须是奇数。)

    This mode is used only for IR send.
    (此模式仅用于红外发送。)

************************************************************************
Data types used by LIRC_MODE_SCANCODE(LIRC_MODE_SCANCODE 使用的数据类型)
************************************************************************

.. kernel-doc:: include/uapi/linux/lirc.h
    :identifiers: lirc_scancode rc_proto

********************
BPF based IR decoder
********************

The kernel has support for decoding the most common
:ref:`IR protocols <Remote_controllers_Protocols>`, but there
are many protocols which are not supported. To support these, it is possible
to load an BPF program which does the decoding. This can only be done on
LIRC devices which support reading raw IR.

First, using the `bpf(2)`_ syscall with the ``BPF_LOAD_PROG`` argument,
program must be loaded of type ``BPF_PROG_TYPE_LIRC_MODE2``. Once attached
to the LIRC device, this program will be called for each pulse, space or
timeout event on the LIRC device. The context for the BPF program is a
pointer to a unsigned int, which is a :ref:`LIRC_MODE_MODE2 <lirc-mode-mode2>`
value. When the program has decoded the scancode, it can be submitted using
the BPF functions ``bpf_rc_keydown()`` or ``bpf_rc_repeat()``. Mouse or pointer
movements can be reported using ``bpf_rc_pointer_rel()``.

Once you have the file descriptor for the ``BPF_PROG_TYPE_LIRC_MODE2`` BPF
program, it can be attached to the LIRC device using the `bpf(2)`_ syscall.
The target must be the file descriptor for the LIRC device, and the
attach type must be ``BPF_LIRC_MODE2``. No more than 64 BPF programs can be
attached to a single LIRC device at a time.

.. _bpf(2): http://man7.org/linux/man-pages/man2/bpf.2.html
