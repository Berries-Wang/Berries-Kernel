=======================
BPF licensing(BPF 许可)
=======================

Background(背景)
================

* Classic BPF was BSD licensed
  (经典 BPF 采用 BSD 许可)

"BPF" was originally introduced as BSD Packet Filter in
http://www.tcpdump.org/papers/bpf-usenix93.pdf. The corresponding instruction
set and its implementation came from BSD with BSD license. That original
instruction set is now known as "classic BPF".
("BPF"最初在 http://www.tcpdump.org/papers/bpf-usenix93.pdf 中以 BSD
Packet Filter 的名义引入。相应的指令集及其实现来自采用 BSD 许可的
BSD。那套原始指令集如今被称为 "classic BPF"。)

However an instruction set is a specification for machine-language interaction,
similar to a programming language.  It is not a code. Therefore, the
application of a BSD license may be misleading in a certain context, as the
instruction set may enjoy no copyright protection.
(然而，指令集是机器语言交互的规范，类似于一种编程语言，而不是代码。
因此，在某些上下文中使用 BSD 许可可能造成误导，因为指令集可能并不
享有版权保护。)

* eBPF (extended BPF) instruction set continues to be BSD
  (eBPF(扩展 BPF)指令集继续采用 BSD)

In 2014, the classic BPF instruction set was significantly extended. We
typically refer to this instruction set as eBPF to disambiguate it from cBPF.
The eBPF instruction set is still BSD licensed.
(2014 年，经典 BPF 指令集得到了大幅扩展。为了与 cBPF 区分，我们通常
把这套指令集称为 eBPF。eBPF 指令集仍然采用 BSD 许可。)

Implementations of eBPF(eBPF 的实现)
====================================

Using the eBPF instruction set requires implementing code in both kernel space
and user space.
(使用 eBPF 指令集需要在内核空间和用户空间都实现代码。)

In Linux Kernel(在 Linux 内核中)
--------------------------------

The reference implementations of the eBPF interpreter and various just-in-time
compilers are part of Linux and are GPLv2 licensed. The implementation of
eBPF helper functions is also GPLv2 licensed. Interpreters, JITs, helpers,
and verifiers are called eBPF runtime.
(eBPF 解释器和各种即时(JIT)编译器的参考实现是 Linux 的一部分，采用
GPLv2 许可。eBPF 辅助函数的实现也采用 GPLv2 许可。解释器、JIT、辅助
函数和验证器合称 eBPF runtime。)

In User Space(在用户空间中)
---------------------------

There are also implementations of eBPF runtime (interpreter, JITs, helper
functions) under
Apache2 (https://github.com/iovisor/ubpf),
MIT (https://github.com/qmonnet/rbpf), and
BSD (https://github.com/DPDK/dpdk/blob/main/lib/librte_bpf).
(在 Apache2 (https://github.com/iovisor/ubpf)、
MIT (https://github.com/qmonnet/rbpf) 和
BSD (https://github.com/DPDK/dpdk/blob/main/lib/librte_bpf) 下也有
eBPF runtime(解释器、JIT、辅助函数)的实现。)

In HW(在硬件中)
---------------

The HW can choose to execute eBPF instruction natively and provide eBPF runtime
in HW or via the use of implementing firmware with a proprietary license.
(硬件可以选择原生执行 eBPF 指令并在硬件中提供 eBPF runtime，或者通过
实现采用专有许可的固件来做到这一点。)

In other operating systems(在其他操作系统中)
--------------------------------------------

Other kernels or user space implementations of eBPF instruction set and runtime
can have proprietary licenses.
(其他内核或用户空间的 eBPF 指令集与 runtime 实现可以采用专有许可。)

Using BPF programs in the Linux kernel(在 Linux 内核中使用 BPF 程序)
====================================================================

Linux Kernel (while being GPLv2) allows linking of proprietary kernel modules
under these rules:
Documentation/process/license-rules.rst
(Linux 内核(本身是 GPLv2)在以下规则下允许链接专有内核模块：
Documentation/process/license-rules.rst)

When a kernel module is loaded, the linux kernel checks which functions it
intends to use. If any function is marked as "GPL only," the corresponding
module or program has to have GPL compatible license.
(加载内核模块时，Linux 内核会检查它打算使用哪些函数。如果有任何函数
被标记为 "GPL only"，则相应的模块或程序必须采用与 GPL 兼容的许可。)

Loading BPF program into the Linux kernel is similar to loading a kernel
module. BPF is loaded at run time and not statically linked to the Linux
kernel. BPF program loading follows the same license checking rules as kernel
modules. BPF programs can be proprietary if they don't use "GPL only" BPF
helper functions.
(把 BPF 程序加载到 Linux 内核类似于加载内核模块。BPF 在运行时加载，
不与 Linux 内核静态链接。BPF 程序加载遵循与内核模块相同的许可检查
规则。如果 BPF 程序不使用 "GPL only" 的 BPF 辅助函数，它可以采用
专有许可。)

Further, some BPF program types - Linux Security Modules (LSM) and TCP
Congestion Control (struct_ops), as of Aug 2021 - are required to be GPL
compatible even if they don't use "GPL only" helper functions directly. The
registration step of LSM and TCP congestion control modules of the Linux
kernel is done through EXPORT_SYMBOL_GPL kernel functions. In that sense LSM
and struct_ops BPF programs are implicitly calling "GPL only" functions.
The same restriction applies to BPF programs that call kernel functions
directly via unstable interface also known as "kfunc".
(此外，某些 BPF 程序类型——Linux 安全模块(LSM)和 TCP 拥塞控制
(struct_ops)，截至 2021 年 8 月——即使不直接使用 "GPL only" 辅助
函数，也要求与 GPL 兼容。Linux 内核的 LSM 和 TCP 拥塞控制模块的注册
步骤通过 EXPORT_SYMBOL_GPL 内核函数完成。从这个意义上说，LSM 和
struct_ops BPF 程序在隐式调用 "GPL only" 函数。同样的限制也适用于
通过不稳定接口(即所谓的 "kfunc")直接调用内核函数的 BPF 程序。)

Packaging BPF programs with user space applications(把 BPF 程序与用户空间应用一起打包)
======================================================================================

Generally, proprietary-licensed applications and GPL licensed BPF programs
written for the Linux kernel in the same package can co-exist because they are
separate executable processes. This applies to both cBPF and eBPF programs.
(一般而言，专有许可的应用程序与为 Linux 内核编写的采用 GPL 许可的
BPF 程序可以共存于同一个软件包中，因为它们是相互独立的可执行进程。
这同时适用于 cBPF 和 eBPF 程序。)
