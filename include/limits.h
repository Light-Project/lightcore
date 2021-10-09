/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIMIT_H_
#define _LIMIT_H_

#include <types.h>

/* Maximum value an 'unsigned char' can hold (Minimum is 0) */
#define UCHAR_MAX       ((unsigned char)~0U)

/* Minimum and maximum values a 'char' can hold */
#define CHAR_MAX        ((signed char)(UCHAR_MAX >> 1))
#define CHAR_MIN        ((signed char)(-CHAR_MAX - 1))

/* Maximum value an 'unsigned short int' can hold (Minimum is 0) */
#define USHRT_MAX       ((unsigned short)~0U)

/* Minimum and maximum values a 'signed short int' can hold */
#define SHRT_MIN        ((signed short)(USHRT_MAX >> 1))
#define SHRT_MAX        ((signed short)(-SHRT_MIN - 1))

/* Maximum value an 'unsigned int' can hold (Minimum is 0) */
#define UINT_MAX        ((unsigned int)~0U)

/* Minimum and maximum values a 'signed int' can hold */
#define INT_MAX         ((signed int)(UINT_MAX >> 1))
#define INT_MIN         ((signed int)(-INT_MAX - 1))

/* Maximum value an 'unsigned long int' can hold (Minimum is 0) */
#define ULONG_MAX       ((unsigned long)~0UL)

/* Minimum and maximum values a 'signed long int' can hold */
#define LONG_MAX        ((signed long)(ULONG_MAX >> 1))
#define LONG_MIN        ((signed long)(-LONG_MAX - 1))

/* Maximum value an 'unsigned long long int' can hold (Minimum is 0) */
#define ULLONG_MAX      ((unsigned long long)~0ULL)

/* Minimum and maximum values a 'signed long long int' can hold */
#define LLONG_MAX       ((signed long long)(ULLONG_MAX >> 1))
#define LLONG_MIN       ((signed long long)(-LLONG_MAX - 1))

/* Maximum of unsigned integral types.  */
#define UINT8_MAX       ((uint8_t)~0U)
#define UINT16_MAX      ((uint16_t)~0U)
#define UINT32_MAX      ((uint32_t)~0U)
#define UINT64_MAX      ((uint64_t)~0U)

/* Maximum of signed integral types.  */
#define INT8_MAX        ((int8_t)(UINT8_MAX >> 1))
#define INT16_MAX       ((int16_t)(UINT16_MAX >> 1))
#define INT32_MAX       ((int32_t)(UINT32_MAX >> 1))
#define INT64_MAX       ((int64_t)(UINT64_MAX >> 1))

/* Minimum of signed integral types.  */
#define INT8_MIN        ((int8_t)(-INT8_MAX- 1))
#define INT16_MIN       ((int16_t)(-INT16_MAX- 1))
#define INT32_MIN       ((int32_t)(-INT32_MAX- 1))
#define INT64_MIN       ((int64_t)(-INT64_MAX- 1))

#endif /* _LIMIT_H_ */
