.. _vkms:

================================================================
 drm/vkms Virtual Kernel Modesetting(drm/vkms 虚拟内核模式设置)
================================================================

.. kernel-doc:: drivers/gpu/drm/vkms/vkms_drv.c
   :doc: vkms (Virtual Kernel Modesetting)

Setup(设置)
===========

The VKMS driver can be setup with the following steps:
(VKMS 驱动可以通过以下步骤进行设置：)

To check if VKMS is loaded, run::

  lsmod | grep vkms

(要检查 VKMS 是否已加载，运行：)

This should list the VKMS driver. If no output is obtained, then
you need to enable and/or load the VKMS driver.
Ensure that the VKMS driver has been set as a loadable module in your
kernel config file. Do::

  make nconfig

  Go to `Device Drivers> Graphics support`

  Enable `Virtual KMS (EXPERIMENTAL)`

(这应该会列出 VKMS 驱动。如果没有得到任何输出，那么你需要启用和/或
加载 VKMS 驱动。确保在你的内核配置文件中把 VKMS 驱动设置为可加载
模块。执行：)

Compile and build the kernel for the changes to get reflected.
Now, to load the driver, use::

  sudo modprobe vkms

(编译并构建内核以使更改生效。现在，要加载驱动，使用：)

On running the lsmod command now, the VKMS driver will appear listed.
You can also observe the driver being loaded in the dmesg logs.
(现在运行 lsmod 命令时，VKMS 驱动就会出现在列表中。你也可以在 dmesg
日志中观察到驱动被加载。)

The VKMS driver has optional features to simulate different kinds of hardware,
which are exposed as module options. You can use the `modinfo` command
to see the module options for vkms::

  modinfo vkms

(VKMS 驱动具有模拟不同种类硬件的可选特性，它们以模块选项的形式暴露。
你可以使用 `modinfo` 命令查看 vkms 的模块选项：)

Module options are helpful when testing, and enabling modules
can be done while loading vkms. For example, to load vkms with cursor enabled,
use::

  sudo modprobe vkms enable_cursor=1

(模块选项在测试时很有用，可以在加载 vkms 时启用模块。例如，要加载
启用了光标的 vkms，使用：)

To disable the driver, use ::

  sudo modprobe -r vkms

(要禁用驱动，使用：)

Configuring With Configfs(通过 configfs 进行配置)
=================================================

It is possible to create and configure multiple VKMS instances via configfs.
(可以通过 configfs 创建并配置多个 VKMS 实例。)

Start by mounting configfs and loading VKMS::

  sudo mount -t configfs none /config
  sudo modprobe vkms

(先挂载 configfs 并加载 VKMS：)

Once VKMS is loaded, ``/config/vkms`` is created automatically. Each directory
under ``/config/vkms`` represents a VKMS instance, create a new one::

  sudo mkdir /config/vkms/my-vkms

(VKMS 加载后，会自动创建 ``/config/vkms``。``/config/vkms`` 下的每个
目录代表一个 VKMS 实例，创建一个新实例：)

By default, the instance is disabled::

  cat /config/vkms/my-vkms/enabled
  0

(默认情况下，实例是禁用的：)

And directories are created for each configurable item of the display pipeline::

  tree /config/vkms/my-vkms
  ├── connectors
  ├── crtcs
  ├── enabled
  ├── encoders
  └── planes

(并且会为显示管线的每个可配置项创建目录：)

To add items to the display pipeline, create one or more directories under the
available paths.
(要向显示管线添加条目，在可用路径下创建一个或多个目录。)

Start by creating one or more planes::

  sudo mkdir /config/vkms/my-vkms/planes/plane0

(先创建一个或多个 plane：)

Planes have 1 configurable attribute:
(Plane 有 1 个可配置属性：)

- type: Plane type: 0 overlay, 1 primary, 2 cursor (same values as those
  exposed by the "type" property of a plane)
  (type：Plane 类型：0 为 overlay，1 为 primary，2 为 cursor (与 plane
  的 "type" 属性暴露的值相同))

Continue by creating one or more CRTCs::

  sudo mkdir /config/vkms/my-vkms/crtcs/crtc0

(接着创建一个或多个 CRTC：)

CRTCs have 1 configurable attribute:
(CRTC 有 1 个可配置属性：)

- writeback: Enable or disable writeback connector support by writing 1 or 0
  (writeback：通过写入 1 或 0 来启用或禁用回写 (writeback) 连接器支持)

Next, create one or more encoders::

  sudo mkdir /config/vkms/my-vkms/encoders/encoder0

(接下来，创建一个或多个 encoder：)

Last but not least, create one or more connectors::

  sudo mkdir /config/vkms/my-vkms/connectors/connector0

(最后同样重要的一步，创建一个或多个 connector：)

Connectors have 1 configurable attribute:
(Connector 有 1 个可配置属性：)

- status: Connection status: 1 connected, 2 disconnected, 3 unknown (same values
  as those exposed by the "status" property of a connector)
  (status：连接状态：1 已连接，2 已断开，3 未知 (与 connector 的
  "status" 属性暴露的值相同))

To finish the configuration, link the different pipeline items::

  sudo ln -s /config/vkms/my-vkms/crtcs/crtc0 /config/vkms/my-vkms/planes/plane0/possible_crtcs
  sudo ln -s /config/vkms/my-vkms/crtcs/crtc0 /config/vkms/my-vkms/encoders/encoder0/possible_crtcs
  sudo ln -s /config/vkms/my-vkms/encoders/encoder0 /config/vkms/my-vkms/connectors/connector0/possible_encoders

(要完成配置，把不同的管线条目链接起来：)

Since at least one primary plane is required, make sure to set the right type::

  echo "1" | sudo tee /config/vkms/my-vkms/planes/plane0/type

(由于至少需要一个 primary plane，请确保设置正确的类型：)

Once you are done configuring the VKMS instance, enable it::

  echo "1" | sudo tee /config/vkms/my-vkms/enabled

(完成 VKMS 实例的配置后，启用它：)

Finally, you can remove the VKMS instance disabling it::

  echo "0" | sudo tee /config/vkms/my-vkms/enabled

(最后，你可以先禁用再移除 VKMS 实例：)

And removing the top level directory and its subdirectories::

  sudo rm /config/vkms/my-vkms/planes/*/possible_crtcs/*
  sudo rm /config/vkms/my-vkms/encoders/*/possible_crtcs/*
  sudo rm /config/vkms/my-vkms/connectors/*/possible_encoders/*
  sudo rmdir /config/vkms/my-vkms/planes/*
  sudo rmdir /config/vkms/my-vkms/crtcs/*
  sudo rmdir /config/vkms/my-vkms/encoders/*
  sudo rmdir /config/vkms/my-vkms/connectors/*
  sudo rmdir /config/vkms/my-vkms

(然后移除顶层目录及其子目录：)

Testing With IGT(使用 IGT 进行测试)
===================================

The IGT GPU Tools is a test suite used specifically for debugging and
development of the DRM drivers.
The IGT Tools can be installed from
`here <https://gitlab.freedesktop.org/drm/igt-gpu-tools>`_ .
(IGT GPU 工具是专门用于调试和开发 DRM 驱动的测试套件。IGT 工具可以从
here (https://gitlab.freedesktop.org/drm/igt-gpu-tools) 安装。)

The tests need to be run without a compositor, so you need to switch to text
only mode. You can do this by::

  sudo systemctl isolate multi-user.target

(测试需要在没有合成器 (compositor) 的情况下运行，所以你需要切换到纯
文本模式。可以这样做：)

To return to graphical mode, do::

  sudo systemctl isolate graphical.target

(要返回图形模式，执行：)

Once you are in text only mode, you can run tests using the IGT_FORCE_DRIVER
variable to specify the device filter for the driver we want to test.
IGT_FORCE_DRIVER can also be used with the run-tests.sh script to run the
tests for a specific driver::

  sudo IGT_FORCE_DRIVER="vkms" ./build/tests/<name of test>
  sudo IGT_FORCE_DRIVER="vkms" ./scripts/run-tests.sh -t <name of test>

(进入纯文本模式后，你可以使用 IGT_FORCE_DRIVER 变量来指定要测试驱动的
设备过滤器，从而运行测试。IGT_FORCE_DRIVER 也可以与 run-tests.sh 脚本
一起使用，来运行特定驱动的测试：)

For example, to test the functionality of the writeback library,
we can run the kms_writeback test::

  sudo IGT_FORCE_DRIVER="vkms" ./build/tests/kms_writeback
  sudo IGT_FORCE_DRIVER="vkms" ./scripts/run-tests.sh -t kms_writeback

(例如，要测试回写库的功能，我们可以运行 kms_writeback 测试：)

You can also run subtests if you do not want to run the entire test::

  sudo IGT_FORCE_DRIVER="vkms" ./build/tests/kms_flip --run-subtest basic-plain-flip

(如果不想运行整个测试，你也可以运行子测试：)

Testing With KUnit(使用 KUnit 进行测试)
=======================================

KUnit (Kernel unit testing framework) provides a common framework for unit tests
within the Linux kernel.
More information in ../dev-tools/kunit/index.rst .
(KUnit (内核单元测试框架) 为 Linux 内核中的单元测试提供了一个通用框架。
更多信息见 ../dev-tools/kunit/index.rst。)

To run the VKMS KUnit tests::

  tools/testing/kunit/kunit.py run --kunitconfig=drivers/gpu/drm/vkms/tests

(要运行 VKMS 的 KUnit 测试：)

TODO(待办事项)
==============

If you want to do any of the items listed below, please share your interest
with VKMS maintainers.
(如果你想做下面列出的任何一项，请向 VKMS 维护者表达你的兴趣。)

IGT better support(更好地支持 IGT)
----------------------------------

Debugging:
(调试：)

- kms_plane: some test cases are failing due to timeout on capturing CRC;
  (kms_plane：由于采集 CRC 超时，某些测试用例失败；)

Virtual hardware (vblank-less) mode:
(虚拟硬件 (无 vblank) 模式：)

- VKMS already has support for vblanks simulated via hrtimers, which can be
  tested with kms_flip test; in some way, we can say that VKMS already mimics
  the real hardware vblank. However, we also have virtual hardware that does
  not support vblank interrupt and completes page_flip events right away; in
  this case, compositor developers may end up creating a busy loop on virtual
  hardware. It would be useful to support Virtual Hardware behavior in VKMS
  because this can help compositor developers to test their features in
  multiple scenarios.
  (VKMS 已经支持通过 hrtimers 模拟的 vblank，可以用 kms_flip 测试来验证；
  在某种意义上，我们可以说 VKMS 已经模拟了真实硬件的 vblank。不过，我们
  还有不支持 vblank 中断、会立即完成 page_flip 事件的虚拟硬件；在这种
  情况下，合成器开发者最终可能会在虚拟硬件上创建忙循环。在 VKMS 中支持
  虚拟硬件行为会很有用，因为这可以帮助合成器开发者在多种场景下测试他们
  的特性。)

Add Plane Features(增加 Plane 特性)
-----------------------------------

There's lots of plane features we could add support for:
(有很多 plane 特性我们可以增加支持：)

- Add background color KMS property[Good to get started].
  (增加背景颜色 KMS 属性 [适合入门]。)

- Scaling.
  (缩放。)

- Additional buffer formats. Low/high bpp RGB formats would be interesting
  [Good to get started].
  (额外的缓冲区格式。低/高 bpp 的 RGB 格式会很有意思 [适合入门]。)

- Async updates (currently only possible on cursor plane using the legacy
  cursor api).
  (异步更新 (目前只能通过传统的光标 API 在 cursor plane 上实现)。)

For all of these, we also want to review the igt test coverage and make sure
all relevant igt testcases work on vkms. They are good options for internship
project.
(对于所有这些，我们还想审查 igt 测试覆盖情况，并确保所有相关的 igt 测试
用例都能在 vkms 上工作。它们是实习项目的好选择。)

Runtime Configuration(运行时配置)
---------------------------------

We want to be able to reconfigure vkms instance without having to reload the
module through configfs. Use/Test-cases:
(我们希望能够通过 configfs 重新配置 vkms 实例，而不必重新加载模块。
用途/测试用例：)

- Hotplug/hotremove connectors on the fly (to be able to test DP MST handling
  of compositors).
  (动态热插拔/热移除 connector (以便测试合成器的 DP MST 处理)。)

- Change output configuration: Plug/unplug screens, change EDID, allow changing
  the refresh rate.
  (更改输出配置：插入/拔出屏幕、更改 EDID、允许更改刷新率。)

Writeback support(回写支持)
---------------------------

- The writeback and CRC capture operations share the use of composer_enabled
  boolean to ensure vblanks. Probably, when these operations work together,
  composer_enabled needs to refcounting the composer state to proper work.
  [Good to get started]
  (回写与 CRC 采集操作共享使用 composer_enabled 布尔值来保证 vblank。
  当这两种操作协同工作时，可能需要把 composer_enabled 改为对 composer
  状态进行引用计数才能正常工作。[适合入门])

- Add support for cloned writeback outputs and related test cases using a
  cloned output in the IGT kms_writeback.
  (增加对克隆回写输出的支持，以及在 IGT kms_writeback 中使用克隆输出的
  相关测试用例。)

- As a v4l device. This is useful for debugging compositors on special vkms
  configurations, so that developers see what's really going on.
  (作为 v4l 设备。这对于在特殊的 vkms 配置上调试合成器很有用，让开发者
  看到真正发生了什么。)

Output Features(输出特性)
-------------------------

- Variable refresh rate/freesync support. This probably needs prime buffer
  sharing support, so that we can use vgem fences to simulate rendering in
  testing. Also needs support to specify the EDID.
  (可变刷新率/freesync 支持。这可能需要 prime 缓冲区共享支持，以便我们
  在测试中用 vgem fence 模拟渲染。还需要支持指定 EDID。)

- Add support for link status, so that compositors can validate their runtime
  fallbacks when e.g. a Display Port link goes bad.
  (增加对链路状态的支持，以便合成器在例如 Display Port 链路变差时验证
  其运行时回退。)

CRC API Improvements(改进 CRC API)
----------------------------------

- Optimize CRC computation ``compute_crc()`` and plane blending ``blend()``
  (优化 CRC 计算 ``compute_crc()`` 与 plane 混合 ``blend()`` 的实现)

Atomic Check using eBPF(使用 eBPF 进行原子检查)
-----------------------------------------------

Atomic drivers have lots of restrictions which are not exposed to userspace in
any explicit form through e.g. possible property values. Userspace can only
inquiry about these limits through the atomic IOCTL, possibly using the
TEST_ONLY flag. Trying to add configurable code for all these limits, to allow
compositors to be tested against them, would be rather futile exercise. Instead
we could add support for eBPF to validate any kind of atomic state, and
implement a library of different restrictions.
(原子驱动有许多限制，而这些限制没有以任何显式形式暴露给用户空间，例如
通过可能的属性值来暴露。用户空间只能通过原子 IOCTL 询问这些限制，并可能
使用 TEST_ONLY 标志。试图为所有这些限制添加可配置的代码，以便让合成器
可以针对它们进行测试，将是相当徒劳的做法。相反，我们可以增加对 eBPF 的
支持来验证任何类型的原子状态，并实现一个包含各种限制的库。)

This needs a bunch of features (plane compositing, multiple outputs, ...)
enabled already to make sense.
(这需要先启用一堆特性 (plane 合成、多输出等) 才有意义。)
