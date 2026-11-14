.. SPDX-License-Identifier: GPL-2.0
.. include:: <isonum.txt>

=====
DLMFS
=====

(DLMFS)

A minimal DLM userspace interface implemented via a virtual file
system.
(通过虚拟文件系统实现的一个最小化 DLM 用户空间接口。)

dlmfs is built with OCFS2 as it requires most of its infrastructure.
(dlmfs 与 OCFS2 一起构建，因为它需要 OCFS2 的大部分基础设施。)

:Project web page:    http://ocfs2.wiki.kernel.org
(项目网页)
:Tools web page:      https://github.com/markfasheh/ocfs2-tools
(工具网页)
:OCFS2 mailing lists: https://subspace.kernel.org/lists.linux.dev.html
(OCFS2 邮件列表)

All code copyright 2005 Oracle except when otherwise noted.
(除另有说明外，所有代码版权归 2005 Oracle 所有。)

Credits
(致谢)
=======

Some code taken from ramfs which is Copyright |copy| 2000 Linus Torvalds
and Transmeta Corp.
(部分代码取自 ramfs，其版权 |copy| 2000 归 Linus Torvalds 和 Transmeta Corp. 所有。)

Mark Fasheh <mark.fasheh@oracle.com>

Caveats
(注意事项)
=======
- Right now it only works with the OCFS2 DLM, though support for other
  DLM implementations should not be a major issue.
  (目前它只能与 OCFS2 DLM 一起工作，不过对其他 DLM 实现的支持应该不会是主要问题。)

Mount options
(挂载选项)
=============
None
(无)

Usage
(用法)
=====

If you're just interested in OCFS2, then please see ocfs2.rst. The
rest of this document will be geared towards those who want to use
dlmfs for easy to setup and easy to use clustered locking in
userspace.
(如果你只对 OCFS2 感兴趣，请参阅 ocfs2.rst。本文档的其余部分面向那些希望使用 dlmfs 来获得易于设置、易于使用的用户空间集群锁的人。)

Setup
(设置)
=====

dlmfs requires that the OCFS2 cluster infrastructure be in
place. Please download ocfs2-tools from the above url and configure a
cluster.
(dlmfs 要求 OCFS2 集群基础设施已经就位。请从上面的网址下载 ocfs2-tools 并配置一个集群。)

You'll want to start heartbeating on a volume which all the nodes in
your lockspace can access. The easiest way to do this is via
ocfs2_hb_ctl (distributed with ocfs2-tools). Right now it requires
that an OCFS2 file system be in place so that it can automatically
find its heartbeat area, though it will eventually support heartbeat
against raw disks.
(你需要在一个锁空间中所有节点都能访问的卷上启动心跳。最简单的方法是使用 ocfs2_hb_ctl(随 ocfs2-tools 一起发布)。目前它要求已存在一个 OCFS2 文件系统，以便自动找到其心跳区域，不过它最终将支持针对原始磁盘的心跳。)

Please see the ocfs2_hb_ctl and mkfs.ocfs2 manual pages distributed
with ocfs2-tools.
(请参阅随 ocfs2-tools 一起发布的 ocfs2_hb_ctl 和 mkfs.ocfs2 手册页。)

Once you're heartbeating, DLM lock 'domains' can be easily created /
destroyed and locks within them accessed.
(一旦心跳开始运行，DLM 锁“域”就可以被轻松地创建/销毁，并且可以访问其中的锁。)

Locking
(锁定)
=====

Users may access dlmfs via standard file system calls, or they can use
'libo2dlm' (distributed with ocfs2-tools) which abstracts the file
system calls and presents a more traditional locking api.
(用户可以通过标准文件系统调用访问 dlmfs，也可以使用 'libo2dlm'(随 ocfs2-tools 一起发布)，它对文件系统调用进行了抽象，提供了更传统的锁 api。)

dlmfs handles lock caching automatically for the user, so a lock
request for an already acquired lock will not generate another DLM
call. Userspace programs are assumed to handle their own local
locking.
(dlmfs 为用户自动处理锁缓存，因此对已获取锁的锁请求不会再次产生 DLM 调用。用户空间程序被假定自行处理其本地锁。)

Two levels of locks are supported - Shared Read, and Exclusive.
Also supported is a Trylock operation.
(支持两种级别的锁——共享读(Shared Read)和独占(Exclusive)。此外还支持 Trylock 操作。)

For information on the libo2dlm interface, please see o2dlm.h,
distributed with ocfs2-tools.
(关于 libo2dlm 接口的信息，请参阅随 ocfs2-tools 一起发布的 o2dlm.h。)

Lock value blocks can be read and written to a resource via read(2)
and write(2) against the fd obtained via your open(2) call. The
maximum currently supported LVB length is 64 bytes (though that is an
OCFS2 DLM limitation). Through this mechanism, users of dlmfs can share
small amounts of data amongst their nodes.
(可以通过对你调用 open(2) 所获得的 fd 执行 read(2) 和 write(2) 来读取和写入资源的锁值块。当前支持的最大 LVB 长度为 64 字节(尽管这是 OCFS2 DLM 的限制)。通过这种机制，dlmfs 的用户可以在其节点之间共享少量数据。)

mkdir(2) signals dlmfs to join a domain (which will have the same name
as the resulting directory)
(mkdir(2) 通知 dlmfs 加入一个域(该域将与由此产生的目录同名))

rmdir(2) signals dlmfs to leave the domain
(rmdir(2) 通知 dlmfs 离开该域)

Locks for a given domain are represented by regular inodes inside the
domain directory.  Locking against them is done via the open(2) system
call.
(给定域的锁由域目录内的普通 inode 表示。针对它们的加锁通过 open(2) 系统调用完成。)

The open(2) call will not return until your lock has been granted or
an error has occurred, unless it has been instructed to do a trylock
operation. If the lock succeeds, you'll get an fd.
(在你的锁被授予或发生错误之前，open(2) 调用不会返回，除非它被指示执行 trylock 操作。如果加锁成功，你将得到一个 fd。)

open(2) with O_CREAT to ensure the resource inode is created - dlmfs does
not automatically create inodes for existing lock resources.
(使用 O_CREAT 调用 open(2) 以确保资源 inode 被创建——dlmfs 不会为已存在的锁资源自动创建 inode。)

============  ===========================
Open Flag     Lock Request Type(打开标志    锁请求类型)
============  ===========================
O_RDONLY      Shared Read(共享读)
O_RDWR        Exclusive(独占)
============  ===========================


============  ===========================
Open Flag     Resulting Locking Behavior(打开标志    产生的锁定行为)
============  ===========================
O_NONBLOCK    Trylock operation(trylock 操作)
============  ===========================

You must provide exactly one of O_RDONLY or O_RDWR.
(你必须提供 O_RDONLY 或 O_RDWR 两者中的恰好一个。)

If O_NONBLOCK is also provided and the trylock operation was valid but
could not lock the resource then open(2) will return ETXTBUSY.
(如果同时提供了 O_NONBLOCK，且 trylock 操作有效但无法锁定资源，那么 open(2) 将返回 ETXTBSY。)

close(2) drops the lock associated with your fd.
(close(2) 会释放与你的 fd 关联的锁。)

Modes passed to mkdir(2) or open(2) are adhered to locally. Chown is
supported locally as well. This means you can use them to restrict
access to the resources via dlmfs on your local node only.
(传递给 mkdir(2) 或 open(2) 的模式在本地得到遵守。chown 在本地也同样受支持。这意味着你可以使用它们来限制仅在本地节点上通过 dlmfs 对资源的访问。)

The resource LVB may be read from the fd in either Shared Read or
Exclusive modes via the read(2) system call. It can be written via
write(2) only when open in Exclusive mode.
(资源的 LVB 可以在共享读或独占模式下通过 read(2) 系统调用从 fd 读取。只有在以独占模式打开时才能通过 write(2) 写入。)

Once written, an LVB will be visible to other nodes who obtain Read
Only or higher level locks on the resource.
(一旦写入，LVB 将对在资源上获得只读或更高级别锁的其他节点可见。)

See Also
(另请参阅)
========
http://opendlm.sourceforge.net/cvsmirror/opendlm/docs/dlmbook_final.pdf

For more information on the VMS distributed locking API.
(更多关于 VMS 分布式锁 API 的信息。)
