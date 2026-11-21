.. SPDX-License-Identifier: GPL-2.0

=====================================================
BPF_PROG_TYPE_CGROUP_SOCKOPT(cgroup sockopt 程序类型)
=====================================================

``BPF_PROG_TYPE_CGROUP_SOCKOPT`` program type can be attached to two
cgroup hooks:
( ``BPF_PROG_TYPE_CGROUP_SOCKOPT`` 程序类型可以附加到两个 cgroup 钩子上：)

* ``BPF_CGROUP_GETSOCKOPT`` - called every time process executes ``getsockopt``
  system call.
  ( ``BPF_CGROUP_GETSOCKOPT`` - 每当进程执行 ``getsockopt`` 系统调用时被调用。)
* ``BPF_CGROUP_SETSOCKOPT`` - called every time process executes ``setsockopt``
  system call.
  ( ``BPF_CGROUP_SETSOCKOPT`` - 每当进程执行 ``setsockopt`` 系统调用时被调用。)

The context (``struct bpf_sockopt``) has associated socket (``sk``) and
all input arguments: ``level``, ``optname``, ``optval`` and ``optlen``.
(上下文 (``struct bpf_sockopt``) 关联了套接字 (``sk``) 以及所有输入参数： ``level`` 、 ``optname`` 、 ``optval`` 和 ``optlen`` 。)

BPF_CGROUP_SETSOCKOPT(SETSOCKOPT 钩子)
======================================

``BPF_CGROUP_SETSOCKOPT`` is triggered *before* the kernel handling of
sockopt and it has writable context: it can modify the supplied arguments
before passing them down to the kernel. This hook has access to the cgroup
and socket local storage.
( ``BPF_CGROUP_SETSOCKOPT`` 在内核处理 sockopt *之前*触发，它具有可写的上下文：可以在把参数向下传递给内核之前修改它们。此钩子可以访问 cgroup 和套接字本地存储。)

If BPF program sets ``optlen`` to -1, the control will be returned
back to the userspace after all other BPF programs in the cgroup
chain finish (i.e. kernel ``setsockopt`` handling will *not* be executed).
(如果 BPF 程序把 ``optlen`` 设置为 -1，那么在 cgroup 链中的所有其他 BPF 程序执行完毕之后，控制权将返回给用户空间(即内核的 ``setsockopt`` 处理将*不会*被执行)。)

Note, that ``optlen`` can not be increased beyond the user-supplied
value. It can only be decreased or set to -1. Any other value will
trigger ``EFAULT``.
(注意， ``optlen`` 不能增加到超过用户提供的值。它只能被减小或设置为 -1。任何其他值都会触发 ``EFAULT`` 。)

Return Type(返回类型)
---------------------

* ``0`` - reject the syscall, ``EPERM`` will be returned to the userspace.
* ``1`` - success, continue with next BPF program in the cgroup chain.
  ( ``0`` - 拒绝该系统调用，将向用户空间返回 ``EPERM`` 。
  ``1`` - 成功，继续执行 cgroup 链中的下一个 BPF 程序。)

BPF_CGROUP_GETSOCKOPT(GETSOCKOPT 钩子)
======================================

``BPF_CGROUP_GETSOCKOPT`` is triggered *after* the kernel handing of
sockopt. The BPF hook can observe ``optval``, ``optlen`` and ``retval``
if it's interested in whatever kernel has returned. BPF hook can override
the values above, adjust ``optlen`` and reset ``retval`` to 0. If ``optlen``
has been increased above initial ``getsockopt`` value (i.e. userspace
buffer is too small), ``EFAULT`` is returned.
( ``BPF_CGROUP_GETSOCKOPT`` 在内核处理 sockopt *之后*触发。如果 BPF 钩子关心内核返回了什么，它可以观察 ``optval`` 、 ``optlen`` 和 ``retval`` 。BPF 钩子可以覆盖上述值：调整 ``optlen`` 并把 ``retval`` 重置为 0。如果 ``optlen`` 被增加到了超过初始 ``getsockopt`` 值(即用户空间缓冲区太小)，则返回 ``EFAULT`` 。)

This hook has access to the cgroup and socket local storage.
(此钩子可以访问 cgroup 和套接字本地存储。)

Note, that the only acceptable value to set to ``retval`` is 0 and the
original value that the kernel returned. Any other value will trigger
``EFAULT``.
(注意，可以设置给 ``retval`` 的唯一可接受的值是 0 和内核返回的原始值。任何其他值都会触发 ``EFAULT`` 。)

Return Type(返回类型)
---------------------

* ``0`` - reject the syscall, ``EPERM`` will be returned to the userspace.
* ``1`` - success: copy ``optval`` and ``optlen`` to userspace, return
  ``retval`` from the syscall (note that this can be overwritten by
  the BPF program from the parent cgroup).
  ( ``0`` - 拒绝该系统调用，将向用户空间返回 ``EPERM`` 。
  ``1`` - 成功：把 ``optval`` 和 ``optlen`` 复制到用户空间，并从系统调用返回 ``retval`` (注意，这可能会被来自父 cgroup 的 BPF 程序覆盖)。)

Cgroup Inheritance(cgroup 继承)
===============================

Suppose, there is the following cgroup hierarchy where each cgroup
has ``BPF_CGROUP_GETSOCKOPT`` attached at each level with
``BPF_F_ALLOW_MULTI`` flag::

  A (root, parent)
   \
    B (child)

(假设有如下 cgroup 层次结构，其中每个 cgroup 在每一层都附加了带有 ``BPF_F_ALLOW_MULTI`` 标志的 ``BPF_CGROUP_GETSOCKOPT`` ：)

When the application calls ``getsockopt`` syscall from the cgroup B,
the programs are executed from the bottom up: B, A. First program
(B) sees the result of kernel's ``getsockopt``. It can optionally
adjust ``optval``, ``optlen`` and reset ``retval`` to 0. After that
control will be passed to the second (A) program which will see the
same context as B including any potential modifications.
(当应用程序从 cgroup B 调用 ``getsockopt`` 系统调用时，程序自底向上执行：B、A。第一个程序(B)看到内核 ``getsockopt`` 的结果。它可以选择性地调整 ``optval`` 、 ``optlen`` 并把 ``retval`` 重置为 0。之后控制权将传递给第二个程序(A)，它将看到与 B 相同的上下文，包括任何潜在的修改。)

Same for ``BPF_CGROUP_SETSOCKOPT``: if the program is attached to
A and B, the trigger order is B, then A. If B does any changes
to the input arguments (``level``, ``optname``, ``optval``, ``optlen``),
then the next program in the chain (A) will see those changes,
*not* the original input ``setsockopt`` arguments. The potentially
modified values will be then passed down to the kernel.
( ``BPF_CGROUP_SETSOCKOPT`` 同理：如果程序附加到 A 和 B，触发顺序是先 B 后 A。如果 B 对输入参数( ``level`` 、 ``optname`` 、 ``optval`` 、 ``optlen`` )做了任何更改，那么链中的下一个程序(A)将看到这些更改，*而不是*原始的 ``setsockopt`` 输入参数。可能被修改的值随后将被向下传递给内核。)

Large optval(大的 optval)
=========================
When the ``optval`` is greater than the ``PAGE_SIZE``, the BPF program
can access only the first ``PAGE_SIZE`` of that data. So it has to options:
(当 ``optval`` 大于 ``PAGE_SIZE`` 时，BPF 程序只能访问该数据的前 ``PAGE_SIZE`` 字节。因此它有两个选择：)

* Set ``optlen`` to zero, which indicates that the kernel should
  use the original buffer from the userspace. Any modifications
  done by the BPF program to the ``optval`` are ignored.
  (把 ``optlen`` 设置为 0，表示内核应使用来自用户空间的原始缓冲区。
  BPF 程序对 ``optval`` 所做的任何修改都会被忽略。)
* Set ``optlen`` to the value less than ``PAGE_SIZE``, which
  indicates that the kernel should use BPF's trimmed ``optval``.
  (把 ``optlen`` 设置为小于 ``PAGE_SIZE`` 的值，表示内核应使用 BPF
  裁剪后的 ``optval`` 。)

When the BPF program returns with the ``optlen`` greater than
``PAGE_SIZE``, the userspace will receive original kernel
buffers without any modifications that the BPF program might have
applied.
(当 BPF 程序返回时 ``optlen`` 大于 ``PAGE_SIZE`` ，用户空间将收到原始的内核缓冲区，不带有 BPF 程序可能已应用的任何修改。)

Example(示例)
=============

Recommended way to handle BPF programs is as follows:
(处理 BPF 程序的推荐方式如下：)

.. code-block:: c

	SEC("cgroup/getsockopt")
	int getsockopt(struct bpf_sockopt *ctx)
	{
		/* Custom socket option. */
		if (ctx->level == MY_SOL && ctx->optname == MY_OPTNAME) {
			ctx->retval = 0;
			optval[0] = ...;
			ctx->optlen = 1;
			return 1;
		}

		/* Modify kernel's socket option. */
		if (ctx->level == SOL_IP && ctx->optname == IP_FREEBIND) {
			ctx->retval = 0;
			optval[0] = ...;
			ctx->optlen = 1;
			return 1;
		}

		/* optval larger than PAGE_SIZE use kernel's buffer. */
		if (ctx->optlen > PAGE_SIZE)
			ctx->optlen = 0;

		return 1;
	}

	SEC("cgroup/setsockopt")
	int setsockopt(struct bpf_sockopt *ctx)
	{
		/* Custom socket option. */
		if (ctx->level == MY_SOL && ctx->optname == MY_OPTNAME) {
			/* do something */
			ctx->optlen = -1;
			return 1;
		}

		/* Modify kernel's socket option. */
		if (ctx->level == SOL_IP && ctx->optname == IP_FREEBIND) {
			optval[0] = ...;
			return 1;
		}

		/* optval larger than PAGE_SIZE use kernel's buffer. */
		if (ctx->optlen > PAGE_SIZE)
			ctx->optlen = 0;

		return 1;
	}

See ``tools/testing/selftests/bpf/progs/sockopt_sk.c`` for an example
of BPF program that handles socket options.
(有关处理套接字选项的 BPF 程序示例，参见 ``tools/testing/selftests/bpf/progs/sockopt_sk.c`` 。)
