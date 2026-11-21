===============================
Free Page Reporting(空闲页上报)
===============================

Free page reporting is an API by which a device can register to receive
lists of pages that are currently unused by the system. This is useful in
the case of virtualization where a guest is then able to use this data to
notify the hypervisor that it is no longer using certain pages in memory.
(空闲页上报（free page reporting）是一种 API，设备通过它注册以接收当前系统
未使用的页的列表。这在虚拟化场景中很有用：客户机可以利用这些数据通知
hypervisor，它不再使用内存中的某些页。)

For the driver, typically a balloon driver, to use of this functionality
it will allocate and initialize a page_reporting_dev_info structure. The
field within the structure it will populate is the "report" function
pointer used to process the scatterlist. It must also guarantee that it can
handle at least PAGE_REPORTING_CAPACITY worth of scatterlist entries per
call to the function. A call to page_reporting_register will register the
page reporting interface with the reporting framework assuming no other
page reporting devices are already registered.
(驱动——通常是 balloon 驱动——要使用此功能，需要分配并初始化一个
page_reporting_dev_info 结构。它要填充的结构字段是用于处理 scatterlist 的
"report" 函数指针。它还必须保证每次调用该函数都能处理至少
PAGE_REPORTING_CAPACITY 个 scatterlist 条目。调用 page_reporting_register 会
把页上报接口注册到上报框架——前提是没有其他页上报设备已经注册。)

Once registered the page reporting API will begin reporting batches of
pages to the driver. The API will start reporting pages 2 seconds after
the interface is registered and will continue to do so 2 seconds after any
page of a sufficiently high order is freed.
(注册之后，页上报 API 会开始分批向驱动上报页。API 会在接口注册 2 秒后开始
上报页，并且在任何足够高阶的页被释放 2 秒后继续上报。)

Pages reported will be stored in the scatterlist passed to the reporting
function with the final entry having the end bit set in entry nent - 1.
While pages are being processed by the report function they will not be
accessible to the allocator. Once the report function has been completed
the pages will be returned to the free area from which they were obtained.
(被上报的页将存放在传递给上报函数的 scatterlist 中，最后一个条目（entry
nent - 1）的结束位被置位。当页正由 report 函数处理时，分配器无法访问它们。
report 函数完成后，页会被归还给它们原本所在的空闲区。)

Prior to removing a driver that is making use of free page reporting it
is necessary to call page_reporting_unregister to have the
page_reporting_dev_info structure that is currently in use by free page
reporting removed. Doing this will prevent further reports from being
issued via the interface. If another driver or the same driver is
registered it is possible for it to resume where the previous driver had
left off in terms of reporting free pages.
(在移除使用空闲页上报的驱动之前，必须调用 page_reporting_unregister，把空闲
页上报当前正在使用的 page_reporting_dev_info 结构移除。这样做可防止经由该
接口再发出更多上报。如果随后注册了另一个驱动或同一个驱动，它可以从此前驱动
停止之处继续上报空闲页。)

Alexander Duyck, Dec 04, 2019
(Alexander Duyck，2019 年 12 月 4 日)
