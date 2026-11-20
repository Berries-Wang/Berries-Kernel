.. SPDX-License-Identifier: GPL-2.0

=========================================================
Debugging advice for driver development(驱动开发调试建议)
=========================================================

This document serves as a general starting point and lookup for debugging
device drivers.
While this guide focuses on debugging that requires re-compiling the
module/kernel, the :doc:`userspace debugging guide
</process/debugging/userspace_debugging_guide>` will guide
you through tools like dynamic debug, ftrace and other tools useful for
debugging issues and behavior.
For general debugging advice, see the :doc:`general advice document
</process/debugging/index>`.
(本文档是调试设备驱动的通用起点与查阅手册。本指南聚焦于需要重新编译模块/内核的调试，而 :doc:`userspace debugging guide
</process/debugging/userspace_debugging_guide>` 将带你了解 dynamic debug、ftrace 等对调试问题与行为有用的工具。
一般性调试建议见 :doc:`general advice document
</process/debugging/index>`。)

.. contents::
    :depth: 3

The following sections show you the available tools.
(以下各节介绍可用的工具。)

printk() & friends(printk() 及其同类)
-------------------------------------

These are derivatives of printf() with varying destinations and support for
being dynamically turned on or off, or lack thereof.
(这些是 printf() 的各种衍生版本，具有不同的输出目的地，并 (或缺少) 支持动态开启或关闭。)

Simple printk()(简单 printk())
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The classic, can be used to great effect for quick and dirty development
of new modules or to extract arbitrary necessary data for troubleshooting.
(经典方式，可用于快速草率地开发新模块，或为故障排查提取任意所需的数据，效果显著。)

Prerequisite: ``CONFIG_PRINTK`` (usually enabled by default)
(前提条件：``CONFIG_PRINTK`` (通常默认启用))

**Pros**:
(**优点**：)

- No need to learn anything, simple to use
  (无需学习任何东西，使用简单)
- Easy to modify exactly to your needs (formatting of the data (See:
  :doc:`/core-api/printk-formats`), visibility in the log)
  (容易按需精确修改 (数据的格式化 (见：:doc:`/core-api/printk-formats`)、在日志中的可见性))
- Can cause delays in the execution of the code (beneficial to confirm whether
  timing is a factor)
  (可能造成代码执行延迟 (有利于确认时序是否是影响因素))

**Cons**:
(**缺点**：)

- Requires rebuilding the kernel/module
  (需要重新构建内核/模块)
- Can cause delays in the execution of the code (which can cause issues to be
  not reproducible)
  (可能造成代码执行延迟 (这可能导致问题无法复现))

For the full documentation see :doc:`/core-api/printk-basics`
(完整文档见 :doc:`/core-api/printk-basics` 。)

Trace_printk
~~~~~~~~~~~~

Prerequisite: ``CONFIG_DYNAMIC_FTRACE`` & ``#include <linux/ftrace.h>``
(前提条件：``CONFIG_DYNAMIC_FTRACE`` 与 ``#include <linux/ftrace.h>``)

It is a tiny bit less comfortable to use than printk(), because you will have
to read the messages from the trace file (See: :ref:`read_ftrace_log`
instead of from the kernel log, but very useful when printk() adds unwanted
delays into the code execution, causing issues to be flaky or hidden.)
(它比 printk() 稍微不那么好用，因为你必须从跟踪文件读取消息 (见：:ref:`read_ftrace_log`
)，而不是从内核日志读取；但当 printk() 会给代码执行加入不想要的延迟、导致问题时隐时现或被隐藏时，它非常有用。)

If the processing of this still causes timing issues then you can try
trace_puts().
(如果处理这些信息仍然造成时序问题，可以尝试 trace_puts()。)

For the full Documentation see trace_printk()
(完整文档见 trace_printk()。)

dev_dbg
~~~~~~~

Print statement, which can be targeted by
:ref:`process/debugging/userspace_debugging_guide:dynamic debug` that contains
additional information about the device used within the context.
(打印语句，可被
:ref:`process/debugging/userspace_debugging_guide:dynamic debug` 定向控制，其中包含上下文中所用设备的附加信息。)

**When is it appropriate to leave a debug print in the code?**
(**什么时候适合把调试打印留在代码中？**)

Permanent debug statements have to be useful for a developer to troubleshoot
driver misbehavior. Judging that is a bit more of an art than a science, but
some guidelines are in the :ref:`Coding style guidelines
<process/coding-style:13) printing kernel messages>`. In almost all cases the
debug statements shouldn't be upstreamed, as a working driver is supposed to be
silent.
(永久的调试语句必须对开发者排查驱动异常行为有用。对此的判断更像是一门艺术而非科学，但 :ref:`Coding style guidelines
<process/coding-style:13) printing kernel messages>` 中有一些指导原则。几乎所有情况下调试语句都不应上游提交，因为正常工作的驱动应当是安静的。)

Custom printk(自定义 printk)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Example::

  #define core_dbg(fmt, arg...) do { \
	  if (core_debug) \
		  printk(KERN_DEBUG pr_fmt("core: " fmt), ## arg); \
	  } while (0)

(示例：)

**When should you do this?**
(**什么时候应该这样做？**)

It is better to just use a pr_debug(), which can later be turned on/off with
dynamic debug. Additionally, a lot of drivers activate these prints via a
variable like ``core_debug`` set by a module parameter. However, Module
parameters `are not recommended anymore
<https://lore.kernel.org/all/2024032757-surcharge-grime-d3dd@gregkh>`_.
(最好直接使用 pr_debug()，它之后可以用 dynamic debug 开关。此外，很多驱动通过由模块参数设置的 ``core_debug`` 之类变量来激活这些打印。不过，模块参数 `are not recommended anymore
<https://lore.kernel.org/all/2024032757-surcharge-grime-d3dd@gregkh>`_ (已不再被推荐)。)

Ftrace(Ftrace)
--------------

Creating a custom Ftrace tracepoint(创建自定义 Ftrace 跟踪点)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A tracepoint adds a hook into your code that will be called and logged when the
tracepoint is enabled. This can be used, for example, to trace hitting a
conditional branch or to dump the internal state at specific points of the code
flow during a debugging session.
(跟踪点 (tracepoint) 在你的代码中加入一个钩子，当跟踪点被启用时它会被调用并记录。例如，可以用它来跟踪某个条件分支的命中情况，或在调试会话期间在代码流的特定点转储内部状态。)

Here is a basic description of :ref:`how to implement new tracepoints
<trace/tracepoints:usage>`.
(这里是 :ref:`how to implement new tracepoints
<trace/tracepoints:usage>` 的基本描述。)

For the full event tracing documentation see :doc:`/trace/events`
(完整的事件跟踪文档见 :doc:`/trace/events` 。)

For the full Ftrace documentation see :doc:`/trace/ftrace`
(完整的 Ftrace 文档见 :doc:`/trace/ftrace` 。)

DebugFS(DebugFS)
----------------

Prerequisite: ``CONFIG_DEBUG_FS` & `#include <linux/debugfs.h>``
(前提条件：``CONFIG_DEBUG_FS` 与 `#include <linux/debugfs.h>``)

DebugFS differs from the other approaches of debugging, as it doesn't write
messages to the kernel log nor add traces to the code. Instead it allows the
developer to handle a set of files.
With these files you can either store values of variables or make
register/memory dumps or you can make these files writable and modify
values/settings in the driver.
(DebugFS 与其他调试方式不同：它不向内核日志写消息，也不向代码添加跟踪。它允许开发者操纵一组文件。
借助这些文件，你可以存储变量的值、进行寄存器/内存转储，或者把这些文件设为可写并在驱动中修改值/设置。)

Possible use-cases among others:
(除其他外，可能的用例：)

- Store register values
  (存储寄存器值)
- Keep track of variables
  (跟踪变量)
- Store errors
  (存储错误)
- Store settings
  (存储设置)
- Toggle a setting like debug on/off
  (切换诸如 debug 开/关之类的设置)
- Error injection
  (错误注入)

This is especially useful, when the size of a data dump would be hard to digest
as part of the general kernel log (for example when dumping raw bitstream data)
or when you are not interested in all the values all the time, but with the
possibility to inspect them.
(当数据转储的规模难以作为一般内核日志的一部分来消化时 (例如转储原始码流数据时)，或者当你并非一直对所有值都感兴趣、但希望有查看它们的可能时，这尤其有用。)

The general idea is:
(总体思路是：)

- Create a directory during probe (``struct dentry *parent =
  debugfs_create_dir("my_driver", NULL);``)
  (在 probe 期间创建目录 (``struct dentry *parent =
  debugfs_create_dir("my_driver", NULL);``))
- Create a file (``debugfs_create_u32("my_value", 444, parent, &my_variable);``)
  (创建文件 (``debugfs_create_u32("my_value", 444, parent, &my_variable);``))

  - In this example the file is found in
    ``/sys/kernel/debug/my_driver/my_value`` (with read permissions for
    user/group/all)
    (在本例中，该文件位于
    ``/sys/kernel/debug/my_driver/my_value`` (对 user/group/all 均有读权限))
  - any read of the file will return the current contents of the variable
    ``my_variable``
    (对该文件的任何读取都会返回变量 ``my_variable`` 的当前内容)

- Clean up the directory when removing the device
  (``debugfs_remove(parent);``)
  (移除设备时清理目录
  (``debugfs_remove(parent);``))

For the full documentation see :doc:`/filesystems/debugfs`.
(完整文档见 :doc:`/filesystems/debugfs` 。)

KASAN, UBSAN, lockdep and other error checkers(KASAN、UBSAN、lockdep 及其他错误检查器)
--------------------------------------------------------------------------------------

KASAN (Kernel Address Sanitizer)(KASAN (内核地址消毒器))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prerequisite: ``CONFIG_KASAN``
(前提条件：``CONFIG_KASAN``)

KASAN is a dynamic memory error detector that helps to find use-after-free and
out-of-bounds bugs. It uses compile-time instrumentation to check every memory
access.
(KASAN 是一个动态内存错误检测器，帮助发现释放后使用 (use-after-free) 与越界 (out-of-bounds) 缺陷。它使用编译时插桩检查每一次内存访问。)

For the full documentation see :doc:`/dev-tools/kasan`.
(完整文档见 :doc:`/dev-tools/kasan` 。)

UBSAN (Undefined Behavior Sanitizer)(UBSAN (未定义行为消毒器))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prerequisite: ``CONFIG_UBSAN``
(前提条件：``CONFIG_UBSAN``)

UBSAN relies on compiler instrumentation and runtime checks to detect undefined
behavior. It is designed to find a variety of issues, including signed integer
overflow, array index out of bounds, and more.
(UBSAN 依靠编译器插桩与运行时检查来检测未定义行为。它旨在发现各种问题，包括有符号整数溢出、数组索引越界等等。)

For the full documentation see :doc:`/dev-tools/ubsan`
(完整文档见 :doc:`/dev-tools/ubsan` 。)

lockdep (Lock Dependency Validator)(lockdep (锁依赖验证器))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prerequisite: ``CONFIG_DEBUG_LOCKDEP``
(前提条件：``CONFIG_DEBUG_LOCKDEP``)

lockdep is a runtime lock dependency validator that detects potential deadlocks
and other locking-related issues in the kernel.
It tracks lock acquisitions and releases, building a dependency graph that is
analyzed for potential deadlocks.
lockdep is especially useful for validating the correctness of lock ordering in
the kernel.
(lockdep 是一个运行时锁依赖验证器，检测内核中潜在的死锁及其他与锁相关的问题。
它跟踪锁的获取与释放，构建依赖图，并分析其中潜在的死锁。
lockdep 对验证内核中锁顺序的正确性尤其有用。)

PSI (Pressure stall information tracking)(PSI (压力停顿信息跟踪))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prerequisite: ``CONFIG_PSI``
(前提条件：``CONFIG_PSI``)

PSI is a measurement tool to identify excessive overcommits on hardware
resources, that can cause performance disruptions or even OOM kills.
(PSI 是一种测量工具，用于识别对硬件资源的过度超售 (overcommit)，这类超售可能导致性能中断甚至 OOM 杀死进程。)

device coredump(设备 coredump)
------------------------------

Prerequisite: ``CONFIG_DEV_COREDUMP`` & ``#include <linux/devcoredump.h>``
(前提条件：``CONFIG_DEV_COREDUMP`` 与 ``#include <linux/devcoredump.h>``)

Provides the infrastructure for a driver to provide arbitrary data to userland.
It is most often used in conjunction with udev or similar userland application
to listen for kernel uevents, which indicate that the dump is ready. Udev has
rules to copy that file somewhere for long-term storage and analysis, as by
default, the data for the dump is automatically cleaned up after a default
5 minutes. That data is analyzed with driver-specific tools or GDB.
(为驱动提供向用户态提交任意数据的基础设施。它最常与 udev 或类似用户态应用配合使用，监听表明转储已就绪的内核 uevent。udev 有规则把该文件复制到某处做长期存储与分析，因为默认情况下转储数据会在默认的 5 分钟后自动清理。这些数据用驱动专用工具或 GDB 分析。)

A device coredump can be created with a vmalloc area, with read/free
methods, or as a scatter/gather list.
(设备 coredump 可以用 vmalloc 区域创建，也可以带 read/free 方法创建，或以散布/聚集列表 (scatter/gather list) 的形式创建。)

You can find an example implementation at:
(你可以在以下位置找到示例实现：)

`drivers/media/platform/qcom/venus/core.c
<https://elixir.bootlin.com/linux/v6.11.6/source/drivers/media/platform/qcom/venus/core.c#L30>`__,
in the Bluetooth HCI layer, in several wireless drivers, and in several
DRM drivers.
(`drivers/media/platform/qcom/venus/core.c
<https://elixir.bootlin.com/linux/v6.11.6/source/drivers/media/platform/qcom/venus/core.c#L30>`__ ，以及蓝牙 HCI 层、若干无线驱动和若干 DRM 驱动中。)

devcoredump interfaces(devcoredump 接口)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: include/linux/devcoredump.h

.. kernel-doc:: drivers/base/devcoredump.c

**Copyright** ©2024 : Collabora
(**版权所有** ©2024 ： Collabora)
