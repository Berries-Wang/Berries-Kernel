.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _net:

########################################
Digital TV Network API(数字电视网络 API)
########################################

The Digital TV net device controls the mapping of data packages that are part
of a transport stream to be mapped into a virtual network interface,
visible through the standard Linux network protocol stack.
(数字电视 net 设备控制把传输流中的数据包映射进一个虚拟网络接口，该接口可通过
标准 Linux 网络协议栈看到。)

Currently, two encapsulations are supported:
(目前支持两种封装：)

-  `Multi Protocol Encapsulation (MPE) <http://en.wikipedia.org/wiki/Multiprotocol_Encapsulation>`__
   (多协议封装（MPE）。)

-  `Ultra Lightweight Encapsulation (ULE) <http://en.wikipedia.org/wiki/Unidirectional_Lightweight_Encapsulation>`__
   (超轻量封装（ULE）。)

In order to create the Linux virtual network interfaces, an application
needs to tell to the Kernel what are the PIDs and the encapsulation
types that are present on the transport stream. This is done through
``/dev/dvb/adapter?/net?`` device node. The data will be available via
virtual ``dvb?_?`` network interfaces, and will be controlled/routed via
the standard ip tools (like ip, route, netstat, ifconfig, etc).
(要创建 Linux 虚拟网络接口，应用程序需要告诉内核传输流中存在哪些 PID 以及
封装类型。这通过 ``/dev/dvb/adapter?/net?`` 设备节点完成。数据将经由虚拟
``dvb?_?`` 网络接口提供，并可通过标准 ip 工具（如 ip、route、netstat、
ifconfig 等）进行控制/路由。)

Data types and ioctl definitions are defined via ``linux/dvb/net.h``
header.
(数据类型与 ioctl 定义由 ``linux/dvb/net.h`` 头文件给出。)


.. _net_fcalls:

Digital TV net Function Calls(数字电视 net 函数调用)
####################################################

.. toctree::
    :maxdepth: 1

    net-types
    net-add-if
    net-remove-if
    net-get-if
