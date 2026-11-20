.. SPDX-License-Identifier: GPL-2.0

KVM x86(KVM x86 维护须知)
=========================

Foreword(前言)
--------------
KVM strives to be a welcoming community; contributions from newcomers are
valued and encouraged.  Please do not be discouraged or intimidated by the
length of this document and the many rules/guidelines it contains.  Everyone
makes mistakes, and everyone was a newbie at some point.  So long as you make
an honest effort to follow KVM x86's guidelines, are receptive to feedback,
and learn from any mistakes you make, you will be welcomed with open arms, not
torches and pitchforks.
(KVM 努力成为一个友善的社区；新人的贡献受到重视和鼓励。请不要因为本文档的长度及其包含的众多规则/指南而气馁或望而却步。人人都会犯错，人人都曾是新手。只要你真诚努力地遵循 KVM x86 的指南、乐于接受反馈并从错误中学习，你就会得到张开双臂的欢迎，而不是火把与草叉。)

TL;DR(要点速览)
---------------
Testing is mandatory.  Be consistent with established styles and patterns.
(测试是强制性的。与既有的风格和模式保持一致。)

Trees(代码树)
-------------
KVM x86 is currently in a transition period from being part of the main KVM
tree, to being "just another KVM arch".  As such, KVM x86 is split across the
main KVM tree, ``git.kernel.org/pub/scm/virt/kvm/kvm.git``, and a KVM x86
specific tree, ``github.com/kvm-x86/linux.git``.
(KVM x86 目前正处于一个过渡期：从主 KVM 树的一部分，转变为"又一名普通 KVM 架构"。因此，KVM x86 分布在主 KVM 树 ``git.kernel.org/pub/scm/virt/kvm/kvm.git`` 与 KVM x86 专属树 ``github.com/kvm-x86/linux.git`` 两处。)

Generally speaking, fixes for the current cycle are applied directly to the
main KVM tree, while all development for the next cycle is routed through the
KVM x86 tree.  In the unlikely event that a fix for the current cycle is routed
through the KVM x86 tree, it will be applied to the ``fixes`` branch before
making its way to the main KVM tree.
(一般而言，针对当前周期的修复直接应用到主 KVM 树，而下一个周期的所有开发都经由 KVM x86 树流转。在极少数情况下，若当前周期的修复经由 KVM x86 树流转，它会先被应用到 ``fixes`` 分支，然后再进入主 KVM 树。)

Note, this transition period is expected to last quite some time, i.e. will be
the status quo for the foreseeable future.
(注意，预计这一过渡期会持续相当长的时间，也就是说，在可预见的未来这将维持现状。)

Branches(分支)
~~~~~~~~~~~~~~
The KVM x86 tree is organized into multiple topic branches.  The purpose of
using finer-grained topic branches is to make it easier to keep tabs on an area
of development, and to limit the collateral damage of human errors and/or buggy
commits, e.g. dropping the HEAD commit of a topic branch has no impact on other
in-flight commits' SHA1 hashes, and having to reject a pull request due to bugs
delays only that topic branch.
(KVM x86 树被组织成多个主题分支 (topic branch)。使用更细粒度主题分支的目的，是让跟踪某个开发领域变得更容易，并限制人为失误和/或有缺陷提交的连带损害。例如，丢弃某个主题分支的 HEAD 提交不会影响其他在途提交的 SHA1 哈希；因缺陷而不得不拒绝一个拉取请求时，也只是推迟那一个主题分支而已。)

All topic branches, except for ``next`` and ``fixes``, are rolled into ``next``
via a Cthulhu merge on an as-needed basis, i.e. when a topic branch is updated.
As a result, force pushes to ``next`` are common.
(除 ``next`` 与 ``fixes`` 之外的所有主题分支，都会按需(即当某个主题分支更新时)通过一次克苏鲁式合并 (Cthulhu merge) 卷入 ``next``。因此，向 ``next`` 强制推送是常有的事。)

Lifecycle(生命周期)
~~~~~~~~~~~~~~~~~~~
Fixes that target the current release, a.k.a. mainline, are typically applied
directly to the main KVM tree, i.e. do not route through the KVM x86 tree.
(针对当前发布版(即主线)的修复通常直接应用到主 KVM 树，即不经由 KVM x86 树流转。)

Changes that target the next release are routed through the KVM x86 tree.  Pull
requests (from KVM x86 to main KVM) are sent for each KVM x86 topic branch,
typically the week before Linus' opening of the merge window, e.g. the week
following rc7 for "normal" releases.  If all goes well, the topic branches are
rolled into the main KVM pull request sent during Linus' merge window.
(针对下一发布版的修改经由 KVM x86 树流转。每个 KVM x86 主题分支都会发送拉取请求(从 KVM x86 到主 KVM)，通常在 Linus 打开合并窗口的前一周，例如对"正常"发布版而言是 rc7 之后的那一周。如果一切顺利，各主题分支会被卷入 Linus 合并窗口期间发送的主 KVM 拉取请求。)

The KVM x86 tree doesn't have its own official merge window, but there's a soft
close around rc5 for new features, and a soft close around rc6 for fixes (for
the next release; see above for fixes that target the current release).
(KVM x86 树没有自己的正式合并窗口，但对新特性有 rc5 前后的软关闭，对修复有 rc6 前后的软关闭(针对下一发布版；针对当前发布版的修复见上文)。)

Timeline(时间线)
~~~~~~~~~~~~~~~~
Submissions are typically reviewed and applied in FIFO order, with some wiggle
room for the size of a series, patches that are "cache hot", etc.  Fixes,
especially for the current release and or stable trees, get to jump the queue.
Patches that will be taken through a non-KVM tree (most often through the tip
tree) and/or have other acks/reviews also jump the queue to some extent.
(提交通常按 FIFO 顺序评审与应用，但系列的大小、"缓存热"的补丁等会有一些弹性。修复——尤其是针对当前发布版和/或 stable 树的修复——可以插队。将经由非 KVM 树(最常见是 tip 树)合入的补丁，和/或已有其他 ack/review 的补丁，也会在一定程度上插队。)

Note, the vast majority of review is done between rc1 and rc6, give or take.
The period between rc6 and the next rc1 is used to catch up on other tasks,
i.e. radio silence during this period isn't unusual.
(注意，绝大多数评审工作大致发生在 rc1 与 rc6 之间。rc6 到下一个 rc1 之间的时间用来处理其他任务，也就是说，这期间一片寂静并不罕见。)

Pings to get a status update are welcome, but keep in mind the timing of the
current release cycle and have realistic expectations.  If you are pinging for
acceptance, i.e. not just for feedback or an update, please do everything you
can, within reason, to ensure that your patches are ready to be merged!  Pings
on series that break the build or fail tests lead to unhappy maintainers!
(欢迎 ping 着要状态更新，但请考虑当前发布周期的时机，并保持现实的预期。如果你 ping 是为了被合入——而不只是要反馈或进度——请在合理范围内竭尽所能确保你的补丁已达到可合并状态！对会破坏构建或测试失败的系列 ping 个不停，只会让维护者心情变差！)

Development(开发)
-----------------

Base Tree/Branch(基底树/分支)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Fixes that target the current release, a.k.a. mainline, should be based on
``git://git.kernel.org/pub/scm/virt/kvm/kvm.git master``.  Note, fixes do not
automatically warrant inclusion in the current release.  There is no singular
rule, but typically only fixes for bugs that are urgent, critical, and/or were
introduced in the current release should target the current release.
(针对当前发布版(即主线)的修复应基于 ``git://git.kernel.org/pub/scm/virt/kvm/kvm.git master``。注意，修复并不会自动保证被纳入当前发布版。没有唯一的标准，但通常只有针对紧急、严重和/或当前发布版引入的缺陷的修复，才应以当前发布版为目标。)

Everything else should be based on ``kvm-x86/next``, i.e. there is no need to
select a specific topic branch as the base.  If there are conflicts and/or
dependencies across topic branches, it is the maintainer's job to sort them
out.
(其余一切都应基于 ``kvm-x86/next``，即无需选择某个特定主题分支作为基底。如果主题分支之间存在冲突和/或依赖，把它们理顺是维护者的工作。)

The only exception to using ``kvm-x86/next`` as the base is if a patch/series
is a multi-arch series, i.e. has non-trivial modifications to common KVM code
and/or has more than superficial changes to other architectures' code.  Multi-
arch patch/series should instead be based on a common, stable point in KVM's
history, e.g. the release candidate upon which ``kvm-x86 next`` is based.  If
you're unsure whether a patch/series is truly multi-arch, err on the side of
caution and treat it as multi-arch, i.e. use a common base.
(使用 ``kvm-x86/next`` 作为基底的唯一例外，是补丁/系列属于跨架构 (multi-arch) 系列，即对 KVM 公共代码有非平凡的修改，和/或对其他架构代码有超出表面的改动。跨架构补丁/系列应改为基于 KVM 历史中一个公共的稳定点，例如 ``kvm-x86 next`` 所基于的候选发布版。如果不确定某个补丁/系列是否真是跨架构的，宁可从保守出发把它当作跨架构处理，即使用公共基底。)

Coding Style(编码风格)
~~~~~~~~~~~~~~~~~~~~~~
When it comes to style, naming, patterns, etc., consistency is the number one
priority in KVM x86.  If all else fails, match what already exists.
(在风格、命名、模式等方面，一致性是 KVM x86 的第一要务。如果实在拿不准，就与既有代码保持一致。)

With a few caveats listed below, follow the tip tree maintainers' preferred
:ref:`maintainer-tip-coding-style`, as patches/series often touch both KVM and
non-KVM x86 files, i.e. draw the attention of KVM *and* tip tree maintainers.
(除下面列出的少数注意事项外，请遵循 tip 树维护者偏好的 :ref:`maintainer-tip-coding-style`，因为补丁/系列常常同时触及 KVM 与非 KVM 的 x86 文件，也就是会同时引起 KVM *和* tip 树维护者的注意。)

Using reverse fir tree, a.k.a. reverse Christmas tree or reverse XMAS tree, for
variable declarations isn't strictly required, though it is still preferred.
(变量声明使用倒置冷杉树 (reverse fir tree)——又名倒置圣诞树 (reverse Christmas tree) 或 reverse XMAS tree——并非严格要求，但仍是偏好做法。)

Except for a handful of special snowflakes, do not use kernel-doc comments for
functions.  The vast majority of "public" KVM functions aren't truly public as
they are intended only for KVM-internal consumption (there are plans to
privatize KVM's headers and exports to enforce this).
(除少数特例之外，不要为函数使用 kernel-doc 注释。绝大多数"公开"的 KVM 函数并非真正公开，因为它们只供 KVM 内部使用(已有计划把 KVM 的头文件与导出私有化以强制这一点)。)

Comments(注释)
~~~~~~~~~~~~~~
Write comments using imperative mood and avoid pronouns.  Use comments to
provide a high level overview of the code, and/or to explain why the code does
what it does.  Do not reiterate what the code literally does; let the code
speak for itself.  If the code itself is inscrutable, comments will not help.
(注释使用祈使语气，避免代词。用注释提供代码的高层概览，和/或解释代码为何要这样做。不要复述代码字面上在做什么；让代码自己说话。如果代码本身晦涩难懂，注释也帮不了忙。)

SDM and APM References(SDM 与 APM 引用)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Much of KVM's code base is directly tied to architectural behavior defined in
Intel's Software Development Manual (SDM) and AMD's Architecture Programmer’s
Manual (APM).  Use of "Intel's SDM" and "AMD's APM", or even just "SDM" or
"APM", without additional context is a-ok.
(KVM 代码库的很大一部分与 Intel 软件开发手册 (SDM) 和 AMD 架构程序员手册 (APM) 中定义的架构行为直接相关。不加额外上下文地使用 "Intel's SDM" 和 "AMD's APM"，甚至只写 "SDM" 或 "APM"，都完全可以。)

Do not reference specific sections, tables, figures, etc. by number, especially
not in comments.  Instead, if necessary (see below), copy-paste the relevant
snippet and reference sections/tables/figures by name.  The layouts of the SDM
and APM are constantly changing, and so the numbers/labels aren't stable.
(不要按编号引用具体的章节、表格、插图等，尤其不要在注释中这样做。必要时(见下文)应复制粘贴相关片段，并按名称引用章节/表格/插图。SDM 与 APM 的版式经常变化，编号/标签并不稳定。)

Generally speaking, do not explicitly reference or copy-paste from the SDM or
APM in comments.  With few exceptions, KVM *must* honor architectural behavior,
therefore it's implied that KVM behavior is emulating SDM and/or APM behavior.
Note, referencing the SDM/APM in changelogs to justify the change and provide
context is perfectly ok and encouraged.
(总体而言，不要在注释中显式引用或复制粘贴 SDM 或 APM 的内容。除极少数例外，KVM *必须* 遵循架构行为，因此 KVM 的行为默认就是在仿真 SDM 和/或 APM 的行为。注意，在变更日志中引用 SDM/APM 来论证修改、提供上下文，是完全允许且受鼓励的。)

Shortlog(短日志)
~~~~~~~~~~~~~~~~
The preferred prefix format is ``KVM: <topic>:``, where ``<topic>`` is one of::

  - x86
  - x86/mmu
  - x86/pmu
  - x86/xen
  - selftests
  - SVM
  - nSVM
  - VMX
  - nVMX

(首选的前缀格式是 ``KVM: <topic>:``，其中 ``<topic>`` 是以下之一：)

**DO NOT use x86/kvm!**  ``x86/kvm`` is used exclusively for Linux-as-a-KVM-guest
changes, i.e. for arch/x86/kernel/kvm.c.  Do not use file names or complete file
paths as the subject/shortlog prefix.
(**不要使用 x86/kvm！** ``x86/kvm`` 专用于 Linux 作为 KVM 客户机的修改，即 arch/x86/kernel/kvm.c。不要用文件名或完整文件路径作为主题/短日志前缀。)

Note, these don't align with the topics branches (the topic branches care much
more about code conflicts).
(注意，这些主题与主题分支并不一一对应(主题分支更关心代码冲突)。)

All names are case sensitive!  ``KVM: x86:`` is good, ``kvm: vmx:`` is not.
(所有名称都区分大小写！``KVM: x86:`` 是对的，``kvm: vmx:`` 不对。)

Capitalize the first word of the condensed patch description, but omit ending
punctionation.  E.g.::

    KVM: x86: Fix a null pointer dereference in function_xyz()

not::

    kvm: x86: fix a null pointer dereference in function_xyz.

(压缩版补丁描述的首词要大写，但省略结尾标点。例如应写成：

    KVM: x86: Fix a null pointer dereference in function_xyz()

而不是：

    kvm: x86: fix a null pointer dereference in function_xyz。)

If a patch touches multiple topics, traverse up the conceptual tree to find the
first common parent (which is often simply ``x86``).  When in doubt,
``git log path/to/file`` should provide a reasonable hint.
(如果补丁触及多个主题，沿概念树向上找到第一个公共父节点(通常就是 ``x86``)。拿不准时，``git log path/to/file`` 应能提供合理的提示。)

New topics do occasionally pop up, but please start an on-list discussion if
you want to propose introducing a new topic, i.e. don't go rogue.
(新主题偶尔会出现，但如果你想提议引入一个新主题，请先在邮件列表上发起讨论，即不要擅自行动。)

See :ref:`the_canonical_patch_format` for more information, with one amendment:
do not treat the 70-75 character limit as an absolute, hard limit.  Instead,
use 75 characters as a firm-but-not-hard limit, and use 80 characters as a hard
limit.  I.e. let the shortlog run a few characters over the standard limit if
you have good reason to do so.
(更多信息参见 :ref:`the_canonical_patch_format`，但有一处修订：不要把 70-75 字符的限制当作绝对的硬性限制。把 75 字符当作"硬但不死板"的限制，80 字符作为硬性限制。也就是说，如果有充分理由，可以让短日志超出标准限制几个字符。)

Changelog(变更日志)
~~~~~~~~~~~~~~~~~~~
Most importantly, write changelogs using imperative mood and avoid pronouns.
(最重要的是，变更日志要使用祈使语气，并避免代词。)

See :ref:`describe_changes` for more information, with one amendment: lead with
a short blurb on the actual changes, and then follow up with the context and
background.  Note!  This order directly conflicts with the tip tree's preferred
approach!  Please follow the tip tree's preferred style when sending patches
that primarily target arch/x86 code that is _NOT_ KVM code.
(更多信息参见 :ref:`describe_changes`，但有一处修订：开头先用简短的几句话讲实际改动，然后再补充上下文与背景。注意！这一顺序与 tip 树偏好的做法直接冲突！当补丁主要针对 *非* KVM 代码的 arch/x86 代码时，请遵循 tip 树偏好的风格。)

Stating what a patch does before diving into details is preferred by KVM x86
for several reasons.  First and foremost, what code is actually being changed
is arguably the most important information, and so that info should be easy to
find. Changelogs that bury the "what's actually changing" in a one-liner after
3+ paragraphs of background make it very hard to find that information.
(KVM x86 偏好先说明补丁做什么、再深入细节，原因有几点。首先，实际改动的是什么代码可以说最重要的信息，因此这些信息应当容易找到。把"实际在改什么"埋在 3 段以上背景之后的一行话里的变更日志，会让人很难找到该信息。)

For initial review, one could argue the "what's broken" is more important, but
for skimming logs and git archaeology, the gory details matter less and less.
E.g. when doing a series of "git blame", the details of each change along the
way are useless, the details only matter for the culprit.  Providing the "what
changed" makes it easy to quickly determine whether or not a commit might be of
interest.
(对于初步评审，"哪里坏了"或许更重要；但对于浏览日志与 git 考古而言，细节越来越不重要。例如，在连续做 "git blame" 时，沿途每个提交的细节毫无用处，只有肇事提交的细节才重要。提供"改了什么"让人可以快速判断一个提交是否值得关注。)

Another benefit of stating "what's changing" first is that it's almost always
possible to state "what's changing" in a single sentence.  Conversely, all but
the most simple bugs require multiple sentences or paragraphs to fully describe
the problem.  If both the "what's changing" and "what's the bug" are super
short then the order doesn't matter.  But if one is shorter (almost always the
"what's changing), then covering the shorter one first is advantageous because
it's less of an inconvenience for readers/reviewers that have a strict ordering
preference.  E.g. having to skip one sentence to get to the context is less
painful than having to skip three paragraphs to get to "what's changing".
(先说"改了什么"的另一个好处是：几乎总能用一句话概括"改了什么"。相反，除了最简单的缺陷，其余缺陷都需要多句话甚至多段才能完整描述问题。如果"改了什么"与"缺陷是什么"都非常短，顺序无关紧要。但若其中一个更短(几乎总是"改了什么")，先讲较短的那个更有利，因为对有严格顺序偏好的读者/评审者而言麻烦更小。例如，跳过一句话去看上下文，总比跳过三段才看到"改了什么"轻松。)

Fixes(修复)
~~~~~~~~~~~
If a change fixes a KVM/kernel bug, add a Fixes: tag even if the change doesn't
need to be backported to stable kernels, and even if the change fixes a bug in
an older release.
(如果修改修复了一个 KVM/内核缺陷，即使该修改不需要回移到 stable 内核，即使它修的是更早发布版中的缺陷，也要加上 Fixes: 标签。)

Conversely, if a fix does need to be backported, explicitly tag the patch with
"Cc: stable@vger.kernel" (though the email itself doesn't need to Cc: stable);
KVM x86 opts out of backporting Fixes: by default.  Some auto-selected patches
do get backported, but require explicit maintainer approval (search MANUALSEL).
(反过来，如果修复确实需要回移，请用 "Cc: stable@vger.kernel" 显式标记补丁(不过邮件本身不需要 Cc: stable)；KVM x86 默认不参与基于 Fixes: 的自动回移。一些自动选中的补丁确实会被回移，但需要维护者明确批准(搜索 MANUALSEL)。)

Function References(函数引用)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
When a function is mentioned in a comment, changelog, or shortlog (or anywhere
for that matter), use the format ``function_name()``.  The parentheses provide
context and disambiguate the reference.
(在注释、变更日志或短日志(实际上任何地方)中提到函数时，使用 ``function_name()`` 格式。括号提供上下文并消除引用的歧义。)

Testing(测试)
-------------
At a bare minimum, *all* patches in a series must build cleanly for KVM_INTEL=m
KVM_AMD=m, and KVM_WERROR=y.  Building every possible combination of Kconfigs
isn't feasible, but the more the merrier.  KVM_SMM, KVM_XEN, PROVE_LOCKING, and
X86_64 are particularly interesting knobs to turn.
(至少要保证系列中的 *所有* 补丁在 KVM_INTEL=m、KVM_AMD=m 与 KVM_WERROR=y 下都能干净构建。构建所有可能的 Kconfig 组合并不现实，但多多益善。KVM_SMM、KVM_XEN、PROVE_LOCKING 与 X86_64 是特别值得调动的开关。)

Running KVM selftests and KVM-unit-tests is also mandatory (and stating the
obvious, the tests need to pass).  The only exception is for changes that have
negligible probability of affecting runtime behavior, e.g. patches that only
modify comments.  When possible and relevant, testing on both Intel and AMD is
strongly preferred.  Booting an actual VM is encouraged, but not mandatory.
(运行 KVM selftests 与 KVM-unit-tests 也是强制性的(不言而喻，测试必须通过)。唯一的例外是对运行时行为影响概率可忽略不计的修改，例如只改注释的补丁。在可行且相关时，强烈建议在 Intel 与 AMD 上都做测试。引导一个真实的 VM 是受鼓励的，但非强制。)

For changes that touch KVM's shadow paging code, running with TDP (EPT/NPT)
disabled is mandatory.  For changes that affect common KVM MMU code, running
with TDP disabled is strongly encouraged.  For all other changes, if the code
being modified depends on and/or interacts with a module param, testing with
the relevant settings is mandatory.
(对于触及 KVM 影子分页代码的修改，在禁用 TDP (EPT/NPT) 的情况下运行是强制的。对于影响 KVM 公共 MMU 代码的修改，强烈建议在禁用 TDP 的情况下运行。对于所有其他修改，如果被修改的代码依赖和/或与某个模块参数交互，则用相关设置进行测试是强制的。)

Note, KVM selftests and KVM-unit-tests do have known failures.  If you suspect
a failure is not due to your changes, verify that the *exact same* failure
occurs with and without your changes.
(注意，KVM selftests 与 KVM-unit-tests 确实存在已知失败。如果你怀疑某个失败与你的修改无关，请验证在带与不带你的修改时失败 *完全相同*。)

Changes that touch reStructured Text documentation, i.e. .rst files, must build
htmldocs cleanly, i.e. with no new warnings or errors.
(触及 reStructured Text 文档(即 .rst 文件)的修改必须能干净地构建 htmldocs，即没有新的警告或错误。)

If you can't fully test a change, e.g. due to lack of hardware, clearly state
what level of testing you were able to do, e.g. in the cover letter.
(如果无法完整测试某个修改，例如缺少硬件，请明确说明你所能做到的测试程度，例如写在封面信里。)

New Features(新特性)
~~~~~~~~~~~~~~~~~~~~
With one exception, new features *must* come with test coverage.  KVM specific
tests aren't strictly required, e.g. if coverage is provided by running a
sufficiently enabled guest VM, or by running a related kernel selftest in a VM,
but dedicated KVM tests are preferred in all cases.  Negative testcases in
particular are mandatory for enabling of new hardware features as error and
exception flows are rarely exercised simply by running a VM.
(除一个例外，新特性 *必须* 附带测试覆盖。KVM 专属测试并非严格要求——例如覆盖可以来自运行一个充分启用的客户机 VM，或在 VM 中运行相关的内核 selftest——但任何情况下都偏好专门的 KVM 测试。启用新硬件特性时，负面测试用例尤其是强制的，因为仅靠运行一个 VM 很少会走到错误与异常路径。)

The only exception to this rule is if KVM is simply advertising support for a
feature via KVM_GET_SUPPORTED_CPUID, i.e. for instructions/features that KVM
can't prevent a guest from using and for which there is no true enabling.
(这一规则的唯一例外是：KVM 只是经由 KVM_GET_SUPPORTED_CPUID 宣告对某特性的支持，即对于那些 KVM 无法阻止客户机使用、也不存在真正"启用"动作的指令/特性。)

Note, "new features" does not just mean "new hardware features"!  New features
that can't be well validated using existing KVM selftests and/or KVM-unit-tests
must come with tests.
(注意，"新特性"不只是指"新硬件特性"！无法用现有 KVM selftests 和/或 KVM-unit-tests 充分验证的新特性，必须附带测试。)

Posting new feature development without tests to get early feedback is more
than welcome, but such submissions should be tagged RFC, and the cover letter
should clearly state what type of feedback is requested/expected.  Do not abuse
the RFC process; RFCs will typically not receive in-depth review.
(非常欢迎不带测试先投递新特性的开发以获得早期反馈，但此类提交应标记为 RFC，且封面信应清楚说明请求/期待哪种类型的反馈。不要滥用 RFC 流程；RFC 通常不会得到深入评审。)

Bug Fixes(缺陷修复)
~~~~~~~~~~~~~~~~~~~
Except for "obvious" found-by-inspection bugs, fixes must be accompanied by a
reproducer for the bug being fixed.  In many cases the reproducer is implicit,
e.g. for build errors and test failures, but it should still be clear to
readers what is broken and how to verify the fix.  Some leeway is given for
bugs that are found via non-public workloads/tests, but providing regression
tests for such bugs is strongly preferred.
(除"一眼可见"的缺陷外，修复必须附带所修缺陷的复现手段。许多情况下复现手段是隐含的，例如构建错误与测试失败，但读者仍应能清楚地知道什么东西坏了以及如何验证修复。对经由非公开工作负载/测试发现的缺陷会有一定宽容，但为这类缺陷提供回归测试是非常受鼓励的。)

In general, regression tests are preferred for any bug that is not trivial to
hit.  E.g. even if the bug was originally found by a fuzzer such as syzkaller,
a targeted regression test may be warranted if the bug requires hitting a
one-in-a-million type race condition.
(总体而言，任何不容易踩中的缺陷都偏好有回归测试。例如，即使缺陷最初是由 syzkaller 之类的模糊测试工具发现的，如果触发该缺陷需要碰上百万分之一级别的竞态，那么针对性的回归测试可能仍然是必要的。)

Note, KVM bugs are rarely urgent *and* non-trivial to reproduce.  Ask yourself
if a bug is really truly the end of the world before posting a fix without a
reproducer.
(注意，KVM 缺陷很少既紧急又难以复现。在不带复现手段就投递修复之前，先问问自己：这个缺陷真的真的是世界末日吗？)

Posting(投递)
-------------

Links(链接)
~~~~~~~~~~~
Do not explicitly reference bug reports, prior versions of a patch/series, etc.
via ``In-Reply-To:`` headers.  Using ``In-Reply-To:`` becomes an unholy mess
for large series and/or when the version count gets high, and ``In-Reply-To:``
is useless for anyone that doesn't have the original message, e.g. if someone
wasn't Cc'd on the bug report or if the list of recipients changes between
versions.
(不要通过 ``In-Reply-To:`` 头显式引用缺陷报告、补丁/系列的先前版本等。对大型系列和/或版本数较多的情况，使用 ``In-Reply-To:`` 会变成一团乱麻；而且对任何没有原始邮件的人而言 ``In-Reply-To:`` 都毫无用处，例如某人未被 Cc 到缺陷报告，或收件人列表在版本之间发生了变化。)

To link to a bug report, previous version, or anything of interest, use lore
links.  For referencing previous version(s), generally speaking do not include
a Link: in the changelog as there is no need to record the history in git, i.e.
put the link in the cover letter or in the section git ignores.  Do provide a
formal Link: for bug reports and/or discussions that led to the patch.  The
context of why a change was made is highly valuable for future readers.
(要链接缺陷报告、先前版本或任何值得关注的内容，请使用 lore 链接。引用先前版本时，一般不要在变更日志中放 Link:，因为没有必要把历史记录进 git——把链接放在封面信或 git 会忽略的部分即可。对于引出该补丁的缺陷报告和/或讨论，则要提供正式的 Link:。修改缘由的上下文对未来读者非常有价值。)

Git Base(Git 基底)
~~~~~~~~~~~~~~~~~~
If you are using git version 2.9.0 or later (Googlers, this is all of you!),
use ``git format-patch`` with the ``--base`` flag to automatically include the
base tree information in the generated patches.
(如果你使用 git 2.9.0 或更高版本(Googlers，说的就是你们所有人！)，请使用带 ``--base`` 旗标的 ``git format-patch``，自动在生成的补丁中包含基底树信息。)

Note, ``--base=auto`` works as expected if and only if a branch's upstream is
set to the base topic branch, e.g. it will do the wrong thing if your upstream
is set to your personal repository for backup purposes.  An alternative "auto"
solution is to derive the names of your development branches based on their
KVM x86 topic, and feed that into ``--base``.  E.g. ``x86/pmu/my_branch_name``,
and then write a small wrapper to extract ``pmu`` from the current branch name
to yield ``--base=x/pmu``, where ``x`` is whatever name your repository uses to
track the KVM x86 remote.
(注意，``--base=auto`` 当且仅当分支的上游被设为基底主题分支时才符合预期，例如若你的上游是为备份目的设成的个人仓库，它就会做错事。另一种"自动"方案是按 KVM x86 主题为开发分支命名，并把主题喂给 ``--base``。例如分支名为 ``x86/pmu/my_branch_name``，然后写一个小包装脚本从当前分支名提取 ``pmu``，得到 ``--base=x/pmu``，其中 ``x`` 是你的仓库用于跟踪 KVM x86 远端的任意名称。)

Co-Posting Tests(随补丁一同投递测试)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
KVM selftests that are associated with KVM changes, e.g. regression tests for
bug fixes, should be posted along with the KVM changes as a single series.  The
standard kernel rules for bisection apply, i.e. KVM changes that result in test
failures should be ordered after the selftests updates, and vice versa, new
tests that fail due to KVM bugs should be ordered after the KVM fixes.
(与 KVM 修改相关联的 KVM selftests——例如缺陷修复的回归测试——应与 KVM 修改作为一个系列一同投递。适用标准的内核二分规则，即导致测试失败的 KVM 修改应排在 selftests 更新之后；反之，因 KVM 缺陷而失败的新测试应排在 KVM 修复之后。)

KVM-unit-tests should *always* be posted separately.  Tools, e.g. b4 am, don't
know that KVM-unit-tests is a separate repository and get confused when patches
in a series apply on different trees.  To tie KVM-unit-tests patches back to
KVM patches, first post the KVM changes and then provide a lore Link: to the
KVM patch/series in the KVM-unit-tests patch(es).
(KVM-unit-tests 应 *始终* 单独投递。工具(例如 b4 am)并不知道 KVM-unit-tests 是一个独立的仓库，当系列中的补丁应用到不同树上时会犯迷糊。要把 KVM-unit-tests 补丁与 KVM 补丁关联起来，先投递 KVM 修改，然后在 KVM-unit-tests 补丁中提供指向 KVM 补丁/系列的 lore Link:。)

Notifications(通知)
-------------------
When a patch/series is officially accepted, a notification email will be sent
in reply to the original posting (cover letter for multi-patch series).  The
notification will include the tree and topic branch, along with the SHA1s of
the commits of applied patches.
(当补丁/系列被正式接受后，会以回复原始投递(多补丁系列回复封面信)的形式发送通知邮件。通知会包含树与主题分支，以及已应用补丁提交的 SHA1。)

If a subset of patches is applied, this will be clearly stated in the
notification.  Unless stated otherwise, it's implied that any patches in the
series that were not accepted need more work and should be submitted in a new
version.
(如果只应用了一部分补丁，通知中会明确说明。除非另有说明，系列中未被接受的补丁即意味着还需要改进，应以新版本提交。)

If for some reason a patch is dropped after officially being accepted, a reply
will be sent to the notification email explaining why the patch was dropped, as
well as the next steps.
(如果补丁因故在正式接受之后被丢弃，会回复通知邮件解释丢弃原因以及后续步骤。)

SHA1 Stability(SHA1 稳定性)
~~~~~~~~~~~~~~~~~~~~~~~~~~~
SHA1s are not 100% guaranteed to be stable until they land in Linus' tree!  A
SHA1 is *usually* stable once a notification has been sent, but things happen.
In most cases, an update to the notification email be provided if an applied
patch's SHA1 changes.  However, in some scenarios, e.g. if all KVM x86 branches
need to be rebased, individual notifications will not be given.
(在 SHA1 落入 Linus 的树之前，不能 100% 保证其稳定！通知发出后 SHA1 *通常* 就稳定了，但意外总会发生。大多数情况下，如果已应用补丁的 SHA1 发生变化，会更新通知邮件。但在某些场景下(例如所有 KVM x86 分支需要变基)，不会逐一发送通知。)

Vulnerabilities(漏洞)
---------------------
Bugs that can be exploited by the guest to attack the host (kernel or
userspace), or that can be exploited by a nested VM to *its* host (L2 attacking
L1), are of particular interest to KVM.  Please follow the protocol for
:ref:`securitybugs` if you suspect a bug can lead to an escape, data leak, etc.
(可被客户机利用来攻击宿主机(内核或用户空间)的缺陷，或可被嵌套 VM 利用去攻击 *它自己的* 宿主机(L2 攻击 L1)的缺陷，是 KVM 特别关注的问题。如果你怀疑某个缺陷可能导致逃逸、数据泄露等，请遵循 :ref:`securitybugs` 的处理流程。)

