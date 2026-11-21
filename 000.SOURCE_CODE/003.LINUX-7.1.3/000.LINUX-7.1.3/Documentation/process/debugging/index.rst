.. SPDX-License-Identifier: GPL-2.0

=============================================================================
Debugging advice for Linux Kernel developers(面向 Linux 内核开发者的调试建议)
=============================================================================

general guides(通用指南)
------------------------

.. toctree::
   :maxdepth: 1

   driver_development_debugging_guide
   gdb-kernel-debugging
   kgdb
   userspace_debugging_guide

subsystem specific guides(子系统专属指南)
-----------------------------------------

.. toctree::
   :maxdepth: 1

   media_specific_debugging_guide

General debugging advice(一般调试建议)
======================================

Depending on the issue, a different set of tools is available to track down the
problem or even to realize whether there is one in the first place.
(根据问题的不同，可以使用不同的工具集来追查问题，甚至首先弄清是否真
的存在问题。)

As a first step you have to figure out what kind of issue you want to debug.
Depending on the answer, your methodology and choice of tools may vary.
(第一步是弄清你想调试的是哪类问题。根据答案，你的方法论与工具选择可
能有所不同。)

Do I need to debug with limited access?(是否需要在受限访问下调试？)
-------------------------------------------------------------------

Do you have limited access to the machine or are you unable to stop the running
execution?
(你对机器的访问是否受限？或者你是否无法停止正在运行的执行？)

In this case your debugging capability depends on built-in debugging support of
provided distribution kernel.
The :doc:`/process/debugging/userspace_debugging_guide` provides a brief
overview over a range of possible debugging tools in that situation. You can
check the capability of your kernel, in most cases, by looking into config file
within the /boot directory.
(在这种情况下，你的调试能力取决于所提供的发行版内核的内建调试支持。
:doc:`/process/debugging/userspace_debugging_guide` 简要概述了这种情况
下的多种可用调试工具。在大多数情况下，你可以通过查看 /boot 目录中的
config 文件来检查你的内核的能力。)

Do I have root access to the system?(是否拥有系统的 root 访问权限？)
--------------------------------------------------------------------

Are you easily able to replace the module in question or to install a new
kernel?
(你是否能轻松替换有问题的模块或安装新内核？)

In that case your range of available tools is a lot bigger, you can find the
tools in the :doc:`/process/debugging/driver_development_debugging_guide`.
(在这种情况下，可用工具的范围要大得多；你可以在
:doc:`/process/debugging/driver_development_debugging_guide` 中找到这些
工具。)

Is timing a factor?(时序是否是一个因素？)
-----------------------------------------

It is important to understand if the problem you want to debug manifests itself
consistently (i.e. given a set of inputs you always get the same, incorrect
output), or inconsistently. If it manifests itself inconsistently, some timing
factor might be at play. If inserting delays into the code does change the
behavior, then quite likely timing is a factor.
(重要的是弄清你想调试的问题的表现是一致的 (即给定一组输入总是得到相
同的错误输出) 还是不一致的。如果表现不一致，可能存在某种时序因素。
如果在代码中插入延迟确实会改变行为，那么时序很可能是一个因素。)

When timing does alter the outcome of the code execution using a simple
printk() for debugging purposes may not work, a similar alternative is to use
trace_printk() , which logs the debug messages to the trace file instead of the
kernel log.
(当时序确实会改变代码执行的结果时，出于调试目的使用简单的 printk()
可能行不通；一个类似的替代方案是使用 trace_printk()，它把调试消息记
录到 trace 文件而不是内核日志中。)

**Copyright** ©2024 : Collabora
