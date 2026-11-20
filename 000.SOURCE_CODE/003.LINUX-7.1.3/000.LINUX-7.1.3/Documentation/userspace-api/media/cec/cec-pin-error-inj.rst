.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _cec_pin_error_inj:

CEC Pin Framework Error Injection(CEC Pin 框架错误注入)
=======================================================

The CEC Pin Framework is a core CEC framework for CEC hardware that only
has low-level support for the CEC bus. Most hardware today will have
high-level CEC support where the hardware deals with driving the CEC bus,
but some older devices aren't that fancy. However, this framework also
allows you to connect the CEC pin to a GPIO on e.g. a Raspberry Pi and
you have now made a CEC adapter.
(CEC Pin 框架是一个面向只具备 CEC 总线底层支持的 CEC 硬件的核心 CEC
框架。如今大多数硬件都具有高层 CEC 支持，由硬件负责驱动 CEC 总线，但一
些较老的设备没有这么高级。不过，该框架还允许你把 CEC 引脚连接到例如树
莓派 (Raspberry Pi) 的一个 GPIO 上，这样你就做成一个 CEC 适配器了。)

What makes doing this so interesting is that since we have full control
over the bus it is easy to support error injection. This is ideal to
test how well CEC adapters can handle error conditions.
(这样做之所以有趣，是因为我们完全掌控总线，因此很容易支持错误注入。
这是测试 CEC 适配器处理错误条件能力的理想手段。)

Currently only the cec-gpio driver (when the CEC line is directly
connected to a pull-up GPIO line) and the AllWinner A10/A20 drm driver
support this framework.
(目前只有 cec-gpio 驱动 (当 CEC 线直接连接到上拉 GPIO 线时) 和
AllWinner A10/A20 drm 驱动支持该框架。)

If ``CONFIG_CEC_PIN_ERROR_INJ`` is enabled, then error injection is available
through debugfs. Specifically, in ``/sys/kernel/debug/cec/cecX/`` there is
now an ``error-inj`` file.
(如果启用了 ``CONFIG_CEC_PIN_ERROR_INJ``，就可以通过 debugfs 使用错误
注入。具体来说，``/sys/kernel/debug/cec/cecX/`` 目录下会多出一个
``error-inj`` 文件。)

.. note::

    The error injection commands are not a stable ABI and may change in the
    future.
    (错误注入命令不是稳定的 ABI，将来可能发生变化。)

With ``cat error-inj`` you can see both the possible commands and the current
error injection status::

	$ cat /sys/kernel/debug/cec/cec0/error-inj
	# Clear error injections:
	#   clear          clear all rx and tx error injections
	#   rx-clear       clear all rx error injections
	#   tx-clear       clear all tx error injections
	#   <op> clear     clear all rx and tx error injections for <op>
	#   <op> rx-clear  clear all rx error injections for <op>
	#   <op> tx-clear  clear all tx error injections for <op>
	#
	# RX error injection settings:
	#   rx-no-low-drive                    do not generate low-drive pulses
	#
	# RX error injection:
	#   <op>[,<mode>] rx-nack              NACK the message instead of sending an ACK
	#   <op>[,<mode>] rx-low-drive <bit>   force a low-drive condition at this bit position
	#   <op>[,<mode>] rx-add-byte          add a spurious byte to the received CEC message
	#   <op>[,<mode>] rx-remove-byte       remove the last byte from the received CEC message
	#    any[,<mode>] rx-arb-lost [<poll>] generate a POLL message to trigger an arbitration lost
	#
	# TX error injection settings:
	#   tx-ignore-nack-until-eom           ignore early NACKs until EOM
	#   tx-custom-low-usecs <usecs>        define the 'low' time for the custom pulse
	#   tx-custom-high-usecs <usecs>       define the 'high' time for the custom pulse
	#   tx-custom-pulse                    transmit the custom pulse once the bus is idle
	#   tx-glitch-low-usecs <usecs>        define the 'low' time for the glitch pulse
	#   tx-glitch-high-usecs <usecs>       define the 'high' time for the glitch pulse
	#   tx-glitch-falling-edge             send the glitch pulse after every falling edge
	#   tx-glitch-rising-edge              send the glitch pulse after every rising edge
	#
	# TX error injection:
	#   <op>[,<mode>] tx-no-eom            don't set the EOM bit
	#   <op>[,<mode>] tx-early-eom         set the EOM bit one byte too soon
	#   <op>[,<mode>] tx-add-bytes <num>   append <num> (1-255) spurious bytes to the message
	#   <op>[,<mode>] tx-remove-byte       drop the last byte from the message
	#   <op>[,<mode>] tx-short-bit <bit>   make this bit shorter than allowed
	#   <op>[,<mode>] tx-long-bit <bit>    make this bit longer than allowed
	#   <op>[,<mode>] tx-custom-bit <bit>  send the custom pulse instead of this bit
	#   <op>[,<mode>] tx-short-start       send a start pulse that's too short
	#   <op>[,<mode>] tx-long-start        send a start pulse that's too long
	#   <op>[,<mode>] tx-custom-start      send the custom pulse instead of the start pulse
	#   <op>[,<mode>] tx-last-bit <bit>    stop sending after this bit
	#   <op>[,<mode>] tx-low-drive <bit>   force a low-drive condition at this bit position
	#
	# <op>       CEC message opcode (0-255) or 'any'
	# <mode>     'once' (default), 'always', 'toggle' or 'off'
	# <bit>      CEC message bit (0-159)
	#            10 bits per 'byte': bits 0-7: data, bit 8: EOM, bit 9: ACK
	# <poll>     CEC poll message used to test arbitration lost (0x00-0xff, default 0x0f)
	# <usecs>    microseconds (0-10000000, default 1000)

	clear

(用 ``cat error-inj`` 可以同时看到可用的命令以及当前的错误注入状态。)

You can write error injection commands to ``error-inj`` using
``echo 'cmd' >error-inj`` or ``cat cmd.txt >error-inj``. The ``cat error-inj``
output contains the current error commands. You can save the output to a file
and use it as an input to ``error-inj`` later.
(可以使用 ``echo 'cmd' >error-inj`` 或 ``cat cmd.txt >error-inj`` 向
``error-inj`` 写入错误注入命令。``cat error-inj`` 的输出包含当前的错误
命令。你可以把输出保存到文件，以后再作为 ``error-inj`` 的输入使用。)

Basic Syntax(基本语法)
----------------------

Leading spaces/tabs are ignored. If the next character is a ``#`` or the end
of the line was reached, then the whole line is ignored. Otherwise a command
is expected.
(行首的空格/制表符会被忽略。如果下一个字符是 ``#`` 或者已到达行尾，则
整行都会被忽略。否则将期待一条命令。)

The error injection commands fall in two main groups: those relating to
receiving CEC messages and those relating to transmitting CEC messages. In
addition, there are commands to clear existing error injection commands and
to create custom pulses on the CEC bus.
(错误注入命令分为两大类：与接收 CEC 消息相关的命令，以及与发送 CEC 消
息相关的命令。此外，还有一些用于清除既有错误注入命令、以及在 CEC 总线上
产生自定义脉冲的命令。)

Most error injection commands can be executed for specific CEC opcodes or for
all opcodes (``any``). Each command also has a 'mode' which can be ``off``
(can be used to turn off an existing error injection command), ``once``
(the default) which will trigger the error injection only once for the next
received or transmitted message, ``always`` to always trigger the error
injection and ``toggle`` to toggle the error injection on or off for every
transmit or receive.
(大多数错误注入命令可以针对特定 CEC 操作码执行，也可以针对所有操作码
(``any``)。每条命令还有一个 'mode'，可以是 ``off`` (可用于关闭一条已有
的错误注入命令)、``once`` (默认值，只为下一条收到或发送的消息触发一次
错误注入)、``always`` (总是触发错误注入) 以及 ``toggle`` (在每次发送或
接收时切换错误注入的开或关)。)

So '``any rx-nack``' will NACK the next received CEC message,
'``any,always rx-nack``' will NACK all received CEC messages and
'``0x82,toggle rx-nack``' will only NACK if an Active Source message was
received and do that only for every other received message.
(因此，'``any rx-nack``' 会对下一条收到的 CEC 消息 NACK；'``any,always
rx-nack``' 会对所有收到的 CEC 消息 NACK；而 '``0x82,toggle rx-nack``'
只会在收到 Active Source 消息时 NACK，并且每隔一条消息才执行一次。)

After an error was injected with mode ``once`` the error injection command
is cleared automatically, so ``once`` is a one-time deal.
(以 ``once`` 模式注入一个错误后，该错误注入命令会被自动清除，因此
``once`` 是一次性的。)

All combinations of ``<op>`` and error injection commands can co-exist. So
this is fine::

	0x9e tx-add-bytes 1
	0x9e tx-early-eom
	0x9f tx-add-bytes 2
	any rx-nack

(``<op>`` 与错误注入命令的所有组合都可以共存。所以下面这样是可以的：)

All four error injection commands will be active simultaneously.
(这四条错误注入命令会同时生效。)

However, if the same ``<op>`` and command combination is specified,
but with different arguments::

	0x9e tx-add-bytes 1
	0x9e tx-add-bytes 2

(但是，如果指定的 ``<op>`` 与命令组合相同而参数不同：)

Then the second will overwrite the first.
(则第二条会覆盖第一条。)

Clear Error Injections(清除错误注入)
------------------------------------

``clear``
    Clear all error injections.
    (清除所有错误注入。)

``rx-clear``
    Clear all receive error injections
    (清除所有接收错误注入)

``tx-clear``
    Clear all transmit error injections
    (清除所有发送错误注入)

``<op> clear``
    Clear all error injections for the given opcode.
    (清除给定操作码的所有错误注入。)

``<op> rx-clear``
    Clear all receive error injections for the given opcode.
    (清除给定操作码的所有接收错误注入。)

``<op> tx-clear``
    Clear all transmit error injections for the given opcode.
    (清除给定操作码的所有发送错误注入。)

Receive Messages(接收消息)
--------------------------

``<op>[,<mode>] rx-nack``
    NACK broadcast messages and messages directed to this CEC adapter.
    Every byte of the message will be NACKed in case the transmitter
    keeps transmitting after the first byte was NACKed.
    (对广播消息以及发往本 CEC 适配器的消息进行 NACK。如果发送方在第一
    个字节被 NACK 之后仍继续发送，则消息的每个字节都会被 NACK。)

``<op>[,<mode>] rx-low-drive <bit>``
    Force a Low Drive condition at this bit position. If <op> specifies
    a specific CEC opcode then the bit position must be at least 18,
    otherwise the opcode hasn't been received yet. This tests if the
    transmitter can handle the Low Drive condition correctly and reports
    the error correctly. Note that a Low Drive in the first 4 bits can also
    be interpreted as an Arbitration Lost condition by the transmitter.
    This is implementation dependent.
    (在该位位置强制产生 Low Drive 条件。如果 <op> 指定了具体的 CEC 操作
    码，则位位置必须至少为 18，否则操作码尚未收到。这用于测试发送方能否
    正确处理 Low Drive 条件并正确报告错误。注意，前 4 位中的 Low Drive 也
    可能被发送方解读为仲裁丢失 (Arbitration Lost) 条件，这取决于具体实现。)

``<op>[,<mode>] rx-add-byte``
    Add a spurious 0x55 byte to the received CEC message, provided
    the message was 15 bytes long or less. This is useful to test
    the high-level protocol since spurious bytes should be ignored.
    (向收到的 CEC 消息附加一个多余的 0x55 字节，前提是消息长度不超过 15
    字节。这对于测试高层协议很有用，因为多余的字节应当被忽略。)

``<op>[,<mode>] rx-remove-byte``
    Remove the last byte from the received CEC message, provided it
    was at least 2 bytes long. This is useful to test the high-level
    protocol since messages that are too short should be ignored.
    (从收到的 CEC 消息中移除最后一个字节，前提是消息至少有 2 字节长。
    这对于测试高层协议很有用，因为过短的消息应当被忽略。)

``<op>[,<mode>] rx-arb-lost <poll>``
    Generate a POLL message to trigger an Arbitration Lost condition.
    This command is only allowed for ``<op>`` values of ``next`` or ``all``.
    As soon as a start bit has been received the CEC adapter will switch
    to transmit mode and it will transmit a POLL message. By default this is
    0x0f, but it can also be specified explicitly via the ``<poll>`` argument.
    (产生一条 POLL 消息以触发仲裁丢失 (Arbitration Lost) 条件。该命令只
    允许用于 ``next`` 或 ``all`` 的 ``<op>`` 值。一旦收到起始位，CEC 适配
    器就会切换到发送模式并发送一条 POLL 消息。默认为 0x0f，但也可以通过
    ``<poll>`` 参数显式指定。)

    This command can be used to test the Arbitration Lost condition in
    the remote CEC transmitter. Arbitration happens when two CEC adapters
    start sending a message at the same time. In that case the initiator
    with the most leading zeroes wins and the other transmitter has to
    stop transmitting ('Arbitration Lost'). This is very hard to test,
    except by using this error injection command.
    (该命令可用于测试远端 CEC 发送方的仲裁丢失条件。当两个 CEC 适配器同
    时开始发送消息时就会发生仲裁。此时前导 0 最多的发起者获胜，另一个发
    送方必须停止发送 ("仲裁丢失")。除了使用这条错误注入命令之外，这种情
    况非常难以测试。)

    This does not work if the remote CEC transmitter has logical address
    0 ('TV') since that will always win.
    (如果远端 CEC 发送方的逻辑地址为 0 ('TV')，则此命令不起作用，因为它
    总是会获胜。)

``rx-no-low-drive``
    The receiver will ignore situations that would normally generate a
    Low Drive pulse (3.6 ms). This is typically done if a spurious pulse is
    detected when receiving a message, and it indicates to the transmitter that
    the message has to be retransmitted since the receiver got confused.
    Disabling this is useful to test how other CEC devices handle glitches
    by ensuring we will not be the one that generates a Low Drive.
    (接收方将忽略那些通常会产生 Low Drive 脉冲 (3.6 ms) 的情形。通常这样
    做是因为在接收消息时检测到了多余的脉冲，它向发送方表明消息必须重传，
    因为接收方已经混乱了。禁用此功能有助于测试其他 CEC 设备如何处理毛刺
    (glitch)，因为它确保我们自己不会成为产生 Low Drive 的一方。)

Transmit Messages(发送消息)
---------------------------

``tx-ignore-nack-until-eom``
    This setting changes the behavior of transmitting CEC messages. Normally
    as soon as the receiver NACKs a byte the transmit will stop, but the
    specification also allows that the full message is transmitted and only
    at the end will the transmitter look at the ACK bit. This is not
    recommended behavior since there is no point in keeping the CEC bus busy
    for longer than is strictly needed. Especially given how slow the bus is.
    (该设置改变发送 CEC 消息的行为。通常接收方一旦 NACK 某个字节，发送就
    会停止，但规范也允许发送完整条消息，直到最后发送方才查看 ACK 位。这
    不是推荐的做法，因为让 CEC 总线忙于超过严格必要的时间毫无意义，尤其
    是考虑到总线本身就很慢。)

    This setting can be used to test how well a receiver deals with
    transmitters that ignore NACKs until the very end of the message.
    (该设置可用于测试接收方对那些直到消息末尾才理会 NACK 的发送方的处理
    能力。)

``<op>[,<mode>] tx-no-eom``
    Don't set the EOM bit. Normally the last byte of the message has the EOM
    (End-Of-Message) bit set. With this command the transmit will just stop
    without ever sending an EOM. This can be used to test how a receiver
    handles this case. Normally receivers have a time-out after which
    they will go back to the Idle state.
    (不设置 EOM 位。通常消息的最后一个字节会设置 EOM (End-Of-Message)
    位。使用该命令时，发送将直接停止，从不发送 EOM。可用于测试接收方如何
    处理这种情况。通常接收方设有超时，超时后会回到 Idle 状态。)

``<op>[,<mode>] tx-early-eom``
    Set the EOM bit one byte too soon. This obviously only works for messages
    of two bytes or more. The EOM bit will be set for the second-to-last byte
    and not for the final byte. The receiver should ignore the last byte in
    this case. Since the resulting message is likely to be too short for this
    same reason the whole message is typically ignored. The receiver should be
    in Idle state after the last byte was transmitted.
    (提前一个字节设置 EOM 位。这显然只对两字节或更长的消息有效。EOM 位将
    设置在倒数第二个字节上，而不是最后一个字节上。此时接收方应当忽略最后
    一个字节。由于结果消息很可能因此过短，整条消息通常会被忽略。在最后一
    个字节发送完毕后，接收方应处于 Idle 状态。)

``<op>[,<mode>] tx-add-bytes <num>``
    Append ``<num>`` (1-255) spurious bytes to the message. The extra bytes
    have the value of the byte position in the message. So if you transmit a
    two byte message (e.g. a Get CEC Version message) and add 2 bytes, then
    the full message received by the remote CEC adapter is
    ``0x40 0x9f 0x02 0x03``.
    (向消息附加 ``<num>`` (1-255) 个多余字节。这些额外字节的值等于其在消
    息中的字节位置。因此如果你发送一条两字节消息 (例如 Get CEC Version
    消息) 并附加 2 个字节，那么远端 CEC 适配器收到的完整消息就是
    ``0x40 0x9f 0x02 0x03`` 。)

    This command can be used to test buffer overflows in the receiver. E.g.
    what does it do when it receives more than the maximum message size of 16
    bytes.
    (该命令可用于测试接收方的缓冲区溢出，例如当它收到超过 16 字节这一最
    大消息长度的消息时会怎么做。)

``<op>[,<mode>] tx-remove-byte``
    Drop the last byte from the message, provided the message is at least
    two bytes long. The receiver should ignore messages that are too short.
    (从消息中丢弃最后一个字节，前提是消息至少有两字节长。接收方应当忽略
    过短的消息。)

``<op>[,<mode>] tx-short-bit <bit>``
    Make this bit period shorter than allowed. The bit position cannot be
    an Ack bit.  If <op> specifies a specific CEC opcode then the bit position
    must be at least 18, otherwise the opcode hasn't been received yet.
    Normally the period of a data bit is between 2.05 and 2.75 milliseconds.
    With this command the period of this bit is 1.8 milliseconds, this is
    done by reducing the time the CEC bus is high. This bit period is less
    than is allowed and the receiver should respond with a Low Drive
    condition.
    (使该位的周期短于允许值。位位置不能是 Ack 位。如果 <op> 指定了具体
    的 CEC 操作码，则位位置必须至少为 18，否则操作码尚未收到。通常数据位
    的周期在 2.05 至 2.75 毫秒之间。使用该命令时，该位的周期为 1.8 毫秒，
    这是通过缩短 CEC 总线处于高电平的时间来实现的。这一位周期低于允许值，
    接收方应以 Low Drive 条件作为响应。)

    This command is ignored for 0 bits in bit positions 0 to 3. This is
    because the receiver also looks for an Arbitration Lost condition in
    those first four bits and it is undefined what will happen if it
    sees a too-short 0 bit.
    (对于位位置 0 到 3 上的 0 位，该命令会被忽略。这是因为接收方也会在
    这前四位中寻找仲裁丢失条件，如果它看到过短的 0 位，结果将不可预知。)

``<op>[,<mode>] tx-long-bit <bit>``
    Make this bit period longer than is valid. The bit position cannot be
    an Ack bit.  If <op> specifies a specific CEC opcode then the bit position
    must be at least 18, otherwise the opcode hasn't been received yet.
    Normally the period of a data bit is between 2.05 and 2.75 milliseconds.
    With this command the period of this bit is 2.9 milliseconds, this is
    done by increasing the time the CEC bus is high.
    (使该位的周期长于有效值。位位置不能是 Ack 位。如果 <op> 指定了具体
    的 CEC 操作码，则位位置必须至少为 18，否则操作码尚未收到。通常数据位
    的周期在 2.05 至 2.75 毫秒之间。使用该命令时，该位的周期为 2.9 毫秒，
    这是通过延长 CEC 总线处于高电平的时间来实现的。)

    Even though this bit period is longer than is valid it is undefined what
    a receiver will do. It might just accept it, or it might time out and
    return to Idle state. Unfortunately the CEC specification is silent about
    this.
    (尽管该位周期长于有效值，但接收方会怎么做并无定义。它可能直接接受，
    也可能超时并回到 Idle 状态。遗憾的是 CEC 规范对此只字未提。)

    This command is ignored for 0 bits in bit positions 0 to 3. This is
    because the receiver also looks for an Arbitration Lost condition in
    those first four bits and it is undefined what will happen if it
    sees a too-long 0 bit.
    (对于位位置 0 到 3 上的 0 位，该命令会被忽略。这是因为接收方也会在
    这前四位中寻找仲裁丢失条件，如果它看到过长的 0 位，结果将不可预知。)

``<op>[,<mode>] tx-short-start``
    Make this start bit period shorter than allowed. Normally the period of
    a start bit is between 4.3 and 4.7 milliseconds. With this command the
    period of the start bit is 4.1 milliseconds, this is done by reducing
    the time the CEC bus is high. This start bit period is less than is
    allowed and the receiver should return to Idle state when this is detected.
    (使该起始位的周期短于允许值。通常起始位的周期在 4.3 至 4.7 毫秒之间。
    使用该命令时，起始位的周期为 4.1 毫秒，这是通过缩短 CEC 总线处于高电
    平的时间来实现的。这一起始位周期低于允许值，接收方在检测到后应回到
    Idle 状态。)

``<op>[,<mode>] tx-long-start``
    Make this start bit period longer than is valid. Normally the period of
    a start bit is between 4.3 and 4.7 milliseconds. With this command the
    period of the start bit is 5 milliseconds, this is done by increasing
    the time the CEC bus is high. This start bit period is more than is
    valid and the receiver should return to Idle state when this is detected.
    (使该起始位的周期长于有效值。通常起始位的周期在 4.3 至 4.7 毫秒之间。
    使用该命令时，起始位的周期为 5 毫秒，这是通过延长 CEC 总线处于高电平
    的时间来实现的。这一起始位周期超出有效值，接收方在检测到后应回到 Idle
    状态。)

    Even though this start bit period is longer than is valid it is undefined
    what a receiver will do. It might just accept it, or it might time out and
    return to Idle state. Unfortunately the CEC specification is silent about
    this.
    (尽管该起始位周期长于有效值，但接收方会怎么做并无定义。它可能直接接
    受，也可能超时并回到 Idle 状态。遗憾的是 CEC 规范对此只字未提。)

``<op>[,<mode>] tx-last-bit <bit>``
    Just stop transmitting after this bit.  If <op> specifies a specific CEC
    opcode then the bit position must be at least 18, otherwise the opcode
    hasn't been received yet. This command can be used to test how the receiver
    reacts when a message just suddenly stops. It should time out and go back
    to Idle state.
    (在该位之后直接停止发送。如果 <op> 指定了具体的 CEC 操作码，则位位置
    必须至少为 18，否则操作码尚未收到。该命令可用于测试接收方在消息突然停
    止时的反应。它应当超时并回到 Idle 状态。)

``<op>[,<mode>] tx-low-drive <bit>``
    Force a Low Drive condition at this bit position. If <op> specifies a
    specific CEC opcode then the bit position must be at least 18, otherwise
    the opcode hasn't been received yet. This can be used to test how the
    receiver handles Low Drive conditions. Note that if this happens at bit
    positions 0-3 the receiver can interpret this as an Arbitration Lost
    condition. This is implementation dependent.
    (在该位位置强制产生 Low Drive 条件。如果 <op> 指定了具体的 CEC 操作
    码，则位位置必须至少为 18，否则操作码尚未收到。这可用于测试接收方如
    何处理 Low Drive 条件。注意，如果这发生在位位置 0-3，接收方可能将其解
    读为仲裁丢失条件，这取决于具体实现。)

Custom Pulses(自定义脉冲)
-------------------------

``tx-custom-low-usecs <usecs>``
    This defines the duration in microseconds that the custom pulse pulls
    the CEC line low. The default is 1000 microseconds.
    (定义自定义脉冲把 CEC 线拉低的持续时间，单位为微秒。默认为 1000 微
    秒。)

``tx-custom-high-usecs <usecs>``
    This defines the duration in microseconds that the custom pulse keeps the
    CEC line high (unless another CEC adapter pulls it low in that time).
    The default is 1000 microseconds. The total period of the custom pulse is
    ``tx-custom-low-usecs + tx-custom-high-usecs``.
    (定义自定义脉冲保持 CEC 线为高的持续时间，单位为微秒 (除非在此期间另
    一个 CEC 适配器把它拉低)。默认为 1000 微秒。自定义脉冲的总周期为
    ``tx-custom-low-usecs + tx-custom-high-usecs``。)

``<op>[,<mode>] tx-custom-bit <bit>``
    Send the custom bit instead of a regular data bit. The bit position cannot
    be an Ack bit.  If <op> specifies a specific CEC opcode then the bit
    position must be at least 18, otherwise the opcode hasn't been received yet.
    (发送自定义位而不是常规数据位。位位置不能是 Ack 位。如果 <op> 指定了
    具体的 CEC 操作码，则位位置必须至少为 18，否则操作码尚未收到。)

``<op>[,<mode>] tx-custom-start``
    Send the custom bit instead of a regular start bit.
    (发送自定义位而不是常规起始位。)

``tx-custom-pulse``
    Transmit a single custom pulse as soon as the CEC bus is idle.
    (一旦 CEC 总线空闲，就发送一个单独的自定义脉冲。)

Glitch Pulses(毛刺脉冲)
-----------------------

This emulates what happens if the signal on the CEC line is seeing spurious
pulses. Typically this happens after the falling or rising edge where there
is a short voltage fluctuation that, if the CEC hardware doesn't do
deglitching, can be seen as a spurious pulse and can cause a Low Drive
condition or corrupt data.
(这模拟了 CEC 线上的信号出现多余脉冲时会发生什么。通常这发生在下降沿或
上升沿之后，那里存在短暂的电压波动；如果 CEC 硬件不做去毛刺 (deglitch)
处理，它可能被看作一个多余的脉冲，从而导致 Low Drive 条件或数据损坏。)

``tx-glitch-low-usecs <usecs>``
    This defines the duration in microseconds that the glitch pulse pulls
    the CEC line low. The default is 1 microsecond. The range is 0-100
    microseconds. If 0, then no glitch pulse will be generated.
    (定义毛刺脉冲把 CEC 线拉低的持续时间，单位为微秒。默认为 1 微秒。范
    围是 0-100 微秒。若为 0，则不会产生毛刺脉冲。)

``tx-glitch-high-usecs <usecs>``
    This defines the duration in microseconds that the glitch pulse keeps the
    CEC line high (unless another CEC adapter pulls it low in that time).
    The default is 1 microseconds. The range is 0-100 microseconds. If 0, then
    no glitch pulse will be generated.The total period of the glitch pulse is
    ``tx-custom-low-usecs + tx-custom-high-usecs``.
    (定义毛刺脉冲保持 CEC 线为高的持续时间，单位为微秒 (除非在此期间另
    一个 CEC 适配器把它拉低)。默认为 1 微秒。范围是 0-100 微秒。若为 0，
    则不会产生毛刺脉冲。毛刺脉冲的总周期为
    ``tx-custom-low-usecs + tx-custom-high-usecs``。)

``tx-glitch-falling-edge``
    Send the glitch pulse right after the falling edge.
    (在下降沿之后紧接着发送毛刺脉冲。)

``tx-glitch-rising-edge``
    Send the glitch pulse right after the rising edge.
    (在上升沿之后紧接着发送毛刺脉冲。)
