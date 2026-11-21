.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _extended-controls:

***********************************
Extended Controls API(扩展控制 API)
***********************************


Introduction(简介)
==================

The control mechanism as originally designed was meant to be used for
user settings (brightness, saturation, etc). However, it turned out to
be a very useful model for implementing more complicated driver APIs
where each driver implements only a subset of a larger API.
(最初设计的控制机制是用于用户设置(亮度、饱和度等)的。然而，事实证明它对于实现更复杂的驱动 API 是一个非常有用的模型，其中每个驱动只实现更大 API 的一个子集。)

The MPEG encoding API was the driving force behind designing and
implementing this extended control mechanism: the MPEG standard is quite
large and the currently supported hardware MPEG encoders each only
implement a subset of this standard. Further more, many parameters
relating to how the video is encoded into an MPEG stream are specific to
the MPEG encoding chip since the MPEG standard only defines the format
of the resulting MPEG stream, not how the video is actually encoded into
that format.
(MPEG 编码 API 是设计并实现这种扩展控制机制背后的推动力：MPEG 标准相当庞大，而当前支持的硬件 MPEG 编码器各自只实现了该标准的一个子集。此外，许多与视频如何编码为 MPEG 流相关的参数是 MPEG 编码芯片特有的，因为 MPEG 标准只定义了最终 MPEG 流的格式，而没有定义视频实际如何编码为该格式。)

Unfortunately, the original control API lacked some features needed for
these new uses and so it was extended into the (not terribly originally
named) extended control API.
(遗憾的是，原始的控制 API 缺少这些新用途所需的一些特性，因此它被扩展成了(命名并不怎么有创意的)扩展控制 API。)

Even though the MPEG encoding API was the first effort to use the
Extended Control API, nowadays there are also other classes of Extended
Controls, such as Camera Controls and FM Transmitter Controls. The
Extended Controls API as well as all Extended Controls classes are
described in the following text.
(尽管 MPEG 编码 API 是使用扩展控制 API 的首次尝试，如今还有其他类别的扩展控制，例如相机控制 (Camera Controls) 和 FM 发送器控制 (FM Transmitter Controls)。扩展控制 API 以及所有扩展控制类都在下文中描述。)


The Extended Control API(扩展控制 API)
======================================

Three new ioctls are available:
:ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`,
:ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` and
:ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>`. These ioctls act
on arrays of controls (as opposed to the
:ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` and
:ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctls that act on a single
control). This is needed since it is often required to atomically change
several controls at once.
(有三个新的 ioctl 可用： :ref:`VIDIOC_G_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 、 :ref:`VIDIOC_S_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 和 :ref:`VIDIOC_TRY_EXT_CTRLS <VIDIOC_G_EXT_CTRLS>` 。这些 ioctl 作用于控制数组(与作用于单个控制的 :ref:`VIDIOC_G_CTRL <VIDIOC_G_CTRL>` 和 :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctl 相对)。这是必需的，因为经常需要原子性地同时更改多个控制。)

Each of the new ioctls expects a pointer to a struct
:c:type:`v4l2_ext_controls`. This structure
contains a pointer to the control array, a count of the number of
controls in that array and a control class. Control classes are used to
group similar controls into a single class. For example, control class
``V4L2_CTRL_CLASS_USER`` contains all user controls (i. e. all controls
that can also be set using the old :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>`
ioctl). Control class ``V4L2_CTRL_CLASS_CODEC`` contains controls
relating to codecs.
(每个新 ioctl 都需要一个指向 struct :c:type:`v4l2_ext_controls` 的指针。该结构包含一个指向控制数组的指针、该数组中控制数量的计数以及一个控制类。控制类用于将相似的控制分组到单个类中。例如，控制类 ``V4L2_CTRL_CLASS_USER`` 包含所有用户控制(即所有也可以使用旧的 :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` ioctl 设置的控制)。控制类 ``V4L2_CTRL_CLASS_CODEC`` 包含与编解码器相关的控制。)

All controls in the control array must belong to the specified control
class. An error is returned if this is not the case.
(控制数组中的所有控制都必须属于指定的控制类。如果不是这样，则返回错误。)

It is also possible to use an empty control array (``count`` == 0) to check
whether the specified control class is supported.
(也可以使用空的控制数组( ``count`` == 0)来检查是否支持指定的控制类。)

The control array is a struct
:c:type:`v4l2_ext_control` array. The
struct :c:type:`v4l2_ext_control` is very similar to
struct :c:type:`v4l2_control`, except for the fact that
it also allows for 64-bit values and pointers to be passed.
(控制数组是一个 struct :c:type:`v4l2_ext_control` 数组。struct :c:type:`v4l2_ext_control` 与 struct :c:type:`v4l2_control` 非常相似，不同之处在于它还允许传递 64 位值和指针。)

Since the struct :c:type:`v4l2_ext_control` supports
pointers it is now also possible to have controls with compound types
such as N-dimensional arrays and/or structures. You need to specify the
``V4L2_CTRL_FLAG_NEXT_COMPOUND`` when enumerating controls to actually
be able to see such compound controls. In other words, these controls
with compound types should only be used programmatically.
(由于 struct :c:type:`v4l2_ext_control` 支持指针，现在也可以拥有复合类型 (compound type) 的控制，例如 N 维数组和/或结构体。在枚举控制时需要指定 ``V4L2_CTRL_FLAG_NEXT_COMPOUND`` 才能真正看到这类复合控制。换句话说，这些具有复合类型的控制只应以编程方式使用。)

Since such compound controls need to expose more information about
themselves than is possible with :ref:`VIDIOC_QUERYCTRL <VIDIOC_QUERYCTRL>`
the :ref:`VIDIOC_QUERY_EXT_CTRL <VIDIOC_QUERYCTRL>` ioctl was added. In
particular, this ioctl gives the dimensions of the N-dimensional array if
this control consists of more than one element.
(由于这类复合控制需要暴露比 :ref:`VIDIOC_QUERYCTRL <VIDIOC_QUERYCTRL>` 所能提供的更多信息，因此增加了 :ref:`VIDIOC_QUERY_EXT_CTRL <VIDIOC_QUERYCTRL>` ioctl。特别是，如果该控制由多于一个元素组成，此 ioctl 会给出该 N 维数组的维度。)

.. note::

   #. It is important to realize that due to the flexibility of controls it is
      necessary to check whether the control you want to set actually is
      supported in the driver and what the valid range of values is. So use
      :ref:`VIDIOC_QUERYCTRL` to check this.
      (重要的是要意识到，由于控制具有灵活性，必须检查你要设置的控制是否确实受驱动支持，以及其有效取值范围是什么。因此请使用 :ref:`VIDIOC_QUERYCTRL` 来检查这一点。)

   #. It is possible that some of the menu indices in a control of
      type ``V4L2_CTRL_TYPE_MENU`` may not be supported (``VIDIOC_QUERYMENU``
      will return an error). A good example is the list of supported MPEG
      audio bitrates. Some drivers only support one or two bitrates, others
      support a wider range.
      ( ``V4L2_CTRL_TYPE_MENU`` 类型的控制中的某些菜单索引可能不受支持(``VIDIOC_QUERYMENU`` 会返回错误)。一个很好的例子是受支持的 MPEG 音频比特率列表。有些驱动只支持一两种比特率，另一些则支持更宽的范围。)

All controls use machine endianness.
(所有控制都使用机器字节序 (machine endianness)。)


Enumerating Extended Controls(枚举扩展控制)
===========================================

The recommended way to enumerate over the extended controls is by using
:ref:`VIDIOC_QUERYCTRL` in combination with the
``V4L2_CTRL_FLAG_NEXT_CTRL`` flag:
(枚举扩展控制的推荐方式是将 :ref:`VIDIOC_QUERYCTRL` 与 ``V4L2_CTRL_FLAG_NEXT_CTRL`` 标志结合使用：)


.. code-block:: c

    struct v4l2_queryctrl qctrl;

    qctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl (fd, VIDIOC_QUERYCTRL, &qctrl)) {
	/* ... */
	qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }

The initial control ID is set to 0 ORed with the
``V4L2_CTRL_FLAG_NEXT_CTRL`` flag. The ``VIDIOC_QUERYCTRL`` ioctl will
return the first control with a higher ID than the specified one. When
no such controls are found an error is returned.
(初始控制 ID 被设置为 0 与 ``V4L2_CTRL_FLAG_NEXT_CTRL`` 标志的按位或。 ``VIDIOC_QUERYCTRL`` ioctl 将返回 ID 高于指定值的第一个控制。如果没有找到这样的控制，则返回错误。)

If you want to get all controls within a specific control class, then
you can set the initial ``qctrl.id`` value to the control class and add
an extra check to break out of the loop when a control of another
control class is found:
(如果你想获取特定控制类中的所有控制，可以将初始 ``qctrl.id`` 值设置为该控制类，并添加一个额外的检查，当发现另一个控制类的控制时跳出循环：)


.. code-block:: c

    qctrl.id = V4L2_CTRL_CLASS_CODEC | V4L2_CTRL_FLAG_NEXT_CTRL;
    while (0 == ioctl(fd, VIDIOC_QUERYCTRL, &qctrl)) {
	if (V4L2_CTRL_ID2CLASS(qctrl.id) != V4L2_CTRL_CLASS_CODEC)
	    break;
	/* ... */
	qctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
    }

The 32-bit ``qctrl.id`` value is subdivided into three bit ranges: the
top 4 bits are reserved for flags (e. g. ``V4L2_CTRL_FLAG_NEXT_CTRL``)
and are not actually part of the ID. The remaining 28 bits form the
control ID, of which the most significant 12 bits define the control
class and the least significant 16 bits identify the control within the
control class. It is guaranteed that these last 16 bits are always
non-zero for controls. The range of 0x1000 and up are reserved for
driver-specific controls. The macro ``V4L2_CTRL_ID2CLASS(id)`` returns
the control class ID based on a control ID.
(32 位的 ``qctrl.id`` 值细分为三个位段：最高的 4 位保留给标志(例如 ``V4L2_CTRL_FLAG_NEXT_CTRL`` )，实际上并不是 ID 的一部分。其余 28 位构成控制 ID，其中最高有效的 12 位定义控制类，最低有效的 16 位标识控制类中的控制。保证对于控制而言，这最后 16 位始终非零。0x1000 及以上的范围保留给驱动特定的控制。宏 ``V4L2_CTRL_ID2CLASS(id)`` 根据控制 ID 返回控制类 ID。)

If the driver does not support extended controls, then
``VIDIOC_QUERYCTRL`` will fail when used in combination with
``V4L2_CTRL_FLAG_NEXT_CTRL``. In that case the old method of enumerating
control should be used (see :ref:`enum_all_controls`). But if it is
supported, then it is guaranteed to enumerate over all controls,
including driver-private controls.
(如果驱动不支持扩展控制，则将 ``VIDIOC_QUERYCTRL`` 与 ``V4L2_CTRL_FLAG_NEXT_CTRL`` 结合使用时会失败。在这种情况下，应使用旧的枚举控制方法(参见 :ref:`enum_all_controls` )。但如果支持，则保证会枚举所有控制，包括驱动私有的控制。)


Creating Control Panels(创建控制面板)
=====================================

It is possible to create control panels for a graphical user interface
where the user can select the various controls. Basically you will have
to iterate over all controls using the method described above. Each
control class starts with a control of type
``V4L2_CTRL_TYPE_CTRL_CLASS``. ``VIDIOC_QUERYCTRL`` will return the name
of this control class which can be used as the title of a tab page
within a control panel.
(可以为图形用户界面创建控制面板，让用户选择各种控制。基本上，你必须使用上述方法遍历所有控制。每个控制类都以一个 ``V4L2_CTRL_TYPE_CTRL_CLASS`` 类型的控制开始。 ``VIDIOC_QUERYCTRL`` 将返回该控制类的名称，它可以用作控制面板中标签页的标题。)

The flags field of struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` also
contains hints on the behavior of the control. See the
:ref:`VIDIOC_QUERYCTRL` documentation for more
details.
(struct :ref:`v4l2_queryctrl <v4l2-queryctrl>` 的 flags 字段还包含关于控制行为的提示。更多细节请参阅 :ref:`VIDIOC_QUERYCTRL` 文档。)
