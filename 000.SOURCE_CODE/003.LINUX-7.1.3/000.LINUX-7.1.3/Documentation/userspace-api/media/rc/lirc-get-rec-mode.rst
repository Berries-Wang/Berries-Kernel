.. SPDX-License-Identifier: GPL-2.0 OR GFDL-1.1-no-invariants-or-later
.. c:namespace:: RC

.. _lirc_get_rec_mode:
.. _lirc_set_rec_mode:

**********************************************
ioctls LIRC_GET_REC_MODE and LIRC_SET_REC_MODE
**********************************************

Name(名称)
==========

LIRC_GET_REC_MODE/LIRC_SET_REC_MODE - Get/set current receive mode.
(LIRC_GET_REC_MODE/LIRC_SET_REC_MODE - 获取/设置当前接收模式。)

Synopsis(概要)
==============

.. c:macro:: LIRC_GET_REC_MODE

``int ioctl(int fd, LIRC_GET_REC_MODE, __u32 *mode)``

.. c:macro:: LIRC_SET_REC_MODE

``int ioctl(int fd, LIRC_SET_REC_MODE, __u32 *mode)``

Arguments(参数)
===============

``fd``
    File descriptor returned by open().
    (由 open() 返回的文件描述符。)

``mode``
    Mode used for receive.
    (用于接收的模式。)

Description(描述)
=================

Get and set the current receive mode. Only
:ref:`LIRC_MODE_MODE2 <lirc-mode-mode2>` and
:ref:`LIRC_MODE_SCANCODE <lirc-mode-scancode>` are supported.
Use :ref:`lirc_get_features` to find out which modes the driver supports.
(获取并设置当前接收模式。只支持 :ref:`LIRC_MODE_MODE2
<lirc-mode-mode2>` 与 :ref:`LIRC_MODE_SCANCODE <lirc-mode-scancode>`。
使用 :ref:`lirc_get_features` 查明驱动程序支持哪些模式。)

Return Value(返回值)
====================

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    -  .. row 1

       -  ``ENODEV``

       -  Device not available.
          (设备不可用。)

    -  .. row 2

       -  ``ENOTTY``

       -  Device does not support receiving.
          (设备不支持接收。)

    -  .. row 3

       -  ``EINVAL``

       -  Invalid mode or invalid mode for this device.
          (无效的模式，或对该设备无效的模式。)
