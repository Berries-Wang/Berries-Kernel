.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_OUTPUT:

**************************************************************
ioctl VIDIOC_G_OUTPUT, VIDIOC_S_OUTPUT(查询或选择当前视频输出)
**************************************************************

Name(名称)
==========

VIDIOC_G_OUTPUT - VIDIOC_S_OUTPUT - Query or select the current video output(VIDIOC_G_OUTPUT - VIDIOC_S_OUTPUT - 查询或选择当前视频输出)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_OUTPUT

``int ioctl(int fd, VIDIOC_G_OUTPUT, int *argp)``

.. c:macro:: VIDIOC_S_OUTPUT

``int ioctl(int fd, VIDIOC_S_OUTPUT, int *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to an integer with output index.
    (指向一个整数，其中为输出索引。)

Description(描述)
=================

To query the current video output applications call the
:ref:`VIDIOC_G_OUTPUT <VIDIOC_G_OUTPUT>` ioctl with a pointer to an integer where the driver
stores the number of the output, as in the struct
:c:type:`v4l2_output` ``index`` field. This ioctl will
fail only when there are no video outputs, returning the ``EINVAL`` error
code.
(要查询当前视频输出，应用调用 :ref:`VIDIOC_G_OUTPUT <VIDIOC_G_OUTPUT>` ioctl，
并传入一个指针，指向驱动用来存放输出编号的整数，如同 struct
:c:type:`v4l2_output` 中的 ``index`` 字段。只有当设备没有任何视频输出时，此
ioctl 才会失败，并返回 ``EINVAL`` 错误码。)

To select a video output applications store the number of the desired
output in an integer and call the :ref:`VIDIOC_S_OUTPUT <VIDIOC_G_OUTPUT>` ioctl with a
pointer to this integer. Side effects are possible. For example outputs
may support different video standards, so the driver may implicitly
switch the current standard. Because of these possible side
effects applications must select an output before querying or
negotiating any other parameters.
(要选择视频输出，应用把期望输出的编号存入一个整数，并调用
:ref:`VIDIOC_S_OUTPUT <VIDIOC_G_OUTPUT>` ioctl，传入指向该整数的指针。可能
产生副作用。例如输出可能支持不同的视频制式，驱动可能会隐式切换当前制式。
由于这些可能的副作用，应用在查询或协商其他任何参数之前必须先选定一个输出。)

Information about video outputs is available using the
:ref:`VIDIOC_ENUMOUTPUT` ioctl.
(关于视频输出的信息可通过 :ref:`VIDIOC_ENUMOUTPUT` ioctl 获取。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The number of the video output is out of bounds, or there are no
    video outputs at all.
    (视频输出编号越界，或者根本没有视频输出。)
