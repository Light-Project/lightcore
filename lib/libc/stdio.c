/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <kernel.h>
#include <ctype.h>
#include <export.h>

/**
 * snprintf - format a string and place it in a buffer.
 * @buf: The buffer to place the result into.
 * @size: The size of the buffer, including the trailing null space.
 * @fmt: The format string to use.
 * @...: Arguments for the format string.
 */
int snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}
EXPORT_SYMBOL(snprintf);

/**
 * vsprintf - format a string and place it in a buffer.
 * @buf: The buffer to place the result into.
 * @fmt: The format string to use.
 * @args: Arguments for the format string.
 */
int vsprintf(char *buf, const char *fmt, va_list args)
{
    return vsnprintf(buf, INT_MAX, fmt, args);
}
EXPORT_SYMBOL(vsprintf);

/**
 * sprintf - format a string and place it in a buffer.
 * @buf: The buffer to place the result into.
 * @fmt: The format string to use.
 * @...: Arguments for the format string.
 */
int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);

    return i;
}
EXPORT_SYMBOL(sprintf);

/**
 * vscnprintf - format a string and place it in a buffer.
 * @buf: The buffer to place the result into.
 * @size: The size of the buffer, including the trailing null space.
 * @fmt: The format string to use.
 * @...: Arguments for the format string.
 */
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    int i;

    i = vsnprintf(buf, size, fmt, args);
    if (likely(i < size))
        return i;

    if (size != 0)
        return size - 1;

    return 0;
}
EXPORT_SYMBOL(vscnprintf);

/**
 * scnprintf - format a string and place it in a buffer.
 * @buf: The buffer to place the result into.
 * @size: The size of the buffer, including the trailing null space.
 * @fmt: The format string to use.
 * @...: Arguments for the format string.
 */
int scnprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vscnprintf(buf, size, fmt, args);
    va_end(args);

    return i;
}
EXPORT_SYMBOL(scnprintf);

/**
 * sscanf - unformat a buffer into a list of arguments.
 * @buf: input buffer.
 * @fmt: formatting of buffer.
 * @...: resulting arguments.
 */
int sscanf(const char *buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsscanf(buf, fmt, args);
    va_end(args);

    return i;
}
EXPORT_SYMBOL(sscanf);
