.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _dv-timings:

**********************************************
Digital Video (DV) Timings(数字视频 (DV) 时序)
**********************************************

The video standards discussed so far have been dealing with Analog TV
and the corresponding video timings. Today there are many more different
hardware interfaces such as High Definition TV interfaces (HDMI), VGA,
DVI connectors etc., that carry video signals and there is a need to
extend the API to select the video timings for these interfaces. Since
it is not possible to extend the :ref:`v4l2_std_id <v4l2-std-id>`
due to the limited bits available, a new set of ioctls was added to
set/get video timings at the input and output.
(前面讨论的视频标准都是针对模拟电视及相应的视频时序。如今有许多不同的
硬件接口——例如高清电视接口 (HDMI)、VGA、DVI 连接器等——都在传送视频
信号，因此需要扩展 API 来为这些接口选择视频时序。由于可用位数有限，
无法扩展 :ref:`v4l2_std_id <v4l2-std-id>` ，于是在输入和输出上新增了一组
用于设置/获取视频时序的 ioctl。)

These ioctls deal with the detailed digital video timings that define
each video format. This includes parameters such as the active video
width and height, signal polarities, frontporches, backporches, sync
widths etc. The ``linux/v4l2-dv-timings.h`` header can be used to get
the timings of the formats in the :ref:`cea861` and :ref:`vesadmt`
standards.
(这些 ioctl 处理定义每种视频格式的详细数字视频时序。包括有效视频宽度与
高度、信号极性、前沿 (frontporch)、后沿 (backporch)、同步宽度等参数。
``linux/v4l2-dv-timings.h`` 头文件可用于获取 :ref:`cea861` 和
:ref:`vesadmt` 标准中各格式的时序。)

To enumerate and query the attributes of the DV timings supported by a
device applications use the
:ref:`VIDIOC_ENUM_DV_TIMINGS` and
:ref:`VIDIOC_DV_TIMINGS_CAP` ioctls. To set
DV timings for the device applications use the
:ref:`VIDIOC_S_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>` ioctl and to get
current DV timings they use the
:ref:`VIDIOC_G_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>` ioctl. To detect
the DV timings as seen by the video receiver applications use the
:ref:`VIDIOC_QUERY_DV_TIMINGS` ioctl.
(要枚举并查询设备支持的 DV 时序的属性，应用使用
:ref:`VIDIOC_ENUM_DV_TIMINGS` 和 :ref:`VIDIOC_DV_TIMINGS_CAP` ioctl。要为
设备设置 DV 时序，应用使用 :ref:`VIDIOC_S_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>`
ioctl；要获取当前 DV 时序，使用 :ref:`VIDIOC_G_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>`
ioctl。要检测视频接收端看到的 DV 时序，应用使用
:ref:`VIDIOC_QUERY_DV_TIMINGS` ioctl。)

When the hardware detects a video source change (e.g. the video
signal appears or disappears, or the video resolution changes), then
it will issue a `V4L2_EVENT_SOURCE_CHANGE` event. Use the
:ref:`ioctl VIDIOC_SUBSCRIBE_EVENT <VIDIOC_SUBSCRIBE_EVENT>` and the
:ref:`VIDIOC_DQEVENT` to check if this event was reported.
(当硬件检测到视频源变化 (例如视频信号出现或消失，或视频分辨率改变) 时，
它会发出 `V4L2_EVENT_SOURCE_CHANGE` 事件。使用
:ref:`ioctl VIDIOC_SUBSCRIBE_EVENT <VIDIOC_SUBSCRIBE_EVENT>` 和
:ref:`VIDIOC_DQEVENT` 来检查是否报告了此事件。)

If the video signal changed, then the application has to stop
streaming, free all buffers, and call the :ref:`VIDIOC_QUERY_DV_TIMINGS`
to obtain the new video timings, and if they are valid, it can set
those by calling the :ref:`ioctl VIDIOC_S_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>`.
This will also update the format, so use the :ref:`ioctl VIDIOC_G_FMT <VIDIOC_G_FMT>`
to obtain the new format. Now the application can allocate new buffers
and start streaming again.
(如果视频信号发生了变化，应用必须停止流式传输、释放所有缓冲区，并调用
:ref:`VIDIOC_QUERY_DV_TIMINGS` 获取新的视频时序；如果它们有效，可以通过
调用 :ref:`ioctl VIDIOC_S_DV_TIMINGS <VIDIOC_G_DV_TIMINGS>` 设置它们。这也
会更新格式，因此要使用 :ref:`ioctl VIDIOC_G_FMT <VIDIOC_G_FMT>` 获取新
格式。此后应用就可以分配新缓冲区并重新开始流式传输。)

The :ref:`VIDIOC_QUERY_DV_TIMINGS` will just report what the
hardware detects, it will never change the configuration. If the
currently set timings and the actually detected timings differ, then
typically this will mean that you will not be able to capture any
video. The correct approach is to rely on the `V4L2_EVENT_SOURCE_CHANGE`
event so you know when something changed.
(:ref:`VIDIOC_QUERY_DV_TIMINGS` 只报告硬件检测到的内容，绝不会更改配置。
如果当前设置的时序与实际检测到的时序不一致，通常意味着将无法采集到任何
视频。正确的做法是依赖 `V4L2_EVENT_SOURCE_CHANGE` 事件，这样你就能知道
何时发生了变化。)

Applications can make use of the :ref:`input-capabilities` and
:ref:`output-capabilities` flags to determine whether the digital
video ioctls can be used with the given input or output.
(应用可以利用 :ref:`input-capabilities` 和 :ref:`output-capabilities`
标志来确定数字视频 ioctl 能否用于给定的输入或输出。)
