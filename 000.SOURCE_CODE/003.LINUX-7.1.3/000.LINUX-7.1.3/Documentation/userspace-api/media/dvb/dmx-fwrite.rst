.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _dmx_fwrite:

================================================
Digital TV demux write()(数字电视 demux write())
================================================

Name(名称)
----------

Digital TV demux write()
(数字电视 demux write()。)

Synopsis(概要)
--------------

.. c:function:: ssize_t write(int fd, const void *buf, size_t count)

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前调用 :c:func:`open()` 返回的文件描述符。)

``buf``
     Buffer with data to be written
     (包含待写入数据的缓冲区。)

``count``
    Number of bytes at the buffer
    (缓冲区中的字节数。)

Description(描述)
-----------------

This system call is only provided by the logical device
``/dev/dvb/adapter?/dvr?``, associated with the physical demux device that
provides the actual DVR functionality. It is used for replay of a
digitally recorded Transport Stream. Matching filters have to be defined
in the corresponding physical demux device, ``/dev/dvb/adapter?/demux?``.
The amount of data to be transferred is implied by count.
(此系统调用仅由逻辑设备 ``/dev/dvb/adapter?/dvr?`` 提供，它与提供实际
DVR 功能的物理 demux 设备相关联。它用于重放数字录制的传输流
(Transport Stream)。必须在相应的物理 demux 设备
``/dev/dvb/adapter?/demux?`` 中定义匹配的过滤器。要传输的数据量由
count 隐含给出。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并适当设置 ``errno`` 变量。)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  -  ``EWOULDBLOCK``
       -  No data was written. This might happen if ``O_NONBLOCK`` was
	  specified and there is no more buffer space available (if
	  ``O_NONBLOCK`` is not specified the function will block until buffer
	  space is available).
          (没有写入任何数据。如果指定了 ``O_NONBLOCK`` 且没有更多可用
          的缓冲区空间，就可能发生这种情况 (如果未指定 ``O_NONBLOCK`` ，
          函数会阻塞直到缓冲区空间可用)。)

    -  -  ``EBUSY``
       -  This error code indicates that there are conflicting requests. The
	  corresponding demux device is setup to receive data from the
	  front- end. Make sure that these filters are stopped and that the
	  filters with input set to ``DMX_IN_DVR`` are started.
          (此错误码表示存在相互冲突的请求。相应的 demux 设备被设置为从
          前端接收数据。请确保这些过滤器已停止，并且输入设置为
          ``DMX_IN_DVR`` 的过滤器已启动。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
