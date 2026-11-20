===============================================
Display Core Debug tools(Display Core 调试工具)
===============================================

In this section, you will find helpful information on debugging the amdgpu
driver from the display perspective. This page introduces debug mechanisms and
procedures to help you identify if some issues are related to display code.
(在本节中，你会找到从显示角度调试 amdgpu 驱动的有用信息。本页介绍调试机制与流程，帮助你判断某些问题是否与显示代码有关。)

Narrow down display issues(缩小显示问题的范围)
==============================================

Since the display is the driver's visual component, it is common to see users
reporting issues as a display when another component causes the problem. This
section equips users to determine if a specific issue was caused by the display
component or another part of the driver.
(由于显示是驱动的可视化组件，用户把其他组件引起的问题当作显示问题上报的情况很常见。本节帮助用户判断某个具体问题是由显示组件引起，还是由驱动的其他部分引起。)

DC dmesg important messages(DC dmesg 重要消息)
----------------------------------------------

The dmesg log is the first source of information to be checked, and amdgpu
takes advantage of this feature by logging some valuable information. When
looking for the issues associated with amdgpu, remember that each component of
the driver (e.g., smu, PSP, dm, etc.) is loaded one by one, and this
information can be found in the dmesg log. In this sense, look for the part of
the log that looks like the below log snippet::

  [    4.254295] [drm] initializing kernel modesetting (IP DISCOVERY 0x1002:0x744C 0x1002:0x0E3B 0xC8).
  [    4.254718] [drm] register mmio base: 0xFCB00000
  [    4.254918] [drm] register mmio size: 1048576
  [    4.260095] [drm] add ip block number 0 <soc21_common>
  [    4.260318] [drm] add ip block number 1 <gmc_v11_0>
  [    4.260510] [drm] add ip block number 2 <ih_v6_0>
  [    4.260696] [drm] add ip block number 3 <psp>
  [    4.260878] [drm] add ip block number 4 <smu>
  [    4.261057] [drm] add ip block number 5 <dm>
  [    4.261231] [drm] add ip block number 6 <gfx_v11_0>
  [    4.261402] [drm] add ip block number 7 <sdma_v6_0>
  [    4.261568] [drm] add ip block number 8 <vcn_v4_0>
  [    4.261729] [drm] add ip block number 9 <jpeg_v4_0>
  [    4.261887] [drm] add ip block number 10 <mes_v11_0>

(dmesg 日志是首先应当检查的信息来源，amdgpu 利用这一特性记录了一些有价值的信息。查找与 amdgpu 相关的问题时，请记住驱动的每个组件（如 smu、PSP、dm 等）是逐个加载的，这些信息可以在 dmesg 日志中找到。从这个意义上说，请在日志中查找类似下面日志片段的部分：)

From the above example, you can see the line that reports that `<dm>`,
(**Display Manager**), was loaded, which means that display can be part of the
issue. If you do not see that line, something else might have failed before
amdgpu loads the display component, indicating that we don't have a
display issue.
(从上面的示例可以看到报告 `<dm>`（**Display Manager**，显示管理器）已加载的那一行，这意味着显示可能是问题的一部分。如果没有看到这一行，说明在 amdgpu 加载显示组件之前可能有其他东西失败了，表明这并不是显示问题。)

After you identified that the DM was loaded correctly, you can check for the
display version of the hardware in use, which can be retrieved from the dmesg
log with the command::

  dmesg | grep -i 'display core'

(确认 DM 已正确加载之后，可以查看所用硬件的显示版本，使用以下命令即可从 dmesg 日志中获取：)

This command shows a message that looks like this::

  [    4.655828] [drm] Display Core v3.2.285 initialized on DCN 3.2

(该命令会显示类似这样的消息：)

This message has two key pieces of information:
(这条消息包含两条关键信息：)

* **The DC version (e.g., v3.2.285)**: Display developers release a new DC version
  every week, and this information can be advantageous in a situation where a
  user/developer must find a good point versus a bad point based on a tested
  version of the display code. Remember from page :ref:`Display Core <amdgpu-display-core>`,
  that every week the new patches for display are heavily tested with IGT and
  manual tests.
  (DC 版本（如 v3.2.285）：显示开发者每周发布一个新的 DC 版本；当用户/开发者必须基于测试过的显示代码版本区分好点与坏点时，这一信息很有用。请记住 :ref:`Display Core <amdgpu-display-core>` 一页中提到的：每周新的显示补丁都会经过 IGT 与人工测试的严格检验。)
* **The DCN version (e.g., DCN 3.2)**: The DCN block is associated with the
  hardware generation, and the DCN version conveys the hardware generation that
  the driver is currently running. This information helps to narrow down the
  code debug area since each DCN version has its files in the DC folder per DCN
  component (from the example, the developer might want to focus on
  files/folders/functions/structs with the dcn32 label might be executed).
  However, keep in mind that DC reuses code across different DCN versions; for
  example, it is expected to have some callbacks set in one DCN that are the same
  as those from another DCN. In summary, use the DCN version just as a guide.
  (DCN 版本（如 DCN 3.2）：DCN 块与硬件代际相关联，DCN 版本传达驱动当前运行的硬件代际。该信息有助于缩小代码调试范围，因为每个 DCN 版本在 DC 目录中都按 DCN 组件有各自的文件（就本例而言，开发者可能想关注带 dcn32 标签、可能被执行的文件/目录/函数/结构体）。不过请记住，DC 在不同 DCN 版本之间复用代码；例如，某个 DCN 中设置的某些回调与另一个 DCN 的相同是意料之中的。总之，把 DCN 版本仅当作指引即可。)

From the dmesg file, it is also possible to get the ATOM bios code by using::

  dmesg  | grep -i 'ATOM BIOS'

(从 dmesg 文件中还可以获取 ATOM bios 代码，方法是：)

Which generates an output that looks like this::

  [    4.274534] amdgpu: ATOM BIOS: 113-D7020100-102

(它生成的输出类似这样：)

This type of information is useful to be reported.
(这类信息值得在报告问题时附上。)

Avoid loading display core(避免加载 display core)
-------------------------------------------------

Sometimes, it might be hard to figure out which part of the driver is causing
the issue; if you suspect that the display is not part of the problem and your
bug scenario is simple (e.g., some desktop configuration) you can try to remove
the display component from the equation. First, you need to identify `dm` ID
from the dmesg log; for example, search for the following log::

  [    4.254295] [drm] initializing kernel modesetting (IP DISCOVERY 0x1002:0x744C 0x1002:0x0E3B 0xC8).
  [..]
  [    4.260095] [drm] add ip block number 0 <soc21_common>
  [    4.260318] [drm] add ip block number 1 <gmc_v11_0>
  [..]
  [    4.261057] [drm] add ip block number 5 <dm>

(有时可能很难弄清驱动中是哪个部分引起问题；如果你怀疑显示不是问题的一部分，且你的 bug 场景简单（例如某种桌面配置），可以尝试把显示组件从等式中排除。首先，需要从 dmesg 日志中确定 `dm` 的 ID；例如搜索以下日志：)

Notice from the above example that the `dm` id is 5 for this specific hardware.
Next, you need to run the following binary operation to identify the IP block
mask::

  0xffffffff & ~(1 << [DM ID])

(从上面的例子可以看出，对该特定硬件而言 `dm` 的 ID 是 5。接下来，运行以下位运算来确定 IP block 掩码：)

From our example the IP mask is::

 0xffffffff & ~(1 << 5) = 0xffffffdf

(以我们的例子而言，IP 掩码为：)

Finally, to disable DC, you just need to set the below parameter in your
bootloader::

 amdgpu.ip_block_mask = 0xffffffdf

(最后，要禁用 DC，只需在引导加载程序中设置以下参数：)

If you can boot your system with the DC disabled and still see the issue, it
means you can rule DC out of the equation. However, if the bug disappears, you
still need to consider the DC part of the problem and keep narrowing down the
issue. In some scenarios, disabling DC is impossible since it might be
necessary to use the display component to reproduce the issue (e.g., play a
game).
(如果你能在禁用 DC 的情况下启动系统并仍然看到该问题，说明可以把 DC 从等式中排除。但如果 bug 消失了，你仍需考虑 DC 是问题的一部分，并继续缩小问题范围。在某些场景下无法禁用 DC，因为可能必须使用显示组件才能复现问题（例如玩游戏）。)

**Note: This will probably lead to the absence of a display output.**
(**注意：这很可能导致没有显示输出。**)

Display flickering(显示闪烁)
----------------------------

Display flickering might have multiple causes; one is the lack of proper power
to the GPU or problems in the DPM switches. A good first generic verification
is to set the GPU to use high voltage::

   bash -c "echo high > /sys/class/drm/card0/device/power_dpm_force_performance_level"

(显示闪烁可能有多种原因；其一是 GPU 供电不足或 DPM 切换存在问题。一个好的初步通用验证方法是把 GPU 设置为使用高电压：)

The above command sets the GPU/APU to use the maximum power allowed which
disables DPM switches. If forcing DPM levels high does not fix the issue, it
is less likely that the issue is related to power management. If the issue
disappears, there is a good chance that other components might be involved, and
the display should not be ignored since this could be a DPM issues. From the
display side, if the power increase fixes the issue, it is worth debugging the
clock configuration and the pipe split police used in the specific
configuration.
(上面的命令把 GPU/APU 设置为使用允许的最大功率，从而禁用 DPM 切换。如果强制 DPM 级别为高不能解决问题，该问题与电源管理相关的可能性就较小。如果问题消失了，那么其他组件很可能也牵涉其中，不应忽视显示——因为这可能是 DPM 问题。从显示的角度看，如果提升功率解决了问题，就值得调试特定配置下使用的时钟配置与管道拆分策略。)

Display artifacts(显示伪影)
---------------------------

Users may see some screen artifacts that can be categorized into two different
types: localized artifacts and general artifacts. The localized artifacts
happen in some specific areas, such as around the UI window corners; if you see
this type of issue, there is a considerable chance that you have a userspace
problem, likely Mesa or similar. The general artifacts usually happen on the
entire screen. They might be caused by a misconfiguration at the driver level
of the display parameters, but the userspace might also cause this issue. One
way to identify the source of the problem is to take a screenshot or make a
desktop video capture when the problem happens; after checking the
screenshot/video recording, if you don't see any of the artifacts, it means
that the issue is likely on the driver side. If you can still see the
problem in the data collected, it is an issue that probably happened during
rendering, and the display code just got the framebuffer already corrupted.
(用户可能看到一些屏幕伪影，可以分为两类：局部伪影与全局伪影。局部伪影发生在特定区域，例如 UI 窗口角落附近；如果你看到这类问题，很有可能是用户空间的问题，多半是 Mesa 或类似组件。全局伪影通常发生在整个屏幕。它们可能由驱动层面的显示参数配置错误引起，但用户空间也可能造成该问题。确定问题来源的一种方法是在问题发生时截图或录制桌面视频；检查截图/录像后，如果看不到任何伪影，说明问题很可能在驱动一侧。如果在采集的数据中仍能看到问题，那么它很可能发生在渲染期间，显示代码拿到的帧缓冲已经是损坏的。)

Disabling/Enabling specific features(禁用/启用特定功能)
=======================================================

DC has a struct named `dc_debug_options`, which is statically initialized by
all DCE/DCN components based on the specific hardware characteristic. This
structure usually facilitates the bring-up phase since developers can start
with many disabled features and enable them individually. This is also an
important debug feature since users can change it when debugging specific
issues.
(DC 有一个名为 `dc_debug_options` 的结构体，所有 DCE/DCN 组件会根据具体的硬件特性对它做静态初始化。该结构通常有助于点亮（bring-up）阶段，因为开发者可以从许多功能被禁用的状态开始，再逐个启用。这也是一项重要的调试功能，用户在调试具体问题时可以修改它。)

For example, dGPU users sometimes see a problem where a horizontal fillet of
flickering happens in some specific part of the screen. This could be an
indication of Sub-Viewport issues; after the users identified the target DCN,
they can set the `force_disable_subvp` field to true in the statically
initialized version of `dc_debug_options` to see if the issue gets fixed. Along
the same lines, users/developers can also try to turn off `fams2_config` and
`enable_single_display_2to1_odm_policy`. In summary, the `dc_debug_options` is
an interesting form for identifying the problem.
(例如，dGPU 用户有时会看到屏幕某个特定部分出现一条水平闪烁带。这可能表明存在 Sub-Viewport 问题；用户确认目标 DCN 之后，可以在静态初始化的 `dc_debug_options` 中把 `force_disable_subvp` 字段设为 true，看看问题是否得到解决。同样地，用户/开发者也可以尝试关闭 `fams2_config` 与 `enable_single_display_2to1_odm_policy`。总之，`dc_debug_options` 是定位问题的一种有趣手段。)

DC Visual Confirmation(DC 视觉确认)
===================================

Display core provides a feature named visual confirmation, which is a set of
bars added at the scanout time by the driver to convey some specific
information. In general, you can enable this debug option by using::

  echo <N> > /sys/kernel/debug/dri/0/amdgpu_dm_visual_confirm

(Display core 提供一个名为视觉确认（visual confirmation）的功能，它是驱动在 scanout 时添加的一组条，用来传达某些特定信息。通常可以用以下命令启用该调试选项：)

Where `N` is an integer number for some specific scenarios that the developer
wants to enable, you will see some of these debug cases in the following
subsection.
(其中 `N` 是一个整数，对应开发者想要启用的某些特定场景。在下面的小节中你会看到其中一些调试用例。)

Multiple Planes Debug(多平面调试)
---------------------------------

If you want to enable or debug multiple planes in a specific user-space
application, you can leverage a debug feature named visual confirm. For
enabling it, you will need::

  echo 1 > /sys/kernel/debug/dri/0/amdgpu_dm_visual_confirm

(如果想启用或调试特定用户空间应用中的多平面（multiple planes），可以利用名为 visual confirm 的调试功能。启用它需要：)

You need to reload your GUI to see the visual confirmation. When the plane
configuration changes or a full update occurs there will be a colored bar at
the bottom of each hardware plane being drawn on the screen.
(你需要重新加载 GUI 才能看到视觉确认。当平面配置发生变化或发生全量更新时，屏幕上正在绘制的每个硬件平面底部都会有一条彩条。)

* The color indicates the format - For example, red is AR24 and green is NV12
* The height of the bar indicates the index of the plane
* Pipe split can be observed if there are two bars with a difference in height
  covering the same plane
(颜色表示格式——例如红色是 AR24，绿色是 NV12。条的高度表示平面的索引。如果同一平面上覆盖着两条高度不同的彩条，则可以观察到管道拆分（pipe split）。)

Consider the video playback case in which a video is played in a specific
plane, and the desktop is drawn in another plane. The video plane should
feature one or two green bars at the bottom of the video depending on pipe
split configuration.
(考虑视频播放的情形：视频在特定平面上播放，桌面绘制在另一个平面上。视管道拆分配置而定，视频平面应在视频底部显示一或两条绿色条。)

* There should **not** be any visual corruption
* There should **not** be any underflow or screen flashes
* There should **not** be any black screens
* There should **not** be any cursor corruption
* Multiple plane **may** be briefly disabled during window transitions or
  resizing but should come back after the action has finished
(不应出现任何显示损坏；不应出现任何下溢（underflow）或屏幕闪烁；不应出现任何黑屏；不应出现任何光标损坏；在窗口切换或调整大小期间，多平面*可能*被短暂禁用，但操作结束后应恢复。)

Pipe Split Debug(管道拆分调试)
------------------------------

Sometimes we need to debug if DCN is splitting pipes correctly, and visual
confirmation is also handy for this case. Similar to the MPO case, you can use
the below command to enable visual confirmation::

  echo 1 > /sys/kernel/debug/dri/0/amdgpu_dm_visual_confirm

(有时我们需要调试 DCN 是否正确拆分管道，视觉确认在这种情况下同样好用。与 MPO 的情况类似，可以用以下命令启用视觉确认：)

In this case, if you have a pipe split, you will see one small red bar at the
bottom of the display covering the entire display width and another bar
covering the second pipe. In other words, you will see a bit high bar in the
second pipe.
(这种情况下，如果存在管道拆分，你会看到显示器底部有一条覆盖整个显示宽度的小红条，以及另一条覆盖第二个管道的条。换句话说，你会在第二个管道处看到一条略高的条。)

DTN Debug(DTN 调试)
===================

DC (DCN) provides an extensive log that dumps multiple details from our
hardware configuration. Via debugfs, you can capture those status values by
using Display Test Next (DTN) log, which can be captured via debugfs by using::

  cat /sys/kernel/debug/dri/0/amdgpu_dm_dtn_log

(DC（DCN）提供一份详细的日志，可转储硬件配置的诸多细节。通过 debugfs，可以使用 Display Test Next（DTN）日志捕获这些状态值，捕获方法是：)

Since this log is updated accordingly with DCN status, you can also follow the
change in real-time by using something like::

  sudo watch -d cat /sys/kernel/debug/dri/0/amdgpu_dm_dtn_log

(由于该日志随 DCN 状态而更新，你还可以用类似下面的命令实时跟踪变化：)

When reporting a bug related to DC, consider attaching this log before and
after you reproduce the bug.
(报告与 DC 相关的 bug 时，建议附上复现 bug 之前与之后的这份日志。)

Collect Firmware information(收集固件信息)
==========================================

When reporting issues, it is important to have the firmware information since
it can be helpful for debugging purposes. To get all the firmware information,
use the command::

  cat /sys/kernel/debug/dri/0/amdgpu_firmware_info

(报告问题时，固件信息很重要，因为它有助于调试。要获取全部固件信息，使用以下命令：)

From the display perspective, pay attention to the firmware of the DMCU and
DMCUB.
(从显示的角度看，请留意 DMCU 与 DMCUB 的固件。)

DMUB Firmware Debug(DMUB 固件调试)
==================================

Sometimes, dmesg logs aren't enough. This is especially true if a feature is
implemented primarily in DMUB firmware. In such cases, all we see in dmesg when
an issue arises is some generic timeout error. So, to get more relevant
information, we can trace DMUB commands by enabling the relevant bits in
`amdgpu_dm_dmub_trace_mask`.
(有时 dmesg 日志并不够用。如果某项功能主要在 DMUB 固件中实现，情况尤其如此。这种情况下，问题出现时我们在 dmesg 中看到的只是一些通用的超时错误。因此，为了获得更相关的信息，可以通过启用 `amdgpu_dm_dmub_trace_mask` 中的相关位来跟踪 DMUB 命令。)

Currently, we support the tracing of the following groups:
(目前我们支持跟踪以下组：)

Trace Groups(跟踪组)
--------------------

.. csv-table::
   :header-rows: 1
   :widths: 1, 1
   :file: ./trace-groups-table.csv

**Note: Not all ASICs support all of the listed trace groups**
(**注意：并非所有 ASIC 都支持所列的全部跟踪组**)

So, to enable just PSR tracing you can use the following command::

  # echo 0x8020 > /sys/kernel/debug/dri/0/amdgpu_dm_dmub_trace_mask

(因此，若只启用 PSR 跟踪，可以使用以下命令：)

Then, you need to enable logging trace events to the buffer, which you can do
using the following::

  # echo 1 > /sys/kernel/debug/dri/0/amdgpu_dm_dmcub_trace_event_en

(然后，需要启用把跟踪事件记录到缓冲区，方法是：)

Lastly, after you are able to reproduce the issue you are trying to debug,
you can disable tracing and read the trace log by using the following::

  # echo 0 > /sys/kernel/debug/dri/0/amdgpu_dm_dmcub_trace_event_en
  # cat /sys/kernel/debug/dri/0/amdgpu_dm_dmub_tracebuffer

(最后，在能够复现要调试的问题之后，可以用以下命令禁用跟踪并读取跟踪日志：)

So, when reporting bugs related to features such as PSR and ABM, consider
enabling the relevant bits in the mask before reproducing the issue and
attach the log that you obtain from the trace buffer in any bug reports that you
create.
(因此，在报告与 PSR、ABM 等功能相关的 bug 时，建议在复现问题之前启用掩码中的相关位，并在你提交的任何 bug 报告中附上从跟踪缓冲区获得的日志。)
