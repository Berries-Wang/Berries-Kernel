.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later

.. _Remote_controllers_table_change:

*******************************************************************
Changing default Remote Controller mappings(更改遥控器默认键映射)
*******************************************************************

The event interface provides two ioctls to be used against the
/dev/input/event device, to allow changing the default keymapping.
(事件接口提供两个针对 /dev/input/event 设备使用的 ioctl，用于更改默认键映射。)

This program demonstrates how to replace the keymap tables.
(本程序演示如何替换键映射表。)


.. toctree::
    :maxdepth: 1

    keytable.c
