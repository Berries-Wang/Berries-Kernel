.. SPDX-License-Identifier: GPL-2.0

I/O request handling(I/O 请求处理)
==================================

An I/O request of a User VM, which is constructed by the hypervisor, is
distributed by the ACRN Hypervisor Service Module to an I/O client
corresponding to the address range of the I/O request. Details of I/O request
handling are described in the following sections.
(由 hypervisor 构造的 User VM 的 I/O 请求，由 ACRN Hypervisor Service
Module 分发给与该 I/O 请求地址范围对应的 I/O 客户端。I/O 请求处理的
细节在以下各节中描述。)

1. I/O request(1. I/O 请求)
---------------------------

For each User VM, there is a shared 4-KByte memory region used for I/O requests
communication between the hypervisor and Service VM. An I/O request is a
256-byte structure buffer, which is 'struct acrn_io_request', that is filled by
an I/O handler of the hypervisor when a trapped I/O access happens in a User
VM. ACRN userspace in the Service VM first allocates a 4-KByte page and passes
the GPA (Guest Physical Address) of the buffer to the hypervisor. The buffer is
used as an array of 16 I/O request slots with each I/O request slot being 256
bytes. This array is indexed by vCPU ID.
(对每个 User VM，都有一个共享的 4 KB 内存区域用于 hypervisor 与
Service VM 之间的 I/O 请求通信。I/O 请求是一个 256 字节的结构缓冲区，
即 'struct acrn_io_request'，当 User VM 中发生被捕获(trapped)的 I/O
访问时，由 hypervisor 的某个 I/O 处理程序填充。Service VM 中的 ACRN
用户空间首先分配一个 4 KB 页面，并把该缓冲区的 GPA(客户物理地址)
传给 hypervisor。该缓冲区用作由 16 个 I/O 请求槽位组成的数组，每个
I/O 请求槽位为 256 字节。这个数组以 vCPU ID 为索引。)

2. I/O clients(2. I/O 客户端)
-----------------------------

An I/O client is responsible for handling User VM I/O requests whose accessed
GPA falls in a certain range. Multiple I/O clients can be associated with each
User VM. There is a special client associated with each User VM, called the
default client, that handles all I/O requests that do not fit into the range of
any other clients. The ACRN userspace acts as the default client for each User
VM.
(I/O 客户端负责处理访问 GPA 落在某个范围内的 User VM I/O 请求。
多个 I/O 客户端可以与每个 User VM 关联。每个 User VM 都有一个特殊的
客户端，称为默认客户端(default client)，它处理所有不属于任何其他
客户端范围的 I/O 请求。ACRN 用户空间充当每个 User VM 的默认客户端。)

Below illustration shows the relationship between I/O requests shared buffer,
I/O requests and I/O clients.

::

     +------------------------------------------------------+
     |                                       Service VM     |
     |+--------------------------------------------------+  |
     ||      +----------------------------------------+  |  |
     ||      | shared page            ACRN userspace  |  |  |
     ||      |    +-----------------+  +------------+ |  |  |
     ||   +----+->| acrn_io_request |<-+  default   | |  |  |
     ||   |  | |  +-----------------+  | I/O client | |  |  |
     ||   |  | |  |       ...       |  +------------+ |  |  |
     ||   |  | |  +-----------------+                 |  |  |
     ||   |  +-|--------------------------------------+  |  |
     ||---|----|-----------------------------------------|  |
     ||   |    |                             kernel      |  |
     ||   |    |            +----------------------+     |  |
     ||   |    |            | +-------------+  HSM |     |  |
     ||   |    +--------------+             |      |     |  |
     ||   |                 | | I/O clients |      |     |  |
     ||   |                 | |             |      |     |  |
     ||   |                 | +-------------+      |     |  |
     ||   |                 +----------------------+     |  |
     |+---|----------------------------------------------+  |
     +----|-------------------------------------------------+
          |
     +----|-------------------------------------------------+
     |  +-+-----------+                                     |
     |  | I/O handler |              ACRN Hypervisor        |
     |  +-------------+                                     |
     +------------------------------------------------------+

(下图展示了 I/O 请求共享缓冲区、I/O 请求和 I/O 客户端之间的关系。)

3. I/O request state transition(3. I/O 请求状态转换)
----------------------------------------------------

The state transitions of an ACRN I/O request are as follows.

::

   FREE -> PENDING -> PROCESSING -> COMPLETE -> FREE -> ...

(ACRN I/O 请求的状态转换如下。)

- FREE: this I/O request slot is empty
  (FREE：此 I/O 请求槽位为空)
- PENDING: a valid I/O request is pending in this slot
  (PENDING：此槽位中有一个待处理的有效 I/O 请求)
- PROCESSING: the I/O request is being processed
  (PROCESSING：I/O 请求正在被处理)
- COMPLETE: the I/O request has been processed
  (COMPLETE：I/O 请求已被处理)

An I/O request in COMPLETE or FREE state is owned by the hypervisor. HSM and
ACRN userspace are in charge of processing the others.
(处于 COMPLETE 或 FREE 状态的 I/O 请求归 hypervisor 所有。HSM 和
ACRN 用户空间负责处理其他状态的请求。)

4. Processing flow of I/O requests(4. I/O 请求的处理流程)
---------------------------------------------------------

a. The I/O handler of the hypervisor will fill an I/O request with PENDING
   state when a trapped I/O access happens in a User VM.
   (a. 当 User VM 中发生被捕获的 I/O 访问时，hypervisor 的 I/O 处理
   程序会以 PENDING 状态填充一个 I/O 请求。)
b. The hypervisor makes an upcall, which is a notification interrupt, to
   the Service VM.
   (b. hypervisor 向 Service VM 发起一次 upcall(一种通知中断)。)
c. The upcall handler schedules a worker to dispatch I/O requests.
   (c. upcall 处理程序调度一个 worker 来分发 I/O 请求。)
d. The worker looks for the PENDING I/O requests, assigns them to different
   registered clients based on the address of the I/O accesses, updates
   their state to PROCESSING, and notifies the corresponding client to handle.
   (d. worker 查找处于 PENDING 状态的 I/O 请求，根据 I/O 访问的地址
   把它们分配给不同的已注册客户端，把其状态更新为 PROCESSING，并通知
   相应的客户端处理。)
e. The notified client handles the assigned I/O requests.
   (e. 被通知的客户端处理分配给它的 I/O 请求。)
f. The HSM updates I/O requests states to COMPLETE and notifies the hypervisor
   of the completion via hypercalls.
   (f. HSM 把 I/O 请求状态更新为 COMPLETE，并通过 hypercall 把完成
   情况通知 hypervisor。)
