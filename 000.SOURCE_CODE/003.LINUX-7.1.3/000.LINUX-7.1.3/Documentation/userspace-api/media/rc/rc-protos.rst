.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later

.. _Remote_controllers_Protocols:

*************************************************************
Remote Controller Protocols and Scancodes(遥控器协议与扫描码)
*************************************************************

IR is encoded as a series of pulses and spaces, using a protocol. These
protocols can encode e.g. an address (which device should respond) and a
command: what it should do. The values for these are not always consistent
across different devices for a given protocol.
(IR 使用某种协议把信息编码为一系列脉冲与空隙。这些协议可以编码诸如地址(哪个设备应当响应)与命令(它应当做什么)之类的信息。对于给定协议，这些取值在不同设备之间并不总是一致。)

Therefore out the output of the IR decoder is a scancode; a single u32
value. Using keymap tables this can be mapped to linux key codes.
(因此，IR 解码器的输出是一个 scancode(扫描码)——单个 u32 值。借助键表(keymap)可以把它映射为 Linux 按键码。)

Other things can be encoded too. Some IR protocols encode a toggle bit; this
is to distinguish whether the same button is being held down, or has been
released and pressed again. If has been released and pressed again, the
toggle bit will invert from one IR message to the next.
(也可以编码其他内容。某些 IR 协议会编码一个切换位(toggle bit)，用于区分是按住同一个按钮不放，还是松开后再次按下。如果是松开后再次按下，toggle bit 会在相邻两条 IR 消息之间反转。)

Some remotes have a pointer-type device which can used to control the
mouse; some air conditioning systems can have their target temperature
target set in IR.
(某些遥控器带有可用于控制鼠标的指针型设备；某些空调系统可以通过 IR 设置目标温度。)

The following are the protocols the kernel knows about and also lists
how scancodes are encoded for each protocol.
(下面列出内核所知悉的协议，并给出每个协议 scancode 的编码方式。)

rc-5 (RC_PROTO_RC5)(rc-5 协议)
------------------------------

This IR protocol uses manchester encoding to encode 14 bits. There is a
detailed description here https://www.sbprojects.net/knowledge/ir/rc5.php.
(该 IR 协议使用曼彻斯特编码对 14 个比特编码。详细描述见 https://www.sbprojects.net/knowledge/ir/rc5.php。)

The scancode encoding is *not* consistent with the lirc daemon (lircd) rc5
protocol, or the manchester BPF decoder.
(该 scancode 编码与 lirc 守护进程(lircd)的 rc5 协议以及 manchester BPF 解码器并不一致。)

.. flat-table:: rc5 bits scancode mapping
   :widths:       1 1 2

   * - rc-5 bit(rc-5 位)

     - scancode bit(scancode 位)

     - description(描述)

   * - 1

     - none

     - Start bit, always set(起始位，始终置位)

   * - 1

     - 6 (inverted)

     - 2nd start bit in rc5,  re-used as 6th command bit(rc5 中的第 2 个起始位，被复用为第 6 个命令位)

   * - 1

     - none

     - Toggle bit(切换位)

   * - 5

     - 8 to 13

     - Address(地址)

   * - 6

     - 0 to 5

     - Command(命令)

There is a variant of rc5 called either rc5x or extended rc5
where there the second stop bit is the 6th command bit, but inverted.
This is done so it the scancodes and encoding is compatible with existing
schemes. This bit is stored in bit 6 of the scancode, inverted. This is
done to keep it compatible with plain rc-5 where there are two start bits.
(rc5 有一个变体，称为 rc5x 或扩展 rc5(extended rc5)：其第二个停止位就是第 6 个命令位，但取反。这样做是为了让 scancode 与编码方式兼容既有方案。该比特取反后存储在 scancode 的第 6 位，这是为了与拥有两个起始位的普通 rc-5 保持兼容。)

rc-5-sz (RC_PROTO_RC5_SZ)(rc-5-sz 协议)
---------------------------------------
This is much like rc-5 but one bit longer. The scancode is encoded
differently.
(这与 rc-5 非常相似，但长出一个比特。scancode 的编码方式不同。)

.. flat-table:: rc-5-sz bits scancode mapping
   :widths:       1 1 2

   * - rc-5-sz bits(rc-5-sz 位)

     - scancode bit(scancode 位)

     - description(描述)

   * - 1

     - none

     - Start bit, always set(起始位，始终置位)

   * - 1

     - 13

     - Address bit(地址位)

   * - 1

     - none

     - Toggle bit(切换位)

   * - 6

     - 6 to 11

     - Address(地址)

   * - 6

     - 0 to 5

     - Command(命令)

rc-5x-20 (RC_PROTO_RC5X_20)(rc-5x-20 协议)
------------------------------------------

This rc-5 extended to encoded 20 bits. The is a 3555 microseconds space
after the 8th bit.
(这是扩展到 20 比特编码的 rc-5。在第 8 个比特之后有一个 3555 微秒的空隙。)

.. flat-table:: rc-5x-20 bits scancode mapping
   :widths:       1 1 2

   * - rc-5-sz bits(rc-5-sz 位)

     - scancode bit(scancode 位)

     - description(描述)

   * - 1

     - none

     - Start bit, always set(起始位，始终置位)

   * - 1

     - 14

     - Address bit(地址位)

   * - 1

     - none

     - Toggle bit(切换位)

   * - 5

     - 16 to 20

     - Address(地址)

   * - 6

     - 8 to 13

     - Address(地址)

   * - 6

     - 0 to 5

     - Command(命令)


jvc (RC_PROTO_JVC)(jvc 协议)
----------------------------

The jvc protocol is much like nec, without the inverted values. It is
described here https://www.sbprojects.net/knowledge/ir/jvc.php.
(jvc 协议与 nec 非常相似，只是没有取反的值。其描述见 https://www.sbprojects.net/knowledge/ir/jvc.php。)

The scancode is a 16 bits value, where the address is the lower 8 bits
and the command the higher 8 bits; this is reversed from IR order.
(scancode 是一个 16 位的值：地址在低 8 位，命令在高 8 位；这与 IR 中的顺序相反。)

sony-12 (RC_PROTO_SONY12)(sony-12 协议)
---------------------------------------

The sony protocol is a pulse-width encoding. There are three variants,
which just differ in number of bits and scancode encoding.
(sony 协议是脉冲宽度编码。它有三个变体，仅在比特数与 scancode 编码上有所不同。)

.. flat-table:: sony-12 bits scancode mapping
   :widths:       1 1 2

   * - sony-12 bits(sony-12 位)

     - scancode bit(scancode 位)

     - description(描述)

   * - 5

     - 16 to 20

     - device(设备)

   * - 7

     - 0 to 6

     - function(功能)

sony-15 (RC_PROTO_SONY15)(sony-15 协议)
---------------------------------------

The sony protocol is a pulse-width encoding. There are three variants,
which just differ in number of bits and scancode encoding.
(sony 协议是脉冲宽度编码。它有三个变体，仅在比特数与 scancode 编码上有所不同。)

.. flat-table:: sony-12 bits scancode mapping
   :widths:       1 1 2

   * - sony-12 bits(sony-12 位)

     - scancode bit(scancode 位)

     - description(描述)

   * - 8

     - 16 to 23

     - device(设备)

   * - 7

     - 0 to 6

     - function(功能)

sony-20 (RC_PROTO_SONY20)(sony-20 协议)
---------------------------------------

The sony protocol is a pulse-width encoding. There are three variants,
which just differ in number of bits and scancode encoding.
(sony 协议是脉冲宽度编码。它有三个变体，仅在比特数与 scancode 编码上有所不同。)

.. flat-table:: sony-20 bits scancode mapping
   :widths:       1 1 2

   * - sony-20 bits(sony-20 位)

     - scancode bit(scancode 位)

     - description(描述)

   * - 5

     - 16 to 20

     - device(设备)

   * - 7

     - 0 to 7

     - device(设备)

   * - 8

     - 8 to 15

     - extended bits(扩展位)

nec (RC_PROTO_NEC)(nec 协议)
----------------------------

The nec protocol encodes an 8 bit address and an 8 bit command. It is
described here https://www.sbprojects.net/knowledge/ir/nec.php. Note
that the protocol sends least significant bit first.
(nec 协议对 8 位地址与 8 位命令进行编码。其描述见 https://www.sbprojects.net/knowledge/ir/nec.php。注意，该协议先发送最低有效位。)

As a check, the nec protocol sends the address and command twice; the
second time it is inverted. This is done for verification.
(作为校验，nec 协议把地址与命令各发送两次；第二次为取反值。这样做是为了校验。)

A plain nec IR message has 16 bits; the high 8 bits are the address
and the low 8 bits are the command.
(一条普通 nec IR 消息共 16 位：高 8 位是地址，低 8 位是命令。)

nec-x (RC_PROTO_NECX)(nec-x 协议)
---------------------------------

Extended nec has a 16 bit address and a 8 bit command. This is encoded
as a 24 bit value as you would expect, with the lower 8 bits the command
and the upper 16 bits the address.
(扩展 nec 具有 16 位地址与 8 位命令。它按预期编码为 24 位的值：低 8 位是命令，高 16 位是地址。)

nec-32 (RC_PROTO_NEC32)(nec-32 协议)
------------------------------------

nec-32 does not send an inverted address or an inverted command; the
entire message, all 32 bits, are used.
(nec-32 不发送取反的地址或取反的命令；整条消息的全部 32 位都会被使用。)

For this to be decoded correctly, the second 8 bits must not be the
inverted value of the first, and also the last 8 bits must not be the
inverted value of the third 8 bit value.
(要正确解码，第二个 8 位不得是第一个 8 位的取反值，且最后 8 位也不得是第三个 8 位值的取反值。)

The scancode has a somewhat unusual encoding.
(scancode 的编码方式有点不寻常。)

.. flat-table:: nec-32 bits scancode mapping

   * - nec-32 bits(nec-32 位)

     - scancode bit(scancode 位)

   * - First 8 bits(前 8 位)

     - 16 to 23

   * - Second 8 bits(第二个 8 位)

     - 24 to 31

   * - Third 8 bits(第三个 8 位)

     - 0 to 7

   * - Fourth 8 bits(第四个 8 位)

     - 8 to 15

sanyo (RC_PROTO_SANYO)(sanyo 协议)
----------------------------------

The sanyo protocol is like the nec protocol, but with 13 bits address
rather than 8 bits. Both the address and the command are followed by
their inverted versions, but these are not present in the scancodes.
(sanyo 协议类似于 nec 协议，但地址为 13 位而非 8 位。地址与命令之后都跟随其取反版本，但这些取反值并不出现在 scancode 中。)

Bis 8 to 20 of the scancode is the 13 bits address, and the lower 8
bits are the command.
(scancode 的第 8 到 20 位是 13 位地址，低 8 位是命令。)

mcir2-kbd (RC_PROTO_MCIR2_KBD)(mcir2-kbd 协议)
----------------------------------------------

This protocol is generated by the Microsoft MCE keyboard for keyboard
events. Refer to the ir-mce_kbd-decoder.c to see how it is encoded.
(该协议由 Microsoft MCE 键盘为键盘事件产生。其编码方式参见 ir-mce_kbd-decoder.c。)

mcir2-mse (RC_PROTO_MCIR2_MSE)(mcir2-mse 协议)
----------------------------------------------

This protocol is generated by the Microsoft MCE keyboard for pointer
events. Refer to the ir-mce_kbd-decoder.c to see how it is encoded.
(该协议由 Microsoft MCE 键盘为指针事件产生。其编码方式参见 ir-mce_kbd-decoder.c。)

rc-6-0 (RC_PROTO_RC6_0)(rc-6-0 协议)
------------------------------------

This is the rc-6 in mode 0. rc-6 is described here
https://www.sbprojects.net/knowledge/ir/rc6.php.
The scancode is the exact 16 bits as in the protocol. There is also a
toggle bit.
(这是 mode 0 下的 rc-6。rc-6 的描述见 https://www.sbprojects.net/knowledge/ir/rc6.php。scancode 就是协议中确切的 16 位。此外还有一个切换位。)

rc-6-6a-20 (RC_PROTO_RC6_6A_20)(rc-6-6a-20 协议)
------------------------------------------------

This is the rc-6 in mode 6a, 20 bits. rc-6 is described here
https://www.sbprojects.net/knowledge/ir/rc6.php.
The scancode is the exact 20 bits
as in the protocol. There is also a toggle bit.
(这是 mode 6a、20 位的 rc-6。rc-6 的描述见 https://www.sbprojects.net/knowledge/ir/rc6.php。scancode 就是协议中确切的 20 位。此外还有一个切换位。)

rc-6-6a-24 (RC_PROTO_RC6_6A_24)(rc-6-6a-24 协议)
------------------------------------------------

This is the rc-6 in mode 6a, 24 bits. rc-6 is described here
https://www.sbprojects.net/knowledge/ir/rc6.php.
The scancode is the exact 24 bits
as in the protocol. There is also a toggle bit.
(这是 mode 6a、24 位的 rc-6。rc-6 的描述见 https://www.sbprojects.net/knowledge/ir/rc6.php。scancode 就是协议中确切的 24 位。此外还有一个切换位。)

rc-6-6a-32 (RC_PROTO_RC6_6A_32)(rc-6-6a-32 协议)
------------------------------------------------

This is the rc-6 in mode 6a, 32 bits. rc-6 is described here
https://www.sbprojects.net/knowledge/ir/rc6.php.
The upper 16 bits are the vendor,
and the lower 16 bits are the vendor-specific bits. This protocol is
for the non-Microsoft MCE variant (vendor != 0x800f).
(这是 mode 6a、32 位的 rc-6。rc-6 的描述见 https://www.sbprojects.net/knowledge/ir/rc6.php。高 16 位是厂商 ID，低 16 位是厂商自定义位。该协议用于非 Microsoft MCE 变体(vendor != 0x800f)。)


rc-6-mce (RC_PROTO_RC6_MCE)(rc-6-mce 协议)
------------------------------------------

This is the rc-6 in mode 6a, 32 bits. The upper 16 bits are the vendor,
and the lower 16 bits are the vendor-specific bits. This protocol is
for the Microsoft MCE variant (vendor = 0x800f). The toggle bit in the
protocol itself is ignored, and the 16th bit should be takes as the toggle
bit.
(这是 mode 6a、32 位的 rc-6。高 16 位是厂商 ID，低 16 位是厂商自定义位。该协议用于 Microsoft MCE 变体(vendor = 0x800f)。协议本身的切换位被忽略，第 16 位应被当作切换位。)

sharp (RC_PROTO_SHARP)(sharp 协议)
----------------------------------

This is a protocol used by Sharp VCRs, is described here
https://www.sbprojects.net/knowledge/ir/sharp.php. There is a very long
(40ms) space between the normal and inverted values, and some IR receivers
cannot decode this.
(这是 Sharp 录像机使用的协议，描述见 https://www.sbprojects.net/knowledge/ir/sharp.php。正常值与取反值之间有一个很长的空隙(40ms)，某些 IR 接收器无法解码这种情况。)

There is a 5 bit address and a 8 bit command. In the scancode the address is
in bits 8 to 12, and the command in bits 0 to 7.
(地址为 5 位，命令为 8 位。在 scancode 中，地址位于第 8 到 12 位，命令位于第 0 到 7 位。)

xmp (RC_PROTO_XMP)(xmp 协议)
----------------------------

This protocol has several versions and only version 1 is supported. Refer
to the decoder (ir-xmp-decoder.c) to see how it is encoded.
(该协议有多个版本，仅支持版本 1。其编码方式参见解码器(ir-xmp-decoder.c)。)


cec (RC_PROTO_CEC)(cec 协议)
----------------------------

This is not an IR protocol, this is a protocol over CEC. The CEC
infrastructure uses rc-core for handling CEC commands, so that they
can easily be remapped.
(这不是 IR 协议，而是 CEC 之上的协议。CEC 基础设施使用 rc-core 来处理 CEC 命令，以便能轻松地重新映射。)

imon (RC_PROTO_IMON)(imon 协议)
-------------------------------

This protocol is used by Antec Veris/SoundGraph iMON remotes.
(该协议由 Antec Veris/SoundGraph iMON 遥控器使用。)

The protocol
describes both button presses and pointer movements. The protocol encodes
31 bits, and the scancode is simply the 31 bits with the top bit always 0.
(该协议同时描述按钮按下与指针移动。协议编码 31 位，scancode 就是这 31 位，最高位恒为 0。)

rc-mm-12 (RC_PROTO_RCMM12)(rc-mm-12 协议)
-----------------------------------------

The rc-mm protocol is described here
https://www.sbprojects.net/knowledge/ir/rcmm.php. The scancode is simply
the 12 bits.
(rc-mm 协议的描述见 https://www.sbprojects.net/knowledge/ir/rcmm.php。scancode 就是那 12 位。)

rc-mm-24 (RC_PROTO_RCMM24)(rc-mm-24 协议)
-----------------------------------------

The rc-mm protocol is described here
https://www.sbprojects.net/knowledge/ir/rcmm.php. The scancode is simply
the 24 bits.
(rc-mm 协议的描述见 https://www.sbprojects.net/knowledge/ir/rcmm.php。scancode 就是那 24 位。)

rc-mm-32 (RC_PROTO_RCMM32)(rc-mm-32 协议)
-----------------------------------------

The rc-mm protocol is described here
https://www.sbprojects.net/knowledge/ir/rcmm.php. The scancode is simply
the 32 bits.
(rc-mm 协议的描述见 https://www.sbprojects.net/knowledge/ir/rcmm.php。scancode 就是那 32 位。)

xbox-dvd (RC_PROTO_XBOX_DVD)(xbox-dvd 协议)
-------------------------------------------

This protocol is used by Xbox DVD Remote, which was made for the original
Xbox. There is no in-kernel decoder or encoder for this protocol. The usb
device decodes the protocol. There is a BPF decoder available in v4l-utils.
(该协议由为初代 Xbox 制造的 Xbox DVD Remote 使用。内核中没有针对该协议的解码器或编码器；USB 设备负责解码该协议。v4l-utils 中提供一个 BPF 解码器。)
