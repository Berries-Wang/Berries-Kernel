=========================================
Linux Networking and Network Devices APIs
=========================================

(Linux 网络与网络设备 API)

Linux Networking
(Linux 网络)
================

Networking Base Types
(网络基础类型)
---------------------

.. kernel-doc:: include/linux/net.h
   :internal:

Socket Buffer Functions
(套接字缓冲区函数)
-----------------------

.. kernel-doc:: include/linux/skbuff.h
   :internal:

.. kernel-doc:: include/net/sock.h
   :internal:

.. kernel-doc:: net/socket.c
   :export:

.. kernel-doc:: net/core/skbuff.c
   :export:

.. kernel-doc:: net/core/sock.c
   :export:

.. kernel-doc:: net/core/datagram.c
   :export:

.. kernel-doc:: net/core/stream.c
   :export:

Socket Filter
(套接字过滤器)
-------------

.. kernel-doc:: net/core/filter.c
   :export:

Generic Network Statistics
(通用网络统计)
--------------------------

.. kernel-doc:: include/uapi/linux/gen_stats.h
   :internal:

.. kernel-doc:: net/core/gen_stats.c
   :export:

.. kernel-doc:: net/core/gen_estimator.c
   :export:

SUN RPC subsystem
(SUN RPC 子系统)
-----------------

.. kernel-doc:: net/sunrpc/xdr.c
   :export:

.. kernel-doc:: net/sunrpc/svc_xprt.c
   :export:

.. kernel-doc:: net/sunrpc/xprt.c
   :export:

.. kernel-doc:: net/sunrpc/sched.c
   :export:

.. kernel-doc:: net/sunrpc/socklib.c
   :export:

.. kernel-doc:: net/sunrpc/stats.c
   :export:

.. kernel-doc:: net/sunrpc/rpc_pipe.c
   :export:

.. kernel-doc:: net/sunrpc/rpcb_clnt.c
   :export:

.. kernel-doc:: net/sunrpc/clnt.c
   :export:

Network device support
(网络设备支持)
======================

Driver Support
(驱动支持)
--------------

.. kernel-doc:: net/core/dev.c
   :export:

.. kernel-doc:: net/ethernet/eth.c
   :export:

.. kernel-doc:: net/sched/sch_generic.c
   :export:

.. kernel-doc:: include/linux/etherdevice.h
   :internal:

.. kernel-doc:: include/linux/netdevice.h
   :internal:

.. kernel-doc:: include/net/net_shaper.h
   :internal:

PHY Support
(PHY 支持)
-----------

.. kernel-doc:: drivers/net/phy/phy.c
   :export:

.. kernel-doc:: drivers/net/phy/phy.c
   :internal:

.. kernel-doc:: drivers/net/phy/phy-core.c
   :export:

.. kernel-doc:: drivers/net/phy/phy-c45.c
   :export:

.. kernel-doc:: include/linux/phy.h
   :internal:

.. kernel-doc:: drivers/net/phy/phy_device.c
   :export:

.. kernel-doc:: drivers/net/phy/phy_device.c
   :internal:

.. kernel-doc:: drivers/net/phy/mdio_bus.c
   :export:

.. kernel-doc:: drivers/net/phy/mdio_bus.c
   :internal:

PHYLINK
(PHYLINK)
-------

  PHYLINK interfaces traditional network drivers with PHYLIB, fixed-links,
  and SFF modules (eg, hot-pluggable SFP) that may contain PHYs.  PHYLINK
  provides management of the link state and link modes.
  (PHYLINK 将传统网络驱动程序与 PHYLIB、固定链路(fixed-links)以及可能包含 PHY 的 SFF
  模块(例如可热插拔的 SFP)连接起来。PHYLINK 提供对链路状态和链路模式的管理。)

.. kernel-doc:: include/linux/phylink.h
   :internal:

.. kernel-doc:: drivers/net/phy/phylink.c

SFP support
(SFP 支持)
-----------

.. kernel-doc:: drivers/net/phy/sfp-bus.c
   :internal:

.. kernel-doc:: include/linux/sfp.h
   :internal:

.. kernel-doc:: drivers/net/phy/sfp-bus.c
   :export:
