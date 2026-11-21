.. _programming_language:

Programming Language(编程语言)
==============================

The Linux kernel is written in the C programming language [c-language]_.
More precisely, it is typically compiled with ``gcc`` [gcc]_
under ``-std=gnu11`` [gcc-c-dialect-options]_: the GNU dialect of ISO C11.
``clang`` [clang]_ is also supported; see documentation on
:ref:`Building Linux with Clang/LLVM <kbuild_llvm>`.
(Linux 内核使用 C 程序设计语言编写。更准确地说，它通常在 ``-std=gnu11``
下用 ``gcc`` 编译——即 ISO C11 的 GNU 方言。 ``clang`` 也受支持；参见
:ref:`Building Linux with Clang/LLVM <kbuild_llvm>` 的相关文档。)

This dialect contains many extensions to the language [gnu-extensions]_,
and many of them are used within the kernel as a matter of course.
(这一方言包含对该语言的许多扩展，其中许多扩展在内核中理所当然地被使用。)

Attributes(属性)
----------------

One of the common extensions used throughout the kernel are attributes
[gcc-attribute-syntax]_. Attributes allow to introduce
implementation-defined semantics to language entities (like variables,
functions or types) without having to make significant syntactic changes
to the language (e.g. adding a new keyword) [n2049]_.
(内核中普遍使用的常见扩展之一是属性 (attributes)。属性允许在不对该语言做
重大语法更改 (例如新增关键字) 的情况下，为语言实体 (如变量、函数或类型)
引入由实现定义的语义。)

In some cases, attributes are optional (i.e. a compiler not supporting them
should still produce proper code, even if it is slower or does not perform
as many compile-time checks/diagnostics).
(在某些情况下，属性是可选的 (即不支持属性的编译器仍应生成正确的代码，哪怕
速度较慢或执行的编译期检查/诊断较少)。)

The kernel defines pseudo-keywords (e.g. ``__pure``) instead of using
directly the GNU attribute syntax (e.g. ``__attribute__((__pure__))``)
in order to feature detect which ones can be used and/or to shorten the code.
(内核定义伪关键字 (例如 ``__pure``) 而不是直接使用 GNU 属性语法 (例如
``__attribute__((__pure__))``)，以便进行特性检测、确定哪些可以使用，并/
或缩短代码。)

Please refer to ``include/linux/compiler_attributes.h`` for more information.
(更多信息请参阅 ``include/linux/compiler_attributes.h`` 。)

Rust(Rust 语言)
---------------

The kernel has support for the Rust programming language
[rust-language]_ under ``CONFIG_RUST``. It is compiled with ``rustc`` [rustc]_
under ``--edition=2021`` [rust-editions]_. Editions are a way to introduce
small changes to the language that are not backwards compatible.
(内核在 ``CONFIG_RUST`` 下支持 Rust 程序设计语言。它在 ``--edition=2021``
下用 ``rustc`` 编译。"版本 (edition)" 是为语言引入不向后兼容的小改动的一
种方式。)

On top of that, some unstable features [rust-unstable-features]_ are used in
the kernel. Unstable features may change in the future, thus it is an important
goal to reach a point where only stable features are used.
(除此之外，内核中还使用了一些不稳定特性。不稳定特性将来可能会变化，因此
达到只使用稳定特性的状态是一个重要目标。)

Please refer to Documentation/rust/index.rst for more information.
(更多信息请参阅 Documentation/rust/index.rst。)

.. [c-language] http://www.open-std.org/jtc1/sc22/wg14/www/standards
.. [gcc] https://gcc.gnu.org
.. [clang] https://clang.llvm.org
.. [gcc-c-dialect-options] https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html
.. [gnu-extensions] https://gcc.gnu.org/onlinedocs/gcc/C-Extensions.html
.. [gcc-attribute-syntax] https://gcc.gnu.org/onlinedocs/gcc/Attribute-Syntax.html
.. [n2049] http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2049.pdf
.. [rust-language] https://www.rust-lang.org
.. [rustc] https://doc.rust-lang.org/rustc/
.. [rust-editions] https://doc.rust-lang.org/edition-guide/editions/
.. [rust-unstable-features] https://github.com/Rust-for-Linux/linux/issues/2
