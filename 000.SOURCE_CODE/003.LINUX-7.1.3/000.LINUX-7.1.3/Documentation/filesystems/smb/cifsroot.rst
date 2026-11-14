.. SPDX-License-Identifier: GPL-2.0

===========================================
Mounting root file system via SMB (cifs.ko)
===========================================

(通过 SMB 挂载根文件系统(cifs.ko))

Written 2019 by Paulo Alcantara <palcantara@suse.de>
(2019 年由 Paulo Alcantara <palcantara@suse.de> 编写)

Written 2019 by Aurelien Aptel <aaptel@suse.com>
(2019 年由 Aurelien Aptel <aaptel@suse.com> 编写)

The CONFIG_CIFS_ROOT option enables experimental root file system
support over the SMB protocol via cifs.ko.
(CONFIG_CIFS_ROOT 选项通过 cifs.ko 启用实验性的基于 SMB 协议的根文件系统支持。)

It introduces a new kernel command-line option called 'cifsroot='
which will tell the kernel to mount the root file system over the
network by utilizing SMB or CIFS protocol.
(它引入了一个新的内核命令行选项 'cifsroot='，该选项告诉内核利用 SMB 或 CIFS 协议通过网络挂载根文件系统。)

In order to mount, the network stack will also need to be set up by
using 'ip=' config option. For more details, see
Documentation/admin-guide/nfs/nfsroot.rst.
(为了进行挂载，还需要使用 'ip=' 配置选项来设置网络栈。更多细节请参见 Documentation/admin-guide/nfs/nfsroot.rst。)

A CIFS root mount currently requires the use of SMB1+UNIX Extensions
which is only supported by the Samba server. SMB1 is the older
deprecated version of the protocol but it has been extended to support
POSIX features (See [1]). The equivalent extensions for the newer
recommended version of the protocol (SMB3) have not been fully
implemented yet which means SMB3 doesn't support some required POSIX
file system objects (e.g. block devices, pipes, sockets).
(CIFS 根挂载目前需要使用 SMB1+UNIX 扩展，只有 Samba 服务器支持它。SMB1 是该协议较旧的已弃用版本，但它已被扩展以支持 POSIX 特性(参见 [1])。较新的推荐协议版本(SMB3)的等效扩展尚未完全实现，这意味着 SMB3 不支持某些必需的 POSIX 文件系统对象(例如块设备、管道、套接字)。)

As a result, a CIFS root will default to SMB1 for now but the version
to use can nonetheless be changed via the 'vers=' mount option.  This
default will change once the SMB3 POSIX extensions are fully
implemented.
(因此，CIFS 根目前默认使用 SMB1，但仍可以通过 'vers=' 挂载选项更改所使用的版本。一旦 SMB3 POSIX 扩展完全实现，此默认值将会改变。)

Server configuration
(服务器配置)
====================

To enable SMB1+UNIX extensions you will need to set these global
settings in Samba smb.conf::
(要启用 SMB1+UNIX 扩展，你需要在 Samba smb.conf 中设置以下全局配置：)

    [global]
    server min protocol = NT1
    unix extension = yes        # default

Kernel command line
(内核命令行)
===================

::

    root=/dev/cifs

This is just a virtual device that basically tells the kernel to mount
the root file system via SMB protocol.
(这只是一个虚拟设备，基本上告诉内核通过 SMB 协议挂载根文件系统。)

::

    cifsroot=//<server-ip>/<share>[,options]

Enables the kernel to mount the root file system via SMB that are
located in the <server-ip> and <share> specified in this option.
(使内核能够通过 SMB 挂载位于此选项中指定的 <server-ip> 和 <share> 内的根文件系统。)

The default mount options are set in fs/smb/client/cifsroot.c.
(默认挂载选项在 fs/smb/client/cifsroot.c 中设置。)

server-ip
	IPv4 address of the server.
	(服务器的 IPv4 地址。)

share
	Path to SMB share (rootfs).
	(SMB 共享(rootfs)的路径。)

options
	Optional mount options. For more information, see mount.cifs(8).
	(可选的挂载选项。更多信息请参见 mount.cifs(8)。)

Examples
(示例)
========

Export root file system as a Samba share in smb.conf file::
(在 smb.conf 文件中将根文件系统导出为 Samba 共享：)

    ...
    [linux]
	    path = /path/to/rootfs
	    read only = no
	    guest ok = yes
	    force user = root
	    force group = root
	    browseable = yes
	    writeable = yes
	    admin users = root
	    public = yes
	    create mask = 0777
	    directory mask = 0777
    ...

Restart smb service::
(重启 smb 服务：)

    # systemctl restart smb

Test it under QEMU on a kernel built with CONFIG_CIFS_ROOT and
CONFIG_IP_PNP options enabled::
(在启用了 CONFIG_CIFS_ROOT 和 CONFIG_IP_PNP 选项构建的内核下，在 QEMU 中进行测试：)

    # qemu-system-x86_64 -enable-kvm -cpu host -m 1024 \
    -kernel /path/to/linux/arch/x86/boot/bzImage -nographic \
    -append "root=/dev/cifs rw ip=dhcp cifsroot=//10.0.2.2/linux,username=foo,password=bar console=ttyS0 3"


1: https://wiki.samba.org/index.php/UNIX_Extensions
