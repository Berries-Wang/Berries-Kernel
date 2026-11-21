======================================
 dGPU firmware flashing(dGPU 固件烧写)
======================================

IFWI(集成固件镜像)
------------------
Flashing the dGPU integrated firmware image (IFWI) is supported by GPUs that
use the PSP to orchestrate the update (Navi3x or newer GPUs).
For supported GPUs, `amdgpu` will export a series of sysfs files that can be
used for the flash process.
(使用 PSP 来统筹更新的 GPU（Navi3x 或更新的 GPU）支持烧写 dGPU 集成固件镜像
(IFWI)。对于受支持的 GPU，`amdgpu` 会导出一系列 sysfs 文件供烧写过程使用。)

The IFWI flash process is:
(IFWI 烧写流程如下：)

1. Ensure the IFWI image is intended for the dGPU on the system.
   (确保 IFWI 镜像适用于系统中的这块 dGPU。)
2. "Write" the IFWI image to the sysfs file `psp_vbflash`. This will stage the IFWI in memory.
   (把 IFWI 镜像"写入" sysfs 文件 `psp_vbflash`。这会将 IFWI 暂存在内存中。)
3. "Read" from the `psp_vbflash` sysfs file to initiate the flash process.
   (从 `psp_vbflash` sysfs 文件"读取"以启动烧写过程。)
4. Poll the `psp_vbflash_status` sysfs file to determine when the flash process completes.
   (轮询 `psp_vbflash_status` sysfs 文件，以确定烧写过程何时完成。)

USB-C PD F/W(USB-C PD 固件)
---------------------------
On GPUs that support flashing an updated USB-C PD firmware image, the process
is done using the `usbc_pd_fw` sysfs file.
(在支持烧写更新版 USB-C PD 固件镜像的 GPU 上，该过程通过 `usbc_pd_fw` sysfs
文件完成。)

* Reading the file will provide the current firmware version.
  (读取该文件可获得当前固件版本。)
* Writing the name of a firmware payload stored in `/lib/firmware/amdgpu` to the sysfs file will initiate the flash process.
  (把存放在 `/lib/firmware/amdgpu` 中的固件负载名称写入该 sysfs 文件将启动
  烧写过程。)

The firmware payload stored in `/lib/firmware/amdgpu` can be named any name
as long as it doesn't conflict with other existing binaries that are used by
`amdgpu`.
(存放在 `/lib/firmware/amdgpu` 中的固件负载可以任意命名，只要不与 `amdgpu`
所使用的其他现有二进制文件冲突即可。)

sysfs files(sysfs 文件)
-----------------------
.. kernel-doc:: drivers/gpu/drm/amd/amdgpu/amdgpu_psp.c
