.. SPDX-License-Identifier: (GPL-2.0+ OR CC-BY-4.0)
.. See the bottom of this file for additional redistribution information.

Handling regressions(处理回归)
++++++++++++++++++++++++++++++

*We don't cause regressions* -- this document describes what this "first rule of
Linux kernel development" means in practice for developers. It complements
Documentation/admin-guide/reporting-regressions.rst, which covers the topic from a
user's point of view; if you never read that text, go and at least skim over it
before continuing here.
(*我们不制造回归*——本文档描述这条"Linux 内核开发第一规则"在实践中对开发者意味着什么。它是对 Documentation/admin-guide/reporting-regressions.rst 的补充，后者从用户的视角覆盖这一主题；如果你从未读过那篇文字，请在继续阅读本文之前至少浏览一遍。)

The important bits (aka "The TL;DR")(重要要点（即"TL;DR"）)
===========================================================

#. Ensure subscribers of the `regression mailing list <https://lore.kernel.org/regressions/>`_
   (regressions@lists.linux.dev) quickly become aware of any new regression
   report:
   (1. 确保回归邮件列表的订阅者能迅速知晓任何新的回归报告：)

    * When receiving a mailed report that did not CC the list, bring it into the
      loop by immediately sending at least a brief "Reply-all" with the list
      CCed.
      (收到没有抄送该列表的邮件报告时，立即发送至少一段简短的"全部回复"并抄送该列表，把它带入循环。)

    * Forward or bounce any reports submitted in bug trackers to the list.
      (把任何提交到缺陷跟踪器的报告转发或退送给该列表。)

#. Make the Linux kernel regression tracking bot "regzbot" track the issue (this
   is optional, but recommended):
   (2. 让 Linux 内核回归跟踪机器人 "regzbot" 跟踪该问题（这是可选的，但推荐这样做）：)

    * For mailed reports, check if the reporter included a line like ``#regzbot
      introduced: v5.13..v5.14-rc1``. If not, send a reply (with the regressions
      list in CC) containing a paragraph like the following, which tells regzbot
      when the issue started to happen::

       #regzbot ^introduced: 1f2e3d4c5b6a

      (对于邮件报告，检查报告者是否包含了类似 ``#regzbot introduced: v5.13..v5.14-rc1`` 的行。如果没有，发送一封回复（抄送回归列表），包含如下所示的段落，告诉 regzbot 问题从什么时候开始出现。)

    * When forwarding reports from a bug tracker to the regressions list (see
      above), include a paragraph like the following::

       #regzbot introduced: v5.13..v5.14-rc1
       #regzbot from: Some N. Ice Human <some.human@example.com>
       #regzbot monitor: http://some.bugtracker.example.com/ticket?id=123456789

      (把缺陷跟踪器的报告转发到回归列表时（见上文），包含如下所示的段落。)

#. When submitting fixes for regressions, add "Closes:" tags to the patch
   description pointing to all places where the issue was reported, as
   mandated by Documentation/process/submitting-patches.rst and
   :ref:`Documentation/process/5.Posting.rst <development_posting>`. If you are
   only fixing part of the issue that caused the regression, you may use
   "Link:" tags instead. regzbot currently makes no distinction between the
   two.
   (3. 提交回归修复时，在补丁描述中添加 "Closes:" 标签，指向该问题被报告的所有位置——这是 Documentation/process/submitting-patches.rst 与 :ref:`Documentation/process/5.Posting.rst <development_posting>` 的要求。如果你只修复了导致回归的问题的一部分，可以改用 "Link:" 标签。regzbot 目前对两者不做区分。)

#. Try to fix regressions quickly once the culprit has been identified; fixes
   for most regressions should be merged within two weeks, but some need to be
   resolved within two or three days.
   (4. 一旦查明罪魁祸首，就尽快修复回归；大多数回归的修复应当在两周内合入，但有些需要在两三天内解决。)


All the details on Linux kernel regressions relevant for developers(与开发者相关的 Linux 内核回归的全部细节)
============================================================================================================


The important basics in more detail(重要基础知识的更多细节)
-----------------------------------------------------------


What to do when receiving regression reports(收到回归报告时该做什么)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ensure the Linux kernel's regression tracker and others subscribers of the
`regression mailing list <https://lore.kernel.org/regressions/>`_
(regressions@lists.linux.dev) become aware of any newly reported regression:
(确保 Linux 内核的回归跟踪员以及回归邮件列表的其他订阅者都能知晓任何新报告的回归：)

 * When you receive a report by mail that did not CC the list, immediately bring
   it into the loop by sending at least a brief "Reply-all" with the list CCed;
   try to ensure it gets CCed again in case you reply to a reply that omitted
   the list.
   (当你收到一封没有抄送该列表的邮件报告时，立即发送至少一段简短的"全部回复"并抄送该列表，把它带入循环；如果你回复的是一封漏掉该列表的回信，尽量确保列表再次被抄送。)

 * If a report submitted in a bug tracker hits your Inbox, forward or bounce it
   to the list. Consider checking the list archives beforehand, if the reporter
   already forwarded the report as instructed by
   Documentation/admin-guide/reporting-issues.rst.
   (如果一份提交到缺陷跟踪器的报告落入你的收件箱，请把它转发或退送给该列表。可以事先查看列表存档，确认报告者是否已经按 Documentation/admin-guide/reporting-issues.rst 的指示转发过该报告。)

When doing either, consider making the Linux kernel regression tracking bot
"regzbot" immediately start tracking the issue:
(无论做上面哪件事，都可以考虑让 Linux 内核回归跟踪机器人 "regzbot" 立即开始跟踪该问题：)

 * For mailed reports, check if the reporter included a "regzbot command" like
   ``#regzbot introduced: 1f2e3d4c5b6a``. If not, send a reply (with the
   regressions list in CC) with a paragraph like the following:::

       #regzbot ^introduced: v5.13..v5.14-rc1

   (对于邮件报告，检查报告者是否包含了类似 ``#regzbot introduced: 1f2e3d4c5b6a`` 的 "regzbot 命令"。如果没有，发送一封回复（抄送回归列表），包含如下所示的段落。)

   This tells regzbot the version range in which the issue started to happen;
   you can specify a range using commit-ids as well or state a single commit-id
   in case the reporter bisected the culprit.
   (这会告诉 regzbot 问题开始出现的版本范围；你也可以用 commit id 指定一个范围，或者——如果报告者已经二分定位出罪魁祸首——给出单个 commit id。)

   Note the caret (^) before the "introduced": it tells regzbot to treat the
   parent mail (the one you reply to) as the initial report for the regression
   you want to see tracked; that's important, as regzbot will later look out
   for patches with "Closes:" tags pointing to the report in the archives on
   lore.kernel.org.
   (注意 "introduced" 前面的插入符（^）：它告诉 regzbot 把父邮件（你回复的那封）当作你希望被跟踪的回归的初始报告；这一点很重要，因为 regzbot 之后会在 lore.kernel.org 的存档中寻找带有指向该报告的 "Closes:" 标签的补丁。)

 * When forwarding a regression reported to a bug tracker, include a paragraph
   with these regzbot commands::

       #regzbot introduced: 1f2e3d4c5b6a
       #regzbot from: Some N. Ice Human <some.human@example.com>
       #regzbot monitor: http://some.bugtracker.example.com/ticket?id=123456789

   (转发提交到缺陷跟踪器的回归报告时，包含一个带有这些 regzbot 命令的段落。)

   Regzbot will then automatically associate patches with the report that
   contain "Closes:" tags pointing to your mail or the mentioned ticket.
   (之后，regzbot 会自动把带有指向你的邮件或上述工单的 "Closes:" 标签的补丁与该报告关联起来。)

What's important when fixing regressions(修复回归时的要点)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You don't need to do anything special when submitting fixes for regression, just
remember to do what Documentation/process/submitting-patches.rst,
:ref:`Documentation/process/5.Posting.rst <development_posting>`, and
Documentation/process/stable-kernel-rules.rst already explain in more detail:
(提交回归修复时不需要做什么特别的事情，只要记得做到 Documentation/process/submitting-patches.rst、:ref:`Documentation/process/5.Posting.rst <development_posting>` 和 Documentation/process/stable-kernel-rules.rst 已经更详细说明过的这几点：)

 * Point to all places where the issue was reported using "Closes:" tags::

       Closes: https://lore.kernel.org/r/30th.anniversary.repost@klaava.Helsinki.FI/
       Closes: https://bugzilla.kernel.org/show_bug.cgi?id=1234567890

   (用 "Closes:" 标签指向该问题被报告的所有位置。)

   If you are only fixing part of the issue, you may use "Link:" instead as
   described in the first document mentioned above. regzbot currently treats
   both of these equivalently and considers the linked reports as resolved.
   (如果你只修复了问题的一部分，可以按上面提到的第一份文档的说明改用 "Link:"。regzbot 目前把两者等同对待，并将被链接的报告视为已解决。)

 * Add a "Fixes:" tag to specify the commit causing the regression.
   (添加 "Fixes:" 标签以指明导致回归的提交。)

 * If the culprit was merged in an earlier development cycle, explicitly mark
   the fix for backporting using the ``Cc: stable@vger.kernel.org`` tag.
   (如果罪魁祸首在较早的开发周期就已合入，用 ``Cc: stable@vger.kernel.org`` 标签明确标记该修复需要向后移植。)

All this is expected from you and important when it comes to regression, as
these tags are of great value for everyone (you included) that might be looking
into the issue weeks, months, or years later. These tags are also crucial for
tools and scripts used by other kernel developers or Linux distributions; one of
these tools is regzbot, which heavily relies on the "Closes:" tags to associate
reports for regression with changes resolving them.
(以上这些都是对你的期望，在涉及回归时非常重要，因为这些标签对所有可能在数周、数月或数年后研究该问题的人（包括你自己）都有巨大价值。这些标签对其他内核开发者或 Linux 发行版使用的工具和脚本也至关重要；regzbot 就是其中之一，它严重依赖 "Closes:" 标签来把回归报告与解决它们的变更关联起来。)

Expectations and best practices for fixing regressions(修复回归的期望与最佳实践)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As a Linux kernel developer, you are expected to give your best to prevent
situations where a regression caused by a recent change of yours leaves users
only these options:
(作为一名 Linux 内核开发者，你应当尽力避免出现这样的局面：你最近的一个变更引发的回归，让用户只剩下以下选择：)

 * Run a kernel with a regression that impacts usage.
   (继续运行带有影响使用体验的回归的内核。)

 * Switch to an older or newer kernel series.
   (切换到更旧或更新的内核系列。)

 * Continue running an outdated and thus potentially insecure kernel for more
   than three weeks after the regression's culprit was identified. Ideally it
   should be less than two. And it ought to be just a few days, if the issue is
   severe or affects many users -- either in general or in prevalent
   environments.
   (在回归的罪魁祸首被查明之后，继续运行过时因而可能不安全的内核超过三周。理想情况下应少于两周。而如果问题严重或影响许多用户——无论是普遍意义上还是在常见环境中——它应当只需要几天。)

How to realize that in practice depends on various factors. Use the following
rules of thumb as a guide.
(在实践中如何做到这一点取决于多种因素。请把以下经验法则作为指引。)

In general:
(总体而言：)

 * Prioritize work on regressions over all other Linux kernel work, unless the
   latter concerns a severe issue (e.g. acute security vulnerability, data loss,
   bricked hardware, ...).
   (把回归相关的工作排在所有其他 Linux 内核工作之前，除非后者涉及严重问题（例如紧迫的安全漏洞、数据丢失、硬件变砖等）。)

 * Expedite fixing mainline regressions that recently made it into a proper
   mainline, stable, or longterm release (either directly or via backport).
   (加快修复最近进入了正式 mainline、stable 或 longterm 版本（无论是直接进入还是通过向后移植）的 mainline 回归。)

 * Do not consider regressions from the current cycle as something that can wait
   till the end of the cycle, as the issue might discourage or prevent users and
   CI systems from testing mainline now or generally.
   (不要把当前开发周期的回归当成可以拖到周期结束的事，因为这类问题可能打击或阻碍用户和 CI 系统现在乃至一直以来对 mainline 的测试。)

 * Work with the required care to avoid additional or bigger damage, even if
   resolving an issue then might take longer than outlined below.
   (以必要的谨慎开展工作，避免额外或更大的损害，即使这样解决问题可能比下文概述的时间更长。)

On timing once the culprit of a regression is known:
(关于查明回归的罪魁祸首之后的时间安排：)

 * Aim to mainline a fix within two or three days, if the issue is severe or
   bothering many users -- either in general or in prevalent conditions like a
   particular hardware environment, distribution, or stable/longterm series.
   (如果问题严重或困扰许多用户——无论是普遍意义上还是在特定硬件环境、发行版或 stable/longterm 系列等常见条件下——力求在两三天内把修复送入 mainline。)

 * Aim to mainline a fix by Sunday after the next, if the culprit made it
   into a recent mainline, stable, or longterm release (either directly or via
   backport); if the culprit became known early during a week and is simple to
   resolve, try to mainline the fix within the same week.
   (如果罪魁祸首进入了近期的 mainline、stable 或 longterm 版本（无论是直接进入还是通过向后移植），力求在下一个周日（即隔周的周日）之前把修复送入 mainline；如果罪魁祸首在一周早些时候就已被查明且容易解决，尽量在同一个星期内送入 mainline。)

 * For other regressions, aim to mainline fixes before the hindmost Sunday
   within the next three weeks. One or two Sundays later are acceptable, if the
   regression is something people can live with easily for a while -- like a
   mild performance regression.
   (对于其他回归，力求在接下来的三周内、最迟的那个周日之前把修复送入 mainline。如果回归是人们能轻松忍受一阵子的问题——例如轻微的性能回归——晚一两个周日也可以接受。)

 * It's strongly discouraged to delay mainlining regression fixes till the next
   merge window, except when the fix is extraordinarily risky or when the
   culprit was mainlined more than a year ago.
   (强烈不建议把回归修复的 mainline 合入拖延到下一个合并窗口，除非修复风险异乎寻常地大，或者罪魁祸首早在一年多以前就已进入 mainline。)

On procedure:
(关于流程：)

 * Always consider reverting the culprit, as it's often the quickest and least
   dangerous way to fix a regression. Don't worry about mainlining a fixed
   variant later: that should be straight-forward, as most of the code went
   through review once already.
   (始终考虑 revert 罪魁祸首，因为这往往是修复回归最快、最不危险的方式。不必担心之后再主线化修复版本：那应当很直接，因为大部分代码已经经过了一轮评审。)

 * Try to resolve any regressions introduced in mainline during the past
   twelve months before the current development cycle ends: Linus wants such
   regressions to be handled like those from the current cycle, unless fixing
   bears unusual risks.
   (尽量在当前开发周期结束之前解决过去十二个月里进入 mainline 的所有回归：Linus 希望这类回归按当前周期的回归一样对待，除非修复带有异乎寻常的风险。)

 * Consider CCing Linus on discussions or patch review, if a regression seems
   tangly. Do the same in precarious or urgent cases -- especially if the
   subsystem maintainer might be unavailable. Also CC the stable team, when you
   know such a regression made it into a mainline, stable, or longterm release.
   (如果某个回归看起来纠缠不清，考虑在讨论或补丁评审时抄送 Linus。在棘手或紧急的情况下也这样做——尤其是子系统维护者可能没空的时候。如果你知道这样的回归已进入 mainline、stable 或 longterm 版本，也请抄送 stable 团队。)

 * For urgent regressions, consider asking Linus to pick up the fix straight
   from the mailing list: he is totally fine with that for uncontroversial
   fixes. Ideally though such requests should happen in accordance with the
   subsystem maintainers or come directly from them.
   (对于紧急回归，可以考虑请 Linus 直接从邮件列表拾取修复：对于没有争议的修复，他完全乐于接受。不过理想情况下，这类请求应与子系统维护者协调进行，或直接由他们提出。)

 * In case you are unsure if a fix is worth the risk applying just days before
   a new mainline release, send Linus a mail with the usual lists and people in
   CC; in it, summarize the situation while asking him to consider picking up
   the fix straight from the list. He then himself can make the call and when
   needed even postpone the release. Such requests again should ideally happen
   in accordance with the subsystem maintainers or come directly from them.
   (如果你不确定一个修复是否值得在新 mainline 版本发布前几天冒险合入，就给 Linus 发一封邮件并抄送惯常的列表和相关人员；在邮件中概述情况，并请他考虑直接从列表拾取该修复。这样他可以自己拿主意，必要时甚至推迟发布。这类请求同样最好与子系统维护者协调进行，或直接由他们提出。)

Regarding stable and longterm kernels:
(关于 stable 和 longterm 内核：)

 * You are free to leave regressions to the stable team, if they at no point in
   time occurred with mainline or were fixed there already.
   (如果回归从未在 mainline 上出现过，或者已经在那里被修复，你可以放心地把它们留给 stable 团队。)

 * If a regression made it into a proper mainline release during the past
   twelve months, ensure to tag the fix with "Cc: stable@vger.kernel.org", as a
   "Fixes:" tag alone does not guarantee a backport. Please add the same tag,
   in case you know the culprit was backported to stable or longterm kernels.
   (如果回归在过去十二个月内进入了正式的 mainline 版本，务必给修复打上 "Cc: stable@vger.kernel.org" 标签，因为仅有 "Fixes:" 标签并不能保证向后移植。如果你知道罪魁祸首已被向后移植到 stable 或 longterm 内核，也请加上同样的标签。)

 * When receiving reports about regressions in recent stable or longterm kernel
   series, please evaluate at least briefly if the issue might happen in current
   mainline as well -- and if that seems likely, take hold of the report. If in
   doubt, ask the reporter to check mainline.
   (收到关于近期 stable 或 longterm 内核系列回归的报告时，请至少简略评估一下该问题是否也可能出现在当前 mainline 中——如果看起来可能，就把报告接手过来。拿不准的话，请报告者去检查 mainline。)

 * Whenever you want to swiftly resolve a regression that recently also made it
   into a proper mainline, stable, or longterm release, fix it quickly in
   mainline; when appropriate thus involve Linus to fast-track the fix (see
   above). That's because the stable team normally does neither revert nor fix
   any changes that cause the same problems in mainline.
   (每当你想迅速解决一个最近也进入了正式 mainline、stable 或 longterm 版本的回归时，请在 mainline 中快速修复它；适当时让 Linus 介入以加快修复（见上文）。这是因为 stable 团队通常既不 revert 也不修复那些在 mainline 中造成相同问题的变更。)

 * In case of urgent regression fixes you might want to ensure prompt
   backporting by dropping the stable team a note once the fix was mainlined;
   this is especially advisable during merge windows and shortly thereafter, as
   the fix otherwise might land at the end of a huge patch queue.
   (对于紧急的回归修复，你可能想在修复进入 mainline 后给 stable 团队捎个信，以确保及时的向后移植；在合并窗口期间及其后不久尤其建议这样做，否则修复可能会排到一个庞大补丁队列的末尾。)

On patch flow:
(关于补丁流转：)

 * Developers, when trying to reach the time periods mentioned above, remember
   to account for the time it takes to get fixes tested, reviewed, and merged by
   Linus, ideally with them being in linux-next at least briefly. Hence, if a
   fix is urgent, make it obvious to ensure others handle it appropriately.
   (开发者们，在争取达到上述时间要求时，记得把修复被测试、评审并由 Linus 合入所需的时间计算在内，理想情况下它们至少要在 linux-next 中短暂停留。因此，如果修复很紧急，请把它标注清楚，确保其他人妥善处理。)

 * Reviewers, you are kindly asked to assist developers in reaching the time
   periods mentioned above by reviewing regression fixes in a timely manner.
   (评审者们，恳请你们及时评审回归修复，协助开发者达到上述时间要求。)

 * Subsystem maintainers, you likewise are encouraged to expedite the handling
   of regression fixes. Thus evaluate if skipping linux-next is an option for
   the particular fix. Also consider sending git pull requests more often than
   usual when needed. And try to avoid holding onto regression fixes over
   weekends -- especially when the fix is marked for backporting.
   (子系统维护者们，同样希望你们加快回归修复的处理。因此请评估对该特定修复而言跳过 linux-next 是否可行。必要时也考虑比平时更频繁地发送 git pull 请求。并尽量避免把回归修复压到周末之后——尤其是当修复已标记要向后移植时。)


More aspects regarding regressions developers should be aware of(开发者应当了解的关于回归的更多方面)
----------------------------------------------------------------------------------------------------


How to deal with changes where a risk of regression is known(如何处理已知存在回归风险的变更)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Evaluate how big the risk of regressions is, for example by performing a code
search in Linux distributions and Git forges. Also consider asking other
developers or projects likely to be affected to evaluate or even test the
proposed change; if problems surface, maybe some solution acceptable for all
can be found.
(评估回归风险有多大，例如在 Linux 发行版和 Git 托管平台中做代码搜索。也可以考虑请其他可能受影响的开发者或项目来评估甚至测试提议的变更；如果问题浮现，也许能找到各方都能接受的解决方案。)

If the risk of regressions in the end seems to be relatively small, go ahead
with the change, but let all involved parties know about the risk. Hence, make
sure your patch description makes this aspect obvious. Once the change is
merged, tell the Linux kernel's regression tracker and the regressions mailing
list about the risk, so everyone has the change on the radar in case reports
trickle in. Depending on the risk, you also might want to ask the subsystem
maintainer to mention the issue in his mainline pull request.
(如果回归风险最终看起来相对较小，就继续进行变更，但要让所有相关方知道这一风险。因此，请确保你的补丁描述把这一点写明白。变更合入之后，把该风险告知 Linux 内核的回归跟踪员和回归邮件列表，让每个人都把这项变更记在心上，以防报告陆续出现。视风险大小而定，你或许还想请子系统维护者在他的 mainline pull 请求中提及该问题。)

What else is there to known about regressions?(关于回归还有哪些需要了解的？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Check out Documentation/admin-guide/reporting-regressions.rst, it covers a lot
of other aspects you want might want to be aware of:
(请查阅 Documentation/admin-guide/reporting-regressions.rst，它涵盖了许多你可能想了解的其他方面：)

 * the purpose of the "no regressions" rule
   ("禁止回归"规则的目的)

 * what issues actually qualify as regression
   (哪些问题才算得上回归)

 * who's in charge for finding the root cause of a regression
   (谁来负责找出回归的根因)

 * how to handle tricky situations, e.g. when a regression is caused by a
   security fix or when fixing a regression might cause another one
   (如何处理棘手的情况，例如回归由安全修复引起，或者修复回归可能引发另一个回归时)

Whom to ask for advice when it comes to regressions(涉及回归时该向谁寻求建议)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Send a mail to the regressions mailing list (regressions@lists.linux.dev) while
CCing the Linux kernel's regression tracker (regressions@leemhuis.info); if the
issue might better be dealt with in private, feel free to omit the list.
(给回归邮件列表（regressions@lists.linux.dev）发邮件，同时抄送 Linux 内核回归跟踪员（regressions@leemhuis.info）；如果问题更适合私下处理，可以不抄送列表。)


More about regression tracking and regzbot(关于回归跟踪与 regzbot 的更多信息)
-----------------------------------------------------------------------------


Why the Linux kernel has a regression tracker, and why is regzbot used?(为什么 Linux 内核要有回归跟踪器，为什么要使用 regzbot？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Rules like "no regressions" need someone to ensure they are followed, otherwise
they are broken either accidentally or on purpose. History has shown this to be
true for the Linux kernel as well. That's why Thorsten Leemhuis volunteered to
keep an eye on things as the Linux kernel's regression tracker, who's
occasionally helped by other people. Neither of them are paid to do this,
that's why regression tracking is done on a best effort basis.
("禁止回归"这样的规则需要有人来确保它被遵守，否则它就会或被无意、或被有意地打破。历史已经证明，Linux 内核也是如此。这就是 Thorsten Leemhuis 自愿担任 Linux 内核回归跟踪员的原因，偶尔也有其他人帮忙。他们做这件事都没有报酬，因此回归跟踪是尽力而为的工作。)

Earlier attempts to manually track regressions have shown it's an exhausting and
frustrating work, which is why they were abandoned after a while. To prevent
this from happening again, Thorsten developed regzbot to facilitate the work,
with the long term goal to automate regression tracking as much as possible for
everyone involved.
(更早的人工回归跟踪尝试表明，这是一项令人精疲力竭又令人沮丧的工作，因此过了一段时间就被放弃了。为了防止这种情况重演，Thorsten 开发了 regzbot 来简化这项工作，长期目标是尽可能为所有相关人员把回归跟踪自动化。)

How does regression tracking work with regzbot?(regzbot 是如何进行回归跟踪的？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The bot watches for replies to reports of tracked regressions. Additionally,
it's looking out for posted or committed patches referencing such reports
with "Closes:" tags; replies to such patch postings are tracked as well.
Combined this data provides good insights into the current state of the fixing
process.
(这个机器人关注被跟踪回归的报告所收到的回复。此外，它还留意引用了这类报告的、已投递或已提交的带有 "Closes:" 标签的补丁；对这类补丁投递的回复也会被跟踪。综合起来，这些数据能很好地反映修复过程的当前状态。)

Regzbot tries to do its job with as little overhead as possible for both
reporters and developers. In fact, only reporters are burdened with an extra
duty: they need to tell regzbot about the regression report using the ``#regzbot
introduced`` command outlined above; if they don't do that, someone else can
take care of that using ``#regzbot ^introduced``.
(regzbot 尽量以对报告者和开发者双方都尽可能小的开销来完成工作。实际上，只有报告者承担了一项额外义务：他们需要用上文概述的 ``#regzbot introduced`` 命令把回归报告告诉 regzbot；如果他们不这样做，其他人也可以用 ``#regzbot ^introduced`` 来代劳。)

For developers there normally is no extra work involved, they just need to make
sure to do something that was expected long before regzbot came to light: add
links to the patch description pointing to all reports about the issue fixed.
(对开发者而言通常没有额外工作，他们只需确保做到一件 regzbot 问世之前早就是期望之事：在补丁描述中添加链接，指向与所修复问题相关的所有报告。)

Do I have to use regzbot?(我必须使用 regzbot 吗？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

It's in the interest of everyone if you do, as kernel maintainers like Linus
Torvalds partly rely on regzbot's tracking in their work -- for example when
deciding to release a new version or extend the development phase. For this they
need to be aware of all unfixed regression; to do that, Linus is known to look
into the weekly reports sent by regzbot.
(使用它符合所有人的利益，因为像 Linus Torvalds 这样的内核维护者在工作中部分依赖 regzbot 的跟踪——例如在决定发布新版本或延长开发阶段时。为此他们需要了解所有未修复的回归；为此，众所周知 Linus 会查看 regzbot 发送的每周报告。)

Do I have to tell regzbot about every regression I stumble upon?(我必须把遇到的每个回归都告诉 regzbot 吗？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Ideally yes: we are all humans and easily forget problems when something more
important unexpectedly comes up -- for example a bigger problem in the Linux
kernel or something in real life that's keeping us away from keyboards for a
while. Hence, it's best to tell regzbot about every regression, except when you
immediately write a fix and commit it to a tree regularly merged to the affected
kernel series.
(理想情况下是的：我们都是普通人，当更重要的事情意外出现时——例如 Linux 内核里冒出一个更大的问题，或者现实生活中有什么事让我们暂时离开键盘——很容易忘记问题。因此，最好把每个回归都告诉 regzbot，除非你立即写好修复并提交到一个定期合入受影响内核系列的树中。)

How to see which regressions regzbot tracks currently?(如何查看 regzbot 当前跟踪着哪些回归？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Check `regzbot's web-interface <https://linux-regtracking.leemhuis.info/regzbot/>`_
for the latest info; alternatively, `search for the latest regression report
<https://lore.kernel.org/lkml/?q=%22Linux+regressions+report%22+f%3Aregzbot>`_,
which regzbot normally sends out once a week on Sunday evening (UTC), which is a
few hours before Linus usually publishes new (pre-)releases.
(查看 regzbot 的 web 界面获取最新信息；或者搜索最新的回归报告，regzbot 通常每周日晚上（UTC）发出一次，比 Linus 通常发布新（预）版本早几个小时。)

What places is regzbot monitoring?(regzbot 监控哪些地方？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Regzbot is watching the most important Linux mailing lists as well as the git
repositories of linux-next, mainline, and stable/longterm.
(regzbot 监控最重要的 Linux 邮件列表，以及 linux-next、mainline 和 stable/longterm 的 git 仓库。)

What kind of issues are supposed to be tracked by regzbot?(regzbot 应该跟踪哪类问题？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The bot is meant to track regressions, hence please don't involve regzbot for
regular issues. But it's okay for the Linux kernel's regression tracker if you
use regzbot to track severe issues, like reports about hangs, corrupted data,
or internal errors (Panic, Oops, BUG(), warning, ...).
(这个机器人是用来跟踪回归的，请不要为普通问题动用 regzbot。但如果你用 regzbot 跟踪严重问题——例如关于挂起、数据损坏或内部错误（Panic、Oops、BUG()、警告等）的报告——Linux 内核回归跟踪员是可以接受的。)

Can I add regressions found by CI systems to regzbot's tracking?(我可以把 CI 系统发现的回归加入 regzbot 的跟踪吗？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Feel free to do so, if the particular regression likely has impact on practical
use cases and thus might be noticed by users; hence, please don't involve
regzbot for theoretical regressions unlikely to show themselves in real world
usage.
(如果该回归很可能影响实际用例、因而可能被用户注意到，尽管这样做；因此，对于那些在真实使用中不太可能暴露的理论回归，请不要动用 regzbot。)

How to interact with regzbot?(如何与 regzbot 交互？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By using a 'regzbot command' in a direct or indirect reply to the mail with the
regression report. These commands need to be in their own paragraph (IOW: they
need to be separated from the rest of the mail using blank lines).
(在直接或间接回复回归报告邮件时使用 "regzbot 命令"。这些命令必须自成一段（换句话说：必须用空行把命令与邮件的其余部分隔开）。)

One such command is ``#regzbot introduced: <version or commit>``, which makes
regzbot consider your mail as a regressions report added to the tracking, as
already described above; ``#regzbot ^introduced: <version or commit>`` is another
such command, which makes regzbot consider the parent mail as a report for a
regression which it starts to track.
(其中一个命令是 ``#regzbot introduced: <version or commit>``，它让 regzbot 把你的邮件视为加入跟踪的一份回归报告，如上文所述；``#regzbot ^introduced: <version or commit>`` 是另一个这样的命令，它让 regzbot 把父邮件视为它开始跟踪的一个回归的报告。)

Once one of those two commands has been utilized, other regzbot commands can be
used in direct or indirect replies to the report. You can write them below one
of the `introduced` commands or in replies to the mail that used one of them
or itself is a reply to that mail:
(一旦使用了这两个命令之一，其他 regzbot 命令就可以在对该报告的直接或间接回复中使用。你可以把它们写在某个 `introduced` 命令下方，或者写在使用了它们的邮件的回复中（该回复本身也可以是对那封邮件的回复）：)

 * Set or update the title::

       #regzbot title: foo

   (设置或更新标题。)

 * Monitor a discussion or bugzilla.kernel.org ticket where additions aspects of
   the issue or a fix are discussed -- for example the posting of a patch fixing
   the regression::

       #regzbot monitor: https://lore.kernel.org/all/30th.anniversary.repost@klaava.Helsinki.FI/

   (监控讨论该问题或修复的其他方面的讨论串或 bugzilla.kernel.org 工单——例如修复该回归的补丁的投递。)

   Monitoring only works for lore.kernel.org and bugzilla.kernel.org; regzbot
   will consider all messages in that thread or ticket as related to the fixing
   process.
   (监控只对 lore.kernel.org 和 bugzilla.kernel.org 有效；regzbot 会把该线程或工单中的所有消息视为与修复过程相关。)

 * Point to a place with further details of interest, like a mailing list post
   or a ticket in a bug tracker that are slightly related, but about a different
   topic::

       #regzbot link: https://bugzilla.kernel.org/show_bug.cgi?id=123456789

   (指向包含更多相关细节的地方，例如轻微相关但主题不同的邮件列表帖子或缺陷跟踪器工单。)

 * Mark a regression as fixed by a commit that is heading upstream or already
   landed::

       #regzbot fix: 1f2e3d4c5d

   (把某个回归标记为已被一个正在走向上游或已经落地的提交修复。)

 * Mark a regression as a duplicate of another one already tracked by regzbot::

       #regzbot dup-of: https://lore.kernel.org/all/30th.anniversary.repost@klaava.Helsinki.FI/

   (把某个回归标记为 regzbot 已跟踪的另一个回归的重复项。)

 * Mark a regression as invalid::

       #regzbot invalid: wasn't a regression, problem has always existed

   (把某个回归标记为无效。)

Is there more to tell about regzbot and its commands?(关于 regzbot 及其命令还有更多要说的吗？)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

More detailed and up-to-date information about the Linux
kernel's regression tracking bot can be found on its
`project page <https://gitlab.com/knurd42/regzbot>`_, which among others
contains a `getting started guide <https://gitlab.com/knurd42/regzbot/-/blob/main/docs/getting_started.md>`_
and `reference documentation <https://gitlab.com/knurd42/regzbot/-/blob/main/docs/reference.md>`_
which both cover more details than the above section.
(关于 Linux 内核回归跟踪机器人的更详细和最新的信息，可以在它的项目页面上找到，其中包含入门指南和参考文档，两者都比上一节覆盖更多细节。)

Quotes from Linus about regression(Linus 关于回归的语录)
--------------------------------------------------------

The following statements from Linus Torvalds provide some insight into Linux
"no regressions" rule and how he expects regressions to be handled:
(下面这些来自 Linus Torvalds 的言论，可以帮助理解 Linux 的"禁止回归"规则，以及他期望回归如何被处理：)

On how quickly regressions should be fixed(关于回归应该多快被修复)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2026-01-22 <https://lore.kernel.org/all/CAHk-=wheQNiW_WtHGO7bKkT7Uib-p+ai2JP9M+z+FYcZ6CAxYA@mail.gmail.com/>`_::

    But a user complaining should basically result in an immediate fix -
    possibly a "revert and rethink".

  (但用户抱怨基本上就应该立刻带来修复——可能是"先 revert、再重新思考"。)

  With a later clarification on `2026-01-28 <https://lore.kernel.org/all/CAHk-%3Dwi86AosXs66-yi54%2BmpQjPu0upxB8ZAfG%2BLsMyJmcuMSA@mail.gmail.com/>`_::

    It's also worth noting that "immediate" obviously doesn't mean "right
    this *second* when the problem has been reported".

    But if it's a regression with a known commit that caused it, I think
    the rule of thumb should generally be "within a week", preferably
    before the next rc.

  (他稍后在 2026-01-28 的澄清中还指出：值得一提的是，"立刻"显然并不意味着"问题被报告的这一秒"。但如果是一个已知致因提交的回归，我认为经验法则通常是"一周之内"，最好在下一个 rc 之前。)

* From `2023-04-21 <https://lore.kernel.org/all/CAHk-=wgD98pmSK3ZyHk_d9kZ2bhgN6DuNZMAJaV0WTtbkf=RDw@mail.gmail.com/>`_::

    Known-broken commits either
     (a) get a timely fix that doesn't have other questions
    or
     (b) get reverted

  (已知有问题的提交，要么 (a) 及时得到一个没有其他疑义的修复，要么 (b) 被 revert。)

* From `2021-09-20(2) <https://lore.kernel.org/all/CAHk-=wgOvmtRw1TNbMC1rn5YqyTKyn0hz+sc4k0DGNn++u9aYw@mail.gmail.com/>`_::

    [...] review shouldn't hold up reported regressions of existing code. That's
    just basic _testing_ - either the fix should be applied, or - if the fix is
    too invasive or too ugly - the problematic source of the regression should
    be reverted.

    Review should be about new code, it shouldn't be holding up "there's a
    bug report, here's the obvious fix".

  ([……]评审不应该拖延已报告的现有代码回归。那只是基本的*测试*——要么应用修复，要么（如果修复太具侵入性或太难看）把导致回归的问题源头 revert 掉。评审应该针对新代码，它不应该拖住"有一个缺陷报告，这是显而易见的修复"这种事。)

* From `2023-05-08 <https://lore.kernel.org/all/CAHk-=wgzU8_dGn0Yg+DyX7ammTkDUCyEJ4C=NvnHRhxKWC7Wpw@mail.gmail.com/>`_::

    If something doesn't even build, it should damn well be fixed ASAP.

  (如果连编译都过不了，那就该死地尽快修好。)

On how fixing regressions with reverts can help prevent maintainer burnout(关于用 revert 修复回归如何有助于防止维护者倦怠)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2026-01-28 <https://lore.kernel.org/all/CAHk-%3Dwi86AosXs66-yi54%2BmpQjPu0upxB8ZAfG%2BLsMyJmcuMSA@mail.gmail.com/>`_::

    > So how can I/we make "immediate fixes" happen more often without
    > contributing to maintainer burnout?

    [...] the "revert and rethink" model [...] often a good idea in general [...]

    Exactly so that maintainers don't get stressed out over having a pending
    problem report that people keep pestering them about.

    I think people are sometimes a bit too bought into whatever changes
    they made, and reverting is seen as "too drastic", but I think it's
    often the quick and easy solution for when there isn't some obvious
    response to a regression report.

  (> 那么我/我们怎样才能让"立刻修复"更常发生，又不加剧维护者的倦怠？
  [……]"revert 并重新思考"的模式[……]总体上往往是个好主意[……]
  正是为了让维护者不至于因为一份悬而未决、被人不断催促的问题报告而焦头烂额。
  我觉得人们有时对自己做的变更投入过多，把 revert 看成"太激烈"的做法，但我认为，在针对回归报告没有显而易见的回应办法时，它往往是快捷而省力的解决方案。)

On mainlining fixes when the last -rc or a new release is close(当最后一个 -rc 或新版本发布临近时如何主线化修复)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2026-02-01 <https://lore.kernel.org/all/CAHk-%3DwhXTw1oPsa%2BTLuY1Rc9D1OAiPVOdR_-R2xG45kwDObKdA@mail.gmail.com/>`_::

    So I think I'd rather see them hit rc8 (later today) and have a week
    of testing in my tree and be reverted if they cause problems, than
    have them go in after rc8 and then cause problems in the 6.19 release
    instead.

  (所以我认为，我宁愿看到它们赶上 rc8（今天晚些时候），在我的树里接受一周的测试、如果出问题就 revert 掉，也不愿看到它们在 rc8 之后才进去、然后给 6.19 正式版惹麻烦。)

* From `2023-04-20 <https://lore.kernel.org/all/CAHk-=wis_qQy4oDNynNKi5b7Qhosmxtoj1jxo5wmB6SRUwQUBQ@mail.gmail.com/>`_::

    But something like this, where the regression was in the previous release
    and it's just a clear fix with no semantic subtlety, I consider to be just a
    regular regression that should be expedited - partly to make it into stable,
    and partly to avoid having to put the fix into _another_ stable kernel.

  (但像这样回归出在上一个版本、而修复清晰且没有任何语义上的微妙之处的情况，我认为它只是一个应当加快处理的普通回归——一部分原因是要让它进入 stable，另一部分原因是要避免不得不把修复塞进*另一个* stable 内核。)

On sending merge requests with just one fix(关于发送只含一个修复的合并请求)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2024-04-24 <https://lore.kernel.org/all/CAHk-=wjy_ph9URuFt-pq+2AJ__p7gFDx=yzVSCsx16xAYvNw9g@mail.gmail.com/>`_::

    If the issue is just that there's nothing else happening, I think people
    should just point me to the patch and say "can you apply this single fix?"

  (如果问题只是没有别的事情在进行，我认为大家应该直接把补丁指给我，然后说"你能应用这一个修复吗？")

* From `2023-04-20 <https://lore.kernel.org/all/CAHk-=wis_qQy4oDNynNKi5b7Qhosmxtoj1jxo5wmB6SRUwQUBQ@mail.gmail.com/>`_::

    I'm always open to direct fixes when there is no controversy about the fix.
    No problem. I still happily deal with individual patches.

  (只要修复本身没有争议，我始终乐于接受直接提交的修复。没问题。我仍然乐意处理单独的补丁。)

On the importance of pointing to bug reports using Link:/Closes: tags(关于用 Link:/Closes: 标签指向缺陷报告的重要性)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2025-07-29(1) <https://lore.kernel.org/all/CAHk-=wj2kJRPWx8B09AAtzj+_g+T6UBX11TP0ebs1WJdTtv=WQ@mail.gmail.com/>`_::

    [...] revert like this, it really would be good to link to the problems, so
    that when people try to re-enable it, they have the history for why it
    didn't work the first time.

  ([……]像这样 revert 时，最好把指向问题的链接带上，这样以后有人想重新启用它时，就知道当初为什么行不通。)

* From `2022-05-08 <https://lore.kernel.org/all/CAHk-=wjMmSZzMJ3Xnskdg4+GGz=5p5p+GSYyFBTh0f-DgvdBWg@mail.gmail.com/>`_::

    So I have to once more complain [...]

    [...] There's no link to the actual problem the patch fixes.

  (所以我不得不再抱怨一次[……]
  [……]没有任何链接指向补丁所修复的实际问题。)

* From `2022-06-22 <https://lore.kernel.org/all/CAHk-=wjxzafG-=J8oT30s7upn4RhBs6TX-uVFZ5rME+L5_DoJA@mail.gmail.com/>`_::

    See, *that* link [to the report] would have been useful in the commit.

  (你看，*那个*指向报告的链接要是放进提交里就派上用场了。)

On why the "no regressions" rule exists(关于"禁止回归"规则为何存在)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2026-01-22 <https://lore.kernel.org/all/CAHk-=wheQNiW_WtHGO7bKkT7Uib-p+ai2JP9M+z+FYcZ6CAxYA@mail.gmail.com/>`_::

    But the basic rule is: be so good about backwards compatibility that
    users never have to worry about upgrading. They should absolutely feel
    confident that any kernel-reported problem will either be solved, or
    have an easy solution that is appropriate for *them* (ie a
    non-technical user shouldn't be expected to be able to do a lot).

    Because the last thing we want is people holding back from trying new
    kernels.

  (但基本规则是：把向后兼容做得足够好，让用户升级时无需担心。他们应当绝对有信心：任何内核报告的问题要么会被解决，要么有一个适合*他们*的简单解决之道（也就是说，不应指望非技术用户会做很多事）。因为我们最不希望看到的就是人们不敢尝试新内核。)

* From `2024-05-28 <https://lore.kernel.org/all/CAHk-=wgtb7y-bEh7tPDvDWru7ZKQ8-KMjZ53Tsk37zsPPdwXbA@mail.gmail.com/>`_::

    I introduced that "no regressions" rule something like two decades
    ago, because people need to be able to update their kernel without
    fear of something they relied on suddenly stopping to work.

  (我是在大约二十年前引入那条"禁止回归"规则的，因为人们需要能够更新自己的内核，而不必担心他们赖以使用的功能突然失效。)

* From `2018-08-03 <https://lore.kernel.org/all/CA+55aFwWZX=CXmWDTkDGb36kf12XmTehmQjbiMPCqCRG2hi9kw@mail.gmail.com/>`_::

    The whole point of "we do not regress" is so that people can upgrade
    the kernel and never have to worry about it.

    [...]

    Because the only thing that matters IS THE USER.

  ("我们不回归"的全部意义就在于：人们可以升级内核而永远不必为此担心。
  [……]
  因为唯一重要的是用户。)

* From `2017-10-26(1) <https://lore.kernel.org/lkml/CA+55aFxW7NMAMvYhkvz1UPbUTUJewRt6Yb51QAx5RtrWOwjebg@mail.gmail.com/>`_::

    If the kernel used to work for you, the rule is that it continues to work
    for you.

    [...]

    People should basically always feel like they can update their kernel
    and simply not have to worry about it.

    I refuse to introduce "you can only update the kernel if you also
    update that other program" kind of limitations. If the kernel used to
    work for you, the rule is that it continues to work for you.

  (如果内核以前对你可用，规则就是它继续对你可用。
  [……]
  人们基本上应当始终觉得可以更新内核而不必操心任何事。
  我拒绝引入"只有同时更新那个别的程序你才能更新内核"之类的限制。如果内核以前对你可用，规则就是它继续对你可用。)

On exceptions to the "no regressions" rule(关于"禁止回归"规则的例外)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2026-01-22 <https://lore.kernel.org/all/CAHk-=wheQNiW_WtHGO7bKkT7Uib-p+ai2JP9M+z+FYcZ6CAxYA@mail.gmail.com/>`_::

    There are _very_ few exceptions to that rule, the main one being "the
    problem was a fundamental huge and gaping security issue and we *had* to
    make that change, and we couldn't even make your limited use-case just
    continue to work".

    The other exception is "the problem was reported years after it was
    introduced, and now most people rely on the new behavior".

    [...]

    Now, if it's one or two users and you can just get them to recompile,
    that's one thing. Niche hardware and odd use-cases can sometimes be
    solved that way, and regressions can sometimes be fixed by handholding
    every single reporter if the reporter is willing and able to change
    his or her workflow.

  (那条规则几乎没有例外，主要的例外是"这个问题是一个根本性的、巨大的、敞开的安全漏洞，我们*不得不*做出那个变更，而且我们甚至无法让你的有限用例继续工作"。
  另一个例外是"问题在被引入多年之后才被报告，而现在大多数人已经依赖新的行为"。
  [……]
  当然，如果只有一两个用户，而且能让他们重新编译就行，那是另一回事。小众硬件和奇怪用例有时可以那样解决；如果报告者愿意且有能力改变自己的工作流，回归有时也可以靠对每一个报告者手把手地引导来修复。)

* From `2023-04-20 <https://lore.kernel.org/all/CAHk-=wis_qQy4oDNynNKi5b7Qhosmxtoj1jxo5wmB6SRUwQUBQ@mail.gmail.com/>`_::

    And yes, I do consider "regression in an earlier release" to be a
    regression that needs fixing.

    There's obviously a time limit: if that "regression in an earlier
    release" was a year or more ago, and just took forever for people to
    notice, and it had semantic changes that now mean that fixing the
    regression could cause a _new_ regression, then that can cause me to
    go "Oh, now the new semantics are what we have to live with".

  (是的，我确实认为"更早版本中的回归"也是需要修复的回归。
  当然有一个时限：如果那个"更早版本中的回归"是一年多以前的事、人们花了很久才注意到，而且它带来的语义变更意味着现在修复它反而可能造成一个*新*回归，那我可能会说"好吧，现在的语义就是我们不得不接受的了"。)

* From `2017-10-26(2) <https://lore.kernel.org/lkml/CA+55aFxW7NMAMvYhkvz1UPbUTUJewRt6Yb51QAx5RtrWOwjebg@mail.gmail.com/>`_::

    There have been exceptions, but they are few and far between, and they
    generally have some major and fundamental reasons for having happened,
    that were basically entirely unavoidable, and people _tried_hard_ to
    avoid them. Maybe we can't practically support the hardware any more
    after it is decades old and nobody uses it with modern kernels any
    more. Maybe there's a serious security issue with how we did things,
    and people actually depended on that fundamentally broken model. Maybe
    there was some fundamental other breakage that just _had_ to have a
    flag day for very core and fundamental reasons.

  (例外确实有过，但寥寥无几，而且它们的发生通常都有一些重大而根本性的原因，基本上完全无法避免，人们也曾*努力*设法避免。也许是硬件用了几十年、已经没人再在现代内核里使用它，实践中无法再支持了；也许是我们做事方式存在严重的安全问题，而人们恰恰依赖那个在根本上就有缺陷的模型；也许是有某种根本性的其他破坏，出于非常核心、非常根本的原因，*不得不*搞一个断裂日（flag day）。)

On situations where updating something in userspace can resolve regressions(关于更新用户空间即可解决回归的情形)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2018-08-03 <https://lore.kernel.org/all/CA+55aFwWZX=CXmWDTkDGb36kf12XmTehmQjbiMPCqCRG2hi9kw@mail.gmail.com/>`_::

    And dammit, we upgrade the kernel ALL THE TIME without upgrading any
    other programs at all. It is absolutely required, because flag-days
    and dependencies are horribly bad.

    And it is also required simply because I as a kernel developer do not
    upgrade random other tools that I don't even care about as I develop the
    kernel, and I want any of my users to feel safe doing the same time.

  (还有，该死，我们一直在升级内核，而完全不必升级任何其他程序。这是绝对必需的，因为断裂日（flag-day）和依赖关系糟糕透顶。
  这也是必需的，原因很简单：我作为内核开发者，在开发内核的过程中并不会升级那些我根本不在乎的其他工具，我也希望我的任何用户做同样的事情时能感到安心。)

* From `2017-10-26(3) <https://lore.kernel.org/lkml/CA+55aFxW7NMAMvYhkvz1UPbUTUJewRt6Yb51QAx5RtrWOwjebg@mail.gmail.com/>`_::

    But if something actually breaks, then the change must get fixed or
    reverted. And it gets fixed in the *kernel*. Not by saying "well, fix your
    user space then". It was a kernel change that exposed the problem, it needs
    to be the kernel that corrects for it, because we have a "upgrade in place"
    model. We don't have a "upgrade with new user space".

    And I seriously will refuse to take code from people who do not understand
    and honor this very simple rule.

    This rule is also not going to change.

    And yes, I realize that the kernel is "special" in this respect. I'm proud
    of it.

  (但如果真的有东西坏了，那个变更就必须被修复或 revert。而且要在*内核*里修复。不能说"那你升级你的用户空间好了"。是内核变更暴露了这个问题，就必须由内核来纠正它，因为我们采用的是"就地升级"模型，而不是"配合新用户空间升级"模型。
  我真的会拒绝接受来自不理解、不尊重这条简单规则的人的代码。
  这条规则也不会改变。
  是的，我知道内核在这方面"特殊"。我为此自豪。)

* From `2017-10-26(4) <https://lore.kernel.org/all/CA+55aFwiiQYJ+YoLKCXjN_beDVfu38mg=Ggg5LFOcqHE8Qi7Zw@mail.gmail.com/>`_::

    If you break existing user space setups THAT IS A REGRESSION.

    It's not ok to say "but we'll fix the user space setup".

    Really. NOT OK.

  (如果你破坏了现有的用户空间配置，那就是回归。
  说"我们会去修用户空间配置"是不行的。
  真的，不行。)

On what qualifies as userspace interface, ABI, API, documented interfaces, etc.(关于什么算作用户空间接口、ABI、API、已文档化接口等)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2026-01-20 <https://lore.kernel.org/all/CAHk-=wga8Qu0-OSE9VZbviq9GuqwhPhLUXeAt-S7_9+fMCLkKg@mail.gmail.com/>`_::

    So I absolutely detest the whole notion of "ABI changes". It's a
    meaningless concept, and I hate it with a passion, [...]

    The Linux rule for regressions is basically based on the philosophical
    question of "If a tree falls in the forest, and nobody is around to
    hear it, does it make a sound?".

    So the only thing that matters is if something breaks user-*conscious*
    behavior.

    And when that happens, the distinction between "bug fix" and "new
    feature" and "ABI change" matters not one whit, and the change needs
    to be done differently.

    [...]

    I just wanted to point out that the argument about whether it's an ABI
    change or not is irrelevant. If it turns out that some program - not a test
    script, but something with relevance to conscious user expectations ~
    depended on the old broken behavior, then it needs to be done some other
    way.

  (所以我极其憎恶"ABI 变更"这整个概念。它是个毫无意义的概念，我对它深恶痛绝，[……]
  Linux 的回归规则基本上建立在这个哲学问题之上："如果一棵树在森林里倒下，周围没有人听见，它发出声音了吗？"
  所以唯一重要的是是否有东西破坏了用户*可感知*的行为。
  一旦发生这种事，"缺陷修复"、"新功能"和"ABI 变更"之间的区分就毫不重要了，这个变更必须换一种方式来做。
  [……]
  我只是想指出，争论它算不算 ABI 变更是无关紧要的。如果最终发现某个程序——不是测试脚本，而是与用户明确预期相关的东西——依赖了旧的、坏掉的行为，那就必须换一种方式来做。)

* From `2026-02-13 <https://lore.kernel.org/all/CAHk-=whY-N8kjm8kiFUV5Ei-8AuYw--EPGD-AR3Pd+5GTx2sAQ@mail.gmail.com/>`_::

    > [...] this should not fall under the don't break user space rule [...]

    Note that the rule is about breaking *users*, not breaking user space per
    se. [...]

    If some user setup breaks, things need fixing.

    [...] but I want to make it very clear that there are no excuses about "user
    space applications".

  (> [……]这不应该落入"不得破坏用户空间"规则的范畴[……]
  注意，这条规则针对的是破坏*用户*，而不是破坏用户空间本身。[……]
  如果某个用户环境坏了，那就得修。
  [……]但我要非常明确地说：拿"用户空间应用程序"当借口是不存在的。)

* From `2021-09-20(4) <https://lore.kernel.org/all/CAHk-=wi7DB2SJ-wngVvsJ7Ak2cM556Q8437sOXo4EJt2BWPdEg@mail.gmail.com/>`_::

    [...] a regression is a bit like Schrödinger's cat - if nobody is around
    to notice it and it doesn't actually affect any real workload, then you
    can treat the regression as if it doesn't exist.

  ([……]回归有点像薛定谔的猫——如果没有人注意到它，而它实际上也不影响任何真实负载，那你可以把它当作不存在。)

* From `2020-05-21 <https://lore.kernel.org/all/CAHk-=wiVi7mSrsMP=fLXQrXK_UimybW=ziLOwSzFTtoXUacWVQ@mail.gmail.com/>`_::

    The rules about regressions have never been about any kind of documented
    behavior, or where the code lives.

    The rules about regressions are always about "breaks user workflow".

    Users are literally the _only_ thing that matters.

  (关于回归的规则，从来都与任何形式的已文档化行为无关，也与代码住在哪里无关。
  关于回归的规则始终围绕"破坏用户工作流"。
  用户字面意义上就是*唯一*重要的东西。)

* From `2019-09-15 <https://lore.kernel.org/lkml/CAHk-=wiP4K8DRJWsCo=20hn_6054xBamGKF2kPgUzpB5aMaofA@mail.gmail.com/>`_::

    One _particularly_ last-minute revert is the top-most commit (ignoring
    the version change itself) done just before the release, and while
    it's very annoying, it's perhaps also instructive.

    What's instructive about it is that I reverted a commit that wasn't
    actually buggy. In fact, it was doing exactly what it set out to do,
    and did it very well. In fact it did it _so_ well that the much
    improved IO patterns it caused then ended up revealing a user-visible
    regression due to a real bug in a completely unrelated area.

    The actual details of that regression are not the reason I point that
    revert out as instructive, though. It's more that it's an instructive
    example of what counts as a regression, and what the whole "no
    regressions" kernel rule means.

    [...] The reverted commit didn't change any API's, and it didn't introduce
    any new bugs. But it ended up exposing another problem, and as such caused
    a kernel upgrade to fail for a user. So it got reverted.

    The point here being that we revert based on user-reported _behavior_, not
    based on some "it changes the ABI" or "it caused a bug" concept. The problem
    was really pre-existing, and it just didn't happen to trigger before. [...]

    Take-away from the whole thing: it's not about whether you change the
    kernel-userspace ABI, or fix a bug, or about whether the old code
    "should never have worked in the first place". It's about whether
    something breaks existing users' workflow.

  (有一次*特别*临门一脚的 revert，是发布前一刻所做的最顶层提交（忽略版本号变更本身）；它虽然非常恼人，但也许很有启发意义。
  它的启发之处在于，我 revert 了一个实际上没有缺陷的提交。事实上，它完全做到了它想做的事，而且做得很好。它做得*太*好了，以至于它带来的大幅改善的 IO 模式最终暴露了一个用户可见的回归，根因是另一个毫不相关领域的真实缺陷。
  不过，我指出那次 revert 有启发意义，并不是因为那个回归的细节本身，而更多是因为它是一个很好的例子，说明了什么算作回归，以及整条"禁止回归"的内核规则意味着什么。
  [……]被 revert 的提交没有改动任何 API，也没有引入任何新缺陷。但它最终暴露了另一个问题，因而导致一个用户的内核升级失败。于是它被 revert 了。
  这里的要点是：我们依据用户报告的*行为*来 revert，而不是依据"它改变了 ABI"或"它引入了一个缺陷"之类的概念。那个问题确实早已存在，只是碰巧之前没有触发。[……]
  整件事的启示在于：这不在于你是否改动了内核-用户空间 ABI，不在于你是否修复了缺陷，也不在于旧代码"从一开始就不应该能工作"。它在于是否有东西破坏了现有用户的工作流。)

* From `2017-11-05 <https://lore.kernel.org/all/CA+55aFzUvbGjD8nQ-+3oiMBx14c_6zOj2n7KLN3UsJ-qsd4Dcw@mail.gmail.com/>`_::

    And our regression rule has never been "behavior doesn't change".
    That would mean that we could never make any changes at all.

  (我们的回归规则从来都不是"行为不许改变"。那意味着我们什么变更都做不了。)

* From `2020-05-21 <https://lore.kernel.org/all/CAHk-=wiVi7mSrsMP=fLXQrXK_UimybW=ziLOwSzFTtoXUacWVQ@mail.gmail.com/>`_::

    No amount of "you shouldn't have used this" or "that behavior was
    undefined, it's your own fault your app broke" or "that used to work
    simply because of a kernel bug" is at all relevant.

  ("你不应该使用这个"、"那个行为是未定义的，你的应用坏了是你自己的错"、"那能工作纯粹是因为内核有个缺陷"——这类说法半点都不相干。)

* From `2021-05-21 <https://lore.kernel.org/all/CAHk-=wiVi7mSrsMP=fLXQrXK_UimybW=ziLOwSzFTtoXUacWVQ@mail.gmail.com/>`_::

    But no, "that was documented to be broken" (whether it's because the code
    was in staging or because the man-page said something else) is irrelevant.
    If staging code is so useful that people end up using it, that means that
    it's basically regular kernel code with a flag saying "please clean this
    up".

    [...]

    The other side of the coin is that people who talk about "API stability" are
    entirely wrong. API's don't matter either. You can make any changes to an
    API you like - as long as nobody notices.

    Again, the regression rule is not about documentation, not about API's, and
    not about the phase of the moon.

  (不，"那已经文档说明是坏的"（无论是代码在 staging 里，还是手册页说了别的话）都无关紧要。如果 staging 代码好用到人们最终用上了它，那它基本上就是普通的内核代码，只是插了一面写着"请清理我"的旗子。
  [……]
  硬币的另一面是：那些谈"API 稳定性"的人完全搞错了。API 也不重要。你可以随意改动一个 API——只要没人注意到。
  再说一遍，回归规则与文档无关，与 API 无关，也与月相无关。)

* From `2012-07-06 <https://lore.kernel.org/all/CA+55aFwnLJ+0sjx92EGREGTWOx84wwKaraSzpTNJwPVV8edw8g@mail.gmail.com/>`_::

    > Now this got me wondering if Debian _unstable_ actually qualifies as a
    > standard distro userspace.

    Oh, if the kernel breaks some standard user space, that counts. Tons
    of people run Debian unstable

  (> 这倒让我好奇 Debian _unstable_ 是否真的算标准发行版用户空间。
  哦，如果内核破坏了某个标准用户空间，那就算数。一大把人在用 Debian unstable。)

* From `2011-05-06 <https://lore.kernel.org/all/BANLkTi=KVXjKR82sqsz4gwjr+E0vtqCmvA@mail.gmail.com/>`_::

    It's clearly NOT an internal tracepoint. By definition. It's being
    used by powertop.

  (它显然不是内部 tracepoint。从定义上讲就不是——powertop 在用它。)

On regressions noticed by users or test-suites/CIs(关于用户或测试套件/CI 注意到的回归)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2026-01-22 <https://lore.kernel.org/all/CAHk-=wheQNiW_WtHGO7bKkT7Uib-p+ai2JP9M+z+FYcZ6CAxYA@mail.gmail.com/>`_::

    Users complaining is the only real line in the end.

    [...] a test-suite complaining is then often a *very* good indication that
    maybe users will hit some problem, and test suite issues should be taken
    very seriously [...]

    But a test-suite error isn't necessarily where you have to draw the
    line - it's a big red flag [...]

  (归根结底，用户抱怨才是唯一真正的底线。
  [……]不过，测试套件抱怨往往是一个非常强的信号，说明用户也许会撞上某些问题，测试套件的问题应当被非常认真地对待[……]
  但测试套件的报错未必就是必须划线的地方——它是一个大大的红旗[……])

* From `2024-29-01 <https://lore.kernel.org/all/CAHk-=wg8BrZEzjJ5kUyZzHPZmFqH6ooMN1gRBCofxxCfucgjaw@mail.gmail.com/>`_::

    The "no regressions" rule is not about made-up "if I do this, behavior
    changes".

    The "no regressions" rule is about *users*.

    If you have an actual user that has been doing insane things, and we
    change something, and now the insane thing no longer works, at that
    point it's a regression, and we'll sigh, and go "Users are insane" and
    have to fix it.

    But if you have some random test that now behaves differently, it's
    not a regression. It's a *warning* sign, sure: tests are useful.

  ("禁止回归"规则针对的不是编造出来的"如果我这样做，行为就变了"。
  "禁止回归"规则针对的是*用户*。
  如果有一个真实用户一直在做疯狂的事情，而我们改了点什么，现在那件疯狂的事不再可用，那它就是回归：我们会叹口气，说一句"用户就是疯狂"，然后不得不去修。
  但如果只是某个随机的测试现在行为不同了，那不是回归。它当然是一个*警告*信号：测试是有用的。)

On accepting when a regression occurred(关于承认回归已经发生)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2026-01-22 <https://lore.kernel.org/all/CAHk-=wheQNiW_WtHGO7bKkT7Uib-p+ai2JP9M+z+FYcZ6CAxYA@mail.gmail.com/>`_::

    But starting to argue about users reporting breaking changes is
    basically the final line for me. I have a couple of people that I have
    in my spam block-list and refuse to have anything to do with, and they
    have generally been about exactly that.

    Note how it's not about making mistakes and _causing_ the regression.
    That's normal. That's development. But then arguing about it is a
    no-no.

  (但开始与报告破坏性变更的用户争辩，对我来说基本上就是最后红线。有几个人在我的垃圾邮件黑名单里，我拒绝与他们有任何来往，而原因基本上恰恰就是这个。
  注意，问题不在于犯错并*造成*回归。那是正常的，那就是开发。但为此争辩就不行了。)

* From `2024-06-23 <https://lore.kernel.org/all/CAHk-=wi_KMO_rJ6OCr8mAWBRg-irziM=T9wxGC+J1VVoQb39gw@mail.gmail.com/>`_::

    We don't introduce regressions and then blame others.

    There's a very clear rule in kernel development: things that break
    other things ARE NOT FIXES.

    EVER.

    They get reverted, or the thing they broke gets fixed.

  (我们不引入回归然后怪罪别人。
  内核开发里有一条非常清晰的规则：破坏其他东西的东西不是修复。
  永远不是。
  要么把它们 revert 掉，要么把被它们破坏的东西修好。)

* From `2021-06-05 <https://lore.kernel.org/all/CAHk-=wiUVqHN76YUwhkjZzwTdjMMJf_zN4+u7vEJjmEGh3recw@mail.gmail.com/>`_::

    THERE ARE NO VALID ARGUMENTS FOR REGRESSIONS.

    Honestly, security people need to understand that "not working" is not
    a success case of security. It's a failure case.

    Yes, "not working" may be secure. But security in that case is *pointless*.

  (回归没有任何正当理由。
  说实话，安全领域的人需要明白："不可用"不是安全的成功案例，而是失败案例。
  是的，"不可用"也许安全。但那种情况下的安全是*毫无意义的*。)

* From `2017-10-26(5) <https://lore.kernel.org/lkml/CA+55aFwiiQYJ+YoLKCXjN_beDVfu38mg=Ggg5LFOcqHE8Qi7Zw@mail.gmail.com/>`_::

    [...] when regressions *do* occur, we admit to them and fix them, instead of
    blaming user space.

    The fact that you have apparently been denying the regression now for
    three weeks means that I will revert, and I will stop pulling apparmor
    requests until the people involved understand how kernel development
    is done.

  ([……]当回归*确实*发生时，我们承认它并修复它，而不是怪罪用户空间。
  你显然已经否认这个回归三个星期了，这意味着我会 revert，并且我会停止拉取 apparmor 的请求，直到相关人员理解内核开发该怎么做。)

On back-and-forth(关于来回拉锯)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2024-05-28 <https://lore.kernel.org/all/CAHk-=wgtb7y-bEh7tPDvDWru7ZKQ8-KMjZ53Tsk37zsPPdwXbA@mail.gmail.com/>`_::

    The "no regressions" rule is that we do not introduce NEW bugs.

    It *literally* came about because we had an endless dance of "fix two
    bugs, introduce one new one", and that then resulted in a system that
    you cannot TRUST.

  ("禁止回归"规则说的是：我们不引入*新*缺陷。
  它*字面意义上*就是因此而来的：我们曾有过"修两个缺陷、引入一个新缺陷"的无尽舞蹈，最终导致一个你无法*信任*的系统。)

* From `2021-09-20(1) <https://lore.kernel.org/all/CAHk-=wi7DB2SJ-wngVvsJ7Ak2cM556Q8437sOXo4EJt2BWPdEg@mail.gmail.com/>`_::

    And the thing that makes regressions special is that back when I
    wasn't so strict about these things, we'd end up in endless "seesaw
    situations" where somebody would fix something, it would break
    something else, then that something else would break, and it would
    never actually converge on anything reliable at all.

  (让回归变得特殊的原因在于：早在我对这些事情不这么严格的时候，我们最终会陷入无尽的"跷跷板局面"——有人修好一个东西，另一个东西坏了，然后那个东西又坏了，永远也无法真正收敛到任何可靠的状态。)

* From `2015-08-13 <https://lore.kernel.org/all/CA+55aFxk8-BsiKwr_S-c+4G6wihKPQVMLE34H9wOZpeua6W9+Q@mail.gmail.com/>`_::

    The strict policy of no regressions actually originally started mainly wrt
    suspend/resume issues, where the "fix one machine, break another" kind of
    back-and-forth caused endless problems, and meant that we didn't actually
    necessarily make any forward progress, just moving a problem around.

  (禁止回归的严格政策最初实际上主要是针对挂起/恢复（suspend/resume）问题的：那种"修好一台机器、弄坏另一台"式的来回拉锯造成了无尽的问题，也意味着我们实际上未必取得任何前进，只是在把问题搬来搬去。)

On changes with a risk of causing regressions(关于有引发回归风险的变更)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2023-06-02 <https://lore.kernel.org/all/CAHk-=wgyAGUMHmQM-5Eb556z5xiHZB7cF05qjrtUH4F7P-1rSA@mail.gmail.com/>`_::

    So what I think you should do is to fix the bug right, with a clean
    patch, and no crazy hacks. That is something we can then apply and
    test. All the while knowing full well that "uhhuh, this is a visible
    change, we may have to revert it".

    If then some *real* load ends up showing a regression, we may just be
    screwed. Our current behavior may be buggy, but we have the rule that
    once user space depends on kernel bugs, they become features pretty
    much by definition, however much we might dislike it.

  (所以我认为你应该做的是：用一个干净的补丁、不搞疯狂的 hack，把缺陷修好。那样我们就可以应用并测试它。同时心里很清楚："嗯哼，这是一个可见的变更，我们可能不得不 revert 它"。
  如果之后某个*真实*负载最终显示出回归，我们可能就完蛋了。我们当前的行为也许有缺陷，但我们有条规则：一旦用户空间依赖了内核缺陷，它们几乎按定义就变成了特性，无论我们多么不喜欢。)

On in-kernel workarounds to avoid regressions(关于在内核内用变通手段避免回归)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2017-10-26(6) <https://lore.kernel.org/lkml/CA+55aFxW7NMAMvYhkvz1UPbUTUJewRt6Yb51QAx5RtrWOwjebg@mail.gmail.com/>`_::

    Behavioral changes happen, and maybe we don't even support some
    feature any more. There's a number of fields in /proc/<pid>/stat that
    are printed out as zeroes, simply because they don't even *exist* in
    the kernel any more, or because showing them was a mistake (typically
    an information leak). But the numbers got replaced by zeroes, so that
    the code that used to parse the fields still works. The user might not
    see everything they used to see, and so behavior is clearly different,
    but things still _work_, even if they might no longer show sensitive
    (or no longer relevant) information.

  (行为变化是会发生的，也许我们甚至不再支持某个功能。/proc/<pid>/stat 中有一些字段会打印成零，仅仅是因为它们在内核中甚至已经*不存在*了，或者是因为展示它们本来就是个错误（通常是信息泄露）。但这些数字被替换成了零，于是以前解析这些字段的代码仍然可以工作。用户可能看不到他们过去看到的全部内容，所以行为显然不同，但事情仍然*可用*，即使它们可能不再展示敏感的（或不再相关的）信息。)

On regressions caused by bugfixes(关于由缺陷修复引起的回归)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2018-08-03 <https://lore.kernel.org/all/CA+55aFwWZX=CXmWDTkDGb36kf12XmTehmQjbiMPCqCRG2hi9kw@mail.gmail.com/>`_::

    > Kernel had a bug which has been fixed

    That is *ENTIRELY* immaterial.

    Guys, whether something was buggy or not DOES NOT MATTER.

    [...]

    It's basically saying "I took something that worked, and I broke it,
    but now it's better". Do you not see how f*cking insane that statement
    is?

  (> 内核有一个已被修复的缺陷
  那是*完全*无关紧要的。
  伙计们，一个东西有没有缺陷*根本*不重要。
  [……]
  这基本上是在说"我把一个能用的东西弄坏了，但现在它更好了"。你们难道看不出这句话有多么疯狂吗？)

On internal API changes(关于内部 API 变更)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2017-10-26(7) <https://lore.kernel.org/lkml/CA+55aFxW7NMAMvYhkvz1UPbUTUJewRt6Yb51QAx5RtrWOwjebg@mail.gmail.com/>`_::

    We do API breakage _inside_ the kernel all the time. We will fix
    internal problems by saying "you now need to do XYZ", but then it's
    about internal kernel API's, and the people who do that then also
    obviously have to fix up all the in-kernel users of that API. Nobody
    can say "I now broke the API you used, and now _you_ need to fix it
    up". Whoever broke something gets to fix it too.

  (我们在内核*内部*一直都在破坏 API。我们会通过"你现在需要改用 XYZ"来修复内部问题，但那是针对内核内部 API 而言的，而且这样做的人显然还必须修好该 API 的所有内核内使用者。没有人可以说"我刚刚破坏了你使用的 API，现在*你*需要修好它"。谁破坏了东西，谁就得负责修好它。)

On regressions only found after a long time(关于很久之后才发现的回归)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2024-03-28 <https://lore.kernel.org/all/CAHk-=wgFuoHpMk_Z_R3qMXVDgq0N1592+bABkyGjwwSL4zBtHA@mail.gmail.com/>`_::

    I'm definitely not reverting a patch from almost a decade ago as a
    regression.

    If it took that long to find, it can't be that critical of a regression.

    So yes, let's treat it as a regular bug.

  (我绝对不会把一个将近十年前的补丁当作回归来 revert。
  如果花了这么久才被发现，它不可能是什么严重致命的回归。
  所以，是的，我们把它当作普通缺陷来对待。)

On testing regressions fixes in linux-next(关于在 linux-next 中测试回归修复)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* On `maintainers summit 2024 <https://lwn.net/Articles/990599/>`_::

   So running fixes though linux-next is just a waste of time.

  (所以让修复跑一遍 linux-next 纯属浪费时间。)

On a few other aspects related to regressions(关于与回归相关的其他一些方面)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* From `2025-07-29(2) <https://lore.kernel.org/all/CAHk-=wjj9DvOZtmTkoLtyfHmy5mNKy6q_96d9=4FUEDXre=cww@mail.gmail.com/>`_
  [which `is not quite a regression, but a huge inconvenience <https://lore.kernel.org/all/CAHk-=wgO0Rx2LcYT4f75Xs46orbJ4JxO2jbAFQnVKDYAjV5HeQ@mail.gmail.com/>`_]::

    I no longer have sound.

    I also suspect that it's purely because "make oldconfig" doesn't work,
    and probably turned off my old Intel HDA settings. Or something.

    Renaming config parameters is *bad*. I've harped on the Kconfig phase
    of the kernel build probably being our nastiest point, and a real pain
    point to people getting involved with development simply because
    building your own kernel can be so daunting with hundreds of fairly
    esoteric questions.

  (我的声音没了。
  我还怀疑这纯粹是因为 "make oldconfig" 不起作用，很可能把我旧的 Intel HDA 设置关掉了。或者是别的什么原因。
  重命名配置参数是*坏*事。我一直念叨内核构建的 Kconfig 阶段可能是我们最糟糕的地方，也是人们参与开发的一个真正的痛点，原因很简单：自建内核本来就让人望而生畏，何况还有成百上千个相当晦涩的问题。)

..
   end-of-content
..
   This text is available under GPL-2.0+ or CC-BY-4.0, as stated at the top
   of the file. If you want to distribute this text under CC-BY-4.0 only,
   please use "The Linux kernel developers" for author attribution and link
   this as source:
   https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/plain/Documentation/process/handling-regressions.rst
..
   Note: Only the content of this RST file as found in the Linux kernel sources
   is available under CC-BY-4.0, as versions of this text that were processed
   (for example by the kernel's build system) might contain content taken from
   files which use a more restrictive license.
