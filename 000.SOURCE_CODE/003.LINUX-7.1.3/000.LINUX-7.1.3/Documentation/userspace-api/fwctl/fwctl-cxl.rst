.. SPDX-License-Identifier: GPL-2.0

================================
fwctl cxl driver(fwctl CXL 驱动)
================================

:Author: Dave Jiang
(作者：Dave Jiang)

Overview(概述)
==============

The CXL spec defines a set of commands that can be issued to the mailbox of a
CXL device or switch. It also left room for vendor specific commands to be
issued to the mailbox as well. fwctl provides a path to issue a set of allowed
mailbox commands from user space to the device moderated by the kernel driver.
(CXL 规范定义了一组可以发送到 CXL 设备或交换机邮箱的命令。它还为厂商特定
命令留出了发送到邮箱的空间。fwctl 提供了一条在内核驱动管控下、从用户空间
向设备发送一组被允许的邮箱命令的路径。)

The following 3 commands will be used to support CXL Features:
CXL spec r3.1 8.2.9.6.1 Get Supported Features (Opcode 0500h)
CXL spec r3.1 8.2.9.6.2 Get Feature (Opcode 0501h)
CXL spec r3.1 8.2.9.6.3 Set Feature (Opcode 0502h)
(以下 3 条命令将用于支持 CXL Feature：)

The "Get Supported Features" return data may be filtered by the kernel driver to
drop any features that are forbidden by the kernel or being exclusively used by
the kernel. The driver will set the "Set Feature Size" of the "Get Supported
Features Supported Feature Entry" to 0 to indicate that the Feature cannot be
modified. The "Get Supported Features" command and the "Get Features" falls
under the fwctl policy of FWCTL_RPC_CONFIGURATION.
("Get Supported Features" 的返回数据可以被内核驱动过滤，以丢弃被内核禁止
或被内核独占使用的特性。驱动会把 "Get Supported Features Supported Feature
Entry" 的 "Set Feature Size" 设置为 0，以表明该 Feature 不能被修改。
"Get Supported Features" 命令和 "Get Features" 命令属于 fwctl 的
FWCTL_RPC_CONFIGURATION 策略。)

For "Set Feature" command, the access policy currently is broken down into two
categories depending on the Set Feature effects reported by the device. If the
Set Feature will cause immediate change to the device, the fwctl access policy
must be FWCTL_RPC_DEBUG_WRITE_FULL. The effects for this level are
"immediate config change", "immediate data change", "immediate policy change",
or "immediate log change" for the set effects mask. If the effects are "config
change with cold reset" or "config change with conventional reset", then the
fwctl access policy must be FWCTL_RPC_DEBUG_WRITE or higher.
(对于 "Set Feature" 命令，当前的访问策略根据设备报告的 Set Feature 效果
分为两类。如果 Set Feature 会立即改变设备，则 fwctl 访问策略必须是
FWCTL_RPC_DEBUG_WRITE_FULL。此级别对应 set effects 掩码中的 "immediate
config change"（立即配置变更）、"immediate data change"（立即数据变更）、
"immediate policy change"（立即策略变更）或 "immediate log change"（立即
日志变更）效果。如果效果是 "config change with cold reset"（冷复位后配置
变更）或 "config change with conventional reset"（常规复位后配置变更），
则 fwctl 访问策略必须是 FWCTL_RPC_DEBUG_WRITE 或更高。)

fwctl cxl User API(fwctl CXL 用户 API)
======================================

.. kernel-doc:: include/uapi/fwctl/cxl.h

1. Driver info query(1. 驱动信息查询)
-------------------------------------

First step for the app is to issue the ioctl(FWCTL_CMD_INFO). Successful
invocation of the ioctl implies the Features capability is operational and
returns an all zeros 32bit payload. A ``struct fwctl_info`` needs to be filled
out with the ``fwctl_info.out_device_type`` set to ``FWCTL_DEVICE_TYPE_CXL``.
The return data should be ``struct fwctl_info_cxl`` that contains a reserved
32bit field that should be all zeros.
(应用程序的第一步是发出 ioctl(FWCTL_CMD_INFO)。成功调用该 ioctl 意味着
Features 能力已可用，并返回全零的 32 位负载。需要填写一个 ``struct
fwctl_info``，把 ``fwctl_info.out_device_type`` 设置为
``FWCTL_DEVICE_TYPE_CXL``。返回数据应为 ``struct fwctl_info_cxl``，其中
包含一个应为全零的保留 32 位字段。)

2. Send hardware commands(2. 发送硬件命令)
------------------------------------------

Next step is to send the 'Get Supported Features' command to the driver from
user space via ioctl(FWCTL_RPC). A ``struct fwctl_rpc_cxl`` is pointed to
by ``fwctl_rpc.in``. ``struct fwctl_rpc_cxl.in_payload`` points to
the hardware input structure that is defined by the CXL spec. ``fwctl_rpc.out``
points to the buffer that contains a ``struct fwctl_rpc_cxl_out`` that includes
the hardware output data inlined as ``fwctl_rpc_cxl_out.payload``. This command
is called twice. First time to retrieve the number of features supported.
A second time to retrieve the specific feature details as the output data.
(下一步是从用户空间通过 ioctl(FWCTL_RPC) 向驱动发送 'Get Supported
Features' 命令。``fwctl_rpc.in`` 指向一个 ``struct fwctl_rpc_cxl``。
``struct fwctl_rpc_cxl.in_payload`` 指向由 CXL 规范定义的硬件输入结构。
``fwctl_rpc.out`` 指向包含 ``struct fwctl_rpc_cxl_out`` 的缓冲区，其中以
``fwctl_rpc_cxl_out.payload`` 的形式内联了硬件输出数据。此命令会调用两次：
第一次检索支持的特性数量；第二次检索作为输出数据的特定特性详情。)

After getting the specific feature details, a Get/Set Feature command can be
appropriately programmed and sent. For a "Set Feature" command, the retrieved
feature info contains an effects field that details the resulting
"Set Feature" command will trigger. That will inform the user whether
the system is configured to allowed the "Set Feature" command or not.
(获得特定特性的详情后，就可以适当地编程并发送 Get/Set Feature 命令。对于
"Set Feature" 命令，检索到的特性信息包含一个 effects 字段，详述由此产生的
"Set Feature" 命令将触发的效果。这会告知用户系统是否被配置为允许
"Set Feature" 命令。)

Code example of a Get Feature(获取 Feature 的代码示例)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

        static int cxl_fwctl_rpc_get_test_feature(int fd, struct test_feature *feat_ctx,
                                                  const uint32_t expected_data)
        {
                struct cxl_mbox_get_feat_in *feat_in;
                struct fwctl_rpc_cxl_out *out;
                struct fwctl_rpc rpc = {0};
                struct fwctl_rpc_cxl *in;
                size_t out_size, in_size;
                uint32_t val;
                void *data;
                int rc;

                in_size = sizeof(*in) + sizeof(*feat_in);
                rc = posix_memalign((void **)&in, 16, in_size);
                if (rc)
                        return -ENOMEM;
                memset(in, 0, in_size);
                feat_in = &in->get_feat_in;

                uuid_copy(feat_in->uuid, feat_ctx->uuid);
                feat_in->count = feat_ctx->get_size;

                out_size = sizeof(*out) + feat_ctx->get_size;
                rc = posix_memalign((void **)&out, 16, out_size);
                if (rc)
                        goto free_in;
                memset(out, 0, out_size);

                in->opcode = CXL_MBOX_OPCODE_GET_FEATURE;
                in->op_size = sizeof(*feat_in);

                rpc.size = sizeof(rpc);
                rpc.scope = FWCTL_RPC_CONFIGURATION;
                rpc.in_len = in_size;
                rpc.out_len = out_size;
                rpc.in = (uint64_t)(uint64_t *)in;
                rpc.out = (uint64_t)(uint64_t *)out;

                rc = send_command(fd, &rpc, out);
                if (rc)
                        goto free_all;

                data = out->payload;
                val = le32toh(*(__le32 *)data);
                if (memcmp(&val, &expected_data, sizeof(val)) != 0) {
                        rc = -ENXIO;
                        goto free_all;
                }

        free_all:
                free(out);
        free_in:
                free(in);
                return rc;
        }

Take a look at CXL CLI test directory
<https://github.com/pmem/ndctl/tree/main/test/fwctl.c> for a detailed user code
for examples on how to exercise this path.
(要了解如何使用这条路径的详细用户代码示例，请查看 CXL CLI 测试目录
<https://github.com/pmem/ndctl/tree/main/test/fwctl.c>。)


fwctl cxl Kernel API(fwctl CXL 内核 API)
========================================

.. kernel-doc:: drivers/cxl/core/features.c
   :export:
.. kernel-doc:: include/cxl/features.h
