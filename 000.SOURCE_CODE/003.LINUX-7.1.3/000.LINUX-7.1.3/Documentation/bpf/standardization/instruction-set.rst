.. contents::
.. sectnum::

============================================================
BPF Instruction Set Architecture (ISA)(BPF 指令集架构 (ISA))
============================================================

eBPF, also commonly
referred to as BPF, is a technology with origins in the Linux kernel
that can run untrusted programs in a privileged context such as an
operating system kernel. This document specifies the BPF instruction
set architecture (ISA).
(eBPF 通常也简称 BPF，是一项起源于 Linux 内核的技术，能够在操作系统内核这类特权上下文中运行不受信任的程序。本文档规定 BPF 指令集架构 (ISA)。)

As a historical note, BPF originally stood for Berkeley Packet Filter,
but now that it can do so much more than packet filtering, the acronym
no longer makes sense. BPF is now considered a standalone term that
does not stand for anything.  The original BPF is sometimes referred to
as cBPF (classic BPF) to distinguish it from the now widely deployed
eBPF (extended BPF).
(作为历史注脚，BPF 最初是 Berkeley Packet Filter (伯克利数据包过滤器) 的缩写，但如今它能做的事情远不止包过滤，这个缩写已不再贴切。BPF 现在被认为是一个独立术语，不代表任何东西。最初的 BPF 有时被称为 cBPF (classic BPF，经典 BPF)，以区别于如今广泛部署的 eBPF (extended BPF，扩展 BPF)。)

Documentation conventions(文档约定)
===================================

The key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT",
"SHOULD", "SHOULD NOT", "RECOMMENDED", "NOT RECOMMENDED", "MAY", and
"OPTIONAL" in this document are to be interpreted as described in
BCP 14 `<https://www.rfc-editor.org/info/rfc2119>`_
`<https://www.rfc-editor.org/info/rfc8174>`_
when, and only when, they appear in all capitals, as shown here.
(本文档中的关键词 "MUST"、"MUST NOT"、"REQUIRED"、"SHALL"、"SHALL NOT"、"SHOULD"、"SHOULD NOT"、"RECOMMENDED"、"NOT RECOMMENDED"、"MAY" 和 "OPTIONAL"，当且仅当它们以全大写形式出现时 (如上所示)，按 BCP 14 `<https://www.rfc-editor.org/info/rfc2119>`_
`<https://www.rfc-editor.org/info/rfc8174>`_ 所描述的方式解释。)

For brevity and consistency, this document refers to families
of types using a shorthand syntax and refers to several expository,
mnemonic functions when describing the semantics of instructions.
The range of valid values for those types and the semantics of those
functions are defined in the following subsections.
(为简洁与一致起见，本文档使用一种简写语法来指代类型家族，并在描述指令语义时使用若干说明性、助记性的函数。这些类型的有效取值范围与这些函数的语义在下面几小节中定义。)

Types(类型)
-----------
This document refers to integer types with the notation `SN` to specify
a type's signedness (`S`) and bit width (`N`), respectively.
(本文档使用记法 `SN` 来指代整数类型，分别表示类型的符号性 (`S`) 与位宽 (`N`)。)

.. table:: Meaning of signedness notation

  ==== =========
  S    Meaning
  ==== =========
  u    unsigned
  s    signed
  ==== =========

(上表给出符号记法的含义：u 表示无符号，s 表示有符号。)

.. table:: Meaning of bit-width notation

  ===== =========
  N     Bit width
  ===== =========
  8     8 bits
  16    16 bits
  32    32 bits
  64    64 bits
  128   128 bits
  ===== =========

(上表给出位宽记法的含义：N 表示位宽，取值 8/16/32/64/128 位。)

For example, `u32` is a type whose valid values are all the 32-bit unsigned
numbers and `s16` is a type whose valid values are all the 16-bit signed
numbers.
(例如，`u32` 是有效取值为全部 32 位无符号数的类型，`s16` 是有效取值为全部 16 位有符号数的类型。)

Functions(函数)
---------------

The following byteswap functions are direction-agnostic.  That is,
the same function is used for conversion in either direction discussed
below.
(下列字节交换函数与方向无关。也就是说，下文讨论的任一方向的转换都使用同一个函数。)

* be16: Takes an unsigned 16-bit number and converts it between
  host byte order and big-endian
  (`IEN137 <https://www.rfc-editor.org/ien/ien137.txt>`_) byte order.
  (be16：接受一个 16 位无符号数，在主机字节序与大端 (`IEN137 <https://www.rfc-editor.org/ien/ien137.txt>`_) 字节序之间转换。)
* be32: Takes an unsigned 32-bit number and converts it between
  host byte order and big-endian byte order.
  (be32：接受一个 32 位无符号数，在主机字节序与大端字节序之间转换。)
* be64: Takes an unsigned 64-bit number and converts it between
  host byte order and big-endian byte order.
  (be64：接受一个 64 位无符号数，在主机字节序与大端字节序之间转换。)
* bswap16: Takes an unsigned 16-bit number in either big- or little-endian
  format and returns the equivalent number with the same bit width but
  opposite endianness.
  (bswap16：接受大端或小端格式的 16 位无符号数，返回位宽相同但字节序相反的等值数。)
* bswap32: Takes an unsigned 32-bit number in either big- or little-endian
  format and returns the equivalent number with the same bit width but
  opposite endianness.
  (bswap32：接受大端或小端格式的 32 位无符号数，返回位宽相同但字节序相反的等值数。)
* bswap64: Takes an unsigned 64-bit number in either big- or little-endian
  format and returns the equivalent number with the same bit width but
  opposite endianness.
  (bswap64：接受大端或小端格式的 64 位无符号数，返回位宽相同但字节序相反的等值数。)
* le16: Takes an unsigned 16-bit number and converts it between
  host byte order and little-endian byte order.
  (le16：接受一个 16 位无符号数，在主机字节序与小端字节序之间转换。)
* le32: Takes an unsigned 32-bit number and converts it between
  host byte order and little-endian byte order.
  (le32：接受一个 32 位无符号数，在主机字节序与小端字节序之间转换。)
* le64: Takes an unsigned 64-bit number and converts it between
  host byte order and little-endian byte order.
  (le64：接受一个 64 位无符号数，在主机字节序与小端字节序之间转换。)

Definitions(定义)
-----------------

.. glossary::

  Sign Extend
    To `sign extend an` ``X`` `-bit number, A, to a` ``Y`` `-bit number, B  ,` means to
    (把一个 ``X`` 位的数 A 符号扩展 (sign extend) 为一个 ``Y`` 位的数 B，指的是：)

    #. Copy all ``X`` bits from `A` to the lower ``X`` bits of `B`.
       (把 A 的全部 ``X`` 个位复制到 B 的低 ``X`` 位。)
    #. Set the value of the remaining ``Y`` - ``X`` bits of `B` to the value of
       the  most-significant bit of `A`.
       (把 B 其余的 ``Y`` - ``X`` 位设置为 A 的最高有效位的值。)

.. admonition:: Example

  Sign extend an 8-bit number ``A`` to a 16-bit number ``B`` on a big-endian platform:
  ::

    A:          10000110
    B: 11111111 10000110

  (在大端平台上把 8 位数 ``A`` 符号扩展为 16 位数 ``B``：)

Conformance groups(一致性组)
----------------------------

An implementation does not need to support all instructions specified in this
document (e.g., deprecated instructions).  Instead, a number of conformance
groups are specified.  An implementation MUST support the base32 conformance
group and MAY support additional conformance groups, where supporting a
conformance group means it MUST support all instructions in that conformance
group.
(实现不必支持本文档规定的全部指令 (例如已弃用的指令)。取而代之的是，本文档规定了若干一致性组 (conformance group)。实现 MUST 支持 base32 一致性组，MAY 支持额外的一致性组；支持某个一致性组意味着它 MUST 支持该组中的全部指令。)

The use of named conformance groups enables interoperability between a runtime
that executes instructions, and tools such as compilers that generate
instructions for the runtime.  Thus, capability discovery in terms of
conformance groups might be done manually by users or automatically by tools.
(使用具名一致性组可以在执行指令的运行时与生成指令的编译器等工具之间实现互操作。因此，以一致性组为单位的 Capability 发掘可以由用户手动完成，也可以由工具自动完成。)

Each conformance group has a short ASCII label (e.g., "base32") that
corresponds to a set of instructions that are mandatory.  That is, each
instruction has one or more conformance groups of which it is a member.
(每个一致性组都有一个简短的 ASCII 标签 (如 "base32")，对应一组必须支持的指令。也就是说，每条指令都属于一个或多个一致性组。)

This document defines the following conformance groups:
(本文档定义了以下一致性组：)

* base32: includes all instructions defined in this
  specification unless otherwise noted.
  (base32：包括本规范定义的全部指令，除非另有说明。)
* base64: includes base32, plus instructions explicitly noted
  as being in the base64 conformance group.
  (base64：包括 base32，外加被明确标注属于 base64 一致性组的指令。)
* atomic32: includes 32-bit atomic operation instructions (see `Atomic operations(原子操作)`_).
  (atomic32：包括 32 位原子操作指令 (见 `Atomic operations(原子操作)`_ )。)
* atomic64: includes atomic32, plus 64-bit atomic operation instructions.
  (atomic64：包括 atomic32，外加 64 位原子操作指令。)
* divmul32: includes 32-bit division, multiplication, and modulo instructions.
  (divmul32：包括 32 位除法、乘法与取模指令。)
* divmul64: includes divmul32, plus 64-bit division, multiplication,
  and modulo instructions.
  (divmul64：包括 divmul32，外加 64 位除法、乘法与取模指令。)
* packet: deprecated packet access instructions.
  (packet：已弃用的数据包访问指令。)

Instruction encoding(指令编码)
==============================

BPF has two instruction encodings:
(BPF 有两种指令编码：)

* the basic instruction encoding, which uses 64 bits to encode an instruction
  (基本指令编码：用 64 位编码一条指令。)
* the wide instruction encoding, which appends a second 64 bits
  after the basic instruction for a total of 128 bits.
  (宽指令编码：在基本指令之后再追加第二个 64 位，总计 128 位。)

Basic instruction encoding(基本指令编码)
----------------------------------------

A basic instruction is encoded as follows::

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |    opcode     |     regs      |            offset             |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                              imm                              |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

(基本指令的编码如下。)

**opcode**
  operation to perform, encoded as follows::

    +-+-+-+-+-+-+-+-+
    |specific |class|
    +-+-+-+-+-+-+-+-+

(要执行的操作，编码如下：)

  **specific**
    The format of these bits varies by instruction class
    (这些位的格式因指令类而异)

  **class**
    The instruction class (see `Instruction classes(指令类)`_)
    (指令类 (见 `Instruction classes(指令类)`_ ))

**regs**
  The source and destination register numbers, encoded as follows
  on a little-endian host::

    +-+-+-+-+-+-+-+-+
    |src_reg|dst_reg|
    +-+-+-+-+-+-+-+-+

(源与目标寄存器编号，在小端主机上编码如下：)

  and as follows on a big-endian host::

    +-+-+-+-+-+-+-+-+
    |dst_reg|src_reg|
    +-+-+-+-+-+-+-+-+

(在大端主机上编码如下：)

  **src_reg**
    the source register number (0-10), except where otherwise specified
    (`64-bit immediate instructions(64 位立即数指令)`_ reuse this field for other purposes)
    (源寄存器编号 (0-10)，除非另有说明 (`64-bit immediate instructions(64 位立即数指令)`_ 会把该字段挪作他用))

  **dst_reg**
    destination register number (0-10), unless otherwise specified
    (future instructions might reuse this field for other purposes)
    (目标寄存器编号 (0-10)，除非另有说明 (未来的指令可能把该字段挪作他用))

**offset**
  signed integer offset used with pointer arithmetic, except where
  otherwise specified (some arithmetic instructions reuse this field
  for other purposes)
(用于指针运算的有符号整数偏移，除非另有说明 (某些算术指令会把该字段挪作他用))

**imm**
  signed integer immediate value
(有符号整数立即值)

Note that the contents of multi-byte fields ('offset' and 'imm') are
stored using big-endian byte ordering on big-endian hosts and
little-endian byte ordering on little-endian hosts.
(注意，多字节字段 ('offset' 与 'imm') 的内容在大端主机上按大端字节序存储，在小端主机上按小端字节序存储。)

For example::

  opcode                  offset imm          assembly
         src_reg dst_reg
  07     0       1        00 00  44 33 22 11  r1 += 0x11223344 // little
         dst_reg src_reg
  07     1       0        00 00  11 22 33 44  r1 += 0x11223344 // big

(例如：)

Note that most instructions do not use all of the fields.
Unused fields SHALL be cleared to zero.
(注意，大多数指令不会用到全部字段。未使用的字段 SHALL 清零。)

Wide instruction encoding(宽指令编码)
-------------------------------------

Some instructions are defined to use the wide instruction encoding,
which uses two 32-bit immediate values.  The 64 bits following
the basic instruction format contain a pseudo instruction
with 'opcode', 'dst_reg', 'src_reg', and 'offset' all set to zero.
(有些指令被定义为使用宽指令编码，它使用两个 32 位立即值。基本指令格式之后的 64 位包含一条伪指令，其 'opcode'、'dst_reg'、'src_reg' 与 'offset' 全部置零。)

This is depicted in the following figure::

  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |    opcode     |     regs      |            offset             |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                              imm                              |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                           reserved                            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                           next_imm                            |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

(如下图所示：)

**opcode**
  operation to perform, encoded as explained above
(要执行的操作，按上文所述编码)

**regs**
  The source and destination register numbers (unless otherwise
  specified), encoded as explained above
(源与目标寄存器编号 (除非另有说明)，按上文所述编码)

**offset**
  signed integer offset used with pointer arithmetic, unless
  otherwise specified
(用于指针运算的有符号整数偏移，除非另有说明)

**imm**
  signed integer immediate value
(有符号整数立即值)

**reserved**
  unused, set to zero
(未使用，置零)

**next_imm**
  second signed integer immediate value
(第二个有符号整数立即值)

Instruction classes(指令类)
---------------------------

The three least significant bits of the 'opcode' field store the instruction class:
('opcode' 字段的最低三位存储指令类：)

.. table:: Instruction class

  =====  =====  ===============================  ===================================
  class  value  description                      reference
  =====  =====  ===============================  ===================================
  LD     0x0    non-standard load operations     `Load and store instructions(加载与存储指令)`_
  LDX    0x1    load into register operations    `Load and store instructions(加载与存储指令)`_
  ST     0x2    store from immediate operations  `Load and store instructions(加载与存储指令)`_
  STX    0x3    store from register operations   `Load and store instructions(加载与存储指令)`_
  ALU    0x4    32-bit arithmetic operations     `Arithmetic and jump instructions(算术与跳转指令)`_
  JMP    0x5    64-bit jump operations           `Arithmetic and jump instructions(算术与跳转指令)`_
  JMP32  0x6    32-bit jump operations           `Arithmetic and jump instructions(算术与跳转指令)`_
  ALU64  0x7    64-bit arithmetic operations     `Arithmetic and jump instructions(算术与跳转指令)`_
  =====  =====  ===============================  ===================================

Arithmetic and jump instructions(算术与跳转指令)
================================================

For arithmetic and jump instructions (``ALU``, ``ALU64``, ``JMP`` and
``JMP32``), the 8-bit 'opcode' field is divided into three parts::

  +-+-+-+-+-+-+-+-+
  |  code |s|class|
  +-+-+-+-+-+-+-+-+

(对于算术与跳转指令 ( ``ALU`` 、 ``ALU64`` 、 ``JMP`` 和 ``JMP32`` )，8 位的 'opcode' 字段被划分为三个部分。)

**code**
  the operation code, whose meaning varies by instruction class
  (操作码，其含义因指令类而异)

**s (source)**
  the source operand location, which unless otherwise specified is one of:
  (源操作数位置，除非另有说明，取以下之一：)

  .. table:: Source operand location

    ======  =====  ==============================================
    source  value  description
    ======  =====  ==============================================
    K       0      use 32-bit 'imm' value as source operand
    X       1      use 'src_reg' register value as source operand
    ======  =====  ==============================================

**instruction class**
  the instruction class (see `Instruction classes(指令类)`_)
  (指令类 (见 `Instruction classes(指令类)`_ ))

Arithmetic instructions(算术指令)
---------------------------------

``ALU`` uses 32-bit wide operands while ``ALU64`` uses 64-bit wide operands for
otherwise identical operations. ``ALU64`` instructions belong to the
base64 conformance group unless noted otherwise.
The 'code' field encodes the operation as below, where 'src' refers to the
the source operand and 'dst' refers to the value of the destination
register.
(``ALU`` 使用 32 位宽的操作数，而 ``ALU64`` 在其他方面相同的操作上使用 64 位宽的操作数。除非另有说明， ``ALU64`` 指令属于 base64 一致性组。'code' 字段按下表编码操作，其中 'src' 指源操作数，'dst' 指目标寄存器的值。)

.. table:: Arithmetic instructions

  =====  =====  =======  ===================================================================================
  name   code   offset   description
  =====  =====  =======  ===================================================================================
  ADD    0x0    0        dst += src
  SUB    0x1    0        dst -= src
  MUL    0x2    0        dst \*= src
  DIV    0x3    0        dst = (src != 0) ? (dst / src) : 0
  SDIV   0x3    1        dst = (src == 0) ? 0 : ((src == -1 && dst == LLONG_MIN) ? LLONG_MIN : (dst s/ src))
  OR     0x4    0        dst \|= src
  AND    0x5    0        dst &= src
  LSH    0x6    0        dst <<= (src & mask)
  RSH    0x7    0        dst >>= (src & mask)
  NEG    0x8    0        dst = -dst
  MOD    0x9    0        dst = (src != 0) ? (dst % src) : dst
  SMOD   0x9    1        dst = (src == 0) ? dst : ((src == -1 && dst == LLONG_MIN) ? 0: (dst s% src))
  XOR    0xa    0        dst ^= src
  MOV    0xb    0        dst = src
  MOVSX  0xb    8/16/32  dst = (s8,s16,s32)src
  ARSH   0xc    0        :term:`sign extending<Sign Extend>` dst >>= (src & mask)
  END    0xd    0        byte swap operations (see `Byte swap instructions(字节交换指令)`_ below)
  =====  =====  =======  ===================================================================================

Underflow and overflow are allowed during arithmetic operations, meaning
the 64-bit or 32-bit value will wrap. If BPF program execution would
result in division by zero, the destination register is instead set to zero.
Otherwise, for ``ALU64``, if execution would result in ``LLONG_MIN``
divided by -1, the destination register is instead set to ``LLONG_MIN``. For
``ALU``, if execution would result in ``INT_MIN`` divided by -1, the
destination register is instead set to ``INT_MIN``.
(算术运算允许下溢与上溢，即 64 位或 32 位的值会发生回绕。如果 BPF 程序执行会导致除以零，目标寄存器改为置零。除此之外，对 ``ALU64`` 而言，如果执行会导致 ``LLONG_MIN`` 除以 -1，目标寄存器改为置为 ``LLONG_MIN`` ；对 ``ALU`` 而言，如果执行会导致 ``INT_MIN`` 除以 -1，目标寄存器改为置为 ``INT_MIN`` 。)

If execution would result in modulo by zero, for ``ALU64`` the value of
the destination register is unchanged whereas for ``ALU`` the upper
32 bits of the destination register are zeroed. Otherwise, for ``ALU64``,
if execution would resuslt in ``LLONG_MIN`` modulo -1, the destination
register is instead set to 0. For ``ALU``, if execution would result in
``INT_MIN`` modulo -1, the destination register is instead set to 0.
(如果执行会导致对零取模， ``ALU64`` 的目标寄存器值保持不变，而 ``ALU`` 的目标寄存器高 32 位被清零。除此之外，对 ``ALU64`` 而言，如果执行会导致 ``LLONG_MIN`` 对 -1 取模，目标寄存器改为置 0；对 ``ALU`` 而言，如果执行会导致 ``INT_MIN`` 对 -1 取模，目标寄存器也改为置 0。)

``{ADD, X, ALU}``, where 'code' = ``ADD``, 'source' = ``X``, and 'class' = ``ALU``, means::

  dst = (u32) ((u32) dst + (u32) src)

( ``{ADD, X, ALU}`` ，即 'code' = ``ADD`` 、'source' = ``X`` 、'class' = ``ALU`` ，表示：)

where '(u32)' indicates that the upper 32 bits are zeroed.
(其中 '(u32)' 表示高 32 位被清零。)

``{ADD, X, ALU64}`` means::

  dst = dst + src

( ``{ADD, X, ALU64}`` 表示：)

``{XOR, K, ALU}`` means::

  dst = (u32) dst ^ (u32) imm

( ``{XOR, K, ALU}`` 表示：)

``{XOR, K, ALU64}`` means::

  dst = dst ^ imm

( ``{XOR, K, ALU64}`` 表示：)

Note that most arithmetic instructions have 'offset' set to 0. Only three instructions
(``SDIV``, ``SMOD``, ``MOVSX``) have a non-zero 'offset'.
(注意，大多数算术指令的 'offset' 置为 0。只有三条指令 ( ``SDIV`` 、 ``SMOD`` 、 ``MOVSX`` ) 的 'offset' 非零。)

Division, multiplication, and modulo operations for ``ALU`` are part
of the "divmul32" conformance group, and division, multiplication, and
modulo operations for ``ALU64`` are part of the "divmul64" conformance
group.
The division and modulo operations support both unsigned and signed flavors.
(``ALU`` 的除法、乘法与取模操作属于 "divmul32" 一致性组， ``ALU64`` 的除法、乘法与取模操作属于 "divmul64" 一致性组。除法与取模操作同时支持无符号与有符号两种形式。)

For unsigned operations (``DIV`` and ``MOD``), for ``ALU``,
'imm' is interpreted as a 32-bit unsigned value. For ``ALU64``,
'imm' is first :term:`sign extended<Sign Extend>` from 32 to 64 bits, and then
interpreted as a 64-bit unsigned value.
(对于无符号操作 ( ``DIV`` 与 ``MOD`` )， ``ALU`` 中 'imm' 被解释为 32 位无符号值； ``ALU64`` 中 'imm' 先从 32 位 :term:`符号扩展<Sign Extend>` 到 64 位，然后被解释为 64 位无符号值。)

For signed operations (``SDIV`` and ``SMOD``), for ``ALU``,
'imm' is interpreted as a 32-bit signed value. For ``ALU64``, 'imm'
is first :term:`sign extended<Sign Extend>` from 32 to 64 bits, and then
interpreted as a 64-bit signed value.
(对于有符号操作 ( ``SDIV`` 与 ``SMOD`` )， ``ALU`` 中 'imm' 被解释为 32 位有符号值； ``ALU64`` 中 'imm' 先从 32 位 :term:`符号扩展<Sign Extend>` 到 64 位，然后被解释为 64 位有符号值。)

Note that there are varying definitions of the signed modulo operation
when the dividend or divisor are negative, where implementations often
vary by language such that Python, Ruby, etc.  differ from C, Go, Java,
etc. This specification requires that signed modulo MUST use truncated division
(where -13 % 3 == -1) as implemented in C, Go, etc.::

   a % n = a - n * trunc(a / n)

(注意，当被除数或除数为负时，有符号取模操作存在多种定义，不同语言的实现常常不同，例如 Python、Ruby 等与 C、Go、Java 等就不一样。本规范要求有符号取模 MUST 使用 C、Go 等语言实现的那种截断除法 (即 -13 % 3 == -1)：)

The ``MOVSX`` instruction does a move operation with sign extension.
``{MOVSX, X, ALU}`` :term:`sign extends<Sign Extend>` 8-bit and 16-bit operands into
32-bit operands, and zeroes the remaining upper 32 bits.
``{MOVSX, X, ALU64}`` :term:`sign extends<Sign Extend>` 8-bit, 16-bit, and 32-bit
operands into 64-bit operands.  Unlike other arithmetic instructions,
``MOVSX`` is only defined for register source operands (``X``).
( ``MOVSX`` 指令执行带符号扩展的传送操作。 ``{MOVSX, X, ALU}`` 把 8 位与 16 位操作数 :term:`符号扩展<Sign Extend>` 成 32 位操作数，并把剩余的高 32 位清零。 ``{MOVSX, X, ALU64}`` 把 8 位、16 位与 32 位操作数 :term:`符号扩展<Sign Extend>` 成 64 位操作数。与其他算术指令不同， ``MOVSX`` 只对寄存器源操作数 ( ``X`` ) 有定义。)

``{MOV, K, ALU64}`` means::

  dst = (s64)imm

( ``{MOV, K, ALU64}`` 表示：)

``{MOV, X, ALU}`` means::

  dst = (u32)src

( ``{MOV, X, ALU}`` 表示：)

``{MOVSX, X, ALU}`` with 'offset' 8 means::

  dst = (u32)(s32)(s8)src

( 'offset' 为 8 的 ``{MOVSX, X, ALU}`` 表示：)


The ``NEG`` instruction is only defined when the source bit is clear
(``K``).
( ``NEG`` 指令只在源位清零 ( ``K`` ) 时有定义。)

Shift operations use a mask of 0x3F (63) for 64-bit operations and 0x1F (31)
for 32-bit operations.
(移位操作在 64 位操作时使用掩码 0x3F (63)，在 32 位操作时使用掩码 0x1F (31)。)

Byte swap instructions(字节交换指令)
------------------------------------

The byte swap instructions use instruction classes of ``ALU`` and ``ALU64``
and a 4-bit 'code' field of ``END``.
(字节交换指令使用 ``ALU`` 与 ``ALU64`` 指令类以及 4 位的 ``END`` 'code' 字段。)

The byte swap instructions operate on the destination register
only and do not use a separate source register or immediate value.
(字节交换指令只作用于目标寄存器，不使用单独的源寄存器或立即值。)

For ``ALU``, the 1-bit source operand field in the opcode is used to
select what byte order the operation converts from or to. For
``ALU64``, the 1-bit source operand field in the opcode is reserved
and MUST be set to 0.
(对 ``ALU`` 而言，opcode 中的 1 位源操作数字段用于选择操作要转换自/转换到的字节序。对 ``ALU64`` 而言，opcode 中的 1 位源操作数字段为保留位，MUST 置 0。)

.. table:: Byte swap instructions

  =====  ========  =====  =================================================
  class  source    value  description
  =====  ========  =====  =================================================
  ALU    LE        0      convert between host byte order and little endian
  ALU    BE        1      convert between host byte order and big endian
  ALU64  Reserved  0      do byte swap unconditionally
  =====  ========  =====  =================================================

The 'imm' field encodes the width of the swap operations.  The following widths
are supported: 16, 32 and 64.  Width 64 operations belong to the base64
conformance group and other swap operations belong to the base32
conformance group.
('imm' 字段编码交换操作的宽度。支持的宽度为 16、32 和 64。宽度 64 的操作属于 base64 一致性组，其他交换操作属于 base32 一致性组。)

Examples:

(示例：)

``{END, LE, ALU}`` with 'imm' = 16/32/64 means::

  dst = le16(dst)
  dst = le32(dst)
  dst = le64(dst)

( 'imm' = 16/32/64 的 ``{END, LE, ALU}`` 表示：)

``{END, BE, ALU}`` with 'imm' = 16/32/64 means::

  dst = be16(dst)
  dst = be32(dst)
  dst = be64(dst)

( 'imm' = 16/32/64 的 ``{END, BE, ALU}`` 表示：)

``{END, TO, ALU64}`` with 'imm' = 16/32/64 means::

  dst = bswap16(dst)
  dst = bswap32(dst)
  dst = bswap64(dst)

( 'imm' = 16/32/64 的 ``{END, TO, ALU64}`` 表示：)

Jump instructions(跳转指令)
---------------------------

``JMP32`` uses 32-bit wide operands and indicates the base32
conformance group, while ``JMP`` uses 64-bit wide operands for
otherwise identical operations, and indicates the base64 conformance
group unless otherwise specified.
The 'code' field encodes the operation as below:
( ``JMP32`` 使用 32 位宽的操作数，对应 base32 一致性组； ``JMP`` 在其他方面相同的操作上使用 64 位宽的操作数，除非另有说明，对应 base64 一致性组。'code' 字段按下表编码操作：)

.. table:: Jump instructions

  ========  =====  =======  =================================  ===================================================
  code      value  src_reg  description                        notes
  ========  =====  =======  =================================  ===================================================
  JA        0x0    0x0      PC += offset                       {JA, K, JMP} only
  JA        0x0    0x0      PC += imm                          {JA, K, JMP32} only
  JEQ       0x1    any      PC += offset if dst == src
  JGT       0x2    any      PC += offset if dst > src          unsigned
  JGE       0x3    any      PC += offset if dst >= src         unsigned
  JSET      0x4    any      PC += offset if dst & src
  JNE       0x5    any      PC += offset if dst != src
  JSGT      0x6    any      PC += offset if dst > src          signed
  JSGE      0x7    any      PC += offset if dst >= src         signed
  CALL      0x8    0x0      call helper function by static ID  {CALL, K, JMP} only, see `Helper functions(辅助函数)`_
  CALL      0x8    0x1      call PC += imm                     {CALL, K, JMP} only, see `Program-local functions(程序局部函数)`_
  CALL      0x8    0x2      call helper function by BTF ID     {CALL, K, JMP} only, see `Helper functions(辅助函数)`_
  EXIT      0x9    0x0      return                             {CALL, K, JMP} only
  JLT       0xa    any      PC += offset if dst < src          unsigned
  JLE       0xb    any      PC += offset if dst <= src         unsigned
  JSLT      0xc    any      PC += offset if dst < src          signed
  JSLE      0xd    any      PC += offset if dst <= src         signed
  ========  =====  =======  =================================  ===================================================

where 'PC' denotes the program counter, and the offset to increment by
is in units of 64-bit instructions relative to the instruction following
the jump instruction.  Thus 'PC += 1' skips execution of the next
instruction if it's a basic instruction or results in undefined behavior
if the next instruction is a 128-bit wide instruction.
(其中 'PC' 表示程序计数器，要增加的偏移以 64 位指令为单位，相对于跳转指令的下一条指令。因此，如果下一条指令是基本指令，'PC += 1' 会跳过它的执行；如果下一条指令是 128 位宽指令，则行为未定义。)

Example:

(示例：)

``{JSGE, X, JMP32}`` means::

  if (s32)dst s>= (s32)src goto +offset

( ``{JSGE, X, JMP32}`` 表示：)

where 's>=' indicates a signed '>=' comparison.
(其中 's>=' 表示有符号的 '>=' 比较。)

``{JLE, K, JMP}`` means::

  if dst <= (u64)(s64)imm goto +offset

( ``{JLE, K, JMP}`` 表示：)

``{JA, K, JMP32}`` means::

  gotol +imm

( ``{JA, K, JMP32}`` 表示：)

where 'imm' means the branch offset comes from the 'imm' field.
(其中 'imm' 表示分支偏移来自 'imm' 字段。)

Note that there are two flavors of ``JA`` instructions. The
``JMP`` class permits a 16-bit jump offset specified by the 'offset'
field, whereas the ``JMP32`` class permits a 32-bit jump offset
specified by the 'imm' field. A > 16-bit conditional jump may be
converted to a < 16-bit conditional jump plus a 32-bit unconditional
jump.
(注意， ``JA`` 指令有两种变体。 ``JMP`` 类允许由 'offset' 字段指定的 16 位跳转偏移，而 ``JMP32`` 类允许由 'imm' 字段指定的 32 位跳转偏移。大于 16 位的条件跳转可以转换为小于 16 位的条件跳转加一个 32 位无条件跳转。)

All ``CALL`` and ``JA`` instructions belong to the
base32 conformance group.
(所有 ``CALL`` 与 ``JA`` 指令都属于 base32 一致性组。)

Helper functions(辅助函数)
~~~~~~~~~~~~~~~~~~~~~~~~~~

Helper functions are a concept whereby BPF programs can call into a
set of function calls exposed by the underlying platform.
(辅助函数 (helper functions) 是一种机制，让 BPF 程序能够调用底层平台暴露的一组函数调用。)

Historically, each helper function was identified by a static ID
encoded in the 'imm' field.  Further documentation of helper functions
is outside the scope of this document and standardization is left for
future work, but use is widely deployed and more information can be
found in platform-specific documentation (e.g., Linux kernel documentation).
(历史上，每个辅助函数由编码在 'imm' 字段中的静态 ID 标识。辅助函数的更多文档超出本文档的范围，其标准化留待后续工作，但其使用已广泛部署，更多信息可在平台特定的文档 (如 Linux 内核文档) 中找到。)

Platforms that support the BPF Type Format (BTF) support identifying
a helper function by a BTF ID encoded in the 'imm' field, where the BTF ID
identifies the helper name and type.  Further documentation of BTF
is outside the scope of this document and standardization is left for
future work, but use is widely deployed and more information can be
found in platform-specific documentation (e.g., Linux kernel documentation).
(支持 BPF Type Format (BTF) 的平台支持以编码在 'imm' 字段中的 BTF ID 来标识辅助函数，BTF ID 标识辅助函数的名字与类型。BTF 的更多文档超出本文档的范围，其标准化留待后续工作，但其使用已广泛部署，更多信息可在平台特定的文档 (如 Linux 内核文档) 中找到。)

Program-local functions(程序局部函数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Program-local functions are functions exposed by the same BPF program as the
caller, and are referenced by offset from the instruction following the call
instruction, similar to ``JA``.  The offset is encoded in the 'imm' field of
the call instruction. An ``EXIT`` within the program-local function will
return to the caller.
(程序局部函数是由与调用者同一个 BPF 程序暴露的函数，以相对 call 指令下一条指令的偏移来引用，类似 ``JA`` 。该偏移编码在 call 指令的 'imm' 字段中。程序局部函数内部的 ``EXIT`` 会返回到调用者。)

Load and store instructions(加载与存储指令)
===========================================

For load and store instructions (``LD``, ``LDX``, ``ST``, and ``STX``), the
8-bit 'opcode' field is divided as follows::

  +-+-+-+-+-+-+-+-+
  |mode |sz |class|
  +-+-+-+-+-+-+-+-+

(对于加载与存储指令 ( ``LD`` 、 ``LDX`` 、 ``ST`` 和 ``STX`` )，8 位的 'opcode' 字段划分如下。)

**mode**
  The mode modifier is one of:
  (模式修饰符 (mode modifier) 为以下之一：)

  .. table:: Mode modifier

    =============  =====  ====================================  =============
    mode modifier  value  description                           reference
    =============  =====  ====================================  =============
    IMM            0      64-bit immediate instructions         `64-bit immediate instructions(64 位立即数指令)`_
    ABS            1      legacy BPF packet access (absolute)   `Legacy BPF Packet access instructions(传统 BPF 数据包访问指令)`_
    IND            2      legacy BPF packet access (indirect)   `Legacy BPF Packet access instructions(传统 BPF 数据包访问指令)`_
    MEM            3      regular load and store operations     `Regular load and store operations(常规加载与存储操作)`_
    MEMSX          4      sign-extension load operations        `Sign-extension load operations(符号扩展加载操作)`_
    ATOMIC         6      atomic operations                     `Atomic operations(原子操作)`_
    =============  =====  ====================================  =============

**sz (size)**
  The size modifier is one of:
  (大小修饰符 (size modifier) 为以下之一：)

  .. table:: Size modifier

    ====  =====  =====================
    size  value  description
    ====  =====  =====================
    W     0      word        (4 bytes)
    H     1      half word   (2 bytes)
    B     2      byte
    DW    3      double word (8 bytes)
    ====  =====  =====================

  Instructions using ``DW`` belong to the base64 conformance group.
  (使用 ``DW`` 的指令属于 base64 一致性组。)

**class**
  The instruction class (see `Instruction classes(指令类)`_)
  (指令类 (见 `Instruction classes(指令类)`_ ))

Regular load and store operations(常规加载与存储操作)
-----------------------------------------------------

The ``MEM`` mode modifier is used to encode regular load and store
instructions that transfer data between a register and memory.
( ``MEM`` 模式修饰符用于编码在寄存器与内存之间传输数据的常规加载与存储指令。)

``{MEM, <size>, STX}`` means::

  *(size *) (dst + offset) = src

( ``{MEM, <size>, STX}`` 表示：)

``{MEM, <size>, ST}`` means::

  *(size *) (dst + offset) = imm

( ``{MEM, <size>, ST}`` 表示：)

``{MEM, <size>, LDX}`` means::

  dst = *(unsigned size *) (src + offset)

( ``{MEM, <size>, LDX}`` 表示：)

Where '<size>' is one of: ``B``, ``H``, ``W``, or ``DW``, and
'unsigned size' is one of: u8, u16, u32, or u64.
(其中 '<size>' 取 ``B`` 、 ``H`` 、 ``W`` 或 ``DW`` 之一，'unsigned size' 取 u8、u16、u32 或 u64 之一。)

Sign-extension load operations(符号扩展加载操作)
------------------------------------------------

The ``MEMSX`` mode modifier is used to encode :term:`sign-extension<Sign Extend>` load
instructions that transfer data between a register and memory.
( ``MEMSX`` 模式修饰符用于编码在寄存器与内存之间传输数据的 :term:`符号扩展<Sign Extend>` 加载指令。)

``{MEMSX, <size>, LDX}`` means::

  dst = *(signed size *) (src + offset)

( ``{MEMSX, <size>, LDX}`` 表示：)

Where '<size>' is one of: ``B``, ``H``, or ``W``, and
'signed size' is one of: s8, s16, or s32.
(其中 '<size>' 取 ``B`` 、 ``H`` 或 ``W`` 之一，'signed size' 取 s8、s16 或 s32 之一。)

Atomic operations(原子操作)
---------------------------

Atomic operations are operations that operate on memory and can not be
interrupted or corrupted by other access to the same memory region
by other BPF programs or means outside of this specification.
(原子操作是对内存进行操作、且不会被其他 BPF 程序或本规范之外的途径对同一内存区域的访问所打断或破坏的操作。)

All atomic operations supported by BPF are encoded as store operations
that use the ``ATOMIC`` mode modifier as follows:
(BPF 支持的所有原子操作都被编码为使用 ``ATOMIC`` 模式修饰符的存储操作，如下：)

* ``{ATOMIC, W, STX}`` for 32-bit operations, which are
  part of the "atomic32" conformance group.
  ( ``{ATOMIC, W, STX}`` 用于 32 位操作，属于 "atomic32" 一致性组。)
* ``{ATOMIC, DW, STX}`` for 64-bit operations, which are
  part of the "atomic64" conformance group.
  ( ``{ATOMIC, DW, STX}`` 用于 64 位操作，属于 "atomic64" 一致性组。)
* 8-bit and 16-bit wide atomic operations are not supported.
  (不支持 8 位与 16 位宽的原子操作。)

The 'imm' field is used to encode the actual atomic operation.
Simple atomic operation use a subset of the values defined to encode
arithmetic operations in the 'imm' field to encode the atomic operation:
('imm' 字段用于编码实际的原子操作。简单原子操作使用 'imm' 字段中为编码算术操作所定义的值的子集来编码原子操作：)

.. table:: Simple atomic operations

  ========  =====  ===========
  imm       value  description
  ========  =====  ===========
  ADD       0x00   atomic add
  OR        0x40   atomic or
  AND       0x50   atomic and
  XOR       0xa0   atomic xor
  ========  =====  ===========


``{ATOMIC, W, STX}`` with 'imm' = ADD means::

  *(u32 *)(dst + offset) += src

( 'imm' = ADD 的 ``{ATOMIC, W, STX}`` 表示：)

``{ATOMIC, DW, STX}`` with 'imm' = ADD means::

  *(u64 *)(dst + offset) += src

( 'imm' = ADD 的 ``{ATOMIC, DW, STX}`` 表示：)

In addition to the simple atomic operations, there also is a modifier and
two complex atomic operations:
(除简单原子操作外，还有一个修饰符和两种复杂原子操作：)

.. table:: Complex atomic operations

  ===========  ================  ===========================
  imm          value             description
  ===========  ================  ===========================
  FETCH        0x01              modifier: return old value
  XCHG         0xe0 | FETCH      atomic exchange
  CMPXCHG      0xf0 | FETCH      atomic compare and exchange
  ===========  ================  ===========================

The ``FETCH`` modifier is optional for simple atomic operations, and
always set for the complex atomic operations.  If the ``FETCH`` flag
is set, then the operation also overwrites ``src`` with the value that
was in memory before it was modified.
( ``FETCH`` 修饰符对简单原子操作是可选的，对复杂原子操作则总是置位。如果设置了 ``FETCH`` 标志，该操作还会用修改前内存中的值覆写 ``src`` 。)

The ``XCHG`` operation atomically exchanges ``src`` with the value
addressed by ``dst + offset``.
( ``XCHG`` 操作原子地把 ``src`` 与 ``dst + offset`` 寻址的值交换。)

The ``CMPXCHG`` operation atomically compares the value addressed by
``dst + offset`` with ``R0``. If they match, the value addressed by
``dst + offset`` is replaced with ``src``. In either case, the
value that was at ``dst + offset`` before the operation is zero-extended
and loaded back to ``R0``.
( ``CMPXCHG`` 操作原子地把 ``dst + offset`` 寻址的值与 ``R0`` 比较。如果匹配， ``dst + offset`` 寻址的值被替换为 ``src`` 。无论哪种情况，操作前 ``dst + offset`` 处的值都会被零扩展并载回 ``R0`` 。)

64-bit immediate instructions(64 位立即数指令)
----------------------------------------------

Instructions with the ``IMM`` 'mode' modifier use the wide instruction
encoding defined in `Instruction encoding(指令编码)`_, and use the 'src_reg' field of the
basic instruction to hold an opcode subtype.
(带 ``IMM`` 'mode' 修饰符的指令使用 `Instruction encoding(指令编码)`_ 中定义的宽指令编码，并把基本指令的 'src_reg' 字段用作 opcode 子类型。)

The following table defines a set of ``{IMM, DW, LD}`` instructions
with opcode subtypes in the 'src_reg' field, using new terms such as "map"
defined further below:
(下表定义了一组 ``{IMM, DW, LD}`` 指令，其 opcode 子类型位于 'src_reg' 字段中，并使用了下文进一步定义的 "map" 等新术语：)

.. table:: 64-bit immediate instructions

  =======  =========================================  ===========  ==============
  src_reg  pseudocode                                 imm type     dst type
  =======  =========================================  ===========  ==============
  0x0      dst = (next_imm << 32) | imm               integer      integer
  0x1      dst = map_by_fd(imm)                       map fd       map
  0x2      dst = map_val(map_by_fd(imm)) + next_imm   map fd       data address
  0x3      dst = var_addr(imm)                        variable id  data address
  0x4      dst = code_addr(imm)                       integer      code address
  0x5      dst = map_by_idx(imm)                      map index    map
  0x6      dst = map_val(map_by_idx(imm)) + next_imm  map index    data address
  =======  =========================================  ===========  ==============

where

(其中：)

* map_by_fd(imm) means to convert a 32-bit file descriptor into an address of a map (see `Maps(映射)`_)
  (map_by_fd(imm)：把 32 位文件描述符转换为某个 map 的地址 (见 `Maps(映射)`_ ))
* map_by_idx(imm) means to convert a 32-bit index into an address of a map
  (map_by_idx(imm)：把 32 位索引转换为某个 map 的地址)
* map_val(map) gets the address of the first value in a given map
  (map_val(map)：获取给定 map 中第一个值的地址)
* var_addr(imm) gets the address of a platform variable (see `Platform Variables(平台变量)`_) with a given id
  (var_addr(imm)：获取具有给定 id 的平台变量 (见 `Platform Variables(平台变量)`_ ) 的地址)
* code_addr(imm) gets the address of the instruction at a specified relative offset in number of (64-bit) instructions
  (code_addr(imm)：以 (64 位) 指令数为单位，获取处于指定相对偏移处指令的地址)
* the 'imm type' can be used by disassemblers for display
  ('imm type' 可供反汇编器用于显示)
* the 'dst type' can be used for verification and JIT compilation purposes
  ('dst type' 可用于校验与 JIT 编译目的)

Maps(映射)
~~~~~~~~~~

Maps are shared memory regions accessible by BPF programs on some platforms.
A map can have various semantics as defined in a separate document, and may or
may not have a single contiguous memory region, but the 'map_val(map)' is
currently only defined for maps that do have a single contiguous memory region.
(map 是某些平台上 BPF 程序可访问的共享内存区域。map 可以具有单独文档中定义的各种语义，可能有也可能没有单块连续的内存区域，但 'map_val(map)' 目前只对确实拥有单块连续内存区域的 map 有定义。)

Each map can have a file descriptor (fd) if supported by the platform, where
'map_by_fd(imm)' means to get the map with the specified file descriptor. Each
BPF program can also be defined to use a set of maps associated with the
program at load time, and 'map_by_idx(imm)' means to get the map with the given
index in the set associated with the BPF program containing the instruction.
(如果平台支持，每个 map 可以有文件描述符 (fd)，'map_by_fd(imm)' 表示获取具有指定文件描述符的 map。每个 BPF 程序也可以被定义为使用在加载时与程序关联的一组 map，'map_by_idx(imm)' 表示获取该组中具有给定索引的 map，该组与包含该指令的 BPF 程序关联。)

Platform Variables(平台变量)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Platform variables are memory regions, identified by integer ids, exposed by
the runtime and accessible by BPF programs on some platforms.  The
'var_addr(imm)' operation means to get the address of the memory region
identified by the given id.
(平台变量是由运行时暴露、在某些平台上可被 BPF 程序访问、以整数 id 标识的内存区域。 'var_addr(imm)' 操作表示获取由给定 id 标识的内存区域的地址。)

Legacy BPF Packet access instructions(传统 BPF 数据包访问指令)
--------------------------------------------------------------

BPF previously introduced special instructions for access to packet data that were
carried over from classic BPF. These instructions used an instruction
class of ``LD``, a size modifier of ``W``, ``H``, or ``B``, and a
mode modifier of ``ABS`` or ``IND``.  The 'dst_reg' and 'offset' fields were
set to zero, and 'src_reg' was set to zero for ``ABS``.  However, these
instructions are deprecated and SHOULD no longer be used.  All legacy packet
access instructions belong to the "packet" conformance group.
(BPF 之前引入了从经典 BPF 沿袭下来的、用于访问数据包数据的特殊指令。这些指令使用 ``LD`` 指令类、 ``W`` 、 ``H`` 或 ``B`` 大小修饰符，以及 ``ABS`` 或 ``IND`` 模式修饰符。 'dst_reg' 与 'offset' 字段置零， ``ABS`` 时 'src_reg' 也置零。不过，这些指令已被弃用，SHOULD 不再使用。所有传统数据包访问指令都属于 "packet" 一致性组。)
