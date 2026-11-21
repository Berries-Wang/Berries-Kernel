.. SPDX-License-Identifier: GPL-2.0-only
.. Copyright (C) 2022 Red Hat, Inc.

================
Redirect(重定向)
================
XDP_REDIRECT(XDP_REDIRECT)
##########################
Supported maps(支持的 map 类型)
-------------------------------

XDP_REDIRECT works with the following map types:
(XDP_REDIRECT 可与以下 map 类型配合工作：)

- ``BPF_MAP_TYPE_DEVMAP``
- ``BPF_MAP_TYPE_DEVMAP_HASH``
- ``BPF_MAP_TYPE_CPUMAP``
- ``BPF_MAP_TYPE_XSKMAP``

For more information on these maps, please see the specific map documentation.
(关于这些 map 的更多信息，请参阅相应 map 的专门文档。)

Process(流程)
-------------

.. kernel-doc:: net/core/filter.c
   :doc: xdp redirect

.. note::
    Not all drivers support transmitting frames after a redirect, and for
    those that do, not all of them support non-linear frames. Non-linear xdp
    bufs/frames are bufs/frames that contain more than one fragment.
    (并非所有驱动程序都支持在重定向之后发送帧；即使支持的驱动，也不是
    全部都支持非线性帧。非线性 xdp buf/frame 是包含多于一个片段
    (fragment) 的 buf/frame。)

Debugging packet drops(调试丢包)
--------------------------------
Silent packet drops for XDP_REDIRECT can be debugged using:
(XDP_REDIRECT 的静默丢包可以使用以下工具调试：)

- bpf_trace
- perf_record

bpf_trace(bpf_trace)
^^^^^^^^^^^^^^^^^^^^
The following bpftrace command can be used to capture and count all XDP tracepoints:
(以下 bpftrace 命令可用于捕获并统计所有 XDP tracepoint：)

.. code-block:: none

    sudo bpftrace -e 'tracepoint:xdp:* { @cnt[probe] = count(); }'
    Attaching 12 probes...
    ^C

    @cnt[tracepoint:xdp:mem_connect]: 18
    @cnt[tracepoint:xdp:mem_disconnect]: 18
    @cnt[tracepoint:xdp:xdp_exception]: 19605
    @cnt[tracepoint:xdp:xdp_devmap_xmit]: 1393604
    @cnt[tracepoint:xdp:xdp_redirect]: 22292200

.. note::
    The various xdp tracepoints can be found in ``source/include/trace/events/xdp.h``
    (各种 xdp tracepoint 可以在 ``source/include/trace/events/xdp.h``
    中找到。)

The following bpftrace command can be used to extract the ``ERRNO`` being returned as
part of the err parameter:
(以下 bpftrace 命令可用于提取作为 err 参数的一部分而返回的
``ERRNO`` ：)

.. code-block:: none

    sudo bpftrace -e \
    'tracepoint:xdp:xdp_redirect*_err {@redir_errno[-args->err] = count();}
    tracepoint:xdp:xdp_devmap_xmit {@devmap_errno[-args->err] = count();}'

perf record(perf record)
^^^^^^^^^^^^^^^^^^^^^^^^
The perf tool also supports recording tracepoints:
(perf 工具也支持记录 tracepoint：)

.. code-block:: none

    perf record -a -e xdp:xdp_redirect_err \
        -e xdp:xdp_redirect_map_err \
        -e xdp:xdp_exception \
        -e xdp:xdp_devmap_xmit

References(参考资料)
====================

- https://github.com/xdp-project/xdp-tutorial/tree/master/tracing02-xdp-monitor
