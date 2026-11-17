.. SPDX-License-Identifier: GPL-2.0

Power Sourcing Equipment (PSE) in IEEE 802.3 Standard(IEEE 802.3 标准中的供电设备 (PSE))
========================================================================================

Overview(概述)
--------------

Power Sourcing Equipment (PSE) is essential in networks for delivering power
along with data over Ethernet cables. It usually refers to devices like
switches and hubs that supply power to Powered Devices (PDs) such as IP
cameras, VoIP phones, and wireless access points.
(供电设备 (Power Sourcing Equipment, PSE) 在网络中负责通过以太网线缆在传输数据的同时提供电力，是不可或缺的。它通常指交换机、集线器等为受电设备 (Powered Device, PD)(如 IP 摄像头、VoIP 电话和无线接入点)供电的设备。)

PSE vs. PoDL PSE(PSE 与 PoDL PSE 的对比)
----------------------------------------

PSE in the IEEE 802.3 standard generally refers to equipment that provides
power alongside data over Ethernet cables, typically associated with Power over
Ethernet (PoE).
(IEEE 802.3 标准中的 PSE 通常指通过以太网线缆在传输数据的同时提供电力的设备，一般与以太网供电 (Power over Ethernet, PoE) 相关联。)

PoDL PSE, or Power over Data Lines PSE, specifically denotes PSEs operating
with single balanced twisted-pair PHYs, as per Clause 104 of IEEE 802.3. PoDL
is significant in contexts like automotive and industrial controls where power
and data delivery over a single pair is advantageous.
(PoDL PSE,即数据线供电 PSE (Power over Data Lines PSE),特指按照 IEEE 802.3 第 104 章 (Clause 104) 与单对平衡双绞线 PHY 一起工作的 PSE。PoDL 在汽车和工业控制等场景中非常重要，因为在这些场景下通过单一双绞线对同时传输电力和数据更具优势。)

IEEE 802.3-2018 Addendums and Related Clauses(IEEE 802.3-2018 增补标准及相关条款)
---------------------------------------------------------------------------------

Key addenda to the IEEE 802.3-2018 standard relevant to power delivery over
Ethernet are as follows:
(IEEE 802.3-2018 标准中与以太网供电相关的关键增补标准如下：)

- **802.3af (Approved in 2003-06-12)**: Known as PoE in the market, detailed in
  Clause 33, delivering up to 15.4W of power.
  (即市场上所称的 PoE，在 Clause 33 中详述，可提供最高 15.4W 的功率。)
- **802.3at (Approved in 2009-09-11)**: Marketed as PoE+, enhancing PoE as
  covered in Clause 33, increasing power delivery to up to 30W.
  (市场名称为 PoE+，在 Clause 33 所覆盖内容的基础上增强了 PoE，将供电功率提升到最高 30W。)
- **802.3bt (Approved in 2018-09-27)**: Known as 4PPoE in the market, outlined
  in Clause 33. Type 3 delivers up to 60W, and Type 4 up to 100W.
  (即市场上所称的 4PPoE，在 Clause 33 中概述。Type 3 最高可提供 60W,Type 4 最高可达 100W。)
- **802.3bu (Approved in 2016-12-07)**: Formerly referred to as PoDL, detailed
  in Clause 104. Introduces Classes 0 - 9. Class 9 PoDL PSE delivers up to ~65W
  (此前称为 PoDL，在 Clause 104 中详述。引入 Class 0 - 9。Class 9 PoDL PSE 最高可提供约 65W 的功率)

Kernel Naming Convention Recommendations(内核命名约定建议)
----------------------------------------------------------

For clarity and consistency within the Linux kernel's networking subsystem, the
following naming conventions are recommended:
(为了在 Linux 内核网络子系统中保持清晰和一致，建议采用以下命名约定：)

- For general PSE (PoE) code, use "c33_pse" key words. For example:
  ``enum ethtool_c33_pse_admin_state c33_admin_control;``.
  This aligns with Clause 33, encompassing various PoE forms.
  (对于通用 PSE (PoE) 代码，使用 "c33_pse" 关键词，例如 ``enum ethtool_c33_pse_admin_state c33_admin_control;`` 。这与涵盖各种 PoE 形式的 Clause 33 保持一致。)

- For PoDL PSE - specific code, use "podl_pse". For example:
  ``enum ethtool_podl_pse_admin_state podl_admin_control;`` to differentiate
  PoDL PSE settings according to Clause 104.
  (对于 PoDL PSE 专用代码，使用 "podl_pse"，例如 ``enum ethtool_podl_pse_admin_state podl_admin_control;`` ，以便按照 Clause 104 区分 PoDL PSE 设置。)

Summary of Clause 33: Data Terminal Equipment (DTE) Power via Media Dependent Interface (MDI)(Clause 33 概述：通过介质相关接口 (MDI) 为数据终端设备 (DTE) 供电)
---------------------------------------------------------------------------------------------------------------------------------------------------------------

Clause 33 of the IEEE 802.3 standard defines the functional and electrical
characteristics of Powered Device (PD) and Power Sourcing Equipment (PSE).
These entities enable power delivery using the same generic cabling as for data
transmission, integrating power with data communication for devices such as
10BASE-T, 100BASE-TX, or 1000BASE-T.
(IEEE 802.3 标准的 Clause 33 定义了受电设备 (PD) 和供电设备 (PSE) 的功能与电气特性。这些实体使用与数据传输相同的通用布线来实现电力传输，将电力与数据通信集成在一起，适用于 10BASE-T、100BASE-TX 或 1000BASE-T 等设备。)

Summary of Clause 104: Power over Data Lines (PoDL) of Single Balanced Twisted-Pair Ethernet(Clause 104 概述：单对平衡双绞线以太网的数据线供电 (PoDL))
------------------------------------------------------------------------------------------------------------------------------------------------------

Clause 104 of the IEEE 802.3 standard delineates the functional and electrical
characteristics of PoDL Powered Devices (PDs) and PoDL Power Sourcing Equipment
(PSEs). These are designed for use with single balanced twisted-pair Ethernet
Physical Layers. In this clause, 'PSE' refers specifically to PoDL PSE, and
'PD' to PoDL PD. The key intent is to provide devices with a unified interface
for both data and the power required to process this data over a single
balanced twisted-pair Ethernet connection.
(IEEE 802.3 标准的 Clause 104 描述了 PoDL 受电设备 (PD) 和 PoDL 供电设备 (PSE) 的功能与电气特性。它们设计用于单对平衡双绞线以太网物理层。在该条款中，'PSE' 特指 PoDL PSE，'PD' 特指 PoDL PD。其主要意图是为设备提供一个统一接口，以便通过单一平衡双绞线以太网连接同时获取数据以及处理这些数据所需的电力。)
