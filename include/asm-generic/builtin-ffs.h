/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUILTIN_FFS_H_
#define _ASM_GENERIC_BUILTIN_FFS_H_

#define ffs ffs
static __always_inline unsigned int ffs(unsigned long value)
{
	return __builtin_ffsl(value);
}

#endif  /* _ASM_GENERIC_BUILTIN_FFS_H_ */
