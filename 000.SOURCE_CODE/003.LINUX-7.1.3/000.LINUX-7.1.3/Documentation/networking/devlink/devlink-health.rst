.. SPDX-License-Identifier: GPL-2.0

==============
Devlink Health
==============

(devlink 健康机制)

Background
(背景)
==========

The ``devlink`` health mechanism is targeted for Real Time Alerting, in
order to know when something bad happened to a PCI device.
(``devlink`` 健康机制面向实时告警，以便在 PCI 设备发生故障时及时知晓。)

  * Provide alert debug information.
    (提供告警调试信息。)
  * Self healing.
    (自我修复。)
  * If problem needs vendor support, provide a way to gather all needed
    debugging information.
    (如果问题需要厂商支持，提供收集全部所需调试信息的途径。)

Overview
(概述)
========

The main idea is to unify and centralize driver health reports in the
generic ``devlink`` instance and allow the user to set different
attributes of the health reporting and recovery procedures.
(主要思想是在通用的 ``devlink`` 实例中统一并集中管理驱动的健康报告，并
允许用户设置健康上报与恢复流程的各种不同属性。)

The ``devlink`` health reporter:
Device driver creates a "health reporter" per each error/health type.
Error/Health type can be a known/generic (e.g. PCI error, fw error, rx/tx error)
or unknown (driver specific).
For each registered health reporter a driver can issue error/health reports
asynchronously. All health reports handling is done by ``devlink``.
Device driver can provide specific callbacks for each "health reporter", e.g.:
(``devlink`` 健康报告器：设备驱动为每种错误/健康类型创建一个
"health reporter"。错误/健康类型可以是已知/通用的 (如 PCI 错误、固件错误、
rx/tx 错误)，也可以是未知的 (驱动特有)。对于每个已注册的健康报告器，驱动
都可以异步地发出错误/健康报告。所有健康报告的处理都由 ``devlink`` 完成。
设备驱动可以为每个 "health reporter" 提供特定的回调，例如：)

  * Recovery procedures
    (恢复流程)
  * Diagnostics procedures
    (诊断流程)
  * Object dump procedures
    (对象转储流程)
  * Out Of Box initial parameters
    (开箱即用的初始参数)

Different parts of the driver can register different types of health reporters
with different handlers.
(驱动的不同部分可以注册带有不同处理器的不同类型的健康报告器。)

Actions
(处理动作)
=======

Once an error is reported, devlink health will perform the following actions:
(一旦报告了错误，devlink health 将执行以下动作：)

  * A log is being send to the kernel trace events buffer
    (向内核 trace events 缓冲区发送一条日志)
  * Health status and statistics are being updated for the reporter instance
    (更新该 reporter 实例的健康状态与统计信息)
  * Object dump is being taken and saved at the reporter instance (as long as
    auto-dump is set and there is no other dump which is already stored)
    (执行对象转储并保存在该 reporter 实例中 (前提是设置了 auto-dump，且没有
    其他已存储的转储))
  * Auto recovery attempt is being done. Depends on:
    (尝试自动恢复。取决于：)

    - Auto-recovery configuration
      (自动恢复配置)
    - Grace period (and burst period)  vs. time passed since last recover
      (宽限期 (及突发期) 与距上次恢复所经过时间的对比)

Devlink formatted message
(devlink 格式化消息)
=========================

To handle devlink health diagnose and health dump requests, devlink creates a
formatted message structure ``devlink_fmsg`` and send it to the driver's callback
to fill the data in using the devlink fmsg API.
(为了处理 devlink health diagnose 和 health dump 请求，devlink 会创建格式化
消息结构 ``devlink_fmsg``，并将其发送给驱动的回调，由回调使用 devlink fmsg
API 填充数据。)

Devlink fmsg is a mechanism to pass descriptors between drivers and devlink, in
json-like format. The API allows the driver to add nested attributes such as
object, object pair and value array, in addition to attributes such as name and
value.
(Devlink fmsg 是一种以类 JSON 格式在驱动与 devlink 之间传递描述符的机制。
除 name 和 value 等属性之外，该 API 还允许驱动添加 object、object pair 和
value array 等嵌套属性。)

Driver should use this API to fill the fmsg context in a format which will be
translated by the devlink to the netlink message later. When it needs to send
the data using SKBs to the netlink layer, it fragments the data between
different SKBs. In order to do this fragmentation, it uses virtual nests
attributes, to avoid actual nesting use which cannot be divided between
different SKBs.
(驱动应使用该 API 以特定格式填充 fmsg 上下文，devlink 稍后会把它转换为
netlink 消息。当需要使用 SKB 把数据发送到 netlink 层时，它会把数据分散到
不同的 SKB 中。为了完成这种分片，它使用虚拟嵌套属性，以避免无法在不同
SKB 之间拆分的真实嵌套。)

User Interface
(用户接口)
==============

User can access/change each reporter's parameters and driver specific callbacks
via ``devlink``, e.g per error type (per health reporter):
(用户可以通过 ``devlink`` 访问/修改每个报告器的参数以及驱动特定的回调，
例如按错误类型 (按健康报告器)：)

  * Configure reporter's generic parameters (like: disable/enable auto recovery)
    (配置报告器的通用参数 (如禁用/启用自动恢复))
  * Invoke recovery procedure
    (触发恢复流程)
  * Run diagnostics
    (运行诊断)
  * Object dump
    (对象转储)

.. list-table:: List of devlink health interfaces
   :widths: 10 90

   * - Name (名称)
     - Description (描述)
   * - ``DEVLINK_CMD_HEALTH_REPORTER_GET``
     - Retrieves status and configuration info per DEV and reporter.
       (获取每个 DEV 和报告器的状态与配置信息。)
   * - ``DEVLINK_CMD_HEALTH_REPORTER_SET``
     - Allows reporter-related configuration setting.
       (允许进行报告器相关的配置设置。)
   * - ``DEVLINK_CMD_HEALTH_REPORTER_RECOVER``
     - Triggers reporter's recovery procedure.
       (触发报告器的恢复流程。)
   * - ``DEVLINK_CMD_HEALTH_REPORTER_TEST``
     - Triggers a fake health event on the reporter. The effects of the test
       event in terms of recovery flow should follow closely that of a real
       event.
       (在报告器上触发一个虚假的健康事件。就恢复流程而言，测试事件的效果
       应当与真实事件非常接近。)
   * - ``DEVLINK_CMD_HEALTH_REPORTER_DIAGNOSE``
     - Retrieves current device state related to the reporter.
       (获取与该报告器相关的当前设备状态。)
   * - ``DEVLINK_CMD_HEALTH_REPORTER_DUMP_GET``
     - Retrieves the last stored dump. Devlink health
       saves a single dump. If an dump is not already stored by devlink
       for this reporter, devlink generates a new dump.
       Dump output is defined by the reporter.
       (获取最近一次存储的转储。devlink health 只保存一个转储。如果
       devlink 尚未为该报告器存储过转储，devlink 会生成新的转储。转储
       输出由报告器定义。)
   * - ``DEVLINK_CMD_HEALTH_REPORTER_DUMP_CLEAR``
     - Clears the last saved dump file for the specified reporter.
       (清除指定报告器最近保存的转储文件。)

The following diagram provides a general overview of ``devlink-health``::
(下图给出了 ``devlink-health`` 的总体概览：)

                                                   netlink
                                          +--------------------------+
                                          |                          |
                                          |            +             |
                                          |            |             |
                                          +--------------------------+
                                                       |request for ops
                                                       |(diagnose,
      driver                               devlink     |recover,
                                                       |dump)
    +--------+                            +--------------------------+
    |        |                            |    reporter|             |
    |        |                            |  +---------v----------+  |
    |        |   ops execution            |  |                    |  |
    |     <----------------------------------+                    |  |
    |        |                            |  |                    |  |
    |        |                            |  + ^------------------+  |
    |        |                            |    | request for ops     |
    |        |                            |    | (recover, dump)     |
    |        |                            |    |                     |
    |        |                            |  +-+------------------+  |
    |        |     health report          |  | health handler     |  |
    |        +------------------------------->                    |  |
    |        |                            |  +--------------------+  |
    |        |     health reporter create |                          |
    |        +---------------------------->                          |
    +--------+                            +--------------------------+
