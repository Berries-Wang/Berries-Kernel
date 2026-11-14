.. SPDX-License-Identifier: GPL-2.0

=====
NTFS3
=====

(NTFS3)

Summary and Features
(概述与特性)
====================

NTFS3 is fully functional NTFS Read-Write driver. The driver works with NTFS
versions up to 3.1. File system type to use on mount is *ntfs3*.
(NTFS3 是一个功能完备的 NTFS 读写驱动程序。该驱动程序支持最高至 3.1 版本的 NTFS。挂载时使用的文件系统类型为 *ntfs3*。)

- This driver implements NTFS read/write support for normal, sparse and
  compressed files.
  (该驱动程序对普通、稀疏和压缩文件实现了 NTFS 读写支持。)
- Supports native journal replaying.
  (支持原生的日志重放。)
- Supports NFS export of mounted NTFS volumes.
  (支持对已挂载 NTFS 卷的 NFS 导出。)
- Supports extended attributes. Predefined extended attributes:
  (支持扩展属性。预定义的扩展属性：)

	- *system.ntfs_security* gets/sets security
	  (获取/设置安全信息)

		Descriptor: SECURITY_DESCRIPTOR_RELATIVE
		(描述符: SECURITY_DESCRIPTOR_RELATIVE)

	- *system.ntfs_attrib* gets/sets ntfs file/dir attributes.
	  (获取/设置 ntfs 文件/目录属性。)

	  Note: Applied to empty files, this allows to switch type between
	  sparse(0x200), compressed(0x800) and normal.
	  (注意: 应用于空文件时，这允许在稀疏(0x200)、压缩(0x800)和普通之间切换类型。)

	- *system.ntfs_attrib_be* gets/sets ntfs file/dir attributes.
	  (获取/设置 ntfs 文件/目录属性。)

	  Same value as system.ntfs_attrib but always represent as big-endian
	  (endianness of system.ntfs_attrib is the same as of the CPU).
	  (与 system.ntfs_attrib 的值相同，但始终以大端序表示(system.ntfs_attrib 的字节序与 CPU 的字节序相同)。)

Mount Options
(挂载选项)
=============

The list below describes mount options supported by NTFS3 driver in addition to
generic ones. You can use every mount option with **no** option. If it is in
this table marked with no it means default is without **no**.
(下面的列表描述了 NTFS3 驱动程序在通用选项之外支持的挂载选项。你可以对每个挂载选项使用 **no** 形式。如果表中某项标有 no，则意味着默认值是不带 **no** 的。)

.. flat-table::
   :widths: 1 5
   :fill-cells:

   * - iocharset=name
     - This option informs the driver how to interpret path strings and
       translate them to Unicode and back. If this option is not set, the
       default codepage will be used (CONFIG_NLS_DEFAULT).
       (此选项告知驱动程序如何解释路径字符串，并将其与 Unicode 相互转换。如果未设置此选项，将使用默认代码页(CONFIG_NLS_DEFAULT)。)

       Example: iocharset=utf8
       (示例: iocharset=utf8)

   * - uid=
     - :rspan:`1`
   * - gid=

   * - umask=
     - Controls the default permissions for files/directories created after
       the NTFS volume is mounted.
       (控制在挂载 NTFS 卷之后创建的文件/目录的默认权限。)

   * - dmask=
     - :rspan:`1` Instead of specifying umask which applies both to files and
       directories, fmask applies only to files and dmask only to directories.
       (与其指定同时应用于文件和目录的 umask，fmask 仅应用于文件，而 dmask 仅应用于目录。)
   * - fmask=

   * - nohidden
     - Files with the Windows-specific HIDDEN (FILE_ATTRIBUTE_HIDDEN) attribute
       will not be shown under Linux.
       (带有 Windows 特有的 HIDDEN (FILE_ATTRIBUTE_HIDDEN)属性的文件将不会在 Linux 下显示。)

   * - sys_immutable
     - Files with the Windows-specific SYSTEM (FILE_ATTRIBUTE_SYSTEM) attribute
       will be marked as system immutable files.
       (带有 Windows 特有的 SYSTEM (FILE_ATTRIBUTE_SYSTEM)属性的文件将被标记为系统不可变文件。)

   * - hide_dot_files
     - Updates the Windows-specific HIDDEN (FILE_ATTRIBUTE_HIDDEN) attribute
       when creating and moving or renaming files. Files whose names start
       with a dot will have the HIDDEN attribute set and files whose names
       do not start with a dot will have it unset.
       (在创建以及移动或重命名文件时更新 Windows 特有的 HIDDEN (FILE_ATTRIBUTE_HIDDEN)属性。名称以点开头的文件将设置 HIDDEN 属性，名称不以点开头的文件将取消该属性。)

   * - windows_names
     - Prevents the creation of files and directories with a name not allowed
       by Windows, either because it contains some not allowed character (which
       are the characters " * / : < > ? \\ | and those whose code is less than
       0x20), because the name (with or without extension) is a reserved file
       name (CON, AUX, NUL, PRN, LPT1-9, COM1-9) or because the last character
       is a space or a dot. Existing such files can still be read and renamed.
       (阻止创建名称不被 Windows 允许的文件和目录——或者因为名称包含不允许的字符(即字符 " * / : < > ? \\ | 以及编码小于 0x20 的字符)，或者因为名称(带或不带扩展名)是保留文件名(CON、AUX、NUL、PRN、LPT1-9、COM1-9)，或者因为最后一个字符是空格或点。已存在的此类文件仍可读取和重命名。)

   * - discard
     - Enable support of the TRIM command for improved performance on delete
       operations, which is recommended for use with the solid-state drives
       (SSD).
       (启用对 TRIM 命令的支持，以提升删除操作的性能，建议在固态硬盘(SSD)上使用。)

   * - force
     - Forces the driver to mount partitions even if volume is marked dirty.
       Not recommended for use.
       (强制驱动程序挂载分区，即使卷被标记为脏。不建议使用。)

   * - sparse
     - Create new files as sparse.
       (将新文件创建为稀疏文件。)

   * - showmeta
     - Use this parameter to show all meta-files (System Files) on a mounted
       NTFS partition. By default, all meta-files are hidden.
       (使用此参数在已挂载的 NTFS 分区上显示所有元文件(系统文件)。默认情况下，所有元文件都是隐藏的。)

   * - prealloc
     - Preallocate space for files excessively when file size is increasing on
       writes. Decreases fragmentation in case of parallel write operations to
       different files.
       (当写入使文件大小增加时，过量地为文件预分配空间。在并行写入不同文件的情况下可减少碎片。)

   * - acl
     - Support POSIX ACLs (Access Control Lists). Effective if supported by
       Kernel. Not to be confused with NTFS ACLs. The option specified as acl
       enables support for POSIX ACLs.
       (支持 POSIX ACL(访问控制列表)。在内核支持时生效。不要与 NTFS ACL 混淆。指定 acl 选项即启用对 POSIX ACL 的支持。)

Todo list
(待办事项)
=========
- Full journaling support over JBD. Currently journal replaying is supported
  which is not necessarily as effective as JBD would be.
  (基于 JBD 的完整日志支持。目前支持日志重放，但其效果不一定能达到 JBD 的水平。)

References
(参考)
==========
- Commercial version of the NTFS driver for Linux.
	https://www.paragon-software.com/home/ntfs-linux-professional/
  (面向 Linux 的 NTFS 驱动程序商业版。)

- Direct e-mail address for feedback and requests on the NTFS3 implementation.
	almaz.alexandrovich@paragon-software.com
  (关于 NTFS3 实现的反馈和请求的直达电子邮件地址。)
