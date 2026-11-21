.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_REMOVE_BUFS:

************************
ioctl VIDIOC_REMOVE_BUFS
************************

Name(名称)
==========

VIDIOC_REMOVE_BUFS - Removes buffers from a queue
(VIDIOC_REMOVE_BUFS - 从队列中移除缓冲区。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_REMOVE_BUFS

``int ioctl(int fd, VIDIOC_REMOVE_BUFS, struct v4l2_remove_buffers *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_remove_buffers`.
    (指向 struct :c:type:`v4l2_remove_buffers` 的指针。)

Description(描述)
=================

Applications can optionally call the :ref:`VIDIOC_REMOVE_BUFS` ioctl to
remove buffers from a queue.
:ref:`VIDIOC_CREATE_BUFS` ioctl support is mandatory to enable :ref:`VIDIOC_REMOVE_BUFS`.
This ioctl is available if the ``V4L2_BUF_CAP_SUPPORTS_REMOVE_BUFS`` capability
is set on the queue when :c:func:`VIDIOC_REQBUFS` or :c:func:`VIDIOC_CREATE_BUFS`
are invoked.
(应用程序可以选择性地调用 :ref:`VIDIOC_REMOVE_BUFS` ioctl 来从队列中移
除缓冲区。要启用 :ref:`VIDIOC_REMOVE_BUFS`，必须支持
:ref:`VIDIOC_CREATE_BUFS` ioctl。当调用 :c:func:`VIDIOC_REQBUFS` 或
:c:func:`VIDIOC_CREATE_BUFS` 时，如果队列上设置了
``V4L2_BUF_CAP_SUPPORTS_REMOVE_BUFS`` 能力，则此 ioctl 可用。)

.. c:type:: v4l2_remove_buffers

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_remove_buffers
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - The starting buffer index to remove. This field is ignored if count == 0.
        (要移除的起始缓冲区索引。当 count == 0 时此字段被忽略。)
    * - __u32
      - ``count``
      - The number of buffers to be removed with indices 'index' until 'index + count - 1'.
        All buffers in this range must be valid and in DEQUEUED state.
        :ref:`VIDIOC_REMOVE_BUFS` will always check the validity of ``type``, if it is
        invalid it returns ``EINVAL`` error code.
        If count is set to 0 :ref:`VIDIOC_REMOVE_BUFS` will do nothing and return 0.
        (要移除的缓冲区数量，索引从 'index' 直到 'index + count - 1'。
        此范围内的所有缓冲区都必须有效且处于 DEQUEUED 状态。
        :ref:`VIDIOC_REMOVE_BUFS` 总是会检查 ``type`` 的有效性，如果无效
        则返回 ``EINVAL`` 错误码。如果 count 被设为 0，
        :ref:`VIDIOC_REMOVE_BUFS` 将不做任何事情并返回 0。)
    * - __u32
      - ``type``
      - Type of the stream or buffers, this is the same as the struct
	:c:type:`v4l2_format` ``type`` field. See
	:c:type:`v4l2_buf_type` for valid values.
        (流或缓冲区的类型，与 struct :c:type:`v4l2_format` 的 ``type``
        字段相同。有效值参见 :c:type:`v4l2_buf_type`。)
    * - __u32
      - ``reserved``\ [13]
      - A place holder for future extensions. Drivers and applications
	must set the array to zero.
        (为将来扩展保留的占位符。驱动程序和应用程序必须将该数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter. If an error occurs, no
buffers will be freed and one of the error codes below will be returned:
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。如果发生错误，不会
释放任何缓冲区，并将返回下面的某个错误码：)

EBUSY
    File I/O is in progress.
    One or more of the buffers in the range ``index`` to ``index + count - 1`` are not
    in DEQUEUED state.
    (文件 I/O 正在进行中。
    范围 ``index`` 到 ``index + count - 1`` 中的一个或多个缓冲区不处于
    DEQUEUED 状态。)

EINVAL
    One or more of the buffers in the range ``index`` to ``index + count - 1`` do not
    exist in the queue.
    The buffer type (``type`` field) is not valid.
    (范围 ``index`` 到 ``index + count - 1`` 中的一个或多个缓冲区不存在
    于队列中。
    缓冲区类型 ( ``type`` 字段) 无效。)
