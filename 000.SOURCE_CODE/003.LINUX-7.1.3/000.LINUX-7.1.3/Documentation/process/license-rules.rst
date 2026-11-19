.. SPDX-License-Identifier: GPL-2.0

.. _kernel_licensing:

Linux kernel licensing rules(内核许可规则)
==========================================

The Linux Kernel is provided under the terms of the GNU General Public
License version 2 only (GPL-2.0), as provided in LICENSES/preferred/GPL-2.0,
with an explicit syscall exception described in
LICENSES/exceptions/Linux-syscall-note, as described in the COPYING file.
( Linux 内核以仅第 2 版 GNU 通用公共许可证 (GPL-2.0) 的条款提供——即
LICENSES/preferred/GPL-2.0 中给出的许可证文本，并带有
LICENSES/exceptions/Linux-syscall-note 中描述的显式系统调用例外，如 COPYING
文件所述。)

This documentation file provides a description of how each source file
should be annotated to make its license clear and unambiguous.
It doesn't replace the Kernel's license.
(本说明文件描述应如何对每个源文件进行注释，使其许可证清晰且无歧义。它并不
取代内核自身的许可证。)

The license described in the COPYING file applies to the kernel source
as a whole, though individual source files can have a different license
which is required to be compatible with the GPL-2.0::

    GPL-1.0+  :  GNU General Public License v1.0 or later
    GPL-2.0+  :  GNU General Public License v2.0 or later
    LGPL-2.0  :  GNU Library General Public License v2 only
    LGPL-2.0+ :  GNU Library General Public License v2 or later
    LGPL-2.1  :  GNU Lesser General Public License v2.1 only
    LGPL-2.1+ :  GNU Lesser General Public License v2.1 or later

( COPYING 文件中描述的许可证适用于内核源码整体，不过单个源文件可以采用不同
的许可证，只要它与 GPL-2.0 兼容：)

Aside from that, individual files can be provided under a dual license,
e.g. one of the compatible GPL variants and alternatively under a
permissive license like BSD, MIT etc.
(除此之外，单个文件可以以双许可证提供，例如一个兼容的 GPL 变体，或另一种
宽松许可 (如 BSD、MIT 等)。)

The User-space API (UAPI) header files, which describe the interface of
user-space programs to the kernel are a special case.  According to the
note in the kernel COPYING file, the syscall interface is a clear boundary,
which does not extend the GPL requirements to any software which uses it to
communicate with the kernel.  Because the UAPI headers must be includable
into any source files which create an executable running on the Linux
kernel, the exception must be documented by a special license expression.
(描述用户空间程序与内核之间接口的用户空间 API (UAPI) 头文件是一个特例。根据
内核 COPYING 文件中的说明，系统调用接口是一个清晰的边界，它不会把 GPL 的要求
扩展到任何使用该接口与内核通信的软件。由于 UAPI 头文件必须能被任何创建在
Linux 内核上运行的可执行文件的源文件所包含，这个例外必须用一个特殊的许可证
表达式来记录。)

The common way of expressing the license of a source file is to add the
matching boilerplate text into the top comment of the file.  Due to
formatting, typos etc. these "boilerplates" are hard to validate for
tools which are used in the context of license compliance.
(表达源文件许可证的常见方式，是把匹配的样板文字加入文件顶部的注释。由于
排版、笔误等原因，这些 "样板" 对于许可证合规场景中使用的工具来说很难校验。)

An alternative to boilerplate text is the use of Software Package Data
Exchange (SPDX) license identifiers in each source file.  SPDX license
identifiers are machine parsable and precise shorthands for the license
under which the content of the file is contributed.  SPDX license
identifiers are managed by the SPDX Workgroup at the Linux Foundation and
have been agreed on by partners throughout the industry, tool vendors, and
legal teams.  For further information see https://spdx.org/
(样板文字的另一种替代方案，是在每个源文件中使用软件包数据交换 (SPDX) 许可证
标识符。SPDX 许可证标识符是机器可解析且精确的简写，标明文件内容以何种许可证
贡献。SPDX 许可证标识符由 Linux 基金会的 SPDX 工作组管理，并已获得业界伙伴、
工具厂商和法律团队的认可。更多信息参见 https://spdx.org/)

The Linux kernel requires the precise SPDX identifier in all source files.
The valid identifiers used in the kernel are explained in the section
`License identifiers(许可证标识符)`_ and have been retrieved from the official SPDX
license list at https://spdx.org/licenses/ along with the license texts.
( Linux 内核要求所有源文件都使用精确的 SPDX 标识符。内核中使用的有效标识符在
`License identifiers(许可证标识符)`_ 一节中说明，它们连同许可证文本一起取自
https://spdx.org/licenses/ 上的官方 SPDX 许可证列表。)

License identifier syntax(许可证标识符语法)
-------------------------------------------

1. Placement:
   (放置位置：)

   The SPDX license identifier in kernel files shall be added at the first
   possible line in a file which can contain a comment.  For the majority
   of files this is the first line, except for scripts which require the
   '#!PATH_TO_INTERPRETER' in the first line.  For those scripts, the SPDX
   license identifier goes into the second line.
   (内核文件中的 SPDX 许可证标识符应添加到文件中第一个可包含注释的行。对大多
   数文件而言是第一行，但要求第一行是 '#!PATH_TO_INTERPRETER' 的脚本除外。对
   于这类脚本，SPDX 许可证标识符放在第二行。)

   The license identifier line can then be followed by one or multiple
   SPDX-FileCopyrightText lines if desired.
   (许可证标识符行之后，如有需要可以跟一行或多行 SPDX-FileCopyrightText 行。)

|

2. Style:
   (风格：)

   The SPDX license identifier is added in form of a comment.  The comment
   style depends on the file type::

      C source:	// SPDX-License-Identifier: <SPDX License Expression>
      C header:	/* SPDX-License-Identifier: <SPDX License Expression> */
      ASM:	/* SPDX-License-Identifier: <SPDX License Expression> */
      scripts:	# SPDX-License-Identifier: <SPDX License Expression>
      .rst:	.. SPDX-License-Identifier: <SPDX License Expression>
      .dts{i}:	// SPDX-License-Identifier: <SPDX License Expression>

   ( SPDX 许可证标识符以注释的形式添加。注释风格取决于文件类型。)

   If a specific tool cannot handle the standard comment style, then the
   appropriate comment mechanism which the tool accepts shall be used. This
   is the reason for having the "/\* \*/" style comment in C header
   files. There was build breakage observed with generated .lds files where
   'ld' failed to parse the C++ comment. This has been fixed by now, but
   there are still older assembler tools which cannot handle C++ style
   comments.
   (如果特定工具无法处理标准注释风格，则应使用该工具所能接受的相应注释机制。
   这正是 C 头文件使用 "/\* \*/" 风格注释的原因。曾观察到生成的 .lds 文件
   造成构建失败——'ld' 无法解析 C++ 风格注释。这个问题现已修复，但仍有较老
   的汇编工具无法处理 C++ 风格注释。)

|

3. Syntax:
   (语法：)

   A <SPDX License Expression> is either an SPDX short form license
   identifier found on the SPDX License List, or the combination of two
   SPDX short form license identifiers separated by "WITH" when a license
   exception applies. When multiple licenses apply, an expression consists
   of keywords "AND", "OR" separating sub-expressions and surrounded by
   "(", ")" .
   ( <SPDX License Expression> 要么是 SPDX 许可证列表中出现的 SPDX 短形式
   许可证标识符，要么是在适用许可证例外时用 "WITH" 分隔的两个 SPDX 短形式
   许可证标识符的组合。当适用多个许可证时，表达式由关键字 "AND"、"OR" 分隔
   各子表达式，并用 "(" 、 ")" 包围。)

   License identifiers for licenses like [L]GPL with the 'or later' option
   are constructed by using a "+" for indicating the 'or later' option.::

      // SPDX-License-Identifier: GPL-2.0+
      // SPDX-License-Identifier: LGPL-2.1+

   (对于带 'or later' 选项的 [L]GPL 等许可证，许可证标识符用 "+" 来表示
   'or later' 选项。)

   WITH should be used when there is a modifier to a license needed.
   For example, the linux kernel UAPI files use the expression::

      // SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note
      // SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note

   (当需要对许可证加以修饰时，应使用 WITH。例如，linux 内核 UAPI 文件使用
   如下表达式：)

   Other examples using WITH exceptions found in the kernel are::

      // SPDX-License-Identifier: GPL-2.0 WITH mif-exception
      // SPDX-License-Identifier: GPL-2.0+ WITH GCC-exception-2.0

   (内核中还有其他使用 WITH 例外的示例：)

   Exceptions can only be used with particular License identifiers. The
   valid License identifiers are listed in the tags of the exception text
   file. For details see the point `Exceptions`_ in the chapter `License
   identifiers(许可证标识符)`_.
   (例外只能与特定的许可证标识符一起使用。有效的许可证标识符列在例外文本文件
   的标签中。详情参见 `License identifiers(许可证标识符)`_ 一章中的
   `Exceptions`_ 一节。)

   OR should be used if the file is dual licensed and only one license is
   to be selected.  For example, some dtsi files are available under dual
   licenses::

      // SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause

   ( OR 应在文件为双许可、且只需选择其中一种许可证时使用。例如，一些 dtsi
   文件以双许可证提供：)

   Examples from the kernel for license expressions in dual licensed files::

      // SPDX-License-Identifier: GPL-2.0 OR MIT
      // SPDX-License-Identifier: GPL-2.0 OR BSD-2-Clause
      // SPDX-License-Identifier: GPL-2.0 OR Apache-2.0
      // SPDX-License-Identifier: GPL-2.0 OR MPL-1.1
      // SPDX-License-Identifier: (GPL-2.0 WITH Linux-syscall-note) OR MIT
      // SPDX-License-Identifier: GPL-1.0+ OR BSD-3-Clause OR OpenSSL

   (内核中双许可文件许可证表达式的示例：)

   AND should be used if the file has multiple licenses whose terms all
   apply to use the file. For example, if code is inherited from another
   project and permission has been given to put it in the kernel, but the
   original license terms need to remain in effect::

      // SPDX-License-Identifier: (GPL-2.0 WITH Linux-syscall-note) AND MIT

   ( AND 应在文件拥有多个许可证、且使用该文件时须遵循其全部条款时使用。
   例如，代码继承自另一个项目、并被许可放入内核，但其原始许可证条款需要保持
   有效时：)

   Another other example where both sets of license terms need to be
   adhered to is::

      // SPDX-License-Identifier: GPL-1.0+ AND LGPL-2.1+

   (另一个须同时遵循两组许可证条款的示例：)

License identifiers(许可证标识符)
---------------------------------

The licenses currently used, as well as the licenses for code added to the
kernel, can be broken down into:
(当前使用的许可证，以及添加到内核的代码所用的许可证，可分为以下几类：)

1. _`Preferred licenses`:
   (首选许可证：)

   Whenever possible these licenses should be used as they are known to be
   fully compatible and widely used.  These licenses are available from the
   directory::

      LICENSES/preferred/

   in the kernel source tree.
   (只要可能，就应使用这些许可证，因为它们已知完全兼容且被广泛使用。这些
   许可证可从内核源码树的 LICENSES/preferred/ 目录获取。)

   The files in this directory contain the full license text and
   `Metatags`_.  The file names are identical to the SPDX license
   identifier which shall be used for the license in source files.
   (该目录中的文件包含完整的许可证文本和 `Metatags`_ 。文件名与源文件中该
   许可证应使用的 SPDX 许可证标识符完全相同。)

   Examples::

      LICENSES/preferred/GPL-2.0

   (示例：)

   Contains the GPL version 2 license text and the required metatags::

      LICENSES/preferred/MIT

   (包含 GPL 第 2 版许可证文本及所需的元标签：)

   Contains the MIT license text and the required metatags

   (包含 MIT 许可证文本及所需的元标签。)

   _`Metatags`:

   The following meta tags must be available in a license file:
   (许可证文件中必须包含以下元标签：)

   - Valid-License-Identifier:

     One or more lines which declare which License Identifiers are valid
     inside the project to reference this particular license text.  Usually
     this is a single valid identifier, but e.g. for licenses with the 'or
     later' options two identifiers are valid.
     (一行或多行，声明在项目内部哪些许可证标识符 (License Identifier) 是有效的、
     可用来引用这份特定的许可证文本。通常只有一个有效标识符；但对于带 'or
     later' 选项的许可证，两个标识符都有效。)

   - SPDX-URL:

     The URL of the SPDX page which contains additional information related
     to the license.
     (包含与该许可证相关补充信息的 SPDX 页面的 URL。)

   - Usage-Guidance:

     Freeform text for usage advice. The text must include correct examples
     for the SPDX license identifiers as they should be put into source
     files according to the `License identifier syntax(许可证标识符语法)`_ guidelines.
     (用法建议的自由文本。文本必须包含正确的 SPDX 许可证标识符示例——这些示例
     应按照 `License identifier syntax(许可证标识符语法)`_ 指南放入源文件。)

   - License-Text:

     All text after this tag is treated as the original license text
     (该标签之后的所有文本都被视为原始许可证文本)

   File format examples::

      Valid-License-Identifier: GPL-2.0
      Valid-License-Identifier: GPL-2.0+
      SPDX-URL: https://spdx.org/licenses/GPL-2.0.html
      Usage-Guide:
        To use this license in source code, put one of the following SPDX
	tag/value pairs into a comment according to the placement
	guidelines in the licensing rules documentation.
	For 'GNU General Public License (GPL) version 2 only' use:
	  SPDX-License-Identifier: GPL-2.0
	For 'GNU General Public License (GPL) version 2 or any later version' use:
	  SPDX-License-Identifier: GPL-2.0+
      License-Text:
        Full license text

   (文件格式示例：)

   ::

      SPDX-License-Identifier: MIT
      SPDX-URL: https://spdx.org/licenses/MIT.html
      Usage-Guide:
	To use this license in source code, put the following SPDX
	tag/value pair into a comment according to the placement
	guidelines in the licensing rules documentation.
	  SPDX-License-Identifier: MIT
      License-Text:
        Full license text

|

2. Deprecated licenses:
   (已弃用的许可证：)

   These licenses should only be used for existing code or for importing
   code from a different project.  These licenses are available from the
   directory::

      LICENSES/deprecated/

   in the kernel source tree.
   (这些许可证只应用于现有代码，或用于从其他项目导入代码。这些许可证可从
   内核源码树的 LICENSES/deprecated/ 目录获取。)

   The files in this directory contain the full license text and
   `Metatags`_.  The file names are identical to the SPDX license
   identifier which shall be used for the license in source files.
   (该目录中的文件包含完整的许可证文本和 `Metatags`_ 。文件名与源文件中该
   许可证应使用的 SPDX 许可证标识符完全相同。)

   Examples::

      LICENSES/deprecated/ISC

   (示例：)

   Contains the Internet Systems Consortium license text and the required
   metatags::

      LICENSES/deprecated/GPL-1.0

   (包含 Internet Systems Consortium 许可证文本及所需的元标签：)

   Contains the GPL version 1 license text and the required metatags.

   (包含 GPL 第 1 版许可证文本及所需的元标签。)

   Metatags:

   (元标签：)

   The metatag requirements for 'other' licenses are identical to the
   requirements of the `Preferred licenses`_.
   ('other' 类许可证的元标签要求与 `Preferred licenses`_ 相同。)

   File format example::

      Valid-License-Identifier: ISC
      SPDX-URL: https://spdx.org/licenses/ISC.html
      Usage-Guide:
        Usage of this license in the kernel for new code is discouraged
	and it should solely be used for importing code from an already
	existing project.
        To use this license in source code, put the following SPDX
	tag/value pair into a comment according to the placement
	guidelines in the licensing rules documentation.
	  SPDX-License-Identifier: ISC
      License-Text:
        Full license text

   (文件格式示例：)

|

3. Dual Licensing Only
   (仅用于双许可：)

   These licenses should only be used to dual license code with another
   license in addition to a preferred license.  These licenses are available
   from the directory::

      LICENSES/dual/

   in the kernel source tree.
   (这些许可证只应用于在首选许可证之外、以另一种许可证对代码进行双许可。
   这些许可证可从内核源码树的 LICENSES/dual/ 目录获取。)

   The files in this directory contain the full license text and
   `Metatags`_.  The file names are identical to the SPDX license
   identifier which shall be used for the license in source files.
   (该目录中的文件包含完整的许可证文本和 `Metatags`_ 。文件名与源文件中该
   许可证应使用的 SPDX 许可证标识符完全相同。)

   Examples::

      LICENSES/dual/MPL-1.1

   (示例：)

   Contains the Mozilla Public License version 1.1 license text and the
   required metatags::

      LICENSES/dual/Apache-2.0

   (包含 Mozilla 公共许可证 1.1 版许可证文本及所需的元标签：)

   Contains the Apache License version 2.0 license text and the required
   metatags.

   (包含 Apache 许可证 2.0 版许可证文本及所需的元标签。)

   Metatags:

   (元标签：)

   The metatag requirements for 'other' licenses are identical to the
   requirements of the `Preferred licenses`_.
   ('other' 类许可证的元标签要求与 `Preferred licenses`_ 相同。)

   File format example::

      Valid-License-Identifier: MPL-1.1
      SPDX-URL: https://spdx.org/licenses/MPL-1.1.html
      Usage-Guide:
        Do NOT use. The MPL-1.1 is not GPL2 compatible. It may only be used for
        dual-licensed files where the other license is GPL2 compatible.
        If you end up using this it MUST be used together with a GPL2 compatible
        license using "OR".
        To use the Mozilla Public License version 1.1 put the following SPDX
        tag/value pair into a comment according to the placement guidelines in
        the licensing rules documentation:
      SPDX-License-Identifier: MPL-1.1
      License-Text:
        Full license text

   (文件格式示例：)

|

4. _`Exceptions`:
   (例外：)

   Some licenses can be amended with exceptions which grant certain rights
   which the original license does not.  These exceptions are available
   from the directory::

      LICENSES/exceptions/

   in the kernel source tree.  The files in this directory contain the full
   exception text and the required `Exception Metatags`_.
   (某些许可证可以通过例外 (exception) 进行修订，授予原始许可证所没有的某些
   权利。这些例外可从内核源码树的 LICENSES/exceptions/ 目录获取。该目录中的
   文件包含完整的例外文本和所需的 `Exception Metatags`_ 。)

   Examples::

      LICENSES/exceptions/Linux-syscall-note

   (示例：)

   Contains the Linux syscall exception as documented in the COPYING
   file of the Linux kernel, which is used for UAPI header files.
   e.g. /\* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note \*/::

      LICENSES/exceptions/GCC-exception-2.0

   (包含 Linux 内核 COPYING 文件中所述的 Linux 系统调用例外，用于 UAPI 头文件。
   例如 /\* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note \*/：)

   Contains the GCC 'linking exception' which allows to link any binary
   independent of its license against the compiled version of a file marked
   with this exception. This is required for creating runnable executables
   from source code which is not compatible with the GPL.
   (包含 GCC "链接例外"，它允许任何二进制程序——无论其许可证为何——链接标有
   该例外的文件的编译版本。这是从与 GPL 不兼容的源代码创建可运行可执行文件
   所必需的。)

   _`Exception Metatags`:

   The following meta tags must be available in an exception file:
   (例外文件中必须包含以下元标签：)

   - SPDX-Exception-Identifier:

     One exception identifier which can be used with SPDX license
     identifiers.
     (一个可与 SPDX 许可证标识符一起使用的例外标识符。)

   - SPDX-URL:

     The URL of the SPDX page which contains additional information related
     to the exception.
     (包含与该例外相关补充信息的 SPDX 页面的 URL。)

   - SPDX-Licenses:

     A comma separated list of SPDX license identifiers for which the
     exception can be used.
     (以逗号分隔的 SPDX 许可证标识符列表——该例外可与这些许可证一起使用。)

   - Usage-Guidance:

     Freeform text for usage advice. The text must be followed by correct
     examples for the SPDX license identifiers as they should be put into
     source files according to the `License identifier syntax(许可证标识符语法)`_ guidelines.
     (用法建议的自由文本。文本之后必须跟有正确的 SPDX 许可证标识符示例——
     这些示例应按照 `License identifier syntax(许可证标识符语法)`_ 指南放入
     源文件。)

   - Exception-Text:

     All text after this tag is treated as the original exception text
     (该标签之后的所有文本都被视为原始例外文本)

   File format examples::

      SPDX-Exception-Identifier: Linux-syscall-note
      SPDX-URL: https://spdx.org/licenses/Linux-syscall-note.html
      SPDX-Licenses: GPL-2.0, GPL-2.0+, GPL-1.0+, LGPL-2.0, LGPL-2.0+, LGPL-2.1, LGPL-2.1+
      Usage-Guidance:
        This exception is used together with one of the above SPDX-Licenses
	to mark user-space API (uapi) header files so they can be included
	into non GPL compliant user-space application code.
        To use this exception add it with the keyword WITH to one of the
	identifiers in the SPDX-Licenses tag:
	  SPDX-License-Identifier: <SPDX-License> WITH Linux-syscall-note
      Exception-Text:
        Full exception text

   (文件格式示例：)

   ::

      SPDX-Exception-Identifier: GCC-exception-2.0
      SPDX-URL: https://spdx.org/licenses/GCC-exception-2.0.html
      SPDX-Licenses: GPL-2.0, GPL-2.0+
      Usage-Guidance:
        The "GCC Runtime Library exception 2.0" is used together with one
	of the above SPDX-Licenses for code imported from the GCC runtime
	library.
        To use this exception add it with the keyword WITH to one of the
	identifiers in the SPDX-Licenses tag:
	  SPDX-License-Identifier: <SPDX-License> WITH GCC-exception-2.0
      Exception-Text:
        Full exception text


All SPDX license identifiers and exceptions must have a corresponding file
in the LICENSES subdirectories. This is required to allow tool
verification (e.g. checkpatch.pl) and to have the licenses ready to read
and extract right from the source, which is recommended by various FOSS
organizations, e.g. the `FSFE REUSE initiative <https://reuse.software/>`_.
(所有 SPDX 许可证标识符和例外都必须在 LICENSES 子目录中有对应的文件。这是
为了允许工具校验 (例如 checkpatch.pl)，并让许可证可以随时从源码中直接读取
和提取——这也是多个 FOSS 组织 (例如 `FSFE REUSE initiative <https://reuse.software/>`_ )
所建议的做法。)

_`MODULE_LICENSE`
-----------------

   Loadable kernel modules also require a MODULE_LICENSE() tag. This tag is
   neither a replacement for proper source code license information
   (SPDX-License-Identifier) nor in any way relevant for expressing or
   determining the exact license under which the source code of the module
   is provided.

   Loadable kernel modules also require a MODULE_LICENSE() tag. This tag is
   neither a replacement for proper source code license information
   (SPDX-License-Identifier) nor in any way relevant for expressing or
   determining the exact license under which the source code of the module
   is provided.
   (可加载内核模块还需要一个 MODULE_LICENSE() 标签。该标签既不能替代恰当的
   源代码许可证信息 (SPDX-License-Identifier)，也与表达或确定模块源代码所
   提供的确切许可证毫无关系。)

   The sole purpose of this tag is to provide sufficient information
   whether the module is free software or proprietary for the kernel
   module loader and for user space tools.
   (该标签的唯一目的，是为内核模块加载器和用户空间工具提供足够的信息，以
   判断模块是自由软件还是专有软件。)

   The valid license strings for MODULE_LICENSE() are:
   ( MODULE_LICENSE() 的有效许可证字符串如下：)

    ============================= =============================================
    "GPL"			  Module is licensed under GPL version 2. This
				  does not express any distinction between
				  GPL-2.0-only or GPL-2.0-or-later. The exact
				  license information can only be determined
				  via the license information in the
				  corresponding source files.
                                  (模块以 GPL 第 2 版授权。这不表达 GPL-2.0-only 与
                                  GPL-2.0-or-later 之间的任何区别。确切的许可证信息只能通过对应源文件中的
                                  许可证信息确定。)

    "GPL v2"			  Same as "GPL". It exists for historic
				  reasons.
                                  (与 "GPL" 相同。它因历史原因而存在。)

    "GPL and additional rights"   Historical variant of expressing that the
				  module source is dual licensed under a
				  GPL v2 variant and MIT license. Please do
				  not use in new code.
                                  (表达模块源码以 GPL v2 变体与 MIT 许可证进行双许可的历史变体写法。请勿在
                                  新代码中使用。)

    "Dual MIT/GPL"		  The correct way of expressing that the
				  module is dual licensed under a GPL v2
				  variant or MIT license choice.
                                  (表达模块以 GPL v2 变体或 MIT 许可证选择进行双许可的正确方式。)

    "Dual BSD/GPL"		  The module is dual licensed under a GPL v2
				  variant or BSD license choice. The exact
				  variant of the BSD license can only be
				  determined via the license information
				  in the corresponding source files.
                                  (模块以 GPL v2 变体或 BSD 许可证选择进行双许可。BSD 许可证的确切变体只能
                                  通过对应源文件中的许可证信息确定。)

    "Dual MPL/GPL"		  The module is dual licensed under a GPL v2
				  variant or Mozilla Public License (MPL)
				  choice. The exact variant of the MPL
				  license can only be determined via the
				  license information in the corresponding
				  source files.
                                  (模块以 GPL v2 变体或 Mozilla 公共许可证 (MPL) 选择进行双许可。MPL 许可证
                                  的确切变体只能通过对应源文件中的许可证信息确定。)

    "Proprietary"		  The module is under a proprietary license.
				  "Proprietary" is to be understood only as
				  "The license is not compatible to GPLv2".
                                  This string is solely for non-GPL2 compatible
                                  third party modules and cannot be used for
                                  modules which have their source code in the
                                  kernel tree. Modules tagged that way are
                                  tainting the kernel with the 'P' flag when
                                  loaded and the kernel module loader refuses
                                  to link such modules against symbols which
                                  are exported with EXPORT_SYMBOL_GPL().
                                  (该模块基于专有许可证。"Proprietary" 仅应理解为 "该许可证与 GPLv2 不兼容"。
                                  此字符串仅用于非 GPL2 兼容的第三方模块，不能用于源代码位于内核树中的模块。
                                  以该方式标记的模块在加载时会以 'P' 标志污染 (taint) 内核，且内核模块加载器
                                  拒绝将这类模块与以 EXPORT_SYMBOL_GPL() 导出的符号相链接。)
    ============================= =============================================



