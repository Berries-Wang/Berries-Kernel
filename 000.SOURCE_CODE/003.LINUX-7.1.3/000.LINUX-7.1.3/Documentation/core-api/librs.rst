===================================================================
Reed-Solomon Library Programming Interface(Reed-Solomon 库编程接口)
===================================================================

:Author: Thomas Gleixner

(作者: Thomas Gleixner)

Introduction(简介)
==================

The generic Reed-Solomon Library provides encoding, decoding and error
correction functions.
(通用 Reed-Solomon 库提供编码、解码与纠错功能。)

Reed-Solomon codes are used in communication and storage applications to
ensure data integrity.
(Reed-Solomon 码被用于通信和存储应用中，以确保数据完整性。)

This documentation is provided for developers who want to utilize the
functions provided by the library.
(本文档面向想要使用该库所提供函数的开发者。)

Known Bugs And Assumptions(已知缺陷与假设)
==========================================

None.
(无。)

Usage(用法)
===========

This chapter provides examples of how to use the library.
(本章提供如何使用该库的示例。)

Initializing(初始化)
--------------------

The init function init_rs returns a pointer to an rs decoder structure,
which holds the necessary information for encoding, decoding and error
correction with the given polynomial. It either uses an existing
matching decoder or creates a new one. On creation all the lookup tables
for fast en/decoding are created. The function may take a while, so make
sure not to call it in critical code paths.

::

    /* the Reed Solomon control structure */
    static struct rs_control *rs_decoder;

    /* Symbolsize is 10 (bits)
     * Primitive polynomial is x^10+x^3+1
     * first consecutive root is 0
     * primitive element to generate roots = 1
     * generator polynomial degree (number of roots) = 6
     */
    rs_decoder = init_rs (10, 0x409, 0, 1, 6);

(初始化函数 init_rs 返回一个指向 rs 解码器结构的指针，该结构保存着用给定多项式进行编码、解码和纠错所需的全部信息。它要么复用一个现有的匹配解码器，要么创建一个新的。创建时会生成所有用于快速编码/解码的查找表。该函数可能耗时较长，因此请确保不要在关键代码路径中调用它。)


Encoding(编码)
--------------

The encoder calculates the Reed-Solomon code over the given data length
and stores the result in the parity buffer. Note that the parity buffer
must be initialized before calling the encoder.
(编码器对给定长度的数据计算 Reed-Solomon 码，并把结果存入奇偶校验缓冲区。注意，调用编码器之前必须先初始化奇偶校验缓冲区。)

The expanded data can be inverted on the fly by providing a non-zero
inversion mask. The expanded data is XOR'ed with the mask. This is used
e.g. for FLASH ECC, where the all 0xFF is inverted to an all 0x00. The
Reed-Solomon code for all 0x00 is all 0x00. The code is inverted before
storing to FLASH so it is 0xFF too. This prevents that reading from an
erased FLASH results in ECC errors.
(通过提供一个非零的反转掩码，可以在处理过程中对扩展后的数据就地取反：扩展数据会与该掩码做 XOR。例如 FLASH ECC 就采用这种方法，把全 0xFF 反转为全 0x00——而全 0x00 的 Reed-Solomon 码仍是全 0x00。该码在写入 FLASH 之前被反转，因此也是 0xFF。这样可以避免从已擦除的 FLASH 中读取时产生 ECC 错误。)

The databytes are expanded to the given symbol size on the fly. There is
no support for encoding continuous bitstreams with a symbol size != 8 at
the moment. If it is necessary it should be not a big deal to implement
such functionality.

::

    /* Parity buffer. Size = number of roots */
    uint16_t par[6];
    /* Initialize the parity buffer */
    memset(par, 0, sizeof(par));
    /* Encode 512 byte in data8. Store parity in buffer par */
    encode_rs8 (rs_decoder, data8, 512, par, 0);

(数据字节会在处理过程中就地扩展到给定的符号大小。目前尚不支持对符号大小不为 8 的连续比特流进行编码。如有必要，实现这一功能应该不是什么难事。)


Decoding(解码)
--------------

The decoder calculates the syndrome over the given data length and the
received parity symbols and corrects errors in the data.
(解码器对给定长度的数据以及接收到的奇偶校验符号计算伴随式(syndrome)，并纠正数据中的错误。)

If a syndrome is available from a hardware decoder then the syndrome
calculation is skipped.
(如果硬件解码器已经给出伴随式，则跳过伴随式计算。)

The correction of the data buffer can be suppressed by providing a
correction pattern buffer and an error location buffer to the decoder.
The decoder stores the calculated error location and the correction
bitmask in the given buffers. This is useful for hardware decoders which
use a weird bit ordering scheme.
(向解码器提供纠正模式缓冲区和错误位置缓冲区，可以抑制对数据缓冲区的直接纠正。此时解码器会把计算出的错误位置和纠正位掩码存入给定的缓冲区。这对使用特殊位序的硬件解码器很有用。)

The databytes are expanded to the given symbol size on the fly. There is
no support for decoding continuous bitstreams with a symbolsize != 8 at
the moment. If it is necessary it should be not a big deal to implement
such functionality.
(数据字节会在处理过程中就地扩展到给定的符号大小。目前尚不支持对符号大小不为 8 的连续比特流进行解码。如有必要，实现这一功能应该不是什么难事。)

Decoding with syndrome calculation, direct data correction(伴随校验子计算的直接数据纠正解码)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    /* Parity buffer. Size = number of roots */
    uint16_t par[6];
    uint8_t  data[512];
    int numerr;
    /* Receive data */
    .....
    /* Receive parity */
    .....
    /* Decode 512 byte in data8.*/
    numerr = decode_rs8 (rs_decoder, data8, par, 512, NULL, 0, NULL, 0, NULL);


Decoding with syndrome given by hardware decoder, direct data correction(校验子由硬件解码器给出的直接数据纠正解码)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

::

    /* Parity buffer. Size = number of roots */
    uint16_t par[6], syn[6];
    uint8_t  data[512];
    int numerr;
    /* Receive data */
    .....
    /* Receive parity */
    .....
    /* Get syndrome from hardware decoder */
    .....
    /* Decode 512 byte in data8.*/
    numerr = decode_rs8 (rs_decoder, data8, par, 512, syn, 0, NULL, 0, NULL);


Decoding with syndrome given by hardware decoder, no direct data correction.(校验子由硬件解码器给出的、无直接数据纠正的解码)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Note: It's not necessary to give data and received parity to the
decoder.
(注意：此时不必向解码器提供数据和接收到的奇偶校验。)

::

    /* Parity buffer. Size = number of roots */
    uint16_t par[6], syn[6], corr[8];
    uint8_t  data[512];
    int numerr, errpos[8];
    /* Receive data */
    .....
    /* Receive parity */
    .....
    /* Get syndrome from hardware decoder */
    .....
    /* Decode 512 byte in data8.*/
    numerr = decode_rs8 (rs_decoder, NULL, NULL, 512, syn, 0, errpos, 0, corr);
    for (i = 0; i < numerr; i++) {
        do_error_correction_in_your_buffer(errpos[i], corr[i]);
    }


Cleanup(清理)
-------------

The function free_rs frees the allocated resources, if the caller is
the last user of the decoder.
(如果调用者是解码器的最后一个使用者，函数 free_rs 会释放已分配的资源。)

::

    /* Release resources */
    free_rs(rs_decoder);


Structures(结构体)
==================

This chapter contains the autogenerated documentation of the structures
which are used in the Reed-Solomon Library and are relevant for a
developer.
(本章包含 Reed-Solomon 库中所使用、且与开发者相关的结构体的自动生成文档。)

.. kernel-doc:: include/linux/rslib.h
   :internal:

Public Functions Provided(提供的公共函数)
=========================================

This chapter contains the autogenerated documentation of the
Reed-Solomon functions which are exported.
(本章包含已导出的 Reed-Solomon 函数的自动生成文档。)

.. kernel-doc:: lib/reed_solomon/reed_solomon.c
   :export:

Credits(致谢)
=============

The library code for encoding and decoding was written by Phil Karn.

::

            Copyright 2002, Phil Karn, KA9Q
            May be used under the terms of the GNU General Public License (GPL)


(编码与解码的库代码由 Phil Karn 编写。)

The wrapper functions and interfaces are written by Thomas Gleixner.
(包装函数与接口由 Thomas Gleixner 编写。)

Many users have provided bugfixes, improvements and helping hands for
testing. Thanks a lot.
(许多用户提供了缺陷修复、改进以及测试上的帮助。非常感谢。)

The following people have contributed to this document:
(以下人员为本文档做出了贡献：)

Thomas Gleixner\ tglx@kernel.org
