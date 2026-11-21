.. SPDX-License-Identifier: GPL-2.0

=======================================================================================================
DAMON: Data Access MONitoring and Access-aware System Operations(DAMON：数据访问监控与访问感知系统操作)
=======================================================================================================

DAMON is a Linux kernel subsystem for efficient :ref:`data access monitoring
<damon_design_monitoring>` and :ref:`access-aware system operations
<damon_design_damos>`.  It is designed for being
(DAMON 是一个用于高效 :ref:`data access monitoring
<damon_design_monitoring>`（数据访问监控）与 :ref:`access-aware system
operations <damon_design_damos>`（访问感知系统操作）的 Linux 内核子系统。它
的设计目标是：)

 - *accurate* (for DRAM level memory management),
   (*准确*（accurate）——面向 DRAM 级别的内存管理，)
 - *light-weight* (for production online usages),
   (*轻量*（light-weight）——面向生产环境的在线使用，)
 - *scalable* (in terms of memory size),
   (*可扩展*（scalable）——就内存容量而言，)
 - *tunable* (for flexible usages), and
   (*可调*（tunable）——用法灵活，)
 - *automated* (for production operation without manual tunings).
   (*自动化*（automated）——无需手动调优即可用于生产运维。)

.. toctree::
   :maxdepth: 2

   faq
   design
   api
   maintainer-profile

To utilize and control DAMON from the user-space, please refer to the
administration :doc:`guide </admin-guide/mm/damon/index>`.
(要从用户空间使用和控制 DAMON，请参阅管理 :doc:`guide
</admin-guide/mm/damon/index>`（指南）。)

If you prefer academic papers for reading and citations, please use the papers
from `HPDC'22 <https://dl.acm.org/doi/abs/10.1145/3502181.3531466>`_ and
`Middleware19 Industry <https://dl.acm.org/doi/abs/10.1145/3366626.3368125>`_ .
Note that those cover DAMON implementations in Linux v5.16 and v5.15,
respectively.
(如果你更愿意阅读和引用学术论文，请使用 `HPDC'22
<https://dl.acm.org/doi/abs/10.1145/3502181.3531466>`_ 与 `Middleware19
Industry <https://dl.acm.org/doi/abs/10.1145/3366626.3368125>`_ 的论文。注意
它们分别对应 Linux v5.16 与 v5.15 中的 DAMON 实现。)
