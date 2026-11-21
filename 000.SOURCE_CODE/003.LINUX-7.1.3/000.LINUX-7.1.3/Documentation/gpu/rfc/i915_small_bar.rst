==================================================
I915 Small BAR RFC Section(I915 小型 BAR RFC 章节)
==================================================
Starting from DG2 we will have resizable BAR support for device local-memory(i.e
I915_MEMORY_CLASS_DEVICE), but in some cases the final BAR size might still be
smaller than the total probed_size. In such cases, only some subset of
I915_MEMORY_CLASS_DEVICE will be CPU accessible(for example the first 256M),
while the remainder is only accessible via the GPU.
(从 DG2 开始，我们将拥有针对设备本地内存 (即 I915_MEMORY_CLASS_DEVICE) 的
可调整大小 BAR 支持，但在某些情况下，最终的 BAR 大小可能仍然小于总的
probed_size。在这种情况下，只有 I915_MEMORY_CLASS_DEVICE 的某个子集是 CPU
可访问的 (例如前 256M)，其余部分只能通过 GPU 访问。)

I915_GEM_CREATE_EXT_FLAG_NEEDS_CPU_ACCESS flag(I915_GEM_CREATE_EXT_FLAG_NEEDS_CPU_ACCESS 标志)
----------------------------------------------------------------------------------------------
New gem_create_ext flag to tell the kernel that a BO will require CPU access.
This becomes important when placing an object in I915_MEMORY_CLASS_DEVICE, where
underneath the device has a small BAR, meaning only some portion of it is CPU
accessible. Without this flag the kernel will assume that CPU access is not
required, and prioritize using the non-CPU visible portion of
I915_MEMORY_CLASS_DEVICE.
(新的 gem_create_ext 标志，用于告诉内核某个 BO 将需要 CPU 访问。当把对象
放置在 I915_MEMORY_CLASS_DEVICE 中、而设备的底层只有一个小 BAR (意味着只有
其中一部分是 CPU 可访问的) 时，这就变得很重要。没有此标志，内核将假定不
需要 CPU 访问，并优先使用 I915_MEMORY_CLASS_DEVICE 中对 CPU 不可见的
部分。)

.. kernel-doc:: Documentation/gpu/rfc/i915_small_bar.h
   :functions: __drm_i915_gem_create_ext

probed_cpu_visible_size attribute(probed_cpu_visible_size 属性)
--------------------------------------------------------------------------------
New struct__drm_i915_memory_region attribute which returns the total size of the
CPU accessible portion, for the particular region. This should only be
applicable for I915_MEMORY_CLASS_DEVICE. We also report the
unallocated_cpu_visible_size, alongside the unallocated_size.
(新的 struct __drm_i915_memory_region 属性，返回特定区域中 CPU 可访问部分
的总大小。它应该只适用于 I915_MEMORY_CLASS_DEVICE。我们还在报告
unallocated_size 的同时报告 unallocated_cpu_visible_size。)

Vulkan will need this as part of creating a separate VkMemoryHeap with the
VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set, to represent the CPU visible portion,
where the total size of the heap needs to be known. It also wants to be able to
give a rough estimate of how memory can potentially be allocated.
(Vulkan 在创建一个设置了 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 的独立
VkMemoryHeap 以表示 CPU 可见部分时需要用到它，此时需要知道堆的总大小。
它还希望能够大致估算内存可能的分配方式。)

.. kernel-doc:: Documentation/gpu/rfc/i915_small_bar.h
   :functions: __drm_i915_memory_region_info

Error Capture restrictions(错误捕获限制)
------------------------------------------------
With error capture we have two new restrictions:
(对于错误捕获，我们有两个新的限制：)

    1) Error capture is best effort on small BAR systems; if the pages are not
    CPU accessible, at the time of capture, then the kernel is free to skip
    trying to capture them.
    (在小型 BAR 系统上，错误捕获是尽力而为的；如果在捕获时页面不是 CPU
    可访问的，内核可以自行决定跳过捕获它们的尝试。)

    2) On discrete and newer integrated platforms we now reject error capture
    on recoverable contexts. In the future the kernel may want to blit during
    error capture, when for example something is not currently CPU accessible.
    (在独立显卡和较新的集成平台上，我们现在拒绝在可恢复上下文上进行
    错误捕获。将来，当某些内容当前不是 CPU 可访问时，内核可能希望在错误
    捕获期间进行 blit 操作。)
