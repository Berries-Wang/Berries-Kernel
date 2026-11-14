.. SPDX-License-Identifier: GPL-2.0

======================================================
eCryptfs: A stacked cryptographic filesystem for Linux
======================================================

(eCryptfs: 面向 Linux 的堆叠式加密文件系统)

eCryptfs is free software. Please see the file COPYING for details.
For documentation, please see the files in the doc/ subdirectory.  For
building and installation instructions please see the INSTALL file.
(eCryptfs 是自由软件。详情请参见 COPYING 文件。文档请参见 doc/ 子目录中的文件。构建和安装说明请参见 INSTALL 文件。)

:Maintainer: Phillip Hellewell
             (维护者: Phillip Hellewell)
:Lead developer: Michael A. Halcrow <mhalcrow@us.ibm.com>
                 (首席开发者: Michael A. Halcrow <mhalcrow@us.ibm.com>)
:Developers: Michael C. Thompson
             Kent Yoder
             (开发者: Michael C. Thompson、Kent Yoder)
:Web Site: http://ecryptfs.sf.net
           (网站: http://ecryptfs.sf.net)

This software is currently undergoing development. Make sure to
maintain a backup copy of any data you write into eCryptfs.
(本软件目前正在开发中。请确保对写入 eCryptfs 的任何数据维护一份备份副本。)

eCryptfs requires the userspace tools downloadable from the
SourceForge site:
(eCryptfs 需要可从 SourceForge 站点下载的用户空间工具：)

http://sourceforge.net/projects/ecryptfs/

Userspace requirements include:
(用户空间要求包括：)

- David Howells' userspace keyring headers and libraries (version
  1.0 or higher), obtainable from
  http://people.redhat.com/~dhowells/keyutils/
  (David Howells 的用户空间密钥环头文件和库(1.0 或更高版本)，可从 http://people.redhat.com/~dhowells/keyutils/ 获取)
- Libgcrypt


.. note::

   In the beta/experimental releases of eCryptfs, when you upgrade
   eCryptfs, you should copy the files to an unencrypted location and
   then copy the files back into the new eCryptfs mount to migrate the
   files.
   (在 eCryptfs 的 beta/实验版本中，当你升级 eCryptfs 时，你应该将文件复制到一个未加密的位置，然后再将文件复制回新的 eCryptfs 挂载中，以迁移这些文件。)


Mount-wide Passphrase
(整个挂载范围的口令)
=====================

Create a new directory into which eCryptfs will write its encrypted
files (i.e., /root/crypt).  Then, create the mount point directory
(i.e., /mnt/crypt).  Now it's time to mount eCryptfs::
(创建一个新目录，eCryptfs 将在其中写入其加密文件(例如 /root/crypt)。然后，创建挂载点目录(例如 /mnt/crypt)。现在到了挂载 eCryptfs 的时候：)

    mount -t ecryptfs /root/crypt /mnt/crypt

You should be prompted for a passphrase and a salt (the salt may be
blank).
(系统应会提示你输入口令和盐值(salt)(盐值可以为空)。)

Try writing a new file::
(尝试写入一个新文件：)

    echo "Hello, World" > /mnt/crypt/hello.txt

The operation will complete.  Notice that there is a new file in
/root/crypt that is at least 12288 bytes in size (depending on your
host page size).  This is the encrypted underlying file for what you
just wrote.  To test reading, from start to finish, you need to clear
the user session keyring:
(操作将会完成。注意，/root/crypt 中有一个大小至少为 12288 字节的新文件(取决于你主机的页大小)。这是你刚刚写入的内容所对应的加密底层文件。要测试从头到尾的读取，你需要清除用户会话密钥环：)

keyctl clear @u

Then umount /mnt/crypt and mount again per the instructions given
above.
(然后按照上面给出的说明卸载 /mnt/crypt 并再次挂载。)

::

    cat /mnt/crypt/hello.txt


Notes
(注意事项)
=====

eCryptfs version 0.1 should only be mounted on (1) empty directories
or (2) directories containing files only created by eCryptfs. If you
mount a directory that has pre-existing files not created by eCryptfs,
then behavior is undefined. Do not run eCryptfs in higher verbosity
levels unless you are doing so for the sole purpose of debugging or
development, since secret values will be written out to the system log
in that case.
(eCryptfs 0.1 版只应挂载在(1)空目录或(2)仅包含由 eCryptfs 创建的文件的目录上。如果你挂载的目录中存在并非由 eCryptfs 创建的既有文件，则行为是未定义的。除非你这么做纯粹是为了调试或开发，否则不要在更高的详细级别上运行 eCryptfs，因为在这种情况下机密值会被写出到系统日志中。)


Mike Halcrow
mhalcrow@us.ibm.com
