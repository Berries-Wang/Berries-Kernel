.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

=============================================
nova NVIDIA GPU drivers(nova NVIDIA GPU 驱动)
=============================================

The nova driver project consists out of two separate drivers nova-core and
nova-drm and intends to supersede the nouveau driver for NVIDIA GPUs based on
the GPU System Processor (GSP).
(nova 驱动项目由两个独立的驱动 nova-core 与 nova-drm 组成，意在取代基于 GPU
系统处理器（GSP）的 NVIDIA GPU 上的 nouveau 驱动。)

The following documents apply to both nova-core and nova-drm.
(以下文档同时适用于 nova-core 与 nova-drm。)

.. toctree::
   :titlesonly:

   guidelines

nova-core(nova-core 核心驱动)
=============================

The nova-core driver is the core driver for NVIDIA GPUs based on GSP. nova-core,
as the 1st level driver, provides an abstraction around the GPUs hard- and
firmware interfaces providing a common base for 2nd level drivers, such as the
vGPU manager VFIO driver and the nova-drm driver.
(nova-core 驱动是基于 GSP 的 NVIDIA GPU 的核心驱动。作为第一级驱动，
nova-core 对 GPU 的硬件与固件接口进行了抽象，为第二级驱动（例如 vGPU 管理
VFIO 驱动与 nova-drm 驱动）提供公共基础。)

.. toctree::
   :titlesonly:

   core/guidelines
   core/todo
   core/vbios
   core/devinit
   core/fwsec
   core/falcon
