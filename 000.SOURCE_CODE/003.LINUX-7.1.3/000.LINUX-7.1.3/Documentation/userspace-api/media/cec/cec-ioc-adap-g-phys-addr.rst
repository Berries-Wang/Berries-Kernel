.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: CEC

.. _CEC_ADAP_PHYS_ADDR:
.. _CEC_ADAP_G_PHYS_ADDR:
.. _CEC_ADAP_S_PHYS_ADDR:

************************************************************************
ioctls CEC_ADAP_G_PHYS_ADDR and CEC_ADAP_S_PHYS_ADDR(获取或设置物理地址)
************************************************************************

Name(名称)
==========

CEC_ADAP_G_PHYS_ADDR, CEC_ADAP_S_PHYS_ADDR - Get or set the physical address(CEC_ADAP_G_PHYS_ADDR, CEC_ADAP_S_PHYS_ADDR - 获取或设置物理地址)

Synopsis(概要)
==============

.. c:macro:: CEC_ADAP_G_PHYS_ADDR

``int ioctl(int fd, CEC_ADAP_G_PHYS_ADDR, __u16 *argp)``

.. c:macro:: CEC_ADAP_S_PHYS_ADDR

``int ioctl(int fd, CEC_ADAP_S_PHYS_ADDR, __u16 *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to the CEC address.
    (指向 CEC 地址的指针。)

Description(描述)
=================

To query the current physical address applications call
:ref:`ioctl CEC_ADAP_G_PHYS_ADDR <CEC_ADAP_G_PHYS_ADDR>` with a pointer to a __u16 where the
driver stores the physical address.
(要查询当前物理地址，应用程序调用 :ref:`ioctl CEC_ADAP_G_PHYS_ADDR <CEC_ADAP_G_PHYS_ADDR>`
并传一个 __u16 指针，驱动程序把物理地址存储在其中。)

To set a new physical address applications store the physical address in
a __u16 and call :ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` with a pointer to
this integer. The :ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` is only available if
``CEC_CAP_PHYS_ADDR`` is set (the ``ENOTTY`` error code will be returned
otherwise). The :ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` can only be called
by a file descriptor in initiator mode (see :ref:`CEC_S_MODE`), if not
the ``EBUSY`` error code will be returned.
(要设置新的物理地址，应用程序把物理地址存储在一个 __u16 中，并调用
:ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` 传这个整数的指针。
只有设置了 ``CEC_CAP_PHYS_ADDR`` 时 :ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>`
才可用(否则返回 ``ENOTTY`` 错误码)。:ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>`
只能由处于 initiator 模式的文件描述符调用(参见 :ref:`CEC_S_MODE`)，
否则返回 ``EBUSY`` 错误码。)

To clear an existing physical address use ``CEC_PHYS_ADDR_INVALID``.
The adapter will go to the unconfigured state.
(要清除现有物理地址，使用 ``CEC_PHYS_ADDR_INVALID``。适配器将进入
未配置状态。)

If logical address types have been defined (see :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`),
then this ioctl will block until all
requested logical addresses have been claimed. If the file descriptor is in non-blocking mode
then it will not wait for the logical addresses to be claimed, instead it just returns 0.
(如果已定义逻辑地址类型(参见 :ref:`ioctl CEC_ADAP_S_LOG_ADDRS <CEC_ADAP_S_LOG_ADDRS>`)，
则此 ioctl 会阻塞，直到所有请求的逻辑地址都被认领。如果文件
描述符处于非阻塞模式，则它不会等待逻辑地址被认领，而是直接返回 0。)

A :ref:`CEC_EVENT_STATE_CHANGE <CEC-EVENT-STATE-CHANGE>` event is sent when the physical address
changes.
(当物理地址变化时，会发送一个 :ref:`CEC_EVENT_STATE_CHANGE <CEC-EVENT-STATE-CHANGE>`
事件。)

The physical address is a 16-bit number where each group of 4 bits
represent a digit of the physical address a.b.c.d where the most
significant 4 bits represent 'a'. The CEC root device (usually the TV)
has address 0.0.0.0. Every device that is hooked up to an input of the
TV has address a.0.0.0 (where 'a' is ≥ 1), devices hooked up to those in
turn have addresses a.b.0.0, etc. So a topology of up to 5 devices deep
is supported. The physical address a device shall use is stored in the
EDID of the sink.
(物理地址是一个 16 位数，其中每组 4 位代表物理地址 a.b.c.d 的一位，
最高 4 位代表 'a'。CEC 根设备(通常是电视)的地址是 0.0.0.0。每个连接
到电视输入的设备地址为 a.0.0.0(其中 'a' ≥ 1)，连接到这些设备的设备
依次为 a.b.0.0 等等。因此支持最多 5 层深度的拓扑。设备应使用的物理
地址存储在接收端(sink)的 EDID 中。)

For example, the EDID for each HDMI input of the TV will have a
different physical address of the form a.0.0.0 that the sources will
read out and use as their physical address.
(例如，电视的每个 HDMI 输入的 EDID 都有一个形如 a.0.0.0 的不同物理
地址，信号源会读出它并用作自己的物理地址。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

The :ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` can return the following
error codes:
(:ref:`ioctl CEC_ADAP_S_PHYS_ADDR <CEC_ADAP_S_PHYS_ADDR>` 可以返回以下
错误码：)

ENOTTY
    The ``CEC_CAP_PHYS_ADDR`` capability wasn't set, so this ioctl is not supported.
    (未设置 ``CEC_CAP_PHYS_ADDR`` 能力，因此不支持此 ioctl。)

EBUSY
    Another filehandle is in exclusive follower or initiator mode, or the filehandle
    is in mode ``CEC_MODE_NO_INITIATOR``.
    (另一个文件句柄处于独占 follower 或 initiator 模式，或该文件句柄
    处于 ``CEC_MODE_NO_INITIATOR`` 模式。)

EINVAL
    The physical address is malformed.
    (物理地址格式不正确。)
