.. SPDX-License-Identifier: GPL-2.0

============================================================
Linux-specific ELF idiosyncrasies(Linux 特有的 ELF 特殊行为)
============================================================

Definitions(定义)
=================

"First" program header is the one with the smallest offset in the file:
e_phoff.
("首个"（first）程序头是文件中偏移最小的那个：e_phoff。)

"Last" program header is the one with the biggest offset in the file:
e_phoff + (e_phnum - 1) * sizeof(Elf_Phdr).
("最后一个"（last）程序头是文件中偏移最大的那个：
e_phoff + (e_phnum - 1) * sizeof(Elf_Phdr)。)

PT_INTERP(ELF 解释器段)
=======================

First PT_INTERP program header is used to locate the filename of ELF
interpreter. Other PT_INTERP headers are ignored (since Linux 2.4.11).
(第一个 PT_INTERP 程序头用于定位 ELF 解释器的文件名。其他 PT_INTERP 头被
忽略（自 Linux 2.4.11 起）。)

PT_GNU_STACK(栈可执行性)
========================

Last PT_GNU_STACK program header defines userspace stack executability
(since Linux 2.6.6). Other PT_GNU_STACK headers are ignored.
(最后一个 PT_GNU_STACK 程序头定义用户空间栈的可执行性（自 Linux 2.6.6 起）。
其他 PT_GNU_STACK 头被忽略。)

PT_GNU_PROPERTY(程序属性)
=========================

ELF interpreter's last PT_GNU_PROPERTY program header is used (since
Linux 5.8). If interpreter doesn't have one, then the last PT_GNU_PROPERTY
program header of an executable is used. Other PT_GNU_PROPERTY headers
are ignored.
(使用 ELF 解释器最后一个 PT_GNU_PROPERTY 程序头（自 Linux 5.8 起）。若解释
器没有该头，则使用可执行文件最后一个 PT_GNU_PROPERTY 程序头。其他
PT_GNU_PROPERTY 头被忽略。)
