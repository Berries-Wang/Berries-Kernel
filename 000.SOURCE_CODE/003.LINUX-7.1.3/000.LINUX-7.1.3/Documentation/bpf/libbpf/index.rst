.. SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)

.. _libbpf:

=================
libbpf(libbpf 库)
=================

If you are looking to develop BPF applications using the libbpf library, this
directory contains important documentation that you should read.
(如果你想使用 libbpf 库开发 BPF 应用程序，本目录包含你应该阅读的重要文档。)

To get started, it is recommended to begin with the :doc:`libbpf Overview
<libbpf_overview>` document, which provides a high-level understanding of the
libbpf APIs and their usage. This will give you a solid foundation to start
exploring and utilizing the various features of libbpf to develop your BPF
applications.
(入门时建议先阅读 :doc:`libbpf Overview <libbpf_overview>` 文档，它可以帮
助你从较高层面理解 libbpf API 及其用法。这会为你打下坚实的基础，从而开始
探索和利用 libbpf 的各种特性来开发 BPF 应用程序。)

.. toctree::
   :maxdepth: 1

   libbpf_overview
   API Documentation <https://libbpf.readthedocs.io/en/latest/api.html>
   program_types
   libbpf_naming_convention
   libbpf_build


All general BPF questions, including kernel functionality, libbpf APIs and their
application, should be sent to bpf@vger.kernel.org mailing list.  You can
`subscribe <http://vger.kernel.org/vger-lists.html#bpf>`_ to the mailing list
search its `archive <https://lore.kernel.org/bpf/>`_.  Please search the archive
before asking new questions. It may be that this was already addressed or
answered before.
(所有一般性 BPF 问题，包括内核功能、libbpf API 及其应用，都应发送到
bpf@vger.kernel.org 邮件列表。你可以`订阅
<http://vger.kernel.org/vger-lists.html#bpf>`_ 该邮件列表，并检索其`归档
<https://lore.kernel.org/bpf/>`_。提问前请先检索归档——问题可能已经被讨论
或回答过。)
