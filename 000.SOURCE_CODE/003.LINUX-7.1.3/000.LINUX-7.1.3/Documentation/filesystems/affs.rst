.. SPDX-License-Identifier: GPL-2.0

=============================
Overview of Amiga Filesystems
=============================

(Amiga 文件系统概览)

Not all varieties of the Amiga filesystems are supported for reading and
writing. The Amiga currently knows six different filesystems:
(并非所有种类的 Amiga 文件系统都支持读写。Amiga 目前已知六种不同的文件系统：)

==============	===============================================================
DOS\0		The old or original filesystem, not really suited for
		hard disks and normally not used on them, either.
		Supported read/write. (旧的或原始的文件系统，并不真正适合硬盘，通常也不在硬盘上使用。支持读写。)

DOS\1		The original Fast File System. Supported read/write. (原始的快速文件系统。支持读写。)

DOS\2		The old "international" filesystem. International means that
		a bug has been fixed so that accented ("international") letters
		in file names are case-insensitive, as they ought to be.
		Supported read/write. (旧的"国际"文件系统。"国际"是指修复了一个缺陷，使得文件名中的重音("国际")字母按其应有的方式不区分大小写。支持读写。)

DOS\3		The "international" Fast File System.  Supported read/write. ("国际"快速文件系统。支持读写。)

DOS\4		The original filesystem with directory cache. The directory
		cache speeds up directory accesses on floppies considerably,
		but slows down file creation/deletion. Doesn't make much
		sense on hard disks. Supported read only. (带目录缓存的原始文件系统。目录缓存大大加快了软盘上的目录访问速度，但减慢了文件的创建/删除。在硬盘上意义不大。只支持读。)

DOS\5		The Fast File System with directory cache. Supported read only. (带目录缓存的快速文件系统。只支持读。)
==============	===============================================================

All of the above filesystems allow block sizes from 512 to 32K bytes.
Supported block sizes are: 512, 1024, 2048 and 4096 bytes. Larger blocks
speed up almost everything at the expense of wasted disk space. The speed
gain above 4K seems not really worth the price, so you don't lose too
much here, either.
(上述所有文件系统都允许 512 到 32K 字节的块大小。支持的块大小为：512、1024、2048 和 4096 字节。更大的块以浪费磁盘空间为代价加快了几乎所有的操作。4K 以上带来的速度提升似乎并不值得付出这个代价，所以在这方面你也不会损失太多。)

The muFS (multi user File System) equivalents of the above file systems
are supported, too.
(上述文件系统的 muFS(多用户文件系统)等价形式也受支持。)

Mount options for the AFFS
(AFFS 的挂载选项)
==========================

protect
		If this option is set, the protection bits cannot be altered.
		(如果设置了此选项，保护位将不能被更改。)

setuid[=uid]
		This sets the owner of all files and directories in the file
		system to uid or the uid of the current user, respectively.
		(这将把文件系统中所有文件和目录的属主分别设置为 uid 或当前用户的 uid。)

setgid[=gid]
		Same as above, but for gid.
		(与上面相同，但针对 gid。)

mode=mode
		Sets the mode flags to the given (octal) value, regardless
		of the original permissions. Directories will get an x
		permission if the corresponding r bit is set.
		This is useful since most of the plain AmigaOS files
		will map to 600.
		(将模式标志设置为给定的(八进制)值，而不考虑原始权限。如果设置了相应的 r 位，目录将获得 x 权限。这很有用，因为大多数普通 AmigaOS 文件会映射到 600。)

nofilenametruncate
		The file system will return an error when filename exceeds
		standard maximum filename length (30 characters).
		(当文件名超过标准最大文件名长度(30 个字符)时，文件系统将返回错误。)

reserved=num
		Sets the number of reserved blocks at the start of the
		partition to num. You should never need this option.
		Default is 2.
		(将分区开头保留块的数量设置为 num。你应该永远不需要此选项。默认值为 2。)

root=block
		Sets the block number of the root block. This should never
		be necessary.
		(设置根块的块号。这应该是永远没有必要的。)

bs=blksize
		Sets the blocksize to blksize. Valid block sizes are 512,
		1024, 2048 and 4096. Like the root option, this should
		never be necessary, as the affs can figure it out itself.
		(将块大小设置为 blksize。有效的块大小为 512、1024、2048 和 4096。与 root 选项一样，这应该是永远没有必要的，因为 affs 可以自己推断出来。)

quiet
		The file system will not return an error for disallowed
		mode changes.
		(对于不允许的模式更改，文件系统将不返回错误。)

verbose
		The volume name, file system type and block size will
		be written to the syslog when the filesystem is mounted.
		(当文件系统被挂载时，卷名、文件系统类型和块大小将被写入 syslog。)

mufs
		The filesystem is really a muFS, also it doesn't
		identify itself as one. This option is necessary if
		the filesystem wasn't formatted as muFS, but is used
		as one.
		(该文件系统实际上是 muFS，只是它并没有把自己标识为 muFS。如果文件系统没有被格式化为 muFS，却被当作 muFS 使用，则此选项是必需的。)

prefix=path
		Path will be prefixed to every absolute path name of
		symbolic links on an AFFS partition. Default = "/".
		(See below.)
		(Path 将被加为 AFFS 分区上符号链接的每个绝对路径名的前缀。默认 = "/"。(见下文。))

volume=name
		When symbolic links with an absolute path are created
		on an AFFS partition, name will be prepended as the
		volume name. Default = "" (empty string).
		(See below.)
		(当在 AFFS 分区上创建带绝对路径的符号链接时，name 将被前置为卷名。默认 = ""(空字符串)。(见下文。))

Handling of the Users/Groups and protection flags
(用户/组与保护位的处理)
=================================================

Amiga -> Linux:
(Amiga -> Linux：)

The Amiga protection flags RWEDRWEDHSPARWED are handled as follows:
(Amiga 保护标志 RWEDRWEDHSPARWED 的处理方式如下：)

  - R maps to r for user, group and others. On directories, R implies x.
    (R 映射为用户、组和其他人的 r。对于目录，R 隐含 x。)

  - W maps to w.
    (W 映射为 w。)

  - E maps to x.
    (E 映射为 x。)

  - D is ignored.
    (D 被忽略。)

  - H, S and P are always retained and ignored under Linux.
    (H、S 和 P 在 Linux 下总是被保留并被忽略。)

  - A is cleared when a file is written to.
    (当文件被写入时，A 被清除。)

User id and group id will be used unless set[gu]id are given as mount
options. Since most of the Amiga file systems are single user systems
they will be owned by root. The root directory (the mount point) of the
Amiga filesystem will be owned by the user who actually mounts the
filesystem (the root directory doesn't have uid/gid fields).
(除非将 set[gu]id 作为挂载选项给出，否则将使用用户 ID 和组 ID。由于大多数 Amiga 文件系统是单用户系统，它们将由 root 拥有。Amiga 文件系统的根目录(挂载点)将由实际挂载该文件系统的用户拥有(根目录没有 uid/gid 字段)。)

Linux -> Amiga:
(Linux -> Amiga：)

The Linux rwxrwxrwx file mode is handled as follows:
(Linux 的 rwxrwxrwx 文件模式按如下方式处理：)

  - r permission will allow R for user, group and others.
    (r 权限将允许用户、组和其他人进行 R。)

  - w permission will allow W for user, group and others.
    (w 权限将允许用户、组和其他人进行 W。)

  - x permission of the user will allow E for plain files.
    (用户的 x 权限将允许对普通文件进行 E。)

  - D will be allowed for user, group and others.
    (用户、组和其他人将被允许 D。)

  - All other flags (suid, sgid, ...) are ignored and will
    not be retained.
    (所有其他标志(suid、sgid 等)都会被忽略，且不会被保留。)

Newly created files and directories will get the user and group ID
of the current user and a mode according to the umask.
(新建的文件和目录将获得当前用户的用户 ID 和组 ID，以及根据 umask 得到的模式。)

Symbolic links
(符号链接)
==============

Although the Amiga and Linux file systems resemble each other, there
are some, not always subtle, differences. One of them becomes apparent
with symbolic links. While Linux has a file system with exactly one
root directory, the Amiga has a separate root directory for each
file system (for example, partition, floppy disk, ...). With the Amiga,
these entities are called "volumes". They have symbolic names which
can be used to access them. Thus, symbolic links can point to a
different volume. AFFS turns the volume name into a directory name
and prepends the prefix path (see prefix option) to it.
(尽管 Amiga 和 Linux 文件系统彼此相似，但仍存在一些并不总是细微的差异。其中一个差异在符号链接上变得明显。Linux 的文件系统只有一个根目录，而 Amiga 为每个文件系统(例如，分区、软盘等)都有一个单独的根目录。在 Amiga 上，这些实体被称为"卷"。它们拥有可用于访问它们的符号名称。因此，符号链接可以指向不同的卷。AFFS 将卷名转换为目录名，并把前缀路径(参见 prefix 选项)加在它前面。)

Example:
You mount all your Amiga partitions under /amiga/<volume> (where
<volume> is the name of the volume), and you give the option
"prefix=/amiga/" when mounting all your AFFS partitions. (They
might be "User", "WB" and "Graphics", the mount points /amiga/User,
/amiga/WB and /amiga/Graphics). A symbolic link referring to
"User:sc/include/dos/dos.h" will be followed to
"/amiga/User/sc/include/dos/dos.h".
(示例：你把所有 Amiga 分区挂载到 /amiga/<volume> 之下(其中 <volume> 是卷的名称)，并且在挂载所有 AFFS 分区时给出选项 "prefix=/amiga/"。(它们可能是 "User"、"WB" 和 "Graphics"，即挂载点 /amiga/User、/amiga/WB 和 /amiga/Graphics)。指向 "User:sc/include/dos/dos.h" 的符号链接将被解析为 "/amiga/User/sc/include/dos/dos.h"。)

Examples
(示例)
========

Command line::
(命令行：)

    mount  Archive/Amiga/Workbench3.1.adf /mnt -t affs -o loop,verbose
    mount  /dev/sda3 /Amiga -t affs

/etc/fstab entry::
(/etc/fstab 条目：)

    /dev/sdb5	/amiga/Workbench    affs    noauto,user,exec,verbose 0 0

IMPORTANT NOTE
(重要说明)
==============

If you boot Windows 95 (don't know about 3.x, 98 and NT) while you
have an Amiga harddisk connected to your PC, it will overwrite
the bytes 0x00dc..0x00df of block 0 with garbage, thus invalidating
the Rigid Disk Block. Sheer luck has it that this is an unused
area of the RDB, so only the checksum doesn't match anymore.
Linux will ignore this garbage and recognize the RDB anyway, but
before you connect that drive to your Amiga again, you must
restore or repair your RDB. So please do make a backup copy of it
before booting Windows!
(如果你在 PC 上连接着 Amiga 硬盘的情况下启动 Windows 95(不清楚 3.x、98 和 NT 的情况)，它会用垃圾数据覆写块 0 的字节 0x00dc..0x00df，从而使 Rigid Disk Block 失效。万幸的是，这是 RDB 中一块未使用的区域，所以只是校验和不再匹配而已。Linux 会忽略这些垃圾数据并照样识别出 RDB，但在你把该硬盘重新接到 Amiga 之前，你必须恢复或修复你的 RDB。所以请在启动 Windows 之前务必备份一份它的副本！)

If the damage is already done, the following should fix the RDB
(where <disk> is the device name).
(如果损坏已经造成，以下步骤应该能修复 RDB(其中 <disk> 是设备名)。)

DO AT YOUR OWN RISK::
(风险自负地执行：)

  dd if=/dev/<disk> of=rdb.tmp count=1
  cp rdb.tmp rdb.fixed
  dd if=/dev/zero of=rdb.fixed bs=1 seek=220 count=4
  dd if=rdb.fixed of=/dev/<disk>

Bugs, Restrictions, Caveats
(缺陷、限制与注意事项)
===========================

Quite a few things may not work as advertised. Not everything is
tested, though several hundred MB have been read and written using
this fs. For a most up-to-date list of bugs please consult
fs/affs/Changes.
(相当多的事情可能不会像宣称的那样工作。并非一切都经过了测试，不过已经用这个文件系统读写了几百 MB 的数据。要获取最新的缺陷列表，请查阅 fs/affs/Changes。)

By default, filenames are truncated to 30 characters without warning.
'nofilenametruncate' mount option can change that behavior.
(默认情况下，文件名会在无警告的情况下被截断为 30 个字符。'nofilenametruncate' 挂载选项可以改变该行为。)

Case is ignored by the affs in filename matching, but Linux shells
do care about the case. Example (with /wb being an affs mounted fs)::
(affs 在文件名匹配时不区分大小写，但 Linux shell 却在意大小写。示例(其中 /wb 是一个已挂载的 affs 文件系统)：)

    rm /wb/WRONGCASE

will remove /mnt/wrongcase, but::
(将删除 /mnt/wrongcase，但是：)

    rm /wb/WR*

will not since the names are matched by the shell.
(由于名称是由 shell 匹配的，所以不会删除。)

The block allocation is designed for hard disk partitions. If more
than 1 process writes to a (small) diskette, the blocks are allocated
in an ugly way (but the real AFFS doesn't do much better). This
is also true when space gets tight.
(块分配是为硬盘分区设计的。如果超过 1 个进程写入一个(小)软盘，块将以难看的方式分配(但真正的 AFFS 也不见得做得更好)。当空间变得紧张时也是如此。)

You cannot execute programs on an OFS (Old File System), since the
program files cannot be memory mapped due to the 488 byte blocks.
For the same reason you cannot mount an image on such a filesystem
via the loopback device.
(你无法在 OFS(旧文件系统)上执行程序，因为由于 488 字节的块，程序文件无法被内存映射。出于同样的原因，你也无法通过回环设备在此类文件系统上挂载镜像。)

The bitmap valid flag in the root block may not be accurate when the
system crashes while an affs partition is mounted. There's currently
no way to fix a garbled filesystem without an Amiga (disk validator)
or manually (who would do this?). Maybe later.
(当挂载着 affs 分区的系统崩溃时，根块中的位图有效标志可能不准确。目前，在没有 Amiga(磁盘校验器)或不进行手动操作(谁会这么做？)的情况下，没有办法修复损坏的文件系统。也许以后会有。)

If you mount affs partitions on system startup, you may want to tell
fsck that the fs should not be checked (place a '0' in the sixth field
of /etc/fstab).
(如果你在系统启动时挂载 affs 分区，你可能想要告诉 fsck 不应检查该文件系统(在 /etc/fstab 的第六个字段放置 '0')。)

It's not possible to read floppy disks with a normal PC or workstation
due to an incompatibility with the Amiga floppy controller.
(由于与 Amiga 软盘控制器不兼容，无法用普通 PC 或工作站读取软盘。)

If you are interested in an Amiga Emulator for Linux, look at
(如果你对 Linux 下的 Amiga 模拟器感兴趣，请看：)

http://web.archive.org/web/%2E/http://www.freiburg.linux.de/~uae/
