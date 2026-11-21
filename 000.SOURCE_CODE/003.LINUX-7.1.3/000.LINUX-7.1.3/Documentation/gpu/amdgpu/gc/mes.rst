.. _amdgpu-mes:

================================================
 MicroEngine Scheduler (MES)(微引擎调度器 (MES))
================================================

.. note::
   Queue and ring buffer are used as a synonymous.
   (队列（queue）与环形缓冲区（ring buffer）在此作为同义词使用。)

.. note::
   This section assumes that you are familiar with the concept of Pipes, Queues, and GC.
   If not, check :ref:`GFX, Compute, and SDMA Overall Behavior<pipes-and-queues-description>`
   and :ref:`drm/amdgpu - Graphics and Compute (GC) <amdgpu-gc>`.
   (本节假设你已经熟悉管道（Pipe）、队列（Queue）与 GC 的概念。若不熟悉，
   请参阅 :ref:`GFX, Compute, and SDMA Overall
   Behavior<pipes-and-queues-description>` 和 :ref:`drm/amdgpu - Graphics
   and Compute (GC) <amdgpu-gc>`。)

Every GFX has a pipe component with one or more hardware queues. Pipes can
switch between queues depending on certain conditions, and one of the
components that can request a queue switch to a pipe is the MicroEngine
Scheduler (MES). Whenever the driver is initialized, it creates one MQD per
hardware queue, and then the MQDs are handed to the MES firmware for mapping
to:
(每个 GFX 都有一个带一个或多个硬件队列的管道（pipe）组件。管道可根据特定
条件在队列之间切换，而可以请求管道切换队列的组件之一就是微引擎调度器
（MES）。每当驱动初始化时，它会为每个硬件队列创建一个 MQD，然后将这些 MQD
交给 MES 固件进行映射，映射目标为：)

1. Kernel Queues (legacy): This queue is statically mapped to HQDs and never
   preempted. Even though this is a legacy feature, it is the current default, and
   most existing hardware supports it. When an application submits work to the
   kernel driver, it submits all of the application command buffers to the kernel
   queues. The CS IOCTL takes the command buffer from the applications and
   schedules them on the kernel queue.
   (内核队列（旧式）：该队列静态映射到 HQD，且从不被抢占。尽管这是旧式特性，
   它仍是当前的默认方式，大多数现有硬件都支持它。当应用程序向内核驱动提交
   工作时，它把应用程序的所有命令缓冲区提交到内核队列。CS IOCTL 从应用程序
   取出命令缓冲区并把它们调度到内核队列上。)

2. User Queues: These queues are dynamically mapped to the HQDs. Regarding the
   utilization of User Queues, the userspace application will create its user
   queues and submit work directly to its user queues with no need to IOCTL for
   each submission and no need to share a single kernel queue.
   (用户队列：这些队列动态映射到 HQD。在使用用户队列时，用户空间应用程序会
   创建自己的用户队列并直接向其提交工作，无需每次提交都进行 IOCTL，也无需
   共享单个内核队列。)

In terms of User Queues, MES can dynamically map them to the HQD. If there are
more MQDs than HQDs, the MES firmware will preempt other user queues to make
sure each queues get a time slice; in other words, MES is a microcontroller
that handles the mapping and unmapping of MQDs into HQDs, as well as the
priorities and oversubscription of MQDs.
(对于用户队列，MES 可以把它们动态映射到 HQD。如果 MQD 数量多于 HQD，MES
固件会抢占其他用户队列，确保每个队列都能获得时间片；换言之，MES 是一个微
控制器，负责 MQD 到 HQD 的映射与解除映射，以及 MQD 的优先级和超额订阅
（oversubscription）的处理。)
