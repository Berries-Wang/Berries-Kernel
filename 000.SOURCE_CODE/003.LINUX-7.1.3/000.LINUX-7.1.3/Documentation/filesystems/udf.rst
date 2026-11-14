.. SPDX-License-Identifier: GPL-2.0

===============
UDF file system
===============

(UDF 文件系统)

If you encounter problems with reading UDF discs using this driver,
please report them according to MAINTAINERS file.
(如果你在使用此驱动程序读取 UDF 光盘时遇到问题，请按照 MAINTAINERS 文件进行报告。)

Write support requires a block driver which supports writing.  Currently
dvd+rw drives and media support true random sector writes, and so a udf
filesystem on such devices can be directly mounted read/write.  CD-RW
media however, does not support this.  Instead the media can be formatted
for packet mode using the utility cdrwtool, then the pktcdvd driver can
be bound to the underlying cd device to provide the required buffering
and read-modify-write cycles to allow the filesystem random sector writes
while providing the hardware with only full packet writes.  While not
required for dvd+rw media, use of the pktcdvd driver often enhances
performance due to very poor read-modify-write support supplied internally
by drive firmware.
(写支持需要一个支持写入的块设备驱动程序。目前 dvd+rw 驱动器和介质支持真正的随机扇区写入，因此此类设备上的 udf 文件系统可以直接以读写方式挂载。然而 CD-RW 介质不支持这一点。作为替代，可以使用 cdrwtool 工具将介质格式化为分组(packet)模式，然后将 pktcdvd 驱动程序绑定到底层 cd 设备，以提供所需的缓冲和读-改-写循环，使文件系统能够进行随机扇区写入，同时只向硬件提供完整的分组写入。虽然 dvd+rw 介质并不需要这样做，但由于驱动器固件内部提供的读-改-写支持非常糟糕，使用 pktcdvd 驱动程序通常能提高性能。)

-------------------------------------------------------------------------------

The following mount options are supported:
(支持以下挂载选项：)

	===========	======================================
	gid=		Set the default group. (设置默认组。)
	umask=		Set the default umask. (设置默认 umask。)
	mode=		Set the default file permissions. (设置默认文件权限。)
	dmode=		Set the default directory permissions. (设置默认目录权限。)
	uid=		Set the default user. (设置默认用户。)
	bs=		Set the block size. (设置块大小。)
	unhide		Show otherwise hidden files. (显示原本隐藏的文件。)
	undelete	Show deleted files in lists. (在列表中显示已删除的文件。)
	adinicb		Embed data in the inode (default) (将数据嵌入 inode(默认))
	noadinicb	Don't embed data in the inode (不将数据嵌入 inode)
	shortad		Use short ad's (使用短 ad)
	longad		Use long ad's (default) (使用长 ad(默认))
	nostrict	Unset strict conformance (取消严格一致性)
	iocharset=	Set the NLS character set (设置 NLS 字符集)
	===========	======================================

The uid= and gid= options need a bit more explaining.  They will accept a
decimal numeric value and all inodes on that mount will then appear as
belonging to that uid and gid.  Mount options also accept the string "forget".
The forget option causes all IDs to be written to disk as -1 which is a way
of UDF standard to indicate that IDs are not supported for these files .
(uid= 和 gid= 选项需要再多解释一下。它们接受十进制数值，该挂载上的所有 inode 随后将显示为属于该 uid 和 gid。挂载选项还接受字符串 "forget"。forget 选项使所有 ID 以 -1 写入磁盘，这是 UDF 标准用来表示这些文件不支持 ID 的方式。)

For typical desktop use of removable media, you should set the ID to that of
the interactively logged on user, and also specify the forget option.  This way
the interactive user will always see the files on the disk as belonging to him.
(对于可移动介质的典型桌面使用，你应该将 ID 设置为交互式登录用户的 ID，并同时指定 forget 选项。这样，交互式用户将始终看到磁盘上的文件属于自己。)

The remaining are for debugging and disaster recovery:
(其余选项用于调试和灾难恢复：)

	=====		================================
	novrs		Skip volume sequence recognition (跳过卷序列识别)
	=====		================================

The following expect a offset from 0.
(以下选项期望一个从 0 开始的偏移量。)

	==========	=================================================
	session=	Set the CDROM session (default= last session) (设置 CDROM 区段(默认=最后一个区段))
	anchor=		Override standard anchor location. (default= 256) (覆盖标准锚点位置。(默认= 256))
	lastblock=	Set the last block of the filesystem/ (设置文件系统的最后一个块/)
	==========	=================================================

-------------------------------------------------------------------------------


For the latest version and toolset see:
(最新版本和工具集请参见：)
	https://github.com/pali/udftools

Documentation on UDF and ECMA 167 is available FREE from:
(关于 UDF 和 ECMA 167 的文档可以从以下位置免费获取：)
	- http://www.osta.org/
	- https://www.ecma-international.org/
