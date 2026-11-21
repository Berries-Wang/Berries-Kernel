.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

******************
Introduction(引言)
******************

Some video capture devices can sample a subsection of a picture and
shrink or enlarge it to an image of arbitrary size. Next, the devices
can insert the image into larger one. Some video output devices can crop
part of an input image, scale it up or down and insert it at an
arbitrary scan line and horizontal offset into a video signal. We call
these abilities cropping, scaling and composing.
(一些视频捕获设备可以采样图像的一个子区域，并将其缩小或放大为任意尺寸的图像；随后设备可以把该图像插入到更大的图像中。一些视频输出设备可以裁剪输入图像的一部分，将其放大或缩小，然后插入到视频信号中任意扫描行和水平偏移处。我们把这些能力称为裁剪 (cropping)、缩放 (scaling) 和合成 (composing)。)

On a video *capture* device the source is a video signal, and the
cropping target determine the area actually sampled. The sink is an
image stored in a memory buffer. The composing area specifies which part
of the buffer is actually written to by the hardware.
(在视频捕获设备上，源是视频信号，裁剪目标决定实际采样的区域；汇聚端 (sink) 是存储在内存缓冲区中的图像。合成区域指定硬件实际写入缓冲区的哪一部分。)

On a video *output* device the source is an image in a memory buffer,
and the cropping target is a part of an image to be shown on a display.
The sink is the display or the graphics screen. The application may
select the part of display where the image should be displayed. The size
and position of such a window is controlled by the compose target.
(在视频输出设备上，源是内存缓冲区中的图像，裁剪目标是将要显示在显示器上的图像的一部分；汇聚端是显示器或图形屏幕。应用程序可以选择显示器上显示图像的那部分区域，该窗口的大小和位置由合成目标 (compose target) 控制。)

Rectangles for all cropping and composing targets are defined even if
the device does supports neither cropping nor composing. Their size and
position will be fixed in such a case. If the device does not support
scaling then the cropping and composing rectangles have the same size.
(即使设备既不支持裁剪也不支持合成，所有裁剪目标和合成目标的矩形也都有定义；在这种情况下，它们的大小和位置是固定的。如果设备不支持缩放，则裁剪矩形与合成矩形大小相同。)
