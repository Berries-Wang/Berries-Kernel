.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _dvb_ca:

######################################
Digital TV CA Device(数字电视 CA 设备)
######################################

The Digital TV CA device controls the conditional access hardware. It
can be accessed through ``/dev/dvb/adapter?/ca?``. Data types and ioctl
definitions can be accessed by including ``linux/dvb/ca.h`` in your
application.
(数字电视 CA 设备控制条件接收 (conditional access) 硬件。可以通过 ``/dev/dvb/adapter?/ca?`` 访问该设备。在应用程序中包含 ``linux/dvb/ca.h`` 即可获得数据类型和 ioctl 定义。)

.. note::

   There are three ioctls at this API that aren't documented:
   :ref:`CA_GET_MSG`, :ref:`CA_SEND_MSG` and :ref:`CA_SET_DESCR`.
   Documentation for them are welcome.
   (此 API 中有三个未在文档中说明的 ioctl：:ref:`CA_GET_MSG` 、:ref:`CA_SEND_MSG` 和 :ref:`CA_SET_DESCR` 。欢迎为它们补充文档。)

.. toctree::
    :maxdepth: 1

    ca_data_types
    ca_function_calls
    ca_high_level
