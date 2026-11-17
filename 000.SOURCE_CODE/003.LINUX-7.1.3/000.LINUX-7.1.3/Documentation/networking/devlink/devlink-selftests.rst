.. SPDX-License-Identifier: (GPL-2.0-only OR BSD-2-Clause)

=================
Devlink Selftests
=================

(devlink 自测)

The ``devlink-selftests`` API allows executing selftests on the device.
(``devlink-selftests`` API 允许在设备上执行自测 (selftests)。)

Tests Mask
==========
(测试掩码)

The ``devlink-selftests`` command should be run with a mask indicating
the tests to be executed.
(``devlink-selftests`` 命令应带有一个掩码运行，以指示要执行的测试。)

Tests Description
=================
(测试描述)

The following is a list of tests that drivers may execute.
(以下是驱动可以执行的测试的列表。)

.. list-table:: List of tests
   :widths: 5 90

   * - Name (名称)
     - Description (描述)
   * - ``DEVLINK_SELFTEST_FLASH``
     - Devices may have the firmware on non-volatile memory on the board, e.g.
       flash. This particular test helps to run a flash selftest on the device.
       Implementation of the test is left to the driver/firmware.
       (设备可能在板载非易失性存储器(例如 flash)上保存固件。该测试用于在设备上运行 flash 自测。测试的实现由驱动/固件自行决定。)

example usage
-------------
(用法示例)

.. code:: shell

    # Query selftests supported on the devlink device
    $ devlink dev selftests show DEV
    # Query selftests supported on all devlink devices
    $ devlink dev selftests show
    # Executes selftests on the device
    $ devlink dev selftests run DEV id flash
