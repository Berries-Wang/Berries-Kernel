.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _dmx_fopen:

==============================================
Digital TV demux open()(数字电视 demux open())
==============================================

Name(名称)
----------

Digital TV demux open()
(数字电视 demux open())

Synopsis(概要)
--------------

.. c:function:: int open(const char *deviceName, int flags)

Arguments(参数)
---------------

``name``
  Name of specific Digital TV demux device.
  (特定数字电视 demux 设备的名称。)

``flags``
  A bit-wise OR of the following flags:
  (以下标志的按位或：)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -
       - ``O_RDONLY``
       - read-only access
         (只读访问)

    -
       - ``O_RDWR``
       - read/write access
         (读/写访问)

    -
       - ``O_NONBLOCK``
       - open in non-blocking mode
         (blocking mode is the default)
         (以非阻塞模式打开
         (阻塞模式是默认值)。)

Description(描述)
-----------------

This system call, used with a device name of ``/dev/dvb/adapter?/demux?``,
allocates a new filter and returns a handle which can be used for
subsequent control of that filter. This call has to be made for each
filter to be used, i.e. every returned file descriptor is a reference to
a single filter. ``/dev/dvb/adapter?/dvr?`` is a logical device to be used
for retrieving Transport Streams for digital video recording. When
reading from this device a transport stream containing the packets from
all PES filters set in the corresponding demux device
(``/dev/dvb/adapter?/demux?``) having the output set to ``DMX_OUT_TS_TAP``.
A recorded Transport Stream is replayed by writing to this device.
(此系统调用与设备名 ``/dev/dvb/adapter?/demux?`` 一起使用时，会分配
一个新过滤器并返回一个句柄，可用于对该过滤器的后续控制。要使用的每个
过滤器都必须调用一次此调用，即每个返回的文件描述符都是对单个过滤器的
引用。``/dev/dvb/adapter?/dvr?`` 是一个逻辑设备，用于为数字视频录制
获取传输流(Transport Stream)。从该设备读取时，会得到一个传输流，其中
包含对应 demux 设备(``/dev/dvb/adapter?/demux?``)中所有输出被设为
``DMX_OUT_TS_TAP`` 的 PES 过滤器的数据包。通过向该设备写入可以重放
已录制的传输流。)

The significance of blocking or non-blocking mode is described in the
documentation for functions where there is a difference. It does not
affect the semantics of the ``open()`` call itself. A device opened
in blocking mode can later be put into non-blocking mode (and vice versa)
using the ``F_SETFL`` command of the fcntl system call.
(阻塞或非阻塞模式的意义在存在差异的函数的文档中说明。它不影响
``open()`` 调用本身的语义。以阻塞模式打开的设备之后可以用 fcntl 系统
调用的 ``F_SETFL`` 命令切换到非阻塞模式(反之亦然)。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1 并适当设置 ``errno`` 变量。)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  -  ``EMFILE``
       -  "Too many open files", i.e. no more filters available.
          ("打开文件过多"，即没有更多可用的过滤器。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
