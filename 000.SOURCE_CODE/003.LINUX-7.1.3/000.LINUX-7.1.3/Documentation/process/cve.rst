==============
CVEs(CVE 编号)
==============

Common Vulnerabilities and Exposure (CVE®) numbers were developed as an
unambiguous way to identify, define, and catalog publicly disclosed
security vulnerabilities.  Over time, their usefulness has declined with
regards to the kernel project, and CVE numbers were very often assigned
in inappropriate ways and for inappropriate reasons.  Because of this,
the kernel development community has tended to avoid them.  However, the
combination of continuing pressure to assign CVEs and other forms of
security identifiers, and ongoing abuses by individuals and companies
outside of the kernel community has made it clear that the kernel
community should have control over those assignments.
(通用漏洞与披露（CVE®）编号被开发为一种明确无误的方式来识别、定义和编目公开披露的安全漏洞。随着时间的推移，就内核项目而言，它们的有用性已经下降，而且 CVE 编号常常以不恰当的方式和出于不恰当的原因被分配。因此，内核开发社区倾向于避开它们。然而，分配 CVE 和其他形式安全标识符的持续压力，加上内核社区之外的个人和公司持续的滥用，清楚地表明内核社区应当掌控这些分配。)

The Linux kernel developer team does have the ability to assign CVEs for
potential Linux kernel security issues.  This assignment is independent
of the :doc:`normal Linux kernel security bug reporting
process<../process/security-bugs>`.
(Linux 内核开发者团队确实有能力为潜在的 Linux 内核安全问题分配 CVE。这种分配独立于 :doc:`normal Linux kernel security bug reporting process<../process/security-bugs>` （常规的 Linux 内核安全缺陷报告流程）。)

A list of all assigned CVEs for the Linux kernel can be found in the
archives of the linux-cve mailing list, as seen on
https://lore.kernel.org/linux-cve-announce/.  To get notice of the
assigned CVEs, please `subscribe
<https://subspace.kernel.org/subscribing.html>`_ to that mailing list.
(Linux 内核所有已分配 CVE 的列表可以在 linux-cve 邮件列表的存档中找到，见 https://lore.kernel.org/linux-cve-announce/ 。要获得 CVE 分配的通知，请 `subscribe <https://subspace.kernel.org/subscribing.html>`_ 订阅该邮件列表。)

Process(流程)
=============

As part of the normal stable release process, kernel changes that are
potentially security issues are identified by the developers responsible
for CVE number assignments and have CVE numbers automatically assigned
to them.  These assignments are published on the linux-cve-announce
mailing list as announcements on a frequent basis.
(作为常规稳定版发布流程的一部分，负责 CVE 编号分配的开发人员会识别出可能是安全问题的内核更改，并自动为其分配 CVE 编号。这些分配作为公告频繁地发布在 linux-cve-announce 邮件列表上。)

Note, due to the layer at which the Linux kernel is in a system, almost
any bug might be exploitable to compromise the security of the kernel,
but the possibility of exploitation is often not evident when the bug is
fixed.  Because of this, the CVE assignment team is overly cautious and
assign CVE numbers to any bugfix that they identify.  This
explains the seemingly large number of CVEs that are issued by the Linux
kernel team.
(注意，由于 Linux 内核在系统中所处的层次，几乎任何缺陷都可能被利用来危害内核的安全，但在缺陷被修复时，其可利用性往往并不明显。因此，CVE 分配团队过于谨慎，会为他们识别出的任何缺陷修复分配 CVE 编号。这解释了 Linux 内核团队发布的 CVE 数量看起来如此之多的原因。)

If the CVE assignment team misses a specific fix that any user feels
should have a CVE assigned to it, please email them at <cve@kernel.org>
and the team there will work with you on it.  Note that no potential
security issues should be sent to this alias, it is ONLY for assignment
of CVEs for fixes that are already in released kernel trees.  If you
feel you have found an unfixed security issue, please follow the
:doc:`normal Linux kernel security bug reporting
process<../process/security-bugs>`.
(如果 CVE 分配团队遗漏了任何用户认为应当分配 CVE 的特定修复，请发送邮件至 <cve@kernel.org>，那里的团队会与你一起处理。注意，潜在的安全问题不应发送到这个别名，它仅用于为已进入已发布内核代码树的修复分配 CVE。如果你认为自己发现了一个未修复的安全问题，请遵循 :doc:`normal Linux kernel security bug reporting process<../process/security-bugs>` 。)

No CVEs will be automatically assigned for unfixed security issues in
the Linux kernel; assignment will only automatically happen after a fix
is available and applied to a stable kernel tree, and it will be tracked
that way by the git commit id of the original fix.  If anyone wishes to
have a CVE assigned before an issue is resolved with a commit, please
contact the kernel CVE assignment team at <cve@kernel.org> to get an
identifier assigned from their batch of reserved identifiers.
(对于 Linux 内核中未修复的安全问题，不会自动分配 CVE；只有在修复可用并已应用到稳定内核代码树之后才会自动分配，并且会以原始修复的 git commit id 来跟踪。如果任何人希望在问题通过提交解决之前获得 CVE 分配，请联系内核 CVE 分配团队 <cve@kernel.org>，从他们预留的标识符批次中获得一个分配。)

No CVEs will be assigned for any issue found in a version of the kernel
that is not currently being actively supported by the Stable/LTS kernel
team.  A list of the currently supported kernel branches can be found at
https://kernel.org/releases.html
(对于 Stable/LTS 内核团队当前不再积极支持的内核版本中发现的任何问题，将不分配 CVE。当前受支持的内核分支列表见 https://kernel.org/releases.html)

Disputes of assigned CVEs(对已分配 CVE 的争议)
==============================================

The authority to dispute or modify an assigned CVE for a specific kernel
change lies solely with the maintainers of the relevant subsystem
affected.  This principle ensures a high degree of accuracy and
accountability in vulnerability reporting.  Only those individuals with
deep expertise and intimate knowledge of the subsystem can effectively
assess the validity and scope of a reported vulnerability and determine
its appropriate CVE designation.  Any attempt to modify or dispute a CVE
outside of this designated authority could lead to confusion, inaccurate
reporting, and ultimately, compromised systems.
(对特定内核更改的已分配 CVE 提出异议或进行修改的权限，完全属于受影响的相关子系统的维护者。这一原则确保了漏洞报告的高度准确性和可问责性。只有那些对该子系统具有深厚专业知识和透彻了解的人，才能有效评估所报告漏洞的有效性和范围，并确定其适当的 CVE 定级。任何在这一指定权限之外修改或质疑 CVE 的尝试都可能导致混乱、不准确的报告，并最终导致系统被攻陷。)

Invalid CVEs(无效的 CVE)
========================

If a security issue is found in a Linux kernel that is only supported by
a Linux distribution due to the changes that have been made by that
distribution, or due to the distribution supporting a kernel version
that is no longer one of the kernel.org supported releases, then a CVE
can not be assigned by the Linux kernel CVE team, and must be asked for
from that Linux distribution itself.
(如果安全问题是由于某个 Linux 发行版所做的更改，或由于该发行版支持已不再是 kernel.org 支持版本的内核版本，而只在受该发行版支持的 Linux 内核中发现，则 Linux 内核 CVE 团队无法为其分配 CVE，必须向该 Linux 发行版本身申请。)

Any CVE that is assigned against the Linux kernel for an actively
supported kernel version, by any group other than the kernel assignment
CVE team should not be treated as a valid CVE.  Please notify the
kernel CVE assignment team at <cve@kernel.org> so that they can work to
invalidate such entries through the CNA remediation process.
(任何由内核 CVE 分配团队之外的其他团体针对 Linux 内核受积极支持的内核版本分配的 CVE，都不应被视为有效的 CVE。请通知内核 CVE 分配团队 <cve@kernel.org>，以便他们通过 CNA 补救流程努力使这些条目失效。)

Applicability of specific CVEs(特定 CVE 的适用性)
=================================================

As the Linux kernel can be used in many different ways, with many
different ways of accessing it by external users, or no access at all,
the applicability of any specific CVE is up to the user of Linux to
determine, it is not up to the CVE assignment team.  Please do not
contact us to attempt to determine the applicability of any specific
CVE.
(由于 Linux 内核可以以许多不同的方式使用，外部用户可以通过许多不同的方式访问它，或者根本不访问，任何特定 CVE 的适用性由 Linux 的使用者自行判断，而不由 CVE 分配团队决定。请不要联系我们试图确定任何特定 CVE 的适用性。)

Also, as the source tree is so large, and any one system only uses a
small subset of the source tree, any users of Linux should be aware that
large numbers of assigned CVEs are not relevant for their systems.
(此外，由于源代码树非常庞大，而任何一个系统只使用源代码树的一小部分，Linux 的任何用户都应当意识到，大量已分配的 CVE 与他们的系统无关。)

In short, we do not know your use case, and we do not know what portions
of the kernel that you use, so there is no way for us to determine if a
specific CVE is relevant for your system.
(简言之，我们不知道你的使用场景，也不知道你使用了内核的哪些部分，因此我们无法确定某个特定 CVE 是否与你的系统相关。)

As always, it is best to take all released kernel changes, as they are
tested together in a unified whole by many community members, and not as
individual cherry-picked changes.  Also note that for many bugs, the
solution to the overall problem is not found in a single change, but by
the sum of many fixes on top of each other.  Ideally CVEs will be
assigned to all fixes for all issues, but sometimes we will fail to
notice fixes, therefore assume that some changes without a CVE assigned
might be relevant to take.
(一如既往，最好接受所有已发布的内核更改，因为它们由许多社区成员作为一个统一的整体一起测试，而不是作为单独的 cherry-pick 更改。另请注意，对于许多缺陷，整体问题的解决方案并不在于单个更改，而在于许多修复的叠加。理想情况下，CVE 会分配给所有问题的所有修复，但有时我们可能未能注意到某些修复，因此请假定一些未分配 CVE 的更改也可能值得采纳。)

