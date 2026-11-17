.. SPDX-License-Identifier: GPL-2.0

================
Devlink Resource
================

(Devlink 资源)

``devlink`` provides the ability for drivers to register resources, which
can allow administrators to see the device restrictions for a given
resource, as well as how much of the given resource is currently
in use. Additionally, these resources can optionally have configurable size.
This could enable the administrator to limit the number of resources that
are used.
(``devlink`` 为驱动程序提供了注册资源的能力，这可以让管理员查看给定资源的设备
限制，以及该资源当前的使用量。此外，这些资源可以选择具有可配置的大小。这可以使
管理员能够限制所使用的资源数量。)

For example, the ``netdevsim`` driver enables ``/IPv4/fib`` and
``/IPv4/fib-rules`` as resources to limit the number of IPv4 FIB entries and
rules for a given device.
(例如，``netdevsim`` 驱动程序启用 ``/IPv4/fib`` 和 ``/IPv4/fib-rules`` 作为资源，
以限制给定设备的 IPv4 FIB 表项和规则的数量。)

Resource Ids
(资源 ID)
============

Each resource is represented by an id, and contains information about its
current size and related sub resources. To access a sub resource, you
specify the path of the resource. For example ``/IPv4/fib`` is the id for
the ``fib`` sub-resource under the ``IPv4`` resource.
(每个资源由一个 id 表示，并包含其当前大小以及相关子资源的信息。要访问子资源，
需要指定资源的路径。例如，``/IPv4/fib`` 是 ``IPv4`` 资源之下 ``fib`` 子资源的
id。)

Generic Resources
(通用资源)
=================

Generic resources are used to describe resources that can be shared by multiple
device drivers and their description must be added to the following table:
(通用资源用于描述可以由多个设备驱动程序共享的资源，其描述必须添加到下表中：)

.. list-table:: List of Generic Resources
   :widths: 10 90

   * - Name (名称)
     - Description (描述)
   * - ``physical_ports``
     - A limited capacity of physical ports that the switch ASIC can support
       (交换 ASIC 可以支持的物理端口数量上限)

example usage
(使用示例)
-------------

The resources exposed by the driver can be observed, for example:
(驱动程序暴露的资源可以查看，例如：)

.. code:: shell

    $devlink resource show pci/0000:03:00.0
    pci/0000:03:00.0:
      name kvd size 245760 unit entry
        resources:
          name linear size 98304 occ 0 unit entry size_min 0 size_max 147456 size_gran 128
          name hash_double size 60416 unit entry size_min 32768 size_max 180224 size_gran 128
          name hash_single size 87040 unit entry size_min 65536 size_max 212992 size_gran 128

Some resource's size can be changed. Examples:
(某些资源的大小可以更改。示例：)

.. code:: shell

    $devlink resource set pci/0000:03:00.0 path /kvd/hash_single size 73088
    $devlink resource set pci/0000:03:00.0 path /kvd/hash_double size 74368

The changes do not apply immediately, this can be validated by the 'size_new'
attribute, which represents the pending change in size. For example:
(更改不会立即生效，这可以通过 'size_new' 属性来验证，该属性表示待生效的大小
更改。例如：)

.. code:: shell

    $devlink resource show pci/0000:03:00.0
    pci/0000:03:00.0:
      name kvd size 245760 unit entry size_valid false
      resources:
        name linear size 98304 size_new 147456 occ 0 unit entry size_min 0 size_max 147456 size_gran 128
        name hash_double size 60416 unit entry size_min 32768 size_max 180224 size_gran 128
        name hash_single size 87040 unit entry size_min 65536 size_max 212992 size_gran 128

Note that changes in resource size may require a device reload to properly
take effect.
(注意，资源大小的更改可能需要重新加载设备才能正确生效。)

Port-level Resources and Full Dump
(端口级资源与完整转储)
==================================

In addition to device-level resources, ``devlink`` also supports port-level
resources. These resources are associated with a specific devlink port rather
than the device as a whole.
(除了设备级资源之外，``devlink`` 还支持端口级资源。这些资源与特定的 devlink
端口相关联，而不是与整个设备相关联。)

To list resources for all devlink devices and ports:
(要列出所有 devlink 设备和端口的资源：)

.. code:: shell

    $ devlink resource show
    pci/0000:03:00.0:
      name max_local_SFs size 128 unit entry dpipe_tables none
      name max_external_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.0/196608:
      name max_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.0/196609:
      name max_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.1:
      name max_local_SFs size 128 unit entry dpipe_tables none
      name max_external_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.1/196708:
      name max_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.1/196709:
      name max_SFs size 128 unit entry dpipe_tables none

To show resources for a specific port:
(要显示特定端口的资源：)

.. code:: shell

    $ devlink resource show pci/0000:03:00.0/196608
    pci/0000:03:00.0/196608:
      name max_SFs size 128 unit entry dpipe_tables none

Resource Scope Filtering
(资源范围过滤)
========================

When dumping resources for all devices, ``devlink resource show`` accepts
an optional ``scope`` parameter to restrict the response to device-level
resources, port-level resources, or both (the default).
(在转储所有设备的资源时，``devlink resource show`` 接受一个可选的 ``scope``
参数，以把响应限制为设备级资源、端口级资源或两者(默认)。)

To dump only device-level resources across all devices:
(要只转储所有设备的设备级资源：)

.. code:: shell

    $ devlink resource show scope dev
    pci/0000:03:00.0:
      name max_local_SFs size 128 unit entry dpipe_tables none
      name max_external_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.1:
      name max_local_SFs size 128 unit entry dpipe_tables none
      name max_external_SFs size 128 unit entry dpipe_tables none

To dump only port-level resources across all devices:
(要只转储所有设备的端口级资源：)

.. code:: shell

    $ devlink resource show scope port
    pci/0000:03:00.0/196608:
      name max_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.0/196609:
      name max_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.1/196708:
      name max_SFs size 128 unit entry dpipe_tables none
    pci/0000:03:00.1/196709:
      name max_SFs size 128 unit entry dpipe_tables none

Note that port-level resources are read-only.
(注意，端口级资源是只读的。)
