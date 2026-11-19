.. _submittingpatches:

Submitting patches: the essential guide to getting your code into the kernel(提交补丁：让你的代码进入内核的必读指南)
====================================================================================================================

For a person or company who wishes to submit a change to the Linux
kernel, the process can sometimes be daunting if you're not familiar
with "the system."  This text is a collection of suggestions which
can greatly increase the chances of your change being accepted.
(对于希望向 Linux 内核提交改动的人或公司来说，如果你不熟悉"这套体系"，这个过程有时会令人望而生畏。本文汇集了一些建议，可以大大提高你的改动被接受的机会。)

This document contains a large number of suggestions in a relatively terse
format.  For detailed information on how the kernel development process
works, see Documentation/process/development-process.rst. Also, read
Documentation/process/submit-checklist.rst
for a list of items to check before submitting code.
For device tree binding patches, read
Documentation/devicetree/bindings/submitting-patches.rst.
(本文以相对简练的形式给出了大量建议。关于内核开发流程的详细说明，见 Documentation/process/development-process.rst。另外，请阅读 Documentation/process/submit-checklist.rst，其中列出了提交代码前需要检查的事项。设备树绑定 (device tree binding) 补丁请阅读 Documentation/devicetree/bindings/submitting-patches.rst。)

This documentation assumes that you're using ``git`` to prepare your patches.
If you're unfamiliar with ``git``, you would be well-advised to learn how to
use it, it will make your life as a kernel developer and in general much
easier.
(本文假定你使用 ``git`` 来准备补丁。如果你对 ``git`` 不熟悉，强烈建议你学会使用它，无论是对你作为内核开发者的工作还是日常生活，它都会让事情容易得多。)

Some subsystems and maintainer trees have additional information about
their workflow and expectations, see
Documentation/process/maintainer-handbooks.rst.
(一些子系统和维护者代码树有关于其工作流程和要求的额外信息，见 Documentation/process/maintainer-handbooks.rst。)

Obtain a current source tree(获取最新的源码树)
----------------------------------------------

If you do not have a repository with the current kernel source handy, use
``git`` to obtain one.  You'll want to start with the mainline repository,
which can be grabbed with::

  git clone git://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git

(如果你手头没有一个包含当前内核源码的仓库，就用 ``git`` 获取一个。你应当从主线仓库开始，可以用以下命令获取：)

Note, however, that you may not want to develop against the mainline tree
directly.  Most subsystem maintainers run their own trees and want to see
patches prepared against those trees.  See the **T:** entry for the subsystem
in the MAINTAINERS file to find that tree, or simply ask the maintainer if
the tree is not listed there.
(不过要注意，你可能并不想直接基于主线树开发。大多数子系统维护者都有自己的代码树，并且希望看到针对这些树准备的补丁。要找到对应的树，可以查看 MAINTAINERS 文件中该子系统的 **T:** 条目；如果那里没有列出，直接询问维护者即可。)

.. _describe_changes:

Describe your changes(描述你的改动)
-----------------------------------

Describe your problem.  Whether your patch is a one-line bug fix or
5000 lines of a new feature, there must be an underlying problem that
motivated you to do this work.  Convince the reviewer that there is a
problem worth fixing and that it makes sense for them to read past the
first paragraph.
(描述你的问题。无论你的补丁是一行的 bug 修复还是 5000 行的新功能，都必须有一个促使你做这项工作的根本问题。要让评审者相信存在一个值得修复的问题，并且让他们觉得有必要继续读第一段之后的内容。)

Describe user-visible impact.  Straight up crashes and lockups are
pretty convincing, but not all bugs are that blatant.  Even if the
problem was spotted during code review, describe the impact you think
it can have on users.  Keep in mind that the majority of Linux
installations run kernels from secondary stable trees or
vendor/product-specific trees that cherry-pick only specific patches
from upstream, so include anything that could help route your change
downstream: provoking circumstances, excerpts from dmesg, crash
descriptions, performance regressions, latency spikes, lockups, etc.
(描述对用户可见的影响。直接的崩溃和死机很有说服力，但并非所有 bug 都这么明显。即使问题是在代码评审中发现的，也要描述你认为它可能对用户造成的影响。请记住，大多数 Linux 安装运行的内核来自二级稳定树或厂商/产品特定树，它们只从上游 cherry-pick 特定的补丁，因此要包含任何有助于把你的改动向下游传递的信息：触发环境、dmesg 摘录、崩溃描述、性能退化、延迟尖峰、死机现象等。)

Quantify optimizations and trade-offs.  If you claim improvements in
performance, memory consumption, stack footprint, or binary size,
include numbers that back them up.  But also describe non-obvious
costs.  Optimizations usually aren't free but trade-offs between CPU,
memory, and readability; or, when it comes to heuristics, between
different workloads.  Describe the expected downsides of your
optimization so that the reviewer can weigh costs against benefits.
(量化优化及其权衡。如果你声称在性能、内存消耗、栈占用或二进制体积上有改进，请给出支撑这些说法的数字。但同时也要描述那些不明显的代价。优化通常不是免费的，而是在 CPU、内存和可读性之间做权衡；对于启发式算法，则是在不同工作负载之间做权衡。请描述你的优化预期带来的缺点，让评审者能够权衡成本与收益。)

Once the problem is established, describe what you are actually doing
about it in technical detail.  It's important to describe the change
in plain English for the reviewer to verify that the code is behaving
as you intend it to.
(在问题确立之后，用技术细节描述你实际是如何处理它的。用平实的语言描述这个改动很重要，这样评审者才能验证代码的行为确实符合你的意图。)

The maintainer will thank you if you write your patch description in a
form which can be easily pulled into Linux's source code management
system, ``git``, as a "commit log".  See :ref:`the_canonical_patch_format`.
(如果你把补丁描述写成能方便地作为 "commit log" 拉入 Linux 源码管理系统 ``git`` 的形式，维护者会感谢你。参见 :ref:`the_canonical_patch_format`。)

Solve only one problem per patch.  If your description starts to get
long, that's a sign that you probably need to split up your patch.
See :ref:`split_changes`.
(每个补丁只解决一个问题。如果你的描述开始变长，那就是你可能需要拆分补丁的信号。参见 :ref:`split_changes`。)

When you submit or resubmit a patch or patch series, include the
complete patch description and justification for it.  Don't just
say that this is version N of the patch (series).  Don't expect the
subsystem maintainer to refer back to earlier patch versions or referenced
URLs to find the patch description and put that into the patch.
I.e., the patch (series) and its description should be self-contained.
This benefits both the maintainers and reviewers.  Some reviewers
probably didn't even receive earlier versions of the patch.
(提交或重新提交补丁或补丁系列时，要包含完整的补丁描述及其理由。不要只说这是第 N 版补丁(系列)。不要指望子系统维护者会翻回早期补丁版本或引用的 URL 去找补丁描述并把它放进补丁。也就是说，补丁(系列)及其描述应当自包含。这对维护者和评审者都有好处——有些评审者甚至可能根本没收到过早期版本的补丁。)

Describe your changes in imperative mood, e.g. "make xyzzy do frotz"
instead of "[This patch] makes xyzzy do frotz" or "[I] changed xyzzy
to do frotz", as if you are giving orders to the codebase to change
its behaviour.
(用祈使语气描述你的改动，例如 "make xyzzy do frotz"，而不是 "[This patch] makes xyzzy do frotz" 或 "[I] changed xyzzy to do frotz"，就好像你在命令代码库改变它的行为。)

If you want to refer to a specific commit, don't just refer to the
SHA-1 ID of the commit. Please also include the oneline summary of
the commit, to make it easier for reviewers to know what it is about.
Example::

	Commit e21d2170f36602ae2708 ("video: remove unnecessary
	platform_set_drvdata()") removed the unnecessary
	platform_set_drvdata(), but left the variable "dev" unused,
	delete it.

(如果你想引用某个特定的 commit，不要只给出该 commit 的 SHA-1 ID，还请附上该 commit 的单行摘要，让评审者更容易知道它是做什么的。示例：)

You should also be sure to use at least the first twelve characters of the
SHA-1 ID.  The kernel repository holds a *lot* of objects, making
collisions with shorter IDs a real possibility.  Bear in mind that, even if
there is no collision with your six-character ID now, that condition may
change five years from now.
(你还应确保至少使用 SHA-1 ID 的前 12 个字符。内核仓库保存着 *大量* 的对象，较短的 ID 发生冲突的可能性是真实存在的。记住，即使你的六字符 ID 现在没有冲突，五年之后情况也可能改变。)

If related discussions or any other background information behind the change
can be found on the web, add 'Link:' tags pointing to it. If the patch is a
result of some earlier mailing list discussions or something documented on the
web, point to it.
(如果能在网上找到与该改动相关的讨论或其他背景信息，请加上指向它们的 'Link:' 标签。如果补丁源于早先的邮件列表讨论或网上已有的文档，请指出来。)

When linking to mailing list archives, preferably use the lore.kernel.org
message archiver service. To create the link URL, use the contents of the
``Message-ID`` header of the message without the surrounding angle brackets.
For example::

    Link: https://lore.kernel.org/30th.anniversary.repost@klaava.Helsinki.FI

(链接邮件列表归档时，最好使用 lore.kernel.org 邮件归档服务。要构造链接 URL，请使用消息 ``Message-ID`` 头的内容并去掉两侧尖括号。例如：)

Please check the link to make sure that it is actually working and points
to the relevant message.
(请检查该链接，确保它确实有效并指向相关消息。)

However, try to make your explanation understandable without external
resources. In addition to giving a URL to a mailing list archive or bug,
summarize the relevant points of the discussion that led to the
patch as submitted.
(不过，要尽量让你的解释在不借助外部资源的情况下也能看懂。除了给出邮件列表归档或 bug 的 URL 之外，还应总结促成这个补丁的讨论中的相关要点。)

In case your patch fixes a bug, use the 'Closes:' tag with a URL referencing
the report in the mailing list archives or a public bug tracker. For example::

	Closes: https://example.com/issues/1234

(如果你的补丁修复了一个 bug，请使用 'Closes:' 标签并附上指向邮件列表归档或公开 bug 跟踪器中该报告的 URL。例如：)

Some bug trackers have the ability to close issues automatically when a
commit with such a tag is applied. Some bots monitoring mailing lists can
also track such tags and take certain actions. Private bug trackers and
invalid URLs are forbidden.
(有些 bug 跟踪器在带有此类标签的 commit 被应用时能够自动关闭 issue。一些监控邮件列表的机器人也能跟踪此类标签并采取特定行动。禁止使用私有 bug 跟踪器和无效的 URL。)

If your patch fixes a bug in a specific commit, e.g. you found an issue using
``git bisect``, please use the 'Fixes:' tag with at least the first 12
characters of the SHA-1 ID, and the one line summary.  Do not split the tag
across multiple lines, tags are exempt from the "wrap at 75 columns" rule in
order to simplify parsing scripts.  For example::

	Fixes: 54a4f0239f2e ("KVM: MMU: make kvm_mmu_zap_page() return the number of pages it actually freed")

(如果你的补丁修复了某个特定 commit 中的 bug(例如你用 ``git bisect`` 发现了问题)，请使用 'Fixes:' 标签，附上至少 12 个字符的 SHA-1 ID 和单行摘要。不要把标签拆到多行——为了便于解析脚本，标签不受"按 75 列折行"规则的约束。例如：)

The following ``git config`` settings can be used to add a pretty format for
outputting the above style in the ``git log`` or ``git show`` commands::

	[core]
		abbrev = 12
	[pretty]
		fixes = Fixes: %h (\"%s\")

(下面的 ``git config`` 设置可以添加一个 pretty 格式，用于在 ``git log`` 或 ``git show`` 命令中输出上述样式：)

An example call::

	$ git log -1 --pretty=fixes 54a4f0239f2e
	Fixes: 54a4f0239f2e ("KVM: MMU: make kvm_mmu_zap_page() return the number of pages it actually freed")

(一个调用示例：)

.. _split_changes:

Separate your changes(拆分你的改动)
-----------------------------------

Separate each **logical change** into a separate patch.
(把每个 **逻辑改动** 拆成单独的补丁。)

For example, if your changes include both bug fixes and performance
enhancements for a single driver, separate those changes into two
or more patches.  If your changes include an API update, and a new
driver which uses that new API, separate those into two patches.
(例如，如果你的改动既包含某个驱动的 bug 修复，又包含它的性能增强，就把这些改动拆成两个或更多补丁。如果你的改动包含一次 API 更新以及一个使用该新 API 的新驱动，就把它们拆成两个补丁。)

On the other hand, if you make a single change to numerous files,
group those changes into a single patch.  Thus a single logical change
is contained within a single patch.
(反过来说，如果你对许多文件做了一个单一改动，就应把这些改动归入一个补丁。这样，一个逻辑改动就包含在一个补丁之内。)

The point to remember is that each patch should make an easily understood
change that can be verified by reviewers.  Each patch should be justifiable
on its own merits.
(要记住的要点是：每个补丁都应做出一个易于理解、能被评审者验证的改动。每个补丁都应能凭其自身价值站得住脚。)

If one patch depends on another patch in order for a change to be
complete, that is OK.  Simply note **"this patch depends on patch X"**
in your patch description.
(如果某个补丁必须依赖另一个补丁改动才算完整，那也没问题。只需在补丁描述中注明 **"this patch depends on patch X"** 即可。)

When dividing your change into a series of patches, take special care to
ensure that the kernel builds and runs properly after each patch in the
series.  Developers using ``git bisect`` to track down a problem can end up
splitting your patch series at any point; they will not thank you if you
introduce bugs in the middle.
(把你的改动划分为一系列补丁时，要特别注意确保系列中的每个补丁应用之后内核都能正常构建和运行。使用 ``git bisect`` 追查问题的开发者可能会在你的补丁系列的任意一点上切分；如果你在中途引入了 bug，他们是不会感谢你的。)

If you cannot condense your patch set into a smaller set of patches,
then only post say 15 or so at a time and wait for review and integration.
(如果你无法把补丁集压缩成更少的补丁，那么一次只张贴 15 个左右，然后等待评审与合入。)



Style-check your changes(对改动做风格检查)
------------------------------------------

Check your patch for basic style violations, details of which can be
found in Documentation/process/coding-style.rst.
Failure to do so simply wastes
the reviewers time and will get your patch rejected, probably
without even being read.
(检查你的补丁是否存在基本的风格违规，细节见 Documentation/process/coding-style.rst。不这样做纯粹是浪费评审者的时间，而且你的补丁会被拒绝——很可能连看都不看。)

One significant exception is when moving code from one file to
another -- in this case you should not modify the moved code at all in
the same patch which moves it.  This clearly delineates the act of
moving the code and your changes.  This greatly aids review of the
actual differences and allows tools to better track the history of
the code itself.
(一个重要的例外是把代码从一个文件移动到另一个文件时——在这种情况下，移动代码的那个补丁里完全不要修改被移动的代码。这样可以把"移动代码"这一动作与你的实际改动清晰地区分开，极大地方便对真正差异的评审，并让工具能够更好地跟踪代码本身的历史。)

Check your patches with the patch style checker prior to submission
(scripts/checkpatch.pl).  Note, though, that the style checker should be
viewed as a guide, not as a replacement for human judgment.  If your code
looks better with a violation then its probably best left alone.
(提交之前，先用补丁风格检查器 (scripts/checkpatch.pl) 检查你的补丁。不过要注意，风格检查器应被视为指南，而不能取代人的判断。如果带一点违规反而让你的代码看起来更好，那多半还是别动为妙。)

The checker reports at three levels:
(检查器按三个级别报告：)

 - ERROR: things that are very likely to be wrong
   (ERROR：很可能是错误的东西)
 - WARNING: things requiring careful review
   (WARNING：需要仔细评审的东西)
 - CHECK: things requiring thought
   (CHECK：需要思考的东西)

You should be able to justify all violations that remain in your
patch.
(你应当能够为补丁中残留的所有违规作出解释。)


Select the recipients for your patch(选择补丁的收件人)
------------------------------------------------------

You should always copy the appropriate subsystem maintainer(s) and list(s) on
any patch to code that they maintain; look through the MAINTAINERS file and the
source code revision history to see who those maintainers are.  The script
scripts/get_maintainer.pl can be very useful at this step (pass paths to your
patches as arguments to scripts/get_maintainer.pl).  If you cannot find a
maintainer for the subsystem you are working on, Andrew Morton
(akpm@linux-foundation.org) serves as a maintainer of last resort.
(对于任何触及他们所维护代码的补丁，你都应始终抄送给相应的子系统维护者和邮件列表；通过翻阅 MAINTAINERS 文件和源码修改历史来找出这些维护者是谁。脚本 scripts/get_maintainer.pl 在这一步非常有用(把你的补丁路径作为参数传给 scripts/get_maintainer.pl)。如果你找不到你所处理子系统的维护者，Andrew Morton (akpm@linux-foundation.org) 是最后一道防线的维护者。)

linux-kernel@vger.kernel.org should be used by default for all patches, but the
volume on that list has caused a number of developers to tune it out.  Please
do not spam unrelated lists and unrelated people, though.
(所有补丁默认都应抄送 linux-kernel@vger.kernel.org，但该列表的邮件量已让不少开发者把它屏蔽了。尽管如此，请不要向无关的列表和无关的人滥发邮件。)

Many kernel-related lists are hosted at kernel.org; you can find a list
of them at https://subspace.kernel.org.  There are kernel-related lists
hosted elsewhere as well, though.
(许多与内核相关的列表托管在 kernel.org 上，完整清单见 https://subspace.kernel.org。不过，也有一些与内核相关的列表托管在别处。)

Linus Torvalds is the final arbiter of all changes accepted into the
Linux kernel.  His e-mail address is <torvalds@linux-foundation.org>.
He gets a lot of e-mail, and, at this point, very few patches go through
Linus directly, so typically you should do your best to -avoid-
sending him e-mail.
(Linus Torvalds 是所有进入 Linux 内核的改动的最终裁决者。他的电子邮箱是 <torvalds@linux-foundation.org>。他收到的邮件非常多，而且如今很少有补丁直接经过 Linus，所以通常你应尽力 -避免- 给他发邮件。)

If you have a patch that fixes an exploitable security bug, send that patch
to security@kernel.org.  For severe bugs, a short embargo may be considered
to allow distributors to get the patch out to users; in such cases,
obviously, the patch should not be sent to any public lists. See also
Documentation/process/security-bugs.rst.
(如果你的补丁修复了可被利用的安全 bug，请把补丁发送到 security@kernel.org。对于严重 bug，可以考虑短时间的禁运 (embargo)，以便发行版厂商能把补丁送到用户手中；显然，这种情况下补丁不应发送到任何公开列表。另见 Documentation/process/security-bugs.rst。)

Patches that fix a severe bug in a released kernel should be directed
toward the stable maintainers by putting a line like this::

  Cc: stable@vger.kernel.org

into the sign-off area of your patch (note, NOT an email recipient).  You
should also read Documentation/process/stable-kernel-rules.rst
in addition to this document.
(修复已发布内核中严重 bug 的补丁，应在补丁的签收区加上上面这样一行，以送达稳定版维护者(注意，这并不是一个邮件收件人)。除本文之外，你还应阅读 Documentation/process/stable-kernel-rules.rst。)

If changes affect userland-kernel interfaces, please send the MAN-PAGES
maintainer (as listed in the MAINTAINERS file) a man-pages patch, or at
least a notification of the change, so that some information makes its way
into the manual pages.  User-space API changes should also be copied to
linux-api@vger.kernel.org.
(如果改动影响了用户态与内核之间的接口，请向 MAN-PAGES 维护者(MAINTAINERS 文件中所列)发送 man-pages 补丁，或至少通知该改动，让一些信息能进入手册页。用户空间 API 的改动还应抄送给 linux-api@vger.kernel.org。)


No MIME, no links, no compression, no attachments.  Just plain text(不用 MIME、不用链接、不压缩、不带附件。只发纯文本)
----------------------------------------------------------------------------------------------------------------------

Linus and other kernel developers need to be able to read and comment
on the changes you are submitting.  It is important for a kernel
developer to be able to "quote" your changes, using standard e-mail
tools, so that they may comment on specific portions of your code.
(Linus 和其他内核开发者需要能够阅读并评论你提交的改动。内核开发者能够用标准电子邮件工具"引用"你的改动非常重要，这样他们才能针对你代码的特定部分发表意见。)

For this reason, all patches should be submitted by e-mail "inline". The
easiest way to do this is with ``git send-email``, which is strongly
recommended.  An interactive tutorial for ``git send-email`` is available at
https://git-send-email.io.
(因此，所有补丁都应以"内联 (inline)"方式通过电子邮件提交。最简单的办法是使用 ``git send-email``，强烈推荐。``git send-email`` 的交互式教程见 https://git-send-email.io。)

If you choose not to use ``git send-email``:
(如果你选择不使用 ``git send-email``：)

.. warning::

  Be wary of your editor's word-wrap corrupting your patch,
  if you choose to cut-n-paste your patch.
  (如果你选择复制粘贴补丁，要当心编辑器的自动换行损坏你的补丁。)

Do not attach the patch as a MIME attachment, compressed or not.
Many popular e-mail applications will not always transmit a MIME
attachment as plain text, making it impossible to comment on your
code.  A MIME attachment also takes Linus a bit more time to process,
decreasing the likelihood of your MIME-attached change being accepted.
(不要把补丁作为 MIME 附件(无论是否压缩)提交。许多流行的邮件应用不一定总是把 MIME 附件当作纯文本传输，这使得别人无法评论你的代码。MIME 附件也会让 Linus 多花一点时间处理，降低你的 MIME 附件改动被接受的可能性。)

Exception:  If your mailer is mangling patches then someone may ask
you to re-send them using MIME.
(例外：如果你的邮件程序总是弄坏补丁，可能会有人要求你用 MIME 重新发送。)

See Documentation/process/email-clients.rst for hints about configuring
your e-mail client so that it sends your patches untouched.
(关于如何配置邮件客户端使其原封不动地发送补丁，见 Documentation/process/email-clients.rst。)

Respond to review comments(回应评审意见)
----------------------------------------

Your patch will almost certainly get comments from reviewers on ways in
which the patch can be improved, in the form of a reply to your email. You must
respond to those comments; ignoring reviewers is a good way to get ignored in
return. You can simply reply to their emails to answer their comments. Review
comments or questions that do not lead to a code change should almost certainly
bring about a comment or changelog entry so that the next reviewer better
understands what is going on.
(你的补丁几乎必然会收到评审者以邮件回复形式给出的改进建议。你必须回应这些意见；无视评审者是换来被无视的好办法。你可以直接回复他们的邮件来回应意见。那些没有导致代码修改的评审意见或问题，几乎肯定应当带来一条注释或 changelog 条目，让下一位评审者更好地理解发生了什么。)

Be sure to tell the reviewers what changes you are making and to thank them
for their time.  Code review is a tiring and time-consuming process, and
reviewers sometimes get grumpy.  Even in that case, though, respond
politely and address the problems they have pointed out.  When sending a next
version, add a ``patch changelog`` to the cover letter or to individual patches
explaining difference against previous submission (see
:ref:`the_canonical_patch_format`).
Notify people that commented on your patch about new versions by adding them to
the patches CC list.
(务必告诉评审者你做了哪些改动，并感谢他们付出的时间。代码评审是个累人又耗时的过程，评审者有时会发脾气。即便如此，也要礼貌地回应并处理他们指出的问题。发送下一版本时，在封面信 (cover letter) 或各个补丁中加上 ``patch changelog``，说明与上次提交的差异(参见 :ref:`the_canonical_patch_format`)。通过把评论过你补丁的人加入 CC 列表，让他们知晓新版本。)

See Documentation/process/email-clients.rst for recommendations on email
clients and mailing list etiquette.
(关于邮件客户端与邮件列表礼仪的建议，见 Documentation/process/email-clients.rst。)

.. _interleaved_replies:

Use trimmed interleaved replies in email discussions(在邮件讨论中使用裁剪后的交错回复)
--------------------------------------------------------------------------------------
Top-posting is strongly discouraged in Linux kernel development
discussions. Interleaved (or "inline") replies make conversations much
easier to follow. For more details see:
https://en.wikipedia.org/wiki/Posting_style#Interleaved_style
(Linux 内核开发的讨论中强烈不建议顶部回复 (top-posting)。交错式(或称"内联式")回复让对话更容易跟进。更多细节见：https://en.wikipedia.org/wiki/Posting_style#Interleaved_style)

As is frequently quoted on the mailing list::

  A: http://en.wikipedia.org/wiki/Top_post
  Q: Where do I find info about this thing called top-posting?
  A: Because it messes up the order in which people normally read text.
  Q: Why is top-posting such a bad thing?
  A: Top-posting.
  Q: What is the most annoying thing in e-mail?

(邮件列表上经常引用下面这段问答：)

Similarly, please trim all unneeded quotations that aren't relevant
to your reply. This makes responses easier to find, and saves time and
space. For more details see: http://daringfireball.net/2007/07/on_top ::

  A: No.
  Q: Should I include quotations after my reply?

(类似地，请裁剪掉所有与你的回复无关的多余引用。这能让回复更容易被找到，也节省时间和空间。更多细节见：http://daringfireball.net/2007/07/on_top。)

.. _resend_reminders:

Don't get discouraged - or impatient(不要气馁——也不要急躁)
----------------------------------------------------------

After you have submitted your change, be patient and wait.  Reviewers are
busy people and may not get to your patch right away.
(提交你的改动之后，请耐心等待。评审者都是大忙人，可能不会立刻处理你的补丁。)

Once upon a time, patches used to disappear into the void without comment,
but the development process works more smoothly than that now.  You should
receive comments within a few weeks (typically 2-3); if that does not
happen, make sure that you have sent your patches to the right place.
Wait for a minimum of one week before resubmitting or pinging reviewers
- possibly longer during busy times like merge windows.
(曾几何时，补丁常常石沉大海、无人回应，但如今开发流程已经顺畅多了。你应当会在几周内(通常 2-3 周)收到意见；如果没有，请确认你把补丁发送到了正确的地方。重新提交或催问评审者之前至少等待一周——在合并窗口 (merge window) 这样的繁忙时期可能要更久。)

It's also ok to resend the patch or the patch series after a couple of
weeks with the word "RESEND" added to the subject line::

   [PATCH Vx RESEND] sub/sys: Condensed patch summary

(几周之后，也可以在主题行加上 "RESEND" 字样重发补丁或补丁系列：)

Don't add "RESEND" when you are submitting a modified version of your
patch or patch series - "RESEND" only applies to resubmission of a
patch or patch series which have not been modified in any way from the
previous submission.
(提交修改过的补丁或补丁系列时不要加 "RESEND"——"RESEND" 只适用于与上次提交相比没有任何修改的补丁(系列)的重新提交。)


Include PATCH in the subject(在主题中包含 PATCH)
------------------------------------------------

Due to high e-mail traffic to Linus, and to linux-kernel, it is common
convention to prefix your subject line with [PATCH].  This lets Linus
and other kernel developers more easily distinguish patches from other
e-mail discussions.
(由于发往 Linus 和 linux-kernel 的邮件流量很大，惯常做法是在主题行前加上 [PATCH] 前缀。这能让 Linus 和其他内核开发者更容易把补丁与其他邮件讨论区分开。)

``git send-email`` will do this for you automatically.
(``git send-email`` 会自动帮你完成这件事。)


Sign your work - the Developer's Certificate of Origin(为你的工作签字——开发者来源证明 (DCO))
----------------------------------------------------------------------------------------------

To improve tracking of who did what, especially with patches that can
percolate to their final resting place in the kernel through several
layers of maintainers, we've introduced a "sign-off" procedure on
patches that are being emailed around.
(为了更好地追踪谁做了什么——尤其是那些要经过多层维护者层层传递、才能最终落入内核中安身之所的补丁——我们对通过邮件流转的补丁引入了"签收 (sign-off)"程序。)

The sign-off is a simple line at the end of the explanation for the
patch, which certifies that you wrote it or otherwise have the right to
pass it on as an open-source patch.  The rules are pretty simple: if you
can certify the below:
(签收是补丁说明末尾的一行简单声明，证明该补丁是你写的，或者你有权把它作为开源补丁传递下去。规则非常简单：如果你能证明以下内容：)

Developer's Certificate of Origin 1.1(开发者来源证明 1.1)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By making a contribution to this project, I certify that:
(通过向本项目作出贡献，本人证明：)

        (a) The contribution was created in whole or in part by me and I
            have the right to submit it under the open source license
            indicated in the file; or
            ((a) 该贡献全部或部分由本人创作，本人有权按照文件中所示的开源许可证提交它；或)

        (b) The contribution is based upon previous work that, to the best
            of my knowledge, is covered under an appropriate open source
            license and I have the right under that license to submit that
            work with modifications, whether created in whole or in part
            by me, under the same open source license (unless I am
            permitted to submit under a different license), as indicated
            in the file; or
            ((b) 该贡献基于在先作品，据本人所知，该在先作品受某个适当的开源许可证保护，且本人有权依该许可证提交经修改的该作品——无论其全部或部分是否由本人创作——并按文件中所示以同一开源许可证提交(除非本人被允许以不同许可证提交)；或)

        (c) The contribution was provided directly to me by some other
            person who certified (a), (b) or (c) and I have not modified
            it.
            ((c) 该贡献由已作出 (a)、(b) 或 (c) 声明的其他人直接提供给本人，且本人未对其作出修改。)

        (d) I understand and agree that this project and the contribution
            are public and that a record of the contribution (including all
            personal information I submit with it, including my sign-off) is
            maintained indefinitely and may be redistributed consistent with
            this project or the open source license(s) involved.
            ((d) 本人理解并同意：本项目与该贡献是公开的，该贡献的记录(包括本人随之提交的全部个人信息，包括本人的签收)将被无限期保存，并可与本项目或所涉开源许可证相一致地再分发。)

then you just add a line saying::

	Signed-off-by: Random J Developer <random@developer.example.org>

(那么你只需加上这样一行：)

using a known identity (sorry, no anonymous contributions.)
This will be done for you automatically if you use ``git commit -s``.
Reverts should also include "Signed-off-by". ``git revert -s`` does that
for you.
(并使用一个已知身份(抱歉，不接受匿名贡献)。如果你使用 ``git commit -s``，这一步会自动完成。回退 (revert) 提交也应包含 "Signed-off-by"，``git revert -s`` 会帮你完成。)

Some people also put extra tags at the end.  They'll just be ignored for
now, but you can do this to mark internal company procedures or just
point out some special detail about the sign-off.
(有些人还会在末尾加上额外的标签。它们目前只会被忽略，但你可以借此标记公司内部流程，或者指出签收的某个特殊细节。)

Any further SoBs (Signed-off-by:'s) following the author's SoB are from
people handling and transporting the patch, but were not involved in its
development. SoB chains should reflect the **real** route a patch took
as it was propagated to the maintainers and ultimately to Linus, with
the first SoB entry signalling primary authorship of a single author.
(作者 SoB 之后的任何后续 SoB(Signed-off-by:)都来自处理和传递该补丁的人，他们并未参与其开发。SoB 链应反映补丁传播到各维护者并最终到达 Linus 的 **真实** 路径，其中第一条 SoB 条目标志着该补丁的主要作者身份属于单一作者。)


When to use Acked-by:, Cc:, and Co-developed-by:(何时使用 Acked-by:、Cc: 和 Co-developed-by:)
---------------------------------------------------------------------------------------------

The Signed-off-by: tag indicates that the signer was involved in the
development of the patch, or that he/she was in the patch's delivery path.
(Signed-off-by: 标签表示签收者参与了补丁的开发，或者处于补丁的传递路径上。)

If a person was not directly involved in the preparation or handling of a
patch but wishes to signify and record their approval of it then they can
ask to have an Acked-by: line added to the patch's changelog.
(如果某人并未直接参与补丁的准备或处理，但希望表明并记录其对补丁的认可，那么他可以要求在补丁的 changelog 中加入一行 Acked-by:。)

Acked-by: is meant to be used by those responsible for or involved with the
affected code in one way or another.  Most commonly, the maintainer when that
maintainer neither contributed to nor forwarded the patch.
(Acked-by: 供那些以某种方式对受影响代码负责或参与其中的人使用。最常见的情况是：维护者本人既未为该补丁贡献代码、也未转发该补丁。)

Acked-by: may also be used by other stakeholders, such as people with domain
knowledge (e.g. the original author of the code being modified), userspace-side
reviewers for a kernel uAPI patch or key users of a feature.  Optionally, in
these cases, it can be useful to add a "# Suffix" to clarify its meaning::

	Acked-by: The Stakeholder <stakeholder@example.org> # As primary user

(Acked-by: 也可以由其他利益相关者使用，例如具有领域知识的人(如被修改代码的原作者)、内核 uAPI 补丁的用户态侧评审者，或某项功能的关键用户。在这些情况下，可以选择性地添加一个 "# Suffix" 后缀来阐明其含义：)

Acked-by: is not as formal as Signed-off-by:.  It is a record that the acker
has at least reviewed the patch and has indicated acceptance.  Hence patch
mergers will sometimes manually convert an acker's "yep, looks good to me"
into an Acked-by: (but note that it is usually better to ask for an
explicit ack).
(Acked-by: 不如 Signed-off-by: 正式。它是确认者 (acker) 至少评审过补丁并表明接受的记录。因此，合入补丁的人有时会手动把确认者一句"嗯，我觉得没问题"转换成 Acked-by:(不过注意，通常更好的做法是请求明确的 ack)。)

Acked-by: is also less formal than Reviewed-by:.  For instance, maintainers may
use it to signify that they are OK with a patch landing, but they may not have
reviewed it as thoroughly as if a Reviewed-by: was provided.  Similarly, a key
user may not have carried out a technical review of the patch, yet they may be
satisfied with the general approach, the feature or the user-facing interface.
(Acked-by: 也比 Reviewed-by: 更不正式。例如，维护者可以用它表示接受该补丁合入，但未必像提供 Reviewed-by: 那样对其做过透彻评审。类似地，关键用户可能没有对补丁做技术评审，但对总体方案、功能或面向用户的接口感到满意。)

Acked-by: does not necessarily indicate acknowledgement of the entire patch.
For example, if a patch affects multiple subsystems and has an Acked-by: from
one subsystem maintainer then this usually indicates acknowledgement of just
the part which affects that maintainer's code.  Judgement should be used here.
When in doubt people should refer to the original discussion in the mailing
list archives.  A "# Suffix" may also be used in this case to clarify.
(Acked-by: 不一定表示对整个补丁的认可。例如，如果补丁影响多个子系统，而其中一个子系统的维护者给出了 Acked-by:，这通常只表示认可影响该维护者代码的那部分。这里需要善加判断。有疑问时应查阅邮件列表归档中的原始讨论。这种情况也可以使用 "# Suffix" 来澄清。)

If a person has had the opportunity to comment on a patch, but has not
provided such comments, you may optionally add a ``Cc:`` tag to the patch.
This tag documents that potentially interested parties have been included in
the discussion. Note, this is one of only three tags you might be able to use
without explicit permission of the person named (see 'Tagging people requires
permission' below for details).
(如果某人本有机会对补丁发表意见但没有提供，你可以选择性地在补丁中加入 ``Cc:`` 标签。该标签记录了可能感兴趣的相关方已被纳入讨论。注意，这是仅有的三个可以不经被点名者明确许可而使用的标签之一(详见下文 'Tagging people requires permission')。)

Co-developed-by: states that the patch was co-created by multiple developers;
it is used to give attribution to co-authors (in addition to the author
attributed by the From: tag) when several people work on a single patch.  Since
Co-developed-by: denotes authorship, every Co-developed-by: must be immediately
followed by a Signed-off-by: of the associated co-author.  Standard sign-off
procedure applies, i.e. the ordering of Signed-off-by: tags should reflect the
chronological history of the patch insofar as possible, regardless of whether
the author is attributed via From: or Co-developed-by:.  Notably, the last
Signed-off-by: must always be that of the developer submitting the patch.
(Co-developed-by: 声明该补丁由多位开发者共同创建；当多人共同完成一个补丁时，用它来为共同作者署名(除了由 From: 标签署名的作者之外)。由于 Co-developed-by: 表示作者身份，每个 Co-developed-by: 后面都必须紧跟相应共同作者的 Signed-off-by:。标准的签收程序照常适用，即 Signed-off-by: 标签的顺序应尽可能反映补丁的时间先后历史，无论作者是通过 From: 还是 Co-developed-by: 署名。尤其要注意，最后一个 Signed-off-by: 必须始终是提交补丁的开发者的。)

Note, the From: tag is optional when the From: author is also the person (and
email) listed in the From: line of the email header.
(注意，当 From: 作者同时也是邮件头 From: 行中所列的人(及邮箱)时，From: 标签是可选的。)

Example of a patch submitted by the From: author::

	<changelog>

	Co-developed-by: First Co-Author <first@coauthor.example.org>
	Signed-off-by: First Co-Author <first@coauthor.example.org>
	Co-developed-by: Second Co-Author <second@coauthor.example.org>
	Signed-off-by: Second Co-Author <second@coauthor.example.org>
	Signed-off-by: From Author <from@author.example.org>

(由 From: 作者本人提交补丁的示例：)

Example of a patch submitted by a Co-developed-by: author::

	From: From Author <from@author.example.org>

	<changelog>

	Co-developed-by: Random Co-Author <random@coauthor.example.org>
	Signed-off-by: Random Co-Author <random@coauthor.example.org>
	Signed-off-by: From Author <from@author.example.org>
	Co-developed-by: Submitting Co-Author <sub@coauthor.example.org>
	Signed-off-by: Submitting Co-Author <sub@coauthor.example.org>


(由某位 Co-developed-by: 作者提交补丁的示例：)

Using Reported-by:, Tested-by:, Reviewed-by:, Suggested-by: and Fixes:(使用 Reported-by:、Tested-by:、Reviewed-by:、Suggested-by: 和 Fixes:)
--------------------------------------------------------------------------------------------------------------------------------------------

The Reported-by tag gives credit to people who find bugs and report them and it
hopefully inspires them to help us again in the future. The tag is intended for
bugs; please do not use it to credit feature requests. The tag should be
followed by a Closes: tag pointing to the report, unless the report is not
available on the web. The Link: tag can be used instead of Closes: if the patch
fixes a part of the issue(s) being reported. Note, the Reported-by tag is one
of only three tags you might be able to use without explicit permission of the
person named (see 'Tagging people requires permission' below for details).
(Reported-by 标签向发现并报告 bug 的人致谢，希望这能激励他们将来再次帮助我们。该标签用于 bug；请不要用它来为功能请求署名。该标签后面应跟着一个 Closes: 标签指向该报告，除非该报告在网上不可得。如果补丁只修复了所报告问题的一部分，可以用 Link: 标签代替 Closes:。注意，Reported-by 标签是仅有的三个可以不经被点名者明确许可而使用的标签之一(详见下文 'Tagging people requires permission')。)

A Tested-by: tag indicates that the patch has been successfully tested (in
some environment) by the person named.  This tag informs maintainers that
some testing has been performed, provides a means to locate testers for
future patches, and ensures credit for the testers.
(Tested-by: 标签表示该补丁已被被点名者在某种环境中成功测试。该标签告知维护者已进行过一些测试，为将来的补丁提供寻找测试者的途径，并确保测试者获得应有的署名。)

Reviewed-by:, instead, indicates that the patch has been reviewed and found
acceptable according to the Reviewer's Statement:
(而 Reviewed-by: 则表示补丁已经过评审，并按照评审者声明被认为是可接受的：)

Reviewer's statement of oversight(评审者的监督声明)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

By offering my Reviewed-by: tag, I state that:
(通过给出我的 Reviewed-by: 标签，本人声明：)

	 (a) I have carried out a technical review of this patch to
	     evaluate its appropriateness and readiness for inclusion into
	     the mainline kernel.

	 (b) Any problems, concerns, or questions relating to the patch
	     have been communicated back to the submitter.  I am satisfied
	     with the submitter's response to my comments.

	 (c) While there may be things that could be improved with this
	     submission, I believe that it is, at this time, (1) a
	     worthwhile modification to the kernel, and (2) free of known
	     issues which would argue against its inclusion.

	 (d) While I have reviewed the patch and believe it to be sound, I
	     do not (unless explicitly stated elsewhere) make any
	     warranties or guarantees that it will achieve its stated
	     purpose or function properly in any given situation.

((a) 本人已对该补丁进行了技术评审，评估其被纳入主线内核的适当性与就绪程度。(b) 与该补丁相关的任何问题、疑虑或疑问均已反馈给提交者，本人对提交者针对本人意见的回应感到满意。(c) 虽然本次提交可能还有可以改进之处，本人相信它此刻 (1) 是对内核的一项有价值的修改，并且 (2) 不存在反对将其纳入的已知问题。(d) 虽然本人已评审该补丁并认为它没有问题，但本人(除非在其他地方明确声明)不对它能在任何给定情况下达到其既定目的或正常运作作出任何担保或保证。)

A Reviewed-by tag is a statement of opinion that the patch is an
appropriate modification of the kernel without any remaining serious
technical issues.  Any interested reviewer (who has done the work) can
offer a Reviewed-by tag for a patch.  This tag serves to give credit to
reviewers and to inform maintainers of the degree of review which has been
done on the patch.  Reviewed-by: tags, when supplied by reviewers known to
understand the subject area and to perform thorough reviews, will normally
increase the likelihood of your patch getting into the kernel.
(Reviewed-by 标签是一种意见声明，表示该补丁是对内核的恰当修改，没有遗留任何严重的技术问题。任何有兴趣的评审者(做过这项工作的人)都可以为一个补丁提供 Reviewed-by 标签。该标签用于向评审者致谢，并告知维护者该补丁所经过的评审程度。当 Reviewed-by: 标签来自以了解该领域并做透彻评审而知名的评审者时，通常会提高你的补丁进入内核的可能性。)

Both Tested-by and Reviewed-by tags, once received on mailing list from tester
or reviewer, should be added by author to the applicable patches when sending
next versions.  However if the patch has changed substantially in following
version, these tags might not be applicable anymore and thus should be removed.
Usually removal of someone's Acked-by, Tested-by or Reviewed-by tags should be
mentioned in the patch changelog with an explanation (after the '---'
separator).
(Tested-by 和 Reviewed-by 标签一旦由测试者或评审者在邮件列表上给出，作者在发送下一版本时就应把它们加到相应的补丁上。不过，如果补丁在后续版本中发生了实质性变化，这些标签可能不再适用，因此应当移除。通常，移除某人的 Acked-by、Tested-by 或 Reviewed-by 标签应在补丁 changelog 中说明原因(在 '---' 分隔符之后)。)

A Suggested-by: tag indicates that the patch idea is suggested by the person
named and ensures credit to the person for the idea: if we diligently credit
our idea reporters, they will, hopefully, be inspired to help us again in the
future. Note, this is one of only three tags you might be able to use without
explicit permission of the person named (see 'Tagging people requires
permission' below for details).
(Suggested-by: 标签表示补丁的思路由被点名者提出，并确保该想法归功于此人：如果我们勤于为想法提出者署名，希望这能激励他们将来再次帮助我们。注意，这是仅有的三个可以不经被点名者明确许可而使用的标签之一(详见下文 'Tagging people requires permission')。)

A Fixes: tag indicates that the patch fixes a bug in a previous commit. It
is used to make it easy to determine where an issue originated, which can help
review a bug fix. This tag also assists the stable kernel team in determining
which stable kernel versions should receive your fix. This is the preferred
method for indicating a bug fixed by the patch. See :ref:`describe_changes`
for more details.
(Fixes: 标签表示该补丁修复了之前某个 commit 中的 bug。它用于让问题起源一目了然，有助于评审 bug 修复。该标签还能帮助稳定版内核团队决定哪些稳定版内核版本应当收到你的修复。这是标明补丁所修复 bug 的首选方式。更多细节参见 :ref:`describe_changes`。)

Note: Attaching a Fixes: tag does not subvert the stable kernel rules
process nor the requirement to Cc: stable@vger.kernel.org on all stable
patch candidates. For more information, please read
Documentation/process/stable-kernel-rules.rst.
(注意：附加 Fixes: 标签并不能绕过稳定版内核规则流程，也不能免除对所有稳定版候选补丁 Cc: stable@vger.kernel.org 的要求。更多信息请阅读 Documentation/process/stable-kernel-rules.rst。)

Finally, while providing tags is welcome and typically very appreciated, please
note that signers (i.e. submitters and maintainers) may use their discretion in
applying offered tags.
(最后，虽然提供标签是受欢迎的、通常也极受感激，但请注意，签收者(即提交者和维护者)可以自行决定是否采用所提供的标签。)

.. _tagging_people:

Tagging people requires permission(给人打标签需要获得许可)
----------------------------------------------------------

Be careful in the addition of the aforementioned tags to your patches, as all
except for Cc:, Reported-by:, and Suggested-by: need explicit permission of the
person named. For those three implicit permission is sufficient if the person
contributed to the Linux kernel using that name and email address according
to the lore archives or the commit history -- and in case of Reported-by:
and Suggested-by: did the reporting or suggestion in public. Note,
bugzilla.kernel.org is a public place in this sense, but email addresses
used there are private; so do not expose them in tags, unless the person
used them in earlier contributions.
(在补丁中加入上述标签时要小心，因为除 Cc:、Reported-by: 和 Suggested-by: 之外的所有标签都需要被点名者的明确许可。对这三个标签而言，如果根据 lore 归档或提交历史，该人曾以该姓名和电子邮箱为 Linux 内核做过贡献——且就 Reported-by: 和 Suggested-by: 而言，其报告或建议是在公开场合做出的——那么默示许可即已足够。注意，从这个意义上说 bugzilla.kernel.org 是公开场所，但那里使用的电子邮箱是私密的；因此除非该人在早先的贡献中使用过这些邮箱，否则不要把它们暴露在标签中。)

Using Assisted-by:(使用 Assisted-by:)
-------------------------------------

If you used any sort of advanced coding tool in the creation of your patch,
you need to acknowledge that use by adding an Assisted-by tag.  Failure to
do so may impede the acceptance of your work.  Please see
Documentation/process/coding-assistants.rst for details regarding the
acknowledgment of coding assistants.
(如果你在创建补丁的过程中使用了任何类型的高级编程工具，你需要通过添加 Assisted-by 标签来声明这一使用。不这样做可能会妨碍你的工作被接受。有关编程助手声明的细节，请参见 Documentation/process/coding-assistants.rst。)


.. _the_canonical_patch_format:

The canonical patch format(规范的补丁格式)
------------------------------------------

This section describes how the patch itself should be formatted.  Note
that, if you have your patches stored in a ``git`` repository, proper patch
formatting can be had with ``git format-patch``.  The tools cannot create
the necessary text, though, so read the instructions below anyway.
(本节描述补丁本身应当如何格式化。注意，如果你的补丁保存在 ``git`` 仓库中，可以用 ``git format-patch`` 得到规范的补丁格式。不过工具无法生成必要的文字，所以无论如何请阅读下面的说明。)

Subject Line(主题行)
^^^^^^^^^^^^^^^^^^^^

The canonical patch subject line is::

    Subject: [PATCH 001/123] subsystem: summary phrase

(规范的补丁主题行如下：)

The canonical patch message body contains the following:

  - A ``from`` line specifying the patch author, followed by an empty
    line (only needed if the person sending the patch is not the author).

  - The body of the explanation, line wrapped at 75 columns, which will
    be copied to the permanent changelog to describe this patch.

  - An empty line.

  - The ``Signed-off-by:`` lines, described above, which will
    also go in the changelog.

  - A marker line containing simply ``---``.

  - Any additional comments not suitable for the changelog.

  - The actual patch (``diff`` output).

(规范的补丁邮件正文包含以下内容：

  - 一行指明补丁作者的 ``from`` 行，后跟一个空行(仅当发送补丁的人不是作者时才需要)。

  - 说明正文，按 75 列折行，将被复制到永久 changelog 中描述这个补丁。

  - 一个空行。

  - ``Signed-off-by:`` 行(如前文所述)，它们同样会进入 changelog。

  - 一行仅包含 ``---`` 的标记行。

  - 任何不适合放入 changelog 的补充说明。

  - 补丁本体(``diff`` 输出)。)

The Subject line format makes it very easy to sort the emails
alphabetically by subject line - pretty much any email reader will
support that - since because the sequence number is zero-padded,
the numerical and alphabetic sort is the same.
(主题行的这种格式使得按主题行对邮件进行字母排序变得非常容易——几乎所有邮件阅读器都支持——因为序号是补零的，数值排序与字母排序结果相同。)

The ``subsystem`` in the email's Subject should identify which
area or subsystem of the kernel is being patched.
(邮件主题中的 ``subsystem`` 应当标明补丁所修改的是内核的哪个区域或子系统。)

The ``summary phrase`` in the email's Subject should concisely
describe the patch which that email contains.  The ``summary
phrase`` should not be a filename.  Do not use the same ``summary
phrase`` for every patch in a whole patch series (where a ``patch
series`` is an ordered sequence of multiple, related patches).
(邮件主题中的 ``summary phrase`` 应当简明地描述该邮件所包含的补丁。``summary phrase`` 不应是文件名。整个补丁系列(``patch series`` 指多个相关补丁的有序序列)中的每个补丁都不要使用相同的 ``summary phrase``。)

Bear in mind that the ``summary phrase`` of your email becomes a
globally-unique identifier for that patch.  It propagates all the way
into the ``git`` changelog.  The ``summary phrase`` may later be used in
developer discussions which refer to the patch.  People will want to
google for the ``summary phrase`` to read discussion regarding that
patch.  It will also be the only thing that people may quickly see
when, two or three months later, they are going through perhaps
thousands of patches using tools such as ``gitk`` or ``git log
--oneline``.
(请记住，你邮件的 ``summary phrase`` 会成为该补丁的全局唯一标识符。它一路传播到 ``git`` changelog 中。``summary phrase`` 之后可能会在开发者讨论该补丁时被引用。人们会用 ``summary phrase`` 去 google 搜索关于该补丁的讨论。两三个月后，当人们用 ``gitk`` 或 ``git log --oneline`` 之类的工具翻阅成千上万条补丁时，它也将是人们能快速看到的唯一内容。)

For these reasons, the ``summary`` must be no more than 70-75
characters, and it must describe both what the patch changes, as well
as why the patch might be necessary.  It is challenging to be both
succinct and descriptive, but that is what a well-written summary
should do.
(出于这些原因，``summary`` 不得超过 70-75 个字符，并且它既要描述补丁改了什么，也要说明补丁为何可能是必要的。既要简洁又有描述性确实很难，但这正是一条写得好的 summary 应该做到的。)

The ``summary phrase`` may be prefixed by tags enclosed in square
brackets: "Subject: [PATCH <tag>...] <summary phrase>".  The tags are
not considered part of the summary phrase, but describe how the patch
should be treated.  Common tags might include a version descriptor if
the multiple versions of the patch have been sent out in response to
comments (i.e., "v1, v2, v3"), or "RFC" to indicate a request for
comments.
(``summary phrase`` 前面可以加上用方括号括起来的标签： "Subject: [PATCH <tag>...] <summary phrase>" 。这些标签不算 summary phrase 的一部分，而是描述该补丁应当如何对待。常见的标签包括：当补丁因评审意见而发出多个版本时使用的版本描述符(即 "v1, v2, v3")，以及表示征求意见 (request for comments) 的 "RFC"。)

If there are four patches in a patch series the individual patches may
be numbered like this: 1/4, 2/4, 3/4, 4/4. This assures that developers
understand the order in which the patches should be applied and that
they have reviewed or applied all of the patches in the patch series.
(如果一个补丁系列中有四个补丁，各个补丁可以按 1/4、2/4、3/4、4/4 这样的方式编号。这能让开发者明白补丁应当应用的顺序，并确认他们已评审或应用了系列中的全部补丁。)

Here are some good example Subjects::

    Subject: [PATCH 2/5] ext2: improve scalability of bitmap searching
    Subject: [PATCH v2 01/27] x86: fix eflags tracking
    Subject: [PATCH v2] sub/sys: Condensed patch summary
    Subject: [PATCH v2 M/N] sub/sys: Condensed patch summary

(下面是一些好的主题行示例：)

From Line(From 行)
^^^^^^^^^^^^^^^^^^

The ``from`` line must be the very first line in the message body,
and has the form:
(``from`` 行必须是邮件正文的第一行，形式如下：)

        From: Patch Author <author@example.com>

The ``from`` line specifies who will be credited as the author of the
patch in the permanent changelog.  If the ``from`` line is missing,
then the ``From:`` line from the email header will be used to determine
the patch author in the changelog.
(``from`` 行指定在永久 changelog 中谁将被署名为补丁作者。如果缺少 ``from`` 行，则使用邮件头中的 ``From:`` 行来确定 changelog 中的补丁作者。)

The author may indicate their affiliation or the sponsor of the work
by adding the name of an organization to the ``from`` and ``SoB`` lines,
e.g.:
(作者可以在 ``from`` 和 ``SoB`` 行中加上组织名称，以表明其所属机构或工作的赞助方，例如：)

	From: Patch Author (Company) <author@example.com>

Explanation Body(说明正文)
^^^^^^^^^^^^^^^^^^^^^^^^^^

The explanation body will be committed to the permanent source
changelog, so should make sense to a competent reader who has long since
forgotten the immediate details of the discussion that might have led to
this patch. Including symptoms of the failure which the patch addresses
(kernel log messages, oops messages, etc.) are especially useful for
people who might be searching the commit logs looking for the applicable
patch. The text should be written in such detail so that when read
weeks, months or even years later, it can give the reader the needed
details to grasp the reasoning for **why** the patch was created.
(说明正文会提交到永久的源码 changelog 中，因此应当让一位称职的读者看得懂——哪怕他早已忘记可能促成该补丁的那场讨论的细节。写明补丁所解决问题的故障现象(内核日志消息、oops 消息等)对那些搜索提交日志寻找相应补丁的人尤其有用。文字应写得足够详细，以便在数周、数月甚至数年后阅读时，仍能给读者提供必要的细节，让他们理解该补丁 **为何** 而生。)

If a patch fixes a compile failure, it may not be necessary to include
_all_ of the compile failures; just enough that it is likely that
someone searching for the patch can find it. As in the ``summary
phrase``, it is important to be both succinct as well as descriptive.
(如果补丁修复的是一个编译失败，可能不必罗列 _全部_ 编译失败信息；只要足以让搜索该补丁的人能找到即可。与 ``summary phrase`` 一样，既简洁又有描述性非常重要。)

.. _backtraces:

Backtraces in commit messages(提交信息中的回溯)
"""""""""""""""""""""""""""""""""""""""""""""""

Backtraces help document the call chain leading to a problem. However,
not all backtraces are helpful. For example, early boot call chains are
unique and obvious. Copying the full dmesg output verbatim, however,
adds distracting information like timestamps, module lists, register and
stack dumps.
(回溯 (backtrace) 有助于记录导致问题的调用链。但并非所有回溯都有帮助。例如，启动早期的调用链独特而明显。而原样复制完整的 dmesg 输出则会加入时间戳、模块列表、寄存器和栈转储等让人分心的信息。)

Therefore, the most useful backtraces should distill the relevant
information from the dump, which makes it easier to focus on the real
issue. Here is an example of a well-trimmed backtrace::

  unchecked MSR access error: WRMSR to 0xd51 (tried to write 0x0000000000000064)
  at rIP: 0xffffffffae059994 (native_write_msr+0x4/0x20)
  Call Trace:
  mba_wrmsr
  update_domains
  rdtgroup_mkdir

(因此，最有用的回溯应当从转储中提炼出相关信息，使人更容易聚焦真正的问题。下面是一个裁剪得当的回溯示例：)

Commentary(补充说明)
^^^^^^^^^^^^^^^^^^^^

The ``---`` marker line serves the essential purpose of marking for
patch handling tools where the changelog message ends.
(``---`` 标记行的一个重要作用是为补丁处理工具标明 changelog 消息到哪里结束。)

One good use for the additional comments after the ``---`` marker is
for a ``diffstat``, to show what files have changed, and the number of
inserted and deleted lines per file. A ``diffstat`` is especially useful
on bigger patches. If you are going to include a ``diffstat`` after the
``---`` marker, please use ``diffstat`` options ``-p 1 -w 70`` so that
filenames are listed from the top of the kernel source tree and don't
use too much horizontal space (easily fit in 80 columns, maybe with some
indentation). (``git`` generates appropriate diffstats by default.)
(``---`` 标记之后的补充说明有一个很好的用途，就是放 ``diffstat``，用来显示哪些文件发生了变化，以及每个文件插入和删除的行数。``diffstat`` 对较大的补丁尤其有用。如果你打算在 ``---`` 标记之后包含 ``diffstat``，请使用 ``diffstat`` 选项 ``-p 1 -w 70``，使文件名从内核源码树的顶层开始列出，并且不占用太多水平空间(轻松放进 80 列，或许带点缩进)。(``git`` 默认会生成合适的 diffstat。))

Other comments relevant only to the moment or the maintainer, not
suitable for the permanent changelog, should also go here. A good
example of such comments might be ``patch changelogs`` which describe
what has changed between the v1 and v2 version of the patch.
(其他只与当下或维护者相关、不适合放入永久 changelog 的说明也应放在这里。这类说明的一个好例子是 ``patch changelogs``，它描述补丁 v1 与 v2 版本之间有哪些变化。)

Please put this information **after** the ``---`` line which separates
the changelog from the rest of the patch. The version information is
not part of the changelog which gets committed to the git tree. It is
additional information for the reviewers. If it's placed above the
commit tags, it needs manual interaction to remove it. If it is below
the separator line, it gets automatically stripped off when applying the
patch.  If available, adding links to previous versions of the patch (e.g.,
lore.kernel.org archive link) is recommended to help reviewers::

  <commit message>
  ...
  Signed-off-by: Author <author@mail>
  ---
  V2 -> V3: Removed redundant helper function
  V1 -> V2: Cleaned up coding style and addressed review comments

  v2: https://lore.kernel.org/bar
  v1: https://lore.kernel.org/foo

  path/to/file | 5+++--
  ...

(请把这些信息放在 ``---`` 行 **之后**——该行把 changelog 与补丁的其余部分隔开。版本信息不属于会提交进 git 树的 changelog，它是给评审者的附加信息。如果放在 commit 标签之上，就需要人工干预才能移除；如果放在分隔行之下，应用补丁时会被自动剥掉。如果有的话，建议附上指向补丁先前版本的链接(如 lore.kernel.org 归档链接)以方便评审者：)

See more details on the proper patch format in the following
references.
(更多关于规范补丁格式的细节见下列参考资料。)

.. _explicit_in_reply_to:

Explicit In-Reply-To headers(显式的 In-Reply-To 头)
---------------------------------------------------

It can be helpful to manually add In-Reply-To: headers to a patch
(e.g., when using ``git send-email``) to associate the patch with
previous relevant discussion, e.g. to link a bug fix to the email with
the bug report.  However, for a multi-patch series, it is generally
best to avoid using In-Reply-To: to link to older versions of the
series.  This way multiple versions of the patch don't become an
unmanageable forest of references in email clients.  If a link is
helpful, you can use the https://lore.kernel.org/ redirector (e.g., in
the cover email text) to link to an earlier version of the patch series.
(手动给补丁加上 In-Reply-To: 头(例如在使用 ``git send-email`` 时)会很有帮助，可以把补丁与先前的相关讨论关联起来，例如把 bug 修复链接到报告该 bug 的邮件。不过，对于多补丁系列，一般最好避免用 In-Reply-To: 链接到该系列的旧版本。这样，补丁的多个版本才不会在邮件客户端里变成一团无法管理的引用森林。如果链接有用，你可以使用 https://lore.kernel.org/ 重定向器(例如在封面邮件正文中)链接到补丁系列的较早版本。)


Providing base tree information(提供基底树信息)
-----------------------------------------------

When other developers receive your patches and start the review process,
it is absolutely necessary for them to know what is the base
commit/branch your work applies on, considering the sheer amount of
maintainer trees present nowadays. Note again the **T:** entry in the
MAINTAINERS file explained above.
(当其他开发者收到你的补丁并开始评审时，他们绝对需要知道你的工作是应用在哪个基础 commit/分支之上的——如今维护者代码树数量庞大，这一点尤为重要。再次注意上文解释过的 MAINTAINERS 文件中的 **T:** 条目。)

This is even more important for automated CI processes that attempt to
run a series of tests in order to establish the quality of your
submission before the maintainer starts the review.
(对于自动化 CI 流程来说这一点更为重要——它们会在维护者开始评审之前尝试运行一系列测试来确认你提交的质量。)

If you are using ``git format-patch`` to generate your patches, you can
automatically include the base tree information in your submission by
using the ``--base`` flag. The easiest and most convenient way to use
this option is with topical branches::

    $ git checkout -t -b my-topical-branch master
    Branch 'my-topical-branch' set up to track local branch 'master'.
    Switched to a new branch 'my-topical-branch'

    [perform your edits and commits]

    $ git format-patch --base=auto --cover-letter -o outgoing/ master
    outgoing/0000-cover-letter.patch
    outgoing/0001-First-Commit.patch
    outgoing/...

(如果你使用 ``git format-patch`` 生成补丁，可以用 ``--base`` 标志在提交中自动包含基底树信息。使用该选项最简单方便的方式是配合主题分支 (topical branch)：)

When you open ``outgoing/0000-cover-letter.patch`` for editing, you will
notice that it will have the ``base-commit:`` trailer at the very
bottom, which provides the reviewer and the CI tools enough information
to properly perform ``git am`` without worrying about conflicts::

    $ git checkout -b patch-review [base-commit-id]
    Switched to a new branch 'patch-review'
    $ git am patches.mbox
    Applying: First Commit
    Applying: ...

(当你打开 ``outgoing/0000-cover-letter.patch`` 编辑时，会注意到它的最底部带有 ``base-commit:`` 尾注，它为评审者和 CI 工具提供了足够的信息，可以放心执行 ``git am`` 而不必担心冲突：)

Please see ``man git-format-patch`` for more information about this
option.
(关于该选项的更多信息，请参见 ``man git-format-patch``。)

.. note::

    The ``--base`` feature was introduced in git version 2.9.0.
    (注意：``--base`` 功能是在 git 2.9.0 版本中引入的。)

If you are not using git to format your patches, you can still include
the same ``base-commit`` trailer to indicate the commit hash of the tree
on which your work is based. You should add it either in the cover
letter or in the first patch of the series and it should be placed
either below the ``---`` line or at the very bottom of all other
content, right before your email signature.

Make sure that base commit is in an official maintainer/mainline tree
and not in some internal, accessible only to you tree - otherwise it
would be worthless.
(如果你不使用 git 来格式化补丁，仍然可以包含同样的 ``base-commit`` 尾注来标明你的工作所基于的树的 commit 哈希。你应当把它加在封面信或系列的首个补丁中，并放在 ``---`` 行之下，或放在所有其他内容的最底部、紧挨着你邮件签名之前。

请确保该基础 commit 位于官方维护者/主线树中，而不是某个只有你能访问的内部树中——否则它将毫无价值。)

Tooling(工具)
-------------

Many of the technical aspects of this process can be automated using
b4, documented at <https://b4.docs.kernel.org/en/latest/>. This can
help with things like tracking dependencies, running checkpatch and
with formatting and sending mails.
(这一流程中的许多技术环节都可以用 b4 自动化，其文档见 <https://b4.docs.kernel.org/en/latest/>。它能帮助处理诸如跟踪依赖、运行 checkpatch 以及格式化和发送邮件之类的事情。)

References(参考资料)
--------------------
(以下为本文件的参考文献条目，均保留原文与链接。)

Andrew Morton, "The perfect patch" (tpp).
  <https://www.ozlabs.org/~akpm/stuff/tpp.txt>

Jeff Garzik, "Linux kernel patch submission format".
  <https://web.archive.org/web/20180829112450/http://linux.yyz.us/patch-format.html>

Greg Kroah-Hartman, "How to piss off a kernel subsystem maintainer".
  <http://www.kroah.com/log/linux/maintainer.html>

  <http://www.kroah.com/log/linux/maintainer-02.html>

  <http://www.kroah.com/log/linux/maintainer-03.html>

  <http://www.kroah.com/log/linux/maintainer-04.html>

  <http://www.kroah.com/log/linux/maintainer-05.html>

  <http://www.kroah.com/log/linux/maintainer-06.html>

Kernel Documentation/process/coding-style.rst

Linus Torvalds's mail on the canonical patch format:
  <https://lore.kernel.org/r/Pine.LNX.4.58.0504071023190.28951@ppc970.osdl.org>

Andi Kleen, "On submitting kernel patches"
  Some strategies to get difficult or controversial changes in.

  http://halobates.de/on-submitting-patches.pdf
