# 002.搭建Linux-5.9实验环境

## 准备工作
### 实验物理机
```shell
wei@Berries-Wang:~/OPEN_SOURCE$ uname -a 
Linux Berries-Wang 5.15.0-126-generic #136~20.04.1-Ubuntu SMP Thu Nov 14 16:38:05 UTC 2024 x86_64 x86_64 x86_64 GNU/Linux
```

### 基础软件安装: 
```shell
1. wei@Berries-Wang:~/OPEN_SOURCE/Berries-Kernel/001.OPE_SPACE/BusyBox/busybox-1.37.0$ sudo apt-get install qemu libncurses5-dev gcc-arm-linux-gnueabi build-essential
```

### gcc
```txt
wei@Berries-Wang:~/OPEN_SOURCE/Berries-Kernel-2/000.SOURCE_CODE/000.LINUX-5.9$ gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-linux-gnu/9/lto-wrapper
OFFLOAD_TARGET_NAMES=nvptx-none:hsa
OFFLOAD_TARGET_DEFAULT=1
Target: x86_64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu 9.4.0-1ubuntu1~20.04.2' --with-bugurl=file:///usr/share/doc/gcc-9/README.Bugs --enable-languages=c,ada,c++,go,brig,d,fortran,objc,obj-c++,gm2 --prefix=/usr --with-gcc-major-version-only --program-suffix=-9 --program-prefix=x86_64-linux-gnu- --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-vtable-verify --enable-plugin --enable-default-pie --with-system-zlib --with-target-system-zlib=auto --enable-objc-gc=auto --enable-multiarch --disable-werror --with-arch-32=i686 --with-abi=m64 --with-multilib-list=m32,m64,mx32 --enable-multilib --with-tune=generic --enable-offload-targets=nvptx-none=/build/gcc-9-9QDOt0/gcc-9-9.4.0/debian/tmp-nvptx/usr,hsa --without-cuda-driver --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu
Thread model: posix
gcc version 9.4.0 (Ubuntu 9.4.0-1ubuntu1~20.04.2) 
```