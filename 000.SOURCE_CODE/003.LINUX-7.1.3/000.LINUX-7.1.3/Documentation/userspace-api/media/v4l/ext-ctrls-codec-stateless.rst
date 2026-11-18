.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _codec-stateless-controls:

************************************************************
Stateless Codec Control Reference(无状态编解码器控制参考)
************************************************************

The Stateless Codec control class is intended to support
stateless decoder and encoders (i.e. hardware accelerators).
(Stateless Codec 控制类旨在支持无状态解码器和编码器（即硬件加速器）。)

These drivers are typically supported by the :ref:`stateless_decoder`,
and deal with parsed pixel formats such as V4L2_PIX_FMT_H264_SLICE.
(这些驱动程序通常由 :ref:`stateless_decoder` 支持，并处理诸如 V4L2_PIX_FMT_H264_SLICE
之类的已解析像素格式。)

Stateless Codec Control ID(无状态编解码器控制 ID)
=================================================

.. _codec-stateless-control-id:

``V4L2_CID_CODEC_STATELESS_CLASS (class)`` (类描述符)
    The Stateless Codec class descriptor.
    (Stateless Codec 类的描述符。)

.. _v4l2-codec-stateless-h264:

``V4L2_CID_STATELESS_H264_SPS (struct)`` (H264 序列参数集)
    Specifies the sequence parameter set (as extracted from the
    bitstream) for the associated H264 slice data. This includes the
    necessary parameters for configuring a stateless hardware decoding
    pipeline for H264. The bitstream parameters are defined according
    to :ref:`h264`, section 7.4.2.1.1 "Sequence Parameter Set Data
    Semantics". For further documentation, refer to the above
    specification, unless there is an explicit comment stating
    otherwise.
    (指定相关联 H264 切片数据的序列参数集（从比特流中提取）。其中包含为 H264
    配置无状态硬件解码流水线所需的参数。比特流参数依据 :ref:`h264` 第
    7.4.2.1.1 节 "Sequence Parameter Set Data Semantics" 定义。除非另有明确说明，
    更多文档请参考上述规范。)

.. c:type:: v4l2_ctrl_h264_sps

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.2cm}|p{8.6cm}|p{7.5cm}|

.. flat-table:: struct v4l2_ctrl_h264_sps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``profile_idc``
      -
    * - __u8
      - ``constraint_set_flags``
      - See :ref:`Sequence Parameter Set Constraints Set Flags <h264_sps_constraints_set_flags>`
    * - __u8
      - ``level_idc``
      -
    * - __u8
      - ``seq_parameter_set_id``
      -
    * - __u8
      - ``chroma_format_idc``
      -
    * - __u8
      - ``bit_depth_luma_minus8``
      -
    * - __u8
      - ``bit_depth_chroma_minus8``
      -
    * - __u8
      - ``log2_max_frame_num_minus4``
      -
    * - __u8
      - ``pic_order_cnt_type``
      -
    * - __u8
      - ``log2_max_pic_order_cnt_lsb_minus4``
      -
    * - __u8
      - ``max_num_ref_frames``
      -
    * - __u8
      - ``num_ref_frames_in_pic_order_cnt_cycle``
      -
    * - __s32
      - ``offset_for_ref_frame[255]``
      -
    * - __s32
      - ``offset_for_non_ref_pic``
      -
    * - __s32
      - ``offset_for_top_to_bottom_field``
      -
    * - __u16
      - ``pic_width_in_mbs_minus1``
      -
    * - __u16
      - ``pic_height_in_map_units_minus1``
      -
    * - __u32
      - ``flags``
      - See :ref:`Sequence Parameter Set Flags <h264_sps_flags>`

.. raw:: latex

    \normalsize

.. _h264_sps_constraints_set_flags:

``Sequence Parameter Set Constraints Set Flags`` (序列参数集约束集标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_SPS_CONSTRAINT_SET0_FLAG``
      - 0x00000001
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET1_FLAG``
      - 0x00000002
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET2_FLAG``
      - 0x00000004
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET3_FLAG``
      - 0x00000008
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET4_FLAG``
      - 0x00000010
      -
    * - ``V4L2_H264_SPS_CONSTRAINT_SET5_FLAG``
      - 0x00000020
      -

.. _h264_sps_flags:

``Sequence Parameter Set Flags`` (序列参数集标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_SPS_FLAG_SEPARATE_COLOUR_PLANE``
      - 0x00000001
      -
    * - ``V4L2_H264_SPS_FLAG_QPPRIME_Y_ZERO_TRANSFORM_BYPASS``
      - 0x00000002
      -
    * - ``V4L2_H264_SPS_FLAG_DELTA_PIC_ORDER_ALWAYS_ZERO``
      - 0x00000004
      -
    * - ``V4L2_H264_SPS_FLAG_GAPS_IN_FRAME_NUM_VALUE_ALLOWED``
      - 0x00000008
      -
    * - ``V4L2_H264_SPS_FLAG_FRAME_MBS_ONLY``
      - 0x00000010
      -
    * - ``V4L2_H264_SPS_FLAG_MB_ADAPTIVE_FRAME_FIELD``
      - 0x00000020
      -
    * - ``V4L2_H264_SPS_FLAG_DIRECT_8X8_INFERENCE``
      - 0x00000040
      -

``V4L2_CID_STATELESS_H264_PPS (struct)`` (H264 图像参数集)
    Specifies the picture parameter set (as extracted from the
    bitstream) for the associated H264 slice data. This includes the
    necessary parameters for configuring a stateless hardware decoding
    pipeline for H264.  The bitstream parameters are defined according
    to :ref:`h264`, section 7.4.2.2 "Picture Parameter Set RBSP
    Semantics". For further documentation, refer to the above
    specification, unless there is an explicit comment stating
    otherwise.
    (指定相关联 H264 切片数据的图像参数集（从比特流中提取）。其中包含为 H264
    配置无状态硬件解码流水线所需的参数。比特流参数依据 :ref:`h264` 第
    7.4.2.2 节 "Picture Parameter Set RBSP Semantics" 定义。除非另有明确说明，
    更多文档请参考上述规范。)

.. c:type:: v4l2_ctrl_h264_pps

.. raw:: latex

    \small

.. flat-table:: struct v4l2_ctrl_h264_pps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``pic_parameter_set_id``
      -
    * - __u8
      - ``seq_parameter_set_id``
      -
    * - __u8
      - ``num_slice_groups_minus1``
      -
    * - __u8
      - ``num_ref_idx_l0_default_active_minus1``
      -
    * - __u8
      - ``num_ref_idx_l1_default_active_minus1``
      -
    * - __u8
      - ``weighted_bipred_idc``
      -
    * - __s8
      - ``pic_init_qp_minus26``
      -
    * - __s8
      - ``pic_init_qs_minus26``
      -
    * - __s8
      - ``chroma_qp_index_offset``
      -
    * - __s8
      - ``second_chroma_qp_index_offset``
      -
    * - __u16
      - ``flags``
      - See :ref:`Picture Parameter Set Flags <h264_pps_flags>`

.. raw:: latex

    \normalsize

.. _h264_pps_flags:

``Picture Parameter Set Flags`` (图像参数集标志)

.. raw:: latex

    \begingroup
    \scriptsize
    \setlength{\tabcolsep}{2pt}

.. tabularcolumns:: |p{9.8cm}|p{1.0cm}|p{6.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       10 1 4

    * - ``V4L2_H264_PPS_FLAG_ENTROPY_CODING_MODE``
      - 0x0001
      -
    * - ``V4L2_H264_PPS_FLAG_BOTTOM_FIELD_PIC_ORDER_IN_FRAME_PRESENT``
      - 0x0002
      -
    * - ``V4L2_H264_PPS_FLAG_WEIGHTED_PRED``
      - 0x0004
      -
    * - ``V4L2_H264_PPS_FLAG_DEBLOCKING_FILTER_CONTROL_PRESENT``
      - 0x0008
      -
    * - ``V4L2_H264_PPS_FLAG_CONSTRAINED_INTRA_PRED``
      - 0x0010
      -
    * - ``V4L2_H264_PPS_FLAG_REDUNDANT_PIC_CNT_PRESENT``
      - 0x0020
      -
    * - ``V4L2_H264_PPS_FLAG_TRANSFORM_8X8_MODE``
      - 0x0040
      -
    * - ``V4L2_H264_PPS_FLAG_SCALING_MATRIX_PRESENT``
      - 0x0080
      - ``V4L2_CID_STATELESS_H264_SCALING_MATRIX``
        must be used for this picture.
        (此图像必须使用 ``V4L2_CID_STATELESS_H264_SCALING_MATRIX``。)

.. raw:: latex

    \endgroup

``V4L2_CID_STATELESS_H264_SCALING_MATRIX (struct)`` (H264 缩放矩阵)
    Specifies the scaling matrix (as extracted from the bitstream) for
    the associated H264 slice data. The bitstream parameters are
    defined according to :ref:`h264`, section 7.4.2.1.1.1 "Scaling
    List Semantics". For further documentation, refer to the above
    specification, unless there is an explicit comment stating
    otherwise.
    (指定相关联 H264 切片数据的缩放矩阵（从比特流中提取）。比特流参数依据
    :ref:`h264` 第 7.4.2.1.1.1 节 "Scaling List Semantics" 定义。除非另有明确说明，
    更多文档请参考上述规范。)

.. c:type:: v4l2_ctrl_h264_scaling_matrix

.. raw:: latex

    \small

.. tabularcolumns:: |p{0.6cm}|p{4.8cm}|p{11.9cm}|

.. flat-table:: struct v4l2_ctrl_h264_scaling_matrix
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``scaling_list_4x4[6][16]``
      - Scaling matrix after applying the inverse scanning process.
        Expected list order is Intra Y, Intra Cb, Intra Cr, Inter Y,
        Inter Cb, Inter Cr. The values on each scaling list are
        expected in raster scan order.
        (应用逆扫描过程后的缩放矩阵。列表预期顺序为 Intra Y、Intra Cb、
        Intra Cr、Inter Y、Inter Cb、Inter Cr。每个缩放列表中的值按光栅扫描
        顺序给出。)
    * - __u8
      - ``scaling_list_8x8[6][64]``
      - Scaling matrix after applying the inverse scanning process.
        Expected list order is Intra Y, Inter Y, Intra Cb, Inter Cb,
        Intra Cr, Inter Cr. The values on each scaling list are
        expected in raster scan order.
        (应用逆扫描过程后的缩放矩阵。列表预期顺序为 Intra Y、Inter Y、
        Intra Cb、Inter Cb、Intra Cr、Inter Cr。每个缩放列表中的值按光栅扫描
        顺序给出。)

``V4L2_CID_STATELESS_H264_SLICE_PARAMS (struct)`` (H264 切片参数)
    Specifies the slice parameters (as extracted from the bitstream)
    for the associated H264 slice data. This includes the necessary
    parameters for configuring a stateless hardware decoding pipeline
    for H264.  The bitstream parameters are defined according to
    :ref:`h264`, section 7.4.3 "Slice Header Semantics". For further
    documentation, refer to the above specification, unless there is
    an explicit comment stating otherwise.
    (指定相关联 H264 切片数据的切片参数（从比特流中提取）。其中包含为 H264
    配置无状态硬件解码流水线所需的参数。比特流参数依据 :ref:`h264` 第
    7.4.3 节 "Slice Header Semantics" 定义。除非另有明确说明，更多文档请参考
    上述规范。)

.. c:type:: v4l2_ctrl_h264_slice_params

.. raw:: latex

    \small

.. tabularcolumns:: |p{4.0cm}|p{5.9cm}|p{7.4cm}|

.. flat-table:: struct v4l2_ctrl_h264_slice_params
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``header_bit_size``
      - Offset in bits to slice_data() from the beginning of this slice.
        (从本切片开头到 slice_data() 的位偏移量。)
    * - __u32
      - ``first_mb_in_slice``
      -
    * - __u8
      - ``slice_type``
      -
    * - __u8
      - ``colour_plane_id``
      -
    * - __u8
      - ``redundant_pic_cnt``
      -
    * - __u8
      - ``cabac_init_idc``
      -
    * - __s8
      - ``slice_qp_delta``
      -
    * - __s8
      - ``slice_qs_delta``
      -
    * - __u8
      - ``disable_deblocking_filter_idc``
      -
    * - __s8
      - ``slice_alpha_c0_offset_div2``
      -
    * - __s8
      - ``slice_beta_offset_div2``
      -
    * - __u8
      - ``num_ref_idx_l0_active_minus1``
      - If num_ref_idx_active_override_flag is not set, this field must be
        set to the value of num_ref_idx_l0_default_active_minus1
        (如果未设置 num_ref_idx_active_override_flag，此字段必须设置为
        num_ref_idx_l0_default_active_minus1 的值)
    * - __u8
      - ``num_ref_idx_l1_active_minus1``
      - If num_ref_idx_active_override_flag is not set, this field must be
        set to the value of num_ref_idx_l1_default_active_minus1
        (如果未设置 num_ref_idx_active_override_flag，此字段必须设置为
        num_ref_idx_l1_default_active_minus1 的值)
    * - __u8
      - ``reserved``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - struct :c:type:`v4l2_h264_reference`
      - ``ref_pic_list0[32]``
      - Reference picture list after applying the per-slice modifications
        (应用逐切片修改后的参考图像列表)
    * - struct :c:type:`v4l2_h264_reference`
      - ``ref_pic_list1[32]``
      - Reference picture list after applying the per-slice modifications
        (应用逐切片修改后的参考图像列表)
    * - __u32
      - ``flags``
      - See :ref:`Slice Parameter Flags <h264_slice_flags>`

.. raw:: latex

    \normalsize

.. _h264_slice_flags:

``Slice Parameter Set Flags`` (切片参数集标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_SLICE_FLAG_DIRECT_SPATIAL_MV_PRED``
      - 0x00000001
      -
    * - ``V4L2_H264_SLICE_FLAG_SP_FOR_SWITCH``
      - 0x00000002
      -

``V4L2_CID_STATELESS_H264_PRED_WEIGHTS (struct)`` (H264 预测权重表)
    Prediction weight table defined according to :ref:`h264`,
    section 7.4.3.2 "Prediction Weight Table Semantics".
    The prediction weight table must be passed by applications
    under the conditions explained in section 7.3.3 "Slice header
    syntax".
    (依据 :ref:`h264` 第 7.4.3.2 节 "Prediction Weight Table Semantics" 定义的
    预测权重表。在 7.3.3 节 "Slice header syntax" 所说明的条件下，应用程序必须
    传递该预测权重表。)

.. c:type:: v4l2_ctrl_h264_pred_weights

.. raw:: latex

    \small

.. tabularcolumns:: |p{4.9cm}|p{4.9cm}|p{7.5cm}|

.. flat-table:: struct v4l2_ctrl_h264_pred_weights
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u16
      - ``luma_log2_weight_denom``
      -
    * - __u16
      - ``chroma_log2_weight_denom``
      -
    * - struct :c:type:`v4l2_h264_weight_factors`
      - ``weight_factors[2]``
      - The weight factors at index 0 are the weight factors for the reference
        list 0, the one at index 1 for the reference list 1.
        (索引 0 处的权重因子是参考列表 0 的权重因子，索引 1 处的则是参考列表
        1 的权重因子。)

.. raw:: latex

    \normalsize

.. c:type:: v4l2_h264_weight_factors

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.0cm}|p{4.5cm}|p{11.8cm}|

.. flat-table:: struct v4l2_h264_weight_factors
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s16
      - ``luma_weight[32]``
      -
    * - __s16
      - ``luma_offset[32]``
      -
    * - __s16
      - ``chroma_weight[32][2]``
      -
    * - __s16
      - ``chroma_offset[32][2]``
      -

.. raw:: latex

    \normalsize

``Picture Reference`` (图像引用)

.. c:type:: v4l2_h264_reference

.. cssclass:: longtable

.. flat-table:: struct v4l2_h264_reference
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``fields``
      - Specifies how the picture is referenced. See :ref:`Reference Fields <h264_ref_fields>`
        (指定图像的引用方式。参见 :ref:`Reference Fields <h264_ref_fields>`。)
    * - __u8
      - ``index``
      - Index into the :c:type:`v4l2_ctrl_h264_decode_params`.dpb array.
        (:c:type:`v4l2_ctrl_h264_decode_params`.dpb 数组的索引。)

.. _h264_ref_fields:

``Reference Fields`` (引用字段)

.. raw:: latex

    \small

.. tabularcolumns:: |p{5.4cm}|p{0.8cm}|p{11.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_TOP_FIELD_REF``
      - 0x1
      - The top field in field pair is used for short-term reference.
        (场对中的顶场用于短期参考。)
    * - ``V4L2_H264_BOTTOM_FIELD_REF``
      - 0x2
      - The bottom field in field pair is used for short-term reference.
        (场对中的底场用于短期参考。)
    * - ``V4L2_H264_FRAME_REF``
      - 0x3
      - The frame (or the top/bottom fields, if it's a field pair)
        is used for short-term reference.
        (帧（如果是场对，则为顶场/底场）用于短期参考。)

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_H264_DECODE_PARAMS (struct)`` (H264 解码参数)
    Specifies the decode parameters (as extracted from the bitstream)
    for the associated H264 slice data. This includes the necessary
    parameters for configuring a stateless hardware decoding pipeline
    for H264. The bitstream parameters are defined according to
    :ref:`h264`. For further documentation, refer to the above
    specification, unless there is an explicit comment stating
    otherwise.
    (指定相关联 H264 切片数据的解码参数（从比特流中提取）。其中包含为 H264
    配置无状态硬件解码流水线所需的参数。比特流参数依据 :ref:`h264` 定义。除非
    另有明确说明，更多文档请参考上述规范。)

.. c:type:: v4l2_ctrl_h264_decode_params

.. raw:: latex

    \small

.. tabularcolumns:: |p{4.0cm}|p{5.9cm}|p{7.4cm}|

.. flat-table:: struct v4l2_ctrl_h264_decode_params
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct :c:type:`v4l2_h264_dpb_entry`
      - ``dpb[16]``
      -
    * - __u16
      - ``nal_ref_idc``
      - NAL reference ID value coming from the NAL Unit header
        (来自 NAL 单元头的 NAL 参考标识值)
    * - __u16
      - ``frame_num``
      -
    * - __s32
      - ``top_field_order_cnt``
      - Picture Order Count for the coded top field
        (编码顶场的图像顺序计数)
    * - __s32
      - ``bottom_field_order_cnt``
      - Picture Order Count for the coded bottom field
        (编码底场的图像顺序计数)
    * - __u16
      - ``idr_pic_id``
      -
    * - __u16
      - ``pic_order_cnt_lsb``
      -
    * - __s32
      - ``delta_pic_order_cnt_bottom``
      -
    * - __s32
      - ``delta_pic_order_cnt0``
      -
    * - __s32
      - ``delta_pic_order_cnt1``
      -
    * - __u32
      - ``dec_ref_pic_marking_bit_size``
      - Size in bits of the dec_ref_pic_marking() syntax element.
        (dec_ref_pic_marking() 语法元素的位大小。)
    * - __u32
      - ``pic_order_cnt_bit_size``
      - Combined size in bits of the picture order count related syntax
        elements: pic_order_cnt_lsb, delta_pic_order_cnt_bottom,
        delta_pic_order_cnt0, and delta_pic_order_cnt1.
        (与图像顺序计数相关的各语法元素 pic_order_cnt_lsb、
        delta_pic_order_cnt_bottom、delta_pic_order_cnt0 和
        delta_pic_order_cnt1 的合计位大小。)
    * - __u32
      - ``slice_group_change_cycle``
      -
    * - __u32
      - ``reserved``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - __u32
      - ``flags``
      - See :ref:`Decode Parameters Flags <h264_decode_params_flags>`

.. raw:: latex

    \normalsize

.. _h264_decode_params_flags:

``Decode Parameters Flags`` (解码参数标志)

.. raw:: latex

    \small

.. tabularcolumns:: |p{8.3cm}|p{2.1cm}|p{6.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_DECODE_PARAM_FLAG_IDR_PIC``
      - 0x00000001
      - That picture is an IDR picture
        (该图像是 IDR 图像)
    * - ``V4L2_H264_DECODE_PARAM_FLAG_FIELD_PIC``
      - 0x00000002
      -
    * - ``V4L2_H264_DECODE_PARAM_FLAG_BOTTOM_FIELD``
      - 0x00000004
      -
    * - ``V4L2_H264_DECODE_PARAM_FLAG_PFRAME``
      - 0x00000008
      -
    * - ``V4L2_H264_DECODE_PARAM_FLAG_BFRAME``
      - 0x00000010
      -

.. raw:: latex

    \normalsize

.. c:type:: v4l2_h264_dpb_entry

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.0cm}|p{4.9cm}|p{11.4cm}|

.. flat-table:: struct v4l2_h264_dpb_entry
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u64
      - ``reference_ts``
      - Timestamp of the V4L2 capture buffer to use as reference, used
        with B-coded and P-coded frames. The timestamp refers to the
        ``timestamp`` field in struct :c:type:`v4l2_buffer`. Use the
        :c:func:`v4l2_timeval_to_ns()` function to convert the struct
        :c:type:`timeval` in struct :c:type:`v4l2_buffer` to a __u64.
        (用作参考的 V4L2 捕获缓冲区的时间戳，用于 B 编码和 P 编码帧。该时间戳
        指的是 struct :c:type:`v4l2_buffer` 中的 ``timestamp`` 字段。使用
        :c:func:`v4l2_timeval_to_ns()` 函数把 struct :c:type:`v4l2_buffer`
        中的 struct :c:type:`timeval` 转换为 __u64。)
    * - __u32
      - ``pic_num``
      - For short term references, this must match the derived value PicNum
	(8-28) and for long term references it must match the derived value
	LongTermPicNum (8-29). When decoding frames (as opposed to fields)
	pic_num is the same as FrameNumWrap.
        (对短期参考，此值必须与推导值 PicNum (8-28) 一致；对长期参考，此值必须
        与推导值 LongTermPicNum (8-29) 一致。解码帧（而非场）时，pic_num 与
        FrameNumWrap 相同。)
    * - __u16
      - ``frame_num``
      - For short term references, this must match the frame_num value from
	the slice header syntax (the driver will wrap the value if needed). For
	long term references, this must be set to the value of
	long_term_frame_idx described in the dec_ref_pic_marking() syntax.
        (对短期参考，此值必须与切片头语法中的 frame_num 值一致（必要时驱动程序
        会对该值做回绕）。对长期参考，此值必须设置为 dec_ref_pic_marking()
        语法中描述的 long_term_frame_idx 的值。)
    * - __u8
      - ``fields``
      - Specifies how the DPB entry is referenced. See :ref:`Reference Fields <h264_ref_fields>`
        (指定该 DPB 表项的引用方式。参见 :ref:`Reference Fields <h264_ref_fields>`。)
    * - __u8
      - ``reserved[5]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - __s32
      - ``top_field_order_cnt``
      -
    * - __s32
      - ``bottom_field_order_cnt``
      -
    * - __u32
      - ``flags``
      - See :ref:`DPB Entry Flags <h264_dpb_flags>`

.. raw:: latex

    \normalsize

.. _h264_dpb_flags:

``DPB Entries Flags`` (DPB 表项标志)

.. raw:: latex

    \small

.. tabularcolumns:: |p{7.7cm}|p{2.1cm}|p{7.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_H264_DPB_ENTRY_FLAG_VALID``
      - 0x00000001
      - The DPB entry is valid (non-empty) and should be considered.
        (该 DPB 表项有效（非空），应予以考虑。)
    * - ``V4L2_H264_DPB_ENTRY_FLAG_ACTIVE``
      - 0x00000002
      - The DPB entry is used for reference.
        (该 DPB 表项用于参考。)
    * - ``V4L2_H264_DPB_ENTRY_FLAG_LONG_TERM``
      - 0x00000004
      - The DPB entry is used for long-term reference.
        (该 DPB 表项用于长期参考。)
    * - ``V4L2_H264_DPB_ENTRY_FLAG_FIELD``
      - 0x00000008
      - The DPB entry is a single field or a complementary field pair.
        (该 DPB 表项是单个场或互补场对。)

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_H264_DECODE_MODE (enum)`` (H264 解码模式)
    Specifies the decoding mode to use. Currently exposes slice-based and
    frame-based decoding but new modes might be added later on.
    This control is used as a modifier for V4L2_PIX_FMT_H264_SLICE
    pixel format. Applications that support V4L2_PIX_FMT_H264_SLICE
    are required to set this control in order to specify the decoding mode
    that is expected for the buffer.
    Drivers may expose a single or multiple decoding modes, depending
    on what they can support.
    (指定要使用的解码模式。当前提供基于切片和基于帧的解码，但以后可能会新增
    其他模式。该控制用作 V4L2_PIX_FMT_H264_SLICE 像素格式的修饰符。支持
    V4L2_PIX_FMT_H264_SLICE 的应用程序必须设置此控制，以指明缓冲区预期的
    解码模式。驱动程序可根据自身支持的能力暴露单个或多个解码模式。)

.. c:type:: v4l2_stateless_h264_decode_mode

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_STATELESS_H264_DECODE_MODE_SLICE_BASED``
      - 0
      - Decoding is done at the slice granularity.
        The OUTPUT buffer must contain a single slice.
        When this mode is selected, the ``V4L2_CID_STATELESS_H264_SLICE_PARAMS``
        control shall be set. When multiple slices compose a frame,
        use of ``V4L2_BUF_CAP_SUPPORTS_M2M_HOLD_CAPTURE_BUF`` flag
        is required.
        (以切片为粒度进行解码。OUTPUT 缓冲区必须包含单个切片。选择此模式时，
        应设置 ``V4L2_CID_STATELESS_H264_SLICE_PARAMS`` 控制。当多个切片组成
        一帧时，需要使用 ``V4L2_BUF_CAP_SUPPORTS_M2M_HOLD_CAPTURE_BUF`` 标志。)
    * - ``V4L2_STATELESS_H264_DECODE_MODE_FRAME_BASED``
      - 1
      - Decoding is done at the frame granularity,
        The OUTPUT buffer must contain all slices needed to decode the
        frame. The OUTPUT buffer must also contain both fields.
        This mode will be supported by devices that
        parse the slice(s) header(s) in hardware. When this mode is
        selected, the ``V4L2_CID_STATELESS_H264_SLICE_PARAMS``
        control shall not be set.
        (以帧为粒度进行解码。OUTPUT 缓冲区必须包含解码该帧所需的全部切片，
        还必须包含两个场。此模式将由在硬件中解析切片头的设备支持。选择此模式时，
        不应设置 ``V4L2_CID_STATELESS_H264_SLICE_PARAMS`` 控制。)

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_H264_START_CODE (enum)`` (H264 起始码)
    Specifies the H264 slice start code expected for each slice.
    This control is used as a modifier for V4L2_PIX_FMT_H264_SLICE
    pixel format. Applications that support V4L2_PIX_FMT_H264_SLICE
    are required to set this control in order to specify the start code
    that is expected for the buffer.
    Drivers may expose a single or multiple start codes, depending
    on what they can support.
    (指定每个 H264 切片预期的起始码。该控制用作 V4L2_PIX_FMT_H264_SLICE
    像素格式的修饰符。支持 V4L2_PIX_FMT_H264_SLICE 的应用程序必须设置此控制，
    以指明缓冲区预期的起始码。驱动程序可根据自身支持的能力暴露单个或多个
    起始码。)

.. c:type:: v4l2_stateless_h264_start_code

.. raw:: latex

    \small

.. tabularcolumns:: |p{7.9cm}|p{0.4cm}|p{9.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       4 1 4

    * - ``V4L2_STATELESS_H264_START_CODE_NONE``
      - 0
      - Selecting this value specifies that H264 slices are passed
        to the driver without any start code. The bitstream data should be
        according to :ref:`h264` 7.3.1 NAL unit syntax, hence contains
        emulation prevention bytes when required.
        (选择此值表示传递给驱动程序的 H264 切片不带任何起始码。比特流数据应符合
        :ref:`h264` 7.3.1 NAL 单元语法，因而在需要时包含仿真预防字节。)
    * - ``V4L2_STATELESS_H264_START_CODE_ANNEX_B``
      - 1
      - Selecting this value specifies that H264 slices are expected
        to be prefixed by Annex B start codes. According to :ref:`h264`
        valid start codes can be 3-bytes 0x000001 or 4-bytes 0x00000001.
        (选择此值表示 H264 切片预期以 Annex B 起始码为前缀。依据 :ref:`h264`，
        有效的起始码可以是 3 字节的 0x000001 或 4 字节的 0x00000001。)

.. raw:: latex

    \normalsize

.. _codec-stateless-fwht:

``V4L2_CID_STATELESS_FWHT_PARAMS (struct)`` (FWHT 参数)
    Specifies the FWHT (Fast Walsh Hadamard Transform) parameters (as extracted
    from the bitstream) for the associated FWHT data. This includes the necessary
    parameters for configuring a stateless hardware decoding pipeline for FWHT.
    This codec is specific to the vicodec test driver.
    (指定相关联 FWHT 数据的 FWHT（快速沃尔什-哈达玛变换）参数（从比特流中提取）。
    其中包含为 FWHT 配置无状态硬件解码流水线所需的参数。该编解码器专属于
    vicodec 测试驱动程序。)

.. c:type:: v4l2_ctrl_fwht_params

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.4cm}|p{3.9cm}|p{12.0cm}|

.. flat-table:: struct v4l2_ctrl_fwht_params
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u64
      - ``backward_ref_ts``
      - Timestamp of the V4L2 capture buffer to use as backward reference, used
        with P-coded frames. The timestamp refers to the
	``timestamp`` field in struct :c:type:`v4l2_buffer`. Use the
	:c:func:`v4l2_timeval_to_ns()` function to convert the struct
	:c:type:`timeval` in struct :c:type:`v4l2_buffer` to a __u64.
        (用作后向参考的 V4L2 捕获缓冲区的时间戳，用于 P 编码帧。该时间戳指的是
        struct :c:type:`v4l2_buffer` 中的 ``timestamp`` 字段。使用
        :c:func:`v4l2_timeval_to_ns()` 函数把 struct :c:type:`v4l2_buffer`
        中的 struct :c:type:`timeval` 转换为 __u64。)
    * - __u32
      - ``version``
      - The version of the codec. Set to ``V4L2_FWHT_VERSION``.
        (编解码器版本。设置为 ``V4L2_FWHT_VERSION``。)
    * - __u32
      - ``width``
      - The width of the frame.
        (帧的宽度。)
    * - __u32
      - ``height``
      - The height of the frame.
        (帧的高度。)
    * - __u32
      - ``flags``
      - The flags of the frame, see :ref:`fwht-flags`.
        (帧的标志，参见 :ref:`fwht-flags`。)
    * - __u32
      - ``colorspace``
      - The colorspace of the frame, from enum :c:type:`v4l2_colorspace`.
        (帧的色彩空间，取自枚举 :c:type:`v4l2_colorspace`。)
    * - __u32
      - ``xfer_func``
      - The transfer function, from enum :c:type:`v4l2_xfer_func`.
        (传输函数，取自枚举 :c:type:`v4l2_xfer_func`。)
    * - __u32
      - ``ycbcr_enc``
      - The Y'CbCr encoding, from enum :c:type:`v4l2_ycbcr_encoding`.
        (Y'CbCr 编码，取自枚举 :c:type:`v4l2_ycbcr_encoding`。)
    * - __u32
      - ``quantization``
      - The quantization range, from enum :c:type:`v4l2_quantization`.
        (量化范围，取自枚举 :c:type:`v4l2_quantization`。)

.. raw:: latex

    \normalsize

.. _fwht-flags:

FWHT Flags(FWHT 标志)
=====================

.. raw:: latex

    \small

.. tabularcolumns:: |p{7.0cm}|p{2.3cm}|p{8.0cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       3 1 4

    * - ``V4L2_FWHT_FL_IS_INTERLACED``
      - 0x00000001
      - Set if this is an interlaced format.
        (如果是隔行格式则置位。)
    * - ``V4L2_FWHT_FL_IS_BOTTOM_FIRST``
      - 0x00000002
      - Set if this is a bottom-first (NTSC) interlaced format.
        (如果是底场在先（NTSC）的隔行格式则置位。)
    * - ``V4L2_FWHT_FL_IS_ALTERNATE``
      - 0x00000004
      - Set if each 'frame' contains just one field.
        (如果每个"帧"只包含一个场则置位。)
    * - ``V4L2_FWHT_FL_IS_BOTTOM_FIELD``
      - 0x00000008
      - If V4L2_FWHT_FL_IS_ALTERNATE was set, then this is set if this 'frame' is the
	bottom field, else it is the top field.
        (如果已设置 V4L2_FWHT_FL_IS_ALTERNATE，则当此"帧"是底场时置位，否则是
        顶场。)
    * - ``V4L2_FWHT_FL_LUMA_IS_UNCOMPRESSED``
      - 0x00000010
      - Set if the Y' (luma) plane is uncompressed.
        (如果 Y'（亮度）平面未压缩则置位。)
    * - ``V4L2_FWHT_FL_CB_IS_UNCOMPRESSED``
      - 0x00000020
      - Set if the Cb plane is uncompressed.
        (如果 Cb 平面未压缩则置位。)
    * - ``V4L2_FWHT_FL_CR_IS_UNCOMPRESSED``
      - 0x00000040
      - Set if the Cr plane is uncompressed.
        (如果 Cr 平面未压缩则置位。)
    * - ``V4L2_FWHT_FL_CHROMA_FULL_HEIGHT``
      - 0x00000080
      - Set if the chroma plane has the same height as the luma plane,
	else the chroma plane is half the height of the luma plane.
        (如果色度平面与亮度平面等高则置位，否则色度平面高度为亮度平面的一半。)
    * - ``V4L2_FWHT_FL_CHROMA_FULL_WIDTH``
      - 0x00000100
      - Set if the chroma plane has the same width as the luma plane,
	else the chroma plane is half the width of the luma plane.
        (如果色度平面与亮度平面等宽则置位，否则色度平面宽度为亮度平面的一半。)
    * - ``V4L2_FWHT_FL_ALPHA_IS_UNCOMPRESSED``
      - 0x00000200
      - Set if the alpha plane is uncompressed.
        (如果 alpha 平面未压缩则置位。)
    * - ``V4L2_FWHT_FL_I_FRAME``
      - 0x00000400
      - Set if this is an I-frame.
        (如果这是 I 帧则置位。)
    * - ``V4L2_FWHT_FL_COMPONENTS_NUM_MSK``
      - 0x00070000
      - The number of color components minus one.
        (颜色分量数减一。)
    * - ``V4L2_FWHT_FL_PIXENC_MSK``
      - 0x00180000
      - The mask for the pixel encoding.
        (像素编码的掩码。)
    * - ``V4L2_FWHT_FL_PIXENC_YUV``
      - 0x00080000
      - Set if the pixel encoding is YUV.
        (如果像素编码是 YUV 则置位。)
    * - ``V4L2_FWHT_FL_PIXENC_RGB``
      - 0x00100000
      - Set if the pixel encoding is RGB.
        (如果像素编码是 RGB 则置位。)
    * - ``V4L2_FWHT_FL_PIXENC_HSV``
      - 0x00180000
      - Set if the pixel encoding is HSV.
        (如果像素编码是 HSV 则置位。)

.. raw:: latex

    \normalsize

.. _v4l2-codec-stateless-vp8:

``V4L2_CID_STATELESS_VP8_FRAME (struct)`` (VP8 帧参数)
    Specifies the frame parameters for the associated VP8 parsed frame data.
    This includes the necessary parameters for
    configuring a stateless hardware decoding pipeline for VP8.
    The bitstream parameters are defined according to :ref:`vp8`.
    (指定相关联 VP8 已解析帧数据的帧参数。其中包含为 VP8 配置无状态硬件解码
    流水线所需的参数。比特流参数依据 :ref:`vp8` 定义。)

.. c:type:: v4l2_ctrl_vp8_frame

.. raw:: latex

    \small

.. tabularcolumns:: |p{7.0cm}|p{4.6cm}|p{5.7cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_vp8_frame
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct :c:type:`v4l2_vp8_segment`
      - ``segment``
      - Structure with segment-based adjustments metadata.
        (包含基于分段的调整元数据的结构体。)
    * - struct :c:type:`v4l2_vp8_loop_filter`
      - ``lf``
      - Structure with loop filter level adjustments metadata.
        (包含环路滤波级别调整元数据的结构体。)
    * - struct :c:type:`v4l2_vp8_quantization`
      - ``quant``
      - Structure with VP8 dequantization indices metadata.
        (包含 VP8 反量化索引元数据的结构体。)
    * - struct :c:type:`v4l2_vp8_entropy`
      - ``entropy``
      - Structure with VP8 entropy coder probabilities metadata.
        (包含 VP8 熵编码器概率元数据的结构体。)
    * - struct :c:type:`v4l2_vp8_entropy_coder_state`
      - ``coder_state``
      - Structure with VP8 entropy coder state.
        (包含 VP8 熵编码器状态的结构体。)
    * - __u16
      - ``width``
      - The width of the frame. Must be set for all frames.
        (帧的宽度。所有帧都必须设置。)
    * - __u16
      - ``height``
      - The height of the frame. Must be set for all frames.
        (帧的高度。所有帧都必须设置。)
    * - __u8
      - ``horizontal_scale``
      - Horizontal scaling factor.
        (水平缩放因子。)
    * - __u8
      - ``vertical_scale``
      - Vertical scaling factor.
        (垂直缩放因子。)
    * - __u8
      - ``version``
      - Bitstream version.
        (比特流版本。)
    * - __u8
      - ``prob_skip_false``
      - Indicates the probability that the macroblock is not skipped.
        (表示宏块不被跳过的概率。)
    * - __u8
      - ``prob_intra``
      - Indicates the probability that a macroblock is intra-predicted.
        (表示宏块被帧内预测的概率。)
    * - __u8
      - ``prob_last``
      - Indicates the probability that the last reference frame is used
        for inter-prediction
        (表示最后一个参考帧被用于帧间预测的概率)
    * - __u8
      - ``prob_gf``
      - Indicates the probability that the golden reference frame is used
        for inter-prediction
        (表示黄金参考帧被用于帧间预测的概率)
    * - __u8
      - ``num_dct_parts``
      - Number of DCT coefficients partitions. Must be one of: 1, 2, 4, or 8.
        (DCT 系数分区的数量。必须是 1、2、4 或 8 之一。)
    * - __u32
      - ``first_part_size``
      - Size of the first partition, i.e. the control partition.
        (第一个分区（即控制分区）的大小。)
    * - __u32
      - ``first_part_header_bits``
      - Size in bits of the first partition header portion.
        (第一个分区头部部分的位大小。)
    * - __u32
      - ``dct_part_sizes[8]``
      - DCT coefficients sizes.
        (DCT 系数大小。)
    * - __u64
      - ``last_frame_ts``
      - Timestamp for the V4L2 capture buffer to use as last reference frame, used
        with inter-coded frames. The timestamp refers to the ``timestamp`` field in
	struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
	function to convert the struct :c:type:`timeval` in struct
	:c:type:`v4l2_buffer` to a __u64.
        (用作最后参考帧的 V4L2 捕获缓冲区的时间戳，用于帧间编码帧。该时间戳指的
        是 ``timestamp`` 字段。使用 :c:func:`v4l2_timeval_to_ns()` 函数把
        struct :c:type:`v4l2_buffer` 中的 struct :c:type:`timeval` 转换为
        __u64。)
    * - __u64
      - ``golden_frame_ts``
      - Timestamp for the V4L2 capture buffer to use as last reference frame, used
        with inter-coded frames. The timestamp refers to the ``timestamp`` field in
	struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
	function to convert the struct :c:type:`timeval` in struct
	:c:type:`v4l2_buffer` to a __u64.
        (用作黄金参考帧的 V4L2 捕获缓冲区的时间戳，用于帧间编码帧。该时间戳指的
        是 ``timestamp`` 字段。使用 :c:func:`v4l2_timeval_to_ns()` 函数把
        struct :c:type:`v4l2_buffer` 中的 struct :c:type:`timeval` 转换为
        __u64。)
    * - __u64
      - ``alt_frame_ts``
      - Timestamp for the V4L2 capture buffer to use as alternate reference frame, used
        with inter-coded frames. The timestamp refers to the ``timestamp`` field in
	struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
	function to convert the struct :c:type:`timeval` in struct
	:c:type:`v4l2_buffer` to a __u64.
        (用作备用参考帧的 V4L2 捕获缓冲区的时间戳，用于帧间编码帧。该时间戳指的
        是 ``timestamp`` 字段。使用 :c:func:`v4l2_timeval_to_ns()` 函数把
        struct :c:type:`v4l2_buffer` 中的 struct :c:type:`timeval` 转换为
        __u64。)
    * - __u64
      - ``flags``
      - See :ref:`Frame Flags <vp8_frame_flags>`

.. raw:: latex

    \normalsize

.. _vp8_frame_flags:

``Frame Flags`` (帧标志)

.. tabularcolumns:: |p{9.8cm}|p{0.8cm}|p{6.7cm}|

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP8_FRAME_FLAG_KEY_FRAME``
      - 0x01
      - Indicates if the frame is a key frame.
        (指示该帧是否为关键帧。)
    * - ``V4L2_VP8_FRAME_FLAG_EXPERIMENTAL``
      - 0x02
      - Experimental bitstream.
        (实验性比特流。)
    * - ``V4L2_VP8_FRAME_FLAG_SHOW_FRAME``
      - 0x04
      - Show frame flag, indicates if the frame is for display.
        (显示帧标志，指示该帧是否用于显示。)
    * - ``V4L2_VP8_FRAME_FLAG_MB_NO_SKIP_COEFF``
      - 0x08
      - Enable/disable skipping of macroblocks with no non-zero coefficients.
        (启用/禁用跳过没有非零系数的宏块。)
    * - ``V4L2_VP8_FRAME_FLAG_SIGN_BIAS_GOLDEN``
      - 0x10
      - Sign of motion vectors when the golden frame is referenced.
        (参考黄金帧时运动矢量的符号。)
    * - ``V4L2_VP8_FRAME_FLAG_SIGN_BIAS_ALT``
      - 0x20
      - Sign of motion vectors when the alt frame is referenced.
        (参考备用帧时运动矢量的符号。)

.. c:type:: v4l2_vp8_entropy_coder_state

.. cssclass:: longtable

.. tabularcolumns:: |p{1.0cm}|p{2.0cm}|p{14.3cm}|

.. flat-table:: struct v4l2_vp8_entropy_coder_state
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``range``
      - coder state value for "Range"
        ("Range" 的编码器状态值)
    * - __u8
      - ``value``
      - coder state value for "Value"-
        ("Value" 的编码器状态值)
    * - __u8
      - ``bit_count``
      - number of bits left.
        (剩余位数。)
    * - __u8
      - ``padding``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. c:type:: v4l2_vp8_segment

.. cssclass:: longtable

.. tabularcolumns:: |p{1.2cm}|p{4.0cm}|p{12.1cm}|

.. flat-table:: struct v4l2_vp8_segment
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s8
      - ``quant_update[4]``
      - Signed quantizer value update.
        (带符号的量化器值更新。)
    * - __s8
      - ``lf_update[4]``
      - Signed loop filter level value update.
        (带符号的环路滤波级别值更新。)
    * - __u8
      - ``segment_probs[3]``
      - Segment probabilities.
        (分段概率。)
    * - __u8
      - ``padding``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - __u32
      - ``flags``
      - See :ref:`Segment Flags <vp8_segment_flags>`
        (参见 :ref:`Segment Flags <vp8_segment_flags>`。)

.. _vp8_segment_flags:

``Segment Flags`` (分段标志)

.. raw:: latex

    \small

.. tabularcolumns:: |p{10cm}|p{1.0cm}|p{6.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP8_SEGMENT_FLAG_ENABLED``
      - 0x01
      - Enable/disable segment-based adjustments.
        (启用/禁用基于分段的调整。)
    * - ``V4L2_VP8_SEGMENT_FLAG_UPDATE_MAP``
      - 0x02
      - Indicates if the macroblock segmentation map is updated in this frame.
        (指示本帧中宏块分段图是否更新。)
    * - ``V4L2_VP8_SEGMENT_FLAG_UPDATE_FEATURE_DATA``
      - 0x04
      - Indicates if the segment feature data is updated in this frame.
        (指示本帧中分段特征数据是否更新。)
    * - ``V4L2_VP8_SEGMENT_FLAG_DELTA_VALUE_MODE``
      - 0x08
      - If is set, the segment feature data mode is delta-value.
        If cleared, it's absolute-value.
        (如果置位，分段特征数据模式为增量值 (delta-value)；如果清零，则为
        绝对值 (absolute-value)。)

.. raw:: latex

    \normalsize

.. c:type:: v4l2_vp8_loop_filter

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{3.9cm}|p{11.9cm}|

.. flat-table:: struct v4l2_vp8_loop_filter
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s8
      - ``ref_frm_delta[4]``
      - Reference adjustment (signed) delta value.
        (参考调整（带符号）增量值。)
    * - __s8
      - ``mb_mode_delta[4]``
      - Macroblock prediction mode adjustment (signed) delta value.
        (宏块预测模式调整（带符号）增量值。)
    * - __u8
      - ``sharpness_level``
      - Sharpness level
        (锐度级别)
    * - __u8
      - ``level``
      - Filter level
        (滤波级别)
    * - __u16
      - ``padding``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - __u32
      - ``flags``
      - See :ref:`Loop Filter Flags <vp8_loop_filter_flags>`
        (参见 :ref:`Loop Filter Flags <vp8_loop_filter_flags>`。)

.. _vp8_loop_filter_flags:

``Loop Filter Flags`` (环路滤波标志)

.. tabularcolumns:: |p{7.0cm}|p{1.2cm}|p{9.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP8_LF_ADJ_ENABLE``
      - 0x01
      - Enable/disable macroblock-level loop filter adjustment.
        (启用/禁用宏块级环路滤波调整。)
    * - ``V4L2_VP8_LF_DELTA_UPDATE``
      - 0x02
      - Indicates if the delta values used in an adjustment are updated.
        (指示调整中使用的增量值是否更新。)
    * - ``V4L2_VP8_LF_FILTER_TYPE_SIMPLE``
      - 0x04
      - If set, indicates the filter type is simple.
        If cleared, the filter type is normal.
        (如果置位，表示滤波器类型为简单 (simple)；如果清零，滤波器类型为普通
        (normal)。)

.. c:type:: v4l2_vp8_quantization

.. tabularcolumns:: |p{1.5cm}|p{3.5cm}|p{12.3cm}|

.. flat-table:: struct v4l2_vp8_quantization
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``y_ac_qi``
      - Luma AC coefficient table index.
        (亮度 AC 系数表索引。)
    * - __s8
      - ``y_dc_delta``
      - Luma DC delta value.
        (亮度 DC 增量值。)
    * - __s8
      - ``y2_dc_delta``
      - Y2 block DC delta value.
        (Y2 块 DC 增量值。)
    * - __s8
      - ``y2_ac_delta``
      - Y2 block AC delta value.
        (Y2 块 AC 增量值。)
    * - __s8
      - ``uv_dc_delta``
      - Chroma DC delta value.
        (色度 DC 增量值。)
    * - __s8
      - ``uv_ac_delta``
      - Chroma AC delta value.
        (色度 AC 增量值。)
    * - __u16
      - ``padding``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. c:type:: v4l2_vp8_entropy

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_vp8_entropy
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``coeff_probs[4][8][3][11]``
      - Coefficient update probabilities.
        (系数更新概率。)
    * - __u8
      - ``y_mode_probs[4]``
      - Luma mode update probabilities.
        (亮度模式更新概率。)
    * - __u8
      - ``uv_mode_probs[3]``
      - Chroma mode update probabilities.
        (色度模式更新概率。)
    * - __u8
      - ``mv_probs[2][19]``
      - MV decoding update probabilities.
        (运动矢量解码更新概率。)
    * - __u8
      - ``padding[3]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. _v4l2-codec-stateless-mpeg2:

``V4L2_CID_STATELESS_MPEG2_SEQUENCE (struct)`` (MPEG-2 序列参数)
    Specifies the sequence parameters (as extracted from the bitstream) for the
    associated MPEG-2 slice data. This includes fields matching the syntax
    elements from the sequence header and sequence extension parts of the
    bitstream as specified by :ref:`mpeg2part2`.
    (指定相关联 MPEG-2 切片数据的序列参数（从比特流中提取）。其中包含与
    :ref:`mpeg2part2` 所规定比特流中序列头和序列扩展部分语法元素相对应的字段。)

.. c:type:: v4l2_ctrl_mpeg2_sequence

.. raw:: latex

    \small

.. cssclass:: longtable

.. tabularcolumns:: |p{1.4cm}|p{6.5cm}|p{9.4cm}|

.. flat-table:: struct v4l2_ctrl_mpeg2_sequence
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u16
      - ``horizontal_size``
      - The width of the displayable part of the frame's luminance component.
        (帧亮度分量可显示部分的宽度。)
    * - __u16
      - ``vertical_size``
      - The height of the displayable part of the frame's luminance component.
        (帧亮度分量可显示部分的高度。)
    * - __u32
      - ``vbv_buffer_size``
      - Used to calculate the required size of the video buffering verifier,
	defined (in bits) as: 16 * 1024 * vbv_buffer_size.
        (用于计算视频缓冲校验器所需的大小，以位为单位定义为
        16 * 1024 * vbv_buffer_size。)
    * - __u16
      - ``profile_and_level_indication``
      - The current profile and level indication as extracted from the
	bitstream.
        (从比特流中提取的当前档次 (profile) 和级别 (level) 指示。)
    * - __u8
      - ``chroma_format``
      - The chrominance sub-sampling format (1: 4:2:0, 2: 4:2:2, 3: 4:4:4).
        (色度子采样格式 (1: 4:2:0，2: 4:2:2，3: 4:4:4)。)
    * - __u8
      - ``flags``
      - See :ref:`MPEG-2 Sequence Flags <mpeg2_sequence_flags>`.
        (参见 :ref:`MPEG-2 Sequence Flags <mpeg2_sequence_flags>`。)

.. _mpeg2_sequence_flags:

``MPEG-2 Sequence Flags`` (MPEG-2 序列标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_MPEG2_SEQ_FLAG_PROGRESSIVE``
      - 0x01
      - Indication that all the frames for the sequence are progressive instead
	of interlaced.
        (指示该序列的所有帧都是逐行而非隔行。)

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_MPEG2_PICTURE (struct)`` (MPEG-2 图像参数)
    Specifies the picture parameters (as extracted from the bitstream) for the
    associated MPEG-2 slice data. This includes fields matching the syntax
    elements from the picture header and picture coding extension parts of the
    bitstream as specified by :ref:`mpeg2part2`.
    (指定相关联 MPEG-2 切片数据的图像参数（从比特流中提取）。其中包含与
    :ref:`mpeg2part2` 所规定比特流中图像头和图像编码扩展部分语法元素相对应的字段。)

.. c:type:: v4l2_ctrl_mpeg2_picture

.. raw:: latex

    \small

.. cssclass:: longtable

.. tabularcolumns:: |p{1.0cm}|p{5.6cm}|p{10.7cm}|

.. flat-table:: struct v4l2_ctrl_mpeg2_picture
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u64
      - ``backward_ref_ts``
      - Timestamp of the V4L2 capture buffer to use as backward reference, used
        with B-coded and P-coded frames. The timestamp refers to the
	``timestamp`` field in struct :c:type:`v4l2_buffer`. Use the
	:c:func:`v4l2_timeval_to_ns()` function to convert the struct
	:c:type:`timeval` in struct :c:type:`v4l2_buffer` to a __u64.
        (用作后向参考的 V4L2 捕获缓冲区的时间戳，用于 B 编码和 P 编码帧。该时间戳
        指的是 ``timestamp`` 字段。使用 :c:func:`v4l2_timeval_to_ns()` 函数把
        struct :c:type:`v4l2_buffer` 中的 struct :c:type:`timeval` 转换为
        __u64。)
    * - __u64
      - ``forward_ref_ts``
      - Timestamp for the V4L2 capture buffer to use as forward reference, used
        with B-coded frames. The timestamp refers to the ``timestamp`` field in
	struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
	function to convert the struct :c:type:`timeval` in struct
	:c:type:`v4l2_buffer` to a __u64.
        (用作前向参考的 V4L2 捕获缓冲区的时间戳，用于 B 编码帧。该时间戳指的是
        ``timestamp`` 字段。使用 :c:func:`v4l2_timeval_to_ns()` 函数把
        struct :c:type:`v4l2_buffer` 中的 struct :c:type:`timeval` 转换为
        __u64。)
    * - __u32
      - ``flags``
      - See :ref:`MPEG-2 Picture Flags <mpeg2_picture_flags>`.
        (参见 :ref:`MPEG-2 Picture Flags <mpeg2_picture_flags>`。)
    * - __u8
      - ``f_code[2][2]``
      - Motion vector codes.
        (运动矢量编码。)
    * - __u8
      - ``picture_coding_type``
      - Picture coding type for the frame covered by the current slice
	(V4L2_MPEG2_PIC_CODING_TYPE_I, V4L2_MPEG2_PIC_CODING_TYPE_P or
	V4L2_MPEG2_PIC_CODING_TYPE_B).
        (当前切片所覆盖帧的图像编码类型
        (V4L2_MPEG2_PIC_CODING_TYPE_I、V4L2_MPEG2_PIC_CODING_TYPE_P 或
        V4L2_MPEG2_PIC_CODING_TYPE_B)。)
    * - __u8
      - ``picture_structure``
      - Picture structure (1: interlaced top field, 2: interlaced bottom field,
	3: progressive frame).
        (图像结构 (1: 隔行顶场，2: 隔行底场，3: 逐行帧)。)
    * - __u8
      - ``intra_dc_precision``
      - Precision of Discrete Cosine transform (0: 8 bits precision,
	1: 9 bits precision, 2: 10 bits precision, 3: 11 bits precision).
        (离散余弦变换的精度 (0: 8 位精度，1: 9 位精度，2: 10 位精度，
        3: 11 位精度)。)
    * - __u8
      - ``reserved[5]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. _mpeg2_picture_flags:

``MPEG-2 Picture Flags`` (MPEG-2 图像标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_MPEG2_PIC_FLAG_TOP_FIELD_FIRST``
      - 0x00000001
      - If set and it's an interlaced stream, top field is output first.
        (如果置位且为隔行流，则顶场先输出。)
    * - ``V4L2_MPEG2_PIC_FLAG_FRAME_PRED_DCT``
      - 0x00000002
      - If set only frame-DCT and frame prediction are used.
        (如果置位，则只使用帧 DCT 和帧预测。)
    * - ``V4L2_MPEG2_PIC_FLAG_CONCEALMENT_MV``
      - 0x00000004
      -  If set motion vectors are coded for intra macroblocks.
        (如果置位，则为帧内宏块编码运动矢量。)
    * - ``V4L2_MPEG2_PIC_FLAG_Q_SCALE_TYPE``
      - 0x00000008
      - This flag affects the inverse quantization process.
        (此标志影响反量化过程。)
    * - ``V4L2_MPEG2_PIC_FLAG_INTRA_VLC``
      - 0x00000010
      - This flag affects the decoding of transform coefficient data.
        (此标志影响变换系数数据的解码。)
    * - ``V4L2_MPEG2_PIC_FLAG_ALT_SCAN``
      - 0x00000020
      - This flag affects the decoding of transform coefficient data.
        (此标志影响变换系数数据的解码。)
    * - ``V4L2_MPEG2_PIC_FLAG_REPEAT_FIRST``
      - 0x00000040
      - This flag affects the decoding process of progressive frames.
        (此标志影响逐行帧的解码过程。)
    * - ``V4L2_MPEG2_PIC_FLAG_PROGRESSIVE``
      - 0x00000080
      - Indicates whether the current frame is progressive.
        (指示当前帧是否为逐行帧。)

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_MPEG2_QUANTISATION (struct)`` (MPEG-2 量化矩阵)
    Specifies quantisation matrices, in zigzag scanning order, for the
    associated MPEG-2 slice data. This control is initialized by the kernel
    to the matrices default values. If a bitstream transmits a user-defined
    quantisation matrices load, applications are expected to use this control.
    Applications are also expected to set the control loading the default
    values, if the quantisation matrices need to be reset, for instance on a
    sequence header. This process is specified by section 6.3.7.
    "Quant matrix extension" of the specification.
    (以 Z 字形扫描顺序为相关联的 MPEG-2 切片数据指定量化矩阵。内核会将此控制
    初始化为矩阵的默认值。如果比特流传送了用户定义的量化矩阵加载，应用程序应
    使用此控制。如果量化矩阵需要重置（例如在序列头处），应用程序也应设置此控制
    以加载默认值。此过程由规范第 6.3.7 节 "Quant matrix extension" 规定。)

.. c:type:: v4l2_ctrl_mpeg2_quantisation

.. tabularcolumns:: |p{0.8cm}|p{8.0cm}|p{8.5cm}|

.. cssclass:: longtable

.. raw:: latex

    \small

.. flat-table:: struct v4l2_ctrl_mpeg2_quantisation
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``intra_quantiser_matrix[64]``
      - The quantisation matrix coefficients for intra-coded frames, in zigzag
	scanning order. It is relevant for both luma and chroma components,
	although it can be superseded by the chroma-specific matrix for
	non-4:2:0 YUV formats.
        (帧内编码帧的量化矩阵系数，按 Z 字形扫描顺序排列。它同时适用于亮度和
        色度分量，但对非 4:2:0 的 YUV 格式，可被色度专用矩阵取代。)
    * - __u8
      - ``non_intra_quantiser_matrix[64]``
      - The quantisation matrix coefficients for non-intra-coded frames, in
	zigzag scanning order. It is relevant for both luma and chroma
	components, although it can be superseded by the chroma-specific matrix
	for non-4:2:0 YUV formats.
        (非帧内编码帧的量化矩阵系数，按 Z 字形扫描顺序排列。它同时适用于亮度和
        色度分量，但对非 4:2:0 的 YUV 格式，可被色度专用矩阵取代。)
    * - __u8
      - ``chroma_intra_quantiser_matrix[64]``
      - The quantisation matrix coefficients for the chominance component of
	intra-coded frames, in zigzag scanning order. Only relevant for
	non-4:2:0 YUV formats.
        (帧内编码帧色度分量的量化矩阵系数，按 Z 字形扫描顺序排列。只对非
        4:2:0 的 YUV 格式有意义。)
    * - __u8
      - ``chroma_non_intra_quantiser_matrix[64]``
      - The quantisation matrix coefficients for the chrominance component of
	non-intra-coded frames, in zigzag scanning order. Only relevant for
	non-4:2:0 YUV formats.
        (非帧内编码帧色度分量的量化矩阵系数，按 Z 字形扫描顺序排列。只对非
        4:2:0 的 YUV 格式有意义。)

.. raw:: latex

    \normalsize

.. _v4l2-codec-stateless-vp9:

``V4L2_CID_STATELESS_VP9_COMPRESSED_HDR (struct)`` (VP9 压缩头)
    Stores VP9 probabilities updates as parsed from the current compressed frame
    header. A value of zero in an array element means no update of the relevant
    probability. Motion vector-related updates contain a new value or zero. All
    other updates contain values translated with inv_map_table[] (see 6.3.5 in
    :ref:`vp9`).
    (存储从当前压缩帧头解析出的 VP9 概率更新。数组元素取值为 0 表示相应概率
    不更新。与运动矢量相关的更新包含新值或 0。所有其他更新包含经
    inv_map_table[] 映射的值（见 :ref:`vp9` 中 6.3.5 节）。)

.. c:type:: v4l2_ctrl_vp9_compressed_hdr

.. tabularcolumns:: |p{1cm}|p{4.8cm}|p{11.4cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_vp9_compressed_hdr
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``tx_mode``
      - Specifies the TX mode. See :ref:`TX Mode <vp9_tx_mode>` for more details.
        (指定 TX 模式。详情参见 :ref:`TX Mode <vp9_tx_mode>`。)
    * - __u8
      - ``tx8[2][1]``
      - TX 8x8 probabilities delta.
        (TX 8x8 概率增量。)
    * - __u8
      - ``tx16[2][2]``
      - TX 16x16 probabilities delta.
        (TX 16x16 概率增量。)
    * - __u8
      - ``tx32[2][3]``
      - TX 32x32 probabilities delta.
        (TX 32x32 概率增量。)
    * - __u8
      - ``coef[4][2][2][6][6][3]``
      - Coefficient probabilities delta.
        (系数概率增量。)
    * - __u8
      - ``skip[3]``
      - Skip probabilities delta.
        (跳过概率增量。)
    * - __u8
      - ``inter_mode[7][3]``
      - Inter prediction mode probabilities delta.
        (帧间预测模式概率增量。)
    * - __u8
      - ``interp_filter[4][2]``
      - Interpolation filter probabilities delta.
        (插值滤波器概率增量。)
    * - __u8
      - ``is_inter[4]``
      - Is inter-block probabilities delta.
        (是否为帧间块的概率增量。)
    * - __u8
      - ``comp_mode[5]``
      - Compound prediction mode probabilities delta.
        (复合预测模式概率增量。)
    * - __u8
      - ``single_ref[5][2]``
      - Single reference probabilities delta.
        (单一参考概率增量。)
    * - __u8
      - ``comp_ref[5]``
      - Compound reference probabilities delta.
        (复合参考概率增量。)
    * - __u8
      - ``y_mode[4][9]``
      - Y prediction mode probabilities delta.
        (Y 预测模式概率增量。)
    * - __u8
      - ``uv_mode[10][9]``
      - UV prediction mode probabilities delta.
        (UV 预测模式概率增量。)
    * - __u8
      - ``partition[16][3]``
      - Partition probabilities delta.
        (划分概率增量。)
    * - __u8
      - ``mv.joint[3]``
      - Motion vector joint probabilities delta.
        (运动矢量联合概率增量。)
    * - __u8
      - ``mv.sign[2]``
      - Motion vector sign probabilities delta.
        (运动矢量符号概率增量。)
    * - __u8
      - ``mv.classes[2][10]``
      - Motion vector class probabilities delta.
        (运动矢量类别概率增量。)
    * - __u8
      - ``mv.class0_bit[2]``
      - Motion vector class0 bit probabilities delta.
        (运动矢量 class0 位概率增量。)
    * - __u8
      - ``mv.bits[2][10]``
      - Motion vector bits probabilities delta.
        (运动矢量位概率增量。)
    * - __u8
      - ``mv.class0_fr[2][2][3]``
      - Motion vector class0 fractional bit probabilities delta.
        (运动矢量 class0 小数位概率增量。)
    * - __u8
      - ``mv.fr[2][3]``
      - Motion vector fractional bit probabilities delta.
        (运动矢量小数位概率增量。)
    * - __u8
      - ``mv.class0_hp[2]``
      - Motion vector class0 high precision fractional bit probabilities delta.
        (运动矢量 class0 高精度小数位概率增量。)
    * - __u8
      - ``mv.hp[2]``
      - Motion vector high precision fractional bit probabilities delta.
        (运动矢量高精度小数位概率增量。)

.. _vp9_tx_mode:

``TX Mode`` (TX 模式)

.. tabularcolumns:: |p{6.5cm}|p{0.5cm}|p{10.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_TX_MODE_ONLY_4X4``
      - 0
      - Transform size is 4x4.
        (变换尺寸为 4x4。)
    * - ``V4L2_VP9_TX_MODE_ALLOW_8X8``
      - 1
      - Transform size can be up to 8x8.
        (变换尺寸最大可达 8x8。)
    * - ``V4L2_VP9_TX_MODE_ALLOW_16X16``
      - 2
      - Transform size can be up to 16x16.
        (变换尺寸最大可达 16x16。)
    * - ``V4L2_VP9_TX_MODE_ALLOW_32X32``
      - 3
      - transform size can be up to 32x32.
        (变换尺寸最大可达 32x32。)
    * - ``V4L2_VP9_TX_MODE_SELECT``
      - 4
      - Bitstream contains the transform size for each block.
        (比特流中包含每个块的变换尺寸。)

See section '7.3.1 Tx mode semantics' of the :ref:`vp9` specification for more details.
(更多细节请参见 :ref:`vp9` 规范的 '7.3.1 Tx mode semantics' 一节。)

``V4L2_CID_STATELESS_VP9_FRAME (struct)`` (VP9 帧参数)
    Specifies the frame parameters for the associated VP9 frame decode request.
    This includes the necessary parameters for configuring a stateless hardware
    decoding pipeline for VP9. The bitstream parameters are defined according
    to :ref:`vp9`.
    (指定相关联 VP9 帧解码请求的帧参数。其中包含为 VP9 配置无状态硬件解码
    流水线所需的参数。比特流参数依据 :ref:`vp9` 定义。)

.. c:type:: v4l2_ctrl_vp9_frame

.. raw:: latex

    \small

.. tabularcolumns:: |p{4.7cm}|p{5.5cm}|p{7.1cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_vp9_frame
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct :c:type:`v4l2_vp9_loop_filter`
      - ``lf``
      - Loop filter parameters. See struct :c:type:`v4l2_vp9_loop_filter` for more details.
        (环路滤波参数。详情参见 struct :c:type:`v4l2_vp9_loop_filter`。)
    * - struct :c:type:`v4l2_vp9_quantization`
      - ``quant``
      - Quantization parameters. See :c:type:`v4l2_vp9_quantization` for more details.
        (量化参数。详情参见 :c:type:`v4l2_vp9_quantization`。)
    * - struct :c:type:`v4l2_vp9_segmentation`
      - ``seg``
      - Segmentation parameters. See :c:type:`v4l2_vp9_segmentation` for more details.
        (分段参数。详情参见 :c:type:`v4l2_vp9_segmentation`。)
    * - __u32
      - ``flags``
      - Combination of V4L2_VP9_FRAME_FLAG_* flags. See :ref:`Frame Flags<vp9_frame_flags>`.
        (V4L2_VP9_FRAME_FLAG_* 标志的组合。参见 :ref:`Frame Flags<vp9_frame_flags>`。)
    * - __u16
      - ``compressed_header_size``
      - Compressed header size in bytes.
        (压缩头大小，单位为字节。)
    * - __u16
      - ``uncompressed_header_size``
      - Uncompressed header size in bytes.
        (未压缩头大小，单位为字节。)
    * - __u16
      - ``frame_width_minus_1``
      - Add 1 to get the frame width expressed in pixels. See section 7.2.3 in :ref:`vp9`.
        (加 1 得到以像素表示的帧宽度。参见 :ref:`vp9` 第 7.2.3 节。)
    * - __u16
      - ``frame_height_minus_1``
      - Add 1 to get the frame height expressed in pixels. See section 7.2.3 in :ref:`vp9`.
        (加 1 得到以像素表示的帧高度。参见 :ref:`vp9` 第 7.2.3 节。)
    * - __u16
      - ``render_width_minus_1``
      - Add 1 to get the expected render width expressed in pixels. This is
        not used during the decoding process but might be used by HW scalers to
        prepare a frame that's ready for scanout. See section 7.2.4 in :ref:`vp9`.
        (加 1 得到以像素表示的预期渲染宽度。解码过程不使用它，但硬件缩放器可能
        会用它来准备可供扫描输出的帧。参见 :ref:`vp9` 第 7.2.4 节。)
    * - __u16
      - render_height_minus_1
      - Add 1 to get the expected render height expressed in pixels. This is
        not used during the decoding process but might be used by HW scalers to
        prepare a frame that's ready for scanout. See section 7.2.4 in :ref:`vp9`.
        (加 1 得到以像素表示的预期渲染高度。解码过程不使用它，但硬件缩放器可能
        会用它来准备可供扫描输出的帧。参见 :ref:`vp9` 第 7.2.4 节。)
    * - __u64
      - ``last_frame_ts``
      - "last" reference buffer timestamp.
	The timestamp refers to the ``timestamp`` field in
        struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
        function to convert the struct :c:type:`timeval` in struct
        :c:type:`v4l2_buffer` to a __u64.
        ("last" 参考缓冲区时间戳。该时间戳指的是 ``timestamp`` 字段。使用
        :c:func:`v4l2_timeval_to_ns()` 函数把 struct :c:type:`v4l2_buffer`
        中的 struct :c:type:`timeval` 转换为 __u64。)
    * - __u64
      - ``golden_frame_ts``
      - "golden" reference buffer timestamp.
	The timestamp refers to the ``timestamp`` field in
        struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
        function to convert the struct :c:type:`timeval` in struct
        :c:type:`v4l2_buffer` to a __u64.
        ("golden" 参考缓冲区时间戳。该时间戳指的是 ``timestamp`` 字段。使用
        :c:func:`v4l2_timeval_to_ns()` 函数把 struct :c:type:`v4l2_buffer`
        中的 struct :c:type:`timeval` 转换为 __u64。)
    * - __u64
      - ``alt_frame_ts``
      - "alt" reference buffer timestamp.
	The timestamp refers to the ``timestamp`` field in
        struct :c:type:`v4l2_buffer`. Use the :c:func:`v4l2_timeval_to_ns()`
        function to convert the struct :c:type:`timeval` in struct
        :c:type:`v4l2_buffer` to a __u64.
        ("alt" 参考缓冲区时间戳。该时间戳指的是 ``timestamp`` 字段。使用
        :c:func:`v4l2_timeval_to_ns()` 函数把 struct :c:type:`v4l2_buffer`
        中的 struct :c:type:`timeval` 转换为 __u64。)
    * - __u8
      - ``ref_frame_sign_bias``
      - a bitfield specifying whether the sign bias is set for a given
        reference frame. See :ref:`Reference Frame Sign Bias<vp9_ref_frame_sign_bias>`
        for more details.
        (一个位域，指明对给定参考帧是否设置符号偏置。详情参见
        :ref:`Reference Frame Sign Bias<vp9_ref_frame_sign_bias>`。)
    * - __u8
      - ``reset_frame_context``
      - specifies whether the frame context should be reset to default values. See
        :ref:`Reset Frame Context<vp9_reset_frame_context>` for more details.
        (指明是否应将帧上下文重置为默认值。详情参见
        :ref:`Reset Frame Context<vp9_reset_frame_context>`。)
    * - __u8
      - ``frame_context_idx``
      - Frame context that should be used/updated.
        (应使用/更新的帧上下文。)
    * - __u8
      - ``profile``
      - VP9 profile. Can be 0, 1, 2 or 3.
        (VP9 档次 (profile)。可为 0、1、2 或 3。)
    * - __u8
      - ``bit_depth``
      - Component depth in bits. Can be 8, 10 or 12. Note that not all profiles
        support 10 and/or 12 bits depths.
        (分量位深。可为 8、10 或 12。注意并非所有档次都支持 10 位和/或 12 位
        位深。)
    * - __u8
      - ``interpolation_filter``
      - Specifies the filter selection used for performing inter prediction. See
        :ref:`Interpolation Filter<vp9_interpolation_filter>` for more details.
        (指定执行帧间预测时使用的滤波器选择。详情参见
        :ref:`Interpolation Filter<vp9_interpolation_filter>`。)
    * - __u8
      - ``tile_cols_log2``
      - Specifies the base 2 logarithm of the width of each tile (where the
        width is measured in units of 8x8 blocks). Shall be less than or equal
        to 6.
        (指定每个 tile 宽度（以 8x8 块为单位）以 2 为底的对数。应小于等于 6。)
    * - __u8
      - ``tile_rows_log2``
      - Specifies the base 2 logarithm of the height of each tile (where the
        height is measured in units of 8x8 blocks).
        (指定每个 tile 高度（以 8x8 块为单位）以 2 为底的对数。)
    * - __u8
      - ``reference_mode``
      - Specifies the type of inter prediction to be used. See
        :ref:`Reference Mode<vp9_reference_mode>` for more details. Note that
	this is derived as part of the compressed header parsing process and
	for this reason should have been part of
	:c:type: `v4l2_ctrl_vp9_compressed_hdr` optional control. It is safe to
	set this value to zero if the driver does not require compressed
	headers.
        (指定要使用的帧间预测类型。详情参见
        :ref:`Reference Mode<vp9_reference_mode>`。注意，该值是在压缩头解析
        过程中推导出来的，因此本应属于 v4l2_ctrl_vp9_compressed_hdr 可选控制
        的一部分。如果驱动程序不需要压缩头，将此值设为 0 是安全的。)
    * - __u8
      - ``reserved[7]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. raw:: latex

    \normalsize

.. _vp9_frame_flags:

``Frame Flags`` (帧标志)

.. tabularcolumns:: |p{10.0cm}|p{1.2cm}|p{6.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_FRAME_FLAG_KEY_FRAME``
      - 0x001
      - The frame is a key frame.
        (该帧是关键帧。)
    * - ``V4L2_VP9_FRAME_FLAG_SHOW_FRAME``
      - 0x002
      - The frame should be displayed.
        (该帧应当被显示。)
    * - ``V4L2_VP9_FRAME_FLAG_ERROR_RESILIENT``
      - 0x004
      - The decoding should be error resilient.
        (解码应具备容错能力。)
    * - ``V4L2_VP9_FRAME_FLAG_INTRA_ONLY``
      - 0x008
      - The frame does not reference other frames.
        (该帧不参考其他帧。)
    * - ``V4L2_VP9_FRAME_FLAG_ALLOW_HIGH_PREC_MV``
      - 0x010
      - The frame can use high precision motion vectors.
        (该帧可以使用高精度运动矢量。)
    * - ``V4L2_VP9_FRAME_FLAG_REFRESH_FRAME_CTX``
      - 0x020
      - Frame context should be updated after decoding.
        (解码后应更新帧上下文。)
    * - ``V4L2_VP9_FRAME_FLAG_PARALLEL_DEC_MODE``
      - 0x040
      - Parallel decoding is used.
        (采用并行解码。)
    * - ``V4L2_VP9_FRAME_FLAG_X_SUBSAMPLING``
      - 0x080
      - Vertical subsampling is enabled.
        (垂直子采样已启用。)
    * - ``V4L2_VP9_FRAME_FLAG_Y_SUBSAMPLING``
      - 0x100
      - Horizontal subsampling is enabled.
        (水平子采样已启用。)
    * - ``V4L2_VP9_FRAME_FLAG_COLOR_RANGE_FULL_SWING``
      - 0x200
      - The full UV range is used.
        (使用全量程 UV 范围。)

.. _vp9_ref_frame_sign_bias:

``Reference Frame Sign Bias`` (参考帧符号偏置)

.. tabularcolumns:: |p{7.0cm}|p{1.2cm}|p{9.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_SIGN_BIAS_LAST``
      - 0x1
      - Sign bias is set for the last reference frame.
        (最后一个参考帧设置了符号偏置。)
    * - ``V4L2_VP9_SIGN_BIAS_GOLDEN``
      - 0x2
      - Sign bias is set for the golden reference frame.
        (黄金参考帧设置了符号偏置。)
    * - ``V4L2_VP9_SIGN_BIAS_ALT``
      - 0x2
      - Sign bias is set for the alt reference frame.
        (备用参考帧设置了符号偏置。)

.. _vp9_reset_frame_context:

``Reset Frame Context`` (重置帧上下文)

.. tabularcolumns:: |p{7.0cm}|p{1.2cm}|p{9.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_RESET_FRAME_CTX_NONE``
      - 0
      - Do not reset any frame context.
        (不重置任何帧上下文。)
    * - ``V4L2_VP9_RESET_FRAME_CTX_SPEC``
      - 1
      - Reset the frame context pointed to by
        :c:type:`v4l2_ctrl_vp9_frame`.frame_context_idx.
        (重置 :c:type:`v4l2_ctrl_vp9_frame`.frame_context_idx 所指向的帧
        上下文。)
    * - ``V4L2_VP9_RESET_FRAME_CTX_ALL``
      - 2
      - Reset all frame contexts.
        (重置所有帧上下文。)

See section '7.2 Uncompressed header semantics' of the :ref:`vp9` specification
for more details.
(更多细节请参见 :ref:`vp9` 规范的 '7.2 Uncompressed header semantics' 一节。)

.. _vp9_interpolation_filter:

``Interpolation Filter`` (插值滤波器)

.. tabularcolumns:: |p{9.0cm}|p{1.2cm}|p{7.1cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_INTERP_FILTER_EIGHTTAP``
      - 0
      - Eight tap filter.
        (八抽头滤波器。)
    * - ``V4L2_VP9_INTERP_FILTER_EIGHTTAP_SMOOTH``
      - 1
      - Eight tap smooth filter.
        (八抽头平滑滤波器。)
    * - ``V4L2_VP9_INTERP_FILTER_EIGHTTAP_SHARP``
      - 2
      - Eeight tap sharp filter.
        (八抽头锐利滤波器。)
    * - ``V4L2_VP9_INTERP_FILTER_BILINEAR``
      - 3
      - Bilinear filter.
        (双线性滤波器。)
    * - ``V4L2_VP9_INTERP_FILTER_SWITCHABLE``
      - 4
      - Filter selection is signaled at the block level.
        (滤波器选择在块级别上发出信号。)

See section '7.2.7 Interpolation filter semantics' of the :ref:`vp9` specification
for more details.
(更多细节请参见 :ref:`vp9` 规范的 '7.2.7 Interpolation filter semantics' 一节。)

.. _vp9_reference_mode:

``Reference Mode`` (参考模式)

.. tabularcolumns:: |p{9.6cm}|p{0.5cm}|p{7.2cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_REFERENCE_MODE_SINGLE_REFERENCE``
      - 0
      - Indicates that all the inter blocks use only a single reference frame
        to generate motion compensated prediction.
        (指示所有帧间块只使用单个参考帧来生成运动补偿预测。)
    * - ``V4L2_VP9_REFERENCE_MODE_COMPOUND_REFERENCE``
      - 1
      - Requires all the inter blocks to use compound mode. Single reference
        frame prediction is not allowed.
        (要求所有帧间块使用复合模式。不允许单一参考帧预测。)
    * - ``V4L2_VP9_REFERENCE_MODE_SELECT``
      - 2
      - Allows each individual inter block to select between single and
        compound prediction modes.
        (允许每个帧间块在单一预测模式和复合预测模式之间进行选择。)

See section '7.3.6 Frame reference mode semantics' of the :ref:`vp9` specification for more details.
(更多细节请参见 :ref:`vp9` 规范的 '7.3.6 Frame reference mode semantics' 一节。)

.. c:type:: v4l2_vp9_segmentation

Encodes the quantization parameters. See section '7.2.10 Segmentation
params syntax' of the :ref:`vp9` specification for more details.
(编码量化参数。更多细节请参见 :ref:`vp9` 规范的 '7.2.10 Segmentation
params syntax' 一节。)

.. tabularcolumns:: |p{0.8cm}|p{5cm}|p{11.4cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_vp9_segmentation
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``feature_data[8][4]``
      - Data attached to each feature. Data entry is only valid if the feature
        is enabled. The array shall be indexed with segment number as the first dimension
        (0..7) and one of V4L2_VP9_SEG_* as the second dimension.
        See :ref:`Segment Feature IDs<vp9_segment_feature>`.
        (附属于每个特征的数据。只有特征启用时数据项才有效。数组的第一维应以
        分段编号 (0..7) 为索引，第二维以 V4L2_VP9_SEG_* 之一为索引。参见
        :ref:`Segment Feature IDs<vp9_segment_feature>`。)
    * - __u8
      - ``feature_enabled[8]``
      - Bitmask defining which features are enabled in each segment. The value for each
        segment is a combination of V4L2_VP9_SEGMENT_FEATURE_ENABLED(id) values where id is
        one of V4L2_VP9_SEG_*. See :ref:`Segment Feature IDs<vp9_segment_feature>`.
        (定义每个分段中启用了哪些特征的位掩码。每个分段的值是
        V4L2_VP9_SEGMENT_FEATURE_ENABLED(id) 值的组合，其中 id 是
        V4L2_VP9_SEG_* 之一。参见
        :ref:`Segment Feature IDs<vp9_segment_feature>`。)
    * - __u8
      - ``tree_probs[7]``
      - Specifies the probability values to be used when decoding a Segment-ID.
        See '5.15 Segmentation map' section of :ref:`vp9` for more details.
        (指定解码 Segment-ID 时要使用的概率值。更多细节请参见 :ref:`vp9` 的
        '5.15 Segmentation map' 一节。)
    * - __u8
      - ``pred_probs[3]``
      - Specifies the probability values to be used when decoding a
        Predicted-Segment-ID. See '6.4.14 Get segment id syntax'
        section of :ref:`vp9` for more details.
        (指定解码 Predicted-Segment-ID 时要使用的概率值。更多细节请参见
        :ref:`vp9` 的 '6.4.14 Get segment id syntax' 一节。)
    * - __u8
      - ``flags``
      - Combination of V4L2_VP9_SEGMENTATION_FLAG_* flags. See
        :ref:`Segmentation Flags<vp9_segmentation_flags>`.
        (V4L2_VP9_SEGMENTATION_FLAG_* 标志的组合。参见
        :ref:`Segmentation Flags<vp9_segmentation_flags>`。)
    * - __u8
      - ``reserved[5]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. _vp9_segment_feature:

``Segment feature IDs`` (分段特征 ID)

.. tabularcolumns:: |p{6.0cm}|p{1cm}|p{10.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_SEG_LVL_ALT_Q``
      - 0
      - Quantizer segment feature.
        (量化器分段特征。)
    * - ``V4L2_VP9_SEG_LVL_ALT_L``
      - 1
      - Loop filter segment feature.
        (环路滤波分段特征。)
    * - ``V4L2_VP9_SEG_LVL_REF_FRAME``
      - 2
      - Reference frame segment feature.
        (参考帧分段特征。)
    * - ``V4L2_VP9_SEG_LVL_SKIP``
      - 3
      - Skip segment feature.
        (跳过分段特征。)
    * - ``V4L2_VP9_SEG_LVL_MAX``
      - 4
      - Number of segment features.
        (分段特征的数量。)

.. _vp9_segmentation_flags:

``Segmentation Flags`` (分段标志)

.. tabularcolumns:: |p{10.6cm}|p{0.8cm}|p{5.9cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_SEGMENTATION_FLAG_ENABLED``
      - 0x01
      - Indicates that this frame makes use of the segmentation tool.
        (指示本帧使用了分段工具。)
    * - ``V4L2_VP9_SEGMENTATION_FLAG_UPDATE_MAP``
      - 0x02
      - Indicates that the segmentation map should be updated during the
        decoding of this frame.
        (指示在解码本帧期间应更新分段图。)
    * - ``V4L2_VP9_SEGMENTATION_FLAG_TEMPORAL_UPDATE``
      - 0x04
      - Indicates that the updates to the segmentation map are coded
        relative to the existing segmentation map.
        (指示分段图的更新是相对于现有分段图进行编码的。)
    * - ``V4L2_VP9_SEGMENTATION_FLAG_UPDATE_DATA``
      - 0x08
      - Indicates that new parameters are about to be specified for each
        segment.
        (指示即将为每个分段指定新参数。)
    * - ``V4L2_VP9_SEGMENTATION_FLAG_ABS_OR_DELTA_UPDATE``
      - 0x10
      - Indicates that the segmentation parameters represent the actual values
        to be used.
        (指示分段参数表示要使用的实际值。)

.. c:type:: v4l2_vp9_quantization

Encodes the quantization parameters. See section '7.2.9 Quantization params
syntax' of the VP9 specification for more details.
(编码量化参数。更多细节请参见 VP9 规范的 '7.2.9 Quantization params
syntax' 一节。)

.. tabularcolumns:: |p{0.8cm}|p{4cm}|p{12.4cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_vp9_quantization
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``base_q_idx``
      - Indicates the base frame qindex.
        (指示基础帧 qindex。)
    * - __s8
      - ``delta_q_y_dc``
      - Indicates the Y DC quantizer relative to base_q_idx.
        (指示相对于 base_q_idx 的 Y DC 量化器。)
    * - __s8
      - ``delta_q_uv_dc``
      - Indicates the UV DC quantizer relative to base_q_idx.
        (指示相对于 base_q_idx 的 UV DC 量化器。)
    * - __s8
      - ``delta_q_uv_ac``
      - Indicates the UV AC quantizer relative to base_q_idx.
        (指示相对于 base_q_idx 的 UV AC 量化器。)
    * - __u8
      - ``reserved[4]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. c:type:: v4l2_vp9_loop_filter

This structure contains all loop filter related parameters. See sections
'7.2.8 Loop filter semantics' of the :ref:`vp9` specification for more details.
(此结构体包含所有与环路滤波相关的参数。更多细节请参见 :ref:`vp9` 规范的
'7.2.8 Loop filter semantics' 一节。)

.. tabularcolumns:: |p{0.8cm}|p{4cm}|p{12.4cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_vp9_loop_filter
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s8
      - ``ref_deltas[4]``
      - Contains the adjustment needed for the filter level based on the chosen
        reference frame.
        (包含基于所选参考帧对滤波级别所需的调整。)
    * - __s8
      - ``mode_deltas[2]``
      - Contains the adjustment needed for the filter level based on the chosen
        mode.
        (包含基于所选模式对滤波级别所需的调整。)
    * - __u8
      - ``level``
      - Indicates the loop filter strength.
        (指示环路滤波强度。)
    * - __u8
      - ``sharpness``
      - Indicates the sharpness level.
        (指示锐度级别。)
    * - __u8
      - ``flags``
      - Combination of V4L2_VP9_LOOP_FILTER_FLAG_* flags.
        See :ref:`Loop Filter Flags <vp9_loop_filter_flags>`.
        (V4L2_VP9_LOOP_FILTER_FLAG_* 标志的组合。参见
        :ref:`Loop Filter Flags <vp9_loop_filter_flags>`。)
    * - __u8
      - ``reserved[7]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)


.. _vp9_loop_filter_flags:

``Loop Filter Flags`` (环路滤波标志)

.. tabularcolumns:: |p{9.6cm}|p{0.5cm}|p{7.2cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_VP9_LOOP_FILTER_FLAG_DELTA_ENABLED``
      - 0x1
      - When set, the filter level depends on the mode and reference frame used
        to predict a block.
        (置位时，滤波级别取决于用于预测块的模式和参考帧。)
    * - ``V4L2_VP9_LOOP_FILTER_FLAG_DELTA_UPDATE``
      - 0x2
      - When set, the bitstream contains additional syntax elements that
        specify which mode and reference frame deltas are to be updated.
        (置位时，比特流中包含额外的语法元素，用于指明要更新哪些模式与参考帧
        增量。)

.. _v4l2-codec-stateless-hevc:

``V4L2_CID_STATELESS_HEVC_SPS (struct)`` (HEVC 序列参数集)
    Specifies the Sequence Parameter Set fields (as extracted from the
    bitstream) for the associated HEVC slice data.
    These bitstream parameters are defined according to :ref:`hevc`.
    They are described in section 7.4.3.2 "Sequence parameter set RBSP
    semantics" of the specification.
    (指定相关联 HEVC 切片数据的序列参数集字段（从比特流中提取）。这些比特流
    参数依据 :ref:`hevc` 定义，在规范第 7.4.3.2 节 "Sequence parameter set
    RBSP semantics" 中描述。)

.. c:type:: v4l2_ctrl_hevc_sps

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.2cm}|p{9.2cm}|p{6.9cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hevc_sps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``video_parameter_set_id``
      - Specifies the value of the vps_video_parameter_set_id of the active VPS
        as described in section "7.4.3.2.1 General sequence parameter set RBSP semantics"
        of H.265 specifications.
        (指定活动 VPS 的 vps_video_parameter_set_id 的值，如 H.265 规范
        "7.4.3.2.1 General sequence parameter set RBSP semantics" 一节所述。)
    * - __u8
      - ``seq_parameter_set_id``
      - Provides an identifier for the SPS for reference by other syntax elements
        as described in section "7.4.3.2.1 General sequence parameter set RBSP semantics"
        of H.265 specifications.
        (为 SPS 提供一个供其他语法元素引用的标识符，如 H.265 规范
        "7.4.3.2.1 General sequence parameter set RBSP semantics" 一节所述。)
    * - __u16
      - ``pic_width_in_luma_samples``
      - Specifies the width of each decoded picture in units of luma samples.
        (以亮度样本为单位指定每个解码图像的宽度。)
    * - __u16
      - ``pic_height_in_luma_samples``
      - Specifies the height of each decoded picture in units of luma samples.
        (以亮度样本为单位指定每个解码图像的高度。)
    * - __u8
      - ``bit_depth_luma_minus8``
      - This value plus 8 specifies the bit depth of the samples of the luma array.
        (此值加 8 指定亮度数组样本的位深。)
    * - __u8
      - ``bit_depth_chroma_minus8``
      - This value plus 8 specifies the bit depth of the samples of the chroma arrays.
        (此值加 8 指定色度数组样本的位深。)
    * - __u8
      - ``log2_max_pic_order_cnt_lsb_minus4``
      - Specifies the value of the variable MaxPicOrderCntLsb.
        (指定变量 MaxPicOrderCntLsb 的值。)
    * - __u8
      - ``sps_max_dec_pic_buffering_minus1``
      - This value plus 1 specifies the maximum required size of the decoded picture buffer for
        the coded video sequence (CVS).
        (此值加 1 指定编码视频序列 (CVS) 所需的解码图像缓冲区的最大尺寸。)
    * - __u8
      - ``sps_max_num_reorder_pics``
      - Indicates the maximum allowed number of pictures.
        (指示允许的最大图像数量。)
    * - __u8
      - ``sps_max_latency_increase_plus1``
      - Used to signal MaxLatencyPictures, which indicates the maximum number of
        pictures that can precede any picture in output order and follow that
        picture in decoding order.
        (用于发出 MaxLatencyPictures 信号，它表示在输出顺序上先于某图像、而在
        解码顺序上后于该图像的最大图像数量。)
    * - __u8
      - ``log2_min_luma_coding_block_size_minus3``
      - This value plus 3 specifies the minimum luma coding block size.
        (此值加 3 指定最小亮度编码块尺寸。)
    * - __u8
      - ``log2_diff_max_min_luma_coding_block_size``
      - Specifies the difference between the maximum and minimum luma coding block size.
        (指定最大与最小亮度编码块尺寸之间的差值。)
    * - __u8
      - ``log2_min_luma_transform_block_size_minus2``
      - This value plus 2 specifies the minimum luma transform block size.
        (此值加 2 指定最小亮度变换块尺寸。)
    * - __u8
      - ``log2_diff_max_min_luma_transform_block_size``
      - Specifies the difference between the maximum and minimum luma transform block size.
        (指定最大与最小亮度变换块尺寸之间的差值。)
    * - __u8
      - ``max_transform_hierarchy_depth_inter``
      - Specifies the maximum hierarchy depth for transform units of coding units coded
        in inter prediction mode.
        (指定以帧间预测模式编码的编码单元的变换单元的最大层次深度。)
    * - __u8
      - ``max_transform_hierarchy_depth_intra``
      - Specifies the maximum hierarchy depth for transform units of coding units coded in
        intra prediction mode.
        (指定以帧内预测模式编码的编码单元的变换单元的最大层次深度。)
    * - __u8
      - ``pcm_sample_bit_depth_luma_minus1``
      - This value plus 1 specifies the number of bits used to represent each of PCM sample values of the
        luma component.
        (此值加 1 指定用于表示亮度分量每个 PCM 样本值的位数。)
    * - __u8
      - ``pcm_sample_bit_depth_chroma_minus1``
      - Specifies the number of bits used to represent each of PCM sample values of
        the chroma components.
        (指定用于表示色度分量每个 PCM 样本值的位数。)
    * - __u8
      - ``log2_min_pcm_luma_coding_block_size_minus3``
      - Plus 3 specifies the minimum size of coding blocks.
        (加 3 指定编码块的最小尺寸。)
    * - __u8
      - ``log2_diff_max_min_pcm_luma_coding_block_size``
      - Specifies the difference between the maximum and minimum size of coding blocks.
        (指定编码块最大尺寸与最小尺寸之间的差值。)
    * - __u8
      - ``num_short_term_ref_pic_sets``
      - Specifies the number of st_ref_pic_set() syntax structures included in the SPS.
        (指定 SPS 中包含的 st_ref_pic_set() 语法结构的数量。)
    * - __u8
      - ``num_long_term_ref_pics_sps``
      - Specifies the number of candidate long-term reference pictures that are
        specified in the SPS.
        (指定 SPS 中规定的候选长期参考图片的数量。)
    * - __u8
      - ``chroma_format_idc``
      - Specifies the chroma sampling.
        (指定色度采样。)
    * - __u8
      - ``sps_max_sub_layers_minus1``
      - This value plus 1 specifies the maximum number of temporal sub-layers.
        (此值加 1 指定时间子层的最大数量。)
    * - __u64
      - ``flags``
      - See :ref:`Sequence Parameter Set Flags <hevc_sps_flags>`

.. raw:: latex

    \normalsize

.. _hevc_sps_flags:

``Sequence Parameter Set Flags`` (序列参数集标志)

.. raw:: latex

    \small

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_HEVC_SPS_FLAG_SEPARATE_COLOUR_PLANE``
      - 0x00000001
      -
    * - ``V4L2_HEVC_SPS_FLAG_SCALING_LIST_ENABLED``
      - 0x00000002
      -
    * - ``V4L2_HEVC_SPS_FLAG_AMP_ENABLED``
      - 0x00000004
      -
    * - ``V4L2_HEVC_SPS_FLAG_SAMPLE_ADAPTIVE_OFFSET``
      - 0x00000008
      -
    * - ``V4L2_HEVC_SPS_FLAG_PCM_ENABLED``
      - 0x00000010
      -
    * - ``V4L2_HEVC_SPS_FLAG_PCM_LOOP_FILTER_DISABLED``
      - 0x00000020
      -
    * - ``V4L2_HEVC_SPS_FLAG_LONG_TERM_REF_PICS_PRESENT``
      - 0x00000040
      -
    * - ``V4L2_HEVC_SPS_FLAG_SPS_TEMPORAL_MVP_ENABLED``
      - 0x00000080
      -
    * - ``V4L2_HEVC_SPS_FLAG_STRONG_INTRA_SMOOTHING_ENABLED``
      - 0x00000100
      -

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_HEVC_PPS (struct)`` (HEVC 图像参数集)
    Specifies the Picture Parameter Set fields (as extracted from the
    bitstream) for the associated HEVC slice data.
    These bitstream parameters are defined according to :ref:`hevc`.
    They are described in section 7.4.3.3 "Picture parameter set RBSP
    semantics" of the specification.
    (指定相关联 HEVC 切片数据的图像参数集字段（从比特流中提取）。这些比特流
    参数依据 :ref:`hevc` 定义，在规范第 7.4.3.3 节 "Picture parameter set
    RBSP semantics" 中描述。)

.. c:type:: v4l2_ctrl_hevc_pps

.. tabularcolumns:: |p{1.2cm}|p{8.6cm}|p{7.5cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hevc_pps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``pic_parameter_set_id``
      - Identifies the PPS for reference by other syntax elements.
        (标识 PPS，供其他语法元素引用。)
    * - __u8
      - ``num_extra_slice_header_bits``
      - Specifies the number of extra slice header bits that are present
        in the slice header RBSP for coded pictures referring to the PPS.
        (指定引用该 PPS 的编码图像在切片头 RBSP 中存在的额外切片头位数。)
    * - __u8
      - ``num_ref_idx_l0_default_active_minus1``
      - This value plus 1 specifies the inferred value of num_ref_idx_l0_active_minus1.
        (此值加 1 指定 num_ref_idx_l0_active_minus1 的推断值。)
    * - __u8
      - ``num_ref_idx_l1_default_active_minus1``
      - This value plus 1 specifies the inferred value of num_ref_idx_l1_active_minus1.
        (此值加 1 指定 num_ref_idx_l1_active_minus1 的推断值。)
    * - __s8
      - ``init_qp_minus26``
      - This value plus 26 specifies the initial value of SliceQp Y for each slice
        referring to the PPS.
        (此值加 26 指定引用该 PPS 的每个切片的 SliceQp Y 初始值。)
    * - __u8
      - ``diff_cu_qp_delta_depth``
      - Specifies the difference between the luma coding tree block size
        and the minimum luma coding block size of coding units that
        convey cu_qp_delta_abs and cu_qp_delta_sign_flag.
        (指定传递 cu_qp_delta_abs 和 cu_qp_delta_sign_flag 的编码单元的亮度
        编码树块尺寸与最小亮度编码块尺寸之间的差值。)
    * - __s8
      - ``pps_cb_qp_offset``
      - Specifies the offsets to the luma quantization parameter Cb.
        (指定亮度量化参数 Cb 的偏移量。)
    * - __s8
      - ``pps_cr_qp_offset``
      - Specifies the offsets to the luma quantization parameter Cr.
        (指定亮度量化参数 Cr 的偏移量。)
    * - __u8
      - ``num_tile_columns_minus1``
      - This value plus 1 specifies the number of tile columns partitioning the picture.
        (此值加 1 指定划分图像的 tile 列数。)
    * - __u8
      - ``num_tile_rows_minus1``
      - This value plus 1 specifies the number of tile rows partitioning the picture.
        (此值加 1 指定划分图像的 tile 行数。)
    * - __u8
      - ``column_width_minus1[20]``
      - This value plus 1 specifies the width of the i-th tile column in units of
        coding tree blocks.
        (此值加 1 指定第 i 个 tile 列以编码树块为单位的宽度。)
    * - __u8
      - ``row_height_minus1[22]``
      - This value plus 1 specifies the height of the i-th tile row in units of coding
        tree blocks.
        (此值加 1 指定第 i 个 tile 行以编码树块为单位的高度。)
    * - __s8
      - ``pps_beta_offset_div2``
      - Specifies the default deblocking parameter offsets for beta divided by 2.
        (指定 beta 的默认去块滤波参数偏移量除以 2。)
    * - __s8
      - ``pps_tc_offset_div2``
      - Specifies the default deblocking parameter offsets for tC divided by 2.
        (指定 tC 的默认去块滤波参数偏移量除以 2。)
    * - __u8
      - ``log2_parallel_merge_level_minus2``
      - This value plus 2 specifies the value of the variable Log2ParMrgLevel.
        (此值加 2 指定变量 Log2ParMrgLevel 的值。)
    * - __u8
      - ``padding[4]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - __u64
      - ``flags``
      - See :ref:`Picture Parameter Set Flags <hevc_pps_flags>`
        (参见 :ref:`Picture Parameter Set Flags <hevc_pps_flags>`。)

.. _hevc_pps_flags:

``Picture Parameter Set Flags`` (图像参数集标志)

.. raw:: latex

    \small

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_HEVC_PPS_FLAG_DEPENDENT_SLICE_SEGMENT_ENABLED``
      - 0x00000001
      -
    * - ``V4L2_HEVC_PPS_FLAG_OUTPUT_FLAG_PRESENT``
      - 0x00000002
      -
    * - ``V4L2_HEVC_PPS_FLAG_SIGN_DATA_HIDING_ENABLED``
      - 0x00000004
      -
    * - ``V4L2_HEVC_PPS_FLAG_CABAC_INIT_PRESENT``
      - 0x00000008
      -
    * - ``V4L2_HEVC_PPS_FLAG_CONSTRAINED_INTRA_PRED``
      - 0x00000010
      -
    * - ``V4L2_HEVC_PPS_FLAG_TRANSFORM_SKIP_ENABLED``
      - 0x00000020
      -
    * - ``V4L2_HEVC_PPS_FLAG_CU_QP_DELTA_ENABLED``
      - 0x00000040
      -
    * - ``V4L2_HEVC_PPS_FLAG_PPS_SLICE_CHROMA_QP_OFFSETS_PRESENT``
      - 0x00000080
      -
    * - ``V4L2_HEVC_PPS_FLAG_WEIGHTED_PRED``
      - 0x00000100
      -
    * - ``V4L2_HEVC_PPS_FLAG_WEIGHTED_BIPRED``
      - 0x00000200
      -
    * - ``V4L2_HEVC_PPS_FLAG_TRANSQUANT_BYPASS_ENABLED``
      - 0x00000400
      -
    * - ``V4L2_HEVC_PPS_FLAG_TILES_ENABLED``
      - 0x00000800
      -
    * - ``V4L2_HEVC_PPS_FLAG_ENTROPY_CODING_SYNC_ENABLED``
      - 0x00001000
      -
    * - ``V4L2_HEVC_PPS_FLAG_LOOP_FILTER_ACROSS_TILES_ENABLED``
      - 0x00002000
      -
    * - ``V4L2_HEVC_PPS_FLAG_PPS_LOOP_FILTER_ACROSS_SLICES_ENABLED``
      - 0x00004000
      -
    * - ``V4L2_HEVC_PPS_FLAG_DEBLOCKING_FILTER_OVERRIDE_ENABLED``
      - 0x00008000
      -
    * - ``V4L2_HEVC_PPS_FLAG_PPS_DISABLE_DEBLOCKING_FILTER``
      - 0x00010000
      -
    * - ``V4L2_HEVC_PPS_FLAG_LISTS_MODIFICATION_PRESENT``
      - 0x00020000
      -
    * - ``V4L2_HEVC_PPS_FLAG_SLICE_SEGMENT_HEADER_EXTENSION_PRESENT``
      - 0x00040000
      -
    * - ``V4L2_HEVC_PPS_FLAG_DEBLOCKING_FILTER_CONTROL_PRESENT``
      - 0x00080000
      - Specifies the presence of deblocking filter control syntax elements in
        the PPS
        (指定 PPS 中存在去块滤波控制语法元素)
    * - ``V4L2_HEVC_PPS_FLAG_UNIFORM_SPACING``
      - 0x00100000
      - Specifies that tile column boundaries and likewise tile row boundaries
        are distributed uniformly across the picture
        (指定 tile 列边界以及同样地 tile 行边界在图像上均匀分布)

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_HEVC_SLICE_PARAMS (struct)`` (HEVC 切片参数)
    Specifies various slice-specific parameters, especially from the NAL unit
    header, general slice segment header and weighted prediction parameter
    parts of the bitstream.
    These bitstream parameters are defined according to :ref:`hevc`.
    They are described in section 7.4.7 "General slice segment header
    semantics" of the specification.
    This control is a dynamically sized 1-dimensional array,
    V4L2_CTRL_FLAG_DYNAMIC_ARRAY flag must be set when using it.
    (指定各种切片专属参数，尤其是比特流中 NAL 单元头、通用切片段头和加权预测
    参数部分的内容。这些比特流参数依据 :ref:`hevc` 定义，在规范第 7.4.7 节
    "General slice segment header semantics" 中描述。此控制是动态大小的
    一维数组，使用时必须设置 V4L2_CTRL_FLAG_DYNAMIC_ARRAY 标志。)

.. c:type:: v4l2_ctrl_hevc_slice_params

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{5.4cm}|p{6.8cm}|p{5.1cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hevc_slice_params
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``bit_size``
      - Size (in bits) of the current slice data.
        (当前切片数据的大小（以位为单位）。)
    * - __u32
      - ``data_byte_offset``
      - Offset (in byte) to the video data in the current slice data.
        (当前切片数据中视频数据的偏移量（以字节为单位）。)
    * - __u32
      - ``num_entry_point_offsets``
      - Specifies the number of entry point offset syntax elements in the slice header.
        When the driver supports it, the ``V4L2_CID_STATELESS_HEVC_ENTRY_POINT_OFFSETS``
        must be set.
        (指定切片头中入口点偏移语法元素的数量。当驱动程序支持时，必须设置
        ``V4L2_CID_STATELESS_HEVC_ENTRY_POINT_OFFSETS``。)
    * - __u8
      - ``nal_unit_type``
      - Specifies the coding type of the slice (B, P or I).
        (指定切片的编码类型 (B、P 或 I)。)
    * - __u8
      - ``nuh_temporal_id_plus1``
      - Minus 1 specifies a temporal identifier for the NAL unit.
        (减 1 指定 NAL 单元的时间标识符。)
    * - __u8
      - ``slice_type``
      -
	(V4L2_HEVC_SLICE_TYPE_I, V4L2_HEVC_SLICE_TYPE_P or
	V4L2_HEVC_SLICE_TYPE_B).
	(V4L2_HEVC_SLICE_TYPE_I、V4L2_HEVC_SLICE_TYPE_P 或
        V4L2_HEVC_SLICE_TYPE_B。)
    * - __u8
      - ``colour_plane_id``
      - Specifies the colour plane associated with the current slice.
        (指定与当前切片相关联的颜色平面。)
    * - __s32
      - ``slice_pic_order_cnt``
      - Specifies the picture order count.
        (指定图像顺序计数。)
    * - __u8
      - ``num_ref_idx_l0_active_minus1``
      - This value plus 1 specifies the maximum reference index for reference picture list 0
        that may be used to decode the slice.
        (此值加 1 指定可用于解码该切片的参考图像列表 0 的最大参考索引。)
    * - __u8
      - ``num_ref_idx_l1_active_minus1``
      - This value plus 1 specifies the maximum reference index for reference picture list 1
        that may be used to decode the slice.
        (此值加 1 指定可用于解码该切片的参考图像列表 1 的最大参考索引。)
    * - __u8
      - ``collocated_ref_idx``
      - Specifies the reference index of the collocated picture used for
        temporal motion vector prediction.
        (指定用于时域运动矢量预测的同位图像的参考索引。)
    * - __u8
      - ``five_minus_max_num_merge_cand``
      - Specifies the maximum number of merging motion vector prediction
        candidates supported in the slice subtracted from 5.
        (指定该切片支持的最大合并运动矢量预测候选数被 5 减去后的值。)
    * - __s8
      - ``slice_qp_delta``
      - Specifies the initial value of QpY to be used for the coding blocks in the slice.
        (指定该切片中编码块要使用的 QpY 初始值。)
    * - __s8
      - ``slice_cb_qp_offset``
      - Specifies a difference to be added to the value of pps_cb_qp_offset.
        (指定要加到 pps_cb_qp_offset 值上的差值。)
    * - __s8
      - ``slice_cr_qp_offset``
      - Specifies a difference to be added to the value of pps_cr_qp_offset.
        (指定要加到 pps_cr_qp_offset 值上的差值。)
    * - __s8
      - ``slice_act_y_qp_offset``
      - Specifies the offset to the luma of quantization parameter qP derived in section 8.6.2
        (指定第 8.6.2 节推导出的量化参数 qP 的亮度偏移量)
    * - __s8
      - ``slice_act_cb_qp_offset``
      - Specifies the offset to the cb of quantization parameter qP derived in section 8.6.2
        (指定第 8.6.2 节推导出的量化参数 qP 的 cb 偏移量)
    * - __s8
      - ``slice_act_cr_qp_offset``
      - Specifies the offset to the cr of quantization parameter qP derived in section 8.6.2
        (指定第 8.6.2 节推导出的量化参数 qP 的 cr 偏移量)
    * - __s8
      - ``slice_beta_offset_div2``
      - Specifies the deblocking parameter offsets for beta divided by 2.
        (指定 beta 的去块滤波参数偏移量除以 2。)
    * - __s8
      - ``slice_tc_offset_div2``
      - Specifies the deblocking parameter offsets for tC divided by 2.
        (指定 tC 的去块滤波参数偏移量除以 2。)
    * - __u8
      - ``pic_struct``
      - Indicates whether a picture should be displayed as a frame or as one or more fields.
        (指示图像应显示为帧还是一个或多个场。)
    * - __u32
      - ``slice_segment_addr``
      - Specifies the address of the first coding tree block in the slice segment.
        (指定切片段中第一个编码树块的地址。)
    * - __u8
      - ``ref_idx_l0[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - The list of L0 reference elements as indices in the DPB.
        (以 DPB 中索引表示的 L0 参考元素列表。)
    * - __u8
      - ``ref_idx_l1[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - The list of L1 reference elements as indices in the DPB.
        (以 DPB 中索引表示的 L1 参考元素列表。)
    * - __u16
      - ``short_term_ref_pic_set_size``
      - Specifies the size, in bits, of the short-term reference picture set, described as st_ref_pic_set()
        in the specification, included in the slice header or SPS (section 7.3.6.1).
        (指定包含在切片头或 SPS 中的短期参考图像集（规范中记作
        st_ref_pic_set()）的大小（以位为单位），见 7.3.6.1 节。)
    * - __u16
      - ``long_term_ref_pic_set_size``
      - Specifies the size, in bits, of the long-term reference picture set include in the slice header
        or SPS. It is the number of bits in the conditional block if(long_term_ref_pics_present_flag)
        in section 7.3.6.1 of the specification.
        (指定包含在切片头或 SPS 中的长期参考图像集的大小（以位为单位）。即规范
        第 7.3.6.1 节中条件块 if(long_term_ref_pics_present_flag) 的位数。)
    * - __u8
      - ``padding``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - struct :c:type:`v4l2_hevc_pred_weight_table`
      - ``pred_weight_table``
      - The prediction weight coefficients for inter-picture prediction.
        (图像间预测的预测权重系数。)
    * - __u64
      - ``flags``
      - See :ref:`Slice Parameters Flags <hevc_slice_params_flags>`

.. raw:: latex

    \normalsize

.. _hevc_slice_params_flags:

``Slice Parameters Flags`` (切片参数标志)

.. raw:: latex

    \scriptsize

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_SAO_LUMA``
      - 0x00000001
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_SAO_CHROMA``
      - 0x00000002
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_TEMPORAL_MVP_ENABLED``
      - 0x00000004
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_MVD_L1_ZERO``
      - 0x00000008
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_CABAC_INIT``
      - 0x00000010
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_COLLOCATED_FROM_L0``
      - 0x00000020
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_USE_INTEGER_MV``
      - 0x00000040
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_DEBLOCKING_FILTER_DISABLED``
      - 0x00000080
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_SLICE_LOOP_FILTER_ACROSS_SLICES_ENABLED``
      - 0x00000100
      -
    * - ``V4L2_HEVC_SLICE_PARAMS_FLAG_DEPENDENT_SLICE_SEGMENT``
      - 0x00000200
      -

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_HEVC_ENTRY_POINT_OFFSETS (integer)`` (HEVC 入口点偏移)
    Specifies entry point offsets in bytes.
    This control is a dynamically sized array. The number of entry point
    offsets is reported by the ``elems`` field.
    This bitstream parameter is defined according to :ref:`hevc`.
    They are described in section 7.4.7.1 "General slice segment header
    semantics" of the specification.
    When multiple slices are submitted in a request, the length of
    this array must be the sum of num_entry_point_offsets of all the
    slices in the request.
    (以字节为单位指定入口点偏移。此控制是动态大小的数组，入口点偏移的数量由
    ``elems`` 字段报告。此比特流参数依据 :ref:`hevc` 定义，在规范第 7.4.7.1 节
    "General slice segment header semantics" 中描述。当请求中提交多个切片时，
    此数组的长度必须是请求中所有切片的 num_entry_point_offsets 之和。)

``V4L2_CID_STATELESS_HEVC_SCALING_MATRIX (struct)`` (HEVC 缩放矩阵)
    Specifies the HEVC scaling matrix parameters used for the scaling process
    for transform coefficients.
    These matrix and parameters are defined according to :ref:`hevc`.
    They are described in section 7.4.5 "Scaling list data semantics" of
    the specification.
    (指定用于变换系数缩放过程的 HEVC 缩放矩阵参数。这些矩阵和参数依据
    :ref:`hevc` 定义，在规范第 7.4.5 节 "Scaling list data semantics" 中描述。)

.. c:type:: v4l2_ctrl_hevc_scaling_matrix

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{5.4cm}|p{6.8cm}|p{5.1cm}|

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hevc_scaling_matrix
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``scaling_list_4x4[6][16]``
      - Scaling list is used for the scaling process for transform
        coefficients. The values on each scaling list are expected
        in raster scan order.
        (缩放列表用于变换系数的缩放过程。每个缩放列表中的值按光栅扫描顺序给出。)
    * - __u8
      - ``scaling_list_8x8[6][64]``
      - Scaling list is used for the scaling process for transform
        coefficients. The values on each scaling list are expected
        in raster scan order.
        (缩放列表用于变换系数的缩放过程。每个缩放列表中的值按光栅扫描顺序给出。)
    * - __u8
      - ``scaling_list_16x16[6][64]``
      - Scaling list is used for the scaling process for transform
        coefficients. The values on each scaling list are expected
        in raster scan order.
        (缩放列表用于变换系数的缩放过程。每个缩放列表中的值按光栅扫描顺序给出。)
    * - __u8
      - ``scaling_list_32x32[2][64]``
      - Scaling list is used for the scaling process for transform
        coefficients. The values on each scaling list are expected
        in raster scan order.
        (缩放列表用于变换系数的缩放过程。每个缩放列表中的值按光栅扫描顺序给出。)
    * - __u8
      - ``scaling_list_dc_coef_16x16[6]``
      - Scaling list is used for the scaling process for transform
        coefficients. The values on each scaling list are expected
        in raster scan order.
        (缩放列表用于变换系数的缩放过程。每个缩放列表中的值按光栅扫描顺序给出。)
    * - __u8
      - ``scaling_list_dc_coef_32x32[2]``
      - Scaling list is used for the scaling process for transform
        coefficients. The values on each scaling list are expected
        in raster scan order.
        (缩放列表用于变换系数的缩放过程。每个缩放列表中的值按光栅扫描顺序给出。)

.. raw:: latex

    \normalsize

.. c:type:: v4l2_hevc_dpb_entry

.. raw:: latex

    \small

.. tabularcolumns:: |p{1.0cm}|p{4.2cm}|p{12.1cm}|

.. flat-table:: struct v4l2_hevc_dpb_entry
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u64
      - ``timestamp``
      - Timestamp of the V4L2 capture buffer to use as reference, used
        with B-coded and P-coded frames. The timestamp refers to the
	``timestamp`` field in struct :c:type:`v4l2_buffer`. Use the
	:c:func:`v4l2_timeval_to_ns()` function to convert the struct
	:c:type:`timeval` in struct :c:type:`v4l2_buffer` to a __u64.
        (用作参考的 V4L2 捕获缓冲区的时间戳，用于 B 编码和 P 编码帧。该时间戳
        指的是 ``timestamp`` 字段。使用 :c:func:`v4l2_timeval_to_ns()` 函数把
        struct :c:type:`v4l2_buffer` 中的 struct :c:type:`timeval` 转换为
        __u64。)
    * - __u8
      - ``flags``
      - Long term flag for the reference frame
        (V4L2_HEVC_DPB_ENTRY_LONG_TERM_REFERENCE). The flag is set as
        described in the ITU HEVC specification chapter "8.3.2 Decoding
        process for reference picture set".
        (参考帧的长期标志 (V4L2_HEVC_DPB_ENTRY_LONG_TERM_REFERENCE)。该标志按
        ITU HEVC 规范 "8.3.2 Decoding process for reference picture set" 一章
        所述设置。)
    * - __u8
      - ``field_pic``
      - Whether the reference is a field picture or a frame.
        See :ref:`HEVC dpb field pic Flags <hevc_dpb_field_pic_flags>`
        (参考是场图像还是帧。参见 :ref:`HEVC dpb field pic Flags
        <hevc_dpb_field_pic_flags>`。)
    * - __s32
      - ``pic_order_cnt_val``
      - The picture order count of the current picture.
        (当前图像的图像顺序计数。)
    * - __u8
      - ``padding[2]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. raw:: latex

    \normalsize

.. _hevc_dpb_field_pic_flags:

``HEVC dpb field pic Flags`` (HEVC DPB 场图像标志)

.. raw:: latex

    \scriptsize

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_HEVC_SEI_PIC_STRUCT_FRAME``
      - 0
      - (progressive) Frame
        (逐行 (progressive) 帧)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_TOP_FIELD``
      - 1
      - Top field
        (顶场)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_BOTTOM_FIELD``
      - 2
      - Bottom field
        (底场)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_TOP_BOTTOM``
      - 3
      - Top field, bottom field, in that order
        (顶场、底场，按此顺序)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_BOTTOM_TOP``
      - 4
      - Bottom field, top field, in that order
        (底场、顶场，按此顺序)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_TOP_BOTTOM_TOP``
      - 5
      - Top field, bottom field, top field repeated, in that order
        (顶场、底场、重复的顶场，按此顺序)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM``
      - 6
      - Bottom field, top field, bottom field repeated, in that order
        (底场、顶场、重复的底场，按此顺序)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_FRAME_DOUBLING``
      - 7
      - Frame doubling
        (帧加倍)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_FRAME_TRIPLING``
      - 8
      - Frame tripling
        (帧三倍)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_TOP_PAIRED_PREVIOUS_BOTTOM``
      - 9
      - Top field paired with previous bottom field in output order
        (顶场与输出顺序中前一个底场配对)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_BOTTOM_PAIRED_PREVIOUS_TOP``
      - 10
      - Bottom field paired with previous top field in output order
        (底场与输出顺序中前一个顶场配对)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_TOP_PAIRED_NEXT_BOTTOM``
      - 11
      - Top field paired with next bottom field in output order
        (顶场与输出顺序中后一个底场配对)
    * - ``V4L2_HEVC_SEI_PIC_STRUCT_BOTTOM_PAIRED_NEXT_TOP``
      - 12
      - Bottom field paired with next top field in output order
        (底场与输出顺序中后一个顶场配对)

.. c:type:: v4l2_hevc_pred_weight_table

.. raw:: latex

    \footnotesize

.. tabularcolumns:: |p{0.8cm}|p{10.6cm}|p{5.9cm}|

.. flat-table:: struct v4l2_hevc_pred_weight_table
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s8
      - ``delta_luma_weight_l0[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - The difference of the weighting factor applied to the luma
        prediction value for list 0.
        (应用于列表 0 亮度预测值的加权因子之差。)
    * - __s8
      - ``luma_offset_l0[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - The additive offset applied to the luma prediction value for list 0.
        (应用于列表 0 亮度预测值的加性偏移。)
    * - __s8
      - ``delta_chroma_weight_l0[V4L2_HEVC_DPB_ENTRIES_NUM_MAX][2]``
      - The difference of the weighting factor applied to the chroma
        prediction value for list 0.
        (应用于列表 0 色度预测值的加权因子之差。)
    * - __s8
      - ``chroma_offset_l0[V4L2_HEVC_DPB_ENTRIES_NUM_MAX][2]``
      - The difference of the additive offset applied to the chroma
        prediction values for list 0.
        (应用于列表 0 色度预测值的加性偏移之差。)
    * - __s8
      - ``delta_luma_weight_l1[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - The difference of the weighting factor applied to the luma
        prediction value for list 1.
        (应用于列表 1 亮度预测值的加权因子之差。)
    * - __s8
      - ``luma_offset_l1[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - The additive offset applied to the luma prediction value for list 1.
        (应用于列表 1 亮度预测值的加性偏移。)
    * - __s8
      - ``delta_chroma_weight_l1[V4L2_HEVC_DPB_ENTRIES_NUM_MAX][2]``
      - The difference of the weighting factor applied to the chroma
        prediction value for list 1.
        (应用于列表 1 色度预测值的加权因子之差。)
    * - __s8
      - ``chroma_offset_l1[V4L2_HEVC_DPB_ENTRIES_NUM_MAX][2]``
      - The difference of the additive offset applied to the chroma
        prediction values for list 1.
        (应用于列表 1 色度预测值的加性偏移之差。)
    * - __u8
      - ``luma_log2_weight_denom``
      - The base 2 logarithm of the denominator for all luma weighting
        factors.
        (所有亮度加权因子分母以 2 为底的对数。)
    * - __s8
      - ``delta_chroma_log2_weight_denom``
      - The difference of the base 2 logarithm of the denominator for
        all chroma weighting factors.
        (所有色度加权因子分母以 2 为底的对数之差。)
    * - __u8
      - ``padding[6]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_HEVC_DECODE_MODE (enum)`` (HEVC 解码模式)
    Specifies the decoding mode to use. Currently exposes slice-based and
    frame-based decoding but new modes might be added later on.
    This control is used as a modifier for V4L2_PIX_FMT_HEVC_SLICE
    pixel format. Applications that support V4L2_PIX_FMT_HEVC_SLICE
    are required to set this control in order to specify the decoding mode
    that is expected for the buffer.
    Drivers may expose a single or multiple decoding modes, depending
    on what they can support.
    (指定要使用的解码模式。当前提供基于切片和基于帧的解码，但以后可能会新增
    其他模式。该控制用作 V4L2_PIX_FMT_HEVC_SLICE 像素格式的修饰符。支持
    V4L2_PIX_FMT_HEVC_SLICE 的应用程序必须设置此控制，以指明缓冲区预期的
    解码模式。驱动程序可根据自身支持的能力暴露单个或多个解码模式。)

.. c:type:: v4l2_stateless_hevc_decode_mode

.. raw:: latex

    \small

.. tabularcolumns:: |p{9.4cm}|p{0.6cm}|p{7.3cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_STATELESS_HEVC_DECODE_MODE_SLICE_BASED``
      - 0
      - Decoding is done at the slice granularity.
        The OUTPUT buffer must contain a single slice.
        (以切片为粒度进行解码。OUTPUT 缓冲区必须包含单个切片。)
    * - ``V4L2_STATELESS_HEVC_DECODE_MODE_FRAME_BASED``
      - 1
      - Decoding is done at the frame granularity.
        The OUTPUT buffer must contain all slices needed to decode the
        frame.
        (以帧为粒度进行解码。OUTPUT 缓冲区必须包含解码该帧所需的全部切片。)

.. raw:: latex

    \normalsize

``V4L2_CID_STATELESS_HEVC_START_CODE (enum)`` (HEVC 起始码)
    Specifies the HEVC slice start code expected for each slice.
    This control is used as a modifier for V4L2_PIX_FMT_HEVC_SLICE
    pixel format. Applications that support V4L2_PIX_FMT_HEVC_SLICE
    are required to set this control in order to specify the start code
    that is expected for the buffer.
    Drivers may expose a single or multiple start codes, depending
    on what they can support.
    (指定每个 HEVC 切片预期的起始码。该控制用作 V4L2_PIX_FMT_HEVC_SLICE
    像素格式的修饰符。支持 V4L2_PIX_FMT_HEVC_SLICE 的应用程序必须设置此控制，
    以指明缓冲区预期的起始码。驱动程序可根据自身支持的能力暴露单个或多个
    起始码。)

.. c:type:: v4l2_stateless_hevc_start_code

.. tabularcolumns:: |p{9.2cm}|p{0.6cm}|p{7.5cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_STATELESS_HEVC_START_CODE_NONE``
      - 0
      - Selecting this value specifies that HEVC slices are passed
        to the driver without any start code. The bitstream data should be
        according to :ref:`hevc` 7.3.1.1 General NAL unit syntax, hence
        contains emulation prevention bytes when required.
        (选择此值表示传递给驱动程序的 HEVC 切片不带任何起始码。比特流数据应符合
        :ref:`hevc` 7.3.1.1 General NAL unit syntax，因而在需要时包含仿真预防
        字节。)
    * - ``V4L2_STATELESS_HEVC_START_CODE_ANNEX_B``
      - 1
      - Selecting this value specifies that HEVC slices are expected
        to be prefixed by Annex B start codes. According to :ref:`hevc`
        valid start codes can be 3-bytes 0x000001 or 4-bytes 0x00000001.
        (选择此值表示 HEVC 切片预期以 Annex B 起始码为前缀。依据 :ref:`hevc`，
        有效的起始码可以是 3 字节的 0x000001 或 4 字节的 0x00000001。)

.. raw:: latex

    \normalsize

``V4L2_CID_MPEG_VIDEO_BASELAYER_PRIORITY_ID (integer)`` (基础层优先级 ID)
    Specifies a priority identifier for the NAL unit, which will be applied to
    the base layer. By default this value is set to 0 for the base layer,
    and the next layer will have the priority ID assigned as 1, 2, 3 and so on.
    The video encoder can't decide the priority id to be applied to a layer,
    so this has to come from client.
    This is applicable to H264 and valid Range is from 0 to 63.
    Source Rec. ITU-T H.264 (06/2019); G.7.4.1.1, G.8.8.1.
    (为 NAL 单元指定优先级标识符，该标识符将应用于基础层。默认情况下，基础层
    的此值设为 0，下一层的优先级 ID 依次指定为 1、2、3 等等。视频编码器无法
    决定应用于某一层的优先级 ID，因此必须由客户端提供。适用于 H264，有效范围
    为 0 到 63。来源：Rec. ITU-T H.264 (06/2019); G.7.4.1.1, G.8.8.1。)

``V4L2_CID_MPEG_VIDEO_LTR_COUNT (integer)`` (LTR 帧数量)
    Specifies the maximum number of Long Term Reference (LTR) frames at any
    given time that the encoder can keep.
    This is applicable to the H264 and HEVC encoders.
    (指定编码器在任一时刻可保留的长期参考 (LTR) 帧的最大数量。适用于 H264 和
    HEVC 编码器。)

``V4L2_CID_MPEG_VIDEO_FRAME_LTR_INDEX (integer)`` (帧 LTR 索引)
    After setting this control the frame that will be queued next
    will be marked as a Long Term Reference (LTR) frame
    and given this LTR index which ranges from 0 to LTR_COUNT-1.
    This is applicable to the H264 and HEVC encoders.
    Source Rec. ITU-T H.264 (06/2019); Table 7.9
    (设置此控制后，接下来入队的帧将被标记为长期参考 (LTR) 帧，并被赋予此
    LTR 索引，取值范围为 0 到 LTR_COUNT-1。适用于 H264 和 HEVC 编码器。
    来源：Rec. ITU-T H.264 (06/2019); Table 7.9。)

``V4L2_CID_MPEG_VIDEO_USE_LTR_FRAMES (bitmask)`` (使用 LTR 帧)
    Specifies the Long Term Reference (LTR) frame(s) to be used for
    encoding the next frame queued after setting this control.
    This provides a bitmask which consists of bits [0, LTR_COUNT-1].
    This is applicable to the H264 and HEVC encoders.
    (指定用于编码设置此控制后下一个入队帧的长期参考 (LTR) 帧。它提供一个由
    位 [0, LTR_COUNT-1] 组成的位掩码。适用于 H264 和 HEVC 编码器。)

``V4L2_CID_STATELESS_HEVC_DECODE_PARAMS (struct)`` (HEVC 解码参数)
    Specifies various decode parameters, especially the references picture order
    count (POC) for all the lists (short, long, before, current, after) and the
    number of entries for each of them.
    These parameters are defined according to :ref:`hevc`.
    They are described in section 8.3 "Slice decoding process" of the
    specification.
    (指定各种解码参数，尤其是所有列表（short、long、before、current、after）
    的参考图像顺序计数 (POC) 以及各自的数量。这些参数依据 :ref:`hevc` 定义，
    在规范第 8.3 节 "Slice decoding process" 中描述。)

.. c:type:: v4l2_ctrl_hevc_decode_params

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hevc_decode_params
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __s32
      - ``pic_order_cnt_val``
      - PicOrderCntVal as described in section 8.3.1 "Decoding process
        for picture order count" of the specification.
        (PicOrderCntVal，如规范第 8.3.1 节 "Decoding process for picture
        order count" 所述。)
    * - __u16
      - ``short_term_ref_pic_set_size``
      - Specifies the size, in bits, of the short-term reference picture set, of the first slice
        described as st_ref_pic_set() in the specification, included in the slice header
        or SPS (section 7.3.6.1).
        (指定第一个切片包含在切片头或 SPS 中的短期参考图像集（规范中记作
        st_ref_pic_set()）的大小（以位为单位）(7.3.6.1 节)。)
    * - __u16
      - ``long_term_ref_pic_set_size``
      - Specifies the size, in bits, of the long-term reference picture set, of the first slice
        included in the slice header or SPS. It is the number of bits in the conditional block
        if(long_term_ref_pics_present_flag) in section 7.3.6.1 of the specification.
        (指定第一个切片包含在切片头或 SPS 中的长期参考图像集的大小（以位为
        单位）。即规范第 7.3.6.1 节中条件块 if(long_term_ref_pics_present_flag)
        的位数。)
    * - __u8
      - ``num_active_dpb_entries``
      - The number of entries in ``dpb``.
        (``dpb`` 中的表项数量。)
    * - __u8
      - ``num_poc_st_curr_before``
      - The number of reference pictures in the short-term set that come before
        the current frame.
        (短期集合中位于当前帧之前的参考图片数量。)
    * - __u8
      - ``num_poc_st_curr_after``
      - The number of reference pictures in the short-term set that come after
        the current frame.
        (短期集合中位于当前帧之后的参考图片数量。)
    * - __u8
      - ``num_poc_lt_curr``
      - The number of reference pictures in the long-term set.
        (长期集合中的参考图片数量。)
    * - __u8
      - ``poc_st_curr_before[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - PocStCurrBefore as described in section 8.3.2 "Decoding process for reference
        picture set": provides the index of the short term before references in DPB array.
        (PocStCurrBefore，如规范第 8.3.2 节 "Decoding process for reference
        picture set" 所述：提供 DPB 数组中短期前向参考的索引。)
    * - __u8
      - ``poc_st_curr_after[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - PocStCurrAfter as described in section 8.3.2 "Decoding process for reference
        picture set": provides the index of the short term after references in DPB array.
        (PocStCurrAfter，如规范第 8.3.2 节 "Decoding process for reference
        picture set" 所述：提供 DPB 数组中短期后向参考的索引。)
    * - __u8
      - ``poc_lt_curr[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - PocLtCurr as described in section 8.3.2 "Decoding process for reference
        picture set": provides the index of the long term references in DPB array.
        (PocLtCurr，如规范第 8.3.2 节 "Decoding process for reference
        picture set" 所述：提供 DPB 数组中长期参考的索引。)
    * - __u8
      - ``num_delta_pocs_of_ref_rps_idx``
      - When the short_term_ref_pic_set_sps_flag in the slice header is equal to 0,
        it is the same as the derived value NumDeltaPocs[RefRpsIdx]. It can be used to parse
        the RPS data in slice headers instead of skipping it with @short_term_ref_pic_set_size.
        When the value of short_term_ref_pic_set_sps_flag in the slice header is
        equal to 1, num_delta_pocs_of_ref_rps_idx shall be set to 0.
        (当切片头中的 short_term_ref_pic_set_sps_flag 等于 0 时，它等同于推导值
        NumDeltaPocs[RefRpsIdx]。它可用于解析切片头中的 RPS 数据，而不是用
        @short_term_ref_pic_set_size 跳过它。当切片头中的
        short_term_ref_pic_set_sps_flag 等于 1 时，
        num_delta_pocs_of_ref_rps_idx 应设为 0。)
    * - struct :c:type:`v4l2_hevc_dpb_entry`
      - ``dpb[V4L2_HEVC_DPB_ENTRIES_NUM_MAX]``
      - The decoded picture buffer, for meta-data about reference frames.
        (解码图像缓冲区，存放参考帧的元数据。)
    * - __u64
      - ``flags``
      - See :ref:`Decode Parameters Flags <hevc_decode_params_flags>`
        (参见 :ref:`Decode Parameters Flags <hevc_decode_params_flags>`。)

.. _hevc_decode_params_flags:

``Decode Parameters Flags`` (解码参数标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_HEVC_DECODE_PARAM_FLAG_IRAP_PIC``
      - 0x00000001
      -
    * - ``V4L2_HEVC_DECODE_PARAM_FLAG_IDR_PIC``
      - 0x00000002
      -
    * - ``V4L2_HEVC_DECODE_PARAM_FLAG_NO_OUTPUT_OF_PRIOR``
      - 0x00000004
      -

``V4L2_CID_STATELESS_HEVC_EXT_SPS_LT_RPS (struct)`` (HEVC 扩展 SPS 长期 RPS)
    Subset of the :c:type:`v4l2_ctrl_hevc_sps` control.
    It extends it with the list of Long-term reference sets parameters.
    These parameters are defined according to :ref:`hevc`.
    They are described in section 7.4.3.2.1 "General sequence parameter set
    RBSP semantics" of the specification.
    This control is a dynamically sized 1-dimensional array.
    The values in the array should be ignored when either
    num_long_term_ref_pics_sps is 0 or the
    V4L2_HEVC_SPS_FLAG_LONG_TERM_REF_PICS_PRESENT flag is not set in
    :c:type:`v4l2_ctrl_hevc_sps`.
    (:c:type:`v4l2_ctrl_hevc_sps` 控制的子集。它以长期参考集参数列表对该控制
    进行扩展。这些参数依据 :ref:`hevc` 定义，在规范第 7.4.3.2.1 节 "General
    sequence parameter set RBSP semantics" 中描述。此控制是动态大小的一维
    数组。当 num_long_term_ref_pics_sps 为 0，或 :c:type:`v4l2_ctrl_hevc_sps`
    中未设置 V4L2_HEVC_SPS_FLAG_LONG_TERM_REF_PICS_PRESENT 标志时，应忽略
    数组中的值。)

.. c:type:: v4l2_ctrl_hevc_ext_sps_lt_rps

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hevc_ext_sps_lt_rps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u16
      - ``lt_ref_pic_poc_lsb_sps``
      - Long term reference picture order count as described in section 7.4.3.2.1
        "General sequence parameter set RBSP semantics" of the specification.
        (长期参考图像顺序计数，如规范第 7.4.3.2.1 节 "General sequence
        parameter set RBSP semantics" 所述。)
    * - __u16
      - ``flags``
      - See :ref:`Extended Long-Term RPS Flags <hevc_ext_sps_lt_rps_flags>`
        (参见 :ref:`Extended Long-Term RPS Flags <hevc_ext_sps_lt_rps_flags>`。)

.. _hevc_ext_sps_lt_rps_flags:

``Extended SPS Long-Term RPS Flags`` (扩展 SPS 长期 RPS 标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_HEVC_EXT_SPS_LT_RPS_FLAG_USED_LT``
      - 0x00000001
      - Specifies if the long-term reference picture is used 7.4.3.2.1 "General sequence parameter
        set RBSP semantics" of the specification.
        (指定是否使用长期参考图片，见规范 7.4.3.2.1 节 "General sequence
        parameter set RBSP semantics"。)

``V4L2_CID_STATELESS_HEVC_EXT_SPS_ST_RPS (struct)`` (HEVC 扩展 SPS 短期 RPS)
    Subset of the :c:type:`v4l2_ctrl_hevc_sps` control.
    It extends it with the list of Short-term reference sets parameters.
    These parameters are defined according to :ref:`hevc`.
    They are described in section 7.4.8 "Short-term reference picture set
    semantics" of the specification.
    This control is a dynamically sized 1-dimensional array.
    The values in the array should be ignored when
    num_short_term_ref_pic_sets is 0.
    (:c:type:`v4l2_ctrl_hevc_sps` 控制的子集。它以短期参考集参数列表对该控制
    进行扩展。这些参数依据 :ref:`hevc` 定义，在规范第 7.4.8 节 "Short-term
    reference picture set semantics" 中描述。此控制是动态大小的一维数组。当
    num_short_term_ref_pic_sets 为 0 时，应忽略数组中的值。)

.. c:type:: v4l2_ctrl_hevc_ext_sps_st_rps

.. cssclass:: longtable

.. flat-table:: struct v4l2_ctrl_hevc_ext_sps_st_rps
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``delta_idx_minus1``
      - Specifies the delta compare to the index. See details in section 7.4.8 "Short-term
        reference picture set semantics" of the specification.
        (指定相对于该索引的增量。细节见规范第 7.4.8 节 "Short-term reference
        picture set semantics"。)
    * - __u8
      - ``delta_rps_sign``
      - Sign of the delta as specified in section 7.4.8 "Short-term reference picture set
        semantics" of the specification.
        (增量符号，如规范第 7.4.8 节 "Short-term reference picture set
        semantics" 所规定。)
    * - __u8
      - ``num_negative_pics``
      - Number of short-term RPS entries that have picture order count values less than the
        picture order count value of the current picture.
        (图像顺序计数值小于当前图像图像顺序计数值的短期 RPS 表项数量。)
    * - __u8
      - ``num_positive_pics``
      - Number of short-term RPS entries that have picture order count values greater than the
        picture order count value of the current picture.
        (图像顺序计数值大于当前图像图像顺序计数值的短期 RPS 表项数量。)
    * - __u32
      - ``used_by_curr_pic``
      - Bit i specifies if short-term RPS i is used by the current picture.
        (位 i 指定当前图像是否使用短期 RPS i。)
    * - __u32
      - ``use_delta_flag``
      - Bit i specifies if short-term RPS i is included in the short-term RPS entries.
        (位 i 指定短期 RPS i 是否包含在短期 RPS 表项中。)
    * - __u16
      - ``abs_delta_rps_minus1``
      - Absolute delta RPS as specified in section 7.4.8 "Short-term reference picture set
        semantics" of the specification.
        (绝对增量 RPS，如规范第 7.4.8 节 "Short-term reference picture set
        semantics" 所规定。)
    * - __u16
      - ``delta_poc_s0_minus1[16]``
      - Specifies the negative picture order count delta for the i-th entry in the short-term RPS.
        See details in section 7.4.8 "Short-term reference picture set semantics" of the
        specification.
        (指定短期 RPS 中第 i 个表项的负图像顺序计数增量。细节见规范第 7.4.8 节
        "Short-term reference picture set semantics"。)
    * - __u16
      - ``delta_poc_s1_minus1[16]``
      - Specifies the positive picture order count delta for the i-th entry in the short-term RPS.
        See details in section 7.4.8 "Short-term reference picture set semantics" of the
        specification.
        (指定短期 RPS 中第 i 个表项的正图像顺序计数增量。细节见规范第 7.4.8 节
        "Short-term reference picture set semantics"。)
    * - __u16
      - ``flags``
      - See :ref:`Extended Short-Term RPS Flags <hevc_ext_sps_st_rps_flags>`
        (参见 :ref:`Extended Short-Term RPS Flags <hevc_ext_sps_st_rps_flags>`。)

.. _hevc_ext_sps_st_rps_flags:

``Extended SPS Short-Term RPS Flags`` (扩展 SPS 短期 RPS 标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_HEVC_EXT_SPS_ST_RPS_FLAG_INTER_REF_PIC_SET_PRED``
      - 0x00000001
      - Specifies if the short-term RPS is predicted from another short term RPS. See details in
        section 7.4.8 "Short-term reference picture set semantics" of the specification.
        (指定短期 RPS 是否由另一个短期 RPS 预测而来。细节见规范第 7.4.8 节
        "Short-term reference picture set semantics"。)

.. _v4l2-codec-stateless-av1:

``V4L2_CID_STATELESS_AV1_SEQUENCE (struct)`` (AV1 序列)
    Represents an AV1 Sequence OBU (Open Bitstream Unit). See section 5.5
    "Sequence header OBU syntax" in :ref:`av1` for more details.
    (表示一个 AV1 Sequence OBU（开放比特流单元）。更多细节参见 :ref:`av1`
    第 5.5 节 "Sequence header OBU syntax"。)

.. c:type:: v4l2_ctrl_av1_sequence

.. cssclass:: longtable

.. tabularcolumns:: |p{5.8cm}|p{4.8cm}|p{6.6cm}|

.. flat-table:: struct v4l2_ctrl_av1_sequence
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``flags``
      - See :ref:`AV1 Sequence Flags <av1_sequence_flags>`.
        (参见 :ref:`AV1 Sequence Flags <av1_sequence_flags>`。)
    * - __u8
      - ``seq_profile``
      - Specifies the features that can be used in the coded video sequence.
        (指定编码视频序列中可以使用的特性。)
    * - __u8
      - ``order_hint_bits``
      - Specifies the number of bits used for the order_hint field at each frame.
        (指定每帧中 order_hint 字段使用的位数。)
    * - __u8
      - ``bit_depth``
      - the bit depth to use for the sequence as described in section 5.5.2
        "Color config syntax" in :ref:`av1` for more details.
        (该序列使用的位深，细节见 :ref:`av1` 第 5.5.2 节 "Color config syntax"。)
    * - __u8
      - ``reserved``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - __u16
      - ``max_frame_width_minus_1``
      - Specifies the maximum frame width minus 1 for the frames represented by
        this sequence header.
        (指定此序列头所表示帧的最大帧宽度减 1。)
    * - __u16
      - ``max_frame_height_minus_1``
      - Specifies the maximum frame height minus 1 for the frames represented by
        this sequence header.
        (指定此序列头所表示帧的最大帧高度减 1。)

.. _av1_sequence_flags:

``AV1 Sequence Flags`` (AV1 序列标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_SEQUENCE_FLAG_STILL_PICTURE``
      - 0x00000001
      - If set, specifies that the coded video sequence contains only one coded
        frame. If not set, specifies that the coded video sequence contains one
        or more coded frames.
        (如果置位，指定编码视频序列只包含一个编码帧。如果未置位，指定编码视频
        序列包含一个或多个编码帧。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_USE_128X128_SUPERBLOCK``
      - 0x00000002
      - If set, indicates that superblocks contain 128x128 luma samples.
        When equal to 0, it indicates that superblocks contain 64x64 luma
        samples. The number of contained chroma samples depends on
        subsampling_x and subsampling_y.
        (如果置位，指示超级块包含 128x128 个亮度样本。等于 0 时，指示超级块
        包含 64x64 个亮度样本。所含色度样本的数量取决于 subsampling_x 和
        subsampling_y。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_FILTER_INTRA``
      - 0x00000004
      - If set, specifies that the use_filter_intra syntax element may be
        present. If not set, specifies that the use_filter_intra syntax element
        will not be present.
        (如果置位，指定 use_filter_intra 语法元素可能存在。如果未置位，指定
        use_filter_intra 语法元素不会存在。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_INTRA_EDGE_FILTER``
      - 0x00000008
      - Specifies whether the intra edge filtering process should be enabled.
        (指定是否应启用帧内边缘滤波过程。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_INTERINTRA_COMPOUND``
      - 0x00000010
      - If set, specifies that the mode info for inter blocks may contain the
        syntax element interintra. If not set, specifies that the syntax element
        interintra will not be present.
        (如果置位，指定帧间块的模式信息可能包含语法元素 interintra。如果未
        置位，指定语法元素 interintra 不会存在。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_MASKED_COMPOUND``
      - 0x00000020
      - If set, specifies that the mode info for inter blocks may contain the
        syntax element compound_type. If not set, specifies that the syntax
        element compound_type will not be present.
        (如果置位，指定帧间块的模式信息可能包含语法元素 compound_type。如果
        未置位，指定语法元素 compound_type 不会存在。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_WARPED_MOTION``
      - 0x00000040
      - If set, indicates that the allow_warped_motion syntax element may be
        present. If not set, indicates that the allow_warped_motion syntax
        element will not be present.
        (如果置位，指示 allow_warped_motion 语法元素可能存在。如果未置位，
        指示 allow_warped_motion 语法元素不会存在。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_DUAL_FILTER``
      - 0x00000080
      - If set, indicates that the inter prediction filter type may be specified
        independently in the horizontal and vertical directions. If the flag is
        equal to 0, only one filter type may be specified, which is then used in
        both directions.
        (如果置位，指示帧间预测滤波器类型可以在水平和垂直方向上独立指定。如果
        该标志等于 0，则只能指定一种滤波器类型，并在两个方向上使用。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_ORDER_HINT``
      - 0x00000100
      - If set, indicates that tools based on the values of order hints may be
        used. If not set, indicates that tools based on order hints are
        disabled.
        (如果置位，指示可以使用基于顺序提示 (order hint) 值的工具。如果未置位，
        指示基于顺序提示的工具被禁用。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_JNT_COMP``
      - 0x00000200
      - If set, indicates that the distance weights process may be used for
        inter prediction.
        (如果置位，指示距离权重过程可用于帧间预测。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_REF_FRAME_MVS``
      - 0x00000400
      - If set, indicates that the use_ref_frame_mvs syntax element may be
        present. If not set, indicates that the use_ref_frame_mvs syntax element
        will not be present.
        (如果置位，指示 use_ref_frame_mvs 语法元素可能存在。如果未置位，指示
        use_ref_frame_mvs 语法元素不会存在。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_SUPERRES``
      - 0x00000800
      - If set, specifies that the use_superres syntax element will be present
        in the uncompressed header. If not set, specifies that the use_superres
        syntax element will not be present (instead use_superres will be set to
        0 in the uncompressed header without being read).
        (如果置位，指定 use_superres 语法元素将出现在未压缩头中。如果未置位，
        指定 use_superres 语法元素不会出现（改为在未压缩头中把 use_superres
        置为 0，且不读取）。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_CDEF``
      - 0x00001000
      - If set, specifies that cdef filtering may be enabled. If not set,
        specifies that cdef filtering is disabled.
        (如果置位，指定 cdef 滤波可以启用。如果未置位，指定 cdef 滤波被禁用。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_ENABLE_RESTORATION``
      - 0x00002000
      - If set, specifies that loop restoration filtering may be enabled. If not
        set, specifies that loop restoration filtering is disabled.
        (如果置位，指定环路恢复滤波可以启用。如果未置位，指定环路恢复滤波被
        禁用。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_MONO_CHROME``
      - 0x00004000
      - If set, indicates that the video does not contain U and V color planes.
        If not set, indicates that the video contains Y, U, and V color planes.
        (如果置位，指示视频不包含 U 和 V 颜色平面。如果未置位，指示视频包含
        Y、U 和 V 颜色平面。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_COLOR_RANGE``
      - 0x00008000
      - If set, signals full swing representation, i.e. "Full Range
        Quantization". If not set, signals studio swing representation, i.e.
        "Limited Range Quantization".
        (如果置位，发出全摆幅表示的信号，即 "Full Range Quantization"（全量程
        量化）。如果未置位，发出工作室摆幅表示的信号，即 "Limited Range
        Quantization"（有限量程量化）。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_SUBSAMPLING_X``
      - 0x00010000
      - Specify the chroma subsampling format.
        (指定色度子采样格式。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_SUBSAMPLING_Y``
      - 0x00020000
      - Specify the chroma subsampling format.
        (指定色度子采样格式。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_FILM_GRAIN_PARAMS_PRESENT``
      - 0x00040000
      - Specifies whether film grain parameters are present in the coded video
        sequence.
        (指定编码视频序列中是否存在胶片颗粒参数。)
    * - ``V4L2_AV1_SEQUENCE_FLAG_SEPARATE_UV_DELTA_Q``
      - 0x00080000
      - If set, indicates that the U and V planes may have separate delta
        quantizer values. If not set, indicates that the U and V planes will share
        the same delta quantizer value.
        (如果置位，指示 U 和 V 平面可以有各自独立的增量量化器值。如果未置位，
        指示 U 和 V 平面共用相同的增量量化器值。)

``V4L2_CID_STATELESS_AV1_TILE_GROUP_ENTRY (struct)`` (AV1 tile 组条目)
    Represents a single AV1 tile inside an AV1 Tile Group. Note that MiRowStart,
    MiRowEnd, MiColStart and MiColEnd can be retrieved from struct
    v4l2_av1_tile_info in struct v4l2_ctrl_av1_frame using tile_row and
    tile_col. See section 6.10.1 "General tile group OBU semantics" in
    :ref:`av1` for more details.
    (表示 AV1 Tile Group 内的一个 AV1 tile。注意，MiRowStart、MiRowEnd、
    MiColStart 和 MiColEnd 可以通过 tile_row 和 tile_col 从 struct
    v4l2_ctrl_av1_frame 的 struct v4l2_av1_tile_info 中获取。更多细节参见
    :ref:`av1` 第 6.10.1 节 "General tile group OBU semantics"。)

.. c:type:: v4l2_ctrl_av1_tile_group_entry

.. cssclass:: longtable

.. tabularcolumns:: |p{5.8cm}|p{4.8cm}|p{6.6cm}|

.. flat-table:: struct v4l2_ctrl_av1_tile_group_entry
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u32
      - ``tile_offset``
      - Offset from the OBU data, i.e. where the coded tile data actually starts.
        (相对于 OBU 数据的偏移量，即编码 tile 数据的实际起始位置。)
    * - __u32
      - ``tile_size``
      - Specifies the size in bytes of the coded tile. Equivalent to "TileSize"
        in :ref:`av1`.
        (指定编码 tile 的大小，单位为字节。等同于 :ref:`av1` 中的 "TileSize"。)
    * - __u32
      - ``tile_row``
      - Specifies the row of the current tile. Equivalent to "TileRow" in
        :ref:`av1`.
        (指定当前 tile 的行。等同于 :ref:`av1` 中的 "TileRow"。)
    * - __u32
      - ``tile_col``
      - Specifies the column of the current tile. Equivalent to "TileColumn" in
        :ref:`av1`.
        (指定当前 tile 的列。等同于 :ref:`av1` 中的 "TileColumn"。)

.. c:type:: v4l2_av1_warp_model

	AV1 Warp Model as described in section 3 "Symbols and abbreviated terms" of
	:ref:`av1`.
	(AV1 扭曲模型，见 :ref:`av1` 第 3 节 "Symbols and abbreviated terms"。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_WARP_MODEL_IDENTITY``
      - 0
      - Warp model is just an identity transform.
        (扭曲模型只是一个恒等变换。)
    * - ``V4L2_AV1_WARP_MODEL_TRANSLATION``
      - 1
      - Warp model is a pure translation.
        (扭曲模型是纯平移。)
    * - ``V4L2_AV1_WARP_MODEL_ROTZOOM``
      - 2
      - Warp model is a rotation + symmetric zoom + translation.
        (扭曲模型是旋转 + 对称缩放 + 平移。)
    * - ``V4L2_AV1_WARP_MODEL_AFFINE``
      - 3
      - Warp model is a general affine transform.
        (扭曲模型是一般仿射变换。)

.. c:type:: v4l2_av1_reference_frame

AV1 Reference Frames as described in section 6.10.24 "Ref frames semantics"
of :ref:`av1`.
(AV1 参考帧，见 :ref:`av1` 第 6.10.24 节 "Ref frames semantics"。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_REF_INTRA_FRAME``
      - 0
      - Intra Frame Reference.
        (帧内参考。)
    * - ``V4L2_AV1_REF_LAST_FRAME``
      - 1
      - Last Frame Reference.
        (Last 帧参考。)
    * - ``V4L2_AV1_REF_LAST2_FRAME``
      - 2
      - Last2 Frame Reference.
        (Last2 帧参考。)
    * - ``V4L2_AV1_REF_LAST3_FRAME``
      - 3
      - Last3 Frame Reference.
        (Last3 帧参考。)
    * - ``V4L2_AV1_REF_GOLDEN_FRAME``
      - 4
      - Golden Frame Reference.
        (Golden 帧参考。)
    * - ``V4L2_AV1_REF_BWDREF_FRAME``
      - 5
      - BWD Frame Reference.
        (BWD 帧参考。)
    * - ``V4L2_AV1_REF_ALTREF2_FRAME``
      - 6
      - ALTREF2 Frame Reference.
        (ALTREF2 帧参考。)
    * - ``V4L2_AV1_REF_ALTREF_FRAME``
      - 7
      - ALTREF Frame Reference.
        (ALTREF 帧参考。)

.. c:type:: v4l2_av1_global_motion

AV1 Global Motion parameters as described in section 6.8.17
"Global motion params semantics" of :ref:`av1`.
(AV1 全局运动参数，见 :ref:`av1` 第 6.8.17 节 "Global motion params
semantics"。)

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_av1_global_motion
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``flags[V4L2_AV1_TOTAL_REFS_PER_FRAME]``
      - A bitfield containing the flags per reference frame. See
        :ref:`AV1 Global Motion Flags <av1_global_motion_flags>` for more
        details.
        (包含每个参考帧标志的位域。详情参见 :ref:`AV1 Global Motion Flags
        <av1_global_motion_flags>`。)
    * - enum :c:type:`v4l2_av1_warp_model`
      - ``type[V4L2_AV1_TOTAL_REFS_PER_FRAME]``
      - The type of global motion transform used.
        (使用的全局运动变换类型。)
    * - __s32
      - ``params[V4L2_AV1_TOTAL_REFS_PER_FRAME][6]``
      - This field has the same meaning as "gm_params" in :ref:`av1`.
        (此字段与 :ref:`av1` 中的 "gm_params" 含义相同。)
    * - __u8
      - ``invalid``
      - Bitfield indicating whether the global motion params are invalid for a
        given reference frame. See section 7.11.3.6 Setup shear process and the
        variable "warpValid". Use V4L2_AV1_GLOBAL_MOTION_IS_INVALID(ref) to
        create a suitable mask.
        (位域，指明全局运动参数对给定参考帧是否无效。参见第 7.11.3.6 节
        Setup shear process 以及变量 "warpValid"。使用
        V4L2_AV1_GLOBAL_MOTION_IS_INVALID(ref) 来构造合适的掩码。)
    * - __u8
      - ``reserved[3]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. _av1_global_motion_flags:

``AV1 Global Motion Flags`` (AV1 全局运动标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_GLOBAL_MOTION_FLAG_IS_GLOBAL``
      - 0x00000001
      - Specifies whether global motion parameters are present for a particular
        reference frame.
        (指定特定参考帧是否存在全局运动参数。)
    * - ``V4L2_AV1_GLOBAL_MOTION_FLAG_IS_ROT_ZOOM``
      - 0x00000002
      - Specifies whether a particular reference frame uses rotation and zoom
        global motion.
        (指定特定参考帧是否使用旋转和缩放全局运动。)
    * - ``V4L2_AV1_GLOBAL_MOTION_FLAG_IS_TRANSLATION``
      - 0x00000004
      - Specifies whether a particular reference frame uses translation global
        motion
        (指定特定参考帧是否使用平移全局运动)

.. c:type:: v4l2_av1_frame_restoration_type

AV1 Frame Restoration Type.
(AV1 帧恢复类型。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_FRAME_RESTORE_NONE``
      - 0
      - No filtering is applied.
        (不应用任何滤波。)
    * - ``V4L2_AV1_FRAME_RESTORE_WIENER``
      - 1
      - Wiener filter process is invoked.
        (调用维纳滤波过程。)
    * - ``V4L2_AV1_FRAME_RESTORE_SGRPROJ``
      - 2
      - Self guided filter process is invoked.
        (调用自引导滤波过程。)
    * - ``V4L2_AV1_FRAME_RESTORE_SWITCHABLE``
      - 3
      - Restoration filter is swichtable.
        (恢复滤波器可切换。)

.. c:type:: v4l2_av1_loop_restoration

AV1 Loop Restoration as described in section 6.10.15 "Loop restoration params
semantics" of :ref:`av1`.
(AV1 环路恢复，见 :ref:`av1` 第 6.10.15 节 "Loop restoration params
semantics"。)

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_av1_loop_restoration
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``flags``
      - See :ref:`AV1 Loop Restoration Flags <av1_loop_restoration_flags>`.
        (参见 :ref:`AV1 Loop Restoration Flags <av1_loop_restoration_flags>`。)
    * - __u8
      - ``lr_unit_shift``
      - Specifies if the luma restoration size should be halved.
        (指定亮度恢复尺寸是否应减半。)
    * - __u8
      - ``lr_uv_shift``
      - Specifies if the chroma size should be half the luma size.
        (指定色度尺寸是否应为亮度尺寸的一半。)
    * - __u8
      - ``reserved``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - :c:type:`v4l2_av1_frame_restoration_type`
      - ``frame_restoration_type[V4L2_AV1_NUM_PLANES_MAX]``
      - Specifies the type of restoration used for each plane.
        (指定每个平面使用的恢复类型。)
    * - __u8
      - ``loop_restoration_size[V4L2_AV1_MAX_NUM_PLANES]``
      - Specifies the size of loop restoration units in units of samples in the
        current plane.
        (以当前平面的样本为单位指定环路恢复单元的大小。)

.. _av1_loop_restoration_flags:

``AV1 Loop Restoration Flags`` (AV1 环路恢复标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_LOOP_RESTORATION_FLAG_USES_LR``
      - 0x00000001
      - Retains the same meaning as UsesLr in :ref:`av1`.
        (与 :ref:`av1` 中 UsesLr 的含义相同。)
    * - ``V4L2_AV1_LOOP_RESTORATION_FLAG_USES_CHROMA_LR``
      - 0x00000002
      - Retains the same meaning as UsesChromaLr in :ref:`av1`.
        (与 :ref:`av1` 中 UsesChromaLr 的含义相同。)

.. c:type:: v4l2_av1_cdef

AV1 CDEF params semantics as described in section 6.10.14 "CDEF params
semantics" of :ref:`av1`.
(AV1 CDEF 参数语义，见 :ref:`av1` 第 6.10.14 节 "CDEF params semantics"。)

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_av1_cdef
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``damping_minus_3``
      - Controls the amount of damping in the deringing filter.
        (控制去环滤波中的阻尼量。)
    * - __u8
      - ``bits``
      - Specifies the number of bits needed to specify which CDEF filter to
        apply.
        (指定指明要应用哪个 CDEF 滤波器所需的位数。)
    * - __u8
      - ``y_pri_strength[V4L2_AV1_CDEF_MAX]``
      -  Specifies the strength of the primary filter.
        (指定主滤波器的强度。)
    * - __u8
      - ``y_sec_strength[V4L2_AV1_CDEF_MAX]``
      -  Specifies the strength of the secondary filter.
        (指定次级滤波器的强度。)
    * - __u8
      - ``uv_pri_strength[V4L2_AV1_CDEF_MAX]``
      -  Specifies the strength of the primary filter.
        (指定主滤波器的强度。)
    * - __u8
      - ``uv_sec_strength[V4L2_AV1_CDEF_MAX]``
      -  Specifies the strength of the secondary filter.
        (指定次级滤波器的强度。)

.. c:type:: v4l2_av1_segment_feature

AV1 segment features as described in section 3 "Symbols and abbreviated terms"
of :ref:`av1`.
(AV1 分段特征，见 :ref:`av1` 第 3 节 "Symbols and abbreviated terms"。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_SEG_LVL_ALT_Q``
      - 0
      - Index for quantizer segment feature.
        (量化器分段特征的索引。)
    * - ``V4L2_AV1_SEG_LVL_ALT_LF_Y_V``
      - 1
      - Index for vertical luma loop filter segment feature.
        (垂直亮度环路滤波分段特征的索引。)
    * - ``V4L2_AV1_SEG_LVL_REF_FRAME``
      - 5
      - Index for reference frame segment feature.
        (参考帧分段特征的索引。)
    * - ``V4L2_AV1_SEG_LVL_REF_SKIP``
      - 6
      - Index for skip segment feature.
        (跳过分段特征的索引。)
    * - ``V4L2_AV1_SEG_LVL_REF_GLOBALMV``
      - 7
      - Index for global mv feature.
        (全局运动矢量特征的索引。)
    * - ``V4L2_AV1_SEG_LVL_MAX``
      - 8
      - Number of segment features.
        (分段特征的数量。)

.. c:type:: v4l2_av1_segmentation

AV1 Segmentation params as defined in section 6.8.13 "Segmentation params
semantics" of :ref:`av1`.
(AV1 分段参数，定义见 :ref:`av1` 第 6.8.13 节 "Segmentation params
semantics"。)

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_av1_segmentation
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``flags``
      - See :ref:`AV1 Segmentation Flags <av1_segmentation_flags>`
        (参见 :ref:`AV1 Segmentation Flags <av1_segmentation_flags>`。)
    * - __u8
      - ``last_active_seg_id``
      -  Indicates the highest numbered segment id that has some
         enabled feature. This is used when decoding the segment id to only decode
         choices corresponding to used segments.
        (指示启用了某些特征的最高编号分段 id。解码分段 id 时使用它，以便只
        解码与已用分段对应的选项。)
    * - __u8
      - ``feature_enabled[V4L2_AV1_MAX_SEGMENTS]``
      - Bitmask defining which features are enabled in each segment. Use
        V4L2_AV1_SEGMENT_FEATURE_ENABLED to build a suitable mask.
        (定义每个分段中启用了哪些特征的位掩码。使用
        V4L2_AV1_SEGMENT_FEATURE_ENABLED 来构造合适的掩码。)
    * - __u16
      - ``feature_data[V4L2_AV1_MAX_SEGMENTS][V4L2_AV1_SEG_LVL_MAX]``
      -  Data attached to each feature. Data entry is only valid if the feature
         is enabled.
        (附属于每个特征的数据。只有特征启用时数据项才有效。)

.. _av1_segmentation_flags:

``AV1 Segmentation Flags`` (AV1 分段标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_SEGMENTATION_FLAG_ENABLED``
      - 0x00000001
      - If set, indicates that this frame makes use of the segmentation tool. If
        not set, indicates that the frame does not use segmentation.
        (如果置位，指示本帧使用了分段工具。如果未置位，指示该帧不使用分段。)
    * - ``V4L2_AV1_SEGMENTATION_FLAG_UPDATE_MAP``
      - 0x00000002
      - If set, indicates that the segmentation map are updated during the
        decoding of this frame. If not set, indicates that the segmentation map
        from the previous frame is used.
        (如果置位，指示在本帧解码期间更新分段图。如果未置位，指示使用前一帧的
        分段图。)
    * - ``V4L2_AV1_SEGMENTATION_FLAG_TEMPORAL_UPDATE``
      - 0x00000004
      - If set, indicates that the updates to the segmentation map are coded
        relative to the existing segmentation map. If not set, indicates that
        the new segmentation map is coded without reference to the existing
        segmentation map.
        (如果置位，指示分段图的更新是相对于现有分段图编码的。如果未置位，指示
        新分段图的编码不参考现有分段图。)
    * - ``V4L2_AV1_SEGMENTATION_FLAG_UPDATE_DATA``
      - 0x00000008
      - If set, indicates that the updates to the segmentation map are coded
        relative to the existing segmentation map. If not set, indicates that
        the new segmentation map is coded without reference to the existing
        segmentation map.
        (如果置位，指示分段图的更新是相对于现有分段图编码的。如果未置位，指示
        新分段图的编码不参考现有分段图。)
    * - ``V4L2_AV1_SEGMENTATION_FLAG_SEG_ID_PRE_SKIP``
      - 0x00000010
      - If set, indicates that the segment id will be read before the skip
        syntax element. If not set, indicates that the skip syntax element will
        be read first.
        (如果置位，指示分段 id 将在 skip 语法元素之前读取。如果未置位，指示
        先读取 skip 语法元素。)

.. c:type:: v4l2_av1_loop_filter

AV1 Loop filter params as defined in section 6.8.10 "Loop filter semantics" of
:ref:`av1`.
(AV1 环路滤波参数，定义见 :ref:`av1` 第 6.8.10 节 "Loop filter
semantics"。)

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_av1_loop_filter
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``flags``
      - See
        :ref:`AV1 Loop Filter flags <av1_loop_filter_flags>` for more details.
        (详情参见 :ref:`AV1 Loop Filter flags <av1_loop_filter_flags>`。)
    * - __u8
      - ``level[4]``
      - An array containing loop filter strength values. Different loop
        filter strength values from the array are used depending on the image
        plane being filtered, and the edge direction (vertical or horizontal)
        being filtered.
        (包含环路滤波强度值的数组。根据被滤波的图像平面以及被滤波边缘的方向
        （垂直或水平），使用数组中不同的环路滤波强度值。)
    * - __u8
      - ``sharpness``
      - indicates the sharpness level. The loop_filter_level and
        loop_filter_sharpness together determine when a block edge is filtered,
        and by how much the filtering can change the sample values. The loop
        filter process is described in section 7.14 of :ref:`av1`.
        (指示锐度级别。loop_filter_level 和 loop_filter_sharpness 共同决定
        何时对块边缘进行滤波，以及滤波可以在多大程度上改变样本值。环路滤波
        过程在 :ref:`av1` 第 7.14 节中描述。)
    * - __u8
      - ``ref_deltas[V4L2_AV1_TOTAL_REFS_PER_FRAME]``
      - contains the adjustment needed for the filter level based on the
        chosen reference frame. If this syntax element is not present, it
        maintains its previous value.
        (包含基于所选参考帧对滤波级别所需的调整。如果此语法元素不存在，则
        保持其先前值。)
    * - __u8
      - ``mode_deltas[2]``
      - contains the adjustment needed for the filter level based on
        the chosen mode. If this syntax element is not present, it maintains its
        previous value.
        (包含基于所选模式对滤波级别所需的调整。如果此语法元素不存在，则保持
        其先前值。)
    * - __u8
      - ``delta_lf_res``
      - specifies the left shift which should be applied to decoded loop filter
        delta values.
        (指定应应用于解码后的环路滤波增量值的左移位数。)

.. _av1_loop_filter_flags:

``AV1 Loop Filter Flags`` (AV1 环路滤波标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_LOOP_FILTER_FLAG_DELTA_ENABLED``
      - 0x00000001
      - If set, means that the filter level depends on the mode and reference
        frame used to predict a block. If not set, means that the filter level
        does not depend on the mode and reference frame.
        (如果置位，表示滤波级别取决于用于预测块的模式和参考帧。如果未置位，
        表示滤波级别不取决于模式和参考帧。)
    * - ``V4L2_AV1_LOOP_FILTER_FLAG_DELTA_UPDATE``
      - 0x00000002
      - If set, means that additional syntax elements are present that specify
        which mode and reference frame deltas are to be updated. If not set,
        means that these syntax elements are not present.
        (如果置位，表示存在用于指明要更新哪些模式与参考帧增量的额外语法元素。
        如果未置位，表示这些语法元素不存在。)
    * - ``V4L2_AV1_LOOP_FILTER_FLAG_DELTA_LF_PRESENT``
      - 0x00000004
      - Specifies whether loop filter delta values are present
        (指定是否存在环路滤波增量值)
    * - ``V4L2_AV1_LOOP_FILTER_FLAG_DELTA_LF_MULTI``
      - 0x00000008
      - A value equal to 1 specifies that separate loop filter
        deltas are sent for horizontal luma edges, vertical luma edges,
        the U edges, and the V edges. A value of delta_lf_multi equal to 0
        specifies that the same loop filter delta is used for all edges.
        (值等于 1 表示为水平亮度边缘、垂直亮度边缘、U 边缘和 V 边缘分别发送
        独立的环路滤波增量。delta_lf_multi 等于 0 表示所有边缘使用相同的环路
        滤波增量。)

.. c:type:: v4l2_av1_quantization

AV1 Quantization params as defined in section 6.8.11 "Quantization params
semantics" of :ref:`av1`.
(AV1 量化参数，定义见 :ref:`av1` 第 6.8.11 节 "Quantization params
semantics"。)

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_av1_quantization
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``flags``
      - See
        :ref:`AV1 Loop Filter flags <av1_quantization_flags>` for more details.
        (详情参见 :ref:`AV1 Loop Filter flags <av1_quantization_flags>`。)
    * - __u8
      - ``base_q_idx``
      - Indicates the base frame qindex. This is used for Y AC coefficients and
        as the base value for the other quantizers.
        (指示基础帧 qindex。它用于 Y AC 系数，并作为其他量化器的基础值。)
    * - __u8
      - ``delta_q_y_dc``
      - Indicates the Y DC quantizer relative to base_q_idx.
        (指示相对于 base_q_idx 的 Y DC 量化器。)
    * - __u8
      - ``delta_q_u_dc``
      - Indicates the U DC quantizer relative to base_q_idx.
        (指示相对于 base_q_idx 的 U DC 量化器。)
    * - __u8
      - ``delta_q_u_ac``
      - Indicates the U AC quantizer relative to base_q_idx.
        (指示相对于 base_q_idx 的 U AC 量化器。)
    * - __u8
      - ``delta_q_v_dc``
      - Indicates the V DC quantizer relative to base_q_idx.
        (指示相对于 base_q_idx 的 V DC 量化器。)
    * - __u8
      - ``delta_q_v_ac``
      - Indicates the V AC quantizer relative to base_q_idx.
        (指示相对于 base_q_idx 的 V AC 量化器。)
    * - __u8
      - ``qm_y``
      - Specifies the level in the quantizer matrix that should be used for
        luma plane decoding.
        (指定亮度平面解码应使用的量化器矩阵级别。)
    * - __u8
      - ``qm_u``
      - Specifies the level in the quantizer matrix that should be used for
        chroma U plane decoding.
        (指定色度 U 平面解码应使用的量化器矩阵级别。)
    * - __u8
      - ``qm_v``
      - Specifies the level in the quantizer matrix that should be used for
        chroma V plane decoding.
        (指定色度 V 平面解码应使用的量化器矩阵级别。)
    * - __u8
      - ``delta_q_res``
      - Specifies the left shift which should be applied to decoded quantizer
        index delta values.
        (指定应应用于解码后的量化器索引增量值的左移位数。)

.. _av1_quantization_flags:

``AV1 Quantization Flags`` (AV1 量化标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_QUANTIZATION_FLAG_DIFF_UV_DELTA``
      - 0x00000001
      - If set, indicates that the U and V delta quantizer values are coded
        separately. If not set, indicates that the U and V delta quantizer
        values share a common value.
        (如果置位，指示 U 和 V 增量量化器值分开编码。如果未置位，指示 U 和 V
        增量量化器值共用一个值。)
    * - ``V4L2_AV1_QUANTIZATION_FLAG_USING_QMATRIX``
      - 0x00000002
      - If set, specifies that the quantizer matrix will be used to compute
        quantizers.
        (如果置位，指定将使用量化器矩阵来计算量化器。)
    * - ``V4L2_AV1_QUANTIZATION_FLAG_DELTA_Q_PRESENT``
      - 0x00000004
      - Specifies whether quantizer index delta values are present.
        (指定是否存在量化器索引增量值。)

.. c:type:: v4l2_av1_tile_info

AV1 Tile info as defined in section 6.8.14 "Tile info semantics" of ref:`av1`.
(AV1 tile 信息，定义见 ref:`av1` 第 6.8.14 节 "Tile info semantics"。)

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_av1_tile_info
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``flags``
      - See
        :ref:`AV1 Tile Info flags <av1_tile_info_flags>` for more details.
        (详情参见 :ref:`AV1 Tile Info flags <av1_tile_info_flags>`。)
    * - __u8
      - ``context_update_tile_id``
      - Specifies which tile to use for the CDF update.
        (指定用于 CDF 更新的 tile。)
    * - __u8
      - ``tile_cols``
      - Specifies the number of tiles across the frame.
        (指定帧横向的 tile 数量。)
    * - __u8
      - ``tile_rows``
      - Specifies the number of tiles down the frame.
        (指定帧纵向的 tile 数量。)
    * - __u32
      - ``mi_col_starts[V4L2_AV1_MAX_TILE_COLS + 1]``
      - An array specifying the start column (in units of 4x4 luma
        samples) for each tile across the image.
        (一个数组，为图像中每个 tile 指定起始列（以 4x4 亮度样本为单位）。)
    * - __u32
      - ``mi_row_starts[V4L2_AV1_MAX_TILE_ROWS + 1]``
      - An array specifying the start row (in units of 4x4 luma
        samples) for each tile across the image.
        (一个数组，为图像中每个 tile 指定起始行（以 4x4 亮度样本为单位）。)
    * - __u32
      - ``width_in_sbs_minus_1[V4L2_AV1_MAX_TILE_COLS]``
      - Specifies the width of a tile minus 1 in units of superblocks.
        (以超级块为单位指定 tile 宽度减 1。)
    * - __u32
      - ``height_in_sbs_minus_1[V4L2_AV1_MAX_TILE_ROWS]``
      - Specifies the height of a tile minus 1 in units of superblocks.
        (以超级块为单位指定 tile 高度减 1。)
    * - __u8
      - ``tile_size_bytes``
      - Specifies the number of bytes needed to code each tile size.
        (指定编码每个 tile 大小所需的字节数。)
    * - __u8
      - ``reserved[3]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. _av1_tile_info_flags:

``AV1 Tile Info Flags`` (AV1 tile 信息标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_TILE_INFO_FLAG_UNIFORM_TILE_SPACING``
      - 0x00000001
      - If set, means that the tiles are uniformly spaced across the frame. (In
        other words, all tiles are the same size except for the ones at the
        right and bottom edge which can be smaller). If not set means that the
        tile sizes are coded.
        (如果置位，表示 tile 在整帧上均匀间隔（换句话说，除右边缘和下边缘的
        tile 可以更小外，所有 tile 尺寸相同）。如果未置位，表示 tile 尺寸被
        编码在码流中。)

.. c:type:: v4l2_av1_frame_type

AV1 Frame Type
(AV1 帧类型。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_KEY_FRAME``
      - 0
      - Key frame.
        (关键帧。)
    * - ``V4L2_AV1_INTER_FRAME``
      - 1
      - Inter frame.
        (帧间帧。)
    * - ``V4L2_AV1_INTRA_ONLY_FRAME``
      - 2
      - Intra-only frame.
        (仅帧内帧。)
    * - ``V4L2_AV1_SWITCH_FRAME``
      - 3
      - Switch frame.
        (切换帧。)

.. c:type:: v4l2_av1_interpolation_filter

AV1 Interpolation Filter
(AV1 插值滤波器。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_INTERPOLATION_FILTER_EIGHTTAP``
      - 0
      - Eight tap filter.
        (八抽头滤波器。)
    * - ``V4L2_AV1_INTERPOLATION_FILTER_EIGHTTAP_SMOOTH``
      - 1
      - Eight tap smooth filter.
        (八抽头平滑滤波器。)
    * - ``V4L2_AV1_INTERPOLATION_FILTER_EIGHTTAP_SHARP``
      - 2
      - Eight tap sharp filter.
        (八抽头锐利滤波器。)
    * - ``V4L2_AV1_INTERPOLATION_FILTER_BILINEAR``
      - 3
      - Bilinear filter.
        (双线性滤波器。)
    * - ``V4L2_AV1_INTERPOLATION_FILTER_SWITCHABLE``
      - 4
      - Filter selection is signaled at the block level.
        (滤波器选择在块级别上发出信号。)

.. c:type:: v4l2_av1_tx_mode

AV1 Tx mode as described in section 6.8.21 "TX mode semantics" of :ref:`av1`.
(AV1 Tx 模式，见 :ref:`av1` 第 6.8.21 节 "TX mode semantics"。)

.. raw:: latex

    \scriptsize

.. tabularcolumns:: |p{7.4cm}|p{0.3cm}|p{9.6cm}|

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_TX_MODE_ONLY_4X4``
      - 0
      -  The inverse transform will use only 4x4 transforms.
        (逆变换将只使用 4x4 变换。)
    * - ``V4L2_AV1_TX_MODE_LARGEST``
      - 1
      - The inverse transform will use the largest transform size that fits
        inside the block.
        (逆变换将使用能放进块内的最大变换尺寸。)
    * - ``V4L2_AV1_TX_MODE_SELECT``
      - 2
      - The choice of transform size is specified explicitly for each block.
        (为每个块显式指定变换尺寸的选择。)

``V4L2_CID_STATELESS_AV1_FRAME (struct)`` (AV1 帧头)
    Represents a Frame Header OBU. See 6.8 "Frame Header OBU semantics" of
    :ref:`av1` for more details.
    (表示一个 Frame Header OBU。更多细节参见 :ref:`av1` 第 6.8 节 "Frame
    Header OBU semantics"。)

.. c:type:: v4l2_ctrl_av1_frame

.. cssclass:: longtable

.. tabularcolumns:: |p{5.8cm}|p{4.8cm}|p{6.6cm}|

.. flat-table:: struct v4l2_ctrl_av1_frame
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - struct :c:type:`v4l2_av1_tile_info`
      - ``tile_info``
      - Tile info
        (tile 信息)
    * - struct :c:type:`v4l2_av1_quantization`
      - ``quantization``
      - Quantization parameters.
        (量化参数。)
    * - __u8
      - ``superres_denom``
      - The denominator for the upscaling ratio.
        (上采样比率的分母。)
    * - struct :c:type:`v4l2_av1_segmentation`
      - ``segmentation``
      - Segmentation parameters.
        (分段参数。)
    * - struct :c:type:`v4l2_av1_loop_filter`
      - ``loop_filter``
      - Loop filter params
        (环路滤波参数)
    * - struct :c:type:`v4l2_av1_cdef`
      - ``cdef``
      - CDEF params
        (CDEF 参数)
    * - __u8
      - ``skip_mode_frame[2]``
      - Specifies the frames to use for compound prediction when skip_mode is
        equal to 1.
        (指定 skip_mode 等于 1 时用于复合预测的帧。)
    * - __u8
      - ``primary_ref_frame``
      - Specifies which reference frame contains the CDF values and other state
        that should be loaded at the start of the frame.
        (指定哪个参考帧包含应在帧开始时加载的 CDF 值和其他状态。)
    * - struct :c:type:`v4l2_av1_loop_restoration`
      - ``loop_restoration``
      - Loop restoration parameters.
        (环路恢复参数。)
    * - struct :c:type:`v4l2_av1_global_motion`
      - ``global_motion``
      - Global motion parameters.
        (全局运动参数。)
    * - __u32
      - ``flags``
      - See
        :ref:`AV1 Frame flags <av1_frame_flags>` for more details.
        (详情参见 :ref:`AV1 Frame flags <av1_frame_flags>`。)
    * - enum :c:type:`v4l2_av1_frame_type`
      - ``frame_type``
      - Specifies the AV1 frame type
        (指定 AV1 帧类型)
    * - __u32
      - ``order_hint``
      - Specifies OrderHintBits least significant bits of the expected output
        order for this frame.
        (指定该帧预期输出顺序的 OrderHintBits 个最低有效位。)
    * - __u32
      - ``upscaled_width``
      - The upscaled width.
        (上采样后的宽度。)
    * - enum :c:type:`v4l2_av1_interpolation_filter`
      - ``interpolation_filter``
      - Specifies the filter selection used for performing inter prediction.
        (指定执行帧间预测时使用的滤波器选择。)
    * - enum :c:type:`v4l2_av1_tx_mode`
      - ``tx_mode``
      - Specifies how the transform size is determined.
        (指定变换尺寸的确定方式。)
    * - __u32
      - ``frame_width_minus_1``
      - Add 1 to get the frame's width.
        (加 1 得到帧宽度。)
    * - __u32
      - ``frame_height_minus_1``
      - Add 1 to get the frame's height.
        (加 1 得到帧高度。)
    * - __u16
      - ``render_width_minus_1``
      - Add 1 to get the render width of the frame in luma samples.
        (加 1 得到帧以亮度样本表示的渲染宽度。)
    * - __u16
      - ``render_height_minus_1``
      - Add 1 to get the render height of the frame in luma samples.
        (加 1 得到帧以亮度样本表示的渲染高度。)
    * - __u32
      - ``current_frame_id``
      - Specifies the frame id number for the current frame. Frame
        id numbers are additional information that do not affect the decoding
        process, but provide decoders with a way of detecting missing reference
        frames so that appropriate action can be taken.
        (指定当前帧的帧 id 号。帧 id 号是不影响解码过程的附加信息，但为解码器
        提供了一种检测缺失参考帧的方法，以便采取适当的措施。)
    * - __u8
      - ``buffer_removal_time[V4L2_AV1_MAX_OPERATING_POINTS]``
      - Specifies the frame removal time in units of DecCT clock ticks counted
        from the removal time of the last random access point for operating point
        opNum.
        (以 DecCT 时钟滴答为单位指定帧移除时间，从操作点 opNum 的最后一个随机
        访问点的移除时间起计。)
    * - __u8
      - ``reserved[4]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)
    * - __u32
      - ``order_hints[V4L2_AV1_TOTAL_REFS_PER_FRAME]``
      - Specifies the expected output order hint for each reference frame.
        This field corresponds to the OrderHints variable from the specification
        (section 5.9.2  "Uncompressed header syntax"). As such, this is only
        used for non-intra frames and ignored otherwise. order_hints[0] is
        always ignored.
        (指定每个参考帧的预期输出顺序提示。此字段对应规范中的 OrderHints
        变量（第 5.9.2 节 "Uncompressed header syntax"）。因此，它只用于
        非帧内帧，其他情况下被忽略。order_hints[0] 总是被忽略。)
    * - __u64
      - ``reference_frame_ts[V4L2_AV1_TOTAL_REFS_PER_FRAME]``
      - The V4L2 timestamp for each of the reference frames enumerated in
        enum :c:type:`v4l2_av1_reference_frame` starting at
        ``V4L2_AV1_REF_LAST_FRAME``. This represents the state of reference
        slot as described in the spec and updated by userland through the
        "Reference frame update process" in section 7.20 The timestamp refers
        to the ``timestamp`` field in struct :c:type:`v4l2_buffer`. Use the
        :c:func:`v4l2_timeval_to_ns()` function to convert the struct
        :c:type:`timeval` in struct :c:type:`v4l2_buffer` to a __u64.
        (从 ``V4L2_AV1_REF_LAST_FRAME`` 开始的、枚举
        :c:type:`v4l2_av1_reference_frame` 中各参考帧的 V4L2 时间戳。它表示
        规范中所述参考槽的状态，由用户态通过第 7.20 节的 "Reference frame
        update process" 更新。该时间戳指的是 ``timestamp`` 字段。使用
        :c:func:`v4l2_timeval_to_ns()` 函数把 struct :c:type:`v4l2_buffer`
        中的 struct :c:type:`timeval` 转换为 __u64。)
    * - __s8
      - ``ref_frame_idx[V4L2_AV1_REFS_PER_FRAME]``
      - An index into ``reference_frame_ts`` representing the ordered list of
        references used by inter-frame. Matches the bitstream syntax
        element of the same name.
        (``reference_frame_ts`` 的索引，表示帧间使用的有序参考列表。与比特流中
        同名语法元素对应。)
    * - __u8
      - ``refresh_frame_flags``
      - Contains a bitmask that specifies which reference frame slots will be
        updated with the current frame after it is decoded.
        (包含一个位掩码，指明当前帧解码后哪些参考帧槽会被更新为当前帧。)

.. _av1_frame_flags:

``AV1 Frame Flags`` (AV1 帧标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_FRAME_FLAG_SHOW_FRAME``
      - 0x00000001
      - If set, specifies that this frame should be immediately output once
        decoded. If not set, specifies that this frame should not be immediately
        output; it may be output later if a later uncompressed header uses
        show_existing_frame equal to 1.
        (如果置位，指定此帧解码后应立即输出。如果未置位，指定此帧不应立即输出；
        如果后续的未压缩头使用 show_existing_frame 等于 1，它可能在稍后输出。)
    * - ``V4L2_AV1_FRAME_FLAG_SHOWABLE_FRAME``
      - 0x00000002
      - If set, specifies that the frame may be output using the
        show_existing_frame mechanism. If not set, specifies that this frame
        will not be output using the show_existing_frame mechanism.
        (如果置位，指定该帧可以通过 show_existing_frame 机制输出。如果未置位，
        指定此帧不会通过 show_existing_frame 机制输出。)
    * - ``V4L2_AV1_FRAME_FLAG_ERROR_RESILIENT_MODE``
      - 0x00000004
      - Specifies whether error resilient mode is enabled.
        (指定是否启用了容错模式。)
    * - ``V4L2_AV1_FRAME_FLAG_DISABLE_CDF_UPDATE``
      - 0x00000008
      - Specifies whether the CDF update in the symbol decoding process should
        be disabled.
        (指定符号解码过程中的 CDF 更新是否应被禁用。)
    * - ``V4L2_AV1_FRAME_FLAG_ALLOW_SCREEN_CONTENT_TOOLS``
      - 0x00000010
      - If set, indicates that intra blocks may use palette encoding. If not
        set, indicates that palette encoding is never used.
        (如果置位，指示帧内块可以使用调色板编码。如果未置位，指示从不使用
        调色板编码。)
    * - ``V4L2_AV1_FRAME_FLAG_FORCE_INTEGER_MV``
      - 0x00000020
      - If set, specifies that motion vectors will always be integers. If not
        set, specifies that motion vectors can contain fractional bits.
        (如果置位，指定运动矢量将总是整数。如果未置位，指定运动矢量可以包含
        小数位。)
    * - ``V4L2_AV1_FRAME_FLAG_ALLOW_INTRABC``
      - 0x00000040
      - If set, indicates that intra block copy may be used in this frame. If
        not set, indicates that intra block copy is not allowed in this frame.
        (如果置位，指示本帧可以使用帧内块复制。如果未置位，指示本帧不允许
        帧内块复制。)
    * - ``V4L2_AV1_FRAME_FLAG_USE_SUPERRES``
      - 0x00000080
      - If set, indicates that upscaling is needed.
        (如果置位，指示需要上采样。)
    * - ``V4L2_AV1_FRAME_FLAG_ALLOW_HIGH_PRECISION_MV``
      - 0x00000100
      - If set, specifies that motion vectors are specified to eighth pel
        precision. If not set, specifies that motion vectors are specified to
        quarter pel precision;
        (如果置位，指定运动矢量以八分之一像素精度给出。如果未置位，指定运动
        矢量以四分之一像素精度给出；)
    * - ``V4L2_AV1_FRAME_FLAG_IS_MOTION_MODE_SWITCHABLE``
      - 0x00000200
      - If not set, specifies that only the SIMPLE motion mode will be used.
        (如果未置位，指定只使用 SIMPLE 运动模式。)
    * - ``V4L2_AV1_FRAME_FLAG_USE_REF_FRAME_MVS``
      - 0x00000400
      - If set specifies that motion vector information from a previous frame
        can be used when decoding the current frame. If not set, specifies that
        this information will not be used.
        (如果置位，指定解码当前帧时可以使用来自前一帧的运动矢量信息。如果未
        置位，指定不使用该信息。)
    * - ``V4L2_AV1_FRAME_FLAG_DISABLE_FRAME_END_UPDATE_CDF``
      - 0x00000800
      - If set indicates that the end of frame CDF update is disabled. If not
        set, indicates that the end of frame CDF update is enabled
        (如果置位，指示帧结束时的 CDF 更新被禁用。如果未置位，指示帧结束时的
        CDF 更新被启用)
    * - ``V4L2_AV1_FRAME_FLAG_ALLOW_WARPED_MOTION``
      - 0x00001000
      - If set, indicates that the syntax element motion_mode may be present, if
        not set, indicates that the syntax element motion_mode will not be
        present.
        (如果置位，指示语法元素 motion_mode 可能存在；如果未置位，指示语法
        元素 motion_mode 不会存在。)
    * - ``V4L2_AV1_FRAME_FLAG_REFERENCE_SELECT``
      - 0x00002000
      - If set, specifies that the mode info for inter blocks contains the
        syntax element comp_mode that indicates whether to use single or
        compound reference prediction. If not set, specifies that all inter
        blocks will use single prediction.
        (如果置位，指定帧间块的模式信息包含语法元素 comp_mode，它指明使用单一
        参考预测还是复合参考预测。如果未置位，指定所有帧间块使用单一预测。)
    * - ``V4L2_AV1_FRAME_FLAG_REDUCED_TX_SET``
      - 0x00004000
      - If set, specifies that the frame is restricted to a reduced subset of
        the full set of transform types.
        (如果置位，指定该帧被限制在全部变换类型的一个缩减子集内。)
    * - ``V4L2_AV1_FRAME_FLAG_SKIP_MODE_ALLOWED``
      - 0x00008000
      - This flag retains the same meaning as SkipModeAllowed in :ref:`av1`.
        (此标志与 :ref:`av1` 中 SkipModeAllowed 的含义相同。)
    * - ``V4L2_AV1_FRAME_FLAG_SKIP_MODE_PRESENT``
      - 0x00010000
      - If set, specifies that the syntax element skip_mode will be present, if
        not set, specifies that skip_mode will not be used for this frame.
        (如果置位，指定语法元素 skip_mode 将出现；如果未置位，指定本帧不使用
        skip_mode。)
    * - ``V4L2_AV1_FRAME_FLAG_FRAME_SIZE_OVERRIDE``
      - 0x00020000
      - If set, specifies that the frame size will either be specified as the
        size of one of the reference frames, or computed from the
        frame_width_minus_1 and frame_height_minus_1 syntax elements. If not
        set, specifies that the frame size is equal to the size in the sequence
        header.
        (如果置位，指定帧尺寸或者被指定为某个参考帧的尺寸，或者由
        frame_width_minus_1 和 frame_height_minus_1 语法元素计算得出。如果
        未置位，指定帧尺寸等于序列头中的尺寸。)
    * - ``V4L2_AV1_FRAME_FLAG_BUFFER_REMOVAL_TIME_PRESENT``
      - 0x00040000
      - If set, specifies that buffer_removal_time is present. If not set,
        specifies that buffer_removal_time is not present.
        (如果置位，指定存在 buffer_removal_time。如果未置位，指定不存在
        buffer_removal_time。)
    * - ``V4L2_AV1_FRAME_FLAG_FRAME_REFS_SHORT_SIGNALING``
      - 0x00080000
      - If set, indicates that only two reference frames are explicitly
        signaled. If not set, indicates that all reference frames are explicitly
        signaled.
        (如果置位，指示只显式发出两个参考帧的信号。如果未置位，指示所有参考
        帧都显式发出信号。)

``V4L2_CID_STATELESS_AV1_FILM_GRAIN (struct)`` (AV1 胶片颗粒)
    Represents the optional film grain parameters. See section
    6.8.20 "Film grain params semantics" of :ref:`av1` for more details.
    (表示可选的胶片颗粒参数。更多细节参见 :ref:`av1` 第 6.8.20 节 "Film
    grain params semantics"。)

.. c:type:: v4l2_ctrl_av1_film_grain

.. cssclass:: longtable

.. tabularcolumns:: |p{1.5cm}|p{5.8cm}|p{10.0cm}|

.. flat-table:: struct v4l2_ctrl_av1_film_grain
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - __u8
      - ``flags``
      - See :ref:`AV1 Film Grain Flags <av1_film_grain_flags>`.
        (参见 :ref:`AV1 Film Grain Flags <av1_film_grain_flags>`。)
    * - __u8
      - ``cr_mult``
      - Represents a multiplier for the cr component used in derivation of the
        input index to the cr component scaling function.
        (表示 cr 分量的乘数，用于推导 cr 分量缩放函数的输入索引。)
    * - __u16
      - ``grain_seed``
      - Specifies the starting value for the pseudo-random numbers used during
        film grain synthesis.
        (指定胶片颗粒合成期间使用的伪随机数的起始值。)
    * - __u8
      - ``film_grain_params_ref_idx``
      - Indicates which reference frame contains the film grain parameters to be
	used for this frame.
        (指示哪个参考帧包含本帧要使用的胶片颗粒参数。)
    * - __u8
      - ``num_y_points``
      - Specifies the number of points for the piece-wise linear scaling
        function of the luma component.
        (指定亮度分量分段线性缩放函数的点数。)
    * - __u8
      - ``point_y_value[V4L2_AV1_MAX_NUM_Y_POINTS]``
      - Represents the x (luma value) coordinate for the i-th point
        of the piecewise linear scaling function for luma component. The values
        are signaled on the scale of 0..255. In case of 10 bit video, these
        values correspond to luma values divided by 4. In case of 12 bit video,
        these values correspond to luma values divided by 16.
        (表示亮度分量分段线性缩放函数第 i 个点的 x（亮度值）坐标。这些值以
        0..255 的刻度发出信号。对 10 位视频，这些值对应亮度值除以 4；对 12 位
        视频，对应亮度值除以 16。)
    * - __u8
      - ``point_y_scaling[V4L2_AV1_MAX_NUM_Y_POINTS]``
      - Represents the scaling (output) value for the i-th point
        of the piecewise linear scaling function for luma component.
        (表示亮度分量分段线性缩放函数第 i 个点的缩放（输出）值。)
    * - __u8
      - ``num_cb_points``
      -  Specifies the number of points for the piece-wise linear scaling
         function of the cb component.
        (指定 cb 分量分段线性缩放函数的点数。)
    * - __u8
      - ``point_cb_value[V4L2_AV1_MAX_NUM_CB_POINTS]``
      - Represents the x coordinate for the i-th point of the
        piece-wise linear scaling function for cb component. The values are
        signaled on the scale of 0..255.
        (表示 cb 分量分段线性缩放函数第 i 个点的 x 坐标。这些值以 0..255 的
        刻度发出信号。)
    * - __u8
      - ``point_cb_scaling[V4L2_AV1_MAX_NUM_CB_POINTS]``
      - Represents the scaling (output) value for the i-th point of the
        piecewise linear scaling function for cb component.
        (表示 cb 分量分段线性缩放函数第 i 个点的缩放（输出）值。)
    * - __u8
      - ``num_cr_points``
      - Represents the number of points for the piece-wise
        linear scaling function of the cr component.
        (表示 cr 分量分段线性缩放函数的点数。)
    * - __u8
      - ``point_cr_value[V4L2_AV1_MAX_NUM_CR_POINTS]``
      - Represents the x coordinate for the i-th point of the
        piece-wise linear scaling function for cr component. The values are
        signaled on the scale of 0..255.
        (表示 cr 分量分段线性缩放函数第 i 个点的 x 坐标。这些值以 0..255 的
        刻度发出信号。)
    * - __u8
      - ``point_cr_scaling[V4L2_AV1_MAX_NUM_CR_POINTS]``
      - Represents the scaling (output) value for the i-th point of the
        piecewise linear scaling function for cr component.
        (表示 cr 分量分段线性缩放函数第 i 个点的缩放（输出）值。)
    * - __u8
      - ``grain_scaling_minus_8``
      - Represents the shift - 8 applied to the values of the chroma component.
        The grain_scaling_minus_8 can take values of 0..3 and determines the
        range and quantization step of the standard deviation of film grain.
        (表示应用于色度分量值的减 8 移位。grain_scaling_minus_8 可取 0..3，
        它决定胶片颗粒标准差的范围和量化步长。)
    * - __u8
      - ``ar_coeff_lag``
      - Specifies the number of auto-regressive coefficients for luma and
        chroma.
        (指定亮度和色度的自回归系数数量。)
    * - __u8
      - ``ar_coeffs_y_plus_128[V4L2_AV1_AR_COEFFS_SIZE]``
      - Specifies auto-regressive coefficients used for the Y plane.
        (指定用于 Y 平面的自回归系数。)
    * - __u8
      - ``ar_coeffs_cb_plus_128[V4L2_AV1_AR_COEFFS_SIZE]``
      - Specifies auto-regressive coefficients used for the U plane.
        (指定用于 U 平面的自回归系数。)
    * - __u8
      - ``ar_coeffs_cr_plus_128[V4L2_AV1_AR_COEFFS_SIZE]``
      - Specifies auto-regressive coefficients used for the V plane.
        (指定用于 V 平面的自回归系数。)
    * - __u8
      - ``ar_coeff_shift_minus_6``
      - Specifies the range of the auto-regressive coefficients. Values of 0,
        1, 2, and 3 correspond to the ranges for auto-regressive coefficients of
        [-2, 2), [-1, 1), [-0.5, 0.5) and [-0.25, 0.25) respectively.
        (指定自回归系数的范围。值 0、1、2 和 3 分别对应自回归系数范围
        [-2, 2)、[-1, 1)、[-0.5, 0.5) 和 [-0.25, 0.25)。)
    * - __u8
      - ``grain_scale_shift``
      - Specifies how much the Gaussian random numbers should be scaled down
        during the grain synthesis process.
        (指定在颗粒合成过程中高斯随机数应缩小多少。)
    * - __u8
      - ``cb_mult``
      - Represents a multiplier for the cb component used in derivation of the
        input index to the cb component scaling function.
        (表示 cb 分量的乘数，用于推导 cb 分量缩放函数的输入索引。)
    * - __u8
      - ``cb_luma_mult``
      - Represents a multiplier for the average luma component used in
        derivation of the input index to the cb component scaling function..
        (表示平均亮度分量的乘数，用于推导 cb 分量缩放函数的输入索引。)
    * - __u8
      - ``cr_luma_mult``
      - Represents a multiplier for the average luma component used in
        derivation of the input index to the cr component scaling function.
        (表示平均亮度分量的乘数，用于推导 cr 分量缩放函数的输入索引。)
    * - __u16
      - ``cb_offset``
      - Represents an offset used in derivation of the input index to the
        cb component scaling function.
        (表示一个偏移量，用于推导 cb 分量缩放函数的输入索引。)
    * - __u16
      - ``cr_offset``
      - Represents an offset used in derivation of the input index to the
        cr component scaling function.
        (表示一个偏移量，用于推导 cr 分量缩放函数的输入索引。)
    * - __u8
      - ``reserved[4]``
      - Applications and drivers must set this to zero.
        (应用程序和驱动程序必须将其设置为 0。)

.. _av1_film_grain_flags:

``AV1 Film Grain Flags`` (AV1 胶片颗粒标志)

.. cssclass:: longtable

.. flat-table::
    :header-rows:  0
    :stub-columns: 0
    :widths:       1 1 2

    * - ``V4L2_AV1_FILM_GRAIN_FLAG_APPLY_GRAIN``
      - 0x00000001
      - If set, specifies that film grain should be added to this frame. If not
        set, specifies that film grain should not be added.
        (如果置位，指定应向此帧添加胶片颗粒。如果未置位，指定不应添加胶片
        颗粒。)
    * - ``V4L2_AV1_FILM_GRAIN_FLAG_UPDATE_GRAIN``
      - 0x00000002
      - If set, means that a new set of parameters should be sent. If not set,
        specifies that the previous set of parameters should be used.
        (如果置位，表示应发送一组新参数。如果未置位，指定应使用上一组参数。)
    * - ``V4L2_AV1_FILM_GRAIN_FLAG_CHROMA_SCALING_FROM_LUMA``
      - 0x00000004
      - If set, specifies that the chroma scaling is inferred from the luma
        scaling.
        (如果置位，指定色度缩放由亮度缩放推断而来。)
    * - ``V4L2_AV1_FILM_GRAIN_FLAG_OVERLAP``
      - 0x00000008
      - If set, indicates that the overlap between film grain blocks shall be
        applied. If not set, indicates that the overlap between film grain blocks
        shall not be applied.
        (如果置位，指示应应用胶片颗粒块之间的重叠。如果未置位，指示不应用
        胶片颗粒块之间的重叠。)
    * - ``V4L2_AV1_FILM_GRAIN_FLAG_CLIP_TO_RESTRICTED_RANGE``
      - 0x00000010
      - If set, indicates that clipping to the restricted (studio, i.e. limited)
        range shall be applied to the sample values after adding the film grain
        (see the semantics for color_range for an explanation of studio swing).
        If not set, indicates that clipping to the full range shall be applied
        to the sample values after adding the film grain.
        (如果置位，指示添加胶片颗粒后，应对样本值应用受限（工作室，即有限）
        范围的裁剪（工作室摆幅的解释见 color_range 的语义）。如果未置位，指示
        添加胶片颗粒后，应对样本值应用全量程的裁剪。)
