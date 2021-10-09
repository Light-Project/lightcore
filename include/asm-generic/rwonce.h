/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_GENERIC_RWONCE_H_
#define _ASM_GENERIC_RWONCE_H_

#define READ_ONCE(x) ({             \
    *(volatile typeof(x) *)&(x);    \
})

#define WRITE_ONCE(x, val) ({               \
    *(volatile typeof(x) *)&(x) = (val);    \
})

#endif  /* _ASM_GENERIC_RWONCE_H_ */
