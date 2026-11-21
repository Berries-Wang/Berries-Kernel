.. SPDX-License-Identifier: GPL-2.0

=============================
GPU RFC Section(GPU RFC 章节)
=============================

For complex work, especially new uapi, it is often good to nail the high level
design issues before getting lost in the code details. This section is meant to
host such documentation:
(对于复杂的工作，尤其是新的 uapi，最好在深入代码细节之前先敲定高层设计问题。
本章节用于承载此类文档：)

* Each RFC should be a section in this file, explaining the goal and main design
  considerations. Especially for uapi make sure you Cc: all relevant project
  mailing lists and involved people outside of dri-devel.
  (每份 RFC 应是本文件中的一节，说明其目标与主要设计考量。尤其是 uapi，务必
  Cc: 所有相关项目邮件列表以及 dri-devel 之外的相关人员。)

* For uapi structures add a file to this directory with and then pull the
  kerneldoc in like with real uapi headers.
  (对于 uapi 结构，先在本目录中添加一个文件，然后像对待真正的 uapi 头文件
  一样把 kerneldoc 引入。)

* Once the code has landed move all the documentation to the right places in
  the main core, helper or driver sections.
  (代码合入后，把全部文档移到主线 core、helper 或 driver 部分的正确位置。)

.. toctree::

    gpusvm.rst

.. toctree::

    i915_gem_lmem.rst

.. toctree::

    i915_scheduler.rst

.. toctree::

    i915_small_bar.rst

.. toctree::

    i915_vm_bind.rst

.. toctree::
    color_pipeline.rst
