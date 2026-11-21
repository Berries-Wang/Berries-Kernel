.. SPDX-License-Identifier: GPL-2.0

=======================================================
Introduction of non-executable mfd(不可执行 mfd 的引入)
=======================================================
:Author:
    Daniel Verkamp <dverkamp@chromium.org>
    Jeff Xu <jeffxu@chromium.org>
    (作者：Daniel Verkamp <dverkamp@chromium.org>、Jeff Xu <jeffxu@chromium.org>)

:Contributor:
	Aleksa Sarai <cyphar@cyphar.com>
	(贡献者：Aleksa Sarai <cyphar@cyphar.com>)

Since Linux introduced the memfd feature, memfds have always had their
execute bit set, and the memfd_create() syscall doesn't allow setting
it differently.
(自 Linux 引入 memfd 特性以来，memfd 一直被置位可执行位，而
memfd_create() 系统调用不允许以其他方式设置它。)

However, in a secure-by-default system, such as ChromeOS, (where all
executables should come from the rootfs, which is protected by verified
boot), this executable nature of memfd opens a door for NoExec bypass
and enables “confused deputy attack”.  E.g, in VRP bug [1]: cros_vm
process created a memfd to share the content with an external process,
however the memfd is overwritten and used for executing arbitrary code
and root escalation. [2] lists more VRP of this kind.
(然而，在默认安全的系统 (例如 ChromeOS，其中所有可执行文件都应来自受
验证启动保护的 rootfs) 中，memfd 的这种可执行性质为绕过 NoExec 打开了
大门，并助长了"被迷惑的代理人攻击" (confused deputy attack)。例如，在
VRP bug [1] 中：cros_vm 进程创建了一个 memfd 来与外部进程共享内容，然而
该 memfd 被覆写并用于执行任意代码和提升权限。[2] 列出了更多此类 VRP。)

On the other hand, executable memfd has its legit use: runc uses memfd’s
seal and executable feature to copy the contents of the binary then
execute them. For such a system, we need a solution to differentiate runc's
use of executable memfds and an attacker's [3].
(另一方面，可执行的 memfd 也有其正当用途：runc 利用 memfd 的封印 (seal)
与可执行特性来复制二进制的内容然后执行它们。对于这样的系统，我们需要
一种方案来区分 runc 对可执行 memfd 的使用与攻击者的使用 [3]。)

To address those above:
(为解决上述问题：)
 - Let memfd_create() set X bit at creation time.
   (让 memfd_create() 在创建时置位 X 位。)
 - Let memfd be sealed for modifying X bit when NX is set.
   (当设置了 NX 时，将 memfd 封印以防修改 X 位。)
 - Add a new pid namespace sysctl: vm.memfd_noexec to help applications in
   migrating and enforcing non-executable MFD.
   (新增一个 pid namespace 的 sysctl：vm.memfd_noexec，以帮助应用程序迁移
   并强制实施不可执行的 MFD。)

User API(用户空间 API)
======================
``int memfd_create(const char *name, unsigned int flags)``
(memfd_create 系统调用原型。)

``MFD_NOEXEC_SEAL``
	When MFD_NOEXEC_SEAL bit is set in the ``flags``, memfd is created
	with NX. F_SEAL_EXEC is set and the memfd can't be modified to
	add X later. MFD_ALLOW_SEALING is also implied.
	This is the most common case for the application to use memfd.
	(当在 ``flags`` 中置位 MFD_NOEXEC_SEAL 位时，memfd 以 NX 创建。
	F_SEAL_EXEC 会被设置，此后 memfd 不能再被修改以添加 X。同时隐含
	MFD_ALLOW_SEALING。这是应用程序使用 memfd 最常见的情况。)

``MFD_EXEC``
	When MFD_EXEC bit is set in the ``flags``, memfd is created with X.
	(当在 ``flags`` 中置位 MFD_EXEC 位时，memfd 以 X 创建。)

Note:
	``MFD_NOEXEC_SEAL`` implies ``MFD_ALLOW_SEALING``. In case that
	an app doesn't want sealing, it can add F_SEAL_SEAL after creation.
	(注意：``MFD_NOEXEC_SEAL`` 隐含 ``MFD_ALLOW_SEALING``。如果应用
	程序不想要封印功能，可以在创建之后添加 F_SEAL_SEAL。)


Sysctl:(Sysctl)
===============
``pid namespaced sysctl vm.memfd_noexec``
(基于 pid namespace 的 sysctl：vm.memfd_noexec。)

The new pid namespaced sysctl vm.memfd_noexec has 3 values:
(新的基于 pid namespace 的 sysctl vm.memfd_noexec 有 3 个取值：)

 - 0: MEMFD_NOEXEC_SCOPE_EXEC
	memfd_create() without MFD_EXEC nor MFD_NOEXEC_SEAL acts like
	MFD_EXEC was set.
	(既不设置 MFD_EXEC 也不设置 MFD_NOEXEC_SEAL 的 memfd_create()
	行为如同设置了 MFD_EXEC。)

 - 1: MEMFD_NOEXEC_SCOPE_NOEXEC_SEAL
	memfd_create() without MFD_EXEC nor MFD_NOEXEC_SEAL acts like
	MFD_NOEXEC_SEAL was set.
	(既不设置 MFD_EXEC 也不设置 MFD_NOEXEC_SEAL 的 memfd_create()
	行为如同设置了 MFD_NOEXEC_SEAL。)

 - 2: MEMFD_NOEXEC_SCOPE_NOEXEC_ENFORCED
	memfd_create() without MFD_NOEXEC_SEAL will be rejected.
	(未设置 MFD_NOEXEC_SEAL 的 memfd_create() 将被拒绝。)

The sysctl allows finer control of memfd_create for old software that
doesn't set the executable bit; for example, a container with
vm.memfd_noexec=1 means the old software will create non-executable memfd
by default while new software can create executable memfd by setting
MFD_EXEC.
(该 sysctl 允许对不设置可执行位的旧软件进行更精细的 memfd_create 控制；
例如，容器设置 vm.memfd_noexec=1 意味着旧软件默认创建不可执行的 memfd，
而新软件可以通过设置 MFD_EXEC 创建可执行的 memfd。)

The value of vm.memfd_noexec is passed to child namespace at creation
time. In addition, the setting is hierarchical, i.e. during memfd_create,
we will search from current ns to root ns and use the most restrictive
setting.
(vm.memfd_noexec 的值会在创建时传递给子 namespace。此外，该设置是分层
的，即在 memfd_create 期间，我们会从当前 ns 向根 ns 搜索并采用最严格的
设置。)

[1] https://crbug.com/1305267

[2] https://bugs.chromium.org/p/chromium/issues/list?q=type%3Dbug-security%20memfd%20escalation&can=1

[3] https://lwn.net/Articles/781013/
