.. SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)

===========================
BPF_PROG_TYPE_CGROUP_SYSCTL
===========================

This document describes ``BPF_PROG_TYPE_CGROUP_SYSCTL`` program type that
provides cgroup-bpf hook for sysctl.
(本文档描述 ``BPF_PROG_TYPE_CGROUP_SYSCTL`` 程序类型，它为 sysctl 提供 cgroup-bpf 钩子。)

The hook has to be attached to a cgroup and will be called every time a
process inside that cgroup tries to read from or write to sysctl knob in proc.
(该钩子必须挂接到一个 cgroup，并且每当该 cgroup 内的进程尝试读取或写入 proc 中的 sysctl 项时都会被调用。)

1. Attach type(1. 挂接类型)
***************************

``BPF_CGROUP_SYSCTL`` attach type has to be used to attach
``BPF_PROG_TYPE_CGROUP_SYSCTL`` program to a cgroup.
(必须使用 ``BPF_CGROUP_SYSCTL`` 挂接类型把 ``BPF_PROG_TYPE_CGROUP_SYSCTL`` 程序挂接到 cgroup。)

2. Context(2. 上下文)
*********************

``BPF_PROG_TYPE_CGROUP_SYSCTL`` provides access to the following context from
BPF program::

    struct bpf_sysctl {
        __u32 write;
        __u32 file_pos;
    };

(``BPF_PROG_TYPE_CGROUP_SYSCTL`` 使 BPF 程序可以访问以下上下文：)

* ``write`` indicates whether sysctl value is being read (``0``) or written
  (``1``). This field is read-only.
  (``write`` 表示 sysctl 值正在被读取（``0``）还是被写入（ ``1`` ）。该字段是只读的。)

* ``file_pos`` indicates file position sysctl is being accessed at, read
  or written. This field is read-write. Writing to the field sets the starting
  position in sysctl proc file ``read(2)`` will be reading from or ``write(2)``
  will be writing to. Writing zero to the field can be used e.g. to override
  whole sysctl value by ``bpf_sysctl_set_new_value()`` on ``write(2)`` even
  when it's called by user space on ``file_pos > 0``. Writing non-zero
  value to the field can be used to access part of sysctl value starting from
  specified ``file_pos``. Not all sysctl support access with ``file_pos !=
  0``, e.g. writes to numeric sysctl entries must always be at file position
  ``0``. See also ``kernel.sysctl_writes_strict`` sysctl.
  (``file_pos`` 表示正在访问 sysctl 的文件位置，读或写。该字段可读可写。向该字段写入将设置 sysctl proc 文件中 ``read(2)`` 将要开始读取的位置，或 ``write(2)`` 将要开始写入的位置。向该字段写入零可用于例如在 ``write(2)`` 时由 ``bpf_sysctl_set_new_value()`` 覆盖整个 sysctl 值，即使用户空间是在 ``file_pos > 0`` 时调用的。向该字段写入非零值可用于从指定的 ``file_pos`` 开始访问 sysctl 值的一部分。并非所有 sysctl 都支持以 ``file_pos != 0`` 访问，例如对数字型 sysctl 条目的写入必须始终在文件位置 ``0`` 处。另参见 ``kernel.sysctl_writes_strict`` sysctl。)

See `linux/bpf.h`_ for more details on how context field can be accessed.
(有关如何访问上下文字段的更多细节，参见 `linux/bpf.h`_ 。)

3. Return code(3. 返回码)
*************************

``BPF_PROG_TYPE_CGROUP_SYSCTL`` program must return one of the following
return codes:
(``BPF_PROG_TYPE_CGROUP_SYSCTL`` 程序必须返回以下返回码之一：)

* ``0`` means "reject access to sysctl";
  (``0`` 表示"拒绝访问 sysctl"；)
* ``1`` means "proceed with access".
  (``1`` 表示"继续访问"。)

If program returns ``0`` user space will get ``-1`` from ``read(2)`` or
``write(2)`` and ``errno`` will be set to ``EPERM``.
(如果程序返回 ``0`` ，用户空间将从 ``read(2)`` 或 ``write(2)`` 得到 ``-1`` ，并且 ``errno`` 将被设置为 ``EPERM`` 。)

4. Helpers(4. 辅助函数)
***********************

Since sysctl knob is represented by a name and a value, sysctl specific BPF
helpers focus on providing access to these properties:
(由于 sysctl 项由名称和值表示，sysctl 特定的 BPF 辅助函数专注于提供对这两个属性的访问：)

* ``bpf_sysctl_get_name()`` to get sysctl name as it is visible in
  ``/proc/sys`` into provided by BPF program buffer;
  (``bpf_sysctl_get_name()`` 用于把 ``/proc/sys`` 中可见的 sysctl 名称获取到 BPF 程序提供的缓冲区中；)

* ``bpf_sysctl_get_current_value()`` to get string value currently held by
  sysctl into provided by BPF program buffer. This helper is available on both
  ``read(2)`` from and ``write(2)`` to sysctl;
  (``bpf_sysctl_get_current_value()`` 用于把 sysctl 当前持有的字符串值获取到 BPF 程序提供的缓冲区中。该辅助函数在从 sysctl ``read(2)`` 和向 sysctl ``write(2)`` 时都可用；)

* ``bpf_sysctl_get_new_value()`` to get new string value currently being
  written to sysctl before actual write happens. This helper can be used only
  on ``ctx->write == 1``;
  (``bpf_sysctl_get_new_value()`` 用于在实际写入发生之前，获取当前正在写入 sysctl 的新字符串值。该辅助函数只能在 ``ctx->write == 1`` 时使用；)

* ``bpf_sysctl_set_new_value()`` to override new string value currently being
  written to sysctl before actual write happens. Sysctl value will be
  overridden starting from the current ``ctx->file_pos``. If the whole value
  has to be overridden BPF program can set ``file_pos`` to zero before calling
  to the helper. This helper can be used only on ``ctx->write == 1``. New
  string value set by the helper is treated and verified by kernel same way as
  an equivalent string passed by user space.
  (``bpf_sysctl_set_new_value()`` 用于在实际写入发生之前，覆盖当前正在写入 sysctl 的新字符串值。sysctl 值将从当前 ``ctx->file_pos`` 开始被覆盖。如果必须覆盖整个值，BPF 程序可以在调用该辅助函数之前把 ``file_pos`` 设为零。该辅助函数只能在 ``ctx->write == 1`` 时使用。由该辅助函数设置的新字符串值会被内核以与用户空间传入的等价字符串相同的方式对待和校验。)

BPF program sees sysctl value same way as user space does in proc filesystem,
i.e. as a string. Since many sysctl values represent an integer or a vector
of integers, the following helpers can be used to get numeric value from the
string:
(BPF 程序看待 sysctl 值的方式与用户空间在 proc 文件系统中相同，即视为字符串。由于许多 sysctl 值表示一个整数或整数向量，可以使用以下辅助函数从字符串中获取数值：)

* ``bpf_strtol()`` to convert initial part of the string to long integer
  similar to user space `strtol(3)`_;
  (``bpf_strtol()`` 把字符串的起始部分转换为 long 整数，类似于用户空间的 `strtol(3)`_ ；)
* ``bpf_strtoul()`` to convert initial part of the string to unsigned long
  integer similar to user space `strtoul(3)`_;
  (``bpf_strtoul()`` 把字符串的起始部分转换为 unsigned long 整数，类似于用户空间的 `strtoul(3)`_ ；)

See `linux/bpf.h`_ for more details on helpers described here.
(有关此处描述的辅助函数的更多细节，参见 `linux/bpf.h`_ 。)

5. Examples(5. 示例)
********************

See `test_sysctl_prog.c`_ for an example of BPF program in C that access
sysctl name and value, parses string value to get vector of integers and uses
the result to make decision whether to allow or deny access to sysctl.
(以 C 语言编写的、访问 sysctl 名称和值、把字符串值解析为整数向量并用结果决定是否允许访问 sysctl 的 BPF 程序示例参见 `test_sysctl_prog.c`_ 。)

6. Notes(6. 注意事项)
*********************

``BPF_PROG_TYPE_CGROUP_SYSCTL`` is intended to be used in **trusted** root
environment, for example to monitor sysctl usage or catch unreasonable values
an application, running as root in a separate cgroup, is trying to set.
(``BPF_PROG_TYPE_CGROUP_SYSCTL`` 旨在用于 **受信任的** root 环境，例如监控 sysctl 的使用，或捕获在单独 cgroup 中以 root 身份运行的应用程序试图设置的不合理值。)

Since `task_dfl_cgroup(current)` is called at `sys_read` / `sys_write` time it
may return results different from that at `sys_open` time, i.e. process that
opened sysctl file in proc filesystem may differ from process that is trying
to read from / write to it and two such processes may run in different
cgroups, what means ``BPF_PROG_TYPE_CGROUP_SYSCTL`` should not be used as a
security mechanism to limit sysctl usage.
(由于 `task_dfl_cgroup(current)` 是在 `sys_read` / `sys_write` 时调用的，它可能返回与 `sys_open` 时不同的结果，即打开 proc 文件系统中 sysctl 文件的进程可能与尝试读取/写入它的进程不同，并且这两个进程可能运行在不同的 cgroup 中。这意味着 ``BPF_PROG_TYPE_CGROUP_SYSCTL`` 不应被用作限制 sysctl 使用的安全机制。)

As with any cgroup-bpf program additional care should be taken if an
application running as root in a cgroup should not be allowed to
detach/replace BPF program attached by administrator.
(与任何 cgroup-bpf 程序一样，如果不允许在 cgroup 中以 root 身份运行的应用程序分离/替换管理员挂接的 BPF 程序，则需要额外小心。)

.. Links
.. _linux/bpf.h: ../../include/uapi/linux/bpf.h
.. _strtol(3): http://man7.org/linux/man-pages/man3/strtol.3p.html
.. _strtoul(3): http://man7.org/linux/man-pages/man3/strtoul.3p.html
.. _test_sysctl_prog.c:
   ../../tools/testing/selftests/bpf/progs/test_sysctl_prog.c
