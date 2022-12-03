/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _STDIO_H_
#define _STDIO_H_

#include <types.h>
#include <stdarg.h>

extern __scanf(2, 0) int vsscanf(const char *buf, const char *fmt, va_list args);
extern __scanf(2, 3) int sscanf(const char *buf, const char *fmt, ...);
extern __printf(2, 3) int sprintf(char *buf, const char *fmt, ...);
extern __printf(3, 4) int snprintf(char *buf, size_t size, const char *fmt, ...);
extern __printf(3, 4) int scnprintf(char *buf, size_t size, const char *fmt, ...);
extern __printf(2, 0) int vsprintf(char *buf, const char *fmt, va_list args);
extern __printf(3, 0) int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
extern __printf(3, 0) int vscnprintf(char *buf, size_t size, const char *fmt, va_list args);

extern __printf(2, 0) __malloc char *kvasprintf(gfp_t flags, const char *fmt, va_list args);
extern __printf(2, 0) __malloc const char *kvasprintf_const(gfp_t flags, const char *fmt, va_list args);
extern __printf(2, 3) __malloc char *kasprintf(gfp_t gfp, const char *fmt, ...);
extern __printf(2, 3) __malloc const char *kasprintf_const(gfp_t gfp, const char *fmt, ...);
extern __printf(1, 0) __malloc char *vasprintf(const char *fmt, va_list args);
extern __printf(1, 0) __malloc const char *vasprintf_const(const char *fmt, va_list args);
extern __printf(1, 2) __malloc char *asprintf(const char *fmt, ...);
extern __printf(1, 2) __malloc const char *asprintf_const(const char *fmt, ...);

#endif /* _STDIO_H_ */
