.. SPDX-License-Identifier: GPL-2.0+

=============================================================
DRM RAS over Generic Netlink(基于 Generic Netlink 的 DRM RAS)
=============================================================

The DRM RAS (Reliability, Availability, Serviceability) interface provides a
standardized way for GPU/accelerator drivers to expose error counters and
other reliability nodes to user space via Generic Netlink. This allows
diagnostic tools, monitoring daemons, or test infrastructure to query hardware
health in a uniform way across different DRM drivers.
(DRM RAS(可靠性、可用性、可服务性，Reliability, Availability,
Serviceability)接口为 GPU/加速器驱动提供了一种标准化的方式，通过
Generic Netlink 向用户空间暴露错误计数器和其他可靠性节点。这使得诊断
工具、监控守护进程或测试基础设施能够以统一的方式跨不同 DRM 驱动查询
硬件健康状况。)

Key Goals:
(关键目标：)

* Provide a standardized RAS solution for GPU and accelerator drivers, enabling
  data center monitoring and reliability operations.
  (为 GPU 和加速器驱动提供一个标准化的 RAS 解决方案，支持数据中心
  监控和可靠性操作。)
* Implement a single drm-ras Generic Netlink family to meet modern Netlink YAML
  specifications and centralize all RAS-related communication in one namespace.
  (实现单一的 drm-ras Generic Netlink family，以满足现代 Netlink
  YAML 规范，并把所有与 RAS 相关的通信集中到一个命名空间中。)
* Support a basic error counter interface, addressing the immediate, essential
  monitoring needs.
  (支持一个基本的错误计数器接口，满足当前最紧迫的核心监控需求。)
* Offer a flexible, future-proof interface that can be extended to support
  additional types of RAS data in the future.
  (提供一个灵活、面向未来的接口，以后可以扩展以支持更多类型的 RAS
  数据。)
* Allow multiple nodes per driver, enabling drivers to register separate
  nodes for different IP blocks, sub-blocks, or other logical subdivisions
  as applicable.
  (允许每个驱动有多个节点，使驱动能够为不同的 IP 模块、子模块或其他
  适用的逻辑划分注册单独的节点。)

Nodes(节点)
===========

Nodes are logical abstractions representing an error type or error source within
the device. Currently, only error counter nodes is supported.
(节点是表示设备内某种错误类型或错误来源的逻辑抽象。目前只支持错误
计数器节点。)

Drivers are responsible for registering and unregistering nodes via the
`drm_ras_node_register()` and `drm_ras_node_unregister()` APIs.
(驱动负责通过 drm_ras_node_register() 和 drm_ras_node_unregister()
API 注册和注销节点。)

Node Management(节点管理)
-------------------------

.. kernel-doc:: drivers/gpu/drm/drm_ras.c
   :doc: DRM RAS Node Management
.. kernel-doc:: drivers/gpu/drm/drm_ras.c
   :internal:

Generic Netlink Usage(Generic Netlink 用法)
===========================================

The interface is implemented as a Generic Netlink family named ``drm-ras``.
User space tools can:
(该接口实现为一个名为 ``drm-ras`` 的 Generic Netlink family。用户空间
工具可以：)

* List registered nodes with the ``list-nodes`` command.
  (用 ``list-nodes`` 命令列出已注册的节点。)
* List all error counters in an node with the ``get-error-counter`` command with
  ``node-id`` as a parameter.
  (用带 ``node-id`` 参数的 ``get-error-counter`` 命令列出一个节点中的
  所有错误计数器。)
* Query specific error counter values with the ``get-error-counter`` command, using both
  ``node-id`` and ``error-id`` as parameters.
  (用同时带 ``node-id`` 和 ``error-id`` 参数的 ``get-error-counter``
  命令查询特定的错误计数器值。)

YAML-based Interface(基于 YAML 的接口)
--------------------------------------

The interface is described in a YAML specification ``Documentation/netlink/specs/drm_ras.yaml``
(该接口在 YAML 规范 ``Documentation/netlink/specs/drm_ras.yaml`` 中
描述。)

This YAML is used to auto-generate user space bindings via
``tools/net/ynl/pyynl/ynl_gen_c.py``, and drives the structure of netlink
attributes and operations.
(此 YAML 用于通过 ``tools/net/ynl/pyynl/ynl_gen_c.py`` 自动生成用户
空间绑定，并决定 netlink 属性和操作的结构。)

Usage Notes(使用说明)
---------------------

* User space must first enumerate nodes to obtain their IDs.
  (用户空间必须先枚举节点以获得其 ID。)
* Node IDs or Node names can be used for all further queries, such as error counters.
  (后续的所有查询(如错误计数器)都可以使用节点 ID 或节点名称。)
* Error counters can be queried by either the Error ID or Error name.
  (错误计数器可以通过错误 ID 或错误名称来查询。)
* Query Parameters should be defined as part of the uAPI to ensure user interface stability.
  (查询参数应定义为 uAPI 的一部分，以确保用户界面的稳定性。)
* The interface supports future extension by adding new node types and
  additional attributes.
  (该接口支持通过添加新的节点类型和额外属性来进行未来扩展。)

Example: List nodes using ynl
(示例：使用 ynl 列出节点。)

.. code-block:: bash

    sudo ynl --family drm_ras --dump list-nodes
    [{'device-name': '0000:03:00.0',
    'node-id': 0,
    'node-name': 'correctable-errors',
    'node-type': 'error-counter'},
    {'device-name': '0000:03:00.0',
     'node-id': 1,
     'node-name': 'uncorrectable-errors',
     'node-type': 'error-counter'}]

Example: List all error counters using ynl
(示例：使用 ynl 列出所有错误计数器。)

.. code-block:: bash

    sudo ynl --family drm_ras --dump get-error-counter --json '{"node-id":0}'
    [{'error-id': 1, 'error-name': 'error_name1', 'error-value': 0},
    {'error-id': 2, 'error-name': 'error_name2', 'error-value': 0}]

Example: Query an error counter for a given node
(示例：查询给定节点的某个错误计数器。)

.. code-block:: bash

    sudo ynl --family drm_ras --do get-error-counter --json '{"node-id":0, "error-id":1}'
    {'error-id': 1, 'error-name': 'error_name1', 'error-value': 0}


