.. SPDX-License-Identifier: GPL-2.0

=====================================
eBPF Userspace API(eBPF 用户空间 API)
=====================================

eBPF is a kernel mechanism to provide a sandboxed runtime environment in the
Linux kernel for runtime extension and instrumentation without changing kernel
source code or loading kernel modules. eBPF programs can be attached to various
kernel subsystems, including networking, tracing and Linux security modules
(LSM).
(eBPF 是一种内核机制，用于在 Linux 内核中提供沙箱化的运行时环境，从而在不修改内核源码或加载内核模块的情况下进行运行时扩展和插桩。eBPF 程序可以挂载到各种内核子系统上，包括网络、跟踪以及 Linux 安全模块 (LSM)。)

For internal kernel documentation on eBPF, see Documentation/bpf/index.rst.
(关于 eBPF 的内核内部文档，请参阅 Documentation/bpf/index.rst。)

.. toctree::
   :maxdepth: 1

   syscall
