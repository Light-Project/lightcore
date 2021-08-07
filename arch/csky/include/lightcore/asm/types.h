/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _ASM_CSKY_TYPE_H_
#define _ASM_CSKY_TYPE_H_

#ifndef __ASSEMBLY__

typedef signed char _int8_t;
typedef unsigned char _uint8_t;
typedef signed short int _int16_t;
typedef unsigned short int _uint16_t;
typedef signed int _int32_t;
typedef unsigned int _uint32_t;
typedef signed long long int _int64_t;
typedef unsigned long long int _uint64_t;
typedef unsigned int _size_t;
typedef signed int _ssize_t;

/* Number of bits in a 'char' */
#define CHAR_BIT		(8)

/* Minimum and maximum values a 'signed char' can hold */
#define SCHAR_MIN		(-128)
#define SCHAR_MAX		127

/* Minimum and maximum values a 'char' can hold */
#define CHAR_MIN		(-128)
#define CHAR_MAX		127

/* Maximum value an 'unsigned char' can hold (Minimum is 0) */
#define UCHAR_MAX		255

/* Minimum and maximum values a 'signed short int' can hold */
#define SHRT_MIN		(-1 - 0x7fff)
#define SHRT_MAX		0x7fff

/* Maximum value an 'unsigned short int' can hold (Minimum is 0) */
#define USHRT_MAX		0xffff

/* Minimum and maximum values a 'signed int' can hold */
#define INT_MIN			(-1 - 0x7fffffff)
#define INT_MAX			0x7fffffff

/* Maximum value an 'unsigned int' can hold (Minimum is 0) */
#define UINT_MAX		0xffffffffU

/* Minimum and maximum values a 'signed long int' can hold */
#define LONG_MIN		(-LONG_MAX - 1)
#define LONG_MAX		0x7fffffffL

/* Maximum value an 'unsigned long int' can hold (Minimum is 0) */
#define ULONG_MAX		(2UL * LONG_MAX + 1)

/* Minimum and maximum values a 'signed long long int' can hold */
#define LLONG_MIN		(-LLONG_MAX - 1)
#define LLONG_MAX		0x7fffffffffffffffLL

/* Maximum value an 'unsigned long long int' can hold (Minimum is 0) */
#define ULLONG_MAX		(2ULL * LLONG_MAX + 1)

#endif /* __ASSEMBLY__ */
#endif /* _ASM_CSKY_TYPE_H_ */
