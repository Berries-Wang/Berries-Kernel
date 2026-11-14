.. SPDX-License-Identifier: GPL-2.0

================
FUSE Passthrough
================

(FUSE 直通(passthrough))

Introduction
(简介)
============

FUSE (Filesystem in Userspace) passthrough is a feature designed to improve the
performance of FUSE filesystems for I/O operations. Typically, FUSE operations
involve communication between the kernel and a userspace FUSE daemon, which can
incur overhead. Passthrough allows certain operations on a FUSE file to bypass
the userspace daemon and be executed directly by the kernel on an underlying
"backing file".
(FUSE (Filesystem in Userspace)直通是一项旨在提升 FUSE 文件系统 I/O 操作性能的特性。通常，FUSE 操作涉及内核与用户空间 FUSE 守护进程之间的通信，这可能带来额外开销。直通允许对 FUSE 文件的某些操作绕过用户空间守护进程，改由内核直接在底层的“后备文件(backing file)”上执行。)

This is achieved by the FUSE daemon registering a file descriptor (pointing to
the backing file on a lower filesystem) with the FUSE kernel module. The kernel
then receives an identifier (``backing_id``) for this registered backing file.
When a FUSE file is subsequently opened, the FUSE daemon can, in its response to
the ``OPEN`` request, include this ``backing_id`` and set the
``FOPEN_PASSTHROUGH`` flag. This establishes a direct link for specific
operations.
(这是通过 FUSE 守护进程向 FUSE 内核模块注册一个文件描述符(指向更低层文件系统上的后备文件)来实现的。内核随后会收到这个已注册后备文件的标识符(``backing_id``)。之后当某个 FUSE 文件被打开时，FUSE 守护进程可以在其对 ``OPEN`` 请求的应答中包含该 ``backing_id``，并设置 ``FOPEN_PASSTHROUGH`` 标志。这会为特定操作建立起一条直接链路。)

Currently, passthrough is supported for operations like ``read(2)``/``write(2)``
(via ``read_iter``/``write_iter``), ``splice(2)``, and ``mmap(2)``.
(目前，直通支持的操作包括 ``read(2)``/``write(2)``(经由 ``read_iter``/``write_iter``)、``splice(2)`` 和 ``mmap(2)``。)

Enabling Passthrough
(启用直通)
====================

To use FUSE passthrough:
(要使用 FUSE 直通：)

  1. The FUSE filesystem must be compiled with ``CONFIG_FUSE_PASSTHROUGH``
     enabled.
     (FUSE 文件系统必须在编译时启用 ``CONFIG_FUSE_PASSTHROUGH``。)
  2. The FUSE daemon, during the ``FUSE_INIT`` handshake, must negotiate the
     ``FUSE_PASSTHROUGH`` capability and specify its desired
     ``max_stack_depth``.
     (FUSE 守护进程必须在 ``FUSE_INIT`` 握手期间协商 ``FUSE_PASSTHROUGH`` 能力，并指定其期望的 ``max_stack_depth``。)
  3. The (privileged) FUSE daemon uses the ``FUSE_DEV_IOC_BACKING_OPEN`` ioctl
     on its connection file descriptor (e.g., ``/dev/fuse``) to register a
     backing file descriptor and obtain a ``backing_id``.
     (具特权的 FUSE 守护进程在其连接文件描述符(例如 ``/dev/fuse``)上使用 ``FUSE_DEV_IOC_BACKING_OPEN`` ioctl 来注册一个后备文件描述符，并获得一个 ``backing_id``。)
  4. When handling an ``OPEN`` or ``CREATE`` request for a FUSE file, the daemon
     replies with the ``FOPEN_PASSTHROUGH`` flag set in
     ``fuse_open_out::open_flags`` and provides the corresponding ``backing_id``
     in ``fuse_open_out::backing_id``.
     (在处理针对某个 FUSE 文件的 ``OPEN`` 或 ``CREATE`` 请求时，守护进程在 ``fuse_open_out::open_flags`` 中设置 ``FOPEN_PASSTHROUGH`` 标志作为应答，并在 ``fuse_open_out::backing_id`` 中提供相应的 ``backing_id``。)
  5. The FUSE daemon should eventually call ``FUSE_DEV_IOC_BACKING_CLOSE`` with
     the ``backing_id`` to release the kernel's reference to the backing file
     when it's no longer needed for passthrough setups.
     (当后备文件不再用于直通设置时，FUSE 守护进程最终应携带 ``backing_id`` 调用 ``FUSE_DEV_IOC_BACKING_CLOSE``，以释放内核对该后备文件的引用。)

Privilege Requirements
(特权要求)
======================

Setting up passthrough functionality currently requires the FUSE daemon to
possess the ``CAP_SYS_ADMIN`` capability. This requirement stems from several
security and resource management considerations that are actively being
discussed and worked on. The primary reasons for this restriction are detailed
below.
(设置直通功能目前要求 FUSE 守护进程拥有 ``CAP_SYS_ADMIN`` 能力。这一要求源于若干正在被积极讨论和处理的安全与资源管理方面的考量。该限制的主要原因详见下文。)

Resource Accounting and Visibility
(资源核算与可见性)
----------------------------------

The core mechanism for passthrough involves the FUSE daemon opening a file
descriptor to a backing file and registering it with the FUSE kernel module via
the ``FUSE_DEV_IOC_BACKING_OPEN`` ioctl. This ioctl returns a ``backing_id``
associated with a kernel-internal ``struct fuse_backing`` object, which holds a
reference to the backing ``struct file``.
(直通的核心机制是：FUSE 守护进程打开一个指向后备文件的文件描述符，并通过 ``FUSE_DEV_IOC_BACKING_OPEN`` ioctl 将其注册到 FUSE 内核模块。该 ioctl 返回一个 ``backing_id``，它与内核内部的 ``struct fuse_backing`` 对象相关联，而该对象持有对后备 ``struct file`` 的引用。)

A significant concern arises because the FUSE daemon can close its own file
descriptor to the backing file after registration. The kernel, however, will
still hold a reference to the ``struct file`` via the ``struct fuse_backing``
object as long as it's associated with a ``backing_id`` (or subsequently, with
an open FUSE file in passthrough mode).
(一个值得注意的问题是，FUSE 守护进程可以在注册之后关闭自己指向后备文件的文件描述符。然而，只要后备文件仍与某个 ``backing_id``(或随后与某个以直通模式打开的 FUSE 文件)相关联，内核就会通过 ``struct fuse_backing`` 对象继续持有对 ``struct file`` 的引用。)

This behavior leads to two main issues for unprivileged FUSE daemons:
(这种行为会给非特权的 FUSE 守护进程带来两个主要问题：)

  1. **Invisibility to lsof and other inspection tools**: Once the FUSE
     daemon closes its file descriptor, the open backing file held by the kernel
     becomes "hidden." Standard tools like ``lsof``, which typically inspect
     process file descriptor tables, would not be able to identify that this
     file is still open by the system on behalf of the FUSE filesystem. This
     makes it difficult for system administrators to track resource usage or
     debug issues related to open files (e.g., preventing unmounts).
     (**对 lsof 及其他检查工具不可见**：一旦 FUSE 守护进程关闭了自己的文件描述符，由内核持有的已打开后备文件就会变得“隐藏”。像 ``lsof`` 这类通常检查进程文件描述符表的标准工具，将无法识别出该文件仍由系统代表 FUSE 文件系统保持打开。这使系统管理员难以跟踪资源使用情况，也难以调试与打开文件相关的问题(例如阻止卸载)。)

  2. **Bypassing RLIMIT_NOFILE**: The FUSE daemon process is subject to
     resource limits, including the maximum number of open file descriptors
     (``RLIMIT_NOFILE``). If an unprivileged daemon could register backing files
     and then close its own FDs, it could potentially cause the kernel to hold
     an unlimited number of open ``struct file`` references without these being
     accounted against the daemon's ``RLIMIT_NOFILE``. This could lead to a
     denial-of-service (DoS) by exhausting system-wide file resources.
     (**绕过 RLIMIT_NOFILE**：FUSE 守护进程作为一个进程也受资源限制的约束，其中包括可打开文件描述符的最大数量(``RLIMIT_NOFILE``)。如果非特权守护进程可以注册后备文件然后关闭自己的 FD，它就可能使内核持有无限数量的已打开 ``struct file`` 引用，而这些引用并不计入该守护进程的 ``RLIMIT_NOFILE``。这可能通过耗尽系统级文件资源而导致拒绝服务(DoS)。)

The ``CAP_SYS_ADMIN`` requirement acts as a safeguard against these issues,
restricting this powerful capability to trusted processes.
(``CAP_SYS_ADMIN`` 要求充当了针对这些问题的防护措施，将这一强大的能力限制给受信任的进程。)

**NOTE**: ``io_uring`` solves this similar issue by exposing its "fixed files",
which are visible via ``fdinfo`` and accounted under the registering user's
``RLIMIT_NOFILE``.
(**注意**：``io_uring`` 通过暴露其“固定文件(fixed files)”解决了类似的问题，这些文件可通过 ``fdinfo`` 查看，并计入注册用户的 ``RLIMIT_NOFILE``。)

Filesystem Stacking and Shutdown Loops
(文件系统堆叠与关闭循环)
--------------------------------------

Another concern relates to the potential for creating complex and problematic
filesystem stacking scenarios if unprivileged users could set up passthrough.
A FUSE passthrough filesystem might use a backing file that resides:
(另一个担忧与潜在的复杂且有问题的文件系统堆叠场景有关——如果非特权用户能够设置直通的话。FUSE 直通文件系统可能使用一个位于以下位置的后备文件：)

  * On the *same* FUSE filesystem.
    (位于*同一个* FUSE 文件系统上。)
  * On another filesystem (like OverlayFS) which itself might have an upper or
    lower layer that is a FUSE filesystem.
    (位于另一个文件系统上(例如 OverlayFS)，而该文件系统自身的上层或下层可能就是一个 FUSE 文件系统。)

These configurations could create dependency loops, particularly during
filesystem shutdown or unmount sequences, leading to deadlocks or system
instability. This is conceptually similar to the risks associated with the
``LOOP_SET_FD`` ioctl, which also requires ``CAP_SYS_ADMIN``.
(这些配置可能造成依赖循环，尤其是在文件系统关闭或卸载序列期间，从而导致死锁或系统不稳定。这在概念上类似于与 ``LOOP_SET_FD`` ioctl 相关的风险，后者同样要求 ``CAP_SYS_ADMIN``。)

To mitigate this, FUSE passthrough already incorporates checks based on
filesystem stacking depth (``sb->s_stack_depth`` and ``fc->max_stack_depth``).
For example, during the ``FUSE_INIT`` handshake, the FUSE daemon can negotiate
the ``max_stack_depth`` it supports. When a backing file is registered via
``FUSE_DEV_IOC_BACKING_OPEN``, the kernel checks if the backing file's
filesystem stack depth is within the allowed limit.
(为缓解这一问题，FUSE 直通已经内置了基于文件系统堆叠深度的检查(``sb->s_stack_depth`` 与 ``fc->max_stack_depth``)。例如，在 ``FUSE_INIT`` 握手期间，FUSE 守护进程可以协商其支持的 ``max_stack_depth``。当通过 ``FUSE_DEV_IOC_BACKING_OPEN`` 注册后备文件时，内核会检查后备文件所在文件系统的堆叠深度是否在允许的限制之内。)

The ``CAP_SYS_ADMIN`` requirement provides an additional layer of security,
ensuring that only privileged users can create these potentially complex
stacking arrangements.
(``CAP_SYS_ADMIN`` 要求提供了额外一层安全保障，确保只有特权用户才能创建这些可能很复杂的堆叠安排。)

General Security Posture
(总体安全姿态)
------------------------

As a general principle for new kernel features that allow userspace to instruct
the kernel to perform direct operations on its behalf based on user-provided
file descriptors, starting with a higher privilege requirement (like
``CAP_SYS_ADMIN``) is a conservative and common security practice. This allows
the feature to be used and tested while further security implications are
evaluated and addressed.
(作为一项通用原则，对于允许用户空间指示内核基于用户提供的文件描述符代为执行直接操作的新内核特性而言，从较高的特权要求(例如 ``CAP_SYS_ADMIN``)起步是一种保守且常见的安全做法。这样可以在进一步评估和处理安全影响的同时，对该特性加以使用和测试。)
