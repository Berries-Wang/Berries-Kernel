======================================
No New Privileges Flag(禁止新特权标志)
======================================

The execve system call can grant a newly-started program privileges that
its parent did not have.  The most obvious examples are setuid/setgid
programs and file capabilities.  To prevent the parent program from
gaining these privileges as well, the kernel and user code must be
careful to prevent the parent from doing anything that could subvert the
child.  For example:
(execve 系统调用可以授予新启动的程序其父进程所不具有的特权。最明显的例子
是 setuid/setgid 程序和文件能力 (file capabilities)。为防止父程序同样获得
这些特权，内核与用户态代码必须小心地阻止父进程做出任何可能暗中破坏子进程
的事情。例如：)

 - The dynamic loader handles ``LD_*`` environment variables differently if
   a program is setuid.
   (如果程序是 setuid 的，动态加载器会以不同方式处理 ``LD_*`` 环境变量。)

 - chroot is disallowed to unprivileged processes, since it would allow
   ``/etc/passwd`` to be replaced from the point of view of a process that
   inherited chroot.
   (chroot 对非特权进程是禁止的，因为从继承了 chroot 的进程的角度看，这会
   允许 ``/etc/passwd`` 被替换。)

 - The exec code has special handling for ptrace.
   (exec 代码对 ptrace 有特殊处理。)

These are all ad-hoc fixes.  The ``no_new_privs`` bit (since Linux 3.5) is a
new, generic mechanism to make it safe for a process to modify its
execution environment in a manner that persists across execve.  Any task
can set ``no_new_privs``.  Once the bit is set, it is inherited across fork,
clone, and execve and cannot be unset.  With ``no_new_privs`` set, ``execve()``
promises not to grant the privilege to do anything that could not have
been done without the execve call.  For example, the setuid and setgid
bits will no longer change the uid or gid; file capabilities will not
add to the permitted set, and LSMs will not relax constraints after
execve.
(这些都是临时性的修补。``no_new_privs`` 位 (自 Linux 3.5 起) 是一种新的、
通用的机制，使进程能够安全地以跨 execve 持续的方式修改自己的执行环境。
任何任务都可以设置 ``no_new_privs``。一旦该位被设置，它会跨越 fork、clone
和 execve 被继承，并且不能被清除。设置 ``no_new_privs`` 后，``execve()``
承诺不会授予任何没有 execve 调用就无法做到的事情的特权。例如，setuid 和
setgid 位将不再改变 uid 或 gid；文件能力不会再添加到许可集中；LSM 在
execve 之后也不会放宽约束。)

To set ``no_new_privs``, use::

    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);

(要设置 ``no_new_privs``，请使用：)

Be careful, though: LSMs might also not tighten constraints on exec
in ``no_new_privs`` mode.  (This means that setting up a general-purpose
service launcher to set ``no_new_privs`` before execing daemons may
interfere with LSM-based sandboxing.)
(不过要注意：在 ``no_new_privs`` 模式下，LSM 也可能同样不会收紧对 exec 的
约束。(这意味着设置一个通用服务启动器在 exec 守护进程之前设置
``no_new_privs``，可能会干扰基于 LSM 的沙箱。))

Note that ``no_new_privs`` does not prevent privilege changes that do not
involve ``execve()``.  An appropriately privileged task can still call
``setuid(2)`` and receive SCM_RIGHTS datagrams.
(注意 ``no_new_privs`` 并不阻止不涉及 ``execve()`` 的特权变更。拥有适当
特权的任务仍然可以调用 ``setuid(2)`` 并接收 SCM_RIGHTS 数据报。)

There are two main use cases for ``no_new_privs`` so far:
(到目前为止，``no_new_privs`` 有两个主要用例：)

 - Filters installed for the seccomp mode 2 sandbox persist across
   execve and can change the behavior of newly-executed programs.
   Unprivileged users are therefore only allowed to install such filters
   if ``no_new_privs`` is set.
   (为 seccomp 模式 2 沙箱安装的过滤器会跨 execve 持续存在，并可能改变新
   执行程序的行为。因此，只有设置了 ``no_new_privs``，才允许非特权用户
   安装此类过滤器。)

 - By itself, ``no_new_privs`` can be used to reduce the attack surface
   available to an unprivileged user.  If everything running with a
   given uid has ``no_new_privs`` set, then that uid will be unable to
   escalate its privileges by directly attacking setuid, setgid, and
   fcap-using binaries; it will need to compromise something without the
   ``no_new_privs`` bit set first.
   (就其本身而言，``no_new_privs`` 可用于缩小非特权用户可用的攻击面。如果
   以某个 uid 运行的所有程序都设置了 ``no_new_privs``，那么该 uid 将无法
   通过直接攻击 setuid、setgid 和使用文件能力的二进制程序来提权；它需要
   先攻破某个未设置 ``no_new_privs`` 位的东西。)

In the future, other potentially dangerous kernel features could become
available to unprivileged tasks if ``no_new_privs`` is set.  In principle,
several options to ``unshare(2)`` and ``clone(2)`` would be safe when
``no_new_privs`` is set, and ``no_new_privs`` + ``chroot`` is considerable less
dangerous than chroot by itself.
(将来，如果设置了 ``no_new_privs``，其他潜在危险的内核特性也可能变得对
非特权任务可用。原则上，设置 ``no_new_privs`` 后，``unshare(2)`` 和
``clone(2)`` 的若干选项会是安全的，而且 ``no_new_privs`` + ``chroot`` 比
单独使用 chroot 危险性小得多。)
