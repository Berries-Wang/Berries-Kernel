.. SPDX-License-Identifier: GPL-2.0

==================================
Macintosh HFS Filesystem for Linux
==================================

(面向 Linux 的 Macintosh HFS 文件系统)


.. Note:: This filesystem doesn't have a maintainer.

(注意：此文件系统没有维护者。)


HFS stands for ``Hierarchical File System`` and is the filesystem used
by the Mac Plus and all later Macintosh models.  Earlier Macintosh
models used MFS (``Macintosh File System``), which is not supported,
MacOS 8.1 and newer support a filesystem called HFS+ that's similar to
HFS but is extended in various areas.  Use the hfsplus filesystem driver
to access such filesystems from Linux.
(HFS 代表 ``Hierarchical File System``(分层文件系统)，是 Mac Plus 及其后所有 Macintosh 机型使用的文件系统。更早的 Macintosh 机型使用 MFS(``Macintosh File System``)，它不受支持。MacOS 8.1 及更新版本支持一种名为 HFS+ 的文件系统，它与 HFS 相似但在多个方面有所扩展。在 Linux 中请使用 hfsplus 文件系统驱动程序来访问这类文件系统。)


Mount options
(挂载选项)
=============

When mounting an HFS filesystem, the following options are accepted:
(挂载 HFS 文件系统时，接受以下选项：)

  creator=cccc, type=cccc
	Specifies the creator/type values as shown by the MacOS finder
	used for creating new files.  Default values: '????'.
	(指定创建新文件时使用的 creator/type 值，如同 MacOS finder 所显示的那样。默认值：'????'。)

  uid=n, gid=n
  	Specifies the user/group that owns all files on the filesystems.
	Default:  user/group id of the mounting process.
	(指定拥有该文件系统上所有文件的用户/组。默认：挂载进程的用户/组 id。)

  dir_umask=n, file_umask=n, umask=n
	Specifies the umask used for all files , all directories or all
	files and directories.  Defaults to the umask of the mounting process.
	(指定用于所有文件、所有目录或全部文件和目录的 umask。默认为挂载进程的 umask。)

  session=n
  	Select the CDROM session to mount as HFS filesystem.  Defaults to
	leaving that decision to the CDROM driver.  This option will fail
	with anything but a CDROM as underlying devices.
	(选择要作为 HFS 文件系统挂载的 CDROM 会话。默认将该决定留给 CDROM 驱动程序。如果底层设备不是 CDROM，此选项将会失败。)

  part=n
  	Select partition number n from the devices.  Does only makes
	sense for CDROMS because they can't be partitioned under Linux.
	For disk devices the generic partition parsing code does this
	for us.  Defaults to not parsing the partition table at all.
	(从设备中选择分区号 n。这仅对 CDROM 有意义，因为它们在 Linux 下无法被分区。对于磁盘设备，通用的分区解析代码会为我们完成这项工作。默认完全不解析分区表。)

  quiet
  	Ignore invalid mount options instead of complaining.
	(忽略无效的挂载选项，而不是报错抱怨。)


Writing to HFS Filesystems
(写入 HFS 文件系统)
==========================

HFS is not a UNIX filesystem, thus it does not have the usual features you'd
expect:
(HFS 不是 UNIX 文件系统，因此它不具备你所期望的通常特性：)

 * You can't modify the set-uid, set-gid, sticky or executable bits or the uid
   and gid of files.
   (你不能修改文件的 set-uid、set-gid、sticky 或可执行位，以及文件的 uid 和 gid。)
 * You can't create hard- or symlinks, device files, sockets or FIFOs.
   (你不能创建硬链接或符号链接、设备文件、套接字或 FIFO。)

HFS does on the other have the concepts of multiple forks per file.  These
non-standard forks are represented as hidden additional files in the normal
filesystems namespace which is kind of a cludge and makes the semantics for
the a little strange:
(另一方面，HFS 确实具有每文件多个 fork 的概念。这些非标准的 fork 在普通文件系统的命名空间中表示为隐藏的附加文件，这有点像是拼凑而成，并使其语义变得有点奇怪：)

 * You can't create, delete or rename resource forks of files or the
   Finder's metadata.
   (你不能创建、删除或重命名文件的资源 fork 或 Finder 的元数据。)
 * They are however created (with default values), deleted and renamed
   along with the corresponding data fork or directory.
   (不过，它们会随对应的数据 fork 或目录一起被创建(使用默认值)、删除和重命名。)
 * Copying files to a different filesystem will loose those attributes
   that are essential for MacOS to work.
   (将文件复制到其他文件系统会丢失那些对 MacOS 正常工作必不可少的属性。)


Creating HFS filesystems
(创建 HFS 文件系统)
========================

The hfsutils package from Robert Leslie contains a program called
hformat that can be used to create HFS filesystem. See
<https://www.mars.org/home/rob/proj/hfs/> for details.
(Robert Leslie 的 hfsutils 软件包包含一个名为 hformat 的程序，可用于创建 HFS 文件系统。详情请参见 <https://www.mars.org/home/rob/proj/hfs/>。)


Credits
(致谢)
=======

The HFS drivers was written by Paul H. Hargrovea (hargrove@sccm.Stanford.EDU).
Roman Zippel (roman@ardistech.com) rewrote large parts of the code and brought
in btree routines derived from Brad Boyer's hfsplus driver.
(HFS 驱动程序由 Paul H. Hargrovea (hargrove@sccm.Stanford.EDU) 编写。Roman Zippel (roman@ardistech.com) 重写了代码的大部分内容，并引入了源自 Brad Boyer 的 hfsplus 驱动程序的 btree 例程。)
