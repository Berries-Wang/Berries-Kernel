NFSD Maintainer Entry Profile
(NFSD 维护者条目概况)
=============================

A Maintainer Entry Profile supplements the top-level process
documents (found in Documentation/process/) with customs that are
specific to a subsystem and its maintainers. A contributor may use
this document to set their expectations and avoid common mistakes.
A maintainer may use these profiles to look across subsystems for
opportunities to converge on best common practices.
(维护者条目概况(Maintainer Entry Profile)是对顶层流程文档(位于 Documentation/process/)的补充，增加了特定于某个子系统及其维护者的惯例。贡献者可以使用本文档来设定预期并避免常见错误。维护者可以使用这些概况来纵观各子系统，寻找向最佳通用实践收敛的机会。)

Overview
(概述)
--------
The Network File System (NFS) is a standardized family of network
protocols that enable access to files across a set of network-
connected peer hosts. Applications on NFS clients access files that
reside on file systems that are shared by NFS servers. A single
network peer can act as both an NFS client and an NFS server.
(网络文件系统(NFS)是一组标准化的网络协议，使一组通过网络连接的对等主机之间能够互相访问文件。NFS 客户端上的应用程序访问的文件位于由 NFS 服务器共享的文件系统之上。单个网络对等端既可以充当 NFS 客户端，也可以充当 NFS 服务器。)

NFSD refers to the NFS server implementation included in the Linux
kernel. An in-kernel NFS server has fast access to files stored
in file systems local to that server. NFSD can share files stored
on most of the file system types native to Linux, including xfs,
ext4, btrfs, and tmpfs.
(NFSD 指的是 Linux 内核中包含的 NFS 服务器实现。内核中的 NFS 服务器可以快速访问存储在该服务器本地文件系统中的文件。NFSD 可以共享存储在大多数 Linux 原生文件系统类型上的文件，包括 xfs、ext4、btrfs 和 tmpfs。)

Mailing list
(邮件列表)
------------
The linux-nfs@vger.kernel.org mailing list is a public list. Its
purpose is to enable collaboration among developers working on the
Linux NFS stack, both client and server. It is not a place for
conversations that are not related directly to the Linux NFS stack.
(linux-nfs@vger.kernel.org 邮件列表是一个公开列表。其目的是让从事 Linux NFS 协议栈(客户端与服务器两方面)工作的开发者能够开展协作。它不是讨论与 Linux NFS 协议栈没有直接关系的话题的场所。)

The linux-nfs mailing list is archived on `lore.kernel.org <https://lore.kernel.org/linux-nfs/>`_.
(linux-nfs 邮件列表归档于 lore.kernel.org。)

The Linux NFS community does not have any chat room.
(Linux NFS 社区没有任何聊天室。)

Reporting bugs
(报告缺陷)
--------------
If you experience an NFSD-related bug on a distribution-built
kernel, please start by working with your Linux distributor.
(如果您在发行版构建的内核上遇到与 NFSD 相关的缺陷，请先与您的 Linux 发行版提供商协作解决。)

Bug reports against upstream Linux code bases are welcome on the
linux-nfs@vger.kernel.org mailing list, where some active triage
can be done. NFSD bugs may also be reported in the Linux kernel
community's bugzilla at:
(欢迎在 linux-nfs@vger.kernel.org 邮件列表上提交针对上游 Linux 代码库的缺陷报告，在那里可以进行一些积极的分诊处理。NFSD 缺陷也可以在 Linux 内核社区的 bugzilla 中报告，网址如下：)

  https://bugzilla.kernel.org

Please file NFSD-related bugs under the "Filesystems/NFSD"
component. In general, including as much detail as possible is a
good start, including pertinent system log messages from both
the client and server.
(请在"Filesystems/NFSD"组件下提交与 NFSD 相关的缺陷。一般来说，尽可能多地包含细节是一个好的开端，包括来自客户端和服务器的相关系统日志消息。)

User space software related to NFSD, such as mountd or the exportfs
command, is contained in the nfs-utils package. Report problems
with those components to linux-nfs@vger.kernel.org. You might be
directed to move the report to a specific bug tracker.
(与 NFSD 相关的用户空间软件(例如 mountd 或 exportfs 命令)包含在 nfs-utils 软件包中。这些组件的问题请报告到 linux-nfs@vger.kernel.org。你可能会被指引将报告转移到某个特定的缺陷跟踪系统。)

Contributor's Guide
(贡献者指南)
-------------------

Standards compliance
(标准符合性)
~~~~~~~~~~~~~~~~~~~~
The priority is for NFSD to interoperate fully with the Linux NFS
client. We also test against other popular NFS client implementa-
tions regularly at NFS bake-a-thon events (also known as plug-
fests). Non-Linux NFS clients are not part of upstream NFSD CI/CD.
(首要目标是让 NFSD 与 Linux NFS 客户端完全互操作。我们还会在 NFS bake-a-thon 活动(也称为 plug-fest)上定期针对其他流行的 NFS 客户端实现进行测试。非 Linux 的 NFS 客户端不属于上游 NFSD CI/CD 的一部分。)

The NFSD community strives to provide an NFS server implementation
that interoperates with all standards-compliant NFS client
implementations. This is done by staying as close as is sensible to
the normative mandates in the IETF's published NFS, RPC, and GSS-API
standards.
(NFSD 社区致力于提供一个能与所有符合标准的 NFS 客户端实现互操作的 NFS 服务器实现。做法是在合理的范围内尽可能贴近 IETF 已发布的 NFS、RPC 和 GSS-API 标准中的规范性要求。)

It is always useful to reference an RFC and section number in a code
comment where behavior deviates from the standard (and even when the
behavior is compliant but the implementation is obfuscatory).
(在行为偏离标准的代码注释中引用 RFC 及章节号总是有用的(即使行为合规但实现令人费解时，也值得这样做)。)

On the rare occasion when a deviation from standard-mandated
behavior is needed, brief documentation of the use case or
deficiencies in the standard is a required part of in-code
documentation.
(在极少数需要偏离标准规定行为的情况下，对用例或标准缺陷的简要说明是代码内文档的必备部分。)

Care must always be taken to avoid leaking local error codes (ie,
errnos) to clients of NFSD. A proper NFS status code is always
required in NFS protocol replies.
(必须始终注意避免把本地错误码(即 errno)泄露给 NFSD 的客户端。NFS 协议应答中必须始终使用恰当的 NFS 状态码。)

NFSD administrative interfaces
(NFSD 管理接口)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
NFSD administrative interfaces include:
(NFSD 管理接口包括：)

- an NFSD or SUNRPC module parameter
  (NFSD 或 SUNRPC 模块参数)

- export options in /etc/exports
  (/etc/exports 中的导出选项)

- files under /proc/fs/nfsd/ or /proc/sys/sunrpc/
  (/proc/fs/nfsd/ 或 /proc/sys/sunrpc/ 下的文件)

- the NFSD netlink protocol
  (NFSD netlink 协议)

Frequently, a request is made to introduce or modify one of NFSD's
traditional administrative interfaces. Certainly it is technically
easy to introduce a new administrative setting. However, there are
good reasons why the NFSD maintainers prefer to leave that as a last
resort:
(经常有人请求引入或修改 NFSD 的某个传统管理接口。诚然，从技术上讲引入一个新的管理设置很容易。然而，NFSD 维护者宁愿把这样做作为最后手段，这是有充分理由的：)

- As with any API, administrative interfaces are difficult to get
  right.
  (与任何 API 一样，管理接口很难设计正确。)

- Once they are documented and have a legacy of use, administrative
  interfaces become difficult to modify or remove.
  (一旦被写入文档并形成使用惯性，管理接口就变得难以修改或移除。)

- Every new administrative setting multiplies the NFSD test matrix.
  (每一个新的管理设置都会让 NFSD 的测试矩阵成倍扩大。)

- The cost of one administrative interface is incremental, but costs
  add up across all of the existing interfaces.
  (单个管理接口的成本是增量式的，但所有现有接口的成本会不断累加。)

It is often better for everyone if effort is made up front to
understanding the underlying requirement of the new setting, and
then trying to make it tune itself (or to become otherwise
unnecessary).
(如果事先花力气去理解新设置背后的底层需求，然后尝试让它自我调优(或以其他方式变得不再必要)，往往对所有人都更好。)

If a new setting is indeed necessary, first consider adding it to
the NFSD netlink protocol. Or if it doesn't need to be a reliable
long term user space feature, it can be added to NFSD's menagerie of
experimental settings which reside under /sys/kernel/debug/nfsd/ .
(如果确实需要新的设置，请首先考虑把它加入 NFSD netlink 协议。或者，如果它不需要成为可靠的长期用户空间特性，则可以把它加进 NFSD 那些位于 /sys/kernel/debug/nfsd/ 之下的实验性设置之中。)

Field observability
(现场可观测性)
~~~~~~~~~~~~~~~~~~~
NFSD employs several different mechanisms for observing operation,
including counters, printks, WARNings, and static trace points. Each
have their strengths and weaknesses. Contributors should select the
most appropriate tool for their task.
(NFSD 采用多种不同的机制来观察运行情况，包括计数器、printk、WARN 警告和静态跟踪点。它们各有优缺点。贡献者应当为自己的任务选择最合适的工具。)

- BUG must be avoided if at all possible, as it will frequently
  result in a full system crash.
  (应尽可能避免使用 BUG，因为它经常会导致整个系统崩溃。)

- WARN is appropriate only when a full stack trace is useful.
  (只有当完整的堆栈跟踪有用时，WARN 才是合适的。)

- printk can show detailed information. These must not be used
  in code paths where they can be triggered repeatedly by remote
  users.
  (printk 可以显示详细信息。在可能被远程用户反复触发的代码路径中，不得使用它们。)

- dprintk can show detailed information, but can be enabled only
  in pre-set groups. The overhead of emitting output makes dprintk
  inappropriate for frequent operations like I/O.
  (dprintk 可以显示详细信息，但只能按预先设定的组启用。输出所带来的开销使得 dprintk 不适合像 I/O 这样频繁的操作。)

- Counters are always on, but provide little information about
  individual events other than how frequently they occur.
  (计数器始终处于开启状态，但除了事件发生的频率之外，几乎不提供关于单个事件的信息。)

- static trace points can be enabled individually or in groups
  (via a glob). These are generally low overhead, and thus are
  favored for use in hot paths.
  (静态跟踪点可以单独启用，也可以成组启用(通过 glob)。它们的开销通常很低，因此更适合在热点路径中使用。)

- dynamic tracing, such as kprobes or eBPF, are quite flexible but
  cannot be used in certain environments (eg, full kernel lock-
  down).
  (动态跟踪(例如 kprobes 或 eBPF)非常灵活，但不能在某些环境中使用(例如内核被完全锁定(lockdown)时)。)

Testing
(测试)
~~~~~~~
The kdevops project
(kdevops 项目)

  https://github.com/linux-kdevops/kdevops

contains several NFS-specific workflows, as well as the community
standard fstests suite. These workflows are based on open source
testing tools such as ltp and fio. Contributors are encouraged to
use these tools without kdevops, or contributors should install and
use kdevops themselves to verify their patches before submission.
(其中包含若干 NFS 专用工作流，以及社区标准的 fstests 套件。这些工作流基于 ltp 和 fio 等开源测试工具。鼓励贡献者在不用 kdevops 的情况下使用这些工具，或者由贡献者自行安装并使用 kdevops，在提交之前验证自己的补丁。)

Coding style
(编码风格)
~~~~~~~~~~~~
Follow the coding style preferences described in
(遵循以下文档中描述的编码风格偏好)

  Documentation/process/coding-style.rst

with the following exceptions:
(但有以下例外：)

- Add new local variables to a function in reverse Christmas tree
  order
  (在函数中按逆圣诞树顺序添加新的局部变量)

- Use the kdoc comment style for
  + non-static functions
  (非静态函数)
  + static inline functions
  (static inline 函数)
  + static functions that are callbacks/virtual functions
  (作为回调/虚函数的静态函数)
  (对以上函数使用 kdoc 注释风格)

- All new function names start with ``nfsd_`` for non-NFS-version-
  specific functions.
  (所有非 NFS 版本专用的新函数名都以 ``nfsd_`` 开头。)

- New function names that are specific to NFSv2 or NFSv3, or are
  used by all minor versions of NFSv4, use ``nfsdN_`` where N is
  the version.
  (专用于 NFSv2 或 NFSv3、或者被 NFSv4 所有次版本使用的新函数名使用 ``nfsdN_``，其中 N 是版本号。)

- New function names specific to an NFSv4 minor version can be
  named with ``nfsd4M_`` where M is the minor version.
  (专用于某一 NFSv4 次版本的新函数名可以用 ``nfsd4M_`` 命名，其中 M 是次版本号。)

Patch preparation
(补丁准备)
~~~~~~~~~~~~~~~~~
Read and follow all guidelines in
(阅读并遵循以下文档中的所有准则)

  Documentation/process/submitting-patches.rst

Use tagging to identify all patch authors. However, reviewers and
testers should be added by replying to the email patch submission.
Email is extensively used in order to publicly archive review and
testing attributions. These tags are automatically inserted into
your patches when they are applied.
(使用标签来标识所有补丁作者。不过，审阅者和测试者应当通过回复邮件补丁提交来添加。电子邮件被大量使用，以便公开存档审阅和测试的署名。这些标签会在补丁被应用时自动插入其中。)

The code in the body of the diff already shows /what/ is being
changed. Thus it is not necessary to repeat that in the patch
description. Instead, the description should contain one or more
of:
(diff 正文中的代码已经展示了/什么/在被修改。因此无需在补丁描述中重复这些内容。相反，描述中应当包含以下一项或多项：)

- A brief problem statement ("what is this patch trying to fix?")
  with a root-cause analysis.
  (一段简短的问题陈述("这个补丁试图修复什么？")以及根本原因分析。)

- End-user visible symptoms or items that a support engineer might
  use to search for the patch, like stack traces.
  (最终用户可见的症状，或者支持工程师可能用来搜索该补丁的信息，例如堆栈跟踪。)

- A brief explanation of why the patch is the best way to address
  the problem.
  (简要解释为什么该补丁是解决这个问题的最佳方式。)

- Any context that reviewers might need to understand the changes
  made by the patch.
  (审阅者理解该补丁所作修改时可能需要的任何背景信息。)

- Any relevant benchmarking results, and/or functional test results.
  (任何相关的基准测试结果，和/或功能测试结果。)

As detailed in Documentation/process/submitting-patches.rst,
identify the point in history that the issue being addressed was
introduced by using a Fixes: tag.
(如 Documentation/process/submitting-patches.rst 中详述，使用 Fixes: 标签指出所处理的问题是在历史上哪个时点引入的。)

Mention in the patch description if that point in history cannot be
determined -- that is, no Fixes: tag can be provided. In this case,
please make it clear to maintainers whether an LTS backport is
needed even though there is no Fixes: tag.
(如果无法确定该历史时点，请在补丁描述中说明——也就是说，无法提供 Fixes: 标签。在这种情况下，请向维护者明确：即使没有 Fixes: 标签，是否仍需要进行 LTS 回移。)

The NFSD maintainers prefer to add stable tagging themselves, after
public discussion in response to the patch submission. Contributors
may suggest stable tagging, but be aware that many version
management tools add such stable Cc's when you post your patches.
Don't add "Cc: stable" unless you are absolutely sure the patch
needs to go to stable during the initial submission process.
(NFSD 维护者更愿意在针对补丁提交进行公开讨论之后，自行添加 stable 标注。贡献者可以建议 stable 标注，但请注意，许多版本管理工具会在你发布补丁时自动添加此类 stable Cc。除非你完全确定在初始提交流程中该补丁需要进入 stable，否则不要添加 "Cc: stable"。)

Patch submission
(补丁提交)
~~~~~~~~~~~~~~~~
Patches to NFSD are submitted via the kernel's email-based review
process that is common to most other kernel subsystems.
(针对 NFSD 的补丁通过内核基于电子邮件的审阅流程提交，该流程为大多数其他内核子系统所通用。)

Just before each submission, rebase your patch or series on the
nfsd-testing branch at
(每次提交之前，请将你的补丁或补丁系列变基到以下位置的 nfsd-testing 分支上)

  https://git.kernel.org/pub/scm/linux/kernel/git/cel/linux.git

The NFSD subsystem is maintained separately from the Linux in-kernel
NFS client. The NFSD maintainers do not normally take submissions
for client changes, nor can they respond authoritatively to bug
reports or feature requests for NFS client code.
(NFSD 子系统与内核中的 Linux NFS 客户端是分开维护的。NFSD 维护者通常不接受针对客户端修改的提交，也无法就 NFS 客户端代码的缺陷报告或功能请求作出权威回应。)

This means that contributors might be asked to resubmit patches if
they were emailed to the incorrect set of maintainers and reviewers.
This is not a rejection, but simply a correction of the submission
process.
(这意味着，如果补丁被发送给了不正确的维护者和审阅者集合，贡献者可能会被要求重新提交。这不是拒绝，而只是对提交流程的一次纠正。)

When in doubt, consult the NFSD entry in the MAINTAINERS file to
see which files and directories fall under the NFSD subsystem.
(如有疑问，请查阅 MAINTAINERS 文件中的 NFSD 条目，看看哪些文件和目录属于 NFSD 子系统。)

The proper set of email addresses for NFSD patches are:
(NFSD 补丁的恰当邮件地址集合如下：)

To: the NFSD maintainers and reviewers listed in MAINTAINERS
(To: MAINTAINERS 中列出的 NFSD 维护者和审阅者)
Cc: linux-nfs@vger.kernel.org and optionally linux-kernel@
(Cc: linux-nfs@vger.kernel.org，以及可选的 linux-kernel@)

If there are other subsystems involved in the patches (for example
MM or RDMA) their primary mailing list address can be included in
the Cc: field. Other contributors and interested parties may be
included there as well.
(如果补丁涉及其他子系统(例如 MM 或 RDMA)，这些子系统的主邮件列表地址也可以加入 Cc: 字段。其他贡献者和相关方也可以一并加入其中。)

In general we prefer that contributors use common patch email tools
such as "git send-email" or "stg email format/send", which tend to
get the details right without a lot of fuss.
(总体上，我们希望贡献者使用常见的补丁邮件工具，例如 "git send-email" 或 "stg email format/send"，它们往往能毫不费事地把细节处理正确。)

A series consisting of a single patch is not required to have a
cover letter. However, a cover letter can be included if there is
substantial context that is not appropriate to include in the
patch description.
(由单个补丁构成的系列不要求附有附函(cover letter)。不过，如果有大量不适合写进补丁描述的背景信息，可以附上附函。)

Please note that, with an e-mail based submission process, series
cover letters are not part of the work that is committed to the
kernel source code base or its commit history. Therefore always try
to keep pertinent information in the patch descriptions.
(请注意，在基于电子邮件的提交流程中，系列附函并不属于提交到内核源代码库或其提交历史的工作内容。因此，请始终尽量把相关信息保留在补丁描述中。)

Design documentation is welcome, but as cover letters are not
preserved, a perhaps better option is to include a patch that adds
such documentation under Documentation/filesystems/nfs/.
(欢迎提交设计文档，但由于附函不会被保留，一个也许更好的选择是附带一个补丁，把这类文档添加到 Documentation/filesystems/nfs/ 之下。)

Reviewers will ask about test coverage and what use cases the
patches are expected to address. Please be prepared to answer these
questions.
(审阅者会询问测试覆盖情况以及补丁预期处理的用例。请准备好回答这些问题。)

Review comments from maintainers might be politely stated, but in
general, these are not optional to address when they are actionable.
If necessary, the maintainers retain the right to not apply patches
when contributors refuse to address reasonable requests.
(维护者的审阅意见可能措辞礼貌，但一般而言，只要这些意见是可操作的，回应它们就不是可选项。如有必要，当贡献者拒绝处理合理请求时，维护者保留不应用其补丁的权利。)

Post changes to kernel source code and user space source code as
separate series. You can connect the two series with comments in
your cover letters.
(内核源代码的修改与用户空间源代码的修改请作为独立的系列发布。你可以在附函的说明中把两个系列关联起来。)

Generally the NFSD maintainers ask for a reposts even for simple
modifications in order to publicly archive the request and the
resulting repost before it is pulled into the NFSD trees. This
also enables us to rebuild a patch series quickly without missing
changes that might have been discussed via email.
(通常，即使是简单的修改，NFSD 维护者也会要求重新发布(repost)，以便在补丁被拉入 NFSD 树之前，把该请求以及随之而来的重新发布公开存档。这也使我们能够快速重建补丁系列，而不会遗漏可能通过电子邮件讨论过的修改。)

Avoid frequently reposting large series with only small changes. As
a rule of thumb, posting substantial changes more than once a week
will result in reviewer overload.
(避免频繁地重新发布只带少量修改的大型系列。根据经验，实质性修改的发布每周超过一次会让审阅者不堪重负。)

Remember, there are only a handful of subsystem maintainers and
reviewers, but potentially many sources of contributions. The
maintainers and reviewers, therefore, are always the less scalable
resource. Be kind to your friendly neighborhood maintainer.
(请记住，子系统维护者和审阅者只有寥寥数人，而贡献来源却可能很多。因此，维护者和审阅者始终是更难扩展的那一类资源。请善待你身边友好的维护者。)

Patch Acceptance
(补丁接受)
~~~~~~~~~~~~~~~~
There isn't a formal review process for NFSD, but we like to see
at least two Reviewed-by: notices for patches that are more than
simple clean-ups. Reviews are done in public on
linux-nfs@vger.kernel.org and are archived on lore.kernel.org.
(NFSD 没有正式的审阅流程，但对于超出简单清理范畴的补丁，我们希望看到至少两个 Reviewed-by: 标注。审阅在 linux-nfs@vger.kernel.org 上公开进行，并归档于 lore.kernel.org。)

Currently the NFSD patch queues are maintained in branches here:
(目前，NFSD 的补丁队列维护在以下分支中：)

  https://git.kernel.org/pub/scm/linux/kernel/git/cel/linux.git

The NFSD maintainers apply patches initially to the nfsd-testing
branch, which is always open to new submissions. Patches can be
applied while review is ongoing. nfsd-testing is a topic branch,
so it can change frequently, it will be rebased, and your patch
might get dropped if there is a problem with it.
(NFSD 维护者最初会把补丁应用到 nfsd-testing 分支，该分支始终接受新的提交。审阅进行期间补丁也可以被应用。nfsd-testing 是一个主题分支，因此它可能频繁变化、会被变基，如果你的补丁有问题，它可能会被丢弃。)

Generally a script-generated "thank you" email will indicate when
your patch has been added to the nfsd-testing branch. You can track
the progress of your patch using the linux-nfs patchworks instance:
(通常，脚本生成的"致谢"邮件会表明你的补丁何时被加入 nfsd-testing 分支。你可以使用 linux-nfs 的 patchworks 实例来跟踪补丁的进展：)

  https://patchwork.kernel.org/project/linux-nfs/list/

While your patch is in nfsd-testing, it is exposed to a variety of
test environments, including community zero-day bots, static
analysis tools, and NFSD continuous integration testing. The soak
period is three to four weeks.
(当补丁位于 nfsd-testing 期间，它会暴露给各种测试环境，包括社区 zero-day 机器人、静态分析工具和 NFSD 持续集成测试。浸泡期为三到四周。)

Each patch that survives in nfsd-testing for the soak period without
changes is moved to the nfsd-next branch.
(在 nfsd-testing 中度过浸泡期且未经修改的补丁会被移入 nfsd-next 分支。)

The nfsd-next branch is automatically merged into linux-next and
fs-next on a nightly basis.
(nfsd-next 分支每晚自动合并到 linux-next 和 fs-next。)

Patches that survive in nfsd-next are included in the next NFSD
merge window pull request. These windows typically occur once every
63 days (nine weeks).
(在 nfsd-next 中留存下来的补丁会被纳入下一次 NFSD 合并窗口的拉取请求。这些窗口通常每 63 天(九周)出现一次。)

When the upstream merge window closes, the nfsd-next branch is
renamed nfsd-fixes, and a new nfsd-next branch is created, based on
the upstream -rc1 tag.
(当上游合并窗口关闭时，nfsd-next 分支被改名为 nfsd-fixes，并基于上游 -rc1 标签创建一个新的 nfsd-next 分支。)

Fixes that are destined for an upstream -rc release also run the
nfsd-testing gauntlet, but are then applied to the nfsd-fixes
branch. That branch is made available for Linus to pull after a
short time. In order to limit the risk of introducing regressions,
we limit such fixes to emergency situations or fixes to breakage
that occurred during the most recent upstream merge.
(面向上游 -rc 发布的修复同样要闯过 nfsd-testing 这一关，但随后会被应用到 nfsd-fixes 分支。该分支会在短暂时间后开放给 Linus 拉取。为了限制引入回退的风险，我们把此类修复限制在紧急情况，或修复最近一次上游合并期间出现的破坏。)

Please make it clear when submitting an emergency patch that
immediate action (either application to -rc or LTS backport) is
needed.
(提交紧急补丁时，请明确说明需要立即采取行动(或是应用到 -rc，或是 LTS 回移)。)

Sensitive patch submissions and bug reports
(敏感的补丁提交与缺陷报告)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CVEs are generated by specific members of the Linux kernel community
and several external entities. The Linux NFS community does not emit
or assign CVEs. CVEs are assigned after an issue and its fix are
known.
(CVE 由 Linux 内核社区的特定成员以及若干外部机构生成。Linux NFS 社区不发布也不分配 CVE。CVE 是在问题及其修复为人所知之后才被分配的。)

However, the NFSD maintainers sometimes receive sensitive security
reports, and at times these are significant enough to need to be
embargoed. In such rare cases, fixes can be developed and reviewed
out of the public eye.
(不过，NFSD 维护者有时会收到敏感的安全报告，这些问题有时严重到需要保密(embargo)处理。在这类罕见情况下，修复可以在公众视野之外进行开发和审阅。)

Please be aware that many version management tools add the stable
Cc's when you post your patches. This is generally a nuisance, but
it can result in outing an embargoed security issue accidentally.
Don't add "Cc: stable" unless you are absolutely sure the patch
needs to go to stable@ during the initial submission process.
(请注意，许多版本管理工具会在你发布补丁时自动添加 stable Cc。这通常令人讨厌，而且可能导致意外泄露处于保密期的安全问题。除非你完全确定在初始提交流程中该补丁需要进入 stable@，否则不要添加 "Cc: stable"。)

Patches that are merged without ever appearing on any list, and
which carry a Reported-by: or Fixes: tag are detected as suspicious
by security-focused people. We encourage that, after any private
review, security-sensitive patches should be posted to linux-nfs@
for the usual public review, archiving, and test period.
(从未在任何邮件列表上出现、却带有 Reported-by: 或 Fixes: 标签就被合并的补丁，会被关注安全的人员视为可疑。我们鼓励：在任何私下审阅之后，安全敏感的补丁应发布到 linux-nfs@，以进行例行的公开审阅、归档和测试期。)

LLM-generated submissions
(LLM 生成的提交)
~~~~~~~~~~~~~~~~~~~~~~~~~
The Linux kernel community as a whole is still exploring the new
world of LLM-generated code. The NFSD maintainers will entertain
submission of patches that are partially or wholly generated by
LLM-based development tools. Such submissions are held to the
same standards as submissions created entirely by human authors:
(整个 Linux 内核社区仍在探索 LLM 生成代码这一新世界。NFSD 维护者愿意考虑接受由基于 LLM 的开发工具部分或全部生成的补丁提交。此类提交与完全由人类作者创建的提交适用相同的标准：)

- The human contributor identifies themselves via a Signed-off-by:
  tag. This tag counts as a DoC.
  (人类贡献者通过 Signed-off-by: 标签表明自己的身份。该标签可视为一份 DoC。)

- The human contributor is solely responsible for code provenance
  and any contamination by inadvertently-included code with a
  conflicting license, as usual.
  (与往常一样，人类贡献者对代码来源，以及因无意引入许可证冲突的代码而造成的任何污染，承担全部责任。)

- The human contributor must be able to answer and address review
  questions. A patch description such as "This fixed my problem
  but I don't know why" is not acceptable.
  (人类贡献者必须能够回答并处理审阅问题。诸如"这修复了我的问题但我不知道为什么"之类的补丁描述是不可接受的。)

- The contribution is subjected to the same test regimen as all
  other submissions.
  (该贡献与所有其他提交一样，接受相同的测试规程。)

- An indication (via a Generated-by: tag or otherwise) that the
  contribution is LLM-generated is not required.
  (不要求以任何方式(例如通过 Generated-by: 标签)表明该贡献由 LLM 生成。)

It is easy to address review comments and fix requests in LLM
generated code. So easy, in fact, that it becomes tempting to repost
refreshed code immediately. Please resist that temptation.
(在 LLM 生成的代码中处理审阅意见和修改请求很容易。事实上容易到让人很想立即重新发布更新后的代码。请抵制这种诱惑。)

As always, please avoid reposting series revisions more than once
every 24 hours.
(与往常一样，请避免在 24 小时内多次重新发布系列修订。)

Clean-up patches
(清理类补丁)
~~~~~~~~~~~~~~~~
The NFSD maintainers discourage patches which perform simple clean-
ups, which are not in the context of other work. For example:
(NFSD 维护者不鼓励那些脱离其他工作背景、单纯执行简单清理的补丁。例如：)

* Addressing ``checkpatch.pl`` warnings after merge
  (在合并之后处理 ``checkpatch.pl`` 警告)
* Addressing :ref:`Local variable ordering<rcs>` issues
  (处理"局部变量顺序"(Local variable ordering)问题)
* Addressing long-standing whitespace damage
  (处理长期存在的空白字符损坏)

This is because it is felt that the churn that such changes produce
comes at a greater cost than the value of such clean-ups.
(这是因为人们认为，此类改动带来的代码搅动，其代价超过了这类清理的价值。)

Conversely, spelling and grammar fixes are encouraged.
(相反，拼写和语法修正则受到鼓励。)

Stable and LTS support
(stable 与 LTS 支持)
----------------------
Upstream NFSD continuous integration testing runs against LTS trees
whenever they are updated.
(每当 LTS 树更新时，上游 NFSD 持续集成测试都会针对它们运行。)

Please indicate when a patch containing a fix needs to be considered
for LTS kernels, either via a Fixes: tag or explicit mention.
(当包含修复的补丁需要被考虑用于 LTS 内核时，请通过 Fixes: 标签或明确说明来加以指示。)

Feature requests
(功能请求)
----------------
There is no one way to make an official feature request, but
discussion about the request should eventually make its way to
the linux-nfs@vger.kernel.org mailing list for public review by
the community.
(提出正式的功能请求并无唯一途径，但关于该请求的讨论最终应当进入 linux-nfs@vger.kernel.org 邮件列表，以供社区公开审阅。)

Subsystem boundaries
(子系统边界)
~~~~~~~~~~~~~~~~~~~~
NFSD itself is not much more than a protocol engine. This means its
primary responsibility is to translate the NFS protocol into API
calls in the Linux kernel. For example, NFSD is not responsible for
knowing exactly how bytes or file attributes are managed on a block
device. It relies on other kernel subsystems for that.
(NFSD 本身不过是一个协议引擎。这意味着它的主要职责是把 NFS 协议转换为 Linux 内核中的 API 调用。例如，NFSD 不负责确切了解字节或文件属性在块设备上是如何管理的。那要依赖其他内核子系统。)

If the subsystems on which NFSD relies do not implement a particular
feature, even if the standard NFS protocols do support that feature,
that usually means NFSD cannot provide that feature without
substantial development work in other areas of the kernel.
(如果 NFSD 所依赖的子系统没有实现某个特定特性，那么即使标准 NFS 协议支持该特性，通常也意味着：若不在内核的其他领域进行大量开发工作，NFSD 就无法提供该特性。)

Specificity
(具体明确)
~~~~~~~~~~~
Feature requests can come from anywhere, and thus can often be
nebulous. A requester might not understand what a "use case" or
"user story" is. These descriptive paradigms are often used by
developers and architects to understand what is required of a
design, but are terms of art in the software trade, not used in
the everyday world.
(功能请求可能来自任何地方，因此往往含糊不清。请求者可能不理解什么是"用例"或"用户故事"。这些描述性范式常被开发者和架构师用来理解设计需要满足什么，但它们是软件行业的专业术语，并非日常生活用语。)

In order to prevent contributors and maintainers from becoming
overwhelmed, we won't be afraid of saying "no" politely to
underspecified requests.
(为了防止贡献者和维护者不堪重负，对于表述不充分的请求，我们会不怕礼貌地说"不"。)

Community roles and their authority
(社区角色及其职权)
-----------------------------------
The purpose of Linux subsystem communities is to provide expertise
and active stewardship of a narrow set of source files in the Linux
kernel. This can include managing user space tooling as well.
(Linux 子系统社区的目的是为 Linux 内核中一小部分源文件提供专业知识并进行积极的看管。这也可以包括管理用户空间工具。)

To contextualize the structure of the Linux NFS community that
is responsible for stewardship of the NFS server code base, we
define the community roles here.
(为了交代负责看管 NFS 服务器代码库的 Linux NFS 社区的结构，我们在此定义社区角色。)

- **Contributor** : Anyone who submits a code change, bug fix,
  recommendation, documentation fix, and so on. A contributor can
  submit regularly or infrequently.
  (**贡献者(Contributor)** ：提交代码修改、缺陷修复、建议、文档修正等内容的任何人。贡献者可以定期提交，也可以偶尔提交。)

- **Outside Contributor** : A contributor who is not a regular actor
  in the Linux NFS community. This can mean someone who contributes
  to other parts of the kernel, or someone who just noticed a
  misspelling in a comment and sent a patch.
  (**外部贡献者(Outside Contributor)** ：并非 Linux NFS 社区固定成员的贡献者。这可能指为内核其他部分做贡献的人，也可能指只是发现某条注释里有拼写错误并发来补丁的人。)

- **Reviewer** : Someone who is named in the MAINTAINERS file as a
  reviewer is an area expert who can request changes to contributed
  code, and expects that contributors will address the request.
  (**审阅者(Reviewer)** ：在 MAINTAINERS 文件中被列为审阅者的人是领域专家，可以对所贡献的代码提出修改要求，并期望贡献者处理该要求。)

- **External Reviewer** : Someone who is not named in the
  MAINTAINERS file as a reviewer, but who is an area expert.
  Examples include Linux kernel contributors with networking,
  security, or persistent storage expertise, or developers who
  contribute primarily to other NFS implementations.
  (**外部审阅者(External Reviewer)** ：未在 MAINTAINERS 文件中被列为审阅者、但属于领域专家的人。例如具备网络、安全或持久存储专业知识的 Linux 内核贡献者，或者主要向其他 NFS 实现做贡献的开发者。)

One or more people will take on the following roles. These people
are often generically referred to as "maintainers", and are
identified in the MAINTAINERS file with the "M:" tag under the NFSD
subsystem.
(一个或多个人将承担以下角色。这些人通常被泛称为"维护者"，并在 MAINTAINERS 文件中通过 NFSD 子系统下的 "M:" 标签加以标识。)

- **Upstream Release Manager** : This role is responsible for
  curating contributions into a branch, reviewing test results, and
  then sending a pull request during merge windows. There is a
  trust relationship between the release manager and Linus.
  (**上游发布经理(Upstream Release Manager)** ：该角色负责把贡献整理进分支、审阅测试结果，然后在合并窗口期间发送拉取请求。发布经理与 Linus 之间存在信任关系。)

- **Bug Triager** : Someone who is a first responder to bug reports
  submitted to the linux-nfs mailing list or bug trackers, and helps
  troubleshoot and identify next steps.
  (**缺陷分诊员(Bug Triager)** ：是对提交到 linux-nfs 邮件列表或缺陷跟踪系统的缺陷报告的第一响应人，帮助排查问题并确定后续步骤。)

- **Security Lead** : The security lead handles contacts from the
  security community to resolve immediate issues, as well as dealing
  with long-term security issues such as supply chain concerns. For
  upstream, that's usually whether contributions violate licensing
  or other intellectual property agreements.
  (**安全负责人(Security Lead)** ：安全负责人负责处理来自安全社区的联络以解决紧迫问题，同时处理供应链担忧之类的长期安全问题。对上游而言，这通常是指贡献是否违反许可协议或其他知识产权协议。)

- **Testing Lead** : The testing lead builds and runs the test
  infrastructure for the subsystem. The testing lead may ask for
  patches to be dropped because of ongoing high defect rates.
  (**测试负责人(Testing Lead)** ：测试负责人为子系统搭建并运行测试基础设施。由于持续偏高的缺陷率，测试负责人可以要求丢弃补丁。)

- **LTS Maintainer** : The LTS maintainer is responsible for managing
  the Fixes: and Cc: stable annotations on patches, and seeing that
  patches that cannot be automatically applied to LTS kernels get
  proper manual backports as necessary.
  (**LTS 维护者(LTS Maintainer)** ：LTS 维护者负责管理补丁上的 Fixes: 和 Cc: stable 标注，并确保无法自动应用到 LTS 内核的补丁在必要时得到恰当的人工回移。)

- **Community Manager** : This umpire role can be asked to call balls
  and strikes during conflicts, but is also responsible for ensuring
  the health of the relationships within the community and for
  facilitating discussions on long-term topics such as how to manage
  growing technical debt.
  (**社区经理(Community Manager)** ：这一裁判角色可以在冲突中被请求来判定是非，但也负责维护社区内部关系的健康，并推动诸如如何管理不断增长的技术债务等长期议题的讨论。)
