.. SPDX-License-Identifier: GPL-2.0+

==========================
Generic parser(通用解析器)
==========================

Overview(概述)
==============

The generic parser is a simple parser for parsing mount options,
filesystem options, driver options, subsystem options, etc.
(通用解析器 (generic parser) 是一个简单的解析器，用于解析挂载选项、文件系统选项、驱动选项、子系统选项等。)

Parser API(解析器 API)
======================

.. kernel-doc:: lib/parser.c
   :export:
