====
VFAT
====

(VFAT)

USING VFAT
(使用 VFAT)
==========

To use the vfat filesystem, use the filesystem type 'vfat'.  i.e.::
(要使用 vfat 文件系统，请使用文件系统类型 'vfat'。即：)

  mount -t vfat /dev/fd0 /mnt


No special partition formatter is required,
'mkdosfs' will work fine if you want to format from within Linux.
(不需要特殊的分区格式化工具，如果想从 Linux 内部进行格式化，'mkdosfs' 就可以很好地完成。)

VFAT MOUNT OPTIONS
(VFAT 挂载选项)
==================

**uid=###**
	Set the owner of all files on this filesystem.
	The default is the uid of current process.
	(设置此文件系统上所有文件的所有者。默认为当前进程的 uid。)

**gid=###**
	Set the group of all files on this filesystem.
	The default is the gid of current process.
	(设置此文件系统上所有文件的属组。默认为当前进程的 gid。)

**umask=###**
	The permission mask (for files and directories, see *umask(1)*).
	The default is the umask of current process.
	(权限掩码(针对文件和目录，参见 *umask(1)*)。默认为当前进程的 umask。)

**dmask=###**
	The permission mask for the directory.
	The default is the umask of current process.
	(针对目录的权限掩码。默认为当前进程的 umask。)

**fmask=###**
	The permission mask for files.
	The default is the umask of current process.
	(针对文件的权限掩码。默认为当前进程的 umask。)

**allow_utime=###**
	This option controls the permission check of mtime/atime.
	(此选项控制对 mtime/atime 的权限检查。)

		**-20**: If current process is in group of file's group ID,
                you can change timestamp.
                (如果当前进程属于文件属组所在的组，就可以更改时间戳。)

		**-2**: Other users can change timestamp.
                (其他用户可以更改时间戳。)

	The default is set from dmask option. If the directory is
	writable, utime(2) is also allowed. i.e. ~dmask & 022.
	(默认值由 dmask 选项设置得出。如果目录可写，那么也允许 utime(2)。即 ~dmask & 022。)

	Normally utime(2) checks current process is owner of
	the file, or it has CAP_FOWNER capability. But FAT
	filesystem doesn't have uid/gid on disk, so normal
	check is too inflexible. With this option you can
	relax it.
	(通常 utime(2) 会检查当前进程是否为文件的属主，或者是否具有 CAP_FOWNER 能力。但 FAT 文件系统在磁盘上没有 uid/gid，所以这种常规检查太过僵硬。有了这个选项，你可以放宽它。)

**codepage=###**
	Sets the codepage number for converting to shortname
	characters on FAT filesystem.
	By default, FAT_DEFAULT_CODEPAGE setting is used.
	(设置在 FAT 文件系统上转换短名字符所用的代码页编号。默认使用 FAT_DEFAULT_CODEPAGE 的设置。)

**iocharset=<name>**
	Character set to use for converting between the
	encoding is used for user visible filename and 16 bit
	Unicode characters. Long filenames are stored on disk
	in Unicode format, but Unix for the most part doesn't
	know how to deal with Unicode.
	By default, FAT_DEFAULT_IOCHARSET setting is used.
	(用于在用户可见文件名所使用的编码与 16 位 Unicode 字符之间进行转换的字符集。长文件名以 Unicode 格式存储在磁盘上，但 Unix 在大多数情况下并不知道如何处理 Unicode。默认使用 FAT_DEFAULT_IOCHARSET 的设置。)

	There is also an option of doing UTF-8 translations
	with the utf8 option.
	(还有一种选择，即通过 utf8 选项来进行 UTF-8 转换。)

.. note:: ``iocharset=utf8`` is not recommended. If unsure, you should consider
	  the utf8 option instead.
	  (注意：不推荐使用 ``iocharset=utf8``。如果不确定，应当考虑改用 utf8 选项。)

**utf8=<bool>**
	UTF-8 is the filesystem safe version of Unicode that
	is used by the console. It can be enabled or disabled
	for the filesystem with this option.
	If 'uni_xlate' gets set, UTF-8 gets disabled.
	By default, FAT_DEFAULT_UTF8 setting is used.
	(UTF-8 是控制台所使用的 Unicode 的文件系统安全版本。可以用此选项为文件系统启用或禁用它。如果设置了 'uni_xlate'，UTF-8 就会被禁用。默认使用 FAT_DEFAULT_UTF8 的设置。)

**uni_xlate=<bool>**
	Translate unhandled Unicode characters to special
	escaped sequences.  This would let you backup and
	restore filenames that are created with any Unicode
	characters.  Until Linux supports Unicode for real,
	this gives you an alternative.  Without this option,
	a '?' is used when no translation is possible.  The
	escape character is ':' because it is otherwise
	illegal on the vfat filesystem.  The escape sequence
	that gets used is ':' and the four digits of hexadecimal
	unicode.
	(把未处理的 Unicode 字符转换成特殊的转义序列。这让你可以备份并恢复用任何 Unicode 字符创建的文件名。在 Linux 真正支持 Unicode 之前，这给你提供了一种替代方案。没有此选项时，在无法转换的情况下会使用 '?'。转义字符是 ':'，因为它在 vfat 文件系统上原本是非法的。所使用的转义序列是 ':' 加上十六进制 Unicode 的四位数字。)

**nonumtail=<bool>**
	When creating 8.3 aliases, normally the alias will
	end in '~1' or tilde followed by some number.  If this
	option is set, then if the filename is
	"longfilename.txt" and "longfile.txt" does not
	currently exist in the directory, longfile.txt will
	be the short alias instead of longfi~1.txt.
	(创建 8.3 别名时，别名通常以 '~1' 或波浪号加某个数字结尾。如果设置了此选项，那么当文件名是 "longfilename.txt" 且目录中当前不存在 "longfile.txt" 时，将使用 longfile.txt 作为短别名，而不是 longfi~1.txt。)

**usefree**
	Use the "free clusters" value stored on FSINFO. It will
	be used to determine number of free clusters without
	scanning disk. But it's not used by default, because
	recent Windows don't update it correctly in some
	case. If you are sure the "free clusters" on FSINFO is
	correct, by this option you can avoid scanning disk.
	(使用存储在 FSINFO 上的"空闲簇"值。它将用于确定空闲簇的数量而无需扫描磁盘。但默认并不使用它，因为近期的 Windows 在某些情况下不会正确更新该值。如果你确信 FSINFO 上的"空闲簇"是正确的，通过此选项可以避免扫描磁盘。)

**quiet**
	Stops printing certain warning messages.
	(停止打印某些警告消息。)

**check=s|r|n**
	Case sensitivity checking setting.
	(大小写敏感性检查设置。)

	**s**: strict, case sensitive
	        (严格，区分大小写)

	**r**: relaxed, case insensitive
	        (宽松，不区分大小写)

	**n**: normal, default setting, currently case insensitive
	        (普通，默认设置，目前不区分大小写)

**nocase**
	This was deprecated for vfat. Use ``shortname=win95`` instead.
	(这对 vfat 已被弃用。请改用 ``shortname=win95``。)

**shortname=lower|win95|winnt|mixed**
	Shortname display/create setting.
	(短名显示/创建设置。)

	**lower**: convert to lowercase for display,
	emulate the Windows 95 rule for create.
	(显示时转换为小写，创建时模拟 Windows 95 规则。)

	**win95**: emulate the Windows 95 rule for display/create.
	(显示/创建都模拟 Windows 95 规则。)

	**winnt**: emulate the Windows NT rule for display/create.
	(显示/创建都模拟 Windows NT 规则。)

	**mixed**: emulate the Windows NT rule for display,
	emulate the Windows 95 rule for create.
	(显示时模拟 Windows NT 规则，创建时模拟 Windows 95 规则。)

	Default setting is `mixed`.
	(默认设置为 `mixed`。)

**tz=UTC**
	Interpret timestamps as UTC rather than local time.
	This option disables the conversion of timestamps
	between local time (as used by Windows on FAT) and UTC
	(which Linux uses internally).  This is particularly
	useful when mounting devices (like digital cameras)
	that are set to UTC in order to avoid the pitfalls of
	local time.
	(将时间戳解释为 UTC 而不是本地时间。此选项禁用本地时间(Windows 在 FAT 上所使用的)与 UTC(Linux 内部使用的)之间的时间戳转换。在挂载被设置为 UTC 的设备(如数码相机)时，这一选项特别有用，可以避免本地时间带来的种种陷阱。)

**time_offset=minutes**
	Set offset for conversion of timestamps from local time
	used by FAT to UTC. I.e. <minutes> minutes will be subtracted
	from each timestamp to convert it to UTC used internally by
	Linux. This is useful when time zone set in ``sys_tz`` is
	not the time zone used by the filesystem. Note that this
	option still does not provide correct time stamps in all
	cases in presence of DST - time stamps in a different DST
	setting will be off by one hour.
	(设置将 FAT 所使用的本地时间戳转换为 UTC 时的偏移量。也就是说，将从每个时间戳中减去 <minutes> 分钟，把它转换为 Linux 内部使用的 UTC。当 ``sys_tz`` 中设置的时区不是文件系统所使用的时区时，这很有用。请注意，在存在夏令时(DST)的情况下，此选项仍然不能在所有情况下都提供正确的时间戳——处于不同 DST 设置下的时间戳会相差一小时。)

**showexec**
	If set, the execute permission bits of the file will be
	allowed only if the extension part of the name is .EXE,
	.COM, or .BAT. Not set by default.
	(如果设置，只有当名字的扩展名部分是 .EXE、.COM 或 .BAT 时，才允许文件拥有可执行权限位。默认不设置。)

**debug**
	Can be set, but unused by the current implementation.
	(可以设置，但当前的实现并未使用它。)

**sys_immutable**
	If set, ATTR_SYS attribute on FAT is handled as
	IMMUTABLE flag on Linux. Not set by default.
	(如果设置，FAT 上的 ATTR_SYS 属性将作为 Linux 上的 IMMUTABLE 标志来处理。默认不设置。)

**flush**
	If set, the filesystem will try to flush to disk more
	early than normal. Not set by default.
	(如果设置，文件系统将尝试比正常情况更早地刷写到磁盘。默认不设置。)

**rodir**
	FAT has the ATTR_RO (read-only) attribute. On Windows,
	the ATTR_RO of the directory will just be ignored,
	and is used only by applications as a flag (e.g. it's set
	for the customized folder).
	(FAT 具有 ATTR_RO(只读)属性。在 Windows 上，目录的 ATTR_RO 只是被忽略，仅由应用程序用作一个标志(例如自定义文件夹会设置它)。)

	If you want to use ATTR_RO as read-only flag even for
	the directory, set this option.
	(如果想对目录也使用 ATTR_RO 作为只读标志，请设置此选项。)

**errors=panic|continue|remount-ro**
	specify FAT behavior on critical errors: panic, continue
	without doing anything or remount the partition in
	read-only mode (default behavior).
	(指定 FAT 在严重错误时的行为：panic、什么都不做继续运行，或以只读模式重新挂载分区(默认行为)。)

**discard**
	If set, issues discard/TRIM commands to the block
	device when blocks are freed. This is useful for SSD devices
	and sparse/thinly-provisioned LUNs.
	(如果设置，当块被释放时向块设备发出 discard/TRIM 命令。这对 SSD 设备和稀疏/精简配置的 LUN 很有用。)

**nfs=stale_rw|nostale_ro**
	Enable this only if you want to export the FAT filesystem
	over NFS.
	(只有在你希望通过 NFS 导出 FAT 文件系统时才启用此选项。)

		**stale_rw**: This option maintains an index (cache) of directory
		*inodes* by *i_logstart* which is used by the nfs-related code to
		improve look-ups. Full file operations (read/write) over NFS is
		supported but with cache eviction at NFS server, this could
		result in ESTALE issues.
		(此选项通过 *i_logstart* 维护目录 *inode* 的索引(缓存)，与 NFS 相关的代码用它来改进查找。支持通过 NFS 进行完整的文件操作(读/写)，但当 NFS 服务器发生缓存逐出时，可能导致 ESTALE 问题。)

		**nostale_ro**: This option bases the *inode* number and filehandle
		on the on-disk location of a file in the MS-DOS directory entry.
		This ensures that ESTALE will not be returned after a file is
		evicted from the inode cache. However, it means that operations
		such as rename, create and unlink could cause filehandles that
		previously pointed at one file to point at a different file,
		potentially causing data corruption. For this reason, this
		option also mounts the filesystem readonly.
		(此选项把 *inode* 编号和文件句柄建立在文件在 MS-DOS 目录项中的磁盘位置之上。这确保文件从 inode 缓存中被逐出后不会返回 ESTALE。然而，这意味着 rename、create 和 unlink 等操作可能使先前指向某个文件的文件句柄指向另一个文件，从而可能导致数据损坏。因此，此选项还会以只读方式挂载文件系统。)

	To maintain backward compatibility, ``'-o nfs'`` is also accepted,
	defaulting to "stale_rw".
	(为了保持向后兼容，也接受 ``'-o nfs'``，默认为 "stale_rw"。)

**dos1xfloppy  <bool>: 0,1,yes,no,true,false**
	If set, use a fallback default BIOS Parameter Block
	configuration, determined by backing device size. These static
	parameters match defaults assumed by DOS 1.x for 160 kiB,
	180 kiB, 320 kiB, and 360 kiB floppies and floppy images.
	(如果设置，使用一个后备的默认 BIOS 参数块配置，由后端设备大小决定。这些静态参数与 DOS 1.x 为 160 kiB、180 kiB、320 kiB 和 360 kiB 软盘及软盘镜像所假定的默认值相匹配。)



LIMITATION
(限制)
==========

The fallocated region of file is discarded at umount/evict time
when using fallocate with FALLOC_FL_KEEP_SIZE.
So, User should assume that fallocated region can be discarded at
last close if there is memory pressure resulting in eviction of
the inode from the memory. As a result, for any dependency on
the fallocated region, user should make sure to recheck fallocate
after reopening the file.
(当在 fallocate 中使用 FALLOC_FL_KEEP_SIZE 时，文件中已预分配的区域会在 umount/evict 时刻被丢弃。因此，如果出现内存压力导致 inode 从内存中被逐出，用户应当假设在最后一次关闭时预分配区域可能被丢弃。结果，对于任何对预分配区域的依赖，用户都应当确保在重新打开文件后重新检查 fallocate。)

TODO
(待办事项)
====
Need to get rid of the raw scanning stuff.  Instead, always use
a get next directory entry approach.  The only thing left that uses
raw scanning is the directory renaming code.
(需要摆脱原始扫描的那一套。相反，应当始终采用"获取下一个目录项"的方式。目前仍在使用原始扫描的只有目录重命名代码。)


POSSIBLE PROBLEMS
(可能的问题)
=================

- vfat_valid_longname does not properly checked reserved names.
  (vfat_valid_longname 没有对保留名字进行恰当的检查。)
- When a volume name is the same as a directory name in the root
  directory of the filesystem, the directory name sometimes shows
  up as an empty file.
  (当卷名与文件系统根目录中的某个目录名相同时，该目录名有时会显示为一个空文件。)
- autoconv option does not work correctly.
  (autoconv 选项不能正确工作。)


TEST SUITE
(测试套件)
==========
If you plan to make any modifications to the vfat filesystem, please
get the test suite that comes with the vfat distribution at
(如果你打算对 vfat 文件系统做任何修改，请从以下位置获取 vfat 发行版自带的测试套件：)

`<http://web.archive.org/web/*/http://bmrc.berkeley.edu/people/chaffee/vfat.html>`_

This tests quite a few parts of the vfat filesystem and additional
tests for new features or untested features would be appreciated.
(它测试了 vfat 文件系统的相当多部分，欢迎为新特性或未测试的特性补充更多测试。)

NOTES ON THE STRUCTURE OF THE VFAT FILESYSTEM
(关于 VFAT 文件系统结构的说明)
=============================================
This documentation was provided by Galen C. Hunt gchunt@cs.rochester.edu and
lightly annotated by Gordon Chaffee.
(本文档由 Galen C. Hunt gchunt@cs.rochester.edu 提供，并由 Gordon Chaffee 作了少量注释。)

This document presents a very rough, technical overview of my
knowledge of the extended FAT file system used in Windows NT 3.5 and
Windows 95.  I don't guarantee that any of the following is correct,
but it appears to be so.
(本文档对我所了解的 Windows NT 3.5 和 Windows 95 中使用的扩展 FAT 文件系统做了一个非常粗略的技术概述。我不保证下面的内容都正确，但看起来确实如此。)

The extended FAT file system is almost identical to the FAT
file system used in DOS versions up to and including *6.223410239847*
:-).  The significant change has been the addition of long file names.
These names support up to 255 characters including spaces and lower
case characters as opposed to the traditional 8.3 short names.
(扩展 FAT 文件系统与 DOS 6.223410239847 及更早版本所使用的 FAT 文件系统几乎完全相同 :-)。重大的变化是加入了长文件名。与传统的 8.3 短名不同，这些名字支持最多 255 个字符，包括空格和小写字符。)

Here is the description of the traditional FAT entry in the current
Windows 95 filesystem::
(下面是当前 Windows 95 文件系统中传统 FAT 表项的描述：)

        struct directory { // Short 8.3 names
                unsigned char name[8];          // file name
                unsigned char ext[3];           // file extension
                unsigned char attr;             // attribute byte
		unsigned char lcase;		// Case for base and extension
		unsigned char ctime_ms;		// Creation time, milliseconds
		unsigned char ctime[2];		// Creation time
		unsigned char cdate[2];		// Creation date
		unsigned char adate[2];		// Last access date
		unsigned char reserved[2];	// reserved values (ignored)
                unsigned char time[2];          // time stamp
                unsigned char date[2];          // date stamp
                unsigned char start[2];         // starting cluster number
                unsigned char size[4];          // size of the file
        };


The lcase field specifies if the base and/or the extension of an 8.3
name should be capitalized.  This field does not seem to be used by
Windows 95 but it is used by Windows NT.  The case of filenames is not
completely compatible from Windows NT to Windows 95.  It is not completely
compatible in the reverse direction, however.  Filenames that fit in
the 8.3 namespace and are written on Windows NT to be lowercase will
show up as uppercase on Windows 95.
(lcase 字段指定 8.3 名字的主名和/或扩展名是否应采用大写。这个字段似乎没有被 Windows 95 使用，但 Windows NT 会使用它。文件名的大小写在从 Windows NT 到 Windows 95 的方向上并不完全兼容。然而，反方向也不完全兼容。在 8.3 命名空间内、在 Windows NT 上被写成小写的文件名，在 Windows 95 上会显示为大写。)

.. note:: Note that the ``start`` and ``size`` values are actually little
          endian integer values.  The descriptions of the fields in this
          structure are public knowledge and can be found elsewhere.
          (注意，``start`` 和 ``size`` 值实际上是小端整数值。此结构中各字段的描述是公开的知识，可以在别处找到。)

With the extended FAT system, Microsoft has inserted extra
directory entries for any files with extended names.  (Any name which
legally fits within the old 8.3 encoding scheme does not have extra
entries.)  I call these extra entries slots.  Basically, a slot is a
specially formatted directory entry which holds up to 13 characters of
a file's extended name.  Think of slots as additional labeling for the
directory entry of the file to which they correspond.  Microsoft
prefers to refer to the 8.3 entry for a file as its alias and the
extended slot directory entries as the file name.
(在扩展 FAT 系统中，Microsoft 为任何具有扩展名的文件插入了额外的目录项。(任何能够合法容纳在旧 8.3 编码方案中的名字都没有额外表项。)我把这些额外的表项称为槽(slot)。基本上，槽是一种特殊格式的目录项，最多可容纳文件扩展名中的 13 个字符。可以把槽看作是对其对应文件的目录项的附加标注。Microsoft 更倾向于把文件的 8.3 表项称为它的别名(alias)，而把扩展槽目录项称为文件名。)

The C structure for a slot directory entry follows::
(槽目录项的 C 结构体如下：)

        struct slot { // Up to 13 characters of a long name
                unsigned char id;               // sequence number for slot
                unsigned char name0_4[10];      // first 5 characters in name
                unsigned char attr;             // attribute byte
                unsigned char reserved;         // always 0
                unsigned char alias_checksum;   // checksum for 8.3 alias
                unsigned char name5_10[12];     // 6 more characters in name
                unsigned char start[2];         // starting cluster number
                unsigned char name11_12[4];     // last 2 characters in name
        };


If the layout of the slots looks a little odd, it's only
because of Microsoft's efforts to maintain compatibility with old
software.  The slots must be disguised to prevent old software from
panicking.  To this end, a number of measures are taken:
(如果槽的布局看起来有点奇怪，那只是因为 Microsoft 努力保持与旧软件的兼容性。必须对槽进行伪装，以防止旧软件出现混乱。为此采取了一些措施：)

        1) The attribute byte for a slot directory entry is always set
           to 0x0f.  This corresponds to an old directory entry with
           attributes of "hidden", "system", "read-only", and "volume
           label".  Most old software will ignore any directory
           entries with the "volume label" bit set.  Real volume label
           entries don't have the other three bits set.
           (槽目录项的属性字节总是被设置为 0x0f。这对应于一个具有"hidden"、"system"、"read-only"和"volume label"属性的旧目录项。大多数旧软件会忽略任何设置了"volume label"位的目录项。真正的卷标表项不会设置另外三位。)

        2) The starting cluster is always set to 0, an impossible
           value for a DOS file.
           (起始簇总是被设置为 0，这对 DOS 文件来说是一个不可能的值。)

Because the extended FAT system is backward compatible, it is
possible for old software to modify directory entries.  Measures must
be taken to ensure the validity of slots.  An extended FAT system can
verify that a slot does in fact belong to an 8.3 directory entry by
the following:
(由于扩展 FAT 系统是向后兼容的，旧软件有可能修改目录项。必须采取措施确保槽的有效性。扩展 FAT 系统可以通过以下方式验证某个槽确实属于一个 8.3 目录项：)

        1) Positioning.  Slots for a file always immediately proceed
           their corresponding 8.3 directory entry.  In addition, each
           slot has an id which marks its order in the extended file
           name.  Here is a very abbreviated view of an 8.3 directory
           entry and its corresponding long name slots for the file
           "My Big File.Extension which is long"::
           (定位。一个文件的各个槽总是紧挨着位于其对应的 8.3 目录项之前。此外，每个槽都有一个 id，标记它在扩展文件名中的次序。下面是文件 "My Big File.Extension which is long" 的 8.3 目录项及其对应长名槽的非常简略的视图：)

                <proceeding files...>
                <slot #3, id = 0x43, characters = "h is long">
                <slot #2, id = 0x02, characters = "xtension whic">
                <slot #1, id = 0x01, characters = "My Big File.E">
                <directory entry, name = "MYBIGFIL.EXT">


           .. note:: Note that the slots are stored from last to first.  Slots
		     are numbered from 1 to N.  The Nth slot is ``or'ed`` with
		     0x40 to mark it as the last one.
		     (注意，槽是按从后往前的顺序存储的。槽从 1 到 N 编号。第 N 个槽会与 0x40 相"或"(``or'ed``)，以标记它是最后一个。)

        2) Checksum.  Each slot has an alias_checksum value.  The
           checksum is calculated from the 8.3 name using the
           following algorithm::
           (校验和。每个槽都有一个 alias_checksum 值。校验和是用 8.3 名字按以下算法计算出来的：)

                for (sum = i = 0; i < 11; i++) {
                        sum = (((sum&1)<<7)|((sum&0xfe)>>1)) + name[i]
                }


	3) If there is free space in the final slot, a Unicode ``NULL (0x0000)``
	   is stored after the final character.  After that, all unused
	   characters in the final slot are set to Unicode 0xFFFF.
	   (如果最后一个槽中有空闲空间，则会在最后一个字符之后存储一个 Unicode ``NULL (0x0000)``。此后，最后一个槽中所有未使用的字符都被设置为 Unicode 0xFFFF。)

Finally, note that the extended name is stored in Unicode.  Each Unicode
character takes either two or four bytes, UTF-16LE encoded.
(最后，注意扩展名是以 Unicode 存储的。每个 Unicode 字符占两或四个字节，采用 UTF-16LE 编码。)
