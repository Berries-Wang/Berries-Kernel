.. SPDX-License-Identifier: GPL-2.0

The tip tree handbook(tip 树手册)
=================================

What is the tip tree?(什么是 tip 树？)
--------------------------------------

The tip tree is a collection of several subsystems and areas of
development. The tip tree is both a direct development tree and an
aggregation tree for several sub-maintainer trees. The tip tree gitweb URL
is: https://git.kernel.org/pub/scm/linux/kernel/git/tip/tip.git
(tip 树是多个子系统和开发领域的集合。tip 树既是直接开发树，也是若干子维护者树的聚合树。tip 树的 gitweb URL 为：https://git.kernel.org/pub/scm/linux/kernel/git/tip/tip.git)

The tip tree contains the following subsystems:
(tip 树包含以下子系统：)

   - **x86 architecture**
     (**x86 架构**)

     The x86 architecture development takes place in the tip tree except
     for the x86 KVM and XEN specific parts which are maintained in the
     corresponding subsystems and routed directly to mainline from
     there. It's still good practice to Cc the x86 maintainers on
     x86-specific KVM and XEN patches.
     (x86 架构开发在 tip 树中进行，但 x86 KVM 和 XEN 特定的部分除外——它们在相应的子系统中维护，并从那里直接进入主线。不过，在 x86 特定的 KVM 和 XEN 补丁上 Cc x86 维护者仍然是好习惯。)

     Some x86 subsystems have their own maintainers in addition to the
     overall x86 maintainers.  Please Cc the overall x86 maintainers on
     patches touching files in arch/x86 even when they are not called out
     by the MAINTAINER file.
     (一些 x86 子系统除了整体 x86 维护者之外还有自己的维护者。对于触及 arch/x86 中文件的补丁，即使 MAINTAINER 文件没有明确列出，也请 Cc 整体 x86 维护者。)

     Note, that ``x86@kernel.org`` is not a mailing list. It is merely a
     mail alias which distributes mails to the x86 top-level maintainer
     team. Please always Cc the Linux Kernel mailing list (LKML)
     ``linux-kernel@vger.kernel.org``, otherwise your mail ends up only in
     the private inboxes of the maintainers.
     (注意，``x86@kernel.org`` 不是邮件列表。它只是一个把邮件分发给 x86 顶层维护者团队的邮件别名。请始终 Cc Linux 内核邮件列表（LKML） ``linux-kernel@vger.kernel.org``，否则你的邮件只会进入维护者的私人收件箱。)

   - **Scheduler**
     (调度器)

     Scheduler development takes place in the -tip tree, in the
     sched/core branch - with occasional sub-topic trees for
     work-in-progress patch-sets.
     (调度器开发在 -tip 树的 sched/core 分支中进行——进行中的补丁集偶尔会有子主题树。)

   - **Locking and atomics**
     (锁与原子操作)

     Locking development (including atomics and other synchronization
     primitives that are connected to locking) takes place in the -tip
     tree, in the locking/core branch - with occasional sub-topic trees
     for work-in-progress patch-sets.
     (锁开发（包括原子操作以及与锁相关的其他同步原语）在 -tip 树的 locking/core 分支中进行——进行中的补丁集偶尔会有子主题树。)

   - **Generic interrupt subsystem and interrupt chip drivers**:
     (通用中断子系统与中断芯片驱动)：)

     - interrupt core development happens in the irq/core branch
       (中断核心开发在 irq/core 分支进行)

     - interrupt chip driver development also happens in the irq/core
       branch, but the patches are usually applied in a separate maintainer
       tree and then aggregated into irq/core
       (中断芯片驱动开发同样在 irq/core 分支进行，但补丁通常先应用在单独的维护者树中，然后再聚合进 irq/core)

   - **Time, timers, timekeeping, NOHZ and related chip drivers**:
     (时间、定时器、时间记账、NOHZ 及相关芯片驱动)：)

     - timekeeping, clocksource core, NTP and alarmtimer development
       happens in the timers/core branch, but patches are usually applied in
       a separate maintainer tree and then aggregated into timers/core
       (时间记账、clocksource 核心、NTP 和 alarmtimer 开发在 timers/core 分支进行，但补丁通常先应用在单独的维护者树中，然后再聚合进 timers/core)

     - clocksource/event driver development happens in the timers/core
       branch, but patches are mostly applied in a separate maintainer tree
       and then aggregated into timers/core
       (clocksource/事件驱动开发在 timers/core 分支进行，但补丁大多先应用在单独的维护者树中，然后再聚合进 timers/core)

   - **Performance counters core, architecture support and tooling**:
     (性能计数器核心、架构支持与工具)：)

     - perf core and architecture support development happens in the
       perf/core branch
       (perf 核心与架构支持开发在 perf/core 分支进行)

     - perf tooling development happens in the perf tools maintainer
       tree and is aggregated into the tip tree.
       (perf 工具开发在 perf tools 维护者树进行，并聚合进 tip 树。)

   - **CPU hotplug core**
     (CPU 热插拔核心)

   - **RAS core**
     (RAS 核心)

     Mostly x86-specific RAS patches are collected in the tip ras/core
     branch.
     (主要是 x86 特定的 RAS 补丁收集在 tip 树的 ras/core 分支中。)

   - **EFI core**
     (EFI 核心)

     EFI development in the efi git tree. The collected patches are
     aggregated in the tip efi/core branch.
     (EFI 开发在 efi git 树进行。收集的补丁聚合在 tip 树的 efi/core 分支中。)

   - **RCU**
     (RCU)

     RCU development happens in the linux-rcu tree. The resulting changes
     are aggregated into the tip core/rcu branch.
     (RCU 开发在 linux-rcu 树进行。产生的更改聚合进 tip 树的 core/rcu 分支。)

   - **Various core code components**:
     (各种核心代码组件)：)

       - debugobjects
         (debugobjects)

       - objtool
         (objtool)

       - random bits and pieces
         (零散的小组件)


Patch submission notes(补丁提交须知)
------------------------------------

Selecting the tree/branch(选择树/分支)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In general, development against the head of the tip tree master branch is
fine, but for the subsystems which are maintained separately, have their
own git tree and are only aggregated into the tip tree, development should
take place against the relevant subsystem tree or branch.
(一般来说，基于 tip 树 master 分支顶端进行开发是可以的，但对于单独维护、拥有自己的 git 树、只是聚合进 tip 树的子系统，开发应当基于相应的子系统树或分支进行。)

Bug fixes which target mainline should always be applicable against the
mainline kernel tree. Potential conflicts against changes which are already
queued in the tip tree are handled by the maintainers.
(针对主线的缺陷修复应当始终可以应用到主线内核树上。与 tip 树中已排队的更改之间的潜在冲突由维护者处理。)

Patch subject(补丁主题)
^^^^^^^^^^^^^^^^^^^^^^^

The tip tree preferred format for patch subject prefixes is
'subsys/component:', e.g. 'x86/apic:', 'x86/mm/fault:', 'sched/fair:',
'genirq/core:'. Please do not use file names or complete file paths as
prefix. 'git log path/to/file' should give you a reasonable hint in most
cases.
(tip 树偏好的补丁主题前缀格式是 'subsys/component:'，例如 'x86/apic:'、'x86/mm/fault:'、'sched/fair:'、'genirq/core:'。请不要使用文件名或完整文件路径作为前缀。在大多数情况下，'git log path/to/file' 能给你合理的提示。)

The condensed patch description in the subject line should start with an
uppercase letter and should be written in imperative tone.
(主题行中的简短补丁描述应当以大写字母开头，并使用祈使语气书写。)


Changelog(变更日志)
^^^^^^^^^^^^^^^^^^^

The general rules about changelogs in the :ref:`Submitting patches guide
<describe_changes>`, apply.
(:ref:`Submitting patches guide
<describe_changes>` 中关于变更日志的一般规则同样适用。)

The tip tree maintainers set value on following these rules, especially on
the request to write changelogs in imperative mood and not impersonating
code or the execution of it. This is not just a whim of the
maintainers. Changelogs written in abstract words are more precise and
tend to be less confusing than those written in the form of novels.
(tip 树维护者非常看重遵守这些规则，尤其是要求用祈使语气撰写变更日志、不要把代码或其执行过程拟人化。这不仅仅是维护者的怪癖。用抽象语言写成的变更日志更精确，而且往往比小说式的变更日志更不容易让人困惑。)

It's also useful to structure the changelog into several paragraphs and not
lump everything together into a single one. A good structure is to explain
the context, the problem and the solution in separate paragraphs and this
order.
(把变更日志分成若干段落而不是把所有内容挤成一段也很有用。好的结构是按上下文、问题和解决方案的顺序分别用独立的段落来说明。)

Examples for illustration:
(用于说明的示例：)

  Example 1::

    x86/intel_rdt/mbm: Fix MBM overflow handler during hot cpu

    When a CPU is dying, we cancel the worker and schedule a new worker on a
    different CPU on the same domain. But if the timer is already about to
    expire (say 0.99s) then we essentially double the interval.

    We modify the hot cpu handling to cancel the delayed work on the dying
    cpu and run the worker immediately on a different cpu in same domain. We
    do not flush the worker because the MBM overflow worker reschedules the
    worker on same CPU and scans the domain->cpu_mask to get the domain
    pointer.

  (示例 1：改进前)

  Improved version::

    x86/intel_rdt/mbm: Fix MBM overflow handler during CPU hotplug

    When a CPU is dying, the overflow worker is canceled and rescheduled on a
    different CPU in the same domain. But if the timer is already about to
    expire this essentially doubles the interval which might result in a non
    detected overflow.

    Cancel the overflow worker and reschedule it immediately on a different CPU
    in the same domain. The work could be flushed as well, but that would
    reschedule it on the same CPU.

  (改进版本)

  Example 2::

    time: POSIX CPU timers: Ensure that variable is initialized

    If cpu_timer_sample_group returns -EINVAL, it will not have written into
    *sample. Checking for cpu_timer_sample_group's return value precludes the
    potential use of an uninitialized value of now in the following block.
    Given an invalid clock_idx, the previous code could otherwise overwrite
    *oldval in an undefined manner. This is now prevented. We also exploit
    short-circuiting of && to sample the timer only if the result will
    actually be used to update *oldval.

  (示例 2：改进前)

  Improved version::

    posix-cpu-timers: Make set_process_cpu_timer() more robust

    Because the return value of cpu_timer_sample_group() is not checked,
    compilers and static checkers can legitimately warn about a potential use
    of the uninitialized variable 'now'. This is not a runtime issue as all
    call sites hand in valid clock ids.

    Also cpu_timer_sample_group() is invoked unconditionally even when the
    result is not used because *oldval is NULL.

    Make the invocation conditional and check the return value.

  (改进版本)

  Example 3::

    The entity can also be used for other purposes.

    Let's rename it to be more generic.

  Improved version::

    The entity can also be used for other purposes.

    Rename it to be more generic.

  (示例 3：改进前与改进版本)


For complex scenarios, especially race conditions and memory ordering
issues, it is valuable to depict the scenario with a table which shows
the parallelism and the temporal order of events. Here is an example::

    CPU0                            CPU1
    free_irq(X)                     interrupt X
                                    spin_lock(desc->lock)
                                    wake irq thread()
                                    spin_unlock(desc->lock)
    spin_lock(desc->lock)
    remove action()
    shutdown_irq()
    release_resources()             thread_handler()
    spin_unlock(desc->lock)           access released resources.
                                      ^^^^^^^^^^^^^^^^^^^^^^^^^
    synchronize_irq()

(对于复杂场景，尤其是竞争条件与内存排序问题，用一张能展示事件并行关系与时间先后的表格来描绘场景很有价值。下面是一个示例：)

Lockdep provides similar useful output to depict a possible deadlock
scenario::

    CPU0                                    CPU1
    rtmutex_lock(&rcu->rt_mutex)
      spin_lock(&rcu->rt_mutex.wait_lock)
                                            local_irq_disable()
                                            spin_lock(&timer->it_lock)
                                            spin_lock(&rcu->mutex.wait_lock)
    --> Interrupt
        spin_lock(&timer->it_lock)


(Lockdep 也会输出类似的有用信息来描绘可能的死锁场景：)


Function references in changelogs(变更日志中的函数引用)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

When a function is mentioned in the changelog, either the text body or the
subject line, please use the format 'function_name()'. Omitting the
brackets after the function name can be ambiguous::

  Subject: subsys/component: Make reservation_count static

  reservation_count is only used in reservation_stats. Make it static.

(在变更日志中提到函数时，无论是正文还是主题行，请使用 'function_name()' 格式。省略函数名后面的括号可能造成歧义：)

The variant with brackets is more precise::

  Subject: subsys/component: Make reservation_count() static

  reservation_count() is only called from reservation_stats(). Make it
  static.

(带括号的形式更精确：)


Backtraces in changelogs(变更日志中的回溯)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

See :ref:`backtraces`.
(参见 :ref:`backtraces`。)

Ordering of commit tags(提交标签的顺序)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To have a uniform view of the commit tags, the tip maintainers use the
following tag ordering scheme:
(为了统一提交标签的呈现，tip 维护者使用以下标签排序方案：)

 - Fixes: 12+char-SHA1 ("sub/sys: Original subject line")
   (修复：12 位以上 SHA1（"sub/sys: 原主题行"）。)

   A Fixes tag should be added even for changes which do not need to be
   backported to stable kernels, i.e. when addressing a recently introduced
   issue which only affects tip or the current head of mainline. These tags
   are helpful to identify the original commit and are much more valuable
   than prominently mentioning the commit which introduced a problem in the
   text of the changelog itself because they can be automatically
   extracted.
   (即使是不需要回移到 stable 内核的更改，也应当加上 Fixes 标签，即修复只影响 tip 或主线当前顶端的新近引入问题时也应如此。这些标签有助于识别原始提交，比在变更日志正文中醒目地提及引入问题的提交要有价值得多，因为它们可以被自动提取。)

   The following example illustrates the difference::

     Commit

       abcdef012345678 ("x86/xxx: Replace foo with bar")

     left an unused instance of variable foo around. Remove it.

     Signed-off-by: J.Dev <j.dev@mail>

   (下面的例子说明了二者的差别：)

   Please say instead::

     The recent replacement of foo with bar left an unused instance of
     variable foo around. Remove it.

     Fixes: abcdef012345678 ("x86/xxx: Replace foo with bar")
     Signed-off-by: J.Dev <j.dev@mail>

   (请改成这样写：)

   The latter puts the information about the patch into the focus and
   amends it with the reference to the commit which introduced the issue
   rather than putting the focus on the original commit in the first place.
   (后一种写法把补丁本身的信息放在焦点位置，并附上引入问题的提交的引用，而不是一开始就把焦点放在原始提交上。)

 - Reported-by: ``Reporter <reporter@mail>``
   (报告者。)

 - Closes: ``URL or Message-ID of the bug report this is fixing``
   (所修复缺陷报告的 URL 或 Message-ID。)

 - Originally-by: ``Original author <original-author@mail>``
   (原作者。)

 - Suggested-by: ``Suggester <suggester@mail>``
   (建议者。)

 - Co-developed-by: ``Co-author <co-author@mail>``
   (共同作者。)

   Signed-off-by: ``Co-author <co-author@mail>``
   (共同作者的签收。)

   Note, that Co-developed-by and Signed-off-by of the co-author(s) must
   come in pairs.
   (注意，共同作者的 Co-developed-by 与 Signed-off-by 必须成对出现。)

 - Signed-off-by: ``Author <author@mail>``

   The first Signed-off-by (SOB) after the last Co-developed-by/SOB pair is the
   author SOB, i.e. the person flagged as author by git.
   (最后一个 Co-developed-by/SOB 对之后的第一条 Signed-off-by（SOB）是作者 SOB，即 git 标记为作者的人。)

 - Signed-off-by: ``Patch handler <handler@mail>``

   SOBs after the author SOB are from people handling and transporting
   the patch, but were not involved in development. SOB chains should
   reflect the **real** route a patch took as it was propagated to us,
   with the first SOB entry signalling primary authorship of a single
   author. Acks should be given as Acked-by lines and review approvals
   as Reviewed-by lines.
   (作者 SOB 之后的 SOB 来自处理和传递补丁但没有参与开发的人。SOB 链应当反映补丁传播到我们这里的真实路径，其中第一条 SOB 条目表明单个作者的主要作者身份。确认（Ack）应当写成 Acked-by 行，评审通过应当写成 Reviewed-by 行。)

   If the handler made modifications to the patch or the changelog, then
   this should be mentioned **after** the changelog text and **above**
   all commit tags in the following format::

     ... changelog text ends.

     [ handler: Replaced foo by bar and updated changelog ]

     First-tag: .....

   (如果处理者对补丁或变更日志做了修改，应当在变更日志正文之后、所有提交标签之上，按以下格式加以说明：)

   Note the two empty new lines which separate the changelog text and the
   commit tags from that notice.
   (注意分隔变更日志正文与提交标签以及该说明的两个空行。)

   If a patch is sent to the mailing list by a handler then the author has
   to be noted in the first line of the changelog with::

     From: Author <author@mail>

     Changelog text starts here....

   (如果补丁由处理者发送到邮件列表，则必须在变更日志的第一行注明作者，写法如下：)

   so the authorship is preserved. The 'From:' line has to be followed
   by an empty newline. If that 'From:' line is missing, then the patch
   would be attributed to the person who sent (transported, handled) it.
   The 'From:' line is automatically removed when the patch is applied
   and does not show up in the final git changelog. It merely affects
   the authorship information of the resulting Git commit.
   (这样作者身份得以保留。'From:' 行之后必须紧跟一个空行。如果缺少该 'From:' 行，补丁就会被归到发送（传递、处理）它的人名下。补丁被应用时 'From:' 行会被自动移除，不会出现在最终的 git 变更日志中，它只影响最终 Git 提交的作者信息。)

 - Tested-by: ``Tester <tester@mail>``
   (测试者。)

 - Reviewed-by: ``Reviewer <reviewer@mail>``
   (审阅者。)

 - Acked-by: ``Acker <acker@mail>``
   (确认者。)

 - Cc: ``cc-ed-person <person@mail>``
   (抄送对象。)

   If the patch should be backported to stable, then please add a '``Cc:
   stable@vger.kernel.org``' tag, but do not Cc stable when sending your
   mail.
   (如果补丁应当回移到 stable，请加上 Cc: stable@vger.kernel.org 标签，但发送邮件时不要抄送 stable。)

 - Link: ``https://link/to/information``
   (相关信息的链接。)

   For referring to an email posted to the kernel mailing lists, please
   use the lore.kernel.org redirector URL::

     Link: https://lore.kernel.org/email-message-id@here

   (引用发送到内核邮件列表的邮件时，请使用 lore.kernel.org 重定向 URL：)

   This URL should be used when referring to relevant mailing list
   topics, related patch sets, or other notable discussion threads.
   A convenient way to associate ``Link:`` trailers with the commit
   message is to use markdown-like bracketed notation, for example::

     A similar approach was attempted before as part of a different
     effort [1], but the initial implementation caused too many
     regressions [2], so it was backed out and reimplemented.

     Link: https://lore.kernel.org/some-msgid@here # [1]
     Link: https://bugzilla.example.org/bug/12345  # [2]

   (引用相关邮件列表话题、相关补丁集或其他值得注意的讨论串时，应使用该 URL。把 ``Link:`` 尾注与提交消息关联起来的一个便捷方式，是使用类似 markdown 的方括号记法，例如：)

   You can also use ``Link:`` trailers to indicate the origin of the
   patch when applying it to your git tree. In that case, please use the
   dedicated ``patch.msgid.link`` domain instead of ``lore.kernel.org``.
   This practice makes it possible for automated tooling to identify
   which link to use to retrieve the original patch submission. For
   example::

     Link: https://patch.msgid.link/patch-source-message-id@here

   (你也可以在把补丁应用进自己的 git 树时，用 ``Link:`` 尾注标明补丁来源。这种情况下，请使用专用的 ``patch.msgid.link`` 域名，而不是 ``lore.kernel.org``。这一做法让自动化工具能够识别应当用哪个链接获取原始补丁提交。例如：)

Please do not use combined tags, e.g. ``Reported-and-tested-by``, as
they just complicate automated extraction of tags.
(请不要使用组合标签，例如 ``Reported-and-tested-by`` 之类，它们只会让自动提取标签变得复杂。)


Links to documentation(文档链接)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Providing links to documentation in the changelog is a great help to later
debugging and analysis.  Unfortunately, URLs often break very quickly
because companies restructure their websites frequently.  Non-'volatile'
exceptions include the Intel SDM and the AMD APM.
(在变更日志中提供文档链接对事后的调试和分析很有帮助。遗憾的是，由于公司经常重构自己的网站，URL 往往很快就失效。非"易变（volatile）"的例外包括 Intel SDM 和 AMD APM。)

Therefore, for 'volatile' documents, please create an entry in the kernel
bugzilla https://bugzilla.kernel.org and attach a copy of these documents
to the bugzilla entry. Finally, provide the URL of the bugzilla entry in
the changelog.
(因此，对于"易变"的文档，请在内核 bugzilla https://bugzilla.kernel.org 上创建一个条目，并把这些文档的副本附加到该条目上。最后，在变更日志中提供该 bugzilla 条目的 URL。)

Patch resend or reminders(补丁重发或提醒)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

See :ref:`resend_reminders`.
(参见 :ref:`resend_reminders`。)

Merge window(合并窗口)
^^^^^^^^^^^^^^^^^^^^^^

Please do not expect patches to be reviewed or merged by tip
maintainers around or during the merge window.  The trees are closed
to all but urgent fixes during this time.  They reopen once the merge
window closes and a new -rc1 kernel has been released.
(在合并窗口前后或期间，请不要指望 tip 维护者会审查或合并补丁。在这段时间里，这些树对除紧急修复以外的一切关闭，直到合并窗口结束、新的 -rc1 内核发布后才重新开放。)

Large series should be submitted in mergeable state *at* *least* a week
before the merge window opens.  Exceptions are made for bug fixes and
*sometimes* for small standalone drivers for new hardware or minimally
invasive patches for hardware enablement.
(大型补丁系列应当在合并窗口开启 *至少* 提前一周、以可合并的状态提交。缺陷修复可以例外，小型的独立新硬件驱动或侵入性极小的硬件支持补丁 *有时* 也可以例外。)

During the merge window, the maintainers instead focus on following the
upstream changes, fixing merge window fallout, collecting bug fixes, and
allowing themselves a breath. Please respect that.
(在合并窗口期间，维护者会转而关注上游的变化、处理合并窗口的遗留问题、收集缺陷修复，并让自己喘口气。请尊重这一点。)

So called _urgent_ branches will be merged into mainline during the
stabilization phase of each release.
(所谓的紧急（urgent）分支会在每个版本的稳定阶段合并进主线。)


Git(Git)
^^^^^^^^

The tip maintainers accept git pull requests from maintainers who provide
subsystem changes for aggregation in the tip tree.
(对于提供子系统更改以聚合进 tip 树的维护者，tip 维护者接受其 git pull 请求。)

Pull requests for new patch submissions are usually not accepted and do not
replace proper patch submission to the mailing list. The main reason for
this is that the review workflow is email based.
(用于提交新补丁的 pull 请求通常不被接受，也不能替代向邮件列表的正式补丁提交，主要原因是评审流程基于邮件。)

If you submit a larger patch series it is helpful to provide a git branch
in a private repository which allows interested people to easily pull the
series for testing. The usual way to offer this is a git URL in the cover
letter of the patch series.
(如果你提交较大的补丁系列，在一个私有仓库中提供一个 git 分支会很有帮助，感兴趣的人可以方便地拉取该系列进行测试。通常的做法是在补丁系列的封面信（cover letter）中给出 git URL。)

Testing(测试)
^^^^^^^^^^^^^

Code should be tested before submitting to the tip maintainers.  Anything
other than minor changes should be built, booted and tested with
comprehensive (and heavyweight) kernel debugging options enabled.
(代码在提交给 tip 维护者之前应当经过测试。除细微更改以外的任何更改，都应当在开启全面（且重量级）内核调试选项的情况下完成构建、启动和测试。)

These debugging options can be found in kernel/configs/x86_debug.config
and can be added to an existing kernel config by running:
(这些调试选项位于 kernel/configs/x86_debug.config 中，可以通过以下命令添加到现有内核配置中：)

	make x86_debug.config

Some of these options are x86-specific and can be left out when testing
on other architectures.
(其中一些选项是 x86 特有的，在其他体系结构上测试时可以省略。)

.. _maintainer-tip-coding-style:

Coding style notes(编码风格备注)
--------------------------------

Comment style(注释风格)
^^^^^^^^^^^^^^^^^^^^^^^

Sentences in comments start with an uppercase letter.
(注释中的句子以大写字母开头。)

Single line comments::

	/* This is a single line comment */

(单行注释：)

Multi-line comments::

	/*
	 * This is a properly formatted
	 * multi-line comment.
	 *
	 * Larger multi-line comments should be split into paragraphs.
	 */

(多行注释：)

No tail comments (see below):
(不要使用尾部注释（见下文）：)

  Please refrain from using tail comments. Tail comments disturb the
  reading flow in almost all contexts, but especially in code::

	if (somecondition_is_true) /* Don't put a comment here */
		dostuff(); /* Neither here */

	seed = MAGIC_CONSTANT; /* Nor here */

  (请不要使用尾部注释。尾部注释在几乎所有上下文中都会干扰阅读流畅性，在代码中尤其如此：)

  Use freestanding comments instead::

	/* This condition is not obvious without a comment */
	if (somecondition_is_true) {
		/* This really needs to be documented */
		dostuff();
	}

	/* This magic initialization needs a comment. Maybe not? */
	seed = MAGIC_CONSTANT;

  (请改用独立注释：)

  Use C++ style, tail comments when documenting structs in headers to
  achieve a more compact layout and better readability::

        // eax
        u32     x2apic_shift    :  5, // Number of bits to shift APIC ID right
                                      // for the topology ID at the next level
                                : 27; // Reserved
        // ebx
        u32     num_processors  : 16, // Number of processors at current level
                                : 16; // Reserved

  (在头文件中为结构体成员编写文档时，使用 C++ 风格的尾部注释，可以获得更紧凑的排版和更好的可读性：)

  versus::

	/* eax */
	        /*
	         * Number of bits to shift APIC ID right for the topology ID
	         * at the next level
	         */
         u32     x2apic_shift    :  5,
		 /* Reserved */
				 : 27;

	/* ebx */
		/* Number of processors at current level */
	u32     num_processors  : 16,
			/* Reserved */
				: 16;

  (对比：)

Comment the important things:
(注释重要的内容：)

  Comments should be added where the operation is not obvious. Documenting
  the obvious is just a distraction::

	/* Decrement refcount and check for zero */
	if (refcount_dec_and_test(&p->refcnt)) {
		do;
		lots;
		of;
		magic;
		things;
	}

  (应当在操作不明显的地方添加注释。给显而易见的事情写文档只会让人分心：)

  Instead, comments should explain the non-obvious details and document
  constraints::

	if (refcount_dec_and_test(&p->refcnt)) {
		/*
		 * Really good explanation why the magic things below
		 * need to be done, ordering and locking constraints,
		 * etc..
		 */
		do;
		lots;
		of;
		magic;
		/* Needs to be the last operation because ... */
		things;
	}

  (相反，注释应当解释不明显的细节并记录约束条件：)

Function documentation comments:
(函数文档注释：)

  To document functions and their arguments please use kernel-doc format
  and not free form comments::

	/**
	 * magic_function - Do lots of magic stuff
	 * @magic:	Pointer to the magic data to operate on
	 * @offset:	Offset in the data array of @magic
	 *
	 * Deep explanation of mysterious things done with @magic along
         * with documentation of the return values.
	 *
	 * Note, that the argument descriptors above are arranged
	 * in a tabular fashion.
	 */

  (要为函数及其参数编写文档，请使用 kernel-doc 格式，而不是自由格式的注释：)

  This applies especially to globally visible functions and inline
  functions in public header files. It might be overkill to use kernel-doc
  format for every (static) function which needs a tiny explanation. The
  usage of descriptive function names often replaces these tiny comments.
  Apply common sense as always.
  (这一点尤其适用于全局可见的函数以及公共头文件中的内联函数。对于每个只需要一句话说明的（静态）函数都使用 kernel-doc 格式可能就过头了，描述性的函数名往往可以替代这些小注释。一如既往，请运用常识。)


Documenting locking requirements(记录锁需求)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  Documenting locking requirements is a good thing, but comments are not
  necessarily the best choice. Instead of writing::

	/* Caller must hold foo->lock */
	void func(struct foo *foo)
	{
		...
	}

  (记录锁需求是件好事，但注释未必是最佳选择。与其这样写：)

  Please use::

	void func(struct foo *foo)
	{
		lockdep_assert_held(&foo->lock);
		...
	}

  (请这样使用：)

  In PROVE_LOCKING kernels, lockdep_assert_held() emits a warning
  if the caller doesn't hold the lock.  Comments can't do that.
  (在启用 PROVE_LOCKING 的内核中，如果调用者不持有该锁，lockdep_assert_held() 会发出警告。注释做不到这一点。)

Bracket rules(花括号规则)
^^^^^^^^^^^^^^^^^^^^^^^^^

Brackets should be omitted only if the statement which follows 'if', 'for',
'while' etc. is truly a single line::

	if (foo)
		do_something();

(只有当 'if'、'for'、'while' 等之后紧跟的语句确实只有一行时，才可以省略花括号：)

The following is not considered to be a single line statement even
though C does not require brackets::

	for (i = 0; i < end; i++)
		if (foo[i])
			do_something(foo[i]);

(下面这种情况即使 C 不要求加括号，也不被视为单行语句：)

Adding brackets around the outer loop enhances the reading flow::

	for (i = 0; i < end; i++) {
		if (foo[i])
			do_something(foo[i]);
	}

(给外层循环加上花括号能提升阅读流畅性：)


Variable declarations(变量声明)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The preferred ordering of variable declarations at the beginning of a
function is reverse fir tree order::

	struct long_struct_name *descriptive_name;
	unsigned long foo, bar;
	unsigned int tmp;
	int ret;

(函数开头变量声明的首选顺序是倒 fir 树顺序（声明从长到短）：)

The above is faster to parse than the reverse ordering::

	int ret;
	unsigned int tmp;
	unsigned long foo, bar;
	struct long_struct_name *descriptive_name;

(上面的顺序比相反的顺序解析起来更快：)

And even more so than random ordering::

	unsigned long foo, bar;
	int ret;
	struct long_struct_name *descriptive_name;
	unsigned int tmp;

(比随机顺序就更胜一筹：)

Also please try to aggregate variables of the same type into a single
line. There is no point in wasting screen space::

	unsigned long a;
	unsigned long b;
	unsigned long c;
	unsigned long d;

(另外请尽量把同一类型的变量聚合到一行。没有理由浪费屏幕空间：)

It's really sufficient to do::

	unsigned long a, b, c, d;

(这样写就已经足够了：)

Please also refrain from introducing line splits in variable declarations::

	struct long_struct_name *descriptive_name = container_of(bar,
						      struct long_struct_name,
	                                              member);
	struct foobar foo;

(另外请不要在变量声明中引入折行：)

It's way better to move the initialization to a separate line after the
declarations::

	struct long_struct_name *descriptive_name;
	struct foobar foo;

	descriptive_name = container_of(bar, struct long_struct_name, member);

(把初始化移到声明之后的单独一行会好得多：)


Variable types(变量类型)
^^^^^^^^^^^^^^^^^^^^^^^^

Please use the proper u8, u16, u32, u64 types for variables which are meant
to describe hardware or are used as arguments for functions which access
hardware. These types are clearly defining the bit width and avoid
truncation, expansion and 32/64-bit confusion.
(对于用来描述硬件、或用作访问硬件的函数的参数的变量，请使用恰当的 u8、u16、u32、u64 类型。这些类型明确定义了位宽，可避免截断、扩展以及 32/64 位混淆。)

u64 is also recommended in code which would become ambiguous for 32-bit
kernels when 'unsigned long' would be used instead. While in such
situations 'unsigned long long' could be used as well, u64 is shorter
and also clearly shows that the operation is required to be 64 bits wide
independent of the target CPU.
(在改用 'unsigned long' 会让代码对 32 位内核产生歧义的场景下，也推荐使用 u64。虽然在这类情况下也可以用 'unsigned long long'，但 u64 更短，而且清楚地表明该操作必须是与目标 CPU 无关的 64 位宽。)

Please use 'unsigned int' instead of 'unsigned'.
(请使用 'unsigned int' 而不是 'unsigned'。)


Constants(常量)
^^^^^^^^^^^^^^^

Please do not use literal (hexa)decimal numbers in code or initializers.
Either use proper defines which have descriptive names or consider using
an enum.
(请不要在代码或初始化器中使用字面的（十六进制/十进制）数字。要么使用具有描述性名称的恰当宏定义，要么考虑使用枚举。)


Struct declarations and initializers(结构体声明与初始化器)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Struct declarations should align the struct member names in a tabular
fashion::

	struct bar_order {
		unsigned int	guest_id;
		int		ordered_item;
		struct menu	*menu;
	};

(结构体声明应当把成员名按表格形式对齐：)

Please avoid documenting struct members within the declaration, because
this often results in strangely formatted comments and the struct members
become obfuscated::

	struct bar_order {
		unsigned int	guest_id; /* Unique guest id */
		int		ordered_item;
		/* Pointer to a menu instance which contains all the drinks */
		struct menu	*menu;
	};

(请避免在结构体声明内部为成员编写文档，因为这常常导致注释格式怪异，结构体成员也会变得难以辨认：)

Instead, please consider using the kernel-doc format in a comment preceding
the struct declaration, which is easier to read and has the added advantage
of including the information in the kernel documentation, for example, as
follows::


	/**
	 * struct bar_order - Description of a bar order
	 * @guest_id:		Unique guest id
	 * @ordered_item:	The item number from the menu
	 * @menu:		Pointer to the menu from which the item
	 *  			was ordered
	 *
	 * Supplementary information for using the struct.
	 *
	 * Note, that the struct member descriptors above are arranged
	 * in a tabular fashion.
	 */
	struct bar_order {
		unsigned int	guest_id;
		int		ordered_item;
		struct menu	*menu;
	};

(相反，请考虑在结构体声明之前的注释中使用 kernel-doc 格式，它更易读，还有把这些信息收入内核文档的额外优点，例如如下所示：)

Static struct initializers must use C99 initializers and should also be
aligned in a tabular fashion::

	static struct foo statfoo = {
		.a		= 0,
		.plain_integer	= CONSTANT_DEFINE_OR_ENUM,
		.bar		= &statbar,
	};

(静态结构体初始化必须使用 C99 初始化器，并且同样应当按表格形式对齐：)

Note that while C99 syntax allows the omission of the final comma,
we recommend the use of a comma on the last line because it makes
reordering and addition of new lines easier, and makes such future
patches slightly easier to read as well.
(请注意，虽然 C99 语法允许省略最后一个逗号，但我们建议在最后一行保留逗号，因为这使得重新排序和添加新行更容易，也让这类将来的补丁略易读一些。)

Line breaks(换行)
^^^^^^^^^^^^^^^^^

Restricting line length to 80 characters makes deeply indented code hard to
read.  Consider breaking out code into helper functions to avoid excessive
line breaking.
(把行长度限制在 80 字符会让缩进很深的代码难以阅读。请考虑把代码拆分成辅助函数，以避免过多的折行。)

The 80 character rule is not a strict rule, so please use common sense when
breaking lines. Especially format strings should never be broken up.
(80 字符规则并不是死规矩，折行时请运用常识。尤其是格式字符串，绝不应该被拆开。)

When splitting function declarations or function calls, then please align
the first argument in the second line with the first argument in the first
line::

  static int long_function_name(struct foobar *barfoo, unsigned int id,
				unsigned int offset)
  {

	if (!id) {
		ret = longer_function_name(barfoo, DEFAULT_BARFOO_ID,
					   offset);
	...

(拆分函数声明或函数调用时，请把第二行的第一个参数与第一行的第一个参数对齐：)

Namespaces(命名空间)
^^^^^^^^^^^^^^^^^^^^

Function/variable namespaces improve readability and allow easy
grepping. These namespaces are string prefixes for globally visible
function and variable names, including inlines. These prefixes should
combine the subsystem and the component name such as 'x86_comp\_',
'sched\_', 'irq\_', and 'mutex\_'.
(函数/变量命名空间可以提高可读性并便于 grep。这些命名空间是全局可见的函数和变量名（包括内联函数）的字符串前缀。这些前缀应当把子系统名和组件名组合起来，例如 'x86_comp\_'、'sched\_'、'irq\_' 和 'mutex\_'。)

This also includes static file scope functions that are immediately put
into globally visible driver templates - it's useful for those symbols
to carry a good prefix as well, for backtrace readability.
(这也包括立即被放进全局可见驱动模板中的静态文件作用域函数——为了回溯的可读性，这些符号带上一个好的前缀同样有用。)

Namespace prefixes may be omitted for local static functions and
variables. Truly local functions, only called by other local functions,
can have shorter descriptive names - our primary concern is greppability
and backtrace readability.
(对于本地静态函数和变量，命名空间前缀可以省略。真正只在本地被其他本地函数调用的函数可以有更短描述性名称——我们最关心的是可 grep 性和回溯可读性。)

Please note that 'xxx_vendor\_' and 'vendor_xxx_` prefixes are not
helpful for static functions in vendor-specific files. After all, it
is already clear that the code is vendor-specific. In addition, vendor
names should only be for truly vendor-specific functionality.
(请注意，'xxx_vendor\_' 和 'vendor_xxx_` 前缀对厂商专用文件中的静态函数没有什么帮助——毕竟代码显然已经是厂商专用的了。此外，厂商名只应当用于真正厂商专用的功能。)

As always apply common sense and aim for consistency and readability.
(一如既往，请运用常识，力求一致性和可读性。)


Commit notifications(提交通知)
------------------------------

The tip tree is monitored by a bot for new commits. The bot sends an email
for each new commit to a dedicated mailing list
(``linux-tip-commits@vger.kernel.org``) and Cc's all people who are
mentioned in one of the commit tags. It uses the email message ID from the
Link tag at the end of the tag list to set the In-Reply-To email header so
the message is properly threaded with the patch submission email.
(tip 树由一个机器人监控新提交。机器人把每个新提交的邮件发送到一个专用邮件列表（``linux-tip-commits@vger.kernel.org``），并 Cc 所有在提交标签中被提到的人。它使用标签列表末尾 Link 标签中的邮件 Message-ID 来设置 In-Reply-To 邮件头，使该邮件能与补丁提交邮件正确串成线程。)

The tip maintainers and submaintainers try to reply to the submitter
when merging a patch, but they sometimes forget or it does not fit the
workflow of the moment. While the bot message is purely mechanical, it
also implies a 'Thank you! Applied.'.
(tip 维护者和子维护者在合并补丁时会尽量回复提交者，但他们有时会忘记，或者当时的工作流程不合适。虽然机器人邮件纯属机械性操作，但它也意味着一句 'Thank you! Applied.'（谢谢！已应用）。)
