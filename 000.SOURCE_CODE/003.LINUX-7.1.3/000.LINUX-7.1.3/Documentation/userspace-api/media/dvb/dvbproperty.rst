.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _frontend-properties:

************************
Property types(属性类型)
************************

Tuning into a Digital TV physical channel and starting decoding it
requires changing a set of parameters, in order to control the tuner,
the demodulator, the Linear Low-noise Amplifier (LNA) and to set the
antenna subsystem via Satellite Equipment Control - SEC (on satellite
systems). The actual parameters are specific to each particular digital
TV standards, and may change as the digital TV specs evolves.
(调谐到数字电视物理频道并开始解码，需要更改一组参数，以控制调谐器、解调器、低噪声放大器（LNA），并通过卫星设备控制（SEC）设置天线子系统（在卫星系统上）。实际参数因具体的数字电视标准而异，并且可能随着数字电视规范的演进而变化。)

In the past (up to DVB API version 3 - DVBv3), the strategy used was to have a
union with the parameters needed to tune for DVB-S, DVB-C, DVB-T and
ATSC delivery systems grouped there. The problem is that, as the second
generation standards appeared, the size of such union was not big
enough to group the structs that would be required for those new
standards. Also, extending it would break userspace.
(过去（直到 DVB API 版本 3，即 DVBv3），采用的策略是把调谐 DVB-S、DVB-C、DVB-T 和 ATSC 传送系统所需的参数集中在一个 union 中。问题在于，随着第二代标准的出现，这样的 union 的大小不足以容纳这些新标准所需的结构体。而且，扩展它会破坏用户空间。)

So, the legacy union/struct based approach was deprecated, in favor
of a properties set approach. On such approach,
:ref:`FE_GET_PROPERTY and FE_SET_PROPERTY <FE_GET_PROPERTY>` are used
to setup the frontend and read its status.
(于是，基于遗留 union/struct 的方法被弃用，转而采用属性集方法。在这种方法中，使用 :ref:`FE_GET_PROPERTY and FE_SET_PROPERTY <FE_GET_PROPERTY>` 来设置前端并读取其状态。)

The actual action is determined by a set of dtv_property cmd/data pairs.
With one single ioctl, is possible to get/set up to 64 properties.
(实际操作由一组 dtv_property cmd/data 对决定。通过单个 ioctl，最多可以获取/设置 64 个属性。)

This section describes the new and recommended way to set the frontend,
with supports all digital TV delivery systems.
(本节介绍设置前端的新的推荐方式，它支持所有数字电视传送系统。)

.. note::

   1. On Linux DVB API version 3, setting a frontend was done via
      struct :c:type:`dvb_frontend_parameters`.
      (在 Linux DVB API 版本 3 中，设置前端是通过 struct :c:type:`dvb_frontend_parameters` 完成的。)

   2. Don't use DVB API version 3 calls on hardware with supports
      newer standards. Such API provides no support or a very limited
      support to new standards and/or new hardware.
      (不要在支持更新标准的硬件上使用 DVB API 版本 3 的调用。这样的 API 对新标准和/或新硬件不提供支持，或支持非常有限。)

   3. Nowadays, most frontends support multiple delivery systems.
      Only with DVB API version 5 calls it is possible to switch between
      the multiple delivery systems supported by a frontend.
      (如今，大多数前端支持多种传送系统。只有使用 DVB API 版本 5 的调用，才能在前端支持的多种传送系统之间切换。)

   4. DVB API version 5 is also called *S2API*, as the first
      new standard added to it was DVB-S2.
      (DVB API 版本 5 也被称为 *S2API* ，因为加入它的第一个新标准是 DVB-S2。)

**Example**: in order to set the hardware to tune into a DVB-C channel
at 651 kHz, modulated with 256-QAM, FEC 3/4 and symbol rate of 5.217
Mbauds, those properties should be sent to
:ref:`FE_SET_PROPERTY <FE_GET_PROPERTY>` ioctl:
(**示例**：为了将硬件调谐到 651 kHz、以 256-QAM 调制、FEC 3/4、符号率 5.217 Mbauds 的 DVB-C 频道，应将以下属性发送给 :ref:`FE_SET_PROPERTY <FE_GET_PROPERTY>` ioctl：)

  :ref:`DTV_DELIVERY_SYSTEM <DTV-DELIVERY-SYSTEM>` = SYS_DVBC_ANNEX_A

  :ref:`DTV_FREQUENCY <DTV-FREQUENCY>` = 651000000

  :ref:`DTV_MODULATION <DTV-MODULATION>` = QAM_256

  :ref:`DTV_INVERSION <DTV-INVERSION>` = INVERSION_AUTO

  :ref:`DTV_SYMBOL_RATE <DTV-SYMBOL-RATE>` = 5217000

  :ref:`DTV_INNER_FEC <DTV-INNER-FEC>` = FEC_3_4

  :ref:`DTV_TUNE <DTV-TUNE>`

The code that would that would do the above is show in
:ref:`dtv-prop-example`.
(执行上述操作的代码展示在 :ref:`dtv-prop-example` 中。)

.. code-block:: c
    :caption: Example: Setting digital TV frontend properties
    :name: dtv-prop-example

    #include <stdio.h>
    #include <fcntl.h>
    #include <sys/ioctl.h>
    #include <linux/dvb/frontend.h>

    static struct dtv_property props[] = {
	{ .cmd = DTV_DELIVERY_SYSTEM, .u.data = SYS_DVBC_ANNEX_A },
	{ .cmd = DTV_FREQUENCY,       .u.data = 651000000 },
	{ .cmd = DTV_MODULATION,      .u.data = QAM_256 },
	{ .cmd = DTV_INVERSION,       .u.data = INVERSION_AUTO },
	{ .cmd = DTV_SYMBOL_RATE,     .u.data = 5217000 },
	{ .cmd = DTV_INNER_FEC,       .u.data = FEC_3_4 },
	{ .cmd = DTV_TUNE }
    };

    static struct dtv_properties dtv_prop = {
	.num = 6, .props = props
    };

    int main(void)
    {
	int fd = open("/dev/dvb/adapter0/frontend0", O_RDWR);

	if (!fd) {
	    perror ("open");
	    return -1;
	}
	if (ioctl(fd, FE_SET_PROPERTY, &dtv_prop) == -1) {
	    perror("ioctl");
	    return -1;
	}
	printf("Frontend set\\n");
	return 0;
    }

.. attention:: While it is possible to directly call the Kernel code like the
   above example, it is strongly recommended to use
   `libdvbv5 <https://linuxtv.org/docs/libdvbv5/index.html>`__, as it
   provides abstraction to work with the supported digital TV standards and
   provides methods for usual operations like program scanning and to
   read/write channel descriptor files.
   (虽然可以直接像上面的例子那样调用内核代码，但强烈建议使用 `libdvbv5 <https://linuxtv.org/docs/libdvbv5/index.html>`__ ，因为它为受支持的数字电视标准提供了抽象，并为诸如节目扫描、读写频道描述符文件等常见操作提供了方法。)

.. toctree::
    :maxdepth: 1

    fe_property_parameters
    frontend-stat-properties
    frontend-property-terrestrial-systems
    frontend-property-cable-systems
    frontend-property-satellite-systems
    frontend-header
