.. contents::
.. sectnum::

================================================================================
BPF ABI Recommended Conventions and Guidelines v1.0(BPF ABI 推荐约定与准则 v1.0)
================================================================================

This is version 1.0 of an informational document containing recommended
conventions and guidelines for producing portable BPF program binaries.
(本文档是 1.0 版的信息性文档，包含用于生成可移植 BPF 程序二进制文件的推荐约定与准则。)

Registers and calling convention(寄存器与调用约定)
====================================================

BPF has 10 general purpose registers and a read-only frame pointer register,
all of which are 64-bits wide.
(BPF 拥有 10 个通用寄存器和一个只读帧指针寄存器，全部为 64 位宽。)

The BPF calling convention is defined as:
(BPF 调用约定定义如下：)

* R0: return value from function calls, and exit value for BPF programs
  (R0：函数调用的返回值，以及 BPF 程序的退出值。)
* R1 - R5: arguments for function calls
  (R1 - R5：函数调用的参数。)
* R6 - R9: callee saved registers that function calls will preserve
  (R6 - R9：被调用者保存寄存器，函数调用会保留这些寄存器。)
* R10: read-only frame pointer to access stack
  (R10：用于访问栈的只读帧指针。)

R0 - R5 are scratch registers and BPF programs needs to spill/fill them if
necessary across calls.
(R0 - R5 是临时寄存器，BPF 程序在跨调用时如有需要须对其进行溢出 (spill)/回填 (fill)。)

The BPF program needs to store the return value into register R0 before doing an
``EXIT``.
(BPF 程序在执行 ``EXIT`` 之前需要把返回值存入寄存器 R0。)
