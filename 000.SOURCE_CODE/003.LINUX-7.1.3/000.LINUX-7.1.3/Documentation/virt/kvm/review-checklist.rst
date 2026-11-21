.. SPDX-License-Identifier: GPL-2.0

==================================================
Review checklist for kvm patches(kvm 补丁评审清单)
==================================================

1.  The patch must follow Documentation/process/coding-style.rst and
    Documentation/process/submitting-patches.rst.
    (补丁必须遵循 Documentation/process/coding-style.rst 和 Documentation/process/submitting-patches.rst。)

2.  Patches should be against kvm.git master or next branches.
    (补丁应基于 kvm.git 的 master 或 next 分支。)

3.  If the patch introduces or modifies a new userspace API:
    (如果补丁引入或修改了新的用户空间 API：)

    - the API must be documented in Documentation/virt/kvm/api.rst
      (该 API 必须在 Documentation/virt/kvm/api.rst 中有文档说明)
    - the API must be discoverable using KVM_CHECK_EXTENSION
      (该 API 必须可以通过 KVM_CHECK_EXTENSION 发现)

4.  New state must include support for save/restore.
    (新状态必须包含对保存/恢复的支持。)

5.  New features must default to off (userspace should explicitly request them).
    Performance improvements can and should default to on.
    (新特性必须默认关闭（用户空间应显式请求它们）。性能改进可以且应该默认开启。)

6.  New cpu features should be exposed via KVM_GET_SUPPORTED_CPUID2,
    or its equivalent for non-x86 architectures
    (新的 CPU 特性应通过 KVM_GET_SUPPORTED_CPUID2 暴露，或对非 x86 架构使用其等价物)

7.  The feature should be testable (see below).
    (该特性应当是可测试的（见下文）。)

8.  Changes should be vendor neutral when possible.  Changes to common code
    are better than duplicating changes to vendor code.
    (变更在可能的情况下应保持供应商中立。对公共代码的更改优于对供应商代码重复进行更改。)

9.  Similarly, prefer changes to arch independent code than to arch dependent
    code.
    (同样，优先更改与架构无关的代码，而不是与架构相关的代码。)

10. User/kernel interfaces and guest/host interfaces must be 64-bit clean
    (all variables and sizes naturally aligned on 64-bit; use specific types
    only - u64 rather than ulong).
    (用户/内核接口和客户机/宿主机接口必须是 64 位干净的（所有变量和大小自然地按 64 位对齐；仅使用特定类型——用 u64 而不是 ulong）。)

11. New guest visible features must either be documented in a hardware manual
    or be accompanied by documentation.
    (新的客户机可见特性必须记录在硬件手册中，或附带文档。)

Testing of KVM code(KVM 代码的测试)
-----------------------------------

All features contributed to KVM, and in many cases bugfixes too, should be
accompanied by some kind of tests and/or enablement in open source guests
and VMMs.  KVM is covered by multiple test suites:
(所有贡献给 KVM 的特性（在很多情况下也包括缺陷修复）都应附带某种测试，和/或在开源客户机和 VMM 中提供启用支持。KVM 由多个测试套件覆盖：)

*Selftests*
  These are low level tests that allow granular testing of kernel APIs.
  This includes API failure scenarios, invoking APIs after specific
  guest instructions, and testing multiple calls to ``KVM_CREATE_VM``
  within a single test.  They are included in the kernel tree at
  ``tools/testing/selftests/kvm``.
  (这些是低层次的测试，允许对内核 API 进行细粒度测试。这包括 API 失败场景、在特定客户机指令之后调用 API，以及在单个测试中对 ``KVM_CREATE_VM`` 进行多次调用。它们包含在内核树中的 ``tools/testing/selftests/kvm`` 中。)

``kvm-unit-tests``
  A collection of small guests that test CPU and emulated device features
  from a guest's perspective.  They run under QEMU or ``kvmtool``, and
  are generally not KVM-specific: they can be run with any accelerator
  that QEMU support or even on bare metal, making it possible to compare
  behavior across hypervisors and processor families.
  (这是一组小型客户机，从客户机的角度测试 CPU 和模拟设备特性。它们在 QEMU 或 ``kvmtool`` 下运行，通常不是 KVM 特有的：它们可以在 QEMU 支持的任何加速器上运行，甚至在裸机上运行，从而可以比较不同管理程序和处理器家族之间的行为。)

Functional test suites
  Various sets of functional tests exist, such as QEMU's ``tests/functional``
  suite and `avocado-vt <https://avocado-vt.readthedocs.io/en/latest/>`__.
  These typically involve running a full operating system in a virtual
  machine.
  (存在各种功能测试套件，例如 QEMU 的 ``tests/functional`` 套件和 `avocado-vt <https://avocado-vt.readthedocs.io/en/latest/>`__ 。这些测试通常涉及在虚拟机中运行完整的操作系统。)

The best testing approach depends on the feature's complexity and
operation. Here are some examples and guidelines:
(最佳测试方法取决于特性的复杂程度和操作方式。以下是一些示例和指导原则：)

New instructions (no new registers or APIs)
  The corresponding CPU features (if applicable) should be made available
  in QEMU.  If the instructions require emulation support or other code in
  KVM, it is worth adding coverage to ``kvm-unit-tests`` or selftests;
  the latter can be a better choice if the instructions relate to an API
  that already has good selftest coverage.
  (相应的 CPU 特性（如适用）应在 QEMU 中提供。如果这些指令需要 KVM 中的模拟支持或其他代码，值得在 ``kvm-unit-tests`` 或 selftests 中添加覆盖；如果这些指令与已经有良好 selftest 覆盖的 API 相关，后者可能是更好的选择。)

New hardware features (new registers, no new APIs)
  These should be tested via ``kvm-unit-tests``; this more or less implies
  supporting them in QEMU and/or ``kvmtool``.  In some cases selftests
  can be used instead, similar to the previous case, or specifically to
  test corner cases in guest state save/restore.
  (这些应通过 ``kvm-unit-tests`` 测试；这多少意味着需要在 QEMU 和/或 ``kvmtool`` 中支持它们。在某些情况下可以改用 selftests，类似于前一种情况，或专门用于测试客户机状态保存/恢复的边界情况。)

Bug fixes and performance improvements
  These usually do not introduce new APIs, but it's worth sharing
  any benchmarks and tests that will validate your contribution,
  ideally in the form of regression tests.  Tests and benchmarks
  can be included in either ``kvm-unit-tests`` or selftests, depending
  on the specifics of your change.  Selftests are especially useful for
  regression tests because they are included directly in Linux's tree.
  (这些通常不引入新的 API，但值得分享任何可以验证你的贡献的基准测试和测试，最好以回归测试的形式。测试和基准测试可以包含在 ``kvm-unit-tests`` 或 selftests 中，具体取决于你更改的细节。Selftests 对于回归测试特别有用，因为它们直接包含在 Linux 的代码树中。)

Large scale internal changes
  While it's difficult to provide a single policy, you should ensure that
  the changed code is covered by either ``kvm-unit-tests`` or selftests.
  In some cases the affected code is run for any guests and functional
  tests suffice.  Explain your testing process in the cover letter,
  as that can help identify gaps in existing test suites.
  (虽然难以提供单一的策略，但你应确保被更改的代码被 ``kvm-unit-tests`` 或 selftests 覆盖。在某些情况下，受影响的代码会在所有客户机上运行，功能测试就足够了。在封面信中说明你的测试过程，这有助于发现现有测试套件中的缺口。)

New APIs
  It is important to demonstrate your use case.  This can be as simple as
  explaining that the feature is already in use on bare metal, or it can be
  a proof-of-concept implementation in userspace.  The latter need not be
  open source, though that is of course preferable for easier testing.
  Selftests should test corner cases of the APIs, and should also cover
  basic host and guest operation if no open source VMM uses the feature.
  (展示你的用例非常重要。这可以简单到解释该特性已在裸机上使用，也可以是用户空间中的概念验证实现。后者不必开源，尽管为了便于测试这当然是更好的选择。Selftests 应测试 API 的边界情况，如果没有开源 VMM 使用该特性，还应涵盖基本的宿主机和客户机操作。)

Bigger features, usually spanning host and guest
  These should be supported by Linux guests, with limited exceptions for
  Hyper-V features that are testable on Windows guests.  It is strongly
  suggested that the feature be usable with an open source host VMM, such
  as at least one of QEMU or crosvm, and guest firmware.  Selftests should
  test at least API error cases.  Guest operation can be covered by
  either selftests of ``kvm-unit-tests`` (this is especially important for
  paravirtualized and Windows-only features).  Strong selftest coverage
  can also be a replacement for implementation in an open source VMM,
  but this is generally not recommended.
  (这些应由 Linux 客户机支持，对于可在 Windows 客户机上测试的 Hyper-V 特性可以有有限的例外。强烈建议该特性可与开源宿主机 VMM（例如 QEMU 或 crosvm 中的至少一个）以及客户机固件一起使用。Selftests 应至少测试 API 错误情况。客户机操作可以由 selftests 或 ``kvm-unit-tests`` 覆盖（这对半虚拟化和仅限 Windows 的特性尤其重要）。强大的 selftest 覆盖也可以替代在开源 VMM 中的实现，但通常不推荐这样做。)

Following the above suggestions for testing in selftests and
``kvm-unit-tests`` will make it easier for the maintainers to review
and accept your code.  In fact, even before you contribute your changes
upstream it will make it easier for you to develop for KVM.
(遵循上述关于在 selftests 和 ``kvm-unit-tests`` 中进行测试的建议，将使维护者更容易评审和接受你的代码。事实上，即使在你向上游贡献更改之前，它也会使你更容易为 KVM 进行开发。)

Of course, the KVM maintainers reserve the right to require more tests,
though they may also waive the requirement from time to time.
(当然，KVM 维护者保留要求更多测试的权利，尽管他们有时也可能免除这一要求。)
