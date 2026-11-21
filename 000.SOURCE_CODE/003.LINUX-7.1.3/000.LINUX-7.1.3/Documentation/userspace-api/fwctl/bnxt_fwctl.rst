.. SPDX-License-Identifier: GPL-2.0

==================================
fwctl bnxt driver(fwctl bnxt 驱动)
==================================

:Author: Pavan Chebbi
(作者：Pavan Chebbi)

Overview(概述)
==============

BNXT driver makes a fwctl service available through an auxiliary_device.
The bnxt_fwctl driver binds to this device and registers itself with the
fwctl subsystem.
(BNXT 驱动通过一个 auxiliary_device 提供 fwctl 服务。bnxt_fwctl 驱动
绑定到该设备，并把自身注册到 fwctl 子系统。)

The bnxt_fwctl driver is agnostic to the device firmware internals. It
uses the Upper Layer Protocol (ULP) conduit provided by bnxt to send
HardWare Resource Manager (HWRM) commands to firmware.
(bnxt_fwctl 驱动不关心设备固件的内部细节。它使用 bnxt 提供的上层协议
(ULP) 通道，向固件发送硬件资源管理器 (HWRM) 命令。)

These commands can query or change firmware driven device configurations
and read/write registers that are useful for debugging.
(这些命令可以查询或更改由固件驱动的设备配置，并读写对调试有用的寄存
器。)

bnxt_fwctl User API(bnxt_fwctl 用户 API)
========================================

Each RPC request contains the HWRM input structure in the fwctl_rpc
'in' buffer while 'out' will contain the response.
(每个 RPC 请求都在 fwctl_rpc 的 'in' 缓冲区中包含 HWRM 输入结构，而
'out' 将包含响应。)

A typical user application can send a FWCTL_INFO command using ioctl()
to discover bnxt_fwctl's RPC capabilities as shown below:
(典型的用户应用程序可以使用 ioctl() 发送 FWCTL_INFO 命令，以发现
bnxt_fwctl 的 RPC 能力，如下所示：)

        ioctl(fd, FWCTL_INFO, &fwctl_info_msg);

where fwctl_info_msg (of type struct fwctl_info) describes bnxt_info_msg
(of type struct fwctl_info_bnxt). fwctl_info_msg is set up as follows:
(其中 fwctl_info_msg (类型为 struct fwctl_info) 描述 bnxt_info_msg
(类型为 struct fwctl_info_bnxt)。fwctl_info_msg 按如下方式设置：)

        size = sizeof(struct fwctl_info);
        flags = 0;
        device_data_len = sizeof(bnxt_info_msg);
        out_device_data = (__aligned_u64)&bnxt_info_msg;

The uctx_caps of bnxt_info_msg represents the capabilities as described
in fwctl_bnxt_commands of include/uapi/fwctl/bnxt.h
( ``bnxt_info_msg`` 的 ``uctx_caps`` 表示
include/uapi/fwctl/bnxt.h 中 ``fwctl_bnxt_commands`` 所描述的能力。)

The FW RPC itself, FWCTL_RPC can be sent using ioctl() as:
(FW RPC 本身 (FWCTL_RPC) 可以使用 ioctl() 发送：)

        ioctl(fd, FWCTL_RPC, &fwctl_rpc_msg);

where fwctl_rpc_msg (of type struct fwctl_rpc) carries the HWRM command
in its 'in' buffer. The HWRM input structures are described in
include/linux/bnxt/hsi.h. An example for HWRM_VER_GET is shown below:
(其中 fwctl_rpc_msg (类型为 struct fwctl_rpc) 在其 'in' 缓冲区中携带
HWRM 命令。HWRM 输入结构在 include/linux/bnxt/hsi.h 中描述。下面是一
个 HWRM_VER_GET 的示例：)

        struct hwrm_ver_get_output resp;
        struct fwctl_rpc fwctl_rpc_msg;
        struct hwrm_ver_get_input req;

        req.req_type = HWRM_VER_GET;
        req.hwrm_intf_maj = HWRM_VERSION_MAJOR;
        req.hwrm_intf_min = HWRM_VERSION_MINOR;
        req.hwrm_intf_upd = HWRM_VERSION_UPDATE;
        req.cmpl_ring = -1;
        req.target_id = -1;

        fwctl_rpc_msg.size = sizeof(struct fwctl_rpc);
        fwctl_rpc_msg.scope = FWCTL_RPC_DEBUG_READ_ONLY;
        fwctl_rpc_msg.in_len = sizeof(req);
        fwctl_rpc_msg.out_len = sizeof(resp);
        fwctl_rpc_msg.in = (__aligned_u64)&req;
        fwctl_rpc_msg.out = (__aligned_u64)&resp;

An example python3 program that can exercise this interface can be found in
the following git repository:
(一个可以演练此接口的 python3 示例程序可在以下 git 仓库中找到：)

https://github.com/Broadcom/fwctl-tools
