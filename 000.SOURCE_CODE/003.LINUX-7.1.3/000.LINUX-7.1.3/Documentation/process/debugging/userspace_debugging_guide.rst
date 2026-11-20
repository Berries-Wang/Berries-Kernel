.. SPDX-License-Identifier: GPL-2.0

============================================
Userspace debugging advice(用户空间调试建议)
============================================

This document provides a brief overview of common tools to debug the Linux
Kernel from userspace.
For debugging advice aimed at driver developers go :doc:`here
</process/debugging/driver_development_debugging_guide>`.
For general debugging advice, see :doc:`general advice document
</process/debugging/index>`.
(本文档简要概述了从用户空间调试 Linux 内核的常用工具。面向驱动开发者的调试建议见
driver_development_debugging_guide 文档，一般性调试建议见 process/debugging/index
下的通用建议文档。)

.. contents::
    :depth: 3

The following sections show you the available tools.
(以下各节介绍可用的工具。)

Dynamic debug(动态调试)
-----------------------

Mechanism to filter what ends up in the kernel log by dis-/en-abling log
messages.
(这是一种通过禁用/启用日志消息来过滤最终写入内核日志的内容的机制。)

Prerequisite: ``CONFIG_DYNAMIC_DEBUG``
(前提条件：启用 CONFIG_DYNAMIC_DEBUG。)

Dynamic debug is only able to target:
(动态调试只能作用于以下打印语句：)

- pr_debug()
- dev_dbg()
- print_hex_dump_debug()
- print_hex_dump_bytes()

Therefore the usability of this tool is, as of now, quite limited as there is
no uniform rule for adding debug prints to the codebase, resulting in a variety
of ways these prints are implemented.
(因此，就目前而言，这个工具的可用性相当有限，因为向代码库添加调试打印没有统一的
规则，导致这些打印的实现方式五花八门。)

Also, note that most debug statements are implemented as a variation of
dprintk(), which have to be activated via a parameter in respective module,
dynamic debug is unable to do that step for you.
(另外请注意，大多数调试语句都是以 dprintk() 的某种变体实现的，需要通过相应模块中
的参数来激活，而动态调试无法替你完成这一步。)

Here is one example, that enables all available pr_debug()'s within the file::

  $ alias ddcmd='echo $* > /proc/dynamic_debug/control'
  $ ddcmd '-p; file v4l2-h264.c +p'
  $ grep =p /proc/dynamic_debug/control
   drivers/media/v4l2-core/v4l2-h264.c:372 [v4l2_h264]print_ref_list_b =p
   "ref_pic_list_b%u (cur_poc %u%c) %s"
   drivers/media/v4l2-core/v4l2-h264.c:333 [v4l2_h264]print_ref_list_p =p
   "ref_pic_list_p (cur_poc %u%c) %s\n"

(下面这个例子启用了该文件中所有可用的 pr_debug()：)

**When should you use this over Ftrace ?**
(什么情况下应当使用它而不是 Ftrace？)

- When the code contains one of the valid print statements (see above) or when
  you have added multiple pr_debug() statements during development
  (当代码中包含上述有效打印语句之一时，或者在开发过程中你已经添加了多条
  pr_debug() 语句时。)
- When timing is not an issue, meaning if multiple pr_debug() statements in
  the code won't cause delays
  (当时间开销不是问题时，即代码中的多条 pr_debug() 语句不会造成延迟时。)
- When you care more about receiving specific log messages than tracing the
  pattern of how a function is called
  (当你更关心收到特定的日志消息，而不是追踪函数的调用模式时。)

For the full documentation see :doc:`/admin-guide/dynamic-debug-howto`
(完整文档见 Documentation/admin-guide/dynamic-debug-howto.rst。)

Ftrace(Ftrace 跟踪工具)
-----------------------

Prerequisite: ``CONFIG_DYNAMIC_FTRACE``
(前提条件：启用 CONFIG_DYNAMIC_FTRACE。)

This tool uses the tracefs file system for the control files and output files.
That file system will be mounted as a ``tracing`` directory, which can be found
in either ``/sys/kernel/`` or ``/sys/debug/kernel/``.
(该工具使用 tracefs 文件系统来存放控制文件和输出文件。该文件系统会被挂载为
``tracing`` 目录，可以在 ``/sys/kernel/`` 或 ``/sys/debug/kernel/`` 中找到它。)

Some of the most important operations for debugging are:
(以下是一些最重要的调试操作：)

- You can perform a function trace by adding a function name to the
  ``set_ftrace_filter`` file (which accepts any function name found within the
  ``available_filter_functions`` file) or you can specifically disable certain
  functions by adding their names to the ``set_ftrace_notrace`` file (more info
  at: :ref:`trace/ftrace:dynamic ftrace`).
  (可以向 ``set_ftrace_filter`` 文件添加函数名来执行函数跟踪(该文件接受
  ``available_filter_functions`` 文件中出现的任何函数名)，也可以把某些函数的名字
  加入 ``set_ftrace_notrace`` 文件来明确禁用它们；更多信息见 trace/ftrace 文档的
  "dynamic ftrace" 一节。)
- In order to find out where calls originate from you can activate the
  ``func_stack_trace`` option under ``options/func_stack_trace``.
  (要查明调用源自何处，可以启用 ``options/func_stack_trace`` 下的
  ``func_stack_trace`` 选项。)
- Tracing the children of a function call and showing the return values are
  possible by adding the desired function in the ``set_graph_function`` file
  (requires config ``FUNCTION_GRAPH_RETVAL``); more info at
  :ref:`trace/ftrace:dynamic ftrace with the function graph tracer`.
  (通过在 ``set_graph_function`` 文件中加入想要的函数，可以跟踪函数调用的子调用并
  显示返回值(需要配置 FUNCTION_GRAPH_RETVAL)；更多信息见 trace/ftrace 文档的
  "dynamic ftrace with the function graph tracer" 一节。)

For the full Ftrace documentation see :doc:`/trace/ftrace`
(Ftrace 完整文档见 Documentation/trace/ftrace.rst。)

Or you could also trace for specific events by :ref:`using event tracing
<trace/events:2. using event tracing>`, which can be defined as described here:
:ref:`Creating a custom Ftrace tracepoint
<process/debugging/driver_development_debugging_guide:ftrace>`.
(你也可以通过事件跟踪(event tracing)来跟踪特定事件，事件的定义方式见
trace/events 文档 "2. using event tracing" 一节；创建自定义 Ftrace
跟踪点的方法见 driver_development_debugging_guide 文档的 ftrace 一节。)

For the full Ftrace event tracing documentation see :doc:`/trace/events`
(Ftrace 事件跟踪的完整文档见 Documentation/trace/events.rst。)

.. _read_ftrace_log:

Reading the ftrace log(读取 ftrace 日志)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``trace`` file can be read just like any other file (``cat``, ``tail``,
``head``, ``vim``, etc.), the size of the file is limited by the
``buffer_size_kb`` (``echo 1000 > buffer_size_kb``). The
:ref:`trace/ftrace:trace_pipe` will behave similarly to the ``trace`` file, but
whenever you read from the file the content is consumed.
(``trace`` 文件可以像普通文件一样读取(用 ``cat``、``tail``、``head``、``vim``
等)，文件大小受 ``buffer_size_kb`` 限制(``echo 1000 > buffer_size_kb``)。
trace_pipe(见 trace/ftrace 文档)的行为与 ``trace`` 文件类似，但每次从该文件读取
时，其内容都会被消费掉。)

Kernelshark(Kernelshark 可视化工具)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A GUI interface to visualize the traces as a graph and list view from the
output of the `trace-cmd
<https://git.kernel.org/pub/scm/utils/trace-cmd/trace-cmd.git/>`__ application.
(这是一个 GUI 界面，可将 trace-cmd 应用程序的输出以图形和列表视图进行可视化。)

For the full documentation see `<https://kernelshark.org/Documentation.html>`__
(完整文档见 kernelshark.org 的 Documentation 页面。)

Perf & alternatives(Perf 及替代工具)
------------------------------------

The tools mentioned above provide ways to inspect kernel code, results,
variable values, etc. Sometimes you have to find out first where to look and
for those cases, a box of performance tracking tools can help you to frame the
issue.
(上文提到的工具提供了检查内核代码、结果、变量值等的手段。有时你得先弄清该从哪里查
起，这时一组性能跟踪工具可以帮助你框定问题。)

Why should you do a performance analysis?(为什么要做性能分析？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A performance analysis is a good first step when among other reasons:
(在诸多原因中，遇到以下情形时，性能分析是一个很好的第一步：)

- you cannot define the issue
  (你无法描述清楚该问题。)
- you do not know where it occurs
  (你不知道问题发生在哪里。)
- the running system should not be interrupted or it is a remote system, where
  you cannot install a new module/kernel
  (运行中的系统不应被打断，或者它是远程系统，你无法在上面安装新模块/内核。)

How to do a simple analysis with linux tools?(如何用 linux 工具做简单分析？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For the start of a performance analysis, you can start with the usual tools
like:
(开始性能分析时，可以先使用以下常用工具：)

- ``top`` / ``htop`` / ``atop`` (*get an overview of the system load, see
  spikes on specific processes*)
  (``top`` / ``htop`` / ``atop`` (*概览系统负载，观察特定进程的尖峰*))
- ``mpstat -P ALL`` (*look at the load distribution among CPUs*)
  (``mpstat -P ALL`` (*查看各 CPU 之间的负载分布*))
- ``iostat -x`` (*observe input and output devices utilization and performance*)
  (``iostat -x`` (*观察输入输出设备的利用率与性能*))
- ``vmstat`` (*overview of memory usage on the system*)
  (``vmstat`` (*概览系统内存使用情况*))
- ``pidstat`` (*similar to* ``vmstat`` *but per process, to dial it down to the
  target*)
  (``pidstat`` (*类似* ``vmstat`` *但按进程统计，以便进一步定位到目标*))
- ``strace -tp $PID`` (*once you know the process, you can figure out how it
  communicates with the Kernel*)
  (``strace -tp $PID`` (*一旦确定了进程，就能弄清它与内核如何通信*))

These should help to narrow down the areas to look at sufficiently.
(这些工具应当足以帮你充分缩小需要排查的范围。)

Diving deeper with perf(用 perf 深入分析)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The **perf** tool provides a series of metrics and events to further dial down
on issues.
(**perf** 工具提供了一系列指标和事件，用于进一步缩小问题范围。)

Prerequisite: build or install perf on your system
(前提条件：在你的系统上编译或安装 perf。)

Gather statistics data for finding all files starting with ``gcc`` in ``/usr``::

  # perf stat -d find /usr -name 'gcc*' | wc -l

   Performance counter stats for 'find /usr -name gcc*':

     1277.81 msec    task-clock             #    0.997 CPUs utilized
     9               context-switches       #    7.043 /sec
     1               cpu-migrations         #    0.783 /sec
     704             page-faults            #  550.943 /sec
     766548897       cycles                 #    0.600 GHz                         (97.15%)
     798285467       instructions           #    1.04  insn per cycle              (97.15%)
     57582731        branches               #   45.064 M/sec                       (2.85%)
     3842573         branch-misses          #    6.67% of all branches             (97.15%)
     281616097       L1-dcache-loads        #  220.390 M/sec                       (97.15%)
     4220975         L1-dcache-load-misses  #    1.50% of all L1-dcache accesses   (97.15%)
     <not supported> LLC-loads
     <not supported> LLC-load-misses

   1.281746009 seconds time elapsed

   0.508796000 seconds user
   0.773209000 seconds sys


  52

(收集统计数据，用于查找 /usr 下所有以 ``gcc`` 开头的文件：)

The availability of events and metrics depends on the system you are running.
(事件和指标的可用性取决于你所运行的系统。)

For the full documentation see
`<https://perf.wiki.kernel.org/index.php/Main_Page>`__
(完整文档见 perf.wiki.kernel.org 的 Main_Page 页面。)

Perfetto(Perfetto 性能分析套件)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A set of tools to measure and analyze how well applications and systems perform.
You can use it to:
(一套用于度量和分析应用程序与系统性能表现的工具。可以用它来：)

* identify bottlenecks
  (识别瓶颈。)
* optimize code
  (优化代码。)
* make software run faster and more efficiently.
  (让软件运行得更快、更高效。)

**What is the difference between perfetto and perf?**
(perfetto 和 perf 有什么区别？)

* perf is tool as part of and specialized for the Linux Kernel and has CLI user
  interface.
  (perf 是 Linux 内核自带的专用工具，提供命令行用户界面。)
* perfetto cross-platform performance analysis stack, has extended
  functionality into userspace and provides a WEB user interface.
  (perfetto 是跨平台的性能分析技术栈，其功能已扩展到用户空间，并提供 WEB
  用户界面。)

For the full documentation see `<https://perfetto.dev/docs/>`__
(完整文档见 perfetto.dev/docs。)

Kernel panic analysis tools(内核崩溃分析工具)
---------------------------------------------

  To capture the crash dump please use ``Kdump`` & ``Kexec``. Below you can find
  some advice for analysing the data.
  (要捕获崩溃转储，请使用 ``Kdump`` 与 ``Kexec``。下面给出一些分析这些数据的建议。)

  For the full documentation see the :doc:`/admin-guide/kdump/kdump`
  (完整文档见 Documentation/admin-guide/kdump/kdump.rst。)

  In order to find the corresponding line in the code you can use `faddr2line
  <https://elixir.bootlin.com/linux/v6.11.6/source/scripts/faddr2line>`__; note
  that you need to enable ``CONFIG_DEBUG_INFO`` for that to work.
  (要找到代码中对应的行，可以使用 faddr2line 脚本；注意，需要启用
  CONFIG_DEBUG_INFO 才能使其正常工作。)

  An alternative to using ``faddr2line`` is the use of ``objdump`` (and its
  derivatives for the different platforms like ``aarch64-linux-gnu-objdump``).
  Take this line as an example:
  (除了 ``faddr2line`` 之外，也可以使用 ``objdump``(以及面向不同平台的衍生工具，
  如 ``aarch64-linux-gnu-objdump``)。以下面这一行为例：)

  ``[  +0.000240]  rkvdec_device_run+0x50/0x138 [rockchip_vdec]``.
  (即这一行。)

  We can find the corresponding line of code by executing::

    aarch64-linux-gnu-objdump -dS drivers/staging/media/rkvdec/rockchip-vdec.ko | grep rkvdec_device_run\>: -A 40
    0000000000000ac8 <rkvdec_device_run>:
     ac8:	d503201f 	nop
     acc:	d503201f 	nop
    {
     ad0:	d503233f 	paciasp
     ad4:	a9bd7bfd 	stp	x29, x30, [sp, #-48]!
     ad8:	910003fd 	mov	x29, sp
     adc:	a90153f3 	stp	x19, x20, [sp, #16]
     ae0:	a9025bf5 	stp	x21, x22, [sp, #32]
        const struct rkvdec_coded_fmt_desc *desc = ctx->coded_fmt_desc;
     ae4:	f9411814 	ldr	x20, [x0, #560]
        struct rkvdec_dev *rkvdec = ctx->dev;
     ae8:	f9418015 	ldr	x21, [x0, #768]
        if (WARN_ON(!desc))
     aec:	b4000654 	cbz	x20, bb4 <rkvdec_device_run+0xec>
        ret = pm_runtime_resume_and_get(rkvdec->dev);
     af0:	f943d2b6 	ldr	x22, [x21, #1952]
        ret = __pm_runtime_resume(dev, RPM_GET_PUT);
     af4:	aa0003f3 	mov	x19, x0
     af8:	52800081 	mov	w1, #0x4                   	// #4
     afc:	aa1603e0 	mov	x0, x22
     b00:	94000000 	bl	0 <__pm_runtime_resume>
        if (ret < 0) {
     b04:	37f80340 	tbnz	w0, #31, b6c <rkvdec_device_run+0xa4>
        dev_warn(rkvdec->dev, "Not good\n");
     b08:	f943d2a0 	ldr	x0, [x21, #1952]
     b0c:	90000001 	adrp	x1, 0 <rkvdec_try_ctrl-0x8>
     b10:	91000021 	add	x1, x1, #0x0
     b14:	94000000 	bl	0 <_dev_warn>
        *bad = 1;
     b18:	d2800001 	mov	x1, #0x0                   	// #0
     ...

  (我们可以通过执行以下命令找到对应的代码行：)

  Meaning, in this line from the crash dump::

    [  +0.000240]  rkvdec_device_run+0x50/0x138 [rockchip_vdec]

  (也就是说，对于崩溃日志中的这一行：)

  I can take the ``0x50`` as offset, which I have to add to the base address
  of the corresponding function, which I find in this line::

    0000000000000ac8 <rkvdec_device_run>:

  (我可以把 ``0x50`` 当作偏移量，把它加到对应函数的基地址上；基地址可以从下面
  这一行中找到：)

  The result of ``0xac8 + 0x50 = 0xb18``
  And when I search for that address within the function I get the
  following line::

    *bad = 1;
    b18:      d2800001        mov     x1, #0x0

  (结果是 ``0xac8 + 0x50 = 0xb18``。当我在函数内搜索该地址时，得到以下行：)

**Copyright** ©2024 : Collabora
(**版权所有** ©2024 : Collabora)
