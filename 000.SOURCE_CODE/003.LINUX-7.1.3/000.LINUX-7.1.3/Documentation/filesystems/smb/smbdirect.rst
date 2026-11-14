.. SPDX-License-Identifier: GPL-2.0

===========================
SMB Direct - SMB3 over RDMA
===========================

(SMB Direct - 基于 RDMA 的 SMB3)

This document describes how to set up the Linux SMB client and server to
use RDMA.
(本文档描述如何设置 Linux SMB 客户端和服务器以使用 RDMA。)

Overview
(概述)
========
The Linux SMB kernel client supports SMB Direct, which is a transport
scheme for SMB3 that uses RDMA (Remote Direct Memory Access) to provide
high throughput and low latencies by bypassing the traditional TCP/IP
stack.
SMB Direct on the Linux SMB client can be tested against KSMBD - a
kernel-space SMB server.
(Linux SMB 内核客户端支持 SMB Direct，这是一种用于 SMB3 的传输方案，它使用 RDMA(远程直接内存访问)通过绕过传统 TCP/IP 栈来提供高吞吐量和低延迟。Linux SMB 客户端上的 SMB Direct 可以针对 KSMBD——一个内核空间的 SMB 服务器——进行测试。)

Installation
(安装)
=============

- Install an RDMA device. As long as the RDMA device driver is supported
  by the kernel, it should work. This includes both software emulators (soft
  RoCE, soft iWARP) and hardware devices (InfiniBand, RoCE, iWARP).
  (安装一个 RDMA 设备。只要内核支持该 RDMA 设备驱动程序，它就应该可以工作。这包括软件模拟器(soft RoCE、soft iWARP)和硬件设备(InfiniBand、RoCE、iWARP)。)

- Install a kernel with SMB Direct support. The first kernel release to
  support SMB Direct on both the client and server side is 5.15. Therefore,
  a distribution compatible with kernel 5.15 or later is required.
  (安装一个支持 SMB Direct 的内核。第一个在客户端和服务器端都支持 SMB Direct 的内核版本是 5.15。因此，需要一个兼容内核 5.15 或更高版本的发行版。)

- Install cifs-utils, which provides the `mount.cifs` command to mount SMB
  shares.
  (安装 cifs-utils，它提供用于挂载 SMB 共享的 `mount.cifs` 命令。)

- Configure the RDMA stack
  (配置 RDMA 栈)

  Make sure that your kernel configuration has RDMA support enabled. Under
  Device Drivers -> Infiniband support, update the kernel configuration to
  enable Infiniband support.
  (确保你的内核配置已启用 RDMA 支持。在 Device Drivers -> Infiniband support 下，更新内核配置以启用 Infiniband 支持。)

  Enable the appropriate IB HCA support or iWARP adapter support,
  depending on your hardware.
  (根据你的硬件，启用相应的 IB HCA 支持或 iWARP 适配器支持。)

  If you are using InfiniBand, enable IP-over-InfiniBand support.
  (如果你使用 InfiniBand，请启用 IP-over-InfiniBand 支持。)

  For soft RDMA, enable either the soft iWARP (`RDMA _SIW`) or soft RoCE
  (`RDMA_RXE`) module. Install the `iproute2` package and use the
  `rdma link add` command to load the module and create an
  RDMA interface.
  (对于软件 RDMA，启用 soft iWARP(`RDMA _SIW`)或 soft RoCE(`RDMA_RXE`)模块之一。安装 `iproute2` 软件包，并使用 `rdma link add` 命令加载模块并创建 RDMA 接口。)

  e.g. if your local ethernet interface is `eth0`, you can use:
  (例如，如果你的本地以太网接口是 `eth0`，你可以使用：)

    .. code-block:: bash

        sudo rdma link add siw0 type siw netdev eth0

- Enable SMB Direct support for both the server and the client in the kernel
  configuration.
  (在内核配置中为服务器和客户端都启用 SMB Direct 支持。)

    Server Setup
    (服务器设置)

    .. code-block:: text

        Network File Systems  --->
            <M> SMB3 server support
                [*] Support for SMB Direct protocol

    Client Setup
    (客户端设置)

    .. code-block:: text

        Network File Systems  --->
            <M> SMB3 and CIFS support (advanced network filesystem)
                [*] SMB Direct support

- Build and install the kernel. SMB Direct support will be enabled in the
  cifs.ko and ksmbd.ko modules.
  (构建并安装内核。SMB Direct 支持将在 cifs.ko 和 ksmbd.ko 模块中启用。)

Setup and Usage
(设置与使用)
================

- Set up and start a KSMBD server as described in the `KSMBD documentation
  <https://www.kernel.org/doc/Documentation/filesystems/smb/ksmbd.rst>`_.
  Also add the "server multi channel support = yes" parameter to ksmbd.conf.
  (按照 `KSMBD documentation <https://www.kernel.org/doc/Documentation/filesystems/smb/ksmbd.rst>`_ 中的描述设置并启动一个 KSMBD 服务器。同时向 ksmbd.conf 添加 "server multi channel support = yes" 参数。)

- On the client, mount the share with `rdma` mount option to use SMB Direct
  (specify a SMB version 3.0 or higher using `vers`).
  (在客户端上，使用 `rdma` 挂载选项挂载共享以使用 SMB Direct(使用 `vers` 指定 SMB 3.0 或更高版本)。)

  For example:
  (例如：)

    .. code-block:: bash

        mount -t cifs //server/share /mnt/point -o vers=3.1.1,rdma

- To verify that the mount is using SMB Direct, you can check dmesg for the
  following log line after mounting:
  (要验证挂载是否使用了 SMB Direct，你可以在挂载后检查 dmesg 中是否出现以下日志行：)

    .. code-block:: text

        CIFS: VFS: RDMA transport established

  Or, verify `rdma` mount option for the share in `/proc/mounts`:
  (或者，在 `/proc/mounts` 中验证该共享的 `rdma` 挂载选项：)

    .. code-block:: bash

        cat /proc/mounts | grep cifs
