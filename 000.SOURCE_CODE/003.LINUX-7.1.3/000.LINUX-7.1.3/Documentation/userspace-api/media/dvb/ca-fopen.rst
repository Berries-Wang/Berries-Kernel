.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: DTV.ca

.. _ca_fopen:

========================================
Digital TV CA open()(数字电视 CA open())
========================================

Name(名称)
----------

Digital TV CA open()
(数字电视 CA open()。)

Synopsis(概要)
--------------

.. c:function:: int open(const char *name, int flags)

Arguments(参数)
---------------

``name``
  Name of specific Digital TV CA device.
  (特定数字电视 CA 设备的名称。)

``flags``
  A bit-wise OR of the following flags:
  (以下标志的按位或 (bit-wise OR)：)

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  - ``O_RDONLY``
       - read-only access
         (只读访问。)

    -  - ``O_RDWR``
       - read/write access
         (读写访问。)

    -  - ``O_NONBLOCK``
       - open in non-blocking mode
         (blocking mode is the default)
         (以非阻塞模式打开 (阻塞模式是默认值)。)

Description(描述)
-----------------

This system call opens a named ca device (e.g. ``/dev/dvb/adapter?/ca?``)
for subsequent use.
(此系统调用打开一个命名的 ca 设备 (例如 ``/dev/dvb/adapter?/ca?``) 以
供后续使用。)

When an ``open()`` call has succeeded, the device will be ready for use. The
significance of blocking or non-blocking mode is described in the
documentation for functions where there is a difference. It does not
affect the semantics of the ``open()`` call itself. A device opened in
blocking mode can later be put into non-blocking mode (and vice versa)
using the ``F_SETFL`` command of the ``fcntl`` system call. This is a
standard system call, documented in the Linux manual page for fcntl.
Only one user can open the CA Device in ``O_RDWR`` mode. All other
attempts to open the device in this mode will fail, and an error code
will be returned.
(当 ``open()`` 调用成功后，设备即可供使用。阻塞或非阻塞模式的意义在存
在差异的那些函数的文档中说明；它不影响 ``open()`` 调用本身的语义。以
阻塞模式打开的设备之后可以用 ``fcntl`` 系统调用的 ``F_SETFL`` 命令切
换到非阻塞模式 (反之亦然)。这是一个标准系统调用，在 Linux 的 fcntl 手
册页中有文档。只有一个用户能以 ``O_RDWR`` 模式打开 CA 设备。其他以该
模式打开设备的尝试都会失败，并返回一个错误码。)

Return Value(返回值)
--------------------

On success 0 is returned.
(成功时返回 0。)

On error -1 is returned, and the ``errno`` variable is set
appropriately.
(出错时返回 -1，并适当设置 ``errno`` 变量。)

Generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
