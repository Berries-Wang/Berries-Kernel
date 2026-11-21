.. SPDX-License-Identifier: GPL-2.0

======================
Arch Support(架构支持)
======================

Currently, the Rust compiler (``rustc``) uses LLVM for code generation,
which limits the supported architectures that can be targeted. In addition,
support for building the kernel with LLVM/Clang varies (please see
Documentation/kbuild/llvm.rst). This support is needed for ``bindgen``
which uses ``libclang``.
(目前，Rust 编译器 (``rustc``) 使用 LLVM 进行代码生成，这限制了可作为目标的支持架构。此外，使用 LLVM/Clang 构建内核的支持情况也各不相同 (请参阅 Documentation/kbuild/llvm.rst)。``bindgen`` 需要这种支持，因为它使用 ``libclang`` 。)

Below is a general summary of architectures that currently work. Level of
support corresponds to ``S`` values in the ``MAINTAINERS`` file.
(下面是目前可正常工作的架构的总体概述。支持级别对应 ``MAINTAINERS`` 文件中的 ``S`` 值。)

=============  ================  ==============================================
Architecture   Level of support  Constraints(约束)
=============  ================  ==============================================
``arm``        Maintained        ARMv7 Little Endian only.(仅支持 ARMv7 小端。)
``arm64``      Maintained        Little Endian only.(仅小端。)
``loongarch``  Maintained        \-
``riscv``      Maintained        ``riscv64`` and LLVM/Clang only.
                                 (仅 ``riscv64`` 与 LLVM/Clang。)
``um``         Maintained        \-
``x86``        Maintained        ``x86_64`` only.(仅 ``x86_64``。)
=============  ================  ==============================================
