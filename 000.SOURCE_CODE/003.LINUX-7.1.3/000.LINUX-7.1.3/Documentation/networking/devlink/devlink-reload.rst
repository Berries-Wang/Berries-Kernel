.. SPDX-License-Identifier: GPL-2.0

================================
Devlink Reload(devlink 重新加载)
================================

``devlink-reload`` provides mechanism to reinit driver entities, applying
``devlink-params`` and ``devlink-resources`` new values. It also provides
mechanism to activate firmware.
(``devlink-reload`` 提供重新初始化驱动实体的机制，并应用 ``devlink-params`` 和 ``devlink-resources`` 的新值。它还提供激活固件的机制。)

Reload Actions(重载动作)
========================

User may select a reload action.
By default ``driver_reinit`` action is selected.
(用户可以选择一个重载动作。默认选择 ``driver_reinit`` 动作。)

.. list-table:: Possible reload actions
   :widths: 5 90

   * - Name (名称)
     - Description (描述)
   * - ``driver-reinit``
     - Devlink driver entities re-initialization, including applying
       new values to devlink entities which are used during driver
       load which are:
       (devlink 驱动实体的重新初始化，包括为驱动加载期间使用的 devlink 实体应用新值，这些实体包括：)

       * ``devlink-params`` in configuration mode ``driverinit``
         (配置模式为 ``driverinit`` 的 ``devlink-params``)
       * ``devlink-resources``

       Other devlink entities may stay over the re-initialization:
       (其他 devlink 实体可在重新初始化之后保持不变：)

       * ``devlink-health-reporter``
       * ``devlink-region``

       The rest of the devlink entities have to be removed and readded.
       (其余的 devlink 实体必须先删除再重新添加。)
   * - ``fw_activate``
     - Firmware activate. Activates new firmware if such image is stored and
       pending activation. If no limitation specified this action may involve
       firmware reset. If no new image pending this action will reload current
       firmware image.
       (固件激活。若已存储并有待激活的新固件镜像，则激活新固件。若未指定限制，该动作可能涉及固件复位。若没有待激活的新镜像，该动作将重新加载当前固件镜像。)

Note that even though user asks for a specific action, the driver
implementation might require to perform another action alongside with
it. For example, some driver do not support driver reinitialization
being performed without fw activation. Therefore, the devlink reload
command returns the list of actions which were actrually performed.
(请注意，即使用户请求的是某个特定动作，驱动的实现也可能需要同时执行另一个动作。例如，某些驱动不支持在不进行固件激活的情况下执行驱动重新初始化。因此，devlink reload 命令会返回实际执行的动作列表。)

Reload Limits(重载限制)
=======================

By default reload actions are not limited and driver implementation may
include reset or downtime as needed to perform the actions.
(默认情况下重载动作不受限制，驱动的实现可根据需要包含复位或停机时间来执行这些动作。)

However, some drivers support action limits, which limit the action
implementation to specific constraints.
(不过，某些驱动支持动作限制，即将动作的执行限定在特定约束之内。)

.. list-table:: Possible reload limits
   :widths: 5 90

   * - Name (名称)
     - Description (描述)
   * - ``no_reset``
     - No reset allowed, no down time allowed, no link flap and no
       configuration is lost.
       (不允许复位，不允许停机，链路不翻转 (link flap)，且不丢失任何配置。)

Change Namespace(更改命名空间)
==============================

The netns option allows user to be able to move devlink instances into
namespaces during devlink reload operation.
By default all devlink instances are created in init_net and stay there.
(netns 选项允许用户在 devlink reload 操作期间将 devlink 实例移动到命名空间中。默认情况下，所有 devlink 实例都在 init_net 中创建并停留在其中。)

example usage(用法示例)
-----------------------

.. code:: shell

    $ devlink dev reload help
    $ devlink dev reload DEV [ netns { PID | NAME | ID } ] [ action { driver_reinit | fw_activate } ] [ limit no_reset ]

    # Run reload command for devlink driver entities re-initialization:
    $ devlink dev reload pci/0000:82:00.0 action driver_reinit
    reload_actions_performed:
      driver_reinit

    # Run reload command to activate firmware:
    # Note that mlx5 driver reloads the driver while activating firmware
    $ devlink dev reload pci/0000:82:00.0 action fw_activate
    reload_actions_performed:
      driver_reinit fw_activate
