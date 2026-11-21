.. contents::
.. sectnum::

==========================================
Clang implementation notes(Clang 实现说明)
==========================================

This document provides more details specific to the Clang/LLVM implementation of the eBPF instruction set.
(本文档提供 eBPF 指令集的 Clang/LLVM 实现特有的更多细节。)

Versions(版本)
==============

Clang defined "CPU" versions, where a CPU version of 3 corresponds to the current eBPF ISA.
(Clang 定义了 "CPU" 版本，其中 CPU 版本 3 对应当前的 eBPF ISA。)

Clang can select the eBPF ISA version using ``-mcpu=v3`` for example to select version 3.
(Clang 可以选择 eBPF ISA 版本，例如使用 ``-mcpu=v3`` 来选择版本 3。)

Arithmetic instructions(算术指令)
=================================

For CPU versions prior to 3, Clang v7.0 and later can enable ``BPF_ALU`` support with
``-Xclang -target-feature -Xclang +alu32``.  In CPU version 3, support is automatically included.
(对于 3 之前的 CPU 版本，Clang v7.0 及更高版本可以使用 ``-Xclang -target-feature
-Xclang +alu32`` 启用 ``BPF_ALU`` 支持。在 CPU 版本 3 中，该支持已自动包含。)

Jump instructions(跳转指令)
===========================

If ``-O0`` is used, Clang will generate the ``BPF_CALL | BPF_X | BPF_JMP`` (0x8d)
instruction, which is not supported by the Linux kernel verifier.
(如果使用 ``-O0``，Clang 会生成 ``BPF_CALL | BPF_X | BPF_JMP`` (0x8d) 指令，
而 Linux 内核校验器不支持该指令。)

Atomic operations(原子操作)
===========================

Clang can generate atomic instructions by default when ``-mcpu=v3`` is
enabled. If a lower version for ``-mcpu`` is set, the only atomic instruction
Clang can generate is ``BPF_ADD`` *without* ``BPF_FETCH``. If you need to enable
the atomics features, while keeping a lower ``-mcpu`` version, you can use
``-Xclang -target-feature -Xclang +alu32``.
(启用 ``-mcpu=v3`` 时，Clang 默认可以生成原子指令。如果 ``-mcpu`` 设置为较
低版本，Clang 能生成的唯一原子指令是不带 ``BPF_FETCH`` 的 ``BPF_ADD``。若
需要在保持较低 ``-mcpu`` 版本的同时启用原子特性，可以使用 ``-Xclang
-target-feature -Xclang +alu32``。)
