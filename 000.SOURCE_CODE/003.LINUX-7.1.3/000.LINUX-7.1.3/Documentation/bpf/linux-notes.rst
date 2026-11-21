.. contents::
.. sectnum::

==========================================
Linux implementation notes(Linux 实现说明)
==========================================

This document provides more details specific to the Linux kernel implementation of the eBPF instruction set.
(本文件提供 eBPF 指令集的 Linux 内核实现的更多细节。)

Byte swap instructions(字节交换指令)
====================================

``BPF_FROM_LE`` and ``BPF_FROM_BE`` exist as aliases for ``BPF_TO_LE`` and ``BPF_TO_BE`` respectively.
( ``BPF_FROM_LE`` 与 ``BPF_FROM_BE`` 分别作为 ``BPF_TO_LE`` 与
``BPF_TO_BE`` 的别名而存在。)

Jump instructions(跳转指令)
===========================

``BPF_CALL | BPF_X | BPF_JMP`` (0x8d), where the helper function
integer would be read from a specified register, is not currently supported
by the verifier.  Any programs with this instruction will fail to load
until such support is added.
( ``BPF_CALL | BPF_X | BPF_JMP`` (0x8d)——即从指定寄存器读取辅助函数编
号的指令——目前不受校验器支持。在加入此类支持之前，带有该指令的任何
程序都将无法加载。)

Maps(映射)
==========

Linux only supports the 'map_val(map)' operation on array maps with a single element.
(Linux 只支持对具有单个元素的数组映射使用 'map_val(map)' 操作。)

Linux uses an fd_array to store maps associated with a BPF program. Thus,
map_by_idx(imm) uses the fd at that index in the array.
(Linux 使用 fd_array 来存储与 BPF 程序关联的映射。因此，
map_by_idx(imm) 使用该数组中该索引处的 fd。)

Variables(变量)
===============

The following 64-bit immediate instruction specifies that a variable address,
which corresponds to some integer stored in the 'imm' field, should be loaded:
(下面这条 64 位立即数指令规定：应加载一个变量地址，该地址对应存储在
'imm' 字段中的某个整数：)

=========================  ======  ===  =========================================  ===========  ==============
opcode construction        opcode  src  pseudocode                                 imm type     dst type
=========================  ======  ===  =========================================  ===========  ==============
BPF_IMM | BPF_DW | BPF_LD  0x18    0x3  dst = var_addr(imm)                        variable id  data pointer
=========================  ======  ===  =========================================  ===========  ==============

On Linux, this integer is a BTF ID.

Legacy BPF Packet access instructions(传统 BPF 数据包访问指令)
==============================================================

As mentioned in the `ISA standard documentation
<instruction-set.html#legacy-bpf-packet-access-instructions>`_,
Linux has special eBPF instructions for access to packet data that have been
carried over from classic BPF to retain the performance of legacy socket
filters running in the eBPF interpreter.
(如 `ISA 标准文档
<instruction-set.html#legacy-bpf-packet-access-instructions>`_ 中所述，
Linux 拥有专用于访问数据包数据的特殊 eBPF 指令，这些指令从 classic BPF
沿袭而来，以保持在 eBPF 解释器中运行的传统套接字过滤器的性能。)

The instructions come in two forms: ``BPF_ABS | <size> | BPF_LD`` and
``BPF_IND | <size> | BPF_LD``.
(这些指令有两种形式： ``BPF_ABS | <size> | BPF_LD`` 和
``BPF_IND | <size> | BPF_LD`` 。)

These instructions are used to access packet data and can only be used when
the program context is a pointer to a networking packet.  ``BPF_ABS``
accesses packet data at an absolute offset specified by the immediate data
and ``BPF_IND`` access packet data at an offset that includes the value of
a register in addition to the immediate data.
(这些指令用于访问数据包数据，并且只能在程序上下文为指向网络数据包的
指针时使用。 ``BPF_ABS`` 按立即数指定的绝对偏移访问数据包数据，而
``BPF_IND`` 按包含寄存器值加立即数的偏移访问数据包数据。)

These instructions have seven implicit operands:
(这些指令有七个隐式操作数：)

* Register R6 is an implicit input that must contain a pointer to a
  struct sk_buff.
  (寄存器 R6 是一个隐式输入，必须包含指向 struct sk_buff 的指针。)
* Register R0 is an implicit output which contains the data fetched from
  the packet.
  (寄存器 R0 是一个隐式输出，包含从数据包中取得的数据。)
* Registers R1-R5 are scratch registers that are clobbered by the
  instruction.
  (寄存器 R1-R5 是临时寄存器，会被该指令破坏。)

These instructions have an implicit program exit condition as well. If an
eBPF program attempts access data beyond the packet boundary, the
program execution will be aborted.
(这些指令还有一个隐式的程序退出条件。如果 eBPF 程序试图访问超出数据
包边界的数据，程序执行将被中止。)

``BPF_ABS | BPF_W | BPF_LD`` (0x20) means::

  R0 = ntohl(*(u32 *) ((struct sk_buff *) R6->data + imm))

( ``BPF_ABS | BPF_W | BPF_LD`` (0x20) 表示：)

where ``ntohl()`` converts a 32-bit value from network byte order to host byte order.
(其中 ``ntohl()`` 把一个 32 位值从网络字节序转换为主机字节序。)

``BPF_IND | BPF_W | BPF_LD`` (0x40) means::

  R0 = ntohl(*(u32 *) ((struct sk_buff *) R6->data + src + imm))

( ``BPF_IND | BPF_W | BPF_LD`` (0x40) 表示：)
