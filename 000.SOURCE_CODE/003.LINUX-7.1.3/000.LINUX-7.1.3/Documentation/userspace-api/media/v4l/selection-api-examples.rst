.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

**************
Examples(示例)
**************

(A video capture device is assumed; change
``V4L2_BUF_TYPE_VIDEO_CAPTURE`` for other devices; change target to
``V4L2_SEL_TGT_COMPOSE_*`` family to configure composing area)
(假设为视频捕获设备；针对其他设备请更换 ``V4L2_BUF_TYPE_VIDEO_CAPTURE``
；将 target 改为 ``V4L2_SEL_TGT_COMPOSE_*`` 系列，以配置合成区域。)

Example: Resetting the cropping parameters(示例：重置裁剪参数)
==============================================================

.. code-block:: c

	struct v4l2_selection sel = {
	    .type = V4L2_BUF_TYPE_VIDEO_CAPTURE,
	    .target = V4L2_SEL_TGT_CROP_DEFAULT,
	};
	ret = ioctl(fd, VIDIOC_G_SELECTION, &sel);
	if (ret)
	    exit(-1);
	sel.target = V4L2_SEL_TGT_CROP;
	ret = ioctl(fd, VIDIOC_S_SELECTION, &sel);
	if (ret)
	    exit(-1);

Setting a composing area on output of size of *at most* half of limit
placed at a center of a display.
(在输出上设置一个大小 *至多* 为限制值一半、且位于显示器中心的合成区
域。)

Example: Simple downscaling(示例：简单降采样)
=============================================

.. code-block:: c

	struct v4l2_selection sel = {
	    .type = V4L2_BUF_TYPE_VIDEO_OUTPUT,
	    .target = V4L2_SEL_TGT_COMPOSE_BOUNDS,
	};
	struct v4l2_rect r;

	ret = ioctl(fd, VIDIOC_G_SELECTION, &sel);
	if (ret)
	    exit(-1);
	/* setting smaller compose rectangle */
	r.width = sel.r.width / 2;
	r.height = sel.r.height / 2;
	r.left = sel.r.width / 4;
	r.top = sel.r.height / 4;
	sel.r = r;
	sel.target = V4L2_SEL_TGT_COMPOSE;
	sel.flags = V4L2_SEL_FLAG_LE;
	ret = ioctl(fd, VIDIOC_S_SELECTION, &sel);
	if (ret)
	    exit(-1);

A video output device is assumed; change ``V4L2_BUF_TYPE_VIDEO_OUTPUT``
for other devices
(假设为视频输出设备；针对其他设备请更换
``V4L2_BUF_TYPE_VIDEO_OUTPUT`` 。)

Example: Querying for scaling factors(示例：查询缩放因子)
=========================================================

.. code-block:: c

	struct v4l2_selection compose = {
	    .type = V4L2_BUF_TYPE_VIDEO_OUTPUT,
	    .target = V4L2_SEL_TGT_COMPOSE,
	};
	struct v4l2_selection crop = {
	    .type = V4L2_BUF_TYPE_VIDEO_OUTPUT,
	    .target = V4L2_SEL_TGT_CROP,
	};
	double hscale, vscale;

	ret = ioctl(fd, VIDIOC_G_SELECTION, &compose);
	if (ret)
	    exit(-1);
	ret = ioctl(fd, VIDIOC_G_SELECTION, &crop);
	if (ret)
	    exit(-1);

	/* computing scaling factors */
	hscale = (double)compose.r.width / crop.r.width;
	vscale = (double)compose.r.height / crop.r.height;
