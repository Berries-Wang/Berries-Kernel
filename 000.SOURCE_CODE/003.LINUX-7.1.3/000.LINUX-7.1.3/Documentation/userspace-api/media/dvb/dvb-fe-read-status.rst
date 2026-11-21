.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _dvb-fe-read-status:

***************************************************************
Querying frontend status and statistics(查询前端状态与统计数据)
***************************************************************

Once :ref:`FE_SET_PROPERTY <FE_GET_PROPERTY>` is called, the
frontend will run a kernel thread that will periodically check for the
tuner lock status and provide statistics about the quality of the
signal.
(调用 :ref:`FE_SET_PROPERTY <FE_GET_PROPERTY>` 后，前端将运行一个内核线程，周期性地检查调谐器锁定状态并提供关于信号质量的统计数据。)

The information about the frontend tuner locking status can be queried
using :ref:`FE_READ_STATUS`.
(关于前端调谐器锁定状态的信息可以使用 :ref:`FE_READ_STATUS` 查询。)

Signal statistics are provided via
:ref:`FE_GET_PROPERTY`.
(信号统计数据通过 :ref:`FE_GET_PROPERTY` 提供。)

.. note::

   Most statistics require the demodulator to be fully locked
   (e. g. with :c:type:`FE_HAS_LOCK <fe_status>` bit set). See
   :ref:`Frontend statistics indicators <frontend-stat-properties>` for
   more details.
   (大多数统计信息要求解调器完全锁定 (例如设置了 :c:type:`FE_HAS_LOCK <fe_status>` 位)。更多细节请参阅 :ref:`Frontend statistics indicators <frontend-stat-properties>` 。)
