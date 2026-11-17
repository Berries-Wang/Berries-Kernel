.. SPDX-License-Identifier: GPL-2.0

Diagnostic Concept for Investigating Twisted Pair Ethernet Variants at OSI Layer 1(在 OSI 第 1 层排查双绞线以太网变体的诊断概念)
================================================================================================================================

Introduction(引言)
------------------

This documentation is designed for two primary audiences:
(本文档面向两类主要读者：)

1. **Users and System Administrators**: For those dealing with real-world
   Ethernet issues, this guide provides a practical, step-by-step
   troubleshooting flow to help identify and resolve common problems in Twisted
   Pair Ethernet at OSI Layer 1. If you're facing unstable links, speed drops,
   or mysterious network issues, jump right into the step-by-step guide and
   follow it through to find your solution.
   (面向 **用户和系统管理员** ：对于处理实际以太网问题的人，本指南提供了实用的、
   分步的故障排除流程，帮助识别并解决 OSI 第 1 层双绞线以太网中的常见问题。如果你
   正面临链路不稳定、速率下降或难以解释的网络问题，请直接进入分步指南并逐步执行，
   找到你的解决方案。)

2. **Kernel Developers**: For developers working with network drivers and PHY
   support, this documentation outlines the diagnostic process and highlights
   areas where the Linux kernel’s diagnostic interfaces could be extended or
   improved. By understanding the diagnostic flow, developers can better
   prioritize future enhancements.
   (面向 **内核开发者** ：对于从事网络驱动和 PHY 支持开发的开发者，本文档概述了
   诊断流程，并指出了 Linux 内核诊断接口可以扩展或改进的方面。通过理解诊断流程，
   开发者可以更好地规划未来的增强工作。)

Step-by-Step Diagnostic Guide from Linux (General Ethernet)(Linux 分步诊断指南 (通用以太网))
--------------------------------------------------------------------------------------------

This diagnostic guide covers common Ethernet troubleshooting scenarios,
focusing on **link stability and detection** across different Ethernet
environments, including **Single-Pair Ethernet (SPE)** and **Multi-Pair
Ethernet (MPE)**, as well as power delivery technologies like **PoDL** (Power
over Data Line) and **PoE** (Clause 33 PSE).
(本诊断指南涵盖常见的以太网故障排除场景，重点讨论不同以太网环境下的
**链路稳定性与检测** ，包括 **单对以太网 (SPE)** 和 **多对以太网 (MPE)** ，以及
**PoDL** (Data Line 供电) 和 **PoE** (Clause 33 PSE) 等供电技术。)

The guide is designed to help users diagnose physical layer (Layer 1) issues on
systems running **Linux kernel version 6.11 or newer**, utilizing **ethtool
version 6.10 or later** and **iproute2 version 6.4.0 or later**.
(本指南旨在帮助用户在运行 **Linux 内核 6.11 或更新版本** 的系统上诊断物理层
(第 1 层) 问题，使用 **ethtool 6.10 或更高版本** 以及 **iproute2 6.4.0 或更高
版本** 。)

In this guide, we assume that users may have **limited or no access to the link
partner** and will focus on diagnosing issues locally.
(在本指南中，我们假定用户可能 **对链路对端的访问受限或完全无法访问** ，并将专注于
在本地诊断问题。)

Diagnostic Scenarios(诊断场景)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- **Link is up and stable, but no data transfer**: If the link is stable but
  there are issues with data transmission, refer to the **OSI Layer 2
  Troubleshooting Guide**.
  (**链路已建立且稳定，但没有数据传输** ：如果链路稳定但数据传输存在问题，请参考
  **OSI 第 2 层故障排除指南** 。)

- **Link is unstable**: Link resets, speed drops, or other fluctuations
  indicate potential issues at the hardware or physical layer.
  (**链路不稳定** ：链路重置、速率下降或其他波动表明硬件或物理层可能存在问题。)

- **No link detected**: The interface is up, but no link is established.
  (**未检测到链路** ：接口已启用，但未建立链路。)

Verify Interface Status(验证接口状态)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Begin by verifying the status of the Ethernet interface to check if it is
administratively up. Unlike `ethtool`, which provides information on the link
and PHY status, it does not show the **administrative state** of the interface.
To check this, you should use the `ip` command, which describes the interface
state within the angle brackets `"<>"` in its output.
(首先验证以太网接口的状态，以检查它是否处于管理性启用 (administratively up) 状态。
`ethtool` 提供链路和 PHY 状态信息，但不显示接口的 **管理状态** 。要检查这一点，应
使用 `ip` 命令，它在输出的尖括号 `"<>"` 内描述接口状态。)

For example, in the output `<NO-CARRIER,BROADCAST,MULTICAST,UP>`, the important
keywords are:
(例如，在输出 `<NO-CARRIER,BROADCAST,MULTICAST,UP>` 中，重要的关键字是：)

- **UP**: The interface is in the administrative "UP" state.
  (**UP** ：接口处于管理性 "UP" 状态。)
- **NO-CARRIER**: The interface is administratively up, but no physical link is
  detected.
  (**NO-CARRIER** ：接口在管理上已启用，但未检测到物理链路。)

If the output shows `<BROADCAST,MULTICAST>`, this indicates the interface is in
the administrative "DOWN" state.
(如果输出显示 `<BROADCAST,MULTICAST>` ，则表示接口处于管理性 "DOWN" 状态。)

- **Command:** `ip link show dev <interface>` (命令)

- **Expected Output:** (预期输出：)

  .. code-block:: bash

     4: eth0: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 ...
        link/ether 88:14:2b:00:96:f2 brd ff:ff:ff:ff:ff:ff

- **Interpreting the Output:** (输出解读：)

  - **Administrative UP State**: (管理性 UP 状态：)

    - If the output contains **"UP"**, the interface is administratively up,
      and the system is trying to establish a physical link.
      (如果输出包含 **"UP"** ，则接口在管理上已启用，系统正在尝试建立物理链路。)

    - If you also see **"NO-CARRIER"**, it means the physical link has not been
      detected, indicating potential Layer 1 issues like a cable fault,
      misconfiguration, or no connection at the link partner. In this case,
      proceed to the **Inspect Link Status and PHY Configuration** section.
      (如果还看到 **"NO-CARRIER"** ，则表示未检测到物理链路，表明可能存在第 1 层
      问题，例如电缆故障、配置错误或链路对端未连接。在这种情况下，请继续阅读
      **Inspect Link Status and PHY Configuration** 一节。)

  - **Administrative DOWN State**: (管理性 DOWN 状态：)

    - If the output lacks **"UP"** and shows only states like
      **"<BROADCAST,MULTICAST>"**, it means the interface is administratively
      down. In this case, bring the interface up using the following command:
      (如果输出缺少 **"UP"** 并且只显示诸如 **"<BROADCAST,MULTICAST>"** 之类的
      状态，则表示接口在管理上处于关闭状态。在这种情况下，请使用以下命令启用
      接口：)

      .. code-block:: bash

         ip link set dev <interface> up

- **Next Steps**: (后续步骤：)

  - If the interface is **administratively up** but shows **NO-CARRIER**,
    proceed to the **Inspect Link Status and PHY Configuration** section to
    troubleshoot potential physical layer issues.
    (如果接口 **在管理上已启用** 但显示 **NO-CARRIER** ，请继续阅读
    **Inspect Link Status and PHY Configuration** 一节，排查潜在的物理层问题。)

  - If the interface was **administratively down** and you have brought it up,
    ensure to **repeat this verification step** to confirm the new state of the
    interface before proceeding
    (如果接口原本 **在管理上处于关闭状态** 而你已将其启用，请确保在继续之前
    **重复此验证步骤** 以确认接口的新状态。)

  - **If the interface is up and the link is detected**:
    (**如果接口已启用且检测到链路** ：)

    - If the output shows **"UP"** and there is **no `NO-CARRIER`**, the
      interface is administratively up, and the physical link has been
      successfully established. If everything is working as expected, the Layer
      1 diagnostics are complete, and no further action is needed.
      (如果输出显示 **"UP"** 且 **没有 `NO-CARRIER`** ，则接口在管理上已启用，
      物理链路已成功建立。如果一切按预期工作，则第 1 层诊断已完成，无需进一步
      操作。)

    - If the interface is up and the link is detected but **no data is being
      transferred**, the issue is likely beyond Layer 1, and you should proceed
      with diagnosing the higher layers of the OSI model. This may involve
      checking Layer 2 configurations (such as VLANs or MAC address issues),
      Layer 3 settings (like IP addresses, routing, or ARP), or Layer 4 and
      above (firewalls, services, etc.).
      (如果接口已启用且检测到链路，但 **没有数据被传输** ，则问题很可能出在
      第 1 层之外，应继续诊断 OSI 模型的更高层。这可能涉及检查第 2 层配置 (例如
      VLAN 或 MAC 地址问题)、第 3 层设置 (例如 IP 地址、路由或 ARP)，或第 4 层及
      以上 (防火墙、服务等)。)

    - If the **link is unstable** or **frequently resetting or dropping**, this
      may indicate a physical layer issue such as a faulty cable, interference,
      or power delivery problems. In this case, proceed with the next step in
      this guide.
      (如果 **链路不稳定** 或 **频繁重置或断开** ，这可能表明存在物理层问题，例如
      电缆故障、干扰或供电问题。在这种情况下，请继续执行本指南的下一步。)

Inspect Link Status and PHY Configuration(检查链路状态与 PHY 配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use `ethtool -I` to check the link status, PHY configuration, supported link
modes, and additional statistics such as the **Link Down Events** counter. This
step is essential for diagnosing Layer 1 problems such as speed mismatches,
duplex issues, and link instability.
(使用 `ethtool -I` 检查链路状态、PHY 配置、支持的链路模式，以及诸如
**Link Down Events** 计数器之类的附加统计信息。这一步对于诊断速率不匹配、双工问题
和链路不稳定等第 1 层问题至关重要。)

For both **Single-Pair Ethernet (SPE)** and **Multi-Pair Ethernet (MPE)**
devices, you will use this step to gather key details about the link. **SPE**
links generally support a single speed and mode without autonegotiation (with
the exception of **10BaseT1L**), while **MPE** devices typically support
multiple link modes and autonegotiation.
(对于 **单对以太网 (SPE)** 和 **多对以太网 (MPE)** 设备，都将使用这一步来收集关于
链路的关键细节。 **SPE** 链路通常支持单一速率和模式，不支持自协商
(**10BaseT1L** 除外)，而 **MPE** 设备通常支持多种链路模式和自协商。)

- **Command:** `ethtool -I <interface>` (命令)

- **Example Output for SPE Interface (Non-autonegotiation)**: (SPE 接口示例输出 (非自协商)：)

  .. code-block:: bash

     Settings for spe4:
         Supported ports: [ TP ]
         Supported link modes:   100baseT1/Full
         Supported pause frame use: No
         Supports auto-negotiation: No
         Supported FEC modes: Not reported
         Advertised link modes: Not applicable
         Advertised pause frame use: No
         Advertised auto-negotiation: No
         Advertised FEC modes: Not reported
         Speed: 100Mb/s
         Duplex: Full
         Auto-negotiation: off
         master-slave cfg: forced slave
         master-slave status: slave
         Port: Twisted Pair
         PHYAD: 6
         Transceiver: external
         MDI-X: Unknown
         Supports Wake-on: d
         Wake-on: d
         Link detected: yes
         SQI: 7/7
         Link Down Events: 2

- **Example Output for MPE Interface (Autonegotiation)**: (MPE 接口示例输出 (自协商)：)

  .. code-block:: bash

     Settings for eth1:
         Supported ports: [ TP    MII ]
         Supported link modes:   10baseT/Half 10baseT/Full
                                 100baseT/Half 100baseT/Full
         Supported pause frame use: Symmetric Receive-only
         Supports auto-negotiation: Yes
         Supported FEC modes: Not reported
         Advertised link modes:  10baseT/Half 10baseT/Full
                                 100baseT/Half 100baseT/Full
         Advertised pause frame use: Symmetric Receive-only
         Advertised auto-negotiation: Yes
         Advertised FEC modes: Not reported
         Link partner advertised link modes:  10baseT/Half 10baseT/Full
                                              100baseT/Half 100baseT/Full
         Link partner advertised pause frame use: Symmetric Receive-only
         Link partner advertised auto-negotiation: Yes
         Link partner advertised FEC modes: Not reported
         Speed: 100Mb/s
         Duplex: Full
         Auto-negotiation: on
         Port: Twisted Pair
         PHYAD: 10
         Transceiver: internal
         MDI-X: Unknown
         Supports Wake-on: pg
         Wake-on: p
         Link detected: yes
         Link Down Events: 1

- **Next Steps**: (后续步骤：)

  - Record the output provided by `ethtool`, particularly noting the
    **master-slave status**, **speed**, **duplex**, and other relevant fields.
    This information will be useful for further analysis or troubleshooting.
    Once the **ethtool** output has been collected and stored, move on to the
    next diagnostic step.
    (记录 `ethtool` 提供的输出，特别注意 **master-slave 状态** 、 **速率** 、
    **双工** 及其他相关字段。这些信息将有助于后续分析或故障排除。在收集并保存
    **ethtool** 输出之后，进入下一个诊断步骤。)

Check Power Delivery (PoDL or PoE)(检查供电 (PoDL 或 PoE))
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If it is known that **PoDL** or **PoE** is **not implemented** on the system,
or the **PSE** (Power Sourcing Equipment) is managed by proprietary user-space
software or external tools, you can skip this step. In such cases, verify power
delivery through alternative methods, such as checking hardware indicators
(LEDs), using multimeters, or consulting vendor-specific software for
monitoring power status.
(如果已知系统上 **未实现** **PoDL** 或 **PoE** ，或者 **PSE** (供电设备，
Power Sourcing Equipment) 由专有用户空间软件或外部工具管理，则可以跳过此步骤。
在这种情况下，请通过其他方法验证供电情况，例如检查硬件指示灯 (LED)、使用万用表，
或查阅用于监控电源状态的厂商专用软件。)

If **PoDL** or **PoE** is implemented and managed directly by Linux, follow
these steps to ensure power is being delivered correctly:
(如果 **PoDL** 或 **PoE** 已实现并由 Linux 直接管理，请按照以下步骤确保供电
正常：)

- **Command:** `ethtool --show-pse <interface>` (命令)

- **Expected Output Examples**: (预期输出示例：)

  1. **PSE Not Supported**: (不支持 PSE：)

     If no PSE is attached or the interface does not support PSE, the following
     output is expected:
     (如果未连接 PSE 或接口不支持 PSE，预期会有如下输出：)

     .. code-block:: bash

        netlink error: No PSE is attached
        netlink error: Operation not supported

  2. **PoDL (Single-Pair Ethernet)**: (PoDL (单对以太网)：)

     When PoDL is implemented, you might see the following attributes:
     (当 PoDL 已实现时，你可能会看到以下属性：)

     .. code-block:: bash

        PSE attributes for eth1:
        PoDL PSE Admin State: enabled
        PoDL PSE Power Detection Status: delivering power

  3. **PoE (Clause 33 PSE)**: (PoE (Clause 33 PSE)：)

     For standard PoE, the output may look like this:
     (对于标准 PoE，输出可能如下所示：)

     .. code-block:: bash

        PSE attributes for eth1:
        Clause 33 PSE Admin State: enabled
        Clause 33 PSE Power Detection Status: delivering power
        Clause 33 PSE Available Power Limit: 18000

- **Adjust Power Limit (if needed)**: (调整功率限制 (如需要)：)

  - Sometimes, the available power limit may not be sufficient for the link
    partner. You can increase the power limit as needed.
    (有时，可用功率限制可能不足以满足链路对端的需要。你可以根据需要提高功率
    限制。)

  - **Command:** `ethtool --set-pse <interface> c33-pse-avail-pw-limit <limit>` (命令)

    Example: (示例：)

    .. code-block:: bash

      ethtool --set-pse eth1 c33-pse-avail-pw-limit 18000
      ethtool --show-pse eth1

    **Expected Output** after adjusting the power limit:
    (调整功率限制后的预期输出：)

    .. code-block:: bash

      Clause 33 PSE Available Power Limit: 18000


- **Next Steps**: (后续步骤：)

  - **PoE or PoDL Not Used**: If **PoE** or **PoDL** is not implemented or used
    on the system, proceed to the next diagnostic step, as power delivery is
    not relevant for this setup.
    (**未使用 PoE 或 PoDL** ：如果系统上未实现或未使用 **PoE** 或 **PoDL** ，
    请进入下一个诊断步骤，因为供电与此配置无关。)

  - **PoE or PoDL Controlled Externally**: If **PoE** or **PoDL** is used but
    is not managed by the Linux kernel's **PSE-PD** framework (i.e., it is
    controlled by proprietary user-space software or external tools), this part
    is out of scope for this documentation. Please consult vendor-specific
    documentation or external tools for monitoring and managing power delivery.
    (**PoE 或 PoDL 由外部控制** ：如果使用了 **PoE** 或 **PoDL** 但不由 Linux
    内核的 **PSE-PD** 框架管理 (即由专有用户空间软件或外部工具控制)，则此部分不在
    本文档的范围内。请查阅厂商专用文档或外部工具来监控和管理供电。)

  - **PSE Admin State Disabled**: (PSE 管理状态被禁用：)

    - If the `PSE Admin State:` is **disabled**, enable it by running one of
      the following commands:
      (如果 `PSE Admin State:` 为 **disabled** ，请通过运行以下命令之一来启用
      它：)

      .. code-block:: bash

         ethtool --set-pse <devname> podl-pse-admin-control enable

      or, for Clause 33 PSE (PoE):
      (或者，对于 Clause 33 PSE (PoE)：)

         ethtool --set-pse <devname> c33-pse-admin-control enable

    - After enabling the PSE Admin State, return to the start of the **Check
      Power Delivery (PoDL or PoE)** step to recheck the power delivery status.
      (启用 PSE 管理状态后，返回 **检查供电 (PoDL 或 PoE)** 步骤的开头，重新检查
      供电状态。)

  - **Power Not Delivered**: If the `Power Detection Status` shows something
    other than "delivering power" (e.g., `over current`), troubleshoot the
    **PSE**. Check for potential issues such as a short circuit in the cable,
    insufficient power delivery, or a fault in the PSE itself.
    (**未供电** ：如果 `Power Detection Status` 显示的不是 "delivering power"
    (例如 `over current`)，请排查 **PSE** 。检查可能存在的问题，例如电缆短路、
    供电不足，或 PSE 本身故障。)

  - **Power Delivered but No Link**: If power is being delivered but no link is
    established, proceed with further diagnostics by performing **Cable
    Diagnostics** or reviewing the **Inspect Link Status and PHY
    Configuration** steps to identify any underlying issues with the physical
    link or settings.
    (**已供电但没有链路** ：如果正在供电但未建立链路，请继续进行进一步的诊断，
    执行 **电缆诊断** 或复查 **检查链路状态与 PHY 配置** 步骤，以找出物理链路或
    设置中存在的潜在问题。)

Cable Diagnostics(电缆诊断)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use `ethtool` to test for physical layer issues such as cable faults. The test
results can vary depending on the cable's condition, the technology in use, and
the state of the link partner. The results from the cable test will help in
diagnosing issues like open circuits, shorts, impedance mismatches, and
noise-related problems.
(使用 `ethtool` 测试物理层问题，例如电缆故障。测试结果可能因电缆状况、所用技术以及
链路对端的状态而异。电缆测试的结果将有助于诊断断路、短路、阻抗失配以及噪声相关等
问题。)

- **Command:** `ethtool --cable-test <interface>` (命令)

The following are the typical outputs for **Single-Pair Ethernet (SPE)** and
**Multi-Pair Ethernet (MPE)**:
(以下是 **单对以太网 (SPE)** 和 **多对以太网 (MPE)** 的典型输出：)

- **For Single-Pair Ethernet (SPE)**: (对于单对以太网 (SPE))
  - **Expected Output (SPE)**: (预期输出 (SPE)：)

  .. code-block:: bash

    Cable test completed for device eth1.
    Pair A, fault length: 25.00m
    Pair A code Open Circuit

  This indicates an open circuit or cable fault at the reported distance, but
  results can be influenced by the link partner's state. Refer to the
  **"Troubleshooting Based on Cable Test Results"** section for further
  interpretation of these results.
  (这表明在报告的距离处存在断路或电缆故障，但结果可能受链路对端状态的影响。有关
  这些结果的进一步解读，请参考 **"基于电缆测试结果的故障排除"** 一节。)

- **For Multi-Pair Ethernet (MPE)**: (对于多对以太网 (MPE))
  - **Expected Output (MPE)**: (预期输出 (MPE)：)

  .. code-block:: bash

    Cable test completed for device eth0.
    Pair A code OK
    Pair B code OK
    Pair C code Open Circuit

  Here, Pair C is reported as having an open circuit, while Pairs A and B are
  functioning correctly. However, if autonegotiation is in use on Pairs A and
  B, the cable test may be disrupted. Refer to the **"Troubleshooting Based on
  Cable Test Results"** section for a detailed explanation of these issues and
  how to resolve them.
  (此处，线对 C 被报告为断路，而线对 A 和 B 工作正常。但是，如果线对 A 和 B 正在
  使用自协商，电缆测试可能会受到干扰。有关这些问题的详细说明及解决方法，请参考
  **"基于电缆测试结果的故障排除"** 一节。)

For detailed descriptions of the different possible cable test results, please
refer to the **"Troubleshooting Based on Cable Test Results"** section.
(有关各种可能的电缆测试结果的详细说明，请参考 **"基于电缆测试结果的故障排除"**
一节。)

Troubleshooting Based on Cable Test Results(基于电缆测试结果的故障排除)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After running the cable test, the results can help identify specific issues in
the physical connection. However, it is important to note that **cable testing
results heavily depend on the capabilities and characteristics of both the
local hardware and the link partner**. The accuracy and reliability of the
results can vary significantly between different hardware implementations.
(运行电缆测试后，结果有助于识别物理连接中的特定问题。但需要注意， **电缆测试结果
在很大程度上取决于本地硬件和链路对端的能力与特性** 。结果的准确性和可靠性在不同
硬件实现之间可能有显著差异。)

In some cases, this can introduce **blind spots** in the current cable testing
implementation, where certain results may not accurately reflect the actual
physical state of the cable. For example:
(在某些情况下，这会给当前的电缆测试实现带来 **盲点** ，即某些结果可能无法准确反映
电缆的实际物理状态。例如：)

- An **Open Circuit** result might not only indicate a damaged or disconnected
  cable but also occur if the cable is properly attached to a powered-down link
  partner.
  (**断路 (Open Circuit)** 结果不仅可能表明电缆损坏或断开，也可能在电缆正确连接到
  已断电的链路对端时出现。)

- Some PHYs may report a **Short within Pair** if the link partner is in
  **forced slave mode**, even though there is no actual short in the cable.
  (如果链路对端处于 **强制从模式 (forced slave mode)** ，某些 PHY 可能会报告
  **对内短路 (Short within Pair)** ，即使电缆中实际上并不存在短路。)

To help users interpret the results more effectively, it could be beneficial to
extend the **kernel UAPI** (User API) to provide additional context or
**possible variants** of issues based on the hardware’s characteristics. Since
these quirks are often hardware-specific, the **kernel driver** would be an
ideal source of such information. By providing flags or hints related to
potential false positives for each test result, users would have a better
understanding of what to verify and where to investigate further.
(为了帮助用户更有效地解读结果，扩展 **内核 UAPI** (User API) 以提供基于硬件特性的
额外上下文或问题的 **可能变体** 可能会有所帮助。由于这些怪癖通常与特定硬件相关，
**内核驱动** 是此类信息的理想来源。通过为每个测试结果提供与潜在误报相关的标志或
提示，用户可以更好地了解需要验证什么以及在哪里进一步排查。)

Until such improvements are made, users should be aware of these limitations
and manually verify cable issues as needed. Physical inspections may help
resolve uncertainties related to false positive results.
(在这些改进完成之前，用户应了解这些局限，并根据需要手动验证电缆问题。物理检查可能
有助于解决与误报结果相关的不确定性。)

The results can be one of the following:
(结果可能是以下几种之一：)

- **OK**: (正常：)

  - The cable is functioning correctly, and no issues were detected.
    (电缆工作正常，未检测到问题。)

  - **Next Steps**: If you are still experiencing issues, it might be related
    to higher-layer problems, such as duplex mismatches or speed negotiation,
    which are not physical-layer issues.
    (**后续步骤** ：如果仍然遇到问题，则可能与更高层的问题有关，例如双工失配或
    速率协商，这些并非物理层问题。)

  - **Special Case for `BaseT1` (1000/100/10BaseT1)**: In `BaseT1` systems, an
    "OK" result typically also means that the link is up and likely in **slave
    mode**, since cable tests usually only pass in this mode. For some
    **10BaseT1L** PHYs, an "OK" result may occur even if the cable is too long
    for the PHY's configured range (for example, when the range is configured
    for short-distance mode).
    (**`BaseT1` (1000/100/10BaseT1) 的特殊情况** ：在 `BaseT1` 系统中，"OK" 结果
    通常还意味着链路已建立且很可能处于 **从模式 (slave mode)** ，因为电缆测试
    通常只能在该模式下通过。对于某些 **10BaseT1L** PHY，即使电缆长度超过 PHY
    配置的范围 (例如范围被配置为短距离模式)，也可能出现 "OK" 结果。)

- **Open Circuit**: (断路：)

  - An **Open Circuit** result typically indicates that the cable is damaged or
    disconnected at the reported fault length. Consider these possibilities:
    (**断路 (Open Circuit)** 结果通常表明电缆在报告的故障长度处损坏或断开。请
    考虑以下可能性：)

    - If the link partner is in **admin down** state or powered off, you might
      still get an "Open Circuit" result even if the cable is functional.
      (如果链路对端处于 **管理关闭 (admin down)** 状态或已断电，即使电缆功能
      正常，你仍可能得到 "Open Circuit" 结果。)

    - **Next Steps**: Inspect the cable at the fault length for visible damage
      or loose connections. Verify the link partner is powered on and in the
      correct mode.
      (**后续步骤** ：检查电缆在故障长度处是否有明显损坏或连接松动。确认链路对端
      已通电并处于正确的模式。)

- **Short within Pair**: (对内短路：)

  - A **Short within Pair** indicates an unintended connection within the same
    pair of wires, typically caused by physical damage to the cable.
    (**对内短路 (Short within Pair)** 表示同一对导线内部出现了意外连接，通常由
    电缆的物理损坏引起。)

    - **Next Steps**: Replace or repair the cable and check for any physical
      damage or improperly crimped connectors.
      (**后续步骤** ：更换或修理电缆，并检查是否存在任何物理损坏或压接不良的
      连接器。)

- **Short to Another Pair**: (对间短路：)

  - A **Short to Another Pair** means the wires from different pairs are
    shorted, which could occur due to physical damage or incorrect wiring.
    (**对间短路 (Short to Another Pair)** 表示不同线对之间的导线发生了短路，可能
    由物理损坏或接线错误引起。)

    - **Next Steps**: Replace or repair the damaged cable. Inspect the cable for
      incorrect terminations or pinched wiring.
      (**后续步骤** ：更换或修理损坏的电缆。检查电缆是否存在端接错误或导线被
      压伤。)

- **Impedance Mismatch**: (阻抗失配：)

  - **Impedance Mismatch** indicates a reflection caused by an impedance
    discontinuity in the cable. This can happen when a part of the cable has
    abnormal impedance (e.g., when different cable types are spliced together
    or when there is a defect in the cable).
    (**阻抗失配 (Impedance Mismatch)** 表示由电缆中阻抗不连续引起的反射。当电缆
    的一部分具有异常阻抗时 (例如不同类型的电缆拼接在一起，或电缆存在缺陷)，就
    可能发生这种情况。)

    - **Next Steps**: Check the cable quality and ensure consistent impedance
      throughout its length. Replace any sections of the cable that do not meet
      specifications.
      (**后续步骤** ：检查电缆质量，确保其全长阻抗一致。更换电缆中任何不符合规格
      的部分。)

- **Noise**: (噪声：)

  - **Noise** means that the Time Domain Reflectometry (TDR) test could not
    complete due to excessive noise on the cable, which can be caused by
    interference from electromagnetic sources.
    (**噪声 (Noise)** 表示时域反射 (TDR) 测试因电缆上噪声过大而无法完成，这可能
    由电磁源的干扰引起。)

    - **Next Steps**: Identify and eliminate sources of electromagnetic
      interference (EMI) near the cable. Consider using shielded cables or
      rerouting the cable away from noise sources.
      (**后续步骤** ：识别并消除电缆附近的电磁干扰 (EMI) 源。考虑使用屏蔽电缆，
      或将电缆重新布线以远离噪声源。)

- **Resolution Not Possible**: (无法解析：)

  - **Resolution Not Possible** means that the TDR test could not detect the
    issue due to the resolution limitations of the test or because the fault is
    beyond the distance that the test can measure.
    (**无法解析 (Resolution Not Possible)** 表示由于测试的分辨率限制，或故障超出
    测试可测量的距离，TDR 测试无法检测到该问题。)

    - **Next Steps**: Inspect the cable manually if possible, or use alternative
      diagnostic tools that can handle greater distances or higher resolution.
      (**后续步骤** ：如有可能，请手动检查电缆，或使用能够处理更长距离或更高分辨率
      的替代诊断工具。)

- **Unknown**: (未知：)

  - An **Unknown** result may occur when the test cannot classify the fault or
    when a specific issue is outside the scope of the tool's detection
    capabilities.
    (**未知 (Unknown)** 结果可能在测试无法对故障进行分类，或特定问题超出工具检测
    能力范围时出现。)

    - **Next Steps**: Re-run the test, verify the link partner's state, and inspect
      the cable manually if necessary.
      (**后续步骤** ：重新运行测试，验证链路对端的状态，如有必要请手动检查
      电缆。)

Verify Link Partner PHY Configuration(验证链路对端 PHY 配置)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the cable test passes but the link is still not functioning correctly, it’s
essential to verify the configuration of the link partner’s PHY. Mismatches in
speed, duplex settings, or master-slave roles can cause connection issues.
(如果电缆测试通过但链路仍然无法正常工作，则必须验证链路对端 PHY 的配置。速率、
双工设置或主从角色的不匹配都可能导致连接问题。)

Autonegotiation Mismatch(自协商失配)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- If both link partners support autonegotiation, ensure that autonegotiation is
  enabled on both sides and that all supported link modes are advertised. A
  mismatch can lead to connectivity problems or sub optimal performance.
  (如果两个链路对端都支持自协商，请确保双方都启用了自协商，并且宣告 (advertise)
  了所有支持的链路模式。失配可能导致连接问题或欠佳的性能。)

- **Quick Fix:** Reset autonegotiation to the default settings, which will
  advertise all default link modes:
  (**快速修复：** 将自协商重置为默认设置，这将宣告所有默认链路模式：)

  .. code-block:: bash

     ethtool -s <interface> autoneg on

- **Command to check configuration:** `ethtool <interface>` (检查配置的命令)

- **Expected Output:** Ensure that both sides advertise compatible link modes.
  If autonegotiation is off, verify that both link partners are configured for
  the same speed and duplex.
  (**预期输出：** 确保双方宣告兼容的链路模式。如果自协商关闭，请验证两个链路对端
  是否配置了相同的速率和双工。)

  The following example shows a case where the local PHY advertises fewer link
  modes than it supports. This will reduce the number of overlapping link modes
  with the link partner. In the worst case, there will be no common link modes,
  and the link will not be created:
  (下面的示例展示了本地 PHY 宣告的链路模式少于其支持的链路模式的情况。这会减少与
  链路对端重叠的链路模式数量。在最坏的情况下，将没有共同的链路模式，链路将无法
  建立：)

  .. code-block:: bash

     Settings for eth0:
        Supported link modes:  1000baseT/Full, 100baseT/Full
        Advertised link modes: 1000baseT/Full
        Speed: 1000Mb/s
        Duplex: Full
        Auto-negotiation: on

Combined Mode Mismatch (Autonegotiation on One Side, Forced on the Other)(组合模式失配 (一侧自协商，另一侧强制))
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- One possible issue occurs when one side is using **autonegotiation** (as in
  most modern systems), and the other side is set to a **forced link mode**
  (e.g., older hardware with single-speed hubs). In such cases, modern PHYs
  will attempt to detect the forced mode on the other side. If the link is
  established, you may notice:
  (一种可能的问题出现在一侧使用 **自协商** (如同大多数现代系统)，而另一侧被设置为
  **强制链路模式** (例如带有单速集线器的旧硬件) 的情况下。在这种情况下，现代 PHY
  会尝试检测另一侧的强制模式。如果链路建立，你可能注意到：)

  - **No or empty "Link partner advertised link modes"**.
    (**没有或空的 "Link partner advertised link modes"** 。)

  - **"Link partner advertised auto-negotiation:"** will be **"no"** or not
    present.
    (**"Link partner advertised auto-negotiation:"** 将为 **"no"** 或不出现。)

- This type of detection does not always work reliably:
  (这种类型的检测并不总是可靠：)

  - Typically, the modern PHY will default to **Half Duplex**, even if the link
    partner is actually configured for **Full Duplex**.
    (通常，现代 PHY 会默认使用 **半双工 (Half Duplex)** ，即使链路对端实际配置为
    **全双工 (Full Duplex)** 。)

  - Some PHYs may not work reliably if the link partner switches from one
    forced mode to another. In this case, only a down/up cycle may help.
    (如果链路对端从一种强制模式切换到另一种，某些 PHY 可能无法可靠工作。在这种
    情况下，可能只有关闭/开启一次链路才能解决。)

- **Next Steps**: Set both sides to the same fixed speed and duplex mode to
  avoid potential detection issues.
  (**后续步骤** ：将两侧设置为相同的固定速率和双工模式，以避免潜在的检测问题。)

  .. code-block:: bash

     ethtool -s <interface> speed 1000 duplex full autoneg off

Master/Slave Role Mismatch (BaseT1 and 1000BaseT PHYs)(主/从角色失配 (BaseT1 和 1000BaseT PHY))
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- In **BaseT1** systems (e.g., 1000BaseT1, 100BaseT1), link establishment
  requires that one device is configured as **master** and the other as
  **slave**. A mismatch in this master-slave configuration can prevent the link
  from being established. However, **1000BaseT** also supports configurable
  master/slave roles and can face similar issues.
  (在 **BaseT1** 系统 (例如 1000BaseT1、100BaseT1) 中，建立链路要求一个设备配置为
  **主 (master)** ，另一个配置为 **从 (slave)** 。主从配置失配会阻止链路建立。
  不过， **1000BaseT** 也支持可配置的主/从角色，并可能面临类似问题。)

- **Role Preference in 1000BaseT**: The **1000BaseT** specification allows link
  partners to negotiate master-slave roles or role preferences during
  autonegotiation. Some PHYs have hardware limitations or bugs that prevent
  them from functioning properly in certain roles. In such cases, drivers may
  force these PHYs into a specific role (e.g., **forced master** or **forced
  slave**) or try a weaker option by setting preferences. If both link partners
  have the same issue and are forced into the same mode (e.g., both forced into
  master mode), they will not be able to establish a link.
  (**1000BaseT 中的角色偏好** ： **1000BaseT** 规范允许链路对端在自协商期间协商
  主从角色或角色偏好。某些 PHY 存在硬件限制或缺陷，导致它们在某些角色中无法正常
  工作。在这种情况下，驱动可能会将这些 PHY 强制为特定角色 (例如 **强制主
  (forced master)** 或 **强制从 (forced slave)** )，或者通过设置偏好来尝试较弱的
  选项。如果两个链路对端存在相同的问题并被强制为相同的模式 (例如都被强制为主
  模式)，它们将无法建立链路。)

- **Next Steps**: Ensure that one side is configured as **master** and the
  other as **slave** to avoid this issue, particularly when hardware
  limitations are involved, or try the weaker **preferred** option instead of
  **forced**. Check for any driver-related restrictions or forced modes.
  (**后续步骤** ：确保一侧配置为 **主 (master)** ，另一侧配置为 **从 (slave)** ，
  以避免此问题，特别是在涉及硬件限制时；或者尝试使用较弱的 **preferred** 选项
  而非 **forced** 。检查是否存在任何与驱动相关的限制或强制模式。)

- **Command to force master/slave mode**: (强制主/从模式的命令)

  .. code-block:: bash

     ethtool -s <interface> master-slave forced-master

  or:

  .. code-block:: bash

     ethtool -s <interface> master-slave forced-master speed 1000 duplex full autoneg off


- **Check the current master/slave status**: (检查当前的主/从状态)

  .. code-block:: bash

     ethtool <interface>

  Example Output: (示例输出：)

  .. code-block:: bash

     master-slave cfg: forced-master
     master-slave status: master

- **Hardware Bugs and Driver Forcing**: If a known hardware issue forces the
  PHY into a specific mode, it’s essential to check the driver source code or
  hardware documentation for details. Ensure that the roles are compatible
  across both link partners, and if both PHYs are forced into the same mode,
  adjust one side accordingly to resolve the mismatch.
  (**硬件缺陷与驱动强制** ：如果已知的硬件问题将 PHY 强制为特定模式，则必须检查
  驱动源代码或硬件文档以了解详情。确保两个链路对端的角色兼容；如果两个 PHY 都被
  强制为相同的模式，请相应地调整一侧以解决失配。)

Monitor Link Resets and Speed Drops(监控链路重置与速率下降)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If the link is unstable, showing frequent resets or speed drops, this may
indicate issues with the cable, PHY configuration, or environmental factors.
While there is still no completely unified way in Linux to directly monitor
downshift events or link speed changes via user space tools, both the Linux
kernel logs and `ethtool` can provide valuable insights, especially if the
driver supports reporting such events.
(如果链路不稳定，频繁出现重置或速率下降，这可能表明电缆、PHY 配置或环境因素存在
问题。虽然 Linux 中仍然没有完全统一的方式可以通过用户空间工具直接监控降速
(downshift) 事件或链路速率变化，但 Linux 内核日志和 `ethtool` 都能提供有价值的
信息，尤其是当驱动支持报告此类事件时。)

- **Monitor Kernel Logs for Link Resets and Speed Drops**: (监控内核日志中的链路重置与速率下降)

  - The Linux kernel will print link status changes, including downshift
    events, in the system logs. These messages typically include speed changes,
    duplex mode, and downshifted link speed (if the driver supports it).
    (Linux 内核会在系统日志中打印链路状态变化，包括降速事件。这些消息通常包括
    速率变化、双工模式以及降速后的链路速率 (如果驱动支持)。)

  - **Command to monitor kernel logs in real-time:** (实时监控内核日志的命令：)

    .. code-block:: bash

      dmesg -w | grep "Link is Up\|Link is Down"

  - Example Output (if a downshift occurs): (示例输出 (如果发生降速)：)

    .. code-block:: bash

      eth0: Link is Up - 100Mbps/Full (downshifted) - flow control rx/tx
      eth0: Link is Down

    This indicates that the link has been established but has downshifted from
    a higher speed.
    (这表明链路已建立，但已从更高的速率降速。)

  - **Note**: Not all drivers or PHYs support downshift reporting, so you may
    not see this information for all devices.
    (**注意** ：并非所有驱动或 PHY 都支持降速报告，因此你可能不会在所有设备上
    看到此信息。)

- **Monitor Link Down Events Using `ethtool`**: (使用 `ethtool` 监控链路断开事件)

  - Starting with the latest kernel and `ethtool` versions, you can track
    **Link Down Events** using the `ethtool -I` command. This will provide
    counters for link drops, helping to diagnose link instability issues if
    supported by the driver.
    (从最新的内核和 `ethtool` 版本开始，你可以使用 `ethtool -I` 命令跟踪
    **链路断开事件 (Link Down Events)** 。这将提供链路断开的计数器，如果驱动
    支持，有助于诊断链路不稳定问题。)

  - **Command to monitor link down events:** (监控链路断开事件的命令：)

    .. code-block:: bash

      ethtool -I <interface>

  - Example Output (if supported): (示例输出 (如果支持)：)

    .. code-block:: bash

      PSE attributes for eth1:
      Link Down Events: 5

    This indicates that the link has dropped 5 times. Frequent link down events
    may indicate cable or environmental issues that require further
    investigation.
    (这表明链路已断开 5 次。频繁的链路断开事件可能表明存在需要进一步排查的电缆
    或环境问题。)

- **Check Link Status and Speed**: (检查链路状态与速率)

  - Even though downshift counts or events are not easily tracked, you can
    still use `ethtool` to manually check the current link speed and status.
    (尽管降速计数或事件不易跟踪，你仍然可以使用 `ethtool` 手动检查当前的链路
    速率和状态。)

  - **Command:** `ethtool <interface>` (命令)

  - **Expected Output:** (预期输出：)

    .. code-block:: bash

      Speed: 1000Mb/s
      Duplex: Full
      Auto-negotiation: on
      Link detected: yes

    Any inconsistencies in the expected speed or duplex setting could indicate
    an issue.
    (预期速率或双工设置中的任何不一致都可能表明存在问题。)

- **Disable Energy-Efficient Ethernet (EEE) for Diagnostics**: (诊断时禁用节能以太网 (EEE))

  - **EEE** (Energy-Efficient Ethernet) can be a source of link instability due
    to transitions in and out of low-power states. For diagnostic purposes, it
    may be useful to **temporarily** disable EEE to determine if it is
    contributing to link instability. This is **not a generic recommendation**
    for disabling power management.
    (**EEE** (节能以太网，Energy-Efficient Ethernet) 由于进出低功耗状态的转换，
    可能是链路不稳定的一个来源。出于诊断目的， **临时** 禁用 EEE 以确定它是否是
    链路不稳定的诱因可能有帮助。这不是禁用电源管理的 **通用建议** 。)

  - **Next Steps**: Disable EEE and monitor if the link becomes stable. If
    disabling EEE resolves the issue, report the bug so that the driver can be
    fixed.
    (**后续步骤** ：禁用 EEE 并观察链路是否变得稳定。如果禁用 EEE 解决了问题，
    请报告该缺陷以便修复驱动。)

  - **Command:** (命令：)

    .. code-block:: bash

      ethtool --set-eee <interface> eee off

  - **Important**: If disabling EEE resolves the instability, the issue should
    be reported to the maintainers as a bug, and the driver should be corrected
    to handle EEE properly without causing instability. Disabling EEE
    permanently should not be seen as a solution.
    (**重要** ：如果禁用 EEE 解决了不稳定问题，应将该问题作为缺陷报告给维护者，
    并修正驱动使其正确处理 EEE 而不引起不稳定。不应将永久禁用 EEE 视为解决
    方案。)

- **Monitor Error Counters**: (监控错误计数器)

  - Use `ethtool -S <interface> --all-groups` to retrieve standardized interface
    statistics if the driver supports the unified interface:
    (如果驱动支持统一接口，使用 `ethtool -S <interface> --all-groups` 获取
    标准化的接口统计信息：)

  - **Command:** `ethtool -S <interface> --all-groups` (命令)

  - **Example Output (if supported)**: (示例输出 (如果支持)：)

    .. code-block:: bash

      phydev-RxFrames: 100391
      phydev-RxErrors: 0
      phydev-TxFrames: 9
      phydev-TxErrors: 0

  - If the unified interface is not supported, use `ethtool -S <interface>` to
    retrieve MAC and PHY counters. Note that non-standardized PHY counter names
    vary by driver and must be interpreted accordingly:
    (如果不支持统一接口，使用 `ethtool -S <interface>` 获取 MAC 和 PHY 计数器。
    注意，非标准化的 PHY 计数器名称因驱动而异，必须相应地进行解读：)

  - **Command:** `ethtool -S <interface>` (命令)

  - **Example Output (if supported)**: (示例输出 (如果支持)：)

    .. code-block:: bash

      rx_crc_errors: 123
      tx_errors: 45
      rx_frame_errors: 78

  - **Note**: If no meaningful error counters are available or if counters are
    not supported, you may need to rely on physical inspections (e.g., cable
    condition) or kernel log messages (e.g., link up/down events) to further
    diagnose the issue.
    (**注意** ：如果没有可用的有意义的错误计数器，或计数器不受支持，你可能需要
    依靠物理检查 (例如电缆状况) 或内核日志消息 (例如链路建立/断开事件) 来进一步
    诊断问题。)

  - **Compare Counters**: (比较计数器)

    - Compare the egress and ingress frame counts reported by the PHY and MAC.
      (比较 PHY 和 MAC 报告的发送与接收帧计数。)

    - A small difference may occur due to sampling rate differences between the
      MAC and PHY drivers, or if the PHY and MAC are not always fully
      synchronized in their UP or DOWN states.
      (由于 MAC 和 PHY 驱动之间的采样率差异，或者 PHY 和 MAC 并非总是完全同步地
      处于 UP 或 DOWN 状态，可能出现较小的差异。)

    - Significant discrepancies indicate potential issues in the data path
      between the MAC and PHY.
      (显著的差异表明 MAC 与 PHY 之间的数据路径可能存在问题。)

When All Else Fails...(当一切都失败了……)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

So you've checked the cables, monitored the logs, disabled EEE, and still...
nothing? Don’t worry, you’re not alone. Sometimes, Ethernet gremlins just don’t
want to cooperate.
(你已经检查了电缆、监控了日志、禁用了 EEE，结果仍然……没有任何发现？别担心，你
并不孤单。有时候，以太网小妖精就是不肯配合。)

But before you throw in the towel (or the Ethernet cable), take a deep breath.
It’s always possible that:
(但在放弃 (或者扔掉以太网电缆) 之前，先深呼吸。以下情况始终是有可能的：)

1. Your PHY has a unique, undocumented personality.
   (你的 PHY 有着独特的、未记载的个性。)

2. The problem is lying dormant, waiting for just the right moment to magically
   resolve itself (hey, it happens!).
   (问题正处于休眠状态，等待着恰当的时机自行神奇地解决 (嘿，这种事确实会发生！)。)

3. Or, it could be that the ultimate solution simply hasn’t been invented yet.
   (又或者，终极解决方案压根还没被发明出来。)

If none of the above bring you comfort, there’s one final step: contribute! If
you've uncovered new or unusual issues, or have creative diagnostic methods,
feel free to share your findings and extend this documentation. Together, we
can hunt down every elusive network issue - one twisted pair at a time.
(如果以上内容都不能给你带来安慰，还有最后一步：贡献！如果你发现了新的或不寻常的
问题，或者有创造性的诊断方法，欢迎分享你的发现并扩展本文档。让我们携手追捕每一个
难以捉摸的网络问题——一次解决一对双绞线。)

Remember: sometimes the solution is just a reboot away, but if not, it’s time to
dig deeper - or report that bug!
(记住：有时候解决方案只差一次重启，但如果不是，那就该深入挖掘——或者报告那个
缺陷！)

