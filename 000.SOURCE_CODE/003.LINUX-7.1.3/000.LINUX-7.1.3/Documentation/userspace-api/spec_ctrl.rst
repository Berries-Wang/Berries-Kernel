=================================
Speculation Control(推测执行控制)
=================================

Quite some CPUs have speculation-related misfeatures which are in
fact vulnerabilities causing data leaks in various forms even across
privilege domains.
(相当多的 CPU 具有与推测执行相关的不良特性（misfeature），它们实际上是各种形式的数据泄露漏洞，甚至可以跨越特权域。)

The kernel provides mitigation for such vulnerabilities in various
forms. Some of these mitigations are compile-time configurable and some
can be supplied on the kernel command line.
(内核以各种形式提供针对这类漏洞的缓解措施。其中一些缓解措施可在编译时配置，一些可在内核命令行上提供。)

There is also a class of mitigations which are very expensive, but they can
be restricted to a certain set of processes or tasks in controlled
environments. The mechanism to control these mitigations is via
:manpage:`prctl(2)`.
(还有一类缓解措施非常昂贵，但在受控环境中可以将它们限制在特定的一组进程或任务上。控制这些缓解措施的机制是通过 :manpage:`prctl(2)` 。)

There are two prctl options which are related to this:
(与此相关的 prctl 选项有两个：)

 * PR_GET_SPECULATION_CTRL

 * PR_SET_SPECULATION_CTRL

PR_GET_SPECULATION_CTRL
-----------------------

PR_GET_SPECULATION_CTRL returns the state of the speculation misfeature
which is selected with arg2 of prctl(2). The return value uses bits 0-3 with
the following meaning (with the caveat that PR_SPEC_L1D_FLUSH has less obvious
semantics, see documentation for that specific control below):
(PR_GET_SPECULATION_CTRL 返回由 prctl(2) 的 arg2 所选择的推测执行缺陷的状态。返回值使用位 0-3，含义如下（注意 PR_SPEC_L1D_FLUSH 的语义不太直观，参见下文该特定控制的文档）：)

==== ====================== ==================================================
Bit  Define                 Description
==== ====================== ==================================================
0    PR_SPEC_PRCTL          Mitigation can be controlled per task by
                            PR_SET_SPECULATION_CTRL.
                            (缓解措施可以通过 PR_SET_SPECULATION_CTRL 按任务控制。)
1    PR_SPEC_ENABLE         The speculation feature is enabled, mitigation is
                            disabled.
                            (推测特性已启用，缓解措施被禁用。)
2    PR_SPEC_DISABLE        The speculation feature is disabled, mitigation is
                            enabled.
                            (推测特性被禁用，缓解措施已启用。)
3    PR_SPEC_FORCE_DISABLE  Same as PR_SPEC_DISABLE, but cannot be undone. A
                            subsequent prctl(..., PR_SPEC_ENABLE) will fail.
                            (与 PR_SPEC_DISABLE 相同，但无法撤销。后续的 prctl(..., PR_SPEC_ENABLE) 将失败。)
4    PR_SPEC_DISABLE_NOEXEC Same as PR_SPEC_DISABLE, but the state will be
                            cleared on :manpage:`execve(2)`.
                            (与 PR_SPEC_DISABLE 相同，但该状态会在 :manpage:`execve(2)` 时被清除。)
==== ====================== ==================================================

If all bits are 0 the CPU is not affected by the speculation misfeature.
(如果所有位都为 0，则 CPU 不受该推测执行缺陷的影响。)

If PR_SPEC_PRCTL is set, then the per-task control of the mitigation is
available. If not set, prctl(PR_SET_SPECULATION_CTRL) for the speculation
misfeature will fail.
(如果设置了 PR_SPEC_PRCTL ，则可以按任务控制缓解措施。如果未设置，针对该推测执行缺陷的 prctl(PR_SET_SPECULATION_CTRL) 将失败。)

.. _set_spec_ctrl:

PR_SET_SPECULATION_CTRL
-----------------------

PR_SET_SPECULATION_CTRL allows to control the speculation misfeature, which
is selected by arg2 of :manpage:`prctl(2)` per task. arg3 is used to hand
in the control value, i.e. either PR_SPEC_ENABLE or PR_SPEC_DISABLE or
PR_SPEC_FORCE_DISABLE.
(PR_SET_SPECULATION_CTRL 允许按任务控制由 :manpage:`prctl(2)` 的 arg2 选择的推测执行缺陷。arg3 用于传入控制值，即 PR_SPEC_ENABLE 、 PR_SPEC_DISABLE 或 PR_SPEC_FORCE_DISABLE 。)

Common error codes(常见错误码)
------------------------------
======= =================================================================
Value   Meaning
======= =================================================================
EINVAL  The prctl is not implemented by the architecture or unused
        prctl(2) arguments are not 0.
        (架构未实现该 prctl，或未使用的 prctl(2) 参数不为 0。)

ENODEV  arg2 is selecting a not supported speculation misfeature.
        (arg2 正在选择一个不受支持的推测执行缺陷。)
======= =================================================================

PR_SET_SPECULATION_CTRL error codes(PR_SET_SPECULATION_CTRL 错误码)
-------------------------------------------------------------------
======= =================================================================
Value   Meaning
======= =================================================================
0       Success
        (成功)

ERANGE  arg3 is incorrect, i.e. it's neither PR_SPEC_ENABLE nor
        PR_SPEC_DISABLE nor PR_SPEC_FORCE_DISABLE.
        (arg3 不正确，即它不是 PR_SPEC_ENABLE 、 PR_SPEC_DISABLE 或 PR_SPEC_FORCE_DISABLE 中的任何一个。)

ENXIO   For PR_SPEC_STORE_BYPASS: control of the selected speculation misfeature
        is not possible via prctl, because of the system's boot configuration.
        (对于 PR_SPEC_STORE_BYPASS：由于系统的启动配置，无法通过 prctl 控制所选的推测执行缺陷。)

EPERM   Speculation was disabled with PR_SPEC_FORCE_DISABLE and caller tried to
        enable it again.
        (推测执行已被 PR_SPEC_FORCE_DISABLE 禁用，而调用者试图再次启用它。)

EPERM   For PR_SPEC_L1D_FLUSH and PR_SPEC_INDIRECT_BRANCH: control of the
        mitigation is not possible because of the system's boot configuration.
        (对于 PR_SPEC_L1D_FLUSH 和 PR_SPEC_INDIRECT_BRANCH：由于系统的启动配置，无法控制缓解措施。)

======= =================================================================

Speculation misfeature controls(推测执行缺陷控制)
-------------------------------------------------
- PR_SPEC_STORE_BYPASS: Speculative Store Bypass
  (PR_SPEC_STORE_BYPASS：推测性存储旁路。)

  Invocations: (调用方式：)
   * prctl(PR_GET_SPECULATION_CTRL, PR_SPEC_STORE_BYPASS, 0, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_STORE_BYPASS, PR_SPEC_ENABLE, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_STORE_BYPASS, PR_SPEC_DISABLE, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_STORE_BYPASS, PR_SPEC_FORCE_DISABLE, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_STORE_BYPASS, PR_SPEC_DISABLE_NOEXEC, 0, 0);

- PR_SPEC_INDIR_BRANCH: Indirect Branch Speculation in User Processes
                        (Mitigate Spectre V2 style attacks against user processes)

  (PR_SPEC_INDIR_BRANCH：用户进程中的间接分支推测（缓解针对用户进程的 Spectre V2 风格攻击）。)

  Invocations: (调用方式：)
   * prctl(PR_GET_SPECULATION_CTRL, PR_SPEC_INDIRECT_BRANCH, 0, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_INDIRECT_BRANCH, PR_SPEC_ENABLE, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_INDIRECT_BRANCH, PR_SPEC_DISABLE, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_INDIRECT_BRANCH, PR_SPEC_FORCE_DISABLE, 0, 0);

- PR_SPEC_L1D_FLUSH: Flush L1D Cache on context switch out of the task
                        (works only when tasks run on non SMT cores)

  (PR_SPEC_L1D_FLUSH：在上下文切换出任务时刷新 L1D 缓存（仅当任务在非 SMT 核心上运行时有效）。)

For this control, PR_SPEC_ENABLE means that the **mitigation** is enabled (L1D
is flushed), PR_SPEC_DISABLE means it is disabled.
(对于此控制，PR_SPEC_ENABLE 表示 **缓解措施** 已启用（L1D 被刷新），PR_SPEC_DISABLE 表示它已禁用。)

  Invocations: (调用方式：)
   * prctl(PR_GET_SPECULATION_CTRL, PR_SPEC_L1D_FLUSH, 0, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_L1D_FLUSH, PR_SPEC_ENABLE, 0, 0);
   * prctl(PR_SET_SPECULATION_CTRL, PR_SPEC_L1D_FLUSH, PR_SPEC_DISABLE, 0, 0);
