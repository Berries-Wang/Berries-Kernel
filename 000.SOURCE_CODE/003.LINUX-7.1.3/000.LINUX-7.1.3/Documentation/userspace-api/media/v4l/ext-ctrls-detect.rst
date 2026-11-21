.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _detect-controls:

**************************************
Detect Control Reference(检测控件参考)
**************************************

The Detect class includes controls for common features of various motion
or object detection capable devices.
(检测 (Detect) 类包含针对各类具备运动或目标检测能力设备的通用功能的控件。)


.. _detect-control-id:

Detect Control IDs(检测控件 ID)
===============================

``V4L2_CID_DETECT_CLASS (class)``
    The Detect class descriptor. Calling
    :ref:`VIDIOC_QUERYCTRL` for this control will
    return a description of this control class.
    (检测类的类描述符。对此控件调用 :ref:`VIDIOC_QUERYCTRL` 会返回该控件
    类的描述。)

``V4L2_CID_DETECT_MD_MODE (menu)``
    Sets the motion detection mode.
    (设置运动检测模式。)

.. tabularcolumns:: |p{7.7cm}|p{9.8cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0

    * - ``V4L2_DETECT_MD_MODE_DISABLED``
      - Disable motion detection.(禁用运动检测。)
    * - ``V4L2_DETECT_MD_MODE_GLOBAL``
      - Use a single motion detection threshold.(使用单一的运动检测阈值。)
    * - ``V4L2_DETECT_MD_MODE_THRESHOLD_GRID``
      - The image is divided into a grid, each cell with its own motion
	detection threshold. These thresholds are set through the
	``V4L2_CID_DETECT_MD_THRESHOLD_GRID`` matrix control.(图像被划分为
	网格，每个单元格有自己的运动检测阈值。这些阈值通过
	``V4L2_CID_DETECT_MD_THRESHOLD_GRID`` 矩阵控件设置。)
    * - ``V4L2_DETECT_MD_MODE_REGION_GRID``
      - The image is divided into a grid, each cell with its own region
	value that specifies which per-region motion detection thresholds
	should be used. Each region has its own thresholds. How these
	per-region thresholds are set up is driver-specific. The region
	values for the grid are set through the
	``V4L2_CID_DETECT_MD_REGION_GRID`` matrix control.(图像被划分为网格，
	每个单元格有自己的区域值，用于指定使用哪些按区域的运动检测阈值。
	每个区域都有自己的阈值。这些按区域阈值的设置方式由驱动决定。网格的
	区域值通过 ``V4L2_CID_DETECT_MD_REGION_GRID`` 矩阵控件设置。)



``V4L2_CID_DETECT_MD_GLOBAL_THRESHOLD (integer)``
    Sets the global motion detection threshold to be used with the
    ``V4L2_DETECT_MD_MODE_GLOBAL`` motion detection mode.
    (设置与 ``V4L2_DETECT_MD_MODE_GLOBAL`` 运动检测模式配合使用的全局运动
    检测阈值。)

``V4L2_CID_DETECT_MD_THRESHOLD_GRID (__u16 matrix)``
    Sets the motion detection thresholds for each cell in the grid. To
    be used with the ``V4L2_DETECT_MD_MODE_THRESHOLD_GRID`` motion
    detection mode. Matrix element (0, 0) represents the cell at the
    top-left of the grid.
    (为网格中的每个单元格设置运动检测阈值。与
    ``V4L2_DETECT_MD_MODE_THRESHOLD_GRID`` 运动检测模式配合使用。矩阵元素
    (0, 0) 表示网格左上角的单元格。)

``V4L2_CID_DETECT_MD_REGION_GRID (__u8 matrix)``
    Sets the motion detection region value for each cell in the grid. To
    be used with the ``V4L2_DETECT_MD_MODE_REGION_GRID`` motion
    detection mode. Matrix element (0, 0) represents the cell at the
    top-left of the grid.
    (为网格中的每个单元格设置运动检测区域值。与
    ``V4L2_DETECT_MD_MODE_REGION_GRID`` 运动检测模式配合使用。矩阵元素
    (0, 0) 表示网格左上角的单元格。)
