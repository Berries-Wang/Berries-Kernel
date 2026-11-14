Buffer Heads
(缓冲头)
============

Linux uses buffer heads to maintain state about individual filesystem blocks.
Buffer heads are deprecated and new filesystems should use iomap instead.
(Linux 使用 buffer head 来维护单个文件系统块的状态。buffer head 已被弃用，新的文件系统应改用 iomap。)

Functions
(函数)
---------

.. kernel-doc:: include/linux/buffer_head.h
.. kernel-doc:: fs/buffer.c
   :export:
