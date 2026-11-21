.. SPDX-License-Identifier: GPL-2.0

========================================
Live Update Orchestrator(实时更新编排器)
========================================
:Author: Pasha Tatashin <pasha.tatashin@soleen.com>
(作者：Pasha Tatashin <pasha.tatashin@soleen.com>)

.. kernel-doc:: kernel/liveupdate/luo_core.c
   :doc: Live Update Orchestrator (LUO)

LUO Sessions(LUO 会话)
======================
.. kernel-doc:: kernel/liveupdate/luo_session.c
   :doc: LUO Sessions

LUO Preserving File Descriptors(LUO 保留文件描述符)
===================================================
.. kernel-doc:: kernel/liveupdate/luo_file.c
   :doc: LUO File Descriptors

LUO File Lifecycle Bound Global Data(LUO 文件生命周期绑定的全局数据)
====================================================================
.. kernel-doc:: kernel/liveupdate/luo_flb.c
   :doc: LUO File Lifecycle Bound Global Data

Live Update Orchestrator ABI(Live Update Orchestrator ABI)
==========================================================
.. kernel-doc:: include/linux/kho/abi/luo.h
   :doc: Live Update Orchestrator ABI

The following types of file descriptors can be preserved
(以下类型的文件描述符可以被保留：)

.. toctree::
   :maxdepth: 1

   ../mm/memfd_preservation

Public API(公共 API)
====================
.. kernel-doc:: include/linux/liveupdate.h

.. kernel-doc:: include/linux/kho/abi/luo.h
   :functions:

.. kernel-doc:: kernel/liveupdate/luo_core.c
   :export:

.. kernel-doc:: kernel/liveupdate/luo_flb.c
   :export:

.. kernel-doc:: kernel/liveupdate/luo_file.c
   :export:

Internal API(内部 API)
======================
.. kernel-doc:: kernel/liveupdate/luo_core.c
   :internal:

.. kernel-doc:: kernel/liveupdate/luo_flb.c
   :internal:

.. kernel-doc:: kernel/liveupdate/luo_session.c
   :internal:

.. kernel-doc:: kernel/liveupdate/luo_file.c
   :internal:

See Also(另请参阅)
==================

- :doc:`Live Update uAPI </userspace-api/liveupdate>`
- :doc:`/core-api/kho/index`
