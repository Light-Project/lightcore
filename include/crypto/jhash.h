/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _CRYPTO_JHASH_H_
#define _CRYPTO_JHASH_H_

#include <types.h>
#include <bitops.h>

/* An arbitrary initial parameter */
#define JHASH_INITVAL 0xdeadbeef

/* Mix 3 32-bit values reversibly */
#define jhash_mix(a, b, c) {            \
	a -= c; a ^= rol32(c, 4);  c += b;	\
	b -= a; b ^= rol32(a, 6);  a += c;	\
	c -= b; c ^= rol32(b, 8);  b += a;	\
	a -= c; a ^= rol32(c, 16); c += b;	\
	b -= a; b ^= rol32(a, 19); a += c;	\
	c -= b; c ^= rol32(b, 4);  b += a;	\
}

/* Final mixing of 3 32-bit values (a,b,c) into c */
#define jhash_final(a, b, c) {  \
    c ^= b; c -= rol32(b, 14);  \
    a ^= c; a -= rol32(c, 11);  \
    b ^= a; b -= rol32(a, 25);  \
    c ^= b; c -= rol32(b, 16);  \
    a ^= c; a -= rol32(c, 4);   \
    b ^= a; b -= rol32(a, 14);  \
    c ^= b; c -= rol32(b, 24);  \
}

extern uint32_t jhash(const void *data, size_t length, uint32_t initval);

#endif  /* _CRYPTO_JHASH_H_ */
