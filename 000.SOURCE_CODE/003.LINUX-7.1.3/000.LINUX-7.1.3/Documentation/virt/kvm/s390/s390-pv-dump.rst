.. SPDX-License-Identifier: GPL-2.0

=============================================================
s390 (IBM Z) Protected Virtualization dumps(受保护虚拟化转储)
=============================================================

Summary(摘要)
-------------

Dumping a VM is an essential tool for debugging problems inside
it. This is especially true when a protected VM runs into trouble as
there's no way to access its memory and registers from the outside
while it's running.
(转储一台虚拟机是调试其内部问题的重要工具。当受保护的虚拟机出现问题时
尤其如此，因为它在运行期间无法从外部访问其内存和寄存器。)

However when dumping a protected VM we need to maintain its
confidentiality until the dump is in the hands of the VM owner who
should be the only one capable of analysing it.
(不过在转储受保护的虚拟机时，我们必须保持其机密性，直到转储数据交到
虚拟机所有者手中——他应当是唯一有能力分析它的人。)

The confidentiality of the VM dump is ensured by the Ultravisor who
provides an interface to KVM over which encrypted CPU and memory data
can be requested. The encryption is based on the Customer
Communication Key which is the key that's used to encrypt VM data in a
way that the customer is able to decrypt.
(虚拟机转储的机密性由 Ultravisor 保证，它向 KVM 提供了一个接口，可以
通过该接口请求加密的 CPU 与内存数据。加密基于客户通信密钥 (Customer
Communication Key)，即用于以客户能够解密的方式加密虚拟机数据的密钥。)


Dump process(转储过程)
----------------------

A dump is done in 3 steps:
(转储分 3 个步骤完成：)

**Initiation**
(初始化。)

This step initializes the dump process, generates cryptographic seeds
and extracts dump keys with which the VM dump data will be encrypted.
(此步骤初始化转储过程，生成加密种子，并提取用于加密虚拟机转储数据的
转储密钥。)

**Data gathering**
(数据收集。)

Currently there are two types of data that can be gathered from a VM:
the memory and the vcpu state.
(目前可以从虚拟机收集两类数据：内存和 vcpu 状态。)

The vcpu state contains all the important registers, general, floating
point, vector, control and tod/timers of a vcpu. The vcpu dump can
contain incomplete data if a vcpu is dumped while an instruction is
emulated with help of the hypervisor. This is indicated by a flag bit
in the dump data. For the same reason it is very important to not only
write out the encrypted vcpu state, but also the unencrypted state
from the hypervisor.
(vcpu 状态包含 vcpu 的所有重要寄存器：通用寄存器、浮点寄存器、向量寄存
器、控制寄存器以及 tod/定时器。如果在借助 hypervisor 仿真指令的同时转储
某个 vcpu，则 vcpu 转储可能包含不完整的数据，转储数据中会有一个标志位
指示这一点。出于同样的原因，不仅要写出加密的 vcpu 状态，还要写出来自
hypervisor 的未加密状态，这一点非常重要。)

The memory state is further divided into the encrypted memory and its
metadata comprised of the encryption tweaks and status flags. The
encrypted memory can simply be read once it has been exported. The
time of the export does not matter as no re-encryption is
needed. Memory that has been swapped out and hence was exported can be
read from the swap and written to the dump target without need for any
special actions.
(内存状态进一步分为加密内存及其元数据，后者由加密调整值 (tweak) 和状态
标志组成。加密内存一旦被导出即可直接读取。导出时间并不重要，因为无需
重新加密。已被换出 (swap out) 因而导出过的内存可以直接从 swap 中读取并
写入转储目标，无需任何特殊操作。)

The tweaks / status flags for the exported pages need to be requested
from the Ultravisor.
(已导出页面的调整值/状态标志需要向 Ultravisor 请求。)

**Finalization**
(收尾。)

The finalization step will provide the data needed to be able to
decrypt the vcpu and memory data and end the dump process. When this
step completes successfully a new dump initiation can be started.
(收尾步骤将提供解密 vcpu 与内存数据所需的数据，并结束转储过程。此步骤
成功完成后，即可开始新一轮转储初始化。)
