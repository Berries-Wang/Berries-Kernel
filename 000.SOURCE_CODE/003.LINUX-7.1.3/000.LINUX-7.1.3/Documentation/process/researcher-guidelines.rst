.. SPDX-License-Identifier: GPL-2.0

.. _researcher_guidelines:

Researcher Guidelines(研究者指南)
+++++++++++++++++++++++++++++++++

The Linux kernel community welcomes transparent research on the Linux
kernel, the activities involved in producing it, and any other byproducts
of its development. Linux benefits greatly from this kind of research, and
most aspects of Linux are driven by research in one form or another.
(Linux 内核社区欢迎针对 Linux 内核、其生产过程所涉及的活动以及其开发的任何其他副产品的透明研究。Linux 从这类研究中获益良多，Linux 的许多方面都由某种形式的研究推动。)

The community greatly appreciates if researchers can share preliminary
findings before making their results public, especially if such research
involves security. Getting involved early helps both improve the quality
of research and ability for Linux to improve from it. In any case,
sharing open access copies of the published research with the community
is recommended.
(如果研究者能在公开研究结果之前分享初步发现，社区将不胜感激，尤其是当这类研究涉及安全时。尽早参与既有助于提高研究质量，也有助于 Linux 从中改进。无论如何，建议将已发表研究的开放获取副本分享给社区。)

This document seeks to clarify what the Linux kernel community considers
acceptable and non-acceptable practices when conducting such research. At
the very least, such research and related activities should follow
standard research ethics rules. For more background on research ethics
generally, ethics in technology, and research of developer communities
in particular, see:
(本文旨在澄清 Linux 内核社区认为在开展此类研究时哪些做法可接受、哪些不可接受。至少而言，这类研究及其相关活动应遵循标准的研究伦理规则。有关一般研究伦理、技术伦理以及开发者社区研究的更多背景，请参阅：)

* `History of Research Ethics <https://www.unlv.edu/research/ORI-HSR/history-ethics>`_
* `IEEE Ethics <https://www.ieee.org/about/ethics/index.html>`_
* `Developer and Researcher Views on the Ethics of Experiments on Open-Source Projects <https://arxiv.org/pdf/2112.13217.pdf>`_

The Linux kernel community expects that everyone interacting with the
project is participating in good faith to make Linux better. Research on
any publicly-available artifact (including, but not limited to source
code) produced by the Linux kernel community is welcome, though research
on developers must be distinctly opt-in.
(Linux 内核社区期望，与项目互动的每个人都是出于善意参与、以使 Linux 变得更好。欢迎对 Linux 内核社区产出的任何公开可用的产物(包括但不限于源代码)进行研究，但对开发者的研究必须明确征得同意 (opt-in)。)

Passive research that is based entirely on publicly available sources,
including posts to public mailing lists and commits to public
repositories, is clearly permissible. Though, as with any research,
standard ethics must still be followed.
(完全基于公开来源的被动研究——包括公共邮件列表上的帖子和公共仓库中的提交——显然是允许的。不过，与任何研究一样，仍必须遵循标准伦理。)

Active research on developer behavior, however, must be done with the
explicit agreement of, and full disclosure to, the individual developers
involved. Developers cannot be interacted with/experimented on without
consent; this, too, is standard research ethics.
(然而，针对开发者行为的主动研究必须在获得所涉开发者的明确同意并对其进行充分披露的情况下进行。未经同意，不得与开发者互动或对其进行实验；这也是标准的研究伦理。)

Surveys(问卷调查)
=================

Research often takes the form of surveys sent to maintainers or
contributors.  As a general rule, though, the kernel community derives
little value from these surveys.  The kernel development process works
because every developer benefits from their participation, even working
with others who have different goals.  Responding to a survey, though, is a
one-way demand placed on busy developers with no corresponding benefit to
themselves or to the kernel community as a whole.  For this reason, this
method of research is discouraged.
(研究通常以发送给维护者或贡献者的问卷调查 (survey) 形式进行。不过，一般而言，内核社区从这些问卷中获益甚少。内核开发流程之所以有效，是因为每位开发者都从参与中受益，即使与目标不同的人共事。而回应问卷则是对繁忙开发者的一种单向要求，对他们自己或整个内核社区都没有相应的益处。因此，不鼓励这种研究方式。)

Kernel community members already receive far too much email and are likely
to perceive survey requests as just another demand on their time.  Sending
such requests deprives the community of valuable contributor time and is
unlikely to yield a statistically useful response.
(内核社区成员收到的电子邮件已经太多了，他们很可能把问卷请求视为对其时间的又一项索取。发送此类请求剥夺了社区宝贵的贡献者时间，并且不太可能产生统计学上有用的回应。)

As an alternative, researchers should consider attending developer events,
hosting sessions where the research project and its benefits to the
participants can be explained, and interacting directly with the community
there.  The information received will be far richer than that obtained from
an email survey, and the community will gain from the ability to learn from
your insights as well.
(作为替代，研究者应考虑参加开发者活动，举办能够解释研究项目及其对参与者益处的会议，并在那里直接与社区互动。这样获得的信息将远比通过电子邮件问卷获得的信息丰富，社区也将因能够从你的见解中学习而受益。)

Patches(补丁)
=============

To help clarify: sending patches to developers *is* interacting
with them, but they have already consented to receiving *good faith
contributions*. Sending intentionally flawed/vulnerable patches or
contributing misleading information to discussions is not consented
to. Such communication can be damaging to the developer (e.g. draining
time, effort, and morale) and damaging to the project by eroding
the entire developer community's trust in the contributor (and the
contributor's organization as a whole), undermining efforts to provide
constructive feedback to contributors, and putting end users at risk of
software flaws.
(为帮助澄清：向开发者发送补丁*确实*是与他们互动，但他们已经同意接收的是*善意的贡献*。发送故意有缺陷/有漏洞的补丁，或在讨论中提供误导性信息，都不在同意之列。这种交流可能损害开发者(例如消耗时间、精力和士气)，并通过侵蚀整个开发者社区对该贡献者(及其所在组织)的信任而损害项目，破坏向贡献者提供建设性反馈的努力，并使最终用户面临软件缺陷的风险。)

Participation in the development of Linux itself by researchers, as
with anyone, is welcomed and encouraged. Research into Linux code is
a common practice, especially when it comes to developing or running
analysis tools that produce actionable results.
(研究者参与 Linux 本身的开发，与任何人一样，是受到欢迎和鼓励的。对 Linux 代码的研究是一种常见做法，尤其是在开发或运行能产生可操作结果的分析工具方面。)

When engaging with the developer community, sending a patch has
traditionally been the best way to make an impact. Linux already has
plenty of known bugs -- what's much more helpful is having vetted fixes.
Before contributing, carefully read the appropriate documentation:
(在与开发者社区互动时，发送补丁传统上是产生影响的最佳方式。Linux 已经有很多已知 bug——更有帮助的是经过审查的修复。在贡献之前，请仔细阅读相应的文档：)

* Documentation/process/development-process.rst
* Documentation/process/submitting-patches.rst
* Documentation/admin-guide/reporting-issues.rst
* Documentation/process/security-bugs.rst

Then send a patch (including a commit log with all the details listed
below) and follow up on any feedback from other developers.
(然后发送补丁(提交日志中包含下文列出的所有细节)，并跟进其他开发者的任何反馈。)

When sending patches produced from research, the commit logs should
contain at least the following details, so that developers have
appropriate context for understanding the contribution. Answer:
(发送来自研究的补丁时，提交日志至少应包含以下细节，以便开发者有适当的背景来理解该贡献。请回答：)

* What is the specific problem that has been found?
  (发现的具体问题是什么？)
* How could the problem be reached on a running system?
  (在运行中的系统上如何才能触达该问题？)
* What effect would encountering the problem have on the system?
  (遇到该问题会对系统产生什么影响？)
* How was the problem found? Specifically include details about any
  testing, static or dynamic analysis programs, and any other tools or
  methods used to perform the work.
  (该问题是如何被发现的？请具体说明所使用的任何测试、静态或动态分析程序，以及用于完成这项工作的任何其他工具或方法。)
* Which version of Linux was the problem found on? Using the most recent
  release or a recent linux-next branch is strongly preferred (see
  Documentation/process/howto.rst).
  (该问题是在哪个版本的 Linux 上发现的？强烈建议使用最新发布版本或最近的 linux-next 分支(参见 Documentation/process/howto.rst)。)
* What was changed to fix the problem, and why it is believed to be correct?
  (为修复该问题做了什么修改，以及为什么确信它是正确的？)
* How was the change build tested and run-time tested?
  (该修改是如何进行构建测试和运行时测试的？)
* What prior commit does this change fix? This should go in a "Fixes:"
  tag as the documentation describes.
  (此修改修复了哪个先前的提交？这应按文档描述放入 "Fixes:" 标签中。)
* Who else has reviewed this patch? This should go in appropriate
  "Reviewed-by:" tags; see below.
  (还有谁审查了这个补丁？这应放入相应的 "Reviewed-by:" 标签中；见下文。)

For example::

  From: Author <author@email>
  Subject: [PATCH] drivers/foo_bar: Add missing kfree()

  The error path in foo_bar driver does not correctly free the allocated
  struct foo_bar_info. This can happen if the attached foo_bar device
  rejects the initialization packets sent during foo_bar_probe(). This
  would result in a 64 byte slab memory leak once per device attach,
  wasting memory resources over time.

  This flaw was found using an experimental static analysis tool we are
  developing, LeakMagic[1], which reported the following warning when
  analyzing the v5.15 kernel release:

   path/to/foo_bar.c:187: missing kfree() call?

  Add the missing kfree() to the error path. No other references to
  this memory exist outside the probe function, so this is the only
  place it can be freed.

  x86_64 and arm64 defconfig builds with CONFIG_FOO_BAR=y using GCC
  11.2 show no new warnings, and LeakMagic no longer warns about this
  code path. As we don't have a FooBar device to test with, no runtime
  testing was able to be performed.

  [1] https://url/to/leakmagic/details

  Reported-by: Researcher <researcher@email>
  Fixes: aaaabbbbccccdddd ("Introduce support for FooBar")
  Signed-off-by: Author <author@email>
  Reviewed-by: Reviewer <reviewer@email>

(例如：)

If you are a first time contributor it is recommended that the patch
itself be vetted by others privately before being posted to public lists.
(This is required if you have been explicitly told your patches need
more careful internal review.) These people are expected to have their
"Reviewed-by" tag included in the resulting patch. Finding another
developer familiar with Linux contribution, especially within your own
organization, and having them help with reviews before sending them to
the public mailing lists tends to significantly improve the quality of the
resulting patches, and there by reduces the burden on other developers.
(如果你是首次贡献者，建议在发布到公共列表之前，先由其他人私下审查补丁本身。(如果你已被明确告知你的补丁需要更仔细的内部审查，则这是必需的。)这些人应将其 "Reviewed-by" 标签包含在最终的补丁中。找一位熟悉 Linux 贡献流程的开发者(尤其是在你自己的组织内)，请其在补丁发送到公共邮件列表之前帮助审查，往往会显著提高最终补丁的质量，从而减轻其他开发者的负担。)

If no one can be found to internally review patches and you need
help finding such a person, or if you have any other questions
related to this document and the developer community's expectations,
please reach out to the private Technical Advisory Board mailing list:
<tech-board@groups.linuxfoundation.org>.
(如果找不到人来内部审查补丁，而你需要帮助寻找这样的人；或者你对本文档以及开发者社区的期望有任何其他问题，请联系私下的技术顾问委员会 (Technical Advisory Board) 邮件列表：
<tech-board@groups.linuxfoundation.org>。)
