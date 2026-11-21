.. SPDX-License-Identifier: GPL-2.0

.. include:: <isonum.txt>

OMAP 3 Image Signal Processor (ISP) driver(OMAP 3 图像信号处理器(ISP)驱动)
==========================================================================

Copyright |copy| 2010 Nokia Corporation
(版权所有 |copy| 2010 Nokia Corporation)

Copyright |copy| 2009 Texas Instruments, Inc.
(版权所有 |copy| 2009 Texas Instruments, Inc.)

Contacts: Laurent Pinchart <laurent.pinchart@ideasonboard.com>,
Sakari Ailus <sakari.ailus@iki.fi>, David Cohen <dacohen@gmail.com>
(联系人: Laurent Pinchart <laurent.pinchart@ideasonboard.com>、Sakari Ailus <sakari.ailus@iki.fi>、David Cohen <dacohen@gmail.com>)


Events(事件)
------------

The OMAP 3 ISP driver does support the V4L2 event interface on CCDC and
statistics (AEWB, AF and histogram) subdevs.
(OMAP 3 ISP 驱动在 CCDC 以及统计(AEWB、AF 和直方图)子设备上支持 V4L2 事件接口。)

The CCDC subdev produces V4L2_EVENT_FRAME_SYNC type event on HS_VS
interrupt which is used to signal frame start. Earlier version of this
driver used V4L2_EVENT_OMAP3ISP_HS_VS for this purpose. The event is
triggered exactly when the reception of the first line of the frame starts
in the CCDC module. The event can be subscribed on the CCDC subdev.
(CCDC 子设备会在 HS_VS 中断时产生 V4L2_EVENT_FRAME_SYNC 类型的事件，用于指示帧开始。此驱动的早期版本使用 V4L2_EVENT_OMAP3ISP_HS_VS 来实现这一目的。该事件恰好在 CCDC 模块开始接收帧的第一行时触发。可以在 CCDC 子设备上订阅该事件。)

(When using parallel interface one must pay account to correct configuration
of the VS signal polarity. This is automatically correct when using the serial
receivers.)
(使用并行接口时，必须注意 VS 信号极性的正确配置。使用串行接收器时，极性会自动正确配置。)

Each of the statistics subdevs is able to produce events. An event is
generated whenever a statistics buffer can be dequeued by a user space
application using the VIDIOC_OMAP3ISP_STAT_REQ IOCTL. The events available
are:
(每个统计子设备都能够产生事件。每当用户空间应用程序能够使用 VIDIOC_OMAP3ISP_STAT_REQ IOCTL 把统计缓冲区出队时，就会生成一个事件。可用的事件有：)

- V4L2_EVENT_OMAP3ISP_AEWB
- V4L2_EVENT_OMAP3ISP_AF
- V4L2_EVENT_OMAP3ISP_HIST

The type of the event data is struct omap3isp_stat_event_status for these
ioctls. If there is an error calculating the statistics, there will be an
event as usual, but no related statistics buffer. In this case
omap3isp_stat_event_status.buf_err is set to non-zero.
(对于这些 ioctl，事件数据的类型是 struct omap3isp_stat_event_status。如果计算统计信息时出错，仍会照常产生一个事件，但没有相关的统计缓冲区。在这种情况下，omap3isp_stat_event_status.buf_err 会被置为非零值。)


Private IOCTLs(私有 IOCTL)
--------------------------

The OMAP 3 ISP driver supports standard V4L2 IOCTLs and controls where
possible and practical. Much of the functions provided by the ISP, however,
does not fall under the standard IOCTLs --- gamma tables and configuration of
statistics collection are examples of such.
(OMAP 3 ISP 驱动在可能且实际可行的情况下支持标准 V4L2 IOCTL 与控制项。然而，ISP 提供的许多功能并不属于标准 IOCTL——伽马表与统计采集配置就是这类例子。)

In general, there is a private ioctl for configuring each of the blocks
containing hardware-dependent functions.
(一般而言，每个包含硬件相关功能的块都有一个用于配置它的私有 ioctl。)

The following private IOCTLs are supported:
(支持以下私有 IOCTL：)

- VIDIOC_OMAP3ISP_CCDC_CFG
- VIDIOC_OMAP3ISP_PRV_CFG
- VIDIOC_OMAP3ISP_AEWB_CFG
- VIDIOC_OMAP3ISP_HIST_CFG
- VIDIOC_OMAP3ISP_AF_CFG
- VIDIOC_OMAP3ISP_STAT_REQ
- VIDIOC_OMAP3ISP_STAT_EN

The parameter structures used by these ioctls are described in
include/linux/omap3isp.h. The detailed functions of the ISP itself related to
a given ISP block is described in the Technical Reference Manuals (TRMs) ---
see the end of the document for those.
(这些 ioctl 所使用的参数结构在 include/linux/omap3isp.h 中描述。ISP 本身与给定 ISP 块相关的详细功能在技术参考手册(TRM)中描述——参见文档末尾。)

While it is possible to use the ISP driver without any use of these private
IOCTLs it is not possible to obtain optimal image quality this way. The AEWB,
AF and histogram modules cannot be used without configuring them using the
appropriate private IOCTLs.
(虽然可以在完全不使用这些私有 IOCTL 的情况下使用 ISP 驱动，但这样无法获得最佳图像质量。AEWB、AF 和直方图模块如果不通过相应的私有 IOCTL 进行配置，就无法使用。)


CCDC and preview block IOCTLs(CCDC 与预览块 IOCTL)
--------------------------------------------------

The VIDIOC_OMAP3ISP_CCDC_CFG and VIDIOC_OMAP3ISP_PRV_CFG IOCTLs are used to
configure, enable and disable functions in the CCDC and preview blocks,
respectively. Both IOCTLs control several functions in the blocks they
control. VIDIOC_OMAP3ISP_CCDC_CFG IOCTL accepts a pointer to struct
omap3isp_ccdc_update_config as its argument. Similarly VIDIOC_OMAP3ISP_PRV_CFG
accepts a pointer to struct omap3isp_prev_update_config. The definition of
both structures is available in [#]_.
(VIDIOC_OMAP3ISP_CCDC_CFG 与 VIDIOC_OMAP3ISP_PRV_CFG IOCTL 分别用于配置、启用和禁用 CCDC 与预览块中的功能。两个 IOCTL 各自控制其所在块中的多个功能。VIDIOC_OMAP3ISP_CCDC_CFG IOCTL 接受指向 struct omap3isp_ccdc_update_config 的指针作为参数。类似地，VIDIOC_OMAP3ISP_PRV_CFG 接受指向 struct omap3isp_prev_update_config 的指针。两个结构的定义见文末参考文献。)

The update field in the structures tells whether to update the configuration
for the specific function and the flag tells whether to enable or disable the
function.
(结构体中的 update 字段表明是否要更新特定功能的配置，flag 字段则表明要启用还是禁用该功能。)

The update and flag bit masks accept the following values. Each separate
functions in the CCDC and preview blocks is associated with a flag (either
disable or enable; part of the flag field in the structure) and a pointer to
configuration data for the function.
(update 与 flag 位掩码接受以下取值。CCDC 与预览块中的每个独立功能都关联一个标志(禁用或启用；位于结构的 flag 字段中)以及一个指向该功能配置数据的指针。)

Valid values for the update and flag fields are listed here for
VIDIOC_OMAP3ISP_CCDC_CFG. Values may be or'ed to configure more than one
function in the same IOCTL call.
(这里列出 VIDIOC_OMAP3ISP_CCDC_CFG 的 update 与 flag 字段的有效取值。可以对取值进行按位或运算，以便在同一次 IOCTL 调用中配置多个功能。)

- OMAP3ISP_CCDC_ALAW
- OMAP3ISP_CCDC_LPF
- OMAP3ISP_CCDC_BLCLAMP
- OMAP3ISP_CCDC_BCOMP
- OMAP3ISP_CCDC_FPC
- OMAP3ISP_CCDC_CULL
- OMAP3ISP_CCDC_CONFIG_LSC
- OMAP3ISP_CCDC_TBL_LSC

The corresponding values for the VIDIOC_OMAP3ISP_PRV_CFG are here:
(以下是 VIDIOC_OMAP3ISP_PRV_CFG 的对应取值：)

- OMAP3ISP_PREV_LUMAENH
- OMAP3ISP_PREV_INVALAW
- OMAP3ISP_PREV_HRZ_MED
- OMAP3ISP_PREV_CFA
- OMAP3ISP_PREV_CHROMA_SUPP
- OMAP3ISP_PREV_WB
- OMAP3ISP_PREV_BLKADJ
- OMAP3ISP_PREV_RGB2RGB
- OMAP3ISP_PREV_COLOR_CONV
- OMAP3ISP_PREV_YC_LIMIT
- OMAP3ISP_PREV_DEFECT_COR
- OMAP3ISP_PREV_GAMMABYPASS
- OMAP3ISP_PREV_DRK_FRM_CAPTURE
- OMAP3ISP_PREV_DRK_FRM_SUBTRACT
- OMAP3ISP_PREV_LENS_SHADING
- OMAP3ISP_PREV_NF
- OMAP3ISP_PREV_GAMMA

The associated configuration pointer for the function may not be NULL when
enabling the function. When disabling a function the configuration pointer is
ignored.
(启用某功能时，其关联的配置指针不得为 NULL。禁用某功能时，配置指针会被忽略。)


Statistic blocks IOCTLs(统计块 IOCTL)
-------------------------------------

The statistics subdevs do offer more dynamic configuration options than the
other subdevs. They can be enabled, disable and reconfigured when the pipeline
is in streaming state.
(统计子设备比其他子设备提供更动态的配置选项。在管道处于流式传输状态时，可以启用、禁用并重新配置它们。)

The statistics blocks always get the input image data from the CCDC (as the
histogram memory read isn't implemented). The statistics are dequeueable by
the user from the statistics subdev nodes using private IOCTLs.
(统计块始终从 CCDC 获取输入图像数据(因为直方图内存读取并未实现)。用户可以通过私有 IOCTL 从统计子设备节点把统计数据出队。)

The private IOCTLs offered by the AEWB, AF and histogram subdevs are heavily
reflected by the register level interface offered by the ISP hardware. There
are aspects that are purely related to the driver implementation and these are
discussed next.
(AEWB、AF 和直方图子设备提供的私有 IOCTL 在很大程度上反映了 ISP 硬件提供的寄存器级接口。还有一些纯粹与驱动实现相关的方面，下面将讨论这些内容。)

VIDIOC_OMAP3ISP_STAT_EN(原名)
-----------------------------

This private IOCTL enables/disables a statistic module. If this request is
done before streaming, it will take effect as soon as the pipeline starts to
stream.  If the pipeline is already streaming, it will take effect as soon as
the CCDC becomes idle.
(此私有 IOCTL 用于启用/禁用统计模块。如果此请求在开始流式传输之前完成，它将在管道开始流式传输时立即生效。如果管道已在流式传输，它将在 CCDC 空闲后立即生效。)

VIDIOC_OMAP3ISP_AEWB_CFG, VIDIOC_OMAP3ISP_HIST_CFG and VIDIOC_OMAP3ISP_AF_CFG(原名)
-----------------------------------------------------------------------------------

Those IOCTLs are used to configure the modules. They require user applications
to have an in-depth knowledge of the hardware. Most of the fields explanation
can be found on OMAP's TRMs. The two following fields common to all the above
configure private IOCTLs require explanation for better understanding as they
are not part of the TRM.
(这些 IOCTL 用于配置模块。它们要求用户应用程序对硬件有深入的了解。大多数字段的说明可以在 OMAP 的 TRM 中找到。上述配置私有 IOCTL 共有的以下两个字段不属于 TRM 的内容，需要加以解释以便更好地理解。)

omap3isp_[h3a_af/h3a_aewb/hist]\_config.buf_size:
(omap3isp_[h3a_af/h3a_aewb/hist]\_config.buf_size 字段：)

The modules handle their buffers internally. The necessary buffer size for the
module's data output depends on the requested configuration. Although the
driver supports reconfiguration while streaming, it does not support a
reconfiguration which requires bigger buffer size than what is already
internally allocated if the module is enabled. It will return -EBUSY on this
case. In order to avoid such condition, either disable/reconfigure/enable the
module or request the necessary buffer size during the first configuration
while the module is disabled.
(这些模块在内部处理自己的缓冲区。模块数据输出所需的缓冲区大小取决于所请求的配置。虽然驱动支持在流式传输期间重新配置，但如果模块已启用，它不支持所需缓冲区大小大于已内部分配大小的重新配置。在这种情况下它将返回 -EBUSY。为避免这种情况，可以禁用/重新配置/启用该模块，或者在模块禁用状态下的首次配置时请求所需的缓冲区大小。)

The internal buffer size allocation considers the requested configuration's
minimum buffer size and the value set on buf_size field. If buf_size field is
out of [minimum, maximum] buffer size range, it's clamped to fit in there.
The driver then selects the biggest value. The corrected buf_size value is
written back to user application.
(内部缓冲区大小的分配会考虑所请求配置的最小缓冲区大小与 buf_size 字段中设置的值。如果 buf_size 字段超出[最小值，最大值]缓冲区大小范围，它会被钳制到该范围内。然后驱动选择其中的最大值。修正后的 buf_size 值会写回给用户应用程序。)

omap3isp_[h3a_af/h3a_aewb/hist]\_config.config_counter:
(omap3isp_[h3a_af/h3a_aewb/hist]\_config.config_counter 字段：)

As the configuration doesn't take effect synchronously to the request, the
driver must provide a way to track this information to provide more accurate
data. After a configuration is requested, the config_counter returned to user
space application will be an unique value associated to that request. When
user application receives an event for buffer availability or when a new
buffer is requested, this config_counter is used to match a buffer data and a
configuration.
(由于配置不会相对于请求同步生效，驱动必须提供一种跟踪此信息的方式，以提供更准确的数据。请求一次配置后，返回给用户空间应用程序的 config_counter 将是与该请求关联的唯一值。当用户应用程序收到缓冲区可用的事件或请求新缓冲区时，此 config_counter 用于把缓冲区数据与某次配置匹配起来。)

VIDIOC_OMAP3ISP_STAT_REQ(原名)
------------------------------

Send to user space the oldest data available in the internal buffer queue and
discards such buffer afterwards. The field omap3isp_stat_data.frame_number
matches with the video buffer's field_count.
(把内部缓冲区队列中最旧的可用数据发送给用户空间，之后丢弃该缓冲区。字段 omap3isp_stat_data.frame_number 与视频缓冲区的 field_count 相匹配。)


References(参考文献)
--------------------

.. [#] include/linux/omap3isp.h
