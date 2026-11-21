.. _display_todos:

=================================================
AMDGPU - Display Contributions(AMDGPU - 显示贡献)
=================================================

First of all, if you are here, you probably want to give some technical
contribution to the display code, and for that, we say thank you :)
(首先，如果你来到这里，你大概想为显示代码做一些技术贡献。对此，我们说声谢谢 :) )

This page summarizes some of the issues you can help with; keep in mind that
this is a static page, and it is always a good idea to try to reach developers
on the amd-gfx mailing list or some of the maintainers. Finally, this page
follows the DRM way of creating a TODO list; for more information, check
'Documentation/gpu/todo.rst'.
(本页总结了你可以帮忙解决的一些问题；请记住，这是一个静态页面，尝试通过 amd-gfx 邮件列表或某些维护者联系开发者始终是个好主意。最后，本页遵循 DRM 创建 TODO 列表的方式；更多信息请查看 'Documentation/gpu/todo.rst'。)

Gitlab issues(Gitlab 问题)
==========================

Users can report issues associated with AMD GPUs at:
(用户可以在以下位置报告与 AMD GPU 相关的问题：)

- https://gitlab.freedesktop.org/drm/amd

Usually, we try to add a proper label to all new tickets to make it easy to
filter issues. If you can reproduce any problem, you could help by adding more
information or fixing the issue.
(通常，我们会尝试为所有新工单添加合适的标签，以便于筛选问题。如果你能复现任何问题，你可以通过提供更多信息或修复该问题来提供帮助。)

Level: diverse(级别：多样)

IGT(IGT 测试)
=============

`IGT`_ provides many integration tests that can be run on your GPU. We always
want to pass a large set of tests to increase the test coverage in our CI. If
you wish to contribute to the display code but are unsure where a good place
is, we recommend you run all IGT tests and try to fix any failure you see in
your hardware. Keep in mind that this failure can be an IGT problem or a kernel
issue; it is necessary to analyze case-by-case.
( `IGT`_ 提供了许多可以在你的 GPU 上运行的集成测试。我们总是希望通过大量测试来提高 CI 中的测试覆盖率。如果你想为显示代码做贡献但不确定哪里是好的切入点，我们建议你运行所有 IGT 测试，并尝试修复在你的硬件上看到的任何失败。请记住，这种失败可能是 IGT 的问题，也可能是内核问题；需要逐案分析。)

Level: diverse(级别：多样)

.. _IGT: https://gitlab.freedesktop.org/drm/igt-gpu-tools

Compilation(编译)
=================

Fix compilation warnings(修复编译警告)
--------------------------------------

Enable the W1 or W2 warning level in the kernel compilation and try to fix the
issues on the display side.
(在内核编译中启用 W1 或 W2 警告级别，并尝试修复显示侧的问题。)

Level: Starter(级别：入门)

Fix compilation issues when using um architecture(修复使用 um 架构时的编译问题)
-------------------------------------------------------------------------------

Linux has a User-mode Linux (UML) feature, and the kernel can be compiled to
the **um** architecture. Compiling for **um** can bring multiple advantages
from the test perspective. We currently have some compilation issues in this
area that we need to fix.
(Linux 有用户模式 Linux (UML) 特性，内核可以编译为 **um** 架构。从测试的角度来看，为 **um** 编译可以带来多种优势。我们目前在这一领域有一些需要修复的编译问题。)

Level: Intermediate(级别：中级)

Code Refactor(代码重构)
=======================

Add prefix to DC functions to improve the debug with ftrace(为 DC 函数添加前缀以改善 ftrace 调试)
-------------------------------------------------------------------------------------------------

The Ftrace debug feature (check 'Documentation/trace/ftrace.rst') is a
fantastic way to check the code path when developers try to make sense of a
bug. Ftrace provides a filter mechanism that can be useful when the developer
has some hunch of which part of the code can cause the issue; for this reason,
if a set of functions has a proper prefix, it becomes easy to create a good
filter. Additionally, prefixes can improve stack trace readability.
(Ftrace 调试特性(参见 'Documentation/trace/ftrace.rst')是开发者在试图弄清一个 bug 时检查代码路径的绝佳方式。Ftrace 提供了一种过滤机制，当开发者对哪部分代码可能导致问题有所预感时非常有用；因此，如果一组函数有合适的前缀，就很容易创建一个好的过滤器。此外，前缀可以提高堆栈跟踪的可读性。)

The DC code does not follow some prefix rules, which makes the Ftrace filter
more complicated and reduces the readability of the stack trace. If you want
something simple to start contributing to the display, you can make patches for
adding prefixes to DC functions. To create those prefixes, use part of the file
name as a prefix for all functions in the target file. Check the
'amdgpu_dm_crtc.c` and `amdgpu_dm_plane.c` for some references. However, we
strongly advise not to send huge patches changing these prefixes; otherwise, it
will be hard to review and test, which can generate second thoughts from
maintainers. Try small steps; in case of double, you can ask before you put in
effort. We recommend first looking at folders like dceXYZ, dcnXYZ, basics,
bios, core, clk_mgr, hwss, resource, and irq.
(DC 代码没有遵循某些前缀规则，这使得 Ftrace 过滤更加复杂，并降低了堆栈跟踪的可读性。如果你想要一些简单的入门任务来为显示做贡献，你可以制作为 DC 函数添加前缀的补丁。要创建这些前缀，使用文件名的一部分作为目标文件中所有函数的前缀。参考 'amdgpu_dm_crtc.c` 和 `amdgpu_dm_plane.c`。不过，我们强烈建议不要发送修改这些前缀的巨大补丁；否则，将难以审查和测试，这可能让维护者产生顾虑。尝试小步前进；如有疑虑，可以在投入精力之前先询问。我们建议先查看 dceXYZ、dcnXYZ、basics、bios、core、clk_mgr、hwss、resource 和 irq 等目录。)

Level: Starter(级别：入门)

Reduce code duplication(减少代码重复)
-------------------------------------

AMD has an extensive portfolio with various dGPUs and APUs that amdgpu
supports. To maintain the new hardware release cadence, DCE/DCN was designed in
a modular design, making the bring-up for new hardware fast. Over the years,
amdgpu accumulated some technical debt in the code duplication area. For this
task, it would be a good idea to find a tool that can discover code duplication
(including patterns) and use it as guidance to reduce duplications.
(AMD 拥有广泛的产品组合，amdgpu 支持各种 dGPU 和 APU。为了保持新硬件的发布节奏，DCE/DCN 被设计成模块化设计，使新硬件的启动 (bring-up) 快速进行。多年来，amdgpu 在代码重复方面积累了一些技术债。对于这项任务，一个好主意是找到一种可以发现代码重复(包括模式)的工具，并以其为指导来减少重复。)

Level: Intermediate(级别：中级)

Make atomic_commit_[check|tail] more readable(使 atomic_commit_[check|tail] 更易读)
-----------------------------------------------------------------------------------

The functions responsible for atomic commit and tail are intricate and
extensive. In particular `amdgpu_dm_atomic_commit_tail` is a long function and
could benefit from being split into smaller helpers. Improvements in this area
are more than welcome, but keep in mind that changes in this area will affect
all ASICs, meaning that refactoring requires a comprehensive verification; in
other words, this effort can take some time for validation.
(负责 atomic commit 和 tail 的函数错综复杂且冗长。特别是 `amdgpu_dm_atomic_commit_tail` 是一个很长的函数，若拆分成更小的辅助函数会大有好处。这一领域的改进非常受欢迎，但请记住，此领域的更改会影响所有 ASIC，这意味着重构需要全面的验证；换句话说，这项工作可能需要一些时间来进行验证。)

Level: Advanced(级别：高级)

Documentation(文档)
===================

Expand kernel-doc(扩展 kernel-doc)
----------------------------------

Many DC functions do not have a proper kernel-doc; understanding a function and
adding documentation is a great way to learn more about the amdgpu driver and
also leave an outstanding contribution to the entire community.
(许多 DC 函数没有合适的 kernel-doc；理解一个函数并为其添加文档是深入了解 amdgpu 驱动的好方法，也是给整个社区留下杰出贡献的好机会。)

Level: Starter(级别：入门)

Beyond AMDGPU(超越 AMDGPU)
==========================

AMDGPU provides features that are not yet enabled in the userspace. This
section highlights some of the coolest display features, which could be enabled
with the userspace developer helper.
(AMDGPU 提供了一些尚未在用户空间中启用的特性。本节重点介绍一些最酷的显示特性，它们可以在用户空间开发者的帮助下启用。)

Enable underlay(启用 underlay)
------------------------------

AMD display has this feature called underlay (which you can read more about at
'Documentation/gpu/amdgpu/display/mpo-overview.rst') which is intended to
save power when playing a video. The basic idea is to put a video in the
underlay plane at the bottom and the desktop in the plane above it with a hole
in the video area. This feature is enabled in ChromeOS, and from our data
measurement, it can save power.
(AMD 显示有一个叫 underlay (底层平面) 的特性(更多信息见
'Documentation/gpu/amdgpu/display/mpo-overview.rst')，旨在在播放视频时省电。基本想法是把视频放在底部的 underlay 平面，而桌面放在其上方的平面中，视频区域开一个洞。此特性已在 ChromeOS 中启用，根据我们的数据测量，它可以省电。)

Level: Unknown(级别：未知)

Adaptive Backlight Modulation (ABM)(自适应背光调制 (ABM))
---------------------------------------------------------

ABM is a feature that adjusts the display panel's backlight level and pixel
values depending on the displayed image. This power-saving feature can be very
useful when the system starts to run off battery; since this will impact the
display output fidelity, it would be good if this option was something that
users could turn on or off.
(ABM 是一种根据显示的图像调整显示面板背光级别和像素值的特性。当系统开始用电池运行时，这种省电特性可能非常有用；由于这会影响显示输出的保真度，如果这个选项可以由用户打开或关闭就更好了。)

Level: Unknown(级别：未知)


HDR & Color management & VRR(HDR、色彩管理与 VRR)
-------------------------------------------------

HDR, Color Management, and VRR are huge topics and it's hard to put these into
concise ToDos. If you are interested in this topic, we recommend checking some
blog posts from the community developers to better understand some of the
specific challenges and people working on the subject. If anyone wants to work
on some particular part, we can try to help with some basic guidance. Finally,
keep in mind that we already have some kernel-doc in place for those areas.
(HDR、色彩管理 (Color Management) 和 VRR 是庞大的主题，很难将它们放入简洁的 TODO 中。如果你对这个主题感兴趣，我们建议查看社区开发者的一些博客文章，以更好地了解一些具体挑战以及正在研究该主题的人。如果有人想研究某个特定部分，我们可以尝试提供一些基础指导来帮忙。最后，请记住，我们在这些领域已经提供了一些 kernel-doc。)

Level: Unknown(级别：未知)
