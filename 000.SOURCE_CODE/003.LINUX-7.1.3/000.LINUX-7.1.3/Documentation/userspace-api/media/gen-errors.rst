.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _gen_errors:

*******************************
Generic Error Codes(通用错误码)
*******************************


.. _gen-errors:

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table:: Generic error codes
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16


    -  -  ``EAGAIN`` (aka ``EWOULDBLOCK``)

       -  The ioctl can't be handled because the device is in state where it
	  can't perform it. This could happen for example in case where
	  device is sleeping and ioctl is performed to query statistics. It
	  is also returned when the ioctl would need to wait for an event,
	  but the device was opened in non-blocking mode.
          (无法处理该 ioctl，因为设备处于无法执行它的状态。例如，设备
          正在睡眠而执行 ioctl 查询统计信息时就可能发生这种情况。当
          ioctl 需要等待某个事件，但设备以非阻塞模式打开时，也会返回
          此错误码。)

    -  -  ``EBADF``

       -  The file descriptor is not a valid.
          (文件描述符无效。)

    -  -  ``EBUSY``

       -  The ioctl can't be handled because the device is busy. This is
	  typically return while device is streaming, and an ioctl tried to
	  change something that would affect the stream, or would require
	  the usage of a hardware resource that was already allocated. The
	  ioctl must not be retried without performing another action to fix
	  the problem first (typically: stop the stream before retrying).
          (无法处理该 ioctl，因为设备忙。通常在设备正在流式传输时返回，
          此时某个 ioctl 试图更改会影响流的内容，或需要使用已经分配的
          硬件资源。在不先执行其他操作修复问题之前(通常是：重试之前
          先停止流)，不得重试该 ioctl。)

    -  -  ``EFAULT``

       -  There was a failure while copying data from/to userspace, probably
	  caused by an invalid pointer reference.
          (在用户空间与内核之间复制数据时发生失败，可能由无效的指针
          引用引起。)

    -  -  ``EINVAL``

       -  One or more of the ioctl parameters are invalid or out of the
	  allowed range. This is a widely used error code. See the
	  individual ioctl requests for specific causes.
          (ioctl 的一个或多个参数无效或超出允许范围。这是一个被广泛
          使用的错误码。具体原因参见各个 ioctl 请求的说明。)

    -  -  ``ENODEV``

       -  Device not found or was removed.
          (找不到设备，或设备已被移除。)

    -  -  ``ENOMEM``

       -  There's not enough memory to handle the desired operation.
          (没有足够的内存来处理所需的操作。)

    -  -  ``ENOTTY``

       -  The ioctl is not supported by the file descriptor.
          (文件描述符不支持该 ioctl。)

    -  -  ``ENOSPC``

       -  On USB devices, the stream ioctl's can return this error, meaning
	  that this request would overcommit the usb bandwidth reserved for
	  periodic transfers (up to 80% of the USB bandwidth).
          (在 USB 设备上，流式 ioctl 可能返回此错误，表示该请求会超出
          为周期性传输保留的 USB 带宽(最多为 USB 带宽的 80%)。)

    -  -  ``EPERM``

       -  Permission denied. Can be returned if the device needs write
	  permission, or some special capabilities is needed (e. g. root)
          (权限被拒绝。当设备需要写权限，或需要某些特殊能力(例如
          root)时，可能返回此错误码。)

    -  -  ``EIO``

       -  I/O error. Typically used when there are problems communicating with
          a hardware device. This could indicate broken or flaky hardware.
	  It's a 'Something is wrong, I give up!' type of error.
          (I/O 错误。通常在与硬件设备通信出现问题时使用。这可能表示
          硬件损坏或不稳定。这是一种 "出问题了，我放弃！" 类型的
          错误。)

    -  - ``ENXIO``

       -  No device corresponding to this device special file exists.
          (不存在与该设备特殊文件对应的设备。)


.. note::

  #. This list is not exhaustive; ioctls may return other error codes.
     Since errors may have side effects such as a driver reset,
     applications should abort on unexpected errors, or otherwise
     assume that the device is in a bad state.
     (此列表并非详尽无遗；ioctl 可能返回其他错误码。由于错误可能
     有副作用(例如驱动程序重置)，应用程序遇到意外错误时应中止，
     否则应假定设备处于不良状态。)

  #. Request-specific error codes are listed in the individual
     requests descriptions.
     (特定于请求的错误码列在各个请求的说明中。)
