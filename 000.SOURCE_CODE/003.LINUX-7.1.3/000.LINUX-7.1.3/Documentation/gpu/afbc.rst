.. SPDX-License-Identifier: GPL-2.0+

==========================================================
 Arm Framebuffer Compression (AFBC)(ARM 帧缓冲压缩 (AFBC))
==========================================================

AFBC is a proprietary lossless image compression protocol and format.
It provides fine-grained random access and minimizes the amount of
data transferred between IP blocks.
(AFBC 是一种专有的无损图像压缩协议与格式。它提供细粒度的随机访问，并最小化 IP 块之间传输的数据量。)

AFBC can be enabled on drivers which support it via use of the AFBC
format modifiers defined in drm_fourcc.h. See DRM_FORMAT_MOD_ARM_AFBC(*).
(可以通过使用 drm_fourcc.h 中定义的 AFBC 格式修饰符，在支持它的驱动上启用 AFBC。见 DRM_FORMAT_MOD_ARM_AFBC(*)。)

All users of the AFBC modifiers must follow the usage guidelines laid
out in this document, to ensure compatibility across different AFBC
producers and consumers.
(所有使用 AFBC 修饰符的用户都必须遵循本文档给出的使用指南，以确保不同 AFBC 生产者与消费者之间的兼容性。)

Components and Ordering(分量与顺序)
===================================

AFBC streams can contain several components - where a component
corresponds to a color channel (i.e. R, G, B, X, A, Y, Cb, Cr).
The assignment of input/output color channels must be consistent
between the encoder and the decoder for correct operation, otherwise
the consumer will interpret the decoded data incorrectly.
(AFBC 流可以包含多个分量 (component)——分量对应一个颜色通道 (即 R、G、B、X、A、Y、Cb、Cr)。为了正确工作，输入/输出颜色通道的指派在编码器与解码器之间必须一致，否则消费者会错误地解读解码后的数据。)

Furthermore, when the lossless colorspace transform is used
(AFBC_FORMAT_MOD_YTR, which should be enabled for RGB buffers for
maximum compression efficiency), the component order must be:
(此外，当使用无损色彩空间变换时 (AFBC_FORMAT_MOD_YTR，对 RGB 缓冲应启用它以获得最大压缩效率)，分量顺序必须是：)

 * Component 0: R
 * Component 1: G
 * Component 2: B

The component ordering is communicated via the fourcc code in the
fourcc:modifier pair. In general, component '0' is considered to
reside in the least-significant bits of the corresponding linear
format. For example, COMP(bits):
(分量顺序通过 fourcc:modifier 对中的 fourcc 码传递。一般而言，分量 '0' 被认为位于对应线性格式的最低有效位。例如，COMP(bits)：)

 * DRM_FORMAT_ABGR8888

   * Component 0: R(8)
   * Component 1: G(8)
   * Component 2: B(8)
   * Component 3: A(8)

 * DRM_FORMAT_BGR888

   * Component 0: R(8)
   * Component 1: G(8)
   * Component 2: B(8)

 * DRM_FORMAT_YUYV

   * Component 0: Y(8)
   * Component 1: Cb(8, 2x1 subsampled)
   * Component 2: Cr(8, 2x1 subsampled)

In AFBC, 'X' components are not treated any differently from any other
component. Therefore, an AFBC buffer with fourcc DRM_FORMAT_XBGR8888
encodes with 4 components, like so:
(在 AFBC 中，'X' 分量与其他任何分量的处理方式没有任何不同。因此，fourcc 为 DRM_FORMAT_XBGR8888 的 AFBC 缓冲以 4 个分量编码，如下：)

 * DRM_FORMAT_XBGR8888

   * Component 0: R(8)
   * Component 1: G(8)
   * Component 2: B(8)
   * Component 3: X(8)

Please note, however, that the inclusion of a "wasted" 'X' channel is
bad for compression efficiency, and so it's recommended to avoid
formats containing 'X' bits. If a fourth component is
required/expected by the encoder/decoder, then it is recommended to
instead use an equivalent format with alpha, setting all alpha bits to
'1'. If there is no requirement for a fourth component, then a format
which doesn't include alpha can be used, e.g. DRM_FORMAT_BGR888.
(不过请注意，包含"浪费的" 'X' 通道对压缩效率不利，因此建议避免使用含 'X' 位的格式。如果编码器/解码器需要/预期第四个分量，建议改用带 alpha 的等价格式，并把所有 alpha 位置为 '1'。如果不需要第四个分量，则可以使用不含 alpha 的格式，例如 DRM_FORMAT_BGR888。)

Number of Planes(平面数)
========================

Formats which are typically multi-planar in linear layouts (e.g. YUV
420), can be encoded into one, or multiple, AFBC planes. As with
component order, the encoder and decoder must agree about the number
of planes in order to correctly decode the buffer. The fourcc code is
used to determine the number of encoded planes in an AFBC buffer,
matching the number of planes for the linear (unmodified) format.
Within each plane, the component ordering also follows the fourcc
code:
(在线性布局中通常为多平面的格式 (例如 YUV 420)，可以编码为一个或多个 AFBC 平面。与分量顺序一样，编码器与解码器必须就平面数达成一致才能正确解码缓冲。fourcc 码用于确定 AFBC 缓冲中编码平面的数量，与线性 (未修改) 格式的平面数相匹配。在每个平面内，分量顺序同样遵循 fourcc 码：)

For example:
(例如：)

 * DRM_FORMAT_YUYV: nplanes = 1

   * Plane 0:

     * Component 0: Y(8)
     * Component 1: Cb(8, 2x1 subsampled)
     * Component 2: Cr(8, 2x1 subsampled)

 * DRM_FORMAT_NV12: nplanes = 2

   * Plane 0:

     * Component 0: Y(8)

   * Plane 1:

     * Component 0: Cb(8, 2x1 subsampled)
     * Component 1: Cr(8, 2x1 subsampled)

Cross-device interoperability(跨设备互操作性)
==============================================

For maximum compatibility across devices, the table below defines
canonical formats for use between AFBC-enabled devices. Formats which
are listed here must be used exactly as specified when using the AFBC
modifiers. Formats which are not listed should be avoided.
(为获得跨设备的最大兼容性，下表定义了在启用 AFBC 的设备之间使用的规范格式。使用 AFBC 修饰符时，此处列出的格式必须严格按规定使用。未列出的格式应避免使用。)

.. flat-table:: AFBC formats

   * - Fourcc code(四字符码)
     - Description(描述)
     - Planes/Components(平面/分量)

   * - DRM_FORMAT_ABGR2101010
     - 10-bit per component RGB, with 2-bit alpha(每分量 10 位的 RGB，带 2 位 alpha)
     - Plane 0: 4 components(平面 0：4 个分量)
              * Component 0: R(10)
              * Component 1: G(10)
              * Component 2: B(10)
              * Component 3: A(2)

   * - DRM_FORMAT_ABGR8888
     - 8-bit per component RGB, with 8-bit alpha(每分量 8 位的 RGB，带 8 位 alpha)
     - Plane 0: 4 components(平面 0：4 个分量)
              * Component 0: R(8)
              * Component 1: G(8)
              * Component 2: B(8)
              * Component 3: A(8)

   * - DRM_FORMAT_BGR888
     - 8-bit per component RGB(每分量 8 位的 RGB)
     - Plane 0: 3 components(平面 0：3 个分量)
              * Component 0: R(8)
              * Component 1: G(8)
              * Component 2: B(8)

   * - DRM_FORMAT_BGR565
     - 5/6-bit per component RGB(每分量 5/6 位的 RGB)
     - Plane 0: 3 components(平面 0：3 个分量)
              * Component 0: R(5)
              * Component 1: G(6)
              * Component 2: B(5)

   * - DRM_FORMAT_ABGR1555
     - 5-bit per component RGB, with 1-bit alpha(每分量 5 位的 RGB，带 1 位 alpha)
     - Plane 0: 4 components(平面 0：4 个分量)
              * Component 0: R(5)
              * Component 1: G(5)
              * Component 2: B(5)
              * Component 3: A(1)

   * - DRM_FORMAT_VUY888
     - 8-bit per component YCbCr 444, single plane(每分量 8 位的 YCbCr 4:4:4，单平面)
     - Plane 0: 3 components(平面 0：3 个分量)
              * Component 0: Y(8)
              * Component 1: Cb(8)
              * Component 2: Cr(8)

   * - DRM_FORMAT_VUY101010
     - 10-bit per component YCbCr 444, single plane(每分量 10 位的 YCbCr 4:4:4，单平面)
     - Plane 0: 3 components(平面 0：3 个分量)
              * Component 0: Y(10)
              * Component 1: Cb(10)
              * Component 2: Cr(10)

   * - DRM_FORMAT_YUYV
     - 8-bit per component YCbCr 422, single plane(每分量 8 位的 YCbCr 4:2:2，单平面)
     - Plane 0: 3 components(平面 0：3 个分量)
              * Component 0: Y(8)
              * Component 1: Cb(8, 2x1 subsampled)
              * Component 2: Cr(8, 2x1 subsampled)

   * - DRM_FORMAT_NV16
     - 8-bit per component YCbCr 422, two plane(每分量 8 位的 YCbCr 4:2:2，双平面)
     - Plane 0: 1 component(平面 0：1 个分量)
              * Component 0: Y(8)
       Plane 1: 2 components(平面 1：2 个分量)
              * Component 0: Cb(8, 2x1 subsampled)
              * Component 1: Cr(8, 2x1 subsampled)

   * - DRM_FORMAT_Y210
     - 10-bit per component YCbCr 422, single plane(每分量 10 位的 YCbCr 4:2:2，单平面)
     - Plane 0: 3 components(平面 0：3 个分量)
              * Component 0: Y(10)
              * Component 1: Cb(10, 2x1 subsampled)
              * Component 2: Cr(10, 2x1 subsampled)

   * - DRM_FORMAT_P210
     - 10-bit per component YCbCr 422, two plane(每分量 10 位的 YCbCr 4:2:2，双平面)
     - Plane 0: 1 component(平面 0：1 个分量)
              * Component 0: Y(10)
       Plane 1: 2 components(平面 1：2 个分量)
              * Component 0: Cb(10, 2x1 subsampled)
              * Component 1: Cr(10, 2x1 subsampled)

   * - DRM_FORMAT_YUV420_8BIT
     - 8-bit per component YCbCr 420, single plane(每分量 8 位的 YCbCr 4:2:0，单平面)
     - Plane 0: 3 components(平面 0：3 个分量)
              * Component 0: Y(8)
              * Component 1: Cb(8, 2x2 subsampled)
              * Component 2: Cr(8, 2x2 subsampled)

   * - DRM_FORMAT_YUV420_10BIT
     - 10-bit per component YCbCr 420, single plane(每分量 10 位的 YCbCr 4:2:0，单平面)
     - Plane 0: 3 components(平面 0：3 个分量)
              * Component 0: Y(10)
              * Component 1: Cb(10, 2x2 subsampled)
              * Component 2: Cr(10, 2x2 subsampled)

   * - DRM_FORMAT_NV12
     - 8-bit per component YCbCr 420, two plane(每分量 8 位的 YCbCr 4:2:0，双平面)
     - Plane 0: 1 component(平面 0：1 个分量)
              * Component 0: Y(8)
       Plane 1: 2 components(平面 1：2 个分量)
              * Component 0: Cb(8, 2x2 subsampled)
              * Component 1: Cr(8, 2x2 subsampled)

   * - DRM_FORMAT_P010
     - 10-bit per component YCbCr 420, two plane(每分量 10 位的 YCbCr 4:2:0，双平面)
     - Plane 0: 1 component(平面 0：1 个分量)
              * Component 0: Y(10)
       Plane 1: 2 components(平面 1：2 个分量)
              * Component 0: Cb(10, 2x2 subsampled)
              * Component 1: Cr(10, 2x2 subsampled)
