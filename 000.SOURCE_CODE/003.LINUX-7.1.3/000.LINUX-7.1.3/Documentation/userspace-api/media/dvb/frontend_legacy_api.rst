.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later

.. _frontend_legacy_types:

Frontend Legacy Data Types(前端旧式数据类型)
============================================


.. toctree::
    :maxdepth: 1

    fe-type-t
    fe-bandwidth-t
    dvb-frontend-parameters
    dvb-frontend-event


.. _frontend_legacy_fcalls:

Frontend Legacy Function Calls(前端旧式函数调用)
================================================

Those functions are defined at DVB version 3. The support is kept in the
kernel due to compatibility issues only. Their usage is strongly not
recommended
(这些函数定义于 DVB 版本 3。出于兼容性考虑，内核中仅保留对它们的支持。
强烈不建议使用。)


.. toctree::
    :maxdepth: 1

    fe-read-ber
    fe-read-snr
    fe-read-signal-strength
    fe-read-uncorrected-blocks
    fe-set-frontend
    fe-get-frontend
    fe-get-event
    fe-dishnetwork-send-legacy-cmd
