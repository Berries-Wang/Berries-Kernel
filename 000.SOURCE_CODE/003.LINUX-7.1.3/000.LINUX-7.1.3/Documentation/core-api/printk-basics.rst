.. SPDX-License-Identifier: GPL-2.0

=====================================================
Message logging with printk(使用 printk 记录消息日志)
=====================================================

printk() is one of the most widely known functions in the Linux kernel. It's the
standard tool we have for printing messages and usually the most basic way of
tracing and debugging. If you're familiar with printf(3) you can tell printk()
is based on it, although it has some functional differences:
(printk() 是 Linux 内核中最广为人知的函数之一。它是我们打印消息的标准工具，
通常也是最基本的追踪和调试手段。如果你熟悉 printf(3)，就能看出 printk() 是
基于它的，尽管存在一些功能差异：)

  - printk() messages can specify a log level.
    (printk() 消息可以指定日志级别。)

  - the format string, while largely compatible with C99, doesn't follow the
    exact same specification. It has some extensions and a few limitations
    (no ``%n`` or floating point conversion specifiers). See :ref:`How to get
    printk format specifiers right <printk-specifiers>`.
    (格式字符串虽然在很大程度上与 C99 兼容，但并不完全遵循相同的规范。它有
    一些扩展和少量限制（不支持 ``%n`` 或浮点数转换说明符）。参见 :ref:`How
    to get printk format specifiers right <printk-specifiers>` 。)

All printk() messages are printed to the kernel log buffer, which is a ring
buffer exported to userspace through /dev/kmsg. The usual way to read it is
using ``dmesg``.
(所有 printk() 消息都会打印到内核日志缓冲区，这是一个通过 /dev/kmsg 导出到
用户空间的环形缓冲区。读取它的常用方法是使用 ``dmesg``。)

printk() is typically used like this::

  printk(KERN_INFO "Message: %s\n", arg);

(printk() 的典型用法如下：)

where ``KERN_INFO`` is the log level (note that it's concatenated to the format
string, the log level is not a separate argument). The available log levels are:
(其中 ``KERN_INFO`` 是日志级别（注意它被拼接在格式字符串上，日志级别并不是
一个独立的参数）。可用的日志级别有：)

+----------------+--------+-----------------------------------------------+
| Name           | String |  Alias function                               |
|(名称)          |(字符串)|(别名函数)                                     |
+================+========+===============================================+
| KERN_EMERG     | "0"    | pr_emerg()                                    |
+----------------+--------+-----------------------------------------------+
| KERN_ALERT     | "1"    | pr_alert()                                    |
+----------------+--------+-----------------------------------------------+
| KERN_CRIT      | "2"    | pr_crit()                                     |
+----------------+--------+-----------------------------------------------+
| KERN_ERR       | "3"    | pr_err()                                      |
+----------------+--------+-----------------------------------------------+
| KERN_WARNING   | "4"    | pr_warn()                                     |
+----------------+--------+-----------------------------------------------+
| KERN_NOTICE    | "5"    | pr_notice()                                   |
+----------------+--------+-----------------------------------------------+
| KERN_INFO      | "6"    | pr_info()                                     |
+----------------+--------+-----------------------------------------------+
| KERN_DEBUG     | "7"    | pr_debug() and pr_devel() if DEBUG is defined |
+----------------+--------+-----------------------------------------------+
| KERN_DEFAULT   | ""     |                                               |
+----------------+--------+-----------------------------------------------+
| KERN_CONT      | "c"    | pr_cont()                                     |
+----------------+--------+-----------------------------------------------+


The log level specifies the importance of a message. The kernel decides whether
to show the message immediately (printing it to the current console) depending
on its log level and the current *console_loglevel* (a kernel variable). If the
message priority is higher (lower log level value) than the *console_loglevel*
the message will be printed to the console.
(日志级别指定消息的重要性。内核根据消息的日志级别和当前的
*console_loglevel*（一个内核变量）来决定是否立即显示该消息（打印到当前
控制台）。如果消息的优先级高于（日志级别数值低于）*console_loglevel*，
该消息将被打印到控制台。)

If the log level is omitted, the message is printed with ``KERN_DEFAULT``
level.
(如果省略日志级别，消息将以 ``KERN_DEFAULT`` 级别打印。)

You can check the current *console_loglevel* with::

  $ cat /proc/sys/kernel/printk
  4        4        1        7

(你可以用以下命令检查当前的 *console_loglevel*：)

The result shows the *current*, *default*, *minimum* and *boot-time-default* log
levels.
(结果依次显示 *current*（当前）、*default*（默认）、*minimum*（最小）和
*boot-time-default*（启动时默认）日志级别。)

To change the current console_loglevel simply write the desired level to
``/proc/sys/kernel/printk``. For example, to print all messages to the console::

  # echo 8 > /proc/sys/kernel/printk

(要更改当前的 console_loglevel，只需把想要的级别写入
``/proc/sys/kernel/printk``。例如，把所有消息打印到控制台：)

Another way, using ``dmesg``::

  # dmesg -n 5

(另一种方法是使用 ``dmesg``：)

sets the console_loglevel to print KERN_WARNING (4) or more severe messages to
console. See ``dmesg(1)`` for more information.
(这将把 console_loglevel 设置为向控制台打印 KERN_WARNING (4) 或更严重的
消息。更多信息参见 ``dmesg(1)``。)

As an alternative to printk() you can use the ``pr_*()`` aliases for
logging. This family of macros embed the log level in the macro names. For
example::

  pr_info("Info message no. %d\n", msg_num);

(作为 printk() 的替代，你可以使用 ``pr_*()`` 别名来记录日志。这一族宏把
日志级别嵌入在宏名中。例如：)

prints a ``KERN_INFO`` message.
(它打印一条 ``KERN_INFO`` 消息。)

Besides being more concise than the equivalent printk() calls, they can use a
common definition for the format string through the pr_fmt() macro. For
instance, defining this at the top of a source file (before any ``#include``
directive)::

  #define pr_fmt(fmt) "%s:%s: " fmt, KBUILD_MODNAME, __func__

(除了比等价的 printk() 调用更简洁之外，它们还可以通过 pr_fmt() 宏为格式
字符串使用一个公共定义。例如，在源文件顶部（任何 ``#include`` 指令之前）
定义：)

would prefix every pr_*() message in that file with the module and function name
that originated the message.
(就会把该文件中的每条 pr_*() 消息加上产生该消息的模块名和函数名作为
前缀。)

For debugging purposes there are also two conditionally-compiled macros:
pr_debug() and pr_devel(), which are compiled-out unless ``DEBUG`` (or
also ``CONFIG_DYNAMIC_DEBUG`` in the case of pr_debug()) is defined.
(用于调试目的，还有两个条件编译的宏：pr_debug() 和 pr_devel()。除非定义了
``DEBUG``（对 pr_debug() 而言，还包括 ``CONFIG_DYNAMIC_DEBUG``），否则它们
不会被编译。)

Avoiding lockups from excessive printk() use(避免过度使用 printk() 导致的锁死)
==============================================================================

.. note::

   This section is relevant only for legacy console drivers (those not
   using the nbcon API) and !PREEMPT_RT kernels. Once all console drivers
   are updated to nbcon, this documentation can be removed.
   (本节仅适用于旧式控制台驱动（不使用 nbcon API 的驱动）以及 !PREEMPT_RT
   内核。一旦所有控制台驱动都更新到 nbcon，本节文档即可删除。)

Using ``printk()`` in hot paths (such as interrupt handlers, timer
callbacks, or high-frequency network receive routines) with legacy
consoles (e.g., ``console=ttyS0``) may cause lockups. Legacy consoles
synchronously acquire ``console_sem`` and block while flushing messages,
potentially disabling interrupts long enough to trigger hard or soft
lockup detectors.
(在热路径（例如中断处理程序、定时器回调或高频网络接收例程）中使用
``printk()`` 且配合旧式控制台（例如 ``console=ttyS0``）可能导致锁死。
旧式控制台会同步获取 ``console_sem`` 并在刷新消息期间阻塞，可能长时间
禁用中断，足以触发硬锁死或软锁死检测器。)

To avoid this:
(为避免这种情况：)

- Use rate-limited variants (e.g., ``pr_*_ratelimited()``) or one-time
  macros (e.g., ``pr_*_once()``) to reduce message frequency.
  (使用限流变体（例如 ``pr_*_ratelimited()``）或一次性宏（例如
  ``pr_*_once()``）来降低消息频率。)
- Assign lower log levels (e.g., ``KERN_DEBUG``) to non-essential messages
  and filter console output via ``console_loglevel``.
  (为非关键消息分配较低的日志级别（例如 ``KERN_DEBUG``），并通过
  ``console_loglevel`` 过滤控制台输出。)
- Use ``printk_deferred()`` to log messages immediately to the ringbuffer
  and defer console printing. This is a workaround for legacy consoles.
  (使用 ``printk_deferred()`` 把消息立即记录到环形缓冲区，并延迟控制台
  打印。这是针对旧式控制台的变通办法。)
- Port legacy console drivers to the non-blocking ``nbcon`` API (indicated
  by ``CON_NBCON``). This is the preferred solution, as nbcon consoles
  offload message printing to a dedicated kernel thread.
  (把旧式控制台驱动移植到非阻塞的 ``nbcon`` API（以 ``CON_NBCON`` 标识）。
  这是首选方案，因为 nbcon 控制台把消息打印工作卸载给一个专门的内核
  线程。)

For temporary debugging, ``trace_printk()`` can be used, but it must not
appear in mainline code. See ``Documentation/trace/debugging.rst`` for
more information.
(对于临时调试，可以使用 ``trace_printk()``，但它绝不能出现在主线代码中。
更多信息参见 ``Documentation/trace/debugging.rst``。)

If more permanent output is needed in a hot path, trace events can be used.
See ``Documentation/trace/events.rst`` and
``samples/trace_events/trace-events-sample.[ch]``.
(如果需要在热路径中输出更持久的信息，可以使用跟踪事件。参见
``Documentation/trace/events.rst`` 和
``samples/trace_events/trace-events-sample.[ch]``。)


Function reference(函数参考)
=============================

.. kernel-doc:: include/linux/printk.h
   :functions: printk pr_emerg pr_alert pr_crit pr_err pr_warn pr_notice pr_info
      pr_fmt pr_debug pr_devel pr_cont
