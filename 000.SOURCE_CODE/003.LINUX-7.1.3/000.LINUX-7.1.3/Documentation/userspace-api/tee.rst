.. SPDX-License-Identifier: GPL-2.0
.. tee:

==================================================================================
TEE (Trusted Execution Environment) Userspace API(TEE（可信执行环境）用户空间 API)
==================================================================================

include/uapi/linux/tee.h defines the generic interface to a TEE.
(include/uapi/linux/tee.h 定义了 TEE 的通用接口。)

User space (the client) connects to the driver by opening /dev/tee[0-9]* or
/dev/teepriv[0-9]*.
(用户空间（客户端）通过打开 /dev/tee[0-9]* 或 /dev/teepriv[0-9]* 连接到
驱动。)

- TEE_IOC_SHM_ALLOC allocates shared memory and returns a file descriptor
  which user space can mmap. When user space doesn't need the file
  descriptor any more, it should be closed. When shared memory isn't needed
  any longer it should be unmapped with munmap() to allow the reuse of
  memory.
  (TEE_IOC_SHM_ALLOC 分配共享内存并返回一个可供用户空间 mmap 的文件描述符。
  当用户空间不再需要该文件描述符时应将其关闭；当不再需要共享内存时，应使用
  munmap() 解除映射，以便复用内存。)

- TEE_IOC_VERSION lets user space know which TEE this driver handles and
  its capabilities.
  (TEE_IOC_VERSION 让用户空间知道该驱动处理的是哪个 TEE 及其能力。)

- TEE_IOC_OPEN_SESSION opens a new session to a Trusted Application.
  (TEE_IOC_OPEN_SESSION 打开与可信应用（Trusted Application）的新会话。)

- TEE_IOC_INVOKE invokes a function in a Trusted Application.
  (TEE_IOC_INVOKE 调用可信应用中的一个函数。)

- TEE_IOC_CANCEL may cancel an ongoing TEE_IOC_OPEN_SESSION or TEE_IOC_INVOKE.
  (TEE_IOC_CANCEL 可取消进行中的 TEE_IOC_OPEN_SESSION 或 TEE_IOC_INVOKE。)

- TEE_IOC_CLOSE_SESSION closes a session to a Trusted Application.
  (TEE_IOC_CLOSE_SESSION 关闭与可信应用的会话。)

There are two classes of clients, normal clients and supplicants. The latter is
a helper process for the TEE to access resources in Linux, for example file
system access. A normal client opens /dev/tee[0-9]* and a supplicant opens
/dev/teepriv[0-9].
(客户端分为两类：普通客户端与 supplicant（求助者）。后者是 TEE 用于访问
Linux 中资源（例如文件系统访问）的辅助进程。普通客户端打开 /dev/tee[0-9]*，
而 supplicant 打开 /dev/teepriv[0-9]。)

Much of the communication between clients and the TEE is opaque to the
driver. The main job for the driver is to receive requests from the
clients, forward them to the TEE and send back the results. In the case of
supplicants the communication goes in the other direction, the TEE sends
requests to the supplicant which then sends back the result.
(客户端与 TEE 之间的通信大部分对驱动是不透明的。驱动的主要工作是接收客户端
的请求，转发给 TEE 并把结果送回。对于 supplicant，通信方向相反：TEE 向
supplicant 发送请求，由其送回结果。)
