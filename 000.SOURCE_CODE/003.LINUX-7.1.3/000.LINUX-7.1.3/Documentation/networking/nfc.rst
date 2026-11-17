===================
Linux NFC subsystem
===================

(Linux NFC 子系统)

The Near Field Communication (NFC) subsystem is required to standardize the
NFC device drivers development and to create an unified userspace interface.
(近场通信 (NFC) 子系统用于规范 NFC 设备驱动的开发，并创建统一的用户空间
接口。)

This document covers the architecture overview, the device driver interface
description and the userspace interface description.
(本文档涵盖架构概述、设备驱动接口描述以及用户空间接口描述。)

Architecture overview
(架构概述)
=====================

The NFC subsystem is responsible for:
(NFC 子系统负责：)

      - NFC adapters management;
        (NFC 适配器管理；)
      - Polling for targets;
        (轮询目标；)
      - Low-level data exchange;
        (底层数据交换；)

The subsystem is divided in some parts. The 'core' is responsible for
providing the device driver interface. On the other side, it is also
responsible for providing an interface to control operations and low-level
data exchange.
(该子系统分为若干部分。'core' 负责提供设备驱动接口。另一方面，它还负责
提供控制操作和底层数据交换的接口。)

The control operations are available to userspace via generic netlink.
(控制操作通过 generic netlink 提供给用户空间。)

The low-level data exchange interface is provided by the new socket family
PF_NFC. The NFC_SOCKPROTO_RAW performs raw communication with NFC targets.
(底层数据交换接口由新的套接字族 PF_NFC 提供。NFC_SOCKPROTO_RAW 执行与
NFC 目标的原始通信。)

.. code-block:: none

        +--------------------------------------+
        |              USER SPACE              |
        +--------------------------------------+
            ^                       ^
            | low-level             | control
            | data exchange         | operations
            |                       |
            |                       v
            |                  +-----------+
            | AF_NFC           |  netlink  |
            | socket           +-----------+
            | raw                   ^
            |                       |
            v                       v
        +---------+            +-----------+
        | rawsock | <--------> |   core    |
        +---------+            +-----------+
                                    ^
                                    |
                                    v
                               +-----------+
                               |  driver   |
                               +-----------+

Device Driver Interface
(设备驱动接口)
=======================

When registering on the NFC subsystem, the device driver must inform the core
of the set of supported NFC protocols and the set of ops callbacks. The ops
callbacks that must be implemented are the following:
(在 NFC 子系统注册时，设备驱动必须向 core 告知其支持的 NFC 协议集合以及
ops 回调集合。必须实现的 ops 回调如下：)

* start_poll - setup the device to poll for targets
  (start_poll - 设置设备以轮询目标)
* stop_poll - stop on progress polling operation
  (stop_poll - 停止正在进行的轮询操作)
* activate_target - select and initialize one of the targets found
  (activate_target - 选择并初始化所发现的目标之一)
* deactivate_target - deselect and deinitialize the selected target
  (deactivate_target - 取消选择并反初始化已选择的目标)
* data_exchange - send data and receive the response (transceive operation)
  (data_exchange - 发送数据并接收响应 (收发操作))

Userspace interface
(用户空间接口)
===================

The userspace interface is divided in control operations and low-level data
exchange operation.
(用户空间接口分为控制操作和底层数据交换操作。)

Control operations
(控制操作)
------------------

Generic netlink is used to implement the interface to the control operations.
The operations are composed by commands and events, all listed below:
(generic netlink 用于实现控制操作接口。操作由命令和事件组成，全部列示
如下：)

* NFC_CMD_GET_DEVICE - get specific device info or dump the device list
  (NFC_CMD_GET_DEVICE - 获取特定设备信息或转储设备列表)
* NFC_CMD_START_POLL - setup a specific device to polling for targets
  (NFC_CMD_START_POLL - 设置特定设备以轮询目标)
* NFC_CMD_STOP_POLL - stop the polling operation in a specific device
  (NFC_CMD_STOP_POLL - 停止特定设备上的轮询操作)
* NFC_CMD_GET_TARGET - dump the list of targets found by a specific device
  (NFC_CMD_GET_TARGET - 转储特定设备发现的目标列表)

* NFC_EVENT_DEVICE_ADDED - reports an NFC device addition
  (NFC_EVENT_DEVICE_ADDED - 报告 NFC 设备的添加)
* NFC_EVENT_DEVICE_REMOVED - reports an NFC device removal
  (NFC_EVENT_DEVICE_REMOVED - 报告 NFC 设备的移除)
* NFC_EVENT_TARGETS_FOUND - reports START_POLL results when 1 or more targets
  are found
  (NFC_EVENT_TARGETS_FOUND - 当发现 1 个或多个目标时报告 START_POLL 的
  结果)

The user must call START_POLL to poll for NFC targets, passing the desired NFC
protocols through NFC_ATTR_PROTOCOLS attribute. The device remains in polling
state until it finds any target. However, the user can stop the polling
operation by calling STOP_POLL command. In this case, it will be checked if
the requester of STOP_POLL is the same of START_POLL.
(用户必须调用 START_POLL 来轮询 NFC 目标，并通过 NFC_ATTR_PROTOCOLS 属性
传递所需的 NFC 协议。设备将保持轮询状态，直到发现任何目标。不过，用户可以
通过调用 STOP_POLL 命令来停止轮询操作。这种情况下，会检查 STOP_POLL 的
请求者是否与 START_POLL 的请求者相同。)

If the polling operation finds one or more targets, the event TARGETS_FOUND is
sent (including the device id). The user must call GET_TARGET to get the list of
all targets found by such device. Each reply message has target attributes with
relevant information such as the supported NFC protocols.
(如果轮询操作发现了一个或多个目标，就会发送 TARGETS_FOUND 事件 (包括设备
id)。用户必须调用 GET_TARGET 来获取该设备发现的全部目标列表。每条应答消息
都带有目标属性，其中包含诸如所支持的 NFC 协议等相关信息。)

All polling operations requested through one netlink socket are stopped when
it's closed.
(通过某个 netlink 套接字请求的所有轮询操作都会在该套接字关闭时停止。)

Low-level data exchange
(底层数据交换)
-----------------------

The userspace must use PF_NFC sockets to perform any data communication with
targets. All NFC sockets use AF_NFC::
(用户空间必须使用 PF_NFC 套接字来执行与目标的任何数据通信。所有 NFC 套接字
都使用 AF_NFC：)

        struct sockaddr_nfc {
               sa_family_t sa_family;
               __u32 dev_idx;
               __u32 target_idx;
               __u32 nfc_protocol;
        };

To establish a connection with one target, the user must create an
NFC_SOCKPROTO_RAW socket and call the 'connect' syscall with the sockaddr_nfc
struct correctly filled. All information comes from NFC_EVENT_TARGETS_FOUND
netlink event. As a target can support more than one NFC protocol, the user
must inform which protocol it wants to use.
(要与某个目标建立连接，用户必须创建 NFC_SOCKPROTO_RAW 套接字，并在正确
填写 sockaddr_nfc 结构后调用 'connect' 系统调用。所有信息都来自
NFC_EVENT_TARGETS_FOUND netlink 事件。由于一个目标可以支持多个 NFC 协议，
用户必须指明想要使用的协议。)

Internally, 'connect' will result in an activate_target call to the driver.
When the socket is closed, the target is deactivated.
(在内部，'connect' 会最终触发对驱动的 activate_target 调用。当套接字关闭
时，目标被去激活。)

The data format exchanged through the sockets is NFC protocol dependent. For
instance, when communicating with MIFARE tags, the data exchanged are MIFARE
commands and their responses.
(通过套接字交换的数据格式取决于 NFC 协议。例如，与 MIFARE 标签通信时，
交换的数据是 MIFARE 命令及其响应。)

The first received package is the response to the first sent package and so
on. In order to allow valid "empty" responses, every data received has a NULL
header of 1 byte.
(第一个收到的数据包是对第一个发送的数据包的响应，依此类推。为了允许有效的
"空" 应答，每个收到的数据都有一个 1 字节的 NULL 头。)
