.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_INPUT:

************************************************************
ioctl VIDIOC_G_INPUT, VIDIOC_S_INPUT(查询或选择当前视频输入)
************************************************************

Name(名称)
==========

VIDIOC_G_INPUT - VIDIOC_S_INPUT - Query or select the current video input(VIDIOC_G_INPUT - VIDIOC_S_INPUT - 查询或选择当前视频输入)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_INPUT

``int ioctl(int fd, VIDIOC_G_INPUT, int *argp)``

.. c:macro:: VIDIOC_S_INPUT

``int ioctl(int fd, VIDIOC_S_INPUT, int *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer an integer with input index.
    (指向一个整数，其中为输入索引。)

Description(描述)
=================

To query the current video input applications call the
:ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` ioctl with a pointer to an integer where the driver
stores the number of the input, as in the struct
:c:type:`v4l2_input` ``index`` field. This ioctl will fail
only when there are no video inputs, returning ``EINVAL``.
(要查询当前视频输入，应用调用 :ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` ioctl，
并传入一个指针，指向驱动用来存放输入编号的整数，如同 struct
:c:type:`v4l2_input` 中的 ``index`` 字段。只有当设备没有任何视频输入时，
此 ioctl 才会失败，返回 ``EINVAL``。)

To select a video input applications store the number of the desired
input in an integer and call the :ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>` ioctl with a pointer
to this integer. Side effects are possible. For example inputs may
support different video standards, so the driver may implicitly switch
the current standard. Because of these possible side effects
applications must select an input before querying or negotiating any
other parameters.
(要选择视频输入，应用把期望输入的编号存入一个整数，并调用
:ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>` ioctl，传入指向该整数的指针。可能
产生副作用。例如输入可能支持不同的视频制式，驱动可能会隐式切换当前制式。
由于这些可能的副作用，应用在查询或协商其他任何参数之前必须先选定一个
输入。)

Information about video inputs is available using the
:ref:`VIDIOC_ENUMINPUT` ioctl.
(关于视频输入的信息可通过 :ref:`VIDIOC_ENUMINPUT` ioctl 获取。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The number of the video input is out of bounds.
    (视频输入编号越界。)
