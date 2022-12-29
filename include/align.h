/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ALIGN_H_
#define _ALIGN_H_

/**
 * align_low/high - aligned address.
 * @size: address to aligned.
 * @align: alignment size.
 */
#define align_low(size, align) ({                   \
    (size) & ~((align) - 1);                        \
})

#define align_high(size, align) ({                  \
    typeof(align) _align = (align);                 \
    ((size) + (_align - 1)) & ~(_align - 1);        \
})

#define align_ptr_low(ptr, align) ({                \
    (typeof(ptr))align_low((size_t)(ptr), align);   \
})

#define align_ptr_high(ptr, align) ({               \
    (typeof(ptr))align_high((size_t)(ptr), align);  \
})

#define align_low_adj(size, align) ({               \
    (size) = align_low(size, align);                \
})

#define align_high_adj(size, align) ({              \
    (size) = align_high(size, align);               \
})

#define align_ptr_low_adj(ptr, align) ({            \
    (ptr) = align_ptr_low(ptr, align);              \
})

#define align_ptr_high_adj(ptr, align) ({           \
    (ptr) = align_ptr_high(ptr, align);             \
})

#define align_check(size, align) (                  \
    !((size) & ((align) - 1))                       \
)

#define align_ptr_check(ptr, align) (               \
    align_check((size_t)(ptr), align)               \
)

#endif  /* _ALIGN_H_ */
