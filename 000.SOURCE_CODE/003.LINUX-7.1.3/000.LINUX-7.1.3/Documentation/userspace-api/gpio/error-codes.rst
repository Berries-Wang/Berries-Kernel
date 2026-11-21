.. SPDX-License-Identifier: GPL-2.0

.. _gpio_errors:

*****************************
GPIO Error Codes(GPIO 错误码)
*****************************

.. _gpio-errors:

.. tabularcolumns:: |p{2.5cm}|p{15.0cm}|

.. flat-table:: Common GPIO error codes
    :header-rows:  0
    :stub-columns: 0
    :widths: 1 16

    -  -  ``EAGAIN`` (aka ``EWOULDBLOCK``)

       -  The device was opened in non-blocking mode and a read can't
          be performed as there is no data available.
          (设备以非阻塞模式打开，且由于没有可用数据而无法执行读取。)

    -  -  ``EBADF``

       -  The file descriptor is not valid.
          (文件描述符无效。)

    -  -  ``EBUSY``

       -  The ioctl can't be handled because the device is busy. Typically
          returned when an ioctl attempts something that would require the
          usage of a resource that was already allocated. The ioctl must not
          be retried without performing another action to fix the problem
          first.
          (由于设备忙，ioctl 无法被处理。通常在 ioctl 尝试某件需要使用已
          被分配的资源的事情时返回。必须先执行另一个动作来解决问题，之
          后才能重试该 ioctl。)

    -  -  ``EFAULT``

       -  There was a failure while copying data from/to userspace, probably
	  caused by an invalid pointer reference.
          (在从用户空间复制数据或向用户空间复制数据时发生失败，可能是
          由无效的指针引用造成的。)

    -  -  ``EINVAL``

       -  One or more of the ioctl parameters are invalid or out of the
          allowed range. This is a widely used error code.
          (ioctl 的一个或多个参数无效或超出允许范围。这是一个被广泛使
          用的错误码。)

    -  -  ``ENODEV``

       -  Device not found or was removed.
          (未找到设备，或设备已被移除。)

    -  -  ``ENOMEM``

       -  There's not enough memory to handle the desired operation.
          (没有足够的内存来处理所需的操作。)

    -  -  ``EPERM``

       -  Permission denied. Typically returned in response to an attempt
          to perform an action incompatible with the current line
          configuration.
          (权限被拒绝。通常在响应与当前线路配置不兼容的操作尝试时返
          回。)

    -  -  ``EIO``

       -  I/O error. Typically returned when there are problems communicating
          with a hardware device or requesting features that hardware does not
          support. This could indicate broken or flaky hardware.
          It's a 'Something is wrong, I give up!' type of error.
          (I/O 错误。通常在与硬件设备通信或请求硬件不支持的特性出现问
          题时返回。这可能表明硬件损坏或不稳定。它属于"出问题了，我放
          弃！"类型的错误。)

    -  - ``ENXIO``

       -  Typically returned when a feature requiring interrupt support was
          requested, but the line does not support interrupts.
          (通常在请求了需要中断支持的特性，但该线路不支持中断时返回。)

.. note::

  #. This list is not exhaustive; ioctls may return other error codes.
     Since errors may have side effects such as a driver reset,
     applications should abort on unexpected errors, or otherwise
     assume that the device is in a bad state.
     (此列表并非详尽无遗；ioctl 可能返回其他错误码。由于错误可能产生
     副作用 (例如驱动程序复位)，应用程序应当在遇到意外错误时中止，或
     者认为设备处于不良状态。)

  #. Request-specific error codes are listed in the individual
     requests descriptions.
     (特定于请求的错误码列在各请求的描述中。)
