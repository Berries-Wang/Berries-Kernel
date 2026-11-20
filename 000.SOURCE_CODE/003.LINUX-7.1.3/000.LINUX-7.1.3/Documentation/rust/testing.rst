.. SPDX-License-Identifier: GPL-2.0

Testing(测试)
=============

This document contains useful information how to test the Rust code in the
kernel.
(本文档包含如何测试内核中 Rust 代码的有用信息。)

There are three sorts of tests:
(共有三类测试：)

- The KUnit tests.(KUnit 测试。)
- The ``#[test]`` tests.(``#[test]`` 测试。)
- The Kselftests.(Kselftests。)

The KUnit tests(KUnit 测试)
---------------------------

These are the tests that come from the examples in the Rust documentation. They
get transformed into KUnit tests.
(这些测试来自 Rust 文档中的示例。它们会被转换成 KUnit 测试。)

Usage(用法)
***********

These tests can be run via KUnit. For example via ``kunit_tool`` (``kunit.py``)
on the command line::

	./tools/testing/kunit/kunit.py run --make_options LLVM=1 --arch x86_64 --kconfig_add CONFIG_RUST=y

(这些测试可以通过 KUnit 运行。例如在命令行上通过 ``kunit_tool`` (``kunit.py``) 运行。)

Alternatively, KUnit can run them as kernel built-in at boot. Refer to
Documentation/dev-tools/kunit/index.rst for the general KUnit documentation
and Documentation/dev-tools/kunit/architecture.rst for the details of kernel
built-in vs. command line testing.
(另外，KUnit 也可以在内核启动时把它们作为内核内建 (built-in) 测试运行。KUnit 的一般文档见 Documentation/dev-tools/kunit/index.rst，内核内建与命令行测试的细节见 Documentation/dev-tools/kunit/architecture.rst。)

To use these KUnit doctests, the following must be enabled::

	CONFIG_KUNIT
	   Kernel hacking -> Kernel Testing and Coverage -> KUnit - Enable support for unit tests
	CONFIG_RUST_KERNEL_DOCTESTS
	   Kernel hacking -> Rust hacking -> Doctests for the `kernel` crate

(要使用这些 KUnit doctest，必须启用以下选项：)

in the kernel config system.
(位于内核配置系统中。)

KUnit tests are documentation tests(KUnit 测试即文档测试)
*********************************************************

These documentation tests are typically examples of usage of any item (e.g.
function, struct, module...).
(这些文档测试通常是任一条目 (如函数、结构体、模块等) 用法的示例。)

They are very convenient because they are just written alongside the
documentation. For instance:
(它们非常方便，因为只需与文档写在一起。例如：)

.. code-block:: rust

	/// Sums two numbers.
	///
	/// ```
	/// assert_eq!(mymod::f(10, 20), 30);
	/// ```
	pub fn f(a: i32, b: i32) -> i32 {
	    a + b
	}

In userspace, the tests are collected and run via ``rustdoc``. Using the tool
as-is would be useful already, since it allows verifying that examples compile
(thus enforcing they are kept in sync with the code they document) and as well
as running those that do not depend on in-kernel APIs.
(在用户态，这些测试通过 ``rustdoc`` 收集并运行。直接使用该工具已经很有用，因为它可以验证示例能够编译 (从而确保示例与其所文档化的代码保持同步)，还能运行那些不依赖内核内 API 的示例。)

For the kernel, however, these tests get transformed into KUnit test suites.
This means that doctests get compiled as Rust kernel objects, allowing them to
run against a built kernel.
(而对内核而言，这些测试会被转换成 KUnit 测试套件。这意味着 doctest 会被编译为 Rust 内核对象，从而可以针对已构建的内核运行。)

A benefit of this KUnit integration is that Rust doctests get to reuse existing
testing facilities. For instance, the kernel log would look like::

	KTAP version 1
	1..1
	    KTAP version 1
	    # Subtest: rust_doctests_kernel
	    1..59
	    # rust_doctest_kernel_build_assert_rs_0.location: rust/kernel/build_assert.rs:13
	    ok 1 rust_doctest_kernel_build_assert_rs_0
	    # rust_doctest_kernel_build_assert_rs_1.location: rust/kernel/build_assert.rs:56
	    ok 2 rust_doctest_kernel_build_assert_rs_1
	    # rust_doctest_kernel_init_rs_0.location: rust/kernel/init.rs:122
	    ok 3 rust_doctest_kernel_init_rs_0
	    ...
	    # rust_doctest_kernel_types_rs_2.location: rust/kernel/types.rs:150
	    ok 59 rust_doctest_kernel_types_rs_2
	# rust_doctests_kernel: pass:59 fail:0 skip:0 total:59
	# Totals: pass:59 fail:0 skip:0 total:59
	ok 1 rust_doctests_kernel

(这种 KUnit 集成的一个好处是，Rust doctest 可以复用现有的测试设施。例如，内核日志看起来像上面这样。)

Tests using the `? <https://doc.rust-lang.org/reference/expressions/operator-expr.html#the-question-mark-operator>`_
operator are also supported as usual, e.g.:
(使用 `? <https://doc.rust-lang.org/reference/expressions/operator-expr.html#the-question-mark-operator>`_
运算符的测试也照常受支持，例如：)

.. code-block:: rust

	/// ```
	/// # use kernel::{spawn_work_item, workqueue};
	/// spawn_work_item!(workqueue::system(), || pr_info!("x\n"))?;
	/// # Ok::<(), Error>(())
	/// ```

The tests are also compiled with Clippy under ``CLIPPY=1``, just like normal
code, thus also benefitting from extra linting.
(在 ``CLIPPY=1`` 下，这些测试也会像普通代码一样用 Clippy 编译，因此同样能受益于额外的 lint 检查。)

In order for developers to easily see which line of doctest code caused a
failure, a KTAP diagnostic line is printed to the log. This contains the
location (file and line) of the original test (i.e. instead of the location in
the generated Rust file)::

	# rust_doctest_kernel_types_rs_2.location: rust/kernel/types.rs:150

(为了让开发者容易看到是哪一行 doctest 代码导致了失败，日志中会打印一条 KTAP 诊断行。它包含原始测试的位置 (文件与行号) (即而不是生成的 Rust 文件中的位置)。)

Rust tests appear to assert using the usual ``assert!`` and ``assert_eq!``
macros from the Rust standard library (``core``). We provide a custom version
that forwards the call to KUnit instead. Importantly, these macros do not
require passing context, unlike those for KUnit testing (i.e.
``struct kunit *``). This makes them easier to use, and readers of the
documentation do not need to care about which testing framework is used. In
addition, it may allow us to test third-party code more easily in the future.
(Rust 测试看起来是使用 Rust 标准库 (``core``) 中常用的 ``assert!`` 与 ``assert_eq!`` 宏进行断言的。我们提供了自定义版本，把调用转发给 KUnit。重要的是，与 KUnit 测试的宏 (即 ``struct kunit *``) 不同，这些宏不需要传递上下文。这使它们更易使用，文档的读者也无需关心使用的是哪种测试框架。此外，这或许还能让我们将来更容易地测试第三方代码。)

A current limitation is that KUnit does not support assertions in other tasks.
Thus, we presently simply print an error to the kernel log if an assertion
actually failed. Additionally, doctests are not run for nonpublic functions.
(当前的限制是 KUnit 不支持在其他任务中进行断言。因此，如果断言真的失败了，我们目前只是向内核日志打印一条错误。另外，非公开函数不会运行 doctest。)

Since these tests are examples, i.e. they are part of the documentation, they
should generally be written like "real code". Thus, for example, instead of
using ``unwrap()`` or ``expect()``, use the ``?`` operator. For more background,
please see:
(由于这些测试是示例，即它们是文档的一部分，它们通常应当写成"真实代码"的样子。因此，例如不要使用 ``unwrap()`` 或 ``expect()``，而应使用 ``?`` 运算符。更多背景请见：)

	https://rust.docs.kernel.org/kernel/error/type.Result.html#error-codes-in-c-and-rust

The ``#[test]`` tests(``#[test]`` 测试)
---------------------------------------

Additionally, there are the ``#[test]`` tests. Like for documentation tests,
these are also fairly similar to what you would expect from userspace, and they
are also mapped to KUnit.
(此外还有 ``#[test]`` 测试。与文档测试一样，它们也相当接近你在用户态所预期的形式，并且同样被映射到 KUnit。)

These tests are introduced by the ``kunit_tests`` procedural macro, which takes
the name of the test suite as an argument.
(这些测试由 ``kunit_tests`` 过程宏引入，该宏以测试套件的名称作为参数。)

For instance, assume we want to test the function ``f`` from the documentation
tests section. We could write, in the same file where we have our function:
(例如，假设我们想测试文档测试一节中的函数 ``f``。我们可以在拥有该函数的同一文件中写：)

.. code-block:: rust

	#[kunit_tests(rust_kernel_mymod)]
	mod tests {
	    use super::*;

	    #[test]
	    fn test_f() {
	        assert_eq!(f(10, 20), 30);
	    }
	}

And if we run it, the kernel log would look like::

	    KTAP version 1
	    # Subtest: rust_kernel_mymod
	    # speed: normal
	    1..1
	    # test_f.speed: normal
	    ok 1 test_f
	ok 1 rust_kernel_mymod

(如果我们运行它，内核日志看起来像上面这样。)

Like documentation tests, the ``assert!`` and ``assert_eq!`` macros are mapped
back to KUnit and do not panic. Similarly, the
`? <https://doc.rust-lang.org/reference/expressions/operator-expr.html#the-question-mark-operator>`_
operator is supported, i.e. the test functions may return either nothing (i.e.
the unit type ``()``) or ``Result`` (i.e. any ``Result<T, E>``). For instance:
(与文档测试一样，``assert!`` 与 ``assert_eq!`` 宏也被映射回 KUnit 且不会 panic。类似地，也支持
`? <https://doc.rust-lang.org/reference/expressions/operator-expr.html#the-question-mark-operator>`_
运算符，即测试函数可以返回空 (即单元类型 ``()``) 或 ``Result`` (即任意 ``Result<T, E>``)。例如：)

.. code-block:: rust

	#[kunit_tests(rust_kernel_mymod)]
	mod tests {
	    use super::*;

	    #[test]
	    fn test_g() -> Result {
	        let x = g()?;
	        assert_eq!(x, 30);
	        Ok(())
	    }
	}

If we run the test and the call to ``g`` fails, then the kernel log would show::

	    KTAP version 1
	    # Subtest: rust_kernel_mymod
	    # speed: normal
	    1..1
	    # test_g: ASSERTION FAILED at rust/kernel/lib.rs:335
	    Expected is_test_result_ok(test_g()) to be true, but is false
	    # test_g.speed: normal
	    not ok 1 test_g
	not ok 1 rust_kernel_mymod

(如果我们运行该测试且对 ``g`` 的调用失败，内核日志将显示如上。)

If a ``#[test]`` test could be useful as an example for the user, then please
use a documentation test instead. Even edge cases of an API, e.g. error or
boundary cases, can be interesting to show in examples.
(如果某个 ``#[test]`` 测试对用户而言可以作为示例，请改用文档测试。即使是 API 的边缘情况，例如错误或边界情形，放进示例中也可能很有意思。)

The ``rusttest`` host tests(``rusttest`` 宿主机测试)
----------------------------------------------------

These are userspace tests that can be built and run in the host (i.e. the one
that performs the kernel build) using the ``rusttest`` Make target::

	make LLVM=1 rusttest

(这些是用户态测试，可以使用 ``rusttest`` Make 目标在宿主机 (即执行内核构建的机器) 上构建和运行。)

This requires the kernel ``.config``.
(这需要内核 ``.config`` 文件。)

Currently, they are mostly used for testing the ``macros`` crate's examples.
(目前它们主要用于测试 ``macros`` crate 的示例。)

The Kselftests(Kselftests)
--------------------------

Kselftests are also available in the ``tools/testing/selftests/rust`` folder.
(Kselftests 也可在 ``tools/testing/selftests/rust`` 目录中找到。)

The kernel config options required for the tests are listed in the
``tools/testing/selftests/rust/config`` file and can be included with the aid
of the ``merge_config.sh`` script::

	./scripts/kconfig/merge_config.sh .config tools/testing/selftests/rust/config

(这些测试所需的内核配置选项列在 ``tools/testing/selftests/rust/config`` 文件中，可借助 ``merge_config.sh`` 脚本并入。)

The kselftests are built within the kernel source tree and are intended to
be executed on a system that is running the same kernel.
(kselftest 在内核源码树内构建，意图在运行相同内核的系统上执行。)

Once a kernel matching the source tree has been installed and booted, the
tests can be compiled and executed using the following command::

	make TARGETS="rust" kselftest

(一旦安装并启动了与源码树匹配的内核，就可以使用以下命令编译并执行这些测试。)

Refer to Documentation/dev-tools/kselftest.rst for the general Kselftest
documentation.
(Kselftest 的一般文档见 Documentation/dev-tools/kselftest.rst。)
