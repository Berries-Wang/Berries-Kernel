.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _VIDIOC_SUBSCRIBE_EVENT:
.. _VIDIOC_UNSUBSCRIBE_EVENT:

******************************************************
ioctl VIDIOC_SUBSCRIBE_EVENT, VIDIOC_UNSUBSCRIBE_EVENT
******************************************************

Name(名称)
==========

VIDIOC_SUBSCRIBE_EVENT - VIDIOC_UNSUBSCRIBE_EVENT - Subscribe or unsubscribe event
(VIDIOC_SUBSCRIBE_EVENT - VIDIOC_UNSUBSCRIBE_EVENT - 订阅或取消订阅事件。)

Synopsis(概要)
==============

.. c:macro:: VIDIOC_SUBSCRIBE_EVENT

``int ioctl(int fd, VIDIOC_SUBSCRIBE_EVENT, struct v4l2_event_subscription *argp)``

.. c:macro:: VIDIOC_UNSUBSCRIBE_EVENT

``int ioctl(int fd, VIDIOC_UNSUBSCRIBE_EVENT, struct v4l2_event_subscription *argp)``

Arguments(参数)
===============

``fd``
    File descriptor returned by :c:func:`open()`.
    (由 :c:func:`open()` 返回的文件描述符。)

``argp``
    Pointer to struct :c:type:`v4l2_event_subscription`.
    (指向 struct :c:type:`v4l2_event_subscription` 的指针。)

Description(描述)
=================

Subscribe or unsubscribe V4L2 event. Subscribed events are dequeued by
using the :ref:`VIDIOC_DQEVENT` ioctl.
(订阅或取消订阅 V4L2 事件。已订阅的事件通过 :ref:`VIDIOC_DQEVENT` ioctl 出队。)

.. tabularcolumns:: |p{2.6cm}|p{4.4cm}|p{10.3cm}|

.. c:type:: v4l2_event_subscription

.. flat-table:: struct v4l2_event_subscription
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``type``
      - Type of the event, see :ref:`event-type`.
        (事件类型，参见 :ref:`event-type` 。)

	.. note::

	   ``V4L2_EVENT_ALL`` can be used with
	   :ref:`VIDIOC_UNSUBSCRIBE_EVENT <VIDIOC_SUBSCRIBE_EVENT>` for
	   unsubscribing all events at once.
	   (``V4L2_EVENT_ALL`` 可与 :ref:`VIDIOC_UNSUBSCRIBE_EVENT <VIDIOC_SUBSCRIBE_EVENT>` 一起使用，一次取消订阅所有事件。)
    * - __u32
      - ``id``
      - ID of the event source. If there is no ID associated with the
	event source, then set this to 0. Whether or not an event needs an
	ID depends on the event type.
        (事件源的 ID。如果事件源没有关联的 ID，则将其设置为 0。事件是否需要 ID 取决于事件类型。)
    * - __u32
      - ``flags``
      - Event flags, see :ref:`event-flags`.
        (事件标志，参见 :ref:`event-flags` 。)
    * - __u32
      - ``reserved``\ [5]
      - Reserved for future extensions. Drivers and applications must set
	the array to zero.
        (保留用于未来的扩展。驱动程序和应用程序必须把该数组置零。)


.. tabularcolumns:: |p{7.5cm}|p{2.0cm}|p{7.8cm}|

.. _event-flags:

.. flat-table:: Event Flags
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_EVENT_SUB_FL_SEND_INITIAL``
      - 0x0001
      - When this event is subscribed an initial event will be sent
	containing the current status. This only makes sense for events
	that are triggered by a status change such as ``V4L2_EVENT_CTRL``.
	Other events will ignore this flag.
        (订阅此事件时，将发送一个包含当前状态的初始事件。这只对由状态变化触发的事件（例如 ``V4L2_EVENT_CTRL`` ）有意义。其他事件将忽略此标志。)
    * - ``V4L2_EVENT_SUB_FL_ALLOW_FEEDBACK``
      - 0x0002
      - If set, then events directly caused by an ioctl will also be sent
	to the filehandle that called that ioctl. For example, changing a
	control using :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` will cause
	a V4L2_EVENT_CTRL to be sent back to that same filehandle.
	Normally such events are suppressed to prevent feedback loops
	where an application changes a control to a one value and then
	another, and then receives an event telling it that that control
	has changed to the first value.
        (如果设置，则由 ioctl 直接引起的事件也会被发送给调用该 ioctl 的文件句柄。例如，使用 :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` 更改控件会导致 V4L2_EVENT_CTRL 被发送回同一个文件句柄。通常这类事件会被抑制，以防止出现这样的反馈循环：应用程序把一个控件改为某个值，然后又改为另一个值，随后收到一个事件告知它该控件已变回第一个值。)

	Since it can't tell whether that event was caused by another
	application or by the :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>`
	call it is hard to decide whether to set the control to the value
	in the event, or ignore it.
        (由于无法判断该事件是由另一个应用程序还是由 :ref:`VIDIOC_S_CTRL <VIDIOC_G_CTRL>` 调用引起的，很难决定是把控件设置为事件中的值，还是忽略它。)

	Think carefully when you set this flag so you won't get into
	situations like that.
        (设置此标志时请仔细考虑，以免陷入类似的情况。)

Return Value(返回值)
====================

On success 0 is returned, on error -1 and the ``errno`` variable is set
appropriately. The generic error codes are described at the
:ref:`Generic Error Codes <gen-errors>` chapter.
(成功时返回 0，出错时返回 -1 并适当设置 ``errno`` 变量。通用错误码在 :ref:`Generic Error Codes <gen-errors>` 一章中描述。)
