.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: MC

.. _media_ioc_setup_link:

****************************************
ioctl MEDIA_IOC_SETUP_LINK(修改链接属性)
****************************************

Name(名称)
==========

MEDIA_IOC_SETUP_LINK - Modify the properties of a link(MEDIA_IOC_SETUP_LINK - 修改链接的属性)

Synopsis(概要)
==============

.. c:macro:: MEDIA_IOC_SETUP_LINK

``int ioctl(int fd, MEDIA_IOC_SETUP_LINK, struct media_link_desc *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`media_link_desc`.
    (指向 struct :c:type:`media_link_desc` 的指针。)

Description(描述)
=================

To change link properties applications fill a struct
:c:type:`media_link_desc` with link identification
information (source and sink pad) and the new requested link flags. They
then call the MEDIA_IOC_SETUP_LINK ioctl with a pointer to that
structure.
(要修改链接属性，应用在 struct :c:type:`media_link_desc` 中填入链接标识
信息 (源 pad 和汇 pad) 以及新的请求链接标志，然后调用
MEDIA_IOC_SETUP_LINK ioctl，传入指向该结构的指针。)

The only configurable property is the ``ENABLED`` link flag to
enable/disable a link. Links marked with the ``IMMUTABLE`` link flag can
not be enabled or disabled.
(唯一可配置的属性是用于启用/禁用链接的 ``ENABLED`` 链接标志。标记了
``IMMUTABLE`` 链接标志的链接不能被启用或禁用。)

Link configuration has no side effect on other links. If an enabled link
at the sink pad prevents the link from being enabled, the driver returns
with an ``EBUSY`` error code.
(链接配置对其他链接没有副作用。如果汇 pad 上已启用的链接阻止本链接被
启用，驱动会返回 ``EBUSY`` 错误码。)

Only links marked with the ``DYNAMIC`` link flag can be enabled/disabled
while streaming media data. Attempting to enable or disable a streaming
non-dynamic link will return an ``EBUSY`` error code.
(只有标记了 ``DYNAMIC`` 链接标志的链接才能在流媒体数据传输期间被启用/
禁用。试图启用或禁用正在流式传输的非动态链接会返回 ``EBUSY`` 错误码。)

If the specified link can't be found the driver returns with an ``EINVAL``
error code.
(如果找不到指定的链接，驱动会返回 ``EINVAL`` 错误码。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The struct :c:type:`media_link_desc` references a
    non-existing link, or the link is immutable and an attempt to modify
    its configuration was made.
    (struct :c:type:`media_link_desc` 引用了不存在的链接；或者该链接是
    不可变的，却试图修改其配置。)
