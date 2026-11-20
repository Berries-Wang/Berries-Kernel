.. SPDX-License-Identifier: GPL-2.0

Quick Start(快速上手)
=====================

This document describes how to get started with kernel development in Rust.
(本文描述如何开始用 Rust 进行内核开发。)

There are a few ways to install a Rust toolchain needed for kernel development.
A simple way is to use the packages from your Linux distribution if they are
suitable -- the first section below explains this approach. An advantage of this
approach is that, typically, the distribution will match the LLVM used by Rust
and Clang.
(安装内核开发所需的 Rust 工具链有几种方式。一种简单的方式是使用你的
Linux 发行版提供的软件包 (如果它们合适)——下文第一节解释这种方法。这一
方式的优点是，发行版通常会使 Rust 与 Clang 所用的 LLVM 相匹配。)

Another way is using the prebuilt stable versions of LLVM+Rust provided on
`kernel.org <https://kernel.org/pub/tools/llvm/rust/>`_. These are the same slim
and fast LLVM toolchains from :ref:`Getting LLVM <getting_llvm>` with versions
of Rust added to them that Rust for Linux supports. Two sets are provided: the
"latest LLVM" and "matching LLVM" (please see the link for more information).
(另一种方式是使用 kernel.org 上提供的预构建 stable 版 LLVM+Rust。它们就
是 :ref:`Getting LLVM <getting_llvm>` 中那些精简快速的 LLVM 工具链，只是
加上了 Rust for Linux 所支持版本的 Rust。提供两组："latest LLVM" 和
"matching LLVM" (更多信息请见该链接)。)

Alternatively, the next two "Requirements" sections explain each component and
how to install them through ``rustup``, the standalone installers from Rust
and/or building them.
(或者，接下来两节"要求"部分解释每个组件，以及如何通过 ``rustup``、Rust
的独立安装器和/或自行构建来安装它们。)

The rest of the document explains other aspects on how to get started.
(本文其余部分解释如何上手的其他方面。)


Distributions(发行版)
---------------------

Arch Linux(Arch Linux 发行版)
*****************************

Arch Linux provides recent Rust releases and thus it should generally work out
of the box, e.g.::

	pacman -S rust rust-src rust-bindgen

(Arch Linux 提供较新的 Rust 发行版本，因此通常应当开箱即用，例如：)


Debian(Debian 发行版)
*********************

Debian 13 (Trixie), as well as Testing and Debian Unstable (Sid) provide recent
Rust releases and thus they should generally work out of the box, e.g.::

	apt install rustc rust-src bindgen rustfmt rust-clippy

(Debian 13 (Trixie) 以及 Testing 和 Debian Unstable (Sid) 提供较新的
Rust 发行版本，因此通常应当开箱即用，例如：)


Fedora Linux(Fedora Linux 发行版)
*********************************

Fedora Linux provides recent Rust releases and thus it should generally work out
of the box, e.g.::

	dnf install rust rust-src bindgen-cli rustfmt clippy

(Fedora Linux 提供较新的 Rust 发行版本，因此通常应当开箱即用，例如：)


Gentoo Linux(Gentoo Linux 发行版)
*********************************

Gentoo Linux provides recent Rust releases and thus it should generally work out
of the box, e.g.::

	USE='rust-src rustfmt clippy' emerge dev-lang/rust dev-util/bindgen

(Gentoo Linux 提供较新的 Rust 发行版本，因此通常应当开箱即用，例如：)

``LIBCLANG_PATH`` may need to be set.
(可能需要设置 ``LIBCLANG_PATH``。)


Nix(Nix 发行版)
***************

Nix provides recent Rust releases and thus it should generally work out of the
box, e.g.::

	{ pkgs ? import <nixpkgs> {} }:
	pkgs.mkShell {
	  nativeBuildInputs = with pkgs; [ rustc rust-bindgen rustfmt clippy ];
	  RUST_LIB_SRC = "${pkgs.rust.packages.stable.rustPlatform.rustLibSrc}";
	}

(Nix 提供较新的 Rust 发行版本，因此通常应当开箱即用，例如：)


openSUSE(openSUSE 发行版)
*************************

openSUSE Slowroll and openSUSE Tumbleweed provide recent Rust releases and thus
they should generally work out of the box, e.g.::

	zypper install rust rust-src rust-bindgen clang

(openSUSE Slowroll 和 openSUSE Tumbleweed 提供较新的 Rust 发行版本，因
此通常应当开箱即用，例如：)


Ubuntu(Ubuntu 发行版)
*********************

Ubuntu 25.10 and 26.04 LTS provide recent Rust releases and thus they should
generally work out of the box, e.g.::

	apt install rustc rust-src bindgen rustfmt rust-clippy

(Ubuntu 25.10 和 26.04 LTS 提供较新的 Rust 发行版本，因此通常应当开箱
即用，例如：)

In addition, ``RUST_LIB_SRC`` needs to be set, e.g.::

	RUST_LIB_SRC=/usr/src/rustc-$(rustc --version | cut -d' ' -f2)/library

(此外，需要设置 ``RUST_LIB_SRC``，例如：)

For convenience, ``RUST_LIB_SRC`` can be exported to the global environment.
(为方便起见，``RUST_LIB_SRC`` 可以导出到全局环境。)


24.04 LTS and older(24.04 LTS 及更早版本)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Though Ubuntu 24.04 LTS and older versions still provide recent Rust
releases, they require some additional configuration to be set, using
the versioned packages, e.g.::

	apt install rustc-1.85 rust-1.85-src bindgen-0.71 rustfmt-1.85 \
		rust-1.85-clippy
	ln -s /usr/lib/rust-1.85/bin/rustfmt /usr/bin/rustfmt-1.85
	ln -s /usr/lib/rust-1.85/bin/clippy-driver /usr/bin/clippy-driver-1.85

(虽然 Ubuntu 24.04 LTS 和更早版本仍然提供较新的 Rust 发行版本，但它
们需要使用带版本号的软件包进行一些额外配置，例如：)

None of these packages set their tools as defaults; therefore they should be
specified explicitly, e.g.::

	make LLVM=1 RUSTC=rustc-1.85 RUSTDOC=rustdoc-1.85 RUSTFMT=rustfmt-1.85 \
		CLIPPY_DRIVER=clippy-driver-1.85 BINDGEN=bindgen-0.71

(这些软件包都没有把其工具设为默认；因此需要显式指定，例如：)

Alternatively, modify the ``PATH`` variable to place the Rust 1.85 binaries
first and set ``bindgen`` as the default, e.g.::

	PATH=/usr/lib/rust-1.85/bin:$PATH
	update-alternatives --install /usr/bin/bindgen bindgen \
		/usr/bin/bindgen-0.71 100
	update-alternatives --set bindgen /usr/bin/bindgen-0.71

(另一种做法是修改 ``PATH`` 变量，把 Rust 1.85 的二进制文件放在前面，
并把 ``bindgen`` 设为默认，例如：)

``RUST_LIB_SRC`` may need to be set when using the versioned packages, e.g.::

	RUST_LIB_SRC=/usr/src/rustc-$(rustc-1.85 --version | cut -d' ' -f2)/library

(使用带版本号的软件包时可能需要设置 ``RUST_LIB_SRC``，例如：)

For convenience, ``RUST_LIB_SRC`` can be exported to the global environment.
(为方便起见，``RUST_LIB_SRC`` 可以导出到全局环境。)

In addition, ``bindgen-0.71`` is available in newer releases (24.04 LTS),
but it may not be available in older ones (20.04 LTS and 22.04 LTS),
thus ``bindgen`` may need to be built manually (please see below).
(此外，``bindgen-0.71`` 在较新的版本 (24.04 LTS) 中可用，但在更老的版
本 (20.04 LTS 和 22.04 LTS) 中可能不可用，因此 ``bindgen`` 可能需要手
动构建 (请见下文)。)


Requirements: Building(要求：构建)
----------------------------------

This section explains how to fetch the tools needed for building.
(本节说明如何获取构建所需的工具。)

To easily check whether the requirements are met, the following target
can be used::

	make LLVM=1 rustavailable

(要轻松检查要求是否满足，可以使用以下 target：)

This triggers the same logic used by Kconfig to determine whether
``RUST_IS_AVAILABLE`` should be enabled; but it also explains why not
if that is the case.
(这会触发与 Kconfig 用来判定是否应启用 ``RUST_IS_AVAILABLE`` 相同的逻
辑；但在不满足的情况下，它还会解释原因。)


rustc(Rust 编译器)
*******************

A recent version of the Rust compiler is required.
(需要较新版本的 Rust 编译器。)

If ``rustup`` is being used, enter the kernel build directory (or use
``--path=<build-dir>`` argument to the ``set`` sub-command) and run,
for instance::

	rustup override set stable

(如果使用 ``rustup``，进入内核构建目录 (或给 ``set`` 子命令使用
``--path=<build-dir>`` 参数)，然后运行例如：)

This will configure your working directory to use the given version of
``rustc`` without affecting your default toolchain.
(这会把你的工作目录配置为使用给定版本的 ``rustc``，而不影响你的默认工
具链。)

Note that the override applies to the current working directory (and its
sub-directories).
(注意，这一覆盖只作用于当前工作目录 (及其子目录)。)

If you are not using ``rustup``, fetch a standalone installer from:

	https://forge.rust-lang.org/infra/other-installation-methods.html#standalone

(如果你不使用 ``rustup``，请从以下地址获取独立安装器：)


Rust standard library source(Rust 标准库源码)
*********************************************

The Rust standard library source is required because the build system will
cross-compile ``core``.
(需要 Rust 标准库源码，因为构建系统会交叉编译 ``core``。)

If ``rustup`` is being used, run::

	rustup component add rust-src

(如果使用 ``rustup``，运行：)

The components are installed per toolchain, thus upgrading the Rust compiler
version later on requires re-adding the component.
(组件按工具链安装，因此以后升级 Rust 编译器版本时需要重新添加该组
件。)

Otherwise, if a standalone installer is used, the Rust source tree may be
downloaded into the toolchain's installation folder::

	curl -L "https://static.rust-lang.org/dist/rust-src-$(rustc --version | cut -d' ' -f2).tar.gz" |
		tar -xzf - -C "$(rustc --print sysroot)/lib" \
		"rust-src-$(rustc --version | cut -d' ' -f2)/rust-src/lib/" \
		--strip-components=3

(否则，如果使用独立安装器，可以把 Rust 源码树下载到工具链的安装目录
中：)

In this case, upgrading the Rust compiler version later on requires manually
updating the source tree (this can be done by removing ``$(rustc --print
sysroot)/lib/rustlib/src/rust`` then rerunning the above command).
(在这种情况下，以后升级 Rust 编译器版本时需要手动更新源码树 (可以通过
删除 ``$(rustc --print sysroot)/lib/rustlib/src/rust`` 然后重新运行上述
命令来完成)。)


libclang(LLVM 的一部分)
***********************

``libclang`` (part of LLVM) is used by ``bindgen`` to understand the C code
in the kernel, which means LLVM needs to be installed; like when the kernel
is compiled with ``LLVM=1``.
(``bindgen`` 使用 ``libclang`` (LLVM 的一部分) 来理解内核中的 C 代码，
这意味着需要安装 LLVM；就像内核用 ``LLVM=1`` 编译时那样。)

Linux distributions are likely to have a suitable one available, so it is
best to check that first.
(Linux 发行版很可能已经有合适的版本可用，所以最好先检查这一点。)

There are also some binaries for several systems and architectures uploaded at:

	https://releases.llvm.org/download.html

(在以下地址还上传了一些适用于多种系统和架构的二进制文件：)

Otherwise, building LLVM takes quite a while, but it is not a complex process:

	https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm

(否则，构建 LLVM 需要相当长的时间，但过程并不复杂：)

Please see Documentation/kbuild/llvm.rst for more information and further ways
to fetch pre-built releases and distribution packages.
(更多信息以及获取预构建发行版和发行软件包的其他方式，请参见
Documentation/kbuild/llvm.rst。)


bindgen(绑定生成器)
*******************

The bindings to the C side of the kernel are generated at build time using
the ``bindgen`` tool.
(内核 C 侧的绑定在构建时使用 ``bindgen`` 工具生成。)

Install it, for instance, via (note that this will download and build the tool
from source)::

	cargo install --locked bindgen-cli

(例如，通过以下命令安装 (注意这会从源码下载并构建该工具)：)

``bindgen`` uses the ``clang-sys`` crate to find a suitable ``libclang`` (which
may be linked statically, dynamically or loaded at runtime). By default, the
``cargo`` command above will produce a ``bindgen`` binary that will load
``libclang`` at runtime. If it is not found (or a different ``libclang`` than
the one found should be used), the process can be tweaked, e.g. by using the
``LIBCLANG_PATH`` environment variable. For details, please see ``clang-sys``'s
documentation at:

	https://github.com/KyleMayes/clang-sys#linking

	https://github.com/KyleMayes/clang-sys#environment-variables

(``bindgen`` 使用 ``clang-sys`` crate 来寻找合适的 ``libclang`` (可以静
态链接、动态链接或在运行时加载)。默认情况下，上面的 ``cargo`` 命令会产
生一个在运行时加载 ``libclang`` 的 ``bindgen`` 二进制文件。如果找不到它
(或者应当使用与找到的不同的 ``libclang``)，可以调整这一过程，例如使用
``LIBCLANG_PATH`` 环境变量。详情请见 ``clang-sys`` 的文档：)


Requirements: Developing(要求：开发)
------------------------------------

This section explains how to fetch the tools needed for developing. That is,
they are not needed when just building the kernel.
(本节说明如何获取开发所需的工具。也就是说，仅构建内核时并不需要它
们。)


rustfmt(Rust 代码格式化工具)
*****************************

The ``rustfmt`` tool is used to automatically format all the Rust kernel code,
including the generated C bindings (for details, please see
coding-guidelines.rst).
(``rustfmt`` 工具用于自动格式化所有 Rust 内核代码，包括生成的 C 绑定
(详情请见 coding-guidelines.rst)。)

If ``rustup`` is being used, its ``default`` profile already installs the tool,
thus nothing needs to be done. If another profile is being used, the component
can be installed manually::

	rustup component add rustfmt

(如果使用 ``rustup``，其 ``default`` profile 已经安装了该工具，因此无需
任何操作。如果使用其他 profile，可以手动安装该组件：)

The standalone installers also come with ``rustfmt``.
(独立安装器也自带 ``rustfmt``。)


clippy(Rust 静态检查工具)
**************************

``clippy`` is a Rust linter. Running it provides extra warnings for Rust code.
It can be run by passing ``CLIPPY=1`` to ``make`` (for details, please see
general-information.rst).
(``clippy`` 是一个 Rust linter。运行它会为 Rust 代码提供额外的警告。
可以通过向 ``make`` 传递 ``CLIPPY=1`` 来运行它 (详情请见
general-information.rst)。)

If ``rustup`` is being used, its ``default`` profile already installs the tool,
thus nothing needs to be done. If another profile is being used, the component
can be installed manually::

	rustup component add clippy

(如果使用 ``rustup``，其 ``default`` profile 已经安装了该工具，因此无需
任何操作。如果使用其他 profile，可以手动安装该组件：)

The standalone installers also come with ``clippy``.
(独立安装器也自带 ``clippy``。)


rustdoc(Rust 文档工具)
***********************

``rustdoc`` is the documentation tool for Rust. It generates pretty HTML
documentation for Rust code (for details, please see
general-information.rst).
(``rustdoc`` 是 Rust 的文档工具。它为 Rust 代码生成精美的 HTML 文档
(详情请见 general-information.rst)。)

``rustdoc`` is also used to test the examples provided in documented Rust code
(called doctests or documentation tests). The ``rusttest`` Make target uses
this feature.
(``rustdoc`` 还用于测试文档化的 Rust 代码中提供的示例 (称为 doctests
或文档测试)。``rusttest`` Make target 使用了这一特性。)

If ``rustup`` is being used, all the profiles already install the tool,
thus nothing needs to be done.
(如果使用 ``rustup``，所有 profile 都已经安装了该工具，因此无需任何操
作。)

The standalone installers also come with ``rustdoc``.
(独立安装器也自带 ``rustdoc``。)


rust-analyzer(语言服务器)
**************************

The `rust-analyzer <https://rust-analyzer.github.io/>`_ language server can
be used with many editors to enable syntax highlighting, completion, go to
definition, and other features.
(`rust-analyzer <https://rust-analyzer.github.io/>`_ 语言服务器可以与许
多编辑器配合使用，提供语法高亮、补全、跳转到定义等特性。)

``rust-analyzer`` needs a configuration file, ``rust-project.json``, which
can be generated by the ``rust-analyzer`` Make target::

	make LLVM=1 rust-analyzer

(``rust-analyzer`` 需要一个配置文件 ``rust-project.json``，它可以由
``rust-analyzer`` Make target 生成：)


Configuration(配置)
-------------------

``Rust support`` (``CONFIG_RUST``) needs to be enabled in the ``General setup``
menu. The option is only shown if a suitable Rust toolchain is found (see
above), as long as the other requirements are met. In turn, this will make
visible the rest of options that depend on Rust.
(需要在 ``General setup`` 菜单中启用 ``Rust support`` (``CONFIG_RUST``)。
只有找到了合适的 Rust 工具链 (见上文) 且其他要求也满足时，该选项才会显
示。相应地，这将使其余依赖 Rust 的选项变得可见。)

Afterwards, go to::

	Kernel hacking
	    -> Sample kernel code
	        -> Rust samples

(然后进入：)

And enable some sample modules either as built-in or as loadable.
(并把一些示例模块启用为内建或可加载。)


Building(构建)
--------------

Building a kernel with a complete LLVM toolchain is the best supported setup
at the moment. That is::

	make LLVM=1

(使用完整的 LLVM 工具链构建内核是目前支持最好的配置。即执行：)

Using GCC also works for some configurations, but it is very experimental at
the moment.
(使用 GCC 对某些配置也可行，但目前还非常实验性。)


Hacking(深入探索)
-----------------

To dive deeper, take a look at the source code of the samples
at ``samples/rust/``, the Rust support code under ``rust/`` and
the ``Rust hacking`` menu under ``Kernel hacking``.
(要深入了解，可以查看 ``samples/rust/`` 中示例的源码、``rust/`` 下的
Rust 支持代码，以及 ``Kernel hacking`` 下的 ``Rust hacking`` 菜单。)
