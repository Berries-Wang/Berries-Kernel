.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _event:

*************************
Event Interface(事件接口)
*************************

The V4L2 event interface provides a means for a user to get immediately
notified on certain conditions taking place on a device. This might
include start of frame or loss of signal events, for example. Changes in
the value or state of a V4L2 control can also be reported through
events.
(V4L2 事件接口为用户提供了一种在设备上发生某些情况时立即得到通知的手段。
例如，这可能包括帧开始或信号丢失事件。V4L2 控件的值或状态的变化也可以
通过事件上报。)

To receive events, the events the user is interested in first must be
subscribed using the
:ref:`VIDIOC_SUBSCRIBE_EVENT` ioctl. Once
an event is subscribed, the events of subscribed types are dequeueable
using the :ref:`VIDIOC_DQEVENT` ioctl. Events may be
unsubscribed using VIDIOC_UNSUBSCRIBE_EVENT ioctl. The special event
type V4L2_EVENT_ALL may be used to unsubscribe all the events the
driver supports.
(要接收事件，必须先用 :ref:`VIDIOC_SUBSCRIBE_EVENT` ioctl 订阅用户感兴趣的
事件。事件一经订阅，就可以用 :ref:`VIDIOC_DQEVENT` ioctl 将已订阅类型的
事件出队。可以使用 VIDIOC_UNSUBSCRIBE_EVENT ioctl 取消订阅。特殊事件类型
V4L2_EVENT_ALL 可用于取消订阅驱动支持的所有事件。)

The event subscriptions and event queues are specific to file handles.
Subscribing an event on one file handle does not affect other file
handles.
(事件订阅和事件队列是针对文件句柄而言的。在一个文件句柄上订阅事件不会
影响其他文件句柄。)

The information on dequeueable events is obtained by using select or
poll system calls on video devices. The V4L2 events use POLLPRI events
on poll system call and exceptions on select system call.
(通过对视频设备使用 select 或 poll 系统调用可以获得可出队事件的信息。
V4L2 事件在 poll 系统调用中使用 POLLPRI 事件，在 select 系统调用中使用
异常 (exception) 集合。)

Starting with kernel 3.1 certain guarantees can be given with regards to
events:
(从内核 3.1 开始，可以对事件提供某些保证：)

1. Each subscribed event has its own internal dedicated event queue.
   This means that flooding of one event type will not interfere with
   other event types.
   (每个已订阅的事件都有自己的内部专用事件队列。这意味着某一种事件类型的
   泛洪不会干扰其他事件类型。)

2. If the internal event queue for a particular subscribed event becomes
   full, then the oldest event in that queue will be dropped.
   (如果某个特定已订阅事件的内部事件队列变满，则该队列中最旧的事件将被
   丢弃。)

3. Where applicable, certain event types can ensure that the payload of
   the oldest event that is about to be dropped will be merged with the
   payload of the next oldest event. Thus ensuring that no information
   is lost, but only an intermediate step leading up to that
   information. See the documentation for the event you want to
   subscribe to whether this is applicable for that event or not.
   (在适用的情况下，某些事件类型可以保证：即将被丢弃的最旧事件的载荷会与
   次旧事件的载荷合并。从而确保不丢失信息，只丢失通往该信息的一个中间
   步骤。请查阅你想订阅的事件的文档，确认该事件是否适用。)
