.. SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause)

Building libbpf(构建 libbpf)
============================

libelf and zlib are internal dependencies of libbpf and thus are required to link
against and must be installed on the system for applications to work.
pkg-config is used by default to find libelf, and the program called
can be overridden with PKG_CONFIG.
(libelf 和 zlib 是 libbpf 的内部依赖，因此链接时需要它们，且系统上必须安装
这些库应用程序才能工作。默认使用 pkg-config 查找 libelf，所调用的程序可用
PKG_CONFIG 覆盖。)

If using pkg-config at build time is not desired, it can be disabled by
setting NO_PKG_CONFIG=1 when calling make.
(如果构建时不希望使用 pkg-config，可以在调用 make 时设置 NO_PKG_CONFIG=1
来禁用它。)

To build both static libbpf.a and shared libbpf.so:
(要同时构建静态库 libbpf.a 与共享库 libbpf.so：)

.. code-block:: bash

    $ cd src
    $ make

To build only static libbpf.a library in directory build/ and install them
together with libbpf headers in a staging directory root/:
(要只在 build/ 目录中构建静态库 libbpf.a，并将其与 libbpf 头文件一起安装到
暂存目录 root/ 中：)

.. code-block:: bash

    $ cd src
    $ mkdir build root
    $ BUILD_STATIC_ONLY=y OBJDIR=build DESTDIR=root make install

To build both static libbpf.a and shared libbpf.so against a custom libelf
dependency installed in /build/root/ and install them together with libbpf
headers in a build directory /build/root/:
(要针对安装在 /build/root/ 中的自定义 libelf 依赖同时构建静态库 libbpf.a 与
共享库 libbpf.so，并将它们与 libbpf 头文件一起安装到构建目录 /build/root/
中：)

.. code-block:: bash

    $ cd src
    $ PKG_CONFIG_PATH=/build/root/lib64/pkgconfig DESTDIR=/build/root make
