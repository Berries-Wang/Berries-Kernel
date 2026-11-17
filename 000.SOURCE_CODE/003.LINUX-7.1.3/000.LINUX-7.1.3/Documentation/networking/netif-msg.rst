.. SPDX-License-Identifier: GPL-2.0

===============================
NETIF Msg Level(NETIF 消息级别)
===============================

The design of the network interface message level setting.
(网络接口消息级别设置的设计。)

History(历史)
-------------

 The design of the debugging message interface was guided and
 constrained by backwards compatibility previous practice.  It is useful
 to understand the history and evolution in order to understand current
 practice and relate it to older driver source code.
 (调试消息接口的设计受到向后兼容既往实践的引导与约束。了解这段历史与
 演变，有助于理解当前实践，并将其与较旧的驱动源码联系起来。)

 From the beginning of Linux, each network device driver has had a local
 integer variable that controls the debug message level.  The message
 level ranged from 0 to 7, and monotonically increased in verbosity.
 (自 Linux 诞生之初，每个网络设备驱动都有一个控制调试消息级别的本地
 整型变量。消息级别从 0 到 7，冗长程度单调递增。)

 The message level was not precisely defined past level 3, but were
 always implemented within +-1 of the specified level.  Drivers tended
 to shed the more verbose level messages as they matured.
 (消息级别在第 3 级之后没有精确定义，但总是在指定级别的 +-1 范围内
 实现。随着驱动逐渐成熟，往往会舍弃更冗长级别的消息。)

   - 0  Minimal messages, only essential information on fatal errors.
     (最少消息，仅限致命错误的必要信息。)
   - 1  Standard messages, initialization status.  No run-time messages
     (标准消息，初始化状态。无运行时消息)
   - 2  Special media selection messages, generally timer-driver.
     (特殊的介质选择消息，通常由定时器驱动。)
   - 3  Interface starts and stops, including normal status messages
     (接口启动与停止，包括正常状态消息)
   - 4  Tx and Rx frame error messages, and abnormal driver operation
     (发送与接收帧错误消息，以及驱动的异常操作)
   - 5  Tx packet queue information, interrupt events.
     (发送数据包队列信息，中断事件。)
   - 6  Status on each completed Tx packet and received Rx packets
     (每个已完成的发送数据包以及接收到的数据包的状态)
   - 7  Initial contents of Tx and Rx packets
     (发送与接收数据包的初始内容)

 Initially this message level variable was uniquely named in each driver
 e.g. "lance_debug", so that a kernel symbolic debugger could locate and
 modify the setting.  When kernel modules became common, the variables
 were consistently renamed to "debug" and allowed to be set as a module
 parameter.
 (最初这个消息级别变量在每个驱动中都有唯一的命名，例如 "lance_debug"，
 以便内核符号调试器能够定位并修改该设置。当内核模块变得普遍时，这些
 变量被统一重命名为 "debug"，并允许作为模块参数进行设置。)

 This approach worked well.  However there is always a demand for
 additional features.  Over the years the following emerged as
 reasonable and easily implemented enhancements
 (这种方式工作得很好。然而，对附加特性的需求总是存在。多年来，以下
 几项作为合理且易于实现的增强而出现：)

   - Using an ioctl() call to modify the level.
     (使用 ioctl() 调用来修改级别。)
   - Per-interface rather than per-driver message level setting.
     (按接口而不是按驱动来设置消息级别。)
   - More selective control over the type of messages emitted.
     (对所发出消息的类型进行更有选择性的控制。)

 The netif_msg recommendation adds these features with only a minor
 complexity and code size increase.
 (netif_msg 建议在仅少量增加复杂度和代码体积的情况下加入这些特性。)

 The recommendation is the following points
 (建议包含以下几点)

  - Retaining the per-driver integer variable "debug" as a module
    parameter with a default level of '1'.
    (保留每驱动整型变量 "debug" 作为模块参数，默认级别为 '1'。)

  - Adding a per-interface private variable named "msg_enable".  The
    variable is a bit map rather than a level, and is initialized as::
    (添加一个名为 "msg_enable" 的每接口私有变量。该变量是一个位图
    而不是级别，其初始化方式为：)

       1 << debug

    Or more precisely::
    (或者更精确地说：)

	debug < 0 ? 0 : 1 << min(sizeof(int)-1, debug)

    Messages should changes from::
    (消息处理应从：)

      if (debug > 1)
	   printk(MSG_DEBUG "%s: ...

    to::
    (变为：)

      if (np->msg_enable & NETIF_MSG_LINK)
	   printk(MSG_DEBUG "%s: ...


The set of message levels is named
(消息级别集合的名称如下)


  =========   ===================	============
  Old level   Name (名称)			Bit position (位位置)
  =========   ===================	============
    0         NETIF_MSG_DRV		0x0001
    1         NETIF_MSG_PROBE		0x0002
    2         NETIF_MSG_LINK		0x0004
    2         NETIF_MSG_TIMER		0x0004
    3         NETIF_MSG_IFDOWN		0x0008
    3         NETIF_MSG_IFUP		0x0008
    4         NETIF_MSG_RX_ERR		0x0010
    4         NETIF_MSG_TX_ERR		0x0010
    5         NETIF_MSG_TX_QUEUED	0x0020
    5         NETIF_MSG_INTR		0x0020
    6         NETIF_MSG_TX_DONE		0x0040
    6         NETIF_MSG_RX_STATUS	0x0040
    7         NETIF_MSG_PKTDATA		0x0080
  =========   ===================	============
