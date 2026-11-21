.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_REQBUFS:

*****************
ioctl DMX_REQBUFS
*****************

Name(名称)
==========

DMX_REQBUFS - Initiate Memory Mapping and/or DMA buffer I/O
(DMX_REQBUFS - 启动内存映射和/或 DMA 缓冲区 I/O。)

.. warning:: this API is still experimental
             (警告：此 API 仍处于实验阶段。)

Synopsis(概要)
==============

.. c:macro:: DMX_REQBUFS

``int ioctl(int fd, DMX_REQBUFS, struct dmx_requestbuffers *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`dmx_requestbuffers`.
    (指向 struct :c:type:`dmx_requestbuffers` 的指针。)

Description(描述)
=================

This ioctl is used to initiate a memory mapped or DMABUF based demux I/O.
(此 ioctl 用于启动基于内存映射或 DMABUF 的 demux I/O。)

Memory mapped buffers are located in device memory and must be allocated
with this ioctl before they can be mapped into the application's address
space. User buffers are allocated by applications themselves, and this
ioctl is merely used to switch the driver into user pointer I/O mode and
to setup some internal structures. Similarly, DMABUF buffers are
allocated by applications through a device driver, and this ioctl only
configures the driver into DMABUF I/O mode without performing any direct
allocation.
(内存映射缓冲区位于设备内存中，必须先用此 ioctl 分配，然后才能映射到
应用程序的地址空间。用户缓冲区由应用程序自己分配，此 ioctl 仅用于把
驱动程序切换到用户指针 I/O 模式并设置一些内部结构。类似地，DMABUF 缓
冲区由应用程序通过某个设备驱动程序分配，此 ioctl 只把驱动程序配置为
DMABUF I/O 模式，而不执行任何直接分配。)

To allocate device buffers applications initialize all fields of the
struct :c:type:`dmx_requestbuffers` structure. They set the  ``count`` field
to the desired number of buffers,  and ``size`` to the size of each
buffer.
(要分配设备缓冲区，应用程序初始化 struct
:c:type:`dmx_requestbuffers` 结构的所有字段。它们把 ``count`` 字段设
置为所需的缓冲区数量，并把 ``size`` 设置为每个缓冲区的大小。)

When the ioctl is called with a pointer to this structure, the driver will
attempt to allocate the requested number of buffers and it stores the actual
number allocated in the ``count`` field. The ``count`` can be smaller than the number requested, even zero, when the driver runs out of free memory. A larger
number is also possible when the driver requires more buffers to
function correctly. The actual allocated buffer size can is returned
at ``size``, and can be smaller than what's requested.
(当以指向此结构的指针调用该 ioctl 时，驱动程序会尝试分配所请求的数量
的缓冲区，并把实际分配的数量存储在 ``count`` 字段中。当驱动程序的空
闲内存耗尽时， ``count`` 可以小于所请求的数量，甚至为零。当驱动程序
需要更多缓冲区才能正常工作时，更大的数量也是可能的。实际分配的缓冲
区大小会在 ``size`` 处返回，并且可能小于所请求的大小。)

When this I/O method is not supported, the ioctl returns an ``EOPNOTSUPP``
error code.
(当不支持此 I/O 方法时，该 ioctl 返回 ``EOPNOTSUPP`` 错误码。)

Applications can call :ref:`DMX_REQBUFS` again to change the number of
buffers, however this cannot succeed when any buffers are still mapped.
A ``count`` value of zero frees all buffers, after aborting or finishing
any DMA in progress.
(应用程序可以再次调用 :ref:`DMX_REQBUFS` 来更改缓冲区数量，但在任何
缓冲区仍被映射时这不会成功。 ``count`` 值为零时，会在中止或完成任何
正在进行的 DMA 之后释放所有缓冲区。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EOPNOTSUPP
    The  the requested I/O method is not supported.
    (所请求的 I/O 方法不受支持。)
