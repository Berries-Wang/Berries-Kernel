.. SPDX-License-Identifier: GPL-2.0

PSE Power Interface (PSE PI) Documentation
==========================================

(PSE 电源接口（PSE PI）文档)

The Power Sourcing Equipment Power Interface (PSE PI) plays a pivotal role in
the architecture of Power over Ethernet (PoE) systems. It is essentially a
blueprint that outlines how one or multiple power sources are connected to the
eight-pin modular jack, commonly known as the Ethernet RJ45 port. This
connection scheme is crucial for enabling the delivery of power alongside data
over Ethernet cables.
(供电设备电源接口（PSE PI）在以太网供电（PoE）系统架构中扮演着举足轻重的
角色。它本质上是一份蓝图，描述一个或多个电源如何连接到八针模块化插孔
——即通常所说的以太网 RJ45 端口。这种连接方案对于在以太网线缆上实现
电力与数据并行传输至关重要。)

Documentation and Standards
(文档与标准)
---------------------------

The IEEE 802.3 standard provides detailed documentation on the PSE PI.
Specifically:
(IEEE 802.3 标准提供了关于 PSE PI 的详细文档。具体而言：)

- Section "33.2.3 PI pin assignments" covers the pin assignments for PoE
  systems that utilize two pairs for power delivery.
  ("33.2.3 PI pin assignments" 一节涵盖使用两对线供电的 PoE 系统的引脚
  分配。)
- Section "145.2.4 PSE PI" addresses the configuration for PoE systems that
  deliver power over all four pairs of an Ethernet cable.
  ("145.2.4 PSE PI" 一节论述通过以太网线缆全部四对线供电的 PoE 系统的
  配置。)

PSE PI and Single Pair Ethernet
(PSE PI 与单对以太网)
-------------------------------

Single Pair Ethernet (SPE) represents a different approach to Ethernet
connectivity, utilizing just one pair of conductors for both data and power
transmission. Unlike the configurations detailed in the PSE PI for standard
Ethernet, which can involve multiple power sourcing arrangements across four or
two pairs of wires, SPE operates on a simpler model due to its single-pair
design. As a result, the complexities of choosing between alternative pin
assignments for power delivery, as described in the PSE PI for multi-pair
Ethernet, are not applicable to SPE.
(单对以太网（SPE）代表了一种不同的以太网连接方式，仅用一对导线同时传输
数据与电力。标准以太网的 PSE PI 所详述的配置可能涉及跨四对或两对导线的
多种供电安排，而 SPE 由于其单对设计而运行于更简单的模型之上。因此，PSE
PI 中针对多对以太网所述的、在多个供电引脚分配方案之间做选择的复杂性，
并不适用于 SPE。)

Understanding PSE PI
(理解 PSE PI)
--------------------

The Power Sourcing Equipment Power Interface (PSE PI) is a framework defining
how Power Sourcing Equipment (PSE) delivers power to Powered Devices (PDs) over
Ethernet cables. It details two main configurations for power delivery, known
as Alternative A and Alternative B, which are distinguished not only by their
method of power transmission but also by the implications for polarity and data
transmission direction.
(供电设备电源接口（PSE PI）是一个定义供电设备（PSE）如何通过以太网线缆
向受电设备（PD）供电的框架。它详述了两种主要供电配置，即方案 A
（Alternative A）与方案 B（Alternative B）；二者不仅供电传输方式不同，
在极性与数据传输方向上的影响也不同。)

Alternative A and B Overview
(方案 A 与方案 B 概览)
----------------------------

- **Alternative A:** Utilizes RJ45 conductors 1, 2, 3 and 6. In either case of
  networks 10/100BaseT or 1G/2G/5G/10GBaseT, the pairs used are carrying data.
  The power delivery's polarity in this alternative can vary based on the MDI
  (Medium Dependent Interface) or MDI-X (Medium Dependent Interface Crossover)
  configuration.
  (**Alternative A：** 使用 RJ45 导线 1、2、3 和 6。无论是在 10/100BaseT
  还是 1G/2G/5G/10GBaseT 网络中，所用线对都承载数据。该方案下的供电极性
  可能随 MDI（介质相关接口）或 MDI-X（介质相关接口交叉）配置而变化。)

- **Alternative B:** Utilizes RJ45 conductors 4, 5, 7 and 8. In case of
  10/100BaseT network the pairs used are spare pairs without data and are less
  influenced by data transmission direction. This is not the case for
  1G/2G/5G/10GBaseT network. Alternative B includes two configurations with
  different polarities, known as variant X and variant S, to accommodate
  different network requirements and device specifications.
  (**Alternative B：** 使用 RJ45 导线 4、5、7 和 8。在 10/100BaseT 网络
  中，所用线对是不承载数据的空闲线对，受数据传输方向的影响较小；而
  1G/2G/5G/10GBaseT 网络则不然。Alternative B 包含两种极性不同的配置，
  分别称为变体 X 与变体 S，以适应不同的网络需求和设备规格。)

Table 145-3 PSE Pinout Alternatives
(表 145-3 PSE 引脚分配备选方案)
-----------------------------------

The following table outlines the pin configurations for both Alternative A and
Alternative B.
(下表列出了方案 A 与方案 B 的引脚配置。)

+------------+-------------------+-----------------+-----------------+-----------------+
| Conductor  | Alternative A     | Alternative A   | Alternative B   | Alternative B   |
|            |    (MDI-X)        |      (MDI)      |        (X)      |        (S)      |
| (导线)     |    (MDI-X) 方案A  |      (MDI) 方案A|        (X) 方案B|        (S) 方案B|
+============+===================+=================+=================+=================+
| 1          | Negative V        | Positive V      | -               | -               |
|            | (负电压)          | (正电压)        |                 |                 |
+------------+-------------------+-----------------+-----------------+-----------------+
| 2          | Negative V        | Positive V      | -               | -               |
|            | (负电压)          | (正电压)        |                 |                 |
+------------+-------------------+-----------------+-----------------+-----------------+
| 3          | Positive V        | Negative V      | -               | -               |
|            | (正电压)          | (负电压)        |                 |                 |
+------------+-------------------+-----------------+-----------------+-----------------+
| 4          | -                 | -               | Negative V      | Positive V      |
|            |                   |                 | (负电压)        | (正电压)        |
+------------+-------------------+-----------------+-----------------+-----------------+
| 5          | -                 | -               | Negative V      | Positive V      |
|            |                   |                 | (负电压)        | (正电压)        |
+------------+-------------------+-----------------+-----------------+-----------------+
| 6          | Positive V        | Negative V      | -               | -               |
|            | (正电压)          | (负电压)        |                 |                 |
+------------+-------------------+-----------------+-----------------+-----------------+
| 7          | -                 | -               | Positive V      | Negative V      |
|            |                   |                 | (正电压)        | (负电压)        |
+------------+-------------------+-----------------+-----------------+-----------------+
| 8          | -                 | -               | Positive V      | Negative V      |
|            |                   |                 | (正电压)        | (负电压)        |
+------------+-------------------+-----------------+-----------------+-----------------+

.. note::
    - "Positive V" and "Negative V" indicate the voltage polarity for each pin.
      ("Positive V" 与 "Negative V" 表示每个引脚的电压极性。)
    - "-" indicates that the pin is not used for power delivery in that
      specific configuration.
      ("-" 表示在该特定配置中，此引脚不用于供电。)

PSE PI compatibilities
(PSE PI 兼容性)
----------------------

The following table outlines the compatibility between the pinout alternative
and the 1000/2.5G/5G/10GBaseT in the PSE 2 pairs connection.
(下表列出了 PSE 两对线连接方式下，各引脚分配方案与
1000/2.5G/5G/10GBaseT 的兼容性。)

+---------+---------------+---------------------+-----------------------+
| Variant | Alternative   | Power Feeding Type  | Compatibility with    |
|         | (A/B)         | (Direct/Phantom)    | 1000/2.5G/5G/10GBaseT |
| (变体)  | (方案)        | (供电方式)          | (兼容性)              |
+=========+===============+=====================+=======================+
| 1       | A             | Phantom (幻象供电)  | Yes (是)              |
+---------+---------------+---------------------+-----------------------+
| 2       | B             | Phantom (幻象供电)  | Yes (是)              |
+---------+---------------+---------------------+-----------------------+
| 3       | B             | Direct (直接供电)   | No (否)               |
+---------+---------------+---------------------+-----------------------+

.. note::
    - "Direct" indicate a variant where the power is injected directly to pairs
       without using magnetics in case of spare pairs.
      ("Direct" 表示一种直接把电力注入线对的变体：在空闲线对的情形下
       不经过磁路（magnetics）。)
    - "Phantom" indicate power path over coils/magnetics as it is done for
       Alternative A variant.
      ("Phantom" 表示电力经由线圈/磁路（coils/magnetics）传输，方案 A
       变体正是如此。)

In case of PSE 4 pairs, a PSE supporting only 10/100BaseT (which mean Direct
Power on pinout Alternative B) is not compatible with a 4 pairs
1000/2.5G/5G/10GBaseT.
(对于 4 对线 PSE 的情形，仅支持 10/100BaseT 的 PSE（即引脚分配方案 B
上的直接供电）与 4 对线 1000/2.5G/5G/10GBaseT 不兼容。)

PSE Power Interface (PSE PI) Connection Diagram
(PSE 电源接口（PSE PI）连接图)
-----------------------------------------------

The diagram below illustrates the connection architecture between the RJ45
port, the Ethernet PHY (Physical Layer), and the PSE PI (Power Sourcing
Equipment Power Interface), demonstrating how power and data are delivered
simultaneously through an Ethernet cable. The RJ45 port serves as the physical
interface for these connections, with each of its eight pins connected to both
the Ethernet PHY for data transmission and the PSE PI for power delivery.
(下图展示了 RJ45 端口、以太网 PHY（物理层）与 PSE PI（供电设备电源接口）
之间的连接架构，演示了电力与数据如何通过以太网线缆同时传输。RJ45 端口
充当这些连接的物理接口，其八个引脚中的每一个都同时连接到用于数据传输的
以太网 PHY 和用于供电的 PSE PI。)

.. code-block::

    +--------------------------+
    |                          |
    |          RJ45 Port       |
    |                          |
    +--+--+--+--+--+--+--+--+--+                +-------------+
      1| 2| 3| 4| 5| 6| 7| 8|                   |             |
       |  |  |  |  |  |  |  o-------------------+             |
       |  |  |  |  |  |  o--|-------------------+             +<--- PSE 1
       |  |  |  |  |  o--|--|-------------------+             |
       |  |  |  |  o--|--|--|-------------------+             |
       |  |  |  o--|--|--|--|-------------------+  PSE PI     |
       |  |  o--|--|--|--|--|-------------------+             |
       |  o--|--|--|--|--|--|-------------------+             +<--- PSE 2 (optional)
       o--|--|--|--|--|--|--|-------------------+             |
       |  |  |  |  |  |  |  |                   |             |
    +--+--+--+--+--+--+--+--+--+                +-------------+
    |                          |
    |       Ethernet PHY       |
    |                          |
    +--------------------------+

Simple PSE PI Configuration for Alternative A
(方案 A 的简单 PSE PI 配置)
---------------------------------------------

The diagram below illustrates a straightforward PSE PI (Power Sourcing
Equipment Power Interface) configuration designed to support the Alternative A
setup for Power over Ethernet (PoE). This implementation is tailored to provide
power delivery through the data-carrying pairs of an Ethernet cable, suitable
for either MDI or MDI-X configurations, albeit supporting one variation at a
time.
(下图展示了一个简单直观的 PSE PI（供电设备电源接口）配置，用于支持以太
网供电（PoE）的方案 A（Alternative A）设置。该实现专为通过以太网线缆中
承载数据的线对供电而设计，既适用于 MDI 也适用于 MDI-X 配置，只是每次
只能支持其中一种变体。)

.. code-block::

         +-------------+
         |    PSE PI   |
 8  -----+                             +-------------+
 7  -----+                    Rail 1   |
 6  -----+------+----------------------+
 5  -----+      |                      |
 4  -----+      |             Rail 2   |  PSE 1
 3  -----+------/         +------------+
 2  -----+--+-------------/            |
 1  -----+--/                          +-------------+
         |
         +-------------+

In this configuration:
(在该配置中：)

- Pins 1 and 2, as well as pins 3 and 6, are utilized for power delivery in
  addition to data transmission. This aligns with the standard wiring for
  10/100BaseT Ethernet networks where these pairs are used for data.
  (引脚 1 和 2 以及引脚 3 和 6 在传输数据之外还用于供电。这与 10/100BaseT
  以太网网络的标准布线一致，因为这些线对正是用于传输数据的。)
- Rail 1 and Rail 2 represent the positive and negative voltage rails, with
  Rail 1 connected to pins 1 and 2, and Rail 2 connected to pins 3 and 6.
  More advanced PSE PI configurations may include integrated or external
  switches to change the polarity of the voltage rails, allowing for
  compatibility with both MDI and MDI-X configurations.
  (Rail 1 与 Rail 2 分别代表正、负电压轨，其中 Rail 1 连接到引脚 1 和 2，
  Rail 2 连接到引脚 3 和 6。更复杂的 PSE PI 配置可能包含集成或外置开关，
  用于改变电压轨的极性，从而同时兼容 MDI 与 MDI-X 配置。)

More complex PSE PI configurations may include additional components, to support
Alternative B, or to provide additional features such as power management, or
additional power delivery capabilities such as 2-pair or 4-pair power delivery.
(更复杂的 PSE PI 配置可能包含额外的组件，以支持方案 B，或提供电源管理
之类的附加功能，以及 2 对线或 4 对线供电之类的额外供电能力。)

.. code-block::

         +-------------+
         |    PSE PI   |
         |        +---+
 8  -----+--------+   |                 +-------------+
 7  -----+--------+   |       Rail 1   |
 6  -----+--------+   +-----------------+
 5  -----+--------+   |                |
 4  -----+--------+   |       Rail 2   |  PSE 1
 3  -----+--------+   +----------------+
 2  -----+--------+   |                |
 1  -----+--------+   |                 +-------------+
         |        +---+
         +-------------+

Device Tree Configuration: Describing PSE PI Configurations
(设备树配置：描述 PSE PI 配置)
-----------------------------------------------------------

The necessity for a separate PSE PI node in the device tree is influenced by
the intricacy of the Power over Ethernet (PoE) system's setup. Here are
descriptions of both simple and complex PSE PI configurations to illustrate
this decision-making process:
(设备树中是否需要一个独立的 PSE PI 节点，取决于以太网供电（PoE）系统
设置的复杂程度。下面分别描述简单与复杂的 PSE PI 配置，以说明这一决策
过程：)

**Simple PSE PI Configuration:**
(简单 PSE PI 配置：)
In a straightforward scenario, the PSE PI setup involves a direct, one-to-one
connection between a single PSE controller and an Ethernet port. This setup
typically supports basic PoE functionality without the need for dynamic
configuration or management of multiple power delivery modes. For such simple
configurations, detailing the PSE PI within the existing PSE controller's node
may suffice, as the system does not encompass additional complexity that
warrants a separate node. The primary focus here is on the clear and direct
association of power delivery to a specific Ethernet port.
(在简单的场景中，PSE PI 设置只是单个 PSE 控制器与一个以太网端口之间
直接的一对一连接。这种设置通常只支持基本 PoE 功能，无需对多种供电模式
进行动态配置或管理。对于这类简单配置，在现有 PSE 控制器节点内描述
PSE PI 即可满足需要，因为系统并不包含需要独立节点的额外复杂性。此处的
重点是清晰、直接地把供电关联到特定以太网端口。)

**Complex PSE PI Configuration:**
(复杂 PSE PI 配置：)
Contrastingly, a complex PSE PI setup may encompass multiple PSE controllers or
auxiliary circuits that collectively manage power delivery to one Ethernet
port. Such configurations might support a range of PoE standards and require
the capability to dynamically configure power delivery based on the operational
mode (e.g., PoE2 versus PoE4) or specific requirements of connected devices. In
these instances, a dedicated PSE PI node becomes essential for accurately
documenting the system architecture. This node would serve to detail the
interactions between different PSE controllers, the support for various PoE
modes, and any additional logic required to coordinate power delivery across
the network infrastructure.
(与之相反，复杂的 PSE PI 设置可能包含多个 PSE 控制器或辅助电路，共同
管理向一个以太网端口的供电。这类配置可能支持多种 PoE 标准，并要求能够
根据运行模式（例如 PoE2 与 PoE4 之争）或所连接设备的具体要求动态配置
供电。在这些情况下，设立一个专用的 PSE PI 节点对准确记录系统架构至关
重要。该节点用于详述不同 PSE 控制器之间的交互、对各种 PoE 模式的支持，
以及在整个网络基础设施中协调供电所需的任何附加逻辑。)

**Guidance:**
(指导建议：)

For simple PSE setups, including PSE PI information in the PSE controller node
might suffice due to the straightforward nature of these systems. However,
complex configurations, involving multiple components or advanced PoE features,
benefit from a dedicated PSE PI node. This method adheres to IEEE 802.3
specifications, improving documentation clarity and ensuring accurate
representation of the PoE system's complexity.
(对于简单的 PSE 设置，由于其系统性质直观，把 PSE PI 信息放进 PSE 控制
器节点可能就已足够。然而，对于涉及多个组件或高级 PoE 特性的复杂配置，
使用专用的 PSE PI 节点更有裨益。这种做法符合 IEEE 802.3 规范，可提高
文档清晰度，并确保准确呈现 PoE 系统的复杂性。)

PSE PI Node: Essential Information
(PSE PI 节点：关键信息)
----------------------------------

The PSE PI (Power Sourcing Equipment Power Interface) node in a device tree can
include several key pieces of information critical for defining the power
delivery capabilities and configurations of a PoE (Power over Ethernet) system.
Below is a list of such information, along with explanations for their
necessity and reasons why they might not be found within a PSE controller node:
(设备树中的 PSE PI（供电设备电源接口）节点可以包含若干关键信息，它们
对定义 PoE（以太网供电）系统的供电能力与配置至关重要。下面列出这些
信息，并说明其必要性，以及它们为何可能不会出现在 PSE 控制器节点中：)

1. **Powered Pairs Configuration**
   (供电线对配置)

   - *Description:* Identifies the pairs used for power delivery in the
     Ethernet cable.
     (*描述：* 标识以太网线缆中用于供电的线对。)
   - *Necessity:* Essential to ensure the correct pairs are powered according
     to the board's design.
     (*必要性：* 对于确保按板卡设计给正确的线对供电至关重要。)
   - *PSE Controller Node:* Typically lacks details on physical pair usage,
     focusing on power regulation.
     (*PSE 控制器节点：* 通常缺少物理线对使用情况的细节，只关注电源
     调节。)

2. **Polarity of Powered Pairs**
   (供电线对的极性)

   - *Description:* Specifies the polarity (positive or negative) for each
     powered pair.
     (*描述：* 为每对供电线对指定极性（正或负）。)
   - *Necessity:* Critical for safe and effective power transmission to PDs.
     (*必要性：* 对于向 PD 安全、有效地传输电力至关重要。)
   - *PSE Controller Node:* Polarity management may exceed the standard
     functionalities of PSE controllers.
     (*PSE 控制器节点：* 极性管理可能超出 PSE 控制器的标准功能范围。)

3. **PSE Cells Association**
   (PSE 单元关联)

   - *Description:* Details the association of PSE cells with Ethernet ports or
     pairs in multi-cell configurations.
     (*描述：* 详述多单元配置中 PSE 单元与以太网端口或线对的关联。)
   - *Necessity:* Allows for optimized power resource allocation in complex
     systems.
     (*必要性：* 便于在复杂系统中优化电力资源分配。)
   - *PSE Controller Node:* Controllers may not manage cell associations
     directly, focusing instead on power flow regulation.
     (*PSE 控制器节点：* 控制器可能不直接管理单元关联，而是专注于电力
     流的调节。)

4. **Support for PoE Standards**
   (对 PoE 标准的支持)

   - *Description:* Lists the PoE standards and configurations supported by the
     system.
     (*描述：* 列出系统支持的 PoE 标准与配置。)
   - *Necessity:* Ensures system compatibility with various PDs and adherence
     to industry standards.
     (*必要性：* 确保系统与各种 PD 的兼容性并符合行业标准。)
   - *PSE Controller Node:* Specific capabilities may depend on the overall PSE
     PI design rather than the controller alone. Multiple PSE cells per PI
     do not necessarily imply support for multiple PoE standards.
     (*PSE 控制器节点：* 具体能力可能取决于整个 PSE PI 的设计而非仅取决
     于控制器。每个 PI 拥有多个 PSE 单元并不一定意味着支持多种 PoE
     标准。)

5. **Protection Mechanisms**
   (保护机制)

   - *Description:* Outlines additional protection mechanisms, such as
     overcurrent protection and thermal management.
     (*描述：* 概述额外的保护机制，例如过流保护和热管理。)
   - *Necessity:* Provides extra safety and stability, complementing PSE
     controller protections.
     (*必要性：* 提供额外的安全性与稳定性，是对 PSE 控制器保护功能的
     补充。)
   - *PSE Controller Node:* Some protections may be implemented via
     board-specific hardware or algorithms external to the controller.
     (*PSE 控制器节点：* 某些保护可能通过板级专用硬件或控制器外部的
     算法实现。)
