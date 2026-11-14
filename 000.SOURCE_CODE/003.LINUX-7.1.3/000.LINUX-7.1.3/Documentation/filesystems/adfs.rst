.. SPDX-License-Identifier: GPL-2.0

===============================
Acorn Disc Filing System - ADFS
===============================

(Acorn 磁盘文件系统 - ADFS)

Filesystems supported by ADFS
(ADFS 支持的文件系统)
-----------------------------

The ADFS module supports the following Filecore formats which have:
(ADFS 模块支持下列具有如下特征的 Filecore 格式：)

- new maps
  (新映射)
- new directories or big directories
  (新目录或大目录)

In terms of the named formats, this means we support:
(就上述命名的格式而言，这意味着我们支持：)

- E and E+, with or without boot block
  (E 和 E+，有无引导块均可)
- F and F+
  (F 和 F+)

We fully support reading files from these filesystems, and writing to
existing files within their existing allocation.  Essentially, we do
not support changing any of the filesystem metadata.
(我们完全支持从这些文件系统读取文件，以及在现有分配范围内写入已有文件。本质上，我们不支持更改任何文件系统元数据。)

This is intended to support loopback mounted Linux native filesystems
on a RISC OS Filecore filesystem, but will allow the data within files
to be changed.
(这是为了支持在 RISC OS Filecore 文件系统上回环挂载 Linux 原生文件系统，但将允许更改文件内的数据。)

If write support (ADFS_FS_RW) is configured, we allow rudimentary
directory updates, specifically updating the access mode and timestamp.
(如果配置了写支持(ADFS_FS_RW)，我们允许基本的目录更新，具体而言是更新访问模式和时间戳。)

Mount options for ADFS
(ADFS 的挂载选项)
----------------------

  ============  ======================================================
  uid=nnn	All files in the partition will be owned by
		user id nnn.  Default 0 (root).(分区中的所有文件将归用户 id nnn 所有。默认 0(root)。)
  gid=nnn	All files in the partition will be in group
		nnn.  Default 0 (root).(分区中的所有文件将属于组 nnn。默认 0(root)。)
  ownmask=nnn	The permission mask for ADFS 'owner' permissions
		will be nnn.  Default 0700.(ADFS 'owner' 权限的权限掩码将为 nnn。默认 0700。)
  othmask=nnn	The permission mask for ADFS 'other' permissions
		will be nnn.  Default 0077.(ADFS 'other' 权限的权限掩码将为 nnn。默认 0077。)
  ftsuffix=n	When ftsuffix=0, no file type suffix will be applied.
		When ftsuffix=1, a hexadecimal suffix corresponding to
		the RISC OS file type will be added.  Default 0.(当 ftsuffix=0 时，不会应用任何文件类型后缀。当 ftsuffix=1 时，将添加与 RISC OS 文件类型对应的十六进制后缀。默认 0。)
  ============  ======================================================

Mapping of ADFS permissions to Linux permissions
(ADFS 权限到 Linux 权限的映射)
------------------------------------------------

  ADFS permissions consist of the following:
  (ADFS 权限由以下各项组成：)

	- Owner read
	  (所有者读)
	- Owner write
	  (所有者写)
	- Other read
	  (其他用户读)
	- Other write
	  (其他用户写)

  (In older versions, an 'execute' permission did exist, but this
  does not hold the same meaning as the Linux 'execute' permission
  and is now obsolete).
  (在较旧的版本中，确实存在过 'execute' 权限，但它与 Linux 的 'execute' 权限含义不同，现已废弃。)

  The mapping is performed as follows::
  (映射按如下方式执行：)

	Owner read				-> -r--r--r--
	Owner write				-> --w--w---w
	Owner read and filetype UnixExec	-> ---x--x--x
    These are then masked by ownmask, eg 700	-> -rwx------
	Possible owner mode permissions		-> -rwx------

	Other read				-> -r--r--r--
	Other write				-> --w--w--w-
	Other read and filetype UnixExec	-> ---x--x--x
    These are then masked by othmask, eg 077	-> ----rwxrwx
	Possible other mode permissions		-> ----rwxrwx

  Hence, with the default masks, if a file is owner read/write, and
  not a UnixExec filetype, then the permissions will be::
  (因此，使用默认掩码时，如果一个文件是所有者读/写，且不是 UnixExec 文件类型，那么权限将是：)

			-rw-------

  However, if the masks were ownmask=0770,othmask=0007, then this would
  be modified to::
  (但是，如果掩码为 ownmask=0770,othmask=0007，那么它将被修改为：)

			-rw-rw----

  There is no restriction on what you can do with these masks.  You may
  wish that either read bits give read access to the file for all, but
  keep the default write protection (ownmask=0755,othmask=0577)::
  (对这些掩码的使用没有任何限制。你可能希望任一读位为所有人提供文件的读访问权限，同时保留默认的写保护(ownmask=0755,othmask=0577)：)

			-rw-r--r--

  You can therefore tailor the permission translation to whatever you
  desire the permissions should be under Linux.
  (因此，你可以将权限转换定制为你希望在 Linux 下呈现的任何权限。)

RISC OS file type suffix
(RISC OS 文件类型后缀)
------------------------

  RISC OS file types are stored in bits 19..8 of the file load address.
  (RISC OS 文件类型存储在文件加载地址的第 19..8 位中。)

  To enable non-RISC OS systems to be used to store files without losing
  file type information, a file naming convention was devised (initially
  for use with NFS) such that a hexadecimal suffix of the form ,xyz
  denoted the file type: e.g. BasicFile,ffb is a BASIC (0xffb) file.  This
  naming convention is now also used by RISC OS emulators such as RPCEmu.
  (为了使非 RISC OS 系统能够在不丢失文件类型信息的情况下存储文件，人们设计了一种文件命名约定(最初用于 NFS)，即形如 ,xyz 的十六进制后缀表示文件类型：例如 BasicFile,ffb 是一个 BASIC (0xffb) 文件。这种命名约定现在也被 RPCEmu 等 RISC OS 模拟器使用。)

  Mounting an ADFS disc with option ftsuffix=1 will cause appropriate file
  type suffixes to be appended to file names read from a directory.  If the
  ftsuffix option is zero or omitted, no file type suffixes will be added.
  (使用选项 ftsuffix=1 挂载 ADFS 磁盘时，会使适当的文件类型后缀被附加到从目录中读取的文件名上。如果 ftsuffix 选项为零或省略，则不会添加任何文件类型后缀。)
