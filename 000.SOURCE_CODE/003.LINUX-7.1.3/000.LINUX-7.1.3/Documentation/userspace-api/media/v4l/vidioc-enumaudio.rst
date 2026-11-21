.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENUMAUDIO:

**************************************
ioctl VIDIOC_ENUMAUDIO(枚举音频输入)
**************************************

Name(名称)
==========

VIDIOC_ENUMAUDIO - Enumerate audio inputs(VIDIOC_ENUMAUDIO - 枚举音频输入)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENUMAUDIO

``int ioctl(int fd, VIDIOC_ENUMAUDIO, struct v4l2_audio *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_audio`.
    (指向 struct :c:type:`v4l2_audio` 的指针。)

Description(描述)
=================

To query the attributes of an audio input applications initialize the
``index`` field and zero out the ``reserved`` array of a struct
:c:type:`v4l2_audio` and call the :ref:`VIDIOC_ENUMAUDIO`
ioctl with a pointer to this structure. Drivers fill the rest of the
structure or return an ``EINVAL`` error code when the index is out of
bounds. To enumerate all audio inputs applications shall begin at index
zero, incrementing by one until the driver returns ``EINVAL``.
(要查询音频输入的属性，应用初始化 struct :c:type:`v4l2_audio` 的 ``index``
字段并将 ``reserved`` 数组清零，然后以指向该结构的指针调用
:ref:`VIDIOC_ENUMAUDIO` ioctl。驱动填充结构的其余部分；当索引越界时返回
``EINVAL`` 错误码。要枚举所有音频输入，应用应从索引 0 开始，每次加一，直到
驱动返回 ``EINVAL``。)

See :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>` for a description of struct
:c:type:`v4l2_audio`.
(struct :c:type:`v4l2_audio` 的描述参见 :ref:`VIDIOC_G_AUDIO <VIDIOC_G_AUDIO>`
。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0；出错时返回 -1，并相应设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    The number of the audio input is out of bounds.
    (音频输入编号越界。)
