.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_ENCODER_CMD:

**************************************************************************************************
ioctl VIDIOC_ENCODER_CMD, VIDIOC_TRY_ENCODER_CMD(ioctl VIDIOC_ENCODER_CMD、VIDIOC_TRY_ENCODER_CMD)
**************************************************************************************************

Name(名称)
==========

VIDIOC_ENCODER_CMD - VIDIOC_TRY_ENCODER_CMD - Execute an encoder command
(VIDIOC_ENCODER_CMD - VIDIOC_TRY_ENCODER_CMD - 执行编码器命令)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_ENCODER_CMD

``int ioctl(int fd, VIDIOC_ENCODER_CMD, struct v4l2_encoder_cmd *argp)``

.. c:macro:: VIDIOC_TRY_ENCODER_CMD

``int ioctl(int fd, VIDIOC_TRY_ENCODER_CMD, struct v4l2_encoder_cmd *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_encoder_cmd`.
    (指向 struct :c:type:`v4l2_encoder_cmd` 的指针。)

Description(描述)
=================

These ioctls control an audio/video (usually MPEG-) encoder.
``VIDIOC_ENCODER_CMD`` sends a command to the encoder,
``VIDIOC_TRY_ENCODER_CMD`` can be used to try a command without actually
executing it.
(这些 ioctl 控制音频/视频(通常是 MPEG)编码器。 ``VIDIOC_ENCODER_CMD`` 向编码器发送命令， ``VIDIOC_TRY_ENCODER_CMD`` 可用于在不实际执行的情况下试用命令。)

To send a command applications must initialize all fields of a struct
:c:type:`v4l2_encoder_cmd` and call
``VIDIOC_ENCODER_CMD`` or ``VIDIOC_TRY_ENCODER_CMD`` with a pointer to
this structure.
(要发送命令，应用程序必须初始化 struct :c:type:`v4l2_encoder_cmd` 的所有字段，并以指向该结构的指针调用 ``VIDIOC_ENCODER_CMD`` 或 ``VIDIOC_TRY_ENCODER_CMD`` 。)

The ``cmd`` field must contain the command code. Some commands use the
``flags`` field for additional information.
( ``cmd`` 字段必须包含命令码。某些命令使用 ``flags`` 字段来携带附加信息。)

After a STOP command, :c:func:`read()` calls will read
the remaining data buffered by the driver. When the buffer is empty,
:c:func:`read()` will return zero and the next :c:func:`read()`
call will restart the encoder.
(在 STOP 命令之后， :c:func:`read()` 调用将读取驱动所缓冲的剩余数据。当缓冲区为空时， :c:func:`read()` 将返回零，而下一次 :c:func:`read()` 调用将重新启动编码器。)

A :c:func:`read()` or :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>`
call sends an implicit START command to the encoder if it has not been
started yet. Applies to both queues of mem2mem encoders.
(如果编码器尚未启动， :c:func:`read()` 或 :ref:`VIDIOC_STREAMON <VIDIOC_STREAMON>` 调用会向编码器发送隐式的 START 命令。适用于 mem2mem 编码器的两个队列。)

A :c:func:`close()` or :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>`
call of a streaming file descriptor sends an implicit immediate STOP to
the encoder, and all buffered data is discarded. Applies to both queues of
mem2mem encoders.
(对流式文件描述符调用 :c:func:`close()` 或 :ref:`VIDIOC_STREAMOFF <VIDIOC_STREAMON>` 会向编码器发送隐式的立即 STOP，所有缓冲的数据都会被丢弃。适用于 mem2mem 编码器的两个队列。)

These ioctls are optional, not all drivers may support them. They were
introduced in Linux 2.6.21. They are, however, mandatory for stateful mem2mem
encoders (as further documented in :ref:`encoder`).
(这些 ioctl 是可选的，并非所有驱动都支持。它们在 Linux 2.6.21 中引入。不过，对于有状态 (stateful) 的 mem2mem 编码器它们是必需的(进一步的说明见 :ref:`encoder` )。)

.. tabularcolumns:: |p{4.4cm}|p{4.4cm}|p{8.5cm}|

.. c:type:: v4l2_encoder_cmd

.. flat-table:: struct v4l2_encoder_cmd
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``cmd``
      - The encoder command, see :ref:`encoder-cmds`.(编码器命令，参见 :ref:`encoder-cmds` 。)
    * - __u32
      - ``flags``
      - Flags to go with the command, see :ref:`encoder-flags`. If no
	flags are defined for this command, drivers and applications must
	set this field to zero.
	(随命令一起使用的标志，参见 :ref:`encoder-flags` 。如果此命令
	没有定义任何标志，驱动和应用程序必须将此字段置零。)
    * - __u32
      - ``data``\ [8]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
	(保留供将来扩展使用。驱动和应用程序必须将该数组置零。)


.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _encoder-cmds:

.. flat-table:: Encoder Commands
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_ENC_CMD_START``
      - 0
      - Start the encoder. When the encoder is already running or paused,
	this command does nothing. No flags are defined for this command.
	(启动编码器。当编码器已在运行或已暂停时，此命令不做任何
	事情。此命令没有定义任何标志。)

	For a device implementing the :ref:`encoder`, once the drain sequence
	is initiated with the ``V4L2_ENC_CMD_STOP`` command, it must be driven
	to completion before this command can be invoked.  Any attempt to
	invoke the command while the drain sequence is in progress will trigger
	an ``EBUSY`` error code. See :ref:`encoder` for more details.
	(对于实现 :ref:`encoder` 的设备，一旦通过 ``V4L2_ENC_CMD_STOP``
	命令启动了排空 (drain) 序列，必须将其驱动至完成后才能调用此
	命令。在排空序列进行期间尝试调用此命令会触发 ``EBUSY`` 错误码。
	更多细节参见 :ref:`encoder` 。)
    * - ``V4L2_ENC_CMD_STOP``
      - 1
      - Stop the encoder. When the ``V4L2_ENC_CMD_STOP_AT_GOP_END`` flag
	is set, encoding will continue until the end of the current *Group
	Of Pictures*, otherwise encoding will stop immediately. When the
	encoder is already stopped, this command does nothing.
	(停止编码器。当设置了 ``V4L2_ENC_CMD_STOP_AT_GOP_END`` 标志时，
	编码将持续到当前 *Group Of Pictures* (图像组) 结束；否则编码将
	立即停止。当编码器已经停止时，此命令不做任何事情。)

	For a device implementing the :ref:`encoder`, the command will initiate
	the drain sequence as documented in :ref:`encoder`. No flags or other
	arguments are accepted in this case. Any attempt to invoke the command
	again before the sequence completes will trigger an ``EBUSY`` error
	code.
	(对于实现 :ref:`encoder` 的设备，该命令将启动 :ref:`encoder` 中
	描述的排空序列。在这种情况下不接受任何标志或其他参数。在序列
	完成之前再次尝试调用该命令会触发 ``EBUSY`` 错误码。)
    * - ``V4L2_ENC_CMD_PAUSE``
      - 2
      - Pause the encoder. When the encoder has not been started yet, the
	driver will return an ``EPERM`` error code. When the encoder is
	already paused, this command does nothing. No flags are defined
	for this command.
	(暂停编码器。当编码器尚未启动时，驱动将返回 ``EPERM`` 错误码。
	当编码器已经暂停时，此命令不做任何事情。此命令没有定义任何
	标志。)
    * - ``V4L2_ENC_CMD_RESUME``
      - 3
      - Resume encoding after a PAUSE command. When the encoder has not
	been started yet, the driver will return an ``EPERM`` error code. When
	the encoder is already running, this command does nothing. No
	flags are defined for this command.
	(在 PAUSE 命令之后恢复编码。当编码器尚未启动时，驱动将返回
	``EPERM`` 错误码。当编码器已在运行时，此命令不做任何事情。
	此命令没有定义任何标志。)

.. tabularcolumns:: |p{6.6cm}|p{2.2cm}|p{8.5cm}|

.. _encoder-flags:

.. flat-table:: Encoder Command Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_ENC_CMD_STOP_AT_GOP_END``
      - 0x0001
      - Stop encoding at the end of the current *Group Of Pictures*,
	rather than immediately.
	(在当前 *Group Of Pictures* (图像组) 结束时停止编码，而不是
	立即停止。)

        Does not apply to :ref:`encoder`.
        (不适用于 :ref:`encoder` 。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)

EBUSY
    A drain sequence of a device implementing the :ref:`encoder` is still in
    progress. It is not allowed to issue another encoder command until it
    completes.
    (实现 :ref:`encoder` 的设备的排空序列仍在进行中。在其完成之前，
    不允许发出其他编码器命令。)

EINVAL
    The ``cmd`` field is invalid.
    ( ``cmd`` 字段无效。)

EPERM
    The application sent a PAUSE or RESUME command when the encoder was
    not running.
    (在编码器未运行时，应用程序发送了 PAUSE 或 RESUME 命令。)
