.. SPDX-License-Identifier: GPL-2.0
.. Copyright © 2024 Microsoft Corporation

=================================
Executability check(可执行性检查)
=================================

The ``AT_EXECVE_CHECK`` :manpage:`execveat(2)` flag, and the
``SECBIT_EXEC_RESTRICT_FILE`` and ``SECBIT_EXEC_DENY_INTERACTIVE`` securebits
are intended for script interpreters and dynamic linkers to enforce a
consistent execution security policy handled by the kernel.  See the
`samples/check-exec/inc.c`_ example.
(``AT_EXECVE_CHECK`` :manpage:`execveat(2)` 标志以及
``SECBIT_EXEC_RESTRICT_FILE`` 和 ``SECBIT_EXEC_DENY_INTERACTIVE`` 安全位
(securebits)，旨在供脚本解释器和动态链接器用来强制执行由内核处理的一致的
执行安全策略。参见 `samples/check-exec/inc.c`_ 示例。)

Whether an interpreter should check these securebits or not depends on the
security risk of running malicious scripts with respect to the execution
environment, and whether the kernel can check if a script is trustworthy or
not.  For instance, Python scripts running on a server can use arbitrary
syscalls and access arbitrary files.  Such interpreters should then be
enlighten to use these securebits and let users define their security policy.
However, a JavaScript engine running in a web browser should already be
sandboxed and then should not be able to harm the user's environment.
(解释器是否应该检查这些安全位，取决于在相应执行环境下运行恶意脚本的安全
风险，以及内核能否检查脚本是否可信。例如，在服务器上运行的 Python 脚本
可以使用任意系统调用并访问任意文件。这样的解释器就应当被启发去使用这些
安全位，并让用户定义自己的安全策略。然而，运行在 Web 浏览器中的
JavaScript 引擎应该已经被沙箱化，因此应该无法危害用户的环境。)

Script interpreters or dynamic linkers built for tailored execution environments
(e.g. hardened Linux distributions or hermetic container images) could use
``AT_EXECVE_CHECK`` without checking the related securebits if backward
compatibility is handled by something else (e.g. atomic update ensuring that
all legitimate libraries are allowed to be executed).  It is then recommended
for script interpreters and dynamic linkers to check the securebits at run time
by default, but also to provide the ability for custom builds to behave like if
``SECBIT_EXEC_RESTRICT_FILE`` or ``SECBIT_EXEC_DENY_INTERACTIVE`` were always
set to 1 (i.e. always enforce restrictions).
(为定制执行环境（例如强化的 Linux 发行版或密封的容器镜像）构建的脚本
解释器或动态链接器，如果向后兼容性由其他机制处理（例如原子更新确保所有
合法库都被允许执行），则可以在不检查相关安全位的情况下使用
``AT_EXECVE_CHECK``。建议脚本解释器和动态链接器默认在运行时检查安全位，
但同时也应提供让定制构建表现得如同 ``SECBIT_EXEC_RESTRICT_FILE`` 或
``SECBIT_EXEC_DENY_INTERACTIVE`` 始终被设置为 1 的能力（即始终强制执行
限制）。)

AT_EXECVE_CHECK(原名)
=====================

Passing the ``AT_EXECVE_CHECK`` flag to :manpage:`execveat(2)` only performs a
check on a regular file and returns 0 if execution of this file would be
allowed, ignoring the file format and then the related interpreter dependencies
(e.g. ELF libraries, script's shebang).
(把 ``AT_EXECVE_CHECK`` 标志传给 :manpage:`execveat(2)` 只会对常规文件执行
检查；如果执行该文件会被允许，则返回 0，此时忽略文件格式以及相关的解释器
依赖（例如 ELF 库、脚本的 shebang）。)

Programs should always perform this check to apply kernel-level checks against
files that are not directly executed by the kernel but passed to a user space
interpreter instead.  All files that contain executable code, from the point of
view of the interpreter, should be checked.  However the result of this check
should only be enforced according to ``SECBIT_EXEC_RESTRICT_FILE`` or
``SECBIT_EXEC_DENY_INTERACTIVE.``.
(程序应始终执行此检查，以便对那些不是由内核直接执行、而是传递给用户空间
解释器的文件应用内核级检查。从解释器的角度看，所有包含可执行代码的文件
都应被检查。但是，此检查的结果只应根据 ``SECBIT_EXEC_RESTRICT_FILE`` 或
``SECBIT_EXEC_DENY_INTERACTIVE.`` 来强制执行。)

The main purpose of this flag is to improve the security and consistency of an
execution environment to ensure that direct file execution (e.g.
``./script.sh``) and indirect file execution (e.g. ``sh script.sh``) lead to
the same result.  For instance, this can be used to check if a file is
trustworthy according to the caller's environment.
(此标志的主要目的是提高执行环境的安全性和一致性，确保直接文件执行（例如
``./script.sh``）和间接文件执行（例如 ``sh script.sh``）产生相同的结果。
例如，这可以用来根据调用者的环境检查文件是否可信。)

In a secure environment, libraries and any executable dependencies should also
be checked.  For instance, dynamic linking should make sure that all libraries
are allowed for execution to avoid trivial bypass (e.g. using ``LD_PRELOAD``).
For such secure execution environment to make sense, only trusted code should
be executable, which also requires integrity guarantees.
(在安全环境中，库和任何可执行的依赖项也应被检查。例如，动态链接应确保
所有库都被允许执行，以避免简单的绕过（例如使用 ``LD_PRELOAD``）。要使这种
安全执行环境有意义，只有受信任的代码才应可执行，这也需要完整性保证。)

To avoid race conditions leading to time-of-check to time-of-use issues,
``AT_EXECVE_CHECK`` should be used with ``AT_EMPTY_PATH`` to check against a
file descriptor instead of a path.
(为避免导致 TOCTOU（time-of-check to time-of-use，检查时刻到使用时刻）
问题的竞态条件，应将 ``AT_EXECVE_CHECK`` 与 ``AT_EMPTY_PATH`` 一起使用，
以针对文件描述符而不是路径进行检查。)

SECBIT_EXEC_RESTRICT_FILE and SECBIT_EXEC_DENY_INTERACTIVE(原名)
================================================================

When ``SECBIT_EXEC_RESTRICT_FILE`` is set, a process should only interpret or
execute a file if a call to :manpage:`execveat(2)` with the related file
descriptor and the ``AT_EXECVE_CHECK`` flag succeed.
(设置 ``SECBIT_EXEC_RESTRICT_FILE`` 后，只有当使用相关文件描述符和
``AT_EXECVE_CHECK`` 标志调用 :manpage:`execveat(2)` 成功时，进程才应解释
或执行该文件。)

This secure bit may be set by user session managers, service managers,
container runtimes, sandboxer tools...  Except for test environments, the
related ``SECBIT_EXEC_RESTRICT_FILE_LOCKED`` bit should also be set.
(此安全位可由用户会话管理器、服务管理器、容器运行时、沙箱工具等设置。
除测试环境外，相关的 ``SECBIT_EXEC_RESTRICT_FILE_LOCKED`` 位也应被
设置。)

Programs should only enforce consistent restrictions according to the
securebits but without relying on any other user-controlled configuration.
Indeed, the use case for these securebits is to only trust executable code
vetted by the system configuration (through the kernel), so we should be
careful to not let untrusted users control this configuration.
(程序应只根据安全位强制执行一致的限制，而不应依赖任何其他用户可控的
配置。实际上，这些安全位的用例是只信任由系统配置（通过内核）审核过的
可执行代码，因此我们要小心，不要让不可信的用户控制这一配置。)

However, script interpreters may still use user configuration such as
environment variables as long as it is not a way to disable the securebits
checks.  For instance, the ``PATH`` and ``LD_PRELOAD`` variables can be set by
a script's caller.  Changing these variables may lead to unintended code
executions, but only from vetted executable programs, which is OK.  For this to
make sense, the system should provide a consistent security policy to avoid
arbitrary code execution e.g., by enforcing a write xor execute policy.
(不过，脚本解释器仍然可以使用用户配置（例如环境变量），只要它不是用来
禁用安全位检查的手段。例如，``PATH`` 和 ``LD_PRELOAD`` 变量可以由脚本的
调用者设置。更改这些变量可能导致意外的代码执行，但只会来自经过审核的
可执行程序，这是可以接受的。为使这一点有意义，系统应提供一致的安全策略
以避免任意代码执行，例如通过强制执行 "写异或执行" (write xor execute)
策略。)

When ``SECBIT_EXEC_DENY_INTERACTIVE`` is set, a process should never interpret
interactive user commands (e.g. scripts).  However, if such commands are passed
through a file descriptor (e.g. stdin), its content should be interpreted if a
call to :manpage:`execveat(2)` with the related file descriptor and the
``AT_EXECVE_CHECK`` flag succeed.
(设置 ``SECBIT_EXEC_DENY_INTERACTIVE`` 后，进程绝不应解释交互式用户命令
（例如脚本）。但是，如果此类命令通过文件描述符（例如 stdin）传入，那么
当使用相关文件描述符和 ``AT_EXECVE_CHECK`` 标志调用 :manpage:`execveat(2)`
成功时，其内容应被解释。)

For instance, script interpreters called with a script snippet as argument
should always deny such execution if ``SECBIT_EXEC_DENY_INTERACTIVE`` is set.
(例如，以脚本片段作为参数调用的脚本解释器，在设置了
``SECBIT_EXEC_DENY_INTERACTIVE`` 时应始终拒绝此类执行。)

This secure bit may be set by user session managers, service managers,
container runtimes, sandboxer tools...  Except for test environments, the
related ``SECBIT_EXEC_DENY_INTERACTIVE_LOCKED`` bit should also be set.
(此安全位可由用户会话管理器、服务管理器、容器运行时、沙箱工具等设置。
除测试环境外，相关的 ``SECBIT_EXEC_DENY_INTERACTIVE_LOCKED`` 位也应被
设置。)

Here is the expected behavior for a script interpreter according to combination
of any exec securebits:
(下面是根据任意 exec 安全位组合，脚本解释器的预期行为：)

1. ``SECBIT_EXEC_RESTRICT_FILE=0`` and ``SECBIT_EXEC_DENY_INTERACTIVE=0``

   Always interpret scripts, and allow arbitrary user commands (default).

   No threat, everyone and everything is trusted, but we can get ahead of
   potential issues thanks to the call to :manpage:`execveat(2)` with
   ``AT_EXECVE_CHECK`` which should always be performed but ignored by the
   script interpreter.  Indeed, this check is still important to enable systems
   administrators to verify requests (e.g. with audit) and prepare for
   migration to a secure mode.
   (始终解释脚本，并允许任意用户命令（默认）。
   没有威胁，任何人和任何东西都被信任，但得益于始终执行却被脚本解释器
   忽略的 ``AT_EXECVE_CHECK`` :manpage:`execveat(2)` 调用，我们可以抢先
   处理潜在问题。实际上，这一检查仍然很重要，它使系统管理员能够核查请求
   （例如通过 audit），并为迁移到安全模式做准备。)

2. ``SECBIT_EXEC_RESTRICT_FILE=1`` and ``SECBIT_EXEC_DENY_INTERACTIVE=0``

   Deny script interpretation if they are not executable, but allow
   arbitrary user commands.

   The threat is (potential) malicious scripts run by trusted (and not fooled)
   users.  That can protect against unintended script executions (e.g. ``sh
   /tmp/*.sh``).  This makes sense for (semi-restricted) user sessions.
   (拒绝解释不可执行的脚本，但允许任意用户命令。
   威胁是受信任（且未被愚弄）的用户运行（潜在的）恶意脚本。这可以防止
   意外的脚本执行（例如 ``sh /tmp/*.sh``）。这对（半受限的）用户会话
   有意义。)

3. ``SECBIT_EXEC_RESTRICT_FILE=0`` and ``SECBIT_EXEC_DENY_INTERACTIVE=1``

   Always interpret scripts, but deny arbitrary user commands.

   This use case may be useful for secure services (i.e. without interactive
   user session) where scripts' integrity is verified (e.g.  with IMA/EVM or
   dm-verity/IPE) but where access rights might not be ready yet.  Indeed,
   arbitrary interactive commands would be much more difficult to check.
   (始终解释脚本，但拒绝任意用户命令。
   此用例可能对安全服务（即没有交互式用户会话）有用：在这些服务中脚本
   的完整性已得到验证（例如通过 IMA/EVM 或 dm-verity/IPE），但访问权限
   可能尚未就绪。实际上，任意交互式命令要检查起来会困难得多。)

4. ``SECBIT_EXEC_RESTRICT_FILE=1`` and ``SECBIT_EXEC_DENY_INTERACTIVE=1``

   Deny script interpretation if they are not executable, and also deny
   any arbitrary user commands.

   The threat is malicious scripts run by untrusted users (but trusted code).
   This makes sense for system services that may only execute trusted scripts.
   (拒绝解释不可执行的脚本，同时拒绝任何任意用户命令。
   威胁是不可信用户（但代码受信任）运行的恶意脚本。这对可能只执行受信任
   脚本的系统服务有意义。)

.. Links
.. _samples/check-exec/inc.c:
   https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/samples/check-exec/inc.c
