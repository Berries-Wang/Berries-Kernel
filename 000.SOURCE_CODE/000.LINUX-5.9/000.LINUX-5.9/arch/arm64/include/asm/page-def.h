/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Based on arch/arm/include/asm/page.h
 *
 * Copyright (C) 1995-2003 Russell King
 * Copyright (C) 2017 ARM Ltd.
 */
#ifndef __ASM_PAGE_DEF_H
#define __ASM_PAGE_DEF_H

#include <linux/const.h>

/* PAGE_SHIFT determines the page size */
/* CONT_SHIFT determines the number of pages which can be tracked together  */

/**
 * CONFIG_ARM64_PAGE_SHIFT 定义在 arch/arm64/Kconfig ， 即在编译内核时配置
 *
 * config ARM64_PAGE_SHIFT
 * 	int
 *	default 16 if ARM64_64K_PAGES
 *	default 14 if ARM64_16K_PAGES
 *	default 12
 *
 *   这意味着 ARM64 支持三种页面大小配置：
 * 4KB 页面（默认）：
 *   CONFIG_ARM64_PAGE_SHIFT=12 (因为 2^12 = 4096)
 * 16KB 页面：
 *    CONFIG_ARM64_PAGE_SHIFT=14 (因为 2^14 = 16384)
 *   需要选择 ARM64_16K_PAGES 配置选项
 * 64KB 页面：
 *    CONFIG_ARM64_PAGE_SHIFT=16 (因为 2^16 = 65536)
 *    需要选择 ARM64_64K_PAGES 配置选项
 */
#define PAGE_SHIFT		CONFIG_ARM64_PAGE_SHIFT
#define CONT_SHIFT		CONFIG_ARM64_CONT_SHIFT
#define PAGE_SIZE		(_AC(1, UL) << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))

#define CONT_SIZE		(_AC(1, UL) << (CONT_SHIFT + PAGE_SHIFT))
#define CONT_MASK		(~(CONT_SIZE-1))

#endif /* __ASM_PAGE_DEF_H */
