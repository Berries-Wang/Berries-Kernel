==================================================================
The Linux kernel user-space API guide(Linux 内核用户空间 API 指南)
==================================================================

.. _man-pages: https://www.kernel.org/doc/man-pages/

While much of the kernel's user-space API is documented elsewhere
(particularly in the man-pages_ project), some user-space information can
also be found in the kernel tree itself.  This manual is intended to be the
place where this information is gathered.
(内核用户空间 API 的很大一部分已在别处文档化 (尤其在 man-pages_ 项目
中)，但一些用户空间信息也可以在内核源码树本身中找到。本手册旨在成为
汇集这些信息的地方。)


System calls(系统调用)
======================

.. toctree::
   :maxdepth: 1

   unshare
   futex2
   ebpf/index
   ioctl/index
   mseal
   rseq

Security-related interfaces(安全相关接口)
=========================================

.. toctree::
   :maxdepth: 1

   no_new_privs
   seccomp_filter
   landlock
   lsm
   mfd_noexec
   spec_ctrl
   tee
   check_exec

Devices and I/O(设备与 I/O)
===========================

.. toctree::
   :maxdepth: 1

   accelerators/ocxl
   dma-buf-heaps
   dma-buf-alloc-exchange
   fwctl/index
   gpio/index
   iommufd
   media/index
   dcdbas
   vduse
   isapnp

Everything else(其他内容)
=========================

.. toctree::
   :maxdepth: 1

   ELF
   liveupdate
   netlink/index
   sysfs-platform_profile
   vduse
   futex2
   perf_ring_buffer
   ntsync
