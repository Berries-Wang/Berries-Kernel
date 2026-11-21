.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_G_AUDOUT:

**************************************************************
ioctl VIDIOC_G_AUDOUT, VIDIOC_S_AUDOUT(查询或选择当前音频输出)
**************************************************************

Name(名称)
==========

VIDIOC_G_AUDOUT - VIDIOC_S_AUDOUT - Query or select the current audio output(VIDIOC_G_AUDOUT - VIDIOC_S_AUDOUT - 查询或选择当前音频输出)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_G_AUDOUT

``int ioctl(int fd, VIDIOC_G_AUDOUT, struct v4l2_audioout *argp)``

.. c:macro:: VIDIOC_S_AUDOUT

``int ioctl(int fd, VIDIOC_S_AUDOUT, const struct v4l2_audioout *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (:c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_audioout`.
    (指向 struct :c:type:`v4l2_audioout` 的指针。)

Description(描述)
=================

To query the current audio output applications zero out the ``reserved``
array of a struct :c:type:`v4l2_audioout` and call the
``VIDIOC_G_AUDOUT`` ioctl with a pointer to this structure. Drivers fill
the rest of the structure or return an ``EINVAL`` error code when the device
has no audio inputs, or none which combine with the current video
output.
(要查询当前音频输出，应用程序把 struct :c:type:`v4l2_audioout` 的
``reserved`` 数组清零，并以指向该结构体的指针调用 ``VIDIOC_G_AUDOUT``
ioctl。当设备没有音频输入，或没有能与当前视频输出组合的音频输入时，
驱动程序填充结构体的其余部分或返回 ``EINVAL`` 错误码。)

Audio outputs have no writable properties. Nevertheless, to select the
current audio output applications can initialize the ``index`` field and
``reserved`` array (which in the future may contain writable properties)
of a struct :c:type:`v4l2_audioout` structure and call the
``VIDIOC_S_AUDOUT`` ioctl. Drivers switch to the requested output or
return the ``EINVAL`` error code when the index is out of bounds. This is a
write-only ioctl, it does not return the current audio output attributes
as ``VIDIOC_G_AUDOUT`` does.
(音频输出没有可写属性。尽管如此，要选择当前音频输出，应用程序可以
初始化 struct :c:type:`v4l2_audioout` 结构体的 ``index`` 字段和
``reserved`` 数组(将来可能包含可写属性)，并调用 ``VIDIOC_S_AUDOUT``
ioctl。驱动程序切换到请求的输出；当索引越界时返回 ``EINVAL`` 错误码。
这是一个只写 ioctl，不会像 ``VIDIOC_G_AUDOUT`` 那样返回当前音频输出
的属性。)

.. note::

   Connectors on a TV card to loop back the received audio signal
   to a sound card are not audio outputs in this sense.
   (电视卡上用于把接收到的音频信号回送(loop back)到声卡的连接器，
   在这个意义上不是音频输出。)

.. c:type:: v4l2_audioout

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. flat-table:: struct v4l2_audioout
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``index``
      - Identifies the audio output, set by the driver or application.
        (标识音频输出，由驱动程序或应用程序设置。)
    * - __u8
      - ``name``\ [32]
      - Name of the audio output, a NUL-terminated ASCII string, for
	example: "Line Out". This information is intended for the user,
	preferably the connector label on the device itself.
        (音频输出的名称，为以 NUL 结尾的 ASCII 字符串，例如 "Line Out"。
        该信息面向用户，最好是设备本身上的连接器标签。)
    * - __u32
      - ``capability``
      - Audio capability flags, none defined yet. Drivers must set this
	field to zero.
        (音频能力标志，目前尚未定义任何标志。驱动程序必须把此字段置
        为零。)
    * - __u32
      - ``mode``
      - Audio mode, none defined yet. Drivers and applications (on
	``VIDIOC_S_AUDOUT``) must set this field to zero.
        (音频模式，目前尚未定义任何模式。驱动程序和应用程序(在
        ``VIDIOC_S_AUDOUT`` 中)必须把此字段置为零。)
    * - __u32
      - ``reserved``\ [2]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
        (保留用于未来的扩展。驱动程序和应用程序必须将此数组置零。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在
:ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EINVAL
    No audio outputs combine with the current video output, or the
    number of the selected audio output is out of bounds or it does not
    combine.
    (没有音频输出能与当前视频输出组合，或所选音频输出的编号越界或
    不能组合。)
