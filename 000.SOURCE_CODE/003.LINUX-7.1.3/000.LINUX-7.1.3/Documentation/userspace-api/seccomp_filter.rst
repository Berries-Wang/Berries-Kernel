===============================================================
Seccomp BPF (SECure COMPuting with filters)(带过滤器的安全计算)
===============================================================

Introduction(简介)
==================

A large number of system calls are exposed to every userland process
with many of them going unused for the entire lifetime of the process.
As system calls change and mature, bugs are found and eradicated.  A
certain subset of userland applications benefit by having a reduced set
of available system calls.  The resulting set reduces the total kernel
surface exposed to the application.  System call filtering is meant for
use with those applications.
(大量系统调用暴露给每一个用户态进程，其中许多在整个进程生命周期内都
不会被用到。随着系统调用不断变化与成熟，缺陷被发现并清除。一部分用户
态应用程序受益于精简后的可用系统调用集合，由此得到的集合减少了暴露给
应用程序的内核攻击面总量。系统调用过滤正是为这类应用而设计的。)

Seccomp filtering provides a means for a process to specify a filter for
incoming system calls.  The filter is expressed as a Berkeley Packet
Filter (BPF) program, as with socket filters, except that the data
operated on is related to the system call being made: system call
number and the system call arguments.  This allows for expressive
filtering of system calls using a filter program language with a long
history of being exposed to userland and a straightforward data set.
(Seccomp 过滤为进程提供了一种为进入的系统调用指定过滤器的方式。过滤
器以 Berkeley Packet Filter (BPF) 程序的形式表达，与套接字过滤器一样，
只不过其操作的数据与正在发起的系统调用相关：系统调用号和系统调用参数。
这使得可以用一种早已向用户态开放、数据集简单直接、历史悠久的过滤器程
序语言，来实现表达力丰富的系统调用过滤。)

Additionally, BPF makes it impossible for users of seccomp to fall prey
to time-of-check-time-of-use (TOCTOU) attacks that are common in system
call interposition frameworks.  BPF programs may not dereference
pointers which constrains all filters to solely evaluating the system
call arguments directly.
(此外，BPF 使 seccomp 的使用者不可能落入系统调用插入 (interposition)
框架中常见的时间检查-时间使用 (TOCTOU) 攻击的圈套。BPF 程序不能解引用
指针，这把所有过滤器限制为只能直接对系统调用参数求值。)

What it isn't(它不是什么)
=========================

System call filtering isn't a sandbox.  It provides a clearly defined
mechanism for minimizing the exposed kernel surface.  It is meant to be
a tool for sandbox developers to use.  Beyond that, policy for logical
behavior and information flow should be managed with a combination of
other system hardening techniques and, potentially, an LSM of your
choosing.  Expressive, dynamic filters provide further options down this
path (avoiding pathological sizes or selecting which of the multiplexed
system calls in socketcall() is allowed, for instance) which could be
construed, incorrectly, as a more complete sandboxing solution.
(系统调用过滤并不是沙箱。它提供一种明确定义的机制，用于最小化暴露的
内核面；它是供沙箱开发者使用的工具。除此之外，逻辑行为与信息流的策略
应当结合其他系统加固技术，以及可能的话你自选的某个 LSM 来管理。表达力
强、动态的过滤器在这条路上提供了更多选项 (例如避免病态的体积，或选择
socketcall() 多路复用的各系统调用中哪些被允许)，这可能被错误地解读为
一种更完整的沙箱化方案。)

Usage(用法)
===========

An additional seccomp mode is added and is enabled using the same
prctl(2) call as the strict seccomp.  If the architecture has
``CONFIG_HAVE_ARCH_SECCOMP_FILTER``, then filters may be added as below:
(在严格 seccomp 之外新增了一种 seccomp 模式，并使用与严格 seccomp 相
同的 prctl(2) 调用启用。如果架构支持 ``CONFIG_HAVE_ARCH_SECCOMP_FILTER``，
则可以按如下方式添加过滤器：)

``PR_SET_SECCOMP``:
	Now takes an additional argument which specifies a new filter
	using a BPF program.
	The BPF program will be executed over struct seccomp_data
	reflecting the system call number, arguments, and other
	metadata.  The BPF program must then return one of the
	acceptable values to inform the kernel which action should be
	taken.
	(现在接受一个额外参数，该参数用 BPF 程序指定一个新的过滤器。BPF
	程序将在 struct seccomp_data 上执行，其中反映了系统调用号、参数以
	及其他元数据。BPF 程序随后必须返回一个可接受的值，告知内核应当采
	取哪种动作。)

	Usage::

		prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, prog);

	(用法：)

	The 'prog' argument is a pointer to a struct sock_fprog which
	will contain the filter program.  If the program is invalid, the
	call will return -1 and set errno to ``EINVAL``.
	('prog' 参数是一个指向 struct sock_fprog 的指针，其中包含过滤器程
	序。如果程序无效，调用将返回 -1 并把 errno 设为 ``EINVAL``。)

	If ``fork``/``clone`` and ``execve`` are allowed by @prog, any child
	processes will be constrained to the same filters and system
	call ABI as the parent.
	(如果 @prog 允许 ``fork``/``clone`` 和 ``execve``，任何子进程都将被
	约束在与父进程相同的过滤器和系统调用 ABI 之下。)

	Prior to use, the task must call ``prctl(PR_SET_NO_NEW_PRIVS, 1)`` or
	run with ``CAP_SYS_ADMIN`` privileges in its namespace.  If these are not
	true, ``-EACCES`` will be returned.  This requirement ensures that filter
	programs cannot be applied to child processes with greater privileges
	than the task that installed them.
	(使用之前，任务必须调用 ``prctl(PR_SET_NO_NEW_PRIVS, 1)``，或者在其
	命名空间内以 ``CAP_SYS_ADMIN`` 特权运行。若不满足，将返回 ``-EACCES``。
	这一要求确保过滤器程序不能被施加到特权高于安装者的子进程上。)

	Additionally, if ``prctl(2)`` is allowed by the attached filter,
	additional filters may be layered on which will increase evaluation
	time, but allow for further decreasing the attack surface during
	execution of a process.
	(此外，如果所附加的过滤器允许 ``prctl(2)``，还可以再叠加更多过滤器，
	这会增加求值时间，但能在进程执行过程中进一步缩小攻击面。)

The above call returns 0 on success and non-zero on error.
(上述调用成功时返回 0，出错时返回非零值。)

Return values(返回值)
=====================

A seccomp filter may return any of the following values. If multiple
filters exist, the return value for the evaluation of a given system
call will always use the highest precedent value. (For example,
``SECCOMP_RET_KILL_PROCESS`` will always take precedence.)
(Seccomp 过滤器可以返回下列值中的任意一个。如果存在多个过滤器，对某
个给定系统调用求值时，其返回值总是采用优先级最高的那个值 (例如，
``SECCOMP_RET_KILL_PROCESS`` 总是占据优先)。)

In precedence order, they are:
(按优先级顺序，它们是：)

``SECCOMP_RET_KILL_PROCESS``:
	Results in the entire process exiting immediately without executing
	the system call.  The exit status of the task (``status & 0x7f``)
	will be ``SIGSYS``, not ``SIGKILL``.
	(使整个进程立即退出，且不执行该系统调用。任务的退出状态
	(``status & 0x7f``) 将是 ``SIGSYS``，而不是 ``SIGKILL``。)

``SECCOMP_RET_KILL_THREAD``:
	Results in the task exiting immediately without executing the
	system call.  The exit status of the task (``status & 0x7f``) will
	be ``SIGSYS``, not ``SIGKILL``.
	(使该任务立即退出，且不执行该系统调用。任务的退出状态
	(``status & 0x7f``) 将是 ``SIGSYS``，而不是 ``SIGKILL``。)

``SECCOMP_RET_TRAP``:
	Results in the kernel sending a ``SIGSYS`` signal to the triggering
	task without executing the system call. ``siginfo->si_call_addr``
	will show the address of the system call instruction, and
	``siginfo->si_syscall`` and ``siginfo->si_arch`` will indicate which
	syscall was attempted.  The program counter will be as though
	the syscall happened (i.e. it will not point to the syscall
	instruction).  The return value register will contain an arch-
	dependent value -- if resuming execution, set it to something
	sensible.  (The architecture dependency is because replacing
	it with ``-ENOSYS`` could overwrite some useful information.)
	(使内核向触发任务发送 ``SIGSYS`` 信号，而不执行该系统调用。
	``siginfo->si_call_addr`` 将显示系统调用指令的地址，
	``siginfo->si_syscall`` 和 ``siginfo->si_arch`` 将指明尝试的是哪个
	系统调用。程序计数器表现为系统调用已经发生 (即它不会指向系统调用
	指令)。返回值寄存器将包含一个架构相关的值 -- 如果要恢复执行，应将
	其设置为某个合理的值。(之所以依赖架构，是因为将其替换为 ``-ENOSYS``
	可能会覆盖一些有用的信息。))

	The ``SECCOMP_RET_DATA`` portion of the return value will be passed
	as ``si_errno``.
	(返回值中的 ``SECCOMP_RET_DATA`` 部分将作为 ``si_errno`` 传递。)

	``SIGSYS`` triggered by seccomp will have a si_code of ``SYS_SECCOMP``.
	(由 seccomp 触发的 ``SIGSYS`` 其 si_code 为 ``SYS_SECCOMP``。)

``SECCOMP_RET_ERRNO``:
	Results in the lower 16-bits of the return value being passed
	to userland as the errno without executing the system call.
	(使返回值的低 16 位作为 errno 传递给用户态，而不执行该系统调用。)

``SECCOMP_RET_USER_NOTIF``:
	Results in a ``struct seccomp_notif`` message sent on the userspace
	notification fd, if it is attached, or ``-ENOSYS`` if it is not. See
	below on discussion of how to handle user notifications.
	(使一条 ``struct seccomp_notif`` 消息发送到用户空间通知 fd 上 (如果
	已附加)，否则返回 ``-ENOSYS``。关于如何处理用户通知的讨论见下文。)

``SECCOMP_RET_TRACE``:
	When returned, this value will cause the kernel to attempt to
	notify a ``ptrace()``-based tracer prior to executing the system
	call.  If there is no tracer present, ``-ENOSYS`` is returned to
	userland and the system call is not executed.
	(返回该值时，内核会在执行系统调用之前尝试通知基于 ``ptrace()`` 的
	跟踪器。如果没有跟踪器在场，则向用户态返回 ``-ENOSYS``，且系统调用
	不会被执行。)

	A tracer will be notified if it requests ``PTRACE_O_TRACESECCOMP``
	using ``ptrace(PTRACE_SETOPTIONS)``.  The tracer will be notified
	of a ``PTRACE_EVENT_SECCOMP`` and the ``SECCOMP_RET_DATA`` portion of
	the BPF program return value will be available to the tracer
	via ``PTRACE_GETEVENTMSG``.
	(如果跟踪器通过 ``ptrace(PTRACE_SETOPTIONS)`` 请求
	``PTRACE_O_TRACESECCOMP``，它就会收到通知。跟踪器会收到
	``PTRACE_EVENT_SECCOMP`` 事件，并且 BPF 程序返回值中的
	``SECCOMP_RET_DATA`` 部分可通过 ``PTRACE_GETEVENTMSG`` 提供给跟踪器。)

	The tracer can skip the system call by changing the syscall number
	to -1.  Alternatively, the tracer can change the system call
	requested by changing the system call to a valid syscall number.  If
	the tracer asks to skip the system call, then the system call will
	appear to return the value that the tracer puts in the return value
	register.
	(跟踪器可以把系统调用号改为 -1 来跳过该系统调用；或者也可以把系统
	调用改为另一个有效的系统调用号，从而变更所请求的系统调用。如果跟踪
	器要求跳过系统调用，那么该系统调用看起来将返回跟踪器放入返回值寄存
	器中的那个值。)

	The seccomp check will not be run again after the tracer is
	notified.  (This means that seccomp-based sandboxes MUST NOT
	allow use of ptrace, even of other sandboxed processes, without
	extreme care; ptracers can use this mechanism to escape.)
	(在跟踪器收到通知之后，seccomp 检查不会再运行。(这意味着基于
	seccomp 的沙箱绝不能轻率地允许使用 ptrace，即使是对其他被沙箱化的进
	程；ptrace 跟踪者可以利用这一机制逃逸。))

``SECCOMP_RET_LOG``:
	Results in the system call being executed after it is logged. This
	should be used by application developers to learn which syscalls their
	application needs without having to iterate through multiple test and
	development cycles to build the list.
	(使系统调用在记录之后被执行。应用程序开发者可用它来了解其应用需要
	哪些系统调用，而不必经历多轮测试与开发迭代来构建这张清单。)

	This action will only be logged if "log" is present in the
	actions_logged sysctl string.
	(只有当 actions_logged sysctl 字符串中包含 "log" 时，该动作才会被
	记录下来。)

``SECCOMP_RET_ALLOW``:
	Results in the system call being executed.
	(使系统调用被执行。)

If multiple filters exist, the return value for the evaluation of a
given system call will always use the highest precedent value.
(如果存在多个过滤器，对某个给定系统调用求值时的返回值总是采用优先级
最高的那个值。)

Precedence is only determined using the ``SECCOMP_RET_ACTION`` mask.  When
multiple filters return values of the same precedence, only the
``SECCOMP_RET_DATA`` from the most recently installed filter will be
returned.
(优先级仅通过 ``SECCOMP_RET_ACTION`` 掩码来确定。当多个过滤器返回相同
优先级的值时，只返回最近安装的过滤器给出的 ``SECCOMP_RET_DATA``。)

Pitfalls(陷阱)
==============

The biggest pitfall to avoid during use is filtering on system call
number without checking the architecture value.  Why?  On any
architecture that supports multiple system call invocation conventions,
the system call numbers may vary based on the specific invocation.  If
the numbers in the different calling conventions overlap, then checks in
the filters may be abused.  Always check the arch value!
(使用中要避免的最大陷阱是只按系统调用号过滤而不检查架构值。为什么？
在任何支持多种系统调用调用约定的架构上，系统调用号可能因具体的调用约
定而不同。如果不同调用约定中的编号相互重叠，过滤器中的检查就可能被滥
用。务必检查 arch 值！)

Example(示例)
=============

The ``samples/seccomp/`` directory contains both an x86-specific example
and a more generic example of a higher level macro interface for BPF
program generation.
(``samples/seccomp/`` 目录中既有一个 x86 专属的示例，也有一个更通用
的高层宏接口示例，用于生成 BPF 程序。)

Userspace Notification(用户空间通知)
====================================

The ``SECCOMP_RET_USER_NOTIF`` return code lets seccomp filters pass a
particular syscall to userspace to be handled. This may be useful for
applications like container managers, which wish to intercept particular
syscalls (``mount()``, ``finit_module()``, etc.) and change their behavior.
(``SECCOMP_RET_USER_NOTIF`` 返回码让 seccomp 过滤器把某个特定系统调用
交给用户空间处理。这对容器管理器之类的应用可能有用，它们希望拦截某些
特定系统调用 (``mount()``、``finit_module()`` 等) 并改变其行为。)

To acquire a notification FD, use the ``SECCOMP_FILTER_FLAG_NEW_LISTENER``
argument to the ``seccomp()`` syscall:
(要获取通知 fd，请在 ``seccomp()`` 系统调用中使用
``SECCOMP_FILTER_FLAG_NEW_LISTENER`` 参数：)

.. code-block:: c

    fd = seccomp(SECCOMP_SET_MODE_FILTER, SECCOMP_FILTER_FLAG_NEW_LISTENER, &prog);

which (on success) will return a listener fd for the filter, which can then be
passed around via ``SCM_RIGHTS`` or similar. Note that filter fds correspond to
a particular filter, and not a particular task. So if this task then forks,
notifications from both tasks will appear on the same filter fd. Reads and
writes to/from a filter fd are also synchronized, so a filter fd can safely
have many readers.
(成功时它将返回该过滤器的监听 fd，之后可以经由 ``SCM_RIGHTS`` 或类似机
制传递出去。注意：过滤器 fd 对应的是某个特定的过滤器，而不是某个特定
的任务。因此如果这个任务随后 fork，两个任务的通知都会出现在同一个过滤
器 fd 上。对过滤器 fd 的读写在内部是同步的，所以一个过滤器 fd 可以安全
地拥有多个读取者。)

The interface for a seccomp notification fd consists of two structures:
(seccomp 通知 fd 的接口由两个结构体组成：)

.. code-block:: c

    struct seccomp_notif_sizes {
        __u16 seccomp_notif;
        __u16 seccomp_notif_resp;
        __u16 seccomp_data;
    };

    struct seccomp_notif {
        __u64 id;
        __u32 pid;
        __u32 flags;
        struct seccomp_data data;
    };

    struct seccomp_notif_resp {
        __u64 id;
        __s64 val;
        __s32 error;
        __u32 flags;
    };

The ``struct seccomp_notif_sizes`` structure can be used to determine the size
of the various structures used in seccomp notifications. The size of ``struct
seccomp_data`` may change in the future, so code should use:
(``struct seccomp_notif_sizes`` 结构体可用于确定 seccomp 通知中用到的
各结构体的大小。``struct seccomp_data`` 的大小将来可能变化，因此代码应
使用：)

.. code-block:: c

    struct seccomp_notif_sizes sizes;
    seccomp(SECCOMP_GET_NOTIF_SIZES, 0, &sizes);

to determine the size of the various structures to allocate. See
samples/seccomp/user-trap.c for an example.
(来确定要分配的各结构体的大小。示例参见 samples/seccomp/user-trap.c。)

Users can read via ``ioctl(SECCOMP_IOCTL_NOTIF_RECV)``  (or ``poll()``) on a
seccomp notification fd to receive a ``struct seccomp_notif``, which contains
five members: the input length of the structure, a unique-per-filter ``id``,
the ``pid`` of the task which triggered this request (which may be 0 if the
task is in a pid ns not visible from the listener's pid namespace). The
notification also contains the ``data`` passed to seccomp, and a filters flag.
The structure should be zeroed out prior to calling the ioctl.
(用户可以在 seccomp 通知 fd 上通过 ``ioctl(SECCOMP_IOCTL_NOTIF_RECV)``
(或 ``poll()``) 读取来接收一个 ``struct seccomp_notif``，其中包含五个成
员：结构的输入长度、每个过滤器唯一的 ``id``、触发该请求的任务的
``pid`` (如果该任务处于监听者 pid 命名空间不可见的 pid ns 中，则可能为
0)。通知还包含传给 seccomp 的 ``data`` 以及一个过滤器标志。调用 ioctl
之前应将该结构体清零。)

Userspace can then make a decision based on this information about what to do,
and ``ioctl(SECCOMP_IOCTL_NOTIF_SEND)`` a response, indicating what should be
returned to userspace. The ``id`` member of ``struct seccomp_notif_resp`` should
be the same ``id`` as in ``struct seccomp_notif``.
(用户空间随后可以基于这些信息决定要做什么，并通过
``ioctl(SECCOMP_IOCTL_NOTIF_SEND)`` 发送一个响应，指明应当向用户空间返回
什么。``struct seccomp_notif_resp`` 的 ``id`` 成员应与
``struct seccomp_notif`` 中的 ``id`` 相同。)

Userspace can also add file descriptors to the notifying process via
``ioctl(SECCOMP_IOCTL_NOTIF_ADDFD)``. The ``id`` member of
``struct seccomp_notif_addfd`` should be the same ``id`` as in
``struct seccomp_notif``. The ``newfd_flags`` flag may be used to set flags
like O_CLOEXEC on the file descriptor in the notifying process. If the supervisor
wants to inject the file descriptor with a specific number, the
``SECCOMP_ADDFD_FLAG_SETFD`` flag can be used, and set the ``newfd`` member to
the specific number to use. If that file descriptor is already open in the
notifying process it will be replaced. The supervisor can also add an FD, and
respond atomically by using the ``SECCOMP_ADDFD_FLAG_SEND`` flag and the return
value will be the injected file descriptor number.
(用户空间还可以通过 ``ioctl(SECCOMP_IOCTL_NOTIF_ADDFD)`` 向发起通知的
进程添加文件描述符。``struct seccomp_notif_addfd`` 的 ``id`` 成员应与
``struct seccomp_notif`` 中的 ``id`` 相同。``newfd_flags`` 标志可用于在
发起通知的进程中为该文件描述符设置 O_CLOEXEC 之类的标志。如果监督者希
望以特定编号注入文件描述符，可以使用 ``SECCOMP_ADDFD_FLAG_SETFD`` 标志，
并把 ``newfd`` 成员设置为要使用的特定编号。如果该文件描述符在发起通知
的进程中已经打开，它将被替换。监督者还可以在添加 fd 的同时原子地作出
响应，即使用 ``SECCOMP_ADDFD_FLAG_SEND`` 标志，此时返回值就是被注入的
文件描述符编号。)

The notifying process can be preempted, resulting in the notification being
aborted. This can be problematic when trying to take actions on behalf of the
notifying process that are long-running and typically retryable (mounting a
filesystem). Alternatively, at filter installation time, the
``SECCOMP_FILTER_FLAG_WAIT_KILLABLE_RECV`` flag can be set. This flag makes it
such that when a user notification is received by the supervisor, the notifying
process will ignore non-fatal signals until the response is sent. Signals that
are sent prior to the notification being received by userspace are handled
normally.
(发起通知的进程可能被抢占，导致通知被中止。当代替发起通知的进程执行
耗时较长且通常可重试的动作 (如挂载文件系统) 时，这会成为问题。另一种
做法是在安装过滤器时设置 ``SECCOMP_FILTER_FLAG_WAIT_KILLABLE_RECV`` 标
志。该标志使得当监督者收到用户通知后，发起通知的进程会忽略非致命信号，
直到响应被发出。在通知被用户空间接收之前发送的信号则照常处理。)

It is worth noting that ``struct seccomp_data`` contains the values of register
arguments to the syscall, but does not contain pointers to memory. The task's
memory is accessible to suitably privileged traces via ``ptrace()`` or
``/proc/pid/mem``. However, care should be taken to avoid the TOCTOU mentioned
above in this document: all arguments being read from the tracee's memory
should be read into the tracer's memory before any policy decisions are made.
This allows for an atomic decision on syscall arguments.
(值得注意的是，``struct seccomp_data`` 包含传给系统调用的寄存器参数的
值，但不包含指向内存的指针。具有足够特权的跟踪者可以通过 ``ptrace()`` 或
``/proc/pid/mem`` 访问任务的内存。然而，应当小心避免本文前文提到的
TOCTOU 问题：所有要从被跟踪者内存中读取的参数，都应在作出任何策略决定
之前先读入跟踪者自己的内存。这样可以就系统调用参数作出原子性的决定。)

Sysctls(sysctl 文件)
====================

Seccomp's sysctl files can be found in the ``/proc/sys/kernel/seccomp/``
directory. Here's a description of each file in that directory:
(Seccomp 的 sysctl 文件位于 ``/proc/sys/kernel/seccomp/`` 目录下。以下
是对该目录中每个文件的说明：)

``actions_avail``:
	A read-only ordered list of seccomp return values (refer to the
	``SECCOMP_RET_*`` macros above) in string form. The ordering, from
	left-to-right, is the least permissive return value to the most
	permissive return value.
	(以字符串形式给出的 seccomp 返回值的只读有序列表 (参见上面的
	``SECCOMP_RET_*`` 宏)。从左到右的顺序是从限制最严的返回值到限制最
	松的返回值。)

	The list represents the set of seccomp return values supported
	by the kernel. A userspace program may use this list to
	determine if the actions found in the ``seccomp.h``, when the
	program was built, differs from the set of actions actually
	supported in the current running kernel.
	(该列表代表内核支持的 seccomp 返回值集合。用户态程序可以用这个列
	表来判断程序构建时 ``seccomp.h`` 中的动作集合，与当前运行内核实际
	支持的动作集合是否存在差异。)

``actions_logged``:
	A read-write ordered list of seccomp return values (refer to the
	``SECCOMP_RET_*`` macros above) that are allowed to be logged. Writes
	to the file do not need to be in ordered form but reads from the file
	will be ordered in the same way as the actions_avail sysctl.
	(允许被记录日志的 seccomp 返回值的可读写有序列表 (参见上面的
	``SECCOMP_RET_*`` 宏)。写入该文件时无需有序，但读取该文件时会按照
	与 actions_avail sysctl 相同的方式排序。)

	The ``allow`` string is not accepted in the ``actions_logged`` sysctl
	as it is not possible to log ``SECCOMP_RET_ALLOW`` actions. Attempting
	to write ``allow`` to the sysctl will result in an EINVAL being
	returned.
	(``actions_logged`` sysctl 不接受 ``allow`` 字符串，因为
	``SECCOMP_RET_ALLOW`` 动作无法被记录。尝试向该 sysctl 写入 "allow"
	将导致返回 EINVAL。)

Adding architecture support(添加架构支持)
=========================================

See ``arch/Kconfig`` for the authoritative requirements.  In general, if an
architecture supports both ptrace_event and seccomp, it will be able to
support seccomp filter with minor fixup: ``SIGSYS`` support and seccomp return
value checking.  Then it must just add ``CONFIG_HAVE_ARCH_SECCOMP_FILTER``
to its arch-specific Kconfig.
(权威要求参见 ``arch/Kconfig``。一般而言，如果某架构同时支持
ptrace_event 和 seccomp，那么只需少量修补 (``SIGSYS`` 支持和 seccomp
返回值检查) 就能支持 seccomp 过滤器。之后它只需在自己的架构专属
Kconfig 中添加 ``CONFIG_HAVE_ARCH_SECCOMP_FILTER``。)



Caveats(注意事项)
=================

The vDSO can cause some system calls to run entirely in userspace,
leading to surprises when you run programs on different machines that
fall back to real syscalls.  To minimize these surprises on x86, make
sure you test with
``/sys/devices/system/clocksource/clocksource0/current_clocksource`` set to
something like ``acpi_pm``.
(vDSO 会让某些系统调用完全在用户空间执行，当你换到不同的机器上运行程
序、这些调用退回到真正的系统调用时，就会出现意外。为了在 x86 上把这类
意外减到最少，请确保在
``/sys/devices/system/clocksource/clocksource0/current_clocksource`` 设
置为 ``acpi_pm`` 之类的情况下进行测试。)

On x86-64, vsyscall emulation is enabled by default.  (vsyscalls are
legacy variants on vDSO calls.)  Currently, emulated vsyscalls will
honor seccomp, with a few oddities:
(在 x86-64 上，vsyscall 模拟默认是启用的。(vsyscall 是 vDSO 调用的旧式
变体。) 目前，被模拟的 vsyscall 会遵守 seccomp，但有一些怪异之处：)

- A return value of ``SECCOMP_RET_TRAP`` will set a ``si_call_addr`` pointing to
  the vsyscall entry for the given call and not the address after the
  'syscall' instruction.  Any code which wants to restart the call
  should be aware that (a) a ret instruction has been emulated and (b)
  trying to resume the syscall will again trigger the standard vsyscall
  emulation security checks, making resuming the syscall mostly
  pointless.
  (返回值为 ``SECCOMP_RET_TRAP`` 时，``si_call_addr`` 将指向给定调用的
  vsyscall 入口，而不是 'syscall' 指令之后的地址。任何想要重新发起该调
  用的代码都应当知道：(a) ret 指令已被模拟，(b) 尝试恢复该系统调用会再
  次触发标准的 vsyscall 模拟安全检查，因此恢复系统调用基本没有意义。)

- A return value of ``SECCOMP_RET_TRACE`` will signal the tracer as usual,
  but the syscall may not be changed to another system call using the
  orig_rax register. It may only be changed to -1 order to skip the
  currently emulated call. Any other change MAY terminate the process.
  The rip value seen by the tracer will be the syscall entry address;
  this is different from normal behavior.  The tracer MUST NOT modify
  rip or rsp.  (Do not rely on other changes terminating the process.
  They might work.  For example, on some kernels, choosing a syscall
  that only exists in future kernels will be correctly emulated (by
  returning ``-ENOSYS``).
  (返回值为 ``SECCOMP_RET_TRACE`` 时会照常通知跟踪器，但不能用 orig_rax
  寄存器把该系统调用改成另一个系统调用；只能把它改为 -1 以跳过当前被模
  拟的调用。任何其他改动都可能终止进程。跟踪器看到的 rip 将是系统调用
  入口地址，这与正常行为不同。跟踪器绝不能修改 rip 或 rsp。(不要指望用
  其他改动来终止进程——它们可能碰巧有效。例如在某些内核上，选择一个只
  存在于未来内核中的系统调用会被正确地模拟 (返回 ``-ENOSYS``)。))

To detect this quirky behavior, check for ``addr & ~0x0C00 ==
0xFFFFFFFFFF600000``.  (For ``SECCOMP_RET_TRACE``, use rip.  For
``SECCOMP_RET_TRAP``, use ``siginfo->si_call_addr``.)  Do not check any other
condition: future kernels may improve vsyscall emulation and current
kernels in vsyscall=native mode will behave differently, but the
instructions at ``0xF...F600{0,4,8,C}00`` will not be system calls in these
cases.
(要检测这种怪异行为，检查 ``addr & ~0x0C00 == 0xFFFFFFFFFF600000``。
(对于 ``SECCOMP_RET_TRACE`` 使用 rip；对于 ``SECCOMP_RET_TRAP`` 使用
``siginfo->si_call_addr``。) 不要检查任何其他条件：未来的内核可能会改
进 vsyscall 模拟，而处于 vsyscall=native 模式的当前内核行为也会不同，
但在这些情况下 ``0xF...F600{0,4,8,C}00`` 处的指令都不会是系统调用。)

Note that modern systems are unlikely to use vsyscalls at all -- they
are a legacy feature and they are considerably slower than standard
syscalls.  New code will use the vDSO, and vDSO-issued system calls
are indistinguishable from normal system calls.
(注意，现代系统基本不太会使用 vsyscall —— 它是旧式特性，而且比标准系
统调用慢得多。新代码会使用 vDSO，而由 vDSO 发起的系统调用与普通系统调
用无法区分。)
