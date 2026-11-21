.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _video:

****************************************
Video Inputs and Outputs(视频输入与输出)
****************************************

Video inputs and outputs are physical connectors of a device. These can
be for example: RF connectors (antenna/cable), CVBS a.k.a. Composite
Video, S-Video and RGB connectors. Camera sensors are also considered to
be a video input. Video and VBI capture devices have inputs. Video and
VBI output devices have outputs, at least one each. Radio devices have
no video inputs or outputs.
(视频输入与输出是设备的物理连接器。例如可以是：RF 连接器 (天线/有线
电)、CVBS (又称 Composite Video)、S-Video 和 RGB 连接器。相机传感器也
被视为视频输入。视频与 VBI 捕获设备有输入。视频与 VBI 输出设备有输
出，至少各一个。收音设备没有视频输入或输出。)

To learn about the number and attributes of the available inputs and
outputs applications can enumerate them with the
:ref:`VIDIOC_ENUMINPUT` and
:ref:`VIDIOC_ENUMOUTPUT` ioctl, respectively. The
struct :c:type:`v4l2_input` returned by the
:ref:`VIDIOC_ENUMINPUT` ioctl also contains signal
status information applicable when the current video input is queried.
(要了解可用输入与输出的数量和属性，应用程序可以分别用
:ref:`VIDIOC_ENUMINPUT` 与 :ref:`VIDIOC_ENUMOUTPUT` ioctl 来枚举它们。
由 :ref:`VIDIOC_ENUMINPUT` ioctl 返回的 struct :c:type:`v4l2_input` 还
包含信号状态信息，适用于查询当前视频输入时使用。)

The :ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` and
:ref:`VIDIOC_G_OUTPUT <VIDIOC_G_OUTPUT>` ioctls return the index of
the current video input or output. To select a different input or output
applications call the :ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>` and
:ref:`VIDIOC_S_OUTPUT <VIDIOC_G_OUTPUT>` ioctls. Drivers must
implement all the input ioctls when the device has one or more inputs,
all the output ioctls when the device has one or more outputs.
( :ref:`VIDIOC_G_INPUT <VIDIOC_G_INPUT>` 与
:ref:`VIDIOC_G_OUTPUT <VIDIOC_G_OUTPUT>` ioctl 返回当前视频输入或输出
的索引。要选择不同的输入或输出，应用程序调用
:ref:`VIDIOC_S_INPUT <VIDIOC_G_INPUT>` 与
:ref:`VIDIOC_S_OUTPUT <VIDIOC_G_OUTPUT>` ioctl。当设备有一个或多个输
入时，驱动程序必须实现所有输入 ioctl；当设备有一个或多个输出时，必
须实现所有输出 ioctl。)

Example: Information about the current video input(示例：当前视频输入的信息)
============================================================================

.. code-block:: c

    struct v4l2_input input;
    int index;

    if (-1 == ioctl(fd, VIDIOC_G_INPUT, &index)) {
	perror("VIDIOC_G_INPUT");
	exit(EXIT_FAILURE);
    }

    memset(&input, 0, sizeof(input));
    input.index = index;

    if (-1 == ioctl(fd, VIDIOC_ENUMINPUT, &input)) {
	perror("VIDIOC_ENUMINPUT");
	exit(EXIT_FAILURE);
    }

    printf("Current input: %s\\n", input.name);


Example: Switching to the first video input(示例：切换到第一个视频输入)
=======================================================================

.. code-block:: c

    int index;

    index = 0;

    if (-1 == ioctl(fd, VIDIOC_S_INPUT, &index)) {
	perror("VIDIOC_S_INPUT");
	exit(EXIT_FAILURE);
    }
