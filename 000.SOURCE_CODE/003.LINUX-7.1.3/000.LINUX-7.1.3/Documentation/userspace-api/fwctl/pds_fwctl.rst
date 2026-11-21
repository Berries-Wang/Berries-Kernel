.. SPDX-License-Identifier: GPL-2.0

================================
fwctl pds driver(fwctl pds 驱动)
================================

:Author: Shannon Nelson
(作者: Shannon Nelson)

Overview(概述)
==============

The PDS Core device makes a fwctl service available through an
auxiliary_device named pds_core.fwctl.N.  The pds_fwctl driver binds to
this device and registers itself with the fwctl subsystem.  The resulting
userspace interface is used by an application that is a part of the
AMD Pensando software package for the Distributed Service Card (DSC).
(PDS Core 设备通过名为 pds_core.fwctl.N 的 auxiliary_device 提供 fwctl 服务。
pds_fwctl 驱动绑定该设备并向 fwctl 子系统注册自身。最终得到的用户空间接口由
AMD Pensando 面向分布式服务卡 (DSC) 的软件包中的应用程序使用。)

The pds_fwctl driver has little knowledge of the firmware's internals.
It only knows how to send commands through pds_core's message queue to the
firmware for fwctl requests.  The set of fwctl operations available
depends on the firmware in the DSC, and the userspace application
version must match the firmware so that they can talk to each other.
(pds_fwctl 驱动对固件内部知之甚少。它只知道如何通过 pds_core 的消息队列向
固件发送 fwctl 请求命令。可用 fwctl 操作的集合取决于 DSC 中的固件，用户空间
应用程序版本必须与固件匹配，二者才能相互通信。)

When a connection is created the pds_fwctl driver requests from the
firmware a list of firmware object endpoints, and for each endpoint the
driver requests a list of operations for that endpoint.
(建立连接时，pds_fwctl 驱动会向固件请求固件对象端点列表，并为每个端点请求
该端点的操作列表。)

Each operation description includes a firmware defined command attribute
that maps to the FWCTL scope levels.  The driver translates those firmware
values into the FWCTL scope values which can then be used for filtering the
scoped user requests.
(每个操作描述都包含一个固件定义的命令属性，该属性映射到 FWCTL 作用域级别。
驱动将这些固件值转换为 FWCTL 作用域值，随后可用于过滤带作用域的用户请求。)

pds_fwctl User API(pds_fwctl 用户 API)
======================================

Each RPC request includes the target endpoint and the operation id, and in
and out buffer lengths and pointers.  The driver verifies the existence
of the requested endpoint and operations, then checks the request scope
against the required scope of the operation.  The request is then put
together with the request data and sent through pds_core's message queue
to the firmware, and the results are returned to the caller.
(每个 RPC 请求包含目标端点与操作 id，以及输入/输出缓冲区的长度和指针。驱动
先验证所请求端点和操作是否存在，再对照操作所需作用域检查请求作用域。随后将
请求与请求数据组装在一起，经 pds_core 的消息队列发送给固件，并把结果返回给
调用者。)

The RPC endpoints, operations, and buffer contents are defined by the
particular firmware package in the device, which varies across the
available product configurations.  The details are available in the
specific product SDK documentation.
(RPC 端点、操作和缓冲区内容由设备中特定的固件包定义，因可用的产品配置而异。
细节见具体产品的 SDK 文档。)
