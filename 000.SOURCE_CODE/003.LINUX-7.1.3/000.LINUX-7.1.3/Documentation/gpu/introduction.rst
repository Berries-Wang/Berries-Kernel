==================
Introduction(简介)
==================

The Linux DRM layer contains code intended to support the needs of
complex graphics devices, usually containing programmable pipelines well
suited to 3D graphics acceleration. Graphics drivers in the kernel may
make use of DRM functions to make tasks like memory management,
interrupt handling and DMA easier, and provide a uniform interface to
applications.
(Linux DRM 层包含旨在满足复杂图形设备需求的代码，这类设备通常包含非常适合 3D 图形加速的可编程流水线。内核中的图形驱动可以利用 DRM 函数来简化内存管理、中断处理和 DMA 等任务，并为应用程序提供统一的接口。)

A note on versions: this guide covers features found in the DRM tree,
including the TTM memory manager, output configuration and mode setting,
and the new vblank internals, in addition to all the regular features
found in current kernels.
(关于版本的说明：本指南涵盖 DRM 树中的特性，包括 TTM 内存管理器、输出配置与模式设置 (mode setting)、新的 vblank 内部实现，以及当前内核中的所有常规特性。)

[Insert diagram of typical DRM stack here]
(此处插入典型 DRM 栈的示意图)

Style Guidelines(风格指南)
==========================

For consistency this documentation uses American English. Abbreviations
are written as all-uppercase, for example: DRM, KMS, IOCTL, CRTC, and so
on. To aid in reading, documentations make full use of the markup
characters kerneldoc provides: @parameter for function parameters,
@member for structure members (within the same structure), &struct structure to
reference structures and function() for functions. These all get automatically
hyperlinked if kerneldoc for the referenced objects exists. When referencing
entries in function vtables (and structure members in general) please use
&vtable_name.vfunc. Unfortunately this does not yet yield a direct link to the
member, only the structure.
(为保持一致性，本文档使用美式英语。缩写全部大写，例如：DRM、KMS、IOCTL、CRTC 等。为便于阅读，文档充分利用 kerneldoc 提供的标记字符：@parameter 表示函数参数，@member 表示结构体成员(同一结构体内)，&struct structure 用于引用结构体，function() 用于函数。如果被引用对象的 kerneldoc 存在，这些都会自动生成超链接。引用函数虚表 (vtable) 中的条目(以及一般的结构体成员)时，请使用 &vtable_name.vfunc。遗憾的是，这尚不能直接链接到该成员，只能链接到结构体。)

Except in special situations (to separate locked from unlocked variants)
locking requirements for functions aren't documented in the kerneldoc.
Instead locking should be check at runtime using e.g.
``WARN_ON(!mutex_is_locked(...));``. Since it's much easier to ignore
documentation than runtime noise this provides more value. And on top of
that runtime checks do need to be updated when the locking rules change,
increasing the chances that they're correct. Within the documentation
the locking rules should be explained in the relevant structures: Either
in the comment for the lock explaining what it protects, or data fields
need a note about which lock protects them, or both.
(除特殊情况(用于区分加锁与不加锁的变体)外，函数的加锁要求不在 kerneldoc 中说明。相反，应在运行时通过例如 ``WARN_ON(!mutex_is_locked(...));`` 这样的代码来检查加锁。由于忽略文档比忽略运行时噪音容易得多，这样能提供更多价值。此外，当加锁规则改变时，运行时检查也必须随之更新，这提高了它们保持正确的机会。在文档中，加锁规则应在相关结构体中说明：要么在锁的注释中说明它保护什么，要么在数据字段上注明由哪个锁保护，或两者兼有。)

Functions which have a non-\ ``void`` return value should have a section
called "Returns" explaining the expected return values in different
cases and their meanings. Currently there's no consensus whether that
section name should be all upper-case or not, and whether it should end
in a colon or not. Go with the file-local style. Other common section
names are "Notes" with information for dangerous or tricky corner cases,
and "FIXME" where the interface could be cleaned up.
(返回值非 \ ``void`` 的函数应有一个名为 "Returns" 的小节，解释不同情况下的预期返回值及其含义。目前对于该节名是否应全部大写、是否应以冒号结尾尚无共识。请遵循该文件本地的风格。其他常见的节名还有 "Notes"(包含针对危险或棘手边界情况的信息)和 "FIXME"(表示该接口可以清理)。)

Also read the :ref:`guidelines for the kernel documentation at large <doc_guide>`.
(另请参阅 :ref:`guidelines for the kernel documentation at large <doc_guide>` 。)

Documentation Requirements for kAPI(kAPI 文档要求)
--------------------------------------------------

All kernel APIs exported to other modules must be documented, including their
datastructures and at least a short introductory section explaining the overall
concepts. Documentation should be put into the code itself as kerneldoc comments
as much as reasonable.
(所有导出到其他模块的内核 API 都必须有文档，包括其数据结构，以及至少一段解释整体概念的简短介绍。文档应尽可能以 kerneldoc 注释的形式直接写入代码。)

Do not blindly document everything, but document only what's relevant for driver
authors: Internal functions of drm.ko and definitely static functions should not
have formal kerneldoc comments. Use normal C comments if you feel like a comment
is warranted. You may use kerneldoc syntax in the comment, but it shall not
start with a /** kerneldoc marker. Similar for data structures, annotate
anything entirely private with ``/* private: */`` comments as per the
documentation guide.
(不要盲目地为一切编写文档，而只记录与驱动作者相关的内容： drm.ko 的内部函数以及肯定是 static 的函数不应有正式的 kerneldoc 注释。如果你觉得有必要写注释，请使用普通的 C 注释。你可以在注释中使用 kerneldoc 语法，但不得以 /** kerneldoc 标记开头。数据结构也是如此：按照文档指南，用 ``/* private: */`` 注释标注完全私有的内容。)

Getting Started(入门)
=====================

Developers interested in helping out with the DRM subsystem are very welcome.
Often people will resort to sending in patches for various issues reported by
checkpatch or sparse. We welcome such contributions.
(欢迎有兴趣为 DRM 子系统提供帮助的开发者。人们通常会针对 checkpatch 或 sparse 报告的各种问题提交补丁。我们欢迎这类贡献。)

Anyone looking to kick it up a notch can find a list of janitorial tasks on
the :ref:`TODO list <todo>`.
(任何想更进一步的人都可以在 :ref:`TODO list <todo>` 上找到一份杂务 (janitorial) 任务清单。)

Contribution Process(贡献流程)
==============================

Mostly the DRM subsystem works like any other kernel subsystem, see :ref:`the
main process guidelines and documentation <process_index>` for how things work.
Here we just document some of the specialities of the GPU subsystem.
(DRM 子系统的工作方式与其他内核子系统基本相同，参见 :ref:`the main process guidelines and documentation <process_index>` 以了解其运作方式。这里只记录 GPU 子系统的一些特殊之处。)

Feature Merge Deadlines(特性合入截止时间)
-----------------------------------------

All feature work must be in the linux-next tree by the -rc6 release of the
current release cycle, otherwise they must be postponed and can't reach the next
merge window. All patches must have landed in the drm-next tree by latest -rc7,
but if your branch is not in linux-next then this must have happened by -rc6
already.
(所有特性工作必须在当前发布周期的 -rc6 发布之前进入 linux-next 树，否则必须推迟，无法赶上下一个合并窗口。所有补丁最迟必须在 -rc7 之前落入 drm-next 树；但如果你的分支尚未进入 linux-next，那么这必须在 -rc6 时就已发生。)

After that point only bugfixes (like after the upstream merge window has closed
with the -rc1 release) are allowed. No new platform enabling or new drivers are
allowed.
(在那之后只允许错误修复(就像上游合并窗口以 -rc1 发布关闭之后一样)。不允许启用新平台或添加新驱动。)

This means that there's a blackout-period of about one month where feature work
can't be merged. The recommended way to deal with that is having a -next tree
that's always open, but making sure to not feed it into linux-next during the
blackout period. As an example, drm-misc works like that.
(这意味着存在大约一个月的特性工作无法合入的空窗期 (blackout-period)。推荐的应对方式是维护一个始终开放的 -next 树，但要确保在空窗期期间不将其喂给 linux-next。例如，drm-misc 就是这样运作的。)

Code of Conduct(行为准则)
-------------------------

As a freedesktop.org project, dri-devel, and the DRM community, follows the
Contributor Covenant, found at: https://www.freedesktop.org/wiki/CodeOfConduct
(作为 freedesktop.org 项目，dri-devel 与 DRM 社区遵循 Contributor Covenant (贡献者公约)，见：https://www.freedesktop.org/wiki/CodeOfConduct)

Please conduct yourself in a respectful and civilised manner when
interacting with community members on mailing lists, IRC, or bug
trackers. The community represents the project as a whole, and abusive
or bullying behaviour is not tolerated by the project.
(在邮件列表、IRC 或错误跟踪系统上与社区成员互动时，请保持尊重和文明。社区代表整个项目，辱骂或欺凌行为不会被项目容忍。)

Simple DRM drivers to use as examples(可作示例的简单 DRM 驱动)
==============================================================

The DRM subsystem contains a lot of helper functions to ease writing drivers for
simple graphic devices. For example, the `drivers/gpu/drm/tiny/` directory has a
set of drivers that are simple enough to be implemented in a single source file.
The tiny DRM drivers are good examples to understand how DRM drivers should look
like. Since are just a few hundreds lines of code, they are quite easy to read.
(DRM 子系统包含许多辅助函数，以简化为简单图形设备编写驱动的工作。例如， `drivers/gpu/drm/tiny/` 目录中有一组驱动，它们简单到可以在单个源文件中实现。这些 tiny DRM 驱动是理解 DRM 驱动应有的样子的好例子。由于只有几百行代码，它们非常易读。)

External References(外部参考资料)
=================================

Delving into a Linux kernel subsystem for the first time can be an overwhelming
experience, one needs to get familiar with all the concepts and learn about the
subsystem's internals, among other details.
(初次深入研究一个 Linux 内核子系统可能是一种令人应接不暇的体验：人们需要熟悉所有概念，并了解子系统的内部结构等诸多细节。)

To shallow the learning curve, this section contains a list of presentations
and documents that can be used to learn about DRM/KMS and graphics in general.
(为了降低学习曲线，本节包含一份可用于学习 DRM/KMS 及一般图形知识的演示文稿和文档列表。)

There are different reasons why someone might want to get into DRM: porting an
existing fbdev driver, write a DRM driver for a new hardware, fixing bugs that
could face when working on the graphics user-space stack, etc. For this reason,
the learning material covers many aspects of the Linux graphics stack. From an
overview of the kernel and user-space stacks to very specific topics.
(人们想深入 DRM 的原因各不相同：移植现有的 fbdev 驱动、为新硬件编写 DRM 驱动、修复在图形用户空间栈上工作时可能遇到的 bug，等等。因此，学习材料涵盖了 Linux 图形栈的许多方面，从内核与用户空间栈的概览到非常具体的主题。)

The list is sorted in reverse chronological order, to keep the most up-to-date
material at the top. But all of them contain useful information, and it can be
valuable to go through older material to understand the rationale and context
in which the changes to the DRM subsystem were made.
(列表按时间倒序排列，以便把最新的材料排在最前面。但它们都包含有用的信息，浏览较旧的材料以理解对 DRM 子系统所做更改的依据和背景也很有价值。)

Conference talks(会议演讲)
--------------------------

* `An Overview of the Linux and Userspace Graphics Stack <https://www.youtube.com/watch?v=wjAJmqwg47k>`_ - Paul Kocialkowski (2020)
* `Getting pixels on screen on Linux: introduction to Kernel Mode Setting <https://www.youtube.com/watch?v=haes4_Xnc5Q>`_ - Simon Ser (2020)
* `Everything Great about Upstream Graphics <https://www.youtube.com/watch?v=kVzHOgt6WGE>`_ - Simona Vetter (2019)
* `An introduction to the Linux DRM subsystem <https://www.youtube.com/watch?v=LbDOCJcDRoo>`_ - Maxime Ripard (2017)
* `Embrace the Atomic (Display) Age <https://www.youtube.com/watch?v=LjiB_JeDn2M>`_ - Simona Vetter (2016)
* `Anatomy of an Atomic KMS Driver <https://www.youtube.com/watch?v=lihqR9sENpc>`_ - Laurent Pinchart (2015)
* `Atomic Modesetting for Drivers <https://www.youtube.com/watch?v=kl9suFgbTc8>`_ - Simona Vetter (2015)
* `Anatomy of an Embedded KMS Driver <https://www.youtube.com/watch?v=Ja8fM7rTae4>`_ - Laurent Pinchart (2013)

Slides and articles(幻灯片与文章)
---------------------------------

* `The Linux graphics stack in a nutshell, part 1 <https://lwn.net/Articles/955376/>`_ - Thomas Zimmermann (2023)
* `The Linux graphics stack in a nutshell, part 2 <https://lwn.net/Articles/955708/>`_ - Thomas Zimmermann (2023)
* `Understanding the Linux Graphics Stack <https://bootlin.com/doc/training/graphics/graphics-slides.pdf>`_ - Bootlin (2022)
* `DRM KMS overview <https://wiki.st.com/stm32mpu/wiki/DRM_KMS_overview>`_ - STMicroelectronics (2021)
* `Linux graphic stack <https://studiopixl.com/2017-05-13/linux-graphic-stack-an-overview>`_ - Nathan Gauër (2017)
* `Atomic mode setting design overview, part 1 <https://lwn.net/Articles/653071/>`_ - Simona Vetter (2015)
* `Atomic mode setting design overview, part 2 <https://lwn.net/Articles/653466/>`_ - Simona Vetter (2015)
* `The DRM/KMS subsystem from a newbie’s point of view <https://bootlin.com/pub/conferences/2014/elce/brezillon-drm-kms/brezillon-drm-kms.pdf>`_ - Boris Brezillon (2014)
* `A brief introduction to the Linux graphics stack <https://blogs.igalia.com/itoral/2014/07/29/a-brief-introduction-to-the-linux-graphics-stack/>`_ - Iago Toral (2014)
* `The Linux Graphics Stack <https://blog.mecheye.net/2012/06/the-linux-graphics-stack/>`_ - Jasper St. Pierre (2012)
