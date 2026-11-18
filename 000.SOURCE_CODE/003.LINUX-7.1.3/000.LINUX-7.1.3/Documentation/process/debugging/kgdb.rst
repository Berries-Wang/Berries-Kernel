======================================================================================
Using kgdb, kdb and the kernel debugger internals(使用 kgdb、kdb 与内核调试器内部机制)
======================================================================================

:Author: Jason Wessel
  (作者: Jason Wessel)

Introduction(简介)
==================

The kernel has two different debugger front ends (kdb and kgdb) which
interface to the debug core. It is possible to use either of the
debugger front ends and dynamically transition between them if you
configure the kernel properly at compile and runtime.
(内核有两个不同的调试器前端 (kdb 和 kgdb)，它们都与调试核心 (debug core) 对接。只要在编译期和运行期正确配置内核，就可以任选其中一个前端，并在两者之间动态切换。)

Kdb is simplistic shell-style interface which you can use on a system
console with a keyboard or serial console. You can use it to inspect
memory, registers, process lists, dmesg, and even set breakpoints to
stop in a certain location. Kdb is not a source level debugger, although
you can set breakpoints and execute some basic kernel run control. Kdb
is mainly aimed at doing some analysis to aid in development or
diagnosing kernel problems. You can access some symbols by name in
kernel built-ins or in kernel modules if the code was built with
``CONFIG_KALLSYMS``.

Kgdb is intended to be used as a source level debugger for the Linux
kernel. It is used along with gdb to debug a Linux kernel. The
expectation is that gdb can be used to "break in" to the kernel to
inspect memory, variables and look through call stack information
similar to the way an application developer would use gdb to debug an
application. It is possible to place breakpoints in kernel code and
perform some limited execution stepping.
(Kgdb 旨在用作 Linux 内核的源码级调试器。它与 gdb 配合使用来调试 Linux 内核。预期用法是：用 gdb "闯入" 内核，检查内存、变量并查看调用栈信息，就像应用开发者用 gdb 调试应用程序一样。可以在内核代码中放置断点，并执行一些有限的单步执行。)

Two machines are required for using kgdb. One of these machines is a
development machine and the other is the target machine. The kernel to
be debugged runs on the target machine. The development machine runs an
instance of gdb against the vmlinux file which contains the symbols (not
a boot image such as bzImage, zImage, uImage...). In gdb the developer
specifies the connection parameters and connects to kgdb. The type of
connection a developer makes with gdb depends on the availability of
kgdb I/O modules compiled as built-ins or loadable kernel modules in the
test machine's kernel.
(使用 kgdb 需要两台机器。一台是开发机，另一台是目标机。被调试的内核运行在目标机上。开发机上则针对包含符号信息的 vmlinux 文件(而不是 bzImage、zImage、uImage 之类的启动镜像)运行一个 gdb 实例。开发者在 gdb 中指定连接参数并连接到 kgdb。开发者用 gdb 建立的连接类型，取决于测试机内核中以内建或可加载模块形式编译的 kgdb I/O 模块的可用情况。)

Compiling a kernel(编译内核)
============================

-  In order to enable compilation of kdb, you must first enable kgdb.
   (要启用 kdb 的编译，必须先启用 kgdb。)

-  The kgdb test compile options are described in the kgdb test suite
   chapter.
   (kgdb 测试相关的编译选项在 kgdb 测试套件一章中描述。)

Kernel config options for kgdb(kgdb 的内核配置选项)
---------------------------------------------------

To enable ``CONFIG_KGDB`` you should look under
:menuselection:`Kernel hacking --> Kernel debugging` and select
:menuselection:`KGDB: kernel debugger`.
(要启用 ``CONFIG_KGDB``，应在 :menuselection:`Kernel hacking --> Kernel debugging` 之下找到并选择 :menuselection:`KGDB: kernel debugger`。)

While it is not a hard requirement that you have symbols in your vmlinux
file, gdb tends not to be very useful without the symbolic data, so you
will want to turn on ``CONFIG_DEBUG_INFO`` which is called
:menuselection:`Compile the kernel with debug info` in the config menu.
(虽然 vmlinux 文件中带不带符号并非硬性要求，但没有符号数据时 gdb 往往没什么用，因此你应当打开 ``CONFIG_DEBUG_INFO``，它在配置菜单中名为 :menuselection:`Compile the kernel with debug info`。)

It is advised, but not required, that you turn on the
``CONFIG_FRAME_POINTER`` kernel option which is called :menuselection:`Compile
the kernel with frame pointers` in the config menu. This option inserts code
into the compiled executable which saves the frame information in registers
or on the stack at different points which allows a debugger such as gdb to
more accurately construct stack back traces while debugging the kernel.
(建议(但不强制)打开 ``CONFIG_FRAME_POINTER`` 内核选项，它在配置菜单中名为 :menuselection:`Compile the kernel with frame pointers`。该选项会在编译出的可执行文件中插入代码，在不同位置把帧信息保存到寄存器或栈上，使 gdb 这样的调试器在调试内核时能够更准确地构建栈回溯。)

If the architecture that you are using supports the kernel option
``CONFIG_STRICT_KERNEL_RWX``, you should consider turning it off. This
option will prevent the use of software breakpoints because it marks
certain regions of the kernel's memory space as read-only. If kgdb
supports it for the architecture you are using, you can use hardware
breakpoints if you desire to run with the ``CONFIG_STRICT_KERNEL_RWX``
option turned on, else you need to turn off this option.
(如果你使用的架构支持内核选项 ``CONFIG_STRICT_KERNEL_RWX``，应考虑将其关闭。该选项会把内核内存空间的某些区域标记为只读，从而无法使用软件断点。如果你所用的架构的 kgdb 支持硬件断点，而你想在 ``CONFIG_STRICT_KERNEL_RWX`` 开启的情况下运行，可以使用硬件断点；否则你需要关闭该选项。)

Next you should choose one or more I/O drivers to interconnect the debugging
host and debugged target. Early boot debugging requires a KGDB I/O
driver that supports early debugging and the driver must be built into
the kernel directly. Kgdb I/O driver configuration takes place via
kernel or module parameters which you can learn more about in the
section that describes the parameter kgdboc.
(接下来，你应当选择一个或多个 I/O 驱动来互联调试主机与被调试目标。早期启动调试 (early boot debugging) 需要支持早期调试的 KGDB I/O 驱动，且该驱动必须直接编译进内核。kgdb I/O 驱动的配置通过内核参数或模块参数完成，详见描述 kgdboc 参数的那一节。)

Here is an example set of ``.config`` symbols to enable or disable for kgdb::

  # CONFIG_STRICT_KERNEL_RWX is not set
  CONFIG_FRAME_POINTER=y
  CONFIG_KGDB=y
  CONFIG_KGDB_SERIAL_CONSOLE=y

 (以下是 kgdb 需要启用或禁用的一组 ``.config`` 符号示例：)

Kernel config options for kdb(kdb 的内核配置选项)
-------------------------------------------------

Kdb is quite a bit more complex than the simple gdbstub sitting on top
of the kernel's debug core. Kdb must implement a shell, and also adds
some helper functions in other parts of the kernel, responsible for
printing out interesting data such as what you would see if you ran
``lsmod``, or ``ps``. In order to build kdb into the kernel you follow the
same steps as you would for kgdb.
(Kdb 比叠加在内核调试核心之上的简单 gdbstub 复杂不少。Kdb 必须实现一个 shell，还在内核的其他部分添加了一些辅助函数，负责打印有意思的数据，比如你运行 ``lsmod`` 或 ``ps`` 时会看到的那些。要把 kdb 编译进内核，步骤与 kgdb 相同。)

The main config option for kdb is ``CONFIG_KGDB_KDB`` which is called
:menuselection:`KGDB_KDB: include kdb frontend for kgdb` in the config menu.
In theory you would have already also selected an I/O driver such as the
``CONFIG_KGDB_SERIAL_CONSOLE`` interface if you plan on using kdb on a
serial port, when you were configuring kgdb.
(kdb 的主要配置选项是 ``CONFIG_KGDB_KDB``，在配置菜单中名为 :menuselection:`KGDB_KDB: include kdb frontend for kgdb`。理论上，在配置 kgdb 时，如果你打算在串口上使用 kdb，就应该已经选择了 ``CONFIG_KGDB_SERIAL_CONSOLE`` 之类的 I/O 驱动。)

If you want to use a PS/2-style keyboard with kdb, you would select
``CONFIG_KDB_KEYBOARD`` which is called :menuselection:`KGDB_KDB: keyboard as
input device` in the config menu. The ``CONFIG_KDB_KEYBOARD`` option is not
used for anything in the gdb interface to kgdb. The ``CONFIG_KDB_KEYBOARD``
option only works with kdb.
(如果想在 kdb 中使用 PS/2 风格的键盘，应选择 ``CONFIG_KDB_KEYBOARD``，它在配置菜单中名为 :menuselection:`KGDB_KDB: keyboard as input device`。``CONFIG_KDB_KEYBOARD`` 选项在 gdb 与 kgdb 的交互接口中毫无用处；它只在 kdb 中起作用。)

Here is an example set of ``.config`` symbols to enable/disable kdb::

  # CONFIG_STRICT_KERNEL_RWX is not set
  CONFIG_FRAME_POINTER=y
  CONFIG_KGDB=y
  CONFIG_KGDB_SERIAL_CONSOLE=y
  CONFIG_KGDB_KDB=y
  CONFIG_KDB_KEYBOARD=y

 (以下是启用/禁用 kdb 的一组 ``.config`` 符号示例：)

Kernel Debugger Boot Arguments(内核调试器启动参数)
==================================================

This section describes the various runtime kernel parameters that affect
the configuration of the kernel debugger. The following chapter covers
using kdb and kgdb as well as providing some examples of the
configuration parameters.
(本节描述影响内核调试器配置的各种运行时内核参数。下一章介绍 kdb 和 kgdb 的使用，并给出一些配置参数示例。)

Kernel parameter: kgdboc(内核参数 kgdboc)
-----------------------------------------

The kgdboc driver was originally an abbreviation meant to stand for
"kgdb over console". Today it is the primary mechanism to configure how
to communicate from gdb to kgdb as well as the devices you want to use
to interact with the kdb shell.
(kgdboc 驱动这个名字最初是 "kgdb over console"(经控制台的 kgdb)的缩写。如今它是配置 gdb 与 kgdb 之间通信方式，以及配置用于与 kdb shell 交互的设备的主要机制。)

For kgdb/gdb, kgdboc is designed to work with a single serial port. It
is intended to cover the circumstance where you want to use a serial
console as your primary console as well as using it to perform kernel
debugging. It is also possible to use kgdb on a serial port which is not
designated as a system console. Kgdboc may be configured as a kernel
built-in or a kernel loadable module. You can only make use of
``kgdbwait`` and early debugging if you build kgdboc into the kernel as
a built-in.
(对 kgdb/gdb 而言，kgdboc 设计为使用单个串口工作。它覆盖的场景是：你想把某个串口既用作主控制台、又用它执行内核调试。也可以在一个未被指定为系统控制台的串口上使用 kgdb。kgdboc 可以配置为内核内建或可加载模块。只有把 kgdboc 作为内建编译进内核，才能使用 ``kgdbwait`` 和早期调试功能。)

Optionally you can elect to activate kms (Kernel Mode Setting)
integration. When you use kms with kgdboc and you have a video driver
that has atomic mode setting hooks, it is possible to enter the debugger
on the graphics console. When the kernel execution is resumed, the
previous graphics mode will be restored. This integration can serve as a
useful tool to aid in diagnosing crashes or doing analysis of memory
with kdb while allowing the full graphics console applications to run.
(此外，你也可以选择激活 kms(内核模式设置, Kernel Mode Setting)集成。当你在 kgdboc 中使用 kms、且你的视频驱动具有 atomic 模式设置钩子时，就可以在图形控制台上进入调试器。恢复内核执行后，之前的图形模式会被还原。这一集成可以作为一个有用的工具，在允许完整图形控制台应用继续运行的同时，借助 kdb 诊断崩溃或分析内存。)

kgdboc arguments(kgdboc 参数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Usage::

	kgdboc=[kms][[,]kbd][[,]serial_device][,baud]

 (用法：)

The order listed above must be observed if you use any of the optional
configurations together.
(如果同时使用任何可选配置，必须遵守上面列出的顺序。

Abbreviations:

-  kms = Kernel Mode Setting

-  kbd = Keyboard

(缩写含义：

-  kms = Kernel Mode Setting(内核模式设置)

-  kbd = Keyboard(键盘)

You can configure kgdboc to use the keyboard, and/or a serial device
depending on if you are using kdb and/or kgdb, in one of the following
scenarios. The order listed above must be observed if you use any of the
optional configurations together. Using kms + only gdb is generally not
a useful combination.
(根据你使用的是 kdb 和/或 kgdb，可以在以下场景之一中把 kgdboc 配置为使用键盘和/或串口设备。如果同时使用任何可选配置，必须遵守上面列出的顺序。kms + 仅 gdb 的组合通常没什么用处。)

Using loadable module or built-in(使用可加载模块或内建)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. As a kernel built-in:

   Use the kernel boot argument::

	kgdboc=<tty-device>,[baud]

   (1. 作为内核内建：使用内核启动参数，如上。)

2. As a kernel loadable module:

   Use the command::

	modprobe kgdboc kgdboc=<tty-device>,[baud]

   (2. 作为内核可加载模块：使用上面的命令。)

   Here are two examples of how you might format the kgdboc string. The
   first is for an x86 target using the first serial port. The second
   example is for the ARM Versatile AB using the second serial port.
   (下面是两个格式化 kgdboc 字符串的示例。第一个用于使用第一串口的 x86 目标机；第二个用于使用第二串口的 ARM Versatile AB。)

   1. ``kgdboc=ttyS0,115200``

   2. ``kgdboc=ttyAMA1,115200``

Configure kgdboc at runtime with sysfs(通过 sysfs 在运行时配置 kgdboc)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

At run time you can enable or disable kgdboc by writing parameters
into sysfs. Here are two examples:
(在运行时，你可以通过向 sysfs 写入参数来启用或禁用 kgdboc。下面是两个示例：)

1. Enable kgdboc on ttyS0::

	echo ttyS0 > /sys/module/kgdboc/parameters/kgdboc

   (1. 在 ttyS0 上启用 kgdboc：)

2. Disable kgdboc::

	echo "" > /sys/module/kgdboc/parameters/kgdboc

   (2. 禁用 kgdboc：)

.. note::

   You do not need to specify the baud if you are configuring the
   console on tty which is already configured or open.
   (注意：如果你配置的 tty 控制台已经配置好或已打开，则无需指定波特率。)

More examples(更多示例)
^^^^^^^^^^^^^^^^^^^^^^^

You can configure kgdboc to use the keyboard, and/or a serial device
depending on if you are using kdb and/or kgdb, in one of the following
scenarios.
(根据你使用的是 kdb 和/或 kgdb，可以在以下场景之一中把 kgdboc 配置为使用键盘和/或串口设备。

1. kdb and kgdb over only a serial port::

	kgdboc=<serial_device>[,baud]

   (1. 仅通过串口使用 kdb 和 kgdb：)

   Example::

	kgdboc=ttyS0,115200

   (示例：)

2. kdb and kgdb with keyboard and a serial port::

	kgdboc=kbd,<serial_device>[,baud]

   (2. 使用键盘和串口的 kdb 和 kgdb：)

   Example::

	kgdboc=kbd,ttyS0,115200

   (示例：)

3. kdb with a keyboard::

	kgdboc=kbd

   (3. 使用键盘的 kdb：)

4. kdb with kernel mode setting::

	kgdboc=kms,kbd

   (4. 使用内核模式设置的 kdb：)

5. kdb with kernel mode setting and kgdb over a serial port::

	kgdboc=kms,kbd,ttyS0,115200

   (5. 使用内核模式设置并经串口使用 kgdb 的 kdb：)

.. note::

   Kgdboc does not support interrupting the target via the gdb remote
   protocol. You must manually send a `SysRq-G` unless you have a proxy
   that splits console output to a terminal program. A console proxy has a
   separate TCP port for the debugger and a separate TCP port for the
   "human" console. The proxy can take care of sending the `SysRq-G`
   for you.
   (注意：kgdboc 不支持通过 gdb 远程协议中断目标机。你必须手动发送 `SysRq-G`，除非你有一个把控制台输出分流给终端程序的代理。控制台代理为调试器和"人类"控制台分别提供独立的 TCP 端口。代理可以替你发送 `SysRq-G`。)

When using kgdboc with no debugger proxy, you can end up connecting the
debugger at one of two entry points. If an exception occurs after you
have loaded kgdboc, a message should print on the console stating it is
waiting for the debugger. In this case you disconnect your terminal
program and then connect the debugger in its place. If you want to
interrupt the target system and forcibly enter a debug session you have
to issue a `Sysrq` sequence and then type the letter `g`. Then you
disconnect the terminal session and connect gdb. Your options if you
don't like this are to hack gdb to send the `SysRq-G` for you as well as
on the initial connect, or to use a debugger proxy that allows an
unmodified gdb to do the debugging.
(在没有调试器代理的情况下使用 kgdboc 时，最终会在两个入口点之一连接调试器。如果在加载 kgdboc 之后发生异常，控制台应会打印一条消息，说明它正在等待调试器。此时你应断开终端程序，然后在其位置连接调试器。如果你想中断目标系统并强行进入调试会话，需要发出 `Sysrq` 序列然后键入字母 `g`。之后断开终端会话并连接 gdb。如果你不喜欢这种方式，可以选择：修改 gdb 让它替你发送 `SysRq-G` 并在初次连接时也发送，或者使用能让未经修改的 gdb 完成调试的调试器代理。)

Kernel parameter: ``kgdboc_earlycon`` (内核参数 kgdboc_earlycon)
----------------------------------------------------------------

If you specify the kernel parameter ``kgdboc_earlycon`` and your serial
driver registers a boot console that supports polling (doesn't need
interrupts and implements a nonblocking read() function) kgdb will attempt
to work using the boot console until it can transition to the regular
tty driver specified by the ``kgdboc`` parameter.
(如果你指定了内核参数 ``kgdboc_earlycon``，且你的串口驱动注册了支持轮询的启动控制台(不需要中断，并实现了非阻塞的 read() 函数)，kgdb 会尝试使用该启动控制台工作，直到能够切换到 ``kgdboc`` 参数指定的常规 tty 驱动。)

Normally there is only one boot console (especially that implements the
read() function) so just adding ``kgdboc_earlycon`` on its own is
sufficient to make this work. If you have more than one boot console you
can add the boot console's name to differentiate. Note that names that
are registered through the boot console layer and the tty layer are not
the same for the same port.
(通常只有一个启动控制台(尤其是实现了 read() 函数的)，因此只添加 ``kgdboc_earlycon`` 本身就足以让其工作。如果有多个启动控制台，可以加上启动控制台的名字来区分。注意，同一端口通过启动控制台层注册的名字与通过 tty 层注册的名字并不相同。)

For instance, on one board to be explicit you might do::

   kgdboc_earlycon=qcom_geni kgdboc=ttyMSM0

 (例如，在某个板子上，为了明确起见你可以这样写：)

If the only boot console on the device was "qcom_geni", you could simplify::

   kgdboc_earlycon kgdboc=ttyMSM0

 (如果设备上唯一的启动控制台就是 "qcom_geni"，则可以简化为：)

Kernel parameter: ``kgdbwait`` (内核参数 kgdbwait)
--------------------------------------------------

The Kernel command line option ``kgdbwait`` makes kgdb wait for a
debugger connection during booting of a kernel. You can only use this
option if you compiled a kgdb I/O driver into the kernel and you
specified the I/O driver configuration as a kernel command line option.
The kgdbwait parameter should always follow the configuration parameter
for the kgdb I/O driver in the kernel command line else the I/O driver
will not be configured prior to asking the kernel to use it to wait.

The kernel will stop and wait as early as the I/O driver and
architecture allows when you use this option. If you build the kgdb I/O
driver as a loadable kernel module kgdbwait will not do anything.
(内核命令行选项 ``kgdbwait`` 使 kgdb 在内核启动期间等待调试器连接。只有在把 kgdb I/O 驱动编译进内核、并且以内核命令行选项的形式指定了该 I/O 驱动的配置时，才能使用该选项。kgdbwait 参数在内核命令行中应始终放在 kgdb I/O 驱动的配置参数之后，否则在要求内核使用它等待之前，该 I/O 驱动尚未完成配置。
使用该选项时，内核会在 I/O 驱动与架构允许的最早时刻停止并等待。如果把 kgdb I/O 驱动编译为可加载模块，kgdbwait 将不起任何作用。)

Kernel parameter: ``kgdbcon`` (内核参数 kgdbcon)
------------------------------------------------

The ``kgdbcon`` feature allows you to see printk() messages inside gdb
while gdb is connected to the kernel. Kdb does not make use of the kgdbcon
feature.
(``kgdbcon`` 特性让你在 gdb 连接到内核时能在 gdb 内看到 printk() 消息。kdb 不使用 kgdbcon 特性。)

Kgdb supports using the gdb serial protocol to send console messages to
the debugger when the debugger is connected and running. There are two
ways to activate this feature.

1. Activate with the kernel command line option::

	kgdbcon

   (1. 用内核命令行选项激活：)

2. Use sysfs before configuring an I/O driver::

	echo 1 > /sys/module/debug_core/parameters/kgdb_use_con

   (2. 在配置 I/O 驱动之前通过 sysfs 激活：)

.. note::

   If you do this after you configure the kgdb I/O driver, the
   setting will not take effect until the next point the I/O is
   reconfigured.
   (注意：如果你在配置 kgdb I/O 驱动之后才这样做，该设置要到下一次重新配置 I/O 时才会生效。)

.. important::

   You cannot use kgdboc + kgdbcon on a tty that is an
   active system console. An example of incorrect usage is::

	console=ttyS0,115200 kgdboc=ttyS0 kgdbcon

   (重要：不能在与 kgdboc + kgdbcon 相同的、作为活动系统控制台的 tty 上使用。错误用法的例子如上。)

It is possible to use this option with kgdboc on a tty that is not a
system console.
(在一个不是系统控制台的 tty 上，kgdboc 配合该选项使用是可行的。)

Run time parameter: ``kgdbreboot`` (运行时参数 kgdbreboot)
----------------------------------------------------------

The kgdbreboot feature allows you to change how the debugger deals with
the reboot notification. You have 3 choices for the behavior. The
default behavior is always set to 0.
(kgdbreboot 特性让你改变调试器处理重启通知的方式。行为有 3 种选择，默认行为始终为 0。

.. tabularcolumns:: |p{0.4cm}|p{11.5cm}|p{5.6cm}|

.. flat-table::
  :widths: 1 10 8

  * - 1
    - ``echo -1 > /sys/module/debug_core/parameters/kgdbreboot``
    - Ignore the reboot notification entirely. (完全忽略重启通知。)

  * - 2
    - ``echo 0 > /sys/module/debug_core/parameters/kgdbreboot``
    - Send the detach message to any attached debugger client. (向所有已连接的调试器客户端发送 detach 消息。)

  * - 3
    - ``echo 1 > /sys/module/debug_core/parameters/kgdbreboot``
    - Enter the debugger on reboot notify. (在收到重启通知时进入调试器。)

Kernel parameter: ``nokaslr`` (内核参数 nokaslr)
------------------------------------------------

If the architecture that you are using enables KASLR by default,
you should consider turning it off.  KASLR randomizes the
virtual address where the kernel image is mapped and confuses
gdb which resolves addresses of kernel symbols from the symbol table
of vmlinux.
(如果你使用的架构默认启用 KASLR，应考虑将其关闭。KASLR 会随机化内核映像映射到的虚拟地址，从而迷惑 gdb——gdb 是根据 vmlinux 的符号表来解析内核符号地址的。)

Kernel parameter: ``rodata`` (内核参数 rodata)
----------------------------------------------

``CONFIG_STRICT_KERNEL_RWX`` is turned on by default and is not
visible to menuconfig on some architectures (arm64 for example),
you can pass ``rodata=off`` to the kernel in this case.
(``CONFIG_STRICT_KERNEL_RWX`` 默认开启，并且在某些架构(例如 arm64)上对 menuconfig 不可见；这种情况下你可以向内核传递 ``rodata=off``。)

Using kdb(使用 kdb)
===================

Quick start for kdb on a serial port(在串口上快速上手 kdb)
----------------------------------------------------------

This is a quick example of how to use kdb.
(这是一个如何使用 kdb 的快速示例。)

1. Configure kgdboc at boot using kernel parameters::

	console=ttyS0,115200 kgdboc=ttyS0,115200 nokaslr

   (1. 在启动时用内核参数配置 kgdboc：)

   OR
   (或者)

   Configure kgdboc after the kernel has booted; assuming you are using
   a serial port console::

	echo ttyS0 > /sys/module/kgdboc/parameters/kgdboc

   (在内核启动后配置 kgdboc；假设你使用的是串口控制台：)

2. Enter the kernel debugger manually or by waiting for an oops or
   fault. There are several ways you can enter the kernel debugger
   manually; all involve using the `SysRq-G`, which means you must have
   enabled ``CONFIG_MAGIC_SYSRQ=y`` in your kernel config.
   (2. 手动进入内核调试器，或等待 oops 或故障发生。手动进入内核调试器有几种方式；都要用到 `SysRq-G`，也就是说你的内核配置中必须启用了 ``CONFIG_MAGIC_SYSRQ=y``。)

   -  When logged in as root or with a super user session you can run::

	echo g > /proc/sysrq-trigger

      (以 root 或超级用户会话登录时，可以运行上面的命令。)

   -  Example using minicom 2.2

      Press: `CTRL-A` `f` `g`

      (使用 minicom 2.2 的示例：依次按 `CTRL-A` `f` `g`。)

   -  When you have telneted to a terminal server that supports sending
      a remote break

      Press: `CTRL-]`

      Type in: ``send break``

      Press: `Enter` `g`

      (当你 telnet 到一台支持发送远程 break 的终端服务器时：按 `CTRL-]`，输入 ``send break``，再按 `Enter` `g`。)

3. From the kdb prompt you can run the ``help`` command to see a complete
   list of the commands that are available.

   Some useful commands in kdb include:
   (3. 在 kdb 提示符下可以运行 ``help`` 命令查看可用命令的完整列表。

   kdb 中一些有用的命令包括：)

   =========== =================================================================
   ``lsmod``   Shows where kernel modules are loaded (显示内核模块加载在何处)
   ``ps``      Displays only the active processes (仅显示活动进程)
   ``ps A``    Shows all the processes (显示所有进程)
   ``summary`` Shows kernel version info and memory usage (显示内核版本信息与内存使用情况)
   ``bt``      Get a backtrace of the current process using dump_stack() (使用 dump_stack() 获取当前进程的回溯)
   ``dmesg``   View the kernel syslog buffer (查看内核 syslog 缓冲区)
   ``go``      Continue the system (继续运行系统)
   =========== =================================================================

4. When you are done using kdb you need to consider rebooting the system
   or using the ``go`` command to resuming normal kernel execution. If you
   have paused the kernel for a lengthy period of time, applications
   that rely on timely networking or anything to do with real wall clock
   time could be adversely affected, so you should take this into
   consideration when using the kernel debugger.
   (4. 用完 kdb 之后，你需要考虑重启系统，或使用 ``go`` 命令恢复内核的正常执行。如果你把内核暂停了很长一段时间，依赖及时网络通信或任何与真实墙上时间相关的事物都可能受到不利影响，因此使用内核调试器时应把这一点考虑在内。)

Quick start for kdb using a keyboard connected console(使用键盘连接的控制台快速上手 kdb)
----------------------------------------------------------------------------------------

This is a quick example of how to use kdb with a keyboard.
(这是一个如何配合键盘使用 kdb 的快速示例。)

1. Configure kgdboc at boot using kernel parameters::

	kgdboc=kbd

   (1. 在启动时用内核参数配置 kgdboc：)

   OR
   (或者)

   Configure kgdboc after the kernel has booted::

	echo kbd > /sys/module/kgdboc/parameters/kgdboc

   (在内核启动后配置 kgdboc：)

2. Enter the kernel debugger manually or by waiting for an oops or
   fault. There are several ways you can enter the kernel debugger
   manually; all involve using the `SysRq-G`, which means you must have
   enabled ``CONFIG_MAGIC_SYSRQ=y`` in your kernel config.
   (2. 手动进入内核调试器，或等待 oops 或故障发生。手动进入内核调试器有几种方式；都要用到 `SysRq-G`，也就是说你的内核配置中必须启用了 ``CONFIG_MAGIC_SYSRQ=y``。)

   -  When logged in as root or with a super user session you can run::

	echo g > /proc/sysrq-trigger

      (以 root 或超级用户会话登录时，可以运行上面的命令。)

   -  Example using a laptop keyboard:

      Press and hold down: `Alt`

      Press and hold down: `Fn`

      Press and release the key with the label: `SysRq`

      Release: `Fn`

      Press and release: `g`

      Release: `Alt`

      (使用笔记本键盘的示例：按住 `Alt`，按住 `Fn`，按下并松开标有 `SysRq` 的键，松开 `Fn`，按下并松开 `g`，最后松开 `Alt`。)

   -  Example using a PS/2 101-key keyboard

      Press and hold down: `Alt`

      Press and release the key with the label: `SysRq`

      Press and release: `g`

      Release: `Alt`

      (使用 PS/2 101 键键盘的示例：按住 `Alt`，按下并松开标有 `SysRq` 的键，按下并松开 `g`，最后松开 `Alt`。)

3. Now type in a kdb command such as ``help``, ``dmesg``, ``bt`` or ``go`` to
   continue kernel execution.
   (3. 现在键入一个 kdb 命令，例如 ``help``、``dmesg``、``bt`` 或 ``go``，以继续内核执行。)

Using kgdb / gdb(使用 kgdb / gdb)
=================================

In order to use kgdb you must activate it by passing configuration
information to one of the kgdb I/O drivers. If you do not pass any
configuration information kgdb will not do anything at all. Kgdb will
only actively hook up to the kernel trap hooks if a kgdb I/O driver is
loaded and configured. If you unconfigure a kgdb I/O driver, kgdb will
unregister all the kernel hook points.
(要使用 kgdb，必须通过把配置信息传递给某个 kgdb I/O 驱动来激活它。如果不传递任何配置信息，kgdb 将什么也不做。只有在加载并配置了 kgdb I/O 驱动的情况下，kgdb 才会主动挂接到内核陷阱钩子上。如果取消配置某个 kgdb I/O 驱动，kgdb 将注销所有内核钩子点。)

All kgdb I/O drivers can be reconfigured at run time, if
``CONFIG_SYSFS`` and ``CONFIG_MODULES`` are enabled, by echo'ing a new
config string to ``/sys/module/<driver>/parameter/<option>``. The driver
can be unconfigured by passing an empty string. You cannot change the
configuration while the debugger is attached. Make sure to detach the
debugger with the ``detach`` command prior to trying to unconfigure a
kgdb I/O driver.
(在启用 ``CONFIG_SYSFS`` 和 ``CONFIG_MODULES`` 的情况下，所有 kgdb I/O 驱动都可以在运行时重新配置，方法是向 ``/sys/module/<driver>/parameter/<option>`` echo 一个新的配置字符串。传入空字符串即可取消驱动配置。调试器已连接时不能更改配置。在尝试取消配置 kgdb I/O 驱动之前，务必先用 ``detach`` 命令断开调试器。)

Connecting with gdb to a serial port(通过串口将 gdb 连接到内核)
---------------------------------------------------------------

1. Configure kgdboc

   Configure kgdboc at boot using kernel parameters::

	kgdboc=ttyS0,115200

   (1. 配置 kgdboc。在启动时用内核参数配置 kgdboc：)

   OR

   Configure kgdboc after the kernel has booted::

	echo ttyS0 > /sys/module/kgdboc/parameters/kgdboc

   (在内核启动后配置 kgdboc：)

2. Stop kernel execution (break into the debugger)

   In order to connect to gdb via kgdboc, the kernel must first be
   stopped. There are several ways to stop the kernel which include
   using kgdbwait as a boot argument, via a `SysRq-G`, or running the
   kernel until it takes an exception where it waits for the debugger to
   attach.
   (2. 停止内核执行(闯入调试器)。要通过 kgdboc 连接 gdb，必须先停止内核。停止内核有几种方式，包括：用 kgdbwait 作为启动参数、通过 `SysRq-G`，或者让内核一直运行直到发生异常并等待调试器连接。)

   -  When logged in as root or with a super user session you can run::

	echo g > /proc/sysrq-trigger

      (以 root 或超级用户会话登录时，可以运行上面的命令。)

   -  Example using minicom 2.2

      Press: `CTRL-A` `f` `g`

      (使用 minicom 2.2 的示例：依次按 `CTRL-A` `f` `g`。)

   -  When you have telneted to a terminal server that supports sending
      a remote break

      Press: `CTRL-]`

      Type in: ``send break``

      Press: `Enter` `g`

      (当你 telnet 到一台支持发送远程 break 的终端服务器时：按 `CTRL-]`，输入 ``send break``，再按 `Enter` `g`。)

3. Connect from gdb

   Example (using a directly connected port)::

           % gdb ./vmlinux
           (gdb) set serial baud 115200
           (gdb) target remote /dev/ttyS0

   (3. 从 gdb 连接。示例(使用直连端口)：)

   Example (kgdb to a terminal server on TCP port 2012)::

           % gdb ./vmlinux
           (gdb) target remote 192.168.2.2:2012

   (示例(kgdb 连接到 TCP 端口 2012 上的终端服务器)：)

   Once connected, you can debug a kernel the way you would debug an
   application program.

   If you are having problems connecting or something is going seriously
   wrong while debugging, it will most often be the case that you want
   to enable gdb to be verbose about its target communications. You do
   this prior to issuing the ``target remote`` command by typing in::

	set debug remote 1

   (连接之后，你就可以像调试应用程序一样调试内核。

   如果连接遇到问题，或调试过程中出现严重错误，多数情况下你会希望让 gdb 对其目标通信输出详细信息。这要在发出 ``target remote`` 命令之前键入上面的命令来完成。)

Remember if you continue in gdb, and need to "break in" again, you need
to issue an other `SysRq-G`. It is easy to create a simple entry point by
putting a breakpoint at ``sys_sync`` and then you can run ``sync`` from a
shell or script to break into the debugger.
(记住：如果你在 gdb 中继续执行之后又需要再次"闯入"，就需要再发一次 `SysRq-G`。有个简单的办法可以建立入口点：在 ``sys_sync`` 处放一个断点，然后从 shell 或脚本中运行 ``sync`` 即可闯入调试器。)

kgdb and kdb interoperability(kgdb 与 kdb 的互操作)
===================================================

It is possible to transition between kdb and kgdb dynamically. The debug
core will remember which you used the last time and automatically start
in the same mode.
(可以在 kdb 与 kgdb 之间动态切换。调试核心会记住你上次使用的是哪一个，并自动以相同模式启动。)

Switching between kdb and kgdb(在 kdb 与 kgdb 之间切换)
-------------------------------------------------------

Switching from kgdb to kdb(从 kgdb 切换到 kdb)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There are two ways to switch from kgdb to kdb: you can use gdb to issue
a maintenance packet, or you can blindly type the command ``$3#33``.
Whenever the kernel debugger stops in kgdb mode it will print the
message ``KGDB or $3#33 for KDB``. It is important to note that you have
to type the sequence correctly in one pass. You cannot type a backspace
or delete because kgdb will interpret that as part of the debug stream.
(从 kgdb 切换到 kdb 有两种方式：可以用 gdb 发送一个 maintenance 包，或者直接盲目键入命令 ``$3#33``。每当内核调试器以 kgdb 模式停下时，都会打印消息 ``KGDB or $3#33 for KDB``。需要注意，必须一次性正确键入该序列。不能使用退格或删除键，因为 kgdb 会把它们当作调试流的一部分。)

1. Change from kgdb to kdb by blindly typing::

	$3#33

   (1. 通过盲目键入上面的命令从 kgdb 切换到 kdb。)

2. Change from kgdb to kdb with gdb::

	maintenance packet 3

   (2. 通过 gdb 从 kgdb 切换到 kdb：)

   .. note::

     Now you must kill gdb. Typically you press `CTRL-Z` and issue
     the command::

	kill -9 %

     (注意：现在你必须杀掉 gdb。通常按 `CTRL-Z` 然后执行上面的命令。)

Change from kdb to kgdb(从 kdb 切换到 kgdb)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

There are two ways you can change from kdb to kgdb. You can manually
enter kgdb mode by issuing the kgdb command from the kdb shell prompt,
or you can connect gdb while the kdb shell prompt is active. The kdb
shell looks for the typical first commands that gdb would issue with the
gdb remote protocol and if it sees one of those commands it
automatically changes into kgdb mode.
(从 kdb 切换到 kgdb 有两种方式。可以在 kdb shell 提示符下执行 kgdb 命令手动进入 kgdb 模式，也可以在 kdb shell 提示符激活时直接连接 gdb。kdb shell 会寻找 gdb 通过 gdb 远程协议通常最先发出的那些命令，一旦看到其中之一，就自动切换到 kgdb 模式。)

1. From kdb issue the command::

	kgdb

   (1. 在 kdb 中执行上面的命令。)

2. At the kdb prompt, disconnect the terminal program and connect gdb in
   its place.
   (2. 在 kdb 提示符下断开终端程序，并在其位置连接 gdb。)

Running kdb commands from gdb(从 gdb 运行 kdb 命令)
---------------------------------------------------

It is possible to run a limited set of kdb commands from gdb, using the
gdb monitor command. You don't want to execute any of the run control or
breakpoint operations, because it can disrupt the state of the kernel
debugger. You should be using gdb for breakpoints and run control
operations if you have gdb connected. The more useful commands to run
are things like lsmod, dmesg, ps or possibly some of the memory
information commands. To see all the kdb commands you can run
``monitor help``.
(可以使用 gdb 的 monitor 命令从 gdb 运行有限的 kdb 命令集。不要执行任何运行控制或断点操作，因为这可能扰乱内核调试器的状态。如果已连接 gdb，就应当用 gdb 来做断点和运行控制操作。比较有用的命令是 lsmod、dmesg、ps 之类，或某些内存信息命令。要查看全部 kdb 命令，可以运行 ``monitor help``。)

Example::

    (gdb) monitor ps
    1 idle process (state I) and
    27 sleeping system daemon (state M) processes suppressed,
    use 'ps A' to see all.
    Task Addr       Pid   Parent [*] cpu State Thread     Command

    0xc78291d0        1        0  0    0   S  0xc7829404  init
    0xc7954150      942        1  0    0   S  0xc7954384  dropbear
    0xc78789c0      944        1  0    0   S  0xc7878bf4  sh
    (gdb)

 (示例：)

kgdb Test Suite(kgdb 测试套件)
==============================

When kgdb is enabled in the kernel config you can also elect to enable
the config parameter ``KGDB_TESTS``. Turning this on will enable a special
kgdb I/O module which is designed to test the kgdb internal functions.
(在内核配置中启用 kgdb 后，你还可以选择启用配置参数 ``KGDB_TESTS``。打开它会启用一个特殊的 kgdb I/O 模块，该模块专门用于测试 kgdb 的内部功能。)

The kgdb tests are mainly intended for developers to test the kgdb
internals as well as a tool for developing a new kgdb architecture
specific implementation. These tests are not really for end users of the
Linux kernel. The primary source of documentation would be to look in
the ``drivers/misc/kgdbts.c`` file.
(kgdb 测试主要面向开发者，用于测试 kgdb 内部机制，也是开发新的 kgdb 架构特定实现的工具。这些测试并非真正面向 Linux 内核的最终用户。主要的文档来源是 ``drivers/misc/kgdbts.c`` 文件。)

The kgdb test suite can also be configured at compile time to run the
core set of tests by setting the kernel config parameter
``KGDB_TESTS_ON_BOOT``. This particular option is aimed at automated
regression testing and does not require modifying the kernel boot config
arguments. If this is turned on, the kgdb test suite can be disabled by
specifying ``kgdbts=`` as a kernel boot argument.
(还可以在编译期通过设置内核配置参数 ``KGDB_TESTS_ON_BOOT``，把 kgdb 测试套件配置为运行核心测试集。该选项面向自动化回归测试，不需要修改内核启动配置参数。如果它被打开，可以通过把 ``kgdbts=`` 指定为内核启动参数来禁用 kgdb 测试套件。)

Kernel Debugger Internals(内核调试器内部机制)
=============================================

Architecture Specifics(架构相关细节)
------------------------------------

The kernel debugger is organized into a number of components:
(内核调试器由多个组件构成：)

1. The debug core
(1. 调试核心 (debug core))

   The debug core is found in ``kernel/debugger/debug_core.c``. It
   contains:

   -  A generic OS exception handler which includes sync'ing the
      processors into a stopped state on an multi-CPU system.

   -  The API to talk to the kgdb I/O drivers

   -  The API to make calls to the arch-specific kgdb implementation

   -  The logic to perform safe memory reads and writes to memory while
      using the debugger

   -  A full implementation for software breakpoints unless overridden
      by the arch

   -  The API to invoke either the kdb or kgdb frontend to the debug
      core.

   -  The structures and callback API for atomic kernel mode setting.

      .. note:: kgdboc is where the kms callbacks are invoked.

   (调试核心位于 ``kernel/debugger/debug_core.c``，包含：

   -  一个通用的 OS 异常处理程序，包括在多 CPU 系统上把各处理器同步到停止状态；
   -  与 kgdb I/O 驱动对话的 API；
   -  调用架构特定 kgdb 实现的 API；
   -  使用调试器时对内存执行安全读写的逻辑；
   -  软件断点的完整实现(除非架构自行覆盖)；
   -  调用调试核心的 kdb 或 kgdb 前端的 API；
   -  atomic 内核模式设置的结构体与回调 API。注意：kms 回调正是在 kgdboc 中被调用的。)

2. kgdb arch-specific implementation

   This implementation is generally found in ``arch/*/kernel/kgdb.c``. As
   an example, ``arch/x86/kernel/kgdb.c`` contains the specifics to
   implement HW breakpoint as well as the initialization to dynamically
   register and unregister for the trap handlers on this architecture.
   The arch-specific portion implements:

   -  contains an arch-specific trap catcher which invokes
      kgdb_handle_exception() to start kgdb about doing its work

   -  translation to and from gdb specific packet format to struct pt_regs

   -  Registration and unregistration of architecture specific trap
      hooks

   -  Any special exception handling and cleanup

   -  NMI exception handling and cleanup

   -  (optional) HW breakpoints

   (2. kgdb 架构特定实现
   该实现一般位于 ``arch/*/kernel/kgdb.c``。例如 ``arch/x86/kernel/kgdb.c`` 包含实现硬件断点的细节，以及在该架构上动态注册/注销陷阱处理程序的初始化代码。架构特定部分实现：

   -  包含一个架构特定的陷阱捕获器，它调用 kgdb_handle_exception() 启动 kgdb 开展工作；
   -  gdb 特定包格式与 struct pt_regs 之间的相互转换；
   -  架构特定陷阱钩子的注册与注销；
   -  任何特殊的异常处理与清理；
   -  NMI 异常处理与清理；
   -  (可选)硬件断点。)

3. gdbstub frontend (aka kgdb)

   The gdbstub is located in ``kernel/debug/gdbstub.c``. It contains:

   -  All the logic to implement the gdb serial protocol

   (3. gdbstub 前端(即 kgdb)
   gdbstub 位于 ``kernel/debug/gdbstub.c``，包含：

   -  实现 gdb 串行协议的全部逻辑。)

4. kdb frontend

   The kdb debugger shell is broken down into a number of components.
   The kdb core is located in kernel/debug/kdb. There are a number of
   helper functions in some of the other kernel components to make it
   possible for kdb to examine and report information about the kernel
   without taking locks that could cause a kernel deadlock. The kdb core
   implements the following functionality.

   -  A simple shell

   -  The kdb core command set

   -  A registration API to register additional kdb shell commands.

      -  A good example of a self-contained kdb module is the ``ftdump``
         command for dumping the ftrace buffer. See:
         ``kernel/trace/trace_kdb.c``

      -  For an example of how to dynamically register a new kdb command
         you can build the kdb_hello.ko kernel module from
         ``samples/kdb/kdb_hello.c``. To build this example you can set
         ``CONFIG_SAMPLES=y`` and ``CONFIG_SAMPLE_KDB=m`` in your kernel
         config. Later run ``modprobe kdb_hello`` and the next time you
         enter the kdb shell, you can run the ``hello`` command.

   -  The implementation for kdb_printf() which emits messages directly
      to I/O drivers, bypassing the kernel log.

   -  SW / HW breakpoint management for the kdb shell

   (4. kdb 前端
   kdb 调试器 shell 被分解为多个组件。kdb 核心位于 kernel/debug/kdb。其他一些内核组件中还有不少辅助函数，使 kdb 能够在不获取可能导致内核死锁的锁的情况下检查并报告内核信息。kdb 核心实现以下功能：

   -  一个简单的 shell；
   -  kdb 核心命令集；
   -  用于注册额外 kdb shell 命令的注册 API。一个自包含 kdb 模块的好例子是用于转储 ftrace 缓冲区的 ``ftdump`` 命令，见 ``kernel/trace/trace_kdb.c``。有关如何动态注册新 kdb 命令的示例，可以从 ``samples/kdb/kdb_hello.c`` 构建 kdb_hello.ko 内核模块：要构建该示例，可在内核配置中设置 ``CONFIG_SAMPLES=y`` 和 ``CONFIG_SAMPLE_KDB=m``，之后运行 ``modprobe kdb_hello``，下次进入 kdb shell 时就可以运行 ``hello`` 命令；
   -  kdb_printf() 的实现，它绕过内核日志直接向 I/O 驱动发送消息；
   -  kdb shell 的软件/硬件断点管理。)

5. kgdb I/O driver

   Each kgdb I/O driver has to provide an implementation for the
   following:

   -  configuration via built-in or module

   -  dynamic configuration and kgdb hook registration calls

   -  read and write character interface

   -  A cleanup handler for unconfiguring from the kgdb core

   -  (optional) Early debug methodology

   Any given kgdb I/O driver has to operate very closely with the
   hardware and must do it in such a way that does not enable interrupts
   or change other parts of the system context without completely
   restoring them. The kgdb core will repeatedly "poll" a kgdb I/O
   driver for characters when it needs input. The I/O driver is expected
   to return immediately if there is no data available. Doing so allows
   for the future possibility to touch watchdog hardware in such a way
   as to have a target system not reset when these are enabled.

   (5. kgdb I/O 驱动
   每个 kgdb I/O 驱动都必须为以下内容提供实现：

   -  通过内建或模块进行配置；
   -  动态配置与 kgdb 钩子注册调用；
   -  字符读写接口；
   -  用于从 kgdb 核心取消配置的清理处理程序；
   -  (可选)早期调试方法。

   任何一个 kgdb I/O 驱动都必须与硬件密切配合，并且必须以不开启中断、或在不完全恢复系统上下文其他部分的情况下不改动它们的方式工作。kgdb 核心在需要输入时会反复"轮询" kgdb I/O 驱动获取字符。如果没有可用数据，I/O 驱动应立即返回。这样做为将来触碰看门狗硬件创造了可能：在这些看门狗启用时目标系统不会被复位。)

If you are intent on adding kgdb architecture specific support for a new
architecture, the architecture should define ``HAVE_ARCH_KGDB`` in the
architecture specific Kconfig file. This will enable kgdb for the
architecture, and at that point you must create an architecture specific
kgdb implementation.
(如果你打算为某个新架构添加 kgdb 架构特定支持，该架构应在其架构特定 Kconfig 文件中定义 ``HAVE_ARCH_KGDB``。这将为该架构启用 kgdb，届时你必须创建一个架构特定的 kgdb 实现。)

There are a few flags which must be set on every architecture in their
``asm/kgdb.h`` file. These are:

-  ``NUMREGBYTES``:
     The size in bytes of all of the registers, so that we
     can ensure they will all fit into a packet.

-  ``BUFMAX``:
     The size in bytes of the buffer GDB will read into. This must
     be larger than NUMREGBYTES.

-  ``CACHE_FLUSH_IS_SAFE``:
     Set to 1 if it is always safe to call
     flush_cache_range or flush_icache_range. On some architectures,
     these functions may not be safe to call on SMP since we keep other
     CPUs in a holding pattern.

(每个架构都必须在其 ``asm/kgdb.h`` 文件中设置几个标志。它们是： ``NUMREGBYTES`` 是所有寄存器按字节计的大小，以便确保它们全部能装进一个包； ``BUFMAX`` 是 GDB 将读入的缓冲区按字节计的大小，必须大于 NUMREGBYTES； ``CACHE_FLUSH_IS_SAFE`` 若设为 1 表示调用 flush_cache_range 或 flush_icache_range 总是安全——在某些架构上，这些函数在 SMP 上调用可能不安全，因为我们让其他 CPU 处于等待状态。)

There are also the following functions for the common backend, found in
``kernel/kgdb.c``, that must be supplied by the architecture-specific
backend unless marked as (optional), in which case a default function
maybe used if the architecture does not need to provide a specific
implementation.
(此外，``kernel/kgdb.c`` 中还有以下供公共后端使用的函数，必须由架构特定后端提供——除非标注为 (optional)；若为后者，当架构不需要提供特定实现时可以使用默认函数。)

.. kernel-doc:: include/linux/kgdb.h
   :internal:

kgdboc internals(kgdboc 内部机制)
---------------------------------

kgdboc and uarts(kgdboc 与 UART)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The kgdboc driver is actually a very thin driver that relies on the
underlying low level to the hardware driver having "polling hooks" to
which the tty driver is attached. In the initial implementation of
kgdboc the serial_core was changed to expose a low level UART hook for
doing polled mode reading and writing of a single character while in an
atomic context. When kgdb makes an I/O request to the debugger, kgdboc
invokes a callback in the serial core which in turn uses the callback in
the UART driver.
(kgdboc 驱动实际上是一个非常薄的驱动，它依赖底层硬件驱动所具有的"轮询钩子 (polling hooks)"，tty 驱动即挂接在其上。在 kgdboc 的初始实现中，serial_core 被修改为暴露一个底层 UART 钩子，用于在原子上下文中以轮询模式读取和写入单个字符。当 kgdb 向调试器发出 I/O 请求时，kgdboc 调用 serial core 中的回调，后者再使用 UART 驱动中的回调。)

When using kgdboc with a UART, the UART driver must implement two
callbacks in the struct uart_ops.
Example from ``drivers/8250.c``::


    #ifdef CONFIG_CONSOLE_POLL
        .poll_get_char = serial8250_get_poll_char,
        .poll_put_char = serial8250_put_poll_char,
    #endif

 (在配合 UART 使用 kgdboc 时，UART 驱动必须在 struct uart_ops 中实现两个回调。以上是 ``drivers/8250.c`` 中的示例。)

Any implementation specifics around creating a polling driver use the
``#ifdef CONFIG_CONSOLE_POLL``, as shown above. Keep in mind that
polling hooks have to be implemented in such a way that they can be
called from an atomic context and have to restore the state of the UART
chip on return such that the system can return to normal when the
debugger detaches. You need to be very careful with any kind of lock you
consider, because failing here is most likely going to mean pressing the
reset button.
(围绕创建轮询驱动的任何实现细节都使用 ``#ifdef CONFIG_CONSOLE_POLL``，如上所示。请记住，轮询钩子的实现必须能从原子上下文中调用，并且返回时必须恢复 UART 芯片的状态，使调试器断开时系统能恢复正常。对你考虑使用的任何锁都要非常小心，因为在这里失败很可能意味着要去按复位按钮了。)

kgdboc and keyboards(kgdboc 与键盘)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The kgdboc driver contains logic to configure communications with an
attached keyboard. The keyboard infrastructure is only compiled into the
kernel when ``CONFIG_KDB_KEYBOARD=y`` is set in the kernel configuration.
(kgdboc 驱动包含与已连接键盘配置通信的逻辑。只有当内核配置中设置了 ``CONFIG_KDB_KEYBOARD=y`` 时，键盘基础设施才会编译进内核。)

The core polled keyboard driver for PS/2 type keyboards is in
``drivers/char/kdb_keyboard.c``. This driver is hooked into the debug core
when kgdboc populates the callback in the array called
:c:expr:`kdb_poll_funcs[]`. The kdb_get_kbd_char() is the top-level
function which polls hardware for single character input.
(PS/2 类型键盘的核心轮询键盘驱动位于 ``drivers/char/kdb_keyboard.c``。当 kgdboc 在 :c:expr:`kdb_poll_funcs[]` 数组中填充回调时，该驱动即挂接到调试核心。kdb_get_kbd_char() 是对硬件轮询单个字符输入的顶层函数。)

kgdboc and kms(kgdboc 与 KMS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The kgdboc driver contains logic to request the graphics display to
switch to a text context when you are using ``kgdboc=kms,kbd``, provided
that you have a video driver which has a frame buffer console and atomic
kernel mode setting support.
(当你使用 ``kgdboc=kms,kbd`` 时，kgdboc 驱动包含请求图形显示切换到文本上下文的逻辑——前提是你的视频驱动具有帧缓冲控制台和 atomic 内核模式设置支持。)

Every time the kernel debugger is entered it calls
kgdboc_pre_exp_handler() which in turn calls con_debug_enter()
in the virtual console layer. On resuming kernel execution, the kernel
debugger calls kgdboc_post_exp_handler() which in turn calls
con_debug_leave().
(每次进入内核调试器时，它都会调用 kgdboc_pre_exp_handler()，后者再调用虚拟控制台层中的 con_debug_enter()。恢复内核执行时，内核调试器调用 kgdboc_post_exp_handler()，后者再调用 con_debug_leave()。)



Credits(致谢)
==============

The following people have contributed to this document:
(以下人员为本文件作出了贡献：)

1. Amit Kale <amitkale@linsyssoft.com>

2. Tom Rini <trini@kernel.crashing.org>

In March 2008 this document was completely rewritten by:
(2008 年 3 月，本文件由以下人员彻底重写：)

-  Jason Wessel <jason.wessel@windriver.com>

In Jan 2010 this document was updated to include kdb.
(2010 年 1 月，本文件更新并加入了 kdb 内容，更新者为：)

-  Jason Wessel <jason.wessel@windriver.com>
