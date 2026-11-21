.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _legacy_dvb_apis:

************************************************
Digital TV Deprecated APIs(数字电视已弃用的 API)
************************************************

The APIs described here **should not** be used on new drivers or applications.
(此处描述的 API **不应** 在新驱动或应用程序中使用。)

The DVBv3 frontend API has issues with new delivery systems, including
DVB-S2, DVB-T2, ISDB, etc.
(DVBv3 前端 API 在新的传送系统 (包括 DVB-S2、DVB-T2、ISDB 等) 上存在问题。)

.. attention::

   The APIs described here doesn't necessarily reflect the current
   code implementation, as this section of the document was written
   for DVB version 1, while the code reflects DVB version 3
   implementation.
   (此处描述的 API 不一定反映当前的代码实现，因为文档的这一节是为 DVB 版本 1 编写的，而代码反映的是 DVB 版本 3 的实现。)


.. toctree::
    :maxdepth: 1

    frontend_legacy_dvbv3_api
    legacy_dvb_decoder_api
