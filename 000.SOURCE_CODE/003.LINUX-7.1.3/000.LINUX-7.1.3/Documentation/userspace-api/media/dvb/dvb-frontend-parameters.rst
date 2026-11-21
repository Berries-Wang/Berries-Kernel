.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. c:type:: dvb_frontend_parameters

*****************************
frontend parameters(前端参数)
*****************************

The kind of parameters passed to the frontend device for tuning depend
on the kind of hardware you are using.
(传递给前端设备进行调谐的参数类型取决于你使用的硬件类型。)

The struct ``dvb_frontend_parameters`` uses a union with specific
per-system parameters. However, as newer delivery systems required more
data, the structure size weren't enough to fit, and just extending its
size would break the existing applications. So, those parameters were
replaced by the usage of
:ref:`FE_GET_PROPERTY/FE_SET_PROPERTY <FE_GET_PROPERTY>`
ioctl's. The new API is flexible enough to add new parameters to
existing delivery systems, and to add newer delivery systems.
(struct ``dvb_frontend_parameters`` 使用一个 union 来存放各个系统特定的参数。然而，随着更新的传送系统需要更多数据，该结构的大小已不够用，而仅仅扩大其大小会破坏现有应用程序。因此，这些参数被改用 :ref:`FE_GET_PROPERTY/FE_SET_PROPERTY <FE_GET_PROPERTY>` ioctl 来取代。新 API 足够灵活，既可以为现有传送系统添加新参数，也可以添加新的传送系统。)

So, newer applications should use
:ref:`FE_GET_PROPERTY/FE_SET_PROPERTY <FE_GET_PROPERTY>`
instead, in order to be able to support the newer System Delivery like
DVB-S2, DVB-T2, DVB-C2, ISDB, etc.
(因此，较新的应用程序应改用 :ref:`FE_GET_PROPERTY/FE_SET_PROPERTY <FE_GET_PROPERTY>` ，以便能够支持 DVB-S2、DVB-T2、DVB-C2、ISDB 等较新的系统传送方式。)

All kinds of parameters are combined as a union in the
``dvb_frontend_parameters`` structure:
(所有类型的参数作为一个 union 组合在 ``dvb_frontend_parameters`` 结构中：)


.. code-block:: c

    struct dvb_frontend_parameters {
	uint32_t frequency;     /* (absolute) frequency in Hz for QAM/OFDM */
		    /* intermediate frequency in kHz for QPSK */
	fe_spectral_inversion_t inversion;
	union {
	    struct dvb_qpsk_parameters qpsk;
	    struct dvb_qam_parameters  qam;
	    struct dvb_ofdm_parameters ofdm;
	    struct dvb_vsb_parameters  vsb;
	} u;
    };

In the case of QPSK frontends the ``frequency`` field specifies the
intermediate frequency, i.e. the offset which is effectively added to
the local oscillator frequency (LOF) of the LNB. The intermediate
frequency has to be specified in units of kHz. For QAM and OFDM
frontends the ``frequency`` specifies the absolute frequency and is
given in Hz.


.. c:type:: dvb_qpsk_parameters

QPSK parameters(QPSK 参数)
==========================

For satellite QPSK frontends you have to use the ``dvb_qpsk_parameters``
structure:
(对于卫星 QPSK 前端，你必须使用 ``dvb_qpsk_parameters`` 结构：)


.. code-block:: c

     struct dvb_qpsk_parameters {
	 uint32_t        symbol_rate;  /* symbol rate in Symbols per second */
	 fe_code_rate_t  fec_inner;    /* forward error correction (see above) */
     };


.. c:type:: dvb_qam_parameters

QAM parameters(QAM 参数)
========================

for cable QAM frontend you use the ``dvb_qam_parameters`` structure:
(对于有线 QAM 前端，你使用 ``dvb_qam_parameters`` 结构：)


.. code-block:: c

     struct dvb_qam_parameters {
	 uint32_t         symbol_rate; /* symbol rate in Symbols per second */
	 fe_code_rate_t   fec_inner;   /* forward error correction (see above) */
	 fe_modulation_t  modulation;  /* modulation type (see above) */
     };


.. c:type:: dvb_vsb_parameters

VSB parameters(VSB 参数)
========================

ATSC frontends are supported by the ``dvb_vsb_parameters`` structure:
(ATSC 前端由 ``dvb_vsb_parameters`` 结构支持：)


.. code-block:: c

    struct dvb_vsb_parameters {
	fe_modulation_t modulation; /* modulation type (see above) */
    };


.. c:type:: dvb_ofdm_parameters

OFDM parameters(OFDM 参数)
==========================

DVB-T frontends are supported by the ``dvb_ofdm_parameters`` structure:
(DVB-T 前端由 ``dvb_ofdm_parameters`` 结构支持：)


.. code-block:: c

     struct dvb_ofdm_parameters {
	 fe_bandwidth_t      bandwidth;
	 fe_code_rate_t      code_rate_HP;  /* high priority stream code rate */
	 fe_code_rate_t      code_rate_LP;  /* low priority stream code rate */
	 fe_modulation_t     constellation; /* modulation type (see above) */
	 fe_transmit_mode_t  transmission_mode;
	 fe_guard_interval_t guard_interval;
	 fe_hierarchy_t      hierarchy_information;
     };
