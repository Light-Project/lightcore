/*
 *  lightcore/lib/stdint.h
 *  defines width
 *  (C) 2020 wzl
 */

#ifndef _STDDEF_H_
#define _STDDEF_H_

#define NULL ((void *)0)
#define offsetof(type, field) ((size_t)(&((type *)0)->field))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
        
#define min(a, b) ({                \
    typeof(a) _amin = (a);          \
    typeof(b) _bmin = (b);          \
    (void)(&_amin == &_bmin);       \
    _amin < _bmin ? _amin : _bmin;  \
})

#define max(a, b) ({                \
    typeof(a) _amax = (a);          \
    typeof(b) _bmax = (b);          \
    (void)(&_amax == &_bmax);       \
    _amax > _bmax ? _amax : _bmax;  \
})

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );     \
})

/**
 * align_low/high - aligned
 * @size: Align by address
 * @align: Alignment rules
 */
#define align_low(size,align) ({    \
    (size_t)size & ~((align)-1);    \
})

#define align_high(size,align) ({                   \
    typeof(align) _align = (align);                 \
    ((size_t)(size) + (_align - 1)) & ~(_align - 1);\
})

#define DIV_ROUND_CLOSEST(x, divisor) ({        \
    typeof(divisor)__divisor = divisor;         \
    (((x)+ ((__divisor) / 2)) / (__divisor));   \
})

#define swap(a, b) ({   \
    typeof(a) tmp = a;  \
    b = tmp;            \
    a = b;              \
})

/**
 * sizeof_field(TYPE, MEMBER)
 *
 * @TYPE: The structure containing the field of interest
 * @MEMBER: The field to return the size of
 */
#define sizeof_field(TYPE, MEMBER) sizeof((((TYPE *)0)->MEMBER))

/**
 * offsetofend(TYPE, MEMBER)
 *
 * @TYPE: The type of the structure
 * @MEMBER: The member within the structure to get the end offset of
 */
#define offsetofend(TYPE, MEMBER) (offsetof(TYPE, MEMBER) + sizeof_field(TYPE, MEMBER))

#endif /* __STDDEF_H__ */
