.. SPDX-License-Identifier: GPL-2.0

==========================
Paravirt_ops(半虚拟化操作)
==========================

Linux provides support for different hypervisor virtualization technologies.
Historically, different binary kernels would be required in order to support
different hypervisors; this restriction was removed with pv_ops.
Linux pv_ops is a virtualization API which enables support for different
hypervisors. It allows each hypervisor to override critical operations and
allows a single kernel binary to run on all supported execution environments
including native machine -- without any hypervisors.
(Linux 支持多种 hypervisor 虚拟化技术。历史上，要支持不同的 hypervisor 需要
不同的二进制内核；这一限制随 pv_ops 而被移除。Linux pv_ops 是一个虚拟化
API，可启用对不同 hypervisor 的支持。它允许每个 hypervisor 覆盖关键操作，并
允许单个内核二进制镜像运行于所有受支持的执行环境，包括没有任何 hypervisor
的原生机器。)

pv_ops provides a set of function pointers which represent operations
corresponding to low-level critical instructions and high-level
functionalities in various areas. pv_ops allows for optimizations at run
time by enabling binary patching of the low-level critical operations
at boot time.
(pv_ops 提供一组函数指针，代表与各领域中低层关键指令和高层功能相对应的
操作。pv_ops 通过在启动时对低层关键操作进行二进制补丁替换，实现运行时
优化。)

pv_ops operations are classified into three categories:
(pv_ops 操作分为三类：)

- simple indirect call
   These operations correspond to high-level functionality where it is
   known that the overhead of indirect call isn't very important.
   (简单间接调用：这些操作对应于已知间接调用开销并不十分重要的高层功能。)

- indirect call which allows optimization with binary patch
   Usually these operations correspond to low-level critical instructions. They
   are called frequently and are performance critical. The overhead is
   very important.
   (可通过二进制补丁优化的间接调用：通常这些操作对应低层关键指令，调用频繁
   且对性能关键，开销非常重要。)

- a set of macros for hand written assembly code
   Hand written assembly codes (.S files) also need paravirtualization
   because they include sensitive instructions or some code paths in
   them are very performance critical.
   (一组用于手写汇编代码的宏：手写汇编代码（.S 文件）也需要半虚拟化，因为
   它们包含敏感指令，或其中某些代码路径对性能非常关键。)
