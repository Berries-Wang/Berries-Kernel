===========================
BPF Documentation(BPF 文档)
===========================

This directory contains documentation for the BPF (Berkeley Packet
Filter) facility, with a focus on the extended BPF version (eBPF).
(本目录包含 BPF（Berkeley Packet Filter，伯克利数据包过滤器）设施的文档，
重点是扩展版 BPF（eBPF）。)

This kernel side documentation is still work in progress.
The Cilium project also maintains a `BPF and XDP Reference Guide`_
that goes into great technical depth about the BPF Architecture.
(这份内核侧文档仍在编写中。Cilium 项目还维护了一份
`BPF and XDP Reference Guide`_，对 BPF 体系结构有深入的技术讲解。)

.. toctree::
   :maxdepth: 1

   verifier
   libbpf/index
   standardization/index
   btf
   faq
   syscall_api
   helpers
   kfuncs
   cpumasks
   fs_kfuncs
   programs
   maps
   bpf_prog_run
   classic_vs_extended.rst
   bpf_iterators
   bpf_licensing
   test_debug
   clang-notes
   linux-notes
   other
   redirect

.. Links:
.. _BPF and XDP Reference Guide: https://docs.cilium.io/en/latest/bpf/
