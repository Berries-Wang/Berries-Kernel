.. _embargoed_hardware_issues:

Embargoed hardware issues(受限披露的硬件问题)
=============================================

Scope(范围)
-----------

Hardware issues which result in security problems are a different category
of security bugs than pure software bugs which only affect the Linux
kernel.
(导致安全问题的硬件缺陷是与只影响 Linux 内核本身的纯软件缺陷不同的另一
类安全缺陷。)

Hardware issues like Meltdown, Spectre, L1TF etc. must be treated
differently because they usually affect all Operating Systems ("OS") and
therefore need coordination across different OS vendors, distributions,
silicon vendors, hardware integrators, and other parties. For some of the
issues, software mitigations can depend on microcode or firmware updates,
which need further coordination.
(像 Meltdown、Spectre、L1TF 等硬件缺陷必须区别对待，因为它们通常影响所
有操作系统 ("OS")，因而需要在不同 OS 厂商、发行版、硅芯片厂商、硬件集成
商以及其他各方之间进行协调。对其中一些问题而言，软件缓解措施可能依赖于
微码或固件更新，这又需要进一步的协调。)

.. _Contact:

Contact(联系方式)
-----------------

The Linux kernel hardware security team is separate from the regular Linux
kernel security team.
(Linux 内核硬件安全团队独立于常规的 Linux 内核安全团队。)

The team only handles developing fixes for embargoed hardware security
issues. Reports of pure software security bugs in the Linux kernel are not
handled by this team and the reporter will be guided to contact the regular
Linux kernel security team (:ref:`Documentation/admin-guide/
<securitybugs>`) instead.
(该团队只负责为受限披露的硬件安全问题开发修复。Linux 内核中纯软件安全
缺陷的报告不由该团队处理，报告者将被引导去联系常规的 Linux 内核安全团队
(:ref:`Documentation/admin-guide/
<securitybugs>`)。)

The team can be contacted by email at <hardware-security@kernel.org>. This
is a private list of security officers who will help you coordinate a fix
according to our documented process.
(可以通过电子邮件 <hardware-security@kernel.org> 联系该团队。这是一个由
安全官员组成的私密列表，他们会帮助你按照我们文档化的流程协调修复。)

The list is encrypted and email to the list can be sent by either PGP or
S/MIME encrypted and must be signed with the reporter's PGP key or S/MIME
certificate. The list's PGP key and S/MIME certificate are available from
the following URLs:
(该列表是加密的，发送到列表的邮件必须以 PGP 或 S/MIME 加密，并且必须用
报告者的 PGP 密钥或 S/MIME 证书签名。列表的 PGP 密钥和 S/MIME 证书可从
以下 URL 获取：)

  - PGP: https://www.kernel.org/static/files/hardware-security.asc
  - S/MIME: https://www.kernel.org/static/files/hardware-security.crt

While hardware security issues are often handled by the affected silicon
vendor, we welcome contact from researchers or individuals who have
identified a potential hardware flaw.
(虽然硬件安全问题通常由受影响的硅芯片厂商处理，但我们欢迎发现潜在硬件
缺陷的研究者或个人与我们联系。)

Hardware security officers(硬件安全官)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The current team of hardware security officers:
(当前的硬件安全官团队：)

  - Linus Torvalds (Linux Foundation Fellow)
  - Greg Kroah-Hartman (Linux Foundation Fellow)
  - Thomas Gleixner (Linux Foundation Fellow)

Operation of mailing-lists(邮件列表的运作)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The encrypted mailing-lists which are used in our process are hosted on
Linux Foundation's IT infrastructure. By providing this service, members
of Linux Foundation's IT operations personnel technically have the
ability to access the embargoed information, but are obliged to
confidentiality by their employment contract. Linux Foundation IT
personnel are also responsible for operating and managing the rest of
kernel.org's infrastructure.
(我们流程中使用的加密邮件列表托管在 Linux 基金会的 IT 基础设施上。通
过提供这项服务，Linux 基金会 IT 运维人员在技术上能够访问受限信息，但其
雇佣合同对其课以保密义务。Linux 基金会 IT 人员还负责运营和管理
kernel.org 的其余基础设施。)

The Linux Foundation's current director of IT Project infrastructure is
Konstantin Ryabitsev.
(Linux 基金会现任 IT 项目基础设施总监是 Konstantin Ryabitsev。)


Non-disclosure agreements(保密协议)
-----------------------------------

The Linux kernel hardware security team is not a formal body and therefore
unable to enter into any non-disclosure agreements.  The kernel community
is aware of the sensitive nature of such issues and offers a Memorandum of
Understanding instead.
(Linux 内核硬件安全团队不是一个正式机构，因此无法签署任何保密协议
(NDA)。内核社区了解此类问题的敏感性，并提供一份谅解备忘录 (Memorandum
of Understanding) 作为替代。)


Memorandum of Understanding(谅解备忘录)
---------------------------------------

The Linux kernel community has a deep understanding of the requirement to
keep hardware security issues under embargo for coordination between
different OS vendors, distributors, silicon vendors, and other parties.
(Linux 内核社区深刻理解为在不同 OS 厂商、发行版、硅芯片厂商和其他各方
之间进行协调而对硬件安全问题实施受限披露的要求。)

The Linux kernel community has successfully handled hardware security
issues in the past and has the necessary mechanisms in place to allow
community compliant development under embargo restrictions.
(Linux 内核社区过去已成功处理过硬件安全问题，并具备必要的机制，可以在
受限披露限制之下进行符合社区规范的开发。)

The Linux kernel community has a dedicated hardware security team for
initial contact, which oversees the process of handling such issues under
embargo rules.
(Linux 内核社区有一个专门的硬件安全团队负责初始联系，该团队监督在受限
披露规则下处理此类问题的全过程。)

The hardware security team identifies the developers (domain experts) who
will form the initial response team for a particular issue. The initial
response team can bring in further developers (domain experts) to address
the issue in the best technical way.
(硬件安全团队会确定将组成某一特定问题初始响应团队的开发者 (领域专
家)。初始响应团队可以引入更多开发者 (领域专家)，以便在技术上以最佳方
式解决该问题。)

All involved developers pledge to adhere to the embargo rules and to keep
the received information confidential. Violation of the pledge will lead to
immediate exclusion from the current issue and removal from all related
mailing lists. In addition, the hardware security team will also exclude
the offender from future issues. The impact of this consequence is a highly
effective deterrent in our community. In case a violation happens the
hardware security team will inform the involved parties immediately. If you
or anyone else becomes aware of a potential violation, please report it
immediately to the Hardware security officers.
(所有参与的开发者都承诺遵守受限披露规则，并对收到的信息保密。违反承诺
将导致立即被排除出当前问题，并从所有相关邮件列表中移除。此外，硬件安全
团队还会将该违规者排除在未来的问题之外。这一后果的影响在我们的社区中是
非常有效的威慑。如果发生违规，硬件安全团队会立即通知相关各方。如果你或
任何其他人得知可能存在违规，请立即向硬件安全官报告。)


Process(流程)
^^^^^^^^^^^^^

Due to the globally distributed nature of Linux kernel development,
face-to-face meetings are almost impossible to address hardware security
issues.  Phone conferences are hard to coordinate due to time zones and
other factors and should be only used when absolutely necessary. Encrypted
email has been proven to be the most effective and secure communication
method for these types of issues.
(由于 Linux 内核开发的全球分布式特性，面对面会议几乎不可能用于解决硬
件安全问题。电话会议因时区和其他因素而难以协调，只应在绝对必要时使用。
加密电子邮件已被证明是处理此类问题最有效、最安全的沟通方式。)

Start of Disclosure(披露的开始)
"""""""""""""""""""""""""""""""

Disclosure starts by emailing the Linux kernel hardware security team per
the Contact section above.  This initial contact should contain a
description of the problem and a list of any known affected silicon. If
your organization builds or distributes the affected hardware, we encourage
you to also consider what other hardware could be affected.  The disclosing
party is responsible for contacting the affected silicon vendors in a
timely manner.
(披露始于按照上文"联系方式"一节给 Linux 内核硬件安全团队发送邮件。这
一初始联系应包含问题描述以及所有已知受影响硅芯片的清单。如果你的组织制
造或分发受影响的硬件，我们鼓励你还考虑其他哪些硬件可能受影响。披露方负
责及时联系受影响的硅芯片厂商。)

The hardware security team will provide an incident-specific encrypted
mailing list which will be used for initial discussion with the reporter,
further disclosure, and coordination of fixes.
(硬件安全团队会提供一个事件专属的加密邮件列表，用于与报告者的初步讨
论、进一步披露以及修复工作的协调。)

The hardware security team will provide the disclosing party a list of
developers (domain experts) who should be informed initially about the
issue after confirming with the developers that they will adhere to this
Memorandum of Understanding and the documented process. These developers
form the initial response team and will be responsible for handling the
issue after initial contact. The hardware security team is supporting the
response team, but is not necessarily involved in the mitigation
development process.
(硬件安全团队会在与开发者确认他们将遵守本谅解备忘录及文档化流程之后，
向披露方提供一份应当最初获知该问题的开发者 (领域专家) 名单。这些开发者
组成初始响应团队，并负责在初始联系之后处理该问题。硬件安全团队为响应团
队提供支持，但不一定参与缓解措施的开发过程。)

While individual developers might be covered by a non-disclosure agreement
via their employer, they cannot enter individual non-disclosure agreements
in their role as Linux kernel developers. They will, however, agree to
adhere to this documented process and the Memorandum of Understanding.
(虽然个别开发者可能通过其雇主受保密协议覆盖，但他们不能以 Linux 内核开
发者的身份签署个人保密协议。不过，他们会同意遵守本文档化的流程和谅解备
忘录。)

The disclosing party should provide a list of contacts for all other
entities who have already been, or should be, informed about the issue.
This serves several purposes:
(披露方应提供已获知或应获知该问题的所有其他实体的联系人清单。这有几
个用途：)

 - The list of disclosed entities allows communication across the
   industry, e.g. other OS vendors, HW vendors, etc.
   (已披露实体的清单使得跨行业沟通成为可能，例如其他 OS 厂商、硬件厂
   商等。)

 - The disclosed entities can be contacted to name experts who should
   participate in the mitigation development.
   (可以联系已披露的实体，请其指名应当参与缓解措施开发的专家。)

 - If an expert who is required to handle an issue is employed by a listed
   entity or member of an listed entity, then the response teams can
   request the disclosure of that expert from that entity. This ensures
   that the expert is also part of the entity's response team.
   (如果处理问题所需的专家受雇于清单上的某实体或为其成员，则响应团队
   可以请求该实体披露该专家。这可确保该专家也是该实体响应团队的一员。)

Disclosure(披露)
""""""""""""""""

The disclosing party provides detailed information to the initial response
team via the specific encrypted mailing-list.
(披露方通过特定加密邮件列表向初始响应团队提供详细信息。)

From our experience, the technical documentation of these issues is usually
a sufficient starting point, and further technical clarification is best
done via email.
(根据我们的经验，这些问题的技术文档通常已足够作为起点，进一步的技术澄
清最好通过电子邮件完成。)

Mitigation development(缓解措施的开发)
""""""""""""""""""""""""""""""""""""""

The initial response team sets up an encrypted mailing-list or repurposes
an existing one if appropriate.
(初始响应团队会建立一个加密邮件列表，或在合适的情况下改用现有的列
表。)

Using a mailing list is close to the normal Linux development process and
has been successfully used to develop mitigations for various hardware
security issues in the past.
(使用邮件列表与正常的 Linux 开发流程接近，过去已被成功地用于开发针对
多种硬件安全问题的缓解措施。)

The mailing list operates in the same way as normal Linux development.
Patches are posted, discussed, and reviewed and if agreed upon, applied to
a non-public git repository which is only accessible to the participating
developers via a secure connection. The repository contains the main
development branch against the mainline kernel and backport branches for
stable kernel versions as necessary.
(邮件列表的运作方式与正常的 Linux 开发相同。补丁被发布、讨论、评审，
达成一致后应用到非公开的 git 仓库，该仓库只有参与的开发者能通过安全连
接访问。仓库中包含针对主线内核的主开发分支，以及必要时的 stable 内核版
本回移分支。)

The initial response team will identify further experts from the Linux
kernel developer community as needed.  Any involved party can suggest
further experts to be included, each of which will be subject to the same
requirements outlined above.
(初始响应团队会根据需要从 Linux 内核开发者社区中确定更多专家。任何参
与方都可以建议纳入更多专家，每位专家都将受到上述相同要求的约束。)

Bringing in experts can happen at any time in the development process and
needs to be handled in a timely manner.
(引入专家可以在开发过程中的任何时间进行，并且需要及时处理。)

If an expert is employed by or a member of an entity on the disclosure list
provided by the disclosing party, then participation will be requested from
the relevant entity.
(如果专家受雇于披露方提供的披露清单上的某实体或为其成员，则会向相关实
体请求其参与。)

If not, then the disclosing party will be informed about the experts'
participation. The experts are covered by the Memorandum of Understanding
and the disclosing party is requested to acknowledge their participation.
In the case where the disclosing party has a compelling reason to object,
any objection must to be raised within five working days and resolved with
the incident team immediately. If the disclosing party does not react
within five working days this is taken as silent acknowledgment.
(如果不是，则会将专家的参与告知披露方。专家受谅解备忘录覆盖，并请求披
露方确认其参与。如果披露方有充分的理由反对，任何反对都必须在五个工作日
内提出，并立即与事件团队解决。如果披露方在五个工作日内没有回应，则视为
默认同意。)

After the incident team acknowledges or resolves an objection, the expert
is disclosed and brought into the development process.
(在事件团队认可或解决反对意见之后，该专家即被披露并加入开发过程。)

List participants may not communicate about the issue outside of the
private mailing list. List participants may not use any shared resources
(e.g. employer build farms, CI systems, etc) when working on patches.
(列表参与者不得在私密邮件列表之外就该问题进行沟通。列表参与者在处理
补丁时不得使用任何共享资源 (例如雇主的构建农场、CI 系统等)。)

Early access(早期获取)
""""""""""""""""""""""

The patches discussed and developed on the list can neither be distributed
to any individual who is not a member of the response team nor to any other
organization.
(在列表上讨论和开发的补丁既不能分发给任何不是响应团队成员的个人，也
不能分发给任何其他组织。)

To allow the affected silicon vendors to work with their internal teams and
industry partners on testing, validation, and logistics, the following
exception is provided:
(为了让受影响的硅芯片厂商能够与其内部团队和行业合作伙伴一起进行测试、
验证和后勤工作，提供以下例外：)

	Designated representatives of the affected silicon vendors are
	allowed to hand over the patches at any time to the silicon
	vendor’s response team. The representative must notify the kernel
	response team about the handover. The affected silicon vendor must
	have and maintain their own documented security process for any
	patches shared with their response team that is consistent with
	this policy.
	(受影响硅芯片厂商的指定代表可以在任何时候把补丁交给该硅芯片厂商
	的响应团队。该代表必须把交接事宜通知内核响应团队。受影响的硅芯片
	厂商必须拥有并维护自己的文档化安全流程，用于与其响应团队共享的任
	何补丁，且该流程须与本政策一致。)

	The silicon vendor’s response team can distribute these patches to
	their industry partners and to their internal teams under the
	silicon vendor’s documented security process. Feedback from the
	industry partners goes back to the silicon vendor and is
	communicated by the silicon vendor to the kernel response team.
	(硅芯片厂商的响应团队可以在其文档化安全流程之下，把这些补丁分发
	给其行业合作伙伴和内部团队。来自行业合作伙伴的反馈返回给硅芯片厂
	商，并由硅芯片厂商传达给内核响应团队。)

	The handover to the silicon vendor’s response team removes any
	responsibility or liability from the kernel response team regarding
	premature disclosure, which happens due to the involvement of the
	silicon vendor’s internal teams or industry partners. The silicon
	vendor guarantees this release of liability by agreeing to this
	process.
	(向硅芯片厂商响应团队的交接解除了内核响应团队对过早披露的一切责
	任或义务，这种过早披露可能因硅芯片厂商内部团队或行业合作伙伴的参
	与而发生。硅芯片厂商通过同意本流程来保证这一责任免除。)

Coordinated release(协调发布)
"""""""""""""""""""""""""""""

The involved parties will negotiate the date and time when the embargo
ends. At that point, the prepared mitigations are published into the
relevant kernel trees.  There is no pre-notification process: the
mitigations are published in public and available to everyone at the same
time.
(相关各方将协商受限披露结束的日期和时间。届时，准备好的缓解措施会发
布到相关的内核树中。没有预先通知流程：缓解措施公开发布，所有人同时可
得。)

While we understand that hardware security issues need coordinated embargo
time, the embargo time should be constrained to the minimum time that is
required for all involved parties to develop, test, and prepare their
mitigations. Extending embargo time artificially to meet conference talk
dates or other non-technical reasons creates more work and burden for the
involved developers and response teams as the patches need to be kept up to
date in order to follow the ongoing upstream kernel development, which
might create conflicting changes.
(虽然我们理解硬件安全问题需要协调一致的受限披露时间，但受限披露时间
应限于所有相关方开发、测试并准备好各自缓解措施所需的最短时间。为了赶会
议演讲日期或其他非技术原因而人为延长受限披露时间，会给相关开发者和响应
团队带来更多工作和负担，因为补丁必须持续更新以跟上上游内核开发的进程，
这可能产生相互冲突的变更。)

CVE assignment(CVE 分配)
""""""""""""""""""""""""

Neither the hardware security team nor the initial response team assign
CVEs, nor are CVEs required for the development process. If CVEs are
provided by the disclosing party they can be used for documentation
purposes.
(硬件安全团队和初始响应团队都不分配 CVE，开发过程也不要求 CVE。如果
披露方提供了 CVE，则可用于文档目的。)

Process ambassadors(流程大使)
-----------------------------

For assistance with this process we have established ambassadors in various
organizations, who can answer questions about or provide guidance on the
reporting process and further handling. Ambassadors are not involved in the
disclosure of a particular issue, unless requested by a response team or by
an involved disclosed party. The current ambassadors list:
(为了在这一流程中提供协助，我们在各个组织中设立了大使，他们可以解答
有关报告流程及后续处理的问题或提供指导。大使不参与特定问题的披露，除非
响应团队或相关已披露方提出请求。现任大使列表：)

  ============= ========================================================
  AMD		Tom Lendacky <thomas.lendacky@amd.com>
  Ampere	Darren Hart <darren@os.amperecomputing.com>
  ARM		Catalin Marinas <catalin.marinas@arm.com>
  IBM Power	Madhavan Srinivasan <maddy@linux.ibm.com>
  IBM Z		Christian Borntraeger <borntraeger@de.ibm.com>
  Intel		Tony Luck <tony.luck@intel.com>
  Qualcomm	Trilok Soni <quic_tsoni@quicinc.com>
  RISC-V	Palmer Dabbelt <palmer@dabbelt.com>
  Samsung	Javier González <javier.gonz@samsung.com>

  Microsoft	James Morris <jamorris@linux.microsoft.com>
  Xen		Andrew Cooper <andrew.cooper3@citrix.com>

  Canonical	John Johansen <john.johansen@canonical.com>
  Debian	Ben Hutchings <ben@decadent.org.uk>
  Oracle	Konrad Rzeszutek Wilk <konrad.wilk@oracle.com>
  Red Hat	Josh Poimboeuf <jpoimboe@redhat.com>
  SUSE		Jiri Kosina <jkosina@suse.cz>

  Google	Kees Cook <keescook@chromium.org>

  LLVM		Nick Desaulniers <nick.desaulniers+lkml@gmail.com>
  ============= ========================================================

If you want your organization to be added to the ambassadors list, please
contact the hardware security team. The nominated ambassador has to
understand and support our process fully and is ideally well-connected in
the Linux kernel community.
(如果你想让自己的组织加入大使列表，请联系硬件安全团队。被提名的大使
必须充分理解并全力支持我们的流程，并且最好在 Linux 内核社区中人脉广
泛。)

Encrypted mailing-lists(加密邮件列表)
-------------------------------------

We use encrypted mailing lists for communication. The operating principle
of these lists is that email sent to the list is encrypted either with the
list's PGP key or with the list's S/MIME certificate. The mailing list
software decrypts the email and re-encrypts it individually for each
subscriber with the subscriber's PGP key or S/MIME certificate. Details
about the mailing list software and the setup that is used to ensure the
security of the lists and protection of the data can be found here:
https://korg.wiki.kernel.org/userdoc/remail.
(我们使用加密邮件列表进行沟通。这些列表的运作原则是：发送到列表的邮
件要么用列表的 PGP 密钥加密，要么用列表的 S/MIME 证书加密。邮件列表软
件会解密邮件，并为每个订阅者用其 PGP 密钥或 S/MIME 证书逐一重新加密。
有关邮件列表软件以及用于确保列表安全和保护数据的设置的详细信息，见
https://korg.wiki.kernel.org/userdoc/remail 。)

List keys(列表密钥)
^^^^^^^^^^^^^^^^^^^

For initial contact see the :ref:`Contact` section above. For incident
specific mailing lists, the key and S/MIME certificate are conveyed to the
subscribers by email sent from the specific list.
(初始联系人见上文 :ref:`Contact` 一节。对于事件专属邮件列表，密钥和
S/MIME 证书由来自该特定列表的邮件传达给订阅者。)

Subscription to incident-specific lists(订阅事件专属列表)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Subscription to incident-specific lists is handled by the response teams.
Disclosed parties who want to participate in the communication send a list
of potential experts to the response team so the response team can validate
subscription requests.
(事件专属列表的订阅由响应团队处理。希望参与沟通的已披露方向响应团队
发送潜在专家名单，以便响应团队校验订阅请求。)

Each subscriber needs to send a subscription request to the response team
by email. The email must be signed with the subscriber's PGP key or S/MIME
certificate. If a PGP key is used, it must be available from a public key
server and is ideally connected to the Linux kernel's PGP web of trust. See
also: https://www.kernel.org/signature.html.
(每位订阅者都需要通过电子邮件向响应团队发送订阅请求。该邮件必须用订阅
者的 PGP 密钥或 S/MIME 证书签名。如果使用 PGP 密钥，则该密钥必须可以从
公钥服务器获取，并且最好接入 Linux 内核的 PGP 信任网络。另见：
https://www.kernel.org/signature.html 。)

The response team verifies that the subscriber request is valid and adds
the subscriber to the list. After subscription the subscriber will receive
email from the mailing-list which is signed either with the list's PGP key
or the list's S/MIME certificate. The subscriber's email client can extract
the PGP key or the S/MIME certificate from the signature so the subscriber
can send encrypted email to the list.
(响应团队会校验订阅请求是否有效，并把订阅者加入列表。订阅之后，订阅
者将收到来自邮件列表的邮件，这些邮件用列表的 PGP 密钥或列表的 S/MIME
证书签名。订阅者的邮件客户端可以从签名中提取 PGP 密钥或 S/MIME 证书，
从而使订阅者能够向列表发送加密邮件。)

