.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_SET_PES_FILTER:

===================================
DMX_SET_PES_FILTER(设置 PES 过滤器)
===================================

Name(名称)
----------

DMX_SET_PES_FILTER

Synopsis(概要)
--------------

.. c:macro:: DMX_SET_PES_FILTER

``int ioctl(int fd, DMX_SET_PES_FILTER, struct dmx_pes_filter_params *params)``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``params``
    Pointer to structure containing filter parameters.
    (指向包含过滤参数的结构的指针。)

Description(描述)
-----------------

This ioctl call sets up a PES filter according to the parameters
provided. By a PES filter is meant a filter that is based just on the
packet identifier (PID), i.e. no PES header or payload filtering
capability is supported.
(此 ioctl 调用根据提供的参数设置一个 PES 过滤器。所谓 PES 过滤器，是指
仅基于包标识符 (PID) 的过滤器，即不支持 PES 头部或载荷过滤能力。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并相应设置 ``errno`` 变量。)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  .. row 1

       -  ``EBUSY``

       -  This error code indicates that there are conflicting requests.
	  There are active filters filtering data from another input source.
	  Make sure that these filters are stopped before starting this
	  filter.(此错误码表示存在相互冲突的请求。已有活动的过滤器正在从另一
	  个输入源过滤数据。请确保在启动本过滤器之前先停止这些过滤器。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
