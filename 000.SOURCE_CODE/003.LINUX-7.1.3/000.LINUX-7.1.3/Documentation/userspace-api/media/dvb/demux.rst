.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _dvb_demux:

###########################################
Digital TV Demux Device(数字电视解复用设备)
###########################################

The Digital TV demux device controls the MPEG-TS filters for the
digital TV. If the driver and hardware supports, those filters are
implemented at the hardware. Otherwise, the Kernel provides a software
emulation.
(数字电视 demux 设备控制数字电视的 MPEG-TS 过滤器。如果驱动和硬件支持，这些过滤器在硬件中实现；否则由内核提供软件模拟。)

It can be accessed through ``/dev/adapter?/demux?``. Data types and
ioctl definitions can be accessed by including ``linux/dvb/dmx.h`` in
your application.
(可以通过 ``/dev/adapter?/demux?`` 访问该设备。在应用程序中包含 ``linux/dvb/dmx.h`` 即可获得数据类型和 ioctl 定义。)


.. toctree::
    :maxdepth: 1

    dmx_types
    dmx_fcalls
