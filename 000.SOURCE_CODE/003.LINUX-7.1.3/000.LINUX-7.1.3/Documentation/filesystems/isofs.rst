.. SPDX-License-Identifier: GPL-2.0

==================
ISO9660 Filesystem
==================

(ISO9660 文件系统)

Mount options that are the same as for msdos and vfat partitions.
(与 msdos 和 vfat 分区相同的挂载选项。)

  =========	========================================================
  gid=nnn	All files in the partition will be in group nnn. (分区中的所有文件都将属于组 nnn。)
  uid=nnn	All files in the partition will be owned by user id nnn. (分区中的所有文件都将属于用户 id nnn。)
  umask=nnn	The permission mask (see umask(1)) for the partition. (该分区的权限掩码(参见 umask(1))。)
  =========	========================================================

Mount options that are the same as vfat partitions. These are only useful
when using discs encoded using Microsoft's Joliet extensions.
(与 vfat 分区相同的挂载选项。这些选项仅在使用以 Microsoft 的 Joliet 扩展编码的光盘时才有用。)

 ==============	=============================================================
 iocharset=name Character set to use for converting from Unicode to
		ASCII.  Joliet filenames are stored in Unicode format, but
		Unix for the most part doesn't know how to deal with Unicode.
		There is also an option of doing UTF-8 translations with the
		utf8 option. (用于将 Unicode 转换为 ASCII 的字符集。Joliet 文件名以 Unicode 格式存储，但 Unix 大多不知道如何处理 Unicode。还可以使用 utf8 选项进行 UTF-8 转换。)
  utf8          Encode Unicode names in UTF-8 format. Default is no. (以 UTF-8 格式编码 Unicode 名称。默认为否。)
 ==============	=============================================================

Mount options unique to the isofs filesystem.
(isofs 文件系统独有的挂载选项。)

 ================= ============================================================
  block=512        Set the block size for the disk to 512 bytes (将磁盘的块大小设为 512 字节)
  block=1024       Set the block size for the disk to 1024 bytes (将磁盘的块大小设为 1024 字节)
  block=2048       Set the block size for the disk to 2048 bytes (将磁盘的块大小设为 2048 字节)
  check=relaxed    Matches filenames with different cases (匹配大小写不同的文件名)
  check=strict     Matches only filenames with the exact same case (仅匹配大小写完全相同的文件名)
  cruft            Try to handle badly formatted CDs. (尝试处理格式损坏的光盘。)
  map=off          Do not map non-Rock Ridge filenames to lower case (不将非 Rock Ridge 文件名映射为小写)
  map=normal       Map non-Rock Ridge filenames to lower case (将非 Rock Ridge 文件名映射为小写)
  map=acorn        As map=normal but also apply Acorn extensions if present (与 map=normal 相同，但如果存在则同时应用 Acorn 扩展)
  mode=xxx         Sets the permissions on files to xxx unless Rock Ridge
		   extensions set the permissions otherwise (将文件的权限设为 xxx，除非 Rock Ridge 扩展另行设置了权限)
  dmode=xxx        Sets the permissions on directories to xxx unless Rock Ridge
		   extensions set the permissions otherwise (将目录的权限设为 xxx，除非 Rock Ridge 扩展另行设置了权限)
  overriderockperm Set permissions on files and directories according to
		   'mode' and 'dmode' even though Rock Ridge extensions are
		   present. (尽管存在 Rock Ridge 扩展，仍按 'mode' 和 'dmode' 设置文件和目录的权限)
  nojoliet         Ignore Joliet extensions if they are present. (如果存在 Joliet 扩展则忽略它们。)
  norock           Ignore Rock Ridge extensions if they are present. (如果存在 Rock Ridge 扩展则忽略它们。)
  hide		   Completely strip hidden files from the file system. (从文件系统中完全剥离隐藏文件。)
  showassoc	   Show files marked with the 'associated' bit (显示标记了 'associated' 位的文件)
  unhide	   Deprecated; showing hidden files is now default;
		   If given, it is a synonym for 'showassoc' which will
		   recreate previous unhide behavior (已弃用；显示隐藏文件现在是默认行为；如果给出该选项，它是 'showassoc' 的同义词，将重现以前的 unhide 行为)
  session=x        Select number of session on multisession CD (在多区段光盘上选择区段编号)
  sbsector=xxx     Session begins from sector xxx (区段从扇区 xxx 开始)
 ================= ============================================================

Recommended documents about ISO 9660 standard are located at:
(关于 ISO 9660 标准的推荐文档位于：)

- http://www.y-adagio.com/
- https://ecma-international.org/wp-content/uploads/ECMA-119_2nd_edition_december_1987.pdf

Quoting from the PDF "This 2nd Edition of Standard ECMA-119 is technically
identical with ISO 9660.", so it is a valid and gratis substitute of the
official ISO specification.
(引用该 PDF 的原话："标准 ECMA-119 的第 2 版在技术上与 ISO 9660 相同。"，因此它是官方 ISO 规范的有效且免费的替代品。)
