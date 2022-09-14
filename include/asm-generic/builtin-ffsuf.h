/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_BUILTIN_FFSUF_H_
#define _ASM_GENERIC_BUILTIN_FFSUF_H_

#define ffsuf ffsuf
static __always_inline unsigned int ffsuf(unsigned long value)
{
	return __builtin_ctzl(value);
}

#endif  /* _ASM_GENERIC_BUILTIN_FFSUF_H_ */
