/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_MMIOWB_H_
#define _ASM_GENERIC_MMIOWB_H_

#ifdef CONFIG_MMIOWB

#include <asm-generic/mmiowb_types.h>




#else
#define mmiowb_set_pending()    do { } while (0)
#define mmiowb_spin_lock()      do { } while (0)
#define mmiowb_spin_unlock()    do { } while (0)
#endif /* CONFIG_MMIOWB */
#endif /* _ASM_GENERIC_MMIOWB_H_ */
