.. SPDX-License-Identifier: GPL-2.0

==========
spu_create
==========

(spu_create)

Name
(名称)
====
       spu_create - create a new spu context
       (spu_create - 创建一个新的 SPU 上下文)


Synopsis
(概要)
========

       ::

         #include <sys/types.h>
         #include <sys/spu.h>

         int spu_create(const char *pathname, int flags, mode_t mode);

Description
(描述)
===========
       The  spu_create  system call is used on PowerPC machines that implement
       the Cell Broadband Engine Architecture in order to  access  Synergistic
       Processor  Units (SPUs). It creates a new logical context for an SPU in
       pathname and returns a handle to associated  with  it.   pathname  must
       point  to  a  non-existing directory in the mount point of the SPU file
       system (spufs).  When spu_create is successful, a directory  gets  cre-
       ated on pathname and it is populated with files.
       (spu_create 系统调用用于实现了 Cell 宽带引擎架构的 PowerPC 机器上，以访问协处理器单元(SPU)。它在 pathname 中为一个 SPU 创建新的逻辑上下文，并返回与之关联的句柄。pathname 必须指向 SPU 文件系统(spufs)挂载点中一个尚不存在的目录。当 spu_create 成功时，会在 pathname 处创建一个目录，并向其中填充文件。)

       The  returned  file  handle can only be passed to spu_run(2) or closed,
       other operations are not defined on it. When it is closed, all  associ-
       ated  directory entries in spufs are removed. When the last file handle
       pointing either inside  of  the  context  directory  or  to  this  file
       descriptor is closed, the logical SPU context is destroyed.
       (返回的文件句柄只能被传递给 spu_run(2) 或被关闭，其他操作在它上面未作定义。当它被关闭时，spufs 中所有相关的目录项都会被移除。当指向该上下文目录内部或指向该文件描述符的最后一个文件句柄被关闭时，这个逻辑 SPU 上下文就会被销毁。)

       The  parameter flags can be zero or any bitwise or'd combination of the
       following constants:
       (参数 flags 可以为零，或以下常量按位或的组合：)

       SPU_RAWIO
              Allow mapping of some of the hardware registers of the SPU  into
              user space. This flag requires the CAP_SYS_RAWIO capability, see
              capabilities(7).
              (允许将 SPU 的某些硬件寄存器映射到用户空间。此标志需要 CAP_SYS_RAWIO 能力，参见 capabilities(7)。)

       The mode parameter specifies the permissions used for creating the  new
       directory  in  spufs.   mode is modified with the user's umask(2) value
       and then used for both the directory and the files contained in it. The
       file permissions mask out some more bits of mode because they typically
       support only read or write access. See stat(2) for a full list  of  the
       possible mode values.
       (mode 参数指定在 spufs 中创建新目录时所使用的权限。mode 会先用用户的 umask(2) 值进行修改，然后同时用于该目录以及其中包含的文件。文件权限会再屏蔽掉 mode 的一些位，因为它们通常只支持读或写访问。完整的 mode 取值列表请参见 stat(2)。)


Return Value
(返回值)
============
       spu_create  returns a new file descriptor. It may return -1 to indicate
       an error condition and set errno to  one  of  the  error  codes  listed
       below.
       (spu_create 返回一个新的文件描述符。它可能返回 -1 以指示错误情况，并将 errno 设置为下面列出的错误码之一。)


Errors
(错误)
======
       EACCES
              The  current  user does not have write access on the spufs mount
              point.
              (当前用户对 spufs 挂载点没有写权限。)

       EEXIST An SPU context already exists at the given path name.
              (给定路径名处已存在一个 SPU 上下文。)

       EFAULT pathname is not a valid string pointer in  the  current  address
              space.
              (pathname 不是当前地址空间中有效的字符串指针。)

       EINVAL pathname is not a directory in the spufs mount point.
              (pathname 不是 spufs 挂载点中的目录。)

       ELOOP  Too many symlinks were found while resolving pathname.
              (在解析 pathname 时发现了太多的符号链接。)

       EMFILE The process has reached its maximum open file limit.
              (进程已达到其最大打开文件限制。)

       ENAMETOOLONG
              pathname was too long.
              (pathname 过长。)

       ENFILE The system has reached the global open file limit.
              (系统已达到全局打开文件限制。)

       ENOENT Part of pathname could not be resolved.
              (pathname 的某一部分无法被解析。)

       ENOMEM The kernel could not allocate all resources required.
              (内核无法分配所需的全部资源。)

       ENOSPC There  are  not  enough  SPU resources available to create a new
              context or the user specific limit for the number  of  SPU  con-
              texts has been reached.
              (没有足够的 SPU 资源来创建新上下文，或者已达到用户特定的 SPU 上下文数量限制。)

       ENOSYS the functionality is not provided by the current system, because
              either the hardware does not provide SPUs or the spufs module is
              not loaded.
              (当前系统不提供该功能，因为硬件不提供 SPU，或者 spufs 模块未加载。)

       ENOTDIR
              A part of pathname is not a directory.
              (pathname 的一部分不是目录。)



Notes
(注)
=====
       spu_create  is  meant  to  be used from libraries that implement a more
       abstract interface to SPUs, not to be used from  regular  applications.
       See  http://www.bsc.es/projects/deepcomputing/linuxoncell/ for the rec-
       ommended libraries.
       (spu_create 旨在由为 SPU 实现了更抽象接口的库来使用，而不是供常规应用程序使用。推荐的库请参见 http://www.bsc.es/projects/deepcomputing/linuxoncell/。)


Files
(文件)
=====
       pathname must point to a location beneath the mount point of spufs.  By
       convention, it gets mounted in /spu.
       (pathname 必须指向 spufs 挂载点之下的某个位置。按照惯例，它被挂载在 /spu。)


Conforming to
(遵循标准)
=============
       This call is Linux specific and only implemented by the ppc64
       architecture. Programs using this system call are not portable.
       (该调用是 Linux 特有的，仅由 ppc64 架构实现。使用该系统调用的程序不具备可移植性。)


Bugs
(缺陷)
====
       The code does not yet fully implement all features lined out here.
       (该代码尚未完全实现这里列出的所有功能。)


Author
(作者)
======
       Arnd Bergmann <arndb@de.ibm.com>

See Also
(另请参阅)
========
       capabilities(7), close(2), spu_run(2), spufs(7)
