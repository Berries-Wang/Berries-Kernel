.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _rw:

*****************
Read/Write(读/写)
*****************

Input and output devices support the :c:func:`read()` and
:c:func:`write()` function, respectively, when the
``V4L2_CAP_READWRITE`` flag in the ``capabilities`` field of struct
:c:type:`v4l2_capability` returned by the
:ref:`VIDIOC_QUERYCAP` ioctl is set.
(当 :ref:`VIDIOC_QUERYCAP` ioctl 返回的 struct :c:type:`v4l2_capability`
的 ``capabilities`` 字段中设置了 ``V4L2_CAP_READWRITE`` 标志时，输入设备
和输出设备分别支持 :c:func:`read()` 和 :c:func:`write()` 函数。)

Drivers may need the CPU to copy the data, but they may also support DMA
to or from user memory, so this I/O method is not necessarily less
efficient than other methods merely exchanging buffer pointers. It is
considered inferior though because no meta-information like frame
counters or timestamps are passed. This information is necessary to
recognize frame dropping and to synchronize with other data streams.
However this is also the simplest I/O method, requiring little or no
setup to exchange data. It permits command line stunts like this (the
vidctrl tool is fictitious):
(驱动可能需要 CPU 来复制数据，但它们也可能支持与用户内存之间的 DMA，因此
这种 I/O 方法不一定比仅交换缓冲区指针的其他方法效率低。不过它被认为较差，
因为没有传递帧计数器或时间戳之类的元信息。识别丢帧以及与其他数据流同步都
需要这些信息。不过这也是最简单的 I/O 方法，交换数据几乎不需要或完全不需要
设置。它允许像这样的命令行绝技 (vidctrl 工具是虚构的)：)

.. code-block:: none

    $ vidctrl /dev/video --input=0 --format=YUYV --size=352x288
    $ dd if=/dev/video of=myimage.422 bs=202752 count=1

To read from the device applications use the :c:func:`read()`
function, to write the :c:func:`write()` function. Drivers
must implement one I/O method if they exchange data with applications,
but it need not be this. [#f1]_ When reading or writing is supported, the
driver must also support the :c:func:`select()` and
:c:func:`poll()` function. [#f2]_
(要从设备读取，应用使用 :c:func:`read()` 函数；要写入，使用
:c:func:`write()` 函数。如果驱动与应用交换数据，必须实现一种 I/O 方法，
但不一定非得是这种。 [#f1]_ 如果支持读取或写入，驱动还必须支持
:c:func:`select()` 和 :c:func:`poll()` 函数。 [#f2]_)

.. [#f1]
   It would be desirable if applications could depend on drivers
   supporting all I/O interfaces, but as much as the complex memory
   mapping I/O can be inadequate for some devices we have no reason to
   require this interface, which is most useful for simple applications
   capturing still images.
   (如果应用可以依赖支持所有 I/O 接口的驱动，那当然再好不过；但由于复杂的
   内存映射 I/O 对某些设备可能并不合适，我们没有理由要求这种接口——它对
   采集静态图像的简单应用最为有用。)

.. [#f2]
   At the driver level :c:func:`select()` and :c:func:`poll()` are
   the same, and :c:func:`select()` is too important to be optional.
   (在驱动层面，:c:func:`select()` 与 :c:func:`poll()` 是一样的，而
   :c:func:`select()` 太重要了，不能作为可选项。)
