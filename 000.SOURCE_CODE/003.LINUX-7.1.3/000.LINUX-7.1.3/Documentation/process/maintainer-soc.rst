.. SPDX-License-Identifier: GPL-2.0

=============
SoC Subsystem
=============

Overview(概述)
--------------

The SoC subsystem is a place of aggregation for SoC-specific code.
The main components of the subsystem are:
(SoC 子系统是 SoC 相关代码的汇聚之地。该子系统的主要组成部分包括：)

* devicetrees (DTS) for 32- & 64-bit ARM and RISC-V
* 32-bit ARM board files (arch/arm/mach*)
* 32- & 64-bit ARM defconfigs
* SoC-specific drivers across architectures, in particular for 32- & 64-bit
  ARM, RISC-V and Loongarch
(32 位与 64 位 ARM 及 RISC-V 的 devicetree(DTS)；32 位 ARM 板级文件(arch/arm/mach*)；32 位与 64 位 ARM 的 defconfig；跨架构的 SoC 专用驱动，尤其是 32 位与 64 位 ARM、RISC-V 和 Loongarch。)

These "SoC-specific drivers" do not include clock, GPIO etc drivers that have
other top-level maintainers. The drivers/soc/ directory is generally meant
for kernel-internal drivers that are used by other drivers to provide SoC-
specific functionality like identifying an SoC revision or interfacing with
power domains.
(这些"SoC 专用驱动"不包括时钟、GPIO 等拥有其他顶层维护者的驱动。drivers/soc/ 目录一般用于内核内部的驱动，供其他驱动使用，以提供 SoC 特有的功能，例如识别 SoC 版本或与电源域交互。)

The SoC subsystem also serves as an intermediate location for changes to
drivers/bus, drivers/firmware, drivers/reset and drivers/memory.  The addition
of new platforms, or the removal of existing ones, often go through the SoC
tree as a dedicated branch covering multiple subsystems.
(SoC 子系统还充当 drivers/bus、drivers/firmware、drivers/reset 和 drivers/memory 变更的中转站。新平台的加入或现有平台的移除，通常会经由 SoC 树上一个覆盖多个子系统的专门分支来完成。)

The main SoC tree is housed on git.kernel.org:
(SoC 主树托管在 git.kernel.org 上：)

  https://git.kernel.org/pub/scm/linux/kernel/git/soc/soc.git/

Maintainers(维护者)
-------------------

Clearly this is quite a wide range of topics, which no one person, or even
small group of people are capable of maintaining.  Instead, the SoC subsystem
is comprised of many submaintainers (platform maintainers), each taking care of
individual platforms and driver subdirectories.
(显然，这里涉及的主题范围相当广泛，没有任何一个人、甚至一小群人能够维护所有内容。因此，SoC 子系统由许多子维护者(平台维护者)组成，每人负责各自的平台和驱动子目录。)

In this regard, "platform" usually refers to a series of SoCs from a given
vendor, for example, Nvidia's series of Tegra SoCs.  Many submaintainers operate
on a vendor level, responsible for multiple product lines.  For several reasons,
including acquisitions/different business units in a company, things vary
significantly here.  The various submaintainers are documented in the
MAINTAINERS file.
(在这一点上，"平台"通常指来自特定厂商的一系列 SoC，例如 Nvidia 的 Tegra SoC 系列。许多子维护者在厂商级别上运作，负责多条产品线。出于多种原因(包括公司收购/不同的业务部门)，具体情况差异很大。各个子维护者都记录在 MAINTAINERS 文件中。)

Most of these submaintainers have their own trees where they stage patches,
sending pull requests to the main SoC tree.  These trees are usually, but not
always, listed in MAINTAINERS.
(这些子维护者中的大多数拥有自己的树，用于暂存补丁，然后向 SoC 主树发送拉取请求(pull request)。这些树通常(但不总是)列在 MAINTAINERS 中。)

What the SoC tree is not, however, is a location for architecture-specific code
changes.  Each architecture has its own maintainers that are responsible for
architectural details, CPU errata and the like.
(不过，SoC 树并不是存放架构特定代码变更的地方。每个架构都有自己的维护者，负责架构细节、CPU 缓误(errata)等事务。)

Submitting Patches for Given SoC(为给定 SoC 提交补丁)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

All typical platform related patches should be sent via SoC submaintainers
(platform-specific maintainers).  This includes also changes to per-platform or
shared defconfigs. Note that scripts/get_maintainer.pl might not provide
correct addresses for the shared defconfig, so ignore its output and manually
create CC-list based on MAINTAINERS file or use something like
``scripts/get_maintainer.pl -f drivers/soc/FOO/``).
(所有典型的平台相关补丁都应通过 SoC 子维护者(平台特定维护者)提交。这也包括对每个平台或共享 defconfig 的更改。注意，scripts/get_maintainer.pl 可能无法为共享 defconfig 提供正确的地址，因此请忽略其输出，根据 MAINTAINERS 文件手动创建 CC 列表，或使用类似 ``scripts/get_maintainer.pl -f drivers/soc/FOO/`` 的命令。)

Submitting Patches to the Main SoC Maintainers(向 SoC 主维护者提交补丁)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The main SoC maintainers can be reached via the alias soc@kernel.org only in
following cases:
(只有在以下情况下，才可以通过 soc@kernel.org 别名联系 SoC 主维护者：)

1. There are no platform-specific maintainers.
   (不存在平台特定的维护者。)

2. Platform-specific maintainers are unresponsive.
   (平台特定的维护者没有回应。)

3. Introducing a completely new SoC platform.  Such new SoC work should be sent
   first to common mailing lists, pointed out by scripts/get_maintainer.pl, for
   community review.  After positive community review, work should be sent to
   soc@kernel.org in one patchset containing new arch/foo/Kconfig entry, DTS
   files, MAINTAINERS file entry and optionally initial drivers with their
   Devicetree bindings.  The MAINTAINERS file entry should list new
   platform-specific maintainers, who are going to be responsible for handling
   patches for the platform from now on.
   (引入全新的 SoC 平台。这类新的 SoC 工作应首先发送到 scripts/get_maintainer.pl 指出的公共邮件列表，以接受社区评审。在获得积极的社区评审之后，工作应以一个补丁集的形式发送到 soc@kernel.org，其中包含新的 arch/foo/Kconfig 条目、DTS 文件、MAINTAINERS 文件条目，以及可选的带 Devicetree 绑定的初始驱动。MAINTAINERS 文件条目应列出新平台特定的维护者，从今以后由他们负责处理该平台的补丁。)

Note that the soc@kernel.org is usually not the place to discuss the patches,
thus work sent to this address should be already considered as acceptable by
the community.
(注意，soc@kernel.org 通常不是讨论补丁的场所，因此发送到该地址的工作应当已经获得社区的认可。)

Information for (new) Submaintainers(给(新)子维护者的信息)
----------------------------------------------------------

As new platforms spring up, they often bring with them new submaintainers,
many of whom work for the silicon vendor, and may not be familiar with the
process.
(随着新平台不断涌现，它们往往带来新的子维护者，其中许多人为芯片厂商工作，可能对这一流程并不熟悉。)

Devicetree ABI Stability(Devicetree ABI 稳定性)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Perhaps one of the most important things to highlight is that dt-bindings
document the ABI between the devicetree and the kernel.
Please read Documentation/devicetree/bindings/ABI.rst.
(也许最需要强调的一点是：dt-bindings 记录了 devicetree 与内核之间的 ABI。请阅读 Documentation/devicetree/bindings/ABI.rst。)

If changes are being made to a DTS that are incompatible with old
kernels, the DTS patch should not be applied until the driver is, or an
appropriate time later.  Most importantly, any incompatible changes should be
clearly pointed out in the patch description and pull request, along with the
expected impact on existing users, such as bootloaders or other operating
systems.
(如果对 DTS 所做的更改与旧内核不兼容，那么在驱动合入(或适当的时机)之前，不应应用该 DTS 补丁。最重要的是，任何不兼容的更改都应在补丁描述和拉取请求中明确指出，并说明对现有用户(如引导加载程序或其他操作系统)的预期影响。)

Driver Branch Dependencies(驱动分支依赖)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A common problem is synchronizing changes between device drivers and devicetree
files. Even if a change is compatible in both directions, this may require
coordinating how the changes get merged through different maintainer trees.
(一个常见问题是在设备驱动与 devicetree 文件之间同步更改。即使更改在两个方向上都是兼容的，也可能需要协调更改如何通过不同的维护者树合并。)

Usually the branch that includes a driver change will also include the
corresponding change to the devicetree binding description, to ensure they are
in fact compatible.  This means that the devicetree branch can end up causing
warnings in the ``make dtbs_check`` step.  If a devicetree change depends on
missing additions to a header file in include/dt-bindings/, it will fail the
``make dtbs`` step and not get merged.
(通常，包含驱动更改的分支也会包含对 devicetree 绑定描述的相应更改，以确保二者确实兼容。这意味着 devicetree 分支最终可能在 ``make dtbs_check`` 步骤中产生警告。如果某个 devicetree 更改依赖于 include/dt-bindings/ 中头文件尚不存在的添加项，它将无法通过 ``make dtbs`` 步骤，从而不会被合并。)

There are multiple ways to deal with this:
(有多种处理方法：)

* Avoid defining custom macros in include/dt-bindings/ for hardware constants
  that can be derived from a datasheet -- binding macros in header files should
  only be used as a last resort if there is no natural way to define a binding
  (避免在 include/dt-bindings/ 中为可从数据手册推导出的硬件常量定义自定义宏——头文件中的绑定宏只应作为最后手段，在没有自然的方式来定义绑定时使用)

* Use literal values in the devicetree file in place of macros even when a
  header is required, and change them to the named representation in a
  following release
  (即使在需要头文件的情况下，也在 devicetree 文件中使用字面值代替宏，并在后续版本中将其改为命名表示形式)

* Defer the devicetree changes to a release after the binding and driver have
  already been merged
  (把 devicetree 更改推迟到绑定和驱动都已合并之后的那个版本)

* Change the bindings in a shared immutable branch that is used as the base for
  both the driver change and the devicetree changes
  (在一个共享的不可变分支中修改绑定，该分支同时作为驱动更改和 devicetree 更改的基础)

* Add duplicate defines in the devicetree file guarded by an #ifndef section,
  removing them in a later release
  (在 devicetree 文件中添加由 #ifndef 段保护的重复定义，并在后续版本中移除它们)

Devicetree Naming Convention(Devicetree 命名约定)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The general naming scheme for devicetree files is as follows.  The aspects of a
platform that are set at the SoC level, like CPU cores, are contained in a file
named $soc.dtsi, for example, jh7100.dtsi.  Integration details, that will vary
from board to board, are described in $soc-$board.dts.  An example of this is
jh7100-beaglev-starlight.dts.  Often many boards are variations on a theme, and
frequently there are intermediate files, such as jh7100-common.dtsi, which sit
between the $soc.dtsi and $soc-$board.dts files, containing the descriptions of
common hardware.
(devicetree 文件的一般命名规则如下：在 SoC 层面确定的平台特性(如 CPU 核心)包含在名为 $soc.dtsi 的文件中，例如 jh7100.dtsi。随板卡而异的集成细节在 $soc-$board.dts 中描述，例如 jh7100-beaglev-starlight.dts。许多板卡常常是同一主题的变体，因此经常存在中间文件，例如 jh7100-common.dtsi，它们位于 $soc.dtsi 与 $soc-$board.dts 文件之间，包含公共硬件的描述。)

Some platforms also have System on Modules, containing an SoC, which are then
integrated into several different boards. For these platforms, $soc-$som.dtsi
and $soc-$som-$board.dts are typical.
(一些平台还有包含 SoC 的系统级模块(System on Module)，然后再集成到几块不同的板卡上。对于这类平台，典型的命名是 $soc-$som.dtsi 和 $soc-$som-$board.dts。)

Directories are usually named after the vendor of the SoC at the time of its
inclusion, leading to some historical directory names in the tree.
(目录通常以 SoC 被收录时的厂商命名，因而在树中留下了一些历史目录名。)

Validating Devicetree Files(校验 Devicetree 文件)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``make dtbs_check`` can be used to validate that devicetree files are compliant
with the dt-bindings that describe the ABI.  Please read the section
"Running checks" of Documentation/devicetree/bindings/writing-schema.rst for
more information on the validation of devicetrees.
( ``make dtbs_check`` 可用于校验 devicetree 文件是否符合描述 ABI 的 dt-bindings。关于 devicetree 校验的更多信息，请阅读 Documentation/devicetree/bindings/writing-schema.rst 的 "Running checks" 一节。)

For new platforms, or additions to existing ones, ``make dtbs_check`` should not
add any new warnings.  For RISC-V and Samsung SoC, ``make dtbs_check W=1`` is
required to not add any new warnings.
(对于新平台或现有平台的新增内容， ``make dtbs_check`` 不应新增任何警告。对于 RISC-V 和 Samsung SoC，则需要 ``make dtbs_check W=1`` 才不新增任何警告。)

Branches and Pull Requests(分支与拉取请求)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Just as the main SoC tree has several branches, it is expected that
submaintainers will do the same. Driver, defconfig and devicetree changes should
all be split into separate branches and appear in separate pull requests to the
SoC maintainers.  Each branch should be usable by itself and avoid
regressions that originate from dependencies on other branches.
(正如 SoC 主树有若干分支一样，子维护者也应当这样做。驱动、defconfig 和 devicetree 更改都应拆分到单独的分支中，并出现在发给 SoC 维护者的单独拉取请求里。每个分支都应可独立使用，并避免由对其他分支的依赖所导致的回归。)

Small sets of patches can also be sent as separate emails to soc@kernel.org,
grouped into the same categories.
(少量补丁也可以作为单独的邮件发送到 soc@kernel.org，按同样的类别分组。)

If changes do not fit into the normal patterns, there can be additional
top-level branches, e.g. for a treewide rework, or the addition of new SoC
platforms including dts files and drivers.
(如果更改不符合常规模式，可以有额外的顶层分支，例如用于全树范围的重构，或者用于添加包含 dts 文件和驱动的新 SoC 平台。)

Branches with a lot of changes can benefit from getting split up into separate
topics branches, even if they end up getting merged into the same branch of the
SoC tree.  An example here would be one branch for devicetree warning fixes, one
for a rework and one for newly added boards.
(包含大量更改的分支可以受益于拆分成单独的主题(topic)分支，即使它们最终会合并到 SoC 树的同一个分支中。例如：一个分支用于 devicetree 警告修复，一个用于重构，一个用于新添加的板卡。)

Another common way to split up changes is to send an early pull request with the
majority of the changes at some point between rc1 and rc4, following up with one
or more smaller pull requests towards the end of the cycle that can add late
changes or address problems identified while testing the first set.
(另一种常见的拆分方式是：在 rc1 与 rc4 之间的某个时间点，把大部分更改作为一个早期拉取请求发出，然后在合并周期接近尾声时跟进一个或多个较小的拉取请求，用以添加迟来的更改，或解决在测试第一组更改时发现的问题。)

While there is no cut-off time for late pull requests, it helps to only send
small branches as time gets closer to the merge window.
(虽然迟到的拉取请求没有截止时间，但随着合并窗口的临近，只发送小分支会有所帮助。)

Pull requests for bugfixes for the current release can be sent at any time, but
again having multiple smaller branches is better than trying to combine too many
patches into one pull request.
(针对当前版本 bug 修复的拉取请求可以随时发送，但同样地，拥有多个较小的分支比试图把太多补丁塞进一个拉取请求更好。)

The subject line of a pull request should begin with "[GIT PULL]" and made using
a signed tag, rather than a branch.  This tag should contain a short description
summarising the changes in the pull request.  For more detail on sending pull
requests, please see Documentation/maintainer/pull-requests.rst.
(拉取请求的主题行应以 "[GIT PULL]" 开头，并使用带签名的标签(tag)而不是分支来创建。该标签应包含一段简短说明，概述拉取请求中的更改。关于发送拉取请求的更多细节，请参见 Documentation/maintainer/pull-requests.rst。)

Defconfigs purpose(defconfig 的用途)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Defconfigs are primarily used by the kernel developers, because distros have
their own configs.  A change adding new CONFIG options to a defconfig should
explain why the kernel developers in general would want such option, e.g. by
providing a name of an upstream-supported machine/board using that new option.
This implies that enabling options in defconfig for non-upstream machines shall
not be accepted.
(defconfig 主要由内核开发者使用，因为发行版有自己的配置。向 defconfig 添加新 CONFIG 选项的更改应说明内核开发者普遍为何需要该选项，例如提供一台使用该新选项的上游受支持机器/板卡的名称。这意味着，为非上游机器在 defconfig 中启用选项将不会被接受。)
