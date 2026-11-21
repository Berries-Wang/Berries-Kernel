.. SPDX-License-Identifier: GPL-2.0

===================================================================================================================
CPUFreq - CPU frequency and voltage scaling code in the Linux(TM) kernel(Linux(TM) 内核中的 CPU 频率与电压调节代码)
===================================================================================================================

Author: Dominik Brodowski  <linux@brodo.de>
(作者：Dominik Brodowski <linux@brodo.de>)

   Clock scaling allows you to change the clock speed of the CPUs on the
   fly. This is a nice method to save battery power, because the lower
   the clock speed, the less power the CPU consumes.
   (时钟调节让你可以动态改变 CPU 的时钟速度。这是一种不错的省电方法，因为
   时钟速度越低，CPU 消耗的电能就越少。)


.. toctree::
   :maxdepth: 1

   core
   cpu-drivers
   cpufreq-stats

Mailing List(邮件列表)
----------------------
There is a CPU frequency general list where you can report bugs,
problems or submit patches. To post a message, send an email to
linux-pm@vger.kernel.org.
(有一个 CPU 频率通用邮件列表，你可以在那里报告 bug、问题或提交补丁。发帖
请发送邮件至 linux-pm@vger.kernel.org。)

Links(链接)
-----------
the FTP archives:
(FTP 归档：)
* ftp://ftp.linux.org.uk/pub/linux/cpufreq/

the CPUFreq Mailing list:
(CPUFreq 邮件列表：)
* http://vger.kernel.org/vger-lists.html#linux-pm

Clock and voltage scaling for the SA-1100:
(SA-1100 的时钟与电压调节：)
* http://www.lartmaker.nl/projects/scaling
