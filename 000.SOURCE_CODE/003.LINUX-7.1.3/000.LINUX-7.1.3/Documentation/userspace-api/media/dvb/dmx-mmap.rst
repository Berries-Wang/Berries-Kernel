.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _dmx-mmap:

*****************
Digital TV mmap()
*****************

Name(名称)
==========

dmx-mmap - Map device memory into application address space
(dmx-mmap - 将设备内存映射到应用程序地址空间。)

.. warning:: this API is still experimental
   (此 API 仍处于实验阶段。)

Synopsis(概要)
==============

.. code-block:: c

    #include <unistd.h>
    #include <sys/mman.h>

.. c:function:: void *mmap( void *start, size_t length, int prot, int flags, int fd, off_t offset )

Arguments(参数)
===============

``start``
    Map the buffer to this address in the application's address space.
    When the ``MAP_FIXED`` flag is specified, ``start`` must be a
    multiple of the pagesize and mmap will fail when the specified
    address cannot be used. Use of this option is discouraged;
    applications should just specify a ``NULL`` pointer here.
    (把缓冲区映射到应用程序地址空间中的该地址。当指定 ``MAP_FIXED`` 标志时，``start`` 必须是页大小的倍数，且当指定地址无法使用时 mmap 将失败。不鼓励使用该选项；应用程序应在此处直接指定 ``NULL`` 指针。)

``length``
    Length of the memory area to map. This must be a multiple of the
    DVB packet length (188, on most drivers).
    (要映射的内存区域的长度。这必须是 DVB 包长度（大多数驱动程序为 188）的倍数。)

``prot``
    The ``prot`` argument describes the desired memory protection.
    Regardless of the device type and the direction of data exchange it
    should be set to ``PROT_READ`` | ``PROT_WRITE``, permitting read
    and write access to image buffers. Drivers should support at least
    this combination of flags.
    (``prot`` 参数描述所需的内存保护。无论设备类型和数据交换方向如何，都应将其设置为 ``PROT_READ`` | ``PROT_WRITE`` ，以允许对图像缓冲区进行读写访问。驱动程序应至少支持这一标志组合。)

``flags``
    The ``flags`` parameter specifies the type of the mapped object,
    mapping options and whether modifications made to the mapped copy of
    the page are private to the process or are to be shared with other
    references.
    (``flags`` 参数指定映射对象的类型、映射选项，以及是对页面的映射副本所做的修改只对本进程私有，还是要与其他引用共享。)

    ``MAP_FIXED`` requests that the driver selects no other address than
    the one specified. If the specified address cannot be used,
    :c:func:`mmap()` will fail. If ``MAP_FIXED`` is specified,
    ``start`` must be a multiple of the pagesize. Use of this option is
    discouraged.
    (``MAP_FIXED`` 请求驱动程序不选择指定地址以外的任何地址。如果指定地址无法使用，:c:func:`mmap()` 将失败。如果指定了 ``MAP_FIXED`` ，``start`` 必须是页大小的倍数。不鼓励使用该选项。)

    One of the ``MAP_SHARED`` or ``MAP_PRIVATE`` flags must be set.
    ``MAP_SHARED`` allows applications to share the mapped memory with
    other (e. g. child-) processes.
    (必须设置 ``MAP_SHARED`` 或 ``MAP_PRIVATE`` 标志之一。``MAP_SHARED`` 允许应用程序与其他（例如子）进程共享映射的内存。)

    .. note::

       The Linux Digital TV applications should not set the
       ``MAP_PRIVATE``, ``MAP_DENYWRITE``, ``MAP_EXECUTABLE`` or ``MAP_ANON``
       flags.
       (Linux Digital TV 应用程序不应设置 ``MAP_PRIVATE`` 、 ``MAP_DENYWRITE`` 、 ``MAP_EXECUTABLE`` 或 ``MAP_ANON`` 标志。)

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``offset``
    Offset of the buffer in device memory, as returned by
    :ref:`DMX_QUERYBUF` ioctl.
    (缓冲区在设备内存中的偏移，由 :ref:`DMX_QUERYBUF` ioctl 返回。)

Description(描述)
=================

The :c:func:`mmap()` function asks to map ``length`` bytes starting at
``offset`` in the memory of the device specified by ``fd`` into the
application address space, preferably at address ``start``. This latter
address is a hint only, and is usually specified as 0.
(:c:func:`mmap()` 函数请求把 ``fd`` 所指定设备的内存中从 ``offset`` 开始的 ``length`` 字节映射到应用程序地址空间，首选地址为 ``start`` 。后一个地址仅是提示，通常指定为 0。)

Suitable length and offset parameters are queried with the
:ref:`DMX_QUERYBUF` ioctl. Buffers must be allocated with the
:ref:`DMX_REQBUFS` ioctl before they can be queried.
(合适的 length 和 offset 参数通过 :ref:`DMX_QUERYBUF` ioctl 查询。缓冲区必须先用 :ref:`DMX_REQBUFS` ioctl 分配，之后才能查询。)

To unmap buffers the :c:func:`munmap()` function is used.
(解除缓冲区映射使用 :c:func:`munmap()` 函数。)

Return Value(返回值)
====================

On success :c:func:`mmap()` returns a pointer to the mapped buffer. On
error ``MAP_FAILED`` (-1) is returned, and the ``errno`` variable is set
appropriately. Possible error codes are:
(成功时 :c:func:`mmap()` 返回指向映射缓冲区的指针。出错时返回 ``MAP_FAILED`` (-1) ，并适当设置 ``errno`` 变量。可能的错误码有：)

EBADF
    ``fd`` is not a valid file descriptor.
    (``fd`` 不是有效的文件描述符。)

EACCES
    ``fd`` is not open for reading and writing.
    (``fd`` 未以读和写方式打开。)

EINVAL
    The ``start`` or ``length`` or ``offset`` are not suitable. (E. g.
    they are too large, or not aligned on a ``PAGESIZE`` boundary.)
    (``start`` 、 ``length`` 或 ``offset`` 不合适。（例如过大，或未按 ``PAGESIZE`` 边界对齐。))

    The ``flags`` or ``prot`` value is not supported.
    (``flags`` 或 ``prot`` 的值不受支持。)

    No buffers have been allocated with the
    :ref:`DMX_REQBUFS` ioctl.
    (尚未用 :ref:`DMX_REQBUFS` ioctl 分配任何缓冲区。)

ENOMEM
    Not enough physical or virtual memory was available to complete the
    request.
    (没有足够的物理或虚拟内存来完成请求。)
