.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. c:namespace:: V4L

.. _colorspaces:

*********************
Colorspaces(色彩空间)
*********************

'Color' is a very complex concept and depends on physics, chemistry and
biology. Just because you have three numbers that describe the 'red',
'green' and 'blue' components of the color of a pixel does not mean that
you can accurately display that color. A colorspace defines what it
actually *means* to have an RGB value of e.g. (255, 0, 0). That is,
which color should be reproduced on the screen in a perfectly calibrated
environment.
('颜色' 是一个非常复杂的概念，取决于物理、化学和生物学。仅仅拥有描述某个像素颜色的 '红'、'绿'、'蓝' 分量的三个数字，并不意味着你能够准确地显示该颜色。色彩空间定义了拥有例如 (255, 0, 0) 的 RGB 值究竟*意味着*什么，也就是说，在完美校准的环境中，屏幕上应再现哪种颜色。)

In order to do that we first need to have a good definition of color,
i.e. some way to uniquely and unambiguously define a color so that
someone else can reproduce it. Human color vision is trichromatic since
the human eye has color receptors that are sensitive to three different
wavelengths of light. Hence the need to use three numbers to describe
color. Be glad you are not a mantis shrimp as those are sensitive to 12
different wavelengths, so instead of RGB we would be using the
ABCDEFGHIJKL colorspace...
(要做到这一点，我们首先需要对颜色有一个良好的定义，即某种能够唯一且无歧义地定义颜色的方式，以便他人可以再现它。人类的色觉是三色的 (trichromatic)，因为人眼具有对三种不同波长光线敏感的颜色感受器。因此需要用三个数字来描述颜色。庆幸你不是螳螂虾，因为它们对 12 种不同的波长敏感，那样的话我们就不是使用 RGB，而是要使用 ABCDEFGHIJKL 色彩空间了...)

Color exists only in the eye and brain and is the result of how strongly
color receptors are stimulated. This is based on the Spectral Power
Distribution (SPD) which is a graph showing the intensity (radiant
power) of the light at wavelengths covering the visible spectrum as it
enters the eye. The science of colorimetry is about the relationship
between the SPD and color as perceived by the human brain.
(颜色只存在于眼睛和大脑中，是颜色感受器受刺激强弱的产物。这基于光谱功率分布 (SPD)——一张显示进入眼睛的光在覆盖可见光谱的波长上的强度(辐射功率)的图。色度学 (colorimetry) 就是关于 SPD 与人脑感知到的颜色之间关系的学科。)

Since the human eye has only three color receptors it is perfectly
possible that different SPDs will result in the same stimulation of
those receptors and are perceived as the same color, even though the SPD
of the light is different.
(由于人眼只有三种颜色感受器，完全可能出现不同的 SPD 对这些感受器产生相同的刺激，从而被感知为相同的颜色，即使光线的 SPD 并不相同。)

In the 1920s experiments were devised to determine the relationship
between SPDs and the perceived color and that resulted in the CIE 1931
standard that defines spectral weighting functions that model the
perception of color. Specifically that standard defines functions that
can take an SPD and calculate the stimulus for each color receptor.
After some further mathematical transforms these stimuli are known as
the *CIE XYZ tristimulus* values and these X, Y and Z values describe a
color as perceived by a human unambiguously. These X, Y and Z values are
all in the range [0…1].
(20 世纪 20 年代，人们设计了一些实验来确定 SPD 与感知颜色之间的关系，其结果是 CIE 1931 标准，该标准定义了模拟颜色感知的光谱加权函数。具体而言，该标准定义了可以接受 SPD 并计算每种颜色感受器刺激的函数。经过一些进一步的数学变换，这些刺激被称为 *CIE XYZ tristimulus* (三刺激值)，这些 X、Y 和 Z 值无歧义地描述了人眼感知的颜色。这些 X、Y 和 Z 值都在 [0…1] 范围内。)

The Y value in the CIE XYZ colorspace corresponds to luminance. Often
the CIE XYZ colorspace is transformed to the normalized CIE xyY
colorspace:
(CIE XYZ 色彩空间中的 Y 值对应于亮度 (luminance)。通常，CIE XYZ 色彩空间会被变换为归一化的 CIE xyY 色彩空间：)

	x = X / (X + Y + Z)

	y = Y / (X + Y + Z)

The x and y values are the chromaticity coordinates and can be used to
define a color without the luminance component Y. It is very confusing
to have such similar names for these colorspaces. Just be aware that if
colors are specified with lower case 'x' and 'y', then the CIE xyY
colorspace is used. Upper case 'X' and 'Y' refer to the CIE XYZ
colorspace. Also, y has nothing to do with luminance. Together x and y
specify a color, and Y the luminance. That is really all you need to
remember from a practical point of view. At the end of this section you
will find reading resources that go into much more detail if you are
interested.
(x 和 y 值是色度坐标，可以在没有亮度分量 Y 的情况下定义颜色。这些色彩空间的名字如此相似实在令人困惑。只需记住：如果颜色是用小写的 'x' 和 'y' 指定的，那么使用的是 CIE xyY 色彩空间。大写的 'X' 和 'Y' 指的是 CIE XYZ 色彩空间。另外，y 与亮度毫无关系。x 和 y 一起指定颜色，Y 表示亮度。从实用的角度来看，这确实就是你需要记住的全部内容。如果你感兴趣，在本节的末尾你会找到更详细介绍的阅读资料。)

A monitor or TV will reproduce colors by emitting light at three
different wavelengths, the combination of which will stimulate the color
receptors in the eye and thus cause the perception of color.
Historically these wavelengths were defined by the red, green and blue
phosphors used in the displays. These *color primaries* are part of what
defines a colorspace.
(显示器或电视通过发射三种不同波长的光来再现颜色，它们的组合会刺激眼睛中的颜色感受器，从而产生颜色感知。从历史上看，这些波长由显示器中使用的红、绿、蓝荧光粉定义。这些 *color primaries* (基色) 是定义色彩空间的一部分。)

Different display devices will have different primaries and some
primaries are more suitable for some display technologies than others.
This has resulted in a variety of colorspaces that are used for
different display technologies or uses. To define a colorspace you need
to define the three color primaries (these are typically defined as x, y
chromaticity coordinates from the CIE xyY colorspace) but also the white
reference: that is the color obtained when all three primaries are at
maximum power. This determines the relative power or energy of the
primaries. This is usually chosen to be close to daylight which has been
defined as the CIE D65 Illuminant.
(不同的显示设备会有不同的基色，而且某些基色比其他基色更适合某些显示技术。这导致出现了用于不同显示技术或用途的多种色彩空间。要定义一个色彩空间，你需要定义三个基色(通常用 CIE xyY 色彩空间中的 x、y 色度坐标定义)，还要定义白基准 (white reference)：即三个基色都处于最大功率时得到的颜色。它决定了基色的相对功率或能量。通常选择接近日光，日光被定义为 CIE D65 光源。)

To recapitulate: the CIE XYZ colorspace uniquely identifies colors.
Other colorspaces are defined by three chromaticity coordinates defined
in the CIE xyY colorspace. Based on those a 3x3 matrix can be
constructed that transforms CIE XYZ colors to colors in the new
colorspace.
(概括一下：CIE XYZ 色彩空间唯一地标识颜色。其他色彩空间由在 CIE xyY 色彩空间中定义的三个色度坐标确定。基于这些坐标，可以构造一个 3x3 矩阵，把 CIE XYZ 颜色变换为新色彩空间中的颜色。)

Both the CIE XYZ and the RGB colorspace that are derived from the
specific chromaticity primaries are linear colorspaces. But neither the
eye, nor display technology is linear. Doubling the values of all
components in the linear colorspace will not be perceived as twice the
intensity of the color. So each colorspace also defines a transfer
function that takes a linear color component value and transforms it to
the non-linear component value, which is a closer match to the
non-linear performance of both the eye and displays. Linear component
values are denoted RGB, non-linear are denoted as R'G'B'. In general
colors used in graphics are all R'G'B', except in openGL which uses
linear RGB. Special care should be taken when dealing with openGL to
provide linear RGB colors or to use the built-in openGL support to apply
the inverse transfer function.
(CIE XYZ 与由特定色度基色导出的 RGB 色彩空间都是线性色彩空间。但无论是眼睛还是显示技术都不是线性的。将线性色彩空间中所有分量的值加倍，并不会被感知为两倍的颜色强度。因此每种色彩空间还定义了一个传递函数 (transfer function)，它将线性颜色分量值变换为非线性分量值，后者与眼睛和显示器的非线性表现更为匹配。线性分量值记作 RGB，非线性分量值记作 R'G'B'。一般来说，图形中使用的颜色都是 R'G'B'，只有 openGL 使用线性 RGB 是个例外。在处理 openGL 时要特别注意，要么提供线性 RGB 颜色，要么使用 openGL 内置的支持来应用逆传递函数。)

The final piece that defines a colorspace is a function that transforms
non-linear R'G'B' to non-linear Y'CbCr. This function is determined by
the so-called luma coefficients. There may be multiple possible Y'CbCr
encodings allowed for the same colorspace. Many encodings of color
prefer to use luma (Y') and chroma (CbCr) instead of R'G'B'. Since the
human eye is more sensitive to differences in luminance than in color
this encoding allows one to reduce the amount of color information
compared to the luma data. Note that the luma (Y') is unrelated to the Y
in the CIE XYZ colorspace. Also note that Y'CbCr is often called YCbCr
or YUV even though these are strictly speaking wrong.
(定义色彩空间的最后一部分是一个把非线性 R'G'B' 变换为非线性 Y'CbCr 的函数。该函数由所谓的亮度系数 (luma coefficients) 决定。同一个色彩空间可能允许多种可能的 Y'CbCr 编码。许多颜色编码更倾向于使用亮度 (Y') 和色度 (CbCr) 而不是 R'G'B'。由于人眼对亮度差异比对颜色差异更敏感，这种编码允许相对于亮度数据减少颜色信息的数量。注意，亮度 (Y') 与 CIE XYZ 色彩空间中的 Y 无关。还要注意，Y'CbCr 通常被称为 YCbCr 或 YUV，尽管严格来说这些叫法是错误的。)

Sometimes people confuse Y'CbCr as being a colorspace. This is not
correct, it is just an encoding of an R'G'B' color into luma and chroma
values. The underlying colorspace that is associated with the R'G'B'
color is also associated with the Y'CbCr color.
(有时人们会误以为 Y'CbCr 是一种色彩空间。这是不正确的：它只是把 R'G'B' 颜色编码为亮度和色度值。与 R'G'B' 颜色相关联的底层色彩空间同样与 Y'CbCr 颜色相关联。)

The final step is how the RGB, R'G'B' or Y'CbCr values are quantized.
The CIE XYZ colorspace where X, Y and Z are in the range [0…1] describes
all colors that humans can perceive, but the transform to another
colorspace will produce colors that are outside the [0…1] range. Once
clamped to the [0…1] range those colors can no longer be reproduced in
that colorspace. This clamping is what reduces the extent or gamut of
the colorspace. How the range of [0…1] is translated to integer values
in the range of [0…255] (or higher, depending on the color depth) is
called the quantization. This is *not* part of the colorspace
definition. In practice RGB or R'G'B' values are full range, i.e. they
use the full [0…255] range. Y'CbCr values on the other hand are limited
range with Y' using [16…235] and Cb and Cr using [16…240].
(最后一步是 RGB、R'G'B' 或 Y'CbCr 值如何量化。CIE XYZ 色彩空间(其中 X、Y 和 Z 在 [0…1] 范围内)描述了人类能感知的所有颜色，但变换到另一个色彩空间会产生超出 [0…1] 范围的颜色。一旦被钳制到 [0…1] 范围内，这些颜色就无法在该色彩空间中再现了。这种钳制正是缩小色彩空间范围或色域 (gamut) 的原因。如何把 [0…1] 范围转换到 [0…255](或更高，取决于颜色深度)范围内的整数值称为量化。这*不是*色彩空间定义的一部分。实践中，RGB 或 R'G'B' 值是 full range (全范围) 的，即使用完整的 [0…255] 范围。而 Y'CbCr 值是 limited range (限幅范围) 的，Y' 使用 [16…235]，Cb 和 Cr 使用 [16…240]。)

Unfortunately, in some cases limited range RGB is also used where the
components use the range [16…235]. And full range Y'CbCr also exists
using the [0…255] range.
(遗憾的是，在某些情况下也会使用 limited range (限幅范围) RGB，即各分量使用 [16…235] 范围。也存在使用 [0…255] 范围的 full range (全范围) Y'CbCr。)

In order to correctly interpret a color you need to know the
quantization range, whether it is R'G'B' or Y'CbCr, the used Y'CbCr
encoding and the colorspace. From that information you can calculate the
corresponding CIE XYZ color and map that again to whatever colorspace
your display device uses.
(为了正确解释一个颜色，你需要知道量化范围、它是 R'G'B' 还是 Y'CbCr、所使用的 Y'CbCr 编码以及色彩空间。根据这些信息，你可以计算出对应的 CIE XYZ 颜色，然后再把它映射到你的显示设备所使用的任何色彩空间。)

The colorspace definition itself consists of the three chromaticity
primaries, the white reference chromaticity, a transfer function and the
luma coefficients needed to transform R'G'B' to Y'CbCr. While some
colorspace standards correctly define all four, quite often the
colorspace standard only defines some, and you have to rely on other
standards for the missing pieces. The fact that colorspaces are often a
mix of different standards also led to very confusing naming conventions
where the name of a standard was used to name a colorspace when in fact
that standard was part of various other colorspaces as well.
(色彩空间定义本身由三个色度基色、白基准色度、一个传递函数以及把 R'G'B' 变换为 Y'CbCr 所需的亮度系数组成。虽然一些色彩空间标准正确地定义了全部四项，但色彩空间标准通常只定义其中的一部分，你必须依靠其他标准来补齐缺失的部分。色彩空间往往是多个标准的混合体，这也导致了非常混乱的命名惯例：某个标准的名字被用来命名一个色彩空间，而实际上该标准也是其他各种色彩空间的一部分。)

If you want to read more about colors and colorspaces, then the
following resources are useful: :ref:`poynton` is a good practical
book for video engineers, :ref:`colimg` has a much broader scope and
describes many more aspects of color (physics, chemistry, biology,
etc.). The
`http://www.brucelindbloom.com <http://www.brucelindbloom.com>`__
website is an excellent resource, especially with respect to the
mathematics behind colorspace conversions. The wikipedia
`CIE 1931 colorspace <http://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_xy_chromaticity_diagram_and_the_CIE_xyY_color_space>`__
article is also very useful.
(如果你想了解更多关于颜色和色彩空间的知识，以下资源很有用：:ref:`poynton` 是一本对视频工程师非常实用的好书；:ref:`colimg` 的范围更广，描述了颜色的更多方面(物理、化学、生物学等)。
`http://www.brucelindbloom.com <http://www.brucelindbloom.com>`__ 网站是一个极佳的资源，尤其是关于色彩空间转换背后的数学。wikipedia 的
`CIE 1931 colorspace <http://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_xy_chromaticity_diagram_and_the_CIE_xyY_color_space>`__
条目也非常有用。)
