.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

================
Guidelines(准则)
================

This document describes the general project guidelines that apply to nova-core
and nova-drm.
(本文档描述适用于 nova-core 与 nova-drm 的一般项目准则。)

Language(语言)
==============

The Nova project uses the Rust programming language. In this context, all rules
of the Rust for Linux project as documented in
:doc:`../../rust/general-information` apply. Additionally, the following rules
apply.
(Nova 项目使用 Rust 编程语言。在此背景下， :doc:`../../rust/general-information`
中记载的 Rust for Linux 项目的所有规则均适用。此外，还适用以下规则。)

- Unless technically necessary otherwise (e.g. uAPI), any driver code is written
  in Rust.
  (除非技术上确有必要 (例如 uAPI)，任何驱动程序代码都使用 Rust 编写。)

- Unless technically necessary, unsafe Rust code must be avoided. In case of
  technical necessity, unsafe code should be isolated in a separate component
  providing a safe API for other driver code to use.
  (除非技术上确有必要，必须避免不安全的 Rust 代码。在技术上确有必要
  时，不安全代码应隔离在一个单独的组件中，为其他驱动程序代码提供安全
  的 API。)

Style(风格)
-----------

All rules of the Rust for Linux project as documented in
:doc:`../../rust/coding-guidelines` apply.
( :doc:`../../rust/coding-guidelines` 中记载的 Rust for Linux 项目的所
有规则均适用。)

For a submit checklist, please also see the `Rust for Linux Submit checklist
addendum <https://rust-for-linux.com/contributing#submit-checklist-addendum>`_.
(关于提交核对清单，另请参阅 `Rust for Linux Submit checklist
addendum <https://rust-for-linux.com/contributing#submit-checklist-addendum>`_
。)

Documentation(文档)
===================

The availability of proper documentation is essential in terms of scalability,
accessibility for new contributors and maintainability of a project in general,
but especially for a driver running as complex hardware as Nova is targeting.
(完善的文档对于项目的可扩展性、新贡献者的可及性以及整体可维护性至关
重要，对于像 Nova 所面向的这样运行在复杂硬件上的驱动程序尤其如此。)

Hence, adding documentation of any kind is very much encouraged by the project.
(因此，项目非常鼓励添加任何形式的文档。)

Besides that, there are some minimum requirements.
(除此之外，还有一些最低要求。)

- Every non-private structure needs at least a brief doc comment explaining the
  semantical sense of the structure, as well as potential locking and lifetime
  requirements. It is encouraged to have the same minimum documentation for
  non-trivial private structures.
  (每个非私有结构都至少需要一段简短的文档注释，解释该结构的语义以及
  潜在的加锁与生命周期要求。鼓励对非平凡的私有结构提供同样的最低限
  度文档。)

- uAPIs must be fully documented with kernel-doc comments; additionally, the
  semantical behavior must be explained including potential special or corner
  cases.
  (uAPI 必须用 kernel-doc 注释完整地文档化；此外，还必须解释其语义行
  为，包括潜在的特例或边界情况。)

- The APIs connecting the 1st level driver (nova-core) with 2nd level drivers
  must be fully documented. This includes doc comments, potential locking and
  lifetime requirements, as well as example code if applicable.
  (连接第一级驱动 (nova-core) 与第二级驱动的 API 必须完整文档化。这
  包括文档注释、潜在的加锁与生命周期要求，以及适用时的示例代码。)

- Abbreviations must be explained when introduced; terminology must be uniquely
  defined.
  (缩略语在首次出现时必须加以解释；术语必须有唯一的定义。)

- Register addresses, layouts, shift values and masks must be defined properly;
  unless obvious, the semantical sense must be documented. This only applies if
  the author is able to obtain the corresponding information.
  (寄存器地址、布局、移位值与掩码必须正确定义；除非显而易见，必须记
  录其语义。这仅在作者能够获得相应信息时适用。)

Acceptance Criteria(接受标准)
=============================

- Patches must only be applied if reviewed by at least one other person on the
  mailing list; this also applies for maintainers.
  (补丁只有在邮件列表上至少被另外一人评审过之后才能被应用；对维护者
  同样如此。)
