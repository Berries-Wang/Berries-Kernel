.. SPDX-License-Identifier: GPL-2.0

.. _virtiofs_index:

===================================================
virtiofs: virtio-fs host<->guest shared file system
===================================================

(virtiofs: virtio-fs 主机<->客户机共享文件系统)

- Copyright (C) 2019 Red Hat, Inc.
(版权所有 (C) 2019 Red Hat, Inc.)

Introduction
(简介)
============
The virtiofs file system for Linux implements a driver for the paravirtualized
VIRTIO "virtio-fs" device for guest<->host file system sharing.  It allows a
guest to mount a directory that has been exported on the host.
(Linux 的 virtiofs 文件系统为用于客户机<->主机文件系统共享的半虚拟化 VIRTIO "virtio-fs" 设备实现了一个驱动程序。它允许客户机挂载在主机上导出的目录。)

Guests often require access to files residing on the host or remote systems.
Use cases include making files available to new guests during installation,
booting from a root file system located on the host, persistent storage for
stateless or ephemeral guests, and sharing a directory between guests.
(客户机经常需要访问位于主机或远程系统上的文件。使用案例包括：在安装期间向新客户机提供文件、从位于主机上的根文件系统引导、为无状态或临时客户机提供持久存储，以及在客户机之间共享目录。)

Although it is possible to use existing network file systems for some of these
tasks, they require configuration steps that are hard to automate and they
expose the storage network to the guest.  The virtio-fs device was designed to
solve these problems by providing file system access without networking.
(虽然可以对其中一些任务使用现有的网络文件系统，但它们需要难以自动化的配置步骤，并且会向客户机暴露存储网络。virtio-fs 设备的设计目标是通过提供无需网络的文件系统访问来解决这些问题。)

Furthermore the virtio-fs device takes advantage of the co-location of the
guest and host to increase performance and provide semantics that are not
possible with network file systems.
(此外，virtio-fs 设备利用客户机与主机共处一地的优势来提高性能，并提供网络文件系统无法实现的语义。)

Usage
(用法)
=====
Mount file system with tag ``myfs`` on ``/mnt``:
(将带有标签 ``myfs`` 的文件系统挂载到 ``/mnt``：)

.. code-block:: sh

  guest# mount -t virtiofs myfs /mnt

Please see https://virtio-fs.gitlab.io/ for details on how to configure QEMU
and the virtiofsd daemon.
(有关如何配置 QEMU 和 virtiofsd 守护进程的详细信息，请参见 https://virtio-fs.gitlab.io/。)

Mount options
(挂载选项)
-------------

virtiofs supports general VFS mount options, for example, remount,
ro, rw, context, etc. It also supports FUSE mount options.
(virtiofs 支持通用的 VFS 挂载选项，例如 remount、ro、rw、context 等。它还支持 FUSE 挂载选项。)

atime behavior
(atime 行为)
^^^^^^^^^^^^^^

The atime-related mount options, for example, noatime, strictatime,
are ignored. The atime behavior for virtiofs is the same as the
underlying filesystem of the directory that has been exported
on the host.
(与 atime 相关的挂载选项(例如 noatime、strictatime)会被忽略。virtiofs 的 atime 行为与主机上所导出目录的底层文件系统相同。)

Internals
(内部实现)
=========
Since the virtio-fs device uses the FUSE protocol for file system requests, the
virtiofs file system for Linux is integrated closely with the FUSE file system
client.  The guest acts as the FUSE client while the host acts as the FUSE
server.  The /dev/fuse interface between the kernel and userspace is replaced
with the virtio-fs device interface.
(由于 virtio-fs 设备对文件系统请求使用 FUSE 协议，Linux 的 virtiofs 文件系统与 FUSE 文件系统客户端紧密集成。客户机充当 FUSE 客户端，而主机充当 FUSE 服务器。内核与用户空间之间的 /dev/fuse 接口被替换为 virtio-fs 设备接口。)

FUSE requests are placed into a virtqueue and processed by the host.  The
response portion of the buffer is filled in by the host and the guest handles
the request completion.
(FUSE 请求被放入 virtqueue 并由主机处理。缓冲区的响应部分由主机填写，客户机负责处理请求完成。)

Mapping /dev/fuse to virtqueues requires solving differences in semantics
between /dev/fuse and virtqueues.  Each time the /dev/fuse device is read, the
FUSE client may choose which request to transfer, making it possible to
prioritize certain requests over others.  Virtqueues have queue semantics and
it is not possible to change the order of requests that have been enqueued.
This is especially important if the virtqueue becomes full since it is then
impossible to add high priority requests.  In order to address this difference,
the virtio-fs device uses a "hiprio" virtqueue specifically for requests that
have priority over normal requests.
(将 /dev/fuse 映射到 virtqueue 需要解决 /dev/fuse 与 virtqueue 之间的语义差异。每次读取 /dev/fuse 设备时，FUSE 客户端可以选择传输哪个请求，从而可以优先处理某些请求。virtqueue 具有队列语义，已入队请求的顺序无法改变。当 virtqueue 变满时这一点尤为重要，因为此时无法添加高优先级请求。为了解决这一差异，virtio-fs 设备使用了一个专门用于优先于普通请求的请求的 "hiprio" virtqueue。)
