.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.dmx

.. _DMX_START:

===================
DMX_START(启动过滤)
===================

Name(名称)
----------

DMX_START

Synopsis(概要)
--------------

.. c:macro:: DMX_START

``int ioctl(int fd, DMX_START)``

Arguments(参数)
---------------

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

Description(描述)
-----------------

This ioctl call is used to start the actual filtering operation defined
via the ioctl calls :ref:`DMX_SET_FILTER` or :ref:`DMX_SET_PES_FILTER`.
(此 ioctl 调用用于启动由 :ref:`DMX_SET_FILTER` 或 :ref:`DMX_SET_PES_FILTER`
ioctl 调用所定义的实际过滤操作。)

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

    -  .. row 1

       -  ``EINVAL``

       -  Invalid argument, i.e. no filtering parameters provided via the
	  :ref:`DMX_SET_FILTER` or :ref:`DMX_SET_PES_FILTER` ioctls.(无效参数，
	  即没有通过 :ref:`DMX_SET_FILTER` 或 :ref:`DMX_SET_PES_FILTER` ioctl
	  提供过滤参数。)

    -  .. row 2

       -  ``EBUSY``

       -  This error code indicates that there are conflicting requests.
	  There are active filters filtering data from another input source.
	  Make sure that these filters are stopped before starting this
	  filter.(此错误码表示存在相互冲突的请求。已有活动的过滤器正在从另一
	  个输入源过滤数据。请确保在启动本过滤器之前先停止这些过滤器。)

The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
