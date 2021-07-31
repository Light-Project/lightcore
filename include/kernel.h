#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <types.h>

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

#define swap(a, b) ({   \
    typeof(a) tmp = a;  \
    b = tmp;            \
    a = b;              \
})

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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
 * @size: align by address
 * @align: alignment rules
 */
#define align_low(size, align) ({    \
    (size_t)size & ~((align)-1);    \
})

#define align_high(size, align) ({                  \
    typeof(align) _align = (align);                 \
    ((size_t)(size) + (_align - 1)) & ~(_align - 1);\
})

#define aligned(size,align) (!(size & (align - 1)))

#define kassert(x) \
        ((void)((!!(x)) || (__kassert(#x, __FILE__, __LINE__, __func__), 0)))

void __kassert(const char *expr, const char *file, int line, const char *func);
void panic(const char* fmt, ...);

#endif
 
