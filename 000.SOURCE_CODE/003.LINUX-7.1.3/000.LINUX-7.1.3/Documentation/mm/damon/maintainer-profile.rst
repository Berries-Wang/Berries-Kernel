.. SPDX-License-Identifier: GPL-2.0

DAMON Maintainer Entry Profile(DAMON 维护者条目档案)
====================================================

The DAMON subsystem covers the files that are listed in 'DAMON' section of
'MAINTAINERS' file.
(DAMON 子系统涵盖 'MAINTAINERS' 文件中 'DAMON' 一节所列出的文件。)

The mailing lists for the subsystem are damon@lists.linux.dev and
linux-mm@kvack.org.  Patches should be made against the `mm-new tree
<https://git.kernel.org/akpm/mm/h/mm-new>`_ whenever possible and posted to the
mailing lists.
(该子系统的邮件列表是 damon@lists.linux.dev 和 linux-mm@kvack.org。
补丁应尽可能基于 mm-new 树制作，并投递到邮件列表。)

SCM Trees(SCM 树)
-----------------

There are multiple Linux trees for DAMON development.  Patches under
development or testing are queued in `damon/next
<https://git.kernel.org/sj/h/damon/next>`_ by the DAMON maintainer.
Sufficiently reviewed patches will be queued in `mm-new
<https://git.kernel.org/akpm/mm/h/mm-new>`_ by the memory management subsystem
maintainer.  As more sufficient tests are done, the patches will move to
`mm-unstable <https://git.kernel.org/akpm/mm/h/mm-unstable>`_ and then to
`mm-stable <https://git.kernel.org/akpm/mm/h/mm-stable>`_.  And finally those
will be pull-requested to the mainline by the memory management subsystem
maintainer.
(DAMON 开发有多个 Linux 树。处于开发或测试阶段的补丁由 DAMON 维护者
排入 damon/next 树。经过充分审查的补丁将由内存管理子系统维护者排入
mm-new 树。随着更充分测试的完成，补丁会转移到 mm-unstable 树，然后是
mm-stable 树。最后，这些补丁将由内存管理子系统维护者向主线发起拉取
请求。)

Note again the patches for `mm-new tree
<https://git.kernel.org/akpm/mm/h/mm-new>`_ are queued by the memory management
subsystem maintainer.  If the patches require some patches in `damon/next tree
<https://git.kernel.org/sj/h/damon/next>`_ which have not yet merged in mm-new,
please make sure the requirement is clearly specified.
(再次注意，mm-new 树的补丁由内存管理子系统维护者排入。如果补丁依赖
damon/next 树中尚未合入 mm-new 的某些补丁，请确保明确说明这一依赖。)

Submit checklist addendum(提交清单补充)
---------------------------------------

When making DAMON changes, you should do below.
(进行 DAMON 修改时，你应当做到以下几点。)

- Build changes related outputs including kernel and documents.
  (构建与修改相关的产物，包括内核和文档。)
- Ensure the builds introduce no new errors or warnings.
  (确保构建不引入新的错误或警告。)
- Run and ensure no new failures for DAMON `selftests
  <https://github.com/damonitor/damon-tests/blob/master/corr/run.sh#L49>`_ and
  `kunittests
  <https://github.com/damonitor/damon-tests/blob/master/corr/tests/kunit.sh>`_.
  (运行 DAMON selftests 和 kunittests，并确保没有新的失败。)

Further doing below and putting the results will be helpful.
(此外，做以下事情并附上结果会很有帮助。)

- Run `damon-tests/corr
  <https://github.com/damonitor/damon-tests/tree/master/corr>`_ for normal
  changes.
  (对常规修改运行 damon-tests/corr。)
- Measure impacts on benchmarks or real world workloads for performance
  changes.
  (对性能修改，测量其对基准测试或真实工作负载的影响。)

Key cycle dates(关键周期日期)
-----------------------------

Patches can be sent anytime.  Key cycle dates of the `mm-new
<https://git.kernel.org/akpm/mm/h/mm-new>`_, `mm-unstable
<https://git.kernel.org/akpm/mm/h/mm-unstable>`_ and `mm-stable
<https://git.kernel.org/akpm/mm/h/mm-stable>`_ trees depend on the memory
management subsystem maintainer.
(补丁可以随时发送。mm-new、mm-unstable 和 mm-stable 树的关键周期日期
取决于内存管理子系统维护者。)

Review cadence(审查节奏)
------------------------

The DAMON maintainer usually work in a flexible way, except early morning in PT
(Pacific Time).  The response to patches will occasionally be slow.  Do not
hesitate to send a ping if you have not heard back within a week of sending a
patch.
(DAMON 维护者通常以灵活的方式工作，太平洋时间(PT)清晨除外。对补丁
的回复偶尔会比较慢。如果发送补丁一周内没有收到回音，请毫不犹豫地发
个提醒(ping)。)

Mailing tool(邮件工具)
----------------------

Like many other Linux kernel subsystems, DAMON uses the mailing lists
(damon@lists.linux.dev and linux-mm@kvack.org) as the major communication
channel.  There is a simple tool called `HacKerMaiL
<https://github.com/damonitor/hackermail>`_ (``hkml``), which is for people who
are not very familiar with the mailing lists based communication.  The tool
could be particularly helpful for DAMON community members since it is developed
and maintained by DAMON maintainer.  The tool is also officially announced to
support DAMON and general Linux kernel development workflow.
(与许多其他 Linux 内核子系统一样，DAMON 使用邮件列表
(damon@lists.linux.dev 和 linux-mm@kvack.org)作为主要的沟通渠道。
有一个名为 HacKerMaiL(``hkml``)的简单工具，面向对基于邮件列表的沟通
不太熟悉的人。该工具由 DAMON 维护者开发和维护，因此对 DAMON 社区成员
可能特别有帮助。该工具也已正式宣布支持 DAMON 和一般的 Linux 内核开发
工作流。)

In other words, `hkml <https://github.com/damonitor/hackermail>`_ is a mailing
tool for DAMON community, which DAMON maintainer is committed to support.
Please feel free to try and report issues or feature requests for the tool to
the maintainer.
(换句话说，hkml 是一个面向 DAMON 社区的邮件工具，DAMON 维护者承诺为
其提供支持。欢迎试用，并向维护者报告该工具的问题或功能请求。)

Community meetup(社区聚会)
--------------------------

DAMON community has a bi-weekly meetup series for members who prefer
synchronous conversations over mails.  It is for discussions on specific topics
between a group of members including the maintainer.  The maintainer shares the
available time slots, and attendees should reserve one of those at least 24
hours before the time slot, by reaching out to the maintainer.
(DAMON 社区有一个双周聚会系列，面向更喜欢同步交流而非邮件的成员。
它用于包括维护者在内的一组成员之间就特定主题进行讨论。维护者会公布
可用的时间段，参加者应至少在时间段开始前 24 小时联系维护者预订其中
一个。)

Schedules and reservation status are available at the Google `doc
<https://docs.google.com/document/d/1v43Kcj3ly4CYqmAkMaZzLiM2GEnWfgdGbZAH3mi2vpM/edit?usp=sharing>`_.
There is also a public Google `calendar
<https://calendar.google.com/calendar/u/0?cid=ZDIwOTA4YTMxNjc2MDQ3NTIyMmUzYTM5ZmQyM2U4NDA0ZGIwZjBiYmJlZGQxNDM0MmY4ZTRjOTE0NjdhZDRiY0Bncm91cC5jYWxlbmRhci5nb29nbGUuY29t>`_
that has the events.  Anyone can subscribe to it.  DAMON maintainer will also
provide periodic reminders to the mailing list (damon@lists.linux.dev).
(日程和预订状态可以在 Google 文档中查看。还有一个包含这些日程的公开
Google 日历，任何人都可以订阅。DAMON 维护者还会定期向邮件列表
(damon@lists.linux.dev)发送提醒。)
