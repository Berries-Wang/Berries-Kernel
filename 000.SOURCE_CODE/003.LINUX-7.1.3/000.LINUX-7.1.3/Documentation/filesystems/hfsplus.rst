.. SPDX-License-Identifier: GPL-2.0

======================================
Macintosh HFSPlus Filesystem for Linux
======================================

(Linux 的 Macintosh HFSPlus 文件系统)

HFSPlus is a filesystem first introduced in MacOS 8.1.
HFSPlus has several extensions to HFS, including 32-bit allocation
blocks, 255-character unicode filenames, and file sizes of 2^63 bytes.
(HFSPlus 是最早在 MacOS 8.1 中引入的文件系统。HFSPlus 对 HFS 进行了多项扩展，包括 32 位分配块、255 字符的 Unicode 文件名以及 2^63 字节的文件大小。)


Mount options
(挂载选项)
=============

When mounting an HFSPlus filesystem, the following options are accepted:
(挂载 HFSPlus 文件系统时，接受以下选项：)

  creator=cccc, type=cccc
	Specifies the creator/type values as shown by the MacOS finder
	used for creating new files.  Default values: '????'.
	(指定创建新文件时使用的创建者/类型值，如 MacOS finder 所显示的那样。默认值：'????'。)

  uid=n, gid=n
	Specifies the user/group that owns all files on the filesystem
	that have uninitialized permissions structures.
	Default:  user/group id of the mounting process.
	(指定拥有文件系统上所有具有未初始化权限结构的文件的用户/组。默认：挂载进程的用户/组 id。)

  umask=n
	Specifies the umask (in octal) used for files and directories
	that have uninitialized permissions structures.
	Default:  umask of the mounting process.
	(指定用于具有未初始化权限结构的文件和目录的 umask(八进制)。默认：挂载进程的 umask。)

  session=n
	Select the CDROM session to mount as HFSPlus filesystem.  Defaults to
	leaving that decision to the CDROM driver.  This option will fail
	with anything but a CDROM as underlying devices.
	(选择要挂载为 HFSPlus 文件系统的 CDROM 区段。默认将该决定留给 CDROM 驱动程序。如果底层设备不是 CDROM，此选项将失败。)

  part=n
	Select partition number n from the devices.  This option only makes
	sense for CDROMs because they can't be partitioned under Linux.
	For disk devices the generic partition parsing code does this
	for us.  Defaults to not parsing the partition table at all.
	(从设备中选择分区号 n。此选项仅对 CDROM 有意义，因为在 Linux 下无法对它们进行分区。对于磁盘设备，通用分区解析代码会为我们完成这项工作。默认完全不解析分区表。)

  decompose
	Decompose file name characters.
	(分解文件名字符。)

  nodecompose
	Do not decompose file name characters.
	(不分解文件名字符。)

  force
	Used to force write access to volumes that are marked as journalled
	or locked.  Use at your own risk.
	(用于强制对标记为已记日志或已锁定的卷进行写访问。使用风险自负。)

  nls=cccc
	Encoding to use when presenting file names.
	(显示文件名时使用的编码。)


References
(参考文献)
==========

kernel source:		<file:fs/hfsplus>
(内核源代码:		<file:fs/hfsplus>)

Apple Technote 1150	https://developer.apple.com/legacy/library/technotes/tn/tn1150.html
(Apple 技术笔记 1150	https://developer.apple.com/legacy/library/technotes/tn/tn1150.html)
