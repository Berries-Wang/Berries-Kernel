.. SPDX-License-Identifier: GPL-2.0

.. _netdev-FAQ:

==================================================
Networking subsystem (netdev)(网络子系统 (netdev))
==================================================

tl;dr(要点)
-----------

 - designate your patch to a tree - ``[PATCH net]`` or ``[PATCH net-next]``
 - for fixes the ``Fixes:`` tag is required, regardless of the tree
 - don't post large series (> 15 patches), break them up
 - don't repost your patches within one 24h period
 - reverse xmas tree

 - designate your patch to a tree - ``[PATCH net]`` or ``[PATCH net-next]``
   (把补丁指定到某个树 - ``[PATCH net]`` 或 ``[PATCH net-next]``。)
 - for fixes the ``Fixes:`` tag is required, regardless of the tree
   (修复类补丁无论进哪个树都必须带 ``Fixes:`` 标签。)
 - don't post large series (> 15 patches), break them up
   (不要投递大系列(超过 15 个补丁)，要拆分开。)
 - don't repost your patches within one 24h period
   (24 小时内不要重复投递同一批补丁。)
 - reverse xmas tree
   (局部变量按倒圣诞树 (reverse xmas tree) 顺序排列。)

netdev(netdev 邮件列表)
-----------------------

netdev is a mailing list for all network-related Linux stuff.  This
includes anything found under net/ (i.e. core code like IPv6) and
drivers/net (i.e. hardware specific drivers) in the Linux source tree.
(netdev 是一个涵盖所有 Linux 网络相关事务的邮件列表。包括 Linux 源码树中
net/ 下的所有内容(即 IPv6 之类的核心代码)和 drivers/net(即硬件专属驱动)。)

Note that some subsystems (e.g. wireless drivers) which have a high
volume of traffic have their own specific mailing lists and trees.
(注意，某些流量很大的子系统(例如无线驱动)有自己的专属邮件列表和树。)

Like many other Linux mailing lists, the netdev list is hosted at
kernel.org with archives available at https://lore.kernel.org/netdev/.
(与许多其他 Linux 邮件列表一样，netdev 列表托管在 kernel.org 上，其归档见
https://lore.kernel.org/netdev/。)

Aside from subsystems like those mentioned above, all network-related
Linux development (i.e. RFC, review, comments, etc.) takes place on
netdev.
(除上述这类子系统之外，所有 Linux 网络相关的开发(即 RFC、评审、评论等)
都在 netdev 上进行。)

Development cycle(开发周期)
---------------------------

Here is a bit of background information on
the cadence of Linux development.  Each new release starts off with a
two week "merge window" where the main maintainers feed their new stuff
to Linus for merging into the mainline tree.  After the two weeks, the
merge window is closed, and it is called/tagged ``-rc1``.  No new
features get mainlined after this -- only fixes to the rc1 content are
expected.  After roughly a week of collecting fixes to the rc1 content,
rc2 is released.  This repeats on a roughly weekly basis until rc7
(typically; sometimes rc6 if things are quiet, or rc8 if things are in a
state of churn), and a week after the last vX.Y-rcN was done, the
official vX.Y is released.
(这里提供一些关于 Linux 开发节奏的背景信息。每个新版本都从为期两周的
"合并窗口" (merge window) 开始，期间各主要维护者把他们的新东西交给 Linus
合入主线树。两周之后合并窗口关闭，此时称为/打标 ``-rc1`` 。此后不再有新特
性进入主线——预期只提交针对 rc1 内容的修复。在收集 rc1 内容的修复大约一
周之后，发布 rc2。之后大致每周重复一次，直到 rc7(通常如此；有时如果风平
浪静则是 rc6，或者如果事情动荡不安则是 rc8)，而在最后一个 vX.Y-rcN 完成的
一周之后，正式的 vX.Y 发布。)

To find out where we are now in the cycle - load the mainline (Linus)
page here:
(要了解我们现在处于开发周期的哪个阶段，请在这里加载主线(Linus)页面：)

  https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

and note the top of the "tags" section.  If it is rc1, it is early in
the dev cycle.  If it was tagged rc7 a week ago, then a release is
probably imminent. If the most recent tag is a final release tag
(without an ``-rcN`` suffix) - we are most likely in a merge window
and ``net-next`` is closed.
(并注意 "tags" 一节的顶部。如果是 rc1，说明开发周期尚早。如果一周前打的是
rc7 标，那么发布大概率已近在眼前。如果最新的 tag 是正式发布 tag(不带
``-rcN`` 后缀)，那么我们很可能正处于合并窗口中，``net-next`` 已关闭。)

git trees and patch flow(git 树与补丁流转)
------------------------------------------

There are two networking trees (git repositories) in play.  Both are
driven by David Miller, the main network maintainer.  There is the
``net`` tree, and the ``net-next`` tree.  As you can probably guess from
the names, the ``net`` tree is for fixes to existing code already in the
mainline tree from Linus, and ``net-next`` is where the new code goes
for the future release.  You can find the trees here:
(这里涉及两个网络树(git 仓库)，都由网络主维护者 David Miller 管理。即
``net`` 树和 ``net-next`` 树。从名字大概能猜到，``net`` 树用于修复 Linus
主线树中已有代码的问题，而 ``net-next`` 则是面向未来发布的新代码的去处。
两个树见：)

- https://git.kernel.org/pub/scm/linux/kernel/git/netdev/net.git
- https://git.kernel.org/pub/scm/linux/kernel/git/netdev/net-next.git

Relating that to kernel development: At the beginning of the 2-week
merge window, the ``net-next`` tree will be closed - no new changes/features.
The accumulated new content of the past ~10 weeks will be passed onto
mainline/Linus via a pull request for vX.Y -- at the same time, the
``net`` tree will start accumulating fixes for this pulled content
relating to vX.Y
(结合内核开发来看：在为期两周的合并窗口开始时，``net-next`` 树会关闭——
不再接受新的变更/特性。过去约 10 周累积的新内容会通过针对 vX.Y 的 pull
request 交给主线/Linus；与此同时，``net`` 树将开始累积针对这份已拉取内容
的、与 vX.Y 相关的修复。)

An announcement indicating when ``net-next`` has been closed is usually
sent to netdev, but knowing the above, you can predict that in advance.
(通常会在 netdev 上发通知，说明 ``net-next`` 何时关闭；但了解了上述规则，
你可以提前预测。)

.. warning::
  Do not send new ``net-next`` content to netdev during the
  period during which ``net-next`` tree is closed.
  (在 ``net-next`` 树关闭期间，不要向 netdev 发送新的 ``net-next`` 内容。)

RFC patches sent for review only are obviously welcome at any time
(use ``--subject-prefix='RFC net-next'`` with ``git format-patch``).
(仅供评审的 RFC 补丁显然任何时候都欢迎(用 ``git format-patch`` 时加上
``--subject-prefix='RFC net-next'``)。)

Shortly after the two weeks have passed (and vX.Y-rc1 is released), the
tree for ``net-next`` reopens to collect content for the next (vX.Y+1)
release.
(两周过后不久(即 vX.Y-rc1 发布之后)，``net-next`` 树会重新开放，收集下一
个(vX.Y+1)发布的内容。)

If you aren't subscribed to netdev and/or are simply unsure if
``net-next`` has re-opened yet, simply check the ``net-next`` git
repository link above for any new networking-related commits.  You may
also check the following website for the current status:
(如果你没有订阅 netdev，和/或只是不确定 ``net-next`` 是否已经重新开放，
只需查看上面 ``net-next`` git 仓库链接里有没有新的网络相关提交即可。也可
以查看以下网站了解当前状态：)

  https://netdev.bots.linux.dev/net-next.html

The ``net`` tree continues to collect fixes for the vX.Y content, and is
fed back to Linus at regular (~weekly) intervals.  Meaning that the
focus for ``net`` is on stabilization and bug fixes.
(``net`` 树继续收集针对 vX.Y 内容的修复，并以定期(约每周)的节奏交回
Linus。也就是说，``net`` 的重点是稳定化和缺陷修复。)

Finally, the vX.Y gets released, and the whole cycle starts over.
(最后，vX.Y 正式发布，整个周期周而复始。)

netdev patch review(netdev 补丁评审)
------------------------------------

.. _patch_status:

Patch status(补丁状态)
~~~~~~~~~~~~~~~~~~~~~~

Status of a patch can be checked by looking at the main patchwork
queue for netdev:
(补丁状态可以在 netdev 的主 patchwork 队列中查看：)

  https://patchwork.kernel.org/project/netdevbpf/list/

The "State" field will tell you exactly where things are at with your
patch:
("State" 字段会准确告诉你补丁目前的进展：)

================== =============================================================
Patch state(状态)  Description(说明)
================== =============================================================
New, Under review  pending review, patch is in the maintainer’s queue for
                   review; the two states are used interchangeably (depending on
                   the exact co-maintainer handling patchwork at the time)
                   (待评审，补丁在维护者队列中等待评审；两种状态可互换使用(取
                   决于当时处理 patchwork 的具体共同维护者))
Accepted           patch was applied to the appropriate networking tree, this is
                   usually set automatically by the pw-bot
                   (补丁已应用到相应的网络树，通常由 pw-bot 自动设置)
Needs ACK          waiting for an ack from an area expert or testing
                   (等待领域专家的 ack 或测试结果)
Changes requested  patch has not passed the review, new revision is expected
                   with appropriate code and commit message changes
                   (补丁未通过评审，预期提交带有相应代码与提交说明修改的新版本)
Rejected           patch has been rejected and new revision is not expected
                   (补丁已被拒绝，不再期待新版本)
Not applicable     patch is expected to be applied outside of the networking
                   subsystem
                   (预期该补丁将在网络子系统之外应用)
Awaiting upstream  patch should be reviewed and handled by appropriate
                   sub-maintainer, who will send it on to the networking trees;
                   patches set to ``Awaiting upstream`` in netdev's patchwork
                   will usually remain in this state, whether the sub-maintainer
                   requested changes, accepted or rejected the patch
                   (补丁应由相应的子维护者评审与处理，由其转交网络树；netdev
                   patchwork 中被设为 ``Awaiting upstream`` 的补丁通常保持此
                   状态不变，无论子维护者是要求修改、接受还是拒绝了补丁)
Deferred           patch needs to be reposted later, usually due to dependency
                   or because it was posted for a closed tree
                   (补丁需要稍后重新投递，通常因为依赖关系，或因为它投给了
                   已关闭的树)
Superseded         new version of the patch was posted, usually set by the
                   pw-bot
                   (补丁的新版本已投递，通常由 pw-bot 设置)
RFC                not to be applied, usually not in maintainer’s review queue,
                   pw-bot can automatically set patches to this state based
                   on subject tags
                   (不会被应用，通常不在维护者评审队列中；pw-bot 可根据主题
                   标签自动把补丁置为此状态)
================== =============================================================

Patches are indexed by the ``Message-ID`` header of the emails
which carried them so if you have trouble finding your patch append
the value of ``Message-ID`` to the URL above.
(补丁按承载它们的邮件的 ``Message-ID`` 头索引，因此如果你难以找到自己的补
丁，可以把 ``Message-ID`` 的值附加到上面的 URL 后面。)

Updating patch status(更新补丁状态)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Contributors and reviewers do not have the permissions to update patch
state directly in patchwork. Patchwork doesn't expose much information
about the history of the state of patches, therefore having multiple
people update the state leads to confusion.
(贡献者和评审者没有直接在 patchwork 中更新补丁状态的权限。patchwork 不暴
露多少补丁状态历史信息，因此让多个人更新状态会导致混乱。)

Instead of delegating patchwork permissions netdev uses a simple mail
bot which looks for special commands/lines within the emails sent to
the mailing list. For example to mark a series as Changes Requested
one needs to send the following line anywhere in the email thread::

  pw-bot: changes-requested

(netdev 没有下放 patchwork 权限，而是使用一个简单的邮件机器人，它会在发到
邮件列表的邮件中查找特殊命令/行。例如，要把一个系列标记为 Changes
Requested，需要在邮件会话的任意位置发送上面这行。)

As a result the bot will set the entire series to Changes Requested.
This may be useful when author discovers a bug in their own series
and wants to prevent it from getting applied.
(随后机器人会把整个系列设置为 Changes Requested。当作者发现自己的系列中
有缺陷、想阻止它被应用时，这会很有用。)

The use of the bot is entirely optional, if in doubt ignore its existence
completely. Maintainers will classify and update the state of the patches
themselves. No email should ever be sent to the list with the main purpose
of communicating with the bot, the bot commands should be seen as metadata.
(机器人的使用完全是可选的；拿不准就当它不存在。维护者会自行分类并更新补
丁状态。绝不应当以与机器人通信为主要目的给列表发邮件，机器人命令应被视为
元数据。)

The use of the bot is restricted to authors of the patches (the ``From:``
header on patch submission and command must match!), maintainers of
the modified code according to the MAINTAINERS file (again, ``From:``
must match the MAINTAINERS entry) and a handful of senior reviewers.
(机器人的使用仅限于补丁作者(补丁提交与命令的 ``From:`` 头必须一致！)、按
MAINTAINERS 文件认定所改代码的维护者(同样，``From:`` 必须与 MAINTAINERS
条目一致)以及少数资深评审者。)

Bot records its activity here:
(机器人在这里记录其活动：)

  https://netdev.bots.linux.dev/pw-bot.html

Review timelines(评审时间线)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Generally speaking, the patches get triaged quickly (in less than
48h). But be patient, if your patch is active in patchwork (i.e. it's
listed on the project's patch list) the chances it was missed are close to zero.
(一般来说，补丁会被很快分诊(48 小时内)。但请保持耐心：如果你的补丁在
patchwork 中处于活动状态(即它列在项目的补丁列表上)，它被漏掉的可能性几乎
为零。)

The high volume of development on netdev makes reviewers move on
from discussions relatively quickly. New comments and replies
are very unlikely to arrive after a week of silence. If a patch
is no longer active in patchwork and the thread went idle for more
than a week - clarify the next steps and/or post the next version.
(netdev 上开发量巨大，评审者会相对较快地从讨论中抽身。沉默一周之后，再收
到新评论或回复的可能性很小。如果补丁在 patchwork 中已不活跃、会话线程也
闲置超过一周——请澄清下一步怎么做，和/或投递下一个版本。)

For RFC postings specifically, if nobody responded in a week - reviewers
either missed the posting or have no strong opinions. If the code is ready,
repost as a PATCH.
(对于 RFC 投递，如果一周内无人回应——评审者要么漏看了投递，要么没有强烈
意见。如果代码已就绪，就以 PATCH 重新投递。)

Emails saying just "ping" or "bump" are considered rude. If you can't figure
out the status of the patch from patchwork or where the discussion has
landed - describe your best guess and ask if it's correct. For example::

  I don't understand what the next steps are. Person X seems to be unhappy
  with A, should I do B and repost the patches?

(只写 "ping" 或 "bump" 的邮件被认为是无礼的。如果你无法从 patchwork 弄清
补丁状态、或不知道讨论进行到了哪一步——请描述你自己的最佳猜测并询问其是
否正确，比如上面这样。)

.. _Changes requested:

Changes requested(请求修改)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Patches :ref:`marked<patch_status>` as ``Changes Requested`` need
to be revised. The new version should come with a change log,
preferably including links to previous postings, for example::

  [PATCH net-next v3] net: make cows go moo

  Even users who don't drink milk appreciate hearing the cows go "moo".

  The amount of mooing will depend on packet rate so should match
  the diurnal cycle quite well.

  Signed-off-by: Joe Defarmer <joe@barn.org>
  ---
  v3:
    - add a note about time-of-day mooing fluctuation to the commit message
  v2: https://lore.kernel.org/netdev/123themessageid@barn.org/
    - fix missing argument in kernel doc for netif_is_bovine()
    - fix memory leak in netdev_register_cow()
  v1: https://lore.kernel.org/netdev/456getstheclicks@barn.org/

(被 :ref:`标记<patch_status>` 为 ``Changes Requested`` 的补丁需要修订。
新版本应附变更日志 (change log)，最好包含此前投递的链接，格式如上例。)

The commit message should be revised to answer any questions reviewers
had to ask in previous discussions. Occasionally the update of
the commit message will be the only change in the new version.
(提交说明 (commit message) 应当修订，以回答评审者在之前讨论中不得不提出的
问题。偶尔，更新提交说明就是新版本中的唯一改动。)

Partial resends(部分重发)
~~~~~~~~~~~~~~~~~~~~~~~~~

Please always resend the entire patch series and make sure you do number your
patches such that it is clear this is the latest and greatest set of patches
that can be applied. Do not try to resend just the patches which changed.
(请始终重发整个补丁系列，并确保给补丁编号，让人清楚这是可应用的最新、最终
的一组补丁。不要试图只重发有改动的那些补丁。)

Handling misapplied patches(处理误应用的补丁)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Occasionally a patch series gets applied before receiving critical feedback,
or the wrong version of a series gets applied.
(偶尔会出现补丁系列在收到关键反馈之前就被应用，或者系列被应用的版本不对。)

Making the patch disappear once it is pushed out is not possible, the commit
history in netdev trees is immutable.
Please send incremental versions on top of what has been merged in order to fix
the patches the way they would look like if your latest patch series was to be
merged.
(补丁一旦被推送出去就无法让它消失，netdev 树中的提交历史是不可变的。请在
已合并内容之上发送增量版本，把补丁修正成"假如你的最新补丁系列被合并"时应
有的样子。)

In cases where full revert is needed the revert has to be submitted
as a patch to the list with a commit message explaining the technical
problems with the reverted commit. Reverts should be used as a last resort,
when original change is completely wrong; incremental fixes are preferred.
(在需要完全回退 (revert) 的情况下，回退必须作为补丁提交到列表，提交说明需
解释被回退提交的技术问题。回退应作为最后手段使用，仅当原始改动完全错误时
才用；首选增量修复。)

Stable tree(stable 树)
~~~~~~~~~~~~~~~~~~~~~~

While it used to be the case that netdev submissions were not supposed
to carry explicit ``CC: stable@vger.kernel.org`` tags that is no longer
the case today. Please follow the standard stable rules in
:ref:`Documentation/process/stable-kernel-rules.rst <stable_kernel_rules>`,
and make sure you include appropriate Fixes tags!
(过去 netdev 投递不应带显式 ``CC: stable@vger.kernel.org`` 标签，如今已不
是这样了。请遵循
:ref:`Documentation/process/stable-kernel-rules.rst <stable_kernel_rules>`
中的标准 stable 规则，并确保带上恰当的 Fixes 标签！)

Security fixes(安全修复)
~~~~~~~~~~~~~~~~~~~~~~~~

Do not email netdev maintainers directly if you think you discovered
a bug that might have possible security implications.
The current netdev maintainer has consistently requested that
people use the mailing lists and not reach out directly.  If you aren't
OK with that, then perhaps consider mailing security@kernel.org or
reading about http://oss-security.openwall.org/wiki/mailing-lists/distros
as possible alternative mechanisms.
(如果你认为自己发现了一个可能有安全影响的缺陷，不要直接给 netdev 维护者
发邮件。现任 netdev 维护者一贯要求人们使用邮件列表而不是私下联系。如果你
不能接受这一点，那么或许可以考虑发邮件给 security@kernel.org，或阅读
http://oss-security.openwall.org/wiki/mailing-lists/distros，作为可能的替
代机制。)


Co-posting changes to user space components(用户空间组件变更的联合发布)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

User space code exercising kernel features should be posted
alongside kernel patches. This gives reviewers a chance to see
how any new interface is used and how well it works.
(使用内核特性的用户空间代码应当与内核补丁一同投递。这让评审者有机会看到
新接口如何被使用以及效果如何。)

When user space tools reside in the kernel repo itself all changes
should generally come as one series. If series becomes too large
or the user space project is not reviewed on netdev include a link
to a public repo where user space patches can be seen.
(当用户空间工具就位于内核仓库内时，所有变更通常应作为一个系列投递。如果
系列过大，或该用户空间项目不在 netdev 上评审，请附上一个公共仓库链接，让
人能看到用户空间补丁。)

In case user space tooling lives in a separate repository but is
reviewed on netdev  (e.g. patches to ``iproute2`` tools) kernel and
user space patches should form separate series (threads) when posted
to the mailing list, e.g.::

  [PATCH net-next 0/3] net: some feature cover letter
   └─ [PATCH net-next 1/3] net: some feature prep
   └─ [PATCH net-next 2/3] net: some feature do it
   └─ [PATCH net-next 3/3] selftest: net: some feature

  [PATCH iproute2-next] ip: add support for some feature

(如果用户空间工具位于单独的仓库但仍在 netdev 上评审(例如 ``iproute2``
工具的补丁)，那么投递到邮件列表时，内核补丁与用户空间补丁应形成独立的系
列(线程)，如上例。)

Posting as one thread is discouraged because it confuses patchwork
(as of patchwork 2.2.2).
(不建议作为一个线程投递，因为这会让 patchwork 混乱(patchwork 2.2.2 时如
此)。)

Co-posting selftests(联合发布自测)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Selftests should be part of the same series as the code changes.
Specifically for fixes both code change and related test should go into
the same tree (the tests may lack a Fixes tag, which is expected).
Mixing code changes and test changes in a single commit is discouraged.
(自测 (selftests) 应当与代码改动属于同一个系列。特别是修复类：代码改动与
相关测试应进入同一个树(测试可以没有 Fixes 标签，这是正常的)。不建议把代
码改动与测试改动混在一个提交里。)

Preparing changes(准备变更)
---------------------------

Attention to detail is important.  Re-read your own work as if you were the
reviewer.  You can start with using ``checkpatch.pl``, perhaps even with
the ``--strict`` flag.  But do not be mindlessly robotic in doing so.
If your change is a bug fix, make sure your commit log indicates the
end-user visible symptom, the underlying reason as to why it happens,
and then if necessary, explain why the fix proposed is the best way to
get things done.  Don't mangle whitespace, and as is common, don't
mis-indent function arguments that span multiple lines.  If it is your
first patch, mail it to yourself so you can test apply it to an
unpatched tree to confirm infrastructure didn't mangle it.
(注重细节很重要。把自己当成评审者重读自己的作品。可以从 ``checkpatch.pl``
入手，甚至可以带上 ``--strict`` 标志。但不要机械盲从。如果你的改动是缺陷
修复，确保提交日志指明最终用户可见的症状、其发生的根本原因；如有必要，再
解释为什么所提修复是解决问题的最佳方式。不要弄乱空白字符，并且像常见的那
样，不要错误缩进跨多行的函数参数。如果是你的第一个补丁，先发邮件给自己，
以便测试性地应用到一个未打补丁的树上，确认基础设施没有损坏它。)

Finally, go back and read
:ref:`Documentation/process/submitting-patches.rst <submittingpatches>`
to be sure you are not repeating some common mistake documented there.
(最后，回头再读一遍
:ref:`Documentation/process/submitting-patches.rst <submittingpatches>`，
确保你没有在重犯其中记载的某些常见错误。)

Indicating target tree(指明目标树)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To help maintainers and CI bots you should explicitly mark which tree
your patch is targeting. Assuming that you use git, use the prefix
flag::

  git format-patch --subject-prefix='PATCH net-next' start..finish

(为了帮助维护者和 CI 机器人，你应当明确标记补丁面向哪个树。假设你使用
git，使用上面的 prefix 标志。)

Use ``net`` instead of ``net-next`` (always lower case) in the above for
bug-fix ``net`` content.
(对于修复类 ``net`` 内容，把上面命令中的 ``net-next`` 换成 ``net`` (始终
小写)。)

Dividing work into patches(把工作拆分为补丁)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Put yourself in the shoes of the reviewer. Each patch is read separately
and therefore should constitute a comprehensible step towards your stated
goal.
(把自己放在评审者的位置。每个补丁都是被单独阅读的，因此每个补丁都应当构
成通往你既定目标的一个可理解的步骤。)

Avoid sending series longer than 15 patches. Larger series takes longer
to review as reviewers will defer looking at it until they find a large
chunk of time. A small series can be reviewed in a short time, so Maintainers
just do it. As a result, a sequence of smaller series gets merged quicker and
with better review coverage. Re-posting large series also increases the mailing
list traffic.
(避免发送超过 15 个补丁的系列。更大的系列评审更慢，因为评审者会推迟查看，
直到找到一大块空闲时间。小系列可以在短时间内评审完，所以维护者会顺手做
掉。因此，一系列较小的系列合并得更快，评审覆盖也更好。重发大系列还会增加
邮件列表流量。)

Limit patches outstanding on mailing list(限制邮件列表上待评审补丁的数量)
-------------------------------------------------------------------------

Avoid having more than 15 patches, across all series, outstanding for
review on the mailing list for a single tree. In other words, a maximum of
15 patches under review on net, and a maximum of 15 patches under review on
net-next.
(避免让超过 15 个补丁(跨所有系列)同时 outstanding、在单个树的邮件列表上
等待评审。换句话说，net 上最多 15 个补丁在评审中，net-next 上也最多 15 个。)

This limit is intended to focus developer effort on testing patches before
upstream review. Aiding the quality of upstream submissions, and easing the
load on reviewers.
(该限制旨在促使开发者把精力集中在 upstream 评审之前对补丁进行测试上，有
助于提高 upstream 提交的质量，并减轻评审者的负担。)

.. _rcs:

Local variable ordering ("reverse xmas tree", "RCS")(局部变量排序("倒圣诞树", "RCS"))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Netdev has a convention for ordering local variables in functions.
Order the variable declaration lines longest to shortest, e.g.::

  struct scatterlist *sg;
  struct sk_buff *skb;
  int err, i;

(netdev 对函数内局部变量的排序有一个约定：把变量声明行按从长到短排序，如
上例。)

If there are dependencies between the variables preventing the ordering
move the initialization out of line.
(如果变量之间存在依赖、无法按此排序，就把初始化移到行外。)

Format precedence(格式优先级)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When working in existing code which uses nonstandard formatting make
your code follow the most recent guidelines, so that eventually all code
in the domain of netdev is in the preferred format.
(在使用非标准格式的现有代码中工作时，让你的代码遵循最新的准则，这样
netdev 领域内的所有代码最终都会变成首选格式。)

Using device-managed and cleanup.h constructs(使用 device-managed 与 cleanup.h 构造)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Netdev remains skeptical about promises of all "auto-cleanup" APIs,
including even ``devm_`` helpers, historically. They are not the preferred
style of implementation, merely an acceptable one.
(从历史上看，netdev 对所有"自动清理" API 的承诺始终持怀疑态度，甚至包括
``devm_`` 辅助函数。它们不是首选的实现风格，只是可接受的风格。)

Use of ``guard()`` is discouraged within any function longer than 20 lines,
``scoped_guard()`` is considered more readable. Using normal lock/unlock is
still (weakly) preferred.
(在超过 20 行的任何函数中不建议使用 ``guard()``，``scoped_guard()`` 被认
为更易读。普通 lock/unlock 仍然(弱)优先。)

Low level cleanup constructs (such as ``__free()``) can be used when building
APIs and helpers, especially scoped iterators. However, direct use of
``__free()`` within networking core and drivers is discouraged.
Similar guidance applies to declaring variables mid-function.
(构建 API 和辅助函数时可以使用低层清理构造(如 ``__free()``)，尤其是作用
域迭代器。但在网络核心和驱动中直接使用 ``__free()`` 是不建议的。类似的指
导也适用于在函数中部声明变量。)

Clean-up patches(清理类补丁)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Netdev discourages patches which perform simple clean-ups, which are not in
the context of other work. For example:
(netdev 不鼓励与其他工作无关的、只做简单清理的补丁。例如：)

* Addressing ``checkpatch.pl``, and other trivial coding style warnings
  (处理 ``checkpatch.pl`` 以及其他琐碎的编码风格警告)
* Addressing :ref:`Local variable ordering<rcs>` issues
  (处理 :ref:`Local variable ordering<rcs>` 问题)
* Conversions to device-managed APIs (``devm_`` helpers)
  (转换为 device-managed API(``devm_`` 辅助函数))

This is because it is felt that the churn that such changes produce comes
at a greater cost than the value of such clean-ups.
(这是因为人们认为这类改动造成的扰动，其代价大于这类清理的价值。)

Conversely, spelling and grammar fixes are not discouraged.
(相反，拼写和语法修正不受打压。)

Resending after review(评审后重发)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Allow at least 24 hours to pass between postings. This will ensure reviewers
from all geographical locations have a chance to chime in. Do not wait
too long (weeks) between postings either as it will make it harder for reviewers
to recall all the context.
(两次投递之间至少间隔 24 小时。这能确保身处各地时区的评审者都有机会参与
意见。但也不要等太久(数周)，那会让评审者更难回忆起全部上下文。)

Make sure you address all the feedback in your new posting. Do not post a new
version of the code if the discussion about the previous version is still
ongoing, unless directly instructed by a reviewer.
(确保新投递回应了所有反馈。如果关于上一版本的讨论仍在进行，不要投递新版
本代码，除非评审者直接指示你这样做。)

The new version of patches should be posted as a separate thread,
not as a reply to the previous posting. Change log should include a link
to the previous posting (see :ref:`Changes requested`).
(新版本补丁应作为单独线程投递，而不是作为对上一次投递的回复。变更日志应
包含此前投递的链接(见 :ref:`Changes requested`)。)

Testing(测试)
-------------

Expected level of testing(预期的测试程度)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At the very minimum your changes must survive an ``allyesconfig`` and an
``allmodconfig`` build with ``W=1`` set without new warnings or failures.
(至少，你的改动必须能在设置 ``W=1`` 的 ``allyesconfig`` 和
``allmodconfig`` 构建下通过，且没有新的警告或失败。)

Ideally you will have done run-time testing specific to your change,
and the patch series contains a set of kernel selftest for
``tools/testing/selftests/net`` or using the KUnit framework.
(理想情况下，你会针对自己的改动做过运行时测试，且补丁系列包含一组用于
``tools/testing/selftests/net`` 的内核自测，或使用 KUnit 框架。)

You are expected to test your changes on top of the relevant networking
tree (``net`` or ``net-next``) and not e.g. a stable tree or ``linux-next``.
(应当基于相关网络树(``net`` 或 ``net-next``)测试你的改动，而不是例如
stable 树或 ``linux-next``。)

patchwork checks(patchwork 检查)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Checks in patchwork are mostly simple wrappers around existing kernel
scripts, the sources are available at:
(patchwork 中的检查大多是对现有内核脚本的简单封装，源码见：)

https://github.com/linux-netdev/nipa/tree/master/tests

**Do not** post your patches just to run them through the checks.
You must ensure that your patches are ready by testing them locally
before posting to the mailing list. The patchwork build bot instance
gets overloaded very easily and netdev@vger really doesn't need more
traffic if we can help it.
(**不要** 仅仅为了让补丁跑一遍检查而投递它们。你必须通过在本地测试来确保
补丁就绪后再投递到邮件列表。patchwork 构建机器人实例很容易过载，如果可以
避免，netdev@vger 确实不需要更多流量。)

netdevsim(netdevsim 测试驱动)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``netdevsim`` is a test driver which can be used to exercise driver
configuration APIs without requiring capable hardware.
Mock-ups and tests based on ``netdevsim`` are encouraged when
adding new APIs with complex logic in the stack. The tests should
be written so that they can run both against ``netdevsim`` and a real
device (see ``tools/testing/selftests/drivers/net/README.rst``).
``netdevsim``-only tests should focus on testing corner cases
and failure paths in the core which are hard to exercise with a real driver.
(``netdevsim`` 是一个测试驱动，可以在没有可用硬件的情况下演练驱动配置
API。在为协议栈添加带复杂逻辑的新 API 时，鼓励基于 ``netdevsim`` 做模型与
测试。测试的编写应使其既能针对 ``netdevsim`` 运行，也能针对真实设备运行
(见 ``tools/testing/selftests/drivers/net/README.rst``)。仅针对
``netdevsim`` 的测试应聚焦于测试核心中那些用真实驱动难以覆盖的极端情况与
失败路径。)

``netdevsim`` in itself is **not** considered
a use case/user. You must also implement the new APIs in a real driver.
(``netdevsim`` 本身 **不** 被视为用例/用户。你还必须在真实驱动中实现新
API。)

We give no guarantees that ``netdevsim`` won't change in the future
in a way which would break what would normally be considered uAPI.
(对于 ``netdevsim`` 未来不会以破坏通常被视为 uAPI 的方式变化，我们不作出
任何保证。)

``netdevsim`` is reserved for use by upstream tests only, so any
new ``netdevsim`` features must be accompanied by selftests under
``tools/testing/selftests/``.
(``netdevsim`` 仅供 upstream 测试使用，因此任何新的 ``netdevsim`` 功能都
必须带有 ``tools/testing/selftests/`` 下的自测。)

Supported status for drivers(驱动的 Supported 状态)
---------------------------------------------------

.. note: The following requirements apply only to Ethernet NIC drivers.

Netdev defines additional requirements for drivers which want to acquire
the ``Supported`` status in the MAINTAINERS file. ``Supported`` drivers must
be running all upstream driver tests and reporting the results twice a day.
Drivers which do not comply with this requirement should use the ``Maintained``
status. There is currently no difference in how ``Supported`` and ``Maintained``
drivers are treated upstream.
(netdev 为想在 MAINTAINERS 文件中获得 ``Supported`` 状态的驱动定义了额外要
求。``Supported`` 驱动必须运行所有 upstream 驱动测试，并每天两次报告结果。
不满足此要求的驱动应使用 ``Maintained`` 状态。目前 ``Supported`` 与
``Maintained`` 驱动在 upstream 的待遇没有区别。)

The exact rules a driver must follow to acquire the ``Supported`` status:
(要获得 ``Supported`` 状态，驱动必须遵循的确切规则如下：)

1. Must run all tests under ``drivers/net`` and ``drivers/net/hw`` targets
   of Linux selftests. Running and reporting private / internal tests is
   also welcome, but upstream tests are a must.
   (必须运行 Linux 自测中 ``drivers/net`` 与 ``drivers/net/hw`` 目标下的所
   有测试。欢迎运行并报告私有/内部测试，但 upstream 测试是必须的。)

2. The minimum run frequency is once every 12 hours. Must test the
   designated branch from the selected branch feed. Note that branches
   are auto-constructed and exposed to intentional malicious patch posting,
   so the test systems must be isolated.
   (最低运行频率是每 12 小时一次。必须测试从所选分支源 (branch feed) 获得
   的指定分支。注意，分支是自动构建的，并暴露于蓄意恶意补丁投递之下，因此
   测试系统必须隔离。)

3. Drivers supporting multiple generations of devices must test at
   least one device from each generation. A testbed manifest (exact
   format TBD) should describe the device models tested.
   (支持多代设备的驱动必须至少测试每代中的一台设备。应有一份测试台清单
   (确切格式待定)描述所测试的设备型号。)

4. The tests must run reliably, if multiple branches are skipped or tests
   are failing due to execution environment problems the ``Supported``
   status will be withdrawn.
   (测试必须可靠运行；如果多个分支被跳过，或测试因执行环境问题而失败，
   ``Supported`` 状态将被撤销。)

5. Test failures due to bugs either in the driver or the test itself,
   or lack of support for the feature the test is targeting are
   *not* a basis for losing the ``Supported`` status.
   (由于驱动或测试自身的缺陷、或测试所针对的特性不受支持而导致的测试失
   败， *不是* 失去 ``Supported`` 状态的依据。)

netdev CI will maintain an official page of supported devices, listing their
recent test results.
(netdev CI 会维护一个官方的受支持设备页面，列出它们近期的测试结果。)

The driver maintainer may arrange for someone else to run the test,
there is no requirement for the person listed as maintainer (or their
employer) to be responsible for running the tests. Collaboration between
vendors, hosting GH CI, other repos under linux-netdev, etc. is most welcome.
(驱动维护者可以安排其他人来运行测试；并不要求列在维护者位置上的人(或其
雇主)负责运行测试。厂商之间的协作、托管 GH CI、linux-netdev 下的其他仓库
等都非常受欢迎。)

See https://github.com/linux-netdev/nipa/wiki for more information about
netdev CI. Feel free to reach out to maintainers or the list with any questions.
(关于 netdev CI 的更多信息见 https://github.com/linux-netdev/nipa/wiki。
有任何问题欢迎联系维护者或列表。)

Reviewer guidance(评审者指南)
-----------------------------

Reviewing other people's patches on the list is highly encouraged,
regardless of the level of expertise. For general guidance and
helpful tips please see :ref:`development_advancedtopics_reviews`.
(非常鼓励在列表上评审他人的补丁，无论专业水平如何。一般指南与有用提示请
参阅 :ref:`development_advancedtopics_reviews`。)

It's safe to assume that netdev maintainers know the community and the level
of expertise of the reviewers. The reviewers should not be concerned about
their comments impeding or derailing the patch flow. A Reviewed-by tag
is understood to mean "I have reviewed this code to the best of my ability"
rather than "I can attest this code is correct".
(可以放心地认为，netdev 维护者了解社区以及评审者的专业水平。评审者不必担
心自己的评论会阻碍或打乱补丁流转。Reviewed-by 标签的含义被理解为"我已尽
自己所能评审了这段代码"，而不是"我可以担保这段代码是正确的"。)

Reviewers are highly encouraged to do more in-depth review of submissions
and not focus exclusively on process issues, trivial or subjective
matters like code formatting, tags etc.
(强烈鼓励评审者对提交做更深入的评审，而不要只盯着流程问题、以及代码格式、
标签等琐碎或主观的事项。)

Testimonials / feedback(评价与反馈)
-----------------------------------

Some companies use peer feedback in employee performance reviews.
Please feel free to request feedback from netdev maintainers,
especially if you spend significant amount of time reviewing code
and go out of your way to improve shared infrastructure.
(一些公司在员工绩效评估中使用同行反馈。请放心向 netdev 维护者请求反馈，
尤其是当你花费大量时间评审代码、并特意投入精力改进共享基础设施时。)

The feedback must be requested by you, the contributor, and will always
be shared with you (even if you request for it to be submitted to your
manager).
(反馈必须由你——贡献者——亲自请求，并且总是会同你分享(即使你请求把反馈
提交给你的经理)。)
