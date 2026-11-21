.. SPDX-License-Identifier: GPL-2.0

======================================================================
Debugging and tracing in the media subsystem(媒体子系统中的调试与跟踪)
======================================================================

This document serves as a starting point and lookup for debugging device
drivers in the media subsystem and to debug these drivers from userspace.
(本文档是调试媒体子系统设备驱动、以及从用户空间调试这些驱动的起点与查阅手册。)

.. contents::
    :depth: 3

General debugging advice(一般调试建议)
--------------------------------------

For general advice see the :doc:`general advice document
</process/debugging/index>`.
(一般建议见 :doc:`一般建议文档 </process/debugging/index>`。)

The following sections show you some of the available tools.
(下面几节介绍其中一些可用的工具。)

dev_debug module parameter(dev_debug 模块参数)
----------------------------------------------

Every video device provides a ``dev_debug`` parameter, which allows to get
further insights into the IOCTLs in the background.::

  # cat /sys/class/video4linux/video3/name
  rkvdec
  # echo 0xff > /sys/class/video4linux/video3/dev_debug
  # dmesg -wH
  [...] videodev: v4l2_open: video3: open (0)
  [  +0.000036] video3: VIDIOC_QUERYCAP: driver=rkvdec, card=rkvdec,
  bus=platform:rkvdec, version=0x00060900, capabilities=0x84204000,
  device_caps=0x04204000

(每个视频设备都提供 ``dev_debug`` 参数，可用来深入了解后台发生的 IOCTL。)

For the full documentation see :ref:`driver-api/media/v4l2-dev:video device
debugging`
(完整文档见 :ref:`driver-api/media/v4l2-dev:video device debugging`。)

dev_dbg() / v4l2_dbg()(两种调试打印)
------------------------------------

Two debug print statements, which are specific for devices and for the v4l2
subsystem, avoid adding these to your final submission unless they have
long-term value for investigations.
(这是两条特定于设备与 v4l2 子系统的调试打印语句；除非它们对排查问题有长期价值，否则不要把它们加进最终提交。)

For a general overview please see the
:ref:`process/debugging/driver_development_debugging_guide:printk() & friends`
guide.
(总体概览请参阅 :ref:`process/debugging/driver_development_debugging_guide:printk() & friends` 指南。)

- Difference between both?
  (两者有何区别？)

  - v4l2_dbg() utilizes v4l2_printk() under the hood, which further uses
    printk() directly, thus it cannot be targeted by dynamic debug
    (v4l2_dbg() 底层使用 v4l2_printk()，后者又直接使用 printk()，因此无法被 dynamic debug 定向启用)
  - dev_dbg() can be targeted by dynamic debug
    (dev_dbg() 可以被 dynamic debug 定向启用)
  - v4l2_dbg() has a more specific prefix format for the media subsystem, while
    dev_dbg only highlights the driver name and the location of the log
    (v4l2_dbg() 对媒体子系统使用更具体的前缀格式，而 dev_dbg 只突出驱动名称与日志位置)

Dynamic debug(动态调试)
-----------------------

A method to trim down the debug output to your needs.
(一种把调试输出裁剪到所需范围的方法。)

For general advice see the
:ref:`process/debugging/userspace_debugging_guide:dynamic debug` guide.
(一般建议见 :ref:`process/debugging/userspace_debugging_guide:dynamic debug` 指南。)

Here is one example, that enables all available pr_debug()'s within the file::

  $ alias ddcmd='echo $* > /proc/dynamic_debug/control'
  $ ddcmd '-p; file v4l2-h264.c +p'
  $ grep =p /proc/dynamic_debug/control
   drivers/media/v4l2-core/v4l2-h264.c:372 [v4l2_h264]print_ref_list_b =p
   "ref_pic_list_b%u (cur_poc %u%c) %s"
   drivers/media/v4l2-core/v4l2-h264.c:333 [v4l2_h264]print_ref_list_p =p
   "ref_pic_list_p (cur_poc %u%c) %s\n"

(下面这个例子启用该文件中所有可用的 pr_debug()：)

Ftrace(Ftrace 跟踪)
-------------------

An internal kernel tracer that can trace static predefined events, function
calls, etc. Very useful for debugging problems without changing the kernel and
understanding the behavior of subsystems.
(内核内部跟踪器，可跟踪静态预定义事件、函数调用等。在不改动内核的情况下调试问题、理解子系统行为时非常有用。)

For general advice see the
:ref:`process/debugging/userspace_debugging_guide:ftrace` guide.
(一般建议见 :ref:`process/debugging/userspace_debugging_guide:ftrace` 指南。)

DebugFS(DebugFS 调试文件系统)
-----------------------------

This tool allows you to dump or modify internal values of your driver to files
in a custom filesystem.
(该工具让你把驱动的内部值转储或修改到自定义文件系统中的文件。)

For general advice see the
:ref:`process/debugging/driver_development_debugging_guide:debugfs` guide.
(一般建议见 :ref:`process/debugging/driver_development_debugging_guide:debugfs` 指南。)

Perf & alternatives(Perf 及替代工具)
------------------------------------

Tools to measure the various stats on a running system to diagnose issues.
(在运行中的系统上测量各种统计数据以诊断问题的工具。)

For general advice see the
:ref:`process/debugging/userspace_debugging_guide:perf & alternatives` guide.
(一般建议见 :ref:`process/debugging/userspace_debugging_guide:perf & alternatives` 指南。)

Example for media devices:
(媒体设备示例：)

Gather statistics data for a decoding job: (This example is on a RK3399 SoC
with the rkvdec codec driver using the `fluster test suite
<https://github.com/fluendo/fluster>`__)::

  perf stat -d python3 fluster.py run -d GStreamer-H.264-V4L2SL-Gst1.0 -ts
  JVT-AVC_V1 -tv AUD_MW_E -j1
  ...
  Performance counter stats for 'python3 fluster.py run -d
  GStreamer-H.264-V4L2SL-Gst1.0 -ts JVT-AVC_V1 -tv AUD_MW_E -j1 -v':

         7794.23 msec task-clock:u                     #    0.697 CPUs utilized
               0      context-switches:u               #    0.000 /sec
               0      cpu-migrations:u                 #    0.000 /sec
           11901      page-faults:u                    #    1.527 K/sec
       882671556      cycles:u                         #    0.113 GHz                         (95.79%)
       711708695      instructions:u                   #    0.81  insn per cycle              (95.79%)
        10581935      branches:u                       #    1.358 M/sec                       (15.13%)
         6871144      branch-misses:u                  #   64.93% of all branches             (95.79%)
       281716547      L1-dcache-loads:u                #   36.144 M/sec                       (95.79%)
         9019581      L1-dcache-load-misses:u          #    3.20% of all L1-dcache accesses   (95.79%)
 <not supported>      LLC-loads:u
 <not supported>      LLC-load-misses:u

    11.180830431 seconds time elapsed

     1.502318000 seconds user
     6.377221000 seconds sys

(收集一次解码作业的统计数据：本例在 RK3399 SoC 上进行，使用 rkvdec 编解码驱动与 fluster 测试套件(https://github.com/fluendo/fluster)。)

The availability of events and metrics depends on the system you are running.
(事件与指标的可用性取决于你运行的系统。)

Error checking & panic analysis(错误检查与 panic 分析)
------------------------------------------------------

Various Kernel configuration options to enhance error detection of the Linux
Kernel with the cost of lowering performance.
(各种用于增强 Linux 内核错误检测能力的内核配置选项，代价是降低性能。)

For general advice see the
:ref:`process/debugging/driver_development_debugging_guide:kasan, ubsan,
lockdep and other error checkers` guide.
(一般建议见 :ref:`process/debugging/driver_development_debugging_guide:kasan, ubsan, lockdep and other error checkers` 指南。)

Driver verification with v4l2-compliance(用 v4l2-compliance 验证驱动)
---------------------------------------------------------------------

To verify, that a driver adheres to the v4l2 API, the tool v4l2-compliance is
used, which is part of the `v4l_utils
<https://git.linuxtv.org/v4l-utils.git>`__, a suite of userspace tools to work
with the media subsystem.
(为验证驱动是否符合 v4l2 API，需要使用 v4l2-compliance 工具。它是 v4l_utils(https://git.linuxtv.org/v4l-utils.git，一套操作媒体子系统的用户空间工具)的一部分。)

To see the detailed media topology (and check it) use::

  v4l2-compliance -M /dev/mediaX --verbose

(要查看并检查详细的媒体拓扑，使用：)

You can also run a full compliance check for all devices referenced in the
media topology with::

  v4l2-compliance -m /dev/mediaX

(也可以对媒体拓扑中引用的所有设备运行完整一致性检查，使用：)

Debugging problems with receiving video(接收视频问题的调试)
-----------------------------------------------------------

Implementing vidioc_log_status in the driver: this can log the current status
to the kernel log. It's called by v4l2-ctl --log-status. Very useful for
debugging problems with receiving video (TV/S-Video/HDMI/etc) since the video
signal is external (so unpredictable). Less useful with camera sensor inputs
since you have control over what the camera sensor does.
(在驱动中实现 vidioc_log_status：它可以把当前状态记录到内核日志，由 v4l2-ctl --log-status 调用。对调试视频接收问题(TV/S-Video/HDMI 等)非常有用，因为视频信号来自外部(因而不可预测)。对摄像头传感器输入用处较小，因为摄像头传感器的行为由你掌控。)

Usually you can just assign the default::

  .vidioc_log_status  = v4l2_ctrl_log_status,

(通常直接指定默认实现即可：)

But you can also create your own callback, to create a custom status log.
(也可以创建自己的回调，生成自定义的状态日志。)

You can find an example in the cobalt driver
(`drivers/media/pci/cobalt/cobalt-v4l2.c <https://elixir.bootlin.com/linux/v6.11.6/source/drivers/media/pci/cobalt/cobalt-v4l2.c#L567>`__).
(cobalt 驱动中有一个示例，见 drivers/media/pci/cobalt/cobalt-v4l2.c(https://elixir.bootlin.com/linux/v6.11.6/source/drivers/media/pci/cobalt/cobalt-v4l2.c#L567)。)

**Copyright** ©2024 : Collabora
(**版权所有** ©2024 : Collabora)
