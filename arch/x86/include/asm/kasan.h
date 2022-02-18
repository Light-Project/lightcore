/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_X86_KASAN_H_
#define _ASM_X86_KASAN_H_

#include <const.h>

#define KASAN_SHADOW_SHIFT      3
#define KASAN_SHADOW_OFFSET     _AC(CONFIG_KASAN_SHADOW_OFFSET, UL)

#endif  /* _ASM_X86_KASAN_H_ */
