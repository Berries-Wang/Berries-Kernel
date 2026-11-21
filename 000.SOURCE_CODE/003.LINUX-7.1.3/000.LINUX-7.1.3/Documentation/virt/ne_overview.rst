.. SPDX-License-Identifier: GPL-2.0

==========================
Nitro Enclaves(Nitro 飞地)
==========================

Overview(概述)
==============

Nitro Enclaves (NE) is a new Amazon Elastic Compute Cloud (EC2) capability
that allows customers to carve out isolated compute environments within EC2
instances [1].
(Nitro Enclaves (NE) 是 Amazon Elastic Compute Cloud (EC2) 的一项新能力，
允许客户在 EC2 实例内划出隔离的计算环境 [1]。)

For example, an application that processes sensitive data and runs in a VM,
can be separated from other applications running in the same VM. This
application then runs in a separate VM than the primary VM, namely an enclave.
It runs alongside the VM that spawned it. This setup matches low latency
applications needs.
(例如，一个在 VM 中处理敏感数据的应用程序，可以与同一 VM 中运行的
其他应用隔离开。此后该应用程序运行在与主 VM 不同的另一个 VM 中，即
enclave(飞地)。它与生成它的那个 VM 并肩运行。这种设置符合低延迟应用
的需求。)

The current supported architectures for the NE kernel driver, available in the
upstream Linux kernel, are x86 and ARM64.
(上游 Linux 内核中的 NE 内核驱动当前支持的架构是 x86 和 ARM64。)

The resources that are allocated for the enclave, such as memory and CPUs, are
carved out of the primary VM. Each enclave is mapped to a process running in the
primary VM, that communicates with the NE kernel driver via an ioctl interface.
(为 enclave 分配的资源(如内存和 CPU)是从主 VM 中划出的。每个 enclave
都映射到主 VM 中运行的一个进程，该进程通过 ioctl 接口与 NE 内核驱动
通信。)

In this sense, there are two components:
(从这个意义上说，有两个组件：)

1. An enclave abstraction process - a user space process running in the primary
VM guest that uses the provided ioctl interface of the NE driver to spawn an
enclave VM (that's 2 below).
(1. enclave 抽象进程——运行在主 VM guest 中的用户空间进程，它使用 NE
驱动提供的 ioctl 接口来生成一个 enclave VM(即下面的第 2 项)。)

There is a NE emulated PCI device exposed to the primary VM. The driver for this
new PCI device is included in the NE driver.
(主 VM 会看到一个 NE 模拟的 PCI 设备。这个新 PCI 设备的驱动包含在 NE
驱动中。)

The ioctl logic is mapped to PCI device commands e.g. the NE_START_ENCLAVE ioctl
maps to an enclave start PCI command. The PCI device commands are then
translated into  actions taken on the hypervisor side; that's the Nitro
hypervisor running on the host where the primary VM is running. The Nitro
hypervisor is based on core KVM technology.
(ioctl 逻辑映射到 PCI 设备命令，例如 NE_START_ENCLAVE ioctl 映射到
enclave 启动 PCI 命令。PCI 设备命令随后被转换为在 hypervisor 侧执行的
操作；hypervisor 即主 VM 所运行主机上的 Nitro hypervisor。Nitro
hypervisor 基于核心 KVM 技术。)

2. The enclave itself - a VM running on the same host as the primary VM that
spawned it. Memory and CPUs are carved out of the primary VM and are dedicated
for the enclave VM. An enclave does not have persistent storage attached.
(2. enclave 本身——一个与生成它的主 VM 运行在同一主机上的 VM。内存
和 CPU 从主 VM 中划出并专用于该 enclave VM。enclave 没有挂接持久
存储。)

The memory regions carved out of the primary VM and given to an enclave need to
be aligned 2 MiB / 1 GiB physically contiguous memory regions (or multiple of
this size e.g. 8 MiB). The memory can be allocated e.g. by using hugetlbfs from
user space [2][3][7]. The memory size for an enclave needs to be at least
64 MiB. The enclave memory and CPUs need to be from the same NUMA node.
(从主 VM 中划出并交给 enclave 的内存区域需要是按 2 MiB / 1 GiB 对齐
的物理连续内存区域(或是该大小的倍数，例如 8 MiB)。内存可以例如由
用户空间通过 hugetlbfs 分配 [2][3][7]。enclave 的内存大小至少需要
64 MiB。enclave 的内存和 CPU 需要来自同一个 NUMA 节点。)

An enclave runs on dedicated cores. CPU 0 and its CPU siblings need to remain
available for the primary VM. A CPU pool has to be set for NE purposes by an
user with admin capability. See the cpu list section from the kernel
documentation [4] for how a CPU pool format looks.
(enclave 运行在专用核心上。CPU 0 及其同级 CPU 需要保持对主 VM 可用。
具有管理员权限的用户必须为 NE 设置一个 CPU 池。CPU 池的格式参见内核
文档 [4] 中的 cpu list 一节。)

An enclave communicates with the primary VM via a local communication channel,
using virtio-vsock [5]. The primary VM has virtio-pci vsock emulated device,
while the enclave VM has a virtio-mmio vsock emulated device. The vsock device
uses eventfd for signaling. The enclave VM sees the usual interfaces - local
APIC and IOAPIC - to get interrupts from virtio-vsock device. The virtio-mmio
device is placed in memory below the typical 4 GiB.
(enclave 通过本地通信信道 virtio-vsock [5] 与主 VM 通信。主 VM 拥有
virtio-pci vsock 模拟设备，而 enclave VM 拥有 virtio-mmio vsock 模拟
设备。vsock 设备使用 eventfd 进行信号通知。enclave VM 看到的是常见的
接口——本地 APIC 和 IOAPIC——以从 virtio-vsock 设备获取中断。
virtio-mmio 设备放置在通常的 4 GiB 以下的内存中。)

The application that runs in the enclave needs to be packaged in an enclave
image together with the OS ( e.g. kernel, ramdisk, init ) that will run in the
enclave VM. The enclave VM has its own kernel and follows the standard Linux
boot protocol [6][8].
(在 enclave 中运行的应用程序需要与将在 enclave VM 中运行的 OS(例如
内核、ramdisk、init)一起打包成 enclave 镜像。enclave VM 有自己的
内核，并遵循标准的 Linux 引导协议 [6][8]。)

The kernel bzImage, the kernel command line, the ramdisk(s) are part of the
Enclave Image Format (EIF); plus an EIF header including metadata such as magic
number, eif version, image size and CRC.
(内核 bzImage、内核命令行、ramdisk 是 Enclave Image Format (EIF) 的
组成部分；外加一个 EIF 头，包含诸如 magic number、eif 版本、镜像大小
和 CRC 之类的元数据。)

Hash values are computed for the entire enclave image (EIF), the kernel and
ramdisk(s). That's used, for example, to check that the enclave image that is
loaded in the enclave VM is the one that was intended to be run.
(会为整个 enclave 镜像(EIF)、内核和 ramdisk 计算哈希值。例如，这用于
检查加载到 enclave VM 中的 enclave 镜像是否就是打算运行的那个。)

These crypto measurements are included in a signed attestation document
generated by the Nitro Hypervisor and further used to prove the identity of the
enclave; KMS is an example of service that NE is integrated with and that checks
the attestation doc.
(这些密码学度量值包含在由 Nitro Hypervisor 生成、经过签名的证明文档
(attestation document)中，并进一步用于证明 enclave 的身份；KMS 是
与 NE 集成并检查该证明文档的服务的一个例子。)

The enclave image (EIF) is loaded in the enclave memory at offset 8 MiB. The
init process in the enclave connects to the vsock CID of the primary VM and a
predefined port - 9000 - to send a heartbeat value - 0xb7. This mechanism is
used to check in the primary VM that the enclave has booted. The CID of the
primary VM is 3.
(enclave 镜像 (EIF) 被加载到 enclave 内存中偏移 8 MiB 处。enclave 中的
init 进程连接到主 VM 的 vsock CID 和预定义端口 9000，发送心跳值
0xb7。主 VM 用该机制检查 enclave 是否已经启动。主 VM 的 CID 是 3。)

If the enclave VM crashes or gracefully exits, an interrupt event is received by
the NE driver. This event is sent further to the user space enclave process
running in the primary VM via a poll notification mechanism. Then the user space
enclave process can exit.
(如果 enclave VM 崩溃或正常退出，NE 驱动会收到一个中断事件。该事件
通过 poll 通知机制进一步发送给运行在主 VM 中的用户空间 enclave 进程。
然后用户空间 enclave 进程可以退出。)

[1] https://aws.amazon.com/ec2/nitro/nitro-enclaves/
[2] https://www.kernel.org/doc/html/latest/admin-guide/mm/hugetlbpage.html
[3] https://lwn.net/Articles/807108/
[4] https://www.kernel.org/doc/html/latest/admin-guide/kernel-parameters.html
[5] https://man7.org/linux/man-pages/man7/vsock.7.html
[6] https://www.kernel.org/doc/html/latest/x86/boot.html
[7] https://www.kernel.org/doc/html/latest/arm64/hugetlbpage.html
[8] https://www.kernel.org/doc/html/latest/arm64/booting.html
