.. SPDX-License-Identifier: GPL-2.0

====================
Read/Write HPFS 2.09
====================

(读/写 HPFS 2.09)

1998-2004, Mikulas Patocka
(1998-2004，Mikulas Patocka)

:email: mikulas@artax.karlin.mff.cuni.cz
(电子邮件: mikulas@artax.karlin.mff.cuni.cz)
:homepage: https://artax.karlin.mff.cuni.cz/~mikulas/vyplody/hpfs/index-e.cgi
(主页: https://artax.karlin.mff.cuni.cz/~mikulas/vyplody/hpfs/index-e.cgi)

Credits
(致谢)
=======
Chris Smith, 1993, original read-only HPFS, some code and hpfs structures file
	is taken from it
	(Chris Smith，1993 年，最初的只读 HPFS，其中一些代码和 hpfs 结构文件取自该实现)

Jacques Gelinas, MSDos mmap, Inspired by fs/nfs/mmap.c (Jon Tombs 15 Aug 1993)
(Jacques Gelinas，MSDos mmap，灵感来自 fs/nfs/mmap.c(Jon Tombs，1993 年 8 月 15 日))

Werner Almesberger, 1992, 1993, MSDos option parser & CR/LF conversion
(Werner Almesberger，1992、1993 年，MSDos 选项解析器与 CR/LF 转换)

Mount options
(挂载选项)

uid=xxx,gid=xxx,umask=xxx (default uid=gid=0 umask=default_system_umask)
	Set owner/group/mode for files that do not have it specified in extended
	attributes. Mode is inverted umask - for example umask 027 gives owner
	all permission, group read permission and anybody else no access. Note
	that for files mode is anded with 0666. If you want files to have 'x'
	rights, you must use extended attributes.
	(为扩展属性中没有指定所有者/属组/模式的文件设置所有者/属组/模式。模式是 umask 的反码——例如 umask 027 给属主全部权限、属组读权限、其他所有人无权限。注意，对文件来说模式会与 0666 相与。如果你想让文件拥有 'x' 权限，必须使用扩展属性。)
case=lower,asis (default asis)
	File name lowercasing in readdir.
	(readdir 中的文件名小写化。)
conv=binary,text,auto (default binary)
	CR/LF -> LF conversion, if auto, decision is made according to extension
	- there is a list of text extensions (I thing it's better to not convert
	text file than to damage binary file). If you want to change that list,
	change it in the source. Original readonly HPFS contained some strange
	heuristic algorithm that I removed. I thing it's danger to let the
	computer decide whether file is text or binary. For example, DJGPP
	binaries contain small text message at the beginning and they could be
	misidentified and damaged under some circumstances.
	(CR/LF -> LF 转换；如果为 auto，则根据扩展名来决定——有一个文本扩展名列表(我认为宁可不转换文本文件也不要损坏二进制文件)。如果你想更改那个列表，请在源代码中修改。最初的只读 HPFS 包含某种奇怪的启发式算法，我把它移除了。我认为让计算机来决定文件是文本还是二进制是危险的。例如，DJGPP 二进制文件开头包含一小段文本消息，在某些情况下它们可能被误认而遭到损坏。)
check=none,normal,strict (default normal)
	Check level. Selecting none will cause only little speedup and big
	danger. I tried to write it so that it won't crash if check=normal on
	corrupted filesystems. check=strict means many superfluous checks -
	used for debugging (for example it checks if file is allocated in
	bitmaps when accessing it).
	(检查级别。选择 none 只会带来很小的提速和很大的危险。我在编写时尽量做到：在已损坏的文件系统上使用 check=normal 时不会崩溃。check=strict 意味着大量多余的检查——用于调试(例如在访问文件时检查文件是否已在位图中分配)。)
errors=continue,remount-ro,panic (default remount-ro)
	Behaviour when filesystem errors found.
	(发现文件系统错误时的行为。)
chkdsk=no,errors,always (default errors)
	When to mark filesystem dirty so that OS/2 checks it.
	(何时把文件系统标记为脏，以便 OS/2 对它进行检查。)
eas=no,ro,rw (default rw)
	What to do with extended attributes. 'no' - ignore them and use always
	values specified in uid/gid/mode options. 'ro' - read extended
	attributes but do not create them. 'rw' - create extended attributes
	when you use chmod/chown/chgrp/mknod/ln -s on the filesystem.
	(如何处理扩展属性。'no'——忽略它们，并始终使用 uid/gid/mode 选项中指定的值。'ro'——读取扩展属性但不创建它们。'rw'——当你在文件系统上使用 chmod/chown/chgrp/mknod/ln -s 时创建扩展属性。)
timeshift=(-)nnn (default 0)
	Shifts the time by nnn seconds. For example, if you see under linux
	one hour more, than under os/2, use timeshift=-3600.
	(把时间偏移 nnn 秒。例如，如果你在 linux 下看到的比在 os/2 下多一个小时，请使用 timeshift=-3600。)


File names
(文件名)
==========

As in OS/2, filenames are case insensitive. However, shell thinks that names
are case sensitive, so for example when you create a file FOO, you can use
'cat FOO', 'cat Foo', 'cat foo' or 'cat F*' but not 'cat f*'. Note, that you
also won't be able to compile linux kernel (and maybe other things) on HPFS
because kernel creates different files with names like bootsect.S and
bootsect.s. When searching for file whose name has characters >= 128, codepages
are used - see below.
OS/2 ignores dots and spaces at the end of file name, so this driver does as
well. If you create 'a. ...', the file 'a' will be created, but you can still
access it under names 'a.', 'a..', 'a .  . . ' etc.
(与 OS/2 一样，文件名不区分大小写。然而，shell 认为名字是区分大小写的，所以例如当你创建文件 FOO 时，可以使用 'cat FOO'、'cat Foo'、'cat foo' 或 'cat F*'，但不能用 'cat f*'。注意，你在 HPFS 上也将无法编译 linux 内核(可能还有其他东西)，因为内核会创建像 bootsect.S 和 bootsect.s 这样名字不同的文件。在查找名字含有大于等于 128 的字符的文件时，会使用代码页——见下文。OS/2 忽略文件名末尾的点号和空格，本驱动也如此。如果你创建 'a. ...'，将会创建文件 'a'，但你仍然可以用 'a.'、'a..'、'a .  . . ' 等名字访问它。)


Extended attributes
(扩展属性)
===================

On HPFS partitions, OS/2 can associate to each file a special information called
extended attributes. Extended attributes are pairs of (key,value) where key is
an ascii string identifying that attribute and value is any string of bytes of
variable length. OS/2 stores window and icon positions and file types there. So
why not use it for unix-specific info like file owner or access rights? This
driver can do it. If you chown/chgrp/chmod on a hpfs partition, extended
attributes with keys "UID", "GID" or "MODE" and 2-byte values are created. Only
that extended attributes those value differs from defaults specified in mount
options are created. Once created, the extended attributes are never deleted,
they're just changed. It means that when your default uid=0 and you type
something like 'chown luser file; chown root file' the file will contain
extended attribute UID=0. And when you umount the fs and mount it again with
uid=luser_uid, the file will be still owned by root! If you chmod file to 444,
extended attribute "MODE" will not be set, this special case is done by setting
read-only flag. When you mknod a block or char device, besides "MODE", the
special 4-byte extended attribute "DEV" will be created containing the device
number. Currently this driver cannot resize extended attributes - it means
that if somebody (I don't know who?) has set "UID", "GID", "MODE" or "DEV"
attributes with different sizes, they won't be rewritten and changing these
values doesn't work.
(在 HPFS 分区上，OS/2 可以为每个文件关联一种称为扩展属性的特殊信息。扩展属性是(key,value)对，其中 key 是标识该属性的 ascii 字符串，value 是任意长度的字节串。OS/2 在那里存储窗口和图标位置以及文件类型。那么为什么不把它用于文件属主或访问权限这类 unix 特有的信息呢？本驱动可以做到。如果你在 hpfs 分区上执行 chown/chgrp/chmod，就会创建键为 "UID"、"GID" 或 "MODE"、值为 2 字节的扩展属性。只有那些值与挂载选项中指定的默认值不同的扩展属性才会被创建。一旦创建，扩展属性永远不会被删除，只会被修改。这意味着当你的默认 uid=0 时，如果你输入类似 'chown luser file; chown root file' 的命令，文件将包含扩展属性 UID=0。而当你 umount 该文件系统并以 uid=luser_uid 重新挂载它时，文件仍然属于 root！如果你把文件 chmod 为 444，扩展属性 "MODE" 不会被设置，这种特殊情况是通过设置只读标志来完成的。当你 mknod 一个块设备或字符设备时，除 "MODE" 之外，还会创建包含设备号的特殊 4 字节扩展属性 "DEV"。目前本驱动无法调整扩展属性的大小——这意味着如果有人(我不知道是谁？)设置了大小不同的 "UID"、"GID"、"MODE" 或 "DEV" 属性，它们不会被重写，修改这些值也不起作用。)


Symlinks
(符号链接)
========

You can do symlinks on HPFS partition, symlinks are achieved by setting extended
attribute named "SYMLINK" with symlink value. Like on ext2, you can chown and
chgrp symlinks but I don't know what is it good for. chmoding symlink results
in chmoding file where symlink points. These symlinks are just for Linux use and
incompatible with OS/2. OS/2 PmShell symlinks are not supported because they are
stored in very crazy way. They tried to do it so that link changes when file is
moved ... sometimes it works. But the link is partly stored in directory
extended attributes and partly in OS2SYS.INI. I don't want (and don't know how)
to analyze or change OS2SYS.INI.
(你可以在 HPFS 分区上使用符号链接，符号链接是通过设置名为 "SYMLINK"、值为符号链接目标的扩展属性来实现的。与 ext2 上一样，你可以对符号链接执行 chown 和 chgrp，但我不知道这有什么用处。对符号链接执行 chmod 的结果是对符号链接所指向的文件执行 chmod。这些符号链接仅供 Linux 使用，与 OS/2 不兼容。不支持 OS/2 PmShell 的符号链接，因为它们的存储方式非常古怪。他们试图做到链接随文件移动而变化……有时能行。但这个链接一部分存储在目录扩展属性中，一部分存储在 OS2SYS.INI 中。我不想(也不知道如何)分析或修改 OS2SYS.INI。)


Codepages
(代码页)
=========

HPFS can contain several uppercasing tables for several codepages and each
file has a pointer to codepage its name is in. However OS/2 was created in
America where people don't care much about codepages and so multiple codepages
support is quite buggy. I have Czech OS/2 working in codepage 852 on my disk.
Once I booted English OS/2 working in cp 850 and I created a file on my 852
partition. It marked file name codepage as 850 - good. But when I again booted
Czech OS/2, the file was completely inaccessible under any name. It seems that
OS/2 uppercases the search pattern with its system code page (852) and file
name it's comparing to with its code page (850). These could never match. Is it
really what IBM developers wanted? But problems continued. When I created in
Czech OS/2 another file in that directory, that file was inaccessible too. OS/2
probably uses different uppercasing method when searching where to place a file
(note, that files in HPFS directory must be sorted) and when searching for
a file. Finally when I opened this directory in PmShell, PmShell crashed (the
funny thing was that, when rebooted, PmShell tried to reopen this directory
again :-). chkdsk happily ignores these errors and only low-level disk
modification saved me.  Never mix different language versions of OS/2 on one
system although HPFS was designed to allow that.
OK, I could implement complex codepage support to this driver but I think it
would cause more problems than benefit with such buggy implementation in OS/2.
So this driver simply uses first codepage it finds for uppercasing and
lowercasing no matter what's file codepage index. Usually all file names are in
this codepage - if you don't try to do what I described above :-)
(HPFS 可以针对多个代码页包含多个大写转换表，每个文件都有一个指针指向其名字所在的代码页。然而 OS/2 诞生于美国，那里的人们不太在意代码页，因此多代码页支持存在相当多的缺陷。我的磁盘上有一个运行在代码页 852 的捷克语 OS/2。有一次我启动了运行在 cp 850 的英语 OS/2，并在我的 852 分区上创建了一个文件。它把文件名代码页标记为 850——很好。但当我再次启动捷克语 OS/2 时，那个文件用任何名字都无法访问了。看来 OS/2 用它的系统代码页(852)对搜索模式进行大写转换，而用它自己的代码页(850)对要比较的文件名进行大写转换。这两者永远不可能匹配。这真的是 IBM 开发者想要的吗？问题还在继续。当我在捷克语 OS/2 中在那个目录里再创建另一个文件时，那个文件也无法访问了。OS/2 在搜索文件应放置的位置时(注意，HPFS 目录中的文件必须有序)和在搜索文件时，可能使用了不同的大写转换方法。最后，当我在 PmShell 中打开这个目录时，PmShell 崩溃了(有趣的是，重启之后 PmShell 又试图重新打开这个目录 :-)。chkdsk 毫不在意地忽略这些错误，最后只有低级别的磁盘修改救了我。尽管 HPFS 的设计允许这样做，但千万不要在一台系统上混用不同语言版本的 OS/2。好吧，我可以为本驱动实现复杂的代码页支持，但考虑到 OS/2 中如此多缺陷的实现，我认为这样做弊大于利。所以本驱动只是使用它找到的第一个代码页来进行大写和小写转换，不管文件的代码页索引是什么。通常所有文件名都在这个代码页中——只要你不去做我上面描述的那些事 :-)


Known bugs
(已知缺陷)
==========

HPFS386 on OS/2 server is not supported. HPFS386 installed on normal OS/2 client
should work. If you have OS/2 server, use only read-only mode. I don't know how
to handle some HPFS386 structures like access control list or extended perm
list, I don't know how to delete them when file is deleted and how to not
overwrite them with extended attributes. Send me some info on these structures
and I'll make it. However, this driver should detect presence of HPFS386
structures, remount read-only and not destroy them (I hope).
(不支持 OS/2 server 上的 HPFS386。安装在普通 OS/2 client 上的 HPFS386 应当可以工作。如果你使用的是 OS/2 server，请只使用只读模式。我不知道如何处理访问控制列表或扩展权限列表这类 HPFS386 结构，不知道文件被删除时如何删除它们，也不知道如何避免用扩展属性覆盖它们。给我发一些关于这些结构的信息，我就会实现它。不过，本驱动应当能够检测到 HPFS386 结构的存在，并以只读方式重新挂载而不破坏它们(我希望如此)。)

When there's not enough space for extended attributes, they will be truncated
and no error is returned.
(当没有足够的空间存放扩展属性时，它们会被截断，并且不返回任何错误。)

OS/2 can't access files if the path is longer than about 256 chars but this
driver allows you to do it. chkdsk ignores such errors.
(如果路径长度超过大约 256 个字符，OS/2 就无法访问文件，但本驱动允许你这样做。chkdsk 会忽略这类错误。)

Sometimes you won't be able to delete some files on a very full filesystem
(returning error ENOSPC). That's because file in non-leaf node in directory tree
(one directory, if it's large, has dirents in tree on HPFS) must be replaced
with another node when deleted. And that new file might have larger name than
the old one so the new name doesn't fit in directory node (dnode). And that
would result in directory tree splitting, that takes disk space. Workaround is
to delete other files that are leaf (probability that the file is non-leaf is
about 1/50) or to truncate file first to make some space.
You encounter this problem only if you have many directories so that
preallocated directory band is full i.e.::
(有时，在一个非常满的文件系统上你将无法删除某些文件(返回错误 ENOSPC)。这是因为目录树非叶节点中的文件(HPFS 上，一个目录如果很大，它的 dirent 就存放在树中)在删除时必须用另一个节点替换。而那个新文件的名字可能比旧文件的大，导致新名字放不进目录节点(dnode)。这会导致目录树分裂，从而占用磁盘空间。解决办法是删除其他属于叶子的文件(文件是非叶子的概率约为 1/50)，或者先截断文件以腾出一些空间。只有当你有很多目录导致预分配的目录带已满时，才会遇到这个问题，即：)

	number_of_directories / size_of_filesystem_in_mb > 4.

You can't delete open directories.
(你无法删除已打开的目录。)

You can't rename over directories (what is it good for?).
(你不能重命名覆盖目录(这有什么用呢？)。)

Renaming files so that only case changes doesn't work. This driver supports it
but vfs doesn't. Something like 'mv file FILE' won't work.
(只改变大小写的文件重命名不起作用。本驱动支持它，但 vfs 不支持。类似 'mv file FILE' 的操作无法工作。)

All atimes and directory mtimes are not updated. That's because of performance
reasons. If you extremely wish to update them, let me know, I'll write it (but
it will be slow).
(所有 atime 和目录的 mtime 都不会更新。这是出于性能方面的原因。如果你非常希望更新它们，请告诉我，我会实现它(但会很慢)。)

When the system is out of memory and swap, it may slightly corrupt filesystem
(lost files, unbalanced directories). (I guess all filesystem may do it).
(当系统的内存和交换空间都耗尽时，它可能会轻微损坏文件系统(丢失文件、目录不平衡)。(我猜所有文件系统都可能如此。))

When compiled, you get warning: function declaration isn't a prototype. Does
anybody know what does it mean?
(编译时会得到警告：function declaration isn't a prototype。有谁知道这是什么意思吗？)


What does "unbalanced tree" message mean?
("unbalanced tree" 消息是什么意思？)
=========================================

Old versions of this driver created sometimes unbalanced dnode trees. OS/2
chkdsk doesn't scream if the tree is unbalanced (and sometimes creates
unbalanced trees too :-) but both HPFS and HPFS386 contain bug that it rarely
crashes when the tree is not balanced. This driver handles unbalanced trees
correctly and writes warning if it finds them. If you see this message, this is
probably because of directories created with old version of this driver.
Workaround is to move all files from that directory to another and then back
again. Do it in Linux, not OS/2! If you see this message in directory that is
whole created by this driver, it is BUG - let me know about it.
(本驱动的旧版本有时会创建不平衡的 dnode 树。如果树不平衡，OS/2 的 chkdsk 并不会大声报错(而且它有时也会创建不平衡的树 :-)，但 HPFS 和 HPFS386 都存在这样的缺陷：当树不平衡时偶尔会崩溃。本驱动能够正确处理不平衡的树，并在发现它们时写入警告。如果你看到这条消息，很可能是由于用旧版本的本驱动创建的目录。解决办法是把那个目录中的所有文件先移到另一个目录，然后再移回来。要在 Linux 下做，而不是 OS/2！如果你在一个完全由本驱动创建的目录中看到这条消息，那就是缺陷——请告诉我。)


Bugs in OS/2
(OS/2 中的缺陷)
============

When you have two (or more) lost directories pointing each to other, chkdsk
locks up when repairing filesystem.
(当你有两个(或更多)相互指向对方的丢失目录时，chkdsk 在修复文件系统时会卡死。)

Sometimes (I think it's random) when you create a file with one-char name under
OS/2, OS/2 marks it as 'long'. chkdsk then removes this flag saying "Minor fs
error corrected".
(有时(我认为是随机的)，当你在 OS/2 下创建一个名字只有一个字符的文件时，OS/2 会把它标记为 'long'。chkdsk 随后会移除这个标志，并显示"Minor fs error corrected"。)

File names like "a .b" are marked as 'long' by OS/2 but chkdsk "corrects" it and
marks them as short (and writes "minor fs error corrected"). This bug is not in
HPFS386.
(像 "a .b" 这样的文件名会被 OS/2 标记为 'long'，但 chkdsk 会"纠正"它并把它们标记为 short(并写入"minor fs error corrected")。这个缺陷不存在于 HPFS386 中。)

Codepage bugs described above
(上文描述的代码页缺陷)
=============================

If you don't install fixpacks, there are many, many more...
(如果你不安装 fixpacks，还有很多、很多更多的问题……)


History
(历史)
=======

====== =========================================================================
0.90   First public release (首次公开发布)
0.91   Fixed bug that caused shooting to memory when write_inode was called on
       open inode (rarely happened) (修复了一个缺陷：在对已打开的 inode 调用 write_inode 时会向内存乱写(很少发生))
0.92   Fixed a little memory leak in freeing directory inodes (修复了释放目录 inode 时的一处小内存泄漏)
0.93   Fixed bug that locked up the machine when there were too many filenames
       with first 15 characters same (修复了当前 15 个字符相同的文件名过多时机器被锁死的缺陷)
       Fixed write_file to zero file when writing behind file end (修复了 write_file 在写到文件末尾之后时把文件清零的问题)
0.94   Fixed a little memory leak when trying to delete busy file or directory (修复了尝试删除正在使用的文件或目录时的一处小内存泄漏)
0.95   Fixed a bug that i_hpfs_parent_dir was not updated when moving files (修复了移动文件时 i_hpfs_parent_dir 未被更新的缺陷)
1.90   First version for 2.1.1xx kernels (面向 2.1.1xx 内核的第一个版本)
1.91   Fixed a bug that chk_sectors failed when sectors were at the end of disk (修复了扇区位于磁盘末尾时 chk_sectors 失败的缺陷)
       Fixed a race-condition when write_inode is called while deleting file (修复了删除文件期间调用 write_inode 时的一个竞态条件)
       Fixed a bug that could possibly happen (with very low probability) when
       using 0xff in filenames. (修复了一个在文件名中使用 0xff 时可能发生(概率极低)的缺陷)

       Rewritten locking to avoid race-conditions (重写了锁定机制以避免竞态条件)

       Mount option 'eas' now works (挂载选项 'eas' 现在可以工作了)

       Fsync no longer returns error (Fsync 不再返回错误)

       Files beginning with '.' are marked hidden (以 '.' 开头的文件会被标记为隐藏)

       Remount support added (增加了重新挂载支持)

       Alloc is not so slow when filesystem becomes full (文件系统变满时 Alloc 不再那么慢)

       Atimes are no more updated because it slows down operation (不再更新 atime，因为它会拖慢操作)

       Code cleanup (removed all commented debug prints) (代码清理(移除了所有被注释掉的调试打印))
1.92   Corrected a bug when sync was called just before closing file (修正了在关闭文件之前恰好调用 sync 时的一个缺陷)
1.93   Modified, so that it works with kernels >= 2.1.131, I don't know if it
       works with previous versions (进行了修改，使其能在 >= 2.1.131 的内核上工作，我不知道它在更早的版本上是否可用)

       Fixed a possible problem with disks > 64G (but I don't have one, so I can't
       test it) (修复了一个在 > 64G 磁盘上可能出现的问题(但我没有这样的磁盘，所以无法测试))

       Fixed a file overflow at 2G (修复了 2G 处的文件溢出)

       Added new option 'timeshift' (增加了新选项 'timeshift')

       Changed behaviour on HPFS386: It is now possible to operate on HPFS386 in
       read-only mode (更改了在 HPFS386 上的行为：现在可以以只读模式操作 HPFS386)

       Fixed a bug that slowed down alloc and prevented allocating 100% space
       (this bug was not destructive) (修复了一个拖慢 alloc 并阻止分配 100% 空间的缺陷(该缺陷不具有破坏性))
1.94   Added workaround for one bug in Linux (为 Linux 中的一个缺陷增加了变通办法)

       Fixed one buffer leak (修复了一处缓冲泄漏)

       Fixed some incompatibilities with large extended attributes (but it's still
       not 100% ok, I have no info on it and OS/2 doesn't want to create them) (修复了一些与大型扩展属性的不兼容问题(但仍然不是 100% 正常，我没有这方面的资料，OS/2 也不愿创建它们))

       Rewritten allocation (重写了分配机制)

       Fixed a bug with i_blocks (du sometimes didn't display correct values) (修复了 i_blocks 的一个缺陷(du 有时不显示正确的值))

       Directories have no longer archive attribute set (some programs don't like
       it) (目录不再设置归档属性(有些程序不喜欢它))

       Fixed a bug that it set badly one flag in large anode tree (it was not
       destructive) (修复了在大型 anode 树中错误设置一个标志的缺陷(不具有破坏性))
1.95   Fixed one buffer leak, that could happen on corrupted filesystem (修复了一处可能发生在已损坏文件系统上的缓冲泄漏)

       Fixed one bug in allocation in 1.94 (修复了 1.94 中分配机制的一个缺陷)
1.96   Added workaround for one bug in OS/2 (HPFS locked up, HPFS386 reported
       error sometimes when opening directories in PMSHELL) (为 OS/2 中的一个缺陷增加了变通办法(在 PMSHELL 中打开目录时，HPFS 会锁死，HPFS386 有时报错))

       Fixed a possible bitmap race (修复了一个可能的位图竞态)

       Fixed possible problem on large disks (修复了大型磁盘上可能出现的问题)

       You can now delete open files (现在可以删除已打开的文件了)

       Fixed a nondestructive race in rename (修复了 rename 中一个非破坏性的竞态)
1.97   Support for HPFS v3 (on large partitions) (支持 HPFS v3(在大型分区上))

       ZFixed a bug that it didn't allow creation of files > 128M
       (it should be 2G) (修复了一个不允许创建 > 128M 文件的缺陷(本应允许 2G))
1.97.1 Changed names of global symbols (更改了全局符号的名称)

       Fixed a bug when chmoding or chowning root directory (修复了对根目录执行 chmod 或 chown 时的一个缺陷)
1.98   Fixed a deadlock when using old_readdir (修复了使用 old_readdir 时的一个死锁)
       Better directory handling; workaround for "unbalanced tree" bug in OS/2 (更好的目录处理；针对 OS/2 中"unbalanced tree"缺陷的变通办法)
1.99   Corrected a possible problem when there's not enough space while deleting
       file (修正了删除文件时空间不足可能出现的问题)

       Now it tries to truncate the file if there's not enough space when
       deleting (现在删除时如果空间不足，它会尝试截断文件)

       Removed a lot of redundant code (移除了大量冗余代码)
2.00   Fixed a bug in rename (it was there since 1.96) (修复了 rename 中的一个缺陷(自 1.96 起就存在))
       Better anti-fragmentation strategy (更好的防碎片策略)
2.01   Fixed problem with directory listing over NFS (修复了通过 NFS 列目录的问题)

       Directory lseek now checks for proper parameters (目录 lseek 现在会检查参数是否恰当)

       Fixed race-condition in buffer code - it is in all filesystems in Linux;
       when reading device (cat /dev/hda) while creating files on it, files
       could be damaged (修复了缓冲代码中的竞态条件——它存在于 Linux 的所有文件系统中；当在创建文件的同时读取设备(cat /dev/hda)时，文件可能被损坏)
2.02   Workaround for bug in breada in Linux. breada could cause accesses beyond
       end of partition (针对 Linux 中 breada 缺陷的变通办法。breada 可能导致越过分区末尾的访问)
2.03   Char, block devices and pipes are correctly created (字符设备、块设备和管道都能被正确创建)

       Fixed non-crashing race in unlink (Alexander Viro) (修复了 unlink 中一个不会导致崩溃的竞态(Alexander Viro))

       Now it works with Japanese version of OS/2 (现在可以在日语版 OS/2 上工作了)
2.04   Fixed error when ftruncate used to extend file (修复了用 ftruncate 扩展文件时的错误)
2.05   Fixed crash when got mount parameters without = (修复了得到不带 = 的挂载参数时的崩溃)

       Fixed crash when allocation of anode failed due to full disk (修复了因磁盘满而导致 anode 分配失败时的崩溃)

       Fixed some crashes when block io or inode allocation failed (修复了块 I/O 或 inode 分配失败时的一些崩溃)
2.06   Fixed some crash on corrupted disk structures (修复了磁盘结构损坏时的一些崩溃)

       Better allocation strategy (更好的分配策略)

       Reschedule points added so that it doesn't lock CPU long time (增加了重新调度点，使其不会长时间占用 CPU)

       It should work in read-only mode on Warp Server (它应当能在 Warp Server 上以只读模式工作)
2.07   More fixes for Warp Server. Now it really works (对 Warp Server 的更多修复。现在它真的可以工作了)
2.08   Creating new files is not so slow on large disks (在大型磁盘上创建新文件不再那么慢)

       An attempt to sync deleted file does not generate filesystem error (尝试 sync 已删除的文件不再产生文件系统错误)
2.09   Fixed error on extremely fragmented files (修复了极度碎片化的文件上的错误)
====== =========================================================================
