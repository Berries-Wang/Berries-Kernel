.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _rf-tuner-controls:

*********************************************
RF Tuner Control Reference(RF 调谐器控制参考)
*********************************************

The RF Tuner (RF_TUNER) class includes controls for common features of
devices having RF tuner.
(RF Tuner (RF_TUNER) 类包含具有 RF 调谐器的设备的常见功能控制项。)

In this context, RF tuner is radio receiver circuit between antenna and
demodulator. It receives radio frequency (RF) from the antenna and
converts that received signal to lower intermediate frequency (IF) or
baseband frequency (BB). Tuners that could do baseband output are often
called Zero-IF tuners. Older tuners were typically simple PLL tuners
inside a metal box, while newer ones are highly integrated chips
without a metal box "silicon tuners". These controls are mostly
applicable for new feature rich silicon tuners, just because older
tuners does not have much adjustable features.
(在这里，RF 调谐器是天线与解调器之间的无线电接收电路。它从天线接收
射频(RF)，并把接收到的信号转换为较低的中频(IF)或基带频率(BB)。
能够输出基带的调谐器通常被称为 Zero-IF 调谐器。较旧的调谐器通常是
金属盒内的简单 PLL 调谐器，而较新的则是没有金属盒的高度集成芯片
("硅调谐器")。这些控制主要适用于功能丰富的新型硅调谐器，因为较旧
的调谐器没有多少可调功能。)

For more information about RF tuners see
`Tuner (radio) <http://en.wikipedia.org/wiki/Tuner_%28radio%29>`__
and `RF front end <http://en.wikipedia.org/wiki/RF_front_end>`__
from Wikipedia.
(关于 RF 调谐器的更多信息，参见 Wikipedia 上的 Tuner (radio) 与
RF front end 条目。)


.. _rf-tuner-control-id:

RF_TUNER Control IDs(RF_TUNER 控制 ID)
======================================

``V4L2_CID_RF_TUNER_CLASS (class)``
    The RF_TUNER class descriptor. Calling
    :ref:`VIDIOC_QUERYCTRL` for this control will
    return a description of this control class.
    (RF_TUNER 类描述符。对该控制调用 :ref:`VIDIOC_QUERYCTRL`
    将返回此控制类的描述。)

``V4L2_CID_RF_TUNER_BANDWIDTH_AUTO (boolean)``
    Enables/disables tuner radio channel bandwidth configuration. In
    automatic mode bandwidth configuration is performed by the driver.
    (启用/禁用调谐器无线电信道带宽配置。在自动模式下，带宽配置由
    驱动程序执行。)

``V4L2_CID_RF_TUNER_BANDWIDTH (integer)``
    Filter(s) on tuner signal path are used to filter signal according
    to receiving party needs. Driver configures filters to fulfill
    desired bandwidth requirement. Used when
    V4L2_CID_RF_TUNER_BANDWIDTH_AUTO is not set. Unit is in Hz. The
    range and step are driver-specific.
    (调谐器信号路径上的滤波器用于按接收方的需求过滤信号。驱动程序
    配置滤波器以满足期望的带宽要求。在未设置
    V4L2_CID_RF_TUNER_BANDWIDTH_AUTO 时使用。单位为 Hz。范围和步长
    由驱动程序决定。)

``V4L2_CID_RF_TUNER_LNA_GAIN_AUTO (boolean)``
    Enables/disables LNA automatic gain control (AGC)
    (启用/禁用 LNA 自动增益控制(AGC))

``V4L2_CID_RF_TUNER_MIXER_GAIN_AUTO (boolean)``
    Enables/disables mixer automatic gain control (AGC)
    (启用/禁用混频器自动增益控制(AGC))

``V4L2_CID_RF_TUNER_IF_GAIN_AUTO (boolean)``
    Enables/disables IF automatic gain control (AGC)
    (启用/禁用 IF 自动增益控制(AGC))

``V4L2_CID_RF_TUNER_RF_GAIN (integer)``
    The RF amplifier is the very first amplifier on the receiver signal
    path, just right after the antenna input. The difference between the
    LNA gain and the RF gain in this document is that the LNA gain is
    integrated in the tuner chip while the RF gain is a separate chip.
    There may be both RF and LNA gain controls in the same device. The
    range and step are driver-specific.
    (RF 放大器是接收信号路径上的第一个放大器，就在天线输入之后。
    在本文中，LNA 增益与 RF 增益的区别在于：LNA 增益集成在调谐器
    芯片中，而 RF 增益是单独的芯片。同一设备中可能同时有 RF 和 LNA
    增益控制。范围和步长由驱动程序决定。)

``V4L2_CID_RF_TUNER_LNA_GAIN (integer)``
    LNA (low noise amplifier) gain is first gain stage on the RF tuner
    signal path. It is located very close to tuner antenna input. Used
    when ``V4L2_CID_RF_TUNER_LNA_GAIN_AUTO`` is not set. See
    ``V4L2_CID_RF_TUNER_RF_GAIN`` to understand how RF gain and LNA gain
    differs from the each others. The range and step are
    driver-specific.
    (LNA(低噪声放大器)增益是 RF 调谐器信号路径上的第一级增益。
    它位于非常靠近调谐器天线输入的位置。在未设置
    ``V4L2_CID_RF_TUNER_LNA_GAIN_AUTO`` 时使用。参见
    ``V4L2_CID_RF_TUNER_RF_GAIN`` 以了解 RF 增益与 LNA 增益之间的
    区别。范围和步长由驱动程序决定。)

``V4L2_CID_RF_TUNER_MIXER_GAIN (integer)``
    Mixer gain is second gain stage on the RF tuner signal path. It is
    located inside mixer block, where RF signal is down-converted by the
    mixer. Used when ``V4L2_CID_RF_TUNER_MIXER_GAIN_AUTO`` is not set.
    The range and step are driver-specific.
    (混频器增益是 RF 调谐器信号路径上的第二级增益。它位于混频器模块
    内部，RF 信号在这里被混频器下变频。在未设置
    ``V4L2_CID_RF_TUNER_MIXER_GAIN_AUTO`` 时使用。范围和步长由驱动
    程序决定。)

``V4L2_CID_RF_TUNER_IF_GAIN (integer)``
    IF gain is last gain stage on the RF tuner signal path. It is
    located on output of RF tuner. It controls signal level of
    intermediate frequency output or baseband output. Used when
    ``V4L2_CID_RF_TUNER_IF_GAIN_AUTO`` is not set. The range and step
    are driver-specific.
    (IF 增益是 RF 调谐器信号路径上的最后一级增益。它位于 RF 调谐器
    的输出处，控制中频输出或基带输出的信号电平。在未设置
    ``V4L2_CID_RF_TUNER_IF_GAIN_AUTO`` 时使用。范围和步长由驱动
    程序决定。)

``V4L2_CID_RF_TUNER_PLL_LOCK (boolean)``
    Is synthesizer PLL locked? RF tuner is receiving given frequency
    when that control is set. This is a read-only control.
    (合成器 PLL 是否已锁定？该控制被置位时，RF 调谐器正在接收给定的
    频率。这是一个只读控制。)
