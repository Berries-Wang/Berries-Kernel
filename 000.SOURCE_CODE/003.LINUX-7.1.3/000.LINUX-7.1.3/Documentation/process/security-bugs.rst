.. _securitybugs:

Security bugs(安全缺陷)
=======================

Linux kernel developers take security very seriously.  As such, we'd
like to know when a security bug is found so that it can be fixed and
disclosed as quickly as possible.
(Linux 内核开发者非常认真地对待安全问题。因此，我们希望得知何时发现了
安全缺陷，以便尽快修复并披露。)

Preparing your report(准备你的报告)
-----------------------------------

Like with any bug report, a security bug report requires a lot of analysis work
from the developers, so the more information you can share about the issue, the
better.  Please review the procedure outlined in
Documentation/admin-guide/reporting-issues.rst if you are unclear about what
information is helpful.  The following information are absolutely necessary in
**any** security bug report:
(与任何缺陷报告一样，一份安全缺陷报告需要开发者做大量分析工作，因此你
能分享的关于该问题的信息越多越好。如果你不清楚哪些信息有帮助，请查阅
Documentation/admin-guide/reporting-issues.rst 中概述的流程。以下信息在
**任何** 一份安全缺陷报告中都是绝对必要的：)

  * **affected kernel version range**: with no version indication, your report
    will not be processed.  A significant part of reports are for bugs that
    have already been fixed, so it is extremely important that vulnerabilities
    are verified on recent versions (development tree or latest stable
    version), at least by verifying that the code has not changed since the
    version where it was detected.
    (**受影响的内核版本范围**：如果没有版本信息，你的报告将不会被处理。
    相当一部分报告针对的是早已修复的缺陷，因此极其重要的是，要在较新的
    版本 (开发树或最新 stable 版本) 上验证漏洞，至少要验证自发现该问题的
    版本以来代码没有发生变化。)

  * **description of the problem**: a detailed description of the problem, with
    traces showing its manifestation, and why you consider that the observed
    behavior as a problem in the kernel, is necessary.
    (**问题描述**：需要对问题的详细描述，包括显示其表现的跟踪信息，以及
    你为什么认为观察到的行为是内核中的问题。)

  * **reproducer**: developers will need to be able to reproduce the problem to
    consider a fix as effective.  This includes both a way to trigger the issue
    and a way to confirm it happens.  A reproducer with low complexity
    dependencies will be needed (source code, shell script, sequence of
    instructions, file-system image etc).  Binary-only executables are not
    accepted.  Working exploits are extremely helpful and will not be released
    without consent from the reporter, unless they are already public.  By
    definition if an issue cannot be reproduced, it is not exploitable, thus it
    is not a security bug.
    (**复现方法**：开发者需要能够复现问题，才能认定修复有效。这包括触发
    该问题的方法以及确认其发生的方法。需要依赖较少的复现方法 (源代码、
    shell 脚本、指令序列、文件系统映像等)。不接受纯二进制可执行文件。可
    用的漏洞利用 (exploit) 极有帮助，未经报告者同意不会公开，除非其已经
    公开。按定义，如果一个问题无法复现，它就不可利用，因而也就不是安全
    缺陷。)

  * **conditions**: if the bug depends on certain configuration options,
    sysctls, permissions, timing, code modifications etc, these should be
    indicated.
    (**条件**：如果该缺陷依赖于特定的配置选项、sysctl、权限、时序、代码
    修改等，应当予以说明。)

In addition, the following information are highly desirable:
(此外，非常希望提供以下信息：)

  * **suspected location of the bug**: the file names and functions where the
    bug is suspected to be present are very important, at least to help forward
    the report to the appropriate maintainers.  When not possible (for example,
    "system freezes each time I run this command"), the security team will help
    identify the source of the bug.
    (**疑似缺陷位置**：疑似存在缺陷的文件名和函数非常重要，至少有助于
    把报告转发给相应的维护者。当无法给出时 (例如 "每次运行这个命令系统
    都会冻结")，安全团队会帮助确定缺陷来源。)

  * **a proposed fix**: bug reporters who have analyzed the cause of a bug in
    the source code almost always have an accurate idea on how to fix it,
    because they spent a long time studying it and its implications.  Proposing
    a tested fix will save maintainers a lot of time, even if the fix ends up
    not being the right one, because it helps understand the bug.  When
    proposing a tested fix, please always format it in a way that can be
    immediately merged (see Documentation/process/submitting-patches.rst).
    This will save some back-and-forth exchanges if it is accepted, and you
    will be credited for finding and fixing this issue.  Note that in this case
    only a ``Signed-off-by:`` tag is needed, without ``Reported-by:`` when the
    reporter and author are the same.
    (**建议的修复**：在源代码中分析过缺陷成因的报告者几乎总是对如何修复
    它有准确的想法，因为他们花了很长时间研究该缺陷及其影响。提出一个经
    过测试的修复会为维护者节省大量时间，即使该修复最终并非正确的那一个，
    因为它有助于理解缺陷。提出经过测试的修复时，请始终把它格式化为可以
    立即合并的形式 (见 Documentation/process/submitting-patches.rst)。如
    果它被接受，这会省去一些来回交流，而且你会因发现并修复此问题而获得
    署名。注意在这种情况下，当报告者与作者为同一人时，只需要
    ``Signed-off-by:`` 标签，而不需要 ``Reported-by:``。)

  * **mitigations**: very often during a bug analysis, some ways of mitigating
    the issue appear. It is useful to share them, as they can be helpful to
    keep end users protected during the time it takes them to apply the fix.
    (**缓解措施**：在缺陷分析过程中，常常会出现一些缓解该问题的办法。分
    享它们很有用，因为它们有助于在最终用户应用修复之前保持受保护。)

What qualifies as a security bug(什么算作安全缺陷)
--------------------------------------------------

It is important that most bugs are handled publicly so as to involve the widest
possible audience and find the best solution.  By nature, bugs that are handled
in closed discussions between a small set of participants are less likely to
produce the best possible fix (e.g., risk of missing valid use cases, limited
testing abilities).
(大多数缺陷以公开方式处理非常重要，这样可以让尽可能广泛的受众参与，找
到最佳解决方案。本质上，在少数参与者之间以封闭讨论方式处理的缺陷，更不
太可能产生最佳修复 (例如遗漏有效用例的风险、测试能力有限)。)

It turns out that the majority of the bugs reported via the security team are
just regular bugs that have been improperly qualified as security bugs due to
a lack of awareness of the Linux kernel's threat model, as described in
Documentation/process/threat-model.rst, and ought to have been sent through
the normal channels described in Documentation/admin-guide/reporting-issues.rst
instead.
(事实证明，通过安全团队报告的缺陷中，大多数只是普通缺陷，由于缺乏对
Linux 内核威胁模型的了解 (如 Documentation/process/threat-model.rst 所
述) 而被错误地定性为安全缺陷，本应通过
Documentation/admin-guide/reporting-issues.rst 所述的正常渠道提交。)

The security list exists for urgent bugs that grant an attacker a capability
they are not supposed to have on a correctly configured production system, and
can be easily exploited, representing an imminent threat to many users.  Before
reporting, consider whether the issue actually crosses a trust boundary on such
a system.
(安全列表的存在是为了处理那些让攻击者在正确配置的生产系统上获得本不应
有的能力、且容易被利用、对众多用户构成迫近威胁的紧急缺陷。在报告之前，
请考虑该问题在这样的系统上是否真的跨越了信任边界。)

**If you resorted to AI assistance to identify a bug, you must treat it as
public**. While you may have valid reasons to believe it is not, the security
team's experience shows that bugs discovered this way systematically surface
simultaneously across multiple researchers, often on the same day. In this
case, do not publicly share a reproducer, as this could cause unintended harm;
just mention that one is available and maintainers might ask for it privately
if they need it.
(**如果你借助 AI 协助发现了一个缺陷，你必须把它当作已经公开**。虽然你
可能有充分理由认为它没有公开，但安全团队的经验表明，以这种方式发现的缺
陷几乎必然会在多个研究者之间同时浮出水面，而且常常是在同一天。在这种情
况下，不要公开分享复现方法，因为这可能造成意想不到的伤害；只需说明你有
复现方法，维护者如果需要可能会私下向你索取。)

If you are unsure whether an issue qualifies, err on the side of reporting
privately: the security team would rather triage a borderline report than miss
a real vulnerability.  Reporting ordinary bugs to the security list, however,
does not make them move faster and consumes triage capacity that other reports
need.
(如果你不确定某个问题是否符合条件，倾向于私密报告更为稳妥：安全团队宁
可甄别一份处于边缘的报告，也不愿错过真实的漏洞。不过，把普通缺陷报告到
安全列表并不会让它处理得更快，反而会消耗其他报告所需的甄别能力。)

Identifying contacts(确定联系人)
--------------------------------

The most effective way to report a security bug is to send it directly to the
affected subsystem's maintainers and Cc: the Linux kernel security team.  Do
not send it to a public list at this stage, unless you have good reasons to
consider the issue as being public or trivial to discover (e.g. result of a
widely available automated vulnerability scanning tool that can be repeated by
anyone, or use of AI-based tools).
(报告安全缺陷最有效的方式是把它直接发给受影响子系统的维护者，并抄送
Linux 内核安全团队。在这一阶段不要把它发到公开列表，除非你有充分理由认
为该问题是公开的或容易被发现 (例如任何人都可以重复的、广泛可用的自动化
漏洞扫描工具的结果，或使用了基于 AI 的工具)。)

If you're sending a report for issues affecting multiple parts in the kernel,
even if they're fairly similar issues, please send individual messages (think
that maintainers will not all work on the issues at the same time). The only
exception is when an issue concerns closely related parts maintained by the
exact same subset of maintainers, and these parts are expected to be fixed all
at once by the same commit, then it may be acceptable to report them at once.
(如果你为影响内核多个部分的问题发送报告，即使它们是相当相似的问题，也
请逐个发送单独的消息 (想一想维护者不会都在同一时间处理这些问题)。唯一
的例外是当问题涉及由完全相同的维护者子集维护的紧密相关部分、且这些部分
预期由同一提交一并修复时，此时一次性报告它们是可以接受的。)

One difficulty for most first-time reporters is to figure the right list of
recipients to send a report to.  In the Linux kernel, all official maintainers
are trusted, so the consequences of accidentally including the wrong maintainer
are essentially a bit more noise for that person, i.e. nothing dramatic.  As
such, a suitable method to figure the list of maintainers (which kernel
security officers use) is to rely on the get_maintainer.pl script, tuned to
only report maintainers.  This script, when passed a file name, will look for
its path in the MAINTAINERS file to figure a hierarchical list of relevant
maintainers.  Calling it a first time with the finest level of filtering will
most of the time return a short list of this specific file's maintainers::

  $ ./scripts/get_maintainer.pl --no-l --no-r --pattern-depth 1 \
    drivers/example.c
  Developer One <dev1@example.com> (maintainer:example driver)
  Developer Two <dev2@example.org> (maintainer:example driver)

(对大多数首次报告者来说，一个困难是弄清应把报告发给哪些收件人。在
Linux 内核中，所有正式维护者都是受信任的，因此不小心包含错误维护者的后
果基本上只是给那个人带来一点噪音，没什么大不了的。因此，确定维护者列
表的一个合适方法 (内核安全官员也在用) 是使用 get_maintainer.pl 脚本，并
调整为只报告维护者。该脚本在被传入一个文件名时，会在 MAINTAINERS 文件
中查找其路径，从而得出相关维护者的层级列表。先用最细的过滤级别调用它，
大多数情况下会返回该特定文件的维护者的简短列表：)

These two maintainers should then receive the message.  If the command does not
return anything, it means the affected file is part of a wider subsystem, so we
should be less specific::

  $ ./scripts/get_maintainer.pl --no-l --no-r drivers/example.c
  Developer One <dev1@example.com> (maintainer:example subsystem)
  Developer Two <dev2@example.org> (maintainer:example subsystem)
  Developer Three <dev3@example.com> (maintainer:example subsystem [GENERAL])
  Developer Four <dev4@example.org> (maintainer:example subsystem [GENERAL])

(这两位维护者随后应当收到该消息。如果该命令没有返回任何内容，说明受影
响的文件属于一个更大的子系统，因此我们应当放宽条件：)

Here, picking the first, most specific ones, is sufficient.  When the list is
long, it is possible to produce a comma-delimited e-mail address list on a
single line suitable for use in the To: field of a mailer like this::

  $ ./scripts/get_maintainer.pl --no-tree --no-l --no-r --no-n --m \
    --no-git-fallback --no-substatus --no-rolestats --no-multiline \
    --pattern-depth 1 drivers/example.c
  dev1@example.com, dev2@example.org

(这里，选择第一个、最具体的那些就足够了。当列表很长时，可以生成适合放
进邮件客户端 To: 字段的、单行的逗号分隔电子邮件地址列表，如下所示：)

or this for the wider list::

  $ ./scripts/get_maintainer.pl --no-tree --no-l --no-r --no-n --m \
    --no-git-fallback --no-substatus --no-rolestats --no-multiline \
    drivers/example.c
  dev1@example.com, dev2@example.org, dev3@example.com, dev4@example.org

(或者用以下命令得到更宽的列表：)

If at this point you're still facing difficulties spotting the right
maintainers, **and only in this case**, it's possible to send your report to
the Linux kernel security team only.  Your message will be triaged, and you
will receive instructions about whom to contact, if needed.  Your message may
equally be forwarded as-is to the relevant maintainers.
(如果此时你仍然难以找到正确的维护者，**且仅在这种情况下**，可以把报
告只发给 Linux 内核安全团队。你的消息会被甄别，如有需要，你会收到关于
应该联系谁的指示。你的消息同样可能被原样转发给相关维护者。)

Responsible use of AI to find bugs(负责任地使用 AI 查找缺陷)
------------------------------------------------------------

A significant fraction of bug reports submitted to the security team are
actually the result of code reviews assisted by AI tools. While this can be an
efficient means to find bugs in rarely explored areas, it causes an overload on
maintainers, who are sometimes forced to ignore such reports due to their poor
quality or accuracy. As such, reporters must be particularly cautious about a
number of points which tend to make these reports needlessly difficult to
handle:
(提交给安全团队的缺陷报告中，相当一部分实际上是 AI 工具辅助代码评审的
产物。虽然这可以是在很少被探索的领域发现缺陷的有效手段，但它给维护者造
成了过载，维护者有时不得不因其质量或准确性欠佳而无视这类报告。因此，报
告者必须特别注意以下几个容易使这类报告不必要地难以处理的要点：)

  * **Length**: AI-generated reports tend to be excessively long, containing
    multiple sections and excessive detail. This makes it difficult to spot
    important information such as affected files, versions, and impact. Please
    ensure that a clear summary of the problem and all critical details are
    presented first. Do not require triage engineers to scan multiple pages of
    text. Configure your tools to produce concise, human-style reports.
    (**篇幅**：AI 生成的报告往往过长，包含多个小节和过量的细节。这使人
    难以找到重要信息，例如受影响的文件、版本和影响。请确保把问题的清晰
    摘要和所有关键细节放在最前面。不要让甄别工程师去翻阅多页文本。请你
    的工具生成简洁的、人类风格的报告。)

  * **Formatting**: Most AI-generated reports are littered with Markdown tags.
    These decorations complicate the search for important information and do
    not survive the quoting processes involved in forwarding or replying.
    Please **always convert your report to plain text** without any formatting
    decorations before sending it.
    (**格式**：大多数 AI 生成的报告都散布着 Markdown 标记。这些装饰增加
    了查找重要信息的难度，而且无法在转发或回复所涉及的引用过程中保留。
    请在发送之前 **务必把报告转换为纯文本** ，不带任何格式修饰。)

  * **Impact Evaluation**: Many AI-generated reports lack an understanding
    of the kernel's threat model (see Documentation/process/threat-model.rst)
    and go to great lengths inventing theoretical consequences. This adds
    noise and complicates triage. Please stick to verifiable facts (e.g.,
    "this bug permits any user to gain CAP_NET_ADMIN") without enumerating
    speculative implications. Have your tool read this documentation as
    part of the evaluation process.
    (**影响评估**：许多 AI 生成的报告缺乏对内核威胁模型的理解 (见
    Documentation/process/threat-model.rst)，并绞尽脑汁编造理论上的后
    果。这会带来噪音并使甄别复杂化。请只陈述可验证的事实 (例如 "此缺陷
    允许任何用户获得 CAP_NET_ADMIN")，不要罗列推测性的影响。让你的工具
    把这份文档作为评估过程的一部分来阅读。)

  * **Reproducer**: AI-based tools are often capable of generating reproducers.
    Please always ensure your tool provides one and **test it thoroughly**. If
    the reproducer does not work, or if the tool cannot produce one, the
    validity of the report should be seriously questioned. Note that since the
    report will be posted to a public list, the reproducer should only be
    shared upon maintainers' request.
    (**复现方法**：基于 AI 的工具通常能够生成复现方法。请始终确保你的工
    具提供其一，并 **对其充分测试** 。如果复现方法不起作用，或工具无法
    产生一个，则报告的有效性应受到严重质疑。注意，由于报告会发布到公开
    列表，复现方法只应在维护者提出要求时才共享。)

  * **Propose a Fix**: Many AI tools are actually better at writing code than
    evaluating it. Please ask your tool to propose a fix and **test it** before
    reporting the problem. If the fix cannot be tested because it relies on
    rare hardware or almost extinct network protocols, the issue is likely not
    a security bug. In any case, if a fix is proposed, it must adhere to
    Documentation/process/submitting-patches.rst and include a 'Fixes:' tag
    designating the commit that introduced the bug.
    (**提出修复**：许多 AI 工具实际上更擅长写代码而不是评估代码。请在
    报告问题之前让你的工具提出修复并 **对它进行测试** 。如果修复因为依
    赖少见的硬件或几乎绝迹的网络协议而无法测试，该问题很可能不是安全缺
    陷。无论如何，如果提出了修复，它必须遵循
    Documentation/process/submitting-patches.rst，并包含指明引入该缺陷的
    提交的 'Fixes:' 标签。)

Failure to consider these points exposes your report to the risk of being
ignored.
(不考虑这些要点会使你的报告面临被忽略的风险。)

Use common sense when evaluating the report. If the affected file has not been
touched for more than one year and is maintained by a single individual, it is
likely that usage has declined and exposed users are virtually non-existent
(e.g., drivers for very old hardware, obsolete filesystems). In such cases,
there is no need to consume a maintainer's time with an unimportant report. If
the issue is clearly trivial and publicly discoverable, you should report it
directly to the public mailing lists.
(评估报告时请运用常识。如果受影响的文件已超过一年无人改动、且仅由一个
人维护，那么很可能其使用已经减少、暴露用户几乎不存在 (例如非常老旧硬件
的驱动、过时的文件系统)。在这种情况下，没有必要用一份无关紧要的报告消
耗维护者的时间。如果问题明显无关紧要且可被公开发现，你应当直接把它报告
到公开邮件列表。)

Sending the report(发送报告)
----------------------------

Reports are to be sent over e-mail exclusively.  Please use a working e-mail
address, preferably the same that you want to appear in ``Reported-by`` tags
if any.  If unsure, send your report to yourself first.
(报告只能通过电子邮件发送。请使用一个有效的电子邮件地址，最好与你想出
现在 ``Reported-by`` 标签中的地址相同 (如果有的话)。如果不确定，先把报
告发给自己。)

The security team and maintainers almost always require additional
information beyond what was initially provided in a report and rely on
active and efficient collaboration with the reporter to perform further
testing (e.g., verifying versions, configuration options, mitigations, or
patches). Before contacting the security team, the reporter must ensure
they are available to explain their findings, engage in discussions, and
run additional tests.  Reports where the reporter does not respond promptly
or cannot effectively discuss their findings may be abandoned if the
communication does not quickly improve.
(安全团队和维护者几乎总是需要报告初始内容之外的更多信息，并依赖与报
告者的积极高效协作来进行进一步测试 (例如验证版本、配置选项、缓解措施
或补丁)。在联系安全团队之前，报告者必须确保自己有时间解释其发现、参与
讨论并运行额外的测试。如果沟通没有迅速改善，报告者不及时回应或无法有效
讨论其发现的报告可能会被放弃。)

The report must be sent to maintainers.  If there are two or fewer
recipients in your message, you must also always Cc: the Linux kernel
security team who will ensure the message is delivered to the proper
people, and will be able to assist small maintainer teams with processes
they may not be familiar with.  For larger teams, Cc: the Linux kernel
security team for your first few reports or when seeking specific help,
such as when resending a message which got no response within a week.
Once you have become comfortable with the process for a few reports, it is
no longer necessary to Cc: the security list when sending to large teams.
The Linux kernel security team can be contacted by email at
<security@kernel.org>.  This is a private list of security officers
who will help verify the bug report and assist developers working on a fix.
It is possible that the security team will bring in extra help from area
maintainers to understand and fix the security vulnerability.
(报告必须发给维护者。如果你消息中的收件人不超过两个，你还必须始终抄
送 Linux 内核安全团队，他们会确保消息送达正确的人，并能协助小型维护者
团队处理他们可能不熟悉的流程。对于较大的团队，在你最初的几次报告中或
在寻求特定帮助时抄送 Linux 内核安全团队，例如重发一条一周内没有得到回
应的消息时。一旦你通过几次报告熟悉了这个流程，向大团队发送时就不再需
要抄送安全列表。Linux 内核安全团队可以通过电子邮件 <security@kernel.org>
联系。这是一个由安全官员组成的私密列表，他们会帮助核实缺陷报告并协助
开发者进行修复。安全团队可能会引入相关领域的维护者提供额外帮助，以理
解并修复该安全漏洞。)

Please send **plain text** emails without attachments where possible.
It is much harder to have a context-quoted discussion about a complex
issue if all the details are hidden away in attachments.  Think of it like a
:doc:`regular patch submission <../process/submitting-patches>`
(even if you don't have a patch yet): describe the problem and impact, list
reproduction steps, and follow it with a proposed fix, all in plain text.
Markdown, HTML and RST formatted reports are particularly frowned upon since
they're quite hard to read for humans and encourage to use dedicated viewers,
sometimes online, which by definition is not acceptable for a confidential
security report. Note that some mailers tend to mangle formatting of plain
text by default, please consult Documentation/process/email-clients.rst for
more info.
(请尽可能发送 **纯文本** 邮件，不带附件。如果所有细节都藏在附件里，围
绕复杂问题进行上下文引用式讨论要困难得多。把它想象成一次 :doc:`regular patch submission <../process/submitting-patches>` (即使你还没有补丁)：描
述问题和影响，列出复现步骤，然后附上建议的修复，全部用纯文本。Markdown、
HTML 和 RST 格式的报告尤其不受欢迎，因为人们很难阅读它们，而且它们会促
使使用专用查看器 (有时是在线的)，按定义这对机密的安全报告是不可接受的。
注意，一些邮件客户端默认会破坏纯文本的格式，更多信息请查阅
Documentation/process/email-clients.rst。)

Disclosure and embargoed information(披露与受限信息)
----------------------------------------------------

The security list is not a disclosure channel.  For that, see Coordination
below.
(安全列表不是一个披露渠道。披露请参见下文"协调"。)

Once a robust fix has been developed, the release process starts.  Fixes
for publicly known bugs are released immediately.
(一旦开发出稳健的修复，发布流程即启动。对已公开缺陷的修复会立即发布。)

Although our preference is to release fixes for publicly undisclosed bugs
as soon as they become available, this may be postponed at the request of
the reporter or an affected party for up to 7 calendar days from the start
of the release process, with an exceptional extension to 14 calendar days
if it is agreed that the criticality of the bug requires more time.  The
only valid reason for deferring the publication of a fix is to accommodate
the logistics of QA and large scale rollouts which require release
coordination.
(虽然我们的偏好是在针对未公开缺陷的修复可用时立即发布，但可以应报告
者或受影响方的要求，从发布流程开始起最多推迟 7 个自然日；如果一致认为
该缺陷的严重性需要更多时间，可例外延长至 14 个自然日。推迟发布修复的
唯一有效理由是为了照顾 QA 和大规模部署的后勤需要，这些需要发布协调。)

While embargoed information may be shared with trusted individuals in
order to develop a fix, such information will not be published alongside
the fix or on any other disclosure channel without the permission of the
reporter.  This includes but is not limited to the original bug report
and followup discussions (if any), exploits, CVE information or the
identity of the reporter.
(虽然受限信息可以与受信任的个人共享以开发修复，但未经报告者许可，此
类信息不会随修复一起发布，也不会通过任何其他披露渠道发布。这包括但不
限于原始缺陷报告和后续讨论 (如果有)、漏洞利用、CVE 信息或报告者的身
份。)

In other words our only interest is in getting bugs fixed.  All other
information submitted to the security list and any followup discussions
of the report are treated confidentially even after the embargo has been
lifted, in perpetuity.
(换句话说，我们唯一的兴趣是把缺陷修复。提交给安全列表的所有其他信息
以及对报告的任何后续讨论都被保密对待，即使在受限披露解除之后也永远如
此。)

Coordination with other groups(与其他团队的协调)
------------------------------------------------

While the kernel security team solely focuses on getting bugs fixed,
other groups focus on fixing issues in distros and coordinating
disclosure between operating system vendors.  Coordination is usually
handled by the "linux-distros" mailing list and disclosure by the
public "oss-security" mailing list, both of which are closely related
and presented in the linux-distros wiki:
<https://oss-security.openwall.org/wiki/mailing-lists/distros>
(内核安全团队只专注于把缺陷修复，而其他团队专注于修复发行版中的问题
并协调操作系统厂商之间的披露。协调通常由 "linux-distros" 邮件列表处理，
披露则通过公开的 "oss-security" 邮件列表进行，两者密切相关，并在
linux-distros wiki 中介绍：
<https://oss-security.openwall.org/wiki/mailing-lists/distros>)

Please note that the respective policies and rules are different since
the 3 lists pursue different goals.  Coordinating between the kernel
security team and other teams is difficult since for the kernel security
team occasional embargoes (as subject to a maximum allowed number of
days) start from the availability of a fix, while for "linux-distros"
they start from the initial post to the list regardless of the
availability of a fix.
(请注意，由于这几个列表追求不同的目标，各自的政策和规则有所不同。内
核安全团队与其他团队之间的协调很困难，因为对内核安全团队而言，偶发的
受限披露期 (以允许的最大天数为准) 从修复可用之时起算，而对
"linux-distros" 而言，受限披露期从最初发帖到列表之时起算，与修复是否
可用无关。)

As such, the kernel security team strongly recommends that as a reporter
of a potential security issue you DO NOT contact the "linux-distros"
mailing list UNTIL a fix is accepted by the affected code's maintainers
and you have read the distros wiki page above and you fully understand
the requirements that contacting "linux-distros" will impose on you and
the kernel community.  This also means that in general it doesn't make
sense to Cc: both lists at once, except maybe for coordination if and
while an accepted fix has not yet been merged.  In other words, until a
fix is accepted do not Cc: "linux-distros", and after it's merged do not
Cc: the kernel security team.
(因此，内核安全团队强烈建议，作为潜在安全问题的报告者，在修复被受影
响代码的维护者接受之前，不要联系 "linux-distros" 邮件列表，并且要先阅
读上面的 distros wiki 页面、完全理解联系 "linux-distros" 将对你和内核
社区提出的要求。这也意味着一般来说同时抄送两个列表没有意义，或许除了
在已被接受的修复尚未合并时进行协调。换句话说，在修复被接受之前不要抄
送 "linux-distros"，在合并之后不要抄送内核安全团队。)

CVE assignment(CVE 分配)
------------------------

The security team does not assign CVEs, nor do we require them for
reports or fixes, as this can needlessly complicate the process and may
delay the bug handling.  If a reporter wishes to have a CVE identifier
assigned for a confirmed issue, they can contact the :doc:`kernel CVE
assignment team<../process/cve>` to obtain one.
(安全团队不分配 CVE，报告或修复也不要求 CVE，因为这会不必要地使流程
复杂化，并可能延误缺陷的处理。如果报告者希望为已确认的问题获得 CVE 标
识符，可以联系 :doc:`kernel CVE assignment team<../process/cve>` 获取。)

Non-disclosure agreements(保密协议)
-----------------------------------

The Linux kernel security team is not a formal body and therefore unable
to enter any non-disclosure agreements.
(Linux 内核安全团队不是一个正式机构，因此无法签署任何保密协议。)
