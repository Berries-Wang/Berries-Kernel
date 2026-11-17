.. include:: <isonum.txt>

===================================
DPAA2 DPIO (Data Path I/O) Overview
===================================

(DPAA2 DPIO(数据路径 I/O)概述)

:Copyright: |copy| 2016-2018 NXP

(版权所有: |copy| 2016-2018 NXP)

This document provides an overview of the Freescale DPAA2 DPIO
drivers
(本文档概述 Freescale DPAA2 DPIO 驱动程序)

Introduction
(简介)
============

A DPAA2 DPIO (Data Path I/O) is a hardware object that provides
interfaces to enqueue and dequeue frames to/from network interfaces
and other accelerators.  A DPIO also provides hardware buffer
pool management for network interfaces.
(DPAA2 DPIO(数据路径 I/O)是一种硬件对象，它提供向/从网络接口和其他加速器入队和
出队帧的接口。DPIO 还为网络接口提供硬件缓冲池管理。)

This document provides an overview the Linux DPIO driver, its
subcomponents, and its APIs.
(本文档概述 Linux DPIO 驱动程序、其子组件及其 API。)

See
Documentation/networking/device_drivers/ethernet/freescale/dpaa2/overview.rst
for a general overview of DPAA2 and the general DPAA2 driver architecture
in Linux.
(有关 DPAA2 以及 Linux 中通用 DPAA2 驱动程序架构的总体概述，请参阅
Documentation/networking/device_drivers/ethernet/freescale/dpaa2/overview.rst。)

Driver Overview
(驱动程序概述)
---------------

The DPIO driver is bound to DPIO objects discovered on the fsl-mc bus and
provides services that:
(DPIO 驱动程序与在 fsl-mc 总线上发现的 DPIO 对象绑定，并提供以下服务：)

  A. allow other drivers, such as the Ethernet driver, to enqueue and dequeue
     frames for their respective objects
     (A. 允许其他驱动程序(例如以太网驱动程序)为其各自的对象入队和出队帧)
  B. allow drivers to register callbacks for data availability notifications
     when data becomes available on a queue or channel
     (B. 允许驱动程序注册数据可用性通知回调，以便在队列或通道上有数据可用时获得
     通知)
  C. allow drivers to manage hardware buffer pools
     (C. 允许驱动程序管理硬件缓冲池)

The Linux DPIO driver consists of 3 primary components--
(Linux DPIO 驱动程序由 3 个主要组件构成——)

   DPIO object driver-- fsl-mc driver that manages the DPIO object
   (DPIO 对象驱动程序——管理 DPIO 对象的 fsl-mc 驱动程序)

   DPIO service-- provides APIs to other Linux drivers for services
   (DPIO 服务——为其他 Linux 驱动程序提供服务 API)

   QBman portal interface-- sends portal commands, gets responses::
   (QBman 门户接口——发送门户命令，获取响应：)

          fsl-mc          other
           bus           drivers
            |               |
        +---+----+   +------+-----+
        |DPIO obj|   |DPIO service|
        | driver |---|  (DPIO)    |
        +--------+   +------+-----+
                            |
                     +------+-----+
                     |    QBman   |
                     | portal i/f |
                     +------------+
                            |
                         hardware


The diagram below shows how the DPIO driver components fit with the other
DPAA2 Linux driver components::
(下图展示了 DPIO 驱动程序组件如何与其他 DPAA2 Linux 驱动程序组件相配合：)

                                                   +------------+
                                                   | OS Network |
                                                   |   Stack    |
                 +------------+                    +------------+
                 | Allocator  |. . . . . . .       |  Ethernet  |
                 |(DPMCP,DPBP)|                    |   (DPNI)   |
                 +-.----------+                    +---+---+----+
                  .          .                         ^   |
                 .            .           <data avail, |   |<enqueue,
                .              .           tx confirm> |   | dequeue>
    +-------------+             .                      |   |
    | DPRC driver |              .    +--------+ +------------+
    |   (DPRC)    |               . . |DPIO obj| |DPIO service|
    +----------+--+                   | driver |-|  (DPIO)    |
               |                      +--------+ +------+-----+
               |<dev add/remove>                 +------|-----+
               |                                 |   QBman    |
          +----+--------------+                  | portal i/f |
          |   MC-bus driver   |                  +------------+
          |                   |                     |
          | /soc/fsl-mc       |                     |
          +-------------------+                     |
                                                    |
 =========================================|=========|========================
                                        +-+--DPIO---|-----------+
                                        |           |           |
                                        |        QBman Portal   |
                                        +-----------------------+

 ============================================================================


DPIO Object Driver (dpio-driver.c)
(DPIO 对象驱动程序(dpio-driver.c))
----------------------------------

   The dpio-driver component registers with the fsl-mc bus to handle objects of
   type "dpio".  The implementation of probe() handles basic initialization
   of the DPIO including mapping of the DPIO regions (the QBman SW portal)
   and initializing interrupts and registering irq handlers.  The dpio-driver
   registers the probed DPIO with dpio-service.
   (dpio-driver 组件向 fsl-mc 总线注册，以处理类型为 "dpio" 的对象。probe() 的实现负责
   DPIO 的基本初始化，包括映射 DPIO 区域(QBman 软件门户)、初始化中断并注册 irq 处理
   函数。dpio-driver 将探测到的 DPIO 注册到 dpio-service。)

DPIO service  (dpio-service.c, dpaa2-io.h)
(DPIO 服务(dpio-service.c、dpaa2-io.h))
------------------------------------------

   The dpio service component provides queuing, notification, and buffers
   management services to DPAA2 drivers, such as the Ethernet driver.  A system
   will typically allocate 1 DPIO object per CPU to allow queuing operations
   to happen simultaneously across all CPUs.
   (dpio service 组件向 DPAA2 驱动程序(例如以太网驱动程序)提供排队、通知和缓冲区管理
   服务。系统通常会为每个 CPU 分配 1 个 DPIO 对象，以允许排队操作在所有 CPU 上同时
   进行。)

   Notification handling
   (通知处理)

      dpaa2_io_service_register()

      dpaa2_io_service_deregister()

      dpaa2_io_service_rearm()

   Queuing
   (排队)

      dpaa2_io_service_pull_fq()

      dpaa2_io_service_pull_channel()

      dpaa2_io_service_enqueue_fq()

      dpaa2_io_service_enqueue_qd()

      dpaa2_io_store_create()

      dpaa2_io_store_destroy()

      dpaa2_io_store_next()

   Buffer pool management
   (缓冲池管理)

      dpaa2_io_service_release()

      dpaa2_io_service_acquire()

QBman portal interface (qbman-portal.c)
(QBman 门户接口(qbman-portal.c))
---------------------------------------

   The qbman-portal component provides APIs to do the low level hardware
   bit twiddling for operations such as:
   (qbman-portal 组件提供用于执行底层硬件位操作的 API，例如：)

      - initializing Qman software portals
        (初始化 Qman 软件门户)
      - building and sending portal commands
        (构建并发送门户命令)
      - portal interrupt configuration and processing
        (门户中断的配置与处理)

   The qbman-portal APIs are not public to other drivers, and are
   only used by dpio-service.
   (qbman-portal API 不对其他驱动程序公开，仅由 dpio-service 使用。)

Other (dpaa2-fd.h, dpaa2-global.h)
(其他(dpaa2-fd.h、dpaa2-global.h))
----------------------------------

   Frame descriptor and scatter-gather definitions and the APIs used to
   manipulate them are defined in dpaa2-fd.h.
   (帧描述符与 scatter-gather 定义以及用于操作它们的 API 定义在 dpaa2-fd.h 中。)

   Dequeue result struct and parsing APIs are defined in dpaa2-global.h.
   (出队结果结构体与解析 API 定义在 dpaa2-global.h 中。)
