.. SPDX-License-Identifier: GPL-2.0

Coding Guidelines(编码规范)
============================

This document describes how to write Rust code in the kernel.
(本文档描述如何在内核中编写 Rust 代码。)


Style & formatting(风格与格式化)
--------------------------------

The code should be formatted using ``rustfmt``. In this way, a person
contributing from time to time to the kernel does not need to learn and
remember one more style guide. More importantly, reviewers and maintainers
do not need to spend time pointing out style issues anymore, and thus
less patch roundtrips may be needed to land a change.
(代码应使用 ``rustfmt`` 进行格式化。这样，偶尔向内核贡献代码的人不需要再学习
并记住又一份风格指南。更重要的是，审查者和维护者不必再花时间指出风格问题，
因此落地一个改动所需的补丁往返可能更少。)

.. note:: Conventions on comments and documentation are not checked by
  ``rustfmt``. Thus those are still needed to be taken care of.
  (注意：关于注释和文档的约定不在 ``rustfmt`` 的检查范围之内，因此这些仍然
  需要自行留意。)

The default settings of ``rustfmt`` are used. This means the idiomatic Rust
style is followed. For instance, 4 spaces are used for indentation rather
than tabs.
(使用 ``rustfmt`` 的默认设置。这意味着遵循地道的 Rust 风格。例如，缩进使用
4 个空格而不是制表符。)

It is convenient to instruct editors/IDEs to format while typing,
when saving or at commit time. However, if for some reason reformatting
the entire kernel Rust sources is needed at some point, the following can be
run::

	make LLVM=1 rustfmt

(在编辑器/IDE 中设置在输入时、保存时或提交时进行格式化会很方便。不过，如果
出于某种原因需要在某个时候重新格式化整个内核 Rust 源码，可以运行以下命令：)

It is also possible to check if everything is formatted (printing a diff
otherwise), for instance for a CI, with::

	make LLVM=1 rustfmtcheck

(也可以检查所有内容是否已格式化 (否则打印 diff)，例如在 CI 中，使用：)

Like ``clang-format`` for the rest of the kernel, ``rustfmt`` works on
individual files, and does not require a kernel configuration. Sometimes it may
even work with broken code.
(与内核其余部分使用的 ``clang-format`` 一样， ``rustfmt`` 针对单个文件工作，
不需要内核配置。有时它甚至可以在代码有破损的情况下工作。)

Imports(导入)
~~~~~~~~~~~~~

``rustfmt``, by default, formats imports in a way that is prone to conflicts
while merging and rebasing, since in some cases it condenses several items into
the same line. For instance:
( ``rustfmt`` 默认格式化导入 (imports) 的方式容易在合并和变基时产生冲突，
因为在某些情况下它会把多个条目压缩到同一行。例如：)

.. code-block:: rust

	// Do not use this style.
	use crate::{
	    example1,
	    example2::{example3, example4, example5},
	    example6, example7,
	    example8::example9,
	};

Instead, the kernel uses a vertical layout that looks like this:
(相反，内核使用如下所示的垂直 (vertical) 布局：)

.. code-block:: rust

	use crate::{
	    example1,
	    example2::{
	        example3,
	        example4,
	        example5, //
	    },
	    example6,
	    example7,
	    example8::example9, //
	};

That is, each item goes into its own line, and braces are used as soon as there
is more than one item in a list.
(也就是说，每个条目各占一行，只要列表中的条目多于一个就使用花括号。)

The trailing empty comment allows to preserve this formatting. Not only that,
``rustfmt`` will actually reformat imports vertically when the empty comment is
added. That is, it is possible to easily reformat the original example into the
expected style by running ``rustfmt`` on an input like:
(行尾的空注释 (trailing empty comment) 可以保持这种格式。不仅如此，当加上
空注释后， ``rustfmt`` 实际上会把导入重新格式化为垂直布局。也就是说，对类似
下面的输入运行 ``rustfmt``，就能轻松把最初的示例重新格式化为期望的风格：)

.. code-block:: rust

	// Do not use this style.
	use crate::{
	    example1,
	    example2::{example3, example4, example5, //
	    },
	    example6, example7,
	    example8::example9, //
	};

The trailing empty comment works for nested imports, as shown above, as well as
for single item imports -- this can be useful to minimize diffs within patch
series:

.. code-block:: rust

	use crate::{
	    example1, //
	};

(行尾空注释既适用于如上所示的嵌套导入，也适用于单条目导入——这有助于把
补丁系列中的 diff 降到最小：)

The trailing empty comment works in any of the lines within the braces, but it
is preferred to keep it in the last item, since it is reminiscent of the
trailing comma in other formatters. Sometimes it may be simpler to avoid moving
the comment several times within a patch series due to changes in the list.
(行尾空注释在花括号内的任何一行都有效，但最好把它放在最后一个条目上，因为
这让人联想到其他格式化工具中的尾随逗号。有时，由于列表的变化，在补丁系列中
避免多次移动该注释可能更简单。)

There may be cases where exceptions may need to be made, i.e. none of this is
a hard rule. There is also code that is not migrated to this style yet, but
please do not introduce code in other styles.
(可能存在需要破例的情况，即这些都不是硬性规则。也还有一些代码尚未迁移到这种
风格，但请不要引入其他风格的代码。)

Eventually, the goal is to get ``rustfmt`` to support this formatting style (or
a similar one) automatically in a stable release without requiring the trailing
empty comment. Thus, at some point, the goal is to remove those comments.
(最终的目标是让 ``rustfmt`` 在稳定版中自动支持这种格式化风格 (或类似的风格)，
而无需行尾空注释。因此，到某个时候，目标是移除这些注释。)


Comments(注释)
--------------

"Normal" comments (i.e. ``//``, rather than code documentation which starts
with ``///`` or ``//!``) are written in Markdown the same way as documentation
comments are, even though they will not be rendered. This improves consistency,
simplifies the rules and allows to move content between the two kinds of
comments more easily. For instance:
("普通" 注释 (即 ``//``，而不是以 ``///`` 或 ``//!`` 开头的代码文档) 与文档
注释一样以 Markdown 书写，即使它们不会被渲染。这提高了一致性，简化了规则，
并使在两类注释之间移动内容更容易。例如：)

.. code-block:: rust

	// `object` is ready to be handled now.
	f(object);

Furthermore, just like documentation, comments are capitalized at the beginning
of a sentence and ended with a period (even if it is a single sentence). This
includes ``// SAFETY:``, ``// TODO:`` and other "tagged" comments, e.g.:
(此外，与文档一样，注释在句首大写，并以句号结尾 (即使只有一个句子)。这包括
``// SAFETY:`` 、 ``// TODO:`` 以及其他 "带标签" 的注释，例如：)

.. code-block:: rust

	// FIXME: The error should be handled properly.

Comments should not be used for documentation purposes: comments are intended
for implementation details, not users. This distinction is useful even if the
reader of the source file is both an implementor and a user of an API. In fact,
sometimes it is useful to use both comments and documentation at the same time.
For instance, for a ``TODO`` list or to comment on the documentation itself.
For the latter case, comments can be inserted in the middle; that is, closer to
the line of documentation to be commented. For any other case, comments are
written after the documentation, e.g.:
(注释不应用于文档目的：注释面向实现细节，而不是面向用户。即使源文件的读者
既是实现者又是 API 的用户，这种区分也很有用。事实上，有时同时使用注释和
文档会很有用。例如，用于 ``TODO`` 列表，或对文档本身进行评论。对于后一种
情况，注释可以插在中间，也就是更靠近要评论的那一行文档。对于其他任何情况，
注释都写在文档之后，例如：)

.. code-block:: rust

	/// Returns a new [`Foo`].
	///
	/// # Examples
	///
	// TODO: Find a better example.
	/// ```
	/// let foo = f(42);
	/// ```
	// FIXME: Use fallible approach.
	pub fn f(x: i32) -> Foo {
	    // ...
	}

This applies to both public and private items. This increases consistency with
public items, allows changes to visibility with less changes involved and will
allow us to potentially generate the documentation for private items as well.
In other words, if documentation is written for a private item, then ``///``
should still be used. For instance:
(这适用于公开和私有条目。这提高了与公开条目的一致性，使可见性变更所涉及的
改动更少，并使我们将来也有可能为私有条目生成文档。换句话说，如果为私有条目
编写了文档，仍然应使用 ``///``。例如：)

.. code-block:: rust

	/// My private function.
	// TODO: ...
	fn f() {}

One special kind of comments are the ``// SAFETY:`` comments. These must appear
before every ``unsafe`` block, and they explain why the code inside the block is
correct/sound, i.e. why it cannot trigger undefined behavior in any case, e.g.:
(有一类特殊的注释是 ``// SAFETY:`` 注释。它们必须出现在每个 ``unsafe`` 块
之前，解释块内的代码为何正确/健全 (sound)，即为何它在任何情况下都不会触发
未定义行为，例如：)

.. code-block:: rust

	// SAFETY: `p` is valid by the safety requirements.
	unsafe { *p = 0; }

``// SAFETY:`` comments are not to be confused with the ``# Safety`` sections
in code documentation. ``# Safety`` sections specify the contract that callers
(for functions) or implementors (for traits) need to abide by. ``// SAFETY:``
comments show why a call (for functions) or implementation (for traits) actually
respects the preconditions stated in a ``# Safety`` section or the language
reference.
( ``// SAFETY:`` 注释不要与代码文档中的 ``# Safety`` 小节混淆。 ``# Safety``
小节规定调用者 (对函数而言) 或实现者 (对 trait 而言) 需要遵守的契约。
``// SAFETY:`` 注释则说明一次调用 (对函数而言) 或一个实现 (对 trait 而言)
为何确实遵守了 ``# Safety`` 小节或语言参考中所述的前置条件。)


Code documentation(代码文档)
----------------------------

Rust kernel code is not documented like C kernel code (i.e. via kernel-doc).
Instead, the usual system for documenting Rust code is used: the ``rustdoc``
tool, which uses Markdown (a lightweight markup language).
( Rust 内核代码不像 C 内核代码那样编写文档 (即通过 kernel-doc)。相反，它使用
通常的 Rust 代码文档系统： ``rustdoc`` 工具，它使用 Markdown (一种轻量级标记
语言)。)

To learn Markdown, there are many guides available out there. For instance,
the one at:
(要学习 Markdown，网上有许多指南可用。例如：)

	https://commonmark.org/help/

This is how a well-documented Rust function may look like:
(一个文档完备的 Rust 函数大致如下：)

.. code-block:: rust

	/// Returns the contained [`Some`] value, consuming the `self` value,
	/// without checking that the value is not [`None`].
	///
	/// # Safety
	///
	/// Calling this method on [`None`] is *[undefined behavior]*.
	///
	/// [undefined behavior]: https://doc.rust-lang.org/reference/behavior-considered-undefined.html
	///
	/// # Examples
	///
	/// ```
	/// let x = Some("air");
	/// assert_eq!(unsafe { x.unwrap_unchecked() }, "air");
	/// ```
	pub unsafe fn unwrap_unchecked(self) -> T {
	    match self {
	        Some(val) => val,

	        // SAFETY: The safety contract must be upheld by the caller.
	        None => unsafe { hint::unreachable_unchecked() },
	    }
	}

This example showcases a few ``rustdoc`` features and some conventions followed
in the kernel:
(这个示例展示了 ``rustdoc`` 的若干特性以及内核中遵循的一些约定：)

- The first paragraph must be a single sentence briefly describing what
  the documented item does. Further explanations must go in extra paragraphs.
  (第一段必须是一句话，简要描述被文档化的条目做什么。进一步的解释必须放在
  额外的段落中。)

- Unsafe functions must document their safety preconditions under
  a ``# Safety`` section.
  (不安全函数必须在 ``# Safety`` 小节中记录其安全性前置条件。)

- While not shown here, if a function may panic, the conditions under which
  that happens must be described under a ``# Panics`` section.
  (虽然这里没有展示，但如果函数可能 panic，则必须在 ``# Panics`` 小节中描述
  其发生条件。)

  Please note that panicking should be very rare and used only with a good
  reason. In almost all cases, a fallible approach should be used, typically
  returning a ``Result``.
  (请注意，panic 应当非常罕见，只在有充分理由时使用。在几乎所有情况下，都
  应采用可失败 (fallible) 的方式，通常即返回 ``Result``。)

- If providing examples of usage would help readers, they must be written in
  a section called ``# Examples``.
  (如果提供用法示例对读者有帮助，必须把它们写在名为 ``# Examples`` 的小节中。)

- Rust items (functions, types, constants...) must be linked appropriately
  (``rustdoc`` will create a link automatically).
  ( Rust 条目 (函数、类型、常量等) 必须恰当地链接 ( ``rustdoc`` 会自动创建
  链接)。)

- Any ``unsafe`` block must be preceded by a ``// SAFETY:`` comment
  describing why the code inside is sound.
  (任何 ``unsafe`` 块之前必须有 ``// SAFETY:`` 注释，说明其中代码为何是
  健全的。)

  While sometimes the reason might look trivial and therefore unneeded,
  writing these comments is not just a good way of documenting what has been
  taken into account, but most importantly, it provides a way to know that
  there are no *extra* implicit constraints.
  (虽然有时理由看起来微不足道因而似乎不需要，但写下这些注释不仅是记录已考虑
  因素的好方法，更重要的是，它提供了一种确认不存在 *额外的* 隐式约束的途径。)

To learn more about how to write documentation for Rust and extra features,
please take a look at the ``rustdoc`` book at:
(要进一步了解如何为 Rust 编写文档以及其他特性，请参阅 ``rustdoc`` 手册：)

	https://doc.rust-lang.org/rustdoc/how-to-write-documentation.html

In addition, the kernel supports creating links relative to the source tree by
prefixing the link destination with ``srctree/``. For instance:
(此外，内核支持创建相对于源码树的链接：在链接目标前加上 ``srctree/`` 前缀。
例如：)

.. code-block:: rust

	//! C header: [`include/linux/printk.h`](srctree/include/linux/printk.h)

or:

.. code-block:: rust

	/// [`struct mutex`]: srctree/include/linux/mutex.h

(或：)


C FFI types(C FFI 类型)
-----------------------

Rust kernel code refers to C types, such as ``int``, using type aliases such as
``c_int``, which are readily available from the ``kernel`` prelude. Please do
not use the aliases from ``core::ffi`` -- they may not map to the correct types.
( Rust 内核代码引用 C 类型 (如 ``int``) 时使用类型别名，如 ``c_int``，这些
别名可以从 ``kernel`` prelude 直接使用。请不要使用 ``core::ffi`` 中的别名
——它们可能无法映射到正确的类型。)

These aliases should generally be referred directly by their identifier, i.e.
as a single segment path. For instance:
(这些别名通常应直接通过其标识符引用，即作为单段路径。例如：)

.. code-block:: rust

	fn f(p: *const c_char) -> c_int {
	    // ...
	}


Naming(命名)
------------

Rust kernel code follows the usual Rust naming conventions:
( Rust 内核代码遵循通常的 Rust 命名约定：)

	https://rust-lang.github.io/api-guidelines/naming.html

When existing C concepts (e.g. macros, functions, objects...) are wrapped into
a Rust abstraction, a name as close as reasonably possible to the C side should
be used in order to avoid confusion and to improve readability when switching
back and forth between the C and Rust sides. For instance, macros such as
``pr_info`` from C are named the same in the Rust side.
(当把现有的 C 概念 (如宏、函数、对象等) 包装为 Rust 抽象时，应使用与 C 侧
尽可能接近的名字，以避免混淆，并在 C 与 Rust 两侧之间来回切换时提高可读性。
例如，C 中的 ``pr_info`` 等宏在 Rust 侧也采用相同的名字。)

Having said that, casing should be adjusted to follow the Rust naming
conventions, and namespacing introduced by modules and types should not be
repeated in the item names. For instance, when wrapping constants like:
(话虽如此，大小写形式应调整为遵循 Rust 命名约定，且模块和类型引入的命名
空间不应在条目名中重复。例如，包装如下常量时：)

.. code-block:: c

	#define GPIO_LINE_DIRECTION_IN	0
	#define GPIO_LINE_DIRECTION_OUT	1

The equivalent in Rust may look like (ignoring documentation):
( Rust 中的等价物可能如下 (忽略文档)：)

.. code-block:: rust

	pub mod gpio {
	    pub enum LineDirection {
	        In = bindings::GPIO_LINE_DIRECTION_IN as _,
	        Out = bindings::GPIO_LINE_DIRECTION_OUT as _,
	    }
	}

That is, the equivalent of ``GPIO_LINE_DIRECTION_IN`` would be referred to as
``gpio::LineDirection::In``. In particular, it should not be named
``gpio::gpio_line_direction::GPIO_LINE_DIRECTION_IN``.
(也就是说， ``GPIO_LINE_DIRECTION_IN`` 的等价物应称为 ``gpio::LineDirection::In`` 。
特别是不应命名为 ``gpio::gpio_line_direction::GPIO_LINE_DIRECTION_IN`` 。)


Lints(Lint 检查)
----------------

In Rust, it is possible to ``allow`` particular warnings (diagnostics, lints)
locally, making the compiler ignore instances of a given warning within a given
function, module, block, etc.
(在 Rust 中，可以在局部 ``allow`` 特定的警告 (诊断、lint)，使编译器忽略给定
函数、模块、块等范围内出现的给定警告。)

It is similar to ``#pragma GCC diagnostic push`` + ``ignored`` + ``pop`` in C
[#]_:
(它类似于 C 中的 ``#pragma GCC diagnostic push`` + ``ignored`` + ``pop`` ：)

.. code-block:: c

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-function"
	static void f(void) {}
	#pragma GCC diagnostic pop

.. [#] In this particular case, the kernel's ``__{always,maybe}_unused``
       attributes (C23's ``[[maybe_unused]]``) may be used; however, the example
       is meant to reflect the equivalent lint in Rust discussed afterwards.
       (在此特定情况下，可以使用内核的 ``__{always,maybe}_unused`` 属性 (C23
       的 ``[[maybe_unused]]`` )；不过，该示例意在反映后文讨论的 Rust 中的
       对应 lint。)

But way less verbose:
(但要简短得多：)

.. code-block:: rust

	#[allow(dead_code)]
	fn f() {}

By that virtue, it makes it possible to comfortably enable more diagnostics by
default (i.e. outside ``W=`` levels). In particular, those that may have some
false positives but that are otherwise quite useful to keep enabled to catch
potential mistakes.
(正因如此，它可以让我们放心地在默认情况下 (即在 ``W=`` 级别之外) 启用更多
诊断。特别是那些可能有一些误报、但保持启用对发现潜在错误相当有用的诊断。)

On top of that, Rust provides the ``expect`` attribute which takes this further.
It makes the compiler warn if the warning was not produced. For instance, the
following will ensure that, when ``f()`` is called somewhere, we will have to
remove the attribute:
(在此基础上，Rust 提供了更进一步 ``expect`` 属性。如果警告没有产生，它会让
编译器发出警告。例如，下面这行将确保：当 ``f()`` 在某处被调用时，我们将不得不
移除该属性：)

.. code-block:: rust

	#[expect(dead_code)]
	fn f() {}

If we do not, we get a warning from the compiler::

	warning: this lint expectation is unfulfilled
	 --> x.rs:3:10
	  |
	3 | #[expect(dead_code)]
	  |          ^^^^^^^^^
	  |
	  = note: `#[warn(unfulfilled_lint_expectations)]` on by default

(如果我们不这么做，就会收到编译器的警告：)

This means that ``expect``\ s do not get forgotten when they are not needed, which
may happen in several situations, e.g.:
(这意味着 ``expect`` 不会在不再需要时被遗忘——这种情况可能在若干场景中发生，
例如：)

- Temporary attributes added while developing.
  (开发期间临时添加的属性。)

- Improvements in lints in the compiler, Clippy or custom tools which may
  remove a false positive.
  (编译器、Clippy 或自定义工具中 lint 的改进可能会消除某个误报。)

- When the lint is not needed anymore because it was expected that it would be
  removed at some point, such as the ``dead_code`` example above.
  (当该 lint 不再需要时——因为它本来就预期会在某个时候被移除，例如上面的
  ``dead_code`` 示例。)

It also increases the visibility of the remaining ``allow``\ s and reduces the
chance of misapplying one.
(它还提高了其余 ``allow`` 的可见性，并降低了误用的几率。)

Thus prefer ``expect`` over ``allow`` unless:
(因此，除非出现以下情况，否则优先使用 ``expect`` 而非 ``allow``：)

- Conditional compilation triggers the warning in some cases but not others.
  (条件编译在某些情况下触发警告而在另一些情况下不触发。)

  If there are only a few cases where the warning triggers (or does not
  trigger) compared to the total number of cases, then one may consider using
  a conditional ``expect`` (i.e. ``cfg_attr(..., expect(...))``). Otherwise,
  it is likely simpler to just use ``allow``.
  (如果相对于总情形数而言，警告触发 (或不触发) 的情况只有少数几种，可以考虑
  使用条件式 ``expect`` (即 ``cfg_attr(..., expect(...))`` )。否则，直接使用
  ``allow`` 可能更简单。)

- Inside macros, when the different invocations may create expanded code that
  triggers the warning in some cases but not in others.
  (在宏内部，不同调用可能展开出在某些情况下触发警告、另一些情况下不触发的
  代码。)

- When code may trigger a warning for some architectures but not others, such
  as an ``as`` cast to a C FFI type.
  (当代码可能在某些架构上触发警告而在其他架构上不会时，例如向 C FFI 类型做
  ``as`` 转换。)

As a more developed example, consider for instance this program:
(举一个更完整的例子，考虑如下程序：)

.. code-block:: rust

	fn g() {}

	fn main() {
	    #[cfg(CONFIG_X)]
	    g();
	}

Here, function ``g()`` is dead code if ``CONFIG_X`` is not set. Can we use
``expect`` here?
(这里，若 ``CONFIG_X`` 未设置，函数 ``g()`` 就是死代码。这里可以用
``expect`` 吗？)

.. code-block:: rust

	#[expect(dead_code)]
	fn g() {}

	fn main() {
	    #[cfg(CONFIG_X)]
	    g();
	}

This would emit a lint if ``CONFIG_X`` is set, since it is not dead code in that
configuration. Therefore, in cases like this, we cannot use ``expect`` as-is.
(如果 ``CONFIG_X`` 被设置，这会产生一条 lint，因为在该配置下它并不是死代码。
因此，在这种情况下我们不能按原样使用 ``expect``。)

A simple possibility is using ``allow``:
(一个简单的办法是使用 ``allow``：)

.. code-block:: rust

	#[allow(dead_code)]
	fn g() {}

	fn main() {
	    #[cfg(CONFIG_X)]
	    g();
	}

An alternative would be using a conditional ``expect``:
(另一种办法是使用条件式 ``expect``：)

.. code-block:: rust

	#[cfg_attr(not(CONFIG_X), expect(dead_code))]
	fn g() {}

	fn main() {
	    #[cfg(CONFIG_X)]
	    g();
	}

This would ensure that, if someone introduces another call to ``g()`` somewhere
(e.g. unconditionally), then it would be spotted that it is not dead code
anymore. However, the ``cfg_attr`` is more complex than a simple ``allow``.
(这可以确保：如果有人在某处又引入了对 ``g()`` 的另一次调用 (例如无条件
调用)，就会发现它不再是死代码。不过， ``cfg_attr`` 比简单的 ``allow`` 更
复杂。)

Therefore, it is likely that it is not worth using conditional ``expect``\ s when
more than one or two configurations are involved or when the lint may be
triggered due to non-local changes (such as ``dead_code``).
(因此，当涉及不止一两个配置，或 lint 可能因非局部改动 (如 ``dead_code``)
而被触发时，使用条件式 ``expect`` 可能不值得。)

For more information about diagnostics in Rust, please see:
(关于 Rust 诊断的更多信息，请参见：)

	https://doc.rust-lang.org/stable/reference/attributes/diagnostics.html

Error handling(错误处理)
------------------------

For some background and guidelines about Rust for Linux specific error handling,
please see:
(关于 Rust for Linux 特有错误处理的背景与指南，请参见：)

	https://rust.docs.kernel.org/kernel/error/type.Result.html#error-codes-in-c-and-rust
