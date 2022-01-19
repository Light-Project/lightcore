/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_GVM_H_
#define _MM_GVM_H_

#include <types.h>
#include <bits.h>

#ifndef __ASSEMBLY__

#define GVM_NOCACHE         BIT(0)
#define GVM_WCOMBINED       BIT(1)
#define GVM_WTHROUGH        BIT(2)

#endif  /* __ASSEMBLY__ */
#endif  /* _MM_GVM_H_ */
