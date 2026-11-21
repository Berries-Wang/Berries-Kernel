Assembler Annotations(汇编器注记)
=================================

Copyright (c) 2017-2019 Jiri Slaby

This document describes the new macros for annotation of data and code in
assembly. In particular, it contains information about ``SYM_FUNC_START``,
``SYM_FUNC_END``, ``SYM_CODE_START``, and similar.
(本文档描述用于在汇编中注记数据与代码的新宏。特别是，它包含关于 ``SYM_FUNC_START`` 、 ``SYM_FUNC_END`` 、 ``SYM_CODE_START`` 及类似宏的信息。)

Rationale(缘由)
----------------
Some code like entries, trampolines, or boot code needs to be written in
assembly. The same as in C, such code is grouped into functions and
accompanied with data. Standard assemblers do not force users into precisely
marking these pieces as code, data, or even specifying their length.
Nevertheless, assemblers provide developers with such annotations to aid
debuggers throughout assembly. On top of that, developers also want to mark
some functions as *global* in order to be visible outside of their translation
units.
(入口、蹦床(trampoline)或引导代码之类的代码需要用汇编编写。与 C 语言一样，这类代码被组织成函数，并伴有相应的数据。标准汇编器并不强制用户精确地把这些片段标记为代码、数据，甚至不要求指明其长度。尽管如此，汇编器还是为开发者提供了这类注记，以帮助调试器在汇编代码中工作。除此之外，开发者还希望把某些函数标记为 *global* ，以便在其翻译单元之外可见。)

Over time, the Linux kernel has adopted macros from various projects (like
``binutils``) to facilitate such annotations. So for historic reasons,
developers have been using ``ENTRY``, ``END``, ``ENDPROC``, and other
annotations in assembly.  Due to the lack of their documentation, the macros
are used in rather wrong contexts at some locations. Clearly, ``ENTRY`` was
intended to denote the beginning of global symbols (be it data or code).
``END`` used to mark the end of data or end of special functions with
*non-standard* calling convention. In contrast, ``ENDPROC`` should annotate
only ends of *standard* functions.
(随着时间的推移，Linux 内核从各种项目(如 ``binutils``)中借鉴了一些宏来辅助这类注记。因此出于历史原因，开发者一直在汇编中使用 ``ENTRY`` 、 ``END`` 、 ``ENDPROC`` 等注记。由于缺乏文档，这些宏在一些地方被用在了相当错误的上下文中。显然， ``ENTRY`` 的本意是表示全局符号(无论是数据还是代码)的开始。 ``END`` 过去用于标记数据的结束，或采用 *非标准* 调用约定的特殊函数的结束。相比之下， ``ENDPROC`` 只应注记 *标准* 函数的结束。)

When these macros are used correctly, they help assemblers generate a nice
object with both sizes and types set correctly. For example, the result of
``arch/x86/lib/putuser.S``::

   Num:    Value          Size Type    Bind   Vis      Ndx Name
    25: 0000000000000000    33 FUNC    GLOBAL DEFAULT    1 __put_user_1
    29: 0000000000000030    37 FUNC    GLOBAL DEFAULT    1 __put_user_2
    32: 0000000000000060    36 FUNC    GLOBAL DEFAULT    1 __put_user_4
    35: 0000000000000090    37 FUNC    GLOBAL DEFAULT    1 __put_user_8

(当正确使用这些宏时，它们能帮助汇编器生成一个大小与类型都设置正确的目标文件。例如 ``arch/x86/lib/putuser.S`` 的结果：)

This is not only important for debugging purposes. When there are properly
annotated objects like this, tools can be run on them to generate more useful
information. In particular, on properly annotated objects, ``objtool`` can be
run to check and fix the object if needed. Currently, ``objtool`` can report
missing frame pointer setup/destruction in functions. It can also
automatically generate annotations for the ORC unwinder
(Documentation/arch/x86/orc-unwinder.rst)
for most code. Both of these are especially important to support reliable
stack traces which are in turn necessary for kernel live patching
(Documentation/livepatch/livepatch.rst).
(这不仅对调试很重要。当存在这样正确注记的目标文件时，可以对它们运行各种工具以生成更有用的信息。特别是，在正确注记的目标文件上可以运行 ``objtool`` 来检查并在必要时修复目标文件。目前， ``objtool`` 能够报告函数中缺失的帧指针建立/销毁，还能为大多数代码自动生成 ORC 回退器(Documentation/arch/x86/orc-unwinder.rst)所需的注记。这两者对于支持可靠栈回溯尤为重要，而可靠栈回溯又是内核动态补丁(Documentation/livepatch/livepatch.rst)所必需的。)

Caveat and Discussion(注意与讨论)
---------------------------------
As one might realize, there were only three macros previously. That is indeed
insufficient to cover all the combinations of cases:
(可以想到，之前只有三个宏。这确实不足以覆盖所有情形的组合：)

* standard/non-standard function
  (标准/非标准函数)
* code/data
  (代码/数据)
* global/local symbol
  (全局/局部符号)

There was a discussion_ and instead of extending the current ``ENTRY/END*``
macros, it was decided that brand new macros should be introduced instead::

    So how about using macro names that actually show the purpose, instead
    of importing all the crappy, historic, essentially randomly chosen
    debug symbol macro names from the binutils and older kernels?

.. _discussion: https://lore.kernel.org/r/20170217104757.28588-1-jslaby@suse.cz

(曾经有一场讨论，最终决定不扩展现有的 ``ENTRY/END*`` 宏，而是引入全新的宏：)

(引文大意：与其从 binutils 和旧内核导入那些蹩脚的、历史遗留的、本质上随机选取的调试符号宏名，不如改用能真正体现用途的宏名，怎么样？)

Macros Description(宏描述)
----------------------------

The new macros are prefixed with the ``SYM_`` prefix and can be divided into
three main groups:
(新宏以 ``SYM_`` 前缀命名，可分为三大类：)

1. ``SYM_FUNC_*`` -- to annotate C-like functions. This means functions with
   standard C calling conventions. For example, on x86, this means that the
   stack contains a return address at the predefined place and a return from
   the function can happen in a standard way. When frame pointers are enabled,
   save/restore of frame pointer shall happen at the start/end of a function,
   respectively, too.
   ( ``SYM_FUNC_*`` ——用于注记类似 C 的函数，即具有标准 C 调用约定的函数。例如在 x86 上，这意味着栈中在预定义位置保存着返回地址，并且函数可以以标准方式返回。启用帧指针时，帧指针的保存/恢复也应在函数的开头/结尾进行。)

   Checking tools like ``objtool`` should ensure such marked functions conform
   to these rules. The tools can also easily annotate these functions with
   debugging information (like *ORC data*) automatically.
   ( ``objtool`` 等检查工具应确保以此标记的函数符合这些规则。这些工具还能轻松地自动为这类函数注记调试信息(如 *ORC 数据*)。)

2. ``SYM_CODE_*`` -- special functions called with special stack. Be it
   interrupt handlers with special stack content, trampolines, or startup
   functions.
   ( ``SYM_CODE_*`` ——以特殊栈调用的特殊函数。可以是栈内容特殊的中断处理程序、蹦床(trampoline)或启动函数。)

   Checking tools mostly ignore checking of these functions. But some debug
   information still can be generated automatically. For correct debug data,
   this code needs hints like ``UNWIND_HINT_REGS`` provided by developers.
   (检查工具大多会忽略对这类函数的检查。但仍可以自动生成一些调试信息。为获得正确的调试数据，这类代码需要开发者提供诸如 ``UNWIND_HINT_REGS`` 的提示。)

3. ``SYM_DATA*`` -- obviously data belonging to ``.data`` sections and not to
   ``.text``. Data do not contain instructions, so they have to be treated
   specially by the tools: they should not treat the bytes as instructions,
   nor assign any debug information to them.
   ( ``SYM_DATA*`` ——显然是属于 ``.data`` 段而非 ``.text`` 段的数据。数据不包含指令，因此工具必须特殊对待它们：不应把这些字节当作指令，也不应为它们分配任何调试信息。)

Instruction Macros(指令宏)
~~~~~~~~~~~~~~~~~~~~~~~~~~
This section covers ``SYM_FUNC_*`` and ``SYM_CODE_*`` enumerated above.
(本节涵盖上文列举的 ``SYM_FUNC_*`` 与 ``SYM_CODE_*`` 。)

``objtool`` requires that all code must be contained in an ELF symbol. Symbol
names that have a ``.L`` prefix do not emit symbol table entries. ``.L``
prefixed symbols can be used within a code region, but should be avoided for
denoting a range of code via ``SYM_*_START/END`` annotations.
( ``objtool`` 要求所有代码都必须包含在一个 ELF 符号内。以 ``.L`` 为前缀的符号名不会生成符号表条目。 ``.L`` 前缀的符号可以在代码区域内部使用，但应避免用它们通过 ``SYM_*_START/END`` 注记来标示一段代码的范围。)

* ``SYM_FUNC_START`` and ``SYM_FUNC_START_LOCAL`` are supposed to be **the
  most frequent markings**. They are used for functions with standard calling
  conventions -- global and local. Like in C, they both align the functions to
  architecture specific ``__ALIGN`` bytes. There are also ``_NOALIGN`` variants
  for special cases where developers do not want this implicit alignment.

  ( ``SYM_FUNC_START`` 与 ``SYM_FUNC_START_LOCAL`` 应当是 **使用最频繁的标记** 。它们用于具有标准调用约定的函数——全局和局部。与 C 中一样，它们都会把函数对齐到架构特定的 ``__ALIGN`` 字节。还有一些 ``_NOALIGN`` 变体，用于开发者不想要这种隐式对齐的特殊情况。)

  ``SYM_FUNC_START_WEAK`` and ``SYM_FUNC_START_WEAK_NOALIGN`` markings are
  also offered as an assembler counterpart to the *weak* attribute known from
  C.

  ( ``SYM_FUNC_START_WEAK`` 与 ``SYM_FUNC_START_WEAK_NOALIGN`` 标记作为 C 语言中 *weak* 属性在汇编中的对应物提供。)

  All of these **shall** be coupled with ``SYM_FUNC_END``. First, it marks
  the sequence of instructions as a function and computes its size to the
  generated object file. Second, it also eases checking and processing such
  object files as the tools can trivially find exact function boundaries.

  (所有这些标记 **必须** 与 ``SYM_FUNC_END`` 配对使用。首先，它把这段指令序列标记为一个函数，并把其大小计算到生成的目标文件中。其次，它还便于检查和处理这类目标文件，因为工具可以轻松找到确切的函数边界。)

  So in most cases, developers should write something like in the following
  example, having some asm instructions in between the macros, of course::

    SYM_FUNC_START(memset)
        ... asm insns ...
    SYM_FUNC_END(memset)

  (因此在大多数情况下，开发者应当编写类似下面示例的代码，当然，两个宏之间会有一些汇编指令：)

  In fact, this kind of annotation corresponds to the now deprecated ``ENTRY``
  and ``ENDPROC`` macros.
  (事实上，这种注记对应于现已废弃的 ``ENTRY`` 和 ``ENDPROC`` 宏。)

* ``SYM_FUNC_ALIAS``, ``SYM_FUNC_ALIAS_LOCAL``, and ``SYM_FUNC_ALIAS_WEAK`` can
  be used to define multiple names for a function. The typical use is::

    SYM_FUNC_START(__memset)
        ... asm insns ...
    SYN_FUNC_END(__memset)
    SYM_FUNC_ALIAS(memset, __memset)

  ( ``SYM_FUNC_ALIAS`` 、 ``SYM_FUNC_ALIAS_LOCAL`` 与 ``SYM_FUNC_ALIAS_WEAK`` 可用于为一个函数定义多个名字。典型用法是：)

  In this example, one can call ``__memset`` or ``memset`` with the same
  result, except the debug information for the instructions is generated to
  the object file only once -- for the non-``ALIAS`` case.
  (在这个例子中，调用 ``__memset`` 或 ``memset`` 的效果相同，只是指令的调试信息只会生成到目标文件一次——即非 ``ALIAS`` 的情况。)

* ``SYM_CODE_START`` and ``SYM_CODE_START_LOCAL`` should be used only in
  special cases -- if you know what you are doing. This is used exclusively
  for interrupt handlers and similar where the calling convention is not the C
  one. ``_NOALIGN`` variants exist too. The use is the same as for the ``FUNC``
  category above::

    SYM_CODE_START_LOCAL(bad_put_user)
        ... asm insns ...
    SYM_CODE_END(bad_put_user)

  ( ``SYM_CODE_START`` 与 ``SYM_CODE_START_LOCAL`` 只应在特殊情况下使用——如果你清楚自己在做什么。它专用于中断处理程序及类似场景，那里的调用约定不是 C 约定。同样存在 ``_NOALIGN`` 变体。用法与上面的 ``FUNC`` 类别相同：)

  Again, every ``SYM_CODE_START*`` **shall** be coupled by ``SYM_CODE_END``.
  (同样，每个 ``SYM_CODE_START*`` **必须** 与 ``SYM_CODE_END`` 配对。)

  To some extent, this category corresponds to deprecated ``ENTRY`` and
  ``END``. Except ``END`` had several other meanings too.
  (在某种程度上，这一类别对应于已废弃的 ``ENTRY`` 和 ``END``。只不过 ``END`` 还有其他几个含义。)

* ``SYM_INNER_LABEL*`` is used to denote a label inside some
  ``SYM_{CODE,FUNC}_START`` and ``SYM_{CODE,FUNC}_END``.  They are very similar
  to C labels, except they can be made global. An example of use::

    SYM_CODE_START(ftrace_caller)
        /* save_mcount_regs fills in first two parameters */
        ...

    SYM_INNER_LABEL(ftrace_caller_op_ptr, SYM_L_GLOBAL)
        /* Load the ftrace_ops into the 3rd parameter */
        ...

    SYM_INNER_LABEL(ftrace_call, SYM_L_GLOBAL)
        call ftrace_stub
        ...
        retq
    SYM_CODE_END(ftrace_caller)

  ( ``SYM_INNER_LABEL*`` 用于表示 ``SYM_{CODE,FUNC}_START`` 与 ``SYM_{CODE,FUNC}_END`` 之间的某个标签。它们与 C 标签非常相似，区别是可以设为全局。用法示例：)

Data Macros(数据宏)
~~~~~~~~~~~~~~~~~~~
Similar to instructions, there is a couple of macros to describe data in the
assembly.
(与指令类似，也有几个宏用于描述汇编中的数据。)

* ``SYM_DATA_START`` and ``SYM_DATA_START_LOCAL`` mark the start of some data
  and shall be used in conjunction with either ``SYM_DATA_END``, or
  ``SYM_DATA_END_LABEL``. The latter adds also a label to the end, so that
  people can use ``lstack`` and (local) ``lstack_end`` in the following
  example::

    SYM_DATA_START_LOCAL(lstack)
        .skip 4096
    SYM_DATA_END_LABEL(lstack, SYM_L_LOCAL, lstack_end)

  ( ``SYM_DATA_START`` 与 ``SYM_DATA_START_LOCAL`` 标记某段数据的开始，应与 ``SYM_DATA_END`` 或 ``SYM_DATA_END_LABEL`` 结合使用。后者还会在结尾添加一个标签，这样人们就可以在下面的示例中使用 ``lstack`` 和(局部) ``lstack_end`` ：)

* ``SYM_DATA`` and ``SYM_DATA_LOCAL`` are variants for simple, mostly one-line
  data::

    SYM_DATA(HEAP,     .long rm_heap)
    SYM_DATA(heap_end, .long rm_stack)

  ( ``SYM_DATA`` 与 ``SYM_DATA_LOCAL`` 是用于简单、大多为一行数据的变体：)

  In the end, they expand to ``SYM_DATA_START`` with ``SYM_DATA_END``
  internally.
  (最终，它们在内部会展开为 ``SYM_DATA_START`` 加 ``SYM_DATA_END`` 。)

Support Macros(支持宏)
~~~~~~~~~~~~~~~~~~~~~~
All the above reduce themselves to some invocation of ``SYM_START``,
``SYM_END``, or ``SYM_ENTRY`` at last. Normally, developers should avoid using
these.
(上述所有宏最终都会归结为对 ``SYM_START`` 、 ``SYM_END`` 或 ``SYM_ENTRY`` 的某种调用。通常情况下，开发者应避免直接使用它们。)

Further, in the above examples, one could see ``SYM_L_LOCAL``. There are also
``SYM_L_GLOBAL`` and ``SYM_L_WEAK``. All are intended to denote linkage of a
symbol marked by them. They are used either in ``_LABEL`` variants of the
earlier macros, or in ``SYM_START``.
(此外，在上面的示例中可以看到 ``SYM_L_LOCAL``。还有 ``SYM_L_GLOBAL`` 和 ``SYM_L_WEAK``。它们都用于表示被标记符号的链接属性(linkage)。它们要么用在前面宏的 ``_LABEL`` 变体中，要么用在 ``SYM_START`` 中。)


Overriding Macros(覆盖宏)
~~~~~~~~~~~~~~~~~~~~~~~~~
Architecture can also override any of the macros in their own
``asm/linkage.h``, including macros specifying the type of a symbol
(``SYM_T_FUNC``, ``SYM_T_OBJECT``, and ``SYM_T_NONE``).  As every macro
described in this file is surrounded by ``#ifdef`` + ``#endif``, it is enough
to define the macros differently in the aforementioned architecture-dependent
header.
(架构也可以在自己的 ``asm/linkage.h`` 中覆盖这些宏中的任意一个，包括指定符号类型的宏( ``SYM_T_FUNC`` 、 ``SYM_T_OBJECT`` 与 ``SYM_T_NONE`` )。由于本文描述的每个宏都被 ``#ifdef`` + ``#endif`` 包围，只需在上述架构相关的头文件中以不同方式定义这些宏即可。)
