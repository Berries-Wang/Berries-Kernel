.. _submitchecklist:

===============================================================
Linux Kernel patch submission checklist(Linux 内核补丁提交清单)
===============================================================

Here are some basic things that developers should do if they want to see their
kernel patch submissions accepted more quickly.
(以下是一些基本事项，开发者如果希望自己的内核补丁提交能更快被接受，就应当做到。)

These are all above and beyond the documentation that is provided in
:ref:`Documentation/process/submitting-patches.rst <submittingpatches>`
and elsewhere regarding submitting Linux kernel patches.
(这些都是在 :ref:`Documentation/process/submitting-patches.rst <submittingpatches>` 以及其他地方提供的、有关提交 Linux 内核补丁的文档之外的额外要求。)

Review your code(检查你的代码)
==============================

1) If you use a facility then #include the file that defines/declares
   that facility.  Don't depend on other header files pulling in ones
   that you use.
   (如果你使用了某个设施，就应 #include 定义/声明该设施的头文件。不要依赖其他头文件拉入你所使用的头文件。)

2) Check your patch for general style as detailed in
   :ref:`Documentation/process/coding-style.rst <codingstyle>`.
   (按照 :ref:`Documentation/process/coding-style.rst <codingstyle>` 中的详细说明检查补丁的一般风格。)

3) All memory barriers {e.g., ``barrier()``, ``rmb()``, ``wmb()``} need a
   comment in the source code that explains the logic of what they are doing
   and why.
   (所有内存屏障 {例如 ``barrier()`` 、 ``rmb()`` 、 ``wmb()`` } 都需要在源代码中加注释，说明其操作逻辑以及这样做的原因。)

Review Kconfig changes(检查 Kconfig 更改)
=========================================

1) Any new or modified ``CONFIG`` options do not muck up the config menu and
   default to off unless they meet the exception criteria documented in
   ``Documentation/kbuild/kconfig-language.rst`` Menu attributes: default value.
   (任何新增或修改的 ``CONFIG`` 选项都不得把配置菜单搞乱，并且除非满足 ``Documentation/kbuild/kconfig-language.rst`` 中 "Menu attributes: default value"（菜单属性：默认值）所记载的例外标准，否则应默认关闭。)

2) All new ``Kconfig`` options have help text.
   (所有新的 ``Kconfig`` 选项都有帮助文本。)

3) Has been carefully reviewed with respect to relevant ``Kconfig``
   combinations.  This is very hard to get right with testing---brainpower
   pays off here.
   (已针对相关的 ``Kconfig`` 组合进行仔细审查。这一点很难靠测试做对——在这里动脑比什么都划算。)

Provide documentation(提供文档)
===============================

1) Include :ref:`kernel-doc <kernel_doc>` to document global kernel APIs.
   (Not required for static functions, but OK there also.)
   (加入 :ref:`kernel-doc <kernel_doc>` 来为全局内核 API 编写文档。（静态函数不要求，但加上也可以。）)

2) All new ``/proc`` entries are documented under ``Documentation/``
   (所有新的 ``/proc`` 条目都在 ``Documentation/`` 下有文档。)

3) All new kernel boot parameters are documented in
   ``Documentation/admin-guide/kernel-parameters.rst``.
   (所有新的内核启动参数都在 ``Documentation/admin-guide/kernel-parameters.rst`` 中有文档。)

4) All new module parameters are documented with ``MODULE_PARM_DESC()``
   (所有新的模块参数都用 ``MODULE_PARM_DESC()`` 加了文档。)

5) All new userspace interfaces are documented in ``Documentation/ABI/``.
   See Documentation/admin-guide/abi.rst (or ``Documentation/ABI/README``)
   for more information.
   Patches that change userspace interfaces should be CCed to
   linux-api@vger.kernel.org.
   (所有新的用户空间接口都在 ``Documentation/ABI/`` 中有文档。更多信息参见 Documentation/admin-guide/abi.rst（或 ``Documentation/ABI/README`` ）。更改用户空间接口的补丁应抄送给 linux-api@vger.kernel.org。)

6) If any ioctl's are added by the patch, then also update
   ``Documentation/userspace-api/ioctl/ioctl-number.rst``.
   (如果补丁新增了任何 ioctl，还要更新 ``Documentation/userspace-api/ioctl/ioctl-number.rst`` 。)

Check your code with tools(使用工具检查你的代码)
================================================

1) Check for trivial violations with the patch style checker prior to
   submission (``scripts/checkpatch.pl``).
   You should be able to justify all violations that remain in
   your patch.
   (提交前先用补丁风格检查器（ ``scripts/checkpatch.pl`` ）检查琐碎违规。你应当能为补丁中残留的所有违规给出理由。)

2) Check cleanly with sparse.
   (用 sparse 干净地通过检查。)

3) Use ``make checkstack`` and fix any problems that it finds.
   Note that ``checkstack`` does not point out problems explicitly,
   but any one function that uses more than 512 bytes on the stack is a
   candidate for change.
   (使用 ``make checkstack`` 并修复它发现的任何问题。注意 ``checkstack`` 不会明确指出问题所在，但任何栈用量超过 512 字节的函数都是值得修改的对象。)

Build your code(构建你的代码)
=============================

1) Builds cleanly:
   (干净地构建：)

  a) with applicable or modified ``CONFIG`` options ``=y``, ``=m``, and
     ``=n``.  No ``gcc`` warnings/errors, no linker warnings/errors.
     (在适用或被修改的 ``CONFIG`` 选项取 ``=y`` 、 ``=m`` 和 ``=n`` 的情况下。没有 ``gcc`` 警告/错误，没有链接器警告/错误。)

  b) Passes ``allnoconfig``, ``allmodconfig``
     (通过 ``allnoconfig`` 与 ``allmodconfig`` 的测试。)

  c) Builds successfully when using ``O=builddir``
     (使用 ``O=builddir`` 时构建成功。)

  d) Any Documentation/ changes build successfully without new warnings/errors.
     Use ``make htmldocs`` or ``make pdfdocs`` to check the build and
     fix any issues.
     (Documentation/ 的任何更改都能成功构建且没有新的警告/错误。使用 ``make htmldocs`` 或 ``make pdfdocs`` 检查构建并修复任何问题。)

2) Builds on multiple CPU architectures by using local cross-compile tools
   or some other build farm.
   Note that testing against architectures of different word sizes
   (32- and 64-bit) and different endianness (big- and little-) is effective
   in catching various portability issues due to false assumptions on
   representable quantity range, data alignment, or endianness, among
   others.
   (使用本地交叉编译工具或其他构建农场，在多种 CPU 架构上构建。注意，针对不同字长（32 位与 64 位）和不同字节序（大端与小端）的架构进行测试，能有效发现由于对可表示数量范围、数据对齐或字节序等的错误假设而导致的各类可移植性问题。)

3) Newly-added code has been compiled with ``gcc -W`` (use
   ``make KCFLAGS=-W``).  This will generate lots of noise, but is good
   for finding bugs like "warning: comparison between signed and unsigned".
   (新增代码已用 ``gcc -W`` 编译（使用 ``make KCFLAGS=-W`` ）。这会产生大量噪音，但有助于发现诸如 "warning: comparison between signed and unsigned" 之类的缺陷。)

4) If your modified source code depends on or uses any of the kernel
   APIs or features that are related to the following ``Kconfig`` symbols,
   then test multiple builds with the related ``Kconfig`` symbols disabled
   and/or ``=m`` (if that option is available) [not all of these at the
   same time, just various/random combinations of them]:
   (如果你修改的源代码依赖或使用与以下 ``Kconfig`` 符号相关的任何内核 API 或特性，则需在禁用相关 ``Kconfig`` 符号和/或设为 ``=m`` （如果该选项可用）的情况下测试多次构建 [并非全部同时禁用，而是它们的各种/随机组合]：)

   ``CONFIG_SMP``, ``CONFIG_SYSFS``, ``CONFIG_PROC_FS``, ``CONFIG_INPUT``,
   ``CONFIG_PCI``, ``CONFIG_BLOCK``, ``CONFIG_PM``, ``CONFIG_MAGIC_SYSRQ``,
   ``CONFIG_NET``, ``CONFIG_INET=n`` (but latter with ``CONFIG_NET=y``).

Test your code(测试你的代码)
============================

1) Has been tested with ``CONFIG_PREEMPT``, ``CONFIG_DEBUG_PREEMPT``,
   ``CONFIG_SLUB_DEBUG``, ``CONFIG_DEBUG_PAGEALLOC``, ``CONFIG_DEBUG_MUTEXES``,
   ``CONFIG_DEBUG_SPINLOCK``, ``CONFIG_DEBUG_ATOMIC_SLEEP``,
   ``CONFIG_PROVE_RCU`` and ``CONFIG_DEBUG_OBJECTS_RCU_HEAD`` all
   simultaneously enabled.
   (已在 ``CONFIG_PREEMPT`` 、 ``CONFIG_DEBUG_PREEMPT`` 、 ``CONFIG_SLUB_DEBUG`` 、 ``CONFIG_DEBUG_PAGEALLOC`` 、 ``CONFIG_DEBUG_MUTEXES`` 、 ``CONFIG_DEBUG_SPINLOCK`` 、 ``CONFIG_DEBUG_ATOMIC_SLEEP`` 、 ``CONFIG_PROVE_RCU`` 和 ``CONFIG_DEBUG_OBJECTS_RCU_HEAD`` 全部同时启用的情况下测试过。)

2) Has been build- and runtime tested with and without ``CONFIG_SMP`` and
   ``CONFIG_PREEMPT.``
   (已在启用与禁用 ``CONFIG_SMP`` 和 ``CONFIG_PREEMPT`` 的情况下分别进行构建与运行时测试。)

3) All codepaths have been exercised with all lockdep features enabled.
   (所有代码路径都已在启用全部 lockdep 特性的情况下演练过。)

4) Has been checked with injection of at least slab and page-allocation
   failures.  See ``Documentation/fault-injection/``.
   If the new code is substantial, addition of subsystem-specific fault
   injection might be appropriate.
   (已通过至少注入 slab 和页面分配失败进行了检查。参见 ``Documentation/fault-injection/`` 。如果新代码规模较大，添加子系统特定的故障注入可能更为合适。)

5) Tested with the most recent tag of linux-next to make sure that it still
   works with all of the other queued patches and various changes in the VM,
   VFS, and other subsystems.
   (已用 linux-next 的最新标签进行测试，确保它与其余所有已排队的补丁以及 VM、VFS 和其他子系统的各种更改仍能协同工作。)
