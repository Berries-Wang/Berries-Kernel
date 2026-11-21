.. SPDX-License-Identifier: GPL-2.0+

==============================================================
Automated testing of the DRM subsystem(DRM 子系统的自动化测试)
==============================================================

Introduction(简介)
==================

Making sure that changes to the core or drivers don't introduce regressions can
be very time-consuming when lots of different hardware configurations need to
be tested. Moreover, it isn't practical for each person interested in this
testing to have to acquire and maintain what can be a considerable amount of
hardware.
(当需要测试大量不同的硬件配置时，确保对核心或驱动的更改不引入回归可能非常耗时。此外，让每个对这种测试感兴趣的人都去获取并维护可能相当数量的硬件，也是不现实的。)

Also, it is desirable for developers to check for regressions in their code by
themselves, instead of relying on the maintainers to find them and then
reporting back.
(同样，希望开发者能够自己检查代码中的回归，而不是依赖维护者去发现它们然后再反馈回来。)

There are facilities in gitlab.freedesktop.org to automatically test Mesa that
can be used as well for testing the DRM subsystem. This document explains how
people interested in testing it can use this shared infrastructure to save
quite some time and effort.
(gitlab.freedesktop.org 中有一些用于自动测试 Mesa 的设施，同样可以用于测试 DRM 子系统。本文解释了对测试感兴趣的人如何使用这种共享基础设施来节省大量时间和精力。)


Relevant files(相关文件)
========================

drivers/gpu/drm/ci/gitlab-ci.yml(GitLab CI 根配置文件)
------------------------------------------------------

This is the root configuration file for GitLab CI. Among other less interesting
bits, it specifies the specific version of the scripts to be used. There are
some variables that can be modified to change the behavior of the pipeline:
(这是 GitLab CI 的根配置文件。除了其他不太重要的内容之外，它还指定了要使用的脚本的具体版本。有一些变量可以修改，以改变流水线的行为：)

DRM_CI_PROJECT_PATH
    Repository that contains the Mesa software infrastructure for CI
    (包含用于 CI 的 Mesa 软件基础设施的仓库)

DRM_CI_COMMIT_SHA
    A particular revision to use from that repository
    (要使用的该仓库的特定修订版本)

UPSTREAM_REPO
    URL to git repository containing the target branch
    (包含目标分支的 git 仓库的 URL)

TARGET_BRANCH
    Branch to which this branch is to be merged into
    (此分支将要合并进去的分支)

IGT_VERSION
    Revision of igt-gpu-tools being used, from
    https://gitlab.freedesktop.org/drm/igt-gpu-tools
    (正在使用的 igt-gpu-tools 的修订版本，来自
    https://gitlab.freedesktop.org/drm/igt-gpu-tools)

drivers/gpu/drm/ci/testlist.txt(测试列表)
-----------------------------------------

IGT tests to be run on all drivers (unless mentioned in a driver's \*-skips.txt
file, see below).
(要在所有驱动上运行的 IGT 测试(除非在某个驱动的 \*-skips.txt 文件中提到，见下文)。)

drivers/gpu/drm/ci/${DRIVER_NAME}-${HW_REVISION}-fails.txt(失败列表)
--------------------------------------------------------------------

Lists the known failures for a given driver on a specific hardware revision.
(列出给定驱动在特定硬件修订版本上的已知失败。)

drivers/gpu/drm/ci/${DRIVER_NAME}-${HW_REVISION}-flakes.txt(不稳定测试列表)
---------------------------------------------------------------------------

Lists the tests that for a given driver on a specific hardware revision are
known to behave unreliably. These tests won't cause a job to fail regardless of
the result. They will still be run.
(列出对于给定驱动在特定硬件修订版本上已知表现不可靠的测试。无论结果如何，这些测试都不会导致作业失败。它们仍会被运行。)

Each new flake entry must be associated with a link to the email reporting the
bug to the author of the affected driver or the relevant GitLab issue. The entry
must also include the board name or Device Tree name, the first kernel version
affected, the IGT version used for tests, and an approximation of the failure rate.
(每个新的 flake 条目必须附带一个指向报告该 bug 的邮件的链接(发给受影响驱动的作者)或相关的 GitLab issue。条目还必须包括开发板名称或设备树名称、受影响的第一个内核版本、用于测试的 IGT 版本，以及失败率的近似值。)

They should be provided under the following format::

  # Bug Report: $LORE_URL_OR_GITLAB_ISSUE
  # Board Name: broken-board.dtb
  # Linux Version: 6.6-rc1
  # IGT Version: 1.28-gd2af13d9f
  # Failure Rate: 100
  flaky-test

(应按以下格式提供：)

Use the appropriate link below to create a GitLab issue:
(使用以下相应的链接创建 GitLab issue：)
amdgpu driver: https://gitlab.freedesktop.org/drm/amd/-/issues
i915 driver: https://gitlab.freedesktop.org/drm/i915/kernel/-/issues
msm driver: https://gitlab.freedesktop.org/drm/msm/-/issues
xe driver: https://gitlab.freedesktop.org/drm/xe/kernel/-/issues

drivers/gpu/drm/ci/${DRIVER_NAME}-${HW_REVISION}-skips.txt(跳过测试列表)
------------------------------------------------------------------------

Lists the tests that won't be run for a given driver on a specific hardware
revision. These are usually tests that interfere with the running of the test
list due to hanging the machine, causing OOM, taking too long, etc.
(列出对于给定驱动在特定硬件修订版本上不会运行的测试。这些通常是会干扰测试列表运行的测试，因为它们会挂起机器、导致 OOM、耗时过长等。)


How to enable automated testing on your tree(如何在你的分支树上启用自动化测试)
==============================================================================

1. Create a Linux tree in https://gitlab.freedesktop.org/ if you don't have one
yet
(如果你还没有的话，在 https://gitlab.freedesktop.org/ 上创建一个 Linux 分支树)

2. In your kernel repo's configuration (eg.
https://gitlab.freedesktop.org/janedoe/linux/-/settings/ci_cd), change the
CI/CD configuration file from .gitlab-ci.yml to
drivers/gpu/drm/ci/gitlab-ci.yml.
(在你的内核仓库的配置中(例如 https://gitlab.freedesktop.org/janedoe/linux/-/settings/ci_cd)，将 CI/CD 配置文件从 .gitlab-ci.yml 更改为 drivers/gpu/drm/ci/gitlab-ci.yml。)

3. Request to be added to the drm/ci-ok group so that your user has the
necessary privileges to run the CI on https://gitlab.freedesktop.org/drm/ci-ok
(申请加入 drm/ci-ok 组，以便你的用户拥有在 https://gitlab.freedesktop.org/drm/ci-ok 上运行 CI 的必要权限)

4. Next time you push to this repository, you will see a CI pipeline being
created (eg. https://gitlab.freedesktop.org/janedoe/linux/-/pipelines)
(下次你推送到这个仓库时，你将看到一个 CI 流水线被创建(例如 https://gitlab.freedesktop.org/janedoe/linux/-/pipelines))

5. The various jobs will be run and when the pipeline is finished, all jobs
should be green unless a regression has been found.
(各种作业将会运行；当流水线完成时，除非发现了回归，否则所有作业都应该是绿色的。)

6. Warnings in the pipeline indicate that lockdep
(see Documentation/locking/lockdep-design.rst) issues have been detected
during the tests.
(流水线中的警告表示在测试期间检测到了 lockdep(参见 Documentation/locking/lockdep-design.rst)问题。)


How to update test expectations(如何更新测试预期)
=================================================

If your changes to the code fix any tests, you will have to remove one or more
lines from one or more of the files in
drivers/gpu/drm/ci/${DRIVER_NAME}_*_fails.txt, for each of the test platforms
affected by the change.
(如果你对代码的更改修复了一些测试，你必须从 drivers/gpu/drm/ci/${DRIVER_NAME}_*_fails.txt 中的一个或多个文件中删除一行或多行，且要针对受该更改影响的每个测试平台进行。)


How to expand coverage(如何扩展覆盖率)
======================================

If your code changes make it possible to run more tests (by solving reliability
issues, for example), you can remove tests from the flakes and/or skips lists,
and then the expected results if there are any known failures.
(如果你的代码更改使更多测试能够运行(例如通过解决可靠性问题)，你可以从 flakes 和/或 skips 列表中删除测试，然后删除已知失败的预期结果(如果有)。)

If there is a need for updating the version of IGT being used (maybe you have
added more tests to it), update the IGT_VERSION variable at the top of the
gitlab-ci.yml file.
(如果需要更新正在使用的 IGT 版本(也许你向其中添加了更多测试)，请更新 gitlab-ci.yml 文件顶部的 IGT_VERSION 变量。)


How to test your changes to the scripts(如何测试你对脚本的修改)
===============================================================

For testing changes to the scripts in the drm-ci repo, change the
DRM_CI_PROJECT_PATH and DRM_CI_COMMIT_SHA variables in
drivers/gpu/drm/ci/gitlab-ci.yml to match your fork of the project (eg.
janedoe/drm-ci). This fork needs to be in https://gitlab.freedesktop.org/.
(要测试 drm-ci 仓库中脚本的更改，请将 drivers/gpu/drm/ci/gitlab-ci.yml 中的 DRM_CI_PROJECT_PATH 和 DRM_CI_COMMIT_SHA 变量更改为你该项目 fork 的对应值(例如 janedoe/drm-ci)。这个 fork 需要位于 https://gitlab.freedesktop.org/ 中。)


How to incorporate external fixes in your testing(如何在你的测试中纳入外部修复)
===============================================================================

Often, regressions in other trees will prevent testing changes local to the
tree under test. These fixes will be automatically merged in during the build
jobs from a branch in the target tree that is named as
${TARGET_BRANCH}-external-fixes.
(通常，其他分支树中的回归会阻止对被测分支树本地更改的测试。这些修复将在构建作业期间从目标树中一个名为 ${TARGET_BRANCH}-external-fixes 的分支自动合并进来。)

If the pipeline is not in a merge request and a branch with the same name
exists in the local tree, commits from that branch will be merged in as well.
(如果流水线不在 merge request 中，并且本地树中存在同名分支，那么该分支的提交也将被合并进来。)


How to deal with automated testing labs that may be down(如何应对可能宕机的自动化测试实验室)
============================================================================================

If a hardware farm is down and thus causing pipelines to fail that would
otherwise pass, one can disable all jobs that would be submitted to that farm
by editing the file at
https://gitlab.freedesktop.org/gfx-ci/lab-status/-/blob/main/lab-status.yml.
(如果一个硬件农场 (hardware farm) 宕机并因此导致原本可以通过的流水线失败，可以通过编辑位于 https://gitlab.freedesktop.org/gfx-ci/lab-status/-/blob/main/lab-status.yml 的文件，禁用所有将提交给该农场的作业。)
