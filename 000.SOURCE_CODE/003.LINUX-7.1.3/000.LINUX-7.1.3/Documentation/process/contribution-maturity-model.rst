.. SPDX-License-Identifier: GPL-2.0

==================================================================
Linux Kernel Contribution Maturity Model(Linux 内核贡献成熟度模型)
==================================================================


Background(背景)
================

As a part of the 2021 Linux Kernel Maintainers’ Summit, there was a
`discussion <https://lwn.net/Articles/870581/>`_ about the challenges in
recruiting kernel maintainers as well as maintainer succession.  Some of
the conclusions from that discussion included that companies which are a
part of the Linux Kernel community need to allow engineers to be
maintainers as part of their job, so they can grow into becoming
respected leaders and eventually, kernel maintainers.  To support a
strong talent pipeline, developers should be allowed and encouraged to
take on upstream contributions such as reviewing other people’s patches,
refactoring kernel infrastructure, and writing documentation.
(作为 2021 年 Linux 内核维护者峰会的一部分，当时曾有一场关于招募内核
维护者以及维护者交接挑战的讨论。讨论得出的一些结论包括：属于 Linux
内核社区的企业需要允许工程师把担任维护者作为其工作的一部分，使他们
能够成长为受人尊敬的领导者，并最终成为内核维护者。为了打造强有力
的人才梯队，应当允许并鼓励开发者承担上游贡献，例如审查他人的补丁、
重构内核基础设施和编写文档。)

To that end, the Linux Foundation Technical Advisory Board (TAB)
proposes this Linux Kernel Contribution Maturity Model. These common
expectations for upstream community engagement aim to increase the
influence of individual developers, increase the collaboration of
organizations, and improve the overall health of the Linux Kernel
ecosystem.
(为此，Linux 基金会技术咨询委员会(TAB)提出了这份 Linux 内核贡献成熟度
模型。这些针对上游社区参与的共同期望，旨在提升个人开发者的影响力、
增进各组织之间的协作，并改善 Linux 内核生态系统的整体健康状况。)

The TAB urges organizations to continuously evaluate their Open Source
maturity model and commit to improvements to align with this model.  To
be effective, this evaluation should incorporate feedback from across
the organization, including management and developers at all seniority
levels.  In the spirit of Open Source, we encourage organizations to
publish their evaluations and plans to improve their engagement with the
upstream community.
(TAB 敦促各组织持续评估自身的开源成熟度模型，并承诺加以改进以与该模型
看齐。要行之有效，这种评估应吸收来自整个组织的反馈，包括管理层和各级
资历的开发者。本着开源精神，我们鼓励各组织公布其评估结果以及改进上游
社区参与的计划。)

Level 0(级别 0)
===============

* Software Engineers are not allowed to contribute patches to the Linux
  kernel.
  (不允许软件工程师向 Linux 内核提交补丁。)


Level 1(级别 1)
===============

* Software Engineers are allowed to contribute patches to the Linux
  kernel, either as part of their job responsibilities or on their own
  time.
  (允许软件工程师向 Linux 内核提交补丁，可以作为其工作职责的一部分，
  也可以利用个人时间进行。)

Level 2(级别 2)
===============

* Software Engineers are expected to contribute to the Linux Kernel as
  part of their job responsibilities.
  (期望软件工程师把向 Linux 内核做贡献作为其工作职责的一部分。)
* Software Engineers will be supported to attend Linux-related
  conferences as a part of their job.
  (支持软件工程师以工作一部分的形式参加 Linux 相关会议。)
* A Software Engineer’s upstream code contributions will be considered
  in promotion and performance reviews.
  (软件工程师的上游代码贡献将在晋升和绩效评估中得到考虑。)

Level 3(级别 3)
===============

* Software Engineers are expected to review patches (including patches
  authored by engineers from other companies) as part of their job
  responsibilities
  (期望软件工程师把审查补丁(包括其他公司工程师编写的补丁)作为其工作
  职责的一部分。)
* Contributing presentations or papers to Linux-related or academic
  conferences (such those organized by the Linux Foundation, Usenix,
  ACM, etc.), are considered part of an engineer’s work.
  (向 Linux 相关会议或学术会议(如 Linux 基金会、Usenix、ACM 等组织的
  会议)提交演讲或论文，被视为工程师工作的一部分。)
* A Software Engineer’s community contributions will be considered in
  promotion and performance reviews.
  (软件工程师的社区贡献将在晋升和绩效评估中得到考虑。)
* Organizations will regularly report metrics of their open source
  contributions and track these metrics over time.  These metrics may be
  published only internally within the organization, or at the
  organization’s discretion, some or all may be published externally.
  Metrics that are strongly suggested include:
  (各组织将定期汇报其开源贡献的度量指标，并随时间跟踪这些指标。这些
  指标可以只在组织内部公布，也可以由组织酌情对外公布部分或全部。强烈
  建议采用的指标包括：)

  * The number of upstream kernel contributions by team or organization
    (e.g., all people reporting up to a manager, director, or VP).
    (按团队或组织统计的上游内核贡献数量(例如，向某位经理、总监或副总
    裁汇报的所有人员)。)
  * The percentage of kernel developers who have made upstream
    contributions relative to the total kernel developers in the
    organization.
    (组织内做过上游贡献的内核开发者占内核开发者总数的比例。)
  * The time interval between kernels used in the organization’s servers
    and/or products, and the publication date of the upstream kernel
    upon which the internal kernel is based.
    (组织服务器和/或产品中使用的内核与作为内部内核基础的上游内核发布
    日期之间的时间间隔。)
  * The number of out-of-tree commits present in internal kernels.
    (内部内核中树外(out-of-tree)提交的数量。)

Level 4(级别 4)
===============

* Software Engineers are encouraged to spend a portion of their work
  time focused on Upstream Work, which is defined as reviewing patches,
  serving on program committees, improving core project infrastructure
  such as writing or maintaining tests, upstream tech debt reduction,
  writing documentation, etc.
  (鼓励软件工程师把一部分工作时间专注于上游工作(Upstream Work)，其
  定义为审查补丁、担任程序委员会委员、改进核心项目基础设施(如编写或
  维护测试)、减少上游技术债务、编写文档等。)
* Software Engineers are supported in helping to organize Linux-related
  conferences.
  (为软件工程师参与组织 Linux 相关会议提供支持。)
* Organizations will consider community member feedback in official
  performance reviews.
  (各组织将在正式绩效评估中考虑社区成员的反馈。)

Level 5(级别 5)
===============

* Upstream kernel development is considered a formal job position, with
  at least a third of the engineer’s time spent doing Upstream Work.
  (上游内核开发被视为正式的工作岗位，工程师至少有三分之一的时间用于
  上游工作。)
* Organizations will actively seek out community member feedback as a
  factor in official performance reviews.
  (各组织将主动征集社区成员的反馈，作为正式绩效评估的一个因素。)
* Organizations will regularly report internally on the ratio of
  Upstream Work to work focused on directly pursuing business goals.
  (各组织将定期在内部汇报上游工作与直接追求业务目标的工作之间的
  比例。)
