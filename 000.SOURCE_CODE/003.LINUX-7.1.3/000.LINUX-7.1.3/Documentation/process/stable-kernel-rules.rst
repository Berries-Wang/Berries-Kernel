.. _stable_kernel_rules:

======================================================================================================
Everything you ever wanted to know about Linux -stable releases(关于 Linux -stable 发布你想知道的一切)
======================================================================================================

Rules on what kind of patches are accepted, and which ones are not, into the
"-stable" tree:
(关于哪类补丁会被接受进 "-stable" 树、哪类不会的规则：)

- It or an equivalent fix must already exist in Linux mainline (upstream).
  (它或等效的修复必须已存在于 Linux mainline (上游)。)
- It must be obviously correct and tested.
  (它必须明显正确且经过测试。)
- It cannot be bigger than 100 lines, with context.
  (连同上下文在内，它不能超过 100 行。)
- It must follow the
  :ref:`Documentation/process/submitting-patches.rst <submittingpatches>`
  rules.
  (它必须遵循
  :ref:`Documentation/process/submitting-patches.rst <submittingpatches>`
  的规则。)
- It must either fix a real bug that bothers people or just add a device ID.
  To elaborate on the former:
  (它必须要么修复困扰人们的真实缺陷，要么只是添加一个设备 ID。对前者展开来说：)

  - It fixes a problem like an oops, a hang, data corruption, a real security
    issue, a hardware quirk, a build error (but not for things marked
    CONFIG_BROKEN), or some "oh, that's not good" issue.
    (它修复诸如 oops、挂起、数据损坏、真实安全问题、硬件怪癖、构建错误 (但标记为 CONFIG_BROKEN 的除外)，或某种"哦，这可不好"的问题。)
  - Serious issues as reported by a user of a distribution kernel may also
    be considered if they fix a notable performance or interactivity issue.
    As these fixes are not as obvious and have a higher risk of a subtle
    regression they should only be submitted by a distribution kernel
    maintainer and include an addendum linking to a bugzilla entry if it
    exists and additional information on the user-visible impact.
    (发行版内核用户报告的严重问题如果修复了显著的性能或交互性问题，也可以被考虑。由于这类修复不那么显而易见，且出现细微退化的风险更高，它们只能由发行版内核维护者提交，并附上附录：如有 bugzilla 条目则链接之，并说明用户可见影响等附加信息。)
  - No "This could be a problem..." type of things like a "theoretical race
    condition", unless an explanation of how the bug can be exploited is also
    provided.
    (不接受"这可能是个问题……"之类的东西，例如"理论上的竞态条件"，除非同时提供该缺陷可被如何利用的解释。)
  - No "trivial" fixes without benefit for users (spelling changes, whitespace
    cleanups, etc).
    (不接受对用户没有益处的"琐碎"修复 (拼写更改、空白清理等)。)


Procedure for submitting patches to the -stable tree(向 -stable 树提交补丁的流程)
----------------------------------------------------------------------------------

.. note::

   Security patches should not be handled (solely) by the -stable review
   process but should follow the procedures in
   :ref:`Documentation/process/security-bugs.rst <securitybugs>`.
   (安全补丁不应 (只) 由 -stable 评审流程处理，而应遵循
   :ref:`Documentation/process/security-bugs.rst <securitybugs>` 中的流程。)

There are three options to submit a change to -stable trees:
(向 -stable 树提交更改有三种方式：)

1. Add a 'stable tag' to the description of a patch you then submit for
   mainline inclusion.
   (在你随后提交以纳入 mainline 的补丁描述中添加 'stable tag'。)
2. Ask the stable team to pick up a patch already mainlined.
   (请 stable 团队拾取一个已经进入 mainline 的补丁。)
3. Submit a patch to the stable team that is equivalent to a change already
   mainlined.
   (向 stable 团队提交一个与已进入 mainline 的更改等效的补丁。)

The sections below describe each of the options in more detail.
(下文各节更详细地描述每一种方式。)

:ref:`option_1` is **strongly** preferred, it is the easiest and most common.
:ref:`option_2` is mainly meant for changes where backporting was not considered
at the time of submission. :ref:`option_3` is an alternative to the two earlier
options for cases where a mainlined patch needs adjustments to apply in older
series (for example due to API changes).
(:ref:`option_1` 是 **强烈** 首选的，它最简单也最常用。:ref:`option_2` 主要适用于提交时未考虑回移植 (backport) 的更改。:ref:`option_3` 是前两种方式的替代，适用于已进入 mainline 的补丁在应用到较旧系列时需要调整的情况 (例如由于 API 变化)。)

When using option 2 or 3 you can ask for your change to be included in specific
stable series. When doing so, ensure the fix or an equivalent is applicable,
submitted, or already present in all newer stable trees still supported. This is
meant to prevent regressions that users might later encounter on updating, if
e.g. a fix merged for 5.19-rc1 would be backported to 5.10.y, but not to 5.15.y.
(使用方式 2 或 3 时，你可以要求把你的更改纳入特定的 stable 系列。这样做时，请确保该修复或等效修复适用于、已提交至、或已存在于所有仍在支持的更新的 stable 树中。这是为了防止用户日后更新时遇到退化，例如一个为 5.19-rc1 合入的修复被回移植到 5.10.y 却没有回移植到 5.15.y。)

.. _option_1:

Option 1(选项 1)
****************

To have a patch you submit for mainline inclusion later automatically picked up
for stable trees, add this tag in the sign-off area::

  Cc: stable@vger.kernel.org

(要让你为纳入 mainline 而提交的补丁之后被自动拾取进 stable 树，请在 sign-off 区域添加此标签。)

Use ``Cc: stable@kernel.org`` instead when fixing unpublished vulnerabilities:
it reduces the chance of accidentally exposing the fix to the public by way of
'git send-email', as mails sent to that address are not delivered anywhere.
(修复未公开漏洞时请改用 ``Cc: stable@kernel.org`` ：这可以降低通过 'git send-email' 把修复意外暴露给公众的几率，因为发送到该地址的邮件不会被投递到任何地方。)

Once the patch is mainlined it will be applied to the stable tree without
anything else needing to be done by the author or subsystem maintainer.
(补丁进入 mainline 后，它就会被应用到 stable 树，作者或子系统维护者无需再做任何事。)

To send additional instructions to the stable team, use a shell-style inline
comment to pass arbitrary or predefined notes:
(要向 stable 团队发送附加指令，可使用 shell 风格的行内注释来传递任意或预定义的说明：)

* Specify any additional patch prerequisites for cherry picking::

    Cc: <stable@vger.kernel.org> # 3.3.x: a1f84a3: sched: Check for idle
    Cc: <stable@vger.kernel.org> # 3.3.x: 1b9508f: sched: Rate-limit newidle
    Cc: <stable@vger.kernel.org> # 3.3.x: fd21073: sched: Fix affinity logic
    Cc: <stable@vger.kernel.org> # 3.3.x
    Signed-off-by: Ingo Molnar <mingo@elte.hu>

  (为 cherry-pick 指定任何额外的补丁前置条件：)

  The tag sequence has the meaning of::

    git cherry-pick a1f84a3
    git cherry-pick 1b9508f
    git cherry-pick fd21073
    git cherry-pick <this commit>

  (该标签序列的含义是：)

  Note that for a patch series, you do not have to list as prerequisites the
  patches present in the series itself. For example, if you have the following
  patch series::

    patch1
    patch2

  (注意，对于补丁系列，你不必把系列本身包含的补丁列为前置条件。例如，如果你有如下补丁系列：)

  where patch2 depends on patch1, you do not have to list patch1 as
  prerequisite of patch2 if you have already marked patch1 for stable
  inclusion.
  (其中 patch2 依赖 patch1，如果你已经把 patch1 标记为纳入 stable，则不必把 patch1 列为 patch2 的前置条件。)

* Point out kernel version prerequisites::

    Cc: <stable@vger.kernel.org> # 3.3.x

  (指出内核版本前置条件：)

  The tag has the meaning of::

    git cherry-pick <this commit>

  (该标签的含义是：)

  For each "-stable" tree starting with the specified version.
  (对从指定版本开始的每个 "-stable" 树。)

  Note, such tagging is unnecessary if the stable team can derive the
  appropriate versions from Fixes: tags.
  (注意，如果 stable 团队能从 Fixes: 标签推导出合适版本，则无需这种标记。)

* Delay pick up of patches::

    Cc: <stable@vger.kernel.org> # after -rc3

  (延迟补丁的拾取：)

* Point out known problems::

    Cc: <stable@vger.kernel.org> # see patch description, needs adjustments for <= 6.3

  (指出已知问题：)

There furthermore is a variant of the stable tag you can use to make the stable
team's backporting tools (e.g AUTOSEL or scripts that look for commits
containing a 'Fixes:' tag) ignore a change::

     Cc: <stable+noautosel@kernel.org> # reason goes here, and must be present

(此外还有一种 stable 标签变体，可以让 stable 团队的回移植工具 (例如 AUTOSEL 或查找含 'Fixes:' 标签提交的脚本) 忽略某个更改：)

.. _option_2:

Option 2(选项 2)
****************

If the patch already has been merged to mainline, send an email to
stable@vger.kernel.org containing the subject of the patch, the commit ID,
why you think it should be applied, and what kernel versions you wish it to
be applied to.
(如果补丁已经合入 mainline，请给 stable@vger.kernel.org 发邮件，内容包括补丁主题、提交 ID、你认为它应被应用的理由，以及你希望它被应用到哪些内核版本。)

.. _option_3:

Option 3(选项 3)
****************

Send the patch, after verifying that it follows the above rules, to
stable@vger.kernel.org and mention the kernel versions you wish it to be applied
to. When doing so, you must note the upstream commit ID in the changelog of your
submission with a separate line above the commit text, like this::

  commit <sha1> upstream.

(在核实补丁遵循上述规则后，把补丁发送给 stable@vger.kernel.org，并说明你希望它被应用到哪些内核版本。这样做时，你必须在提交说明 (changelog) 中、提交文本上方单独一行注明上游提交 ID，如下面所示：)

Or alternatively::

  [ Upstream commit <sha1> ]

(或者另一种写法：)

If the submitted patch deviates from the original upstream patch (for example
because it had to be adjusted for the older API), this must be very clearly
documented and justified in the patch description.
(如果提交的补丁与原上游补丁有偏差 (例如因为它必须为较旧的 API 做调整)，必须在补丁描述中非常清晰地记录并说明理由。)


Following the submission(提交之后)
----------------------------------

The sender will receive an ACK when the patch has been accepted into the
queue, or a NAK if the patch is rejected.  This response might take a few
days, according to the schedules of the stable team members.
(补丁被接受进队列时，发送者会收到 ACK；补丁被拒绝时收到 NAK。根据 stable 团队成员的日程安排，这一响应可能需要几天时间。)

If accepted, the patch will be added to the -stable queue, for review by other
developers and by the relevant subsystem maintainer.
(如果被接受，补丁将被加入 -stable 队列，供其他开发者及相关子系统维护者评审。)


Review cycle(评审周期)
----------------------

- When the -stable maintainers decide for a review cycle, the patches will be
  sent to the review committee, and the maintainer of the affected area of
  the patch (unless the submitter is the maintainer of the area) and CC: to
  the linux-kernel mailing list.
  (当 -stable 维护者决定进行评审周期时，补丁会被发送给评审委员会，以及受影响区域的补丁维护者 (除非提交者就是该区域维护者)，并抄送 linux-kernel 邮件列表。)
- The review committee has 48 hours in which to ACK or NAK the patch.
  (评审委员会有 48 小时时间来 ACK 或 NAK 该补丁。)
- If the patch is rejected by a member of the committee, or linux-kernel
  members object to the patch, bringing up issues that the maintainers and
  members did not realize, the patch will be dropped from the queue.
  (如果补丁被委员会成员拒绝，或 linux-kernel 成员提出维护者与成员未曾意识到的问题而反对该补丁，补丁将被从队列中丢弃。)
- The ACKed patches will be posted again as part of release candidate (-rc)
  to be tested by developers and testers.
  (获得 ACK 的补丁会作为发布候选 (release candidate, -rc) 的一部分再次发布，供开发者与测试者测试。)
- Usually only one -rc release is made, however if there are any outstanding
  issues, some patches may be modified or dropped or additional patches may
  be queued. Additional -rc releases are then released and tested until no
  issues are found.
  (通常只发布一个 -rc；不过如果存在未解决的问题，某些补丁可能被修改或丢弃，也可能有额外补丁入队。随后会发布并测试更多 -rc，直到没有发现问题时为止。)
- Responding to the -rc releases can be done on the mailing list by sending
  a "Tested-by:" email with any testing information desired. The "Tested-by:"
  tags will be collected and added to the release commit.
  (对 -rc 发布的响应可以在邮件列表上进行：发送带有所需测试信息的 "Tested-by:" 邮件。"Tested-by:" 标签会被收集并加入发布提交。)
- At the end of the review cycle, the new -stable release will be released
  containing all the queued and tested patches.
  (评审周期结束时，将发布包含所有已排队且已测试补丁的新 -stable 版本。)
- Security patches will be accepted into the -stable tree directly from the
  security kernel team, and not go through the normal review cycle.
  Contact the kernel security team for more details on this procedure.
  (安全补丁由内核安全团队直接接受进 -stable 树，不经过正常的评审周期。有关此流程的更多细节请联系内核安全团队。)


Trees(代码树)
--------------

- The queues of patches, for both completed versions and in progress
  versions can be found at:

    https://git.kernel.org/pub/scm/linux/kernel/git/stable/stable-queue.git

  (已完成版本与进行中版本的补丁队列可以在上述地址找到。)
- The finalized and tagged releases of all stable kernels can be found
  in separate branches per version at:

    https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git

  (所有 stable 内核最终定稿并打上标签的发布，可在上述地址按版本位于各自分支中找到。)
- The release candidate of all stable kernel versions can be found at:

    https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable-rc.git/

  (所有 stable 内核版本的发布候选可在上述地址找到。)

  .. warning::
     The -stable-rc tree is a snapshot in time of the stable-queue tree and
     will change frequently, hence will be rebased often. It should only be
     used for testing purposes (e.g. to be consumed by CI systems).
     (-stable-rc 树是 stable-queue 树在某一时刻的快照，会频繁变化，因此经常被 rebase。它只应用于测试目的 (例如供 CI 系统使用)。)


Review committee(评审委员会)
----------------------------

- This is made up of a number of kernel developers who have volunteered for
  this task, and a few that haven't.
  (它由一些自愿承担此任务的内核开发者，以及少数并非自愿者组成。)
