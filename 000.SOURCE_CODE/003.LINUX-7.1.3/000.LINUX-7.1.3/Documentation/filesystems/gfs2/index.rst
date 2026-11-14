.. SPDX-License-Identifier: GPL-2.0

====================
Global File System 2
====================

(全局文件系统 2)

Overview
(概述)
========

GFS2 is a cluster file system. It allows a cluster of computers to
simultaneously use a block device that is shared between them (with FC,
iSCSI, NBD, etc).  GFS2 reads and writes to the block device like a local
file system, but also uses a lock module to allow the computers coordinate
their I/O so file system consistency is maintained.  One of the nifty
features of GFS2 is perfect consistency -- changes made to the file system
on one machine show up immediately on all other machines in the cluster.
(GFS2 是一个集群文件系统。它允许一个计算机集群同时使用彼此之间共享的块设备(通过 FC、iSCSI、NBD 等)。GFS2 像本地文件系统一样读写块设备，同时还使用一个锁模块让各计算机协调其 I/O，从而保持文件系统的一致性。GFS2 的一个精妙特性是完美的一致性 —— 在一台机器上对文件系统所做的更改会立即显示在集群中所有其他机器上。)

GFS2 uses interchangeable inter-node locking mechanisms, the currently
supported mechanisms are:
(GFS2 使用可互换的节点间锁定机制，当前支持的机制有：)

  lock_nolock
    - allows GFS2 to be used as a local file system
      (允许将 GFS2 用作本地文件系统)

  lock_dlm
    - uses the distributed lock manager (dlm) for inter-node locking.
      The dlm is found at linux/fs/dlm/
      (使用分布式锁管理器(dlm)进行节点间锁定。dlm 位于 linux/fs/dlm/。)

lock_dlm depends on user space cluster management systems found
at the URL above.
(lock_dlm 依赖于上面 URL 处的用户空间集群管理系统。)

To use GFS2 as a local file system, no external clustering systems are
needed, simply::
(要将 GFS2 用作本地文件系统，不需要外部集群系统，只需：)

  $ mkfs -t gfs2 -p lock_nolock -j 1 /dev/block_device
  $ mount -t gfs2 /dev/block_device /dir

The gfs2-utils package is required on all cluster nodes and, for lock_dlm, you
will also need the dlm and corosync user space utilities configured as per the
documentation.
(所有集群节点都需要 gfs2-utils 包；对于 lock_dlm，还需要按照文档配置 dlm 和 corosync 用户空间工具。)

gfs2-utils can be found at https://pagure.io/gfs2-utils
(gfs2-utils 可以在 https://pagure.io/gfs2-utils 找到。)

GFS2 is not on-disk compatible with previous versions of GFS, but it
is pretty close.
(GFS2 在磁盘上与先前版本的 GFS 不兼容，但已经相当接近。)

The following man pages are available from gfs2-utils:
(gfs2-utils 提供了以下手册页：)

  ============		=============================================
  fsck.gfs2		to repair a filesystem (修复文件系统)
  gfs2_grow		to expand a filesystem online (在线扩展文件系统)
  gfs2_jadd		to add journals to a filesystem online (在线向文件系统添加日志)
  tunegfs2		to manipulate, examine and tune a filesystem (操作、检查和调优文件系统)
  gfs2_convert		to convert a gfs filesystem to GFS2 in-place (将 gfs 文件系统就地转换为 GFS2)
  mkfs.gfs2		to make a filesystem (创建文件系统)
  ============		=============================================

Implementation Notes
(实现说明)
====================

.. toctree::
   :maxdepth: 1

   glocks
   uevents
