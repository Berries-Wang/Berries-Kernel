.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _dmx_fread:

==============================================
Digital TV demux read()(数字电视 demux read())
==============================================

Name(名称)
----------

Digital TV demux read()
(数字电视 demux read()。)

Synopsis(概要)
--------------

.. c:function:: size_t read(int fd, void *buf, size_t count)

Arguments(参数)
---------------

``fd``
  File descriptor returned by a previous call to :c:func:`open()`.
  (由之前调用 :c:func:`open()` 返回的文件描述符。)

 ``buf``
   Buffer to be filled
   (待填充的缓冲区。)

``count``
   Max number of bytes to read
   (要读取的最大字节数。)

Description(描述)
-----------------

This system call returns filtered data, which might be section or Packetized
Elementary Stream (PES) data. The filtered data is transferred from
the driver's internal circular buffer to ``buf``. The maximum amount of data
to be transferred is implied by count.
(该系统调用返回已过滤的数据，可能是 section 或打包基本流 (PES) 数据。
已过滤的数据从驱动程序的内部环形缓冲区传输到 ``buf`` 。要传输的最大
数据量由 count 隐含给出。)

.. note::

   if a section filter created with
   :c:type:`DMX_CHECK_CRC <dmx_sct_filter_params>` flag set,
   data that fails on CRC check will be silently ignored.
   (如果创建 section 过滤器时设置了
   :c:type:`DMX_CHECK_CRC <dmx_sct_filter_params>` 标志，则未通过 CRC
   校验的数据将被静默忽略。)

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
       -  No data to return and ``O_NONBLOCK`` was specified.
          (没有可返回的数据，且指定了 ``O_NONBLOCK`` 。)

    -  -  ``EOVERFLOW``
       -  The filtered data was not read from the buffer in due time,
	  resulting in non-read data being lost. The buffer is flushed.
          (已过滤的数据未在规定时间内从缓冲区读出，导致未读的数据丢
          失。缓冲区被清空。)

    -  -  ``ETIMEDOUT``
       -  The section was not loaded within the stated timeout period.
          See ioctl :ref:`DMX_SET_FILTER` for how to set a timeout.
          (section 未在给定的超时期限内加载完成。设置超时的方法参见
          ioctl :ref:`DMX_SET_FILTER` 。)

    -  -  ``EFAULT``
       -  The driver failed to write to the callers buffer due to an
          invalid \*buf pointer.
          (由于 \*buf 指针无效，驱动程序未能写入调用者的缓冲区。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
