.. _filesystems_index:

===============================
Filesystems in the Linux kernel
===============================

(Linux 内核中的文件系统)

This under-development manual will, some glorious day, provide
comprehensive information on how the Linux virtual filesystem (VFS) layer
works, along with the filesystems that sit below it.  For now, what we have
can be found below.
(这本尚在编写中的手册终将在某个光辉的日子里，全面介绍 Linux 虚拟文件系统
(VFS)层如何工作，以及位于其下的各个文件系统。目前，我们已有的内容可以在
下方找到。)

Core VFS documentation
(VFS 核心文档)
======================

See these manuals for documentation about the VFS layer itself and how its
algorithms work.
(关于 VFS 层本身及其算法如何工作的文档，请参阅这些手册。)

.. toctree::
   :maxdepth: 2

   vfs
   path-lookup
   api-summary
   splice
   locking
   directory-locking
   devpts
   dnotify
   fiemap
   files
   locks
   mmap_prepare
   multigrain-ts
   mount_api
   quota
   seq_file
   sharedsubtree
   idmappings
   iomap/index

   automount-support

   caching/index

   porting

Filesystem support layers
(文件系统支撑层)
=========================

Documentation for the support code within the filesystem layer for use in
filesystem implementations.
(关于文件系统层内供文件系统实现使用的支撑代码的文档。)

.. toctree::
   :maxdepth: 2

   buffer
   journalling
   fscrypt
   fsverity
   netfs_library

Filesystems
(文件系统)
===========

Documentation for filesystem implementations.
(关于各文件系统实现的文档。)

.. toctree::
   :maxdepth: 2

   9p
   adfs
   affs
   afs
   autofs
   autofs-mount-control
   befs
   bfs
   btrfs
   ceph
   coda
   configfs
   cramfs
   dax
   debugfs
   dlmfs
   ecryptfs
   efivarfs
   erofs
   ext2
   ext3
   ext4/index
   f2fs
   gfs2/index
   hfs
   hfsplus
   hpfs
   fuse/index
   inotify
   isofs
   nilfs2
   nfs/index
   ntfs
   ntfs3
   ocfs2
   ocfs2-online-filecheck
   omfs
   orangefs
   overlayfs
   proc
   qnx6
   ramfs-rootfs-initramfs
   relay
   resctrl
   romfs
   smb/index
   spufs/index
   squashfs
   sysfs
   tmpfs
   ubifs
   ubifs-authentication
   udf
   virtiofs
   vfat
   xfs/index
   zonefs
