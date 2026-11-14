XFS Maintainer Entry Profile
(XFS 维护者入门指南)
============================

Overview
(概述)
--------
XFS is a well known high-performance filesystem in the Linux kernel.
The aim of this project is to provide and maintain a robust and
performant filesystem.
(XFS 是 Linux 内核中众所周知的高性能文件系统。本项目的目标是提供并维护一个健壮且高性能的文件系统。)

Patches are generally merged to the for-next branch of the appropriate
git repository.
After a testing period, the for-next branch is merged to the master
branch.
(补丁通常被合并到相应 git 仓库的 for-next 分支。经过一段测试期后，for-next 分支被合并到 master 分支。)

Kernel code are merged to the xfs-linux tree[0].
Userspace code are merged to the xfsprogs tree[1].
Test cases are merged to the xfstests tree[2].
Ondisk format documentation are merged to the xfs-documentation tree[3].
(内核代码合并到 xfs-linux 树[0]。用户空间代码合并到 xfsprogs 树[1]。测试用例合并到 xfstests 树[2]。磁盘格式文档合并到 xfs-documentation 树[3]。)

All patchsets involving XFS *must* be cc'd in their entirety to the mailing
list linux-xfs@vger.kernel.org.
(所有涉及 XFS 的补丁集都 *必须* 完整地抄送到邮件列表 linux-xfs@vger.kernel.org。)

Roles
(角色)
-----
There are eight key roles in the XFS project.
A person can take on multiple roles, and a role can be filled by
multiple people.
Anyone taking on a role is advised to check in with themselves and
others on a regular basis about burnout.
(XFS 项目中有八个关键角色。一个人可以承担多个角色，一个角色也可以由多个人担任。建议承担任何角色的人定期与自己和他人沟通，关注倦怠问题。)

- **Outside Contributor**: Anyone who sends a patch but is not involved
  in the XFS project on a regular basis.
  These folks are usually people who work on other filesystems or
  elsewhere in the kernel community.
  (**外部贡献者**: 发送补丁但不定期参与 XFS 项目的任何人。这些人通常是在其他文件系统或内核社区其他领域工作的人。)

- **Developer**: Someone who is familiar with the XFS codebase enough to
  write new code, documentation, and tests.
  (**开发者**: 对 XFS 代码库足够熟悉、能够编写新代码、文档和测试的人。)

  Developers can often be found in the IRC channel mentioned by the ``C:``
  entry in the kernel MAINTAINERS file.
  (通常可以在内核 MAINTAINERS 文件中 ``C:`` 条目所提到的 IRC 频道中找到开发者。)

- **Senior Developer**: A developer who is very familiar with at least
  some part of the XFS codebase and/or other subsystems in the kernel.
  These people collectively decide the long term goals of the project
  and nudge the community in that direction.
  They should help prioritize development and review work for each release
  cycle.
  (**资深开发者**: 对 XFS 代码库的至少某一部分和/或内核中其他子系统非常熟悉的开发者。这些人共同决定项目的长期目标，并引导社区朝这个方向前进。他们应当帮助为每个发布周期确定开发和评审工作的优先级。)

  Senior developers tend to be more active participants in the IRC channel.
  (资深开发者往往是 IRC 频道中更活跃的参与者。)

- **Reviewer**: Someone (most likely also a developer) who reads code
  submissions to decide:
  (**评审者**: 某个人(很可能也是开发者)，阅读代码提交以决定：)

  0. Is the idea behind the contribution sound?
     (贡献背后的想法是否合理？)
  1. Does the idea fit the goals of the project?
     (该想法是否符合项目的目标？)
  2. Is the contribution designed correctly?
     (该贡献的设计是否正确？)
  3. Is the contribution polished?
     (该贡献是否足够完善？)
  4. Can the contribution be tested effectively?
     (该贡献能否被有效测试？)

  Reviewers should identify themselves with an ``R:`` entry in the kernel
  and fstests MAINTAINERS files.
  (评审者应当在内核和 fstests 的 MAINTAINERS 文件中以 ``R:`` 条目标明自己的身份。)

- **Testing Lead**: This person is responsible for setting the test
  coverage goals of the project, negotiating with developers to decide
  on new tests for new features, and making sure that developers and
  release managers execute on the testing.
  (**测试负责人**: 此人负责设定项目的测试覆盖目标，与开发者协商决定为新特性编写哪些新测试，并确保开发者和发布管理者落实测试。)

  The testing lead should identify themselves with an ``M:`` entry in
  the XFS section of the fstests MAINTAINERS file.
  (测试负责人应当在 fstests MAINTAINERS 文件的 XFS 部分以 ``M:`` 条目标明自己的身份。)

- **Bug Triager**: Someone who examines incoming bug reports in just
  enough detail to identify the person to whom the report should be
  forwarded.
  (**缺陷分类员**: 以刚好足够的细致程度检查收到的缺陷报告、从而确定报告应当转发给谁的人。)

  The bug triagers should identify themselves with a ``B:`` entry in
  the kernel MAINTAINERS file.
  (缺陷分类员应当在内核 MAINTAINERS 文件中以 ``B:`` 条目标明自己的身份。)

- **Release Manager**: This person merges reviewed patchsets into an
  integration branch, tests the result locally, pushes the branch to a
  public git repository, and sends pull requests further upstream.
  The release manager is not expected to work on new feature patchsets.
  If a developer and a reviewer fail to reach a resolution on some point,
  the release manager must have the ability to intervene to try to drive a
  resolution.
  (**发布管理者**: 此人将已评审的补丁集合并到集成分支，在本地测试结果，将分支推送到公共 git 仓库，并进一步向上游发送拉取请求。发布管理者不需要从事新特性补丁集方面的工作。如果开发者和评审者在某一点上无法达成一致，发布管理者必须有能力介入并尝试推动达成一致。)

  The release manager should identify themselves with an ``M:`` entry in
  the kernel MAINTAINERS file.
  (发布管理者应当在内核 MAINTAINERS 文件中以 ``M:`` 条目标明自己的身份。)

- **Community Manager**: This person calls and moderates meetings of as many
  XFS participants as they can get when mailing list discussions prove
  insufficient for collective decisionmaking.
  They may also serve as liaison between managers of the organizations
  sponsoring work on any part of XFS.
  (**社区管理者**: 当邮件列表讨论不足以进行集体决策时，此人召集并主持其所能召集到的尽可能多的 XFS 参与者参加会议。他们还可能在赞助 XFS 任何部分工作的组织的管理者之间充当联络人。)

- **LTS Maintainer**: Someone who backports and tests bug fixes from
  upstream to the LTS kernels.
  There tend to be six separate LTS trees at any given time.
  (**LTS 维护者**: 将来自上游的缺陷修复向后移植到 LTS 内核并进行测试的人。在任一时刻通常有六个独立的 LTS 树。)

  The maintainer for a given LTS release should identify themselves with an
  ``M:`` entry in the MAINTAINERS file for that LTS tree.
  Unmaintained LTS kernels should be marked with status ``S: Orphan`` in that
  same file.
  (给定 LTS 版本的维护者应当在该 LTS 树的 MAINTAINERS 文件中以 ``M:`` 条目标明自己的身份。无人维护的 LTS 内核应在同一文件中标记状态 ``S: Orphan``。)

Submission Checklist Addendum
(提交清单补充说明)
-----------------------------
Please follow these additional rules when submitting to XFS:
(向 XFS 提交时请遵循以下附加规则：)

- Patches affecting only the filesystem itself should be based against
  the latest -rc or the for-next branch.
  These patches will be merged back to the for-next branch.
  (仅影响文件系统本身的补丁应基于最新的 -rc 或 for-next 分支。这些补丁将被合并回 for-next 分支。)

- Authors of patches touching other subsystems need to coordinate with
  the maintainers of XFS and the relevant subsystems to decide how to
  proceed with a merge.
  (涉及其他子系统的补丁，其作者需要与 XFS 及相关子系统的维护者协调，以决定如何进行合并。)

- Any patchset changing XFS should be cc'd in its entirety to linux-xfs.
  Do not send partial patchsets; that makes analysis of the broader
  context of the changes unnecessarily difficult.
  (任何修改 XFS 的补丁集都应完整地抄送到 linux-xfs。不要发送不完整的补丁集；这会使分析这些更改的更广泛上下文变得不必要的困难。)

- Anyone making kernel changes that have corresponding changes to the
  userspace utilities should send the userspace changes as separate
  patchsets immediately after the kernel patchsets.
  (任何进行内核更改且用户空间工具存在相应更改的人，都应在内核补丁集之后立即将用户空间更改作为单独的补丁集发送。)

- Authors of bug fix patches are expected to use fstests[2] to perform
  an A/B test of the patch to determine that there are no regressions.
  When possible, a new regression test case should be written for
  fstests.
  (缺陷修复补丁的作者应使用 fstests[2] 对补丁进行 A/B 测试，以确定没有回归。在可能的情况下，应为 fstests 编写新的回归测试用例。)

- Authors of new feature patchsets must ensure that fstests will have
  appropriate functional and input corner-case test cases for the new
  feature.
  (新特性补丁集的作者必须确保 fstests 为新特性准备适当的功能和输入边界情况测试用例。)

- When implementing a new feature, it is strongly suggested that the
  developers write a design document to answer the following questions:
  (在实现新特性时，强烈建议开发者编写设计文档来回答以下问题：)

  * **What** problem is this trying to solve?
    (这是在试图解决**什么**问题？)

  * **Who** will benefit from this solution, and **where** will they
    access it?
    (**谁**将从这个解决方案中受益，他们将在**哪里**访问它？)

  * **How** will this new feature work?  This should touch on major data
    structures and algorithms supporting the solution at a higher level
    than code comments.
    (这个新特性将**如何**工作？这应当以比代码注释更高的层次论及支持该解决方案的主要数据结构和算法。)

  * **What** userspace interfaces are necessary to build off of the new
    features?
    (要在新特性的基础上进行构建，需要**哪些**用户空间接口？)

  * **How** will this work be tested to ensure that it solves the
    problems laid out in the design document without causing new
    problems?
    (将**如何**测试这项工作，以确保它解决设计文档中列出的问题而不引发新问题？)

  The design document should be committed in the kernel documentation
  directory.
  It may be omitted if the feature is already well known to the
  community.
  (设计文档应提交到内核文档目录中。如果该特性已为社区所熟知，则可以省略。)

- Patchsets for the new tests should be submitted as separate patchsets
  immediately after the kernel and userspace code patchsets.
  (新测试的补丁集应在内核和用户空间代码补丁集之后立即作为单独的补丁集提交。)

- Changes to the on-disk format of XFS must be described in the ondisk
  format document[3] and submitted as a patchset after the fstests
  patchsets.
  (对 XFS 磁盘格式的更改必须在磁盘格式文档[3]中描述，并在 fstests 补丁集之后作为补丁集提交。)

- Patchsets implementing bug fixes and further code cleanups should put
  the bug fixes at the beginning of the series to ease backporting.
  (实现缺陷修复和进一步代码清理的补丁集应将缺陷修复放在系列的开头，以便于向后移植。)

Key Release Cycle Dates
(关键发布周期日期)
-----------------------
Bug fixes may be sent at any time, though the release manager may decide to
defer a patch when the next merge window is close.
(缺陷修复可以随时发送，但当下一个合并窗口临近时，发布管理者可能会决定推迟某个补丁。)

Code submissions targeting the next merge window should be sent between
-rc1 and -rc6.
This gives the community time to review the changes, to suggest other changes,
and for the author to retest those changes.
(面向下一个合并窗口的代码提交应在 -rc1 与 -rc6 之间发送。这给社区留出了评审更改、建议其他更改的时间，也让作者能够重新测试这些更改。)

Code submissions also requiring changes to fs/iomap and targeting the
next merge window should be sent between -rc1 and -rc4.
This allows the broader kernel community adequate time to test the
infrastructure changes.
(同样需要修改 fs/iomap 且面向下一个合并窗口的代码提交应在 -rc1 与 -rc4 之间发送。这让更广泛的内核社区有充足的时间测试基础设施更改。)

Review Cadence
(评审节奏)
--------------
In general, please wait at least one week before pinging for feedback.
To find reviewers, either consult the MAINTAINERS file, or ask
developers that have Reviewed-by tags for XFS changes to take a look and
offer their opinion.
(一般来说，在催促反馈之前请至少等待一周。要寻找评审者，可以查阅 MAINTAINERS 文件，或者请曾为 XFS 更改给出 Reviewed-by 标签的开发者看一看并提出他们的意见。)

References
(参考文献)
----------
| [0] https://git.kernel.org/pub/scm/fs/xfs/xfs-linux.git/
| [1] https://git.kernel.org/pub/scm/fs/xfs/xfsprogs-dev.git/
| [2] https://git.kernel.org/pub/scm/fs/xfs/xfstests-dev.git/
| [3] https://git.kernel.org/pub/scm/fs/xfs/xfs-documentation.git/
