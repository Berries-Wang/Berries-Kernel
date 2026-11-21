.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _media-controller-model:

Media device model(媒体设备模型)
================================

Discovering a device internal topology, and configuring it at runtime,
is one of the goals of the media controller API. To achieve this,
hardware devices and Linux Kernel interfaces are modelled as graph
objects on an oriented graph. The object types that constitute the graph
are:
(发现设备内部拓扑并在运行时对其进行配置，是媒体控制器 API 的目标之一。为
此，硬件设备和 Linux 内核接口被建模为有向图上的图对象。构成该图的对象类型
有：)

-  An **entity** is a basic media hardware or software building block.
   It can correspond to a large variety of logical blocks such as
   physical hardware devices (CMOS sensor for instance), logical
   hardware devices (a building block in a System-on-Chip image
   processing pipeline), DMA channels or physical connectors.
   (**实体** （entity）是基本的媒体硬件或软件构建块。它可以对应多种多样的
   逻辑块，如物理硬件设备（例如 CMOS 传感器）、逻辑硬件设备（片上系统图像
   处理流水线中的一个构建块）、DMA 通道或物理连接器。)

-  An **interface** is a graph representation of a Linux Kernel
   userspace API interface, like a device node or a sysfs file that
   controls one or more entities in the graph.
   (**接口** （interface）是 Linux 内核用户空间 API 接口的图表示，例如设备
   节点或控制图中一个或多个实体的 sysfs 文件。)

-  A **pad** is a data connection endpoint through which an entity can
   interact with other entities. Data (not restricted to video) produced
   by an entity flows from the entity's output to one or more entity
   inputs. Pads should not be confused with physical pins at chip
   boundaries.
   (**pad** 是一个数据连接端点，实体通过它与其他实体交互。实体产生的数据
   （不限于视频）从实体的输出流向一个或多个实体输入。pad 不应与芯片边界上
   的物理引脚相混淆。)

-  A **data link** is a point-to-point oriented connection between two
   pads, either on the same entity or on different entities. Data flows
   from a source pad to a sink pad.
   (**数据链路** （data link）是两个 pad 之间的点对点有向连接——可在同一
   实体上，也可在不同实体之间。数据从源 pad 流向宿 pad。)

-  An **interface link** is a point-to-point bidirectional control
   connection between a Linux Kernel interface and an entity.
   (**接口链路** （interface link）是 Linux 内核接口与实体之间的点对点双向
   控制连接。)

- An **ancillary link** is a point-to-point connection denoting that two
  entities form a single logical unit. For example this could represent the
  fact that a particular camera sensor and lens controller form a single
  physical module, meaning this lens controller drives the lens for this
  camera sensor.
  (**辅助链路** （ancillary link）是一种点对点连接，表示两个实体构成一个
  单一逻辑单元。例如，它可以表示某个摄像头传感器与镜头控制器构成单一物理
  模块这一事实，即该镜头控制器为该摄像头传感器驱动镜头。)
