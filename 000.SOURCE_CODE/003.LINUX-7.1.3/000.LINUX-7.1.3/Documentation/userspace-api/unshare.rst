unshare system call(unshare 系统调用)
=====================================

This document describes the new system call, unshare(). The document
provides an overview of the feature, why it is needed, how it can
be used, its interface specification, design, implementation and
how it can be tested.
(本文档描述新的系统调用 unshare()。文档概述该特性、其必要性、使用方式、接口规格、设计、实现以及测试方法。)

Change Log(变更记录)
--------------------
version 0.1  Initial document, Janak Desai (janak@us.ibm.com), Jan 11, 2006
(version 0.1  初始文档，Janak Desai (janak@us.ibm.com)，2006 年 1 月 11 日)

Contents(目录)
--------------
	1) Overview(概述)
	2) Benefits(收益)
	3) Cost(代价)
	4) Requirements(需求)
	5) Functional Specification(功能规格)
	6) High Level Design(高层设计)
	7) Low Level Design(低层设计)
	8) Test Specification(测试规格)
	9) Future Work(后续工作)

1) Overview(概述)
-----------------

Most legacy operating system kernels support an abstraction of threads
as multiple execution contexts within a process. These kernels provide
special resources and mechanisms to maintain these "threads". The Linux
kernel, in a clever and simple manner, does not make distinction
between processes and "threads". The kernel allows processes to share
resources and thus they can achieve legacy "threads" behavior without
requiring additional data structures and mechanisms in the kernel. The
power of implementing threads in this manner comes not only from
its simplicity but also from allowing application programmers to work
outside the confinement of all-or-nothing shared resources of legacy
threads. On Linux, at the time of thread creation using the clone system
call, applications can selectively choose which resources to share
between threads.
(大多数传统操作系统内核把线程抽象为进程内的多个执行上下文，并提供专门的资源与机制来维护这些"线程"。Linux 内核以一种巧妙而简单的方式，不区分进程与"线程"。内核允许进程之间共享资源，因此无需在内核中增加额外的数据结构和机制就能实现传统"线程"的行为。以这种方式实现线程的好处不仅在于简单，还在于允许应用程序程序员摆脱传统线程"全有或全无"共享资源的束缚。在 Linux 上，使用 clone 系统调用创建线程时，应用程序可以有选择地挑选线程之间要共享哪些资源。)

unshare() system call adds a primitive to the Linux thread model that
allows threads to selectively 'unshare' any resources that were being
shared at the time of their creation. unshare() was conceptualized by
Al Viro in the August of 2000, on the Linux-Kernel mailing list, as part
of the discussion on POSIX threads on Linux.  unshare() augments the
usefulness of Linux threads for applications that would like to control
shared resources without creating a new process. unshare() is a natural
addition to the set of available primitives on Linux that implement
the concept of process/thread as a virtual machine.
(unshare() 系统调用为 Linux 线程模型增加了一个原语，让线程可以有选择地"取消共享"（unshare）创建时正在共享的任何资源。unshare() 由 Al Viro 于 2000 年 8 月在 Linux-Kernel 邮件列表上关于 Linux POSIX 线程的讨论中提出。对于想在不创建新进程的情况下控制共享资源的应用程序，unshare() 增强了 Linux 线程的实用性。unshare() 是 Linux 上实现"把进程/线程当作虚拟机"这一概念的现有原语集合的自然补充。)

2) Benefits(收益)
-----------------

unshare() would be useful to large application frameworks such as PAM
where creating a new process to control sharing/unsharing of process
resources is not possible. Since namespaces are shared by default
when creating a new process using fork or clone, unshare() can benefit
even non-threaded applications if they have a need to disassociate
from default shared namespace. The following lists two use-cases
where unshare() can be used.
(unshare() 对 PAM 这样的大型应用框架很有用，因为它们不可能通过创建新进程来控制进程资源的共享/取消共享。由于用 fork 或 clone 创建新进程时命名空间默认是共享的，即使是非多线程应用程序，如果需要脱离默认共享的命名空间，也能从 unshare() 中受益。下面列出 unshare() 可用的两个用例。)

2.1 Per-security context namespaces(基于安全上下文的命名空间)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

unshare() can be used to implement polyinstantiated directories using
the kernel's per-process namespace mechanism. Polyinstantiated directories,
such as per-user and/or per-security context instance of /tmp, /var/tmp or
per-security context instance of a user's home directory, isolate user
processes when working with these directories. Using unshare(), a PAM
module can easily setup a private namespace for a user at login.
Polyinstantiated directories are required for Common Criteria certification
with Labeled System Protection Profile, however, with the availability
of shared-tree feature in the Linux kernel, even regular Linux systems
can benefit from setting up private namespaces at login and
polyinstantiating /tmp, /var/tmp and other directories deemed
appropriate by system administrators.
(unshare() 可以利用内核的每进程命名空间机制来实现多实例化目录（polyinstantiated directories）。多实例化目录——例如 /tmp、/var/tmp 的每用户和/或每安全上下文实例，或用户主目录的每安全上下文实例——在用户进程使用这些目录时将其相互隔离。使用 unshare()，PAM 模块可以在登录时轻松为用户建立私有命名空间。多实例化目录是通过 Common Criteria 认证（Labeled System Protection Profile）所要求的；不过，得益于 Linux 内核的共享树（shared-tree）特性，即使是普通的 Linux 系统，也可以从"登录时建立私有命名空间并多实例化 /tmp、/var/tmp 以及系统管理员认为合适的其他目录"中受益。)

2.2 unsharing of virtual memory and/or open files(取消共享虚拟内存和/或打开的文件)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Consider a client/server application where the server is processing
client requests by creating processes that share resources such as
virtual memory and open files. Without unshare(), the server has to
decide what needs to be shared at the time of creating the process
which services the request. unshare() allows the server an ability to
disassociate parts of the context during the servicing of the
request. For large and complex middleware application frameworks, this
ability to unshare() after the process was created can be very
useful.
(考虑一个客户机/服务器应用：服务器通过创建共享虚拟内存、打开的文件等资源的进程来处理客户请求。没有 unshare() 时，服务器必须在创建服务该请求的进程时就决定要共享什么。unshare() 让服务器能够在处理请求的过程中取消与部分执行上下文的关联。对于大型复杂的中间件应用框架来说，这种"在进程创建之后再 unshare()"的能力非常有用。)

3) Cost(代价)
-------------

In order to not duplicate code and to handle the fact that unshare()
works on an active task (as opposed to clone/fork working on a newly
allocated inactive task) unshare() had to make minor reorganizational
changes to copy_* functions utilized by clone/fork system call.
There is a cost associated with altering existing, well tested and
stable code to implement a new feature that may not get exercised
extensively in the beginning. However, with proper design and code
review of the changes and creation of an unshare() test for the LTP
the benefits of this new feature can exceed its cost.
(为了避免代码重复，并处理 unshare() 作用于活动 task（而 clone/fork 作用于新分配的非活动 task）这一事实，unshare() 不得不对 clone/fork 系统调用所用的 copy_* 函数做少量重组改动。改动现有经过充分测试且稳定的代码来实现一个初期可能不会被大量使用的新特性，是有代价的。不过，只要对改动进行适当的设计与代码评审，并为 LTP 创建 unshare() 测试，这一新特性的收益就能超过其代价。)

4) Requirements(需求)
---------------------

unshare() reverses sharing that was done using clone(2) system call,
so unshare() should have a similar interface as clone(2). That is,
since flags in clone(int flags, void \*stack) specifies what should
be shared, similar flags in unshare(int flags) should specify
what should be unshared. Unfortunately, this may appear to invert
the meaning of the flags from the way they are used in clone(2).
However, there was no easy solution that was less confusing and that
allowed incremental context unsharing in future without an ABI change.
(unshare() 逆转的是用 clone(2) 系统调用建立的共享，因此 unshare() 应当具有与 clone(2) 类似的接口。也就是说，clone(int flags, void \*stack) 中的 flags 指定要共享什么，unshare(int flags) 中类似的 flags 就应指定要取消共享什么。遗憾的是，这看起来像是把 flags 相对 clone(2) 中的用法反了过来。然而，当时没有既不那么令人困惑、又允许将来在不做 ABI 变更的情况下增量取消共享上下文的更简单方案。)

unshare() interface should accommodate possible future addition of
new context flags without requiring a rebuild of old applications.
If and when new context flags are added, unshare() design should allow
incremental unsharing of those resources on an as needed basis.
(unshare() 的接口应当能容纳将来可能新增的上下文 flags，而不需要重新编译旧应用程序。当新的上下文 flags 加入时，unshare() 的设计应支持按需对这些资源做增量取消共享。)

5) Functional Specification(功能规格)
-------------------------------------

NAME
	unshare - disassociate parts of the process execution context
	(NAME - 与进程执行上下文的部分内容解除关联)

SYNOPSIS
	#include <sched.h>

	int unshare(int flags);
	(概要：#include <sched.h>；int unshare(int flags);)

DESCRIPTION
	unshare() allows a process to disassociate parts of its execution
	context that are currently being shared with other processes. Part
	of execution context, such as the namespace, is shared by default
	when a new process is created using fork(2), while other parts,
	such as the virtual memory, open file descriptors, etc, may be
	shared by explicit request to share them when creating a process
	using clone(2).
	(描述：unshare() 允许进程将其执行上下文中当前正与其他进程共享的部分解除关联。执行上下文的某些部分——例如命名空间——在用 fork(2) 创建新进程时默认共享；而另一些部分，如虚拟内存、打开的文件描述符等，则只有在用 clone(2) 创建进程时显式请求共享才会共享。)

	The main use of unshare() is to allow a process to control its
	shared execution context without creating a new process.
	(unshare() 的主要用途是让进程无需创建新进程就能控制自己共享的执行上下文。)

	The flags argument specifies one or bitwise-or'ed of several of
	the following constants.
	(flags 参数指定下列常量中的一个，或多个按位或的组合。)

	CLONE_FS
		If CLONE_FS is set, file system information of the caller
		is disassociated from the shared file system information.
		(如果设置了 CLONE_FS，调用者的文件系统信息将与共享的文件系统信息解除关联。)

	CLONE_FILES
		If CLONE_FILES is set, the file descriptor table of the
		caller is disassociated from the shared file descriptor
		table.
		(如果设置了 CLONE_FILES，调用者的文件描述符表将与共享的文件描述符表解除关联。)

	CLONE_NEWNS
		If CLONE_NEWNS is set, the namespace of the caller is
		disassociated from the shared namespace.
		(如果设置了 CLONE_NEWNS，调用者的命名空间将与共享的命名空间解除关联。)

	CLONE_VM
		If CLONE_VM is set, the virtual memory of the caller is
		disassociated from the shared virtual memory.
		(如果设置了 CLONE_VM，调用者的虚拟内存将与共享的虚拟内存解除关联。)

RETURN VALUE
	On success, zero returned. On failure, -1 is returned and errno is
	(返回值：成功时返回 0。失败时返回 -1 并设置 errno。)

ERRORS
	EPERM	CLONE_NEWNS was specified by a non-root process (process
		without CAP_SYS_ADMIN).
	(错误：EPERM——非 root 进程（没有 CAP_SYS_ADMIN 的进程）指定了 CLONE_NEWNS。)

	ENOMEM	Cannot allocate sufficient memory to copy parts of caller's
		context that need to be unshared.
	(ENOMEM——无法分配足够的内存来复制调用者上下文中需要取消共享的部分。)

	EINVAL	Invalid flag was specified as an argument.
	(EINVAL——作为参数指定的 flag 无效。)

CONFORMING TO
	The unshare() call is Linux-specific and  should  not be used
	in programs intended to be portable.
	(遵循标准：unshare() 调用是 Linux 特有的，不应在追求可移植性的程序中使用。)

SEE ALSO
	clone(2), fork(2)
	(另见：clone(2)、fork(2))

6) High Level Design(高层设计)
------------------------------

Depending on the flags argument, the unshare() system call allocates
appropriate process context structures, populates it with values from
the current shared version, associates newly duplicated structures
with the current task structure and releases corresponding shared
versions. Helper functions of clone (copy_*) could not be used
directly by unshare() because of the following two reasons.
(根据 flags 参数，unshare() 系统调用分配相应的进程上下文结构，用当前共享版本的值填充它，把新复制的结构关联到当前 task 结构，并释放对应的共享版本。unshare() 不能直接使用 clone 的辅助函数（copy_*），原因有二：)

  1) clone operates on a newly allocated not-yet-active task
     structure, where as unshare() operates on the current active
     task. Therefore unshare() has to take appropriate task_lock()
     before associating newly duplicated context structures
     (1) clone 作用于新分配的、尚未活动的 task 结构，而 unshare() 作用于当前活动的 task。因此 unshare() 在关联新复制的上下文结构之前必须先适当地执行 task_lock()。)

  2) unshare() has to allocate and duplicate all context structures
     that are being unshared, before associating them with the
     current task and releasing older shared structures. Failure
     do so will create race conditions and/or oops when trying
     to backout due to an error. Consider the case of unsharing
     both virtual memory and namespace. After successfully unsharing
     vm, if the system call encounters an error while allocating
     new namespace structure, the error return code will have to
     reverse the unsharing of vm. As part of the reversal the
     system call will have to go back to older, shared, vm
     structure, which may not exist anymore.
     (2) unshare() 必须先分配并复制所有要取消共享的上下文结构，然后才把它们关联到当前 task 并释放旧的共享结构。不这样做的话，在因出错而回退时会产生竞态条件和/或 oops。考虑同时取消共享虚拟内存和命名空间的情形：成功取消共享 vm 之后，如果系统调用在分配新命名空间结构时遇到错误，错误返回路径将不得不撤销对 vm 的取消共享；而撤销时系统调用必须退回旧的、共享的 vm 结构，但它可能已不存在了。)

Therefore code from copy_* functions that allocated and duplicated
current context structure was moved into new dup_* functions. Now,
copy_* functions call dup_* functions to allocate and duplicate
appropriate context structures and then associate them with the
task structure that is being constructed. unshare() system call on
the other hand performs the following:
(因此，把 copy_* 函数中分配并复制当前上下文结构的代码移到了新的 dup_* 函数中。现在，copy_* 函数调用 dup_* 函数来分配并复制相应的上下文结构，然后把它们关联到正在构造的 task 结构上。而 unshare() 系统调用则执行以下步骤：)

  1) Check flags to force missing, but implied, flags
     (1) 检查 flags，强制补上缺失但被隐含的 flags。)

  2) For each context structure, call the corresponding unshare()
     helper function to allocate and duplicate a new context
     structure, if the appropriate bit is set in the flags argument.
     (2) 对每个上下文结构，如果 flags 参数中设置了相应的位，就调用对应的 unshare() 辅助函数来分配并复制新的上下文结构。)

  3) If there is no error in allocation and duplication and there
     are new context structures then lock the current task structure,
     associate new context structures with the current task structure,
     and release the lock on the current task structure.
     (3) 如果分配与复制没有出错且存在新的上下文结构，则锁定当前 task 结构，把新上下文结构关联到当前 task 结构，然后释放锁。)

  4) Appropriately release older, shared, context structures.
     (4) 恰当地释放旧的、共享的上下文结构。)

7) Low Level Design(低层设计)
-----------------------------

Implementation of unshare() can be grouped in the following 4 different
items:
(unshare() 的实现可以归为以下 4 个部分：)

  a) Reorganization of existing copy_* functions
     (a) 重组现有的 copy_* 函数。)

  b) unshare() system call service function
     (b) unshare() 系统调用服务函数。)

  c) unshare() helper functions for each different process context
     (c) 针对每种不同进程上下文的 unshare() 辅助函数。)

  d) Registration of system call number for different architectures
     (d) 为不同体系结构注册系统调用号。)

7.1) Reorganization of copy_* functions(copy_* 函数的重组)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Each copy function such as copy_mm, copy_namespace, copy_files,
etc, had roughly two components. The first component allocated
and duplicated the appropriate structure and the second component
linked it to the task structure passed in as an argument to the copy
function. The first component was split into its own function.
These dup_* functions allocated and duplicated the appropriate
context structure. The reorganized copy_* functions invoked
their corresponding dup_* functions and then linked the newly
duplicated structures to the task structure with which the
copy function was called.
(每个 copy 函数，如 copy_mm、copy_namespace、copy_files 等，大致包含两个部分：第一部分分配并复制相应的结构，第二部分把它链接到作为参数传入 copy 函数的 task 结构上。第一部分被拆分成独立的函数——这些 dup_* 函数负责分配并复制相应的上下文结构。重组后的 copy_* 函数调用各自对应的 dup_* 函数，然后把新复制的结构链接到调用 copy 函数时所传入的 task 结构上。)

7.2) unshare() system call service function(unshare() 系统调用服务函数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

       * Check flags
	 Force implied flags. If CLONE_THREAD is set force CLONE_VM.
	 If CLONE_VM is set, force CLONE_SIGHAND. If CLONE_SIGHAND is
	 set and signals are also being shared, force CLONE_THREAD. If
	 CLONE_NEWNS is set, force CLONE_FS.
         (检查 flags：强制补上被隐含的 flags。如果设置了 CLONE_THREAD 则强制 CLONE_VM；如果设置了 CLONE_VM 则强制 CLONE_SIGHAND；如果设置了 CLONE_SIGHAND 且信号也被共享，则强制 CLONE_THREAD；如果设置了 CLONE_NEWNS 则强制 CLONE_FS。)

       * For each context flag, invoke the corresponding unshare_*
	 helper routine with flags passed into the system call and a
	 reference to pointer pointing the new unshared structure
         (对每个上下文 flag，把传入系统调用的 flags 和指向"新取消共享结构"的指针的引用传给对应的 unshare_* 辅助例程。)

       * If any new structures are created by unshare_* helper
	 functions, take the task_lock() on the current task,
	 modify appropriate context pointers, and release the
         task lock.
         (如果 unshare_* 辅助函数创建了任何新结构，就对当前 task 执行 task_lock()，修改相应的上下文指针，然后释放 task 锁。)

       * For all newly unshared structures, release the corresponding
         older, shared, structures.
         (对所有新取消共享的结构，释放对应的旧的共享结构。)

7.3) unshare_* helper functions(unshare_* 辅助函数)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For unshare_* helpers corresponding to CLONE_SYSVSEM, CLONE_SIGHAND,
and CLONE_THREAD, return -EINVAL since they are not implemented yet.
For others, check the flag value to see if the unsharing is
required for that structure. If it is, invoke the corresponding
dup_* function to allocate and duplicate the structure and return
a pointer to it.
(对应 CLONE_SYSVSEM、CLONE_SIGHAND 和 CLONE_THREAD 的 unshare_* 辅助函数尚未实现，返回 -EINVAL。对其他结构，检查 flag 值以判断是否需要对该结构取消共享；如果需要，就调用相应的 dup_* 函数来分配并复制该结构，并返回指向它的指针。)

7.4) Finally(最后)
~~~~~~~~~~~~~~~~~~

Appropriately modify architecture specific code to register the
new system call.
(适当地修改体系结构相关代码，注册新的系统调用。)

8) Test Specification(测试规格)
-------------------------------

The test for unshare() should test the following:
(unshare() 的测试应覆盖以下几项：)

  1) Valid flags: Test to check that clone flags for signal and
     signal handlers, for which unsharing is not implemented
     yet, return -EINVAL.
     (1) 有效 flags：测试针对信号与信号处理程序的 clone flags（尚未实现其取消共享）返回 -EINVAL。)

  2) Missing/implied flags: Test to make sure that if unsharing
     namespace without specifying unsharing of filesystem, correctly
     unshares both namespace and filesystem information.
     (2) 缺失/隐含 flags：测试确保在不指定取消共享文件系统的情况下取消共享命名空间时，命名空间与文件系统信息都被正确取消共享。)

  3) For each of the four (namespace, filesystem, files and vm)
     supported unsharing, verify that the system call correctly
     unshares the appropriate structure. Verify that unsharing
     them individually as well as in combination with each
     other works as expected.
     (3) 对四种受支持的取消共享（命名空间、文件系统、文件和 vm）逐个验证系统调用正确取消共享相应的结构；并验证单独以及相互组合地取消共享均符合预期。)

  4) Concurrent execution: Use shared memory segments and futex on
     an address in the shm segment to synchronize execution of
     about 10 threads. Have a couple of threads execute execve,
     a couple _exit and the rest unshare with different combination
     of flags. Verify that unsharing is performed as expected and
     that there are no oops or hangs.
     (4) 并发执行：用共享内存段以及在 shm 段内某地址上的 futex 来同步约 10 个线程的执行。让几个线程执行 execve，几个执行 _exit，其余用不同的 flag 组合执行 unshare。验证取消共享按预期执行，且没有 oops 或挂起。)

9) Future Work(后续工作)
------------------------

The current implementation of unshare() does not allow unsharing of
signals and signal handlers. Signals are complex to begin with and
to unshare signals and/or signal handlers of a currently running
process is even more complex. If in the future there is a specific
need to allow unsharing of signals and/or signal handlers, it can
be incrementally added to unshare() without affecting legacy
applications using unshare().
(当前的 unshare() 实现不支持取消共享信号与信号处理程序。信号本身就复杂，而取消共享一个正在运行进程的信号和/或信号处理程序更为复杂。如果将来确有需要支持取消共享信号和/或信号处理程序，可以在不影响使用 unshare() 的既有应用程序的前提下，把它增量地加入 unshare()。)

