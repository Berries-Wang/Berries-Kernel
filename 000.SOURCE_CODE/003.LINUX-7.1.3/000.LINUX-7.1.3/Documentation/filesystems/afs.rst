.. SPDX-License-Identifier: GPL-2.0

====================
kAFS: AFS FILESYSTEM
====================

(kAFS：AFS 文件系统)

.. Contents:

 - Overview.
   (概述。)
 - Usage.
   (用法。)
 - Mountpoints.
   (挂载点。)
 - Dynamic root.
   (动态根。)
 - Proc filesystem.
   (Proc 文件系统。)
 - The cell database.
   (cell(单元)数据库。)
 - Security.
   (安全性。)
 - The @sys substitution.
   (@sys 替换。)


Overview
(概述)
========

This filesystem provides a fairly simple secure AFS filesystem driver. It is
under development and does not yet provide the full feature set.  The features
it does support include:
(该文件系统提供了一个相当简单的安全 AFS 文件系统驱动程序。它仍在开发中，尚未提供完整的功能集。它已支持的功能包括：)

 (*) Security (currently only AFS kaserver and KerberosIV tickets).
     (安全性(目前只有 AFS kaserver 和 KerberosIV 票据)。)

 (*) File reading and writing.
     (文件读取与写入。)

 (*) Automounting.
     (自动挂载。)

 (*) Local caching (via fscache).
     (本地缓存(通过 fscache)。)

It does not yet support the following AFS features:
(它尚不支持以下 AFS 功能：)

 (*) pioctl() system call.
     (pioctl() 系统调用。)


Compilation
(编译)
===========

The filesystem should be enabled by turning on the kernel configuration
options::
(应当通过打开以下内核配置选项来启用该文件系统：)

	CONFIG_AF_RXRPC		- The RxRPC protocol transport
	CONFIG_RXKAD		- The RxRPC Kerberos security handler
	CONFIG_AFS_FS		- The AFS filesystem

Additionally, the following can be turned on to aid debugging::
(此外，可以打开以下选项来辅助调试：)

	CONFIG_AF_RXRPC_DEBUG	- Permit AF_RXRPC debugging to be enabled
	CONFIG_AFS_DEBUG	- Permit AFS debugging to be enabled

They permit the debugging messages to be turned on dynamically by manipulating
the masks in the following files::
(它们允许通过操纵以下文件中的掩码来动态地打开调试消息：)

	/sys/module/af_rxrpc/parameters/debug
	/sys/module/kafs/parameters/debug


Usage
(用法)
=====

When inserting the driver modules the root cell must be specified along with a
list of volume location server IP addresses::
(插入驱动程序模块时，必须指定根 cell 以及卷位置(volume location)服务器 IP 地址的列表：)

	modprobe rxrpc
	modprobe kafs rootcell=cambridge.redhat.com:172.16.18.73:172.16.18.91

The first module is the AF_RXRPC network protocol driver.  This provides the
RxRPC remote operation protocol and may also be accessed from userspace.  See:
(第一个模块是 AF_RXRPC 网络协议驱动程序。它提供 RxRPC 远程操作协议，也可以从用户空间访问。参见：)

	Documentation/networking/rxrpc.rst

The second module is the kerberos RxRPC security driver, and the third module
is the actual filesystem driver for the AFS filesystem.
(第二个模块是 kerberos RxRPC 安全驱动程序，第三个模块则是 AFS 文件系统实际的文件系统驱动程序。)

Once the module has been loaded, more modules can be added by the following
procedure::
(模块加载之后，可以通过以下步骤添加更多的模块：)

	echo add grand.central.org 18.9.48.14:128.2.203.61:130.237.48.87 >/proc/fs/afs/cells

Where the parameters to the "add" command are the name of a cell and a list of
volume location servers within that cell, with the latter separated by colons.
(其中 "add" 命令的参数是一个 cell 的名称以及该 cell 内卷位置服务器的列表，后者以冒号分隔。)

Filesystems can be mounted anywhere by commands similar to the following::
(文件系统可以用类似以下的命令挂载到任何地方：)

	mount -t afs "%cambridge.redhat.com:root.afs." /afs
	mount -t afs "#cambridge.redhat.com:root.cell." /afs/cambridge
	mount -t afs "#root.afs." /afs
	mount -t afs "#root.cell." /afs/cambridge

Where the initial character is either a hash or a percent symbol depending on
whether you definitely want a R/W volume (percent) or whether you'd prefer a
R/O volume, but are willing to use a R/W volume instead (hash).
(其中起始字符是井号还是百分号，取决于你是明确想要一个 R/W 卷(百分号)，还是更倾向于 R/O 卷、但也愿意改用 R/W 卷(井号)。)

The name of the volume can be suffixes with ".backup" or ".readonly" to
specify connection to only volumes of those types.
(卷的名称可以加上 ".backup" 或 ".readonly" 后缀，以指定只连接到那些类型的卷。)

The name of the cell is optional, and if not given during a mount, then the
named volume will be looked up in the cell specified during modprobe.
(cell 的名称是可选的，如果在挂载时没有给出，那么指定的卷将在执行 modprobe 时所指定的 cell 中查找。)

Additional cells can be added through /proc (see later section).
(可以通过 /proc 添加更多的 cell(参见后面的章节)。)


Mountpoints
(挂载点)
===========

AFS has a concept of mountpoints. In AFS terms, these are specially formatted
symbolic links (of the same form as the "device name" passed to mount).  kAFS
presents these to the user as directories that have a follow-link capability
(i.e.: symbolic link semantics).  If anyone attempts to access them, they will
automatically cause the target volume to be mounted (if possible) on that site.
(AFS 有挂载点的概念。用 AFS 的术语来说，这些是特殊格式的符号链接(其形式与传递给 mount 的"设备名"相同)。kAFS 把它们作为具有跟随链接能力的目录(即符号链接语义)呈现给用户。如果有人试图访问它们，将会自动(如果可能的话)使目标卷挂载到那个位置上。)

Automatically mounted filesystems will be automatically unmounted approximately
twenty minutes after they were last used.  Alternatively they can be unmounted
directly with the umount() system call.
(自动挂载的文件系统会在最后一次被使用约二十分钟之后被自动卸载。或者，也可以用 umount() 系统调用直接卸载它们。)

Manually unmounting an AFS volume will cause any idle submounts upon it to be
culled first.  If all are culled, then the requested volume will also be
unmounted, otherwise error EBUSY will be returned.
(手动卸载一个 AFS 卷会先剔除其上所有空闲的子挂载。如果全部被剔除，那么所请求的卷也会被卸载，否则将返回错误 EBUSY。)

This can be used by the administrator to attempt to unmount the whole AFS tree
mounted on /afs in one go by doing::
(管理员可以用它来尝试一次性卸载整个挂载在 /afs 上的 AFS 树，方法是执行：)

	umount /afs


Dynamic Root
(动态根)
============

A mount option is available to create a serverless mount that is only usable
for dynamic lookup.  Creating such a mount can be done by, for example::
(有一个挂载选项可以创建只能用于动态查找的无服务器挂载。创建这样的挂载可以这样做，例如：)

	mount -t afs none /afs -o dyn

This creates a mount that just has an empty directory at the root.  Attempting
to look up a name in this directory will cause a mountpoint to be created that
looks up a cell of the same name, for example::
(这会创建一个根处只有一个空目录的挂载。尝试在这个目录中查找名字将导致创建一个挂载点，该挂载点会查找同名的 cell，例如：)

	ls /afs/grand.central.org/


Proc Filesystem
(Proc 文件系统)
===============

The AFS module creates a "/proc/fs/afs/" directory and populates it:
(AFS 模块会创建一个 "/proc/fs/afs/" 目录并填充它：)

  (*) A "cells" file that lists cells currently known to the afs module and
      their usage counts::
      (一个 "cells" 文件，列出 afs 模块当前已知的 cell 及其使用计数：)

	[root@andromeda ~]# cat /proc/fs/afs/cells
	USE NAME
	  3 cambridge.redhat.com

  (*) A directory per cell that contains files that list volume location
      servers, volumes, and active servers known within that cell::
      (每个 cell 一个目录，其中包含的文件会列出该 cell 内已知的卷位置服务器、卷和活动服务器：)

	[root@andromeda ~]# cat /proc/fs/afs/cambridge.redhat.com/servers
	USE ADDR            STATE
	  4 172.16.18.91        0
	[root@andromeda ~]# cat /proc/fs/afs/cambridge.redhat.com/vlservers
	ADDRESS
	172.16.18.91
	[root@andromeda ~]# cat /proc/fs/afs/cambridge.redhat.com/volumes
	USE STT VLID[0]  VLID[1]  VLID[2]  NAME
	  1 Val 20000000 20000001 20000002 root.afs


The Cell Database
(cell 数据库)
=================

The filesystem maintains an internal database of all the cells it knows and the
IP addresses of the volume location servers for those cells.  The cell to which
the system belongs is added to the database when modprobe is performed by the
"rootcell=" argument or, if compiled in, using a "kafs.rootcell=" argument on
the kernel command line.
(该文件系统维护着一个内部数据库，记录它所知道的所有 cell 以及这些 cell 的卷位置服务器的 IP 地址。系统所属的 cell 会在执行 modprobe 时通过 "rootcell=" 参数加入该数据库；如果是编译进内核的，则通过内核命令行上的 "kafs.rootcell=" 参数加入。)

Further cells can be added by commands similar to the following::
(可以通过类似以下的命令添加更多的 cell：)

	echo add CELLNAME VLADDR[:VLADDR][:VLADDR]... >/proc/fs/afs/cells
	echo add grand.central.org 18.9.48.14:128.2.203.61:130.237.48.87 >/proc/fs/afs/cells

No other cell database operations are available at this time.
(目前没有其他可用的 cell 数据库操作。)


Security
(安全性)
========

Secure operations are initiated by acquiring a key using the klog program.  A
very primitive klog program is available at:
(安全操作通过使用 klog 程序获取密钥来发起。一个非常原始的 klog 程序可以在以下位置获得：)

	https://people.redhat.com/~dhowells/rxrpc/klog.c

This should be compiled by::
(应当这样编译它：)

	make klog LDLIBS="-lcrypto -lcrypt -lkrb4 -lkeyutils"

And then run as::
(然后这样运行：)

	./klog

Assuming it's successful, this adds a key of type RxRPC, named for the service
and cell, e.g.: "afs@<cellname>".  This can be viewed with the keyctl program or
by cat'ing /proc/keys::
(假设成功的话，这会添加一个 RxRPC 类型的密钥，以服务和 cell 命名，例如："afs@<cellname>"。可以用 keyctl 程序或通过 cat /proc/keys 查看：)

	[root@andromeda ~]# keyctl show
	Session Keyring
	       -3 --alswrv      0     0  keyring: _ses.3268
		2 --alswrv      0     0   \_ keyring: _uid.0
	111416553 --als--v      0     0   \_ rxrpc: afs@CAMBRIDGE.REDHAT.COM

Currently the username, realm, password and proposed ticket lifetime are
compiled into the program.
(目前，用户名、realm、密码以及建议的票据生存期都被编译进了程序。)

It is not required to acquire a key before using AFS facilities, but if one is
not acquired then all operations will be governed by the anonymous user parts
of the ACLs.
(在使用 AFS 设施之前并不需要先获取密钥，但如果未获取，那么所有操作都将受 ACL 中匿名用户部分的管辖。)

If a key is acquired, then all AFS operations, including mounts and automounts,
made by a possessor of that key will be secured with that key.
(如果获取了密钥，那么该密钥的持有者所进行的所有 AFS 操作(包括挂载和自动挂载)都将由该密钥加以保护。)

If a file is opened with a particular key and then the file descriptor is
passed to a process that doesn't have that key (perhaps over an AF_UNIX
socket), then the operations on the file will be made with key that was used to
open the file.
(如果一个文件是用某个特定密钥打开的，随后文件描述符又被传递给一个不拥有该密钥的进程(也许是通过 AF_UNIX 套接字)，那么对该文件的操作将使用打开该文件时所用的那个密钥来进行。)


The @sys Substitution
(@sys 替换)
=====================

The list of up to 16 @sys substitutions for the current network namespace can
be configured by writing a list to /proc/fs/afs/sysname::
(当前网络命名空间的多达 16 个 @sys 替换所组成的列表，可以通过向 /proc/fs/afs/sysname 写入一个列表来配置：)

	[root@andromeda ~]# echo foo amd64_linux_26 >/proc/fs/afs/sysname

or cleared entirely by writing an empty list::
(或者通过写入一个空列表来将其完全清空：)

	[root@andromeda ~]# echo >/proc/fs/afs/sysname

The current list for current network namespace can be retrieved by::
(当前网络命名空间的当前列表可以这样获取：)

	[root@andromeda ~]# cat /proc/fs/afs/sysname
	foo
	amd64_linux_26

When @sys is being substituted for, each element of the list is tried in the
order given.
(当 @sys 被替换时，列表中的每个元素会按给定的顺序依次尝试。)

By default, the list will contain one item that conforms to the pattern
"<arch>_linux_26", amd64 being the name for x86_64.
(默认情况下，该列表将包含一个符合 "<arch>_linux_26" 模式的条目，amd64 是 x86_64 的名称。)
