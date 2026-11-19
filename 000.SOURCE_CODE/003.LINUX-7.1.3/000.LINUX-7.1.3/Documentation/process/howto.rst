.. _process_howto:

HOWTO do Linux kernel development(Linux 内核开发 HOWTO)
=======================================================

This is the be-all, end-all document on this topic.  It contains
instructions on how to become a Linux kernel developer and how to learn
to work with the Linux kernel development community.  It tries to not
contain anything related to the technical aspects of kernel programming,
but will help point you in the right direction for that.
(这是关于本主题最全面、最权威的文档。它包含如何成为 Linux 内核开发者、如何学会与 Linux 内核开发社区协作的指引。它尽量不涉及内核编程的技术细节，但会帮你在这方面找到正确的方向。)

If anything in this document becomes out of date, please send in patches
to the maintainer of this file, who is listed at the bottom of the
document.
(如果本文档中的任何内容过时了，请向本文件的维护者提交补丁，维护者列在文档末尾。)


Introduction(引言)
-------------------

So, you want to learn how to become a Linux kernel developer?  Or you
have been told by your manager, "Go write a Linux driver for this
device."  This document's goal is to teach you everything you need to
know to achieve this by describing the process you need to go through,
and hints on how to work with the community.  It will also try to
explain some of the reasons why the community works like it does.
(那么，你想学习如何成为 Linux 内核开发者？或者你的经理对你说："去给这个设备写一个 Linux 驱动。"本文档的目标是通过描述你需要经历的流程以及与社区协作的提示，教会你实现这一目标所需知道的一切。它还会尝试解释社区之所以这样运作的一些原因。)

The kernel is written mostly in C, with some architecture-dependent
parts written in assembly. A good understanding of C is required for
kernel development.  Assembly (any architecture) is not required unless
you plan to do low-level development for that architecture.  Though they
are not a good substitute for a solid C education and/or years of
experience, the following books are good for, if anything, reference:
(内核主要用 C 编写，部分架构相关的部分用汇编编写。内核开发要求对 C 有透彻的理解。除非你打算为某个架构做底层开发，否则不需要会汇编 (任何架构的)。以下书籍虽然不能很好地替代扎实的 C 语言教育和/或多年经验，但至少可以用来参考：)

 - "The C Programming Language" by Kernighan and Ritchie [Prentice Hall]
   (Kernighan 和 Ritchie 著《The C Programming Language》[Prentice Hall])
 - "Practical C Programming" by Steve Oualline [O'Reilly]
   (Steve Oualline 著《Practical C Programming》[O'Reilly])
 - "C:  A Reference Manual" by Harbison and Steele [Prentice Hall]
   (Harbison 和 Steele 著《C:  A Reference Manual》[Prentice Hall])

The kernel is written using GNU C and the GNU toolchain.  While it
adheres to the ISO C11 standard, it uses a number of extensions that are
not featured in the standard.  The kernel is a freestanding C
environment, with no reliance on the standard C library, so some
portions of the C standard are not supported.  Arbitrary long long
divisions and floating point are not allowed.  It can sometimes be
difficult to understand the assumptions the kernel has on the toolchain
and the extensions that it uses, and unfortunately there is no
definitive reference for them.  Please check the gcc info pages (`info
gcc`) for some information on them.
(内核使用 GNU C 和 GNU 工具链编写。虽然它遵循 ISO C11 标准，但使用了许多标准中没有的特性。内核是一个独立 (freestanding) 的 C 环境，不依赖标准 C 库，因此 C 标准的某些部分不被支持。任意 long long 除法和浮点运算都不允许。有时很难理解内核对工具链的假设以及它使用的扩展，遗憾的是也没有关于它们的权威参考。相关信息请查阅 gcc info 页面 (`info gcc`)。)

Please remember that you are trying to learn how to work with the
existing development community.  It is a diverse group of people, with
high standards for coding, style and procedure.  These standards have
been created over time based on what they have found to work best for
such a large and geographically dispersed team.  Try to learn as much as
possible about these standards ahead of time, as they are well
documented; do not expect people to adapt to you or your company's way
of doing things.
(请记住，你是在学习如何与现有的开发社区协作。这是一个多元化的人群，对编码、风格和流程都有很高的标准。这些标准经过长期积累而形成，基于他们对这样一个庞大且地理上分散的团队行之有效的经验。这些标准都有完善的文档，请提前尽可能多地了解它们；不要指望别人来适应你或你公司的做事方式。)


Legal Issues(法律问题)
----------------------

The Linux kernel source code is released under the GPL.  Please see the file
COPYING in the main directory of the source tree. The Linux kernel licensing
rules and how to use `SPDX <https://spdx.org/>`_ identifiers in source code are
described in :ref:`Documentation/process/license-rules.rst <kernel_licensing>`.
If you have further questions about the license, please contact a lawyer, and do
not ask on the Linux kernel mailing list.  The people on the mailing lists are
not lawyers, and you should not rely on their statements on legal matters.
(Linux 内核源代码以 GPL 发布。请查看源码树主目录下的 COPYING 文件。Linux 内核的许可规则以及在源代码中使用 `SPDX <https://spdx.org/>`_ 标识符的方法在 :ref:`Documentation/process/license-rules.rst <kernel_licensing>` 中描述。如果你对许可证还有疑问，请咨询律师，不要在 Linux 内核邮件列表上提问。邮件列表上的人不是律师，你不应依赖他们在法律问题上的说法。)

For common questions and answers about the GPL, please see:

	https://www.gnu.org/licenses/gpl-faq.html

(有关 GPL 的常见问答见：)


Documentation(文档)
-------------------

The Linux kernel source tree has a large range of documents that are
invaluable for learning how to interact with the kernel community.  When
new features are added to the kernel, it is recommended that new
documentation files are also added which explain how to use the feature.
When a kernel change causes the interface that the kernel exposes to
userspace to change, it is recommended that you send the information or
a patch to the manual pages explaining the change to the manual pages
maintainer at alx@kernel.org, and CC the list linux-api@vger.kernel.org.
(Linux 内核源码树中有大量文档，对学习如何与内核社区交互极其宝贵。向内核添加新特性时，建议同时添加解释如何使用该特性的新文档文件。当内核修改导致内核暴露给用户态的接口发生变化时，建议你把相关信息或解释该变更的手册页补丁发送给手册页维护者 alx@kernel.org，并抄送 linux-api@vger.kernel.org 列表。)

Here is a list of files that are in the kernel source tree that are
required reading:
(以下是内核源码树中必读文件的清单：)

  :ref:`Documentation/admin-guide/README.rst <readme>`
    This file gives a short background on the Linux kernel and describes
    what is necessary to do to configure and build the kernel.  People
    who are new to the kernel should start here.
    (本文件简要介绍 Linux 内核的背景，并描述配置和构建内核需要做的事情。内核新手应从这里开始。)

  :ref:`Documentation/process/changes.rst <changes>`
    This file gives a list of the minimum levels of various software
    packages that are necessary to build and run the kernel
    successfully.
    (本文件列出了成功构建和运行内核所需的各种软件包的最低版本。)

  :ref:`Documentation/process/coding-style.rst <codingstyle>`
    This describes the Linux kernel coding style, and some of the
    rationale behind it. All new code is expected to follow the
    guidelines in this document. Most maintainers will only accept
    patches if these rules are followed, and many people will only
    review code if it is in the proper style.
    (本文件描述 Linux 内核编码风格及其背后的一些理由。所有新代码都应遵循本文档中的准则。大多数维护者只有在遵循这些规则时才接受补丁，很多人只有在代码风格正确时才会审查代码。)

  :ref:`Documentation/process/submitting-patches.rst <submittingpatches>`
    This file describes in explicit detail how to successfully create
    and send a patch, including (but not limited to):
    (本文件详细明确地描述了如何成功地创建和发送补丁，包括 (但不限于)：)

       - Email contents
       - Email format
       - Who to send it to
       (邮件内容、邮件格式、发送给谁)

    Following these rules will not guarantee success (as all patches are
    subject to scrutiny for content and style), but not following them
    will almost always prevent it.
    (遵循这些规则并不能保证成功 (所有补丁都要经过内容与风格的审查)，但不遵循它们几乎总是会导致失败。)

    Other excellent descriptions of how to create patches properly are:

	"The Perfect Patch"
	(《完美的补丁》)
		https://www.ozlabs.org/~akpm/stuff/tpp.txt

	"Linux kernel patch submission format"
	(《Linux 内核补丁提交格式》)
		https://web.archive.org/web/20180829112450/http://linux.yyz.us/patch-format.html

  :ref:`Documentation/process/stable-api-nonsense.rst <stable_api_nonsense>`
    This file describes the rationale behind the conscious decision to
    not have a stable API within the kernel, including things like:
    (本文件描述内核有意不提供稳定 API 的理由，包括诸如：)

      - Subsystem shim-layers (for compatibility?)
      - Driver portability between Operating Systems.
      - Mitigating rapid change within the kernel source tree (or
	preventing rapid change)
      (子系统垫层 (为了兼容性？)、驱动程序在操作系统之间的可移植性、缓解 (或阻止) 内核源码树的快速变化。)

    This document is crucial for understanding the Linux development
    philosophy and is very important for people moving to Linux from
    development on other Operating Systems.
    (本文档对理解 Linux 开发哲学至关重要，对从其他操作系统开发转向 Linux 的人尤其重要。)

  :ref:`Documentation/process/security-bugs.rst <securitybugs>`
    If you feel you have found a security problem in the Linux kernel,
    please follow the steps in this document to help notify the kernel
    developers, and help solve the issue.
    (如果你认为自己在 Linux 内核中发现了一个安全问题，请按本文档中的步骤通知内核开发者，并帮助解决问题。)

  :ref:`Documentation/process/management-style.rst <managementstyle>`
    This document describes how Linux kernel maintainers operate and the
    shared ethos behind their methodologies.  This is important reading
    for anyone new to kernel development (or anyone simply curious about
    it), as it resolves a lot of common misconceptions and confusion
    about the unique behavior of kernel maintainers.
    (本文档描述 Linux 内核维护者的行事方式以及其方法论背后的共同精神。对任何内核开发新手 (或单纯好奇的人) 来说，这都是必读材料，因为它澄清了关于内核维护者独特行为的许多常见误解和困惑。)

  :ref:`Documentation/process/stable-kernel-rules.rst <stable_kernel_rules>`
    This file describes the rules on how the stable kernel releases
    happen, and what to do if you want to get a change into one of these
    releases.
    (本文件描述稳定内核发布的规则，以及想把修改纳入其中一个发布时该怎么做。)

  :ref:`Documentation/process/kernel-docs.rst <kernel_docs>`
    A list of external documentation that pertains to kernel
    development.  Please consult this list if you do not find what you
    are looking for within the in-kernel documentation.
    (与内核开发相关的外部文档清单。如果在内核文档中找不到你要找的东西，请查阅此清单。)

  :ref:`Documentation/process/applying-patches.rst <applying_patches>`
    A good introduction describing exactly what a patch is and how to
    apply it to the different development branches of the kernel.
    (一篇很好的介绍，准确说明补丁是什么以及如何把它应用到内核的不同开发分支。)

The kernel also has a large number of documents that can be
automatically generated from the source code itself or from
ReStructuredText markups (ReST), like this one. This includes a
full description of the in-kernel API, and rules on how to handle
locking properly.
(内核还有大量文档可以从源代码本身或 ReStructuredText 标记 (ReST) 自动生成，比如本文档。其中包括内核内 API 的完整描述，以及如何正确处理加锁的规则。)

All such documents can be generated as PDF or HTML by running::

	make pdfdocs
	make htmldocs

(所有这些文档都可以生成为 PDF 或 HTML：)

respectively from the main kernel source directory.
(分别要在内核源码主目录下运行。)

The documents that uses ReST markup will be generated at Documentation/output.
They can also be generated on LaTeX and ePub formats with::

	make latexdocs
	make epubdocs

(使用 ReST 标记的文档将生成到 Documentation/output。它们还可以生成为 LaTeX 和 ePub 格式：)

Becoming A Kernel Developer(成为内核开发者)
--------------------------------------------

If you do not know anything about Linux kernel development, you should
look at the Linux KernelNewbies project:

	https://kernelnewbies.org

(如果你对 Linux 内核开发一无所知，可以看看 Linux KernelNewbies 项目：)

It consists of a helpful mailing list where you can ask almost any type
of basic kernel development question (make sure to search the archives
first, before asking something that has already been answered in the
past.)  It also has an IRC channel that you can use to ask questions in
real-time, and a lot of helpful documentation that is useful for
learning about Linux kernel development.
(它有一个实用的邮件列表，你可以在上面提几乎任何类型的内核开发基础问题 (提问前请先搜索归档，不要问过去已经被回答过的问题)。它还有一个可以实时提问的 IRC 频道，以及大量对学习 Linux 内核开发有帮助的文档。)

The website has basic information about code organization, subsystems,
and current projects (both in-tree and out-of-tree). It also describes
some basic logistical information, like how to compile a kernel and
apply a patch.
(该网站有关于代码组织、子系统和当前项目 (树内与树外) 的基本信息。它还描述了一些基本的操作信息，比如如何编译内核和应用补丁。)

If you do not know where you want to start, but you want to look for
some task to start doing to join into the kernel development community,
go to the Linux Kernel Janitor's project:

	https://kernelnewbies.org/KernelJanitors

(如果你不知道想从哪里入手，但想找一些任务来做、以便加入内核开发社区，可以去 Linux Kernel Janitor's 项目：)

It is a great place to start.  It describes a list of relatively simple
problems that need to be cleaned up and fixed within the Linux kernel
source tree.  Working with the developers in charge of this project, you
will learn the basics of getting your patch into the Linux kernel tree,
and possibly be pointed in the direction of what to go work on next, if
you do not already have an idea.
(这是一个很好的起点。它列出了 Linux 内核源码树中需要清理和修复的相对简单的问题。与负责该项目的开发者合作，你将学到让补丁进入 Linux 内核树的基础知识；如果你还没有头绪，他们也许还会为你指明接下来可以做什么。)

Before making any actual modifications to the Linux kernel code, it is
imperative to understand how the code in question works.  For this
purpose, nothing is better than reading through it directly (most tricky
bits are commented well), perhaps even with the help of specialized
tools.  One such tool that is particularly recommended is the Linux
Cross-Reference project, which is able to present source code in a
self-referential, indexed webpage format. An excellent up-to-date
repository of the kernel code may be found at:

	https://elixir.bootlin.com/

(在对 Linux 内核代码做任何实际修改之前，必须先理解相关代码是如何工作的。为此，最好的办法莫过于直接通读代码 (大多数棘手之处都有良好的注释)，必要时还可以借助专门的工具。特别推荐的一个工具是 Linux Cross-Reference 项目，它能够以自引用、带索引的网页形式呈现源代码。一个极佳且保持更新的内核代码仓库见：)


The development process(开发流程)
---------------------------------

Linux kernel development process currently consists of a few different
main kernel "branches" and lots of different subsystem-specific kernel
branches.  These different branches are:
(Linux 内核开发流程目前由若干不同的主内核 "branch" 和大量子系统专用内核 branch 组成。这些不同的 branch 包括：)

  - Linus's mainline tree
    (Linus 的主线树)
  - Various stable trees with multiple major numbers
    (多个主版本号的各种稳定树)
  - Subsystem-specific trees
    (子系统专用树)
  - linux-next integration testing tree
    (linux-next 集成测试树)

Mainline tree(主线树)
~~~~~~~~~~~~~~~~~~~~~

The mainline tree is maintained by Linus Torvalds, and can be found at
https://kernel.org or in the repo.  Its development process is as follows:
(主线树由 Linus Torvalds 维护，见 https://kernel.org 或 repo。其开发流程如下：)

  - As soon as a new kernel is released a two week window is open,
    during this period of time maintainers can submit big diffs to
    Linus, usually the patches that have already been included in the
    linux-next for a few weeks.  The preferred way to submit big changes
    is using git (the kernel's source management tool, more information
    can be found at https://git-scm.com/) but plain patches are also just
    fine.
    (一旦新内核发布，就会开启一个为期两周的合并窗口，在此期间维护者可以向 Linus 提交大改动，通常是已经进入 linux-next 几周的补丁。提交大改动的首选方式是使用 git (内核的源码管理工具，更多信息见 https://git-scm.com/)，不过普通补丁也完全可以。)
  - After two weeks a -rc1 kernel is released and the focus is on making the
    new kernel as rock solid as possible.  Most of the patches at this point
    should fix a regression.  Bugs that have always existed are not
    regressions, so only push these kinds of fixes if they are important.
    Please note that a whole new driver (or filesystem) might be accepted
    after -rc1 because there is no risk of causing regressions with such a
    change as long as the change is self-contained and does not affect areas
    outside of the code that is being added.  git can be used to send
    patches to Linus after -rc1 is released, but the patches need to also be
    sent to a public mailing list for review.
    (两周之后发布 -rc1 内核，重点是把新内核做得尽可能稳固。此阶段的补丁大多应当修复回归。一直存在的 bug 不算回归，因此只有重要的才推送这类修复。请注意，-rc1 之后仍可能接受全新的驱动 (或文件系统)，因为只要改动自成一体、不影响所添加代码之外的区域，这类改动就没有造成回归的风险。-rc1 发布之后仍可用 git 向 Linus 发送补丁，但补丁也必须同时发送到公开邮件列表接受评审。)
  - A new -rc is released whenever Linus deems the current git tree to
    be in a reasonably sane state adequate for testing.  The goal is to
    release a new -rc kernel every week.
    (只要 Linus 认为当前 git 树处于足够健全、适合测试的状态，就发布一个新的 -rc。目标是每周发布一个新的 -rc 内核。)
  - Process continues until the kernel is considered "ready", the
    process should last around 6 weeks.
    (这一过程持续到内核被认为 "就绪" 为止，整个过程大约应持续 6 周。)

It is worth mentioning what Andrew Morton wrote on the linux-kernel
mailing list about kernel releases:
(值得一提的是 Andrew Morton 在 linux-kernel 邮件列表上关于内核发布的一段话：)

	*"Nobody knows when a kernel will be released, because it's
	released according to perceived bug status, not according to a
	preconceived timeline."*

("没人知道内核什么时候会发布，因为它是按照感知到的 bug 状态发布的，而不是按照预定的时间表。")

Various stable trees with multiple major numbers(多个主版本号的各种稳定树)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Kernels with 3-part versions are -stable kernels. They contain
relatively small and critical fixes for security problems or significant
regressions discovered in a given major mainline release. Each release
in a major stable series increments the third part of the version
number, keeping the first two parts the same.
(3 段式版本号的内核是 -stable 内核。它们包含针对某个主主线版本中发现的安全问题或重大回归的相对较小且关键的修复。主稳定系列中的每个发布都会递增版本号的第三段，前两段保持不变。)

This is the recommended branch for users who want the most recent stable
kernel and are not interested in helping test development/experimental
versions.
(对于想要最新稳定内核、不打算协助测试开发/实验版本的用户，推荐使用这个分支。)

Stable trees are maintained by the "stable" team <stable@vger.kernel.org>, and
are released as needs dictate.  The normal release period is approximately
two weeks, but it can be longer if there are no pressing problems.  A
security-related problem, instead, can cause a release to happen almost
instantly.
(稳定树由 "stable" 团队 <stable@vger.kernel.org> 维护，按需发布。正常发布周期约为两周，如果没有紧迫问题也可能更长。而安全相关问题则可能让发布几乎立即发生。)

The file :ref:`Documentation/process/stable-kernel-rules.rst <stable_kernel_rules>`
in the kernel tree documents what kinds of changes are acceptable for
the -stable tree, and how the release process works.
(内核树中的 :ref:`Documentation/process/stable-kernel-rules.rst <stable_kernel_rules>` 文档说明了哪些改动可以被 -stable 树接受，以及发布流程如何运作。)

Subsystem-specific trees(子系统专用树)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The maintainers of the various kernel subsystems --- and also many
kernel subsystem developers --- expose their current state of
development in source repositories.  That way, others can see what is
happening in the different areas of the kernel.  In areas where
development is rapid, a developer may be asked to base his submissions
onto such a subsystem kernel tree so that conflicts between the
submission and other already ongoing work are avoided.
(各个内核子系统的维护者——以及许多子系统开发者——会把他们当前的开发状态公开在源码仓库中。这样，其他人就能看到内核不同领域正在发生的事情。在开发迅速的领域，开发者可能被要求把提交基于这样的子系统内核树，以避免提交与其他正在进行的工作发生冲突。)

Most of these repositories are git trees, but there are also other SCMs
in use, or patch queues being published as quilt series.  Addresses of
these subsystem repositories are listed in the MAINTAINERS file.  Many
of them can be browsed at https://git.kernel.org/.
(这些仓库大多是 git 树，但也有使用其他 SCM 的，或者以 quilt series 形式发布的补丁队列。这些子系统仓库的地址列在 MAINTAINERS 文件中。其中许多可以在 https://git.kernel.org/ 浏览。)

Before a proposed patch is committed to such a subsystem tree, it is
subject to review which primarily happens on mailing lists (see the
respective section below).  For several kernel subsystems, this review
process is tracked with the tool patchwork.  Patchwork offers a web
interface which shows patch postings, any comments on a patch or
revisions to it, and maintainers can mark patches as under review,
accepted, or rejected.  Most of these patchwork sites are listed at
https://patchwork.kernel.org/.
(在拟议的补丁提交到这样的子系统树之前，它要接受评审，评审主要在邮件列表上进行 (见下文相应小节)。对于一些内核子系统，这个评审过程用 patchwork 工具跟踪。Patchwork 提供一个网页界面，显示补丁提交、对补丁的评论或其修订版本，维护者可以把补丁标记为评审中、已接受或已拒绝。这些 patchwork 站点大多列在 https://patchwork.kernel.org/。)

linux-next integration testing tree(linux-next 集成测试树)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Before updates from subsystem trees are merged into the mainline tree,
they need to be integration-tested.  For this purpose, a special
testing repository exists into which virtually all subsystem trees are
pulled on an almost daily basis:

	https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git

(在子系统树的更新合并进主线树之前，需要先做集成测试。为此存在一个专门的测试仓库，几乎所有子系统树都会以几乎每日一次的频率拉取进它：)

This way, the linux-next gives a summary outlook onto what will be
expected to go into the mainline kernel at the next merge period.
Adventurous testers are very welcome to runtime-test the linux-next.
(这样，linux-next 概括了下一合并周期预计将进入主线内核的内容。非常欢迎大胆的测试者对 linux-next 做运行时测试。)


Bug Reporting(错误报告)
-----------------------

The file 'Documentation/admin-guide/reporting-issues.rst' in the main kernel
source directory describes how to report a possible kernel bug, and details
what kind of information is needed by the kernel developers to help track
down the problem.
(主内核源码目录中的 'Documentation/admin-guide/reporting-issues.rst' 文件描述了如何报告可能的内核 bug，并详细说明内核开发者需要什么样的信息来帮助定位问题。)


Managing bug reports(管理错误报告)
----------------------------------

One of the best ways to put into practice your hacking skills is by fixing
bugs reported by other people. Not only will you help to make the kernel
more stable, but you'll also learn to fix real-world problems and you will
improve your skills, and other developers will be aware of your presence.
Fixing bugs is one of the best ways to get merits among other developers,
because not many people like wasting time fixing other people's bugs.
(把你的 hack 技能付诸实践的最好方式之一，就是修复别人报告的 bug。你不仅能让内核更稳定，还能学到如何解决现实世界的问题、提升自己的技能，并让其他开发者注意到你的存在。修复 bug 是在其他开发者中赢得声誉的最好方式之一，因为没多少人喜欢把时间浪费在修复别人的 bug 上。)

To work on already reported bug reports, find a subsystem you are interested in.
Check the MAINTAINERS file where bugs for that subsystem get reported to; often
it will be a mailing list, rarely a bugtracker. Search the archives of said
place for recent reports and help where you see fit. You may also want to check
https://bugzilla.kernel.org for bug reports; only a handful of kernel subsystems
use it actively for reporting or tracking, nevertheless bugs for the whole
kernel get filed there.
(要处理已报告的 bug，先找一个你感兴趣的子系统。查看 MAINTAINERS 文件了解该子系统的 bug 报告到哪里；通常是邮件列表，很少是 bugtracker。在该处的归档中搜索最近的报告，并在你认为合适的地方提供帮助。你也可以查看 https://bugzilla.kernel.org 上的 bug 报告；只有少数内核子系统积极使用它来报告或跟踪，但整个内核的 bug 都会登记在那里。)


Mailing lists(邮件列表)
-----------------------

As some of the above documents describe, the majority of the core kernel
developers participate on the Linux Kernel Mailing list.  Details on how
to subscribe and unsubscribe from the list can be found at:

	https://subspace.kernel.org/subscribing.html

(正如上文一些文档所述，大多数核心内核开发者都参与 Linux Kernel Mailing list。订阅与退订的详情见：)

There are archives of the mailing list on the web in many different
places.  Use a search engine to find these archives.  For example:

	https://lore.kernel.org/linux-kernel/

(邮件列表的归档在网络上多处都有。用搜索引擎可以找到这些归档。例如：)

It is highly recommended that you search the archives about the topic
you want to bring up, before you post it to the list. A lot of things
already discussed in detail are only recorded at the mailing list
archives.
(强烈建议在你把某个话题发到列表之前，先搜索相关归档。很多已被详细讨论过的内容只记录在邮件列表归档中。)

Most of the individual kernel subsystems also have their own separate
mailing list where they do their development efforts.  See the
MAINTAINERS file for a list of what these lists are for the different
groups.
(大多数单个内核子系统也有自己独立的邮件列表用于开展开发工作。不同群体的这些列表清单见 MAINTAINERS 文件。)

Many of the lists are hosted on kernel.org. Information on them can be
found at:

	https://subspace.kernel.org

(许多列表托管在 kernel.org 上。相关信息见：)

Please remember to follow good behavioral habits when using the lists.
Though a bit cheesy, the following URL has some simple guidelines for
interacting with the list (or any list):

	https://subspace.kernel.org/etiquette.html

(使用这些列表时请记住遵守良好的行为习惯。下面这个 URL 虽然有点老套，但给出了一些与列表 (或任何列表) 交互的简单准则：)

If multiple people respond to your mail, the CC: list of recipients may
get pretty large. Don't remove anybody from the CC: list without a good
reason, or don't reply only to the list address. Get used to receiving the
mail twice, one from the sender and the one from the list, and don't try
to tune that by adding fancy mail-headers, people will not like it.
(如果多人回复你的邮件，收件人的 CC: 列表可能变得相当大。没有充分理由不要把任何人从 CC: 列表移除，也不要只回复列表地址。要习惯收到两份邮件——一份来自发件人、一份来自列表——不要试图通过添加花哨的邮件头来改变这一点，人们不会喜欢。)

Remember to keep the context and the attribution of your replies intact,
keep the "John Kernelhacker wrote ...:" lines at the top of your reply, and
add your statements between the individual quoted sections instead of
writing at the top of the mail.
(记住保持回复中的上下文与署名完整，把 "John Kernelhacker wrote ...:" 这样的行保留在回复顶部，并把你的陈述加在各个引用段落之间，而不是写在邮件顶部。)

If you add patches to your mail, make sure they are plain readable text
as stated in :ref:`Documentation/process/submitting-patches.rst <submittingpatches>`.
Kernel developers don't want to deal with
attachments or compressed patches; they may want to comment on
individual lines of your patch, which works only that way. Make sure you
use a mail program that does not mangle spaces and tab characters. A
good first test is to send the mail to yourself and try to apply your
own patch by yourself. If that doesn't work, get your mail program fixed
or change it until it works.
(如果你在邮件中附上补丁，确保它们是 :ref:`Documentation/process/submitting-patches.rst <submittingpatches>` 所述的纯可读文本。内核开发者不想处理附件或压缩补丁；他们可能想对补丁的单独某行发表评论，只有纯文本才行。确保你使用的邮件程序不会破坏空格和 tab 字符。一个好的初步测试是把邮件发给自己，然后尝试亲手应用你自己的补丁。如果不行，就修好或换掉你的邮件程序，直到能用为止。)

Above all, please remember to show respect to other subscribers.
(最重要的是，请记住尊重其他订阅者。)


Working with the community(与社区协作)
--------------------------------------

The goal of the kernel community is to provide the best possible kernel
there is.  When you submit a patch for acceptance, it will be reviewed
on its technical merits and those alone.  So, what should you be
expecting?
(内核社区的目标是提供尽可能最好的内核。当你提交补丁请求接受时，评审将只看它的技术价值，也仅看技术价值。那么，你应当期待什么？)

  - criticism
  - comments
  - requests for change
  - requests for justification
  - silence
  (批评、评论、修改要求、说明理由的要求、沉默)

Remember, this is part of getting your patch into the kernel.  You have
to be able to take criticism and comments about your patches, evaluate
them at a technical level and either rework your patches or provide
clear and concise reasoning as to why those changes should not be made.
If there are no responses to your posting, wait a few days and try
again, sometimes things get lost in the huge volume.
(记住，这是让补丁进入内核过程的一部分。你必须能够接受关于补丁的批评和评论，从技术层面评估它们，然后要么重做补丁，要么给出清晰简洁的理由说明为什么不应做这些修改。如果你的帖子没有回应，等几天再试，海量邮件中有时确实会漏掉。)

What should you not do?
(你不应该做什么？)

  - expect your patch to be accepted without question
  - become defensive
  - ignore comments
  - resubmit the patch without making any of the requested changes
  (指望补丁不加质疑就被接受、变得防御心重、无视评论、不做任何要求的修改就重新提交补丁)

In a community that is looking for the best technical solution possible,
there will always be differing opinions on how beneficial a patch is.
You have to be cooperative, and willing to adapt your idea to fit within
the kernel.  Or at least be willing to prove your idea is worth it.
Remember, being wrong is acceptable as long as you are willing to work
toward a solution that is right.
(在一个寻求最佳技术方案的社区里，对补丁有多大好处总会有不同意见。你必须乐于合作，愿意调整自己的想法以适应内核，或者至少愿意证明你的想法是值得的。记住，犯错是可以接受的，只要你愿意朝着正确的解决方案努力。)

It is normal that the answers to your first patch might simply be a list
of a dozen things you should correct.  This does **not** imply that your
patch will not be accepted, and it is **not** meant against you
personally.  Simply correct all issues raised against your patch and
resend it.
(你的第一个补丁得到的回应可能只是十几条需要修正的问题清单，这很正常。这 **并不** 意味着你的补丁不会被接受，也 **不是** 针对你个人。把针对补丁提出的问题全部修正，然后重新发送即可。)


Differences between the kernel community and corporate structures(内核社区与公司结构的差异)
-------------------------------------------------------------------------------------------

The kernel community works differently than most traditional corporate
development environments.  Here are a list of things that you can try to
do to avoid problems:
(内核社区的运作方式与大多数传统公司开发环境不同。以下是一份可以尝试去做以避免问题的清单：)

  Good things to say regarding your proposed changes:
  (关于你提议的修改，可以说的话：)

    - "This solves multiple problems."
    - "This deletes 2000 lines of code."
    - "Here is a patch that explains what I am trying to describe."
    - "I tested it on 5 different architectures..."
    - "Here is a series of small patches that..."
    - "This increases performance on typical machines..."
    ("这能解决多个问题。"、"这删掉了 2000 行代码。"、"这里有一个补丁，解释我想描述的东西。"、"我在 5 种不同架构上测试过……"、"这里是一系列小补丁……"、"这能提升典型机器上的性能……")

  Bad things you should avoid saying:
  (应当避免说的话：)

    - "We did it this way in AIX/ptx/Solaris, so therefore it must be
      good..."
    - "I've being doing this for 20 years, so..."
    - "This is required for my company to make money"
    - "This is for our Enterprise product line."
    - "Here is my 1000 page design document that describes my idea"
    - "I've been working on this for 6 months..."
    - "Here's a 5000 line patch that..."
    - "I rewrote all of the current mess, and here it is..."
    - "I have a deadline, and this patch needs to be applied now."
    ("我们在 AIX/ptx/Solaris 上就是这么做的，所以一定好……"、"我干这行 20 年了，所以……"、"这是我公司赚钱所必需的"、"这是给我们的企业产品线的"、"这是我 1000 页的设计文档，描述了我的想法"、"我已经在这上面干了 6 个月……"、"这是一个 5000 行的补丁……"、"我把现在这堆乱摊子全部重写了，就在这里……"、"我有截止日期，这个补丁现在必须被合入。")

Another way the kernel community is different than most traditional
software engineering work environments is the faceless nature of
interaction.  One benefit of using email and irc as the primary forms of
communication is the lack of discrimination based on gender or race.
The Linux kernel work environment is accepting of women and minorities
because all you are is an email address.  The international aspect also
helps to level the playing field because you can't guess gender based on
a person's name. A man may be named Andrea and a woman may be named Pat.
Most women who have worked in the Linux kernel and have expressed an
opinion have had positive experiences.
(内核社区与大多数传统软件工程工作环境不同的另一点，是交互的匿名性。以电子邮件和 irc 为主要沟通方式的一个好处，是不会基于性别或种族产生歧视。Linux 内核的工作环境对女性和少数群体是接纳的，因为你只是一个电子邮件地址。国际化的一面也有助于公平竞争，因为你无法从名字猜出性别——男性可能叫 Andrea，女性可能叫 Pat。在 Linux 内核中工作过并表达过意见的女性大多都有积极的经历。)

The language barrier can cause problems for some people who are not
comfortable with English.  A good grasp of the language can be needed in
order to get ideas across properly on mailing lists, so it is
recommended that you check your emails to make sure they make sense in
English before sending them.
(语言障碍会给一些不习惯英语的人带来困难。要在邮件列表上恰当地表达自己的想法，可能需要对语言有良好的掌握，因此建议你在发送邮件之前先检查一遍，确保它们用英语读起来是通顺的。)


Break up your changes(拆分你的修改)
-----------------------------------

The Linux kernel community does not gladly accept large chunks of code
dropped on it all at once.  The changes need to be properly introduced,
discussed, and broken up into tiny, individual portions.  This is almost
the exact opposite of what companies are used to doing.  Your proposal
should also be introduced very early in the development process, so that
you can receive feedback on what you are doing.  It also lets the
community feel that you are working with them, and not simply using them
as a dumping ground for your feature.  However, don't send 50 emails at
one time to a mailing list, your patch series should be smaller than
that almost all of the time.
(Linux 内核社区不乐于接受一次性倾倒过来的大块代码。修改需要被恰当地介绍、讨论，并拆分成微小的独立部分。这几乎与公司习惯的做法完全相反。你的提案还应当在开发流程的极早期就提出来，这样你能收到关于你正在做的事情的反馈。这也让社区感觉你是在与他们协作，而不是简单地把他们当作你功能的倾倒场。不过，不要一次向邮件列表发 50 封邮件，你的补丁系列几乎在所有情况下都应当比这更小。)

The reasons for breaking things up are the following:
(拆分修改的理由如下：)

1) Small patches increase the likelihood that your patches will be
   applied, since they don't take much time or effort to verify for
   correctness.  A 5 line patch can be applied by a maintainer with
   barely a second glance. However, a 500 line patch may take hours to
   review for correctness (the time it takes is exponentially
   proportional to the size of the patch, or something).
   (小补丁能提高补丁被采纳的可能性，因为验证其正确性不需要太多时间和精力。一个 5 行的补丁，维护者瞥第二眼就能合入。而一个 500 行的补丁可能要花数小时来评审其正确性 (所花时间与补丁大小大致呈指数比例，或者类似的关系)。)

   Small patches also make it very easy to debug when something goes
   wrong.  It's much easier to back out patches one by one than it is
   to dissect a very large patch after it's been applied (and broken
   something).
   (小补丁也让出错时的调试变得非常容易。把补丁逐一回退，远比在应用了一个超大补丁 (并且搞坏了东西) 之后再解剖它容易得多。)

2) It's important not only to send small patches, but also to rewrite
   and simplify (or simply re-order) patches before submitting them.
   (重要的不仅是发送小补丁，还包括在提交之前重写并简化 (或者只是重新排序) 补丁。)

Here is an analogy from kernel developer Al Viro:
(内核开发者 Al Viro 打过一个比方：)

	*"Think of a teacher grading homework from a math student.  The
	teacher does not want to see the student's trials and errors
	before they came up with the solution. They want to see the
	cleanest, most elegant answer.  A good student knows this, and
	would never submit her intermediate work before the final
	solution.*

	*The same is true of kernel development. The maintainers and
	reviewers do not want to see the thought process behind the
	solution to the problem one is solving. They want to see a
	simple and elegant solution."*

("想象一位老师批改数学学生的作业。老师不想看到学生得出解答之前的种种试错，他们想看到最干净、最优雅的答案。好学生明白这一点，绝不会在最终解答之前提交自己的中间作业。内核开发也是如此。维护者和评审者不想看到解决问题背后的思考过程，他们想看到简单而优雅的解答。")

It may be challenging to keep the balance between presenting an elegant
solution and working together with the community and discussing your
unfinished work. Therefore it is good to get early in the process to
get feedback to improve your work, but also keep your changes in small
chunks that they may get already accepted, even when your whole task is
not ready for inclusion now.
(在展示优雅的解决方案、与社区协作并讨论未完成的工作之间保持平衡，可能颇有挑战。因此，尽早在流程中获得反馈以改进你的工作是有益的，同时也要把修改保持成小块，这样即使你的整个任务现在还不具备合入条件，其中一些也可能已经被接受。)

Also realize that it is not acceptable to send patches for inclusion
that are unfinished and will be "fixed up later."
(还要认识到，提交未完成、打算 "以后再修好" 的补丁来请求合入是不可接受的。)


Justify your change(说明修改的理由)
-----------------------------------

Along with breaking up your patches, it is very important for you to let
the Linux community know why they should add this change.  New features
must be justified as being needed and useful.
(除了拆分补丁之外，让 Linux 社区知道为什么应当加入这个修改也非常重要。新功能必须证明其必要性与有用性。)


Document your change(为你的修改写文档)
--------------------------------------

When sending in your patches, pay special attention to what you say in
the text in your email.  This information will become the ChangeLog
information for the patch, and will be preserved for everyone to see for
all time.  It should describe the patch completely, containing:
(发送补丁时，要特别留意你在邮件正文里写的内容。这些信息会成为补丁的 ChangeLog，并且会被永久保留供所有人查看。它应当完整地描述补丁，包括：)

  - why the change is necessary
  - the overall design approach in the patch
  - implementation details
  - testing results
  (为什么需要这个修改、补丁的整体设计思路、实现细节、测试结果)

For more details on what this should all look like, please see the
ChangeLog section of the document:
(关于这一切应当是什么样子的更多细节，请参阅该文档的 ChangeLog 一节：)

  "The Perfect Patch"
  (《完美的补丁》)
      https://www.ozlabs.org/~akpm/stuff/tpp.txt


All of these things are sometimes very hard to do. It can take years to
perfect these practices (if at all). It's a continuous process of
improvement that requires a lot of patience and determination. But
don't give up, it's possible. Many have done it before, and each had to
start exactly where you are now.
(所有这些有时都很难做到。完善这些实践可能需要多年 (如果真能做到的话)。这是一个需要极大耐心和决心的持续改进过程。但不要放弃，这是可能的。许多人以前都做到过，而每个人都必须恰恰从你现在所在的地方起步。)




----------

Thanks to Paolo Ciarrocchi who allowed the "Development Process"
(https://lwn.net/Articles/94386/) section
to be based on text he had written, and to Randy Dunlap and Gerrit
Huizenga for some of the list of things you should and should not say.
Also thanks to Pat Mochel, Hanna Linder, Randy Dunlap, Kay Sievers,
Vojtech Pavlik, Jan Kara, Josh Boyer, Kees Cook, Andrew Morton, Andi
Kleen, Vadim Lobanov, Jesper Juhl, Adrian Bunk, Keri Harris, Frans Pop,
David A. Wheeler, Junio Hamano, Michael Kerrisk, and Alex Shepard for
their review, comments, and contributions.  Without their help, this
document would not have been possible.
(感谢 Paolo Ciarrocchi 允许 "Development Process" (https://lwn.net/Articles/94386/) 一节基于他撰写的文本；感谢 Randy Dunlap 和 Gerrit Huizenga 提供了 "应该说与不该说的话" 清单的一部分。也感谢 Pat Mochel、Hanna Linder、Randy Dunlap、Kay Sievers、Vojtech Pavlik、Jan Kara、Josh Boyer、Kees Cook、Andrew Morton、Andi Kleen、Vadim Lobanov、Jesper Juhl、Adrian Bunk、Keri Harris、Frans Pop、David A. Wheeler、Junio Hamano、Michael Kerrisk 和 Alex Shepard 的评审、评论与贡献。没有他们的帮助，本文档不可能完成。)



Maintainer: Greg Kroah-Hartman <greg@kroah.com>
(维护者：Greg Kroah-Hartman <greg@kroah.com>)
