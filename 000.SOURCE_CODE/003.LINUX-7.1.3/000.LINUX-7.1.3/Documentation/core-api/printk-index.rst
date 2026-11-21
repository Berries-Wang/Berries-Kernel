.. SPDX-License-Identifier: GPL-2.0

=========================
Printk Index(Printk 索引)
=========================

There are many ways to monitor the state of the system. One important
source of information is the system log. It provides a lot of information,
including more or less important warnings and error messages.
(监控系统状态的方法有很多。系统日志是一个重要的信息来源，它提供了大量信息，包括或多或少重要的警告和错误消息。)

There are monitoring tools that filter and take action based on messages
logged.
(已有一些监控工具可以根据记录的消息进行过滤并采取行动。)

The kernel messages are evolving together with the code. As a result,
particular kernel messages are not KABI and never will be!
(内核消息随代码一起演进。因此，特定的内核消息不是 KABI，也永远不会是！)

It is a huge challenge for maintaining the system log monitors. It requires
knowing what messages were updated in a particular kernel version and why.
Finding these changes in the sources would require non-trivial parsers.
Also it would require matching the sources with the binary kernel which
is not always trivial. Various changes might be backported. Various kernel
versions might be used on different monitored systems.
(这对维护系统日志监控器来说是巨大的挑战。它需要了解特定内核版本更新了哪些消息以及为什么更新。要在源代码中找到这些更改，需要相当复杂的解析器。还需要将源代码与二进制内核相匹配，而这并非总是容易。各种更改可能被向后移植到旧版本。不同的受监控系统可能运行着各种不同的内核版本。)

This is where the printk index feature might become useful. It provides
a dump of printk formats used all over the source code used for the kernel
and modules on the running system. It is accessible at runtime via debugfs.
(这正是 printk 索引特性可能派上用场的地方。它对运行系统中内核和模块所用的全部源代码里的 printk 格式进行转储。可通过 debugfs 在运行时访问。)

The printk index helps to find changes in the message formats. Also it helps
to track the strings back to the kernel sources and the related commit.
(printk 索引有助于发现消息格式的变化，也有助于把字符串回溯到内核源代码及相关的提交。)


User Interface(用户界面)
========================

The index of printk formats are split in into separate files. The files are
named according to the binaries where the printk formats are built-in. There
is always "vmlinux" and optionally also modules, for example::

   /sys/kernel/debug/printk/index/vmlinux
   /sys/kernel/debug/printk/index/ext4
   /sys/kernel/debug/printk/index/scsi_mod

(printk 格式的索引被拆分成多个单独的文件。文件根据 printk 格式内置于其中的二进制文件来命名。总是有 "vmlinux"，可选地还有模块，例如：)

Note that only loaded modules are shown. Also printk formats from a module
might appear in "vmlinux" when the module is built-in.
(注意，只显示已加载的模块。另外，当模块被内建时，来自该模块的 printk 格式可能出现在 "vmlinux" 中。)

The content is inspired by the dynamic debug interface and looks like::

   $> head -1 /sys/kernel/debug/printk/index/vmlinux; shuf -n 5 vmlinux
   # <level[,flags]> filename:line function "format"
   <5> block/blk-settings.c:661 disk_stack_limits "%s: Warning: Device %s is misaligned\n"
   <4> kernel/trace/trace.c:8296 trace_create_file "Could not create tracefs '%s' entry\n"
   <6> arch/x86/kernel/hpet.c:144 _hpet_print_config "hpet: %s(%d):\n"
   <6> init/do_mounts.c:605 prepare_namespace "Waiting for root device %s...\n"
   <6> drivers/acpi/osl.c:1410 acpi_no_auto_serialize_setup "ACPI: auto-serialization disabled\n"

(其内容受到动态调试接口的启发，形如下面所示：)

, where the meaning is:
(其中各部分的含义如下：)

   - :level: log level value: 0-7 for particular severity, -1 as default,
	'c' as continuous line without an explicit log level
        (:level: 日志级别值：0-7 表示特定的严重级别，-1 为默认值，'c' 表示没有显式日志级别的连续行)
   - :flags: optional flags: currently only 'c' for KERN_CONT
      (:flags: 可选标志：目前只有表示 KERN_CONT 的 'c')
   - :filename\:line: source filename and line number of the related
	printk() call. Note that there are many wrappers, for example,
	pr_warn(), pr_warn_once(), dev_warn().
        (:filename\:line: 相关 printk() 调用的源文件名和行号。注意存在许多包装器，例如 pr_warn() 、 pr_warn_once() 、 dev_warn() 。)
   - :function: function name where the printk() call is used.
      (:function: 使用该 printk() 调用的函数名。)
   - :format: format string
      (:format: 格式字符串)

The extra information makes it a bit harder to find differences
between various kernels. Especially the line number might change
very often. On the other hand, it helps a lot to confirm that
it is the same string or find the commit that is responsible
for eventual changes.
(这些额外的信息使得在不同内核之间查找差异变得稍微困难一些，尤其是行号可能经常变化。另一方面，它非常有助于确认这是同一个字符串，或找到导致最终更改的提交。)


printk() Is Not a Stable KABI(printk() 不是稳定的 KABI)
=======================================================

Several developers are afraid that exporting all these implementation
details into the user space will transform particular printk() calls
into KABI.
(一些开发者担心，把所有这些实现细节导出到用户空间，会使特定的 printk() 调用变成 KABI。)

But it is exactly the opposite. printk() calls must _not_ be KABI.
And the printk index helps user space tools to deal with this.
(但情况恰恰相反。printk() 调用绝不能成为 KABI。而 printk 索引有助于用户空间工具应对这一点。)


Subsystem specific printk wrappers(子系统特定的 printk 封装)
============================================================

The printk index is generated using extra metadata that are stored in
a dedicated .elf section ".printk_index". It is achieved using macro
wrappers doing __printk_index_emit() together with the real printk()
call. The same technique is used also for the metadata used by
the dynamic debug feature.
(printk 索引使用存储在专用 .elf 节 ".printk_index" 中的额外元数据生成。它是通过宏包装器实现的，这些包装器在进行真正的 printk() 调用的同时执行 __printk_index_emit() 。动态调试特性所用的元数据也采用同样的技术。)

The metadata are stored for a particular message only when it is printed
using these special wrappers. It is implemented for the commonly
used printk() calls, including, for example, pr_warn(), or pr_once().
(只有当某条消息通过这些特殊包装器打印时，才会存储它的元数据。常用的 printk() 调用已经实现了该机制，例如 pr_warn() 或 pr_once() 。)

Additional changes are necessary for various subsystem specific wrappers
that call the original printk() via a common helper function. These needs
their own wrappers adding __printk_index_emit().
(对于通过公共辅助函数调用原始 printk() 的各子系统特定包装器，还需要额外的更改。它们需要自己的包装器来添加 __printk_index_emit() 。)

Only few subsystem specific wrappers have been updated so far,
for example, dev_printk(). As a result, the printk formats from
some subsystems can be missing in the printk index.
(到目前为止，只有少数子系统特定的包装器被更新，例如 dev_printk() 。因此，某些子系统的 printk 格式可能在 printk 索引中缺失。)


Subsystem specific prefix(子系统特定的前缀)
===========================================

The macro pr_fmt() macro allows to define a prefix that is printed
before the string generated by the related printk() calls.
(宏 pr_fmt() 允许定义一个前缀，打印在相关 printk() 调用生成的字符串之前。)

Subsystem specific wrappers usually add even more complicated
prefixes.
(子系统特定的包装器通常会添加更加复杂的前缀。)

These prefixes can be stored into the printk index metadata
by an optional parameter of __printk_index_emit(). The debugfs
interface might then show the printk formats including these prefixes.
For example, drivers/acpi/osl.c contains::

  #define pr_fmt(fmt) "ACPI: OSL: " fmt

  static int __init acpi_no_auto_serialize_setup(char *str)
  {
	acpi_gbl_auto_serialize_methods = FALSE;
	pr_info("Auto-serialization disabled\n");

	return 1;
  }

(这些前缀可以通过 __printk_index_emit() 的一个可选参数存入 printk 索引元数据。之后 debugfs 接口显示的 printk 格式就可能包含这些前缀。例如，drivers/acpi/osl.c 包含：)

This results in the following printk index entry::

  <6> drivers/acpi/osl.c:1410 acpi_no_auto_serialize_setup "ACPI: auto-serialization disabled\n"

(这会得到如下的 printk 索引条目：)

It helps matching messages from the real log with printk index.
Then the source file name, line number, and function name can
be used to match the string with the source code.
(它有助于把真实日志中的消息与 printk 索引匹配起来。然后可以使用源文件名、行号和函数名把该字符串与源代码匹配。)
