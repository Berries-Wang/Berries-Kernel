=======================================================================================
 drm/xen-front Xen para-virtualized frontend driver(drm/xen-front Xen 半虚拟化前端驱动)
=======================================================================================

This frontend driver implements Xen para-virtualized display
according to the display protocol described at
include/xen/interface/io/displif.h
(此前端驱动依照 include/xen/interface/io/displif.h 中描述的显示协议实现 Xen
半虚拟化显示。)

Driver modes of operation in terms of display buffers used(就所用显示缓冲区而言的驱动操作模式)
==============================================================================================

.. kernel-doc:: drivers/gpu/drm/xen/xen_drm_front.h
   :doc: Driver modes of operation in terms of display buffers used

Buffers allocated by the frontend driver(由前端驱动分配的缓冲区)
----------------------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/xen/xen_drm_front.h
   :doc: Buffers allocated by the frontend driver

Buffers allocated by the backend(由后端分配的缓冲区)
----------------------------------------------------

.. kernel-doc:: drivers/gpu/drm/xen/xen_drm_front.h
   :doc: Buffers allocated by the backend

Driver limitations(驱动限制)
============================

.. kernel-doc:: drivers/gpu/drm/xen/xen_drm_front.h
   :doc: Driver limitations
