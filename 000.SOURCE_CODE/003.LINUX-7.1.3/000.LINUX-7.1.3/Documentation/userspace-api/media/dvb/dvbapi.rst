.. SPDX-License-Identifier: GFDL-1.1-no-invariants-or-later
.. include:: <isonum.txt>

.. _dvbapi:

###################################################
Part II - Digital TV API(第二部分 - Digital TV API)
###################################################

.. note::

   This API is also known as Linux **DVB API**.
   (此 API 也被称为 Linux **DVB API** 。)

   It it was originally written to support the European digital TV
   standard (DVB), and later extended to support all digital TV standards.
   (它最初为支持欧洲数字电视标准（DVB）而编写，后来扩展为支持所有数字电视标准。)

   In order to avoid confusion, within this document, it was opted to refer to
   it, and to associated hardware as **Digital TV**.
   (为了避免混淆，在本文档中，选择把它及其相关硬件称为 **Digital TV** （数字电视）。)

   The word **DVB** is reserved to be used for:
   (**DVB** 一词保留用于：)

     - the Digital TV API version
       (e. g. DVB API version 3 or DVB API version 5);
       (Digital TV API 版本（例如 DVB API 版本 3 或 DVB API 版本 5）；)
     - digital TV data types (enums, structs, defines, etc);
       (数字电视数据类型（枚举、结构体、定义等）；)
     - digital TV device nodes (``/dev/dvb/...``);
       (数字电视设备节点（ ``/dev/dvb/...`` ）；)
     - the European DVB standard.
       (欧洲 DVB 标准。)

**Version 5.10**
(**版本 5.10**)

.. toctree::
    :caption: Table of Contents
    :maxdepth: 5
    :numbered:

    intro
    frontend
    demux
    ca
    net
    legacy_dvb_apis
    examples
    headers


**********************************
Revision and Copyright(修订与版权)
**********************************

Authors:
(作者：)

- J. K. Metzler, Ralph <rjkm@metzlerbros.de>

 - Original author of the Digital TV API documentation.
   (Digital TV API 文档的原作者。)

- O. C. Metzler, Marcus <rjkm@metzlerbros.de>

 - Original author of the Digital TV API documentation.
   (Digital TV API 文档的原作者。)

- Carvalho Chehab, Mauro <mchehab+samsung@kernel.org>

 - Ported document to Docbook XML, addition of DVBv5 API, documentation gaps fix.
   (将文档移植到 Docbook XML，加入 DVBv5 API，修复文档缺口。)

**Copyright** |copy| 2002-2003 : Convergence GmbH

**Copyright** |copy| 2009-2017 : Mauro Carvalho Chehab

**************************
Revision History(修订历史)
**************************

:revision: 2.2.0 / 2017-09-01 (*mcc*)

Most gaps between the uAPI document and the Kernel implementation
got fixed for the non-legacy API.
(非遗留 API 的 uAPI 文档与内核实现之间的大部分缺口已得到修复。)

:revision: 2.1.0 / 2015-05-29 (*mcc*)

DocBook improvements and cleanups, in order to document the system calls
on a more standard way and provide more description about the current
Digital TV API.
(改进和清理 DocBook，以便以更标准的方式记录系统调用，并提供更多关于当前 Digital TV API 的描述。)

:revision: 2.0.4 / 2011-05-06 (*mcc*)

Add more information about DVBv5 API, better describing the frontend
GET/SET props ioctl's.
(增加关于 DVBv5 API 的更多信息，更好地描述前端 GET/SET props ioctl。)


:revision: 2.0.3 / 2010-07-03 (*mcc*)

Add some frontend capabilities flags, present on kernel, but missing at
the specs.
(添加一些存在于内核但规范中缺失的前端能力标志。)


:revision: 2.0.2 / 2009-10-25 (*mcc*)

documents FE_SET_FRONTEND_TUNE_MODE and
FE_DISHETWORK_SEND_LEGACY_CMD ioctls.
(记录了 FE_SET_FRONTEND_TUNE_MODE 和 FE_DISHETWORK_SEND_LEGACY_CMD ioctl。)


:revision: 2.0.1 / 2009-09-16 (*mcc*)

Added ISDB-T test originally written by Patrick Boettcher
(加入了最初由 Patrick Boettcher 编写的 ISDB-T 测试)


:revision: 2.0.0 / 2009-09-06 (*mcc*)

Conversion from LaTex to DocBook XML. The contents is the same as the
original LaTex version.
(从 LaTex 转换为 DocBook XML。内容与原始 LaTex 版本相同。)


:revision: 1.0.0 / 2003-07-24 (*rjkm*)

Initial revision on LaTEX.
(LaTEX 上的初始修订。)
