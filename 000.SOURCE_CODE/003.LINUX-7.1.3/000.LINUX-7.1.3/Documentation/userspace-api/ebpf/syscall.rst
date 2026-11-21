.. SPDX-License-Identifier: GPL-2.0

eBPF Syscall(eBPF 系统调用)
---------------------------

:Authors: - Alexei Starovoitov <ast@kernel.org>
          - Joe Stringer <joe@wand.net.nz>
          - Michael Kerrisk <mtk.manpages@gmail.com>
          (作者: Alexei Starovoitov <ast@kernel.org>、Joe Stringer <joe@wand.net.nz>、Michael Kerrisk <mtk.manpages@gmail.com>。)

The primary info for the bpf syscall is available in the `man-pages`_
for `bpf(2)`_.
(bpf 系统调用的主要信息可在 man-pages 项目中的 bpf(2) 手册页里找到。)

bpf() subcommand reference(bpf() 子命令参考)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: include/uapi/linux/bpf.h
   :doc: eBPF Syscall Preamble

.. kernel-doc:: include/uapi/linux/bpf.h
   :doc: eBPF Syscall Commands

.. Links:
.. _man-pages: https://www.kernel.org/doc/man-pages/
.. _bpf(2): https://man7.org/linux/man-pages/man2/bpf.2.html
