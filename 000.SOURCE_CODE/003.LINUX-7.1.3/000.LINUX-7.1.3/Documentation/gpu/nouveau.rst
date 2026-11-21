.. SPDX-License-Identifier: (GPL-2.0+ OR MIT)

===========================================================
 drm/nouveau NVIDIA GPU Driver(drm/nouveau NVIDIA GPU 驱动)
===========================================================

The drm/nouveau driver provides support for a wide range of NVIDIA GPUs,
covering GeForce, Quadro, and Tesla series, from the NV04 architecture up
to the latest Turing, Ampere, Ada families.
(drm/nouveau 驱动为各种 NVIDIA GPU 提供支持，涵盖 GeForce、Quadro 与 Tesla
系列，从 NV04 架构一直到最新的 Turing、Ampere 与 Ada 家族。)

NVKM: NVIDIA Kernel Manager(NVKM：NVIDIA 内核管理器)
====================================================

The NVKM component serves as the core abstraction layer within the nouveau
driver, responsible for managing NVIDIA GPU hardware at the kernel level.
NVKM provides a unified interface for handling various GPU  architectures.
(NVKM 组件充当 nouveau 驱动内的核心抽象层，负责在内核层管理 NVIDIA GPU 硬
件。NVKM 为处理各种 GPU 体系结构提供统一接口。)

It enables resource management, power control, memory handling, and command
submission required for the proper functioning of NVIDIA GPUs under the
nouveau driver.
(它提供 nouveau 驱动之下 NVIDIA GPU 正常运转所需的资源管理、电源控制、内存
处理与命令提交。)

NVKM plays a critical role in abstracting hardware complexities and
providing a consistent API to upper layers of the driver stack.
(NVKM 在抽象硬件复杂性、向驱动栈上层提供一致 API 方面发挥着关键作用。)

GSP Support(GSP 支持)
---------------------

.. kernel-doc:: drivers/gpu/drm/nouveau/nvkm/subdev/gsp/rm/r535/rpc.c
   :doc: GSP message queue element

.. kernel-doc:: drivers/gpu/drm/nouveau/include/nvkm/subdev/gsp.h
   :doc: GSP message handling policy
