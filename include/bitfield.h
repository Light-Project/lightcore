/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _BITFIELD_H_
#define _BITFIELD_H_

#define __bf_shf(x) (__builtin_ffsll(x) - 1)

#define FIELD_MAX(mask) ({                                  \
    (typeof(mask))((mask) >> __bf_shf(mask));               \
})

#define FIELD_FIT(mask, val) ({                             \
    !((((typeof(mask))val) << __bf_shf(mask)) & ~(mask));   \
})

#define FIELD_PREP(mask, val) ({                            \
    ((typeof(mask))(val) << __bf_shf(mask)) & (mask);       \
})

#define FIELD_GET(mask, _reg) ({                            \
    (typeof(mask))(((_reg) & (mask)) >> __bf_shf(mask));	\
})

#endif /* _BITFIELD_H_ */
