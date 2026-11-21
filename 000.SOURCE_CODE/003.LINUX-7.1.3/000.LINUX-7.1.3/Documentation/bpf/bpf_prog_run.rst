.. SPDX-License-Identifier: GPL-2.0

============================================================
Running BPF programs from userspace(从用户空间运行 BPF 程序)
============================================================

This document describes the ``BPF_PROG_RUN`` facility for running BPF programs
from userspace.
(本文档描述用于从用户空间运行 BPF 程序的 ``BPF_PROG_RUN`` 设施。)

.. contents::
    :local:
    :depth: 2


Overview(概述)
--------------

The ``BPF_PROG_RUN`` command can be used through the ``bpf()`` syscall to
execute a BPF program in the kernel and return the results to userspace. This
can be used to unit test BPF programs against user-supplied context objects, and
as way to explicitly execute programs in the kernel for their side effects. The
command was previously named ``BPF_PROG_TEST_RUN``, and both constants continue
to be defined in the UAPI header, aliased to the same value.
(``BPF_PROG_RUN`` 命令可以通过 ``bpf()`` 系统调用来在内核中执行 BPF 程序并把结果返回给用户空间。它可用于针对用户提供的上下文对象对 BPF 程序进行单元测试，也可作为在内核中为产生副作用而显式执行程序的一种方式。该命令以前命名为 ``BPF_PROG_TEST_RUN`` ，两个常量都继续在 UAPI 头文件中定义，并别名到同一个值。)

The ``BPF_PROG_RUN`` command can be used to execute BPF programs of the
following types:
(``BPF_PROG_RUN`` 命令可用于执行以下类型的 BPF 程序：)

- ``BPF_PROG_TYPE_SOCKET_FILTER``
- ``BPF_PROG_TYPE_SCHED_CLS``
- ``BPF_PROG_TYPE_SCHED_ACT``
- ``BPF_PROG_TYPE_XDP``
- ``BPF_PROG_TYPE_SK_LOOKUP``
- ``BPF_PROG_TYPE_CGROUP_SKB``
- ``BPF_PROG_TYPE_LWT_IN``
- ``BPF_PROG_TYPE_LWT_OUT``
- ``BPF_PROG_TYPE_LWT_XMIT``
- ``BPF_PROG_TYPE_FLOW_DISSECTOR``
- ``BPF_PROG_TYPE_STRUCT_OPS``
- ``BPF_PROG_TYPE_RAW_TRACEPOINT``
- ``BPF_PROG_TYPE_SYSCALL``
- ``BPF_PROG_TYPE_TRACING``
- ``BPF_PROG_TYPE_NETFILTER``

When using the ``BPF_PROG_RUN`` command, userspace supplies an input context
object and (for program types operating on network packets) a buffer containing
the packet data that the BPF program will operate on. The kernel will then
execute the program and return the results to userspace. Note that programs will
not have any side effects while being run in this mode; in particular, packets
will not actually be redirected or dropped, the program return code will just be
returned to userspace. A separate mode for live execution of XDP programs is
provided, documented separately below.
(使用 ``BPF_PROG_RUN`` 命令时，用户空间提供一个输入上下文对象，以及（对于操作网络数据包的程序类型）一个包含 BPF 程序将要处理的数据包数据的缓冲区。然后内核执行该程序并把结果返回给用户空间。注意，程序在此模式下运行时不会有任何副作用；特别是，数据包不会真正被重定向或丢弃，只会把程序返回码返回给用户空间。针对 XDP 程序的实时执行提供了单独的模式，在下面单独说明。)

Running XDP programs in "live frame mode"(以 "live frame mode" 运行 XDP 程序)
-----------------------------------------------------------------------------

The ``BPF_PROG_RUN`` command has a separate mode for running live XDP programs,
which can be used to execute XDP programs in a way where packets will actually
be processed by the kernel after the execution of the XDP program as if they
arrived on a physical interface. This mode is activated by setting the
``BPF_F_TEST_XDP_LIVE_FRAMES`` flag when supplying an XDP program to
``BPF_PROG_RUN``.
(``BPF_PROG_RUN`` 命令有一个单独的模式用于运行实时（live）XDP 程序，它可用于以这样一种方式执行 XDP 程序：在 XDP 程序执行完成后，数据包会被内核真正处理，就像它们到达了一个物理接口。当向 ``BPF_PROG_RUN`` 提供 XDP 程序时，设置 ``BPF_F_TEST_XDP_LIVE_FRAMES`` 标志即可激活此模式。)

The live packet mode is optimised for high performance execution of the supplied
XDP program many times (suitable for, e.g., running as a traffic generator),
which means the semantics are not quite as straight-forward as the regular test
run mode. Specifically:
(实时数据包模式为多次高性能执行所提供的 XDP 程序而优化（例如适合用作流量生成器），这意味着它的语义不像常规测试运行模式那样直截了当。具体来说：)

- When executing an XDP program in live frame mode, the result of the execution
  will not be returned to userspace; instead, the kernel will perform the
  operation indicated by the program's return code (drop the packet, redirect
  it, etc). For this reason, setting the ``data_out`` or ``ctx_out`` attributes
  in the syscall parameters when running in this mode will be rejected. In
  addition, not all failures will be reported back to userspace directly;
  specifically, only fatal errors in setup or during execution (like memory
  allocation errors) will halt execution and return an error. If an error occurs
  in packet processing, like a failure to redirect to a given interface,
  execution will continue with the next repetition; these errors can be detected
  via the same trace points as for regular XDP programs.
  (在实时帧模式下执行 XDP 程序时，执行结果不会返回给用户空间；相反，内核会执行程序的返回码所指示的操作（丢弃数据包、重定向它等）。因此，在此模式下运行时，系统调用参数中设置 ``data_out`` 或 ``ctx_out`` 属性将被拒绝。此外，并非所有失败都会直接报告给用户空间；具体来说，只有设置或执行期间的致命错误（例如内存分配错误）才会中止执行并返回错误。如果错误发生在数据包处理中，例如重定向到给定接口失败，执行将继续进行下一次重复；这些错误可以通过与常规 XDP 程序相同的跟踪点检测。)

- Userspace can supply an ifindex as part of the context object, just like in
  the regular (non-live) mode. The XDP program will be executed as though the
  packet arrived on this interface; i.e., the ``ingress_ifindex`` of the context
  object will point to that interface. Furthermore, if the XDP program returns
  ``XDP_PASS``, the packet will be injected into the kernel networking stack as
  though it arrived on that ifindex, and if it returns ``XDP_TX``, the packet
  will be transmitted *out* of that same interface. Do note, though, that
  because the program execution is not happening in driver context, an
  ``XDP_TX`` is actually turned into the same action as an ``XDP_REDIRECT`` to
  that same interface (i.e., it will only work if the driver has support for the
  ``ndo_xdp_xmit`` driver op).
  (用户空间可以在上下文对象中提供 ifindex，就像在常规（非实时）模式中一样。XDP 程序将像数据包到达该接口一样被执行；即上下文对象的 ``ingress_ifindex`` 将指向该接口。此外，如果 XDP 程序返回 ``XDP_PASS`` ，数据包将被注入内核网络协议栈，就像它到达该 ifindex 一样；如果返回 ``XDP_TX`` ，数据包将 *从* 该同一接口发送出去。不过请注意，由于程序执行并不发生在驱动程序上下文中，``XDP_TX`` 实际上会被转换为与 ``XDP_REDIRECT`` 到同一接口相同的动作（即只有当驱动程序支持 ``ndo_xdp_xmit`` 驱动程序操作时才有效）。)

- When running the program with multiple repetitions, the execution will happen
  in batches. The batch size defaults to 64 packets (which is same as the
  maximum NAPI receive batch size), but can be specified by userspace through
  the ``batch_size`` parameter, up to a maximum of 256 packets. For each batch,
  the kernel executes the XDP program repeatedly, each invocation getting a
  separate copy of the packet data. For each repetition, if the program drops
  the packet, the data page is immediately recycled (see below). Otherwise, the
  packet is buffered until the end of the batch, at which point all packets
  buffered this way during the batch are transmitted at once.
  (以多次重复运行程序时，执行将分批进行。批大小默认为 64 个数据包（与 NAPI 的最大接收批大小相同），但用户空间可以通过 ``batch_size`` 参数指定，最多 256 个数据包。对每个批次，内核重复执行 XDP 程序，每次调用都获得数据包数据的一个单独副本。对于每次重复，如果程序丢弃数据包，数据页会立即回收（见下文）。否则，数据包会被缓冲直到批次结束，届时在批次期间以此方式缓冲的所有数据包会被一次性发送。)

- When setting up the test run, the kernel will initialise a pool of memory
  pages of the same size as the batch size. Each memory page will be initialised
  with the initial packet data supplied by userspace at ``BPF_PROG_RUN``
  invocation. When possible, the pages will be recycled on future program
  invocations, to improve performance. Pages will generally be recycled a full
  batch at a time, except when a packet is dropped (by return code or because
  of, say, a redirection error), in which case that page will be recycled
  immediately. If a packet ends up being passed to the regular networking stack
  (because the XDP program returns ``XDP_PASS``, or because it ends up being
  redirected to an interface that injects it into the stack), the page will be
  released and a new one will be allocated when the pool is empty.
  (设置测试运行时，内核会初始化一个与批大小相同大小的内存页池。每个内存页都会用用户空间在 ``BPF_PROG_RUN`` 调用时提供的初始数据包数据进行初始化。在可能的情况下，页会在后续的程序调用中回收，以提高性能。页通常会一次回收一整个批次，除非某个数据包被丢弃（由于返回码或例如重定向错误），这种情况下该页会立即回收。如果一个数据包最终被传递给常规网络协议栈（因为 XDP 程序返回 ``XDP_PASS`` ，或者因为它最终被重定向到一个将其注入协议栈的接口），该页会被释放，并在池变空时分配一个新页。)

  When recycling, the page content is not rewritten; only the packet boundary
  pointers (``data``, ``data_end`` and ``data_meta``) in the context object will
  be reset to the original values. This means that if a program rewrites the
  packet contents, it has to be prepared to see either the original content or
  the modified version on subsequent invocations.
  (回收时，页内容不会被重写；只有上下文对象中的数据包边界指针（ ``data`` 、 ``data_end`` 和 ``data_meta`` ）会被重置为原始值。这意味着如果程序重写了数据包内容，它必须准备好在后续调用中看到原始内容或修改后的版本。)
