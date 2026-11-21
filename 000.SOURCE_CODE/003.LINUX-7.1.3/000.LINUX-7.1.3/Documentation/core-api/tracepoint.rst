=====================================================
The Linux Kernel Tracepoint API(Linux 内核跟踪点 API)
=====================================================

:Author: Jason Baron
(作者: Jason Baron)
:Author: William Cohen
(作者: William Cohen)

Introduction(简介)
==================

Tracepoints are static probe points that are located in strategic points
throughout the kernel. 'Probes' register/unregister with tracepoints via
a callback mechanism. The 'probes' are strictly typed functions that are
passed a unique set of parameters defined by each tracepoint.
(跟踪点 (tracepoint) 是位于内核中各个关键位置的静态探测点。"探针" (probe)
通过回调机制向跟踪点注册/注销。"探针"是严格类型化的函数，接收由每个跟踪
点定义的一组独特参数。)

From this simple callback mechanism, 'probes' can be used to profile,
debug, and understand kernel behavior. There are a number of tools that
provide a framework for using 'probes'. These tools include Systemtap,
ftrace, and LTTng.
(基于这一简单的回调机制，"探针"可用于剖析、调试和理解内核行为。有许多
工具为使用"探针"提供了框架，这些工具包括 Systemtap、ftrace 和 LTTng。)

Tracepoints are defined in a number of header files via various macros.
Thus, the purpose of this document is to provide a clear accounting of
the available tracepoints. The intention is to understand not only what
tracepoints are available but also to understand where future
tracepoints might be added.
(跟踪点通过各种宏定义在多个头文件中。因此，本文档的目的在于清楚地梳理
可用的跟踪点。其意图不仅是了解有哪些跟踪点可用，还要了解未来可能在何处
添加跟踪点。)

The API presented has functions of the form:
``trace_tracepointname(function parameters)``. These are the tracepoints
callbacks that are found throughout the code. Registering and
unregistering probes with these callback sites is covered in the
``Documentation/trace/*`` directory.
(这里展示的 API 具有如下形式的函数：
``trace_tracepointname(function parameters)`` 。这些是遍布代码中的跟踪点
回调。在 ``Documentation/trace/*`` 目录中介绍了如何在这些回调位置注册和
注销探针。)

IRQ(中断)
=========

.. kernel-doc:: include/trace/events/irq.h
   :internal:

SIGNAL(信号)
============

.. kernel-doc:: include/trace/events/signal.h
   :internal:

Block IO(块 I/O)
================

.. kernel-doc:: include/trace/events/block.h
   :internal:

Workqueue(工作队列)
===================

.. kernel-doc:: include/trace/events/workqueue.h
   :internal:
