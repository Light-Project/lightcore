/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  arch/arm/include/asm/page.h
 *
 *  Copyright (C) 1995-2003 Russell King
 */
#ifndef _ASM_ARM_PAGE_H_
#define _ASM_ARM_PAGE_H_

#include <const.h>

/* PAGE_SHIFT determines the page size */
#define PAGE_SHIFT      12
#define PAGE_SIZE       (_AC(1,UL) << PAGE_SHIFT)
#define PAGE_MASK       (~((1 << PAGE_SHIFT) - 1))

#define THREAD_SIZE_ORDER   1
#define THREAD_SIZE         (PAGE_SIZE << THREAD_SIZE_ORDER)
#define THREAD_START_SP     (THREAD_SIZE - 8)


#endif
