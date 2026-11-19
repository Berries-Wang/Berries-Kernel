
.. _addsyscalls:

Adding a New System Call(添加新系统调用)
========================================

This document describes what's involved in adding a new system call to the
Linux kernel, over and above the normal submission advice in
:ref:`Documentation/process/submitting-patches.rst <submittingpatches>`.
(本文描述在 :ref:`Documentation/process/submitting-patches.rst <submittingpatches>` 的常规提交建议之外，为 Linux 内核添加一个新系统调用还需要做哪些事情。)


System Call Alternatives(系统调用的替代方案)
--------------------------------------------

The first thing to consider when adding a new system call is whether one of
the alternatives might be suitable instead.  Although system calls are the
most traditional and most obvious interaction points between userspace and the
kernel, there are other possibilities -- choose what fits best for your
interface.
(添加新系统调用时首先要考虑的问题：某个替代方案是否可能更合适。尽管系统调用是用户态与内核之间最传统、最显见的交互点，但也存在其他可能性——请选择最适合你的接口的方式。)

 - If the operations involved can be made to look like a filesystem-like
   object, it may make more sense to create a new filesystem or device.  This
   also makes it easier to encapsulate the new functionality in a kernel module
   rather than requiring it to be built into the main kernel.
   (如果相关操作可以设计成类似文件系统的对象，那么创建新的文件系统或设备可能更合理。这也便于把新功能封装进内核模块，而不必编译进主内核。)

     - If the new functionality involves operations where the kernel notifies
       userspace that something has happened, then returning a new file
       descriptor for the relevant object allows userspace to use
       ``poll``/``select``/``epoll`` to receive that notification.
       (如果新功能涉及内核通知用户态"某件事发生了"的操作，那么为相关对象返回一个新的文件描述符，用户态就可以用 ``poll``/``select``/``epoll`` 接收该通知。)
     - However, operations that don't map to
       :manpage:`read(2)`/:manpage:`write(2)`-like operations
       have to be implemented as :manpage:`ioctl(2)` requests, which can lead
       to a somewhat opaque API.
       (不过，无法映射为 :manpage:`read(2)`/:manpage:`write(2)` 类操作的部分只能实现为 :manpage:`ioctl(2)` 请求，这可能导致 API 相当晦涩。)

 - If you're just exposing runtime system information, a new node in sysfs
   (see ``Documentation/filesystems/sysfs.rst``) or the ``/proc`` filesystem may
   be more appropriate.  However, access to these mechanisms requires that the
   relevant filesystem is mounted, which might not always be the case (e.g.
   in a namespaced/sandboxed/chrooted environment).  Avoid adding any API to
   debugfs, as this is not considered a 'production' interface to userspace.
   (如果只是暴露运行时系统信息，那么在 sysfs 中新增节点 (见 ``Documentation/filesystems/sysfs.rst``) 或使用 ``/proc`` 文件系统可能更合适。不过访问这些机制要求相应文件系统已挂载，而这并非总是成立 (例如在 namespace/沙盒/chroot 环境中)。不要向 debugfs 添加任何 API，因为它不被视为面向用户态的"生产"接口。)
 - If the operation is specific to a particular file or file descriptor, then
   an additional :manpage:`fcntl(2)` command option may be more appropriate.  However,
   :manpage:`fcntl(2)` is a multiplexing system call that hides a lot of complexity, so
   this option is best for when the new function is closely analogous to
   existing :manpage:`fcntl(2)` functionality, or the new functionality is very simple
   (for example, getting/setting a simple flag related to a file descriptor).
   (如果操作针对特定文件或文件描述符，那么为 :manpage:`fcntl(2)` 增加一个命令选项可能更合适。不过 :manpage:`fcntl(2)` 是一个隐藏了大量复杂性的多路复用系统调用，因此这种方式最适合新功能与既有 :manpage:`fcntl(2)` 功能高度类似、或新功能非常简单的情况 (例如获取/设置一个与文件描述符相关的简单标志)。)
 - If the operation is specific to a particular task or process, then an
   additional :manpage:`prctl(2)` command option may be more appropriate.  As
   with :manpage:`fcntl(2)`, this system call is a complicated multiplexor so
   is best reserved for near-analogs of existing ``prctl()`` commands or
   getting/setting a simple flag related to a process.
   (如果操作针对特定任务或进程，那么为 :manpage:`prctl(2)` 增加一个命令选项可能更合适。与 :manpage:`fcntl(2)` 一样，该系统调用是一个复杂的多路复用器，因此最好只用于与既有 ``prctl()`` 命令近似的功能，或获取/设置一个与进程相关的简单标志。)


Designing the API: Planning for Extension(设计 API：为扩展做规划)
-----------------------------------------------------------------

A new system call forms part of the API of the kernel, and has to be supported
indefinitely.  As such, it's a very good idea to explicitly discuss the
interface on the kernel mailing list, and it's important to plan for future
extensions of the interface.
(新系统调用是内核 API 的一部分，必须被无限期地支持。因此，非常值得在内核邮件列表上明确讨论这个接口，并且要为接口的未来扩展做好规划。)

(The syscall table is littered with historical examples where this wasn't done,
together with the corresponding follow-up system calls --
``eventfd``/``eventfd2``, ``dup2``/``dup3``, ``inotify_init``/``inotify_init1``,
``pipe``/``pipe2``, ``renameat``/``renameat2`` -- so
learn from the history of the kernel and plan for extensions from the start.)
(系统调用表里散落着大量没有这样做的历史案例，以及相应的后续补救系统调用——``eventfd``/``eventfd2``、``dup2``/``dup3``、``inotify_init``/``inotify_init1``、``pipe``/``pipe2``、``renameat``/``renameat2``——所以要吸取内核的历史教训，从一开始就为扩展做好规划。)

For simpler system calls that only take a couple of arguments, the preferred
way to allow for future extensibility is to include a flags argument to the
system call.  To make sure that userspace programs can safely use flags
between kernel versions, check whether the flags value holds any unknown
flags, and reject the system call (with ``EINVAL``) if it does::

    if (flags & ~(THING_FLAG1 | THING_FLAG2 | THING_FLAG3))
        return -EINVAL;

(对于只接受两三个参数的较简单系统调用，预留未来扩展能力的首选方式是在系统调用中包含一个 flags 参数。为确保用户态程序可以在不同内核版本之间安全地使用 flags，应检查 flags 值中是否含有未知标志，若有则拒绝该系统调用 (返回 ``EINVAL``)。)

(If no flags values are used yet, check that the flags argument is zero.)
(如果尚未使用任何 flags 值，则检查 flags 参数是否为零。)

For more sophisticated system calls that involve a larger number of arguments,
it's preferred to encapsulate the majority of the arguments into a structure
that is passed in by pointer.  Such a structure can cope with future extension
by including a size argument in the structure::

    struct xyzzy_params {
        u32 size; /* userspace sets p->size = sizeof(struct xyzzy_params) */
        u32 param_1;
        u64 param_2;
        u64 param_3;
    };

(对于涉及更多参数的较复杂系统调用，首选方式是把大部分参数封装到一个通过指针传入的结构体中。这样的结构体可以在结构体内包含一个 size 参数，以应对未来扩展：)

As long as any subsequently added field, say ``param_4``, is designed so that a
zero value gives the previous behaviour, then this allows both directions of
version mismatch:
(只要后续新增的字段 (比如 ``param_4``) 设计成零值即保持原有行为，这种方式就能同时应对两个方向的版本失配：)

 - To cope with a later userspace program calling an older kernel, the kernel
   code should check that any memory beyond the size of the structure that it
   expects is zero (effectively checking that ``param_4 == 0``).
   (为了应对较新的用户态程序调用较旧内核：内核代码应检查其预期结构体大小之外的内存是否为零 (即等效于检查 ``param_4 == 0``)。)
 - To cope with an older userspace program calling a newer kernel, the kernel
   code can zero-extend a smaller instance of the structure (effectively
   setting ``param_4 = 0``).
   (为了应对较旧的用户态程序调用较新内核：内核代码可以对较小的结构体实例做零扩展 (即等效于设置 ``param_4 = 0``)。)

See :manpage:`perf_event_open(2)` and the ``perf_copy_attr()`` function (in
``kernel/events/core.c``) for an example of this approach.
(这种做法的示例见 :manpage:`perf_event_open(2)` 手册以及 ``perf_copy_attr()`` 函数 (位于 ``kernel/events/core.c``)。)


Designing the API: Other Considerations(设计 API：其他考虑事项)
---------------------------------------------------------------

If your new system call allows userspace to refer to a kernel object, it
should use a file descriptor as the handle for that object -- don't invent a
new type of userspace object handle when the kernel already has mechanisms and
well-defined semantics for using file descriptors.
(如果你的新系统调用允许用户态引用某个内核对象，应当使用文件描述符作为该对象的句柄——在内核已有使用文件描述符的机制和明确定义的语义的情况下，不要另发明一种用户态对象句柄。)

If your new xyzzy(2) system call does return a new file descriptor,
then the flags argument should include a value that is equivalent to setting
``O_CLOEXEC`` on the new FD.  This makes it possible for userspace to close
the timing window between ``xyzzy()`` and calling
``fcntl(fd, F_SETFD, FD_CLOEXEC)``, where an unexpected ``fork()`` and
``execve()`` in another thread could leak a descriptor to
the exec'ed program. (However, resist the temptation to reuse the actual value
of the ``O_CLOEXEC`` constant, as it is architecture-specific and is part of a
numbering space of ``O_*`` flags that is fairly full.)
(如果你的新 xyzzy(2) 系统调用确实会返回新的文件描述符，那么 flags 参数中应包含一个等效于对新 FD 设置 ``O_CLOEXEC`` 的值。这样用户态就能消除 ``xyzzy()`` 与调用 ``fcntl(fd, F_SETFD, FD_CLOEXEC)`` 之间的时间窗口——在这个窗口里，另一线程上意外的 ``fork()`` 和 ``execve()`` 可能把描述符泄漏给被 exec 的程序。不过要克制复用 ``O_CLOEXEC`` 常量实际值的诱惑，因为它是架构相关的，并且属于一个已经相当拥挤的 ``O_*`` 标志编号空间。)

If your system call returns a new file descriptor, you should also consider
what it means to use the :manpage:`poll(2)` family of system calls on that file
descriptor. Making a file descriptor ready for reading or writing is the
normal way for the kernel to indicate to userspace that an event has
occurred on the corresponding kernel object.
(如果你的系统调用返回新的文件描述符，还应当考虑对它使用 :manpage:`poll(2)` 系统调用族意味着什么。让文件描述符变为可读或可写，是内核向用户态表明对应内核对象上发生了事件的标准方式。)

If your new xyzzy(2) system call involves a filename argument::

    int sys_xyzzy(const char __user *path, ..., unsigned int flags);

(如果新的 xyzzy(2) 系统调用带有文件名参数：)

you should also consider whether an xyzzyat(2) version is more appropriate::

    int sys_xyzzyat(int dfd, const char __user *path, ..., unsigned int flags);

(你还应当考虑 xyzzyat(2) 版本是否更合适：)

This allows more flexibility for how userspace specifies the file in question;
in particular it allows userspace to request the functionality for an
already-opened file descriptor using the ``AT_EMPTY_PATH`` flag, effectively
giving an fxyzzy(3) operation for free::

 - xyzzyat(AT_FDCWD, path, ..., 0) is equivalent to xyzzy(path,...)
 - xyzzyat(fd, "", ..., AT_EMPTY_PATH) is equivalent to fxyzzy(fd, ...)

(这让用户态在指定目标文件时有更大的灵活性；特别是允许用户态借助 ``AT_EMPTY_PATH`` 标志对已打开的文件描述符请求该功能，等于免费获得一个 fxyzzy(3) 操作：)

(For more details on the rationale of the \*at() calls, see the
:manpage:`openat(2)` man page; for an example of AT_EMPTY_PATH, see the
:manpage:`fstatat(2)` man page.)
(关于 \*at() 系列调用设计理由的更多细节见 :manpage:`openat(2)` 手册页；AT_EMPTY_PATH 的示例见 :manpage:`fstatat(2)` 手册页。)

If your new xyzzy(2) system call involves a parameter describing an
offset within a file, make its type ``loff_t`` so that 64-bit offsets can be
supported even on 32-bit architectures.
(如果新的 xyzzy(2) 系统调用带有描述文件内偏移量的参数，应把其类型定为 ``loff_t``，这样即使在 32 位架构上也能支持 64 位偏移。)

If your new xyzzy(2) system call involves privileged functionality,
it needs to be governed by the appropriate Linux capability bit (checked with
a call to ``capable()``), as described in the :manpage:`capabilities(7)` man
page.  Choose an existing capability bit that governs related functionality,
but try to avoid combining lots of only vaguely related functions together
under the same bit, as this goes against capabilities' purpose of splitting
the power of root.  In particular, avoid adding new uses of the already
overly-general ``CAP_SYS_ADMIN`` capability.
(如果新的 xyzzy(2) 系统调用涉及特权功能，它需要由适当的 Linux capability 位约束 (通过调用 ``capable()`` 检查)，如 :manpage:`capabilities(7)` 手册页所述。应选择一个既有的、管辖相关功能的 capability 位，但尽量避免把大量仅勉强相关的功能塞进同一个位，因为这违背了 capabilities 拆分 root 权力的初衷。特别要避免给已经过于宽泛的 ``CAP_SYS_ADMIN`` capability 增加新用途。)

If your new xyzzy(2) system call manipulates a process other than
the calling process, it should be restricted (using a call to
``ptrace_may_access()``) so that only a calling process with the same
permissions as the target process, or with the necessary capabilities, can
manipulate the target process.
(如果新的 xyzzy(2) 系统调用会操作调用进程之外的其他进程，应加以限制 (通过调用 ``ptrace_may_access()``)，使得只有与目标进程权限相同的调用进程、或具备必要 capabilities 的调用进程才能操作目标进程。)

Finally, be aware that some non-x86 architectures have an easier time if
system call parameters that are explicitly 64-bit fall on odd-numbered
arguments (i.e. parameter 1, 3, 5), to allow use of contiguous pairs of 32-bit
registers.  (This concern does not apply if the arguments are part of a
structure that's passed in by pointer.)
(最后要注意，某些非 x86 架构在显式 64 位的系统调用参数落在奇数序号参数 (即参数 1、3、5) 上时会更容易处理，以便使用相邻的 32 位寄存器对。如果参数是通过指针传入的结构体的一部分，则不存在这个顾虑。)


Proposing the API(提出 API)
---------------------------

To make new system calls easy to review, it's best to divide up the patchset
into separate chunks.  These should include at least the following items as
distinct commits (each of which is described further below):
(为了让新系统调用便于评审，最好把补丁集拆分成独立的块。至少应把以下条目作为独立的 commit (下文会逐一详述)：)

 - The core implementation of the system call, together with prototypes,
   generic numbering, Kconfig changes and fallback stub implementation.
   (系统调用的核心实现，连同原型、通用编号、Kconfig 修改和后备 stub 实现。)
 - Wiring up of the new system call for one particular architecture, usually
   x86 (including all of x86_64, x86_32 and x32).
   (为某个特定架构接上新系统调用，通常是 x86 (包括 x86_64、x86_32 和 x32 全部)。)
 - A demonstration of the use of the new system call in userspace via a
   selftest in ``tools/testing/selftests/``.
   (通过 ``tools/testing/selftests/`` 中的 selftest 演示用户态如何使用新系统调用。)
 - A draft man-page for the new system call, either as plain text in the
   cover letter, or as a patch to the (separate) man-pages repository.
   (新系统调用的 man 手册页草稿，可以作为纯文本放在封面信 (cover letter) 中，或作为对 (独立的) man-pages 仓库的补丁。)

New system call proposals, like any change to the kernel's API, should always
be cc'ed to linux-api@vger.kernel.org.
(与对内核 API 的任何修改一样，新系统调用提案应当始终抄送 linux-api@vger.kernel.org。)


Generic System Call Implementation(通用系统调用实现)
----------------------------------------------------

The main entry point for your new xyzzy(2) system call will be called
``sys_xyzzy()``, but you add this entry point with the appropriate
``SYSCALL_DEFINEn()`` macro rather than explicitly.  The 'n' indicates the
number of arguments to the system call, and the macro takes the system call name
followed by the (type, name) pairs for the parameters as arguments.  Using
this macro allows metadata about the new system call to be made available for
other tools.
(新 xyzzy(2) 系统调用的主入口点将被称为 ``sys_xyzzy()``，但应使用相应的 ``SYSCALL_DEFINEn()`` 宏来添加这个入口点，而不是显式声明。'n' 表示系统调用的参数个数，宏的参数依次为系统调用名以及各参数的 (类型, 名称) 对。使用该宏可以让关于新系统调用的元数据供其他工具使用。)

The new entry point also needs a corresponding function prototype, in
``include/linux/syscalls.h``, marked as asmlinkage to match the way that system
calls are invoked::

    asmlinkage long sys_xyzzy(...);

(新的入口点还需要在 ``include/linux/syscalls.h`` 中有对应的函数原型，并标记为 asmlinkage 以匹配系统调用的调用方式：)

Some architectures (e.g. x86) have their own architecture-specific syscall
tables, but several other architectures share a generic syscall table. Add your
new system call to the generic list by adding an entry to the list in
``include/uapi/asm-generic/unistd.h``::

    #define __NR_xyzzy 292
    __SYSCALL(__NR_xyzzy, sys_xyzzy)

(某些架构 (例如 x86) 有自己的架构专用系统调用表，但另外一些架构共享一张通用系统调用表。要在 ``include/uapi/asm-generic/unistd.h`` 的列表中添加一个条目，把新系统调用加入通用列表：)

Also update the __NR_syscalls count to reflect the additional system call, and
note that if multiple new system calls are added in the same merge window,
your new syscall number may get adjusted to resolve conflicts.
(同时更新 __NR_syscalls 计数以反映新增的系统调用；还要注意，如果同一个合并窗口中添加了多个新系统调用，你的新系统调用编号可能会为解决冲突而被调整。)

The file ``kernel/sys_ni.c`` provides a fallback stub implementation of each
system call, returning ``-ENOSYS``.  Add your new system call here too::

    COND_SYSCALL(xyzzy);

(文件 ``kernel/sys_ni.c`` 为每个系统调用提供了返回 ``-ENOSYS`` 的后备 stub 实现。也要把新系统调用加到这里：)

Your new kernel functionality, and the system call that controls it, should
normally be optional, so add a ``CONFIG`` option (typically to
``init/Kconfig``) for it. As usual for new ``CONFIG`` options:
(新的内核功能及其控制系统调用通常应是可选的，所以要为它添加一个 ``CONFIG`` 选项 (通常加在 ``init/Kconfig``)。按照新 ``CONFIG`` 选项的惯例：)

 - Include a description of the new functionality and system call controlled
   by the option.
   (为该选项所控制的新功能和系统调用写一段描述。)
 - Make the option depend on EXPERT if it should be hidden from normal users.
   (如果该选项应对普通用户隐藏，就让它依赖 EXPERT。)
 - Make any new source files implementing the function dependent on the CONFIG
   option in the Makefile (e.g. ``obj-$(CONFIG_XYZZY_SYSCALL) += xyzzy.o``).
   (在 Makefile 中让实现该功能的新源文件依赖该 CONFIG 选项 (例如 ``obj-$(CONFIG_XYZZY_SYSCALL) += xyzzy.o``)。)
 - Double check that the kernel still builds with the new CONFIG option turned
   off.
   (再次确认关闭新 CONFIG 选项后内核仍能构建。)

To summarize, you need a commit that includes:
(概括起来，你需要一个包含以下内容的 commit：)

 - ``CONFIG`` option for the new function, normally in ``init/Kconfig``
   (新功能的 ``CONFIG`` 选项，通常位于 ``init/Kconfig``)
 - ``SYSCALL_DEFINEn(xyzzy, ...)`` for the entry point
   (入口点的 ``SYSCALL_DEFINEn(xyzzy, ...)``)
 - corresponding prototype in ``include/linux/syscalls.h``
   (``include/linux/syscalls.h`` 中对应的原型)
 - generic table entry in ``include/uapi/asm-generic/unistd.h``
   (``include/uapi/asm-generic/unistd.h`` 中的通用表条目)
 - fallback stub in ``kernel/sys_ni.c``
   (``kernel/sys_ni.c`` 中的后备 stub)


.. _syscall_generic_6_11:

Since 6.11(自 6.11 起)
~~~~~~~~~~~~~~~~~~~~~~

Starting with kernel version 6.11, general system call implementation for the
following architectures no longer requires modifications to
``include/uapi/asm-generic/unistd.h``:
(从内核版本 6.11 开始，以下架构的通用系统调用实现不再需要修改 ``include/uapi/asm-generic/unistd.h``：)

 - arc
 - arm64
 - csky
 - hexagon
 - loongarch
 - nios2
 - openrisc
 - riscv

Instead, you need to update ``scripts/syscall.tbl`` and, if applicable, adjust
``arch/*/kernel/Makefile.syscalls``.
(你需要改为更新 ``scripts/syscall.tbl``，并在适用时调整 ``arch/*/kernel/Makefile.syscalls``。)

As ``scripts/syscall.tbl`` serves as a common syscall table across multiple
architectures, a new entry is required in this table::

    468   common   xyzzy     sys_xyzzy

(由于 ``scripts/syscall.tbl`` 充当跨多个架构的公共系统调用表，需要在这张表中添加一个新条目：)

Note that adding an entry to ``scripts/syscall.tbl`` with the "common" ABI
also affects all architectures that share this table. For more limited or
architecture-specific changes, consider using an architecture-specific ABI or
defining a new one.
(注意，向 ``scripts/syscall.tbl`` 添加 "common" ABI 条目也会影响共享这张表的所有架构。对于更受限或架构特定的修改，考虑使用某个架构专用 ABI 或定义新的 ABI。)

If a new ABI, say ``xyz``, is introduced, the corresponding updates should be
made to ``arch/*/kernel/Makefile.syscalls`` as well::

    syscall_abis_{32,64} += xyz (...)

(如果引入了新 ABI (比如 ``xyz``)，还需要对 ``arch/*/kernel/Makefile.syscalls`` 做相应更新：)

To summarize, you need a commit that includes:
(概括起来，你需要一个包含以下内容的 commit：)

 - ``CONFIG`` option for the new function, normally in ``init/Kconfig``
   (新功能的 ``CONFIG`` 选项，通常位于 ``init/Kconfig``)
 - ``SYSCALL_DEFINEn(xyzzy, ...)`` for the entry point
   (入口点的 ``SYSCALL_DEFINEn(xyzzy, ...)``)
 - corresponding prototype in ``include/linux/syscalls.h``
   (``include/linux/syscalls.h`` 中对应的原型)
 - new entry in ``scripts/syscall.tbl``
   (``scripts/syscall.tbl`` 中的新条目)
 - (if needed) Makefile updates in ``arch/*/kernel/Makefile.syscalls``
   ((如需要) ``arch/*/kernel/Makefile.syscalls`` 中的 Makefile 更新)
 - fallback stub in ``kernel/sys_ni.c``
   (``kernel/sys_ni.c`` 中的后备 stub)


x86 System Call Implementation(x86 系统调用实现)
------------------------------------------------

To wire up your new system call for x86 platforms, you need to update the
master syscall tables.  Assuming your new system call isn't special in some
way (see below), this involves a "common" entry (for x86_64 and x32) in
arch/x86/entry/syscalls/syscall_64.tbl::

    333   common   xyzzy     sys_xyzzy

(要为 x86 平台接上你的新系统调用，需要更新主系统调用表。假设你的新系统调用没有什么特殊之处 (见下文)，这需要在 arch/x86/entry/syscalls/syscall_64.tbl 中添加一个 "common" 条目 (适用于 x86_64 和 x32)：)

and an "i386" entry in ``arch/x86/entry/syscalls/syscall_32.tbl``::

    380   i386     xyzzy     sys_xyzzy

(以及在 ``arch/x86/entry/syscalls/syscall_32.tbl`` 中添加一个 "i386" 条目：)

Again, these numbers are liable to be changed if there are conflicts in the
relevant merge window.
(同样，如果相关合并窗口中存在冲突，这些编号也可能被改动。)


Compatibility System Calls (Generic)(通用架构的兼容系统调用)
------------------------------------------------------------

For most system calls the same 64-bit implementation can be invoked even when
the userspace program is itself 32-bit; even if the system call's parameters
include an explicit pointer, this is handled transparently.
(对大多数系统调用而言，即使用户态程序本身是 32 位的，也可以调用同一个 64 位实现；即使系统调用参数包含显式指针，也会被透明地处理。)

However, there are a couple of situations where a compatibility layer is
needed to cope with size differences between 32-bit and 64-bit.
(不过，有几种情况需要兼容层来应对 32 位与 64 位之间的大小差异。)

The first is if the 64-bit kernel also supports 32-bit userspace programs, and
so needs to parse areas of (``__user``) memory that could hold either 32-bit or
64-bit values.  In particular, this is needed whenever a system call argument
is:
(第一种情况是 64 位内核同时支持 32 位用户态程序，因此需要解析可能容纳 32 位或 64 位值的 (``__user``) 内存区域。特别是，只要系统调用参数属于以下情形就需要兼容层：)

 - a pointer to a pointer
   (指向指针的指针)
 - a pointer to a struct containing a pointer (e.g. ``struct iovec __user *``)
   (指向包含指针的结构体的指针 (例如 ``struct iovec __user *``))
 - a pointer to a varying sized integral type (``time_t``, ``off_t``,
   ``long``, ...)
   (指向大小可变的整数类型 (``time_t``、``off_t``、``long`` 等) 的指针)
 - a pointer to a struct containing a varying sized integral type.
   (指向包含大小可变的整数类型的结构体的指针。)

The second situation that requires a compatibility layer is if one of the
system call's arguments has a type that is explicitly 64-bit even on a 32-bit
architecture, for example ``loff_t`` or ``__u64``.  In this case, a value that
arrives at a 64-bit kernel from a 32-bit application will be split into two
32-bit values, which then need to be re-assembled in the compatibility layer.
(第二种需要兼容层的情况是：系统调用的某个参数即使在 32 位架构上也显式是 64 位类型，例如 ``loff_t`` 或 ``__u64``。这时，从 32 位应用程序传到 64 位内核的值会被拆成两个 32 位值，需要在兼容层中重新拼装。)

(Note that a system call argument that's a pointer to an explicit 64-bit type
does **not** need a compatibility layer; for example, :manpage:`splice(2)`'s arguments of
type ``loff_t __user *`` do not trigger the need for a ``compat_`` system call.)
(注意：指向显式 64 位类型的指针参数 **不需要** 兼容层；例如 :manpage:`splice(2)` 中 ``loff_t __user *`` 类型的参数就不会触发对 ``compat_`` 系统调用的需求。)

The compatibility version of the system call is called ``compat_sys_xyzzy()``,
and is added with the ``COMPAT_SYSCALL_DEFINEn()`` macro, analogously to
SYSCALL_DEFINEn.  This version of the implementation runs as part of a 64-bit
kernel, but expects to receive 32-bit parameter values and does whatever is
needed to deal with them.  (Typically, the ``compat_sys_`` version converts the
values to 64-bit versions and either calls on to the ``sys_`` version, or both of
them call a common inner implementation function.)
(系统调用的兼容版本名为 ``compat_sys_xyzzy()``，使用 ``COMPAT_SYSCALL_DEFINEn()`` 宏添加，方式与 SYSCALL_DEFINEn 类似。这一版本作为 64 位内核的一部分运行，但预期接收 32 位参数值，并做必要的处理。通常 ``compat_sys_`` 版本会把这些值转换为 64 位，然后继续调用 ``sys_`` 版本；或者两者都调用一个公共的内部实现函数。)

The compat entry point also needs a corresponding function prototype, in
``include/linux/compat.h``, marked as asmlinkage to match the way that system
calls are invoked::

    asmlinkage long compat_sys_xyzzy(...);

(compat 入口点也需要在 ``include/linux/compat.h`` 中有对应的函数原型，并标记为 asmlinkage 以匹配系统调用的调用方式：)

If the system call involves a structure that is laid out differently on 32-bit
and 64-bit systems, say ``struct xyzzy_args``, then the include/linux/compat.h
header file should also include a compat version of the structure (``struct
compat_xyzzy_args``) where each variable-size field has the appropriate
``compat_`` type that corresponds to the type in ``struct xyzzy_args``.  The
``compat_sys_xyzzy()`` routine can then use this ``compat_`` structure to
parse the arguments from a 32-bit invocation.
(如果系统调用涉及一个在 32 位和 64 位系统上布局不同的结构体，比如 ``struct xyzzy_args``，那么 include/linux/compat.h 头文件还应包含该结构体的 compat 版本 (``struct compat_xyzzy_args``)，其中每个大小可变的字段都使用与 ``struct xyzzy_args`` 中类型对应的 ``compat_`` 类型。``compat_sys_xyzzy()`` 例程随后就可以用这个 ``compat_`` 结构体解析来自 32 位调用的参数。)

For example, if there are fields::

    struct xyzzy_args {
        const char __user *ptr;
        __kernel_long_t varying_val;
        u64 fixed_val;
        /* ... */
    };

(例如，如果有以下字段：)

in struct xyzzy_args, then struct compat_xyzzy_args would have::

    struct compat_xyzzy_args {
        compat_uptr_t ptr;
        compat_long_t varying_val;
        u64 fixed_val;
        /* ... */
    };

(那么在 struct xyzzy_args 中，struct compat_xyzzy_args 对应地为：)

The generic system call list also needs adjusting to allow for the compat
version; the entry in ``include/uapi/asm-generic/unistd.h`` should use
``__SC_COMP`` rather than ``__SYSCALL``::

    #define __NR_xyzzy 292
    __SC_COMP(__NR_xyzzy, sys_xyzzy, compat_sys_xyzzy)

(通用系统调用列表也需要调整以支持 compat 版本；``include/uapi/asm-generic/unistd.h`` 中的条目应使用 ``__SC_COMP`` 而不是 ``__SYSCALL``：)

To summarize, you need:
(概括起来，你需要：)

 - a ``COMPAT_SYSCALL_DEFINEn(xyzzy, ...)`` for the compat entry point
   (compat 入口点的 ``COMPAT_SYSCALL_DEFINEn(xyzzy, ...)``)
 - corresponding prototype in ``include/linux/compat.h``
   (``include/linux/compat.h`` 中对应的原型)
 - (if needed) 32-bit mapping struct in ``include/linux/compat.h``
   ((如需要) ``include/linux/compat.h`` 中的 32 位映射结构体)
 - instance of ``__SC_COMP`` not ``__SYSCALL`` in
   ``include/uapi/asm-generic/unistd.h``
   (``include/uapi/asm-generic/unistd.h`` 中使用 ``__SC_COMP`` 而不是 ``__SYSCALL``)


Since 6.11(自 6.11 起)
~~~~~~~~~~~~~~~~~~~~~~

This applies to all the architectures listed in :ref:`Since 6.11<syscall_generic_6_11>`
under "Generic System Call Implementation", except arm64. See
:ref:`Compatibility System Calls (arm64)<compat_arm64>` for more information.
(这适用于 "Generic System Call Implementation" 一节中 :ref:`Since 6.11<syscall_generic_6_11>` 列出的所有架构，但 arm64 除外。更多信息见 :ref:`Compatibility System Calls (arm64)<compat_arm64>`。)

You need to extend the entry in ``scripts/syscall.tbl`` with an extra column
to indicate that a 32-bit userspace program running on a 64-bit kernel should
hit the compat entry point::

    468   common     xyzzy     sys_xyzzy    compat_sys_xyzzy

(需要在 ``scripts/syscall.tbl`` 的条目中扩展出一列，表明 64 位内核上运行的 32 位用户态程序应命中 compat 入口点：)

To summarize, you need:
(概括起来，你需要：)

 - ``COMPAT_SYSCALL_DEFINEn(xyzzy, ...)`` for the compat entry point
   (compat 入口点的 ``COMPAT_SYSCALL_DEFINEn(xyzzy, ...)``)
 - corresponding prototype in ``include/linux/compat.h``
   (``include/linux/compat.h`` 中对应的原型)
 - modification of the entry in ``scripts/syscall.tbl`` to include an extra
   "compat" column
   (修改 ``scripts/syscall.tbl`` 中的条目，增加一列 "compat")
 - (if needed) 32-bit mapping struct in ``include/linux/compat.h``
   ((如需要) ``include/linux/compat.h`` 中的 32 位映射结构体)


.. _compat_arm64:

Compatibility System Calls (arm64)(arm64 的兼容系统调用)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On arm64, there is a dedicated syscall table for compatibility system calls
targeting 32-bit (AArch32) userspace: ``arch/arm64/tools/syscall_32.tbl``.
You need to add an additional line to this table specifying the compat
entry point::

    468   common     xyzzy     sys_xyzzy    compat_sys_xyzzy

(在 arm64 上，有一张专门针对 32 位 (AArch32) 用户态的兼容系统调用表：``arch/arm64/tools/syscall_32.tbl``。需要在这张表中额外添加一行，指定 compat 入口点：)


Compatibility System Calls (x86)(x86 的兼容系统调用)
----------------------------------------------------

To wire up the x86 architecture of a system call with a compatibility version,
the entries in the syscall tables need to be adjusted.
(要为带兼容版本的系统调用接上 x86 架构，需要调整系统调用表中的条目。)

First, the entry in ``arch/x86/entry/syscalls/syscall_32.tbl`` gets an extra
column to indicate that a 32-bit userspace program running on a 64-bit kernel
should hit the compat entry point::

    380   i386     xyzzy     sys_xyzzy    __ia32_compat_sys_xyzzy

(首先，``arch/x86/entry/syscalls/syscall_32.tbl`` 中的条目增加一列，表明 64 位内核上运行的 32 位用户态程序应命中 compat 入口点：)

Second, you need to figure out what should happen for the x32 ABI version of
the new system call.  There's a choice here: the layout of the arguments
should either match the 64-bit version or the 32-bit version.
(其次，需要确定新系统调用的 x32 ABI 版本应如何处理。这里有一个选择：参数布局应与 64 位版本或 32 位版本匹配。)

If there's a pointer-to-a-pointer involved, the decision is easy: x32 is
ILP32, so the layout should match the 32-bit version, and the entry in
``arch/x86/entry/syscalls/syscall_64.tbl`` is split so that x32 programs hit
the compatibility wrapper::

    333   64       xyzzy     sys_xyzzy
    ...
    555   x32      xyzzy     __x32_compat_sys_xyzzy

(如果涉及指向指针的指针，决定就很简单：x32 是 ILP32，所以布局应与 32 位版本匹配，并且 ``arch/x86/entry/syscalls/syscall_64.tbl`` 中的条目会被拆分，让 x32 程序命中兼容包装器：)

If no pointers are involved, then it is preferable to reuse the 64-bit system
call for the x32 ABI (and consequently the entry in
arch/x86/entry/syscalls/syscall_64.tbl is unchanged).
(如果不涉及指针，那么 x32 ABI 最好复用 64 位系统调用 (因此 arch/x86/entry/syscalls/syscall_64.tbl 中的条目保持不变)。)

In either case, you should check that the types involved in your argument
layout do indeed map exactly from x32 (-mx32) to either the 32-bit (-m32) or
64-bit (-m64) equivalents.
(无论哪种情况，都应检查参数布局涉及的类型确实能从 x32 (-mx32) 精确映射到 32 位 (-m32) 或 64 位 (-m64) 对应类型。)


System Calls Returning Elsewhere(从别处返回的系统调用)
------------------------------------------------------

For most system calls, once the system call is complete the user program
continues exactly where it left off -- at the next instruction, with the
stack the same and most of the registers the same as before the system call,
and with the same virtual memory space.
(对大多数系统调用来说，系统调用完成后用户程序会从离开处原样继续——从下一条指令开始，栈不变，大多数寄存器与系统调用之前相同，虚拟内存空间也相同。)

However, a few system calls do things differently.  They might return to a
different location (``rt_sigreturn``) or change the memory space
(``fork``/``vfork``/``clone``) or even architecture (``execve``/``execveat``)
of the program.
(但有少数系统调用的行为不同。它们可能返回到不同的位置 (``rt_sigreturn``)，或改变程序的内存空间 (``fork``/``vfork``/``clone``)，甚至改变架构 (``execve``/``execveat``)。)

To allow for this, the kernel implementation of the system call may need to
save and restore additional registers to the kernel stack, allowing complete
control of where and how execution continues after the system call.
(为了支持这一点，系统调用的内核实现可能需要把额外的寄存器保存到内核栈再恢复，从而完全控制系统调用之后在哪里以及如何继续执行。)

This is arch-specific, but typically involves defining assembly entry points
that save/restore additional registers and invoke the real system call entry
point.
(这是架构相关的，但通常涉及定义汇编入口点，由它们保存/恢复额外的寄存器并调用真正的系统调用入口点。)

For x86_64, this is implemented as a ``stub_xyzzy`` entry point in
``arch/x86/entry/entry_64.S``, and the entry in the syscall table
(``arch/x86/entry/syscalls/syscall_64.tbl``) is adjusted to match::

    333   common   xyzzy     stub_xyzzy

(对 x86_64，这实现为 ``arch/x86/entry/entry_64.S`` 中的 ``stub_xyzzy`` 入口点，并相应调整系统调用表 (``arch/x86/entry/syscalls/syscall_64.tbl``) 中的条目：)

The equivalent for 32-bit programs running on a 64-bit kernel is normally
called ``stub32_xyzzy`` and implemented in ``arch/x86/entry/entry_64_compat.S``,
with the corresponding syscall table adjustment in
``arch/x86/entry/syscalls/syscall_32.tbl``::

    380   i386     xyzzy     sys_xyzzy    stub32_xyzzy

(64 位内核上运行的 32 位程序的对应物通常叫 ``stub32_xyzzy``，实现在 ``arch/x86/entry/entry_64_compat.S`` 中，并相应调整 ``arch/x86/entry/syscalls/syscall_32.tbl`` 中的系统调用表：)

If the system call needs a compatibility layer (as in the previous section)
then the ``stub32_`` version needs to call on to the ``compat_sys_`` version
of the system call rather than the native 64-bit version.  Also, if the x32 ABI
implementation is not common with the x86_64 version, then its syscall
table will also need to invoke a stub that calls on to the ``compat_sys_``
version.
(如果系统调用需要兼容层 (如上一节所述)，那么 ``stub32_`` 版本需要调用系统调用的 ``compat_sys_`` 版本而不是原生 64 位版本。另外，如果 x32 ABI 实现与 x86_64 版本不共用，那么它的系统调用表也需要调用一个转向 ``compat_sys_`` 版本的 stub。)

For completeness, it's also nice to set up a mapping so that user-mode Linux
still works -- its syscall table will reference stub_xyzzy, but the UML build
doesn't include ``arch/x86/entry/entry_64.S`` implementation (because UML
simulates registers etc).  Fixing this is as simple as adding a #define to
``arch/x86/um/sys_call_table_64.c``::

    #define stub_xyzzy sys_xyzzy

(此外，为了完整性，最好设置一个映射让 user-mode Linux 仍能工作——它的系统调用表会引用 stub_xyzzy，但 UML 构建不包含 ``arch/x86/entry/entry_64.S`` 的实现 (因为 UML 模拟寄存器等)。修复很简单，只需向 ``arch/x86/um/sys_call_table_64.c`` 添加一个 #define：)


Other Details(其他细节)
-----------------------

Most of the kernel treats system calls in a generic way, but there is the
occasional exception that may need updating for your particular system call.
(内核的大部分都以通用方式对待系统调用，但偶有例外，可能需要针对你的特定系统调用做更新。)

The audit subsystem is one such special case; it includes (arch-specific)
functions that classify some special types of system call -- specifically
file open (``open``/``openat``), program execution (``execve``/``exeveat``) or
socket multiplexor (``socketcall``) operations. If your new system call is
analogous to one of these, then the audit system should be updated.
(audit 子系统就是一个这样的特例；它包含一些 (架构相关的) 函数，用于归类某些特殊类型的系统调用——具体是文件打开 (``open``/``openat``)、程序执行 (``execve``/``exeveat``) 或套接字多路复用器 (``socketcall``) 操作。如果你的新系统调用与其中之一类似，就应更新 audit 系统。)

More generally, if there is an existing system call that is analogous to your
new system call, it's worth doing a kernel-wide grep for the existing system
call to check there are no other special cases.
(更一般地，如果存在与你的新系统调用类似的既有系统调用，值得对既有系统调用做一次全内核 grep，确认没有其他特殊情况。)


Testing(测试)
-------------

A new system call should obviously be tested; it is also useful to provide
reviewers with a demonstration of how user space programs will use the system
call.  A good way to combine these aims is to include a simple self-test
program in a new directory under ``tools/testing/selftests/``.
(新系统调用显然需要测试；向评审者展示用户态程序将如何使用该系统调用也很有用。同时实现这两个目标的好办法，是在 ``tools/testing/selftests/`` 下的新目录中放一个简单的自测程序。)

For a new system call, there will obviously be no libc wrapper function and so
the test will need to invoke it using ``syscall()``; also, if the system call
involves a new userspace-visible structure, the corresponding header will need
to be installed to compile the test.
(对新系统调用来说，显然不会有 libc 包装函数，所以测试需要用 ``syscall()`` 来调用它；另外，如果系统调用涉及新的用户态可见结构体，则需要安装相应头文件才能编译测试。)

Make sure the selftest runs successfully on all supported architectures.  For
example, check that it works when compiled as an x86_64 (-m64), x86_32 (-m32)
and x32 (-mx32) ABI program.
(确保 selftest 在所有支持的架构上都能成功运行。例如，检查它以 x86_64 (-m64)、x86_32 (-m32) 和 x32 (-mx32) ABI 程序编译时都能工作。)

For more extensive and thorough testing of new functionality, you should also
consider adding tests to the Linux Test Project, or to the xfstests project
for filesystem-related changes.
(要对新功能做更广泛、更彻底的测试，还应考虑把测试加进 Linux Test Project；文件系统相关的修改则可加进 xfstests 项目。)

 - https://linux-test-project.github.io/
 - git://git.kernel.org/pub/scm/fs/xfs/xfstests-dev.git


Man Page(man 手册页)
--------------------

All new system calls should come with a complete man page, ideally using groff
markup, but plain text will do.  If groff is used, it's helpful to include a
pre-rendered ASCII version of the man page in the cover email for the
patchset, for the convenience of reviewers.
(所有新系统调用都应附带完整的 man 手册页，最好用 groff 标记编写，纯文本也可以。如果使用 groff，在补丁集的封面邮件中附上预渲染的 ASCII 版本手册页，会给评审者带来便利。)

The man page should be cc'ed to linux-man@vger.kernel.org
For more details, see https://www.kernel.org/doc/man-pages/patches.html
(man 手册页应抄送 linux-man@vger.kernel.org。更多细节见 https://www.kernel.org/doc/man-pages/patches.html。)


Do not call System Calls in the Kernel(不要在内核中调用系统调用函数)
--------------------------------------------------------------------

System calls are, as stated above, interaction points between userspace and
the kernel.  Therefore, system call functions such as ``sys_xyzzy()`` or
``compat_sys_xyzzy()`` should only be called from userspace via the syscall
table, but not from elsewhere in the kernel.  If the syscall functionality is
useful to be used within the kernel, needs to be shared between an old and a
new syscall, or needs to be shared between a syscall and its compatibility
variant, it should be implemented by means of a "helper" function (such as
``ksys_xyzzy()``).  This kernel function may then be called within the
syscall stub (``sys_xyzzy()``), the compatibility syscall stub
(``compat_sys_xyzzy()``), and/or other kernel code.
(如上所述，系统调用是用户态与内核之间的交互点。因此，``sys_xyzzy()`` 或 ``compat_sys_xyzzy()`` 这类系统调用函数只能由用户态经由系统调用表调用，而不能从内核其他地方调用。如果该系统调用功能在内核内也有用、需要在旧系统调用与新系统调用之间共享、或需要在系统调用与其兼容变体之间共享，就应当通过 "helper" 函数 (例如 ``ksys_xyzzy()``) 来实现。这个内核函数随后可以在系统调用 stub (``sys_xyzzy()``)、兼容系统调用 stub (``compat_sys_xyzzy()``) 和/或其他内核代码中调用。)

At least on 64-bit x86, it will be a hard requirement from v4.17 onwards to not
call system call functions in the kernel.  It uses a different calling
convention for system calls where ``struct pt_regs`` is decoded on-the-fly in a
syscall wrapper which then hands processing over to the actual syscall function.
This means that only those parameters which are actually needed for a specific
syscall are passed on during syscall entry, instead of filling in six CPU
registers with random user space content all the time (which may cause serious
trouble down the call chain).
(至少在 64 位 x86 上，从 v4.17 起就硬性要求不得在内核中调用系统调用函数。它对系统调用使用了不同的调用约定：``struct pt_regs`` 在系统调用包装器中被即时解码，然后由包装器把处理转交给实际的系统调用函数。这意味着系统调用进入时只传递特定系统调用真正需要的参数，而不是始终用随机的用户态内容填满六个 CPU 寄存器 (后者可能在调用链下游造成严重问题)。)

Moreover, rules on how data may be accessed may differ between kernel data and
user data.  This is another reason why calling ``sys_xyzzy()`` is generally a
bad idea.
(此外，内核数据与用户数据在数据访问规则上可能不同。这是调用 ``sys_xyzzy()`` 通常不是好主意的另一个原因。)

Exceptions to this rule are only allowed in architecture-specific overrides,
architecture-specific compatibility wrappers, or other code in arch/.
(该规则的例外仅允许出现在架构专属的覆盖实现、架构专属的兼容包装器或 arch/ 下的其他代码中。)


References and Sources(参考资料与来源)
--------------------------------------

 - LWN article from Michael Kerrisk on use of flags argument in system calls:
   https://lwn.net/Articles/585415/
   (Michael Kerrisk 关于系统调用中 flags 参数用法的 LWN 文章：https://lwn.net/Articles/585415/)
 - LWN article from Michael Kerrisk on how to handle unknown flags in a system
   call: https://lwn.net/Articles/588444/
   (Michael Kerrisk 关于如何处理系统调用中未知标志的 LWN 文章：https://lwn.net/Articles/588444/)
 - LWN article from Jake Edge describing constraints on 64-bit system call
   arguments: https://lwn.net/Articles/311630/
   (Jake Edge 描述 64 位系统调用参数约束的 LWN 文章：https://lwn.net/Articles/311630/)
 - Pair of LWN articles from David Drysdale that describe the system call
   implementation paths in detail for v3.14:
   (David Drysdale 的两篇 LWN 文章，详细描述了 v3.14 的系统调用实现路径：)

    - https://lwn.net/Articles/604287/
    - https://lwn.net/Articles/604515/

 - Architecture-specific requirements for system calls are discussed in the
   :manpage:`syscall(2)` man-page:
   http://man7.org/linux/man-pages/man2/syscall.2.html#NOTES
   (:manpage:`syscall(2)` 手册页讨论了系统调用的架构特定要求：http://man7.org/linux/man-pages/man2/syscall.2.html#NOTES)
 - Collated emails from Linus Torvalds discussing the problems with ``ioctl()``:
   https://yarchive.net/comp/linux/ioctl.html
   (Linus Torvalds 关于 ``ioctl()`` 问题的邮件汇编：https://yarchive.net/comp/linux/ioctl.html)
 - "How to not invent kernel interfaces", Arnd Bergmann,
   https://www.ukuug.org/events/linux2007/2007/papers/Bergmann.pdf
   (《How to not invent kernel interfaces》，Arnd Bergmann：https://www.ukuug.org/events/linux2007/2007/papers/Bergmann.pdf)
 - LWN article from Michael Kerrisk on avoiding new uses of CAP_SYS_ADMIN:
   https://lwn.net/Articles/486306/
   (Michael Kerrisk 关于避免 CAP_SYS_ADMIN 新用法的 LWN 文章：https://lwn.net/Articles/486306/)
 - Recommendation from Andrew Morton that all related information for a new
   system call should come in the same email thread:
   https://lore.kernel.org/r/20140724144747.3041b208832bbdf9fbce5d96@linux-foundation.org
   (Andrew Morton 的建议：新系统调用的所有相关信息应出现在同一个邮件线程中：https://lore.kernel.org/r/20140724144747.3041b208832bbdf9fbce5d96@linux-foundation.org)
 - Recommendation from Michael Kerrisk that a new system call should come with
   a man page: https://lore.kernel.org/r/CAKgNAkgMA39AfoSoA5Pe1r9N+ZzfYQNvNPvcRN7tOvRb8+v06Q@mail.gmail.com
   (Michael Kerrisk 的建议：新系统调用应附带 man 手册页：https://lore.kernel.org/r/CAKgNAkgMA39AfoSoA5Pe1r9N+ZzfYQNvNPvcRN7tOvRb8+v06Q@mail.gmail.com)
 - Suggestion from Thomas Gleixner that x86 wire-up should be in a separate
   commit: https://lore.kernel.org/r/alpine.DEB.2.11.1411191249560.3909@nanos
   (Thomas Gleixner 的建议：x86 接线应放在单独的 commit 中：https://lore.kernel.org/r/alpine.DEB.2.11.1411191249560.3909@nanos)
 - Suggestion from Greg Kroah-Hartman that it's good for new system calls to
   come with a man-page & selftest: https://lore.kernel.org/r/20140320025530.GA25469@kroah.com
   (Greg Kroah-Hartman 的建议：新系统调用最好附带 man 手册页和 selftest：https://lore.kernel.org/r/20140320025530.GA25469@kroah.com)
 - Discussion from Michael Kerrisk of new system call vs. :manpage:`prctl(2)` extension:
   https://lore.kernel.org/r/CAHO5Pa3F2MjfTtfNxa8LbnkeeU8=YJ+9tDqxZpw7Gz59E-4AUg@mail.gmail.com
   (Michael Kerrisk 关于新系统调用与 :manpage:`prctl(2)` 扩展之取舍的讨论：https://lore.kernel.org/r/CAHO5Pa3F2MjfTtfNxa8LbnkeeU8=YJ+9tDqxZpw7Gz59E-4AUg@mail.gmail.com)
 - Suggestion from Ingo Molnar that system calls that involve multiple
   arguments should encapsulate those arguments in a struct, which includes a
   size field for future extensibility: https://lore.kernel.org/r/20150730083831.GA22182@gmail.com
   (Ingo Molnar 的建议：涉及多个参数的系统调用应把这些参数封装进一个结构体，其中包含用于未来扩展的 size 字段：https://lore.kernel.org/r/20150730083831.GA22182@gmail.com)
 - Numbering oddities arising from (re-)use of O_* numbering space flags:
   (因 (重复) 使用 O_* 编号空间标志而产生的编号怪事：)

    - commit 75069f2b5bfb ("vfs: renumber FMODE_NONOTIFY and add to uniqueness
      check")
    - commit 12ed2e36c98a ("fanotify: FMODE_NONOTIFY and __O_SYNC in sparc
      conflict")
    - commit bb458c644a59 ("Safer ABI for O_TMPFILE")

 - Discussion from Matthew Wilcox about restrictions on 64-bit arguments:
   https://lore.kernel.org/r/20081212152929.GM26095@parisc-linux.org
   (Matthew Wilcox 关于 64 位参数限制的讨论：https://lore.kernel.org/r/20081212152929.GM26095@parisc-linux.org)
 - Recommendation from Greg Kroah-Hartman that unknown flags should be
   policed: https://lore.kernel.org/r/20140717193330.GB4703@kroah.com
   (Greg Kroah-Hartman 的建议：未知标志应受到管制：https://lore.kernel.org/r/20140717193330.GB4703@kroah.com)
 - Recommendation from Linus Torvalds that x32 system calls should prefer
   compatibility with 64-bit versions rather than 32-bit versions:
   https://lore.kernel.org/r/CA+55aFxfmwfB7jbbrXxa=K7VBYPfAvmu3XOkGrLbB1UFjX1+Ew@mail.gmail.com
   (Linus Torvalds 的建议：x32 系统调用应优先兼容 64 位版本而非 32 位版本：https://lore.kernel.org/r/CA+55aFxfmwfB7jbbrXxa=K7VBYPfAvmu3XOkGrLbB1UFjX1+Ew@mail.gmail.com)
 - Patch series revising system call table infrastructure to use
   scripts/syscall.tbl across multiple architectures:
   https://lore.kernel.org/lkml/20240704143611.2979589-1-arnd@kernel.org
   (修订系统调用表基础设施、在多个架构上使用 scripts/syscall.tbl 的补丁系列：https://lore.kernel.org/lkml/20240704143611.2979589-1-arnd@kernel.org)
