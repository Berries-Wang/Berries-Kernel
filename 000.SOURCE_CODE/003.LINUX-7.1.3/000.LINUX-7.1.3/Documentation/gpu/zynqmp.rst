.. SPDX-License-Identifier: GPL-2.0+

=============================================================================================
Xilinx ZynqMP Ultrascale+ DisplayPort Subsystem(Xilinx ZynqMP Ultrascale+ DisplayPort 子系统)
=============================================================================================

This subsystem handles DisplayPort video and audio output on the ZynqMP. It
supports in-memory framebuffers with the DisplayPort DMA controller
(xilinx-dpdma), as well as "live" video and audio from the programmable logic
(PL). This subsystem can perform several transformations, including color space
conversion, alpha blending, and audio mixing, although not all features are
currently supported.
(该子系统在 ZynqMP 上处理 DisplayPort 视频和音频输出。它通过 DisplayPort DMA
控制器 (xilinx-dpdma) 支持内存中的帧缓冲，也支持来自可编程逻辑 (PL) 的
"live"（实时）视频和音频。该子系统可以执行多种变换，包括色彩空间转换、
alpha 混合和音频混合，不过并非所有特性目前都受支持。)

debugfs(调试文件系统)
---------------------

To support debugging and compliance testing, several test modes can be enabled
though debugfs. The following files in /sys/kernel/debug/dri/X/DP-1/test/
control the DisplayPort test modes:
(为支持调试和一致性测试，可以通过 debugfs 启用多种测试模式。
/sys/kernel/debug/dri/X/DP-1/test/ 中的以下文件控制 DisplayPort 测试
模式：)

active:
        Writing a 1 to this file will activate test mode, and writing a 0 will
        deactivate test mode. Writing a 1 or 0 when the test mode is already
        active/inactive will re-activate/re-deactivate test mode. When test
        mode is inactive, changes made to other files will have no (immediate)
        effect, although the settings will be saved for when test mode is
        activated. When test mode is active, changes made to other files will
        apply immediately.
        (向此文件写入 1 将激活测试模式，写入 0 将停用测试模式。当测试模式
        已经处于活动/非活动状态时写入 1 或 0，会重新激活/重新停用测试模式。
        当测试模式非活动时，对其他文件所做的更改不会有（立即的）效果，不过
        这些设置会被保存下来，供测试模式激活时使用。当测试模式活动时，对
        其他文件所做的更改会立即生效。)

custom:
        Custom test pattern value
        (自定义测试图案值)

downspread:
        Enable/disable clock downspreading (spread-spectrum clocking) by
        writing 1/0
        (通过写入 1/0 启用/禁用时钟下扩频（spread-spectrum clocking）)

enhanced:
        Enable/disable enhanced framing
        (启用/禁用增强帧传输 (enhanced framing))

ignore_aux_errors:
        Ignore AUX errors when set to 1. Writes to this file take effect
        immediately (regardless of whether test mode is active) and affect all
        AUX transfers.
        (设置为 1 时忽略 AUX 错误。对此文件的写入立即生效（无论测试模式是否
        活动），并影响所有 AUX 传输。)

ignore_hpd:
        Ignore hotplug events (such as cable removals or monitor link
        retraining requests) when set to 1. Writes to this file take effect
        immediately (regardless of whether test mode is active).
        (设置为 1 时忽略热插拔事件（例如拔掉线缆或显示器链路重训练请求）。
        对此文件的写入立即生效（无论测试模式是否活动）。)

laneX_preemphasis:
        Preemphasis from 0 (lowest) to 2 (highest) for lane X
        (通道 X 的预加重 (preemphasis)，从 0（最低）到 2（最高）)

laneX_swing:
        Voltage swing from 0 (lowest) to 3 (highest) for lane X
        (通道 X 的电压摆幅 (voltage swing)，从 0（最低）到 3（最高）)

lanes:
        Number of lanes to use (1, 2, or 4)
        (要使用的通道数（1、2 或 4）)

pattern:
        Test pattern. May be one of:
        (测试图案。可以是以下之一：)

                video
                        Use regular video input
                        (使用常规视频输入)

                symbol-error
                        Symbol error measurement pattern
                        (符号错误测量图案)

                prbs7
                        Output of the PRBS7 (x^7 + x^6 + 1) polynomial
                        (PRBS7 (x^7 + x^6 + 1) 多项式的输出)

                80bit-custom
                        A custom 80-bit pattern
                        (自定义的 80 位图案)

                cp2520
                        HBR2 compliance eye pattern
                        (HBR2 一致性眼图)

                tps1
                        Link training symbol pattern TPS1 (/D10.2/)
                        (链路训练符号图案 TPS1 (/D10.2/))

                tps2
                        Link training symbol pattern TPS2
                        (链路训练符号图案 TPS2)

                tps3
                        Link training symbol pattern TPS3 (for HBR2)
                        (链路训练符号图案 TPS3（用于 HBR2）)

rate:
        Rate in hertz. One of
        (以赫兹为单位的速率。以下之一：)

                * 5400000000 (HBR2)
                * 2700000000 (HBR)
                * 1620000000 (RBR)

You can dump the displayport test settings with the following command::

        for prop in /sys/kernel/debug/dri/1/DP-1/test/*; do
                printf '%-17s ' ${prop##*/}
                if [ ${prop##*/} = custom ]; then
                        hexdump -C $prop | head -1
                else
                        cat $prop
                fi
        done

(你可以用以下命令转储 DisplayPort 测试设置：)

The output could look something like::

        active            1
        custom            00000000  00 00 00 00 00 00 00 00  00 00                    |..........|
        downspread        0
        enhanced          1
        ignore_aux_errors 1
        ignore_hpd        1
        lane0_preemphasis 0
        lane0_swing       3
        lane1_preemphasis 0
        lane1_swing       3
        lanes             2
        pattern           prbs7
        rate              1620000000

(输出可能类似于：)

The recommended test procedure is to connect the board to a monitor,
configure test mode, activate test mode, and then disconnect the cable
and connect it to your test equipment of choice. For example, one
sequence of commands could be::

        echo 1 > /sys/kernel/debug/dri/1/DP-1/test/enhanced
        echo tps1 > /sys/kernel/debug/dri/1/DP-1/test/pattern
        echo 1620000000 > /sys/kernel/debug/dri/1/DP-1/test/rate
        echo 1 > /sys/kernel/debug/dri/1/DP-1/test/ignore_aux_errors
        echo 1 > /sys/kernel/debug/dri/1/DP-1/test/ignore_hpd
        echo 1 > /sys/kernel/debug/dri/1/DP-1/test/active

(推荐的测试流程是：把板子连接到显示器，配置测试模式，激活测试模式，然后
断开线缆并把它连接到你选择的测试设备。例如，一个命令序列可以是：)

at which point the cable could be disconnected from the monitor.
(此时就可以把线缆从显示器上断开了。)

Internals(内部实现)
-------------------

.. kernel-doc:: drivers/gpu/drm/xlnx/zynqmp_disp.h

.. kernel-doc:: drivers/gpu/drm/xlnx/zynqmp_dpsub.h

.. kernel-doc:: drivers/gpu/drm/xlnx/zynqmp_kms.h

.. kernel-doc:: drivers/gpu/drm/xlnx/zynqmp_disp.c

.. kernel-doc:: drivers/gpu/drm/xlnx/zynqmp_dp.c

.. kernel-doc:: drivers/gpu/drm/xlnx/zynqmp_kms.c
