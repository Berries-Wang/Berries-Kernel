.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _selection-vs-crop:

*****************************************************
Comparison with old cropping API(与旧裁剪 API 的比较)
*****************************************************

The selection API was introduced to cope with deficiencies of the
older :ref:`CROP API <crop>`, that was designed to control simple
capture devices. Later the cropping API was adopted by video output
drivers. The ioctls are used to select a part of the display were the
video signal is inserted. It should be considered as an API abuse
because the described operation is actually the composing. The
selection API makes a clear distinction between composing and cropping
operations by setting the appropriate targets.
(选择 API 的引入是为了弥补较旧的 :ref:`CROP API <crop>` 的不足——后者最初
为控制简单采集设备而设计。后来视频输出驱动也采用了裁剪 API。这些 ioctl 用
于选择显示器上插入视频信号的部分。这应视为对 API 的滥用，因为所述操作实际
上是合成（composing）。选择 API 通过设置适当的目标，对合成与裁剪操作作出了
清晰的区分。)

The CROP API lacks any support for composing to and cropping from an
image inside a memory buffer. The application could configure a
capture device to fill only a part of an image by abusing V4L2
API. Cropping a smaller image from a larger one is achieved by setting
the field ``bytesperline`` at struct :c:type:`v4l2_pix_format`.
Introducing an image offsets could be done by modifying field
``m_userptr`` at struct :c:type:`v4l2_buffer` before calling
:ref:`VIDIOC_QBUF <VIDIOC_QBUF>`. Those operations should be avoided
because they are not portable (endianness), and do not work for
macroblock and Bayer formats and mmap buffers.
(CROP API 完全不支持在内存缓冲区内对图像进行合成与裁剪。应用程序可以滥用
V4L2 API 把采集设备配置为只填充图像的一部分：通过设置 struct
:c:type:`v4l2_pix_format` 的 ``bytesperline`` 字段实现从大图裁出小图；在调
用 :ref:`VIDIOC_QBUF <VIDIOC_QBUF>` 之前修改 struct :c:type:`v4l2_buffer`
的 ``m_userptr`` 字段引入图像偏移。这些操作应当避免，因为它们不可移植（字
节序问题），且对宏块和 Bayer 格式以及 mmap 缓冲区不生效。)

The selection API deals with configuration of buffer
cropping/composing in a clear, intuitive and portable way. Next, with
the selection API the concepts of the padded target and constraints
flags are introduced. Finally, struct :c:type:`v4l2_crop` and struct
:c:type:`v4l2_cropcap` have no reserved fields. Therefore there is no
way to extend their functionality. The new struct
:c:type:`v4l2_selection` provides a lot of place for future
extensions.
(选择 API 以清晰、直观且可移植的方式处理缓冲区裁剪/合成的配置。此外，选择
API 引入了带填充目标（padded target）与约束标志的概念。最后，struct
:c:type:`v4l2_crop` 和 struct :c:type:`v4l2_cropcap` 没有保留字段，因此无
法扩展其功能。新的 struct :c:type:`v4l2_selection` 为未来扩展提供了充足空
间。)

Driver developers are encouraged to implement only selection API. The
former cropping API would be simulated using the new one.
(鼓励驱动开发者只实现选择 API。旧的裁剪 API 将用新 API 模拟实现。)
