.. SPDX-License-Identifier: GPL-2.0

=====================
The Devpts Filesystem
=====================

(devpts 文件系统)

Each mount of the devpts filesystem is now distinct such that ptys
and their indices allocated in one mount are independent from ptys
and their indices in all other mounts.
(现在 devpts 文件系统的每次挂载都是相互独立的，在一个挂载中分配的 pty 及其索引与所有其他挂载中的 pty 及其索引互不相干。)

All mounts of the devpts filesystem now create a ``/dev/pts/ptmx`` node
with permissions ``0000``.
(现在 devpts 文件系统的所有挂载都会创建一个权限为 ``0000`` 的 ``/dev/pts/ptmx`` 节点。)

To retain backwards compatibility the a ptmx device node (aka any node
created with ``mknod name c 5 2``) when opened will look for an instance
of devpts under the name ``pts`` in the same directory as the ptmx device
node.
(为了保持向后兼容，ptmx 设备节点(即任何用 ``mknod name c 5 2`` 创建的节点)在被打开时会在与 ptmx 设备节点相同的目录中查找名为 ``pts`` 的 devpts 实例。)

As an option instead of placing a ``/dev/ptmx`` device node at ``/dev/ptmx``
it is possible to place a symlink to ``/dev/pts/ptmx`` at ``/dev/ptmx`` or
to bind mount ``/dev/ptx/ptmx`` to ``/dev/ptmx``.  If you opt for using
the devpts filesystem in this manner devpts should be mounted with
the ``ptmxmode=0666``, or ``chmod 0666 /dev/pts/ptmx`` should be called.
(作为一种可选方案，可以不把 ``/dev/ptmx`` 设备节点放在 ``/dev/ptmx``，而是将指向 ``/dev/pts/ptmx`` 的符号链接放在 ``/dev/ptmx``，或者将 ``/dev/ptx/ptmx`` 绑定挂载到 ``/dev/ptmx``。如果选择以这种方式使用 devpts 文件系统，则 devpts 应使用 ``ptmxmode=0666`` 挂载，或者应调用 ``chmod 0666 /dev/pts/ptmx``。)

Total count of pty pairs in all instances is limited by sysctls::
(所有实例中 pty 对的总数由 sysctl 限制：)

    kernel.pty.max = 4096	- global limit
    kernel.pty.reserve = 1024	- reserved for filesystems mounted from the initial mount namespace
    kernel.pty.nr		- current count of ptys

Per-instance limit could be set by adding mount option ``max=<count>``.
(可以通过添加挂载选项 ``max=<count>`` 来设置每个实例的限制。)

This feature was added in kernel 3.4 together with
``sysctl kernel.pty.reserve``.
(该特性在内核 3.4 中与 ``sysctl kernel.pty.reserve`` 一起加入。)

In kernels older than 3.4 sysctl ``kernel.pty.max`` works as per-instance limit.
(在早于 3.4 的内核中，sysctl ``kernel.pty.max`` 充当每个实例的限制。)
