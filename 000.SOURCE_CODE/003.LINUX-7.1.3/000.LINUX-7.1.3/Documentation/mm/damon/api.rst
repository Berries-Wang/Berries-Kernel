.. SPDX-License-Identifier: GPL-2.0

=======================
API Reference(API 参考)
=======================

Kernel space programs can use every feature of DAMON using below APIs.  All you
need to do is including ``damon.h``, which is located in ``include/linux/`` of
the source tree.
(内核空间程序可以通过以下 API 使用 DAMON 的全部功能。你只需包含位于源码树 ``include/linux/`` 目录下的 ``damon.h`` 即可。)

Structures(结构体)
==================

.. kernel-doc:: include/linux/damon.h


Functions(函数)
===============

.. kernel-doc:: mm/damon/core.c
