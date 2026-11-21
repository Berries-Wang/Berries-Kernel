.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUMAUDOUT:

***************************************
ioctl VIDIOC_ENUMAUDOUT(枚举音频输出)
***************************************

Name(名称)
==========

VIDIOC_ENUMAUDOUT - Enumerate audio outputs(VIDIOC_ENUMAUDOUT - 枚举音频输出)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUMAUDOUT

``int ioctl(int fd, VIDIOC_ENUMAUDOUT, struct v4l2_audioout *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_audioout`.
    (指向 struct :c:type:`v4l2_audioout` 的指针。)

Description(描述)
=================

To query the attributes of an audio output applications initialize the
``index`` field and zero out the ``reserved`` array of a struct
:c:type:`v4l2_audioout` and call the ``VIDIOC_G_AUDOUT``
ioctl with a pointer to this structure. Drivers fill the rest of the
structure or return an ``EINVAL`` error code when the index is out of
bounds. To enumerate all audio outputs applications shall begin at index
zero, incrementing by one until the driver returns ``EINVAL``.
(要查询音频输出的属性，应用初始化 struct :c:type:`v4l2_audioout` 的
``index`` 字段并将 ``reserved`` 数组清零，然后以指向该结构的指针调用
``VIDIOC_G_AUDOUT`` ioctl。驱动填充结构的其余部分；当索引越界时返回
``EINVAL`` 错误码。要枚举所有音频输出，应用应从索引 0 开始，每次加一，直到
驱动返回 ``EINVAL``。)

.. note::

    Connectors on a TV card to loop back the received audio signal
    to a sound card are not audio outputs in this sense.
    (电视卡上用于把接收到的音频信号回送 (loop back) 给声卡的连接器，在这个
    意义上不属于音频输出。)

See :ref:`VIDIOC_G_AUDIOout <VIDIOC_G_AUDOUT>` for a description of struct
:c:type:`v4l2_audioout`.
(struct :c:type:`v4l2_audioout` 的描述参见
:ref:`VIDIOC_G_AUDIOout <VIDIOC_G_AUDOUT>` 。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The number of the audio output is out of bounds.
    (音频输出编号越界。)
