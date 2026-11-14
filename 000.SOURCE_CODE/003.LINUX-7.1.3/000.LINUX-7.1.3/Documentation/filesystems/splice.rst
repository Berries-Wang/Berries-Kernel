================
splice and pipes
================

(splice 与管道)

splice API
==========

splice is a method for moving blocks of data around inside the kernel,
without continually transferring them between the kernel and user space.
(splice(拼接)是一种在内核内部搬移数据块的方法，无需在内核与用户空间之间反复传输。)

.. kernel-doc:: fs/splice.c

pipes API
(管道 API)
=========

Pipe interfaces are all for in-kernel (builtin image) use. They are not
exported for use by modules.
(管道接口全部供内核内(内建镜像)使用。它们不导出供模块使用。)

.. kernel-doc:: include/linux/pipe_fs_i.h
   :internal:

.. kernel-doc:: fs/pipe.c
