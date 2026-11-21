=====================================================================================
Platform Profile Selection (e.g. /sys/firmware/acpi/platform_profile)(平台配置档选择)
=====================================================================================

On modern systems the platform performance, temperature, fan and other
hardware related characteristics are often dynamically configurable. The
platform configuration is often automatically adjusted to the current
conditions by some automatic mechanism (which may very well live outside
the kernel).
(在现代系统上，平台的性能、温度、风扇等硬件相关特性通常是可动态配置
的。平台配置通常由某种自动机制 (很可能位于内核之外) 根据当前状况自动
调整。)

These auto platform adjustment mechanisms often can be configured with
one of several platform profiles, with either a bias towards low power
operation or towards performance.
(这些自动平台调整机制通常可以配置为若干平台配置档 (platform profile)
之一，或偏向低功耗运行，或偏向性能。)

The purpose of the platform_profile attribute is to offer a generic sysfs
API for selecting the platform profile of these automatic mechanisms.
(platform_profile 属性的目的在于提供一个通用的 sysfs API，用于选择这
些自动机制的平台配置档。)

Note that this API is only for selecting the platform profile, it is
NOT a goal of this API to allow monitoring the resulting performance
characteristics. Monitoring performance is best done with device/vendor
specific tools, e.g. turbostat.
(注意，此 API 仅用于选择平台配置档；允许监视由此产生的性能特性并不是
此 API 的目标。监视性能最好用设备/厂商专用工具完成，例如 turbostat。)

Specifically, when selecting a high performance profile the actual achieved
performance may be limited by various factors such as: the heat generated
by other components, room temperature, free air flow at the bottom of a
laptop, etc. It is explicitly NOT a goal of this API to let userspace know
about any sub-optimal conditions which are impeding reaching the requested
performance level.
(具体而言，当选择高性能配置档时，实际达到的性能可能受多种因素限制，
例如：其他组件产生的热量、室温、笔记本底部的空气流通情况等。让用户
空间了解任何妨碍达到所请求性能水平的次优条件，明确不是此 API 的目
标。)

Since numbers on their own cannot represent the multiple variables that a
profile will adjust (power consumption, heat generation, etc) this API
uses strings to describe the various profiles. To make sure that userspace
gets a consistent experience the sysfs-platform_profile ABI document defines
a fixed set of profile names. Drivers *must* map their internal profile
representation onto this fixed set.
(由于数字本身无法表示一个配置档要调整的多个变量 (功耗、发热等)，此
API 使用字符串来描述各种配置档。为确保用户空间获得一致的体验，
sysfs-platform_profile ABI 文档定义了一组固定的配置档名称。驱动程序
*必须* 把其内部的配置档表示映射到这组固定名称上。)

If there is no good match when mapping then a new profile name may be
added. Drivers which wish to introduce new profile names must:
(如果映射时没有好的匹配，可以添加新的配置档名称。希望引入新配置档名
称的驱动程序必须：)

 1. Explain why the existing profile names cannot be used.
    (解释为什么现有的配置档名称不能使用。)
 2. Add the new profile name, along with a clear description of the
    expected behaviour, to the sysfs-platform_profile ABI documentation.
    (把新的配置档名称连同对预期行为的清晰描述，添加到
    sysfs-platform_profile ABI 文档中。)

"Custom" profile support("Custom" 配置档支持)
=============================================
The platform_profile class also supports profiles advertising a "custom"
profile. This is intended to be set by drivers when the settings in the
driver have been modified in a way that a standard profile doesn't represent
the current state.
(platform_profile 类还支持声明 "custom" 配置档。当驱动程序中的设置被
修改成标准配置档无法代表当前状态时，驱动程序应设置此值。)

Multiple driver support(多驱动支持)
===================================
When multiple drivers on a system advertise a platform profile handler, the
platform profile handler core will only advertise the profiles that are
common between all drivers to the ``/sys/firmware/acpi`` interfaces.
(当系统上有多个驱动程序声明了平台配置档 handler 时，平台配置档
handler 核心只会向 ``/sys/firmware/acpi`` 接口通告所有驱动程序共同支
持的配置档。)

This is to ensure there is no ambiguity on what the profile names mean when
all handlers don't support a profile.
(这是为了确保在并非所有 handler 都支持某个配置档时，配置档名称的含义
不存在歧义。)

Individual drivers will register a 'platform_profile' class device that has
similar semantics as the ``/sys/firmware/acpi/platform_profile`` interface.
(各个驱动程序会注册一个语义类似于 ``/sys/firmware/acpi/platform_profile``
接口的 'platform_profile' 类设备。)

To discover which driver is associated with a platform profile handler the
user can read the ``name`` attribute of the class device.
(要发现哪个驱动程序与某个平台配置档 handler 相关联，用户可以读取该类
设备的 ``name`` 属性。)

To discover available profiles from the class interface the user can read the
``choices`` attribute.
(要从类接口发现可用的配置档，用户可以读取 ``choices`` 属性。)

If a user wants to select a profile for a specific driver, they can do so
by writing to the ``profile`` attribute of the driver's class device.
(如果用户想为特定驱动程序选择配置档，可以通过写入该驱动程序类设备的
``profile`` 属性来实现。)

This will allow users to set different profiles for different drivers on the
same system. If the selected profile by individual drivers differs the
platform profile handler core will display the profile 'custom' to indicate
that the profiles are not the same.
(这将允许用户在同一系统上为不同驱动程序设置不同的配置档。如果各驱动
程序所选的配置档不同，平台配置档 handler 核心将显示配置档 'custom'，
以表示各配置档并不相同。)

While the ``platform_profile`` attribute has the value ``custom``, writing a
common profile from ``platform_profile_choices`` to the platform_profile
attribute of the platform profile handler core will set the profile for all
drivers.
(当 ``platform_profile`` 属性的值为 ``custom`` 时，把
``platform_profile_choices`` 中的一个公共配置档写入平台配置档 handler
核心的 platform_profile 属性，将为所有驱动程序设置该配置档。)
