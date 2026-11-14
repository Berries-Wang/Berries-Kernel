.. SPDX-License-Identifier: GPL-2.0

=========================
BeOS filesystem for Linux
=========================

(Linux 的 BeOS 文件系统)

Document last updated: Dec 6, 2001
(文档最后更新于: 2001年12月6日)

Warning
(警告)
=======
Make sure you understand that this is alpha software.  This means that the
implementation is neither complete nor well-tested.
(请确保你理解这是一个 alpha 软件。这意味着该实现既不完整，也未经过充分测试。)

I DISCLAIM ALL RESPONSIBILITY FOR ANY POSSIBLE BAD EFFECTS OF THIS CODE!
(对于本代码可能造成的任何不良影响，我概不负责！)

License
(许可)
=======
This software is covered by the GNU General Public License.
See the file COPYING for the complete text of the license.
Or the GNU website: <http://www.gnu.org/licenses/licenses.html>
(本软件由 GNU 通用公共许可证(GNU General Public License)覆盖。许可证的完整文本请参见 COPYING 文件，或 GNU 网站: <http://www.gnu.org/licenses/licenses.html>)

Author
(作者)
======
The largest part of the code written by Will Dyson <will_dyson@pobox.com>
He has been working on the code since Aug 13, 2001. See the changelog for
details.
(代码的大部分由 Will Dyson <will_dyson@pobox.com> 编写。自 2001 年 8 月 13 日起，他一直致力于该代码的工作。详情请参见更新日志。)

Original Author: Makoto Kato <m_kato@ga2.so-net.ne.jp>
(原作者: Makoto Kato <m_kato@ga2.so-net.ne.jp>)

His original code can still be found at:
<http://hp.vector.co.jp/authors/VA008030/bfs/>
(他的原始代码仍可在以下位置找到：
<http://hp.vector.co.jp/authors/VA008030/bfs/>)

Does anyone know of a more current email address for Makoto? He doesn't
respond to the address given above...
(有人知道 Makoto 更新一点的电子邮件地址吗？他不再回复上面给出的地址……)

This filesystem doesn't have a maintainer.
(这个文件系统没有维护者。)

What is this Driver?
(这个驱动程序是什么？)
====================
This module implements the native filesystem of BeOS http://www.beincorporated.com/
for the linux 2.4.1 and later kernels. Currently it is a read-only
implementation.
(本模块为 linux 2.4.1 及后续内核实现了 BeOS http://www.beincorporated.com/ 的原生文件系统。目前它是一个只读实现。)

Which is it, BFS or BEFS?
(到底是 BFS 还是 BEFS？)
=========================
Be, Inc said, "BeOS Filesystem is officially called BFS, not BeFS".
But Unixware Boot Filesystem is called bfs, too. And they are already in
the kernel. Because of this naming conflict, on Linux the BeOS
filesystem is called befs.
(Be, Inc 曾表示，“BeOS Filesystem 的正式名称是 BFS，而不是 BeFS”。但 Unixware Boot Filesystem 也叫 bfs，而且它们已经在内核之中。由于这一命名冲突，在 Linux 上 BeOS 文件系统被称为 befs。)

How to Install
(如何安装)
==============
step 1.  Install the BeFS  patch into the source code tree of linux.
(步骤 1.  将 BeFS 补丁安装到 linux 源代码树中。)

Apply the patchfile to your kernel source tree.
Assuming that your kernel source is in /foo/bar/linux and the patchfile
is called patch-befs-xxx, you would do the following:
(将补丁文件应用到你的内核源代码树。假设你的内核源代码位于 /foo/bar/linux，补丁文件名为 patch-befs-xxx，那么你需要执行以下操作：)

	cd /foo/bar/linux
	patch -p1 < /path/to/patch-befs-xxx

if the patching step fails (i.e. there are rejected hunks), you can try to
figure it out yourself (it shouldn't be hard), or mail the maintainer
(Will Dyson <will_dyson@pobox.com>) for help.
(如果打补丁的步骤失败(即存在被拒绝的补丁块)，你可以尝试自己解决(应该不难)，或者发邮件给维护者(Will Dyson <will_dyson@pobox.com>)寻求帮助。)

step 2.  Configuration & make kernel
(步骤 2.  配置并编译内核)

The linux kernel has many compile-time options. Most of them are beyond the
scope of this document. I suggest the Kernel-HOWTO document as a good general
reference on this topic. http://www.linuxdocs.org/HOWTOs/Kernel-HOWTO-4.html
(linux 内核有许多编译时选项，其中大部分超出了本文档的范围。我建议将 Kernel-HOWTO 文档作为关于该主题的一个很好的通用参考。http://www.linuxdocs.org/HOWTOs/Kernel-HOWTO-4.html)

However, to use the BeFS module, you must enable it at configure time::
(不过，要使用 BeFS 模块，你必须在配置时启用它：)

	cd /foo/bar/linux
	make menuconfig (or xconfig)

The BeFS module is not a standard part of the linux kernel, so you must first
enable support for experimental code under the "Code maturity level" menu.
(BeFS 模块不是 linux 内核的标准组成部分，因此你必须首先在 "Code maturity level" 菜单下启用对实验性代码的支持。)

Then, under the "Filesystems" menu will be an option called "BeFS
filesystem (experimental)", or something like that. Enable that option
(it is fine to make it a module).
(然后，在 "Filesystems" 菜单下会有一个名为 "BeFS filesystem (experimental)" 或类似名称的选项。启用该选项(把它做成模块也没问题)。)

Save your kernel configuration and then build your kernel.
(保存你的内核配置，然后编译你的内核。)

step 3.  Install
(步骤 3.  安装)

See the kernel howto <http://www.linux.com/howto/Kernel-HOWTO.html> for
instructions on this critical step.
(关于这一关键步骤的说明，请参阅内核 howto <http://www.linux.com/howto/Kernel-HOWTO.html>。)

Using BFS
(使用 BFS)
=========
To use the BeOS filesystem, use filesystem type 'befs'.
(要使用 BeOS 文件系统，请使用文件系统类型 'befs'。)

ex::
(例如：)

    mount -t befs /dev/fd0 /beos

Mount Options
(挂载选项)
=============

=============  ===========================================================
uid=nnn        All files in the partition will be owned by user id nnn.(分区中的所有文件将属于用户 id nnn。)
gid=nnn	       All files in the partition will be in group nnn.(分区中的所有文件将属于组 nnn。)
iocharset=xxx  Use xxx as the name of the NLS translation table.(使用 xxx 作为 NLS 转换表的名称。)
debug          The driver will output debugging information to the syslog.(驱动程序将向 syslog 输出调试信息。)
=============  ===========================================================

How to Get Latest Version
(如何获取最新版本)
=========================

The latest version is currently available at:
<http://befs-driver.sourceforge.net/>
(最新版本目前可在以下位置获取：
<http://befs-driver.sourceforge.net/>)

Any Known Bugs?
(已知的缺陷？)
===============
As of Jan 20, 2002:
(截至 2002 年 1 月 20 日：)

	None
	(无)

Special Thanks
(特别致谢)
==============
Dominic Giampalo ... Writing "Practical file system design with Be filesystem"
(Dominic Giampalo ... 著有 "Practical file system design with Be filesystem" 一书)

Hiroyuki Yamada  ... Testing LinuxPPC.
(Hiroyuki Yamada  ... 测试 LinuxPPC。)



