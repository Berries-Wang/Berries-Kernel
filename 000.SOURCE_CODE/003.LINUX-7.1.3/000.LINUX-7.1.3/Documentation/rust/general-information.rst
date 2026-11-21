.. SPDX-License-Identifier: GPL-2.0

General Information(常规信息)
=============================

This document contains useful information to know when working with
the Rust support in the kernel.
(本文包含在使用内核中的 Rust 支持时需要了解的有用信息。)


``no_std`` (无标准库)
---------------------

The Rust support in the kernel can link only `core <https://doc.rust-lang.org/core/>`_,
but not `std <https://doc.rust-lang.org/std/>`_. Crates for use in the
kernel must opt into this behavior using the ``#![no_std]`` attribute.
(内核中的 Rust 支持只能链接 `core <https://doc.rust-lang.org/core/>`_ ，而不能链接 `std <https://doc.rust-lang.org/std/>`_ 。要在内核中使用的 crate 必须通过 ``#![no_std]`` 属性选择这一行为。)


.. _rust_code_documentation:

Code documentation(代码文档)
----------------------------

Rust kernel code is documented using ``rustdoc``, its built-in documentation
generator.
(Rust 内核代码使用其内置的文档生成器 ``rustdoc`` 进行文档化。)

The generated HTML docs include integrated search, linked items (e.g. types,
functions, constants), source code, etc. They may be read at:
(生成的 HTML 文档包括集成搜索、带链接的条目(例如类型、函数、常量)、源代码等。可以在以下位置阅读：)

	https://rust.docs.kernel.org

For linux-next, please see:
(对于 linux-next，请参阅：)

	https://rust.docs.kernel.org/next/

There are also tags for each main release, e.g.:
(每个主发布版本也有对应的标签，例如：)

	https://rust.docs.kernel.org/6.10/

The docs can also be easily generated and read locally. This is quite fast
(same order as compiling the code itself) and no special tools or environment
are needed. This has the added advantage that they will be tailored to
the particular kernel configuration used. To generate them, use the ``rustdoc``
target with the same invocation used for compilation, e.g.::

	make LLVM=1 rustdoc

(这些文档也可以很容易地在本地生成并阅读。这相当快(与编译代码本身处于同一量级)，并且不需要特殊的工具或环境。这还有一个额外的好处：文档会针对所使用的特定内核配置量身定制。要生成它们，使用与编译相同的调用方式来运行 ``rustdoc`` 目标，例如：)

To read the docs locally in your web browser, run e.g.::

	xdg-open Documentation/output/rust/rustdoc/kernel/index.html

(要在本地 web 浏览器中阅读文档，运行例如：)

To learn about how to write the documentation, please see coding-guidelines.rst.
(要了解如何编写文档，请参阅 coding-guidelines.rst。)


Extra lints(额外的 lint)
------------------------

While ``rustc`` is a very helpful compiler, some extra lints and analyses are
available via ``clippy``, a Rust linter. To enable it, pass ``CLIPPY=1`` to
the same invocation used for compilation, e.g.::

	make LLVM=1 CLIPPY=1


(虽然 ``rustc`` 是一个非常有帮助的编译器，但还可以通过 ``clippy`` (一个 Rust linter) 使用一些额外的 lint 和分析。要启用它，向与编译相同的调用传入 ``CLIPPY=1`` ，例如：)

Please note that Clippy may change code generation, thus it should not be
enabled while building a production kernel.
(请注意，Clippy 可能会改变代码生成，因此在构建生产内核时不应启用它。)


Abstractions vs. bindings(抽象层与绑定)
---------------------------------------

Abstractions are Rust code wrapping kernel functionality from the C side.
(抽象层 (Abstractions) 是包装来自 C 侧的内核功能的 Rust 代码。)

In order to use functions and types from the C side, bindings are created.
Bindings are the declarations for Rust of those functions and types from
the C side.
(为了使用来自 C 侧的函数和类型，会创建绑定 (bindings)。绑定是那些来自 C 侧的函数和类型在 Rust 中的声明。)

For instance, one may write a ``Mutex`` abstraction in Rust which wraps
a ``struct mutex`` from the C side and calls its functions through the bindings.
(例如，可以在 Rust 中编写一个 ``Mutex`` 抽象，它包装来自 C 侧的 ``struct mutex``，并通过绑定调用其函数。)

Abstractions are not available for all the kernel internal APIs and concepts,
but it is intended that coverage is expanded as time goes on. "Leaf" modules
(e.g. drivers) should not use the C bindings directly. Instead, subsystems
should provide as-safe-as-possible abstractions as needed.
(抽象层并非覆盖所有内核内部 API 和概念，但目标是随时间推移不断扩大覆盖范围。"叶子" (leaf) 模块(例如驱动)不应直接使用 C 绑定。相反，子系统应按需提供尽可能安全的抽象。)

.. code-block::

	                                                rust/bindings/
	                                               (rust/helpers/)

	                                                   include/ -----+ <-+
	                                                                 |   |
	  drivers/              rust/kernel/              +----------+ <-+   |
	    fs/                                           | bindgen  |       |
	   .../            +-------------------+          +----------+ --+   |
	                   |    Abstractions   |                         |   |
	+---------+        | +------+ +------+ |          +----------+   |   |
	| my_foo  | -----> | | foo  | | bar  | | -------> | Bindings | <-+   |
	| driver  |  Safe  | | sub- | | sub- | |  Unsafe  |          |       |
	+---------+        | |system| |system| |          | bindings | <-----+
	     |             | +------+ +------+ |          |  crate   |       |
	     |             |   kernel crate    |          +----------+       |
	     |             +-------------------+                             |
	     |                                                               |
	     +------------------# FORBIDDEN #--------------------------------+

The main idea is to encapsulate all direct interaction with the kernel's C APIs
into carefully reviewed and documented abstractions. Then users of these
abstractions cannot introduce undefined behavior (UB) as long as:

(主要思想是将与内核 C API 的所有直接交互封装到经过仔细审查并编写了文档的抽象层中。这样，只要满足以下条件，这些抽象的使用者就不会引入未定义行为 (UB)：)

#. The abstractions are correct ("sound").
   (这些抽象是正确的 ("sound"，健全的)。)
#. Any ``unsafe`` blocks respect the safety contract necessary to call the
   operations inside the block. Similarly, any ``unsafe impl``\ s respect the
   safety contract necessary to implement the trait.
   (任何 ``unsafe`` 块都遵守调用块内操作所需的安全契约。同样，任何
   ``unsafe impl``\ s 都遵守实现该 trait 所需的安全契约。)

Bindings(绑定)
~~~~~~~~~~~~~~

By including a C header from ``include/`` into
``rust/bindings/bindings_helper.h``, the ``bindgen`` tool will auto-generate the
bindings for the included subsystem. After building, see the ``*_generated.rs``
output files in the ``rust/bindings/`` directory.
(通过把 ``include/`` 中的 C 头文件包含进 ``rust/bindings/bindings_helper.h`` ， ``bindgen`` 工具会为所包含的子系统自动生成绑定。构建之后，请查看 ``rust/bindings/`` 目录中的 ``*_generated.rs`` 输出文件。)

For parts of the C header that ``bindgen`` does not auto generate, e.g. C
``inline`` functions or non-trivial macros, it is acceptable to add a small
wrapper function to ``rust/helpers/`` to make it available for the Rust side as
well.
(对于 ``bindgen`` 不能自动生成的 C 头文件部分，例如 C ``inline`` 函数或非平凡宏，可以在 ``rust/helpers/`` 中添加一个小的包装函数，使其对 Rust 侧也可用。)

Abstractions(抽象层)
~~~~~~~~~~~~~~~~~~~~

Abstractions are the layer between the bindings and the in-kernel users. They
are located in ``rust/kernel/`` and their role is to encapsulate the unsafe
access to the bindings into an as-safe-as-possible API that they expose to their
users. Users of the abstractions include things like drivers or file systems
written in Rust.
(抽象层是绑定与内核内使用者之间的层。它们位于 ``rust/kernel/`` 中，其作用是把对绑定的不安全访问封装成一个尽可能安全的 API 并暴露给使用者。抽象层的使用者包括用 Rust 编写的驱动或文件系统等。)

Besides the safety aspect, the abstractions are supposed to be "ergonomic", in
the sense that they turn the C interfaces into "idiomatic" Rust code. Basic
examples are to turn the C resource acquisition and release into Rust
constructors and destructors or C integer error codes into Rust's ``Result``\ s.
(除了安全性之外，抽象层还应当是"符合人体工程学"的 (ergonomic)，即它们把 C 接口转换成"符合习惯用法" (idiomatic) 的 Rust 代码。基本的例子包括：把 C 的资源获取与释放转换成 Rust 的构造函数与析构函数，或者把 C 的整数错误码转换成 Rust 的 ``Result``\ s。)


Conditional compilation(条件编译)
---------------------------------

Rust code has access to conditional compilation based on the kernel
configuration:
(Rust 代码可以访问基于内核配置的条件编译：)

.. code-block:: rust

	#[cfg(CONFIG_X)]       // Enabled               (`y` or `m`)
	#[cfg(CONFIG_X="y")]   // Enabled as a built-in (`y`)
	#[cfg(CONFIG_X="m")]   // Enabled as a module   (`m`)
	#[cfg(not(CONFIG_X))]  // Disabled

For other predicates that Rust's ``cfg`` does not support, e.g. expressions with
numerical comparisons, one may define a new Kconfig symbol:
(对于 Rust 的 ``cfg`` 不支持的其他谓词，例如带数值比较的表达式，可以定义一个新的 Kconfig 符号：)

.. code-block:: kconfig

	config RUSTC_HAS_SPAN_FILE
		def_bool RUSTC_VERSION >= 108800
