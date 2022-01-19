/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <types.h>
#include <string.h>
#include <stdarg.h>
#include <bits.h>

#define ZEROPAD     BIT(0)  /* pad with zero                    */
#define SIGN        BIT(1)  /* unsigned/signed long             */
#define PLUS        BIT(2)  /* show plus                        */
#define SPACE       BIT(3)  /* space if plus                    */
#define LEFT        BIT(4)  /* left justified                   */
#define SPECIAL     BIT(5)  /* 0x                               */
#define SMALL       BIT(6)  /* use 'abcdef' instead of 'ABCDEF' */

#define is_digit(c) ((c - '0') < 10)

static int skip_atoi(const char **s)
{
    int i = 0;

    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';

    return i;
}

static char* format_int(char *str, size_t num, char base, int size, int precision, int type)
{
    char tmp[32];
    char c, sign;
    const char *digits;
    int i = 0;  /* Conversion length */

    if (type & SMALL)
        digits = "0123456789abcdef";
    else
        digits = "0123456789ABCDEF";

    if (type & LEFT)
        type &= ~ZEROPAD;

    c = (type & ZEROPAD) ? '0' : ' ' ;

    if (type & SIGN && (ssize_t)num < 0) {
        sign = '-';
        num = (size_t)(-(ssize_t)num);
    } else
        sign=(type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);

    if (sign)
        size--;

    if (type & SPECIAL) {
        if (base == 16)
            size -= 2;
        else if (base==8)
            size--;
    }

    /* Convert variable to character */
    if (num == 0)
        tmp[i++] = '0';
    else while (num!=0) {
        tmp[i++] = digits[num % base];
        num /= base;
    }

    if (i > precision)
        precision = i;

    size -= precision;

    if (!(type & (ZEROPAD | LEFT)))
        while (size-- > 0)
            *str++ = ' ';

    if (sign)
        *str++ = sign;

    if (type & SPECIAL) {
        if (base == 8)
            *str++ = '0';
        else if (base == 16) {
            *str++ = '0';
            *str++ = (type & SMALL) ? 'x' : 'X';
        }
    }

    if (!(type & LEFT))
        while(size-- > 0)
            *str++ = c;

    while (i < precision--)
        *str++ = '0';

    /* Invert buffer */
    while (0 < i--)
        *str++ = tmp[i];

    while (0 < size--)
        *str++ = ' ';

    return str;
}

int vsprintf(char* buf, const char* fmt, va_list args)
{
    int     *ip, loop, len, field_width, precision;
    char    *cp, *str = buf;
    char    base, flags = 0;
    size_t  val;

    while (*fmt) {

        if (*fmt != '%') {
            *str++ = *fmt++;
            continue;
        }

        repeat:
        switch (*++fmt) { /* Skip '%' */
            case '-': flags |= LEFT; goto repeat;
            case '+': flags |= PLUS; goto repeat;
            case ' ': flags |= SPACE; goto repeat;
            case '#': flags |= SPECIAL; goto repeat;
            case '0': flags |= ZEROPAD; goto repeat;
        }

        /* add zero pad */
        field_width = -1;
        if (is_digit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*') {
            ++fmt;  /* Skip '*' */
            field_width = va_arg(args, ssize_t);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        precision = -1;
        if (*fmt == '.') {
            ++fmt;  /* Skip '.' */
            if (is_digit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*')
                precision = va_arg(args, ssize_t);
            if (precision < 0)
                precision = 0;
        }

        /* These parameters are not supported */
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
            ++fmt;

        switch (*fmt) {
            case 'd':   /* Signed decimal output */
            case 'i':
                flags |= SIGN;
                base = 10;
                val = va_arg(args, ssize_t);
                goto is_integer;

            case 'u':   /* Unsigned decimal */
                base = 10;
                val = va_arg(args, size_t);
                goto is_integer;

            case 'x':   /* Hexadecimal */
                flags |= SMALL;
            case 'X':   /* Upper case hexadecimal */
                base = 16;
                val = va_arg(args, size_t);
                goto is_integer;

            case 'o':   /* Octal */
                base = 8;
                val = va_arg(args, size_t);
                goto is_integer;

            case 'p':   /* pointer */
                flags |= SMALL;
            case 'P':   /* Upper case pointer */
                flags |= SPECIAL;
                base = 16;
                val = (size_t)(uintptr_t)
                    va_arg(args, void *);

            is_integer: str = format_int(str, val, base,
                        field_width, precision, flags);
                break;

            /* string */
            case 's':
                cp = va_arg(args, char *);
                len = strlen(cp);

                if (precision < 0)
                    precision = len;
                else if (len > precision)
                    len = precision;

                if (!(flags & LEFT))
                    while (len < field_width--)
                        *str++ = ' ';

                for (loop = 0; loop < len; ++loop)
                    *str++ = *cp++;
                while (len < field_width--)
                    *str++ = ' ';
                break;

            /* Copy Character */
            case 'c':
                if (!(flags & LEFT))
                    while (--field_width > 0)
                        *str++ = ' ';
                *str++ = (unsigned char) va_arg(args, size_t);
                while (--field_width > 0)
                    *str++ = ' ';
                break;

            case '%':
                *str++ = '%';
                break;

            /* Length of string before feedback */
            case 'n':
                ip = va_arg(args, int *);
                *ip = (str - buf);
                break;

            /* illegal parameter */
            default:
                break;
        }
        ++fmt;  /* Skip parameters */
    }
    *str = '\0';

    /* Return conversion length */
    return str - buf;
}
