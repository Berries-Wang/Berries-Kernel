.. _amdgpu-display-core:

====================================================================
drm/amd/display - Display Core (DC)(drm/amd/display - 显示核心 (DC))
====================================================================

AMD display engine is partially shared with other operating systems; for this
reason, our Display Core Driver is divided into two pieces:
(AMD 显示引擎与其他操作系统部分共享；因此，我们的 Display Core 驱动
分为两部分：)

#. **Display Core (DC)** contains the OS-agnostic components. Things like
   hardware programming and resource management are handled here.
   (Display Core (DC) 包含与操作系统无关的组件。诸如硬件编程和资源
   管理之类的事情都在这里处理。)
#. **Display Manager (DM)** contains the OS-dependent components. Hooks to the
   amdgpu base driver and DRM are implemented here. For example, you can check
   display/amdgpu_dm/ folder.
   (Display Manager (DM) 包含与操作系统相关的组件。对 amdgpu 基础
   驱动和 DRM 的钩子在这里实现。例如，可以查看 display/amdgpu_dm/
   目录。)

-------------------------------
DC Code validation(DC 代码验证)
-------------------------------

Maintaining the same code base across multiple OSes requires a lot of
synchronization effort between repositories and exhaustive validation. In the
DC case, we maintain a tree to centralize code from different parts. The shared
repository has integration tests with our Internal Linux CI farm, and we run a
comprehensive set of IGT tests in various AMD GPUs/APUs (mostly recent dGPUs
and APUs). Our CI also checks ARM64/32, PPC64/32, and x86_64/32 compilation
with DCN enabled and disabled.
(在多个操作系统之间维护同一代码库需要在仓库之间进行大量同步工作，
以及穷尽式的验证。就 DC 而言，我们维护一个树来集中来自不同部分的
代码。共享仓库与我们的内部 Linux CI 测试场有集成测试，我们在各种
AMD GPU/APU(主要是较新的 dGPU 和 APU)上运行一整套 IGT 测试。我们的
CI 还会在启用和禁用 DCN 的情况下检查 ARM64/32、PPC64/32 和
x86_64/32 编译。)

When we upstream a new feature or some patches, we pack them in a patchset with
the prefix **DC Patches for <DATE>**, which is created based on the latest
`amd-staging-drm-next <https://gitlab.freedesktop.org/agd5f/linux>`_. All of
those patches are under a DC version tested as follows:
(当我们向上游提交新特性或一些补丁时，我们会把它们打包成一个以
**DC Patches for <DATE>** 为前缀的补丁集，它基于最新的
amd-staging-drm-next 树创建。所有这些补丁都属于经过如下测试的 DC
版本：)

* Ensure that every patch compiles and the entire series pass our set of IGT
  test in different hardware.
  (确保每个补丁都能编译，且整个系列在不同硬件上通过我们的 IGT 测试
  集。)
* Prepare a branch with those patches for our validation team. If there is an
  error, a developer will debug as fast as possible; usually, a simple bisect
  in the series is enough to point to a bad change, and two possible actions
  emerge: fix the issue or drop the patch. If it is not an easy fix, the bad
  patch is dropped.
  (用这些补丁准备一个分支供我们的验证团队使用。如果有错误，开发者会
  尽快调试；通常，在系列中做一次简单的二分就足以定位一个坏的修改，
  随后有两种可能的行动：修复问题或丢弃该补丁。如果不容易修复，就会
  丢弃那个坏补丁。)
* Finally, developers wait a few days for community feedback before we merge
  the series.
  (最后，开发者会等待几天以获取社区反馈，然后我们才合并该系列。)

It is good to stress that the test phase is something that we take extremely
seriously, and we never merge anything that fails our validation. Follows an
overview of our test set:
(需要强调的是，测试阶段是我们极其认真对待的事情，我们从不合并任何
未通过我们验证的内容。以下是我们测试集的概览：)

#. Manual test
   (手动测试。)
    * Multiple Hotplugs with DP and HDMI.
      (用 DP 和 HDMI 进行多次热插拔。)
    * Stress test with multiple display configuration changes via the user interface.
      (通过用户界面进行多次显示配置更改的压力测试。)
    * Validate VRR behaviour.
      (验证 VRR 行为。)
    * Check PSR.
      (检查 PSR。)
    * Validate MPO when playing video.
      (在播放视频时验证 MPO。)
    * Test more than two displays connected at the same time.
      (测试同时连接两个以上显示器。)
    * Check suspend/resume.
      (检查挂起/恢复。)
    * Validate FPO.
      (验证 FPO。)
    * Check MST.
      (检查 MST。)
#. Automated test
   (自动化测试。)
    * IGT tests in a farm with GPUs and APUs that support DCN and DCE.
      (在带有支持 DCN 和 DCE 的 GPU 与 APU 的测试场中进行 IGT 测试。)
    * Compilation validation with the latest GCC and Clang from LTS distro.
      (使用 LTS 发行版中最新的 GCC 和 Clang 进行编译验证。)
    * Cross-compilation for PowerPC 64/32, ARM 64/32, and x86 32.
      (针对 PowerPC 64/32、ARM 64/32 和 x86 32 的交叉编译。)

In terms of test setup for CI and manual tests, we usually use:
(在 CI 和手动测试的测试环境方面，我们通常使用：)

#. The latest Ubuntu LTS.
   (最新的 Ubuntu LTS。)
#. In terms of userspace, we only use fully updated open-source components
   provided by the distribution official package manager.
   (在用户空间方面，我们只使用由发行版官方包管理器提供的、完全更新
   的开源组件。)
#. Regarding IGT, we use the latest code from the upstream.
   (关于 IGT，我们使用来自上游的最新代码。)
#. Most of the manual tests are conducted in the GNome but we also use KDE.
   (大多数手动测试在 GNome 中进行，但我们也使用 KDE。)

Notice that someone from our test team will always reply to the cover letter
with the test report.
(请注意，我们测试团队总会有人以测试报告回复附信(cover letter)。)

-----------------------
DC Information(DC 信息)
-----------------------

The display pipe is responsible for "scanning out" a rendered frame from the
GPU memory (also called VRAM, FrameBuffer, etc.) to a display. In other words,
it would:
(显示管线负责把渲染好的帧从 GPU 内存(也称为 VRAM、FrameBuffer 等)
"扫描输出"(scan out)到显示器。换言之，它会：)

#. Read frame information from memory;
   (从内存读取帧信息；)
#. Perform required transformation;
   (执行所需的变换；)
#. Send pixel data to sink devices.
   (把像素数据发送到接收端(sink)设备。)

If you want to learn more about our driver details, take a look at the below
table of content:
(如果你想进一步了解我们的驱动细节，请查看下面的目录：)

.. toctree::

   display-manager.rst
   dcn-overview.rst
   dcn-blocks.rst
   programming-model-dcn.rst
   mpo-overview.rst
   dc-debug.rst
   display-contributing.rst
   dc-glossary.rst
