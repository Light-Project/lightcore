/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _TYPECHECK_H_
#define _TYPECHECK_H_

#define typecheck(type, object) ({          \
    type __dummy1;                          \
    typeof(object) __dummy2;                \
    (void)(&__dummy1 == &__dummy2);         \
    1;                                      \
})

#define typecheck_func(type, func) ({       \
    typeof(type) __dummy = func;            \
    (void)__dummy;                          \
    1;                                      \
})

#define typecheck_pointer(pointer) ({       \
    typeof(pointer) __dummy;                \
    (void)sizeof(*__dummy);                 \
    1;                                      \
})

#endif  /* _TYPECHECK_H_ */
