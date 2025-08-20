# 缺页异常处理
在ARM64架构里把异常分成同步异常和异步异常两种。通常异步异常指的是中断，而同步异常指的是异常

ARMv8架构中有一个与访问失效相关的寄存器——异常综合信息寄存器（Exception Syndrome Register，ESR）

ARMv8架构还提供了另外一个寄存器——失效地址寄存器（Fault Address Register，FAR）。这个寄存器保存了发生异常的虚拟地址。