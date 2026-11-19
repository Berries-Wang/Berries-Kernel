========================================================
HOWTO interact with BPF subsystem(如何与 BPF 子系统交互)
========================================================

This document provides information for the BPF subsystem about various
workflows related to reporting bugs, submitting patches, and queueing
patches for stable kernels.
(本文档为 BPF 子系统提供与报告缺陷、提交补丁以及为 stable 内核排队补丁相关的
各种工作流程的信息。)

For general information about submitting patches, please refer to
Documentation/process/submitting-patches.rst. This document only describes
additional specifics related to BPF.
(关于提交补丁的一般信息，请参阅
Documentation/process/submitting-patches.rst。本文档只描述与 BPF 相关的补充
细节。)

.. contents::
    :local:
    :depth: 2

Reporting bugs(报告缺陷)
========================

Q: How do I report bugs for BPF kernel code?(问：如何为 BPF 内核代码报告缺陷？)
-------------------------------------------------------------------------------
A: Since all BPF kernel development as well as bpftool and iproute2 BPF
loader development happens through the bpf kernel mailing list,
please report any found issues around BPF to the following mailing
list:
(A: 由于所有 BPF 内核开发以及 bpftool 和 iproute2 BPF 加载器的开发都是通过
bpf 内核邮件列表进行的，请把发现的任何与 BPF 相关的问题报告到以下邮件列表：)

 bpf@vger.kernel.org

This may also include issues related to XDP, BPF tracing, etc.
(这也可能包括与 XDP、BPF 跟踪等相关的问题。)

Given netdev has a high volume of traffic, please also add the BPF
maintainers to Cc (from kernel ``MAINTAINERS`` file):
(鉴于 netdev 邮件列表流量很大，请同时把 BPF 维护者加入 Cc（来自内核
``MAINTAINERS`` 文件）：)

* Alexei Starovoitov <ast@kernel.org>
* Daniel Borkmann <daniel@iogearbox.net>

In case a buggy commit has already been identified, make sure to keep
the actual commit authors in Cc as well for the report. They can
typically be identified through the kernel's git tree.
(如果已经定位到有缺陷的提交，请确保在报告中同时把该提交的实际作者加入 Cc。
通常可以通过内核的 git 树找到他们。)

**Please do NOT report BPF issues to bugzilla.kernel.org since it
is a guarantee that the reported issue will be overlooked.**
(**请勿向 bugzilla.kernel.org 报告 BPF 问题，因为这样做必然导致所报告的问题
被忽视。**)

Submitting patches(提交补丁)
============================

Q: How do I run BPF CI on my changes before sending them out for review?(问：在发送更改供评审之前，如何对它们运行 BPF CI？)
---------------------------------------------------------------------------------------------------------------------------
A: BPF CI is GitHub based and hosted at https://github.com/kernel-patches/bpf.
While GitHub also provides a CLI that can be used to accomplish the same
results, here we focus on the UI based workflow.
(A: BPF CI 基于 GitHub，托管在 https://github.com/kernel-patches/bpf。
虽然 GitHub 也提供了可以完成同样工作的 CLI，但这里我们关注基于 UI 的工作流程。)

The following steps lay out how to start a CI run for your patches:
(以下步骤说明了如何为你的补丁启动一次 CI 运行：)

- Create a fork of the aforementioned repository in your own account (one time
  action)
  (在你自己的账号中创建上述仓库的复刻 (fork)（一次性操作）。)

- Clone the fork locally, check out a new branch tracking either the bpf-next
  or bpf branch, and apply your to-be-tested patches on top of it
  (在本地克隆该复刻，检出一个跟踪 bpf-next 或 bpf 分支的新分支，并把你要测试的
  补丁应用在其之上。)

- Push the local branch to your fork and create a pull request against
  kernel-patches/bpf's bpf-next_base or bpf_base branch, respectively
  (把本地分支推送到你的复刻，并分别针对 kernel-patches/bpf 的 bpf-next_base 或
  bpf_base 分支创建拉取请求。)

Shortly after the pull request has been created, the CI workflow will run. Note
that capacity is shared with patches submitted upstream being checked and so
depending on utilization the run can take a while to finish.
(拉取请求创建后不久，CI 工作流就会运行。注意，CI 容量与上游提交的、正在检查的
补丁共享，因此视利用率而定，运行可能需要一段时间才能完成。)

Note furthermore that both base branches (bpf-next_base and bpf_base) will be
updated as patches are pushed to the respective upstream branches they track. As
such, your patch set will automatically (be attempted to) be rebased as well.
This behavior can result in a CI run being aborted and restarted with the new
base line.
(另外注意，两个基线分支（bpf-next_base 和 bpf_base）会随着补丁被推送到它们所
跟踪的相应上游分支而更新。因此，你的补丁集也会被自动（尝试）变基 (rebase)。
这种行为可能导致一次 CI 运行被中止，并以新的基线重新开始。)

Q: To which mailing list do I need to submit my BPF patches?(问：我需要把 BPF 补丁提交到哪个邮件列表？)
-------------------------------------------------------------------------------------------------------
A: Please submit your BPF patches to the bpf kernel mailing list:
(A: 请把你的 BPF 补丁提交到 bpf 内核邮件列表：)

 bpf@vger.kernel.org

In case your patch has changes in various different subsystems (e.g.
networking, tracing, security, etc), make sure to Cc the related kernel mailing
lists and maintainers from there as well, so they are able to review
the changes and provide their Acked-by's to the patches.
(如果你的补丁涉及多个不同的子系统（例如网络、跟踪、安全等），请确保同时 Cc
相应子系统的内核邮件列表和维护者，使他们能够评审这些更改并为补丁提供
Acked-by。)

Q: Where can I find patches currently under discussion for BPF subsystem?(问：在哪里可以找到 BPF 子系统当前正在讨论中的补丁？)
------------------------------------------------------------------------------------------------------------------------------
A: All patches that are Cc'ed to netdev are queued for review under netdev
patchwork project:
(A: 所有被 Cc 到 netdev 的补丁都会进入 netdev patchwork 项目排队等待评审：)

  https://patchwork.kernel.org/project/netdevbpf/list/

Those patches which target BPF, are assigned to a 'bpf' delegate for
further processing from BPF maintainers. The current queue with
patches under review can be found at:
(其中面向 BPF 的补丁会被指派给 'bpf' 委派员 (delegate)，由 BPF 维护者做进一步
处理。当前正在评审的补丁队列可以在以下地址找到：)

  https://patchwork.kernel.org/project/netdevbpf/list/?delegate=121173

Once the patches have been reviewed by the BPF community as a whole
and approved by the BPF maintainers, their status in patchwork will be
changed to 'Accepted' and the submitter will be notified by mail. This
means that the patches look good from a BPF perspective and have been
applied to one of the two BPF kernel trees.
(一旦补丁通过了整个 BPF 社区的评审并获得 BPF 维护者的批准，它们在 patchwork
中的状态就会变为 'Accepted'，提交者也会收到邮件通知。这意味着这些补丁从 BPF
的角度来看没有问题，并且已被应用到两个 BPF 内核树之一。)

In case feedback from the community requires a respin of the patches,
their status in patchwork will be set to 'Changes Requested', and purged
from the current review queue. Likewise for cases where patches would
get rejected or are not applicable to the BPF trees (but assigned to
the 'bpf' delegate).
(如果社区的反馈要求补丁重新提交一版，它们在 patchwork 中的状态会被设置为
'Changes Requested'，并从当前评审队列中移除。补丁被拒绝或不适用于 BPF 树的
情况（但被指派给了 'bpf' 委派员）也是如此。)

Q: How do the changes make their way into Linux?(问：这些更改是如何进入 Linux 的？)
-----------------------------------------------------------------------------------
A: There are two BPF kernel trees (git repositories). Once patches have
been accepted by the BPF maintainers, they will be applied to one
of the two BPF trees:
(A: 有两个 BPF 内核树（git 仓库）。补丁一旦被 BPF 维护者接受，就会被应用到
这两个 BPF 树之一：)

 * https://git.kernel.org/pub/scm/linux/kernel/git/bpf/bpf.git/
 * https://git.kernel.org/pub/scm/linux/kernel/git/bpf/bpf-next.git/

The bpf tree itself is for fixes only, whereas bpf-next for features,
cleanups or other kind of improvements ("next-like" content). This is
analogous to net and net-next trees for networking. Both bpf and
bpf-next will only have a master branch in order to simplify against
which branch patches should get rebased to.
(bpf 树本身只用于修复 (fix)，而 bpf-next 用于特性、清理或其他类型的改进
（"next 类"内容）。这类似于网络领域的 net 和 net-next 树。bpf 和 bpf-next 都
只有一个 master 分支，以便简化"补丁应当变基到哪个分支"的问题。)

Accumulated BPF patches in the bpf tree will regularly get pulled
into the net kernel tree. Likewise, accumulated BPF patches accepted
into the bpf-next tree will make their way into net-next tree. net and
net-next are both run by David S. Miller. From there, they will go
into the kernel mainline tree run by Linus Torvalds. To read up on the
process of net and net-next being merged into the mainline tree, see
the documentation on netdev subsystem at
Documentation/process/maintainer-netdev.rst.
(bpf 树中积累的 BPF 补丁会定期被拉入 net 内核树。同样，被接受进 bpf-next 树的
BPF 补丁会进入 net-next 树。net 和 net-next 都由 David S. Miller 管理。从那里，
它们会进入由 Linus Torvalds 管理的内核主线树。关于 net 和 net-next 被合并进
主线树的过程，请参阅 Documentation/process/maintainer-netdev.rst 处的 netdev
子系统文档。)



Occasionally, to prevent merge conflicts, we might send pull requests
to other trees (e.g. tracing) with a small subset of the patches, but
net and net-next are always the main trees targeted for integration.
(偶尔，为了避免合并冲突，我们可能会把一小部分补丁以拉取请求的形式发送到其他树
（例如 tracing），但 net 和 net-next 始终是集成所面向的主要树。)

The pull requests will contain a high-level summary of the accumulated
patches and can be searched on netdev kernel mailing list through the
following subject lines (``yyyy-mm-dd`` is the date of the pull
request)::

  pull-request: bpf yyyy-mm-dd
  pull-request: bpf-next yyyy-mm-dd

(拉取请求会包含所积累补丁的高层级摘要，并可以通过以下主题行在 netdev 内核
邮件列表中搜索到（yyyy-mm-dd 为拉取请求的日期）：)

Q: How do I indicate which tree (bpf vs. bpf-next) my patch should be applied to?(问：如何指明我的补丁应应用到哪个树（bpf 还是 bpf-next）？)
--------------------------------------------------------------------------------------------------------------------------------------------

A: The process is the very same as described in the netdev subsystem
documentation at Documentation/process/maintainer-netdev.rst,
so please read up on it. The subject line must indicate whether the
patch is a fix or rather "next-like" content in order to let the
maintainers know whether it is targeted at bpf or bpf-next.
(A: 其流程与 Documentation/process/maintainer-netdev.rst 处的 netdev 子系统
文档所描述的完全相同，请先阅读它。主题行必须表明补丁是修复还是 "next 类"
内容，以便维护者知道它面向的是 bpf 还是 bpf-next。)

For fixes eventually landing in bpf -> net tree, the subject must
look like::

  git format-patch --subject-prefix='PATCH bpf' start..finish

(对于最终进入 bpf -> net 树的修复，主题行必须形如：)

For features/improvements/etc that should eventually land in
bpf-next -> net-next, the subject must look like::

  git format-patch --subject-prefix='PATCH bpf-next' start..finish

(对于最终应进入 bpf-next -> net-next 的特性/改进等，主题行必须形如：)

If unsure whether the patch or patch series should go into bpf
or net directly, or bpf-next or net-next directly, it is not a
problem either if the subject line says net or net-next as target.
It is eventually up to the maintainers to do the delegation of
the patches.
(如果不确定补丁或补丁系列应当直接进入 bpf 还是 net，或直接进入 bpf-next 还是
net-next，那么主题行把目标写成 net 或 net-next 也没有问题。最终由维护者来决定
这些补丁的分派。)

If it is clear that patches should go into bpf or bpf-next tree,
please make sure to rebase the patches against those trees in
order to reduce potential conflicts.
(如果明确补丁应进入 bpf 或 bpf-next 树，请确保把补丁针对这些树进行变基，以
减少潜在冲突。)

In case the patch or patch series has to be reworked and sent out
again in a second or later revision, it is also required to add a
version number (``v2``, ``v3``, ...) into the subject prefix::

  git format-patch --subject-prefix='PATCH bpf-next v2' start..finish

(如果补丁或补丁系列不得不重做并以第二版或更高版本再次发送，则还需要在主题
前缀中加入版本号（v2、v3 等）：)

When changes have been requested to the patch series, always send the
whole patch series again with the feedback incorporated (never send
individual diffs on top of the old series).
(当补丁系列被要求修改时，总是重新发送整个补丁系列并纳入反馈（绝不基于旧系列
发送单个 diff）。)

Q: What does it mean when a patch gets applied to bpf or bpf-next tree?(问：补丁被应用到 bpf 或 bpf-next 树意味着什么？)
------------------------------------------------------------------------------------------------------------------------
A: It means that the patch looks good for mainline inclusion from
a BPF point of view.
(A: 这意味着从 BPF 的角度看，该补丁适合进入主线。)

Be aware that this is not a final verdict that the patch will
automatically get accepted into net or net-next trees eventually:
(注意，这并不是"该补丁最终一定会被 net 或 net-next 树接受"的最终结论：)

On the bpf kernel mailing list reviews can come in at any point
in time. If discussions around a patch conclude that they cannot
get included as-is, we will either apply a follow-up fix or drop
them from the trees entirely. Therefore, we also reserve to rebase
the trees when deemed necessary. After all, the purpose of the tree
is to:
(在 bpf 内核邮件列表上，评审意见可能在任何时间点到来。如果围绕某个补丁的讨论
结论是它无法按原样合入，我们要么追加一个后续修复，要么把它从树中完全移除。
因此，我们也保留在认为必要时对树进行变基的权利。毕竟，这些树的目的是：)

i) accumulate and stage BPF patches for integration into trees
   like net and net-next, and
   (积累并暂存 BPF 补丁，以便合入 net 和 net-next 等树；)

ii) run extensive BPF test suite and
    workloads on the patches before they make their way any further.
    (在补丁进一步流转之前，在补丁之上运行大规模的 BPF 测试套件和
    工作负载。)

Once the BPF pull request was accepted by David S. Miller, then
the patches end up in net or net-next tree, respectively, and
make their way from there further into mainline. Again, see the
documentation for netdev subsystem at
Documentation/process/maintainer-netdev.rst for additional information
e.g. on how often they are merged to mainline.
(BPF 拉取请求一旦被 David S. Miller 接受，补丁就会分别进入 net 或 net-next
树，并从那里继续进入主线。同样，更多信息（例如它们多久合并一次到主线）请参阅
Documentation/process/maintainer-netdev.rst 处的 netdev 子系统文档。)

Q: How long do I need to wait for feedback on my BPF patches?(问：BPF 补丁的反馈需要等多久？)
---------------------------------------------------------------------------------------------
A: We try to keep the latency low. The usual time to feedback will
be around 2 or 3 business days. It may vary depending on the
complexity of changes and current patch load.
(A: 我们尽量保持较低的延迟。通常的反馈时间大约是 2 到 3 个工作日。具体时长可能
因更改的复杂度和当前补丁负载而异。)

Q: How often do you send pull requests to major kernel trees like net or net-next?(问：你们多久向 net 或 net-next 等主要内核树发送一次拉取请求？)
-------------------------------------------------------------------------------------------------------------------------------------------------

A: Pull requests will be sent out rather often in order to not
accumulate too many patches in bpf or bpf-next.
(A: 拉取请求会相当频繁地发出，以免在 bpf 或 bpf-next 中积累过多补丁。)

As a rule of thumb, expect pull requests for each tree regularly
at the end of the week. In some cases pull requests could additionally
come also in the middle of the week depending on the current patch
load or urgency.
(根据经验，可以预期每个树通常在周末会收到拉取请求。在某些情况下，根据当前补丁
负载或紧迫程度，拉取请求也可能在周中额外发出。)

Q: Are patches applied to bpf-next when the merge window is open?(问：合并窗口打开期间补丁会应用到 bpf-next 吗？)
-----------------------------------------------------------------------------------------------------------------
A: For the time when the merge window is open, bpf-next will not be
processed. This is roughly analogous to net-next patch processing,
so feel free to read up on the netdev docs at
Documentation/process/maintainer-netdev.rst about further details.
(A: 在合并窗口打开期间，bpf-next 不会被处理。这大致类似于 net-next 的补丁处理
方式，更多细节请自行阅读 Documentation/process/maintainer-netdev.rst 处的
netdev 文档。)

During those two weeks of merge window, we might ask you to resend
your patch series once bpf-next is open again. Once Linus released
a ``v*-rc1`` after the merge window, we continue processing of bpf-next.
(在合并窗口的那两周期间，我们可能会请你一旦 bpf-next 重新开放就重新发送你的
补丁系列。一旦 Linus 在合并窗口之后发布了 ``v*-rc1`` ，我们就继续处理
bpf-next。)

For non-subscribers to kernel mailing lists, there is also a status
page run by David S. Miller on net-next that provides guidance:
(对于未订阅内核邮件列表的人，David S. Miller 还在 net-next 上维护着一个提供
指引的状态页：)

  http://vger.kernel.org/~davem/net-next.html

Q: Verifier changes and test cases(问：验证器更改与测试用例)
------------------------------------------------------------
Q: I made a BPF verifier change, do I need to add test cases for
BPF kernel selftests_?
(问：我修改了 BPF 验证器，需要为 BPF 内核 selftests 添加测试用例吗？)

A: If the patch has changes to the behavior of the verifier, then yes,
it is absolutely necessary to add test cases to the BPF kernel
selftests_ suite. If they are not present and we think they are
needed, then we might ask for them before accepting any changes.
(A: 如果补丁改变了验证器的行为，那么是的，绝对有必要向 BPF 内核 selftests
套件添加测试用例。如果测试用例缺失而我们认为需要，我们可能会在接受任何更改
之前要求先补上。)

In particular, test_verifier.c is tracking a high number of BPF test
cases, including a lot of corner cases that LLVM BPF back end may
generate out of the restricted C code. Thus, adding test cases is
absolutely crucial to make sure future changes do not accidentally
affect prior use-cases. Thus, treat those test cases as: verifier
behavior that is not tracked in test_verifier.c could potentially
be subject to change.
(特别是，test_verifier.c 跟踪了大量 BPF 测试用例，包括 LLVM BPF 后端可能从
受限 C 代码生成的许多边界情况。因此，添加测试用例对于确保未来的更改不会意外
影响既有用例至关重要。可以把这些测试用例理解为：未在 test_verifier.c 中跟踪的
验证器行为都可能发生变化。)

Q: samples/bpf preference vs selftests?(问：偏好 samples/bpf 还是 selftests？)
------------------------------------------------------------------------------
Q: When should I add code to ``samples/bpf/`` and when to BPF kernel
selftests_?
(问：什么时候应该向 ``samples/bpf/`` 添加代码，什么时候向 BPF 内核 selftests
添加代码？)

A: In general, we prefer additions to BPF kernel selftests_ rather than
``samples/bpf/``. The rationale is very simple: kernel selftests are
regularly run by various bots to test for kernel regressions.
(A: 总的来说，我们更倾向于向 BPF 内核 selftests 添加内容，而不是
``samples/bpf/`` 。理由很简单：内核 selftests 会被各种机器人定期运行以测试
内核回归。)

The more test cases we add to BPF selftests, the better the coverage
and the less likely it is that those could accidentally break. It is
not that BPF kernel selftests cannot demo how a specific feature can
be used.
(我们向 BPF selftests 添加的测试用例越多，覆盖率就越好，这些用例被意外破坏的
可能性就越低。这并不是说 BPF 内核 selftests 无法演示某个特定特性的用法。)

That said, ``samples/bpf/`` may be a good place for people to get started,
so it might be advisable that simple demos of features could go into
``samples/bpf/``, but advanced functional and corner-case testing rather
into kernel selftests.
(话虽如此， ``samples/bpf/`` 可能是人们入门的好地方，因此特性的简单演示可以
放进 ``samples/bpf/`` ，而高级功能与边界情况测试则更适合放进内核 selftests。)

If your sample looks like a test case, then go for BPF kernel selftests
instead!
(如果你的示例看起来像一个测试用例，那就改用 BPF 内核 selftests！)

Q: When should I add code to the bpftool?(问：何时应该向 bpftool 添加代码？)
----------------------------------------------------------------------------
A: The main purpose of bpftool (under tools/bpf/bpftool/) is to provide
a central user space tool for debugging and introspection of BPF programs
and maps that are active in the kernel. If UAPI changes related to BPF
enable for dumping additional information of programs or maps, then
bpftool should be extended as well to support dumping them.
(A: bpftool（位于 tools/bpf/bpftool/）的主要目的是提供一个中心化的用户空间
工具，用于调试和内省内核中活跃的 BPF 程序和映射 (map)。如果与 BPF 相关的 UAPI
更改使得可以转储 (dump) 程序或映射的额外信息，那么 bpftool 也应当扩展以支持
转储它们。)

Q: When should I add code to iproute2's BPF loader?(问：何时应该向 iproute2 的 BPF 加载器添加代码？)
----------------------------------------------------------------------------------------------------
A: For UAPI changes related to the XDP or tc layer (e.g. ``cls_bpf``),
the convention is that those control-path related changes are added to
iproute2's BPF loader as well from user space side. This is not only
useful to have UAPI changes properly designed to be usable, but also
to make those changes available to a wider user base of major
downstream distributions.
(A: 对于与 XDP 或 tc 层（例如 ``cls_bpf`` ）相关的 UAPI 更改，惯例是这些与
控制路径相关的更改也要在用户空间侧添加到 iproute2 的 BPF 加载器中。这不仅
有助于确保 UAPI 更改被妥善设计为可用，还能让这些更改被主要下游发行版的更多
用户使用。)

Q: Do you accept patches as well for iproute2's BPF loader?(问：你们也接受 iproute2 的 BPF 加载器的补丁吗？)
------------------------------------------------------------------------------------------------------------
A: Patches for the iproute2's BPF loader have to be sent to:
(A: iproute2 的 BPF 加载器的补丁必须发送到：)

  netdev@vger.kernel.org

While those patches are not processed by the BPF kernel maintainers,
please keep them in Cc as well, so they can be reviewed.
(虽然这些补丁不由 BPF 内核维护者处理，但请把他们保持在 Cc 中，以便他们可以
评审。)

The official git repository for iproute2 is run by Stephen Hemminger
and can be found at:
(iproute2 的官方 git 仓库由 Stephen Hemminger 管理，位于：)

  https://git.kernel.org/pub/scm/linux/kernel/git/shemminger/iproute2.git/

The patches need to have a subject prefix of '``[PATCH iproute2
master]``' or '``[PATCH iproute2 net-next]``'. '``master``' or
'``net-next``' describes the target branch where the patch should be
applied to. Meaning, if kernel changes went into the net-next kernel
tree, then the related iproute2 changes need to go into the iproute2
net-next branch, otherwise they can be targeted at master branch. The
iproute2 net-next branch will get merged into the master branch after
the current iproute2 version from master has been released.
(这些补丁的主题前缀必须是 '[PATCH iproute2 master]' 或 '[PATCH iproute2
net-next]'。'master' 或 'net-next' 描述补丁应应用到的目标分支。也就是说，如果
内核更改进入了 net-next 内核树，那么相应的 iproute2 更改就需要进入 iproute2 的
net-next 分支；否则它们可以面向 master 分支。当 master 上的当前 iproute2 版本
发布之后，iproute2 的 net-next 分支会被合并进 master 分支。)

Like BPF, the patches end up in patchwork under the netdev project and
are delegated to 'shemminger' for further processing:
(与 BPF 一样，这些补丁会进入 netdev 项目下的 patchwork，并被指派给
'shemminger' 做进一步处理：)

  http://patchwork.ozlabs.org/project/netdev/list/?delegate=389

Q: What is the minimum requirement before I submit my BPF patches?(问：提交 BPF 补丁之前有什么最低要求？)
---------------------------------------------------------------------------------------------------------
A: When submitting patches, always take the time and properly test your
patches *prior* to submission. Never rush them! If maintainers find
that your patches have not been properly tested, it is a good way to
get them grumpy. Testing patch submissions is a hard requirement!
(A: 提交补丁时，总要花时间在提交 *之前* 恰当地测试你的补丁。绝不要仓促了事！
如果维护者发现你的补丁没有得到恰当测试，那是让他们恼火的好办法。测试补丁提交
是硬性要求！)

Note, fixes that go to bpf tree *must* have a ``Fixes:`` tag included.
The same applies to fixes that target bpf-next, where the affected
commit is in net-next (or in some cases bpf-next). The ``Fixes:`` tag is
crucial in order to identify follow-up commits and tremendously helps
for people having to do backporting, so it is a must have!
(注意，进入 bpf 树的修复 *必须* 带有 ``Fixes:`` 标签。
面向 bpf-next 的修复也是如此，此时受影响的提交在 net-next 中（某些情况下在
bpf-next 中）。 ``Fixes:`` 标签对于识别后续提交至关重要，并极大地帮助需要做
回移 (backport) 的人，所以它是必备的！)

We also don't accept patches with an empty commit message. Take your
time and properly write up a high quality commit message, it is
essential!
(我们也不接受提交消息为空的补丁。花点时间认真写一条高质量的提交消息，这
必不可少！)

Think about it this way: other developers looking at your code a month
from now need to understand *why* a certain change has been done that
way, and whether there have been flaws in the analysis or assumptions
that the original author did. Thus providing a proper rationale and
describing the use-case for the changes is a must.
(不妨这样想：一个月后查看你代码的其他开发者需要理解某个更改 *为什么* 要那样
做，以及原作者的分析或假设是否存在缺陷。因此，给出恰当的理由并描述更改的
使用场景是必须的。)

Patch submissions with >1 patch must have a cover letter which includes
a high level description of the series. This high level summary will
then be placed into the merge commit by the BPF maintainers such that
it is also accessible from the git log for future reference.
(包含多于 1 个补丁的提交必须带有封面信 (cover letter)，其中包含该系列的高层级
描述。这个高层级摘要随后会被 BPF 维护者放入合并提交中，以便将来可以从 git log
中查阅。)

Q: Features changing BPF JIT and/or LLVM(问：涉及 BPF JIT 和/或 LLVM 的特性更改)
--------------------------------------------------------------------------------
Q: What do I need to consider when adding a new instruction or feature
that would require BPF JIT and/or LLVM integration as well?
(问：添加一条新指令或一个同样需要 BPF JIT 和/或 LLVM 集成的新特性时，需要
考虑什么？)

A: We try hard to keep all BPF JITs up to date such that the same user
experience can be guaranteed when running BPF programs on different
architectures without having the program punt to the less efficient
interpreter in case the in-kernel BPF JIT is enabled.
(A: 我们努力让所有 BPF JIT 保持最新，以便在内核 BPF JIT 已启用的情况下，在
不同架构上运行 BPF 程序时能保证同样的用户体验，而不会让程序退回到效率较低的
解释器。)

If you are unable to implement or test the required JIT changes for
certain architectures, please work together with the related BPF JIT
developers in order to get the feature implemented in a timely manner.
Please refer to the git log (``arch/*/net/``) to locate the necessary
people for helping out.
(如果你无法为某些架构实现或测试所需的 JIT 更改，请与相关的 BPF JIT 开发者
合作，以便及时实现该特性。请参阅 git log（arch/*/net/）来找到可以提供帮助的
人。)

Also always make sure to add BPF test cases (e.g. test_bpf.c and
test_verifier.c) for new instructions, so that they can receive
broad test coverage and help run-time testing the various BPF JITs.
(同时，总要确保为新指令添加 BPF 测试用例（例如 test_bpf.c 和
test_verifier.c），使其获得广泛的测试覆盖，并帮助在运行时测试各种 BPF JIT。)

In case of new BPF instructions, once the changes have been accepted
into the Linux kernel, please implement support into LLVM's BPF back
end. See `LLVM(LLVM 编译器框架)`_ section below for further information.
(对于新的 BPF 指令，一旦更改被接受进 Linux 内核，请在 LLVM 的 BPF 后端中实现
支持。更多信息参见下文 LLVM 一节。)

Q: What "BPF_INTERNAL" symbol namespace is for?(问："BPF_INTERNAL" 符号命名空间的用途是什么？)
----------------------------------------------------------------------------------------------
A: Symbols exported as BPF_INTERNAL can only be used by BPF infrastructure
like preload kernel modules with light skeleton. Most symbols outside
of BPF_INTERNAL are not expected to be used by code outside of BPF either.
Symbols may lack the designation because they predate the namespaces,
or due to an oversight.
(A: 以 BPF_INTERNAL 导出的符号只能被 BPF 基础设施使用，例如带有轻量骨架
(light skeleton) 的 preload 内核模块。BPF_INTERNAL 之外的大多数符号也不期望
被 BPF 以外的代码使用。符号可能缺少这一命名空间标记，原因是它们早于命名空间
机制出现，或是由于疏忽。)

Stable submission(提交到 stable)
================================

Q: I need a specific BPF commit in stable kernels. What should I do?(问：我需要在 stable 内核中加入某个特定的 BPF 提交，该怎么做？)
-------------------------------------------------------------------------------------------------------------------------------------
A: In case you need a specific fix in stable kernels, first check whether
the commit has already been applied in the related ``linux-*.y`` branches:
(A: 如果你需要在 stable 内核中加入某个特定的修复，先检查该提交是否已被应用到
相关的 ``linux-*.y`` 分支：)

  https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git/

If not the case, then drop an email to the BPF maintainers with the
netdev kernel mailing list in Cc and ask for the fix to be queued up:
(如果不是这样，就给 BPF 维护者发邮件并 Cc netdev 内核邮件列表，请求把该修复
排队处理：)

  netdev@vger.kernel.org

The process in general is the same as on netdev itself, see also the
the documentation on networking subsystem at
Documentation/process/maintainer-netdev.rst.
(总体流程与 netdev 本身相同，另请参阅 Documentation/process/maintainer-netdev.rst
处的网络子系统文档。)

Q: Do you also backport to kernels not currently maintained as stable?(问：你们也会向当前未作为 stable 维护的内核回移 (backport) 吗？)
--------------------------------------------------------------------------------------------------------------------------------------
A: No. If you need a specific BPF commit in kernels that are currently not
maintained by the stable maintainers, then you are on your own.
(A: 不会。如果你需要在当前不受 stable 维护者维护的内核中加入某个特定的 BPF
提交，那就只能靠你自己了。)

The current stable and longterm stable kernels are all listed here:
(当前的 stable 与 longterm stable 内核都列在下面：)

  https://www.kernel.org/

Q: The BPF patch I am about to submit needs to go to stable as well(问：我即将提交的 BPF 补丁也需要进入 stable)
---------------------------------------------------------------------------------------------------------------
What should I do?
(该怎么办？)

A: The same rules apply as with netdev patch submissions in general, see
the netdev docs at Documentation/process/maintainer-netdev.rst.
(A: 与 netdev 补丁提交的通用规则相同，参见
Documentation/process/maintainer-netdev.rst 处的 netdev 文档。)

Never add "``Cc: stable@vger.kernel.org``" to the patch description, but
ask the BPF maintainers to queue the patches instead. This can be done
with a note, for example, under the ``---`` part of the patch which does
not go into the git log. Alternatively, this can be done as a simple
request by mail instead.
(绝不要在补丁描述中加入 "``Cc: stable@vger.kernel.org``" ，而应请 BPF 维护者
把补丁排队。这可以通过一个说明来实现，例如放在补丁的 ``---`` 部分（该部分不会
进入 git log）。或者，也可以通过一封简单的邮件请求来完成。)

Q: Queue stable patches(问：已排队的 stable 补丁)
-------------------------------------------------
Q: Where do I find currently queued BPF patches that will be submitted
to stable?
(问：在哪里可以找到当前已排队并将被提交到 stable 的 BPF 补丁？)

A: Once patches that fix critical bugs got applied into the bpf tree, they
are queued up for stable submission under:
(A: 一旦修复严重缺陷的补丁被应用到 bpf 树，它们就会在以下地址排队等待提交到
stable：)

  http://patchwork.ozlabs.org/bundle/bpf/stable/?state=*

They will be on hold there at minimum until the related commit made its
way into the mainline kernel tree.
(它们至少会在那里保留到相应提交进入主线内核树为止。)

After having been under broader exposure, the queued patches will be
submitted by the BPF maintainers to the stable maintainers.
(经过更广泛的暴露之后，排队的补丁会由 BPF 维护者提交给 stable 维护者。)

Testing patches(测试补丁)
=========================

Q: How to run BPF selftests(问：如何运行 BPF selftests)
-------------------------------------------------------
A: After you have booted into the newly compiled kernel, navigate to
the BPF selftests_ suite in order to test BPF functionality (current
working directory points to the root of the cloned git tree)::

  $ cd tools/testing/selftests/bpf/
  $ make

(A: 在启动进入新编译的内核后，进入 BPF selftests 套件以测试 BPF 功能（当前
工作目录指向所克隆 git 树的根目录）：)

To run the verifier tests::

  $ sudo ./test_verifier

(运行验证器测试：)

The verifier tests print out all the current checks being
performed. The summary at the end of running all tests will dump
information of test successes and failures::

  Summary: 418 PASSED, 0 FAILED

(验证器测试会打印出当前正在执行的所有检查。运行全部测试结束时的摘要会转储
测试成功与失败的信息：)

In order to run through all BPF selftests, the following command is
needed::

  $ sudo make run_tests

(要运行全部 BPF selftests，需要以下命令：)

See :doc:`kernel selftest documentation </dev-tools/kselftest>`
for details.
(详情参见 kernel selftest 文档。)

To maximize the number of tests passing, the .config of the kernel
under test should match the config file fragment in
tools/testing/selftests/bpf as closely as possible.
(为了让通过的测试数量最大化，被测内核的 .config 应尽可能接近
tools/testing/selftests/bpf 中的配置文件片段。)

Finally to ensure support for latest BPF Type Format features -
discussed in Documentation/bpf/btf.rst - pahole version 1.16
is required for kernels built with CONFIG_DEBUG_INFO_BTF=y.
pahole is delivered in the dwarves package or can be built
from source at

https://github.com/acmel/dwarves

(最后，为确保支持最新的 BPF Type Format 特性（在 Documentation/bpf/btf.rst 中
讨论），使用 CONFIG_DEBUG_INFO_BTF=y 构建的内核需要 pahole 版本 1.16。
pahole 随 dwarves 软件包提供，或可以从以下地址的源码构建：)

pahole starts to use libbpf definitions and APIs since v1.13 after the
commit 21507cd3e97b ("pahole: add libbpf as submodule under lib/bpf").
It works well with the git repository because the libbpf submodule will
use "git submodule update --init --recursive" to update.
(自提交 21507cd3e97b ("pahole: add libbpf as submodule under lib/bpf") 之后，
pahole 从 v1.13 起开始使用 libbpf 的定义和 API。它与 git 仓库配合良好，因为
libbpf 子模块会使用 "git submodule update --init --recursive" 来更新。)

Unfortunately, the default github release source code does not contain
libbpf submodule source code and this will cause build issues, the tarball
from https://git.kernel.org/pub/scm/devel/pahole/pahole.git/ is same with
github, you can get the source tarball with corresponding libbpf submodule
codes from

https://fedorapeople.org/~acme/dwarves

(遗憾的是，GitHub 默认发布的源码不包含 libbpf 子模块的源码，这会导致构建问题。
https://git.kernel.org/pub/scm/devel/pahole/pahole.git/ 的 tarball 与 GitHub 的
相同，你可以从以下地址获取带有对应 libbpf 子模块代码的源码 tarball：)

Some distros have pahole version 1.16 packaged already, e.g.
Fedora, Gentoo.
(一些发行版已经打包了 pahole 1.16，例如 Fedora、Gentoo。)

Q: Which BPF kernel selftests version should I run my kernel against?(问：我应该让内核搭配哪个版本的 BPF 内核 selftests 运行？)
-------------------------------------------------------------------------------------------------------------------------------
A: If you run a kernel ``xyz``, then always run the BPF kernel selftests
from that kernel ``xyz`` as well. Do not expect that the BPF selftest
from the latest mainline tree will pass all the time.
(A: 如果你运行内核 ``xyz`` ，那么就总是运行同一内核 ``xyz`` 中的 BPF 内核
selftests。不要指望来自最新主线树的 BPF selftest 会一直全部通过。)

In particular, test_bpf.c and test_verifier.c have a large number of
test cases and are constantly updated with new BPF test sequences, or
existing ones are adapted to verifier changes e.g. due to verifier
becoming smarter and being able to better track certain things.
(特别是，test_bpf.c 和 test_verifier.c 拥有大量测试用例，并且会不断更新以加入
新的 BPF 测试序列，或对现有用例做出调整以适应验证器的变化，例如验证器变得更
聪明、能够更好地跟踪某些东西。)

LLVM(LLVM 编译器框架)
=====================

Q: Where do I find LLVM with BPF support?(问：在哪里可以找到支持 BPF 的 LLVM？)
-------------------------------------------------------------------------------
A: The BPF back end for LLVM is upstream in LLVM since version 3.7.1.
(A: LLVM 的 BPF 后端自 3.7.1 版本起已进入 LLVM 上游。)

All major distributions these days ship LLVM with BPF back end enabled,
so for the majority of use-cases it is not required to compile LLVM by
hand anymore, just install the distribution provided package.
(如今所有主流发行版附带的 LLVM 都启用了 BPF 后端，因此对大多数使用场景而言，
已无需再手动编译 LLVM，安装发行版提供的软件包即可。)

LLVM's static compiler lists the supported targets through
``llc --version``, make sure BPF targets are listed. Example::

     $ llc --version
     LLVM (http://llvm.org/):
       LLVM version 10.0.0
       Optimized build.
       Default target: x86_64-unknown-linux-gnu
       Host CPU: skylake

       Registered Targets:
         aarch64    - AArch64 (little endian)
         bpf        - BPF (host endian)
         bpfeb      - BPF (big endian)
         bpfel      - BPF (little endian)
         x86        - 32-bit X86: Pentium-Pro and above
         x86-64     - 64-bit X86: EM64T and AMD64

(LLVM 的静态编译器通过 ``llc --version`` 列出受支持的目标，请确保 BPF 目标在列
其中。示例：)

For developers in order to utilize the latest features added to LLVM's
BPF back end, it is advisable to run the latest LLVM releases. Support
for new BPF kernel features such as additions to the BPF instruction
set are often developed together.
(对于希望使用 LLVM BPF 后端最新特性的开发者，建议运行最新的 LLVM 版本。诸如
向 BPF 指令集添加内容之类的新 BPF 内核特性通常是协同开发的。)

All LLVM releases can be found at: http://releases.llvm.org/
(所有 LLVM 发布版本都可以在以下地址找到：http://releases.llvm.org/)

Q: Got it, so how do I build LLVM manually anyway?(问：明白了，那么究竟该如何手动构建 LLVM？)
---------------------------------------------------------------------------------------------
A: We recommend that developers who want the fastest incremental builds
use the Ninja build system, you can find it in your system's package
manager, usually the package is ninja or ninja-build.
(A: 我们建议想要最快增量构建的开发者使用 Ninja 构建系统，可以在系统的包管理器
中找到它，软件包名通常是 ninja 或 ninja-build。)

You need ninja, cmake and gcc-c++ as build requisites for LLVM. Once you
have that set up, proceed with building the latest LLVM and clang version
from the git repositories::

     $ git clone https://github.com/llvm/llvm-project.git
     $ mkdir -p llvm-project/llvm/build
     $ cd llvm-project/llvm/build
     $ cmake .. -G "Ninja" -DLLVM_TARGETS_TO_BUILD="BPF;X86" \
                -DLLVM_ENABLE_PROJECTS="clang"    \
                -DCMAKE_BUILD_TYPE=Release        \
                -DLLVM_BUILD_RUNTIME=OFF
     $ ninja

(构建 LLVM 需要 ninja、cmake 和 gcc-c++ 作为先决条件。完成这些准备后，继续从
git 仓库构建最新的 LLVM 和 clang 版本：)

The built binaries can then be found in the build/bin/ directory, where
you can point the PATH variable to.
(构建出的二进制文件可以在 build/bin/ 目录中找到，你可以把 PATH 变量指向
那里。)

Set ``-DLLVM_TARGETS_TO_BUILD`` equal to the target you wish to build, you
will find a full list of targets within the llvm-project/llvm/lib/Target
directory.
(把 ``-DLLVM_TARGETS_TO_BUILD`` 设置为你想构建的目标；在
llvm-project/llvm/lib/Target 目录中可以找到目标的完整列表。)

Q: Reporting LLVM BPF issues(问：报告 LLVM BPF 问题)
----------------------------------------------------
Q: Should I notify BPF kernel maintainers about issues in LLVM's BPF code
generation back end or about LLVM generated code that the verifier
refuses to accept?
(问：关于 LLVM 的 BPF 代码生成后端中的问题，或验证器拒绝接受的 LLVM 生成代码，
我应当通知 BPF 内核维护者吗？)

A: Yes, please do!
(A: 是的，请务必告知！)

LLVM's BPF back end is a key piece of the whole BPF
infrastructure and it ties deeply into verification of programs from the
kernel side. Therefore, any issues on either side need to be investigated
and fixed whenever necessary.
(LLVM 的 BPF 后端是整个 BPF 基础设施的关键部分，它与内核侧的程序验证深度
耦合。因此，任何一侧的问题都需要调查并在必要时修复。)

Therefore, please make sure to bring them up at netdev kernel mailing
list and Cc BPF maintainers for LLVM and kernel bits:
(因此，请务必在 netdev 内核邮件列表上提出这些问题，并 Cc 负责 LLVM 与内核
部分的 BPF 维护者：)

* Yonghong Song <yhs@fb.com>
* Alexei Starovoitov <ast@kernel.org>
* Daniel Borkmann <daniel@iogearbox.net>

LLVM also has an issue tracker where BPF related bugs can be found:
(LLVM 也有一个问题跟踪器，可以在其中找到与 BPF 相关的缺陷：)

  https://bugs.llvm.org/buglist.cgi?quicksearch=bpf

However, it is better to reach out through mailing lists with having
maintainers in Cc.
(不过，更好的做法是通过邮件列表联系，并把维护者加入 Cc。)

Q: New BPF instruction for kernel and LLVM(问：为内核和 LLVM 添加新 BPF 指令)
-----------------------------------------------------------------------------
Q: I have added a new BPF instruction to the kernel, how can I integrate
it into LLVM?
(问：我向内核添加了一条新的 BPF 指令，如何把它集成进 LLVM？)

A: LLVM has a ``-mcpu`` selector for the BPF back end in order to allow
the selection of BPF instruction set extensions. Before llvm version 20,
the ``generic`` processor target is used, which is the base instruction
set (v1) of BPF. Since llvm 20, the default processor target has changed
to instruction set v3.
(A: LLVM 为 BPF 后端提供了 ``-mcpu`` 选择器，用于选择 BPF 指令集扩展。在
llvm 20 版本之前，使用的是 ``generic`` 处理器目标，即 BPF 的基础指令集
(v1)。自 llvm 20 起，默认处理器目标已改为指令集 v3。)

LLVM has an option to select ``-mcpu=probe`` where it will probe the host
kernel for supported BPF instruction set extensions and selects the
optimal set automatically.
(LLVM 提供了选择 ``-mcpu=probe`` 的选项，此时它会探测主机内核支持的 BPF 指令集
扩展并自动选择最优集合。)

For cross-compilation, a specific version can be select manually as well ::

     $ llc -march bpf -mcpu=help
     Available CPUs for this target:

       generic - Select the generic processor.
       probe   - Select the probe processor.
       v1      - Select the v1 processor.
       v2      - Select the v2 processor.
     [...]

(交叉编译时，也可以手动选择一个特定版本：)

Newly added BPF instructions to the Linux kernel need to follow the same
scheme, bump the instruction set version and implement probing for the
extensions such that ``-mcpu=probe`` users can benefit from the
optimization transparently when upgrading their kernels.
(向 Linux 内核新添加的 BPF 指令需要遵循同样的方案：提升指令集版本并为扩展实现
探测，使 ``-mcpu=probe`` 用户在升级内核时能透明地受益于该优化。)

If you are unable to implement support for the newly added BPF instruction
please reach out to BPF developers for help.
(如果你无法为新添加的 BPF 指令实现支持，请联系 BPF 开发者寻求帮助。)

By the way, the BPF kernel selftests run with ``-mcpu=probe`` for better
test coverage.
(顺便一提，BPF 内核 selftests 以 ``-mcpu=probe`` 运行，以获得更好的测试
覆盖。)

Q: clang flag for target bpf?(问：面向 bpf 目标的 clang 标志？)
---------------------------------------------------------------
Q: In some cases clang flag ``--target=bpf`` is used but in other cases the
default clang target, which matches the underlying architecture, is used.
What is the difference and when I should use which?
(问：有些情况下使用 clang 标志 ``--target=bpf`` ，另一些情况下则使用与底层
架构匹配的默认 clang 目标。二者有什么区别，何时该用哪一个？)

A: Although LLVM IR generation and optimization try to stay architecture
independent, ``--target=<arch>`` still has some impact on generated code:
(A: 尽管 LLVM IR 生成与优化尽量保持架构无关， ``--target=<arch>`` 仍会对生成
的代码产生一些影响：)

- BPF program may recursively include header file(s) with file scope
  inline assembly codes. The default target can handle this well,
  while ``bpf`` target may fail if bpf backend assembler does not
  understand these assembly codes, which is true in most cases.
  (BPF 程序可能递归包含带有文件作用域内联汇编代码的头文件。默认目标可以很好
  地处理这种情况，而 ``bpf`` 目标在 BPF 后端汇编器不理解这些汇编代码时可能
  失败——大多数情况下确实如此。)

- When compiled without ``-g``, additional elf sections, e.g.,
  .eh_frame and .rela.eh_frame, may be present in the object file
  with default target, but not with ``bpf`` target.
  (在不带 ``-g`` 编译时，默认目标生成的目标文件中可能存在额外的 elf 节，例如
  .eh_frame 和 .rela.eh_frame，而 ``bpf`` 目标则没有。)

- The default target may turn a C switch statement into a switch table
  lookup and jump operation. Since the switch table is placed
  in the global readonly section, the bpf program will fail to load.
  The bpf target does not support switch table optimization.
  The clang option ``-fno-jump-tables`` can be used to disable
  switch table generation.
  (默认目标可能把 C switch 语句转换为 switch 表查找与跳转操作。由于 switch
  表被放在全局只读段中，bpf 程序将加载失败。bpf 目标不支持 switch 表优化。
  clang 选项 ``-fno-jump-tables`` 可用于禁止生成 switch 表。)

- For clang ``--target=bpf``, it is guaranteed that pointer or long /
  unsigned long types will always have a width of 64 bit, no matter
  whether underlying clang binary or default target (or kernel) is
  32 bit. However, when native clang target is used, then it will
  compile these types based on the underlying architecture's conventions,
  meaning in case of 32 bit architecture, pointer or long / unsigned
  long types e.g. in BPF context structure will have width of 32 bit
  while the BPF LLVM back end still operates in 64 bit. The native
  target is mostly needed in tracing for the case of walking ``pt_regs``
  or other kernel structures where CPU's register width matters.
  Otherwise, ``clang --target=bpf`` is generally recommended.
  (对于 clang ``--target=bpf`` ，可以保证 pointer 或 long / unsigned long
  类型总是 64 位宽，无论底层 clang 二进制或默认目标（或内核）是否为 32 位。
  然而，当使用本机 clang 目标时，这些类型将按底层架构的约定编译，也就是说在
  32 位架构上，BPF 上下文结构体中的 pointer 或 long / unsigned long 类型将是
  32 位宽，而 BPF LLVM 后端仍按 64 位运作。本机目标主要用于跟踪场景中遍历
  ``pt_regs`` 或其他 CPU 寄存器宽度有意义的内核结构体的情况。除此之外，一般
  推荐使用 ``clang --target=bpf`` 。)

You should use default target when:
(在以下情况下应使用默认目标：)

- Your program includes a header file, e.g., ptrace.h, which eventually
  pulls in some header files containing file scope host assembly codes.
  (你的程序包含一个头文件（例如 ptrace.h），它最终会拉入一些包含文件作用域
  主机汇编代码的头文件。)

- You can add ``-fno-jump-tables`` to work around the switch table issue.
  (你可以添加 ``-fno-jump-tables`` 来规避 switch 表问题。)

Otherwise, you can use ``bpf`` target. Additionally, you *must* use bpf target
when:
(否则，你可以使用 ``bpf`` 目标。此外，在以下情况下你 *必须* 使用 bpf 目标：)

- Your program uses data structures with pointer or long / unsigned long
  types that interface with BPF helpers or context data structures. Access
  into these structures is verified by the BPF verifier and may result
  in verification failures if the native architecture is not aligned with
  the BPF architecture, e.g. 64-bit. An example of this is
  BPF_PROG_TYPE_SK_MSG require ``--target=bpf``
  (你的程序使用与 BPF 辅助函数或上下文数据结构交互的、含 pointer 或 long /
  unsigned long 类型的数据结构。BPF 验证器会验证对这些结构的访问；如果本机
  架构与 BPF 架构（例如 64 位）不一致，可能导致验证失败。一个例子是
  BPF_PROG_TYPE_SK_MSG 需要 ``--target=bpf`` 。)
(否则，你可以使用 ``bpf`` 目标。此外，在以下情况下你 *必须* 使用 bpf 目标：)
(你的程序使用与 BPF 辅助函数或上下文数据结构交互的、含 pointer 或 long /
unsigned long 类型的数据结构。BPF 验证器会验证对这些结构的访问；如果本机
架构与 BPF 架构（例如 64 位）不一致，可能导致验证失败。一个例子是
BPF_PROG_TYPE_SK_MSG 需要 ``--target=bpf`` 。)


.. Links
.. _selftests:
   https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/testing/selftests/bpf/

Happy BPF hacking!
(祝 BPF 开发愉快！)
