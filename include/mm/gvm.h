/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_GVM_H_
#define _MM_GVM_H_

#include <types.h>
#include <bits.h>

enum gvm_flags {
    __GVM_NOCACHE = 0,
    __GVM_WCOMBINED,
    __GVM_WTHROUGH,
    __GVM_WRPROTECT,
    __GVM_NOEXECUTE,
};

#define GVM_NOCACHE         BIT(__GVM_NOCACHE)
#define GVM_WCOMBINED       BIT(__GVM_WCOMBINED)
#define GVM_WTHROUGH        BIT(__GVM_WTHROUGH)
#define GVM_WRPROTECT       BIT(__GVM_WRPROTECT)
#define GVM_NOEXECUTE       BIT(__GVM_NOEXECUTE)

#endif /* _MM_GVM_H_ */
