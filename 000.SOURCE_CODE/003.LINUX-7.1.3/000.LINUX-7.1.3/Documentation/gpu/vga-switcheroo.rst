.. _vga_switcheroo:

==========================
VGA Switcheroo(VGA 切换器)
==========================

.. kernel-doc:: drivers/gpu/vga/vga_switcheroo.c
   :doc: Overview

Modes of Use(使用模式)
======================

Manual switching and manual power control(手动切换与手动电源控制)
-----------------------------------------------------------------

.. kernel-doc:: drivers/gpu/vga/vga_switcheroo.c
   :doc: Manual switching and manual power control

Driver power control(驱动电源控制)
----------------------------------

.. kernel-doc:: drivers/gpu/vga/vga_switcheroo.c
   :doc: Driver power control

API(API)
========

Public functions(公开函数)
--------------------------

.. kernel-doc:: drivers/gpu/vga/vga_switcheroo.c
   :export:

Public structures(公开结构体)
-----------------------------

.. kernel-doc:: include/linux/vga_switcheroo.h
   :functions: vga_switcheroo_handler

.. kernel-doc:: include/linux/vga_switcheroo.h
   :functions: vga_switcheroo_client_ops

Public constants(公开常量)
--------------------------

.. kernel-doc:: include/linux/vga_switcheroo.h
   :functions: vga_switcheroo_handler_flags_t

.. kernel-doc:: include/linux/vga_switcheroo.h
   :functions: vga_switcheroo_client_id

.. kernel-doc:: include/linux/vga_switcheroo.h
   :functions: vga_switcheroo_state

Private structures(私有结构体)
------------------------------

.. kernel-doc:: drivers/gpu/vga/vga_switcheroo.c
   :functions: vgasr_priv

.. kernel-doc:: drivers/gpu/vga/vga_switcheroo.c
   :functions: vga_switcheroo_client

Handlers(处理程序)
==================

apple-gmux Handler(apple-gmux 处理程序)
---------------------------------------

.. kernel-doc:: drivers/platform/x86/apple-gmux.c
   :doc: Overview

.. kernel-doc:: drivers/platform/x86/apple-gmux.c
   :doc: Interrupt

Graphics mux(图形 mux)
~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: drivers/platform/x86/apple-gmux.c
   :doc: Graphics mux

Power control(电源控制)
~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: drivers/platform/x86/apple-gmux.c
   :doc: Power control

Backlight control(背光控制)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: drivers/platform/x86/apple-gmux.c
   :doc: Backlight control

Public functions(公开函数)
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. kernel-doc:: include/linux/apple-gmux.h
   :internal:
