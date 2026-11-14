.. SPDX-License-Identifier: GPL-2.0

==========================
File Locking Release Notes
==========================

(文件锁定发布说明)

		Andy Walker <andy@lysaker.kvaerner.no>
(作者: Andy Walker <andy@lysaker.kvaerner.no>)

			    12 May 1997
(日期: 1997年5月12日)


1. What's New?
(1. 新增内容？)
==============

1.1 Broken Flock Emulation
(1.1 修复有缺陷的 flock 模拟)
--------------------------

The old flock(2) emulation in the kernel was swapped for proper BSD
compatible flock(2) support in the 1.3.x series of kernels. With the
release of the 2.1.x kernel series, support for the old emulation has
been totally removed, so that we don't need to carry this baggage
forever.
(在 1.3.x 系列内核中，内核里旧的 flock(2) 模拟已被替换为符合 BSD 的正确 flock(2) 支持。随着 2.1.x 内核系列的发布，对旧模拟的支持已被完全移除，这样我们就无需永远背负这一包袱。)

This should not cause problems for anybody, since everybody using a
2.1.x kernel should have updated their C library to a suitable version
anyway (see the file "Documentation/process/changes.rst".)
(这不应该给任何人带来问题，因为使用 2.1.x 内核的用户反正都应该已经将 C 库更新到了合适的版本(参见文件 "Documentation/process/changes.rst"。)。)

1.2 Allow Mixed Locks Again
(1.2 重新允许混合锁定)
---------------------------

1.2.1 Typical Problems - Sendmail
(1.2.1 典型问题 - sendmail)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Because sendmail was unable to use the old flock() emulation, many sendmail
installations use fcntl() instead of flock(). This is true of Slackware 3.0
for example. This gave rise to some other subtle problems if sendmail was
configured to rebuild the alias file. Sendmail tried to lock the aliases.dir
file with fcntl() at the same time as the GDBM routines tried to lock this
file with flock(). With pre 1.3.96 kernels this could result in deadlocks that,
over time, or under a very heavy mail load, would eventually cause the kernel
to lock solid with deadlocked processes.
(由于 sendmail 无法使用旧的 flock() 模拟，许多 sendmail 安装使用 fcntl() 而不是 flock()。例如 Slackware 3.0 就是如此。如果 sendmail 被配置为重建别名文件，这会引起其他一些微妙的问题。sendmail 尝试用 fcntl() 锁定 aliases.dir 文件，与此同时 GDBM 例程尝试用 flock() 锁定该文件。在 1.3.96 之前的内核中，这可能导致死锁，随着时间的推移或在非常沉重的邮件负载下，最终会使内核因进程死锁而完全僵死。)


1.2.2 The Solution
(1.2.2 解决方案)
^^^^^^^^^^^^^^^^^^
The solution I have chosen, after much experimentation and discussion,
is to make flock() and fcntl() locks oblivious to each other. Both can
exists, and neither will have any effect on the other.
(经过大量实验和讨论后，我选择的解决方案是让 flock() 和 fcntl() 锁互不感知。两者可以并存，且互不影响。)

I wanted the two lock styles to be cooperative, but there were so many
race and deadlock conditions that the current solution was the only
practical one. It puts us in the same position as, for example, SunOS
4.1.x and several other commercial Unices. The only OS's that support
cooperative flock()/fcntl() are those that emulate flock() using
fcntl(), with all the problems that implies.
(我希望这两种锁风格能够协作，但竞争和死锁情况实在太多，当前的解决方案是唯一可行的。这使我们处于与 SunOS 4.1.x 以及其他几个商业 Unix 相同的位置。唯一支持协作式 flock()/fcntl() 的操作系统是那些用 fcntl() 来模拟 flock() 的系统，以及随之而来的所有问题。)


1.3 Mandatory Locking As A Mount Option
(1.3 作为挂载选项的强制锁定)
---------------------------------------

Mandatory locking was prior to this release a general configuration option
that was valid for all mounted filesystems.  This had a number of inherent
dangers, not the least of which was the ability to freeze an NFS server by
asking it to read a file for which a mandatory lock existed.
(在此版本发布之前，强制锁定是一个对所有已挂载文件系统有效的通用配置选项。这存在许多固有的危险，其中最严重的是可以通过要求 NFS 服务器读取一个存在强制锁的文件来使其冻结。)

Such option was dropped in Kernel v5.14.
(该选项已在内核 v5.14 中被移除。)
