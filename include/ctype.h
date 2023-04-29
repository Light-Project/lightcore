/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CTYPE_H_
#define _CTYPE_H_

enum ctype_num {
    __CTYPE_UPPER = 0,
    __CTYPE_LOWER,
    __CTYPE_CNTRL,
    __CTYPE_PUNCT,
    __CTYPE_BLANK,
    __CTYPE_DIGIT,
    __CTYPE_HEXDG,
    __CTYPE_SPACE,
    __CTYPE_HDSPA,

    CTYPE_UPPER = (1UL << __CTYPE_UPPER),
    CTYPE_LOWER = (1UL << __CTYPE_LOWER),
    CTYPE_CNTRL = (1UL << __CTYPE_CNTRL),
    CTYPE_PUNCT = (1UL << __CTYPE_PUNCT),
    CTYPE_BLANK = (1UL << __CTYPE_BLANK),
    CTYPE_DIGIT = (1UL << __CTYPE_DIGIT),
    CTYPE_HEXDG = (1UL << __CTYPE_HEXDG),
    CTYPE_SPACE = (1UL << __CTYPE_SPACE),
    CTYPE_HDSPA = (1UL << __CTYPE_HDSPA),
};

extern const unsigned short ctype_table[256];

static __attribute_const__ __always_inline
int isalpha_const(int c)
{
    return (((unsigned char)c | 32) - 'a') < 26;
}

static __attribute_const__ __always_inline
int isdigit_const(int c)
{
    return ((unsigned char)c - '0') < 10;
}

static __attribute_const__ __always_inline
int isblank_const(int c)
{
    return c == ' ' || c == '\t';
}

static __attribute_const__ __always_inline
int iscntrl_const(int c)
{
    return ((unsigned char)c < 0x20) || (c == 0x7f);
}

static __attribute_const__ __always_inline
int isgraph_const(int c)
{
    return ((unsigned char)c - 0x21) < 0x5e;
}

static __attribute_const__ __always_inline
int islower_const(int c)
{
    return ((unsigned char)c - 'a') < 26;
}

static __attribute_const__ __always_inline
int isprint_const(int c)
{
    return ((unsigned char)c - 0x20) < 0x5f;
}

static __attribute_const__ __always_inline
int isspace_const(int c)
{
    return (c == ' ') || ((unsigned char)c - '\t' < 5);
}

static __attribute_const__ __always_inline
int isupper_const(int c)
{
    return ((unsigned char)c - 'A') < 26;
}

static __attribute_const__ __always_inline
int isalnum_const(int c)
{
    return isalpha_const(c) || isdigit_const(c);
}

static __attribute_const__ __always_inline
int ispunct_const(int c)
{
    return isgraph_const(c) && !isalnum_const(c);
}

static __attribute_const__ __always_inline
int isxdigit_const(int c)
{
    return isdigit_const(c) || (((unsigned char)c | 32) - 'a' < 6);
}

#define GENERIC_CTYPE_DYNAMIC(name, bitflags)   \
static inline __attribute_const__               \
int name##_dynamic(int c)                       \
{                                               \
    return !!(ctype_table[(int)                 \
             (unsigned char)c] & (bitflags));   \
}

GENERIC_CTYPE_DYNAMIC(isalnum, CTYPE_UPPER | CTYPE_LOWER | CTYPE_DIGIT)
GENERIC_CTYPE_DYNAMIC(isalpha, CTYPE_UPPER | CTYPE_LOWER)
GENERIC_CTYPE_DYNAMIC(isblank, CTYPE_BLANK)
GENERIC_CTYPE_DYNAMIC(iscntrl, CTYPE_CNTRL)
GENERIC_CTYPE_DYNAMIC(isdigit, CTYPE_DIGIT)
GENERIC_CTYPE_DYNAMIC(isgraph, CTYPE_PUNCT | CTYPE_UPPER | CTYPE_LOWER | CTYPE_DIGIT)
GENERIC_CTYPE_DYNAMIC(islower, CTYPE_LOWER)
GENERIC_CTYPE_DYNAMIC(isprint, CTYPE_PUNCT | CTYPE_UPPER | CTYPE_LOWER | CTYPE_DIGIT | CTYPE_HDSPA)
GENERIC_CTYPE_DYNAMIC(ispunct, CTYPE_PUNCT)
GENERIC_CTYPE_DYNAMIC(isspace, CTYPE_SPACE)
GENERIC_CTYPE_DYNAMIC(isupper, CTYPE_UPPER)
GENERIC_CTYPE_DYNAMIC(isxdigit, CTYPE_DIGIT | CTYPE_HEXDG)

#define isalnum(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? isalnum_const(__c)        \
    : isalnum_dynamic(__c);     \
})

#define isalpha(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? isalpha_const(__c)        \
    : isalpha_dynamic(__c);     \
})

#define isblank(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? isblank_const(__c)        \
    : isblank_dynamic(__c);     \
})

#define iscntrl(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? iscntrl_const(__c)        \
    : iscntrl_dynamic(__c);     \
})

#define isdigit(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? isdigit_const(__c)        \
    : isdigit_dynamic(__c);     \
})

#define isgraph(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? isgraph_const(__c)        \
    : isgraph_dynamic(__c);     \
})

#define islower(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? islower_const(__c)        \
    : islower_dynamic(__c);     \
})

#define isprint(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? isprint_const(__c)        \
    : isprint_dynamic(__c);     \
})

#define ispunct(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? ispunct_const(__c)        \
    : ispunct_dynamic(__c);     \
})

#define isspace(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? isspace_const(__c)        \
    : isspace_dynamic(__c);     \
})

#define isupper(c) ({           \
    int __c = (int)(c);         \
    __builtin_constant_p(__c)   \
    ? isupper_const(__c)        \
    : isupper_dynamic(__c); \
})

#define isxdigit(c) ({          \
     int __c = (int)(c);        \
    __builtin_constant_p(__c)   \
    ? isxdigit_const(__c)       \
    : isxdigit_dynamic(__c);    \
})

static inline int isascii(int c)
{
    return (unsigned char)c <= 0x7f;
}

static inline int toascii(int c)
{
    return (unsigned char)c & 0x7f;
}

static inline int tolower(int c)
{
    if (isupper(c))
        return (unsigned char)c | 32;
    return (unsigned char)c;
}

static inline int toupper(int c)
{
    if (islower(c))
        return (unsigned char)c & 0x5f;
    return (unsigned char)c;
}

#endif /* _CTYPE_H_ */
