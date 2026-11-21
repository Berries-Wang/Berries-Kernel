.. SPDX-License-Identifier: GPL-2.0

Rust(Rust 语言)
===============

Documentation related to Rust within the kernel. To start using Rust
in the kernel, please read the quick-start.rst guide.
(内核中与 Rust 相关的文档。要开始在内核中使用 Rust，请阅读 quick-start.rst
指南。)


Code documentation(代码文档)
----------------------------

Given a kernel configuration, the kernel may generate Rust code documentation,
i.e. HTML rendered by the ``rustdoc`` tool.
(在给定内核配置的情况下，内核可以生成 Rust 代码文档，即由 ``rustdoc`` 工具
渲染的 HTML。)

.. only:: rustdoc and html

	This kernel documentation was built with `Rust code documentation
	<rustdoc/kernel/index.html>`_.
	(本文档构建时启用了 Rust 代码文档。)

.. only:: not rustdoc and html

	This kernel documentation was not built with Rust code documentation.
	(本文档构建时未启用 Rust 代码文档。)

A pregenerated version is provided at:
(预先生成的版本见：)

	https://rust.docs.kernel.org

Please see the :ref:`Code documentation <rust_code_documentation>` section for
more details.
(更多细节请参阅 :ref:`Code documentation <rust_code_documentation>` 一节。)

.. toctree::
    :maxdepth: 1

    quick-start
    general-information
    coding-guidelines
    arch-support
    testing

You can also find learning materials for Rust in its section in
:doc:`../process/kernel-docs`.
(你还可以在 :doc:`../process/kernel-docs` 的 Rust 一节中找到 Rust 学习资料。)
