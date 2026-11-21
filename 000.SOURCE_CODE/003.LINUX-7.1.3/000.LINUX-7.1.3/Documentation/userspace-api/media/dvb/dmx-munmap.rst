.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _dmx-munmap:

******************************
DVB munmap()(DVB 解除内存映射)
******************************

Name(名称)
==========

dmx-munmap - Unmap device memory(dmx-munmap - 解除设备内存映射)

.. warning:: This API is still experimental.
    (注意：此 API 仍处于实验阶段。)

Synopsis(概要)
==============

.. code-block:: c

    #include <unistd.h>
    #include <sys/mman.h>

.. c:function:: int munmap( void *start, size_t length )

Arguments(参数)
===============

``start``
    Address of the mapped buffer as returned by the
    :c:func:`mmap()` function.
    (已映射缓冲区的地址，由 :c:func:`mmap()` 函数返回。)

``length``
    Length of the mapped buffer. This must be the same value as given to
    :c:func:`mmap()`.
    (已映射缓冲区的长度。此值必须与传给 :c:func:`mmap()` 的值相同。)

Description(描述)
=================

Unmaps a previously with the :c:func:`mmap()` function mapped
buffer and frees it, if possible.
(解除映射一个之前用 :c:func:`mmap()` 函数映射的缓冲区，并尽可能释放它。)

Return Value(返回值)
====================

On success :c:func:`munmap()` returns 0, on failure -1 and the
``errno`` variable is set appropriately:
(成功时 :c:func:`munmap()` 返回 0；失败时返回 -1，并相应设置 ``errno``
变量：)

EINVAL
    The ``start`` or ``length`` is incorrect, or no buffers have been
    mapped yet.
    (``start`` 或 ``length`` 不正确，或尚未映射任何缓冲区。)
