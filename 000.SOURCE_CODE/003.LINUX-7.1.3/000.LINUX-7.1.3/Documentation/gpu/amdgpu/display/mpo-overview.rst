==========================================
Multiplane Overlay (MPO)(多平面叠加 (MPO))
==========================================

.. note:: You will get more from this page if you have already read the
   'Documentation/gpu/amdgpu/display/dcn-overview.rst'.
   (如果你已经读过 'Documentation/gpu/amdgpu/display/dcn-overview.rst'，将从本页获得更多收获。)


Multiplane Overlay (MPO) allows for multiple framebuffers to be composited via
fixed-function hardware in the display controller rather than using graphics or
compute shaders for composition. This can yield some power savings if it means
the graphics/compute pipelines can be put into low-power states. In summary,
MPO can bring the following benefits:
(多平面叠加 (MPO) 允许通过显示控制器中的固定功能硬件来合成多个 framebuffer，而不是使用图形或计算着色器进行合成。如果这意味着图形/计算流水线可以进入低功耗状态，就能带来一定的节能效果。总之，MPO 可以带来以下好处：)

* Decreased GPU and CPU workload - no composition shaders needed, no extra
  buffer copy needed, GPU can remain idle.
  (降低 GPU 与 CPU 负载——不需要合成着色器，不需要额外的缓冲拷贝，GPU 可以保持空闲。)
* Plane independent page flips - No need to be tied to global compositor
  page-flip present rate, reduced latency, independent timing.
  (平面独立的页翻转 (page flip)——无需受制于全局合成器的页翻转呈现速率，延迟更低，时序独立。)

.. note:: Keep in mind that MPO is all about power-saving; if you want to learn
   more about power-save in the display context, check the link:
   `Power <https://gitlab.freedesktop.org/pq/color-and-hdr/-/blob/main/doc/power.rst>`__.
   (请记住，MPO 完全是关于节能的；如果想进一步了解显示场景下的省电，请查看链接：
   `Power <https://gitlab.freedesktop.org/pq/color-and-hdr/-/blob/main/doc/power.rst>`__ 。)

Multiplane Overlay is only available using the DRM atomic model. The atomic
model only uses a single userspace IOCTL for configuring the display hardware
(modesetting, page-flipping, etc) - drmModeAtomicCommit. To query hardware
resources and limitations userspace also calls into drmModeGetResources which
reports back the number of planes, CRTCs, and connectors. There are three types
of DRM planes that the driver can register and work with:
(多平面叠加只能通过 DRM atomic 模型使用。atomic 模型只用单个用户态 IOCTL 来配置显示硬件 (模式设置、页翻转等)——drmModeAtomicCommit。为查询硬件资源与限制，用户态还会调用 drmModeGetResources，它回报平面 (plane)、CRTC 与连接器 (connector) 的数量。驱动可以注册并使用三类 DRM 平面：)

* ``DRM_PLANE_TYPE_PRIMARY``: Primary planes represent a "main" plane for a
  CRTC, primary planes are the planes operated upon by CRTC modesetting and
  flipping operations.
* ``DRM_PLANE_TYPE_CURSOR``: Cursor planes represent a "cursor" plane for a
  CRTC. Cursor planes are the planes operated upon by the cursor IOCTLs
* ``DRM_PLANE_TYPE_OVERLAY``: Overlay planes represent all non-primary,
  non-cursor planes. Some drivers refer to these types of planes as "sprites"
  internally.
  (``DRM_PLANE_TYPE_PRIMARY``：主平面代表一个 CRTC 的 "main" (主) 平面，CRTC 模式设置与翻转操作所作用于的平面就是主平面。
  ``DRM_PLANE_TYPE_CURSOR``：光标平面代表一个 CRTC 的 "cursor" (光标) 平面。光标平面是由光标 IOCTL 操作的平面。
  ``DRM_PLANE_TYPE_OVERLAY``：叠加平面代表所有非主、非光标平面。有些驱动在内部把这类平面称为 "sprites"。)

To illustrate how it works, let's take a look at a device that exposes the
following planes to userspace:
(为了说明其工作原理，我们来看一个向用户态暴露以下平面的设备：)

* 4 Primary planes (1 per CRTC).(4 个主平面 (每个 CRTC 一个)。)
* 4 Cursor planes (1 per CRTC).(4 个光标平面 (每个 CRTC 一个)。)
* 1 Overlay plane (shared among CRTCs).(1 个叠加平面 (由各 CRTC 共享)。)

.. note:: Keep in mind that different ASICs might expose other numbers of
   planes.
   (请注意，不同的 ASIC 可能暴露不同数量的平面。)

For this hardware example, we have 4 pipes (if you don't know what AMD pipe
means, look at 'Documentation/gpu/amdgpu/display/dcn-overview.rst', section
"AMD Hardware Pipeline"). Typically most AMD devices operate in a pipe-split
configuration for optimal single display output (e.g., 2 pipes per plane).
(在这个硬件示例中，我们有 4 条 pipe (如果你不知道 AMD pipe 是什么意思，请查阅 'Documentation/gpu/amdgpu/display/dcn-overview.rst' 的 "AMD Hardware Pipeline" 一节)。通常，大多数 AMD 设备为获得最佳单显示器输出而工作在 pipe-split 配置下 (例如每个平面 2 条 pipe)。)

A typical MPO configuration from userspace - 1 primary + 1 overlay on a single
display - will see 4 pipes in use, 2 per plane.
(用户态典型的 MPO 配置——单个显示器上 1 个主平面 + 1 个叠加平面——将使用 4 条 pipe，每个平面 2 条。)

At least 1 pipe must be used per plane (primary and overlay), so for this
hypothetical hardware that we are using as an example, we have an absolute
limit of 4 planes across all CRTCs. Atomic commits will be rejected for display
configurations using more than 4 planes. Again, it is important to stress that
every DCN has different restrictions; here, we are just trying to provide the
concept idea.
(每个平面 (主平面与叠加平面) 至少要用 1 条 pipe，因此对于我们用作示例的这台假想硬件，所有 CRTC 合计的平面上限是 4 个。使用超过 4 个平面的显示配置其 atomic 提交将被拒绝。再次强调，每个 DCN 的限制都不同；这里我们只是想提供概念性的说明。)

Plane Restrictions(平面限制)
============================

AMDGPU imposes restrictions on the use of DRM planes in the driver.
(AMDGPU 在驱动中对 DRM 平面的使用施加了一些限制。)

Atomic commits will be rejected for commits which do not follow these
restrictions:
(不遵守以下限制的 atomic 提交将被拒绝：)

* Overlay planes must be in ARGB8888 or XRGB8888 format
  (叠加平面必须是 ARGB8888 或 XRGB8888 格式)
* Planes cannot be placed outside of the CRTC destination rectangle
  (平面不能被放置到 CRTC 目标矩形之外)
* Planes cannot be downscaled more than 1/4x of their original size
  (平面不能被缩小到原尺寸的 1/4 以下)
* Planes cannot be upscaled more than 16x of their original size
  (平面不能被放大到原尺寸的 16 倍以上)

Not every property is available on every plane:
(并非每个平面都支持所有属性：)

* Only primary planes have color-space and non-RGB format support
  (只有主平面支持色彩空间与非 RGB 格式)
* Only overlay planes have alpha blending support
  (只有叠加平面支持 alpha 混合)

Cursor Restrictions(光标限制)
==============================

Before we start to describe some restrictions around cursor and MPO, see the
below image:
(在开始描述光标与 MPO 相关的一些限制之前，请看下图：)

.. kernel-figure:: mpo-cursor.svg

The image on the left side represents how DRM expects the cursor and planes to
be blended. However, AMD hardware handles cursors differently, as you can see
on the right side; basically, our cursor cannot be drawn outside its associated
plane as it is being treated as part of the plane. Another consequence of that
is that cursors inherit the color and scale from the plane.
(左图表示 DRM 期望的光标与平面混合方式。而 AMD 硬件处理光标的方式不同，如右图所示；基本上，我们的光标不能绘制到其关联平面之外，因为它被视为该平面的一部分。由此带来的另一个结果是光标继承平面的颜色与缩放。)

As a result of the above behavior, do not use legacy API to set up the cursor
plane when working with MPO; otherwise, you might encounter unexpected
behavior.
(由于上述行为，在使用 MPO 时不要用旧式 API 设置光标平面；否则可能遇到意外的行为。)

In short, AMD HW has no dedicated cursor planes. A cursor is attached to
another plane and therefore inherits any scaling or color processing from its
parent plane.
(简而言之，AMD 硬件没有专用的光标平面。光标附着在另一个平面上，因此继承其父平面的任何缩放或颜色处理。)

Use Cases(用例)
===============

Picture-in-Picture (PIP) playback - Underlay strategy(画中画 (PIP) 播放 - 底层 (underlay) 策略)
-----------------------------------------------------------------------------------------------

Video playback should be done using the "primary plane as underlay" MPO
strategy. This is a 2 planes configuration:
(视频播放应使用 "主平面作为底层 (underlay)" 的 MPO 策略。这是一个双平面配置：)

* 1 YUV DRM Primary Plane (e.g. NV12 Video)
  (1 个 YUV DRM 主平面 (例如 NV12 视频)。)
* 1 RGBA DRM Overlay Plane (e.g. ARGB8888 desktop). The compositor should
  prepare the framebuffers for the planes as follows:
  (1 个 RGBA DRM 叠加平面 (例如 ARGB8888 桌面)。合成器应按如下方式为各平面准备 framebuffer：叠加平面包含一般桌面 UI、视频播放器控件与视频字幕；主平面包含一个或多个视频。)
  - The overlay plane contains general desktop UI, video player controls, and video subtitles
  - Primary plane contains one or more videos

.. note:: Keep in mind that we could extend this configuration to more planes,
   but that is currently not supported by our driver yet (maybe if we have a
   userspace request in the future, we can change that).
   (请记住，我们可以把这一配置扩展到更多平面，但目前驱动尚不支持 (如果将来有用户态需求，我们可以改变这一点)。)

See below a single-video example:
(下面是单视频的示例：)

.. kernel-figure:: single-display-mpo.svg

.. note:: We could extend this behavior to more planes, but that is currently
   not supported by our driver.
   (我们可以把这一行为扩展到更多平面，但目前驱动尚不支持。)

The video buffer should be used directly for the primary plane. The video can
be scaled and positioned for the desktop using the properties: CRTC_X, CRTC_Y,
CRTC_W, and CRTC_H. The primary plane should also have the color encoding and
color range properties set based on the source content:
(视频缓冲应直接用于主平面。可以使用属性 CRTC_X、CRTC_Y、CRTC_W 与 CRTC_H 对视频进行缩放和定位。主平面还应根据源内容设置色彩编码与色彩范围属性：)

* ``COLOR_RANGE``, ``COLOR_ENCODING``

The overlay plane should be the native size of the CRTC. The compositor must
draw a transparent cutout for where the video should be placed on the desktop
(i.e., set the alpha to zero). The primary plane video will be visible through
the underlay. The overlay plane's buffer may remain static while the primary
plane's framebuffer is used for standard double-buffered playback.
(叠加平面应为 CRTC 的原生尺寸。合成器必须在桌面上为视频放置位置画一个透明挖空区域 (即把 alpha 置零)。主平面的视频将通过底层透出显示。叠加平面的缓冲可以保持静态，而主平面的 framebuffer 则用于标准的双缓冲播放。)

The compositor should create a YUV buffer matching the native size of the CRTC.
Each video buffer should be composited onto this YUV buffer for direct YUV
scanout. The primary plane should have the color encoding and color range
properties set based on the source content: ``COLOR_RANGE``,
``COLOR_ENCODING``. However, be mindful that the source color space and
encoding match for each video since it affect the entire plane.
(合成器应创建一个与 CRTC 原生尺寸匹配的 YUV 缓冲。每个视频缓冲都应合成到这个 YUV 缓冲上，以便进行直接的 YUV 扫描输出。主平面应根据源内容设置色彩编码与色彩范围属性：``COLOR_RANGE``、``COLOR_ENCODING`` 。但要注意每个视频的源色彩空间与编码要相互匹配，因为这会影响整个平面。)

The overlay plane should be the native size of the CRTC. The compositor must
draw a transparent cutout for where each video should be placed on the desktop
(i.e., set the alpha to zero). The primary plane videos will be visible through
the underlay. The overlay plane's buffer may remain static while compositing
operations for video playback will be done on the video buffer.
(叠加平面应为 CRTC 的原生尺寸。合成器必须在桌面上为每个视频的放置位置画一个透明挖空区域 (即把 alpha 置零)。主平面的视频将通过底层透出显示。叠加平面的缓冲可以保持静态，而视频播放的合成操作则在视频缓冲上进行。)

This kernel interface is validated using IGT GPU Tools. The following tests can
be run to validate positioning, blending, scaling under a variety of sequences
and interactions with operations such as DPMS and S3:
(该内核接口使用 IGT GPU Tools 验证。可以运行以下测试来验证在各种序列下以及与 DPMS、S3 等操作交互时的定位、混合与缩放：)

- ``kms_plane@plane-panning-bottom-right-pipe-*-planes``
- ``kms_plane@plane-panning-bottom-right-suspend-pipe-*-``
- ``kms_plane@plane-panning-top-left-pipe-*-``
- ``kms_plane@plane-position-covered-pipe-*-``
- ``kms_plane@plane-position-hole-dpms-pipe-*-``
- ``kms_plane@plane-position-hole-pipe-*-``
- ``kms_plane_multiple@atomic-pipe-*-tiling-``
- ``kms_plane_scaling@pipe-*-plane-scaling``
- ``kms_plane_alpha_blend@pipe-*-alpha-basic``
- ``kms_plane_alpha_blend@pipe-*-alpha-transparant-fb``
- ``kms_plane_alpha_blend@pipe-*-alpha-opaque-fb``
- ``kms_plane_alpha_blend@pipe-*-constant-alpha-min``
- ``kms_plane_alpha_blend@pipe-*-constant-alpha-mid``
- ``kms_plane_alpha_blend@pipe-*-constant-alpha-max``

- ``kms_plane@plane-panning-bottom-right-pipe-*-planes``
- ``kms_plane@plane-panning-bottom-right-suspend-pipe-*-``
- ``kms_plane@plane-panning-top-left-pipe-*-``
- ``kms_plane@plane-position-covered-pipe-*-``
- ``kms_plane@plane-position-hole-dpms-pipe-*-``
- ``kms_plane@plane-position-hole-pipe-*-``
- ``kms_plane_multiple@atomic-pipe-*-tiling-``
- ``kms_plane_scaling@pipe-*-plane-scaling``
- ``kms_plane_alpha_blend@pipe-*-alpha-basic``
- ``kms_plane_alpha_blend@pipe-*-alpha-transparant-fb``
- ``kms_plane_alpha_blend@pipe-*-alpha-opaque-fb``
- ``kms_plane_alpha_blend@pipe-*-constant-alpha-min``
- ``kms_plane_alpha_blend@pipe-*-constant-alpha-mid``
- ``kms_plane_alpha_blend@pipe-*-constant-alpha-max``

Multiple Display MPO(多显示器 MPO)
----------------------------------

AMDGPU supports display MPO when using multiple displays; however, this feature
behavior heavily relies on the compositor implementation. Keep in mind that
userspace can define different policies. For example, some OSes can use MPO to
protect the plane that handles the video playback; notice that we don't have
many limitations for a single display. Nonetheless, this manipulation can have
many more restrictions for a multi-display scenario. The below example shows a
video playback in the middle of two displays, and it is up to the compositor to
define a policy on how to handle it:
(AMDGPU 在使用多个显示器时支持显示 MPO；不过该特性的行为很大程度上依赖于合成器的实现。请记住，用户态可以定义不同的策略。例如，有些操作系统可以用 MPO 来保护处理视频播放的平面；注意单显示器场景下我们的限制并不多。但在多显示器场景下，这种操作可能受到多得多的限制。下面的例子展示了视频播放位于两个显示器中间的情况，如何处理由合成器自行制定策略：)

.. kernel-figure:: multi-display-hdcp-mpo.svg

Let's discuss some of the hardware limitations we have when dealing with
multi-display with MPO.
(下面讨论使用 MPO 处理多显示器时我们存在的一些硬件限制。)

Limitations(限制)
~~~~~~~~~~~~~~~~~

For simplicity's sake, for discussing the hardware limitation, this
documentation supposes an example where we have two displays and video playback
that will be moved around different displays.
(为简单起见，在讨论硬件限制时，本文档假设这样一个示例：我们有两个显示器，并且视频播放会在不同显示器之间移动。)

* **Hardware limitations**
  (**硬件限制**)

From the DCN overview page, each display requires at least one pipe and each
MPO plane needs another pipe. As a result, when the video is in the middle of
the two displays, we need to use 2 pipes. See the example below where we avoid
pipe split:
(根据 DCN 概述页，每个显示器至少需要一条 pipe，每个 MPO 平面还需要另一条 pipe。因此当视频位于两个显示器中间时，我们需要使用 2 条 pipe。参见下面避免 pipe split 的例子：)

- 1 display (1 pipe) + MPO (1 pipe), we will use two pipes
  (1 个显示器 (1 条 pipe) + MPO (1 条 pipe)，我们将使用两条 pipe)
- 2 displays (2 pipes) + MPO (1-2 pipes); we will use 4 pipes. MPO in the
  middle of both displays needs 2 pipes.
  (2 个显示器 (2 条 pipe) + MPO (1-2 条 pipe)；我们将使用 4 条 pipe。位于两个显示器中间的 MPO 需要 2 条 pipe。)
- 3 Displays (3 pipes) + MPO (1-2 pipes), we need 5 pipes.
  (3 个显示器 (3 条 pipe) + MPO (1-2 条 pipe)，我们需要 5 条 pipe。)

If we use MPO with multiple displays, the userspace has to decide to enable
multiple MPO by the price of limiting the number of external displays supported
or disable it in favor of multiple displays; it is a policy decision. For
example:
(如果在多显示器上使用 MPO，用户态必须做出抉择：以限制支持的外部显示器数量为代价启用多个 MPO，或者为了多显示器而禁用它；这是一个策略决策。例如：)

* When ASIC has 3 pipes, AMD hardware can NOT support 2 displays with MPO
  (当 ASIC 有 3 条 pipe 时，AMD 硬件不能支持带 MPO 的 2 个显示器)
* When ASIC has 4 pipes, AMD hardware can NOT support 3 displays with MPO
  (当 ASIC 有 4 条 pipe 时，AMD 硬件不能支持带 MPO 的 3 个显示器)

Let's briefly explore how userspace can handle these two display configurations
on an ASIC that only supports three pipes. We can have:
(我们简要探讨一下，在只支持 3 条 pipe 的 ASIC 上，用户态可以如何处理这两种显示器配置。情况可以是：)

.. kernel-figure:: multi-display-hdcp-mpo-less-pipe-ex.svg

- Total pipes are 3
  (总共 3 条 pipe)
- User lights up 2 displays (2 out of 3 pipes are used)
  (用户点亮 2 个显示器 (3 条 pipe 中用掉 2 条))
- User launches video (1 pipe used for MPO)
  (用户启动视频 (1 条 pipe 用于 MPO))
- Now, if the user moves the video in the middle of 2 displays, one part of the
  video won't be MPO since we have used 3/3 pipes.
  (此时，如果用户把视频移动到 2 个显示器中间，视频的一部分将不再是 MPO，因为 3/3 条 pipe 已用完。)

* **Scaling limitation**
  (**缩放限制**)

MPO cannot handle scaling less than 0.25 and more than x16. For example:
(MPO 无法处理小于 0.25 或大于 16 倍的缩放。例如：)

If 4k video (3840x2160) is playing in windowed mode, the physical size of the
window cannot be smaller than (960x540).
(如果 4k 视频 (3840x2160) 以窗口模式播放，窗口的物理尺寸不能小于 (960x540)。)

.. note:: These scaling limitations might vary from ASIC to ASIC.
   (这些缩放限制可能因 ASIC 而异。)

* **Size Limitation**
  (**尺寸限制**)

The minimum MPO size is 12px.
(MPO 的最小尺寸是 12px。)
