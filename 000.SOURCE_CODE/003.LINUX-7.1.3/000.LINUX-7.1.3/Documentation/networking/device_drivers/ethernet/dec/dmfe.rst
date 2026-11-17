.. SPDX-License-Identifier: GPL-2.0

=========================================================================================================================
Davicom DM9102(A)/DM9132/DM9801 fast ethernet driver for Linux(Linux 下的 Davicom DM9102(A)/DM9132/DM9801 快速以太网驱动)
=========================================================================================================================

Note: This driver doesn't have a maintainer.
(注意：该驱动目前没有维护者。)


This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General   Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
(本程序是自由软件；您可以遵照自由软件基金会发布的 GNU 通用公共许可证对其进行再分发和/或修改；许可证可以是第 2 版，或(由您选择)任何更新的版本。)

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
(分发本程序是希望它有用，但不提供任何保证；甚至不提供适销性或特定用途适用性的默示保证。更多细节请参阅 GNU 通用公共许可证。)


This driver provides kernel support for Davicom DM9102(A)/DM9132/DM9801 ethernet cards ( CNET
10/100 ethernet cards uses Davicom chipset too, so this driver supports CNET cards too ).If you
didn't compile this driver as a module, it will automatically load itself on boot and print a
line similar to::
(该驱动为 Davicom DM9102(A)/DM9132/DM9801 以太网卡提供内核支持(CNET 10/100 以太网卡也使用 Davicom 芯片组，因此该驱动同样支持 CNET 网卡)。如果你没有把该驱动编译为模块，它会在启动时自动加载，并打印一行类似下面的内容：)

	dmfe: Davicom DM9xxx net driver, version 1.36.4 (2002-01-17)

If you compiled this driver as a module, you have to load it on boot.You can load it with command::
(如果你把该驱动编译成了模块，则需要在启动时加载它。可以用以下命令加载：)

	insmod dmfe

This way it will autodetect the device mode.This is the suggested way to load the module.Or you can pass
a mode= setting to module while loading, like::
(这样它将自动检测设备模式。这是建议的模块加载方式。你也可以在加载时向模块传递 mode= 设置，例如：)

	insmod dmfe mode=0 # Force 10M Half Duplex
	insmod dmfe mode=1 # Force 100M Half Duplex
	insmod dmfe mode=4 # Force 10M Full Duplex
	insmod dmfe mode=5 # Force 100M Full Duplex

Next you should configure your network interface with a command similar to::

	ifconfig eth0 172.22.3.18
		      ^^^^^^^^^^^
		     Your IP Address

(接下来，你应该用类似下面的命令配置你的网络接口：)

Then you may have to modify the default routing table with command::
(然后，你可能需要用以下命令修改默认路由表：)

	route add default eth0


Now your ethernet card should be up and running.
(现在你的以太网卡应该已经启动并正常工作了。)


TODO:
(待办事项：)

- Implement pci_driver::suspend() and pci_driver::resume() power management methods.
  (实现 pci_driver::suspend() 和 pci_driver::resume() 电源管理方法。)
- Check on 64 bit boxes.
  (在 64 位机器上进行检查。)
- Check and fix on big endian boxes.
  (在大端机器上进行检查并修复。)
- Test and make sure PCI latency is now correct for all cases.
  (进行测试，确保所有情况下 PCI 延迟设置现在都是正确的。)


Authors:
(作者：)

Sten Wang <sten_wang@davicom.com.tw >   : Original Author
(Sten Wang <sten_wang@davicom.com.tw >：原作者)

Contributors:
(贡献者：)

- Marcelo Tosatti <marcelo@conectiva.com.br>
- Alan Cox <alan@lxorguk.ukuu.org.uk>
- Jeff Garzik <jgarzik@pobox.com>
- Vojtech Pavlik <vojtech@suse.cz>
